.bss
.section .rodata
.LC0:
  .string "hello world"
  .text
  .globl print 
  .type print, @function 
print:
.LFB0:
  endbr64
  pushq %rbp
  movl $.LC0, %edi
  movl $0, %eax
  call printf@PLT
  popq %rbp 
.LFE0:

.globl main 
.type main, @function
main:
.LFB1:
  endbr64 
  pushq %rbp
  call print 
  popq %rbp 
  mov $200, %eax
  ret 
.LFE1:
