[ORG 0x7c00]
[BITS 16]

jmp main 

bdb_oem: db 'MSWIN4.1'
bdb_bytes_per_sector: dw 512
bdb_sectors_per_cluster: db 1 
bdb_reserved_sectors: db 1 
bdb_fat_count: db 2
bdb_dir_entries_count: dw 0x0e0
bdb_total_sectors: 2880
bdb_media_descriptor_type: db 0x0F0
bdb_sectors_per_fat: dw 9
bdb_sectors_per_track: dw 18
bdb_heads: dw 2 
bdb_hidden_sectors: dd 0 
bdb_large_sector_count: dd 0 

ebr_drive_count: db 0 
db 0 
ebr_signiture: db 0x29 
ebr_volume_id: db 0x12, 0x34, 0x56, 0x78
ebr_volume_label: db 'Exo OS     '
ebr_system_id: db 'FAT12   '


msg: db "Type something idk: ", 0 

main:
  cli
  xor ax, ax
  mov ds, ax
  mov es, ax 
  mov ss, ax
  mov sp, 0x7c00 
  sti 

  mov si, msg
  jmp printf

printf:
  lodsb
  test al, al 
  jz getinput
  mov ah, 0x0e 
  int 0x10
  jmp printf

getinput:
  mov ah, 0x00 
  xor al, al 
  int 0x16
  mov ah, 0x0e 
  int 0x10 
  jmp getinput

times 510-($-$$) db 0 
  dw 0xaa55
