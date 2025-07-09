#include "include.h"
#include "func.h"


#if UTE_MODULE_SCREENS_TIMER_SUPPORT

#define TRACE_EN    1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
#define PAGE_LOCATION_LAST            -173-54
#define PAGE_HEIGHT                   338
#define CONTROL_LENGTH                10
#define TIME_SET_TXT_SPACING          43
#define TIME_SET_TXT_SPACING_SHIFT    20
//显示页面
enum
{
    TIMER_PAGE_SELECT,
    TIMER_PAGE_CUSTOM,
    TIMER_PAGE_COUNTDOWN,
};

//计时状态
enum
{
    TIMER_STA_IDLE,
    TIMER_STA_WORKING,
    TIMER_STA_PAUSE,
    TIMER_STA_DONE,
    TIMER_STA_RESET,
};

//组件ID
enum
{
    ///PAGE_SELECT
    //按键
    COMPO_ID_BTN_1MIN   = 1,
    COMPO_ID_BTN_2MIN   = 2,
    COMPO_ID_BTN_3MIN   = 3,
    COMPO_ID_BTN_5MIN   = 5,
    COMPO_ID_BTN_10MIN  = 10,
    COMPO_ID_BTN_30MIN  = 30,
    COMPO_ID_BTN_CUSTOM,

    ///PAGE_CUSTOM
    //秒
    COMPO_ID_SEC_1,
    COMPO_ID_SEC_2,
    COMPO_ID_SEC_3,
    COMPO_ID_SEC_4,
    COMPO_ID_SEC_5,
    //分
    COMPO_ID_MIN_1,
    COMPO_ID_MIN_2,
    COMPO_ID_MIN_3,
    COMPO_ID_MIN_4,
    COMPO_ID_MIN_5,
    //时
    COMPO_ID_HOUR_1,
    COMPO_ID_HOUR_2,
    COMPO_ID_HOUR_3,
    COMPO_ID_HOUR_4,
    COMPO_ID_HOUR_5,
    //按键
    COMPO_ID_HOUR_IMG_BTN,
    COMPO_ID_MIN_IMG_BTN,
    COMPO_ID_SEC_IMG_BTN,
    COMPO_ID_BTN_OK,            //ok
    ///PAGE_COUNTDOWN
    //按键
    COMPO_ID_BTN_START,
    COMPO_ID_BTN_NO,
    //数字
    COMPO_ID_NUM_COUNTDOWN,
    COMPO_ID_COUNT_FINSH,
};

typedef struct f_timer_t_
{
    widget_page_t *page_sub;///创建按键父局器页面
    point_t moveto;
    s16 old_y;
    s16 disp_flag;
    bool touch_flag;
    u8 set_time_flag;
    u8 page_disp;
    s8 hour;
    s8 min;
    s8 sec;
    u8 sec_buf[5];
    u8 min_buf[5];
    u8 hour_buf[5];
    u8 set_time_buf[5];
} f_timer_t;

typedef struct timer_txt_item_t_
{
    const char *text;
    u16 btn_id;
    s16 x;
    s16 y;
    u32 res_addr;
} timer_txt_item_t;
#define TIMER_TXT_ITEM_CNT  ((int)(sizeof(tbl_timer_txt_item) / sizeof(tbl_timer_txt_item[0])))
static const timer_txt_item_t tbl_timer_txt_item[] =
{
    {"01:00",      COMPO_ID_BTN_1MIN,  GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74,       UI_BUF_I330001_TIMER_BG_BIN},
    {"02:00",      COMPO_ID_BTN_2MIN,  GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74,       UI_BUF_I330001_TIMER_BG_BIN},
    {"03:00",      COMPO_ID_BTN_3MIN,  GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74*2+CONTROL_LENGTH,       UI_BUF_I330001_TIMER_BG_BIN},
    {"05:00",      COMPO_ID_BTN_5MIN,  GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74*2+CONTROL_LENGTH,       UI_BUF_I330001_TIMER_BG_BIN},
    {"10:00",      COMPO_ID_BTN_10MIN, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74*3+CONTROL_LENGTH*2,       UI_BUF_I330001_TIMER_BG_BIN},
    {"30:00",      COMPO_ID_BTN_30MIN, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74*3+CONTROL_LENGTH*2,       UI_BUF_I330001_TIMER_BG_BIN},
};

//item列表，创建时遍历一下
typedef struct timer_btn_item_t_
{
    const char *text;
    u16 txt_id;
    s16 x;
    s16 y;
} timer_btn_item_t;

#define TIMER_BTN_ITEM_CNT  ((int)(sizeof(tbl_timer_btn_item) / sizeof(tbl_timer_btn_item[0])))
static const timer_btn_item_t tbl_timer_btn_item[] =
{
    {"58",         COMPO_ID_SEC_1,      207,                         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING*2},
    {"59",         COMPO_ID_SEC_2,      207,                         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING},
    {"00",         COMPO_ID_SEC_3,      207,                         GUI_SCREEN_CENTER_Y},
    {"01",         COMPO_ID_SEC_4,      207,                         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING},
    {"02",         COMPO_ID_SEC_5,      207,                         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING*2},

    {"58",         COMPO_ID_MIN_1,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING*2},
    {"59",         COMPO_ID_MIN_2,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING},
    {"00",         COMPO_ID_MIN_3,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y},
    {"01",         COMPO_ID_MIN_4,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING},
    {"02",         COMPO_ID_MIN_5,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING*2},

    {"22",         COMPO_ID_HOUR_1,      33,                         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING*2},
    {"23",         COMPO_ID_HOUR_2,      33,                         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING},
    {"00",         COMPO_ID_HOUR_3,      33,                         GUI_SCREEN_CENTER_Y},
    {"01",         COMPO_ID_HOUR_4,      33,                         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING},
    {"02",         COMPO_ID_HOUR_5,      33,                         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING*2},
};

//时间设置初始化
static void func_timer_setting_date_init(void)
{
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;
    char time_setting[5]= {58,59,0,01,02};
    char time_setting_hour[5]= {22,23,0,01,02};

    for(int i=0; i<5; i++)
    {
        f_timer->sec_buf[i]=time_setting[i];
        f_timer->min_buf[i]=time_setting[i];
        f_timer->hour_buf[i]=time_setting_hour[i];
    }
}
//创建定时器窗体，文件内调用
static compo_form_t *func_timer_form_create_by_type(u8 page_type)
{
    u8 hour, min, sec;
    u32 res_addr;
    char str_buff[24];
    compo_button_t *btn;
    compo_textbox_t *txt;

    f_timer_t *f_timer = NULL;
    if (func_cb.sta == FUNC_TIMER)
    {
        f_timer = (f_timer_t*)func_cb.f_cb;
    }


    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    // if(page_type != TIMER_PAGE_SELECT)
    // {
    //     compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    //     compo_form_set_title(frm, i18n[STR_TIMER]);
    // }

    switch (page_type)
    {
        case TIMER_PAGE_SELECT:
        {
//                widget_page_t* page = widget_page_create(frm->page);
            widget_page_t* page = frm->page_body;//frm_main->page_body;
            widget_set_location(page, GUI_SCREEN_CENTER_X,PAGE_HEIGHT/2,GUI_SCREEN_WIDTH,PAGE_HEIGHT);

            //创建文本->按钮
            for (u8 idx = 0; idx < TIMER_TXT_ITEM_CNT; idx++)
            {
                btn = compo_button_create_page_by_image(frm, page, tbl_timer_txt_item[idx].res_addr);
                compo_setid(btn, tbl_timer_txt_item[idx].btn_id);
                compo_button_set_pos(btn, tbl_timer_txt_item[idx].x, tbl_timer_txt_item[idx].y);

                txt = compo_textbox_create_for_page(frm, page, strlen(tbl_timer_txt_item[idx].text));
                compo_textbox_set_pos(txt, tbl_timer_txt_item[idx].x, tbl_timer_txt_item[idx].y);
                compo_textbox_set(txt, tbl_timer_txt_item[idx].text);
            }
            btn = compo_button_create_page_by_image(frm, page, UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN);
            compo_setid(btn, COMPO_ID_BTN_CUSTOM);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/2.74*4+CONTROL_LENGTH);

            txt = compo_textbox_create_for_page(frm, page, strlen(i18n[STR_CUSTOM]));
            compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/2.74*4+CONTROL_LENGTH);
            compo_textbox_set(txt, i18n[STR_CUSTOM]);

            if (func_cb.sta == FUNC_TIMER)
            {
                f_timer->page_sub = page;
            }
            compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
            compo_form_set_title(frm, i18n[STR_TIMER]);
            widget_page_set_client(page, 0, -44);
        }
        break;

        case TIMER_PAGE_CUSTOM:

            compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
            compo_form_set_title(frm, i18n[STR_CUSTOM]);
            if (func_cb.sta == FUNC_TIMER)
            {
                func_timer_setting_date_init();
            }
            compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_PUBLIC_KUANG_COLON_BIN);
            compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X+44,GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING/2+4 );
            picbox = compo_picturebox_create(frm, UI_BUF_I330001_PUBLIC_KUANG_COLON_BIN);
            compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-44,GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING/2+4 );

            for(int i=0; i<TIMER_BTN_ITEM_CNT; i++)
            {
                txt = compo_textbox_create(frm, 2);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set(txt,tbl_timer_btn_item[i].text );
                compo_setid(txt,tbl_timer_btn_item[i].txt_id);
                compo_textbox_set_pos(txt,tbl_timer_btn_item[i].x,tbl_timer_btn_item[i].y-TIME_SET_TXT_SPACING_SHIFT);
            }
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_TIMER_BOX01_BIN);
            compo_button_set_pos(btn,tbl_timer_btn_item[2].x,tbl_timer_btn_item[2].y -TIME_SET_TXT_SPACING_SHIFT);
            compo_setid(btn,COMPO_ID_SEC_IMG_BTN);

            btn = compo_button_create_by_image(frm, UI_BUF_I330001_TIMER_BOX01_BIN);
            compo_button_set_pos(btn,tbl_timer_btn_item[7].x,tbl_timer_btn_item[7].y -TIME_SET_TXT_SPACING_SHIFT);
            compo_setid(btn,COMPO_ID_MIN_IMG_BTN);

            btn = compo_button_create_by_image(frm, UI_BUF_I330001_TIMER_BOX01_BIN);
            compo_button_set_pos(btn,tbl_timer_btn_item[12].x,tbl_timer_btn_item[12].y -TIME_SET_TXT_SPACING_SHIFT);
            compo_setid(btn,COMPO_ID_HOUR_IMG_BTN);

            compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,tbl_timer_btn_item[0].y-TIME_SET_TXT_SPACING_SHIFT,GUI_SCREEN_WIDTH,42);
            compo_shape_set_color(shape, COLOR_BLACK);

            shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,tbl_timer_btn_item[4].y+21-TIME_SET_TXT_SPACING_SHIFT,GUI_SCREEN_WIDTH,84);
            compo_shape_set_color(shape, COLOR_BLACK);

            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN);
            compo_setid(btn,COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 247);

            txt = compo_textbox_create(frm, strlen(i18n[STR_OK]));
            compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 247);
            compo_textbox_set(txt, i18n[STR_OK]);

            break;

        case TIMER_PAGE_COUNTDOWN:
            compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
            compo_form_set_title(frm, i18n[STR_TIMER]);
            //新建按钮
            res_addr = sys_cb.timer_sta == TIMER_STA_WORKING ? UI_BUF_I330001_PUBLIC_PAUSED_BIN : (sys_cb.timer_sta == TIMER_STA_DONE ? UI_BUF_I330001_PUBLIC_RETRY_BIN : UI_BUF_I330001_PUBLIC_PLAY_BIN);
            btn = compo_button_create_by_image(frm, res_addr); //start/pause/again
            compo_setid(btn, COMPO_ID_BTN_START);
            compo_button_set_pos(btn, 175, 246);
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE01_BIN);  //close
            compo_setid(btn, COMPO_ID_BTN_NO);
            compo_button_set_pos(btn, 63, 246);
            //新建数字
            hour = SEC_TO_HOUR(sys_cb.timer_left_sec);
            min = SEC_TO_MIN(sys_cb.timer_left_sec);
            sec = SEC_TO_SEC(sys_cb.timer_left_sec);
            txt = compo_textbox_create(frm, 12);
            compo_setid(txt, COMPO_ID_NUM_COUNTDOWN);
            compo_textbox_set_align_center(txt, false);     //左对齐显示
            compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/4, 110);
            compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
            snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
            compo_textbox_set(txt, str_buff);

//            txt = compo_textbox_create(frm, strlen("计时结束"));
//            compo_setid(txt, COMPO_ID_COUNT_FINSH);
//            compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X/1.5);
//            compo_textbox_set(txt, "计时结束");
//            compo_textbox_set_visible(txt, sys_cb.timer_sta == TIMER_STA_DONE);

            break;

        default:
            break;
    }

    return frm;
}

//创建定时器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_timer_form_create(void)
{
    compo_form_t *frm = NULL;
    switch (sys_cb.timer_sta)
    {
        case TIMER_STA_IDLE:
            frm = func_timer_form_create_by_type(TIMER_PAGE_SELECT);
//             printf("11111111111111111111111111111\n");
            break;

        case TIMER_STA_WORKING:
            uteModuleGuiCommonDisplayOffAllowGoBack(false);
        case TIMER_STA_PAUSE:
        case TIMER_STA_DONE:
        case TIMER_STA_RESET:
//             printf("22222222222222222222222222222222222\n");
            frm = func_timer_form_create_by_type(TIMER_PAGE_COUNTDOWN);
            break;

        default:
            break;
    }

    return frm;
}

//触摸按钮效果处理
static void func_timer_button_touch_handle(void)
{
    compo_button_t *btn;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_START:
            btn = compo_getobj_byid(COMPO_ID_BTN_START);
            if (sys_cb.timer_sta == TIMER_STA_WORKING)
            {
                compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_PAUSED_BIN);
            }
            else if (sys_cb.timer_sta == TIMER_STA_DONE)
            {
                ;
            }
            else
            {
                compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_PLAY_BIN);
            }
            break;

        case COMPO_ID_BTN_NO:
            btn = compo_getobj_byid(COMPO_ID_BTN_NO);
//            compo_button_set_bgimg(btn, UI_BUF_COMMON_NO_CLICK_BIN);
            break;

        default:
            break;
    }

}

//释放按钮效果处理
static void func_timer_button_release_handle(void)
{
    u32 res_addr;
    compo_button_t *btn;

    if ((btn = compo_getobj_byid(COMPO_ID_BTN_START)) != NULL)
    {
        res_addr = sys_cb.timer_sta == TIMER_STA_WORKING ? UI_BUF_I330001_PUBLIC_PAUSED_BIN : (sys_cb.timer_sta == TIMER_STA_DONE ? UI_BUF_I330001_PUBLIC_RETRY_BIN : UI_BUF_I330001_PUBLIC_PLAY_BIN);
        compo_button_set_bgimg(btn, res_addr);
    }
    if ((btn = compo_getobj_byid(COMPO_ID_BTN_NO)) != NULL)
    {
//        compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
    }
}

//100ms计时器秒数刷新回调函数
static void timer_100ms_pro(co_timer_t *timer, void *param)
{
//    bool done = false;
    u8 *count = NULL;
    static bool lowpwr_sta_bkp;
    static u8 rtc_cal_cnt_bkp;
    static u32 rtccnt_tmp;
    u16 sec_past;
    bool lowpwr_sta = bsp_system_is_sleep() /*| sys_cb.idle_sta*/;
    static u8 cnt = 0;

    if (param)
    {
        count = (u8 *)param;
    }

    if (count && sys_cb.timer_sta == TIMER_STA_WORKING)    //休眠不计时
    {
        if (++(*count) >= 10)   //1s
        {
            *count = 0;
        }

        if (!lowpwr_sta)
        {
            if (rtccnt_tmp != RTCCNT)
            {
                rtccnt_tmp = RTCCNT;
                *count = 0;
            }
        }
        else            //省电/休眠模式，RTC已休眠
        {
            if (lowpwr_sta_bkp == false)      //初次进入
            {
                rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
            }
            if (rtc_cal_cnt_bkp != sys_cb.rtc_cal_cnt)      //RTC已校准，同步校准
            {
                rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
                rtccnt_tmp = RTCCNT;
                // printf("calibrated!\n");
            }
            else if (*count == 0)
            {
                rtccnt_tmp++;
            }
        }


        sec_past = rtccnt_tmp - sys_cb.timer_start_rtc;
        if (sec_past < sys_cb.timer_total_sec)
        {
            sys_cb.timer_left_sec = sys_cb.timer_total_sec - sec_past;
            // printf("countdown-->[%d/%d][%d %d]\n", sec_past, sys_cb.timer_total_sec, rtccnt_tmp, RTCCNT);
        }
        else
        {
            sys_cb.timer_left_sec = 0;
            // printf(">>>COUNTDOWN_FINISH\n");

            if(func_cb.sta == FUNC_TIMER)
            {
                sys_cb.timer_sta = TIMER_STA_DONE;
            }
            else
            {
                sys_cb.timer_sta = TIMER_STA_IDLE;
            }
            if (sys_cb.timer_done == false)
            {
                sys_cb.timer_done = true;
            }
        }
    }


//    if (done)
//    {
//        printf(">>>COUNTDOWN_FINISH\n");
//        sys_cb.timer_sta = TIMER_STA_DONE;
//        if (sys_cb.timer_done == false)
//        {
//            sys_cb.timer_done = true;
//        }
//    }

    lowpwr_sta_bkp = lowpwr_sta;
}

//单击按钮
static void func_timer_button_click(void)
{
    static co_timer_t timer_timer;
    static u8 count_100ms;  //100ms定时器计数，10次为1s，减小暂停误差
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;
    u8 page_next = f_timer->page_disp;
    int id = compo_get_button_id();

    switch (f_timer->page_disp)
    {
        case TIMER_PAGE_SELECT:
            switch (id)
            {
                case COMPO_ID_BTN_1MIN:
                case COMPO_ID_BTN_2MIN:
                case COMPO_ID_BTN_3MIN:
                case COMPO_ID_BTN_5MIN:
                case COMPO_ID_BTN_10MIN:
                case COMPO_ID_BTN_30MIN:
                    sys_cb.timer_custom_sec = sys_cb.timer_left_sec = sys_cb.timer_total_sec = id * 60;
                    sys_cb.timer_sta = TIMER_STA_RESET;
                    page_next = TIMER_PAGE_COUNTDOWN;
                    break;

                case COMPO_ID_BTN_CUSTOM:
                    f_timer->hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                    f_timer->min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                    f_timer->sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                    page_next = TIMER_PAGE_CUSTOM;
                    break;

                default:
                    break;
            }
            break;

        case TIMER_PAGE_CUSTOM:
            switch (id)
            {
                case COMPO_ID_BTN_OK:

                    f_timer->hour = f_timer->hour_buf[2];
                    f_timer->min  = f_timer->min_buf[2];
                    f_timer->sec  = f_timer->sec_buf[2];

                    sys_cb.timer_left_sec = sys_cb.timer_total_sec = sys_cb.timer_custom_sec = f_timer->hour * 3600 + f_timer->min * 60 + f_timer->sec;
                    sys_cb.timer_sta = TIMER_STA_RESET;
                    page_next = TIMER_PAGE_COUNTDOWN;
                    break;

                default:
                    break;
            }
            break;

        case TIMER_PAGE_COUNTDOWN:
            switch (id)
            {
                case COMPO_ID_BTN_START:
//                    printf("COMPO_ID_BTN_START %d\n",sys_cb.timer_sta);
                    if (sys_cb.timer_sta == TIMER_STA_WORKING)
                    {
                        sys_cb.timer_sta = TIMER_STA_PAUSE;
                        uteModuleGuiCommonDisplayOffAllowGoBack(true);
                    }
                    else if (sys_cb.timer_sta == TIMER_STA_DONE)
                    {
                        sys_cb.timer_start_rtc = compo_cb.rtc_cnt;
                        count_100ms = 0;
                        co_timer_set(&timer_timer, 100, TIMER_REPEAT, LEVEL_LOW_PRI, timer_100ms_pro, &count_100ms);
//                        count_100ms = 0;
//                        sys_cb.timer_left_sec = sys_cb.timer_total_sec;
//                        sys_cb.timer_sta = TIMER_STA_RESET;
//                        uteModuleGuiCommonDisplayOffAllowGoBack(true);
                        uteModuleGuiCommonDisplayOffAllowGoBack(false);
                        sys_cb.timer_sta = TIMER_STA_WORKING;
                    }
                    else
                    {
                        if (sys_cb.timer_sta == TIMER_STA_RESET)
                        {
                            sys_cb.timer_start_rtc = compo_cb.rtc_cnt;
                            count_100ms = 0;
                            co_timer_set(&timer_timer, 100, TIMER_REPEAT, LEVEL_LOW_PRI, timer_100ms_pro, &count_100ms);
                        }
                        sys_cb.timer_sta = TIMER_STA_WORKING;
                        uteModuleGuiCommonDisplayOffAllowGoBack(false);
                    }
                    break;

                case COMPO_ID_BTN_NO:
                    co_timer_del(&timer_timer);
                    sys_cb.timer_sta = TIMER_STA_IDLE;
                    sys_cb.timer_left_sec = sys_cb.timer_total_sec = 0;
                    page_next = TIMER_PAGE_SELECT;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    func_timer_button_release_handle();

    if (page_next != f_timer->page_disp)
    {
        f_timer->old_y = 0;
        f_timer->moveto.y = 0;
        f_timer->page_disp = page_next;
        compo_form_destroy(func_cb.frm_main);
        func_cb.frm_main = func_timer_form_create_by_type(f_timer->page_disp);

    }

//    TRACE("page[%d] sta[%d]\n", f_timer->page_disp, sys_cb.timer_sta);
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define PAGE_LOCATION_LAST            -173-80
#define PAGE_HEIGHT                   338
#define CONTROL_LENGTH                10
#define TIME_SET_TXT_SPACING          43
#define TIME_SET_TXT_SPACING_SHIFT    20
//显示页面
enum
{
    TIMER_PAGE_SELECT,
    TIMER_PAGE_CUSTOM,
    TIMER_PAGE_COUNTDOWN,
};

//计时状态
enum
{
    TIMER_STA_IDLE,
    TIMER_STA_WORKING,
    TIMER_STA_PAUSE,
    TIMER_STA_DONE,
    TIMER_STA_RESET,
};

//组件ID
enum
{
    ///PAGE_SELECT
    //按键
    COMPO_ID_BTN_1MIN   = 1,
    COMPO_ID_BTN_2MIN   = 2,
    COMPO_ID_BTN_3MIN   = 3,
    COMPO_ID_BTN_5MIN   = 5,
    COMPO_ID_BTN_10MIN  = 10,
    COMPO_ID_BTN_30MIN  = 30,
    COMPO_ID_BTN_CUSTOM,

    ///PAGE_CUSTOM
    //秒
    COMPO_ID_SEC_1,
    COMPO_ID_SEC_2,
    COMPO_ID_SEC_3,
    COMPO_ID_SEC_4,
    COMPO_ID_SEC_5,
    //分
    COMPO_ID_MIN_1,
    COMPO_ID_MIN_2,
    COMPO_ID_MIN_3,
    COMPO_ID_MIN_4,
    COMPO_ID_MIN_5,
    //时
    COMPO_ID_HOUR_1,
    COMPO_ID_HOUR_2,
    COMPO_ID_HOUR_3,
    COMPO_ID_HOUR_4,
    COMPO_ID_HOUR_5,
    //按键
    COMPO_ID_HOUR_IMG_BTN,
    COMPO_ID_MIN_IMG_BTN,
    COMPO_ID_SEC_IMG_BTN,
    COMPO_ID_BTN_OK,            //ok
    ///PAGE_COUNTDOWN
    //按键
    COMPO_ID_BTN_START,
    COMPO_ID_BTN_NO,
    //数字
    COMPO_ID_NUM_COUNTDOWN,
    COMPO_ID_COUNT_FINSH,
};

typedef struct f_timer_t_
{
    widget_page_t *page_sub;///创建按键父局器页面
    point_t moveto;
    s16 old_y;
    s16 disp_flag;
    bool touch_flag;
    u8 set_time_flag;
    u8 page_disp;
    s8 hour;
    s8 min;
    s8 sec;
    u8 sec_buf[5];
    u8 min_buf[5];
    u8 hour_buf[5];
    u8 set_time_buf[5];
} f_timer_t;

typedef struct timer_txt_item_t_
{
    const char *text;
    u16 btn_id;
    s16 x;
    s16 y;
    u32 res_addr;
} timer_txt_item_t;
#define TIMER_TXT_ITEM_CNT  ((int)(sizeof(tbl_timer_txt_item) / sizeof(tbl_timer_txt_item[0])))
static const timer_txt_item_t tbl_timer_txt_item[] =
{
    {"01:00",      COMPO_ID_BTN_1MIN,  GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74,       UI_BUF_I332001_TIMER_BG_BIN},
    {"02:00",      COMPO_ID_BTN_2MIN,  GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74,       UI_BUF_I332001_TIMER_BG_BIN},
    {"03:00",      COMPO_ID_BTN_3MIN,  GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74*2+CONTROL_LENGTH,       UI_BUF_I332001_TIMER_BG_BIN},
    {"05:00",      COMPO_ID_BTN_5MIN,  GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74*2+CONTROL_LENGTH,       UI_BUF_I332001_TIMER_BG_BIN},
    {"10:00",      COMPO_ID_BTN_10MIN, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74*3+CONTROL_LENGTH*2,       UI_BUF_I332001_TIMER_BG_BIN},
    {"30:00",      COMPO_ID_BTN_30MIN, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,   GUI_SCREEN_HEIGHT/2.74*3+CONTROL_LENGTH*2,       UI_BUF_I332001_TIMER_BG_BIN},
};

//item列表，创建时遍历一下
typedef struct timer_btn_item_t_
{
    const char *text;
    u16 txt_id;
    s16 x;
    s16 y;
} timer_btn_item_t;

#define TIMER_BTN_ITEM_CNT  ((int)(sizeof(tbl_timer_btn_item) / sizeof(tbl_timer_btn_item[0])))
static const timer_btn_item_t tbl_timer_btn_item[] =
{
    {"58",         COMPO_ID_SEC_1,      293,                         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING*2},
    {"59",         COMPO_ID_SEC_2,      293,                         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING},
    {"00",         COMPO_ID_SEC_3,      293,                         GUI_SCREEN_CENTER_Y},
    {"01",         COMPO_ID_SEC_4,      293,                         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING},
    {"02",         COMPO_ID_SEC_5,      293,                         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING*2},

    {"58",         COMPO_ID_MIN_1,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING*2},
    {"59",         COMPO_ID_MIN_2,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING},
    {"00",         COMPO_ID_MIN_3,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y},
    {"01",         COMPO_ID_MIN_4,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING},
    {"02",         COMPO_ID_MIN_5,      GUI_SCREEN_CENTER_X,         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING*2},

    {"22",         COMPO_ID_HOUR_1,      72,                         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING*2},
    {"23",         COMPO_ID_HOUR_2,      72,                         GUI_SCREEN_CENTER_Y-TIME_SET_TXT_SPACING},
    {"00",         COMPO_ID_HOUR_3,      72,                         GUI_SCREEN_CENTER_Y},
    {"01",         COMPO_ID_HOUR_4,      72,                         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING},
    {"02",         COMPO_ID_HOUR_5,      72,                         GUI_SCREEN_CENTER_Y+TIME_SET_TXT_SPACING*2},
};

//时间设置初始化
static void func_timer_setting_date_init(void)
{
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;
    char time_setting[5]= {58,59,0,01,02};
    char time_setting_hour[5]= {22,23,0,01,02};

    for(int i=0; i<5; i++)
    {
        f_timer->sec_buf[i]=time_setting[i];
        f_timer->min_buf[i]=time_setting[i];
        f_timer->hour_buf[i]=time_setting_hour[i];
    }
}
//创建定时器窗体，文件内调用
static compo_form_t *func_timer_form_create_by_type(u8 page_type)
{
    u8 hour, min, sec;
    u32 res_addr;
    char str_buff[24];
    compo_button_t *btn;
    compo_textbox_t *txt;

    f_timer_t *f_timer = NULL;
    if (func_cb.sta == FUNC_TIMER)
    {
        f_timer = (f_timer_t*)func_cb.f_cb;
    }


    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    if(page_type != TIMER_PAGE_SELECT)
    {
        compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
        compo_form_set_title(frm, i18n[STR_TIMER]);
    }

    switch (page_type)
    {
        case TIMER_PAGE_SELECT:
        {
            widget_page_t* page = widget_page_create(frm->page_body);
//            widget_page_t* page = frm->page_body;//frm_main->page_body;
            widget_page_set_client(page, 0, 0);
            widget_set_location(page, GUI_SCREEN_CENTER_X,PAGE_HEIGHT/2,GUI_SCREEN_WIDTH,PAGE_HEIGHT);

            //创建文本->按钮
            for (u8 idx = 0; idx < TIMER_TXT_ITEM_CNT; idx++)
            {
                btn = compo_button_create_page_by_image(frm, page, tbl_timer_txt_item[idx].res_addr);
                compo_setid(btn, tbl_timer_txt_item[idx].btn_id);
                compo_button_set_pos(btn, tbl_timer_txt_item[idx].x, tbl_timer_txt_item[idx].y);

                txt = compo_textbox_create_for_page(frm, page, strlen(tbl_timer_txt_item[idx].text));
                compo_textbox_set_pos(txt, tbl_timer_txt_item[idx].x, tbl_timer_txt_item[idx].y);
                compo_textbox_set(txt, tbl_timer_txt_item[idx].text);
            }
            btn = compo_button_create_page_by_image(frm, page, UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN);
            compo_setid(btn, COMPO_ID_BTN_CUSTOM);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/2.74*4+CONTROL_LENGTH);

            txt = compo_textbox_create_for_page(frm, page, strlen(i18n[STR_CUSTOM]));
            compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/2.74*4+CONTROL_LENGTH);
            compo_textbox_set(txt, i18n[STR_CUSTOM]);

            if (func_cb.sta == FUNC_TIMER)
            {
                f_timer->page_sub = page;
            }
            compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
            compo_form_set_title(frm, i18n[STR_TIMER]);
        }
        break;

        case TIMER_PAGE_CUSTOM:
            if (func_cb.sta == FUNC_TIMER)
            {
                func_timer_setting_date_init();
            }
            compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I332001_PUBLIC_KUANG_COLON_BIN);
            compo_picturebox_set_pos(picbox, 125,tbl_timer_btn_item[2].y-TIME_SET_TXT_SPACING_SHIFT );
            picbox = compo_picturebox_create(frm, UI_BUF_I332001_PUBLIC_KUANG_COLON_BIN);
            compo_picturebox_set_pos(picbox, 235,tbl_timer_btn_item[2].y-TIME_SET_TXT_SPACING_SHIFT);

            for(int i=0; i<TIMER_BTN_ITEM_CNT; i++)
            {
                txt = compo_textbox_create(frm, 2);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set(txt,tbl_timer_btn_item[i].text );
                compo_setid(txt,tbl_timer_btn_item[i].txt_id);
                compo_textbox_set_pos(txt,tbl_timer_btn_item[i].x,tbl_timer_btn_item[i].y-TIME_SET_TXT_SPACING_SHIFT);
            }
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN);
            compo_button_set_pos(btn,tbl_timer_btn_item[2].x,tbl_timer_btn_item[2].y -TIME_SET_TXT_SPACING_SHIFT);
            compo_setid(btn,COMPO_ID_SEC_IMG_BTN);

            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN);
            compo_button_set_pos(btn,tbl_timer_btn_item[7].x,tbl_timer_btn_item[7].y -TIME_SET_TXT_SPACING_SHIFT);
            compo_setid(btn,COMPO_ID_MIN_IMG_BTN);

            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN);
            compo_button_set_pos(btn,tbl_timer_btn_item[12].x,tbl_timer_btn_item[12].y -TIME_SET_TXT_SPACING_SHIFT);
            compo_setid(btn,COMPO_ID_HOUR_IMG_BTN);

            compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,tbl_timer_btn_item[0].y-TIME_SET_TXT_SPACING_SHIFT-14,GUI_SCREEN_WIDTH,42);
            compo_shape_set_color(shape, COLOR_BLACK);

            shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,tbl_timer_btn_item[4].y+21-TIME_SET_TXT_SPACING_SHIFT+14,GUI_SCREEN_WIDTH,84);
            compo_shape_set_color(shape, COLOR_BLACK);

            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN);
            compo_setid(btn,COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 266+64/2);

            txt = compo_textbox_create(frm, strlen(i18n[STR_OK]));
            compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 266+64/2);
            compo_textbox_set(txt, i18n[STR_OK]);

            break;

        case TIMER_PAGE_COUNTDOWN:
            //新建按钮
            res_addr = sys_cb.timer_sta == TIMER_STA_WORKING ? UI_BUF_I332001_PUBLIC_PAUSED_SMALL_BIN : (sys_cb.timer_sta == TIMER_STA_DONE ? UI_BUF_I332001_PUBLIC_RETRY_BIN : UI_BUF_I332001_PUBLIC_PLAY_SMALL_BIN);
            btn = compo_button_create_by_image(frm, res_addr); //start/pause/again
            compo_setid(btn, COMPO_ID_BTN_START);
            compo_button_set_pos(btn, 212+80/2, 234+80/2);
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);  //close
            compo_setid(btn, COMPO_ID_BTN_NO);
            compo_button_set_pos(btn, 68+80/2, 234+80/2);
            //新建数字
            hour = SEC_TO_HOUR(sys_cb.timer_left_sec);
            min = SEC_TO_MIN(sys_cb.timer_left_sec);
            sec = SEC_TO_SEC(sys_cb.timer_left_sec);
            txt = compo_textbox_create(frm, 12);
            compo_setid(txt, COMPO_ID_NUM_COUNTDOWN);
            compo_textbox_set_align_center(txt, false);     //左对齐显示
            compo_textbox_set_pos(txt,63, 146);
            compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
            snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
            compo_textbox_set(txt, str_buff);

            break;

        default:
            break;
    }

    return frm;
}

//创建定时器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_timer_form_create(void)
{
    compo_form_t *frm = NULL;
    switch (sys_cb.timer_sta)
    {
        case TIMER_STA_IDLE:
            frm = func_timer_form_create_by_type(TIMER_PAGE_SELECT);
            // printf("11111111111111111111111111111\n");
            break;

        case TIMER_STA_WORKING:
            uteModuleGuiCommonDisplayOffAllowGoBack(false);
        case TIMER_STA_PAUSE:
        case TIMER_STA_DONE:
        case TIMER_STA_RESET:
            // printf("22222222222222222222222222222222222\n");
            frm = func_timer_form_create_by_type(TIMER_PAGE_COUNTDOWN);
            break;

        default:
            break;
    }

    return frm;
}

//触摸按钮效果处理
static void func_timer_button_touch_handle(void)
{
    compo_button_t *btn;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_START:
            btn = compo_getobj_byid(COMPO_ID_BTN_START);
            if (sys_cb.timer_sta == TIMER_STA_WORKING)
            {
//                compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_PAUSED_BIN);
            }
            else if (sys_cb.timer_sta == TIMER_STA_DONE)
            {
                ;
            }
            else
            {
//                compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_PLAY_BIN);
            }
            break;

        case COMPO_ID_BTN_NO:
            btn = compo_getobj_byid(COMPO_ID_BTN_NO);
//            compo_button_set_bgimg(btn, UI_BUF_COMMON_NO_CLICK_BIN);
            break;

        default:
            break;
    }

}

//释放按钮效果处理
static void func_timer_button_release_handle(void)
{
    u32 res_addr;
    compo_button_t *btn;

    if ((btn = compo_getobj_byid(COMPO_ID_BTN_START)) != NULL)
    {
//        res_addr = sys_cb.timer_sta == TIMER_STA_WORKING ? UI_BUF_I330001_PUBLIC_PAUSED_BIN : (sys_cb.timer_sta == TIMER_STA_DONE ? UI_BUF_I330001_PUBLIC_RETRY_BIN : UI_BUF_I330001_PUBLIC_PLAY_BIN);
        compo_button_set_bgimg(btn, res_addr);
    }
    if ((btn = compo_getobj_byid(COMPO_ID_BTN_NO)) != NULL)
    {
//        compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
    }
}

//100ms计时器秒数刷新回调函数
static void timer_100ms_pro(co_timer_t *timer, void *param)
{
//    bool done = false;
    u8 *count = NULL;
    static bool lowpwr_sta_bkp;
    static u8 rtc_cal_cnt_bkp;
    static u32 rtccnt_tmp;
    u16 sec_past;
    bool lowpwr_sta = bsp_system_is_sleep() /*| sys_cb.idle_sta*/;
    static u8 cnt = 0;

    if (param)
    {
        count = (u8 *)param;
    }

    if (count && sys_cb.timer_sta == TIMER_STA_WORKING)    //休眠不计时
    {
        if (++(*count) >= 10)   //1s
        {
            *count = 0;
        }

        if (!lowpwr_sta)
        {
            if (rtccnt_tmp != RTCCNT)
            {
                rtccnt_tmp = RTCCNT;
                *count = 0;
            }
        }
        else            //省电/休眠模式，RTC已休眠
        {
            if (lowpwr_sta_bkp == false)      //初次进入
            {
                rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
            }
            if (rtc_cal_cnt_bkp != sys_cb.rtc_cal_cnt)      //RTC已校准，同步校准
            {
                rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
                rtccnt_tmp = RTCCNT;
                // printf("calibrated!\n");
            }
            else if (*count == 0)
            {
                rtccnt_tmp++;
            }
        }


        sec_past = rtccnt_tmp - sys_cb.timer_start_rtc;
        if (sec_past < sys_cb.timer_total_sec)
        {
            sys_cb.timer_left_sec = sys_cb.timer_total_sec - sec_past;
            // printf("countdown-->[%d/%d][%d %d]\n", sec_past, sys_cb.timer_total_sec, rtccnt_tmp, RTCCNT);
        }
        else
        {
            sys_cb.timer_left_sec = 0;
            // printf(">>>COUNTDOWN_FINISH\n");

            if(func_cb.sta == FUNC_TIMER)
            {
                sys_cb.timer_sta = TIMER_STA_DONE;
            }
            else
            {
                sys_cb.timer_sta = TIMER_STA_IDLE;
            }
            if (sys_cb.timer_done == false)
            {
                sys_cb.timer_done = true;
            }
        }
    }
    lowpwr_sta_bkp = lowpwr_sta;
}

//单击按钮
static void func_timer_button_click(void)
{
    static co_timer_t timer_timer;
    static u8 count_100ms;  //100ms定时器计数，10次为1s，减小暂停误差
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;
    u8 page_next = f_timer->page_disp;
    int id = compo_get_button_id();

    switch (f_timer->page_disp)
    {
        case TIMER_PAGE_SELECT:
            switch (id)
            {
                case COMPO_ID_BTN_1MIN:
                case COMPO_ID_BTN_2MIN:
                case COMPO_ID_BTN_3MIN:
                case COMPO_ID_BTN_5MIN:
                case COMPO_ID_BTN_10MIN:
                case COMPO_ID_BTN_30MIN:
                    sys_cb.timer_custom_sec = sys_cb.timer_left_sec = sys_cb.timer_total_sec = id * 60;
                    sys_cb.timer_sta = TIMER_STA_RESET;
                    page_next = TIMER_PAGE_COUNTDOWN;
                    break;

                case COMPO_ID_BTN_CUSTOM:
                    f_timer->hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                    f_timer->min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                    f_timer->sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                    page_next = TIMER_PAGE_CUSTOM;
                    break;

                default:
                    break;
            }
            break;

        case TIMER_PAGE_CUSTOM:
            switch (id)
            {
                case COMPO_ID_BTN_OK:

                    f_timer->hour = f_timer->hour_buf[2];
                    f_timer->min  = f_timer->min_buf[2];
                    f_timer->sec  = f_timer->sec_buf[2];

                    sys_cb.timer_left_sec = sys_cb.timer_total_sec = sys_cb.timer_custom_sec = f_timer->hour * 3600 + f_timer->min * 60 + f_timer->sec;
                    sys_cb.timer_sta = TIMER_STA_RESET;
                    page_next = TIMER_PAGE_COUNTDOWN;
                    break;

                default:
                    break;
            }
            break;

        case TIMER_PAGE_COUNTDOWN:
            switch (id)
            {
                case COMPO_ID_BTN_START:
//                    printf("COMPO_ID_BTN_START %d\n",sys_cb.timer_sta);
                    if (sys_cb.timer_sta == TIMER_STA_WORKING)
                    {
                        sys_cb.timer_sta = TIMER_STA_PAUSE;
                        uteModuleGuiCommonDisplayOffAllowGoBack(true);
                    }
                    else if (sys_cb.timer_sta == TIMER_STA_DONE)
                    {
                        sys_cb.timer_start_rtc = compo_cb.rtc_cnt;
                        count_100ms = 0;
                        co_timer_set(&timer_timer, 100, TIMER_REPEAT, LEVEL_LOW_PRI, timer_100ms_pro, &count_100ms);
//                        count_100ms = 0;
//                        sys_cb.timer_left_sec = sys_cb.timer_total_sec;
//                        sys_cb.timer_sta = TIMER_STA_RESET;
//                        uteModuleGuiCommonDisplayOffAllowGoBack(true);
                        uteModuleGuiCommonDisplayOffAllowGoBack(false);
                        sys_cb.timer_sta = TIMER_STA_WORKING;
                    }
                    else
                    {
                        if (sys_cb.timer_sta == TIMER_STA_RESET)
                        {
                            sys_cb.timer_start_rtc = compo_cb.rtc_cnt;
                            count_100ms = 0;
                            co_timer_set(&timer_timer, 100, TIMER_REPEAT, LEVEL_LOW_PRI, timer_100ms_pro, &count_100ms);
                        }
                        sys_cb.timer_sta = TIMER_STA_WORKING;
                        uteModuleGuiCommonDisplayOffAllowGoBack(false);
                    }
                    break;

                case COMPO_ID_BTN_NO:
                    co_timer_del(&timer_timer);
                    sys_cb.timer_sta = TIMER_STA_IDLE;
                    sys_cb.timer_left_sec = sys_cb.timer_total_sec = 0;
                    page_next = TIMER_PAGE_SELECT;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    func_timer_button_release_handle();

    if (page_next != f_timer->page_disp)
    {
        f_timer->old_y = 0;
        f_timer->moveto.y = 0;
        f_timer->page_disp = page_next;
        compo_form_destroy(func_cb.frm_main);
        func_cb.frm_main = func_timer_form_create_by_type(f_timer->page_disp);
    }

//    TRACE("page[%d] sta[%d]\n", f_timer->page_disp, sys_cb.timer_sta);
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//定时器功能事件处理
static void func_timer_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    u8 hour, min, sec;
    char str_buff[24];
    compo_textbox_t *txt_num;
    compo_button_t *btn;
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;

    switch (f_timer->page_disp)
    {
        case TIMER_PAGE_CUSTOM:
            if(f_timer->touch_flag  == true)
            {
                s32 dy=0,dx=0;
                f_timer->touch_flag = ctp_get_dxy(&dx, &dy);

                if(f_timer->touch_flag == true)
                {
                    f_timer->old_y = ((int)(dy/TIME_SET_TXT_SPACING))*TIME_SET_TXT_SPACING;
                    f_timer->moveto.y = dy-f_timer->old_y;

                    switch(f_timer->set_time_flag)
                    {
                        case 3:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] =60+f_timer->sec_buf[i]-(f_timer->old_y/TIME_SET_TXT_SPACING);
                                f_timer->set_time_buf[i] =f_timer->set_time_buf[i]%60;

                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_SEC_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[i].x,tbl_timer_btn_item[i].y-TIME_SET_TXT_SPACING_SHIFT+f_timer->moveto.y);
                                memset(str_buff,0,sizeof(str_buff));
                                snprintf(str_buff,sizeof(str_buff),"%02d",f_timer->set_time_buf[i]);
                                compo_textbox_set(txt,str_buff);
                            }
                            break;
                        case 2:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] =60+f_timer->min_buf[i]-(f_timer->old_y/TIME_SET_TXT_SPACING);
                                f_timer->set_time_buf[i] =f_timer->set_time_buf[i]%60;

                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_MIN_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[i+5].x,tbl_timer_btn_item[i+5].y-TIME_SET_TXT_SPACING_SHIFT+f_timer->moveto.y);
                                memset(str_buff,0,sizeof(str_buff));
                                snprintf(str_buff,sizeof(str_buff),"%02d",f_timer->set_time_buf[i]);
                                compo_textbox_set(txt,str_buff);
                            }
                            break;
                        case 1:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] =24+f_timer->hour_buf[i]-(f_timer->old_y/TIME_SET_TXT_SPACING);
                                f_timer->set_time_buf[i] =f_timer->set_time_buf[i]%24;

                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_HOUR_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[i+10].x,tbl_timer_btn_item[i+10].y-TIME_SET_TXT_SPACING_SHIFT+f_timer->moveto.y);
                                memset(str_buff,0,sizeof(str_buff));
                                snprintf(str_buff,sizeof(str_buff),"%02d",f_timer->set_time_buf[i]);
                                compo_textbox_set(txt,str_buff);
                            }
                            break;
                    }
                }
                else
                {
                    for(int i=0; i<5; i++)
                    {
                        switch(f_timer->set_time_flag)
                        {
                            case 3:
                            {
                                f_timer->sec_buf[i] = f_timer->set_time_buf[i];
                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_SEC_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[i].x,tbl_timer_btn_item[i].y-TIME_SET_TXT_SPACING_SHIFT);
                            }
                            break;
                            case 2:
                            {
                                f_timer->min_buf[i] = f_timer->set_time_buf[i];
                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_MIN_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[5+i].x,tbl_timer_btn_item[5+i].y-TIME_SET_TXT_SPACING_SHIFT);
                            }
                            break;
                            case 1:
                            {
                                f_timer->hour_buf[i] = f_timer->set_time_buf[i];
                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_HOUR_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[10+i].x,tbl_timer_btn_item[10+i].y-TIME_SET_TXT_SPACING_SHIFT);
                            }
                            break;
                        }


                    }
                }
            }
            break;

        case TIMER_PAGE_COUNTDOWN:
            hour = SEC_TO_HOUR(sys_cb.timer_left_sec);
            min = SEC_TO_MIN(sys_cb.timer_left_sec);
            sec = SEC_TO_SEC(sys_cb.timer_left_sec);
            if (f_timer->sec != sec || f_timer->min != min || f_timer->hour != hour)
            {
                f_timer->sec = sec;
                f_timer->min = min;
                f_timer->hour = hour;
                txt_num = compo_getobj_byid(COMPO_ID_NUM_COUNTDOWN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", f_timer->hour, f_timer->min, f_timer->sec);
                compo_textbox_set(txt_num, str_buff);
            }
            if (sys_cb.timer_sta == TIMER_STA_DONE)
            {
                sys_cb.timer_left_sec = sys_cb.timer_total_sec;
                hour = SEC_TO_HOUR(sys_cb.timer_left_sec);
                min = SEC_TO_MIN(sys_cb.timer_left_sec);
                sec = SEC_TO_SEC(sys_cb.timer_left_sec);


                btn = compo_getobj_byid(COMPO_ID_BTN_START);
                compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RETRY_BIN);

                txt_num = compo_getobj_byid(COMPO_ID_NUM_COUNTDOWN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", f_timer->hour, f_timer->min, f_timer->sec);
                compo_textbox_set(txt_num, str_buff);
            }
            break;
        case TIMER_PAGE_SELECT:
            if(f_timer->touch_flag  == true)
            {

                s32 dy=0,dx=0;

                f_timer->touch_flag = ctp_get_dxy(&dx, &dy);
                if(f_timer->touch_flag == true)
                {
                    f_timer->moveto.y = f_timer->old_y+dy;
                    if(f_timer->moveto.y > -44 )
                    {
                        f_timer->moveto.y = -44;
                    }
                    else if(f_timer->moveto.y < PAGE_LOCATION_LAST)
                    {
                        f_timer->moveto.y = PAGE_LOCATION_LAST;
                    }

                    widget_page_set_client(f_timer->page_sub, 0, f_timer->moveto.y);

                }
                else
                {
                    f_timer->old_y = f_timer->moveto.y;
                }


            }
            break;

        default:
            break;
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    u8 hour, min, sec;
    char str_buff[24];
    compo_textbox_t *txt_num;
    compo_button_t *btn;
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;

    switch (f_timer->page_disp)
    {
        case TIMER_PAGE_CUSTOM:
            if(f_timer->touch_flag  == true)
            {
                s32 dy=0,dx=0;
                f_timer->touch_flag = ctp_get_dxy(&dx, &dy);

                if(f_timer->touch_flag == true)
                {
                    f_timer->old_y = ((int)(dy/TIME_SET_TXT_SPACING))*TIME_SET_TXT_SPACING;
                    f_timer->moveto.y = dy-f_timer->old_y;

                    switch(f_timer->set_time_flag)
                    {
                        case 3:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] =60+f_timer->sec_buf[i]-(f_timer->old_y/TIME_SET_TXT_SPACING);
                                f_timer->set_time_buf[i] =f_timer->set_time_buf[i]%60;

                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_SEC_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[i].x,tbl_timer_btn_item[i].y-TIME_SET_TXT_SPACING_SHIFT+f_timer->moveto.y);
                                memset(str_buff,0,sizeof(str_buff));
                                snprintf(str_buff,sizeof(str_buff),"%02d",f_timer->set_time_buf[i]);
                                compo_textbox_set(txt,str_buff);
                            }
                            break;
                        case 2:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] =60+f_timer->min_buf[i]-(f_timer->old_y/TIME_SET_TXT_SPACING);
                                f_timer->set_time_buf[i] =f_timer->set_time_buf[i]%60;

                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_MIN_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[i+5].x,tbl_timer_btn_item[i+5].y-TIME_SET_TXT_SPACING_SHIFT+f_timer->moveto.y);
                                memset(str_buff,0,sizeof(str_buff));
                                snprintf(str_buff,sizeof(str_buff),"%02d",f_timer->set_time_buf[i]);
                                compo_textbox_set(txt,str_buff);
                            }
                            break;
                        case 1:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] =24+f_timer->hour_buf[i]-(f_timer->old_y/TIME_SET_TXT_SPACING);
                                f_timer->set_time_buf[i] =f_timer->set_time_buf[i]%24;

                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_HOUR_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[i+10].x,tbl_timer_btn_item[i+10].y-TIME_SET_TXT_SPACING_SHIFT+f_timer->moveto.y);
                                memset(str_buff,0,sizeof(str_buff));
                                snprintf(str_buff,sizeof(str_buff),"%02d",f_timer->set_time_buf[i]);
                                compo_textbox_set(txt,str_buff);
                            }
                            break;
                    }
                }
                else
                {
                    for(int i=0; i<5; i++)
                    {
                        switch(f_timer->set_time_flag)
                        {
                            case 3:
                            {
                                f_timer->sec_buf[i] = f_timer->set_time_buf[i];
                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_SEC_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[i].x,tbl_timer_btn_item[i].y-TIME_SET_TXT_SPACING_SHIFT);
                            }
                            break;
                            case 2:
                            {
                                f_timer->min_buf[i] = f_timer->set_time_buf[i];
                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_MIN_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[5+i].x,tbl_timer_btn_item[5+i].y-TIME_SET_TXT_SPACING_SHIFT);
                            }
                            break;
                            case 1:
                            {
                                f_timer->hour_buf[i] = f_timer->set_time_buf[i];
                                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_HOUR_1+i);
                                compo_textbox_set_pos(txt,tbl_timer_btn_item[10+i].x,tbl_timer_btn_item[10+i].y-TIME_SET_TXT_SPACING_SHIFT);
                            }
                            break;
                        }


                    }
                }
            }
            break;

        case TIMER_PAGE_COUNTDOWN:
            hour = SEC_TO_HOUR(sys_cb.timer_left_sec);
            min = SEC_TO_MIN(sys_cb.timer_left_sec);
            sec = SEC_TO_SEC(sys_cb.timer_left_sec);
            if (f_timer->sec != sec || f_timer->min != min || f_timer->hour != hour)
            {
                f_timer->sec = sec;
                f_timer->min = min;
                f_timer->hour = hour;
                txt_num = compo_getobj_byid(COMPO_ID_NUM_COUNTDOWN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", f_timer->hour, f_timer->min, f_timer->sec);
                compo_textbox_set(txt_num, str_buff);
            }
            if (sys_cb.timer_sta == TIMER_STA_DONE)
            {
                sys_cb.timer_left_sec = sys_cb.timer_total_sec;
                hour = SEC_TO_HOUR(sys_cb.timer_left_sec);
                min = SEC_TO_MIN(sys_cb.timer_left_sec);
                sec = SEC_TO_SEC(sys_cb.timer_left_sec);


                btn = compo_getobj_byid(COMPO_ID_BTN_START);
                compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RETRY_BIN);

                txt_num = compo_getobj_byid(COMPO_ID_NUM_COUNTDOWN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", f_timer->hour, f_timer->min, f_timer->sec);
                compo_textbox_set(txt_num, str_buff);
            }
            break;
        case TIMER_PAGE_SELECT:
            if(f_timer->touch_flag  == true)
            {

                s32 dy=0,dx=0;

                f_timer->touch_flag = ctp_get_dxy(&dx, &dy);
                if(f_timer->touch_flag == true)
                {
                    f_timer->moveto.y = f_timer->old_y+dy;
                    if(f_timer->moveto.y > 0 )
                    {
                        f_timer->moveto.y = 0;
                    }
                    else if(f_timer->moveto.y < PAGE_LOCATION_LAST)
                    {
                        f_timer->moveto.y = PAGE_LOCATION_LAST;
                    }

                    widget_page_set_client(f_timer->page_sub, 0, f_timer->moveto.y);

                }
                else
                {
                    f_timer->old_y = f_timer->moveto.y;
                }
                printf("f_timer->moveto.y:%d\n",f_timer->moveto.y);
            }
            break;

        default:
            break;
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_process();
}

//定时器功能消息处理
static void func_timer_message(size_msg_t msg)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:

            if(f_timer->page_disp  == TIMER_PAGE_SELECT)
            {
                f_timer->touch_flag = true;
            }
            else if(f_timer->page_disp  == TIMER_PAGE_CUSTOM)
            {
                int id = compo_get_button_id();
                compo_button_t *btn_sec  = compo_getobj_byid(COMPO_ID_SEC_IMG_BTN);
                compo_button_t *btn_min  = compo_getobj_byid(COMPO_ID_MIN_IMG_BTN);
                compo_button_t *btn_hour = compo_getobj_byid(COMPO_ID_HOUR_IMG_BTN);

                if(f_timer->touch_flag == false)
                {
                    switch(id)
                    {
                        case COMPO_ID_HOUR_IMG_BTN:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] = f_timer->hour_buf[i];
                            }
                            compo_button_set_bgimg(btn_hour,UI_BUF_I330001_TIMER_BOX00_BIN );
                            compo_button_set_bgimg(btn_min,UI_BUF_I330001_TIMER_BOX01_BIN );
                            compo_button_set_bgimg(btn_sec,UI_BUF_I330001_TIMER_BOX01_BIN );
                            f_timer->set_time_flag = 1;
                            break;
                        case COMPO_ID_MIN_IMG_BTN:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] = f_timer->min_buf[i];
                            }
                            compo_button_set_bgimg(btn_hour,UI_BUF_I330001_TIMER_BOX01_BIN );
                            compo_button_set_bgimg(btn_min,UI_BUF_I330001_TIMER_BOX00_BIN );
                            compo_button_set_bgimg(btn_sec,UI_BUF_I330001_TIMER_BOX01_BIN );
                            f_timer->set_time_flag = 2;
                            break;
                        case COMPO_ID_SEC_IMG_BTN:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] = f_timer->sec_buf[i];
                            }
                            compo_button_set_bgimg(btn_hour,UI_BUF_I330001_TIMER_BOX01_BIN );
                            compo_button_set_bgimg(btn_min,UI_BUF_I330001_TIMER_BOX01_BIN );
                            compo_button_set_bgimg(btn_sec,UI_BUF_I330001_TIMER_BOX00_BIN );
                            f_timer->set_time_flag = 3;
                            break;
                    }
                }
                f_timer->touch_flag = true;
            }
            func_timer_button_touch_handle();

            break;

        case MSG_CTP_CLICK:
            func_timer_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_timer_button_release_handle();
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_timer_button_release_handle();
            func_message(msg);
            break;

        default:
            func_message(msg);
            break;
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:

            if(f_timer->page_disp  == TIMER_PAGE_SELECT)
            {
                f_timer->touch_flag = true;
            }
            else if(f_timer->page_disp  == TIMER_PAGE_CUSTOM)
            {
                int id = compo_get_button_id();
                compo_button_t *btn_sec  = compo_getobj_byid(COMPO_ID_SEC_IMG_BTN);
                compo_button_t *btn_min  = compo_getobj_byid(COMPO_ID_MIN_IMG_BTN);
                compo_button_t *btn_hour = compo_getobj_byid(COMPO_ID_HOUR_IMG_BTN);

                if(f_timer->touch_flag == false)
                {
                    switch(id)
                    {
                        case COMPO_ID_HOUR_IMG_BTN:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] = f_timer->hour_buf[i];
                            }
                            compo_button_set_bgimg(btn_hour,UI_BUF_I332001_PUBLIC_KUANG_YELLOW_SMALL_BIN );
                            compo_button_set_bgimg(btn_min,UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN );
                            compo_button_set_bgimg(btn_sec, UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN);
                            f_timer->set_time_flag = 1;
                            break;
                        case COMPO_ID_MIN_IMG_BTN:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] = f_timer->min_buf[i];
                            }
                            compo_button_set_bgimg(btn_hour,UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN );
                            compo_button_set_bgimg(btn_min,UI_BUF_I332001_PUBLIC_KUANG_YELLOW_SMALL_BIN );
                            compo_button_set_bgimg(btn_sec,UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN );
                            f_timer->set_time_flag = 2;
                            break;
                        case COMPO_ID_SEC_IMG_BTN:
                            for(int i=0; i<5; i++)
                            {
                                f_timer->set_time_buf[i] = f_timer->sec_buf[i];
                            }
                            compo_button_set_bgimg(btn_hour,UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN );
                            compo_button_set_bgimg(btn_min,UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN );
                            compo_button_set_bgimg(btn_sec,UI_BUF_I332001_PUBLIC_KUANG_YELLOW_SMALL_BIN );
                            f_timer->set_time_flag = 3;
                            break;
                    }
                }
                f_timer->touch_flag = true;
            }
            func_timer_button_touch_handle();

            break;

        case MSG_CTP_CLICK:
            func_timer_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_timer_button_release_handle();
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_timer_button_release_handle();
            func_message(msg);
            break;

        default:
            func_message(msg);
            break;
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//进入定时器功能
static void func_timer_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_timer_t));
    func_cb.frm_main = func_timer_form_create();
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;
    f_timer->page_disp = sys_cb.timer_sta == TIMER_STA_IDLE ? TIMER_PAGE_SELECT : TIMER_PAGE_COUNTDOWN;
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    f_timer_t *f_timer = (f_timer_t*)func_cb.f_cb;
    f_timer->page_disp = sys_cb.timer_sta == TIMER_STA_IDLE ? TIMER_PAGE_SELECT : TIMER_PAGE_COUNTDOWN;
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//退出定时器功能
static void func_timer_exit(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_cb.last = FUNC_TIMER;
}

//定时器功能
void func_timer(void)
{
    printf("%s\n", __func__);
    func_timer_enter();
    while (func_cb.sta == FUNC_TIMER)
    {
        func_timer_process();
        func_timer_message(msg_dequeue());
    }
    func_timer_exit();
}

#endif
