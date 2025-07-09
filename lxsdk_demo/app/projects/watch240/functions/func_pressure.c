//#include "include.h"
//#include "func.h"
//
//#if TRACE_EN
//#define TRACE(...)              printf(__VA_ARGS__)
//#else
//#define TRACE(...)
//#endif
//
//#define TODAY_PRESSURE_TXT_MAX   2   //压力文本最大字数
//#define CHART_NUM 28 //柱形图数量
//#define CHART_100_LENGTH GUI_SCREEN_CENTER_Y*6.25/8  //柱形图框高度
//#define CHART_100_VALUE 115 //柱形图对应底层图表100%数据
//
//
//enum
//{
//    COMPO_ID_TEXT_PRESSURE_MAX = 1,
//    COMPO_ID_TEXT_PRESSURE_MIN,
//    COMPO_ID_TEXT_MEAN,
//    COMPO_ID_IMG_BTNTON,
//
//    COMPO_ID_CHART,
//};
//
//
//typedef struct f_pressure_t_
//{
//    page_tp_move_t *ptm;
//    u16 data[CHART_NUM];
//} f_pressure_t;
//
//
//u16 chart_value[CHART_NUM];
//
////创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
//compo_form_t *func_pressure_form_create(void)
//{
//    for(int i=0; i<CHART_NUM; i++)chart_value[i] = 30;
//    chart_value[0]=115;
//    chart_value[3]=0;
//    chart_value[3+7]=0;
//    chart_value[3+7+7]=0;
//    chart_value[3+7+7+7]=0;
//
//    char  txt_buf[4];
//    //component_t *compo;
//    compo_textbox_t * txt;
//
//    //新建窗体和背景
//    compo_form_t *frm = compo_form_create(true);
//
//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_PRESSURE]);
//
//    //创建图表
//    compo_form_add_image(frm, UI_BUF_PRESSURE_CHART_BIN, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/8);//底层图片
//    compo_chartbox_t *chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);//图表内的柱形图
//    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/9, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/9-2, (GUI_SCREEN_WIDTH/40+1)*CHART_NUM,CHART_100_LENGTH);
//    compo_chartbox_set_pixel(chart, 1);
//    compo_setid(chart, COMPO_ID_CHART);
//
//
//    //平均压力
//    txt = compo_textbox_create(frm, TODAY_PRESSURE_TXT_MAX);
//    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
//    compo_setid(txt, COMPO_ID_TEXT_MEAN);
//    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/5);
//    snprintf(txt_buf, sizeof(txt_buf), "50");
//    compo_textbox_set(txt, txt_buf);
//
//    //最大压力
//    txt = compo_textbox_create(frm, TODAY_PRESSURE_TXT_MAX);
//    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_BIN);
//    compo_setid(txt, COMPO_ID_TEXT_PRESSURE_MAX);
//    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_HEIGHT-GUI_SCREEN_HEIGHT/12-2);
//    snprintf(txt_buf, sizeof(txt_buf), "50");
//    compo_textbox_set(txt, txt_buf);
//
//    //最小压力
//    txt = compo_textbox_create(frm, TODAY_PRESSURE_TXT_MAX);
//    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_BIN);
//    compo_setid(txt, COMPO_ID_TEXT_PRESSURE_MIN);
//    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_HEIGHT-GUI_SCREEN_HEIGHT/12-2);
//    snprintf(txt_buf, sizeof(txt_buf), "50");
//    compo_textbox_set(txt, txt_buf);
//
//    compo_form_add_image(frm, UI_BUF_HEART_RATE_UP_BIN, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X*2/6, GUI_SCREEN_HEIGHT-GUI_SCREEN_HEIGHT/12);//红色向上图标
//    compo_form_add_image(frm, UI_BUF_HEART_RATE_DOWN_BIN, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X*2/6,GUI_SCREEN_HEIGHT-GUI_SCREEN_HEIGHT/12);//绿色向下图标
//
//    //压力说明按键
//    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_PRESSURE_EXPLAIN_BIN);
//    compo_setid(btn, COMPO_ID_IMG_BTNTON);
//    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/4, GUI_SCREEN_HEIGHT/5);
//
//
//    return frm;
//}
//
////睡眠图表动画
//static void func_pressure_animation(void)
//{
//
//}
//
//
////压力功能消息处理
//static void func_pressure_process(void)
//{
//    ///f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
//
//    func_pressure_animation();
//    func_process();
//}
//
////刷新压力数据
//static void func_pressure_refresh(void)
//{
//    compo_chartbox_t *chart = compo_getobj_byid(COMPO_ID_CHART);
//    u8 max_num = compo_chartbox_get_max_num(chart);
//
//    chart_t chart_info;
//
//    chart_info.y = 0;
//    chart_info.width = GUI_SCREEN_WIDTH/40;   //像素点
//    for (int i=0; i<max_num; i++)
//    {
//        chart_info.x = i*chart_info.width + i;
//        // printf("%s->[%d,%d]\n",__func__, i, chart_info.x);
//        chart_info.height = chart_value[i];
//        compo_chartbox_set_value(chart, i, chart_info, COLOR_GRAY);
//    }
//
//}
////进入压力功能
//static void func_pressure_enter(void)
//{
//    func_cb.f_cb = func_zalloc(sizeof(f_pressure_t));
//    func_cb.frm_main = func_pressure_form_create();
//
//    //f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
//    func_pressure_refresh();
//}
//
////触摸单击按钮
//static void func_pressure_button_click(void)
//{
//    //f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
//
//    int id = compo_get_button_id();
//
//    switch (id)
//    {
//        case COMPO_ID_IMG_BTNTON:
//            func_switch_to(FUNC_PRESSURE_EXPLAIN, 0);//跳转压力说明界面
//            break;
//        default:
//            break;
//    }
//}
//
//static void func_pressure_message(size_msg_t msg)
//{
//    //f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
//
//    switch (msg)
//    {
//        case MSG_CTP_CLICK:
//            func_pressure_button_click();
//            break;
//
//        default:
//            func_message(msg);
//            break;
//    }
//}
////退出压力功能
//static void func_pressure_exit(void)
//{
//    func_cb.last = FUNC_PRESSURE;
//}
////压力功能
//void func_pressure(void)
//{
//    printf("%s\n", __func__);
//    func_pressure_enter();
//    while (func_cb.sta == FUNC_PRESSURE)
//    {
//        func_pressure_process();
//        func_pressure_message(msg_dequeue());
//    }
//    func_pressure_exit();
//}
//
//
