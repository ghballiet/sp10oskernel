	.text
	.code 32

	.global	syscall
	.func	syscall
syscall:
	swi	#0
	mov	pc,lr
	
	.size   syscall, . -syscall
	.endfunc

