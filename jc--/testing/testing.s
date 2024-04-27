.section .rodata
.LC0:
  .string "odachi = big fat blade \n"
  .LC0_LEN = . - .LC0

.text
.globl foo
.type foo, @function
foo:
.LFB0:
  endbr32 
  pushq %rbp
  movq %rsp, %rbp
  movl %edi, -0x04(%rbp)  # Adjusted offset for 64-bit
  movl -0x04(%rbp), %eax
  addl $1, %eax
  popq %rbp
  ret
.LFE0:

.globl print
.type print, @function
print:
.LFB1:
  endbr32
  pushq %rbp                # Set up stack frame
  movq %rsp, %rbp
  movl $0x04, %eax             # System call number for write
  movl $0x01, %ebx             # File descriptor for stdout
  movl $.LC0, %ecx          # Address of the local constant to print
  movl $.LC0_LEN, %edx      #length of the string
  int $0x80
  pop %rbp
  ret
.LFE1:

.globl main
.type main, @function
main:
.LFB2:
  endbr32 
  pushq %rbp
  movq %rsp, %rbp
  call print
  movl $0x02, %edi  # passing argument to foo
  call foo
  popq %rbp
  ret
.LFE2:





