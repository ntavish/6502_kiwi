/*
kiwi implementation
*/

#include "kiwi.h"
#include "instructions.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
	see: https://www.masswerk.at/6502/6502_instruction_set.html
	we have four tables to describe each function:
	
	- Address mode table (also tells size)
	- Function to invoke
	- String for instruction/opcode
	- Clock cycles for instruction
*/

enum opcode_mode_types{
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

// not a useful table
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
	ADC, AND, ASL,
	BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS,
	CLC, CLD, CLI, CLV, CMP, CPX, CPY,
	DEC, DEX, DEY,
	EOR,
	INC, INX, INY,
	JMP, JSR,
	LDA, LDX, LDY, LSR,
	NOP,
	ORA,
	PHA, PHP, PLA, PLP,
	ROL, ROR, RTI, RTS,
	SBC, SEC, SED, SEI, STA, STX, STY,
	TAX, TAY, TSX, TXA, TXS, TYA,
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

u8 opcode_to_cycles[256] = {
/*      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/* 0 */ 7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
/* 1 */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 2 */ 6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
/* 3 */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 4 */ 6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
/* 5 */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 6 */ 6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
/* 7 */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* 8 */ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
/* 9 */ 2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
/* A */ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
/* B */ 2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
/* C */ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
/* D */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
/* E */ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
/* F */ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

static u8 (*instruction_to_function[])(struct kiwi_ctx *ctx, u16 addr, u8 acc) = {
	f_adc, f_and, f_asl,
	f_bcc, f_bcs, f_beq, f_bit, f_bmi, f_bne, f_bpl, f_brk, f_bvc, f_bvs,
	f_clc, f_cld, f_cli, f_clv, f_cmp, f_cpx, f_cpy,
	f_dec, f_dex, f_dey,
	f_eor,
	f_inc, f_inx, f_iny,
	f_jmp, f_jsr,
	f_lda, f_ldx, f_ldy, f_lsr,
	f_nop,
	f_ora,
	f_pha, f_php, f_pla, f_plp,
	f_rol, f_ror, f_rti, f_rts,
	f_sbc, f_sec, f_sed, f_sei, f_sta, f_stx, f_sty,
	f_tax, f_tay, f_tsx, f_txa, f_txs, f_tya,
	f__n_,
};

static inline struct bus_device *kiwi_find_mapped_dev(struct kiwi_ctx *ctx, u16 addr)
{
	struct bus_device *dev = 0;

	for(u8 i=0; i<ctx->num_devices; i++) {
		dev = &ctx->dev[i];
		if(addr >= dev->start && addr <= dev->end) {
			return dev;
		}
	}

	return 0;
}

u8 kiwi_read_byte(struct kiwi_ctx *ctx, u16 addr)
{
	struct bus_device *dev = kiwi_find_mapped_dev(ctx, addr);

	if(dev) {
		return dev->read(addr);
	}
	return 0;
}

void kiwi_write_byte(struct kiwi_ctx *ctx, u16 addr, u8 value)
{
	struct bus_device *dev = kiwi_find_mapped_dev(ctx, addr);

	if(dev) {
		return dev->write(addr, value);
	}
}

/*
	This function writes to addr, the calculated address depednding on opcode
	and following bytes (if any), and returns true is operand is to be ACC register.
	
	mode - see opcode_mode_types
*/
static u8 kiwi_6502_address_mode(struct kiwi_ctx *ctx, u8 mode, u16 *addr)
{
	u8 hb, lb;
	u8 is_acc = 0;

	switch(mode) {
		case MD_ACCUM:
			return 1;
			break;
		case MD_ABSOL:
			lb = kiwi_read_byte(ctx, ctx->pc++);
			hb = kiwi_read_byte(ctx, ctx->pc++);
			*addr = (hb << 8) | lb;
			break;
		case MD_ABS_X:
			lb = kiwi_read_byte(ctx, ctx->pc++);
			hb = kiwi_read_byte(ctx, ctx->pc++);
			*addr = ((hb << 8) | lb) + ctx->regs.x;
			break;
		case MD_ABS_Y:
			lb = kiwi_read_byte(ctx, ctx->pc++);
			hb = kiwi_read_byte(ctx, ctx->pc++);
			*addr = ((hb << 8) | lb) + ctx->regs.y;
			break;
		case MD_IMMED:
			*addr = ctx->pc++;
			break;
		case MD_IMPLI:
			//nothing to do, it's implied
			break;
		case MD_INDIR:
			// JMP indirect 0x6C is the only instruction which uses this
			lb = kiwi_read_byte(ctx, ctx->pc++);
			hb = kiwi_read_byte(ctx, ctx->pc++);
			*addr = (hb << 8) | lb;
			lb = kiwi_read_byte(ctx, *addr);
			hb = kiwi_read_byte(ctx, *addr+1);
			*addr = (hb << 8) | lb;
			break;
		case MD_X_IND:
			// this one is weird, it will be like 
			// see https://github.com/spacerace/6502/blob/master/doc/6502-asm-doc/dr6502-docs/ADDRESS.DOC#L277
			lb = (kiwi_read_byte(ctx, ctx->pc++) + ctx->regs.x) & 0xFF;
			hb = (lb + 1) & 0xFF;
			*addr = kiwi_read_byte(ctx, lb) + (kiwi_read_byte(ctx, hb) << 8);
			break;
		case MD_IND_Y:
			lb = kiwi_read_byte(ctx, ctx->pc++);
			hb = (lb + 1) & 0xFF;
			*addr = kiwi_read_byte(ctx, lb) + (kiwi_read_byte(ctx, hb) << 8) + ctx->regs.y;
			break;
		case MD_RELAT:
			*addr = (u16)kiwi_read_byte(ctx, ctx->pc++);
			if (*addr & 0x80) {
				*addr |= 0xFF00;
			}
			*addr += ctx->pc;
			break;
		case MD_ZPAGE:
			*addr = (u16)kiwi_read_byte(ctx, ctx->pc++);
			break;
		case MD_ZPG_X:
			*addr = (u16)(kiwi_read_byte(ctx, ctx->pc++) + ctx->regs.x) & 0xFF;
			break;
		case MD_ZPG_Y:
			*addr = (u16)(kiwi_read_byte(ctx, ctx->pc++) + ctx->regs.y) & 0xFF;
			break;
		case MD_NOTVL:

			break;
	}

	return is_acc;
}

u8 kiwi_execute_opcode(struct kiwi_ctx *ctx)
{
	// get the opcode, and increment pc by 1
	u8 opcode = kiwi_read_byte(ctx, ctx->pc++);
	u8 mode = opcode_to_mode[opcode];
	
	// calculate address from mode
	u16 addr;
	u8 acc = kiwi_6502_address_mode(ctx, mode, &addr);

	// find out instruction function
	u8 (*func)(struct kiwi_ctx *ctx, u16 addr, u8 acc) = instruction_to_function[opcode_to_ins[opcode]];
	
	// execute instruction
	u8 extra_cycles = func(ctx, addr, acc);

	// other parameters
	u8 cycles = opcode_to_cycles[opcode] + extra_cycles;
	printf("opcode 0x%02x\tmode 0x%02x\tcycles %u\tPC->0x%x\tfunc %p\r\n", opcode, mode, cycles, ctx->pc, func);

	return cycles;
}

void kiwi_reset_cpu(struct kiwi_ctx *ctx)
{
	ctx->pc = 0;
	memset(&ctx->regs, 0, sizeof(ctx->regs));
}

struct kiwi_ctx* kiwi_create_ctx()
{
	struct kiwi_ctx * ctx;
	ctx = (struct kiwi_ctx*) malloc(sizeof(struct kiwi_ctx));
	
	if(!ctx) {
		return 0;
	}

	kiwi_initialize_ctx(ctx);

	return ctx;
}

void kiwi_initialize_ctx(struct kiwi_ctx *ctx)
{
	ctx->num_devices = 0;

	kiwi_reset_cpu(ctx);
}

u8 kiwi_attach_device(struct kiwi_ctx *ctx, struct bus_device *dev)
{
	if(ctx->num_devices >= MAX_DEVICES) {
		ctx->num_devices = MAX_DEVICES;
		return ctx->num_devices;
	}

	memcpy(&ctx->dev[ctx->num_devices], dev, sizeof(struct bus_device));

	ctx->num_devices++;

	return ctx->num_devices;
}