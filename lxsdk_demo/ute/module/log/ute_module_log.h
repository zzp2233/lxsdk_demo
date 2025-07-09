/**
*@file
*@brief     ute log 模块h文件
*@details    log 模块，打印函数转换
*@author        zn.zeng
*@date        Jun 29, 2021
*@version       v1.0
*/

#ifndef _UTE_MODULE_LOG_H_
#define _UTE_MODULE_LOG_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"

#if UTE_MODULE_LOG_SUPPORT
#define UTE_LOG_SYSTEM_LVL  1   //  1
#define UTE_LOG_FILE_SYSTEM_LVL 0 //2
#define UTE_LOG_KEYS_LVL 0 //3
#define UTE_LOG_HEART_LVL 0 //4
#define UTE_LOG_BP_LVL 0 //5
#define UTE_LOG_OXYGEN_LVL 0 //6
#define UTE_LOG_STEP_LVL 0 //7
#define UTE_LOG_PROTOCOL_LVL 8//8
#define UTE_LOG_NOTIFY_LVL 0//9
#define UTE_LOG_WEATHER_LVL 0 //10
#define UTE_LOG_BREATHRATE_LVL 0 //11
#define UTE_LOG_GUI_LVL 0 //12
#define UTE_LOG_DRV_SCREEN_LVL 0 //13
#define UTE_LOG_DRV_TP_LVL 0 //14
#define UTE_LOG_DRV_MOTOR_LVL 0 //15
#define UTE_LOG_SLEEP_LVL 0 //16
#define UTE_LOG_BT_AUDIO_LVL 0 //17
#define UTE_LOG_DRV_BAT_LVL 0//18
#define UTE_LOG_MUSIC_LVL 0 //19
#define UTE_LOG_WATCHONLINE_LVL 0 //20
#define UTE_LOG_TEMPERATURE_LVL 0 //21
#define UTE_LOG_CALL_LVL 0 //22
#define UTE_LOG_MICRECORD_LVL 0 //23
#define UTE_LOG_TIME_LVL 0//24
#define UTE_LOG_EMOTION_PRESSURE_LVL 0//25
#define UTE_LOG_DRV_PSRAM 0//26
#define UTE_LOG_EARPHONE_LVL 0//27
#define UTE_LOG_ALI_UPAY_LVL 0//28
#define UTE_LOG_CYWEE_MOTION_LVL 0 //29
#define UTE_LOG_MEMORY_LVL 0 //30

/** 打印log，输入log等级和格式等*/
extern const uint8_t uteModuleLogTagString[][31];
#define UTE_MODULE_LOG(log_lvl,format,...) \
{\
  if((log_lvl!=0)&&(log_lvl<31))\
  {\
   printf("%s:"format"\n",&uteModuleLogTagString[log_lvl][0],##__VA_ARGS__);\
  }\
}
#define UTE_MODULE_LOG_BUFF(log_lvl,buff, size)\
{\
  if((log_lvl!=0)&&(log_lvl<31))\
  {\
   printf("%s:",&uteModuleLogTagString[log_lvl][0]);\
   uint8_t *tmpBuff = (uint8_t *)buff;\
   for(uint32_t i=0;i<size;i++)\
   {\
     if((i%16)==0)\
     {\
        printf("\n");\
     }\
     printf("%02x ",tmpBuff[i]);\
   }\
   printf("\n");\
  }\
}

#if UTE_MODULE_RUNING_LOG_SUPPORT
#define UTE_MODULE_LOG_RUNING_MAX_BUFF_SIZE 256
/** 打印log，输入log等级和格式等*/
#define UTE_MODULE_LOG_BLE(log_lvl,format,...) uteModuleLogRuningPrintf(log_lvl,format,##__VA_ARGS__)
void uteModuleLogSetSendRuningLogSwitch(bool status);
void uteModuleLogRuningPrintf(uint8_t log_lvl, const char * format,...);
void uteModuleLogSetSendRuningLogSwitch(bool status);
#else
#define UTE_MODULE_LOG_BLE(dbg_lvl,format,...)
#endif

#else
#define UTE_MODULE_LOG(dbg_lvl,format,...)
#define UTE_MODULE_LOG_BUFF(dbg_lvl,buff, size)
#define UTE_MODULE_LOG_BLE(dbg_lvl,format,...)
#endif

#endif //_UTE_MODULE_LOG_H_

