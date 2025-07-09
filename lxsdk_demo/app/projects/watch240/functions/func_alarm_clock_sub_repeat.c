#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if (USE_APP_TYPE == USE_AB_APP)
#define ALARM_EDIT(...)                 alarm_clock_edit(__VA_ARGS__)
#define ALARM_SET(...)                  alarm_clock_set(__VA_ARGS__)
#define ALARM_GET_HOUR(idx)             app_data.sector0.alarm[idx].hour
#define ALARM_GET_MIN(idx)              app_data.sector0.alarm[idx].minute
#define ALARM_GET_CYCLE(idx)            app_data.sector0.alarm[idx].cycle
#define ALARM_GET_SWITCH(idx)           app_data.sector0.alarm[idx].swi
#elif (USE_APP_TYPE == USE_UTE_APP)
#define ALARM_EDIT(...)                 uteModuleSystemtimeAlarmEdit(__VA_ARGS__)
#define ALARM_SET(...)                  uteModuleSystemtimeAlarmEdit(__VA_ARGS__)
#define ALARM_GET_HOUR(idx)             uteModuleSystemtimeGetAlarmHour(idx)
#define ALARM_GET_MIN(idx)              uteModuleSystemtimeGetAlarmMin(idx)
#define ALARM_GET_CYCLE(idx)            uteModuleSystemtimeGetAlarmCycle(idx)
#define ALARM_GET_SWITCH(idx)           uteModuleSystemtimeGetAlarmEnableState(idx)
#else
#define ALARM_EDIT(...)
#define ALARM_SET(...)
#define ALARM_GET_HOUR(idx)             0
#define ALARM_GET_MIN(idx)              0
#define ALARM_GET_CYCLE(idx)            0
#define ALARM_GET_SWITCH(idx)           0
#endif

#define WEEKS_LIST_CNT                  ((int)(sizeof(tbl_weeks_list) / sizeof(tbl_weeks_list[0])))

//组件ID
enum
{
    //列表
    COMPO_ID_LISTBOX = 1,

    //按键
//    COMPO_ID_BTN_REPETAT_NO,
    COMPO_ID_BTN_REPETAT_YES,

    //图像
//    COMPO_ID_PIC_REPETAT_NO_CLICK,
    COMPO_ID_PIC_REPETAT_YES_CLICK,

};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
typedef struct f_alarm_clock_sub_repeat_t_
{
    compo_listbox_t *listbox;

} f_alarm_clock_sub_repeat_t;

static const compo_listbox_item_t tbl_weeks_list[] =
{
    {STR_MONDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_MON},
    {STR_TUESDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_TUE},
    {STR_WEDNESDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_WED},
    {STR_THURSDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_THU},
    {STR_FRIDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_FRI},
    {STR_SATURDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_SAT},
    {STR_SUNDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_SUN},
};

//创建闹钟--重复窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_sub_repeat_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_REPEAT]);

#define SLEF_LIST_Y_OFFSET    15
#define SLEF_LIST_HEI_OFFSET  30
    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_listbox_set(listbox, tbl_weeks_list, WEEKS_LIST_CNT);
    // compo_listbox_set_location(listbox, GUI_SCREEN_CENTER_X, (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_FIRSTORDER_CARD_BIN).hei - 5)/2 + SLEF_LIST_Y_OFFSET,
    //                            GUI_SCREEN_WIDTH, (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_FIRSTORDER_CARD_BIN).hei-SLEF_LIST_HEI_OFFSET));

    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I330001_ALARM_SELECT01_BIN, UI_BUF_I330001_ALARM_SELECT00_BIN);
    compo_listbox_set_bithook(listbox, bsp_sys_get_ctlbit);
    compo_listbox_set_focus_byidx(listbox, 1);

    s32 last_y = compo_listbox_gety_byidx(listbox, WEEKS_LIST_CNT );
    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_page_by_image(frm,listbox->page, UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN);
    compo_setid(btn, COMPO_ID_BTN_REPETAT_YES);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,last_y);
    //确定文本
    compo_textbox_t* yes_txt = compo_textbox_create_for_page(frm,listbox->page, strlen(i18n[STR_OK]));
    compo_textbox_set_location(yes_txt, GUI_SCREEN_CENTER_X, last_y,
                               gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).wid - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei,
                               gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei);
    compo_textbox_set(yes_txt, i18n[STR_OK]);

    for (int i=0; i<7; i++)         //获取当前闹钟设置的星期
    {
        bsp_sys_set_ctlbit(SYS_CTL_ACLOCK_MON + i, (ALARM_GET_CYCLE(sys_cb.alarm_edit_idx) >> i) & 1);
    }

    compo_listbox_update(listbox);



    return frm;
}

//触摸按钮效果处理
static void func_alarm_clock_sub_repeat_button_touch_handle(void)
{

    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_REPETAT_YES:
            break;

        default:
            break;
    }

}

//释放按钮效果处理
static void func_alarm_clock_sub_repeat_button_release_handle(void)
{
}

//单击按钮
static void func_alarm_clock_sub_repeat_button_click(void)
{
    int id = compo_get_button_id();

    int week_idx;
    f_alarm_clock_sub_repeat_t *f_aclock = (f_alarm_clock_sub_repeat_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_aclock->listbox;

    week_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (week_idx >= 0)
    {
        bsp_sys_reverse_ctlbit(tbl_weeks_list[week_idx].vidx);
        compo_listbox_update(listbox);
    }

    switch (id)
    {

        case COMPO_ID_BTN_REPETAT_YES:
            sys_cb.alarm_edit_cycle = 0;
            for (int i=0; i<7; i++)
            {
                sys_cb.alarm_edit_cycle |= (bsp_sys_get_ctlbit(SYS_CTL_ACLOCK_MON + i) << i);
            }
//        printf("%s-->alarm_edit_cycle[0x%x]:%x\n", __func__, sys_cb.alarm_edit_idx, sys_cb.alarm_edit_cycle);

            if (sys_cb.alarm_edit_cycle == 0)
            {
                sys_cb.alarm_edit_cycle = 0x80;   //单次
            }

            if (task_stack_get_last() == FUNC_ALARM_CLOCK_SUB_EDIT)
            {
                ALARM_EDIT(sys_cb.alarm_edit_idx,
                           ALARM_GET_SWITCH(sys_cb.alarm_edit_idx),
                           sys_cb.alarm_edit_cycle,
                           ALARM_GET_HOUR(sys_cb.alarm_edit_idx),
                           ALARM_GET_MIN(sys_cb.alarm_edit_idx),
                           0,
                           0);
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_EDIT;
                break;
            }
//            printf("hour:%d min:%d\n",sys_cb.alarm_edit_hour,sys_cb.alarm_edit_min);
            ALARM_SET(sys_cb.alarm_edit_idx, true, sys_cb.alarm_edit_cycle, sys_cb.alarm_edit_hour, sys_cb.alarm_edit_min, 0, 0);
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
            func_cb.sta = FUNC_ALARM_CLOCK;
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
            break;

        default:
            break;
    }

    func_alarm_clock_sub_repeat_button_release_handle();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
typedef struct f_alarm_clock_sub_repeat_t_
{
    compo_listbox_t *listbox;

} f_alarm_clock_sub_repeat_t;

static const compo_listbox_item_t tbl_weeks_list[] =
{
    {STR_MONDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_MON},
    {STR_TUESDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_TUE},
    {STR_WEDNESDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_WED},
    {STR_THURSDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_THU},
    {STR_FRIDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_FRI},
    {STR_SATURDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_SAT},
    {STR_SUNDAY, .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_ACLOCK_SUN},
};

//创建闹钟--重复窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_sub_repeat_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_REPEAT]);

#define SLEF_LIST_Y_OFFSET    15
#define SLEF_LIST_HEI_OFFSET  30
    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_listbox_set(listbox, tbl_weeks_list, WEEKS_LIST_CNT);
    compo_listbox_set_location(listbox, GUI_SCREEN_CENTER_X, (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_FIRSTORDER_CARD_BIN).hei - 5)/2 + SLEF_LIST_Y_OFFSET,
                               GUI_SCREEN_WIDTH,280);/* (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_FIRSTORDER_CARD_BIN).hei-SLEF_LIST_HEI_OFFSET))*/

    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I332001_ALARM_SELECT01_BIN, UI_BUF_I332001_ALARM_SELECT00_BIN);
    compo_listbox_set_bithook(listbox, bsp_sys_get_ctlbit);
    compo_listbox_set_focus_byidx(listbox, 1);

    for (int i=0; i<7; i++)         //获取当前闹钟设置的星期
    {
        bsp_sys_set_ctlbit(SYS_CTL_ACLOCK_MON + i, (ALARM_GET_CYCLE(sys_cb.alarm_edit_idx) >> i) & 1);
    }

    compo_listbox_update(listbox);

//    新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN);
    compo_setid(btn, COMPO_ID_BTN_REPETAT_YES);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN).hei / 2 - 20);

    //确定文本
    compo_textbox_t* yes_txt = compo_textbox_create(frm, strlen(i18n[STR_OK]));
    compo_textbox_set_location(yes_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN).hei / 2 - 20,
                               gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN).wid - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN).hei,
                               gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN).hei);
    compo_textbox_set(yes_txt, i18n[STR_OK]);

    return frm;
}

//触摸按钮效果处理
static void func_alarm_clock_sub_repeat_button_touch_handle(void)
{

    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_REPETAT_YES:
            break;

        default:
            break;
    }

}

//释放按钮效果处理
static void func_alarm_clock_sub_repeat_button_release_handle(void)
{
}

//单击按钮
static void func_alarm_clock_sub_repeat_button_click(void)
{
    int id = compo_get_button_id();

    int week_idx;
    f_alarm_clock_sub_repeat_t *f_aclock = (f_alarm_clock_sub_repeat_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_aclock->listbox;

    week_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (week_idx >= 0)
    {
        bsp_sys_reverse_ctlbit(tbl_weeks_list[week_idx].vidx);
        compo_listbox_update(listbox);
    }

    switch (id)
    {

        case COMPO_ID_BTN_REPETAT_YES:
            sys_cb.alarm_edit_cycle = 0;
            for (int i=0; i<7; i++)
            {
                sys_cb.alarm_edit_cycle |= (bsp_sys_get_ctlbit(SYS_CTL_ACLOCK_MON + i) << i);
            }
//        printf("%s-->alarm_edit_cycle[0x%x]:%x\n", __func__, sys_cb.alarm_edit_idx, sys_cb.alarm_edit_cycle);

            if (sys_cb.alarm_edit_cycle == 0)
            {
                sys_cb.alarm_edit_cycle = 0x80;   //单次
            }

            if (task_stack_get_last() == FUNC_ALARM_CLOCK_SUB_EDIT)
            {
                ALARM_EDIT(sys_cb.alarm_edit_idx,
                           ALARM_GET_SWITCH(sys_cb.alarm_edit_idx),
                           sys_cb.alarm_edit_cycle,
                           ALARM_GET_HOUR(sys_cb.alarm_edit_idx),
                           ALARM_GET_MIN(sys_cb.alarm_edit_idx),
                           0,
                           0);
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_EDIT;
                break;
            }
//            printf("hour:%d min:%d\n",sys_cb.alarm_edit_hour,sys_cb.alarm_edit_min);
            ALARM_SET(sys_cb.alarm_edit_idx, true, sys_cb.alarm_edit_cycle, sys_cb.alarm_edit_hour, sys_cb.alarm_edit_min, 0, 0);
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
            func_cb.sta = FUNC_ALARM_CLOCK;
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
            break;

        default:
            break;
    }

    func_alarm_clock_sub_repeat_button_release_handle();
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//闹钟--重复功能事件处理
static void func_alarm_clock_sub_repeat_process(void)
{
    f_alarm_clock_sub_repeat_t *f_aclock = (f_alarm_clock_sub_repeat_t *)func_cb.f_cb;
    compo_listbox_move(f_aclock->listbox);
    func_process();
}


//闹钟--重复功能消息处理
static void func_alarm_clock_sub_repeat_message(size_msg_t msg)
{
    f_alarm_clock_sub_repeat_t *f_aclock = (f_alarm_clock_sub_repeat_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_aclock->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        func_alarm_clock_sub_repeat_button_release_handle();
        return;                                         //处理列表框信息
    }
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_alarm_clock_sub_repeat_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_alarm_clock_sub_repeat_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_alarm_clock_sub_repeat_button_release_handle();
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_alarm_clock_sub_repeat_button_release_handle();
            func_message(msg);
            break;



        default:
            func_message(msg);
            break;
    }
}

//进入闹钟--重复功能
static void func_alarm_clock_sub_repeat_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alarm_clock_sub_repeat_t));
    func_cb.frm_main = func_alarm_clock_sub_repeat_form_create();

    f_alarm_clock_sub_repeat_t *f_aclock = (f_alarm_clock_sub_repeat_t *)func_cb.f_cb;
    f_aclock->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_aclock->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放

    s32 first_y = compo_listbox_gety_byidx(listbox, 1);
    s32 last_y = compo_listbox_gety_byidx(listbox, WEEKS_LIST_CNT - 1);
    compo_listbox_move_init_modify(listbox, first_y, last_y);

    func_cb.enter_tick = tick_get();

}

//退出闹钟--重复功能
static void func_alarm_clock_sub_repeat_exit(void)
{
    f_alarm_clock_sub_repeat_t *f_aclock = (f_alarm_clock_sub_repeat_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_aclock->listbox;
    func_free(listbox->mcb);
}

//闹钟--重复功能
void func_alarm_clock_sub_repeat(void)
{
    printf("%s\n", __func__);
    func_alarm_clock_sub_repeat_enter();
    while (func_cb.sta == FUNC_ALARM_CLOCK_SUB_REPEAT)
    {
        func_alarm_clock_sub_repeat_process();
        func_alarm_clock_sub_repeat_message(msg_dequeue());
    }
    func_alarm_clock_sub_repeat_exit();
}
