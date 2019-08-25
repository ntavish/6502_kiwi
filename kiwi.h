#ifndef __KIWI_H__
#define __KIWI_H__

#include "types.h"


/*
	All the registers of the CPU, A, X, Y are similar.
	There is a stack pointer and also a status register to
	query or set modify some CPU states.
*/
struct regs_6502 {
	u8 ac;		// accumulator
	u8 x;		// x register
	u8 y;		// y register
	u8 sr;		// status register
	u8 sp;		// stack pointer
};

/*
	sr: status bits (7 is msb):
		7 N : negative (1=negative)
		6 O : overflow (1=true)
		5 - : ignored (1 always?)
		4 B : break (1=brk)
		3 D : decimal mode (1=true)
		2 I : interrupt (1=disable)
		1 Z : zero (1=result zero)
		0 C : carry (1=true)
*/

enum status_flag_bits {
	SF_C = (1 << 0),
	SF_Z = (1 << 1),
	SF_I = (1 << 2),
	SF_D = (1 << 3),
	SF_B = (1 << 4),
	SF_U = (1 << 5),
	SF_O = (1 << 6),
	SF_N = (1 << 7),
};

/*
	= general notes about 6502 processor
	
	16-bit words are little endian (least significant byte first)

	signed numbers are two's complement, sign is stored in msb (7th bit)

	stack: LIFO, top down, 8-bit range, hardcoded range between 0x1FF to 0x100
	e.g. at init you will set sp to 0xFF (i.e. address 0x1FF), and it goes down
*/

struct kiwi_ctx {
	struct regs_6502 regs; 	// 6502 registers
	u16 pc;					// program counter
	u8 *memory;				// address space (16-bit)
};

/*
	allocates and returns pointer to kiwi context
	return 0 on failure
*/
struct kiwi_ctx* kiwi_create_ctx();

/*
	resets the cpu state
*/
void kiwi_reset_cpu(struct kiwi_ctx *ctx);

/*
	executes one opcode
*/
void kiwi_execute_opcode(struct kiwi_ctx *ctx);

#endif
