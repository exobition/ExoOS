section .text
extern main

jmpToKernel:
  call main
  jmp $

section .data
