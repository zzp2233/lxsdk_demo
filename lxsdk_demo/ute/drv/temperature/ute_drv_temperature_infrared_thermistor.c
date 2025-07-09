/**
*@file
*@brief        红外热敏电阻驱动文件
*@details
*@author       zn.zeng
*@date       2021-12-02
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_drv_temperature_infrared_thermistor.h"
#include "ute_project_config.h"
#include "include.h"
#include <math.h>

#if UTE_DRV_TEMPERATURE_INFRARED_THERMISTOR_SUPPORT
float uteDrvTemperatureInfraredThermistorAmbient = 0;
float uteDrvTemperatureInfraredThermistorSkin = 0;
uint8_t uteDrvTemperatureInfraredThermistorAdcHandler = 0;
bool uteDrvTemperatureInfraredThermistorIsRegAdc = false;
bool uteDrvTemperatureInfraredThermistorIsOnlyOneTime = false;

// 泰勒级数近似 log(x)
// float taylorLog(float x, int terms)
// {
//     float result = 0.0f;
//     float term = x - 1.0f;
//     for (int i = 1; i <= terms; i++)
//     {
//         result += term / i;
//         term *= (x - 1.0f);
//     }
//     return result;
// }

/**
*@brief         串联模式下，从adc值转换成温度值
*@details
*@param[in] uint16_t vddAdc,采样到的VDD电压
*@param[in] uint16_t adc,采样到的adc数值
*@return ,转换后的数值，单位为摄氏度
*@author        zn.zeng
*@date        2021-12-02
B=ln(RT/RTk)*[(T*Tk)/(Tk-T)]
Tk=B*T/[B-T*ln(RT/RTk)]
RT为25+273.15开氏度时电阻值
RTk为Tk开氏度时电阻值（被测温度对应阻值）
T为25+273.15，开氏温度单位K
Tk为Tc+273.15，开氏温度单位K（被测温度）
1、令B=4250，求Tk=B*T/[B-T*ln(RT/RTk)]
B为规格书标称典型值，具体参考规格书
RT为NTC在25℃(298.15K)时典型电阻值，和NTC有关
RTk为Tk开氏度时电阻值,RTk=Vtk*Rs/(Vdd-Vtk),Rs为硬件上拉电阻值1500
T=Tk-273.15
*/
float uteDrvTemperatureInfraredThermistorSeriesConversionTemperature(uint16_t vddAdc,uint16_t adc)
{
    uint16_t b = UTE_DRV_INFRARED_THERMISTOR_RT_B;
    uint16_t RS = UTE_DRV_INFRARED_THERMISTOR_RS;
    uint16_t RT = UTE_DRV_INFRARED_THERMISTOR_RT;
    float t = 25+273.15f;//298.15
#if UTE_DRV_INFRARED_THERMISTOR_IS_PARALLEL
    uint16_t RTp = UTE_DRV_INFRARED_THERMISTOR_PARALLEL_RT;
    double RTk = adc/((vddAdc-adc)/RS*1.0f-adc/RTp*1.0f)*1.0f;
#else
    double RTk = adc*RS*1.0f/(vddAdc-adc);
#endif
    double logvalue = (double)(RT/RTk);
    // double lnvalue = taylorLog(logvalue, 5);
    double lnvalue = log(logvalue);
    float tk = b*t/(b-t*lnvalue);
    tk = tk-273.15f;
    return tk;
}

/**
*@brief        红外热敏电阻驱动关闭电源
*@details
*@author        zn.zeng
*@date        2021-12-02
*/
void uteDrvTemperatureInfraredThermistorPowerOff(void)
{
#if UTE_MODULE_TEMPERATURE_POWER_SUPPORT
    uteModulePlatformOutputGpioSet(UTE_MODULE_TEMPERATURE_POWER_GPIO_PIN,!UTE_MODULE_TEMPERATURE_POWER_ACTION_LEVEL);
#endif
    if(uteDrvTemperatureInfraredThermistorIsRegAdc)
    {
        uteDrvTemperatureInfraredThermistorIsRegAdc = false;
    }
}

/**
*@brief        红外热敏电阻开始采样
*@details
*@author        zn.zeng
*@date        2021-12-02
*/
void uteDrvTemperatureInfraredThermistorStartSample(void)
{
    uint16_t skinAdc;
    uint16_t ambientAdc;
    uint16_t vddAdc;
    saradc_set_channel(BIT(UTE_DRV_TEMPERATURE_INFRARED_THERMISTOR_SKIN_ADC));
    skinAdc = saradc_get_value10(UTE_DRV_TEMPERATURE_INFRARED_THERMISTOR_SKIN_ADC);
    saradc_set_channel(BIT(UTE_DRV_TEMPERATURE_INFRARED_THERMISTOR_AMBIENT_ADC));
    ambientAdc = saradc_get_value10(UTE_DRV_TEMPERATURE_INFRARED_THERMISTOR_AMBIENT_ADC);
#ifdef  UTE_DRV_TEMPERATURE_INFRARED_THERMISTOR_VDD_ADC
    saradc_set_channel(BIT(UTE_DRV_TEMPERATURE_INFRARED_THERMISTOR_VDD_ADC));
    vddAdc = saradc_get_value10(UTE_DRV_TEMPERATURE_INFRARED_THERMISTOR_VDD_ADC);
#else
    saradc_set_channel(ADCCH15_ANA_BG);
    // adc_cb.bg = saradc_get_value10(ADCCH_BGOP);
    vddAdc =  sys_trim.vbg_volt * 1024 / adc_cb.bg / 10;
#endif

    ambientAdc = ambientAdc * vddAdc / 1024;
    skinAdc = skinAdc * vddAdc / 1024;

    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"InfraredThermistor,skinAdc=%d,ambientAdc=%d,vddAdc=%d",skinAdc,ambientAdc,vddAdc);
    uteDrvTemperatureInfraredThermistorSkin = uteDrvTemperatureInfraredThermistorSeriesConversionTemperature(vddAdc,skinAdc);
    uteDrvTemperatureInfraredThermistorAmbient = uteDrvTemperatureInfraredThermistorSeriesConversionTemperature(vddAdc,ambientAdc);
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"InfraredThermistor,skin=%d,ambient=%d",(int16_t)uteDrvTemperatureInfraredThermistorSkin,(int16_t)uteDrvTemperatureInfraredThermistorAmbient);
    if(uteDrvTemperatureInfraredThermistorIsOnlyOneTime)
    {
        uteDrvTemperatureInfraredThermistorPowerOff();
    }
}
/**
*@brief        红外热敏电阻驱动获取环境温度
*@details
*@author        zn.zeng
*@date        2021-12-02
*/
float uteDrvTemperatureInfraredThermistorGetAmbientValue(void)
{
    return uteDrvTemperatureInfraredThermistorAmbient;
}
/**
*@brief        红外热敏电阻驱动获取皮肤温度
*@details
*@author        zn.zeng
*@date        2021-12-02
*/
float uteDrvTemperatureInfraredThermistorGetSkinValue(void)
{
    return uteDrvTemperatureInfraredThermistorSkin;
}

/**
*@brief        红外热敏电阻驱动打开电源
*@details
*@bool isOnlyOneTime,为ture的时候，只测量一次，自动停止
*@author        zn.zeng
*@date        2021-12-02
*/
void uteDrvTemperatureInfraredThermistorPowerOn(bool isOnlyOneTime)
{
    if(!uteDrvTemperatureInfraredThermistorIsRegAdc)
    {
#if UTE_MODULE_TEMPERATURE_POWER_SUPPORT
        uteModulePlatformOutputGpioSet(UTE_MODULE_TEMPERATURE_POWER_GPIO_PIN,UTE_MODULE_TEMPERATURE_POWER_ACTION_LEVEL);
#endif
        uteDrvTemperatureInfraredThermistorIsRegAdc = true;
        uteModulePlatformDelayMs(15);
        uteDrvTemperatureInfraredThermistorIsOnlyOneTime = isOnlyOneTime;
    }
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"%s",__func__);
}

const ute_drv_temperature_common_config_t uteDrvTemperatureInfraredThermistorConfig=
{
    .name = "infrared thermistor",
    .startSample = uteDrvTemperatureInfraredThermistorStartSample,
    .getAmbient = uteDrvTemperatureInfraredThermistorGetAmbientValue,
    .getSkin = uteDrvTemperatureInfraredThermistorGetSkinValue,
    .powerOn = uteDrvTemperatureInfraredThermistorPowerOn,
    .powerOff = uteDrvTemperatureInfraredThermistorPowerOff,
};
#endif

