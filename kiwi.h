#ifndef __KIWI_H__
#define __KIWI_H__

#include "types.h"

/*
	= notes for struct regs_6502:

	sr: status bits (7 is msb):
		7 N : negative
		6 O : overflow
		5 - : ignored
		4 B : break
		3 D : decimal
		2 I : interrupt (IRQ disable)
		1 Z : zero
		0 C : carry
*/
struct regs_6502 {
	u8 ac;		// accumulator
	u8 x;		// x register
	u8 y;		// y register
	u8 sr;		// status register
	u8 sp;		// stack pointer
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

struct kiwi_ctx* kiwi_create_ctx();

#endif