/**
*@file
*@brief     系统时间头文件
*@details
*@author        zn.zeng
*@date        Jun 29, 2021
*@version       v1.0
*/

#ifndef _UTE_MODULE_SYSTEMTIME_H_
#define _UTE_MODULE_SYSTEMTIME_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
/*!写入时间格式参数 zn.zeng, 2021-08-18  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_TIME_FORMAT  "systemparam/timeFormat"
/*!写入闹钟信息参数目录 zn.zeng, 2021-08-21  */
#define UTE_MODULE_FILESYSTEM_ALARMINFO_DIR "alarmInfo"

/**
 * @brief       系统时间数据结构
 * @details 系统时间数据结构
 */
typedef struct
{
    uint8_t month;/*!系统时间月份，数值为1~12月 */
    uint8_t day;/*!系统时间，一个月内的天，数值为1~31 */
    uint8_t hour;/*!系统时间，24小时格式的小时格式，数值为0~23 */
    uint8_t min;/*!系统时间，分钟，数值为0~59 */
    uint8_t sec;/*!系统时间，秒钟，数值为0~59 */
    uint8_t week;/*!系统时间，星期，数值为0~6，星期天开始算，星期天为0 */
    uint16_t year;/*!系统时间，年，数值为2000以上 */
    int8_t zone;/*!系统时间，当前时区，数值为   +-12 ，对应东12个时区，西12个时区，东时区为正，西时区为负*/
    bool is12HourTime;
    bool isDistanceMi;
    uint16_t languageId;
    uint16_t AppSetlanguageId;
    bool isSettingTime;
    bool isWatchSetLangage;/*手环端设置语言*/
    bool change12HourTime;
} ute_module_systemtime_time_t;
typedef void (*ute_module_systemtime_reg_func_t)(void);

/**
 * @brief       闹钟数据结构
 * @details 包括 年，月，日，时，分，秒，是否是重复闹钟，闹钟持续时长
 */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekDay;/*!有效星期，0x01，0x02，0x04，0x08，0x10，0x20，0x40，0x7f分别代表星期天，星期一，星期二，星期三，星期四，星期五，星期六以及每天（0x7f）有效 */
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    bool isSingle;/*!是否是单次闹钟，是单次闹钟的时候，weekDay参数无效，是重复闹钟的时候 year，month，day参数无效*/
    uint8_t durationTimeSec;/*!闹钟持续时长 */
    bool isOpen;
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
    bool isRepeatRemindOpen;    //重复提醒开关
    uint8_t repeatRemindHour;
    uint8_t repeatRemindMin;
    uint8_t repeatRemindTimes;  //重复提醒剩余次数
#endif
    bool isFinish;/*! 是否完成创建闹钟 */
} ute_module_systemtime_one_alarm_t;

typedef struct
{
    ute_module_systemtime_one_alarm_t alarmParam[SYSTEM_TIME_ALARMS_MAX_CNT];
    uint8_t alarmDisplayIndex;
    uint8_t alarmTotalCnt;
    uint8_t isRemindingIndex;   /*!正在闹铃中的闹钟*/
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
    uint8_t cancelCurrAlarmIndex; //取消当前闹钟重复提醒
    uint8_t cancelRepeatAlarmIndex;//取消重复闹钟信息提醒
#endif
} ute_module_systemtime_alarm_t;

/**
 * @brief   注册每秒数据结构
 * @details
 */
typedef struct
{
    uint8_t regCnt;/*!已注册数量 zn.zeng, 2021-07-12  */
    ute_module_systemtime_reg_func_t function[UTE_MODULE_SYSTEMTEIM_REGISTER_MAX_CNT];/*! 注册的函数指针zn.zeng, 2021-07-12  */
} ute_module_systemtime_register_t;
enum
{
    SETTING_TIME = 0,
    SETTING_DATE,
};
void uteModuleSystemtimeInit(void);
void uteModuleSystemtimeSetTime(ute_module_systemtime_time_t set);

void uteModuleSystemtimeSecondCb(void);
uint8_t uteModuleSystemtimeGetWeek(uint16_t year, uint8_t month, uint8_t day);
void uteModuleSystemtimeGetTime(ute_module_systemtime_time_t *time);
void uteModuleSystemtimeInputDateCalDay(ute_module_systemtime_time_t *time,uint16_t addDay);
void uteModuleSystemtimeInputDateIncreaseDecreaseDay(ute_module_systemtime_time_t *time, int day);
void uteModuleSystemtimeRegisterSecond(void* function);
void uteModuleSystemtimeRegisterSecondCb(uint32_t param);
bool uteModuleSystemtimeIsNight(void);
bool uteModuleSystemtimeIsSleepTime(void);
void uteModuleSystemtimeSetTimeFormat(bool is12Hour,bool isMi);
void uteModuleSystemtimeSetTimeZone(int8_t zone);
void uteModuleSystemtimeSetLanguage(uint16_t id);
void uteModuleSystemtimeAPPSetLanguage(uint16_t id);
void uteModuleSystemtimeReadConfig(void);
uint16_t uteModuleSystemtimeReadLanguage(void);
bool uteModuleSystemtimeCompareLanguage(uint16_t languageId);
bool uteModuleSystemtimeGetDistanceMiType(void);
void uteModuleSystemtimeSaveTimeInfo(void);
void uteModuleSystemtimeResetChange12HourTime(void);
bool uteModuleSystemtimeWhetherChange12HourTime(void);
void uteModuleSystemtime12HSwitchStatus(bool isopen);
bool uteModuleSystemtime12HOn(void);
void uteModuleSystemTimeLocalTimeSetStatus(uint8_t status);
uint8_t uteModuleSystemTimeLocalTimeGetStatus(void);
void uteModuleSystemTimeLocalSetDate(uint16_t year,uint8_t month,uint8_t day);
void uteModuleSystemTimeLocalSetHourMin(uint8_t hour,uint8_t min);

void uteModuleSystemtimeSaveAlarmInfo(ute_module_systemtime_one_alarm_t value,uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmTotalCnt(void);
void uteModuleSystemtimeDeleteAlarm(uint8_t index);
void uteModuleSystemtimeGetAlarm(ute_module_systemtime_one_alarm_t *set, uint8_t index);
void uteModuleSystemtimeSetAlarm(ute_module_systemtime_one_alarm_t set, uint8_t index);

///适配中科平台接口（闹钟）
void uteModuleSystemtimeEnableAlarm(uint8_t index, bool en);
uint8_t uteModuleSystemtimeGetAlarmHour(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmMin(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmSec(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmCycle(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmEnableState(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmMaxCnt(void);
uint8_t uteModuleSystemtimeGetAlarmIsFree(uint8_t index);
uint8_t uteModuleSystemtimeAlarmEdit(uint8_t index, bool enable, uint8_t cycle, uint8_t alarm_hour, uint8_t alarm_minute, uint8_t motor_mode, uint8_t remind_later);
uint8_t uteModuleSystemtimeGetAlarmRingIndex(void);
void uteModuleSystemtimeSetAlarmRingIndex(uint8_t index);

#endif //_UTE_MODULE_SYSTEMTIME_H_

