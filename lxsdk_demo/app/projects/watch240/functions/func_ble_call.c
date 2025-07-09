#include "include.h"
#include "func.h"
#include "ute_module_call.h"
#include "ute_module_platform.h"
#include "ute_module_gui_common.h"
#include "ute_drv_motor.h"


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
#define TXT_X_MIN 20
enum
{
    COMPO_ID_TXT_NUMBER = 0x01,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
};

typedef struct f_ble_call_t_
{
    u16 times;                  //通话秒数
    char call_time_str[10];     //通话计时字符串
    char pbap_result_Name[50];//存放来电与接听联系人名字
    char tmp_pbap_result_Name[50];//存放来电与接听联系人名字
} f_ble_call_t;


//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_ble_call_form_create(void)
{
    //printf("%s\n", __func__);

//    char txt_buf[20]="121353461";///假数据

    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    ute_bt_call_data_t callData;
    uteModuleCallGetData(&callData);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5-GUI_SCREEN_CENTER_Y/6, GUI_SCREEN_WIDTH/1.2, 50);
//    compo_textbox_set_autosize(name_txt, true);
    compo_textbox_set(name_txt, (char*)callData.name);
//    compo_textbox_set(name_txt, "中国移动");
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

//    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
//    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5+8, GUI_SCREEN_WIDTH/1.2, 50);
////    compo_textbox_set_autosize(number_txt, true);
//    compo_textbox_set(number_txt, (char*)callData.number);
//    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5+GUI_SCREEN_CENTER_Y/6+16, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, 120, 240);



    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define TXT_X_MIN 20
enum
{
    COMPO_ID_TXT_NUMBER = 0x01,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
};

typedef struct f_ble_call_t_
{
    u16 times;                  //通话秒数
    char call_time_str[10];     //通话计时字符串
    char pbap_result_Name[50];//存放来电与接听联系人名字
    char tmp_pbap_result_Name[50];//存放来电与接听联系人名字
} f_ble_call_t;


//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_ble_call_form_create(void)
{
    //printf("%s\n", __func__);

//    char txt_buf[20]="121353461";///假数据

    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);

    ute_bt_call_data_t callData;
    uteModuleCallGetData(&callData);

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(name_txt, (char*)callData.name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);


    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);

    //挂断
    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I332001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 279);

    return frm;
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT


void func_ble_ring_number_update(void)
{
    if (bt_cb.number_sta)
    {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
        bt_pbap_lookup_number((char*)hfp_get_last_call_number(0));
    }
}

void func_ble_call_process(void)
{
    f_ble_call_t *f_bt_ring = (f_ble_call_t*)func_cb.f_cb;

    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = 1;
    }
    reset_sleep_delay_all();

    if(strcmp(f_bt_ring->pbap_result_Name, sys_cb.pbap_result_Name)!=0)
    {
        s16 txt_leng;
        s16 txt_x=0;

        memcpy(f_bt_ring->pbap_result_Name, sys_cb.pbap_result_Name, 50);

        memset(f_bt_ring->tmp_pbap_result_Name, '\0', sizeof(f_bt_ring->tmp_pbap_result_Name));
//        extern void truncate_and_append(const char *src, char *dst, int dst_size);
        truncate_and_append(sys_cb.pbap_result_Name, f_bt_ring->tmp_pbap_result_Name, sizeof(f_bt_ring->tmp_pbap_result_Name));

        compo_textbox_t *name_txt  = compo_getobj_byid(COMPO_ID_TXT_NAME);
        compo_textbox_set(name_txt, f_bt_ring->tmp_pbap_result_Name);

        txt_leng = widget_text_get_area(name_txt->txt).wid;
        txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
        if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
        compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_height()/2);
    }
    func_process();
}

static void func_ble_call_click(void)
{
//     f_ble_call_t *f_ble_call = (f_ble_call_t *)func_cb.f_cb;

    int id = compo_get_button_id();
    switch (id)
    {

        case COMPO_ID_BTN_REJECT:
            uteModulePlatformRejectIncall();
            uteModuleGuiCommonGoBackLastScreen();
            break;

        default:
            break;
    }

}

//蓝牙音乐消息处理
static void func_ble_call_message(size_msg_t msg)
{
//    f_ble_call_t *f_ble_call = (f_ble_call_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_ble_call_click();
            break;
        case KL_BACK:
            uteModulePlatformRejectIncall();
            uteModuleGuiCommonGoBackLastScreen();
//        case EVT_CALL_NUMBER_UPDATE:
//            func_ble_ring_number_update();
//            break;
        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_SHORT_LEFT:
            break;

        default:
            func_message(msg);
            break;
    }
}

void func_ble_call_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_ble_call_t));
    func_cb.frm_main = func_ble_call_form_create();
    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 0xff);
}

void func_ble_call_exit(void)
{
//    f_ble_call_t *f_ble_call = (f_ble_call_t *)func_cb.f_cb;
    uteDrvMotorStop();
    func_cb.last = FUNC_BLE_CALL;
}

void func_ble_call(void)
{
    printf("%s\n", __func__);
    func_ble_call_enter();
    while (func_cb.sta == FUNC_BLE_CALL)
    {
        func_ble_call_process();
        func_ble_call_message(msg_dequeue());
    }
    func_ble_call_exit();
}

