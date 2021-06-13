/*
	kiwi not emu for 6502
*/

#include "kiwi.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>

/*=== devices list ===*/
/*=========RAM=========*/
static u8 ram_buf[0x400];
static void ram_write(u16 addr, u8 value)
{
	ram_buf[addr] = value;
}

static u8 ram_read(u16 addr)
{
	return ram_buf[addr];
}

static struct bus_device ram = {
	.start = 0x0000,
	.end   = 0x03ff,
	.read  = ram_read,
	.write = ram_write,
};

/*=========ROM=========*/

static void * rom_base_addr;
static struct stat finfo;
static void rom_write(u16 addr, u8 value)
{
	#if defined(DEBUG)
	printf("%s 0x%04X\n", __func__, addr);
	#endif
	u8 *p = rom_base_addr;
	if (addr < finfo.st_size) {
		p[addr] = value;
	}
}
static u8 rom_read(u16 addr)
{
	#if defined(DEBUG)
	printf("%s 0x%04X\n", __func__, addr);
	#endif
	u8 *p = rom_base_addr;
	if (addr < finfo.st_size) {
		return p[addr];
	} else {
		return 0xff;
	}
}
static struct bus_device rom = {
	.start = 0x0400,
	.end   = 0xFFFF,
	.read  = rom_read,
	.write = rom_write,
};

/*=========VECTORS=======*/
#if 0
static uint8_t vec_buf[] = {
		/* NMI */
		0x00, 0x04,
		/* RST */
		0x00, 0x04,
		/* IRQ */
		0x00, 0x04
};
static void vec_write(u16 addr, u8 value)
{
	/* disallow writes */
	return;
}
static u8 vec_read(u16 addr)
{
	return vec_buf[addr - 0xFFFA];
}
static struct bus_device vec = {
	.start = 0xFFFA,
	.end   = 0xFFFF,
	.read  = vec_read,
	.write = vec_write,
};
#endif
/*=====================*/
/*=====================*/

int main(int argc, char **argv)
{
	/* emulator context */
	struct kiwi_ctx *ctx;
	ctx = kiwi_create_ctx();
	kiwi_initialize_ctx(ctx);

	if (argc < 2)
	{
		printf("need ROM file as argument\n");
		return -1;
	}

	/* configure rom */
	int fd = open(argv[1], O_RDONLY);
	if (!fd) {
		fprintf(stderr, "error opening functional test file");
		exit(-1);
	}
	
	fstat(fd, &finfo);
	printf("size: %lu\n", finfo.st_size);
	
	rom_base_addr = mmap(NULL, 0xFFFF, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (!fd) {
		fprintf(stderr, "error in mmap");
		exit(-1);
	}

	kiwi_attach_device(ctx, &ram);
	kiwi_attach_device(ctx, &rom);
	/* kiwi_attach_device(ctx, &vec); */

	for(u32 i=0; i<sizeof(ram_buf); i++) {
		ram_buf[i] = rom_read(i);
	}

	ctx->pc = 0x400;

	while(1) {
		kiwi_execute_opcode(ctx);
	}

	return 0;
}
