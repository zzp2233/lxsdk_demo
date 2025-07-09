#include "include.h"
#include "func.h"
#include "func_calculator_alg.h"

#define TRACE_EN    1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{
    COMPO_ID_NUM_DISP = 128,
};

typedef struct f_calculator_t_
{

} f_calculator_t;

#define CALCULATOR_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_calculator_disp_btn_item) / sizeof(tbl_calculator_disp_btn_item[0])))
#define CALCULATOR_DISP_NUMBER_MAX      32                 //显示最大位数

typedef struct calculator_disp_btn_item_t_
{
    u32 res_addr;
    u32 res_addr_dwon;
    u16 btn_id;
    s16 x;
    s16 y;
    u16 num_btn_old;
    u16 hold_btn_old;
} calculator_disp_btn_item_t;

//按钮item，创建时遍历一下
static const calculator_disp_btn_item_t tbl_calculator_disp_btn_item[] =
{
    {UI_BUF_I330001_CALCULATOR_DEFAULT_00_BIN,       UI_BUF_I330001_CALCULATOR_STATE_00_BIN,        BTN_0,              91,    257},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_01_BIN,       UI_BUF_I330001_CALCULATOR_STATE_01_BIN,        BTN_1,              32,     213},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_02_BIN,       UI_BUF_I330001_CALCULATOR_STATE_02_BIN,        BTN_2,              91,    213},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_03_BIN,       UI_BUF_I330001_CALCULATOR_STATE_03_BIN,        BTN_3,              149,    213},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_04_BIN,       UI_BUF_I330001_CALCULATOR_STATE_04_BIN,        BTN_4,              32,     169},//
    {UI_BUF_I330001_CALCULATOR_DEFAULT_05_BIN,       UI_BUF_I330001_CALCULATOR_STATE_05_BIN,        BTN_5,              91,    169},//
    {UI_BUF_I330001_CALCULATOR_DEFAULT_06_BIN,       UI_BUF_I330001_CALCULATOR_STATE_06_BIN,        BTN_6,              149,    169},//
    {UI_BUF_I330001_CALCULATOR_DEFAULT_07_BIN,       UI_BUF_I330001_CALCULATOR_STATE_07_BIN,        BTN_7,              32,     125},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_08_BIN,       UI_BUF_I330001_CALCULATOR_STATE_08_BIN,        BTN_8,              91,    125},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_09_BIN,       UI_BUF_I330001_CALCULATOR_STATE_09_BIN,        BTN_9,              149,    125},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_DIAN_BIN,     UI_BUF_I330001_CALCULATOR_STATE_DIAN_BIN,      BTN_POINT,          149,    257},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_JIAJIAN_BIN,  UI_BUF_I330001_CALCULATOR_STATE_JIAJIAN_BIN,   BTN_OPPOSITE,       32,     257},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_CE_BIN,       UI_BUF_I330001_CALCULATOR_STATE_CE_BIN,        BTN_CE,             32,     81},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_AC_BIN,       UI_BUF_I330001_CALCULATOR_STATE_AC_BIN,        BTN_C,              91,    81},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_DELETE_BIN,   UI_BUF_I330001_CALCULATOR_STATE_DELETE_BIN,    BTN_DEL,            149,    81},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_CHU_BIN,      UI_BUF_I330001_CALCULATOR_STATE_CHU_BIN,       BTN_DIV,            208,    81},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_CHENG_BIN,    UI_BUF_I330001_CALCULATOR_STATE_CHENG_BIN,     BTN_MUL,            208,    125},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_JIAN_BIN,     UI_BUF_I330001_CALCULATOR_STATE_JIAN_BIN,      BTN_SUB,            208,    213},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_JIA_BIN,      UI_BUF_I330001_CALCULATOR_STATE_JIA_BIN,       BTN_ADD,            208,    169},//
    {UI_BUF_I330001_CALCULATOR_DEFAULT_DENG_BIN,     UI_BUF_I330001_CALCULATOR_STATE_DENG_BIN,      BTN_EQUAL,          208,    257},
};


//创建计算器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_calculator_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < CALCULATOR_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_calculator_disp_btn_item[idx_btn].res_addr);
        compo_button_set_pos(btn, tbl_calculator_disp_btn_item[idx_btn].x, tbl_calculator_disp_btn_item[idx_btn].y-6);
        compo_setid(btn,tbl_calculator_disp_btn_item[idx_btn].btn_id);
    }

    //创建文本
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, CALCULATOR_DISP_NUMBER_MAX);
    compo_setid(txt, COMPO_ID_NUM_DISP);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
//    compo_textbox_set_autosize(txt, true);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 208-widget_text_get_area(txt->txt).wid,14);

    return frm;
}

//按钮触摸效果
static void func_calculator_button_Refresh_disp(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;

//    printf("num_btn_old = %d\n", calculator_disp_btn_item->num_btn_old);
    if(calculator_disp_btn_item->num_btn_old)
    {
        compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
    }

}

//按钮触摸效果
static void func_calculator_button_press_handle(void)
{
//    printf("%s\n", __func__);
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    if(id && id!=BTN_ADD && id!=BTN_SUB && id!=BTN_MUL && id!=BTN_DIV)
    {
        compo_button_t *btn = NULL;
        if(calculator_disp_btn_item->num_btn_old != id && calculator_disp_btn_item->num_btn_old)
        {
            btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->num_btn_old = id;
        btn = compo_getobj_byid(id);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[id-1].res_addr_dwon);
    }

}

//符号按钮释放效果
static void func_calculator_button_release_handle(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    u16 hold_id = gcal_get_holding_operator();
    component_t *compo = (component_t *)compo_pool_get_top();

//    printf("btn:%d  btn:%d\n",calculator_disp_btn_item->hold_btn_old,hold_id );
    if(calculator_disp_btn_item->hold_btn_old!=hold_id)
    {
        if(calculator_disp_btn_item->hold_btn_old != 0)
        {
            compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->hold_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->hold_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->hold_btn_old = hold_id;
    }

    while (compo != NULL)
    {
        if (compo->type == COMPO_TYPE_BUTTON && compo->id == hold_id)
        {
            compo_button_t *btn = (compo_button_t *)compo;
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[compo->id-1].res_addr_dwon);
        }
        compo = compo_get_next(compo);
    }
}

//单击按钮
static void func_calculator_button_click_handler(void)
{
    int id = compo_get_button_id();
    gcalc_btn_click_handler(id);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_NUM_DISP);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 208-widget_text_get_area(txt->txt).wid,14);
    func_calculator_button_release_handle();
    func_calculator_button_Refresh_disp();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum
{
    COMPO_ID_NUM_DISP = 128,
};

typedef struct f_calculator_t_
{

} f_calculator_t;

#define CALCULATOR_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_calculator_disp_btn_item) / sizeof(tbl_calculator_disp_btn_item[0])))
#define CALCULATOR_DISP_NUMBER_MAX      32                 //显示最大位数

typedef struct calculator_disp_btn_item_t_
{
    u32 res_addr;
    u32 res_addr_dwon;
    u16 btn_id;
    s16 x;
    s16 y;
    u16 num_btn_old;
    u16 hold_btn_old;
} calculator_disp_btn_item_t;

//按钮item，创建时遍历一下
static const calculator_disp_btn_item_t tbl_calculator_disp_btn_item[] =
{
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_00_BIN,       UI_BUF_I330001_CALCULATOR_STATE_00_BIN,        BTN_0,              91,    257},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_01_BIN,       UI_BUF_I330001_CALCULATOR_STATE_01_BIN,        BTN_1,              32,     213},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_02_BIN,       UI_BUF_I330001_CALCULATOR_STATE_02_BIN,        BTN_2,              91,    213},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_03_BIN,       UI_BUF_I330001_CALCULATOR_STATE_03_BIN,        BTN_3,              149,    213},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_04_BIN,       UI_BUF_I330001_CALCULATOR_STATE_04_BIN,        BTN_4,              32,     169},//
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_05_BIN,       UI_BUF_I330001_CALCULATOR_STATE_05_BIN,        BTN_5,              91,    169},//
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_06_BIN,       UI_BUF_I330001_CALCULATOR_STATE_06_BIN,        BTN_6,              149,    169},//
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_07_BIN,       UI_BUF_I330001_CALCULATOR_STATE_07_BIN,        BTN_7,              32,     125},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_08_BIN,       UI_BUF_I330001_CALCULATOR_STATE_08_BIN,        BTN_8,              91,    125},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_09_BIN,       UI_BUF_I330001_CALCULATOR_STATE_09_BIN,        BTN_9,              149,    125},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_DIAN_BIN,     UI_BUF_I330001_CALCULATOR_STATE_DIAN_BIN,      BTN_POINT,          149,    257},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_JIAJIAN_BIN,  UI_BUF_I330001_CALCULATOR_STATE_JIAJIAN_BIN,   BTN_OPPOSITE,       32,     257},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_CE_BIN,       UI_BUF_I330001_CALCULATOR_STATE_CE_BIN,        BTN_CE,             32,     81},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_AC_BIN,       UI_BUF_I330001_CALCULATOR_STATE_AC_BIN,        BTN_C,              91,    81},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_DELETE_BIN,   UI_BUF_I330001_CALCULATOR_STATE_DELETE_BIN,    BTN_DEL,            149,    81},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_CHU_BIN,      UI_BUF_I330001_CALCULATOR_STATE_CHU_BIN,       BTN_DIV,            208,    81},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_CHENG_BIN,    UI_BUF_I330001_CALCULATOR_STATE_CHENG_BIN,     BTN_MUL,            208,    125},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_JIAN_BIN,     UI_BUF_I330001_CALCULATOR_STATE_JIAN_BIN,      BTN_SUB,            208,    213},
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_JIA_BIN,      UI_BUF_I330001_CALCULATOR_STATE_JIA_BIN,       BTN_ADD,            208,    169},//
//    {UI_BUF_I330001_CALCULATOR_DEFAULT_DENG_BIN,     UI_BUF_I330001_CALCULATOR_STATE_DENG_BIN,      BTN_EQUAL,          208,    257},
};

//创建计算器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_calculator_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < CALCULATOR_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_calculator_disp_btn_item[idx_btn].res_addr);
        compo_button_set_pos(btn, tbl_calculator_disp_btn_item[idx_btn].x, tbl_calculator_disp_btn_item[idx_btn].y-6);
        compo_setid(btn,tbl_calculator_disp_btn_item[idx_btn].btn_id);
    }

    //创建文本
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, CALCULATOR_DISP_NUMBER_MAX);
    compo_setid(txt, COMPO_ID_NUM_DISP);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
//    compo_textbox_set_autosize(txt, true);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 208-widget_text_get_area(txt->txt).wid,14);

    return frm;
}

//按钮触摸效果
static void func_calculator_button_Refresh_disp(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;

//    printf("num_btn_old = %d\n", calculator_disp_btn_item->num_btn_old);
    if(calculator_disp_btn_item->num_btn_old)
    {
        compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
    }

}

//按钮触摸效果
static void func_calculator_button_press_handle(void)
{
//    printf("%s\n", __func__);
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    if(id && id!=BTN_ADD && id!=BTN_SUB && id!=BTN_MUL && id!=BTN_DIV)
    {
        compo_button_t *btn = NULL;
        if(calculator_disp_btn_item->num_btn_old != id && calculator_disp_btn_item->num_btn_old)
        {
            btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->num_btn_old = id;
        btn = compo_getobj_byid(id);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[id-1].res_addr_dwon);
    }

}

//符号按钮释放效果
static void func_calculator_button_release_handle(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    u16 hold_id = gcal_get_holding_operator();
    component_t *compo = (component_t *)compo_pool_get_top();

//    printf("btn:%d  btn:%d\n",calculator_disp_btn_item->hold_btn_old,hold_id );
    if(calculator_disp_btn_item->hold_btn_old!=hold_id)
    {
        if(calculator_disp_btn_item->hold_btn_old != 0)
        {
            compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->hold_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->hold_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->hold_btn_old = hold_id;
    }

    while (compo != NULL)
    {
        if (compo->type == COMPO_TYPE_BUTTON && compo->id == hold_id)
        {
            compo_button_t *btn = (compo_button_t *)compo;
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[compo->id-1].res_addr_dwon);
        }
        compo = compo_get_next(compo);
    }
}

//单击按钮
static void func_calculator_button_click_handler(void)
{
    int id = compo_get_button_id();
    gcalc_btn_click_handler(id);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_NUM_DISP);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 208-widget_text_get_area(txt->txt).wid,14);
    func_calculator_button_release_handle();
    func_calculator_button_Refresh_disp();
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//计算器功能事件处理
static void func_calculator_process(void)
{
    // if (sys_cb.gui_sleep_sta)
    // {
    //     sys_cb.gui_need_wakeup = 1;
    // }
    // reset_sleep_delay_all();
    func_process();
}

//计算器功能消息处理
static void func_calculator_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_calculator_button_press_handle();
            break;
//        case MSG_SYS_500MS:
//            func_calculator_button_Refresh_disp();
//            break;
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
//            printf("MSG_CTP_SHORT_LEFT\n");
            func_calculator_button_Refresh_disp();
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_calculator_button_Refresh_disp();
            func_message(msg);
            break;

        case MSG_CTP_CLICK:
            func_calculator_button_click_handler();
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入计算器功能
static void func_calculator_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_calculator_t));
    func_cb.frm_main = func_calculator_form_create();

    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    calculator_disp_btn_item->num_btn_old = 0;
    calculator_disp_btn_item->hold_btn_old = 0;
    if (gcal_cb_init() == false)
    {
        halt(HALT_MALLOC);
    }
    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = 1;
    }
    reset_sleep_delay_all();
}

//退出计算器功能
static void func_calculator_exit(void)
{
    gcal_cb_destroy();
}

//计算器功能
void func_calculator(void)
{
    printf("%s\n", __func__);
    func_calculator_enter();
    while (func_cb.sta == FUNC_CALCULATOR)
    {
        func_calculator_process();
        func_calculator_message(msg_dequeue());
    }
    func_calculator_exit();
}
#endif
