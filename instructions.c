#include "kiwi.h"

#define ACC ctx->regs.ac

#define CARRY_VAL()	((ctx->regs.sr & SF_C)?1:0)
#define DECIMEAL_VAL()	((ctx->regs.sr & SF_D)?1:0)

#define CARRY_SET(x)	( (x) ? (ctx->regs.sr |= SF_C) : (ctx->regs.sr &= ~SF_C) )
#define OVF_SET(x)	( (x) ? (ctx->regs.sr |= SF_O) : (ctx->regs.sr &= ~SF_O) )
#define ZERO_SET(x)	( (x) ? (ctx->regs.sr |= SF_Z) : (ctx->regs.sr &= ~SF_Z) )
#define NEGATIVE_SET(x)	( (x) ? (ctx->regs.sr |= SF_Z) : (ctx->regs.sr &= ~SF_Z) )

u8 f_adc(struct kiwi_ctx *ctx, u16 addr, u8 acc)
{
	u8 operand = kiwi_read_byte(ctx, addr);
	u16 result = ACC + operand + CARRY_VAL();

	if(DECIMEAL_VAL()) {
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
	u8 operand = kiwi_read_byte(ctx, addr);
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
		result = kiwi_read_byte(ctx, addr) << 1;
		CARRY_SET(result > 0xFF);
		kiwi_write_byte(ctx, addr, result & 0xFF);
	}

	NEGATIVE_SET(result & 0x80);
	ZERO_SET(!result);

	return 0;
}

u8 f_bcc(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_bcs(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_beq(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_bit(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_bmi(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_bne(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_bpl(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_brk(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_bvc(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_bvs(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_clc(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_cld(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_cli(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_clv(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_cmp(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_cpx(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_cpy(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_dec(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_dex(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_dey(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_eor(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_inc(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_inx(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_iny(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_jmp(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_jsr(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_lda(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_ldx(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_ldy(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_lsr(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_nop(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_ora(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_pha(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_php(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_pla(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_plp(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_rol(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_ror(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_rti(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_rts(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_sbc(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_sec(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_sed(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_sei(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_sta(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_stx(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_sty(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_tax(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_tay(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_tsx(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_txa(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_txs(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f_tya(struct kiwi_ctx *ctx, u16 addr)
{

}

u8 f__n_(struct kiwi_ctx *ctx, u16 addr)
{

}
