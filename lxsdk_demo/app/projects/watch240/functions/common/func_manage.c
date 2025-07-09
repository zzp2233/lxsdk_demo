#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

/**
 * -------------function item infomation-----------------
            任务项id、图标资源、标题序号信息
 **/
typedef struct
{
    u8 func_sta;
    u32 res_addr;
    u32 str_idx;
} func_item_info_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
#define FUNC_ITEM_CNT   (sizeof(func_item_tbl) / sizeof(func_item_tbl[0]))
const static func_item_info_t func_item_tbl[] =
{
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {FUNC_HEARTRATE,                UI_BUF_I330001_THEME_1_HEART_BIN,                 STR_HEART_RATE},            //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {FUNC_BT,                       UI_BUF_I330001_THEME_1_MUSIC_BIN,                      STR_MUSIC},                 //蓝牙播放器(控制手机音乐)
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {FUNC_ALARM_CLOCK,              UI_BUF_I330001_THEME_1_ALARM_BIN,                STR_ALARM_CLOCK},           //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {FUNC_BLOOD_OXYGEN,             UI_BUF_I330001_THEME_1_SPO2_BIN,               STR_BLOOD_OXYGEN},          //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//    {FUNC_BLOODSUGAR,               UI_BUF_ICON_BLOODSUGAR_BIN,                 STR_BLOOD_SUGAR},           //血糖
//    {FUNC_BLOOD_PRESSURE,           UI_BUF_ICON_BLOOD_PRESSURE_BIN,             STR_BLOOD_PRESSURE},        //血压
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {FUNC_BREATHE,                  UI_BUF_I330001_THEME_1_BREATHE_BIN,                    STR_BREATHE_TRAIN},               //呼吸
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {FUNC_CALCULATOR,               UI_BUF_I330001_THEME_1_CALCULATOR_BIN,                 STR_CALCULATOR},            //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {FUNC_CAMERA,                   UI_BUF_I330001_THEME_1_PHOTO_BIN,                     STR_CAMERA},                //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {FUNC_LIGHT,                    UI_BUF_I330001_THEME_1_BRIGHTNESS_BIN,                STR_SETTING_LIGHT},         //亮度调节
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {FUNC_TIMER,                    UI_BUF_I330001_THEME_1_TIMER_BIN,                      STR_TIMER},                 //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {FUNC_SLEEP,                    UI_BUF_I330001_THEME_1_SLEEP_BIN,                      STR_SLEEP},                 //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {FUNC_STOPWATCH,                UI_BUF_I330001_THEME_1_STOPWATCH_BIN,                  STR_STOP_WATCH},            //秒表
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {FUNC_WEATHER,                  UI_BUF_I330001_THEME_1_WEATHER_BIN,                    STR_WEATHER},               //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
//    {FUNC_PRESSURE,                 UI_BUF_ICON_PRESSURE_BIN,                   STR_PRESSURE},              //压力

#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {FUNC_GAME,                     UI_BUF_I330001_THEME_1_GAME_BIN,                       STR_GAME},                  //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
//    {FUNC_ALTITUDE,                 UI_BUF_ICON_ALTITUDE_BIN,                   STR_ALTITUDE},              //海拔
//    {FUNC_MAP,                      UI_BUF_ICON_MAP_BIN,                        STR_MAP},                   //地图
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {FUNC_MESSAGE,                  UI_BUF_I330001_THEME_1_MESSAGE_BIN,                    STR_MESSAGE},               //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    {FUNC_SCAN,                     UI_BUF_I330001_THEME_1_QR_BIN,                       STR_QRCODE},                  //扫一扫
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {FUNC_VOICE,                    UI_BUF_I330001_THEME_1_VIOCE_BIN,                      STR_VOICE},                 //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
//    {FUNC_COMPASS,                  UI_BUF_ICON_COMPASS_BIN,                    STR_COMPASS},               //指南针
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {FUNC_SPORT,                    UI_BUF_I330001_THEME_1_SOPRT_BIN,                      STR_SPORTS},                //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {FUNC_CALL,                     UI_BUF_I330001_THEME_1_CALL_BIN,                       STR_PHONE},                 //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
    {FUNC_FINDPHONE,                UI_BUF_I330001_THEME_1_FINDPHONE_BIN,                  STR_FIND_PHONE},            //寻找手机
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {FUNC_CALENDAER,                UI_BUF_I330001_THEME_1_CALENDAR_BIN,                   STR_CALENDAR},              //日历
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {FUNC_SET_SUB_SAV,                   UI_BUF_I330001_THEME_1_SOUND_BIN,                     STR_VOL},                //音量
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {FUNC_ACTIVITY,                 UI_BUF_I330001_THEME_1_ACTIVITY_BIN,                   STR_EVREY_DAY_ACTIVITY},       //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {FUNC_FLASHLIGHT,               UI_BUF_I330001_THEME_1_FLASHLIGHT_BIN,                 STR_FLASHLIGHT},            //手电筒
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {FUNC_SETTING,                  UI_BUF_I330001_THEME_1_SETTINGS_BIN,                    STR_SETTING},               //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
    {FUNC_LANGUAGE,                 UI_BUF_I330001_THEME_1_LANUAGE_BIN,                   STR_SETTING_LANGUAGE},      //设置--语言
    {FUNC_RESTART,                  UI_BUF_I330001_THEME_1_BOOT_BIN,                    STR_SETTING_RESTART},       //重启
    {FUNC_RSTFY,                    UI_BUF_I330001_THEME_1_RETRY_BIN,            STR_SETTING_RSTFY},         //设置--恢复出厂
    {FUNC_SET_SUB_ABOUT,            UI_BUF_I330001_THEME_1_GUANYU_BIN,            STR_SETTING_ABOUT},         //设置--关于
//    {FUNC_OFF,                      UI_BUF_ICON_OFF_BIN,                        STR_SETTING_OFF},           //关机
};
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define FUNC_ITEM_CNT   (sizeof(func_item_tbl) / sizeof(func_item_tbl[0]))
const static func_item_info_t func_item_tbl[] =
{
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {FUNC_HEARTRATE,                UI_BUF_I332001_THEME_ICON1_HEART_BIN,                 STR_HEART_RATE},            //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {FUNC_BT,                       UI_BUF_I332001_THEME_ICON1_MUSIC_BIN,                      STR_MUSIC},                 //蓝牙播放器(控制手机音乐)
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {FUNC_ALARM_CLOCK,              UI_BUF_I332001_THEME_ICON1_ALARM_BIN,                STR_ALARM_CLOCK},           //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {FUNC_BLOOD_OXYGEN,             UI_BUF_I332001_THEME_ICON1_SPO2_BIN,               STR_BLOOD_OXYGEN},          //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//    {FUNC_BLOODSUGAR,               UI_BUF_ICON_BLOODSUGAR_BIN,                 STR_BLOOD_SUGAR},           //血糖
//    {FUNC_BLOOD_PRESSURE,           UI_BUF_ICON_BLOOD_PRESSURE_BIN,             STR_BLOOD_PRESSURE},        //血压
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {FUNC_BREATHE,                  UI_BUF_I332001_THEME_ICON1_BREATHE_BIN,                    STR_BREATHE_TRAIN},               //呼吸
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {FUNC_CALCULATOR,               UI_BUF_I332001_THEME_ICON1_CALCULATOR_BIN,                 STR_CALCULATOR},            //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {FUNC_CAMERA,                   UI_BUF_I332001_THEME_ICON1_PHOTO_BIN,                     STR_CAMERA},                //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {FUNC_LIGHT,                    UI_BUF_I332001_THEME_ICON1_BRIGHTNESS_BIN,                      STR_SETTING_LIGHT},         //亮度调节
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {FUNC_TIMER,                    UI_BUF_I332001_THEME_ICON1_TIMER_BIN,                      STR_TIMER},                 //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {FUNC_SLEEP,                    UI_BUF_I332001_THEME_ICON1_SLEEP_BIN,                      STR_SLEEP},                 //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {FUNC_STOPWATCH,                UI_BUF_I332001_THEME_ICON1_STOPWATCH_BIN,                  STR_STOP_WATCH},            //秒表
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {FUNC_WEATHER,                  UI_BUF_I332001_THEME_ICON1_WEATHER_BIN,                    STR_WEATHER},               //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
//    {FUNC_PRESSURE,                 UI_BUF_ICON_PRESSURE_BIN,                   STR_PRESSURE},              //压力

#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {FUNC_GAME,                     UI_BUF_I332001_THEME_ICON1_GAME_BIN,                       STR_GAME},                  //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
//    {FUNC_ALTITUDE,                 UI_BUF_ICON_ALTITUDE_BIN,                   STR_ALTITUDE},              //海拔
//    {FUNC_MAP,                      UI_BUF_ICON_MAP_BIN,                        STR_MAP},                   //地图
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {FUNC_MESSAGE,                  UI_BUF_I332001_THEME_ICON1_MESSAGE_BIN,                    STR_MESSAGE},               //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    {FUNC_SCAN,                     UI_BUF_I332001_THEME_ICON1_QR_BIN,                       STR_QRCODE},                  //扫一扫
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {FUNC_VOICE,                    UI_BUF_I332001_THEME_ICON1_VIOCE_BIN,                      STR_VOICE},                 //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
//    {FUNC_COMPASS,                  UI_BUF_ICON_COMPASS_BIN,                    STR_COMPASS},               //指南针
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {FUNC_SPORT,                    UI_BUF_I332001_THEME_ICON1_SOPRT_BIN,                      STR_SPORTS},                //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {FUNC_CALL,                     UI_BUF_I332001_THEME_ICON1_CALL_BIN,                       STR_PHONE},                 //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
    {FUNC_FINDPHONE,                UI_BUF_I332001_THEME_ICON1_FINDPHONE_BIN,                  STR_FIND_PHONE},            //寻找手机
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {FUNC_CALENDAER,                UI_BUF_I332001_THEME_ICON1_CALENDAR_BIN,                   STR_CALENDAR},              //日历
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {FUNC_SET_SUB_SAV,                   UI_BUF_I332001_THEME_ICON1_SOUND_BIN,                     STR_VOL},                //音量
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {FUNC_ACTIVITY,                 UI_BUF_I332001_THEME_ICON1_ACTIVITY_BIN,                   STR_EVREY_DAY_ACTIVITY},       //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {FUNC_FLASHLIGHT,               UI_BUF_I332001_THEME_ICON1_FLASHLIGHT_BIN,                 STR_FLASHLIGHT},            //手电筒
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {FUNC_SETTING,                  UI_BUF_I332001_THEME_ICON1_SETTINGS_BIN,                    STR_SETTING},               //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
    {FUNC_LANGUAGE,                 UI_BUF_I332001_THEME_ICON1_LANUAGE_BIN,                   STR_SETTING_LANGUAGE},      //设置--语言
    {FUNC_RESTART,                  UI_BUF_I332001_THEME_ICON1_BOOT_BIN,                    STR_SETTING_RESTART},       //重启
    {FUNC_RSTFY,                    UI_BUF_I332001_THEME_ICON1_RETRY_BIN,            STR_SETTING_RSTFY},         //设置--恢复出厂
    {FUNC_SET_SUB_ABOUT,            UI_BUF_I332001_THEME_ICON1_GUANYU_BIN,            STR_SETTING_ABOUT},         //设置--关于
//    {FUNC_OFF,                      UI_BUF_ICON_OFF_BIN,                        STR_SETTING_OFF},           //关机
};
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
#define FUNC_ITEM_CNT   (sizeof(func_item_tbl) / sizeof(func_item_tbl[0]))
const static func_item_info_t func_item_tbl[] =
{
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {.func_sta=FUNC_CALL,                     .res_addr=UI_BUF_I335001_2_HONEYCOMB_CALL_BIN,                   .str_idx=STR_PHONE},                 //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {.func_sta=FUNC_ACTIVITY,                 .res_addr=UI_BUF_I335001_2_HONEYCOMB_ACTIVITY_BIN,               .str_idx=STR_EVREY_DAY_ACTIVITY},    //每日活动
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {.func_sta=FUNC_HEARTRATE,                .res_addr=UI_BUF_I335001_2_HONEYCOMB_HEART_RATE_BIN,             .str_idx=STR_HEART_RATE},            //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
// #if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {.func_sta=FUNC_HEARTRATE,                .res_addr=UI_BUF_I335001_2_HONEYCOMB_STRESS_BIN,                 .str_idx=STR_HEART_RATE},            //压力
// #endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {.func_sta=FUNC_SPORT,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_EXERCISE_BIN,               .str_idx=STR_SPORTS},                //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {.func_sta=FUNC_BLOOD_OXYGEN,             .res_addr=UI_BUF_I335001_2_HONEYCOMB_BLOOD_OXYGEN_BIN,           .str_idx=STR_BLOOD_OXYGEN},          //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
// #if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {.func_sta=FUNC_BLOOD_OXYGEN,             .res_addr=UI_BUF_I335001_2_HONEYCOMB_BLOOD_PRESSURE_BIN,         .str_idx=STR_BLOOD_OXYGEN},          //血压
// #endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {.func_sta=FUNC_SLEEP,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_SLEEP_BIN,                 .str_idx=STR_SLEEP},                 //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {.func_sta=FUNC_BT,                       .res_addr=UI_BUF_I335001_2_HONEYCOMB_MUSIC_BIN,                 .str_idx=STR_MUSIC},                 //音乐
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {.func_sta=FUNC_WEATHER,                  .res_addr=UI_BUF_I335001_2_HONEYCOMB_WEATHER_BIN,               .str_idx=STR_WEATHER},               //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {.func_sta=FUNC_ALARM_CLOCK,              .res_addr=UI_BUF_I335001_2_HONEYCOMB_ALARM_BIN,                 .str_idx=STR_ALARM_CLOCK},           //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
// #if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {.func_sta=FUNC_HEARTRATE,                .res_addr=UI_BUF_I335001_2_HONEYCOMB_MOOD_BIN,                  .str_idx=STR_HEART_RATE},            //情绪
// #endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {.func_sta=FUNC_BREATHE,                  .res_addr=UI_BUF_I335001_2_HONEYCOMB_BREATHE_BIN,               .str_idx=STR_BREATHE_TRAIN},         //呼吸
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {.func_sta=FUNC_MESSAGE,                  .res_addr=UI_BUF_I335001_2_HONEYCOMB_INFORMATION_BIN,           .str_idx=STR_MESSAGE},               //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {.func_sta=FUNC_VOICE,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_VOICE_ASSISTANT_BIN,       .str_idx=STR_VOICE},                 //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
// #if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {.func_sta=FUNC_VOICE,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_SOS_BIN,                   .str_idx=STR_VOICE},                 //SOS
// #endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {.func_sta=FUNC_GAME,                     .res_addr=UI_BUF_I335001_2_HONEYCOMB_GAME_BIN,                  .str_idx=STR_GAME},                 //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
// #if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {.func_sta=FUNC_VOICE,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_TOOLBOX_BIN,               .str_idx=STR_VOICE},                 //工具箱
// #endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {.func_sta=FUNC_SETTING,                  .res_addr=UI_BUF_I335001_2_HONEYCOMB_SETTINGS_BIN,              .str_idx=STR_SETTING},               //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
// #if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {.func_sta=FUNC_WOMEN_HEALTH,             .res_addr=UI_BUF_I335001_2_HONEYCOMB_PERIOD_BIN,                .str_idx=STR_WOMEN_HEALTH},          //女性健康
// #endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {.func_sta=STR_STOP_WATCH,                .res_addr=UI_BUF_I335001_2_HONEYCOMB_STOPWATCH_BIN,             .str_idx = FUNC_STOPWATCH},            //秒表
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {.func_sta=STR_TIMER,                     .res_addr=UI_BUF_I335001_2_HONEYCOMB_TIMER_BIN,                 .str_idx = FUNC_TIMER},                //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
    {.func_sta=STR_FIND_PHONE,                .res_addr=UI_BUF_I335001_2_HONEYCOMB_FIND_PHONE_BIN,            .str_idx = FUNC_FINDPHONE},            //查找手机
    {.func_sta=STR_FLASHLIGHT,                .res_addr=UI_BUF_I335001_2_HONEYCOMB_FLASHLIGHT_BIN,            .str_idx = FUNC_FLASHLIGHT},           //手电筒
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {.func_sta=STR_CAMERA,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_REMOTE_CAMERA_BIN,         .str_idx = FUNC_CAMERA},               //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {.func_sta=STR_CALCULATOR,               .res_addr=UI_BUF_I335001_2_HONEYCOMB_CALCULATOR_BIN,             .str_idx = FUNC_CALCULATOR},           //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
};
#else
#define FUNC_ITEM_CNT   (sizeof(func_item_tbl) / sizeof(func_item_tbl[0]))
const static func_item_info_t func_item_tbl[] =
{
    {0,0,0},
};
#endif

/**
 * @brief 获取任务图标资源地址
 * @param[in] func_sta : 任务id
 * @return 资源地址
 **/
u32 func_get_icon_addr(u8 func_sta)
{
    for (u8 i = 0; i < FUNC_ITEM_CNT; i++)
    {
        if (func_item_tbl[i].func_sta == func_sta)
        {
            return func_item_tbl[i].res_addr;
        }
    }
    return 0;
}

/**
 * @brief 获取任务标题字符串序号
 * @param[in] func_sta : 任务id
 * @return 标题字符串序号
 **/
u32 func_get_str_idx(u8 func_sta)
{
    for (u8 i = 0; i < FUNC_ITEM_CNT; i++)
    {
        if (func_item_tbl[i].func_sta == func_sta)
        {
            return func_item_tbl[i].str_idx;
        }
    }
    return 0;
}


/**
 * -------------gui stack manage-----------------
    根据界面跳转的复杂程度是否入栈管理
 **/
typedef struct
{
    u8 task_tbl[TASK_STACK_MAX];
    u8 num;
} task_stack_t;

static task_stack_t task_stack;

/**
 * @brief 堆栈初始化
 **/
void task_stack_init(void)
{
    task_stack.num = 0;
    func_cb.flag_sort = 0;
    func_cb.menu_idx = 0;
}

/**
 * @brief 入栈
          目前首先入栈的是时钟表盘
 * @param[in] val : 入栈的值
 **/
void task_stack_push(u8 val)
{
    printf("%s:[", __func__);
    for (u8 i = 0; i < task_stack.num; i++)      //有重复值直接回退到第一次入栈处
    {
        if (task_stack.task_tbl[i] == val)
        {
            task_stack.num = i + 1;
            printf("%d]<--[%d]\n", val, val);
            return;
        }
        else
        {
            printf("%d ", task_stack.task_tbl[i]);
        }
    }
    if (task_stack.num < TASK_STACK_MAX)
    {
        task_stack.task_tbl[task_stack.num] = val;
        task_stack.num++;
        printf("%d]<--[%d]\n", val, val);
    }
    else
    {
        printf("]<--[%d]\n", val, val);
    }
}

/**
 * @brief 出栈
 * @return 出栈后，处于栈顶的值
 **/
u8 task_stack_pop(void)
{
    printf("%s", __func__);
    TRACE(":[");
    u8 val = 0;
#if TRACE_EN
    if (task_stack.num > 0)
    {
        for (u8 i = 0; i < (task_stack.num - 1); i++)
        {
            if (i == task_stack.num - 2)
            {
                val = task_stack.task_tbl[i];
                TRACE("%d", val);
            }
            else
            {
                TRACE("%d ", task_stack.task_tbl[i]);
            }
        }
        task_stack.num--;
        TRACE("]-->[%d]\n", task_stack.task_tbl[task_stack.num]);
    }
    else
    {
        TRACE("]-->[]\n");
    }
#else
    if (task_stack.num > 0)
    {
        task_stack.num--;
        if (task_stack.num)
        {
            val = task_stack.task_tbl[task_stack.num - 1];
        }
    }
    printf("[top:%d]\n", val);
#endif // TRACE_EN
    return val;
}

/**
 * @brief 获取栈顶
 * @return 栈顶的值
 **/
u8 task_stack_get_top(void)
{
    if (task_stack.num > 0)
    {
        return task_stack.task_tbl[task_stack.num - 1];
    }
    return 0;
}

/**
 * @brief 获取栈顶前一个任务值
 * @return 栈顶前一个任务值
 **/
u8 task_stack_get_last(void)
{
    if (task_stack.num > 1)
    {
        return task_stack.task_tbl[task_stack.num - 2];
    }
    return 0;
}

/**
 * @brief 查找栈中是否存在某个界面
 * @param[in] val : 要查找的值
 * @return 存在返回true，不存在返回false
 **/
bool task_stack_contains(u8 val)
{
    for (u8 i = 0; i < task_stack.num; i++)
    {
        if (task_stack.task_tbl[i] == val)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief 删除栈中的某个界面
 * @param[in] val : 要删除的值
 **/
void task_stack_remove(u8 val)
{
    printf("%s:[", __func__);
    u8 write_idx = 0;
    for (u8 i = 0; i < task_stack.num; i++)
    {
        if (task_stack.task_tbl[i] != val)
        {
            task_stack.task_tbl[write_idx] = task_stack.task_tbl[i];
            printf("%d ", task_stack.task_tbl[write_idx]);
            write_idx++;
        }
    }
    printf("]");
    if (task_stack.num > write_idx)
    {
        printf("-->[%d]", val);
    }
    printf("\n");
    task_stack.num = write_idx;
}

/**
 * @brief 在当前栈顶元素之前插入一个新界面
 * @param[in] val : 要插入的值
 **/
void task_stack_insert_before_top(u8 val)
{
    // 检查栈是否为空或只有一个元素
    if (task_stack.num <= 1)
    {
        task_stack_push(val);
        return;
    }

    // 检查栈是否已满
    if (task_stack.num >= TASK_STACK_MAX)
    {
        printf("Failed: Stack is full. (%d/%d)\n", task_stack.num, TASK_STACK_MAX);
        return;
    }

    // 当前栈顶位置
    u8 top_idx = task_stack.num - 1;
    // 栈顶前一个位置
    u8 prev_idx = top_idx - 1;

    // 将栈顶元素之后的元素后移一位
    for (u8 i = task_stack.num; i > prev_idx + 1; i--)
    {
        task_stack.task_tbl[i] = task_stack.task_tbl[i - 1];
    }

    // 插入新值
    task_stack.task_tbl[prev_idx + 1] = val;
    task_stack.num++;

    printf("Val %d inserted after prev. New stack: [", val);
    for (u8 i = 0; i < task_stack.num; i++)
    {
        printf("%d ", task_stack.task_tbl[i]);
    }
    printf("]\n");
}

/**
* -----------------latest tasks manage-----------------
                      历史任务管理
**/
typedef struct
{
    u8 task_tbl[LATEST_TASK_MAX];
    u8 num;
} latest_list_t;

static latest_list_t latest_list;

//最近任务允许记录的id
#define LATEST_ALLOWED_CNT  (sizeof(latest_allowed_id) / sizeof(latest_allowed_id[0]))
const static u8 latest_allowed_id[] =
{
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    FUNC_HEARTRATE,                     //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    FUNC_BT,                            //蓝牙播放器(控制手机音乐)
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    FUNC_ALARM_CLOCK,                   //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    FUNC_BLOOD_OXYGEN,                  //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//    FUNC_BLOODSUGAR,                    //血糖
//    FUNC_BLOOD_PRESSURE,                //血压
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    FUNC_BREATHE,                       //呼吸
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    FUNC_CALCULATOR,                    //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    FUNC_CAMERA,                        //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    FUNC_LIGHT,                         //亮度调节
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    FUNC_TIMER,                         //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    FUNC_SLEEP,                         //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    FUNC_STOPWATCH,                     //秒表
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    FUNC_WEATHER,                       //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    FUNC_GAME,                          //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
//    FUNC_ALTITUDE,                      //海拔
//    FUNC_MAP,                           //地图
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    FUNC_MESSAGE,                       //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    FUNC_SCAN,                          //扫一扫
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    FUNC_VOICE,                         //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
//    FUNC_COMPASS,                       //指南针
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    FUNC_SPORT,                         //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    FUNC_CALL,                          //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
    FUNC_FINDPHONE,                     //寻找手机
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    FUNC_CALENDAER,                     //日历
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    FUNC_SET_SUB_SAV,                        //音量
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    FUNC_ACTIVITY,                      //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    FUNC_FLASHLIGHT,                    //手电筒
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    FUNC_SETTING,                       //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
    FUNC_LANGUAGE,                      //设置--语言
    FUNC_RESTART,                       //重启
    FUNC_RSTFY,                         //设置--恢复出厂
//    FUNC_OFF,                           //关机
    FUNC_SET_SUB_ABOUT,                 //关于
};

/**
 * @brief 最近任务列表初始化
 **/
void latest_task_init(void)
{
    latest_list.num = 0;
}

/**
 * @brief 获取记录的最近任务个数
 * @return 最近任务个数
 **/
u8 latest_task_count(void)
{
    return latest_list.num;
}

/**
 * @brief 插入最近任务到列表头部，尾部元素先入先出
          注意：idx=0为最新
 * @param[in] func_sta : 任务id
 **/
void latest_task_add(u8 func_sta)
{
    u8 i;
    u8 id = FUNC_NULL;
    u8 tail = latest_list.num;
    for (i = 0; i < tail; i++)      //查重
    {
        if (func_sta == latest_list.task_tbl[i])
        {
            while (i > 0)
            {
                latest_list.task_tbl[i] = latest_list.task_tbl[i - 1];
                i--;
            }
            latest_list.task_tbl[0] = func_sta;
            TRACE("%s[%d]\n", __func__, func_sta);
            return;
        }
    }
    for (i = 0; i < LATEST_ALLOWED_CNT; i++)     //过滤
    {
        if (func_sta == latest_allowed_id[i])
        {
            id = func_sta;
            break;
        }
    }
    if (id != FUNC_NULL && func_get_icon_addr(func_sta))
    {
        if (latest_list.num >= LATEST_TASK_MAX)    //添加
        {
            tail = LATEST_TASK_MAX - 1;
        }
        for ( ; tail > 0; tail--)
        {
            latest_list.task_tbl[tail] = latest_list.task_tbl[tail - 1];
        }
        latest_list.task_tbl[0] = func_sta;
        latest_list.num = MIN(latest_list.num + 1, LATEST_TASK_MAX);
#if (TRACE_EN == 0)
        printf("%s[sta:%d num:%d]\n", __func__, func_sta, latest_list.num);
#endif
    }

#if TRACE_EN
    TRACE("%s[%d]:", __func__, func_sta);
    if (latest_list.num)
    {
        for (i = 0; i < (latest_list.num - 1); i++)
        {
            TRACE("%d,", latest_list.task_tbl[i]);
        }
        TRACE("%d", latest_list.task_tbl[i]);
    }
    TRACE("\n");
#endif
}

/**
 * @brief 通过下标（0最新）删除最近任务列表中的任务
 * @param[in] idx : 任务在列表中的序号
 **/
void latest_task_del(u8 idx)
{
    u8 i = idx;
    if (i < latest_list.num)
    {
        latest_list.num--;
        for ( ; i < latest_list.num; i++)
        {
            latest_list.task_tbl[i] = latest_list.task_tbl[i + 1];
        }
#if (TRACE_EN == 0)
        printf("%s[idx:%d num:%d]\n", __func__, idx, latest_list.num);
#endif
    }

#if TRACE_EN
    TRACE("%s[%d]:", __func__, idx);
    if (latest_list.num)
    {
        for (i = 0; i < (latest_list.num - 1); i++)
        {
            TRACE("%d,", latest_list.task_tbl[i]);
        }
        TRACE("%d", latest_list.task_tbl[i]);
    }
    TRACE("\n");
#endif // TRACE_EN
}

/**
 * @brief 通过下标（0最新）获取最近任务列表中的任务id
 * @param[in] idx : 任务在列表中的序号
 * @return 任务idl
 **/
u8 latest_task_get(u8 idx)
{
    if (idx < latest_list.num)
    {
        return latest_list.task_tbl[idx];
    }
    return FUNC_NULL;
}

/**
 * @brief 最近任务列表中查找任务
 * @param[in] func_sta : 任务id
 * @return 任务在列表中的序号（没有则返回-1）
 **/
s8 latest_task_find(u8 func_sta)
{
    for (u8 i = 0; i < (latest_list.num); i++)
    {
        if (latest_list.task_tbl[i] == func_sta)
        {
            return i;
        }
    }
    return -1;
}
