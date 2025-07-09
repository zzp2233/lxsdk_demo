#ifndef _FUNC_COVER_H
#define _FUNC_COVER_H

///闹钟蓝牙连接提醒等简单界面弹窗
enum
{
    REMIND_COVER_NONE,
    //这个顺序更改,需要同步到func_cover.h那边
    REMIND_COVER_ALARM,
    //这一段HEALTH按顺序,需要与analysis那边一致
    REMIND_COVER_HEALTH_SEDENTARY,
    REMIND_COVER_HEALTH_DRINK,

    REMIND_COVER_FIND_WATCH,
    REMIND_COVER_GOAL,
    REMIND_GCOVER_BT_NOT_CONNECT,
    REMIND_GCOVER_BT_CONNECT,
    REMIND_GCOVER_APP_CONNECT,

    REMIND_COVER_LOW_BATTERY,    //低电提醒
    REMIND_COVER_TIMER_FINISH,    //秒表计时结束
    REMIND_COVER_TARGET_SMOKE_FINISH,    //目标口数达成
};

/////消息弹窗界面
//enum {
//    MSG_COVER_NULL,         //无
//    MSG_COVER_WECHAT,       //微信消息
//    MSG_COVER_QQ,           //QQ消息
//    MSG_COVER_SMS,          //手机短信
//    MSG_COVER_OTHER,        //其他消息
//
//    MSG_COVER_ALL,
//};

typedef struct f_cover_detail_msg_item_t_
{
    u32 res_addr;
    s16 pic_y;
    s16 txt_y;
    s16 title_y;
    s16 time_txt_y;
} f_cover_detail_msg_item_t;

typedef struct f_cover_brief_msg_item_t_
{
    u32 res_addr;
    s16 pic_y;
    s16 txt_y;
    s16 title_y;
    s16 time_txt_y;
} f_cover_brief_msg_item_t;

void gui_set_cover_index(uint8_t index);
void gui_cover_msg_enqueue(uint8_t index);

u32 func_cover_get_pic_res_addr(u8 type);
u16 func_cover_get_str_idx(u8 type);
s16 func_cover_get_pic_y(u8 type);
s16 func_cover_get_txt_y(u8 type);
s16 func_cover_get_title_txt_y(u8 type);
s16 func_cover_get_time_txt_y(u8 type);

u32 func_cover_get_detail_msg_cnt(void);
u32 func_cover_get_detail_msg_ui(u8 index);
#endif // _FUNC_COVER_H
