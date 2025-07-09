/**
*@file
*@brief        马达驱动
*@details
*@author       zn.zeng
*@date       2021-10-12
*@version      v1.0
*/
#ifndef _UTE_DRV_MOTOR_H_
#define _UTE_DRV_MOTOR_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"

/*! 马达震动强度数据 dengli.lu, 2022-06-16  */
#define UTE_MODULE_FILESYSTEM_MOTOR_VIBRATION_INFO "systemparam/motorVibration"

/*! 马达震动强度数据 dengli.lu, 2022-06-16  */
#define UTE_MODULE_FILESYSTEM_MOTOR_VIBRATION_SWITCH_STATUS "systemparam/VibrationSwitch"

/*! 马达震动数据结构 zn.zeng, 2021-10-12  */
typedef struct
{
    bool isRunning;
    uint8_t currMaxCnt;
    uint8_t currCnt;
    uint8_t motorVibrationLevel;
    uint16_t durationTimeMsec;
    uint16_t intervalTimeMsec;
    bool isDuration;
    bool isAllowMotorVibration;//允许振动
    bool MotorVibrationSwitch;
    bool MotorIsOpenVibration;
    uint8_t motorTempVibrationLevel; //临时震动
} ute_drv_motor_t;
void uteDrvMotorInit(void);
void uteDrvMotorVibrationReadConfig(void);
void uteDrvMotorStart(uint32_t durationTimeMsec,uint32_t intervalTimeMsec,uint8_t cnt);
void uteDrvMotorStop(void);
void uteDrvMotorStartHandlerMsg(uint32_t param);
void uteDrvMotorStopHandlerMsg(void);
void uteDrvMotorTimerHandlerMsg(void);
bool uteDrvMotorGetRunningStatus(void);
void uteDrvMotorEnable(void);
void uteDrvMotorDisable(void);
void uteDrvMotorSetVibrationLevel(uint8_t motorVibrationLevel);
uint8_t uteDrvMotorGetVibrationLevel(void);
void uteDrvMotorSetIsAllowMotorVibration(bool isAllowMotorVibration);
bool uteDrvMotorGetIsAllowMotorVibration(void);



/*! 马达震动的开关状态,无论开关是否打开，查找手环震动，其他都不震动 pcm, 2022-11-11  */
#if UTE_MODULE_MOTOR_POWER_STATUS_SAVE_SUPPORT
void uteModuleMotorSaveVibrationSwitchStatus(void);
void uteModuleMotorReadVibrationSwitchStatus(void);
bool uteModuleMotorGetVibrationSwitchStatus(void);
void uteModuleMotorSetVibrationSwitchStatus(bool MotorVibrationSwitch);
bool uteModuleMotorGetIsOpenVibrationStatus(void);
void uteModuleMotorSetIsOpenVibrationStatus(bool MotorIsOpenVibration);
#endif


void uteDrvMotorSetTempVibrationLevel(uint8_t motorVibrationLevel);

#endif//_UTE_DRV_MOTOR_H_

