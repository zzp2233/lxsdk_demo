#include "include.h"
#include "func.h"
#include "ute_module_weather.h"
#include "ute_module_systemtime.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_WEATHER_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;
    bool no_weather_dada;
} f_weather_t;

enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

static const f_weather_t weather_list[] =
{
    {STR_UNKNOWN,           UI_BUF_I330001_WEATHER_ICON_WEATHER_00_BIN  },                //未知
    {STR_SUNNY,             UI_BUF_I330001_WEATHER_ICON_WEATHER_01_BIN  },                //晴天
    {STR_CLOUDY,            UI_BUF_I330001_WEATHER_ICON_WEATHER_02_BIN  },                //多云
    {STR_OVERCAST,          UI_BUF_I330001_WEATHER_ICON_WEATHER_03_BIN  },                //阴天
    {STR_MODERATE_RAIN,     UI_BUF_I330001_WEATHER_ICON_WEATHER_04_BIN  },                //阵雨
    {STR_RAINY_SHOWERS,     UI_BUF_I330001_WEATHER_ICON_WEATHER_05_BIN  },                //雷阵雨
    {STR_SLEET,             UI_BUF_I330001_WEATHER_ICON_WEATHER_06_BIN  },                //雨夹雪
    {STR_DRIZZLE,           UI_BUF_I330001_WEATHER_ICON_WEATHER_07_BIN  },                //小雨
    {STR_HEAVY_RAIN,        UI_BUF_I330001_WEATHER_ICON_WEATHER_08_BIN  },                //大雨
    {STR_RAINY,             UI_BUF_I330001_WEATHER_ICON_WEATHER_09_BIN  },                //雪
    {STR_SAND_AND_DUST,     UI_BUF_I330001_WEATHER_ICON_WEATHER_10_BIN  },                //沙尘暴
    {STR_HAZE,              UI_BUF_I330001_WEATHER_ICON_WEATHER_11_BIN  },                //雾霾
    {STR_WINDY,             UI_BUF_I330001_WEATHER_ICON_WEATHER_12_BIN  },                //大风
    {STR_SUNNY,             UI_BUF_I330001_WEATHER_ICON_WEATHER_13_BIN  },                //明夜
    {STR_CLOUDY,            UI_BUF_I330001_WEATHER_ICON_WEATHER_14_BIN  },                //云遮月
    {STR_RAINY_SHOWERS,     UI_BUF_I330001_WEATHER_ICON_WEATHER_15_BIN  },                //阵雨夜

};

static int16_t page_y = 0;
bool weather_nodata_flag= true;
static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t  weather_date;
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        if(f_weather->DayWeather[i] != weather_date.DayWeather[i])
        {
            weather_flag = true;
            break;
        }
    }

    if(!weather_nodata_flag &&(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true))    //是否为华氏度
    {
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
    printf("1111weather_nodata_flag =%d\n",weather_nodata_flag);
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
}

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{
    compo_textbox_t *txt;
    compo_picturebox_t * picbox;
    bool weather_no_data_flag = true;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    u8 week_sort[7];
    u8 week_day=0;
    u8 get_weather_id[7];//存放一个星期的排序
    char str_buff[30];//用于存放打印数据

    memset(get_weather_id,0,sizeof(get_weather_id));

    uteModuleSystemtimeGetTime(&time);//获取系统时间
//    printf("week:%d\n",time.week);
    week_day = time.week; //获取星期
    for(int i=0; i<=6; i++) //星期排序
    {
//        switch(week_day)
//        {
//            case 0 :
//                week_sort[i] = 6;
//                break;
//            case 1 :
//                week_sort[i] = 0;
//                break;
//            case 2 :
//                week_sort[i] = 1;
//                break;
//            case 3 :
//                week_sort[i] = 2;
//                break;
//            case 4 :
//                week_sort[i] = 3;
//                break;
//            case 5 :
//                week_sort[i] = 4;
//                break;
//            case 6 :
//                week_sort[i] = 5;
//                break;
//        }
        week_sort[i] = week_day;
        if(++week_day==7)
        {
            week_day = 0;
        }
    }

    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态
//        printf("year:%d mon:%d day:%d hour:%d min:%d sec:%d\n",weather_date.updateTime.year,weather_date.updateTime.month,weather_date.updateTime.day,weather_date.updateTime.hour,weather_date.updateTime.min,weather_date.updateTime.sec);

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                weather_nodata_flag = false;
                break;
            }
        }
//        int pic_num=3;
        for(int i=0; i<7; i++) //获取一周的天气
        {
//            printf("weather:%d\n",weather_date.DayWeather[i]>>8);
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
                {
                    case WEATHER_TYPE_SUNNY:
                        get_weather_id[i] = 13;
                        break;
                    case WEATHER_TYPE_CLOUDY:
                        get_weather_id[i] = 14;
                        break;
                    case WEATHER_TYPE_THUNDERSHOWER_RAIN:
                        get_weather_id[i] = 15;
                        break;
                }
            }
            get_weather_id[i] = weather_date.DayWeather[i]>>8;//赋值排序天气状态
        }

        if(displayInfo.isFahrenheit)    //是否为华氏度
        {
            weather_date.fristDayCurrTemperature= weather_date.fristDayCurrTemperature*9/5+32;
            /*pcm 2022-09-19 */
            if(weather_date.fristDayCurrTemperature<(-99))
            {
                weather_date.fristDayCurrTemperature=-99;
            }

            for(int i=0; i<7; i++)
            {
                weather_date.dayTemperatureMax[i]= weather_date.dayTemperatureMax[i]*9/5+32;
                weather_date.dayTemperatureMin[i]= weather_date.dayTemperatureMin[i]*9/5+32;
                if(weather_date.dayTemperatureMax[i]<(-99))  weather_date.dayTemperatureMax[i]=-99;
                if(weather_date.dayTemperatureMin[i]<(-99))  weather_date.dayTemperatureMin[i]=-99;
            }
        }
    }

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_WEATHER]);


    if(weather_no_data_flag)
    {
        printf("222222weather_nodata_flag =%d\n",weather_nodata_flag);
        if(func_cb.sta == FUNC_WEATHER)
        {
            f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
            f_weather->no_weather_dada = true;
        }
        ///设置标题栏名字///
        txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
//        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_align_center(txt, false);

        picbox = compo_picturebox_create(frm,UI_BUF_I330001_WEATHER_NO_DATA_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,84+50/2);

        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,160+56/2, GUI_SCREEN_WIDTH,GUI_SCREEN_HEIGHT-(GUI_SCREEN_CENTER_X,160+56/2));
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);

        return frm;
    }

    ///设置标题栏时间///
//    compo_textbox_t* time_title = compo_textbox_create(frm, 32);
//    compo_textbox_set_align_center(time_title, false);
//    compo_textbox_set_location(time_title, 168, 15, 52, 23);
//    compo_setid(time_title, COMPO_ID_TXT_TIME);
//    compo_bonddata(time_title, COMPO_BOND_HOURMIN_TXT);
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_WEATHER_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
//    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
    compo_textbox_set(txt, i18n[STR_WEATHER]);
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
    compo_textbox_set(txt, i18n[STR_WEATHER]);
    compo_textbox_set_align_center(txt, false);

    picbox = compo_picturebox_create(frm, weather_list[get_weather_id[0]].res_addr);///背景图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5);
//    weather_date.fristDayCurrTemperature = 44;
//    if(weather_date.fristDayCurrTemperature == 0)weather_date.fristDayCurrTemperature = 0;
    compo_number_t *num=NULL;
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        num = compo_number_create(frm,UI_BUF_I330001_WEATHER_NUM_BIN, 3);
        compo_number_set_margin(num, 0);
        compo_number_set_radix(num, 11, true);
        compo_number_set(num, weather_date.fristDayCurrTemperature);
        compo_number_set_align(num, 1 );
        compo_number_set_pos(num, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }
    else
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-gui_image_get_size(UI_BUF_I330001_WEATHER_NUM_BIN).wid/2+2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);

        picbox = compo_picturebox_create(frm, UI_BUF_I330001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X+gui_image_get_size(UI_BUF_I330001_WEATHER_NUM_BIN).wid/2-2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }

    if(displayInfo.isFahrenheit)     ///是否为华氏度
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I330001_WEATHER_DF_BIN);///温度符号
    }
    else
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I330001_WEATHER_DC_BIN);///温度符号
    }

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+compo_number_get_rel_location(num).wid/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    else
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+gui_image_get_size(UI_BUF_I330001_WEATHER_NUM_BIN).wid, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_HIGHEST],weather_date.dayTemperatureMax[0]); ///今天 最高温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_HIGHEST]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-4-92, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,92,24);
    compo_textbox_set_right_align(txt, true);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_LOWSET],weather_date.dayTemperatureMin[0]); ///今天 最低温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_LOWSET]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+4, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,92,24);
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm, strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.25);
    compo_textbox_set_forecolor(txt, make_color(0xc2,0xe1,0xf9));

    // printf("height:%d\n",widget_text_get_max_height());
    //第二页
    for(int i=0; i<7; i++)
    {

        txt = compo_textbox_create(frm,20);
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X/6,GUI_SCREEN_HEIGHT+30+(i*36)-8,GUI_SCREEN_WIDTH/5,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,false);
        compo_textbox_set(txt,i18n[STR_SUNDAY+week_sort[i]]);/// 星期
        if(i==0)
        {
            compo_textbox_set(txt,i18n[STR_TO_DAY]);/// 星期
        }
        // printf("y:%d\n",GUI_SCREEN_HEIGHT+30+(i*36)-8);

//        printf("weather_id=%d\n",get_weather_id[i]);
        picbox = compo_picturebox_create(frm,weather_list[get_weather_id[i]].res_addr);/// 天气
        compo_picturebox_set_size(picbox,gui_image_get_size( weather_list[get_weather_id[i]].res_addr).wid/3,\
                                  gui_image_get_size( weather_list[get_weather_id[i]].res_addr).hei/3);
        compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/10-6,GUI_SCREEN_HEIGHT+30+(i*36));

        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            if(displayInfo.isFahrenheit)     ///是否为华氏度
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℉",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
            else
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℃",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --/--");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X*1.2,GUI_SCREEN_HEIGHT+30+(i*36)- 10);
        compo_textbox_set_align_center(txt, false);

    }
    widget_page_set_client(frm->page_body, 0, page_y);

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;

    bool        touch_flag;
    s32         move_offset;
    s32         page_old_y;
    u8          touch_state;
    u8          page_num;
    uint32_t    tick;
    u8          switch_page_state;
} f_weather_t;


enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

static const f_weather_t weather_list[] =
{
    {STR_UNKNOWN,           UI_BUF_I332001_WEATHER_ICON_WEATHER_00_BIN  },                //未知
    {STR_SUNNY,             UI_BUF_I332001_WEATHER_ICON_WEATHER_01_BIN  },                //晴天
    {STR_CLOUDY,            UI_BUF_I332001_WEATHER_ICON_WEATHER_02_BIN  },                //多云
    {STR_OVERCAST,          UI_BUF_I332001_WEATHER_ICON_WEATHER_03_BIN  },                //阴天
    {STR_MODERATE_RAIN,     UI_BUF_I332001_WEATHER_ICON_WEATHER_04_BIN  },                //阵雨
    {STR_RAINY_SHOWERS,     UI_BUF_I332001_WEATHER_ICON_WEATHER_05_BIN  },                //雷阵雨
    {STR_SLEET,             UI_BUF_I332001_WEATHER_ICON_WEATHER_06_BIN  },                //雨夹雪
    {STR_DRIZZLE,           UI_BUF_I332001_WEATHER_ICON_WEATHER_07_BIN  },                //小雨
    {STR_HEAVY_RAIN,        UI_BUF_I332001_WEATHER_ICON_WEATHER_08_BIN  },                //大雨
    {STR_RAINY,             UI_BUF_I332001_WEATHER_ICON_WEATHER_09_BIN  },                //雪
    {STR_SAND_AND_DUST,     UI_BUF_I332001_WEATHER_ICON_WEATHER_10_BIN  },                //沙尘暴
    {STR_HAZE,              UI_BUF_I332001_WEATHER_ICON_WEATHER_11_BIN  },                //雾霾
    {STR_WINDY,             UI_BUF_I332001_WEATHER_ICON_WEATHER_12_BIN  },                //大风
    {STR_SUNNY,             UI_BUF_I332001_WEATHER_ICON_WEATHER_13_BIN  },                //明夜
    {STR_CLOUDY,            UI_BUF_I332001_WEATHER_ICON_WEATHER_14_BIN  },                //云遮月
    {STR_RAINY_SHOWERS,     UI_BUF_I332001_WEATHER_ICON_WEATHER_15_BIN  },                //阵雨夜

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
};
enum
{
    SWITCH_YES=0,
    SWITCH_NO,
    TOTCH_MOVE,
};
static int16_t page_y = 0;
//滑动处理
static void func_weather_move(void)
{
#define   PAGE_TWO_SIZE  558  //最底y轴
#define   TOYCH_LAST_DY  40   //切换页滑动y
#define   TICK_TIME      8   //步进y像素点时间
#define   STEP_NUM       8    //步进y像素点
    f_weather_t *f_sleep = (f_weather_t *)func_cb.f_cb;


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
                page_y = f_sleep->move_offset;
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);
            }
        }
    }

//        printf("move_offset:%d\n",f_sleep->move_offset);
}

static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t  weather_date;
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        if(f_weather->DayWeather[i] != weather_date.DayWeather[i])
        {
            weather_flag = true;
            break;
        }
    }

    if(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true)    //是否为华氏度
    {
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
}

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{
    compo_textbox_t *txt;
    compo_picturebox_t * picbox;
    bool weather_no_data_flag = true;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    u8 week_sort[7];
    u8 week_day=0;
    u8 get_weather_id[7];//存放一个星期的排序
    char str_buff[50];//用于存放打印数据

    memset(get_weather_id,0,sizeof(get_weather_id));

    uteModuleSystemtimeGetTime(&time);//获取系统时间
//    printf("week:%d\n",time.week);
    week_day = time.week; //获取星期
    for(int i=0; i<=6; i++) //星期排序
    {
        week_sort[i] = week_day;
        if(++week_day==7)
        {
            week_day = 0;
        }
    }

    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态
//        printf("year:%d mon:%d day:%d hour:%d min:%d sec:%d\n",weather_date.updateTime.year,weather_date.updateTime.month,weather_date.updateTime.day,weather_date.updateTime.hour,weather_date.updateTime.min,weather_date.updateTime.sec);

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }
//        int pic_num=3;
        for(int i=0; i<7; i++) //获取一周的天气
        {
//            printf("weather:%d\n",weather_date.DayWeather[i]>>8);
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
                {
                    case WEATHER_TYPE_SUNNY:
                        get_weather_id[i] = 13;
                        break;
                    case WEATHER_TYPE_CLOUDY:
                        get_weather_id[i] = 14;
                        break;
                    case WEATHER_TYPE_THUNDERSHOWER_RAIN:
                        get_weather_id[i] = 15;
                        break;
                }
            }
            get_weather_id[i] = weather_date.DayWeather[i]>>8;//赋值排序天气状态
        }

        if(displayInfo.isFahrenheit)    //是否为华氏度
        {
            weather_date.fristDayCurrTemperature= weather_date.fristDayCurrTemperature*9/5+32;
            /*pcm 2022-09-19 */
            if(weather_date.fristDayCurrTemperature<(-99))
            {
                weather_date.fristDayCurrTemperature=-99;
            }

            for(int i=0; i<7; i++)
            {
                weather_date.dayTemperatureMax[i]= weather_date.dayTemperatureMax[i]*9/5+32;
                weather_date.dayTemperatureMin[i]= weather_date.dayTemperatureMin[i]*9/5+32;
                if(weather_date.dayTemperatureMax[i]<(-99))  weather_date.dayTemperatureMax[i]=-99;
                if(weather_date.dayTemperatureMin[i]<(-99))  weather_date.dayTemperatureMin[i]=-99;
            }
        }
    }

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_WEATHER]);


    if(weather_no_data_flag)
    {
        ///设置标题栏名字///
        txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
//        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_align_center(txt, false);

        picbox = compo_picturebox_create(frm,UI_BUF_I332001_WEATHER_NO_DATA_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,84+50/2);

        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,160+56/2, GUI_SCREEN_WIDTH,GUI_SCREEN_HEIGHT-(GUI_SCREEN_CENTER_X,160+56/2));
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);

        return frm;
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_WEATHER_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    picbox = compo_picturebox_create(frm, weather_list[get_weather_id[0]].res_addr);///背景图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5);
    compo_number_t *num=NULL;
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        num = compo_number_create(frm,UI_BUF_I332001_WEATHER_NUM_BIN, 3);
        compo_number_set_margin(num, 0);
        compo_number_set_radix(num, 11, true);
        compo_number_set(num, weather_date.fristDayCurrTemperature);
        compo_number_set_align(num, 1 );
        compo_number_set_pos(num, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }
    else
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I332001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-gui_image_get_size(UI_BUF_I332001_WEATHER_NUM_BIN).wid/2+2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);

        picbox = compo_picturebox_create(frm, UI_BUF_I332001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X+gui_image_get_size(UI_BUF_I332001_WEATHER_NUM_BIN).wid/2-2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }

    if(displayInfo.isFahrenheit)     ///是否为华氏度
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I332001_WEATHER_DF_BIN);///温度符号
    }
    else
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I332001_WEATHER_DC_BIN);///温度符号
    }

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+compo_number_get_rel_location(num).wid/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    else
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+gui_image_get_size(UI_BUF_I332001_WEATHER_NUM_BIN).wid, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_HIGHEST],weather_date.dayTemperatureMax[0]); ///今天 最高温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_HIGHEST]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-4-92, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,92,24);
    compo_textbox_set_right_align(txt, true);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_LOWSET],weather_date.dayTemperatureMin[0]); ///今天 最低温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_LOWSET]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+4, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,92,24);
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm, strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.25);
    compo_textbox_set_forecolor(txt, make_color(0xc2,0xe1,0xf9));

    //第二页

    //设置标题栏名字//
    txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
    compo_textbox_set(txt, i18n[STR_WEATHER]);
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/2,390+38/2,120,38);
    compo_textbox_set(txt, i18n[STR_WEATHER]);

    for(int i=0; i<7; i++)
    {
        if(i!=6)
        {
            compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+136+(i*57), 316, 1);
            compo_shape_set_color(shape, make_color(0X3D,0X3D,0X3D));
        }

        txt = compo_textbox_create(frm,20);
        compo_textbox_set_location(txt, 22,GUI_SCREEN_HEIGHT+95+(i*57),90,34);
        compo_textbox_set(txt,i18n[STR_SUNDAY+week_sort[i]]);/// 星期
        compo_textbox_set_align_center(txt,false);
        if(i==0)
        {
            compo_textbox_set(txt,i18n[STR_TO_DAY]);/// 星期
        }

        picbox = compo_picturebox_create(frm,weather_list[get_weather_id[i]].res_addr);/// 天气
        compo_picturebox_set_size(picbox,32,32);
        compo_picturebox_set_pos(picbox, 163,GUI_SCREEN_HEIGHT+108+(i*57));

        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            if(displayInfo.isFahrenheit)     ///是否为华氏度
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℉",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
            else
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℃",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --/-- ");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_location(txt, 230,GUI_SCREEN_HEIGHT+95+(i*57),108,34);
        compo_textbox_set_align_center(txt, false);

    }
    widget_page_set_client(frm->page_body, 0, page_y);

    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//天气功能事件处理
static void func_weather_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    compo_page_move_process(f_weather->ptm);
//    printf("page:%d\n",compo_page_move_get_offset(f_weather->ptm));
    page_y = compo_page_move_get_offset(f_weather->ptm);
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    func_weather_move();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_process();
}

//天气功能消息处理
static void func_weather_message(size_msg_t msg)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
            f_weather->touch_flag = true;
#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            if(!f_weather->no_weather_dada)
            {
                compo_page_move_touch_handler(f_weather->ptm);
            }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;
        case MSG_SYS_500MS:
            weather_refresh();
            break;
        case MSG_CTP_CLICK:
        default:
            func_message(msg);
            break;
    }
}

//进入天气功能
static void func_weather_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_weather_t));
    func_cb.frm_main = func_weather_form_create();
    f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_weather->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
//        .title_used = true,
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = GUI_SCREEN_HEIGHT,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_weather->ptm, func_cb.frm_main->page_body, &info);
    weather_data_Init();
    f_weather->ptm->move_offset = page_y;
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    weather_data_Init();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//退出天气功能
static void func_weather_exit(void)
{
    f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
    func_cb.last = FUNC_WEATHER;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    if(sys_cb.refresh_language_flag == false)
    {
        page_y = 0;
    }
    if (f_weather->ptm)
    {
        func_free(f_weather->ptm);
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    if(sys_cb.refresh_language_flag == false)
    {
        page_y = 0;
    }
    if (f_weather->ptm)
    {
        func_free(f_weather->ptm);
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//天气功能
void func_weather(void)
{
    printf("%s\n", __func__);
    func_weather_enter();
    while (func_cb.sta == FUNC_WEATHER)
    {
        func_weather_process();
        func_weather_message(msg_dequeue());
    }
    func_weather_exit();
}

#endif
