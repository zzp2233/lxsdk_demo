/**
*@file
*@brief        天气模块
*@details
*@author       zn.zeng
*@date       2021-08-25
*@version      v1.0
*/
#include "ute_module_weather.h"
#include "ute_module_log.h"
#include "ute_module_filesystem.h"
#include "ute_module_charencode.h"

/*! 天气数据 zn.zeng, 2021-08-25  */
ute_module_weather_data_t uteModuleWeatherData;
/* 天气互斥量 zn.zeng 2022-02-14*/
void *uteModuleWeatherMute;
/**
*@brief            读取天气数据
*@details
*@author        zn.zeng
*@date        2021-08-25
*/
void uteModuleWeatherReadConfig(void)
{
    void *file = NULL;
    memset(&uteModuleWeatherData,0,sizeof(ute_module_weather_data_t));
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_WEATHERINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,(void*)&uteModuleWeatherData,sizeof(ute_module_weather_data_t));
        uteModuleFilesystemCloseFile(file);
    }
#if UTE_MODULE_LOG_SUPPORT
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,currDay=%d,pm25=%d,aqi=%d,%d C", __func__,uteModuleWeatherData.currDay,uteModuleWeatherData.fristDayPm25,uteModuleWeatherData.fristDayAqi,uteModuleWeatherData.fristDayCurrTemperature);
    for(uint8_t i=0; i<7; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,weather=0x%02x,min=%d C,max=%d C", __func__,uteModuleWeatherData.DayWeather[i],uteModuleWeatherData.dayTemperatureMin[i],uteModuleWeatherData.dayTemperatureMax[i]);
    }
#endif

}
/**
*@brief            初始化
*@details
*@author        zn.zeng
*@date       2022-02-14
*/
void uteModuleWeatherInit(void)
{
    uteModuleWeatherReadConfig();
    uteModulePlatformCreateMutex(&uteModuleWeatherMute);
}
/**
*@brief            保存天气数据
*@details
*@author        zn.zeng
*@date        2021-08-25
*/
void uteModuleWeatherSaveData(void)
{
    printf("KSKSKSKSKS%s,len: %d\n",__func__,__LINE__);
    void *file = NULL;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_WEATHERINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,uteModuleWeatherSaveData", __func__);
        uteModuleFilesystemWriteData(file,(void*)&uteModuleWeatherData,sizeof(ute_module_weather_data_t));
        uteModuleFilesystemCloseFile(file);
    }

}
/**
*@brief            获取天气数据
*@details
*@author        zn.zeng
*@date        2021-08-25
*/
void uteModuleWeatherGetData(ute_module_weather_data_t *data)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    /*! 跨天更新天气数zn.zeng, 2021-08-26  */
    uteModulePlatformTakeMutex(uteModuleWeatherMute);
    if((time.day!=uteModuleWeatherData.currDay)&&(uteModuleWeatherData.currDay!=0))
    {
        for(uint8_t i=0; i<6; i++)
        {
            uteModuleWeatherData.DayWeather[i] = uteModuleWeatherData.DayWeather[i+1];
            uteModuleWeatherData.dayTemperatureMin[i] = uteModuleWeatherData.dayTemperatureMin[i+1];
            uteModuleWeatherData.dayTemperatureMax[i] = uteModuleWeatherData.dayTemperatureMax[i+1];
        }
        uteModuleWeatherData.fristDayCurrTemperature = (uteModuleWeatherData.dayTemperatureMin[0]+uteModuleWeatherData.dayTemperatureMax[0])/2;
        uteModuleWeatherData.DayWeather[6] = 0;
        uteModuleWeatherData.dayTemperatureMin[6] = 0;
        uteModuleWeatherData.dayTemperatureMax[6] = 0;
        uteModuleWeatherData.currDay = time.day;
        if(uteModuleWeatherData.DayWeather[0]==0)
        {
            uteModuleWeatherData.currDay = 0;
        }
    }
#if UTE_MODULE_WEATHER_CITY_NAME_NEW_SUPPORT
    UTE_MODULE_LOG(UTE_LOG_WEATHER_LVL,"%s,cityName:", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_WEATHER_LVL,&uteModuleWeatherData.cityString[0],uteModuleWeatherData.cityNameLen);
#endif
    memcpy(data,&uteModuleWeatherData,sizeof(ute_module_weather_data_t));
    uteModulePlatformGiveMutex(uteModuleWeatherMute);
}
/**
*@brief            设置天气数据
*@details
*@author        zn.zeng
*@date        2022-02-14
*/
void uteModuleWeatherSetData(ute_module_weather_data_t data)
{
    uteModulePlatformTakeMutex(uteModuleWeatherMute);
    memcpy(&uteModuleWeatherData,&data,sizeof(ute_module_weather_data_t));
    uteModulePlatformGiveMutex(uteModuleWeatherMute);
}

/**
*@brief         获取当天日期
*@details
*@author        dengli.lu
*@date        2021-10-27
*/
uint8_t uteModuleWeatherGetCurrDay(void)
{
    return uteModuleWeatherData.currDay;
}
#if UTE_MODULE_WEATHER_HUMIDITY_ULTRAVIOLET_SUPPORT
/**
*@brief         紫外数据转换为等级
*@details
*@param[in] uint8_t ultraviolet,紫外数据值
*@return uint8_t 返回等级，0~4，分别为 很弱、弱、中等、强、很强
*@author       zn.zeng
*@date        2022-09-22
*/
uint8_t uteModuleWeatherGetUltravioletLevel(uint8_t ultraviolet)
{
    uint8_t ultravioletLevel = 0;
    if (ultraviolet <= 2) //最弱
    {
        ultravioletLevel = 0;
    }
    else if ((ultraviolet == 3) || (ultraviolet == 4)) //弱
    {
        ultravioletLevel = 1;
    }
    else if ((ultraviolet == 5) || (ultraviolet == 6)) //中等
    {
        ultravioletLevel = 2;
    }
    else if ((ultraviolet >= 7) && (ultraviolet <= 9)) //强
    {
        ultravioletLevel = 3;
    }
    else //很强
    {
        ultravioletLevel = 4;
    }
    return ultravioletLevel;
}
#endif
#if  UTE_MODULE_WEATHER_CITY_NAME_NEW_SUPPORT
/**
*@brief        设置城市名
*@details
*@author        Txj
*@date        2022-11-10
***/
void uteModuleWeatherSetCityName(uint8_t *WeatherData,uint16_t size)
{
    uteModuleWeatherData.cityNameLen=size;
    memset(&uteModuleWeatherData.cityString[0],0,UTE_WEATHER_CITY_NAME_TITLE_MAX_SIZE);
    memcpy(&uteModuleWeatherData.cityString[0],&WeatherData[0],size);
    uteModuleWeatherSaveData();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,cityName:", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,&uteModuleWeatherData.cityString[0],uteModuleWeatherData.cityNameLen);

}
/**
*@brief  获取城市名
*@details  输入编码为utf-8
*@param[in] uint8_t *data,数据
*@param[in] uint16_t size,长度
*@author        Txj
*@date        2022-11-10
*/
void uteModuleWeatherGetCityName(uint8_t *WeatherData,uint16_t *size)
{
    ute_module_weather_data_t data;
    uteModuleWeatherGetData(&data);
    uint16_t len =100;
    uteModuleCharencodeUtf8ConversionUnicode(WeatherData,&len,&data.cityString[0],data.cityNameLen);
    *size = len;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,cityNameLen=%d,len=%d", __func__,data.cityNameLen,len);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,&data.cityString[0],data.cityNameLen);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,WeatherData,len);
}
#endif
