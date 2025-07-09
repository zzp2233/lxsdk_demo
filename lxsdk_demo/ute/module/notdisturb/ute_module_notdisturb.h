/**
*@file
*@brief        勿扰模块
*@details
*@author       zn.zeng
*@date       2021-08-27
*@version      v1.0
*/
#ifndef _UTE_MODULE_NOTDISTURB_H_
#define _UTE_MODULE_NOTDISTURB_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"

/*!写入勿扰信息 zn.zeng, 2021-08-27  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_NOTDISTURBNFO "systemparam/notDisturbInfo"

/*! 勿扰数据结构zn.zeng, 2021-08-27  */
typedef struct
{
    bool isOpen;
    bool isRejectCall;
    bool isNormalNotNotify;
    bool isNormalNotVibration;
    uint16_t startHourMin;
    uint16_t endHourMin;
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    bool isScheduledOpen;
#endif
} ute_module_not_disturb_data_t;
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
enum
{
    NOT_DISTURB_START_TIME = 0,
    NOT_DISTURB_END_TIME,
};
enum
{
    NOT_DISTURB_CLOSE = 0,
    NOT_DISTURB_ALLDAY_OPEN,
    NOT_DISTURB_SCHEDULED_OPEN,
};
void uteModuleNotDisturbSetTimeStatus(uint8_t timestatus);
uint8_t uteModuleNotDisturbGetTimeStatus(void);
void uteModuleNotDisturbSetTime(uint16_t notDisturbHourMin);
uint16_t uteModuleNotDisturbGetTime(uint8_t timestatus);
void uteModuleNotDisturbSetOpenStatus(uint8_t openstatus);
uint8_t uteModuleNotDisturbGetOpenStatus(void);
void uteModuleNotDisturbScheduledSwitch(void);
void uteModuleNotDisturbAllDaySwitch(void);
#endif
void uteModuleNotDisturbReadConfig(void);
void uteModuleNotDisturbInit(void);
void uteModuleNotDisturbSaveParam(ute_module_not_disturb_data_t param);
bool uteModuleNotDisturbIsTimeBucket(void);
bool uteModuleNotDisturbIsAllowNotify(void);
bool uteModuleNotDisturbIsAllowVibration(void);
bool uteModuleNotDisturbIsAllowHandScreenOn(void);
bool uteModuleNotDisturbIsOpenScheduled(void);
void uteModuleNotDisturbSetScheduled(bool isopen);
void uteModuleNotDisturbSetOneKeyRejectCall(bool isOpen);
bool uteModuleNotDisturbIsAllowLocalRingtone(void);
void uteModuleNotDisturbGetParam(ute_module_not_disturb_data_t *param);

#endif //_UTE_MODULE_NOTDISTURB_H_

