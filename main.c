/*
kiwi not emu for 6502
*/

#include "kiwi.h"
#include <stdlib.h>


//=== devices list ====

// ram mirrors every 0x800 bytes
static u8 ram_buf[0x800];
static void ram_write(u16 addr, u8 value)
{
	ram_buf[addr % 0x800] = value;
}

static u8 ram_read(u16 addr)
{
	return ram_buf[addr % 0x800];
}

struct bus_device ram = {
	.start = 0x0000,
	.end   = 0x1FFF,
	.read  = ram_read,
	.write = ram_write,
};

int main()
{
	// emulator context
	struct kiwi_ctx *ctx;
	ctx = kiwi_create_ctx();
	kiwi_initialize_ctx(ctx);

	kiwi_attach_device(ctx, &ram);

	for(u32 i=0; i<0x800; i++) {
		ram_buf[i] = random();
	}

	while(1) {
		kiwi_execute_opcode(ctx);
	}

	return 0;
}