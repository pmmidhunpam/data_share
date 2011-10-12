	.file	"test.c"
	.text
.globl fun
	.type	fun, @function
fun:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	movl	$888, -4(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	leave
	ret
	.size	fun, .-fun
.globl main
	.type	main, @function
main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ecx
	subl	$100000000, %esp
	movl	$0, %eax
	addl	$100000000, %esp
	popl	%ecx
	popl	%ebp
	leal	-4(%ecx), %esp
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.2.4 (Ubuntu 4.2.4-1ubuntu4)"
	.section	.note.GNU-stack,"",@progbits
