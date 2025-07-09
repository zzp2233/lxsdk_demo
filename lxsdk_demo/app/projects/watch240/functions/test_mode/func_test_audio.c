#include "include.h"
#include "func.h"

#define TXT_SPACING_Y  GUI_SCREEN_HEIGHT/10
#define TXT_COLOR      make_color(0x66,0x00,0xcc)

enum
{
    BLUETOOTH_STATE_TXT_ID=1,
    BLUETOOTH_SWITCH_TXT_ID,
    BLUETOOTH_SWITCH_BTN_ID,
    UP_SWITCH_BTN_ID,
    UP_SWITCH_TXT_ID,
    DOWN_SWITCH_BTN_ID,
    DOWN_SWITCH_TXT_ID,
    EXIT_BTN_ID,
};



typedef struct f_audio_t_
{
    bool bluetooth;
    bool up_switch_flag;
    bool dwon_switch_flag;
} f_audio_t;

compo_form_t * func_audio_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t * textbox = compo_textbox_create(frm, 5 );
    if(!bt_get_scan())
    {
        compo_textbox_set(textbox, "蓝牙未连接");
        compo_textbox_set_forecolor(textbox, COLOR_RED);
    }
    else
    {
        compo_textbox_set(textbox, "蓝牙已连接");
        compo_textbox_set_forecolor(textbox, COLOR_GREEN);
    }

    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING_Y);
    compo_setid(textbox,BLUETOOTH_STATE_TXT_ID);

    textbox = compo_textbox_create(frm, 5 );
    if(!bt_get_scan())
    {
        compo_textbox_set(textbox, "蓝牙未开启");
        compo_textbox_set_forecolor(textbox, TXT_COLOR);
    }
    else
    {
        compo_textbox_set(textbox, "蓝牙已开启");
        compo_textbox_set_forecolor(textbox, COLOR_GREEN);
    }
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING_Y*2);
    compo_setid(textbox,BLUETOOTH_SWITCH_TXT_ID);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, TXT_SPACING_Y*2, widget_text_get_area(textbox->txt).wid,  widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,BLUETOOTH_SWITCH_BTN_ID);

    textbox = compo_textbox_create(frm, 4 );
    compo_textbox_set(textbox, "上行关闭");
    compo_textbox_set_forecolor(textbox, TXT_COLOR);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, TXT_SPACING_Y*4);
    compo_setid(textbox,UP_SWITCH_TXT_ID);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, TXT_SPACING_Y*4, widget_text_get_area(textbox->txt).wid,  widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,UP_SWITCH_BTN_ID);

    textbox = compo_textbox_create(frm, 4 );
    compo_textbox_set(textbox, "下行关闭");
    compo_textbox_set_forecolor(textbox, TXT_COLOR);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, TXT_SPACING_Y*4);
    compo_setid(textbox,DOWN_SWITCH_TXT_ID);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, TXT_SPACING_Y*4, widget_text_get_area(textbox->txt).wid,  widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,DOWN_SWITCH_BTN_ID);

    textbox = compo_textbox_create(frm, 4 );
    compo_textbox_set(textbox, "退出测试");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING_Y*9);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, TXT_SPACING_Y*9, widget_text_get_area(textbox->txt).wid,  widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,EXIT_BTN_ID);

    uint8_t Ble_Address[6];//获取蓝牙地址数组
    char Ble_Address_str_buf[20];//蓝牙地址文本数组
    memset(Ble_Address_str_buf,'\0',sizeof(Ble_Address_str_buf));//初始化数组
    uteModulePlatformGetBleMacAddress(Ble_Address);//获取蓝牙地址

    snprintf((char *)Ble_Address_str_buf, sizeof(Ble_Address_str_buf), "%02X:%02X:%02X:%02X:%02X:%02X",\
             Ble_Address[0],Ble_Address[1],Ble_Address[2],Ble_Address[3],Ble_Address[4],Ble_Address[5]); //信息

    compo_qrcodebox_t *qrcodebox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, strlen(Ble_Address_str_buf));
    compo_qrcodebox_set_bitwid_by_qrwid(qrcodebox, GUI_SCREEN_CENTER_X*2/4);
    compo_qrcodebox_set_pos(qrcodebox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/4);
    compo_qrcodebox_set(qrcodebox, Ble_Address_str_buf);

    return frm;
}


///老化测试按键处理
static void func_test_audio_click(void)
{
    f_audio_t *f_audio = (f_audio_t*)func_cb.f_cb;
    int id = compo_get_button_id();
//    compo_textbox_t * textbox1 = compo_getobj_byid(BLUETOOTH_STATE_TXT_ID);
    compo_textbox_t * textbox2 = compo_getobj_byid(BLUETOOTH_SWITCH_TXT_ID);
    compo_textbox_t * textbox3 = compo_getobj_byid(UP_SWITCH_TXT_ID);
    compo_textbox_t * textbox4 = compo_getobj_byid(DOWN_SWITCH_TXT_ID);

    switch(id)
    {
        case BLUETOOTH_SWITCH_BTN_ID:
            printf("2222222222222222222222\n");
            f_audio->bluetooth ^=1;
            if(f_audio->bluetooth == true)
            {
                compo_textbox_set(textbox2, "蓝牙已开启");
                compo_textbox_set_forecolor(textbox2, COLOR_GREEN);
            }
            else
            {
                compo_textbox_set(textbox2, "蓝牙已关闭");
                compo_textbox_set_forecolor(textbox2, TXT_COLOR);
            }
            if(bt_get_scan())
            {
                if (bt_is_connected())
                {
                    bt_disconnect(0);
                }
                bt_scan_disable();
            }
            else
            {
                bt_scan_enable();
                bt_connect();
            }
            break;
        case UP_SWITCH_BTN_ID:
            printf("333333333333\n");
            f_audio->up_switch_flag ^=1;
            if(f_audio->up_switch_flag == true)
            {
                compo_textbox_set(textbox3, "上行开启");
                compo_textbox_set_forecolor(textbox3, COLOR_GREEN);
            }
            else
            {
                compo_textbox_set(textbox3, "上行关闭");
                compo_textbox_set_forecolor(textbox3, TXT_COLOR);
            }
            break;
        case DOWN_SWITCH_BTN_ID:
            f_audio->dwon_switch_flag ^=1;
            if(f_audio->dwon_switch_flag == true)
            {
                compo_textbox_set(textbox4, "下行开启");
                compo_textbox_set_forecolor(textbox4, COLOR_GREEN);
            }
            else
            {
                compo_textbox_set(textbox4, "下行关闭");
                compo_textbox_set_forecolor(textbox4, TXT_COLOR);
            }
            break;
        case EXIT_BTN_ID:
            func_back_to();
            break;
    }

}
///老化测试功能消息处理
static void func_audio_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_test_audio_click();
            printf("111111111111111111111111\n");
            break;

        default:
            func_message(msg);
            break;
    }
}
///老化测试功能事件处理
static void func_audio_process(void)
{
    compo_textbox_t * textbox1 = compo_getobj_byid(BLUETOOTH_STATE_TXT_ID);

    if(!bt_is_connected())
    {
        compo_textbox_set(textbox1, "蓝牙未连接");
        compo_textbox_set_forecolor(textbox1, COLOR_RED);
    }
    else
    {
        compo_textbox_set(textbox1, "蓝牙已连接");
        compo_textbox_set_forecolor(textbox1, COLOR_RED);
    }

    func_process();
}
///进入老化测试功能
static void func_audio_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_audio_t));
    func_cb.frm_main = func_audio_create();
}
///退出老化测试功能
static void func_audio_exit(void)
{
    func_cb.last = FUNC_AUDIO;
}
///老化测试功能
void func_audio(void)
{
    printf("%s\n", __func__);
    func_audio_enter();
    while (func_cb.sta == FUNC_AUDIO)
    {
        func_audio_process();
        func_audio_message(msg_dequeue());
    }
    func_audio_exit();
}



