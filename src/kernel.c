#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"

uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char color)
{
    return (color << 8) | c;
}

void terminal_put_char(int x, int y, char c, char color)
{
    video_mem[y * VGA_WIDTH + x] = terminal_make_char(c, color);
}

void terminal_write_char(char c, char color)
{
    if (c == '\n')
    {
        terminal_col = 0;
        terminal_row++;
        return;
    }

    terminal_put_char(terminal_col, terminal_row, c, color);
    terminal_col++;

    if (terminal_col > VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row++;
    }
}

void terminal_init()
{
    video_mem = (uint16_t *)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_put_char(x, y, ' ', 0);
        }
    }
}

size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len])
    {
        len++;
    }
    return len;
}

void getDecStr(uint8_t *str, uint8_t len, uint32_t val)
{
    uint8_t i;
    for (i = 1; i <= len; i++)
    {
        str[len - i] = (uint8_t)((val % 10UL) + '0');
        val /= 10;
    }
    str[i - 1] = '\0';
}

void print_ptr_addr(void *ptr)
{
    char buffer[32];
    getDecStr((uint8_t *)buffer, 32, (uint32_t)ptr);
    int leading_zeros = 0;
    for (int i = 0; i < 32; i++)
    {
        if(leading_zeros > 0)
        {
            terminal_write_char(buffer[i], 15);
        }
        else if (leading_zeros == 0 && buffer[i] == '0')
        {
            continue;
        }
        else if (leading_zeros == 0 && buffer[i] != '0')
        {
            leading_zeros = 1;
            terminal_write_char(buffer[i], 15);
        }
    }
    terminal_write_char('\n', 15);
}

void print(const char *str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        terminal_write_char(str[i], 15);
    }
}

extern void problem();

void kernel_main()
{
    terminal_init();
    print("PEACH OS INIT ROUTINE\n");

    // Initialize KERNEL HEAP
    kheap_init();
    // Initialize INTERRUPT DESCRIPTOR TABLE
    idt_init();
    // Test exception
    // problem();
    // Test interrupt
    // outb(0x60, 0xff);
    void *ptr = kheap_malloc(50);
    void *ptr2 = kheap_malloc(5000);
    void *ptr3 = kheap_malloc(5000);
    kfree(ptr);
    void *ptr4 = kheap_malloc(50);

    if (ptr || ptr2)
    {
        print_ptr_addr(ptr);
        print_ptr_addr(ptr2);
        print_ptr_addr(ptr3);
        print_ptr_addr(ptr4);
        print("Heap allocated\n");        
    }
    else
    {
        print("Heap allocation failed\n");
    }
}