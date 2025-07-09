/**
*@file
*@brief        多运动模块头文件
*@details     包含正常走路，跑步，登山等其他运动模式
*@author       zn.zeng
*@date       2021-07-27
*@version      v1.0
*/

#ifndef _UTE_MODULE_SPORT_H_
#define _UTE_MODULE_SPORT_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_systemtime.h"
#include "include.h"

#if  UTE_MODULE_SPORT_SUPPORT
/*! sport 计步数据保存目录 zn.zeng, 2021-08-23  */
#define UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR "sportStep"
/*!写入久坐提醒，抬手亮屏，目标步数信息参数 zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_SPORTINFO  "systemparam/sportInfo"
/*! sport 多运动数据保存目录,只保存运动数据，其他内容不允许放在该文件夹下 zn.zeng, 2021-11-12  */
#define UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR "moreSports"
/*! 保存多运动列表显示的运动数量跟类型 xjc, 2022-03-29  */
#define UTE_MODULE_FILESYSTEM_SPORT_HUNDRED_SPORT_SORT_DATA "systemparam/sportSort"

/*! 站立时长、运动时长、卡路里、步数、距离目标提醒数据 dengli.lu, 2022-01-17  */
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_TARGET_NOTIFY_DATA "systemparam/targetNotifyData"
#endif
/*! 站立时长数据 dengli.lu, 2022-01-17  */
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
#define UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA "satndTime"
#endif

/*! 每小时所有运动的步数数据 casen, 2022-07-14  */
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_ALLSPORT_STEP_DATA "systemparam/everyHourAllSportStep"
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_ALLSPORT_KCAL_DATA "systemparam/everyHourAllSportKcal"
#endif

#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_SPORT_STEP_DATA "systemparam/everyHourSportStep"
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_SPORT_KCAL_DATA "systemparam/everyHourSportKcal"
#endif

/*多运动目标设置保存*/
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
#define UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_DATA "systemparam/moreSportsTarget"
#endif

#define SPORTS_HEART_DATAS_CACHE_LENGTH 90

/*! 计步类型，计步、睡眠、不计步不睡眠 zn.zeng, 2021-08-05  */
typedef enum
{
    STEP_TYPE_NONE,/*! 不计步不睡眠zn.zeng, 2021-08-05  */
    STEP_TYPE_STEP,/*! 计步zn.zeng, 2021-08-05  */
    STEP_TYPE_SLEEP,/*! 睡眠zn.zeng, 2021-08-05  */
} ute_module_sport_step_type;
/*! 久坐提醒数据结构zn.zeng, 2021-08-06  */
typedef struct
{
    bool isOpen;
    uint8_t setMin;
    uint8_t startPeriodHour;
    uint8_t startPeriodMin;
    uint8_t endPeriodHour;
    uint8_t endPeriodMin;
    uint8_t isOpenNoonSleep;
    uint16_t noonSleepStartTime;//午休开始时间，单位：分钟，时*60+分
    uint16_t noonSleepEndTime;//午休结束时间，单位：分钟，时*60+分
    uint16_t sedentaryRemindSecCnt;
} ute_module_sport_sedentary_param_t;
/*! 多运动数据 zn.zeng, 2021-11-11  */
typedef enum
{
    ALL_SPORT_STATUS_CLOSE      = 0X00,
    ALL_SPORT_STATUS_OPEN       = 0x11,
    ALL_SPORT_STATUS_PAUSE      = 0x22,
    ALL_SPORT_STATUS_CONTINUE   = 0x33,
    ALL_SPORT_STATUS_UPDATE_DATA =0x44,
} all_sport_status_type;
typedef struct
{
    uint32_t step;
    uint16_t distance;
    uint16_t caloire;
} ute_sports_today_with_step_data_t;
typedef struct
{
    uint8_t  startInterval;// 存储间隔时间,10s 一个单位
    uint8_t  sportsType; //运动标志
    struct prf_date_time startSportTime; //开始时间
    struct prf_date_time stopSportTime;  //结束时间
    uint32_t sportStep; //运动量,步数
    uint16_t sportTimes;    //运动量,次数
    uint16_t sportCaloire;  //运动量,卡路里
    uint8_t  sportDistanceInteger;  //运动量,距离（整数） //距离（2byte前一个字节是整数位，后一个字节是小数位）
    uint8_t  sportDistanceDecimals; //运动量,距离（小数）
    uint8_t  avgHeartRate;  //平均心率
    uint8_t  maxHeartRate;  //最大心率
    uint8_t  minHeartRate;  //最小心率
    uint8_t  avgTimeMinute; //平均配速，2bytes 格式为”分+秒”。0x08代表8分，0x32代表50秒，平均配速的意思是，“移动一公里需要多少时间”；
    uint8_t  avgTimeSecond;
} ute_sports_save_data_t;
typedef struct
{
    uint8_t status;/*! 运动状态，开始/停止、暂停、继续 zn.zeng, 2021-11-11  */
    bool isAppStart;/*! 是否为app端启动 zn.zeng, 2021-11-11  */
    uint8_t heartCache[SPORTS_HEART_DATAS_CACHE_LENGTH];//15min
    uint16_t heartTotalCnt;
    uint32_t heartTotalSum;
    uint32_t totalSportTime; /*! 总时长，单位为秒zn.zeng, 2021-11-11  */
    int8_t countZeroIndex;/*! 倒计时开始 zn.zeng, 2021-11-11  */
    ute_sports_save_data_t saveData;/*! 需要保存的数据 zn.zeng, 2021-11-11  */
    bool isAppStartSportReConnection;
    uint16_t appStartSportDisconnectTimeOutCnt;
} ute_module_more_sports_data_t;

#if 1//APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
typedef struct
{
    bool isTodayKcalTargetNotify;
    bool isTodayStepTargetNotify;
    bool isTodayDistanceTargetNotify;
    bool isTodayStandTimeTargetNotify;
    bool isTodaySportTimeTargetNotify;
    bool isTodayAllTargetNotify;
} ute_module_today_target_notify_type_t;

typedef struct
{
    uint16_t todayKcalTarget;
    bool isTodayKcalTargetNotifyOpen;
    bool isTodayKcalTargetHasNotify;
    bool isTodayStepTargetNotifyOpen;
    uint32_t todayDistanceTarget;//单位米
    bool isTodayDistanceTargetNotifyOpen;
    bool isTodayDistanceTargetHasNotify;

    bool isStandTimeTargetNotifyOpen;
    bool isStandTimeTargetHasNotify;
    uint16_t standTimeTargetSetMin;//分钟
    uint16_t currStandMin;
    uint32_t allMinStepCnt;
    uint32_t lastMinStepCnt;
    uint8_t standStartHour;//小时
    uint8_t standStartMin;//分钟

    bool isSportTimeTargetNotifyOpen;
    bool isSportTimeTargetHasNotify;
    uint16_t sportTimeTargetSetMin;//分钟
    uint32_t currentDayAllSportTimeSec;//当天运动时长，秒，不需要保存

    bool isTodayAllTargetHasNotify;
    ute_module_today_target_notify_type_t todayTargetNotifyType;
} ute_module_target_notify_data_t;
#endif
#if (UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT)||(GUI_SCREEN_SIZE_368X448RGB_I260001_SUPPORT)
typedef struct
{
    uint32_t everyHourAllSportStep[24];
    uint16_t everyHourAllSportKcal[24];
    uint32_t everyHourRunStep[24];
    uint32_t everyHourWalkStep[24];
    uint16_t everyHourKcal[24];
    uint16_t everyHourSportTime[24];
    uint16_t everyHourStandTime[24];
    uint32_t weekDayTotalStep[7];
    bool sedentarinessFlag;
#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
    uint32_t currentDayEveryHourSportStep[24];
    uint16_t currentDayEveryHourSportKcal[24];
    uint32_t lastHourSportStep;
    uint16_t lastHourSportKcal;
    bool needSaveCurrentDayEveryHourSportData;
#endif
} ute_module_daily_activity_data_t;
#endif
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
typedef struct
{
    sport_mode_type sportMode; //运动类型
    uint8_t sportOnOff; //运动开关
    uint8_t sportOrder; //运动序号
} ute_sports_list_param_t;
typedef struct
{
    ute_sports_list_param_t sportListData[UTE_MODULE_SPORT_MAX_SPORT_NUM];
    uint8_t receiveCrcCheck;
    uint16_t receiveSportBytesLen;
    uint8_t receiveSportBytes[UTE_MODULE_SPORT_MAX_SPORT_NUM*3];
    uint16_t minSportNum; //最少显示运动数量
    uint16_t maxSportNum; //最大显示运动数量
    bool appUpdateTrainingListFlag; //APP修改运动列表
} ute_sports_hundred_sort_data_t;
#endif

#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
typedef struct
{
    ute_sports_save_data_t oneHistoryRecordData[UTE_MODULE_SPORT_MORE_SPORTS_DATA_MAX_CNT];
    uint8_t sportsHistoryRecordTotalCnt;
    uint8_t displayIndex;
    bool IsHistoryRecordData;
} ute_module_sports_record_data_t;
#endif

#if UTE_MODULE_SCREENS_WORKOUTS_LIST_SUPPORT
typedef enum
{
    WORKOUTS_LIST_MODE_RUNNING_AND_WALKING =0,
    WORKOUTS_LIST_MODE_CYCLING =1,
    WORKOUTS_LIST_MODE_OUTDOOR,
    WORKOUTS_LIST_MODE_INDOOR,
    WORKOUTS_LIST_MODE_DANCE,
    WORKOUTS_LIST_MODE_COMBAT,
    WORKOUTS_LIST_MODE_BALL,
    WORKOUTS_LIST_MODE_WATER,
    WORKOUTS_LIST_MODE_WINTER,
    WORKOUTS_LIST_MODE_LEISURE,
    WORKOUTS_LIST_MODE_OTHER,
} ute_module_workout_list_mode_t;
#endif
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
typedef struct
{
    uint32_t sportDistanceTargeCnt;//米
    bool isSportDistanceTargetNotify;//目标距离提醒
    bool isSportDistanceTargetHasNotify;//目标时距离已经提醒过
    uint32_t sportTimeTargetSec;
    bool isSportTimeTargetNotify;//目标时间提醒
    bool isSportTimeTargetHasNotify;//目标时间已经提醒过
    uint16_t sportKcalTarget;
    bool isSportKcalTargetNotify;
    bool isSportKcalTargetHasNotify;
    bool isSportIntegerHourNotify;//运动整小时提醒
    bool isSportIntegerHourHasNotify;
    bool isSportIntegerKmNotify;//运动整千米提醒
    bool isSportIntegerKmHasNotify;
    bool isNoTarget;//无目标
} ute_module_sports_target_data_t;
#endif

#if UTE_MODULE_SPORT_CLASSIFICATION_SUPPORT
typedef enum
{
    TRAIN_LIST_PAGE_MAIN_SPORT = 0, //默认运动|运动分类
    TRAIN_LIST_PAGE_MORE_SPORT,     //更多运动
    TRAIN_LIST_PAGE_NORMAL,         //常规模式
    TRAIN_LIST_PAGE_OUTDOOR,        //户外模式
    TRAIN_LIST_PAGE_BALL,           //球类
    TRAIN_LIST_PAGE_DANCE,          //舞蹈
    TRAIN_LIST_PAGE_MARTIAL_ART,    //武术
    TRAIN_LIST_PAGE_GYMNASTICS,     //体操
    TRAIN_LIST_PAGE_OTHER,          //其它运动
    TRAIN_LIST_PAGE_MAX,
} TRAIN_LIST_PAGE_T;
#endif


/*! sport数据结构zn.zeng, 2021-07-27  */
typedef struct
{
    gsensor_mode_type accRange;/*! 传入算法acc 范围标记，区别于驱动zn.zeng, 2021-07-27  */
    gsensor_id_type gsensorId;/*! 传入算法gsensor型号标记 zn.zeng, 2021-07-27  */
    ute_module_sport_step_type stepType;
    ute_module_sport_step_type lastStepType;/* 记录上一次的类型 zn.zeng 2021-12-27*/
    ute_step_sleep_param_t stepSleepData;
    uint16_t lastHourStepCnt;
    ute_module_sport_sedentary_param_t sedentary;
    bool isOpenHandScreenOn;
    uint32_t stepsTargetCnt;
    bool isTakingPictures;
    uint32_t currDayTotalStep;
    uint32_t currDayWalkStep;
    uint32_t currDayRunStep;
    uint32_t currDayReadTotalStep;//不包括当前小时
    uint32_t currDayReadWalkStep;//不包括当前小时
    uint32_t currDayReadRunStep;//不包括当前小时
    int lastDayReadHour;//上一次读取数据的小时，减少功耗
    bool isTargetCompleted;
    uint32_t currDayTotaladdSportStep;
    ute_module_more_sports_data_t moreSportData;
    ute_sports_today_with_step_data_t todaySportWithStepsData;
#if 1//APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
    ute_module_target_notify_data_t todayTargetNotify;
#endif
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    ute_sports_hundred_sort_data_t sportSort;
#endif
#if (UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT)||(GUI_SCREEN_SIZE_368X448RGB_I260001_SUPPORT)
    ute_module_daily_activity_data_t dailyActivity;
#endif
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
    ute_module_sports_record_data_t sportsHistoryRecord;
#endif
#if UTE_MODULE_SCREENS_WORKOUTS_LIST_SUPPORT
    ute_module_workout_list_mode_t modeWorkOutList;
#endif
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
    ute_module_sports_target_data_t moreSportsTarget;
#endif
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
    bool updateTodayAllSportData;
#endif
#if UTE_MODULE_SPORT_CLASSIFICATION_SUPPORT
    TRAIN_LIST_PAGE_T trainListPage;
#endif
#if APP_DBG_GSENSOR_DATA
    bool appDebugGsensorDataSwitch;
#endif
#if UTE_MODULE_WATCH_ROTATE_SUPPORT
    bool isGsensorRotateTesting;//手表旋转中
#endif
#if UTE_MODULE_SPROT_ALGO_AUTO_SWITCH_SYSCLK_SUPPORT
    uint8_t switchSysclkCountdown; //切换系统时钟倒计时
#endif
} ute_module_sport_data_t;

#if UTE_MODULE_SPORTS_HISTORY_HEART_ZONE_SUPPORT
typedef struct
{
    uint8_t reducePer;
    uint8_t burnFatPer;
    uint8_t heartLungPer;
    uint8_t noOxygenPer;
    uint8_t limitPer;
} ute_module_sport_heart_zone_data_t;
#endif

void uteModuleSportInit(void);
void uteModuleSportEverySecond(void);
void uteModuleSportSetGsensorIdAccRange(void);
void uteModuleSportUpdateGsensorIdAccRange(void);
void uteModuleSportStepTypeSetStep(void);
void uteModuleSportStepTypeSetSleep(void);
void uteModuleSportStepTypeSetNone(void);
ute_module_sport_step_type uteModuleSportGetStepType(void);
void uteModuleSprotResetRovllverScreenMode(void);
void uteModuleSportInputDataBeforeAlgo(void);
void uteModuleSportSendRealTimeStepInfo(void);
void uteModuleSportSaveStepData(void);
void uteModuleSportStartSendStepHistoryData(ute_module_systemtime_time_t time);
void uteModuleSportSetTakePictureEnable(bool isEnable);
void uteModuleSportTakePictureHandlerMsg(void);
void uteModuleSportOpenAppTakePictureScreenHandler(void);
void uteModuleSportExitTakePicture(void);
bool uteModuleSportIsTakePicture(void);
void uteModuleSportSaveSedentaryParam(ute_module_sport_sedentary_param_t *param);
void uteModuleSportReadSedentaryParam(ute_module_sport_sedentary_param_t *param);
void uteModuleSportSaveHandScreenOnStepsTargetCnt(bool isHandOpen,uint32_t targetCnt);
void uteModuleSportReadConfig(void);
void uteModuleSportGetCurrDayStepCnt(uint32_t *total,uint32_t *walk,uint32_t *run);
uint32_t uteModuleSportGetStepsTargetCnt(void);
void uteModuleSportSetIsTargetCompleted(bool isTargetCompleted);
bool uteModuleSportGetIsTargetCompleted(void);
bool uteModuleSportGetIsOpenHandScreenOn(void);
uint16_t uteModuleSportGetCurrDayDistanceData(void);
uint16_t uteModuleSportGetCurrDayKcalData(void);
void uteModuleSportReadCurrDayStepCnt(ute_module_systemtime_time_t time);
void uteModuleSportStartMoreSports(uint8_t sportsType,uint8_t startInterval,bool isAppStart);
void uteModuleSportStartMoreSportsMsgHandler(uint32_t param);
void uteModuleSportStopMoreSports(void);
bool uteModuleSportMoreSportsIsLessData(void);
void uteModuleSportStopMoreSportsMsgHandler(void);
void uteModuleSportMoreSportsEverySecond(ute_module_systemtime_time_t *time);

#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
void uteModuleSportCalculatorEveryHourSportDataHandler(bool isFinish);
void uteModuleSportSaveEveryHourData(void);
void uteModuleSportReadEveryHourData(void);
#endif

void uteModuleSportMoreSportsSaveData(bool isStopSport);
void uteModuleSportMoreSportsDeleteLessData(void);
void uteModuleSportSyncAppMoreSportData(const uint8_t *receive);
void uteModuleSportStartSendMoreSportHistoryData(ute_module_systemtime_time_t time);
void uteModuleSportSetCountZeroIndex(uint8_t countZeroIndex);
void uteModuleSportDisconnectHandler(void);
void uteModuleSportSedentaryMsg(void);
void uteModuleSportUnWearToWearSwitch(void);
void uteModuleSportSyncAppSportStatus(all_sport_status_type sportStatus);
void uteModuleSportDelaySetTakePictureEnableHandlerMsg(void);
void uteModuleSportDelaySetTakePictureEnableTimerStart(uint16_t ms);
bool uteModuleSportMoreSportIsRuning(void);
uint8_t uteModuleSportMoreSportGetType(void);
uint8_t uteModuleSportMoreSportGetStatus(void);
void uteModuleSportMoreSportSetStatus(all_sport_status_type sportStatus);
bool uteModuleSportMoreSportIsAppStart(void);
void uteModuleSportMoreSportSetIsAppStartReconnection(bool isAppStartSportReConnection);
bool uteModuleSportMoreSportIsAppStartReconnection(void);
void uteModuleSportGetMoreSportsDatas(ute_module_more_sports_data_t *data);
int8_t uteModuleSportMoreSportGetCountZeroIndex(void);
void uteModuleSprotCountdownTimerHandlerMsg(void);
void uteModuleSprotCountdownTimerStart(uint16_t ms);
void uteModuleSprotCountdownTimerStop(void);
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
void uteModuleSportHundredSportCmd(uint8_t*receive,uint8_t length);
int uteModuleSportFindHundredSportIndex(uint8_t sportOrder);
uint8_t uteModuleSportGetRealIndex(uint8_t sportMode, const uint8_t *sportList);
int uteModuleSportGetHundredSportValidNumber(void);
bool uteModuleSportGetAppUpdateTrainingListFlag(void);
void uteModuleSportSetAppUpdateTrainingListFlag(bool isUpdate);
#endif
void uteModuleSportSystemtimeChange(ute_module_systemtime_time_t curr,ute_module_systemtime_time_t newSet);
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
void uteModuleSportClearTodayAllSportData(void);
void uteModuleSportReadTodayAllSportDataConfig(ute_module_systemtime_time_t time,ute_sports_today_with_step_data_t *stepData,ute_module_target_notify_data_t *targetNotifyData);
#endif
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
void uteModuleSportReadTodayTargetNotifyData(void);
void uteModuleSportSaveTodayTargetNotifyData(void);
void uteModuleSportTodayTargetHandler(void);
void uteModuleSportSetTodayTargetNotifyData(ute_module_target_notify_data_t setTodayTarget);
void uteModuleSportGetTodayTargetNotifyData(ute_module_target_notify_data_t *todayTarget);
void uteModuleSportProtocolGetTodayTargetNotifyData(ute_module_target_notify_data_t *todayTarget);
void uteModuleSportSaveStandTimeData(uint8_t standStartHour,uint8_t standStartMin,uint8_t currStandMin);
void uteModuleSportStartSendStandTimeHistoryData(void);
#endif
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
void uteModuleSportReadHistoryRecordData(void);
void uteModuleSportsHistoryRecordGetData(ute_module_sports_record_data_t *data);
void uteModuleSportsHistoryRecordSetIsHistoryRecordData(bool IsHistoryRecordData);
bool uteModuleSportsHistoryRecordGetIsHistoryRecordData(void);
uint8_t uteModuleSportsHistoryGetRecordTotalCnt(void);
void uteModuleSportsSetHistoryRecordDisplayIndex(uint8_t displayIndex);
#endif
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
void uteModuleSportReadTodayEveryHourAllSportStepData(void);
void uteModuleSportSaveTodayEveryHourAllSportStepData(void);
void uteModuleSportReadTodayEveryHourAllSportKcalData(void);
void uteModuleSportSaveTodayEveryHourAllSportKcalData(void);
uint32_t uteModuleSportGetCurrDayEveryHourSportTime(void);
uint16_t uteModuleSportGetCurrDayEveryHourKcal(void);
uint16_t uteModuleSportGetCurrDayEveryHourStandTime(void);
uint32_t uteModuleSportLoadTodayEveryHourStepHistoryData(UT_GraphsParam *everyHourStepHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
bool uteModuleSportLoadTodayEveryHourKcalHistoryData(UT_GraphsParam *everyHourKcalHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
bool uteModuleSportLoadTodayEveryHourSportTimeHistoryData(UT_GraphsParam *everyHourSportTimeHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
uint16_t uteModuleSportLoadTodayEveryHourStandTimeHistoryData(UT_GraphsParam *everyHourStandTimeHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
void uteModuleSportReadWeekDayStep(void);
uint32_t uteModuleSportLoadWeekDayStepHistoryData(UT_GraphsParam *weekDayStepHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange,uint32_t *totalStep);
#endif

#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
void uteModuleSportMoreSportsTargetInit(void);
void uteModuleSportMoreSportsTargetReadConfig(void);
void uteModuleSportMoreSportsTargetSaveData(void);
void uteModuleSportSetMoreSportsTargetData(ute_module_sports_target_data_t setMoreSportsTarget);
void uteModuleSportGetMoreSportsTargetData(ute_module_sports_target_data_t *sportsTarget);
void uteModuleSportMoreSportsTargetsProcess(void);
#endif
#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
void uteModuleSportMoreSportsHeartWarningProcess(void);
#endif
uint16_t uteModuleSportMoreSportGetDisconnectTimeoutCnt(void);

#if UTE_MODULE_SPORT_CLASSIFICATION_SUPPORT
TRAIN_LIST_PAGE_T uteModuleSportMoreSportGetTrainListPage(void);
void uteModuleSportMoreSportSetTrainListPage(TRAIN_LIST_PAGE_T trainListPage);
#endif

#if UTE_MODULE_SPORTS_HISTORY_HEART_ZONE_SUPPORT
void uteModuleSportGetHeartZoneArray(ute_module_sport_heart_zone_data_t *zoneData);
void uteModuleSportReadHistoryHeartZoneData(ute_module_systemtime_time_t *queryTime);
void uteModuleSportLoadHeartZoneGrapData(UT_GraphsParam (*zoneGramGraph)[5],int16_t x, int16_t y, uint16_t drawWidth, uint16_t drawHight);
#endif

#if APP_DBG_GSENSOR_DATA
void uteModuleSportSendGsensorDataToPhone(int16_t tempx, int16_t tempy,int16_t tempz);
void uteModuleSportSetAppDebugGsensorDataSwitch(bool appDebugGsensorDataSwitch);
bool uteModuleSportGetAppDebugGsensorDataSwitch(void);
#endif

#if UTE_MODULE_WATCH_ROTATE_SUPPORT
bool uteModuleSportGetGsensorRotateTesting(void);
void uteModuleSportSetGsensorRotateTesting(bool isGsensorRotateTesting);
uint16_t uteModuleSportGetGsensorRotateCnt(void);
void uteModuleSportClearGsensorRotateData(void);
#endif

void uteModuleSportAlgoTimerStart(uint16_t ms);
void uteModuleSprotAlgoTimerStop(void);
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_ELLIPSIS_TIMER_SUPPORT
bool uteModuleSportAlgoTimerIsRunning(void);
#endif
void uteModuleSprotInputDataBeforeAlgoTimerHandler(void);

#endif
#endif //_UTE_MODULE_SPORT_H_

