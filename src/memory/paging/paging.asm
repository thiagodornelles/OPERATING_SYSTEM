[BITS 32]

section .asm

global paging_load_directory
global enable_paging

paging_load_directory:
  push ebp
  mov ebp, esp
  mov eax, [ebp+8]
  ; this is important
  mov cr3, eax
  pop ebp
  ret

enable_paging:
  push ebp
  mov ebp, esp
  ; cannot change cr0 directly so moving to eax
  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax
  pop ebp
  ret