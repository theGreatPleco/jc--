# int main()
# {
#  int a = 200;
#  return a;
# }
.text
.globl main
.type main, @function 

main:
LFB0:
  endbr32
  pushq %rbp
  movq %rsp, %rbp
  movl $200, -4(%rbp)
  movl -4(%rbp), %eax 
  popq %rbp
  ret  
LFE0:

