/**
*@file
*@brief        ??????????
*@details
*@author       raymond
*@date       2022-03-03
*@version      v1.0
*/
#ifndef _UTE_MODULE_LIFTWRIST_H_
#define _UTE_MODULE_LIFTWRIST_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"

#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_LIFTWRISTINFO "systemparam/liftWristInfo"

typedef struct
{
    bool isOpen;
    uint16_t startHourMin;
    uint16_t endHourMin;
    bool isScheduledOpen;
} ute_module_lift_wrist_data_t;

enum
{
    LIFT_WRIST_START_TIME = 0,
    LIFT_WRIST_END_TIME,
};
enum
{
    LIFT_WRIST_CLOSE = 0,
    LIFT_WRIST_ALLDAY_OPEN,
    LIFT_WRIST_SCHEDULED_OPEN,
};
void uteModuleliftWristReadConfig(void);
void uteModuleLiftWristInit(void);
void uteModuleLiftWristSaveParam(void);
bool uteModuleLiftWristIsTimeBucket(void);
void uteModuleLiftWristSetTimeStatus(uint8_t timestatus);
uint8_t uteModuleLiftWristGetTimeStatus(void);
void uteModuleLiftWristSetTime(uint16_t LiftWristHourMin);
uint16_t uteModuleLiftWristGetTime(uint8_t timestatus);
bool uteModuleLiftWristIsOpen(void);
void uteModuleLiftWristSetOpenStatus(bool isopen);
bool uteModuleLiftWristIsOpenScheduled(void);
void uteModuleLiftWristSetScheduled(bool isopen);
void uteModuleLiftWristRecordOpenStatus(void);
bool uteModuleLiftWristGetOldOpenStatus(void);
bool uteModuleLiftWristGetOldScheduledOpenStatus(void);
void uteModuleLiftWristSetNowOpenStatus(uint8_t openstatus);
uint8_t uteModuleLiftWristGetNowOpenStatus(void);

#endif //_UTE_MODULE_NOTDISTURB_H_


