#include "include.h"
#include "func.h"

#define TRACE_EN        1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_OK = 1,
    COMPO_ID_BTN_SLIDE,

    //数字
    COMPO_ID_NUM_UU,
    COMPO_ID_NUM_UP,
    COMPO_ID_NUM_CENTER,
    COMPO_ID_NUM_DOWN,
    COMPO_ID_NUM_DD,
};

typedef struct breathe_set_num_item_t_
{
    int num_cnt;
    u16 num_id;
    s16 load_id;
    int val;
    s16 x;
    s16 y;
    bool visible_en;
    u8 alpha;
} breathe_set_num_item_t;

typedef struct f_breathe_sub_time_t_
{
    breathe_set_num_item_t set_num_item[5];
    area_t size;
    u16 num_height;
    s16 hcnt;
    s16 mcnt;
} f_breathe_sub_time_t;

#define BREATHE_NUM_PIC_CENTER_X              GUI_SCREEN_CENTER_X           //时间设置子页面位置x
#define BREATHE_NUM_PIC_CENTER_Y              (GUI_SCREEN_HEIGHT/2-20)           //时间设置子页面位置y
#define BREATHE_SET_NUM_ITEM_CNT             ((int)(sizeof(tbl_breathe_set_num_item) / sizeof(tbl_breathe_set_num_item[0])))
#define BREATHE_TXT_ITEM_M_X                  BREATHE_NUM_PIC_CENTER_X
#define BREATHE_TXT_ITEM_Y                    -20
#define BREATHE_TXT_ITEM_Y_OFFSET             40

#define BREATHE_CLOCK_SET_NUM_POS_Y_START                 BREATHE_TXT_ITEM_Y
#define BREATHE_CLOCK_SET_NUM_POS_Y(y_start,space,cnt)    ((y_start)+(space)*(cnt))

#define BREATHE_SUB_TIME_MIN                  1
#define BREATHE_SUB_TIME_MAX                  5

//搞个数字item，创建时遍历一下
static const breathe_set_num_item_t tbl_breathe_set_num_item[] =
{
    /*   num_cnt,      num_id,             load_num_id              val,                 x,                    y,                                             visible_en      alpha*/
    {2,        COMPO_ID_NUM_UU,       COMPO_ID_NUM_UU,          58,         BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y,                                true,           100},
    {2,        COMPO_ID_NUM_UP,       COMPO_ID_NUM_UP,          59,         BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y+BREATHE_TXT_ITEM_Y_OFFSET,      true,           100},
    {2,        COMPO_ID_NUM_CENTER,   COMPO_ID_NUM_CENTER,      0,          BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y+BREATHE_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {2,        COMPO_ID_NUM_DOWN,     COMPO_ID_NUM_DOWN,        1,          BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y+BREATHE_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {2,        COMPO_ID_NUM_DD,       COMPO_ID_NUM_DD,          2,          BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y+BREATHE_TXT_ITEM_Y_OFFSET*4,    true,           100},

};

//获取分 上一个和下一个的数字
static u8 func_breathe_get_time_cal(s8 num, u8 mode)
{
//    if (num < 0) {
//        num += 60;
//    }
//
//    if (mode == 1) {
//        if (num == 0 || num > 59) {
//            return 59;
//        }
//        return num - 1;
//    } else if (mode == 2){
//        if (num >= 59) {
//            return 0;
//        }
//        return num + 1;
//    } else {
//        if (num > 59) {
//            return 59;
//        }
//        return num;
//    }
    if (num < BREATHE_SUB_TIME_MIN)
    {
        num += BREATHE_SUB_TIME_MAX;
    }

    if (mode == BREATHE_SUB_TIME_MIN)
    {
        if (num == BREATHE_SUB_TIME_MIN || num > BREATHE_SUB_TIME_MAX)
        {
            return BREATHE_SUB_TIME_MAX;
        }
        return num - 1;
    }
    else if (mode == 2)
    {
        if (num >= BREATHE_SUB_TIME_MAX)
        {
            return BREATHE_SUB_TIME_MIN;
        }
        return num + 1;
    }
    else
    {
        if (num > BREATHE_SUB_TIME_MAX)
        {
            return BREATHE_SUB_TIME_MAX;
        }
        return num;
    }
}

//创建设置窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_breathe_sub_time_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BEATHER_DURATION]);

    //创建子页
    widget_page_t* sub_page = widget_page_create(frm->page_body);
    widget_set_location(sub_page, BREATHE_NUM_PIC_CENTER_X, BREATHE_NUM_PIC_CENTER_Y, GUI_SCREEN_WIDTH, BREATHE_TXT_ITEM_Y_OFFSET*3);

    //创建数字
    compo_textbox_t *txt_num;
    u8 min = ((sys_cb.breathe_duration / 1000) % 3600) / 60;

    char buf[4];
    for (u8 idx = 0; idx < BREATHE_SET_NUM_ITEM_CNT; idx++)
    {
        txt_num = compo_textbox_create_for_page(frm, sub_page, tbl_breathe_set_num_item[idx].num_cnt);
        compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
        compo_setid(txt_num, tbl_breathe_set_num_item[idx].num_id);
        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_pos(txt_num, tbl_breathe_set_num_item[idx].x, tbl_breathe_set_num_item[idx].y);
        compo_textbox_set_visible(txt_num, tbl_breathe_set_num_item[idx].visible_en);
        compo_textbox_set_alpha(txt_num, tbl_breathe_set_num_item[idx].alpha);

        memset(buf, 0, sizeof(buf));
        if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_UU)
        {
            snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(func_breathe_get_time_cal(min, 1), 1));
        }
        else if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_UP)
        {
            snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min, 1));
        }
        else if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_CENTER)
        {
            compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_38_BIN);
            snprintf(buf, sizeof(buf), "%02d", min);
        }
        else if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_DOWN)
        {
            snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min, 2));
        }
        else if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_DD)
        {
            snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(func_breathe_get_time_cal(min, 2), 2));
        }
        compo_textbox_set(txt_num, buf);
    }

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
    compo_setid(btn, COMPO_ID_BTN_OK);
    compo_button_set_pos(btn,120, 247);

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_SLIDE);
    compo_button_set_location(btn, BREATHE_NUM_PIC_CENTER_X, BREATHE_NUM_PIC_CENTER_Y, GUI_SCREEN_WIDTH/3, BREATHE_TXT_ITEM_Y_OFFSET*3);

    //新建图像
    compo_picturebox_t *pic_bg = compo_picturebox_create(frm, UI_BUF_I330001_BREATHE_BOX_BIN);
    compo_picturebox_set_pos(pic_bg, GUI_SCREEN_CENTER_X, BREATHE_NUM_PIC_CENTER_Y);

    //创建TEXT
    compo_textbox_t *txt = compo_textbox_create(frm, 10);
    compo_textbox_set_align_center(txt, true);
    compo_textbox_set_location(txt,120, 247, GUI_SCREEN_WIDTH, widget_text_get_height());
    compo_textbox_set_visible(txt, true);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_OK]);

    return frm;
}

static void breathe_set_num_pos_cal(s32 dy, bool press)
{
    f_breathe_sub_time_t *f_set = (f_breathe_sub_time_t*)func_cb.f_cb;

    //area_t size = f_set->size;
    u16 size_hei = BREATHE_TXT_ITEM_Y_OFFSET*3;
    u16 num_height = f_set->num_height;

    static s32 last_dy = 0;
    s8 dir = 0;
    s32 distance = 0;

    breathe_set_num_item_t *time_item;

    time_item = &f_set->set_num_item[0];

    if (press)
    {
        distance = dy - last_dy;
    }
    else
    {
        dy = 0;
        last_dy = 0;
        distance = 0;
    }

    time_item[0].y += distance;
    time_item[1].y += distance;
    time_item[2].y += distance;
    time_item[3].y += distance;
    time_item[4].y += distance;

    for (int i=0; i<5; i++)
    {
        if (time_item[i].load_id == COMPO_ID_NUM_UP)
        {
            if ((time_item[i].y /*- (BREATHE_NUM_PIC_CENTER_Y - size.hei/2)*/) <= -num_height)
            {
                dir = -1;
                break;
            }
            else if (press == false && ((time_item[i].y /*- (BREATHE_NUM_PIC_CENTER_Y - size.hei/2)*/) <= -num_height/2))
            {
                dir = -1;
                break;
            }
        }
        else if (time_item[i].load_id == COMPO_ID_NUM_DOWN)
        {
            if (time_item[i].y >= (/*BREATHE_NUM_PIC_CENTER_Y + size.hei/2*/size_hei + num_height))
            {
                dir = 1;
                break;
            }
            else if (press == false && (time_item[i].y >= (/*BREATHE_NUM_PIC_CENTER_Y + size.hei/2*/size_hei + num_height/2)))
            {
                dir = 1;
                break;
            }
        }
    }

    if (dir)
    {
        for(int i=0; i<5; i++)
        {
            time_item[i].load_id += dir;
            if (time_item[i].load_id < COMPO_ID_NUM_UU)
            {
                time_item[i].load_id = COMPO_ID_NUM_DD;
            }
            else if (time_item[i].load_id > COMPO_ID_NUM_DD)
            {
                time_item[i].load_id = COMPO_ID_NUM_UU;
            }
        }

        f_set->mcnt += dir;
    }

    if (press == false || dir)
    {
        for(int i=0; i<5; i++)
        {
            if (time_item[i].load_id == COMPO_ID_NUM_UU)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_UU-COMPO_ID_NUM_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_DD)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_DD-COMPO_ID_NUM_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_UP)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_UP-COMPO_ID_NUM_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_CENTER)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_CENTER-COMPO_ID_NUM_UU);
                time_item[i].alpha = 255;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_DOWN)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_DOWN-COMPO_ID_NUM_UU);
                time_item[i].alpha = 100;
            }
        }
    }

    for (int i=0; i<5; i++)
    {
        compo_textbox_t *txt_num = compo_getobj_byid(time_item[i].num_id);

        if (time_item[i].load_id == COMPO_ID_NUM_CENTER)
        {
            compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_38_BIN);
        }
        else
        {
            compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
        }

        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_visible(txt_num, time_item[i].visible_en);
        compo_textbox_set_alpha(txt_num, time_item[i].alpha);
        compo_textbox_set_pos(txt_num, time_item[i].x, time_item[i].y);


    }

    last_dy = dy;

    //printf("===============================\n");
    //for(int i=0; i<5; i++) {
    //    printf("[%d] id:[%d], vis:[%d], xy:[%d,%d], ret:[%d], dir[%d], press:[%d], hcnt:[%d], mcnt[%d]\n", time_item[i].num_id, time_item[i].load_id,
    //            time_item[i].visible_en, time_item[i].x, time_item[i].y, ret, dir, press, f_set->hcnt, f_set->mcnt);
    //}
    //printf("===============================\n");
}

static void func_breathe_move_handle(void)
{
    f_breathe_sub_time_t *f_set = (f_breathe_sub_time_t*)func_cb.f_cb;
    breathe_set_num_item_t *time_item;
    bool flag_press = 0;
    s32 dx = 0, dy = 0;     //坐标差量
    char buf[4];
    time_item = &f_set->set_num_item[0];
    u8 min = ((sys_cb.breathe_duration / 1000) % 3600) / 60;
    s8 min_disp = min;

    for (;;)
    {
        flag_press = ctp_get_dxy(&dx, &dy);
        //printf("x:%d, y:%d\n",dx, dy);

        breathe_set_num_pos_cal(dy, flag_press);
//        min_disp = (min - f_set->mcnt) % 60;
        min_disp = (min - f_set->mcnt) % BREATHE_SUB_TIME_MAX;

        for (int idx = 0; idx < 5; idx++)
        {
            memset(buf, 0, sizeof(buf));
            compo_textbox_t *txt_num = compo_getobj_byid(time_item[idx].num_id);
            if (time_item[idx].load_id == COMPO_ID_NUM_UU)        //上上次的时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(func_breathe_get_time_cal(min_disp, 1), 1));
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_UP)     //上次的时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min_disp, 1));
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_CENTER)      //当前时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min_disp, 0));
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_DOWN)     //下次时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min_disp, 2));
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_DD)       //下下次时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(func_breathe_get_time_cal(min_disp, 2), 2));
            }
            compo_textbox_set(txt_num, buf);
        }


        if (!flag_press)
        {
            f_set->mcnt = 0;
            min = func_breathe_get_time_cal(min_disp, 0);
            sys_cb.breathe_duration = min * 60 * 1000;
            TRACE(">>>breathe_duration:%ds\n", sys_cb.breathe_duration / 1000);
            break;
        }
        evt_message(msg_dequeue());
        func_process();
    }

}

////触摸按钮效果处理
static void func_breathe_sub_time_button_touch_handle(void)
{

    int id = compo_get_button_id();

    switch (id)
    {

        case COMPO_ID_BTN_SLIDE:
            func_breathe_move_handle();
            break;

        default:
            break;
    }

}

////释放按钮效果处理
//static void func_breathe_sub_time_button_release_handle(void)
//{
//    compo_picturebox_t *pic_start_click = compo_getobj_byid(COMPO_ID_PIC_OK_CLICK);
//    compo_picturebox_set_visible(pic_start_click, false);
//}

//单击按钮
static void func_breathe_sub_time_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_OK:
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
            if (func_cb.last == FUNC_BREATHE)
            {
                func_cb.sta = FUNC_BREATHE;
                task_stack_pop();
                break;
            }

            func_cb.sta = FUNC_BREATHE;
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
            break;

        default:
            break;
    }

//    func_breathe_sub_time_button_release_handle();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//组件ID
enum
{
    //按键
    COMPO_ID_BTN_OK = 1,
    COMPO_ID_BTN_SLIDE,

    //数字
    COMPO_ID_NUM_UU,
    COMPO_ID_NUM_UP,
    COMPO_ID_NUM_CENTER,
    COMPO_ID_NUM_DOWN,
    COMPO_ID_NUM_DD,
};

typedef struct breathe_set_num_item_t_
{
    int num_cnt;
    u16 num_id;
    s16 load_id;
    int val;
    s16 x;
    s16 y;
    bool visible_en;
    u8 alpha;
} breathe_set_num_item_t;

typedef struct f_breathe_sub_time_t_
{
    breathe_set_num_item_t set_num_item[5];
    area_t size;
    u16 num_height;
    s16 hcnt;
    s16 mcnt;
} f_breathe_sub_time_t;

#define BREATHE_NUM_PIC_CENTER_X              GUI_SCREEN_CENTER_X           //时间设置子页面位置x
#define BREATHE_NUM_PIC_CENTER_Y              (GUI_SCREEN_HEIGHT/2-20)           //时间设置子页面位置y
#define BREATHE_SET_NUM_ITEM_CNT             ((int)(sizeof(tbl_breathe_set_num_item) / sizeof(tbl_breathe_set_num_item[0])))
#define BREATHE_TXT_ITEM_M_X                  BREATHE_NUM_PIC_CENTER_X
#define BREATHE_TXT_ITEM_Y                    -20
#define BREATHE_TXT_ITEM_Y_OFFSET             40

#define BREATHE_CLOCK_SET_NUM_POS_Y_START                 BREATHE_TXT_ITEM_Y
#define BREATHE_CLOCK_SET_NUM_POS_Y(y_start,space,cnt)    ((y_start)+(space)*(cnt))

#define BREATHE_SUB_TIME_MIN                  1
#define BREATHE_SUB_TIME_MAX                  5

//搞个数字item，创建时遍历一下
static const breathe_set_num_item_t tbl_breathe_set_num_item[] =
{
    /*   num_cnt,      num_id,             load_num_id              val,                 x,                    y,                                             visible_en      alpha*/
    {2,        COMPO_ID_NUM_UU,       COMPO_ID_NUM_UU,          58,         BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y,                                true,           100},
    {2,        COMPO_ID_NUM_UP,       COMPO_ID_NUM_UP,          59,         BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y+BREATHE_TXT_ITEM_Y_OFFSET,      true,           100},
    {2,        COMPO_ID_NUM_CENTER,   COMPO_ID_NUM_CENTER,      0,          BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y+BREATHE_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {2,        COMPO_ID_NUM_DOWN,     COMPO_ID_NUM_DOWN,        1,          BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y+BREATHE_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {2,        COMPO_ID_NUM_DD,       COMPO_ID_NUM_DD,          2,          BREATHE_TXT_ITEM_M_X,         BREATHE_TXT_ITEM_Y+BREATHE_TXT_ITEM_Y_OFFSET*4,    true,           100},

};

//获取分 上一个和下一个的数字
static u8 func_breathe_get_time_cal(s8 num, u8 mode)
{
//    if (num < 0) {
//        num += 60;
//    }
//
//    if (mode == 1) {
//        if (num == 0 || num > 59) {
//            return 59;
//        }
//        return num - 1;
//    } else if (mode == 2){
//        if (num >= 59) {
//            return 0;
//        }
//        return num + 1;
//    } else {
//        if (num > 59) {
//            return 59;
//        }
//        return num;
//    }
    if (num < BREATHE_SUB_TIME_MIN)
    {
        num += BREATHE_SUB_TIME_MAX;
    }

    if (mode == BREATHE_SUB_TIME_MIN)
    {
        if (num == BREATHE_SUB_TIME_MIN || num > BREATHE_SUB_TIME_MAX)
        {
            return BREATHE_SUB_TIME_MAX;
        }
        return num - 1;
    }
    else if (mode == 2)
    {
        if (num >= BREATHE_SUB_TIME_MAX)
        {
            return BREATHE_SUB_TIME_MIN;
        }
        return num + 1;
    }
    else
    {
        if (num > BREATHE_SUB_TIME_MAX)
        {
            return BREATHE_SUB_TIME_MAX;
        }
        return num;
    }
}

//创建设置窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_breathe_sub_time_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BEATHER_DURATION]);

    //创建子页
    widget_page_t* sub_page = widget_page_create(frm->page_body);
    widget_set_location(sub_page, BREATHE_NUM_PIC_CENTER_X, BREATHE_NUM_PIC_CENTER_Y, GUI_SCREEN_WIDTH, BREATHE_TXT_ITEM_Y_OFFSET*3);

    //创建数字
    compo_textbox_t *txt_num;
    u8 min = ((sys_cb.breathe_duration / 1000) % 3600) / 60;

    char buf[4];
    for (u8 idx = 0; idx < BREATHE_SET_NUM_ITEM_CNT; idx++)
    {
        txt_num = compo_textbox_create_for_page(frm, sub_page, tbl_breathe_set_num_item[idx].num_cnt);
        compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
        compo_setid(txt_num, tbl_breathe_set_num_item[idx].num_id);
        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_pos(txt_num, tbl_breathe_set_num_item[idx].x, tbl_breathe_set_num_item[idx].y);
        compo_textbox_set_visible(txt_num, tbl_breathe_set_num_item[idx].visible_en);
        compo_textbox_set_alpha(txt_num, tbl_breathe_set_num_item[idx].alpha);

        memset(buf, 0, sizeof(buf));
        if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_UU)
        {
            snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(func_breathe_get_time_cal(min, 1), 1));
        }
        else if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_UP)
        {
            snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min, 1));
        }
        else if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_CENTER)
        {
            compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_38_BIN);
            snprintf(buf, sizeof(buf), "%02d", min);
        }
        else if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_DOWN)
        {
            snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min, 2));
        }
        else if (tbl_breathe_set_num_item[idx].load_id == COMPO_ID_NUM_DD)
        {
            snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(func_breathe_get_time_cal(min, 2), 2));
        }
        compo_textbox_set(txt_num, buf);
    }

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
    compo_setid(btn, COMPO_ID_BTN_OK);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 64/2+266);

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_SLIDE);
    compo_button_set_location(btn, BREATHE_NUM_PIC_CENTER_X, BREATHE_NUM_PIC_CENTER_Y, GUI_SCREEN_WIDTH/3, BREATHE_TXT_ITEM_Y_OFFSET*3);

    //新建图像
    compo_picturebox_t *pic_bg = compo_picturebox_create(frm, UI_BUF_I332001_BREATHE_BOX_BIN);
    compo_picturebox_set_pos(pic_bg, GUI_SCREEN_CENTER_X, BREATHE_NUM_PIC_CENTER_Y);

    //创建TEXT
    compo_textbox_t *txt = compo_textbox_create(frm, 10);
    compo_textbox_set_align_center(txt, true);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X, 64/2+266, 100, widget_text_get_height());
    compo_textbox_set_visible(txt, true);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_OK]);

    return frm;
}

static void breathe_set_num_pos_cal(s32 dy, bool press)
{
    f_breathe_sub_time_t *f_set = (f_breathe_sub_time_t*)func_cb.f_cb;

    //area_t size = f_set->size;
    u16 size_hei = BREATHE_TXT_ITEM_Y_OFFSET*3;
    u16 num_height = f_set->num_height;

    static s32 last_dy = 0;
    s8 dir = 0;
    s32 distance = 0;

    breathe_set_num_item_t *time_item;

    time_item = &f_set->set_num_item[0];

    if (press)
    {
        distance = dy - last_dy;
    }
    else
    {
        dy = 0;
        last_dy = 0;
        distance = 0;
    }

    time_item[0].y += distance;
    time_item[1].y += distance;
    time_item[2].y += distance;
    time_item[3].y += distance;
    time_item[4].y += distance;

    for (int i=0; i<5; i++)
    {
        if (time_item[i].load_id == COMPO_ID_NUM_UP)
        {
            if ((time_item[i].y /*- (BREATHE_NUM_PIC_CENTER_Y - size.hei/2)*/) <= -num_height)
            {
                dir = -1;
                break;
            }
            else if (press == false && ((time_item[i].y /*- (BREATHE_NUM_PIC_CENTER_Y - size.hei/2)*/) <= -num_height/2))
            {
                dir = -1;
                break;
            }
        }
        else if (time_item[i].load_id == COMPO_ID_NUM_DOWN)
        {
            if (time_item[i].y >= (/*BREATHE_NUM_PIC_CENTER_Y + size.hei/2*/size_hei + num_height))
            {
                dir = 1;
                break;
            }
            else if (press == false && (time_item[i].y >= (/*BREATHE_NUM_PIC_CENTER_Y + size.hei/2*/size_hei + num_height/2)))
            {
                dir = 1;
                break;
            }
        }
    }

    if (dir)
    {
        for(int i=0; i<5; i++)
        {
            time_item[i].load_id += dir;
            if (time_item[i].load_id < COMPO_ID_NUM_UU)
            {
                time_item[i].load_id = COMPO_ID_NUM_DD;
            }
            else if (time_item[i].load_id > COMPO_ID_NUM_DD)
            {
                time_item[i].load_id = COMPO_ID_NUM_UU;
            }
        }

        f_set->mcnt += dir;
    }

    if (press == false || dir)
    {
        for(int i=0; i<5; i++)
        {
            if (time_item[i].load_id == COMPO_ID_NUM_UU)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_UU-COMPO_ID_NUM_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_DD)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_DD-COMPO_ID_NUM_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_UP)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_UP-COMPO_ID_NUM_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_CENTER)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_CENTER-COMPO_ID_NUM_UU);
                time_item[i].alpha = 255;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_DOWN)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = BREATHE_CLOCK_SET_NUM_POS_Y(BREATHE_CLOCK_SET_NUM_POS_Y_START,BREATHE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_DOWN-COMPO_ID_NUM_UU);
                time_item[i].alpha = 100;
            }
        }
    }

    for (int i=0; i<5; i++)
    {
        compo_textbox_t *txt_num = compo_getobj_byid(time_item[i].num_id);

        if (time_item[i].load_id == COMPO_ID_NUM_CENTER)
        {
            compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_38_BIN);
        }
        else
        {
            compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
        }

        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_visible(txt_num, time_item[i].visible_en);
        compo_textbox_set_alpha(txt_num, time_item[i].alpha);
        compo_textbox_set_pos(txt_num, time_item[i].x, time_item[i].y);


    }

    last_dy = dy;

    //printf("===============================\n");
    //for(int i=0; i<5; i++) {
    //    printf("[%d] id:[%d], vis:[%d], xy:[%d,%d], ret:[%d], dir[%d], press:[%d], hcnt:[%d], mcnt[%d]\n", time_item[i].num_id, time_item[i].load_id,
    //            time_item[i].visible_en, time_item[i].x, time_item[i].y, ret, dir, press, f_set->hcnt, f_set->mcnt);
    //}
    //printf("===============================\n");
}

static void func_breathe_move_handle(void)
{
    f_breathe_sub_time_t *f_set = (f_breathe_sub_time_t*)func_cb.f_cb;
    breathe_set_num_item_t *time_item;
    bool flag_press = 0;
    s32 dx = 0, dy = 0;     //坐标差量
    char buf[4];
    time_item = &f_set->set_num_item[0];
    u8 min = ((sys_cb.breathe_duration / 1000) % 3600) / 60;
    s8 min_disp = min;

    for (;;)
    {
        flag_press = ctp_get_dxy(&dx, &dy);
        //printf("x:%d, y:%d\n",dx, dy);

        breathe_set_num_pos_cal(dy, flag_press);
//        min_disp = (min - f_set->mcnt) % 60;
        min_disp = (min - f_set->mcnt) % BREATHE_SUB_TIME_MAX;

        for (int idx = 0; idx < 5; idx++)
        {
            memset(buf, 0, sizeof(buf));
            compo_textbox_t *txt_num = compo_getobj_byid(time_item[idx].num_id);
            if (time_item[idx].load_id == COMPO_ID_NUM_UU)        //上上次的时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(func_breathe_get_time_cal(min_disp, 1), 1));
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_UP)     //上次的时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min_disp, 1));
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_CENTER)      //当前时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min_disp, 0));
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_DOWN)     //下次时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(min_disp, 2));
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_DD)       //下下次时间
            {
                snprintf(buf, sizeof(buf), "%02d", func_breathe_get_time_cal(func_breathe_get_time_cal(min_disp, 2), 2));
            }
            compo_textbox_set(txt_num, buf);
        }


        if (!flag_press)
        {
            f_set->mcnt = 0;
            min = func_breathe_get_time_cal(min_disp, 0);
            sys_cb.breathe_duration = min * 60 * 1000;
            TRACE(">>>breathe_duration:%ds\n", sys_cb.breathe_duration / 1000);
            break;
        }
        evt_message(msg_dequeue());
        func_process();
    }

}

////触摸按钮效果处理
static void func_breathe_sub_time_button_touch_handle(void)
{

    int id = compo_get_button_id();

    switch (id)
    {

        case COMPO_ID_BTN_SLIDE:
            func_breathe_move_handle();
            break;

        default:
            break;
    }

}

////释放按钮效果处理
//static void func_breathe_sub_time_button_release_handle(void)
//{
//    compo_picturebox_t *pic_start_click = compo_getobj_byid(COMPO_ID_PIC_OK_CLICK);
//    compo_picturebox_set_visible(pic_start_click, false);
//}

//单击按钮
static void func_breathe_sub_time_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_OK:
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
            if (func_cb.last == FUNC_BREATHE)
            {
                func_cb.sta = FUNC_BREATHE;
                task_stack_pop();
                break;
            }

            func_cb.sta = FUNC_BREATHE;
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
            break;

        default:
            break;
    }

//    func_breathe_sub_time_button_release_handle();
}
#endif // UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_SUPPORT

//设置功能事件处理
static void func_breathe_sub_time_process(void)
{
    func_process();
}

//设置功能消息处理
static void func_breathe_sub_time_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_breathe_sub_time_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_breathe_sub_time_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
//        func_breathe_sub_time_button_release_handle();
            break;

        case MSG_CTP_SHORT_RIGHT:
//        func_breathe_sub_time_button_release_handle();
            func_message(msg);
            break;

        case MSG_QDEC_BACKWARD:
        case MSG_QDEC_FORWARD:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入设置功能
static void func_breathe_sub_time_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_breathe_sub_time_t));
    func_cb.frm_main = func_breathe_sub_time_form_create();

    f_breathe_sub_time_t *f_set = (f_breathe_sub_time_t*) func_cb.f_cb;
    memcpy(f_set->set_num_item, tbl_breathe_set_num_item, sizeof(tbl_breathe_set_num_item));
    //f_set->size = gui_image_get_size(UI_BUF_ALARM_CLOCK_NUM_BIN);
    f_set->num_height = widget_text_get_height();

    func_cb.enter_tick = tick_get();
}

//退出设置功能
static void func_breathe_sub_time_exit(void)
{
    printf("%s\n",__func__);
    func_cb.last = FUNC_BREATHE_SUB_TIME;

    compo_form_destroy(func_cb.frm_main);
    func_free(func_cb.f_cb);
    func_cb.frm_main = NULL;
    func_cb.f_cb = NULL;
}

//呼吸设置功能
void func_breathe_sub_time(void)
{
    printf("%s\n", __func__);
    func_breathe_sub_time_enter();
    while (func_cb.sta == FUNC_BREATHE_SUB_TIME)
    {
        func_breathe_sub_time_process();
        func_breathe_sub_time_message(msg_dequeue());
    }
    func_breathe_sub_time_exit();
}
#endif






