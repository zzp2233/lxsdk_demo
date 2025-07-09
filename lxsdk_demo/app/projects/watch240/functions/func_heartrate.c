#include "include.h"
#include "func.h"
#include "ute_module_heart.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT

#define FINT_HEIGHT 24*2+8

#define FUNC_HR_SIMULATE_DATA_EN  0
#define CHART_NUM 24 //柱形图数量
#define CHART_200_LENGTH GUI_SCREEN_CENTER_Y*6.25/8  //柱形图框高度
#define CONTROL_Y  8
//组件ID;
enum
{
    COMPO_ID_HEART_PIC=1,
    COMPO_ID_HEART_MAX_TXT,
    COMPO_ID_HEART_MIN_TXT,
    COMPO_ID_HEART_VALUE_TXT,
    COMPO_ID_HEART_BPM_TXT,
    COMPO_ID_CHART,
};

typedef struct f_heartrate_t_
{
    page_tp_move_t *ptm;
    uint32_t tick;
    u8 last_hr;
    u8 max_hr;
    u8 min_hr;
    bool heart_pic_state;
    u8 heart_pic_size;
} f_heartrate_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_heartrate_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_HEART_RATE]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_HEART_ICON_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6-CONTROL_Y);
    compo_setid(picbox,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
//    if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255){  ///佩戴处理
//        memset(txt_buf,0,sizeof(txt_buf));
//        snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
//        textbox = compo_textbox_create(frm, 3 );///次/分 数据
//        compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
//        compo_textbox_set_align_center(textbox, false);
//        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/2.3-CONTROL_Y);
//        compo_textbox_set(textbox,txt_buf);
//        compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);
//    }else{
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2+2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/2.3-CONTROL_Y);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);
//    }

    area_t txt_leng = widget_text_get_area(textbox->txt);
//    printf("leng:%d\n",txt_leng.wid);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.3+txt_leng.wid,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.75-CONTROL_Y,58,28);
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.45,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y,50,23);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
#if UTE_MODULE_HEART_SUPPORT
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
    }
    else
#endif
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.45+26,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X*1.1,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y,50,23);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
#if UTE_MODULE_HEART_SUPPORT
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
    }
    else
#endif
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X*1.1+26,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_textbox_set_align_center(textbox, false);


//    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255){
//        memset(txt_buf,0,sizeof(txt_buf));
//        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
//        textbox = compo_textbox_create(frm, 3);///最低数据
//        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2.85,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y);
//        compo_textbox_set(textbox,txt_buf);
//        compo_textbox_set_forecolor(textbox, COLOR_GRAY);
//        compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
//    }else{
//        textbox = compo_textbox_create(frm, 3);///最低数据
//        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2.85,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y);
//        compo_textbox_set(textbox,"--");
//        compo_textbox_set_forecolor(textbox, COLOR_GRAY);
//        compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
//    }

    ///*第二页*/
    uint8_t heart_date[24];
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率
#else
    for (int i=0; i<24; i++)heart_date[i]=120;
#endif


    picbox = compo_picturebox_create(frm, UI_BUF_I330001_HEART_MAP_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y/1.1);
    ///创建图表
    compo_chartbox_t *chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/7, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y/1.15,196,153);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart, COMPO_ID_CHART);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 7;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        chart_info.x = i*chart_info.width + i;
        chart_info.height = heart_date[i]/1.31;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    return frm;
}

// 刷新心率
static void func_heartrate_refresh(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 10+(f_heartrate->heart_pic_size/20)))
    {
        f_heartrate->tick = tick_get();
        area_t pic_size;
        char txt_buf[20];
        compo_textbox_t *textbox_max = compo_getobj_byid(COMPO_ID_HEART_MAX_TXT);
        compo_textbox_t *textbox_min = compo_getobj_byid(COMPO_ID_HEART_MIN_TXT);
        compo_textbox_t *textbox_value = compo_getobj_byid(COMPO_ID_HEART_VALUE_TXT);
        compo_textbox_t *textbox_bpm = compo_getobj_byid(COMPO_ID_HEART_BPM_TXT);
        compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_HEART_PIC);

        pic_size = gui_image_get_size(UI_BUF_I330001_HEART_ICON_BIN);
#if UTE_MODULE_HEART_SUPPORT
        if(uteModuleHeartIsWear() == true)   ///佩戴处理
        {
            if(f_heartrate->heart_pic_size <= 72)
            {
                f_heartrate->heart_pic_state = true;
            }
            else if(f_heartrate->heart_pic_size >= 110)
            {
                f_heartrate->heart_pic_state = false;
            }

            if(f_heartrate->heart_pic_state == true)
            {
                f_heartrate->heart_pic_size ++ ;
            }
            else
            {
                f_heartrate->heart_pic_size -- ;
            }

        }
        else
#endif
        {
            f_heartrate->heart_pic_size = 100;
        }

        compo_picturebox_set_size(picbox,f_heartrate->heart_pic_size*pic_size.wid/100,f_heartrate->heart_pic_size*pic_size.hei/100);
#if UTE_MODULE_HEART_SUPPORT
        if(uteModuleHeartGetMinHeartValue() == 0 || uteModuleHeartGetMinHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_min,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
            compo_textbox_set(textbox_min,txt_buf);
        }

        if(uteModuleHeartGetMaxHeartValue() == 0 || uteModuleHeartGetMaxHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_max,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
            compo_textbox_set(textbox_max,txt_buf);
        }

        if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255)  ///佩戴处理
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
            compo_textbox_set(textbox_value,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"--");
            compo_textbox_set(textbox_value,txt_buf);
        }
#endif
        area_t txt_leng = widget_text_get_area(textbox_value->txt);
        compo_textbox_set_location(textbox_bpm,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.3+txt_leng.wid,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.75-CONTROL_Y,58,28);
    }

}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_heartrate_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_HEART_RATE]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_ICON_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6-CONTROL_Y);
    compo_setid(picbox,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
//    if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255){  ///佩戴处理
//        memset(txt_buf,0,sizeof(txt_buf));
//        snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
//        textbox = compo_textbox_create(frm, 3 );///次/分 数据
//        compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
//        compo_textbox_set_align_center(textbox, false);
//        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/2.3-CONTROL_Y);
//        compo_textbox_set(textbox,txt_buf);
//        compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);
//    }else{
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2+2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/2.3-CONTROL_Y);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);
//    }

    area_t txt_leng = widget_text_get_area(textbox->txt);
//    printf("leng:%d\n",txt_leng.wid);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.3+txt_leng.wid,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.75-CONTROL_Y,58,28);
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.45,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y,50,23);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.45+26,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X*1.1,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y,50,23);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X*1.1+26,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_textbox_set_align_center(textbox, false);

    ///*第二页*/
    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

//    for (int i=0; i<24; i++)heart_date[i]=120;

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_MAP_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y/1.1);
    ///创建图表
    compo_chartbox_t *chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/7, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y/1.15,196,153);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart, COMPO_ID_CHART);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 7;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        chart_info.x = i*chart_info.width + i;
        chart_info.height = heart_date[i]/1.31;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    return frm;
}

// 刷新心率
static void func_heartrate_refresh(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 10+(f_heartrate->heart_pic_size/20)))
    {
        f_heartrate->tick = tick_get();
        area_t pic_size;
        char txt_buf[20];
        compo_textbox_t *textbox_max = compo_getobj_byid(COMPO_ID_HEART_MAX_TXT);
        compo_textbox_t *textbox_min = compo_getobj_byid(COMPO_ID_HEART_MIN_TXT);
        compo_textbox_t *textbox_value = compo_getobj_byid(COMPO_ID_HEART_VALUE_TXT);
        compo_textbox_t *textbox_bpm = compo_getobj_byid(COMPO_ID_HEART_BPM_TXT);
        compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_HEART_PIC);

        pic_size = gui_image_get_size(UI_BUF_I332001_HEART_ICON_BIN);

        if(uteModuleHeartIsWear() == true)   ///佩戴处理
        {
            if(f_heartrate->heart_pic_size <= 72)
            {
                f_heartrate->heart_pic_state = true;
            }
            else if(f_heartrate->heart_pic_size >= 110)
            {
                f_heartrate->heart_pic_state = false;
            }

            if(f_heartrate->heart_pic_state == true)
            {
                f_heartrate->heart_pic_size ++ ;
            }
            else
            {
                f_heartrate->heart_pic_size -- ;
            }

        }
        else
        {
            f_heartrate->heart_pic_size = 100;
        }

        compo_picturebox_set_size(picbox,f_heartrate->heart_pic_size*pic_size.wid/100,f_heartrate->heart_pic_size*pic_size.hei/100);

        if(uteModuleHeartGetMinHeartValue() == 0 || uteModuleHeartGetMinHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_min,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
            compo_textbox_set(textbox_min,txt_buf);
        }

        if(uteModuleHeartGetMaxHeartValue() == 0 || uteModuleHeartGetMaxHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_max,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
            compo_textbox_set(textbox_max,txt_buf);
        }

        if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255)  ///佩戴处理
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
            compo_textbox_set(textbox_value,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"--");
            compo_textbox_set(textbox_value,txt_buf);
        }
        area_t txt_leng = widget_text_get_area(textbox_value->txt);
        compo_textbox_set_location(textbox_bpm,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.3+txt_leng.wid,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.75-CONTROL_Y,58,28);
    }

}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_heartrate_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_HEART_RATE]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_ICON_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6-CONTROL_Y);
    compo_setid(picbox,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,110,235);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,120+txt_leng.wid,258,58,22);
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_SHANG_BIN);
    compo_picturebox_set_pos(picbox, 93+28/2,304+22/2);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,125,302);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_XIA_BIN);
    compo_picturebox_set_pos(picbox, 235-28/2,302+22/2);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,239,302);
    compo_textbox_set_align_center(textbox, false);

    ///*第二页*/

    ///设置标题栏名字///
    textbox = compo_textbox_create(frm,strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_location(textbox,GUI_SCREEN_WIDTH/2,390+38/2,120,38);
    compo_textbox_set(textbox, i18n[STR_HEART_RATE]);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    // for (int i=0; i<24; i++)heart_date[i]=120;

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_MAP_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y+7);
    ///创建图表
    compo_chartbox_t *chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, 200, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y+20,238,157);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart, COMPO_ID_CHART);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 8;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        chart_info.x = i*chart_info.width + i*2;
        chart_info.height = heart_date[i]/1.04347;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    return frm;
}

// 刷新心率
static void func_heartrate_refresh(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 10+(f_heartrate->heart_pic_size/20)))
    {
        f_heartrate->tick = tick_get();
        area_t pic_size;
        char txt_buf[20];
        compo_textbox_t *textbox_max = compo_getobj_byid(COMPO_ID_HEART_MAX_TXT);
        compo_textbox_t *textbox_min = compo_getobj_byid(COMPO_ID_HEART_MIN_TXT);
        compo_textbox_t *textbox_value = compo_getobj_byid(COMPO_ID_HEART_VALUE_TXT);
        compo_textbox_t *textbox_bpm = compo_getobj_byid(COMPO_ID_HEART_BPM_TXT);
        compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_HEART_PIC);

        pic_size = gui_image_get_size(UI_BUF_I332001_HEART_ICON_BIN);

        if(uteModuleHeartIsWear() == true)   ///佩戴处理
        {
            if(f_heartrate->heart_pic_size <= 72)
            {
                f_heartrate->heart_pic_state = true;
            }
            else if(f_heartrate->heart_pic_size >= 110)
            {
                f_heartrate->heart_pic_state = false;
            }

            if(f_heartrate->heart_pic_state == true)
            {
                f_heartrate->heart_pic_size ++ ;
            }
            else
            {
                f_heartrate->heart_pic_size -- ;
            }

        }
        else
        {
            f_heartrate->heart_pic_size = 100;
        }

        compo_picturebox_set_size(picbox,f_heartrate->heart_pic_size*pic_size.wid/100,f_heartrate->heart_pic_size*pic_size.hei/100);

        if(uteModuleHeartGetMinHeartValue() == 0 || uteModuleHeartGetMinHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_min,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
            compo_textbox_set(textbox_min,txt_buf);
        }

        if(uteModuleHeartGetMaxHeartValue() == 0 || uteModuleHeartGetMaxHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_max,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
            compo_textbox_set(textbox_max,txt_buf);
        }

        if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255)  ///佩戴处理
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
            compo_textbox_set(textbox_value,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"--");
            compo_textbox_set(textbox_value,txt_buf);
        }
        area_t txt_leng = widget_text_get_area(textbox_value->txt);
        compo_textbox_set_location(textbox_bpm,120+txt_leng.wid,258,58,22);
    }

}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//心率功能事件处理
static void func_heartrate_process(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    compo_page_move_process(f_heartrate->ptm);
    func_heartrate_refresh();
    func_process();
}

//心率功能消息处理
static void func_heartrate_message(size_msg_t msg)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_heartrate->ptm);
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入心率功能
static void func_heartrate_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_heartrate_t));
    func_cb.frm_main = func_heartrate_form_create();

    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    f_heartrate->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
        .title_used = true,
        .page_size = GUI_SCREEN_HEIGHT - (GUI_SCREEN_HEIGHT / 8),
#else
        .title_used = false,
        .page_size = GUI_SCREEN_HEIGHT,
#endif
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = 100,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    // bsp_sensor_hr_init(0);
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartStartSingleTesting(TYPE_HEART);
#endif
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.flag_animation = false;

    f_heartrate->heart_pic_size = 100;
}

//退出心率功能
static void func_heartrate_exit(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if (f_heartrate->ptm)
    {
        func_free(f_heartrate->ptm);
    }
    // bsp_sensor_hr_stop();
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartStopSingleTesting(TYPE_HEART);
#endif
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
}

//心率功能
void func_heartrate(void)
{
    printf("%s\n", __func__);
    func_heartrate_enter();
    while (func_cb.sta == FUNC_HEARTRATE)
    {
        func_heartrate_process();
        func_heartrate_message(msg_dequeue());
    }
    func_heartrate_exit();
}

#endif
