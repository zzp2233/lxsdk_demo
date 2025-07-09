#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "ute_language_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define LANGUAGE_LIST_CNT                       ((int)(sizeof(tbl_power_on_language_list) / sizeof(tbl_power_on_language_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_power_on_language_list_t_
{
    compo_listbox_t *listbox;

} f_power_on_language_list_t;

static const compo_listbox_item_t tbl_power_on_language_list[] =
{
#if SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT
    {STR_LANGUAGE_CN,   .vidx = CHINESE_LANGUAGE_ID},
#endif
#if SCREEN_TITLE_MULTIPLE_ENGLISH_LANGUAGE_SUPPORT
    {STR_LANGUAGE_ENG,  .vidx = ENGLISH_LANGUAGE_ID},
#endif
#if SCREEN_TITLE_MULTIPLE_SPANISH_LANGUAGE_SUPPORT
    {STR_XBYY,   .vidx = SPANISH_LANGUAGE_ID},
#endif
#if SCREEN_TITLE_MULTIPLE_FRENCH_LANGUAGE_SUPPORT
    {STR_LANGUAGE_FN,   .vidx = FRENCH_LANGUAGE_ID},
#endif
#if SCREEN_TITLE_MULTIPLE_RUSSIAN_LANGUAGE_SUPPORT
    {STR_LANGUAGE_RU,   .vidx = RUSSIAN_LANGUAGE_ID},
#endif

#if SCREEN_TITLE_MULTIPLE_ITALIAN_LANGUAGE_SUPPORT
    {STR_ITALY,   .vidx = ITALIAN_LANGUAGE_ID},//意大利
#endif
#if SCREEN_TITLE_MULTIPLE_GERMAN_LANGUAGE_SUPPORT
    {STR_GERMAN,   .vidx = GERMAN_LANGUAGE_ID},//德文
#endif
#if SCREEN_TITLE_MULTIPLE_ARABIC_LANGUAGE_SUPPORT
    {STR_ARABIC,   .vidx = ARABIC_LANGUAGE_ID},//阿拉伯文
#endif
#if SCREEN_TITLE_MULTIPLE_POLISH_LANGUAGE_SUPPORT
    {STR_BLY,  .vidx = POLISH_LANGUAGE_ID},
#endif
#if SCREEN_TITLE_MULTIPLE_INDONESUAN_LANGUAGE_SUPPORT
    {STR_YDNXY,    .vidx = INDONESUAN_LANGUAGE_ID},
#endif
#if SCREEN_TITLE_MULTIPLE_CZECH_LANGUAGE_SUPPORT
    {STR_JK,    .vidx = CZECH_LANGUAGE_ID},
#endif
#if SCREEN_TITLE_MULTIPLE_VIETNAMESE_LANGUAGE_SUPPORT
    {STR_YN,    .vidx = VIETNAMESE_LANGUAGE_ID},
#endif
#if SCREEN_TITLE_MULTIPLE_TURKISH_LANGUAGE_SUPPORT
    {STR_TURKEY,    .vidx = TURKISH_LANGUAGE_ID},
#endif
#if SCREEN_TITLE_MULTIPLE_PORTUGUESE_LANGUAGE_SUPPORT
    {STR_PORTUGUESE,    .vidx = PORTUGUESE_LANGUAGE_ID},
#endif
};
extern u8 func_sel_language_bit(uint n);

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//语言设置页面
compo_form_t *func_power_on_language_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LANGUAGE]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_power_on_language_list, LANGUAGE_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);

    compo_listbox_set_sta_icon(listbox, UI_BUF_I330001_PUBLIC_GOU_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_language_bit);

    compo_setid(listbox, COMPO_ID_LISTBOX);
    uint8_t current_language = uteModuleSystemtimeReadLanguage();
    uint8_t set_idx = 1; // 默认值
    for (int i = 0; i < LANGUAGE_LIST_CNT; i++)
    {
        if (tbl_power_on_language_list[i].vidx == current_language)
        {
            set_idx = i;
            break;
        }
    }

    // 确保set_idx有效
    if (set_idx >= LANGUAGE_LIST_CNT)
    {
        set_idx = 1; // 如果未找到，使用默认值
    }

    // 计算使当前选中项居中的索引（考虑边界情况）
    int center_idx = LANGUAGE_LIST_CNT / 2;
    int offset = center_idx - set_idx;

    // 调整索引，使选中项居中（避免超出边界）
    if (offset > 0 && set_idx < 2)
    {
        // 当前选中项在前两个，不调整
        offset = 0;
    }
    else if (offset < 0 && set_idx > (LANGUAGE_LIST_CNT - 3))
    {
        // 当前选中项在后两个，不调整
        offset = 0;
    }

    // 应用偏移量
    set_idx += offset;

    // 确保调整后的索引有效
    if (set_idx < 1)
    {
        set_idx = 1;
    }
    else if (set_idx >= LANGUAGE_LIST_CNT)
    {
        set_idx = LANGUAGE_LIST_CNT - 1;
    }
    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);


    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//语言设置页面
compo_form_t *func_power_on_language_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LANGUAGE]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_power_on_language_list, LANGUAGE_LIST_CNT);
//    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);

//    compo_listbox_set_sta_icon(listbox, UI_BUF_I330001_PUBLIC_GOU_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_language_bit);

    compo_setid(listbox, COMPO_ID_LISTBOX);
    uint8_t set_idx = 1;
    if (set_idx < 1)
    {
        set_idx = 1;
    }
    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);


    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//点进图标进入应用
void func_power_on_language_list_icon_click(void)
{
    int icon_idx;
    f_power_on_language_list_t *f_set = (f_power_on_language_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());

    if (icon_idx < 0 || icon_idx >= LANGUAGE_LIST_CNT)
    {
        return;
    }

    // func_switch_to(FUNC_POWER_ON_SCAN, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
    func_switch_to_clock();
    compo_listbox_update(listbox);
    uteModuleSystemtimeSetLanguage(tbl_power_on_language_list[icon_idx].vidx);
}

//语言设置功能事件处理
static void func_power_on_language_list_process(void)
{
    f_power_on_language_list_t *f_set = (f_power_on_language_list_t *)func_cb.f_cb;
    compo_listbox_move(f_set->listbox);
    func_process();
}

//语言设置功能消息处理
static void func_power_on_language_list_message(size_msg_t msg)
{
    f_power_on_language_list_t *f_set = (f_power_on_language_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }


    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_power_on_language_list_icon_click();                //单击图标
            break;
        case KL_BACK:
            //    func_switch_to(FUNC_LONG_PRESS, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
            break;
    }

}

//进入语言设置功能
static void func_power_on_language_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_power_on_language_list_t));
    func_cb.frm_main = func_power_on_language_form_create();

    f_power_on_language_list_t *f_set = (f_power_on_language_list_t *)func_cb.f_cb;
    f_set->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    // compo_listbox_move_init(listbox);

    compo_listbox_move_init(listbox);
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, LANGUAGE_LIST_CNT - 2));
    //func_cb.enter_tick = tick_get();

}

//退出语言设置功能
static void func_power_on_language_exit(void)
{
    f_power_on_language_list_t *f_set = (f_power_on_language_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    func_free(listbox->mcb);                                            //释放移动控制块
    func_cb.last = FUNC_POWER_ON_LANGUAGE;
}

//语言设置功能
void func_power_on_language(void)
{
    printf("%s\n", __func__);
    func_power_on_language_enter();
    while (func_cb.sta == FUNC_POWER_ON_LANGUAGE)
    {
        func_power_on_language_list_process();
        func_power_on_language_list_message(msg_dequeue());
    }
    func_power_on_language_exit();
}
