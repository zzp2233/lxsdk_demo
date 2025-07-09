/**
 ****************************************************************************************
 *
 * @file ute_step_algorithms_common.h
 *
 * @brief Application utility functions header file.
 *
 * Copyright (C) 2014-11-03
 *
 ****************************************************************************************
 */

#ifndef _UTE_STEP_ALGORITHMS_COMMON_H_
#define _UTE_STEP_ALGORITHMS_COMMON_H_
// #include <stdint.h>
// #include <stdlib.h>
// #include <stddef.h>     // standard definitions
// #include <stdbool.h>    // boolean definition
#include "include.h"
#include "ute_all_sports_int_algorithms.h"
// #include <string.h>

/*! 兼容旧的时间数据结构，仅算法库使用zn.zeng, 2021-07-27  */
struct prf_date_time
{
    /// year time element
    uint16_t year;
    /// month time element
    uint8_t month;
    /// day time element
    uint8_t day;
    /// hour time element
    uint8_t hour;
    /// minute time element
    uint8_t min;
    /// second time element
    uint8_t sec;
};

/****
*define gsensor id for step_algorithms,default 250e
*for gsensor init
*/
typedef enum
{
    GSENSOR_BMA250E_ID = 0X01, //
    GSENSOR_BMA421_ID  = 0X02,  //
    GSENSOR_STK8321_ID = 0X03,//
    GSENSOR_SC7A20_ID  = 0X04, //
    GSENSOR_KX022_ID   = 0X05,
    GSENSOR_BMI160_ID   = 0X06,
    GSENSOR_BMI270_ID   = 0X07,
} gsensor_id_type;

/****
*define gsensor Sensitivity,default 4g
*for gsensor init
*/
typedef enum
{
    GSENSOR_8G_MODE = 0X01,
    GSENSOR_4G_MODE = 0X02,
} gsensor_mode_type;

#define WALKING 0 //caicai add 20161021
#define RUNNING 1 //caicai add 20161021
typedef struct
{


    uint16_t currentHourStepCnt;              // 当前小时总步数 =走路+跑步
    uint16_t current_hour_walk_secondCnt;     // 当前小时走路时长
    uint16_t current_hour_walk_stepCnt;       // 当前小时走路步数
    uint16_t current_hour_run_step_secondCnt; // 当前小时跑步时长
    uint16_t current_hour_run_stepCnt;        // 当前小时跑步步数
    uint8_t current_hour_start_walking_min;   // 当前小时走路开始时间
    uint8_t currentMinuteSleepTurnCnt;        // 当前分钟的翻身次数
    uint16_t currentMinuteStepCnt;            // 当前分钟的计步数据
    uint8_t current_hour_end_walking_min;     // 当前小时走路结束的时间
    uint8_t current_hour_start_running_min;   // 当前小时跑步开始的时间
    uint8_t current_hour_end_running_min;     // 当前小时跑步结束的时间
    uint8_t currentHourSleepTurnCnt;          // 当前小时翻身次数


} ute_step_sleep_param_t;
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT

extern ute_step_sleep_param_t mStepSleepParam;


/****
*睡眠计步模块，操作步骤
*0、初始化睡眠策略的手环侧放静止时gsensor三轴initOptSleepQuiteAxisParam，开机初始一次即可；
*1、声明全局变量ute_step_sleep_param_t *ssParam，初始化算法initUteStepSleepAlgorithms；
*2、定时输入三轴数据计算步数uteStepSleepAlgorithmsInputData，进行步数分析计算；
*3、每秒更新uteUpdateStepSleepData从全局变量*ssParam获取一次运动数据，
*   1）当开始屏幕跑步模式的时候需要调用enableDrvScreenStartRunning，获取的跑步运动有数值；
*   2）如果不开启或者disableDrvScreenStartRunning，获取的跑步运动数据为0；
*4、整点小时在flash保存当前小时数据之后，需要清零计步算法里面的指定数据clearStepSleepParam
****/
void initOptSleepQuiteAxisParam(int16_t xMin,int16_t xMax,int16_t yMax,int16_t zMax);
void initUteStepSleepAlgorithms(ute_step_sleep_param_t ssParam);
#if UTE_MODULE_NEW_T02_SPORT_STEP_ALGORITHMS_SUPPORT
void uteStepSleepAlgorithmsInputData(uint8_t id, uint8_t rangeMode, int16_t *x,int16_t *y,int16_t *z,uint8_t frame_cnt,uint8_t frame_index,uint16_t *accValue,bool isNightOrNoonSleep);
#else
void uteStepSleepAlgorithmsInputData(uint8_t id, uint8_t rangeMode, int16_t *x,int16_t *y,int16_t *z,uint8_t frame_cnt,uint8_t frame_index,uint16_t *accValue,bool isNightOrNoonSleep,uint8_t Sampling_freq);
#endif
void uteUpdateStepSleepData(struct prf_date_time *systemTime);
void resetUteStepsParameter(void);



/****
*久坐提醒模块，操作步骤如下
*1、app打开久坐提醒功能，需要调用一次enableSedentaryRemind，使能久坐提醒的算法（一次有效）；
*2、每秒计时并在设置的时间点内获取getSedentaryRemindActivityStatus，久坐提醒的计步状态，
*   1）result =1,表示设置时间内有计步活动，则不用提醒久坐；
*   2）result =2,表示设置时间内没有计步活动，这需要久坐提醒；
*   3) result =0,表示空闲,久坐提醒没有使能；
*3、久坐提醒一次以后，需要重新使能久坐提醒，enableSedentaryRemind，之后才能正常使用；
*4、关闭久坐提醒，disableSedentaryRemind
****/
#define SEDENTARY_ADD_UP_STEP_TOTAL 20
#define SedentaryRemind_NULL 0               //保持当前状态
#define SedentaryRemind_NO_REMIND 1         // 不需要提醒
#define SedentaryRemind_NEED_REMIND 2       //需要提醒
void enableSedentaryRemind(void);
void disableSedentaryRemind(void);
uint8_t getSedentaryRemindActivityStatus(void);


/*****
*抬手亮屏功能，操作步骤如下
*1、开机初始化抬手亮屏参数，initHandRollverScreenParam只要初始化一次即可，也可以多次调用；
*2、使能抬手亮屏模块enableHandRollVerScreenMode，以下几种情况必须重新使能：1）屏幕off的时候；2）（手点击/滑动屏幕）手动操作屏幕的时候，
*3、实时传入gsensor三轴数据，进行抬手亮灭屏检测uteHandScreenAlgorithmsInputData，但是以下情况不能传入gsensor数据
*   1）马达震动的时候或者系统未全部启动的时候（必选）
*   2）不在一级界面的时候（可选）
*4、使能以后，在传入gsensor数据后，马上获取抬手亮屏的显示参数getHandRollVerScreenDisplayParam；返回值有几种情况
*   1）result = 0，空闲不需要操作屏幕；
*   2）result = 1，此时是抬手亮屏状态，如果现实是灭屏则需要点亮屏幕；
*   3）result = 2，此时是翻腕灭屏状态，如果当前是亮屏则需要关灭屏幕；
****/
#define ROLLOVER_HAND_SCREEN_IDLE_STATUS 0  //保存当前状态
#define ROLLOVER_HAND_SCREEN_ON_STATUS 1    // 需要亮屏
#define ROLLOVER_HAND_SCREEN_OFF_STATUS 2   // 需要灭屏
void initHandRollverScreenParam(int16_t xMin,int16_t xMax,int16_t yMin,int16_t yMax,int16_t zMin,int16_t zMax,int16_t accDiff);
void enableHandRollverScreenMode(void);
void uteHandScreenAlgorithmsInputData(int16_t *x,int16_t *y,int16_t *z,uint16_t accValue);
uint8_t getHandRollVerScreenDisplayParam(void);
void uteHandScreenParamResetForMotorVibration(void);


/**
*摇一摇拍照功能，操作步骤如下
*1、（第一次）启动拍照功能或者获取拍照结果以后，必须enableTakingPicturesState；
*2、退出拍照功能，必须disableTakingPicturesState；
***/
#define APP_TAKE_PICTRUES_STEP_CYC_DEFUALT  1  // 2~3 STEP
void enableTakingPicturesState(void);
void disableTakingPicturesState(void);
bool getTakingPicturesResult(void);
//void disableTakingPicturesResult(void);


/***
*function： getStepOrSleepStateWhetherSwitch
* return value 1 or 2;
* #define STEPING_STATE 1
* #define SLEEPING_STATE 2
***/
uint8_t getStepOrSleepStateWhetherSwitch(void);
void setCurrentStepSleepState(bool isStepingT, bool isSleepingT);


/**
*辅助心率检测（此功能非必要，开发者可以根据不同的heart sensor选用或不用）,操作步骤如下
*1、每秒检测脱手检测，当手环侧放桌面时，如果心率是测试心率的状态，则返回脱手检测，
*   1）result =true，心率脱手，需要停止心率；
*   2）result =false，心率不脱手；
**/
bool getIsNotWearQuietStateResult(void);
#if UTE_MODULE_SLEEP
uint16_t SqrtByBisection(uint32_t n);
#endif
#endif
#endif
