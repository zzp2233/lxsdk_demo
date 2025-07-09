//#include "include.h"
//#include "func.h"
//
//#if TRACE_EN
//#define TRACE(...)              printf(__VA_ARGS__)
//#else
//#define TRACE(...)
//#endif
//
////血糖检测状态
//enum {
//  BO_STA_IDLE,            //空闲
//  BO_STA_WORKING,         //检测中
//  BO_STA_DONE,            //完成
//  BO_STA_FAIL,            //失败
//};
//
////组件ID
//enum {
//    //按键
//  COMPO_ID_BTN_IDLE = 1,  //无触摸
//
//  //图像
//  COMPO_ID_PIC_CLICK,     //触摸
//
//  //文本框
//  COMPO_ID_TXT_IDLE,      //"测量"
//  COMPO_ID_TXT_WORKING,   //"测量中"
//  COMPO_ID_TXT_DONE,      //"完成"
//  COMPO_ID_TXT_FAIL,      //"重试"
//};
//
//typedef struct f_bloodsugar_t_ {
//  u8 det_sta;
//} f_bloodsugar_t;
//
//
////创建血糖窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
//compo_form_t *func_bloodsugar_form_create(void)
//{
//    //新建窗体和背景
//    compo_form_t *frm = compo_form_create(true);
////    compo_form_add_image(frm, UI_BUF_ICON_BLOODSUGAR_BIN, 120, 125);
//
//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_BLOOD_SUGAR]);
//
//  //新建按钮
//  compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BUTTON_BIN);
//    compo_setid(btn, COMPO_ID_BTN_IDLE);
//    compo_button_set_pos(btn, 120, 260);
//
//    //新建图像
//    compo_picturebox_t *pic_click = compo_picturebox_create(frm, UI_BUF_COMMON_BUTTON_CLICK_BIN);
//    compo_setid(pic_click, COMPO_ID_PIC_CLICK);
//    compo_picturebox_set_pos(pic_click, 120, 260);
//    compo_picturebox_set_visible(pic_click, false);          //触摸按下的效果图先设置不可见
//
//    //创建文本
//    compo_textbox_t *txt_idle = compo_textbox_create(frm, 2);
//    compo_setid(txt_idle, COMPO_ID_TXT_IDLE);
//    compo_textbox_set_pos(txt_idle, 120, 260);
//    compo_textbox_set(txt_idle, "测量");
//
//    compo_textbox_t *txt_working = compo_textbox_create(frm, 3);
//    compo_setid(txt_working, COMPO_ID_TXT_WORKING);
//    compo_textbox_set_pos(txt_working, 120, 260);
//    compo_textbox_set(txt_working, "测量中");
//    compo_textbox_set_visible(txt_working, false);
//
//    return frm;
//}
//
////触摸按钮效果处理
//static void func_bloodsugar_button_touch_handle(void)
//{
//    f_bloodsugar_t *f_bo = (f_bloodsugar_t *)func_cb.f_cb;
//    int id = compo_get_button_id();
//    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_CLICK);
//
//    printf("%s-->id:%x\n",__func__,id);
//    switch (id) {
//    case COMPO_ID_BTN_IDLE:
//        if (f_bo->det_sta == BO_STA_IDLE) {
//            compo_picturebox_set_visible(pic_click, true);       //触摸效果图设置可见
//        }
//        break;
//
//    default:
//        break;
//    }
//
//}
//
////释放按钮效果处理
//static void func_bloodsugar_button_release_handle(void)
//{
//    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_CLICK);
//    compo_picturebox_set_visible(pic_click, false);       //触摸效果图设置不可见
//}
//
////单击按钮
//static void func_bloodsugar_button_click(void)
//{
//    f_bloodsugar_t *f_bo = (f_bloodsugar_t *)func_cb.f_cb;
//    int id = compo_get_button_id();
//    compo_textbox_t *txt_idle     = compo_getobj_byid(COMPO_ID_TXT_IDLE);
//    compo_textbox_t *txt_working  = compo_getobj_byid(COMPO_ID_TXT_WORKING);
//
//    printf("%s-->id:%x\n",__func__,id);
//    switch (id) {
//    case COMPO_ID_BTN_IDLE:
//        if (f_bo->det_sta == BO_STA_IDLE) {
//            f_bo->det_sta = BO_STA_WORKING;
//
//            compo_textbox_set_visible(txt_idle, false);
//            compo_textbox_set_visible(txt_working, true);
//            func_bloodsugar_button_release_handle();      //触摸结束后，触摸效果图处理
//        }
//        break;
//
//    default:
//        break;
//    }
//}
//
//
////血糖功能事件处理
//static void func_bloodsugar_process(void)
//{
//    func_process();
//}
//
////血糖功能消息处理
//static void func_bloodsugar_message(size_msg_t msg)
//{
//    switch (msg) {
//    case MSG_CTP_TOUCH:
//        func_bloodsugar_button_touch_handle();
//        break;
//
//  case MSG_CTP_CLICK:
//        func_bloodsugar_button_click();
//        break;
//
//    case MSG_CTP_SHORT_UP:
//    case MSG_CTP_SHORT_DOWN:
//    case MSG_CTP_SHORT_LEFT:
//    case MSG_CTP_LONG:
//        func_bloodsugar_button_release_handle();
//        break;
//
//    case MSG_CTP_SHORT_RIGHT:
//        func_bloodsugar_button_release_handle();
//        func_message(msg);
//        break;
//
//    default:
//        func_message(msg);
//        break;
//    }
//}
//
////进入血糖功能
//static void func_bloodsugar_enter(void)
//{
//    func_cb.f_cb = func_zalloc(sizeof(f_bloodsugar_t));
//    func_cb.frm_main = func_bloodsugar_form_create();
//}
//
////退出血糖功能
//static void func_bloodsugar_exit(void)
//{
//
//    func_cb.last = FUNC_BLOODSUGAR;
//}
//
////血糖功能
//void func_bloodsugar(void)
//{
//    printf("%s\n", __func__);
//    func_bloodsugar_enter();
//    while (func_cb.sta == FUNC_BLOODSUGAR) {
//        func_bloodsugar_process();
//        func_bloodsugar_message(msg_dequeue());
//    }
//    func_bloodsugar_exit();
//}
