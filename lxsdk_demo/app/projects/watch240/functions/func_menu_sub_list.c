#include "include.h"
#include "func.h"
#include "func_menu.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define MENU_LIST_CNT                       ((int)(sizeof(tbl_menu_list) / sizeof(tbl_menu_list[0])))

#define ENTERING_ANIMATION_MAX              (GUI_SCREEN_HEIGHT * 9 / 6)
#define ENTERING_ANIMATION_CNT              30
#define ANIMATION_TICK_EXPIRE               12

#define ENTERING_STYLE                      0               //入场动画风格 可选 0,1

enum
{
    COMPO_ID_LISTBOX = 1,
    COMPO_ID_TXT_BATTERY_PIC,

    COMPO_ID_SCROLLBAR,
};

typedef struct f_menu_list_t_
{
    compo_listbox_t *listbox;
    int animation_cnt;
    u32 tick;
} f_menu_list_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
static const compo_listbox_item_t tbl_menu_list[] =
{
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    // {STR_SPORTS,                 UI_BUF_I330001_THEME_1_SOPRT_BIN,             .func_sta = FUNC_SPORT},                //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    // {STR_SLEEP,                  UI_BUF_I330001_THEME_1_SLEEP_BIN,             .func_sta = FUNC_SLEEP},                //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {STR_EVREY_DAY_ACTIVITY,        UI_BUF_I330001_THEME_1_ACTIVITY_BIN,          .func_sta = FUNC_ACTIVITY},             //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    // {STR_HEART_RATE,             UI_BUF_I330001_THEME_1_HEART_BIN,             .func_sta = FUNC_HEARTRATE},            //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
//    {STR_BLOOD_PRESSURE,         UI_BUF_ICON_BLOOD_PRESSURE_BIN,    .func_sta = FUNC_BLOOD_PRESSURE},       //血压
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    // {STR_BLOOD_OXYGEN,           UI_BUF_I330001_THEME_1_SPO2_BIN,              .func_sta = FUNC_BLOOD_OXYGEN},         //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {STR_MESSAGE,                UI_BUF_I330001_THEME_1_MESSAGE_BIN,           .func_sta = FUNC_MESSAGE},              //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {STR_PHONE,                  UI_BUF_I330001_THEME_1_CALL_BIN,              .func_sta = FUNC_CALL},                 //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {STR_MUSIC,                  UI_BUF_I330001_THEME_1_MUSIC_BIN,             .func_sta = FUNC_BT},                   //音乐
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {STR_WEATHER,                UI_BUF_I330001_THEME_1_WEATHER_BIN,           .func_sta = FUNC_WEATHER},              //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    // {STR_BREATHE_TRAIN,          UI_BUF_I330001_THEME_1_BREATHE_BIN,           .func_sta = FUNC_BREATHE},              //呼吸
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {STR_CALCULATOR,             UI_BUF_I330001_THEME_1_CALCULATOR_BIN,        .func_sta = FUNC_CALCULATOR},           //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {STR_ALARM_CLOCK,            UI_BUF_I330001_THEME_1_ALARM_BIN,             .func_sta = FUNC_ALARM_CLOCK},          //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {STR_TIMER,                  UI_BUF_I330001_THEME_1_TIMER_BIN,             .func_sta = FUNC_TIMER},                //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {STR_STOP_WATCH,             UI_BUF_I330001_THEME_1_STOPWATCH_BIN,         .func_sta = FUNC_STOPWATCH},            //秒表
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {STR_CALENDAR,               UI_BUF_I330001_THEME_1_CALENDAR_BIN,          .func_sta = FUNC_CALENDAER},            //日历
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {STR_CAMERA,                 UI_BUF_I330001_THEME_1_PHOTO_BIN,             .func_sta = FUNC_CAMERA},               //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {STR_VOICE_ASSISTANT,        UI_BUF_I330001_THEME_1_VIOCE_BIN,             .func_sta = FUNC_VOICE},                //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
    {STR_FIND_PHONE,             UI_BUF_I330001_THEME_1_FINDPHONE_BIN,         .func_sta = FUNC_FINDPHONE},            //查找手机
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {STR_GAME,                   UI_BUF_I330001_THEME_1_GAME_BIN,              .func_sta = FUNC_GAME},                 //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {STR_SETTING,                UI_BUF_I330001_THEME_1_SETTINGS_BIN,          .func_sta = FUNC_SETTING},              //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_STYLE,                  UI_BUF_I330001_THEME_1_THEME_BIN,             .func_sta = FUNC_STYLE},                //风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_SETTING_ABOUT,          UI_BUF_I330001_THEME_1_GUANYU_BIN,            .func_sta = FUNC_SET_SUB_ABOUT},        //关于
};
#define  BATTERY_PIC_0_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_00_BIN                ///电池电量图标0
#define  BATTERY_PIC_1_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_01_BIN                ///电池电量图标1
#define  BATTERY_PIC_2_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_02_BIN                ///电池电量图标2
#define  BATTERY_PIC_3_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_03_BIN                ///电池电量图标3
#define  BATTERY_PIC_4_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_04_BIN                ///电池电量图标4
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
static const compo_listbox_item_t tbl_menu_list[] =
{
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {STR_SPORTS,                 UI_BUF_I332001_THEME_ICON1_SOPRT_BIN,             .func_sta = FUNC_SPORT},                //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {STR_SLEEP,                  UI_BUF_I332001_THEME_ICON1_SLEEP_BIN,             .func_sta = FUNC_SLEEP},                //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {STR_EVREY_DAY_ACTIVITY,        UI_BUF_I332001_THEME_ICON1_ACTIVITY_BIN,          .func_sta = FUNC_ACTIVITY},             //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {STR_HEART_RATE,             UI_BUF_I332001_THEME_ICON1_HEART_BIN,             .func_sta = FUNC_HEARTRATE},            //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
//    {STR_BLOOD_PRESSURE,         UI_BUF_ICON_BLOOD_PRESSURE_BIN,    .func_sta = FUNC_BLOOD_PRESSURE},       //血压
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {STR_BLOOD_OXYGEN,           UI_BUF_I332001_THEME_ICON1_SPO2_BIN,              .func_sta = FUNC_BLOOD_OXYGEN},         //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {STR_MESSAGE,                UI_BUF_I332001_THEME_ICON1_MESSAGE_BIN,           .func_sta = FUNC_MESSAGE},              //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {STR_PHONE,                  UI_BUF_I332001_THEME_ICON1_CALL_BIN,              .func_sta = FUNC_CALL},                 //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {STR_MUSIC,                  UI_BUF_I332001_THEME_ICON1_MUSIC_BIN,             .func_sta = FUNC_BT},                   //音乐
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {STR_WEATHER,                UI_BUF_I332001_THEME_ICON1_WEATHER_BIN,           .func_sta = FUNC_WEATHER},              //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {STR_BREATHE_TRAIN,          UI_BUF_I332001_THEME_ICON1_BREATHE_BIN,           .func_sta = FUNC_BREATHE},              //呼吸
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {STR_CALCULATOR,             UI_BUF_I332001_THEME_ICON1_CALCULATOR_BIN,        .func_sta = FUNC_CALCULATOR},           //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {STR_ALARM_CLOCK,            UI_BUF_I332001_THEME_ICON1_ALARM_BIN,             .func_sta = FUNC_ALARM_CLOCK},          //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {STR_TIMER,                  UI_BUF_I332001_THEME_ICON1_TIMER_BIN,             .func_sta = FUNC_TIMER},                //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {STR_STOP_WATCH,             UI_BUF_I332001_THEME_ICON1_STOPWATCH_BIN,         .func_sta = FUNC_STOPWATCH},            //秒表
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {STR_CALENDAR,               UI_BUF_I332001_THEME_ICON1_CALENDAR_BIN,          .func_sta = FUNC_CALENDAER},            //日历
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {STR_CAMERA,                 UI_BUF_I332001_THEME_ICON1_PHOTO_BIN,             .func_sta = FUNC_CAMERA},               //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {STR_VOICE_ASSISTANT,        UI_BUF_I332001_THEME_ICON1_VIOCE_BIN,             .func_sta = FUNC_VOICE},                //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
    {STR_FIND_PHONE,             UI_BUF_I332001_THEME_ICON1_FINDPHONE_BIN,         .func_sta = FUNC_FINDPHONE},            //查找手机
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {STR_GAME,                   UI_BUF_I332001_THEME_ICON1_GAME_BIN,              .func_sta = FUNC_GAME},                 //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {STR_SETTING,                UI_BUF_I332001_THEME_ICON1_SETTINGS_BIN,          .func_sta = FUNC_SETTING},              //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_STYLE,                  UI_BUF_I332001_THEME_ICON1_THEME_BIN,             .func_sta = FUNC_STYLE},                //风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_SETTING_ABOUT,          UI_BUF_I332001_THEME_ICON1_GUANYU_BIN,            .func_sta = FUNC_SET_SUB_ABOUT},        //关于
};
#define  BATTERY_PIC_0_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_00_BIN                ///电池电量图标0
#define  BATTERY_PIC_1_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_01_BIN                ///电池电量图标1
#define  BATTERY_PIC_2_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_02_BIN                ///电池电量图标2
#define  BATTERY_PIC_3_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_03_BIN                ///电池电量图标3
#define  BATTERY_PIC_4_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_04_BIN                ///电池电量图标4
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//菜单栏自定义图标更新
static void func_menu_sub_list_battery_pic_update(void)
{
    compo_picturebox_t *battery_pic = compo_getobj_byid(COMPO_ID_TXT_BATTERY_PIC);
    switch(uteDrvBatteryCommonGetBatteryIndex(5))
    {
        case 0:
            compo_picturebox_set(battery_pic, BATTERY_PIC_0_BIN);
            break;
        case 1:
            compo_picturebox_set(battery_pic, BATTERY_PIC_1_BIN);
            break;
        case 2:
            compo_picturebox_set(battery_pic, BATTERY_PIC_2_BIN);
            break;
        case 3:
            compo_picturebox_set(battery_pic, BATTERY_PIC_3_BIN);
            break;
        case 4:
            compo_picturebox_set(battery_pic, BATTERY_PIC_4_BIN);
            break;
        default:
            compo_picturebox_set(battery_pic, BATTERY_PIC_4_BIN);
            break;
    }
}

//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_sub_list_form_create(void)
{
    u8 menu_idx = func_cb.menu_idx;
    //新建窗体
    compo_form_t *frm = compo_form_create(false);       //菜单一般创建在底层
    //新建菜单列表
    compo_listbox_t *listbox;
    if (func_cb.menu_style == MENU_STYLE_CUM_FOURGRID)
    {
        listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_MENU_FOURGRID);
//        compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TIME|COMPO_FORM_MODE_SHOW_ICON);
        //电池电量
//        compo_textbox_t *battery_txt = compo_textbox_create(frm, 4);
//        compo_textbox_set_pos(battery_txt, 43, 25);
////        compo_textbox_set_autosize(battery_txt, true);
//        compo_bonddata(battery_txt, COMPO_BOND_BATTERY);
        //电池
        compo_picturebox_t *battery_pic = compo_picturebox_create(frm, BATTERY_PIC_4_BIN);
        compo_setid(battery_pic, COMPO_ID_TXT_BATTERY_PIC);
        compo_picturebox_set_pos(battery_pic, 43, 25);
        func_menu_sub_list_battery_pic_update();

        if (menu_idx < 3)
        {
            menu_idx = 3;
        }
    }
    else
    {
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
        listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_MENU_NORMAL);
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
        listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_MENU_CIRCLE);
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
        if (menu_idx < 1)
        {
            menu_idx = 1;
        }
    }

    //compo_listbox_cycle_en(listbox, true);  //循环滑动
    compo_listbox_set(listbox, tbl_menu_list, MENU_LIST_CNT);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_focus_byidx(listbox, menu_idx);

//    compo_listbox_add_time(listbox, COMPO_LISTBOX_TIME_TYPE_HOUR, UI_BUF_ICON_CLOCK_H_BIN, 1, 2);
//    compo_listbox_add_time(listbox, COMPO_LISTBOX_TIME_TYPE_MIN, UI_BUF_ICON_CLOCK_M_BIN, 1, 2);
//    compo_listbox_add_time(listbox, COMPO_LISTBOX_TIME_TYPE_SEC, UI_BUF_ICON_CLOCK_S_BIN, 9, 2);
//    compo_listbox_set_start_angle(listbox, 900);
//    compo_listbox_set_time_idx(listbox, 0);

    compo_listbox_update(listbox);

    if (func_cb.flag_animation)
    {
        widget_set_visible(listbox->page, false);
    }

    if (func_cb.menu_style == MENU_STYLE_LIST)
    {
//        compo_scroll_t* scroll = compo_scroll_create(frm, SCROLL_TYPE_VERTICAL);
//        compo_setid(scroll, COMPO_ID_SCROLLBAR);
//        compo_scroll_set_w_r(scroll, 10);
//        compo_scroll_set_range(scroll, 200);
//        compo_scroll_set_pos(scroll, 300, 90);
//        compo_scroll_set_value(scroll, 0);
//        compo_scroll_set_edge_circle(scroll, true);
    }
    return frm;
}

//点进图标进入应用
static void func_menu_sub_list_icon_click(void)
{
    int icon_idx;
    f_menu_list_t *f_menu = (f_menu_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_menu->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= MENU_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_menu_list[icon_idx].func_sta;

    //切入应用
    if (func_sta > 0)
    {
        compo_form_t *frm = func_create_form(func_sta);
        func_switching(FUNC_SWITCH_ZOOM_ENTER | FUNC_SWITCH_AUTO, listbox->sel_icon);
        compo_form_destroy(frm);
        func_cb.sta = func_sta;
        func_cb.menu_idx = icon_idx;                //记住当前编号
    }
}

//切换到时钟
static void func_menu_sub_list_switch_to_clock(void)
{
    f_menu_list_t *f_menu = (f_menu_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_menu->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    u8 func_sta = FUNC_CLOCK;
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    func_cb.menu_idx = 0;
    func_cb.flag_animation = true;
}

static void func_menu_sub_list_entering(void)
{
    f_menu_list_t *f_menu = (f_menu_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_menu->listbox;

#if ENTERING_STYLE == 0           //入场风格1
    if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
    {
        widget_set_visible(listbox->page, true);
        return;
    }
    //入场动画
    static const int tbl_entering_start_pos[] =
    {
        ENTERING_ANIMATION_MAX,
        ENTERING_ANIMATION_MAX - GUI_SCREEN_HEIGHT * 2 / 6,
        ENTERING_ANIMATION_MAX - GUI_SCREEN_HEIGHT * 4 / 6,
        ENTERING_ANIMATION_MAX - GUI_SCREEN_HEIGHT * 6 / 6,
        ENTERING_ANIMATION_MAX - GUI_SCREEN_HEIGHT * 7 / 6,
        0
    };
    int idx = 0;
    int y_step = ENTERING_ANIMATION_MAX / ENTERING_ANIMATION_CNT;
    widget_set_visible(listbox->page, true);
    if(listbox->page_time != NULL)
    {
        widget_set_visible(listbox->page_time, false);
    }
    compo_listbox_update(listbox);
    widget_t *widget = widget_get_next(listbox->page);
    while (widget != NULL)
    {
        if (widget_get_parent(widget) == listbox->page)
        {
            int ln = idx;
            rect_t rect = widget_get_location(widget);

            if (ln <= LISTBOX_ITEM_CNT/2-2)    //基础空页面跳过
            {

            }
            else
            {
                ln -= (LISTBOX_ITEM_CNT/2-2);
                int y = rect.y + y_step * f_menu->animation_cnt - tbl_entering_start_pos[ln];
                if (y > rect.y)
                {
                    int w_rate = 0x10000 + 0x20000 * (y - rect.y) / (y_step * ENTERING_ANIMATION_CNT - tbl_entering_start_pos[ln]);
                    rect.y = y;
                    rect.wid = muls_shift16(rect.wid, w_rate);
                    rect.hei = muls_shift16(rect.hei, w_rate);
                }
            }
            if (idx > LISTBOX_ITEM_CNT/2-2 && idx < LISTBOX_ITEM_CNT/2+3)
            {
                widget_set_pos(widget, rect.x, rect.y);
                //widget_page_scale_to(widget, GUI_SCREEN_WIDTH, rect.hei);
            }
            else
            {
                widget_set_pos(widget, rect.x, rect.y-20);
            }
            idx++;
            if (idx > LISTBOX_ITEM_CNT/2+3)
            {
                return;
            }
        }
        if(listbox->page_time != NULL && f_menu->animation_cnt <= 1)
        {
            widget_set_visible(listbox->page_time, true);
        }

        //遍历Widget
        widget = widget_get_next(widget);
    }
#elif ENTERING_STYLE == 1 //入场风格2
    if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
    {
        widget_set_visible(listbox->page, true);
        return;
    }
    int y_step = (GUI_SCREEN_HEIGHT) / ENTERING_ANIMATION_CNT;
    int ofs_y = y_step * f_menu->animation_cnt;
    widget_set_visible(listbox->page, true);
    compo_listbox_update(listbox);
    if (ofs_y < listbox->line_space + listbox->line_height/2)
    {
        compo_listbox_set_focus_byidx(listbox, 1);
    }
    else
    {
        widget_page_set_client(listbox->page, 0, ofs_y);
    }
#endif


}

//主菜单功能事件处理
static void func_menu_sub_list_process(void)
{
    f_menu_list_t *f_menu = (f_menu_list_t *)func_cb.f_cb;
    if (f_menu->animation_cnt > 0)
    {
        //入场动画
        if (tick_check_expire(f_menu->tick, ANIMATION_TICK_EXPIRE))
        {
            func_menu_sub_list_entering();
            f_menu->tick = tick_get();
            f_menu->animation_cnt--;
        }
    }
    else
    {
        if (func_cb.menu_style == MENU_STYLE_LIST)
        {
            int value = (f_menu->listbox->ofs_y - f_menu->listbox->mcb->first_y) * 1000 / abs_s(f_menu->listbox->mcb->last_y - f_menu->listbox->mcb->first_y);
            //printf("value = %d, ofs_y=%d, first_y=%d, last_y=%d\n", value, f_menu->listbox->ofs_y,
            //                f_menu->listbox->mcb->first_y, f_menu->listbox->mcb->last_y);
//            compo_scroll_t* scroll = compo_getobj_byid(COMPO_ID_SCROLLBAR);
//            compo_scroll_set_value(scroll, value);
        }
//        printf("length:%d\n",f_menu->listbox->ofs_y);
        compo_listbox_move(f_menu->listbox);
    }
    func_process();
}

static void func_menu_sub_list_entering_message(size_msg_t msg)
{
    switch (msg)
    {
        default:
            evt_message(msg);
            break;
    }
}

//主菜单功能消息处理
static void func_menu_sub_list_message(size_msg_t msg)
{
    f_menu_list_t *f_menu = (f_menu_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_menu->listbox;

    if (f_menu->animation_cnt > 0)
    {
        func_menu_sub_list_entering_message(msg);
    }
    else
    {
        if (compo_listbox_message(listbox, msg))
        {
            return;                                         //处理列表框信息
        }
        switch (msg)
        {
            case MSG_CTP_CLICK:
                func_menu_sub_list_icon_click();                //单击图标
                break;

            case MSG_CTP_LONG:
                break;

            case MSG_CTP_SHORT_RIGHT:
//                if(func_cb.menu_style == MENU_STYLE_CUM_FOURGRID)
//                {
//                    if(f_menu->listbox->ofs_y <= 90)
//                    {
//                        if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
//                        {
//                            func_menu_sub_list_switch_to_clock();       //返回时钟表盘界面
//                        }
//                    }
//                }
//
//                break;
            case KU_DELAY_BACK:
                if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
                {
                    func_menu_sub_list_switch_to_clock();       //返回时钟表盘界面
                }
                break;

            case MSG_SYS_500MS:
                if (func_cb.menu_style == MENU_STYLE_CUM_FOURGRID)
                {
                    func_menu_sub_list_battery_pic_update();
                }
                break;

            default:
                func_menu_sub_message(msg);
                break;
        }
    }
}

//进入主菜单功能
static void func_menu_sub_list_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_menu_list_t));
    func_cb.frm_main = func_menu_sub_list_form_create();

    f_menu_list_t *f_menu = (f_menu_list_t *)func_cb.f_cb;
    f_menu->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_menu->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init(listbox);
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, MENU_LIST_CNT - 2));
    func_cb.enter_tick = tick_get();
    if (func_cb.flag_animation)
    {
        func_cb.flag_animation = 0;
        f_menu->animation_cnt = ENTERING_ANIMATION_CNT;
//        f_menu->animation_cnt = 0;
        f_menu->tick = tick_get();
    }
}

//退出菜单样式
void func_menu_sub_list_exit(void)
{
    f_menu_list_t *f_menu = (f_menu_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_menu->listbox;
    func_free(listbox->mcb);

    compo_form_destroy(func_cb.frm_main);
    func_free(func_cb.f_cb);
    func_cb.frm_main = NULL;
    func_cb.f_cb = NULL;
}

//主菜单功能
void func_menu_sub_list(void)
{
    printf("%s\n", __func__);
    func_menu_sub_list_enter();
    while (func_cb.sta == FUNC_MENU && (func_cb.menu_style == MENU_STYLE_LIST || func_cb.menu_style == MENU_STYLE_CUM_FOURGRID))
    {
        func_menu_sub_list_process();
        func_menu_sub_list_message(msg_dequeue());
    }
    func_menu_sub_list_exit();
}
