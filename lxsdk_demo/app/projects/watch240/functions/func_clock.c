#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_gui_common.h"
#include "ute_module_watchonline.h"
#include "ute_project_config.h"
#include "ute_module_log.h"
#include "ute_module_systemtime.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

extern u16 func_clock_preview_get_type(void);

//立方体图标列表(固定6项)
//static const compo_cube_item_t tbl_menu_cube[] =
//{
//    {UI_BUF_DIALPLATE_CUBE_CALL_BIN,             FUNC_CALL},
//    {UI_BUF_DIALPLATE_CUBE_HEART_RATE_BIN,       FUNC_HEARTRATE},
//    {UI_BUF_DIALPLATE_CUBE_MUSIC_BIN,            FUNC_BT},
//    {UI_BUF_DIALPLATE_CUBE_SLEEP_BIN,            FUNC_SLEEP},
//    {UI_BUF_DIALPLATE_CUBE_SPORT_BIN,            FUNC_SPORT},
//    {UI_BUF_DIALPLATE_CUBE_STEP_BIN,             FUNC_ACTIVITY},
//};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
static const compo_cube_item_t tbl_menu_cube[] =
{
    {UI_BUF_I330001_3D_MENU_THEM_BIN,             FUNC_STYLE},
    {UI_BUF_I330001_3D_MENU_FINDPHONE_BIN,         FUNC_FINDPHONE},
    {UI_BUF_I330001_3D_MENU_MUSIC_BIN,             FUNC_BT},
    {UI_BUF_I330001_3D_MENU_POWER_BIN,             FUNC_ECIG_SET_POWER},
    {UI_BUF_I330001_3D_MENU_SETING_BIN,             FUNC_SETTING},
    {UI_BUF_I330001_3D_MENU_WEATHER_BIN,             FUNC_WEATHER},
};
// static const compo_cube_item_t tbl_menu_cube[] =
// {
// #if 1//UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
//     {UI_BUF_I330001_WATCH4_MF_00_BIN,            FUNC_ECIG_SET_POWER},
// #endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
// #if 1//UTE_MODULE_SCREENS_SLEEP_SUPPORT
//     {UI_BUF_I330001_WATCH4_MF_03_BIN,            FUNC_ECIG_SET_POWER},
// #endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
// #if UTE_MODULE_SCREENS_WEATHER_SUPPORT
//     {UI_BUF_I330001_WATCH4_MF_04_BIN,            FUNC_WEATHER},
// #endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
// #if 1//UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//     {UI_BUF_I330001_WATCH4_MF_01_BIN,            FUNC_ECIG_SET_POWER},
// #endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
// #if 1//UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
//     {UI_BUF_I330001_WATCH4_MF_02_BIN,            FUNC_ECIG_SET_POWER},
// #endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
// #if UTE_MODULE_SCREENS_MUSIC_SUPPORT
//     {UI_BUF_I330001_WATCH4_MF_05_BIN,            FUNC_BT},
// #endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
// };
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
static const compo_cube_item_t tbl_menu_cube[] =
{
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {UI_BUF_I332001_WATCH4_MF_00_BIN,            FUNC_ACTIVITY},
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {UI_BUF_I332001_WATCH4_MF_03_BIN,            FUNC_SLEEP},
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {UI_BUF_I332001_WATCH4_MF_04_BIN,            FUNC_WEATHER},
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {UI_BUF_I332001_WATCH4_MF_01_BIN,            FUNC_BLOOD_OXYGEN},
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {UI_BUF_I332001_WATCH4_MF_02_BIN,            FUNC_HEARTRATE},
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {UI_BUF_I332001_WATCH4_MF_05_BIN,            FUNC_BT},
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
};
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT


//蝴蝶表盘
#define BTF_MIN_WING                20                      //翅膀挥动最小幅度
#define BTF_MAX_WING                25                      //翅膀挥动最大幅度
#define BTF_MOVE_MAX_WING           35                      //飞舞时翅膀挥动最大幅度

#define BTF_Y_POS                   30                      //主体相对于屏幕中心Y的偏移
#define BTF_LIGHT_NUM               20                      //左+右边光晕泡泡数量
#define BTF_STATIC_FLAP_EXPIRE      5                       //静态挥动翅膀频率,ms
#define BTF_MOVE_FLAP_EXPIRE        2                       //静态挥动翅膀频率,ms
#define BTF_LEFT_EDGE               GUI_SCREEN_WIDTH / 3    //点击范围左边距判断
#define BTF_RIGHT_EDGE              GUI_SCREEN_WIDTH / 3 * 2//点击范围右边距判断

//立方体
#define CUBE_RADIUS                 (gui_image_get_size(tbl_menu_cube[0].res_addr).wid)/2          //切图正方形一半
#define KALE_EDGE_SPACE             2                                                              //边缘距离
#define KALE_ICON_SPACE             2                                                              //图标距离
#define KALE_ICON_OUTER_SIZE        (gui_image_get_size(tbl_menu_cube[0].res_addr).wid)            //外圈图标大小
#define KALE_ICON_INNER_SIZE        (KALE_ICON_OUTER_SIZE / 5 * 2)                                 //内圈图标大小
#define KALE_REFRASH_EXPIRE         4                                                              //刷新间隔 uint：ms
#define KALE_SWITCH_DR              2                                                              //单次变更R值
#define KALE_ROTATE_ANGLE           25


#define DIALPLATE_NUM               uteModuleGuiCommonGetCurrWatchMaxIndex()
#define DIALPLATE_BTF_IDX           UTE_WATCHS_DIALPLATE_BTF_INDEX        //蝴蝶表盘默认最后一个
#define DIALPLATE_CUBE_IDX          UTE_WATCHS_DIALPLATE_CUBE_INDEX        //立方体表盘默认倒数第二个

u32 dialplate_info[UTE_MODULE_SCREENS_WATCH_CNT_MAX + UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT] = UTE_MODULE_WATCHS_SORT_ADDRESS_ARRAYS;

//表盘快捷按钮编号表 对应表盘工具
const u8 quick_btn_tbl[] =
{
    [0]     = FUNC_NULL,                    //空
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    [1]     = FUNC_HEARTRATE,               //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    [2]     = FUNC_BT,                      //音乐
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    [3]     = FUNC_ALARM_CLOCK,             //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    [4]     = FUNC_BLOOD_OXYGEN,            //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    [5]     = FUNC_NULL,                    //血糖
    [6]     = FUNC_BLOOD_PRESSURE,          //血压
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    [7]     = FUNC_BREATHE,                 //呼吸
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    [8]     = FUNC_CALCULATOR,              //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    [9]     = FUNC_CAMERA,                  //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    [10]    = FUNC_TIMER,                   //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    [11]    = FUNC_SLEEP,                   //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    [12]    = FUNC_STOPWATCH,               //秒表
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    [13]    = FUNC_WEATHER,                 //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    [14]    = FUNC_GAME,                    //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    [15]    = FUNC_STYLE,                   //菜单风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    [16]    = FUNC_ALTITUDE,                //海拔
    [17]    = FUNC_MAP,                     //地图
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    [18]    = FUNC_MESSAGE,                 //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    [19]    = FUNC_SCAN,                    //扫一扫
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    [20]    = FUNC_VOICE,                   //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
#if SECURITY_PAY_EN
    [21]    = FUNC_ALIPAY,                  //支付宝
#else
    [21]    = FUNC_NULL,                    //支付宝
#endif // SECURITY_PAY_EN
//    [22]    = FUNC_COMPASS,                 //指南针
#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
    [23]    = FUNC_ADDRESS_BOOK,            //电话簿
#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    [24]    = FUNC_SPORT,                   //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    [25]    = FUNC_CALL,                    //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
    [26]    = FUNC_FINDPHONE,               //寻找手机
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    [27]    = FUNC_CALENDAER,               //日历
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    [28]    = FUNC_ACTIVITY,                //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    [29]    = FUNC_FLASHLIGHT,              //手电筒
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    [30]    = FUNC_SETTING,                 //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
};

enum
{
    COMPO_ID_BTFLY = 50,
    COMPO_ID_TIME_DOT,
    COMPO_ID_L_LIGHT1,
    COMPO_ID_L_LIGHT2,
    COMPO_ID_L_LIGHT3,
    COMPO_ID_L_LIGHT4,
    COMPO_ID_L_LIGHT5,
    COMPO_ID_L_LIGHT6,
    COMPO_ID_L_LIGHT7,
    COMPO_ID_L_LIGHT8,
    COMPO_ID_L_LIGHT9,
    COMPO_ID_L_LIGHT10,
    COMPO_ID_R_LIGHT1,
    COMPO_ID_R_LIGHT2,
    COMPO_ID_R_LIGHT3,
    COMPO_ID_R_LIGHT4,
    COMPO_ID_R_LIGHT5,
    COMPO_ID_R_LIGHT6,
    COMPO_ID_R_LIGHT7,
    COMPO_ID_R_LIGHT8,
    COMPO_ID_R_LIGHT9,
    COMPO_ID_R_LIGHT10,

    COMPO_ID_CUBE,

};

int compo_get_animation_id(void);
void compo_animation_manual_next(compo_animation_t *animation);
void func_switch_to(u8 sta, u16 switch_mode);

u8 func_clock_get_max_dialplate_num(void)
{
    // return (sizeof(dialplate_info) / sizeof(u32));
    return uteModuleGuiCommonGetCurrWatchMaxIndex();
}

u32 func_clock_get_dialplate_info(u8 index)
{
    return dialplate_info[index];
}

//点进图标进入应用
static void func_clock_cube_disk_icon_click(void)
{
#if UTE_WATCHS_CUBE_DIAL_SUPPORT
    if (sys_cb.dialplate_index != DIALPLATE_CUBE_IDX)
    {
        return;
    }

    compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
    point_t pt = ctp_get_sxy();

    int icon_idx = compo_cube_get_idx(cube, pt.x, pt.y);
    u8 func_sta;
    if (icon_idx < 0 || icon_idx >= CUBE_ITEM_CNT)
    {
        return;
    }
    //根据图标索引获取应用ID
    func_sta = tbl_menu_cube[icon_idx].func_sta;

    //切入应用
    if (func_sta > 0)
    {
//        func_switching(FUNC_SWITCH_FADE_OUT | FUNC_SWITCH_AUTO, NULL);
        func_switch_to(func_sta, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
        func_cb.sta = func_sta;
        func_cb.menu_idx = icon_idx;                //记住当前编号
    }
#endif
}

//触摸某一个区域触发飞舞
void func_clock_butterfly_click(void)
{
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    if (sys_cb.dialplate_index != DIALPLATE_BTF_IDX || sys_cb.dialplate_btf_ready == false)
    {
        return;
    }

    s32 sx, sy, x, y;
    compo_butterfly_t *btfly = compo_getobj_byid(COMPO_ID_BTFLY);
    ctp_get_cur_point(&sx, &sy, &x, &y);
    if (sx < BTF_LEFT_EDGE)       //左边
    {
        compo_butterfly_move_control(btfly, COMPO_BUTTERFLY_MOVE_CMD_LEFT);
    }
    else if (sx > BTF_RIGHT_EDGE)
    {
        compo_butterfly_move_control(btfly, COMPO_BUTTERFLY_MOVE_CMD_RIGHT);
    }
    else
    {
        compo_butterfly_move_control(btfly, COMPO_BUTTERFLY_MOVE_CMD_LOOP);
    }
#endif
}

//单击按钮
static void func_clock_button_click(void)
{
    u16 btn_id = compo_get_button_id();
    u16 animation_id = compo_get_animation_id();
    if (btn_id)
    {
        func_switch_to(quick_btn_tbl[btn_id], FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
    }
    else if (animation_id)
    {
        compo_animation_t *animation = compo_getobj_byid(animation_id);
        if (animation->bond_data == COMPO_BOND_IMAGE_CLICK)
        {
            compo_animation_manual_next(animation);
        }
        else if (animation->bond_data == COMPO_BOND_ANIMATION_AREA_CLICK)
        {
            compo_animation_click_set_vis(animation);
        }
    }
    func_clock_butterfly_click();
    func_clock_cube_disk_icon_click();                //单击图标
}

#define CUBE_RADIUS_MAX             60                                                             //切图正方形一倍
#define CUBE_RADIUS_MIN             45
//立方体表盘
compo_form_t *func_clock_cube_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);       //菜单一般创建在底层

//    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_DIALPLATE_CUBE_BG_BIN);
//    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20);

//    printf("w:%d h:%d\n",gui_image_get_size(UI_BUF_DIALPLATE_CUBE_CALL_BIN).wid,gui_image_get_size(UI_BUF_DIALPLATE_CUBE_CALL_BIN).hei);

    //创建立方体菜单
    // compo_cube_t *cube = compo_cube_create(frm, CUBE_RADIUS, tbl_menu_cube, CUBE_ITEM_CNT);
    compo_cube_t *cube = compo_cube_create(frm, CUBE_RADIUS_MAX, tbl_menu_cube, CUBE_ITEM_CNT);
    compo_cube_set_type(cube, COMPO_CUBE_TYPE_POWER);
    // compo_cube_set_pos(cube, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20);
    compo_cube_add_element(cube, 0, UI_BUF_I330001_3D_BATTER_LIGHT_BIN, 18);

    compo_cube_set_pos(cube, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+20 );
    compo_setid(cube, COMPO_ID_CUBE);

//    s32 ax, ay;
//    ax = 0;
//    ay = 1400;
//    compo_cube_roll_from(cube, ax, ay);
//    compo_cube_update(cube);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 40, GUI_SCREEN_CENTER_Y - 100, 300, 70);
    compo_bonddata(txt, COMPO_BOND_HOUR);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 40, GUI_SCREEN_CENTER_Y - 100, 300, 70);
    compo_bonddata(txt, COMPO_BOND_MINUTE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));


    txt = compo_textbox_create(frm, 10);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 180, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_X, 300, 70);
    compo_bonddata(txt, COMPO_BOND_DATE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));


    txt = compo_textbox_create(frm, 1);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 104, 300, 70);
    compo_textbox_set(txt, ":");
    compo_setid(txt, COMPO_ID_TIME_DOT);

    //compo_cube_update(cube);
    return frm;
}

//蝴表盘
compo_form_t *func_clock_butterfly_form_create(void)
{
    tft_set_temode(0);
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_butterfly_res_t btf_res;
    btf_res.res_body = UI_BUF_DIALPLATE_BUTTERFLY_BODY_BIN;
    btf_res.res_wing_down = UI_BUF_DIALPLATE_BUTTERFLY_DOWN_BIN;
    btf_res.res_wing_up = UI_BUF_DIALPLATE_BUTTERFLY_UP_BIN;

    compo_butterfly_t *btfly = compo_butterfly_create(frm, &btf_res);
    compo_butterfly_set_pos(btfly, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + BTF_Y_POS);
    compo_setid(btfly, COMPO_ID_BTFLY);

    //新建光晕泡泡
    for(u8 i=0; i<BTF_LIGHT_NUM; i++)
    {
        compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_DIALPLATE_BUTTERFLY_LIGHT_BIN);
        compo_setid(pic, COMPO_ID_L_LIGHT1 + i);
        compo_picturebox_set_visible(pic, false);
        widget_set_top(pic->img, true);
    }

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 40, GUI_SCREEN_CENTER_Y - 100, 300, 70);
    compo_bonddata(txt, COMPO_BOND_HOUR);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 40, GUI_SCREEN_CENTER_Y - 100, 300, 70);
    compo_bonddata(txt, COMPO_BOND_MINUTE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    txt = compo_textbox_create(frm, 10);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 70, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 60, 300, 70);
    compo_bonddata(txt, COMPO_BOND_DATE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    txt = compo_textbox_create(frm, 1);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 100, 260, 70);
    compo_textbox_set(txt, ":");
    compo_setid(txt, COMPO_ID_TIME_DOT);

    return frm;
}


compo_form_t *func_clock_form_create(void)
{
    compo_form_t *frm;

    uteModuleGuiCommonGetCurrWatchIndex(&sys_cb.dialplate_index);
#if UTE_MODULE_WATCHONLINE_SUPPORT
    if (uteModuleWatchOnlineGetVailWatchCnt())
    {
        for (uint8_t i = 0; i < uteModuleWatchOnlineGetVailWatchCnt(); i++)
        {
            dialplate_info[UTE_MODULE_SCREENS_WATCH_CNT_MAX + i] = uteModuleWatchOnlineGetBaseAddress(i);
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,watch online index %d: 0x%x", __func__, i, dialplate_info[UTE_MODULE_SCREENS_WATCH_CNT_MAX + i]);
        }
    }
    else
#endif
    {
        if (sys_cb.dialplate_index > UTE_MODULE_SCREENS_WATCH_CNT_MAX - 1)
        {
            sys_cb.dialplate_index = DEFAULT_WATCH_INDEX;
        }
    }

    switch (sys_cb.dialplate_index)
    {
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
        case DIALPLATE_BTF_IDX:
        {
//            printf("1111111111111111111111\n");
            frm = func_clock_butterfly_form_create();
        }
        break;
#endif
#if UTE_WATCHS_CUBE_DIAL_SUPPORT
        case DIALPLATE_CUBE_IDX:
        {
//             printf("2222222222222222222222222222\n");
            frm = func_clock_cube_form_create();
        }
        break;
#endif
        default:
        {
//             printf("33333333333333333333333333\n");
            u32 base_addr = dialplate_info[sys_cb.dialplate_index];
            u16 compo_num = bsp_uitool_header_phrase(base_addr);
            if (!compo_num)
            {
                halt(HALT_GUI_DIALPLATE_HEAD);
            }
            frm = compo_form_create(true);
            bsp_uitool_create(frm, base_addr, compo_num);
        }
        break;
    }

    return frm;
}

void func_clock_butterfly_set_light_visible(bool visible)
{
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    if (sys_cb.dialplate_index != DIALPLATE_BTF_IDX)
    {
        return;
    }

    sys_cb.dialplate_btf_ready = visible;
    compo_picturebox_t *pic;
    for(u8 i=0; i<BTF_LIGHT_NUM; i++)
    {
        pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + i);
        if (compo_picturebox_get_visible(pic))
        {
            compo_picturebox_set_visible(pic, visible);
        }
    }
#endif
}

void func_clock_butterfly_process(void)
{
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    if (sys_cb.dialplate_index != DIALPLATE_BTF_IDX || sys_cb.dialplate_btf_ready == false)
    {
        return;
    }

    compo_butterfly_t *btfly = compo_getobj_byid(COMPO_ID_BTFLY);

    static s8 direction = -1;
    static u32 ticks = 0;

    static u32 ticks_light = 0;
    static s16 btfly_wind_angle = BTF_MAX_WING;
    u8 expire = BTF_STATIC_FLAP_EXPIRE;
    u8 max_wing = BTF_MAX_WING;

    compo_picturebox_t *pic;

    //移动挥动翅膀加速
    if (compo_butterfly_get_sta(btfly) == COMPO_BUTTERFLY_STA_MOVE)
    {
        expire = BTF_MOVE_FLAP_EXPIRE;
        max_wing = BTF_MOVE_MAX_WING;
    }
    else
    {
        if (btfly_wind_angle > max_wing)
        {
            btfly_wind_angle = max_wing;
        }
    }

    //挥动翅膀处理
    if (tick_check_expire(ticks, expire))
    {
        ticks = tick_get();
        if (btfly_wind_angle == BTF_MAX_WING)
        {
            direction = 1;
        }
        else if(btfly_wind_angle == max_wing)
        {
            direction = -1;
        }
        btfly_wind_angle += direction;
        compo_butterfly_flap(btfly, btfly_wind_angle);
        compo_butterfly_update(btfly);


        u8 index = get_random(BTF_LIGHT_NUM >> 1);
        s32 rota = compo_butterfly_get_rotation(btfly);
        s16 z = compo_butterfly_get_z(btfly);
        pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + index);

        if (compo_picturebox_get_visible(pic) == false)
        {
            s16 x = 100 + get_random(70);
            s16 y = -get_random(200 + rota / 10);
            compo_picturebox_set_rotation_center(pic, x, y);
            compo_picturebox_set_rotation(pic, rota);
            compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 170 + BTF_Y_POS + z * 30 / 100);
            compo_picturebox_set_size(pic, 1, 1);
            compo_picturebox_set_alpha(pic, 0);
            compo_picturebox_set_visible(pic, true);
        }

        index = get_random(BTF_LIGHT_NUM >> 1) + (BTF_LIGHT_NUM >> 1);
        pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + index);
        if (compo_picturebox_get_visible(pic) == false)
        {
            s16 x = -100 - get_random(70);
            s16 y = -get_random(200 + rota / 10);
            compo_picturebox_set_rotation_center(pic, x, y);
            compo_picturebox_set_rotation(pic, rota);
            compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 170 + BTF_Y_POS + z * 30 / 100);
            compo_picturebox_set_size(pic, 1, 1);
            compo_picturebox_set_alpha(pic, 0);
            compo_picturebox_set_visible(pic, true);

        }
    }

    compo_butterfly_move(btfly);

    //光晕处理
    if (tick_check_expire(ticks_light, expire * 2))
    {
        ticks_light = tick_get();
        for(u8 i=0; i<BTF_LIGHT_NUM; i++)
        {
            pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + i);
            if (compo_picturebox_get_visible(pic) == true)
            {
                u8 alpha = compo_picturebox_get_alpha(pic);
                alpha = !alpha ? 204 :alpha;
                alpha -= 5;
                s16 size = 50 *(255 - alpha) / 255;
                if (size >= 10)
                {
                    compo_picturebox_set_size(pic, size,size);
                    compo_picturebox_set_alpha(pic, alpha);
                }

                s32 rota = compo_butterfly_get_rotation(btfly);
                compo_picturebox_set_rotation(pic, rota);
                if (alpha < 30)
                {
                    compo_picturebox_set_visible(pic, false);
                }
            }
        }
    }
#endif
}

//子功能公共事件处理
void func_clock_sub_process(void)
{
    func_process();                                     //刷新UI
}

//子功能公共消息处理
void func_clock_sub_message(size_msg_t msg)
{
    func_message(msg);
}

//static int rp = 0;
//static int ra = 1300;
#define CUBE_HALF_CIRCUM(x)                 ((int)(M_PI * x))         //圆周一半
#define CUBE_TOUCH_TIME  2000

#define CUBE_DIAL_ANIMATION_TICK_EXPIRE               18                                  //动画单位时间Tick(ms)
#define CUBE_DIAL_FOCUS_AUTO_STEP_DIV                 10

enum
{
    DIAL_3D_CLICK_STA_NONE,   //None
    DIAL_3D_CLICK_STA_DEC,    //缩小
    DIAL_3D_CLICK_STA_INC,    //放大
    DIAL_3D_CLICK_STA_END,    //结束
    DIAL_3D_CLICK_STA_STOP,   //停顿
};


static bool click = false;

void func_3d_dial_click(void)
{
    if (click == false)
    {
        click = true;
    }
}
void func_clock_3d_process(void)
{

    compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
    compo_cube_move(cube);
    static u32 tick = 0;
    static u8 sta = DIAL_3D_CLICK_STA_NONE; //0:None 1:开始缩小; 2:开始放大; 3:结束; 4:停顿
    u32 width_max = cube->ele[0].location.wid;//gui_image_get_size(tbl_menu_ele[0].res_addr).wid / 2;
    static u32 width;
    static u8 stop_cnt = 0;
    // printf("%s,click=%d,flag_move_auto=%d\n",__func__, click,cube->move_cb.flag_move_auto);
    if (cube->move_cb.flag_move_auto==0)
    {
        if(tick_check_expire(cube->move_cb.tick, 18))
        {
            s32 da;
            cube->move_cb.tick = tick_get();
            da = 20;
            compo_cube_roll(cube, da);
            compo_cube_update(cube);
        }
    }


    if (click)
    {
        if (tick_check_expire(tick, 100))
        {
            tick = tick_get();

            switch (sta)
            {
                case DIAL_3D_CLICK_STA_NONE:
                    width = width_max;
                    if (cube->radius >= CUBE_RADIUS_MAX)
                    {
                        sta = DIAL_3D_CLICK_STA_DEC;
                    }
                    break;

                case DIAL_3D_CLICK_STA_DEC:
                    cube->radius--;
                    //1:x = CUBE_RADIUS_MIN * 1414/1000 : (s32)cube->radius * 1414 / 1000;
                    if ((((s32)cube->radius * 1414 / 1000) / (CUBE_RADIUS_MIN * 1414/1000)) <= width_max)
                    {
                        width--;
                        if (width < 0)
                        {
                            width = 0;
                        }
                    }
                    if (cube->radius <= CUBE_RADIUS_MIN)
                    {
                        width = 0;
                        sta =DIAL_3D_CLICK_STA_STOP;
                    }
                    break;

                case DIAL_3D_CLICK_STA_INC:
                    cube->radius++;

                    if ((((s32)cube->radius * 1414 / 1000) / (CUBE_RADIUS_MIN * 1414/1000)) <= width_max)
                    {
                        width++;
                        if (width > width_max)
                        {
                            width = width_max;
                        }
                    }

                    if (cube->radius >= CUBE_RADIUS_MAX)
                    {
                        width = width_max;
                        sta = DIAL_3D_CLICK_STA_END;
                    }
                    break;

                case DIAL_3D_CLICK_STA_END:
                    click = false;
                    sta = DIAL_3D_CLICK_STA_NONE;
                    break;

                case DIAL_3D_CLICK_STA_STOP:
                    stop_cnt++;
                    if (stop_cnt > 5)
                    {
                        sta = 2;
                        stop_cnt = 0;
                    }
                    break;
            }

            for (int i=0; i<CUBE_ITEM_CNT; i++)
            {
                widget_image3d_set_r(cube->item_img[i], cube->radius);
            }

            for (int i=0; i<CUBE_ITEM_ELE_CNT; i++)
            {
                if (cube->ele[i].img3d != NULL)
                {
                    widget_set_size(cube->ele[i].img3d, width, cube->ele[i].location.hei);
                }
            }
        }
    }
}
//时钟表盘功能事件处理
static void func_clock_process(void)
{
#if UTE_WATCHS_CUBE_DIAL_SUPPORT
    if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
    {

        func_clock_3d_process();
        // compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
        // compo_cube_move(cube);


    }
    else if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
    {
        func_clock_butterfly_process();
    }
#endif
    func_process();                                  //刷新UI
}

//static void func_clock_swith(bool dir)
//{
//    if (dir) {
//        sys_cb.dialplate_index++;
//        if (sys_cb.dialplate_index > DIALPLATE_NUM - 1) {
//            sys_cb.dialplate_index = 0;
//        }
//    } else {
//        if (sys_cb.dialplate_index == 0) {
//            sys_cb.dialplate_index = DIALPLATE_NUM - 1;
//        } else {
//            sys_cb.dialplate_index--;
//        }
//    }
//    printf("dialplate index:%d\n", sys_cb.dialplate_index);
//    compo_form_destroy(func_cb.frm_main);
//    func_cb.frm_main = func_clock_form_create();
//}

//重建表盘界面
void func_clock_recreate_dial(void)
{
    printf("dialplate index:%d\n", sys_cb.dialplate_index);
    if(func_cb.sta == FUNC_CLOCK)
    {
        compo_form_destroy(func_cb.frm_main);
        func_cb.frm_main = func_clock_form_create();
    }
}

//时钟表盘功能消息处理
static void func_clock_message(size_msg_t msg)
{
    f_clock_t *f_clock = (f_clock_t*)func_cb.f_cb;
    bool flag_cube_touch = false;
    point_t pt = ctp_get_sxy();
    // s16 cube_limit_x = (GUI_SCREEN_WIDTH - gui_image_get_size(UI_BUF_DIALPLATE_CUBE_BG_BIN).wid) / 2;
    // s16 cube_limit_y = (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_DIALPLATE_CUBE_BG_BIN).wid) / 2;
    // bool flag_cube_touch_x = (pt.x >= cube_limit_x) && (pt.x <= (cube_limit_x + gui_image_get_size(UI_BUF_DIALPLATE_CUBE_BG_BIN).wid));
    // bool flag_cube_touch_y = (pt.y >= cube_limit_y) && (pt.y <= (cube_limit_y + gui_image_get_size(UI_BUF_DIALPLATE_CUBE_BG_BIN).wid));
    s16 cube_limit_x = (GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X/2);
    s16 cube_limit_y = (GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y/2);
    bool flag_cube_touch_x = (pt.x >= cube_limit_x) && (pt.x <= (cube_limit_x + GUI_SCREEN_CENTER_X));
    bool flag_cube_touch_y = (pt.y >= cube_limit_y) && (pt.y <= (cube_limit_y + GUI_SCREEN_CENTER_Y));

    if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX && flag_cube_touch_x && flag_cube_touch_y)
    {
        compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
        if (msg == MSG_CTP_TOUCH && cube != NULL)
        {
            // 移动过程中，触屏停止。重新进入到开始拖动模式
            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_DRAG);
            flag_cube_touch = true;
            // compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
            // //移动过程中，触屏停止。重新进入到开始拖动模式
            // compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_DRAG);
            // f_clock->cube_touch = true;
        }
        else if (msg >= MSG_CTP_SHORT_LEFT && msg <= MSG_CTP_SHORT_DOWN)
        {
            return;
        }

    }
#if UTE_LOG_DRV_TP_LVL
    if(msg >= MSG_CTP_TOUCH && msg <= MSG_CTP_COVER)
    {
        printf("%s -> msg:0x%x\n", __func__, msg);
    }
#endif
    switch (msg)
    {
        case MSG_CTP_SHORT_UP:
//        func_clock_sub_pullup();                //上拉菜单
            func_clock_butterfly_set_light_visible(false);
            func_switch_to(FUNC_CARD, FUNC_SWITCH_MENU_PULLUP_UP);  //上拉卡片界面
            if (func_cb.sta == FUNC_CLOCK)
            {
                func_clock_butterfly_set_light_visible(true);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
//        func_clock_sub_side();                  //右拉边菜单
//            func_switch_to(FUNC_SIDEBAR, FUNC_SWITCH_LR_ZOOM_RIGHT);  //右滑界面
            printf("%s -> MSG_CTP_SHORT_RIGHT\n", __func__);
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            func_cb.left_sta = FUNC_MESSAGE;
            func_switch_to(FUNC_MESSAGE, FUNC_SWITCH_LR_ZOOM_RIGHT);
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            break;

        case MSG_CTP_SHORT_DOWN:
            func_clock_sub_dropdown();              //下拉菜单
            break;

        case MSG_CTP_CLICK:
            if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
            {
                flag_cube_touch = true;
                func_3d_dial_click();
            }

            func_clock_button_click();
            break;

        case MSG_SYS_500MS: //秒跳动处理
        {
            if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX || sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
            {
                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TIME_DOT);
                if (txt != NULL)
                {
                    static bool time_visible = true;
                    compo_textbox_set_visible(txt, time_visible);
                    time_visible = !time_visible;
                }
            }
        }
        break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
#if UTE_WATCHS_CUBE_DIAL_SUPPORT
            if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
            {
                compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
                if(cube == NULL) return;
                compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_FORWARD);
                flag_cube_touch = true;
            }
#endif
            break;

        case MSG_QDEC_BACKWARD:                             //向后滚动菜单
#if UTE_WATCHS_CUBE_DIAL_SUPPORT
            if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
            {
                compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
                if(cube == NULL) return;
                compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_BACKWARD);
                flag_cube_touch = true;
            }
#endif
            break;

        case MSG_CTP_LONG:
            if (func_clock_preview_get_type() == PREVIEW_ROTARY_STYLE)
            {
                func_cb.sta = FUNC_CLOCK_PREVIEW;
            }
            else
            {
                func_switch_to(FUNC_CLOCK_PREVIEW, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);                    //切换回主时钟
            }
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入时钟表盘功能
static void func_clock_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_clock_t));
    func_cb.frm_main = func_clock_form_create();
    f_clock_t *f_clock = (f_clock_t*)func_cb.f_cb;
    func_cb.flag_animation = true;///
    if(sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
    {
        compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
        f_clock->cube_touch_time = 5;
//        f_clock->cube_ra = cube->move_cb.focus_sph.azimuth;
//        f_clock->cube_rp = cube->move_cb.focus_sph.rotation;
        f_clock->cube_ra = 1400;
        f_clock->cube_rp = 550;
        f_clock->cube_touch = false;
    }
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
    {
        sys_cb.dialplate_btf_ready = true;
        tft_set_temode(0);
    }
#endif
}

//退出时钟表盘功能
static void func_clock_exit(void)
{
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
    {
        sys_cb.dialplate_btf_ready = false;
        tft_set_temode(DEFAULT_TE_MODE);
    }
#endif
    func_cb.last = FUNC_CLOCK;

    if(!uteApplicationCommonIsHasConnectOurApp())//防止上电直接进入表盘特殊情况
    {
        sys_cb.power_on_state=true;//开机绑定流程
    }
}

//时钟表盘功能
void func_clock(void)
{
    printf("%s\n", __func__);
    func_clock_enter();
    while (func_cb.sta == FUNC_CLOCK)
    {
        func_clock_process();
        func_clock_message(msg_dequeue());
    }
    func_clock_exit();
}
