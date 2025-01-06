#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

void kernel_main();
void print_ptr_addr(void* ptr);
void print(const char* str);

#endif