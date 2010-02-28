
	.equ	CACHE_DSIZE,16384
	.equ	MINICACHE_DSIZE,512
	
	.text
	.code 32

/* Clean and invalidate the entire cache. */
/* void SA1110_flush_cache_all() */
	.global	SA1110_flush_cache_all
	.func	SA1110_flush_cache_all
SA1110_flush_cache_all:	
	mov	ip, #0
	mcr	p15, 0, ip, c7, c5, 0 /*  invalidate I cache */

	ldr	r3, =__cacheflush_start__
	add	r1, r3, #CACHE_DSIZE
l1:	ldr	r2, [r3, #32]!
	teq	r1, r3
	bne	l1
	mcr	p15, 0, r0, c7, c6, 0 /* invalidate D cache */

	ldr	r3, =__minicacheflush_start__
	add	r1, r3, #MINICACHE_DSIZE
l2:	ldr	r2, [r3, #32]!
	teq	r1, r3
	bne	l2
	mcr	p15, 0, r0, c7, c6, 0 /* invalidate D cache */

	mcr	p15, 0, r0, c7, c7, 0 /* invalidate I+D cache */
	
	mcr	p15, 0, ip, c7, c10, 4 /* drain write buffer */
	mov	pc, lr
	.size   SA1110_flush_cache_all, . - SA1110_flush_cache_all
	.endfunc


