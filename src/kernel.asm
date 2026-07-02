[bits 32]
[org 0x10000]

main:
  mov al, 'A'
  mov ah, 0x0F
  mov [0xB8000], ax

  jmp $

times 512-($-$$) db 0
