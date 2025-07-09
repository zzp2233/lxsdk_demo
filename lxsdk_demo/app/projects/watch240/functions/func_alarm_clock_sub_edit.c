#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if (USE_APP_TYPE == USE_AB_APP)
#define ALARM_ENABLE_CNT()              alarm_num_get()
#define ALARM_DELETE(idx)               alarm_clock_delete(idx)
#define ALARM_GET_HOUR(idx)             app_data.sector0.alarm[idx].hour
#define ALARM_GET_MIN(idx)              app_data.sector0.alarm[idx].minute
#define ALARM_GET_CYCLE(idx)            app_data.sector0.alarm[idx].cycle
#elif (USE_APP_TYPE == USE_UTE_APP)
#define ALARM_ENABLE_CNT()              uteModuleSystemtimeGetAlarmTotalCnt()
#define ALARM_DELETE(idx)               uteModuleSystemtimeDeleteAlarm(idx)
#define ALARM_GET_HOUR(idx)             uteModuleSystemtimeGetAlarmHour(idx)
#define ALARM_GET_MIN(idx)              uteModuleSystemtimeGetAlarmMin(idx)
#define ALARM_GET_CYCLE(idx)            uteModuleSystemtimeGetAlarmCycle(idx)
#else
#define ALARM_ENABLE_CNT()              0
#define ALARM_DELETE(idx)
#define ALARM_GET_HOUR(idx)             0
#define ALARM_GET_MIN(idx)              0
#define ALARM_GET_CYCLE(idx)            0
#endif


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
static void func_alarm_clock_sub_edit_enter(void);
static void func_alarm_clock_sub_edit_exit(void);

typedef struct f_alarm_clock_sub_edit_t_
{
    bool time_scale;        //时间制 0:24小时; 1:12小时
} f_alarm_clock_sub_edit_t;

typedef struct func_alarm_hour_format_t_
{
    u8 hour;
    u8 am_pm;
} func_alarm_hour_format_t;

static func_alarm_hour_format_t func_alarm_convert_to_12hour(s8 hour24)
{
    u8 am_pm = (hour24 >= 12) ? 2 : 1;    //2 PM, 1 AM
    func_alarm_hour_format_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (hour24 == 0)
        {
            hour12.hour = 12;
        }
        else if (hour24 > 12)
        {
            hour12.hour = hour24 - 12;
        }
        else
        {
            hour12.hour = hour24;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = hour24;
    hour12.am_pm = 0;
    return hour12;
}

typedef struct ui_handle_t_
{

    ///删除按钮
    struct btn_t
    {
        u16 id;
        u16 id_click;
        s16 x,y;
        u16 w,h;
        u32 res;
        u32 res_click;
    } del_btn;

    ///删除按钮文本
    struct btn_txt_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;
        u16 str_id;
        color_t color;
    } del_btn_txt;

    ///时间设置卡片
    struct card_time_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t
        {
            u8 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
        } rect[1];

//        struct card_icon_t {
//            u8 idx;
//            s16 x,y;
//            u16 w,h;
//            u16 res;
//        } icon[0];

        struct card_text_t
        {
            u8 idx;
            s16 x,y;
            u16 w,h;
            u16 str_id;
            u32 res;
            bool center;
            bool wordwrap;
            color_t color;
            u16 rev;
        } text[3];
    } card_time;

    ///日期设置
    struct card_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t rect[1];

//        struct card_icon_t {
//            u8 idx;
//            s16 x,y;
//            u16 w,h;
//            u16 res;
//        } icon[0];

        struct card_text_t text[2];
    } card_day;

} ui_handle_t;

///组件ID
enum
{
    //按键
    COMPO_ID_BTN_DEL = 1,

    //图像
    COMPO_ID_PIC_DEL_CLICK,

    //卡片
    CARD_ID_START,
    COMPO_ID_CARD_SET_TIME,
    COMPO_ID_CARD_SET_DAY,
    CARD_ID_END,

    //文本
    COMPO_ID_TEXT_BTN_DEL,W
};

static const ui_handle_t ui_handle =
{
    ///删除按钮
    .del_btn = {
        .id     = COMPO_ID_BTN_DEL,
        .id_click   = COMPO_ID_PIC_DEL_CLICK,
        .x      = 14+212/2,
        .y      = 216+62/2,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN,
        .res_click = 0,
    },

    ///删除按钮文本
    .del_btn_txt = {
        .id     = COMPO_ID_TEXT_BTN_DEL,
        .x      = 14+212/2,
        .y      = 216+62/2,
        .w      = 0,
        .h      = 0,
        .str_id = STR_DEL,
        .color = {254, 59, 48},
    },

    ///时间设置卡片
    .card_time = {
        .id     = COMPO_ID_CARD_SET_TIME,
        .x      = 4 + 232/2,
        .y      = 54 + 72/2,
        .w      = 232,
        .h      = 72,

        .rect  = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 0,
                .w      = 232,
                .h      = 72,
                .r      = 16,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 10 - 232/2,
                .y      = 5 - 72/2,
                .w      = 70,
                .h      = 34,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_NUM_22_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
            },

            [1] = {
                .idx    = 1,
                .x      = 10 - 232/2,
                .y      = 40 - 72/2,
                .w      = 200,
                .h      = 26,
                .str_id = STR_CUSTOM_TIME,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
            },

            [2] = {
                .idx    = 2,
                .x      = 80 - 232/2,
                .y      = 5 - 72/2,
                .w      = 100,
                .h      = 34,
                .str_id = STR_AM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = STR_PM,
            },
        },
    },

    ///日期设置
    .card_day = {
        .id     = COMPO_ID_CARD_SET_DAY,
        .x      = 4 + 232/2,
        .y      = 132 + 72/2,
        .w      = 232,
        .h      = 72,

        .rect  = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 0,
                .w      = 232,
                .h      = 72,
                .r      = 16,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 10 - 232/2,
                .y      = 10 - 72/2,
                .w      = 200,
                .h      = 26,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
            },

            [1] = {
                .idx    = 1,
                .x      = 10 - 232/2,
                .y      = 40 - 72/2,
                .w      = 200,
                .h      = 26,
                .str_id = STR_ALARM_CLOCK_REPEAT,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
            },
        },
    },

};

//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_sub_edit_form_create(void)
{

    int buf_num=0;
    int str_week_buf[7]=
    {
        STR_MONDAY, // 周一
        STR_TUESDAY, // 周二
        STR_WEDNESDAY, // 周三
        STR_THURSDAY, // 周四
        STR_FRIDAY, // 周五
        STR_SATURDAY, // 周六
        STR_SUNDAY, // 周日
    };

    char *str_buff = NULL;
    uint16_t str_buff_size = 0;

    for (uint8_t i = 0; i < 7; i++)
    {
        str_buff_size += strlen(i18n[str_week_buf[i]]) + 2;
    }

    if(str_buff_size < MAX(strlen(i18n[STR_ONCE]) + 2, strlen(i18n[STR_EVERY_DAY]) + 2))
    {
        str_buff_size = MAX(strlen(i18n[STR_ONCE]) + 2, strlen(i18n[STR_EVERY_DAY]) + 2);
    }

    str_buff = (char *)uteModulePlatformMemoryAlloc(str_buff_size);

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_EDIT]);

    //DEL按钮
    compo_button_t * btn = compo_button_create_by_image(frm, ui_handle.del_btn.res);
    compo_setid(btn, ui_handle.del_btn.id);
    compo_button_set_pos(btn, ui_handle.del_btn.x, ui_handle.del_btn.y);

    //DEL按钮点击图片
    compo_picturebox_t* btn_pic_click = compo_picturebox_create(frm, ui_handle.del_btn.res_click);
    compo_setid(btn_pic_click,  ui_handle.del_btn.id_click);
    compo_picturebox_set_pos(btn_pic_click, ui_handle.del_btn.x, ui_handle.del_btn.y);
    compo_picturebox_set_visible(btn_pic_click, false);

    //DEL按钮TXT
    compo_textbox_t* btn_txt = compo_textbox_create(frm, strlen(i18n[ui_handle.del_btn_txt.str_id]));
    compo_textbox_set_location(btn_txt, ui_handle.del_btn_txt.x, ui_handle.del_btn_txt.y,
                               gui_image_get_size(ui_handle.del_btn.res).wid - gui_image_get_size(ui_handle.del_btn.res).hei,
                               gui_image_get_size(ui_handle.del_btn.res).hei);
    widget_text_set_color(btn_txt->txt, make_color(ui_handle.del_btn_txt.color.r, ui_handle.del_btn_txt.color.g, ui_handle.del_btn_txt.color.b));
    compo_textbox_set(btn_txt, i18n[ui_handle.del_btn_txt.str_id]);
    compo_setid(btn_txt, ui_handle.del_btn_txt.id);



    //时间卡片
    compo_cardbox_t* card_time = compo_cardbox_create(frm,
                                 sizeof(ui_handle.card_time.rect)/sizeof(ui_handle.card_time.rect[0]),
                                 0,
                                 sizeof(ui_handle.card_time.text)/sizeof(ui_handle.card_time.text[0]),
                                 ui_handle.card_time.w, ui_handle.card_time.h);
    compo_cardbox_set_visible(card_time, true);
    compo_cardbox_set_location(card_time, ui_handle.card_time.x, ui_handle.card_time.y, ui_handle.card_time.w, ui_handle.card_time.h);
    compo_setid(card_time, ui_handle.card_time.id);
    for (u8 i=0; i<sizeof(ui_handle.card_time.rect)/sizeof(ui_handle.card_time.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(card_time, ui_handle.card_time.rect[i].idx, ui_handle.card_time.rect[i].x, ui_handle.card_time.rect[i].y,
                                        ui_handle.card_time.rect[i].w, ui_handle.card_time.rect[i].h, ui_handle.card_time.rect[i].r);
        compo_cardbox_rect_set_color(card_time, ui_handle.card_time.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.card_time.text)/sizeof(ui_handle.card_time.text[0]); i++)
    {
        compo_cardbox_text_set_font(card_time, ui_handle.card_time.text[i].idx, ui_handle.card_time.text[i].res);
        widget_text_set_ellipsis(card_time->text[ui_handle.card_time.text[i].idx], false);
        compo_cardbox_text_set_align_center(card_time, ui_handle.card_time.text[i].idx, ui_handle.card_time.text[i].center);
        widget_text_set_wordwrap(card_time->text[ui_handle.card_time.text[i].idx], ui_handle.card_time.text[i].wordwrap);
        widget_text_set_color(card_time->text[ui_handle.card_time.text[i].idx], make_color(ui_handle.card_time.text[i].color.r, ui_handle.card_time.text[i].color.g, ui_handle.card_time.text[i].color.b));
        compo_cardbox_text_set_location(card_time, ui_handle.card_time.text[i].idx, ui_handle.card_time.text[i].x, ui_handle.card_time.text[i].y,
                                        ui_handle.card_time.text[i].w, ui_handle.card_time.text[i].h);

        u8 hour = ALARM_GET_HOUR(sys_cb.alarm_edit_idx);
        u8 min = ALARM_GET_MIN(sys_cb.alarm_edit_idx);
        u8 am_pm = 0;
        func_alarm_hour_format_t hour_cov = func_alarm_convert_to_12hour(hour);
        hour = hour_cov.hour;
        am_pm = hour_cov.am_pm;
        char aclock_str[20] = {0};
        memset(aclock_str, '\0', sizeof(aclock_str));
        sprintf(aclock_str, "%02d:%02d", hour, min);
        if (ui_handle.card_time.text[i].str_id == STR_AM && ui_handle.card_time.text[i].rev == STR_PM && ui_handle.card_time.text[i].idx == 2)
        {

            if(am_pm == 1)      //AM
            {
                compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, i18n[ui_handle.card_time.text[i].str_id]);
            }
            else if (am_pm == 2)
            {
                compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, i18n[ui_handle.card_time.text[i].rev]);
            }
            else
            {
//                compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, i18n[ui_handle.card_time.text[i].rev]);
            }
        }
        else if (ui_handle.card_time.text[i].str_id == STR_NULL && ui_handle.card_time.text[i].idx == 0)
        {
            compo_cardbox_text_set_font(card_time, ui_handle.card_time.text[i].idx, UI_BUF_0FONT_FONT_NUM_28_BIN);
            compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, aclock_str);
        }
        else if (ui_handle.card_time.text[i].str_id != STR_NULL && ui_handle.card_time.text[i].idx == 1)
        {
            compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, i18n[ui_handle.card_time.text[i].str_id]);
        }
    }

    //日期卡片
    compo_cardbox_t* card_day = compo_cardbox_create(frm,
                                sizeof(ui_handle.card_day.rect)/sizeof(ui_handle.card_day.rect[0]),
                                0,
                                sizeof(ui_handle.card_day.text)/sizeof(ui_handle.card_day.text[0]),
                                ui_handle.card_day.w, ui_handle.card_day.h);
    compo_cardbox_set_visible(card_day, true);
    compo_cardbox_set_location(card_day, ui_handle.card_day.x, ui_handle.card_day.y, ui_handle.card_day.w, ui_handle.card_day.h);
    compo_setid(card_day, ui_handle.card_day.id);
    for (u8 i=0; i<sizeof(ui_handle.card_day.rect)/sizeof(ui_handle.card_day.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(card_day, ui_handle.card_day.rect[i].idx, ui_handle.card_day.rect[i].x, ui_handle.card_day.rect[i].y,
                                        ui_handle.card_day.rect[i].w, ui_handle.card_day.rect[i].h, ui_handle.card_day.rect[i].r);
        compo_cardbox_rect_set_color(card_day, ui_handle.card_day.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.card_day.text)/sizeof(ui_handle.card_day.text[0]); i++)
    {
        compo_cardbox_text_set_font(card_day, ui_handle.card_day.text[i].idx, ui_handle.card_day.text[i].res);
        widget_text_set_ellipsis(card_day->text[ui_handle.card_day.text[i].idx], false);
        compo_cardbox_text_set_align_center(card_day, ui_handle.card_day.text[i].idx, ui_handle.card_day.text[i].center);
        widget_text_set_wordwrap(card_day->text[ui_handle.card_day.text[i].idx], ui_handle.card_day.text[i].wordwrap);
        widget_text_set_color(card_day->text[ui_handle.card_day.text[i].idx], make_color(ui_handle.card_day.text[i].color.r, ui_handle.card_day.text[i].color.g, ui_handle.card_day.text[i].color.b));
        compo_cardbox_text_set_location(card_day, ui_handle.card_day.text[i].idx, ui_handle.card_day.text[i].x, ui_handle.card_day.text[i].y,
                                        ui_handle.card_day.text[i].w, ui_handle.card_day.text[i].h);

        memset(str_buff, '\0', str_buff_size);
        if (ALARM_GET_CYCLE(sys_cb.alarm_edit_idx) & BIT(7))
        {
            snprintf(str_buff, str_buff_size, i18n[STR_ONCE]);
        }
        else if (ALARM_GET_CYCLE(sys_cb.alarm_edit_idx) == 0x7f)
        {
            snprintf(str_buff,str_buff_size, i18n[STR_EVERY_DAY]);
        }
        else
        {

            for (u8 j=0; j<7; j++)
            {
                if (ALARM_GET_CYCLE(sys_cb.alarm_edit_idx) & BIT(j))
                {
                    const char *week_str = i18n[str_week_buf[j]];
                    uint8_t week_str_len = strlen(week_str);
                    if (buf_num + week_str_len + 1 <= str_buff_size)
                    {
                        memcpy(&str_buff[buf_num], week_str, week_str_len);
                        buf_num += week_str_len;
                        str_buff[buf_num++] = ' ';
                    }
                }
            }
        }

        if (ui_handle.card_day.text[i].idx == 0)        //周 1 2 3 4 5 6 7
        {
            compo_textbox_t *textbox = compo_textbox_create_for_page(frm,card_day->page,strlen(str_buff));
            compo_textbox_set_align_center(textbox, false);
            compo_textbox_set_location(textbox, ui_handle.card_day.text[i].x, ui_handle.card_day.text[i].y,ui_handle.card_day.text[i].w, ui_handle.card_day.text[i].h);
            compo_textbox_set(textbox,str_buff);
            compo_textbox_set_forecolor(textbox, make_color(ui_handle.card_day.text[i].color.r, ui_handle.card_day.text[i].color.g, ui_handle.card_day.text[i].color.b));
            // compo_cardbox_text_set(card_day, ui_handle.card_day.text[i].idx, str_buff);
        }
        else if (ui_handle.card_day.text[i].idx == 1)      //重复
        {
            compo_cardbox_text_set(card_day, ui_handle.card_day.text[i].idx, i18n[ui_handle.card_day.text[i].str_id]);
        }
    }

    uteModulePlatformMemoryFree(str_buff);
    return frm;
}

//触摸按钮效果处理
static void func_alarm_clock_sub_edit_button_touch_handle(void)
{

    int id = compo_get_button_id();
    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_DEL_CLICK);

    switch (id)
    {
        case COMPO_ID_BTN_DEL:
            compo_picturebox_set_visible(pic_click, true);
            break;

        default:
            break;
    }

}

//释放按钮效果处理
static void func_alarm_clock_sub_edit_button_release_handle(void)
{
    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_DEL_CLICK);
    compo_picturebox_set_visible(pic_click, false);
}

//获取点击卡片的id
static u16 func_alarm_clcok_sub_edit_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_ID_END-CARD_ID_START-1; i++)
    {
        id = CARD_ID_START + 1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (compo_cardbox_get_visible(cardbox) && abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//单击按钮
static void func_alarm_clock_sub_edit_card_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_DEL:
            ALARM_DELETE(sys_cb.alarm_edit_idx);
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
            func_cb.sta = FUNC_ALARM_CLOCK;
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
            break;

//        case COMPO_ID_BTN_SET:
//            func_cb.sta = FUNC_ALARM_CLOCK_SUB_SET;
//            break;
//
//        case COMPO_ID_BTN_REPEAT:
//            func_cb.sta = FUNC_ALARM_CLOCK_SUB_REPEAT;
//            break;

        default:
        {
            point_t pt = ctp_get_sxy();
            u16 compo_id = func_alarm_clcok_sub_edit_card_get_btn_id(pt);
            if (compo_id <= 0 || compo_id > CARD_ID_END-1)
            {
                return;
            }
            printf("click compo_id:%d\n", compo_id);

            compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
            if (compo_cardbox_get_visible(cardbox))
            {
                if (compo_id == ui_handle.card_time.id)
                {
                    func_cb.sta = FUNC_ALARM_CLOCK_SUB_SET;
                }
                else if (compo_id == ui_handle.card_day.id)
                {
                    func_cb.sta = FUNC_ALARM_CLOCK_SUB_REPEAT;
                }
            }
        }
        break;
    }

    func_alarm_clock_sub_edit_button_release_handle();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
static void func_alarm_clock_sub_edit_enter(void);
static void func_alarm_clock_sub_edit_exit(void);

typedef struct f_alarm_clock_sub_edit_t_
{
    bool time_scale;        //时间制 0:24小时; 1:12小时
    page_tp_move_t *ptm;
} f_alarm_clock_sub_edit_t;

typedef struct func_alarm_hour_format_t_
{
    u8 hour;
    u8 am_pm;
} func_alarm_hour_format_t;

static func_alarm_hour_format_t func_alarm_convert_to_12hour(s8 hour24)
{
    u8 am_pm = (hour24 >= 12) ? 2 : 1;    //2 PM, 1 AM
    func_alarm_hour_format_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (hour24 == 0)
        {
            hour12.hour = 12;
        }
        else if (hour24 > 12)
        {
            hour12.hour = hour24 - 12;
        }
        else
        {
            hour12.hour = hour24;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = hour24;
    hour12.am_pm = 0;
    return hour12;
}

typedef struct ui_handle_t_
{

    ///删除按钮
    struct btn_t
    {
        u16 id;
        u16 id_click;
        s16 x,y;
        u16 w,h;
        u32 res;
        u32 res_click;
    } del_btn;

    ///删除按钮文本
    struct btn_txt_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;
        u16 str_id;
        color_t color;
    } del_btn_txt;

    ///时间设置卡片
    struct card_time_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t
        {
            u8 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
        } rect[1];

//        struct card_icon_t {
//            u8 idx;
//            s16 x,y;
//            u16 w,h;
//            u16 res;
//        } icon[0];

        struct card_text_t
        {
            u8 idx;
            s16 x,y;
            u16 w,h;
            u16 str_id;
            u32 res;
            bool center;
            bool wordwrap;
            color_t color;
            u16 rev;
        } text[3];
    } card_time;

    ///日期设置
    struct card_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t rect[1];

//        struct card_icon_t {
//            u8 idx;
//            s16 x,y;
//            u16 w,h;
//            u16 res;
//        } icon[0];

        struct card_text_t text[2];
    } card_day;

} ui_handle_t;

///组件ID
enum
{
    //按键
    COMPO_ID_BTN_DEL = 1,

    //图像
    COMPO_ID_PIC_DEL_CLICK,

    //卡片
    CARD_ID_START,
    COMPO_ID_CARD_SET_TIME,
    COMPO_ID_CARD_SET_DAY,
    CARD_ID_END,

    //文本
    COMPO_ID_TEXT_BTN_DEL,W
};

static const ui_handle_t ui_handle =
{
    ///删除按钮
    .del_btn = {
        .id     = COMPO_ID_BTN_DEL,
        .id_click   = COMPO_ID_PIC_DEL_CLICK,
        .x      = 72+216/2,
        .y      = 324+64/2-50,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN,
        .res_click = 0,
    },

    ///删除按钮文本
    .del_btn_txt = {
        .id     = COMPO_ID_TEXT_BTN_DEL,
        .x      = 72+216/2,
        .y      = 324+64/2-50,
        .w      = 0,
        .h      = 0,
        .str_id = STR_DEL,
        .color = {254, 59, 48},
    },

    ///时间设置卡片
    .card_time = {
        .id     = COMPO_ID_CARD_SET_TIME,
        .x      = 18 + 324/2,
        .y      = 102 + 90/2-50,
        .w      = 324,
        .h      = 90,

        .rect  = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 0,
                .w      = 324,
                .h      = 90,
                .r      = 16,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 10 - 232/2-30,
                .y      = 5 - 72/2,
                .w      = 70,
                .h      = 34,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_NUM_22_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
            },

            [1] = {
                .idx    = 1,
                .x      = 10 - 232/2-30,
                .y      = 40 - 72/2,
                .w      = 200,
                .h      = 26,
                .str_id = STR_CUSTOM_TIME,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
            },

            [2] = {
                .idx    = 2,
                .x      = 80 - 232/2-30,
                .y      = 5 - 72/2,
                .w      = 100,
                .h      = 34,
                .str_id = STR_AM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = STR_PM,
            },
        },
    },

    ///日期设置
    .card_day = {
        .id     = COMPO_ID_CARD_SET_DAY,
        .x      = 18 + 324/2,
        .y      = 202 + 90/2-50,
        .w      = 324,
        .h      = 90,

        .rect  = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 0,
                .w      = 324,
                .h      = 90,
                .r      = 16,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 10 - 232/2-30,
                .y      = 10 - 72/2,
                .w      = 200,
                .h      = 26,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
            },

            [1] = {
                .idx    = 1,
                .x      = 10 - 232/2-30,
                .y      = 40 - 72/2,
                .w      = 200,
                .h      = 26,
                .str_id = STR_ALARM_CLOCK_REPEAT,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
            },
        },
    },

};

//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_sub_edit_form_create(void)
{

    int buf_num=0;
    int str_week_buf[7]=
    {
        STR_MONDAY, // 周一
        STR_TUESDAY, // 周二
        STR_WEDNESDAY, // 周三
        STR_THURSDAY, // 周四
        STR_FRIDAY, // 周五
        STR_SATURDAY, // 周六
        STR_SUNDAY, // 周日
    };

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_EDIT]);

    //DEL按钮
    compo_button_t * btn = compo_button_create_by_image(frm, ui_handle.del_btn.res);
    compo_setid(btn, ui_handle.del_btn.id);
    compo_button_set_pos(btn, ui_handle.del_btn.x, ui_handle.del_btn.y);

    //DEL按钮点击图片
    compo_picturebox_t* btn_pic_click = compo_picturebox_create(frm, ui_handle.del_btn.res_click);
    compo_setid(btn_pic_click,  ui_handle.del_btn.id_click);
    compo_picturebox_set_pos(btn_pic_click, ui_handle.del_btn.x, ui_handle.del_btn.y);
    compo_picturebox_set_visible(btn_pic_click, false);

    //DEL按钮TXT
    compo_textbox_t* btn_txt = compo_textbox_create(frm, strlen(i18n[ui_handle.del_btn_txt.str_id]));
    compo_textbox_set_location(btn_txt, ui_handle.del_btn_txt.x, ui_handle.del_btn_txt.y,
                               gui_image_get_size(ui_handle.del_btn.res).wid - gui_image_get_size(ui_handle.del_btn.res).hei,
                               gui_image_get_size(ui_handle.del_btn.res).hei);
    widget_text_set_color(btn_txt->txt, make_color(ui_handle.del_btn_txt.color.r, ui_handle.del_btn_txt.color.g, ui_handle.del_btn_txt.color.b));
    compo_textbox_set(btn_txt, i18n[ui_handle.del_btn_txt.str_id]);
    compo_setid(btn_txt, ui_handle.del_btn_txt.id);



    //时间卡片
    compo_cardbox_t* card_time = compo_cardbox_create(frm,
                                 sizeof(ui_handle.card_time.rect)/sizeof(ui_handle.card_time.rect[0]),
                                 0,
                                 sizeof(ui_handle.card_time.text)/sizeof(ui_handle.card_time.text[0]),
                                 ui_handle.card_time.w, ui_handle.card_time.h);
    compo_cardbox_set_visible(card_time, true);
    compo_cardbox_set_location(card_time, ui_handle.card_time.x, ui_handle.card_time.y, ui_handle.card_time.w, ui_handle.card_time.h);
    compo_setid(card_time, ui_handle.card_time.id);
    for (u8 i=0; i<sizeof(ui_handle.card_time.rect)/sizeof(ui_handle.card_time.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(card_time, ui_handle.card_time.rect[i].idx, ui_handle.card_time.rect[i].x, ui_handle.card_time.rect[i].y,
                                        ui_handle.card_time.rect[i].w, ui_handle.card_time.rect[i].h, ui_handle.card_time.rect[i].r);
        compo_cardbox_rect_set_color(card_time, ui_handle.card_time.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.card_time.text)/sizeof(ui_handle.card_time.text[0]); i++)
    {
        compo_cardbox_text_set_font(card_time, ui_handle.card_time.text[i].idx, ui_handle.card_time.text[i].res);
        widget_text_set_ellipsis(card_time->text[ui_handle.card_time.text[i].idx], false);
        compo_cardbox_text_set_align_center(card_time, ui_handle.card_time.text[i].idx, ui_handle.card_time.text[i].center);
        widget_text_set_wordwrap(card_time->text[ui_handle.card_time.text[i].idx], ui_handle.card_time.text[i].wordwrap);
        widget_text_set_color(card_time->text[ui_handle.card_time.text[i].idx], make_color(ui_handle.card_time.text[i].color.r, ui_handle.card_time.text[i].color.g, ui_handle.card_time.text[i].color.b));
        compo_cardbox_text_set_location(card_time, ui_handle.card_time.text[i].idx, ui_handle.card_time.text[i].x, ui_handle.card_time.text[i].y,
                                        ui_handle.card_time.text[i].w, ui_handle.card_time.text[i].h);

        u8 hour = ALARM_GET_HOUR(sys_cb.alarm_edit_idx);
        u8 min = ALARM_GET_MIN(sys_cb.alarm_edit_idx);
        u8 am_pm = 0;
        func_alarm_hour_format_t hour_cov = func_alarm_convert_to_12hour(hour);
        hour = hour_cov.hour;
        am_pm = hour_cov.am_pm;
        char aclock_str[20] = {0};
        memset(aclock_str, '\0', sizeof(aclock_str));
        sprintf(aclock_str, "%02d:%02d", hour, min);
        if (ui_handle.card_time.text[i].str_id == STR_AM && ui_handle.card_time.text[i].rev == STR_PM && ui_handle.card_time.text[i].idx == 2)
        {

            if(am_pm == 1)      //AM
            {
                compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, i18n[ui_handle.card_time.text[i].str_id]);
            }
            else if (am_pm == 2)
            {
                compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, i18n[ui_handle.card_time.text[i].rev]);
            }
            else
            {
//                compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, i18n[ui_handle.card_time.text[i].rev]);
            }
        }
        else if (ui_handle.card_time.text[i].str_id == STR_NULL && ui_handle.card_time.text[i].idx == 0)
        {
            compo_cardbox_text_set_font(card_time, ui_handle.card_time.text[i].idx, UI_BUF_0FONT_FONT_NUM_28_BIN);
            compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, aclock_str);
        }
        else if (ui_handle.card_time.text[i].str_id != STR_NULL && ui_handle.card_time.text[i].idx == 1)
        {
            compo_cardbox_text_set(card_time, ui_handle.card_time.text[i].idx, i18n[ui_handle.card_time.text[i].str_id]);
        }
    }

    //日期卡片
    compo_cardbox_t* card_day = compo_cardbox_create(frm,
                                sizeof(ui_handle.card_day.rect)/sizeof(ui_handle.card_day.rect[0]),
                                0,
                                sizeof(ui_handle.card_day.text)/sizeof(ui_handle.card_day.text[0]),
                                ui_handle.card_day.w, ui_handle.card_day.h);
    compo_cardbox_set_visible(card_day, true);
    compo_cardbox_set_location(card_day, ui_handle.card_day.x, ui_handle.card_day.y, ui_handle.card_day.w, ui_handle.card_day.h);
    compo_setid(card_day, ui_handle.card_day.id);
    for (u8 i=0; i<sizeof(ui_handle.card_day.rect)/sizeof(ui_handle.card_day.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(card_day, ui_handle.card_day.rect[i].idx, ui_handle.card_day.rect[i].x, ui_handle.card_day.rect[i].y,
                                        ui_handle.card_day.rect[i].w, ui_handle.card_day.rect[i].h, ui_handle.card_day.rect[i].r);
        compo_cardbox_rect_set_color(card_day, ui_handle.card_day.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.card_day.text)/sizeof(ui_handle.card_day.text[0]); i++)
    {
        compo_cardbox_text_set_font(card_day, ui_handle.card_day.text[i].idx, ui_handle.card_day.text[i].res);
        widget_text_set_ellipsis(card_day->text[ui_handle.card_day.text[i].idx], false);
        compo_cardbox_text_set_align_center(card_day, ui_handle.card_day.text[i].idx, ui_handle.card_day.text[i].center);
        widget_text_set_wordwrap(card_day->text[ui_handle.card_day.text[i].idx], ui_handle.card_day.text[i].wordwrap);
        widget_text_set_color(card_day->text[ui_handle.card_day.text[i].idx], make_color(ui_handle.card_day.text[i].color.r, ui_handle.card_day.text[i].color.g, ui_handle.card_day.text[i].color.b));
        compo_cardbox_text_set_location(card_day, ui_handle.card_day.text[i].idx, ui_handle.card_day.text[i].x, ui_handle.card_day.text[i].y,
                                        ui_handle.card_day.text[i].w, ui_handle.card_day.text[i].h);

        static char str_buff[300];
        memset(str_buff, '\0', sizeof(str_buff));
        if (ALARM_GET_CYCLE(sys_cb.alarm_edit_idx) & BIT(7))
        {
            snprintf(str_buff, sizeof(str_buff), i18n[STR_ONCE]);
        }
        else if (ALARM_GET_CYCLE(sys_cb.alarm_edit_idx) == 0x7f)
        {
            snprintf(str_buff, sizeof(str_buff), i18n[STR_EVERY_DAY]);
        }
        else
        {
            for (u8 j=0; j<7; j++)
            {
                static char string_handle[300];
                memset(string_handle,0,sizeof(string_handle));
                if (ALARM_GET_CYCLE(sys_cb.alarm_edit_idx) & BIT(j))
                {
                    snprintf(string_handle, sizeof(string_handle),i18n[str_week_buf[j]]);
                    for(int k=0; k<strlen(i18n[str_week_buf[j]]); k++)
                    {
                        str_buff[buf_num] = string_handle[k];
                        buf_num++;
                    }
                    str_buff[buf_num] = ' ';
                    buf_num++;
                }
            }

        }

        if (ui_handle.card_day.text[i].idx == 0)        //周 1 2 3 4 5 6 7
        {
            compo_cardbox_text_set(card_day, ui_handle.card_day.text[i].idx, str_buff);
        }
        else if (ui_handle.card_day.text[i].idx == 1)      //重复
        {
            compo_cardbox_text_set(card_day, ui_handle.card_day.text[i].idx, i18n[ui_handle.card_day.text[i].str_id]);
        }
    }

    return frm;
}

//触摸按钮效果处理
static void func_alarm_clock_sub_edit_button_touch_handle(void)
{

    int id = compo_get_button_id();
    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_DEL_CLICK);

    switch (id)
    {
        case COMPO_ID_BTN_DEL:
            compo_picturebox_set_visible(pic_click, true);
            break;

        default:
            break;
    }

}

//释放按钮效果处理
static void func_alarm_clock_sub_edit_button_release_handle(void)
{
    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_DEL_CLICK);
    compo_picturebox_set_visible(pic_click, false);
}

//获取点击卡片的id
static u16 func_alarm_clcok_sub_edit_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_ID_END-CARD_ID_START-1; i++)
    {
        id = CARD_ID_START + 1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (compo_cardbox_get_visible(cardbox) && abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//单击按钮
static void func_alarm_clock_sub_edit_card_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_DEL:
            ALARM_DELETE(sys_cb.alarm_edit_idx);
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
            func_cb.sta = FUNC_ALARM_CLOCK;
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
            break;

//        case COMPO_ID_BTN_SET:
//            func_cb.sta = FUNC_ALARM_CLOCK_SUB_SET;
//            break;
//
//        case COMPO_ID_BTN_REPEAT:
//            func_cb.sta = FUNC_ALARM_CLOCK_SUB_REPEAT;
//            break;

        default:
        {
            point_t pt = ctp_get_sxy();
            u16 compo_id = func_alarm_clcok_sub_edit_card_get_btn_id(pt);
            if (compo_id <= 0 || compo_id > CARD_ID_END-1)
            {
                return;
            }
            printf("click compo_id:%d\n", compo_id);

            compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
            if (compo_cardbox_get_visible(cardbox))
            {
                if (compo_id == ui_handle.card_time.id)
                {
                    func_cb.sta = FUNC_ALARM_CLOCK_SUB_SET;
                }
                else if (compo_id == ui_handle.card_day.id)
                {
                    func_cb.sta = FUNC_ALARM_CLOCK_SUB_REPEAT;
                }
            }
        }
        break;
    }

    func_alarm_clock_sub_edit_button_release_handle();
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//闹钟功能事件处理
static void func_alarm_clock_sub_edit_process(void)
{

    for(u8 i=0; i<CARD_ID_END-CARD_ID_START-1; i++)      //文本滚动
    {
        u16 id = CARD_ID_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    f_alarm_clock_sub_edit_t *f_alarm_clock_sub_edit = (f_alarm_clock_sub_edit_t*)func_cb.f_cb;
//    compo_page_move_process(f_alarm_clock_sub_edit->ptm);
#endif // GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    func_process();
}

//闹钟功能消息处理
static void func_alarm_clock_sub_edit_message(size_msg_t msg)
{
    f_alarm_clock_sub_edit_t *f_alarm_clock_sub_edit = (f_alarm_clock_sub_edit_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_alarm_clock_sub_edit_button_touch_handle();
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
            compo_page_move_touch_handler(f_alarm_clock_sub_edit->ptm);
#endif // GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
            break;

        case MSG_CTP_CLICK:
            func_alarm_clock_sub_edit_card_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_alarm_clock_sub_edit_button_release_handle();
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_alarm_clock_sub_edit_button_release_handle();
            func_message(msg);
            break;

        case MSG_SYS_500MS:
            if (f_alarm_clock_sub_edit->time_scale != uteModuleSystemtime12HOn())
            {
                if (func_cb.frm_main != NULL)
                {
                    compo_form_destroy(func_cb.frm_main);
                }
                func_alarm_clock_sub_edit_exit();
                if (func_cb.f_cb != NULL)
                {
                    func_free(func_cb.f_cb);
                    func_cb.f_cb = NULL;
                }
                func_alarm_clock_sub_edit_enter();
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入闹钟功能
static void func_alarm_clock_sub_edit_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alarm_clock_sub_edit_t));
    func_cb.frm_main = func_alarm_clock_sub_edit_form_create();

    f_alarm_clock_sub_edit_t *f_alarm_clock_sub_edit = (f_alarm_clock_sub_edit_t*)func_cb.f_cb;
    f_alarm_clock_sub_edit->time_scale = uteModuleSystemtime12HOn();

#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    f_alarm_clock_sub_edit->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .page_size = 360,
        .page_count = 1,
        .up_over_perc = 5,
        .down_over_perc = 5,
    };
    compo_page_move_init(f_alarm_clock_sub_edit->ptm, func_cb.frm_main->page_body, &info);
#endif // GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
}

//退出闹钟功能
static void func_alarm_clock_sub_edit_exit(void)
{
    func_cb.last = FUNC_ALARM_CLOCK_SUB_EDIT;
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    f_alarm_clock_sub_edit_t *f_alarm_clock_sub_edit = (f_alarm_clock_sub_edit_t*)func_cb.f_cb;
    if (f_alarm_clock_sub_edit->ptm)
    {
        func_free(f_alarm_clock_sub_edit->ptm);
    }
#endif // GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
}

//闹钟功能
void func_alarm_clock_sub_edit(void)
{
    printf("%s\n", __func__);
    func_alarm_clock_sub_edit_enter();
    while (func_cb.sta == FUNC_ALARM_CLOCK_SUB_EDIT)
    {
        func_alarm_clock_sub_edit_process();
        func_alarm_clock_sub_edit_message(msg_dequeue());
    }
    func_alarm_clock_sub_edit_exit();
}
