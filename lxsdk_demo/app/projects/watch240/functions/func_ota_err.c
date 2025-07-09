
#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
enum
{
    COMPO_ID_TXT_OTA_ERR1 = 1,
    COMPO_ID_TXT_OTA_ERR2,
};

typedef struct f_ota_err_t_
{

} f_ota_err_t;

//创建窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_OTA_02_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 35+112/2);

    //TXT1 升级失败
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR1);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(255,255,255));
    compo_textbox_set(txt, i18n[STR_UPDATE_FAILED]);

    //TXT2 请连接App重试
    txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR2);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 215+26/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(128,128,128));
    compo_textbox_set(txt,i18n[STR_APP_AGIAN]);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum
{
    COMPO_ID_TXT_OTA_ERR1 = 1,
    COMPO_ID_TXT_OTA_ERR2,
};

typedef struct f_ota_err_t_
{

} f_ota_err_t;

//创建窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_OTA_02_BIN);
//    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 35+112/2);

    //TXT1 升级失败
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR1);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(255,255,255));
    compo_textbox_set(txt, i18n[STR_UPDATE_FAILED]);

    //TXT2 请连接App重试
    txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR2);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 215+26/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(128,128,128));
    compo_textbox_set(txt,i18n[STR_APP_AGIAN]);

    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//地图功能事件处理
static void func_ota_err_process(void)
{
    func_process();
}
#include "func_cover.h"
//地图功能消息处理
static void func_ota_err_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_SHORT_DOWN:

            break;

        case KU_BACK:
            func_switch_to(FUNC_CLOCK, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
            break;

        default:
            func_message(msg);
            break;
    }


}

//进入地图功能
static void func_ota_err_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_ota_err_t));
    func_cb.frm_main = func_ota_err_form_create();
}

//退出地图功能
static void func_ota_err_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_OTA_ERROR;
}

//地图功能
void func_ota_err(void)
{
    printf("%s\n", __func__);
    func_ota_err_enter();
    while (func_cb.sta == FUNC_OTA_ERROR)
    {
        func_ota_err_process();
        func_ota_err_message(msg_dequeue());
    }
    func_ota_err_exit();
}
