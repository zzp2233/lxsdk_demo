#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#include "ute_module_heart.h"

#define TRACE_EN        0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
u32 func_sport_get_current_idx(void);
u32 func_sport_get_str(u8 sport_idx);
u32 func_sport_get_ui(u8 sport_idx);

static bool sport_refresh = true;
static bool sport_start_flag = false;

static u16 cur_sport_type = 0;

enum
{
    COMPO_ID_NUM_SPORT_TIME = 1,        //运动时间
    COMPO_ID_NUM_SPORT_HEARTRATE,   //心率
    COMPO_ID_NUM_SPORT_KCAL,        //卡路里
    COMPO_ID_NUM_SPORT_STEP,        //计步
    COMPO_ID_NUM_SPORT_KM,          //距离

    COMPO_ID_TEXT_SPORT_KCAL,
    COMPO_ID_TEXT_SPORT_STEP,
    COMPO_ID_TEXT_SPORT_KM,
    COMPO_ID_TEXT_SPORT_HEARTRATE,

    COMPO_ID_PIC_SPORT_TYPE,        //运动图标
    COMPO_ID_PIC_SPORT_HEART,
    COMPO_ID_PIC_SPORT_TIME,
    COMPO_ID_PIC_SPORT_STEP,
    COMPO_ID_PIC_SPORT_KCAL,
    COMPO_ID_PIC_SPORT_KM,

    COMPO_ID_ARC_SPORT_KM,
    COMPO_ID_ARC_SPORT_STEP,
    COMPO_ID_ARC_SPORT_KCAL,

    COMPO_ID_BTN_SPORT_STOP,
    COMPO_ID_BTN_SPORT_EXIT,
    COMPO_ID_BTN_SPORT_PAUSE,

    COMPO_ID_TXT_TIME,
};

typedef struct
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u32 res;
    bool wordwap;
    bool center;
    color_t color;
    u8 wordcnt;
    char* str;
} text_t;

#define TEXT_CNT    ((int)(sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0])))
static const text_t sport_sub_run_text[] =
{
    {COMPO_ID_NUM_SPORT_TIME,       10,     66+6,     0,      0,  UI_BUF_0FONT_FONT_NUM_48_BIN,   false,  false,  {.r=169, .g=255, .b=0},         20,    "02:38.58"},
    {COMPO_ID_NUM_SPORT_KM,         45,     163+6-3,    0,      0,  UI_BUF_0FONT_FONT_NUM_28_BIN,   false,  false,  {.r=255, .g=255, .b=255},       10,    "50 KM"},
    {COMPO_ID_NUM_SPORT_STEP,       45,     201+6-3,    0,      0,  UI_BUF_0FONT_FONT_NUM_28_BIN,   false,  false,  {.r=255, .g=255, .b=255},       20,    "20000 STEP"},
    {COMPO_ID_NUM_SPORT_KCAL,       42,     125+6-3,    0,      0,  UI_BUF_0FONT_FONT_NUM_28_BIN,   false,  false,  {.r=255, .g=255, .b=255},       20,     "5000 KCAL"},
    {COMPO_ID_NUM_SPORT_HEARTRATE,  44,     237+6-3,    0,      0,  UI_BUF_0FONT_FONT_NUM_38_BIN,   false,  false,  {.r=255, .g=255, .b=255},       3,      "108"},

    {COMPO_ID_TEXT_SPORT_KM,        45,      163-3,      0,      0,  UI_BUF_0FONT_FONT_BIN,          false,  false,  {.r=255, .g=255, .b=255},       20,     "KM"},
    {COMPO_ID_TEXT_SPORT_STEP,      45,      201+3,      100,    26,  UI_BUF_0FONT_FONT_BIN,          false,  false,  {.r=255, .g=255, .b=255},       20,     "STEP"},
    {COMPO_ID_TEXT_SPORT_KCAL,      42,      125-3,      0,      0,  UI_BUF_0FONT_FONT_BIN,          false,  false,  {.r=255, .g=255, .b=255},       20,     "KCAL"},
    {COMPO_ID_TEXT_SPORT_HEARTRATE, 44,      237+6-3,      0,      0,  UI_BUF_0FONT_FONT_BIN,          false,  false,  {.r=255, .g=255, .b=255},       20,     "MIN/Cnt"},
};

typedef struct
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u32 res;
} picture_t;
#define PICTURE_CNT     ((int)(sizeof(sport_sub_run_picture)/sizeof(sport_sub_run_picture[0])))
static const picture_t sport_sub_run_picture[] =
{
//    {COMPO_ID_PIC_SPORT_TYPE,   10+56/2,  12+56/2,    56,   56,       0},
//    {COMPO_ID_PIC_SPORT_KM,     10+24/2,  169+24/2-3,   24,  24,      UI_BUF_I330001_SPORT_ICON2_DIS_BIN},
//    {COMPO_ID_PIC_SPORT_STEP,   10+24/2,  207+24/2-3,   24,  24,      UI_BUF_I330001_SPORT_ICON2_STEP_BIN},
//    {COMPO_ID_PIC_SPORT_KCAL,   10+24/2,  131+24/2-3,   24,  24,      UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN},
//    {COMPO_ID_PIC_SPORT_HEART,  10+24/2,  251+24/2-3,   24,  24,      UI_BUF_I330001_SPORT_ICON2_HR_BIN},
};

#if USE_GOAL_ARC
typedef struct
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u16 width;
    color_t content_color;
    color_t bg_color;
} arc_t;
#define ARC_CNT     ((int)(sizeof(sport_sub_run_arc)/sizeof(sport_sub_run_arc[0])))
static const arc_t sport_sub_run_arc[] =
{
    {COMPO_ID_ARC_SPORT_KM, 260, 260, 40, 40, 10, {.r=0, .g=202, .b=222}, {.r=0, .g=40, .b=44}},
    {COMPO_ID_ARC_SPORT_STEP, 260, 260, 70, 70, 10, {.r=74, .g=223, .b=12}, {.r=15, .g=45, .b=2}},
    {COMPO_ID_ARC_SPORT_KCAL, 260,260, 100, 100, 10, {.r=253, .g=46, .b=129}, {.r=51, .g=9, .b=26}},
};
#endif // USE_GOAL_ARC

typedef struct f_sport_sub_run_t_
{
    u8 sta;
    u8 hour;                //时
    u8 min;                 //分
    u8 sec;                 //秒
    u16 msec;               //毫秒
    u32 total_msec;         //总毫秒

    u8 heartrate;           //心率
    u16 kcal;               //卡路里
    u32 step;               //计步
    u8 km_integer;                 //距离 整数
    u8 km_decimals;                     // 小数

    page_tp_move_t *ptm;
    bool flag_drag;
    bool flag_auto_move;
    s16 last_dx;
    s16 focus_x;
    point_t moveto;
    u32 tick;

    bool sport_run_state;

    bool heart_pic_state;
    u8 heart_pic_size;
} f_sport_sub_run_t;

u8 sport_finish_mode=0;

//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_sub_run_form_create(void)
{
    bool sport_flag[4]= {true,true,true,true};
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
//    u32 str_id = STR_NULL;
//    if (uteModuleSportMoreSportIsAppStart())                        //多运动是手机端开启的
//    {
//        str_id = func_sport_get_str(uteModuleSportMoreSportGetType()-1);
//        TRACE("【APP连接】运动的类型:%d\n", str_id);
//    }
//    else                                                            //本地点击
//    {
//        str_id = func_sport_get_str(func_sport_get_current_idx());
//        TRACE("【本地】运动的类型:%d\n", str_id);
//    }
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[str_id]);

    compo_textbox_t* time = compo_textbox_create(frm, 32);
    compo_textbox_set_align_center(time, false);
    compo_textbox_set_location(time, 168-52, 15, GUI_SCREEN_CENTER_X, 23);
    compo_setid(time, COMPO_ID_TXT_TIME);
    compo_bonddata(time, COMPO_BOND_HOURMIN_TXT);

//    time = compo_textbox_create(frm, 32);
//    compo_textbox_set_align_center(time, false);
//    compo_textbox_set_location(time, 168-GUI_SCREEN_WIDTH-GUI_SCREEN_CENTER_X/1.5-52, 15, 52+52, 23);
//    compo_setid(time, COMPO_ID_TXT_TIME);
//    compo_bonddata(time, COMPO_BOND_HOURMIN_TXT);


//    switch(func_sport_get_current_idx())
    switch(uteModuleSportMoreSportGetType())
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            sport_flag[0] = true;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        default:
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
    }

    sport_flag[3] = true;   //心率

    if(sport_flag[1] == true)
    {
        sport_finish_mode = 0;
    }
//    else if(func_sport_get_current_idx() == 2 || func_sport_get_current_idx() == 3)
    else if(((uteModuleSportMoreSportGetType() - 1) == 2) || ((uteModuleSportMoreSportGetType() - 1) == 3))
    {
        sport_finish_mode = 1;
    }
    else
    {
        sport_finish_mode = 2;
    }

    //不同运行界面不同数据,用户自己添加 --> todo

    //否则采用默认运行界面

    //创建数字
    for (int i=0; i<TEXT_CNT; i++)
    {

        compo_textbox_t* txt = compo_textbox_create(frm, sport_sub_run_text[i].wordcnt);
        compo_textbox_set_font(txt, sport_sub_run_text[i].res);
        widget_text_set_color(txt->txt, make_color(sport_sub_run_text[i].color.r, sport_sub_run_text[i].color.g, sport_sub_run_text[i].color.b));
        compo_textbox_set_align_center(txt, sport_sub_run_text[i].center);
        compo_textbox_set_multiline(txt, sport_sub_run_text[i].wordwap);
        if (sport_sub_run_text[i].w == 0 || sport_sub_run_text[i].h == 0)
        {
            compo_textbox_set_autosize(txt, true);
        }
        compo_textbox_set_location(txt, sport_sub_run_text[i].x, sport_sub_run_text[i].y, sport_sub_run_text[i].w, sport_sub_run_text[i].h);
        compo_setid(txt, sport_sub_run_text[i].id);

        if(i>=1 && i<5)
        {
            compo_textbox_set_visible(txt, sport_flag[i-1]);///不同模式屏蔽不同功能
        }
        else if(i>=5)
        {
            compo_textbox_set_visible(txt, sport_flag[i-5]);///不同模式屏蔽不同功能
        }
    }

#if USE_GOAL_ARC
    //创建圆弧
    for (int i=0; i<ARC_CNT; i++)
    {
        compo_arc_t *arc = compo_arc_create(frm);
        compo_arc_set_alpha(arc, 0xff, 0xff);
        compo_arc_set_location(arc, sport_sub_run_arc[i].x, sport_sub_run_arc[i].y, sport_sub_run_arc[i].w, sport_sub_run_arc[i].h);
        compo_arc_set_width(arc, sport_sub_run_arc[i].width);
        compo_arc_set_rotation(arc, 0);
        compo_arc_set_angles(arc, 0, 3600);
        compo_arc_set_color(arc, make_color(sport_sub_run_arc[i].content_color.r, sport_sub_run_arc[i].content_color.g, sport_sub_run_arc[i].content_color.b),
                            make_color(sport_sub_run_arc[i].bg_color.r, sport_sub_run_arc[i].bg_color.g, sport_sub_run_arc[i].bg_color.b));
        widget_arc_set_edge_circle(arc->arc, true, true);
        compo_setid(arc, sport_sub_run_arc[i].id);
    }
#endif // USE_GOAL_ARC

    //创建运动类型图片
    for (int i=0; i<PICTURE_CNT; i++)
    {
        compo_picturebox_t* pic;
        if (sport_sub_run_picture[i].res)
        {
            pic = compo_picturebox_create(frm, sport_sub_run_picture[i].res);
        }
        else
        {
//            pic = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
            pic = compo_picturebox_create(frm, func_sport_get_ui(uteModuleSportMoreSportGetType() - 1));
        }
        compo_picturebox_set_pos(pic, sport_sub_run_picture[i].x, sport_sub_run_picture[i].y);
        if (sport_sub_run_picture[i].w !=0 && sport_sub_run_picture[i].h != 0)
        {
            compo_picturebox_set_size(pic, sport_sub_run_picture[i].w, sport_sub_run_picture[i].h);
        }
        compo_setid(pic, sport_sub_run_picture[i].id);

//        if(func_sport_get_current_idx() == 3 || func_sport_get_current_idx() == 2)
        if(((uteModuleSportMoreSportGetType() - 1) == 3) || ((uteModuleSportMoreSportGetType() - 1) == 2))
        {
//            if(sport_sub_run_picture[i].res == UI_BUF_I330001_SPORT_ICON2_DIS_BIN)
//            {
//                compo_picturebox_set(pic, UI_BUF_I330001_SPORT_ICON2_TIMES_BIN);
//            }
        }

        if(i>=1)
        {
            compo_picturebox_set_visible(pic, sport_flag[i-1]);///不同模式屏蔽不同功能
        }
    }


    //右滑退出界面
//    compo_button_t* btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_PUSED_BIN);
//    compo_button_set_pos(btn, -140-120/2, 62+68/2);
//    compo_button_set_visible(btn, true);
//    compo_setid(btn, COMPO_ID_BTN_SPORT_STOP);
//
//    btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_CLOSE_BIN);
//    compo_button_set_pos(btn, -140-120/2, 176+68/2);
//    compo_button_set_visible(btn, true);
//    compo_setid(btn, COMPO_ID_BTN_SPORT_EXIT);

    compo_textbox_t* text = compo_textbox_create(frm, 32);
//    compo_textbox_set_location(text, -140-120/2, 134+23/2,
//                               GUI_SCREEN_WIDTH/1.1,
//                               gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).hei);
    compo_textbox_set(text, i18n[STR_PAUSE]);
    compo_setid(text,COMPO_ID_BTN_SPORT_PAUSE);

//    text = compo_textbox_create(frm, 32);
//    compo_textbox_set_location(text, -140-120/2, 248+23/2,
//                               GUI_SCREEN_WIDTH/1.1,
//                               gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).hei);
    compo_textbox_set(text, i18n[STR_END]);

    return frm;
}

static void func_sport_sub_run_updata(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t *)func_cb.f_cb;
//    u32 str_id = func_sport_get_str(func_sport_get_current_idx());
    u32 str_id = func_sport_get_str(uteModuleSportMoreSportGetType()-1);

    ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
    uteModuleSportGetMoreSportsDatas(data);

//    if (sys_cb.guioff_delay == 0 && !sys_cb.gui_sleep_sta)
//    {
//        gui_sleep();                //仅熄屏
//    }
//
//    reset_sleep_delay();
//    reset_pwroff_delay();

//    if (f_sport_sub_run->sport_run_state == false)
//    {
//        goto __exit;
//    }

    switch (str_id)
    {
        //不同运行界面不同数据,用户自己添加 --> todo
//    case STR_RIDE_ATVS:
//        break;
        //否则采用默认运行界面
        default:
        {
            //更新运动类型


            //更新时间
            char buf[14];
            compo_textbox_t *txt_time = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++)
            {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_TIME)
                {
                    txt_time = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            f_sport_sub_run->hour = data->totalSportTime / 3600;
            f_sport_sub_run->min = ((data->totalSportTime) % 3600) / 60;
            f_sport_sub_run->sec = (data->totalSportTime) % 60;
            snprintf(buf, sizeof(buf), "%02d:%02d:%02d", f_sport_sub_run->hour, f_sport_sub_run->min, f_sport_sub_run->sec);
            compo_textbox_set(txt_time, buf);


            //更新心率
            f_sport_sub_run->heartrate = uteModuleHeartGetHeartValue();//data->saveData.avgHeartRate;
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "%d", f_sport_sub_run->heartrate);
            compo_textbox_t* txt_heartrate = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++)
            {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_HEARTRATE)
                {
                    txt_heartrate = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            compo_textbox_set(txt_heartrate, buf);

            compo_textbox_t* txt_heartrate_unit = compo_getobj_byid(COMPO_ID_TEXT_SPORT_HEARTRATE);
            area_t rel_text_area = widget_text_get_area(txt_heartrate->txt);
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++)
            {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_HEARTRATE)
                {
                    compo_textbox_set_location(txt_heartrate_unit, sport_sub_run_text[i].x + rel_text_area.wid + 10, sport_sub_run_text[i].y+8, 0, 0);
                    break;
                }
            }
            compo_textbox_set(txt_heartrate_unit,i18n[STR_PER_MINUTE]);


            //更新卡路里
            f_sport_sub_run->kcal = data->saveData.sportCaloire;
            memset(buf, 0, sizeof(buf));
//        snprintf(buf, sizeof(buf), "%dKCAL", f_sport_sub_run->kcal);
            snprintf(buf, sizeof(buf), "%d", f_sport_sub_run->kcal);
            compo_textbox_t* txt_kcal = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++)
            {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_KCAL)
                {
                    txt_kcal = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            compo_textbox_set(txt_kcal, buf);

            compo_textbox_t* txt_kcal_unit = compo_getobj_byid(COMPO_ID_TEXT_SPORT_KCAL);
            rel_text_area = widget_text_get_area(txt_kcal->txt);
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++)
            {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_KCAL)
                {
                    compo_textbox_set_location(txt_kcal_unit, sport_sub_run_text[i].x + rel_text_area.wid + 10, sport_sub_run_text[i].y+2, 0, 0);
                    break;
                }
            }
            compo_textbox_set(txt_kcal_unit, i18n[STR_KCAL]);

            //更新计步
            f_sport_sub_run->step = data->saveData.sportStep;
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "%d", f_sport_sub_run->step);
            compo_textbox_t* txt_step = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++)
            {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_STEP)
                {
                    txt_step = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            compo_textbox_set(txt_step, buf);

            compo_textbox_t* txt_step_unit = compo_getobj_byid(COMPO_ID_TEXT_SPORT_STEP);
            rel_text_area = widget_text_get_area(txt_step->txt);
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++)
            {
                if (sport_sub_run_text[i].id == COMPO_ID_TEXT_SPORT_STEP)
                {
                    compo_textbox_set_pos(txt_step_unit, sport_sub_run_text[i].x + rel_text_area.wid + 10, sport_sub_run_text[i].y);
                    break;
                }
            }
            compo_textbox_set(txt_step_unit, i18n[STR_STEP]);

            //更新距离
            // data->saveData.sportDistanceInteger = 5;
            // data->saveData.sportDistanceDecimals = 0;
            f_sport_sub_run->km_integer = data->saveData.sportDistanceInteger;
            f_sport_sub_run->km_decimals = data->saveData.sportDistanceDecimals;
            memset(buf, 0, sizeof(buf));
//            if(func_sport_get_current_idx() == 3 || func_sport_get_current_idx() == 2)//运动次数
            if(((uteModuleSportMoreSportGetType()-1) == 3) || ((uteModuleSportMoreSportGetType()-1) == 2))//运动次数
            {
                ute_module_more_sports_data_t sport_data;
                uteModuleSportGetMoreSportsDatas(&sport_data);
                snprintf(buf,sizeof(buf),"%d",sport_data.saveData.sportTimes);
            }
            else
            {
                if(uteModuleSystemtimeGetDistanceMiType())//英里
                {
                    uint16_t distance = f_sport_sub_run->km_integer*1000+f_sport_sub_run->km_decimals*10;
                    distance = distance*0.6213712;
                    // distance = (uint16_t)ExactDecimalPoint(distance,2);
                    // printf("distance:%d\n",distance);
                    f_sport_sub_run->km_integer  = distance/1000;
                    f_sport_sub_run->km_decimals = distance%1000/10;
                }
                snprintf(buf, sizeof(buf), "%d.%02d", f_sport_sub_run->km_integer, f_sport_sub_run->km_decimals);
            }
            compo_textbox_t* txt_km = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++)
            {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_KM)
                {
                    txt_km = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            compo_textbox_set(txt_km, buf);

            compo_textbox_t* txt_km_unit = compo_getobj_byid(COMPO_ID_TEXT_SPORT_KM);
            rel_text_area = widget_text_get_area(txt_km->txt);
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++)
            {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_KM)
                {
                    compo_textbox_set_location(txt_km_unit, sport_sub_run_text[i].x + rel_text_area.wid + 10, sport_sub_run_text[i].y+2, 0, 0);
                    break;
                }
            }
//            if(func_sport_get_current_idx() == 3 || func_sport_get_current_idx() == 2)//运动次数
            if(((uteModuleSportMoreSportGetType()-1) == 3) || ((uteModuleSportMoreSportGetType()-1) == 2))//运动次数
            {
                compo_textbox_set(txt_km_unit, i18n[STR_SPORT_ORDER]);
            }
            else
            {

                if(uteModuleSystemtimeGetDistanceMiType())//英里
                {
                    compo_textbox_set(txt_km_unit, i18n[STR_MILE]);
                }
                else
                {
                    compo_textbox_set(txt_km_unit, i18n[STR_KM]);
                }

            }



            compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_SPORT_HEART);
//            area_t pic_size = gui_image_get_size(UI_BUF_I330001_SPORT_ICON2_HR_BIN);

            if(uteModuleHeartIsWear() == true)   ///佩戴处理
            {
                if(f_sport_sub_run->heart_pic_size <= 72)
                {
                    f_sport_sub_run->heart_pic_state = true;
                }
                else if(f_sport_sub_run->heart_pic_size >= 110)
                {
                    f_sport_sub_run->heart_pic_state = false;
                }

                if(f_sport_sub_run->heart_pic_state == true)
                {
                    f_sport_sub_run->heart_pic_size ++ ;
                }
                else
                {
                    f_sport_sub_run->heart_pic_size -- ;
                }
//                compo_picturebox_set_size(picbox,f_sport_sub_run->heart_pic_size*pic_size.wid/100,f_sport_sub_run->heart_pic_size*pic_size.hei/100);
            }

#if USE_GOAL_ARC
            //更新圆弧
            compo_arc_t* rc_km = compo_getobj_byid(sport_sub_run_arc[0].id);
            compo_arc_set_value(rc_km, f_sport_sub_run->km * (ARC_VALUE_MAX / 100));
            compo_arc_t* rc_step = compo_getobj_byid(sport_sub_run_arc[1].id);
            compo_arc_set_value(rc_step, f_sport_sub_run->step * ARC_VALUE_MAX / 30000);
            compo_arc_t* rc_kcal = compo_getobj_byid(sport_sub_run_arc[2].id);
            compo_arc_set_value(rc_kcal, f_sport_sub_run->kcal * ARC_VALUE_MAX / 6000);
#endif // USE_GOAL_ARC
        }

        break;

    }

//__exit:
    ab_free(data);

}
static void func_sport_sub_run_handle(void)
{
    static u32 sport_ticks = 0;

    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    compo_button_t* btn = compo_getobj_byid(COMPO_ID_BTN_SPORT_STOP);
    compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_BTN_SPORT_PAUSE);


    if (btn != NULL)
    {
//        if (uteModuleSportMoreSportIsAppStart())                        //多运动是手机端开启的
//        {
#if UTE_MODULE_SPORT_SUPPORT
        u8 ute_sport_status = uteModuleSportMoreSportGetStatus();

        switch (ute_sport_status)
        {
            case ALL_SPORT_STATUS_CLOSE:
            case ALL_SPORT_STATUS_PAUSE:
//                compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PLAY_BIN);
                compo_textbox_set(txt, i18n[STR_CONTINUE]);
                f_sport_sub_run->sport_run_state = false;
                TRACE("【APP连接】运动停止/退出\n");
                sport_start_flag = false;
                break;

            case ALL_SPORT_STATUS_OPEN:
            case ALL_SPORT_STATUS_CONTINUE:
//                compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PUSED_BIN);
                compo_textbox_set(txt, i18n[STR_PAUSE]);
                f_sport_sub_run->sport_run_state = true;
                TRACE("【APP连接】运动开始/继续\n");
                break;

            default:
                break;
        }
//        }
#endif
    }

    if (f_sport_sub_run->flag_drag)
    {
        s32 dy=0,dx=0;
        f_sport_sub_run->flag_drag = ctp_get_dxy(&dx, &dy);
        if (f_sport_sub_run->flag_drag)
        {
            f_sport_sub_run->focus_x = f_sport_sub_run->last_dx + dx;
            if (f_sport_sub_run->focus_x > 320)
            {
                f_sport_sub_run->focus_x = 320;
            }
            else if (f_sport_sub_run->focus_x < 0)
            {
                f_sport_sub_run->focus_x = 0;
            }
            widget_page_set_client(func_cb.frm_main->page_body, f_sport_sub_run->focus_x, 0);
        }
        else     //松手
        {
            s32 dx = f_sport_sub_run->focus_x - f_sport_sub_run->last_dx;
            f_sport_sub_run->last_dx = f_sport_sub_run->focus_x;
            //printf("dx=%d, last_dx = %d\n", dx, f_sport_sub_run->last_dx);
            if (abs_s(dx) > GUI_SCREEN_WIDTH / 5)
            {
                if (dx > 0)
                {
                    f_sport_sub_run->moveto.x = 320;
                    f_sport_sub_run->moveto.y = 0;
                }
                else if (dx < 0)
                {
                    f_sport_sub_run->moveto.x = 0;
                    f_sport_sub_run->moveto.y = 0;
                }
            }
            f_sport_sub_run->flag_auto_move = true;
        }
    }
    else if (f_sport_sub_run->flag_auto_move)
    {
        if (f_sport_sub_run->last_dx == f_sport_sub_run->moveto.x)
        {
            f_sport_sub_run->flag_auto_move = false;
        }
        else if (tick_check_expire(f_sport_sub_run->tick, 16))
        {
            f_sport_sub_run->tick = tick_get();
            if (f_sport_sub_run->last_dx < f_sport_sub_run->moveto.x)
            {
                f_sport_sub_run->last_dx += 16;
                f_sport_sub_run->last_dx = (f_sport_sub_run->last_dx > f_sport_sub_run->moveto.x) ? f_sport_sub_run->moveto.x : f_sport_sub_run->last_dx;
            }
            else if (f_sport_sub_run->last_dx > f_sport_sub_run->moveto.x)
            {
                f_sport_sub_run->last_dx -= 16;
                f_sport_sub_run->last_dx = (f_sport_sub_run->last_dx < f_sport_sub_run->moveto.x) ? f_sport_sub_run->moveto.x : f_sport_sub_run->last_dx;
            }
            widget_page_set_client(func_cb.frm_main->page_body, f_sport_sub_run->last_dx, 0);
        }
    }

    if (tick_check_expire(sport_ticks, 10))
    {
        sport_ticks = tick_get();
        //界面数据更新
        func_sport_sub_run_updata();
    }

    compo_textbox_t* time = compo_getobj_byid(COMPO_ID_TXT_TIME);
    if(uteModuleSystemtime12HOn())
    {
        compo_textbox_set_location(time, 168-52, 15, GUI_SCREEN_CENTER_X, 23);
    }
    else
    {
        compo_textbox_set_location(time, 168, 15, 52+52, 23);
    }
}
static void func_sport_sub_run_init(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;

    if(sport_start_flag == false)
    {
        sport_start_flag = true;
        if (
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
            func_cb.last != FUNC_CAMERA
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
            && func_cb.last != FUNC_CHARGE && func_cb.last != FUNC_LONG_PRESS && func_cb.last != FUNC_BT_CALL && func_cb.last != FUNC_BT_RING)
        {
            if (uteModuleSportMoreSportIsAppStart())
            {
                //uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 1, 1);
                printf("func_cb.last = %d\n", func_cb.last);
                if (func_cb.last != FUNC_SPORT_SWITCH)
                {
                    func_cb.sta = FUNC_SPORT_SWITCH;
                }
                else
                {
                    uteModuleSportSetCountZeroIndex(0);
                }
                TRACE("【APP】开始运动\n");
            }
            else
            {
                uteModuleSportStartMoreSports(func_sport_get_current_idx()+1, 1, 0);
                uteModuleSportSetCountZeroIndex(0);
                TRACE("【本地】开始运动:%d\n",func_sport_get_current_idx()+1);
            }
        }
        f_sport_sub_run->heart_pic_size = 100;
        cur_sport_type = uteModuleSportMoreSportGetType();
        uteModuleHeartStartSingleTesting(TYPE_HEART);
    }
    func_cb.frm_main = func_sport_sub_run_form_create();

}
static void func_sport_sub_run_exit_data(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;

    if(sport_start_flag == false)
    {
        uteModuleGuiCommonDisplayOffAllowGoBack(true);
        uteModuleHeartStopSingleTesting(TYPE_HEART);
    }
    func_cb.last = FUNC_SPORT_SUB_RUN;
}


//按键处理
static void func_sport_sub_run_click_handler(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SPORT_STOP:
        {
            //停止按钮
            compo_button_t* btn = compo_getobj_byid(COMPO_ID_BTN_SPORT_STOP);
            compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_BTN_SPORT_PAUSE);

            if (btn != NULL)
            {
#if UTE_MODULE_SPORT_SUPPORT
                if (f_sport_sub_run->sport_run_state == true)
                {
                    compo_textbox_set(txt, i18n[STR_CONTINUE]);
//                    compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PLAY_BIN);
//                    widget_set_size(btn->widget, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).wid, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).hei);
                    f_sport_sub_run->sport_run_state = false;
                    uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_PAUSE);   //通知APP暂停运动
                    printf("【APP连接】本地通知APP运动停止\n");
                }
                else if (f_sport_sub_run->sport_run_state == false)
                {
//                    compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PUSED_BIN);
                    f_sport_sub_run->sport_run_state = true;
                    compo_textbox_set(txt, i18n[STR_PAUSE]);
//                    widget_set_size(btn->widget, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).wid, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).hei);
                    uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);   //通知APP继续运动
                    printf("【APP连接】本地通知APP运动继续\n");
                }
#endif
            }
        }
        break;
        case COMPO_ID_BTN_SPORT_EXIT:
        {
            const char* msg = NULL;
            int res=0;
#if UTE_MODULE_SPORT_SUPPORT
            bool sport_flag = uteModuleSportMoreSportsIsLessData();
#else
            bool sport_flag = false;
#endif

            //    sport_flag ^=1;
            if (sport_flag)
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG2], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_SPORT);
            }
            else
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG1], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_NONE);
            }

            if (res == MSGBOX_RES_OK)
            {
//                if (uteModuleSportMoreSportIsAppStart()) {                      //多运动是手机端开启的
#if UTE_MODULE_SPORT_SUPPORT
                uteModuleSportStopMoreSports();                             //通知APP退出运动
                TRACE("【APP连接】本地通知APP运动退出\n");
#endif
//                } else {
//                    task_stack_pop();
//                    func_switch_to(FUNC_SPORT, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
//                }
                if (!sport_flag)
                {
//                    func_switch_to(FUNC_SPORT_FINISH, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
                    func_cb.sta = FUNC_SPORT_FINISH;
                }
                sport_start_flag = false;
                if (task_stack_get_top() == FUNC_SPORT_SUB_RUN)
                {
                    task_stack_pop();
                }
            }
            else if (res == MSGBOX_RES_CANCEL)
            {

            }
        }   break;
        default:
            break;
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
u32 func_sport_get_current_idx(void);
u32 func_sport_get_str(u8 sport_idx);
u32 func_sport_get_ui(u8 sport_idx);

static bool sport_refresh = true;
static bool sport_start_flag = false;
u8 sport_finish_mode=0;
static u16 cur_sport_type = 0;

enum
{
    COMPO_ID_NUM_SPORT_TIME = 1,    //运动时间
    COMPO_ID_NUM_SPORT_HEARTRATE,   //心率数据
    COMPO_ID_NUM_SPORT_KCAL,        //卡路里数据
    COMPO_ID_NUM_SPORT_TIME_DOWN,   //运动时间
    COMPO_ID_NUM_SPORT_STEP,        //计步数据
    COMPO_ID_NUM_SPORT_KM,          //距离数据
    COMPO_ID_PIC_SPORT_STEP,        //计步图片
    COMPO_ID_PIC_SPORT_KM,          //距离图片
    COMPO_ID_UINT_SPORT_KM,         //距离单位
    COMPO_ID_NUM_SPORT_COUNT,       //计次数据
    COMPO_ID_PIC_SPORT_COUNT,       //计次图片
    COMPO_ID_NUM_SPORT_TIME_RIGHT,  //运动时间
    COMPO_ID_BTN_SPORT_PALY,        //暂停开始
    COMPO_ID_BTN_SPORT_EXIT,        //结束退出
    COMPO_ID_PIC_SPORT_TITLE,       //运动类型图片
    COMPO_ID_SPOT_SPORT_SHAPE1,      //页码图片
    COMPO_ID_SPOT_SPORT_SHAPE2,      //页码图片
};

typedef struct f_sport_sub_run_t_
{
    bool        touch_flag;
    s32         move_offset;
    s32         page_old_y;
    s32         move_offset_x;
    s32         page_old_x;
    u8          touch_state;
    u8          page_num;
    uint32_t    tick;
    u8          switch_page_state;
    bool        direction;
    uint32_t    updata_tick;
} f_sport_sub_run_t;
enum
{
    UP_DOWM_DIR=0,
    LEFT_RIGHT_DIR,
};

enum
{
    TOUCH_FINISH_STATE=0,
    AUTO_STATE,

};
enum
{
    PAGE_1=0,
    PAGE_2,
    PAGE_3,
};
enum
{
    SWITCH_YES=0,
    SWITCH_NO,
    TOTCH_MOVE,
};
//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_sub_run_form_create(void)
{
    bool sport_flag[3]= {true,true,true};
    char txt_buf[50];
    ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
    uteModuleSportGetMoreSportsDatas(data);
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    switch(uteModuleSportMoreSportGetType() - 1)///功能处理
    {
        case 0://跑步
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 1://骑行
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 2://跳绳
            sport_flag[0] = true;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 3://游泳
            sport_flag[0] = true;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 4://羽毛球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 5://乒乓球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 6://网球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 7://爬山
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 8://徒步
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 9://篮球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 10://足球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 11://棒球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 12://排球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 13://板球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 14://橄榄球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 15://曲棍球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 16://跳舞
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 17://动感单车
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 18://瑜伽
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 19://仰卧起坐
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 20://跑步机
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 21://体操
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 22://划船
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 23://开合跳
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 24://自由训练
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
    }

    compo_picturebox_t* pic = compo_picturebox_create(frm, func_sport_get_ui(uteModuleSportMoreSportGetType() - 1));///运动类型图片
    compo_picturebox_set_size(pic,70,70);
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,70/2+20);

    compo_textbox_t* txt = compo_textbox_create(frm, 8);///运动时长
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 58/2+100, 240, 60);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",data->totalSportTime / 3600,((data->totalSportTime) % 3600) / 60,(data->totalSportTime) % 60);
    compo_textbox_set(txt, txt_buf);
    compo_textbox_set_forecolor(txt, make_color(0xa9,0xff,0x00));
    compo_setid(txt,COMPO_ID_NUM_SPORT_TIME);

    txt = compo_textbox_create(frm, 50);///心率文本
    compo_textbox_set_location(txt, 154/2+32, 22/2+184, 154, 22);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_HEART_RATE],i18n[STR_PER_MINUTE]);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
    compo_textbox_set(txt, txt_buf);

    txt = compo_textbox_create(frm, 50);///卡路里文本
    compo_textbox_set_location(txt, 154/2+196, 22/2+184, 154, 22);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_KCAL],i18n[STR_KCAL]);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
    compo_textbox_set(txt, txt_buf);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///灰色矩形
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+222, 296, 48);
    compo_shape_set_radius(shape, 48/2);
    compo_shape_set_color(shape, make_color(0x29,0x29,0x29));

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///白色矩形
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+222, 2, 48);
    compo_shape_set_color(shape, make_color(0x54,0x54,0x54));

    pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_HR_BIN);///心率图片
    compo_picturebox_set_pos(pic,30/2+56,48/2+222);

    pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_CALORIES_BIN);///卡路里图片
    compo_picturebox_set_pos(pic,30/2+205,48/2+222);

    txt = compo_textbox_create(frm, 3);///心率数据文本
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(txt, 53/2+92-16, 232, 60, 43);
    compo_textbox_set_align_center(txt, false);
    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", uteModuleHeartGetHeartValue());
    compo_textbox_set(txt, txt_buf);
    compo_setid(txt,COMPO_ID_NUM_SPORT_HEARTRATE);

    txt = compo_textbox_create(frm, 3);///卡路里数据文本
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(txt, 53/2+245-16, 232, 60, 43);
    compo_textbox_set_align_center(txt, false);
    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", data->saveData.sportCaloire);
    compo_textbox_set(txt, txt_buf);
    compo_setid(txt,COMPO_ID_NUM_SPORT_KCAL);

    ///*右页*/
    pic = compo_picturebox_create_for_page(frm,frm->page,func_sport_get_ui(uteModuleSportMoreSportGetType() - 1));///运动类型图片
    compo_picturebox_set_size(pic,70,70);
    compo_picturebox_set_pos(pic,-GUI_SCREEN_CENTER_X,70/2+20);
    compo_setid(pic,COMPO_ID_PIC_SPORT_TITLE);

    txt = compo_textbox_create_for_page(frm,frm->page, 8);///运动时长
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(txt, -GUI_SCREEN_CENTER_X, 58/2+126, 240, 60);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",data->totalSportTime / 3600,((data->totalSportTime) % 3600) / 60,(data->totalSportTime) % 60);
    compo_textbox_set(txt, txt_buf);
    compo_textbox_set_forecolor(txt, make_color(0xa9,0xff,0x00));
    compo_setid(txt,COMPO_ID_NUM_SPORT_TIME_RIGHT);

    compo_button_t * btn = compo_button_create_page_by_image(frm,frm->page,UI_BUF_I332001_SPORT_BTN_PUSED_BIN);
    compo_setid(btn,COMPO_ID_BTN_SPORT_PALY);
    compo_button_set_pos(btn,80/2+68-GUI_SCREEN_WIDTH,80/2+234);

    btn =compo_button_create_page_by_image(frm,frm->page,UI_BUF_I332001_SPORT_BTN_CLOSE_BIN);
    compo_setid(btn,COMPO_ID_BTN_SPORT_EXIT);
    compo_button_set_pos(btn,80/2+212-GUI_SCREEN_WIDTH,80/2+234);

    ///*下页*/
    if((uteModuleSportMoreSportGetType() - 1 == 2 || uteModuleSportMoreSportGetType() - 1 == 3) &&
       (sport_flag[0] == false && sport_flag[0] == false) )
    {
        ab_free(data);
        return frm;
    }

    widget_page_t *page = widget_page_create(frm->page);///创建页码页面
    widget_page_set_client(page, 0, 0);
    widget_set_location(page,12/2+340, 30/2+165,12,30);

    shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, 6, 6, 12, 12);
    compo_setid(shape,COMPO_ID_SPOT_SPORT_SHAPE1);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_shape_set_radius(shape, 6);

    shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, 6, 30-6, 12, 12);
    compo_setid(shape,COMPO_ID_SPOT_SPORT_SHAPE2);
    compo_shape_set_color(shape, make_color(0x29,0x29,0x29));
    compo_shape_set_radius(shape, 6);

    txt = compo_textbox_create(frm, 8);///运动时长
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 58/2+26+360, 180, 60);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",data->totalSportTime / 3600,((data->totalSportTime) % 3600) / 60,(data->totalSportTime) % 60);
    compo_textbox_set(txt, txt_buf);
    compo_textbox_set_forecolor(txt, make_color(0xa9,0xff,0x00));
    compo_setid(txt,COMPO_ID_NUM_SPORT_TIME_DOWN);

    if(!(uteModuleSportMoreSportGetType() - 1 == 2 || uteModuleSportMoreSportGetType() - 1 == 3))
    {
        if(sport_flag[0] == true)
        {
            txt = compo_textbox_create(frm, 50);///公里文本
            compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 24/2+96+360, 240, 24);
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_KM],i18n[STR_KM]);
            compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
            compo_textbox_set(txt, txt_buf);
            compo_setid(txt,COMPO_ID_UINT_SPORT_KM);

            shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///灰色矩形
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+130+360, 296, 48);
            compo_shape_set_radius(shape, 48/2);
            compo_shape_set_color(shape, make_color(0x29,0x29,0x29));

            txt = compo_textbox_create(frm, 6);///公里数据文本
            compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
            compo_textbox_set_pos(txt, 43/2+167, 48/2+130+360);
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",  data->saveData.sportDistanceInteger,data->saveData.sportDistanceDecimals);
            compo_textbox_set(txt, txt_buf);
            compo_setid(txt,COMPO_ID_NUM_SPORT_KM);

            pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_DIS_BIN);///公里图片
            compo_picturebox_set_pos(pic,43/2+167-30-widget_text_get_area(txt->txt).wid/2,48/2+130+360);
            compo_setid(pic,COMPO_ID_PIC_SPORT_KM);
        }
        if(sport_flag[1] == true)
        {
            txt = compo_textbox_create(frm, strlen(i18n[STR_STEPS]));///步数文本
            compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 43/2+198+360, 240, 24);
            compo_textbox_set(txt, i18n[STR_STEPS]);

            shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///灰色矩形
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+232+360, 296, 48);
            compo_shape_set_radius(shape, 48/2);
            compo_shape_set_color(shape, make_color(0x29,0x29,0x29));

            txt = compo_textbox_create(frm, 6);///步数数据文本
            compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
            compo_textbox_set_pos(txt, 43/2+167, 48/2+232+360);
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d", data->saveData.sportStep);
            compo_textbox_set(txt, txt_buf);
            compo_setid(txt,COMPO_ID_NUM_SPORT_STEP);

            pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_STEP_BIN);///步数图片
            compo_picturebox_set_pos(pic,43/2+167-30-widget_text_get_area(txt->txt).wid/2,48/2+232+360);
            compo_setid(pic,COMPO_ID_PIC_SPORT_STEP);
        }
    }
    else
    {
        txt = compo_textbox_create(frm, 50);///计数文本
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 24/2+96+360, 240, 24);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_NO_DATA],i18n[STR_SPORT_ORDER]);
        compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
        compo_textbox_set(txt, txt_buf);

        shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///灰色矩形
        compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+130+360, 296, 48);
        compo_shape_set_radius(shape, 48/2);
        compo_shape_set_color(shape, make_color(0x29,0x29,0x29));

        txt = compo_textbox_create(frm, 6);///计数数据文本
        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
        compo_textbox_set_pos(txt, 43/2+167, 48/2+130+360);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",  data->saveData.sportTimes);
        compo_textbox_set(txt, txt_buf);
        compo_setid(txt,COMPO_ID_NUM_SPORT_COUNT);

        pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_TIMES_BIN);///计数图片
        compo_picturebox_set_pos(pic,43/2+167-30-widget_text_get_area(txt->txt).wid/2,48/2+130+360);
        compo_setid(pic,COMPO_ID_PIC_SPORT_COUNT);
    }
    ab_free(data);
    return frm;
}
static void func_soprt_run_move(void)
{
#define   PAGE_TWO_SIZE  360  //最底y轴
#define   TOYCH_LAST_DY  40   //切换页滑动y
#define   TICK_TIME      8   //步进y像素点时间
#define   STEP_NUM       8    //步进y像素点
    f_sport_sub_run_t *f_sleep = (f_sport_sub_run_t *)func_cb.f_cb;


    if(f_sleep->page_num == PAGE_1)//第一页
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            if(f_sleep->direction == UP_DOWM_DIR)
            {
                f_sleep->move_offset   = dy;
            }
            else if(f_sleep->direction == LEFT_RIGHT_DIR)
            {
                f_sleep->move_offset_x = dx;
            }

            if(f_sleep->move_offset > 0)
            {
                f_sleep->move_offset = 0;
            }
            else if(f_sleep->move_offset_x < 0)
            {
                f_sleep->move_offset_x = 0;
            }

            if(f_sleep->direction == UP_DOWM_DIR)
            {
                widget_page_set_client(func_cb.frm_main->page_body, f_sleep->move_offset_x, f_sleep->move_offset);
            }
            else if(f_sleep->direction == LEFT_RIGHT_DIR)
            {
                widget_page_set_client(func_cb.frm_main->page, f_sleep->move_offset_x, f_sleep->move_offset);
            }


            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset <= (-TOYCH_LAST_DY) || f_sleep->move_offset_x >= TOYCH_LAST_DY)//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    f_sleep->tick = tick_get();
                    if(f_sleep->direction == UP_DOWM_DIR)
                    {
                        if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                        {
                            f_sleep->move_offset-=STEP_NUM;

                            if(f_sleep->move_offset <= -GUI_SCREEN_HEIGHT)
                            {
                                f_sleep->move_offset = -GUI_SCREEN_HEIGHT;
                                f_sleep->page_num = PAGE_2;//第2页
                                f_sleep->touch_state = TOUCH_FINISH_STATE;
                                f_sleep->page_old_y = f_sleep->move_offset;
                            }
                        }
                        else if(f_sleep->switch_page_state == SWITCH_NO)
                        {
                            f_sleep->move_offset+=STEP_NUM;

                            if(f_sleep->move_offset >= 0)
                            {
                                f_sleep->move_offset = 0;
                                f_sleep->touch_state = TOUCH_FINISH_STATE;
                            }
                        }
                    }
                    else if(f_sleep->direction == LEFT_RIGHT_DIR)
                    {
                        if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                        {
                            f_sleep->move_offset_x +=STEP_NUM;

                            if(f_sleep->move_offset_x >= GUI_SCREEN_HEIGHT)
                            {
                                f_sleep->move_offset_x = GUI_SCREEN_HEIGHT;
                                f_sleep->page_num = PAGE_3;//第2页
                                f_sleep->touch_state = TOUCH_FINISH_STATE;
                                f_sleep->page_old_x = f_sleep->move_offset_x;
                            }
                        }
                        else if(f_sleep->switch_page_state == SWITCH_NO)
                        {
                            f_sleep->move_offset_x-=STEP_NUM;

                            if(f_sleep->move_offset_x <= 0)
                            {
                                f_sleep->move_offset_x = 0;
                                f_sleep->touch_state = TOUCH_FINISH_STATE;
                            }
                        }
                    }

                }
                if(f_sleep->direction == UP_DOWM_DIR)
                {
                    widget_page_set_client(func_cb.frm_main->page_body, f_sleep->move_offset_x, f_sleep->move_offset);
                }
                else if(f_sleep->direction == LEFT_RIGHT_DIR)
                {
                    widget_page_set_client(func_cb.frm_main->page, f_sleep->move_offset_x, f_sleep->move_offset);
                }
                f_sleep->page_old_x = f_sleep->move_offset_x;
                printf("px:%d py:%d\n",f_sleep->move_offset_x,f_sleep->move_offset);
            }
        }

    }
    else if(f_sleep->page_num == PAGE_2)
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            f_sleep->move_offset = f_sleep->page_old_y+dy;
            if(f_sleep->move_offset < -PAGE_TWO_SIZE)
            {
                f_sleep->move_offset = -PAGE_TWO_SIZE;
            }
            widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);

            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset >= -(GUI_SCREEN_HEIGHT-TOYCH_LAST_DY))//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else if(f_sleep->move_offset > -GUI_SCREEN_HEIGHT)
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
                else
                {
                    f_sleep->switch_page_state = TOTCH_MOVE;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    f_sleep->tick = tick_get();
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->move_offset+=STEP_NUM;

                        if(f_sleep->move_offset >= 0)
                        {
                            f_sleep->move_offset = 0;
                            f_sleep->page_num = PAGE_1;//第2页
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->move_offset-=STEP_NUM;

                        if(f_sleep->move_offset <= -(GUI_SCREEN_HEIGHT))
                        {
                            f_sleep->move_offset = -GUI_SCREEN_HEIGHT;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                }
                f_sleep->page_old_y = f_sleep->move_offset;
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);
            }
        }
    }
    else if(f_sleep->page_num == PAGE_3)//第三页
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            if(f_sleep->direction == LEFT_RIGHT_DIR)
            {
                f_sleep->move_offset_x = f_sleep->page_old_x+dx;
            }

            if(f_sleep->move_offset_x > 360)
            {
                f_sleep->move_offset_x = 360;
            }

            if(f_sleep->direction == LEFT_RIGHT_DIR)
            {
                widget_page_set_client(func_cb.frm_main->page, f_sleep->move_offset_x, 0);
            }


            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset_x <= 360-TOYCH_LAST_DY)//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    f_sleep->tick = tick_get();
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->move_offset_x -=STEP_NUM;

                        if(f_sleep->move_offset_x <= 0)
                        {
                            f_sleep->page_num = PAGE_1;//第1页
                            f_sleep->move_offset_x = 0;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->move_offset_x+=STEP_NUM;

                        if(f_sleep->move_offset_x >= 360)
                        {
                            f_sleep->move_offset_x = 360;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                }

                widget_page_set_client(func_cb.frm_main->page, f_sleep->move_offset_x, 0);
                f_sleep->page_old_x = f_sleep->move_offset_x;
            }
        }
        // printf("px:%d py:%d\n",f_sleep->move_offset_x,f_sleep->move_offset);
    }


}

static void func_sport_sub_run_updata(void)
{

    f_sport_sub_run_t *f_sleep = (f_sport_sub_run_t *)func_cb.f_cb;
    if(tick_check_expire(f_sleep->updata_tick, 200))
    {
        f_sleep->updata_tick = tick_get();
        char txt_buf[50];
        compo_textbox_t* txt_time       = compo_getobj_byid(COMPO_ID_NUM_SPORT_TIME);
        compo_textbox_t* txt_heart      = compo_getobj_byid(COMPO_ID_NUM_SPORT_HEARTRATE);
        compo_textbox_t* txt_kcal       = compo_getobj_byid(COMPO_ID_NUM_SPORT_KCAL);
        compo_textbox_t* txt_step       = compo_getobj_byid(COMPO_ID_NUM_SPORT_STEP);
        compo_textbox_t* txt_km         = compo_getobj_byid(COMPO_ID_NUM_SPORT_KM);
        compo_textbox_t* uint_km        = compo_getobj_byid(COMPO_ID_UINT_SPORT_KM);
        compo_textbox_t* txt_count      = compo_getobj_byid(COMPO_ID_NUM_SPORT_COUNT);
        compo_picturebox_t* pic_km      = compo_getobj_byid(COMPO_ID_PIC_SPORT_KM);
        compo_picturebox_t* pic_step    = compo_getobj_byid(COMPO_ID_PIC_SPORT_STEP);
        compo_picturebox_t* pic_count   = compo_getobj_byid(COMPO_ID_PIC_SPORT_COUNT);
        compo_textbox_t* txt_time_down  = compo_getobj_byid(COMPO_ID_NUM_SPORT_TIME_DOWN);
        compo_textbox_t* txt_time_right = compo_getobj_byid(COMPO_ID_NUM_SPORT_TIME_RIGHT);
        compo_shape_t * page_point1     = compo_getobj_byid(COMPO_ID_SPOT_SPORT_SHAPE1);
        compo_shape_t * page_point2     = compo_getobj_byid(COMPO_ID_SPOT_SPORT_SHAPE2);
        compo_button_t * btn_play       = compo_getobj_byid(COMPO_ID_BTN_SPORT_PALY);

        ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
        uteModuleSportGetMoreSportsDatas(data);

        if(btn_play != NULL)
        {
            if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_OPEN)
            {
                compo_button_set_bgimg(btn_play,UI_BUF_I332001_SPORT_BTN_PUSED_BIN);
            }
            else if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_PAUSE)
            {
                compo_button_set_bgimg(btn_play,UI_BUF_I332001_SPORT_BTN_PLAY_BIN);
            }
        }


        if(txt_time != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",data->totalSportTime / 3600,((data->totalSportTime) % 3600) / 60,(data->totalSportTime) % 60);
            compo_textbox_set(txt_time, txt_buf);
        }

        if(txt_heart != NULL)
        {
            memset(txt_buf, 0, sizeof(txt_buf));
            snprintf(txt_buf, sizeof(txt_buf), "%d", uteModuleHeartGetHeartValue());
            compo_textbox_set(txt_heart, txt_buf);
        }

        if(txt_kcal != NULL)
        {
            memset(txt_buf, 0, sizeof(txt_buf));
            snprintf(txt_buf, sizeof(txt_buf), "%d", data->saveData.sportCaloire);
            compo_textbox_set(txt_kcal, txt_buf);
        }

        if(txt_step != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d", data->saveData.sportStep);
            compo_textbox_set(txt_step, txt_buf);
        }

        if(txt_km != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",  data->saveData.sportDistanceInteger,data->saveData.sportDistanceDecimals);
            compo_textbox_set(txt_km, txt_buf);
        }

        if(txt_count != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",  data->saveData.sportTimes);
            compo_textbox_set(txt_km, txt_buf);
        }

        if(pic_km != NULL)
        {
            compo_picturebox_set_pos(pic_km,43/2+167-30-widget_text_get_area(txt_km->txt).wid/2,48/2+130+360);
        }

        if(pic_step != NULL)
        {
            compo_picturebox_set_pos(pic_step,43/2+167-30-widget_text_get_area(txt_step->txt).wid/2,48/2+232+360);
        }

        if(pic_count != NULL)
        {
            compo_picturebox_set_pos(pic_count,43/2+167-30-widget_text_get_area(txt_count->txt).wid/2,48/2+130+360);
        }

        if(txt_time_down != NULL)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",data->totalSportTime / 3600,((data->totalSportTime) % 3600) / 60,(data->totalSportTime) % 60);
            compo_textbox_set(txt_time_down, txt_buf);
        }

        if(txt_time_right != NULL)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",data->totalSportTime / 3600,((data->totalSportTime) % 3600) / 60,(data->totalSportTime) % 60);
            compo_textbox_set(txt_time_right, txt_buf);
        }

        if(page_point1 != NULL)
        {
            if(f_sleep->page_num == PAGE_1)
            {
                compo_shape_set_color(page_point1,COLOR_WHITE);
            }
            else
            {
                compo_shape_set_color(page_point1, make_color(0x29,0x29,0x29));
            }

        }

        if(page_point2 != NULL)
        {
            if(f_sleep->page_num == PAGE_2)
            {
                compo_shape_set_color(page_point2,COLOR_WHITE);
            }
            else
            {
                compo_shape_set_color(page_point2, make_color(0x29,0x29,0x29));
            }
        }

        ab_free(data);
    }
}
//按键处理
static void func_sport_sub_run_click_handler(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SPORT_PALY:
            if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_OPEN)
            {
                uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_PAUSE);
            }
            else if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_PAUSE)
            {
                uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
            }
            break;
        case COMPO_ID_BTN_SPORT_EXIT:
        {
            int res=0;
            bool sport_flag = uteModuleSportMoreSportsIsLessData();
            //            sport_flag ^=1;
            if (sport_flag)
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG2], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_SPORT);
            }
            else
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG1], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_NONE);
            }

            if (res == MSGBOX_RES_OK)
            {
                uteModuleSportStopMoreSports();                             //通知APP退出运动
                if (!sport_flag)
                {
                    func_cb.sta = FUNC_SPORT_FINISH;
                }
                sport_start_flag = false;
                if (task_stack_get_top() == FUNC_SPORT_SUB_RUN)
                {
                    task_stack_pop();
                }
            }
        }
        break;
        default:
            break;
    }
}
static void func_sport_sub_run_init(void)
{
    if(sport_start_flag == false)///是否正常进入运动
    {
        sport_start_flag = true;
        uteModuleSportStartMoreSports(func_sport_get_current_idx()+1, 1, uteModuleSportMoreSportIsAppStart());
        uteModuleSportSetCountZeroIndex(0);
        uteModuleHeartStartSingleTesting(TYPE_HEART);
        cur_sport_type   = uteModuleSportMoreSportGetType();
    }

    func_cb.frm_main = func_sport_sub_run_form_create();


}
static void func_sport_sub_run_exit_data(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;

    if(sys_cb.refresh_language_flag == false || sport_start_flag == true)//刷新语言时不清除数据
    {
        uteModuleHeartStopSingleTesting(TYPE_HEART);
        uteModuleGuiCommonDisplayOffAllowGoBack(true);
        if (task_stack_get_top() == FUNC_SPORT_SUB_RUN)
        {
            task_stack_pop();
        }
    }
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//功能事件处理
static void func_sport_sub_run_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_sport_sub_run_handle();
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    func_soprt_run_move();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_process();
}


//室内跑步功能消息处理
static void func_sport_sub_run_message(size_msg_t msg)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
            f_sport_sub_run->touch_flag = true;
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;
        case MSG_SYS_500MS:
            func_sport_sub_run_updata();
            break;
        case MSG_CTP_CLICK:
            func_sport_sub_run_click_handler();
            break;
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_LONG_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_LONG_DOWN:
            f_sport_sub_run->direction = UP_DOWM_DIR;
            break;
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG_LEFT:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_LONG_RIGHT:
            f_sport_sub_run->direction = LEFT_RIGHT_DIR;
            break;
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:

            f_sport_sub_run->flag_drag = true;
            f_sport_sub_run->flag_auto_move = false;

            break;
        case KU_BACK:
            f_sport_sub_run->flag_auto_move = true;
            f_sport_sub_run->moveto.x = 320;
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;

        default:
            func_message(msg);
            break;
    }
}



//进入室内跑步功能
static void func_sport_sub_run_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_sport_sub_run_t));
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_sport_sub_run_init();
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    func_sport_sub_run_init();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//退出室内跑步功能
static void func_sport_sub_run_exit(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_sport_sub_run_exit_data();
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    func_sport_sub_run_exit_data();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//室内跑步功能
void func_sport_sub_run(void)
{
    printf("%s\n", __func__);
    func_sport_sub_run_enter();
    while (func_cb.sta == FUNC_SPORT_SUB_RUN)
    {
        func_sport_sub_run_message(msg_dequeue());
        func_sport_sub_run_process();
#if UTE_MODULE_SPORT_SUPPORT
        if (cur_sport_type != uteModuleSportMoreSportGetType())         //运动种类改变，直接退出，重新进去对应的运动界面
        {
            break;
        }
#endif
    }
    func_sport_sub_run_exit();
}
#endif

