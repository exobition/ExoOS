org 0x7c00 ; Offsetting to 31744 hexadecimal
bits 16 ; setting bits to 16 (for the compiler)

jmp main

msg: db "Detecting memory..." ; Message to print to the screen
endmsg:


main:
  mov bx, 0x000F ; set to page 0 and color 15 for white
  mov cx, 1 ; Setting to write 1 character
  xor dx, dx ; start at the top left of the screen
  mov ds, dx ; set ds to 0 to let us load the Message
  cld ; ensure direction flag is cleared

print: mov si, msg ; move the first letter into 0x7c02

char:
  mov ah, 2 ; move 2 into ah to set cursor
  int 0x10
  lodsb ; load string byte

  mov ah, 9 ; print character and color
  int 0x10

  inc dl ; advance cursor

  cmp dl, 80 ; wrap around the screen if needed
  jne skip
  xor dl, dl ; set cursor to top left
  inc dh ; obviously increment dh

  cmp dh, 25 ; wrap around bottom of screen if needed
  jne skip
  xor dh, dh ; set dh to zero

skip:
  cmp si, endmsg
  jne char
  jmp .done

.done:
  xor dx, dx ; All of this is just clearing
  xor ax, ax
  xor cx, cx
  xor dx, dx
  xor dh, dh
  xor dl, dl
  xor ds, ds
  xor si, si
  jmp detect_mem

detect_mem:
  clc ; clear carry flag
  int 0x12 ; detect up to a MiB of memory (usually 640KiB)
  jc .error ; checking for error
  mov dx, ax ; mov ax to a different general purpose register
  xor ax, ax ; clear ax
  jmp .loop

.error:
  cli
  hlt
  jmp .error

.loop:
  cli
  hlt
  jmp .loop

jmp $

times 510-($-$$) db 0 ; fill the empty space with 0
dw 0xAA55 ; what the legacy bios looks for to see if a disk is bootable
