#include "include.h"
#include "func.h"
#include "ute_module_factorytest.h"
#include "ute_module_newFactoryTest.h"
#include "ute_application_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define SIDEBAR_PAGE_HEIGHT GUI_SCREEN_HEIGHT    ///长图总高度
#define SPACING_Y  GUI_SCREEN_HEIGHT/5

enum
{
    MODE_TITLE_ID = 1,
    MODE_FACTORY_TESTING_ID,
    MODE_AGING_TESTING_ID,
    // MODE_SHIPPING_ID,
    // MODE_AUDIO_ID,
};

typedef struct f_test_mode_t_
{
    page_tp_move_t *ptm;
} f_test_mode_t;


///创建测试窗体
compo_form_t *func_test_mode_form_create(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox;
    compo_button_t *btn;


    textbox = compo_textbox_create(frm, strlen("模式选择"));
    compo_textbox_set(textbox, "模式选择");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/6+SPACING_Y*(1-1));

    textbox = compo_textbox_create(frm, strlen("工厂测试"));
    compo_textbox_set(textbox, "工厂测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/6+SPACING_Y*(2-1));
    compo_textbox_set_forecolor(textbox,COLOR_GREEN);

    btn = compo_button_create(frm);///透明按钮
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/6+SPACING_Y*(2-1), widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,MODE_FACTORY_TESTING_ID);

    textbox = compo_textbox_create(frm, strlen("老化测试"));
    compo_textbox_set(textbox, "老化测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/6+SPACING_Y*(3-1));
    compo_textbox_set_forecolor(textbox,COLOR_GREEN);

    btn = compo_button_create(frm);///透明按钮
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/6+SPACING_Y*(3-1), widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,MODE_AGING_TESTING_ID);


    return frm;
}

///测试功能事件处理
static void func_test_mode_process(void)
{
    reset_guioff_delay();
    f_test_mode_t *f_test_mode = (f_test_mode_t *)func_cb.f_cb;
    compo_page_move_process(f_test_mode->ptm);
    func_process();
}
static void func_test_mode_click(void)
{
    int id = compo_get_button_id();
    u8 ret = 0;
    if (id == 0)
    {
        return;
    }

    switch(id)
    {
        case MODE_FACTORY_TESTING_ID:///工厂测试
            ret = msgbox("工厂测试", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
            if(ret == MSGBOX_RES_OK)
            {

                uteTaskGuiStartScreen(FUNC_FACTORY_TESTING,0,__func__);
                // func_switch_to(FUNC_TEST_MODE_LIST, 0);///跳转工厂测试界面
            }
            break;
        case MODE_AGING_TESTING_ID:///老化测试
            ret = msgbox("老化测试", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);

            if(ret == MSGBOX_RES_OK)
            {
                // uteModuleFactoryTestStartTestItem(TEST_ITEM_AGING,true);
                // func_cb.sta = FUNC_ONLINE_FACTORY_TEST;
                uteTaskGuiStartScreen(FUNC_AGEING,0,__func__);
                // func_switch_to(FUNC_AGEING, 0);///跳转老化测试界面
            }
            break;
//        case MODE_SHIPPING_ID:///船运测试
//            snprintf(buf_txt,BUF_TXT_LEN,"%s%s?",i18n[STR_DO_WANT_IN],i18n[f_test_mode_list[MODE_SHIPPING_ID-1].txt_num]);
//            ret = msgbox(buf_txt, NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
//            if(ret == MSGBOX_RES_OK)
//            {
////                func_switch_to(FUNC_PRESSURE_EXPLAIN, 0);//跳转压力说明界面
//            }
//            break;
//        case MODE_AUDIO_ID:///音频测试
//            printf("%s2\n", __func__);
//            snprintf(buf_txt,BUF_TXT_LEN,"%s%s?",i18n[STR_DO_WANT_IN],i18n[f_test_mode_list[MODE_AUDIO_ID-1].txt_num]);
//            ret = msgbox(buf_txt, NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
//
//            if(ret == MSGBOX_RES_OK)
//            {
//                func_switch_to(FUNC_AUDIO, 0);//跳转压力说明界面
//            }
//            break;
        default:
            break;
    }

    printf("%s3\n", __func__);
}

///测试功能消息处理
static void func_test_mode_message(size_msg_t msg)
{
    f_test_mode_t *f_test_mode= (f_test_mode_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_test_mode->ptm);
            break;
        case MSG_CTP_CLICK:
            func_test_mode_click();
            break;
        case KL_BACK:
        {
            uint8_t ret = msgbox("退出当前测试？", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
            if (ret == MSGBOX_RES_OK)
            {
                uteApplicationCommonRestart();
            }
        }
        break;
        default:
            evt_message(msg);
            break;
    }
}

///进入测试功能
static void func_test_mode_enter(void)
{
    printf("%s\n", __func__);
    func_cb.f_cb = func_zalloc(sizeof(f_test_mode_t));
    func_cb.frm_main = func_test_mode_form_create();
    f_test_mode_t *f_test_mode = (f_test_mode_t *)func_cb.f_cb;

    f_test_mode->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .page_size = SIDEBAR_PAGE_HEIGHT,
        .page_count = 1,
        .up_over_perc = 0,
        .down_over_perc = 0,
    };
    compo_page_move_init(f_test_mode->ptm, func_cb.frm_main->page_body, &info);
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
}

///退出测试功能
static void func_test_mode_exit(void)
{
    f_test_mode_t *f_test_mode = (f_test_mode_t *)func_cb.f_cb;
    if (f_test_mode->ptm)
    {
        func_free(f_test_mode->ptm);
    }
    func_cb.last = FUNC_TEST_MODE;
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
}

///测试功能
void func_test_mode(void)
{
    printf("%s\n", __func__);
    func_test_mode_enter();
    while (func_cb.sta == FUNC_TEST_MODE)
    {
        func_test_mode_process();
        func_test_mode_message(msg_dequeue());
    }
    func_test_mode_exit();
}
