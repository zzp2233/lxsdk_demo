#include "include.h"
#include "func.h"
#include "ute_application_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
typedef struct f_power_on_scan_t_
{

} f_power_on_scan_t;

//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_QRCODE]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_PUBLIC_MORE_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, 24/4+278-FORM_TITLE_HEIGHT);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_DOWNLOAD]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,235,GUI_SCREEN_WIDTH,28);
    compo_textbox_set(textbox,i18n[STR_APP_DOWNLOAD]);

    static const uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    uteApplicationCommonGetDeviceQrCodeLink(qr_str,maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set(qrbox, qr_str);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, GUI_SCREEN_CENTER_X*0.7);
    uteModulePlatformMemoryFree(qr_str);
    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
typedef struct f_power_on_scan_t_
{

} f_power_on_scan_t;

//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_QRCODE]);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_DOWNLOAD]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,235,GUI_SCREEN_WIDTH,28);
    compo_textbox_set(textbox,i18n[STR_APP_DOWNLOAD]);

    static const uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    uteApplicationCommonGetDeviceQrCodeLink(qr_str,maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set(qrbox, qr_str);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, GUI_SCREEN_CENTER_X*0.7);
    uteModulePlatformMemoryFree(qr_str);

//    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_PUBLIC_MORE_BIN);
//    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/1.15);

    return frm;
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//扫一扫功能事件处理
static void func_power_on_scan_process(void)
{
    func_process();
}

//扫一扫功能消息处理
static void func_power_on_scan_message(size_msg_t msg)
{

    if(sys_cb.power_on_state==false)
    {
        switch (msg)
        {
            case MSG_CTP_SHORT_RIGHT:
                func_switch_to(FUNC_POWER_ON_LANGUAGE, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
                break;
            case MSG_CTP_SHORT_UP:
                func_switch_to(FUNC_SET_SUB_ABOUT, FUNC_SWITCH_UD_ZOOM_UP | FUNC_SWITCH_AUTO);
                break;
            case MSG_CTP_LONG:
                task_stack_init();  //任务堆栈
                latest_task_init(); //最近任务
                func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
                sys_cb.power_on_state=true;
                break;
            case KL_BACK:
                func_switch_to(FUNC_LONG_PRESS, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
                break;
            case MSG_SYS_500MS:
                if(uteApplicationCommonIsHasConnectOurApp())
                {
                    task_stack_init();  //任务堆栈
                    latest_task_init(); //最近任务
                    func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
                    sys_cb.power_on_state=true;
                }
                break;

        }
    }
}

//进入扫一扫功能
static void func_power_on_scan_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_power_on_scan_t));
    func_cb.frm_main = func_power_on_scan_form_create();
}

//退出扫一扫功能
static void func_power_on_scan_exit(void)
{
    func_cb.last = FUNC_POWER_ON_SCAN;
}

//扫一扫功能
void func_power_on_scan(void)
{
    printf("%s\n", __func__);
    func_power_on_scan_enter();
    while (func_cb.sta == FUNC_POWER_ON_SCAN)
    {
        func_power_on_scan_process();
        func_power_on_scan_message(msg_dequeue());
    }
    func_power_on_scan_exit();
}
