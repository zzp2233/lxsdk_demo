#include "include.h"
#include "func.h"
#include "func_menu.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define FOCUS_AUTO_TICK_EXPIRE              18                  //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                     12                  //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV                 8
#define FOCUS_AUTO_EXCESS                   4                   //回弹延迟距离

#define DRAG_AUTO_SPEED                     12                  //拖动松手后的速度(惯性)

#define ENTERING_ANIMATION_MAX              (GUI_SCREEN_HEIGHT * 9 / 6)
#define ENTERING_ANIMATION_CNT              30
#define ANIMATION_TICK_EXPIRE               18

#define MENU_SUD_CNT                         ((int)((sizeof(tbl_menu_cum_sudoku) / sizeof(tbl_menu_cum_sudoku[0]))))

enum
{
    COMPO_ID_ICONLIST = 1,
};

typedef struct f_menu_cum_sudoku_t_
{
    compo_iconlist_t *iconlist;
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    u32 tick;
    s32 focus_x;
    s32 focus_y;
    s32 focus_dx;
    s32 focus_dy;
    s32 focus_xstep;
    s32 focus_ystep;
    int moveto_ix;                  //设定自动移到的目标菜单ix
    int moveto_iy;                  //设置自动移到的目标菜单iy
    point_t moveto;                 //设定自动移到的坐标

    int animation_cnt;
} f_menu_cum_sudoku_t;

typedef struct menu_hc_item_t_
{
    u32 res_addr;
    u8 func_sta;
} menu_hc_item_t;


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//瀑布流图标列表及顺序
const menu_hc_item_t tbl_menu_cum_sudoku[] =
{
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {UI_BUF_I330001_THEME_1_HEART_BIN,                FUNC_HEARTRATE},
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {UI_BUF_I330001_THEME_1_ACTIVITY_BIN,                  FUNC_ACTIVITY},
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {UI_BUF_I330001_THEME_1_SPO2_BIN,              FUNC_BLOOD_OXYGEN},
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT

#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {UI_BUF_I330001_THEME_1_MESSAGE_BIN,                   FUNC_MESSAGE},
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
//    {UI_BUF_ICON_BLOODSUGAR_BIN,                FUNC_BLOODSUGAR},
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {UI_BUF_I330001_THEME_1_BREATHE_BIN,                   FUNC_BREATHE},
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {UI_BUF_I330001_THEME_1_MUSIC_BIN,                     FUNC_BT},
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {UI_BUF_I330001_THEME_1_CALL_BIN,                      FUNC_CALL},
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
//    {UI_BUF_ICON_CLOCK_BG_BIN,                  FUNC_CLOCK},
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {UI_BUF_I330001_THEME_1_THEME_BIN,                      FUNC_STYLE},
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {UI_BUF_I330001_THEME_1_FLASHLIGHT_BIN,                FUNC_FLASHLIGHT},
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT

    {UI_BUF_I330001_THEME_1_FINDPHONE_BIN,                 FUNC_FINDPHONE},
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {UI_BUF_I330001_THEME_1_GAME_BIN,                      FUNC_GAME},
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
//    {UI_BUF_ICON_PRESSURE_BIN,                  FUNC_PRESSURE},//压力
    ////{UI_BUF_ICON_ALTITUDE_BIN,                  FUNC_ALTITUDE},

#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {UI_BUF_I330001_THEME_1_ALARM_BIN,               FUNC_ALARM_CLOCK},
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
    {UI_BUF_I330001_THEME_1_RETRY_BIN,           FUNC_RSTFY},
    {UI_BUF_I330001_THEME_1_LANUAGE_BIN,                  FUNC_LANGUAGE},
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {UI_BUF_I330001_THEME_1_BRIGHTNESS_BIN,                     FUNC_LIGHT},
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
    ////{UI_BUF_ICON_MAP_BIN,                       FUNC_MAP},
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {UI_BUF_I330001_THEME_1_CALCULATOR_BIN,                FUNC_CALCULATOR},
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
//    {UI_BUF_ICON_OFF_BIN,                       FUNC_OFF},
    {UI_BUF_I330001_THEME_1_BOOT_BIN,                   FUNC_RESTART},
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {UI_BUF_I330001_THEME_1_CALENDAR_BIN,                  FUNC_CALENDAER},
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    {UI_BUF_I330001_THEME_1_QR_BIN,                      FUNC_SCAN},
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {UI_BUF_I330001_THEME_1_SETTINGS_BIN,                   FUNC_SETTING},
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT

#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {UI_BUF_I330001_THEME_1_SLEEP_BIN,                     FUNC_SLEEP},
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
//    {UI_BUF_I330001_THEME_1_SOUND_BIN,                    FUNC_SET_SUB_SAV},
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {UI_BUF_I330001_THEME_1_SOPRT_BIN,                         FUNC_SPORT},
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {UI_BUF_I330001_THEME_1_STOPWATCH_BIN,                 FUNC_STOPWATCH},
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {UI_BUF_I330001_THEME_1_PHOTO_BIN,                    FUNC_CAMERA},
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {UI_BUF_I330001_THEME_1_TIMER_BIN,                     FUNC_TIMER},
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
    {UI_BUF_I330001_THEME_1_CONTACTS_BIN,              FUNC_ADDRESS_BOOK},
#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {UI_BUF_I330001_THEME_1_VIOCE_BIN,                     FUNC_VOICE},
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {UI_BUF_I330001_THEME_1_WEATHER_BIN,                   FUNC_WEATHER},
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT

    //{UI_BUF_ICON_BLOOD_PRESSURE_BIN,            FUNC_BLOOD_PRESSURE},
    ////{UI_BUF_ICON_COMPASS_BIN,                   FUNC_COMPASS},
};
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
const menu_hc_item_t tbl_menu_cum_sudoku[] =
{
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_HEART_BIN,                FUNC_HEARTRATE},
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_ACTIVITY_BIN,                  FUNC_ACTIVITY},
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_SPO2_BIN,              FUNC_BLOOD_OXYGEN},
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT

#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_MESSAGE_BIN,                   FUNC_MESSAGE},
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
//    {UI_BUF_ICON_BLOODSUGAR_BIN,                FUNC_BLOODSUGAR},
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_BREATHE_BIN,                   FUNC_BREATHE},
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_MUSIC_BIN,                     FUNC_BT},
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_CALL_BIN,                      FUNC_CALL},
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
//    {UI_BUF_ICON_CLOCK_BG_BIN,                  FUNC_CLOCK},
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_THEME_BIN,                      FUNC_STYLE},
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_FLASHLIGHT_BIN,                FUNC_FLASHLIGHT},
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT

    {UI_BUF_I332001_THEME_ICON1_FINDPHONE_BIN,                 FUNC_FINDPHONE},
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_GAME_BIN,                      FUNC_GAME},
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
//    {UI_BUF_ICON_PRESSURE_BIN,                  FUNC_PRESSURE},//压力
    ////{UI_BUF_ICON_ALTITUDE_BIN,                  FUNC_ALTITUDE},

#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_ALARM_BIN,               FUNC_ALARM_CLOCK},
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_RETRY_BIN,           FUNC_RSTFY},
    {UI_BUF_I332001_THEME_ICON1_LANUAGE_BIN,                  FUNC_LANGUAGE},
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_BRIGHTNESS_BIN,                     FUNC_LIGHT},
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
    ////{UI_BUF_ICON_MAP_BIN,                       FUNC_MAP},
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_CALCULATOR_BIN,                FUNC_CALCULATOR},
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
//    {UI_BUF_ICON_OFF_BIN,                       FUNC_OFF},
    {UI_BUF_I332001_THEME_ICON1_BOOT_BIN,                   FUNC_RESTART},
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_CALENDAR_BIN,                  FUNC_CALENDAER},
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_QR_BIN,                      FUNC_SCAN},
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_SETTINGS_BIN,                   FUNC_SETTING},
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT

#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_SLEEP_BIN,                     FUNC_SLEEP},
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
//    {UI_BUF_I332001_THEME_ICON1_SOUND_BIN,                    FUNC_SET_SUB_SAV},
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_SOPRT_BIN,                         FUNC_SPORT},
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_STOPWATCH_BIN,                 FUNC_STOPWATCH},
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_PHOTO_BIN,                    FUNC_CAMERA},
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_TIMER_BIN,                     FUNC_TIMER},
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_CONTACTS_BIN,              FUNC_ADDRESS_BOOK},
#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_VIOCE_BIN,                     FUNC_VOICE},
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {UI_BUF_I332001_THEME_ICON1_WEATHER_BIN,                   FUNC_WEATHER},
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT

    //{UI_BUF_ICON_BLOOD_PRESSURE_BIN,            FUNC_BLOOD_PRESSURE},
    ////{UI_BUF_ICON_COMPASS_BIN,                   FUNC_COMPASS},
};
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_sub_cum_sudoku_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(false);       //菜单一般创建在底层

    //新建九宫格效果
    compo_iconlist_t *iconlist;
    iconlist = compo_iconlist_create(frm, ICONLIST_STYLE_CUM_SUDOKU);
    compo_setid(iconlist, COMPO_ID_ICONLIST);
    for (int i=0; i<MENU_SUD_CNT; i++)
    {
        compo_iconlist_add(iconlist, tbl_menu_cum_sudoku[i].res_addr);
    }

//    compo_iconlist_set_time_idx(iconlist, 7);
//    compo_iconlist_add_time(iconlist, COMPO_ICONLIST_TIME_TYPE_HOUR, UI_BUF_ICON_CLOCK_H_BIN, 1, 2);
//    compo_iconlist_add_time(iconlist, COMPO_ICONLIST_TIME_TYPE_MIN, UI_BUF_ICON_CLOCK_M_BIN, 1, 2);
//    compo_iconlist_add_time(iconlist, COMPO_ICONLIST_TIME_TYPE_SEC, UI_BUF_ICON_CLOCK_S_BIN, 9, 2);
//    compo_iconlist_set_start_angle(iconlist, 900);

    if (func_cb.menu_idx < 7)
    {
        func_cb.menu_idx = 7;
    }
    compo_iconlist_set_focus_byidx(iconlist, func_cb.menu_idx);
    compo_iconlist_update(iconlist);

    if (func_cb.flag_animation)
    {
        widget_set_visible(iconlist->page,false);
    }
    return frm;
}

//点进图标进入应用
static void func_menu_sub_cum_sudoku_icon_click(void)
{
    point_t pt;
    int icon_idx;
    f_menu_cum_sudoku_t *f_menu = (f_menu_cum_sudoku_t *)func_cb.f_cb;
    compo_iconlist_t *iconlist = f_menu->iconlist;
    u8 func_sta;

    pt = ctp_get_sxy();
    icon_idx = compo_iconlist_select(iconlist, pt.x, pt.y);
    if (icon_idx < 0 || icon_idx >= MENU_SUD_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_menu_cum_sudoku[icon_idx].func_sta;

    //切入应用
    if (func_sta > 0)
    {
        compo_form_t *frm = func_create_form(func_sta);
        func_switching(FUNC_SWITCH_ZOOM_ENTER | FUNC_SWITCH_AUTO, iconlist->sel_icon);
        compo_form_destroy(frm);
        func_cb.sta = func_sta;
        func_cb.menu_idx = icon_idx;                //记住当前编号
    }
}

//切换到时钟
static void func_menu_sub_cum_sudoku_switch_to_clock(void)
{
    f_menu_cum_sudoku_t *f_menu = (f_menu_cum_sudoku_t *)func_cb.f_cb;
    compo_iconlist_t *iconlist = f_menu->iconlist;
    widget_icon_t *icon = compo_iconlist_select_byidx(iconlist, 0);
    u8 func_sta = FUNC_CLOCK;
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    func_cb.menu_idx = 0;
    func_cb.flag_animation = true;
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

static void func_menu_sub_cum_sudoku_entering(void)
{
    f_menu_cum_sudoku_t *f_menu = (f_menu_cum_sudoku_t *)func_cb.f_cb;
    compo_iconlist_t *iconlist = f_menu->iconlist;
    int idx = 0;
    int y_step = ENTERING_ANIMATION_MAX / ENTERING_ANIMATION_CNT;
    widget_set_visible(iconlist->page,true);
    compo_iconlist_update(iconlist);
    widget_t *widget = widget_get_next(iconlist->page);
    while (widget != NULL)
    {
        if (widget_is_icon(widget) && widget_get_parent(widget) == iconlist->page)
        {
            int ln = idx/3;
            rect_t rect = widget_get_location(widget);

            if (ln == 0)    //基础页面跳过
            {

            }
            else
            {
                int y = rect.y + y_step * f_menu->animation_cnt - tbl_entering_start_pos[ln];
                if (y > rect.y)
                {
                    int w_rate = 0x10000 + 0x20000 * (y - rect.y) / (y_step * ENTERING_ANIMATION_CNT - tbl_entering_start_pos[ln]);
                    rect.y = y;
                    rect.wid = muls_shift16(rect.wid, w_rate);
                    rect.hei = muls_shift16(rect.hei, w_rate);
                }
            }
            if (iconlist->page_time != NULL)
            {
                if (idx == iconlist->idx_time)
                {
                    widget_page_scale_to(iconlist->page_time, rect.wid, rect.hei);
                    widget_set_pos(iconlist->page_time, rect.x, rect.y);
                }
            }
            idx++;
            if (idx > 15)
            {
                return;
            }

            widget_set_pos(widget, rect.x, rect.y);
            //widget_page_scale_to(widget, GUI_SCREEN_WIDTH, rect.hei);
        }
        //遍历Widget
        widget = widget_get_next(widget);
    }
}

//主菜单功能事件处理
static void func_menu_sub_cum_sudoku_process(void)
{
    f_menu_cum_sudoku_t *f_menu = (f_menu_cum_sudoku_t *)func_cb.f_cb;
    compo_iconlist_t *iconlist = f_menu->iconlist;
    if (f_menu->animation_cnt > 0)
    {
        //入场动画
        if (tick_check_expire(f_menu->tick, ANIMATION_TICK_EXPIRE))
        {
            func_menu_sub_cum_sudoku_entering();
            f_menu->tick = tick_get();
            f_menu->animation_cnt--;
        }
    }
    else if (f_menu->flag_drag)
    {
        s32 dx = f_menu->focus_dx;
        s32 dy = f_menu->focus_dy;
        f_menu->flag_drag = ctp_get_dxy(&f_menu->focus_dx, &f_menu->focus_dy);
        if (f_menu->flag_drag)
        {
            //拖动菜单图标
            f_menu->focus_xstep = f_menu->focus_dx - dx;
            f_menu->focus_ystep = f_menu->focus_dy - dy;
            compo_iconlist_set_focus(iconlist, f_menu->focus_x - f_menu->focus_dx, f_menu->focus_y - f_menu->focus_dy);
            compo_iconlist_update(iconlist);
        }
        else
        {
            //抬手后开始自动移动
            point_t last_dxy = ctp_get_last_dxy();
            s32 to_x;
            s32 to_y;
            f_menu->focus_x = iconlist->ofs_x;
            f_menu->focus_y = iconlist->ofs_y;
            f_menu->flag_move_auto = true;
            to_x = f_menu->focus_x - (last_dxy.x * DRAG_AUTO_SPEED);
            to_y = f_menu->focus_y - (last_dxy.y * DRAG_AUTO_SPEED);
            f_menu->moveto = compo_iconlist_align_xy(iconlist, to_x, to_y);
            f_menu->tick = tick_get();
        }
    }
    else if (f_menu->flag_move_auto)
    {
        //自动移动
        if (f_menu->focus_x == f_menu->moveto.x && f_menu->focus_y == f_menu->moveto.y)
        {
            if (f_menu->focus_y < 0)
            {
                f_menu->moveto.y = 0;
            }
            else if (f_menu->focus_y > iconlist->ofs_max)
            {
                f_menu->moveto.y = iconlist->ofs_max;
            }
            else
            {
                f_menu->flag_move_auto = false;            //移动完成
            }
        }
        else if (tick_check_expire(f_menu->tick, FOCUS_AUTO_TICK_EXPIRE))
        {
            TRACE("[%d %d]--move_to->[%d %d]\n", f_menu->focus_x, f_menu->focus_y, f_menu->moveto.x, f_menu->moveto.y);
            s16 dx, dy;
            f_menu->tick = tick_get();
            dx = f_menu->moveto.x - f_menu->focus_x;
            dy = f_menu->moveto.y - f_menu->focus_y;
            if (dx > 0)
            {
                if (dx > FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dx = dx / FOCUS_AUTO_STEP_DIV;
                }
                else if (dx > FOCUS_AUTO_STEP)
                {
                    dx = FOCUS_AUTO_STEP;
                }
                else
                {
                    dx = 1;
                }
            }
            else if (dx < 0)
            {
                if (dx < -FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dx = dx / FOCUS_AUTO_STEP_DIV;
                }
                else if (dx < -FOCUS_AUTO_STEP)
                {
                    dx = -FOCUS_AUTO_STEP;
                }
                else
                {
                    dx = -1;
                }
            }
            if (dy > 0)
            {
                if (dy > FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy > FOCUS_AUTO_STEP)
                {
                    dy = FOCUS_AUTO_STEP;
                }
                else
                {
                    dy = 1;
                }
            }
            else if (dy < 0)
            {
                if (dy < -FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy < -FOCUS_AUTO_STEP)
                {
                    dy = -FOCUS_AUTO_STEP;
                }
                else
                {
                    dy = -1;
                }
            }
            f_menu->focus_x += dx;
            f_menu->focus_y += dy;
            compo_iconlist_set_focus(iconlist, f_menu->focus_x, f_menu->focus_y);
            compo_iconlist_update(iconlist);
        }
    }
    func_process();
}

static void func_menu_sub_cum_sudoku_entering_message(size_msg_t msg)
{
    switch (msg)
    {
        default:
            evt_message(msg);
            break;
    }
}

//拖动过程中，只响应部分消息
static void func_menu_sub_cum_sudoku_drag_message(size_msg_t msg)
{
    switch (msg)
    {
        default:
            evt_message(msg);
            break;
    }
}

//自动移动中，只响应部分消息
static void func_menu_sub_cum_sudoku_move_auto_message(size_msg_t msg)
{
    int to_x, to_y;
    f_menu_cum_sudoku_t *f_menu = (f_menu_cum_sudoku_t *)func_cb.f_cb;
    compo_iconlist_t *iconlist = f_menu->iconlist;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            f_menu->flag_drag = true;                               //移动过程中，触屏停止
            f_menu->flag_move_auto = false;
            break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
            f_menu->flag_move_auto = true;
            to_x = f_menu->moveto.x + iconlist->ln_hei;
            to_y = f_menu->moveto.y + iconlist->ln_hei;
            f_menu->moveto = compo_iconlist_align_xy(iconlist, to_x, to_y);
            break;

        case MSG_QDEC_BACKWARD:
            f_menu->flag_move_auto = true;
            to_x = f_menu->moveto.x - iconlist->ln_hei;
            to_y = f_menu->moveto.y - iconlist->ln_hei;
            f_menu->moveto = compo_iconlist_align_xy(iconlist, to_x, to_y);
            break;

        default:
            evt_message(msg);
            break;
    }
}

//正常模式下
static void func_menu_sub_cum_sudoku_normal_message(size_msg_t msg)
{
    int to_x, to_y;
    f_menu_cum_sudoku_t *f_menu = (f_menu_cum_sudoku_t *)func_cb.f_cb;
    compo_iconlist_t *iconlist = f_menu->iconlist;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_menu_sub_cum_sudoku_icon_click();           //单击图标
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
            f_menu->flag_drag = true;                       //上下短划，开启拖动
            f_menu->flag_move_auto = false;
            break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
            f_menu->flag_move_auto = true;
            to_x = f_menu->moveto.x + iconlist->ln_hei;
            to_y = f_menu->moveto.y + iconlist->ln_hei;
            f_menu->moveto = compo_iconlist_align_xy(iconlist, to_x, to_y);
            break;

        case MSG_QDEC_BACKWARD:
            f_menu->flag_move_auto = true;
            to_x = f_menu->moveto.x - iconlist->ln_hei;
            to_y = f_menu->moveto.y - iconlist->ln_hei;
            f_menu->moveto = compo_iconlist_align_xy(iconlist, to_x, to_y);
            break;

        case MSG_CTP_LONG:
            break;

        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_menu_sub_cum_sudoku_switch_to_clock();      //返回时钟表盘界面
            }
            break;

        default:
            func_menu_sub_message(msg);
            break;
    }
}

//主菜单功能消息处理
static void func_menu_sub_cum_sudoku_message(size_msg_t msg)
{
    f_menu_cum_sudoku_t *f_menu = (f_menu_cum_sudoku_t *)func_cb.f_cb;
    if (f_menu->animation_cnt > 0)
    {
        func_menu_sub_cum_sudoku_entering_message(msg);
    }
    else if (f_menu->flag_drag)
    {
        //拖动过程中，只响应部分消息
        func_menu_sub_cum_sudoku_drag_message(msg);
    }
    else if (f_menu->flag_move_auto)
    {
        //自动移动中，只响应部分消息
        func_menu_sub_cum_sudoku_move_auto_message(msg);
    }
    else
    {
        //正常模式下
        func_menu_sub_cum_sudoku_normal_message(msg);
    }
}

//进入主菜单功能
static void func_menu_sub_cum_sudoku_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_menu_cum_sudoku_t));
    func_cb.frm_main = func_menu_sub_cum_sudoku_form_create();

    f_menu_cum_sudoku_t *f_menu = (f_menu_cum_sudoku_t *)func_cb.f_cb;
    f_menu->iconlist = compo_getobj_byid(COMPO_ID_ICONLIST);
    compo_iconlist_t *iconlist = f_menu->iconlist;
    if (iconlist->type != COMPO_TYPE_ICONLIST)
    {
        halt(HALT_GUI_COMPO_ICONLIST_TYPE);
    }

    f_menu->focus_x = iconlist->ofs_x;
    f_menu->focus_y = iconlist->ofs_y;
    f_menu->moveto = compo_iconlist_align_xy(iconlist, 0, f_menu->focus_y);

    func_cb.enter_tick = tick_get();
    f_menu->animation_cnt = 0;

    if (func_cb.flag_animation)
    {
        func_cb.flag_animation = 0;
        f_menu->animation_cnt = ENTERING_ANIMATION_CNT;
        f_menu->tick = tick_get();
    }
}

//主菜单功能
void func_menu_sub_cum_sudoku(void)
{
    printf("%s\n", __func__, func_cb.menu_style);
    func_menu_sub_cum_sudoku_enter();
    while (func_cb.sta == FUNC_MENU && func_cb.menu_style == MENU_STYLE_CUM_SUDOKU)
    {
        func_menu_sub_cum_sudoku_process();
        func_menu_sub_cum_sudoku_message(msg_dequeue());
    }
    func_menu_sub_exit();
}
