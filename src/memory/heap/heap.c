#include "heap.h"
#include "kernel.h"
#include "status.h"
#include "memory/memory.h"
#include <stdbool.h>

static int validate_heap_table(void *heap_begin, void *heap_end, struct heap_table *table)
{
    int res = 0;
    size_t table_size = (size_t)(heap_end - heap_begin);
    size_t total_blocks = table_size / PEACHOS_HEAP_BLOCK_SIZE;

    if (table->total != total_blocks)
    {
        res = -EINVARG;
        goto out;
    }

out:
    return res;
}

static bool heap_validate_alignment(void *ptr)
{
    return ((unsigned int)ptr % PEACHOS_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap *heap, void *heap_begin, void *heap_end, struct heap_table *table)
{
    int res = 0;
    if (!heap_validate_alignment(heap_begin) || !heap_validate_alignment(heap_end))
    {
        res = -EINVARG;
        goto out;
    }

    memset(heap, 0, sizeof(struct heap));
    heap->start_address = heap_begin;
    heap->table = table;

    res = validate_heap_table(heap_begin, heap_end, table);
    if (res < 0)
    {
        goto out;
    }

    // Initialize heap entries to free blocks
    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_ENTRY_FREE, table_size);

out:
    return res;
}

static uint32_t heap_align_value_to_upper(uint32_t val)
{
    if (val % PEACHOS_HEAP_BLOCK_SIZE == 0)
    {
        return val;
    }
    val = val - (val % PEACHOS_HEAP_BLOCK_SIZE);
    val += PEACHOS_HEAP_BLOCK_SIZE;
    return val;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0F;
}

int heap_get_start_block(struct heap *heap, uint32_t total_blocks)
{
    struct heap_table *table = heap->table;
    int bc = 0;
    int bs = -1;

    for (size_t i = 0; i < table->total; i++)
    {
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_ENTRY_FREE)
        {
            bc = 0;
            bs = -1;
            continue;
        }

        // If this is the first block
        if (bs == -1)
        {
            bs = i;
        }
        bc++;

        if (bc == total_blocks)
        {
            break;
        }
    }
    if (bs == -1)
    {
        return -ENOMEM;
    }
    return bs;
}

void heap_mark_blocks_as_taken(struct heap *heap, uint32_t start_block, uint32_t total_blocks)
{
    int end_block = (start_block + total_blocks) - 1;
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;

    if (total_blocks > 1)
    {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    for (int i = start_block; i <= end_block; i++)
    {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_ENTRY_TAKEN;
        if (i == end_block - 1)
        {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

void *heap_block_to_address(struct heap *heap, uint32_t block)
{
    return heap->start_address + (block * PEACHOS_HEAP_BLOCK_SIZE);
}

void *heap_malloc_blocks(struct heap *heap, uint32_t total_blocks)
{
    void *address = 0;
    int start_block = heap_get_start_block(heap, total_blocks);
    if (start_block < 0)
    {
        goto out;
    }
    address = heap_block_to_address(heap, start_block);
    // Mark blocks as taken
    heap_mark_blocks_as_taken(heap, start_block, total_blocks);

out:
    return address;
}

void *heap_malloc(struct heap *heap, size_t size)
{
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / PEACHOS_HEAP_BLOCK_SIZE;
    void *addr = heap_malloc_blocks(heap, total_blocks);
    return addr;
}

void heap_mark_blocks_as_free(struct heap *heap, uint32_t start_block)
{
    struct heap_table* table = heap->table;
    for (int i = start_block; i < table->total; i++)
    {
        HEAP_BLOCK_TABLE_ENTRY entry = table->entries[i];
        table->entries[i] = HEAP_BLOCK_ENTRY_FREE;        
        if (!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }

}

int heap_address_to_block(struct heap *heap, void* address)
{
    return (int)(address - heap->start_address) / PEACHOS_HEAP_BLOCK_SIZE;
}

void heap_free(struct heap *heap, void *ptr)
{
    heap_mark_blocks_as_free(heap, heap_address_to_block(heap, ptr));
}