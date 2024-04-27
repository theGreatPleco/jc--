	.file	"var.c"
	.text
.Ltext0:
	.file 0 "/home/hosokawa/projects/compiler/testing" "var.c"
	.section	.rodata
.LC0:
	.string	"hello"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.file 1 "var.c"
	.loc 1 3 11
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	.loc 1 4 3
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	.loc 1 5 10
	movl	$0, %eax
	.loc 1 6 1
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
.Letext0:
	.section	.debug_info,"",@progbits
