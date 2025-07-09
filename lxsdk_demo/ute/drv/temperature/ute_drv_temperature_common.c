/**
*@file
*@brief        体温公共层驱动文件
*@details
*@author       zn.zeng
*@date       2021-12-02
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_drv_temperature_common.h"
#include "ute_project_config.h"
#include "ute_drv_temperature_infrared_thermistor.h"

#if (UTE_MODULE_TEMPERATURE_SUPPORT || UTE_DRV_BATTERY_CE_AUTH_SUPPORT)

/*! 驱动配置函数指针zn.zeng, 2021-09-27  */
const ute_drv_temperature_common_config_t *uteDrvTemperatureCommonFunction=NULL;
/**
*@brief        体温驱动开始采样
*@details
*@author        zn.zeng
*@date        2021-12-02
*/
void uteDrvTemperatureCommonInit(void)
{
#if UTE_DRV_TEMPERATURE_INFRARED_THERMISTOR_SUPPORT
    uteDrvTemperatureCommonFunction = &uteDrvTemperatureInfraredThermistorConfig;
#else
#error "No Temperature Driver Support"
#endif
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,name=%s", __func__,uteDrvTemperatureCommonFunction->name);
}

/**
*@brief        判断是否初始化成功
*@details
*@author       Wang.Luo
*@date         2024-11-19
*/
bool uteDrvTemperatureCommonIsInitSuccessful(void)
{
    bool ret = false;
    if(uteDrvTemperatureCommonFunction != NULL)
    {
        ret = true;
    }
    return ret;
}

/**
*@brief        体温驱动开始采样
*@details
*@author        zn.zeng
*@date        2021-12-02
*/
void uteDrvTemperatureCommonEverySecond(void)
{
    if(!uteDrvTemperatureCommonIsInitSuccessful())
    {
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,drv not init", __func__)
        return;
    }
    uteDrvTemperatureCommonFunction->startSample();
}
/**
*@brief        体温驱动获取环境温度
*@details
*@author        zn.zeng
*@date        2021-12-02
*/
float uteDrvTemperatureCommonGetAmbientValue(void)
{
    if(!uteDrvTemperatureCommonIsInitSuccessful())
    {
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,drv not init", __func__)
        return 0.0f;
    }
    return uteDrvTemperatureCommonFunction->getAmbient();
}
/**
*@brief        体温驱动获取皮肤温度
*@details
*@author        zn.zeng
*@date        2021-12-02
*/
float uteDrvTemperatureCommonGetSkinValue(void)
{
    if(!uteDrvTemperatureCommonIsInitSuccessful())
    {
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,drv not init", __func__)
        return 0.0f;
    }
    return uteDrvTemperatureCommonFunction->getSkin();
}
/**
*@brief        打开电源
*@details
*@bool isOnlyOneTime,为ture的时候，只测量一次，自动停止
*@author        zn.zeng
*@date        2021-12-02
*/
void uteDrvTemperatureCommonPowerOn(bool isOnlyOneTime)
{
    if(!uteDrvTemperatureCommonIsInitSuccessful())
    {
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,drv not init", __func__)
        return;
    }
    uteDrvTemperatureCommonFunction->powerOn(isOnlyOneTime);
}
/**
*@brief        关闭电源
*@details
*@author        zn.zeng
*@date        2021-12-02
*/
void uteDrvTemperatureCommonPowerOff(void)
{
    if(!uteDrvTemperatureCommonIsInitSuccessful())
    {
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,drv not init", __func__)
        return;
    }
    uteDrvTemperatureCommonFunction->powerOff();
}


#endif

