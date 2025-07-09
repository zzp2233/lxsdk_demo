#include "include.h"
#include "func.h"
#include "func_menu.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define DY_TO_DDEG                          4                   //y变化1单位deg变化0.4度（*10=4）

#define FOCUS_AUTO_TICK_EXPIRE              10                  //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                     5                   //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV                 16

#define DRAG_AUTO_SPEED                     60                  //拖动松手后的速度

#define MENU_DISK_CNT                       ((int)(sizeof(tbl_menu_disk) / sizeof(tbl_menu_disk[0])))

#define ENTERING_ANIMATION_CNT              80
#define ANIMATION_TICK_EXPIRE               5

#define ICON_START_SIZE                     40

enum
{
    COMPO_ID_DISKLIST = 1,
    COMPO_ID_GEAR,
};

typedef struct f_menu_disk_t_
{
    compo_disklist_t *disklist;
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    u32 tick;
    s32 focus_deg;
    s32 focus_ddeg;
    s32 focus_degstep;
    int moveto_idx;                 //设定自动移到的目标菜单
    s32 moveto_deg;                 //设定自动移到的角度（10倍）

    int animation_cnt;

    s32 deg_last;
    s16 deg_gear;
} f_menu_disk_t;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
static const compo_disklist_item_t tbl_menu_disk[] =
{
//    {STR_CLOCK,                  UI_BUF_ICON_CLOCK_BG_BIN,          .func_sta = FUNC_CLOCK},                //时钟
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {STR_SPORTS,                 UI_BUF_I330001_THEME_1_SOPRT_BIN,             .func_sta = FUNC_SPORT},                //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
//    {STR_STEP,                   UI_BUF_ICON_STEP_BIN,              .func_sta = FUNC_NULL},                 //计步
//    {STR_PRESSURE,               UI_BUF_ICON_PRESSURE_BIN,          .func_sta = FUNC_PRESSURE},             //压力
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {STR_SLEEP,                  UI_BUF_I330001_THEME_1_SLEEP_BIN,             .func_sta = FUNC_SLEEP},                //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {STR_EVREY_DAY_ACTIVITY,        UI_BUF_I330001_THEME_1_ACTIVITY_BIN,          .func_sta = FUNC_ACTIVITY},             //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {STR_HEART_RATE,             UI_BUF_I330001_THEME_1_HEART_BIN,        .func_sta = FUNC_HEARTRATE},            //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
//    {STR_BLOOD_PRESSURE,         UI_BUF_ICON_BLOOD_PRESSURE_BIN,    .func_sta = FUNC_BLOOD_PRESSURE},       //血压
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {STR_BLOOD_OXYGEN,           UI_BUF_I330001_THEME_1_SPO2_BIN,      .func_sta = FUNC_BLOOD_OXYGEN},         //血氧
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
    {STR_BEATHER_DURATION,                UI_BUF_I330001_THEME_1_BREATHE_BIN,           .func_sta = FUNC_BREATHE},              //呼吸
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {STR_CALCULATOR,             UI_BUF_I330001_THEME_1_CALCULATOR_BIN,        .func_sta = FUNC_CALCULATOR},           //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {STR_ALARM_CLOCK,            UI_BUF_I330001_THEME_1_ALARM_BIN,       .func_sta = FUNC_ALARM_CLOCK},          //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {STR_TIMER,                  UI_BUF_I330001_THEME_1_TIMER_BIN,             .func_sta = FUNC_TIMER},                //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
    {STR_STOP_WATCH,             UI_BUF_I330001_THEME_1_STOPWATCH_BIN,         .func_sta = FUNC_STOPWATCH},            //秒表
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {STR_CAMERA,                 UI_BUF_I330001_THEME_1_PHOTO_BIN,            .func_sta = FUNC_CAMERA},               //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {STR_VOICE_ASSISTANT,        UI_BUF_I330001_THEME_1_VIOCE_BIN,             .func_sta = FUNC_VOICE},                //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
    {STR_FIND_PHONE,             UI_BUF_I330001_THEME_1_FINDPHONE_BIN,         .func_sta = FUNC_FINDPHONE},            //查找手机
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {STR_GAME,                   UI_BUF_I330001_THEME_1_GAME_BIN,              .func_sta = FUNC_GAME},                 //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {STR_SETTING,                UI_BUF_I330001_THEME_1_SETTINGS_BIN,           .func_sta = FUNC_SETTING},              //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_STYLE,                  UI_BUF_I330001_THEME_1_THEME_BIN,              .func_sta = FUNC_STYLE},                //风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
};
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
static const compo_disklist_item_t tbl_menu_disk[] =
{
//#if UTE_MODULE_SCREENS_SPORT_SUPPORT
//    {STR_SPORTS,                 UI_BUF_I330001_THEME_1_SOPRT_BIN,             .func_sta = FUNC_SPORT},                //运动
//#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
////    {STR_STEP,                   UI_BUF_ICON_STEP_BIN,              .func_sta = FUNC_NULL},                 //计步
////    {STR_PRESSURE,               UI_BUF_ICON_PRESSURE_BIN,          .func_sta = FUNC_PRESSURE},             //压力
//#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
//    {STR_SLEEP,                  UI_BUF_I330001_THEME_1_SLEEP_BIN,             .func_sta = FUNC_SLEEP},                //睡眠
//#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
//#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
//    {STR_EVREY_DAY_ACTIVITY,        UI_BUF_I330001_THEME_1_ACTIVITY_BIN,          .func_sta = FUNC_ACTIVITY},             //活动记录
//#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
//#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
//    {STR_HEART_RATE,             UI_BUF_I330001_THEME_1_HEART_BIN,        .func_sta = FUNC_HEARTRATE},            //心率
//#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
////    {STR_BLOOD_PRESSURE,         UI_BUF_ICON_BLOOD_PRESSURE_BIN,    .func_sta = FUNC_BLOOD_PRESSURE},       //血压
//#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//    {STR_BLOOD_OXYGEN,           UI_BUF_I330001_THEME_1_SPO2_BIN,      .func_sta = FUNC_BLOOD_OXYGEN},         //血氧
//#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
//    {STR_MESSAGE,                UI_BUF_I330001_THEME_1_MESSAGE_BIN,           .func_sta = FUNC_MESSAGE},              //消息
//#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
//#if UTE_MODULE_SCREENS_CALL_SUPPORT
//    {STR_PHONE,                  UI_BUF_I330001_THEME_1_CALL_BIN,              .func_sta = FUNC_CALL},                 //电话
//#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
//#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
//    {STR_MUSIC,                  UI_BUF_I330001_THEME_1_MUSIC_BIN,             .func_sta = FUNC_BT},                   //音乐
//#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
//#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
//    {STR_WEATHER,                UI_BUF_I330001_THEME_1_WEATHER_BIN,           .func_sta = FUNC_WEATHER},              //天气
//#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
//#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
//    {STR_BEATHER_DURATION,                UI_BUF_I330001_THEME_1_BREATHE_BIN,           .func_sta = FUNC_BREATHE},              //呼吸
//#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
//#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
//    {STR_CALCULATOR,             UI_BUF_I330001_THEME_1_CALCULATOR_BIN,        .func_sta = FUNC_CALCULATOR},           //计算器
//#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
//#if UTE_MODULE_SCREENS_ALARM_SUPPORT
//    {STR_ALARM_CLOCK,            UI_BUF_I330001_THEME_1_ALARM_BIN,       .func_sta = FUNC_ALARM_CLOCK},          //闹钟
//#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
//#if UTE_MODULE_SCREENS_TIMER_SUPPORT
//    {STR_TIMER,                  UI_BUF_I330001_THEME_1_TIMER_BIN,             .func_sta = FUNC_TIMER},                //定时器
//#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
//    {STR_STOP_WATCH,             UI_BUF_I330001_THEME_1_STOPWATCH_BIN,         .func_sta = FUNC_STOPWATCH},            //秒表
//#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
//    {STR_CAMERA,                 UI_BUF_I330001_THEME_1_PHOTO_BIN,            .func_sta = FUNC_CAMERA},               //相机
//#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
//#if UTE_MODULE_SCREENS_VOICE_SUPPORT
//    {STR_VOICE_ASSISTANT,        UI_BUF_I330001_THEME_1_VIOCE_BIN,             .func_sta = FUNC_VOICE},                //语音助手
//#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
//    {STR_FIND_PHONE,             UI_BUF_I330001_THEME_1_FINDPHONE_BIN,         .func_sta = FUNC_FINDPHONE},            //查找手机
//#if UTE_MODULE_SCREENS_GAME_SUPPORT
//    {STR_GAME,                   UI_BUF_I330001_THEME_1_GAME_BIN,              .func_sta = FUNC_GAME},                 //游戏
//#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
//#if UTE_MODULE_SCREENS_SETTING_SUPPORT
//    {STR_SETTING,                UI_BUF_I330001_THEME_1_SETTINGS_BIN,           .func_sta = FUNC_SETTING},              //设置
//#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
//#if UTE_MODULE_SCREENS_STYLE_SUPPORT
//    {STR_STYLE,                  UI_BUF_I330001_THEME_1_THEME_BIN,              .func_sta = FUNC_STYLE},                //风格
//#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
};
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_sub_disk_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(false);       //菜单一般创建在底层

    //新建菜单列表
    compo_disklist_t *disklist = compo_disklist_create(frm);
    compo_disklist_set(disklist, tbl_menu_disk, MENU_DISK_CNT);

//    compo_disklist_set_time_idx(disklist, 0);
//    compo_disklist_add_time(disklist, COMPO_ICONLIST_TIME_TYPE_HOUR, UI_BUF_ICON_CLOCK_H_BIN, 1, 2);
//    compo_disklist_add_time(disklist, COMPO_ICONLIST_TIME_TYPE_MIN, UI_BUF_ICON_CLOCK_M_BIN, 1, 2);
//    compo_disklist_add_time(disklist, COMPO_ICONLIST_TIME_TYPE_SEC, UI_BUF_ICON_CLOCK_S_BIN, 9, 2);
//    compo_disklist_set_start_angle(disklist, 900);

    if (func_cb.flag_animation)
    {
        compo_disklist_set_iconsize(disklist, ICON_START_SIZE);
    }

    compo_setid(disklist, COMPO_ID_DISKLIST);

    compo_disklist_set_focus_byidx(disklist, func_cb.menu_idx);
    compo_disklist_update_with_text_scroll_rst(disklist);

    //齿轮背景图
    compo_picturebox_t *pic = compo_picturebox_create(frm, 0);
    compo_setid(pic, COMPO_ID_GEAR);
    compo_picturebox_set_size(pic, GUI_SCREEN_WIDTH/2, GUI_SCREEN_WIDTH/2);
    compo_picturebox_set_pos(pic, 0, GUI_SCREEN_CENTER_Y);

    return frm;
}

//点进图标进入应用
static void func_menu_sub_disk_icon_click(void)
{
    int icon_idx;
    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    compo_disklist_t *disklist = f_menu->disklist;
    u8 func_sta;

    icon_idx = compo_disklist_select(disklist, ctp_get_sxy().x, ctp_get_sxy().y);
    if (icon_idx < 0 || icon_idx >= MENU_DISK_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_menu_disk[icon_idx].func_sta;

    //切入应用
    if (func_sta > 0)
    {
        compo_form_t *frm = func_create_form(func_sta);
        func_switching(FUNC_SWITCH_ZOOM_ENTER | FUNC_SWITCH_AUTO, disklist->sel_icon);
        compo_form_destroy(frm);
        func_cb.sta = func_sta;
        func_cb.menu_idx = icon_idx;                //记住当前编号
    }
}

//切换到时钟
static void func_menu_sub_disk_switch_to_clock(void)
{
    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    compo_disklist_t *disklist = f_menu->disklist;
    widget_icon_t *icon = compo_disklist_select_byidx(disklist, 0);
    u8 func_sta = FUNC_CLOCK;
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    func_cb.menu_idx = 0;
    func_cb.flag_animation = true;
}

//自动移动标志更新，停止后同时更新焦点
static void set_auto_flag(bool auto_sta)
{
    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    compo_disklist_t *disklist = f_menu->disklist;
    f_menu->flag_move_auto = auto_sta;
    disklist->focus_rem_en = !auto_sta;
    if (disklist->focus_rem_en)
    {
        compo_disklist_set_focus(disklist, f_menu->focus_deg);
        compo_disklist_update(disklist);
        f_menu->focus_deg = disklist->ofs_deg;
    }
}

//入场动画
static void func_menu_sub_disk_entering(void)
{
    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    compo_disklist_t *disklist = f_menu->disklist;
    if (disklist->icon_size < disklist->icon_org_size)
    {
        disklist->icon_size++;
    }
    compo_disklist_set_iconsize(disklist, disklist->icon_size);
    compo_disklist_update(disklist);

}

//齿轮角度更新
static void func_menu_sub_disk_gear_update(void)
{
    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    compo_disklist_t *disklist = f_menu->disklist;

    s32 deg_cur = -disklist->ofs_deg;
    s32 deg_diff = deg_cur - f_menu->deg_last;
    f_menu->deg_last = deg_cur;
    if (deg_diff && abs(deg_diff) < 300)    //防止连续不停转动后突变
    {
        f_menu->deg_gear += deg_diff;
    }
    else if (deg_diff)
    {
        TRACE("[deg_diff:%d]\n", deg_diff);
    }

    f_menu->deg_gear %= 3600;
    compo_picturebox_set_rotation(compo_getobj_byid(COMPO_ID_GEAR), f_menu->deg_gear);
}

//主菜单功能事件处理
static void func_menu_sub_disk_process(void)
{
    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    compo_disklist_t *disklist = f_menu->disklist;
    if (f_menu->animation_cnt > 0)
    {
        //入场动画
        if (tick_check_expire(f_menu->tick, ANIMATION_TICK_EXPIRE))
        {
            f_menu->tick = tick_get();
            f_menu->animation_cnt--;
            func_menu_sub_disk_entering();
        }
    }
    else if (f_menu->flag_drag)
    {
        s32 dx;
        s32 ddeg = f_menu->focus_ddeg;
        f_menu->flag_drag = ctp_get_dxy(&dx, &f_menu->focus_ddeg);
        if (f_menu->flag_drag)
        {
            //拖动菜单图标
            f_menu->focus_ddeg *= DY_TO_DDEG;
            f_menu->focus_degstep = f_menu->focus_ddeg - ddeg;
            if (f_menu->focus_degstep)
            {
                compo_disklist_set_focus(disklist, f_menu->focus_deg - f_menu->focus_ddeg);
                compo_disklist_update(disklist);
            }
        }
        else
        {
            //抬手后开始自动移动
            s32 last_dy = ctp_get_last_dxy().y;
            f_menu->focus_deg = disklist->ofs_deg;
            set_auto_flag(true);
            f_menu->moveto_idx = disklist->focus_icon_idx - (last_dy * DRAG_AUTO_SPEED / GUI_SCREEN_HEIGHT);
            f_menu->moveto_deg = compo_disklist_get_deg_byidx(disklist, f_menu->moveto_idx);
            TRACE("moveto_deg[%d]\n", f_menu->moveto_deg);
            f_menu->tick = tick_get();
        }
    }
    if (f_menu->flag_move_auto)
    {
        //自动移动
        if (f_menu->focus_deg == f_menu->moveto_deg)
        {
            set_auto_flag(false);                 //移动完成
        }
        else if (tick_check_expire(f_menu->tick, FOCUS_AUTO_TICK_EXPIRE))
        {
            s32 ddeg;
            f_menu->tick = tick_get();
            ddeg = f_menu->moveto_deg - f_menu->focus_deg;
            if (ddeg > 0)
            {
                if (ddeg > FOCUS_AUTO_STEP * DY_TO_DDEG * FOCUS_AUTO_STEP_DIV)
                {
                    ddeg = ddeg / FOCUS_AUTO_STEP_DIV;
                }
                else if (ddeg > FOCUS_AUTO_STEP * DY_TO_DDEG)
                {
                    ddeg = FOCUS_AUTO_STEP * DY_TO_DDEG;
                }
                else if (ddeg > DY_TO_DDEG)
                {
                    ddeg = DY_TO_DDEG;
                }
            }
            else
            {
                if (ddeg < -FOCUS_AUTO_STEP * DY_TO_DDEG * FOCUS_AUTO_STEP_DIV)
                {
                    ddeg = ddeg / FOCUS_AUTO_STEP_DIV;
                }
                else if (ddeg < -FOCUS_AUTO_STEP * DY_TO_DDEG)
                {
                    ddeg = -FOCUS_AUTO_STEP * DY_TO_DDEG;
                }
                else if (ddeg < -DY_TO_DDEG)
                {
                    ddeg = -DY_TO_DDEG;
                }
            }
            f_menu->focus_deg += ddeg;
            compo_disklist_set_focus(disklist, f_menu->focus_deg);
            compo_disklist_update(disklist);
        }
    }

    func_menu_sub_disk_gear_update();
    func_process();
}

static void func_menu_sub_disk_entering_message(size_msg_t msg)
{
    switch (msg)
    {
        default:
            evt_message(msg);
            break;
    }
}

//拖动过程中，只响应部分消息
static void func_menu_sub_disk_drag_message(size_msg_t msg)
{
    switch (msg)
    {
        default:
            evt_message(msg);
            break;
    }
}

//自动移动中，只响应部分消息
static void func_menu_sub_disk_move_auto_message(size_msg_t msg)
{
    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    compo_disklist_t *disklist = f_menu->disklist;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            f_menu->flag_drag = true;                       //移动过程中，触屏停止
            set_auto_flag(false);
            break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
            f_menu->moveto_idx++;
            f_menu->moveto_deg = compo_disklist_get_deg_byidx(disklist, f_menu->moveto_idx);
            TRACE("++moveto_deg[%d]\n", f_menu->moveto_deg);
            break;

        case MSG_QDEC_BACKWARD:                              //向后滚动菜单
            f_menu->moveto_idx--;
            f_menu->moveto_deg = compo_disklist_get_deg_byidx(disklist, f_menu->moveto_idx);
            TRACE("--moveto_deg[%d]\n", f_menu->moveto_deg);
            break;

        default:
            evt_message(msg);
            break;
    }
}

//正常模式下
static void func_menu_sub_disk_normal_message(size_msg_t msg)
{
    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    compo_disklist_t *disklist = f_menu->disklist;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_menu_sub_disk_icon_click();                //单击图标
            break;

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_SHORT_DOWN:
            f_menu->flag_drag = true;                       //任何方向短划，开启拖动
            set_auto_flag(false);
            break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
            set_auto_flag(true);
            f_menu->moveto_idx = disklist->focus_icon_idx + 1;
            f_menu->moveto_deg = compo_disklist_get_deg_byidx(disklist, f_menu->moveto_idx);
            TRACE("++moveto_deg[%d]\n", f_menu->moveto_deg);
            break;

        case MSG_QDEC_BACKWARD:                              //向后滚动菜单
            set_auto_flag(true);
            f_menu->moveto_idx = disklist->focus_icon_idx - 1;
            f_menu->moveto_deg = compo_disklist_get_deg_byidx(disklist, f_menu->moveto_idx);
            TRACE("--moveto_deg[%d]\n", f_menu->moveto_deg);
            break;

        case MSG_CTP_LONG:
            break;

        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_menu_sub_disk_switch_to_clock();       //返回时钟表盘界面
            }
            break;

        default:
            func_menu_sub_message(msg);
            break;
    }
}

//主菜单功能消息处理
static void func_menu_sub_disk_message(size_msg_t msg)
{
    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    if (f_menu->animation_cnt > 0)
    {
        func_menu_sub_disk_entering_message(msg);
    }
    else if (f_menu->flag_drag)
    {
        //拖动过程中，只响应部分消息
        func_menu_sub_disk_drag_message(msg);
    }
    else if (f_menu->flag_move_auto)
    {
        //自动移动中，只响应部分消息
        func_menu_sub_disk_move_auto_message(msg);
    }
    else
    {
        //正常模式下
        func_menu_sub_disk_normal_message(msg);
    }
}

//进入主菜单功能
static void func_menu_sub_disk_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_menu_disk_t));
    func_cb.frm_main = func_menu_sub_disk_form_create();

    f_menu_disk_t *f_menu = (f_menu_disk_t *)func_cb.f_cb;
    f_menu->disklist = compo_getobj_byid(COMPO_ID_DISKLIST);
    compo_disklist_t *disklist = f_menu->disklist;
    if (disklist->type != COMPO_TYPE_DISKLIST)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    f_menu->focus_deg = disklist->ofs_deg;
    func_cb.enter_tick = tick_get();

    f_menu->animation_cnt = 0;
    if (func_cb.flag_animation)
    {
        func_cb.flag_animation = false;
        f_menu->animation_cnt = ENTERING_ANIMATION_CNT;
        f_menu->tick = tick_get();
    }
}

//主菜单功能
void func_menu_sub_disk(void)
{
    printf("%s\n", __func__);
    func_menu_sub_disk_enter();
    while (func_cb.sta == FUNC_MENU && func_cb.menu_style == MENU_STYLE_DISK)
    {
        func_menu_sub_disk_process();
        func_menu_sub_disk_message(msg_dequeue());
    }
    func_menu_sub_exit();
}
