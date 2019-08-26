#include "kiwi.h"

#define ACC 	ctx->regs.ac
#define X 	ctx->regs.x
#define Y 	ctx->regs.y
#define PC	ctx->pc
#define STATUS	ctx->regs.sr
#define SP	ctx->regs.sp

#define READ_BYTE(addr)			kiwi_read_byte(ctx, addr)
#define WRITE_BYTE(addr, value)		kiwi_write_byte(ctx, addr, value)

#define PUSH(value)		WRITE_BYTE(0x100 + ctx->regs.sp--, (value))
#define POP()			READ_BYTE(0x100 + ++ctx->regs.sp)

#define CARRY_VAL()	((ctx->regs.sr & SF_C)?1:0)
#define DECIMAL_VAL()	((ctx->regs.sr & SF_D)?1:0)
#define ZERO_VAL()	((ctx->regs.sr & SF_Z)?1:0)
#define NEGATIVE_VAL()	((ctx->regs.sr & SF_N)?1:0)
#define OVF_VAL()	((ctx->regs.sr & SF_O)?1:0)

#define CARRY_SET(x)	( (x) ? (ctx->regs.sr |= SF_C) : (ctx->regs.sr &= ~SF_C) )
#define OVF_SET(x)	( (x) ? (ctx->regs.sr |= SF_O) : (ctx->regs.sr &= ~SF_O) )
#define ZERO_SET(x)	( (x) ? (ctx->regs.sr |= SF_Z) : (ctx->regs.sr &= ~SF_Z) )
#define NEGATIVE_SET(x)	( (x) ? (ctx->regs.sr |= SF_N) : (ctx->regs.sr &= ~SF_N) )
#define INT_SET(x)	( (x) ? (ctx->regs.sr |= SF_I) : (ctx->regs.sr &= ~SF_I) )
#define DECIMAL_SET(x)	( (x) ? (ctx->regs.sr |= SF_D) : (ctx->regs.sr &= ~SF_D) )

u8 f_adc(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 operand = READ_BYTE(addr);
	u16 result = ACC + operand + CARRY_VAL();

	if(DECIMAL_VAL()) {
		if( (ACC & 0x0F) + (operand & 0x0F) + CARRY_VAL() > 9 ) {
			result += 6;
		}
		NEGATIVE_SET(result & 0x80);
		OVF_SET(((ACC ^ result) & (operand ^ result) & 0x80) != 0);
		if(result > 0x99) {
			result += 0x60;
		}
		CARRY_SET(result > 0x99);
	}
	else {
		NEGATIVE_SET(result & 0x80);
		CARRY_SET(result > 255);
		OVF_SET(((ACC ^ result) & (operand ^ result) & 0x80) != 0);
	}

	ACC = result & 0xFF;

	return 0;
}

u8 f_and(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 operand = READ_BYTE(addr);
	u8 result = operand & ACC;

	NEGATIVE_SET(result & 0x80);
	ZERO_SET(!result);
	ACC = result;

	return 0;
}

u8 f_asl(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u16 result;

	if(acc) {
		result = ACC << 1;
		CARRY_SET(result > 0xFF);
		ACC = result;
	}
	else {
		result = READ_BYTE(addr) << 1;
		CARRY_SET(result > 0xFF);
		WRITE_BYTE(addr, result & 0xFF);
	}

	NEGATIVE_SET(result & 0x80);
	ZERO_SET(!result);

	return 0;
}

u8 f_bcc(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(!CARRY_VAL()) {
		PC = addr;
	}
	return 0;
}

u8 f_bcs(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(CARRY_VAL()) {
		PC = addr;
	}
	return 0;
}

u8 f_beq(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(ZERO_VAL()) {
		PC = addr;
	}
	return 0;
}

u8 f_bit(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 operand = READ_BYTE(addr);
	u8 result = ACC & operand;

	// bit 7 and 6 (N and V/O) are copied from operand to status
	// zero bit is set if ACC & operand is zero
	NEGATIVE_SET(operand & 0x80);
	OVF_SET(operand & SF_O);
	ZERO_SET(!result);
	return 0;
}

u8 f_bmi(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(NEGATIVE_VAL()) {
		PC = addr;
	}
	return 0;
}

u8 f_bne(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(!ZERO_VAL()) {
		PC = addr;
	}
	return 0;
}

u8 f_bpl(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(!NEGATIVE_VAL()) {
		PC = addr;
	}
	return 0;
}

u8 f_brk(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	PUSH(PC >> 8);
	PUSH(PC & 0xFF);
	PUSH(STATUS | 0x30);
	INT_SET(1);
	PC = READ_BYTE(0xFFFE);
	PC += READ_BYTE(0xFFFF) << 8;
	return 0;
}

u8 f_bvc(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(!OVF_VAL()) {
		PC = addr;
	}
	return 0;
}

u8 f_bvs(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(OVF_VAL()) {
		PC = addr;
	}
	return 0;	
}

u8 f_clc(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	CARRY_SET(0);
	return 0;
}

u8 f_cld(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	DECIMAL_SET(0);
	return 0;
}

u8 f_cli(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	INT_SET(0);
	return 0;
}

u8 f_clv(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	OVF_SET(0);
	return 0;
}

u8 f_cmp(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 operand = READ_BYTE(addr);
	u16 result = ACC - operand;

	// https://www.atariarchives.org/alp/appendix_1.php
	// C is set if value is greater or equal to ACC
	CARRY_SET(operand <= ACC);
	NEGATIVE_SET(result & 0x80);
	ZERO_SET(!result);
	return 0;
}

u8 f_cpx(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 operand = READ_BYTE(addr);
	u16 result = X - operand;

	CARRY_SET(operand <= X);
	NEGATIVE_SET(result & 0x80);
	ZERO_SET(!result);
	return 0;
}

u8 f_cpy(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 operand = READ_BYTE(addr);
	u16 result = Y - operand;

	CARRY_SET(operand <= Y);
	NEGATIVE_SET(result & 0x80);
	ZERO_SET(!result);
	return 0;
}

u8 f_dec(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(acc) {
		ACC--;
		NEGATIVE_SET(ACC & 0x80);
		ZERO_SET(!ACC);
	}
	else {
		u8 result = READ_BYTE(addr) - 1;
		NEGATIVE_SET(result & 0x80);
		ZERO_SET(!result);
		WRITE_BYTE(addr, result);
	}
	return 0;
}

u8 f_dex(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	X--;
	NEGATIVE_SET(X & 0x80);
	ZERO_SET(!X);
	return 0;
}

u8 f_dey(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	Y--;
	NEGATIVE_SET(Y & 0x80);
	ZERO_SET(!Y);
	return 0;
}

u8 f_eor(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	ACC ^= READ_BYTE(addr);
	NEGATIVE_SET(ACC & 0x80);
	ZERO_SET(!ACC);
	return 0;
}

u8 f_inc(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 result = READ_BYTE(addr) + 1;
	NEGATIVE_SET(result & 0x80);
	ZERO_SET(!result);
	WRITE_BYTE(addr, result);
	return 0;
}

u8 f_inx(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	X++;
	NEGATIVE_SET(X & 0x80);
	ZERO_SET(!X);
	return 0;
}

u8 f_iny(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	Y++;
	NEGATIVE_SET(Y & 0x80);
	ZERO_SET(!Y);
	return 0;
}

u8 f_jmp(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	// TODO fix all functions with variable clock cycles
	PC = addr;
	return 0;
}

u8 f_jsr(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	PC--;
	PUSH(PC >> 8);
	PUSH(PC & 0xFF);
	PC = addr;
	return 0;
}

u8 f_lda(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	ACC = READ_BYTE(addr);
	NEGATIVE_SET(ACC & 0x80);
	ZERO_SET(!ACC);
	return 0;
}

u8 f_ldx(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	X = READ_BYTE(addr);
	NEGATIVE_SET(X & 0x80);
	ZERO_SET(!X);
	return 0;
}

u8 f_ldy(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	Y = READ_BYTE(addr);
	NEGATIVE_SET(Y & 0x80);
	ZERO_SET(!Y);
	return 0;
}

u8 f_lsr(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	uint8_t result = READ_BYTE(addr);

	CARRY_SET(result & 0x01);
	result >>= 1;
	NEGATIVE_SET(result & 0x80);
	ZERO_SET(!result);
	WRITE_BYTE(addr, result);
	return 0;
}

u8 f_nop(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	return 0;
}

u8 f_ora(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	ACC = READ_BYTE(addr);
	NEGATIVE_SET(ACC & 0x80);
	ZERO_SET(!ACC);
	return 0;
}

u8 f_pha(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	PUSH(ACC);
	return 0;
}

u8 f_php(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	PUSH(STATUS | 0x30);
	return 0;
}

u8 f_pla(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	ACC = POP();
	NEGATIVE_SET(ACC & 0x80);
	ZERO_SET(!ACC);
	return 0;
}

u8 f_plp(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	SP = POP();
	return 0;
}

u8 f_rol(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(acc) {
		u16 result = ACC;
		result <<= 1;
		if(CARRY_VAL()) {
			result |= 0x01;
		}
		CARRY_SET(result > 0xFF);
		result &= 0xFF;
		NEGATIVE_SET(result & 0x80);
		ZERO_SET(!result);
		ACC = result;
	}
	else {
		u16 result = READ_BYTE(addr);
		result <<= 1;
		if(CARRY_VAL()) {
			result |= 0x01;
		}
		CARRY_SET(result > 0xFF);
		result &= 0xFF;
		NEGATIVE_SET(result & 0x80);
		ZERO_SET(!result);
		WRITE_BYTE(addr, result);
	}
	return 0;
}

u8 f_ror(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	if(acc) {
		u16 result = ACC;
		if(CARRY_VAL()) {
			result |= 0x100;
		}
		CARRY_SET(result & 0x01);
		result >>= 1;
		result &= 0xFF;
		NEGATIVE_SET(result & 0x80);
		ZERO_SET(!result);
		ACC = result;
	}
	else {
		u16 result = READ_BYTE(addr);
		if(CARRY_VAL()) {
			result |= 0x100;
		}
		CARRY_SET(result & 0x01);
		result >>= 1;
		result &= 0xFF;
		NEGATIVE_SET(result & 0x80);
		ZERO_SET(!result);
		WRITE_BYTE(addr, result);
	}
	return 0;
}

u8 f_rti(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 lb, hb;

	STATUS = POP();
	lb = POP();
	hb = POP();
	PC = (hb << 8) | lb;

	return 0;
}

u8 f_rts(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 lb, hb;

	STATUS = POP();
	lb = POP();
	hb = POP();
	PC = ((hb << 8) | lb) + 1;

	return 0;
}

u8 f_sbc(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 operand = READ_BYTE(addr);
	u16 result = ACC - operand - (CARRY_VAL() ? 0 : 1);
	
	ZERO_SET(!result);
	WRITE_BYTE(addr, result);

	if(DECIMAL_VAL()) {
		if ( ((ACC & 0x0F) - (CARRY_VAL() ? 0 : 1)) < (operand & 0x0F)) {
			result -= 6;
		}
		if (result > 0x99) {
			result -= 0x60;
		}
	}
	CARRY_SET(result < 0x100);
	ACC = result & 0xFF;

	return 0;
}

u8 f_sec(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	CARRY_SET(1);
	return 0;
}

u8 f_sed(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	DECIMAL_SET(1);
	return 0;
}

u8 f_sei(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	INT_SET(1);
	return 0;
}

u8 f_sta(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	WRITE_BYTE(addr, ACC);
	return 0;
}

u8 f_stx(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	WRITE_BYTE(addr, X);
	return 0;
}

u8 f_sty(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	WRITE_BYTE(addr, Y);
	return 0;
}

u8 f_tax(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	X = ACC;
	NEGATIVE_SET(X & 0x80);
	ZERO_SET(!X);
	return 0;
}

u8 f_tay(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	Y = ACC;
	NEGATIVE_SET(Y & 0x80);
	ZERO_SET(!Y);
	return 0;
}

u8 f_tsx(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	X = SP;
	NEGATIVE_SET(X & 0x80);
	ZERO_SET(!X);
	return 0;
}

u8 f_txa(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 result = X;
	NEGATIVE_SET(result & 0x80);
	ZERO_SET(!result);
	ACC = result;
	return 0;
}

u8 f_txs(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	SP = X;
	return 0;
}

u8 f_tya(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	ACC = Y;
	NEGATIVE_SET(ACC & 0x80);
	ZERO_SET(!ACC);
	return 0;
}

u8 f__n_(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	return 0;
}
