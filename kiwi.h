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
		6 O : overflow (1=true), sometimes written as V also
		5 - : ignored (1 always?)
		4 B : break (1=brk)
		3 D : decimal mode (1=true)
		2 I : interrupt (1=disable)
		1 Z : zero (1=result zero)
		0 C : carry (1=true)
*/

#define SF_C ((u8)(1 << 0))
#define SF_Z ((u8)(1 << 1))
#define SF_I ((u8)(1 << 2))
#define SF_D ((u8)(1 << 3))
#define SF_B ((u8)(1 << 4))
#define SF_U ((u8)(1 << 5))
#define SF_O ((u8)(1 << 6))
#define SF_N ((u8)(1 << 7))

struct bus_device {
	u16 start;
	u16 end;
	u8 (*read)(u16 addr);
	void (*write)(u16 addr, u8 val);
};

/*
	= general notes about 6502 processor
	
	16-bit words are little endian (least significant byte first)

	signed numbers are two's complement, sign is stored in msb (7th bit)

	stack: LIFO, top down, 8-bit range, hardcoded range between 0x1FF to 0x100
	e.g. at init you will set sp to 0xFF (i.e. address 0x1FF), and it goes down

	Devices are memory locations that can be read or written to.
*/

#define MAX_DEVICES	10

struct kiwi_ctx {
	struct regs_6502 regs; 	// 6502 registers
	u16 pc;					// program counter
	struct bus_device dev[MAX_DEVICES];	// list of devices on bus
	u8 num_devices;				// number of valid devices in dev
};

/*
	allocates, then initilizes a kiwi context
	returns pointer to allocated and initialized ctx
	return 0 on failure
*/
struct kiwi_ctx* kiwi_create_ctx();

/*
	initializes the ctx argument by resetting the cpu state
*/
struct kiwi_ctx* kiwi_initialize_ctx(struct kiwi_ctx *ctx);

/*
	resets the cpu state
*/
void kiwi_reset_cpu(struct kiwi_ctx *ctx);

/*
	executes one opcode, returns number of cycles taken by instruction
*/
u8 kiwi_execute_opcode(struct kiwi_ctx *ctx);


/*
	adds dev to internal list of devices for ctx, returns number of devices
	registered
*/
u8 kiwi_attach_device(struct kiwi_ctx *ctx, struct bus_device *dev);

/*
	reads a byte from the correct 'device'
*/
u8 kiwi_read_byte(struct kiwi_ctx *ctx, u16 addr);

/*
	writes a byte to the correct 'device'
*/
void kiwi_write_byte(struct kiwi_ctx *ctx, u16 addr, u8 value);

#endif
