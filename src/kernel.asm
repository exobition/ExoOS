[bits 32]
[org 0x10000]

section .text

main:
  mov esi, msg

  mov edi, 0xB8000
  mov edx, 0xB8000

  call printf
  
  mov al, 0x00 

  call keyboard

  jmp $

printf:
  lodsb
  test al, al
  jz return
  cmp al, 0x0A 
  je newline
  mov ah, 0x0F
  mov [edi], al
  mov ebx, 0x02
  add edi, ebx
  xor ebx, ebx
  jmp printf

return:

  ret
  
section .data
  msg: db "#########################", 10, "# Welcome to ExoOS v0.1 #", 10, "#########################", 0 

  VGANL equ 0xA0

times 512-($-$$) db 0
