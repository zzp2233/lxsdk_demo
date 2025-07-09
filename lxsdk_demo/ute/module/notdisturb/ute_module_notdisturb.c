/**
*@file
*@brief        勿扰模块
*@details
*@author       zn.zeng
*@date       2021-08-27
*@version      v1.0
*/

#include "ute_module_notdisturb.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_drv_motor.h"
#include "ute_module_gui_common.h"
#include "ute_module_sport.h"
#include "ute_module_liftwrist.h"
#include "func_cover.h"

/* NotDisturb 互斥量 zn.zeng 2022-02-14*/
void *uteModuleNotDisturbMute;
/*! 勿扰数据zn.zeng, 2021-08-27  */
ute_module_not_disturb_data_t uteModuleNotDisturbData;
static uint8_t uteModuleNotDisturbTimeStatus;
static uint8_t uteModuleNotDisturbOpenStatus;

#ifndef DEFAULT_NOTDISTURB_SCHEDULED_SWITCH
#define DEFAULT_NOTDISTURB_SCHEDULED_SWITCH false
#endif

#ifndef DEFAULT_NOTDISTURB_ALLDAY_SWITCH
#define DEFAULT_NOTDISTURB_ALLDAY_SWITCH false
#endif

#define UTE_MODULE_NOT_DISTURB_DATA_LENGTH 20

/**
*@brief  读取勿扰参数信息
*@details
*@author        zn.zeng
*@date        2021-08-27
*/
void uteModuleNotDisturbReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[UTE_MODULE_NOT_DISTURB_DATA_LENGTH];
    /*! 勿扰配置zn.zeng, 2021-08-27  */
    readbuff[0] = DEFAULT_DONOT_DISTURB_OPNE;
    readbuff[1] = DEFAULT_DONOT_DISTURB_REJECT_CALL_OPNE;
    readbuff[2] = 0;
    readbuff[3] = 0;
    readbuff[4] = (DEFAULT_DONOT_DISTURB_START_HOUR_MIN_VALUE>>8)&0xff;
    readbuff[5] = DEFAULT_DONOT_DISTURB_START_HOUR_MIN_VALUE&0xff;
    readbuff[6] = (DEFAULT_DONOT_DISTURB_END_HOUR_MIN_VALUE>>8)&0xff;
    readbuff[7] = (DEFAULT_DONOT_DISTURB_END_HOUR_MIN_VALUE)&0xff;
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    readbuff[8] = DEFAULT_NOTDISTURB_SCHEDULED_SWITCH;
    readbuff[9] = DEFAULT_NOTDISTURB_ALLDAY_SWITCH;
#endif
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_NOTDISTURBNFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file,&readbuff[0],UTE_MODULE_NOT_DISTURB_DATA_LENGTH);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleNotDisturbData.isOpen = readbuff[0];
    uteModuleNotDisturbData.isRejectCall = readbuff[1];
    uteModuleNotDisturbData.isNormalNotNotify=readbuff[2];
    uteModuleNotDisturbData.isNormalNotVibration=readbuff[3];
    uteModuleNotDisturbData.startHourMin = readbuff[4]<<8|readbuff[5];
    uteModuleNotDisturbData.endHourMin = readbuff[6]<<8|readbuff[7];
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    uteModuleNotDisturbData.isScheduledOpen = readbuff[8];
#endif
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
#if !UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    quick.isNotDisturb = uteModuleNotDisturbData.isOpen;
#endif
    quick.isRejectCall = uteModuleNotDisturbData.isRejectCall;
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isOpen=%,startHourMin=%d,endHourMin=%d", __func__,uteModuleNotDisturbData.isOpen,uteModuleNotDisturbData.startHourMin,uteModuleNotDisturbData.endHourMin);
}
/**
*@brief  勿扰参数初始化
*@details
*@author        zn.zeng
*@date        2022-02-15
*/
void uteModuleNotDisturbInit(void)
{
    uteModuleNotDisturbReadConfig();
    uteModulePlatformCreateMutex(&uteModuleNotDisturbMute);
}
/**
*@brief  保存勿扰信息
*@details
*@param[in] (ute_not_disturb_param_t param)
*@author        zn.zeng
*@date        2021-08-27
*/
void uteModuleNotDisturbSaveParam(ute_module_not_disturb_data_t param)
{
    uteModulePlatformTakeMutex(uteModuleNotDisturbMute);
    memcpy(&uteModuleNotDisturbData,&param,sizeof(ute_module_not_disturb_data_t));
    uteModulePlatformGiveMutex(uteModuleNotDisturbMute);
    /*! 保存到文件zn.zeng, 2021-08-27*/
    void *file;
    uint8_t writebuff[UTE_MODULE_NOT_DISTURB_DATA_LENGTH];
    writebuff[0] = uteModuleNotDisturbData.isOpen;
    writebuff[1] = uteModuleNotDisturbData.isRejectCall;
    writebuff[2] = uteModuleNotDisturbData.isNormalNotNotify;
    writebuff[3] = uteModuleNotDisturbData.isNormalNotVibration;
    writebuff[4] = (uteModuleNotDisturbData.startHourMin>>8)&0xff;
    writebuff[5] = (uteModuleNotDisturbData.startHourMin)&0xff;
    writebuff[6] = (uteModuleNotDisturbData.endHourMin>>8)&0xff;
    writebuff[7] = (uteModuleNotDisturbData.endHourMin)&0xff;
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    writebuff[8] = uteModuleNotDisturbData.isScheduledOpen;
#endif
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_NOTDISTURBNFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],UTE_MODULE_NOT_DISTURB_DATA_LENGTH);
        uteModuleFilesystemCloseFile(file);
    }
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
#if !UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    quick.isNotDisturb = uteModuleNotDisturbData.isOpen;
#endif
    quick.isRejectCall = uteModuleNotDisturbData.isRejectCall;
    uteApplicationCommonSetQuickSwitchStatus(&quick);
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSprotResetRovllverScreenMode();
#endif
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isOpen=%d,startHourMin=0x%02x,endHourMin=0x%02x", __func__, uteModuleNotDisturbData.isOpen,uteModuleNotDisturbData.startHourMin,uteModuleNotDisturbData.endHourMin);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isNormalNotNotify=%d,isNormalNotVibration=%d,isRejectCall=%d", __func__, uteModuleNotDisturbData.isNormalNotNotify,uteModuleNotDisturbData.isNormalNotVibration,uteModuleNotDisturbData.isRejectCall);
}

/**
*@brief  获取勿扰信息
*@details
*@param[in] (ute_not_disturb_param_t param)
*@author        xjc
*@date        2022-05-05
*/
void uteModuleNotDisturbGetParam(ute_module_not_disturb_data_t *param)
{
    memcpy(param,&uteModuleNotDisturbData,sizeof(ute_module_not_disturb_data_t));
}
/**
*@brief  读取当前是否是勿扰时间段
*@details
*@return true ,当前在勿扰时间段内
*@author        zn.zeng
*@date        2021-08-27
*/
bool uteModuleNotDisturbIsTimeBucket(void)
{
    bool isNotDisturbTimeNow = false;
    uint16_t hourMin = 0;
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    if(uteModuleNotDisturbData.isScheduledOpen)
#else
    if(uteModuleNotDisturbData.isOpen)
#endif
    {
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        hourMin = time.hour*60+time.min;
        if(uteModuleNotDisturbData.startHourMin<uteModuleNotDisturbData.endHourMin)
        {
            if((hourMin>=uteModuleNotDisturbData.startHourMin)&&(hourMin<=uteModuleNotDisturbData.endHourMin))
            {
                isNotDisturbTimeNow = true;
            }
        }
        else
        {
            if((hourMin>=uteModuleNotDisturbData.startHourMin)||(hourMin<=uteModuleNotDisturbData.endHourMin))
            {
                isNotDisturbTimeNow = true;
            }
        }
    }

    return isNotDisturbTimeNow;
}
/**
*@brief  勿扰功能下是否允许消息推送
*@details
*@return
*@author        zn.zeng
*@date        2021-08-27
*/
bool uteModuleNotDisturbIsAllowNotify(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    bool isAllow = true;
    if(uteModuleNotDisturbIsTimeBucket())
    {
        isAllow = false;
    }
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    else if(quick.isNotDisturb)
    {
        isAllow = false;
    }
#endif
    else
    {
        isAllow = !uteModuleNotDisturbData.isNormalNotNotify;
    }
#if UTE_MODULE_POWER_SAVING_SUPPORT
    if(uteModuleGuiCommonGetPowerSavingModeOpen())
    {
        isAllow = false;
    }
#endif
    return isAllow;
}
/**
*@brief  勿扰功能下是否允许震动
*@details
*@return
*@author        zn.zeng
*@date        2021-08-27
*/
bool uteModuleNotDisturbIsAllowVibration(void)
{
    bool isAllow = true;
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if(uteModuleNotDisturbIsTimeBucket())
    {
        isAllow = false;
    }
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    else if(quick.isNotDisturb)
    {
        isAllow = false;
    }
#endif
    else if(!uteDrvMotorGetIsAllowMotorVibration())
    {
        isAllow = false;
    }
    else
    {
        isAllow = !uteModuleNotDisturbData.isNormalNotVibration;
    }
#if UTE_MODULE_POWER_SAVING_SUPPORT
    if(uteModuleGuiCommonGetPowerSavingModeOpen())
    {
        isAllow = false;
    }
#endif
#if UTE_ALARM_NOTDISTURB_ALLOW_MOTOR_VIBRATION_SUPPORT//勿扰时闹钟允许振动
//    if(uteModuleGuiCommonGetCurrentScreenId()==UTE_MOUDLE_SCREENS_ALARM_CLOCK_ID)
    if (sys_cb.cover_index == REMIND_COVER_ALARM)
    {
        isAllow = true;
    }
#endif
    return isAllow;
}
/**
*@brief  勿扰功能下是否允许抬手亮屏
*@details
*@return
*@author        zn.zeng
*@date        2021-08-27
*/
bool uteModuleNotDisturbIsAllowHandScreenOn(void)
{
    bool isAllow = true;
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
#if (!UTE_NOTDISTURB_ALLOW_HAND_SCREEN_ON_SUPPORT)
    if(uteModuleNotDisturbIsTimeBucket())
    {
        isAllow = false;
    }
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    else if(quick.isNotDisturb)
    {
        isAllow = false;
    }
#endif
#endif
#if UTE_MODULE_POWER_SAVING_SUPPORT
    if(uteModuleGuiCommonGetPowerSavingModeOpen())
    {
        isAllow = false;
    }
#endif
    return isAllow;
}
/**
*@brief  勿扰功能下是否允许本地铃声
*@details
*@return
*@author        xjc
*@date        2022-07-25
*/
bool uteModuleNotDisturbIsAllowLocalRingtone(void)
{
    bool isAllow = true;
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
#endif
    if(uteModuleNotDisturbIsTimeBucket())
    {
        isAllow = false;
    }
    // else if(uteModuleLocalRingtoneGetMuteStatus())
    // {
    //     isAllow = false;
    // }
    // else if(uteModuleLocalRingtoneGetPlayRingType() != RINGTON_TYPE_CALL)
    // {
    //     isAllow = false;
    // }
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    else if (quick.isNotDisturb)
    {
        isAllow = false;
    }
#endif
#if UTE_MODULE_POWER_SAVING_SUPPORT
    if(uteModuleGuiCommonGetPowerSavingModeOpen())
    {
        isAllow = false;
    }
#endif
    return isAllow;
}
/**
*@brief  是否打开一键拒接功能
*@details
*@return
*@author        zn.zeng
*@date        2021-08-27
*/
bool uteModuleNotDisturbIsOpenOneKeyRejectCall(void)
{
    return uteModuleNotDisturbData.isRejectCall;
}

void uteModuleNotDisturbSetOneKeyRejectCall(bool isOpen)
{
    uteModuleNotDisturbData.isRejectCall = isOpen;
}

#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT

/**
*@brief  保存本文件勿扰信息
*@details
*@param[in] (ute_not_disturb_param_t param)
*@author        raymond
*@date        2021-08-27
*/
void uteModuleNotDisturbSaveLocalParam(void)
{
    /*! 保存到文件zn.zeng, 2021-08-27*/
    void *file;
    uint8_t writebuff[UTE_MODULE_NOT_DISTURB_DATA_LENGTH];
    writebuff[0] = uteModuleNotDisturbData.isOpen;
    writebuff[1] = uteModuleNotDisturbData.isRejectCall;
    writebuff[2] = uteModuleNotDisturbData.isNormalNotNotify;
    writebuff[3] = uteModuleNotDisturbData.isNormalNotVibration;
    writebuff[4] = (uteModuleNotDisturbData.startHourMin>>8)&0xff;
    writebuff[5] = (uteModuleNotDisturbData.startHourMin)&0xff;
    writebuff[6] = (uteModuleNotDisturbData.endHourMin>>8)&0xff;
    writebuff[7] = (uteModuleNotDisturbData.endHourMin)&0xff;
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    writebuff[8] = uteModuleNotDisturbData.isScheduledOpen;
#endif
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_NOTDISTURBNFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],8);
        uteModuleFilesystemCloseFile(file);
    }
    // ute_quick_switch_t quick;
    // uteApplicationCommonGetQuickSwitchStatus(&quick);
    // quick.isRejectCall = uteModuleNotDisturbData.isRejectCall;
    // uteApplicationCommonSetQuickSwitchStatus(quick);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isOpen=%d,startHourMin=0x%02x,endHourMin=0x%02x", __func__, uteModuleNotDisturbData.isOpen,uteModuleNotDisturbData.startHourMin,uteModuleNotDisturbData.endHourMin);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isNormalNotNotify=%d,isNormalNotVibration=%d,isRejectCall=%d", __func__, uteModuleNotDisturbData.isNormalNotNotify,uteModuleNotDisturbData.isNormalNotVibration,uteModuleNotDisturbData.isRejectCall);
}
/**
*@brief  设置时间状态
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
void uteModuleNotDisturbSetTimeStatus(uint8_t timestatus)
{
    uteModuleNotDisturbTimeStatus = timestatus;
    UTE_MODULE_LOG(1, "%s,startHourMin=0x%02x,endHourMin=0x%02x",__func__, uteModuleNotDisturbData.startHourMin, uteModuleNotDisturbData.endHourMin);
}

/**
*@brief  获取时间状态
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
uint8_t uteModuleNotDisturbGetTimeStatus(void)
{
    return uteModuleNotDisturbTimeStatus;
}

/**
*@brief  设置开始或结束时间
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
void uteModuleNotDisturbSetTime(uint16_t notDisturbHourMin)
{
    uteModulePlatformTakeMutex(uteModuleNotDisturbMute);
    if (uteModuleNotDisturbTimeStatus == NOT_DISTURB_START_TIME)
    {
        uteModuleNotDisturbData.startHourMin = notDisturbHourMin;
    }
    else
    {
        uteModuleNotDisturbData.endHourMin = notDisturbHourMin;
    }
    uteModuleNotDisturbSaveLocalParam();
    uteModulePlatformGiveMutex(uteModuleNotDisturbMute);

}

/**
*@brief  获取开始或结束时间
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
uint16_t uteModuleNotDisturbGetTime(uint8_t timestatus)
{
//    UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s,startHourMin=0x%02x,endHourMin=0x%02x", __func__, uteModuleNotDisturbData.startHourMin, uteModuleNotDisturbData.endHourMin)
    if (timestatus == NOT_DISTURB_START_TIME)
    {
        return (uteModuleNotDisturbData.startHourMin);
    }
    else
    {
        return (uteModuleNotDisturbData.endHourMin);
    }
}
/**
*@brief  是否打开定时勿扰
*@details
*@return
*@author        raymond
*@date        2021-08-27
*/
bool uteModuleNotDisturbIsOpenScheduled(void)
{
    return uteModuleNotDisturbData.isScheduledOpen;
}
/**
*@brief  设置定时勿扰
*@details
*@return
*@author        raymond
*@date        2021-08-27
*/
void uteModuleNotDisturbSetScheduled(bool isopen)
{
    uteModulePlatformTakeMutex(uteModuleNotDisturbMute);
    uteModuleNotDisturbData.isScheduledOpen = isopen;
    uteModuleNotDisturbSaveLocalParam();
    uteModulePlatformGiveMutex(uteModuleNotDisturbMute);
}
/**
*@brief  设置勿扰状态
*@details
*@return
*@author        raymond
*@date        2021-08-27
*/
void uteModuleNotDisturbSetOpenStatus(uint8_t openstatus)
{
    uteModuleNotDisturbOpenStatus = openstatus;
}
/**
*@brief  获取勿扰状态
*@details
*@return
*@author        raymond
*@date        2021-08-27
*/
uint8_t uteModuleNotDisturbGetOpenStatus(void)
{
    return uteModuleNotDisturbOpenStatus;
}

/**
*@brief        定时勿扰开关
*@details
*@author       raymond
*@date       2022-03-01
*/
void uteModuleNotDisturbScheduledSwitch(void)
{
    bool isopen = false;
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,isNotDisturb=%d,IsOpenScheduled=%d",__func__,quick.isNotDisturb,uteModuleNotDisturbIsOpenScheduled());
    if (!uteModuleNotDisturbIsOpenScheduled())
    {
        isopen = true;
        uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_SCHEDULED_OPEN);
        if (quick.isNotDisturb)
        {
            quick.isNotDisturb = false;
            uteApplicationCommonSetQuickSwitchStatus(&quick);
        }
    }
    else
    {
        uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_CLOSE);
    }
    uteModuleNotDisturbSetScheduled(isopen);
}

/**
*@brief        勿扰全天开关
*@details
*@author       raymond
*@date       2022-03-01
*/
void uteModuleNotDisturbAllDaySwitch(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,isNotDisturb=%d",__func__,quick.isNotDisturb);
    quick.isNotDisturb = !quick.isNotDisturb;
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    if (quick.isNotDisturb)
    {
        uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_ALLDAY_OPEN);
        uteModuleLiftWristRecordOpenStatus();
        if (uteModuleLiftWristGetOldOpenStatus())
        {
            uteModuleLiftWristSetOpenStatus(false);
        }
        else if (uteModuleLiftWristGetOldScheduledOpenStatus())
        {
            uteModuleLiftWristSetScheduled(false);
        }
        if (uteModuleNotDisturbIsOpenScheduled())
        {
            uteModuleNotDisturbSetScheduled(false);
        }
    }
    else
    {
        uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_CLOSE);
        if (uteModuleLiftWristGetOldOpenStatus())
        {
            uteModuleLiftWristSetOpenStatus(true);
        }
        else if (uteModuleLiftWristGetOldScheduledOpenStatus())
        {
            uteModuleLiftWristSetScheduled(true);
        }
    }
}
#endif
