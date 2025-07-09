//#include "include.h"
//#include "func.h"
//
//#if TRACE_EN
//#define TRACE(...)              printf(__VA_ARGS__)
//#else
//#define TRACE(...)
//#endif
//
//enum
//{
//
//    COMPO_ID_BTN_NUM0 = 1,
//    COMPO_ID_BTN_NUM1,
//    COMPO_ID_BTN_NUM2,
//    COMPO_ID_BTN_NUM3,
//    COMPO_ID_BTN_NUM4,
//    COMPO_ID_BTN_NUM5,
//    COMPO_ID_BTN_NUM6,
//    COMPO_ID_BTN_NUM7,
//    COMPO_ID_BTN_NUM8,
//    COMPO_ID_BTN_NUM9,
//    COMPO_ID_BTN_SURE,
//    COMPO_ID_BTN_BACK,
//
//    COMPO_ID_TXT_NUM,
//    COMPO_ID_TXT_SHAPE_BOX,
//    COMPO_ID_TXT_SHAPE_BG,
//    SHAPE_ID_KEYBOARD_BG,
//    SHAPE_ID_KEYBOARD_BOX,
//    COMPO_ID_INPUT_BOX,
//    COMPO_ID_OPEN_BTN,
//    COMPO_ID_TXT_CURSOR,
//
//};
//
//#define CALL_DISP_BTN_ITEM_CNT    ((int)(sizeof(keyboard) / sizeof(keyboard[0])))
//
//typedef struct sos_call_disp_btn_item_t_
//{
//    u32 res_addr;
//    u16 btn_id;
//    s16 x;
//    s16 y;
//} sos_call_disp_btn_item_t;
//
/////创建键盘，创建时遍历一下
//static const sos_call_disp_btn_item_t keyboard[] =
//{
//    {UI_BUF_CALCULATOR_1_CLICK_BIN,             COMPO_ID_BTN_NUM1,              GUI_SCREEN_WIDTH/3.94,    GUI_SCREEN_HEIGHT/2.368},
//    {UI_BUF_CALCULATOR_2_CLICK_BIN,             COMPO_ID_BTN_NUM2,              GUI_SCREEN_WIDTH/1.984,   GUI_SCREEN_HEIGHT/2.368},
//    {UI_BUF_CALCULATOR_3_CLICK_BIN,             COMPO_ID_BTN_NUM3,              GUI_SCREEN_WIDTH/1.34,    GUI_SCREEN_HEIGHT/2.368},
//    {UI_BUF_CALCULATOR_4_CLICK_BIN,             COMPO_ID_BTN_NUM4,              GUI_SCREEN_WIDTH/3.94,    GUI_SCREEN_HEIGHT/1.76},
//    {UI_BUF_CALCULATOR_5_CLICK_BIN,             COMPO_ID_BTN_NUM5,              GUI_SCREEN_WIDTH/1.984,   GUI_SCREEN_HEIGHT/1.76},
//    {UI_BUF_CALCULATOR_6_CLICK_BIN,             COMPO_ID_BTN_NUM6,              GUI_SCREEN_WIDTH/1.34,    GUI_SCREEN_HEIGHT/1.76},
//    {UI_BUF_CALCULATOR_7_CLICK_BIN,             COMPO_ID_BTN_NUM7,              GUI_SCREEN_WIDTH/3.94,    GUI_SCREEN_HEIGHT/1.39},
//    {UI_BUF_CALCULATOR_8_CLICK_BIN,             COMPO_ID_BTN_NUM8,              GUI_SCREEN_WIDTH/1.984,   GUI_SCREEN_HEIGHT/1.39},
//    {UI_BUF_CALCULATOR_9_CLICK_BIN,             COMPO_ID_BTN_NUM9,              GUI_SCREEN_WIDTH/1.34,    GUI_SCREEN_HEIGHT/1.39},
//    {UI_BUF_CALCULATOR_0_CLICK_BIN,             COMPO_ID_BTN_NUM0,              GUI_SCREEN_WIDTH/1.984,   GUI_SCREEN_HEIGHT/1.152},
//    {UI_BUF_CALCULATOR_DEL_CLICK_BIN,           COMPO_ID_BTN_BACK,              GUI_SCREEN_WIDTH/1.34,    GUI_SCREEN_HEIGHT/1.152},
//    {UI_BUF_CALCULATOR_C_CLICK_BIN,             COMPO_ID_BTN_SURE,              GUI_SCREEN_WIDTH/3.94,    GUI_SCREEN_HEIGHT/1.152},
//};
//
//typedef struct f_sos_t_
//{
//    uint32_t tick;
//    u8 phone_number_cnt;
//    bool input_mode_flag;
//    u8 cursor_disp_time;
//} f_sos_t;
//
//
/////创建键盘
//static void key_board_create(compo_form_t *frm)
//{
//    compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, keyboard[7].x, keyboard[3].y+((keyboard[6].y-keyboard[3].y)/2), GUI_SCREEN_WIDTH/1.2, GUI_SCREEN_HEIGHT/1.7+6);
//    compo_shape_set_color(shape, COLOR_WHITE);
//    compo_shape_set_alpha(shape, 180);
//    compo_setid(shape,SHAPE_ID_KEYBOARD_BOX);
//    compo_shape_set_visible(shape, false );
//
//    shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, keyboard[7].x, keyboard[3].y+((keyboard[6].y-keyboard[3].y)/2),  GUI_SCREEN_WIDTH/1.2-6, GUI_SCREEN_HEIGHT/1.7);
//    compo_shape_set_color(shape, COLOR_BLACK);
//    compo_shape_set_alpha(shape, 180);
//    compo_setid(shape,SHAPE_ID_KEYBOARD_BG);
//    compo_shape_set_visible(shape, false );
//
//    ///创建图片按钮
//    compo_button_t *btn;
//    for (u8 idx_btn = 0; idx_btn < CALL_DISP_BTN_ITEM_CNT; idx_btn++)
//    {
//        btn = compo_button_create_by_image(frm, keyboard[idx_btn].res_addr);
//        compo_setid(btn, keyboard[idx_btn].btn_id);
//        compo_button_set_pos(btn, keyboard[idx_btn].x, keyboard[idx_btn].y);
//        compo_button_set_visible(btn, false );
//    }
//}
//
/////键盘弹出函数
//static void key_board_visible(bool open_flag)
//{
//    compo_shape_t *shape_box = compo_getobj_byid(SHAPE_ID_KEYBOARD_BOX);
//    compo_shape_t *shape_bg = compo_getobj_byid(SHAPE_ID_KEYBOARD_BG);
//    compo_button_t *btn = NULL;
//
//    for (u8 idx_btn = 0; idx_btn < CALL_DISP_BTN_ITEM_CNT; idx_btn++)
//    {
//        btn = compo_getobj_byid(COMPO_ID_BTN_NUM0+idx_btn);
//        compo_button_set_visible(btn, open_flag );
//    }
//    compo_shape_set_visible(shape_box, open_flag );
//    compo_shape_set_visible(shape_bg, open_flag );
//}
//
/////输入模式
//static void input_mode(bool inpu_mode_flag)
//{
//    key_board_visible(inpu_mode_flag);
//    compo_shape_t *shape_bg     = compo_getobj_byid(COMPO_ID_TXT_SHAPE_BG);
//    compo_shape_t *shape_box    = compo_getobj_byid(COMPO_ID_TXT_SHAPE_BOX);
//    compo_textbox_t *txt_number = compo_getobj_byid(COMPO_ID_TXT_NUM);
//    compo_button_t *btn_input   = compo_getobj_byid(COMPO_ID_INPUT_BOX);
//    compo_button_t *img_btn     = compo_getobj_byid(COMPO_ID_OPEN_BTN);
//    compo_textbox_t *txt_cursor = compo_getobj_byid(COMPO_ID_TXT_CURSOR);///创建输入框光标
//
//    if(inpu_mode_flag == true)
//    {
//        compo_shape_set_location(shape_box, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6, GUI_SCREEN_HEIGHT/7+6);
//        compo_shape_set_location(shape_bg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6-6, GUI_SCREEN_HEIGHT/7.5);
//        compo_textbox_set_pos(txt_number, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2.5);
//        compo_textbox_set_font(txt_number, UI_BUF_0FONT_FONT_NUM_24_BIN);
//        compo_button_set_location(btn_input,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y/2.5,GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6-6,GUI_SCREEN_HEIGHT/7.5);
//        compo_button_set_location(img_btn, 0,0,0,0);
//        compo_button_set_visible(img_btn, false);
//        compo_textbox_set(txt_cursor,"2");
//        compo_textbox_set_pos(txt_cursor, GUI_SCREEN_CENTER_X+(widget_text_get_area(txt_cursor->txt).wid*strlen(sys_cb.sos_call_number))/2+widget_text_get_area(txt_cursor->txt).wid/3, GUI_SCREEN_CENTER_Y/2.5);
//        compo_textbox_set(txt_cursor,"I");
//        compo_textbox_set_visible(txt_cursor, true);
//        compo_textbox_set_visible(txt_number, true);
//    }
//    else
//    {
//        compo_shape_set_location(shape_box, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6, GUI_SCREEN_HEIGHT/7+6);
//        compo_shape_set_location(shape_bg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6-6, GUI_SCREEN_HEIGHT/7.5);
//        compo_textbox_set_pos(txt_number, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
//        compo_button_set_location(btn_input,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6-6,GUI_SCREEN_HEIGHT/7.5);
//        compo_button_set_location(img_btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/4 );
//        compo_button_set_visible(img_btn, true);
//        if(sys_cb.sos_call_number[0]=='\0' || sys_cb.sos_call_number[0]==0)
//        {
//            compo_textbox_set_font(txt_number, UI_BUF_0FONT_FONT_BIN);
//            compo_textbox_set(txt_number,i18n[STR_PLEASE_ENTER_SOS]);
//        }
//        compo_textbox_set_visible(txt_cursor, false);
//        compo_textbox_set_visible(txt_number, true);
//    }
//}
//
//
/////SOS页面
//compo_form_t *func_set_sub_sos_form_create(void)
//{
//    char number_buf[20];
//    ///新建窗体
//    compo_form_t *frm = compo_form_create(true);
//
//    compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);///创建输入框边角
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6, GUI_SCREEN_HEIGHT/7+6);
//    compo_setid(shape, COMPO_ID_TXT_SHAPE_BOX);
//
//    shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);///创建输入框背景
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6-6, GUI_SCREEN_HEIGHT/7.5);
//    compo_shape_set_color(shape, COLOR_BLACK);
//    compo_shape_set_alpha(shape, 180);
//    compo_setid(shape, COMPO_ID_TXT_SHAPE_BG);
//
//    compo_textbox_t *txt_number = compo_textbox_create(frm, 30);///创建输入框文本
//    if(sys_cb.sos_call_number[0]=='\0' || sys_cb.sos_call_number[0]==0)
//    {
//        compo_textbox_set(txt_number,i18n[STR_PLEASE_ENTER_SOS]);///请输入SOS电话号码
//    }
//    else
//    {
//        compo_textbox_set_font(txt_number, UI_BUF_0FONT_FONT_NUM_24_BIN);
//        snprintf(number_buf,sizeof(number_buf),"%s",sys_cb.sos_call_number);
//        compo_textbox_set(txt_number,number_buf);
//    }
//    compo_textbox_set_location(txt_number, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6-6, GUI_SCREEN_HEIGHT/3.5);
//    compo_textbox_set_alpha(txt_number, 100 );
//    compo_setid(txt_number, COMPO_ID_TXT_NUM);
//
//    compo_textbox_t *txt_cursor = compo_textbox_create(frm, 1);///创建输入框光标
//    compo_textbox_set_font(txt_cursor, UI_BUF_0FONT_FONT_NUM_24_BIN);
//    compo_textbox_set_alpha(txt_cursor, 100 );
//    compo_textbox_set(txt_cursor,"I");
//    compo_textbox_set_location(txt_cursor, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2.5, widget_text_get_area(txt_cursor->txt).wid, widget_text_get_area(txt_cursor->txt).hei);
//    compo_setid(txt_cursor, COMPO_ID_TXT_CURSOR);
//    compo_textbox_set_visible(txt_cursor, false);
////    printf("txt_wid:%d",widget_text_get_area(txt_cursor->txt).wid);
//
//    compo_button_t *btn = compo_button_create(frm);///创建输入框按钮
//    compo_button_set_location(btn,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y/2.5,GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6-6,GUI_SCREEN_HEIGHT/7.5);
//    compo_setid(btn, COMPO_ID_INPUT_BOX);
//
//    if(sys_cb.sos_open_flag == true)
//    {
//        compo_button_t *img_btn = compo_button_create_by_image(frm, UI_BUF_COMMON_ON_BIN );///创建SOS功能打开按钮
//        compo_button_set_location(img_btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/4 );
//        compo_setid(img_btn, COMPO_ID_OPEN_BTN);
//    }
//    else
//    {
//        compo_button_t *img_btn = compo_button_create_by_image(frm, UI_BUF_COMMON_OFF_BIN );///创建SOS功能打开按钮
//        compo_button_set_location(img_btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/4 );
//        compo_setid(img_btn, COMPO_ID_OPEN_BTN);
//    }
//
//    key_board_create(frm);///创建键盘
//
//    return frm;
//}
//
/////SOS功能事件处理
//static void func_set_sub_sos_process(void)
//{
//    f_sos_t *f_sos = (f_sos_t *)func_cb.f_cb;
//    compo_textbox_t *txt_cursor = compo_getobj_byid(COMPO_ID_TXT_CURSOR);
//    compo_textbox_t *txt_number = compo_getobj_byid(COMPO_ID_TXT_NUM);
//
//    if(tick_check_expire(f_sos->tick, 100))
//    {
//        f_sos->tick = tick_get();
//
//        if(f_sos->input_mode_flag == true)///输入模式显示光标
//        {
//            if(++f_sos->cursor_disp_time==10)f_sos->cursor_disp_time=0;
//
//            if(f_sos->cursor_disp_time<7)
//            {
//                compo_textbox_set(txt_cursor,"2");
//                compo_textbox_set_pos(txt_cursor, GUI_SCREEN_CENTER_X+(widget_text_get_area(txt_cursor->txt).wid*strlen(sys_cb.sos_call_number))/2+widget_text_get_area(txt_cursor->txt).wid/3, GUI_SCREEN_CENTER_Y/2.5);
//                compo_textbox_set(txt_cursor,"I");
//                if(strlen(sys_cb.sos_call_number)!=11)
//                {
//                    compo_textbox_set_visible(txt_cursor, true);
//                }
//                else
//                {
//                    compo_textbox_set_visible(txt_cursor, false);
//                }
//            }
//            else
//            {
//                compo_textbox_set_visible(txt_cursor, false);
//            }
//        }
//        else
//        {
//
//            if(sys_cb.sos_call_number[0]=='\0' || sys_cb.sos_call_number[0]==0)
//            {
//                if(++f_sos->cursor_disp_time==10)f_sos->cursor_disp_time=0;
//
//                if(f_sos->cursor_disp_time<7)
//                {
//                    compo_textbox_set_visible(txt_number, true);
//                }
//                else
//                {
//                    compo_textbox_set_visible(txt_number, false);
//                }
//            }
//        }
//
//    }
//
//    func_process();
//}
//
/////按键事件处理
//static void func_sos_button_click(void)
//{
//    f_sos_t *f_sos = (f_sos_t *)func_cb.f_cb;
//    int id = compo_get_button_id();
//    compo_textbox_t *txt_number = compo_getobj_byid(COMPO_ID_TXT_NUM);
//    compo_button_t *img_btn = compo_getobj_byid(COMPO_ID_OPEN_BTN);
//    char buf[30];
//
//    switch (id)
//    {
//        case COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM9:
//            if(f_sos->input_mode_flag == true)
//            {
//                if(f_sos->phone_number_cnt < 11)
//                {
//                    sys_cb.sos_call_number[f_sos->phone_number_cnt++] = id+0x30 - 1;
//                }
//            }
//            break;
//        case COMPO_ID_BTN_BACK:///删除
//            if(f_sos->phone_number_cnt > 0)
//            {
//                sys_cb.sos_call_number[--f_sos->phone_number_cnt] = '\0';
//            }
//            break;
//        case COMPO_ID_BTN_SURE:///确认
//            input_mode(false);
//            f_sos->input_mode_flag = false;
//            break;
//        case COMPO_ID_INPUT_BOX:///点击输入框
//            input_mode(true);
//            f_sos->input_mode_flag = true;
//            break;
//        case COMPO_ID_OPEN_BTN:
//            sys_cb.sos_open_flag ^=1;
//            if(sys_cb.sos_open_flag == true)compo_button_set_bgimg(img_btn, UI_BUF_COMMON_ON_BIN);
//            else compo_button_set_bgimg(img_btn, UI_BUF_COMMON_OFF_BIN);
//            compo_button_set_location(img_btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/4 );
//            break;
//        default:
//            break;
//    }
//
//    if(f_sos->input_mode_flag == true)
//    {
//        snprintf(buf, sizeof(buf), "%s", sys_cb.sos_call_number);
//        compo_textbox_set(txt_number, buf);
//    }
//
////    printf("number:%s  touch:%d  length:%d\n",sys_cb.sos_call_number, f_sos->input_mode_flag ,f_sos->phone_number_cnt);
//}
//
/////SOS功能消息处理
//static void func_set_sub_sos_message(size_msg_t msg)
//{
//    switch (msg)
//    {
//        case MSG_CTP_CLICK:
//            func_sos_button_click();                  //单击按钮
//            break;
//
//        case KU_DELAY_BACK:
//            break;
//
//        default:
//            func_message(msg);
//            break;
//    }
//}
//
/////进入SOS功能
//static void func_set_sub_sos_enter(void)
//{
//    func_cb.f_cb = func_zalloc(sizeof(f_sos_t));
//    f_sos_t *f_sos = (f_sos_t *)func_cb.f_cb;
//    func_cb.frm_main = func_set_sub_sos_form_create();
//    input_mode(false);
//    f_sos->phone_number_cnt = strlen(sys_cb.sos_call_number);
//}
//
/////退出SOS功能
//static void func_set_sub_sos_exit(void)
//{
//    func_cb.last = FUNC_SET_SUB_SOS;
//}
//
/////SOS功能
//void func_set_sub_sos(void)
//{
//    printf("%s\n", __func__);
//    func_set_sub_sos_enter();
//    while (func_cb.sta == FUNC_SET_SUB_SOS)
//    {
//        func_set_sub_sos_process();
//        func_set_sub_sos_message(msg_dequeue());
//    }
//    func_set_sub_sos_exit();
//}
