[BITS 16]
[ORG 0x8000]

main:
  cli
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7e10
  sti
  
  mov si, msg
  jmp printf

printf:
  lodsb
  test al, al
  jz $
  mov ah, 0x0e
  int 0x10
  jmp printf

msg: db "disk read successful, stage 2 loaded", 0

times 512-($-$$) db 0
