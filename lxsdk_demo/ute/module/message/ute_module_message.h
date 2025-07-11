/**
*@file
*@brief     模块系统消息头文件
*@details   系统消息写在此文件
*@author        zn.zeng
*@date        Jun 29, 2021
*@version       v1.0
*/

#ifndef _UTE_MODULE_MESSAGE_H_
#define _UTE_MODULE_MESSAGE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_task_application.h"

/*!消息枚举，没有的功能也保持，枚举内容不能使用宏控制 zn.zeng, 2021-07-13  */
enum
{
    MSG_TYPE_SYSTEM_TIME_SEC_BASE,
    MSG_TYPE_SYSTEM_START_POWER_OFF,
    MSG_TYPE_SYSTEM_REAL_POWER_OFF,
    MSG_TYPE_SYNC_DATA_TIMER,
    TO_APP_TASK_MSG_UPDATE_ADV_DATA,
    TO_APP_TASK_MSG_UPDATE_DEV_NAME,
    TO_APP_TASK_MSG_STOP_ADV,
    MSG_TYPE_DRV_MOTOR_START,
    MSG_TYPE_DRV_MOTOR_STOP,
    MSG_TYPE_DRV_MOTOR_HANDLE_TIMER,
    MSG_TYPE_HNAD_SCREEN_ON_NOTIFY,
    MSG_TYPE_HNAD_SCREEN_OFF_NOTIFY,
    MSG_TYPE_MODULE_NOTIFY_NOTIFYCATTION,
    MSG_TYPE_MODULE_GUI_CLEAR_DEPTH,
    MSG_TYPE_TAKE_PICTURE_NOTIFY,
    MSG_TYPE_SEDENTARY_NOTIFY,
    MSG_TYPE_MODULE_SPORT_START_MORE_SPORTS,
    MSG_TYPE_MODULE_SPORT_STOP_MORE_SPORTS,
    MSG_TYPE_DRV_SPORT_COUNTDOWN_TIMER,
    MSG_TYPE_DRV_SPORT_ALGO_INPUT_DATA_TIMER, //记步算法定时器
    MSG_TYPE_TAKE_PICTURE_OPEN_APP_SCREEN,
    MSG_TYPE_TAKE_PICTURE_CLOSE_APP_SCREEN,//退出app相机界面
    MSG_TYPE_HEART_HANDLER,
    MSG_TYPE_HEART_START_SINGLE_TESTING,
    MSG_TYPE_HEART_STOP_SINGLE_TESTING,
    MSG_TYPE_BLOODOXYGEN_START_SINGLE_TESTING,
    MSG_TYPE_BLOODOXYGEN_STOP_SINGLE_TESTING,
    MSG_TYPE_HEART_ALGO_HANDLER,
    MSG_TYPE_RESET_ROVLLVER_SCREEN_MODE,
    MSG_TYPE_MODULE_WATCHONLINE_RECVTIMEOUT_TIMER,
    MSG_TYPE_MODULE_WATCHONLINE_ONESEC_TIMER,
    MSG_TYPE_MODULE_WATCHONLINE_START_TIMER,
    MSG_TYPE_DRV_KEY_HANDLER,
    MSG_TYPE_MODULE_NOTIFY_ANCS_START_PAIR,
    MSG_TYPE_CALL_DISABLE_NOTIFY,
    TO_APP_TASK_CONNECT_A2DP,
    TO_APP_TASK_DISCONNECT_A2DP,
    TO_APP_TASK_PLAY_RING,
    TO_APP_TASK_STOP_RING,
    MSG_TYPE_EMOTION_PRESSURE_START_SINGLE_TESTING,
    MSG_TYPE_EMOTION_PRESSURE_STOP_SINGLE_TESTING,
    MSG_TYPE_SMOKE_REMIND,
    MSG_TYPE_FACTORY_WRITE_RECORD_DATA_TO_FLASH, // 工厂录音写入数据到flash
    MSG_TYPE_FACTORY_PLAY_RECORD, // 工厂录音播放
    MSG_TYPE_SCREEN_ESD_TE_INT_ERROR_RESET, //屏幕ESD测试时TE中断错误处理
};

void uteModuleMessageUteApplicationTaskHandler(ute_task_application_message_t *msg);

#endif //_UTE_MODULE_MESSAGE_H_

