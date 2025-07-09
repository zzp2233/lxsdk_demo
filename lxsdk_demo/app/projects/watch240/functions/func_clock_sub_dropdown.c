#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_notdisturb.h"
#include "ute_module_liftwrist.h"
#include "ute_application_common.h"
#include "ute_drv_battery_common.h"
#include "ute_module_sport.h"
#include "func_menu.h"
#include "ute_module_call.h"
#include "ute_module_localRingtone.h"
#include "ute_module_lockScreen.h"

#if UTE_MODULE_SCREENS_DWON_MENU_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define PAGE_HEIGHT     GUI_SCREEN_HEIGHT - GUI_SCREEN_HEIGHT/3.7
#define  BT_ON_PIC_BIN       UI_BUF_I330001_SLIDEMENU_ICON_TELEPHONE_GROUP_898_BIN    ///BT 连接状态图片
#define  BT_OFF_PIC_BIN      UI_BUF_I330001_SLIDEMENU_ICON_TELEPHONE_FRAME_1389_BIN   ///BT 断开状态图片
#define  BLE_ON_PIC_BIN      UI_BUF_I330001_SLIDEMENU_ICON_CONNECT_GROUP_899_BIN      ///BLE 连接状态图片
#define  BLE_OFF_PIC_BIN     UI_BUF_I330001_SLIDEMENU_ICON_CONNECT_FRAME_1390_BIN     ///BLE 断开状态图片
#define  BATTERY_PIC_0_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_00_BIN                ///电池电量图标0
#define  BATTERY_PIC_1_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_01_BIN                ///电池电量图标1
#define  BATTERY_PIC_2_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_02_BIN                ///电池电量图标2
#define  BATTERY_PIC_3_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_03_BIN                ///电池电量图标3
#define  BATTERY_PIC_4_BIN   UI_BUF_I330001_SLIDEMENU_ICON_BATT_04_BIN                ///电池电量图标4
#define  PAGE_NUM_WHITE_BIN   UI_BUF_I330001_SLIDEMENU_DOT_01_BIN           ///底部页码点->白
#define  PAGE_NUM_GREY_BIN    UI_BUF_I330001_SLIDEMENU_DOT_00_BIN           ///底部页码点->灰
#define  PAGE_HEIGHT_ADJUST    -3   ///page 页面y轴调节

static uint8_t dropdown_disturb_sw;     //功能未做，加个开关先放着

enum
{
    COMPO_ID_BTN_START = FUNC_MAX_NUM, //从任务最大枚举开始，避免和表盘跳转et的d冲突
    //按键
    COMPO_ID_BTN_CONNECT,       //蓝牙连接开关
    COMPO_ID_BTN_MENU,          //菜单
    // COMPO_ID_BTN_FLASHLIGHT,    //手电筒
#if GUI_MODULE_WRIST_SUPPORT
    COMPO_ID_BTN_WIRST,         //抬婉亮屏
#endif
    COMPO_ID_BTN_DISCURD,       //勿扰模式开关
    COMPO_ID_BTN_LIGHT,         //亮度调节
    COMPO_ID_BTN_MUTE,          //静音模式开关
    COMPO_ID_BTN_PHONE,         //查找手机
    COMPO_ID_BTN_SETTING,       //设置

    COMPO_ID_PIC_WHITE,       //底部页码点->白
    COMPO_ID_PIC_GREY,        //底部页码点->灰
    COMPO_ID_PIC_PASSWORD,//童锁
    //电池百分比文本
    COMPO_ID_TXT_BATTERY_PERCENT,
    //电池百分比图片
    COMPO_ID_TXT_BATTERY_PIC,
    //蓝牙状态图片
    COMPO_ID_TXT_BLUETOOTH_STA_PIC,
    COMPO_ID_TXT_BTETOOTH_STA_PIC,
};

typedef struct dropdown_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    s16 x;
    s16 y;
    u8 sel_idx;
} dropdown_disp_btn_item_t;

#define DROPDOWN_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_dropdown_disp_btn_item) / sizeof(tbl_dropdown_disp_btn_item[0])))
//类型有显示信息，控制开关，点击跳转任务
//按钮item，创建时遍历一下
static const  dropdown_disp_btn_item_t tbl_dropdown_disp_btn_item[] =
{
    ///*第一页*/
    {UI_BUF_I330001_SLIDEMENU_ICON_BT_CLOSE1_BIN,          COMPO_ID_BTN_CONNECT,          GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  31},///蓝牙连接开关
    {UI_BUF_I330001_SLIDEMENU_ICON_THEME03_BIN,         COMPO_ID_BTN_MENU,             GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  31},///菜单
    {UI_BUF_I330001_CHILD_LOCK_PASSWORD00_BIN,       COMPO_ID_PIC_PASSWORD,       GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  62+31+4},///童锁功能
    {UI_BUF_I330001_SLIDEMENU_ICON_SETTINGS_BIN,        COMPO_ID_BTN_SETTING,            GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  62+31+4},//设置
    {UI_BUF_I330001_SLIDEMENU_ICON_DND00_BIN,           COMPO_ID_BTN_DISCURD,          GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  62*2+31+8},///勿扰模式开关
    {UI_BUF_I330001_SLIDEMENU_ICON_BRIGHTNESS_BIN,      COMPO_ID_BTN_LIGHT,            GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  62*2+31+8},///亮度调节
    ///*第二页*/
    {UI_BUF_I330001_SLIDEMENU_ICON_FINDPHONE_BIN,       COMPO_ID_BTN_PHONE,               GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X/2,  31},///静音模式开关
    //  {UI_BUF_I330001_SLIDEMENU_ICON_FINDPHONE_BIN,       COMPO_ID_BTN_PHONE,            GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  31},///查找手机
    //  {UI_BUF_I330001_SLIDEMENU_ICON_SHOUDIAN00_BIN,      COMPO_ID_BTN_FLASHLIGHT,            GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  31},//手电筒
    //  {UI_BUF_I330001_SLIDEMENU_ICON_SETTINGS_BIN,        COMPO_ID_BTN_SETTING,          GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X/2,  62+31+4},///设置
};

#define MENU_CNT    ((int)(sizeof(dwon_tbl_style_list) / sizeof(dwon_tbl_style_list[0])))
//风格列表tbl
static const compo_listbox_item_t dwon_tbl_style_list[] =
{
    {STR_STYLE_LIST_1,          UI_BUF_I330001_SLIDEMENU_ICON_THEME03_BIN,           .menu_style = MENU_STYLE_LIST},             //列表
    {STR_GONG_GE,               UI_BUF_I330001_SLIDEMENU_ICON_THEME01_BIN,           .menu_style = MENU_STYLE_CUM_SUDOKU},       //宫格
    {STR_STYLE_HONEYCOMB,       UI_BUF_I330001_SLIDEMENU_ICON_THEME00_BIN,           .menu_style = MENU_STYLE_HONEYCOMB},        //蜂窝
    //  {STR_SPHERE,                UI_BUF_I330001_SLIDEMENU_ICON_THEME07_BIN,           .menu_style = MENU_STYLE_FOOTBALL},         //球体
    {STR_CHECKERBOARD,          UI_BUF_I330001_SLIDEMENU_ICON_THEME06_BIN,           .menu_style = MENU_STYLE_GRID},             //棋盘
    {STR_HALO,                  UI_BUF_I330001_SLIDEMENU_ICON_THEME02_BIN,           .menu_style = MENU_STYLE_KALE},             //光环
    {STR_STYLE_SKYRER,          UI_BUF_I330001_SLIDEMENU_ICON_THEME08_BIN,           .menu_style = MENU_STYLE_SKYRER},           //天圆地方
    {STR_STYLE_GRID_1,          UI_BUF_I330001_SLIDEMENU_ICON_THEME09_BIN,           .menu_style = MENU_STYLE_CUM_GRID},         //网格
    {STR_SIX_PALACE_GRID,       UI_BUF_I330001_SLIDEMENU_ICON_THEME04_BIN,           .menu_style = MENU_STYLE_CUM_FOURGRID},     //六宫格
    {STR_STYLE_WATERFALL,       UI_BUF_I330001_SLIDEMENU_ICON_THEME05_BIN,           .menu_style = MENU_STYLE_WATERFALL},        //瀑布
};

///               /                 *更新*                    /                 ///

//下拉电量图标更新
static void func_clock_sub_dropdown_battery_pic_update(void)
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
////下拉蓝牙连接标志更新
static void func_clock_sub_dropdown_bluetooth_pic_update(void)
{
    compo_picturebox_t *bluetooth_pic = compo_getobj_byid(COMPO_ID_TXT_BLUETOOTH_STA_PIC);
    compo_picturebox_t *btooth_pic = compo_getobj_byid(COMPO_ID_TXT_BTETOOTH_STA_PIC);
    if (ble_is_connect())
    {
        compo_picturebox_set(bluetooth_pic, BLE_ON_PIC_BIN);
    }
    else
    {
        compo_picturebox_set(bluetooth_pic, BLE_OFF_PIC_BIN);
    }

    if(uteModuleCallBtIsConnected())
    {
        compo_picturebox_set(btooth_pic, BT_ON_PIC_BIN);
    }
    else
    {
        compo_picturebox_set(btooth_pic, BT_OFF_PIC_BIN);
    }

}
//下拉静音图标更新
static void func_clock_sub_dropdown_mute_pic_update(void)
{
    // compo_button_t *mute_pic = compo_getobj_byid(COMPO_ID_BTN_MUTE);
    // // if(sys_cb.mute)
    // if(uteModuleLocalRingtoneGetMuteStatus())
    // {
    //     compo_button_set_bgimg(mute_pic, UI_BUF_I330001_SLIDEMENU_ICON_VOLUMES01_BIN);
    // }
    // else
    // {
    //     compo_button_set_bgimg(mute_pic, UI_BUF_I330001_SLIDEMENU_ICON_VOLUMES00_BIN);
    // }
}
//童锁图标更新
static void func_clock_sub_dropdown_password_pic_update(void)
{
    compo_button_t *mute_pic = compo_getobj_byid(COMPO_ID_PIC_PASSWORD);
    // if(sys_cb.mute)
    if(uteModulePasswordData.password_cnt == 4)
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_I330001_CHILD_LOCK_PASSWORD01_BIN);
    }
    else
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_I330001_CHILD_LOCK_PASSWORD00_BIN);
    }
}
////下拉蓝牙按钮更新
static void func_clock_sub_dropdown_bluetooth_btn_pic_update(void)
{
    compo_button_t *bluetooth_pic = compo_getobj_byid(COMPO_ID_BTN_CONNECT);
    compo_picturebox_t *bluetooth = compo_getobj_byid(COMPO_ID_TXT_BTETOOTH_STA_PIC);
    // printf("bt_get_scan: 0x%x,bt_get_status:%d\n", bt_get_scan(),bt_get_status());
    if(uteModuleCallBtIsPowerOn())
    {
        compo_button_set_bgimg(bluetooth_pic, UI_BUF_I330001_SLIDEMENU_ICON_BT_OPEN1_BIN);
        // compo_picturebox_set_visible(bluetooth, true);
    }
    else
    {
        compo_button_set_bgimg(bluetooth_pic,UI_BUF_I330001_SLIDEMENU_ICON_BT_CLOSE1_BIN );
        // compo_picturebox_set_visible(bluetooth, false);

    }
}
#if GUI_MODULE_WRIST_SUPPORT
//下拉抬婉亮屏按钮状态切换
static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
#endif
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}
//下拉抬婉亮屏按钮更新
static void func_clock_sub_dropdown_wrist_pic_update(void)
{
    compo_button_t *wrist_pic = compo_getobj_byid(COMPO_ID_BTN_WIRST);

    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);

    if(quick.isTurnTheWrist == true)
    {
        compo_button_set_bgimg(wrist_pic, UI_BUF_I330001_SLIDEMENU_ICON_TAIWAN01_BIN);
    }
    else
    {
        compo_button_set_bgimg(wrist_pic, UI_BUF_I330001_SLIDEMENU_ICON_TAIWAN00_BIN);
    }

}
#endif
//下拉勿扰按钮更新
static void func_clock_sub_dropdown_disturb_pic_update(void)
{
    compo_button_t *disturb_pic = compo_getobj_byid(COMPO_ID_BTN_DISCURD);

    if(dropdown_disturb_sw)
    {
        sys_cb.disturd_adl = true;
        compo_button_set_bgimg(disturb_pic, UI_BUF_I330001_SLIDEMENU_ICON_DND01_BIN);
        uteModuleNotDisturbSetOpenStatus(sys_cb.disturd_adl);
    }
    else
    {
        sys_cb.disturd_adl = false;
        compo_button_set_bgimg(disturb_pic, UI_BUF_I330001_SLIDEMENU_ICON_DND00_BIN);
        uteModuleNotDisturbSetOpenStatus(sys_cb.disturd_adl);
    }

}
//菜单id更新
static void get_menu_idx_update(void)
{
    dropdown_disp_btn_item_t *dropdown_disp_btn_item = (dropdown_disp_btn_item_t *)func_cb.f_cb;
    for(int i=0; i<MENU_CNT; i++)
    {
        if(dwon_tbl_style_list[i].menu_style == func_cb.menu_style)
        {
            dropdown_disp_btn_item->sel_idx = i;
            return;
        }
    }
}
//菜单样式图标更新
static void func_clock_sub_dropdown_menu_pic_update(void)
{
    dropdown_disp_btn_item_t *dropdown_disp_btn_item = (dropdown_disp_btn_item_t *)func_cb.f_cb;
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_MENU);
    compo_button_set_bgimg(btn, dwon_tbl_style_list[dropdown_disp_btn_item->sel_idx].res_addr);
    if (func_cb.menu_style == MENU_STYLE_SKYRER)
    {
        u8 func_menu_sub_skyrer_get_first_idx(void);
        func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();
    }
    else
    {
        func_cb.menu_idx = 0;           //切换风格后进入回中心位置
    }
    uteModuleGuiCommonSetThemeTypeId(func_cb.menu_style);
}

///               /                 *更新*                    /                 ///

widget_page_t *widget;///创建按键父局器页面

//创建下拉菜单
static void func_clock_sub_dropdown_form_create(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if(quick.isNotDisturb)
    {
        dropdown_disturb_sw = 1;
    }
    else
    {
        dropdown_disturb_sw = 0;
    }
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    if(uteModuleNotDisturbIsOpenScheduled())
    {
        if(uteModuleNotDisturbIsTimeBucket())
        {
            dropdown_disturb_sw = 1;
        }
        else
        {
            dropdown_disturb_sw = 0;
        }
    }
#endif

    compo_form_t *frm = compo_form_create(true);

    ///创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    widget = widget_page_create(frm->page);///创建按键页面
    widget_page_set_client(widget, 0, 0);
    widget_set_location(widget,GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT/1.55+PAGE_HEIGHT_ADJUST,GUI_SCREEN_WIDTH*2,PAGE_HEIGHT);

    ///创建页码点
    compo_picturebox_t *picbox = compo_picturebox_create(frm, PAGE_NUM_WHITE_BIN);
    compo_picturebox_set_pos(picbox,114, 278);
    compo_setid(picbox,COMPO_ID_PIC_WHITE);

    picbox = compo_picturebox_create(frm, PAGE_NUM_GREY_BIN);
    compo_picturebox_set_pos(picbox,126, 278);
    compo_setid(picbox,COMPO_ID_PIC_GREY);

    ///创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < DROPDOWN_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn =compo_button_create_page_by_image(frm,widget, tbl_dropdown_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn,  tbl_dropdown_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_dropdown_disp_btn_item[idx_btn].x, tbl_dropdown_disp_btn_item[idx_btn].y);
    }
    //电池
    compo_picturebox_t *battery_pic = compo_picturebox_create(frm, BATTERY_PIC_4_BIN);
    compo_setid(battery_pic, COMPO_ID_TXT_BATTERY_PIC);
    compo_picturebox_set_pos(battery_pic, GUI_SCREEN_WIDTH/1.6, GUI_SCREEN_CENTER_Y/8);


    //蓝牙状态 Bt
    compo_picturebox_t *bluetooth_pic = compo_picturebox_create(frm, BT_ON_PIC_BIN);
    compo_setid(bluetooth_pic, COMPO_ID_TXT_BLUETOOTH_STA_PIC);
    compo_picturebox_set_pos(bluetooth_pic, GUI_SCREEN_WIDTH/2.1, GUI_SCREEN_CENTER_Y/8);

    //蓝牙状态 Ble
    bluetooth_pic = compo_picturebox_create(frm, BLE_OFF_PIC_BIN);
    compo_setid(bluetooth_pic, COMPO_ID_TXT_BTETOOTH_STA_PIC);
    compo_picturebox_set_pos(bluetooth_pic, GUI_SCREEN_WIDTH/2.8, GUI_SCREEN_CENTER_Y/8);


    char txt_buf[50];
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);//获取系统时间

    snprintf(txt_buf,sizeof(txt_buf),"%02d/%02d %s",time.month,time.day,i18n[STR_SUNDAY+time.week]);
    compo_textbox_t *textbox = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/5.5,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/10.5);
    compo_textbox_set(textbox,txt_buf );

    func_clock_sub_dropdown_battery_pic_update();//下拉电量图标更新
    func_clock_sub_dropdown_bluetooth_pic_update();     //蓝牙更新
    func_clock_sub_dropdown_bluetooth_btn_pic_update();////下拉蓝牙按钮更新
    // func_clock_sub_dropdown_mute_pic_update();          //静音更新
    func_clock_sub_dropdown_password_pic_update();//童锁开关
    func_clock_sub_dropdown_disturb_pic_update();       //勿扰
#if GUI_MODULE_WRIST_SUPPORT
    func_clock_sub_dropdown_wrist_pic_update();//下拉抬婉亮屏按钮更新
#endif
    func_clock_sub_dropdown_menu_pic_update();//下拉菜单->菜单切换按钮更新

    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clk->sub_frm = frm;
    f_clk->masklayer = masklayer;
}

static bool disp_flag=false;
static u8 touch_state=false;
static s32 old_dx = 0;
static s16 touch_last_dx = 0;
uint32_t tick = 0;
#define MOVE_DISP_PIXEL   GUI_SCREEN_WIDTH/24
//下滑菜单左右滑动处理
static void func_clock_sub_dropdown_slide_handle(void)
{
    s32 dy=0,dx=0;
    bool touch_flag = 0;
    compo_picturebox_t *picbox_white = compo_getobj_byid(COMPO_ID_PIC_WHITE);
    compo_picturebox_t *picbox_grey  = compo_getobj_byid(COMPO_ID_PIC_GREY);

    touch_flag = ctp_get_dxy(&dx, &dy);
    if(touch_flag == true)
    {
        old_dx = dx;
        touch_last_dx = ctp_get_last_dxy().x;
        if(disp_flag == true)
        {
            old_dx=old_dx-GUI_SCREEN_WIDTH;
            if(old_dx < -GUI_SCREEN_WIDTH)
            {
                old_dx = -GUI_SCREEN_WIDTH;
            }
        }
        else
        {
            if(old_dx>0)old_dx=0;
        }
        widget_page_set_client(widget, old_dx, 0);
        widget_page_update();
        touch_state = 1;
    }
    else if(touch_state)
    {
        if(touch_state == 1)
        {
            if(disp_flag == false)
            {
                if(old_dx < -GUI_SCREEN_CENTER_X || touch_last_dx <=(-7))
                {
                    touch_state = 2;
                    disp_flag = true;
                }
                else
                {
                    touch_state = 3;
                }
            }
            else
            {
                if(old_dx > -GUI_SCREEN_CENTER_X ||  touch_last_dx >=7)
                {
                    touch_state = 4;
                    disp_flag = false;
                }
                else
                {
                    touch_state = 5;
                }
            }
        }
        if(old_dx == 0 || old_dx == -GUI_SCREEN_WIDTH) touch_state = 0;

        if(tick_check_expire(tick, 3))
        {
            tick = tick_get();
            switch(touch_state)
            {
                case 2:
                    old_dx -=MOVE_DISP_PIXEL;
                    if(old_dx<=-GUI_SCREEN_WIDTH)
                    {
                        touch_state = 0;
                        old_dx=-GUI_SCREEN_WIDTH;
                        compo_picturebox_set(picbox_white,PAGE_NUM_GREY_BIN);
                        compo_picturebox_set(picbox_grey,PAGE_NUM_WHITE_BIN);
                    }
                    break;
                case 3:
                    old_dx+=MOVE_DISP_PIXEL;
                    if(old_dx >= 0)
                    {
                        touch_state = 0;
                        old_dx = 0;
                    }
                    break;
                case 4:
                    old_dx+=MOVE_DISP_PIXEL;
                    if(old_dx >= 0)
                    {
                        touch_state = 0;
                        old_dx = 0;
                        compo_picturebox_set(picbox_white,PAGE_NUM_WHITE_BIN);
                        compo_picturebox_set(picbox_grey,PAGE_NUM_GREY_BIN);
                    }
                    break;
                case 5:
                    old_dx -=MOVE_DISP_PIXEL;
                    if(old_dx <= -GUI_SCREEN_WIDTH)
                    {
                        touch_state = 0;
                        old_dx = -GUI_SCREEN_WIDTH;
                    }
                    break;
            }
            widget_page_set_client(widget, old_dx, 0);
            widget_page_update();
        }
    }
}
static void func_clock_sub_dropdown_click_handler(void)
{
    dropdown_disp_btn_item_t *dropdown_disp_btn_item = (dropdown_disp_btn_item_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch(id)
    {
        case COMPO_ID_BTN_CONNECT:
            if(uteModuleCallBtIsPowerOn())
            {
                uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_BUTTON);
            }
            else
            {
                uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
            }
            func_clock_sub_dropdown_bluetooth_btn_pic_update();
            break;
        case COMPO_ID_BTN_DISCURD:
        {
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
            ute_quick_switch_t quick;
            uteApplicationCommonGetQuickSwitchStatus(&quick);
            quick.isNotDisturb = !quick.isNotDisturb;
            if(quick.isNotDisturb)
            {
                uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_ALLDAY_OPEN);
                uteModuleLiftWristRecordOpenStatus();
                if(uteModuleLiftWristGetOldOpenStatus())
                {
                    uteModuleLiftWristSetOpenStatus(false);
                }
                else if(uteModuleLiftWristGetOldScheduledOpenStatus())
                {
                    uteModuleLiftWristSetScheduled(false);
                }
                if(uteModuleNotDisturbIsOpenScheduled())
                {
                    uteModuleNotDisturbSetScheduled(false);
                }
            }
            else
            {
                uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_CLOSE);
                if(uteModuleLiftWristGetOldOpenStatus())
                {
                    uteModuleLiftWristSetOpenStatus(true);
                }
                else if(uteModuleLiftWristGetOldScheduledOpenStatus())
                {
                    uteModuleLiftWristSetScheduled(true);
                }
            }
            uteApplicationCommonSetQuickSwitchStatus(&quick);
#else
            ute_module_not_disturb_data_t param;
            uteModuleNotDisturbGetParam(&param);
            param.isOpen = !param.isOpen;
            uteModuleNotDisturbSaveParam(param);

            ute_quick_switch_t quick;
            uteApplicationCommonGetQuickSwitchStatus(&quick);
            quick.isNotDisturb = !quick.isNotDisturb;
            uteApplicationCommonSetQuickSwitchStatus(&quick);
#endif
            uteApplicationCommonSendQuickSwitchStatus();
            dropdown_disturb_sw = quick.isNotDisturb;
            func_clock_sub_dropdown_disturb_pic_update();
        }
        break;
        // case COMPO_ID_BTN_MUTE:
        //     // if(sys_cb.mute)
        //     // {
        //     //     bsp_sys_unmute();
        //     // }
        //     // else
        //     // {
        //     //     bsp_sys_mute();
        //     // }
        //     if(uteModuleLocalRingtoneGetMuteStatus())
        //     {
        //         uteModuleLocalRingtoneSetMuteStatus(false,true);
        //     }
        //     else
        //     {
        //         uteModuleLocalRingtoneSetMuteStatus(true,true);
        //     }
        //     func_clock_sub_dropdown_mute_pic_update();          //静音更新
        //     break;
        //点击任务跳转
        case COMPO_ID_PIC_PASSWORD:

            // if(sys_cb.password_flag == true)
            // {
            //     sys_cb.password_flag = false;
            // }
            // else
            // {
            //     sys_cb.password_flag = true;
            // }
            func_cb.sta = FUNC_SET_SUB_PASSWORD;
            break;
        //点击任务跳转
//         case COMPO_ID_BTN_FLASHLIGHT:
// #if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
//             func_cb.sta = FUNC_FLASHLIGHT;
// #endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
//             break;
        case COMPO_ID_BTN_LIGHT:
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
            func_cb.sta = FUNC_LIGHT;
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            break;
        case COMPO_ID_BTN_SETTING:
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
            func_cb.sta = FUNC_SETTING;
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
            break;
        case COMPO_ID_BTN_PHONE:
            func_cb.sta = FUNC_FINDPHONE;
            break;
#if GUI_MODULE_WRIST_SUPPORT
        case COMPO_ID_BTN_WIRST:
            switch_set_sub_wrist();
#endif
        case COMPO_ID_BTN_MENU:
            if(++dropdown_disp_btn_item->sel_idx==MENU_CNT)
            {
                dropdown_disp_btn_item->sel_idx = 0;
            }
            func_cb.menu_style = dwon_tbl_style_list[dropdown_disp_btn_item->sel_idx].menu_style;
            func_clock_sub_dropdown_menu_pic_update();
//           printf("1:%d  2:%d\n",func_cb.menu_style, dwon_tbl_style_list[dropdown_disp_btn_item->sel_idx].menu_style);
            break;
        default:
            break;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define PAGE_HEIGHT     176
#define  BT_ON_PIC_BIN       UI_BUF_I332001_SLIDEMENU_ICON_TELEPHONE_00_BIN    ///BT 连接状态图片
#define  BT_OFF_PIC_BIN      UI_BUF_I332001_SLIDEMENU_ICON_TELEPHONE_01_BIN   ///BT 断开状态图片
#define  BLE_ON_PIC_BIN      UI_BUF_I332001_SLIDEMENU_ICON_CONNECT_00_BIN      ///BLE 连接状态图片
#define  BLE_OFF_PIC_BIN     UI_BUF_I332001_SLIDEMENU_ICON_CONNECT_01_BIN     ///BLE 断开状态图片
#define  BATTERY_PIC_0_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_00_BIN                ///电池电量图标0
#define  BATTERY_PIC_1_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_01_BIN                ///电池电量图标1
#define  BATTERY_PIC_2_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_02_BIN                ///电池电量图标2
#define  BATTERY_PIC_3_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_03_BIN                ///电池电量图标3
#define  BATTERY_PIC_4_BIN   UI_BUF_I332001_SLIDEMENU_ICON_BATT_04_BIN                ///电池电量图标4
#define  PAGE_NUM_WHITE_BIN   UI_BUF_I332001_SLIDEMENU_DOT_01_BIN           ///底部页码点->白
#define  PAGE_NUM_GREY_BIN    UI_BUF_I332001_SLIDEMENU_DOT_00_BIN           ///底部页码点->灰
#define  PAGE_HEIGHT_ADJUST    -3   ///page 页面y轴调节

static uint8_t dropdown_disturb_sw;     //功能未做，加个开关先放着

enum
{
    COMPO_ID_BTN_START = FUNC_MAX_NUM, //从任务最大枚举开始，避免和表盘跳转et的d冲突
    //按键
    COMPO_ID_BTN_CONNECT,       //蓝牙连接开关
    COMPO_ID_BTN_MENU,          //菜单
    COMPO_ID_BTN_FLASHLIGHT,    //手电筒
    COMPO_ID_BTN_WIRST,         //抬婉亮屏
    COMPO_ID_BTN_DISCURD,       //勿扰模式开关
    COMPO_ID_BTN_LIGHT,         //亮度调节
    COMPO_ID_BTN_MUTE,          //静音模式开关
    COMPO_ID_BTN_PHONE,         //查找手机
    COMPO_ID_BTN_SETTING,       //设置

    COMPO_ID_PIC_WHITE,       //底部页码点->白
    COMPO_ID_PIC_GREY,        //底部页码点->灰

    //电池百分比文本
    COMPO_ID_TXT_BATTERY_PERCENT,
    //电池百分比图片
    COMPO_ID_TXT_BATTERY_PIC,
    //蓝牙状态图片
    COMPO_ID_TXT_BLUETOOTH_STA_PIC,
    COMPO_ID_TXT_BTETOOTH_STA_PIC,
};

typedef struct dropdown_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    s16 x;
    s16 y;
    u8 sel_idx;
} dropdown_disp_btn_item_t;

#define DROPDOWN_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_dropdown_disp_btn_item) / sizeof(tbl_dropdown_disp_btn_item[0])))
//类型有显示信息，控制开关，点击跳转任务
//按钮item，创建时遍历一下
static const  dropdown_disp_btn_item_t tbl_dropdown_disp_btn_item[] =
{
    ///*第一页*/
    {UI_BUF_I332001_SLIDEMENU_ICON_CALL00_BIN,          COMPO_ID_BTN_CONNECT,          GUI_SCREEN_CENTER_X-96, 40},///蓝牙连接开关
    {UI_BUF_I332001_SLIDEMENU_ICON_THEME03_BIN,         COMPO_ID_BTN_MENU,             GUI_SCREEN_CENTER_X,    40},///菜单
    {UI_BUF_I332001_SLIDEMENU_ICON_SHOUDIAN_BIN,        COMPO_ID_BTN_FLASHLIGHT,       GUI_SCREEN_CENTER_X+96, 40},///手电筒
    {UI_BUF_I332001_SLIDEMENU_ICON_TAIWAN00_BIN,        COMPO_ID_BTN_WIRST,            GUI_SCREEN_CENTER_X-96, 96+40},///抬婉亮屏
    {UI_BUF_I332001_SLIDEMENU_ICON_DND00_BIN,           COMPO_ID_BTN_DISCURD,          GUI_SCREEN_CENTER_X,    96+40},///勿扰模式开关
    {UI_BUF_I332001_SLIDEMENU_ICON_BRIGHTNESS_BIN,      COMPO_ID_BTN_LIGHT,            GUI_SCREEN_CENTER_X+96, 96+40},///亮度调节
    ///*第二页*/
    {UI_BUF_I332001_SLIDEMENU_ICON_VOLUMES00_BIN,       COMPO_ID_BTN_MUTE,             GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X-96,40},///静音模式开关
    {UI_BUF_I332001_SLIDEMENU_ICON_FINDPHONE_BIN,       COMPO_ID_BTN_PHONE,            GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X,   40},///查找手机
    {UI_BUF_I332001_SLIDEMENU_ICON_SETTINGS_BIN,        COMPO_ID_BTN_SETTING,          GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X+96,40},///设置
};

#define MENU_CNT    ((int)(sizeof(dwon_tbl_style_list) / sizeof(dwon_tbl_style_list[0])))
//风格列表tbl
static const compo_listbox_item_t dwon_tbl_style_list[] =
{
    {STR_STYLE_LIST_1,          UI_BUF_I332001_SLIDEMENU_ICON_THEME03_BIN,           .menu_style = MENU_STYLE_LIST},             //列表
    {STR_GONG_GE,               UI_BUF_I332001_SLIDEMENU_ICON_THEME01_BIN,           .menu_style = MENU_STYLE_CUM_SUDOKU},       //宫格
    {STR_STYLE_HONEYCOMB,       UI_BUF_I332001_SLIDEMENU_ICON_THEME00_BIN,           .menu_style = MENU_STYLE_HONEYCOMB},        //蜂窝
    {STR_SPHERE,                UI_BUF_I332001_SLIDEMENU_ICON_THEME07_BIN,           .menu_style = MENU_STYLE_FOOTBALL},         //球体
    {STR_CHECKERBOARD,          UI_BUF_I332001_SLIDEMENU_ICON_THEME06_BIN,           .menu_style = MENU_STYLE_GRID},             //棋盘
    {STR_HALO,                  UI_BUF_I332001_SLIDEMENU_ICON_THEME02_BIN,           .menu_style = MENU_STYLE_KALE},             //光环
    {STR_STYLE_SKYRER,          UI_BUF_I332001_SLIDEMENU_ICON_THEME08_BIN,           .menu_style = MENU_STYLE_SKYRER},           //天圆地方
    {STR_STYLE_GRID_1,          UI_BUF_I332001_SLIDEMENU_ICON_THEME09_BIN,           .menu_style = MENU_STYLE_CUM_GRID},         //网格
    {STR_SIX_PALACE_GRID,       UI_BUF_I332001_SLIDEMENU_ICON_THEME04_BIN,           .menu_style = MENU_STYLE_CUM_FOURGRID},     //六宫格
    {STR_STYLE_WATERFALL,       UI_BUF_I332001_SLIDEMENU_ICON_THEME05_BIN,           .menu_style = MENU_STYLE_WATERFALL},        //瀑布
};

///               /                 *更新*                    /                 ///

//下拉电量图标更新
static void func_clock_sub_dropdown_battery_pic_update(void)
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
////下拉蓝牙连接标志更新
static void func_clock_sub_dropdown_bluetooth_pic_update(void)
{
    compo_picturebox_t *bluetooth_pic = compo_getobj_byid(COMPO_ID_TXT_BLUETOOTH_STA_PIC);
    compo_picturebox_t *btooth_pic = compo_getobj_byid(COMPO_ID_TXT_BTETOOTH_STA_PIC);
    if (ble_is_connect())
    {
        compo_picturebox_set(bluetooth_pic, BLE_ON_PIC_BIN);
    }
    else
    {
        compo_picturebox_set(bluetooth_pic, BLE_OFF_PIC_BIN);
    }

    if(uteModuleCallBtIsConnected())
    {
        compo_picturebox_set(btooth_pic, BT_ON_PIC_BIN);
    }
    else
    {
        compo_picturebox_set(btooth_pic, BT_OFF_PIC_BIN);
    }

}
//下拉静音图标更新
static void func_clock_sub_dropdown_mute_pic_update(void)
{
    compo_button_t *mute_pic = compo_getobj_byid(COMPO_ID_BTN_MUTE);
    // if(sys_cb.mute)
    if(uteModuleLocalRingtoneGetMuteStatus())
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_I332001_SLIDEMENU_ICON_VOLUMES01_BIN);
    }
    else
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_I332001_SLIDEMENU_ICON_VOLUMES00_BIN);
    }
}

////下拉蓝牙按钮更新
static void func_clock_sub_dropdown_bluetooth_btn_pic_update(void)
{
    compo_button_t *bluetooth_pic = compo_getobj_byid(COMPO_ID_BTN_CONNECT);
    compo_picturebox_t *bluetooth = compo_getobj_byid(COMPO_ID_TXT_BTETOOTH_STA_PIC);
    printf("bt_get_scan: 0x%x\n", bt_get_scan());
    if(uteModuleCallBtIsPowerOn())
    {
        compo_button_set_bgimg(bluetooth_pic, UI_BUF_I332001_SLIDEMENU_ICON_CALL01_BIN);
        compo_picturebox_set_visible(bluetooth, true);
    }
    else
    {
        compo_button_set_bgimg(bluetooth_pic,UI_BUF_I332001_SLIDEMENU_ICON_CALL00_BIN );
        compo_picturebox_set_visible(bluetooth, false);

    }
}
#if GUI_MODULE_WRIST_SUPPORT
//下拉抬婉亮屏按钮状态切换
static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}
//下拉抬婉亮屏按钮更新
static void func_clock_sub_dropdown_wrist_pic_update(void)
{
    compo_button_t *wrist_pic = compo_getobj_byid(COMPO_ID_BTN_WIRST);

    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);

    if(quick.isTurnTheWrist == true)
    {
        compo_button_set_bgimg(wrist_pic, UI_BUF_I332001_SLIDEMENU_ICON_TAIWAN01_BIN);
    }
    else
    {
        compo_button_set_bgimg(wrist_pic, UI_BUF_I332001_SLIDEMENU_ICON_TAIWAN00_BIN);
    }

}
#endif
//下拉勿扰按钮更新
static void func_clock_sub_dropdown_disturb_pic_update(void)
{
    compo_button_t *disturb_pic = compo_getobj_byid(COMPO_ID_BTN_DISCURD);

    if(dropdown_disturb_sw)
    {
        sys_cb.disturd_adl = true;
        compo_button_set_bgimg(disturb_pic, UI_BUF_I332001_SLIDEMENU_ICON_DND01_BIN);
        uteModuleNotDisturbSetOpenStatus(sys_cb.disturd_adl);
    }
    else
    {
        sys_cb.disturd_adl = false;
        compo_button_set_bgimg(disturb_pic, UI_BUF_I332001_SLIDEMENU_ICON_DND00_BIN);
        uteModuleNotDisturbSetOpenStatus(sys_cb.disturd_adl);
    }

}
//菜单id更新
static void get_menu_idx_update(void)
{
    dropdown_disp_btn_item_t *dropdown_disp_btn_item = (dropdown_disp_btn_item_t *)func_cb.f_cb;
    for(int i=0; i<MENU_CNT; i++)
    {
        if(dwon_tbl_style_list[i].menu_style == func_cb.menu_style)
        {
            dropdown_disp_btn_item->sel_idx = i;
            return;
        }
    }
}
//菜单样式图标更新
static void func_clock_sub_dropdown_menu_pic_update(void)
{
    dropdown_disp_btn_item_t *dropdown_disp_btn_item = (dropdown_disp_btn_item_t *)func_cb.f_cb;
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_MENU);
    compo_button_set_bgimg(btn, dwon_tbl_style_list[dropdown_disp_btn_item->sel_idx].res_addr);
    if (func_cb.menu_style == MENU_STYLE_SKYRER)
    {
        u8 func_menu_sub_skyrer_get_first_idx(void);
        func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();
    }
    else
    {
        func_cb.menu_idx = 0;           //切换风格后进入回中心位置
    }
    uteModuleGuiCommonSetThemeTypeId(func_cb.menu_style);
}

///               /                 *更新*                    /                 ///

widget_page_t *widget;///创建按键父局器页面

//创建下拉菜单
static void func_clock_sub_dropdown_form_create(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if(quick.isNotDisturb)
    {
        dropdown_disturb_sw = 1;
    }
    else
    {
        dropdown_disturb_sw = 0;
    }
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    if(uteModuleNotDisturbIsOpenScheduled())
    {
        if(uteModuleNotDisturbIsTimeBucket())
        {
            dropdown_disturb_sw = 1;
        }
        else
        {
            dropdown_disturb_sw = 0;
        }
    }
#endif

    compo_form_t *frm = compo_form_create(true);

    ///创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    widget = widget_page_create(frm->page);///创建按键页面
    widget_page_set_client(widget, 0, 0);
    widget_set_location(widget,GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y+PAGE_HEIGHT_ADJUST,GUI_SCREEN_WIDTH*2,PAGE_HEIGHT);

    ///创建页码点
    compo_picturebox_t *picbox = compo_picturebox_create(frm, PAGE_NUM_WHITE_BIN);
    compo_picturebox_set_pos(picbox,177, 344);
    compo_setid(picbox,COMPO_ID_PIC_WHITE);

    picbox = compo_picturebox_create(frm, PAGE_NUM_GREY_BIN);
    compo_picturebox_set_pos(picbox,189, 344);
    compo_setid(picbox,COMPO_ID_PIC_GREY);

    ///创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < DROPDOWN_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn =compo_button_create_page_by_image(frm,widget, tbl_dropdown_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn,  tbl_dropdown_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_dropdown_disp_btn_item[idx_btn].x, tbl_dropdown_disp_btn_item[idx_btn].y);
    }
    //电池
    compo_picturebox_t *battery_pic = compo_picturebox_create(frm, BATTERY_PIC_4_BIN);
    compo_setid(battery_pic, COMPO_ID_TXT_BATTERY_PIC);
    compo_picturebox_set_pos(battery_pic,224,25);

    //蓝牙状态 Ble
    compo_picturebox_t *bluetooth_pic = compo_picturebox_create(frm, BT_ON_PIC_BIN);
    compo_setid(bluetooth_pic, COMPO_ID_TXT_BLUETOOTH_STA_PIC);
    compo_picturebox_set_pos(bluetooth_pic,180, 25);

    //蓝牙状态 Bt
    bluetooth_pic = compo_picturebox_create(frm, BLE_OFF_PIC_BIN);
    compo_setid(bluetooth_pic, COMPO_ID_TXT_BTETOOTH_STA_PIC);
    compo_picturebox_set_pos(bluetooth_pic,146, 25);


    char txt_buf[30];
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);//获取系统时间

    snprintf(txt_buf,sizeof(txt_buf),"%02d/%02d %s",time.month,time.day,i18n[STR_SUNDAY+time.week]);
    compo_textbox_t *textbox = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,65,170,40);
    compo_textbox_set(textbox,txt_buf );

    func_clock_sub_dropdown_battery_pic_update();//下拉电量图标更新
    func_clock_sub_dropdown_bluetooth_pic_update();     //蓝牙更新
    func_clock_sub_dropdown_bluetooth_btn_pic_update();////下拉蓝牙按钮更新
    func_clock_sub_dropdown_mute_pic_update();          //静音更新
    func_clock_sub_dropdown_disturb_pic_update();       //勿扰
#if GUI_MODULE_WRIST_SUPPORT
    func_clock_sub_dropdown_wrist_pic_update();//下拉抬婉亮屏按钮更新
#endif
    func_clock_sub_dropdown_menu_pic_update();//下拉菜单->菜单切换按钮更新

    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clk->sub_frm = frm;
    f_clk->masklayer = masklayer;
}

static bool disp_flag=false;
static u8 touch_state=false;
static s32 old_dx = 0;
static s16 touch_last_dx = 0;
uint32_t tick = 0;
#define MOVE_DISP_PIXEL   GUI_SCREEN_WIDTH/24
//下滑菜单左右滑动处理
static void func_clock_sub_dropdown_slide_handle(void)
{
    s32 dy=0,dx=0;
    bool touch_flag = 0;
    compo_picturebox_t *picbox_white = compo_getobj_byid(COMPO_ID_PIC_WHITE);
    compo_picturebox_t *picbox_grey  = compo_getobj_byid(COMPO_ID_PIC_GREY);

    touch_flag = ctp_get_dxy(&dx, &dy);
    if(touch_flag == true)
    {
        old_dx = dx;
        touch_last_dx = ctp_get_last_dxy().x;
        if(disp_flag == true)
        {
            old_dx=old_dx-GUI_SCREEN_WIDTH;
            if(old_dx < -GUI_SCREEN_WIDTH)
            {
                old_dx = -GUI_SCREEN_WIDTH;
            }
        }
        else
        {
            if(old_dx>0)old_dx=0;
        }
        widget_page_set_client(widget, old_dx, 0);
        widget_page_update();
        touch_state = 1;
    }
    else if(touch_state)
    {
        if(touch_state == 1)
        {
            if(disp_flag == false)
            {
                if(old_dx < -GUI_SCREEN_CENTER_X || touch_last_dx <=(-7))
                {
                    touch_state = 2;
                    disp_flag = true;
                }
                else
                {
                    touch_state = 3;
                }
            }
            else
            {
                if(old_dx > -GUI_SCREEN_CENTER_X ||  touch_last_dx >=7)
                {
                    touch_state = 4;
                    disp_flag = false;
                }
                else
                {
                    touch_state = 5;
                }
            }
        }
        if(old_dx == 0 || old_dx == -GUI_SCREEN_WIDTH) touch_state = 0;

        if(tick_check_expire(tick, 3))
        {
            tick = tick_get();
            switch(touch_state)
            {
                case 2:
                    old_dx -=MOVE_DISP_PIXEL;
                    if(old_dx<=-GUI_SCREEN_WIDTH)
                    {
                        touch_state = 0;
                        old_dx=-GUI_SCREEN_WIDTH;
                        compo_picturebox_set(picbox_white,PAGE_NUM_GREY_BIN);
                        compo_picturebox_set(picbox_grey,PAGE_NUM_WHITE_BIN);
                    }
                    break;
                case 3:
                    old_dx+=MOVE_DISP_PIXEL;
                    if(old_dx >= 0)
                    {
                        touch_state = 0;
                        old_dx = 0;
                    }
                    break;
                case 4:
                    old_dx+=MOVE_DISP_PIXEL;
                    if(old_dx >= 0)
                    {
                        touch_state = 0;
                        old_dx = 0;
                        compo_picturebox_set(picbox_white,PAGE_NUM_WHITE_BIN);
                        compo_picturebox_set(picbox_grey,PAGE_NUM_GREY_BIN);
                    }
                    break;
                case 5:
                    old_dx -=MOVE_DISP_PIXEL;
                    if(old_dx <= -GUI_SCREEN_WIDTH)
                    {
                        touch_state = 0;
                        old_dx = -GUI_SCREEN_WIDTH;
                    }
                    break;
            }
            widget_page_set_client(widget, old_dx, 0);
            widget_page_update();
        }
    }
}
static void func_clock_sub_dropdown_click_handler(void)
{
    dropdown_disp_btn_item_t *dropdown_disp_btn_item = (dropdown_disp_btn_item_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch(id)
    {
        case COMPO_ID_BTN_CONNECT:
            if(uteModuleCallBtIsPowerOn())
            {
                uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_BUTTON);
            }
            else
            {
                uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
            }
            func_clock_sub_dropdown_bluetooth_btn_pic_update();
            break;
        case COMPO_ID_BTN_DISCURD:
        {
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
            ute_quick_switch_t quick;
            uteApplicationCommonGetQuickSwitchStatus(&quick);
            quick.isNotDisturb = !quick.isNotDisturb;
            if(quick.isNotDisturb)
            {
                uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_ALLDAY_OPEN);
                uteModuleLiftWristRecordOpenStatus();
                if(uteModuleLiftWristGetOldOpenStatus())
                {
                    uteModuleLiftWristSetOpenStatus(false);
                }
                else if(uteModuleLiftWristGetOldScheduledOpenStatus())
                {
                    uteModuleLiftWristSetScheduled(false);
                }
                if(uteModuleNotDisturbIsOpenScheduled())
                {
                    uteModuleNotDisturbSetScheduled(false);
                }
            }
            else
            {
                uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_CLOSE);
                if(uteModuleLiftWristGetOldOpenStatus())
                {
                    uteModuleLiftWristSetOpenStatus(true);
                }
                else if(uteModuleLiftWristGetOldScheduledOpenStatus())
                {
                    uteModuleLiftWristSetScheduled(true);
                }
            }
            uteApplicationCommonSetQuickSwitchStatus(&quick);
#else
            ute_module_not_disturb_data_t param;
            uteModuleNotDisturbGetParam(&param);
            param.isOpen = !param.isOpen;
            uteModuleNotDisturbSaveParam(param);

            ute_quick_switch_t quick;
            uteApplicationCommonGetQuickSwitchStatus(&quick);
            quick.isNotDisturb = !quick.isNotDisturb;
            uteApplicationCommonSetQuickSwitchStatus(&quick);
#endif
            uteApplicationCommonSendQuickSwitchStatus();
            dropdown_disturb_sw = quick.isNotDisturb;
            func_clock_sub_dropdown_disturb_pic_update();
        }
        break;
        case COMPO_ID_BTN_MUTE:
            // if(sys_cb.mute)
            // {
            //     bsp_sys_unmute();
            // }
            // else
            // {
            //     bsp_sys_mute();
            // }
            if(uteModuleLocalRingtoneGetMuteStatus())
            {
                uteModuleLocalRingtoneSetMuteStatus(false,true);
            }
            else
            {
                uteModuleLocalRingtoneSetMuteStatus(true,true);
            }
            func_clock_sub_dropdown_mute_pic_update();          //静音更新
            break;
        //点击任务跳转
        case COMPO_ID_BTN_FLASHLIGHT:
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
            func_cb.sta = FUNC_FLASHLIGHT;
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
            break;
        case COMPO_ID_BTN_LIGHT:
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
            func_cb.sta = FUNC_LIGHT;
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            break;
        case COMPO_ID_BTN_SETTING:
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
            func_cb.sta = FUNC_SETTING;
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
            break;
        case COMPO_ID_BTN_PHONE:
            func_cb.sta = FUNC_FINDPHONE;
            break;
        case COMPO_ID_BTN_WIRST:
            switch_set_sub_wrist();
            break;
        case COMPO_ID_BTN_MENU:
            if(++dropdown_disp_btn_item->sel_idx==MENU_CNT)
            {
                dropdown_disp_btn_item->sel_idx = 0;
            }
            func_cb.menu_style = dwon_tbl_style_list[dropdown_disp_btn_item->sel_idx].menu_style;
            func_clock_sub_dropdown_menu_pic_update();
//           printf("1:%d  2:%d\n",func_cb.menu_style, dwon_tbl_style_list[dropdown_disp_btn_item->sel_idx].menu_style);
            break;
        default:
            break;
    }
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//时钟表盘主要事件流程处理
static void func_clock_sub_dropdown_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_clock_sub_dropdown_slide_handle();
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    func_clock_sub_dropdown_slide_handle();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_clock_sub_process();
}

//时钟表盘下拉菜单功能消息处理
static void func_clock_sub_dropdown_message(size_msg_t msg)
{
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            func_clock_sub_dropdown_click_handler();
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
            func_clock_sub_dropdown_click_handler();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;
        case MSG_CTP_SHORT_LEFT:
            break;
        case MSG_CTP_SHORT_RIGHT:
            break;
        case MSG_CTP_SHORT_UP:
            if (func_switching(FUNC_SWITCH_MENU_DROPDOWN_UP, NULL))
            {
                f_clk->sta = FUNC_CLOCK_MAIN;                   //上滑返回到时钟主界面
            }
            break;

        case KU_BACK:
            func_switching(FUNC_SWITCH_MENU_DROPDOWN_UP | FUNC_SWITCH_AUTO, NULL);
            f_clk->sta = FUNC_CLOCK_MAIN;                       //单击BACK键返回到时钟主界面
            break;
        case MSG_SYS_1S:
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            func_clock_sub_dropdown_battery_pic_update();//下拉电量图标更新
            func_clock_sub_dropdown_bluetooth_pic_update();     //蓝牙更新
            func_clock_sub_dropdown_bluetooth_btn_pic_update();////下拉蓝牙按钮更新
            //   func_clock_sub_dropdown_mute_pic_update();          //静音更新
            func_clock_sub_dropdown_disturb_pic_update();       //勿扰
#if GUI_MODULE_WRIST_SUPPORT
            func_clock_sub_dropdown_wrist_pic_update();//下拉抬婉亮屏按钮更新
#endif
            func_clock_sub_dropdown_menu_pic_update();//下拉菜单->菜单切换按钮更新
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
            func_clock_sub_dropdown_battery_pic_update();//下拉电量图标更新
            func_clock_sub_dropdown_bluetooth_pic_update();     //蓝牙更新
            func_clock_sub_dropdown_bluetooth_btn_pic_update();////下拉蓝牙按钮更新
            func_clock_sub_dropdown_mute_pic_update();          //静音更新
            func_clock_sub_dropdown_disturb_pic_update();       //勿扰
#if GUI_MODULE_WRIST_SUPPORT
            func_clock_sub_dropdown_wrist_pic_update();//下拉抬婉亮屏按钮更新
#endif
            func_clock_sub_dropdown_menu_pic_update();//下拉菜单->菜单切换按钮更新
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;
        case MSG_QDEC_BACKWARD:
            printf("MSG_QDEC_BACKWARD\n");
            break;

        case MSG_QDEC_FORWARD:
            printf("MSG_QDEC_FORWARD\n");
            break;

        case EVT_CLOCK_DROPDOWN_EXIT:
            f_clk->sta = FUNC_CLOCK_MAIN;                       //返回到时钟主界面
            break;

        default:
            func_clock_sub_message(msg);
            break;
    }
}

//时钟表盘下拉菜单进入处理
static void func_clock_sub_dropdown_enter(void)
{
    if(uteModuleNotDisturbGetOpenStatus() == true)
    {
        dropdown_disturb_sw = 1;
    }
    else
    {
        dropdown_disturb_sw = 0;
    }

    func_clock_butterfly_set_light_visible(false);
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    get_menu_idx_update();
    disp_flag=false;
    touch_state=false;
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    get_menu_idx_update();
    disp_flag=false;
    touch_state=false;
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_clock_sub_dropdown_form_create();
    if (!func_switching(FUNC_SWITCH_MENU_DROPDOWN_DOWN, NULL))
    {
        return;                                             //下拉到一半取消
    }
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clk->sta = FUNC_CLOCK_SUB_DROPDOWN;                   //进入到下拉菜单
}

//时钟表盘下拉菜单退出处理
/* static  */void func_clock_sub_dropdown_exit(void)
{
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    compo_form_destroy(f_clk->sub_frm);
    func_clock_butterfly_set_light_visible(true);
    f_clk->sub_frm = NULL;
}

//时钟表盘下拉菜单
void func_clock_sub_dropdown(void)
{
    func_clock_sub_dropdown_enter();
    while (func_cb.sta == FUNC_CLOCK && ((f_clock_t *)func_cb.f_cb)->sta == FUNC_CLOCK_SUB_DROPDOWN)
    {
        func_clock_sub_dropdown_process();
        func_clock_sub_dropdown_message(msg_dequeue());
    }
    func_clock_sub_dropdown_exit();
}
#endif // UTE_MODULE_SCREENS_DWON_MENU_SUPPORT
