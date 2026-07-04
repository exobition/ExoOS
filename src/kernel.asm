
[bits 32]
[org 0x10000]

section .text

main:
  mov esi, msg

  mov edi, 0xB8000
  mov edx, 0xB8000
  
  call clearVGA 
  call printf

  jmp $

clearVGA:
  mov al, nullchar
  mov ah, VGAcolor
  cmp edi, VGAMAX
  je resetVGA
  mov [edi], al
  inc edi
  mov [edi], ah
  inc edi
  jmp clearVGA

resetVGA:
  mov edi, 0xB8000
  jmp return

printf:
  lodsb
  test al, al
  jz return
  cmp al, 0x0A 
  je DNSys 
  mov ah, VGAcolor
  mov [edi], al
  mov ebx, 0x02
  add edi, ebx
  xor ebx, ebx
  jmp printf

DNSys:
  add edx, VGANL
  cmp edi, edx
  ja DNSys 
  mov edi, edx
  jmp printf

return:

  ret
  
section .data
msg: db "#########################", 10, "# Welcome to ExoOS v0.1 #", 10, "#########################", 0 

  nullchar equ 0x20
  VGAcolor equ 0x0F

  VGANL equ 0xA0
  VGAMAX equ 0xB9000

times 2048-($-$$) db 0
