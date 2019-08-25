/*
kiwi not emu for 6502
*/

#include "kiwi.h"

int main()
{
	// emulator context
	struct kiwi_ctx *ctx;
	ctx = kiwi_create_ctx();
	kiwi_reset_cpu(ctx);

	while(1) {
		kiwi_execute_opcode(ctx);
	}

	return 0;
}