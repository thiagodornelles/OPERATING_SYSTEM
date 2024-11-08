ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

;This routine is necessary to avoid code overwriting by BIOS BPB (BIOS Parameter Block)
_start:
  JMP SHORT start
  NOP

times 33 db 0

start:
  ;Ensures that CS (code segment) is 0x7C0
  JMP 0:step2

step2:
  CLI ;Clear interrupts
  
  ;Setting up manually these segments to make sure its correctly set
  MOV ax, 0 
  MOV ds, ax ;Set data segment
  MOV es, ax ;Set extra segment

  ;Setting up stack segments
  MOV ss, ax
  MOV sp, 0x7C00

  STI ;Enables interrupts

load_protected:
  CLI
  LGDT[gdt_descriptor]
  ;Setting the last bit of cr0 (???)
  MOV eax, cr0
  OR eax, 0x1
  MOV cr0, eax
  JMP CODE_SEG:load32


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
  MOV ax, DATA_SEG
  MOV ds, ax
  MOV es, ax
  MOV fs, ax
  MOV gs, ax
  MOV ss, ax
  MOV ebp, 0x00200000
  MOV esp, ebp
  JMP $

times 510-($ - $$) db 0
dw 0xAA55