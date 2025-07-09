#include "ute_module_mem.h"
#include "ute_module_log.h"

#if UTE_MODULE_USER_MALLOC_SUPPORT 

typedef struct MemoryBlock
{
    size_t size;
    int free;
    struct MemoryBlock *next;
} MemoryBlock;

#define BLOCK_SIZE sizeof(MemoryBlock)
static char memory_pool[UTE_MODULE_MEM_POOL_SIZE] AT(.com_text.ute_mem);
static MemoryBlock *base = (MemoryBlock *)memory_pool;

// 初始化内存池
void uteModulePlatformMemoryInitPool(void)
{
    base->size = UTE_MODULE_MEM_POOL_SIZE - BLOCK_SIZE;
    base->free = 1;
    base->next = NULL;
}

// 自定义 malloc
void *uteModulePlatformMemoryAlloc(size_t size)
{
    MemoryBlock *block = base;
    while (block && !(block->free && block->size >= size))
    {
        block = block->next;
    }
    if (!block)
    {
        // 如果自定义的内存池无法分配内存，则调用系统 malloc
        void *ptr = ab_malloc(size);
        if (ptr) {
            UTE_MODULE_LOG(UTE_LOG_MEMORY_LVL,"%s,Using system malloc,size:%d ptr:0x%x",__func__,size,ptr);
        }
        return ptr;
    }
    if (block->size >= size + BLOCK_SIZE)
    {
        MemoryBlock *new_block = (MemoryBlock *)((char *)block + BLOCK_SIZE + size);
        new_block->size = block->size - size - BLOCK_SIZE;
        new_block->free = 1;
        new_block->next = block->next;
        block->next = new_block;
    }
    block->size = size;
    block->free = 0;
    UTE_MODULE_LOG(UTE_LOG_MEMORY_LVL,"%s,Alloc size:%d ptr:0x%x",__func__,size,block+1)
    return (block + 1);
}

void uteModulePlatformMemoryFree(void *ptr)
{
    if (!ptr)
    {
        return;
    }
    if (ptr >= (void *)memory_pool && ptr < (void *)(memory_pool + UTE_MODULE_MEM_POOL_SIZE))
    {
        // 释放自定义内存池中的内存
        MemoryBlock *block = (MemoryBlock *)ptr - 1;
        block->free = 1;
        UTE_MODULE_LOG(UTE_LOG_MEMORY_LVL,"%s,Free ptr:0x%x",__func__,ptr)
    }
    else
    {
        // 释放系统 malloc 分配的内存
        ab_free(ptr);
        UTE_MODULE_LOG(UTE_LOG_MEMORY_LVL,"%s,Freeing system ptr:0x%x",__func__,ptr)
    }
}

// 获取剩余可用内存
size_t uteModulePlatformMemoryGetFree(void)
{
    MemoryBlock *block = base;
    size_t free_memory = 0;
    while (block)
    {
        if (block->free)
        {
            free_memory += block->size;
        }
        block = block->next;
    }
    return free_memory;
}

#endif
