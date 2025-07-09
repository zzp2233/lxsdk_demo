#include "include.h"
#include "func.h"

static void func_bt_update_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
}

void func_bt_update_message(u16 msg)
{
    func_message(msg);
}

static void func_bt_update_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_BT_UPDATE;
}

void func_bt_update_process(void)
{
    func_process();
}

AT(.text.func.bt_update)
void func_bt_update(void)
{
    printf("%s\n", __func__);

    func_bt_update_enter();

    while (func_cb.sta == FUNC_BT_UPDATE)
    {
        func_bt_update_process();
        func_bt_update_message(msg_dequeue());
    }

    func_bt_update_exit();
}
