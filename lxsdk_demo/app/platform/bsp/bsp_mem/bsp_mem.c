#include "include.h"

typedef struct
{
    u32 free_size;
    u32 used_size;
} mem_monitor_t;

static void mem_walker(void * ptr, size_t size, int used, void * user)
{
    mem_monitor_t *mon = (mem_monitor_t *)user;
    if (used)
    {
        mon->used_size += size;
    }
    else
    {
        mon->free_size += size;
    }
}

void mem_monitor_run(void)
{
    if (sys_tlsf == NULL)
    {
        printf("%s sys_tlsf=NULL\n", __func__);
        return;
    }

    mem_monitor_t mon = {0};
    tlsf_walk_pool(tlsf_get_pool(sys_tlsf), mem_walker, &mon);
    printf("free: %d, used: %d\n", mon.free_size, mon.used_size);
}
