#include "include.h"
#include "func.h"
#include "ute_module_call.h"
#include "ute_module_localRingtone.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级

typedef struct f_sav_t_
{
    u8 vol_value;
    u8 shk_value;
} f_sav_t;

enum
{
    COMPO_ID_SAV = 1,
    //  COMPO_ID_MUTE,
};

#define  ON_PIC     UI_BUF_I330001_PUBLIC_SWITCH02_BIN
#define  OFF_PIC    UI_BUF_I330001_PUBLIC_SWITCH00_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VOL]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_SAV);
    compo_cardbox_rect_set_location(cardbox,0,0,0,232,62,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+15, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

//     cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
//     compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2+62+6,GUI_SCREEN_WIDTH,62);
//     compo_setid(cardbox, COMPO_ID_MUTE);
//     compo_cardbox_rect_set_location(cardbox,0,0,0,232,62,16);
//     compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
//     compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+15, -11, 145, 30);
//    // compo_cardbox_text_set(cardbox,0,i18n[STR_MUTE]);
//     compo_cardbox_text_set_align_center(cardbox, 0, false);
//     compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
//     compo_cardbox_icon_set(cardbox,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
//     compo_cardbox_text_scroll_process(cardbox, true);

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
// compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set_location(cardbox_sav, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
//   compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级

typedef struct f_sav_t_
{
    u8 vol_value;
    u8 shk_value;
} f_sav_t;

enum
{
    COMPO_ID_SAV = 1,
    COMPO_ID_MUTE,
};

#define  ON_PIC     0
#define  OFF_PIC    0

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VOL]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_SAV);
    compo_cardbox_rect_set_location(cardbox,0,0,0,232,62,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+15, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2+62+6,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_MUTE);
    compo_cardbox_rect_set_location(cardbox,0,0,0,232,62,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+15, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox,0,uteModuleLocalRingtoneGetMuteStatus() ? OFF_PIC : ON_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
    compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set_location(cardbox_sav, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus() ? OFF_PIC : ON_PIC);
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//声音与振动事件处理
static void func_set_sub_sav_process(void)
{
    func_set_sub_sav_disp();
    func_process();
}

//获取点击的卡片组件id
static u16 func_setting_sav_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<COMPO_ID_SAV; i++)
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;
        id = COMPO_ID_SAV+i;
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
//单击按钮
static void func_sav_button_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_setting_sav_card_get_btn_id(pt);
    // if (compo_id <= 0 || compo_id > COMPO_ID_MUTE)
    // {
    //     return;
    // }

    switch(compo_id)
    {
        case COMPO_ID_SAV:
            uteModuleCallChangeEntertranmentVoiceSwitchStatus();
            uteModuleCallEntertranmentVoiceSwitchSaveConfig();
            break;
//         case COMPO_ID_MUTE:
// //            compo_cardbox_icon_set_location(cardbox_mute, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
// //            compo_cardbox_icon_set(cardbox_mute,0,ON_PIC);
//             if(uteModuleLocalRingtoneGetMuteStatus())
//             {
//                 uteModuleLocalRingtoneSetMuteStatus(false,true);
//             }
//             else
//             {
//                 uteModuleLocalRingtoneSetMuteStatus(true,true);
//             }
//             break;
        default:
            break;
    }
    func_set_sub_sav_disp();
}

//声音与振动功能消息处理
static void func_set_sub_sav_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sav_button_click();
            break;

        default:
            func_message(msg);
            break;
    }

}

//进入声音与振动功能
static void func_set_sub_sav_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sav_t));
    func_cb.frm_main = func_set_sub_sav_form_create();
}

//退出声音与振动功能
static void func_set_sub_sav_exit(void)
{
    func_cb.last = FUNC_SET_SUB_SAV;
}

//声音与振动功能
void func_set_sub_sav(void)
{
    printf("%s\n", __func__);
    func_set_sub_sav_enter();
    while (func_cb.sta == FUNC_SET_SUB_SAV)
    {
        func_set_sub_sav_process();
        func_set_sub_sav_message(msg_dequeue());
    }
    func_set_sub_sav_exit();
}
