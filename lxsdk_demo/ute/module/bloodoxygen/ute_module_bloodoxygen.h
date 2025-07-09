/**
*@file
*@brief     心氧模块
*@details   心氧模块头文件
*@author        zn.zeng
*@date        2021-07-21
*@version       v1.0
*/
#ifndef _UTE_MODULE_BLOODOXYGEN_H_
#define _UTE_MODULE_BLOODOXYGEN_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"

#if UTE_MODULE_BLOODOXYGEN_SUPPORT
/*!写入心率测试参数 zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SET_INFO "systemparam/bloodoxygenSetInfo"
/*! 心率24小时自动测试数据保存目录 zn.zeng, 2021-08-30  */
#define UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR "bloodoxygenAutoData"

/*! 血氧数据结构zn.zeng, 2021-07-21  */
typedef struct
{
    bool isSingleTesting;
    uint8_t value;
    uint8_t testingSecond;
    bool isAutoTesting;
    bool isAutoTimeBucketTesting;
    uint16_t intervalMin;
    uint16_t startTimeHourMin;
    uint16_t endTimeHourMin;
    bool isBloodOxygenAutoTestFlag;
    uint8_t dayMinValue;
    uint8_t dayMaxValue;
    uint8_t lastValue;
} ute_module_bloodoxygen_data_t;
void uteModuleBloodoxygenInit(void);
void uteModuleBloodoxygenReadConfig(void);
void uteModuleBloodoxygenEverySecond(void);
void uteModuleBloodoxygenStartSingleTesting(void);
void uteModuleBloodoxygenStopSingleTesting(void);
bool uteModuleBloodoxygenIsWear(void);
uint8_t uteModuleBloodoxygenGetValue(void);
void uteModuleBloodoxygenSetValue(uint8_t value);
bool uteModuleBloodoxygenIsTesting(void);
void uteModuleBloodoxygenSaveAutoIntervalParam(bool isAutoTesting,uint16_t intervalMin);
void uteModuleBloodoxygenSaveAutoTimeBucketParam(bool isAutoTimeBucketTesting,uint16_t startTimeHourMin,uint16_t endTimeHourMin);
void uteModuleBloodoxygenStartSendAutoTestHistoryData(ute_module_systemtime_time_t time);
void uteModuleBloodoxygenAutoSaveOxygenData(void);
void uteModuleBloodoxygenDelHistoryData(void);
bool uteModuleBloodoxygenIsBloodOxygenAutoTesting(void);
uint8_t uteModuleBloodoxygenGetTestingSecond(void);
void uteModuleBloodoxygenSetFirstEnterAppFlag(bool flag);
void uteModuleBloodoxygenGetMinMaxValue(uint8_t *min,uint8_t *max);
void uteModuleBloodoxygenSetBloodOxygenAutoTesting(bool isAutoTest);
#endif //UTE_MODULE_BLOODOXYGEN_SUPPORT
#endif //_UTE_MODULE_BLOODOXYGEN_H_

