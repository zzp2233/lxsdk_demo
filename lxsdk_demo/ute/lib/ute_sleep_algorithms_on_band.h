/**
 ****************************************************************************************
 *
 * @file _UTE_SLEEP_ALGORITHMS_ON_BAND.h
 *
 * @brief Application utility functions header file.
 *
 * Copyright (C) 2014-11-03
 *
 ****************************************************************************************
 */
#ifndef _UTE_SLEEP_ALGORITHMS_ON_BAND_H_
#define _UTE_SLEEP_ALGORITHMS_ON_BAND_H_
//#include "ute_hal_layer.h"
#ifdef __cplusplus
extern "C" {
#endif

// #include <stdlib.h>
// #include <math.h>
// #include <stdint.h>
// #include <stdbool.h>
#include "include.h"

#define UTE_SLEEP_ON_BAND_SUPPORT 1

#if UTE_SLEEP_ON_BAND_SUPPORT

#define UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE 90

//小睡睡眠显示最大记录数
#define UTE_SLEEP_NAP_DATA_DISPLAY_MAX_RECORD  24

#define INIDLE  0
#define DEEP_SLEEP  1
#define LIGHT_SLEEP 2
#define AWAKE_SLEEP 3
#define REM_SLEEP   4 //add 20220720 for REM
#define STOP_SLEEP  0xFF  //表示结束睡眠

/*****记录每条睡眠状态*****/
typedef struct
{
    uint8_t hour;       //0-23取值
    uint8_t min;        //0-59取值
} timestmap_t;

typedef struct
{
    uint8_t state;
    uint8_t sleepFlag;
    uint16_t period;
    timestmap_t startTime;
} sleep_record_t;
/*****记录每条睡眠状态*****/
typedef struct
{
    uint16_t year;
    uint8_t mon;
    uint8_t day;
    uint8_t recordIndex;
    timestmap_t getUpTime;
    sleep_record_t sleep_record[UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE];
} ute_sleep_data_save_t;

/*****外部传入参数*****/
typedef struct
{
    uint8_t mSleepState;
    uint8_t stepMinCnt;// 计步连续分钟计数
    uint8_t notWearMinCnt;// 脱检连续分钟计数
    uint8_t interruptContinuousCnt; // 连续中断(计步+脱检)分钟计数
    uint16_t continuousSleepMinCnt; // 连续睡眠分钟计数
    uint16_t MinCnt;
    uint16_t maxRecord;
    uint8_t continuousQuietCnt; // 连续静放计数 20191129
    uint16_t sumSleepingTurnCnt; // 翻身次数累计,用于入睡辅助判断20191217 modify the name
    uint16_t sumMinStep; // 每分钟步数的累加 20191127
    /***入睡和起床优化20191217***/
    uint8_t OptStatus;
    uint8_t continuousZeroTurnCnt; // 连续翻身=0计数，用于优化入睡和起床时间点
    uint16_t AidSlpOptTurnCnt; // 睡眠和非睡眠的翻身次数求和,用于入睡-起床辅助判断(出现连续三次翻身=0或isGetUp=true时,才做判断。注意区别于sumSleepingTurnCnt) 20191213
    uint16_t AidSlpOptTurnCntNum; // 睡眠和非睡眠的翻身次数求和的个数 20191213
    uint16_t optGetUpTime ; // 用于优化起床时间的暂存值minutes
    bool isUpdateGetUpTimeFlag; // 用于判断是否赋过值
    /***入睡和起床优化20191217***/
    /***20200615 start 静止玩手机的情况下的二次睡眠优化***/
    uint8_t GlobalContinuousWeakTurnNum; //睡眠或非睡眠状态下的连续微小翻身波动次数累计
    uint8_t GlobalContinuousZeroTurnNum; //睡眠或非睡眠状态下的连续0翻身波动次数累计
    uint8_t GlobalZeroTurnNum; //睡眠或非睡眠状态下的出现0翻身次数累计
    bool isStartWeakTurnCount; //是否开始微小翻身波动次数累计
    bool isMeetWeakTurnMaxThreshold; //是否满足微小波动的最大阈值条件
    /***20200615 end***/
    uint8_t slpSameHRCnt; //20191225 用于未佩戴静放判断
    uint8_t slpLastHR; //20191225 用于未佩戴静放判断
    bool isQuiet; //是否一直静止20191127
    bool isHasSave;
    bool isGetUp;
    bool isHadSleep; // 当天是否进入过深度睡眠(per>=20min)
    ute_sleep_data_save_t saveData;
} ute_sleep_data_t;
void uteSleepAnalysisOnBand(uint8_t currentHour,uint8_t currentMin,uint8_t minTurnCnt,uint16_t minStepCnt,uint8_t heart,uint8_t sleepFlag);
void uteSleepOnBandSetPoint(ute_sleep_data_t *point);
void uteSleepOnBandClearSleepStatus(void);
char *getSleepLibVersion(void);
uint32_t getSleepTotalMinute(void);
uint8_t uteSleepOnBandGetRecordIndex(void);
#endif
#ifdef __cplusplus
}
#endif
#endif
