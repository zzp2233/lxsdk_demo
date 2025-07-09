#include "include.h"


#define OS_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))
#define DEADWORD                        0x23232323


extern u32 __comm_start, __comm_end;
extern u32 __code_start_stream, __code_end_all;
os_err_t os_backtrace_dump_stack(void);

AT(.com_rodata.exception)
const char dump_str[] = "\n<<< task: %s, stack: %p--%p, sp: %p >>>\n";
AT(.com_rodata.exception)
const char backtrace_str[] = "riscv32-elf-addr2line -e app.rv32 -a -f ";
AT(.com_rodata.exception)
const char num_str[] = "%08x ";
AT(.com_rodata.exception)
const char ln_str[] = "\n";


AT(.com_text.exception)
os_err_t os_backtrace_dump_thread_stack(os_thread_t thread)
{
    uint32_t start_addr = (uint32_t)thread->sp;
    uint32_t end_addr = (uint32_t)thread->stack_addr + thread->stack_size - 28;

    printk(dump_str, thread->name, thread->stack_addr, thread->stack_addr + thread->stack_size, thread->sp);
    printk(backtrace_str);

    //inverse search
    for(int i = 0; i < thread->stack_size/4; i++)
    {
        if ((end_addr - i * 4) < start_addr)
        {
            break;
        }
        uint32_t addr = *(uint32_t*)(end_addr - i * 4);
        bool is_valid = (((addr >= (uint32_t)(&__comm_start)) && (addr <= (uint32_t)(&__comm_end))) || ((addr >= (uint32_t)(&__code_start_stream))  && (addr <= (uint32_t)(&__code_end_all))));
        if(is_valid)
        {
            printk(num_str,  addr);
        }
    }

    printk(ln_str);
    printk(ln_str);

    for(int i = 0; i < thread->stack_size/4; i++)
    {
        if (((uint32_t)thread->sp + i * 4) > ((uint32_t)thread->stack_addr + thread->stack_size))
        {
            break;
        }
        if(i % 8 == 0)
        {
            printk(ln_str);
        }
        uint32_t addr = *(uint32_t*)((uint32_t)thread->sp + i * 4);
        printk(num_str,  addr);
    }

    printk(ln_str);
    printk(ln_str);

    return OS_EOK;
}

AT(.com_text.exception)
os_err_t os_backtrace_dump_stack(void)
{
    return os_backtrace_dump_thread_stack(os_thread_self());
}

AT(.com_text.exception)
void exception_isr_callback(void)
{
    os_backtrace_dump_stack();
}
