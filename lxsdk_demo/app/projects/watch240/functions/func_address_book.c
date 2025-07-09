#include "include.h"
#include "func.h"
#include "ute_module_call.h"
#include "ute_module_charencode.h"
#include "func_cover.h"

#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
extern bool func_switching_flag;
enum
{
    COMPO_ID_LISTBOX = 1,

    COMPO_ID_COVER_PIC,
    COMPO_ID_COVER_TXT,

};

typedef struct f_address_book_list_t_
{
    compo_listbox_t *listbox;
    u32 tick;
} f_address_book_list_t;

static const compo_listbox_item_t tbl_call_list[UTE_MODULE_CALL_ADDRESSBOOK_MAX_COUNT] = {0};       //list列表
static ute_module_call_addressbook_t* address_book_tbl = NULL;            //电话簿数据
static u16 address_book_cnt = 0;                                       //联系人个数

//更新电话簿列表回调函数
static bool address_book_update_callback(u32 item_cnt, char* str_txt1, u16 str_txt1_len, char* str_txt2, u16 str_txt2_len, u16 index)
{

    if (index < item_cnt && index < address_book_cnt)
    {
        //需要使用静态数组作为中间变量，以防占用堆栈内存
        //中间缓存大小要比实际获取的名字要大
        //方便再接受到的名字过长时，方便后面使用uteModuleCharencodeGetUtf8String转换的时候可以自动加入省略号
        static char name_utf8[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH+5] = {0};
        uint16_t name_utf8_len = 0;

        static char name_utf8_l[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH+10] = {0};
        uint16_t name_utf8_len_l = 0;

        memset(name_utf8, 0, sizeof(name_utf8));
        memset(name_utf8_l, 0, sizeof(name_utf8_l));

        if (str_txt1_len > sizeof(name_utf8_l))
        {
            str_txt1_len = sizeof(name_utf8_l);
        }

        if (str_txt2_len > address_book_tbl[index].numberAsciiLen)
        {
            str_txt2_len = address_book_tbl[index].numberAsciiLen;
        }

        uteModuleCharencodeUnicodeConversionUtf8(address_book_tbl[index].nameUnicode,
                address_book_tbl[index].nameUnicodeLen,
                (uint8_t*)name_utf8,
                &name_utf8_len,
                sizeof(name_utf8));
//        printf("name=%s",name_utf8);

        if (name_utf8_len >= UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH)
        {

            uteModuleCharencodeGetUtf8String((uint8_t*)name_utf8, name_utf8_len, (uint8_t*)name_utf8_l, &name_utf8_len_l);
            memcpy(str_txt1, name_utf8_l, str_txt1_len);
//            printf("[%d, %d] -> %s, %s\n", name_utf8_len, name_utf8_len_l,  name_utf8, name_utf8_l);
        }
        else
        {
            memcpy(name_utf8_l, name_utf8, sizeof(name_utf8));
            memcpy(str_txt1, name_utf8_l, str_txt1_len);
//            printf("****[%d] -> %s\n", name_utf8_len, name_utf8);
        }

        memcpy(str_txt2, address_book_tbl[index].numberAscii, str_txt2_len);
        return true;
    }
    return false;
}

static u8 func_address_book_update(void)
{
    //获取通讯录联系人
    if (address_book_cnt != uteModuleCallGetAddressBookSize())
    {
        address_book_cnt = uteModuleCallGetAddressBookSize();
        printf("address_book_cnt:%d\n", address_book_cnt);
//        if (func_cb.sta == FUNC_ADDRESS_BOOK) {
        if (address_book_tbl != NULL)
        {
            ab_free(address_book_tbl);
            address_book_tbl = NULL;
        }
        address_book_tbl = ab_zalloc(sizeof(ute_module_call_addressbook_t)*address_book_cnt);
        if (address_book_tbl != NULL)
        {
            printf("update address\n");
            uteModuleCallGetAllAddressBookContactContent(address_book_cnt, address_book_tbl);
            return true;
        }
//        }
    }

    return false;
}

//创建电话簿窗体
compo_form_t *func_address_book_form_create(void)
{
//    printf("name=%s",get_address_name(address_book_tbl[0].numberAscii));
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_CALL_LINK]);

    //创建无消息界面
    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I330001_CALL_NO_CONTACTS_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_COVER_PIC);

    compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_NO_CONTACTS]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*3/4, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_NO_CONTACTS]);
    compo_setid(txt, COMPO_ID_COVER_TXT);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_TWO_TEXT);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    //更新联系人
    func_address_book_update();
    if (address_book_cnt)
    {
        compo_listbox_set(listbox, tbl_call_list, (address_book_cnt < 2) ? 2 : address_book_cnt);
    }
    compo_listbox_set_alike_icon(listbox, UI_BUF_I330001_CALL_CALL_LOG_CONTACTS_BIN);
    compo_listbox_set_text_modify_by_idx_callback2(listbox, address_book_update_callback);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_focus_byidx(listbox, 1);
    compo_listbox_update(listbox);

    compo_textbox_set_visible(txt, address_book_cnt > 0 ? false : true);
    compo_picturebox_set_visible(pic, address_book_cnt > 0 ? false : true);

    return frm;
}

//点进图标进入应用
void func_address_book_icon_click(void)
{
    int icon_idx;
    u8 i = 0;
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;
//    pbap_pb_buf_t *pb_list = f_book->pb_list;
    compo_listbox_t *listbox = f_book->listbox;
    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= address_book_cnt)
    {
        return;
    }

    if(!uteModuleCallBtIsConnected())
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
        msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
    }
    if (bt_is_connected())
    {
        memset(sys_cb.outgoing_number, 0, 16);
        for (i = 0; i < 16; i++)
        {
            sys_cb.outgoing_number[i] = address_book_tbl[icon_idx].numberAscii[i];
        }
        bt_call_redial_number();
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
extern bool func_switching_flag;
enum
{
    COMPO_ID_LISTBOX = 1,

    COMPO_ID_COVER_PIC,
    COMPO_ID_COVER_TXT,

};

typedef struct f_address_book_list_t_
{
    compo_listbox_t *listbox;
    u32 tick;
} f_address_book_list_t;

static const compo_listbox_item_t tbl_call_list[UTE_MODULE_CALL_ADDRESSBOOK_MAX_COUNT] = {0};       //list列表
static ute_module_call_addressbook_t* address_book_tbl = NULL;            //电话簿数据
static u16 address_book_cnt = 0;                                       //联系人个数

//更新电话簿列表回调函数
static bool address_book_update_callback(u32 item_cnt, char* str_txt1, u16 str_txt1_len, char* str_txt2, u16 str_txt2_len, u16 index)
{

    if (index < item_cnt && index < address_book_cnt)
    {
        //需要使用静态数组作为中间变量，以防占用堆栈内存
        //中间缓存大小要比实际获取的名字要大
        //方便再接受到的名字过长时，方便后面使用uteModuleCharencodeGetUtf8String转换的时候可以自动加入省略号
        static char name_utf8[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH+5] = {0};
        uint16_t name_utf8_len = 0;

        static char name_utf8_l[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH+10] = {0};
        uint16_t name_utf8_len_l = 0;

        memset(name_utf8, 0, sizeof(name_utf8));
        memset(name_utf8_l, 0, sizeof(name_utf8_l));

        if (str_txt1_len > sizeof(name_utf8_l))
        {
            str_txt1_len = sizeof(name_utf8_l);
        }

        if (str_txt2_len > address_book_tbl[index].numberAsciiLen)
        {
            str_txt2_len = address_book_tbl[index].numberAsciiLen;
        }

        uteModuleCharencodeUnicodeConversionUtf8(address_book_tbl[index].nameUnicode,
                address_book_tbl[index].nameUnicodeLen,
                (uint8_t*)name_utf8,
                &name_utf8_len,
                sizeof(name_utf8));
//        printf("name=%s",name_utf8);

        if (name_utf8_len >= UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH)
        {

            uteModuleCharencodeGetUtf8String((uint8_t*)name_utf8, name_utf8_len, (uint8_t*)name_utf8_l, &name_utf8_len_l);
            memcpy(str_txt1, name_utf8_l, str_txt1_len);
//            printf("[%d, %d] -> %s, %s\n", name_utf8_len, name_utf8_len_l,  name_utf8, name_utf8_l);
        }
        else
        {
            memcpy(name_utf8_l, name_utf8, sizeof(name_utf8));
            memcpy(str_txt1, name_utf8_l, str_txt1_len);
//            printf("****[%d] -> %s\n", name_utf8_len, name_utf8);
        }

        memcpy(str_txt2, address_book_tbl[index].numberAscii, str_txt2_len);
        return true;
    }
    return false;
}

static u8 func_address_book_update(void)
{
    //获取通讯录联系人
    if (address_book_cnt != uteModuleCallGetAddressBookSize())
    {
        address_book_cnt = uteModuleCallGetAddressBookSize();
        printf("address_book_cnt:%d\n", address_book_cnt);
//        if (func_cb.sta == FUNC_ADDRESS_BOOK) {
        if (address_book_tbl != NULL)
        {
            ab_free(address_book_tbl);
            address_book_tbl = NULL;
        }
        address_book_tbl = ab_zalloc(sizeof(ute_module_call_addressbook_t)*address_book_cnt);
        if (address_book_tbl != NULL)
        {
            printf("update address\n");
            uteModuleCallGetAllAddressBookContactContent(address_book_cnt, address_book_tbl);
            return true;
        }
//        }
    }

    return false;
}

//创建电话簿窗体
compo_form_t *func_address_book_form_create(void)
{
//    printf("name=%s",get_address_name(address_book_tbl[0].numberAscii));
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_CALL_LINK]);

    //创建无消息界面
    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I332001_CALL_NO_CONTACTS_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_COVER_PIC);

    compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_NO_CONTACTS]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*3/4, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_NO_CONTACTS]);
    compo_setid(txt, COMPO_ID_COVER_TXT);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_TWO_TEXT);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    //更新联系人
    func_address_book_update();
    if (address_book_cnt)
    {
        compo_listbox_set(listbox, tbl_call_list, (address_book_cnt < 2) ? 2 : address_book_cnt);
    }
    compo_listbox_set_alike_icon(listbox, UI_BUF_I332001_CALL_CALL_LOG_CONTACTS_G_BIN);
    compo_listbox_set_text_modify_by_idx_callback2(listbox, address_book_update_callback);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_focus_byidx(listbox, 1);
    compo_listbox_update(listbox);

    compo_textbox_set_visible(txt, address_book_cnt > 0 ? false : true);
    compo_picturebox_set_visible(pic, address_book_cnt > 0 ? false : true);

    return frm;
}

//点进图标进入应用
void func_address_book_icon_click(void)
{
    int icon_idx;
    u8 i = 0;
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;
//    pbap_pb_buf_t *pb_list = f_book->pb_list;
    compo_listbox_t *listbox = f_book->listbox;
    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= address_book_cnt)
    {
        return;
    }

    if(!uteModuleCallBtIsConnected())
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
        msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
    }
    if (bt_is_connected())
    {
        memset(sys_cb.outgoing_number, 0, 16);
        for (i = 0; i < 16; i++)
        {
            sys_cb.outgoing_number[i] = address_book_tbl[icon_idx].numberAscii[i];
        }
        bt_call_redial_number();
    }
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//电话簿功能事件处理
static void func_address_book_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;

    //两秒更新一次
    if (tick_check_expire(f_book->tick, 1000))
    {
        f_book->tick = tick_get();
        //更新联系人
        u8 sta = func_address_book_update();
        if (sta)
        {
            compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_COVER_TXT);
            compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_COVER_PIC);
            if (address_book_cnt)
            {
                compo_listbox_set_visible(f_book->listbox, true);
                compo_listbox_set(f_book->listbox, tbl_call_list, (address_book_cnt < 2) ? 2 : address_book_cnt);
                compo_listbox_set_alike_icon(f_book->listbox, UI_BUF_I330001_CALL_CALL_LOG_CONTACTS_BIN);
                compo_listbox_move_init_modify(f_book->listbox, 127-30, compo_listbox_gety_byidx(f_book->listbox, (address_book_cnt - 2 > 0) ? address_book_cnt - 2 : 1));
                compo_listbox_update_with_text_scroll_rst(f_book->listbox);
                compo_listbox_set_bgimg(f_book->listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
                compo_listbox_set_focus_byidx(f_book->listbox, 1);
                compo_listbox_update(f_book->listbox);
            }
            else
            {
                compo_listbox_set_visible(f_book->listbox, false);
                compo_listbox_set(f_book->listbox, tbl_call_list, address_book_cnt);
            }
            compo_textbox_set_visible(txt, address_book_cnt > 0 ? false : true);
            compo_picturebox_set_visible(pic, address_book_cnt > 0 ? false : true);
        }
    }

    compo_listbox_move(f_book->listbox);
    compo_listbox_update(f_book->listbox);
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;

    //两秒更新一次
    if (tick_check_expire(f_book->tick, 1000))
    {
        f_book->tick = tick_get();
        //更新联系人
        u8 sta = func_address_book_update();
        if (sta)
        {
            compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_COVER_TXT);
            compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_COVER_PIC);
            if (address_book_cnt)
            {
                compo_listbox_set_visible(f_book->listbox, true);
                compo_listbox_set(f_book->listbox, tbl_call_list, (address_book_cnt < 2) ? 2 : address_book_cnt);
                compo_listbox_set_alike_icon(f_book->listbox, UI_BUF_I332001_CALL_CALL_LOG_CONTACTS_G_BIN);
                compo_listbox_move_init_modify(f_book->listbox, 127-30, compo_listbox_gety_byidx(f_book->listbox, (address_book_cnt - 2 > 0) ? address_book_cnt - 2 : 1));
                compo_listbox_update_with_text_scroll_rst(f_book->listbox);
                compo_listbox_set_bgimg(f_book->listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
                compo_listbox_set_focus_byidx(f_book->listbox, 1);
                compo_listbox_update(f_book->listbox);
            }
            else
            {
                compo_listbox_set_visible(f_book->listbox, false);
                compo_listbox_set(f_book->listbox, tbl_call_list, address_book_cnt);
            }
            compo_textbox_set_visible(txt, address_book_cnt > 0 ? false : true);
            compo_picturebox_set_visible(pic, address_book_cnt > 0 ? false : true);
        }
    }

    compo_listbox_move(f_book->listbox);
    compo_listbox_update(f_book->listbox);
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

    func_process();
}

//电话簿功能消息处理
static void func_address_book_message(size_msg_t msg)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_book->listbox;

    if (address_book_cnt)
    {
        if (compo_listbox_message(listbox, msg))
        {
            return;                                         //处理列表框信息
        }
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            if (address_book_cnt)
            {
                func_address_book_icon_click();
            }
            break;

        case MSG_CTP_LONG:
            break;

        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {

            }
            break;

        case MSG_CTP_SHORT_RIGHT:
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
            if (func_cb.flag_sort)
            {

            }
            else
            {
                func_message(msg);
            }
            break;

        default:
            func_message(msg);
            break;
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_book->listbox;

    if (address_book_cnt)
    {
        if (compo_listbox_message(listbox, msg))
        {
            return;                                         //处理列表框信息
        }
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            if (address_book_cnt)
            {
                func_address_book_icon_click();
            }
            break;

        case MSG_CTP_LONG:
            break;

        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {

            }
            break;

        case MSG_CTP_SHORT_RIGHT:
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
            if (func_cb.flag_sort)
            {

            }
            else
            {
                func_message(msg);
            }
            break;

        default:
            func_message(msg);
            break;
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}


//进入电话簿功能
static void func_address_book_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_address_book_list_t));
    func_cb.frm_main = func_address_book_form_create();
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;
    f_book->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_book->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init_modify(f_book->listbox, 127-30, compo_listbox_gety_byidx(f_book->listbox, (address_book_cnt - 2 > 0) ? address_book_cnt - 2 : 1));
    func_cb.enter_tick = tick_get();
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;
    f_book->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_book->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init_modify(f_book->listbox, 127-30, compo_listbox_gety_byidx(f_book->listbox, (address_book_cnt - 2 > 0) ? address_book_cnt - 2 : 1));
    func_cb.enter_tick = tick_get();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}


//退出电话簿功能
static void func_address_book_exit(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_book->listbox;

    if (address_book_tbl != NULL)
    {
        ab_free(address_book_tbl);
        address_book_tbl = NULL;
    }
    address_book_cnt = 0;
    func_free(listbox->mcb);
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    f_address_book_list_t *f_book = (f_address_book_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_book->listbox;

    if (address_book_tbl != NULL)
    {
        ab_free(address_book_tbl);
        address_book_tbl = NULL;
    }
    address_book_cnt = 0;
    func_free(listbox->mcb);
#endif //GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_cb.last = FUNC_ADDRESS_BOOK;
}

//电话簿功能
void func_address_book(void)
{
    printf("%s\n", __func__);
    func_address_book_enter();
    while (func_cb.sta == FUNC_ADDRESS_BOOK)
    {
        func_address_book_process();
        func_address_book_message(msg_dequeue());
    }
    func_address_book_exit();
}
#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
