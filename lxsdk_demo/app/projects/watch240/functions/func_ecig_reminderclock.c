#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "../../../../ute/module/smoke/ute_module_smoke.h"

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if ECIG_POWER_CONTROL

enum
{
    COMPO_ID_PIC_SMOCKING = 1,
    COMPO_ID_PIC_SMOCK_TARGET = 1,
};
typedef struct f_ecig_reminderclock_t_
{
    u32 tick;
} f_ecig_reminderclock_t;
// 创建窗体
compo_form_t *func_ecig_reminderclock_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);
    printf("1111111111111frm%d\n",frm);
    compo_picturebox_t *picbox;
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 20);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_CHILD_LOCK_ICON_84X84_X78_Y89_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 109);
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_CHILD_LOCK_ICON_LOCK_18X24_X111_Y35_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 35);

    txt = compo_textbox_create(frm, strlen(i18n[STR_CHILD_LOCK]));
    compo_textbox_set(txt, i18n[STR_CHILD_LOCK]);
    compo_textbox_set_autoroll_mode(txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    // compo_textbox_set_multiline(txt, true);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 80, 210, 80);
    // compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X + 80);

    return frm;
}

// 事件处理
static void func_ecig_reminderclock_process(void)
{
    f_ecig_reminderclock_t *f_ecig_reminderclock = (f_ecig_reminderclock_t *)func_cb.f_cb;
    if (tick_check_expire(f_ecig_reminderclock->tick, 1500))
    {
        func_directly_back_to();
    }
    func_process();

}

// 单击按钮
// static void func_ecig_reminder_button_click(void)
//{
//     int id = compo_get_button_id();

//    switch(id) {
//    case COMPO_ID_BTN_FLASHON:

//    break;

//    default:
//    break;

//}

// 消息处理
static void func_ecig_reminderclock_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            // func_ecig_reminder_button_click();
            //  func_cb.sta = FUNC_ACTIVITY;
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

// 进入控制功能
static void func_ecig_reminderclock_enter(void)
{

    func_cb.f_cb = func_zalloc(sizeof(f_ecig_reminderclock_t));
    func_cb.frm_main = func_ecig_reminderclock_form_create();
    f_ecig_reminderclock_t *f_ecig_reminderclock = (f_ecig_reminderclock_t *)func_cb.f_cb;
    f_ecig_reminderclock->tick = tick_get();
}

// 退出
static void func_ecig_reminderclock_exit(void)
{
    func_cb.last = FUNC_ECIG_REMINDERCLOCK;

}

// 功能
void func_ecig_reminderclock(void)
{
    printf("%s\n", __func__);
    func_ecig_reminderclock_enter();
    while (func_cb.sta == FUNC_ECIG_REMINDERCLOCK)
    {
        func_ecig_reminderclock_process();
        func_ecig_reminderclock_message(msg_dequeue());
    }
    func_ecig_reminderclock_exit();
}
#endif
