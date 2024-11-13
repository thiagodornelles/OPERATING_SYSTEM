ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

;This routine is necessary to avoid code overwriting by BIOS BPB (BIOS Parameter Block)
_start:
  jmp SHORT start
  nop

times 33 db 0

start:
  ;Ensures that CS (code segment) is 0x7C0
  jmp 0:step2

step2:
  cli ;Clear interrupts
  
  ;Setting up manually these segments to make sure its correctly set
  mov ax, 0 
  mov ds, ax ;Set data segment
  mov es, ax ;Set extra segment

  ;Setting up stack segments
  mov ss, ax
  mov sp, 0x7C00

  sti ;Enables interrupts

load_protected:
  cli
  lgdt[gdt_descriptor]
  ;Setting the last bit of cr0 (???)
  mov eax, cr0
  or eax, 0x1
  mov cr0, eax
  jmp CODE_SEG:load32

;GDT
gdt_start:
gdt_null:
  dd 0x0
  dd 0x0

; offset 0x8
gdt_code: ; CS SHOULD POINT AT THIS
  dw 0xffff ; Segment limit first 0-15 bits
  dw 0 ; Base first 0-15 bits
  db 0 ; Base 16-23 bits
  db 0x9a ; Access byte
  db 11001111b ; High 4 bit flags and Low 4 bits flags
  db 0 ; Base 24-31 bits

; offset 0x10
gdt_data: ; DS, SS, ES, FS, GS
  dw 0xffff ; Segment limit first 0-15 bits
  dw 0 ; Base first 0-15 bits
  db 0 ; Base 16-23 bits
  db 0x92 ; Access byte
  db 11001111b ; High 4 bit flags and Low 4 bits flags
  db 0 ; Base 24-31 bits

gdt_end:

gdt_descriptor:
  dw gdt_end - gdt_start-1
  dd gdt_start

[BITS 32]
load32:
  mov eax, 1
  mov ecx, 100
  mov edi, 0x0100000
  call ata_lba_read
  jmp CODE_SEG:0x0100000

ata_lba_read:
  mov ebx, eax ;Backup LBA
  ;Send highest 8 bits of the lba to hard disk controller
  shr eax, 24
  or eax, 0xE0 ;select master drive
  mov dx, 0x1f6
  out dx, al
  ;Finished sending highest 8 bits of the lba
  ;Send the total sectors to read
  mov eax, ecx
  mov dx, 0x1f2
  out dx, al
  ;Finished total sectors to read
  ;Send more bit to LBA
  mov eax, ebx ; Restore LBA backup
  mov dx, 0x1f3
  out dx, al
  ;Finished send more bits to LBA  
  ;Send more bit to LBA
  mov dx, 0x1f4
  mov eax, ebx ; Restore LBA backup
  shr eax, 8
  out dx, al
  ;Finished send more bits to LBA  
  ;Send upper 16 bits to the LBA
  mov dx, 0x1f5
  mov eax, ebx ; Restore LBA backup
  shr eax, 16
  out dx, al
  ;Finished send upper 16 bits to the LBA
  mov dx, 0x1f7
  mov al, 0x20
  out dx, al

;Read all sector into memory
.next_sector:
  push ecx
;Checking if we need to read LBA again
.try_again:
  mov dx, 0x1f7
  in al, dx
  test al, 8
  jz .try_again
  ;We need to read 256 word at a time
  mov ecx, 256
  mov dx, 0x1f0
  rep insw ;read from 0x100000 address(edi param) 256 words(ecx param)
  pop ecx
  loop .next_sector
; End of reading sectors into memory
  ret

times 510-($ - $$) db 0
dw 0xAA55