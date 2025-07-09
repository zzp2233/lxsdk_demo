#include "include.h"
#include "func.h"
#include "ute_module_log.h"
#include "string.h"
#include "ute_application_common.h"
#include "ute_module_systemtime.h"


#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define SHAPE_HEIGTH  GUI_SCREEN_HEIGHT/4.5

extern void func_debug_info_check();

typedef struct f_about_t_
{
    //bool flag_drag;                 //开始拖动
    s32 x_pos;
    s32 y_pos;
    s32 x;
    s32 y;

} f_about_t;

//关于页面
compo_form_t *func_set_sub_about_form_create(void)
{
    compo_textbox_t * txt;
//    compo_shape_t *shape;

//    uint8_t davName[40];
//    uint8_t Ble_Address[6];
//    uint8_t Ble_Address_str_buf[17+2];

//
//    memset(Ble_Address,'\0',sizeof(Ble_Address));
//    memset(davName,'\0',sizeof(davName));
//
//    uteModulePlatformGetDevName(davName,&davNameLength);//获取设备名称
//    uteModulePlatformGetBleMacAddress(Ble_Address);//获取蓝牙地址
//   // UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL,"11111:%s",davName);
    char davName[40];
    memset(davName,'\0',sizeof(davName));
    uint8_t davNameLength = sizeof(davName);
    uteModulePlatformGetDevName((uint8_t*)davName,&davNameLength);//获取设备名称

    uint8_t Ble_Address[6];//获取蓝牙地址数组
    char Ble_Address_str_buf[20];//蓝牙地址文本数组
    memset(Ble_Address_str_buf,'\0',sizeof(Ble_Address_str_buf));//初始化数组
    uteModulePlatformGetBleMacAddress(Ble_Address);//获取蓝牙地址

    snprintf((char *)Ble_Address_str_buf, sizeof(Ble_Address_str_buf), "%02X:%02X:%02X:%02X:%02X:%02X",\
             Ble_Address[0],Ble_Address[1],Ble_Address[2],Ble_Address[3],Ble_Address[4],Ble_Address[5]); //信息
    char Ble_last_four[10];
    snprintf(Ble_last_four, sizeof(Ble_last_four), "%02X%02X", Ble_Address[4], Ble_Address[5]);

    // 组合蓝牙名称和后四位地址
    char combined_name[50];
    snprintf(combined_name, sizeof(combined_name), "%s(ID-%s)", davName, Ble_last_four);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_ABOUT]);

    //设备名称
//    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/30, SHAPE_HEIGTH);
//    compo_shape_set_radius(shape,SHAPE_HEIGTH/5);
//    compo_shape_set_color(shape,COLOR_DGRAY);

    txt = compo_textbox_create(frm,strlen(i18n[STR_DEV_NEME]));
    compo_textbox_set(txt, i18n[STR_DEV_NEME]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2-GUI_SCREEN_CENTER_Y/1.8);

    txt = compo_textbox_create(frm,strlen(combined_name));
    compo_textbox_set(txt, combined_name);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 -GUI_SCREEN_CENTER_Y/1.8);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //系统版本
//    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/30, SHAPE_HEIGTH);
//    compo_shape_set_radius(shape,SHAPE_HEIGTH/5);
//    compo_shape_set_color(shape,COLOR_DGRAY);

    txt = compo_textbox_create(frm,strlen(i18n[STR_SYS_VERSION]));
    compo_textbox_set(txt, i18n[STR_SYS_VERSION]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1  - SHAPE_HEIGTH/2.2);

    txt = compo_textbox_create(frm,strlen(UTE_SW_VERSION));
    compo_textbox_set(txt, UTE_SW_VERSION);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 );
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //蓝牙地址
//    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y*1.5, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/30, SHAPE_HEIGTH);
//    compo_shape_set_radius(shape,SHAPE_HEIGTH/5);
//    compo_shape_set_color(shape,COLOR_DGRAY);

    txt = compo_textbox_create(frm,strlen(i18n[STR_BLE_MAC]));
    compo_textbox_set(txt, i18n[STR_BLE_MAC]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2+GUI_SCREEN_CENTER_Y/1.8);

    txt = compo_textbox_create(frm,strlen(Ble_Address_str_buf));
    compo_textbox_set(txt, (char*)Ble_Address_str_buf);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 +GUI_SCREEN_CENTER_Y/1.8);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    return frm;
}

//关于功能事件处理
//static void func_set_sub_about_process(void)
//{
//    s32 h_x = 0, h_y = 0;
//    f_about_t *slp = (f_about_t *)func_cb.f_cb;
//    widget_page_t *page_body = func_cb.frm_main->page_body;
//
//    if(slp->flag_drag) {
//        slp->flag_drag = ctp_get_dxy(&h_x, &h_y);
//         if (slp->flag_drag ) {
//             slp->y = slp->y_pos + h_y;
//             widget_page_set_client(page_body, 0, slp->y);
//         } else {
//              slp->y_pos = slp->y;
//         }
//    }
//    func_debug_info_check();
//    func_process();
//}


//关于功能消息处理
static void func_set_sub_about_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_SHORT_DOWN:
            //  func_switch_to(FUNC_POWER_ON_SCAN, FUNC_SWITCH_UD_ZOOM_DOWN | FUNC_SWITCH_AUTO);
            break;
        case KL_BACK:
            func_switch_to(FUNC_LONG_PRESS, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
            break;
        case MSG_CTP_LONG:
            if (!uteApplicationCommonIsHasConnectOurApp())
            {
                uteTaskGuiStartScreen(FUNC_TEST_MODE, 0, __func__);
            }
            break;
        default:
            func_message(msg);
            break;
    }


}

//进入关于功能
static void func_set_sub_about_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_about_t));
    func_cb.frm_main = func_set_sub_about_form_create();
}

//退出关于功能
static void func_set_sub_about_exit(void)
{
    func_cb.last = FUNC_SET_SUB_ABOUT;
}

//关于功能
void func_set_sub_about(void)
{
    printf("%s\n", __func__);
    func_set_sub_about_enter();
    while (func_cb.sta == FUNC_SET_SUB_ABOUT)
    {
        //func_set_sub_about_process();
        func_process();
        func_set_sub_about_message(msg_dequeue());
    }
    func_set_sub_about_exit();
}
