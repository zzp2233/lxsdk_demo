#include "include.h"
#include "func.h"
#include "ute_module_sleep.h"

#define TRACE_EN   1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SLEEP_SUPPORT


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
#define FINT_HEIGHT 24
#define GUI_PAGE_HEAD_HEIGHT            (GUI_SCREEN_HEIGHT / 8)
#define GUI_PAGE_BODY_HEIGHT            (GUI_SCREEN_HEIGHT - GUI_PAGE_HEAD_HEIGHT)
#define GUI_PAGE_BODY_CENTER_Y          (GUI_PAGE_HEAD_HEIGHT + GUI_PAGE_BODY_HEIGHT / 2)
#define GUI_PAGE_TITLE_WIDTH            (GUI_SCREEN_WIDTH * 2 / 5)
#define GUI_PAGE_TIME_WIDTH             (GUI_SCREEN_WIDTH - FORM_TITLE_RIGHT - GUI_SCREEN_WIDTH/12)//(GUI_SCREEN_WIDTH / 3)
#define CONTROL_Y  16

enum
{
    COMPO_ID_TEXT_DEEP_TIME_HOUR = 1,
    COMPO_ID_TEXT_DEEP_TIME_MIN,
    COMPO_ID_TEXT_LIGHT_TIME_HOUR,
    COMPO_ID_TEXT_LIGHT_TIME_MIN,

    COMPO_ID_CHART_SHALLOW,
    COMPO_ID_CHART_DEEP,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

enum
{
    CHART_ANIM_STA_IDLE,
    CHART_ANIM_STA_START,
    CHART_ANIM_STA_END,

};

typedef struct f_sleep_t_
{
    page_tp_move_t *ptm;
} f_sleep_t;

//创建睡眠窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sleep_form_create(void)
{
    compo_textbox_t * txt;
    compo_picturebox_t *pic;
    char buf[50];

    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);

//    sleep_data->totalSleepMin = 480;
//    sleep_data->fallAsSleepTime.hour =0;
//    sleep_data->fallAsSleepTime.min =0;
//    sleep_data->getUpSleepTime.hour =8;
//    sleep_data->getUpSleepTime.min = 0;

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
//    compo_form_set_title(frm, i18n[STR_SLEEP]);
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_SLEEP]));
//    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
    compo_textbox_set(txt, i18n[STR_SLEEP]);
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
    compo_textbox_set(txt, i18n[STR_SLEEP]);
    compo_textbox_set_align_center(txt, false);

    ///设置标题栏时间///
//    compo_textbox_t* time_title = compo_textbox_create(frm, 32);
//    compo_textbox_set_align_center(time_title, false);
//    compo_textbox_set_location(time_title, 168, 15, 52, 23);
//    compo_setid(time_title, COMPO_ID_TXT_TIME);
//    compo_bonddata(time_title, COMPO_BOND_HOURMIN_TXT);
    memset(buf,0,sizeof(buf));
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--");///* 总睡眠小时*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.7-CONTROL_Y);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_HOUR]));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.7-4,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.5-FINT_HEIGHT/2-CONTROL_Y,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_HOUR]);

    memset(buf,0,sizeof(buf));
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--");///* 总睡眠分钟*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/7-10,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.7-CONTROL_Y);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_MIN]));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2.4-14,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.5-FINT_HEIGHT/2-CONTROL_Y,FINT_HEIGHT*2,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_MIN]);

    memset(buf,0,sizeof(buf));
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d:%02d - %02d:%02d", sleep_data->fallAsSleepTime.hour,sleep_data->fallAsSleepTime.min,sleep_data->getUpSleepTime.hour,sleep_data->getUpSleepTime.min);///* 睡-起 点*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--:-- - --:--");///* 睡-起 点*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/3+6,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y);
    compo_textbox_set(txt, buf);

    ///* 第二页*/
    if(sleep_data->totalSleepMin == 0) ///是否有睡眠时长
    {
        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_DATA]));
        compo_textbox_set(txt, i18n[STR_NO_DATA]);
        compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y/3+GUI_SCREEN_HEIGHT);
    }

//    sleep_data->fallAsSleepTime.hour=30;
//    sleep_data->fallAsSleepTime.min=30;
//    sleep_data->totalSleepMin = 30;
//    area_t pic_size = gui_image_get_size(UI_BUF_SLEEP_SLEEP_DATE_BG_BIN);
//    compo_form_add_image(frm, UI_BUF_SLEEP_SLEEP_DATE_BG_BIN, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4);///* 背景图*/
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->fallAsSleepTime.hour,sleep_data->fallAsSleepTime.min);///* 睡眠初始点*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--:--");///* 睡眠初始点*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set(txt, buf);
    compo_textbox_set_pos(txt, 8+widget_text_get_area(txt->txt).wid/2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+108/2-4);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);

    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->getUpSleepTime.hour,sleep_data->getUpSleepTime.min);///* 睡眠结束点*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--:--");///* 睡眠结束点*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set(txt, buf);
    compo_textbox_set_pos(txt, GUI_SCREEN_WIDTH-8-widget_text_get_area(txt->txt).wid/2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+108/2-4);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_DOT1_BIN);///* 深度睡眠图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.1, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_DOT2_BIN);///* 浅度睡眠图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/10, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_DOT3_BIN);///* 清醒睡眠图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/10, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*3);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_DOT4_BIN);///* 快速眼动睡眠图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.1, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*3);

    txt = compo_textbox_create(frm,strlen(i18n[STR_DEEP_SLEEP]));///* 深睡*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8-widget_text_get_height()/2,86,32);
    compo_textbox_set(txt, i18n[STR_DEEP_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x69, 0x7e, 0xff));

    txt = compo_textbox_create(frm,strlen(i18n[STR_LIGHT_SLEEP]));///* 浅睡*
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8-widget_text_get_height()/2,86,32);
    compo_textbox_set(txt, i18n[STR_LIGHT_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x36, 0xb7, 0xff));

    txt = compo_textbox_create(frm,strlen(i18n[STR_ALWAKE_SLEEP]));///* 清醒*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*3-widget_text_get_height()/2,86,32);
    compo_textbox_set(txt, i18n[STR_ALWAKE_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0xff, 0x87, 0x0f));

    txt = compo_textbox_create(frm,strlen(i18n[STR_EYE_MOVE_SLEEP]));///* 快速眼动*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*3-widget_text_get_height()/2,90,32);
    compo_textbox_set(txt, i18n[STR_EYE_MOVE_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x00, 0xf7, 0xd6));
//////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s%02d%s",sleep_data->deepSleepMin/60,i18n[STR_HOUR], sleep_data->deepSleepMin%60,i18n[STR_MIN]);///* 深睡数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()-widget_text_get_height()/3,90,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s%02d%s",sleep_data->lightSleepMin/60,i18n[STR_HOUR], sleep_data->lightSleepMin%60,i18n[STR_MIN]);///* 浅睡数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()-widget_text_get_height()/3,90,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s%02d%s",sleep_data->wakeSleepMin/60,i18n[STR_HOUR], sleep_data->wakeSleepMin%60,i18n[STR_MIN]);///* 清醒数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*4-widget_text_get_height()/3,90,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s%02d%s",sleep_data->RemSleepMin/60,i18n[STR_HOUR], sleep_data->RemSleepMin%60,i18n[STR_MIN]);///* 快速眼动数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*4-widget_text_get_height()/3,90,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    uint16_t width = 236;    // 总宽度
    uint8_t fillRect_h = 21; // 绘制单个高度
    uint16_t startX = 0;     // 相对坐标
    uint16_t fillRect_y = GUI_SCREEN_HEIGHT + GUI_SCREEN_HEIGHT / 4.1+10;

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, fillRect_y + fillRect_h+12, width, 1);
    compo_shape_set_color(shape, make_color(0X3D,0X3D,0X3D));

//    sleep_data->recordCnt = 5;
//    sleep_data->sleep_record[1].state = LIGHT_SLEEP;
//    sleep_data->sleep_record[0].state = AWAKE_SLEEP;
//    sleep_data->sleep_record[3].state = AWAKE_SLEEP;
//    sleep_data->sleep_record[2].state = REM_SLEEP;
//    sleep_data->sleep_record[4].state = DEEP_SLEEP;
//    sleep_data->sleep_record[0].period = 1;
//    sleep_data->sleep_record[1].period = 100;
//    sleep_data->sleep_record[2].period = 120;
//    sleep_data->sleep_record[3].period = 119;
//    sleep_data->sleep_record[4].period = 20;

    for(int k=0; k<sleep_data->recordCnt; k++)
    {
        TRACE("SLEEP={state[%d] data[%d]}\n",sleep_data->sleep_record[k].state,sleep_data->sleep_record[k].period);
    }

    if (sleep_data->totalSleepMin) //是否有睡眠时长
    {
        uint16_t fillRect_x = 0;
        uint16_t startTime = sleep_data->fallAsSleepTime.hour * 60 + sleep_data->fallAsSleepTime.min; // 终止时间23:00
        uint16_t endTime = sleep_data->getUpSleepTime.hour * 60 + sleep_data->getUpSleepTime.min;     // 起始时间8:00
        uint16_t timeInterval = (endTime + 24 * 60 - startTime) % (24 * 60);                          // 总时间间隔（分钟）
        if (timeInterval == 0)
            timeInterval = 24 * 60;
        widget_page_t *page = widget_page_create(frm->page_body);
        widget_set_location(page, GUI_SCREEN_CENTER_X, fillRect_y-21/2, width, 21*4);

//        void *rect = widget_rect_create(page);
//        widget_set_location(rect, width/2, 21*2, width, 21*4);

        fillRect_y = 21*3-11;
        for (uint16_t i = 0; i < sleep_data->recordCnt; i++)
        {
//            compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); // 创建显示块矩形
            compo_shape_t *shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);// 创建显示块矩形

            uint16_t fillRect_w = (width * sleep_data->sleep_record[i].period) / timeInterval; // 当前段的宽度

            // 先检查是否超出总宽度
            if (fillRect_x + fillRect_w > width)
            {
                fillRect_w = width - fillRect_x;
                if (i < sleep_data->recordCnt - 1)
                {
                    TRACE("Warning===>i=%d,recordCnt=%dfillRect_x=%d,fillRect_w=%d,width=%d\n",i,sleep_data->recordCnt,fillRect_x,fillRect_w,width);
                }
            }

            // 最后一段画满图表
            // if (i == sleep_data->recordCnt - 1)
            // {
            //     if (fillRect_x + fillRect_w < width)
            //     {
            //         fillRect_w = width - fillRect_x;
            //     }
            // }

            // 不为0的数据最少显示1像素宽
            if (fillRect_w < 1 && sleep_data->sleep_record[i].period != 0)
            {
                fillRect_w = 1;
            }

            // 设置矩形位置和颜色
            if (sleep_data->sleep_record[i].state == DEEP_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y + fillRect_h, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0x69, 0x7e, 0xff));
            }
            else if (sleep_data->sleep_record[i].state == LIGHT_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0x36, 0xb7, 0xff));
            }
            else if (sleep_data->sleep_record[i].state == AWAKE_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y - fillRect_h * 2, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0xff, 0x87, 0x0f));
            }
            else if (sleep_data->sleep_record[i].state == REM_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y - fillRect_h, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0x00, 0xf7, 0xd6));
            }

            // 更新起始点
            fillRect_x += fillRect_w;

            TRACE("i=%d, fillRect_x=%u, fillRect_w=%u\n", i, fillRect_x, fillRect_w);
        }
        widget_page_set_client(page,(width-fillRect_x)/2, 0);
    }

    ab_free(sleep_data);
    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define FINT_HEIGHT 24
#define GUI_PAGE_HEAD_HEIGHT            (GUI_SCREEN_HEIGHT / 8)
#define GUI_PAGE_BODY_HEIGHT            (GUI_SCREEN_HEIGHT - GUI_PAGE_HEAD_HEIGHT)
#define GUI_PAGE_BODY_CENTER_Y          (GUI_PAGE_HEAD_HEIGHT + GUI_PAGE_BODY_HEIGHT / 2)
#define GUI_PAGE_TITLE_WIDTH            (GUI_SCREEN_WIDTH * 2 / 5)
#define GUI_PAGE_TIME_WIDTH             (GUI_SCREEN_WIDTH - FORM_TITLE_RIGHT - GUI_SCREEN_WIDTH/12)//(GUI_SCREEN_WIDTH / 3)
#define CONTROL_Y  16

enum
{
    COMPO_ID_TEXT_DEEP_TIME_HOUR = 1,
    COMPO_ID_TEXT_DEEP_TIME_MIN,
    COMPO_ID_TEXT_LIGHT_TIME_HOUR,
    COMPO_ID_TEXT_LIGHT_TIME_MIN,

    COMPO_ID_CHART_SHALLOW,
    COMPO_ID_CHART_DEEP,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

enum
{
    CHART_ANIM_STA_IDLE,
    CHART_ANIM_STA_START,
    CHART_ANIM_STA_END,

};
typedef struct f_sleep_t_
{
    bool        touch_flag;
    s32         move_offset;
    s32         page_old_y;
    u8          touch_state;
    u8          page_num;
    uint32_t    tick;
    u8          switch_page_state;
} f_sleep_t;

enum
{
    TOUCH_FINISH_STATE=0,
    AUTO_STATE,

};
enum
{
    PAGE_1=0,
    PAGE_2,
};
enum
{
    SWITCH_YES=0,
    SWITCH_NO,
    TOTCH_MOVE,
};

//创建睡眠窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sleep_form_create(void)
{
    compo_textbox_t * txt;
    compo_picturebox_t *pic;
    char buf[50];

    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);

    // sleep_data->totalSleepMin = 480;
    // sleep_data->fallAsSleepTime.hour =0;
    // sleep_data->fallAsSleepTime.min =0;
    // sleep_data->getUpSleepTime.hour =8;
    // sleep_data->getUpSleepTime.min = 0;

    // sleep_data->recordCnt = 5;
    // sleep_data->sleep_record[1].state = LIGHT_SLEEP;
    // sleep_data->sleep_record[0].state = AWAKE_SLEEP;
    // sleep_data->sleep_record[3].state = AWAKE_SLEEP;
    // sleep_data->sleep_record[2].state = REM_SLEEP;
    // sleep_data->sleep_record[4].state = DEEP_SLEEP;
    // sleep_data->sleep_record[0].period = 20;
    // sleep_data->sleep_record[1].period = 100;
    // sleep_data->sleep_record[2].period = 100;
    // sleep_data->sleep_record[3].period = 120;
    // sleep_data->sleep_record[4].period = 20;

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
//    compo_form_set_title(frm, i18n[STR_SLEEP]);
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);

    pic = compo_picturebox_create(frm, UI_BUF_I332001_SLEEP_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    memset(buf,0,sizeof(buf));
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--");///* 总睡眠小时*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(txt,83,229,58,65);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_HOUR]));
    compo_textbox_set_location(txt,145,258,30,34);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set(txt, i18n[STR_HOUR]);

    memset(buf,0,sizeof(buf));
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--");///* 总睡眠分钟*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(txt,187,229,58,65);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_MIN]));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,248,258,30,34);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_MIN]);

    memset(buf,0,sizeof(buf));
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d:%02d - %02d:%02d", sleep_data->fallAsSleepTime.hour,sleep_data->fallAsSleepTime.min,sleep_data->getUpSleepTime.hour,sleep_data->getUpSleepTime.min);///* 睡-起 点*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--:-- - --:--");///* 睡-起 点*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,292,140,30);
    compo_textbox_set(txt, buf);

    ///* 第二页*/

    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_SLEEP]));
    compo_textbox_set(txt, i18n[STR_SLEEP]);
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/2,390+38/2,120,38);
    compo_textbox_set(txt, i18n[STR_SLEEP]);

    if(sleep_data->totalSleepMin == 0) ///是否有睡眠时长
    {
        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_DATA]));
        compo_textbox_set(txt, i18n[STR_NO_DATA]);
        compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,142+GUI_SCREEN_HEIGHT);
    }

    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->fallAsSleepTime.hour,sleep_data->fallAsSleepTime.min);///* 睡眠初始点*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--:--");///* 睡眠初始点*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_location(txt, 63,GUI_SCREEN_HEIGHT+206,47,24);
    compo_textbox_set(txt, buf);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);

    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->getUpSleepTime.hour,sleep_data->getUpSleepTime.min);///* 睡眠结束点*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--:--");///* 睡眠结束点*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_location(txt, 295,GUI_SCREEN_HEIGHT+203,47,24);
    compo_textbox_set(txt, buf);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);

    pic = compo_picturebox_create(frm, UI_BUF_I332001_SLEEP_DOT1_BIN);///* 深度睡眠图*/
    compo_picturebox_set_pos(pic, 45, GUI_SCREEN_HEIGHT+253);

    pic = compo_picturebox_create(frm, UI_BUF_I332001_SLEEP_DOT2_BIN);///* 浅度睡眠图*/
    compo_picturebox_set_pos(pic, 45, GUI_SCREEN_HEIGHT+306);

    pic = compo_picturebox_create(frm, UI_BUF_I332001_SLEEP_DOT3_BIN);///* 清醒睡眠图*/
    compo_picturebox_set_pos(pic, 45, GUI_SCREEN_HEIGHT+359);

    pic = compo_picturebox_create(frm, UI_BUF_I332001_SLEEP_DOT4_BIN);///* 快速眼动睡眠图*/
    compo_picturebox_set_pos(pic, 45, GUI_SCREEN_HEIGHT+412);

    txt = compo_textbox_create(frm,strlen(i18n[STR_DEEP_SLEEP]));///* 深睡*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 58, GUI_SCREEN_HEIGHT+237,100,32);
    compo_textbox_set(txt, i18n[STR_DEEP_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x69, 0x7e, 0xff));

    txt = compo_textbox_create(frm,strlen(i18n[STR_LIGHT_SLEEP]));///* 浅睡*
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 58,GUI_SCREEN_HEIGHT+290,100,32);
    compo_textbox_set(txt, i18n[STR_LIGHT_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x36, 0xb7, 0xff));

    txt = compo_textbox_create(frm,strlen(i18n[STR_ALWAKE_SLEEP]));///* 清醒*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 58, GUI_SCREEN_HEIGHT+396,100,32);
    compo_textbox_set(txt, i18n[STR_ALWAKE_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0xff, 0x87, 0x0f));

    txt = compo_textbox_create(frm,strlen(i18n[STR_EYE_MOVE_SLEEP]));///* 快速眼动*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 58,GUI_SCREEN_HEIGHT+343,100,32);
    compo_textbox_set(txt, i18n[STR_EYE_MOVE_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x00, 0xf7, 0xd6));
//////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s%02d%s",sleep_data->deepSleepMin/60,i18n[STR_HOUR], sleep_data->deepSleepMin%60,i18n[STR_MIN]);///* 深睡数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 209, GUI_SCREEN_HEIGHT+235,112,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s%02d%s",sleep_data->lightSleepMin/60,i18n[STR_HOUR], sleep_data->lightSleepMin%60,i18n[STR_MIN]);///* 浅睡数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 209, GUI_SCREEN_HEIGHT+288,112,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s%02d%s",sleep_data->wakeSleepMin/60,i18n[STR_HOUR], sleep_data->wakeSleepMin%60,i18n[STR_MIN]);///* 清醒数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 209, GUI_SCREEN_HEIGHT+394,112,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s%02d%s",sleep_data->RemSleepMin/60,i18n[STR_HOUR], sleep_data->RemSleepMin%60,i18n[STR_MIN]);///* 快速眼动数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 209,GUI_SCREEN_HEIGHT+341,122,36);
    compo_textbox_set(txt, buf);
/////////////////////////////////////////////////////////////////////////////////////////////////////
    for(int i=0; i<3; i++)
    {
        compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+279+i*53, 280, 1);
        compo_shape_set_color(shape, make_color(0X3D,0X3D,0X3D));
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////
    uint16_t width = 280;    // 总宽度
    uint8_t fillRect_h = 28; // 绘制单个高度
    uint16_t startX = 0;     // 相对坐标
    uint16_t fillRect_y = GUI_SCREEN_HEIGHT + GUI_SCREEN_HEIGHT / 4.1+10;

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+190, width, 1);
    compo_shape_set_color(shape, make_color(0X3D,0X3D,0X3D));

    for(int k=0; k<sleep_data->recordCnt; k++)
    {
        TRACE("SLEEP={state[%d] data[%d]}\n",sleep_data->sleep_record[k].state,sleep_data->sleep_record[k].period);
    }

    if (sleep_data->totalSleepMin) //是否有睡眠时长
    {
        uint16_t fillRect_x = 0;
        uint16_t startTime = sleep_data->fallAsSleepTime.hour * 60 + sleep_data->fallAsSleepTime.min; // 终止时间23:00
        uint16_t endTime = sleep_data->getUpSleepTime.hour * 60 + sleep_data->getUpSleepTime.min;     // 起始时间8:00
        uint16_t timeInterval = (endTime + 24 * 60 - startTime) % (24 * 60);                          // 总时间间隔（分钟）
        if (timeInterval == 0)
            timeInterval = 24 * 60;
        widget_page_t *page = widget_page_create(frm->page_body);
        widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_WIDTH+129, width, fillRect_h*4);

        // compo_shape_t *shapes = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);// 创建显示块矩形
        // compo_shape_set_location(shapes, GUI_SCREEN_CENTER_X,fillRect_h*2,GUI_SCREEN_WIDTH,GUI_SCREEN_HEIGHT);

        fillRect_y = fillRect_h*2+fillRect_h/2;
        for (uint16_t i = 0; i < sleep_data->recordCnt; i++)
        {
            compo_shape_t *shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);// 创建显示块矩形
            uint16_t fillRect_w = (width * sleep_data->sleep_record[i].period) / timeInterval; // 当前段的宽度

            // 先检查是否超出总宽度
            if (fillRect_x + fillRect_w > width)
            {
                fillRect_w = width - fillRect_x;
                if (i < sleep_data->recordCnt - 1)
                {
                    TRACE("Warning===>i=%d,recordCnt=%dfillRect_x=%d,fillRect_w=%d,width=%d\n",i,sleep_data->recordCnt,fillRect_x,fillRect_w,width);
                }
            }

            // 不为0的数据最少显示1像素宽
            if (fillRect_w < 1 && sleep_data->sleep_record[i].period != 0)
            {
                fillRect_w = 1;
            }

            // 设置矩形位置和颜色
            if (sleep_data->sleep_record[i].state == DEEP_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y + fillRect_h, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0x69, 0x7e, 0xff));
            }
            else if (sleep_data->sleep_record[i].state == LIGHT_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0x36, 0xb7, 0xff));
            }
            else if (sleep_data->sleep_record[i].state == AWAKE_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y - fillRect_h * 2, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0xff, 0x87, 0x0f));
            }
            else if (sleep_data->sleep_record[i].state == REM_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y - fillRect_h, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0x00, 0xf7, 0xd6));
            }

            // 更新起始点
            fillRect_x += fillRect_w;

            TRACE("i=%d, fillRect_x=%u, fillRect_w=%u\n", i, fillRect_x, fillRect_w);
        }
        widget_page_set_client(page,(width-fillRect_x)/2, 0);
    }

    ab_free(sleep_data);
    return frm;
}
//滑动处理
static void func_sleep_move(void)
{
#define   PAGE_TWO_SIZE  510  //最底y轴
#define   TOYCH_LAST_DY  40   //切换页滑动y
#define   TICK_TIME      8   //步进y像素点时间
#define   STEP_NUM       8    //步进y像素点
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;


    if(f_sleep->page_num == PAGE_1)//第一页
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            f_sleep->move_offset = dy;
            if(f_sleep->move_offset > 0)
            {
                f_sleep->move_offset = 0;
            }
            widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);

            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset <= (-TOYCH_LAST_DY))//满足切换下一页
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
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->tick = tick_get();
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
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);
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

//        printf("move_offset:%d\n",f_sleep->move_offset);
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//睡眠功能消息处理
static void func_sleep_process(void)
{
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    compo_page_move_process(f_sleep->ptm);
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    func_sleep_move();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_process();
}

static void func_sleep_message(size_msg_t msg)
{
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            compo_page_move_touch_handler(f_sleep->ptm);
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
            f_sleep->touch_flag = true;
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;

        case MSG_CTP_CLICK:
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_LONG:
            break;

        case MSG_QDEC_BACKWARD:
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            if (func_cb.flag_sort)      //快捷应用状态下不滚动页面
            {
                func_message(msg);
            }
            else
            {
                compo_page_move_set_by_pages(f_sleep->ptm, -1);
            }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;

        case MSG_QDEC_FORWARD:
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            else
            {
                compo_page_move_set_by_pages(f_sleep->ptm, 1);
            }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入睡眠功能
static void func_sleep_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sleep_t));
    func_cb.frm_main = func_sleep_form_create();

    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_sleep->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = 2,
        .jump_perc = 10,
        .quick_jump_perc = 40,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_sleep->ptm, func_cb.frm_main->page_body, &info);
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//退出睡眠功能
static void func_sleep_exit(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;
    if (f_sleep->ptm)
    {
        func_free(f_sleep->ptm);
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//睡眠功能
void func_sleep(void)
{
    printf("%s\n", __func__);
    func_sleep_enter();
    while (func_cb.sta == FUNC_SLEEP)
    {
        func_sleep_process();
        func_sleep_message(msg_dequeue());
    }
    func_sleep_exit();
}

#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
