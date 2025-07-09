#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_sport.h"
#include "ute_module_sleep.h"
#include "ute_module_heart.h"
#include "func_bt.h"

#if UTE_MODULE_SCREENS_UP_MENU_SUPPORT

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if 0

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define CARD_SCALE_START_POS            190//265                                                                     //卡片开始缩小位置
#define CARD_SCALE_STOP_POS             230//320                                                                     //卡片停止缩小位置
#define CARD_SCALE_STOP_Y               (CARD_SCALE_STOP_POS + 156 * 2)                                         //卡片停止缩小实际位置（越大缩小越慢，可见叠加越多）
#define CARD_WIDTH_MIN                  150//200                                                                     //卡片缩小的最小宽度
#define CARD_UNVISIBLE_POS_TOP          (-100)//(-160)                                                                  //卡片超出屏幕顶部某位置后设为不可见
#define CARD_UNVISIBLE_POS_BOTTOM       (CARD_SCALE_STOP_Y + 156)                                               //卡片缩到最小后实际位置大于某值设为不可见
#define CARD_BG_GRAY_START              41                                                                      //卡片开始缩小位置处背景的灰度
#define CARD_BG_GRAY_STOP               10                                                                      //卡片停止缩小位置处背景的灰度
#define GRAY_LV(x)                      make_color(x, x, x)                                                     //生成灰度颜色值（0:黑 255:白）

#define SPRING_Y_MAX                    0                                                                       //回弹到顶部时的ofs_y
//#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - card_y_info[CARD_COUNT - 1] + 50)                     //回弹到底部时的ofs_y
#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_END-1) + 50)                     //回弹到底部时的ofs_y
#define DRAG_Y_MAX                      (SPRING_Y_MAX + 30)                                                     //拖动到顶部时的ofs_y
#define DRAG_Y_MIN                      (SPRING_Y_MIN - 78)                                                     //拖动到底部时的ofs_y

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 16                                                                      //拖动松手后的速度(惯性)

#define EXIT_Y_MIN                      0
#define EXIT_Y_MAX                      50//60                                                                      //下滑退出触摸点离屏幕顶部小于某值
#define TXT_CNT_MAX                     8                                                                       //文本框最大字符数
#define CARD_BTN_COUNT                  (COMPO_CARD_END - COMPO_CARD_START - 1)    //卡片（按钮）数量

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

#define DEFAULT_LATEST_TASK_NUM 5   //最近任务不足5个时默认值补上
// const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY,FUNC_WEATHER};
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT,FUNC_ADDRESS_BOOK, FUNC_HEARTRATE,FUNC_WEATHER};
///卡片、组件原始位置（相对于卡片中心点）
#define CARD_WIDTH_ORG                  232//308

typedef struct ui_handle_t_
{
    struct card_clock_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct date_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            u8 bonddata;
            bool center;
            color_t color;
            u32 res;
        } date;

        struct week_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            bool center;
            u8 bonddata;
            color_t color;
            u32 res;
        } week;

        struct clock_t
        {
            u16 id_bg;
            u16 id_h;
            u16 id_min;
            s16 bg_x,bg_y,h_x,h_y,m_x,m_y;
            u16 bg_w,bg_h,h_w,h_h,m_w,m_h;
            u32 bg_res, h_res, m_res;
            u8 h_bonddata, m_bonddata;
        } clock;
    } card_clock_day;

    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
            color_t color;
        } rect;

        struct pic_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
        } pic_kcal;
        struct pic_t pic_km;
        struct pic_t pic_step;

        struct sport_arc_t
        {
            u16 id;
            s16 x,y;
            u16 w,r;
            color_t color;
            color_t bg_color;
            u16 zero_angle;
            u16 angle_min;
            u16 angle_max;
        } arc_kcal;
        struct sport_arc_t arc_km;
        struct sport_arc_t arc_step;

        struct text_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u16 str_id;
            bool center;
            color_t color;
        } text_kcal;
        struct text_t text_km;
        struct text_t text_step;
    } card1;

    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_sleep;
        struct text_t text_hour;
        struct text_t text_hour_unit;
        struct text_t text_min;
        struct text_t text_min_unit;
    } card2;

    struct card3_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_music;

        struct pic_click_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u32 res_click;
            u32 res_switch;
            u16 func_sta;
        } pic_click_prev;
        struct pic_click_t pic_click_next;
        struct pic_click_t pic_click_play;
    } card3;

    struct card4_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_timer;
        struct text_t text_time;
    } card4;

    struct card5_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_stopwatch;
        struct text_t text_time;
    } card5;

    struct card6_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_comm_use;
        struct pic_click_t pic_click[3];
    } card6;

    struct card7_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_last_use;
        struct pic_click_t pic_click[4];
    } card7;

} ui_handle_t;
//组件ID
enum
{
    COMPO_ID_CLOCK_BG = 256,
    COMPO_ID_CLOCK_H,
    COMPO_ID_CLOCK_M,
    COMPO_ID_DATE,
    COMPO_ID_WEEKDAY,

    COMPO_CARD_START,
    COMPO_ID_CARD_1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    COMPO_ID_CARD_5,
    COMPO_ID_CARD_6,
    COMPO_ID_CARD_7,
    COMPO_CARD_END,

    COMPO_ID_ARC_KCAL,
    COMPO_ID_ARC_KM,
    COMPO_ID_ARC_STEP,

    COMPO_MUSIC_BTN_PREV,
    COMPO_MUSIC_BTN_NEXT,
    COMPO_MUSIC_BTN_PLAY,

    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
    COMPO_ID_APP4,
};

static const ui_handle_t ui_handle =
{
    .card_clock_day = {
        .id = COMPO_CARD_START,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 24+100/2,
        .w  = 0,
        .h  = 0,

        .date = {
            .id = COMPO_ID_DATE,
            .x  = 12-232/2,
            .y  = 41-136/2,
            .w  = 80+20,
            .h  = 40,
            .center = false,
            .bonddata = COMPO_BOND_DATE,
            .color = {255,255,255},
            .res = UI_BUF_0FONT_FONT_NUM_32_BIN,
        },

        .week = {
            .id = COMPO_ID_WEEKDAY,
            .x  = 12-232/2,
            .y  = 79-136/2,
            .w  = 80,
            .h  = 32,
            .center = false,
            .bonddata = COMPO_BOND_WEEKDAY,
            .color = {153,153,153},
            .res = UI_BUF_0FONT_FONT_BIN,
        },

        .clock = {
            .id_bg  = COMPO_ID_CLOCK_BG,
            .id_h   = COMPO_ID_CLOCK_H,
            .id_min = COMPO_ID_CLOCK_M,
            .bg_x   = 130+100/2-232/2,
            .bg_y   = 0,
            .h_x    = 130+96/2-232/2,
            .h_y    = 0,
            .m_x    = 130+96/2-232/2,
            .m_y    = 0,
            .bg_w   = 100,
            .bg_h   = 100,
            .h_w    = 53,
            .h_h    = 18,
            .m_w    = 65,
            .m_h    = 18,
            .bg_res = UI_BUF_I330001_FIRSTORDER_TIME_BG_BIN,
            .h_res  = UI_BUF_I330001_FIRSTORDER_TIME_H_BIN,
            .m_res  = UI_BUF_I330001_FIRSTORDER_TIME_M_BIN,
            .h_bonddata = COMPO_BOND_HOUR,
            .m_bonddata = COMPO_BOND_MINUTE,
        },
    },

    .card1 = {
        .id = COMPO_ID_CARD_1,
        .x  = 4+232/2,
        .y  = 190,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },
        .pic_kcal = {
            .idx    = 0,
            .x      = 10+18/2-232/2,
            .y      = 21+18/2-108/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_CALORIES_BIN,
        },
        .pic_km = {
            .idx    = 1,
            .x      = 10+18/2-232/2,
            .y      = 46+18/2-108/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_DIS_BIN,
        },
        .pic_step = {
            .idx    = 2,
            .x      = 10+18/2-232/2,
            .y      = 71+18/2-108/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_STEPT_BIN,
        },

        .arc_kcal = {
            .id     = COMPO_ID_ARC_KCAL,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100,
            .color  = {252,55,40},
            .bg_color = {0x7f,0x6e,0x19},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_km = {
            .id     = COMPO_ID_ARC_KM,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-12,
            .color  = {255,212,0},
            .bg_color = {0x19,0x7a,0x6f},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_step = {
            .id     = COMPO_ID_ARC_STEP,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-24,
            .color  = {0,242,214},
            .bg_color = {0x7e,0x2f,0x29},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .text_kcal = {
            .idx    = 0,
            .x      = 34-232/2,
            .y      = 21+18/2-108/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 0,
            .center = false,
            .color  = {252,55,40},
        },
        .text_km = {
            .idx    = 1,
            .x      = 34-232/2,
            .y      = 46+18/2-108/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 1,
            .center = false,
            .color  = {255,212,0},
        },
        .text_step  = {
            .idx    = 2,
            .x      = 34-232/2,
            .y      = 71+18/2-108/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 2,
            .center = false,
            .color  = {0,242,214},
        },
    },

    .card2 = {
        .id = COMPO_ID_CARD_2,
        .x  = 4+232/2,
        .y  = 190+(108+6)*1,
        .w  = 232,
        .h  = 108,
        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_sleep = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_SLEEP,
            .center = false,
            .color  = {100,95,255},
        },
        .text_hour = {
            .idx    = 1,
            .x      = 12-232/2,
            .y      = 38-108/2+6,
            .w      = 41+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_hour_unit = {
            .idx    = 2,
            .x      = 56-232/2,
            .y      = 53-108/2,
            .w      = 46,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_HOUR,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min = {
            .idx    = 3,
            .x      = 97-232/2+14,
            .y      = 38-108/2+6,
            .w      = 40+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min_unit = {
            .idx    = 4,
            .x      = 140-232/2+12,
            .y      = 53-108/2,
            .w      = 50,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MIN,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card3 = {
        .id = COMPO_ID_CARD_3,
        .x  = 4+232/2,
        .y  = 190+(108+6)*2,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_music = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MUSIC,
            .center = false,
            .color  = {255,48,96},
        },

        .pic_click_prev = {
            .idx    = COMPO_MUSIC_BTN_PREV,
            .x      = 26+28/2-232/2,
            .y      = 52+28/2-108/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_UP00_BIN,
            .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_UP01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_next = {
            .idx    = COMPO_MUSIC_BTN_NEXT,
            .x      = 178+28/2-232/2,
            .y      = 52+28/2-108/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_NEXT00_BIN,
            .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_NEXT01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_play = {
            .idx    = COMPO_MUSIC_BTN_PLAY,
            .x      = 88+56/2-232/2,
            .y      = 38+56/2-108/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY00_BIN,
            .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY01_BIN,
            .res_switch = UI_BUF_I330001_FIRSTORDER_MUSIC_PAUSED_BIN,
            .func_sta = 0,
        },
    },

    .card4 = {
        .id = COMPO_ID_CARD_4,
        .x  = 4+232/2,
        .y  = 190+(108+6)*3,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_timer = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_TIMER,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-232/2,
            .y      = 37-108/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card5 = {
        .id = COMPO_ID_CARD_5,
        .x  = 4+232/2,
        .y  = 190+(108+6)*4,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_stopwatch = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_STOP_WATCH,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-232/2,
            .y      = 37-108/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card6 = {
        .id = COMPO_ID_CARD_6,
        .x  = 4+232/2,
        .y  = 190+(108+6)*5,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_comm_use = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_COMMON_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = 0,
                .x      = 27+48/2-232/2,
                .y      = 41+48/2-108/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I330001_FIRSTORDER_FIXED_SPORT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },

            [1] = {
                .idx    = 1,
                .x      = 91+48/2-232/2,
                .y      = 41+48/2-108/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I330001_FIRSTORDER_FIXED_MSM_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
                .func_sta = FUNC_MESSAGE,
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            },

            [2] = {
                .idx    = 2,
                .x      = 155+48/2-232/2,
                .y      = 41+48/2-108/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I330001_FIRSTORDER_FIXED_CALL_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },

    .card7 = {
        .id = COMPO_ID_CARD_7,
        .x  = 4+232/2,
        .y  = 190+(108+6)*6+7,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_last_use = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_LATEST_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = COMPO_ID_APP1,
                .x      = 12+44/2-232/2,
                .y      = 43+44/2-108/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I330001_THEME_1_SOPRT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },
            [1] = {
                .idx    = COMPO_ID_APP2,
                .x      = 66+44/2-232/2,
                .y      = 43+44/2-108/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I330001_THEME_1_BRIGHTNESS_BIN,
                .res_click = 1,
                .res_switch = 1,
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
                .func_sta = FUNC_LIGHT,
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            },
            [2] = {
                .idx    = COMPO_ID_APP3,
                .x      = 120+44/2-232/2,
                .y      = 43+44/2-108/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I330001_THEME_1_TIMER_BIN,
                .res_click = 2,
                .res_switch = 2,
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
                .func_sta = FUNC_TIMER,
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            },
            [3] = {
                .idx    = COMPO_ID_APP4,
                .x      = 174+44/2-232/2,
                .y      = 43+44/2-108/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I330001_THEME_1_CALL_BIN,
                .res_click = 3,
                .res_switch = 3,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },
};

//获取卡片原始位置
static int func_clock_sub_card_get_y_info(int card_compo_id)
{
    int ret = 0;

    if (card_compo_id == ui_handle.card_clock_day.id)
    {
        ret = ui_handle.card_clock_day.y;
    }

    if (card_compo_id == ui_handle.card1.id)
    {
        ret = ui_handle.card1.y;
    }
    if (card_compo_id == ui_handle.card2.id)
    {
        ret = ui_handle.card2.y;
    }
    if (card_compo_id == ui_handle.card3.id)
    {
        ret = ui_handle.card3.y;
    }
    if (card_compo_id == ui_handle.card4.id)
    {
        ret = ui_handle.card4.y;
    }
    if (card_compo_id == ui_handle.card5.id)
    {
        ret = ui_handle.card5.y;
    }
    if (card_compo_id == ui_handle.card6.id)
    {
        ret = ui_handle.card6.y;
    }
    if (card_compo_id == ui_handle.card7.id)
    {
        ret = ui_handle.card7.y;
    }
    return ret;
}

//功能结构体
typedef struct f_card_t_
{
    s32 ofs_y;
    s32 ofs_y_drag;
    s16 focus_card;                  //靠近屏幕中央的卡片序号
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    s32 moveto_y;                   //设定自动移到的坐标
    s16 moveto_card;                //设定自动移到的卡片序号
    u32 tick;
    u32 activity_tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
//    co_timer_t music_refresh;
} f_card_t;

static void card1_updata_disp(void)
{
    f_card_t *f_activity = (f_card_t *)func_cb.f_cb;

//    if(tick_check_expire(f_activity->activity_tick, ))
//    {
//        f_activity->activity_tick = tick_get();
    char txt_buf[20];
    uint32_t totalStepCnt = 0;
    uint32_t Step_value = 0;
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
#endif
    Step_value = totalStepCnt;
#if UTE_MODULE_SPORT_SUPPORT
    if(Step_value>uteModuleSportGetStepsTargetCnt())
    {
        Step_value = uteModuleSportGetStepsTargetCnt();
    }
#endif
    compo_picturebox_t *arc_kcal = compo_getobj_byid(COMPO_ID_ARC_KCAL);
    compo_picturebox_t *arc_km   = compo_getobj_byid(COMPO_ID_ARC_KM);
    compo_picturebox_t *arc_step = compo_getobj_byid(COMPO_ID_ARC_STEP);
    compo_cardbox_t *cardbox = compo_getobj_byid(ui_handle.card1.id);

    compo_picturebox_set_visible(arc_kcal, true);
    compo_picturebox_set_visible(arc_km, true);
    compo_picturebox_set_visible(arc_step, true);


    if(f_activity->activity_state == 0)
    {
        f_activity->arc_step_value+=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value >=1800)
        {
            f_activity->activity_state =1;
        }
    }
    else if(f_activity->activity_state == 1)
    {

        f_activity->arc_step_value-=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );
#if UTE_MODULE_SPORT_SUPPORT
        if(f_activity->arc_step_value <= Step_value*1800 / uteModuleSportGetStepsTargetCnt())
        {
            f_activity->activity_state = 2;
        }
#endif

    }
    else
    {
#if UTE_MODULE_SPORT_SUPPORT
        f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
        f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
        f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());
#else
        f_activity->arc_kcal_value =0;
        f_activity->arc_km_value   =0;
        f_activity->arc_step_value =0;

#endif

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
        compo_cardbox_text_set(cardbox, 2, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
        compo_cardbox_text_set(cardbox, 1, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
        compo_cardbox_text_set(cardbox, 0, txt_buf);


        if(f_activity->arc_step_value)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);
        }
        else
        {
            compo_picturebox_set_visible(arc_kcal,false);
            compo_picturebox_set_visible(arc_km,false);
            compo_picturebox_set_visible(arc_step,false);
        }
    }

//    }
}
//根据序号获取最近任务序号（idx=0为最近，无任务返回0）(idx<=3)
static u8 card_get_latest_func(u8 idx)
{
    u8 i, j;
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};
    u8 latest_cnt = latest_task_count();
    if (latest_cnt > idx)
    {
        return latest_task_get(idx);
    }
    else
    {
        j = 0;
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)     //最近任务不足DEFAULT_LATEST_TASK_NUM个且包含默认值
        {
            if (latest_task_find(last_default[i]) == -1)
            {
                latest_default_tmp[j] = last_default[i];
                j++;
            }
        }
        return latest_default_tmp[idx - latest_cnt];
    }
}

//根据序号获取最近任务图标资源地址
static u32 card_get_latest_icon(u8 idx)
{
    return func_get_icon_addr(card_get_latest_func(idx));
}

//创建组件
static void func_clock_sub_card_compo_create(compo_form_t *frm)
{
    char txt_buf[20];
    compo_button_t *btn=NULL;

    ///卡片7
    compo_cardbox_t* card7 = compo_cardbox_create(frm, 1, 4, 1, ui_handle.card7.w, ui_handle.card7.h);
    compo_setid(card7, ui_handle.card7.id);

    compo_cardbox_rect_set_location(card7, ui_handle.card7.rect.idx, ui_handle.card7.rect.x, ui_handle.card7.rect.y, ui_handle.card7.rect.w, ui_handle.card7.rect.h, ui_handle.card7.rect.r);
    compo_cardbox_rect_set_color(card7, ui_handle.card7.rect.idx, make_color(ui_handle.card7.rect.color.r, ui_handle.card7.rect.color.g, ui_handle.card7.rect.color.b));

    compo_cardbox_text_set_font(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.res);
    compo_cardbox_text_set_align_center(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.center);
    widget_text_set_color(card7->text[ui_handle.card7.text_last_use.idx], make_color(ui_handle.card7.text_last_use.color.r, ui_handle.card7.text_last_use.color.g, ui_handle.card7.text_last_use.color.b));
    compo_cardbox_text_set_location(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.x, ui_handle.card7.text_last_use.y, ui_handle.card7.text_last_use.w, ui_handle.card7.text_last_use.h);
    compo_cardbox_text_set(card7, ui_handle.card7.text_last_use.idx, i18n[ui_handle.card7.text_last_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card7.pic_click)/sizeof(ui_handle.card7.pic_click[0]); i++)
    {
        btn = compo_button_create_page_by_image(frm,card7->page,card_get_latest_icon(i));
        compo_button_set_location(btn,ui_handle.card7.pic_click[i].x,ui_handle.card7.pic_click[i].y, ui_handle.card7.pic_click[i].w, ui_handle.card7.pic_click[i].h);
        compo_setid(btn,ui_handle.card7.pic_click[i].idx);
    }

    ///卡片6
    compo_cardbox_t* card6 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card6.w, ui_handle.card6.h);
    compo_setid(card6, ui_handle.card6.id);

    compo_cardbox_rect_set_location(card6, ui_handle.card6.rect.idx, ui_handle.card6.rect.x, ui_handle.card6.rect.y, ui_handle.card6.rect.w, ui_handle.card6.rect.h, ui_handle.card6.rect.r);
    compo_cardbox_rect_set_color(card6, ui_handle.card6.rect.idx, make_color(ui_handle.card6.rect.color.r, ui_handle.card6.rect.color.g, ui_handle.card6.rect.color.b));

    compo_cardbox_text_set_font(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.res);
    compo_cardbox_text_set_align_center(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.center);
    widget_text_set_color(card6->text[ui_handle.card6.text_comm_use.idx], make_color(ui_handle.card6.text_comm_use.color.r, ui_handle.card6.text_comm_use.color.g, ui_handle.card6.text_comm_use.color.b));
    compo_cardbox_text_set_location(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.x, ui_handle.card6.text_comm_use.y, ui_handle.card6.text_comm_use.w, ui_handle.card6.text_comm_use.h);
    compo_cardbox_text_set(card6, ui_handle.card6.text_comm_use.idx, i18n[ui_handle.card6.text_comm_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
    {
        compo_cardbox_icon_set_location(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].x, ui_handle.card6.pic_click[i].y, ui_handle.card6.pic_click[i].w, ui_handle.card6.pic_click[i].h);
        compo_cardbox_icon_set(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].res);
    }


    ///卡片5
    compo_cardbox_t* card5 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card5.w, ui_handle.card5.h);
    compo_setid(card5, ui_handle.card5.id);

    compo_cardbox_rect_set_location(card5, ui_handle.card5.rect.idx, ui_handle.card5.rect.x, ui_handle.card5.rect.y, ui_handle.card5.rect.w, ui_handle.card5.rect.h, ui_handle.card5.rect.r);
    compo_cardbox_rect_set_color(card5, ui_handle.card5.rect.idx, make_color(ui_handle.card5.rect.color.r, ui_handle.card5.rect.color.g, ui_handle.card5.rect.color.b));

//    compo_cardbox_text_set_font(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_stopwatch.idx], make_color(ui_handle.card5.text_stopwatch.color.r, ui_handle.card5.text_stopwatch.color.g, ui_handle.card5.text_stopwatch.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.x, ui_handle.card5.text_stopwatch.y, ui_handle.card5.text_stopwatch.w, ui_handle.card5.text_stopwatch.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_stopwatch.idx, i18n[ui_handle.card5.text_stopwatch.str_id]);

    compo_cardbox_text_set_font(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_time.idx], make_color(ui_handle.card5.text_time.color.r, ui_handle.card5.text_time.color.g, ui_handle.card5.text_time.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.x, ui_handle.card5.text_time.y, ui_handle.card5.text_time.w, ui_handle.card5.text_time.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_time.idx, "00:00.00");

    ///卡片4
    compo_cardbox_t* card4 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card4.w, ui_handle.card4.h);
    compo_setid(card4, ui_handle.card4.id);

    compo_cardbox_rect_set_location(card4, ui_handle.card4.rect.idx, ui_handle.card4.rect.x, ui_handle.card4.rect.y, ui_handle.card4.rect.w, ui_handle.card4.rect.h, ui_handle.card4.rect.r);
    compo_cardbox_rect_set_color(card4, ui_handle.card4.rect.idx, make_color(ui_handle.card4.rect.color.r, ui_handle.card4.rect.color.g, ui_handle.card4.rect.color.b));

    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_timer.idx], make_color(ui_handle.card4.text_timer.color.r, ui_handle.card4.text_timer.color.g, ui_handle.card4.text_timer.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.x, ui_handle.card4.text_timer.y, ui_handle.card4.text_timer.w, ui_handle.card4.text_timer.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_timer.idx, i18n[ui_handle.card4.text_timer.str_id]);

    compo_cardbox_text_set_font(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.res);
    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_time.idx], make_color(ui_handle.card4.text_time.color.r, ui_handle.card4.text_time.color.g, ui_handle.card4.text_time.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.x, ui_handle.card4.text_time.y, ui_handle.card4.text_time.w, ui_handle.card4.text_time.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_time.idx, "00:00:00");


    ///卡片3
    compo_cardbox_t* card3 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card3.w, ui_handle.card3.h);
    compo_setid(card3, ui_handle.card3.id);

    compo_cardbox_rect_set_location(card3, ui_handle.card3.rect.idx, ui_handle.card3.rect.x, ui_handle.card3.rect.y, ui_handle.card3.rect.w, ui_handle.card3.rect.h, ui_handle.card3.rect.r);
    compo_cardbox_rect_set_color(card3, ui_handle.card3.rect.idx, make_color(ui_handle.card3.rect.color.r, ui_handle.card3.rect.color.g, ui_handle.card3.rect.color.b));

    compo_cardbox_text_set_font(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.res);
    compo_cardbox_text_set_align_center(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.center);
    widget_text_set_color(card3->text[ui_handle.card3.text_music.idx], make_color(ui_handle.card3.text_music.color.r, ui_handle.card3.text_music.color.g, ui_handle.card3.text_music.color.b));
    compo_cardbox_text_set_location(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.x, ui_handle.card3.text_music.y, ui_handle.card3.text_music.w, ui_handle.card3.text_music.h);
    compo_cardbox_text_set(card3, ui_handle.card3.text_music.idx, i18n[ui_handle.card3.text_music.str_id]);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_prev.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_prev.x, ui_handle.card3.pic_click_prev.y, ui_handle.card3.pic_click_prev.w, ui_handle.card3.pic_click_prev.h);
    compo_setid(btn,ui_handle.card3.pic_click_prev.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_next.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_next.x, ui_handle.card3.pic_click_next.y, ui_handle.card3.pic_click_next.w, ui_handle.card3.pic_click_next.h);
    compo_setid(btn,ui_handle.card3.pic_click_next.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_play.res_click);
    compo_button_set_pos(btn,ui_handle.card3.pic_click_play.x, ui_handle.card3.pic_click_play.y);
    compo_setid(btn,ui_handle.card3.pic_click_play.idx);

    ///卡片2
    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);
    compo_cardbox_t* card2 = compo_cardbox_create(frm, 1, 0, 5, ui_handle.card2.w, ui_handle.card2.h);
    compo_setid(card2, ui_handle.card2.id);

    compo_cardbox_rect_set_location(card2, ui_handle.card2.rect.idx, ui_handle.card2.rect.x, ui_handle.card2.rect.y, ui_handle.card2.rect.w, ui_handle.card2.rect.h, ui_handle.card2.rect.r);
    compo_cardbox_rect_set_color(card2, ui_handle.card2.rect.idx, make_color(ui_handle.card2.rect.color.r, ui_handle.card2.rect.color.g, ui_handle.card2.rect.color.b));

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_sleep.idx], make_color(ui_handle.card2.text_sleep.color.r, ui_handle.card2.text_sleep.color.g, ui_handle.card2.text_sleep.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.x, ui_handle.card2.text_sleep.y, ui_handle.card2.text_sleep.w, ui_handle.card2.text_sleep.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_sleep.idx, i18n[ui_handle.card2.text_sleep.str_id]);

//    sleep_data->totalSleepMin =  9;
    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour.idx], make_color(ui_handle.card2.text_hour.color.r, ui_handle.card2.text_hour.color.g, ui_handle.card2.text_hour.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.x, ui_handle.card2.text_hour.y, ui_handle.card2.text_hour.w, ui_handle.card2.text_hour.h);
    memset(txt_buf,0,sizeof(txt_buf));
    if(sleep_data->totalSleepMin)   ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠小时*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour_unit.idx], make_color(ui_handle.card2.text_hour_unit.color.r, ui_handle.card2.text_hour_unit.color.g, ui_handle.card2.text_hour_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.x+6, ui_handle.card2.text_hour_unit.y, ui_handle.card2.text_hour_unit.w, ui_handle.card2.text_hour_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour_unit.idx, i18n[ui_handle.card2.text_hour_unit.str_id]);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min.idx], make_color(ui_handle.card2.text_min.color.r, ui_handle.card2.text_min.color.g, ui_handle.card2.text_min.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.x, ui_handle.card2.text_min.y, ui_handle.card2.text_min.w, ui_handle.card2.text_min.h);
    if(sleep_data->totalSleepMin)  ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠分钟*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_min.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min_unit.idx], make_color(ui_handle.card2.text_min_unit.color.r, ui_handle.card2.text_min_unit.color.g, ui_handle.card2.text_min_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.x+6, ui_handle.card2.text_min_unit.y, ui_handle.card2.text_min_unit.w, ui_handle.card2.text_min_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_min_unit.idx, i18n[ui_handle.card2.text_min_unit.str_id]);

    ///卡片1
    compo_cardbox_t* card1 = compo_cardbox_create(frm, 1, 3, 3, ui_handle.card1.w, ui_handle.card1.h);
    compo_setid(card1, ui_handle.card1.id);

    compo_cardbox_rect_set_location(card1, ui_handle.card1.rect.idx, ui_handle.card1.rect.x, ui_handle.card1.rect.y, ui_handle.card1.rect.w, ui_handle.card1.rect.h, ui_handle.card1.rect.r);
    compo_cardbox_rect_set_color(card1, ui_handle.card1.rect.idx, make_color(ui_handle.card1.rect.color.r, ui_handle.card1.rect.color.g, ui_handle.card1.rect.color.b));

    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.x, ui_handle.card1.pic_kcal.y, ui_handle.card1.pic_kcal.w, ui_handle.card1.pic_kcal.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.x, ui_handle.card1.pic_km.y, ui_handle.card1.pic_km.w, ui_handle.card1.pic_km.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.x, ui_handle.card1.pic_step.y, ui_handle.card1.pic_step.w, ui_handle.card1.pic_step.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.res);

#if UTE_MODULE_SPORT_SUPPORT
    uint16_t KM = uteModuleSportGetCurrDayDistanceData();
#else
    uint16_t KM = 0;
#endif
    uint32_t totalStepCnt = 0;
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
#endif
    compo_cardbox_text_set_font(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_kcal.idx], make_color(ui_handle.card1.text_kcal.color.r, ui_handle.card1.text_kcal.color.g, ui_handle.card1.text_kcal.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.x, ui_handle.card1.text_kcal.y, ui_handle.card1.text_kcal.w, ui_handle.card1.text_kcal.h);
    memset(txt_buf,0,sizeof(txt_buf));
#if UTE_MODULE_SPORT_SUPPORT
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
#else
    snprintf(txt_buf,sizeof(txt_buf),"%d",0);
#endif
    compo_cardbox_text_set(card1, ui_handle.card1.text_kcal.idx, txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_km.idx], make_color(ui_handle.card1.text_km.color.r, ui_handle.card1.text_km.color.g, ui_handle.card1.text_km.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.x, ui_handle.card1.text_km.y, ui_handle.card1.text_km.w, ui_handle.card1.text_km.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",KM/100, KM%100);///公里数据
    compo_cardbox_text_set(card1, ui_handle.card1.text_km.idx,txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_step.idx], make_color(ui_handle.card1.text_step.color.r, ui_handle.card1.text_step.color.g, ui_handle.card1.text_step.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.x, ui_handle.card1.text_step.y, ui_handle.card1.text_step.w, ui_handle.card1.text_step.h);
    snprintf(txt_buf,sizeof(txt_buf),"%ld",totalStepCnt);
    compo_cardbox_text_set(card1, ui_handle.card1.text_step.idx, txt_buf);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I330001_FIRSTORDER_ACTIVITY_BG_BIN);
    widget_page_t * widget_page = widget_page_create(card1->page);///创建按键页面
    widget_set_location(widget_page,52,0,pic_bg_area.wid,pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_BG_BIN);//彩虹背景
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_RED_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,50,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KCAL);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_YELLOW_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,50,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KM);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_BLUE_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,50,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_STEP);


    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_YUANJIAO_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,52,pic_bg_area.hei-gui_image_get_size(UI_BUF_I330001_FIRSTORDER_ACTIVITY_YUANJIAO_BIN).hei );

    ///时钟 日期
    compo_picturebox_t* clock_bg = compo_picturebox_create(frm, ui_handle.card_clock_day.clock.bg_res);
    compo_setid(clock_bg, ui_handle.card_clock_day.clock.id_bg);

    compo_datetime_t* clock_h = compo_datetime_create(frm, ui_handle.card_clock_day.clock.h_res);
    compo_setid(clock_h, ui_handle.card_clock_day.clock.id_h);
    compo_datetime_set_center(clock_h, 13, ui_handle.card_clock_day.clock.h_h / 2);
    compo_datetime_set_start_angle(clock_h, 900);
    compo_bonddata(clock_h, ui_handle.card_clock_day.clock.h_bonddata);

    compo_datetime_t* clock_m = compo_datetime_create(frm, ui_handle.card_clock_day.clock.m_res);
    compo_setid(clock_m, ui_handle.card_clock_day.clock.id_min);
    compo_datetime_set_center(clock_m, 13, ui_handle.card_clock_day.clock.m_h / 2);
    compo_datetime_set_start_angle(clock_m, 900);
    compo_bonddata(clock_m, ui_handle.card_clock_day.clock.m_bonddata);

    compo_textbox_t* date = compo_textbox_create(frm, 5);                                 //日期
    compo_setid(date, ui_handle.card_clock_day.date.id);
    compo_textbox_set_font(date, ui_handle.card_clock_day.date.res);
    compo_textbox_set_align_center(date, ui_handle.card_clock_day.date.center);
    widget_text_set_color(date->txt, make_color(ui_handle.card_clock_day.date.color.r, ui_handle.card_clock_day.date.color.g, ui_handle.card_clock_day.date.color.b));
    compo_bonddata(date, ui_handle.card_clock_day.date.bonddata);

    compo_textbox_t* week = compo_textbox_create(frm, 20);                                 //日期
    compo_setid(week, ui_handle.card_clock_day.week.id);
    compo_textbox_set_font(week, ui_handle.card_clock_day.week.res);
    compo_textbox_set_align_center(week, ui_handle.card_clock_day.week.center);
    widget_text_set_color(week->txt, make_color(ui_handle.card_clock_day.week.color.r, ui_handle.card_clock_day.week.color.g, ui_handle.card_clock_day.week.color.b));
    compo_bonddata(week, ui_handle.card_clock_day.week.bonddata);

//    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 6, 36, 6);
//    compo_shape_set_color(shape, make_color(0X4C,0X4C,0X4C));
//    compo_shape_set_radius(shape, 35);

    ab_free(sleep_data);

}

//更新组件位置和大小
static void func_clock_sub_card_compo_update(s32 ofs_y, bool creating)
{
    s16 x, y, w, h, card_y, card_w;
    u8 card_id = 0;
    u16 dc;  //当前卡片中心距屏幕中心距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL)
    {
        //获取初始值
        switch (compo->id)
        {
            case COMPO_ID_CLOCK_BG:
                if (ui_handle.card_clock_day.clock.id_bg != COMPO_ID_CLOCK_BG)
                {
                    printf("COMPO_ID_CLOCK_BG ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.bg_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.bg_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.bg_w;
                h = ui_handle.card_clock_day.clock.bg_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_H:
                if (ui_handle.card_clock_day.clock.id_h != COMPO_ID_CLOCK_H)
                {
                    printf("COMPO_ID_CLOCK_H ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.h_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.h_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.h_w;
                h = ui_handle.card_clock_day.clock.h_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_M:
                if (ui_handle.card_clock_day.clock.id_min != COMPO_ID_CLOCK_M)
                {
                    printf("COMPO_ID_CLOCK_M ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.m_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.m_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.m_w;
                h = ui_handle.card_clock_day.clock.m_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_DATE:
                if (ui_handle.card_clock_day.date.id != COMPO_ID_DATE)
                {
                    printf("COMPO_ID_DATE ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.date.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.date.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.date.w;
                h = ui_handle.card_clock_day.date.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_WEEKDAY:
                if (ui_handle.card_clock_day.week.id != COMPO_ID_WEEKDAY)
                {
                    printf("COMPO_ID_WEEKDAY ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.week.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.week.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.week.w;
                h = ui_handle.card_clock_day.week.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CARD_1:
                if (ui_handle.card1.id != COMPO_ID_CARD_1)
                {
                    printf("COMPO_ID_CARD_1 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card1.x;
                y = card_y = ui_handle.card1.y;
                w = ui_handle.card1.w;
                h = ui_handle.card1.h;
                card_id = ui_handle.card1.id;
                break;

            case COMPO_ID_CARD_2:
                if (ui_handle.card2.id != COMPO_ID_CARD_2)
                {
                    printf("COMPO_ID_CARD_2 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card2.x;
                y = card_y = ui_handle.card2.y;
                w = ui_handle.card2.w;
                h = ui_handle.card2.h;
                card_id = ui_handle.card2.id;
                break;

            case COMPO_ID_CARD_3:
                if (ui_handle.card3.id != COMPO_ID_CARD_3)
                {
                    printf("COMPO_ID_CARD_3 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card3.x;
                y = card_y = ui_handle.card3.y;
                w = ui_handle.card3.w;
                h = ui_handle.card3.h;
                card_id = ui_handle.card3.id;
                break;

            case COMPO_ID_CARD_4:
                if (ui_handle.card4.id != COMPO_ID_CARD_4)
                {
                    printf("COMPO_ID_CARD_4 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card4.x;
                y = card_y = ui_handle.card4.y;
                w = ui_handle.card4.w;
                h = ui_handle.card4.h;
                card_id = ui_handle.card4.id;
                break;

            case COMPO_ID_CARD_5:
                if (ui_handle.card5.id != COMPO_ID_CARD_5)
                {
                    printf("COMPO_ID_CARD_5 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card5.x;
                y = card_y = ui_handle.card5.y;
                w = ui_handle.card5.w;
                h = ui_handle.card5.h;
                card_id = ui_handle.card5.id;
                break;


            case COMPO_ID_CARD_6:
                if (ui_handle.card6.id != COMPO_ID_CARD_6)
                {
                    printf("COMPO_ID_CARD_6 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card6.x;
                y = card_y = ui_handle.card6.y;
                w = ui_handle.card6.w;
                h = ui_handle.card6.h;
                card_id = ui_handle.card6.id;
                break;


            case COMPO_ID_CARD_7:
                if (ui_handle.card7.id != COMPO_ID_CARD_7)
                {
                    printf("COMPO_ID_CARD_7 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card7.x;
                y = card_y = ui_handle.card7.y;
                w = ui_handle.card7.w;
                h = ui_handle.card7.h;
                card_id = ui_handle.card7.id;
                break;

            default:
                compo = compo_get_next(compo);
                continue;
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH_ORG;
        card_y += ofs_y;
        y += ofs_y;
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);
        if (creating == false && dc < dc_min)    //焦点，创建时不使用f_cb
        {
            f_card_t *f_card = (f_card_t *)func_cb.f_cb;
            f_card->focus_card = card_id;
            dc_min = dc;
//            TRACE("focus[%d]\n", card_id);
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM)
        {
            if (card_y > CARD_SCALE_STOP_Y)    //缩到最小后保持一段时间
            {
                y -= card_y - CARD_SCALE_STOP_Y;
                card_y = CARD_SCALE_STOP_Y;
            }
            if (card_w > CARD_WIDTH_MIN)
            {
                //缩放
                s16 card_w_org = card_w;
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                w = w * card_w / card_w_org;
                h = h * card_w / card_w_org;
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org;
                y += (card_y - y) * (card_w_org - card_w) / card_w_org;
                //偏移
                s16 card_y_org = card_y;
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                y += card_y - card_y_org;
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM;
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level));   //背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level));
            if (unvisible)
            {
                compo_cardbox_set_visible(cardbox, false);
            }
            else
            {
                compo_cardbox_set_visible(cardbox, true);
            }

//            if (card_id > COMPO_CARD_END-1)      //被覆盖的卡片不可见，降低element溢出概率
//            {
//                rect_t rect = compo_cardbox_get_location((compo_cardbox_t *)compo_getobj_byid(COMPO_ID_CARD_SPORT_COMPASS + (card_id - CARD_ID_SPORT_COMPASS) - 1));
//                if (y <= rect.y)
//                {
//                    compo_cardbox_set_visible(cardbox, false);
//                }
//                else
//                {
//                    compo_cardbox_set_visible(cardbox, true);
//                }
//            }

        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_set_pos(pic, x, y);
            // compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_picturebox_set_visible(pic, false);
            }
            else
            {
                compo_picturebox_set_visible(pic, true);
            }
        }
        else if (compo->type == COMPO_TYPE_TEXTBOX)
        {
            compo_textbox_t *txt = (compo_textbox_t *)compo;
            compo_textbox_set_location(txt, x, y, w, h);
            if (unvisible)
            {
                compo_textbox_set_visible(txt, false);
            }
            else
            {
                compo_textbox_set_visible(txt, true);
            }
        }
        else if (compo->type == COMPO_TYPE_DATETIME)
        {
            compo_datetime_t *dt = (compo_datetime_t *)compo;
            compo_datetime_set_pos(dt, x, y);
        }

        compo = compo_get_next(compo);          //遍历组件
    }
}

//创建上拉卡片菜单（文件内调用）
static compo_form_t *func_clock_sub_card_form_create_by_ofs(s16 ofs_y)
{
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
//    compo_shape_set_alpha(masklayer, 160);
    //创建所有组件
    func_clock_sub_card_compo_create(frm);
    //更新组件位置和大小
    func_clock_sub_card_compo_update(ofs_y, true);

    return frm;
}

//创建上拉卡片菜单（供外部调用）
compo_form_t *func_clock_sub_card_form_create(void)
{
    return func_clock_sub_card_form_create_by_ofs(SPRING_Y_MAX);
}

//获取点击的卡片组件id
static u16 func_clock_sub_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_BTN_COUNT; i++)
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;
        id = COMPO_CARD_START + 1 + i;
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

//表盘上拉卡片菜单点击处理
static void func_clock_sub_card_click_handler(void)
{
    compo_cardbox_t *cardbox = NULL;
//    rect_t rect;
    u8 func_jump = FUNC_NULL;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_clock_sub_card_get_btn_id(pt);
    printf("click compo_id:%d\n", compo_id);
    int id = compo_get_button_id();

    switch(compo_id)
    {
        case COMPO_ID_CARD_1 :
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            func_jump = FUNC_ACTIVITY;
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            break;
        case COMPO_ID_CARD_2 :
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
            func_jump = FUNC_SLEEP;
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
            break;
        case COMPO_ID_CARD_3:
            switch(id)
            {
                case COMPO_MUSIC_BTN_PREV:
                {
                    uteModuleMusicCtrl(false,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV);
                    compo_button_set_bgimg(btn,ui_handle.card3.pic_click_prev.res);
                }
                break;
                case COMPO_MUSIC_BTN_PLAY:
                {
                    uteModuleMusicCtrlPaused(false);
                    if(ble_is_connect())
                    {
                        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
                    }
//                    if(bt_cb.music_playing)
//                    {
//                        compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY);
//                        compo_button_set_bgimg(btn, ui_handle.card3.pic_click_play.res_click);
//                    }
                }
                break;
                case COMPO_MUSIC_BTN_NEXT:
                {
                    uteModuleMusicCtrl(true,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);
                    compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res);
                }
                break;
                default:
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    func_jump = FUNC_BT;
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    break;
            }
            break;
        case COMPO_ID_CARD_4 :
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
            func_jump = FUNC_TIMER;
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            break;
        case COMPO_ID_CARD_5 :
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            func_jump = FUNC_STOPWATCH;
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            break;
        case COMPO_ID_CARD_6 :
            cardbox = compo_getobj_byid(COMPO_ID_CARD_6);
            for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
            {
                rect_t rect = compo_cardbox_get_icon_absolute(cardbox, ui_handle.card6.pic_click[i].idx); //上一首
                if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
                {
//                    printf("rect = [%d,%d,%d,%d]\n", rect.x, rect.y, rect.wid, rect.hei);
                    func_cb.sta = ui_handle.card6.pic_click[i].func_sta;
                }
            }
            break;
        case COMPO_ID_CARD_7 :
            printf("id : %d\n", id);
            if (id >= COMPO_ID_APP1)
            {
                func_jump = card_get_latest_func(id-COMPO_ID_APP1);
            }
            break;
        default:
            break;
    }
    if(func_jump != FUNC_NULL)
    {
        printf("func_jump=%d\n", func_jump);
        func_cb.sta = func_jump;  //直接跳转
    }
}

//根据偏移位置把最近的卡片对齐到屏幕中央
static s16 func_clock_sub_card_align_y(s16 ofs_y)
{
    u8 i, nearest_id = 0;
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    u16 dc;
    if (ofs_y >= DRAG_Y_MAX)
    {
        return DRAG_Y_MAX;  //回弹效果
    }
    else if (ofs_y <= DRAG_Y_MIN)
    {
        return DRAG_Y_MIN;
    }
//    for (i=0; i<CARD_COUNT; i++)
    for (i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
//        dc = abs_s(card_y_info[i] + ofs_y - GUI_SCREEN_CENTER_Y);
        dc = abs_s(func_clock_sub_card_get_y_info(COMPO_CARD_START+1+i) + ofs_y - GUI_SCREEN_CENTER_Y);
        if (dc < dc_min)
        {
            nearest_id = i;
            dc_min = dc;
        }
    }
//    return (GUI_SCREEN_CENTER_Y - card_y_info[nearest_id]);
    return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+nearest_id));
}

//根据卡片序号对齐页面偏移位置到屏幕中央
static s16 func_clock_sub_card_align_by_idx(s8 card_id)
{
//    if (card_id <= CARD_ID_CLOCK)
    if (card_id <= COMPO_CARD_START)
    {
        return DRAG_Y_MAX;
    }
//    else if (card_id > (CARD_COUNT - 1))
    else if (card_id > (COMPO_CARD_END-COMPO_CARD_START-1 - 1))
    {
        return DRAG_Y_MIN;
    }
    else
    {
//        return (GUI_SCREEN_CENTER_Y - card_y_info[card_id]);
        return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+card_id));
    }
}

//限制滑动偏移位置
static s16 func_clock_sub_card_limit_y(s16 ofs_y)
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));
    return ofs_y;
}

//设置页面偏移位置
static void func_clock_sub_card_set_offs(s16 ofs_y)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    f_card->ofs_y = ofs_y;
    func_clock_sub_card_compo_update(f_card->ofs_y, false);
}

//时钟表盘上拉菜单数据更新----------------------------------------->todo
static void func_clock_sub_card_data_update(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    compo_cardbox_t *cardbox = NULL;
    char txt_buf[20];

    if(f_card->flag_drag == 0)
    {
        card1_updata_disp();
    }

//    if(f_activity->activity_state != 2) return;

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",SEC_TO_HOUR(sys_cb.timer_left_sec),SEC_TO_MIN(sys_cb.timer_left_sec),SEC_TO_SEC(sys_cb.timer_left_sec));
    cardbox = compo_getobj_byid(ui_handle.card4.id);
    compo_cardbox_text_set(cardbox, ui_handle.card4.text_time.idx,txt_buf);

    u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    u16 msec = sys_cb.stopwatch_total_msec % 1000;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min  = 99;
        sec  = 59;
        msec = 999;
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld.%02ld",min,sec,msec/10);
    cardbox = compo_getobj_byid(ui_handle.card5.id);
    compo_cardbox_text_set(cardbox, ui_handle.card5.text_time.idx,txt_buf);
}

//下拉返回表盘
static void func_clock_sub_card_switch_to_clock(bool auto_switch)
{
    printf("%s\n", __func__);
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    u16 switch_mode = FUNC_SWITCH_MENU_PULLUP_DOWN | (auto_switch ? FUNC_SWITCH_AUTO : 0);
    compo_form_destroy(func_cb.frm_main);
    compo_form_t * frm_clock = func_create_form(FUNC_CLOCK);
    compo_form_t * frm = func_clock_sub_card_form_create_by_ofs(f_card->ofs_y);
    func_cb.frm_main = frm;
    if (func_switching(switch_mode, NULL))
    {
        func_cb.sta = FUNC_CLOCK;
    }
    compo_form_destroy(frm_clock);
}

void music_data_refresh(void)
{
    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }

    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY); //播放/暂停
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I330001_FIRSTORDER_MUSIC_PAUSED_BIN : ui_handle.card3.pic_click_play.res);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_play.res_click);
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define CARD_SCALE_START_POS            228//265                                                                     //卡片开始缩小位置
#define CARD_SCALE_STOP_POS             94+236//320                                                                     //卡片停止缩小位置
#define CARD_SCALE_STOP_Y               (CARD_SCALE_STOP_POS + 200 * 2)                                         //卡片停止缩小实际位置（越大缩小越慢，可见叠加越多）
#define CARD_WIDTH_MIN                  150//200                                                                     //卡片缩小的最小宽度
#define CARD_UNVISIBLE_POS_TOP          (-100)//(-160)                                                                  //卡片超出屏幕顶部某位置后设为不可见
#define CARD_UNVISIBLE_POS_BOTTOM       (CARD_SCALE_STOP_Y + 200)                                               //卡片缩到最小后实际位置大于某值设为不可见
#define CARD_BG_GRAY_START              41                                                                      //卡片开始缩小位置处背景的灰度
#define CARD_BG_GRAY_STOP               10                                                                      //卡片停止缩小位置处背景的灰度
#define GRAY_LV(x)                      make_color(x, x, x)                                                     //生成灰度颜色值（0:黑 255:白）

#define SPRING_Y_MAX                    0                                                                       //回弹到顶部时的ofs_y
//#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - card_y_info[CARD_COUNT - 1] + 50)                     //回弹到底部时的ofs_y
#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_END-1) + 50)                     //回弹到底部时的ofs_y
#define DRAG_Y_MAX                      (SPRING_Y_MAX + 30)                                                     //拖动到顶部时的ofs_y
#define DRAG_Y_MIN                      (SPRING_Y_MIN - 78)                                                     //拖动到底部时的ofs_y

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 16                                                                      //拖动松手后的速度(惯性)

#define EXIT_Y_MIN                      0
#define EXIT_Y_MAX                      50//60                                                                      //下滑退出触摸点离屏幕顶部小于某值
#define TXT_CNT_MAX                     8                                                                       //文本框最大字符数
#define CARD_BTN_COUNT                  (COMPO_CARD_END - COMPO_CARD_START - 1)    //卡片（按钮）数量

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

#define DEFAULT_LATEST_TASK_NUM 5   //最近任务不足5个时默认值补上
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY,FUNC_WEATHER};

///卡片、组件原始位置（相对于卡片中心点）
#define CARD_WIDTH_ORG                  232//308

typedef struct ui_handle_t_
{
    struct card_clock_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct date_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            u8 bonddata;
            bool center;
            color_t color;
            u32 res;
        } date;

        struct week_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            bool center;
            u8 bonddata;
            color_t color;
            u32 res;
        } week;

        struct clock_t
        {
            u16 id_bg;
            u16 id_h;
            u16 id_min;
            s16 bg_x,bg_y,h_x,h_y,m_x,m_y;
            u16 bg_w,bg_h,h_w,h_h,m_w,m_h;
            u32 bg_res, h_res, m_res;
            u8 h_bonddata, m_bonddata;
        } clock;
    } card_clock_day;

    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
            color_t color;
        } rect;

        struct pic_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
        } pic_kcal;
        struct pic_t pic_km;
        struct pic_t pic_step;

        struct sport_arc_t
        {
            u16 id;
            s16 x,y;
            u16 w,r;
            color_t color;
            color_t bg_color;
            u16 zero_angle;
            u16 angle_min;
            u16 angle_max;
        } arc_kcal;
        struct sport_arc_t arc_km;
        struct sport_arc_t arc_step;

        struct text_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u16 str_id;
            bool center;
            color_t color;
        } text_kcal;
        struct text_t text_km;
        struct text_t text_step;
    } card1;

    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_sleep;
        struct text_t text_hour;
        struct text_t text_hour_unit;
        struct text_t text_min;
        struct text_t text_min_unit;
    } card2;

    struct card3_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_music;

        struct pic_click_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u32 res_click;
            u32 res_switch;
            u16 func_sta;
        } pic_click_prev;
        struct pic_click_t pic_click_next;
        struct pic_click_t pic_click_play;
    } card3;

    struct card4_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_timer;
        struct text_t text_time;
    } card4;

    struct card5_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_stopwatch;
        struct text_t text_time;
    } card5;

    struct card6_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_comm_use;
        struct pic_click_t pic_click[3];
    } card6;

    struct card7_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_last_use;
        struct pic_click_t pic_click[4];
    } card7;

} ui_handle_t;
//组件ID
enum
{
    COMPO_ID_CLOCK_BG = 256,
    COMPO_ID_CLOCK_H,
    COMPO_ID_CLOCK_M,
    COMPO_ID_DATE,
    COMPO_ID_WEEKDAY,

    COMPO_CARD_START,
    COMPO_ID_CARD_1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    COMPO_ID_CARD_5,
    COMPO_ID_CARD_6,
    COMPO_ID_CARD_7,
    COMPO_CARD_END,

    COMPO_ID_ARC_KCAL,
    COMPO_ID_ARC_KM,
    COMPO_ID_ARC_STEP,

    COMPO_MUSIC_BTN_PREV,
    COMPO_MUSIC_BTN_NEXT,
    COMPO_MUSIC_BTN_PLAY,

    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
    COMPO_ID_APP4,
};

static const ui_handle_t ui_handle =
{
    .card_clock_day = {
        .id = COMPO_CARD_START,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 24+100/2,
        .w  = 0,
        .h  = 0,

        .date = {
            .id = COMPO_ID_DATE,
            .x  = 12-232/2,
            .y  = 41-136/2,
            .w  = 80+20,
            .h  = 40,
            .center = false,
            .bonddata = COMPO_BOND_DATE,
            .color = {255,255,255},
            .res = UI_BUF_0FONT_FONT_NUM_32_BIN,
        },

        .week = {
            .id = COMPO_ID_WEEKDAY,
            .x  = 12-232/2,
            .y  = 79-136/2,
            .w  = 45,
            .h  = 32,
            .center = false,
            .bonddata = COMPO_BOND_WEEKDAY,
            .color = {153,153,153},
            .res = UI_BUF_0FONT_FONT_BIN,
        },

        .clock = {
            .id_bg  = COMPO_ID_CLOCK_BG,
            .id_h   = COMPO_ID_CLOCK_H,
            .id_min = COMPO_ID_CLOCK_M,
            .bg_x   = 130+100/2-232/2,
            .bg_y   = 0,
            .h_x    = 130+96/2-232/2,
            .h_y    = 0,
            .m_x    = 130+96/2-232/2,
            .m_y    = 0,
            .bg_w   = 100,
            .bg_h   = 100,
            .h_w    = 53,
            .h_h    = 18,
            .m_w    = 65,
            .m_h    = 18,
            .bg_res = UI_BUF_I332001_FIRSTORDER_TIME_BG_BIN,
            .h_res  = UI_BUF_I332001_FIRSTORDER_TIME_H_BIN,
            .m_res  = UI_BUF_I332001_FIRSTORDER_TIME_M_BIN,
            .h_bonddata = COMPO_BOND_HOUR,
            .m_bonddata = COMPO_BOND_MINUTE,
        },
    },

    .card1 = {
        .id = COMPO_ID_CARD_1,
        .x  = 180,
        .y  = 228,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },
        .pic_kcal = {
            .idx    = 0,
            .x      = 10+18/2-324/2,
            .y      = 21+18/2-152/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_CALORIES_BIN,
        },
        .pic_km = {
            .idx    = 1,
            .x      = 10+18/2-324/2,
            .y      = 46+18/2-152/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_DIS_BIN,
        },
        .pic_step = {
            .idx    = 2,
            .x      = 10+18/2-324/2,
            .y      = 71+18/2-152/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_STEPT_BIN,
        },

        .arc_kcal = {
            .id     = COMPO_ID_ARC_KCAL,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100,
            .color  = {252,55,40},
            .bg_color = {0x7f,0x6e,0x19},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_km = {
            .id     = COMPO_ID_ARC_KM,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-12,
            .color  = {255,212,0},
            .bg_color = {0x19,0x7a,0x6f},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_step = {
            .id     = COMPO_ID_ARC_STEP,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-24,
            .color  = {0,242,214},
            .bg_color = {0x7e,0x2f,0x29},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .text_kcal = {
            .idx    = 0,
            .x      = 34-324/2,
            .y      = 21+18/2-152/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 0,
            .center = false,
            .color  = {252,55,40},
        },
        .text_km = {
            .idx    = 1,
            .x      = 34-324/2,
            .y      = 46+18/2-152/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 1,
            .center = false,
            .color  = {255,212,0},
        },
        .text_step  = {
            .idx    = 2,
            .x      = 34-324/2,
            .y      = 71+18/2-152/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 2,
            .center = false,
            .color  = {0,242,214},
        },
    },

    .card2 = {
        .id = COMPO_ID_CARD_2,
        .x  = 180,
        .y  = 76+316,
        .w  = 324,
        .h  = 152,
        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_sleep = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_SLEEP,
            .center = false,
            .color  = {100,95,255},
        },
        .text_hour = {
            .idx    = 1,
            .x      = 12-324/2,
            .y      = 38-152/2+6,
            .w      = 41+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_hour_unit = {
            .idx    = 2,
            .x      = 56-324/2,
            .y      = 53-152/2,
            .w      = 46,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_HOUR,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min = {
            .idx    = 3,
            .x      = 97-324/2+14,
            .y      = 38-152/2+6,
            .w      = 40+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min_unit = {
            .idx    = 4,
            .x      = 140-324/2+12,
            .y      = 53-152/2,
            .w      = 50,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MIN,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card3 = {
        .id = COMPO_ID_CARD_3,
        .x  = 180,
        .y  = 480+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_music = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MUSIC,
            .center = false,
            .color  = {255,48,96},
        },

        .pic_click_prev = {
            .idx    = COMPO_MUSIC_BTN_PREV,
            .x      = 26+28/2-324/2,
            .y      = 52+28/2-152/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_UP00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_UP01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_next = {
            .idx    = COMPO_MUSIC_BTN_NEXT,
            .x      = 178+28/2-324/2,
            .y      = 52+28/2-152/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_NEXT00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_NEXT01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_play = {
            .idx    = COMPO_MUSIC_BTN_PLAY,
            .x      = 88+56/2-324/2,
            .y      = 38+56/2-152/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_PLAY00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_PLAY01_BIN,
            .res_switch = UI_BUF_I332001_FIRSTORDER_MUSIC_PAUSED_BIN,
            .func_sta = 0,
        },
    },

    .card4 = {
        .id = COMPO_ID_CARD_4,
        .x  = 180,
        .y  = 644+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_timer = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_TIMER,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-324/2,
            .y      = 37-152/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card5 = {
        .id = COMPO_ID_CARD_5,
        .x  = 180,
        .y  = 808+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_stopwatch = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_STOP_WATCH,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-324/2,
            .y      = 37-152/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card6 = {
        .id = COMPO_ID_CARD_6,
        .x  = 180,
        .y  = 972+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_comm_use = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_COMMON_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = 0,
                .x      = 27+48/2-324/2,
                .y      = 41+48/2-152/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_SPORT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },

            [1] = {
                .idx    = 1,
                .x      = 91+48/2-324/2,
                .y      = 41+48/2-152/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_MSM_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
                .func_sta = FUNC_MESSAGE,
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            },

            [2] = {
                .idx    = 2,
                .x      = 155+48/2-324/2,
                .y      = 41+48/2-152/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_CALL_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },

    .card7 = {
        .id = COMPO_ID_CARD_7,
        .x  = 180,
        .y  = 1136+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_last_use = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_LATEST_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = COMPO_ID_APP1,
                .x      = 12+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I332001_THEME_ICON1_SOPRT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },
            [1] = {
                .idx    = COMPO_ID_APP2,
                .x      = 66+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I332001_THEME_ICON1_BRIGHTNESS_BIN,
                .res_click = 1,
                .res_switch = 1,
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
                .func_sta = FUNC_LIGHT,
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            },
            [2] = {
                .idx    = COMPO_ID_APP3,
                .x      = 120+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I332001_THEME_ICON1_TIMER_BIN,
                .res_click = 2,
                .res_switch = 2,
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
                .func_sta = FUNC_TIMER,
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            },
            [3] = {
                .idx    = COMPO_ID_APP4,
                .x      = 174+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I332001_THEME_ICON1_CALL_BIN,
                .res_click = 3,
                .res_switch = 3,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },
};

//获取卡片原始位置
static int func_clock_sub_card_get_y_info(int card_compo_id)
{
    int ret = 0;

    if (card_compo_id == ui_handle.card_clock_day.id)
    {
        ret = ui_handle.card_clock_day.y;
    }

    if (card_compo_id == ui_handle.card1.id)
    {
        ret = ui_handle.card1.y;
    }
    if (card_compo_id == ui_handle.card2.id)
    {
        ret = ui_handle.card2.y;
    }
    if (card_compo_id == ui_handle.card3.id)
    {
        ret = ui_handle.card3.y;
    }
    if (card_compo_id == ui_handle.card4.id)
    {
        ret = ui_handle.card4.y;
    }
    if (card_compo_id == ui_handle.card5.id)
    {
        ret = ui_handle.card5.y;
    }
    if (card_compo_id == ui_handle.card6.id)
    {
        ret = ui_handle.card6.y;
    }
    if (card_compo_id == ui_handle.card7.id)
    {
        ret = ui_handle.card7.y;
    }
    return ret;
}

//功能结构体
typedef struct f_card_t_
{
    s32 ofs_y;
    s32 ofs_y_drag;
    s16 focus_card;                  //靠近屏幕中央的卡片序号
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    s32 moveto_y;                   //设定自动移到的坐标
    s16 moveto_card;                //设定自动移到的卡片序号
    u32 tick;
    u32 activity_tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
//    co_timer_t music_refresh;
} f_card_t;

static void card1_updata_disp(void)
{
    f_card_t *f_activity = (f_card_t *)func_cb.f_cb;

//    if(tick_check_expire(f_activity->activity_tick, ))
//    {
//        f_activity->activity_tick = tick_get();
    char txt_buf[20];
    uint32_t totalStepCnt = 0;
    uint32_t Step_value = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    Step_value = totalStepCnt;
    if(Step_value>uteModuleSportGetStepsTargetCnt())
    {
        Step_value = uteModuleSportGetStepsTargetCnt();
    }

    compo_picturebox_t *arc_kcal = compo_getobj_byid(COMPO_ID_ARC_KCAL);
    compo_picturebox_t *arc_km   = compo_getobj_byid(COMPO_ID_ARC_KM);
    compo_picturebox_t *arc_step = compo_getobj_byid(COMPO_ID_ARC_STEP);
    compo_cardbox_t *cardbox = compo_getobj_byid(ui_handle.card1.id);

    compo_picturebox_set_visible(arc_kcal, true);
    compo_picturebox_set_visible(arc_km, true);
    compo_picturebox_set_visible(arc_step, true);


    if(f_activity->activity_state == 0)
    {
        f_activity->arc_step_value+=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value >=1800)
        {
            f_activity->activity_state =1;
        }
    }
    else if(f_activity->activity_state == 1)
    {

        f_activity->arc_step_value-=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value <= Step_value*1800 / uteModuleSportGetStepsTargetCnt())
        {
            f_activity->activity_state = 2;
        }

    }
    else
    {

        f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
        f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
        f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
        compo_cardbox_text_set(cardbox, 2, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
        compo_cardbox_text_set(cardbox, 1, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
        compo_cardbox_text_set(cardbox, 0, txt_buf);


        if(f_activity->arc_step_value)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);
        }
        else
        {
            compo_picturebox_set_visible(arc_kcal,false);
            compo_picturebox_set_visible(arc_km,false);
            compo_picturebox_set_visible(arc_step,false);
        }
    }

//    }
}
//根据序号获取最近任务序号（idx=0为最近，无任务返回0）(idx<=3)
static u8 card_get_latest_func(u8 idx)
{
    u8 i, j;
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};
    u8 latest_cnt = latest_task_count();
    if (latest_cnt > idx)
    {
        return latest_task_get(idx);
    }
    else
    {
        j = 0;
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)     //最近任务不足DEFAULT_LATEST_TASK_NUM个且包含默认值
        {
            if (latest_task_find(last_default[i]) == -1)
            {
                latest_default_tmp[j] = last_default[i];
                j++;
            }
        }
        return latest_default_tmp[idx - latest_cnt];
    }
}

//根据序号获取最近任务图标资源地址
static u32 card_get_latest_icon(u8 idx)
{
    return func_get_icon_addr(card_get_latest_func(idx));
}

//创建组件
static void func_clock_sub_card_compo_create(compo_form_t *frm)
{
    char txt_buf[20];
    compo_button_t *btn=NULL;

    ///卡片7
    compo_cardbox_t* card7 = compo_cardbox_create(frm, 1, 4, 1, ui_handle.card7.w, ui_handle.card7.h);
    compo_setid(card7, ui_handle.card7.id);

    compo_cardbox_rect_set_location(card7, ui_handle.card7.rect.idx, ui_handle.card7.rect.x, ui_handle.card7.rect.y, ui_handle.card7.rect.w, ui_handle.card7.rect.h, ui_handle.card7.rect.r);
    compo_cardbox_rect_set_color(card7, ui_handle.card7.rect.idx, make_color(ui_handle.card7.rect.color.r, ui_handle.card7.rect.color.g, ui_handle.card7.rect.color.b));

    compo_cardbox_text_set_font(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.res);
    compo_cardbox_text_set_align_center(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.center);
    widget_text_set_color(card7->text[ui_handle.card7.text_last_use.idx], make_color(ui_handle.card7.text_last_use.color.r, ui_handle.card7.text_last_use.color.g, ui_handle.card7.text_last_use.color.b));
    compo_cardbox_text_set_location(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.x, ui_handle.card7.text_last_use.y, ui_handle.card7.text_last_use.w, ui_handle.card7.text_last_use.h);
    compo_cardbox_text_set(card7, ui_handle.card7.text_last_use.idx, i18n[ui_handle.card7.text_last_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card7.pic_click)/sizeof(ui_handle.card7.pic_click[0]); i++)
    {
        btn = compo_button_create_page_by_image(frm,card7->page,card_get_latest_icon(i));
        compo_button_set_location(btn,ui_handle.card7.pic_click[i].x,ui_handle.card7.pic_click[i].y, ui_handle.card7.pic_click[i].w, ui_handle.card7.pic_click[i].h);
        compo_setid(btn,ui_handle.card7.pic_click[i].idx);
    }

    ///卡片6
    compo_cardbox_t* card6 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card6.w, ui_handle.card6.h);
    compo_setid(card6, ui_handle.card6.id);

    compo_cardbox_rect_set_location(card6, ui_handle.card6.rect.idx, ui_handle.card6.rect.x, ui_handle.card6.rect.y, ui_handle.card6.rect.w, ui_handle.card6.rect.h, ui_handle.card6.rect.r);
    compo_cardbox_rect_set_color(card6, ui_handle.card6.rect.idx, make_color(ui_handle.card6.rect.color.r, ui_handle.card6.rect.color.g, ui_handle.card6.rect.color.b));

    compo_cardbox_text_set_font(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.res);
    compo_cardbox_text_set_align_center(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.center);
    widget_text_set_color(card6->text[ui_handle.card6.text_comm_use.idx], make_color(ui_handle.card6.text_comm_use.color.r, ui_handle.card6.text_comm_use.color.g, ui_handle.card6.text_comm_use.color.b));
    compo_cardbox_text_set_location(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.x, ui_handle.card6.text_comm_use.y, ui_handle.card6.text_comm_use.w, ui_handle.card6.text_comm_use.h);
    compo_cardbox_text_set(card6, ui_handle.card6.text_comm_use.idx, i18n[ui_handle.card6.text_comm_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
    {
        compo_cardbox_icon_set_location(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].x, ui_handle.card6.pic_click[i].y, ui_handle.card6.pic_click[i].w, ui_handle.card6.pic_click[i].h);
        compo_cardbox_icon_set(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].res);
    }


    ///卡片5
    compo_cardbox_t* card5 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card5.w, ui_handle.card5.h);
    compo_setid(card5, ui_handle.card5.id);

    compo_cardbox_rect_set_location(card5, ui_handle.card5.rect.idx, ui_handle.card5.rect.x, ui_handle.card5.rect.y, ui_handle.card5.rect.w, ui_handle.card5.rect.h, ui_handle.card5.rect.r);
    compo_cardbox_rect_set_color(card5, ui_handle.card5.rect.idx, make_color(ui_handle.card5.rect.color.r, ui_handle.card5.rect.color.g, ui_handle.card5.rect.color.b));

//    compo_cardbox_text_set_font(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_stopwatch.idx], make_color(ui_handle.card5.text_stopwatch.color.r, ui_handle.card5.text_stopwatch.color.g, ui_handle.card5.text_stopwatch.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.x, ui_handle.card5.text_stopwatch.y, ui_handle.card5.text_stopwatch.w, ui_handle.card5.text_stopwatch.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_stopwatch.idx, i18n[ui_handle.card5.text_stopwatch.str_id]);

    compo_cardbox_text_set_font(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_time.idx], make_color(ui_handle.card5.text_time.color.r, ui_handle.card5.text_time.color.g, ui_handle.card5.text_time.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.x, ui_handle.card5.text_time.y, ui_handle.card5.text_time.w, ui_handle.card5.text_time.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_time.idx, "00:00.00");

    ///卡片4
    compo_cardbox_t* card4 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card4.w, ui_handle.card4.h);
    compo_setid(card4, ui_handle.card4.id);

    compo_cardbox_rect_set_location(card4, ui_handle.card4.rect.idx, ui_handle.card4.rect.x, ui_handle.card4.rect.y, ui_handle.card4.rect.w, ui_handle.card4.rect.h, ui_handle.card4.rect.r);
    compo_cardbox_rect_set_color(card4, ui_handle.card4.rect.idx, make_color(ui_handle.card4.rect.color.r, ui_handle.card4.rect.color.g, ui_handle.card4.rect.color.b));

    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_timer.idx], make_color(ui_handle.card4.text_timer.color.r, ui_handle.card4.text_timer.color.g, ui_handle.card4.text_timer.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.x, ui_handle.card4.text_timer.y, ui_handle.card4.text_timer.w, ui_handle.card4.text_timer.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_timer.idx, i18n[ui_handle.card4.text_timer.str_id]);

    compo_cardbox_text_set_font(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.res);
    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_time.idx], make_color(ui_handle.card4.text_time.color.r, ui_handle.card4.text_time.color.g, ui_handle.card4.text_time.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.x, ui_handle.card4.text_time.y, ui_handle.card4.text_time.w, ui_handle.card4.text_time.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_time.idx, "00:00:00");


    ///卡片3
    compo_cardbox_t* card3 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card3.w, ui_handle.card3.h);
    compo_setid(card3, ui_handle.card3.id);

    compo_cardbox_rect_set_location(card3, ui_handle.card3.rect.idx, ui_handle.card3.rect.x, ui_handle.card3.rect.y, ui_handle.card3.rect.w, ui_handle.card3.rect.h, ui_handle.card3.rect.r);
    compo_cardbox_rect_set_color(card3, ui_handle.card3.rect.idx, make_color(ui_handle.card3.rect.color.r, ui_handle.card3.rect.color.g, ui_handle.card3.rect.color.b));

    compo_cardbox_text_set_font(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.res);
    compo_cardbox_text_set_align_center(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.center);
    widget_text_set_color(card3->text[ui_handle.card3.text_music.idx], make_color(ui_handle.card3.text_music.color.r, ui_handle.card3.text_music.color.g, ui_handle.card3.text_music.color.b));
    compo_cardbox_text_set_location(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.x, ui_handle.card3.text_music.y, ui_handle.card3.text_music.w, ui_handle.card3.text_music.h);
    compo_cardbox_text_set(card3, ui_handle.card3.text_music.idx, i18n[ui_handle.card3.text_music.str_id]);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_prev.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_prev.x, ui_handle.card3.pic_click_prev.y, ui_handle.card3.pic_click_prev.w, ui_handle.card3.pic_click_prev.h);
    compo_setid(btn,ui_handle.card3.pic_click_prev.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_next.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_next.x, ui_handle.card3.pic_click_next.y, ui_handle.card3.pic_click_next.w, ui_handle.card3.pic_click_next.h);
    compo_setid(btn,ui_handle.card3.pic_click_next.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_play.res_click);
    compo_button_set_pos(btn,ui_handle.card3.pic_click_play.x, ui_handle.card3.pic_click_play.y);
    compo_setid(btn,ui_handle.card3.pic_click_play.idx);

    ///卡片2
    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);
    compo_cardbox_t* card2 = compo_cardbox_create(frm, 1, 0, 5, ui_handle.card2.w, ui_handle.card2.h);
    compo_setid(card2, ui_handle.card2.id);

    compo_cardbox_rect_set_location(card2, ui_handle.card2.rect.idx, ui_handle.card2.rect.x, ui_handle.card2.rect.y, ui_handle.card2.rect.w, ui_handle.card2.rect.h, ui_handle.card2.rect.r);
    compo_cardbox_rect_set_color(card2, ui_handle.card2.rect.idx, make_color(ui_handle.card2.rect.color.r, ui_handle.card2.rect.color.g, ui_handle.card2.rect.color.b));

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_sleep.idx], make_color(ui_handle.card2.text_sleep.color.r, ui_handle.card2.text_sleep.color.g, ui_handle.card2.text_sleep.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.x, ui_handle.card2.text_sleep.y, ui_handle.card2.text_sleep.w, ui_handle.card2.text_sleep.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_sleep.idx, i18n[ui_handle.card2.text_sleep.str_id]);

//    sleep_data->totalSleepMin =  9;
    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour.idx], make_color(ui_handle.card2.text_hour.color.r, ui_handle.card2.text_hour.color.g, ui_handle.card2.text_hour.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.x, ui_handle.card2.text_hour.y, ui_handle.card2.text_hour.w, ui_handle.card2.text_hour.h);
    memset(txt_buf,0,sizeof(txt_buf));
    if(sleep_data->totalSleepMin)   ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠小时*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour_unit.idx], make_color(ui_handle.card2.text_hour_unit.color.r, ui_handle.card2.text_hour_unit.color.g, ui_handle.card2.text_hour_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.x+6, ui_handle.card2.text_hour_unit.y, ui_handle.card2.text_hour_unit.w, ui_handle.card2.text_hour_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour_unit.idx, i18n[ui_handle.card2.text_hour_unit.str_id]);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min.idx], make_color(ui_handle.card2.text_min.color.r, ui_handle.card2.text_min.color.g, ui_handle.card2.text_min.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.x, ui_handle.card2.text_min.y, ui_handle.card2.text_min.w, ui_handle.card2.text_min.h);
    if(sleep_data->totalSleepMin)  ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠分钟*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_min.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min_unit.idx], make_color(ui_handle.card2.text_min_unit.color.r, ui_handle.card2.text_min_unit.color.g, ui_handle.card2.text_min_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.x+6, ui_handle.card2.text_min_unit.y, ui_handle.card2.text_min_unit.w, ui_handle.card2.text_min_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_min_unit.idx, i18n[ui_handle.card2.text_min_unit.str_id]);

    ///卡片1
    compo_cardbox_t* card1 = compo_cardbox_create(frm, 1, 3, 3, ui_handle.card1.w, ui_handle.card1.h);
    compo_setid(card1, ui_handle.card1.id);

    compo_cardbox_rect_set_location(card1, ui_handle.card1.rect.idx, ui_handle.card1.rect.x, ui_handle.card1.rect.y, ui_handle.card1.rect.w, ui_handle.card1.rect.h, ui_handle.card1.rect.r);
    compo_cardbox_rect_set_color(card1, ui_handle.card1.rect.idx, make_color(ui_handle.card1.rect.color.r, ui_handle.card1.rect.color.g, ui_handle.card1.rect.color.b));

    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.x, ui_handle.card1.pic_kcal.y, ui_handle.card1.pic_kcal.w, ui_handle.card1.pic_kcal.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.x, ui_handle.card1.pic_km.y, ui_handle.card1.pic_km.w, ui_handle.card1.pic_km.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.x, ui_handle.card1.pic_step.y, ui_handle.card1.pic_step.w, ui_handle.card1.pic_step.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.res);

    uint16_t KM = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_kcal.idx], make_color(ui_handle.card1.text_kcal.color.r, ui_handle.card1.text_kcal.color.g, ui_handle.card1.text_kcal.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.x, ui_handle.card1.text_kcal.y, ui_handle.card1.text_kcal.w, ui_handle.card1.text_kcal.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
    compo_cardbox_text_set(card1, ui_handle.card1.text_kcal.idx, txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_km.idx], make_color(ui_handle.card1.text_km.color.r, ui_handle.card1.text_km.color.g, ui_handle.card1.text_km.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.x, ui_handle.card1.text_km.y, ui_handle.card1.text_km.w, ui_handle.card1.text_km.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",KM/100, KM%100);///公里数据
    compo_cardbox_text_set(card1, ui_handle.card1.text_km.idx,txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_step.idx], make_color(ui_handle.card1.text_step.color.r, ui_handle.card1.text_step.color.g, ui_handle.card1.text_step.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.x, ui_handle.card1.text_step.y, ui_handle.card1.text_step.w, ui_handle.card1.text_step.h);
    snprintf(txt_buf,sizeof(txt_buf),"%ld",totalStepCnt);
    compo_cardbox_text_set(card1, ui_handle.card1.text_step.idx, txt_buf);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I332001_FIRSTORDER_ACTIVITY_BG_BIN);
    widget_page_t * widget_page = widget_page_create(card1->page);///创建按键页面
    widget_set_location(widget_page,52,0,pic_bg_area.wid,pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_BG_BIN);//彩虹背景
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_RED_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KCAL);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_YELLOW_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KM);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_BLUE_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_STEP);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_YUANJIAO1_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I332001_FIRSTORDER_ACTIVITY_YUANJIAO1_BIN).hei );

    ///时钟 日期
    compo_picturebox_t* clock_bg = compo_picturebox_create(frm, ui_handle.card_clock_day.clock.bg_res);
    compo_setid(clock_bg, ui_handle.card_clock_day.clock.id_bg);

    compo_datetime_t* clock_h = compo_datetime_create(frm, ui_handle.card_clock_day.clock.h_res);
    compo_setid(clock_h, ui_handle.card_clock_day.clock.id_h);
    compo_datetime_set_center(clock_h, 13, ui_handle.card_clock_day.clock.h_h / 2);
    compo_datetime_set_start_angle(clock_h, 900);
    compo_bonddata(clock_h, ui_handle.card_clock_day.clock.h_bonddata);

    compo_datetime_t* clock_m = compo_datetime_create(frm, ui_handle.card_clock_day.clock.m_res);
    compo_setid(clock_m, ui_handle.card_clock_day.clock.id_min);
    compo_datetime_set_center(clock_m, 13, ui_handle.card_clock_day.clock.m_h / 2);
    compo_datetime_set_start_angle(clock_m, 900);
    compo_bonddata(clock_m, ui_handle.card_clock_day.clock.m_bonddata);

    compo_textbox_t* date = compo_textbox_create(frm, 5);                                 //日期
    compo_setid(date, ui_handle.card_clock_day.date.id);
    compo_textbox_set_font(date, ui_handle.card_clock_day.date.res);
    compo_textbox_set_align_center(date, ui_handle.card_clock_day.date.center);
    widget_text_set_color(date->txt, make_color(ui_handle.card_clock_day.date.color.r, ui_handle.card_clock_day.date.color.g, ui_handle.card_clock_day.date.color.b));
    compo_bonddata(date, ui_handle.card_clock_day.date.bonddata);

    compo_textbox_t* week = compo_textbox_create(frm, 20);                                 //日期
    compo_setid(week, ui_handle.card_clock_day.week.id);
    compo_textbox_set_font(week, ui_handle.card_clock_day.week.res);
    compo_textbox_set_align_center(week, ui_handle.card_clock_day.week.center);
    widget_text_set_color(week->txt, make_color(ui_handle.card_clock_day.week.color.r, ui_handle.card_clock_day.week.color.g, ui_handle.card_clock_day.week.color.b));
    compo_bonddata(week, ui_handle.card_clock_day.week.bonddata);

//    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 6, 36, 6);
//    compo_shape_set_color(shape, make_color(0X4C,0X4C,0X4C));
//    compo_shape_set_radius(shape, 35);

    ab_free(sleep_data);

}

//更新组件位置和大小
static void func_clock_sub_card_compo_update(s32 ofs_y, bool creating)
{
    s16 x, y, w, h, card_y, card_w;
    u8 card_id = 0;
    u16 dc;  //当前卡片中心距屏幕中心距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL)
    {
        //获取初始值
        switch (compo->id)
        {
            case COMPO_ID_CLOCK_BG:
                if (ui_handle.card_clock_day.clock.id_bg != COMPO_ID_CLOCK_BG)
                {
                    printf("COMPO_ID_CLOCK_BG ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.bg_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.bg_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.bg_w;
                h = ui_handle.card_clock_day.clock.bg_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_H:
                if (ui_handle.card_clock_day.clock.id_h != COMPO_ID_CLOCK_H)
                {
                    printf("COMPO_ID_CLOCK_H ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.h_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.h_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.h_w;
                h = ui_handle.card_clock_day.clock.h_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_M:
                if (ui_handle.card_clock_day.clock.id_min != COMPO_ID_CLOCK_M)
                {
                    printf("COMPO_ID_CLOCK_M ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.m_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.m_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.m_w;
                h = ui_handle.card_clock_day.clock.m_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_DATE:
                if (ui_handle.card_clock_day.date.id != COMPO_ID_DATE)
                {
                    printf("COMPO_ID_DATE ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.date.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.date.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.date.w;
                h = ui_handle.card_clock_day.date.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_WEEKDAY:
                if (ui_handle.card_clock_day.week.id != COMPO_ID_WEEKDAY)
                {
                    printf("COMPO_ID_WEEKDAY ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.week.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.week.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.week.w;
                h = ui_handle.card_clock_day.week.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CARD_1:
                if (ui_handle.card1.id != COMPO_ID_CARD_1)
                {
                    printf("COMPO_ID_CARD_1 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card1.x;
                y = card_y = ui_handle.card1.y;
                w = ui_handle.card1.w;
                h = ui_handle.card1.h;
                card_id = ui_handle.card1.id;
                break;

            case COMPO_ID_CARD_2:
                if (ui_handle.card2.id != COMPO_ID_CARD_2)
                {
                    printf("COMPO_ID_CARD_2 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card2.x;
                y = card_y = ui_handle.card2.y;
                w = ui_handle.card2.w;
                h = ui_handle.card2.h;
                card_id = ui_handle.card2.id;
                break;

            case COMPO_ID_CARD_3:
                if (ui_handle.card3.id != COMPO_ID_CARD_3)
                {
                    printf("COMPO_ID_CARD_3 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card3.x;
                y = card_y = ui_handle.card3.y;
                w = ui_handle.card3.w;
                h = ui_handle.card3.h;
                card_id = ui_handle.card3.id;
                break;

            case COMPO_ID_CARD_4:
                if (ui_handle.card4.id != COMPO_ID_CARD_4)
                {
                    printf("COMPO_ID_CARD_4 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card4.x;
                y = card_y = ui_handle.card4.y;
                w = ui_handle.card4.w;
                h = ui_handle.card4.h;
                card_id = ui_handle.card4.id;
                break;

            case COMPO_ID_CARD_5:
                if (ui_handle.card5.id != COMPO_ID_CARD_5)
                {
                    printf("COMPO_ID_CARD_5 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card5.x;
                y = card_y = ui_handle.card5.y;
                w = ui_handle.card5.w;
                h = ui_handle.card5.h;
                card_id = ui_handle.card5.id;
                break;


            case COMPO_ID_CARD_6:
                if (ui_handle.card6.id != COMPO_ID_CARD_6)
                {
                    printf("COMPO_ID_CARD_6 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card6.x;
                y = card_y = ui_handle.card6.y;
                w = ui_handle.card6.w;
                h = ui_handle.card6.h;
                card_id = ui_handle.card6.id;
                break;


            case COMPO_ID_CARD_7:
                if (ui_handle.card7.id != COMPO_ID_CARD_7)
                {
                    printf("COMPO_ID_CARD_7 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card7.x;
                y = card_y = ui_handle.card7.y;
                w = ui_handle.card7.w;
                h = ui_handle.card7.h;
                card_id = ui_handle.card7.id;
                break;

            default:
                compo = compo_get_next(compo);
                continue;
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH_ORG;
        card_y += ofs_y;
        y += ofs_y;
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);
        if (creating == false && dc < dc_min)    //焦点，创建时不使用f_cb
        {
            f_card_t *f_card = (f_card_t *)func_cb.f_cb;
            f_card->focus_card = card_id;
            dc_min = dc;
//            TRACE("focus[%d]\n", card_id);
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM)
        {
            if (card_y > CARD_SCALE_STOP_Y)    //缩到最小后保持一段时间
            {
                y -= card_y - CARD_SCALE_STOP_Y;
                card_y = CARD_SCALE_STOP_Y;
            }
            if (card_w > CARD_WIDTH_MIN)
            {
                //缩放
                s16 card_w_org = card_w;
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                w = w * card_w / card_w_org;
                h = h * card_w / card_w_org;
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org;
                y += (card_y - y) * (card_w_org - card_w) / card_w_org;
                //偏移
                s16 card_y_org = card_y;
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                y += card_y - card_y_org;
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM;
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level));   //背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level));
            if (unvisible)
            {
                compo_cardbox_set_visible(cardbox, false);
            }
            else
            {
                compo_cardbox_set_visible(cardbox, true);
            }
        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_set_pos(pic, x, y);
            compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_picturebox_set_visible(pic, false);
            }
            else
            {
                compo_picturebox_set_visible(pic, true);
            }
        }
        else if (compo->type == COMPO_TYPE_TEXTBOX)
        {
            compo_textbox_t *txt = (compo_textbox_t *)compo;
            compo_textbox_set_location(txt, x, y, w, h);
            if (unvisible)
            {
                compo_textbox_set_visible(txt, false);
            }
            else
            {
                compo_textbox_set_visible(txt, true);
            }
        }
        else if (compo->type == COMPO_TYPE_DATETIME)
        {
            compo_datetime_t *dt = (compo_datetime_t *)compo;
            compo_datetime_set_pos(dt, x, y);
        }

        compo = compo_get_next(compo);          //遍历组件
    }
}

//创建上拉卡片菜单（文件内调用）
static compo_form_t *func_clock_sub_card_form_create_by_ofs(s16 ofs_y)
{
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
//    compo_shape_set_alpha(masklayer, 160);
    //创建所有组件
    func_clock_sub_card_compo_create(frm);
    //更新组件位置和大小
    func_clock_sub_card_compo_update(ofs_y, true);

    return frm;
}

//创建上拉卡片菜单（供外部调用）
compo_form_t *func_clock_sub_card_form_create(void)
{
    return func_clock_sub_card_form_create_by_ofs(SPRING_Y_MAX);
}

//获取点击的卡片组件id
static u16 func_clock_sub_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_BTN_COUNT; i++)
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;
        id = COMPO_CARD_START + 1 + i;
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

//表盘上拉卡片菜单点击处理
static void func_clock_sub_card_click_handler(void)
{
    compo_cardbox_t *cardbox = NULL;
//    rect_t rect;
    u8 func_jump = FUNC_NULL;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_clock_sub_card_get_btn_id(pt);
    printf("click compo_id:%d\n", compo_id);
    int id = compo_get_button_id();

    switch(compo_id)
    {
        case COMPO_ID_CARD_1 :
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            func_jump = FUNC_ACTIVITY;
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            break;
        case COMPO_ID_CARD_2 :
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
            func_jump = FUNC_SLEEP;
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
            break;
        case COMPO_ID_CARD_3:
            switch(id)
            {
                case COMPO_MUSIC_BTN_PREV:
                {
                    uteModuleMusicCtrl(false,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV);
                    compo_button_set_bgimg(btn,ui_handle.card3.pic_click_prev.res);
                }
                break;
                case COMPO_MUSIC_BTN_PLAY:
                {
                    uteModuleMusicCtrlPaused(false);
                    if(ble_is_connect())
                    {
                        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
                    }
//                    if(bt_cb.music_playing)
//                    {
//                        compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY);
//                        compo_button_set_bgimg(btn, ui_handle.card3.pic_click_play.res_click);
//                    }
                }
                break;
                case COMPO_MUSIC_BTN_NEXT:
                {
                    uteModuleMusicCtrl(true,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);
                    compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res);
                }
                break;
                default:
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    func_jump = FUNC_BT;
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    break;
            }
            break;
        case COMPO_ID_CARD_4 :
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
            func_jump = FUNC_TIMER;
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            break;
        case COMPO_ID_CARD_5 :
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            func_jump = FUNC_STOPWATCH;
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            break;
        case COMPO_ID_CARD_6 :
            cardbox = compo_getobj_byid(COMPO_ID_CARD_6);
            for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
            {
                rect_t rect = compo_cardbox_get_icon_absolute(cardbox, ui_handle.card6.pic_click[i].idx); //上一首
                if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
                {
//                    printf("rect = [%d,%d,%d,%d]\n", rect.x, rect.y, rect.wid, rect.hei);
                    func_cb.sta = ui_handle.card6.pic_click[i].func_sta;
                }
            }
            break;
        case COMPO_ID_CARD_7 :
            printf("id : %d\n", id);
            if (id >= COMPO_ID_APP1)
            {
                func_jump = card_get_latest_func(id-COMPO_ID_APP1);
            }
            break;
        default:
            break;
    }
    if(func_jump != FUNC_NULL)
    {
        printf("func_jump=%d\n", func_jump);
        func_cb.sta = func_jump;  //直接跳转
    }
}

//根据偏移位置把最近的卡片对齐到屏幕中央
static s16 func_clock_sub_card_align_y(s16 ofs_y)
{
    u8 i, nearest_id = 0;
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    u16 dc;
    if (ofs_y >= DRAG_Y_MAX)
    {
        return DRAG_Y_MAX;  //回弹效果
    }
    else if (ofs_y <= DRAG_Y_MIN)
    {
        return DRAG_Y_MIN;
    }
//    for (i=0; i<CARD_COUNT; i++)
    for (i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
//        dc = abs_s(card_y_info[i] + ofs_y - GUI_SCREEN_CENTER_Y);
        dc = abs_s(func_clock_sub_card_get_y_info(COMPO_CARD_START+1+i) + ofs_y - GUI_SCREEN_CENTER_Y);
        if (dc < dc_min)
        {
            nearest_id = i;
            dc_min = dc;
        }
    }
//    return (GUI_SCREEN_CENTER_Y - card_y_info[nearest_id]);
    return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+nearest_id));
}

//根据卡片序号对齐页面偏移位置到屏幕中央
static s16 func_clock_sub_card_align_by_idx(s8 card_id)
{
//    if (card_id <= CARD_ID_CLOCK)
    if (card_id <= COMPO_CARD_START)
    {
        return DRAG_Y_MAX;
    }
//    else if (card_id > (CARD_COUNT - 1))
    else if (card_id > (COMPO_CARD_END-COMPO_CARD_START-1 - 1))
    {
        return DRAG_Y_MIN;
    }
    else
    {
//        return (GUI_SCREEN_CENTER_Y - card_y_info[card_id]);
        return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+card_id));
    }
}

//限制滑动偏移位置
static s16 func_clock_sub_card_limit_y(s16 ofs_y)
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));
    return ofs_y;
}

//设置页面偏移位置
static void func_clock_sub_card_set_offs(s16 ofs_y)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    f_card->ofs_y = ofs_y;
    func_clock_sub_card_compo_update(f_card->ofs_y, false);
}

//时钟表盘上拉菜单数据更新----------------------------------------->todo
static void func_clock_sub_card_data_update(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    compo_cardbox_t *cardbox = NULL;
    char txt_buf[20];

    if(f_card->flag_drag == 0)
    {
        card1_updata_disp();
    }

//    if(f_activity->activity_state != 2) return;

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",SEC_TO_HOUR(sys_cb.timer_left_sec),SEC_TO_MIN(sys_cb.timer_left_sec),SEC_TO_SEC(sys_cb.timer_left_sec));
    cardbox = compo_getobj_byid(ui_handle.card4.id);
    compo_cardbox_text_set(cardbox, ui_handle.card4.text_time.idx,txt_buf);

    u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    u16 msec = sys_cb.stopwatch_total_msec % 1000;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min  = 99;
        sec  = 59;
        msec = 999;
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld.%02ld",min,sec,msec/10);
    cardbox = compo_getobj_byid(ui_handle.card5.id);
    compo_cardbox_text_set(cardbox, ui_handle.card5.text_time.idx,txt_buf);
}

//下拉返回表盘
static void func_clock_sub_card_switch_to_clock(bool auto_switch)
{
    printf("%s\n", __func__);
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    u16 switch_mode = FUNC_SWITCH_MENU_PULLUP_DOWN | (auto_switch ? FUNC_SWITCH_AUTO : 0);
    compo_form_destroy(func_cb.frm_main);
    compo_form_t * frm_clock = func_create_form(FUNC_CLOCK);
    compo_form_t * frm = func_clock_sub_card_form_create_by_ofs(f_card->ofs_y);
    func_cb.frm_main = frm;
    if (func_switching(switch_mode, NULL))
    {
        func_cb.sta = FUNC_CLOCK;
    }
    compo_form_destroy(frm_clock);
}

void music_data_refresh(void)
{
    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }

    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY); //播放/暂停
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I332001_FIRSTORDER_MUSIC_PAUSED_BIN : ui_handle.card3.pic_click_play.res);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_play.res_click);
    }
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//时钟表盘上拉菜单主要事件流程处理
static void func_clock_sub_card_process(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    if (f_card->flag_drag)
    {
        s32 dx, dy;
        f_card->flag_drag = ctp_get_dxy(&dx, &dy);
        if (f_card->flag_drag)
        {
            //拖动页面
            f_card->ofs_y_drag = func_clock_sub_card_limit_y(f_card->ofs_y + dy);
            func_clock_sub_card_compo_update(f_card->ofs_y_drag, false);
        }
        else
        {
            //抬手
            f_card->ofs_y = f_card->ofs_y_drag;
            s16 last_dy = ctp_get_last_dxy().y;
            s16 to_y = f_card->ofs_y + (last_dy * DRAG_AUTO_SPEED);
            f_card->moveto_y = func_clock_sub_card_align_y(to_y);
            f_card->flag_move_auto = true;
            f_card->tick = tick_get();
        }
        for(u8 i=0; i<CARD_BTN_COUNT; i++)      //文本滚动
        {
            u16 id = COMPO_CARD_START + 1 + i;
            compo_cardbox_text_scroll_reset((compo_cardbox_t *)compo_getobj_byid(id));   //scroll test
            compo_cardbox_text_scroll_pause((compo_cardbox_t *)compo_getobj_byid(id), true);   //scroll test
        }

    }
    else if (f_card->flag_move_auto)
    {
        for(u8 i=0; i<CARD_BTN_COUNT; i++)      //文本滚动
        {
            u16 id = COMPO_CARD_START + 1 + i;
            compo_cardbox_text_scroll_pause((compo_cardbox_t *)compo_getobj_byid(id), false);   //scroll test
        }

        //自动移动
        if (f_card->ofs_y == f_card->moveto_y)
        {
            if (f_card->ofs_y < SPRING_Y_MIN)
            {
                f_card->moveto_y = SPRING_Y_MIN;
            }
            else if (f_card->ofs_y > SPRING_Y_MAX)
            {
                f_card->moveto_y = SPRING_Y_MAX;
            }
            else
            {
                f_card->flag_move_auto = false;            //移动完成
                f_card->moveto_card = f_card->focus_card;
            }
        }
        else if (tick_check_expire(f_card->tick, FOCUS_AUTO_TICK_EXPIRE))
        {
            TRACE("[%d]--move_to->[%d]\n", f_card->ofs_y, f_card->moveto_y);
            s16 dy;
            f_card->tick = tick_get();
            dy = f_card->moveto_y - f_card->ofs_y;
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
            }
            func_clock_sub_card_set_offs(f_card->ofs_y + dy);
        }
    }

    func_clock_sub_card_data_update();
    func_process();

    for(u8 i=0; i<CARD_BTN_COUNT; i++)      //文本滚动
    {
        u16 id = COMPO_CARD_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
}

#define SPRING_Y_MAX                    0                                                                       //回弹到顶部时的ofs_y
//#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - card_y_info[CARD_COUNT - 1] + 50)                     //回弹到底部时的ofs_y
#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_END-1) + 50)                     //回弹到底部时的ofs_y
#define DRAG_Y_MAX                      (SPRING_Y_MAX + 30)                                                     //拖动到顶部时的ofs_y
#define DRAG_Y_MIN                      (SPRING_Y_MIN - 78)                                                     //拖动到底部时的ofs_y

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 16                                                                      //拖动松手后的速度(惯性)

#define EXIT_Y_MIN                      0
#define EXIT_Y_MAX                      50//60                                                                      //下滑退出触摸点离屏幕顶部小于某值
#define TXT_CNT_MAX                     8                                                                       //文本框最大字符数
#define CARD_BTN_COUNT                  (COMPO_CARD_END - COMPO_CARD_START - 1)    //卡片（按钮）数量

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

#define DEFAULT_LATEST_TASK_NUM 5   //最近任务不足5个时默认值补上
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY,FUNC_WEATHER};

///卡片、组件原始位置（相对于卡片中心点）
#define CARD_WIDTH_ORG                  232//308

typedef struct ui_handle_t_
{
    struct card_clock_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct date_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            u8 bonddata;
            bool center;
            color_t color;
            u32 res;
        } date;

        struct week_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            bool center;
            u8 bonddata;
            color_t color;
            u32 res;
        } week;

        struct clock_t
        {
            u16 id_bg;
            u16 id_h;
            u16 id_min;
            s16 bg_x,bg_y,h_x,h_y,m_x,m_y;
            u16 bg_w,bg_h,h_w,h_h,m_w,m_h;
            u32 bg_res, h_res, m_res;
            u8 h_bonddata, m_bonddata;
        } clock;
    } card_clock_day;

    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
            color_t color;
        } rect;

        struct pic_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
        } pic_kcal;
        struct pic_t pic_km;
        struct pic_t pic_step;

        struct sport_arc_t
        {
            u16 id;
            s16 x,y;
            u16 w,r;
            color_t color;
            color_t bg_color;
            u16 zero_angle;
            u16 angle_min;
            u16 angle_max;
        } arc_kcal;
        struct sport_arc_t arc_km;
        struct sport_arc_t arc_step;

        struct text_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u16 str_id;
            bool center;
            color_t color;
        } text_kcal;
        struct text_t text_km;
        struct text_t text_step;
    } card1;

    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_sleep;
        struct text_t text_hour;
        struct text_t text_hour_unit;
        struct text_t text_min;
        struct text_t text_min_unit;
    } card2;

    struct card3_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_music;

        struct pic_click_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u32 res_click;
            u32 res_switch;
            u16 func_sta;
        } pic_click_prev;
        struct pic_click_t pic_click_next;
        struct pic_click_t pic_click_play;
    } card3;

    struct card4_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_timer;
        struct text_t text_time;
    } card4;

    struct card5_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_stopwatch;
        struct text_t text_time;
    } card5;

    struct card6_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_comm_use;
        struct pic_click_t pic_click[3];
    } card6;

    struct card7_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_last_use;
        struct pic_click_t pic_click[4];
    } card7;

} ui_handle_t;
//组件ID
enum
{
    COMPO_ID_CLOCK_BG = 256,
    COMPO_ID_CLOCK_H,
    COMPO_ID_CLOCK_M,
    COMPO_ID_DATE,
    COMPO_ID_WEEKDAY,

    COMPO_CARD_START,
    COMPO_ID_CARD_1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    COMPO_ID_CARD_5,
    COMPO_ID_CARD_6,
    COMPO_ID_CARD_7,
    COMPO_CARD_END,

    COMPO_ID_ARC_KCAL,
    COMPO_ID_ARC_KM,
    COMPO_ID_ARC_STEP,

    COMPO_MUSIC_BTN_PREV,
    COMPO_MUSIC_BTN_NEXT,
    COMPO_MUSIC_BTN_PLAY,

    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
    COMPO_ID_APP4,
};

static const ui_handle_t ui_handle =
{
    .card_clock_day = {
        .id = COMPO_CARD_START,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 24+100/2,
        .w  = 0,
        .h  = 0,

        .date = {
            .id = COMPO_ID_DATE,
            .x  = 12-232/2,
            .y  = 41-136/2,
            .w  = 80+20,
            .h  = 40,
            .center = false,
            .bonddata = COMPO_BOND_DATE,
            .color = {255,255,255},
            .res = UI_BUF_0FONT_FONT_NUM_32_BIN,
        },

        .week = {
            .id = COMPO_ID_WEEKDAY,
            .x  = 12-232/2,
            .y  = 79-136/2,
            .w  = 45,
            .h  = 32,
            .center = false,
            .bonddata = COMPO_BOND_WEEKDAY,
            .color = {153,153,153},
            .res = UI_BUF_0FONT_FONT_BIN,
        },

        .clock = {
            .id_bg  = COMPO_ID_CLOCK_BG,
            .id_h   = COMPO_ID_CLOCK_H,
            .id_min = COMPO_ID_CLOCK_M,
            .bg_x   = 130+100/2-232/2,
            .bg_y   = 0,
            .h_x    = 130+96/2-232/2,
            .h_y    = 0,
            .m_x    = 130+96/2-232/2,
            .m_y    = 0,
            .bg_w   = 100,
            .bg_h   = 100,
            .h_w    = 53,
            .h_h    = 18,
            .m_w    = 65,
            .m_h    = 18,
            .bg_res = UI_BUF_I332001_FIRSTORDER_TIME_BG_BIN,
            .h_res  = UI_BUF_I332001_FIRSTORDER_TIME_H_BIN,
            .m_res  = UI_BUF_I332001_FIRSTORDER_TIME_M_BIN,
            .h_bonddata = COMPO_BOND_HOUR,
            .m_bonddata = COMPO_BOND_MINUTE,
        },
    },

    .card1 = {
        .id = COMPO_ID_CARD_1,
        .x  = 180,
        .y  = 228,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },
        .pic_kcal = {
            .idx    = 0,
            .x      = 10+18/2-324/2,
            .y      = 21+18/2-152/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_CALORIES_BIN,
        },
        .pic_km = {
            .idx    = 1,
            .x      = 10+18/2-324/2,
            .y      = 46+18/2-152/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_DIS_BIN,
        },
        .pic_step = {
            .idx    = 2,
            .x      = 10+18/2-324/2,
            .y      = 71+18/2-152/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_STEPT_BIN,
        },

        .arc_kcal = {
            .id     = COMPO_ID_ARC_KCAL,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100,
            .color  = {252,55,40},
            .bg_color = {0x7f,0x6e,0x19},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_km = {
            .id     = COMPO_ID_ARC_KM,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-12,
            .color  = {255,212,0},
            .bg_color = {0x19,0x7a,0x6f},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_step = {
            .id     = COMPO_ID_ARC_STEP,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-24,
            .color  = {0,242,214},
            .bg_color = {0x7e,0x2f,0x29},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .text_kcal = {
            .idx    = 0,
            .x      = 34-324/2,
            .y      = 21+18/2-152/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 0,
            .center = false,
            .color  = {252,55,40},
        },
        .text_km = {
            .idx    = 1,
            .x      = 34-324/2,
            .y      = 46+18/2-152/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 1,
            .center = false,
            .color  = {255,212,0},
        },
        .text_step  = {
            .idx    = 2,
            .x      = 34-324/2,
            .y      = 71+18/2-152/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 2,
            .center = false,
            .color  = {0,242,214},
        },
    },

    .card2 = {
        .id = COMPO_ID_CARD_2,
        .x  = 180,
        .y  = 76+316,
        .w  = 324,
        .h  = 152,
        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_sleep = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_SLEEP,
            .center = false,
            .color  = {100,95,255},
        },
        .text_hour = {
            .idx    = 1,
            .x      = 12-324/2,
            .y      = 38-152/2+6,
            .w      = 41+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_hour_unit = {
            .idx    = 2,
            .x      = 56-324/2,
            .y      = 53-152/2,
            .w      = 46,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_HOUR,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min = {
            .idx    = 3,
            .x      = 97-324/2+14,
            .y      = 38-152/2+6,
            .w      = 40+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min_unit = {
            .idx    = 4,
            .x      = 140-324/2+12,
            .y      = 53-152/2,
            .w      = 50,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MIN,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card3 = {
        .id = COMPO_ID_CARD_3,
        .x  = 180,
        .y  = 480+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_music = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MUSIC,
            .center = false,
            .color  = {255,48,96},
        },

        .pic_click_prev = {
            .idx    = COMPO_MUSIC_BTN_PREV,
            .x      = 26+28/2-324/2,
            .y      = 52+28/2-152/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_UP00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_UP01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_next = {
            .idx    = COMPO_MUSIC_BTN_NEXT,
            .x      = 178+28/2-324/2,
            .y      = 52+28/2-152/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_NEXT00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_NEXT01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_play = {
            .idx    = COMPO_MUSIC_BTN_PLAY,
            .x      = 88+56/2-324/2,
            .y      = 38+56/2-152/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_PLAY00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_PLAY01_BIN,
            .res_switch = UI_BUF_I332001_FIRSTORDER_MUSIC_PAUSED_BIN,
            .func_sta = 0,
        },
    },

    .card4 = {
        .id = COMPO_ID_CARD_4,
        .x  = 180,
        .y  = 644+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_timer = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_TIMER,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-324/2,
            .y      = 37-152/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card5 = {
        .id = COMPO_ID_CARD_5,
        .x  = 180,
        .y  = 808+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_stopwatch = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_STOP_WATCH,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-324/2,
            .y      = 37-152/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card6 = {
        .id = COMPO_ID_CARD_6,
        .x  = 180,
        .y  = 972+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_comm_use = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_COMMON_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = 0,
                .x      = 27+48/2-324/2,
                .y      = 41+48/2-152/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_SPORT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },

            [1] = {
                .idx    = 1,
                .x      = 91+48/2-324/2,
                .y      = 41+48/2-152/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_MSM_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
                .func_sta = FUNC_MESSAGE,
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            },

            [2] = {
                .idx    = 2,
                .x      = 155+48/2-324/2,
                .y      = 41+48/2-152/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_CALL_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },

    .card7 = {
        .id = COMPO_ID_CARD_7,
        .x  = 180,
        .y  = 1136+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_last_use = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_LATEST_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = COMPO_ID_APP1,
                .x      = 12+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I332001_THEME_ICON1_SOPRT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },
            [1] = {
                .idx    = COMPO_ID_APP2,
                .x      = 66+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I332001_THEME_ICON1_BRIGHTNESS_BIN,
                .res_click = 1,
                .res_switch = 1,
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
                .func_sta = FUNC_LIGHT,
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            },
            [2] = {
                .idx    = COMPO_ID_APP3,
                .x      = 120+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I332001_THEME_ICON1_TIMER_BIN,
                .res_click = 2,
                .res_switch = 2,
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
                .func_sta = FUNC_TIMER,
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            },
            [3] = {
                .idx    = COMPO_ID_APP4,
                .x      = 174+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I332001_THEME_ICON1_CALL_BIN,
                .res_click = 3,
                .res_switch = 3,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },
};

//获取卡片原始位置
static int func_clock_sub_card_get_y_info(int card_compo_id)
{
    int ret = 0;

    if (card_compo_id == ui_handle.card_clock_day.id)
    {
        ret = ui_handle.card_clock_day.y;
    }

    if (card_compo_id == ui_handle.card1.id)
    {
        ret = ui_handle.card1.y;
    }
    if (card_compo_id == ui_handle.card2.id)
    {
        ret = ui_handle.card2.y;
    }
    if (card_compo_id == ui_handle.card3.id)
    {
        ret = ui_handle.card3.y;
    }
    if (card_compo_id == ui_handle.card4.id)
    {
        ret = ui_handle.card4.y;
    }
    if (card_compo_id == ui_handle.card5.id)
    {
        ret = ui_handle.card5.y;
    }
    if (card_compo_id == ui_handle.card6.id)
    {
        ret = ui_handle.card6.y;
    }
    if (card_compo_id == ui_handle.card7.id)
    {
        ret = ui_handle.card7.y;
    }
    return ret;
}

//功能结构体
typedef struct f_card_t_
{
    s32 ofs_y;
    s32 ofs_y_drag;
    s16 focus_card;                  //靠近屏幕中央的卡片序号
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    s32 moveto_y;                   //设定自动移到的坐标
    s16 moveto_card;                //设定自动移到的卡片序号
    u32 tick;
    u32 activity_tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
//    co_timer_t music_refresh;
} f_card_t;

static void card1_updata_disp(void)
{
    f_card_t *f_activity = (f_card_t *)func_cb.f_cb;

//    if(tick_check_expire(f_activity->activity_tick, ))
//    {
//        f_activity->activity_tick = tick_get();
    char txt_buf[20];
    uint32_t totalStepCnt = 0;
    uint32_t Step_value = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    Step_value = totalStepCnt;
    if(Step_value>uteModuleSportGetStepsTargetCnt())
    {
        Step_value = uteModuleSportGetStepsTargetCnt();
    }

    compo_picturebox_t *arc_kcal = compo_getobj_byid(COMPO_ID_ARC_KCAL);
    compo_picturebox_t *arc_km   = compo_getobj_byid(COMPO_ID_ARC_KM);
    compo_picturebox_t *arc_step = compo_getobj_byid(COMPO_ID_ARC_STEP);
    compo_cardbox_t *cardbox = compo_getobj_byid(ui_handle.card1.id);

    compo_picturebox_set_visible(arc_kcal, true);
    compo_picturebox_set_visible(arc_km, true);
    compo_picturebox_set_visible(arc_step, true);


    if(f_activity->activity_state == 0)
    {
        f_activity->arc_step_value+=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value >=1800)
        {
            f_activity->activity_state =1;
        }
    }
    else if(f_activity->activity_state == 1)
    {

        f_activity->arc_step_value-=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value <= Step_value*1800 / uteModuleSportGetStepsTargetCnt())
        {
            f_activity->activity_state = 2;
        }

    }
    else
    {

        f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
        f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
        f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
        compo_cardbox_text_set(cardbox, 2, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
        compo_cardbox_text_set(cardbox, 1, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
        compo_cardbox_text_set(cardbox, 0, txt_buf);


        if(f_activity->arc_step_value)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);
        }
        else
        {
            compo_picturebox_set_visible(arc_kcal,false);
            compo_picturebox_set_visible(arc_km,false);
            compo_picturebox_set_visible(arc_step,false);
        }
    }

//    }
}
//根据序号获取最近任务序号（idx=0为最近，无任务返回0）(idx<=3)
static u8 card_get_latest_func(u8 idx)
{
    u8 i, j;
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};
    u8 latest_cnt = latest_task_count();
    if (latest_cnt > idx)
    {
        return latest_task_get(idx);
    }
    else
    {
        j = 0;
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)     //最近任务不足DEFAULT_LATEST_TASK_NUM个且包含默认值
        {
            if (latest_task_find(last_default[i]) == -1)
            {
                latest_default_tmp[j] = last_default[i];
                j++;
            }
        }
        return latest_default_tmp[idx - latest_cnt];
    }
}

//根据序号获取最近任务图标资源地址
static u32 card_get_latest_icon(u8 idx)
{
    return func_get_icon_addr(card_get_latest_func(idx));
}

//创建组件
static void func_clock_sub_card_compo_create(compo_form_t *frm)
{
    char txt_buf[20];
    compo_button_t *btn=NULL;

    ///卡片7
    compo_cardbox_t* card7 = compo_cardbox_create(frm, 1, 4, 1, ui_handle.card7.w, ui_handle.card7.h);
    compo_setid(card7, ui_handle.card7.id);

    compo_cardbox_rect_set_location(card7, ui_handle.card7.rect.idx, ui_handle.card7.rect.x, ui_handle.card7.rect.y, ui_handle.card7.rect.w, ui_handle.card7.rect.h, ui_handle.card7.rect.r);
    compo_cardbox_rect_set_color(card7, ui_handle.card7.rect.idx, make_color(ui_handle.card7.rect.color.r, ui_handle.card7.rect.color.g, ui_handle.card7.rect.color.b));

    compo_cardbox_text_set_font(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.res);
    compo_cardbox_text_set_align_center(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.center);
    widget_text_set_color(card7->text[ui_handle.card7.text_last_use.idx], make_color(ui_handle.card7.text_last_use.color.r, ui_handle.card7.text_last_use.color.g, ui_handle.card7.text_last_use.color.b));
    compo_cardbox_text_set_location(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.x, ui_handle.card7.text_last_use.y, ui_handle.card7.text_last_use.w, ui_handle.card7.text_last_use.h);
    compo_cardbox_text_set(card7, ui_handle.card7.text_last_use.idx, i18n[ui_handle.card7.text_last_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card7.pic_click)/sizeof(ui_handle.card7.pic_click[0]); i++)
    {
        btn = compo_button_create_page_by_image(frm,card7->page,card_get_latest_icon(i));
        compo_button_set_location(btn,ui_handle.card7.pic_click[i].x,ui_handle.card7.pic_click[i].y, ui_handle.card7.pic_click[i].w, ui_handle.card7.pic_click[i].h);
        compo_setid(btn,ui_handle.card7.pic_click[i].idx);
    }

    ///卡片6
    compo_cardbox_t* card6 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card6.w, ui_handle.card6.h);
    compo_setid(card6, ui_handle.card6.id);

    compo_cardbox_rect_set_location(card6, ui_handle.card6.rect.idx, ui_handle.card6.rect.x, ui_handle.card6.rect.y, ui_handle.card6.rect.w, ui_handle.card6.rect.h, ui_handle.card6.rect.r);
    compo_cardbox_rect_set_color(card6, ui_handle.card6.rect.idx, make_color(ui_handle.card6.rect.color.r, ui_handle.card6.rect.color.g, ui_handle.card6.rect.color.b));

    compo_cardbox_text_set_font(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.res);
    compo_cardbox_text_set_align_center(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.center);
    widget_text_set_color(card6->text[ui_handle.card6.text_comm_use.idx], make_color(ui_handle.card6.text_comm_use.color.r, ui_handle.card6.text_comm_use.color.g, ui_handle.card6.text_comm_use.color.b));
    compo_cardbox_text_set_location(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.x, ui_handle.card6.text_comm_use.y, ui_handle.card6.text_comm_use.w, ui_handle.card6.text_comm_use.h);
    compo_cardbox_text_set(card6, ui_handle.card6.text_comm_use.idx, i18n[ui_handle.card6.text_comm_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
    {
        compo_cardbox_icon_set_location(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].x, ui_handle.card6.pic_click[i].y, ui_handle.card6.pic_click[i].w, ui_handle.card6.pic_click[i].h);
        compo_cardbox_icon_set(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].res);
    }


    ///卡片5
    compo_cardbox_t* card5 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card5.w, ui_handle.card5.h);
    compo_setid(card5, ui_handle.card5.id);

    compo_cardbox_rect_set_location(card5, ui_handle.card5.rect.idx, ui_handle.card5.rect.x, ui_handle.card5.rect.y, ui_handle.card5.rect.w, ui_handle.card5.rect.h, ui_handle.card5.rect.r);
    compo_cardbox_rect_set_color(card5, ui_handle.card5.rect.idx, make_color(ui_handle.card5.rect.color.r, ui_handle.card5.rect.color.g, ui_handle.card5.rect.color.b));

//    compo_cardbox_text_set_font(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_stopwatch.idx], make_color(ui_handle.card5.text_stopwatch.color.r, ui_handle.card5.text_stopwatch.color.g, ui_handle.card5.text_stopwatch.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.x, ui_handle.card5.text_stopwatch.y, ui_handle.card5.text_stopwatch.w, ui_handle.card5.text_stopwatch.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_stopwatch.idx, i18n[ui_handle.card5.text_stopwatch.str_id]);

    compo_cardbox_text_set_font(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_time.idx], make_color(ui_handle.card5.text_time.color.r, ui_handle.card5.text_time.color.g, ui_handle.card5.text_time.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.x, ui_handle.card5.text_time.y, ui_handle.card5.text_time.w, ui_handle.card5.text_time.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_time.idx, "00:00.00");

    ///卡片4
    compo_cardbox_t* card4 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card4.w, ui_handle.card4.h);
    compo_setid(card4, ui_handle.card4.id);

    compo_cardbox_rect_set_location(card4, ui_handle.card4.rect.idx, ui_handle.card4.rect.x, ui_handle.card4.rect.y, ui_handle.card4.rect.w, ui_handle.card4.rect.h, ui_handle.card4.rect.r);
    compo_cardbox_rect_set_color(card4, ui_handle.card4.rect.idx, make_color(ui_handle.card4.rect.color.r, ui_handle.card4.rect.color.g, ui_handle.card4.rect.color.b));

    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_timer.idx], make_color(ui_handle.card4.text_timer.color.r, ui_handle.card4.text_timer.color.g, ui_handle.card4.text_timer.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.x, ui_handle.card4.text_timer.y, ui_handle.card4.text_timer.w, ui_handle.card4.text_timer.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_timer.idx, i18n[ui_handle.card4.text_timer.str_id]);

    compo_cardbox_text_set_font(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.res);
    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_time.idx], make_color(ui_handle.card4.text_time.color.r, ui_handle.card4.text_time.color.g, ui_handle.card4.text_time.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.x, ui_handle.card4.text_time.y, ui_handle.card4.text_time.w, ui_handle.card4.text_time.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_time.idx, "00:00:00");


    ///卡片3
    compo_cardbox_t* card3 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card3.w, ui_handle.card3.h);
    compo_setid(card3, ui_handle.card3.id);

    compo_cardbox_rect_set_location(card3, ui_handle.card3.rect.idx, ui_handle.card3.rect.x, ui_handle.card3.rect.y, ui_handle.card3.rect.w, ui_handle.card3.rect.h, ui_handle.card3.rect.r);
    compo_cardbox_rect_set_color(card3, ui_handle.card3.rect.idx, make_color(ui_handle.card3.rect.color.r, ui_handle.card3.rect.color.g, ui_handle.card3.rect.color.b));

    compo_cardbox_text_set_font(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.res);
    compo_cardbox_text_set_align_center(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.center);
    widget_text_set_color(card3->text[ui_handle.card3.text_music.idx], make_color(ui_handle.card3.text_music.color.r, ui_handle.card3.text_music.color.g, ui_handle.card3.text_music.color.b));
    compo_cardbox_text_set_location(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.x, ui_handle.card3.text_music.y, ui_handle.card3.text_music.w, ui_handle.card3.text_music.h);
    compo_cardbox_text_set(card3, ui_handle.card3.text_music.idx, i18n[ui_handle.card3.text_music.str_id]);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_prev.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_prev.x, ui_handle.card3.pic_click_prev.y, ui_handle.card3.pic_click_prev.w, ui_handle.card3.pic_click_prev.h);
    compo_setid(btn,ui_handle.card3.pic_click_prev.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_next.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_next.x, ui_handle.card3.pic_click_next.y, ui_handle.card3.pic_click_next.w, ui_handle.card3.pic_click_next.h);
    compo_setid(btn,ui_handle.card3.pic_click_next.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_play.res_click);
    compo_button_set_pos(btn,ui_handle.card3.pic_click_play.x, ui_handle.card3.pic_click_play.y);
    compo_setid(btn,ui_handle.card3.pic_click_play.idx);

    ///卡片2
    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);
    compo_cardbox_t* card2 = compo_cardbox_create(frm, 1, 0, 5, ui_handle.card2.w, ui_handle.card2.h);
    compo_setid(card2, ui_handle.card2.id);

    compo_cardbox_rect_set_location(card2, ui_handle.card2.rect.idx, ui_handle.card2.rect.x, ui_handle.card2.rect.y, ui_handle.card2.rect.w, ui_handle.card2.rect.h, ui_handle.card2.rect.r);
    compo_cardbox_rect_set_color(card2, ui_handle.card2.rect.idx, make_color(ui_handle.card2.rect.color.r, ui_handle.card2.rect.color.g, ui_handle.card2.rect.color.b));

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_sleep.idx], make_color(ui_handle.card2.text_sleep.color.r, ui_handle.card2.text_sleep.color.g, ui_handle.card2.text_sleep.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.x, ui_handle.card2.text_sleep.y, ui_handle.card2.text_sleep.w, ui_handle.card2.text_sleep.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_sleep.idx, i18n[ui_handle.card2.text_sleep.str_id]);

//    sleep_data->totalSleepMin =  9;
    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour.idx], make_color(ui_handle.card2.text_hour.color.r, ui_handle.card2.text_hour.color.g, ui_handle.card2.text_hour.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.x, ui_handle.card2.text_hour.y, ui_handle.card2.text_hour.w, ui_handle.card2.text_hour.h);
    memset(txt_buf,0,sizeof(txt_buf));
    if(sleep_data->totalSleepMin)   ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠小时*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour_unit.idx], make_color(ui_handle.card2.text_hour_unit.color.r, ui_handle.card2.text_hour_unit.color.g, ui_handle.card2.text_hour_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.x+6, ui_handle.card2.text_hour_unit.y, ui_handle.card2.text_hour_unit.w, ui_handle.card2.text_hour_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour_unit.idx, i18n[ui_handle.card2.text_hour_unit.str_id]);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min.idx], make_color(ui_handle.card2.text_min.color.r, ui_handle.card2.text_min.color.g, ui_handle.card2.text_min.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.x, ui_handle.card2.text_min.y, ui_handle.card2.text_min.w, ui_handle.card2.text_min.h);
    if(sleep_data->totalSleepMin)  ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠分钟*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_min.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min_unit.idx], make_color(ui_handle.card2.text_min_unit.color.r, ui_handle.card2.text_min_unit.color.g, ui_handle.card2.text_min_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.x+6, ui_handle.card2.text_min_unit.y, ui_handle.card2.text_min_unit.w, ui_handle.card2.text_min_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_min_unit.idx, i18n[ui_handle.card2.text_min_unit.str_id]);

    ///卡片1
    compo_cardbox_t* card1 = compo_cardbox_create(frm, 1, 3, 3, ui_handle.card1.w, ui_handle.card1.h);
    compo_setid(card1, ui_handle.card1.id);

    compo_cardbox_rect_set_location(card1, ui_handle.card1.rect.idx, ui_handle.card1.rect.x, ui_handle.card1.rect.y, ui_handle.card1.rect.w, ui_handle.card1.rect.h, ui_handle.card1.rect.r);
    compo_cardbox_rect_set_color(card1, ui_handle.card1.rect.idx, make_color(ui_handle.card1.rect.color.r, ui_handle.card1.rect.color.g, ui_handle.card1.rect.color.b));

    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.x, ui_handle.card1.pic_kcal.y, ui_handle.card1.pic_kcal.w, ui_handle.card1.pic_kcal.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.x, ui_handle.card1.pic_km.y, ui_handle.card1.pic_km.w, ui_handle.card1.pic_km.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.x, ui_handle.card1.pic_step.y, ui_handle.card1.pic_step.w, ui_handle.card1.pic_step.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.res);

    uint16_t KM = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_kcal.idx], make_color(ui_handle.card1.text_kcal.color.r, ui_handle.card1.text_kcal.color.g, ui_handle.card1.text_kcal.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.x, ui_handle.card1.text_kcal.y, ui_handle.card1.text_kcal.w, ui_handle.card1.text_kcal.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
    compo_cardbox_text_set(card1, ui_handle.card1.text_kcal.idx, txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_km.idx], make_color(ui_handle.card1.text_km.color.r, ui_handle.card1.text_km.color.g, ui_handle.card1.text_km.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.x, ui_handle.card1.text_km.y, ui_handle.card1.text_km.w, ui_handle.card1.text_km.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",KM/100, KM%100);///公里数据
    compo_cardbox_text_set(card1, ui_handle.card1.text_km.idx,txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_step.idx], make_color(ui_handle.card1.text_step.color.r, ui_handle.card1.text_step.color.g, ui_handle.card1.text_step.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.x, ui_handle.card1.text_step.y, ui_handle.card1.text_step.w, ui_handle.card1.text_step.h);
    snprintf(txt_buf,sizeof(txt_buf),"%ld",totalStepCnt);
    compo_cardbox_text_set(card1, ui_handle.card1.text_step.idx, txt_buf);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I332001_FIRSTORDER_ACTIVITY_BG_BIN);
    widget_page_t * widget_page = widget_page_create(card1->page);///创建按键页面
    widget_set_location(widget_page,52,0,pic_bg_area.wid,pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_BG_BIN);//彩虹背景
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_RED_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KCAL);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_YELLOW_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KM);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_BLUE_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_STEP);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_YUANJIAO1_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I332001_FIRSTORDER_ACTIVITY_YUANJIAO1_BIN).hei );

    ///时钟 日期
    compo_picturebox_t* clock_bg = compo_picturebox_create(frm, ui_handle.card_clock_day.clock.bg_res);
    compo_setid(clock_bg, ui_handle.card_clock_day.clock.id_bg);

    compo_datetime_t* clock_h = compo_datetime_create(frm, ui_handle.card_clock_day.clock.h_res);
    compo_setid(clock_h, ui_handle.card_clock_day.clock.id_h);
    compo_datetime_set_center(clock_h, 13, ui_handle.card_clock_day.clock.h_h / 2);
    compo_datetime_set_start_angle(clock_h, 900);
    compo_bonddata(clock_h, ui_handle.card_clock_day.clock.h_bonddata);

    compo_datetime_t* clock_m = compo_datetime_create(frm, ui_handle.card_clock_day.clock.m_res);
    compo_setid(clock_m, ui_handle.card_clock_day.clock.id_min);
    compo_datetime_set_center(clock_m, 13, ui_handle.card_clock_day.clock.m_h / 2);
    compo_datetime_set_start_angle(clock_m, 900);
    compo_bonddata(clock_m, ui_handle.card_clock_day.clock.m_bonddata);

    compo_textbox_t* date = compo_textbox_create(frm, 5);                                 //日期
    compo_setid(date, ui_handle.card_clock_day.date.id);
    compo_textbox_set_font(date, ui_handle.card_clock_day.date.res);
    compo_textbox_set_align_center(date, ui_handle.card_clock_day.date.center);
    widget_text_set_color(date->txt, make_color(ui_handle.card_clock_day.date.color.r, ui_handle.card_clock_day.date.color.g, ui_handle.card_clock_day.date.color.b));
    compo_bonddata(date, ui_handle.card_clock_day.date.bonddata);

    compo_textbox_t* week = compo_textbox_create(frm, 20);                                 //日期
    compo_setid(week, ui_handle.card_clock_day.week.id);
    compo_textbox_set_font(week, ui_handle.card_clock_day.week.res);
    compo_textbox_set_align_center(week, ui_handle.card_clock_day.week.center);
    widget_text_set_color(week->txt, make_color(ui_handle.card_clock_day.week.color.r, ui_handle.card_clock_day.week.color.g, ui_handle.card_clock_day.week.color.b));
    compo_bonddata(week, ui_handle.card_clock_day.week.bonddata);

//    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 6, 36, 6);
//    compo_shape_set_color(shape, make_color(0X4C,0X4C,0X4C));
//    compo_shape_set_radius(shape, 35);

    ab_free(sleep_data);

}

//更新组件位置和大小
static void func_clock_sub_card_compo_update(s32 ofs_y, bool creating)
{
    s16 x, y, w, h, card_y, card_w;
    u8 card_id = 0;
    u16 dc;  //当前卡片中心距屏幕中心距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL)
    {
        //获取初始值
        switch (compo->id)
        {
            case COMPO_ID_CLOCK_BG:
                if (ui_handle.card_clock_day.clock.id_bg != COMPO_ID_CLOCK_BG)
                {
                    printf("COMPO_ID_CLOCK_BG ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.bg_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.bg_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.bg_w;
                h = ui_handle.card_clock_day.clock.bg_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_H:
                if (ui_handle.card_clock_day.clock.id_h != COMPO_ID_CLOCK_H)
                {
                    printf("COMPO_ID_CLOCK_H ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.h_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.h_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.h_w;
                h = ui_handle.card_clock_day.clock.h_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_M:
                if (ui_handle.card_clock_day.clock.id_min != COMPO_ID_CLOCK_M)
                {
                    printf("COMPO_ID_CLOCK_M ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.m_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.m_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.m_w;
                h = ui_handle.card_clock_day.clock.m_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_DATE:
                if (ui_handle.card_clock_day.date.id != COMPO_ID_DATE)
                {
                    printf("COMPO_ID_DATE ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.date.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.date.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.date.w;
                h = ui_handle.card_clock_day.date.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_WEEKDAY:
                if (ui_handle.card_clock_day.week.id != COMPO_ID_WEEKDAY)
                {
                    printf("COMPO_ID_WEEKDAY ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.week.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.week.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.week.w;
                h = ui_handle.card_clock_day.week.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CARD_1:
                if (ui_handle.card1.id != COMPO_ID_CARD_1)
                {
                    printf("COMPO_ID_CARD_1 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card1.x;
                y = card_y = ui_handle.card1.y;
                w = ui_handle.card1.w;
                h = ui_handle.card1.h;
                card_id = ui_handle.card1.id;
                break;

            case COMPO_ID_CARD_2:
                if (ui_handle.card2.id != COMPO_ID_CARD_2)
                {
                    printf("COMPO_ID_CARD_2 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card2.x;
                y = card_y = ui_handle.card2.y;
                w = ui_handle.card2.w;
                h = ui_handle.card2.h;
                card_id = ui_handle.card2.id;
                break;

            case COMPO_ID_CARD_3:
                if (ui_handle.card3.id != COMPO_ID_CARD_3)
                {
                    printf("COMPO_ID_CARD_3 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card3.x;
                y = card_y = ui_handle.card3.y;
                w = ui_handle.card3.w;
                h = ui_handle.card3.h;
                card_id = ui_handle.card3.id;
                break;

            case COMPO_ID_CARD_4:
                if (ui_handle.card4.id != COMPO_ID_CARD_4)
                {
                    printf("COMPO_ID_CARD_4 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card4.x;
                y = card_y = ui_handle.card4.y;
                w = ui_handle.card4.w;
                h = ui_handle.card4.h;
                card_id = ui_handle.card4.id;
                break;

            case COMPO_ID_CARD_5:
                if (ui_handle.card5.id != COMPO_ID_CARD_5)
                {
                    printf("COMPO_ID_CARD_5 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card5.x;
                y = card_y = ui_handle.card5.y;
                w = ui_handle.card5.w;
                h = ui_handle.card5.h;
                card_id = ui_handle.card5.id;
                break;


            case COMPO_ID_CARD_6:
                if (ui_handle.card6.id != COMPO_ID_CARD_6)
                {
                    printf("COMPO_ID_CARD_6 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card6.x;
                y = card_y = ui_handle.card6.y;
                w = ui_handle.card6.w;
                h = ui_handle.card6.h;
                card_id = ui_handle.card6.id;
                break;


            case COMPO_ID_CARD_7:
                if (ui_handle.card7.id != COMPO_ID_CARD_7)
                {
                    printf("COMPO_ID_CARD_7 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card7.x;
                y = card_y = ui_handle.card7.y;
                w = ui_handle.card7.w;
                h = ui_handle.card7.h;
                card_id = ui_handle.card7.id;
                break;

            default:
                compo = compo_get_next(compo);
                continue;
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH_ORG;
        card_y += ofs_y;
        y += ofs_y;
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);
        if (creating == false && dc < dc_min)    //焦点，创建时不使用f_cb
        {
            f_card_t *f_card = (f_card_t *)func_cb.f_cb;
            f_card->focus_card = card_id;
            dc_min = dc;
//            TRACE("focus[%d]\n", card_id);
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM)
        {
            if (card_y > CARD_SCALE_STOP_Y)    //缩到最小后保持一段时间
            {
                y -= card_y - CARD_SCALE_STOP_Y;
                card_y = CARD_SCALE_STOP_Y;
            }
            if (card_w > CARD_WIDTH_MIN)
            {
                //缩放
                s16 card_w_org = card_w;
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                w = w * card_w / card_w_org;
                h = h * card_w / card_w_org;
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org;
                y += (card_y - y) * (card_w_org - card_w) / card_w_org;
                //偏移
                s16 card_y_org = card_y;
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                y += card_y - card_y_org;
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM;
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level));   //背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level));
            if (unvisible)
            {
                compo_cardbox_set_visible(cardbox, false);
            }
            else
            {
                compo_cardbox_set_visible(cardbox, true);
            }
        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_set_pos(pic, x, y);
            compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_picturebox_set_visible(pic, false);
            }
            else
            {
                compo_picturebox_set_visible(pic, true);
            }
        }
        else if (compo->type == COMPO_TYPE_TEXTBOX)
        {
            compo_textbox_t *txt = (compo_textbox_t *)compo;
            compo_textbox_set_location(txt, x, y, w, h);
            if (unvisible)
            {
                compo_textbox_set_visible(txt, false);
            }
            else
            {
                compo_textbox_set_visible(txt, true);
            }
        }
        else if (compo->type == COMPO_TYPE_DATETIME)
        {
            compo_datetime_t *dt = (compo_datetime_t *)compo;
            compo_datetime_set_pos(dt, x, y);
        }

        compo = compo_get_next(compo);          //遍历组件
    }
}

//创建上拉卡片菜单（文件内调用）
static compo_form_t *func_clock_sub_card_form_create_by_ofs(s16 ofs_y)
{
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
//    compo_shape_set_alpha(masklayer, 160);
    //创建所有组件
    func_clock_sub_card_compo_create(frm);
    //更新组件位置和大小
    func_clock_sub_card_compo_update(ofs_y, true);

    return frm;
}

//创建上拉卡片菜单（供外部调用）
compo_form_t *func_clock_sub_card_form_create(void)
{
    return func_clock_sub_card_form_create_by_ofs(SPRING_Y_MAX);
}

//获取点击的卡片组件id
static u16 func_clock_sub_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_BTN_COUNT; i++)
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;
        id = COMPO_CARD_START + 1 + i;
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

//表盘上拉卡片菜单点击处理
static void func_clock_sub_card_click_handler(void)
{
    compo_cardbox_t *cardbox = NULL;
//    rect_t rect;
    u8 func_jump = FUNC_NULL;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_clock_sub_card_get_btn_id(pt);
    printf("click compo_id:%d\n", compo_id);
    int id = compo_get_button_id();

    switch(compo_id)
    {
        case COMPO_ID_CARD_1 :
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            func_jump = FUNC_ACTIVITY;
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            break;
        case COMPO_ID_CARD_2 :
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
            func_jump = FUNC_SLEEP;
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
            break;
        case COMPO_ID_CARD_3:
            switch(id)
            {
                case COMPO_MUSIC_BTN_PREV:
                {
                    uteModuleMusicCtrl(false,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV);
                    compo_button_set_bgimg(btn,ui_handle.card3.pic_click_prev.res);
                }
                break;
                case COMPO_MUSIC_BTN_PLAY:
                {
                    uteModuleMusicCtrlPaused(false);
                    if(ble_is_connect())
                    {
                        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
                    }
//                    if(bt_cb.music_playing)
//                    {
//                        compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY);
//                        compo_button_set_bgimg(btn, ui_handle.card3.pic_click_play.res_click);
//                    }
                }
                break;
                case COMPO_MUSIC_BTN_NEXT:
                {
                    uteModuleMusicCtrl(true,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);
                    compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res);
                }
                break;
                default:
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    func_jump = FUNC_BT;
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    break;
            }
            break;
        case COMPO_ID_CARD_4 :
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
            func_jump = FUNC_TIMER;
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            break;
        case COMPO_ID_CARD_5 :
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            func_jump = FUNC_STOPWATCH;
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            break;
        case COMPO_ID_CARD_6 :
            cardbox = compo_getobj_byid(COMPO_ID_CARD_6);
            for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
            {
                rect_t rect = compo_cardbox_get_icon_absolute(cardbox, ui_handle.card6.pic_click[i].idx); //上一首
                if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
                {
//                    printf("rect = [%d,%d,%d,%d]\n", rect.x, rect.y, rect.wid, rect.hei);
                    func_cb.sta = ui_handle.card6.pic_click[i].func_sta;
                }
            }
            break;
        case COMPO_ID_CARD_7 :
            printf("id : %d\n", id);
            if (id >= COMPO_ID_APP1)
            {
                func_jump = card_get_latest_func(id-COMPO_ID_APP1);
            }
            break;
        default:
            break;
    }
    if(func_jump != FUNC_NULL)
    {
        printf("func_jump=%d\n", func_jump);
        func_cb.sta = func_jump;  //直接跳转
    }
}

//根据偏移位置把最近的卡片对齐到屏幕中央
static s16 func_clock_sub_card_align_y(s16 ofs_y)
{
    u8 i, nearest_id = 0;
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    u16 dc;
    if (ofs_y >= DRAG_Y_MAX)
    {
        return DRAG_Y_MAX;  //回弹效果
    }
    else if (ofs_y <= DRAG_Y_MIN)
    {
        return DRAG_Y_MIN;
    }
//    for (i=0; i<CARD_COUNT; i++)
    for (i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
//        dc = abs_s(card_y_info[i] + ofs_y - GUI_SCREEN_CENTER_Y);
        dc = abs_s(func_clock_sub_card_get_y_info(COMPO_CARD_START+1+i) + ofs_y - GUI_SCREEN_CENTER_Y);
        if (dc < dc_min)
        {
            nearest_id = i;
            dc_min = dc;
        }
    }
//    return (GUI_SCREEN_CENTER_Y - card_y_info[nearest_id]);
    return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+nearest_id));
}

//根据卡片序号对齐页面偏移位置到屏幕中央
static s16 func_clock_sub_card_align_by_idx(s8 card_id)
{
//    if (card_id <= CARD_ID_CLOCK)
    if (card_id <= COMPO_CARD_START)
    {
        return DRAG_Y_MAX;
    }
//    else if (card_id > (CARD_COUNT - 1))
    else if (card_id > (COMPO_CARD_END-COMPO_CARD_START-1 - 1))
    {
        return DRAG_Y_MIN;
    }
    else
    {
//        return (GUI_SCREEN_CENTER_Y - card_y_info[card_id]);
        return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+card_id));
    }
}

//限制滑动偏移位置
static s16 func_clock_sub_card_limit_y(s16 ofs_y)
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));
    return ofs_y;
}

//设置页面偏移位置
static void func_clock_sub_card_set_offs(s16 ofs_y)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    f_card->ofs_y = ofs_y;
    func_clock_sub_card_compo_update(f_card->ofs_y, false);
}

//时钟表盘上拉菜单数据更新----------------------------------------->todo
static void func_clock_sub_card_data_update(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    compo_cardbox_t *cardbox = NULL;
    char txt_buf[20];

    if(f_card->flag_drag == 0)
    {
        card1_updata_disp();
    }

//    if(f_activity->activity_state != 2) return;

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",SEC_TO_HOUR(sys_cb.timer_left_sec),SEC_TO_MIN(sys_cb.timer_left_sec),SEC_TO_SEC(sys_cb.timer_left_sec));
    cardbox = compo_getobj_byid(ui_handle.card4.id);
    compo_cardbox_text_set(cardbox, ui_handle.card4.text_time.idx,txt_buf);

    u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    u16 msec = sys_cb.stopwatch_total_msec % 1000;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min  = 99;
        sec  = 59;
        msec = 999;
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld.%02ld",min,sec,msec/10);
    cardbox = compo_getobj_byid(ui_handle.card5.id);
    compo_cardbox_text_set(cardbox, ui_handle.card5.text_time.idx,txt_buf);
}

//下拉返回表盘
static void func_clock_sub_card_switch_to_clock(bool auto_switch)
{
    printf("%s\n", __func__);
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    u16 switch_mode = FUNC_SWITCH_MENU_PULLUP_DOWN | (auto_switch ? FUNC_SWITCH_AUTO : 0);
    compo_form_destroy(func_cb.frm_main);
    compo_form_t * frm_clock = func_create_form(FUNC_CLOCK);
    compo_form_t * frm = func_clock_sub_card_form_create_by_ofs(f_card->ofs_y);
    func_cb.frm_main = frm;
    if (func_switching(switch_mode, NULL))
    {
        func_cb.sta = FUNC_CLOCK;
    }
    compo_form_destroy(frm_clock);
}

void music_data_refresh(void)
{
    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }

    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY); //播放/暂停
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I332001_FIRSTORDER_MUSIC_PAUSED_BIN : ui_handle.card3.pic_click_play.res);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_play.res_click);
    }
}

#endif


#if 0

#elif 1
#define CARD_SCALE_START_POS            190//265                                                                     //卡片开始缩小位置
#define CARD_SCALE_STOP_POS             230//320                                                                     //卡片停止缩小位置
#define CARD_SCALE_STOP_Y               (CARD_SCALE_STOP_POS + 156 * 2)                                         //卡片停止缩小实际位置（越大缩小越慢，可见叠加越多）
#define CARD_WIDTH_MIN                  150//200                                                                     //卡片缩小的最小宽度
#define CARD_UNVISIBLE_POS_TOP          (-100)//(-160)                                                                  //卡片超出屏幕顶部某位置后设为不可见
#define CARD_UNVISIBLE_POS_BOTTOM       (CARD_SCALE_STOP_Y + 156)                                               //卡片缩到最小后实际位置大于某值设为不可见
#define CARD_BG_GRAY_START              41                                                                      //卡片开始缩小位置处背景的灰度
#define CARD_BG_GRAY_STOP               10                                                                      //卡片停止缩小位置处背景的灰度
#define GRAY_LV(x)                      make_color(x, x, x)                                                     //生成灰度颜色值（0:黑 255:白）

#define SPRING_Y_MAX                    0                                                                       //回弹到顶部时的ofs_y
//#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - card_y_info[CARD_COUNT - 1] + 50)                     //回弹到底部时的ofs_y
#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_END-1) + 50)                     //回弹到底部时的ofs_y
#define DRAG_Y_MAX                      (SPRING_Y_MAX + 30)                                                     //拖动到顶部时的ofs_y
#define DRAG_Y_MIN                      (SPRING_Y_MIN - 78)                                                     //拖动到底部时的ofs_y

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 16                                                                      //拖动松手后的速度(惯性)

#define EXIT_Y_MIN                      0
#define EXIT_Y_MAX                      50//60                                                                      //下滑退出触摸点离屏幕顶部小于某值
#define TXT_CNT_MAX                     8                                                                       //文本框最大字符数
#define CARD_BTN_COUNT                  (COMPO_CARD_END - COMPO_CARD_START - 1)    //卡片（按钮）数量

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

#define DEFAULT_LATEST_TASK_NUM 5   //最近任务不足5个时默认值补上
// const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY,FUNC_WEATHER};
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_CALCULATOR, FUNC_MESSAGE,FUNC_WEATHER, FUNC_ALARM_CLOCK,FUNC_WEATHER};
///卡片、组件原始位置（相对于卡片中心点）
#define CARD_WIDTH_ORG                  208//308

typedef struct ui_handle_t_
{
    struct card_clock_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct date_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            u8 bonddata;
            bool center;
            color_t color;
            u32 res;
        } date;

        struct week_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            bool center;
            u8 bonddata;
            color_t color;
            u32 res;
        } week;

        struct clock_t
        {
            u16 id_bg;
            u16 id_h;
            u16 id_min;
            s16 bg_x,bg_y,h_x,h_y,m_x,m_y;
            u16 bg_w,bg_h,h_w,h_h,m_w,m_h;
            u32 bg_res, h_res, m_res;
            u8 h_bonddata, m_bonddata;
        } clock;
    } card_clock_day;

    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
            color_t color;
        } rect;

        struct pic_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
        } pic_kcal;
        struct pic_t pic_km;
        struct pic_t pic_step;

        struct sport_arc_t
        {
            u16 id;
            s16 x,y;
            u16 w,r;
            color_t color;
            color_t bg_color;
            u16 zero_angle;
            u16 angle_min;
            u16 angle_max;
        } arc_kcal;
        struct sport_arc_t arc_km;
        struct sport_arc_t arc_step;

        struct text_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u16 str_id;
            bool center;
            color_t color;
        } text_kcal;
        struct text_t text_km;
        struct text_t text_step;
    } card1;

    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_sleep;
        struct text_t text_hour;
        struct text_t text_hour_unit;
        struct text_t text_min;
        struct text_t text_min_unit;
    } card2;

    struct card3_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_music;

        struct pic_click_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u32 res_click;
            u32 res_switch;
            u16 func_sta;
        } pic_click_prev;
        struct pic_click_t pic_click_next;
        struct pic_click_t pic_click_play;
    } card3;

    struct card4_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_timer;
        struct text_t text_time;
    } card4;

    struct card5_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_stopwatch;
        struct text_t text_time;
    } card5;

    struct card6_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_comm_use;
        struct pic_click_t pic_click[3];
    } card6;

    struct card7_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_last_use;
        struct pic_click_t pic_click[3];
    } card7;

} ui_handle_t;
//组件ID
enum
{
    COMPO_ID_CLOCK_BG = 256,
    COMPO_ID_CLOCK_H,
    COMPO_ID_CLOCK_M,
    COMPO_ID_DATE,
    COMPO_ID_WEEKDAY,

    COMPO_CARD_START,
    COMPO_ID_CARD_1,
    COMPO_ID_CARD_2,
    // COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    COMPO_ID_CARD_5,
    COMPO_ID_CARD_6,
    COMPO_ID_CARD_7,
    COMPO_CARD_END,

    COMPO_ID_ARC_KCAL,
    COMPO_ID_ARC_KM,
    COMPO_ID_ARC_STEP,

    COMPO_MUSIC_BTN_PREV,
    COMPO_MUSIC_BTN_NEXT,
    COMPO_MUSIC_BTN_PLAY,

    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
    COMPO_ID_APP4,
};

#define SUB_CARD_W 208
#define SUB_CARD_H 94
#define SUB_CARD_X 16
// #define SUB_CARD_Y
static const ui_handle_t ui_handle =
{
    .card_clock_day = {
        .id = COMPO_CARD_START,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 24+100/2,
        .w  = 0,
        .h  = 0,

        .date = {
            .id = COMPO_ID_DATE,
            .x  = SUB_CARD_X-SUB_CARD_W/2,
            .y  = 41-136/2,
            .w  = 80+20,
            .h  = 40,
            .center = false,
            .bonddata = COMPO_BOND_DATE,
            .color = {255,255,255},
            .res = UI_BUF_0FONT_FONT_NUM_32_BIN,
        },

        .week = {
            .id = COMPO_ID_WEEKDAY,
            .x  = SUB_CARD_X-SUB_CARD_W/2,
            .y  = 79-136/2,
            .w  = 45,
            .h  = 32,
            .center = false,
            .bonddata = COMPO_BOND_WEEKDAY,
            .color = {153,153,153},
            .res = UI_BUF_0FONT_FONT_BIN,
        },

        .clock = {
            .id_bg  = COMPO_ID_CLOCK_BG,
            .id_h   = COMPO_ID_CLOCK_H,
            .id_min = COMPO_ID_CLOCK_M,
            .bg_x   = 100+100/2-SUB_CARD_W/2,
            .bg_y   = 0,
            .h_x    = 100+96/2-SUB_CARD_W/2,
            .h_y    = 0,
            .m_x    = 100+96/2-SUB_CARD_W/2,
            .m_y    = 0,
            .bg_w   = 100,
            .bg_h   = 100,
            .h_w    = 53,
            .h_h    = 18,
            .m_w    = 65,
            .m_h    = 18,
            .bg_res = UI_BUF_I330001_FIRSTORDER_TIME_BG_BIN,
            .h_res  = UI_BUF_I330001_FIRSTORDER_TIME_H_BIN,
            .m_res  = UI_BUF_I330001_FIRSTORDER_TIME_M_BIN,
            .h_bonddata = COMPO_BOND_HOUR,
            .m_bonddata = COMPO_BOND_MINUTE,
        },
    },

    .card1 = {
        .id = COMPO_ID_CARD_1,
        .x  = SUB_CARD_X+SUB_CARD_W/2,
        .y  = 190,
        .w  = SUB_CARD_W,
        .h  = SUB_CARD_H,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = SUB_CARD_W,
            .h      = SUB_CARD_H,
            .r      = 16,
            .color  = {41,41,41},
        },
        .pic_kcal = {
            .idx    = 0,
            .x      = 10+18/2-SUB_CARD_W/2,
            .y      = 21+18/2-SUB_CARD_H/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_CALORIES_BIN,
        },
        .pic_km = {
            .idx    = 1,
            .x      = 10+18/2-SUB_CARD_W/2,
            .y      = 46+18/2-SUB_CARD_H/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_DIS_BIN,
        },
        .pic_step = {
            .idx    = 2,
            .x      = 10+18/2-SUB_CARD_W/2,
            .y      = 71+18/2-SUB_CARD_H/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_STEPT_BIN,
        },

        .arc_kcal = {
            .id     = COMPO_ID_ARC_KCAL,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100,
            .color  = {252,55,40},
            .bg_color = {0x7f,0x6e,0x19},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_km = {
            .id     = COMPO_ID_ARC_KM,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-12,
            .color  = {255,212,0},
            .bg_color = {0x19,0x7a,0x6f},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_step = {
            .id     = COMPO_ID_ARC_STEP,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-24,
            .color  = {0,242,214},
            .bg_color = {0x7e,0x2f,0x29},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .text_kcal = {
            .idx    = 0,
            .x      = 34-SUB_CARD_W/2,
            .y      = 21+18/2-SUB_CARD_H/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 0,
            .center = false,
            .color  = {252,55,40},
        },
        .text_km = {
            .idx    = 1,
            .x      = 34-SUB_CARD_W/2,
            .y      = 46+18/2-SUB_CARD_H/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 1,
            .center = false,
            .color  = {255,212,0},
        },
        .text_step  = {
            .idx    = 2,
            .x      = 34-SUB_CARD_W/2,
            .y      = 71+18/2-SUB_CARD_H/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 2,
            .center = false,
            .color  = {0,242,214},
        },
    },

    .card2 = {
        .id = COMPO_ID_CARD_2,
        .x  = SUB_CARD_X+SUB_CARD_W/2,
        .y  = 190+(SUB_CARD_H+6)*1,
        .w  = SUB_CARD_W,
        .h  = SUB_CARD_H,
        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = SUB_CARD_W,
            .h      = SUB_CARD_H,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_sleep = {
            .idx    = 0,
            .x      = 12-SUB_CARD_W/2,
            .y      = 10-SUB_CARD_H/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_SLEEP,
            .center = false,
            .color  = {100,95,255},
        },
        .text_hour = {
            .idx    = 1,
            .x      = 12-SUB_CARD_W/2,
            .y      = 38-SUB_CARD_H/2+6,
            .w      = 41+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_hour_unit = {
            .idx    = 2,
            .x      = 56-SUB_CARD_W/2,
            .y      = 53-SUB_CARD_H/2,
            .w      = 46,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_HOUR,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min = {
            .idx    = 3,
            .x      = 97-SUB_CARD_W/2+14,
            .y      = 38-SUB_CARD_H/2+6,
            .w      = 40+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min_unit = {
            .idx    = 4,
            .x      = 140-SUB_CARD_W/2+12,
            .y      = 53-SUB_CARD_H/2,
            .w      = 50,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MIN,
            .center = false,
            .color  = {255,255,255},
        },
    },

    // .card3 = {
    //     .id = COMPO_ID_CARD_3,
    //     .x  = SUB_CARD_X+SUB_CARD_W/2,
    //     .y  = 190+(SUB_CARD_H+6)*2,
    //     .w  = SUB_CARD_W,
    //     .h  = SUB_CARD_H,

    //     .rect = {
    //         .idx    = 0,
    //         .x      = 0,
    //         .y      = 0,
    //         .w      = SUB_CARD_W,
    //         .h      = SUB_CARD_H,
    //         .r      = 16,
    //         .color  = {41,41,41},
    //     },

    //     .text_music = {
    //         .idx    = 0,
    //         .x      = 12-SUB_CARD_W/2,
    //         .y      = 10-SUB_CARD_H/2,
    //         .w      = 200,
    //         .h      = 30,
    //         .res    = UI_BUF_0FONT_FONT_BIN,
    //         .str_id = STR_MUSIC,
    //         .center = false,
    //         .color  = {255,48,96},
    //     },

    //     .pic_click_prev = {
    //         .idx    = COMPO_MUSIC_BTN_PREV,
    //         .x      = 26+28/2-SUB_CARD_W/2,
    //         .y      = 52+28/2-SUB_CARD_H/2,
    //         .w      = 28,
    //         .h      = 28,
    //         .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_UP00_BIN,
    //         .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_UP01_BIN,
    //         .res_switch = 0,
    //         .func_sta = 0,
    //     },
    //     .pic_click_next = {
    //         .idx    = COMPO_MUSIC_BTN_NEXT,
    //         .x      = 178+28/2-SUB_CARD_W/2,
    //         .y      = 52+28/2-SUB_CARD_H/2,
    //         .w      = 28,
    //         .h      = 28,
    //         .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_NEXT00_BIN,
    //         .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_NEXT01_BIN,
    //         .res_switch = 0,
    //         .func_sta = 0,
    //     },
    //     .pic_click_play = {
    //         .idx    = COMPO_MUSIC_BTN_PLAY,
    //         .x      = 88+56/2-SUB_CARD_W/2,
    //         .y      = 38+56/2-SUB_CARD_H/2,
    //         .w      = 28,
    //         .h      = 28,
    //         .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY00_BIN,
    //         .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY01_BIN,
    //         .res_switch = UI_BUF_I330001_FIRSTORDER_MUSIC_PAUSED_BIN,
    //         .func_sta = 0,
    //     },
    // },

    .card4 = {
        .id = COMPO_ID_CARD_4,
        .x  = SUB_CARD_X+SUB_CARD_W/2,
        .y  = 190+(SUB_CARD_H+6)*2,
        .w  = SUB_CARD_W,
        .h  = SUB_CARD_H,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = SUB_CARD_W,
            .h      = SUB_CARD_H,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_timer = {
            .idx    = 0,
            .x      = 12-SUB_CARD_W/2,
            .y      = 10-SUB_CARD_H/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_TIMER,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-SUB_CARD_W/2,
            .y      = 37-SUB_CARD_H/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card5 = {
        .id = COMPO_ID_CARD_5,
        .x  = SUB_CARD_X+SUB_CARD_W/2,
        .y  = 190+(SUB_CARD_H+6)*3,
        .w  = SUB_CARD_W,
        .h  = SUB_CARD_H,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = SUB_CARD_W,
            .h      = SUB_CARD_H,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_stopwatch = {
            .idx    = 0,
            .x      = 12-SUB_CARD_W/2,
            .y      = 10-SUB_CARD_H/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_STOP_WATCH,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-SUB_CARD_W/2,
            .y      = 37-SUB_CARD_H/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card6 = {
        .id = COMPO_ID_CARD_6,
        .x  = SUB_CARD_X+SUB_CARD_W/2,
        .y  = 190+(SUB_CARD_H+6)*4,
        .w  = SUB_CARD_W,
        .h  = SUB_CARD_H,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = SUB_CARD_W,
            .h      = SUB_CARD_H,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_comm_use = {
            .idx    = 0,
            .x      = 12-SUB_CARD_W/2,
            .y      = 10-SUB_CARD_H/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_COMMON_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = 0,
                .x      = 27+48/2-SUB_CARD_W/2,
                .y      = 41+48/2-SUB_CARD_H/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I330001_FIRSTORDER_FIXED_SPORT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },

            [1] = {
                .idx    = 1,
                .x      = 91+48/2-SUB_CARD_W/2,
                .y      = 41+48/2-SUB_CARD_H/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I330001_FIRSTORDER_FIXED_MSM_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
                .func_sta = FUNC_MESSAGE,
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            },

            [2] = {
                .idx    = 2,
                .x      = 155+48/2-SUB_CARD_W/2,
                .y      = 41+48/2-SUB_CARD_H/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I330001_FIRSTORDER_FIXED_CALL_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },

    .card7 = {
        .id = COMPO_ID_CARD_7,
        .x  = SUB_CARD_X+SUB_CARD_W/2,
        .y  = 190+(SUB_CARD_H+6)*5+7,
        .w  = SUB_CARD_W,
        .h  = SUB_CARD_H,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = SUB_CARD_W,
            .h      = SUB_CARD_H,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_last_use = {
            .idx    = 0,
            .x      = 12-SUB_CARD_W/2,
            .y      = 10-SUB_CARD_H/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_LATEST_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = COMPO_ID_APP1,
                .x      = 12+44/2-SUB_CARD_W/2,
                .y      = 43+44/2-SUB_CARD_H/2,
                .w      = 50,
                .h      = 50,
                .res    = UI_BUF_I330001_THEME_1_SOPRT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },
            [1] = {
                .idx    = COMPO_ID_APP2,
                .x      = 66+44/2-SUB_CARD_W/2,
                .y      = 43+44/2-SUB_CARD_H/2,
                .w      = 50,
                .h      = 50,
                .res    = UI_BUF_I330001_THEME_1_BRIGHTNESS_BIN,
                .res_click = 1,
                .res_switch = 1,
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
                .func_sta = FUNC_LIGHT,
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            },
            [2] = {
                .idx    = COMPO_ID_APP3,
                .x      = 120+44/2-SUB_CARD_W/2,
                .y      = 43+44/2-SUB_CARD_H/2,
                .w      = 50,
                .h      = 50,
                .res    = UI_BUF_I330001_THEME_1_TIMER_BIN,
                .res_click = 2,
                .res_switch = 2,
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
                .func_sta = FUNC_TIMER,
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            },
//             [3] = {
//                 .idx    = COMPO_ID_APP4,
//                 .x      = 174+44/2-SUB_CARD_W/2,
//                 .y      = 43+44/2-SUB_CARD_H/2,
//                 .w      = 46,
//                 .h      = 46,
//                 .res    = UI_BUF_I330001_THEME_1_CALL_BIN,
//                 .res_click = 3,
//                 .res_switch = 3,
// #if UTE_MODULE_SCREENS_CALL_SUPPORT
//                 .func_sta = FUNC_CALL,
// #endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            // },
        },
    },
};

//获取卡片原始位置
static int func_clock_sub_card_get_y_info(int card_compo_id)
{
    int ret = 0;

    if (card_compo_id == ui_handle.card_clock_day.id)
    {
        ret = ui_handle.card_clock_day.y;
    }

    if (card_compo_id == ui_handle.card1.id)
    {
        ret = ui_handle.card1.y;
    }
    if (card_compo_id == ui_handle.card2.id)
    {
        ret = ui_handle.card2.y;
    }
    // if (card_compo_id == ui_handle.card3.id)
    // {
    //     ret = ui_handle.card3.y;
    // }
    if (card_compo_id == ui_handle.card4.id)
    {
        ret = ui_handle.card4.y;
    }
    if (card_compo_id == ui_handle.card5.id)
    {
        ret = ui_handle.card5.y;
    }
    if (card_compo_id == ui_handle.card6.id)
    {
        ret = ui_handle.card6.y;
    }
    if (card_compo_id == ui_handle.card7.id)
    {
        ret = ui_handle.card7.y;
    }
    return ret;
}

//功能结构体
typedef struct f_card_t_
{
    s32 ofs_y;
    s32 ofs_y_drag;
    s16 focus_card;                  //靠近屏幕中央的卡片序号
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    s32 moveto_y;                   //设定自动移到的坐标
    s16 moveto_card;                //设定自动移到的卡片序号
    u32 tick;
    u32 activity_tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
//    co_timer_t music_refresh;
} f_card_t;

static void card1_updata_disp(void)
{
    f_card_t *f_activity = (f_card_t *)func_cb.f_cb;

//    if(tick_check_expire(f_activity->activity_tick, ))
//    {
//        f_activity->activity_tick = tick_get();
    char txt_buf[20];
    uint32_t totalStepCnt = 0;
    uint32_t Step_value = 0;
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
#endif
    Step_value = totalStepCnt;
#if UTE_MODULE_SPORT_SUPPORT
    if(Step_value>uteModuleSportGetStepsTargetCnt())
    {
        Step_value = uteModuleSportGetStepsTargetCnt();
    }
#endif
    compo_picturebox_t *arc_kcal = compo_getobj_byid(COMPO_ID_ARC_KCAL);
    compo_picturebox_t *arc_km   = compo_getobj_byid(COMPO_ID_ARC_KM);
    compo_picturebox_t *arc_step = compo_getobj_byid(COMPO_ID_ARC_STEP);
    compo_cardbox_t *cardbox = compo_getobj_byid(ui_handle.card1.id);

    compo_picturebox_set_visible(arc_kcal, true);
    compo_picturebox_set_visible(arc_km, true);
    compo_picturebox_set_visible(arc_step, true);


    if(f_activity->activity_state == 0)
    {
        f_activity->arc_step_value+=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value >=1800)
        {
            f_activity->activity_state =1;
        }
    }
    else if(f_activity->activity_state == 1)
    {

        f_activity->arc_step_value-=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );
#if UTE_MODULE_SPORT_SUPPORT
        if(f_activity->arc_step_value <= Step_value*1800 / uteModuleSportGetStepsTargetCnt())
        {
            f_activity->activity_state = 2;
        }
#endif

    }
    else
    {
#if UTE_MODULE_SPORT_SUPPORT
        f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
        f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
        f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());
#else
        f_activity->arc_kcal_value =0;
        f_activity->arc_km_value   =0;
        f_activity->arc_step_value =0;

#endif

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
        compo_cardbox_text_set(cardbox, 2, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
        compo_cardbox_text_set(cardbox, 1, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
        compo_cardbox_text_set(cardbox, 0, txt_buf);


        if(f_activity->arc_step_value)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);
        }
        else
        {
            compo_picturebox_set_visible(arc_kcal,false);
            compo_picturebox_set_visible(arc_km,false);
            compo_picturebox_set_visible(arc_step,false);
        }
    }

//    }
}
//根据序号获取最近任务序号（idx=0为最近，无任务返回0）(idx<=3)
static u8 card_get_latest_func(u8 idx)
{
    u8 i, j;
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};
    u8 latest_cnt = latest_task_count();
    if (latest_cnt > idx)
    {
        return latest_task_get(idx);
    }
    else
    {
        j = 0;
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)     //最近任务不足DEFAULT_LATEST_TASK_NUM个且包含默认值
        {
            if (latest_task_find(last_default[i]) == -1)
            {
                latest_default_tmp[j] = last_default[i];
                j++;
            }
        }
        return latest_default_tmp[idx - latest_cnt];
    }
}

//根据序号获取最近任务图标资源地址
static u32 card_get_latest_icon(u8 idx)
{
    return func_get_icon_addr(card_get_latest_func(idx));
}
#define CARDBOX_RADIUS 16
void set_cardbox_rect_location(compo_cardbox_t *cardbox,int card_index, int x, int y, int w, int h,u16 color)
{
    if (cardbox == NULL || card_index>=CARD_RECT_MAX)
    {
        TRACE("[%s]-- erro\n",__func__);
        return;
    }
    compo_cardbox_rect_set_location(cardbox, card_index, x, y, w, h, CARDBOX_RADIUS);
    compo_cardbox_rect_set_color(cardbox, card_index, color);
}

void set_cardbox_icon(compo_cardbox_t *cardbox,int card_index, void* icon, int x, int y, int w, int h,int card_w,int card_h)
{

    if (cardbox == NULL || icon == NULL||card_index>=CARD_ICON_MAX)
    {
        TRACE("[%s]-- erro\n",__func__);
        return;
    }
    compo_cardbox_icon_set(cardbox, card_index, icon);
    compo_cardbox_icon_set_location(cardbox, card_index, x+(w/2-card_w/2), y+(h/2-card_h/2), w, h);
}

void set_cardbox_text(compo_cardbox_t *cardbox,int card_index,widget_text_t *txt, const char* text,u16 color,u32 res, int x, int y, int w, int h,int card_w,int card_h,bool align_center)
{

    if (cardbox == NULL || txt == NULL || text == NULL||card_index>=CARD_TEXT_MAX)
    {
        TRACE("[%s]-- erro\n",__func__);
        return;
    }
    compo_cardbox_text_set_location(cardbox, card_index, x-(card_w/2), y-(card_h/2), w, h);
    compo_cardbox_text_set_font(cardbox, card_index, res);
    compo_cardbox_text_set_align_center(cardbox,card_index,align_center);
    widget_text_set_color(txt,color);
    compo_cardbox_text_set(cardbox, card_index, text);
}

void set_cardbox_imgbtn(compo_button_t *btn,u16 id, int x, int y, int w, int h,int card_w,int card_h)
{

    if (btn == NULL)
    {
        TRACE("[%s]-- erro\n",__func__);
        return;
    }
    compo_button_set_location(btn,x-card_w/2, y-card_h/2, w, h);
    compo_setid(btn,id);
}
#define CARD_LAST_APP_MAX  3
const uint16_t Last_Icon_ID[]=
{
    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
};
const uint16_t Week_ID[]=
{
    STR_SUNDAY, // 周日
    STR_MONDAY, // 周一
    STR_TUESDAY, // 周二
    STR_WEDNESDAY, // 周三
    STR_THURSDAY, // 周四
    STR_FRIDAY, // 周五
    STR_SATURDAY, // 周六
};

//创建组件
static void func_clock_sub_card_compo_create(compo_form_t *frm)
{
    char txt_buf[20];
    compo_button_t *btn=NULL;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);//获取系统时间

    ///卡片7 最近3个应用
    compo_cardbox_t* card7 = compo_cardbox_create(frm, 1, 4, 1, SUB_CARD_W, SUB_CARD_H);
    compo_setid(card7, COMPO_ID_CARD_7);

    //标题
    set_cardbox_rect_location(card7, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41));
    set_cardbox_text(card7,0,card7->text[0],i18n[STR_LATEST_APP],make_color(255,255,255),UI_BUF_0FONT_FONT_BIN,12,10,200,30,SUB_CARD_W,SUB_CARD_H,false);

    //图片按键
    for (u8 i=0; i<CARD_LAST_APP_MAX; i++)
    {
        btn = compo_button_create_page_by_image(frm,card7->page,card_get_latest_icon(i));
        set_cardbox_imgbtn(btn,Last_Icon_ID[i],37+(65*i),65,50,50,SUB_CARD_W,SUB_CARD_H);
    }

    //卡片6 日历

    compo_cardbox_t* card6 = compo_cardbox_create(frm, 1, 3, 3, SUB_CARD_W, SUB_CARD_H);
    compo_setid(card6, COMPO_ID_CARD_6);
    set_cardbox_rect_location(card6, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41));
    set_cardbox_icon(card6, 0,UI_BUF_I330001_FIRSTORDER_CALENDAR_BG_L_BIN,0,0,SUB_CARD_W, SUB_CARD_H,SUB_CARD_W,SUB_CARD_H);

    set_cardbox_text(card6,0,card6->text[0],i18n[Week_ID[time.week]],make_color(255,255,255),UI_BUF_0FONT_FONT_BIN,12,10,200,30,SUB_CARD_W,SUB_CARD_H,false);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%02d",time.month);///月份
    set_cardbox_text(card6,1,card6->text[1],txt_buf,make_color(255,255,255),UI_BUF_0FONT_FONT_NUM_38_BIN,7,50,200,46,SUB_CARD_W,SUB_CARD_H,false);

    set_cardbox_icon(card6, 1,UI_BUF_I330001_FIRSTORDER_CALENDAR_MOTH_BIN,55,55,26, 26,SUB_CARD_W,SUB_CARD_H);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%02d",time.day);///月份
    set_cardbox_text(card6,2,card6->text[1],txt_buf,make_color(255,255,255),UI_BUF_0FONT_FONT_NUM_38_BIN,80,50,200,46,SUB_CARD_W,SUB_CARD_H,false);

    set_cardbox_icon(card6, 2,UI_BUF_I330001_FIRSTORDER_CALENDAR_DATA_BIN,123,55,26, 26,SUB_CARD_W,SUB_CARD_H);
    //卡片5

    compo_cardbox_t* card5 = compo_cardbox_create(frm, 1, 0, 2, SUB_CARD_W, SUB_CARD_H);
    compo_setid(card5, COMPO_ID_CARD_5);
    set_cardbox_rect_location(card5, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41));
    //标题
    set_cardbox_text(card5,0,card5->text[0],i18n[STR_STOP_WATCH],make_color(248,132,10),UI_BUF_0FONT_FONT_BIN,12,10,200,30,SUB_CARD_W,SUB_CARD_H,false);
    //计时器数字
    set_cardbox_text(card5,1,card5->text[1],"00:00:00",make_color(255,255,255),UI_BUF_0FONT_FONT_NUM_38_BIN,12,37,200,46,SUB_CARD_W,SUB_CARD_H,false);


    //卡片4 倒计时
    compo_cardbox_t* card4 = compo_cardbox_create(frm, 1, 0, 2, SUB_CARD_W, SUB_CARD_H);
    compo_setid(card4, COMPO_ID_CARD_4);
    set_cardbox_rect_location(card4, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41));
    //标题
    set_cardbox_text(card4,0,card4->text[0],i18n[STR_TIMER],make_color(248,132,10),UI_BUF_0FONT_FONT_BIN,12,10,200,30,SUB_CARD_W,SUB_CARD_H,false);
    //倒计时数字文本
    set_cardbox_text(card4,1,card4->text[1],"00:00:00",make_color(255,255,255),UI_BUF_0FONT_FONT_NUM_38_BIN,12,37,200,46,SUB_CARD_W,SUB_CARD_H,false);



    // //卡片3 //闹钟
    // compo_cardbox_t* card3 = compo_cardbox_create(frm, 1, 1, 5, SUB_CARD_W,SUB_CARD_H);
    // compo_setid(card3, COMPO_ID_CARD_3);
    // set_cardbox_rect_location(card3, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41));
    // set_cardbox_icon(card3, 0,UI_BUF_I330001_FIRSTORDER_VAPE_RECORD_BG_L_BIN,0,0,SUB_CARD_W, SUB_CARD_H,SUB_CARD_W,SUB_CARD_H);
    // // set_cardbox_text(card3,0,card3->text[0],i18n[STR_ALARM_CLOCK],make_color(255,255,255),UI_BUF_0FONT_FONT_BIN,12,10,200,30,SUB_CARD_W,SUB_CARD_H,false);


    //卡片2 //音乐
    compo_cardbox_t* card2 = compo_cardbox_create(frm, 1, 4, 1, SUB_CARD_W, SUB_CARD_H);
    compo_setid(card2,COMPO_ID_CARD_2);
    set_cardbox_rect_location(card2, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41));
    set_cardbox_icon(card2, 0,UI_BUF_I330001_FIRSTORDER_E_MUSIC_BG_BIN,0,0,SUB_CARD_W, SUB_CARD_H,SUB_CARD_W,SUB_CARD_H);
    set_cardbox_text(card2,0,card2->text[0],i18n[STR_MUSIC],make_color(255,255,255),UI_BUF_0FONT_FONT_BIN,12,10,200, 30,SUB_CARD_W,SUB_CARD_H,false);

    //下一曲
    btn = compo_button_create_page_by_image(frm,card2->page,UI_BUF_I330001_FIRSTORDER_E_MUSIC_NEXT_SONG_BIN);
    set_cardbox_imgbtn(btn,COMPO_MUSIC_BTN_NEXT,177,55,30,30,SUB_CARD_W,SUB_CARD_H);

    //开始/暂停
    btn = compo_button_create_page_by_image(frm,card2->page, UI_BUF_I330001_FIRSTORDER_E_MUSIC_PAUSE_BIN);
    set_cardbox_imgbtn(btn,COMPO_MUSIC_BTN_PLAY,104,55,50,50,SUB_CARD_W,SUB_CARD_H);

    //上一曲
    btn = compo_button_create_page_by_image(frm,card2->page, UI_BUF_I330001_FIRSTORDER_E_MUSIC_PREVIOUS_SONG_BIN);
    set_cardbox_imgbtn(btn,COMPO_MUSIC_BTN_PREV,31,55,30,30,SUB_CARD_W,SUB_CARD_H);




    ///卡片1  ///计算器
    compo_cardbox_t* card1 = compo_cardbox_create(frm, 1, 3, 3, SUB_CARD_W, SUB_CARD_H);
    compo_setid(card1, COMPO_ID_CARD_1);

    set_cardbox_rect_location(card1, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41));
    set_cardbox_icon(card1, 0,UI_BUF_I330001_FIRSTORDER_CALCULATOR_L_BIN,0,0,SUB_CARD_W, SUB_CARD_H,SUB_CARD_W,SUB_CARD_H);


    ///时钟 日期
    compo_picturebox_t* clock_bg = compo_picturebox_create(frm, ui_handle.card_clock_day.clock.bg_res);
    compo_setid(clock_bg, ui_handle.card_clock_day.clock.id_bg);

    compo_datetime_t* clock_h = compo_datetime_create(frm, ui_handle.card_clock_day.clock.h_res);
    compo_setid(clock_h, ui_handle.card_clock_day.clock.id_h);
    compo_datetime_set_center(clock_h, 13, ui_handle.card_clock_day.clock.h_h / 2);
    compo_datetime_set_start_angle(clock_h, 900);
    compo_bonddata(clock_h, ui_handle.card_clock_day.clock.h_bonddata);

    compo_datetime_t* clock_m = compo_datetime_create(frm, ui_handle.card_clock_day.clock.m_res);
    compo_setid(clock_m, ui_handle.card_clock_day.clock.id_min);
    compo_datetime_set_center(clock_m, 13, ui_handle.card_clock_day.clock.m_h / 2);
    compo_datetime_set_start_angle(clock_m, 900);
    compo_bonddata(clock_m, ui_handle.card_clock_day.clock.m_bonddata);

    compo_textbox_t* date = compo_textbox_create(frm, 5);                                 //日期
    compo_setid(date, ui_handle.card_clock_day.date.id);
    compo_textbox_set_font(date, ui_handle.card_clock_day.date.res);
    compo_textbox_set_align_center(date, ui_handle.card_clock_day.date.center);
    widget_text_set_color(date->txt, make_color(ui_handle.card_clock_day.date.color.r, ui_handle.card_clock_day.date.color.g, ui_handle.card_clock_day.date.color.b));
    compo_bonddata(date, ui_handle.card_clock_day.date.bonddata);

    compo_textbox_t* week = compo_textbox_create(frm, 20);                                 //日期
    compo_setid(week, ui_handle.card_clock_day.week.id);
    compo_textbox_set_font(week, ui_handle.card_clock_day.week.res);
    compo_textbox_set_align_center(week, ui_handle.card_clock_day.week.center);
    widget_text_set_color(week->txt, make_color(ui_handle.card_clock_day.week.color.r, ui_handle.card_clock_day.week.color.g, ui_handle.card_clock_day.week.color.b));
    compo_bonddata(week, ui_handle.card_clock_day.week.bonddata);


}

//更新组件位置和大小
static void func_clock_sub_card_compo_update(s32 ofs_y, bool creating)
{
    s16 x, y, w, h, card_y, card_w;
    u8 card_id = 0;
    u16 dc;  //当前卡片中心距屏幕中心距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL)
    {
        //获取初始值
        switch (compo->id)
        {
            case COMPO_ID_CLOCK_BG:
                if (ui_handle.card_clock_day.clock.id_bg != COMPO_ID_CLOCK_BG)
                {
                    printf("COMPO_ID_CLOCK_BG ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.bg_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.bg_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.bg_w;
                h = ui_handle.card_clock_day.clock.bg_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_H:
                if (ui_handle.card_clock_day.clock.id_h != COMPO_ID_CLOCK_H)
                {
                    printf("COMPO_ID_CLOCK_H ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.h_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.h_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.h_w;
                h = ui_handle.card_clock_day.clock.h_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_M:
                if (ui_handle.card_clock_day.clock.id_min != COMPO_ID_CLOCK_M)
                {
                    printf("COMPO_ID_CLOCK_M ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.m_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.m_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.m_w;
                h = ui_handle.card_clock_day.clock.m_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_DATE:
                if (ui_handle.card_clock_day.date.id != COMPO_ID_DATE)
                {
                    printf("COMPO_ID_DATE ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.date.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.date.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.date.w;
                h = ui_handle.card_clock_day.date.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_WEEKDAY:
                if (ui_handle.card_clock_day.week.id != COMPO_ID_WEEKDAY)
                {
                    printf("COMPO_ID_WEEKDAY ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.week.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.week.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.week.w;
                h = ui_handle.card_clock_day.week.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CARD_1:
                if (ui_handle.card1.id != COMPO_ID_CARD_1)
                {
                    printf("COMPO_ID_CARD_1 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card1.x;
                y = card_y = ui_handle.card1.y;
                w = ui_handle.card1.w;
                h = ui_handle.card1.h;
                card_id = ui_handle.card1.id;
                break;

            case COMPO_ID_CARD_2:
                if (ui_handle.card2.id != COMPO_ID_CARD_2)
                {
                    printf("COMPO_ID_CARD_2 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card2.x;
                y = card_y = ui_handle.card2.y;
                w = ui_handle.card2.w;
                h = ui_handle.card2.h;
                card_id = ui_handle.card2.id;
                break;

            // case COMPO_ID_CARD_3:
            //     if (ui_handle.card3.id != COMPO_ID_CARD_3)
            //     {
            //         printf("COMPO_ID_CARD_3 ERR\n");
            //         halt(HALT_GUI);
            //     }
            //     x = ui_handle.card3.x;
            //     y = card_y = ui_handle.card3.y;
            //     w = ui_handle.card3.w;
            //     h = ui_handle.card3.h;
            //     card_id = ui_handle.card3.id;
            //     break;

            case COMPO_ID_CARD_4:
                if (ui_handle.card4.id != COMPO_ID_CARD_4)
                {
                    printf("COMPO_ID_CARD_4 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card4.x;
                y = card_y = ui_handle.card4.y;
                w = ui_handle.card4.w;
                h = ui_handle.card4.h;
                card_id = ui_handle.card4.id;
                break;

            case COMPO_ID_CARD_5:
                if (ui_handle.card5.id != COMPO_ID_CARD_5)
                {
                    printf("COMPO_ID_CARD_5 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card5.x;
                y = card_y = ui_handle.card5.y;
                w = ui_handle.card5.w;
                h = ui_handle.card5.h;
                card_id = ui_handle.card5.id;
                break;


            case COMPO_ID_CARD_6:
                if (ui_handle.card6.id != COMPO_ID_CARD_6)
                {
                    printf("COMPO_ID_CARD_6 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card6.x;
                y = card_y = ui_handle.card6.y;
                w = ui_handle.card6.w;
                h = ui_handle.card6.h;
                card_id = ui_handle.card6.id;
                break;


            case COMPO_ID_CARD_7:
                if (ui_handle.card7.id != COMPO_ID_CARD_7)
                {
                    printf("COMPO_ID_CARD_7 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card7.x;
                y = card_y = ui_handle.card7.y;
                w = ui_handle.card7.w;
                h = ui_handle.card7.h;
                card_id = ui_handle.card7.id;
                break;

            default:
                compo = compo_get_next(compo);
                continue;
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH_ORG;
        card_y += ofs_y;
        y += ofs_y;
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);
        if (creating == false && dc < dc_min)    //焦点，创建时不使用f_cb
        {
            f_card_t *f_card = (f_card_t *)func_cb.f_cb;
            f_card->focus_card = card_id;
            dc_min = dc;
//            TRACE("focus[%d]\n", card_id);
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM)
        {
            if (card_y > CARD_SCALE_STOP_Y)    //缩到最小后保持一段时间
            {
                y -= card_y - CARD_SCALE_STOP_Y;
                card_y = CARD_SCALE_STOP_Y;
            }
            if (card_w > CARD_WIDTH_MIN)
            {
                //缩放
                s16 card_w_org = card_w;
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                w = w * card_w / card_w_org;
                h = h * card_w / card_w_org;
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org;
                y += (card_y - y) * (card_w_org - card_w) / card_w_org;
                //偏移
                s16 card_y_org = card_y;
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                y += card_y - card_y_org;
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM;
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level));   //背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level));
            if (unvisible)
            {
                compo_cardbox_set_visible(cardbox, false);
            }
            else
            {
                compo_cardbox_set_visible(cardbox, true);
            }

//            if (card_id > COMPO_CARD_END-1)      //被覆盖的卡片不可见，降低element溢出概率
//            {
//                rect_t rect = compo_cardbox_get_location((compo_cardbox_t *)compo_getobj_byid(COMPO_ID_CARD_SPORT_COMPASS + (card_id - CARD_ID_SPORT_COMPASS) - 1));
//                if (y <= rect.y)
//                {
//                    compo_cardbox_set_visible(cardbox, false);
//                }
//                else
//                {
//                    compo_cardbox_set_visible(cardbox, true);
//                }
//            }

        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_set_pos(pic, x, y);
            compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_picturebox_set_visible(pic, false);
            }
            else
            {
                compo_picturebox_set_visible(pic, true);
            }
        }
        else if (compo->type == COMPO_TYPE_TEXTBOX)
        {
            compo_textbox_t *txt = (compo_textbox_t *)compo;
            compo_textbox_set_location(txt, x, y, w, h);
            if (unvisible)
            {
                compo_textbox_set_visible(txt, false);
            }
            else
            {
                compo_textbox_set_visible(txt, true);
            }
        }
        else if (compo->type == COMPO_TYPE_DATETIME)
        {
            compo_datetime_t *dt = (compo_datetime_t *)compo;
            compo_datetime_set_pos(dt, x, y);
        }

        compo = compo_get_next(compo);          //遍历组件
    }
}

//创建上拉卡片菜单（文件内调用）
static compo_form_t *func_clock_sub_card_form_create_by_ofs(s16 ofs_y)
{
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
//    compo_shape_set_alpha(masklayer, 160);
    //创建所有组件
    func_clock_sub_card_compo_create(frm);
    //更新组件位置和大小
    func_clock_sub_card_compo_update(ofs_y, true);

    return frm;
}

//创建上拉卡片菜单（供外部调用）
compo_form_t *func_clock_sub_card_form_create(void)
{
    return func_clock_sub_card_form_create_by_ofs(SPRING_Y_MAX);
}

//获取点击的卡片组件id
static u16 func_clock_sub_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_BTN_COUNT; i++)
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;
        id = COMPO_CARD_START + 1 + i;
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

//表盘上拉卡片菜单点击处理
static void func_clock_sub_card_click_handler(void)
{
    compo_cardbox_t *cardbox = NULL;
//    rect_t rect;
    u8 func_jump = FUNC_NULL;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_clock_sub_card_get_btn_id(pt);
    printf("click compo_id:%d\n", compo_id);
    int id = compo_get_button_id();

    switch(compo_id)
    {
        case COMPO_ID_CARD_1 :

#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            func_jump = FUNC_ACTIVITY;
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
            func_jump =FUNC_CALCULATOR;                 //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
            break;
        case COMPO_ID_CARD_2 :
            switch(id)
            {
                case COMPO_MUSIC_BTN_PREV:
                {
                    uteModuleMusicCtrl(false,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV);
                    compo_button_set_bgimg(btn,UI_BUF_I330001_FIRSTORDER_E_MUSIC_PREVIOUS_SONG_BIN);
                }
                break;
                case COMPO_MUSIC_BTN_PLAY:
                {
                    uteModuleMusicCtrlPaused(false);
                    if(ble_is_connect())
                    {
                        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
                    }
                    music_data_refresh();
                }
                break;
                case COMPO_MUSIC_BTN_NEXT:
                {
                    uteModuleMusicCtrl(true,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);
                    compo_button_set_bgimg(btn, UI_BUF_I330001_FIRSTORDER_E_MUSIC_NEXT_SONG_BIN);
                }
                break;
                default:
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    func_jump = FUNC_BT;
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    break;
            }

            break;
        // case COMPO_ID_CARD_3:
        //     func_jump = FUNC_ECIG_VPAE_SUB;
        //     break;
        case COMPO_ID_CARD_4 :
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
            func_jump = FUNC_TIMER;
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            break;
        case COMPO_ID_CARD_5 :
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            func_jump = FUNC_STOPWATCH;
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            break;
        case COMPO_ID_CARD_6 :
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
            func_jump =  FUNC_CALENDAER;                     //日历
#endif
            break;
        case COMPO_ID_CARD_7 :
            printf("id : %d\n", id);
            if (id >= COMPO_ID_APP1)
            {
                func_jump = card_get_latest_func(id-COMPO_ID_APP1);
            }
            break;
        default:
            break;
    }
    if(func_jump != FUNC_NULL)
    {
        printf("func_jump=%d\n", func_jump);
        func_cb.sta = func_jump;  //直接跳转
    }
}

//根据偏移位置把最近的卡片对齐到屏幕中央
static s16 func_clock_sub_card_align_y(s16 ofs_y)
{
    u8 i, nearest_id = 0;
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    u16 dc;
    if (ofs_y >= DRAG_Y_MAX)
    {
        return DRAG_Y_MAX;  //回弹效果
    }
    else if (ofs_y <= DRAG_Y_MIN)
    {
        return DRAG_Y_MIN;
    }
//    for (i=0; i<CARD_COUNT; i++)
    for (i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
//        dc = abs_s(card_y_info[i] + ofs_y - GUI_SCREEN_CENTER_Y);
        dc = abs_s(func_clock_sub_card_get_y_info(COMPO_CARD_START+1+i) + ofs_y - GUI_SCREEN_CENTER_Y);
        if (dc < dc_min)
        {
            nearest_id = i;
            dc_min = dc;
        }
    }
//    return (GUI_SCREEN_CENTER_Y - card_y_info[nearest_id]);
    return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+nearest_id));
}

//根据卡片序号对齐页面偏移位置到屏幕中央
static s16 func_clock_sub_card_align_by_idx(s8 card_id)
{
//    if (card_id <= CARD_ID_CLOCK)
    if (card_id <= COMPO_CARD_START)
    {
        return DRAG_Y_MAX;
    }
//    else if (card_id > (CARD_COUNT - 1))
    else if (card_id > (COMPO_CARD_END-COMPO_CARD_START-1 - 1))
    {
        return DRAG_Y_MIN;
    }
    else
    {
//        return (GUI_SCREEN_CENTER_Y - card_y_info[card_id]);
        return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+card_id));
    }
}

//限制滑动偏移位置
static s16 func_clock_sub_card_limit_y(s16 ofs_y)
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));
    return ofs_y;
}

//设置页面偏移位置
static void func_clock_sub_card_set_offs(s16 ofs_y)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    f_card->ofs_y = ofs_y;
    func_clock_sub_card_compo_update(f_card->ofs_y, false);
}

//时钟表盘上拉菜单数据更新----------------------------------------->todo
static void func_clock_sub_card_data_update(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    compo_cardbox_t *cardbox = NULL;
    char txt_buf[20];

    if(f_card->flag_drag == 0)
    {
        // card1_updata_disp();
    }

//    if(f_activity->activity_state != 2) return;

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",SEC_TO_HOUR(sys_cb.timer_left_sec),SEC_TO_MIN(sys_cb.timer_left_sec),SEC_TO_SEC(sys_cb.timer_left_sec));
    cardbox = compo_getobj_byid(ui_handle.card4.id);
    compo_cardbox_text_set(cardbox, ui_handle.card4.text_time.idx,txt_buf);

    u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    u16 msec = sys_cb.stopwatch_total_msec % 1000;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min  = 99;
        sec  = 59;
        msec = 999;
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld.%02ld",min,sec,msec/10);
    cardbox = compo_getobj_byid(ui_handle.card5.id);
    compo_cardbox_text_set(cardbox, ui_handle.card5.text_time.idx,txt_buf);
}

//下拉返回表盘
static void func_clock_sub_card_switch_to_clock(bool auto_switch)
{
    printf("%s\n", __func__);
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    u16 switch_mode = FUNC_SWITCH_MENU_PULLUP_DOWN | (auto_switch ? FUNC_SWITCH_AUTO : 0);
    compo_form_destroy(func_cb.frm_main);
    compo_form_t * frm_clock = func_create_form(FUNC_CLOCK);
    compo_form_t * frm = func_clock_sub_card_form_create_by_ofs(f_card->ofs_y);
    func_cb.frm_main = frm;
    if (func_switching(switch_mode, NULL))
    {
        func_cb.sta = FUNC_CLOCK;
    }
    compo_form_destroy(frm_clock);
}

void music_data_refresh(void)
{
    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }

    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY); //播放/暂停
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I330001_FIRSTORDER_MUSIC_PAUSED_BIN : UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY00_BIN);
    }
    else
    {
        compo_button_set_bgimg(btn,UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY01_BIN);
    }
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//时钟表盘上拉菜单主要事件流程处理
static void func_clock_sub_card_process(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    if (f_card->flag_drag)
    {
        s32 dx, dy;
        f_card->flag_drag = ctp_get_dxy(&dx, &dy);
        if (f_card->flag_drag)
        {
            //拖动页面
            f_card->ofs_y_drag = func_clock_sub_card_limit_y(f_card->ofs_y + dy);
            func_clock_sub_card_compo_update(f_card->ofs_y_drag, false);
        }
        else
        {
            //抬手
            f_card->ofs_y = f_card->ofs_y_drag;
            s16 last_dy = ctp_get_last_dxy().y;
            s16 to_y = f_card->ofs_y + (last_dy * DRAG_AUTO_SPEED);
            f_card->moveto_y = func_clock_sub_card_align_y(to_y);
            f_card->flag_move_auto = true;
            f_card->tick = tick_get();
        }
        for(u8 i=0; i<CARD_BTN_COUNT; i++)      //文本滚动
        {
            u16 id = COMPO_CARD_START + 1 + i;
            compo_cardbox_text_scroll_reset((compo_cardbox_t *)compo_getobj_byid(id));   //scroll test
            compo_cardbox_text_scroll_pause((compo_cardbox_t *)compo_getobj_byid(id), true);   //scroll test
        }

    }
    else if (f_card->flag_move_auto)
    {
        for(u8 i=0; i<CARD_BTN_COUNT; i++)      //文本滚动
        {
            u16 id = COMPO_CARD_START + 1 + i;
            compo_cardbox_text_scroll_pause((compo_cardbox_t *)compo_getobj_byid(id), false);   //scroll test
        }

        //自动移动
        if (f_card->ofs_y == f_card->moveto_y)
        {
            if (f_card->ofs_y < SPRING_Y_MIN)
            {
                f_card->moveto_y = SPRING_Y_MIN;
            }
            else if (f_card->ofs_y > SPRING_Y_MAX)
            {
                f_card->moveto_y = SPRING_Y_MAX;
            }
            else
            {
                f_card->flag_move_auto = false;            //移动完成
                f_card->moveto_card = f_card->focus_card;
            }
        }
        else if (tick_check_expire(f_card->tick, FOCUS_AUTO_TICK_EXPIRE))
        {
            TRACE("[%d]--move_to->[%d]\n", f_card->ofs_y, f_card->moveto_y);
            s16 dy;
            f_card->tick = tick_get();
            dy = f_card->moveto_y - f_card->ofs_y;
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
            }
            func_clock_sub_card_set_offs(f_card->ofs_y + dy);
        }
    }

    func_clock_sub_card_data_update();
    func_process();

    for(u8 i=0; i<CARD_BTN_COUNT; i++)      //文本滚动
    {
        u16 id = COMPO_CARD_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
}

//时钟表盘上拉菜单功能消息处理
static void func_clock_sub_card_message(size_msg_t msg)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    if (f_card->flag_drag)
    {
        evt_message(msg);  //拖动中，只处理部分消息
        return;
    }
    else if (f_card->flag_move_auto)
    {
        switch (msg)
        {
            case MSG_CTP_TOUCH:   //自动移动过程中，触屏停止
                f_card->flag_drag = true;
                f_card->flag_move_auto = false;
                break;

            case MSG_QDEC_FORWARD:  //向前滚动菜单
                if (++f_card->moveto_card > f_card->focus_card)
                {
                    f_card->moveto_y = func_clock_sub_card_align_by_idx(f_card->moveto_card);
                }
                break;

            case MSG_QDEC_BACKWARD:
                if (--f_card->moveto_card < f_card->focus_card)
                {
                    f_card->moveto_y = func_clock_sub_card_align_by_idx(f_card->moveto_card);
                }
                break;
                break;
            default:
                evt_message(msg);
                break;
        }
        return;
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_clock_sub_card_click_handler();
            break;
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
            if (msg == MSG_CTP_SHORT_DOWN && ctp_get_sxy().y < EXIT_Y_MAX && ctp_get_sxy().y > EXIT_Y_MIN)     //下滑返回到时钟主界面
            {
                func_clock_sub_card_switch_to_clock(false);
            }
            else
            {
                f_card->flag_drag = true;
                f_card->flag_move_auto = false;
            }
            break;

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_LONG:
            break;

        case MSG_QDEC_FORWARD:
            f_card->flag_move_auto = true;
            if (++f_card->moveto_card > f_card->focus_card)
            {
                f_card->moveto_y = func_clock_sub_card_align_by_idx(f_card->moveto_card);
            }
            TRACE("focus_card[%d] to_y[%d]\n", f_card->focus_card, f_card->moveto_y);
            break;

        case MSG_QDEC_BACKWARD:
            f_card->flag_move_auto = true;
            if (--f_card->moveto_card < f_card->focus_card)
            {
                f_card->moveto_y = func_clock_sub_card_align_by_idx(f_card->moveto_card);
            }
            TRACE("focus_card[%d] to_y[%d]\n", f_card->focus_card, f_card->moveto_y);
            break;

        case KU_BACK:
            func_clock_sub_card_switch_to_clock(true);  //单击BACK键返回到时钟主界面
            break;
        case MSG_SYS_500MS:
        {
// #if BT_ID3_TAG_EN
//             if (bt_is_connected())
//             {
//                 bt_music_paly_status_info();
//             }
// #endif
//             compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV);
//             if(uteModuleCallBtIsConnected() || ble_is_connect())
//             {
//                 compo_button_set_bgimg(btn, ui_handle.card3.pic_click_prev.res_click);
//             }
//             else
//             {
//                 compo_button_set_bgimg(btn, ui_handle.card3.pic_click_prev.res);
//             }

//             btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);
//             if(uteModuleCallBtIsConnected() || ble_is_connect())
//             {
//                 compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res_click);
//             }
//             else
//             {
//                 compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res);
//             }

            music_data_refresh();
        }
        default:
            func_message(msg);
            break;
    }
}



//时钟表盘上拉菜单进入处理
static void func_clock_sub_card_enter(void)
{
    tft_set_temode(3);
    func_cb.f_cb = func_zalloc(sizeof(f_card_t));
    func_cb.frm_main = func_clock_sub_card_form_create();

    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
//    co_timer_set(&f_card->music_refresh, 50, TIMER_REPEAT, LEVEL_LOW_PRI, music_data_refresh, NULL);

    func_clock_sub_card_set_offs(SPRING_Y_MAX);
}

//时钟表盘上拉菜单退出处理
static void func_clock_sub_card_exit(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
//    co_timer_del(&f_card->music_refresh);
    func_cb.last = FUNC_CARD;
    tft_set_temode(DEFAULT_TE_MODE);
}

//时钟表盘上拉菜单
void func_clock_sub_card(void)
{
    printf("%s\n", __func__);
    func_clock_sub_card_enter();
    while (func_cb.sta == FUNC_CARD)
    {
        func_clock_sub_card_process();
        func_clock_sub_card_message(msg_dequeue());
    }
    func_clock_sub_card_exit();
}
#endif // UTE_MODULE_SCREENS_UP_MENU_SUPPORT
