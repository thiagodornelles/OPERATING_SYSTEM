section .asm

global insb
global insw
global outb
global outw

insb:
  push ebp
  mov ebp, esp
  
  ;setting zero to eax
  xor eax, eax
  ;reading c function parameter val
  mov edx, [ebp+8]
  ;receiving lower 8 bits into al register
  in al, dx

  pop ebp
  ;eax is the return value using c
  ret

insw:
  push ebp
  mov ebp, esp

  xor eax, eax
  mov edx, [ebp+8]
  ;receiving 16 bits into al register
  in ax, dx

  pop ebp
  ret

outb:
  push ebp
  mov ebp, esp
  
  mov eax, [ebp+12]
  mov edx, [ebp+8]
  out dx, al

  pop ebp
  ret

outw:
  push ebp
  mov ebp, esp
  
  mov eax, [ebp+12]
  mov edx, [ebp+8]
  out dx, ax

  pop ebp
  ret