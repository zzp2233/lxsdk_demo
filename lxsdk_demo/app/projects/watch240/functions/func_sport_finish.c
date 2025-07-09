#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#include "ute_drv_motor.h"

#define TRACE_EN        0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if UTE_MODULE_SCREENS_SPORT_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
    COMPO_ID_KM_VALUE,
    COMPO_ID_KM_UINT,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;
} f_sport_finish_t;

extern u8 sport_finish_mode;
extern u32 func_sport_get_current_idx(void);
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);

//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    if (func_cb.sta == FUNC_SPORT_FINISH)
    {
#if  UTE_MODULE_SPORT_SUPPORT
        f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
//        printf("sport = %d\n",f_sport_finish->sport_finish_state);

        ute_module_more_sports_data_t sport_data;
        uteModuleSportGetMoreSportsDatas(&sport_data);
//        ute_module_systemtime_time_t time;
//        uteModuleSystemtimeGetTime(&time);//获取系统时间

        if(uteModuleSystemtime12HOn())
        {
            sport_data.saveData.startSportTime.hour %=12;
            if(sport_data.saveData.startSportTime.hour == 0)
            {
                sport_data.saveData.startSportTime.hour = 12;
            }
//            sport_data.saveData.startSportTime.min  %=12;
        }
//        sport_data.totalSportTime = 51564;
        char txt_buf[50];
        memset(txt_buf,0,sizeof(txt_buf));
//        f_sport_finish->sport_finish_state = COMPO_WALK_STATE;
        switch(f_sport_finish->sport_finish_state)
        {
            case COMPO_WALK_STATE://户外跑步类别
            {
                //运动类型图片
//                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(uteModuleSportMoreSportGetType() - 1));
                compo_picturebox_set_pos(picbox, 10+56/2, 12+56/2);

                //运动类型名称
//                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(func_sport_get_current_idx())]));
                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_24_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+73);
//                compo_textbox_set(textbox, i18n[func_sport_get_str(func_sport_get_current_idx())]);
                compo_textbox_set(textbox, i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]);

                //时间日期
                snprintf(txt_buf,sizeof(txt_buf),"%04d/%02d/%02d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+105);
                compo_textbox_set(textbox, txt_buf);
                compo_textbox_set_forecolor(textbox, COLOR_GREEN);

                //运动数据图标
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIME_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_DIS_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_STEP_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*3);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*4);


                /*运动数据*/
                memset(txt_buf,0,sizeof(txt_buf));//运动时长
                snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142);
                compo_textbox_set(textbox, txt_buf);

                memset(txt_buf,0,sizeof(txt_buf));//运动卡路里
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40);
                compo_textbox_set(textbox, txt_buf);
                area_t txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184);
                compo_textbox_set(textbox, i18n[STR_KCAL]);

                u8 km_integer=sport_data.saveData.sportDistanceInteger;                 //距离 整数
                u8 km_decimals=sport_data.saveData.sportDistanceDecimals;                     // 小数
                if(uteModuleSystemtimeGetDistanceMiType())//英里
                {
                    uint16_t distance = km_integer*1000+km_decimals*10;
                    distance = distance*0.6213712;
                    km_integer  = distance/1000;
                    km_decimals = distance%1000/10;
                }
                memset(txt_buf,0,sizeof(txt_buf)); //运动公里
                snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*2);
                compo_textbox_set(textbox, txt_buf);
                compo_setid(textbox,COMPO_ID_KM_VALUE);

                txt_leng = widget_text_get_area(textbox->txt);
                if(uteModuleSystemtimeGetDistanceMiType())//英里
                {
                    textbox = compo_textbox_create(frm, strlen(i18n[STR_MILE]));
                    compo_textbox_set_align_center(textbox, false);
                    compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40);
                    compo_textbox_set(textbox, i18n[STR_MILE]);
                }
                else
                {
                    textbox = compo_textbox_create(frm, strlen(i18n[STR_KM]));
                    compo_textbox_set_align_center(textbox, false);
                    compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40);
                    compo_textbox_set(textbox, i18n[STR_KM]);
                }
                compo_setid(textbox,COMPO_ID_KM_UINT);

                memset(txt_buf,0,sizeof(txt_buf));//运动步数
                snprintf(txt_buf,sizeof(txt_buf),"%ld",sport_data.saveData.sportStep);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*3);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_location(textbox, 50+txt_leng.wid, 5+184+40*2,100,widget_text_get_max_height());
                compo_textbox_set(textbox, i18n[STR_STEP]);

                memset(txt_buf,0,sizeof(txt_buf));//运动心率
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.avgHeartRate);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*4);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*3);
                compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
                /*运动数据*/

                compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_BIN);
                compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 5+184+40*5);
                compo_setid(btn,COMPO_BTN_SURE);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
                compo_textbox_set_forecolor(textbox, COLOR_BLACK);
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*5,GUI_SCREEN_CENTER_X,28);
                compo_textbox_set(textbox, i18n[STR_OK]);

                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
                compo_textbox_set_align_center(textbox, true);
                compo_textbox_set_multiline(textbox, true);
                // compo_textbox_set_multiline_drag(textbox,true);
                widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*7,GUI_SCREEN_CENTER_X*1.7,28*2);
                compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);
            }
            break;
            case COMPO_SWIMING_STATE://游泳 跳绳
            {
                //运动类型图片
//                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(uteModuleSportMoreSportGetType() - 1));
                compo_picturebox_set_pos(picbox, 10+56/2, 12+56/2);

                //运动类型名称
//                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(func_sport_get_current_idx())]));
                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_24_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+73);
//                compo_textbox_set(textbox, i18n[func_sport_get_str(func_sport_get_current_idx())]);
                compo_textbox_set(textbox, i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]);

                //时间日期
                snprintf(txt_buf,sizeof(txt_buf),"%04d/%02d/%02d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+105);
                compo_textbox_set(textbox, txt_buf);
                compo_textbox_set_forecolor(textbox, COLOR_GREEN);

                //运动数据图标
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIME_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIMES_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*3);


                /*运动数据*/
                memset(txt_buf,0,sizeof(txt_buf));//运动时长
                snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142);
                compo_textbox_set(textbox, txt_buf);

                memset(txt_buf,0,sizeof(txt_buf));//运动卡路里
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40);
                compo_textbox_set(textbox, txt_buf);
                area_t txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184);
                compo_textbox_set(textbox, i18n[STR_KCAL]);

                memset(txt_buf,0,sizeof(txt_buf));//运动次数
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*2);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*1);
                compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

                memset(txt_buf,0,sizeof(txt_buf));//运动心率
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.avgHeartRate);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*3);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*2);
                compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
                /*运动数据*/

                compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_BIN);
                compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 5+184+40*4);
                compo_setid(btn,COMPO_BTN_SURE);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
                compo_textbox_set_forecolor(textbox, COLOR_BLACK);
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*4,GUI_SCREEN_CENTER_X,28);
                compo_textbox_set(textbox, i18n[STR_OK]);

                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
                compo_textbox_set_align_center(textbox, true);
                compo_textbox_set_multiline(textbox, true);
                // compo_textbox_set_multiline_drag(textbox,true);
                widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*6,GUI_SCREEN_CENTER_X*1.7,28*2);
                compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);
            }
            break;
            case COMPO_OTHER_STATE://其他
            {
                //运动类型图片
//                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(uteModuleSportMoreSportGetType() - 1));
                compo_picturebox_set_pos(picbox, 10+56/2, 12+56/2);

                //运动类型名称
//                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(func_sport_get_current_idx())]));
                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_24_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+73);
//                compo_textbox_set(textbox, i18n[func_sport_get_str(func_sport_get_current_idx())]);
                compo_textbox_set(textbox, i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]);

                //时间日期
                snprintf(txt_buf,sizeof(txt_buf),"%04d/%02d/%02d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+105);
                compo_textbox_set(textbox, txt_buf);
                compo_textbox_set_forecolor(textbox, COLOR_GREEN);

                //运动数据图标
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIME_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*2);

                /*运动数据*/
                memset(txt_buf,0,sizeof(txt_buf));//运动时长
                snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142);
                compo_textbox_set(textbox, txt_buf);

                memset(txt_buf,0,sizeof(txt_buf));//运动卡路里
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40);
                compo_textbox_set(textbox, txt_buf);
                area_t txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184);
                compo_textbox_set(textbox, i18n[STR_KCAL]);

                memset(txt_buf,0,sizeof(txt_buf));//运动心率
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.avgHeartRate);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*2);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*1);
                compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
                /*运动数据*/

                compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_BIN);
                compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 5+184+40*3);
                compo_setid(btn,COMPO_BTN_SURE);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
                compo_textbox_set_forecolor(textbox, COLOR_BLACK);
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*3,GUI_SCREEN_CENTER_X,28);
                compo_textbox_set(textbox, i18n[STR_OK]);

                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
                compo_textbox_set_align_center(textbox, true);
                compo_textbox_set_multiline(textbox, true);
                // compo_textbox_set_multiline_drag(textbox,true);
                widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*5,GUI_SCREEN_CENTER_X*1.7,28*2);
                compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);
            }
            break;
        }
#endif
    }
    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;

} f_sport_finish_t;

extern u8 sport_finish_mode;
extern u32 func_sport_get_current_idx(void);
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);

//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

//    if (func_cb.sta == FUNC_SPORT_FINISH)
//    {
//        f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
////        printf("sport = %d\n",f_sport_finish->sport_finish_state);
//        ute_module_more_sports_data_t sport_data;
//        uteModuleSportGetMoreSportsDatas(&sport_data);
////        ute_module_systemtime_time_t time;
////        uteModuleSystemtimeGetTime(&time);//获取系统时间
//
//        if(uteModuleSystemtime12HOn())
//        {
//            sport_data.saveData.startSportTime.hour %=12;
//            if(sport_data.saveData.startSportTime.hour == 0)
//            {
//                sport_data.saveData.startSportTime.hour = 12;
//            }
////            sport_data.saveData.startSportTime.min  %=12;
//        }
////        sport_data.totalSportTime = 51564;
//        char txt_buf[50];
//        memset(txt_buf,0,sizeof(txt_buf));
////        f_sport_finish->sport_finish_state = COMPO_WALK_STATE;
//        switch(f_sport_finish->sport_finish_state)
//        {
//            case COMPO_WALK_STATE://户外跑步类别
//            {
//                //运动类型图片
////                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
//                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(uteModuleSportMoreSportGetType() - 1));
//                compo_picturebox_set_pos(picbox, 10+56/2, 12+56/2);
//
//                //运动类型名称
////                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(func_sport_get_current_idx())]));
//                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]));
////                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_24_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 10, 5+73);
////                compo_textbox_set(textbox, i18n[func_sport_get_str(func_sport_get_current_idx())]);
//                compo_textbox_set(textbox, i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]);
//
//                //时间日期
//                snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %d:%d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 10, 5+105);
//                compo_textbox_set(textbox, txt_buf);
//                compo_textbox_set_forecolor(textbox, COLOR_GREEN);
//
//                //运动数据图标
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIME_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2);
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40);
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_DIS_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*2);
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_STEP_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*3);
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*4);
//
//
//                /*运动数据*/
//                memset(txt_buf,0,sizeof(txt_buf));//运动时长
//                snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142);
//                compo_textbox_set(textbox, txt_buf);
//
//                memset(txt_buf,0,sizeof(txt_buf));//运动卡路里
//                snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.sportCaloire);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142+40);
//                compo_textbox_set(textbox, txt_buf);
//                area_t txt_leng = widget_text_get_area(textbox->txt);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184);
//                compo_textbox_set(textbox, i18n[STR_KCAL]);
//
//                memset(txt_buf,0,sizeof(txt_buf)); //运动公里
//                snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",sport_data.saveData.sportDistanceInteger,sport_data.saveData.sportDistanceDecimals);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142+40*2);
//                compo_textbox_set(textbox, txt_buf);
//                txt_leng = widget_text_get_area(textbox->txt);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_KM]));
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40);
//                compo_textbox_set(textbox, i18n[STR_KM]);
//
//                memset(txt_buf,0,sizeof(txt_buf));//运动步数
//                snprintf(txt_buf,sizeof(txt_buf),"%ld",sport_data.saveData.sportStep);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142+40*3);
//                compo_textbox_set(textbox, txt_buf);
//                txt_leng = widget_text_get_area(textbox->txt);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*2);
//                compo_textbox_set(textbox, i18n[STR_STEP]);
//
//                memset(txt_buf,0,sizeof(txt_buf));//运动心率
//                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.avgHeartRate);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142+40*4);
//                compo_textbox_set(textbox, txt_buf);
//                txt_leng = widget_text_get_area(textbox->txt);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*3);
//                compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
//                /*运动数据*/
//
//                compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_BIN);
//                compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 5+184+40*5);
//                compo_setid(btn,COMPO_BTN_SURE);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
//                compo_textbox_set_forecolor(textbox, COLOR_BLACK);
//                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*5,GUI_SCREEN_CENTER_X,28);
//                compo_textbox_set(textbox, i18n[STR_OK]);
//
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
//                compo_textbox_set_align_center(textbox, true);
//                compo_textbox_set_multiline(textbox, true);
//                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*7,GUI_SCREEN_CENTER_X*1.7,28*2);
//                compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);
//            }
//            break;
//            case COMPO_SWIMING_STATE://游泳 跳绳
//            {
//                //运动类型图片
////                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
//                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(uteModuleSportMoreSportGetType() - 1));
//                compo_picturebox_set_pos(picbox, 10+56/2, 12+56/2);
//
//                //运动类型名称
////                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(func_sport_get_current_idx())]));
//                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]));
////                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_24_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 10, 5+73);
////                compo_textbox_set(textbox, i18n[func_sport_get_str(func_sport_get_current_idx())]);
//                compo_textbox_set(textbox, i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]);
//
//                //时间日期
//                snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %d:%d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 10, 5+105);
//                compo_textbox_set(textbox, txt_buf);
//                compo_textbox_set_forecolor(textbox, COLOR_GREEN);
//
//                //运动数据图标
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIME_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2);
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40);
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIMES_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*2);
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*3);
//
//
//                /*运动数据*/
//                memset(txt_buf,0,sizeof(txt_buf));//运动时长
//                snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142);
//                compo_textbox_set(textbox, txt_buf);
//
//                memset(txt_buf,0,sizeof(txt_buf));//运动卡路里
//                snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.sportCaloire);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142+40);
//                compo_textbox_set(textbox, txt_buf);
//                area_t txt_leng = widget_text_get_area(textbox->txt);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184);
//                compo_textbox_set(textbox, i18n[STR_KCAL]);
//
//                memset(txt_buf,0,sizeof(txt_buf));//运动次数
//                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142+40*2);
//                compo_textbox_set(textbox, txt_buf);
//                txt_leng = widget_text_get_area(textbox->txt);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*1);
//                compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);
//
//                memset(txt_buf,0,sizeof(txt_buf));//运动心率
//                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.avgHeartRate);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142+40*3);
//                compo_textbox_set(textbox, txt_buf);
//                txt_leng = widget_text_get_area(textbox->txt);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*2);
//                compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
//                /*运动数据*/
//
//                compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_BIN);
//                compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 5+184+40*4);
//                compo_setid(btn,COMPO_BTN_SURE);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
//                compo_textbox_set_forecolor(textbox, COLOR_BLACK);
//                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*4,GUI_SCREEN_CENTER_X,28);
//                compo_textbox_set(textbox, i18n[STR_OK]);
//
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
//                compo_textbox_set_align_center(textbox, true);
//                compo_textbox_set_multiline(textbox, true);
//                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*6,GUI_SCREEN_CENTER_X*1.7,28*2);
//                compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);
//            }
//            break;
//            case COMPO_OTHER_STATE://其他
//            {
//                //运动类型图片
////                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
//                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(uteModuleSportMoreSportGetType() - 1));
//                compo_picturebox_set_pos(picbox, 10+56/2, 12+56/2);
//
//                //运动类型名称
////                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(func_sport_get_current_idx())]));
//                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]));
////                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_24_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 10, 5+73);
////                compo_textbox_set(textbox, i18n[func_sport_get_str(func_sport_get_current_idx())]);
//                compo_textbox_set(textbox, i18n[func_sport_get_str(uteModuleSportMoreSportGetType() - 1)]);
//
//                //时间日期
//                snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %d:%d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 10, 5+105);
//                compo_textbox_set(textbox, txt_buf);
//                compo_textbox_set_forecolor(textbox, COLOR_GREEN);
//
//                //运动数据图标
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIME_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2);
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40);
//                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);
//                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*2);
//
//                /*运动数据*/
//                memset(txt_buf,0,sizeof(txt_buf));//运动时长
//                snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142);
//                compo_textbox_set(textbox, txt_buf);
//
//                memset(txt_buf,0,sizeof(txt_buf));//运动卡路里
//                snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.sportCaloire);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142+40);
//                compo_textbox_set(textbox, txt_buf);
//                area_t txt_leng = widget_text_get_area(textbox->txt);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184);
//                compo_textbox_set(textbox, i18n[STR_KCAL]);
//
//                memset(txt_buf,0,sizeof(txt_buf));//运动心率
//                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.avgHeartRate);
//                textbox = compo_textbox_create(frm, strlen(txt_buf));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 44,5+142+40*2);
//                compo_textbox_set(textbox, txt_buf);
//                txt_leng = widget_text_get_area(textbox->txt);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
//                compo_textbox_set_align_center(textbox, false);
//                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*1);
//                compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
//                /*运动数据*/
//
//                compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_BIN);
//                compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 5+184+40*3);
//                compo_setid(btn,COMPO_BTN_SURE);
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
//                compo_textbox_set_forecolor(textbox, COLOR_BLACK);
//                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*3,GUI_SCREEN_CENTER_X,28);
//                compo_textbox_set(textbox, i18n[STR_OK]);
//
//                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
//                compo_textbox_set_align_center(textbox, true);
//                compo_textbox_set_multiline(textbox, true);
//                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*5,GUI_SCREEN_CENTER_X*1.7,28*2);
//                compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);
//            }
//            break;
//        }
//    }
    return frm;
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//功能事件处理
static void func_sport_finish_process(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    compo_page_move_process(f_sport_finish->ptm);
    func_process();
}
//室内跑步功能消息处理
static void func_sport_finish_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case COMPO_BTN_SURE:
            func_back_to();
            break;
    }

}
//室内跑步功能消息处理
static void func_sport_finish_message(size_msg_t msg)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_sport_finish->ptm);
            break;
        case MSG_CTP_CLICK:
            func_sport_finish_click();
            break;
        case MSG_SYS_500MS:
            if(f_sport_finish->uint_km != uteModuleSystemtimeGetDistanceMiType())
            {
                f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
                msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
            }
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入室内跑步功能
static void func_sport_finish_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sport_finish_t));
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;

    //防止跳回运动中界面
    if(task_stack_contains(FUNC_SPORT_SUB_RUN))
    {
        task_stack_remove(FUNC_SPORT_SUB_RUN);
    }

    int page_length=0;
    switch(sport_finish_mode)
    {
        case 0:
            f_sport_finish->sport_finish_state = COMPO_WALK_STATE;
            page_length = 524;
            break;
        case 1:
            f_sport_finish->sport_finish_state = COMPO_SWIMING_STATE;
            page_length = 524-24*2;
            break;
        case 2:
            f_sport_finish->sport_finish_state = COMPO_OTHER_STATE;
            page_length = 524-24*4;
            break;
    }

    func_cb.frm_main = func_sport_finish_form_create();
    f_sport_finish->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .page_size = page_length+5,
        .page_count = 1,
        .up_over_perc = 5,
        .down_over_perc = 5,
    };
    compo_page_move_init(f_sport_finish->ptm, func_cb.frm_main->page_body, &info);

    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
    f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
}

//退出室内跑步功能
static void func_sport_finish_exit(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    func_cb.last = FUNC_SPORT_FINISH;
    if (f_sport_finish->ptm)
    {
        func_free(f_sport_finish->ptm);
    }
}

//室内跑步功能
void func_sport_finish(void)
{
    printf("%s\n", __func__);
    func_sport_finish_enter();
    while (func_cb.sta == FUNC_SPORT_FINISH)
    {
        func_sport_finish_process();
        func_sport_finish_message(msg_dequeue());
    }
    func_sport_finish_exit();
}

#endif
