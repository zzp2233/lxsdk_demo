#include "include.h"
#include "func.h"
#include "func_cover.h"

#if UTE_MODULE_SCREENS_VOICE_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//组件ID
enum
{
    //图像
    COMPO_ID_PIC_VOICE = 1,

    //TEXT
    COMPO_ID_TXT_VOICE,
};

typedef struct f_voice_t_
{
    bool siri_en;
    u8 siri_status;
    u32 voice_over_tick;
} f_voice_t;

enum
{
    FUNC_SIRI_STATUS_NONE = 0,
    FUNC_SIRI_STATUS_CLICK,
    FUNC_SIRI_STATUS_CONNBT,
    FUNC_SIRI_STATUS_SPEAKER,
};

typedef struct ui_handle_t_
{
    struct animation_t
    {
        u16 id;
        s16 x,y;
        u8 radix;
        u32 interval;
        u32 res;
    } animation;

    struct text_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;
        u32 res;
        bool center;
        u16 str_id1;
        u16 str_id2;
    } text;
} ui_handle_t;

static const ui_handle_t ui_handle =
{
    .animation = {
        .id = COMPO_ID_PIC_VOICE,
        .x  = 240/2,
        .y  = 228+ 28/2,
        .radix  = 16,
        .interval   = 5,
        .res    = UI_BUF_I330001_VOICE_GIF_BIN,
    },

    .text = {
        .id = COMPO_ID_TXT_VOICE,
        .x  = 16+208/2,
        .y  = 122+26/2,
        .w  = 208,
        .h  = 26,
        .res= UI_BUF_0FONT_FONT_BIN,
        .center = true,
        .str_id1 = STR_VOICE_SIRI_START,
        .str_id2 = STR_VOICE_SPEAKER,
    },
};

//创建语音助手窗体
compo_form_t *func_voice_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VOICE]);

    //创建动画
    compo_animation_t *animation = compo_animation_create(frm, ui_handle.animation.res);
    compo_animation_set_pos(animation, ui_handle.animation.x, ui_handle.animation.y);
    compo_animation_set_radix(animation, ui_handle.animation.radix);
//    compo_animation_set_interval(animation, ui_handle.animation.interval);
    compo_setid(animation, ui_handle.animation.id);
//    compo_animation_set_roll(animation, UI_BUF_I330001_VOICE_GIF_BIN);

    //创建TEXT
    compo_textbox_t* txt = compo_textbox_create(frm, MAX(strlen(i18n[ui_handle.text.str_id1]), strlen(i18n[ui_handle.text.str_id2])));
    compo_textbox_set_font(txt, ui_handle.text.res);
    compo_textbox_set_align_center(txt, ui_handle.text.center);
    compo_textbox_set_location(txt, ui_handle.text.x, ui_handle.text.y, ui_handle.text.w, ui_handle.text.h);
    compo_setid(txt, ui_handle.text.id);

    return frm;
}

//动画播放(开启语音助手)
static void func_voice_animation_playing(bool en)
{
    compo_animation_t *animation = compo_getobj_byid(ui_handle.animation.id);
    compo_animation_set_interval(animation, en*ui_handle.animation.interval);
}

static void func_voice_frist_check(void)
{
    f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    if (bt_is_connected() == false)
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
        msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
    }
}

static void func_voice_start_siri(void)
{
    f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    if (bt_is_connected())
    {
        bt_hfp_siri_switch();
        f_voice->siri_en = true;
        f_voice->voice_over_tick = tick_get();
    }
    else
    {
        f_voice->siri_en = false;
//        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
//        sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
//        msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        func_voice_frist_check();
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//组件ID
enum
{
    //图像
    COMPO_ID_PIC_VOICE = 1,

    //TEXT
    COMPO_ID_TXT_VOICE,
};

typedef struct f_voice_t_
{
    bool siri_en;
    u8 siri_status;
    u32 voice_over_tick;
} f_voice_t;

enum
{
    FUNC_SIRI_STATUS_NONE = 0,
    FUNC_SIRI_STATUS_CLICK,
    FUNC_SIRI_STATUS_CONNBT,
    FUNC_SIRI_STATUS_SPEAKER,
};

typedef struct ui_handle_t_
{
    struct animation_t
    {
        u16 id;
        s16 x,y;
        u8 radix;
        u32 interval;
        u32 res;
    } animation;

    struct text_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;
        u32 res;
        bool center;
        u16 str_id1;
        u16 str_id2;
    } text;
} ui_handle_t;

static const ui_handle_t ui_handle =
{
    .animation = {
        .id = COMPO_ID_PIC_VOICE,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 314,
        .radix  = 16,
        .interval   = 5,
        .res    = UI_BUF_I332001_VOICE_GIF_BIN,
    },

    .text = {
        .id = COMPO_ID_TXT_VOICE,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = GUI_SCREEN_CENTER_Y,
        .w  = 240,
        .h  = 34,
        .res= UI_BUF_0FONT_FONT_BIN,
        .center = true,
        .str_id1 = STR_VOICE_SIRI_START,
        .str_id2 = STR_VOICE_SPEAKER,
    },
};

//创建语音助手窗体
compo_form_t *func_voice_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_VOICE]);

    //创建动画
    compo_animation_t *animation = compo_animation_create(frm, ui_handle.animation.res);
    compo_animation_set_pos(animation, ui_handle.animation.x, ui_handle.animation.y);
    compo_animation_set_radix(animation, ui_handle.animation.radix);
    compo_animation_set_interval(animation, ui_handle.animation.interval);
    compo_setid(animation, ui_handle.animation.id);
    compo_animation_set_roll(animation, UI_BUF_I332001_VOICE_GIF_BIN);

    //创建TEXT
    compo_textbox_t* txt = compo_textbox_create(frm, MAX(strlen(i18n[ui_handle.text.str_id1]), strlen(i18n[ui_handle.text.str_id2])));
    compo_textbox_set_font(txt, ui_handle.text.res);
    compo_textbox_set_align_center(txt, ui_handle.text.center);
    compo_textbox_set_location(txt, ui_handle.text.x, ui_handle.text.y, ui_handle.text.w, ui_handle.text.h);
    compo_setid(txt, ui_handle.text.id);

    return frm;
}

//动画播放(开启语音助手)
static void func_voice_animation_playing(bool en)
{
    compo_animation_t *animation = compo_getobj_byid(ui_handle.animation.id);
    compo_animation_set_interval(animation, en*ui_handle.animation.interval);
}

static void func_voice_frist_check(void)
{
    f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    if (bt_is_connected() == false)
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
        msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
    }
}

static void func_voice_start_siri(void)
{
    f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    if (bt_is_connected())
    {
        bt_hfp_siri_switch();
        f_voice->siri_en = true;
        f_voice->voice_over_tick = tick_get();
    }
    else
    {
        f_voice->siri_en = false;
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
        msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        func_voice_frist_check();
    }
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//语音助手功能事件处理
static void func_voice_process(void)
{

    f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    static u8 siri_cnt = 0;
    static bool bt_connect_status = false;
    compo_textbox_t* txt = compo_getobj_byid(ui_handle.text.id);

    if (!bt_is_connected())
    {
        f_voice->siri_en = false;
    }
    else if (!bt_get_siri_status())
    {
        if (bt_is_ios_device())     //IOS设备可以正常获取siri的状态
        {
            if (f_voice->siri_en)
            {
                siri_cnt++;
                if (siri_cnt > 50)
                {
                    f_voice->siri_en = false;
                }
            }
        }
        else      //部分安卓设备无法获取得到siri的状态,先这样处理(5s后界面显示自动关闭)
        {
            if (f_voice->siri_en)
            {
                if (tick_check_expire(f_voice->voice_over_tick, 5000))
                {
                    f_voice->siri_en = false;
                }
            }
        }
    }
    else
    {
        f_voice->siri_en = true;
        siri_cnt = 0;
    }

    //BT变化弹窗
    if (bt_is_connected())
    {
        if (bt_connect_status == false)
        {
            printf("bt_connect_status true\n");
            bt_connect_status = true;
        }
    }
    else
    {
        if (bt_connect_status == true)
        {
            printf("bt_connect_status false\n");
            func_voice_frist_check();
            bt_connect_status = false;
        }
    }


//    printf("siri_en = %d\n",  f_voice->siri_en);
    if (f_voice->siri_en == true)
    {
        func_voice_animation_playing(true);
        if (f_voice->siri_status != FUNC_SIRI_STATUS_SPEAKER)
        {
            f_voice->siri_status = FUNC_SIRI_STATUS_SPEAKER;
            compo_textbox_set(txt, i18n[ui_handle.text.str_id2]);
        }
    }
    else
    {

        if (!bt_is_connected())
        {
            if (f_voice->siri_status != FUNC_SIRI_STATUS_CONNBT)
            {
                func_voice_animation_playing(false);
                f_voice->siri_status = FUNC_SIRI_STATUS_CONNBT;
//                compo_textbox_set(txt, i18n[STR_VOICE_BT_NOT_CONNECT]);
                compo_textbox_set(txt, i18n[ui_handle.text.str_id1]);
            }
        }
        else
        {
            if (f_voice->siri_status != FUNC_SIRI_STATUS_CLICK)
            {
                func_voice_animation_playing(false);
                f_voice->siri_status = FUNC_SIRI_STATUS_CLICK;
                compo_textbox_set(txt, i18n[ui_handle.text.str_id1]);
            }
        }
    }

    func_process();
}

//语音助手功能消息处理
static void func_voice_message(size_msg_t msg)
{
    //f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_voice_start_siri();
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

//进入语音助手功能
static void func_voice_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_voice_t));
    func_cb.frm_main = func_voice_form_create();
#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    f_voice->siri_en = false;
    func_voice_frist_check();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

}

//退出语音助手功能
static void func_voice_exit(void)
{
    func_cb.last = FUNC_VOICE;
}

//语音助手功能
void func_voice(void)
{
    printf("%s\n", __func__);
    func_voice_enter();
    while (func_cb.sta == FUNC_VOICE)
    {
        func_voice_process();
        func_voice_message(msg_dequeue());
    }
    func_voice_exit();
}
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
