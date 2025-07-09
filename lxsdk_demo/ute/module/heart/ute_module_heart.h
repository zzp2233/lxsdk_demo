/**
*@file
*@brief     心率模块
*@details   心率模块相关
*@author        zn.zeng
*@date        2021-07-12
*@version       v1.0
*/
#ifndef _UTE_MODULE_HEART_H_
#define _UTE_MODULE_HEART_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"

#if UTE_MODULE_HEART_SUPPORT

/*! 心率24小时自动测试数据保存目录 zn.zeng, 2021-08-30  */
#define UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR "heartAutoData"
/*!写入心率警告范围信息参数 zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEARTWARNINGINFO  "systemparam/heartWarningInfo"
/*!写入心率测试参数 zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEART_SET_INFO "systemparam/heartSetInfo"
/*!心率最大值最小值平均值 xjc, 2022-01-17  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEART_MAX_MIN_AVG_INFO "systemparam/heartMaxMinAvgInfo"
/*! 多运动心率预警设置 dengli.lu, 2022-03-08  */
#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEART_SPORTS_WARNING_INFO "systemparam/heartSportWarning"
#endif
#define UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_DIR "weekDayStaticHeart"
#define UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_MAX_FILE 10

/*! 最小心率值zn.zeng, 2021-07-16  */
#define UTE_MODULE_HEART_MIN_VALUE 40
/*! 最大心率值zn.zeng, 2021-07-16  */
#define UTE_MODULE_HEART_MAX_VALUE 200

/*! 心率自动测量超时时间,wang.luo 2024-10-31 */
#ifndef UTE_MODULE_HEART_AUTO_TEST_TIMEOUT_SECOND
#define UTE_MODULE_HEART_AUTO_TEST_TIMEOUT_SECOND 30
#endif

/*! 心率值警告数据结构zn.zeng, 2021-07-13  */
typedef struct
{
    uint8_t setMinHeart;
    uint8_t setMaxHeart;
    uint32_t warningSecond;
    bool isOpen;
} ute_module_heart_warning_t;
/*! 运动心率值警告数据结构dengli.lu, 2022-03-08  */
#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
typedef struct
{
    uint8_t maxHeartValue;
    bool isMaxHeartOpen;
    uint8_t minHeartValue;
    bool isMinHeartOpen;
    uint32_t warningSecond;
} ute_module_heart_sport_warning_t;
#endif
/*! 类型枚举zn.zeng, 2021-07-19*/
typedef enum
{
    TYPE_HEART,
    TYPE_BLOODPRESSURE,
    TYPE_BLOODOXYGEN,
    TYPE_BREATHRATE,
    TYPE_EMOTION_PRESSURE,
    TYPE_FACTORY0,
    TYPE_FACTORY1,
    TYPE_WEAR,
} ute_module_heart_type_t;
/*! 心率数据结构zn.zeng, 2021-07-13  */
typedef struct
{
    bool isAutoTesting;
    bool isSingleTesting;
    bool isAutoTestFlag;
    uint16_t autoTestSecond;
    bool lastIsWear;
    int heartValue;
    uint8_t currentDayMinHeart;
    uint8_t currentDayMaxHeart;
    uint8_t currentDayAvgHeart;
    uint8_t notWearSecond;
    uint32_t avgHeartCnt;
    ute_module_heart_type_t type;
    uint8_t bloodoxygenValue;
    ute_module_heart_warning_t warning;
#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
    ute_module_heart_sport_warning_t sportWarning;
#endif
#if UTE_MODULE_HEART_WEEK_STATIC_SUPPORT
    uint8_t weekDayStaticHeart[7];
    uint8_t dayStaticHeart;
    uint32_t dayStaticMin;
#endif
#if APP_MODULE_HEART_RESTING_HEARTRATE_SUPPORT
    uint8_t maxHourheartValue;
    uint8_t minHourheartValue;
#endif
} ute_module_heart_data_t;

void uteModuleHeartInit(void);
void uteModuleHeartReadConfig(void);
void uteModuleHeartEverySecond(void);
void uteModuleHeartReadValue(void);
bool uteModuleHeartIsNotNeedHeartLight(void);
bool uteModuleHeartIsNeedTesting(void);
bool uteModuleHeartIsAutoTesting(void);
bool uteModuleHeartIsSingleTesting(void);
int uteModuleHeartGetHeartValue(void);
void uteModuleHeartSetHeartValue(int heartValue);
uint8_t uteModuleHeartGetMaxHeartValue(void);
uint8_t uteModuleHeartGetMinHeartValue(void);
uint8_t uteModuleHeartGetAvgHeartValue(void);
void uteModuleHeartStartSingleTesting(ute_module_heart_type_t type);
void uteModuleHeartStartSingleTestingMsgHandler(uint32_t param);
void uteModuleHeartStopSingleTesting(ute_module_heart_type_t type);
void uteModuleHeartStopSingleTestingMsgHandler(uint32_t param);
bool uteModuleHeartIsWear(void);
void uteModuleHeartAvgHeartOfCurrentDayProcess(uint8_t heart);
void uteModuleHeartClearAvgHeartOfCurrentDay(ute_module_systemtime_time_t *time);
void uteModuleHeartMinMaxWarningValueProcess(void);
void uteModuleHeartSaveHeartWaringInfo(ute_module_heart_warning_t *value);
uint8_t uteModuleHeartGetBloodOxygenValue(void);
uint8_t uteModuleHeartGetBreathrateValue(void);
void uteModuleHeartAutoSaveHeartData(void);
void uteModuleHeartSaveHeartMaxMinAvgData(void);
void uteModuleHeartStartSendHeartAutoTestHistoryData(ute_module_systemtime_time_t time);
void uteModuleHeartSetAutoTesting(bool isAutoTesting);
void uteModuleHeartPowerOff(void);
uint8_t uteModuleHeartGetWorkMode(void);
uint8_t uteModuleHeartGetHeartValueRange(uint8_t heart);
uint8_t uteModuleHeartGetHeartWaringMaxValue(void);
uint8_t uteModuleHeartGetHeartWaringMinValue(void);
bool uteModuleHeartGetTodayHistoryData(uint8_t *heartHistoryGraph, uint8_t heartHistoryGraphCount);
#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
void uteModuleHeartReadSportHeartWaringInfo(void);
void uteModuleHeartSetSportHeartWaringInfo(uint8_t maxHeartValue,bool isMaxHeartOpen,uint8_t minHeartValue,bool isMinHeartOpen);
uint8_t uteModuleHeartGetSportHeartWaringMaxValue(void);
bool uteModuleHeartGetSportHeartWaringMaxIsOpen(void);
uint8_t uteModuleHeartGetSportHeartWaringMinValue(void);
bool uteModuleHeartGetSportHeartWaringMinIsOpen(void);
void uteModuleHeartSetSportHeartWaringSecond(uint32_t warningSecond);
uint32_t uteModuleHeartGetSportHeartWaringSecond(void);
#endif
void uteModuleHeartSystemtimeChange(ute_module_systemtime_time_t curr,ute_module_systemtime_time_t newSet);
#if UTE_MODULE_HEART_WEEK_STATIC_SUPPORT
void uteModuleHeartrReadStaticHeartData(void);
void uteModuleHeartrSaveStaticHeartData(void);
void uteModuleHeartWeekStaticSecond(ute_module_systemtime_time_t time);
void uteModuleHeartLoadWeekDayStaticHeartData(UT_GraphsParam *weekDayStaticHeartGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
uint8_t uteModuleHeartrGetCurrDayStaticHeartData(void);
uint8_t uteModuleHeartrGetLast7DayAvgStaticHeartData(void);
#endif
#if APP_MODULE_HEART_RESTING_HEARTRATE_SUPPORT
void uteModuleHeartRestingHeartrateSend(void);
void uteModuleHeartSendEveryHourMaxMInHeartData(uint8_t hour);
void uteModuleHeartSendHistoryRestingHeartData(void);
#endif
#endif
#endif //_UTE_MODULE_HEART_H_

