#include "include.h"
#include "func.h"
#include "func_cover.h"

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{

    COMPO_ID_BTN_NUM0 = 1,
    COMPO_ID_BTN_NUM1,
    COMPO_ID_BTN_NUM2,
    COMPO_ID_BTN_NUM3,
    COMPO_ID_BTN_NUM4,
    COMPO_ID_BTN_NUM5,
    COMPO_ID_BTN_NUM6,
    COMPO_ID_BTN_NUM7,
    COMPO_ID_BTN_NUM8,
    COMPO_ID_BTN_NUM9,
    COMPO_ID_BTN_CALL,
    COMPO_ID_BTN_DEL,

    COMPO_ID_PIC_CLICK_NUM0,
    COMPO_ID_PIC_CLICK_NUM1,
    COMPO_ID_PIC_CLICK_NUM2,
    COMPO_ID_PIC_CLICK_NUM3,
    COMPO_ID_PIC_CLICK_NUM4,
    COMPO_ID_PIC_CLICK_NUM5,
    COMPO_ID_PIC_CLICK_NUM6,
    COMPO_ID_PIC_CLICK_NUM7,
    COMPO_ID_PIC_CLICK_NUM8,
    COMPO_ID_PIC_CLICK_NUM9,
    COMPO_ID_PIC_CLICK_CALL,
    COMPO_ID_PIC_CLICK_DEL,


    COMPO_ID_NUM_DISP_ZERO,
//    COMPO_ID_PIC_NUM,


};

#define CALL_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_call_disp_btn_item) / sizeof(tbl_call_disp_btn_item[0])))

typedef struct password_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    u32 res_click;
    u16 btn_click_id;
    s16 x;
    s16 y;
} call_disp_btn_item_t;

//按钮item，创建时遍历一下
static const call_disp_btn_item_t tbl_call_disp_btn_item[] =
{
    {UI_BUF_I330001_CALL_DIAL_GREY_01_BIN,             COMPO_ID_BTN_NUM1,       UI_BUF_I330001_CALL_DIAL_BLUE_01_BIN,  COMPO_ID_PIC_CLICK_NUM1,     45,     84},
    {UI_BUF_I330001_CALL_DIAL_GREY_02_BIN,             COMPO_ID_BTN_NUM2,       UI_BUF_I330001_CALL_DIAL_BLUE_02_BIN,  COMPO_ID_PIC_CLICK_NUM2,     120,    84},
    {UI_BUF_I330001_CALL_DIAL_GREY_03_BIN,             COMPO_ID_BTN_NUM3,       UI_BUF_I330001_CALL_DIAL_BLUE_03_BIN,  COMPO_ID_PIC_CLICK_NUM3,     195,    84},
    {UI_BUF_I330001_CALL_DIAL_GREY_04_BIN,             COMPO_ID_BTN_NUM4,       UI_BUF_I330001_CALL_DIAL_BLUE_04_BIN,  COMPO_ID_PIC_CLICK_NUM4,     45,     140},
    {UI_BUF_I330001_CALL_DIAL_GREY_05_BIN,             COMPO_ID_BTN_NUM5,       UI_BUF_I330001_CALL_DIAL_BLUE_05_BIN,  COMPO_ID_PIC_CLICK_NUM5,     120,    140},
    {UI_BUF_I330001_CALL_DIAL_GREY_06_BIN,             COMPO_ID_BTN_NUM6,       UI_BUF_I330001_CALL_DIAL_BLUE_06_BIN,  COMPO_ID_PIC_CLICK_NUM6,     195,    140},
    {UI_BUF_I330001_CALL_DIAL_GREY_07_BIN,             COMPO_ID_BTN_NUM7,       UI_BUF_I330001_CALL_DIAL_BLUE_07_BIN,  COMPO_ID_PIC_CLICK_NUM7,     45,     196},
    {UI_BUF_I330001_CALL_DIAL_GREY_08_BIN,             COMPO_ID_BTN_NUM8,       UI_BUF_I330001_CALL_DIAL_BLUE_08_BIN,  COMPO_ID_PIC_CLICK_NUM8,     120,    196},
    {UI_BUF_I330001_CALL_DIAL_GREY_09_BIN,             COMPO_ID_BTN_NUM9,       UI_BUF_I330001_CALL_DIAL_BLUE_09_BIN,  COMPO_ID_PIC_CLICK_NUM9,     195,    196},
    {UI_BUF_I330001_CALL_DIAL_GREY_00_BIN,             COMPO_ID_BTN_NUM0,       UI_BUF_I330001_CALL_DIAL_BLUE_00_BIN,  COMPO_ID_PIC_CLICK_NUM0,     120,    252},
    {UI_BUF_I330001_CALL_DIAL_DELETE_BIN,              COMPO_ID_BTN_DEL,        0,                                     COMPO_ID_PIC_CLICK_DEL,      195,    252},
    {UI_BUF_I330001_CALL_DIAL_CALL_BIN,                COMPO_ID_BTN_CALL,       0,                                     COMPO_ID_PIC_CLICK_CALL,     45,     252},
};

typedef struct f_call_sub_dial_t_
{
    u8 phone_number_cnt;
    u8 phone_number[16];
} f_call_sub_dial_t;

//打电话页面
compo_form_t *func_call_sub_dial_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < CALL_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_call_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn, tbl_call_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_call_disp_btn_item[idx_btn].x, tbl_call_disp_btn_item[idx_btn].y);
    }
//    printf("W:%d H:%d\n",gui_image_get_size(UI_BUF_COMMON_1_CLICK_BIN).wid,gui_image_get_size(UI_BUF_COMMON_1_CLICK_BIN).hei);

    //创建点击按钮变色图片
    for (u8 i=0; i<CALL_DISP_BTN_ITEM_CNT; i++)
    {
        compo_picturebox_t* pic = compo_picturebox_create(frm, tbl_call_disp_btn_item[i].res_click);
        compo_setid(pic, tbl_call_disp_btn_item[i].btn_click_id);
        compo_picturebox_set_pos(pic, tbl_call_disp_btn_item[i].x, tbl_call_disp_btn_item[i].y);
        compo_picturebox_set_visible(pic, false);
    }

    //创建文本
//    compo_textbox_t *txt_title = compo_textbox_create(frm, strlen(i18n[STR_ENTER_NUMBER]));
//    compo_setid(txt_title, COMPO_ID_PIC_NUM);
//    //compo_textbox_set_align_center(txt_title, false);
//    compo_textbox_set_pos(txt_title, 10, 18);
//    compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/6, GUI_SCREEN_WIDTH, widget_text_get_height());
//    //compo_textbox_set_autoroll_mode(txt_title,1);
//    compo_textbox_set(txt_title, i18n[STR_ENTER_NUMBER]);

    //创建数字
    compo_textbox_t *txt_num;
    txt_num = compo_textbox_create(frm, 16);
    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_setid(txt_num, COMPO_ID_NUM_DISP_ZERO);
    compo_textbox_set_align_center(txt_num, true);
    compo_textbox_set_location(txt_num, GUI_SCREEN_CENTER_X, 30, GUI_SCREEN_WIDTH/1.2, 40);
//    compo_textbox_set_pos(txt_num, , 30);
    compo_textbox_set(txt_num, "");

    return frm;
}

///触摸按钮效果处理
static void func_call_sub_dial_button_touch_handle(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_NUM0...COMPO_ID_BTN_DEL:
//        printf("id=%d\n", id);
            compo_picturebox_set_visible(compo_getobj_byid(id+COMPO_ID_BTN_DEL), true);
            break;
        default:
            break;
    }
}

///释放按钮效果处理
static void func_call_sub_dial_button_release_handle(void)
{
    for (u8 i=0; i<CALL_DISP_BTN_ITEM_CNT; i++)
    {
        compo_picturebox_t* pic = compo_getobj_byid(tbl_call_disp_btn_item[i].btn_click_id);
        compo_picturebox_set_visible(pic, false);       //触摸效果图设置不可见
    }
}

///单击
static void func_call_sub_dial_button_click(void)
{
    int id = compo_get_button_id();
    //获取文本组件的地址
//    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_PIC_NUM);

    //获取数字组件地址
    char buf[34];
    compo_textbox_t *txt_num = compo_getobj_byid(COMPO_ID_NUM_DISP_ZERO);

    f_call_sub_dial_t *call = (f_call_sub_dial_t *)func_cb.f_cb;

    switch (id)
    {
        case COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM9:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt <= 16)
            {
                call->phone_number[call->phone_number_cnt++] = id+0x30 - 1;
            }
            break;

        case COMPO_ID_BTN_CALL:
            bt_pbap_lookup_number((char*)call->phone_number);
            memset(sys_cb.outgoing_number, 0, 16);
            for(int i=0; i<call->phone_number_cnt; i++)
            {
                sys_cb.outgoing_number[i] = call->phone_number[i];
            }
            if(!uteModuleCallBtIsConnected())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
                msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
            }
#if MODEM_CAT1_EN
            if (bsp_modem_get_init_flag())
            {
                modem_call_dial(sys_cb.outgoing_number);
            }
            else
#endif
            {
                bt_call_redial_number();
            }
//            printf("name:%s\n",sys_cb.pbap_result_Name);
            break;

        case COMPO_ID_BTN_DEL:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt > 0)
            {
                call->phone_number[--call->phone_number_cnt] = '\0';
            }
            break;

        default:
            break;
    }

    memset(buf, 0, sizeof(buf));
    if(call->phone_number_cnt)
    {
        snprintf(buf, sizeof(buf), "%s", call->phone_number);
    }
    compo_textbox_set(txt_num, buf);
    func_call_sub_dial_button_release_handle();
}

///长按处理
static void func_call_sub_dial_button_long(void)
{
    int id = compo_get_button_id();
    //获取文本组件的地址
//    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_PIC_NUM);

    //获取数字组件地址
    char buf[34];
    compo_textbox_t *txt_num = compo_getobj_byid(COMPO_ID_NUM_DISP_ZERO);

    f_call_sub_dial_t *call = (f_call_sub_dial_t *)func_cb.f_cb;

    switch (id)
    {
        case COMPO_ID_BTN_NUM7:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt < 16)
            {
                call->phone_number[call->phone_number_cnt++] = '*';
            }
            break;
        case COMPO_ID_BTN_NUM9:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt < 16)
            {
                call->phone_number[call->phone_number_cnt++] = '#';
            }
            break;
        case COMPO_ID_BTN_NUM0:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt < 16)
            {
                call->phone_number[call->phone_number_cnt++] = '+';
            }
            break;
    }

    memset(buf, 0, sizeof(buf));
    if(call->phone_number_cnt)
    {
        snprintf(buf, sizeof(buf), "%s", call->phone_number);
    }
    compo_textbox_set(txt_num, buf);
    func_call_sub_dial_button_release_handle();

}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

enum
{

    COMPO_ID_BTN_NUM0 = 1,
    COMPO_ID_BTN_NUM1,
    COMPO_ID_BTN_NUM2,
    COMPO_ID_BTN_NUM3,
    COMPO_ID_BTN_NUM4,
    COMPO_ID_BTN_NUM5,
    COMPO_ID_BTN_NUM6,
    COMPO_ID_BTN_NUM7,
    COMPO_ID_BTN_NUM8,
    COMPO_ID_BTN_NUM9,
    COMPO_ID_BTN_CALL,
    COMPO_ID_BTN_DEL,

    COMPO_ID_PIC_CLICK_NUM0,
    COMPO_ID_PIC_CLICK_NUM1,
    COMPO_ID_PIC_CLICK_NUM2,
    COMPO_ID_PIC_CLICK_NUM3,
    COMPO_ID_PIC_CLICK_NUM4,
    COMPO_ID_PIC_CLICK_NUM5,
    COMPO_ID_PIC_CLICK_NUM6,
    COMPO_ID_PIC_CLICK_NUM7,
    COMPO_ID_PIC_CLICK_NUM8,
    COMPO_ID_PIC_CLICK_NUM9,
    COMPO_ID_PIC_CLICK_CALL,
    COMPO_ID_PIC_CLICK_DEL,


    COMPO_ID_NUM_DISP_ZERO,
//    COMPO_ID_PIC_NUM,


};

#define CALL_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_call_disp_btn_item) / sizeof(tbl_call_disp_btn_item[0])))

typedef struct password_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    u32 res_click;
    u16 btn_click_id;
    s16 x;
    s16 y;
} call_disp_btn_item_t;

//按钮item，创建时遍历一下
static const call_disp_btn_item_t tbl_call_disp_btn_item[] =
{
    {UI_BUF_I332001_CALL_DIAL_BLACK_01_BIN,             COMPO_ID_BTN_NUM1,       UI_BUF_I332001_CALL_DIAL_BLUE_01_BIN,  COMPO_ID_PIC_CLICK_NUM1,     180-92,     183-62},
    {UI_BUF_I332001_CALL_DIAL_BLACK_02_BIN,             COMPO_ID_BTN_NUM2,       UI_BUF_I332001_CALL_DIAL_BLUE_02_BIN,  COMPO_ID_PIC_CLICK_NUM2,     180,    183-62},
    {UI_BUF_I332001_CALL_DIAL_BLACK_03_BIN,             COMPO_ID_BTN_NUM3,       UI_BUF_I332001_CALL_DIAL_BLUE_03_BIN,  COMPO_ID_PIC_CLICK_NUM3,     180+92,    183-62},
    {UI_BUF_I332001_CALL_DIAL_BLACK_04_BIN,             COMPO_ID_BTN_NUM4,       UI_BUF_I332001_CALL_DIAL_BLUE_04_BIN,  COMPO_ID_PIC_CLICK_NUM4,     180-92,     183},
    {UI_BUF_I332001_CALL_DIAL_BLACK_05_BIN,             COMPO_ID_BTN_NUM5,       UI_BUF_I332001_CALL_DIAL_BLUE_05_BIN,  COMPO_ID_PIC_CLICK_NUM5,     180,    183},
    {UI_BUF_I332001_CALL_DIAL_BLACK_06_BIN,             COMPO_ID_BTN_NUM6,       UI_BUF_I332001_CALL_DIAL_BLUE_06_BIN,  COMPO_ID_PIC_CLICK_NUM6,     180+92,    183},
    {UI_BUF_I332001_CALL_DIAL_BLACK_07_BIN,             COMPO_ID_BTN_NUM7,       UI_BUF_I332001_CALL_DIAL_BLUE_07_BIN,  COMPO_ID_PIC_CLICK_NUM7,     180-92,     183+62},
    {UI_BUF_I332001_CALL_DIAL_BLACK_08_BIN,             COMPO_ID_BTN_NUM8,       UI_BUF_I332001_CALL_DIAL_BLUE_08_BIN,  COMPO_ID_PIC_CLICK_NUM8,     180,    183+62},
    {UI_BUF_I332001_CALL_DIAL_BLACK_09_BIN,             COMPO_ID_BTN_NUM9,       UI_BUF_I332001_CALL_DIAL_BLUE_09_BIN,  COMPO_ID_PIC_CLICK_NUM9,     180+92,    183+62},
    {UI_BUF_I332001_CALL_DIAL_BLACK_00_BIN,             COMPO_ID_BTN_NUM0,       UI_BUF_I332001_CALL_DIAL_BLUE_00_BIN,  COMPO_ID_PIC_CLICK_NUM0,     180,    183+124},
    {UI_BUF_I332001_CALL_DIAL_DELETE_BIN,               COMPO_ID_BTN_DEL,        0,                                     COMPO_ID_PIC_CLICK_DEL,      180+92,    183+124},
    {UI_BUF_I332001_CALL_DIAL_CALL_BIN,                 COMPO_ID_BTN_CALL,       0,                                     COMPO_ID_PIC_CLICK_CALL,     180-92,     183+124},
};

typedef struct f_call_sub_dial_t_
{
    u8 phone_number_cnt;
    u8 phone_number[16];
} f_call_sub_dial_t;

//打电话页面
compo_form_t *func_call_sub_dial_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < CALL_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_call_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn, tbl_call_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_call_disp_btn_item[idx_btn].x, tbl_call_disp_btn_item[idx_btn].y);
    }
//    printf("W:%d H:%d\n",gui_image_get_size(UI_BUF_COMMON_1_CLICK_BIN).wid,gui_image_get_size(UI_BUF_COMMON_1_CLICK_BIN).hei);

    //创建点击按钮变色图片
    for (u8 i=0; i<CALL_DISP_BTN_ITEM_CNT; i++)
    {
        compo_picturebox_t* pic = compo_picturebox_create(frm, tbl_call_disp_btn_item[i].res_click);
        compo_setid(pic, tbl_call_disp_btn_item[i].btn_click_id);
        compo_picturebox_set_pos(pic, tbl_call_disp_btn_item[i].x, tbl_call_disp_btn_item[i].y);
        compo_picturebox_set_visible(pic, false);
    }

    //创建文本
//    compo_textbox_t *txt_title = compo_textbox_create(frm, strlen(i18n[STR_ENTER_NUMBER]));
//    compo_setid(txt_title, COMPO_ID_PIC_NUM);
//    //compo_textbox_set_align_center(txt_title, false);
//    compo_textbox_set_pos(txt_title, 10, 18);
//    compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/6, GUI_SCREEN_WIDTH, widget_text_get_height());
//    //compo_textbox_set_autoroll_mode(txt_title,1);
//    compo_textbox_set(txt_title, i18n[STR_ENTER_NUMBER]);

    //创建数字
    compo_textbox_t *txt_num;
    txt_num = compo_textbox_create(frm, 16);
    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_setid(txt_num, COMPO_ID_NUM_DISP_ZERO);
    compo_textbox_set_align_center(txt_num, true);
    compo_textbox_set_location(txt_num, GUI_SCREEN_CENTER_X, 22+36, GUI_SCREEN_WIDTH/1.2, 40);
//    compo_textbox_set_pos(txt_num, , 30);
    compo_textbox_set(txt_num, "");

    return frm;
}

///触摸按钮效果处理
static void func_call_sub_dial_button_touch_handle(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_NUM0...COMPO_ID_BTN_DEL:
//        printf("id=%d\n", id);
            compo_picturebox_set_visible(compo_getobj_byid(id+COMPO_ID_BTN_DEL), true);
            break;
        default:
            break;
    }
}

///释放按钮效果处理
static void func_call_sub_dial_button_release_handle(void)
{
    for (u8 i=0; i<CALL_DISP_BTN_ITEM_CNT; i++)
    {
        compo_picturebox_t* pic = compo_getobj_byid(tbl_call_disp_btn_item[i].btn_click_id);
        compo_picturebox_set_visible(pic, false);       //触摸效果图设置不可见
    }
}

///单击
static void func_call_sub_dial_button_click(void)
{
    int id = compo_get_button_id();
    //获取文本组件的地址
//    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_PIC_NUM);

    //获取数字组件地址
    char buf[34];
    compo_textbox_t *txt_num = compo_getobj_byid(COMPO_ID_NUM_DISP_ZERO);

    f_call_sub_dial_t *call = (f_call_sub_dial_t *)func_cb.f_cb;

    switch (id)
    {
        case COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM9:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt <= 16)
            {
                call->phone_number[call->phone_number_cnt++] = id+0x30 - 1;
            }
            break;

        case COMPO_ID_BTN_CALL:
            bt_pbap_lookup_number((char*)call->phone_number);
            memset(sys_cb.outgoing_number, 0, 16);
            for(int i=0; i<call->phone_number_cnt; i++)
            {
                sys_cb.outgoing_number[i] = call->phone_number[i];
            }
            if(!uteModuleCallBtIsConnected())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
                msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
            }
#if MODEM_CAT1_EN
            if (bsp_modem_get_init_flag())
            {
                modem_call_dial(sys_cb.outgoing_number);
            }
            else
#endif
            {
                bt_call_redial_number();
            }
//            printf("name:%s\n",sys_cb.pbap_result_Name);
            break;

        case COMPO_ID_BTN_DEL:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt > 0)
            {
                call->phone_number[--call->phone_number_cnt] = '\0';
            }
            break;

        default:
            break;
    }

    memset(buf, 0, sizeof(buf));
    if(call->phone_number_cnt)
    {
        snprintf(buf, sizeof(buf), "%s", call->phone_number);
    }
    compo_textbox_set(txt_num, buf);
    func_call_sub_dial_button_release_handle();
}

///长按处理
static void func_call_sub_dial_button_long(void)
{
    int id = compo_get_button_id();
    //获取文本组件的地址
//    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_PIC_NUM);

    //获取数字组件地址
    char buf[34];
    compo_textbox_t *txt_num = compo_getobj_byid(COMPO_ID_NUM_DISP_ZERO);

    f_call_sub_dial_t *call = (f_call_sub_dial_t *)func_cb.f_cb;

    switch (id)
    {
        case COMPO_ID_BTN_NUM7:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt < 16)
            {
                call->phone_number[call->phone_number_cnt++] = '*';
            }
            break;
        case COMPO_ID_BTN_NUM9:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt < 16)
            {
                call->phone_number[call->phone_number_cnt++] = '#';
            }
            break;
        case COMPO_ID_BTN_NUM0:
//            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt < 16)
            {
                call->phone_number[call->phone_number_cnt++] = '+';
            }
            break;
    }

    memset(buf, 0, sizeof(buf));
    if(call->phone_number_cnt)
    {
        snprintf(buf, sizeof(buf), "%s", call->phone_number);
    }
    compo_textbox_set(txt_num, buf);
    func_call_sub_dial_button_release_handle();

}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//电话消息处理
static void func_call_sub_dial_message(size_msg_t msg)
{

    switch (msg)
    {

        case MSG_CTP_TOUCH:
            func_call_sub_dial_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_call_sub_dial_button_click();
            break;
        case MSG_CTP_LONG:
            func_call_sub_dial_button_long();
            break;

        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_call_sub_dial_button_release_handle();
            if (func_cb.flag_sort)
            {
                func_backing_to();
            }
            else
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_LEFT:
            func_call_sub_dial_button_release_handle();
            if (func_cb.flag_sort)
            {

            }
            else
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
            func_call_sub_dial_button_release_handle();
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入打电话界面
static void func_call_sub_dial_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_call_sub_dial_t));
    func_cb.frm_main = func_call_sub_dial_form_create();
}

//电话功能事件处理
static void func_call_sub_dial_process(void)
{
    func_process();                                  //刷新UI
}

//退出电话表盘功能
static void func_call_sub_dial_exit(void)
{
    func_cb.last = FUNC_CALL_SUB_DIAL;
}

//电话显示界面
void func_call_sub_dial(void)
{
    printf("%s\n", __func__);
    func_call_sub_dial_enter();
    while (func_cb.sta == FUNC_CALL_SUB_DIAL)
    {
        func_call_sub_dial_process();
        func_call_sub_dial_message(msg_dequeue());
    }
    func_call_sub_dial_exit();
}
