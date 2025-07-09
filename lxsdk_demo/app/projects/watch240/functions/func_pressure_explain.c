//#include "include.h"
//#include "func.h"
//
//#if TRACE_EN
//#define TRACE(...)              printf(__VA_ARGS__)
//#else
//#define TRACE(...)
//#endif
//
//#define SIDEBAR_PAGE_HEIGHT 550    //长图总高度
//
//typedef struct f_pressure_explain_t_
//{
//    page_tp_move_t *ptm;
//
//} f_pressure_explain_t;
//
//compo_form_t *func_pressure_explain_form_create(void)
//{
//    char txt_buf[30];
//    compo_textbox_t *textbox;
//    compo_form_t *frm = compo_form_create(true);
//
//    textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE]));
//    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_BIN);
//    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/5);
//    compo_textbox_set(textbox,i18n[STR_PRESSURE]);
//
//    textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE_EXPLAIN]));
//    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_BIN);
//    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3,GUI_SCREEN_WIDTH*7/8,GUI_SCREEN_HEIGHT);
//    compo_textbox_set_multiline(textbox, true);
//    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN]);
//
//    snprintf(txt_buf, sizeof(txt_buf),"%s: 81-100",i18n[STR_SEVERE]);
//    textbox = compo_textbox_create(frm, strlen(txt_buf));
//    //compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_BIN);
//    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT*8/24);
//    compo_textbox_set(textbox,txt_buf);
//
//    snprintf(txt_buf,sizeof(txt_buf),"%s:  51-80",i18n[STR_MODERATE]);
//    textbox = compo_textbox_create(frm, strlen(txt_buf));
//    //compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_BIN);
//    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT*11/24);
//    compo_textbox_set(textbox,txt_buf);
//
//    snprintf(txt_buf,sizeof(txt_buf),"%s:  25-50",i18n[STR_LIGHT]);
//    textbox = compo_textbox_create(frm, strlen(txt_buf));
//    //compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_BIN);
//    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT*14/24);
//    compo_textbox_set(textbox,txt_buf);
//
//    snprintf(txt_buf,sizeof(txt_buf),"%s:   1-25",i18n[STR_RELAX]);
//    textbox = compo_textbox_create(frm, strlen(txt_buf));
//    //compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_BIN);
//    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT*17/24);
//    compo_textbox_set(textbox,txt_buf);
//
//    return frm;
//}
//
//
////时钟表盘右滑菜单功能消息处理
//static void func_pressure_explain_message(size_msg_t msg)
//{
//    f_pressure_explain_t *f_pressure_explain= (f_pressure_explain_t*)func_cb.f_cb;
//
//    switch (msg)
//    {
//        case MSG_CTP_TOUCH:
//            compo_page_move_touch_handler(f_pressure_explain->ptm);
//            break;
//
//        default:
//            func_message(msg);
//            break;
//    }
//}
////下滑事件流程处理
//static void func_pressure_explain_process(void)
//{
//    f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
//
//    compo_page_move_process(f_pressure_explain->ptm);
//    func_process();
//}
//
////创建界面
//static void func_pressure_enter(void)
//{
//    func_cb.f_cb = func_zalloc(sizeof(f_pressure_explain_t));
//    func_cb.frm_main = func_pressure_explain_form_create();
//    f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
//
//    f_pressure_explain->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
//    page_move_info_t info =
//    {
//        .page_size = SIDEBAR_PAGE_HEIGHT,
//        .page_count = 1,
//        .up_over_perc = 5,
//        .down_over_perc = 5,
//    };
//    compo_page_move_init(f_pressure_explain->ptm, func_cb.frm_main->page_body, &info);
//
//}
////退出任务
//static void func_pressure_explain_exit(void)
//{
//    f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
//    func_cb.last = FUNC_PRESSURE_EXPLAIN;
//    if (f_pressure_explain->ptm)
//    {
//        func_free(f_pressure_explain->ptm);
//    }
//}
//
////压力功能
//void func_pressure_explain(void)
//{
//    printf("%s\n", __func__);
//    func_pressure_enter();
//    while (func_cb.sta == FUNC_PRESSURE_EXPLAIN)
//    {
//        func_pressure_explain_process();
//        func_pressure_explain_message(msg_dequeue());
//    }
//    func_pressure_explain_exit();
//}
//
//
