#include "include.h"
#include "func.h"
#include "ute_module_lockScreen.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//按钮
enum
{
    COMPO_ID_BTN_NO = 1,
    COMPO_ID_BTN_YES,
};

//创建修改密码窗体
compo_form_t *func_password_sub_select_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_PASSWORD]);

    //创建文本
    compo_textbox_t *txt_set = compo_textbox_create(frm, 8);
    compo_textbox_set_align_center(txt_set, false);
    compo_textbox_set_pos(txt_set, 30, 140);
    compo_textbox_set(txt_set,  i18n[STR_SET_PASSWORD]);
    compo_textbox_set_visible(txt_set, true);

    if(uteModulePasswordData.password_change)
    {
        compo_textbox_set_visible(txt_set, false);
    }

    compo_textbox_t *txt_title = compo_textbox_create(frm, 7);
    compo_textbox_set_align_center(txt_title, true);
    compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, 120);
    compo_textbox_set(txt_title, i18n[STR_SUCCES_PASSWORD]);
    compo_textbox_set_visible(txt_title, false);

    if(uteModulePasswordData.password_change)
    {
        compo_textbox_set_visible(txt_title, true);
    }

    //创建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
    compo_setid(btn, COMPO_ID_BTN_NO);
    compo_button_set_pos(btn, 65, 249);
    compo_button_set_visible(btn, false);

    if(!uteModulePasswordData.password_change)
    {
        compo_button_set_visible(btn, true);
    }

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
    compo_setid(btn, COMPO_ID_BTN_YES);
    compo_button_set_pos(btn, 175, 249);

    return frm;
}

//修改密码功能事件处理
static void func_password_sub_select_process(void)
{
    func_process();
}

//按键事件处理
static void func_select_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_YES:
            if(uteModulePasswordData.password_cnt > 0)
            {
                // 确保密码已设置且有效
                uteModulePasswordDataSaveConfig(); // 保存密码配置
                printf("Password saved successfully\n");
            }
            func_cb.sta = FUNC_SET_SUB_PASSWORD;
            break;

        case COMPO_ID_BTN_NO:
            uteModulePasswordData.password_cnt = 0;
            uteModulePasswordDataSaveConfig(); // 保存清除后的状态
            func_cb.sta = FUNC_PASSWORD_SUB_DISP;
            break;

        default:
            break;
    }
}

//修改密码功能消息处理
static void func_password_sub_select_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_select_button_click();                  //单击按钮
            break;


        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入修改密码功能
static void func_password_sub_select_enter(void)
{
    func_cb.frm_main = func_password_sub_select_form_create();
}

//退出修改密码功能
static void func_password_sub_select_exit(void)
{
    func_cb.last = FUNC_PASSWORD_SUB_SELECT;
}

//修改密码功能
void func_password_sub_select(void)
{
    printf("%s\n", __func__);
    func_password_sub_select_enter();
    while (func_cb.sta == FUNC_PASSWORD_SUB_SELECT)
    {
        func_password_sub_select_process();
        func_password_sub_select_message(msg_dequeue());
    }
    func_password_sub_select_exit();
}
