#include "include.h"
#include "func.h"
#include "ute_module_findphone.h"
#include "func_cover.h"
#include "ute_module_call.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT

#define FINDPHONE_POS_Y_MAX        170// 240
#define FINDPHONE_POS_Y_MIN        108// 140

typedef struct f_findphone_t_
{
    s16 angle;
} f_findphone_t;

enum
{
    COMPO_ID_TEXT_FIND = 1,
    COMPO_ID_BUTTON_FIND,
    COMPO_ID_PIC_FIND_BG,

};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I330001_FINDPHONE_BG_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 129);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_THEME_1_FINDPHONE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 129);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 247);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 247);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            if(bt_a2dp_profile_completely_connected())
            {
                // bt_a2dp_profile_dis();
            }
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            // bt_a2dp_profile_en();
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I332001_FINDPHONE_BG_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 80+86);

    pic = compo_picturebox_create(frm, UI_BUF_I332001_THEME_ICON1_FINDPHONE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 80+86);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 64/2+266);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 64/2+266);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            if(bt_a2dp_profile_completely_connected())
            {
                bt_a2dp_profile_dis();
            }
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            bt_a2dp_profile_en();
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//查找手机功能事件处理
static void func_findphone_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_process();
}

//查找手机功能消息处理
static void func_findphone_message(size_msg_t msg)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            if (!ble_is_connect())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
                msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
            }
            else
            {
                func_findphone_button_touch_handle();
            }
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            if (!ble_is_connect())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
                msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
            }
            else
            {
                func_findphone_button_touch_handle();
            }
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//进入查找手机功能
static void func_findphone_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_findphone_t));
    func_cb.frm_main = func_findphone_form_create();

    if(bt_a2dp_profile_completely_connected())
    {
        // bt_audio_bypass();
        bt_a2dp_profile_dis();
    }
//    ab_app_search_phone(true);
}

//退出查找手机功能
static void func_findphone_exit(void)
{
    func_cb.last = FUNC_FINDPHONE;

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        uteModuleFindPhoneStopRing();
    }

//    ab_app_search_phone(false);
    if (bt_hfp_is_connected() && uteModuleCallIsEntertranmentVoiceOn() && !bt_a2dp_profile_completely_connected())
    {
        bt_a2dp_profile_en();
    }
}

//查找手机功能
void func_findphone(void)
{
    printf("%s\n", __func__);
    func_findphone_enter();
    while (func_cb.sta == FUNC_FINDPHONE)
    {
        func_findphone_process();
        func_findphone_message(msg_dequeue());
    }
    func_findphone_exit();
}

#endif
