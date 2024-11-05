ORG 0
BITS 16

;This routine is necessary to avoid code overwriting by BIOS BPB (BIOS Parameter Block)
_start:
  JMP SHORT start
  NOP

times 33 db 0

start:
  ;Ensures that CS (code segment) is 0x7C0
  JMP 0x7C0:step2

step2:
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

  ;Disc operation
  MOV ah, 2 ;READ SECTOR Command
  MOV al, 1 ;Read 1 sector
  MOV ch, 0 ;Cylinder low bits
  MOV cl, 2 ;Read sector number 2
  MOV dh, 0 ;Head number
  MOV bx, buffer
  INT 0x13 ;Invoke READ SECTOR COMMAND
  JC error ;When carry is set mean data could be not read
  MOV si, buffer ; prints disc data
  CALL print
  JMP $

error:
  MOV si, error_message
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

error_message: db 'Failed to load sector', 0

message: db 'Hello World!', 0
times 510-($ - $$) db 0
dw 0xAA55

buffer: