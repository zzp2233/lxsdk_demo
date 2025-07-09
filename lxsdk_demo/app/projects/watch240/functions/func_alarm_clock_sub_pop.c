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
    COMPO_ID_PIC_ALARM = 1,
};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
typedef struct f_alarm_clock_sub_pop_t_
{
    u32 tick;
    bool visible;
} f_alarm_clock_sub_pop_t;

//创建闹钟弹出窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_sub_pop_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I330001_THEME_1_ALARM_BIN);
    compo_setid(pic, COMPO_ID_PIC_ALARM);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 40);

    //创建文本
    char str_buff[8];
    compo_textbox_t *txt = compo_textbox_create(frm, 8);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 40);
    snprintf(str_buff, sizeof(str_buff), "%02d:%02d", ALARM_RING_HOUR(), ALARM_RING_MIN());
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(txt, str_buff);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
typedef struct f_alarm_clock_sub_pop_t_
{
    u32 tick;
    bool visible;
} f_alarm_clock_sub_pop_t;

//创建闹钟弹出窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_sub_pop_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //新建图像
    compo_picturebox_t *pic;
//    pic = compo_picturebox_create(frm, UI_BUF_I330001_THEME_1_ALARM_BIN);
//    compo_setid(pic, COMPO_ID_PIC_ALARM);
//    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 40);

    //创建文本
    char str_buff[8];
    compo_textbox_t *txt = compo_textbox_create(frm, 8);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 40);
    snprintf(str_buff, sizeof(str_buff), "%02d:%02d", ALARM_RING_HOUR(), ALARM_RING_MIN());
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(txt, str_buff);

    return frm;
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//闹钟弹出功能事件处理
static void func_alarm_clock_sub_pop_process(void)
{
    f_alarm_clock_sub_pop_t *f_alarm_clock_sub_pop = (f_alarm_clock_sub_pop_t *)func_cb.f_cb;

    if (tick_check_expire(f_alarm_clock_sub_pop->tick, 500))    //闪烁
    {
        f_alarm_clock_sub_pop->tick = tick_get();
        f_alarm_clock_sub_pop->visible ^= 1;
        compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_ALARM), f_alarm_clock_sub_pop->visible);
    }

    if (tick_check_expire(func_cb.enter_tick, 30 * 1000))    //30s无动作，退出
    {
        func_directly_back_to();
    }

    reset_sleep_delay_all();    //不休眠

    func_process();
}

//闹钟弹出功能消息处理
static void func_alarm_clock_sub_pop_message(size_msg_t msg)
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

//进入闹钟弹出功能
static void func_alarm_clock_sub_pop_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alarm_clock_sub_pop_t));
    func_cb.frm_main = func_alarm_clock_sub_pop_form_create();

    f_alarm_clock_sub_pop_t *f_alarm_clock_sub_pop = (f_alarm_clock_sub_pop_t *)func_cb.f_cb;
    f_alarm_clock_sub_pop->visible = true;
    f_alarm_clock_sub_pop->tick = tick_get();
    func_cb.enter_tick = tick_get();

    TRACE("alarm index:%d\n", ALARM_RING_IDX());
}

//退出闹钟弹出功能
static void func_alarm_clock_sub_pop_exit(void)
{
}

//闹钟弹出功能
void func_alarm_clock_sub_pop(void)
{
    printf("%s\n", __func__);
    func_alarm_clock_sub_pop_enter();
    while (func_cb.sta == FUNC_ALARM_CLOCK_SUB_POP)
    {
        func_alarm_clock_sub_pop_process();
        func_alarm_clock_sub_pop_message(msg_dequeue());
    }
    func_alarm_clock_sub_pop_exit();
}
