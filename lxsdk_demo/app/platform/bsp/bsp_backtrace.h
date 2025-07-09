#ifndef __BSP_BACKTRACE_H
#define __BSP_BACKTRACE_H


#define OS_BACKTRACE_LEVEL_MAX_NR 32
/**
 * Hardware Layer Backtrace Service
 */
typedef struct _hw_backtrace_frame
{
    os_base_t fp;
    os_base_t pc;
} os_hw_backtrace_frame_t;



os_err_t os_backtrace(void);

os_err_t os_backtrace_frame(os_hw_backtrace_frame_t *frame);

os_err_t os_backtrace_thread(os_thread_t thread);



#endif


