#include "include.h"
#include "func.h"
#include "ute_module_charencode.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define DOUSING_LIST_CNT                       ((int)(sizeof(tbl_dousing_list) / sizeof(tbl_dousing_list[0])))

enum
{
    COMPO_ID_BTN_NUM0 = 0,
    COMPO_ID_BTN_NUM1,
    COMPO_ID_BTN_NUM2,
    COMPO_ID_BTN_NUM3,
    COMPO_ID_BTN_NUM4,
    COMPO_ID_BTN_NUM5,
    COMPO_ID_BTN_NUM6,
    COMPO_ID_BTN_NUM7,
    COMPO_ID_LISTBOX,
};

typedef struct f_dousing_list_t_
{
    char str[30];
    compo_listbox_t *listbox;

} f_dousing_list_t;

static const compo_listbox_item_t tbl_dousing_list[] =
{
//    {STR_SEC,         .vidx = COMPO_ID_BTN_NUM0},
//    {STR_SEC,          .vidx = COMPO_ID_BTN_NUM1},
//    {STR_SEC,       .vidx = COMPO_ID_BTN_NUM2},
//    {STR_MIN,       .vidx = COMPO_ID_BTN_NUM3},
//    {STR_MIN,          .vidx = COMPO_ID_BTN_NUM4},
//    {STR_LIGHT_ALWAYS,     .vidx = COMPO_ID_BTN_NUM5},

    {     .vidx = COMPO_ID_BTN_NUM0},
    {     .vidx = COMPO_ID_BTN_NUM1},
    {     .vidx = COMPO_ID_BTN_NUM2},
    {     .vidx = COMPO_ID_BTN_NUM3},
    {     .vidx = COMPO_ID_BTN_NUM4},
    {     .vidx = COMPO_ID_BTN_NUM5},
};

u8 func_sel_dousing_bit(uint n)
{
    if(sys_cb.set_sleep_time_id == n)
    {
        return true;
    }

    return false;
}

static compo_listbox_custom_item_t tbl_list_dousing[6]= {0};


char *back_string(char *num,char*txt)
{
    if(func_cb.sta == FUNC_SET_SUB_DOUSING)
    {
        f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
        memset(f_set->str,0,sizeof(f_set->str));
        uteModuleCharencodeReplaceSubString(txt, f_set->str,"##",num);
        return f_set->str;
    }
    return NULL;
}

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//熄屏设置页面
compo_form_t *func_set_sub_dousing_form_create(void)
{

    switch (uteModuleGuiCommonGetDisplayOffTime())
    {
        case 5:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM0;
            break;

        case 10:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM1;
            break;

        case 20:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM2;
            break;

        case 30:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM3;
            break;

        case 60:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM4;
            break;

        case 0:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM5;
            break;
    }

    snprintf(tbl_list_dousing[0].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("5",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[1].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("10",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[2].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("20",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[3].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("30",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[4].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("1",i18n[STR_MIN_JOINT]));
    snprintf(tbl_list_dousing[5].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",i18n[STR_LIGHT_ALWAYS]);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_DOUSING]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_dousing_list, DOUSING_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_text_modify(listbox, tbl_list_dousing);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I330001_PUBLIC_GOU_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_dousing_bit);

    compo_setid(listbox, COMPO_ID_LISTBOX);
    uint8_t set_idx = 0;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);


    return frm;
}

//点进图标进入应用
void func_set_sub_dousing_list_icon_click(void)
{
    int icon_idx;
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= DOUSING_LIST_CNT)
    {
        return;
    }
    sys_cb.set_sleep_time_id = icon_idx;
    //切入应用
    switch(icon_idx)
    {
        case  COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM6:
            if (icon_idx==COMPO_ID_BTN_NUM0)
            {
                uteModuleGuiCommonSetDisplayOffTime(5);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM1)
            {
                uteModuleGuiCommonSetDisplayOffTime(10);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM2)
            {
                uteModuleGuiCommonSetDisplayOffTime(20);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM3)
            {
                uteModuleGuiCommonSetDisplayOffTime(30);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM4)
            {
                uteModuleGuiCommonSetDisplayOffTime(60);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM5)
            {
                uteModuleGuiCommonSetDisplayOffTime(0);
            }
            compo_listbox_update(listbox);
            break;

        default:
            break;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//熄屏设置页面
compo_form_t *func_set_sub_dousing_form_create(void)
{

    switch (uteModuleGuiCommonGetDisplayOffTime())
    {
        case 5:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM0;
            break;

        case 10:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM1;
            break;

        case 20:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM2;
            break;

        case 30:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM3;
            break;

        case 60:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM4;
            break;

        case 0:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM5;
            break;
    }

    snprintf(tbl_list_dousing[0].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("5",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[1].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("10",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[2].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("20",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[3].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("30",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[4].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("1",i18n[STR_MIN_JOINT]));
    snprintf(tbl_list_dousing[5].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",i18n[STR_LIGHT_ALWAYS]);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_DOUSING]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_dousing_list, DOUSING_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_text_modify(listbox, tbl_list_dousing);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I332001_PUBLIC_GOU_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_dousing_bit);

    compo_setid(listbox, COMPO_ID_LISTBOX);
    uint8_t set_idx = 0;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);


    return frm;
}

//点进图标进入应用
void func_set_sub_dousing_list_icon_click(void)
{
    int icon_idx;
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= DOUSING_LIST_CNT)
    {
        return;
    }
    sys_cb.set_sleep_time_id = icon_idx;
    //切入应用
    switch(icon_idx)
    {
        case  COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM6:
            if (icon_idx==COMPO_ID_BTN_NUM0)
            {
                uteModuleGuiCommonSetDisplayOffTime(5);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM1)
            {
                uteModuleGuiCommonSetDisplayOffTime(10);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM2)
            {
                uteModuleGuiCommonSetDisplayOffTime(20);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM3)
            {
                uteModuleGuiCommonSetDisplayOffTime(30);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM4)
            {
                uteModuleGuiCommonSetDisplayOffTime(60);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM5)
            {
                uteModuleGuiCommonSetDisplayOffTime(0);
            }
            compo_listbox_update(listbox);
            break;

        default:
            break;
    }
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//熄屏时长功能事件处理
static void func_set_sub_dousing_list_process(void)
{
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
    compo_listbox_move(f_set->listbox);
    func_process();
}

//熄屏时长功能消息处理
static void func_set_sub_dousing_list_message(size_msg_t msg)
{
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_set_sub_dousing_list_icon_click();                //单击图标
            break;


        case MSG_CTP_LONG:
            break;

        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {

            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入设置功能
static void func_set_sub_dousing_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_dousing_list_t));
    func_cb.frm_main = func_set_sub_dousing_form_create();

    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
    f_set->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放

    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, DOUSING_LIST_CNT - 2));
    func_cb.enter_tick = tick_get();
}

//退出设置功能
static void func_set_sub_dousing_exit(void)
{
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    func_free(listbox->mcb);                                            //释放移动控制块
    func_cb.last = FUNC_SET_SUB_DOUSING;
}

//设置功能
void func_set_sub_dousing(void)
{
    printf("%s\n", __func__);
    func_set_sub_dousing_enter();
    while (func_cb.sta == FUNC_SET_SUB_DOUSING)
    {
        func_set_sub_dousing_list_process();
        func_set_sub_dousing_list_message(msg_dequeue());
    }
    func_set_sub_dousing_exit();
}
