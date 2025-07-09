
#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_empty_t_
{
    u8 back_light_lvl;
} f_empty_t;

// 创建空白窗体
compo_form_t *func_empty_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}

// 空白窗口事件处理
static void func_empty_process(void)
{
    func_process();
}

static void func_empty_message(size_msg_t msg)
{
    switch (msg)
    {
        default:
            evt_message(msg);
            break;
    }
}

// 进入空白窗口
static void func_empty_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_empty_t));
    func_cb.frm_main = func_empty_form_create();
}

// 退出空白窗口
static void func_empty_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    task_stack_init();  //任务堆栈
    latest_task_init(); //最近任务
}

// 空白窗口
void func_empty(void)
{
    printf("%s\n", __func__);
    func_empty_enter();
    while (func_cb.sta == FUNC_EMPTY)
    {
        func_empty_process();
        func_empty_message(msg_dequeue());
    }
    func_empty_exit();
}
