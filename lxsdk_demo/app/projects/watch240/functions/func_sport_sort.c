#include "include.h"
#include "func.h"

#include "ute_all_sports_int_algorithms.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

///户外跑步卡片
#define CARD1_RECT_CNT      1
#define CARD1_ICON_CNT      1
#define CARD1_TEXT_CNT      1
#define CARD1_X             8+109/2
#define CARD1_Y             58+102/2
#define CARD1_WIDTH         109
#define CARD1_HEIGHT        102

//矩形                      id       x       y       w                       h                       r
#define CARD1_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           20

//图标                      id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD1_ICON0_DATA          0,       8+44/2-109/2, 8+44/2-102/2,      44,     44,     0,         UTE_SPORT_IDX(SPORT_TYPE_RUNNING)

//文本                      id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD1_TEXT0_DATA          0,      8-109/2,    63-102/2,     86,     30,     NULL,   UI_BUF_0FONT_FONT_BIN,    false,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_RUNNING)

///户外骑行卡片
#define CARD2_RECT_CNT      1
#define CARD2_ICON_CNT      1
#define CARD2_TEXT_CNT      1
#define CARD2_X             123+109/2
#define CARD2_Y             58+102/2
#define CARD2_WIDTH         109
#define CARD2_HEIGHT        102
//矩形                            id       x       y       w                       h                       r
#define CARD2_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           20

//图标                            id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD2_ICON0_DATA          0,       8+44/2-109/2, 8+44/2-102/2,      44,     44,     0,         UTE_SPORT_IDX(SPORT_TYPE_RIDE_BIKE)

//文本                            id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD2_TEXT0_DATA          0,      8-109/2,    63-102/2,     86,     30,     NULL,   UI_BUF_0FONT_FONT_BIN,    false,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_RIDE_BIKE)

///跑步机卡片
#define CARD3_RECT_CNT      1
#define CARD3_ICON_CNT      1
#define CARD3_TEXT_CNT      1
#define CARD3_X             8+109/2
#define CARD3_Y             166+102/2
#define CARD3_WIDTH         109
#define CARD3_HEIGHT        102

//矩形                            id       x       y       w                       h                       r
#define CARD3_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           20

//图标                            id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD3_ICON0_DATA          0,       8+44/2-109/2, 8+44/2-102/2,      44,     44,     0,         UTE_SPORT_IDX(SPORT_TYPE_TREADMILL)

//文本                            id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD3_TEXT0_DATA          0,      8-109/2,    63-102/2,     86,     30,     NULL,   UI_BUF_0FONT_FONT_BIN,    false,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_TREADMILL)

///更多运动卡片
#define CARD4_RECT_CNT      1
#define CARD4_ICON_CNT      1
#define CARD4_TEXT_CNT      1
#define CARD4_X             123+109/2
#define CARD4_Y             166+102/2
#define CARD4_WIDTH         109
#define CARD4_HEIGHT        102

//矩形                            id       x       y       w                       h                       r
#define CARD4_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           20

//图标                            id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD4_ICON0_DATA          0,       8+44/2-109/2, 8+44/2-102/2,      44,     44,     0,         UTE_SPORT_IDX(SPORT_TYPE_MAX)

//文本                            id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD4_TEXT0_DATA          0,      8-109/2,    63-102/2,     86,     30,     NULL,   UI_BUF_0FONT_FONT_BIN,    false,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_MAX)

typedef struct f_sport_sort_t_
{


} f_sport_sort_t;

enum
{
    COMPO_ID_CARD_1 = 1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    CARD_TOTAL_CNT,                         //卡片的全部数量
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
    u8 rev;             //保留位
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
    u8 rev;
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

// ///创建卡片
// #define CREAT_CARD(compo_id, rect_cnt, icon_cnt, text_cnt, card0, ORG_W, ORG_H) \
// {\
//     compo_cardbox_t * cardbox = compo_cardbox_create(frm, rect_cnt, icon_cnt, text_cnt, ORG_W, ORG_H); \
//     compo_cardbox_set_visible(cardbox, true); \
//     compo_cardbox_set_location(cardbox, card0.x, card0.y, card0.w, card0.h);\
//     compo_setid(cardbox, compo_id); \
//     for (u8 i=0; i<rect_cnt; i++) { \
//         compo_cardbox_rect_set_location(cardbox, card0.rect[i].id, card0.rect[i].x, card0.rect[i].y, card0.rect[i].w, card0.rect[i].h, card0.rect[i].r); \
//         compo_cardbox_rect_set_color(cardbox, card0.rect[i].id, make_color(41,41,41)); \
//     } \
//     for (u8 i=0; i<icon_cnt; i++) { \
//         compo_cardbox_icon_set(cardbox, card0.icon[i].id, card0.icon[i].res); \
//         extern u32 func_sport_get_ui(u8 sport_idx); \
//         compo_cardbox_icon_set(cardbox, card0.icon[i].id, func_sport_get_ui(card0.icon[i].rev)); \
//         compo_cardbox_icon_set_location(cardbox, card0.icon[i].id, card0.icon[i].x, card0.icon[i].y, card0.icon[i].w, card0.icon[i].h); \
//     } \
//     for (u8 i=0; i<text_cnt; i++) { \
//         compo_cardbox_text_set_font(cardbox, card0.text[i].id, card0.text[i].res); \
//         widget_text_set_ellipsis(cardbox->text[card0.text[i].id], false);   \
//         if (card0.text[i].w == 0 || card0.text[i].h == 0) { \
//             compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, true); \
//         } else { \
//             compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, false); \
//         } \
//         widget_set_align_center(cardbox->text[card0.text[i].id], card0.text[i].center); \
//         widget_text_set_wordwrap(cardbox->text[card0.text[i].id], card0.text[i].wordwrap); \
//         widget_text_set_color(cardbox->text[card0.text[i].id], make_color(card0.text[i].r, card0.text[i].g, card0.text[i].b)); \
//         compo_cardbox_text_set_location(cardbox, card0.text[i].id, card0.text[i].x, card0.text[i].y, card0.text[i].w, card0.text[i].h); \
//         extern u32 func_sport_get_str(u8 sport_idx); \
//         if (card0.text[i].str != NULL) {    \
//             compo_cardbox_text_set(cardbox, card0.text[i].id, card0.text[i].str); \
//         } else if (func_sport_get_str(card0.text[i].rev) != STR_NULL) { \
//             compo_cardbox_text_set(cardbox, card0.text[i].id, i18n[func_sport_get_str(card0.text[i].rev)]); \
//         } \
//     }\
// }

#define UTE_SPORT_IDX(type)        type-1

///户外跑步卡片
CARD_TYPEDEF_T(card1, CARD1_RECT_CNT, CARD1_ICON_CNT, CARD1_TEXT_CNT);
static const CARD_T(card1) card1 =
{
    CARD1_X,
    CARD1_Y,
    CARD1_WIDTH,
    CARD1_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD1_RECT0_DATA},
    },

    {
        //图标
        //id      x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
        {CARD1_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD1_TEXT0_DATA},
    },
};

///户外骑行卡片
CARD_TYPEDEF_T(card2, CARD2_RECT_CNT, CARD2_ICON_CNT, CARD2_TEXT_CNT);
static const CARD_T(card2) card2 =
{
    CARD2_X,
    CARD2_Y,
    CARD2_WIDTH,
    CARD2_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD2_RECT0_DATA},
    },

    {
        //图标
        //id      x       y       w       h       res
        {CARD2_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD2_TEXT0_DATA},
    },
};

///跑步机卡片
CARD_TYPEDEF_T(card3, CARD3_RECT_CNT, CARD3_ICON_CNT, CARD3_TEXT_CNT);
static const CARD_T(card3) card3 =
{
    CARD3_X,
    CARD3_Y,
    CARD3_WIDTH,
    CARD3_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD3_RECT0_DATA},
    },

    {
        //图标
        //id      x       y       w       h       res
        {CARD3_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD3_TEXT0_DATA},
    },
};

///更多运动卡片
CARD_TYPEDEF_T(card4, CARD4_RECT_CNT, CARD4_ICON_CNT, CARD4_TEXT_CNT);
static const CARD_T(card4) card4 =
{
    CARD4_X,
    CARD4_Y,
    CARD4_WIDTH,
    CARD4_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD4_RECT0_DATA},
    },

    {
        //图标
        //id      x       y       w       h       res
        {CARD4_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD4_TEXT0_DATA},
    },
};

#define CARD_COMPO_ID(i)    COMPO_ID_CARD_##i
#define CARD_RECT_CNT(i)    CARD##i##_RECT_CNT
#define CARD_ICON_CNT(i)    CARD##i##_ICON_CNT
#define CARD_TEXT_CNT(i)    CARD##i##_TEXT_CNT
#define CARD_HANDLE(i)      card##i

//创建窗体
compo_form_t *func_sport_sort_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);

//     //户外跑步
//     CREAT_CARD(CARD_COMPO_ID(1), CARD_RECT_CNT(1), CARD_ICON_CNT(1), CARD_TEXT_CNT(1), CARD_HANDLE(1), CARD_HANDLE(1).w, CARD_HANDLE(1).h);
// //    //户外骑行
//     CREAT_CARD(CARD_COMPO_ID(2), CARD_RECT_CNT(2), CARD_ICON_CNT(2), CARD_TEXT_CNT(2), CARD_HANDLE(2), CARD_HANDLE(2).w, CARD_HANDLE(2).h);
// //    //跑步机
//     CREAT_CARD(CARD_COMPO_ID(3), CARD_RECT_CNT(3), CARD_ICON_CNT(3), CARD_TEXT_CNT(3), CARD_HANDLE(3), CARD_HANDLE(3).w, CARD_HANDLE(3).h);
// //    //更多运动
//     CREAT_CARD(CARD_COMPO_ID(4), CARD_RECT_CNT(4), CARD_ICON_CNT(4), CARD_TEXT_CNT(4), CARD_HANDLE(4), CARD_HANDLE(4).w, CARD_HANDLE(4).h);

    return frm;
}

//获取点击卡片组件的id
static u16 func_sport_sort_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_TOTAL_CNT-1; i++)
    {
        id = COMPO_ID_CARD_1 + i;
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

//卡片点击处理
static void func_sport_sort_card_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_sport_sort_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > CARD_TOTAL_CNT-1)
    {
        return;
    }
    printf("click compo_id:%d\n", compo_id);
    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == COMPO_ID_CARD_1)
        {
            sys_cb.sport_idx = CARD_HANDLE(1).icon[0].rev;
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else if (compo_id == COMPO_ID_CARD_2)
        {
            sys_cb.sport_idx = CARD_HANDLE(2).icon[0].rev;
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else if (compo_id == COMPO_ID_CARD_3)
        {
            sys_cb.sport_idx = CARD_HANDLE(3).icon[0].rev;
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else if (compo_id == COMPO_ID_CARD_4)           //更多运动
        {
            sys_cb.sport_idx = 0;
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
            func_cb.sta = FUNC_SPORT;
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
        }
    }
}

//更新卡片内容状态
static void func_sport_sort_card_update(void)
{
    for(u8 i=0; i<CARD_TOTAL_CNT; i++)      //文本滚动
    {
        u16 id = COMPO_ID_CARD_1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
///户外跑步卡片
#define CARD1_RECT_CNT      1
#define CARD1_ICON_CNT      1
#define CARD1_TEXT_CNT      1
#define CARD1_X             102
#define CARD1_Y             127
#define CARD1_WIDTH         136
#define CARD1_HEIGHT        136

//矩形                      id       x       y       w                       h                       r
#define CARD1_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           86

//图标                      id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD1_ICON0_DATA          0,       0, -25,      50,     50,     0,         UTE_SPORT_IDX(SPORT_TYPE_RUNNING)

//文本                      id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD1_TEXT0_DATA          0,      0,    25,     100,     22,     NULL,   UI_BUF_0FONT_FONT_BIN,    true,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_RUNNING)

///户外骑行卡片
#define CARD2_RECT_CNT      1
#define CARD2_ICON_CNT      1
#define CARD2_TEXT_CNT      1
#define CARD2_X             258
#define CARD2_Y             127
#define CARD2_WIDTH         136
#define CARD2_HEIGHT        136
//矩形                            id       x       y       w                       h                       r
#define CARD2_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           86

//图标                            id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD2_ICON0_DATA          0,       0, -25,      50,     50,     0,         UTE_SPORT_IDX(SPORT_TYPE_RIDE_BIKE)

//文本                            id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD2_TEXT0_DATA          0,      0,    25,     100,     22,    NULL,   UI_BUF_0FONT_FONT_BIN,    true,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_RIDE_BIKE)

///跑步机卡片
#define CARD3_RECT_CNT      1
#define CARD3_ICON_CNT      1
#define CARD3_TEXT_CNT      1
#define CARD3_X             102
#define CARD3_Y             270
#define CARD3_WIDTH         136
#define CARD3_HEIGHT        136

//矩形                            id       x       y       w                       h                       r
#define CARD3_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           86

//图标                            id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD3_ICON0_DATA          0,       0, -25,      50,     50,     0,         UTE_SPORT_IDX(SPORT_TYPE_TREADMILL)

//文本                            id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD3_TEXT0_DATA          0,      0,    25,     100,     22,    NULL,   UI_BUF_0FONT_FONT_BIN,    true,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_TREADMILL)

///更多运动卡片
#define CARD4_RECT_CNT      1
#define CARD4_ICON_CNT      1
#define CARD4_TEXT_CNT      1
#define CARD4_X             258
#define CARD4_Y             270
#define CARD4_WIDTH         136
#define CARD4_HEIGHT        136

//矩形                            id       x       y       w                       h                       r
#define CARD4_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           86

//图标                            id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD4_ICON0_DATA          0,       0, -25,      50,     50,     0,         UTE_SPORT_IDX(SPORT_TYPE_MAX)

//文本                            id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD4_TEXT0_DATA          0,      0,    25,     100,     22,     NULL,   UI_BUF_0FONT_FONT_BIN,    true,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_MAX)

typedef struct f_sport_sort_t_
{


} f_sport_sort_t;

enum
{
    COMPO_ID_CARD_1 = 1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    CARD_TOTAL_CNT,                         //卡片的全部数量
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
    u8 rev;             //保留位
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
    u8 rev;
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
{\
    compo_cardbox_t * cardbox = compo_cardbox_create(frm, rect_cnt, icon_cnt, text_cnt, ORG_W, ORG_H); \
    compo_cardbox_set_visible(cardbox, true); \
    compo_cardbox_set_location(cardbox, card0.x, card0.y, card0.w, card0.h);\
    compo_setid(cardbox, compo_id); \
    for (u8 i=0; i<rect_cnt; i++) { \
        compo_cardbox_rect_set_location(cardbox, card0.rect[i].id, card0.rect[i].x, card0.rect[i].y, card0.rect[i].w, card0.rect[i].h, card0.rect[i].r); \
        compo_cardbox_rect_set_color(cardbox, card0.rect[i].id, make_color(41,41,41)); \
    } \
    for (u8 i=0; i<icon_cnt; i++) { \
        compo_cardbox_icon_set(cardbox, card0.icon[i].id, card0.icon[i].res); \
        extern u32 func_sport_get_ui(u8 sport_idx); \
        compo_cardbox_icon_set(cardbox, card0.icon[i].id, func_sport_get_ui(card0.icon[i].rev)); \
        compo_cardbox_icon_set_location(cardbox, card0.icon[i].id, card0.icon[i].x, card0.icon[i].y, card0.icon[i].w, card0.icon[i].h); \
    } \
    for (u8 i=0; i<text_cnt; i++) { \
        compo_cardbox_text_set_font(cardbox, card0.text[i].id, card0.text[i].res); \
        widget_text_set_ellipsis(cardbox->text[card0.text[i].id], false);   \
        if (card0.text[i].w == 0 || card0.text[i].h == 0) { \
            compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, true); \
        } else { \
            compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, false); \
        } \
        widget_set_align_center(cardbox->text[card0.text[i].id], card0.text[i].center); \
        widget_text_set_wordwrap(cardbox->text[card0.text[i].id], card0.text[i].wordwrap); \
        widget_text_set_color(cardbox->text[card0.text[i].id], make_color(card0.text[i].r, card0.text[i].g, card0.text[i].b)); \
        compo_cardbox_text_set_location(cardbox, card0.text[i].id, card0.text[i].x, card0.text[i].y, card0.text[i].w, card0.text[i].h); \
        extern u32 func_sport_get_str(u8 sport_idx); \
        if (card0.text[i].str != NULL) {    \
            compo_cardbox_text_set(cardbox, card0.text[i].id, card0.text[i].str); \
        } else if (func_sport_get_str(card0.text[i].rev) != STR_NULL) { \
            compo_cardbox_text_set(cardbox, card0.text[i].id, i18n[func_sport_get_str(card0.text[i].rev)]); \
        } \
    }\
}

#define UTE_SPORT_IDX(type)        type-1

///户外跑步卡片
CARD_TYPEDEF_T(card1, CARD1_RECT_CNT, CARD1_ICON_CNT, CARD1_TEXT_CNT);
static const CARD_T(card1) card1 =
{
    CARD1_X,
    CARD1_Y,
    CARD1_WIDTH,
    CARD1_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD1_RECT0_DATA},
    },

    {
        //图标
        //id      x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
        {CARD1_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD1_TEXT0_DATA},
    },
};

///户外骑行卡片
CARD_TYPEDEF_T(card2, CARD2_RECT_CNT, CARD2_ICON_CNT, CARD2_TEXT_CNT);
static const CARD_T(card2) card2 =
{
    CARD2_X,
    CARD2_Y,
    CARD2_WIDTH,
    CARD2_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD2_RECT0_DATA},
    },

    {
        //图标
        //id      x       y       w       h       res
        {CARD2_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD2_TEXT0_DATA},
    },
};

///跑步机卡片
CARD_TYPEDEF_T(card3, CARD3_RECT_CNT, CARD3_ICON_CNT, CARD3_TEXT_CNT);
static const CARD_T(card3) card3 =
{
    CARD3_X,
    CARD3_Y,
    CARD3_WIDTH,
    CARD3_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD3_RECT0_DATA},
    },

    {
        //图标
        //id      x       y       w       h       res
        {CARD3_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD3_TEXT0_DATA},
    },
};

///更多运动卡片
CARD_TYPEDEF_T(card4, CARD4_RECT_CNT, CARD4_ICON_CNT, CARD4_TEXT_CNT);
static const CARD_T(card4) card4 =
{
    CARD4_X,
    CARD4_Y,
    CARD4_WIDTH,
    CARD4_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD4_RECT0_DATA},
    },

    {
        //图标
        //id      x       y       w       h       res
        {CARD4_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD4_TEXT0_DATA},
    },
};

#define CARD_COMPO_ID(i)    COMPO_ID_CARD_##i
#define CARD_RECT_CNT(i)    CARD##i##_RECT_CNT
#define CARD_ICON_CNT(i)    CARD##i##_ICON_CNT
#define CARD_TEXT_CNT(i)    CARD##i##_TEXT_CNT
#define CARD_HANDLE(i)      card##i

//创建窗体
compo_form_t *func_sport_sort_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    //户外跑步
    CREAT_CARD(CARD_COMPO_ID(1), CARD_RECT_CNT(1), CARD_ICON_CNT(1), CARD_TEXT_CNT(1), CARD_HANDLE(1), CARD_HANDLE(1).w, CARD_HANDLE(1).h);
//    //户外骑行
    CREAT_CARD(CARD_COMPO_ID(2), CARD_RECT_CNT(2), CARD_ICON_CNT(2), CARD_TEXT_CNT(2), CARD_HANDLE(2), CARD_HANDLE(2).w, CARD_HANDLE(2).h);
//    //跑步机
    CREAT_CARD(CARD_COMPO_ID(3), CARD_RECT_CNT(3), CARD_ICON_CNT(3), CARD_TEXT_CNT(3), CARD_HANDLE(3), CARD_HANDLE(3).w, CARD_HANDLE(3).h);
//    //更多运动
    CREAT_CARD(CARD_COMPO_ID(4), CARD_RECT_CNT(4), CARD_ICON_CNT(4), CARD_TEXT_CNT(4), CARD_HANDLE(4), CARD_HANDLE(4).w, CARD_HANDLE(4).h);

    return frm;
}

//获取点击卡片组件的id
static u16 func_sport_sort_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_TOTAL_CNT-1; i++)
    {
        id = COMPO_ID_CARD_1 + i;
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

//卡片点击处理
static void func_sport_sort_card_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_sport_sort_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > CARD_TOTAL_CNT-1)
    {
        return;
    }
    printf("click compo_id:%d\n", compo_id);
    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == COMPO_ID_CARD_1)
        {
            sys_cb.sport_idx = CARD_HANDLE(1).icon[0].rev;
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else if (compo_id == COMPO_ID_CARD_2)
        {
            sys_cb.sport_idx = CARD_HANDLE(2).icon[0].rev;
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else if (compo_id == COMPO_ID_CARD_3)
        {
            sys_cb.sport_idx = CARD_HANDLE(3).icon[0].rev;
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else if (compo_id == COMPO_ID_CARD_4)           //更多运动
        {
            sys_cb.sport_idx = 0;
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
            func_cb.sta = FUNC_SPORT;
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
        }
    }
}
//更新卡片内容状态
static void func_sport_sort_card_update(void)
{
    for(u8 i=0; i<CARD_TOTAL_CNT; i++)      //文本滚动
    {
        u16 id = COMPO_ID_CARD_1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
///户外跑步卡片
#define CARD1_RECT_CNT      1
#define CARD1_ICON_CNT      1
#define CARD1_TEXT_CNT      1
#define CARD1_X             40+136/2
#define CARD1_Y             40+136/2
#define CARD1_WIDTH         136
#define CARD1_HEIGHT        136

//矩形                      id       x       y       w                       h                       r
#define CARD1_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           86

//图标                      id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD1_ICON0_DATA          0,       0, -25,      50,     50,     0,         UTE_SPORT_IDX(SPORT_TYPE_RUNNING)

//文本                      id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD1_TEXT0_DATA          0,      0,    25,     100,     22,     NULL,   UI_BUF_0FONT_FONT_BIN,    true,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_RUNNING)

///户外骑行卡片
#define CARD2_RECT_CNT      1
#define CARD2_ICON_CNT      1
#define CARD2_TEXT_CNT      1
#define CARD2_X             184+136/2
#define CARD2_Y             40+136/2
#define CARD2_WIDTH         136
#define CARD2_HEIGHT        136
//矩形                            id       x       y       w                       h                       r
#define CARD2_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           86

//图标                            id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD2_ICON0_DATA          0,       0, -25,      50,     50,     0,         UTE_SPORT_IDX(SPORT_TYPE_RIDE_BIKE)

//文本                            id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD2_TEXT0_DATA          0,      0,    25,     100,     22,    NULL,   UI_BUF_0FONT_FONT_BIN,    true,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_RIDE_BIKE)

///跑步机卡片
#define CARD3_RECT_CNT      1
#define CARD3_ICON_CNT      1
#define CARD3_TEXT_CNT      1
#define CARD3_X             40+136/2
#define CARD3_Y             184+136/2
#define CARD3_WIDTH         136
#define CARD3_HEIGHT        136

//矩形                            id       x       y       w                       h                       r
#define CARD3_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           86

//图标                            id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD3_ICON0_DATA          0,       0, -25,      50,     50,     0,         UTE_SPORT_IDX(SPORT_TYPE_TREADMILL)

//文本                            id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD3_TEXT0_DATA          0,      0,    25,     100,     22,    NULL,   UI_BUF_0FONT_FONT_BIN,    true,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_TREADMILL)

///更多运动卡片
#define CARD4_RECT_CNT      1
#define CARD4_ICON_CNT      1
#define CARD4_TEXT_CNT      1
#define CARD4_X             184+136/2
#define CARD4_Y             184+136/2
#define CARD4_WIDTH         136
#define CARD4_HEIGHT        136

//矩形                            id       x       y       w                       h                       r
#define CARD4_RECT0_DATA          0,       0,      0,      CARD1_WIDTH,            CARD1_HEIGHT,           86

//图标                            id       x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
#define CARD4_ICON0_DATA          0,       0, -25,      50,     50,     0,         UTE_SPORT_IDX(SPORT_TYPE_MAX)

//文本                            id      x           y             w       h       str     res                       center  wordwrap   r    g    b    对应func_sport中的tbl_sport_list 的 idx
#define CARD4_TEXT0_DATA          0,      0,    25,     100,     22,     NULL,   UI_BUF_0FONT_FONT_BIN,    true,  false,     255,   255,   255, UTE_SPORT_IDX(SPORT_TYPE_MAX)

typedef struct f_sport_sort_t_
{


} f_sport_sort_t;

enum
{
    COMPO_ID_CARD_1 = 1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    CARD_TOTAL_CNT,                         //卡片的全部数量
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
    u8 rev;             //保留位
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
    u8 rev;
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
{\
    compo_cardbox_t * cardbox = compo_cardbox_create(frm, rect_cnt, icon_cnt, text_cnt, ORG_W, ORG_H); \
    compo_cardbox_set_visible(cardbox, true); \
    compo_cardbox_set_location(cardbox, card0.x, card0.y, card0.w, card0.h);\
    compo_setid(cardbox, compo_id); \
    for (u8 i=0; i<rect_cnt; i++) { \
        compo_cardbox_rect_set_location(cardbox, card0.rect[i].id, card0.rect[i].x, card0.rect[i].y, card0.rect[i].w, card0.rect[i].h, card0.rect[i].r); \
        compo_cardbox_rect_set_color(cardbox, card0.rect[i].id, make_color(41,41,41)); \
    } \
    for (u8 i=0; i<icon_cnt; i++) { \
        compo_cardbox_icon_set(cardbox, card0.icon[i].id, card0.icon[i].res); \
        extern u32 func_sport_get_ui(u8 sport_idx); \
        compo_cardbox_icon_set(cardbox, card0.icon[i].id, func_sport_get_ui(card0.icon[i].rev)); \
        compo_cardbox_icon_set_location(cardbox, card0.icon[i].id, card0.icon[i].x, card0.icon[i].y, card0.icon[i].w, card0.icon[i].h); \
    } \
    for (u8 i=0; i<text_cnt; i++) { \
        compo_cardbox_text_set_font(cardbox, card0.text[i].id, card0.text[i].res); \
        widget_text_set_ellipsis(cardbox->text[card0.text[i].id], false);   \
        if (card0.text[i].w == 0 || card0.text[i].h == 0) { \
            compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, true); \
        } else { \
            compo_cardbox_text_set_autosize(cardbox, card0.text[i].id, false); \
        } \
        widget_set_align_center(cardbox->text[card0.text[i].id], card0.text[i].center); \
        widget_text_set_wordwrap(cardbox->text[card0.text[i].id], card0.text[i].wordwrap); \
        widget_text_set_color(cardbox->text[card0.text[i].id], make_color(card0.text[i].r, card0.text[i].g, card0.text[i].b)); \
        compo_cardbox_text_set_location(cardbox, card0.text[i].id, card0.text[i].x, card0.text[i].y, card0.text[i].w, card0.text[i].h); \
        extern u32 func_sport_get_str(u8 sport_idx); \
        if (card0.text[i].str != NULL) {    \
            compo_cardbox_text_set(cardbox, card0.text[i].id, card0.text[i].str); \
        } else if (func_sport_get_str(card0.text[i].rev) != STR_NULL) { \
            compo_cardbox_text_set(cardbox, card0.text[i].id, i18n[func_sport_get_str(card0.text[i].rev)]); \
        } \
    }\
}

#define UTE_SPORT_IDX(type)        type-1

///户外跑步卡片
CARD_TYPEDEF_T(card1, CARD1_RECT_CNT, CARD1_ICON_CNT, CARD1_TEXT_CNT);
static const CARD_T(card1) card1 =
{
    CARD1_X,
    CARD1_Y,
    CARD1_WIDTH,
    CARD1_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD1_RECT0_DATA},
    },

    {
        //图标
        //id      x             y                  w       h       res       对应func_sport中的tbl_sport_list 的 idx
        {CARD1_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD1_TEXT0_DATA},
    },
};

///户外骑行卡片
CARD_TYPEDEF_T(card2, CARD2_RECT_CNT, CARD2_ICON_CNT, CARD2_TEXT_CNT);
static const CARD_T(card2) card2 =
{
    CARD2_X,
    CARD2_Y,
    CARD2_WIDTH,
    CARD2_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD2_RECT0_DATA},
    },

    {
        //图标
        //id      x       y       w       h       res
        {CARD2_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD2_TEXT0_DATA},
    },
};

///跑步机卡片
CARD_TYPEDEF_T(card3, CARD3_RECT_CNT, CARD3_ICON_CNT, CARD3_TEXT_CNT);
static const CARD_T(card3) card3 =
{
    CARD3_X,
    CARD3_Y,
    CARD3_WIDTH,
    CARD3_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD3_RECT0_DATA},
    },

    {
        //图标
        //id      x       y       w       h       res
        {CARD3_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD3_TEXT0_DATA},
    },
};

///更多运动卡片
CARD_TYPEDEF_T(card4, CARD4_RECT_CNT, CARD4_ICON_CNT, CARD4_TEXT_CNT);
static const CARD_T(card4) card4 =
{
    CARD4_X,
    CARD4_Y,
    CARD4_WIDTH,
    CARD4_HEIGHT,
    {
        //矩形背景
        //id      x       y       w                       h                       r
        {CARD4_RECT0_DATA},
    },

    {
        //图标
        //id      x       y       w       h       res
        {CARD4_ICON0_DATA},
    },

    {
        ///文本
        //id      x       y       w       h       str                                res                         center  wordwrap   r  g  b
        {CARD4_TEXT0_DATA},
    },
};

#define CARD_COMPO_ID(i)    COMPO_ID_CARD_##i
#define CARD_RECT_CNT(i)    CARD##i##_RECT_CNT
#define CARD_ICON_CNT(i)    CARD##i##_ICON_CNT
#define CARD_TEXT_CNT(i)    CARD##i##_TEXT_CNT
#define CARD_HANDLE(i)      card##i

//创建窗体
compo_form_t *func_sport_sort_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    //户外跑步
    CREAT_CARD(CARD_COMPO_ID(1), CARD_RECT_CNT(1), CARD_ICON_CNT(1), CARD_TEXT_CNT(1), CARD_HANDLE(1), CARD_HANDLE(1).w, CARD_HANDLE(1).h);
//    //户外骑行
    CREAT_CARD(CARD_COMPO_ID(2), CARD_RECT_CNT(2), CARD_ICON_CNT(2), CARD_TEXT_CNT(2), CARD_HANDLE(2), CARD_HANDLE(2).w, CARD_HANDLE(2).h);
//    //跑步机
    CREAT_CARD(CARD_COMPO_ID(3), CARD_RECT_CNT(3), CARD_ICON_CNT(3), CARD_TEXT_CNT(3), CARD_HANDLE(3), CARD_HANDLE(3).w, CARD_HANDLE(3).h);
//    //更多运动
    CREAT_CARD(CARD_COMPO_ID(4), CARD_RECT_CNT(4), CARD_ICON_CNT(4), CARD_TEXT_CNT(4), CARD_HANDLE(4), CARD_HANDLE(4).w, CARD_HANDLE(4).h);

    return frm;
}

//获取点击卡片组件的id
static u16 func_sport_sort_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_TOTAL_CNT-1; i++)
    {
        id = COMPO_ID_CARD_1 + i;
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

//卡片点击处理
static void func_sport_sort_card_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_sport_sort_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > CARD_TOTAL_CNT-1)
    {
        return;
    }
    printf("click compo_id:%d\n", compo_id);
    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == COMPO_ID_CARD_1)
        {
            sys_cb.sport_idx = CARD_HANDLE(1).icon[0].rev;
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else if (compo_id == COMPO_ID_CARD_2)
        {
            sys_cb.sport_idx = CARD_HANDLE(2).icon[0].rev;
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else if (compo_id == COMPO_ID_CARD_3)
        {
            sys_cb.sport_idx = CARD_HANDLE(3).icon[0].rev;
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else if (compo_id == COMPO_ID_CARD_4)           //更多运动
        {
            sys_cb.sport_idx = 0;
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
            func_cb.sta = FUNC_SPORT;
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
        }
    }
}
//更新卡片内容状态
static void func_sport_sort_card_update(void)
{
    for(u8 i=0; i<CARD_TOTAL_CNT; i++)      //文本滚动
    {
        u16 id = COMPO_ID_CARD_1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

static void func_sport_sort_process(void)
{
    func_sport_sort_card_update();
    func_process();
}

static void func_sport_sort_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sport_sort_card_click();
            break;
        case MSG_CTP_LONG:
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
            break;

        default:
            func_message(msg);
            break;
    }
}

static void func_sport_sort_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sport_sort_t));
//    f_sport_sort_t *f_sport_sort = (f_sport_sort_t*)func_cb.f_cb;
    func_cb.frm_main = func_sport_sort_form_create();
}

static void func_sport_sort_exit(void)
{
//    f_sport_sort_t *f_sport_sort = (f_sport_sort_t*)func_cb.f_cb;
    func_cb.last = FUNC_SPORT_SORT;
}

void func_sport_sort(void)
{
    printf("%s\n", __func__);
    func_sport_sort_enter();
    while (func_cb.sta == FUNC_SPORT_SORT)
    {
        func_sport_sort_message(msg_dequeue());
        func_sport_sort_process();
    }
    func_sport_sort_exit();
}


