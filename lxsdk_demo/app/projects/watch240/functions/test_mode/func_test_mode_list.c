#include "include.h"
#include "func.h"
#include "ute_module_factorytest.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_test_mode_list_t_
{
    compo_listbox_t *listbox;
} f_test_mode_list_t;



static const compo_listbox_item_t online_factory_test_list[] =
{
    // {     .vidx = TEST_ITEM_AGING},
    {     .vidx = TEST_ITEM_BATTERY},
    {     .vidx = TEST_ITEM_SCREEN_RGB},
    {     .vidx = TEST_ITEM_VCXX_CHECK_LIGHT},
    {     .vidx = TEST_ITEM_TOUCH},
    {     .vidx = TEST_ITEM_MIC_SPEAKER},
};
#define SET_LIST_CNT                       ((int)(sizeof(online_factory_test_list) / sizeof(online_factory_test_list[0])))
static const compo_listbox_custom_item_t online_factory_test_list_txt[SET_LIST_CNT]= {/*"老化模式",*/"电量测试","RGB测试","漏光测试","九宫格测试","咪头喇叭测试"};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
// 工厂测试选项
compo_form_t *func_test_mode_list_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, online_factory_test_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_text_modify(listbox, online_factory_test_list_txt);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
    compo_listbox_update(listbox);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
// 工厂测试选项
compo_form_t *func_test_mode_list_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, online_factory_test_list, SET_LIST_CNT);
//    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_text_modify(listbox, online_factory_test_list_txt);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
    compo_listbox_update(listbox);

    return frm;
}
#else
compo_form_t *func_test_mode_list_form_create(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

///测试功能事件处理
static void func_test_mode_list_process(void)
{
    f_test_mode_list_t *f_test_mode_list = (f_test_mode_list_t *)func_cb.f_cb;
    compo_listbox_move(f_test_mode_list->listbox);
    func_process();
}
static void func_test_mode_list_click(void)
{
    f_test_mode_list_t *f_test_mode_list = (f_test_mode_list_t *)func_cb.f_cb;
    int icon_idx = compo_listbox_select(f_test_mode_list->listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }
    uteModuleFactoryTestStartTestItem(online_factory_test_list[icon_idx].vidx,true);
    func_cb.sta = FUNC_ONLINE_FACTORY_TEST;
}

///测试功能消息处理
static void func_test_mode_list_message(size_msg_t msg)
{
    f_test_mode_list_t *f_test_mode_list = (f_test_mode_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_test_mode_list->listbox;
    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_test_mode_list_click();
            break;
        case KL_BACK:
        {
            uint8_t ret = msgbox("退出当前测试？", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
            if (ret == MSGBOX_RES_OK)
            {
                func_cb.sta = FUNC_TEST_MODE;
            }
        }
        default:
            evt_message(msg);
            break;
    }
}

///进入测试功能
static void func_test_mode_list_enter(void)
{
    printf("%s\n", __func__);
    func_cb.f_cb = func_zalloc(sizeof(f_test_mode_list_t));
    func_cb.frm_main = func_test_mode_list_form_create();

    f_test_mode_list_t *f_test_mode_list = (f_test_mode_list_t *)func_cb.f_cb;
    f_test_mode_list->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_test_mode_list->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init(listbox);
}

///退出测试功能
static void func_test_mode_list_exit(void)
{
    printf("%s\n", __func__);
    f_test_mode_list_t *f_test_mode_list = (f_test_mode_list_t *)func_cb.f_cb;;
    compo_listbox_t *listbox = f_test_mode_list->listbox;
    func_free(listbox->mcb);
    func_cb.last = FUNC_TEST_MODE_LIST;
    // uteModuleGuiCommonDisplayOffAllowGoBack(true);
}

///测试功能
void func_test_mode_list(void)
{
    printf("%s\n", __func__);
    func_test_mode_list_enter();
    while (func_cb.sta == FUNC_TEST_MODE_LIST)
    {
        func_test_mode_list_process();
        func_test_mode_list_message(msg_dequeue());
    }
    func_test_mode_list_exit();
}
