[BITS 32]

global _start
global problem
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
  mov ax, DATA_SEG
  mov ds, ax
  mov es, ax  
  mov fs, ax
  mov gs, ax
  mov ss, ax
  mov ebp, 0x00200000
  mov esp, ebp

  ;Enable A20 
  in al, 0x92
  or al, 2
  out 0x92, al

  ;Remap the master PIC (Programmable Interrupt Controller)
  mov al, 00010001b ; Put PIC into initialization mode
  out 0x20, al ; Tell master PIC
  mov al, 0x20 ; Interrupt 0x20 is where master ISR should start
  out 0x21, al
  mov al, 00000001b 
  out 0x21, al
  ;End of remap of he master PIC

  sti
  call kernel_main
  jmp $


problem:
  int 0
  jmp $  

;Fix alignment issue with c compiler
times 512-($ - $$) db 0
