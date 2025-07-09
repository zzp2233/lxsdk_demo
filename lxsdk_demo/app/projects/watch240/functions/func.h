#ifndef _FUNC_H
#define _FUNC_H

#include "common/msgbox.h"
#include "common/listbox.h"
#include "common/rotary.h"
#include "common/func_switching.h"
#include "func_clock.h"
#include "func_bt.h"
#include "common/func_idle.h"
#include "common/func_lowpwr.h"
#include "common/func_manage.h"
#include "common/func_update.h"
#include "func_bt_dut.h"
#include "func_music.h"
#include "func_usbdev.h"
#include "func_recorder.h"

#define TICK_IGNORE_KEY            700      //忽略700ms内的部分消息

//task number
enum
{
    FUNC_NULL = 0,
    FUNC_MENU,                          //主菜单
    FUNC_MENUSTYLE,                     //主菜单样式选择
    FUNC_CLOCK,                         //时钟表盘
    FUNC_CLOCK_PREVIEW,                 //时钟表盘预览
    FUNC_SIDEBAR,                       //表盘右滑
    FUNC_CARD,                          //表盘上拉
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    FUNC_HEARTRATE,                     //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    FUNC_BT,                            //蓝牙播放器(控制手机音乐)
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
    FUNC_COMPO_SELECT,                  //组件选择
    FUNC_COMPO_SELECT_SUB,              //组件选择子界面
    FUNC_BT_RING,                       //来电界面
    FUNC_BT_CALL,                       //通话界面
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    FUNC_ALARM_CLOCK,                   //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
    FUNC_ALARM_CLOCK_SUB_SET,           //闹钟--设置
    FUNC_ALARM_CLOCK_SUB_REPEAT,        //闹钟--重复
    FUNC_ALARM_CLOCK_SUB_EDIT,          //闹钟--编辑
    FUNC_ALARM_CLOCK_SUB_POP,           //闹钟--弹出
    FUNC_SMOKE_TARGET_SUB_POP,           //抽烟目标达成
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    FUNC_BLOOD_OXYGEN,                  //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    FUNC_PRESSURE,                      //压力
    FUNC_PRESSURE_EXPLAIN,              //压力说明
    FUNC_LONG_PRESS,                    //关机 重启 SOS
    FUNC_BLOODSUGAR,                    //血糖
    FUNC_BLOOD_PRESSURE,                //血压
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    FUNC_BREATHE,                       //呼吸
    FUNC_BREATHE_SUB_MODE,              //呼吸--模式设置
    FUNC_BREATHE_SUB_TIME,              //呼吸--时间设置
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
    FUNC_STOPWATCH_SUB_RECORD,          //秒表--秒表记录
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    FUNC_WEATHER,                       //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    FUNC_GAME,                          //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    FUNC_STYLE,                         //菜单风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    FUNC_ALTITUDE,                      //海拔
    FUNC_MAP,                           //地图
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    FUNC_MESSAGE,                       //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    FUNC_SCAN,                          //扫一扫
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
    FUNC_BREATHE_FINISH,
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    FUNC_VOICE,                         //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
//    FUNC_COMPASS,                       //指南针
#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
    FUNC_ADDRESS_BOOK,                  //电话簿
#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
    FUNC_CALL_SUB_LINKMAN = FUNC_ADDRESS_BOOK,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    FUNC_SPORT,                         //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
    FUNC_SPORT_CONFIG,                  //运动配置
    FUNC_SPORT_SUB_RUN,                 //运动--室内跑步
    FUNC_SPORT_SWITCH,                  //运动开启
    FUNC_SPORT_SORT,                    //运动边菜单
    FUNC_SPORT_FINISH,                  //运动结束
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    FUNC_CALL,                          //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
    FUNC_CALL_SUB_RECORD,               //电话-最近通话
    FUNC_CALL_SUB_DIAL,                 //电话-拨打电话
    FUNC_CALL_SORT,                     //通话-一级界面
    FUNC_FINDPHONE,                     //寻找手机
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    FUNC_CALENDAER,                     //日历
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
//    FUNC_SET_SUB_SAV,                        //音量
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    FUNC_ACTIVITY,                      //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    FUNC_FLASHLIGHT,                    //手电筒
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    FUNC_SETTING,                       //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
    FUNC_SET_SUB_DOUSING,               //设置--熄屏
#if GUI_MODULE_WRIST_SUPPORT
    FUNC_SET_SUB_WRIST,                 //设置--抬腕
#endif
    FUNC_SET_SUB_DISTURD,               //设置--勿扰
    FUNC_DISTURD_SUB_SET,               //勿扰--时间设置
    FUNC_SET_SUB_SAV,                   //设置--声音与振动
    FUNC_SET_PUFFS,                   //设置口数
    FUNC_TARGET_PUFFS,                   //设置目标口数
    FUNC_SLIDING_UNLOCK_SCREEN,                   //锁屏界面
#if UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
    FUNC_SET_SUB_LANGUAGE,              //设置--语言
    FUNC_LANGUAGE = FUNC_SET_SUB_LANGUAGE,
#endif // UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
    FUNC_POWER_ON_LANGUAGE,
    FUNC_POWER_ON_SCAN,
//    FUNC_SET_SUB_TIME,                  //设置--时间
//    FUNC_TIME_SUB_CUSTOM,               //调整日期
    FUNC_SET_SUB_PASSWORD,              //设置--密码锁
    FUNC_PASSWORD_SUB_DISP,             //新密码锁设置
    FUNC_PASSWORD_SUB_SELECT,           //确认密码锁
    FUNC_SET_SUB_ABOUT,                 //设置--关于
    FUNC_SET_SUB_SOS,                   //SOS
#if UTE_MODULE_SCREENS_RESTART_SUPPORT
    FUNC_SET_SUB_RESTART,               //设置--重启
    FUNC_RESTART = FUNC_SET_SUB_RESTART,//重启
#endif // UTE_MODULE_SCREENS_RESTART_SUPPORT
#if UTE_MODULE_SCREENS_RESFY_SUPPORT
    FUNC_SET_SUB_RSTFY,                 //设置--恢复出厂
    FUNC_RSTFY = FUNC_SET_SUB_RSTFY,
#endif // UTE_MODULE_SCREENS_RESFY_SUPPORT
    FUNC_SET_SUB_OFF,                   //设置--关机
    FUNC_OFF = FUNC_SET_SUB_OFF,        //关机
    FUNC_EMPTY,                         //空白界面
#if FUNC_MUSIC_EN
    FUNC_MUSIC,
#endif
    FUNC_BTHID,
#if FUNC_USBDEV_EN
    FUNC_USBDEV,
#endif
    FUNC_AUX,
    FUNC_SPDIF,
    FUNC_SPEAKER,
    FUNC_PWROFF,
    FUNC_DEEPSLEEP,
    FUNC_SLEEPMODE,
    FUNC_I2S,
#if FUNC_RECORDER_EN
    FUNC_RECORDER,
#endif
    FUNC_BT_DUT,
    FUNC_BT_UPDATE,
    FUNC_IDLE,
    FUNC_CHARGE,
    FUNC_DEBUG_INFO,
    FUNC_SMARTSTACK,
    FUNC_MODEM_CALL,
    FUNC_MODEM_RING,
    FUNC_MESSAGE_REPLY,                  //消息发送
    FUNC_MIC_TEST,
    FUNC_OTA_UI_MODE,

    FUNC_BIRD,
    FUNC_TETRIS,
    FUNC_TETRIS_START,
    FUNC_OTA_MODE,         //OTA升级
    FUNC_OTA_ERROR,        //OTA升级失败界面
    FUNC_OTA_SUCC,          //OTA升级成功界面
    FUNC_POWER_ON,         //开机

    FUNC_BLE_CALL,
    FUNC_TEST_MODE,      ///*出厂测试模式选择*/
    FUNC_FACTORY_TESTING,///*工厂测试*/
    FUNC_AGEING,         ///*老化测试*/
    FUNC_AUDIO,         ///*音频测试*/
    FUNC_ONLINE_FACTORY_TEST, /*在线工厂测试*/
#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
    FUNC_UP_WATCH_DIAL,
#endif // UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT

    //电子烟功能
    FUNC_ECIG_SET_POWER,                //功率设置页面
    FUNC_ECIG_REMINDER,                 //抽烟状态提醒
    FUNC_ECIG_REMINDERCLOCK,                 //童锁不能抽烟提醒
#if UTE_MODULE_SCREENS_CALENDAER_SUB_SUPPORT
    FUNC_CALENDAER_SUB,
#endif
    FUNC_ECIG_VPAE_SUB,

    FUNC_MAX_NUM,           //用于计数
};

//task control block
typedef struct
{
    void *f_cb;                                     //当前任务控制指针
    compo_form_t *frm_main;                         //当前窗体
    void *msg_cb;                                   //对话框控制指针
    u32 enter_tick;                                 //记录进入func的tick
    u8 sta;                                         //cur working task number
    u8 last;                                        //lask task number
    u8 menu_style;                                  //菜单样式
    u8 menu_idx;                                    //菜单编号
    int32_t menu_idx_angle;                         //菜单编号角度
    u8 sta_break;                                   //被中断的任务
    u8 sort_cnt;                                    //快捷任务个数
    u8 tbl_sort[MAX_FUNC_SORT_CNT];                 //快捷任务表
    u16 left_sta;                                   //CLOCK左边菜单
    u8 msgbox_enter_sta;                            //弹窗进入之前的界面任务
    u8  flag_sort       : 1,                        //已进入快捷任务
    flag_animation  : 1;                        //入场动画



    void (*mp3_res_play)(u32 addr, u32 len);        //各任务的语音播报函数接口
    void (*set_vol_callback)(u8 dir);               //设置音量的回调函数，用于各任务的音量事件处理。
} func_cb_t;


extern func_cb_t func_cb;
extern const u8 func_sort_table[];     //任务切换排序table

ALWAYS_INLINE void func_mp3_res_play(u32 addr, u32 len)
{
    if (func_cb.mp3_res_play)
    {
        func_cb.mp3_res_play(addr, len);
    }
}

ALWAYS_INLINE void func_set_vol_callback(u8 dir)
{
    if (func_cb.set_vol_callback)
    {
        func_cb.set_vol_callback(dir);
    }
}

void func_run(void);
compo_form_t *func_create_form(u8 sta);     //根据任务名创建窗体

void func_switch_prev(bool flag_auto);
void func_switch_next(bool flag_auto,bool flag_loop);
void func_switch_to_clock(void);
void func_switch_to_menu(void);


u8 get_funcs_total(void);
void func_process(void);
void func_message(size_msg_t msg);
void evt_message(size_msg_t msg);

void func_switch_to(u8 sta, u16 switch_mode);
void func_switching_to_menu(void);
void func_backing_to(void);                     //页面滑动回退功能
void func_back_to(void);                        //页面按键回退功能
u8 func_directly_back_to(void);                 //页面直接回退,无动画效果

#endif // _FUNC_H
