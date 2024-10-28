ORG 0x7C00
BITS 16

start:
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