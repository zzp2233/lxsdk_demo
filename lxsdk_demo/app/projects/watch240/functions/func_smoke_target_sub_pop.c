#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#define TRACE_EN    1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if (USE_APP_TYPE == USE_AB_APP)
#define ALARM_RING_IDX()                app_data.alarm_ring
#define ALARM_RING_HOUR()               app_data.sector0.alarm[app_data.alarm_ring].hour
#define ALARM_RING_MIN()                app_data.sector0.alarm[app_data.alarm_ring].minute
#elif (USE_APP_TYPE == USE_UTE_APP)
#define ALARM_RING_IDX()                uteModuleSystemtimeGetAlarmRingIndex()
#define ALARM_RING_HOUR()               uteModuleSystemtimeGetAlarmHour(ALARM_RING_IDX())
#define ALARM_RING_MIN()                uteModuleSystemtimeGetAlarmMin(ALARM_RING_IDX())
#else
#define ALARM_RING_IDX()                0xff
#define ALARM_RING_HOUR()               0
#define ALARM_RING_MIN()                0
#endif

enum
{
    //图像id
    COMPO_ID_PIC_SMOKE = 1,
};

typedef struct f_alarm_clock_sub_pop_t_
{
    u32 tick;
    bool visible;
} f_alarm_clock_sub_pop_t;

//创建抽烟口数目标达成弹出窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_smoke_target_sub_pop_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I330001_SET_PUFFS_OK01_BIN);
    compo_setid(pic, COMPO_ID_PIC_SMOKE);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 40);

    //创建文本
    char str_buff[8];
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_DELETE_SUCCES]));///PASS
    compo_textbox_set(textbox, i18n[STR_DELETE_SUCCES]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+60);
    compo_textbox_set_forecolor(textbox, COLOR_WHITE);

    return frm;
}
// GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//抽烟口数目标达成弹出功能事件处理
static void func_smoke_target_sub_pop_process(void)
{
    f_alarm_clock_sub_pop_t *f_alarm_clock_sub_pop = (f_alarm_clock_sub_pop_t *)func_cb.f_cb;
    if (tick_check_expire(func_cb.enter_tick, 30 * 1000))    //30s无动作，退出
    {
        func_directly_back_to();
    }

    reset_sleep_delay_all();    //不休眠

    func_process();
}

//抽烟口数目标达成弹出功能消息处理
static void func_smoke_target_sub_pop_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_LONG:
        case KU_BACK:
            func_directly_back_to();
            break;

        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入抽烟口数目标达成弹出功能
static void func_smoke_target_sub_pop_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alarm_clock_sub_pop_t));
    func_cb.frm_main = func_smoke_target_sub_pop_form_create();

    f_alarm_clock_sub_pop_t *f_alarm_clock_sub_pop = (f_alarm_clock_sub_pop_t *)func_cb.f_cb;

}

//退出抽烟口数目标达成弹出功能
static void func_smoke_target_sub_pop_exit(void)
{
}

//抽烟口数目标达成弹出功能
void func_smoke_target_sub_pop(void)
{
    printf("%s\n", __func__);
    func_smoke_target_sub_pop_enter();
    while (func_cb.sta == FUNC_SMOKE_TARGET_SUB_POP)
    {
        func_smoke_target_sub_pop_process();
        func_smoke_target_sub_pop_message(msg_dequeue());
    }
    func_smoke_target_sub_pop_exit();
}
