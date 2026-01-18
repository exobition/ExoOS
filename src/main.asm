[ORG 0x7c00]
[BITS 16]

mov si, msg

jmp main 

msg: db "Type something idk :", 0 

main:
  mov sp, 0x7c00
  jmp printf

printf:
  lodsb
  mov ah, 0x0e 
  int 0x10 
  cmp al, 0 
  je getinput 
  jmp printf

getinput:
  mov ah, 0x00 
  xor al, al 
  int 0x16
  mov ah, 0x0e 
  int 0x10 
  jmp getinput

.halt:
  cli
  hlt 
  jmp .halt

jmp $

times 510-($-$$) db 0 
  dw 0xAA55
