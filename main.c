/*
kiwi not emu for 6502
*/

#include "kiwi.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/mman.h>

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
	.end   = 0x01FF,
	.read  = ram_read,
	.write = ram_write,
};

void * rom_base_addr;
static void rom_write(u16 addr, u8 value)
{
	// actually disallow writes
	return;
	// u8 *p = rom_base_addr;
	// p[addr] = value;
}
static u8 rom_read(u16 addr)
{
	u8 *p = rom_base_addr;
	return p[addr];
}
struct bus_device rom = {
	.start = 0x0400,
	.end   = 0xFFFF,
	.read  = rom_read,
	.write = rom_write,
};
//=====================

int main()
{
	// emulator context
	struct kiwi_ctx *ctx;
	ctx = kiwi_create_ctx();
	kiwi_initialize_ctx(ctx);

	// configure rom
	int fd = open("6502_functional_test.bin", O_RDONLY);
	if(!fd) {
		fprintf(stderr, "error opening functional test file");
		exit(-1);
	}
	rom_base_addr = mmap(NULL, 0xFFFF, PROT_READ, MAP_PRIVATE, fd, 0);
	if(!fd) {
		fprintf(stderr, "error in mmap");
		exit(-1);
	}

	kiwi_attach_device(ctx, &ram);
	kiwi_attach_device(ctx, &rom);

	for(u32 i=0; i<0x400; i++) {
		ram_buf[i] = rom_read(i);
	}

	ctx->pc = 0x400;

	while(1) {
		kiwi_execute_opcode(ctx);
	}

	return 0;
}