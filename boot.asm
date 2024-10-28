ORG 0
BITS 16

;Ensures that CS (code segment) is 0x7C0
JMP 0x7C0:start

start:
  CLI ;Clear interrupts
  
  ;Setting up manually these segments to make sure its correctly set
  MOV ax, 0x7C0 
  MOV ds, ax ;Set data segment
  MOV es, ax ;Set extra segment

  ;Setting up stack segments
  MOV ax, 0
  MOV ss, ax
  MOV sp, 0x7C00

  STI ;Enables interrupts

  MOV si, message
  CALL print
  JMP $

print:
  MOV bx, 0
.loop_string:
  LODSB
  CMP al, 0
  JE .done
  CALL print_char
  JMP .loop_string  
.done:
  RET

print_char:
  MOV ah, 0eh    
  INT 0x10
  RET

message: db 'Hello World!', 0
times 510-($ - $$) db 0
dw 0xAA55