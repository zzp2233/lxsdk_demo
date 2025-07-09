#include "include.h"
#include "func.h"
#include "ute_module_notdisturb.h"
#include "ute_application_common.h"

static void func_set_sub_disturd_exit(void);
static void func_set_sub_disturd_enter(void);
#define         DRAG_AUTO_SPEED     10
#define         DRAG_MAX_DISTANCE   151
#define         DRAG_MAX_DISTANCE1  84
#define         DRAG_MIN_DISTANCE   4
#define         DRAG_MAX_BACK_DISTANCE      117
#define         DRAG_MIN_BACK_DISTANCE      49
#define         FOCUS_AUTO_STEP         5
#define         FOCUS_AUTO_STEP_DIV     16

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

typedef struct f_disturd_t_
{
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移动，惯性
    u32 tick;
    s32 focus_x;
    s32 focus_y;
    s32 focus_dx;
    s32 focus_dy;
    s32 focus_ofsx;
    s32 focus_ofsy;
//    s32 focus_ofsx_max;
//    s32 focus_ofsy_max;
    int moveto_ix;                  //设定自动移到的目标菜单ix
    int moveto_iy;                  //设置自动移到的目标菜单iy
    point_t moveto;                 //设定自动移到的坐标
    u8 time_scale;
} f_disturd_t;

enum
{
    CARD_ID_START = 1,
    COMPO_ID_CARD_DISTURD_ALL,
    COMPO_ID_CARD_DISTURD_DEF,
    COMPO_ID_CARD_DISTURD_START_TIME,
    COMPO_ID_CARD_DISTURD_END_TIME,
    CARD_ID_END,
};

typedef struct ui_handle_t_
{

    ///全天勿扰
    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t
        {
            u8 idx;
            s16 x,y;
            s16 w,h;
            u16 r;
        } rect[1];

        struct card_icon_t
        {
            u8 idx;
            s16 x,y;
            u16 w,h;
            u32 res_on;
            u32 res_off;
        } icon[1];

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
        } text[1];
    } disturd_all_card;

    ///定时勿扰
    struct card1_t disturd_def_card;

    ///开始时间
    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t rect[1];
        struct card_text_t text[3];
    } disturd_start_time;

    ///结束时间
    struct card2_t disturd_end_time;

} ui_handle_t;

static const ui_handle_t ui_handle =
{
    .disturd_all_card = {
        .id = COMPO_ID_CARD_DISTURD_ALL,
        .x  = 4+232/2,
        .y  = 54+62/2,
        .w  = 232,
        .h  = 62,

        .rect = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 0,
                .w      = 232,
                .h      = 62,
                .r      = 16,
            },
        },

        .icon = {
            [0] = {
                .idx    = 0,
                .x      = 182+40/2 - 232/2,
                .y      = 19+24/2 - 62/2,
                .w      = 40,
                .h      = 24,
                .res_on = UI_BUF_I330001_PUBLIC_SWITCH02_BIN,
                .res_off= UI_BUF_I330001_PUBLIC_SWITCH00_BIN,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 12 - 232/2,
                .y      = 18 - 62/2,
                .w      = 165,
                .h      = 26,
                .str_id = STR_DISTURD_ALL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },
        },
    },

    .disturd_def_card = {
        .id = COMPO_ID_CARD_DISTURD_DEF,
        .x  = 4+232/2,
        .y  = 122+62/2,
        .w  = 232,
        .h  = 62,

        .rect = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 0,
                .w      = 232,
                .h      = 62,
                .r      = 16,
            },
        },

        .icon = {
            [0] = {
                .idx    = 0,
                .x      = 182+40/2 - 232/2,
                .y      = 19+24/2 - 62/2,
                .w      = 40,
                .h      = 24,
                .res_on = UI_BUF_I330001_PUBLIC_SWITCH02_BIN,
                .res_off= UI_BUF_I330001_PUBLIC_SWITCH00_BIN,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 12 - 232/2,
                .y      = 18 - 62/2,
                .w      = 165,
                .h      = 26,
                .str_id = STR_DISTURD_TIM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },
        },
    },

    .disturd_start_time = {
        .id = COMPO_ID_CARD_DISTURD_START_TIME,
        .x  = 4+232/2,
        .y  = 190+72/2,
        .w  = 232,
        .h  = 72,

        .rect = {
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
                .w      = 96,
                .h      = 26,
                .str_id = STR_DISTURD_TIM_START,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },

            [1] = {
                .idx    = 1,
                .x      = 10 - 232/2,
                .y      = 40 - 72/2,
                .w      = 60,
                .h      = 26,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = 0,
            },

            [2] = {
                .idx    = 2,
                .x      = 10 - 232/2 + 60,
                .y      = 40 - 72/2,
                .w      = 52,
                .h      = 26,
                .str_id = STR_AM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = STR_PM,
            },
        },
    },


    .disturd_end_time = {
        .id = COMPO_ID_CARD_DISTURD_END_TIME,
        .x  = 4+232/2,
        .y  = 268+72/2,
        .w  = 232,
        .h  = 72,

        .rect = {
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
                .w      = 96,
                .h      = 26,
                .str_id = STR_DISTURD_TIM_END,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },

            [1] = {
                .idx    = 1,
                .x      = 10 - 232/2,
                .y      = 40 - 72/2,
                .w      = 60,
                .h      = 26,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = 0,
            },

            [2] = {
                .idx    = 2,
                .x      = 10 - 232/2 + 60,
                .y      = 40 - 72/2,
                .w      = 52,
                .h      = 26,
                .str_id = STR_AM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = STR_PM,
            },
        },
    },
};

typedef struct func_alarm_hour_format_t_
{
    u8 hour;
    u8 am_pm;
} func_alarm_hour_format_t;

static func_alarm_hour_format_t func_alarm_convert_to_12hour(s8 hour24)
{
    u8 am_pm = (hour24 >= 12) ? 2 : 1;    // 2 PM, 1 AM
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


static void func_set_sub_disturd_state_update(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if(quick.isNotDisturb)
    {
        if(uteModuleNotDisturbIsOpenScheduled())
        {
            sys_cb.disturd_adl = 0;
            sys_cb.disturd_tim = 1;
        }
        else
        {
            sys_cb.disturd_adl = 1;
            sys_cb.disturd_tim = 0;
        }
    }
    else
    {
        if(uteModuleNotDisturbIsOpenScheduled())
        {
            sys_cb.disturd_tim = 1;
        }
        else
        {
            sys_cb.disturd_tim = 0;
        }
        sys_cb.disturd_adl = 0;
    }
    sys_cb.disturd_start_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) * 60;
    sys_cb.disturd_end_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME) * 60;

}

//勿扰模式页面
compo_form_t *func_set_sub_disturd_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_DISTURD]);

    widget_page_set_client(frm->page_body, 0, -DRAG_MIN_BACK_DISTANCE);

    func_set_sub_disturd_state_update();

    ///创建全天勿扰卡片
    compo_cardbox_t* disturd_all_card = compo_cardbox_create(frm,
                                        sizeof(ui_handle.disturd_all_card.rect)/sizeof(ui_handle.disturd_all_card.rect[0]),
                                        sizeof(ui_handle.disturd_all_card.icon)/sizeof(ui_handle.disturd_all_card.icon[0]),
                                        sizeof(ui_handle.disturd_all_card.text)/sizeof(ui_handle.disturd_all_card.text[0]),
                                        ui_handle.disturd_all_card.w, ui_handle.disturd_all_card.h);
    compo_cardbox_set_visible(disturd_all_card, true);
    compo_cardbox_set_location(disturd_all_card, ui_handle.disturd_all_card.x, ui_handle.disturd_all_card.y, ui_handle.disturd_all_card.w, ui_handle.disturd_all_card.h);
    compo_setid(disturd_all_card, ui_handle.disturd_all_card.id);
    for (u8 i=0; i<sizeof(ui_handle.disturd_all_card.rect)/sizeof(ui_handle.disturd_all_card.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(disturd_all_card, ui_handle.disturd_all_card.rect[i].idx, ui_handle.disturd_all_card.rect[i].x, ui_handle.disturd_all_card.rect[i].y,
                                        ui_handle.disturd_all_card.rect[i].w, ui_handle.disturd_all_card.rect[i].h, ui_handle.disturd_all_card.rect[i].r);
        compo_cardbox_rect_set_color(disturd_all_card, ui_handle.disturd_all_card.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_all_card.icon)/sizeof(ui_handle.disturd_all_card.icon[0]); i++)
    {
        compo_cardbox_icon_set_location(disturd_all_card, ui_handle.disturd_all_card.icon[i].idx, ui_handle.disturd_all_card.icon[i].x, ui_handle.disturd_all_card.icon[i].y,
                                        ui_handle.disturd_all_card.icon[i].w, ui_handle.disturd_all_card.icon[i].h);
        compo_cardbox_icon_set(disturd_all_card, ui_handle.disturd_all_card.icon[i].idx,
                               sys_cb.disturd_adl > 0 ? ui_handle.disturd_all_card.icon[i].res_on : ui_handle.disturd_all_card.icon[i].res_off);
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_all_card.text)/sizeof(ui_handle.disturd_all_card.text[0]); i++)
    {
        compo_cardbox_text_set_font(disturd_all_card, ui_handle.disturd_all_card.text[i].idx, ui_handle.disturd_all_card.text[i].res);
        widget_text_set_ellipsis(disturd_all_card->text[ui_handle.disturd_all_card.text[i].idx], false);
        compo_cardbox_text_set_align_center(disturd_all_card, ui_handle.disturd_all_card.text[i].idx, ui_handle.disturd_all_card.text[i].center);
        widget_text_set_wordwrap(disturd_all_card->text[ui_handle.disturd_all_card.text[i].idx], ui_handle.disturd_all_card.text[i].wordwrap);
        widget_text_set_color(disturd_all_card->text[ui_handle.disturd_all_card.text[i].idx], make_color(ui_handle.disturd_all_card.text[i].color.r, ui_handle.disturd_all_card.text[i].color.g, ui_handle.disturd_all_card.text[i].color.b));
        compo_cardbox_text_set_location(disturd_all_card, ui_handle.disturd_all_card.text[i].idx, ui_handle.disturd_all_card.text[i].x, ui_handle.disturd_all_card.text[i].y,
                                        ui_handle.disturd_all_card.text[i].w, ui_handle.disturd_all_card.text[i].h);
        compo_cardbox_text_set(disturd_all_card, ui_handle.disturd_all_card.text[i].idx, i18n[ui_handle.disturd_all_card.text[i].str_id]);
    }

    ///创建定时勿扰卡片
    compo_cardbox_t* disturd_def_card = compo_cardbox_create(frm,
                                        sizeof(ui_handle.disturd_def_card.rect)/sizeof(ui_handle.disturd_def_card.rect[0]),
                                        sizeof(ui_handle.disturd_def_card.icon)/sizeof(ui_handle.disturd_def_card.icon[0]),
                                        sizeof(ui_handle.disturd_def_card.text)/sizeof(ui_handle.disturd_def_card.text[0]),
                                        ui_handle.disturd_def_card.w, ui_handle.disturd_def_card.h);
    compo_cardbox_set_visible(disturd_def_card, true);
    compo_cardbox_set_location(disturd_def_card, ui_handle.disturd_def_card.x, ui_handle.disturd_def_card.y, ui_handle.disturd_def_card.w, ui_handle.disturd_def_card.h);
    compo_setid(disturd_def_card, ui_handle.disturd_def_card.id);
    for (u8 i=0; i<sizeof(ui_handle.disturd_def_card.rect)/sizeof(ui_handle.disturd_def_card.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(disturd_def_card, ui_handle.disturd_def_card.rect[i].idx, ui_handle.disturd_def_card.rect[i].x, ui_handle.disturd_def_card.rect[i].y,
                                        ui_handle.disturd_def_card.rect[i].w, ui_handle.disturd_def_card.rect[i].h, ui_handle.disturd_def_card.rect[i].r);
        compo_cardbox_rect_set_color(disturd_def_card, ui_handle.disturd_def_card.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_def_card.icon)/sizeof(ui_handle.disturd_def_card.icon[0]); i++)
    {
        compo_cardbox_icon_set_location(disturd_def_card, ui_handle.disturd_def_card.icon[i].idx, ui_handle.disturd_def_card.icon[i].x, ui_handle.disturd_def_card.icon[i].y,
                                        ui_handle.disturd_def_card.icon[i].w, ui_handle.disturd_def_card.icon[i].h);
        compo_cardbox_icon_set(disturd_def_card, ui_handle.disturd_def_card.icon[i].idx,
                               sys_cb.disturd_tim > 0 ? ui_handle.disturd_def_card.icon[i].res_on : ui_handle.disturd_def_card.icon[i].res_off);
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_def_card.text)/sizeof(ui_handle.disturd_def_card.text[0]); i++)
    {
        compo_cardbox_text_set_font(disturd_def_card, ui_handle.disturd_def_card.text[i].idx, ui_handle.disturd_def_card.text[i].res);
        widget_text_set_ellipsis(disturd_def_card->text[ui_handle.disturd_def_card.text[i].idx], false);
        compo_cardbox_text_set_align_center(disturd_def_card, ui_handle.disturd_def_card.text[i].idx, ui_handle.disturd_def_card.text[i].center);
        widget_text_set_wordwrap(disturd_def_card->text[ui_handle.disturd_def_card.text[i].idx], ui_handle.disturd_def_card.text[i].wordwrap);
        widget_text_set_color(disturd_def_card->text[ui_handle.disturd_def_card.text[i].idx], make_color(ui_handle.disturd_def_card.text[i].color.r, ui_handle.disturd_def_card.text[i].color.g, ui_handle.disturd_def_card.text[i].color.b));
        compo_cardbox_text_set_location(disturd_def_card, ui_handle.disturd_def_card.text[i].idx, ui_handle.disturd_def_card.text[i].x, ui_handle.disturd_def_card.text[i].y,
                                        ui_handle.disturd_def_card.text[i].w, ui_handle.disturd_def_card.text[i].h);
        compo_cardbox_text_set(disturd_def_card, ui_handle.disturd_def_card.text[i].idx, i18n[ui_handle.disturd_def_card.text[i].str_id]);
    }

    ///创建开始时间卡片
    compo_cardbox_t* disturd_start_time = compo_cardbox_create(frm,
                                          sizeof(ui_handle.disturd_start_time.rect)/sizeof(ui_handle.disturd_start_time.rect[0]),
                                          0,
                                          sizeof(ui_handle.disturd_start_time.text)/sizeof(ui_handle.disturd_start_time.text[0]),
                                          ui_handle.disturd_start_time.w, ui_handle.disturd_start_time.h);
    compo_cardbox_set_visible(disturd_start_time, sys_cb.disturd_tim > 0 ? true : false);
    compo_cardbox_set_location(disturd_start_time, ui_handle.disturd_start_time.x, ui_handle.disturd_start_time.y, ui_handle.disturd_start_time.w, ui_handle.disturd_start_time.h);
    compo_setid(disturd_start_time, ui_handle.disturd_start_time.id);
    for (u8 i=0; i<sizeof(ui_handle.disturd_start_time.text)/sizeof(ui_handle.disturd_start_time.text[0]); i++)
    {
        compo_cardbox_text_set_font(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, ui_handle.disturd_start_time.text[i].res);
        widget_text_set_ellipsis(disturd_start_time->text[ui_handle.disturd_start_time.text[i].idx], false);
        compo_cardbox_text_set_align_center(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, ui_handle.disturd_start_time.text[i].center);
        widget_text_set_wordwrap(disturd_start_time->text[ui_handle.disturd_start_time.text[i].idx], ui_handle.disturd_start_time.text[i].wordwrap);
        widget_text_set_color(disturd_start_time->text[ui_handle.disturd_start_time.text[i].idx], make_color(ui_handle.disturd_start_time.text[i].color.r, ui_handle.disturd_start_time.text[i].color.g, ui_handle.disturd_start_time.text[i].color.b));
        compo_cardbox_text_set_location(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, ui_handle.disturd_start_time.text[i].x, ui_handle.disturd_start_time.text[i].y,
                                        ui_handle.disturd_start_time.text[i].w, ui_handle.disturd_start_time.text[i].h);

        u8 hour = sys_cb.disturd_start_time_sec / 3600;
        u8 min  = (sys_cb.disturd_start_time_sec % 3600) / 60;
        u8 am_pm = 0;
        func_alarm_hour_format_t hour_cov = func_alarm_convert_to_12hour(hour);
        hour = hour_cov.hour;
        am_pm = hour_cov.am_pm;
        char aclock_str[20] = {0};
        memset(aclock_str, '\0', sizeof(aclock_str));
        sprintf(aclock_str, "%02d:%02d", hour, min);

        printf("pm_flag:%d\n", am_pm);
        if (ui_handle.disturd_start_time.text[i].idx == 2)
        {
            if(am_pm == 2)      // PM
            {
                compo_cardbox_text_set(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, i18n[ui_handle.disturd_start_time.text[i].rev]);
            }
            else if (am_pm == 1)
            {
                compo_cardbox_text_set(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, i18n[ui_handle.disturd_start_time.text[i].str_id]);
            }
        }
        else if (ui_handle.disturd_start_time.text[i].idx == 1)
        {
            compo_cardbox_text_set(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, aclock_str);
        }
        else if (ui_handle.disturd_start_time.text[i].idx == 0)
        {
            compo_cardbox_text_set(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, i18n[ui_handle.disturd_start_time.text[i].str_id]);
        }
    }


    ///创建结束时间卡片
    compo_cardbox_t* disturd_end_time = compo_cardbox_create(frm,
                                        sizeof(ui_handle.disturd_end_time.rect)/sizeof(ui_handle.disturd_end_time.rect[0]),
                                        0,
                                        sizeof(ui_handle.disturd_end_time.text)/sizeof(ui_handle.disturd_end_time.text[0]),
                                        ui_handle.disturd_end_time.w, ui_handle.disturd_end_time.h);
    compo_cardbox_set_visible(disturd_end_time, sys_cb.disturd_tim > 0 ? true : false);
    compo_cardbox_set_location(disturd_end_time, ui_handle.disturd_end_time.x, ui_handle.disturd_end_time.y, ui_handle.disturd_end_time.w, ui_handle.disturd_end_time.h);
    compo_setid(disturd_end_time, ui_handle.disturd_end_time.id);
    for (u8 i=0; i<sizeof(ui_handle.disturd_end_time.text)/sizeof(ui_handle.disturd_end_time.text[0]); i++)
    {
        compo_cardbox_text_set_font(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, ui_handle.disturd_end_time.text[i].res);
        widget_text_set_ellipsis(disturd_end_time->text[ui_handle.disturd_end_time.text[i].idx], false);
        compo_cardbox_text_set_align_center(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, ui_handle.disturd_end_time.text[i].center);
        widget_text_set_wordwrap(disturd_end_time->text[ui_handle.disturd_end_time.text[i].idx], ui_handle.disturd_end_time.text[i].wordwrap);
        widget_text_set_color(disturd_end_time->text[ui_handle.disturd_end_time.text[i].idx], make_color(ui_handle.disturd_end_time.text[i].color.r, ui_handle.disturd_end_time.text[i].color.g, ui_handle.disturd_end_time.text[i].color.b));
        compo_cardbox_text_set_location(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, ui_handle.disturd_end_time.text[i].x, ui_handle.disturd_end_time.text[i].y,
                                        ui_handle.disturd_end_time.text[i].w, ui_handle.disturd_end_time.text[i].h);

        u8 hour   = sys_cb.disturd_end_time_sec / 3600;
        u8 min  = (sys_cb.disturd_end_time_sec % 3600) / 60;
        u8 am_pm = 0;
        func_alarm_hour_format_t hour_cov = func_alarm_convert_to_12hour(hour);
        hour = hour_cov.hour;
        am_pm = hour_cov.am_pm;
        char aclock_str[20] = {0};
        memset(aclock_str, '\0', sizeof(aclock_str));
        sprintf(aclock_str, "%02d:%02d", hour, min);

        if (ui_handle.disturd_end_time.text[i].idx == 2)
        {
            if(am_pm == 2)      // PM
            {
                compo_cardbox_text_set(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, i18n[ui_handle.disturd_end_time.text[i].rev]);
            }
            else if (am_pm == 1)
            {
                compo_cardbox_text_set(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, i18n[ui_handle.disturd_end_time.text[i].str_id]);
            }
        }
        else if (ui_handle.disturd_end_time.text[i].idx == 1)
        {
            compo_cardbox_text_set(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, aclock_str);
        }
        else if (ui_handle.disturd_end_time.text[i].idx == 0)
        {
            compo_cardbox_text_set(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, i18n[ui_handle.disturd_end_time.text[i].str_id]);
        }
    }

    return frm;
}

//获取点击卡片的id
static u16 func_set_sub_disturd_card_get_btn_id(point_t pt)
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

//更新显示勿扰模式界面
static void func_set_sub_disturd_disp_update(void)
{
    func_set_sub_disturd_state_update();
    compo_cardbox_t* cardbox_disturd_all = compo_getobj_byid(ui_handle.disturd_all_card.id);
    if (sys_cb.disturd_adl==0)                              //全天勿扰关闭状态
    {
        compo_cardbox_icon_set(cardbox_disturd_all, ui_handle.disturd_all_card.icon[0].idx, ui_handle.disturd_all_card.icon[0].res_off);
    }
    else
    {
        compo_cardbox_icon_set(cardbox_disturd_all, ui_handle.disturd_all_card.icon[0].idx, ui_handle.disturd_all_card.icon[0].res_on);
    }

    compo_cardbox_t* cardbox_disturd_def = compo_getobj_byid(ui_handle.disturd_def_card.id);
    if(sys_cb.disturd_tim == 0)                             //定时勿扰关闭
    {
        compo_cardbox_icon_set(cardbox_disturd_def, ui_handle.disturd_def_card.icon[0].idx, ui_handle.disturd_def_card.icon[0].res_off);
        compo_cardbox_t* cardbox_start_time = compo_getobj_byid(ui_handle.disturd_start_time.id);
        compo_cardbox_set_visible(cardbox_start_time, false);
        compo_cardbox_t* cardbox_end_time = compo_getobj_byid(ui_handle.disturd_end_time.id);
        compo_cardbox_set_visible(cardbox_end_time, false);
    }
    else
    {
        compo_cardbox_icon_set(cardbox_disturd_def, ui_handle.disturd_def_card.icon[0].idx, ui_handle.disturd_def_card.icon[0].res_on);
        compo_cardbox_t* cardbox_start_time = compo_getobj_byid(ui_handle.disturd_start_time.id);
        compo_cardbox_set_visible(cardbox_start_time, true);
        compo_cardbox_t* cardbox_end_time = compo_getobj_byid(ui_handle.disturd_end_time.id);
        compo_cardbox_set_visible(cardbox_end_time, true);
    }

}

//单击按钮
static void func_disturd_card_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_set_sub_disturd_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > CARD_ID_END-1)
    {
        return;
    }
//    printf("click compo_id:%d\n", compo_id);

    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == ui_handle.disturd_all_card.id)              //全天勿扰
        {
            if (sys_cb.disturd_adl == 0)
            {
                if (msgbox((char *)i18n[STR_DISTURD_TIM_CTT], NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE) == MSGBOX_RES_OK)
                {
                    if (sys_cb.disturd_adl == 0)
                    {
                        sys_cb.disturd_adl = 1;
                        uteModuleNotDisturbAllDaySwitch();
                    }
                }
            }
            else
            {
                sys_cb.disturd_adl = 0;
                uteModuleNotDisturbAllDaySwitch();
            }
            uteModuleNotDisturbSetOpenStatus(sys_cb.disturd_adl);
        }
        else if (compo_id == ui_handle.disturd_def_card.id)         //定时勿扰
        {
            if (sys_cb.disturd_tim == 0)
            {
                if (msgbox((char *)i18n[STR_DISTURD_TIM_CTT], NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE) == MSGBOX_RES_OK)
                {
                    if(sys_cb.disturd_tim == 0)
                    {
                        sys_cb.disturd_tim = 1;
                        uteModuleNotDisturbScheduledSwitch();
                    }
                }
            }
            else
            {
                sys_cb.disturd_tim = 0;
                uteModuleNotDisturbScheduledSwitch();
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
                uteModuleNotDisturbSetTime(23*60);
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
                uteModuleNotDisturbSetTime(7*60);
                sys_cb.disturd_start_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) * 60;
                sys_cb.disturd_end_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME) * 60;
                msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
            }
        }
        else if (compo_id == ui_handle.disturd_start_time.id)      //开始时间
        {
            if(sys_cb.disturd_tim)
            {
                sys_cb.disturd_sel = 0;
                func_cb.sta = FUNC_DISTURD_SUB_SET;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
//                task_stack_pop();
            }
        }
        else if (compo_id == ui_handle.disturd_end_time.id)      //结束时间
        {
            if(sys_cb.disturd_tim)
            {
                sys_cb.disturd_sel = 1;
                func_cb.sta = FUNC_DISTURD_SUB_SET;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
//                task_stack_pop();
            }
        }
    }
    func_set_sub_disturd_disp_update();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
typedef struct f_disturd_t_
{
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移动，惯性
    u32 tick;
    s32 focus_x;
    s32 focus_y;
    s32 focus_dx;
    s32 focus_dy;
    s32 focus_ofsx;
    s32 focus_ofsy;
//    s32 focus_ofsx_max;
//    s32 focus_ofsy_max;
    int moveto_ix;                  //设定自动移到的目标菜单ix
    int moveto_iy;                  //设置自动移到的目标菜单iy
    point_t moveto;                 //设定自动移到的坐标
    u8 time_scale;
} f_disturd_t;

enum
{
    CARD_ID_START = 1,
    COMPO_ID_CARD_DISTURD_ALL,
    COMPO_ID_CARD_DISTURD_DEF,
    COMPO_ID_CARD_DISTURD_START_TIME,
    COMPO_ID_CARD_DISTURD_END_TIME,
    CARD_ID_END,
};

typedef struct ui_handle_t_
{

    ///全天勿扰
    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t
        {
            u8 idx;
            s16 x,y;
            s16 w,h;
            u16 r;
        } rect[1];

        struct card_icon_t
        {
            u8 idx;
            s16 x,y;
            u16 w,h;
            u32 res_on;
            u32 res_off;
        } icon[1];

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
        } text[1];
    } disturd_all_card;

    ///定时勿扰
    struct card1_t disturd_def_card;

    ///开始时间
    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t rect[1];
        struct card_text_t text[3];
    } disturd_start_time;

    ///结束时间
    struct card2_t disturd_end_time;

} ui_handle_t;

static const ui_handle_t ui_handle =
{
    .disturd_all_card = {
        .id = COMPO_ID_CARD_DISTURD_ALL,
        .x  = 4+232/2,
        .y  = 54+62/2,
        .w  = 232,
        .h  = 62,

        .rect = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 0,
                .w      = 232,
                .h      = 62,
                .r      = 16,
            },
        },

        .icon = {
            [0] = {
                .idx    = 0,
                .x      = 182+40/2 - 232/2,
                .y      = 19+24/2 - 62/2,
                .w      = 40,
                .h      = 24,
//                .res_on = UI_BUF_I330001_PUBLIC_SWITCH02_BIN,
//                .res_off= UI_BUF_I330001_PUBLIC_SWITCH00_BIN,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 12 - 232/2,
                .y      = 18 - 62/2,
                .w      = 96,
                .h      = 26,
                .str_id = STR_DISTURD_ALL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },
        },
    },

    .disturd_def_card = {
        .id = COMPO_ID_CARD_DISTURD_DEF,
        .x  = 4+232/2,
        .y  = 122+62/2,
        .w  = 232,
        .h  = 62,

        .rect = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 0,
                .w      = 232,
                .h      = 62,
                .r      = 16,
            },
        },

        .icon = {
            [0] = {
                .idx    = 0,
                .x      = 182+40/2 - 232/2,
                .y      = 19+24/2 - 62/2,
                .w      = 40,
                .h      = 24,
//                .res_on = UI_BUF_I330001_PUBLIC_SWITCH02_BIN,
//                .res_off= UI_BUF_I330001_PUBLIC_SWITCH00_BIN,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 12 - 232/2,
                .y      = 18 - 62/2,
                .w      = 96,
                .h      = 26,
                .str_id = STR_DISTURD_TIM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },
        },
    },

    .disturd_start_time = {
        .id = COMPO_ID_CARD_DISTURD_START_TIME,
        .x  = 4+232/2,
        .y  = 190+72/2,
        .w  = 232,
        .h  = 72,

        .rect = {
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
                .w      = 96,
                .h      = 26,
                .str_id = STR_DISTURD_TIM_START,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },

            [1] = {
                .idx    = 1,
                .x      = 10 - 232/2,
                .y      = 40 - 72/2,
                .w      = 60,
                .h      = 26,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = 0,
            },

            [2] = {
                .idx    = 2,
                .x      = 10 - 232/2 + 60,
                .y      = 40 - 72/2,
                .w      = 52,
                .h      = 26,
                .str_id = STR_AM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = STR_PM,
            },
        },
    },


    .disturd_end_time = {
        .id = COMPO_ID_CARD_DISTURD_END_TIME,
        .x  = 4+232/2,
        .y  = 268+72/2,
        .w  = 232,
        .h  = 72,

        .rect = {
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
                .w      = 96,
                .h      = 26,
                .str_id = STR_DISTURD_TIM_END,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },

            [1] = {
                .idx    = 1,
                .x      = 10 - 232/2,
                .y      = 40 - 72/2,
                .w      = 60,
                .h      = 26,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = 0,
            },

            [2] = {
                .idx    = 2,
                .x      = 10 - 232/2 + 60,
                .y      = 40 - 72/2,
                .w      = 52,
                .h      = 26,
                .str_id = STR_AM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = STR_PM,
            },
        },
    },
};

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

static void func_set_sub_disturd_state_update(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if(quick.isNotDisturb)
    {
        if(uteModuleNotDisturbIsOpenScheduled())
        {
            sys_cb.disturd_adl = 0;
            sys_cb.disturd_tim = 1;
        }
        else
        {
            sys_cb.disturd_adl = 1;
            sys_cb.disturd_tim = 0;
        }
    }
    else
    {
        if(uteModuleNotDisturbIsOpenScheduled())
        {
            sys_cb.disturd_tim = 1;
        }
        else
        {
            sys_cb.disturd_tim = 0;
        }
        sys_cb.disturd_adl = 0;
    }
    sys_cb.disturd_start_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) * 60;
    sys_cb.disturd_end_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME) * 60;

}

//勿扰模式页面
compo_form_t *func_set_sub_disturd_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_DISTURD]);

    widget_page_set_client(frm->page_body, 0, -DRAG_MIN_BACK_DISTANCE);

    func_set_sub_disturd_state_update();

    ///创建全天勿扰卡片
    compo_cardbox_t* disturd_all_card = compo_cardbox_create(frm,
                                        sizeof(ui_handle.disturd_all_card.rect)/sizeof(ui_handle.disturd_all_card.rect[0]),
                                        sizeof(ui_handle.disturd_all_card.icon)/sizeof(ui_handle.disturd_all_card.icon[0]),
                                        sizeof(ui_handle.disturd_all_card.text)/sizeof(ui_handle.disturd_all_card.text[0]),
                                        ui_handle.disturd_all_card.w, ui_handle.disturd_all_card.h);
    compo_cardbox_set_visible(disturd_all_card, true);
    compo_cardbox_set_location(disturd_all_card, ui_handle.disturd_all_card.x, ui_handle.disturd_all_card.y, ui_handle.disturd_all_card.w, ui_handle.disturd_all_card.h);
    compo_setid(disturd_all_card, ui_handle.disturd_all_card.id);
    for (u8 i=0; i<sizeof(ui_handle.disturd_all_card.rect)/sizeof(ui_handle.disturd_all_card.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(disturd_all_card, ui_handle.disturd_all_card.rect[i].idx, ui_handle.disturd_all_card.rect[i].x, ui_handle.disturd_all_card.rect[i].y,
                                        ui_handle.disturd_all_card.rect[i].w, ui_handle.disturd_all_card.rect[i].h, ui_handle.disturd_all_card.rect[i].r);
        compo_cardbox_rect_set_color(disturd_all_card, ui_handle.disturd_all_card.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_all_card.icon)/sizeof(ui_handle.disturd_all_card.icon[0]); i++)
    {
        compo_cardbox_icon_set_location(disturd_all_card, ui_handle.disturd_all_card.icon[i].idx, ui_handle.disturd_all_card.icon[i].x, ui_handle.disturd_all_card.icon[i].y,
                                        ui_handle.disturd_all_card.icon[i].w, ui_handle.disturd_all_card.icon[i].h);
        compo_cardbox_icon_set(disturd_all_card, ui_handle.disturd_all_card.icon[i].idx,
                               sys_cb.disturd_adl > 0 ? ui_handle.disturd_all_card.icon[i].res_on : ui_handle.disturd_all_card.icon[i].res_off);
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_all_card.text)/sizeof(ui_handle.disturd_all_card.text[0]); i++)
    {
        compo_cardbox_text_set_font(disturd_all_card, ui_handle.disturd_all_card.text[i].idx, ui_handle.disturd_all_card.text[i].res);
        widget_text_set_ellipsis(disturd_all_card->text[ui_handle.disturd_all_card.text[i].idx], false);
        compo_cardbox_text_set_align_center(disturd_all_card, ui_handle.disturd_all_card.text[i].idx, ui_handle.disturd_all_card.text[i].center);
        widget_text_set_wordwrap(disturd_all_card->text[ui_handle.disturd_all_card.text[i].idx], ui_handle.disturd_all_card.text[i].wordwrap);
        widget_text_set_color(disturd_all_card->text[ui_handle.disturd_all_card.text[i].idx], make_color(ui_handle.disturd_all_card.text[i].color.r, ui_handle.disturd_all_card.text[i].color.g, ui_handle.disturd_all_card.text[i].color.b));
        compo_cardbox_text_set_location(disturd_all_card, ui_handle.disturd_all_card.text[i].idx, ui_handle.disturd_all_card.text[i].x, ui_handle.disturd_all_card.text[i].y,
                                        ui_handle.disturd_all_card.text[i].w, ui_handle.disturd_all_card.text[i].h);
        compo_cardbox_text_set(disturd_all_card, ui_handle.disturd_all_card.text[i].idx, i18n[ui_handle.disturd_all_card.text[i].str_id]);
    }

    ///创建定时勿扰卡片
    compo_cardbox_t* disturd_def_card = compo_cardbox_create(frm,
                                        sizeof(ui_handle.disturd_def_card.rect)/sizeof(ui_handle.disturd_def_card.rect[0]),
                                        sizeof(ui_handle.disturd_def_card.icon)/sizeof(ui_handle.disturd_def_card.icon[0]),
                                        sizeof(ui_handle.disturd_def_card.text)/sizeof(ui_handle.disturd_def_card.text[0]),
                                        ui_handle.disturd_def_card.w, ui_handle.disturd_def_card.h);
    compo_cardbox_set_visible(disturd_def_card, true);
    compo_cardbox_set_location(disturd_def_card, ui_handle.disturd_def_card.x, ui_handle.disturd_def_card.y, ui_handle.disturd_def_card.w, ui_handle.disturd_def_card.h);
    compo_setid(disturd_def_card, ui_handle.disturd_def_card.id);
    for (u8 i=0; i<sizeof(ui_handle.disturd_def_card.rect)/sizeof(ui_handle.disturd_def_card.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(disturd_def_card, ui_handle.disturd_def_card.rect[i].idx, ui_handle.disturd_def_card.rect[i].x, ui_handle.disturd_def_card.rect[i].y,
                                        ui_handle.disturd_def_card.rect[i].w, ui_handle.disturd_def_card.rect[i].h, ui_handle.disturd_def_card.rect[i].r);
        compo_cardbox_rect_set_color(disturd_def_card, ui_handle.disturd_def_card.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_def_card.icon)/sizeof(ui_handle.disturd_def_card.icon[0]); i++)
    {
        compo_cardbox_icon_set_location(disturd_def_card, ui_handle.disturd_def_card.icon[i].idx, ui_handle.disturd_def_card.icon[i].x, ui_handle.disturd_def_card.icon[i].y,
                                        ui_handle.disturd_def_card.icon[i].w, ui_handle.disturd_def_card.icon[i].h);
        compo_cardbox_icon_set(disturd_def_card, ui_handle.disturd_def_card.icon[i].idx,
                               sys_cb.disturd_tim > 0 ? ui_handle.disturd_def_card.icon[i].res_on : ui_handle.disturd_def_card.icon[i].res_off);
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_def_card.text)/sizeof(ui_handle.disturd_def_card.text[0]); i++)
    {
        compo_cardbox_text_set_font(disturd_def_card, ui_handle.disturd_def_card.text[i].idx, ui_handle.disturd_def_card.text[i].res);
        widget_text_set_ellipsis(disturd_def_card->text[ui_handle.disturd_def_card.text[i].idx], false);
        compo_cardbox_text_set_align_center(disturd_def_card, ui_handle.disturd_def_card.text[i].idx, ui_handle.disturd_def_card.text[i].center);
        widget_text_set_wordwrap(disturd_def_card->text[ui_handle.disturd_def_card.text[i].idx], ui_handle.disturd_def_card.text[i].wordwrap);
        widget_text_set_color(disturd_def_card->text[ui_handle.disturd_def_card.text[i].idx], make_color(ui_handle.disturd_def_card.text[i].color.r, ui_handle.disturd_def_card.text[i].color.g, ui_handle.disturd_def_card.text[i].color.b));
        compo_cardbox_text_set_location(disturd_def_card, ui_handle.disturd_def_card.text[i].idx, ui_handle.disturd_def_card.text[i].x, ui_handle.disturd_def_card.text[i].y,
                                        ui_handle.disturd_def_card.text[i].w, ui_handle.disturd_def_card.text[i].h);
        compo_cardbox_text_set(disturd_def_card, ui_handle.disturd_def_card.text[i].idx, i18n[ui_handle.disturd_def_card.text[i].str_id]);
    }

    ///创建开始时间卡片
    compo_cardbox_t* disturd_start_time = compo_cardbox_create(frm,
                                          sizeof(ui_handle.disturd_start_time.rect)/sizeof(ui_handle.disturd_start_time.rect[0]),
                                          0,
                                          sizeof(ui_handle.disturd_start_time.text)/sizeof(ui_handle.disturd_start_time.text[0]),
                                          ui_handle.disturd_start_time.w, ui_handle.disturd_start_time.h);
    compo_cardbox_set_visible(disturd_start_time, sys_cb.disturd_tim > 0 ? true : false);
    compo_cardbox_set_location(disturd_start_time, ui_handle.disturd_start_time.x, ui_handle.disturd_start_time.y, ui_handle.disturd_start_time.w, ui_handle.disturd_start_time.h);
    compo_setid(disturd_start_time, ui_handle.disturd_start_time.id);
    for (u8 i=0; i<sizeof(ui_handle.disturd_start_time.rect)/sizeof(ui_handle.disturd_start_time.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(disturd_start_time, ui_handle.disturd_start_time.rect[i].idx, ui_handle.disturd_start_time.rect[i].x, ui_handle.disturd_start_time.rect[i].y,
                                        ui_handle.disturd_start_time.rect[i].w, ui_handle.disturd_start_time.rect[i].h, ui_handle.disturd_start_time.rect[i].r);
        compo_cardbox_rect_set_color(disturd_start_time, ui_handle.disturd_start_time.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_start_time.text)/sizeof(ui_handle.disturd_start_time.text[0]); i++)
    {
        compo_cardbox_text_set_font(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, ui_handle.disturd_start_time.text[i].res);
        widget_text_set_ellipsis(disturd_start_time->text[ui_handle.disturd_start_time.text[i].idx], false);
        compo_cardbox_text_set_align_center(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, ui_handle.disturd_start_time.text[i].center);
        widget_text_set_wordwrap(disturd_start_time->text[ui_handle.disturd_start_time.text[i].idx], ui_handle.disturd_start_time.text[i].wordwrap);
        widget_text_set_color(disturd_start_time->text[ui_handle.disturd_start_time.text[i].idx], make_color(ui_handle.disturd_start_time.text[i].color.r, ui_handle.disturd_start_time.text[i].color.g, ui_handle.disturd_start_time.text[i].color.b));
        compo_cardbox_text_set_location(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, ui_handle.disturd_start_time.text[i].x, ui_handle.disturd_start_time.text[i].y,
                                        ui_handle.disturd_start_time.text[i].w, ui_handle.disturd_start_time.text[i].h);
//        if(sys_cb.disturd_tim == 0)
//        {
//            sys_cb.disturd_start_time_sec = 0;
//            sys_cb.disturd_end_time_sec = 0;
//        }
        u8 hour = sys_cb.disturd_start_time_sec / 3600;
        u8 min  = (sys_cb.disturd_start_time_sec % 3600) / 60;
        u8 am_pm = 0;
        func_alarm_hour_format_t hour_cov = func_alarm_convert_to_12hour(hour);
        hour = hour_cov.hour;
        am_pm = hour_cov.am_pm;
        char aclock_str[20] = {0};
        memset(aclock_str, '\0', sizeof(aclock_str));
        sprintf(aclock_str, "%02d:%02d", hour, min);

        if (ui_handle.disturd_start_time.text[i].idx == 2)
        {
            if(am_pm == 1)      //AM
            {
                compo_cardbox_text_set(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, i18n[ui_handle.disturd_start_time.text[i].str_id]);
            }
            else if (am_pm == 2)
            {
                compo_cardbox_text_set(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, i18n[ui_handle.disturd_start_time.text[i].rev]);
            }
            else
            {
//                compo_cardbox_text_set(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, i18n[ui_handle.disturd_start_time.text[i].rev]);
            }
        }
        else if (ui_handle.disturd_start_time.text[i].idx == 1)
        {
            compo_cardbox_text_set(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, aclock_str);
        }
        else if (ui_handle.disturd_start_time.text[i].idx == 0)
        {
            compo_cardbox_text_set(disturd_start_time, ui_handle.disturd_start_time.text[i].idx, i18n[ui_handle.disturd_start_time.text[i].str_id]);
        }
    }

    ///创建结束时间卡片
    compo_cardbox_t* disturd_end_time = compo_cardbox_create(frm,
                                        sizeof(ui_handle.disturd_end_time.rect)/sizeof(ui_handle.disturd_end_time.rect[0]),
                                        0,
                                        sizeof(ui_handle.disturd_end_time.text)/sizeof(ui_handle.disturd_end_time.text[0]),
                                        ui_handle.disturd_end_time.w, ui_handle.disturd_end_time.h);
    compo_cardbox_set_visible(disturd_end_time, sys_cb.disturd_tim > 0 ? true : false);
    compo_cardbox_set_location(disturd_end_time, ui_handle.disturd_end_time.x, ui_handle.disturd_end_time.y, ui_handle.disturd_end_time.w, ui_handle.disturd_end_time.h);
    compo_setid(disturd_end_time, ui_handle.disturd_end_time.id);
    for (u8 i=0; i<sizeof(ui_handle.disturd_end_time.rect)/sizeof(ui_handle.disturd_end_time.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(disturd_end_time, ui_handle.disturd_end_time.rect[i].idx, ui_handle.disturd_end_time.rect[i].x, ui_handle.disturd_end_time.rect[i].y,
                                        ui_handle.disturd_end_time.rect[i].w, ui_handle.disturd_end_time.rect[i].h, ui_handle.disturd_end_time.rect[i].r);
        compo_cardbox_rect_set_color(disturd_end_time, ui_handle.disturd_end_time.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.disturd_end_time.text)/sizeof(ui_handle.disturd_end_time.text[0]); i++)
    {
        compo_cardbox_text_set_font(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, ui_handle.disturd_end_time.text[i].res);
        widget_text_set_ellipsis(disturd_end_time->text[ui_handle.disturd_end_time.text[i].idx], false);
        compo_cardbox_text_set_align_center(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, ui_handle.disturd_end_time.text[i].center);
        widget_text_set_wordwrap(disturd_end_time->text[ui_handle.disturd_end_time.text[i].idx], ui_handle.disturd_end_time.text[i].wordwrap);
        widget_text_set_color(disturd_end_time->text[ui_handle.disturd_end_time.text[i].idx], make_color(ui_handle.disturd_end_time.text[i].color.r, ui_handle.disturd_end_time.text[i].color.g, ui_handle.disturd_end_time.text[i].color.b));
        compo_cardbox_text_set_location(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, ui_handle.disturd_end_time.text[i].x, ui_handle.disturd_end_time.text[i].y,
                                        ui_handle.disturd_end_time.text[i].w, ui_handle.disturd_end_time.text[i].h);

//        if(sys_cb.disturd_tim == 0)
//        {
//            sys_cb.disturd_start_time_sec = 0;
//            sys_cb.disturd_end_time_sec = 0;
//        }
        u8 hour   = sys_cb.disturd_end_time_sec / 3600;
        u8 min  = (sys_cb.disturd_end_time_sec % 3600) / 60;
        u8 am_pm = 0;
        func_alarm_hour_format_t hour_cov = func_alarm_convert_to_12hour(hour);
        hour = hour_cov.hour;
        am_pm = hour_cov.am_pm;
        char aclock_str[20] = {0};
        memset(aclock_str, '\0', sizeof(aclock_str));
        sprintf(aclock_str, "%02d:%02d", hour, min);

        if (ui_handle.disturd_end_time.text[i].idx == 2)
        {
            if(am_pm == 1)      //AM
            {
                compo_cardbox_text_set(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, i18n[ui_handle.disturd_end_time.text[i].str_id]);
            }
            else if (am_pm == 2)
            {
                compo_cardbox_text_set(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, i18n[ui_handle.disturd_end_time.text[i].rev]);
            }
            else
            {
//                compo_cardbox_text_set(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, i18n[ui_handle.disturd_end_time.text[i].rev]);
            }
        }
        else if (ui_handle.disturd_end_time.text[i].idx == 1)
        {
            compo_cardbox_text_set(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, aclock_str);
        }
        else if (ui_handle.disturd_end_time.text[i].idx == 0)
        {
            compo_cardbox_text_set(disturd_end_time, ui_handle.disturd_end_time.text[i].idx, i18n[ui_handle.disturd_end_time.text[i].str_id]);
        }
    }
    return frm;
}

//获取点击卡片的id
static u16 func_set_sub_disturd_card_get_btn_id(point_t pt)
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

//更新显示勿扰模式界面
static void func_set_sub_disturd_disp_update(void)
{
    func_set_sub_disturd_state_update();
    compo_cardbox_t* cardbox_disturd_all = compo_getobj_byid(ui_handle.disturd_all_card.id);
    if (sys_cb.disturd_adl==0)                              //全天勿扰关闭状态
    {
        compo_cardbox_icon_set(cardbox_disturd_all, ui_handle.disturd_all_card.icon[0].idx, ui_handle.disturd_all_card.icon[0].res_off);
    }
    else
    {
        compo_cardbox_icon_set(cardbox_disturd_all, ui_handle.disturd_all_card.icon[0].idx, ui_handle.disturd_all_card.icon[0].res_on);
    }

    compo_cardbox_t* cardbox_disturd_def = compo_getobj_byid(ui_handle.disturd_def_card.id);
    if(sys_cb.disturd_tim == 0)                             //定时勿扰关闭
    {
        compo_cardbox_icon_set(cardbox_disturd_def, ui_handle.disturd_def_card.icon[0].idx, ui_handle.disturd_def_card.icon[0].res_off);
        compo_cardbox_t* cardbox_start_time = compo_getobj_byid(ui_handle.disturd_start_time.id);
        compo_cardbox_set_visible(cardbox_start_time, false);
        compo_cardbox_t* cardbox_end_time = compo_getobj_byid(ui_handle.disturd_end_time.id);
        compo_cardbox_set_visible(cardbox_end_time, false);
    }
    else
    {
        compo_cardbox_icon_set(cardbox_disturd_def, ui_handle.disturd_def_card.icon[0].idx, ui_handle.disturd_def_card.icon[0].res_on);
        compo_cardbox_t* cardbox_start_time = compo_getobj_byid(ui_handle.disturd_start_time.id);
        compo_cardbox_set_visible(cardbox_start_time, true);
        compo_cardbox_t* cardbox_end_time = compo_getobj_byid(ui_handle.disturd_end_time.id);
        compo_cardbox_set_visible(cardbox_end_time, true);
    }

}

//单击按钮
static void func_disturd_card_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_set_sub_disturd_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > CARD_ID_END-1)
    {
        return;
    }
//    printf("click compo_id:%d\n", compo_id);

    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == ui_handle.disturd_all_card.id)              //全天勿扰
        {
            if (sys_cb.disturd_adl == 0)
            {
                if (msgbox((char *)i18n[STR_DISTURD_TIM_CTT], NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE) == MSGBOX_RES_OK)
                {
                    if (sys_cb.disturd_adl == 0)
                    {
                        sys_cb.disturd_adl = 1;
                        uteModuleNotDisturbAllDaySwitch();
                    }
                }
            }
            else
            {
                sys_cb.disturd_adl = 0;
                uteModuleNotDisturbAllDaySwitch();
            }
            uteModuleNotDisturbSetOpenStatus(sys_cb.disturd_adl);
        }
        else if (compo_id == ui_handle.disturd_def_card.id)         //定时勿扰
        {
            if (sys_cb.disturd_tim == 0)
            {
                if (msgbox((char *)i18n[STR_DISTURD_TIM_CTT], NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE) == MSGBOX_RES_OK)
                {
                    if(sys_cb.disturd_tim == 0)
                    {
                        sys_cb.disturd_tim = 1;
                        uteModuleNotDisturbScheduledSwitch();
                    }
                }
            }
            else
            {
                sys_cb.disturd_tim = 0;
                uteModuleNotDisturbScheduledSwitch();
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
                uteModuleNotDisturbSetTime(23*60);
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
                uteModuleNotDisturbSetTime(7*60);
                sys_cb.disturd_start_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) * 60;
                sys_cb.disturd_end_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME) * 60;
                msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
            }
        }
        else if (compo_id == ui_handle.disturd_start_time.id)      //开始时间
        {
            if(sys_cb.disturd_tim)
            {
                sys_cb.disturd_sel = 0;
                func_cb.sta = FUNC_DISTURD_SUB_SET;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
//                task_stack_pop();
            }
        }
        else if (compo_id == ui_handle.disturd_end_time.id)      //结束时间
        {
            if(sys_cb.disturd_tim)
            {
                sys_cb.disturd_sel = 1;
                func_cb.sta = FUNC_DISTURD_SUB_SET;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
//                task_stack_pop();
            }
        }
    }
    func_set_sub_disturd_disp_update();
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//勿扰模式事件处理
static void func_set_sub_disturd_process(void)
{
    f_disturd_t* f_disturd = (f_disturd_t*)func_cb.f_cb;
    if (f_disturd->flag_drag)
    {
//        s32 dx = f_disturd->focus_dx;
//        s32 dy = f_disturd->focus_dy;
        f_disturd->flag_drag = ctp_get_dxy(&f_disturd->focus_dx, &f_disturd->focus_dy);
        if (f_disturd->flag_drag)
        {
            //拖动菜单图标
//            f_disturd->focus_xstep = f_disturd->focus_dx - dx;
//            f_disturd->focus_ystep = f_disturd->focus_dy - dy;
            f_disturd->focus_ofsx = 0;
            f_disturd->focus_ofsy = f_disturd->focus_y - f_disturd->focus_dy;

            if (sys_cb.disturd_tim == 1)
            {
                if (f_disturd->focus_ofsy > DRAG_MAX_DISTANCE)
                {
                    f_disturd->focus_ofsy = DRAG_MAX_DISTANCE;
                }
            }
            else
            {
                if (f_disturd->focus_ofsy > DRAG_MAX_DISTANCE1)
                {
                    f_disturd->focus_ofsy = DRAG_MAX_DISTANCE1;
                }
            }

            if (f_disturd->focus_ofsy < DRAG_MIN_DISTANCE)
            {
                f_disturd->focus_ofsy = DRAG_MIN_DISTANCE;
            }

            widget_page_set_client(func_cb.frm_main->page_body, f_disturd->focus_ofsx, -f_disturd->focus_ofsy);

//            printf("f_disturd->focus_ofsy =%d\n", f_disturd->focus_ofsy);
        }
        else
        {
            //抬手后开始自动移动
            point_t last_dxy = ctp_get_last_dxy();
            s32 to_x;
            s32 to_y;
            f_disturd->focus_x = f_disturd->focus_ofsx;
            f_disturd->focus_y = f_disturd->focus_ofsy;
            f_disturd->flag_move_auto = true;
            to_x = 0;
            to_y = f_disturd->focus_y - (last_dxy.y * DRAG_AUTO_SPEED);
            f_disturd->moveto.x = to_x;
            f_disturd->moveto.y = to_y;
            if (sys_cb.disturd_tim == 1)
            {
                if (f_disturd->moveto.y > DRAG_MAX_DISTANCE)
                {
                    f_disturd->moveto.y = DRAG_MAX_DISTANCE;
                }
            }
            else
            {
                if (f_disturd->moveto.y > DRAG_MAX_DISTANCE1)
                {
                    f_disturd->moveto.y = DRAG_MAX_DISTANCE1;
                }
            }

            if (f_disturd->moveto.y < DRAG_MIN_DISTANCE)
            {
                f_disturd->moveto.y = DRAG_MIN_DISTANCE;
            }
            f_disturd->tick = tick_get();
        }
    }
    else if (f_disturd->flag_move_auto)
    {
        //自动移动
        if (f_disturd->focus_x == f_disturd->moveto.x && f_disturd->focus_y == f_disturd->moveto.y)
        {
            if (sys_cb.disturd_tim == 1)
            {
                if (f_disturd->focus_y < DRAG_MIN_BACK_DISTANCE)
                {
                    f_disturd->moveto.y = DRAG_MIN_BACK_DISTANCE;
                }
                else if (f_disturd->focus_y > DRAG_MAX_BACK_DISTANCE)
                {
                    f_disturd->moveto.y = DRAG_MAX_BACK_DISTANCE;
                }
                else
                {
                    f_disturd->flag_move_auto = false;            //移动完成
                }
            }
            else
            {
                if (f_disturd->focus_y < DRAG_MIN_BACK_DISTANCE)
                {
                    f_disturd->moveto.y = DRAG_MIN_BACK_DISTANCE;
                }
                else if (f_disturd->focus_y > DRAG_MIN_BACK_DISTANCE)
                {
                    f_disturd->moveto.y = DRAG_MIN_BACK_DISTANCE;
                }
                else
                {
                    f_disturd->flag_move_auto = false;            //移动完成
                }
            }
        }
        else if (tick_check_expire(f_disturd->tick, 10))
        {
            TRACE("[%d %d]--move_to->[%d %d]\n", f_disturd->focus_x, f_disturd->focus_y, f_disturd->moveto.x, f_disturd->moveto.y);
            s16 dx, dy;
            f_disturd->tick = tick_get();
            dx = 0;
            dy = f_disturd->moveto.y - f_disturd->focus_y;
            if (dy > 0)
            {
                if (dy > FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy > FOCUS_AUTO_STEP)
                {
                    dy = FOCUS_AUTO_STEP;
                }
                else
                {
                    dy = 1;
                }
            }
            else if (dy < 0)
            {
                if (dy < -FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy < -FOCUS_AUTO_STEP)
                {
                    dy = -FOCUS_AUTO_STEP;
                }
                else
                {
                    dy = -1;
                }
            }
            f_disturd->focus_x += dx;
            f_disturd->focus_y += dy;
            widget_page_set_client(func_cb.frm_main->page_body, f_disturd->focus_x, -f_disturd->focus_y);
        }
    }


    func_set_sub_disturd_disp_update();

    //卡片文本滚动
    for(u8 i=0; i<CARD_ID_END - CARD_ID_START - 1; i++)      //文本滚动
    {
        u16 id = CARD_ID_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
    func_process();
}

//勿扰模式能消息处理
static void func_set_sub_disturd_message(size_msg_t msg)
{
    f_disturd_t* f_disturd = (f_disturd_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_disturd_card_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
//        case MSG_CTP_TOUCH:
            f_disturd->flag_drag = true;
            f_disturd->flag_move_auto = true;
            break;

//        case MSG_QDEC_FORWARD:                              //向前滚动菜单
//            f_disturd->flag_move_auto = true;
//            f_disturd->moveto.x = 0;
//            f_disturd->moveto.y++;
////            if (f_disturd->moveto.y > 268+90) {
////                f_disturd->moveto.y = 268+90;
////            }
//            break;
//
//        case MSG_QDEC_BACKWARD:
//            f_disturd->flag_move_auto = true;
//            f_disturd->moveto.x = 0;
//            f_disturd->moveto.y--;
//
////            if (f_disturd->moveto.y < 0) {
////                f_disturd->moveto.y = 0;
////            }
//
//            break;
        case MSG_SYS_500MS:

            if (f_disturd->time_scale != uteModuleSystemtime12HOn())
            {
                if (func_cb.frm_main != NULL)
                {
                    compo_form_destroy(func_cb.frm_main);
                }
                func_set_sub_disturd_exit();
                if (func_cb.f_cb != NULL)
                {
                    func_free(func_cb.f_cb);
                    func_cb.f_cb = NULL;
                }
                func_set_sub_disturd_enter();
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入勿扰模式功能
static void func_set_sub_disturd_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_disturd_t));
    if (sys_cb.disturd_tim == 0)
    {
        uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
        uteModuleNotDisturbSetTime(23*60);
        uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
        uteModuleNotDisturbSetTime(7*60);
    }
    sys_cb.disturd_start_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) * 60;
    sys_cb.disturd_end_time_sec = uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME) * 60;
    printf("hour3:%d\n",sys_cb.disturd_start_time_sec);

    func_cb.frm_main = func_set_sub_disturd_form_create();
    f_disturd_t* f_disturd = (f_disturd_t*)func_cb.f_cb;
    memset(f_disturd, 0, sizeof(f_disturd_t));

    f_disturd->focus_y = DRAG_MIN_BACK_DISTANCE;
    widget_page_set_client(func_cb.frm_main->page_body, 0, -f_disturd->focus_y);
    f_disturd->time_scale = uteModuleSystemtime12HOn();
}

//退出勿扰模式功能
static void func_set_sub_disturd_exit(void)
{
    func_cb.last = FUNC_SET_SUB_DISTURD;
}

//勿扰模式功能
void func_set_sub_disturd(void)
{
    printf("%s\n", __func__);
    func_set_sub_disturd_enter();
    while (func_cb.sta == FUNC_SET_SUB_DISTURD)
    {
        func_set_sub_disturd_process();
        func_set_sub_disturd_message(msg_dequeue());
    }
    func_set_sub_disturd_exit();
}
