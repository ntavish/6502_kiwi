/*
kiwi implementation
*/

#include "kiwi.h"
#include <stdlib.h>

/*
	see: https://www.masswerk.at/6502/6502_instruction_set.html
	we have four tables to describe each function:
	
	- Address mode table (also tells size)
	- Function to invoke
	- String for instruction/opcode
	- Clock cycles for instruction
*/

enum opcode_mode{
	MD_ACCUM, //0
	MD_ABSOL, //1
	MD_ABS_X, //2
	MD_ABS_Y, //3
	MD_IMMED, //4
	MD_IMPLI, //5
	MD_INDIR, //6
	MD_X_IND, //7
	MD_IND_Y, //8
	MD_RELAT, //9
	MD_ZPAGE, //10
	MD_ZPG_X, //11
	MD_ZPG_Y, //12
	MD_NOTVL // keep this at the end, invalid
};

u8 mode_to_size[] = {
	1,	// ACC
	3,	// ABS
	3,	// ABS_X
	3,	// ABS_Y
	2,	// IMM
	1,	// IMP
	3,	// IND
	2,	// X_IND
	2,	// IND_Y
	2,	// REL
	2,	// ZPG
	2,	// ZPG_X
	2,	// ZPG_Y
};

u8 opcode_to_mode[256] = {
/*   *///  0        1         2         3         4         5         6         7         8          9         A         B         C         D        E         F
/* 0 */	MD_IMPLI, MD_X_IND, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ZPAGE, MD_ZPAGE, MD_NOTVL, MD_IMPLI, MD_IMMED, MD_ACCUM, MD_NOTVL, MD_NOTVL, MD_ABSOL, MD_ABSOL, MD_NOTVL,
/* 1 */	MD_RELAT, MD_IND_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ZPG_X, MD_ZPG_X, MD_NOTVL, MD_IMPLI, MD_ABS_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ABS_X, MD_ABS_X, MD_NOTVL,
/* 2 */	MD_ABSOL, MD_X_IND, MD_NOTVL, MD_NOTVL, MD_ZPAGE, MD_ZPAGE, MD_ZPAGE, MD_NOTVL, MD_IMPLI, MD_IMMED, MD_ACCUM, MD_NOTVL, MD_ABSOL, MD_ABSOL, MD_ABSOL, MD_NOTVL,
/* 3 */	MD_RELAT, MD_IND_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ZPG_X, MD_ZPG_X, MD_NOTVL, MD_IMPLI, MD_ABS_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ABS_X, MD_ABS_X, MD_NOTVL,
/* 4 */	MD_IMPLI, MD_X_IND, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ZPAGE, MD_ZPAGE, MD_NOTVL, MD_IMPLI, MD_IMMED, MD_ACCUM, MD_NOTVL, MD_ABSOL, MD_ABSOL, MD_NOTVL, MD_NOTVL,
/* 5 */	MD_RELAT, MD_IND_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ZPG_X, MD_ZPG_X, MD_NOTVL, MD_IMPLI, MD_ABS_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ABS_X, MD_ABS_X, MD_NOTVL,
/* 6 */	MD_IMPLI, MD_X_IND, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ZPAGE, MD_ZPAGE, MD_NOTVL, MD_IMPLI, MD_IMMED, MD_ACCUM, MD_NOTVL, MD_INDIR, MD_ABSOL, MD_ABSOL, MD_NOTVL,
/* 7 */	MD_RELAT, MD_IND_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ZPG_X, MD_ZPG_X, MD_NOTVL, MD_IMPLI, MD_ABS_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ABS_X, MD_ABS_X, MD_NOTVL,
/* 8 */	MD_NOTVL, MD_X_IND, MD_NOTVL, MD_NOTVL, MD_ZPAGE, MD_ZPAGE, MD_ZPAGE, MD_NOTVL, MD_IMPLI, MD_NOTVL, MD_IMPLI, MD_NOTVL, MD_ABSOL, MD_ABSOL, MD_ABSOL, MD_NOTVL,
/* 9 */	MD_RELAT, MD_IND_Y, MD_NOTVL, MD_NOTVL, MD_ZPG_X, MD_ZPG_X, MD_ZPG_Y, MD_NOTVL, MD_IMPLI, MD_ABS_Y, MD_IMPLI, MD_NOTVL, MD_NOTVL, MD_ABS_X, MD_NOTVL, MD_NOTVL,
/* A */	MD_IMMED, MD_X_IND, MD_IMMED, MD_NOTVL, MD_ZPAGE, MD_ZPAGE, MD_ZPAGE, MD_NOTVL, MD_IMPLI, MD_IMMED, MD_IMPLI, MD_NOTVL, MD_ABSOL, MD_ABSOL, MD_ABSOL, MD_NOTVL,
/* B */	MD_RELAT, MD_IND_Y, MD_NOTVL, MD_NOTVL, MD_ZPG_X, MD_ZPG_X, MD_ZPG_Y, MD_NOTVL, MD_IMPLI, MD_ABS_Y, MD_IMPLI, MD_NOTVL, MD_ABS_X, MD_ABS_X, MD_ABS_Y, MD_NOTVL,
/* C */	MD_IMMED, MD_X_IND, MD_NOTVL, MD_NOTVL, MD_ZPAGE, MD_ZPAGE, MD_ZPAGE, MD_NOTVL, MD_IMPLI, MD_IMMED, MD_IMPLI, MD_NOTVL, MD_ABSOL, MD_ABSOL, MD_ABSOL, MD_NOTVL,
/* D */	MD_RELAT, MD_IND_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ZPG_X, MD_ZPG_X, MD_NOTVL, MD_IMPLI, MD_ABS_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ABS_X, MD_ABS_X, MD_NOTVL,
/* E */	MD_IMMED, MD_X_IND, MD_NOTVL, MD_NOTVL, MD_ZPAGE, MD_ZPAGE, MD_ZPAGE, MD_NOTVL, MD_IMPLI, MD_IMMED, MD_IMPLI, MD_NOTVL, MD_ABSOL, MD_ABSOL, MD_ABSOL, MD_NOTVL,
/* F */	MD_RELAT, MD_IND_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ZPG_X, MD_ZPG_X, MD_NOTVL, MD_IMPLI, MD_ABS_Y, MD_NOTVL, MD_NOTVL, MD_NOTVL, MD_ABS_X, MD_ABS_X, MD_NOTVL,
};

/*
	allocates and returns pointer to kiwi context
	return 0 on failure
*/
struct kiwi_ctx* kiwi_create_ctx()
{
	struct kiwi_ctx * ctx;
	ctx = (struct kiwi_ctx*) malloc(sizeof(struct kiwi_ctx));
	
	if(!ctx) {
		return 0;
	}

	u8 *addr = malloc(0xFFFF); // 16-bit address space
	ctx->memory = addr;

	if(!addr) {
		free(ctx);
		ctx = 0;
	}

	return ctx;
}