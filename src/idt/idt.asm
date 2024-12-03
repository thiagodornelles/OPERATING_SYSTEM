section .asm

global idt_load
idt_load:
  push ebp
  mov ebp, esp
  
  ; ebp+4 Would get the return of the functio, ebp+8 gets the first argument of the function
  mov ebx, [ebp+8]
  lidt [ebx]
  
  pop ebp
  ret