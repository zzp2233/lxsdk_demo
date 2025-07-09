#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SETTING_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define SET_LIST_CNT                       ((int)(sizeof(tbl_setting_list) / sizeof(tbl_setting_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_set_list_t_
{
    compo_listbox_t *listbox;

} f_set_list_t;

static const compo_listbox_item_t tbl_setting_list[] =
{
    {STR_DIAL_SWICTH,                    UI_BUF_I330001_SETTINGS_ICON_WATCHFACES_BIN,       .func_sta = FUNC_CLOCK_PREVIEW},    //表盘切换
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {STR_SETTING_LIGHT,                  UI_BUF_I330001_SETTINGS_ICON_RIGHTNESS_BIN,        .func_sta = FUNC_LIGHT},            //亮度调节
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {STR_SETTING_DOUSING,                UI_BUF_I330001_SETTINGS_ICON_TIME_BIN,             .func_sta = FUNC_SET_SUB_DOUSING},  //息屏时长
    {STR_SETTING_PASSWORD,                     UI_BUF_I330001_SETTINGS_ICON_TAIWAN_BIN,           .func_sta = FUNC_SET_SUB_PASSWORD},    //密码
#if GUI_MODULE_WRIST_SUPPORT
    //  {STR_SETTING_UP,                     UI_BUF_I330001_SETTINGS_ICON_TAIWAN_BIN,           .func_sta = FUNC_SET_SUB_WRIST},    //抬腕亮屏
#endif
    {STR_SETTING_DISTURD,                UI_BUF_I330001_SETTINGS_ICON_DND_BIN,              .func_sta = FUNC_SET_SUB_DISTURD},  //勿扰模式
    {STR_VOL,                           UI_BUF_I330001_SETTINGS_ICON_SOUND_BIN,            .func_sta = FUNC_SET_SUB_SAV},       //声音
    {STR_SET_PUFFS,                           UI_BUF_I330001_SETTINGS_ICON_DEVICE_BIN,            .func_sta = FUNC_SET_PUFFS},       //设置口数
#if UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
    {STR_SETTING_LANGUAGE,               UI_BUF_I330001_SETTINGS_ICON_LANUAGE_BIN,          .func_sta = FUNC_SET_SUB_LANGUAGE}, //语言设置
#endif // UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_STYLE,                          UI_BUF_I330001_SETTINGS_ICON_THEME_BIN,            .func_sta = FUNC_STYLE}, //主题
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    //{STR_BEATHER_DURATION,                   UI_BUF_SETTING_TIME_BIN,                       .func_sta = FUNC_SET_SUB_TIME},     //时间设置
//    {STR_SETTING_CALENDAR,               UI_BUF_SETTING_CALENDAR_BIN,                   .func_sta = FUNC_CALENDAER},        //日期设置
//    {STR_SETTING_PASSWORD,               UI_BUF_SETTING_PASSWORD_BIN,                   .func_sta = FUNC_SET_SUB_PASSWORD}, //密码锁
    {STR_SETTING_ABOUT,                  UI_BUF_I330001_SETTINGS_ICON_ABOUT_BIN,            .func_sta = FUNC_SET_SUB_ABOUT},    //关于
    // {STR_SETTING_4G,                     UI_BUF_SETTING_ABOUT_BIN,                      .func_sta = FUNC_SET_SUB_4G},       //4G
//    {STR_SETTING_ABOUT,                  UI_BUF_SETTING_RESTART_BIN,                    .func_sta = FUNC_SET_SUB_SOS},  //SOS
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    {STR_QRCODE,                        UI_BUF_I330001_SETTINGS_ICON_QR_BIN,               .func_sta = FUNC_SCAN},    //二维码
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_RESTART_SUPPORT
    {STR_SETTING_RESTART,               UI_BUF_I330001_SETTINGS_ICON_BOOT_BIN,             .func_sta = FUNC_SET_SUB_RESTART},  //重启
#endif // UTE_MODULE_SCREENS_RESTART_SUPPORT
    {STR_SETTING_OFF,                   UI_BUF_I330001_SETTINGS_ICON_SHUTDOWN_BIN,         .func_sta = FUNC_SET_SUB_OFF},      //关机
#if UTE_MODULE_SCREENS_RESFY_SUPPORT
    {STR_SETTING_RSTFY,                 UI_BUF_I330001_SETTINGS_ICON_RETYR_BIN,            .func_sta = FUNC_SET_SUB_RSTFY},    //恢复出厂
#endif // UTE_MODULE_SCREENS_RESFY_SUPPORT
};

//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_set_sub_list_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_setting_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    u8 set_idx = sys_cb.set_idx;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入应用
static void func_set_sub_list_icon_click(void)
{
    int icon_idx;
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_setting_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {

//        func_switch_to(func_sta, FUNC_SWITCH_ZOOM_FADE | FUNC_SWITCH_AUTO);
        func_cb.sta = func_sta;
        sys_cb.set_idx = listbox->focus_icon_idx;
    }
}

//切换到设置菜单页面
static void func_set_sub_list_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    sys_cb.set_idx = 0;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define SET_LIST_CNT                       ((int)(sizeof(tbl_setting_list) / sizeof(tbl_setting_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_set_list_t_
{
    compo_listbox_t *listbox;

} f_set_list_t;

static const compo_listbox_item_t tbl_setting_list[] =
{
    {STR_DIAL_SWICTH,                    UI_BUF_I332001_SETTINGS_ICON_WATCHFACES_BIN,       .func_sta = FUNC_CLOCK_PREVIEW},    //表盘切换
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {STR_SETTING_LIGHT,                  UI_BUF_I332001_SETTINGS_ICON_RIGHTNESS_BIN,        .func_sta = FUNC_LIGHT},            //亮度调节
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {STR_SETTING_DOUSING,                UI_BUF_I332001_SETTINGS_ICON_TIME_BIN,             .func_sta = FUNC_SET_SUB_DOUSING},  //息屏时长
//    {STR_SETTING_UP,                     UI_BUF_I332001_SETTINGS_ICON_TAIWAN_BIN,           .func_sta = FUNC_SET_SUB_WRIST},    //抬腕亮屏
    {STR_SETTING_DISTURD,                UI_BUF_I332001_SETTINGS_ICON_DND_BIN,              .func_sta = FUNC_SET_SUB_DISTURD},  //勿扰模式
    {STR_VOL,                           UI_BUF_I332001_SETTINGS_ICON_SOUND_BIN,            .func_sta = FUNC_SET_SUB_SAV},       //声音
#if UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
    {STR_SETTING_LANGUAGE,               UI_BUF_I332001_SETTINGS_ICON_LANUAGE_BIN,          .func_sta = FUNC_SET_SUB_LANGUAGE}, //语言设置
#endif // UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_STYLE,                          UI_BUF_I332001_SETTINGS_ICON_THEME_BIN,            .func_sta = FUNC_STYLE}, //主题
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    //{STR_BEATHER_DURATION,                   UI_BUF_SETTING_TIME_BIN,                       .func_sta = FUNC_SET_SUB_TIME},     //时间设置
//    {STR_SETTING_CALENDAR,               UI_BUF_SETTING_CALENDAR_BIN,                   .func_sta = FUNC_CALENDAER},        //日期设置
//    {STR_SETTING_PASSWORD,               UI_BUF_SETTING_PASSWORD_BIN,                   .func_sta = FUNC_SET_SUB_PASSWORD}, //密码锁
    {STR_SETTING_ABOUT,                  UI_BUF_I332001_SETTINGS_ICON_ABOUT_BIN,            .func_sta = FUNC_SET_SUB_ABOUT},    //关于
    // {STR_SETTING_4G,                     UI_BUF_SETTING_ABOUT_BIN,                      .func_sta = FUNC_SET_SUB_4G},       //4G
//    {STR_SETTING_ABOUT,                  UI_BUF_SETTING_RESTART_BIN,                    .func_sta = FUNC_SET_SUB_SOS},  //SOS
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    {STR_QRCODE,                        UI_BUF_I332001_SETTINGS_ICON_QR_BIN,               .func_sta = FUNC_SCAN},    //二维码
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_RESTART_SUPPORT
    {STR_SETTING_RESTART,               UI_BUF_I332001_SETTINGS_ICON_BOOT_BIN,             .func_sta = FUNC_SET_SUB_RESTART},  //重启
#endif // UTE_MODULE_SCREENS_RESTART_SUPPORT
    {STR_SETTING_OFF,                   UI_BUF_I332001_SETTINGS_ICON_SHUTDOWN_BIN,         .func_sta = FUNC_SET_SUB_OFF},      //关机
#if UTE_MODULE_SCREENS_RESFY_SUPPORT
    {STR_SETTING_RSTFY,                 UI_BUF_I332001_SETTINGS_ICON_RETYR_BIN,            .func_sta = FUNC_SET_SUB_RSTFY},    //恢复出厂
#endif // UTE_MODULE_SCREENS_RESFY_SUPPORT
};
//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_set_sub_list_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_setting_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    u8 set_idx = sys_cb.set_idx;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入应用
static void func_set_sub_list_icon_click(void)
{
    int icon_idx;
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_setting_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {

//        func_switch_to(func_sta, FUNC_SWITCH_ZOOM_FADE | FUNC_SWITCH_AUTO);
        func_cb.sta = func_sta;
        sys_cb.set_idx = listbox->focus_icon_idx;
    }
}

//切换到设置菜单页面
static void func_set_sub_list_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    sys_cb.set_idx = 0;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define SET_LIST_CNT                       ((int)(sizeof(tbl_setting_list) / sizeof(tbl_setting_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_set_list_t_
{
    compo_listbox_t *listbox;

} f_set_list_t;

static const compo_listbox_item_t tbl_setting_list[] =
{
    {STR_DIAL_SWICTH,                    UI_BUF_I332001_SETTINGS_ICON_WATCHFACES_BIN,       .func_sta = FUNC_CLOCK_PREVIEW},    //表盘切换
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {STR_SETTING_LIGHT,                  UI_BUF_I332001_SETTINGS_ICON_RIGHTNESS_BIN,        .func_sta = FUNC_LIGHT},            //亮度调节
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {STR_SETTING_DOUSING,                UI_BUF_I332001_SETTINGS_ICON_TIME_BIN,             .func_sta = FUNC_SET_SUB_DOUSING},  //息屏时长
#if GUI_MODULE_WRIST_SUPPORT
//    {STR_SETTING_UP,                     UI_BUF_I332001_SETTINGS_ICON_TAIWAN_BIN,           .func_sta = FUNC_SET_SUB_WRIST},    //抬腕亮屏
#endif
    {STR_SETTING_DISTURD,                UI_BUF_I332001_SETTINGS_ICON_DND_BIN,              .func_sta = FUNC_SET_SUB_DISTURD},  //勿扰模式
    {STR_VOL,                           UI_BUF_I332001_SETTINGS_ICON_SOUND_BIN,            .func_sta = FUNC_SET_SUB_SAV},       //声音
#if UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
    {STR_SETTING_LANGUAGE,               UI_BUF_I332001_SETTINGS_ICON_LANUAGE_BIN,          .func_sta = FUNC_SET_SUB_LANGUAGE}, //语言设置
#endif // UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_STYLE,                          UI_BUF_I332001_SETTINGS_ICON_THEME_BIN,            .func_sta = FUNC_STYLE}, //主题
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    //{STR_BEATHER_DURATION,                   UI_BUF_SETTING_TIME_BIN,                       .func_sta = FUNC_SET_SUB_TIME},     //时间设置
//    {STR_SETTING_CALENDAR,               UI_BUF_SETTING_CALENDAR_BIN,                   .func_sta = FUNC_CALENDAER},        //日期设置
//    {STR_SETTING_PASSWORD,               UI_BUF_SETTING_PASSWORD_BIN,                   .func_sta = FUNC_SET_SUB_PASSWORD}, //密码锁
    {STR_SETTING_ABOUT,                  UI_BUF_I332001_SETTINGS_ICON_ABOUT_BIN,            .func_sta = FUNC_SET_SUB_ABOUT},    //关于
    // {STR_SETTING_4G,                     UI_BUF_SETTING_ABOUT_BIN,                      .func_sta = FUNC_SET_SUB_4G},       //4G
//    {STR_SETTING_ABOUT,                  UI_BUF_SETTING_RESTART_BIN,                    .func_sta = FUNC_SET_SUB_SOS},  //SOS
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    {STR_QRCODE,                        UI_BUF_I332001_SETTINGS_ICON_QR_BIN,               .func_sta = FUNC_SCAN},    //二维码
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_RESTART_SUPPORT
    {STR_SETTING_RESTART,               UI_BUF_I332001_SETTINGS_ICON_BOOT_BIN,             .func_sta = FUNC_SET_SUB_RESTART},  //重启
#endif // UTE_MODULE_SCREENS_RESTART_SUPPORT
    {STR_SETTING_OFF,                   UI_BUF_I332001_SETTINGS_ICON_SHUTDOWN_BIN,         .func_sta = FUNC_SET_SUB_OFF},      //关机
#if UTE_MODULE_SCREENS_RESFY_SUPPORT
    {STR_SETTING_RSTFY,                 UI_BUF_I332001_SETTINGS_ICON_RETYR_BIN,            .func_sta = FUNC_SET_SUB_RSTFY},    //恢复出厂
#endif // UTE_MODULE_SCREENS_RESFY_SUPPORT
};
//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_set_sub_list_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_setting_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    u8 set_idx = sys_cb.set_idx;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入应用
static void func_set_sub_list_icon_click(void)
{
    int icon_idx;
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_setting_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {

//        func_switch_to(func_sta, FUNC_SWITCH_ZOOM_FADE | FUNC_SWITCH_AUTO);
        func_cb.sta = func_sta;
        sys_cb.set_idx = listbox->focus_icon_idx;
    }
}

//切换到设置菜单页面
static void func_set_sub_list_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    sys_cb.set_idx = 0;
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//主菜单功能事件处理
static void func_set_sub_list_process(void)
{
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_move(f_set->listbox);
    func_process();
}

//进入设置主菜单页面
static void func_set_sub_list_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_set_list_t));
    func_cb.frm_main = func_set_sub_list_form_create();

    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    f_set->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2));
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2));
#endif
    func_cb.enter_tick = tick_get();
}

//主菜单功能消息处理
static void func_set_sub_list_message(size_msg_t msg)
{
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_set_sub_list_icon_click();                //单击图标
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
                func_set_sub_list_switch_to_clock();       //返回设置界面
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//退出菜单样式
void func_set_sub_exit(void)
{
//    printf("func_set_sub_exit\r\n");
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    func_free(listbox->mcb);                                            //释放移动控制块
    func_cb.last = FUNC_SETTING;
}

//主菜单功能
void func_set_sub_list(void)
{
//    printf("pic w:%d h:%d\n",gui_image_get_size(tbl_setting_list->res_addr).wid,gui_image_get_size(tbl_setting_list->res_addr).hei);
    func_set_sub_list_enter();
    while (func_cb.sta == FUNC_SETTING)
    {
        func_set_sub_list_process();
        func_set_sub_list_message(msg_dequeue());
    }
    func_set_sub_exit();
}
#endif
