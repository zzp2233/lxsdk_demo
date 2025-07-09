/**
*@file
*@brief        抬腕亮屏模块
*@details
*@author       raymond
*@date       2022-03-03
*@version      v1.0
*/

#include "ute_module_liftwrist.h"
#include "ute_module_log.h"
#include "ute_application_common.h"

void *uteModuleLiftWristMute;

ute_module_lift_wrist_data_t uteModuleliftWristData;
static uint8_t uteModuleliftWristTimeStatus;
static bool uteModuleliftWristOpenOldStatus;
static bool uteModuleliftWristScheduledOpenOldStatus;
static uint8_t uteModuleliftWristOpenNowStatus;

/**
*@brief  读取勿扰参数信息
*@details
*@author        zn.zeng
*@date        2021-08-27
*/
void uteModuleliftWristReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[8] = {0};
    readbuff[0] = DEFAULT_LIFT_WRIST_OPNE;
    readbuff[1] = (DEFAULT_LIFT_WRIST_START_HOUR_MIN_VALUE>>8)&0xff;
    readbuff[2] = DEFAULT_LIFT_WRIST_START_HOUR_MIN_VALUE&0xff;
    readbuff[3] = (DEFAULT_LIFT_WRIST_END_HOUR_MIN_VALUE>>8)&0xff;
    readbuff[4] = (DEFAULT_LIFT_WRIST_END_HOUR_MIN_VALUE)&0xff;
    readbuff[5] = DEFAULT_LIFT_WRIST_SCHEDULED_OPNE;

    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_LIFTWRISTINFO, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file,&readbuff[0],8);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleliftWristData.isOpen = readbuff[0];
    uteModuleliftWristData.startHourMin = readbuff[1] << 8 | readbuff[2];
    uteModuleliftWristData.endHourMin = readbuff[3] << 8 | readbuff[4];
    uteModuleliftWristData.isScheduledOpen = readbuff[5];

    // ute_quick_switch_t quick;
    // uteApplicationCommonGetQuickSwitchStatus(&quick);
    // quick.isLiftWrist = uteModuleliftWristData.isOpen;
    // quick.isRejectCall = uteModuleliftWristData.isRejectCall;
    // uteApplicationCommonSetQuickSwitchStatus(quick);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isOpen=%,startHourMin=%d,endHourMin=%d", __func__, uteModuleliftWristData.isOpen, uteModuleliftWristData.startHourMin, uteModuleliftWristData.endHourMin);
}
/**
*@brief  抬腕亮屏参数初始化
*@details
*@author        raymond
*@date        2022-03-03
*/
void uteModuleLiftWristInit(void)
{
    uteModuleliftWristReadConfig();
    uteModulePlatformCreateMutex(&uteModuleLiftWristMute);
}
/**
*@brief  保存抬腕亮屏信息
*@details
*@param[in] (ute_not_disturb_param_t param)
*@author        zn.zeng
*@date        2021-08-27
*/
void uteModuleLiftWristSaveParam(void)
{
//    memcpy(&uteModuleliftWristData, &param, sizeof(ute_module_lift_wrist_data_t));
    /*! 保存到文件raymond 2022-03-03*/
    void *file;
    uint8_t writebuff[8];
    writebuff[0] = uteModuleliftWristData.isOpen;
    writebuff[1] = (uteModuleliftWristData.startHourMin >> 8) & 0xff;
    writebuff[2] = (uteModuleliftWristData.startHourMin) & 0xff;
    writebuff[3] = (uteModuleliftWristData.endHourMin >> 8) & 0xff;
    writebuff[4] = (uteModuleliftWristData.endHourMin) & 0xff;
    writebuff[5] = uteModuleliftWristData.isScheduledOpen;

    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_LIFTWRISTINFO, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],8);
        uteModuleFilesystemCloseFile(file);
    }
    // ute_quick_switch_t quick;
    // uteApplicationCommonGetQuickSwitchStatus(&quick);
    // quick.isLiftWrist = uteModuleliftWristData.isOpen;
    // quick.isRejectCall = uteModuleliftWristData.isRejectCall;
    // uteApplicationCommonSetQuickSwitchStatus(quick);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isOpen=%d,startHourMin=0x%02x,endHourMin=0x%02x", __func__, uteModuleliftWristData.isOpen, uteModuleliftWristData.startHourMin, uteModuleliftWristData.endHourMin);
}

/**
*@brief  读取当前是否是抬腕亮屏时间段
*@details
*@return true ,当前在抬腕亮屏时间段
*@author        raymond
*@date        2022-03-03
*/
bool uteModuleLiftWristIsTimeBucket(void)
{
    bool isLiftWristTimeNow = false;
    if(uteModuleliftWristData.isScheduledOpen)
    {
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        uint16_t hourMin = time.hour<<8|time.min;
        if(uteModuleliftWristData.startHourMin<uteModuleliftWristData.endHourMin)
        {
            if((hourMin>=uteModuleliftWristData.startHourMin)&&(hourMin<=uteModuleliftWristData.endHourMin))
            {
                isLiftWristTimeNow = true;
            }
        }
        else
        {
            if((hourMin>=uteModuleliftWristData.startHourMin)||(hourMin<=uteModuleliftWristData.endHourMin))
            {
                isLiftWristTimeNow = true;
            }
        }
    }
    return isLiftWristTimeNow;
}
/**
*@brief  设置时间状态
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
void uteModuleLiftWristSetTimeStatus(uint8_t timestatus)
{
    uteModuleliftWristTimeStatus = timestatus;
    UTE_MODULE_LOG(1, "%s,startHourMin=0x%02x,endHourMin=0x%02x",__func__, uteModuleliftWristData.startHourMin, uteModuleliftWristData.endHourMin);
}

/**
*@brief  获取时间状态
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
uint8_t uteModuleLiftWristGetTimeStatus(void)
{
    return uteModuleliftWristTimeStatus;
}

/**
*@brief  设置开始或结束时间
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
void uteModuleLiftWristSetTime(uint16_t LiftWristHourMin)
{
    uteModulePlatformTakeMutex(uteModuleLiftWristMute);
    if (uteModuleliftWristTimeStatus == LIFT_WRIST_START_TIME)
    {
        uteModuleliftWristData.startHourMin = LiftWristHourMin;
    }
    else
    {
        uteModuleliftWristData.endHourMin = LiftWristHourMin;
    }
    uteModuleLiftWristSaveParam();
    uteModulePlatformGiveMutex(uteModuleLiftWristMute);
    UTE_MODULE_LOG(1, "timeStatus = %d,startHourMin=0x%02x,endHourMin=0x%02x", uteModuleliftWristTimeStatus, uteModuleliftWristData.startHourMin, uteModuleliftWristData.endHourMin);
}

/**
*@brief  获取开始或结束时间
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
uint16_t uteModuleLiftWristGetTime(uint8_t timestatus)
{
//    UTE_MODULE_LOG(1, "%s,startHourMin=0x%02x,endHourMin=0x%02x", __func__, uteModuleliftWristData.startHourMin, uteModuleliftWristData.endHourMin)
    if (timestatus == LIFT_WRIST_START_TIME)
    {
        return (uteModuleliftWristData.startHourMin);
    }
    else
    {
        return (uteModuleliftWristData.endHourMin);
    }
}


/**
*@brief  是否打开抬腕亮屏
*@details
*@return
*@author        raymond
*@date        2022-03-14
*/
bool uteModuleLiftWristIsOpen(void)
{
    return uteModuleliftWristData.isOpen;
}
/**
*@brief  设置抬腕亮屏
*@details
*@return
*@author        raymond
*@date        2022-03-14
*/
void uteModuleLiftWristSetOpenStatus(bool isopen)
{
    uteModulePlatformTakeMutex(uteModuleLiftWristMute);
    uteModuleliftWristData.isOpen = isopen;
    uteModuleLiftWristSaveParam();
    uteModulePlatformGiveMutex(uteModuleLiftWristMute);
}
/**
*@brief  是否打开定时抬腕亮屏
*@details
*@return
*@author        raymond
*@date        2022-03-14
*/
bool uteModuleLiftWristIsOpenScheduled(void)
{
    return uteModuleliftWristData.isScheduledOpen;
}
/**
*@brief  设置定时抬腕亮屏
*@details
*@return
*@author        raymond
*@date        2022-03-14
*/
void uteModuleLiftWristSetScheduled(bool isopen)
{
    uteModulePlatformTakeMutex(uteModuleLiftWristMute);
    uteModuleliftWristData.isScheduledOpen = isopen;
    uteModuleLiftWristSaveParam();
    uteModulePlatformGiveMutex(uteModuleLiftWristMute);
}

/**
*@brief  记录开关状态
*@details
*@return
*@author        raymond
*@date        2022-03-14
*/
void uteModuleLiftWristRecordOpenStatus(void)
{
    if(uteModuleliftWristData.isOpen)
    {
        uteModuleliftWristOpenOldStatus = true;
    }
    else
    {
        uteModuleliftWristOpenOldStatus = false;
    }
    if(uteModuleliftWristData.isScheduledOpen)
    {
        uteModuleliftWristScheduledOpenOldStatus = true;
    }
    else
    {
        uteModuleliftWristScheduledOpenOldStatus = false;
    }
}
/**
*@brief  获取旧开关状态
*@details
*@return
*@author        raymond
*@date        2022-03-14
*/
bool uteModuleLiftWristGetOldOpenStatus(void)
{
    return uteModuleliftWristOpenOldStatus;
}
/**
*@brief  获取旧定时开关状态
*@details
*@return
*@author        raymond
*@date        2022-03-14
*/
bool uteModuleLiftWristGetOldScheduledOpenStatus(void)
{
    return uteModuleliftWristScheduledOpenOldStatus;
}
/**
*@brief  设置开关状态
*@details
*@return
*@author        raymond
*@date        2022-03-14
*/
void uteModuleLiftWristSetNowOpenStatus(uint8_t openstatus)
{
    uteModuleliftWristOpenNowStatus = openstatus;
}
/**
*@brief  获取开关状态
*@details
*@return
*@author        raymond
*@date        2022-03-14
*/
uint8_t uteModuleLiftWristGetNowOpenStatus(void)
{
    return uteModuleliftWristOpenNowStatus;
}
