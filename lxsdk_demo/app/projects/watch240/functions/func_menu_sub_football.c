#include "include.h"
#include "func.h"
#include "func_menu.h"
#include "ute_module_message.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define FOOTBALL_RADIUS                          140*0.77
#define MENU_FOOTBALL_ITEM_CNT                   ((int)(sizeof(tbl_menu_football) / sizeof(tbl_menu_football[0])))
#define KALE_EDGE_SPACE                          2                                                              //边缘距离
#define KALE_ICON_SPACE                          2                                                              //图标距离
#define KALE_ICON_OUTER_SIZE                     (gui_image_get_size(tbl_menu_football[0].res_addr).wid)    //外圈图标大小
#define KALE_ICON_INNER_SIZE                     (KALE_ICON_OUTER_SIZE / 5 * 2)                                 //内圈图标大小
#define KALE_REFRASH_EXPIRE                      1                                                              //刷新间隔 uint：ms
#define KALE_SWITCH_DR                           2                                                              //单次变更R值
#define KALE_ROTATE_ANGLE                        25                                                             //单次旋转角度 范围：1~KALE_CIR_ANGLE_COM

enum
{
    COMPO_ID_FOOTBALL = 1,
};

typedef struct f_menu_football_t_
{
    compo_football_t *ball;
    int  cube_rp ;
    int  cube_ra ;
    bool cube_touch;
    uint32_t tick;
    u16 cube_touch_time;
    u8 touch_time_flag;
} f_menu_football_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//足球图标列表(固定20项)
static const compo_football_item_t tbl_menu_football[20] =
{
// #if UTE_MODULE_SCREENS_STYLE_SUPPORT
//     {UI_BUF_I330001_THEME_2_THEME_BIN,            FUNC_STYLE},
// #endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
// #if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
//     {UI_BUF_I330001_THEME_2_FLASHLIGHT_BIN,       FUNC_FLASHLIGHT},
// #endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
// #if UTE_MODULE_SCREENS_TIMER_SUPPORT
//     {UI_BUF_I330001_THEME_2_TIMER_BIN,            FUNC_TIMER},
// #endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
//     {UI_BUF_I330001_THEME_2_LANUAGE_BIN,          FUNC_LANGUAGE},
// #if UTE_MODULE_SCREENS_ALARM_SUPPORT
//     {UI_BUF_I330001_THEME_2_ALARM_BIN,            FUNC_ALARM_CLOCK},
// #endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
// #if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
//     {UI_BUF_I330001_THEME_2_CONTACTS_BIN,         FUNC_ADDRESS_BOOK},
// #endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
// //    {UI_BUF_I330001_THEME_2_SOUND_BIN,            FUNC_SET_SUB_SAV},
// #if UTE_MODULE_SCREENS_CALL_SUPPORT
//     {UI_BUF_I330001_THEME_2_CALL_BIN,             FUNC_CALL},
// #endif // UTE_MODULE_SCREENS_CALL_SUPPORT
// #if UTE_MODULE_SCREENS_MUSIC_SUPPORT
//     {UI_BUF_I330001_THEME_2_MUSIC_BIN,            FUNC_BT},
// #endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
// #if UTE_MODULE_SCREENS_GAME_SUPPORT
//     {UI_BUF_I330001_THEME_2_GAME_BIN,             FUNC_GAME},
// #endif // UTE_MODULE_SCREENS_GAME_SUPPORT
// #if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//     {UI_BUF_I330001_THEME_2_SPO2_BIN,             FUNC_BLOOD_OXYGEN},
// #endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
// #if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
//     {UI_BUF_I330001_THEME_2_CALENDAR_BIN,         FUNC_CALENDAER},
// #endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
// #if UTE_MODULE_SCREENS_WEATHER_SUPPORT
//     {UI_BUF_I330001_THEME_2_WEATHER_BIN,          FUNC_WEATHER},
// #endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
// #if UTE_MODULE_SCREENS_SLEEP_SUPPORT
//     {UI_BUF_I330001_THEME_2_SLEEP_BIN,            FUNC_SLEEP},
// #endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
// #if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
//     {UI_BUF_I330001_THEME_2_CALCULATOR_BIN,       FUNC_CALCULATOR},
// #endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
// #if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
//     {UI_BUF_I330001_THEME_2_STOPWATCH_BIN,        FUNC_STOPWATCH},
// #endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT

// #if UTE_MODULE_SCREENS_SPORT_SUPPORT
//     {UI_BUF_I330001_THEME_2_SOPRT_BIN,            FUNC_SPORT},
// #endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
// #if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
//     {UI_BUF_I330001_THEME_2_HEART_BIN,            FUNC_HEARTRATE},
// #endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
// #if UTE_MODULE_SCREENS_SETTING_SUPPORT
//     {UI_BUF_I330001_THEME_2_SETTINGS_BIN,         FUNC_SETTING},
// #endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
// #if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
//     {UI_BUF_I330001_THEME_2_ACTIVITY_BIN,         FUNC_ACTIVITY},
// #endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
// #if UTE_MODULE_SCREENS_BREATHE_SUPPORT
//     {UI_BUF_I330001_THEME_2_BREATHE_BIN,          FUNC_BREATHE},
// #endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
// #if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
//     {UI_BUF_I330001_THEME_2_CALENDAR_BIN,         FUNC_CALENDAER},
// #endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
// #if UTE_MODULE_SCREENS_WEATHER_SUPPORT
//     {UI_BUF_I330001_THEME_2_WEATHER_BIN,          FUNC_WEATHER},
// #endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
// #if UTE_MODULE_SCREENS_ALARM_SUPPORT
//     {UI_BUF_I330001_THEME_2_ALARM_BIN,            FUNC_ALARM_CLOCK},
// #endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
// #if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
//     {UI_BUF_I330001_THEME_2_CONTACTS_BIN,         FUNC_ADDRESS_BOOK},
// #endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
// #if UTE_MODULE_SCREENS_STYLE_SUPPORT
//     {UI_BUF_I330001_THEME_2_THEME_BIN,            FUNC_STYLE},
// #endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
// #if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
//     {UI_BUF_I330001_THEME_2_FLASHLIGHT_BIN,       FUNC_FLASHLIGHT},
// #endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
// #if UTE_MODULE_SCREENS_TIMER_SUPPORT
//     {UI_BUF_I330001_THEME_2_TIMER_BIN,            FUNC_TIMER},
// #endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
};
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//足球图标列表(固定20项)
static const compo_football_item_t tbl_menu_football[20] =
{
//#if UTE_MODULE_SCREENS_STYLE_SUPPORT
//    {UI_BUF_I330001_THEME_2_THEME_BIN,            FUNC_STYLE},
//#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
//#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
//    {UI_BUF_I330001_THEME_2_FLASHLIGHT_BIN,       FUNC_FLASHLIGHT},
//#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
//#if UTE_MODULE_SCREENS_TIMER_SUPPORT
//    {UI_BUF_I330001_THEME_2_TIMER_BIN,            FUNC_TIMER},
//#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
//    {UI_BUF_I330001_THEME_2_LANUAGE_BIN,          FUNC_LANGUAGE},
//#if UTE_MODULE_SCREENS_ALARM_SUPPORT
//    {UI_BUF_I330001_THEME_2_ALARM_BIN,            FUNC_ALARM_CLOCK},
//#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
//#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
//    {UI_BUF_I330001_THEME_2_CONTACTS_BIN,         FUNC_ADDRESS_BOOK},
//#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
////    {UI_BUF_I330001_THEME_2_SOUND_BIN,            FUNC_SET_SUB_SAV},
//#if UTE_MODULE_SCREENS_CALL_SUPPORT
//    {UI_BUF_I330001_THEME_2_CALL_BIN,             FUNC_CALL},
//#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
//#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
//    {UI_BUF_I330001_THEME_2_MUSIC_BIN,            FUNC_BT},
//#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
//#if UTE_MODULE_SCREENS_GAME_SUPPORT
//    {UI_BUF_I330001_THEME_2_GAME_BIN,             FUNC_GAME},
//#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
//#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//    {UI_BUF_I330001_THEME_2_SPO2_BIN,             FUNC_BLOOD_OXYGEN},
//#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
//    {UI_BUF_I330001_THEME_2_CALENDAR_BIN,         FUNC_CALENDAER},
//#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
//#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
//    {UI_BUF_I330001_THEME_2_WEATHER_BIN,          FUNC_WEATHER},
//#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
//#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
//    {UI_BUF_I330001_THEME_2_SLEEP_BIN,            FUNC_SLEEP},
//#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
//#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
//    {UI_BUF_I330001_THEME_2_CALCULATOR_BIN,       FUNC_CALCULATOR},
//#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
//#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
//    {UI_BUF_I330001_THEME_2_STOPWATCH_BIN,        FUNC_STOPWATCH},
//#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
//
//#if UTE_MODULE_SCREENS_SPORT_SUPPORT
//    {UI_BUF_I330001_THEME_2_SOPRT_BIN,            FUNC_SPORT},
//#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
//#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
//    {UI_BUF_I330001_THEME_2_HEART_BIN,            FUNC_HEARTRATE},
//#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
//#if UTE_MODULE_SCREENS_SETTING_SUPPORT
//    {UI_BUF_I330001_THEME_2_SETTINGS_BIN,         FUNC_SETTING},
//#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
//#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
//    {UI_BUF_I330001_THEME_2_ACTIVITY_BIN,         FUNC_ACTIVITY},
//#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
//#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
//    {UI_BUF_I330001_THEME_2_BREATHE_BIN,          FUNC_BREATHE},
//#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
};
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_sub_football_form_create(void)
{

    //新建窗体
    compo_form_t *frm = compo_form_create(false);       //菜单一般创建在底层

//    //创建背景
//    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ICON_FOOTBALL_BG_BIN);
//    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //创建足球菜单
    // compo_football_t *ball = compo_football_create(frm, FOOTBALL_RADIUS, tbl_menu_football, MENU_FOOTBALL_ITEM_CNT);

    // compo_setid(ball, COMPO_ID_FOOTBALL);
    //compo_football_update(ball);
    return frm;
}

//点进图标进入应用
static void func_menu_sub_disk_icon_click(void)
{
    f_menu_football_t *f_menu = (f_menu_football_t *)func_cb.f_cb;
    compo_football_t *ball = f_menu->ball;
    point_t pt = ctp_get_sxy();

    int icon_idx = compo_football_get_idx(ball, pt.x, pt.y);
    u8 func_sta;
    if (icon_idx < 0 || icon_idx >= MENU_FOOTBALL_ITEM_CNT)
    {
        return;
    }
    //根据图标索引获取应用ID
    func_sta = tbl_menu_football[icon_idx].func_sta;

    //切入应用
    if (func_sta > 0)
    {
        func_switching(FUNC_SWITCH_FADE_OUT | FUNC_SWITCH_AUTO, NULL);
        func_cb.sta = func_sta;
        func_cb.menu_idx = icon_idx;                //记住当前编号
    }
}

//切换到时钟
static void func_menu_sub_football_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    func_switching(FUNC_SWITCH_FADE_OUT | FUNC_SWITCH_AUTO, NULL);
    func_cb.sta = func_sta;
    func_cb.menu_idx = 0;
    func_cb.flag_animation = true;
}
#define FOOTBALL_HALF_CIRCUM            ((int)(M_PI * FOOTBALL_RADIUS))     //圆周一半
//主菜单功能事件处理
static void func_menu_sub_football_process(void)
{
#if  0
    f_menu_football_t *f_menu = (f_menu_football_t *)func_cb.f_cb;
    compo_football_t *ball = f_menu->ball;
    s32 dx, dy, ax, ay;
//    printf("touch=%d\n",f_menu->cube_touch);

    if(f_menu->cube_touch == true)
    {
        f_menu->cube_touch = ctp_get_dxy(&dx, &dy);
        //拖动菜单图标
        ax = dx * 1800 / FOOTBALL_HALF_CIRCUM;
        ay = dy * 1800 / FOOTBALL_HALF_CIRCUM;
        f_menu->cube_rp = sqrt64(ax * ax + ay * ay);
        f_menu->cube_ra = ARCTAN2(-ay, ax);
        // printf("1 dx:%d dy:%d\n",dx,dy);
        // printf("1 cube_rp:%d cube_ra:%d\n",f_menu->cube_rp,f_menu->cube_ra);
        compo_football_move(ball);

        if(f_menu->cube_touch)
        {
            f_menu->cube_touch_time = 5;
        }
        else
        {
            f_menu->cube_touch_time = 2000;
        }
    }
    else
    {
        // printf("2 cube_rp:%d cube_ra:%d\n",f_menu->cube_rp,f_menu->cube_ra);
        if(tick_check_expire(f_menu->tick, f_menu->cube_touch_time))
        {
            f_menu->cube_touch_time = 5;
            f_menu->tick = tick_get();
            f_menu->cube_rp+=2;
            if(f_menu->cube_rp == 1800) f_menu->cube_rp+=2;
            if(f_menu->cube_rp>3600)f_menu->cube_rp=0;
            compo_football_roll_from(ball,f_menu->cube_rp, f_menu->cube_ra);
            compo_football_update(ball);
        }
    }
    func_process();
#endif
    f_menu_football_t *f_menu = (f_menu_football_t *)func_cb.f_cb;
    compo_football_t *ball = f_menu->ball;
    compo_football_move(ball);


    func_process();
}

//拖动过程中，只响应部分消息
static void func_menu_sub_football_drag_message(size_msg_t msg)
{
    switch (msg)
    {
        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_menu_sub_football_switch_to_clock();       //返回时钟表盘界面
            }
            break;
        default:
            evt_message(msg);
            break;
    }
}

//自动移动中，只响应部分消息
static void func_menu_sub_football_move_message(size_msg_t msg)
{
    f_menu_football_t *f_menu = (f_menu_football_t *)func_cb.f_cb;
    compo_football_t *ball = f_menu->ball;

    switch (msg)
    {
        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_menu_sub_football_switch_to_clock();       //返回时钟表盘界面
            }
            break;
        case MSG_CTP_TOUCH:
            //移动过程中，触屏停止。重新进入到开始拖动模式
            compo_football_move_control(ball, COMPO_FOOTBALL_MOVE_CMD_DRAG);
            break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
            compo_football_move_control(ball, COMPO_FOOTBALL_MOVE_CMD_FORWARD);
            break;

        case MSG_QDEC_BACKWARD:                             //向后滚动菜单
            compo_football_move_control(ball, COMPO_FOOTBALL_MOVE_CMD_BACKWARD);
            break;

        default:
            func_menu_sub_message(msg);
            break;
    }
}

//正常模式下
static void func_menu_sub_football_normal_message(size_msg_t msg)
{
    f_menu_football_t *f_menu = (f_menu_football_t *)func_cb.f_cb;
    compo_football_t *ball = f_menu->ball;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_menu_sub_disk_icon_click();                //单击图标
            break;

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_SHORT_DOWN:
            compo_football_move_control(ball, COMPO_FOOTBALL_MOVE_CMD_DRAG);
            break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
            compo_football_move_control(ball, COMPO_FOOTBALL_MOVE_CMD_FORWARD);
            break;

        case MSG_QDEC_BACKWARD:                             //向后滚动菜单
            compo_football_move_control(ball, COMPO_FOOTBALL_MOVE_CMD_BACKWARD);
            break;

//        case KU_DELAY_BACK:
        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_menu_sub_football_switch_to_clock();       //返回时钟表盘界面
            }
            break;

        default:
            func_menu_sub_message(msg);
            break;
    }
}

//主菜单功能消息处理
static void func_menu_sub_football_message(size_msg_t msg)
{
    f_menu_football_t *f_menu = (f_menu_football_t *)func_cb.f_cb;

    compo_football_t *ball = f_menu->ball;
    u8 sta = compo_football_get_sta(ball);

    // if(MSG_SYS_1S  == msg && f_menu->touch_time_flag!=2)
    // {
    //     GLOBAL_INT_RESTORE();
    //     if(++f_menu->touch_time_flag)
    //     {
    //         GLOBAL_INT_DISABLE();
    //     }
    // }

    // if (MSG_CTP_TOUCH == msg && f_menu->touch_time_flag ==2)
    // {
    //     f_menu->cube_touch = true;
    // }
    if (MSG_CTP_TOUCH == msg)
    {
        f_menu->cube_touch = true;
    }


    switch (sta)
    {
        case COMPO_FOOTBALL_STA_IDLE:
            //正常模式下
            func_menu_sub_football_normal_message(msg);
            break;

        case COMPO_FOOTBALL_STA_DARG:
            //拖动过程中，只响应部分消息
            func_menu_sub_football_drag_message(msg);
            break;

        case COMPO_FOOTBALL_STA_MOVE:
            //自动移动中，只响应部分消息
            func_menu_sub_football_move_message(msg);
            break;

        default:
            halt(HALT_GUI_COMPO_FOOTBALL_MOVE_STA);
            break;
    }
}

//进入主菜单功能
static void func_menu_sub_football_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_menu_football_t));
    func_cb.frm_main = func_menu_sub_football_form_create();

    tft_set_temode(false);
    f_menu_football_t *f_menu = (f_menu_football_t *)func_cb.f_cb;
    f_menu->ball = compo_getobj_byid(COMPO_ID_FOOTBALL);
    compo_football_t *ball = f_menu->ball;
    if (ball->type != COMPO_TYPE_FOOTBALL)
    {
        halt(HALT_GUI_COMPO_FOOTBALL_TYPE);
    }
    func_cb.enter_tick = tick_get();
    f_menu->cube_ra = 900;
    f_menu->cube_rp = 900;
    f_menu->cube_touch = false;
    f_menu->cube_touch_time = 5;
}

//主菜单功能
void func_menu_sub_football(void)
{
    printf("%s\n", __func__);
    f_menu_football_t *f_menu = (f_menu_football_t *)func_cb.f_cb;
    func_menu_sub_football_enter();
    while (func_cb.sta == FUNC_MENU && func_cb.menu_style == MENU_STYLE_FOOTBALL)
    {

        func_menu_sub_football_process();
        func_menu_sub_football_message(msg_dequeue());

    }
    func_menu_sub_exit();
}
