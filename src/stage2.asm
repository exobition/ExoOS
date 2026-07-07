[BITS 16]
[ORG 0x8000]

main:
  cli
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7c00

  mov dl, 0x00

  sti
  
  mov si, msg
  jmp printf

printf:
  lodsb
  test al, al
  jz LoadKernel
  mov ah, 0x0e
  int 0x10
  jmp printf

msg: db "Disk read successful, stage 2 loaded", 13, 10, 0

GDT_Start:
  NULL_SEGMENT:
    dq 0

  KERNEL_CODE_SEGMENT:
    dw 0xFFFF
    dw 0000
    db 00
    db 0x9A
    db 0xCF
    db 00

  KERNEL_DATA_SEGMENT:
    dw 0xFFFF
    dw 0000
    db 00
    db 0x92
    db 0xCF
    db 00

  USER_CODE_SEGMENT:
    dw 0xFFFF
    dw 0000
    db 00
    db 0xFA
    db 0xCF
    db 00

  USER_DATA_SEGMENT:
    dw 0xFFFF
    dw 0000
    db 00
    db 0xF2
    db 0xCF
    db 00
  SYSTEM_SEGMENT:
    dw 0x0067
    dw 0x0000
    db 0x00    
    db 0xE9
    db 0x00
    db 0x00

GDT_End:

GDT_Descriptor:
  dw GDT_End - GDT_Start - 1
  dd GDT_Start

LoadKernel:
  mov si, error_msg
  
  jmp diskload

diskload:
  mov ax, 0x1000
  mov es, ax

  xor bx, bx

  mov ah, 0x02
  mov al, 0x04
  mov ch, 0x00
  mov cl, 0x03
  mov dh, 0x00

  mov bx, 0x10000
  
  int 0x13

  jc .error
  
  xor ax, ax
  mov es, ax

  jmp LoadGDTforPM

.error:
  cli
  hlt ; sorry i need to fix smth so this is the most you're gonna get :P

LoadGDTforPM:
  cli
  
  in al, 0x92
  or al, 2
  out 0x92, al
  
  lgdt [GDT_Descriptor]
  mov eax, cr0
  or eax, 1
  mov cr0, eax
  jmp KERNEL_CODE_SEG:start_PM

[bits 32]
start_PM:
  mov ax, KERNEL_DATA_SEG
  mov ds, ax
  mov ss, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  
  mov esp, 0x90000
  mov ebp, esp
  
  jmp KERNEL_CODE_SEG:0x10000

KERNEL_CODE_SEG equ KERNEL_CODE_SEGMENT - GDT_Start
KERNEL_DATA_SEG equ KERNEL_DATA_SEGMENT - GDT_Start
USER_CODE_SEG   equ USER_CODE_SEGMENT - GDT_Start
USER_DATA_SEG   equ USER_DATA_SEGMENT - GDT_Start
SYSTEM_SEG equ SYSTEM_SEGMENT - GDT_Start

error_msg: db "Disk read error"

BOOT_DRIVE db 0

times 512-($-$$) db 0
