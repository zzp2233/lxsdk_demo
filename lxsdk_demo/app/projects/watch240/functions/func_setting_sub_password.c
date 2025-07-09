#include "include.h"
#include "func.h"
#include "ute_module_lockScreen.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_password_t_
{

} f_password_t;

//组件ID
enum
{
    COMPO_ID_BIN_ON = 1,
    COMPO_ID_BIN_OFF,
    COMPO_ID_BIN_CHANGE,
    COMPO_ID_BIN_BG,
    COMPO_ID_BIN_BG1,
};
static int password_switch_on = 0;
//密码设置页面
compo_form_t *func_set_sub_password_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_PASSWORD]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2,GUI_SCREEN_WIDTH,62);
    // compo_setid(cardbox, COMPO_ID_RESET_PUFFS);
    compo_cardbox_rect_set_location(cardbox,0,0,0,232,62,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_setid(cardbox, COMPO_ID_BIN_BG);

    //创建文本
    compo_textbox_t *txt_onoff = compo_textbox_create(frm, strlen(i18n[STR_PASSWORD_SWITCH]));
    compo_textbox_set_align_center(txt_onoff, false);
    compo_textbox_set_location(txt_onoff, 30, 90-17,136,40);
    // compo_textbox_set_pos(txt_onoff, 30, 90-17);
    compo_textbox_set(txt_onoff, i18n[STR_PASSWORD_SWITCH]);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2+62+6,GUI_SCREEN_WIDTH,62);
    compo_cardbox_rect_set_location(cardbox,0,0,0,232,62,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_setid(cardbox, COMPO_ID_BIN_BG1);
    compo_cardbox_set_visible(cardbox, false);

    compo_textbox_t *txt_change = compo_textbox_create(frm, strlen(i18n[STR_CHANGE_PASSWORD]));
    compo_textbox_set_align_center(txt_change, false);
    compo_textbox_set_location(txt_change, 30, 173-30,136,40);
    //  compo_textbox_set_pos(txt_change, 30, 173-30);
    compo_textbox_set(txt_change,i18n[STR_CHANGE_PASSWORD]);
    compo_textbox_set_visible(txt_change, false);

    if(uteModulePasswordData.password_cnt == 4)
    {
        // ecig.clock_flag = 1;
        uteModulePasswordData.password_flag = true;
        compo_textbox_set_visible(txt_change, true);
        compo_cardbox_set_visible(cardbox, true);
        uteModulePasswordDataSaveConfig();
    }

    //创建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_SWITCH00_BIN);
    compo_setid(btn, COMPO_ID_BIN_OFF);
    compo_button_set_pos(btn, 190, 100-17);
    compo_button_set_visible(btn, false);

    if(uteModulePasswordData.password_cnt != 4)
    {
        uteModulePasswordData.password_flag = false;
        uteModulePasswordDataSaveConfig();
        //ecig.clock_flag = 0;
        compo_button_set_visible(btn, true);
    }

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_SWITCH02_BIN);
    compo_setid(btn, COMPO_ID_BIN_ON);
    compo_button_set_pos(btn, 190, 100-17);
    compo_button_set_visible(btn, false);

    if(uteModulePasswordData.password_cnt == 4)
    {
        compo_button_set_visible(btn, true);
    }

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BIN_CHANGE);
    compo_button_set_location(btn, 140, 183-30, 180, 80);
    compo_button_set_visible(btn, false);

    if(uteModulePasswordData.password_cnt == 4)
    {
        compo_button_set_visible(btn, true);
        //compo_form_add_image(frm, UI_BUF_I330001_PUBLIC_SWITCH02_BIN, 190, 183-30);
    }

    return frm;
}

//密码设置功能事件处理
static void func_set_sub_password_process(void)
{
    func_process();
}

//单击按钮
static void func_password_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BIN_OFF:
            password_switch_on = 1; // 打开密码开关
            // c = true;
            //uteModuleSetPasswordLock(true);
            // 新增：调用保存函数
            uteModulePasswordDataSaveConfig();
            func_cb.sta = FUNC_PASSWORD_SUB_DISP;
            printf("11111111111111111111\n");
            break;
        case COMPO_ID_BIN_ON:
            password_switch_on = 0; // 关闭密码开关
            //  uteModulePasswordData.password_flag = false;
            // uteModuleSetPasswordLock(false);
            // 新增：调用保存函数
            uteModulePasswordDataSaveConfig();
            func_cb.sta = FUNC_PASSWORD_SUB_DISP;
            printf("3333333333333333333333\n");
            break;

        case COMPO_ID_BIN_CHANGE:
            uteModulePasswordData.password_change = 1;
            func_cb.sta = FUNC_PASSWORD_SUB_DISP;
            printf("0000000000000000000000000000\n");
            break;

        default:
            break;
    }
}

//密码设置功能消息处理
static void func_set_sub_password_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_password_button_click();
            break;



        default:
            func_message(msg);
            break;
    }
}

//进入密码设置功能
static void func_set_sub_password_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_password_t));
    func_cb.frm_main = func_set_sub_password_form_create();
    if(uteModulePasswordData.password_change)
    {
        uteModulePasswordData.password_change = 0;
    }
}

//退出密码设置功能
static void func_set_sub_password_exit(void)
{
    func_cb.last = FUNC_SET_SUB_PASSWORD;
}

//密码设置功能
void func_set_sub_password(void)
{
    printf("%s\n", __func__);
    func_set_sub_password_enter();
    while (func_cb.sta == FUNC_SET_SUB_PASSWORD)
    {
        func_set_sub_password_process();
        func_set_sub_password_message(msg_dequeue());
    }
    func_set_sub_password_exit();
}
