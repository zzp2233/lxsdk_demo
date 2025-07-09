#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

typedef struct f_breathe_finish_t_
{
    char str[30];
} f_breathe_finish_t;

enum
{
    COMPO_ID_BTN_OK=1,
};

compo_form_t *func_breathe_finish_form_create(void)
{
    char txt_buf[50];
    char time_buf[30];
    char time_num[10];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_FINISH]);

    //设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_BREATHE_00_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/3, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/5);

    memset(txt_buf,0,sizeof(txt_buf));
    u8 heart_value = bsp_sensor_hrs_data_get();
    if(heart_value != 0 && heart_value!= 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3,GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/5);
    compo_textbox_set(textbox,txt_buf);

    // printf("breathe->time:%d\n",sys_cb.breathe_duration);
    memset(time_num,0,sizeof(time_num));
    snprintf(time_num,sizeof(time_num),"%d",sys_cb.breathe_duration / 60000);

    memset(time_buf,0,sizeof(time_buf));
    uteModuleCharencodeReplaceSubString(i18n[STR_MIN_JOINT], time_buf,"##",time_num);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s:%s",i18n[STR_BREATHE_TIME],time_buf);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+30, GUI_SCREEN_WIDTH, 30);
    compo_textbox_set(textbox,txt_buf);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);///确定
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_setid(btn,COMPO_ID_BTN_OK);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_textbox_set(textbox,i18n[STR_OK]);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
typedef struct f_breathe_finish_t_
{
    char str[30];
} f_breathe_finish_t;

enum
{
    COMPO_ID_BTN_OK=1,
};

compo_form_t *func_breathe_finish_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

static void func_breathe_finish_process(void)
{
    func_process();
}


static void func_breathe_finish_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:

            if(compo_get_button_id() == COMPO_ID_BTN_OK)
            {
                func_back_to();
            }
            break;
        default:
            func_message(msg);
            break;
    }
}


static void func_breathe_finish_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_breathe_finish_t));
    func_cb.frm_main = func_breathe_finish_form_create();
}


static void func_breathe_finish_exit(void)
{
    func_cb.last = FUNC_BREATHE_FINISH;
}


void func_breathe_finish(void)
{
    printf("%s\n", __func__);
    func_breathe_finish_enter();
    while (func_cb.sta == FUNC_BREATHE_FINISH)
    {
        func_breathe_finish_process();
        func_breathe_finish_message(msg_dequeue());
    }
    func_breathe_finish_exit();
}
#endif