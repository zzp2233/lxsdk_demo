/**
*@file
*@brief        体温模块
*@details
*@author       zn.zeng
*@date       2021-12-01
*@version      v1.0
*/
#ifndef _UTE_MODULE_TEMPERATURE_H_
#define _UTE_MODULE_TEMPERATURE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"

#if UTE_MODULE_TEMPERATURE_SUPPORT
/*! 体温数据保存目录 zn.zeng, 2021-08-25  */
#define UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR "temperature"
/*!写入体温测试参数 zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_TEMPERATURE_SET_INFO "systemparam/temperatureInfo"

/*! 体温测试模式 zn.zeng, 2021-12-02  */
typedef enum
{
    TEMPERATURE_MODE_NONE = 0X00,
    TEMPERATURE_MODE_APP_SINGLE     = 0X01,
    TEMPERATURE_MODE_BAND_SINGLE    = 0X02,
    TEMPERATURE_MODE_BAND_AUTO      = 0X03,
    TEMPERATURE_MODE_AMBIENT_CHECK  = 0X04,
    TEMPERATURE_MODE_APP_GET_AMBIENT = 0X05,
} ute_module_temperature_mode_t;
/*! 体温数据结构zn.zeng, 2021-07-16  */
typedef struct
{
    ute_module_temperature_mode_t testMode;
    uint8_t testingSecond;
    float bodyTemperature;
    float skinTemperature;
    float ambitentTemperature;
    bool isAutoTestOpen;
    bool isAutoTestLimitTimeOpen;
    uint32_t autoTestIntervalSecond;//自动测试间隔
    uint32_t autoTestLimitStartTimeSecond;//一天的秒数
    uint32_t autoTestLimitStopTimeSecond;//一天的秒数
    bool isTemperatureWarningOpen;
    bool isHasTemperatureWarning;//是否已经提醒过
    uint16_t warningLowValue;//低温警告值，单位为摄氏度的100倍
    uint16_t warningHeightValue;//高温警告值，单位为摄氏度的100倍
    uint8_t warningSecond;
    bool isNeedSaveAutoTestData;
} ute_module_temperature_data_t;

void uteModuleTemperatureInit(void);
void uteModuleTemperatureEverySecond(void);
bool uteModuleTemperatureIsWear(void);
bool uteModuleTemperatureIsTesting(void);
void uteModuleTemperatureStartTesting(ute_module_temperature_mode_t mode);
void uteModuleTemperatureStopTesting(void);
float uteModuleTemperatureGetBodyTemperature(void);
float uteModuleTemperatureGetAmbitentTemperature(void);
float uteModuleTemperatureGetSkinTemperature(void);
void uteModuleTemperatureSetAutoTestInterval(bool isOpen,uint32_t intervalSecond);
void uteModuleTemperatureSetAutoTestLimitTime(bool isOpen,uint32_t startSecond,uint32_t stopSecond);
void uteModuleTemperatureSetWarningParam(bool isOpen,uint16_t low,uint16_t height);
void uteModuleTemperatureSaveBodyTemperatureData(void);
void uteModuleTemperatureStartSendHistoryData(ute_module_systemtime_time_t time);
void uteModuleTemperatureDelHistoryData(void);
ute_module_temperature_mode_t uteModuleTemperatureGetTestMode(void);
#else
void uteModuleTemperatureIoInit(void);
#endif
#endif //_UTE_MODULE_TEMPERATURE_H_

