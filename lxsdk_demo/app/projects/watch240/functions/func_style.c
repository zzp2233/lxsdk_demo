#include "include.h"
#include "func.h"
#include "func_menu.h"
#include "ute_module_gui_common.h"

#define TRACE_EN    1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_STYLE_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define SET_LIST_CNT                       ((int)(sizeof(tbl_style_list) / sizeof(tbl_style_list[0])))

u8 func_menu_sub_skyrer_get_first_idx(void);

enum
{
    COMPO_ID_LISTBOX = 1,
};

//风格列表tbl
static const compo_listbox_item_t tbl_style_list[] =
{
    {STR_STYLE_LIST_1,          UI_BUF_I330001_SETTINGS_THEME_03_BIN,           .menu_style = MENU_STYLE_LIST},             //列表
    {STR_GONG_GE,               UI_BUF_I330001_SETTINGS_THEME_02_BIN,           .menu_style = MENU_STYLE_CUM_SUDOKU},       //宫格
    {STR_STYLE_HONEYCOMB,       UI_BUF_I330001_SETTINGS_THEME_00_BIN,           .menu_style = MENU_STYLE_HONEYCOMB},        //蜂窝
    // {STR_SPHERE,                UI_BUF_I330001_SETTINGS_THEME_07_BIN,           .menu_style = MENU_STYLE_FOOTBALL},         //球体
    {STR_CHECKERBOARD,          UI_BUF_I330001_SETTINGS_THEME_06_BIN,           .menu_style = MENU_STYLE_GRID},             //棋盘
    {STR_HALO,                  UI_BUF_I330001_SETTINGS_THEME_01_BIN,           .menu_style = MENU_STYLE_KALE},             //光环
    {STR_STYLE_SKYRER,          UI_BUF_I330001_SETTINGS_THEME_08_BIN,           .menu_style = MENU_STYLE_SKYRER},           //天圆地方
    //{STR_STYLE_GRID_1,          UI_BUF_I330001_SETTINGS_THEME_09_BIN,           .menu_style = MENU_STYLE_CUM_GRID},         //网格
    {STR_SIX_PALACE_GRID,       UI_BUF_I330001_SETTINGS_THEME_04_BIN,           .menu_style = MENU_STYLE_CUM_FOURGRID},     //六宫格
    {STR_STYLE_WATERFALL,       UI_BUF_I330001_SETTINGS_THEME_05_BIN,           .menu_style = MENU_STYLE_WATERFALL},        //瀑布
//    {STR_STYLE_SUDOKU_1,        UI_BUF_STYLE_01_BIN,           .menu_style = MENU_STYLE_SUDOKU},           //九宫格
//    //{STR_STYLE_SUDOKU_2,        UI_BUF_STYLE_02_BIN,           .menu_style = MENU_STYLE_SUDOKU_HRZ},       //九宫格(横向)
//    {STR_STYLE_DISK,            UI_BUF_STYLE_13_BIN,           .menu_style = MENU_STYLE_DISK},             //圆盘
//    {STR_STYLE_GRID_1,          UI_BUF_STYLE_02_BIN,           .menu_style = MENU_STYLE_GRID},             //网格
//    {STR_STYLE_HEXAGON,         UI_BUF_STYLE_05_BIN,           .menu_style = MENU_STYLE_CUM_HEXAGON},      //六边形
};

typedef struct f_style_t_
{
    compo_listbox_t *listbox;
    u8 sel_idx;
    bool flags;
} f_style_t;


u8 func_sel_style_bit(uint n)
{
    //TRACE("%s -> [%d,%d]\n", __func__, n, func_cb.menu_style);
    if (func_cb.menu_style == n)
    {
        return true;
    }

    return false;
}


//创建菜单风格窗体
compo_form_t *func_style_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STYLE]);

    //新建菜单选择列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_style_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I330001_PUBLIC_GOU_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_style_bit);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    uint8_t set_idx = 0;//sys_cb.set_idx;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入对应的菜单风格
static void func_set_sub_list_icon_click(void)
{
    int icon_idx;
    f_style_t *f_set = (f_style_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 menu_style;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    menu_style = tbl_style_list[icon_idx].menu_style;

    //切换风格
    if (menu_style >= 0)
    {
        func_cb.menu_style = menu_style;
        if (func_cb.menu_style == MENU_STYLE_SKYRER)
        {
            func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();
        }
        else
        {
            func_cb.menu_idx = 0;           //切换风格后进入回中心位置
        }
        sys_cb.set_idx = listbox->focus_icon_idx;
        uteModuleGuiCommonSetThemeTypeId(func_cb.menu_style);
    }
}

//切换到设置时钟页面
static void func_set_sub_list_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    f_style_t *f_set = (f_style_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    sys_cb.set_idx = 0;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define SET_LIST_CNT                       ((int)(sizeof(tbl_style_list) / sizeof(tbl_style_list[0])))

u8 func_menu_sub_skyrer_get_first_idx(void);

enum
{
    COMPO_ID_LISTBOX = 1,
};

//风格列表tbl
static const compo_listbox_item_t tbl_style_list[] =
{
    {STR_STYLE_LIST_1,          UI_BUF_I332001_SETTINGS_THEME_03_BIN,           .menu_style = MENU_STYLE_LIST},             //列表
    {STR_GONG_GE,               UI_BUF_I332001_SETTINGS_THEME_02_BIN,           .menu_style = MENU_STYLE_CUM_SUDOKU},       //宫格
    {STR_STYLE_HONEYCOMB,       UI_BUF_I332001_SETTINGS_THEME_00_BIN,           .menu_style = MENU_STYLE_HONEYCOMB},        //蜂窝
    {STR_SPHERE,                UI_BUF_I332001_SETTINGS_THEME_07_BIN,           .menu_style = MENU_STYLE_FOOTBALL},         //球体
    {STR_CHECKERBOARD,          UI_BUF_I332001_SETTINGS_THEME_06_BIN,           .menu_style = MENU_STYLE_GRID},             //棋盘
    {STR_HALO,                  UI_BUF_I332001_SETTINGS_THEME_01_BIN,           .menu_style = MENU_STYLE_KALE},             //光环
    {STR_STYLE_SKYRER,          UI_BUF_I332001_SETTINGS_THEME_08_BIN,           .menu_style = MENU_STYLE_SKYRER},           //天圆地方
    {STR_STYLE_GRID_1,          UI_BUF_I332001_SETTINGS_THEME_09_BIN,           .menu_style = MENU_STYLE_CUM_GRID},         //网格
    {STR_SIX_PALACE_GRID,       UI_BUF_I332001_SETTINGS_THEME_04_BIN,           .menu_style = MENU_STYLE_CUM_FOURGRID},     //六宫格
    {STR_STYLE_WATERFALL,       UI_BUF_I332001_SETTINGS_THEME_05_BIN,           .menu_style = MENU_STYLE_WATERFALL},        //瀑布
//    {STR_STYLE_SUDOKU_1,        UI_BUF_STYLE_01_BIN,           .menu_style = MENU_STYLE_SUDOKU},           //九宫格
//    //{STR_STYLE_SUDOKU_2,        UI_BUF_STYLE_02_BIN,           .menu_style = MENU_STYLE_SUDOKU_HRZ},       //九宫格(横向)
//    {STR_STYLE_DISK,            UI_BUF_STYLE_13_BIN,           .menu_style = MENU_STYLE_DISK},             //圆盘
//    {STR_STYLE_GRID_1,          UI_BUF_STYLE_02_BIN,           .menu_style = MENU_STYLE_GRID},             //网格
//    {STR_STYLE_HEXAGON,         UI_BUF_STYLE_05_BIN,           .menu_style = MENU_STYLE_CUM_HEXAGON},      //六边形
};

typedef struct f_style_t_
{
    compo_listbox_t *listbox;
    u8 sel_idx;
    bool flags;
} f_style_t;


u8 func_sel_style_bit(uint n)
{
    //TRACE("%s -> [%d,%d]\n", __func__, n, func_cb.menu_style);
    if (func_cb.menu_style == n)
    {
        return true;
    }

    return false;
}
//创建菜单风格窗体
compo_form_t *func_style_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STYLE]);

    //新建菜单选择列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_style_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I332001_PUBLIC_GOU_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_style_bit);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    uint8_t set_idx = 0;//sys_cb.set_idx;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入对应的菜单风格
static void func_set_sub_list_icon_click(void)
{
    int icon_idx;
    f_style_t *f_set = (f_style_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 menu_style;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    menu_style = tbl_style_list[icon_idx].menu_style;

    //切换风格
    if (menu_style >= 0)
    {
        func_cb.menu_style = menu_style;
        if (func_cb.menu_style == MENU_STYLE_SKYRER)
        {
            func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();
        }
        else
        {
            func_cb.menu_idx = 0;           //切换风格后进入回中心位置
        }
        sys_cb.set_idx = listbox->focus_icon_idx;
        uteModuleGuiCommonSetThemeTypeId(func_cb.menu_style);
    }
}

//切换到设置时钟页面
static void func_set_sub_list_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    f_style_t *f_set = (f_style_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    sys_cb.set_idx = 0;
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT


//菜单风格功能事件处理
static void func_style_process(void)
{
    f_style_t *f_set = (f_style_t *)func_cb.f_cb;
    compo_listbox_move(f_set->listbox);
    compo_listbox_update(f_set->listbox);
    func_process();
}

//菜单风格功能消息处理
static void func_style_message(size_msg_t msg)
{
    f_style_t *f_set = (f_style_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_set_sub_list_icon_click();                //单击图标
            func_switch_to_menu();                      //退回到主菜单
            break;

        case MSG_CTP_LONG:
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_message(msg);
            sys_cb.set_idx = 0;
            break;

        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_set_sub_list_switch_to_clock();       //返回时钟
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入菜单风格功能
static void func_style_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_style_t));
    func_cb.frm_main = func_style_form_create();

    f_style_t *f_set = (f_style_t *)func_cb.f_cb;
    f_set->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2));
    func_cb.enter_tick = tick_get();
}

//退出菜单风格功能
static void func_style_exit(void)
{
    printf("%s exit\n", __func__);
    f_style_t *f_set = (f_style_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    func_free(listbox->mcb);
    func_cb.last = FUNC_STYLE;
}

//菜单风格功能
void func_style(void)
{
    printf("%s\n", __func__);
    func_style_enter();
    while (func_cb.sta == FUNC_STYLE)
    {
        func_style_process();
        func_style_message(msg_dequeue());
    }
    func_style_exit();
}

#endif
