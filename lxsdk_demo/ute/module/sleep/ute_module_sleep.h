/**
*@file
*@brief        睡眠
*@details
*@author       zn.zeng
*@date       2021-10-25
*@version      v1.0
*/

#ifndef _UTE_MODULE_SLEEP_H_
#define _UTE_MODULE_SLEEP_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_sleep_algorithms_on_band.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_systemtime.h"
#include "include.h"
#if UTE_MODULE_SLEEP
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
#include "ute_module_CyweeMotion_inactivity.h"
#endif
/*! 睡眠数据保存目录 zn.zeng, 2021-10-26  */
#define UTE_MODULE_FILESYSTEM_SLEEP_DIR "sleep"
/*!保存睡眠采样原始数据 zn.zeng, 2021-12-29  */
#define UTE_MODULE_FILESYSTEM_LOG_SLEEP_SAMPLE_DATA  "log/sleepSampleData"

#define UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR  "log/sleepDebug"

#define UTE_MODULE_FILESYSTEM_SLEEP_TMP_DATA  "log/sleepTmpData"
typedef struct
{
    uint16_t lightSleepMin;
    uint16_t deepSleepMin;
    uint16_t wakeSleepMin;
    uint16_t totalSleepMin;
    uint16_t SleepHour;
    uint16_t SleepMin;
#if UTE_REM_SLEEP_SUPPORT
    uint16_t RemSleepMin;
#endif
    timestmap_t fallAsSleepTime;
    timestmap_t getUpSleepTime;
    sleep_record_t sleep_record[UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE];
    uint8_t recordCnt;
} ute_module_sleep_display_data_t;
/*! zn.zeng, 2021-10-26  */
typedef struct
{
    uint8_t heartCnt;
    uint8_t oneMinHeartValue[60];
    bool isCurrHasSleepData;
} ute_module_sleep_data_t;
void uteModuleSleepInit(void);
void uteModuleSleepDataInputSecond(ute_step_sleep_param_t *sleepData,ute_module_systemtime_time_t time,bool isStepping);
void uteModuleSleepStartSendSleepHistoryData(ute_module_systemtime_time_t time);
void uteModuleSleepGetCurrDayDataDisplay(ute_module_sleep_display_data_t *sleepDisplayData);
#if UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT
void uteModuleSleepStartSendSleepSampleData(void);
void uteModuleSleepStartSendSleepLogData(void);
#endif
void uteModuleSleepSystemtimeChange(ute_module_systemtime_time_t curr,ute_module_systemtime_time_t newSet);
bool uteModuleSleepCurrDayIsGetUp(void);
bool uteModuleSleepCurrDayIsHasSleep(void);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
typedef struct
{
    timestmap_t startTime;
    timestmap_t endTime;
    uint16_t min;
} ute_module_sleep_nap_data_t;
typedef struct
{
    ute_module_sleep_nap_data_t data[UTE_SLEEP_NAP_DATA_DISPLAY_MAX_RECORD];  //最大24段小睡
    uint8_t recordCnt;
} ute_module_sleep_nap_display_data_t;
void uteModuleSleepAnalysis(uteCwmSleepOnceData_t uteCwmSleepOnceData,bool isSleepMode);
bool uteModuleSleepReadDayNapData(ute_module_sleep_nap_display_data_t *pread);
void uteModuleSleepStartSendCwmNapSleepHistoryData(ute_module_systemtime_time_t time);
#endif
#endif
#endif //_UTE_MODULE_SLEEP_H_

