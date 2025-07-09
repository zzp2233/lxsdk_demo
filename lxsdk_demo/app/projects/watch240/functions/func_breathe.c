#include "include.h"
#include "func.h"
#include "ute_module_heart.h"

#if UTE_MODULE_SCREENS_BREATHE_SUPPORT

#define TRACE_EN    1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define BREATHE_TIME_MIN    (1 * 60 * 1000)//(10 * 1000)  //时间设置最小值,ms
#define BREATHE_TIME_MAX    (30 * 60 * 1000)  //时间设置最大值

//速度模式
enum
{
    BREATHE_MODE_SLOW,          //缓慢
    BREATHE_MODE_MEDIUM,        //舒缓
    BREATHE_MODE_FAST,          //稍快
};

#define ANIMATION_TICK_EXPIRE   20  //动画帧间隔，ms
#define BREATHE_ICON_SIZE_MIN   45  //图标缩放最小尺寸
#define BREATHE_ICON_SIZE_MAX   170
#define BREATHE_PREPARE_TIME    1800  //入场动画持续时间
#define BREATHE_FINISH_TIME     500  //出场动画持续时间
#define BREATHE_RHYTHM0         5000  //呼/吸气时间（慢）
#define BREATHE_RHYTHM1         4300  //中
#define BREATHE_RHYTHM2         3800  //快

//呼吸检测状态
enum
{
    BREATHE_STA_IDLE,           //空闲
    BREATHE_STA_WORKING,        //呼吸中
};

//动画状态
enum
{
    ANIMATION_PREPARE,          //入场动画
    ANIMATION_DELAY,            //保持静止
    ANIMATION_INHALE,           //吸气
//    ANIMATION_HOLD,             //屏气
    ANIMATION_EXHALE,           //呼气
    ANIMATION_FINISH,           //返回idle状态
};

enum
{
    //按键
    COMPO_ID_BTN_START = 1,
    COMPO_ID_BTN_MODE,
    COMPO_ID_BTN_TIME,

    //图像
    COMPO_ID_PIC_BREATHE,

    //文本框
    COMPO_ID_TXT_START,
};

typedef struct f_breathe_t_
{
    u8 sta;
    u32 tick;
    u32 time_past;
    u8 animation_sta;
    u16 icon_org_size;
    u16 icon_size;
    s16 icon_deg;
} f_breathe_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//创建呼吸窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_breathe_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_TRAIN]);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
    compo_setid(btn, COMPO_ID_BTN_START);
    compo_button_set_pos(btn, 120, 247);

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_BREATHE_TIME_BIN);
    compo_setid(btn, COMPO_ID_BTN_TIME);
    compo_button_set_pos(btn, 32, 133);

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_BREATHE_SPEED_BIN);
//    widget_set_size(btn->widget, gui_image_get_size(UI_BUF_BREATHE_TIME_BIN).wid, gui_image_get_size(UI_BUF_BREATHE_TIME_BIN).hei);
    compo_setid(btn, COMPO_ID_BTN_MODE);
    compo_button_set_pos(btn, 208, 133);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I330001_BREATHE_BG_BIN);
    compo_setid(pic, COMPO_ID_PIC_BREATHE);
    compo_picturebox_set_pos(pic, 120, 133);

    //创建文本
    compo_textbox_t *txt_start = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt_start,120, 247);
    compo_setid(txt_start, COMPO_ID_TXT_START);
    compo_textbox_set(txt_start, i18n[STR_START]);

    return frm;
}

//页面状态转换时组件显示参数初始化
static void func_breathe_disp_init(u8 page_sta)
{
    f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    compo_button_t *btn = NULL;
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TXT_START);
    compo_picturebox_t *pic_breathe = compo_getobj_byid(COMPO_ID_PIC_BREATHE);
    switch (page_sta)
    {
        case BREATHE_STA_IDLE:
            compo_textbox_set(txt, i18n[STR_START]);
            btn = compo_getobj_byid(COMPO_ID_BTN_START);
            compo_button_set_visible(btn, true);
            btn = compo_getobj_byid(COMPO_ID_BTN_TIME);
            compo_button_set_visible(btn, true);
            btn = compo_getobj_byid(COMPO_ID_BTN_MODE);
            compo_button_set_visible(btn, true);
            f_breathe->sta = BREATHE_STA_IDLE;
            break;

        case BREATHE_STA_WORKING:
            compo_textbox_set(txt, "");
            btn = compo_getobj_byid(COMPO_ID_BTN_START);
            compo_button_set_visible(btn, false);
            btn = compo_getobj_byid(COMPO_ID_BTN_TIME);
            compo_button_set_visible(btn, false);
            btn = compo_getobj_byid(COMPO_ID_BTN_MODE);
            compo_button_set_visible(btn, false);
            f_breathe->sta = BREATHE_STA_IDLE;

            f_breathe->animation_sta = ANIMATION_PREPARE;
            f_breathe->time_past = 0;
            f_breathe->icon_deg = 0;
            f_breathe->icon_size = f_breathe->icon_org_size;
            f_breathe->tick = tick_get();
            break;

        default:
            break;
    }
    compo_picturebox_set_size(pic_breathe, f_breathe->icon_org_size, f_breathe->icon_org_size);
    compo_picturebox_set_rotation(pic_breathe, 0);
}

//呼吸功能事件处理
static void func_breathe_process(void)
{
    static u16 frames, rhythm, size_step_time, finish_deg;
    static s16 deg_step, size_step;
    s16 temp;
    f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    compo_picturebox_t *pic_breathe = compo_getobj_byid(COMPO_ID_PIC_BREATHE);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TXT_START);

    switch (f_breathe->sta)
    {
        case BREATHE_STA_IDLE:
            break;
        case BREATHE_STA_WORKING:
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(f_breathe->tick, ANIMATION_TICK_EXPIRE))
            {
                f_breathe->tick = tick_get();
                f_breathe->time_past += ANIMATION_TICK_EXPIRE;
                switch (f_breathe->animation_sta)
                {
                    case ANIMATION_PREPARE:
                        if (f_breathe->time_past <= ANIMATION_TICK_EXPIRE)
                        {
                            TRACE("BREATHE PREPARE\n");
                            frames = (BREATHE_PREPARE_TIME / ANIMATION_TICK_EXPIRE / 2);
                            deg_step = (-3600 - frames / 2) / frames;  //转1圈
                            temp = (BREATHE_ICON_SIZE_MAX - f_breathe->icon_size) + (BREATHE_ICON_SIZE_MAX - BREATHE_ICON_SIZE_MIN);
                            size_step = (temp + frames / 2) / frames;
                            if (size_step == 0)
                            {
                                size_step_time = ANIMATION_TICK_EXPIRE * ((frames + temp / 2) / temp);
                                size_step = 1;
                            }
                            else
                            {
                                size_step_time = ANIMATION_TICK_EXPIRE;
                            }
                        }
                        f_breathe->icon_deg += deg_step;
                        if (f_breathe->time_past % size_step_time == 0)
                        {
                            f_breathe->icon_size += size_step;  //先放大后缩小
                            if (f_breathe->icon_size >= BREATHE_ICON_SIZE_MAX)
                            {
                                size_step = -size_step;
                            }
                            else if (f_breathe->icon_size <= BREATHE_ICON_SIZE_MIN)
                            {
                                size_step = 0;
                            }
                        }
                        if (--frames == 0)
                        {
                            f_breathe->icon_deg = 0;
                            f_breathe->icon_size = BREATHE_ICON_SIZE_MIN;
                            f_breathe->animation_sta = ANIMATION_DELAY;
                            f_breathe->time_past = 0;
                        }
//                TRACE("%d deg[%d] size[%d]\n", frames, f_breathe->icon_deg, f_breathe->icon_size);
                        break;

                    case ANIMATION_DELAY:
                        if (f_breathe->time_past <= ANIMATION_TICK_EXPIRE)
                        {
                            TRACE("BREATHE DELAY\n");
                            frames = (BREATHE_PREPARE_TIME / ANIMATION_TICK_EXPIRE / 2);  //入场动画一半运动一半静止
                        }
                        if (--frames == 0)
                        {
                            f_breathe->animation_sta = ANIMATION_INHALE;
                            f_breathe->time_past = 0;
                        }
                        break;

                    case ANIMATION_INHALE:
//            case ANIMATION_EXHALE:
                        if (f_breathe->time_past <= ANIMATION_TICK_EXPIRE)
                        {
                            TRACE("BREATHE START\n");
                            compo_textbox_set(txt, i18n[STR_BREATHE_INHALE]);
                            rhythm = sys_cb.breathe_mode == BREATHE_MODE_SLOW ? BREATHE_RHYTHM0 : \
                                     (sys_cb.breathe_mode == BREATHE_MODE_FAST ? BREATHE_RHYTHM2 : BREATHE_RHYTHM1);
                            frames = (rhythm / ANIMATION_TICK_EXPIRE);
                            deg_step = 1800 / frames;  //呼吸各半圈
                            temp = BREATHE_ICON_SIZE_MAX - BREATHE_ICON_SIZE_MIN;
                            size_step = ((temp + frames) / 2) / frames;
                            if (size_step == 0)
                            {
                                size_step_time = ANIMATION_TICK_EXPIRE * ((frames + temp / 2) / temp);
                                size_step = 1;
                            }
                            else
                            {
                                size_step_time = ANIMATION_TICK_EXPIRE;
                            }
//                    TRACE("rhythm[%d] frames[%d] degstep[%d] sizestep[%d]\n", rhythm, frames, deg_step, size_step);
                        }
                        if (f_breathe->time_past % size_step_time == 0)
                        {
                            if (size_step > 0)
                            {
                                if (f_breathe->icon_size <= BREATHE_ICON_SIZE_MAX - size_step)
                                {
                                    f_breathe->icon_size += size_step;
                                    //                        f_breathe->icon_deg = (f_breathe->icon_deg + deg_step) % 3600;  //缩放旋转一起停
                                }
                                else
                                {
                                    f_breathe->icon_size = BREATHE_ICON_SIZE_MAX;
                                }
                            }
                            else
                            {
                                if (f_breathe->icon_size >= BREATHE_ICON_SIZE_MIN - size_step)
                                {
                                    f_breathe->icon_size += size_step;
                                    //                        f_breathe->icon_deg = (f_breathe->icon_deg + deg_step) % 3600;
                                }
                                else
                                {
                                    f_breathe->icon_size = BREATHE_ICON_SIZE_MIN;
                                }
                            }
                        }
                        f_breathe->icon_deg = (f_breathe->icon_deg + deg_step) % 3600;  //缩放停止后继续旋转
                        if (f_breathe->time_past % rhythm == 0)
                        {
                            size_step = -size_step;
                            if (abs_s(f_breathe->icon_size - BREATHE_ICON_SIZE_MAX) < abs_s(f_breathe->icon_size - BREATHE_ICON_SIZE_MIN))
                            {
                                compo_textbox_set(txt, i18n[STR_BREATHE_EXHALE]);  //呼气
                                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                            }
                            else
                            {
                                compo_textbox_set(txt, i18n[STR_BREATHE_INHALE]);  //吸气
//                                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                            }
                            TRACE("total:%ds past:%ds\n", sys_cb.breathe_duration / 1000, f_breathe->time_past / 1000);
//                    TRACE("deg[%d] size[%d] degstep[%d] sizestep[%d]\n", f_breathe->icon_deg, f_breathe->icon_size, deg_step, size_step);
                        }
                        if (f_breathe->time_past >= sys_cb.breathe_duration)
                        {
                            f_breathe->time_past = 0;
                            f_breathe->animation_sta = ANIMATION_FINISH;

                            func_cb.sta = FUNC_BREATHE_FINISH;
                            f_breathe->sta = BREATHE_STA_IDLE;

                        }
                        break;

                    case ANIMATION_FINISH:
                        if (f_breathe->time_past <= ANIMATION_TICK_EXPIRE)
                        {
                            TRACE("BREATHE FINISH\n");
                            compo_textbox_set(txt, "");
                            frames = BREATHE_FINISH_TIME / ANIMATION_TICK_EXPIRE;
                            finish_deg = abs_s(0 - f_breathe->icon_deg) <= abs_s(3600 - f_breathe->icon_deg) ? 0 : 3600;
                            temp = finish_deg - f_breathe->icon_deg;
                            deg_step = (temp + (temp < 0 ? -(frames / 2) : (frames / 2))) / frames;
                            if (deg_step == 0 && temp != 0)
                            {
                                deg_step = temp < 0 ? -1 : 1;
                            }
                            temp = f_breathe->icon_org_size - f_breathe->icon_size;
                            size_step = (temp + (temp < 0 ? -(frames / 2) : (frames / 2))) / frames;
                            if (size_step == 0 && temp != 0)
                            {
                                size_step = temp < 0 ? -1 : 1;
                            }
                        }
//                if (f_breathe->time_past % 100 == 0) {
//                    TRACE("deg[%d] size[%d] degstep[%d] sizestep[%d]\n", f_breathe->icon_deg, f_breathe->icon_size, deg_step, size_step);
//                }
                        if (f_breathe->icon_deg != finish_deg || f_breathe->icon_size != f_breathe->icon_org_size)
                        {
                            if (abs_s(finish_deg - f_breathe->icon_deg) <= abs_s(deg_step))
                            {
                                f_breathe->icon_deg = finish_deg;
                            }
                            else
                            {
                                f_breathe->icon_deg += deg_step;
                            }
                            if (abs_s(f_breathe->icon_org_size - f_breathe->icon_size) <= abs_s(size_step))
                            {
                                f_breathe->icon_size = f_breathe->icon_org_size;
                            }
                            else
                            {
                                f_breathe->icon_size += size_step;
                            }
                            if (f_breathe->time_past > BREATHE_FINISH_TIME * 2)    //意外超时直接退出
                            {
                                f_breathe->icon_deg = finish_deg;
                                f_breathe->icon_size = f_breathe->icon_org_size;
                            }
                        }
                        else
                        {
                            func_breathe_disp_init(BREATHE_STA_IDLE);
//                            func_cb.sta = FUNC_BREATHE_FINISH;
                            f_breathe->sta = BREATHE_STA_IDLE;
//                            printf("1111111111111111111111111111111111111\n");
                        }
                        break;

                    default:
                        break;
                }
                compo_picturebox_set_size(pic_breathe, f_breathe->icon_size, f_breathe->icon_size);
                compo_picturebox_set_rotation(pic_breathe, f_breathe->icon_deg);
            }
            break;

        default:
            break;
    }

    func_process();
}


//单击按钮
static void func_breathe_button_click(void)
{
    f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch(id)
    {
        case COMPO_ID_BTN_START:
            if (f_breathe->sta == BREATHE_STA_IDLE)
            {
                func_breathe_disp_init(BREATHE_STA_WORKING);
                f_breathe->sta = BREATHE_STA_WORKING;
            }
            break;

        case COMPO_ID_BTN_TIME:
            func_cb.sta = FUNC_BREATHE_SUB_TIME;
            break;

        case COMPO_ID_BTN_MODE:
            func_cb.sta = FUNC_BREATHE_SUB_MODE;
            break;

        default:
            break;
    }
}

//呼吸功能消息处理
static void func_breathe_message(size_msg_t msg)
{
    f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    if (f_breathe->sta == BREATHE_STA_IDLE)
    {
        switch (msg)
        {
            case MSG_CTP_TOUCH:
                break;

            case MSG_CTP_CLICK:
                func_breathe_button_click();
                break;

            case MSG_CTP_SHORT_UP:
            case MSG_CTP_SHORT_DOWN:
            case MSG_CTP_SHORT_LEFT:
            case MSG_CTP_LONG:
                if (func_cb.flag_sort)
                {
                    func_message(msg);
                }
                break;

            case MSG_CTP_SHORT_RIGHT:
                func_message(msg);
                break;

            case MSG_QDEC_FORWARD:
            case MSG_QDEC_BACKWARD:
                break;

            default:
                func_message(msg);
                break;
        }
    }
    else
    {
        switch (msg)
        {
            case KU_BACK:
                f_breathe->time_past = 0;
                f_breathe->animation_sta = ANIMATION_FINISH;
                break;

            case MSG_CTP_SHORT_RIGHT:  //右滑不退出
                break;

            default:
                func_message(msg);
                break;
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建呼吸窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_breathe_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_TRAIN]);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);
    compo_setid(btn, COMPO_ID_BTN_START);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 64/2+266);

    btn = compo_button_create_by_image(frm, UI_BUF_I332001_BREATHE_TIME_BIN);
    compo_setid(btn, COMPO_ID_BTN_TIME);
    compo_button_set_pos(btn, 54/2+18, 54/2+153);

    btn = compo_button_create_by_image(frm, UI_BUF_I332001_BREATHE_SPEED_BIN);
//    widget_set_size(btn->widget, gui_image_get_size(UI_BUF_BREATHE_TIME_BIN).wid, gui_image_get_size(UI_BUF_BREATHE_TIME_BIN).hei);
    compo_setid(btn, COMPO_ID_BTN_MODE);
    compo_button_set_pos(btn, 54/2+288, 54/2+153);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I332001_BREATHE_BG_BIN);
    compo_setid(pic, COMPO_ID_PIC_BREATHE);
    compo_picturebox_set_pos(pic, 186/2+87,186/2+79);

    //创建文本
    compo_textbox_t *txt_start = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt_start,GUI_SCREEN_CENTER_X, 64/2+266);
    compo_setid(txt_start, COMPO_ID_TXT_START);
    compo_textbox_set(txt_start, i18n[STR_START]);

    return frm;
}

//页面状态转换时组件显示参数初始化
static void func_breathe_disp_init(u8 page_sta)
{
    f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    compo_button_t *btn = NULL;
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TXT_START);
    compo_picturebox_t *pic_breathe = compo_getobj_byid(COMPO_ID_PIC_BREATHE);
    switch (page_sta)
    {
        case BREATHE_STA_IDLE:
            compo_textbox_set(txt, i18n[STR_START]);
            btn = compo_getobj_byid(COMPO_ID_BTN_START);
            compo_button_set_visible(btn, true);
            btn = compo_getobj_byid(COMPO_ID_BTN_TIME);
            compo_button_set_visible(btn, true);
            btn = compo_getobj_byid(COMPO_ID_BTN_MODE);
            compo_button_set_visible(btn, true);
            f_breathe->sta = BREATHE_STA_IDLE;
            break;

        case BREATHE_STA_WORKING:
            compo_textbox_set(txt, "");
            btn = compo_getobj_byid(COMPO_ID_BTN_START);
            compo_button_set_visible(btn, false);
            btn = compo_getobj_byid(COMPO_ID_BTN_TIME);
            compo_button_set_visible(btn, false);
            btn = compo_getobj_byid(COMPO_ID_BTN_MODE);
            compo_button_set_visible(btn, false);
            f_breathe->sta = BREATHE_STA_IDLE;

            f_breathe->animation_sta = ANIMATION_PREPARE;
            f_breathe->time_past = 0;
            f_breathe->icon_deg = 0;
            f_breathe->icon_size = f_breathe->icon_org_size;
            f_breathe->tick = tick_get();
            break;

        default:
            break;
    }
    compo_picturebox_set_size(pic_breathe, f_breathe->icon_org_size, f_breathe->icon_org_size);
    compo_picturebox_set_rotation(pic_breathe, 0);
}

//呼吸功能事件处理
static void func_breathe_process(void)
{
    static u16 frames, rhythm, size_step_time, finish_deg;
    static s16 deg_step, size_step;
    s16 temp;
    f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    compo_picturebox_t *pic_breathe = compo_getobj_byid(COMPO_ID_PIC_BREATHE);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TXT_START);

    switch (f_breathe->sta)
    {
        case BREATHE_STA_IDLE:
            break;
        case BREATHE_STA_WORKING:
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(f_breathe->tick, ANIMATION_TICK_EXPIRE))
            {
                f_breathe->tick = tick_get();
                f_breathe->time_past += ANIMATION_TICK_EXPIRE;
                switch (f_breathe->animation_sta)
                {
                    case ANIMATION_PREPARE:
                        if (f_breathe->time_past <= ANIMATION_TICK_EXPIRE)
                        {
                            TRACE("BREATHE PREPARE\n");
                            frames = (BREATHE_PREPARE_TIME / ANIMATION_TICK_EXPIRE / 2);
                            deg_step = (-3600 - frames / 2) / frames;  //转1圈
                            temp = (BREATHE_ICON_SIZE_MAX - f_breathe->icon_size) + (BREATHE_ICON_SIZE_MAX - BREATHE_ICON_SIZE_MIN);
                            size_step = (temp + frames / 2) / frames;
                            if (size_step == 0)
                            {
                                size_step_time = ANIMATION_TICK_EXPIRE * ((frames + temp / 2) / temp);
                                size_step = 1;
                            }
                            else
                            {
                                size_step_time = ANIMATION_TICK_EXPIRE;
                            }
                        }
                        f_breathe->icon_deg += deg_step;
                        if (f_breathe->time_past % size_step_time == 0)
                        {
                            f_breathe->icon_size += size_step;  //先放大后缩小
                            if (f_breathe->icon_size >= BREATHE_ICON_SIZE_MAX)
                            {
                                size_step = -size_step;
                            }
                            else if (f_breathe->icon_size <= BREATHE_ICON_SIZE_MIN)
                            {
                                size_step = 0;
                            }
                        }
                        if (--frames == 0)
                        {
                            f_breathe->icon_deg = 0;
                            f_breathe->icon_size = BREATHE_ICON_SIZE_MIN;
                            f_breathe->animation_sta = ANIMATION_DELAY;
                            f_breathe->time_past = 0;
                        }
//                TRACE("%d deg[%d] size[%d]\n", frames, f_breathe->icon_deg, f_breathe->icon_size);
                        break;

                    case ANIMATION_DELAY:
                        if (f_breathe->time_past <= ANIMATION_TICK_EXPIRE)
                        {
                            TRACE("BREATHE DELAY\n");
                            frames = (BREATHE_PREPARE_TIME / ANIMATION_TICK_EXPIRE / 2);  //入场动画一半运动一半静止
                        }
                        if (--frames == 0)
                        {
                            f_breathe->animation_sta = ANIMATION_INHALE;
                            f_breathe->time_past = 0;
                        }
                        break;

                    case ANIMATION_INHALE:
//            case ANIMATION_EXHALE:
                        if (f_breathe->time_past <= ANIMATION_TICK_EXPIRE)
                        {
                            TRACE("BREATHE START\n");
                            compo_textbox_set(txt, i18n[STR_BREATHE_INHALE]);
                            rhythm = sys_cb.breathe_mode == BREATHE_MODE_SLOW ? BREATHE_RHYTHM0 : \
                                     (sys_cb.breathe_mode == BREATHE_MODE_FAST ? BREATHE_RHYTHM2 : BREATHE_RHYTHM1);
                            frames = (rhythm / ANIMATION_TICK_EXPIRE);
                            deg_step = 1800 / frames;  //呼吸各半圈
                            temp = BREATHE_ICON_SIZE_MAX - BREATHE_ICON_SIZE_MIN;
                            size_step = ((temp + frames) / 2) / frames;
                            if (size_step == 0)
                            {
                                size_step_time = ANIMATION_TICK_EXPIRE * ((frames + temp / 2) / temp);
                                size_step = 1;
                            }
                            else
                            {
                                size_step_time = ANIMATION_TICK_EXPIRE;
                            }
//                    TRACE("rhythm[%d] frames[%d] degstep[%d] sizestep[%d]\n", rhythm, frames, deg_step, size_step);
                        }
                        if (f_breathe->time_past % size_step_time == 0)
                        {
                            if (size_step > 0)
                            {
                                if (f_breathe->icon_size <= BREATHE_ICON_SIZE_MAX - size_step)
                                {
                                    f_breathe->icon_size += size_step;
                                    //                        f_breathe->icon_deg = (f_breathe->icon_deg + deg_step) % 3600;  //缩放旋转一起停
                                }
                                else
                                {
                                    f_breathe->icon_size = BREATHE_ICON_SIZE_MAX;
                                }
                            }
                            else
                            {
                                if (f_breathe->icon_size >= BREATHE_ICON_SIZE_MIN - size_step)
                                {
                                    f_breathe->icon_size += size_step;
                                    //                        f_breathe->icon_deg = (f_breathe->icon_deg + deg_step) % 3600;
                                }
                                else
                                {
                                    f_breathe->icon_size = BREATHE_ICON_SIZE_MIN;
                                }
                            }
                        }
                        f_breathe->icon_deg = (f_breathe->icon_deg + deg_step) % 3600;  //缩放停止后继续旋转
                        if (f_breathe->time_past % rhythm == 0)
                        {
                            size_step = -size_step;
                            if (abs_s(f_breathe->icon_size - BREATHE_ICON_SIZE_MAX) < abs_s(f_breathe->icon_size - BREATHE_ICON_SIZE_MIN))
                            {
                                compo_textbox_set(txt, i18n[STR_BREATHE_EXHALE]);  //呼气
                                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                            }
                            else
                            {
                                compo_textbox_set(txt, i18n[STR_BREATHE_INHALE]);  //吸气
//                                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                            }
                            TRACE("total:%ds past:%ds\n", sys_cb.breathe_duration / 1000, f_breathe->time_past / 1000);
//                    TRACE("deg[%d] size[%d] degstep[%d] sizestep[%d]\n", f_breathe->icon_deg, f_breathe->icon_size, deg_step, size_step);
                        }
                        if (f_breathe->time_past >= sys_cb.breathe_duration)
                        {
                            f_breathe->time_past = 0;
                            f_breathe->animation_sta = ANIMATION_FINISH;

                            func_cb.sta = FUNC_BREATHE_FINISH;
                            f_breathe->sta = BREATHE_STA_IDLE;

                        }
                        break;

                    case ANIMATION_FINISH:
                        if (f_breathe->time_past <= ANIMATION_TICK_EXPIRE)
                        {
                            TRACE("BREATHE FINISH\n");
                            compo_textbox_set(txt, "");
                            frames = BREATHE_FINISH_TIME / ANIMATION_TICK_EXPIRE;
                            finish_deg = abs_s(0 - f_breathe->icon_deg) <= abs_s(3600 - f_breathe->icon_deg) ? 0 : 3600;
                            temp = finish_deg - f_breathe->icon_deg;
                            deg_step = (temp + (temp < 0 ? -(frames / 2) : (frames / 2))) / frames;
                            if (deg_step == 0 && temp != 0)
                            {
                                deg_step = temp < 0 ? -1 : 1;
                            }
                            temp = f_breathe->icon_org_size - f_breathe->icon_size;
                            size_step = (temp + (temp < 0 ? -(frames / 2) : (frames / 2))) / frames;
                            if (size_step == 0 && temp != 0)
                            {
                                size_step = temp < 0 ? -1 : 1;
                            }
                        }
//                if (f_breathe->time_past % 100 == 0) {
//                    TRACE("deg[%d] size[%d] degstep[%d] sizestep[%d]\n", f_breathe->icon_deg, f_breathe->icon_size, deg_step, size_step);
//                }
                        if (f_breathe->icon_deg != finish_deg || f_breathe->icon_size != f_breathe->icon_org_size)
                        {
                            if (abs_s(finish_deg - f_breathe->icon_deg) <= abs_s(deg_step))
                            {
                                f_breathe->icon_deg = finish_deg;
                            }
                            else
                            {
                                f_breathe->icon_deg += deg_step;
                            }
                            if (abs_s(f_breathe->icon_org_size - f_breathe->icon_size) <= abs_s(size_step))
                            {
                                f_breathe->icon_size = f_breathe->icon_org_size;
                            }
                            else
                            {
                                f_breathe->icon_size += size_step;
                            }
                            if (f_breathe->time_past > BREATHE_FINISH_TIME * 2)    //意外超时直接退出
                            {
                                f_breathe->icon_deg = finish_deg;
                                f_breathe->icon_size = f_breathe->icon_org_size;
                            }
                        }
                        else
                        {
                            func_breathe_disp_init(BREATHE_STA_IDLE);
//                            func_cb.sta = FUNC_BREATHE_FINISH;
                            f_breathe->sta = BREATHE_STA_IDLE;
//                            printf("1111111111111111111111111111111111111\n");
                        }
                        break;

                    default:
                        break;
                }
                compo_picturebox_set_size(pic_breathe, f_breathe->icon_size, f_breathe->icon_size);
                compo_picturebox_set_rotation(pic_breathe, f_breathe->icon_deg);
            }
            break;

        default:
            break;
    }

    func_process();
}


//单击按钮
static void func_breathe_button_click(void)
{
    f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch(id)
    {
        case COMPO_ID_BTN_START:
            if (f_breathe->sta == BREATHE_STA_IDLE)
            {
                func_breathe_disp_init(BREATHE_STA_WORKING);
                f_breathe->sta = BREATHE_STA_WORKING;
            }
            break;

        case COMPO_ID_BTN_TIME:
            func_cb.sta = FUNC_BREATHE_SUB_TIME;
            break;

        case COMPO_ID_BTN_MODE:
            func_cb.sta = FUNC_BREATHE_SUB_MODE;
            break;

        default:
            break;
    }
}

//呼吸功能消息处理
static void func_breathe_message(size_msg_t msg)
{
    f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    if (f_breathe->sta == BREATHE_STA_IDLE)
    {
        switch (msg)
        {
            case MSG_CTP_TOUCH:
                break;

            case MSG_CTP_CLICK:
                func_breathe_button_click();
                break;

            case MSG_CTP_SHORT_UP:
            case MSG_CTP_SHORT_DOWN:
            case MSG_CTP_SHORT_LEFT:
            case MSG_CTP_LONG:
                if (func_cb.flag_sort)
                {
                    func_message(msg);
                }
                break;

            case MSG_CTP_SHORT_RIGHT:
                func_message(msg);
                break;

            case MSG_QDEC_FORWARD:
            case MSG_QDEC_BACKWARD:
                break;

            default:
                func_message(msg);
                break;
        }
    }
    else
    {
        switch (msg)
        {
            case KU_BACK:
                f_breathe->time_past = 0;
                f_breathe->animation_sta = ANIMATION_FINISH;
                break;

            case MSG_CTP_SHORT_RIGHT:  //右滑不退出
                break;

            default:
                func_message(msg);
                break;
        }
    }
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//进入呼吸功能
static void func_breathe_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_breathe_t));
    func_cb.frm_main = func_breathe_form_create();

    f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    compo_picturebox_t *pic_breathe = compo_getobj_byid(COMPO_ID_PIC_BREATHE);
    f_breathe->icon_org_size = compo_picturebox_get_location(pic_breathe).wid;

    sys_cb.breathe_mode = MIN(sys_cb.breathe_mode, BREATHE_MODE_FAST);
    sys_cb.breathe_duration = MAX(BREATHE_TIME_MIN, MIN(BREATHE_TIME_MAX, sys_cb.breathe_duration));
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartStartSingleTesting(TYPE_HEART);  //开启心率
#endif
}

//退出呼吸功能
static void func_breathe_exit(void)
{
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartStopSingleTesting(TYPE_HEART);  //关闭心率
#endif
    func_cb.last = FUNC_BREATHE;
}

//呼吸功能
void func_breathe(void)
{
    printf("%s\n", __func__);
    func_breathe_enter();
    while (func_cb.sta == FUNC_BREATHE)
    {
        func_breathe_process();
        func_breathe_message(msg_dequeue());
    }
    func_breathe_exit();
}

#endif
