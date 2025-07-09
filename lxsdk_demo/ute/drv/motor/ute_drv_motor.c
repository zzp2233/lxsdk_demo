/**
*@file
*@brief        马达驱动
*@details
*@author       zn.zeng
*@date       2021-10-12
*@version      v1.0
*/
#include "ute_drv_motor.h"
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_drv_battery_common.h"
#include "ute_application_common.h"
#include "ute_module_platform.h"
#include "ute_module_notdisturb.h"


/*! 马达运行数据zn.zeng, 2021-10-12  */
ute_drv_motor_t uteDrvMotorData;
void *uteDrvMotorTimerPointer;
/**
*@brief        马达初始化
*@details
*@author       zn.zeng
*@date       2021-10-12
*/

void uteDrvMotorInit(void)
{
    memset(&uteDrvMotorData,0,sizeof(ute_drv_motor_t));
    uteDrvMotorVibrationReadConfig();
#if UTE_MODULE_MOTOR_POWER_STATUS_SAVE_SUPPORT
    uteModuleMotorReadVibrationSwitchStatus();
#endif
}

/**
*@brief     读取马达数据到内存
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
void uteDrvMotorVibrationReadConfig(void)
{
    /*! 保存到文件dengli.lu, 2022-06-16*/
    void *file;
    uint8_t readbuff[2];
    memset(readbuff,0,2);
    readbuff[0] = UTE_MODULE_MOTOR_VIBRATION_DEFAULT_LEVEL;
    readbuff[1] = UTE_MOTOR_ALLOW_VIBRATION_DEFAULT_OPEN;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MOTOR_VIBRATION_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],2);
        uteModuleFilesystemCloseFile(file);
    }
    uteDrvMotorData.motorVibrationLevel = readbuff[0];
    uteDrvMotorData.isAllowMotorVibration = readbuff[1];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,uteDrvMotorData.motorVibrationLevel=%d", __func__,uteDrvMotorData.motorVibrationLevel);
}

/**
*@brief     保存马达数据到内存
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
void uteDrvMotorVibrationSaveData(void)
{
    /*! 保存到文件，dengli.lu, 2022-06-16*/
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteDrvMotorData.motorVibrationLevel;
    writebuff[1] = uteDrvMotorData.isAllowMotorVibration;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MOTOR_VIBRATION_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],2);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief        马达开始震动
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvMotorEnable(void)
{
#if UTE_DRV_MOTOR_PWM_MODE_SUPPORT
    //开始振动前先拉高引脚，防止马达差异引起不振动现象，dengli.lu 2022/08/30
    uteModulePlatformPwmInit(UTE_DRV_MOTOR_PWM_ID,UTE_DRV_MOTOR_GPIO_PIN,80,UTE_DRV_MOTOR_PWM_RATE_HZ);
    uteModulePlatformPwmEnable(UTE_DRV_MOTOR_PWM_ID);
    uteModulePlatformDelayMs(20);
    uint8_t motorDuty = 0;
    if(uteDrvMotorData.motorTempVibrationLevel != 0)  //使用临时震动值
    {
        motorDuty = uteDrvMotorData.motorTempVibrationLevel;
    }
    else
    {
        if (0)
        {}
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
        else if(uteModuleNewFactoryTestGetMode()!= FACTORY_TEST_MODE_NULL)
        {
            motorDuty = (uint8_t)((UTE_DRV_MOTOR_FACTORY_MODE_VOLTAGE_SUPPORT/curVoltage)*100);
        }
#endif
        else
        {
            motorDuty = uteDrvMotorData.motorVibrationLevel;
        }
        UTE_MODULE_LOG(UTE_LOG_DRV_MOTOR_LVL, "%s,motorDuty=%d,motorVibrationLevel=%d",__func__,motorDuty,uteDrvMotorData.motorVibrationLevel);
    }
    UTE_MODULE_LOG(UTE_LOG_DRV_MOTOR_LVL, "%s,motorDuty=%d",__func__,motorDuty);
    uteModulePlatformPwmInit(UTE_DRV_MOTOR_PWM_ID,UTE_DRV_MOTOR_GPIO_PIN,motorDuty,UTE_DRV_MOTOR_PWM_RATE_HZ);
    uteModulePlatformPwmEnable(UTE_DRV_MOTOR_PWM_ID);
    uteModulePlatformDlpsDisable(UTE_MODULE_PLATFORM_DLPS_BIT_MOTOR);
#else
    uteModulePlatformOutputGpioSet(UTE_DRV_MOTOR_GPIO_PIN,true);
#endif
}
/**
*@brief        马达停止震动
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvMotorDisable(void)
{
#if UTE_DRV_MOTOR_PWM_MODE_SUPPORT
    uteModulePlatformPwmDisable(UTE_DRV_MOTOR_PWM_ID,UTE_DRV_MOTOR_GPIO_PIN);
    uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_MOTOR);
#endif
    uteModulePlatformOutputGpioSet(UTE_DRV_MOTOR_GPIO_PIN,false);
}

/**
*@brief        马达震动处理函数
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvMotorTimerCallback(void *pxTimer)
{
    if(uteDrvMotorData.isRunning)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_HANDLE_TIMER,0);
    }
}

/**
*@brief        马达开始震动次数
*@details
*@param[in] durationTimeMsec ，每个周期震动时长
*@param[in] intervalTimeMsec , 每个周期静止时长
*@param[in] cnt ,震动次数
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvMotorStart(uint32_t durationTimeMsec,uint32_t intervalTimeMsec,uint8_t cnt)
{
    if(uteDrvMotorData.motorVibrationLevel == 0 && uteDrvMotorData.motorTempVibrationLevel == 0)
    {
        return;
    }
    if((uteDrvBatteryCommonGetChargerStatus()==BAT_STATUS_NO_CHARGE)&&(uteDrvBatteryCommonGetLvl()<10))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,lvl=%d is too low", __func__,uteDrvBatteryCommonGetLvl());
        return;
    }
#if 0//UTE_BT30_CALL_SUPPORT
    ute_bt_call_data_t callData;
    uteModuleCallGetData(&callData);
    if ((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_CALL_ING_ID) || (uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_QUICK_REPLY_LIST_ID) || (callData.state == BT_CALL_ING))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,BT calling not allow motor!", __func__);
        return;
    }
#endif

#if UTE_MODULE_MOTOR_POWER_STATUS_SAVE_SUPPORT
    if((!uteModuleMotorGetVibrationSwitchStatus())&&(!uteModuleMotorGetIsOpenVibrationStatus()))
    {
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
        if(uteModuleNewFactoryTestGetMode()!= FACTORY_TEST_MODE_NULL)
        {
        }
        else
#endif
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,VibrationSwitchStatus=false!", __func__);
            return;
        }
    }
    else
    {
        uteModuleMotorSetIsOpenVibrationStatus(false);
    }
#endif

    if(uteModuleNotDisturbIsAllowVibration()||(!uteApplicationCommonIsPowerOn()))
    {
        uteDrvMotorData.durationTimeMsec = durationTimeMsec;
        uteDrvMotorData.intervalTimeMsec = intervalTimeMsec;
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_START,cnt);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,is not allow notify", __func__);
    }
}
/**
*@brief        马达停止震动
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvMotorStop(void)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_STOP,0);
}
/**
*@brief        马达开始震动msg处理函数
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvMotorStartHandlerMsg(uint32_t param)
{
    if(uteDrvMotorTimerPointer!=NULL)
    {
        uteModulePlatformStopTimer(&uteDrvMotorTimerPointer);
        uteModulePlatformDeleteTimer(&uteDrvMotorTimerPointer);
        uteDrvMotorTimerPointer = NULL;
        uteDrvMotorDisable();
    }
    uteModulePlatformCreateTimer(&uteDrvMotorTimerPointer, "motor",1, 500, false, uteDrvMotorTimerCallback);
    uteDrvMotorData.currCnt = 0;
    uteDrvMotorData.currMaxCnt = param&0xff;
    uteDrvMotorData.isRunning = true;
    // uteDrvMotorData.durationTimeMsec = UTE_MOTOR_DURATION_TIME;
    // uteDrvMotorData.intervalTimeMsec = UTE_MOTOR_INTERVAL_TIME;
    uteDrvMotorData.isDuration = true;
    uteDrvMotorEnable();
    uteModulePlatformRestartTimer(&uteDrvMotorTimerPointer,uteDrvMotorData.durationTimeMsec);
    UTE_MODULE_LOG(UTE_LOG_DRV_MOTOR_LVL, "%s,param=%d, uteDrvMotorTimerPointer = %d",__func__,param,uteDrvMotorTimerPointer);
}
/**
*@brief        马达停止震动msg处理函数
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvMotorStopHandlerMsg(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_MOTOR_LVL, "%s,.isRunning = %d uteDrvMotorTimerPointer = %d",__func__,uteDrvMotorData.isRunning,uteDrvMotorTimerPointer);
    if(uteDrvMotorData.isRunning)
    {
        uteDrvMotorDisable();
        uteModulePlatformStopTimer(&uteDrvMotorTimerPointer);
        uteModulePlatformDeleteTimer(&uteDrvMotorTimerPointer);
        uteDrvMotorData.isRunning = false;
        uteDrvMotorTimerPointer = NULL;
        uteDrvMotorData.currMaxCnt = 0;
    }
    uteDrvMotorData.motorTempVibrationLevel = 0;
}

/**
*@brief        马达震动定时器处理函数
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvMotorTimerHandlerMsg(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_MOTOR_LVL, "%s,isDuration=%d,.currCnt=%d,.currMaxCnt=%d",__func__,uteDrvMotorData.isDuration,uteDrvMotorData.currCnt,uteDrvMotorData.currMaxCnt);
    if(!uteDrvMotorData.isDuration)
    {
        // if(uteDrvMotorData.currMaxCnt)
        {
            uteDrvMotorData.currCnt++;
            if(uteDrvMotorData.currCnt>=uteDrvMotorData.currMaxCnt)
            {
                uteDrvMotorStopHandlerMsg();
                return;
            }
        }
    }
    uteDrvMotorData.isDuration=!uteDrvMotorData.isDuration;
    if(uteDrvMotorData.isDuration)
    {
        uteDrvMotorEnable();
        uteModulePlatformRestartTimer(&uteDrvMotorTimerPointer,uteDrvMotorData.durationTimeMsec);
    }
    else
    {
        uteDrvMotorDisable();
        uteModulePlatformRestartTimer(&uteDrvMotorTimerPointer,uteDrvMotorData.intervalTimeMsec);
    }
}

/**
*@brief        获取马达震动状态
*@details
*@author       dengli.lu
*@date       2021-12-09
*/
AT(.com_text.ute_drv_motor)
bool uteDrvMotorGetRunningStatus(void)
{
    return uteDrvMotorData.isRunning;
}

/**
*@brief     设置马达震动强度
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
void uteDrvMotorSetVibrationLevel(uint8_t motorVibrationLevel)
{
    if(motorVibrationLevel > 100)
    {
        motorVibrationLevel = 100;
    }
    uteDrvMotorData.motorVibrationLevel = motorVibrationLevel;
    uteDrvMotorVibrationSaveData();
}
/**
*@brief     获取马达震动强度
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
uint8_t uteDrvMotorGetVibrationLevel(void)
{
    return uteDrvMotorData.motorVibrationLevel;
}
/**
*@brief     设置马达允许震动
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
void uteDrvMotorSetIsAllowMotorVibration(bool isAllowMotorVibration)
{
    uteDrvMotorData.isAllowMotorVibration = isAllowMotorVibration;
    uteDrvMotorVibrationSaveData();
}
/**
*@brief     获取马达允许震动
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
bool uteDrvMotorGetIsAllowMotorVibration(void)
{
    return uteDrvMotorData.isAllowMotorVibration;
}


/*! 马达震动的开关状态,无论开关是否打开，查找手环震动，其他都不震动 pcm, 2022-11-11  */
#if UTE_MODULE_MOTOR_POWER_STATUS_SAVE_SUPPORT
/**
*@brief     保存马达震动的开关状态
*@details
*@author    pcm
*@date      2022-11-11
*/
void uteModuleMotorSaveVibrationSwitchStatus(void)
{
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteDrvMotorData.MotorVibrationSwitch;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MOTOR_VIBRATION_SWITCH_STATUS,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief     读取马达震动的开关状态
*@details
*@author    pcm
*@date      2022-11-11
*/
void uteModuleMotorReadVibrationSwitchStatus(void)
{
    void *file;
    uint8_t readbuff[2];
    readbuff[0] = UTE_MODULE_OTHER_RING_POWER_DEFAULT_ON;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MOTOR_VIBRATION_SWITCH_STATUS,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteDrvMotorData.MotorVibrationSwitch = readbuff[0];
//    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,uteModuleLocalRingtoneData.OtherLocalRingtoneStatu=%d", __func__,uteModuleLocalRingtoneData.OtherLocalRingtoneStatu);
}

/**
*@brief  获取马达震动的开关状态
*@details
*@return
*@author      pcm
*@date        2022-11-11
*/
bool uteModuleMotorGetVibrationSwitchStatus(void)
{
    return uteDrvMotorData.MotorVibrationSwitch;
}
/**
*@brief  设置马达震动的开关状态
*@details
*@param[in]
*@author      pcm
*@date        2022-11-11
*/
void uteModuleMotorSetVibrationSwitchStatus(bool MotorVibrationSwitch)
{
    uteDrvMotorData.MotorVibrationSwitch = MotorVibrationSwitch;
    uteModuleMotorSaveVibrationSwitchStatus();
}

/**
*@brief  获取当前否需要打开马达震动
*@details
*@return
*@author      pcm
*@date        2022-11-11
*/
bool uteModuleMotorGetIsOpenVibrationStatus(void)
{
    return uteDrvMotorData.MotorIsOpenVibration;
}
/**
*@brief  设置当前否需要打开马达震动
*@details
*@param[in]
*@author      pcm
*@date        2022-11-11
*/
void uteModuleMotorSetIsOpenVibrationStatus(bool MotorIsOpenVibration)
{
    uteDrvMotorData.MotorIsOpenVibration = MotorIsOpenVibration;
//    uteModuleMotorSaveVibrationSwitchStatus();
}
#endif

/**
*@brief     设置马达震动强度不保存
*@details
*@author      casen
*@date        2022-11-14
*/
void uteDrvMotorSetTempVibrationLevel(uint8_t motorVibrationLevel)
{
    if(motorVibrationLevel > 100)
    {
        motorVibrationLevel = 100;
    }
    uteDrvMotorData.motorTempVibrationLevel = motorVibrationLevel;
}

