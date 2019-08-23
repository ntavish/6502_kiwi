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
	MD_ACCUM,
	MD_ABSOL,
	MD_ABS_X,
	MD_ABS_Y,
	MD_IMMED,
	MD_IMPLI,
	MD_INDIR,
	MD_X_IND,
	MD_IND_Y,
	MD_RELAT,
	MD_ZPAGE,
	MD_ZPG_X,
	MD_ZPG_Y,
	MD_NOTVL,
};

u8 mode_to_size[] = {
	1, // MD_ACCUM
	3, // MD_ABSOL
	3, // MD_ABS_X
	3, // MD_ABS_Y
	2, // MD_IMMED
	1, // MD_IMPLI
	3, // MD_INDIR
	2, // MD_X_IND
	2, // MD_IND_Y
	2, // MD_RELAT
	2, // MD_ZPAGE
	2, // MD_ZPG_X
	2, // MD_ZPG_Y
	1, // MD_NOTVL
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

enum instructions_6502 {
	ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE,
	BPL, BRK, BVC, BVS, CLC, CLD, CLI, CLV, CMP,
	CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY,
	JMP, JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA,
	PHP, PLA, PLP, ROL, ROR, RTI, RTS, SBC, SEC,
	SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA,
	TXS, TYA,
	_N_, // invalid
};

u8 opcode_to_ins[256] = {
/*   */// 0   1     2    3   4    5    6    7    8    9    A    B    C    D    E    F
/* 0 */ BRK, ORA, _N_, _N_, _N_, ORA, ASL, _N_, PHP, ORA, ASL, _N_, _N_, ORA, ASL, _N_,
/* 1 */ BPL, ORA, _N_, _N_, _N_, ORA, ASL, _N_, CLC, ORA, _N_, _N_, _N_, ORA, ASL, _N_,
/* 2 */ JSR, AND, _N_, _N_, BIT, AND, ROL, _N_, PLP, AND, ROL, _N_, BIT, AND, ROL, _N_,
/* 3 */ BMI, AND, _N_, _N_, _N_, AND, ROL, _N_, SEC, AND, _N_, _N_, _N_, AND, ROL, _N_,
/* 4 */ RTI, EOR, _N_, _N_, _N_, EOR, LSR, _N_, PHA, EOR, LSR, _N_, JMP, EOR, LSR, _N_,
/* 5 */ BVC, EOR, _N_, _N_, _N_, EOR, LSR, _N_, CLI, EOR, _N_, _N_, _N_, EOR, LSR, _N_,
/* 6 */ RTS, ADC, _N_, _N_, _N_, ADC, ROR, _N_, PLA, ADC, ROR, _N_, JMP, ADC, ROR, _N_,
/* 7 */ BVS, ADC, _N_, _N_, _N_, ADC, ROR, _N_, SEI, ADC, _N_, _N_, _N_, ADC, ROR, _N_,
/* 8 */ _N_, STA, _N_, _N_, STY, STA, STX, _N_, DEY, _N_, TXA, _N_, STY, STA, STX, _N_,
/* 9 */ BCC, STA, _N_, _N_, STY, STA, STX, _N_, TYA, STA, TXS, _N_, _N_, STA, _N_, _N_,
/* A */ LDY, LDA, LDX, _N_, LDY, LDA, LDX, _N_, TAY, LDA, TAX, _N_, LDY, LDA, LDX, _N_,
/* B */ BCS, LDA, _N_, _N_, LDY, LDA, LDX, _N_, CLV, LDA, TSX, _N_, LDY, LDA, LDX, _N_,
/* C */ CPY, CMP, _N_, _N_, CPY, CMP, DEC, _N_, INY, CMP, DEX, _N_, CPY, CMP, DEC, _N_,
/* D */ BNE, CMP, _N_, _N_, _N_, CMP, DEC, _N_, CLD, CMP, _N_, _N_, _N_, CMP, DEC, _N_,
/* E */ CPX, SBC, _N_, _N_, CPX, SBC, INC, _N_, INX, SBC, NOP, _N_, CPX, SBC, INC, _N_,
/* F */ BEQ, SBC, _N_, _N_, _N_, SBC, INC, _N_, SED, SBC, _N_, _N_, _N_, SBC, INC, _N_,
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