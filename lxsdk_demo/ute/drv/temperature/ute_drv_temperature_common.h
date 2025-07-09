/**
*@file
*@brief        体温公共层驱动文件
*@details
*@author       zn.zeng
*@date       2021-12-02
*@version      v1.0
*/


#ifndef _UTE_DRV_TEMPERATURE_COMMON_H_
#define _UTE_DRV_TEMPERATURE_COMMON_H_
#include "ute_module_platform.h"
#include "ute_project_config.h"
#if (UTE_MODULE_TEMPERATURE_SUPPORT || UTE_DRV_BATTERY_CE_AUTH_SUPPORT)
typedef void (*ute_drv_temperature_common_poweron_t)(bool isOnlyOneTime);
typedef void (*ute_drv_temperature_common_poweroff_t)(void);
typedef void (*ute_drv_temperature_common_start_sample_t)(void);
typedef float (*ute_drv_temperature_common_get_ambient_t)(void);
typedef float (*ute_drv_temperature_common_get_skin_t)(void);

/*! 配置体温驱动函数指针数据结构 zn.zeng, 2021-12-02  */
typedef struct
{
    char *name;/*! 器件型号zn.zeng, 2021-12-02  */
    ute_drv_temperature_common_start_sample_t startSample;
    ute_drv_temperature_common_get_ambient_t getAmbient;
    ute_drv_temperature_common_get_skin_t getSkin;
    ute_drv_temperature_common_poweron_t powerOn;
    ute_drv_temperature_common_poweroff_t powerOff;
} ute_drv_temperature_common_config_t;
void uteDrvTemperatureCommonInit(void);
void uteDrvTemperatureCommonEverySecond(void);
float uteDrvTemperatureCommonGetSkinValue(void);
float uteDrvTemperatureCommonGetAmbientValue(void);
void uteDrvTemperatureCommonPowerOn(bool isOnlyOneTime);
void uteDrvTemperatureCommonPowerOff(void);
bool uteDrvTemperatureCommonIsInitSuccessful(void);
#endif
#endif // _UTE_DRV_TEMPERATURE_COMMON_H_

