/**
*@file
*@brief        系统时间模块
*@details  系统时间，闹钟接口，UTC时间等接口
*@author       zn.zeng
*@date       Jun 29, 2021
*@version      v1.0
*/

#include "ute_module_systemtime.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_drv_battery_common.h"
// #include "ute_drv_keys_common.h"
#include "ute_module_message.h"
#include "ute_project_config.h"
#include "ute_module_sleep.h"
#include "ute_module_sport.h"
#include "ute_module_heart.h"
#include "ute_drv_motor.h"
#include "ute_language_common.h"
// #include "ute_module_localRingtone.h"
// #include "bt_hfp.h"
#include "ute_module_filesystem.h"
//#include "time.h"

/*! 系统时间参数 */
ute_module_systemtime_time_t systemTime;
/*! 系统闹钟参数 */
static ute_module_systemtime_alarm_t systemAlarms;
/*! 注册每秒回调函数数据结构zn.zeng, 2021-07-12  */
ute_module_systemtime_register_t systemTimeRegisterData;
static uint8_t uteModuleSystemTimeLocalTimeStatus;
/*! 平年每月天数 */
const static uint8_t everyMonDays[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/*! 闰年每月天数 */
const static uint8_t leapEveryMonDays[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/* 时间互斥量 zn.zeng 2022-02-14*/
void *uteModuleSystemtimeMute;

/**
*@brief  系统时间初始化
*@details
*@author        zn.zeng
*@date        Jun 29, 2021
*/

void uteModuleSystemtimeInit(void)
{
    memset(&systemAlarms, 0, sizeof(ute_module_systemtime_alarm_t));
    systemAlarms.isRemindingIndex = 0xff;

    uteModulePlatformCreateMutex(&uteModuleSystemtimeMute);
    uteModulePlatformRtcInit();
    uteModulePlatformRtcStart();
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_ALARMINFO_DIR);
    uteModuleSystemtimeReadConfig();
    memset(&systemTimeRegisterData,0,sizeof(ute_module_systemtime_register_t));
    uteLanguageCommonSelect(systemTime.languageId);
}

/**
*@brief  读取时间参数
*@details
*@author        zn.zeng
*@date        2021-08-26
*/
void uteModuleSystemtimeReadConfig(void)
{
    /*! 闹钟参数读取zn.zeng, 2021-08-21  */
    void *file=NULL;
    uint8_t readbuff[15];
    memset(&readbuff[0],0,15);
    char path[20];
    memset(&path[0],0,20);
    systemAlarms.alarmTotalCnt = 0;
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
    readbuff[11] = ALARM_REPEAT_REMIND_DEFAULT_OPEN;
    readbuff[12] = 0;
    readbuff[13] = 0;
    readbuff[14] = ALARM_REPEAT_REMIND_DEFAULT_TIMES;
#endif
    for(uint8_t i=0; i<SYSTEM_TIME_ALARMS_MAX_CNT; i++)
    {
        sprintf((char *)&path[0],"%s/%02d",UTE_MODULE_FILESYSTEM_ALARMINFO_DIR,i);
        memset(&systemAlarms.alarmParam[i],0,sizeof(ute_module_systemtime_one_alarm_t));
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&readbuff[0],15);
            uteModuleFilesystemCloseFile(file);
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].year = readbuff[0]<<8|readbuff[1];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].month = readbuff[2];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].day = readbuff[3];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].weekDay = readbuff[4];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].hour = readbuff[5];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].min = readbuff[6];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].sec = readbuff[7];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].isSingle = readbuff[8];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].durationTimeSec = readbuff[9];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].isOpen = readbuff[10];
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].isRepeatRemindOpen = readbuff[11];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].repeatRemindHour = readbuff[12];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].repeatRemindMin = readbuff[13];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].repeatRemindTimes = readbuff[14];
#endif
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].isFinish = true;
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,i=%d.weekDay = 0x%02x,.hour = %d,.min = %d,.isOpen=%d", __func__,i,systemAlarms.alarmParam[i].weekDay,systemAlarms.alarmParam[i].hour,systemAlarms.alarmParam[i].min,systemAlarms.alarmParam[i].isOpen);
            systemAlarms.alarmTotalCnt++;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,readAlarmTotalCnt = %d", __func__,systemAlarms.alarmTotalCnt);

    /*! 时间格式，当设置语言 zn.zeng, 2021-08-18  */
    readbuff[0] = DEFAULT_SYSTEM_TIME_FORMAT_MI;
    readbuff[1] = DEFAULT_SYSTEM_TIME_FORMAT_12HOUR;
    readbuff[2] = DEFAULT_SYSTEM_TIME_ZONE;
    readbuff[3] = DEFAULT_LANGUAGE>>8&0xff;
    readbuff[4] = DEFAULT_LANGUAGE&0xff;

    readbuff[5] = (DEFAULT_SYSTEM_TIME_YEAR>>8)&0xff;
    readbuff[6] = DEFAULT_SYSTEM_TIME_YEAR&0xff;
    readbuff[7] = DEFAULT_SYSTEM_TIME_MONTH;
    readbuff[8] = DEFAULT_SYSTEM_TIME_DAY;
    readbuff[9] = DEFAULT_SYSTEM_TIME_HOUR;
    readbuff[10] = DEFAULT_SYSTEM_TIME_MIN;
    readbuff[11] = DEFAULT_SYSTEM_TIME_SEC;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_TIME_FORMAT,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],14);
        uteModuleFilesystemCloseFile(file);
    }
    ute_module_systemtime_time_t set;
    memset(&set,0,sizeof(ute_module_systemtime_time_t));
    set.isDistanceMi = readbuff[0];
    set.is12HourTime = readbuff[1];
    set.zone = readbuff[2];
    set.languageId = readbuff[3]<<8|readbuff[4];

    // rtc_clock_calc_by_rc(1);
    tm_t rtc_tm;
    rtc_tm.year = readbuff[5] << 8 | readbuff[6];
    rtc_tm.mon = readbuff[7];
    rtc_tm.day = readbuff[8];
    rtc_tm.hour = readbuff[9];
    rtc_tm.min = readbuff[10];
    rtc_tm.sec = readbuff[11];

    if (tm_to_time(rtc_tm) < RTCCNT)
    {
        rtc_tm = time_to_tm(RTCCNT);
    }

    set.year = rtc_tm.year;
    set.month = rtc_tm.mon;
    set.day = rtc_tm.day;
    set.hour = rtc_tm.hour;
    set.min = rtc_tm.min;
    set.sec = rtc_tm.sec;

    set.isWatchSetLangage = readbuff[12];
    set.AppSetlanguageId = readbuff[13];
    uteModuleSystemtimeSetTime(set);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,isDistanceMi=%d,is12HourTime=%d,time.zone=%d,languageId=%d", __func__,systemTime.isDistanceMi,systemTime.is12HourTime,systemTime.zone,systemTime.languageId);
}
/**
*@brief  系统时间设置时间
*@details
*@param[in] ute_module_systemtime_time_t set  传入要设置的时间参数
*@author        zn.zeng
*@date        Jun 29, 2021
*/

void uteModuleSystemtimeSetTime(ute_module_systemtime_time_t set)
{
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,%04d-%02d-%02d,%02d:%02d:%02d,zone=%d", __func__, set.year, set.month, set.day, set.hour, set.min, set.sec,set.zone);
    if (set.year < 2000)
    {
        set.year = 2000;
    }
    if (set.month == 0)
    {
        set.month = 1;
    }
    if (set.day == 0)
    {
        set.day = 1;
    }

    ute_module_systemtime_time_t oldTime;
    memcpy(&oldTime,&systemTime,sizeof(ute_module_systemtime_time_t));
#if UTE_MODULE_SLEEP
    uteModuleSleepSystemtimeChange(systemTime,set);
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    //sportSystemTimeChange 中有uteModuleCwmReadCurrDayStepFromFs 操作,所以这个放在它执行之前
    uteModuleCwmStepDataSystemtimeChange(systemTime,set);
#endif
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportSystemtimeChange(systemTime,set);
#endif
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(&systemTime, &set, sizeof(ute_module_systemtime_time_t));
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
    systemTime.isSettingTime = true;
    systemTime.week = uteModuleSystemtimeGetWeek(systemTime.year, systemTime.month, systemTime.day);
    uteModulePlatformRtcSetTime(systemTime.year,systemTime.month,systemTime.day,systemTime.hour,systemTime.min,systemTime.sec);
    uteModuleSystemtimeSaveTimeInfo();
//    uteModulePlaformUpdateConnectParam(12,36,30*1000);
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    if(uteModuleGuiCommonIsInScreenSaver())
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_TIME_MIN_REGISER,NULL);
    }
#endif
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartSystemtimeChange(oldTime,set);
#endif
}
/**
*@brief  判断是否是闰年
*@details
*@param[in] uint16_t year  传入年份
*@author        zn.zeng
*@return  返回true，此年份是闰年
*@date        Jun 29, 2021
*/
static bool uteModuleSystemtimeIsLeapYear(uint16_t year)
{
    return (((year % 100 != 0) && (year % 4 == 0)) || (year % 400 == 0));
}
/**
*@brief  系统时间秒计时
*@details   秒加一
*@param[in] ute_module_systemtime_time_t *time  时间变量指针
*@author        zn.zeng
*@date        Jun 29, 2021
*/
static void uteModuleSystemtimeChange(ute_module_systemtime_time_t *time)
{
#if UTE_MODULE_CREATE_SYS_1S_TIMER_SUPPORT
    time->sec++;
    if (time->sec > 59)
    {
        time->sec = 0;
        time->min++;
        if (time->min > 59)
        {
            time->min = 0;
            time->hour++;
            if (time->hour > 23)
            {
                const uint8_t *monday;
                time->hour = 0;
                time->day++;
                if (uteModuleSystemtimeIsLeapYear(time->year))
                {
                    monday = &leapEveryMonDays[0];
                }
                else
                {
                    monday = &everyMonDays[0];
                }
                if (time->day > monday[time->month])
                {
                    time->day = 1;
                    time->month++;
                    if (time->month > 12)
                    {
                        time->month = 1;
                        time->year = time->year + 1;
                    }
                }
            }
        }
    }
#else
    bool lowpwr_sta = bsp_system_is_sleep();
    static u8 rtc_cal_cnt_bkp = 0;
    static bool lowpwr_sta_bkp = false;
    uint32_t rtc_cnt = 0;
    tm_t rtc_tm;
    bool allow_calibration = (time->sec % 10 != 0 && time->sec < 55 && time->sec > 5); //避免校准时间跳过关键时间点
    rtc_tm.year = time->year;
    rtc_tm.mon = time->month;
    rtc_tm.day = time->day;
    rtc_tm.hour = time->hour;
    rtc_tm.min = time->min;
    rtc_tm.sec = time->sec;
    rtc_cnt = tm_to_time(rtc_tm);
    if (!lowpwr_sta && allow_calibration)
    {
        rtc_cnt = RTCCNT;
    }
    else // 省电/休眠模式，RTC已休眠
    {
        if (lowpwr_sta_bkp == false) // 初次进入
        {
            rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
        }
        if (rtc_cal_cnt_bkp != sys_cb.rtc_cal_cnt && allow_calibration) // RTC已校准，同步校准
        {
            rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
            rtc_cnt = RTCCNT;
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,RTC calibrated,rtc_cal_cnt_bkp:%d,sys_cb.rtc_cal_cnt:%d", __func__, rtc_cal_cnt_bkp, sys_cb.rtc_cal_cnt);
        }
        else
        {
            rtc_cnt++;
        }
    }
    rtc_tm = time_to_tm(rtc_cnt);
    time->year = rtc_tm.year;
    time->month = rtc_tm.mon;
    time->day = rtc_tm.day;
    time->hour = rtc_tm.hour;
    time->min = rtc_tm.min;
    time->sec = rtc_tm.sec;
#endif
}
/**
*@brief  系统时间每秒处理函数
*@details  系统时间每秒处理总入口
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModuleSystemtimeSecondCb(void)
{
    if (!sys_cb.sys_init_complete)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,sys_init_complete:%d",__func__,sys_cb.sys_init_complete);
        return;
    }
    if (uteApplicationCommonIsStartupFinish())
    {
        uteModuleSystemtimeChange(&systemTime);
        systemTime.week = uteModuleSystemtimeGetWeek(systemTime.year,systemTime.month,systemTime.day);
        if(systemTime.isSettingTime)
        {
            systemTime.isSettingTime = false;
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL,"%s,now is setting time",__func__);
        }
#if 0//(UTE_LOG_TIME_LVL && UTE_MODULE_LOG_SUPPORT)
        else
        {
            uteModulePlatformCalibrationSystemTimer();
        }
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,%04d-%02d-%02d %02d:%02d:%02d", __func__, systemTime.year, systemTime.month, systemTime.day, systemTime.hour, systemTime.min, systemTime.sec);
#endif
        if (systemTime.sec == 0)
        {
            uteModuleSystemtimeSaveTimeInfo();
        }
        for (uint8_t i = 0; i < systemTimeRegisterData.regCnt; i++)
        {
            uteModuleSystemtimeRegisterSecondCb(i);
        }
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,uteApplicationCommonStartupSecond",__func__);
        /*! 启动流程 读取电池电压 zn.zeng  modify Jul 01, 2021 */
        uteDrvBatteryCommonUpdateBatteryInfo();
        if (uteDrvBatteryCommonGetVoltage() > UTE_DRV_BATTERY_POWER_ON_VOLTAGE)
        {
            uteApplicationCommonStartupSecond();
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,!!! battery voltage:%d", __func__, uteDrvBatteryCommonGetVoltage());
        }
    }
}
/**
*@brief  获取week
*@details   传入年月日，计算星期
*@param[in] uint16_t year
*@param[in] uint16_t month
*@param[in] uint16_t day
*@return  返回星期，星期天为0
*@author        zn.zeng
*@date        Jun 29, 2021
*/
uint8_t uteModuleSystemtimeGetWeek(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t week;  //代表星期几
    if (month == 1 || month == 2)
    {
        month += 12;
        year--;
    }
    week = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    return week;
}

/**
*@brief  获取系统时间
*@details
*@param[out] ute_module_systemtime_time_t *time  时间变量指针
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModuleSystemtimeGetTime(ute_module_systemtime_time_t *time)
{
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(time, &systemTime, sizeof(ute_module_systemtime_time_t));
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}
/**
*@brief
*@details 计算增加天数的日期
*@param[out] ute_module_systemtime_time_t *time  时间变量指针
*@param[in] uint8_t addDay 增加的具体天数
*@author        zn.zeng
*@date        Jun 30, 2021
*/
void uteModuleSystemtimeInputDateCalDay(ute_module_systemtime_time_t *time, uint16_t addDay)
{
    for (int i = 0; i < addDay; i++)
    {
        const uint8_t *monday;
        time->hour = 0;
        time->day++;
        if (uteModuleSystemtimeIsLeapYear(time->year))
        {
            monday = &leapEveryMonDays[0];
        }
        else
        {
            monday = &everyMonDays[0];
        }
        if (time->day > monday[time->month])
        {
            time->day = 1;
            time->month++;
            if (time->month > 12)
            {
                time->month = 1;
                time->year = time->year + 1;
            }
        }
    }
}
/**
*@brief
*@details 计算增加减少天数的日期
*@param[out] ute_module_systemtime_time_t *time  时间变量指针
*@param[in] int  增加减少的具体天数，正为增加，负为减少，0为不操作
*@author        zn.zeng
*@date        2022-08-19
*/
void uteModuleSystemtimeInputDateIncreaseDecreaseDay(ute_module_systemtime_time_t *time, int day)
{
    /*
    struct tm timeptr;
    struct tm *timeout;
    time_t timestamp = 0;
    timeptr.tm_year = time->year - 1900;
    timeptr.tm_mon = systemTime.month - 1;
    timeptr.tm_mday = systemTime.day;
    timeptr.tm_hour = 0;
    timeptr.tm_min = 0;
    timeptr.tm_sec = 0;
    time_t tmp = mktime(&timeptr);
    timestamp = tmp + day * 86400;
    timeout = localtime(&timestamp);
    time->year = timeout->tm_year + 1900;
    time->month = timeout->tm_mon + 1;
    time->day = timeout->tm_mday;
    time->week = timeout->tm_wday;
    */
}
/**
*@brief 注册系统每秒时间函数
*@details
*@param[in] void* function 函数指针
*@author        zn.zeng
*@date        2021-07-12
*/
void uteModuleSystemtimeRegisterSecond(void* function)
{
    for(uint8_t i = 0; i<UTE_MODULE_SYSTEMTEIM_REGISTER_MAX_CNT; i++)
    {
        if(systemTimeRegisterData.function[i]==NULL)
        {
            systemTimeRegisterData.function[i] = (ute_module_systemtime_reg_func_t)function;
            systemTimeRegisterData.regCnt++;
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,systemTimeRegisterData.regCnt=%d,.function[i]=%p", __func__, systemTimeRegisterData.regCnt,systemTimeRegisterData.function[i]);
            return;
        }
        else if(systemTimeRegisterData.function[i]==(ute_module_systemtime_reg_func_t)function)
        {
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,has register function=0x%p", __func__, function);
            return;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,systemTimeRegisterData.regCnt is max", __func__);
}
/**
*@brief 注册系统每秒时间回调函数
*@details
*@param[in] uint32_t param 参数
*@author        zn.zeng
*@date        2021-07-12
*/
void uteModuleSystemtimeRegisterSecondCb(uint32_t param)
{
    if(param>(UTE_MODULE_SYSTEMTEIM_REGISTER_MAX_CNT-1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,param is too lager", __func__);
    }
    if(systemTimeRegisterData.function[param]!=NULL)
    {
//        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,function[%d]=%p", __func__,param,systemTimeRegisterData.function[param]);
        systemTimeRegisterData.function[param]();
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,param is error,function is NULL", __func__);
    }
}
/**
*@brief 注册系统每分钟时间回调函数
*@details
*@param[in] uint32_t param 参数
*@author        cxd
*@date        2022-04-26
*/
void uteModuleSystemtimeRegisterMinuteCb(void)
{
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s", __func__);
//    if(!uteApplicationCommonIsPowerOn())
//    {
//        return;
//    }
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    if(uteModuleGuiCommonIsInScreenSaver())
    {
        // uteModulePlatformDlpsDisable(UTE_MODULE_PLATFORM_DLPS_BIT_SCREEN);
        // uteDrvScreenCommonInterfaceInit();
        // uteModulePlatformDelayMs(10);
        uteModuleGuiCommonAnimationTimerCallback(NULL);
        // uteModulePlatformDelayMs(100);
        // uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_SCREEN);
    }
#endif
}
/**
*@brief  获取是否晚上时间
*@details
*@return true为晚上时间定义
*@author        zn.zeng
*@date        2021-08-05
*/
bool uteModuleSystemtimeIsNight(void)
{
    uint32_t oneDaySecond = systemTime.hour*3600+systemTime.min*60+systemTime.sec;
    if((oneDaySecond<28800||oneDaySecond>64800))  // 18:00~8:00
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief  获取是否睡眠时间
*@details
*@return true 为睡眠时间
*@author        zn.zeng
*@date        2021-08-05
*/
bool uteModuleSystemtimeIsSleepTime(void)
{
    uint32_t oneDaySecond = systemTime.hour*3600+systemTime.min*60+systemTime.sec;
    if((oneDaySecond<43200||oneDaySecond>64800))  // 18:00~12:00
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief    保存时间信息
*@details
*@author        zn.zeng
*@date        2021-11-08
*/
void uteModuleSystemtimeSaveTimeInfo(void)
{
    /*! 保存到文件zn.zeng, 2021-08-18  */
    void *file;
    uint8_t writebuff[14];
    writebuff[0] = systemTime.isDistanceMi;
    writebuff[1] = systemTime.is12HourTime;
    writebuff[2] = systemTime.zone;
    writebuff[3] = (systemTime.languageId>>8)&0xff;
    writebuff[4] = systemTime.languageId&0xff;
    writebuff[5] = (systemTime.year>>8)&0xff;
    writebuff[6] = systemTime.year&0xff;
    writebuff[7] = systemTime.month;
    writebuff[8] = systemTime.day;
    writebuff[9] = systemTime.hour;
    writebuff[10] = systemTime.min;
    writebuff[11] = systemTime.sec;
    writebuff[12] = systemTime.isWatchSetLangage;
    writebuff[13] = systemTime.AppSetlanguageId;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,isDistanceMi=%d,is12HourTime=%d,time.zone=%d,languageId=%d", __func__,systemTime.isDistanceMi,systemTime.is12HourTime,systemTime.zone,systemTime.languageId);
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_TIME_FORMAT,&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],14);
        uteModuleFilesystemCloseFile(file);
    }

}
/**
*@brief    设置显示时间格式
*@details   包括距离的单位，英里和公里的切换，并保存
*@author        zn.zeng
*@date        2021-08-18
*/
void uteModuleSystemtimeSetTimeFormat(bool is12Hour,bool isMi)
{
    if(is12Hour != systemTime.is12HourTime)
    {
        systemTime.change12HourTime = true;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言消息更新界面
    }
    systemTime.isDistanceMi = isMi;
    systemTime.is12HourTime = is12Hour;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,isDistanceMi=%d,is12HourTime=%d", __func__,systemTime.isDistanceMi,systemTime.is12HourTime);
    uteModuleSystemtimeSaveTimeInfo();
}
/**
*@brief  保存时区
*@details
*@param[in] (int8_t zone)
*@author        zn.zeng
*@date        2021-08-18
*/
void uteModuleSystemtimeSetTimeZone(int8_t zone)
{
    systemTime.zone=zone;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,zone=%d", __func__,systemTime.zone);
    uteModuleSystemtimeSaveTimeInfo();
}
/**
*@brief  保存语言
*@details
*@param[in] (uint8_t id)
*@author        zn.zeng
*@date        2021-08-23
*/

void uteModuleSystemtimeSetLanguage(uint16_t id)
{
    systemTime.languageId=id;
    systemTime.isWatchSetLangage = true;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,languageId=%d", __func__,systemTime.languageId);
    uteModuleSystemtimeSaveTimeInfo();
    uteLanguageCommonSelect(systemTime.languageId);
    msg_enqueue(MSG_CHECK_LANGUAGE);
}
/**
*@brief  保存APP设计的语言
*@details
*@param[in] (uint8_t id)
*@author        gyj
*@date        2023-09-7
*/

void uteModuleSystemtimeAPPSetLanguage(uint16_t id)
{
    systemTime.AppSetlanguageId = id;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,AppSetlanguageId=%d", __func__,systemTime.languageId);
    uteModuleSystemtimeSaveTimeInfo();
}
/**
*@brief  读取当前设置语言
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
uint16_t uteModuleSystemtimeReadLanguage(void)
{
    uint16_t id = 0;
    id = systemTime.languageId;
    return id;
}
/**
*@brief  对比当前设置语言
*@details
*@author        xjc
*@date        2022-01-22
*/
bool uteModuleSystemtimeCompareLanguage(uint16_t languageId)
{
    if(systemTime.languageId == languageId)
    {
        return true;
    }
    return false;
}
/**
*@brief  读取距离类型
*@details
*@author        dengli.lu
*@date        2021-10-26
*/
bool uteModuleSystemtimeGetDistanceMiType(void)
{
    return systemTime.isDistanceMi;
}
/**
*@brief  复位时间制切换状态
*@details
*@author        xjc
*@date        2022-08-26
*/
void uteModuleSystemtimeResetChange12HourTime(void)
{
    systemTime.change12HourTime = false;
}
/**
*@brief  读取时间制是否切换
*@details
*@author        xjc
*@date        2022-08-26
*/
bool uteModuleSystemtimeWhetherChange12HourTime(void)
{
    return systemTime.change12HourTime;
}
/**
*@brief  是否打开24H制
*@details
*@return
*@author        raymond
*@date        2021-08-27
*/
bool uteModuleSystemtime12HOn(void)
{
    return systemTime.is12HourTime;
}
/**
*@brief  设置24H制
*@details
*@return
*@author        raymond
*@date        2021-08-27
*/
void uteModuleSystemtime12HSwitchStatus(bool isopen)
{
    systemTime.is12HourTime = isopen;
}
/**
*@brief  设置时间和日期状态
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
void uteModuleSystemTimeLocalTimeSetStatus(uint8_t status)
{
    uteModuleSystemTimeLocalTimeStatus = status;
}

/**
*@brief  获取时间和日期状态
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
uint8_t uteModuleSystemTimeLocalTimeGetStatus(void)
{
    return uteModuleSystemTimeLocalTimeStatus;
}

/**
*@brief             设置日期
*@details
*@return
*@author      casen
*@date        2022-10-29
*/
void uteModuleSystemTimeLocalSetDate(uint16_t year,uint8_t month,uint8_t day)
{
    if(year<2000 || year>2099) return;
    if(month<1 || month >12) return;
    if(day<1) return;
    if (uteModuleSystemtimeIsLeapYear(year))
    {
        if(day>leapEveryMonDays[month])
        {
            return;
        }
    }
    else
    {
        if(day>everyMonDays[month])
        {
            return;
        }
    }
    ute_module_systemtime_time_t set;
    uteModuleSystemtimeGetTime(&set);
    set.year  = year;
    set.month = month;
    set.day     =   day;
    uteModuleSystemtimeSetTime(set);
}

/**
*@brief             设置时分
*@details
*@return
*@author      casen
*@date        2022-10-29
*/
void uteModuleSystemTimeLocalSetHourMin(uint8_t hour,uint8_t min)
{
    if(hour>23) return;
    if(min>59) return;
    ute_module_systemtime_time_t set;
    uteModuleSystemtimeGetTime(&set);
    set.hour  = hour;
    set.min = min;
    uteModuleSystemtimeSetTime(set);
}

/**
*@brief  设置闹钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        Jun 29, 2021
*/

void uteModuleSystemtimeSetAlarm(ute_module_systemtime_one_alarm_t set, uint8_t index)
{
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(&systemAlarms.alarmParam[index], &set, sizeof(ute_module_systemtime_one_alarm_t));
    uteModuleSystemtimeSaveAlarmInfo(systemAlarms.alarmParam[index],index);
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}

/**
*@brief  获取闹钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/
void uteModuleSystemtimeGetAlarm(ute_module_systemtime_one_alarm_t *set, uint8_t index)
{
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(set,&systemAlarms.alarmParam[index], sizeof(ute_module_systemtime_one_alarm_t));
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}


/**
*@brief  删除闹钟
*@details
*@param[in] uint8_t index 所需要删除的闹钟序号，从0开始
*@author       dengli.lu
*@date        2021-11-29
*/
void uteModuleSystemtimeDeleteAlarm(uint8_t index)
{
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    uint8_t beforeTotalCnt = systemAlarms.alarmTotalCnt;
    uint8_t path[20];
    memset(&path[0],0,20);
    sprintf((char *)&path[0],"%s/%02d",UTE_MODULE_FILESYSTEM_ALARMINFO_DIR,index);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index = %d,beforeTotalCnt=%d", __func__,index,beforeTotalCnt);
    memset(&systemAlarms.alarmParam[index],0,sizeof(ute_module_systemtime_one_alarm_t));
    systemAlarms.alarmParam[index].isFinish = false;
    uteModuleFilesystemDelFile((char *)&path[0]);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_ALARMINFO_DIR, dirInfo, NULL);
    if((index+1)<beforeTotalCnt)//删除最后一个，不需要重新命名
    {
        uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
        for(uint8_t i = index+1; i <beforeTotalCnt ; i++)
        {
            uint8_t oldPath[20],newPath[20];
            memset(&oldPath[0],0,20);
            memset(&newPath[0],0,20);
            sprintf((char *)&oldPath[0],"%s/%02d",UTE_MODULE_FILESYSTEM_ALARMINFO_DIR,i);
            sprintf((char *)&newPath[0],"%s/%02d",UTE_MODULE_FILESYSTEM_ALARMINFO_DIR,i-1);
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,oldPath=%s,newPath=%s", __func__,oldPath,newPath);
            uteModuleFilesystemRenameFile((char *)oldPath,(char *)newPath);
            memcpy(&systemAlarms.alarmParam[i-1],&systemAlarms.alarmParam[i],sizeof(ute_module_systemtime_one_alarm_t));
        }
        uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
    }
    systemAlarms.alarmTotalCnt = dirInfo->filesCnt;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,alarmTotalCnt = %d", __func__,systemAlarms.alarmTotalCnt);
#if (UTE_LOG_TIME_LVL&&UTE_MODULE_LOG_SUPPORT)
    for(uint8_t i = 0; i <systemAlarms.alarmTotalCnt ; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,hour=%d,min = %d", __func__,systemAlarms.alarmParam[i].hour,systemAlarms.alarmParam[i].min);
    }
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_ALARMINFO_DIR, dirInfo, NULL);
#endif
    uteModulePlatformMemoryFree(dirInfo);

}

/**
*@brief  保存闹钟参数
*@details
*@param[in](ute_module_systemtime_one_alarm_t value,uint8_t index)
*@author        zn.zeng
*@date        2021-08-21
*/
void uteModuleSystemtimeSaveAlarmInfo(ute_module_systemtime_one_alarm_t value,uint8_t index)
{
    /*! 保存到文件zn.zeng, 2021-08-21*/
    void *file;
    uint8_t writebuff[15];
    memset(&writebuff[0],0,15);
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    memcpy(&systemAlarms.alarmParam[index],&value,sizeof(ute_module_systemtime_one_alarm_t));
    writebuff[0] = systemAlarms.alarmParam[index].year>>8&0xff;
    writebuff[1] = systemAlarms.alarmParam[index].year&0xff;
    writebuff[2] = systemAlarms.alarmParam[index].month;
    writebuff[3] = systemAlarms.alarmParam[index].day;
    writebuff[4] = systemAlarms.alarmParam[index].weekDay;
    writebuff[5] = systemAlarms.alarmParam[index].hour;
    writebuff[6] = systemAlarms.alarmParam[index].min;
    writebuff[7] = systemAlarms.alarmParam[index].sec;
    writebuff[8] = systemAlarms.alarmParam[index].isSingle;
    writebuff[9] = systemAlarms.alarmParam[index].durationTimeSec;
    writebuff[10] = systemAlarms.alarmParam[index].isOpen;
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
    writebuff[11] = systemAlarms.alarmParam[index].isRepeatRemindOpen;
    writebuff[12] = systemAlarms.alarmParam[index].repeatRemindHour;
    writebuff[13] = systemAlarms.alarmParam[index].repeatRemindMin;
    writebuff[14] = systemAlarms.alarmParam[index].repeatRemindTimes;
#endif
    uint8_t path[20];
    memset(&path[0],0,20);
    sprintf((char *)&path[0],"%s/%02d",UTE_MODULE_FILESYSTEM_ALARMINFO_DIR,index);
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],15);
        uteModuleFilesystemCloseFile(file);
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,isSingle=%d,hour=%d,min=%d", __func__,value.isSingle,value.hour,value.min);
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index=%d,weekDay=0x%x,durationTimeSec=%d.isOpen=%d", __func__,index,value.weekDay,value.durationTimeSec,value.isOpen);
    }
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_ALARMINFO_DIR, dirInfo, NULL);
    systemAlarms.alarmTotalCnt = dirInfo->filesCnt;
    uteModulePlatformMemoryFree(dirInfo);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,alarmTotalCnt = %d", __func__,systemAlarms.alarmTotalCnt);
}

/**
*@brief  设置闹钟显示索引
*@details
*@param[in] uint8_t index 所需要设置的闹钟索引1~8
*@author       dengli.lu
*@date        2021-11-29
*/
void uteModuleSystemtimeSetAlarmDisplayIndex(uint8_t alarmDisplayIndex)
{
    systemAlarms.alarmDisplayIndex = alarmDisplayIndex;
}
/**
*@brief  获取闹钟显示索引
*@details
*@return  闹钟索引1~8
*@author       dengli.lu
*@date        2021-11-29
*/
uint8_t uteModuleSystemtimeGetAlarmDisplayIndex(void)
{
    return systemAlarms.alarmDisplayIndex;
}
/**
*@brief  设置当前闹钟数量
*@details
*@param[in] uint8_t index 所需要设置的闹钟数量1~8
*@author       dengli.lu
*@date        2021-11-29
*/
void uteModuleSystemtimeSetAlarmTotalCnt(uint8_t alarmTotalCnt)
{
    systemAlarms.alarmTotalCnt = alarmTotalCnt;
}
/**
*@brief  获取闹钟数量
*@details
*@return  闹钟索引1~8
*@author       dengli.lu
*@date        2021-11-29
*/
uint8_t uteModuleSystemtimeGetAlarmTotalCnt(void)
{
    return systemAlarms.alarmTotalCnt;
}


///适配中科平台接口（闹钟）

/**
*@brief  使能闹钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        Jun 29, 2021
*/

void uteModuleSystemtimeEnableAlarm(uint8_t index, bool en)
{
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    systemAlarms.alarmParam[index].isOpen = en;
    uteModuleSystemtimeSaveAlarmInfo(systemAlarms.alarmParam[index],index);
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}

/**
*@brief  获取闹钟设置的小时
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmHour(uint8_t index)
{
    uint8_t hour = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);
    hour = alarm->hour;
    ab_free(alarm);
    return hour;
}

/**
*@brief  获取闹钟设置的分钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmMin(uint8_t index)
{
    uint8_t min = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);
    min = alarm->min;
    ab_free(alarm);
    return min;
}

/**
*@brief  获取闹钟设置的秒
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmSec(uint8_t index)
{
    uint8_t sec = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);
    sec = alarm->sec;
    ab_free(alarm);
    return sec;
}

/**
*@brief  获取闹钟设置的提醒周期
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmCycle(uint8_t index)
{
    uint8_t ret = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);

    ret |= alarm->weekDay;

    if (alarm->isSingle)
    {
        ret |= BIT(7);
    }
    else
    {
        ret &= ~BIT(7);
    }

    ab_free(alarm);
    return ret;
}

/**
*@brief  获取闹钟是否使能
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmEnableState(uint8_t index)
{
    uint8_t isOpen = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);
    isOpen = alarm->isOpen;
    ab_free(alarm);
    return isOpen;
}

/**
*@brief  获取闹钟可以设置的最大数量
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmMaxCnt(void)
{
    return SYSTEM_TIME_ALARMS_MAX_CNT;
}

/**
*@brief  获取闹钟是否空闲
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/
uint8_t uteModuleSystemtimeGetAlarmIsFree(uint8_t index)
{
    return (index < uteModuleSystemtimeGetAlarmMaxCnt()) && (index >= uteModuleSystemtimeGetAlarmTotalCnt());
}

/**
*@brief  编辑闹钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/
uint8_t uteModuleSystemtimeAlarmEdit(uint8_t index, bool enable, uint8_t cycle, uint8_t alarm_hour, uint8_t alarm_minute, uint8_t motor_mode, uint8_t remind_later)
{

    ute_module_systemtime_one_alarm_t *set = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (set == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    set->isOpen = enable;
    set->weekDay = cycle & (~BIT(7));
    set->isSingle = (cycle & BIT(7)) ? true : false;
    set->hour = alarm_hour;
    set->min = alarm_minute;
    set->sec = 0;

//    uteModuleSystemtimeSetAlarmTotalCnt(uteModuleSystemtimeGetAlarmTotalCnt()+1);
    uteModuleSystemtimeSetAlarm(*set, index);
    ab_free(set);
    return 0;
}

uint8_t uteModuleSystemtimeGetAlarmRingIndex(void)
{
    return systemAlarms.isRemindingIndex;
}

void uteModuleSystemtimeSetAlarmRingIndex(uint8_t index)
{
    systemAlarms.isRemindingIndex = index;
}
