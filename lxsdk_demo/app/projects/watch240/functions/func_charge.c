#include "include.h"
#include "func.h"
#include "ute_drv_battery_common.h"
#include "ute_drv_motor.h"
#include "ute_module_systemtime.h"

#define TRACE_EN    1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{
    //数字
    COMPO_ID_NUM_TIME_HOUR = 1,
    COMPO_ID_NUM_TIME_MIN,
    COMPO_ID_PIC_TIME_COLON,

    COMPO_ID_NUM_BAT_VALUE,
    COMPO_ID_PIC_BAT_PERCENT,

    COMPO_ID_RECT_BAT_1,
    COMPO_ID_RECT_BAT_2,
    COMPO_ID_RECT_BAT_3,
    COMPO_ID_RECT_BAT_4,
    COMPO_ID_RECT_BAT_5,
};

#define BAT_PERCENT_VALUE       uteDrvBatteryCommonGetLvl()  //电量百分比数值

typedef struct charge_ui_handle_t_
{
    struct ui_time_hour
    {
        u16 id_hour;
        s16 x_hour, y_hour;
        u16 w_hour, h_hour;
        u8 hour;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_hour;

    struct ui_time_min
    {
        u16 id_min;
        s16 x_min, y_min;
        u16 w_min, h_min;
        u8 min;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_min;

    struct ui_time_colon
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;
        u32 destroy_res;
        u8 rev;
    } time_colon;

    struct ui_bat_value
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } bat_value;

    struct ui_bat_percent
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;
        u8 rev;
    } bat_percent;

    struct ui_bat_rect
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;       //res不为0使用图库，为0使用组件实现
        u32 destroy_res;
        color_t bright_color;
        color_t destroy_color;
        u16 rev;              //矩形圆角
    } bat_rect[5];

} charge_ui_handle_t;

static const charge_ui_handle_t ui_handle =
{

    .time_hour = {
        .id_hour = COMPO_ID_NUM_TIME_HOUR,
        .x_hour = 122+78/2,
        .y_hour = 35+94/2,
        .w_hour = 0,
        .h_hour = 0,
        .hour   = 0,
        .res    = UI_BUF_I330001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 1,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_min = {
        .id_min = COMPO_ID_NUM_TIME_MIN,
        .x_min  = 122+78/2,
        .y_min  = 153+94/2,
        .w_min  = 0,
        .h_min  = 0,
        .min    = 0,
        .res    = UI_BUF_I330001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 1,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_colon = {
        .id     = COMPO_ID_PIC_TIME_COLON,
        .x      = 122+78/2,
        .y      = 128+25/2,
        .w      = 0,
        .h      = 0,
        .bright_res     = UI_BUF_I330001_CHARGE_COLON2_BIN,
        .destroy_res    = UI_BUF_I330001_CHARGE_COLON1_BIN,
        .rev    = 0,
    },

    .bat_value = {
        .id     = COMPO_ID_NUM_BAT_VALUE,
        .x      = 37+60/2,
        .y      = 30+87/2,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I330001_CHARGE_POWER_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 1,  //横向，竖向排列
        .num_align  = 0,   //对其方式
        .flag_zfill = false,
        .margin     = -5,
        .rev    = 3,
    },

    .bat_percent = {
        .id     = COMPO_ID_PIC_BAT_PERCENT,
        .x      = 37+60/2,
        .y      = 30+87+48/2,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I330001_CHARGE_PERCENT_BIN,
        .rev    = 0,
    },

    .bat_rect = {
        [0]     = {COMPO_ID_RECT_BAT_1,     37+60/2,    241+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [1]     = {COMPO_ID_RECT_BAT_2,     37+60/2,    223+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [2]     = {COMPO_ID_RECT_BAT_3,     37+60/2,    205+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [3]     = {COMPO_ID_RECT_BAT_4,     37+60/2,    187+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [4]     = {COMPO_ID_RECT_BAT_5,     37+60/2,    169+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
    },


};

typedef struct f_charge_t_
{
    u8 percent_bkp;
} f_charge_t;

//电量更新
void func_charge_update(void)
{

    //更新bat Value
    compo_number_t* bat_value = compo_getobj_byid(ui_handle.bat_value.id);
    compo_number_set(bat_value, BAT_PERCENT_VALUE);

    //更新bat rect
    u8 bright_value = (sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])) * BAT_PERCENT_VALUE / 100;      //等级

    //熄灭的
    for(int i=0; i<(sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])); i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].destroy_color.r, ui_handle.bat_rect[i].destroy_color.g, ui_handle.bat_rect[i].destroy_color.b));
    }

    //点亮
    for(int i=0; i<bright_value; i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
    }

    //更新 :
    static u32 tick = 0;
    static bool bright_sta = 0;
    compo_picturebox_t* colon = compo_getobj_byid(ui_handle.time_colon.id);
    if (tick_check_expire(tick, 1000))
    {
        tick = tick_get();
        if (bright_sta)
        {
            compo_picturebox_set(colon, ui_handle.time_colon.bright_res);
        }
        else
        {
            compo_picturebox_set(colon, ui_handle.time_colon.destroy_res);
        }
        bright_sta = !bright_sta;
    }

}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建time hour
    compo_number_t* hour = compo_number_create(frm, ui_handle.time_hour.res, ui_handle.time_hour.rev);
    compo_number_set_align(hour, ui_handle.time_hour.num_align);
    compo_number_set_layout(hour, ui_handle.time_hour.num_layout);
    compo_number_set_zfill(hour, ui_handle.time_hour.flag_zfill);
    compo_number_set_margin(hour, ui_handle.time_hour.margin);
    compo_number_set_pos(hour, ui_handle.time_hour.x_hour, ui_handle.time_hour.y_hour);
    compo_number_set(hour, ui_handle.time_hour.hour);
    compo_setid(hour, ui_handle.time_hour.id_hour);
    compo_bonddata(hour, COMPO_BOND_HOUR);

    //创建timer min
    compo_number_t* min = compo_number_create(frm, ui_handle.time_min.res, ui_handle.time_min.rev);
    compo_number_set_align(min, ui_handle.time_min.num_align);
    compo_number_set_layout(min, ui_handle.time_min.num_layout);
    compo_number_set_zfill(min, ui_handle.time_min.flag_zfill);
    compo_number_set_margin(min, ui_handle.time_min.margin);
    compo_number_set_pos(min, ui_handle.time_min.x_min, ui_handle.time_min.y_min);
    compo_number_set(min, ui_handle.time_min.min);
    compo_setid(min, ui_handle.time_min.id_min);
    compo_bonddata(min, COMPO_BOND_MINUTE);

    //创建 :
    compo_picturebox_t* colon = compo_picturebox_create(frm, ui_handle.time_colon.bright_res);
    compo_picturebox_set_pos(colon, ui_handle.time_colon.x, ui_handle.time_colon.y);
    compo_setid(colon, ui_handle.time_colon.id);

    //创建BAT Vaule
    compo_number_t* bat_value = compo_number_create(frm, ui_handle.bat_value.res, ui_handle.bat_value.rev);
    compo_number_set_align(bat_value, ui_handle.bat_value.num_align);
    compo_number_set_layout(bat_value, ui_handle.bat_value.num_layout);
    compo_number_set_zfill(bat_value, ui_handle.bat_value.flag_zfill);
    compo_number_set_margin(bat_value, ui_handle.bat_value.margin);
    compo_number_set_pos(bat_value, ui_handle.bat_value.x, ui_handle.bat_value.y);
    compo_number_set(bat_value, 100);
    compo_setid(bat_value, ui_handle.bat_value.id);

    //创建 %
    compo_picturebox_t* bat_percent = compo_picturebox_create(frm, ui_handle.bat_percent.res);
    compo_picturebox_set_pos(bat_percent, ui_handle.bat_percent.x, ui_handle.bat_percent.y);
    compo_setid(bat_percent, ui_handle.bat_percent.id);

    //创建rect
    for (int i=0; i<sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0]); i++)
    {
        compo_shape_t* bat_rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
        compo_shape_set_location(bat_rect, ui_handle.bat_rect[i].x, ui_handle.bat_rect[i].y, ui_handle.bat_rect[i].w, ui_handle.bat_rect[i].h);
        compo_shape_set_radius(bat_rect, ui_handle.bat_rect[i].rev);
        compo_setid(bat_rect, ui_handle.bat_rect[i].id);
    }

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum
{
    //数字
    COMPO_ID_NUM_TIME_HOUR = 1,
    COMPO_ID_NUM_TIME_MIN,
    COMPO_ID_PIC_TIME_COLON,

    COMPO_ID_NUM_BAT_VALUE,
    COMPO_ID_PIC_BAT_PERCENT,

    COMPO_ID_RECT_BAT_1,
    COMPO_ID_RECT_BAT_2,
    COMPO_ID_RECT_BAT_3,
    COMPO_ID_RECT_BAT_4,
    COMPO_ID_RECT_BAT_5,
};

#define BAT_PERCENT_VALUE       uteDrvBatteryCommonGetLvl()  //电量百分比数值

typedef struct charge_ui_handle_t_
{
    struct ui_time_hour
    {
        u16 id_hour;
        s16 x_hour, y_hour;
        u16 w_hour, h_hour;
        u8 hour;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_hour;

    struct ui_time_min
    {
        u16 id_min;
        s16 x_min, y_min;
        u16 w_min, h_min;
        u8 min;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_min;

    struct ui_time_colon
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;
        u32 destroy_res;
        u8 rev;
    } time_colon;

    struct ui_bat_value
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } bat_value;

    struct ui_bat_percent
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;
        u8 rev;
    } bat_percent;

    struct ui_bat_rect
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;       //res不为0使用图库，为0使用组件实现
        u32 destroy_res;
        color_t bright_color;
        color_t destroy_color;
        u16 rev;              //矩形圆角
    } bat_rect[5];

} charge_ui_handle_t;

static const charge_ui_handle_t ui_handle =
{

    .time_hour = {
        .id_hour = COMPO_ID_NUM_TIME_HOUR,
        .x_hour = 122+78/2,
        .y_hour = 35+94/2,
        .w_hour = 0,
        .h_hour = 0,
        .hour   = 0,
//        .res    = UI_BUF_I330001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 1,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_min = {
        .id_min = COMPO_ID_NUM_TIME_MIN,
        .x_min  = 122+78/2,
        .y_min  = 153+94/2,
        .w_min  = 0,
        .h_min  = 0,
        .min    = 0,
//        .res    = UI_BUF_I330001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 1,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_colon = {
        .id     = COMPO_ID_PIC_TIME_COLON,
        .x      = 122+78/2,
        .y      = 128+25/2,
        .w      = 0,
        .h      = 0,
//        .bright_res     = UI_BUF_I330001_CHARGE_COLON2_BIN,
//        .destroy_res    = UI_BUF_I330001_CHARGE_COLON1_BIN,
        .rev    = 0,
    },

    .bat_value = {
        .id     = COMPO_ID_NUM_BAT_VALUE,
        .x      = 37+60/2,
        .y      = 30+87/2,
        .w      = 0,
        .h      = 0,
//        .res    = UI_BUF_I330001_CHARGE_POWER_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 1,  //横向，竖向排列
        .num_align  = 0,   //对其方式
        .flag_zfill = false,
        .margin     = -5,
        .rev    = 3,
    },

    .bat_percent = {
        .id     = COMPO_ID_PIC_BAT_PERCENT,
        .x      = 37+60/2,
        .y      = 30+87+48/2,
        .w      = 0,
        .h      = 0,
//        .res    = UI_BUF_I330001_CHARGE_PERCENT_BIN,
        .rev    = 0,
    },

    .bat_rect = {
        [0]     = {COMPO_ID_RECT_BAT_1,     37+60/2,    241+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [1]     = {COMPO_ID_RECT_BAT_2,     37+60/2,    223+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [2]     = {COMPO_ID_RECT_BAT_3,     37+60/2,    205+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [3]     = {COMPO_ID_RECT_BAT_4,     37+60/2,    187+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [4]     = {COMPO_ID_RECT_BAT_5,     37+60/2,    169+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
    },


};

typedef struct f_charge_t_
{
    u8 percent_bkp;
} f_charge_t;

//电量更新
void func_charge_update(void)
{

    //更新bat Value
    compo_number_t* bat_value = compo_getobj_byid(ui_handle.bat_value.id);
    compo_number_set(bat_value, BAT_PERCENT_VALUE);

    //更新bat rect
    u8 bright_value = (sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])) * BAT_PERCENT_VALUE / 100;      //等级

    //熄灭的
    for(int i=0; i<(sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])); i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].destroy_color.r, ui_handle.bat_rect[i].destroy_color.g, ui_handle.bat_rect[i].destroy_color.b));
    }

    //点亮
    for(int i=0; i<bright_value; i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
    }

    //更新 :
    static u32 tick = 0;
    static bool bright_sta = 0;
    compo_picturebox_t* colon = compo_getobj_byid(ui_handle.time_colon.id);
    if (tick_check_expire(tick, 1000))
    {
        tick = tick_get();
        if (bright_sta)
        {
            compo_picturebox_set(colon, ui_handle.time_colon.bright_res);
        }
        else
        {
            compo_picturebox_set(colon, ui_handle.time_colon.destroy_res);
        }
        bright_sta = !bright_sta;
    }

}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建time hour
    compo_number_t* hour = compo_number_create(frm, ui_handle.time_hour.res, ui_handle.time_hour.rev);
    compo_number_set_align(hour, ui_handle.time_hour.num_align);
    compo_number_set_layout(hour, ui_handle.time_hour.num_layout);
    compo_number_set_zfill(hour, ui_handle.time_hour.flag_zfill);
    compo_number_set_margin(hour, ui_handle.time_hour.margin);
    compo_number_set_pos(hour, ui_handle.time_hour.x_hour, ui_handle.time_hour.y_hour);
    compo_number_set(hour, ui_handle.time_hour.hour);
    compo_setid(hour, ui_handle.time_hour.id_hour);
    compo_bonddata(hour, COMPO_BOND_HOUR);

    //创建timer min
    compo_number_t* min = compo_number_create(frm, ui_handle.time_min.res, ui_handle.time_min.rev);
    compo_number_set_align(min, ui_handle.time_min.num_align);
    compo_number_set_layout(min, ui_handle.time_min.num_layout);
    compo_number_set_zfill(min, ui_handle.time_min.flag_zfill);
    compo_number_set_margin(min, ui_handle.time_min.margin);
    compo_number_set_pos(min, ui_handle.time_min.x_min, ui_handle.time_min.y_min);
    compo_number_set(min, ui_handle.time_min.min);
    compo_setid(min, ui_handle.time_min.id_min);
    compo_bonddata(min, COMPO_BOND_MINUTE);

    //创建 :
    compo_picturebox_t* colon = compo_picturebox_create(frm, ui_handle.time_colon.bright_res);
    compo_picturebox_set_pos(colon, ui_handle.time_colon.x, ui_handle.time_colon.y);
    compo_setid(colon, ui_handle.time_colon.id);

    //创建BAT Vaule
    compo_number_t* bat_value = compo_number_create(frm, ui_handle.bat_value.res, ui_handle.bat_value.rev);
    compo_number_set_align(bat_value, ui_handle.bat_value.num_align);
    compo_number_set_layout(bat_value, ui_handle.bat_value.num_layout);
    compo_number_set_zfill(bat_value, ui_handle.bat_value.flag_zfill);
    compo_number_set_margin(bat_value, ui_handle.bat_value.margin);
    compo_number_set_pos(bat_value, ui_handle.bat_value.x, ui_handle.bat_value.y);
    compo_number_set(bat_value, 100);
    compo_setid(bat_value, ui_handle.bat_value.id);

    //创建 %
    compo_picturebox_t* bat_percent = compo_picturebox_create(frm, ui_handle.bat_percent.res);
    compo_picturebox_set_pos(bat_percent, ui_handle.bat_percent.x, ui_handle.bat_percent.y);
    compo_setid(bat_percent, ui_handle.bat_percent.id);

    //创建rect
    for (int i=0; i<sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0]); i++)
    {
        compo_shape_t* bat_rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
        compo_shape_set_location(bat_rect, ui_handle.bat_rect[i].x, ui_handle.bat_rect[i].y, ui_handle.bat_rect[i].w, ui_handle.bat_rect[i].h);
        compo_shape_set_radius(bat_rect, ui_handle.bat_rect[i].rev);
        compo_setid(bat_rect, ui_handle.bat_rect[i].id);
    }

    return frm;
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//充电功能事件处理
static void func_charge_process(void)
{
//    f_charge_t *f_charge = (f_charge_t *)func_cb.f_cb;

    func_charge_update();

    if (bsp_charge_sta_get() == 0)
    {
        // func_cb.sta = FUNC_CLOCK;
        uteModuleGuiCommonGoBackLastScreen();
    }

    func_process();
}

//充电功能消息处理
static void func_charge_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            break;
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_LONG:
            break;

        case KU_BACK:
        case KL_BACK:
        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入充电功能
static void func_charge_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_charge_t));
    func_cb.frm_main = func_charge_form_create();

    f_charge_t *f_charge = (f_charge_t *)func_cb.f_cb;
    f_charge->percent_bkp = BAT_PERCENT_VALUE;
    func_cb.enter_tick = tick_get();
}

//退出充电功能
static void func_charge_exit(void)
{
    printf("%s\n", __func__);
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_CHARGE;

    if(sys_cb.power_on_flag == true) return;

    sys_cb.power_on_state=true;
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        sys_cb.power_on_state=false;
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);

        if(time.isWatchSetLangage == false)
        {
            func_cb.sta = FUNC_POWER_ON_LANGUAGE;
        }
        // else
        // {
        //     func_cb.sta = FUNC_POWER_ON_SCAN;
        // }
    }
    sys_cb.power_on_flag = true;
}

//充电功能
void func_charge(void)
{
    printf("%s\n", __func__);
    func_charge_enter();
    while (func_cb.sta == FUNC_CHARGE)
    {
        func_charge_process();
        func_charge_message(msg_dequeue());
        if (bsp_charge_sta_get() && func_cb.sta != FUNC_OTA_UI_MODE && func_cb.sta != FUNC_OTA_MODE)
        {
            func_cb.sta = FUNC_CHARGE;
        }
    }
    func_charge_exit();
}
