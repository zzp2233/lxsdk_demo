/**
*@file
*@brief        天气模块
*@details
*@author       zn.zeng
*@date       2021-08-25
*@version      v1.0
*/
#ifndef _UTE_MODULE_WEATHER_H_
#define _UTE_MODULE_WEATHER_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"

/*!写入天气信息 zn.zeng, 2021-08-26  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_WEATHERINFO  "systemparam/weatherInfo"

/*! 7天气数据结构zn.zeng, 2021-08-06  */
typedef struct
{
    uint16_t fristDayPm25;
    uint16_t fristDayAqi;
    int8_t fristDayCurrTemperature;
    uint16_t DayWeather[7];
    int8_t dayTemperatureMin[7];
    int8_t dayTemperatureMax[7];
    uint8_t fristDayWeatherHumidity;
    uint8_t fristDayWeatherUltraviolet;
    uint8_t currDay;
    ute_module_systemtime_time_t updateTime;
#if UTE_MODULE_WEATHER_CITY_NAME_NEW_SUPPORT
    uint16_t cityNameLen;
    uint8_t cityString[UTE_WEATHER_CITY_NAME_TITLE_MAX_SIZE];
#endif
} ute_module_weather_data_t;

typedef enum
{
    WEATHER_TYPE_UNKNOWN =0x00,     //未知
    WEATHER_TYPE_SUNNY =0x01,       // 晴天
    WEATHER_TYPE_CLOUDY,            // 多云
    WEATHER_TYPE_OVERCAST,          //阴
    WEATHER_TYPE_SHOWER_RAIN,       //阵雨
    WEATHER_TYPE_THUNDERSHOWER_RAIN,//雷阵雨
    WEATHER_TYPE_RAIN_SNOW,         //雨夹雪
    WEATHER_TYPE_LIGHT_RAIN,        //小雨
    WEATHER_TYPE_HEAVY_RAIN,        //大雨
    WEATHER_TYPE_SNOW,              //雪
    WEATHER_TYPE_SAND_STORM,        //沙尘暴
    WEATHER_TYPE_HAZE,              //雾霾
    WEATHER_TYPE_WIND,              //风
    WEATHER_TYPE_NIGHT,             //晚上
} weather_type_id;
void uteModuleWeatherInit(void);
void uteModuleWeatherReadConfig(void);
void uteModuleWeatherSaveData(void);
void uteModuleWeatherGetData(ute_module_weather_data_t *data);
void uteModuleWeatherSetData(ute_module_weather_data_t data);
uint8_t uteModuleWeatherGetCurrDay(void);
uint8_t uteModuleWeatherGetUltravioletLevel(uint8_t ultraviolet);
#if UTE_MODULE_WEATHER_CITY_NAME_NEW_SUPPORT
void uteModuleWeatherSetCityName(uint8_t *Data,uint16_t size);
void uteModuleWeatherGetCityName(uint8_t *Data,uint16_t *size);
#endif
#endif //_UTE_MODULE_WEATHER_H_

