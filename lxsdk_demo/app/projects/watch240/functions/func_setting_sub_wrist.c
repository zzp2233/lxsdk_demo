#include "include.h"
#include "func.h"
#include "ute_module_sport.h"
#include "ute_application_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if GUI_MODULE_WRIST_SUPPORT
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
    UI_BUF_I330001_PUBLIC_SWITCH02_BIN,         //ON
    UI_BUF_I330001_PUBLIC_SWITCH00_BIN,         //OFF
};

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
#endif
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    //创建卡片
    compo_cardbox_t * card = compo_cardbox_create(frm, 1, 1, 1, 232, 72);
    compo_cardbox_set_visible(card, true);
    compo_cardbox_set_location(card, 4 + 232/2, 54 + 72/2, 232, 72);
    compo_setid(card, COMPO_CARD_1);

    compo_cardbox_rect_set_color(card, 0, make_color(41,41,41));
    compo_cardbox_rect_set_location(card, 0, 0, 0, 232, 72, 16);

    compo_cardbox_icon_set_location(card, 0,
                                    232/2 - 5 - gui_image_get_size(tbl_wrist_switch_res[0]).wid/2,
                                    0, gui_image_get_size(tbl_wrist_switch_res[0]).wid, gui_image_get_size(tbl_wrist_switch_res[0]).hei);

#if UTE_MODULE_SPORT_SUPPORT
    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
#endif
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }

    compo_cardbox_text_set_font(card, 0, UI_BUF_0FONT_FONT_BIN);
    compo_cardbox_text_set_location(card, 0, 10-232/2, -14,
                                    232 - gui_image_get_size(tbl_wrist_switch_res[0]).wid - 50,
                                    30);
    compo_cardbox_text_set_align_center(card, 0, false);
    widget_text_set_color(card->text[0], make_color(255,255,255));
    compo_cardbox_text_set(card, 0, i18n[STR_SETTING_UP]);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
//    UI_BUF_I330001_PUBLIC_SWITCH02_BIN,         //ON
//    UI_BUF_I330001_PUBLIC_SWITCH00_BIN,         //OFF
};

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    //创建卡片
    compo_cardbox_t * card = compo_cardbox_create(frm, 1, 1, 1, 232, 72);
    compo_cardbox_set_visible(card, true);
    compo_cardbox_set_location(card, 4 + 232/2, 54 + 72/2, 232, 72);
    compo_setid(card, COMPO_CARD_1);

    compo_cardbox_rect_set_color(card, 0, make_color(41,41,41));
    compo_cardbox_rect_set_location(card, 0, 0, 0, 232, 72, 16);

    compo_cardbox_icon_set_location(card, 0,
                                    232/2 - 5 - gui_image_get_size(tbl_wrist_switch_res[0]).wid/2,
                                    0, gui_image_get_size(tbl_wrist_switch_res[0]).wid, gui_image_get_size(tbl_wrist_switch_res[0]).hei);

    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }

    compo_cardbox_text_set_font(card, 0, UI_BUF_0FONT_FONT_BIN);
    compo_cardbox_text_set_location(card, 0, 10-232/2, -14,
                                    232 - gui_image_get_size(tbl_wrist_switch_res[0]).wid - 50,
                                    30);
    compo_cardbox_text_set_align_center(card, 0, false);
    widget_text_set_color(card->text[0], make_color(255,255,255));
    compo_cardbox_text_set(card, 0, i18n[STR_SETTING_UP]);

    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//抬腕亮屏事件处理
static void func_set_sub_wrist_process(void)
{

    compo_cardbox_t * card = compo_getobj_byid(COMPO_CARD_1);
#if UTE_MODULE_SPORT_SUPPORT
    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }
#endif

    for(u8 i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)      //文本滚动
    {
        u16 id = COMPO_CARD_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }

    func_process();
}

//获取点击卡片的id
static u16 func_wrist_card_get_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t* cardbox = NULL;
    for(i = 0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
        id = COMPO_CARD_START + 1 + i;
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
static void func_wrist_button_click(void)
{
    u8 ret = 0;
    f_wrist_t *wrs = (f_wrist_t *)func_cb.f_cb;

    point_t pt = ctp_get_sxy();
    u16 compo_id = func_wrist_card_get_id(pt);
    if (compo_id <= 0 || compo_id > COMPO_CARD_END - 1)
    {
        return;
    }
//    printf("click compo_id:%d\n", compo_id);

    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == COMPO_CARD_1)
        {
//            ret = msgbox((char *)i18n[STR_SETTING_UP], NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);

//            if (ret == MSGBOX_RES_OK)
//            {
//                if (wrs->value) {
//                    wrs->value = false;
//                    compo_cardbox_icon_set(cardbox, 0, tbl_wrist_switch_res[1]);
//                } else {
//                    wrs->value = true;
//                    compo_cardbox_icon_set(cardbox, 0, tbl_wrist_switch_res[0]);
//                }
            switch_set_sub_wrist();
//            }
#if UTE_MODULE_SPORT_SUPPORT
            if(uteModuleSportGetIsOpenHandScreenOn())
            {
                compo_cardbox_icon_set(cardbox, 0, tbl_wrist_switch_res[0]);
            }
            else
#endif
            {
                compo_cardbox_icon_set(cardbox, 0, tbl_wrist_switch_res[1]);
            }

        }
    }

}

//抬腕亮屏功能消息处理
static void func_set_sub_wrist_message(size_msg_t msg)
{
    switch (msg)
    {

        case MSG_CTP_CLICK:
            func_wrist_button_click();
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入抬腕亮屏功能
static void func_set_sub_wrist_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_wrist_t));
    func_cb.frm_main = func_set_sub_wrist_form_create();

    //初始化变量
    f_wrist_t *wrs = (f_wrist_t *)func_cb.f_cb;
#if UTE_MODULE_SPORT_SUPPORT
    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        wrs->value = true;
    }
    else
#endif
    {
        wrs->value = false;
    }
}

//退出抬腕亮屏功能
static void func_set_sub_wrist_exit(void)
{
    func_cb.last = FUNC_SET_SUB_WRIST;
}

//抬腕亮屏功能
void func_set_sub_wrist(void)
{
    printf("%s\n", __func__);
    func_set_sub_wrist_enter();
    while (func_cb.sta == FUNC_SET_SUB_WRIST)
    {
        func_set_sub_wrist_process();
        func_set_sub_wrist_message(msg_dequeue());
    }
    func_set_sub_wrist_exit();
}
#endif
