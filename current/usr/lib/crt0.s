	.text
	.code 32

	.global	_start
	.func	_start
_start:
	b	main
	mov	pc,lr
	
	.size   _start, . - _start
	.endfunc

