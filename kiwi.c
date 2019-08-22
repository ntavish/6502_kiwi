/*
kiwi implementation
*/

#include "kiwi.h"
#include <stdlib.h>

/*
	allocates and returns pointer to kiwi context
	return 0 on failure
*/
struct kiwi_ctx* kiwi_create_ctx()
{
	struct kiwi_ctx * ctx;
	ctx = (struct kiwi_ctx*) malloc(sizeof(struct kiwi_ctx));
	
	if(!ctx) {
		return 0;
	}

	u8 *addr = malloc(0xFFFF); // 16-bit address space
	ctx->memory = addr;

	if(!addr) {
		free(ctx);
		ctx = 0;
	}

	return ctx;
}