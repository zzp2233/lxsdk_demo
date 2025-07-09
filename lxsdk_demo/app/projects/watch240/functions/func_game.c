#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_GAME_SUPPORT

#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define GAME_NUM                    1

typedef struct f_game_t_
{
    compo_button_t* rect[GAME_NUM];
    compo_picturebox_t * pic[GAME_NUM];
    compo_textbox_t* text[GAME_NUM];

    bool drag_flag;
    s32 ofs_y;
    s32 focus_y;
    bool                flag_move_auto;
    s16                 focus_icon_idx;
    int                 moveto_idx;
    s32                 moveto_y;
    int                 line_height;
    uint32_t            tick;
    int                 line_center_y;
} f_game_t;

typedef struct
{
    char name[20];
    u32 res_addr;
    bool cutflag;   //图片资源是否需要裁剪
} Style;

enum
{
    GAME_ID_BTN_ICON_1 = 1,
    GAME_ID_BTN_ICON_2,
};



static Style game[GAME_NUM] =
{
    {"飞扬的小鸟", UI_BUF_I330001_GAME_BIRD2_BIN, 1},
    //{"俄罗斯方块", UI_BUF_TETRIS_16_1_BIN, 0},
};



//创建海拔窗体
compo_form_t *func_game_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_GAME]);
    for(int i=0; i<GAME_NUM; i++)
    {
        //矩形框
        compo_button_t * btn = compo_create(frm, COMPO_TYPE_BUTTON);
        widget_icon_t *img_btn = widget_icon_create(frm->page_body, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
        btn->widget = img_btn;
        compo_setid(btn, GAME_ID_BTN_ICON_1 + i);
        compo_button_set_location(btn, 120, 100 + i*72, 220, 60);

        //文本
        compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_FLY_BIRD]));
        compo_textbox_set_align_center(txt, false);
        compo_textbox_set_location(txt, 82, 100 + i*72-widget_text_get_height()/2, 140, widget_text_get_height()+8);
//        compo_textbox_set(txt, game[i].name);
        compo_textbox_set(txt, i18n[STR_FLY_BIRD]);
        compo_textbox_set_visible(txt, 1);

        //图标
        compo_picturebox_t * pic = compo_picturebox_create(frm, game[i].res_addr);
        if(game[i].cutflag)
            compo_picturebox_cut(pic, 0, 3);
        compo_picturebox_set_pos(pic, 50, 100 + i*72);
    }

    widget_page_set_client(frm->page_body, 0, -65);
    printf("%s\n", __func__);
    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define GAME_NUM                    1

typedef struct f_game_t_
{
    compo_button_t* rect[GAME_NUM];
    compo_picturebox_t * pic[GAME_NUM];
    compo_textbox_t* text[GAME_NUM];

    bool drag_flag;
    s32 ofs_y;
    s32 focus_y;
    bool                flag_move_auto;
    s16                 focus_icon_idx;
    int                 moveto_idx;
    s32                 moveto_y;
    int                 line_height;
    uint32_t            tick;
    int                 line_center_y;
} f_game_t;

typedef struct
{
    char name[20];
    u32 res_addr;
    bool cutflag;   //图片资源是否需要裁剪
} Style;

enum
{
    GAME_ID_BTN_ICON_1 = 1,
    GAME_ID_BTN_ICON_2,
};



static Style game[GAME_NUM] =
{
    {"飞扬的小鸟", UI_BUF_I332001_GAME_BIRD2_BIN, 1},
    //{"俄罗斯方块", UI_BUF_TETRIS_16_1_BIN, 0},
};
//创建海拔窗体
compo_form_t *func_game_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_GAME]);
    for(int i=0; i<GAME_NUM; i++)
    {
        //矩形框
        compo_button_t * btn = compo_create(frm, COMPO_TYPE_BUTTON);
        widget_icon_t *img_btn = widget_icon_create(frm->page_body, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
        btn->widget = img_btn;
        compo_setid(btn, GAME_ID_BTN_ICON_1 + i);
        compo_button_set_location(btn, 324/2+18, 112+80/2+ i*84, 324, 80);

        //文本
        compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_FLY_BIRD]));
        compo_textbox_set_align_center(txt, false);
        compo_textbox_set_location(txt, 88+20, 112+80/2+ i*84-10, 200, 34);
//        compo_textbox_set(txt, game[i].name);
        compo_textbox_set(txt, i18n[STR_FLY_BIRD]);
        compo_textbox_set_visible(txt, 1);

        //图标
        compo_picturebox_t * pic = compo_picturebox_create(frm, game[i].res_addr);
        if(game[i].cutflag)
            compo_picturebox_cut(pic, 0, 3);
        compo_picturebox_set_pos(pic, 56/2+40, 112+80/2+ i*84);
    }

    widget_page_set_client(frm->page_body, 0, -65);
    printf("%s\n", __func__);
    return frm;
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//事件处理
static void func_game_process(void)
{
    f_game_t *f_game = (f_game_t *)func_cb.f_cb;
    f_game->line_height = 40;
    f_game->line_center_y = f_game->line_height / 2;
    if (f_game->drag_flag)
    {
        s32 dx, dy;

        f_game->drag_flag = ctp_get_dxy(&dx, &dy);

        if (f_game->drag_flag)
        {
            f_game->ofs_y = f_game->focus_y - dy;
            int iy = -45 - f_game->ofs_y;
//            widget_page_set_client(func_cb.frm_main->page_body, 0, iy);

            int kidx;

            kidx = f_game->ofs_y / f_game->line_height;
            kidx -= f_game->ofs_y < 0 ? 1 : 0;
            f_game->focus_icon_idx = kidx;
        }
        else
        {
            s32 last_dy = ctp_get_last_dxy().y;
            f_game->flag_move_auto = true;
            f_game->focus_y = f_game->ofs_y;
            f_game->moveto_idx = f_game->focus_icon_idx - (last_dy * 80 / 466);

            if (f_game->moveto_idx > GAME_NUM - 4)
            {
                f_game->moveto_idx = GAME_NUM - 4;
            }
            if (f_game->moveto_idx < 0)
            {
                f_game->moveto_idx = 0;
            }

            f_game->moveto_y = f_game->line_center_y + f_game->line_height * f_game->moveto_idx;
            f_game->tick = tick_get();
        }
    }
    if (f_game->flag_move_auto)
    {
        int AUTO_STEP = 10;
        if (f_game->focus_y == f_game->moveto_y)
        {
            f_game->flag_move_auto = false;
            f_game->focus_icon_idx = f_game->ofs_y / f_game->line_height - (f_game->ofs_y < 0 ? 1 : 0);
            f_game->focus_y = f_game->ofs_y;
        }
        else if (tick_check_expire(f_game->tick, 10))
        {
            s32 dy;
            f_game->tick = tick_get();

            dy = f_game->moveto_y - f_game->focus_y;

            if (dy > 0)
            {
                if (dy > AUTO_STEP * 16)
                {
                    dy = dy / 16;
                }
                else if (dy > AUTO_STEP)
                {
                    dy = AUTO_STEP;
                }
                else
                {
                    dy = 1;
                }
            }
            else
            {
                if (dy < -AUTO_STEP * 16)
                {
                    dy = dy / 16;
                }
                else if (dy < -AUTO_STEP)
                {
                    dy = -AUTO_STEP;
                }
                else
                {
                    dy = -1;
                }
            }
            f_game->focus_y += dy;

            f_game->ofs_y = f_game->focus_y - dy;
            int iy = -45 - f_game->ofs_y;
//            widget_page_set_client(func_cb.frm_main->page_body, 0, iy);
        }
    }
    func_process();
}

static void func_game_click(void)
{
    int id = compo_get_button_id();

    if(id == GAME_ID_BTN_ICON_1)
    {
        func_switch_to(FUNC_BIRD, FUNC_SWITCH_DIRECT);
    }
    else if(id == GAME_ID_BTN_ICON_2)
    {
        func_switch_to(FUNC_TETRIS_START, FUNC_SWITCH_DIRECT);
    }

}

//海拔功能消息处理
static void func_game_message(size_msg_t msg)
{
    f_game_t *f_game = (f_game_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            f_game->drag_flag = true;
            f_game->flag_move_auto = false;
            f_game->focus_icon_idx = f_game->ofs_y / f_game->line_height - (f_game->ofs_y < 0 ? 1 : 0);
            f_game->focus_y = f_game->ofs_y;
            break;
        case MSG_CTP_CLICK:
            func_game_click();
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入海拔功能
static void func_game_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_game_t));
    func_cb.frm_main = func_game_form_create();
}

//退出海拔功能
static void func_game_exit(void)
{
    func_cb.last = FUNC_GAME;
}

//海拔功能
void func_game(void)
{
    printf("%s\n", __func__);
    func_game_enter();
    while (func_cb.sta == FUNC_GAME)
    {
        func_game_process();
        func_game_message(msg_dequeue());
    }
    func_game_exit();
}

#endif
