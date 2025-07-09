//#include "include.h"
//#include "func.h"
//
//#if TRACE_EN
//#define TRACE(...)              printf(__VA_ARGS__)
//#else
//#define TRACE(...)
//#endif
//
////开始按钮的坐标信息
//#define START_X                           (GUI_SCREEN_CENTER_X)
//#define START_Y                           (GUI_SCREEN_HEIGHT - 25)
//#define START_WID                         200
//#define START_HEI                         75
//
//
//
//typedef struct f_tetris_start_t_ {
//
//} f_tetris_start_t;
//
////创建海拔窗体
//compo_form_t *func_tetris_start_form_create(void)
//{
//    //新建窗体和背景
//    compo_form_t *frm = compo_form_create(true);
//    widget_image_t * image;
//    compo_cardbox_t *cardbox;
//    char str_buff[20];
//
//    image = widget_image_create(frm->page_body, UI_BUF_TETRIS__S_BJ_START_BIN);
//    widget_set_location(image, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT-5);
//    widget_set_visible(image, true);
//
//    //最高纪录
//    cardbox = compo_cardbox_create(frm, 0, 2, 4, 290, 150);
//    sprintf(str_buff,"最高纪录 %ld分",sys_cb.max_score);
//    compo_cardbox_text_set(cardbox, 0, str_buff);
//    compo_cardbox_text_set_location(cardbox, 0, 0, 0, 290, 35);
//    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+70, 160, 85);
//
//
//    return frm;
//}
//
////海拔功能事件处理
//static void func_tetris_start_process(void)
//{
//    func_process();
//}
//
////海拔功能消息处理
//static void func_tetris_start_message(size_msg_t msg)
//{
//    point_t pt;
//    pt = ctp_get_sxy();
//    switch (msg) {
//    case MSG_CTP_CLICK:
//        printf("x[%d,%d], y[%d,%d]", abs_s(pt.x - START_X)*2, START_WID, abs_s(pt.y - START_Y) * 2, START_HEI);
//        if (abs_s(pt.x - START_X) * 2 <= START_WID && abs_s(pt.y - START_Y) * 2 <= START_HEI)
//        {
//            func_switch_to(FUNC_TETRIS, FUNC_SWITCH_DIRECT);
//        }
//
//        break;
//
//    case MSG_CTP_SHORT_UP:
//        break;
//
//    case MSG_CTP_SHORT_DOWN:
//        break;
//
//    case MSG_CTP_LONG:
//        break;
//
//    default:
//        func_message(msg);
//        break;
//    }
//}
//
////进入海拔功能
//static void func_tetris_start_enter(void)
//{
//    func_cb.f_cb = func_zalloc(sizeof(f_tetris_start_t));
//    func_cb.frm_main = func_tetris_start_form_create();
//}
//
////退出海拔功能
//static void func_tetris_start_exit(void)
//{
//    func_cb.last = FUNC_TETRIS_START;
//}
//
////海拔功能
//void func_tetris_start(void)
//{
//    printf("%s\n", __func__);
//    func_tetris_start_enter();
//    while (func_cb.sta == FUNC_TETRIS_START) {
//        func_tetris_start_process();
//        func_tetris_start_message(msg_dequeue());
//    }
//    func_tetris_start_exit();
//}
