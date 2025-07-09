#include "include.h"
#include "func.h"
#include "func_alipay.h"

#if SECURITY_PAY_EN

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

enum
{
    COMPO_ID_RATE = 1,
    //按钮
    COMPO_ID_BTN_PAYCODE,       //付款码
    COMPO_ID_BTN_SETTING,       //设置
    COMPO_ID_BTN_HELP,          //帮助
    COMPO_ID_BTN_UNBIND,        //解除绑定
    COMPO_ID_BTN_UNBIND_YES,    //解除绑定-确认
    COMPO_ID_BTN_UNBIND_NO,     //解除绑定-取消
    COMPO_ID_BTN_CONFIRM,       //确认
};

enum
{
    ALIPAY_PAGE_LOGO = 1,
    ALIPAY_PAGE_SCAN,
    ALIPAY_PAGE_BINDING,
    ALIPAY_PAGE_BIND_FAIL,
    ALIPAY_PAGE_OPTION_LIST,
    ALIPAY_PAGE_QRCODE,
    ALIPAY_PAGE_BARCODE,
    ALIPAY_PAGE_UNBIND,
    ALIPAY_PAGE_HELP,
    ALIPAY_PAGE_UNBIND_CONFIRM,
    ALIPAY_PAGE_UNBIND_FINISH,
};


typedef struct f_alipay_t_
{
    uint8_t page;
    uint8_t logo_display_cnt;
    uint8_t binding_time_cnt;
    uint8_t bind_fail_time_cnt;
    uint8_t bind_success_time_cnt;
    uint8_t bind_code_refresh_time_cnt;
} f_alipay_t;

extern struct Node* csi_head;

//创建支付宝logo窗体
compo_form_t *func_alipay_form_logo_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_radius(rect, 80);

    //新建图像
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_LOGO2_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    return frm;
}

//创建支付宝窗体
compo_form_t *func_alipay_form_scan_create(void)
{


    int binding_str_len = 256;
    char *binding_str = func_zalloc(binding_str_len);
    memset(binding_str, 0, binding_str_len);
    u8 res = alipay_get_binding_code(binding_str, &binding_str_len);
    printf("res:%d, binding_str: %s\n", res, binding_str);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALIPAY]);

    compo_qrcodebox_t *qrcode = compo_qrcodebox_create(frm, 0, 100);
    compo_qrcodebox_set(qrcode, (char *)binding_str);

    func_free(binding_str);
    return frm;
}

//创建支付宝窗体
compo_form_t *func_alipay_form_binding_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALIPAY]);

    compo_picturebox_t *pic_bg1 = compo_picturebox_create(frm, UI_BUF_ALIPAY_BG1_BIN);
    compo_picturebox_set_pos(pic_bg1, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic_bg1, true);

    compo_arc_t *arc = compo_arc_create(frm);
    compo_setid(arc, COMPO_ID_RATE);
    compo_arc_set_alpha(arc, 0xff, 0);
    compo_arc_set_location(arc, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 180, 180);
    compo_arc_set_width(arc, 7);
    compo_arc_set_rotation(arc, 0);
    compo_arc_set_angles(arc, 0, 3600);
    compo_arc_set_color(arc, COLOR_BLUE, COLOR_WHITE);
    compo_arc_set_value(arc, 250);

    compo_picturebox_t *pic_logo = compo_picturebox_create(frm, UI_BUF_ALIPAY_LOGO_BIN);
    compo_picturebox_set_pos(pic_logo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic_logo, true);

    return frm;
}

//创建支付宝绑定状态窗体
compo_form_t *func_alipay_form_bind_fail_create(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (!f_alipay)
    {
        return NULL;
    }

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建图像
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_BIND_STA_BIN);
    compo_picturebox_cut(pic, 0, 2);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 30);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_BIND_FAILED]);

    return frm;
}

//创建支付宝选项列表窗体
compo_form_t *func_alipay_form_option_list_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建按钮
    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_PAYCODE);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 100);
    btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_SETTING);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 190);
    btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_HELP);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 280);

    //新建图像
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 0, 3);
    compo_picturebox_set_pos(pic, 40, 100);
    pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 1, 3);
    compo_picturebox_set_pos(pic, 40, 190);
    pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 2, 3);
    compo_picturebox_set_pos(pic, 40, 280);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 100 - widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_PAYCODE]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 190 - widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_SETTING]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 280 - widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_HELP]);

    return frm;
}

//创建支付宝二维码窗体
compo_form_t *func_alipay_form_qrcode_create(void)
{

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_radius(rect, 80);

    //页码示意条
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, COLOR_WHITE);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X - 8, 15, 10, 10);
    widget_rect_set_radius(rect->rect, 5);
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, COLOR_GRAY);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X + 8, 15, 10, 10);
    widget_rect_set_radius(rect->rect, 5);

    u32 paycode_len = 20;
    u8 *paycode = func_zalloc(paycode_len);
    memset(paycode, 0, paycode_len);
    u8 res = alipay_get_paycode(paycode, &paycode_len);
    u8 retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_paycode(paycode, &paycode_len);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_paycode err\n");
            break;
        }
    }
    printf("res:%d, paycode: %s\n", res, paycode);

    //新建二维码
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, 64);
    compo_qrcodebox_set(qrbox, (char *)paycode);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 180);

    //新建图标
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_LOGO_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_size(pic, 45, 45);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_PAYCODE]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_SWITCH_BARCODE]);

    func_free(paycode);
    return frm;
}

//创建支付宝条形码窗体
compo_form_t *func_alipay_form_barcode_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_radius(rect, 80);

    //页码示意条
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, COLOR_GRAY);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X - 8, 15, 10, 10);
    widget_rect_set_radius(rect->rect, 5);
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, COLOR_WHITE);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X + 8, 15, 10, 10);
    widget_rect_set_radius(rect->rect, 5);

    u32 paycode_len = 20;
    u8 *paycode = func_zalloc(paycode_len);
    memset(paycode, 0, paycode_len);
    u8 res = alipay_get_paycode(paycode, &paycode_len);
    u8 retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_paycode(paycode, &paycode_len);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_paycode err\n");
            break;
        }
    }
    printf("res:%d, paycode: %s\n", res, paycode);

    //新建二维码
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_HORIZONTAL, 64);
    compo_qrcodebox_set(qrbox, (char *)paycode);
    compo_qrcodebox_set_bitwid(qrbox, 10);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 200);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_PAYCODE]);
    txt = compo_textbox_create(frm, 32);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 80, 300, widget_text_get_height());
    compo_textbox_set(txt, (char *)paycode);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_SWITCH_QRCODE]);

    func_free(paycode);
    return frm;
}

//创建支付宝解绑（设置）窗体
compo_form_t *func_alipay_form_unbind_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建按钮蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 150, 60);
    widget_rect_set_radius(rect->rect, 30);

    //新建按钮
    compo_button_t *btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_UNBIND);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 150, 60);

    u8 res;
    u32 id_len = 128;
    u32 name_len = 128;
    u8 *id = func_zalloc(id_len);
    u8 *name = func_zalloc(name_len);
    memset(id, 0, id_len);
    memset(name, 0, name_len);
    res = alipay_get_logon_ID(id, &id_len);
    u8 retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_logon_ID(id, &id_len);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_logon_ID err\n");
            break;
        }
    }
    printf("alipay_get_logon_ID: %d: %s\n", res, id);
    res = alipay_get_nick_name(name, &name_len);
    retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_nick_name(name, &name_len);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_nick_name err\n");
            break;
        }
    }
    printf("alipay_get_logon_name: %d: %s\n", res, name);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_SETTING]);
    txt = compo_textbox_create(frm, 32);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 20, 300, widget_text_get_height());
    compo_textbox_set(txt, (char *)id);
    txt = compo_textbox_create(frm, 32);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20, 300, widget_text_get_height());
    compo_textbox_set_forecolor(txt, COLOR_GRAY);
    compo_textbox_set(txt, (char *)name);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 130, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_UNBIND]);

    func_free(id);
    func_free(name);

    return frm;
}

//创建支付宝二维码窗体
compo_form_t *func_alipay_form_help_create(void)
{

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_radius(rect, 80);

    u32 len_aid_code = 256;
    char *aid_code = func_zalloc(len_aid_code);
    memset(aid_code, 0, len_aid_code);
    u8 res = alipay_get_aid_code(aid_code, &len_aid_code, true);
    u8 retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_aid_code(aid_code, &len_aid_code, true);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_paycode err\n");
            break;
        }
    }
    printf("res:%d, aid_code: %s, len_aid_code: %d\n", res, aid_code, len_aid_code);

    //新建二维码
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, 200);
    compo_qrcodebox_set(qrbox, (char *)aid_code);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 200);

    //新建图标
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_LOGO_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_size(pic, 45, 45);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 45, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_HELP_TILE]);

    func_free(aid_code);
    return frm;
}


//创建支付宝确认解绑窗体
compo_form_t *func_alipay_form_unbind_confirm_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建按钮背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); //白
    compo_shape_set_color(rect, COLOR_WHITE);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X - 60, GUI_SCREEN_HEIGHT - 60, 100, 60);
    widget_rect_set_radius(rect->rect, 30);
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); //蓝
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X + 60, GUI_SCREEN_HEIGHT - 60, 100, 60);
    widget_rect_set_radius(rect->rect, 30);

    //新建按钮
    compo_button_t *btn = compo_button_create(frm);  //确认
    compo_setid(btn, COMPO_ID_BTN_UNBIND_YES);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X - 60, GUI_SCREEN_HEIGHT - 60, 100, 60);
    btn = compo_button_create(frm);  //取消
    compo_setid(btn, COMPO_ID_BTN_UNBIND_NO);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X + 60, GUI_SCREEN_HEIGHT - 60, 100, 60);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_UNBIND]);
    txt = compo_textbox_create(frm, 64);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 300, 150);
    compo_textbox_set_multiline(txt, true);
    compo_textbox_set(txt, i18n[STR_ALIPAY_UNBIND_TIP]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 60, GUI_SCREEN_HEIGHT - 60, 80, widget_text_get_height());
    compo_textbox_set_forecolor(txt, COLOR_BLACK);
    compo_textbox_set(txt, i18n[STR_CONFIMR]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 60, GUI_SCREEN_HEIGHT - 60, 80, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_CANCEL]);

    return frm;
}

//创建支付宝解绑完成窗体
compo_form_t *func_alipay_form_unbind_finish_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建按钮蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 150, 60);
    widget_rect_set_radius(rect->rect, 30);

    //新建按钮
    compo_button_t *btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_CONFIRM);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 150, 60);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 64);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 300, 150);
    compo_textbox_set_multiline(txt, true);
    compo_textbox_set(txt, i18n[STR_ALIPAY_UNBIND_TIP2]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 130, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_CONFIMR]);

    return frm;
}
extern u32 __alipay_start, __alipay_size;
void csi_exit_lpm(void *ctx);
AT(.text.func.alipay)
void func_alipay_prepare(void)
{
    printf("__alipay_start: 0x%x, size: 0x%x\n", &__alipay_start, &__alipay_size);
    memset(&__alipay_start, 0, (u32)&__alipay_size);
#if (SECURITY_PAY_VENDOR == SECURITY_VENDOR_HS)
    HS_IIC_Init();
#endif
#if (SECURITY_PAY_VENDOR == SECURITY_VENDOR_HED)
    uint8_t *alipay_com_outbuf = func_zalloc(200);
    uint32_t alipay_outlen =0;
    hed_rst_init();
    csi_exit_lpm(0);
    HED_IIC_Init();
    api_connect(alipay_com_outbuf, &alipay_outlen);
    printf("api_connect atr:\n");
    print_r(alipay_com_outbuf, alipay_outlen);
    func_free(alipay_com_outbuf);
#endif
    u8 sta = alipay_pre_init();
    printf("alipay_pre_init: %d\n", sta);
    if (sta != ALIPAY_RV_OK)
    {
        func_back_to();
    }

    sta = alipay_get_binding_status();
    printf("alipay_get_binding_status: %d\n", sta);
    if (sta == true)
    {
        func_alipay_form_create_by_page(ALIPAY_PAGE_OPTION_LIST);
    }
    else
    {
        func_alipay_form_create_by_page(ALIPAY_PAGE_SCAN);
    }

    printf("func_alipay_prepare finish\n");
}

void func_alipay_init(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    f_alipay->page = ALIPAY_PAGE_LOGO;
}

//支付宝功能事件处理
static void func_alipay_process(void)
{
    func_process();
}

//创建支付宝窗体（默认为logo页面）
compo_form_t *func_alipay_form_create(void)
{
    return func_alipay_form_logo_create();
}


//创建指定页面的窗体（销毁&创建）
void func_alipay_form_create_by_page(u8 page_sta)
{
    printf("%s: %d\n", __func__, page_sta);
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    compo_form_t *frm = func_cb.frm_main;

    if (frm != NULL)
    {
        compo_form_destroy(frm);
        frm = NULL;
    }

    switch (page_sta)
    {
        case ALIPAY_PAGE_LOGO:
            frm = func_alipay_form_logo_create();
            break;

        case ALIPAY_PAGE_SCAN:
            frm = func_alipay_form_scan_create();
            break;

        case ALIPAY_PAGE_BINDING:
            frm = func_alipay_form_binding_create();
            break;

        case ALIPAY_PAGE_BIND_FAIL:
            frm = func_alipay_form_bind_fail_create();
            break;

        case ALIPAY_PAGE_OPTION_LIST:
            frm = func_alipay_form_option_list_create();
            break;

        case ALIPAY_PAGE_QRCODE:
            frm = func_alipay_form_qrcode_create();
            break;

        case ALIPAY_PAGE_BARCODE:
            frm = func_alipay_form_barcode_create();
            break;

        case ALIPAY_PAGE_UNBIND:
            frm = func_alipay_form_unbind_create();
            break;

        case ALIPAY_PAGE_HELP:
            frm = func_alipay_form_help_create();
            break;

        case ALIPAY_PAGE_UNBIND_CONFIRM:
            frm = func_alipay_form_unbind_confirm_create();
            break;

        case ALIPAY_PAGE_UNBIND_FINISH:
            frm = func_alipay_form_unbind_finish_create();
            break;
    }

    if (frm)
    {
        f_alipay->page = page_sta;
        printf(">>>page_sta[%d]\n", page_sta);
    }
}


//检测绑定状态改变之后跳转界面
void alipay_binding_detect(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (f_alipay->page == ALIPAY_PAGE_LOGO)
    {
        f_alipay->logo_display_cnt++;
        printf("logo_display_cnt: %d\n", f_alipay->logo_display_cnt);
        if (f_alipay->logo_display_cnt == ALIPAY_LOGO_TIMEOUT)
        {
            f_alipay->logo_display_cnt = 0;
            func_alipay_prepare();
        }

    }
    else if (f_alipay->page == ALIPAY_PAGE_SCAN || f_alipay->page == ALIPAY_PAGE_BINDING)
    {
        u32 ret;
        binding_status_e sta = ALIPAY_STATUS_UNKNOWN;
        ret = alipay_query_binding_result(&sta);
        printf("ret: %d, sta: %d, page: %d\n", ret, sta, f_alipay->page);

        if (sta == ALIPAY_STATUS_START_BINDING && f_alipay->page == ALIPAY_PAGE_SCAN)
        {
            func_alipay_form_create_by_page(ALIPAY_PAGE_BINDING);
        }
        compo_arc_t *arc = compo_getobj_byid(COMPO_ID_RATE);
        if (f_alipay->page == ALIPAY_PAGE_BINDING)
        {

            switch (sta)
            {
                case ALIPAY_STATUS_BINDING_OK:
                    compo_arc_set_value(arc, 1000);
                    break;
                case ALIPAY_STATUS_BINDING_FAIL:
                    break;
                default :
                    break;
            }
        }
        //binding success go to option
        if (sta == ALIPAY_STATUS_BINDING_OK && f_alipay->page == ALIPAY_PAGE_BINDING)
        {
            f_alipay->bind_success_time_cnt++;
            printf("bind_success_time_cnt: %d\n", f_alipay->bind_success_time_cnt);
            if (f_alipay->bind_success_time_cnt == ALIPAY_SUCCESS_TIMEOUT)
            {
                f_alipay->bind_success_time_cnt = 0;
                func_alipay_form_create_by_page(ALIPAY_PAGE_OPTION_LIST);
            }
        }

        //binding timeout go to logo
        if (sta != ALIPAY_STATUS_BINDING_OK && f_alipay->page == ALIPAY_PAGE_BINDING)
        {
            f_alipay->binding_time_cnt++;
            if (f_alipay->binding_time_cnt == ALIPAY_BIND_TIMEOUT)
            {
                f_alipay->binding_time_cnt = 0;
                func_alipay_form_create_by_page(ALIPAY_PAGE_BIND_FAIL);
            }
        }

    }

    //bind failed go to logo
    else if (f_alipay->page == ALIPAY_PAGE_BIND_FAIL)
    {
        f_alipay->bind_fail_time_cnt++;
        printf("bind_fail_time_cnt: %d\n", f_alipay->bind_fail_time_cnt);
        if (f_alipay->bind_fail_time_cnt == ALIPAY_FAIL_TIMEOUT)
        {
            f_alipay->bind_fail_time_cnt = 0;
//                alipay_bind_env_destroy();
            printf("alipay_bind_env_destroy\n");
            printf("%s\n", __func__);
            freeList(&csi_head);
            func_alipay_prepare();
        }
    }

    //刷新支付码 ALIPAY_CODE_REFRESH_TIMEOUT 秒 刷新一次
    else if (f_alipay->page == ALIPAY_PAGE_QRCODE || f_alipay->page == ALIPAY_PAGE_BARCODE)
    {
        f_alipay->bind_code_refresh_time_cnt++;
        //printf("bind_code_refresh_time_cnt: %d\n", f_alipay->bind_code_refresh_time_cnt);

        if (f_alipay->bind_code_refresh_time_cnt >= ALIPAY_CODE_REFRESH_TIMEOUT)
        {
            printf("===>> qr code refresh\n");
            f_alipay->bind_code_refresh_time_cnt = 0;
            func_alipay_form_create_by_page(f_alipay->page);
        }
    }

}


//支付宝按钮点击处理
static void func_alipay_click_handler(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    u8 id = compo_get_button_id();
    if (id)
    {
        printf(">>>button_id:%d\n", id);
        switch (f_alipay->page)
        {
            case ALIPAY_PAGE_OPTION_LIST:
                if (id == COMPO_ID_BTN_PAYCODE)
                {
                    func_alipay_form_create_by_page(ALIPAY_PAGE_QRCODE);
                }
                else if(id == COMPO_ID_BTN_SETTING)
                {
                    func_alipay_form_create_by_page(ALIPAY_PAGE_UNBIND);
                }
                else if(id == COMPO_ID_BTN_HELP)
                {
                    func_alipay_form_create_by_page(ALIPAY_PAGE_HELP);
                }
                break;

            case ALIPAY_PAGE_UNBIND:
                func_alipay_form_create_by_page(ALIPAY_PAGE_UNBIND_CONFIRM);
                break;

            case ALIPAY_PAGE_UNBIND_CONFIRM:
                if (id == COMPO_ID_BTN_UNBIND_YES)
                {
                    if (alipay_unbinding() == ALIPAY_RV_OK)
                    {
                        freeList(&csi_head);
                        printf("alipay_unbinding success!\n");
                    }
                    else
                    {
                        printf("alipay_unbinding fail!\n");
                    }
                    func_alipay_form_create_by_page(ALIPAY_PAGE_UNBIND_FINISH);
                }
                else
                {
                    func_alipay_form_create_by_page(ALIPAY_PAGE_UNBIND);
                }
                break;

            case ALIPAY_PAGE_UNBIND_FINISH:
                func_alipay_prepare();
                break;
        }
    }
}

//支付宝功能消息处理
static void func_alipay_touch_move(size_msg_t msg)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (msg == MSG_CTP_SHORT_RIGHT && f_alipay->page == ALIPAY_PAGE_BARCODE)
    {
        func_alipay_form_create_by_page(ALIPAY_PAGE_QRCODE);
    }

    if (msg == MSG_CTP_SHORT_LEFT && f_alipay->page == ALIPAY_PAGE_QRCODE)
    {
        func_alipay_form_create_by_page(ALIPAY_PAGE_BARCODE);
    }
    if (f_alipay->page == ALIPAY_PAGE_SCAN || f_alipay->page == ALIPAY_PAGE_OPTION_LIST)
    {
        func_message(msg);
    }
}

//支付宝功能消息处理
static void func_alipay_touch_key(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (f_alipay->page == ALIPAY_PAGE_SCAN || f_alipay->page == ALIPAY_PAGE_OPTION_LIST)
    {
        func_back_to();
    }

    switch (f_alipay->page)
    {
        case ALIPAY_PAGE_QRCODE:
        case ALIPAY_PAGE_BARCODE:
        case ALIPAY_PAGE_UNBIND:
        case ALIPAY_PAGE_UNBIND_CONFIRM:
        case ALIPAY_PAGE_HELP:
            func_alipay_form_create_by_page(ALIPAY_PAGE_OPTION_LIST);
            break;
        default:
            break;
    }
}


//支付宝功能消息处理
static void func_alipay_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_alipay_click_handler();
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;
        case KU_BACK:
            func_alipay_touch_key();
            break;

        case MSG_SYS_1S:
            alipay_binding_detect();
            break;


        case MSG_CTP_SHORT_RIGHT:
            func_alipay_touch_move(MSG_CTP_SHORT_RIGHT);
            break;
        case MSG_CTP_SHORT_LEFT:
            func_alipay_touch_move(MSG_CTP_SHORT_LEFT);
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入支付宝功能
static void func_alipay_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alipay_t));
    func_cb.frm_main = func_alipay_form_create();
    func_alipay_init();
}


//退出支付宝功能
static void func_alipay_exit(void)
{
    printf("%s\n", __func__);
    freeList(&csi_head);
    func_cb.last = FUNC_ALIPAY;
}

//支付宝功能
void func_alipay(void)
{
    printf("%s\n", __func__);
    func_alipay_enter();
    while (func_cb.sta == FUNC_ALIPAY)
    {
        func_alipay_process();
        func_alipay_message(msg_dequeue());
    }
    func_alipay_exit();
}
#endif // SECURITY_PAY_EN

