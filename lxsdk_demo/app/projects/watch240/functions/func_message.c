#include "include.h"
#include "func.h"
#include "ute_module_notify.h"
#include "func_cover.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{
    //MSGBOX_MSG_TYPE_NONE,
    MSGBOX_MSG_TYPE_WECHAT=1,   //微信消息
};

#define MSG_MAX_LIST_CNT    (CARD_TOTAL_CNT - COMPO_CARD_START -1)  //最多保存多少条消息

#define MSG_MAX_CONTENT     64  //消息保存的最大内容

enum
{
    COMPO_CARD_START = FUNC_MAX_NUM,
    COMPO_ID_CARD_MSG1,
    COMPO_ID_CARD_MSGn = COMPO_CARD_START + UTE_MODULE_NOTIFY_SAVE_CNT,

    CARD_TOTAL_CNT,

    COMPO_ID_BTN_DEL,

    COMPO_ID_TXT_DEL,

    COMPO_ID_COVER_PIC,
    COMPO_ID_COVER_TXT,
};

//按钮
typedef struct message_btn_t_
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u32 res;
} message_btn_t;

#define MSG_BTN_CNT     ((int)(sizeof(message_btn)/sizeof(message_btn[0])))

const message_btn_t message_btn[] =
{
    {COMPO_ID_BTN_DEL,  GUI_SCREEN_CENTER_X,  0,    70,      70,      UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN},
};
//消息界面对应卡片结构
typedef struct card_rect_t_
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u16 r;
} card_rect_t;

typedef struct card_icon_t_
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u32 res;
} card_icon_t;

typedef struct card_text_t_
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    const char* str;
    u32 res;
    bool center;
    bool wordwrap;
    u8 r;
    u8 g;
    u8 b;
} card_text_t;

#define CARD_TYPEDEF_T(type, rect_cnt, icon_cnt, text_cnt) \
typedef struct card_##type##_t_ {   \
    s32 x;                              \
    s32 y;                              \
    u16 w;                              \
    u16 h;                              \
    card_rect_t rect[rect_cnt+1];         \
    card_icon_t icon[icon_cnt+1];         \
    card_text_t text[text_cnt+1];         \
} card_##type##_t;

#define CARD_T(type) card_##type##_t

///创建卡片
#define CREAT_CARD(compo_id, rect_cnt, icon_cnt, text_cnt, card0, ORG_W, ORG_H) \
    compo_cardbox_t * cardbox = compo_cardbox_create(frm, rect_cnt, icon_cnt, text_cnt, ORG_W, ORG_H); \
    compo_cardbox_set_visible(cardbox, false); \
    compo_cardbox_set_location(cardbox, card0.x, card0.y + ((card0.h+10) * (compo_id-1)), card0.w, card0.h);\
    compo_setid(cardbox, compo_id); \
    for (u8 i=0; i<rect_cnt; i++) { \
        compo_cardbox_rect_set_location(cardbox, card0.rect[i].id, card0.rect[i].x, card0.rect[i].y, card0.rect[i].w, card0.rect[i].h, card0.rect[i].r); \
        compo_cardbox_rect_set_color(cardbox, card0.rect[i].id, make_color(41,41,41)); \
    } \
    for (u8 i=0; i<icon_cnt; i++) { \
        compo_cardbox_icon_set_location(cardbox, card0.icon[i].id, card0.icon[i].x, card0.icon[i].y, card0.icon[i].w, card0.icon[i].h); \
        compo_cardbox_icon_set(cardbox, card0.icon[i].id, card0.icon[i].res); \
    } \
    for (u8 i=0; i<text_cnt; i++) { \
        compo_cardbox_text_set_font(cardbox, card0.text[i].id, card0.text[i].res); \
        compo_cardbox_text_set(cardbox, card0.text[i].id, card0.text[i].str); \
        if (card0.text[i].w == 0 || card0.text[i].h == 0) { \
            compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, true); \
            widget_text_set_ellipsis(cardbox->text[card0.text[i].id], false);   \
        } else { \
            compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, false); \
            widget_text_set_ellipsis(cardbox->text[card0.text[i].id], true);   \
        } \
        widget_set_align_center(cardbox->text[card0.text[i].id], card0.text[i].center); \
        widget_text_set_wordwrap(cardbox->text[card0.text[i].id], card0.text[i].wordwrap); \
        widget_text_set_color(cardbox->text[card0.text[i].id], make_color(card0.text[i].r, card0.text[i].g, card0.text[i].b)); \
        compo_cardbox_text_set_location(cardbox, card0.text[i].id, card0.text[i].x, card0.text[i].y, card0.text[i].w, card0.text[i].h); \
    }

#define CARD_RECT_CNT      1
#define CARD_ICON_CNT      1
#define CARD_TEXT_CNT      2
#define CARD_MSG_X                 6+228/2                     //中心点
#define CARD_MSG_START_Y           54+110/2
CARD_TYPEDEF_T(msg, CARD_RECT_CNT, CARD_ICON_CNT, CARD_TEXT_CNT);
const CARD_T(msg) card =
{
    CARD_MSG_X,
    CARD_MSG_START_Y,
    228,//GUI_SCREEN_WIDTH-12,
    110,//GUI_SCREEN_HEIGHT/3,

    {
        ///矩形背景
        //id      x       y         w        h      r
        {0,       0,      8,   228,    94,    16},
    },

    {
        ///图标
        //id      x                   y               w       h       res
        {0,       12+36/2-228/2,      36/2-110/2,      32,     32,     0},
    },

    {
        ///文本
        //id      x                 y       w       h       str                                res                         center  wordwrap   r  g  b
        {0,      -18,               -38,    118,    55,      "2024-10-28 19:08",                UI_BUF_0FONT_FONT_BIN,      false,   false,  148, 148, 148},
        {1,      3+36/2-228/2,      -10,    205,    55,     "Hello! You are a good man ...",   UI_BUF_0FONT_FONT_BIN,     false,   true,   255, 255, 255},
    },
};


typedef struct f_message_t_
{
    bool flag_drag;
    s32 y_pos;  //上次松手y坐标
    s32 y;      //当前要对齐的y坐标

    s32 y_min;  //可拖动y轴最小范围
    s32 y_max;  //可拖动y轴最大范围

    bool flag_move_auto;
    u32 tick;

    s32 moveto_y;

    u8 debug_msg; //测试

    //message_info_t msg[MSG_MAX_LIST_CNT];
    ute_module_notify_data_t *ute_msg;

    char tmp_msg[UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+4];

} f_message_t;

static void func_message_card_update(bool fist, compo_form_t *frm);


//创建消息窗体
compo_form_t *func_message_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MESSAGE]);

    //创建消息卡片
    for (int i=0; i<MSG_MAX_LIST_CNT; i++)
    {
        u8 id = COMPO_ID_CARD_MSG1+i;
        CREAT_CARD(id, CARD_RECT_CNT, CARD_ICON_CNT, CARD_TEXT_CNT, card, card.w, card.h);
    }


    for (int i=0; i<MSG_BTN_CNT; i++)
    {
        //创建按钮
        compo_button_t* btn;
        if (message_btn[i].res)
        {
            btn = compo_button_create_by_image(frm, message_btn[i].res);
        }
        compo_button_set_pos(btn, message_btn[i].x, message_btn[i].y);
        compo_button_set_visible(btn, false);
        compo_setid(btn, message_btn[i].id);

        //创建删除按钮文本
        compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR_ALL]));
        compo_textbox_set_location(text, message_btn[i].x, message_btn[i].y, gui_image_get_size(message_btn[i].res).wid - gui_image_get_size(message_btn[i].res).hei, gui_image_get_size(message_btn[i].res).hei);
        compo_textbox_set(text, i18n[STR_CLEAR_ALL]);
        compo_textbox_set_visible(text, false);
        compo_setid(text, COMPO_ID_TXT_DEL);
    }


    //创建无消息界面
    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I330001_NOTIFICATION_NO_DATA_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_COVER_PIC);

    compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_NO_MSG]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 184, GUI_SCREEN_WIDTH/1.1, 30);
    compo_textbox_set(txt, i18n[STR_NO_MSG]);
    compo_textbox_set_visible(txt, false);
    compo_setid(txt, COMPO_ID_COVER_TXT);

    if (func_cb.sta == FUNC_MESSAGE)
    {
        func_cb.frm_main = frm;
    }

//    if (func_cb.sta == FUNC_MESSAGE) {
    func_message_card_update(true, frm);
//    }

    return frm;
}

static void func_message_btn_click(void)
{
    f_message_t *f_msg = (f_message_t *)func_cb.f_cb;
    u8 id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_DEL:
            //清除消息
            uteModuleNotifyDelAllHistoryData(true);
            memset(f_msg->ute_msg, 0, sizeof(ute_module_notify_data_t));
//            f_msg->y_pos = 0;
//            f_msg->y = 0;
//            f_msg->y_max = 0;
//            card_y += (card.h+10);
//            f_msg->y_max = ;
            widget_page_set_client(func_cb.frm_main->page_body, 0, -(card.h + 10) / 4);
            break;
    }


}

//获取点击的卡片组件id
static u16 func_message_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<MSG_MAX_LIST_CNT; i++)
    {
        id = COMPO_ID_CARD_MSG1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

static void func_message_card_click(void)
{
    f_message_t *f_msg = (f_message_t *)func_cb.f_cb;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_message_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > CARD_TOTAL_CNT - 1)
    {
        return;
    }
//    printf("click compo_id:%d\n", compo_id);
    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        uteModuleNotifyGetData(f_msg->ute_msg);
        u16 msg_id = compo_id - COMPO_CARD_START - 1;
        char *msg = (char*)f_msg->ute_msg->historyNotify[msg_id].content;
        char time[30]= {0};

//        printf("===================================================>>>>>>>\n");
//        print_r(msg, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
//        printf("===================================================>>>>>>>\n");

        memset(f_msg->tmp_msg, '\0', sizeof(f_msg->tmp_msg));
        memcpy(f_msg->tmp_msg, msg, strlen(msg));
        //消息内容超过UTE_NOTIFY_MSG_CONTENT_MAX_SIZE补充省略号
        if (strlen(msg) >= UTE_NOTIFY_MSG_CONTENT_MAX_SIZE-2)
        {
            memset(&f_msg->tmp_msg[strlen(msg)], '.', 3);
            f_msg->tmp_msg[strlen(msg)+3] = '\0';
        }

//        printf("===================================================>>>>>>>\n");
//        print_r(f_msg->tmp_msg, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
//        printf("===================================================>>>>>>>\n");

        printf("msg[%d]:%s\n", strlen(msg), f_msg->tmp_msg);
        // sprintf(time, "%04d/%02d/%02d", f_msg->ute_msg->historyNotify[msg_id].year, f_msg->ute_msg->historyNotify[msg_id].month,
        //         f_msg->ute_msg->historyNotify[msg_id].day);
        ute_module_systemtime_time_t time_data;
        uteModuleSystemtimeGetTime(&time_data);//获取系统时间
        u8 time_disp_state = 1;

        if(time_data.year != f_msg->ute_msg->historyNotify[msg_id].year || time_data.month != f_msg->ute_msg->historyNotify[msg_id].month)
        {
            time_disp_state = 0;
        }
        else if(time_data.day > f_msg->ute_msg->historyNotify[msg_id].day && time_data.month == f_msg->ute_msg->historyNotify[msg_id].month)
        {
            time_disp_state = 1;
        }
        else
        {
            time_disp_state = 2;
        }
        switch(time_disp_state)
        {
            case 0:
                sprintf((char*)time, "%04d/%02d/%02d", //record_tbl[index].callTime.year,
                        f_msg->ute_msg->historyNotify[msg_id].year,
                        f_msg->ute_msg->historyNotify[msg_id].month,
                        f_msg->ute_msg->historyNotify[msg_id].day);
                break;
            case 1:
                sprintf((char*)time, "%02d/%02d", //record_tbl[index].callTime.year,
                        f_msg->ute_msg->historyNotify[msg_id].month,
                        f_msg->ute_msg->historyNotify[msg_id].day);
                break;
            case 2:
                sprintf((char*)time, "%02d:%02d", //record_tbl[index].callTime.year,
                        f_msg->ute_msg->historyNotify[msg_id].hour,
                        f_msg->ute_msg->historyNotify[msg_id].min);
                break;
        }

        sys_cb.msg_index = f_msg->ute_msg->historyNotify[msg_id].type;
        int res = msgbox(f_msg->tmp_msg, NULL, time, MSGBOX_MODE_BTN_DELETE, MSGBOX_MSG_TYPE_DETAIL);
        if (res == MSGBOX_RES_DELETE)
        {
            uteModuleNotifySetDisplayIndex(msg_id);
            uteModuleNotifyDelAllHistoryData(false);
            uteModuleNotifyGetData(f_msg->ute_msg);
            if (uteModuleNotifyGetTotalNotifyCnt() > 1)
            {
                f_msg->flag_drag = true;
                f_msg->flag_move_auto = false;
            }
            if(compo_id == 1)
            {
                widget_page_set_client(func_cb.frm_main->page_body, 0, -(card.h + 10) / 4);
            }
        }
    }

}

static u32 func_message_card_get_icon(u8 type)
{
    if (type < func_cover_get_detail_msg_cnt())
    {
        return func_cover_get_detail_msg_ui(type);
    }
    return 0;
}

//更新消息卡片
static void func_message_card_update(bool first_update, compo_form_t *frm)
{
    f_message_t *f_msg = NULL;

    if (first_update == true && func_cb.sta != FUNC_MESSAGE)
    {
        f_msg = ab_zalloc(sizeof(f_message_t));
        f_msg->ute_msg = ab_zalloc(sizeof(ute_module_notify_data_t));
        if (f_msg->ute_msg == NULL)
        {
            printf("%s malloc err\n", __func__);
            halt(0x1010);
        }
    }
    else if (func_cb.sta == FUNC_MESSAGE)
    {
        f_msg = (f_message_t *)func_cb.f_cb;
    }
    //更新卡片
    uteModuleNotifyGetData(f_msg->ute_msg);
    s32 card_y = 0;
    for (int i=0; i<MSG_MAX_LIST_CNT; i++)
    {

        compo_cardbox_t* cardbox = compo_getobj_byid(COMPO_ID_CARD_MSG1+i);

        if (i <= uteModuleNotifyGetTotalNotifyCnt()-1)
        {

            char* time = (char*)func_zalloc(30);
            char* msg = (char*)f_msg->ute_msg->historyNotify[i].content;
            // sprint/f(time, "%04d/%02d/%02d", f_msg->ute_msg->historyNotify[i].year, f_msg->ute_msg->historyNotify[i].month,
            // f_msg->ute_msg->historyNotify[i].day);

            ute_module_systemtime_time_t time_data;
            uteModuleSystemtimeGetTime(&time_data);//获取系统时间
            u8 time_disp_state = 0;

            if(time_data.year != f_msg->ute_msg->historyNotify[i].year || time_data.month != f_msg->ute_msg->historyNotify[i].month)
            {
                time_disp_state = 0;
            }
            else if(time_data.day > f_msg->ute_msg->historyNotify[i].day && time_data.month == f_msg->ute_msg->historyNotify[i].month)
            {
                time_disp_state = 1;
            }
            else
            {
                time_disp_state = 2;
            }
            switch(time_disp_state)
            {
                case 0:
                    sprintf((char*)time, "%04d/%02d/%02d", //record_tbl[index].callTime.year,
                            f_msg->ute_msg->historyNotify[i].year,
                            f_msg->ute_msg->historyNotify[i].month,
                            f_msg->ute_msg->historyNotify[i].day);
                    break;
                case 1:
                    sprintf((char*)time, "%02d/%02d", //record_tbl[index].callTime.year,
                            f_msg->ute_msg->historyNotify[i].month,
                            f_msg->ute_msg->historyNotify[i].day);
                    break;
                case 2:
                    sprintf((char*)time, "%02d:%02d", //record_tbl[index].callTime.year,
                            f_msg->ute_msg->historyNotify[i].hour,
                            f_msg->ute_msg->historyNotify[i].min);
                    break;
            }


            card_y += (card.h+10);
            compo_cardbox_set_visible(cardbox, true);
            compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, card_y);
            compo_cardbox_icon_set(cardbox, 0, func_message_card_get_icon(f_msg->ute_msg->historyNotify[i].type));
            compo_cardbox_icon_set_location(cardbox, card.icon[0].id, card.icon[0].x, card.icon[0].y, card.icon[0].w, card.icon[0].h);
            compo_cardbox_text_set(cardbox, 0, time);
            widget_text_set_right_align(cardbox->text[0],true);
            compo_cardbox_text_set(cardbox, 1, msg);

            func_free(time);
        }
        else
        {
            compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, -GUI_SCREEN_HEIGHT);
            compo_cardbox_set_visible(cardbox, false);
        }
    }

    if (card_y)
    {
//        printf("card_y=%d\n",card_y);
////        if(card_y<=GUI_SCREEN_WIDTH/2)
////        {
////            card_y-=40;
////        }
//        card_y-=40;
//        printf("card_y=%d\n",card_y);
        //更新按钮
        s32 btn_y  = 0;
        for (int i=0; i<MSG_BTN_CNT; i++)
        {
            btn_y = card_y + (card.h + 10) / 2 + gui_image_get_size(message_btn[i].res).hei/2 + 10;
//            printf("btn_y [%d,%d]\n", btn_y, GUI_SCREEN_HEIGHT - gui_image_get_size(message_btn[i].res).hei/2);
            if (btn_y < GUI_SCREEN_HEIGHT - gui_image_get_size(message_btn[i].res).hei/2)   // 按钮位置小于屏幕底部特殊处理
            {
                btn_y = GUI_SCREEN_HEIGHT - gui_image_get_size(message_btn[i].res).hei/2;
            }

            compo_button_t* btn = compo_getobj_byid(COMPO_ID_BTN_DEL+i);
            compo_button_set_visible(btn, true);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, btn_y);

            compo_textbox_t* text = compo_getobj_byid(COMPO_ID_TXT_DEL+i);
            compo_textbox_set_visible(text, true);
            compo_textbox_set_pos(text, GUI_SCREEN_CENTER_X, btn_y);

        }

        //显示无消息页面
        compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_COVER_TXT);
        compo_textbox_set_visible(txt, false);
        compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_COVER_PIC);
        compo_picturebox_set_visible(pic, false);
//        if (first_update == false && func_cb.sta == FUNC_MESSAGE)
//        if (func_cb.sta == FUNC_MESSAGE)
//        {
        //更新拖动范围
        f_msg->y_min = -(btn_y - (GUI_SCREEN_HEIGHT - gui_image_get_size(message_btn[0].res).hei) + 32);
        f_msg->y_max = -65;//-(card.h + 10) / 2;
        if (btn_y + gui_image_get_size(message_btn[0].res).hei / 2 <= GUI_SCREEN_HEIGHT || uteModuleNotifyGetTotalNotifyCnt() == 1 || first_update == true)
        {
            //f_msg->flag_drag = false;
            f_msg->y_min = f_msg->y_max;
            if (func_cb.sta == FUNC_MESSAGE)
            {
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_msg->y_max);
            }
            else
            {
                if (frm != NULL)
                {
                    widget_page_set_client(frm->page_body, 0, f_msg->y_max);
                }
            }
        }
//        }
        //printf("drag=>[%d,%d] -> [%d,%d,%d]\n", f_msg->y_min, f_msg->y_max, btn_y, card_y, (GUI_SCREEN_HEIGHT - message_btn[0].w / 2));
    }
    else
    {
        //更新按钮
        for (int i=0; i<MSG_BTN_CNT; i++)
        {
            compo_button_t* btn = compo_getobj_byid(COMPO_ID_BTN_DEL+i);
            compo_button_set_visible(btn, false);
            compo_textbox_t* text = compo_getobj_byid(COMPO_ID_TXT_DEL+i);
            compo_textbox_set_visible(text, false);
        }

        //显示无消息页面
        compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_COVER_TXT);
        compo_textbox_set_visible(txt, true);
        compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_COVER_PIC);
        compo_picturebox_set_visible(pic, true);



        //更新拖动状态
        f_msg->flag_drag = false;
    }


    if (first_update == true && func_cb.sta != FUNC_MESSAGE)
    {
        if (f_msg != NULL)
        {
            if (f_msg->ute_msg != NULL)
            {
                ab_free(f_msg->ute_msg);
                f_msg->ute_msg = NULL;
            }
            ab_free(f_msg);
            f_msg = NULL;
        }
    }

}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum
{
    //MSGBOX_MSG_TYPE_NONE,
    MSGBOX_MSG_TYPE_WECHAT=1,   //微信消息
};

#define MSG_MAX_LIST_CNT    (CARD_TOTAL_CNT - COMPO_CARD_START -1)  //最多保存多少条消息

#define MSG_MAX_CONTENT     64  //消息保存的最大内容

enum
{
    COMPO_CARD_START = FUNC_MAX_NUM,
    COMPO_ID_CARD_MSG1,
    COMPO_ID_CARD_MSGn = COMPO_CARD_START + UTE_MODULE_NOTIFY_SAVE_CNT,

    CARD_TOTAL_CNT,

    COMPO_ID_BTN_DEL,

    COMPO_ID_TXT_DEL,

    COMPO_ID_COVER_PIC,
    COMPO_ID_COVER_TXT,
};

//按钮
typedef struct message_btn_t_
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u32 res;
} message_btn_t;

#define MSG_BTN_CNT     ((int)(sizeof(message_btn)/sizeof(message_btn[0])))

const message_btn_t message_btn[] =
{
    {COMPO_ID_BTN_DEL,  GUI_SCREEN_CENTER_X,  0,    70,      70,      UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN},
};
//消息界面对应卡片结构
typedef struct card_rect_t_
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u16 r;
} card_rect_t;

typedef struct card_icon_t_
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u32 res;
} card_icon_t;

typedef struct card_text_t_
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    const char* str;
    u32 res;
    bool center;
    bool wordwrap;
    u8 r;
    u8 g;
    u8 b;
} card_text_t;

#define CARD_TYPEDEF_T(type, rect_cnt, icon_cnt, text_cnt) \
typedef struct card_##type##_t_ {   \
    s32 x;                              \
    s32 y;                              \
    u16 w;                              \
    u16 h;                              \
    card_rect_t rect[rect_cnt+1];         \
    card_icon_t icon[icon_cnt+1];         \
    card_text_t text[text_cnt+1];         \
} card_##type##_t;

#define CARD_T(type) card_##type##_t

///创建卡片
#define CREAT_CARD(compo_id, rect_cnt, icon_cnt, text_cnt, card0, ORG_W, ORG_H) \
    compo_cardbox_t * cardbox = compo_cardbox_create(frm, rect_cnt, icon_cnt, text_cnt, ORG_W, ORG_H); \
    compo_cardbox_set_visible(cardbox, false); \
    compo_cardbox_set_location(cardbox, card0.x, card0.y + ((card0.h+10) * (compo_id-1)), card0.w, card0.h);\
    compo_setid(cardbox, compo_id); \
    for (u8 i=0; i<rect_cnt; i++) { \
        compo_cardbox_rect_set_location(cardbox, card0.rect[i].id, card0.rect[i].x, card0.rect[i].y, card0.rect[i].w, card0.rect[i].h, card0.rect[i].r); \
        compo_cardbox_rect_set_color(cardbox, card0.rect[i].id, make_color(41,41,41)); \
    } \
    for (u8 i=0; i<icon_cnt; i++) { \
        compo_cardbox_icon_set_location(cardbox, card0.icon[i].id, card0.icon[i].x, card0.icon[i].y, card0.icon[i].w, card0.icon[i].h); \
        compo_cardbox_icon_set(cardbox, card0.icon[i].id, card0.icon[i].res); \
    } \
    for (u8 i=0; i<text_cnt; i++) { \
        compo_cardbox_text_set_font(cardbox, card0.text[i].id, card0.text[i].res); \
        compo_cardbox_text_set(cardbox, card0.text[i].id, card0.text[i].str); \
        if (card0.text[i].w == 0 || card0.text[i].h == 0) { \
            compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, true); \
            widget_text_set_ellipsis(cardbox->text[card0.text[i].id], false);   \
        } else { \
            compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, false); \
            widget_text_set_ellipsis(cardbox->text[card0.text[i].id], true);   \
        } \
        widget_set_align_center(cardbox->text[card0.text[i].id], card0.text[i].center); \
        widget_text_set_wordwrap(cardbox->text[card0.text[i].id], card0.text[i].wordwrap); \
        widget_text_set_color(cardbox->text[card0.text[i].id], make_color(card0.text[i].r, card0.text[i].g, card0.text[i].b)); \
        compo_cardbox_text_set_location(cardbox, card0.text[i].id, card0.text[i].x, card0.text[i].y, card0.text[i].w, card0.text[i].h); \
    }

#define CARD_RECT_CNT      1
#define CARD_ICON_CNT      1
#define CARD_TEXT_CNT      2
#define CARD_MSG_X                 6+228/2                     //中心点
#define CARD_MSG_START_Y           54+110/2
CARD_TYPEDEF_T(msg, CARD_RECT_CNT, CARD_ICON_CNT, CARD_TEXT_CNT);
const CARD_T(msg) card =
{
    CARD_MSG_X,
    CARD_MSG_START_Y,
    284,//GUI_SCREEN_WIDTH-12,
    116+50,//GUI_SCREEN_HEIGHT/3,

    {
        ///矩形背景
        //id      x       y         w        h      r
        {0,       0,      0,   284,    116,    20},
    },

    {
        ///图标
        //id      x                   y               w       h       res
        {0,       -(284/2-43),      -(116/2),      46,     46,     0},
    },

    {
        ///文本
        //id      x                 y       w       h       str                                res                         center  wordwrap   r  g  b
        {0,      5,              -48,    0,      0,      "2024-10-28 19:08",                UI_BUF_0FONT_FONT_BIN,      false,   false,  148, 148, 148},
        {1,      -(284/2-23),      -10,    250,    72,     "Hello! You are a good man ...",   UI_BUF_0FONT_FONT_BIN,     false,   true,   255, 255, 255},
    },
};


typedef struct f_message_t_
{
    bool flag_drag;
    s32 y_pos;  //上次松手y坐标
    s32 y;      //当前要对齐的y坐标

    s32 y_min;  //可拖动y轴最小范围
    s32 y_max;  //可拖动y轴最大范围

    bool flag_move_auto;
    u32 tick;

    s32 moveto_y;

    u8 debug_msg; //测试

    //message_info_t msg[MSG_MAX_LIST_CNT];
    ute_module_notify_data_t *ute_msg;

    char tmp_msg[UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+4];

} f_message_t;

static void func_message_card_update(bool fist, compo_form_t *frm);


//创建消息窗体
compo_form_t *func_message_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MESSAGE]);

    //创建消息卡片
    for (int i=0; i<MSG_MAX_LIST_CNT; i++)
    {
        u8 id = COMPO_ID_CARD_MSG1+i;
        CREAT_CARD(id, CARD_RECT_CNT, CARD_ICON_CNT, CARD_TEXT_CNT, card, card.w, card.h);
    }


    for (int i=0; i<MSG_BTN_CNT; i++)
    {
        //创建按钮
        compo_button_t* btn;
        if (message_btn[i].res)
        {
            btn = compo_button_create_by_image(frm, message_btn[i].res);
        }
        compo_button_set_pos(btn, message_btn[i].x, message_btn[i].y);
        compo_button_set_visible(btn, false);
        compo_setid(btn, message_btn[i].id);

        //创建删除按钮文本
        compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR_ALL]));
        compo_textbox_set_location(text, message_btn[i].x, message_btn[i].y, gui_image_get_size(message_btn[i].res).wid - gui_image_get_size(message_btn[i].res).hei, gui_image_get_size(message_btn[i].res).hei);
        compo_textbox_set(text, i18n[STR_CLEAR_ALL]);
        compo_textbox_set_visible(text, false);
        compo_setid(text, COMPO_ID_TXT_DEL);
    }


    //创建无消息界面
    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I332001_NOTIFICATION_NO_DATA_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_COVER_PIC);

    compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_NO_MSG]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 304, 150, 40);
    compo_textbox_set(txt, i18n[STR_NO_MSG]);
    compo_textbox_set_visible(txt, false);
    compo_setid(txt, COMPO_ID_COVER_TXT);

    if (func_cb.sta == FUNC_MESSAGE)
    {
        func_cb.frm_main = frm;
    }

//    if (func_cb.sta == FUNC_MESSAGE) {
    func_message_card_update(true, frm);
//    }

    return frm;
}

static void func_message_btn_click(void)
{
    f_message_t *f_msg = (f_message_t *)func_cb.f_cb;
    u8 id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_DEL:
            //清除消息
            uteModuleNotifyDelAllHistoryData(true);
            memset(f_msg->ute_msg, 0, sizeof(ute_module_notify_data_t));
//            f_msg->y_pos = 0;
//            f_msg->y = 0;
//            f_msg->y_max = 0;
//            card_y += (card.h+10);
//            f_msg->y_max = ;
            widget_page_set_client(func_cb.frm_main->page_body, 0, -(card.h + 10) / 4);
            break;
    }


}

//获取点击的卡片组件id
static u16 func_message_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<MSG_MAX_LIST_CNT; i++)
    {
        id = COMPO_ID_CARD_MSG1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

static void func_message_card_click(void)
{
    f_message_t *f_msg = (f_message_t *)func_cb.f_cb;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_message_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > CARD_TOTAL_CNT - 1)
    {
        return;
    }
//    printf("click compo_id:%d\n", compo_id);
    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        uteModuleNotifyGetData(f_msg->ute_msg);
        u16 msg_id = compo_id - COMPO_CARD_START - 1;
        char *msg = (char*)f_msg->ute_msg->historyNotify[msg_id].content;
        char time[30]= {0};

//        printf("===================================================>>>>>>>\n");
//        print_r(msg, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
//        printf("===================================================>>>>>>>\n");

        memset(f_msg->tmp_msg, '\0', sizeof(f_msg->tmp_msg));
        memcpy(f_msg->tmp_msg, msg, strlen(msg));
        //消息内容超过UTE_NOTIFY_MSG_CONTENT_MAX_SIZE补充省略号
        if (strlen(msg) >= UTE_NOTIFY_MSG_CONTENT_MAX_SIZE-2)
        {
            memset(&f_msg->tmp_msg[strlen(msg)], '.', 3);
            f_msg->tmp_msg[strlen(msg)+3] = '\0';
        }

//        printf("===================================================>>>>>>>\n");
//        print_r(f_msg->tmp_msg, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
//        printf("===================================================>>>>>>>\n");

        printf("msg[%d]:%s\n", strlen(msg), f_msg->tmp_msg);
//        sprintf(time, "%04d/%02d/%02d %02d:%02d", f_msg->ute_msg->historyNotify[msg_id].year, f_msg->ute_msg->historyNotify[msg_id].month,
//                f_msg->ute_msg->historyNotify[msg_id].day, f_msg->ute_msg->historyNotify[msg_id].hour, f_msg->ute_msg->historyNotify[msg_id].min);
        sprintf(time, "%04d/%02d/%02d", f_msg->ute_msg->historyNotify[msg_id].year, f_msg->ute_msg->historyNotify[msg_id].month,
                f_msg->ute_msg->historyNotify[msg_id].day);
        sys_cb.msg_index = f_msg->ute_msg->historyNotify[msg_id].type;
        int res = msgbox(f_msg->tmp_msg, NULL, time, MSGBOX_MODE_BTN_DELETE, MSGBOX_MSG_TYPE_DETAIL);
        if (res == MSGBOX_RES_DELETE)
        {
            uteModuleNotifySetDisplayIndex(msg_id);
            uteModuleNotifyDelAllHistoryData(false);
            uteModuleNotifyGetData(f_msg->ute_msg);

            if(compo_id == 1)
            {
                widget_page_set_client(func_cb.frm_main->page_body, 0, -(card.h + 10) / 4);
            }
        }
    }

}

static u32 func_message_card_get_icon(u8 type)
{
    if (type < func_cover_get_detail_msg_cnt())
    {
        return func_cover_get_detail_msg_ui(type);
    }
    return 0;
}

//更新消息卡片
static void func_message_card_update(bool first_update, compo_form_t *frm)
{
    f_message_t *f_msg = NULL;
    if (first_update == true && func_cb.sta != FUNC_MESSAGE)
    {
        f_msg = ab_zalloc(sizeof(f_message_t));
        f_msg->ute_msg = ab_zalloc(sizeof(ute_module_notify_data_t));
        if (f_msg->ute_msg == NULL)
        {
            printf("%s malloc err\n", __func__);
            halt(0x1010);
        }
    }
    else if (func_cb.sta == FUNC_MESSAGE)
    {
        f_msg = (f_message_t *)func_cb.f_cb;
    }
    //更新卡片
    uteModuleNotifyGetData(f_msg->ute_msg);
    s32 card_y = 0;
    for (int i=0; i<MSG_MAX_LIST_CNT; i++)
    {

        compo_cardbox_t* cardbox = compo_getobj_byid(COMPO_ID_CARD_MSG1+i);

        if (i <= uteModuleNotifyGetTotalNotifyCnt()-1)
        {

            char* time = (char*)func_zalloc(30);
            char* msg = (char*)f_msg->ute_msg->historyNotify[i].content;

            //memcpy(msg, f_msg->ute_msg->historyNotify[i].content, f_msg->ute_msg->historyNotify[i].size);
//            sprintf(time, "%04d/%02d/%02d %02d:%02d", f_msg->ute_msg->historyNotify[i].year, f_msg->ute_msg->historyNotify[i].month,
//                    f_msg->ute_msg->historyNotify[i].day, f_msg->ute_msg->historyNotify[i].hour, f_msg->ute_msg->historyNotify[i].min);
            sprintf(time, "%04d/%02d/%02d", f_msg->ute_msg->historyNotify[i].year, f_msg->ute_msg->historyNotify[i].month,
                    f_msg->ute_msg->historyNotify[i].day);
            card_y += (card.h+10);
            compo_cardbox_set_visible(cardbox, true);
            compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, card_y);
            compo_cardbox_icon_set(cardbox, 0, func_message_card_get_icon(f_msg->ute_msg->historyNotify[i].type));
            compo_cardbox_icon_set_location(cardbox, card.icon[0].id, card.icon[0].x, card.icon[0].y, card.icon[0].w, card.icon[0].h);
            compo_cardbox_text_set(cardbox, 0, time);
            compo_cardbox_text_set(cardbox, 1, msg);

            func_free(time);
        }
        else
        {
            compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, -GUI_SCREEN_HEIGHT);
            compo_cardbox_set_visible(cardbox, false);
        }
    }

    if (card_y)
    {

        //更新按钮
        s32 btn_y  = 0;
        for (int i=0; i<MSG_BTN_CNT; i++)
        {
            btn_y = card_y + (card.h + 10) / 2 + gui_image_get_size(message_btn[i].res).hei/2 + 10;
//            printf("btn_y [%d,%d]\n", btn_y, GUI_SCREEN_HEIGHT - gui_image_get_size(message_btn[i].res).hei/2);
            if (btn_y < GUI_SCREEN_HEIGHT - gui_image_get_size(message_btn[i].res).hei/2)   // 按钮位置小于屏幕底部特殊处理
            {
                btn_y = GUI_SCREEN_HEIGHT - gui_image_get_size(message_btn[i].res).hei/2;
            }

            compo_button_t* btn = compo_getobj_byid(COMPO_ID_BTN_DEL+i);
            compo_button_set_visible(btn, true);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, btn_y-8);

            compo_textbox_t* text = compo_getobj_byid(COMPO_ID_TXT_DEL+i);
            compo_textbox_set_visible(text, true);
            compo_textbox_set_pos(text, GUI_SCREEN_CENTER_X, btn_y-8);

        }

        //显示无消息页面
        compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_COVER_TXT);
        compo_textbox_set_visible(txt, false);
        compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_COVER_PIC);
        compo_picturebox_set_visible(pic, false);
//        if (first_update == false && func_cb.sta == FUNC_MESSAGE)
//        if (func_cb.sta == FUNC_MESSAGE)
//        {
        //更新拖动范围
        f_msg->y_min = -(btn_y - (GUI_SCREEN_HEIGHT - gui_image_get_size(message_btn[0].res).hei) + 32);
        f_msg->y_max = -65;//-(card.h + 10) / 2;
        if (btn_y + gui_image_get_size(message_btn[0].res).hei / 2 <= GUI_SCREEN_HEIGHT || uteModuleNotifyGetTotalNotifyCnt() == 1 || first_update == true)
        {
            //f_msg->flag_drag = false;
            f_msg->y_min = f_msg->y_max;
            if (func_cb.sta == FUNC_MESSAGE)
            {
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_msg->y_max);
            }
            else
            {
                if (frm != NULL)
                {
                    widget_page_set_client(frm->page_body, 0, f_msg->y_max);
                }
            }
        }
//        }
        //printf("drag=>[%d,%d] -> [%d,%d,%d]\n", f_msg->y_min, f_msg->y_max, btn_y, card_y, (GUI_SCREEN_HEIGHT - message_btn[0].w / 2));
    }
    else
    {
        //更新按钮
        for (int i=0; i<MSG_BTN_CNT; i++)
        {
            compo_button_t* btn = compo_getobj_byid(COMPO_ID_BTN_DEL+i);
            compo_button_set_visible(btn, false);
            compo_textbox_t* text = compo_getobj_byid(COMPO_ID_TXT_DEL+i);
            compo_textbox_set_visible(text, false);
        }

        //显示无消息页面
        compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_COVER_TXT);
        compo_textbox_set_visible(txt, true);
        compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_COVER_PIC);
        compo_picturebox_set_visible(pic, true);



        //更新拖动状态
        f_msg->flag_drag = false;
    }


    if (first_update == true && func_cb.sta != FUNC_MESSAGE)
    {
        if (f_msg != NULL)
        {
            if (f_msg->ute_msg != NULL)
            {
                ab_free(f_msg->ute_msg);
                f_msg->ute_msg = NULL;
            }
            ab_free(f_msg);
            f_msg = NULL;
        }
    }

}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//消息功能事件处理
static void func_message_process(void)
{
    f_message_t *f_msg = (f_message_t *)func_cb.f_cb;

    func_message_card_update(false, NULL);

    s32 dx,dy;
    //开始拖动页面
    if (f_msg->flag_drag)
    {
        f_msg->flag_drag = ctp_get_dxy(&dx, &dy);
        if (f_msg->flag_drag)
        {
            f_msg->y = f_msg->y_pos + dy;

            if (f_msg->y > f_msg->y_max + 20)
            {
                f_msg->y = f_msg->y_max + 20;
            }
            else if (f_msg->y < f_msg->y_min - 20)
            {
                f_msg->y = f_msg->y_min - 20;
            }

            //printf("drag=>[%d,%d] -> %d\n", f_msg->y_min, f_msg->y_max, f_msg->y);
            widget_page_set_client(func_cb.frm_main->page_body, 0, f_msg->y);
        }
        else
        {
            //抬手
            f_msg->y_pos = f_msg->y;
            s16 last_dy = ctp_get_last_dxy().y;
            s16 to_y = f_msg->y_pos + (last_dy * 16);
            f_msg->moveto_y = to_y;
            if (f_msg->moveto_y < f_msg->y_min - 20)
            {
                f_msg->moveto_y = f_msg->y_min - 20;
            }
            else if (f_msg->moveto_y > f_msg->y_max + 20)
            {
                f_msg->moveto_y = f_msg->y_max + 20;
            }
            f_msg->flag_move_auto = true;
            f_msg->tick = tick_get();
        }
    }
    else if (f_msg->flag_move_auto)
    {
        if (f_msg->y_pos == f_msg->moveto_y)
        {
            //printf("=>moveto over\n");
            if (f_msg->y_pos < f_msg->y_min)            //回弹
            {
                f_msg->moveto_y = f_msg->y_min;
            }
            else if (f_msg->y_pos > f_msg->y_max)
            {
                f_msg->moveto_y = f_msg->y_max;
            }
            else
            {
                f_msg->flag_move_auto = false;            //移动完成
            }
        }
        else if (tick_check_expire(f_msg->tick, 18))
        {
            s16 dy;
            f_msg->tick = tick_get();
            dy = f_msg->moveto_y - f_msg->y_pos;
            if (dy > 0)
            {
                if (dy > 16 * 6)
                {
                    dy = dy / 6;
                }
                else if (dy > 16)
                {
                    dy = 16;
                }
                else
                {
                    dy = 1;
                }
            }
            else if (dy < 0)
            {
                if (dy < -16 * 6)
                {
                    dy = dy / 6;
                }
                else if (dy < -16)
                {
                    dy = -16;
                }
                else
                {
                    dy = -1;
                }
            }
            f_msg->y_pos +=dy;
            widget_page_set_client(func_cb.frm_main->page_body, 0, f_msg->y_pos);
//            printf("moveto -> f_msg->y_pos = %d, %d\n", f_msg->y_pos, widget_text_get_max_height());
        }
    }

    func_process();
}

//消息功能消息处理
static void func_message_message(size_msg_t msg)
{
    f_message_t *f_msg = (f_message_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_message_btn_click();
            func_message_card_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
            f_msg->flag_drag = true;
            f_msg->flag_move_auto = false;
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入消息功能
static void func_message_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_message_t));
    f_message_t *f_msg = (f_message_t *)func_cb.f_cb;
    f_msg->ute_msg = ab_zalloc(sizeof(ute_module_notify_data_t));
    if (f_msg->ute_msg == NULL)
    {
        printf("%s malloc err\n", __func__);
        halt(0x1010);
    }
    func_cb.frm_main = func_message_form_create();
}

//退出消息功能
static void func_message_exit(void)
{
    f_message_t *f_msg = (f_message_t *)func_cb.f_cb;
    if (f_msg->ute_msg != NULL)
    {
        ab_free(f_msg->ute_msg);
        f_msg->ute_msg = NULL;
    }
    if (func_cb.left_sta == FUNC_MESSAGE)
    {
        if (sys_cb.refresh_language_flag == false)
        {
            func_cb.last = FUNC_MESSAGE;
            func_cb.left_sta = FUNC_NULL;
            task_stack_init();  //任务堆栈
            latest_task_init(); //最近任务
        }
    }
    else
    {
        func_cb.last = FUNC_MESSAGE;
        func_cb.left_sta = FUNC_NULL;
    }



}

//消息功能
void func_message_info(void)
{
    printf("%s\n", __func__);
    func_message_enter();
    while (func_cb.sta == FUNC_MESSAGE)
    {
        func_message_process();
        func_message_message(msg_dequeue());
    }
    func_message_exit();
}


#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
