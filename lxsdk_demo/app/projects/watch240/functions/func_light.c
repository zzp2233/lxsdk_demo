#include "include.h"
#include "func.h"
#include "ute_module_gui_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_LIGHT_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_REDUCE = 1,
    COMPO_ID_BTN_INCREASE,

    //图像
    COMPO_ID_PIC_LEVEL1,            //亮度等级 1~5
    COMPO_ID_PIC_LEVEL2,
    COMPO_ID_PIC_LEVEL3,
    COMPO_ID_PIC_LEVEL4,
    COMPO_ID_PIC_LEVEL5,
    COMPO_ID_PIC_REDUCE_CLICK,      //按钮触摸效果图
    COMPO_ID_PIC_INCREASE_CLICK,
};

typedef struct f_light_t_
{
    int last_y_move;
    u8 light_level;
} f_light_t;

typedef struct light_pic_item_t_
{
    u32 res_addr;
    u16 pic_id;
    s16 x;
    s16 y;
} light_pic_item_t;

#define LIGHT_PIC_ITEM_CNT    ((int)(sizeof(tbl_light_pic_item) / sizeof(tbl_light_pic_item[0])))

//图片item，创建时遍历一下
static const light_pic_item_t tbl_light_pic_item[] =
{
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN,     COMPO_ID_PIC_LEVEL1,         120,     82   },
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL2,         120,     123    },
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL3,         120,     164    },
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL4,         120,     205    },
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_00_BIN,     COMPO_ID_PIC_LEVEL5,         120,     246    },
};

//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_SETTINGS_BRIGHTNESS_LESS_BIN);
    compo_setid(btn, COMPO_ID_BTN_REDUCE);
    compo_button_set_pos(btn, 39, 164);

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_SETTINGS_BRIGHTNESS_PLUS_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCREASE);
    compo_button_set_pos(btn, 201, 164);

    //新建图像
    compo_picturebox_t *pic_level[LIGHT_PIC_ITEM_CNT];
    for (u8 idx = 0; idx < LIGHT_PIC_ITEM_CNT; idx++)
    {
        pic_level[idx] = compo_picturebox_create(frm, tbl_light_pic_item[idx].res_addr);
        compo_setid(pic_level[idx], tbl_light_pic_item[idx].pic_id);
        compo_picturebox_set_pos(pic_level[idx], tbl_light_pic_item[idx].x, tbl_light_pic_item[idx].y);
    }
    printf("sys_cb.light_level=%d\n",sys_cb.light_level);
    return frm;
}

static void func_light_disp_Refresh(void)
{
    compo_picturebox_t *pic_level[5];
    pic_level[0] = compo_getobj_byid(COMPO_ID_PIC_LEVEL1);
    pic_level[1] = compo_getobj_byid(COMPO_ID_PIC_LEVEL2);
    pic_level[2] = compo_getobj_byid(COMPO_ID_PIC_LEVEL3);
    pic_level[3] = compo_getobj_byid(COMPO_ID_PIC_LEVEL4);
    pic_level[4] = compo_getobj_byid(COMPO_ID_PIC_LEVEL5);

    switch(sys_cb.light_level)
    {
        case 1:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
        case 2:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
        case 3:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
        case 4:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
        case 5:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
    }
}

//单击按钮
static void func_light_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_REDUCE:
            if (sys_cb.light_level > DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level <= DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level--;
            }
            break;

        case COMPO_ID_BTN_INCREASE:
            if (sys_cb.light_level >= DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level < DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level++;
            }
            break;

        default:
            break;
    }
    func_light_disp_Refresh();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//组件ID
enum
{
    //按键
    COMPO_ID_BTN_REDUCE = 1,
    COMPO_ID_BTN_INCREASE,

    //图像
    COMPO_ID_PIC_LEVEL1,            //亮度等级 1~5
    COMPO_ID_PIC_LEVEL2,
    COMPO_ID_PIC_LEVEL3,
    COMPO_ID_PIC_LEVEL4,
    COMPO_ID_PIC_LEVEL5,
    COMPO_ID_PIC_REDUCE_CLICK,      //按钮触摸效果图
    COMPO_ID_PIC_INCREASE_CLICK,
};

typedef struct f_light_t_
{
    int last_y_move;
    u8 light_level;
} f_light_t;

typedef struct light_pic_item_t_
{
    u32 res_addr;
    u16 pic_id;
    s16 x;
    s16 y;
} light_pic_item_t;

#define LIGHT_PIC_ITEM_CNT    ((int)(sizeof(tbl_light_pic_item) / sizeof(tbl_light_pic_item[0])))

//图片item，创建时遍历一下
static const light_pic_item_t tbl_light_pic_item[] =
{
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN,     COMPO_ID_PIC_LEVEL1,         GUI_SCREEN_CENTER_X,     88+48/2   },
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL2,         GUI_SCREEN_CENTER_X,     137+48/2     },
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL3,         GUI_SCREEN_CENTER_X,     186+48/2     },
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL4,         GUI_SCREEN_CENTER_X,     235+48/2     },
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN,     COMPO_ID_PIC_LEVEL5,         GUI_SCREEN_CENTER_X,     284+48/2     },
};
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_SETTINGS_BRIGHTNESS_LESS_BIN);
    compo_setid(btn, COMPO_ID_BTN_REDUCE);
    compo_button_set_pos(btn, 62/2+41,62/2+159);

    btn = compo_button_create_by_image(frm, UI_BUF_I332001_SETTINGS_BRIGHTNESS_PLUS_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCREASE);
    compo_button_set_pos(btn,  62/2+257,62/2+159);

    //新建图像
    compo_picturebox_t *pic_level[LIGHT_PIC_ITEM_CNT];
    for (u8 idx = 0; idx < LIGHT_PIC_ITEM_CNT; idx++)
    {
        pic_level[idx] = compo_picturebox_create(frm, tbl_light_pic_item[idx].res_addr);
        compo_setid(pic_level[idx], tbl_light_pic_item[idx].pic_id);
        compo_picturebox_set_pos(pic_level[idx], tbl_light_pic_item[idx].x, tbl_light_pic_item[idx].y);
    }
    return frm;
}

static void func_light_disp_Refresh(void)
{
    compo_picturebox_t *pic_level[5];
    pic_level[0] = compo_getobj_byid(COMPO_ID_PIC_LEVEL1);
    pic_level[1] = compo_getobj_byid(COMPO_ID_PIC_LEVEL2);
    pic_level[2] = compo_getobj_byid(COMPO_ID_PIC_LEVEL3);
    pic_level[3] = compo_getobj_byid(COMPO_ID_PIC_LEVEL4);
    pic_level[4] = compo_getobj_byid(COMPO_ID_PIC_LEVEL5);

    switch(sys_cb.light_level)
    {
        case 1:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 2:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 3:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 4:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 5:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_02_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
    }
}

//单击按钮
static void func_light_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_REDUCE:
            if (sys_cb.light_level > DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level <= DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level--;
            }
            break;

        case COMPO_ID_BTN_INCREASE:
            if (sys_cb.light_level >= DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level < DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level++;
            }
            break;

        default:
            break;
    }
    func_light_disp_Refresh();
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//亮度调节功能事件处理
static void func_light_process(void)
{
    func_process();
}

//亮度调节功能消息处理
static void func_light_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_light_button_click();
            uint8_t level_tmp = sys_cb.light_level;
            if(level_tmp < DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                level_tmp = DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
            }
            else if(level_tmp > DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                level_tmp = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
            }
            tft_bglight_set_level(level_tmp,false);
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_light_disp_Refresh();
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_light_disp_Refresh();
            func_message(msg);
            break;

        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入亮度调节功能
static void func_light_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_light_t));                                      // 动态申请亮度功能的数据结构内存（如last_y_move, light_level），保存至全局任务控制块func_cb中

    func_cb.frm_main = func_light_form_create();                                        // 创建亮度调节窗口UI（按钮、图标等），返回窗体指针存入func_cb.frm_main

    sys_cb.light_level = tft_cb.tft_bglight_duty / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE; // 初始化亮度等级（软件层的0~5级） = 当前PWM占空比 / 步进值

    func_light_disp_Refresh();                                                          // 刷新UI显示内容，通常会更新亮度图标显示状态
}


//退出亮度调节功能
static void func_light_exit(void)
{
    uteModuleGuiCommonSetBackLightPercent(tft_cb.tft_bglight_duty);
    func_cb.last = FUNC_LIGHT;
}

//亮度调节功能
void func_light(void)
{
    printf("%s\n", __func__);
    func_light_enter();
    while (func_cb.sta == FUNC_LIGHT)
    {
        func_light_process();
        func_light_message(msg_dequeue());
    }
    func_light_exit();
}

#endif
