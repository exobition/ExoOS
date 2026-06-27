[BITS 16]
[ORG 0x8000]

main:
  cli
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7a0

  mov [BOOT_DRIVE], dl

  sti
  
  mov si, msg
  jmp printf

printf:
  lodsb
  test al, al
  jz .loop
  mov ah, 0x0e
  int 0x10
  jmp printf

.loop:
  cli
  hlt

msg: db "disk read successful, stage 2 loaded", 0

BOOT_DRIVE db 0

times 512-($-$$) db 0
