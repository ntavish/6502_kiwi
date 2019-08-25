#ifndef __KIWI_INSTRUCTIONS_H__
#define __KIWI_INSTRUCTIONS_H__

#include "kiwi.h"

/* All instructions return any 'extra' cycles to be counted.
   Followin * and ** notation is in datasheets for 6502.
   So if an instruction says 4* cycles, then function returns 0 or 1
   If function says 4**, then it returns 1 or 2 cycles.

   The last argument acc is for instruction that can act on either acc or 
   an address. If acc is true, result is written on acc
*/

u8 f_adc(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_and(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_asl(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_bcc(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_bcs(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_beq(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_bit(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_bmi(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_bne(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_bpl(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_brk(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_bvc(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_bvs(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_clc(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_cld(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_cli(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_clv(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_cmp(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_cpx(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_cpy(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_dec(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_dex(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_dey(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_eor(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_inc(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_inx(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_iny(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_jmp(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_jsr(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_lda(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_ldx(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_ldy(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_lsr(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_nop(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_ora(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_pha(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_php(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_pla(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_plp(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_rol(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_ror(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_rti(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_rts(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_sbc(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_sec(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_sed(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_sei(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_sta(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_stx(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_sty(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_tax(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_tay(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_tsx(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_txa(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_txs(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f_tya(struct kiwi_ctx *ctx, u16 addr, u8 acc);
u8 f__n_(struct kiwi_ctx *ctx, u16 addr, u8 acc);

#endif