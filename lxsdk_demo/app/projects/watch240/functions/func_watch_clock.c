#include "include.h"
#include "func.h"
#include "compo_bar.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
enum
{
    COMPO_ID_PIC_BG = 1,
    COMPO_ID_PIC_ICON,
    COMPO_ID_REC_ALPH,
    COMPO_ID_TXT,
    COMPO_ID_REC_SLIDER,
    COMPO_ID_REC_ALPH1,
};

typedef struct f_sliding_unlcok_t_
{
    u32 tick;
    u8 local;
} f_sliding_unlcok_t;

bool slider_unlock;

void func_sliding_unlcok_value_change_callback(int32_t change_value)
{
    printf("%s %d%%\n",__func__,change_value);
    compo_picturebox_t *pic;
    //f_sliding_unlcok_t *f_sliding_unlcok = (f_sliding_unlcok_t *)func_cb.f_cb;
    pic = compo_getobj_byid(COMPO_ID_PIC_ICON);
    u16 num = 220-change_value;
    if(num > 140)
    {
        num = 140;
    }
    if(change_value > 70)
    {
        slider_unlock = false;
    }
    if(change_value <= 70 && !slider_unlock)
    {
        printf("1999999999999999999999999999999\r\n");
        //func_directly_back_to();
        slider_unlock = true;
    }
    compo_picturebox_set_pos(pic, 45 + num, GUI_SCREEN_CENTER_Y);
    compo_shape_t *shape;
    shape = compo_getobj_byid(COMPO_ID_REC_SLIDER);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X + num/2, GUI_SCREEN_CENTER_Y, 200 - num,56);

}

//创建锁屏窗体
compo_form_t *func_sliding_unlcok_screen_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t *shape;
    compo_bar_t *slider;

    slider = compo_bar_create(frm);
    compo_bar_set_location(slider, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 200, 56);
    compo_bar_set_rabardius(slider, 28);
    compo_bar_set_color(slider,COLOR_BLACK, COLOR_BLACK);
    compo_bar_set_range(slider, 0, 200);
    compo_bar_set_value(slider, 200);
    compo_bar_set_dir(slider, COMPO_BAR_DIR_HORIZONTAL);
    compo_bar_set_forward_dir(slider, 1);
    compo_setid(slider, COMPO_ID_PIC_BG);
    compo_bar_set_value_change_cb(slider, func_sliding_unlcok_value_change_callback);


    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 200,56);
    compo_shape_set_radius(shape, 28);
    compo_shape_set_color(shape, COLOR_BG);
    compo_setid(shape, COMPO_ID_REC_SLIDER);

    compo_textbox_t * compo = compo_textbox_create(frm, strlen(i18n[STR_SLIDE_UNLOCK]));
    compo_textbox_set_font(compo, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_location((compo_textbox_t *)compo, GUI_SCREEN_CENTER_X+25, GUI_SCREEN_CENTER_Y,130,40);
    // compo_textbox_set_pos((compo_textbox_t *)compo, GUI_SCREEN_CENTER_X+20, GUI_SCREEN_CENTER_Y);
    compo_textbox_set((compo_textbox_t *)compo, i18n[STR_SLIDE_UNLOCK]);
    compo_setid(compo, COMPO_ID_TXT);


    //创建图片
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I330001_CHILD_LOCK_LOCK_BIN);
    compo_picturebox_set_pos(pic, 45, GUI_SCREEN_CENTER_Y);
    compo_setid(pic, COMPO_ID_PIC_ICON);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 40,40);
    compo_shape_set_color(shape, COLOR_GRAY);
    compo_shape_set_alpha(shape, 140);
    compo_shape_set_radius(shape, 20);
    compo_setid(shape, COMPO_ID_REC_ALPH);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_CHILD_LOCK_TIMEBG_BIN);///背景图片
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,296-15 );

    compo_textbox_t *txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_20_BIN);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 19,296-16, 240, 40);
    compo_bonddata(txt, COMPO_BOND_HOUR);

    compo = compo_textbox_create(frm, 5);
    compo_textbox_set_font(compo, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_pos((compo_textbox_t *)compo, GUI_SCREEN_CENTER_X,  296-18);
    compo_textbox_set((compo_textbox_t *)compo, ":");

    txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_20_BIN);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 19, 296-16, 240, 40);
    compo_bonddata(txt, COMPO_BOND_MINUTE);

    shape =compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 296-15, 84,30);
    compo_shape_set_color(shape, COLOR_BLACK);
    compo_shape_set_alpha(shape, 140);
    compo_shape_set_radius(shape, 15);
    compo_setid(shape, COMPO_ID_REC_ALPH1);
    return frm;
}

//锁屏功能事件处理
static void func_sliding_unlcok_screen_process(void)
{
    compo_bar_t *slider;
    compo_shape_t *shape;
    compo_textbox_t * txt;
    compo_picturebox_t *pic;
    f_sliding_unlcok_t *f_sliding_unlcok = (f_sliding_unlcok_t *)func_cb.f_cb;
    slider = compo_getobj_byid(COMPO_ID_PIC_BG);
    shape = compo_getobj_byid(COMPO_ID_REC_ALPH);
    txt = compo_getobj_byid(COMPO_ID_TXT);

    compo_bar_move_ctr(slider);

    if(tick_check_expire(f_sliding_unlcok->tick,50))
    {
        f_sliding_unlcok->tick = tick_get();
        f_sliding_unlcok->local = f_sliding_unlcok->local + 2;
        if(f_sliding_unlcok->local > 60)
        {
            f_sliding_unlcok->local = 0;
        }
        compo_shape_set_pos(shape, GUI_SCREEN_CENTER_X + 10 + f_sliding_unlcok->local, GUI_SCREEN_CENTER_Y);
        if(slider->move_ctr.touch_flag)
        {
            compo_shape_set_visible(shape, false);
            compo_textbox_set_visible(txt, false);

        }
        else
        {
            compo_shape_set_visible(shape, true);
            compo_textbox_set_visible(txt, true);
            slider = compo_getobj_byid(COMPO_ID_PIC_BG);
            compo_bar_set_value(slider, 200);
            pic = compo_getobj_byid(COMPO_ID_PIC_ICON);
            compo_picturebox_set_pos(pic, 45, GUI_SCREEN_CENTER_Y);
            shape = compo_getobj_byid(COMPO_ID_REC_SLIDER);
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 200,56);
            if(slider_unlock)
            {
                //  slider_unlock = false;
                // printf("1211slider_unlock=%d\r\n",slider_unlock);
                // if(sys_cb.password_cnt == 4){
                //     func_cb.sta = FUNC_PASSWORD_SUB_DISP;
                //     sys_cb.password_flag = true;
                // } else {
                func_directly_back_to();
                // }

            }
        }
    }
    func_process();
}

//锁屏功能消息处理
static void func_sliding_unlcok_screen_message(size_msg_t msg)
{
    compo_bar_t *slider;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            break;
        case MSG_CTP_TOUCH:
            slider = compo_getobj_byid(COMPO_ID_PIC_BG);
            compo_bar_move_status(slider, true);
            printf("MSG_CTP_TOUCH >>>>>>> \n");
            break;
        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
            printf("MSG_CTP_SHORT_LEFT >>>>>>> \n");
            break;
        case MSG_CTP_LONG:
            break;
        case KU_BACK:
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入锁屏功能
static void func_sliding_unlcok_screen_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sliding_unlcok_t));
    func_cb.frm_main = func_sliding_unlcok_screen_form_create();

}

//退出锁屏功能
static void func_sliding_unlcok_screen_exit(void)
{
    func_cb.last = FUNC_SLIDING_UNLOCK_SCREEN;
}

//锁屏功能
void func_sliding_unlcok_screen(void)
{
    printf("%s\n", __func__);
    func_sliding_unlcok_screen_enter();
    while (func_cb.sta == FUNC_SLIDING_UNLOCK_SCREEN)
    {
        func_sliding_unlcok_screen_process();
        func_sliding_unlcok_screen_message(msg_dequeue());
    }
    func_sliding_unlcok_screen_exit();
}
