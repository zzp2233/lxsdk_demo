#include "include.h"
#include "func.h"
#include "ute_application_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_RESTART_SUPPORT

typedef struct f_restart_t_
{

} f_restart_t;

enum
{
    COMPO_ID_BTN_NO = 1,
    COMPO_ID_BTN_YES,
};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//重启页面
compo_form_t *func_set_sub_restart_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_RESTART]);

    //创建文本
    compo_textbox_t *txt_rst = compo_textbox_create(frm, strlen(i18n[STR_SURE_REBOOT]));
//    compo_textbox_set_align_center(txt_rst, false);
    compo_textbox_set(txt_rst, i18n[STR_SURE_REBOOT]);
    compo_textbox_set_location(txt_rst, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/2.2,GUI_SCREEN_WIDTH/1.1,widget_text_get_area(txt_rst->txt).hei);
    compo_textbox_set(txt_rst, i18n[STR_SURE_REBOOT]);

    //创建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
    compo_setid(btn, COMPO_ID_BTN_NO);
    compo_button_set_pos(btn, 65, 240);

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
    compo_setid(btn, COMPO_ID_BTN_YES);
    compo_button_set_pos(btn, 175, 240);

    return frm;
}

//按键事件处理
static void func_restart_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_YES:
            // ble_disconnect();
            // bt_disconnect(1);
            // WDT_RST();
            uteApplicationCommonRestart();
            break;

        case COMPO_ID_BTN_NO:
//            task_stack_pop();
            func_back_to();
//            if(func_cb.last == FUNC_SETTING)
//            {
//                func_cb.sta = FUNC_SETTING;
//            }
//            else
//            {
//                func_cb.sta = FUNC_MENU;
//            }
            break;

        default:
            break;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//重启页面
compo_form_t *func_set_sub_restart_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_SETTING_RESTART]);

    //创建文本
    compo_textbox_t *txt_rst = compo_textbox_create(frm, strlen(i18n[STR_SURE_REBOOT]));
//    compo_textbox_set_align_center(txt_rst, false);
    compo_textbox_set(txt_rst, i18n[STR_SURE_REBOOT]);
    compo_textbox_set_location(txt_rst, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/2.8,GUI_SCREEN_WIDTH/1.3,widget_text_get_area(txt_rst->txt).hei);
    compo_textbox_set(txt_rst, i18n[STR_SURE_REBOOT]);

    //创建按钮
    compo_button_t *btn;
//    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
    compo_setid(btn, COMPO_ID_BTN_NO);
    compo_button_set_pos(btn, 65, 240);

//    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
    compo_setid(btn, COMPO_ID_BTN_YES);
    compo_button_set_pos(btn, 175, 240);

    return frm;
}

//按键事件处理
static void func_restart_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_YES:
            // ble_disconnect();
            // bt_disconnect(1);
            // WDT_RST();
            uteApplicationCommonRestart();
            break;

        case COMPO_ID_BTN_NO:
//            task_stack_pop();
            func_back_to();
//            if(func_cb.last == FUNC_SETTING)
//            {
//                func_cb.sta = FUNC_SETTING;
//            }
//            else
//            {
//                func_cb.sta = FUNC_MENU;
//            }
            break;

        default:
            break;
    }
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//重启功能事件处理
static void func_set_sub_restart_process(void)
{
    func_process();
}

//重启功能消息处理
static void func_set_sub_restart_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_restart_button_click();                  //单击按钮
            break;

        case KU_DELAY_BACK:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入重启功能
static void func_set_sub_restart_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_restart_t));
    func_cb.frm_main = func_set_sub_restart_form_create();
}

//退出重启功能
static void func_set_sub_restart_exit(void)
{
    func_cb.last = FUNC_SET_SUB_RESTART;
}

//重启功能
void func_set_sub_restart(void)
{
    printf("%s\n", __func__);
    func_set_sub_restart_enter();
    while (func_cb.sta == FUNC_SET_SUB_RESTART)
    {
        func_set_sub_restart_process();
        func_set_sub_restart_message(msg_dequeue());
    }
    func_set_sub_restart_exit();
}

#endif
