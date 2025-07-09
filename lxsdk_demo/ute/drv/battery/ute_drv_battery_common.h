/**
*@file
*@brief     电池驱动头文件
*@details
*@author        zn.zeng
*@date        Jul 05, 2021
*@version       v1.0
*/

#ifndef _UTE_DRV_BATTERY_COMMON_H_
#define _UTE_DRV_BATTERY_COMMON_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "include.h"

/*!记录电池电量 xjc, 2022-01-25  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_RECORD_BAT_PERCENT  "systemparam/recordBattery"

typedef enum
{
    BAT_STATUS_NO_CHARGE,
    BAT_STATUS_CHARGING,
    BAT_STATUS_CHARGED,
} ute_drv_battery_charger_status_t;
typedef struct
{
    bool isChecking;
    float ambitent;
} ute_drv_battery_ce_param_t;
typedef struct
{
    ute_drv_battery_charger_status_t chargerStatus;
    uint16_t voltage;/* 电池电压zn.zeng 2021-12-15*/
    int16_t current;/* 电池电流zn.zeng 2021-12-15*/
    uint8_t lvl;
    uint8_t lastLvl;
    uint8_t lvlBuff[UTE_DRV_BATTERY_AVG_LEVEL_BUFF_MAX];
    uint8_t lvlCnt;
    bool isPlugIn;
    bool isDelayDisplayCharger;
#if UTE_DRV_BATTERY_CE_AUTH_SUPPORT
    ute_drv_battery_ce_param_t ceParam;
#endif
    bool isHasLowPowerNotify;
    int chargerTimeSecond;
    uint8_t startChargerLvl;
    uint32_t chargerMAS;
    uint8_t recordLvl;
#if UTE_MODULE_BATTERY_CHARGED_DISPLAY_ON_SUPPORT
    bool alreadyDisplayOn;
#endif
#if UTE_MODULE_BATTERY_LINEAR_CONSUME_SUPPORT
    uint8_t bat100PercentTimeout; //电量100%后，几分钟之后才允许掉电
#endif
#if UTE_DRV_BATTERY_LOW_POWER_FIRST_PERECNT//第一次低电提醒的电量
    bool isHasLowPowerFirstNotify;
#endif
    uint32_t chargedTimeSecond;
#if UTE_DRV_CHARGE_CURRENT_SECTION_WITH_TEMPERATURE
    uint16_t chargeCurrentValue;//充电电流，用于记录分段充电记录设置的充电电流
#endif
    uint8_t fullFactoryAgingLvl;
} ute_drv_battery_common_data_t;
void uteDrvBatteryCommonInit(void);
void uteDrvBatteryCommonReadConfig(void);
void uteDrvBatteryCommonCompareBatLvl(void);
uint16_t uteDrvBatteryCommonGetVoltage(void);
uint8_t uteDrvBatteryCommonGetLvl(void);
uint16_t uteDrvBatteryCommonGetMaxVoltage(void);
ute_drv_battery_charger_status_t uteDrvBatteryCommonGetChargerStatus(void);
void uteDrvBatteryCommonUpdateBatteryInfo(void);
void uteDrvBatteryCommonEverySecond(void);
void uteDrvBatteryCommonSetChargerPlug(bool isPlugIn);
bool uteDrvBatteryCommonGetChargerPlug(void);
bool uteDrvBatteryCommonIsDelayDisplayCharger(void);
void uteDrvBatteryCommonSetChargerTimeSecond(int chargerTimeSecond);
uint8_t uteDrvBatteryCommonGetBatteryIndex(uint8_t totalIndex);
#if UTE_DRV_BATTERY_CE_AUTH_SUPPORT
float uteDrvBatteryCommonGetCEAuthAmbientValue(void);
#endif
#if UTE_MODULE_BATTERY_CHARGED_DISPLAY_ON_SUPPORT
bool uteDrvBatteryCommonGetAlreadyDisplayOnParam(void);
#endif
#if UTE_MODULE_BATTERY_SAVE_LAST_LVL_BEFORE_FACTORY_SUPPORT
void uteDrvBatteryCommonSaveLastLvlToSN1(void);
void uteDrvBatteryCommonReadLastLvlFromSN1(void);
#endif
void uteDrvBatteryCommonChargerChangeCb(uint8_t state);

#endif //_UTE_DRV_BATTERY_COMMON_H_

