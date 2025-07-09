/**
*@file
*@brief        多运动模块
*@details     包含正常走路，跑步，登山等其他运动模式
*@author       zn.zeng
*@date       2021-07-27
*@version      v1.0
*/

#include "ute_module_message.h"
#include "ute_module_log.h"
#include "ute_module_sport.h"
#include "ute_drv_gsensor_common.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_protocol.h"
#include "ute_module_filesystem.h"
#include "ute_module_crc.h"
#include "ute_module_sleep.h"
#include "ute_module_gui_common.h"
#include "ute_drv_battery_common.h"
#include "ute_drv_motor.h"
// #include "ute_drv_heart_vcxx_common.h"
#include "ute_module_notdisturb.h"
#if UTE_MODULE_DRINK_WATER_NOTIFY_SCREEN_SUPPORT
#include "ute_module_drinkwater.h"
#endif
#include "ute_module_heart.h"
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
#include "cwm_lib.h"
#include "ute_module_CyweeMotion.h"
#if (!UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT)
ute_step_sleep_param_t mStepSleepParam;
#endif
#endif
#if UTE_MODULE_WATCH_ROTATE_SUPPORT
#include "ute_rotate.h"
#endif

#include "func_cover.h"
#if UTE_MODULE_SPORT_SUPPORT
/*! sport的数据 zn.zeng, 2021-10-23  */
ute_module_sport_data_t uteModuleSprotData;
uint8_t uteModuleSportMoreSportSyncDataBuff[170];
/* 运动互斥量 zn.zeng 2022-02-14*/
void *uteModuleSprotMute;
/* 运动倒计时定时器 xjc 2022-05-31*/
void *uteModuleSprotCountdownTimer = NULL;
/*! 记步算法定时器,wang.luo 2023-05-05 */
#if !UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_ELLIPSIS_TIMER_SUPPORT
void *uteModuleSportInputDataBeforeAlgoTimer = NULL;
#else
static bool enableSportAlgorithmsTimer = false;
#endif
/**
*@brief  多运动模块初始化
*@details  在gsensor初始化后执行
*@author        zn.zeng
*@date        2021-07-27
*/
void uteModuleSportInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR);
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR);
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA);
#endif
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR, NULL, NULL);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR, NULL, NULL);
#endif
    uteModulePlatformCreateMutex(&uteModuleSprotMute);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,uteModuleSprotMute = 0x%d,", __func__,uteModuleSprotMute);
    memset(&uteModuleSprotData,0,sizeof(ute_module_sport_data_t));
    uteModuleSprotData.lastDayReadHour = -1;
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    uteModuleSprotData.sportSort.minSportNum = UTE_MODULE_SPORT_DISPLAY_MIN_SPORT_NUM;
    uteModuleSprotData.sportSort.maxSportNum = UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM;
#endif

    uteModuleSportReadConfig();
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
    uteModuleSportReadTodayTargetNotifyData();
#endif
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
    uteModuleSportReadTodayEveryHourAllSportStepData();
    uteModuleSportReadTodayEveryHourAllSportKcalData();
    uteModuleSportReadWeekDayStep();
#endif

    uteModuleSystemtimeRegisterSecond(uteModuleSportEverySecond);
    uteModuleSportStepTypeSetStep();
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    disableTakingPicturesState();
    initHandRollverScreenParam(ROLLOVER_HAND_SCREEN_X_MIN,ROLLOVER_HAND_SCREEN_X_MAX,ROLLOVER_HAND_SCREEN_Y_MIN,ROLLOVER_HAND_SCREEN_Y_MAX,ROLLOVER_HAND_SCREEN_Z_MIN,ROLLOVER_HAND_SCREEN_Z_MAX,ROLLOVER_HAND_SCREEN_ACC_DIFF_VALUE);
#endif
    uteModuleSprotResetRovllverScreenMode();

#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
    uteModuleSportReadHistoryRecordData();
#endif
}

/*! 支持百种多运动 xjc, 2022-03-29  */
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
const ute_sports_list_param_t uteModuleSportDefaultSort[] =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    {.sportMode = SPORT_TYPE_RUNNING, .sportOnOff = UTE_MODULE_SPORT_RUNNING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_RUNNING_ORDER},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {.sportMode = SPORT_TYPE_RIDE_BIKE, .sportOnOff = UTE_MODULE_SPORT_BIKE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BIKE_ORDER},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    {.sportMode = SPORT_TYPE_JUMP_ROPE, .sportOnOff = UTE_MODULE_SPORT_JUMP_ROPE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_JUMP_ROPE_ORDER},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    {.sportMode = SPORT_TYPE_SWIMMING, .sportOnOff = UTE_MODULE_SPORT_SWIMMING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SWIMMING_ORDER},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    {.sportMode = SPORT_TYPE_BADMINTON, .sportOnOff = UTE_MODULE_SPORT_BADMINTION_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BADMINTION_ORDER},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    {.sportMode = SPORT_TYPE_TABLE_TENNIS, .sportOnOff = UTE_MODULE_SPORT_TABLE_TENNIS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_TABLE_TENNIS_ORDER},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    {.sportMode = SPORT_TYPE_TENNIS, .sportOnOff = UTE_MODULE_SPORT_TENNIS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_TENNIS_ORDER},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    {.sportMode = SPORT_TYPE_CLIMBING, .sportOnOff = UTE_MODULE_SPORT_CLIMBING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_CLIMBING_ORDER},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    {.sportMode = SPORT_TYPE_WALKING, .sportOnOff = UTE_MODULE_SPORT_WALKING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_WALKING_ORDER},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    {.sportMode = SPORT_TYPE_BASKETBALL, .sportOnOff = UTE_MODULE_SPORT_BASKETBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BASKETBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    {.sportMode = SPORT_TYPE_FOOTBALL, .sportOnOff = UTE_MODULE_SPORT_FOOTBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_FOOTBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    {.sportMode = SPORT_TYPE_BASEBALL, .sportOnOff = UTE_MODULE_SPORT_BASEBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BASEBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    {.sportMode = SPORT_TYPE_VOLLEYBALL, .sportOnOff = UTE_MODULE_SPORT_VOLLEYBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_VOLLEYBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    {.sportMode = SPORT_TYPE_CRICKET, .sportOnOff = UTE_MODULE_SPORT_CRICKET_ON_OFF, .sportOrder = UTE_MODULE_SPORT_CRICKET_ORDER},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    {.sportMode = SPORT_TYPE_RUGBY, .sportOnOff = UTE_MODULE_SPORT_RUGBY_ON_OFF, .sportOrder = UTE_MODULE_SPORT_RUGBY_ORDER},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    {.sportMode = SPORT_TYPE_HOCKEY, .sportOnOff = UTE_MODULE_SPORT_HOCKEY_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HOCKEY_ORDER},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    {.sportMode = SPORT_TYPE_DANCE, .sportOnOff = UTE_MODULE_SPORT_DANCE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_DANCE_ORDER},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    {.sportMode = SPORT_TYPE_SPINNING, .sportOnOff = UTE_MODULE_SPORT_SPINNING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SPINNING_ORDER},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    {.sportMode = SPORT_TYPE_YOGA, .sportOnOff = UTE_MODULE_SPORT_YOGA_ON_OFF, .sportOrder = UTE_MODULE_SPORT_YOGA_ORDER},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    {.sportMode = SPORT_TYPE_SIT_UP, .sportOnOff = UTE_MODULE_SPORT_SIT_UP_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SIT_UP_ORDER},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    {.sportMode = SPORT_TYPE_TREADMILL, .sportOnOff = UTE_MODULE_SPORT_TREADMILL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_TREADMILL_ORDER},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    {.sportMode = SPORT_TYPE_GYMNASTICS, .sportOnOff = UTE_MODULE_SPORT_GYMNASTICS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_GYMNASTICS_ORDER},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    {.sportMode = SPORT_TYPE_BOATING, .sportOnOff = UTE_MODULE_SPORT_BOATING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BOATING_ORDER},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    {.sportMode = SPORT_TYPE_JUMPING_JACK, .sportOnOff = UTE_MODULE_SPORT_JUMPING_JACK_ON_OFF, .sportOrder = UTE_MODULE_SPORT_JUMPING_JACK_ORDER},
#endif
#if UTE_MODULE_SPORT_FREE_TRAINING_SUPPORT
    {.sportMode = SPORT_TYPE_FREE_TRAINING, .sportOnOff = UTE_MODULE_SPORT_FREE_TRAINING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_FREE_TRAINING_ORDER},
#endif
#if UTE_MODULE_SPORT_INDOOR_WALK_SUPPORT
    {.sportMode = SPORT_TYPE_INDOOR_WALK, .sportOnOff = UTE_MODULE_SPORT_INDOOR_WALK_ON_OFF, .sportOrder = UTE_MODULE_SPORT_INDOOR_WALK_ORDER},
#endif
#if UTE_MODULE_SPORT_INDOOR_RUN_SUPPORT
    {.sportMode = SPORT_TYPE_INDOOR_RUN, .sportOnOff = UTE_MODULE_SPORT_INDOOR_RUN_ON_OFF, .sportOrder = UTE_MODULE_SPORT_INDOOR_RUN_ORDER},
#endif
#if UTE_MODULE_SPORT_STRENGTH_TRAINING_SUPPORT
    {.sportMode = SPORT_TYPE_STRENGTH_TRAINING, .sportOnOff = UTE_MODULE_SPORT_STRENGTH_TRAINING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_STRENGTH_TRAINING_ORDER},
#endif
#if UTE_MODULE_SPORT_STEP_TRAINING_SUPPORT
    {.sportMode = SPORT_TYPE_STEP_TRAINING, .sportOnOff = UTE_MODULE_SPORT_STEP_TRAINING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_STEP_TRAINING_ORDER},
#endif
#if UTE_MODULE_SPORT_HORSE_RIDING_SUPPORT
    {.sportMode = SPORT_TYPE_HORSE_RIDING, .sportOnOff = UTE_MODULE_SPORT_HORSE_RIDING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HORSE_RIDING_ORDER},
#endif
#if UTE_MODULE_SPORT_ELLIPTICAL_TRAINER_SUPPORT
    {.sportMode = SPORT_TYPE_ELLIPTICAL_TRAINER, .sportOnOff = UTE_MODULE_SPORT_ELLIPTICAL_TRAINER_ON_OFF, .sportOrder = UTE_MODULE_SPORT_ELLIPTICAL_TRAINER_ORDER},
#endif
#if UTE_MODULE_SPORT_TAI_CHI_SUPPORT
    {.sportMode = SPORT_TYPE_TAI_CHI, .sportOnOff = UTE_MODULE_SPORT_TAI_CHI_ON_OFF, .sportOrder = UTE_MODULE_SPORT_TAI_CHI_ORDER},
#endif
#if UTE_MODULE_SPORT_SHUTTLECOCK_SUPPORT
    {.sportMode = SPORT_TYPE_SHUTTLECOCK, .sportOnOff = UTE_MODULE_SPORT_SHUTTLECOCK_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SHUTTLECOCK_ORDER},
#endif
#if UTE_MODULE_SPORT_BOXING_SUPPORT
    {.sportMode = SPORT_TYPE_BOXING, .sportOnOff = UTE_MODULE_SPORT_BOXING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BOXING_ORDER},
#endif
#if UTE_MODULE_SPORT_OUTDOOR_WALK_SUPPORT
    {.sportMode = SPORT_TYPE_OUTDOOR_WALK, .sportOnOff = UTE_MODULE_SPORT_OUTDOOR_WALK_ON_OFF, .sportOrder = UTE_MODULE_SPORT_OUTDOOR_WALK_ORDER},
#endif
#if UTE_MODULE_SPORT_TRAIL_RUNNING_SUPPORT
    {.sportMode = SPORT_TYPE_TRAIL_RUNNING, .sportOnOff = UTE_MODULE_SPORT_TRAIL_RUNNING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_TRAIL_RUNNING_ORDER},
#endif
#if UTE_MODULE_SPORT_SKIING_SUPPORT
    {.sportMode = SPORT_TYPE_SKIING, .sportOnOff = UTE_MODULE_SPORT_SKIING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SKIING_ORDER},
#endif
#if UTE_MODULE_SPORT_ICE_HOCKEY_SUPPORT
    {.sportMode = SPORT_TYPE_ICE_HOCKEY, .sportOnOff = UTE_MODULE_SPORT_ICE_HOCKEY_ON_OFF, .sportOrder = UTE_MODULE_SPORT_ICE_HOCKEY_ORDER},
#endif
#if UTE_MODULE_SPORT_TAEKWONDO_SUPPORT
    {.sportMode = SPORT_TYPE_TAEKWONDO, .sportOnOff = UTE_MODULE_SPORT_TAEKWONDO_ON_OFF, .sportOrder = UTE_MODULE_SPORT_TAEKWONDO_ORDER},
#endif
#if UTE_MODULE_SPORT_VO2MAX_TEST_SUPPORT
    {.sportMode = SPORT_TYPE_VO2MAX_TEST, .sportOnOff = UTE_MODULE_SPORT_VO2MAX_TEST_ON_OFF, .sportOrder = UTE_MODULE_SPORT_VO2MAX_TEST_ORDER},
#endif
#if UTE_MODULE_SPORT_ROWING_MACHING_SUPPORT
    {.sportMode = SPORT_TYPE_ROWING_MACHING, .sportOnOff = UTE_MODULE_SPORT_ROWING_MACHING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_ROWING_MACHING_ORDER},
#endif
#if UTE_MODULE_SPORT_AIR_WALKER_SUPPORT
    {.sportMode = SPORT_TYPE_AIR_WALKER, .sportOnOff = UTE_MODULE_SPORT_AIR_WALKER_ON_OFF, .sportOrder = UTE_MODULE_SPORT_AIR_WALKER_ORDER},
#endif
#if UTE_MODULE_SPORT_HIKING_SUPPORT
    {.sportMode = SPORT_TYPE_HIKING, .sportOnOff = UTE_MODULE_SPORT_HIKING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HIKING_ORDER},
#endif
#if UTE_MODULE_SPORT_ATHLETICS_SUPPORT
    {.sportMode = SPORT_TYPE_ATHLETICS, .sportOnOff = UTE_MODULE_SPORT_ATHLETICS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_ATHLETICS_ORDER},
#endif
#if UTE_MODULE_SPORT_WAIST_TRAINING_SUPPORT
    {.sportMode = SPORT_TYPE_WAIST_TRAINING, .sportOnOff = UTE_MODULE_SPORT_WAIST_TRAINING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_WAIST_TRAINING_ORDER},
#endif
#if UTE_MODULE_SPORT_KARATE_SUPPORT
    {.sportMode = SPORT_TYPE_KARATE, .sportOnOff = UTE_MODULE_SPORT_KARATE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_KARATE_ORDER},
#endif
#if UTE_MODULE_SPORT_COOLDOWN_SUPPORT
    {.sportMode = SPORT_TYPE_COOLDOWN, .sportOnOff = UTE_MODULE_SPORT_COOLDOWN_ON_OFF, .sportOrder = UTE_MODULE_SPORT_COOLDOWN_ORDER},
#endif
#if UTE_MODULE_SPORT_CROSS_TRAINING_SUPPORT
    {.sportMode = SPORT_TYPE_CROSS_TRAINING, .sportOnOff = UTE_MODULE_SPORT_CROSS_TRAINING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_CROSS_TRAINING_ORDER},
#endif
#if UTE_MODULE_SPORT_PILATES_SUPPORT
    {.sportMode = SPORT_TYPE_PILATES, .sportOnOff = UTE_MODULE_SPORT_PILATES_ON_OFF, .sportOrder = UTE_MODULE_SPORT_PILATES_ORDER},
#endif
#if UTE_MODULE_SPORT_CROSS_FIT_SUPPORT
    {.sportMode = SPORT_TYPE_CROSS_FIT, .sportOnOff = UTE_MODULE_SPORT_CROSS_FIT_ON_OFF, .sportOrder = UTE_MODULE_SPORT_CROSS_FIT_ORDER},
#endif
#if UTE_MODULE_SPORT_FUNCTIONAL_TRAINING_SUPPORT
    {.sportMode = SPORT_TYPE_FUNCTIONAL_TRAINING, .sportOnOff = UTE_MODULE_SPORT_FUNCTIONAL_TRAINING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_FUNCTIONAL_TRAINING_ORDER},
#endif
#if UTE_MODULE_SPORT_PHYSICAL_TRAINING_SUPPORT
    {.sportMode = SPORT_TYPE_PHYSICAL_TRAINING, .sportOnOff = UTE_MODULE_SPORT_PHYSICAL_TRAINING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_PHYSICAL_TRAINING_ORDER},
#endif
#if UTE_MODULE_SPORT_ARCHERY_SUPPORT
    {.sportMode = SPORT_TYPE_ARCHERY, .sportOnOff = UTE_MODULE_SPORT_ARCHERY_ON_OFF, .sportOrder = UTE_MODULE_SPORT_ARCHERY_ORDER},
#endif
#if UTE_MODULE_SPORT_FLEXIBILITY_SUPPORT
    {.sportMode = SPORT_TYPE_FLEXIBILITY, .sportOnOff = UTE_MODULE_SPORT_FLEXIBILITY_ON_OFF, .sportOrder = UTE_MODULE_SPORT_FLEXIBILITY_ORDER},
#endif
#if UTE_MODULE_SPORT_MIXED_CARDIO_SUPPORT
    {.sportMode = SPORT_TYPE_MIXED_CARDIO, .sportOnOff = UTE_MODULE_SPORT_MIXED_CARDIO_ON_OFF, .sportOrder = UTE_MODULE_SPORT_MIXED_CARDIO_ORDER},
#endif
#if UTE_MODULE_SPORT_LATIN_DANCE_SUPPORT
    {.sportMode = SPORT_TYPE_LATIN_DANCE, .sportOnOff = UTE_MODULE_SPORT_LATIN_DANCE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_LATIN_DANCE_ORDER},
#endif
#if UTE_MODULE_SPORT_STREET_DANCE_SUPPORT
    {.sportMode = SPORT_TYPE_STREET_DANCE, .sportOnOff = UTE_MODULE_SPORT_STREET_DANCE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_STREET_DANCE_ORDER},
#endif
#if UTE_MODULE_SPORT_KICKBOXING_SUPPORT
    {.sportMode = SPORT_TYPE_KICKBOXING, .sportOnOff = UTE_MODULE_SPORT_KICKBOXING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_KICKBOXING_ORDER},
#endif
#if UTE_MODULE_SPORT_BARRE_SUPPORT
    {.sportMode = SPORT_TYPE_BARRE, .sportOnOff = UTE_MODULE_SPORT_BARRE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BARRE_ORDER},
#endif
#if UTE_MODULE_SPORT_AUSTRALIAN_FOOTBALL_SUPPORT
    {.sportMode = SPORT_TYPE_AUSTRALIAN_FOOTBALL, .sportOnOff = UTE_MODULE_SPORT_AUSTRALIAN_FOOTBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_AUSTRALIAN_FOOTBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_MARTIAL_ARTS_SUPPORT
    {.sportMode = SPORT_TYPE_MARTIAL_ARTS, .sportOnOff = UTE_MODULE_SPORT_MARTIAL_ARTS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_MARTIAL_ARTS_ORDER},
#endif
#if UTE_MODULE_SPORT_STAIRS_SUPPORT
    {.sportMode = SPORT_TYPE_STAIRS, .sportOnOff = UTE_MODULE_SPORT_STAIRS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_STAIRS_ORDER},
#endif
#if UTE_MODULE_SPORT_HANDBALL_SUPPORT
    {.sportMode = SPORT_TYPE_HANDBALL, .sportOnOff = UTE_MODULE_SPORT_HANDBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HANDBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_BOWLING_SUPPORT
    {.sportMode = SPORT_TYPE_BOWLING, .sportOnOff = UTE_MODULE_SPORT_BOWLING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BOWLING_ORDER},
#endif
#if UTE_MODULE_SPORT_RACQUETBALL_SUPPORT
    {.sportMode = SPORT_TYPE_RACQUETBALL, .sportOnOff = UTE_MODULE_SPORT_RACQUETBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_RACQUETBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_CURLING_SUPPORT
    {.sportMode = SPORT_TYPE_CURLING, .sportOnOff = UTE_MODULE_SPORT_CURLING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_CURLING_ORDER},
#endif
#if UTE_MODULE_SPORT_HUNTING_SUPPORT
    {.sportMode = SPORT_TYPE_HUNTING, .sportOnOff = UTE_MODULE_SPORT_HUNTING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HUNTING_ORDER},
#endif
#if UTE_MODULE_SPORT_SNOWBOARDING_SUPPORT
    {.sportMode = SPORT_TYPE_SNOWBOARDING, .sportOnOff = UTE_MODULE_SPORT_SNOWBOARDING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SNOWBOARDING_ORDER},
#endif
#if UTE_MODULE_SPORT_PLAY_SUPPORT
    {.sportMode = SPORT_TYPE_PLAY, .sportOnOff = UTE_MODULE_SPORT_PLAY_ON_OFF, .sportOrder = UTE_MODULE_SPORT_PLAY_ORDER},
#endif
#if UTE_MODULE_SPORT_AMERICAN_FOOTBALL_SUPPORT
    {.sportMode = SPORT_TYPE_AMERICAN_FOOTBALL, .sportOnOff = UTE_MODULE_SPORT_AMERICAN_FOOTBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_AMERICAN_FOOTBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_HAND_CYCLING_SUPPORT
    {.sportMode = SPORT_TYPE_HAND_CYCLING, .sportOnOff = UTE_MODULE_SPORT_HAND_CYCLING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HAND_CYCLING_ORDER},
#endif
#if UTE_MODULE_SPORT_FISHING_SUPPORT
    {.sportMode = SPORT_TYPE_FISHING, .sportOnOff = UTE_MODULE_SPORT_FISHING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_FISHING_ORDER},
#endif
#if UTE_MODULE_SPORT_DISC_SPORTS_SUPPORT
    {.sportMode = SPORT_TYPE_DISC_SPORTS, .sportOnOff = UTE_MODULE_SPORT_DISC_SPORTS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_DISC_SPORTS_ORDER},
#endif
#if UTE_MODULE_SPORT_RUGGER_SUPPORT
    {.sportMode = SPORT_TYPE_RUGGER, .sportOnOff = UTE_MODULE_SPORT_RUGGER_ON_OFF, .sportOrder = UTE_MODULE_SPORT_RUGGER_ORDER},
#endif
#if UTE_MODULE_SPORT_GOLF_SUPPORT
    {.sportMode = SPORT_TYPE_GOLF, .sportOnOff = UTE_MODULE_SPORT_GOLF_ON_OFF, .sportOrder = UTE_MODULE_SPORT_GOLF_ORDER},
#endif
#if UTE_MODULE_SPORT_FOLK_DANCE_SUPPORT
    {.sportMode = SPORT_TYPE_FOLK_DANCE, .sportOnOff = UTE_MODULE_SPORT_FOLK_DANCE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_FOLK_DANCE_ORDER},
#endif
#if UTE_MODULE_SPORT_DOWNHILL_SKIING_SUPPORT
    {.sportMode = SPORT_TYPE_DOWNHILL_SKIING, .sportOnOff = UTE_MODULE_SPORT_DOWNHILL_SKIING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_DOWNHILL_SKIING_ORDER},
#endif
#if UTE_MODULE_SPORT_SNOW_SPORTS_SUPPORT
    {.sportMode = SPORT_TYPE_SNOW_SPORTS, .sportOnOff = UTE_MODULE_SPORT_SNOW_SPORTS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SNOW_SPORTS_ORDER},
#endif
#if UTE_MODULE_SPORT_MIND_BODY_SUPPORT
    {.sportMode = SPORT_TYPE_MIND_BODY, .sportOnOff = UTE_MODULE_SPORT_MIND_BODY_ON_OFF, .sportOrder = UTE_MODULE_SPORT_MIND_BODY_ORDER},
#endif
#if UTE_MODULE_SPORT_CORE_TRAINING_SUPPORT
    {.sportMode = SPORT_TYPE_CORE_TRAINING, .sportOnOff = UTE_MODULE_SPORT_CORE_TRAINING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_CORE_TRAINING_ORDER},
#endif
#if UTE_MODULE_SPORT_SKATING_SUPPORT
    {.sportMode = SPORT_TYPE_SKATING, .sportOnOff = UTE_MODULE_SPORT_SKATING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SKATING_ORDER},
#endif
#if UTE_MODULE_SPORT_FITNESS_GAMING_SUPPORT
    {.sportMode = SPORT_TYPE_FITNESS_GAMING, .sportOnOff = UTE_MODULE_SPORT_FITNESS_GAMING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_FITNESS_GAMING_ORDER},
#endif
#if UTE_MODULE_SPORT_AEROBICS_SUPPORT
    {.sportMode = SPORT_TYPE_AEROBICS, .sportOnOff = UTE_MODULE_SPORT_AEROBICS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_AEROBICS_ORDER},
#endif
#if UTE_MODULE_SPORT_GROUP_TRAINING_SUPPORT
    {.sportMode = SPORT_TYPE_GROUP_TRAINING, .sportOnOff = UTE_MODULE_SPORT_GROUP_TRAINING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_GROUP_TRAINING_ORDER},
#endif
#if UTE_MODULE_SPORT_KENDO_SUPPORT
    {.sportMode = SPORT_TYPE_KENDO, .sportOnOff = UTE_MODULE_SPORT_KENDO_ON_OFF, .sportOrder = UTE_MODULE_SPORT_KENDO_ORDER},
#endif
#if UTE_MODULE_SPORT_LACROSSE_SUPPORT
    {.sportMode = SPORT_TYPE_LACROSSE, .sportOnOff = UTE_MODULE_SPORT_LACROSSE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_LACROSSE_ORDER},
#endif
#if UTE_MODULE_SPORT_ROLLING_SUPPORT
    {.sportMode = SPORT_TYPE_ROLLING, .sportOnOff = UTE_MODULE_SPORT_ROLLING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_ROLLING_ORDER},
#endif
#if UTE_MODULE_SPORT_WRESTLING_SUPPORT
    {.sportMode = SPORT_TYPE_WRESTLING, .sportOnOff = UTE_MODULE_SPORT_WRESTLING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_WRESTLING_ORDER},
#endif
#if UTE_MODULE_SPORT_FENCING_SUPPORT
    {.sportMode = SPORT_TYPE_FENCING, .sportOnOff = UTE_MODULE_SPORT_FENCING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_FENCING_ORDER},
#endif
#if UTE_MODULE_SPORT_SOFTBALL_SUPPORT
    {.sportMode = SPORT_TYPE_SOFTBALL, .sportOnOff = UTE_MODULE_SPORT_SOFTBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SOFTBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_SINGLE_BAR_SUPPORT
    {.sportMode = SPORT_TYPE_SINGLE_BAR, .sportOnOff = UTE_MODULE_SPORT_SINGLE_BAR_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SINGLE_BAR_ORDER},
#endif
#if UTE_MODULE_SPORT_PARALLEL_BARS_SUPPORT
    {.sportMode = SPORT_TYPE_PARALLEL_BARS, .sportOnOff = UTE_MODULE_SPORT_PARALLEL_BARS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_PARALLEL_BARS_ORDER},
#endif
#if UTE_MODULE_SPORT_ROLLER_SKATING_SUPPORT
    {.sportMode = SPORT_TYPE_ROLLER_SKATING, .sportOnOff = UTE_MODULE_SPORT_ROLLER_SKATING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_ROLLER_SKATING_ORDER},
#endif
#if UTE_MODULE_SPORT_HULA_HOOP_SUPPORT
    {.sportMode = SPORT_TYPE_HULA_HOOP, .sportOnOff = UTE_MODULE_SPORT_HULA_HOOP_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HULA_HOOP_ORDER},
#endif
#if UTE_MODULE_SPORT_DARTS_SUPPORT
    {.sportMode = SPORT_TYPE_DARTS, .sportOnOff = UTE_MODULE_SPORT_DARTS_ON_OFF, .sportOrder = UTE_MODULE_SPORT_DARTS_ORDER},
#endif
#if UTE_MODULE_SPORT_PICKLEBALL_SUPPORT
    {.sportMode = SPORT_TYPE_PICKLEBALL, .sportOnOff = UTE_MODULE_SPORT_PICKLEBALL_ON_OFF, .sportOrder = UTE_MODULE_SPORT_PICKLEBALL_ORDER},
#endif
#if UTE_MODULE_SPORT_HIIT_SUPPORT
    {.sportMode = SPORT_TYPE_HIIT, .sportOnOff = UTE_MODULE_SPORT_HIIT_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HIIT_ORDER},
#endif
#if UTE_MODULE_SPORT_SHOOTING_SUPPORT
    {.sportMode = SPORT_TYPE_SHOOTING, .sportOnOff = UTE_MODULE_SPORT_SHOOTING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SHOOTING_ORDER},
#endif
#if UTE_MODULE_SPORT_JUDO_SUPPORT
    {.sportMode = SPORT_TYPE_JUDO, .sportOnOff = UTE_MODULE_SPORT_JUDO_ON_OFF, .sportOrder = UTE_MODULE_SPORT_JUDO_ORDER},
#endif
#if UTE_MODULE_SPORT_TRAMPOLINE_SUPPORT
    {.sportMode = SPORT_TYPE_TRAMPOLINE, .sportOnOff = UTE_MODULE_SPORT_TRAMPOLINE_ON_OFF, .sportOrder = UTE_MODULE_SPORT_TRAMPOLINE_ORDER},
#endif
#if UTE_MODULE_SPORT_SKATEBOARDING_SUPPORT
    {.sportMode = SPORT_TYPE_SKATEBOARDING, .sportOnOff = UTE_MODULE_SPORT_SKATEBOARDING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SKATEBOARDING_ORDER},
#endif
#if UTE_MODULE_SPORT_HOVERBOARD_SUPPORT
    {.sportMode = SPORT_TYPE_HOVERBOARD, .sportOnOff = UTE_MODULE_SPORT_HOVERBOARD_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HOVERBOARD_ORDER},
#endif
#if UTE_MODULE_SPORT_BLADING_SUPPORT
    {.sportMode = SPORT_TYPE_BLADING, .sportOnOff = UTE_MODULE_SPORT_BLADING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BLADING_ORDER},
#endif
#if UTE_MODULE_SPORT_PARKOUR_SUPPORT
    {.sportMode = SPORT_TYPE_PARKOUR, .sportOnOff = UTE_MODULE_SPORT_PARKOUR_ON_OFF, .sportOrder = UTE_MODULE_SPORT_PARKOUR_ORDER},
#endif
#if UTE_MODULE_SPORT_DIVING_SUPPORT
    {.sportMode = SPORT_TYPE_DIVING, .sportOnOff = UTE_MODULE_SPORT_DIVING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_DIVING_ORDER},
#endif
#if UTE_MODULE_SPORT_SURFING_SUPPORT
    {.sportMode = SPORT_TYPE_SURFING, .sportOnOff = UTE_MODULE_SPORT_SURFING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SURFING_ORDER},
#endif
#if UTE_MODULE_SPORT_SNORKELING_SUPPORT
    {.sportMode = SPORT_TYPE_SNORKELING, .sportOnOff = UTE_MODULE_SPORT_SNORKELING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_SNORKELING_ORDER},
#endif
#if UTE_MODULE_SPORT_PULL_UP_SUPPORT
    {.sportMode = SPORT_TYPE_PULL_UP, .sportOnOff = UTE_MODULE_SPORT_PULL_UP_ON_OFF, .sportOrder = UTE_MODULE_SPORT_PULL_UP_ORDER},
#endif
#if UTE_MODULE_SPORT_PUSH_UP_SUPPORT
    {.sportMode = SPORT_TYPE_PUSH_UP, .sportOnOff = UTE_MODULE_SPORT_PUSH_UP_ON_OFF, .sportOrder = UTE_MODULE_SPORT_PUSH_UP_ORDER},
#endif
#if UTE_MODULE_SPORT_PLANKING_SUPPORT
    {.sportMode = SPORT_TYPE_PLANKING, .sportOnOff = UTE_MODULE_SPORT_PLANKING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_PLANKING_ORDER},
#endif
#if UTE_MODULE_SPORT_ROCK_CLIMBING_SUPPORT
    {.sportMode = SPORT_TYPE_ROCK_CLIMBING, .sportOnOff = UTE_MODULE_SPORT_ROCK_CLIMBING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_ROCK_CLIMBING_ORDER},
#endif
#if UTE_MODULE_SPORT_HIGHTJUMP_SUPPORT
    {.sportMode = SPORT_TYPE_HIGHTJUMP, .sportOnOff = UTE_MODULE_SPORT_HIGHTJUMP_ON_OFF, .sportOrder = UTE_MODULE_SPORT_HIGHTJUMP_ORDER},
#endif
#if UTE_MODULE_SPORT_BUNGEE_JUMPING_SUPPORT
    {.sportMode = SPORT_TYPE_BUNGEE_JUMPING, .sportOnOff = UTE_MODULE_SPORT_BUNGEE_JUMPING_ON_OFF, .sportOrder = UTE_MODULE_SPORT_BUNGEE_JUMPING_ORDER},
#endif
#if UTE_MODULE_SPORT_LONGJUMP_SUPPORT
    {.sportMode = SPORT_TYPE_LONGJUMP, .sportOnOff = UTE_MODULE_SPORT_LONGJUMP_ON_OFF, .sportOrder = UTE_MODULE_SPORT_LONGJUMP_ORDER},
#endif
#if UTE_MODULE_SPORT_MARATHON_SUPPORT
    {.sportMode = SPORT_TYPE_MARATHON, .sportOnOff = UTE_MODULE_SPORT_MARATHON_ON_OFF, .sportOrder = UTE_MODULE_SPORT_MARATHON_ORDER},
#endif
};

/**
*@brief  读取config
*@details
*@author        xjc
*@date       2022-03-30
*/
void uteModuleSportReadHundredSportConfig(void)
{
    void *file;
    /*! 运动列表排序数据 xjc, 2022-03-29  */
    uint16_t sportSortSize = sizeof(ute_sports_list_param_t)*UTE_MODULE_SPORT_MAX_SPORT_NUM;
    ute_sports_list_param_t *readBuffSportList = (ute_sports_list_param_t *)uteModulePlatformMemoryAlloc(sportSortSize);
    // init
    memcpy(readBuffSportList,uteModuleSportDefaultSort,sportSortSize);
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_HUNDRED_SPORT_SORT_DATA, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readBuffSportList[0],sportSortSize);
        uteModuleFilesystemCloseFile(file);
    }
    memcpy(&uteModuleSprotData.sportSort.sportListData[0],readBuffSportList,sportSortSize);
    uteModulePlatformMemoryFree(readBuffSportList);
#if 0//UTE_MODULE_LOG_SUPPORT
    for (uint8_t i = 0; i < UTE_MODULE_SPORT_MAX_SPORT_NUM; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,%d,sportMode=%d,sportOnOff=%d,sportOrder=%d", __func__,i,uteModuleSprotData.sportSort.sportListData[i].sportMode,uteModuleSprotData.sportSort.sportListData[i].sportOnOff,uteModuleSprotData.sportSort.sportListData[i].sportOrder);
    }
#endif
}

/**
*@brief  保存config
*@details
*@author        xjc
*@date       2022-03-30
*/
void uteModuleSportSaveHundredSportConfig(void)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s", __func__);
    void *file;
    /*! 保存到文件xjc, 2022-03-30*/
    uint16_t sportSortSize = sizeof(ute_sports_list_param_t)*UTE_MODULE_SPORT_MAX_SPORT_NUM;
    ute_sports_list_param_t *writeBuffSportList = (ute_sports_list_param_t *)uteModulePlatformMemoryAlloc(sportSortSize);
    memcpy(writeBuffSportList,&uteModuleSprotData.sportSort.sportListData[0],sportSortSize);
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_HUNDRED_SPORT_SORT_DATA, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writeBuffSportList[0],sportSortSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(writeBuffSportList);
}

/**
*@brief     根据数组序号获取实际运动
*@details
*@param[in]   uint8_t sportOrder 传入当前要查找第几个运动
*@return     返回在列表中的序号
*@author        xjc
*@date       2022-03-30
*/
int uteModuleSportFindHundredSportIndex(uint8_t sportOrder)
{
    uint8_t tempSportOrder = sportOrder+1; //序号 = 1,2,3...,从1开始
    if (tempSportOrder > UTE_MODULE_SPORT_MAX_SPORT_NUM)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,ERROR, sportOrder out of range!", __func__);
        return 0;
    }
    for (uint8_t i = 0; i < UTE_MODULE_SPORT_MAX_SPORT_NUM; i++)
    {
        if (uteModuleSprotData.sportSort.sportListData[i].sportOrder == tempSportOrder)
        {
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,success, sportOrder = %d, sportIndex = %d!", __func__,tempSportOrder,i);
            return i;
        }
    }
    return 0;
}

/**
*@brief     根据实际运动获取数组序号
*@details
*@param[in]   sport_mode_type sportMode 传入当前运动模式
*@param[in]   sport_mode_type *sportList 传入界面运动的数组
*@return     返回在列表中的序号
*@author        xjc
*@date       2022-03-30
*/
uint8_t uteModuleSportGetRealIndex(uint8_t sportMode, const uint8_t *sportList)
{
    for (uint8_t i = 0; i < UTE_MODULE_SPORT_MAX_SPORT_NUM; i++)
    {
        if (sportList[i] == sportMode)
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,success, sportMode = %d", __func__,i);
            return i;
        }
    }
    return 0;
}

/**
*@brief  获取界面显示运动数量
*@details
*@return     返回一共显示多少运动
*@author        xjc
*@date       2022-03-30
*/
int uteModuleSportGetHundredSportValidNumber(void)
{
    int totalSportNumber = 0;
    for (uint8_t i = 0; i < UTE_MODULE_SPORT_MAX_SPORT_NUM; i++)
    {
        if (uteModuleSprotData.sportSort.sportListData[i].sportOnOff>0)
        {
            totalSportNumber++;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,totalSportNumber = %d", __func__,totalSportNumber);
    return totalSportNumber;
}
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
/**
 * @brief        获取APP修改运动列表标志
 * @details      用于界面更新显示
 * @author       Wang.Luo
 * @date         2023-01-06
 */
bool uteModuleSportGetAppUpdateTrainingListFlag(void)
{
    return uteModuleSprotData.sportSort.appUpdateTrainingListFlag;
}
/**
 * @brief        设置APP修改运动列表标志
 * @details      更新显示后需清除标志
 * @author       Wang.Luo
 * @date         2023-01-06
 */
void uteModuleSportSetAppUpdateTrainingListFlag(bool isUpdate)
{
    uteModuleSprotData.sportSort.appUpdateTrainingListFlag = isUpdate;
}
#endif
/**
*@brief        开始发送百种数据
*@details
*@author       xjc
*@date       2022-03-30
*/
void uteModuleSportSendHundredData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t sendSize = 0;
    uint16_t sendPerSportNum = ((sendParam->currSendMtuSize - 4) / 3); //每个包发送多少个运动数据
    uint16_t responseBufferSize = 4 + (sendPerSportNum * 3);           //每个包发送的运动数据需要开多大的buffer
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sendPerSportNum =%d,responseBufferSize =%d", __func__, sendPerSportNum, responseBufferSize);
    uint8_t *response = uteModulePlatformMemoryAlloc(responseBufferSize);
    memset(response,0,responseBufferSize);
    response[0] = CMD_SPORT_MODE_AND_SPORT_HEART_RATE;
    response[1] = 0x48;//自定义运动列表显示功能
    response[2] = 0xAA;//查询指令
    response[3] = sendParam->currSendMinIndex;//第几个包
    sendSize = 4;
    if((sendParam->currSendFileSize - sendParam->currSendFileIndex) >= sendPerSportNum)
    {
        for (uint8_t i = 0; i < sendPerSportNum; i++)
        {
            response[4+((i*3)+0)] = uteModuleSprotData.sportSort.sportListData[i+sendParam->currSendFileIndex].sportMode;
            response[4+((i*3)+1)] = uteModuleSprotData.sportSort.sportListData[i+sendParam->currSendFileIndex].sportOnOff;
            response[4+((i*3)+2)] = uteModuleSprotData.sportSort.sportListData[i+sendParam->currSendFileIndex].sportOrder;
            sendSize += 3;
            // UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,30 Index=%d,sportMode=%d,sportOnOff=%d,sportOrder=%d,", __func__,sendParam->currSendFileIndex,response[4+((i*3)+0)],response[4+((i*3)+1)],response[4+((i*3)+2)]);
        }
        sendParam->currSendFileIndex += sendPerSportNum;
    }
    else if((sendParam->currSendFileSize - sendParam->currSendFileIndex) > 0)
    {
        for (uint8_t i = 0; i < (sendParam->currSendFileSize - sendParam->currSendFileIndex); i++)
        {
            response[4+((i*3)+0)] = uteModuleSprotData.sportSort.sportListData[i+sendParam->currSendFileIndex].sportMode;
            response[4+((i*3)+1)] = uteModuleSprotData.sportSort.sportListData[i+sendParam->currSendFileIndex].sportOnOff;
            response[4+((i*3)+2)] = uteModuleSprotData.sportSort.sportListData[i+sendParam->currSendFileIndex].sportOrder;
            sendSize += 3;
            // UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,last Index=%d,sportMode=%d,sportOnOff=%d,sportOrder=%d,", __func__,sendParam->currSendFileIndex,response[4+((i*3)+0)],response[4+((i*3)+1)],response[4+((i*3)+2)]);
        }
        sendParam->currSendFileIndex = sendParam->currSendFileSize;
    }
    else
    {
        // end
        response[3] = 0xFD;
        response[4] = sendParam->crc;
        response[5] = (uteModuleSprotData.sportSort.minSportNum>>8)&0xFF;
        response[6] = (uteModuleSprotData.sportSort.minSportNum)&0xFF;
        response[7] = (uteModuleSprotData.sportSort.maxSportNum>>8)&0xFF;
        response[8] = (uteModuleSprotData.sportSort.maxSportNum)&0xFF;
        sendSize = 9;
    }
    if(response[3] == 0xFD)
    {
        uteApplicationCommonSyncDataTimerStop();
    }
    else
    {
        sendParam->currSendMinIndex++;
        uteModuleCrc8Bit(&sendParam->crc, &response[3],sendSize-3);
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currSendFileIndex=%d,currSendFileSize=%d,currSendMinIndex=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendFileSize,sendParam->currSendMinIndex);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sendSize=%d,sendParam->crc=%d", __func__,sendSize,sendParam->crc);
    uteModuleProfileBle50SendToPhone(&response[0],sendSize);
    uteModulePlatformMemoryFree(response);
}

/**
*@brief        准备开始发送百种运动数据
*@details
*@author       xjc
*@date       2022-03-30
*/
void uteModuleSportStartSendHundredData(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->currSendMinIndex = 0; //用作发送的包数量
    param->currSendFileIndex = 0; //已发送的运动数量
    param->currSendFileSize = UTE_MODULE_SPORT_MAX_SPORT_NUM; //总的运动数量
    param-> currSendMtuSize = uteApplicationCommonGetMtuSize();
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSportSendHundredData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
}

/**
*@brief        接收APP设置百种运动数据
*@details
*@author       xjc
*@date       2022-03-30
*/
void uteModuleSportSetHundredData(uint8_t*receive,uint8_t length)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
    uint8_t response[4];
    response[0] = receive[0];
    response[1] = receive[1];
    if(receive[2] == 0xFD)
    {
        if (receive[3] == uteModuleSprotData.sportSort.receiveCrcCheck)
        {
            response[2] = 0xFD;
            response[3] = uteModuleSprotData.sportSort.receiveCrcCheck;
            uteModuleProfileBle50SendToPhone(&response[0], 4);
            /*! 清理旧数据*/
            for (uint8_t i = 0; i < UTE_MODULE_SPORT_MAX_SPORT_NUM; i++)
            {
                uteModuleSprotData.sportSort.sportListData[i].sportOnOff = 0;
                uteModuleSprotData.sportSort.sportListData[i].sportOrder = 0;
            }
            /*! 处理接收到数据*/
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,.receiveSportBytesLen=%d", __func__, uteModuleSprotData.sportSort.receiveSportBytesLen);
            for (uint8_t i = 0; i < (uteModuleSprotData.sportSort.receiveSportBytesLen / 3); i++)
            {
                UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "receiveSportBytes[%d]= %d-%d-%d", i,uteModuleSprotData.sportSort.receiveSportBytes[(i * 3) + 0],uteModuleSprotData.sportSort.receiveSportBytes[(i * 3) + 1],uteModuleSprotData.sportSort.receiveSportBytes[(i * 3) + 2]);
                for (uint8_t j = 0; j < UTE_MODULE_SPORT_MAX_SPORT_NUM; j++)
                {
                    if (uteModuleSprotData.sportSort.sportListData[j].sportMode == uteModuleSprotData.sportSort.receiveSportBytes[(i * 3) + 0])
                    {
                        uteModuleSprotData.sportSort.sportListData[j].sportOnOff = uteModuleSprotData.sportSort.receiveSportBytes[(i * 3) + 1];
                        uteModuleSprotData.sportSort.sportListData[j].sportOrder = uteModuleSprotData.sportSort.receiveSportBytes[(i * 3) + 2];
                        break;
                    }
                }
            }
            /*! 数据校验通过保存最新数据*/
            uteModuleSportSaveHundredSportConfig();
            /*! 设置app更新运动列表标志，用于界面显示更新,wang.luo 2023-01-06 */
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
            uteModuleSportSetAppUpdateTrainingListFlag(true);
#endif
        }
        else
        {
            response[2] = 0xFF;
            uteModuleProfileBle50SendToPhone(&response[0], 3);
            /*! 数据校验失败恢复之前数据*/
            uteModuleSportReadHundredSportConfig();
        }
    }
    else
    {
        /*! 第一条数据*/
        if (receive[2] == 0)
        {
            uteModuleSprotData.sportSort.receiveCrcCheck = 0;
            uteModuleSprotData.sportSort.receiveSportBytesLen = 0;
            memset(&uteModuleSprotData.sportSort.receiveSportBytes[0], 0, (UTE_MODULE_SPORT_MAX_SPORT_NUM * 3));
        }
        /*! 数据校验*/
        uteModuleCrc8Bit(&uteModuleSprotData.sportSort.receiveCrcCheck, &receive[2], (length - 2));
        /*! 保存接收到数据*/
        memcpy(&uteModuleSprotData.sportSort.receiveSportBytes[uteModuleSprotData.sportSort.receiveSportBytesLen], &receive[3], (length - 3));
        uteModuleSprotData.sportSort.receiveSportBytesLen += (length - 3);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,.receiveSportBytesLen=%d", __func__, uteModuleSprotData.sportSort.receiveSportBytesLen);
        if (uteModuleSprotData.sportSort.receiveSportBytesLen > (UTE_MODULE_SPORT_MAX_SPORT_NUM * 3))
        {
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,ERROR,receive sport num not current!", __func__);
            uteModuleSprotData.sportSort.receiveCrcCheck = 0;
        }
        /*! BLE收到每条指令以后返回指令0xFD48FB+n(序号)*/
        response[2] = 0xFB;
        response[3] = receive[2];
        uteModuleProfileBle50SendToPhone(&response[0], 4);
    }
}

/**
*@brief      处理百种运动支持协议
*@details
*@author       xjc
*@date       2022-03-29
*/
void uteModuleSportHundredSportCmd(uint8_t*receive,uint8_t length)
{
    if(receive[2] == 0xAA)
    {
        /*! 查询指令：手机端查询当前运动列表显示的内容，xjc 2022-03-29*/
        uteModuleSportStartSendHundredData();
    }
    else
    {
        /*! 接收自定义运动列表显示功能，xjc 2022-03-29*/
        uteModuleSportSetHundredData(receive,length);
    }
}
#endif

/**
*@brief  读取config
*@details
*@author        zn.zeng
*@date        2021-08-26
*/
void uteModuleSportReadConfig(void)
{
    void *file;
    uint8_t readbuff[17];
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    readbuff[0] = DEFAULT_SEDENTARY_NOTIFY_OPNE;
    readbuff[1] = DEFAULT_SEDENTARY_NOTIFY_TIME_INTERVAL;
    readbuff[2] = (DEFAULT_SEDENTARY_NOTIFY_START_TIME>>8)&0xff;
    readbuff[3] = DEFAULT_SEDENTARY_NOTIFY_START_TIME&0xff;;
    readbuff[4] = (DEFAULT_SEDENTARY_NOTIFY_END_TIME>>8)&0xff;
    readbuff[5] = DEFAULT_SEDENTARY_NOTIFY_END_TIME&0xff;
    readbuff[6] = DEFAULT_HAND_SCREEN_ON_OPEN;
    readbuff[7] = (DEFAULT_STEP_TARGET_CNT>>24)&0xff;
    readbuff[8] = (DEFAULT_STEP_TARGET_CNT>>16)&0xff;
    readbuff[9] = (DEFAULT_STEP_TARGET_CNT>>8)&0xff;
    readbuff[10] = DEFAULT_STEP_TARGET_CNT&0xff;
    readbuff[11] = 0;
    readbuff[12] = DEFAULT_NOON_SLEEP_OPNE;
    readbuff[13] = (DEFAULT_NOON_SLEEP_START_TIME>>8)&0xff;
    readbuff[14] = DEFAULT_NOON_SLEEP_START_TIME&0xff;;
    readbuff[15] = (DEFAULT_NOON_SLEEP_END_TIME>>8)&0xff;
    readbuff[16] = DEFAULT_NOON_SLEEP_END_TIME&0xff;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SPORTINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file,&readbuff[0],17);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleSprotData.sedentary.isOpen = readbuff[0];
    uteModuleSprotData.sedentary.setMin= readbuff[1];
    uteModuleSprotData.sedentary.startPeriodHour= readbuff[2];
    uteModuleSprotData.sedentary.startPeriodMin= readbuff[3];
    uteModuleSprotData.sedentary.endPeriodHour= readbuff[4];
    uteModuleSprotData.sedentary.endPeriodMin= readbuff[5];
    uteModuleSprotData.isOpenHandScreenOn= readbuff[6];
    uteModuleSprotData.stepsTargetCnt = readbuff[7]<<24|readbuff[8]<<16|readbuff[9]<<8|readbuff[10];
    uteModuleSprotData.isTargetCompleted = readbuff[11];
    uteModuleSprotData.sedentary.isOpenNoonSleep = readbuff[12];
#if UTE_MODULE_SEDENTARY_NOTIFY_SET_NOONSLEEP_TIME
    uteModuleSprotData.sedentary.noonSleepStartTime = readbuff[13]<<8|readbuff[14];
    uteModuleSprotData.sedentary.noonSleepEndTime = readbuff[15]<<8|readbuff[16];
#endif
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = uteModuleSprotData.isOpenHandScreenOn;
    //quick.isSedentary = uteModuleSprotData.sedentary.isOpen;
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,.isOpen=%d,setMin=%d,startPeriodHour=%d,startPeriodMin=%d", __func__,uteModuleSprotData.sedentary.isOpen,uteModuleSprotData.sedentary.setMin, uteModuleSprotData.sedentary.startPeriodHour,uteModuleSprotData.sedentary.startPeriodMin);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,endPeriodHour=%d,endPeriodMin=%d", __func__,uteModuleSprotData.sedentary.endPeriodHour,uteModuleSprotData.sedentary.endPeriodMin);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,isOpenHandScreenOn=%d,stepsTargetCnt=%d", __func__,uteModuleSprotData.isOpenHandScreenOn,uteModuleSprotData.stepsTargetCnt);

#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
    /*! 开机读取当天有步数运动的步数，距离跟卡路里 xjc, 2022-01-17*/
    // uteModuleSportReadTodayAllSportDataConfig(time);
    uteModuleSprotData.updateTodayAllSportData = true;
#endif
    /*! 开机恢复当前小时步数 zn.zeng, 2021-10-23  */
    uint8_t path[40];
    uint16_t buffSize = 12*24+4;
    uint8_t *readBuffOneDay = uteModulePlatformMemoryAlloc(buffSize);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,%04d-%02d-%02d,%02d:%02d:%02d", __func__,time.year,time.month,time.day,time.hour,time.min,time.sec);
    memset(&path[0],0,sizeof(path));
    sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR,time.year,time.month,time.day);
    // read
    memset(&readBuffOneDay[0],0,buffSize);
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readBuffOneDay[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleSprotData.stepSleepData.currentHourStepCnt = (readBuffOneDay[4+time.hour*12]<<8|readBuffOneDay[4+time.hour*12+1]);
    uteModuleSprotData.stepSleepData.current_hour_run_stepCnt = (readBuffOneDay[4+time.hour*12+5]<<8|readBuffOneDay[4+time.hour*12+6]);
    uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt = (readBuffOneDay[4+time.hour*12+10]<<8|readBuffOneDay[4+time.hour*12+11]);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currentHourStepCnt=%d,current_hour_run_stepCnt=%d,current_hour_walk_stepCnt=%d", __func__,uteModuleSprotData.stepSleepData.currentHourStepCnt,uteModuleSprotData.stepSleepData.current_hour_run_stepCnt,uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt);
    mStepSleepParam.currentHourStepCnt = uteModuleSprotData.stepSleepData.currentHourStepCnt;
    mStepSleepParam.current_hour_run_stepCnt = uteModuleSprotData.stepSleepData.current_hour_run_stepCnt;
    mStepSleepParam.current_hour_walk_stepCnt = uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt;
    uteModulePlatformMemoryFree(readBuffOneDay);

#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    /*! 运动列表排序数据 xjc, 2022-03-29  */
    uteModuleSportReadHundredSportConfig();
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCwmReadCurrHourStepDataFromFs();
#endif
}

/**
*@brief        往算法更新参数
*@details
*@author       xjc
*@date       2022-03-21
*/
void uteModuleSportUpdateStepSleepParam(void)
{
    bool isMoreSporting = false;
    if(uteModuleSprotData.moreSportData.status!=ALL_SPORT_STATUS_CLOSE)
    {
        isMoreSporting = true;
    }
    if(isMoreSporting)
    {
        mStepSleepParam = uteModuleSprotData.stepSleepData;
    }
    else
    {
        memcpy(&uteModuleSprotData.stepSleepData,&mStepSleepParam,sizeof(ute_step_sleep_param_t));
    }
}

/**
*@brief        往算法更新参数
*@details
*@param[in]   传入时间数据
*@author       zn.zeng
*@date       2021-08-05
*/
void uteModuleSportUpdateAlgoParam(ute_module_systemtime_time_t time)
{
    struct prf_date_time oldTime;
    oldTime.year = time.year;
    oldTime.month = time.month;
    oldTime.day = time.day;
    oldTime.hour = time.hour;
    oldTime.min = time.min;
    oldTime.sec = time.sec;
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    uteUpdateStepSleepData(&oldTime);
    uteModuleSportUpdateStepSleepParam();
    uint8_t state = getStepOrSleepStateWhetherSwitch();
    if((state==1)&&(uteModuleSprotData.stepType == STEP_TYPE_SLEEP))
    {
        uteModuleSportStepTypeSetStep();
    }
    if((state==2)&&(uteModuleSprotData.stepType == STEP_TYPE_STEP))
    {
        uteModuleSportStepTypeSetSleep();
        mStepSleepParam.currentHourSleepTurnCnt =0;
    }
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleSprotData.stepSleepData.currentHourStepCnt = uteModuleCwmGetTodayCurrHourStep();
    UTE_MODULE_LOG(UTE_LOG_CYWEE_MOTION_LVL,"%s, currentHourStepCnt = %d", __func__,uteModuleSprotData.stepSleepData.currentHourStepCnt);
#endif
}
/**
*@brief        检查摇一摇拍照状态
*@details
*@param[in]
*@author       zn.zeng
*@date       2021-08-06
*/
void uteModuleSportCheckTakePicture(void)
{
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    if (getTakingPicturesResult()&&uteModuleSprotData.isTakingPictures)
    {
        disableTakingPicturesState();
        uteDrvGsensorCommonClearFifo();
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_TAKE_PICTURE_NOTIFY,0);
    }
#endif
}
/**
*@brief        手环端主动退出拍照状态
*@details
*@author       xjc
*@date       2022-04-25
*/
void uteModuleSportExitTakePicture(void)
{
    uint8_t exitPhoto[2] = {CMD_SEND_KEYCODE, 0x0F};
    uteModuleProfileBleSendToPhone(&exitPhoto[0], 2);
    uteModuleSportSetTakePictureEnable(false);
}
/**
*@brief        查看是否在摇一摇拍照状态
*@details
*@author       xjc
*@date       2022-04-25
*/
bool uteModuleSportIsTakePicture(void)
{
    return uteModuleSprotData.isTakingPictures;

}
/**
*@brief        久坐提醒处理
*@details
*@param[in]
*@author       zn.zeng
*@date       2021-08-06
*/
void uteModuleSportSedentaryHandler(ute_module_systemtime_time_t time)
{
    bool needNotify = false;
    /*! 运动中久坐提醒时间清零, xjc 2022-03-21*/
    if(uteModuleSprotData.moreSportData.status!=ALL_SPORT_STATUS_CLOSE)
    {
        uteModuleSprotData.sedentary.sedentaryRemindSecCnt = 0;
        return;
    }
    if(uteModuleSprotData.sedentary.isOpen)
    {
        uint32_t oneDaySecond = time.hour*3600+time.min*60+time.sec;
        uint32_t startTimeSecond = uteModuleSprotData.sedentary.startPeriodHour*3600+uteModuleSprotData.sedentary.startPeriodMin*60;
        uint32_t endTimeSecond = uteModuleSprotData.sedentary.endPeriodHour*3600+uteModuleSprotData.sedentary.endPeriodMin*60;
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,startTimeSecond=%d,endTimeSecond=%d,sedentaryRemindSecCnt=%d", __func__,startTimeSecond,endTimeSecond,uteModuleSprotData.sedentary.sedentaryRemindSecCnt);
#if UTE_MODULE_SEDENTARY_NOTIFY_SET_NOONSLEEP_TIME
        if(uteModuleSprotData.sedentary.isOpenNoonSleep&&(oneDaySecond>=uteModuleSprotData.sedentary.noonSleepStartTime*60)&&(oneDaySecond<=uteModuleSprotData.sedentary.noonSleepEndTime*60))
#else
        if(uteModuleSprotData.sedentary.isOpenNoonSleep&&(oneDaySecond>=12*3600)&&(oneDaySecond<=14*3600))
#endif
        {
            needNotify = false;
        }
        else
        {
            if(startTimeSecond<endTimeSecond)   // 8:00 - 18 :00
            {
                if( (oneDaySecond>=startTimeSecond) && (oneDaySecond<=endTimeSecond) )
                {
                    needNotify =true;
                }
                else
                {
                    needNotify =false;
                }
            }
            else     // 18:00 -8:00
            {
                if((startTimeSecond<=oneDaySecond)
                   || (endTimeSecond>=oneDaySecond) )
                {
                    needNotify =true;
                }
                else
                {
                    needNotify =false;
                }
            }
        }
    }
    if(uteModuleSprotData.sedentary.isOpen&&needNotify)
    {

#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT

        if(getSedentaryRemindActivityStatus()==SedentaryRemind_NEED_REMIND)
        {
            uteModuleSprotData.sedentary.sedentaryRemindSecCnt++;
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sedentaryRemindSecCnt=%d,needNotify=%d,stepType=%d", __func__,uteModuleSprotData.sedentary.sedentaryRemindSecCnt,needNotify,uteModuleSprotData.stepType);

            if(uteModuleSprotData.sedentary.sedentaryRemindSecCnt>=(uteModuleSprotData.sedentary.setMin*60))
            {
                uteModuleSprotData.sedentary.sedentaryRemindSecCnt = 0;

                /*||getSleepTotalMinute()<30  add by pcm 2022-10-17
                添加判断条件，判断睡眠模式下，睡眠数据总时间小于30分钟时候，还是继续提示久坐，
                解决之前早上测试久坐无提醒和晚上测试久坐大概率无提醒的问题*/
                if(uteModuleSprotData.stepType == STEP_TYPE_STEP||getSleepTotalMinute()<30)
                {
                    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SEDENTARY_NOTIFY,0);
                }
                enableSedentaryRemind();
            }
        }
        else
        {
            uteModuleSprotData.sedentary.sedentaryRemindSecCnt = 0;
        }
#endif
    }
    else
    {
        uteModuleSprotData.sedentary.sedentaryRemindSecCnt = 0;
    }
}

/**
*@brief        切换到下一小时
*@details
*@param[in]
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleSportBeforeGotoNextHour(ute_module_systemtime_time_t time)
{
    if((time.min==59)&&(time.sec==59))
    {
        uteModuleSportSaveStepData();
        //clear
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s", __func__);
#if !UTE_MODULE_SPORT_CLEAR_STEP_WHEN_HOUR_SUPPORT
        memset(&uteModuleSprotData.stepSleepData,0,sizeof(ute_step_sleep_param_t));
        memset(&mStepSleepParam,0,sizeof(ute_step_sleep_param_t));
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
        uteModuleCwmGotoNextHourCallback(time.hour);
        if(time.hour == 23)
        {
            void cwmPedometerReSet(void);
            cwmPedometerReSet();    //清除所有数据
        }
#endif
    }
#if UTE_MODULE_SPORT_CLEAR_STEP_WHEN_HOUR_SUPPORT
    if((time.min==0)&&(time.sec==0))
    {
        memset(&uteModuleSprotData.stepSleepData,0,sizeof(ute_step_sleep_param_t));
        memset(&mStepSleepParam,0,sizeof(ute_step_sleep_param_t));
    }
#endif
}
/**
*@brief        步数目标提醒
*@details
*@author       zn.zeng
*@date       2021-11-30
*/
void uteModuleSportStepTargetHandler(void)
{
    uint32_t second = uteApplicationCommonGetSystemPowerOnSecond();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    if ((!uteModuleSprotData.isTargetCompleted) &&
        (totalStepCnt >= uteModuleSprotData.stepsTargetCnt) &&
        (second > 9))
    {
        uteModuleSprotData.isTargetCompleted = true;
        /*! 保存到文件xjc, 2022-01-05*/
        void *file;
        uint8_t buff[12];
        if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SPORTINFO, &file, FS_O_RDONLY|FS_O_WRONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &buff[0], 12);
            buff[11] = uteModuleSprotData.isTargetCompleted;
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemWriteData(file,&buff[0],12);
            uteModuleFilesystemCloseFile(file);
        }
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
        memset(&uteModuleSprotData.todayTargetNotify.todayTargetNotifyType,0,sizeof(ute_module_today_target_notify_type_t));
        uteModuleSprotData.todayTargetNotify.todayTargetNotifyType.isTodayStepTargetNotify = true;
#endif
#if QUICK_SWITCH_GOAL_REACH_SUPPORT
        ute_quick_switch_t quick;
        uteApplicationCommonGetQuickSwitchStatus(&quick);
        if(quick.isGoalReach)
#endif
        {
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
            if(uteModuleSprotData.todayTargetNotify.isTodayStepTargetNotifyOpen)
#endif
            {
                // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TARGET_COMPLETED_ID);       //日常步数目标完成弹窗
                sys_cb.cover_index = REMIND_COVER_GOAL;
                sys_cb.remind_tag = true;
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
            }
        }
    }
}
/**
*@brief        换天清除目标提醒标志
*@details
*@param[in]
*@author      xjc
*@date       2022-01-05
*/
void uteModuleSportDayStartClearStepTarget(ute_module_systemtime_time_t time)
{
    if(time.hour == 0 && time.min == 0 && time.sec == 0)
    {
        uteModuleSprotData.isTargetCompleted = false;
        /*! 保存到文件xjc, 2022-01-05*/
        void *file;
        uint8_t buff[12];
        if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SPORTINFO, &file, FS_O_RDONLY|FS_O_WRONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &buff[0], 12);
            buff[11] = uteModuleSprotData.isTargetCompleted;
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemWriteData(file,&buff[0],12);
            uteModuleFilesystemCloseFile(file);
        }
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
        uteModuleSportReadWeekDayStep();
#endif
    }
}
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
/**
*@brief  清除当天运动步数缓存
*@detailss
*@author        xjc
*@date       2022-03-30
*/
void uteModuleSportClearTodayAllSportData(void)
{
    memset(&uteModuleSprotData.todaySportWithStepsData, 0, sizeof(ute_sports_today_with_step_data_t));
#if UTE_MODULE_SPORT_TIME_TARGET_NOTIFY_SUPPORT
    uteModuleSprotData.todayTargetNotify.currentDayAllSportTimeSec = 0;
#endif
}

/**
*@brief  读取当天运动步数config
*@details
*@param[in] ute_module_systemtime_time_t time, 输入读取的日期
*@param[out] ute_sports_today_with_step_data_t *stepData, 计步数据输出数据指针
*@param[out] ute_module_target_notify_data_t *targetNotifyData，目标提醒数据输出指针
*@author        xjc
*@date       2022-03-30
*/
void uteModuleSportReadTodayAllSportDataConfig(ute_module_systemtime_time_t time,ute_sports_today_with_step_data_t *stepData,ute_module_target_notify_data_t *targetNotifyData)
{
    void *file;
    uint8_t path[40];
    memset(&path[0],0,sizeof(path));
    uint8_t datePath[12];
    memset(&datePath[0],0,sizeof(datePath));
    uint8_t readBuff[33];
    memset(&readBuff[0],0,sizeof(readBuff));
    uint8_t headBuff[33];
    memset(&headBuff[0],0,sizeof(headBuff));
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    sprintf((char *)&datePath[0],"%04d%02d%02d",time.year,time.month,time.day);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR, dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    uteModuleSportClearTodayAllSportData();
    for (uint8_t i = 0; i < dirInfo->filesCnt; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,datePath=%s", __func__,&datePath[0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,dirInfo->filesName[%d]=%s", __func__,i,&dirInfo->filesName[i][0]);
        if(memcmp(&datePath[0],&dirInfo->filesName[i][0],8)==0)
        {
            sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR,&dirInfo->filesName[i][0]);
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,read file=%s", __func__,&path[0]);
            if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
            {
                uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
                uteModuleFilesystemReadData(file,&headBuff[0],33);
                uteModuleFilesystemCloseFile(file);
                UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,readData.sportsType=%d", __func__,headBuff[1]);
                if ((headBuff[1] == SPORT_TYPE_RUNNING) /*跑步 */
                    || (headBuff[1] == SPORT_TYPE_CLIMBING) /*登山 */
                    || (headBuff[1] == SPORT_TYPE_WALKING) /*健走 */
                    || (headBuff[1] == SPORT_TYPE_TREADMILL) /*跑步机 */
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
                    || (headBuff[1] == SPORT_TYPE_INDOOR_WALK) /*室内走路 */
                    || (headBuff[1] == SPORT_TYPE_STEP_TRAINING) /*踏步 */
                    || (headBuff[1] == SPORT_TYPE_OUTDOOR_WALK) /*户外健走 */
                    || (headBuff[1] == SPORT_TYPE_HIKING) /*徒步 */
                    || (headBuff[1] == SPORT_TYPE_INDOOR_RUN) /*室内跑步 */
                    || (headBuff[1] == SPORT_TYPE_PARKOUR) /*跑酷 */
                    || (headBuff[1] == SPORT_TYPE_MARATHON) /*马拉松 */
#endif
                   )
                {
                    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,readData-sport.step=%d,caloire=%d,distance=%d.%d", __func__,headBuff[16]<<24|headBuff[17]<<16|headBuff[18]<<8|headBuff[19],headBuff[22]<<8|headBuff[23],headBuff[24],headBuff[25]);
                    stepData->step += headBuff[16]<<24|headBuff[17]<<16|headBuff[18]<<8|headBuff[19];
                    stepData->caloire += headBuff[22]<<8|headBuff[23];
                    stepData->distance += (headBuff[24]*100 + headBuff[25]);//整数部分先放大100倍
                }
#if UTE_MODULE_SPORT_TODAY_ADD_KCAL_SPORT_DATA_SUPPORT
                else
                {
                    stepData->caloire += headBuff[22]<<8|headBuff[23];
                }
#endif
#if UTE_MODULE_SPORT_TIME_TARGET_NOTIFY_SUPPORT
                if (targetNotifyData != NULL)
                {
                    targetNotifyData->currentDayAllSportTimeSec += (headBuff[13] << 16 | headBuff[14] << 8 | headBuff[15]);
                }
#endif
            }
        }
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sport.step=%d,caloire=%d,distance=%d", __func__,stepData->step,stepData->caloire,stepData->distance);

    uteModulePlatformMemoryFree(dirInfo);
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
#endif

/**
*@brief        sport每秒函数
*@details       需要注册到主时间，每秒执行一次
*@author       zn.zeng
*@date       2021-07-27
*/
void uteModuleSportEverySecond(void)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL,"uteModuleSprotData.stepType=%d,updateTodayAllSportData=%d",uteModuleSprotData.stepType,uteModuleSprotData.updateTodayAllSportData);
    if(time.hour == 0 && time.min == 0 && (time.sec == 0 || time.sec == 1))
    {
        uteModuleSportClearTodayAllSportData();
    }
    else if(uteModuleSprotData.updateTodayAllSportData == true)
    {
        uteModuleSprotData.updateTodayAllSportData = false;
        /*! 读取当天有步数运动的步数，距离跟卡路里 xjc, 2022-01-17*/
        uteModuleSportReadTodayAllSportDataConfig(time,&uteModuleSprotData.todaySportWithStepsData,&uteModuleSprotData.todayTargetNotify);
#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
        uteModuleSportCalculatorEveryHourSportDataHandler(true);
#endif
    }
#endif
    if(!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    /*喝水提醒，定时提醒，不需要判是否佩戴*/
#if UTE_MODULE_DRINK_WATER_NOTIFY_SCREEN_SUPPORT
    uteModuleDrinkWaterHandler(time);
#endif
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
    uteModuleSportTodayTargetHandler();
#endif
    uteModuleSportBeforeGotoNextHour(time);
    uteModuleSportMoreSportsEverySecond(&time);
    uteModuleSportDayStartClearStepTarget(time);
#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
    uteModuleSportCalculatorEveryHourSportDataHandler(false);
#endif
    bool isStepping=false;
    if(uteModuleSprotData.stepType == STEP_TYPE_STEP)
    {
        isStepping = true;
    }
    uteModuleSleepDataInputSecond(&mStepSleepParam,time,isStepping);

#if (UTE_DRV_HEART_VCXX_NIGHT_OPTIMIZE_SLEEP_SUPPORT && !UTE_DRV_HEART_VCXX_REMAIN_POWER_SUPPORT)
    if (isStepping)
    {
        if(vc30fx_usr_get_work_status() && uteModuleHeartGetWorkMode() == WORK_MODE_WEAR)
        {
#if UTE_MODULE_HEART_SUPPORT
            uteModuleHeartStopSingleTesting(TYPE_WEAR);
#endif
        }
    }
    else
    {
        if(uteModuleSystemtimeIsNight() && !vc30fx_usr_get_work_status())
        {
#if UTE_MODULE_HEART_SUPPORT
            uteModuleHeartStartSingleTesting(TYPE_WEAR);
#endif
        }
    }
#endif

    uteModuleSportReadCurrDayStepCnt(time);
    if(uteModuleSprotData.stepType == STEP_TYPE_NONE)
    {
        return;
    }
    ute_ble_connect_state_t state;
    uteApplicationCommonGetBleConnectionState(&state);
    uteModuleSportUpdateAlgoParam(time);
    uteModuleSportStepTargetHandler();
    if (uteModuleSprotData.stepType == STEP_TYPE_STEP)
    {
        /*! 发送实时计步数据给应用端zn.zeng, 2021-08-05  */
        if((uteModuleSprotData.lastHourStepCnt!=uteModuleSprotData.stepSleepData.currentHourStepCnt)&&
           (state.connectedSecond>5))
        {
#if UTE_MODULE_SPORT_CLEAR_STEP_WHEN_HOUR_SUPPORT
            if(!((time.min==59)&&(time.sec==59)))
            {
                uteModuleSportSendRealTimeStepInfo();
                uteModuleSprotData.lastHourStepCnt = uteModuleSprotData.stepSleepData.currentHourStepCnt;
            }
#else
            uteModuleSportSendRealTimeStepInfo();
            uteModuleSprotData.lastHourStepCnt = uteModuleSprotData.stepSleepData.currentHourStepCnt;
#endif
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,lastHourStepCnt=%d",__func__,uteModuleSprotData.lastHourStepCnt);
        }
    }
#if UTE_MODULE_SPROT_ALGO_AUTO_SWITCH_SYSCLK_SUPPORT && UTE_LOG_STEP_LVL
    uint8_t cur_sys_clk = sys_clk_get();
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,cur_sys_clk=%d", __func__, cur_sys_clk);
#endif
    uteModuleSportCheckTakePicture();
    uteModuleSportSedentaryHandler(time);
}
/**
*@brief        gsensor 数据输入算法库
*@details
*@author       zn.zeng
*@date       2021-07-27
*/
AT(.com_text.gsensor)
void uteModuleSportInputDataBeforeAlgo(void)
{
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    int16_t x[UTE_DRV_GSENSOR_AXIS_DATA_MAX] = {0};
    int16_t y[UTE_DRV_GSENSOR_AXIS_DATA_MAX] = {0};
    int16_t z[UTE_DRV_GSENSOR_AXIS_DATA_MAX] = {0};
    uint8_t frameCnt = 0;
    if(uteModuleSprotData.stepType == STEP_TYPE_NONE)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,STEP_TYPE_NONE", __func__);
#if UTE_MODULE_SPROT_ALGO_AUTO_SWITCH_SYSCLK_SUPPORT
        uteModuleSprotData.switchSysclkCountdown = 0;
#endif
        return;
    }
    ute_drv_gsensor_common_axis_data_t *data = NULL;
    uteDrvGsensorCommonReadFifo(&data);
    uteDrvGsensorCommonClearFifo();
    frameCnt = data->frameCnt;
    ute_drv_gsensor_common_axis_bit_change_t axisBitChange;
    axisBitChange.inputXaxis = &data->accXaxis[0];
    axisBitChange.inputYaxis = &data->accYaxis[0];
    axisBitChange.inputZaxis = &data->accZaxis[0];
    axisBitChange.outputXaxis = &x[0];
    axisBitChange.outputYaxis = &y[0];
    axisBitChange.outputZaxis = &z[0];
    uteDrvGsensorCommonXYZaxisDataBitChange(&axisBitChange,frameCnt,GSENSOR_DATA_BIT_STEP);
#if UTE_MODULE_SPROT_ALGO_AUTO_SWITCH_SYSCLK_SUPPORT
    uint32_t accValueSum = 0;
    uint8_t accValueCount = 0;
    uint8_t accValueAvg = 0;
#endif
    for(int i=0; i<frameCnt; i++)
    {
        if(uteDrvMotorGetRunningStatus())
        {
            /* ellison add ,2022-Jun-07 16:8 马达振动产生的步数不要，振动时摇一摇拍照的干扰结果也不要*/
            break;
        }
        if(!((x[i]==0)&&(y[i]==0)&&(z[i]==0)))
        {
            uint16_t accValue =0;
            // if(i==0)
            // {
            //     UTE_MODULE_LOG(1, "%s,i=%02d,frameCnt=%02d,x=%d,y=%d,z=%d", __func__,i,frameCnt,x[i],y[i],z[i]);
            // }
            if(uteModuleSprotData.moreSportData.status==ALL_SPORT_STATUS_PAUSE)
            {
                /*! 运动暂停不走算法，不出运动值 ,xjc 2022-03-09*/
            }
            else
            {
                if((uteModuleSprotData.moreSportData.saveData.sportsType==SPORT_TYPE_JUMP_ROPE)&&(uteModuleSprotData.moreSportData.status!=ALL_SPORT_STATUS_CLOSE))
                {
                    app_jump_rope_algorithms_input_data(uteModuleSprotData.accRange,&x[i],&y[i],&z[i],&accValue);
                }
                else
                {
                    uint8_t accRateValue = 40;
                    uint8_t accRate = uteDrvGsensorCommonGetAccRate();
                    if(accRate==GSENSOR_RATE_20HZ)
                    {
                        accRateValue = 20;
                    }
                    else if(accRate==GSENSOR_RATE_40HZ)
                    {
                        accRateValue = 40;
                    }
                    else if(accRate==GSENSOR_RATE_100HZ)
                    {
                        accRateValue = 100;
                    }
#if UTE_MODULE_NEW_T02_SPORT_STEP_ALGORITHMS_SUPPORT
                    uteStepSleepAlgorithmsInputData(uteModuleSprotData.gsensorId,uteModuleSprotData.accRange,&x[i],&y[i],&z[i],frameCnt,i,&accValue,uteModuleSystemtimeIsSleepTime());
#else
                    uteStepSleepAlgorithmsInputData(uteModuleSprotData.gsensorId,uteModuleSprotData.accRange,&x[i],&y[i],&z[i],frameCnt,i,&accValue,uteModuleSystemtimeIsSleepTime(),accRateValue);
#endif
#if APP_DBG_GSENSOR_DATA
                    uteModuleSportSendGsensorDataToPhone(x[i],y[i],z[i]);
#endif
                }
            }
            uteHandScreenAlgorithmsInputData(&x[i],&y[i],&z[i],accValue);
#if UTE_MODULE_WATCH_ROTATE_SUPPORT
            if(uteModuleSprotData.isGsensorRotateTesting && (!uteDrvHeartVcxxIsWear()))//手表旋转中且未佩戴，传入三轴数据
            {
                uteRotateInputData(&x[i],&y[i],&z[i]);
            }
#endif
#if UTE_MODULE_SPROT_ALGO_AUTO_SWITCH_SYSCLK_SUPPORT
            accValueSum += accValue;
            accValueCount ++;
#endif
        }
    }
#if UTE_MODULE_SPROT_ALGO_AUTO_SWITCH_SYSCLK_SUPPORT
    accValueAvg = accValueCount > 0 ? accValueSum / accValueCount : 0;
    if (abs(accValueAvg - 127) > 10)
    {
        uteModuleSprotData.switchSysclkCountdown = 10;
    }
    else
    {
        if (uteModuleSprotData.switchSysclkCountdown > 0)
        {
            uteModuleSprotData.switchSysclkCountdown--;
        }
    }
#endif
    if(uteModuleSprotData.isOpenHandScreenOn)
    {
        uint8_t rolloverHandScreenStatus = getHandRollVerScreenDisplayParam();
        // UTE_MODULE_LOG(1,"%s,rolloverHandScreenStatus=%d",__func__,rolloverHandScreenStatus);
        switch (rolloverHandScreenStatus)
        {
            case ROLLOVER_HAND_SCREEN_ON_STATUS:
            {
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
                if((!uteModuleGuiCommonIsDisplayOn())||(uteModuleGuiCommonIsInScreenSaver()))
#else
                if(!uteModuleGuiCommonIsDisplayOn())
#endif
                {
                    // if(uteModuleNotDisturbIsAllowHandScreenOn()&&(uteModuleGuiCommonGetCurrentScreenId()!=UTE_MOUDLE_SCREENS_FACTORY_TEST_ID))
                    if(uteModuleNotDisturbIsAllowHandScreenOn())
                    {
#if UTE_MODULE_LOCAL_SET_LIFT_WRIST_SUPPORT
                        if(uteModuleLiftWristIsOpen()||uteModuleLiftWristIsTimeBucket())
#endif
                        {
                            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HNAD_SCREEN_ON_NOTIFY,0);
                        }
                    }
                }
            }
            break;
            case ROLLOVER_HAND_SCREEN_OFF_STATUS:
            {
#if UTE_MODULE_HAND_SCREEN_OFF_ONLY_WATCHMAIN_ID_SUPPORT//仅主界面允许落腕息屏，其他不允许，默认开启
                if(uteModuleGuiCommonIsDisplayOn()&&(uteModuleGuiCommonGetCurrentScreenId() == FUNC_CLOCK))
#else
                if(uteModuleGuiCommonIsAllowHandGestureDisplayOff())
#endif
                {
                    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HNAD_SCREEN_OFF_NOTIFY,0);
                }
            }
            break;
            default:
                break;
        }
#if 1//UTE_MODULE_LIFT_WRIST_EXCEPTION_HANDLING_SUPPORT
        // clear handup on screen status/*灭屏的时候，如果还是灭屏姿态说明有抬腕亮屏算法需要重新初始化*/
        if(!uteModuleGuiCommonIsDisplayOn() && rolloverHandScreenStatus == ROLLOVER_HAND_SCREEN_OFF_STATUS)
        {
            /*ellison add 防止灭屏的时候没有重新初始化抬手亮屏，这个和勿扰模式无关或者是否开启抬腕亮屏也没有直接关系，
            其他的逻辑不要控制算法逻辑，否则会导致算法异常。现在发现因为队列消息丢失导致抬腕亮屏算法没有重新初始化，抬腕失灵。*/
            uteModuleSprotResetRovllverScreenMode();
        }
#endif
    }
#endif
}
/**
*@brief        更新传入算法库参数标识
*@details
*@author       zn.zeng
*@date       2021-07-27
*/
void uteModuleSportUpdateGsensorIdAccRange(void)
{
    uteModuleSprotData.gsensorId = GSENSOR_BMA250E_ID;
    uteModuleSprotData.accRange = GSENSOR_4G_MODE;
    switch (uteDrvGsensorCommonGetId())
    {
        case UTE_DRV_GSENSOR_ID_STK8321:
        case UTE_DRV_GSENSOR_ID_STK8325:
        {
            uteModuleSprotData.gsensorId = GSENSOR_STK8321_ID;
        }
        break;
        case UTE_DRV_GSENSOR_ID_BMI160:
        {
            uteModuleSprotData.gsensorId = GSENSOR_BMI160_ID;
        }
        break;
        case UTE_DRV_GSENSOR_ID_ICM40607:
        {

        }
        break;
        case UTE_DRV_GSENSOR_ID_SC7A20:
        case UTE_DRV_GSENSOR_ID_SC7A20E:
        case UTE_DRV_GSENSOR_ID_SC7A20H:
        {
            uteModuleSprotData.gsensorId = GSENSOR_SC7A20_ID;
        }
        break;
        case UTE_DRV_GSENSOR_ID_BMI270:
        {
            uteModuleSprotData.gsensorId = GSENSOR_BMI160_ID ;
        }
        break;
    }
    if(uteDrvGsensorCommonGetAccRange()==8)
    {
        uteModuleSprotData.accRange =GSENSOR_8G_MODE;
    }
    else
    {
        uteModuleSprotData.accRange =GSENSOR_4G_MODE;
    }
}
/**
*@brief        设置step type
*@details
*@author       zn.zeng
*@date       2021-08-05
*/

void uteModuleSportStepTypeSetStep(void)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,uteModuleSprotData.stepType=%d", __func__,uteModuleSprotData.stepType);
    if(uteModuleSprotData.stepType!=STEP_TYPE_STEP)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s", __func__);
        uteModuleSprotData.stepType = STEP_TYPE_STEP;
        uteModuleSprotData.lastStepType = STEP_TYPE_STEP;
        uteDrvGsensorCommonInit(UTE_DRV_GSENSOR_DEFAULT_ACC_RATE_VALUE,UTE_DRV_GSENSOR_DEFAULT_ACC_RANGE_VALUE);
        uteModuleSportUpdateGsensorIdAccRange();
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
        uteModuleSprotResetRovllverScreenMode();
        setCurrentStepSleepState(true,false);
#endif
    }

}
/**
*@brief        设置sleep type
*@details
*@author       zn.zeng
*@date       2021-08-05
*/
void uteModuleSportStepTypeSetSleep(void)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,uteModuleSprotData.stepType=%d", __func__,uteModuleSprotData.stepType);
    if(uteModuleSprotData.stepType!=STEP_TYPE_SLEEP)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s", __func__);
        uteModuleSprotData.stepType = STEP_TYPE_SLEEP;
        uteModuleSprotData.lastStepType = STEP_TYPE_SLEEP;
        uteDrvGsensorCommonInit(UTE_DRV_GSENSOR_DEFAULT_ACC_RATE_VALUE,UTE_DRV_GSENSOR_DEFAULT_ACC_RANGE_VALUE);
        uteModuleSportUpdateGsensorIdAccRange();
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
        uteModuleSprotResetRovllverScreenMode();
        setCurrentStepSleepState(false,true);
#endif
        uteModuleSportSaveStepData();
    }

}
/**
*@brief        设置none type
*@details     不计步不睡眠，不跑算法
*@author       zn.zeng
*@date       2021-08-05
*/
void uteModuleSportStepTypeSetNone(void)
{
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT  //赛维算法不设置NONE状态
//    UTE_MODULE_LOG(1, "%s,uteModuleSprotData.stepType=%d", __func__,uteModuleSprotData.stepType);
    if(uteModuleSprotData.stepType!=STEP_TYPE_NONE)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s", __func__);
        if(uteModuleSprotData.sedentary.isOpen)
        {
            uteModuleSprotData.sedentary.sedentaryRemindSecCnt = 0;
        }
        uteModuleSprotData.stepType = STEP_TYPE_NONE;
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
        resetUteStepsParameter();
#endif
        uteDrvGsensorCommonClearFifo();
        uteDrvGsensorCommonSleep();
        // 保存现有数据
        uteModuleSportSaveStepData();
    }
#endif
}

/**
 * @brief        获取当前计步状态
 * @details
 * @return       计步状态
 * @author       Wang.Luo
 * @date         2025-01-10
 */
ute_module_sport_step_type uteModuleSportGetStepType(void)
{
    return uteModuleSprotData.stepType;
}

/**
*@brief        清空抬手亮屏参数
*@details     以下几种情况必须重新使能：1）屏幕off的时候；2）（手点击/滑动屏幕）手动操作屏幕的时候
*@author       zn.zeng
*@date       2021-08-05
*/
void uteModuleSprotResetRovllverScreenMode(void)
{
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    if(uteModuleSprotData.isOpenHandScreenOn)
    {
        enableHandRollverScreenMode();
    }
#endif
#if UTE_CUSTOM_HAND_SCREEN_ON_DISPLAY_OFF_TIME
    uteResetHandScreenOnFlag();
#endif
}
/**
*@brief        发送实时步数
*@details
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleSportSendRealTimeStepInfo(void)
{
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uint8_t response[20];
    memset(response,0x00,20);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    response[0] = CMD_CYWEE_READ_STEP_DATA;
    response[1] = 0x01;
    response[2] = time.year>>8&0xff;
    response[3] = time.year&0xff;
    response[4] = time.month;
    response[5] = time.day;
    response[6] = time.hour;
    response[7] = uteModuleSprotData.stepSleepData.currentHourStepCnt>>8&0xff;
    response[8] = uteModuleSprotData.stepSleepData.currentHourStepCnt>>0&0xff;
    uint16_t distance = (uint16_t)uteModuleCwmGetTodayCurrHourDistance();
    response[9] = distance>>8&0xff;
    response[10] = distance&0xff;
    uint32_t calories =(uint32_t) uteModuleCwmGetTodayCurrHourCalories();
    response[11] = calories>>16&0xff;
    response[12] = calories>>8&0xff;
    response[13] = calories&0xff;
    uteModuleProfileBleSendToPhone(&response[0],14);
#else
    uint8_t response[20];
    memset(response,0x00,20);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    response[0] = CMD_READ_HOUR_STEP_DATA;
    response[1] = time.year>>8&0xff;
    response[2] = time.year&0xff;
    response[3] = time.month;
    response[4] = time.day;
    response[5] = time.hour;
    response[6] = uteModuleSprotData.stepSleepData.currentHourStepCnt>>8&0xff;
    response[7] = uteModuleSprotData.stepSleepData.currentHourStepCnt&0xff;
    response[8] = uteModuleSprotData.stepSleepData.current_hour_start_running_min;
    if (uteModuleSprotData.stepSleepData.current_hour_start_running_min >= uteModuleSprotData.stepSleepData.current_hour_end_running_min)
    {
        response[8] = 0;
    }
    response[9] = uteModuleSprotData.stepSleepData.current_hour_end_running_min;
    response[10] = uteModuleSprotData.stepSleepData.current_hour_run_step_secondCnt/60; //one hour, seconds change to minutes
    response[11] = uteModuleSprotData.stepSleepData.current_hour_run_stepCnt >> 8 & 0xff;
    response[12] = uteModuleSprotData.stepSleepData.current_hour_run_stepCnt & 0xff;
    response[13] = uteModuleSprotData.stepSleepData.current_hour_start_walking_min;
    if (uteModuleSprotData.stepSleepData.current_hour_start_walking_min >= uteModuleSprotData.stepSleepData.current_hour_end_walking_min)
    {
        response[13] = 0;
    }
    response[14] = uteModuleSprotData.stepSleepData.current_hour_end_walking_min;
    response[15] = uteModuleSprotData.stepSleepData.current_hour_walk_secondCnt/60; //one hour ,seconds change to minutes
    uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt = uteModuleSprotData.stepSleepData.currentHourStepCnt -uteModuleSprotData.stepSleepData.current_hour_run_stepCnt;
    response[16] = uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt >> 8 & 0xff;
    response[17] = uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt & 0xff;
#if APP_DBG_GSENSOR_DATA
    // do nothings
    if(uteModuleSprotData.appDebugGsensorDataSwitch == false)
#endif
    {
        uteModuleProfileBleSendToPhone(&response[0],18);
    }
#endif
}
/**
*@brief        保存当前计步数据
*@details      每天保存一个文件，前面4个byte为年月日，后面每12个byte为一个小时的数据（参考协议的数据格式）
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleSportSaveStepData(void)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = 12*24+4;
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
    uint8_t path[40];
    memset(&readBuff[0],0,buffSize);
    memset(&path[0],0,sizeof(path));
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0],"%04d%02d%02d",time.year,time.month,time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR, dirInfo, NULL);
    if((dirInfo->filesCnt>=UTE_MODULE_SPORT_SAVE_STEP_DATA_MAX_DAYS)&&(memcmp(&path[0],&dirInfo->filesName[0][0],8)!=0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-08-23  */
        memset(&path[0],0,sizeof(path));
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR,&dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0],0,sizeof(path));
    sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR,time.year,time.month,time.day);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,save file=%s", __func__,&path[0]);
    /*! 保存当前数据zn.zeng, 2021-08-23  */
    void *file;
    // read
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    else
    {
        readBuff[0] = time.year>>8&0xff;
        readBuff[1] = time.year&0xff;
        readBuff[2] = time.month;
        readBuff[3] = time.day;
    }
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uint16_t cacheOffset = 0;
        cacheOffset = 4+12*time.hour;
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,cacheOffset=%d , currentHourStepCnt = %d", __func__,cacheOffset,uteModuleSprotData.stepSleepData.currentHourStepCnt);
        uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt = uteModuleSprotData.stepSleepData.currentHourStepCnt -uteModuleSprotData.stepSleepData.current_hour_run_stepCnt;
        readBuff[cacheOffset] = (uteModuleSprotData.stepSleepData.currentHourStepCnt>>8)&0xff;
        readBuff[cacheOffset+1] = (uteModuleSprotData.stepSleepData.currentHourStepCnt)&0xff;
        readBuff[cacheOffset+2] = uteModuleSprotData.stepSleepData.current_hour_start_running_min;
        if (uteModuleSprotData.stepSleepData.current_hour_start_running_min >= uteModuleSprotData.stepSleepData.current_hour_end_running_min)
        {
            readBuff[cacheOffset+2] = 0;
        }
        readBuff[cacheOffset+3] = uteModuleSprotData.stepSleepData.current_hour_end_running_min;
        readBuff[cacheOffset+4] = uteModuleSprotData.stepSleepData.current_hour_run_step_secondCnt/60;
        readBuff[cacheOffset+5] = (uteModuleSprotData.stepSleepData.current_hour_run_stepCnt>>8)&0xff;
        readBuff[cacheOffset+6] = (uteModuleSprotData.stepSleepData.current_hour_run_stepCnt)&0xff;
        readBuff[cacheOffset + 7] = uteModuleSprotData.stepSleepData.current_hour_start_walking_min;
        if (uteModuleSprotData.stepSleepData.current_hour_start_walking_min >= uteModuleSprotData.stepSleepData.current_hour_end_walking_min)
        {
            readBuff[cacheOffset + 7] = 0;
        }
        readBuff[cacheOffset+8] = uteModuleSprotData.stepSleepData.current_hour_end_walking_min;
        readBuff[cacheOffset+9] = uteModuleSprotData.stepSleepData.current_hour_walk_secondCnt/60;
        readBuff[cacheOffset+10] = (uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt>>8)&0xff;
        readBuff[cacheOffset+11] = (uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt)&0xff;
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(readBuff);
    uteModulePlatformMemoryFree(dirInfo);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCwmSaveStepData();     //保存当天运动数据
#endif
}
#if  UTE_MODULE_CYWEE_MOTION_SUPPORT
/**
*@brief        赛维开始发送计步数据
*@details
*@author     casen
*@date       2022-11-03
*/
void uteModuleSportSendStepHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[32];
    uint8_t response[20];
    uint8_t sendSize = 0;
    response[0] = CMD_CYWEE_READ_STEP_DATA;
    response[1] = 0x02;
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&dataBuff[0],0,sendParam->dataBuffSize);
    memset(&path[0],0,32);

    if(sendParam->dirInfo.filesCnt>0)
    {
    SEND_NEXT_DATA:
        sendSize = 14;
        bool isHasData = false;
        void *file;
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CWM_STEP_DATA_DIR,&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,read file=%s", __func__,&path[0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex);
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file,&dataBuff[0],sendParam->dataBuffSize);
            uteModuleFilesystemCloseFile(file);
        }
        uint8_t invalidHour = 0;
        for(uint8_t i=sendParam->currSendHourIndex; i<24; i++)
        {
            uint16_t stetCnt = dataBuff[4+CWMSTEPDATA_ONCE_DATA_LEN*i]<<8|dataBuff[4+CWMSTEPDATA_ONCE_DATA_LEN*i+1];
            if(stetCnt==0)
            {
                invalidHour++;
            }
            else
            {
                memcpy(&response[2],&dataBuff[0],4);//year month day
                response[6] = i;
                memcpy(&response[7],&dataBuff[4+CWMSTEPDATA_ONCE_DATA_LEN*i],CWMSTEPDATA_ONCE_DATA_LEN);
                invalidHour++;
                isHasData = true;
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,invalidHour=%d", __func__,invalidHour);
        sendParam->currSendHourIndex+=invalidHour;
        if(sendParam->currSendHourIndex>=24)
        {
            sendParam->currSendFileIndex++;
            if(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt)
            {
                sendParam->currSendHourIndex = 0;
            }
            else
            {
                sendParam->currSendHourIndex=24;
            }
        }
        if((!isHasData)&&(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt))
        {
            goto SEND_NEXT_DATA;
        }
    }
    else
    {
        sendParam->currSendHourIndex=24;
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex,sendParam->dirInfo.filesCnt);
    if((sendParam->currSendFileIndex==sendParam->dirInfo.filesCnt)
       &&(sendParam->currSendHourIndex==24))
    {
        sendSize = 4;
        response[1] = 0x02;
        response[2] = 0xfd;
        response[3] = sendParam->crc;
        uteApplicationCommonSyncDataTimerStop();
    }
    if(sendSize==14)
    {
        uteModuleCrc8Bit(&sendParam->crc, &response[2],12);
    }
    uteModuleProfileBleSendToPhone(&response[0],sendSize);
    uteModulePlatformMemoryFree(dataBuff);
}
#else
/**
*@brief        开始发送计步数据
*@details
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleSportSendStepHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[40];
    uint8_t response[sizeof(path)];
    uint8_t sendSize = 0;
    response[0] = CMD_START_SYN_SETP_DATA;
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&path[0],0,sizeof(path));
    bool isHasData = false;
    if(sendParam->dirInfo.filesCnt>0)
    {
    SEND_NEXT_DATA:
        sendSize = 18;
        isHasData = false;
        void *file;
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR,&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,read file=%s", __func__,&path[0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex);
        memset(&dataBuff[0],0,sendParam->dataBuffSize);
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&dataBuff[0],sendParam->dataBuffSize);
            uteModuleFilesystemCloseFile(file);
        }
        uint8_t invalidHour = 0;
        for(uint8_t i=sendParam->currSendHourIndex; i<24; i++)
        {
            uint16_t stetCnt = dataBuff[4+12*i]<<8|dataBuff[4+12*i+1];
            if(stetCnt==0)
            {
                invalidHour++;
            }
            else
            {
                memcpy(&response[1],&dataBuff[0],4);//year month day
                response[5] = i;
                memcpy(&response[6],&dataBuff[4+12*i],12);
                invalidHour++;
                isHasData = true;
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,invalidHour=%d", __func__,invalidHour);
        sendParam->currSendHourIndex+=invalidHour;
        if(sendParam->currSendHourIndex>=24)
        {
            sendParam->currSendFileIndex++;
            if(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt)
            {
                sendParam->currSendHourIndex = 0;
            }
            else
            {
                sendParam->currSendHourIndex=24;
            }
        }
        if((!isHasData)&&(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt))
        {
            goto SEND_NEXT_DATA;
        }
    }
    else
    {
        sendParam->currSendHourIndex=24;
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex,sendParam->dirInfo.filesCnt);
    if((sendParam->currSendFileIndex>=sendParam->dirInfo.filesCnt)
       &&(sendParam->currSendHourIndex==24)&&(!isHasData))
    {
        sendSize = 3;
        response[1] = 0xfd;
        response[2] = sendParam->crc;
        uteApplicationCommonSyncDataTimerStop();
    }
    if(sendSize==18)
    {
        uteModuleCrc8Bit(&sendParam->crc, &response[1],17);
    }
    uteModuleProfileBleSendToPhone(&response[0],sendSize);
    uteModulePlatformMemoryFree(dataBuff);
}
#endif
/**
*@brief        准备开始发送计步数据
*@details
*@param[in]  需要同步的数据时间戳
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleSportStartSendStepHistoryData(ute_module_systemtime_time_t time)
{
    uteModuleSportSaveStepData();
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->time = time;
    if(param->time.year==0)
    {
        param->isAllData = true;
    }
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    param->dataBuffSize = CWMSTEPDATA_ONCE_DATA_LEN*24+4;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CWM_STEP_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSportSendStepHistoryData);
#else
    param->dataBuffSize = 12*24+4;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSportSendStepHistoryData);
#endif
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
}
/**
*@brief          打开关闭摇一摇拍照
*@details
*@author       zn.zeng
*@date       2021-08-24
*/
void uteModuleSportSetTakePictureEnable(bool isEnable)
{
    uteModuleSprotData.isTakingPictures = isEnable;
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    if(uteModuleSprotData.isTakingPictures)
    {
        enableTakingPicturesState();
    }
    else
    {
        disableTakingPicturesState();
    }
#endif
}
/**
*@brief          摇一摇拍照消息处理
*@details
*@author       zn.zeng
*@date       2021-08-24
*/
void uteModuleSportTakePictureHandlerMsg(void)
{
    uint8_t response[2] = {CMD_TAKE_PICTURE_MODE,0x02};
    response[0] = CMD_TAKE_PICTURE_MODE;
    uteModuleProfileBleSendToPhone(&response[0],2);
#if UTE_MODULE_SPORT_TAKE_PICTURE_MOTOR_SUPPORT
    if(uteModuleSportIsTakePicture())
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        uteModuleSportDelaySetTakePictureEnableTimerStart(2000);
    }
#else
    enableTakingPicturesState();
#endif
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s", __func__);
}
/**
*@brief          摇一摇拍照打开app相机界面消息处理
*@details
*@author       dengli.lu
*@date       2022-08-23
*/
#if UTE_MODULE_SPORT_TAKE_PICTURE_OPEN_APP_SCREEN
void uteModuleSportOpenAppTakePictureScreenHandler(void)
{
    uint8_t response[2];
    memset(&response[0],0,2);
    response[0] = CMD_SEND_KEYCODE;
    response[1] = 0x11;
    uteModuleProfileBleSendToPhone(&response[0],2);
}
#endif
/**
*@brief         保存久坐提醒参数
*@details
*@author       zn.zeng
*@date       2021-08-24
*/
void uteModuleSportSaveSedentaryParam(ute_module_sport_sedentary_param_t *param)
{
    if(param) memcpy(&uteModuleSprotData.sedentary,param,sizeof(ute_module_sport_sedentary_param_t));
    uteModuleSprotData.sedentary.sedentaryRemindSecCnt=0;
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    if(uteModuleSprotData.sedentary.isOpen)
    {
        enableSedentaryRemind();
    }
    else
    {
        disableSedentaryRemind();
    }
#endif
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,.isOpen=%d,setMin=%d,startPeriodHour=%d,startPeriodMin=%d", __func__,uteModuleSprotData.sedentary.isOpen,uteModuleSprotData.sedentary.setMin, uteModuleSprotData.sedentary.startPeriodHour,uteModuleSprotData.sedentary.startPeriodMin);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,endPeriodHour=%d,endPeriodMin=%d", __func__,uteModuleSprotData.sedentary.endPeriodHour,uteModuleSprotData.sedentary.endPeriodMin);
    /*! 保存到文件zn.zeng, 2021-08-26*/
    void *file;
    uint8_t writebuff[17];
    writebuff[0] = uteModuleSprotData.sedentary.isOpen;
    writebuff[1] = uteModuleSprotData.sedentary.setMin;
    writebuff[2] = uteModuleSprotData.sedentary.startPeriodHour;
    writebuff[3] = uteModuleSprotData.sedentary.startPeriodMin;
    writebuff[4] = uteModuleSprotData.sedentary.endPeriodHour;
    writebuff[5] = uteModuleSprotData.sedentary.endPeriodMin;
    writebuff[6] = uteModuleSprotData.isOpenHandScreenOn;
    writebuff[7] = (uteModuleSprotData.stepsTargetCnt>>24)&0xff;
    writebuff[8] = (uteModuleSprotData.stepsTargetCnt>>16)&0xff;
    writebuff[9] = (uteModuleSprotData.stepsTargetCnt>>8)&0xff;
    writebuff[10] = uteModuleSprotData.stepsTargetCnt&0xff;
    writebuff[11] = uteModuleSprotData.isTargetCompleted;
    writebuff[12] = uteModuleSprotData.sedentary.isOpenNoonSleep;
#if UTE_MODULE_SEDENTARY_NOTIFY_SET_NOONSLEEP_TIME
    writebuff[13] = (uteModuleSprotData.sedentary.noonSleepStartTime>>8)&0xff;
    writebuff[14] = uteModuleSprotData.sedentary.noonSleepStartTime&0xff;
    writebuff[15] = (uteModuleSprotData.sedentary.noonSleepEndTime>>8)&0xff;
    writebuff[16] = uteModuleSprotData.sedentary.noonSleepEndTime&0xff;
#endif
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SPORTINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],17);
        uteModuleFilesystemCloseFile(file);
    }
    //ute_quick_switch_t quick;
    //uteApplicationCommonGetQuickSwitchStatus(&quick);
    //quick.isSedentary = uteModuleSprotData.sedentary.isOpen;
    //uteApplicationCommonSetQuickSwitchStatus(&quick);
}

/**
*@brief         读取久坐提醒参数
*@details
*@author       bang.zhou
*@date       2022-10-11
*/
void uteModuleSportReadSedentaryParam(ute_module_sport_sedentary_param_t *param)
{
    memcpy(param,&uteModuleSprotData.sedentary,sizeof(ute_module_sport_sedentary_param_t));
}

/**
*@brief         保存抬手亮屏、目标步数
*@details
*@author       zn.zeng
*@date       2021-08-26
*/
void uteModuleSportSaveHandScreenOnStepsTargetCnt(bool isHandOpen,uint32_t targetCnt)
{
    /*! 只有修改的目标步数大于当前设置的目标步数，才重置已提醒状态 xjc, 2022-01-05*/
    if(targetCnt > uteModuleSprotData.stepsTargetCnt)
    {
        uteModuleSprotData.isTargetCompleted = false;
    }
    uteModuleSprotData.isOpenHandScreenOn = isHandOpen;
    uteModuleSprotData.stepsTargetCnt = targetCnt;
    /*! 保存到文件zn.zeng, 2021-08-26*/
    void *file;
    uint8_t writebuff[12];
    writebuff[0] = uteModuleSprotData.sedentary.isOpen;
    writebuff[1] = uteModuleSprotData.sedentary.setMin;
    writebuff[2] = uteModuleSprotData.sedentary.startPeriodHour;
    writebuff[3] = uteModuleSprotData.sedentary.startPeriodMin;
    writebuff[4] = uteModuleSprotData.sedentary.endPeriodHour;
    writebuff[5] = uteModuleSprotData.sedentary.endPeriodMin;
    writebuff[6] = uteModuleSprotData.isOpenHandScreenOn;
    writebuff[7] = (uteModuleSprotData.stepsTargetCnt>>24)&0xff;
    writebuff[8] = (uteModuleSprotData.stepsTargetCnt>>16)&0xff;
    writebuff[9] = (uteModuleSprotData.stepsTargetCnt>>8)&0xff;
    writebuff[10] = uteModuleSprotData.stepsTargetCnt&0xff;
    writebuff[11] = uteModuleSprotData.isTargetCompleted;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SPORTINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],12);
        uteModuleFilesystemCloseFile(file);
    }
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    if(uteModuleSprotData.sedentary.isOpen)
    {
        enableSedentaryRemind();
    }
    else
    {
        disableSedentaryRemind();
    }
#endif
    ute_quick_switch_t quick;
    uteModuleSprotResetRovllverScreenMode();
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = uteModuleSprotData.isOpenHandScreenOn;
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,isOpenHandScreenOn=%d,stepsTargetCnt=%d", __func__,uteModuleSprotData.isOpenHandScreenOn,uteModuleSprotData.stepsTargetCnt);
}

bool uteModuleSportSedentaryOpenCtrl(bool isSet,bool isOpen)
{
    if(isSet)
    {
        uteModuleSprotData.sedentary.isOpen=isOpen;
        uteModuleSportSaveSedentaryParam(NULL);
    }

    return uteModuleSprotData.sedentary.isOpen;
}
void uteModuleSportSedentaryOpenSwitch(void)
{
    uteModuleSprotData.sedentary.isOpen=!uteModuleSprotData.sedentary.isOpen;
    uteModuleSportSaveSedentaryParam(NULL);
}



/**
*@brief         从flash读取当天步数信息
*@details
*@param[out] uint32_t *total 总步数
*@param[out] uint32_t *walk 总走路步数
*@param[out] uint32_t *run 总跑步步数
*@author       zn.zeng
*@date       2021-11-03
*/
void uteModuleSportReadCurrDayStepCnt(ute_module_systemtime_time_t time)
{
    uint32_t readTotal=0;
    uint32_t readWalk=0;
    uint32_t readRun=0;
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    //Casen add 22-07-13
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
    uteModuleSprotData.dailyActivity.everyHourRunStep[time.hour] = (uteModuleSprotData.stepSleepData.current_hour_run_stepCnt);
    uteModuleSprotData.dailyActivity.everyHourWalkStep[time.hour] = (uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleSprotData.dailyActivity.everyHourWalkStep[time.hour] = uteModuleSprotData.stepSleepData.currentHourStepCnt;
#endif
#endif
    if(time.hour!=uteModuleSprotData.lastDayReadHour)
    {
        void *file;
        uint8_t path[40];
        uint16_t buffSize = 12*24+4;
        uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
        memset(&path[0],0,sizeof(path));
        sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR,time.year,time.month,time.day);
        // read
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&readBuff[0],buffSize);
            uteModuleFilesystemCloseFile(file);
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
            memset(&uteModuleSprotData.dailyActivity.everyHourRunStep[0], 0, 24 * 4);
            memset(&uteModuleSprotData.dailyActivity.everyHourWalkStep[0], 0, 24 * 4);
#endif
            for(uint8_t i=0; i<24; i++)
            {
                uint32_t tmpTotal = (readBuff[4+i*12]<<8|readBuff[4+i*12+1]);
                uint32_t tmpRun = (readBuff[4+i*12+5]<<8|readBuff[4+i*12+6]);
                uint32_t tmpWalk = (readBuff[4+i*12+10]<<8|readBuff[4+i*12+11]);
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
                uteModuleSprotData.dailyActivity.everyHourRunStep[i] = tmpRun;
                uteModuleSprotData.dailyActivity.everyHourWalkStep[i] = tmpWalk;
#endif
                if(time.hour==i)
                {
                    break;
                }
                // UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,i=%d,tmpTotal=%d,tmpRun=%d,tmpWalk=%d,everyHourWalkStep[i]=%d", __func__,i,tmpTotal,tmpRun,tmpWalk,uteModuleSprotData.dailyActivity.everyHourWalkStep[i]);
                readTotal = readTotal+tmpTotal;
                readRun = readRun+tmpRun;
                readWalk = readWalk+tmpWalk;
            }
        }
        uteModulePlatformMemoryFree(readBuff);
        uteModuleSprotData.currDayReadTotalStep = readTotal;
        uteModuleSprotData.currDayReadWalkStep = readWalk;
        uteModuleSprotData.currDayReadRunStep = readRun;
        uteModuleSprotData.lastDayReadHour = time.hour;
        //  UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,readTotal=%d,readRun=%d,readWalk=%d", __func__,readTotal,readRun,readWalk);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
        uteModuleCwmReadCurrDayStepFromFs();
#endif
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currentHourStepCnt=%d,current_hour_run_stepCnt=%d,current_hour_walk_stepCnt=%d", __func__,uteModuleSprotData.stepSleepData.currentHourStepCnt,uteModuleSprotData.stepSleepData.current_hour_run_stepCnt,uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    cwmStepData_t currentDayStepData;
    uteModuleCwmGetCurrStepData(&currentDayStepData);
    uteModuleSprotData.currDayTotalStep = (uint32_t)(currentDayStepData.step*1.0f) + uteModuleSprotData.stepSleepData.currentHourStepCnt;
//    UTE_MODULE_LOG(UTE_LOG_CYWEE_MOTION_LVL, "%s,currDayTotalStep=%d, step = %f", __func__,uteModuleSprotData.currDayTotalStep,currentDayStepData.step);
#else
    uteModuleSprotData.currDayTotalStep = uteModuleSprotData.currDayReadTotalStep +uteModuleSprotData.stepSleepData.currentHourStepCnt;
#endif
    uteModuleSprotData.currDayWalkStep = uteModuleSprotData.currDayReadWalkStep+uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt;
    uteModuleSprotData.currDayRunStep = uteModuleSprotData.currDayReadRunStep+uteModuleSprotData.stepSleepData.current_hour_run_stepCnt;
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currDayTotalStep=%d,currDayWalkStep=%d,currDayRunStep=%d", __func__,uteModuleSprotData.currDayTotalStep,uteModuleSprotData.currDayWalkStep,uteModuleSprotData.currDayRunStep);

    uteModulePlatformGiveMutex(uteModuleSprotMute);
}

/**
*@brief         获取当天步数信息
*@details
*@param[out] uint32_t *total 总步数
*@param[out] uint32_t *walk 总走路步数
*@param[out] uint32_t *run 总跑步步数
*@author       zn.zeng
*@date       2021-10-23
*/
void uteModuleSportGetCurrDayStepCnt(uint32_t *total,uint32_t *walk,uint32_t *run)
{
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    if (total != NULL)
    {
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
        *total = uteModuleSprotData.currDayTotalStep + uteModuleSprotData.todaySportWithStepsData.step;
#else
        *total = uteModuleSprotData.currDayTotalStep;
#endif
#if UTE_LOG_GUI_LVL // test
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        if(time.sec%3==0)
        {
            *total=3210;
        }
        if(time.sec%5==0)
        {
            *total=654;
        }
        if(time.sec%7==0)
        {
            *total=87;
        }
        if(time.sec%11==0)
        {
            *total=9;
        }
#endif
    }
    if (walk != NULL)
    {
        *walk = uteModuleSprotData.currDayWalkStep;
    }
    if (run != NULL)
    {
        *run = uteModuleSprotData.currDayRunStep;
    }
    if(*total > 99999)
    {
        *total = 99999;
    }
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
/**
*@brief  读取目标步数
*@details
*@author        dengli.lu
*@date        2021-10-26
*/
uint32_t uteModuleSportGetStepsTargetCnt(void)
{
    return uteModuleSprotData.stepsTargetCnt;
}
/**
*@brief  设置目标提醒是否完成
*@details
*@author        dengli.lu
*@date        2021-10-26
*/
void uteModuleSportSetIsTargetCompleted(bool isTargetCompleted)
{
    uteModuleSprotData.isTargetCompleted = isTargetCompleted;
}
/**
*@brief  获取目标提醒是否完成
*@details
*@author        dengli.lu
*@date        2021-10-26
*/
bool uteModuleSportGetIsTargetCompleted(void)
{
    return uteModuleSprotData.isTargetCompleted;
}
/**
*@brief  读取抬手亮屏开关
*@details
*@author        dengli.lu
*@date        2021-10-26
*/
bool uteModuleSportGetIsOpenHandScreenOn(void)
{
    return uteModuleSprotData.isOpenHandScreenOn;
}
/**
*@brief         获取当天距离
*@details
*@author       dengli.lu
*@date       2021-10-26
*/
uint16_t uteModuleSportGetCurrDayDistanceData(void)
{
    float distance;
    float distance_Step,distance_Run;
    ute_personal_info_t personal;
    uteApplicationCommonGetPersonalInfo(&personal);
    uint16_t tempDistance=0;
    uteModulePlatformTakeMutex(uteModuleSprotMute);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    cwmStepData_t currentDayStepData;
    uteModuleCwmGetCurrStepData(&currentDayStepData);
    float currHourdistance = uteModuleCwmGetTodayCurrHourDistance();
    distance = ExactDecimalPoint(currentDayStepData.distance*0.001f, 2) + ExactDecimalPoint(currHourdistance*0.001f, 2);
#else
    {
        if(personal.isMan)
        {
            distance_Step = (float)((float)((personal.height) * (uteModuleSprotData.currDayWalkStep))*0.00001f*0.4102670f);
            distance_Run = (float)((float)((personal.height) * (uteModuleSprotData.currDayRunStep))*0.00001f*0.5461055f);
        }
        else
        {
            distance_Step = (float)((float)((personal.height) * (uteModuleSprotData.currDayWalkStep))*0.00001f*0.4151582f);
            distance_Run = (float)((float)((personal.height) * (uteModuleSprotData.currDayRunStep))*0.00001f*0.5047813f);
        }
    }
    distance_Step = ExactDecimalPoint(distance_Step,2);
    distance_Run = ExactDecimalPoint(distance_Run,2);
    distance = ExactDecimalPoint((distance_Step+distance_Run),2);
#endif
    if(uteModuleSystemtimeGetDistanceMiType())
    {
        distance = (float)distance * 0.6213712f;
    }
    distance = ExactDecimalPoint(distance,2);
#if UTE_LOG_GUI_LVL
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if(time.sec%3==0)
    {
        distance=12.34;
    }
    if(time.sec%5==0)
    {
        distance=45.67;
    }
    if(time.sec%7==0)
    {
        distance=78.90;
    }
    if(time.sec%11==0)
    {
        distance=70;
    }
#endif
    tempDistance = (uint16_t)(distance*100.f) ; //放大浮点取小数点后两位
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
    uint16_t moreSportDistance = (uint16_t)((uteModuleSystemtimeGetDistanceMiType()) ? (uteModuleSprotData.todaySportWithStepsData.distance * 0.6213712f) : uteModuleSprotData.todaySportWithStepsData.distance);
    tempDistance = tempDistance + moreSportDistance;
#endif
    uteModulePlatformGiveMutex(uteModuleSprotMute);
    if(tempDistance>9999)
    {
        tempDistance = 9999;
    }
    return tempDistance;
}
/**
*@brief         获取当天卡路里
*@details
*@author       dengli.lu
*@date       2021-10-26
*/
uint16_t uteModuleSportGetCurrDayKcalData(void)
{
    uint16_t kcal=0;
    float distance_Step,distance_Run;
    float kcli_Step,kcli_Run,kcli;
    ute_personal_info_t personal;
    uteApplicationCommonGetPersonalInfo(&personal);
    uteModulePlatformTakeMutex(uteModuleSprotMute);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    cwmStepData_t currentDayStepData;
    uteModuleCwmGetCurrStepData(&currentDayStepData);
    float calories = uteModuleCwmGetTodayCurrHourCalories();
    kcli_Step = ExactDecimalPoint(currentDayStepData.calories*0.001f,1) + ExactDecimalPoint(calories*0.001f,1);
    kcli_Run = 0.0f;
#else
    if(personal.isMan)
    {
        distance_Step = (float)((float)((personal.height) * (uteModuleSprotData.currDayWalkStep))*0.00001f*0.4102670f);
        distance_Run = (float)((float)((personal.height) * (uteModuleSprotData.currDayRunStep))*0.00001f*0.5461055f);

        kcli_Step = (float)(distance_Step*personal.weight/1000.0f*0.5907296f);
        kcli_Run =  (float)(distance_Run*personal.weight/1000.0f*0.9288487f);
    }
    else
    {
        distance_Step = (float)((float)((personal.height) * (uteModuleSprotData.currDayWalkStep))*0.00001f*0.4151582f);
        distance_Run = (float)((float)((personal.height) * (uteModuleSprotData.currDayRunStep))*0.00001f*0.5047813f);

        kcli_Step = (float)(distance_Step*personal.weight/1000.0f*0.7918928f);
        kcli_Run = (float)(distance_Run*personal.weight/1000.0f*1.234930f);
    }
    kcli_Step = ExactDecimalPoint(kcli_Step,1);
    kcli_Run = ExactDecimalPoint(kcli_Run,1);
#endif
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
    float kcalAllSport;
    kcalAllSport = uteModuleSprotData.todaySportWithStepsData.caloire;
    kcalAllSport = ExactDecimalPoint(kcalAllSport,1);
    kcli = ExactDecimalPoint((kcli_Step+kcli_Run+kcalAllSport),2);
#else
    kcli = ExactDecimalPoint((kcli_Step+kcli_Run),2);
#endif
    uteModulePlatformGiveMutex(uteModuleSprotMute);
    kcal = (uint16_t)(kcli*1.0f);
#if UTE_LOG_GUI_LVL
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if(time.sec%3==0)
    {
        kcal=3210;
    }
    if(time.sec%5==0)
    {
        kcal=654;
    }
    if(time.sec%7==0)
    {
        kcal=87;
    }
    if(time.sec%11==0)
    {
        kcal=9;
    }
#endif
    if(kcal>9999)
    {
        kcal = 9999;
    }
    return kcal;
}

/**
*@brief     开始多运动
*@details
*@param[in]  sportsType     运动模式标志
*@param[in]  startInterval  每隔N个10秒
*@param[in]  isAppStart     是否为APP开启运动
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportStartMoreSports(uint8_t sportsType,uint8_t startInterval,bool isAppStart)
{
    printf("%s\n",__func__);
    uint32_t param = sportsType<<24|startInterval<<16|isAppStart<<8;
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_SPORT_START_MORE_SPORTS,param);
}
/**
*@brief     发送运动状态给app
*@details
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportSendMoreSportsStatus(void)
{
    uint8_t resp[4];
    resp[0] = CMD_SPORT_MODE_AND_SPORT_HEART_RATE;
    resp[1] = uteModuleSprotData.moreSportData.status;
    resp[2] = uteModuleSprotData.moreSportData.saveData.sportsType;
    resp[3] = uteModuleSprotData.moreSportData.saveData.startInterval;
    uteModuleProfileBleSendToPhone(&resp[0],4);
}

/**
*@brief     用于与APP同步运动状态
*@details
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportSyncAppSportStatus(all_sport_status_type sportStatus)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sportStatus=0x%x,uteModuleSprotData.moreSportData.status=0x%x,isAppStart=%d ", __func__,sportStatus,uteModuleSprotData.moreSportData.status,uteModuleSprotData.moreSportData.isAppStart);
    if(sportStatus ==uteModuleSprotData.moreSportData.status)
    {
        return;//相同的状态就不用重复更新给手机
    }
    if (sportStatus ==ALL_SPORT_STATUS_CONTINUE)
    {
        /*!APP启动运动，断开连接回连时,不发送继续指令，在回连同步数据的时候可能会导致手环端数据不更新 xjc 2022-03-07*/
        if((uteModuleSportMoreSportIsAppStart())&&(!uteModuleSportMoreSportIsAppStartReconnection()))
        {
            return;
        }
    }
    uint8_t length =13;
    if (sportStatus ==ALL_SPORT_STATUS_CLOSE)
    {
        length = 4;
    }

    uteModuleSprotData.moreSportData.status = (sportStatus==ALL_SPORT_STATUS_CONTINUE)?ALL_SPORT_STATUS_OPEN:sportStatus ;

    if(uteModuleSprotData.moreSportData.isAppStart)
    {
        static uint8_t tmpSendData[14];

        tmpSendData[0] = CMD_SPORT_MODE_AND_SPORT_HEART_RATE;   //指令头标志
        tmpSendData[1] = sportStatus;
        tmpSendData[2] = uteModuleSprotData.moreSportData.saveData.sportsType;
        tmpSendData[3] = uteModuleSprotData.moreSportData.saveData.startInterval;
        //get sport time
        tmpSendData[4] = (uteModuleSprotData.moreSportData.totalSportTime>>16&0xff);
        tmpSendData[5] = (uteModuleSprotData.moreSportData.totalSportTime>>8)&0xff;
        tmpSendData[6] = uteModuleSprotData.moreSportData.totalSportTime&0xff;
        // kcal
        tmpSendData[7] = (uteModuleSprotData.moreSportData.saveData.sportCaloire>>8)&0xff;
        tmpSendData[8] = uteModuleSprotData.moreSportData.saveData.sportCaloire & 0xff;
        //distance
        tmpSendData[9] = uteModuleSprotData.moreSportData.saveData.sportDistanceInteger;
        tmpSendData[10] = uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals;
        //avg time
        tmpSendData[11] = uteModuleSprotData.moreSportData.saveData.avgTimeMinute;
        tmpSendData[12] = uteModuleSprotData.moreSportData.saveData.avgTimeSecond;

        uteModuleProfileBleSendToPhone(&tmpSendData[0],length);
    }
}

/**
*@brief     app端启动运动是否的带gps的运动类型
*@details
*@author       zn.zeng
*@date       2021-11-11
*/
bool uteModuleSportIsPhoneAppStartGpsSportsType(void)
{
    bool result =false;
    if((uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_RUNNING)
       ||(uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_WALKING)
#if (!UTE_MODULE_SPORT_BIKE_KCAL_USE_LOCAL_DATA_SUPPORT)
       ||(uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_RIDE_BIKE)
#endif
      )
    {
        result =true;
    }
    return result;
}
/**
*@brief     多运动每秒从算法获取数据
*@details
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportMoreSportsGetDataFormAlgo(ute_module_systemtime_time_t *time)
{
    uint8_t sportDistanceInteger,sportDistanceDecimals;
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    cwmSportModeData_t cwmSportModeData = {0};
    uint16_t sportPace=0;
    uteModuleCwmGetCurrSportData(&cwmSportModeData);
#endif
    struct prf_date_time currentTime;
    currentTime.year = time->year;
    currentTime.month = time->month;
    currentTime.day = time->day;
    currentTime.hour = time->hour;
    currentTime.min = time->min;
    currentTime.sec = time->sec;
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    if(uteModuleSprotData.moreSportData.isAppStart)
    {
        //如果是APP开启的GPS运动，那么卡路里、距离、配速、运动时长，这四个数据由APP推送下来，不在从算法中获取
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
        getUTESportData(currentTime);//这个只是更新开始和结束时间，为了统一性，总的时长还是由APP下发，卡路里由算法提供
#endif
        if (!uteModuleSportIsPhoneAppStartGpsSportsType())
        {
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
            uteModuleSprotData.moreSportData.saveData.sportCaloire = (uint16_t)cwmSportModeData.sportCalories;
            //距离:保留2位小数
            sportDistanceInteger  = (uint8_t)(cwmSportModeData.sportDistance/1000);
            sportDistanceDecimals = (uint8_t)((cwmSportModeData.sportDistance - sportDistanceInteger*1000)/10);
            uteModuleSprotData.moreSportData.saveData.sportDistanceInteger = sportDistanceInteger;
            uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals = sportDistanceDecimals;
            //配速
            sportPace = (uint16_t)(cwmSportModeData.sportPace*60);
            uteModuleSprotData.moreSportData.saveData.avgTimeMinute = sportPace/60;
            uteModuleSprotData.moreSportData.saveData.avgTimeSecond = sportPace%60;;

#else
            uteModuleSprotData.moreSportData.saveData.sportCaloire = mUTESportDatas.sportKcal;
            //距离:保留2位小数
            sportDistanceInteger  = (uint8_t)(mUTESportDatas.sportDistance);
            sportDistanceDecimals = (uint8_t)((mUTESportDatas.sportDistance-(float)sportDistanceInteger)*100);
            uteModuleSprotData.moreSportData.saveData.sportDistanceInteger = sportDistanceInteger;
            uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals = sportDistanceDecimals;
            //配速
            uteModuleSprotData.moreSportData.saveData.avgTimeMinute = (mUTESportDatas.sportSpeed>>8)&0xFF;
            uteModuleSprotData.moreSportData.saveData.avgTimeSecond = mUTESportDatas.sportSpeed&0xFF;
#endif
        }
    }
    else
    {
#if UTE_MODULE_HEART_SUPPORT
        if(uteModuleHeartIsWear())
#endif
        {
            uteModuleSprotData.moreSportData.totalSportTime++;
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
            getUTESportData(currentTime);
#endif
        }
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
        //卡路里
        uteModuleSprotData.moreSportData.saveData.sportCaloire = (uint16_t)cwmSportModeData.sportCalories;
        //距离:保留2位小数
        sportDistanceInteger  = (uint8_t)(cwmSportModeData.sportDistance/1000);
        sportDistanceDecimals = (uint8_t)((cwmSportModeData.sportDistance - sportDistanceInteger*1000)/10);
        uteModuleSprotData.moreSportData.saveData.sportDistanceInteger = sportDistanceInteger;
        uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals = sportDistanceDecimals;
        //配速
        sportPace = (uint16_t)(cwmSportModeData.sportPace*60);
        uteModuleSprotData.moreSportData.saveData.avgTimeMinute = sportPace/60;
        uteModuleSprotData.moreSportData.saveData.avgTimeSecond = sportPace%60;;
#else
        //卡路里
        uteModuleSprotData.moreSportData.saveData.sportCaloire = mUTESportDatas.sportKcal;
        //距离:保留2位小数
        sportDistanceInteger  = (uint8_t)(mUTESportDatas.sportDistance);
        sportDistanceDecimals = (uint8_t)((mUTESportDatas.sportDistance-(float)sportDistanceInteger)*100);
        uteModuleSprotData.moreSportData.saveData.sportDistanceInteger = sportDistanceInteger;
        uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals = sportDistanceDecimals;
        //配速
        uteModuleSprotData.moreSportData.saveData.avgTimeMinute = (mUTESportDatas.sportSpeed>>8)&0xFF;
        uteModuleSprotData.moreSportData.saveData.avgTimeSecond = mUTESportDatas.sportSpeed&0xFF;
#endif
    }
    //UTE_LOG(UTE_LOG_SAVE_LVL,"sportDistanceInteger = %d \t sportDistanceDecimals = %d \n\r",sportDistanceInteger,sportDistanceDecimals);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    //步数
    uteModuleSprotData.moreSportData.saveData.sportStep = (uint32_t)cwmSportModeData.sportStep;
    //赛维提供的库跳绳和游泳时不输出运动次数
//    //次数
//    if(mUTESportDatas.sportType == SPORT_TYPE_JUMP_ROPE)
//    {
//        uteModuleSprotData.moreSportData.saveData.sportTimes = mUTESportDatas.jumpRopeCurrentCnt;
//    }
//    else if(mUTESportDatas.sportType == SPORT_TYPE_SWIMMING)
//    {
//        uteModuleSprotData.moreSportData.saveData.sportTimes = mUTESportDatas.swimmingCurrentCnt;
//    }
    //开始时间&结束时间
    memcpy(&uteModuleSprotData.moreSportData.saveData.startSportTime,&cwmSportModeData.startSportTime,7);
    memcpy(&uteModuleSprotData.moreSportData.saveData.stopSportTime,&cwmSportModeData.stopSportTime,7);

#else
    //步数
    uteModuleSprotData.moreSportData.saveData.sportStep = mUTESportDatas.sportSteps;
    //次数
    if(mUTESportDatas.sportType == SPORT_TYPE_JUMP_ROPE)
    {
        uteModuleSprotData.moreSportData.saveData.sportTimes = mUTESportDatas.jumpRopeCurrentCnt;
    }
    else if(mUTESportDatas.sportType == SPORT_TYPE_SWIMMING)
    {
        uteModuleSprotData.moreSportData.saveData.sportTimes = mUTESportDatas.swimmingCurrentCnt;
    }
    //开始时间&结束时间
    memcpy(&uteModuleSprotData.moreSportData.saveData.startSportTime,&mUTESportDatas.startSportTime,7);
    memcpy(&uteModuleSprotData.moreSportData.saveData.stopSportTime,&mUTESportDatas.stopSportTime,7);
#endif

    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
/**
*@brief     多运动每秒处理心率数据
*@details
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportMoreSportsHeartDataHandler(void)
{
#if UTE_MODULE_HEART_SUPPORT
    if(!vc30fx_usr_get_work_status() || uteModuleHeartGetWorkMode() != WORK_MODE_HR)
    {
        uteModuleHeartStartSingleTesting(TYPE_HEART);
    }
    uint16_t saveTimeInterval = uteModuleSprotData.moreSportData.saveData.startInterval*10;
    uint8_t heart = (uint8_t)uteModuleHeartGetHeartValue();
    if((uteModuleSprotData.moreSportData.totalSportTime%saveTimeInterval==0 )&&
       (uteModuleSprotData.moreSportData.status != ALL_SPORT_STATUS_PAUSE))
    {
        //Casen add 20-11-09
        uteModulePlatformTakeMutex(uteModuleSprotMute);
        if(heart != 0 && heart != 0xFF)
        {
            uteModuleSprotData.moreSportData.heartCache[uteModuleSprotData.moreSportData.heartTotalCnt%SPORTS_HEART_DATAS_CACHE_LENGTH] = heart;
            if((uteModuleSprotData.moreSportData.heartTotalCnt%SPORTS_HEART_DATAS_CACHE_LENGTH)==0 && uteModuleSprotData.moreSportData.heartTotalCnt!=0)
            {
                /*! 保存数据 zn.zeng, 2021-11-11  */
                uteModuleSportMoreSportsSaveData(false);
            }
            //心率 MAX MIN AVG
            if(heart>uteModuleSprotData.moreSportData.saveData.maxHeartRate)
            {
                uteModuleSprotData.moreSportData.saveData.maxHeartRate = heart;
            }
            if(heart<uteModuleSprotData.moreSportData.saveData.minHeartRate)
            {
                uteModuleSprotData.moreSportData.saveData.minHeartRate = heart;
            }
            uteModuleSprotData.moreSportData.heartTotalSum += heart;
            uteModuleSprotData.moreSportData.heartTotalCnt++;
            uteModuleSprotData.moreSportData.saveData.avgHeartRate = uteModuleSprotData.moreSportData.heartTotalSum/uteModuleSprotData.moreSportData.heartTotalCnt;
        }
        uteModulePlatformGiveMutex(uteModuleSprotMute);
    }
#endif
}
/**
*@brief     多运动每秒发送数据给app
*@details
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportMoreSportsSendRealTimeData(void)
{
    if(uteModuleSprotData.moreSportData.isAppStart)
    {
#if UTE_MODULE_HEART_SUPPORT
        static uint8_t tmpSendData[14];
        bool   updateRealTimeData = false;
        uint8_t heart = (uint8_t)uteModuleHeartGetHeartValue();
        if(heart != tmpSendData[2])
        {
            updateRealTimeData = true;
        }
        else if(uteModuleSprotData.moreSportData.saveData.sportCaloire != ((tmpSendData[3]<<8)+tmpSendData[4]))
        {
            updateRealTimeData = true;
        }
        else if((uteModuleSprotData.moreSportData.saveData.avgTimeMinute!=tmpSendData[5]) || (uteModuleSprotData.moreSportData.saveData.avgTimeSecond!=tmpSendData[6]))
        {
            updateRealTimeData = true;
        }
        else if(uteModuleSprotData.moreSportData.saveData.sportStep != ((tmpSendData[7]<<16)+(tmpSendData[8]<<8)+tmpSendData[9]))
        {
            updateRealTimeData = true;
        }
        else if(uteModuleSprotData.moreSportData.saveData.sportTimes != ((tmpSendData[10]<<8)+tmpSendData[11]))
        {
            updateRealTimeData = true;
        }
        else if((uteModuleSprotData.moreSportData.saveData.sportDistanceInteger!=tmpSendData[12]) || (uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals!=tmpSendData[13]))
        {
            updateRealTimeData = true;
        }
        if(updateRealTimeData)
        {
            UTE_MODULE_LOG(1,"RealTimeData heart =%d",heart);
            tmpSendData[0] = CMD_SPORT_MODE_AND_SPORT_HEART_RATE;   //指令头标志
            tmpSendData[1] = uteModuleSprotData.moreSportData.saveData.sportsType;
            tmpSendData[2] = heart;
            tmpSendData[3] = uteModuleSprotData.moreSportData.saveData.sportCaloire>>8;
            tmpSendData[4] = uteModuleSprotData.moreSportData.saveData.sportCaloire;
            tmpSendData[5] = uteModuleSprotData.moreSportData.saveData.avgTimeMinute;
            tmpSendData[6] = uteModuleSprotData.moreSportData.saveData.avgTimeSecond;
            tmpSendData[7] = uteModuleSprotData.moreSportData.saveData.sportStep>>16;
            tmpSendData[8] = uteModuleSprotData.moreSportData.saveData.sportStep>>8;
            tmpSendData[9] = uteModuleSprotData.moreSportData.saveData.sportStep;
#if (UTE_MODULE_SPORT_WITH_TIMES_SUPPORT)
            tmpSendData[10] = uteModuleSprotData.moreSportData.saveData.sportTimes>>8;
            tmpSendData[11] = uteModuleSprotData.moreSportData.saveData.sportTimes;
#else
            tmpSendData[10] = 0;
            tmpSendData[11] = 0;
#endif
            tmpSendData[12] = uteModuleSprotData.moreSportData.saveData.sportDistanceInteger;
            tmpSendData[13] = uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals;
            uteModuleProfileBleSendToPhone(&tmpSendData[0],14);
        }
#endif
    }
}
/**
*@brief     多运动每秒处理
*@details
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportMoreSportsEverySecond(ute_module_systemtime_time_t *time)
{
    /*! 低电量自动停止运动 zn.zeng, 2021-11-11  */
    if ((uteModuleSprotData.moreSportData.status == ALL_SPORT_STATUS_OPEN) &&
        (uteModuleSprotData.moreSportData.countZeroIndex == -1) &&
        uteDrvBatteryCommonGetLvl() < UTE_DRV_BATTERY_LOW_POWER_PERECNT)
    {
        uteModuleSportStopMoreSportsMsgHandler();
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        sys_cb.cover_index = REMIND_COVER_LOW_BATTERY;
        sys_cb.remind_tag = true;
//        msgbox((char*)i18n[STR_LOW_BATTERY], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_LOW_BATTERY);
#else
        // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID);              //低电停止运动返回表盘
        func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
#endif
    }
    if(uteModuleSprotData.moreSportData.status!=ALL_SPORT_STATUS_CLOSE)
    {
        if(uteModuleSprotData.moreSportData.countZeroIndex==0)
        {
            struct prf_date_time sportTime;
            sportTime.year = time->year;
            sportTime.month = time->month;
            sportTime.day = time->day;
            sportTime.hour = time->hour;
            sportTime.min = time->min;
            sportTime.sec = time->sec;
            ute_personal_info_t info;
            uteApplicationCommonGetPersonalInfo(&info);
            //Casen add 22-06-15
            info.weight = (info.weight/1000.0f);
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
            if(0)
            {}
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
            else if(uteModuleSprotData.moreSportData.saveData.sportsType>SPORT_TYPE_FREE_TRAINING)
            {
                if ((uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_INDOOR_WALK) ||
                    (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_STEP_TRAINING) ||
                    (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_OUTDOOR_WALK) ||
                    (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_HIKING)) //室内走路/踏步/户外健走/徒步
                {
                    enableUTESport(SPORT_TYPE_WALKING,sportTime,info.isMan,info.weight,info.height);
                }
                else if ((uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_INDOOR_RUN) ||
                         (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_PARKOUR) ||
                         (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_MARATHON)) //室内跑步/跑酷/马拉松
                {
                    enableUTESport(SPORT_TYPE_RUNNING,sportTime,info.isMan,info.weight,info.height);
                }
                else
                {
                    enableUTESport(SPORT_TYPE_FREE_TRAINING,sportTime,info.isMan,info.weight,info.height);
                }
            }
#endif
            else
            {
                enableUTESport((sport_mode_type)uteModuleSprotData.moreSportData.saveData.sportsType,sportTime,info.isMan,info.weight,info.height);
            }
#endif
            uteModuleSprotData.moreSportData.countZeroIndex = -1;
        }
        else if(uteModuleSprotData.moreSportData.countZeroIndex==-1)
        {
            if(uteModuleSprotData.moreSportData.status != ALL_SPORT_STATUS_PAUSE)
            {
                //每秒获取一次运动数据
                uteModuleSportMoreSportsGetDataFormAlgo(time);
#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
                uteModuleSportMoreSportsHeartWarningProcess();
#endif
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
                if(uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_SPORTS_DETAIL_ID)
                {
                    uteModuleSportMoreSportsTargetsProcess();
                }
#endif
            }
            //心率数据
            uteModuleSportMoreSportsHeartDataHandler();
            /*运动跨越天数的时候：
             *1，将前一天的数据保存
             *2，关闭运动算法
             *3，sportHeartRateCnt,sportHeartRateSum,heartRateDataLength清零
             *4，开启运动算法
             */
            if((time->hour==23)&&(time->min==59)&&(time->sec==59))
            {
                uteModuleSportStopMoreSportsMsgHandler();
                // 下一秒开始的参数
                // uteModuleSprotData.moreSportData.status = ALL_SPORT_STATUS_OPEN;
                // uteModuleSprotData.moreSportData.countZeroIndex = 0;
                // uteModuleSprotData.moreSportData.saveData.avgHeartRate = 0;
                // uteModuleSprotData.moreSportData.saveData.maxHeartRate = 0;
                // uteModuleSprotData.moreSportData.saveData.minHeartRate = 0xff;
                // uteModuleSprotData.moreSportData.heartTotalSum = 0;
                // uteModuleSprotData.moreSportData.heartTotalCnt = 0;
                // uteModuleSprotData.moreSportData.totalSportTime = 0;
            }
            /*! 发送实时数据到app zn.zeng, 2021-11-11  */
            uteModuleSportMoreSportsSendRealTimeData();
            /*! 断开超时 zn.zeng, 2021-11-11  */
            if((uteModuleSprotData.moreSportData.isAppStart)&& /*App开启的运动*/
               (!uteModuleSprotData.moreSportData.isAppStartSportReConnection)&&           /*断开没有收到APP运动继续的时候*/
               (uteModuleSprotData.moreSportData.status==ALL_SPORT_STATUS_PAUSE) /*进入到暂停界面*/
              )
            {
                uteModuleSprotData.moreSportData.appStartSportDisconnectTimeOutCnt++;
                if(uteModuleSprotData.moreSportData.appStartSportDisconnectTimeOutCnt>=300)
                {
                    uteModuleSportStopMoreSportsMsgHandler();
                    /*! 跳转界面zn.zeng, 2021-11-11  */
                    if(uteModuleSportMoreSportsIsLessData())
                    {
                        // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TRAINING_LIST_ID); //退回运动列表
                        void func_switch_to(u8 sta, u16 switch_mode);
//                        func_switch_to(FUNC_SPORT, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                        func_cb.sta = FUNC_SPORT;
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
                    }
                    else
                    {
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
                        uteModuleSprotData.sportsHistoryRecord.displayIndex = 0;
#endif
                        /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
                        if(!uteApplicationCommonIsHasConnectOurApp())
                        {
                            // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TRAINING_LIST_ID);  //退回运动列表
//                            extern u8 task_stack_pop(void);
//                            task_stack_pop();
                            void func_switch_to(u8 sta, u16 switch_mode);
//                            func_switch_to(FUNC_SPORT, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                            func_cb.sta = FUNC_SPORT;
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
                        }
                        else
                        {
                            // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORTS_REPORTS_ID);     //运动完成数据弹窗
                            func_cb.sta = FUNC_SPORT_FINISH;       //todo 结束报告界面，暂时没有做，直接返回运动列表
//                            sys_cb.cover_index = REMIND_COVER_GOAL;
//                            sys_cb.remind_tag = true;

                        }
                    }
                }
            }
            else
            {
                uteModuleSprotData.moreSportData.appStartSportDisconnectTimeOutCnt = 0;
            }
        }
    }
}

#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
/**
*@brief     重新计算每小时运动数据
*@details
*@author      xjc
*@date       2023-05-31
*/
void uteModuleSportCalculatorEveryHourSportDataHandler(bool isFinish)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if (((uteModuleSprotData.moreSportData.status != ALL_SPORT_STATUS_CLOSE) && (uteModuleSprotData.moreSportData.countZeroIndex == (-1))) ||
        (isFinish))
    {
        /*! 有步数的运动*/
        if ((uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_RUNNING)      /*跑步 */
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_CLIMBING)  /*登山*/
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_WALKING)   /*健走|徒步 */
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_TREADMILL) /*跑步机 */
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_INDOOR_WALK)   /*室内走路 */
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_STEP_TRAINING) /*踏步 */
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_OUTDOOR_WALK)  /*户外健走 */
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_HIKING)        /*徒步 */
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_INDOOR_RUN)    /*室内跑步 */
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_PARKOUR)       /*跑酷 */
            || (uteModuleSprotData.moreSportData.saveData.sportsType == SPORT_TYPE_MARATHON)      /*马拉松 */
#endif
           )
        {
            /*! 跨小时保存数据*/
            if (((time.min == 59) && (time.sec == 59)) || (isFinish))
            {
                uint32_t tempStep, tempKcal;
                tempStep = uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[time.hour];
                tempKcal = uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal[time.hour];
                if (time.hour == uteModuleSprotData.moreSportData.saveData.startSportTime.hour)
                {
                    /*保存第一个小时数据*/
                    tempStep += uteModuleSprotData.moreSportData.saveData.sportStep;
                    tempKcal += uteModuleSprotData.moreSportData.saveData.sportCaloire;
                }
                else
                {
                    /*保存其它小时数据*/
                    tempStep += (uteModuleSprotData.moreSportData.saveData.sportStep - uteModuleSprotData.dailyActivity.lastHourSportStep);
                    tempKcal += (uteModuleSprotData.moreSportData.saveData.sportCaloire - uteModuleSprotData.dailyActivity.lastHourSportKcal);
                }
                uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[time.hour] = tempStep;
                uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal[time.hour] = tempKcal;

                uteModuleSprotData.dailyActivity.lastHourSportStep = uteModuleSprotData.moreSportData.saveData.sportStep;
                uteModuleSprotData.dailyActivity.lastHourSportKcal = uteModuleSprotData.moreSportData.saveData.sportCaloire;

                uteModuleSprotData.dailyActivity.needSaveCurrentDayEveryHourSportData = true;
            }
        }
    }

    if (isFinish && uteModuleSprotData.dailyActivity.needSaveCurrentDayEveryHourSportData)
    {
        uteModuleSportSaveEveryHourData();
    }
}

/**
 *@brief     保存多运动每小时数据
 *@details
 *@author       xjc
 *@date       2023-05-31
 */
void uteModuleSportSaveEveryHourData(void)
{
    if (!uteModuleSprotData.dailyActivity.needSaveCurrentDayEveryHourSportData)
    {
        return;
    }
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_SPORT_STEP_DATA, &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[0], 24 * sizeof(uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[0]));
        uteModuleFilesystemCloseFile(file);
    }
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_SPORT_KCAL_DATA, &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal[0], 24 * sizeof(uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal[0]));
        uteModuleFilesystemCloseFile(file);
    }
    memset(&uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[0], 0, sizeof(uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep));
    memset(&uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal[0], 0, sizeof(uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal));
    uteModuleSprotData.dailyActivity.needSaveCurrentDayEveryHourSportData = false;
}

/**
 *@brief     读取多运动每小时数据
 *@details
 *@author       xjc
 *@date       2023-05-31
 */
void uteModuleSportReadEveryHourData(void)
{
    void *file;
    memset(&uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[0], 0, sizeof(uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep));
    memset(&uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal[0], 0, sizeof(uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal));
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_SPORT_STEP_DATA, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[0], 24 * sizeof(uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[0]));
        uteModuleFilesystemCloseFile(file);
    }
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_SPORT_KCAL_DATA, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal[0], 24 * sizeof(uteModuleSprotData.dailyActivity.currentDayEveryHourSportKcal[0]));
        uteModuleFilesystemCloseFile(file);
    }
}
#endif

/**
*@brief     开始多运动消息处理
*@details
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportStartMoreSportsMsgHandler(uint32_t param)
{
    uint8_t sportsType = (param>>24)&0xff;
    uint8_t startInterval = (param>>16)&0xff;
    bool isAppStart = (param>>8)&0xff;
    bool isAppRestart = false;
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sportsType=%d,startInterval=%d,isAppStart=%d,isAppRestart=%d", __func__,sportsType,startInterval,isAppStart,isAppRestart);
    if((uteModuleSprotData.moreSportData.status==ALL_SPORT_STATUS_OPEN)&&isAppStart)
    {
        isAppRestart = true;
        uteModuleSprotCountdownTimerStop();
        uteModuleSprotCountdownTimerStart(1000);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sportsType1=%d,startInterval1=%d,isAppStart1=%d,isAppRestart1=%d", __func__,sportsType,startInterval,isAppStart,isAppRestart);
    }
    if(isAppRestart)
    {
//        uteModuleSportStopMoreSportsMsgHandler();
        //关闭运动算法
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
        uteModuleCwmStopSportMode();
#else
        disableUTESport();
#endif
        if(!uteModuleSportMoreSportsIsLessData())
        {
            /*! 保存数据 zn.zeng, 2021-11-11  */
            uteModuleSportMoreSportsSaveData(true);
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
            uteModuleSportReadHistoryRecordData();
#endif
#if  UTE_MODULE_SPORTS_HISTORY_HEART_ZONE_SUPPORT
            ute_module_systemtime_time_t time;
            memset(&time,0,sizeof(ute_module_systemtime_time_t));
            time.year = uteModuleSprotData.moreSportData.saveData.startSportTime.year;
            time.month = uteModuleSprotData.moreSportData.saveData.startSportTime.month;
            time.day = uteModuleSprotData.moreSportData.saveData.startSportTime.day;
            time.hour = uteModuleSprotData.moreSportData.saveData.startSportTime.hour;
            time.min = uteModuleSprotData.moreSportData.saveData.startSportTime.min;
            time.sec = uteModuleSprotData.moreSportData.saveData.startSportTime.sec;
            uteModuleSportReadHistoryHeartZoneData(&time);
#endif
        }
        else
        {
            uteModuleSportMoreSportsDeleteLessData();
        }
        uteModuleSprotData.moreSportData.status = ALL_SPORT_STATUS_CLOSE;
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
    }
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCwmStartSportMode(sportsType);   //开始运动
#endif
    memset(&uteModuleSprotData.moreSportData,0,sizeof(ute_module_more_sports_data_t));
    uteModuleSprotData.moreSportData.isAppStart = isAppStart;
    uteModuleSprotData.moreSportData.status = ALL_SPORT_STATUS_OPEN;
    uteModuleSprotData.moreSportData.saveData.sportsType = sportsType;
    uteModuleSprotData.moreSportData.saveData.startInterval = startInterval;
    uteModuleSprotData.moreSportData.saveData.avgHeartRate = 0;
    uteModuleSprotData.moreSportData.saveData.maxHeartRate = 0;
    uteModuleSprotData.moreSportData.saveData.minHeartRate = 0xff;
    uteModuleSprotData.moreSportData.countZeroIndex = UTE_MODULE_SPROT_MORE_SPORT_COUNT_ZORE;
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartStartSingleTesting(TYPE_HEART);
#endif
    if(uteModuleSprotData.moreSportData.countZeroIndex > 0)
    {
        uteModuleSprotCountdownTimerStart(1000);
    }
    if(isAppRestart)
    {
        uteModuleSprotData.moreSportData.isAppStartSportReConnection = true;
    }
    uteModuleSprotData.moreSportData.appStartSportDisconnectTimeOutCnt = 0;
    uteModuleSportSendMoreSportsStatus();
    if(!isAppRestart)
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
    }
    /*! 跳转界面zn.zeng, 2021-11-11  */
    // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORTS_DETAIL_ID);      //运动RUNING界面
    //func_switch_to(FUNC_SPORT_SUB_RUN, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
    func_cb.sta = FUNC_SPORT_SUB_RUN;
    sys_cb.sport_app_disconnect = false;
    if(sportsType==SPORT_TYPE_JUMP_ROPE)
    {
        // uteDrvHeartVcxxStopSample();
        // uteDrvHeartVcxxStartSample();
        uteDrvGsensorCommonInit(GSENSOR_RATE_100HZ,UTE_DRV_GSENSOR_DEFAULT_ACC_RANGE_VALUE);
        uteModuleSprotResetRovllverScreenMode();
    }

#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
    uteModuleSportReadEveryHourData();
#endif
}
/**
*@brief     结束多运动
*@details
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportStopMoreSports(void)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_SPORT_STOP_MORE_SPORTS,0);
}
/**
*@brief     判断运动量是否过少
*@details
*@return ture 为运动量过少
*@author       zn.zeng
*@date       2021-11-11
*/

bool uteModuleSportMoreSportsIsLessData(void)
{
    bool ret = false;
//  return ret;
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    if(uteModuleSprotData.moreSportData.totalSportTime<UTE_MODULE_SPORT_MORE_SPORT_MIN_SAVE_TIME)
    {
        ret = true;
    }
    uint32_t tempDistance = 0;
    float distance = 0;
    distance = (float)(uteModuleSprotData.moreSportData.saveData.sportDistanceInteger + (float)(uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals*0.01));
    distance = distance*1000.0f; //放大1000倍,m
    tempDistance = (uint32_t)distance;
    // 1 距离等于零，卡路里等于零
    if((tempDistance==0)&&(uteModuleSprotData.moreSportData.saveData.sportCaloire==0))
    {
        ret =  true;  // 不需要保存数据
    }
    // 2 距离小于100m
    if((tempDistance!=0)&&(tempDistance<100))
    {
        ret = true;  // 不需要保存数据
    }
    // 3 卡路里不等于零，时间小于1分钟
    if((uteModuleSprotData.moreSportData.saveData.sportCaloire!=0)&&(uteModuleSprotData.moreSportData.totalSportTime<UTE_MODULE_SPORT_MORE_SPORT_MIN_SAVE_TIME))
    {
        ret = true;  // 不需要保存数据
    }
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uint16_t stopType = uteModuleSprotData.moreSportData.saveData.sportsType;
    if ((stopType == SPORT_TYPE_RUNNING)          /*跑步 */
        || (stopType == SPORT_TYPE_CLIMBING)      /*登山*/
        || (stopType == SPORT_TYPE_WALKING)       /*健走|徒步 */
        || (stopType == SPORT_TYPE_TREADMILL)     /*跑步机 */
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
        || (stopType == SPORT_TYPE_INDOOR_WALK)   /*室内走路 */
        || (stopType == SPORT_TYPE_STEP_TRAINING) /*踏步 */
        || (stopType == SPORT_TYPE_OUTDOOR_WALK)  /*户外健走 */
        || (stopType == SPORT_TYPE_HIKING)        /*徒步 */
        || (stopType == SPORT_TYPE_INDOOR_RUN)    /*室内跑步 */
        || (stopType == SPORT_TYPE_PARKOUR)       /*跑酷 */
        || (stopType == SPORT_TYPE_MARATHON)      /*马拉松 */
#endif
       )
    {
        if(tempDistance<100)
        {
            ret = true;
        }
    }
#endif
    uteModulePlatformGiveMutex(uteModuleSprotMute);

#if UTE_MODULE_LOG_SUPPORT&&UTE_LOG_GUI_LVL
    ret = false;
#endif
    return ret;
}
/**
*@brief     填充保存多运动数据
*@details
*@param[out] uint8_t *buff
*@author       zn.zeng
*@date       2021-11-12
*/
void uteModuleSportFillMoreSportsSaveData(uint8_t *buff)
{
    ute_module_systemtime_time_t globalTime;
    uteModuleSystemtimeGetTime(&globalTime);
    buff[0] = uteModuleSprotData.moreSportData.saveData.startInterval;
    buff[1] = uteModuleSprotData.moreSportData.saveData.sportsType;
    buff[2] = uteModuleSprotData.moreSportData.saveData.startSportTime.year>>8&0xff;
    buff[3] = uteModuleSprotData.moreSportData.saveData.startSportTime.year&0xff;
    buff[4] = uteModuleSprotData.moreSportData.saveData.startSportTime.month;
    buff[5] = uteModuleSprotData.moreSportData.saveData.startSportTime.day;
    buff[6] = uteModuleSprotData.moreSportData.saveData.startSportTime.hour;
    buff[7] = uteModuleSprotData.moreSportData.saveData.startSportTime.min;
    buff[8] = uteModuleSprotData.moreSportData.saveData.startSportTime.sec;
    tm_t timeptr;
    timeptr.year = uteModuleSprotData.moreSportData.saveData.stopSportTime.year-1900;
    timeptr.mon = uteModuleSprotData.moreSportData.saveData.stopSportTime.month-1;
    timeptr.day = uteModuleSprotData.moreSportData.saveData.stopSportTime.day;
    timeptr.hour = uteModuleSprotData.moreSportData.saveData.stopSportTime.hour;
    timeptr.min = uteModuleSprotData.moreSportData.saveData.stopSportTime.min;
    timeptr.sec = uteModuleSprotData.moreSportData.saveData.stopSportTime.sec;
    time_t tmp = tm_to_time(timeptr);
    //Casen 在原时间基础上强制转换时间到GMT+8 存储
    tmp = tmp - globalTime.zone*3600 + (globalTime.zone-8)*3600;
    buff[9] = (tmp>>24&0x000000FF);
    buff[10] = (tmp>>16&0x000000FF);
    buff[11] = (tmp>>8&0x000000FF);
    buff[12] = (tmp&0x000000FF);
    buff[13] = (uteModuleSprotData.moreSportData.totalSportTime>>16&0xFF);
    buff[14] = (uteModuleSprotData.moreSportData.totalSportTime>>8);
    buff[15] = uteModuleSprotData.moreSportData.totalSportTime&0xFF;
    buff[16] = uteModuleSprotData.moreSportData.saveData.sportStep>>24&0xff;
    buff[17] = uteModuleSprotData.moreSportData.saveData.sportStep>>16&0xff;
    buff[18] = uteModuleSprotData.moreSportData.saveData.sportStep>>8&0xff;
    buff[19] = uteModuleSprotData.moreSportData.saveData.sportStep&0xff;
#if (UTE_MODULE_SPORT_WITH_TIMES_SUPPORT)
    buff[20] = uteModuleSprotData.moreSportData.saveData.sportTimes>>8&0xff;
    buff[21] = uteModuleSprotData.moreSportData.saveData.sportTimes&0xff;
#else
    buff[20] = 0;
    buff[21] = 0;
#endif
    buff[22] = uteModuleSprotData.moreSportData.saveData.sportCaloire>>8&0xff;
    buff[23] = uteModuleSprotData.moreSportData.saveData.sportCaloire;
    buff[24] = uteModuleSprotData.moreSportData.saveData.sportDistanceInteger;
    buff[25] = uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals;
    buff[26] = uteModuleSprotData.moreSportData.saveData.avgHeartRate;
    buff[27] = uteModuleSprotData.moreSportData.saveData.maxHeartRate;
    buff[28] = uteModuleSprotData.moreSportData.saveData.minHeartRate;
#if UTE_MODULE_SPORT_SAVE_AVE_PACE_SUPPORT
    //num of pace
    uint16_t tempPace = 0;
    float totalDistance = 0;
    totalDistance = (float)(uteModuleSprotData.moreSportData.saveData.sportDistanceInteger + (float)(uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals * 0.01));
    totalDistance = ExactDecimalPoint(totalDistance, 2);
    float pace = 0;
    pace = ((float)uteModuleSprotData.moreSportData.totalSportTime)/totalDistance;
    pace = pace + 0.5f;//4舍5入
    pace = ExactDecimalPoint(pace, 2);
    tempPace = (uint16_t)pace;
    uint8_t min = tempPace/60;
    uint8_t sec = tempPace%60;
    if (min > 254)
    {
        min = 254;
    }
    if (sec > 59)
    {
        sec = 59;
    }
    buff[29] = min;
    buff[30] = sec;
#else
    buff[29] = uteModuleSprotData.moreSportData.saveData.avgTimeMinute;
    buff[30] = uteModuleSprotData.moreSportData.saveData.avgTimeSecond;
#endif
    buff[31] = uteModuleSprotData.moreSportData.heartTotalCnt>>8&0xff;
    buff[32] = uteModuleSprotData.moreSportData.heartTotalCnt&0xff;
}
/**
*@brief     保存多运动数据
*@details
*@param[in] bool isStopSport
*@author       zn.zeng
*@date       2021-11-11
*/
void uteModuleSportMoreSportsSaveData(bool isStopSport)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    // if(!uteApplicationCommonIsHasConnectOurApp())
    // {
    //     return;
    // }
    uint8_t path[40];
    memset(&path[0],0,sizeof(path));
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;

    time.year = uteModuleSprotData.moreSportData.saveData.startSportTime.year;
    time.month = uteModuleSprotData.moreSportData.saveData.startSportTime.month;
    time.day = uteModuleSprotData.moreSportData.saveData.startSportTime.day;
    time.hour = uteModuleSprotData.moreSportData.saveData.startSportTime.hour;
    time.min = uteModuleSprotData.moreSportData.saveData.startSportTime.min;

    sprintf((char *)&path[0],"%04d%02d%02d%02d%02d",time.year,time.month,time.day,time.hour,time.min);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR, dirInfo, NULL);
    if((dirInfo->filesCnt>=UTE_MODULE_SPORT_MORE_SPORTS_DATA_MAX_CNT)&&(memcmp(&path[0],&dirInfo->filesName[0][0],12)!=0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-11-12   */
        memset(&path[0],0,sizeof(path));
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR,&dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0],0,sizeof(path));
    sprintf((char *)&path[0],"%s/%04d%02d%02d%02d%02d",UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR,time.year,time.month,time.day,time.hour,time.min);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,save file=%s", __func__,&path[0]);
    /*! 保存当前数据zn.zeng, 2021-11-12  */
    void *file;
    int32_t fileSize = 0;
    uint8_t headBuff[33];
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_END);
        fileSize = uteModuleFilesystemGetFileSize(file);
        if(isStopSport)
        {
            // 其他数据
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleSportFillMoreSportsSaveData(&headBuff[0]);
            uteModuleFilesystemWriteData(file,&headBuff[0],33);
            // 心率数据
            uteModuleFilesystemSeek(file,0,FS_SEEK_END);
            uint16_t heartCacheCnt = uteModuleSprotData.moreSportData.heartTotalCnt%SPORTS_HEART_DATAS_CACHE_LENGTH;
            uteModuleFilesystemWriteData(file,&uteModuleSprotData.moreSportData.heartCache[0],heartCacheCnt);
        }
        else
        {
            if(fileSize==0)
            {
                // 其他数据
                uteModuleSportFillMoreSportsSaveData(&headBuff[0]);
                uteModuleFilesystemWriteData(file,&headBuff[0],33);
            }
            uteModuleFilesystemSeek(file,0,FS_SEEK_END);
            uteModuleFilesystemWriteData(file,&uteModuleSprotData.moreSportData.heartCache[0],SPORTS_HEART_DATAS_CACHE_LENGTH);
        }
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(dirInfo);

#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
    if(isStopSport)
    {
        // ute_module_systemtime_time_t currTime;
        // uteModuleSystemtimeGetTime(&currTime);
        // uteModuleSportReadTodayAllSportDataConfig(currTime);
        uteModuleSprotData.updateTodayAllSportData = true;
    }
#endif
}

/**
*@brief     删除缓存心率数据时保存的运动过少的数据
*@details
*@author     dengli.lu
*@date       2023-11-23
*/
void uteModuleSportMoreSportsDeleteLessData(void)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s, heartTotalCnt = %d", __func__,uteModuleSprotData.moreSportData.heartTotalCnt);
    if(uteModuleSprotData.moreSportData.heartTotalCnt>=SPORTS_HEART_DATAS_CACHE_LENGTH)
    {
        uint8_t path[40];
        memset(&path[0],0,sizeof(path));
        ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
        ute_module_systemtime_time_t time;
        time.year = uteModuleSprotData.moreSportData.saveData.startSportTime.year;
        time.month = uteModuleSprotData.moreSportData.saveData.startSportTime.month;
        time.day = uteModuleSprotData.moreSportData.saveData.startSportTime.day;
        time.hour = uteModuleSprotData.moreSportData.saveData.startSportTime.hour;
        time.min = uteModuleSprotData.moreSportData.saveData.startSportTime.min;
        sprintf((char *)&path[0],"%04d%02d%02d%02d%02d",time.year,time.month,time.day,time.hour,time.min);
        uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR, dirInfo, NULL);
        if(dirInfo->filesCnt > 0)
        {
            if(memcmp(&path[0],&dirInfo->filesName[dirInfo->filesCnt-1][0],12) == 0)
            {
                /*! 删除不要保存的数据*/
                memset(&path[0],0,30);
                sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR,&dirInfo->filesName[dirInfo->filesCnt-1][0]);
                uteModuleFilesystemDelFile((char *)&path[0]);
                UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,del file=%s", __func__,&path[0]);
            }
        }
        uteModulePlatformMemoryFree(dirInfo);
    }
}

/**
*@brief     结束多运动消息处理
*@details   改方法仅作为结束运动动作，请不要在其中实现UI跳转
*@author       zn.zeng
*@date      casen modify 2021-12-30
*/
void uteModuleSportStopMoreSportsMsgHandler(void)
{
    //关闭运动算法
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCwmStopSportMode();
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uteModuleSportMoreSportsGetDataFormAlgo(&time);
#else
    disableUTESport();
#endif
#if UTE_MODULE_HEART_SUPPORT
    if (vc30fx_usr_get_work_status() && uteModuleHeartGetWorkMode() == WORK_MODE_HR)
    {
        uteModuleHeartStopSingleTesting(TYPE_HEART);
    }
#endif
    if(!uteModuleSportMoreSportsIsLessData())
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s ", __func__);
        /*! 保存数据 zn.zeng, 2021-11-11  */
        uteModuleSportMoreSportsSaveData(true);
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
        uteModuleSportReadHistoryRecordData();
#endif
#if  UTE_MODULE_SPORTS_HISTORY_HEART_ZONE_SUPPORT
        ute_module_systemtime_time_t time;
        memset(&time,0,sizeof(ute_module_systemtime_time_t));
        time.year = uteModuleSprotData.moreSportData.saveData.startSportTime.year;
        time.month = uteModuleSprotData.moreSportData.saveData.startSportTime.month;
        time.day = uteModuleSprotData.moreSportData.saveData.startSportTime.day;
        time.hour = uteModuleSprotData.moreSportData.saveData.startSportTime.hour;
        time.min = uteModuleSprotData.moreSportData.saveData.startSportTime.min;
        time.sec = uteModuleSprotData.moreSportData.saveData.startSportTime.sec;
        uteModuleSportReadHistoryHeartZoneData(&time);
#endif
    }
    else
    {
        uteModuleSportMoreSportsDeleteLessData();
    }
    uteModuleSprotData.moreSportData.status = ALL_SPORT_STATUS_CLOSE;
    uteModuleSprotData.moreSportData.isAppStart = false;
    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
    uteModuleSportSendMoreSportsStatus();

    if(uteModuleSprotData.moreSportData.saveData.sportsType==SPORT_TYPE_JUMP_ROPE)
    {
        // uteDrvHeartVcxxStopSample();
        // uteDrvHeartVcxxStartSample();
        uteDrvGsensorCommonInit(UTE_DRV_GSENSOR_DEFAULT_ACC_RATE_VALUE,UTE_DRV_GSENSOR_DEFAULT_ACC_RANGE_VALUE);
#if UTE_MODULE_SPORT_APP_STOP_SPORT_STOP_HEART_TEST_SUPPORT
        if(!uteModuleHeartIsAutoTesting())
        {
            uteDrvHeartVcxxStopSample();
        }
#endif
    }

    /*! 界面跳转, xjc Mark 20220111 */
    if(uteModuleSportMoreSportsIsLessData())
    {
        // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TRAINING_LIST_ID);      //运动列表
//        func_switch_to(FUNC_SPORT, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
        func_cb.sta = FUNC_SPORT;
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
    }
    else
    {
#if UTE_MODULE_SCREENS_SPORTS_REPORT_LIST_SUPPORT
        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORTS_REPORT_LIST_ID);
#else
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
        uteModuleSprotData.sportsHistoryRecord.displayIndex = 0;
#endif
        // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORTS_REPORTS_ID);     //运动完成数据界面
        func_cb.sta = FUNC_SPORT_FINISH;//FUNC_SPORT;       //todo 结束报告界面，暂时没有做，直接返回运动列表
//        sys_cb.cover_index = REMIND_COVER_GOAL;
//        sys_cb.remind_tag = true;
#endif
    }
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
    uteModuleSportMoreSportsTargetInit();
#endif
}
/**
*@brief     app 同步运动数据到ble
*@details
*@author       zn.zeng
*@date       2021-11-13
*/
void uteModuleSportSyncAppMoreSportData(const uint8_t *receive)
{
    uteModulePlatformTakeMutex(uteModuleSprotMute);

    //set intervel
    uteModuleSprotData.moreSportData.saveData.startInterval = receive[3];
    //set sport time
    uteModuleSprotData.moreSportData.totalSportTime = (receive[4]*3600)+(receive[5]*60) + receive[6]; // hour/min/sec
    if (uteModuleSportIsPhoneAppStartGpsSportsType())
    {
        // kcal
        uteModuleSprotData.moreSportData.saveData.sportCaloire = (receive[7]<<8&0xff00) | receive[8];
        //distance
        uteModuleSprotData.moreSportData.saveData.sportDistanceInteger = receive[9];
        uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals =receive[10];
        //avgTime
        uteModuleSprotData.moreSportData.saveData.avgTimeMinute = receive[11];
        uteModuleSprotData.moreSportData.saveData.avgTimeSecond = receive[12];
    }
    uteModuleSprotData.moreSportData.isAppStartSportReConnection = true;
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
/**
*@brief        开始发送多运动数据
*@details
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleSportSendMoreSportHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t response[20];
    uint8_t sendSize = 0;
    uint8_t path[40];
    bool isSendFinish = false;
    memset(&path[0],0,sizeof(path));
    response[0] = CMD_SPORT_MODE_AND_SPORT_HEART_RATE;
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currSendFileIndex=%d,currSendMinIndex=%d,dirInfo.filesCnt=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendMinIndex,sendParam->dirInfo.filesCnt);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sendParam->currSendFileSize=%d,currSendFileDataOffset=%d", __func__,sendParam->currSendFileSize,sendParam->currSendFileDataOffset);
    if(((sendParam->currSendFileSize==sendParam->currSendFileDataOffset)&&(sendParam->currSendFileSize!=0))
       ||(sendParam->dirInfo.filesCnt==0)
       ||(sendParam->currSendFileIndex==sendParam->dirInfo.filesCnt))
    {
        sendSize = 3;
        response[1] = 0xfd;
        response[2] = sendParam->crc;
        sendParam->crc = 0;
        sendParam->currSendMinIndex = 0;
        sendParam->currSendFileDataOffset = 0;
        sendParam->currSendFileIndex++;
        if(sendParam->currSendFileIndex>=sendParam->dirInfo.filesCnt)
        {
            uteApplicationCommonSyncDataTimerStop();
        }
        isSendFinish = true;
    }
    if(!isSendFinish)
    {
        void *file;
        if(sendParam->dirInfo.filesCnt>0)
        {
            if(sendParam->currSendMinIndex == 0)
            {
                snprintf((char *)&path[0],30,"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR,&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
                if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
                {
                    uteModuleFilesystemSeek(file,0,FS_SEEK_END);
                    sendParam->currSendFileSize = (uint32_t)uteModuleFilesystemGetFileSize(file);
                    uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
                    memset(&uteModuleSportMoreSportSyncDataBuff[0],0,sendParam->dataBuffSize);
                    uteModuleFilesystemReadData(file,&uteModuleSportMoreSportSyncDataBuff[0],33);
                    uteModuleFilesystemCloseFile(file);
                    sendParam->currSendFileDataOffset = 17;
                    sendSize = 20;
                    response[1] = 0;
                    response[2] = sendParam->currSendMinIndex;
                    response[3] = uteModuleSportMoreSportSyncDataBuff[1];
                    response[4] = uteModuleSportMoreSportSyncDataBuff[31];
                    response[5] = uteModuleSportMoreSportSyncDataBuff[32];
                    response[6] = uteModuleSportMoreSportSyncDataBuff[2];
                    response[7] = uteModuleSportMoreSportSyncDataBuff[3];
                    response[8] = uteModuleSportMoreSportSyncDataBuff[4];
                    response[9] = uteModuleSportMoreSportSyncDataBuff[5];
                    response[10] = uteModuleSportMoreSportSyncDataBuff[6];
                    response[11] = uteModuleSportMoreSportSyncDataBuff[7];
                    response[12] = uteModuleSportMoreSportSyncDataBuff[8];
                    response[13] = uteModuleSportMoreSportSyncDataBuff[9];
                    response[14] = uteModuleSportMoreSportSyncDataBuff[10];
                    response[15] = uteModuleSportMoreSportSyncDataBuff[11];
                    response[16] = uteModuleSportMoreSportSyncDataBuff[12];
                    response[17] = uteModuleSportMoreSportSyncDataBuff[13];
                    response[18] = uteModuleSportMoreSportSyncDataBuff[14];
                    response[19] = uteModuleSportMoreSportSyncDataBuff[15];
                    uteModuleCrc8Bit(&sendParam->crc, &response[1],19);
                    sendParam->currSendMinIndex++;
                }
                else
                {
                    sendParam->currSendFileIndex++;
                }
            }
            else if(sendParam->currSendMinIndex == 1)
            {
                sendParam->currSendFileDataOffset = 33;
                sendSize = 18;
                response[1] = 0;
                response[2] = sendParam->currSendMinIndex;
                response[3] = uteModuleSportMoreSportSyncDataBuff[17];
                response[4] = uteModuleSportMoreSportSyncDataBuff[18];
                response[5] = uteModuleSportMoreSportSyncDataBuff[19];
                response[6] = uteModuleSportMoreSportSyncDataBuff[20];
                response[7] = uteModuleSportMoreSportSyncDataBuff[21];
                response[8] = uteModuleSportMoreSportSyncDataBuff[22];
                response[9] = uteModuleSportMoreSportSyncDataBuff[23];
                response[10] = uteModuleSportMoreSportSyncDataBuff[24];
                response[11] = uteModuleSportMoreSportSyncDataBuff[25];
                response[12] = uteModuleSportMoreSportSyncDataBuff[26];
                response[13] = uteModuleSportMoreSportSyncDataBuff[27];
                response[14] = uteModuleSportMoreSportSyncDataBuff[28];
                response[15] = uteModuleSportMoreSportSyncDataBuff[29];
                response[16] = uteModuleSportMoreSportSyncDataBuff[30];
                response[17] = uteModuleSportMoreSportSyncDataBuff[0];
                uteModuleCrc8Bit(&sendParam->crc, &response[1],17);
                sendParam->currSendMinIndex++;
            }
            else
            {
                uint8_t sendindexMod = ((sendParam->currSendMinIndex-2)%10);
                UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sendindexMod=%d,currSendFileSize=%d,currSendFileDataOffset=%d", __func__,sendindexMod,sendParam->currSendFileSize,sendParam->currSendFileDataOffset);
                if(sendindexMod==0)
                {
                    //read
                    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR,&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
                    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
                    {
                        uint32_t readSize = 0;
                        if((sendParam->currSendFileDataOffset+sendParam->dataBuffSize)>sendParam->currSendFileSize)
                        {
                            readSize = sendParam->currSendFileSize-sendParam->currSendFileDataOffset;
                        }
                        else
                        {
                            readSize = sendParam->dataBuffSize;
                        }
                        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,readSize=%d", __func__,readSize);
                        uteModuleFilesystemSeek(file,sendParam->currSendFileDataOffset,FS_SEEK_SET);
                        memset(&uteModuleSportMoreSportSyncDataBuff[0],0,sendParam->dataBuffSize);
                        uteModuleFilesystemReadData(file,&uteModuleSportMoreSportSyncDataBuff[0],readSize);
                        uteModuleFilesystemCloseFile(file);
                    }
                }
                response[1] = (sendParam->currSendMinIndex>>8)&0xff;
                response[2] = sendParam->currSendMinIndex&0xff;
                if((sendParam->currSendFileDataOffset+17)>sendParam->currSendFileSize)
                {
                    sendSize = sendParam->currSendFileSize - sendParam->currSendFileDataOffset;
                }
                else
                {
                    sendSize = 17;
                }
                sendParam->currSendFileDataOffset += sendSize;
                memcpy(&response[3],&uteModuleSportMoreSportSyncDataBuff[sendindexMod*17],sendSize);
                sendSize = sendSize+3;
                uteModuleCrc8Bit(&sendParam->crc,&response[1],sendSize-1);
                sendParam->currSendMinIndex++;
            }
        }
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sendSize=%d", __func__,sendSize);
    if(sendSize>0)
    {
        uteModuleProfileBleSendToPhone(&response[0],sendSize);
    }
}

/**
*@brief        准备开始发送多运动数据
*@details
*@param[in]  需要同步的数据时间戳
*@author       zn.zeng
*@date       2021-11-13
*/
void uteModuleSportStartSendMoreSportHistoryData(ute_module_systemtime_time_t time)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->time = time;
    if(param->time.year==0)
    {
        param->isAllData = true;
    }
    else
    {
        param->isAllData = false;
    }
    param->currSendFileIndex = 0;
    param->currSendFileDataOffset = 0;
    param->currSendMinIndex =0;
    param->dataBuffSize = 170;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR, &param->dirInfo, NULL);
    uint16_t  hasFiles = 0;
    /*Casen add start 23-02-09 新增判断,如果处于运动中并且心率保存数大于90则当条数据不发给APP*/
    uint8_t sportStatus = uteModuleSportMoreSportGetStatus();
    if(sportStatus != ALL_SPORT_STATUS_CLOSE)
    {
        if(uteModuleSprotData.moreSportData.heartTotalCnt>=SPORTS_HEART_DATAS_CACHE_LENGTH)
        {
            if(param->dirInfo.filesCnt>0)
            {
                param->dirInfo.filesCnt = param->dirInfo.filesCnt -1 ;
            }
        }
    }
    /*Casen add end 23-02-09 */
    if(!param->isAllData)
    {
        uint64_t readTime,fileTime;
        uint8_t timeBuff[18];
        memset(timeBuff,0,18);
        sprintf((char *)&timeBuff[0],"%04d%02d%02d%02d%02d",time.year,time.month,time.day,time.hour,time.min);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,%s", __func__,timeBuff);
        readTime = atoll((const char *)&timeBuff[0]);
        for(uint8_t i = 0; i<param->dirInfo.filesCnt; i++)
        {
            fileTime = atoll((const char *)&param->dirInfo.filesName[i][0]);
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,fileTime=%lld,readTime=%lld", __func__,fileTime,readTime);
            if(fileTime>=readTime)
            {
                hasFiles = param->dirInfo.filesCnt -i;
                param->currSendFileIndex = i;
                break;
            }
        }
        if(hasFiles==0)
        {
            param->currSendFileIndex = param->dirInfo.filesCnt;
        }
    }
    else
    {
        hasFiles = param->dirInfo.filesCnt;
    }
    uint8_t response[10];
    memset(&response[0],0,10);
    response[0] = CMD_SPORT_MODE_AND_SPORT_HEART_RATE;
    response[1] = 0xfa;
    response[2] = time.year>>8&0xff;
    response[3] = time.year&0xff;
    response[4] = time.month;
    response[5] = time.day;
    response[6] = time.hour;
    response[7] = time.min;
    response[8] = hasFiles>>8&0xff;
    response[9] = hasFiles&0xff;

    uteModuleProfileBleSendToPhone(&response[0],10);
    if(hasFiles>0)
    {
        uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSportSendMoreSportHistoryData);
        uteApplicationCommonSyncDataTimerStart();
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
}
/**
*@brief        设置运动开始的倒计时
*@details
*@author       dengli.lu
*@date       2021-11-17
*/
void uteModuleSportSetCountZeroIndex(uint8_t countZeroIndex)
{
    uteModuleSprotData.moreSportData.countZeroIndex = countZeroIndex;
}
/**
*@brief        断开连接处理
*@details
*@author       zn.zeng
*@date       2021-11-30
*/
void uteModuleSportDisconnectHandler(void)
{
    if (
        (uteModuleSprotData.moreSportData.isAppStart) &&
        ((uteModuleSprotData.moreSportData.status == ALL_SPORT_STATUS_OPEN) || (uteModuleSprotData.moreSportData.status == ALL_SPORT_STATUS_UPDATE_DATA))
    )
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s", __func__);
        uteModuleSprotData.moreSportData.isAppStartSportReConnection = false;
        uteModuleSprotData.moreSportData.status = ALL_SPORT_STATUS_PAUSE;
        // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORTS_PAUSED_ID);          //跳到暂停界面，自动暂停运动，蓝牙连接才可继续跑
        //func_switch_to(FUNC_SPORT_SUB_RUN, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
        func_cb.sta = FUNC_SPORT_SUB_RUN;
        sys_cb.sport_app_disconnect = true;
    }
}
/**
*@brief    久坐提醒msg处理函数
*@details
*@author        zn.zeng
*@date        2021-08-06
*/
void uteModuleSportSedentaryMsg(void)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s", __func__);
    // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SEDENTARY_REMIND_ID);           //久坐提醒界面
//    if(!uteModuleHeartIsWear())
//    {
//        return;
//    }
    sys_cb.cover_index = REMIND_COVER_HEALTH_SEDENTARY;
    sys_cb.remind_tag = true;
    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
}
/**
*@brief    从未佩戴到佩戴切换
*@details
*@author        zn.zeng
*@date        2021-12-27
*/
void uteModuleSportUnWearToWearSwitch(void)
{
    if(uteModuleSprotData.lastStepType == STEP_TYPE_SLEEP)
    {
        uteModuleSportStepTypeSetSleep();
    }
    else
    {
        uteModuleSportStepTypeSetStep();
    }
}

#if UTE_MODULE_SPORT_TAKE_PICTURE_MOTOR_SUPPORT
void *uteModuleSportDelaySetTakePictureEnableTimerPointer = NULL;
/**
*@brief        延时使能拍照处理函数
*@details
*@author       xjc
*@date       2022-01-07
*/
void uteModuleSportDelaySetTakePictureEnableHandlerMsg(void)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    if(uteModuleSprotData.isTakingPictures)
    {
        enableTakingPicturesState();
    }
    else
    {
        disableTakingPicturesState();
    }
#endif
}
/**
*@brief   停止使能拍照定时器创建函数
*@details
*@author       xjc
*@date       2022-01-07
*/
void uteModuleSportDelaySetTakePictureEnableTimerStop(void)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
    uteModulePlatformStopTimer(&uteModuleSportDelaySetTakePictureEnableTimerPointer);
    //delete timer
    uteModulePlatformDeleteTimer(&uteModuleSportDelaySetTakePictureEnableTimerPointer);
    uteModuleSportDelaySetTakePictureEnableTimerPointer = NULL;
}
/**
*@brief        延时使能拍照回调函数
*@details
*@author       xjc
*@date       2022-01-07
*/
void uteModuleSportDelaySetTakePictureEnableTimerCallback(void *pxTimer)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_SPORT_DELAY_ENABLE_TAKEPICTURE_TIMER,0);
    uteModuleSportDelaySetTakePictureEnableTimerStop();
}
/**
*@brief        延时使能拍照定时器创建函数
*@details
*@author       xjc
*@date       2022-01-07
*/
void uteModuleSportDelaySetTakePictureEnableTimerCreate(void)
{
    if(uteModuleSportDelaySetTakePictureEnableTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleSportDelaySetTakePictureEnableTimerPointer, "delaySetTakePictureEnableTimer",1, 1000, false, uteModuleSportDelaySetTakePictureEnableTimerCallback);
    }
}
/**
*@brief     启动使能拍照定时器创建函数
*@details
*@author       xjc
*@date       2022-01-07
*/
void uteModuleSportDelaySetTakePictureEnableTimerStart(uint16_t ms)
{
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
    uteModuleSportDelaySetTakePictureEnableTimerCreate();
    uteModulePlatformRestartTimer(&uteModuleSportDelaySetTakePictureEnableTimerPointer, ms);
}
#endif
/**
*@brief       多运动正在运行
*@details
*@author       zn.zeng
*@date       2022-01-11
*/
AT(.com_text.ute_sport)
bool uteModuleSportMoreSportIsRuning(void)
{
    // UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,.moreSportData.status=%d", __func__,uteModuleSprotData.moreSportData.status);
    if(uteModuleSprotData.moreSportData.status!=ALL_SPORT_STATUS_CLOSE)
    {
        return true;
    }
    else
    {
        return false;
    }

}
/**
*@brief       获取多运动的运动类型
*@details
*@author       zn.zeng
*@date       2022-02-14
*/
AT(.com_text.ute_sport)
uint8_t uteModuleSportMoreSportGetType(void)
{
    return uteModuleSprotData.moreSportData.saveData.sportsType;
}

/**
*@brief       获取多运动的运动状态
*@details
*@author       zn.zeng
*@date       2022-02-14
*/
uint8_t uteModuleSportMoreSportGetStatus(void)
{
    return uteModuleSprotData.moreSportData.status;
}
/**
*@brief       设置多运动的运动状态
*@details
*@author       zn.zeng
*@date       2022-02-14
*/
void uteModuleSportMoreSportSetStatus(all_sport_status_type sportStatus)
{
    uteModuleSprotData.moreSportData.status = sportStatus;
}
/**
*@brief       获取多运动的运动是否是手机端开启的
*@details
*@author       zn.zeng
*@date       2022-02-14
*/
bool uteModuleSportMoreSportIsAppStart(void)
{
    return uteModuleSprotData.moreSportData.isAppStart;
}
/**
*@brief       设置多运动的运动是否是手机端开启的回连状态
*@details
*@author       zn.zeng
*@date       2022-02-14
*/
void uteModuleSportMoreSportSetIsAppStartReconnection(bool isAppStartSportReConnection)
{
    uteModuleSprotData.moreSportData.isAppStartSportReConnection = isAppStartSportReConnection;
}
/**
*@brief       获取多运动的运动是否是手机端开启的回连状态
*@details
*@author       zn.zeng
*@date       2022-02-14
*/
bool uteModuleSportMoreSportIsAppStartReconnection(void)
{
    return uteModuleSprotData.moreSportData.isAppStartSportReConnection;
}
/**
*@brief     获取多运动数据
*@details
*@author       zn.zeng
*@date       2022-02-14
*/
void uteModuleSportGetMoreSportsDatas(ute_module_more_sports_data_t *data)
{
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    memcpy(data,&uteModuleSprotData.moreSportData,sizeof(ute_module_more_sports_data_t));
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
/**
*@brief       获取多运动的倒计时计数
*@details
*@author       zn.zeng
*@date       2022-02-14
*/
int8_t uteModuleSportMoreSportGetCountZeroIndex(void)
{
    return uteModuleSprotData.moreSportData.countZeroIndex;
}

/**
*@brief        运动倒计时处理函数
*@details
*@author       xjc
*@date       2022-05-31
*/
void uteModuleSprotCountdownTimerHandlerMsg(void)
{
    if(uteModuleSprotData.moreSportData.countZeroIndex > 0)
    {
        uteModuleSprotData.moreSportData.countZeroIndex--;
        uteModuleSprotCountdownTimerStart(1000);
    }
    else
    {
        uteModuleSprotCountdownTimerStop();
    }
}
/**
*@brief      运动倒计时定时器回调
*@details
*@author     xjc
*@date       2022-05-31
*/
void uteModuleSprotCountdownTimerCallback(void *pxTimer)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_SPORT_COUNTDOWN_TIMER,0);
}
/**
*@brief      开始运动倒计时定时器
*@details
*@author     xjc
*@date       2022-05-31
*/
void uteModuleSprotCountdownTimerStart(uint16_t ms)
{
    if(uteModuleSprotCountdownTimer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleSprotCountdownTimer, "sports countdown timer", 1, 1000, false, uteModuleSprotCountdownTimerCallback);
    }
    uteModulePlatformRestartTimer(&uteModuleSprotCountdownTimer, ms);
}
/**
*@brief      停止运动倒计时定时器
*@details
*@author     xjc
*@date       2022-05-31
*/
void uteModuleSprotCountdownTimerStop(void)
{
    uteModulePlatformStopTimer(&uteModuleSprotCountdownTimer);
    uteModulePlatformDeleteTimer(&uteModuleSprotCountdownTimer);
    uteModuleSprotCountdownTimer = NULL;
}

/**
 *@brief        时间变化
 *@details   跨天数时要处理清零
 *@param[in] ute_module_systemtime_time_t curr,当前时间
 *@param[in] ute_module_systemtime_time_t newSet,将要设置的时间
 *@author       xjc
 *@date       2022-04-28
 */
void uteModuleSportSystemtimeChange(ute_module_systemtime_time_t curr,ute_module_systemtime_time_t newSet)
{
    if (!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,curr %04d-%02d-%02d %02d:%02d:%02d", __func__,curr.year,curr.month,curr.day,curr.hour,curr.min,curr.sec);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,newSet %04d-%02d-%02d %02d:%02d:%02d", __func__,newSet.year,newSet.month,newSet.day,newSet.hour,newSet.min,newSet.sec);
    uint32_t newDay = newSet.year<<16|newSet.month<<8|newSet.day;
    uint32_t currDay = curr.year<<16|curr.month<<8|curr.day;
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    void *file;
    uint16_t buffSize = 12*24+4;
    uint8_t path[40];
    memset(&path[0],0,sizeof(path));
    uint8_t *readBuffOneDay = uteModulePlatformMemoryAlloc(buffSize);
    uteModuleSprotData.lastDayReadHour = -1;
    if(newDay<currDay)
    {
        /*! 时间往回设置,删除大于新设置日期的数据*/
        uint32_t newSetDay = 0;
        uint32_t fileDay = 0;
        ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,before del file", __func__);
        uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR, dirInfo, NULL);
        for (int i = (dirInfo->filesCnt-1); i >= 0; i--)
        {
            sprintf((char *)&path[0],"%04d%02d%02d",newSet.year,newSet.month,newSet.day);
            newSetDay = atoi((const char *)&path[0]);
            fileDay = atoi((const char *)&dirInfo->filesName[i][0]);
            if(fileDay > newSetDay)
            {
                memset(&path[0],0,20);
                sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR,&dirInfo->filesName[i][0]);
                uteModuleFilesystemDelFile((char *)&path[0]);
                UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,del file=%s", __func__,&path[0]);
            }
            else
            {
                break;
            }
        }
#if UTE_MODULE_LOG_SUPPORT
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,after del file", __func__);
        uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR, NULL, NULL);
#endif
        /*! 清除当天步数数据*/
        uteModuleSprotData.stepSleepData.currentHourStepCnt = 0;
        uteModuleSprotData.stepSleepData.current_hour_run_stepCnt = 0;
        uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt = 0;
        mStepSleepParam.currentHourStepCnt = 0;
        mStepSleepParam.current_hour_run_stepCnt = 0;
        mStepSleepParam.current_hour_walk_stepCnt = 0;
        uteModuleSprotData.currDayTotalStep = 0;
        uteModuleSprotData.currDayWalkStep = 0;
        uteModuleSprotData.currDayRunStep = 0;
        /*! 恢复当前步数*/
        memset(&path[0],0,sizeof(path));
        sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR,newSet.year,newSet.month,newSet.day);
        memset(&readBuffOneDay[0],0,buffSize);
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY|FS_O_WRONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&readBuffOneDay[0],buffSize);
            /*! 清除当前时间下一个小时之后的数据*/
            memset(&readBuffOneDay[4+(newSet.hour+1)*12],0,buffSize-(4+(newSet.hour+1)*12));
            uteModuleFilesystemWriteData(file,&readBuffOneDay[0],buffSize);
            uteModuleFilesystemCloseFile(file);
        }
        /*! 恢复当前小时步数*/
        uteModuleSprotData.stepSleepData.currentHourStepCnt = (readBuffOneDay[4+newSet.hour*12]<<8|readBuffOneDay[4+newSet.hour*12+1]);
        uteModuleSprotData.stepSleepData.current_hour_run_stepCnt = (readBuffOneDay[4+newSet.hour*12+5]<<8|readBuffOneDay[4+newSet.hour*12+6]);
        uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt = (readBuffOneDay[4+newSet.hour*12+10]<<8|readBuffOneDay[4+newSet.hour*12+11]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currentHourStepCnt=%d,current_hour_run_stepCnt=%d,current_hour_walk_stepCnt=%d", __func__,uteModuleSprotData.stepSleepData.currentHourStepCnt,uteModuleSprotData.stepSleepData.current_hour_run_stepCnt,uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt);
        mStepSleepParam.currentHourStepCnt = uteModuleSprotData.stepSleepData.currentHourStepCnt;
        mStepSleepParam.current_hour_run_stepCnt = uteModuleSprotData.stepSleepData.current_hour_run_stepCnt;
        mStepSleepParam.current_hour_walk_stepCnt = uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt;
        /*! 恢复当天步数信息*/
        uteModuleSportReadCurrDayStepCnt(newSet);
        uteModulePlatformMemoryFree(dirInfo);
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
        uteModuleSprotData.todayTargetNotify.currStandMin = 0;
        uteModuleSprotData.todayTargetNotify.lastMinStepCnt = uteModuleSprotData.todayTargetNotify.allMinStepCnt = 0;
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
        memset(uteModuleSprotData.dailyActivity.everyHourAllSportStep,0x00,24*4);
        memset(uteModuleSprotData.dailyActivity.everyHourAllSportKcal,0x00,24*2);
#endif
        uteModuleSportSaveTodayTargetNotifyData();
#endif
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
        /*! 重新读取当天有步数运动的步数，距离跟卡路里 xjc, 2022-08-03*/
        // uteModuleSportReadTodayAllSportDataConfig(newSet);
        uteModuleSprotData.updateTodayAllSportData = true;
#endif
    }
    else if(newDay == currDay)
    {
        if(newSet.hour < curr.hour)
        {
            /*! 清除当天步数数据*/
            uteModuleSprotData.stepSleepData.currentHourStepCnt = 0;
            uteModuleSprotData.stepSleepData.current_hour_run_stepCnt = 0;
            uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt = 0;
            mStepSleepParam.currentHourStepCnt = 0;
            mStepSleepParam.current_hour_run_stepCnt = 0;
            mStepSleepParam.current_hour_walk_stepCnt = 0;
            uteModuleSprotData.currDayTotalStep = 0;
            uteModuleSprotData.currDayWalkStep = 0;
            uteModuleSprotData.currDayRunStep = 0;
            memset(&path[0],0,sizeof(path));
            sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR,newSet.year,newSet.month,newSet.day);
            memset(&readBuffOneDay[0],0,buffSize);
            if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY|FS_O_WRONLY))
            {
                uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
                uteModuleFilesystemReadData(file,&readBuffOneDay[0],buffSize);
                /*! 清除当前时间下一个小时之后的数据*/
                memset(&readBuffOneDay[4+(newSet.hour+1)*12],0,buffSize-(4+(newSet.hour+1)*12));
                uteModuleFilesystemWriteData(file,&readBuffOneDay[0],buffSize);
                uteModuleFilesystemCloseFile(file);
            }
            /*! 恢复当前小时步数*/
            uteModuleSprotData.stepSleepData.currentHourStepCnt = (readBuffOneDay[4+newSet.hour*12]<<8|readBuffOneDay[4+newSet.hour*12+1]);
            uteModuleSprotData.stepSleepData.current_hour_run_stepCnt = (readBuffOneDay[4+newSet.hour*12+5]<<8|readBuffOneDay[4+newSet.hour*12+6]);
            uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt = (readBuffOneDay[4+newSet.hour*12+10]<<8|readBuffOneDay[4+newSet.hour*12+11]);
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,currentHourStepCnt=%d,current_hour_run_stepCnt=%d,current_hour_walk_stepCnt=%d", __func__,uteModuleSprotData.stepSleepData.currentHourStepCnt,uteModuleSprotData.stepSleepData.current_hour_run_stepCnt,uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt);
            mStepSleepParam.currentHourStepCnt = uteModuleSprotData.stepSleepData.currentHourStepCnt;
            mStepSleepParam.current_hour_run_stepCnt = uteModuleSprotData.stepSleepData.current_hour_run_stepCnt;
            mStepSleepParam.current_hour_walk_stepCnt = uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt;
            /*! 恢复当天步数信息*/
            uteModuleSportReadCurrDayStepCnt(newSet);
        }
#if (UTE_MODULE_CYWEE_MOTION_SUPPORT == false)
        else if(newSet.hour > curr.hour)
        {
#if UTE_MODULE_SPORT_CLEAR_STEP_WHEN_HOUR_SUPPORT
            if(!((curr.min==59)&&(curr.sec==59)))
            {
                uteModuleSportSaveStepData();
                //clear
                memset(&uteModuleSprotData.stepSleepData,0,sizeof(ute_step_sleep_param_t));
                memset(&mStepSleepParam,0,sizeof(ute_step_sleep_param_t));
                uteModuleSportReadCurrDayStepCnt(newSet);
            }
#else
            uteModuleSportSaveStepData();
            //clear
            memset(&uteModuleSprotData.stepSleepData,0,sizeof(ute_step_sleep_param_t));
            memset(&mStepSleepParam,0,sizeof(ute_step_sleep_param_t));
            uteModuleSportReadCurrDayStepCnt(newSet);
#endif
        }
#endif
    }
    else
    {
        /*! 清除当天步数数据*/
        memset(&path[0],0,sizeof(path));
        sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR,newSet.year,newSet.month,newSet.day);
        uteModuleFilesystemDelFile((char *)&path[0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleSprotData.stepSleepData.currentHourStepCnt = 0;
        uteModuleSprotData.stepSleepData.current_hour_run_stepCnt = 0;
        uteModuleSprotData.stepSleepData.current_hour_walk_stepCnt = 0;
        mStepSleepParam.currentHourStepCnt = 0;
        mStepSleepParam.current_hour_run_stepCnt = 0;
        mStepSleepParam.current_hour_walk_stepCnt = 0;
        uteModuleSprotData.currDayTotalStep = 0;
        uteModuleSprotData.currDayWalkStep = 0;
        uteModuleSprotData.currDayRunStep = 0;
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
        uteModuleSprotData.todayTargetNotify.currStandMin = 0;
        uteModuleSprotData.todayTargetNotify.lastMinStepCnt = uteModuleSprotData.todayTargetNotify.allMinStepCnt = 0;
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
        memset(uteModuleSprotData.dailyActivity.everyHourAllSportStep,0x00,24*4);
        memset(uteModuleSprotData.dailyActivity.everyHourAllSportKcal,0x00,24*2);
#endif
        uteModuleSportSaveTodayTargetNotifyData();
#endif
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
        /*! 重新读取当天有步数运动的步数，距离跟卡路里 xjc, 2022-08-03*/
        // uteModuleSportReadTodayAllSportDataConfig(newSet);
        uteModuleSprotData.updateTodayAllSportData = true;
#endif
    }
    uteModulePlatformMemoryFree(readBuffOneDay);
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}

#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
/**
*@brief     读取目标数据
*@details
*@author        dengli.lu
*@date        2022-03-18
*/
void uteModuleSportReadTodayTargetNotifyData(void)
{
    /*! 读取目标数据*/
    void *file;
    uint8_t readbuff[32];
    memset(&readbuff[0],0,sizeof(readbuff));
    readbuff[0] = (DEFAULT_KCAL_TARGET_CNT>>8)&0xff;
    readbuff[1] = DEFAULT_KCAL_TARGET_CNT&0xff;
    readbuff[2] = DEFAULT_KCAL_TARGET_OPEN;
    readbuff[3] = 0;
    readbuff[4] = DEFAULT_STEP_TARGET_OPEN;
    readbuff[5] = (DEFAULT_DISTANCE_TARGET_CNT>>24)&0xff;
    readbuff[6] = (DEFAULT_DISTANCE_TARGET_CNT>>16)&0xff;
    readbuff[7] = (DEFAULT_DISTANCE_TARGET_CNT>>8)&0xff;
    readbuff[8] = DEFAULT_DISTANCE_TARGET_CNT&0xff;
    readbuff[9] = DEFAULT_DISTANCE_TARGET_OPEN;
    readbuff[10] = 0;
    readbuff[11] = DEFAULT_STAND_TIME_TARGET_OPEN;
    readbuff[12] = 0;
    readbuff[13] = (DEFAULT_STAND_TIME_TARGET_CNT>>8)&0xff;
    readbuff[14] = DEFAULT_STAND_TIME_TARGET_CNT&0xff;
    readbuff[27] = DEFAULT_SPORE_TIME_TARGET_OPEN;
    readbuff[28] = 0;
    readbuff[29] = (DEFAULT_SPORT_TIME_TARGET_CNT>>8)&0xff;
    readbuff[30] = DEFAULT_SPORT_TIME_TARGET_CNT&0xff;
    readbuff[31] = 0;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_TARGET_NOTIFY_DATA,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],32);
        uteModuleFilesystemCloseFile(file);
    }
    //kcal step distance
    uteModuleSprotData.todayTargetNotify.todayKcalTarget = readbuff[0]<<8|readbuff[1];
    uteModuleSprotData.todayTargetNotify.isTodayKcalTargetNotifyOpen = readbuff[2];
    uteModuleSprotData.todayTargetNotify.isTodayKcalTargetHasNotify = readbuff[3];
    uteModuleSprotData.todayTargetNotify.isTodayStepTargetNotifyOpen = readbuff[4];
    uteModuleSprotData.todayTargetNotify.todayDistanceTarget = readbuff[5]<<24|readbuff[6]<<16|readbuff[7]<<8|readbuff[8];
    uteModuleSprotData.todayTargetNotify.isTodayDistanceTargetNotifyOpen = readbuff[9];
    uteModuleSprotData.todayTargetNotify.isTodayDistanceTargetHasNotify = readbuff[10];
    //stand time
    uteModuleSprotData.todayTargetNotify.isStandTimeTargetNotifyOpen = readbuff[11];
    uteModuleSprotData.todayTargetNotify.isStandTimeTargetHasNotify = readbuff[12];
    uteModuleSprotData.todayTargetNotify.standTimeTargetSetMin = readbuff[13]<<8|readbuff[14];
    uteModuleSprotData.todayTargetNotify.currStandMin = readbuff[15]<<8|readbuff[16];
    uteModuleSprotData.todayTargetNotify.allMinStepCnt = readbuff[17]<<24|readbuff[18]<<16|readbuff[19]<<8|readbuff[20];
    uteModuleSprotData.todayTargetNotify.lastMinStepCnt = readbuff[21]<<24|readbuff[22]<<16|readbuff[23]<<8|readbuff[24];
    uteModuleSprotData.todayTargetNotify.standStartHour = readbuff[25];
    uteModuleSprotData.todayTargetNotify.standStartMin = readbuff[26];
    //sport time
    uteModuleSprotData.todayTargetNotify.isSportTimeTargetNotifyOpen = readbuff[27];
    uteModuleSprotData.todayTargetNotify.isSportTimeTargetHasNotify = readbuff[28];
    uteModuleSprotData.todayTargetNotify.sportTimeTargetSetMin = readbuff[29]<<8|readbuff[30];
    //all target
    uteModuleSprotData.todayTargetNotify.isTodayAllTargetHasNotify = readbuff[31];
}

/**
*@brief     保存目标数据
*@details
*@author        dengli.lu
*@date        2022-03-18
*/
void uteModuleSportSaveTodayTargetNotifyData(void)
{
    /*! 保存目标数据*/
    void *file;
    uint8_t writeBuff[32];
    memset(&writeBuff[0],0,sizeof(writeBuff));
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_TARGET_NOTIFY_DATA, &file, FS_O_RDONLY|FS_O_WRONLY|FS_O_CREAT))
    {
        //kcal step distance
        writeBuff[0] = (uteModuleSprotData.todayTargetNotify.todayKcalTarget>>8)&0xff;
        writeBuff[1] = (uteModuleSprotData.todayTargetNotify.todayKcalTarget)&0xff;
        writeBuff[2] = uteModuleSprotData.todayTargetNotify.isTodayKcalTargetNotifyOpen;
        writeBuff[3] = uteModuleSprotData.todayTargetNotify.isTodayKcalTargetHasNotify;
        writeBuff[4] = uteModuleSprotData.todayTargetNotify.isTodayStepTargetNotifyOpen;
        writeBuff[5] = (uteModuleSprotData.todayTargetNotify.todayDistanceTarget>>24)&0xff;
        writeBuff[6] = (uteModuleSprotData.todayTargetNotify.todayDistanceTarget>>16)&0xff;
        writeBuff[7] = (uteModuleSprotData.todayTargetNotify.todayDistanceTarget>>8)&0xff;
        writeBuff[8] = (uteModuleSprotData.todayTargetNotify.todayDistanceTarget)&0xff;
        writeBuff[9] = uteModuleSprotData.todayTargetNotify.isTodayDistanceTargetNotifyOpen;
        writeBuff[10] = uteModuleSprotData.todayTargetNotify.isTodayDistanceTargetHasNotify;
        //stand time
        writeBuff[11] = uteModuleSprotData.todayTargetNotify.isStandTimeTargetNotifyOpen;
        writeBuff[12] = uteModuleSprotData.todayTargetNotify.isStandTimeTargetHasNotify;
        writeBuff[13] = (uteModuleSprotData.todayTargetNotify.standTimeTargetSetMin>>8)&0xff;
        writeBuff[14] = (uteModuleSprotData.todayTargetNotify.standTimeTargetSetMin)&0xff;
        writeBuff[15] = (uteModuleSprotData.todayTargetNotify.currStandMin>>8)&0xff;
        writeBuff[16] = (uteModuleSprotData.todayTargetNotify.currStandMin)&0xff;
        writeBuff[17] = (uteModuleSprotData.todayTargetNotify.allMinStepCnt>>24)&0xff;
        writeBuff[18] = (uteModuleSprotData.todayTargetNotify.allMinStepCnt>>16)&0xff;
        writeBuff[19] = (uteModuleSprotData.todayTargetNotify.allMinStepCnt>>8)&0xff;
        writeBuff[20] = (uteModuleSprotData.todayTargetNotify.allMinStepCnt)&0xff;
        writeBuff[21] = (uteModuleSprotData.todayTargetNotify.lastMinStepCnt>>24)&0xff;
        writeBuff[22] = (uteModuleSprotData.todayTargetNotify.lastMinStepCnt>>16)&0xff;
        writeBuff[23] = (uteModuleSprotData.todayTargetNotify.lastMinStepCnt>>8)&0xff;
        writeBuff[24] = (uteModuleSprotData.todayTargetNotify.lastMinStepCnt)&0xff;
        writeBuff[25] = uteModuleSprotData.todayTargetNotify.standStartHour;
        writeBuff[26] = uteModuleSprotData.todayTargetNotify.standStartMin;
        //sport time
        writeBuff[27] = uteModuleSprotData.todayTargetNotify.isSportTimeTargetNotifyOpen;
        writeBuff[28] = uteModuleSprotData.todayTargetNotify.isSportTimeTargetHasNotify;
        writeBuff[29] = (uteModuleSprotData.todayTargetNotify.sportTimeTargetSetMin>>8)&0xff;
        writeBuff[30] = (uteModuleSprotData.todayTargetNotify.sportTimeTargetSetMin)&0xff;
        //all target
        writeBuff[31] = uteModuleSprotData.todayTargetNotify.isTodayAllTargetHasNotify;
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writeBuff[0],32);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief  设置今日活动目标
*@details
*@param[in] ute_module_target_notify_data_t setTodayTarget  传入要设置今日活动参数的指针
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportSetTodayTargetNotifyData(ute_module_target_notify_data_t setTodayTarget)
{
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    if(setTodayTarget.todayDistanceTarget > 9999)
    {
        setTodayTarget.todayDistanceTarget = 9999;
    }
    if(setTodayTarget.todayKcalTarget > 9999)
    {
        setTodayTarget.todayKcalTarget = 9999;
    }
    memcpy(&uteModuleSprotData.todayTargetNotify, &setTodayTarget, sizeof(ute_module_target_notify_data_t));
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
/**
*@brief  获取今日活动目标
*@details
*@param[in] ute_module_target_notify_data_t *set  传入获取今日活动参数的指针
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportGetTodayTargetNotifyData(ute_module_target_notify_data_t *todayTarget)
{
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    memcpy(todayTarget,&uteModuleSprotData.todayTargetNotify, sizeof(ute_module_target_notify_data_t));
    if(uteModuleSystemtimeGetDistanceMiType())
    {
        todayTarget->todayDistanceTarget = todayTarget->todayDistanceTarget * 0.6213712f;
    }
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
/**
*@brief  获取今日活动目标
*@details
*@param[in] ute_module_target_notify_data_t *set  传入获取今日活动参数的指针
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportProtocolGetTodayTargetNotifyData(ute_module_target_notify_data_t *todayTarget)
{
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    memcpy(todayTarget,&uteModuleSprotData.todayTargetNotify, sizeof(ute_module_target_notify_data_t));
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
/**
*@brief     目标数据每秒处理函数
*@details
*@author        dengli.lu
*@date        2022-03-18
*/
void uteModuleSportTodayTargetHandler(void)
{
    if(!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint16_t tempDistance = uteModuleSportGetCurrDayDistanceData();
    uint16_t curryDayKcal = uteModuleSportGetCurrDayKcalData();
    UTE_MODULE_LOG(1,"curryDayKcal=%d,tempDistance=%d,isStandTimeOpen =%d,currStandMin=%d.moreSportData.status=%d",curryDayKcal,tempDistance,
                   (uteModuleSprotData.todayTargetNotify.isStandTimeTargetNotifyOpen),uteModuleSprotData.todayTargetNotify.currStandMin,uteModuleSprotData.moreSportData.status);
    if(uteModuleSystemtimeGetDistanceMiType())
    {
        tempDistance = (float)tempDistance / 0.6213712f;//先转换为KM，在转换为米
    }
    uint32_t curryDayDistance = tempDistance;
    //kcal notify
    if((curryDayKcal >= uteModuleSprotData.todayTargetNotify.todayKcalTarget)&&\
       (!uteModuleSprotData.todayTargetNotify.isTodayKcalTargetHasNotify)&&\
       (uteModuleSprotData.todayTargetNotify.isTodayKcalTargetNotifyOpen))
    {
        memset(&uteModuleSprotData.todayTargetNotify.todayTargetNotifyType,0,sizeof(ute_module_today_target_notify_type_t));
        uteModuleSprotData.todayTargetNotify.todayTargetNotifyType.isTodayKcalTargetNotify = true;
        uteModuleSprotData.todayTargetNotify.isTodayKcalTargetHasNotify = true;
#if UTE_MODULE_KCAL_TARGET_NOTIFY_SUPPORT
#if QUICK_SWITCH_GOAL_REACH_SUPPORT
        ute_quick_switch_t quick;
        uteApplicationCommonGetQuickSwitchStatus(&quick);
        if(quick.isGoalReach)
#endif
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TARGET_COMPLETED_ID);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
        }
#endif
    }
    //distance notify
    if((curryDayDistance >= uteModuleSprotData.todayTargetNotify.todayDistanceTarget)&&\
       (!uteModuleSprotData.todayTargetNotify.isTodayDistanceTargetHasNotify)&&\
       (uteModuleSprotData.todayTargetNotify.isTodayDistanceTargetNotifyOpen))
    {
        memset(&uteModuleSprotData.todayTargetNotify.todayTargetNotifyType,0,sizeof(ute_module_today_target_notify_type_t));
        uteModuleSprotData.todayTargetNotify.todayTargetNotifyType.isTodayDistanceTargetNotify = true;
        uteModuleSprotData.todayTargetNotify.isTodayDistanceTargetHasNotify = true;
#if UTE_MODULE_DISTANCE_TARGET_NOTIFY_SUPPORT
#if QUICK_SWITCH_GOAL_REACH_SUPPORT
        ute_quick_switch_t quick;
        uteApplicationCommonGetQuickSwitchStatus(&quick);
        if(quick.isGoalReach)
#endif
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TARGET_COMPLETED_ID);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
        }
#endif
    }
    //stand time
    if(time.sec==59)
    {
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT&(!UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT)
        uint32_t currentDayTotleStepsCnt =0;
        uteModuleSportGetCurrDayStepCnt(&currentDayTotleStepsCnt,NULL,NULL);
        if(uteModuleSprotData.moreSportData.status == ALL_SPORT_STATUS_CLOSE)
        {
            uteModuleSprotData.dailyActivity.everyHourAllSportStep[time.hour] = currentDayTotleStepsCnt;
            uteModuleSprotData.dailyActivity.everyHourAllSportKcal[time.hour] = curryDayKcal;
        }
        else
        {
            if(!uteModuleSportMoreSportsIsLessData())
            {
                uteModuleSprotData.dailyActivity.everyHourAllSportStep[time.hour] = currentDayTotleStepsCnt + uteModuleSprotData.moreSportData.saveData.sportStep;
                uteModuleSprotData.dailyActivity.everyHourAllSportKcal[time.hour] = curryDayKcal + uteModuleSprotData.moreSportData.saveData.sportCaloire;
            }
            else
            {
                uteModuleSprotData.dailyActivity.everyHourAllSportStep[time.hour] = currentDayTotleStepsCnt;
                uteModuleSprotData.dailyActivity.everyHourAllSportKcal[time.hour] = curryDayKcal;
            }
        }
#endif
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
        uint32_t totalStepCnt = 0;
        uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
        uteModuleSprotData.todayTargetNotify.allMinStepCnt = totalStepCnt;
        if((uteModuleSprotData.moreSportData.status) && (!uteModuleSportMoreSportsIsLessData()))
        {
            uteModuleSprotData.todayTargetNotify.allMinStepCnt += uteModuleSprotData.moreSportData.saveData.sportStep;
        }
#endif
        if(uteModuleSprotData.todayTargetNotify.isStandTimeTargetNotifyOpen)
        {
            uteModuleSprotData.todayTargetNotify.currStandMin++;
            if(uteModuleSprotData.todayTargetNotify.allMinStepCnt < (uteModuleSprotData.todayTargetNotify.lastMinStepCnt + 15))
            {
                if(uteModuleSprotData.todayTargetNotify.currStandMin==60)
                {
                    if(uteModuleSprotData.todayTargetNotify.allMinStepCnt > (uteModuleSprotData.todayTargetNotify.lastMinStepCnt + 10))
                    {
                        uteModuleSprotData.dailyActivity.sedentarinessFlag = false;
                    }
                }
            }
            else
            {
                uteModuleSprotData.dailyActivity.sedentarinessFlag = false;
            }
            if(time.min == 59)  //整小时xx:59:59
            {
                if(uteModuleSprotData.dailyActivity.sedentarinessFlag == false && uteModuleSprotData.todayTargetNotify.currStandMin == 60)
                {
                    uteModuleSprotData.todayTargetNotify.standStartHour = time.hour;
                    uteModuleSprotData.todayTargetNotify.standStartMin = time.min;
                    uteModuleSprotData.todayTargetNotify.currStandMin = 60;
                    uteModuleSportSaveStandTimeData(uteModuleSprotData.todayTargetNotify.standStartHour,uteModuleSprotData.todayTargetNotify.standStartMin,uteModuleSprotData.todayTargetNotify.currStandMin);
                    //保存一次数据
                }
                uteModuleSprotData.todayTargetNotify.currStandMin = 0;
                uteModuleSprotData.todayTargetNotify.standStartHour = time.hour;
                uteModuleSprotData.todayTargetNotify.standStartMin = time.min;
                uteModuleSprotData.dailyActivity.sedentarinessFlag = true;
            }
            if(!uteModuleSprotData.todayTargetNotify.isStandTimeTargetHasNotify)
            {
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT  //add pcm 2022+09-23 添加宏控制，防止其他项目编译不过
                if(uteModuleSportGetCurrDayEveryHourStandTime()>=uteModuleSprotData.todayTargetNotify.standTimeTargetSetMin)
#endif
                {
                    //提醒
#if !UTE_MODULE_RH281LX_DEBUG_SUPPORT
                    memset(&uteModuleSprotData.todayTargetNotify.todayTargetNotifyType,0,sizeof(ute_module_today_target_notify_type_t));
#endif
                    uteModuleSprotData.todayTargetNotify.todayTargetNotifyType.isTodayStandTimeTargetNotify = true;
                    uteModuleSprotData.todayTargetNotify.isStandTimeTargetHasNotify = true;
#if UTE_MODULE_STAND_TIME_TARGET_NOTIFY_SUPPORT
#if QUICK_SWITCH_GOAL_REACH_SUPPORT
                    ute_quick_switch_t quick;
                    uteApplicationCommonGetQuickSwitchStatus(&quick);
                    if(quick.isGoalReach)
#endif
                    {
                        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TARGET_COMPLETED_ID);
                        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
                    }
#endif
                }
            }
        }
        uteModuleSprotData.todayTargetNotify.lastMinStepCnt = uteModuleSprotData.todayTargetNotify.allMinStepCnt;
    }
    // sport time
    if((uteModuleSprotData.todayTargetNotify.isSportTimeTargetNotifyOpen)&&(!uteModuleSprotData.todayTargetNotify.isSportTimeTargetHasNotify))
    {
        if((uteModuleSprotData.todayTargetNotify.currentDayAllSportTimeSec/60)>=uteModuleSprotData.todayTargetNotify.sportTimeTargetSetMin)
        {
#if !UTE_MODULE_RH281LX_DEBUG_SUPPORT
            memset(&uteModuleSprotData.todayTargetNotify.todayTargetNotifyType,0,sizeof(ute_module_today_target_notify_type_t));
#endif
            uteModuleSprotData.todayTargetNotify.todayTargetNotifyType.isTodaySportTimeTargetNotify = true;
            uteModuleSprotData.todayTargetNotify.isSportTimeTargetHasNotify = true;
#if UTE_MODULE_SPORT_TIME_TARGET_NOTIFY_SUPPORT
#if QUICK_SWITCH_GOAL_REACH_SUPPORT
            ute_quick_switch_t quick;
            uteApplicationCommonGetQuickSwitchStatus(&quick);
            if(quick.isGoalReach)
#endif
            {
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TARGET_COMPLETED_ID);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
            }
#endif
        }
    }
    //all target
    if((!uteModuleSprotData.todayTargetNotify.isTodayAllTargetHasNotify)&&\
       uteModuleSprotData.todayTargetNotify.isTodayKcalTargetHasNotify &&\
       uteModuleSprotData.todayTargetNotify.isTodayDistanceTargetHasNotify &&\
       uteModuleSprotData.todayTargetNotify.isStandTimeTargetHasNotify &&\
       uteModuleSprotData.todayTargetNotify.isSportTimeTargetHasNotify &&\
       uteModuleSprotData.isTargetCompleted)
    {
#if !UTE_MODULE_RH281LX_DEBUG_SUPPORT
        memset(&uteModuleSprotData.todayTargetNotify.todayTargetNotifyType,0,sizeof(ute_module_today_target_notify_type_t));
#endif
        uteModuleSprotData.todayTargetNotify.todayTargetNotifyType.isTodayAllTargetNotify = true;
        uteModuleSprotData.todayTargetNotify.isTodayAllTargetHasNotify = true;
#if UTE_MODULE_ALL_TARGET_NOTIFY_SUPPORT
#if QUICK_SWITCH_GOAL_REACH_SUPPORT
        ute_quick_switch_t quick;
        uteApplicationCommonGetQuickSwitchStatus(&quick);
        if(quick.isGoalReach)
#endif
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TARGET_COMPLETED_ID);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
        }
#endif
    }
    if((time.hour==0)&&(time.min==0)&&(time.sec==0))
    {
        uteModuleSprotData.todayTargetNotify.isTodayKcalTargetHasNotify = false;
        uteModuleSprotData.todayTargetNotify.isTodayDistanceTargetHasNotify = false;
        uteModuleSprotData.todayTargetNotify.isTodayAllTargetHasNotify = false;
        uteModuleSprotData.todayTargetNotify.isStandTimeTargetHasNotify = false;
        uteModuleSprotData.todayTargetNotify.isSportTimeTargetHasNotify = false;
        uteModuleSprotData.todayTargetNotify.currStandMin = 0;
        uteModuleSprotData.todayTargetNotify.standStartHour = time.hour;
        uteModuleSprotData.todayTargetNotify.standStartMin = time.min;
        uteModuleSprotData.dailyActivity.sedentarinessFlag = true;
        memset(uteModuleSprotData.dailyActivity.everyHourAllSportStep,0x00,24*4);
        memset(uteModuleSprotData.dailyActivity.everyHourAllSportKcal,0x00,24*2);
        uteModuleSprotData.todayTargetNotify.lastMinStepCnt = uteModuleSprotData.todayTargetNotify.allMinStepCnt = 0;
    }
}

/**
*@brief      保存当前站立时长数据
*@details    每天保存一个文件，前面4个byte为年月日，后面每4个byte为一个小时的数据（参考协议的数据格式）
*@author     casen
*@date       2022-07-14
*/
void uteModuleSportSaveStandTimeData(uint8_t standStartHour,uint8_t standStartMin,uint8_t currStandMin)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = 4;
    uint8_t *writeBuff = uteModulePlatformMemoryAlloc(buffSize);
    uint8_t path[20];
    memset(&writeBuff[0],0,buffSize);
    memset(&path[0],0,20);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0],"%04d%02d%02d",time.year,time.month,time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA, dirInfo, NULL);
    if((dirInfo->filesCnt>=UTE_MODULE_SPORT_STANDTIME_SAVE_DATA_MAX_DAYS)&&(memcmp(&path[0],&dirInfo->filesName[0][0],8)!=0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-08-23  */
        memset(&path[0],0,20);
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA,&dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0],0,20);
    sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA,time.year,time.month,time.day);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,save file=%s", __func__,&path[0]);
    /*! 保存当前数据zn.zeng, 2021-08-23  */
    void *file;

    writeBuff[0] = standStartHour;
    writeBuff[1] = standStartMin;
    writeBuff[2] = currStandMin>>8&0xff;
    writeBuff[3] = currStandMin&0xff;

    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT))
    {
        int32_t size = uteModuleFilesystemGetFileSize(file);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,size=%d", __func__, size);
        if((size+buffSize)<=(4*24)) //防止跳转日期反复存储
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_END);  //在文件末尾添加数据
            uteModuleFilesystemWriteData(file,&writeBuff[0],buffSize);
            uteModuleFilesystemCloseFile(file);
        }
    }
    uteModulePlatformMemoryFree(writeBuff);
    uteModulePlatformMemoryFree(dirInfo);
}
/**
*@brief      开始发送站立时长数据
*@details
*@author     casen
*@date       2022-07-14
*/
void uteModuleSportSendStandTimeHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t sendBuff[247];
    memset(&sendBuff[0], 0, 247);
    sendBuff[0] = CMD_TODAY_TARGET_CTRL;
    sendBuff[1] = 0x01;
    sendBuff[2] = 0x02;
    uint16_t mtu = uteApplicationCommonGetMtuSize();
    void *file;
    uint8_t path[20];
    memset(&path[0],0,20);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sendParam->currSendFileIndex=%d , sendParam->dirInfo.filesCnt = %d", __func__,sendParam->currSendFileIndex,sendParam->dirInfo.filesCnt);
    if(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt)
    {
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA,&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,path file=%s", __func__,&path[0]);
        if( uteModuleFilesystemOpenFile(path,&file,FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_END);
            sendParam->currSendFileSize = uteModuleFilesystemGetFileSize(file);
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,sendParam->currSendFileSize =%d", __func__,sendParam->currSendFileSize );
            uint32_t numDate; //yyyymmnn
            numDate = atoi((const char *)&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
            //year
            sendBuff[3] = (numDate/10000)>>8&0xff;
            sendBuff[4] = (numDate/10000)&0xff;
            //month
            sendBuff[5] = ((numDate%10000)/100)&0xff;
            //day
            sendBuff[6] = (numDate%100)&0xff;
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&sendBuff[7],sendParam->currSendFileSize);
            uteModuleFilesystemCloseFile(file);
            uteModuleProfileBle50SendToPhone(&sendBuff[0], 7+sendParam->currSendFileSize);
            for(uint8_t j=0; j<(7+sendParam->currSendFileSize); j++)
            {
                sendParam->crc ^=sendBuff[j];
            }
        }
        sendParam->currSendFileIndex ++;
    }
    else
    {
        uteApplicationCommonSyncDataTimerStop();
        sendBuff[3] = 0xfa;
        sendBuff[4] = sendParam->crc;
        uteModuleProfileBleSendToPhone(&sendBuff[0], 5);
    }
}
/**
*@brief        准备开始发送计步数据
*@details
*@param[in]  需要同步的数据时间戳
*@author     casen
*@date       2022-07-14
*/
void uteModuleSportStartSendStandTimeHistoryData(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->currSendFileIndex = 0;
    param->currSendFileSize = 0;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSportSendStandTimeHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
}

#endif

#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
/**
*@brief  读取运动历史记录
*@details
*@author      dengli.lu
*@date       2022-06-25
*/
void uteModuleSportReadHistoryRecordData(void)
{
    void *file=NULL;
    uint8_t readbuff[33];
    memset(&readbuff[0],0,sizeof(readbuff));
    uint8_t path[30];
    memset(&path[0],0,sizeof(path));
    memset(&uteModuleSprotData.sportsHistoryRecord,0,sizeof(uteModuleSprotData.sportsHistoryRecord));
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR, dirInfo, NULL);
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    uint8_t vailCnt = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,dirInfo->filesCnt = %d,", __func__,dirInfo->filesCnt);
    for(uint8_t i = dirInfo->filesCnt; i > 0; i--)
    {
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR,&dirInfo->filesName[i-1][0]);
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&readbuff[0],33);
            uteModuleFilesystemCloseFile(file);
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].startInterval = readbuff[0];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].sportsType = readbuff[1];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].startSportTime.year = (readbuff[2]<<8|readbuff[3]);
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].startSportTime.month = readbuff[4];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].startSportTime.day = readbuff[5];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].startSportTime.hour = readbuff[6];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].startSportTime.min = readbuff[7];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].startSportTime.sec = readbuff[8];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].stopSportTime.hour = (readbuff[13]<<16|readbuff[14]<<8|readbuff[15])/3600;//用于表示运动时长的小时
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].stopSportTime.min = (readbuff[13]<<16|readbuff[14]<<8|readbuff[15])%3600/60;//用于表示运动时长的分钟
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].stopSportTime.sec = (readbuff[13]<<16|readbuff[14]<<8|readbuff[15])%60;//用于表示运动时长的秒
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].sportStep = (readbuff[16]<<24|readbuff[17]<<16|readbuff[18]<<8|readbuff[19]);
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].sportTimes = (readbuff[20]<<8|readbuff[21]);
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].sportCaloire = (readbuff[22]<<8|readbuff[23]);
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].sportDistanceInteger = readbuff[24];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].sportDistanceDecimals = readbuff[25];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].avgHeartRate = readbuff[26];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].maxHeartRate = readbuff[27];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].minHeartRate = readbuff[28];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].avgTimeMinute = readbuff[29];
            uteModuleSprotData.sportsHistoryRecord.oneHistoryRecordData[vailCnt].avgTimeSecond = readbuff[30];
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,readbuff[31] =%d , readbuff[32] = %d", __func__,readbuff[31],readbuff[32]);
            vailCnt ++;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,vailCnt = %d,", __func__,vailCnt);
    uteModuleSprotData.sportsHistoryRecord.sportsHistoryRecordTotalCnt = vailCnt;
    uteModulePlatformGiveMutex(uteModuleSprotMute);
    uteModulePlatformMemoryFree(dirInfo);
}
/**
*@brief     获取运动历史数据
*@details
*@author      dengli.lu
*@date       2022-06-25
*/
void uteModuleSportsHistoryRecordGetData(ute_module_sports_record_data_t *data)
{
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    memcpy(data,&uteModuleSprotData.sportsHistoryRecord,sizeof(ute_module_sports_record_data_t));
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}

/**
*@brief     设置运动历史数据标志（在历史记录界面进入设置）
*@details
*@author      pcm
*@date       2022-010-15
*/
void uteModuleSportsHistoryRecordSetIsHistoryRecordData(bool IsHistoryRecordData)
{

    uteModuleSprotData.sportsHistoryRecord.IsHistoryRecordData=IsHistoryRecordData;

}
/**
*@brief     获取运动历史数据标志（在历史记录界面进入设置）
*@details
*@author      pcm
*@date       2022-010-15
*/
bool uteModuleSportsHistoryRecordGetIsHistoryRecordData(void)
{

    return uteModuleSprotData.sportsHistoryRecord.IsHistoryRecordData;

}

/**
*@brief        获取存在记录的总条数
*@details
*@return uint8_t
*@author      dengli.lu
*@date       2022-06-25
*/
uint8_t uteModuleSportsHistoryGetRecordTotalCnt(void)
{
    return uteModuleSprotData.sportsHistoryRecord.sportsHistoryRecordTotalCnt;
}
/**
*@brief     设置运动历史记录显示索引
*@details
*@author      dengli.lu
*@date       2022-06-25
*/
void uteModuleSportsSetHistoryRecordDisplayIndex(uint8_t displayIndex)
{
    uteModuleSprotData.sportsHistoryRecord.displayIndex = displayIndex;
}
#endif

#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
/**
*@brief  读取运动当天每小时的运动时长
*@details  totalSportMinSec:当天总的运动时长
*@author   casen
*@date     2022-07-13
*/
uint32_t uteModuleSportGetCurrDayEveryHourSportTime(void)
{
    void *file=NULL;
    uint32_t totalSportMinSec = 0;
    uint8_t readbuff[33];
    memset(&readbuff[0],0,sizeof(readbuff));
    uint8_t path[30];
    memset(&path[0],0,sizeof(path));
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR, dirInfo, NULL);
    //清空数据
    memset(uteModuleSprotData.dailyActivity.everyHourSportTime,0x00,sizeof(uteModuleSprotData.dailyActivity.everyHourSportTime[0])*24);

    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint8_t currDayPath[8];
    memset(&currDayPath[0],0,8);
    sprintf((char *)&currDayPath[0],"%04d%02d%02d",time.year,time.month,time.day);

    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,dirInfo->filesCnt = %d,", __func__,dirInfo->filesCnt);
    for(uint8_t i = dirInfo->filesCnt; i > 0; i--)
    {
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR,&dirInfo->filesName[i-1][0]);
        if(memcmp(&currDayPath[0],&path[11],8) == 0)  //currDay
        {
            if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
            {
                uint32_t sportTime = 0;
                uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
                uteModuleFilesystemReadData(file,&readbuff[0],33);
                uteModuleFilesystemCloseFile(file);
                sportTime = (uint32_t)readbuff[13]<<16|readbuff[14]<<8|readbuff[15];
                (totalSportMinSec) += sportTime;
                uint8_t startSportHour = readbuff[6],startSportMin = readbuff[7];
                if(startSportHour<24)
                {
                    uint16_t sportTimeMin = (sportTime/60);
                    if((startSportMin + sportTimeMin)>60)
                    {
                        uint16_t tempMin = 60 - startSportMin ;
                        uteModuleSprotData.dailyActivity.everyHourSportTime[startSportHour] += tempMin;
                        sportTimeMin = sportTimeMin - tempMin;
                        uint8_t i = 1;
                        while(sportTimeMin  > 60)
                        {
                            //跨天的数据不处理
                            if((startSportHour+i)<24)
                            {
                                uteModuleSprotData.dailyActivity.everyHourSportTime[startSportHour+i] += 60;
                            }
                            sportTimeMin -= 60;
                            i++;
                        }
                        //跨天的数据不处理
                        if((startSportHour+i)<24)
                        {
                            uteModuleSprotData.dailyActivity.everyHourSportTime[startSportHour+i] += sportTimeMin;
                        }
                    }
                    else
                    {
                        uteModuleSprotData.dailyActivity.everyHourSportTime[startSportHour] += sportTimeMin;
                    }
                }
            }
        }
    }
    uteModulePlatformMemoryFree(dirInfo);
    return totalSportMinSec;
}

/**
*@brief      获取当天每个小时的卡路里
*@details
*@author     casen
*@date       2022-07-13
*/
uint16_t uteModuleSportGetCurrDayEveryHourKcal(void)
{
    uint16_t kcal=0;
    float distance_Step,distance_Run;
    float kcli_Step,kcli_Run,kcli;
    ute_personal_info_t personal;
    uteApplicationCommonGetPersonalInfo(&personal);
    memset(uteModuleSprotData.dailyActivity.everyHourKcal,0x00,sizeof(uteModuleSprotData.dailyActivity.everyHourKcal[0])*24);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCwmGetCurrEveryHourKcalData(&uteModuleSprotData.dailyActivity.everyHourKcal[0]);
#else
    for(uint8_t i=0; i<24; i++)
    {
        if(personal.isMan)
        {
            distance_Step = (float)((float)((personal.height) * (uteModuleSprotData.dailyActivity.everyHourWalkStep[i]))*0.00001f*0.4102670f);
            distance_Run = (float)((float)((personal.height) * (uteModuleSprotData.dailyActivity.everyHourRunStep[i]))*0.00001f*0.5461055f);

            kcli_Step = (float)(distance_Step*personal.weight/1000.0f*0.5907296f);
            kcli_Run =  (float)(distance_Run*personal.weight/1000.0f*0.9288487f);
        }
        else
        {
            distance_Step = (float)((float)((personal.height) * (uteModuleSprotData.dailyActivity.everyHourWalkStep[i]))*0.00001f*0.4151582f);
            distance_Run = (float)((float)((personal.height) * (uteModuleSprotData.dailyActivity.everyHourRunStep[i]))*0.00001f*0.5047813f);

            kcli_Step = (float)(distance_Step*personal.weight/1000.0f*0.7918928f);
            kcli_Run = (float)(distance_Run*personal.weight/1000.0f*1.234930f);
        }
        kcli_Step = ExactDecimalPoint(kcli_Step,1);
        kcli_Run = ExactDecimalPoint(kcli_Run,1);
        kcli = ExactDecimalPoint((kcli_Step+kcli_Run),2);
        kcal = (uint16_t)(kcli*1.0f);
        uteModuleSprotData.dailyActivity.everyHourKcal[i] = kcal;
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,everyHourKcal[%d] = %d,", __func__,uteModuleSprotData.dailyActivity.everyHourKcal[i],i);
    }
#endif
    if(kcal>9999)
    {
        kcal = 9999;
    }
    return kcal;
}
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
/**
*@brief      获取当天每个小时的站立时长数据
*@details   1）读取当天每个小时的数据，并缓存到全局变量值中；
            2）返回值是当天的站立时长，每次是60分钟，如果要显示次数，需要除以60即可得到次数
*@return    当天站立时长，单位分钟
*@author     casen
*@date       2022-07-13
*/
uint16_t uteModuleSportGetCurrDayEveryHourStandTime(void)
{
    uint16_t totalStandTimeMin= 0;
    void *file=NULL;
    uint8_t readbuff[4*24];
    memset(&readbuff[0],0,sizeof(readbuff));
    uint8_t path[30];
    memset(&path[0],0,sizeof(path));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint8_t currDayPath[8];
    memset(&currDayPath[0],0,8);
    sprintf((char *)&currDayPath[0],"%04d%02d%02d",time.year,time.month,time.day);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA, dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,uteModuleSprotMute = 0x%d,", __func__,uteModuleSprotMute);
    //清空数据
    memset(uteModuleSprotData.dailyActivity.everyHourStandTime,0x00,sizeof(uteModuleSprotData.dailyActivity.everyHourStandTime[0])*24);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,dirInfo->filesCnt = %d,", __func__,dirInfo->filesCnt);
    for(uint8_t i = dirInfo->filesCnt; i > 0; i--)
    {
        uint32_t fileSize = 0;
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA,&dirInfo->filesName[i-1][0]);
        if(memcmp(&currDayPath[0],&dirInfo->filesName[i-1][0],8) == 0)  //currDay
        {
            if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
            {
                uteModuleFilesystemSeek(file,0,FS_SEEK_END);
                fileSize = uteModuleFilesystemGetFileSize(file);

                uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
                uteModuleFilesystemReadData(file,&readbuff[0],fileSize);
                uteModuleFilesystemCloseFile(file);
            }
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,fileSize = %d,", __func__,fileSize);
            for(uint8_t j=0; j<(fileSize/4); j++)
            {
                if(readbuff[0+j*4]<24)
                {
                    uteModuleSprotData.dailyActivity.everyHourStandTime[readbuff[0+j*4]] += (readbuff[2+j*4]<<8|readbuff[3+j*4]);
                    if((uteModuleSprotData.dailyActivity.everyHourStandTime[readbuff[0+j*4]]!=0) &&(uteModuleSprotData.dailyActivity.everyHourStandTime[readbuff[0+j*4]] !=0xffff))
                    {
                        totalStandTimeMin +=60;
                    }
                }
            }
        }
    }
    uteModulePlatformMemoryFree(dirInfo);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,totalStandTimeMin=%d", __func__,totalStandTimeMin);
    return totalStandTimeMin;
}
#endif
/**
*@brief     读取每小时所有运动的步数数据
*@details   开机的时候读取flash里面的保存数据
*@author    casen
*@date      2022-07-14
*/
void uteModuleSportReadTodayEveryHourAllSportStepData(void)
{

    void *file;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_ALLSPORT_STEP_DATA,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&uteModuleSprotData.dailyActivity.everyHourAllSportStep[0],24*4);
        uteModuleFilesystemCloseFile(file);
    }
#if UTE_LOG_STEP_LVL
    for(uint8_t i=0; i<6; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,read everyHourAllSportStep =%d,%d,%d,%d",__func__, uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4],uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4+1], \
                       uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4+2],uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4+3]);
    }
#endif
}

/**
*@brief     保存每小时所有运动步数数据
*@details   关机的时候，保存数据到flash
*@author     casen
*@date        2022-07-14
*/
void uteModuleSportSaveTodayEveryHourAllSportStepData(void)
{
    /*! 保存目标数据*/
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_ALLSPORT_STEP_DATA, &file, FS_O_RDONLY|FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&uteModuleSprotData.dailyActivity.everyHourAllSportStep[0],24*4);
        uteModuleFilesystemCloseFile(file);
    }
#if UTE_LOG_STEP_LVL
    for(uint8_t i=0; i<6; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,read everyHourAllSportStep =%d,%d,%d,%d",__func__, uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4],uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4+1], \
                       uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4+2],uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4+3]);
    }
#endif
}

/**
*@brief     读取每小时所有运动的卡路里数据
*@details
*@author    casen
*@date      2022-07-14
*/
void uteModuleSportReadTodayEveryHourAllSportKcalData(void)
{
    void *file;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_ALLSPORT_KCAL_DATA,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&uteModuleSprotData.dailyActivity.everyHourAllSportKcal[0],24*2);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief     读取一天的所有步数，包括多运动的
*@details
*@param[in] ute_module_systemtime_time_t time, 读取的时间
*@return uint32_t 返回总步数
*@author        zn.zeng
*@date        2022-09-02
*/
uint32_t uteModuleSportReadAllStepWithTime(ute_module_systemtime_time_t time)
{
    uint32_t totalStep = 0;
    void *file;
    uint8_t path[20];
    uint16_t buffSize = 12 * 24 + 4;
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
    memset(&path[0], 0, 20);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR, time.year, time.month, time.day);
    // read
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readBuff[0], buffSize);
        uteModuleFilesystemCloseFile(file);
        for (uint8_t i = 0; i < 24; i++)
        {
            uint32_t tmpTotal = (readBuff[4 + i * 12] << 8 | readBuff[4 + i * 12 + 1]);
            totalStep += tmpTotal;
        }
    }
    uteModulePlatformMemoryFree(readBuff);
    ute_sports_today_with_step_data_t moreStep;
    moreStep.step = 0;
    uteModuleSportReadTodayAllSportDataConfig(time,&moreStep,NULL);
    totalStep = totalStep + moreStep.step;
    return totalStep;
}
/**
*@brief     读取一周内每天的总计步数据
*@details
*@author        zn.zeng
*@date        2022-08-19
*/
void uteModuleSportReadWeekDayStep(void)
{
    ute_module_systemtime_time_t time;
    ute_module_systemtime_time_t readTime;
    uteModuleSystemtimeGetTime(&time);
    memset(&uteModuleSprotData.dailyActivity.weekDayTotalStep[0],0,28);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL,"%s,%04d%02d%02d,week=%d",__func__,time.year,time.month,time.day,time.week);
    for (uint8_t i = 0; i <= time.week; i++)
    {
        memcpy(&readTime,&time,sizeof(ute_module_systemtime_time_t));
        uteModuleSystemtimeInputDateIncreaseDecreaseDay(&readTime,i-time.week);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL,"%s,readTime,%04d%02d%02d,week=%d",__func__,readTime.year,readTime.month,readTime.day,readTime.week);
        uteModuleSprotData.dailyActivity.weekDayTotalStep[i] = uteModuleSportReadAllStepWithTime(readTime);
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL,"%s,weekDayTotalStep[%d]=%d",__func__,i,uteModuleSprotData.dailyActivity.weekDayTotalStep[i]);
    }
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL,"%s,finish",__func__);
}
/**
*@brief     保存每小时所有卡路里步数数据
*@details
*@author        dengli.lu
*@date        2022-03-18
*/
void uteModuleSportSaveTodayEveryHourAllSportKcalData(void)
{
    /*! 保存目标数据*/
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_ALLSPORT_KCAL_DATA, &file, FS_O_RDONLY|FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&uteModuleSprotData.dailyActivity.everyHourAllSportKcal[0],24 * 2);
        uteModuleFilesystemCloseFile(file);
    }

}
/**
*@brief     加载今日活动每个小时的步数柱状图
*@details
*@param[in] everyHourStepHistorygramGraph
*@param[in] color 颜色
*@param[in] x开始坐标
*@param[in] y开始坐标
*@param[in] drawWidth 宽度
*@param[in] intervalWidth 之间的间隔
*@param[in] hightRange 对应的像素高度
*@return  uint32_t  当天最大数据的整1000倍数据，如2300，返回3000，最小返回1000
*@author      dengli.lu
*@date       2022-07-14
*/
uint32_t uteModuleSportLoadTodayEveryHourStepHistoryData(UT_GraphsParam *everyHourStepHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange)
{
    //step graph
    uint32_t maxStep = 0,everySportStep = 0;
    uint32_t intStep1000 = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    memset(everyHourStepHistorygramGraph,0, sizeof(UT_GraphsParam)*24);
    for(uint8_t i = 0; i < 24; i++)
    {
        everySportStep = 0;
        if(i > time.hour)
        {
            break;
        }
#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
        everySportStep = uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[i];
        if ((uteModuleSprotData.dailyActivity.everyHourRunStep[i] + uteModuleSprotData.dailyActivity.everyHourWalkStep[i] + everySportStep) > maxStep)
        {
            maxStep = uteModuleSprotData.dailyActivity.everyHourRunStep[i] + uteModuleSprotData.dailyActivity.everyHourWalkStep[i] + everySportStep;
        }
#else
        if(i == 0)
        {
            everySportStep = uteModuleSprotData.dailyActivity.everyHourAllSportStep[0];
        }
        else
        {
            if(uteModuleSprotData.dailyActivity.everyHourAllSportStep[i] > uteModuleSprotData.dailyActivity.everyHourAllSportStep[i-1])
            {
                everySportStep = uteModuleSprotData.dailyActivity.everyHourAllSportStep[i] - uteModuleSprotData.dailyActivity.everyHourAllSportStep[i-1];
            }
        }
        if((everySportStep + uteModuleSprotData.dailyActivity.everyHourRunStep[i] + uteModuleSprotData.dailyActivity.everyHourWalkStep[i]) > maxStep)
        {
            maxStep = everySportStep + uteModuleSprotData.dailyActivity.everyHourRunStep[i] + uteModuleSprotData.dailyActivity.everyHourWalkStep[i];
        }
#endif
        if(maxStep > 99999)
        {
            maxStep = 99999;
        }
        UTE_MODULE_LOG(1, "%s,everySportStep[%d] = %d,.everyHourRunStep = %d,.everyHourWalkStep = %d", __func__, i, everySportStep, uteModuleSprotData.dailyActivity.everyHourRunStep[i], uteModuleSprotData.dailyActivity.everyHourWalkStep[i]);
    }
    UTE_MODULE_LOG(1,"%s,maxStep = %d",__func__,maxStep);
    if(maxStep%1000)
    {
        intStep1000 = (maxStep/1000+1)*1000;
    }
    else
    {
        intStep1000 = maxStep;
        if(intStep1000==0)
        {
            intStep1000 = 1000;
        }
    }
    UTE_MODULE_LOG(1,"%s,maxStep = %d,intStep1000 = %d",__func__,maxStep,intStep1000);
    for(uint8_t i=0; i<24; i++)
    {
        everySportStep = 0;
        if(i>time.hour)
        {
            break;
        }
#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
        everySportStep = uteModuleSprotData.dailyActivity.currentDayEveryHourSportStep[i];
#else
        if(i == 0)
        {
            everySportStep = uteModuleSprotData.dailyActivity.everyHourAllSportStep[0];
        }
        else
        {
            if(uteModuleSprotData.dailyActivity.everyHourAllSportStep[i] > uteModuleSprotData.dailyActivity.everyHourAllSportStep[i-1])
            {
                everySportStep = uteModuleSprotData.dailyActivity.everyHourAllSportStep[i] - uteModuleSprotData.dailyActivity.everyHourAllSportStep[i-1];
            }
        }
#endif
        uint32_t step = everySportStep + uteModuleSprotData.dailyActivity.everyHourRunStep[i] + uteModuleSprotData.dailyActivity.everyHourWalkStep[i];
        if(step > 99999)
        {
            step = 99999;
        }
        UTE_MODULE_LOG(1,"%s,step[%d] = %d,hightRange = %d,hight = %d",__func__,i,step,hightRange,(step)*hightRange/intStep1000);
        everyHourStepHistorygramGraph[i].colorData = color;
        everyHourStepHistorygramGraph[i].width = drawWidth;
#if ACTIVITY_DAY_STEP_MAX_INT1000_SUPPORT
        everyHourStepHistorygramGraph[i].hight = (step)*hightRange/intStep1000;
#else
        everyHourStepHistorygramGraph[i].hight = (step)*hightRange/maxStep;
#endif
        if(everyHourStepHistorygramGraph[i].hight == 0 && (step!=0))
        {
            everyHourStepHistorygramGraph[i].hight = 1;
        }
        everyHourStepHistorygramGraph[i].x = x + (drawWidth + intervalWidth)*i;
        everyHourStepHistorygramGraph[i].y = y - everyHourStepHistorygramGraph[i].hight;
    }
#if UTE_LOG_STEP_LVL
    for(uint8_t i=0; i<6; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "everyHourAllSportStep =%d,%d,%d,%d", uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4],uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4+1], \
                       uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4+2],uteModuleSprotData.dailyActivity.everyHourAllSportStep[i*4+3]);
    }
#endif
    return intStep1000;
}
/**
*@brief     加载今日活动每个小时的Kcal柱状图
*@details
*@param[in] everyHourStepHistorygramGraph
*@param[in] color 颜色
*@param[in] x开始坐标
*@param[in] y开始坐标
*@param[in] drawWidth 宽度
*@param[in] intervalWidth 之间的间隔
*@param[in] hightRange 对应的像素高度
*@return    当天有数据返回true,没有则返回false
*@author      dengli.lu
*@date       2022-07-14
*/
bool uteModuleSportLoadTodayEveryHourKcalHistoryData(UT_GraphsParam *everyHourKcalHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange)
{
    uint16_t maxKcal = 0,everySportKcal = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uteModuleSportGetCurrDayEveryHourKcal();
    memset(everyHourKcalHistorygramGraph,0, sizeof(UT_GraphsParam)*24);
    for(uint8_t i = 0; i < 24; i++)
    {
        everySportKcal = 0;
        if(i > time.hour)
        {
            break;
        }
        if(i == 0)
        {
            everySportKcal = uteModuleSprotData.dailyActivity.everyHourAllSportKcal[0];
        }
        else
        {
            if(uteModuleSprotData.dailyActivity.everyHourAllSportKcal[i] > uteModuleSprotData.dailyActivity.everyHourAllSportKcal[i-1])
            {
                everySportKcal = uteModuleSprotData.dailyActivity.everyHourAllSportKcal[i] - uteModuleSprotData.dailyActivity.everyHourAllSportKcal[i-1];
            }
        }
        if((everySportKcal + uteModuleSprotData.dailyActivity.everyHourKcal[i]) > maxKcal)
        {
            maxKcal = everySportKcal + uteModuleSprotData.dailyActivity.everyHourKcal[i];
        }
        if(maxKcal > 9999)
        {
            maxKcal = 9999;
        }
    }
    if(maxKcal == 0)
    {
        return false;
    }
    for(uint8_t i = 0; i < 24; i++)
    {
        everySportKcal = 0;
        if(i > time.hour)
        {
            break;
        }
        if(i == 0)
        {
            everySportKcal = uteModuleSprotData.dailyActivity.everyHourAllSportKcal[0];
        }
        else
        {
            if(uteModuleSprotData.dailyActivity.everyHourAllSportKcal[i] > uteModuleSprotData.dailyActivity.everyHourAllSportKcal[i-1])
            {
                everySportKcal = uteModuleSprotData.dailyActivity.everyHourAllSportKcal[i] - uteModuleSprotData.dailyActivity.everyHourAllSportKcal[i-1];
            }
        }
        uint16_t kcal = everySportKcal + uteModuleSprotData.dailyActivity.everyHourKcal[i];
        if(kcal > 9999)
        {
            kcal = 9999;
        }
        everyHourKcalHistorygramGraph[i].colorData = color;
        everyHourKcalHistorygramGraph[i].width = drawWidth;
        everyHourKcalHistorygramGraph[i].hight = (kcal)*hightRange/maxKcal;
        if((everyHourKcalHistorygramGraph[i].hight == 0) && (kcal != 0))
        {
            everyHourKcalHistorygramGraph[i].hight = 1;
        }
        everyHourKcalHistorygramGraph[i].x = x + (drawWidth + intervalWidth)*i;
        everyHourKcalHistorygramGraph[i].y = y - everyHourKcalHistorygramGraph[i].hight;
    }
    return true;
}
/**
*@brief     加载今日活动每个小时的运动时长柱状图
*@details
*@param[in] everyHourStepHistorygramGraph
*@param[in] color 颜色
*@param[in] x开始坐标
*@param[in] y开始坐标
*@param[in] drawWidth 宽度
*@param[in] intervalWidth 之间的间隔
*@param[in] hightRange 对应的像素高度
*@return    当天有数据返回true,没有则返回false
*@author      dengli.lu
*@date       2022-07-14
*/
bool uteModuleSportLoadTodayEveryHourSportTimeHistoryData(UT_GraphsParam *everyHourSportTimeHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange)
{
    uint16_t maxSportTime = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uteModuleSportGetCurrDayEveryHourSportTime();
    memset(everyHourSportTimeHistorygramGraph,0, sizeof(UT_GraphsParam)*24);
    for(uint8_t i = 0; i < 24; i++)
    {
        if(i > time.hour)
        {
            break;
        }
        if(uteModuleSprotData.dailyActivity.everyHourSportTime[i] > maxSportTime)
        {
            maxSportTime = uteModuleSprotData.dailyActivity.everyHourSportTime[i];
        }
        if(maxSportTime > 60)
        {
            maxSportTime = 60;
        }
    }
    if(maxSportTime == 0)
    {
        return false;
    }
    for(uint8_t i = 0; i < 24; i++)
    {
        if(i > time.hour)
        {
            break;
        }
        uint16_t sportTime = uteModuleSprotData.dailyActivity.everyHourSportTime[i];
        if(sportTime > 60)
        {
            sportTime = 60;
        }
        everyHourSportTimeHistorygramGraph[i].colorData = color;
        everyHourSportTimeHistorygramGraph[i].width = drawWidth;
        everyHourSportTimeHistorygramGraph[i].hight = (sportTime)*hightRange/maxSportTime;
        if(everyHourSportTimeHistorygramGraph[i].hight == 0 && (sportTime != 0))
        {
            everyHourSportTimeHistorygramGraph[i].hight  = 1;
        }
        everyHourSportTimeHistorygramGraph[i].x = x + (drawWidth + intervalWidth)*i;
        everyHourSportTimeHistorygramGraph[i].y = y - everyHourSportTimeHistorygramGraph[i].hight;
    }
    return true;
}
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
/**
*@brief     加载今日活动每个小时的站立时长柱状图
*@details
*@param[in] everyHourStepHistorygramGraph
*@param[in] color 颜色
*@param[in] x开始坐标
*@param[in] y开始坐标
*@param[in] drawWidth 宽度
*@param[in] intervalWidth 之间的间隔
*@param[in] hightRange 对应的像素高度
*@return    当天有数据返回true,没有则返回false
*@author      dengli.lu
*@date       2022-07-14
*/
uint16_t uteModuleSportLoadTodayEveryHourStandTimeHistoryData(UT_GraphsParam *everyHourStandTimeHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint16_t totalStandTimeMin = uteModuleSportGetCurrDayEveryHourStandTime();
    memset(everyHourStandTimeHistorygramGraph,0,sizeof(UT_GraphsParam)*24);
    for(uint8_t i = 0; i < 24; i++)
    {
        uint16_t standTime = uteModuleSprotData.dailyActivity.everyHourStandTime[i];
        if((standTime != 0) && (standTime != 0xffff))
        {
            standTime = 60;
        }
        everyHourStandTimeHistorygramGraph[i].colorData = color;
        everyHourStandTimeHistorygramGraph[i].width = drawWidth;
        everyHourStandTimeHistorygramGraph[i].hight = (standTime)*hightRange/60;
        if((everyHourStandTimeHistorygramGraph[i].hight == 0) && (standTime != 0))
        {
            everyHourStandTimeHistorygramGraph[i].hight = 1;
        }
        everyHourStandTimeHistorygramGraph[i].x = x + (drawWidth + intervalWidth)*i;
        everyHourStandTimeHistorygramGraph[i].y = y - everyHourStandTimeHistorygramGraph[i].hight;
    }
    return totalStandTimeMin;
}
#endif
/**
*@brief     加载今日活动周每天的步数柱状图
*@details
*@param[in] weekDayStepHistorygramGraph
*@param[in] color 颜色
*@param[in] x开始坐标
*@param[in] y开始坐标
*@param[in] drawWidth 宽度
*@param[in] intervalWidth 之间的间隔
*@param[in] hightRange 对应的像素高度
*@param[out] *totalStep 周步数总步数
*@return  uint32_t  当周最大数据的整1000倍数据，如2300，返回3000，最小返回1000
*@author      zn.zeng
*@date       2022-09-02
*/
uint32_t uteModuleSportLoadWeekDayStepHistoryData(UT_GraphsParam *weekDayStepHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange,uint32_t *totalStep)
{
    //step graph
    uint32_t maxStep = 0;
    uint32_t intStep1000 = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint32_t currTotal = 0;
    uteModuleSportGetCurrDayStepCnt(&currTotal,NULL,NULL);
    uteModuleSprotData.dailyActivity.weekDayTotalStep[time.week] = currTotal;
    memset(weekDayStepHistorygramGraph,0, sizeof(UT_GraphsParam)*7);
    *totalStep = 0;
    for(uint8_t i = 0; i < 7; i++)
    {
#if UTE_LOG_GUI_LVL
        uteModuleSprotData.dailyActivity.weekDayTotalStep[i] = 1300*(i+1);
#endif
        *totalStep = *totalStep + uteModuleSprotData.dailyActivity.weekDayTotalStep[i];
        if(uteModuleSprotData.dailyActivity.weekDayTotalStep[i]>maxStep)
        {
            maxStep = uteModuleSprotData.dailyActivity.weekDayTotalStep[i];
        }
        if(maxStep > 99999)
        {
            maxStep = 99999;
        }
    }
    if (maxStep % 1000)
    {
        intStep1000 = (maxStep / 1000 + 1) * 1000;
    }
    else
    {
        intStep1000 = maxStep;
        if (intStep1000 == 0)
        {
            intStep1000 = 1000;
        }
    }
    if (maxStep != 0)
    {
        for (uint8_t i = 0; i < 7; i++)
        {
            uint32_t step = uteModuleSprotData.dailyActivity.weekDayTotalStep[i];
            if (step > 99999)
            {
                step = 99999;
            }
            weekDayStepHistorygramGraph[i].colorData = color;
            weekDayStepHistorygramGraph[i].width = drawWidth;
            weekDayStepHistorygramGraph[i].hight = (step)*hightRange / intStep1000;
            if (weekDayStepHistorygramGraph[i].hight == 0 && (step != 0))
            {
                weekDayStepHistorygramGraph[i].hight = 2;
            }
            weekDayStepHistorygramGraph[i].x = x + (drawWidth + intervalWidth) * i;
            weekDayStepHistorygramGraph[i].y = y - weekDayStepHistorygramGraph[i].hight;
        }
    }
    return intStep1000;
}
#endif

#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
/**
*@brief  多运动目标提醒初始化函数
*@details
*@author        dengli.lu
*@date        2022-03-08
*/
void uteModuleSportMoreSportsTargetInit(void)
{
    memset(&uteModuleSprotData.moreSportsTarget,0,sizeof(ute_module_sports_target_data_t));
#if UTE_MORE_SPORTS_TARGET_SAVE_SUPPORT
    uteModuleSportMoreSportsTargetReadConfig();
#endif
}
/**
*@brief     多运动目标提醒读取数据
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleSportMoreSportsTargetReadConfig(void)
{
    /*! 保存到文件dengli.lu, 2022-04-24*/
    void *file;
    uint8_t readbuff[11];
    memset(readbuff,0,11);
    readbuff[0] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET>>24)&0xff;
    readbuff[1] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET>>16)&0xff;
    readbuff[2] = (UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET>>8)&0xff;
    readbuff[3] = UTE_MORE_SPORTS_DEFAULT_DISTANCE_TARGET&0xff;
    uint32_t timeTargetSec = UTE_MORE_SPORTS_DEFAULT_TIME_TARGET_MIN*60;
    readbuff[4] = (timeTargetSec>>24)&0xff;
    readbuff[5] = (timeTargetSec>>16)&0xff;
    readbuff[6] = (timeTargetSec>>8)&0xff;
    readbuff[7] = timeTargetSec&0xff;
    readbuff[8] = (UTE_MORE_SPORTS_DEFAULT_KCAL_TARGET>>8)&0xff;
    readbuff[9] = UTE_MORE_SPORTS_DEFAULT_KCAL_TARGET&0xff;
    readbuff[10] = UTE_MORE_SPORTS_DEFAULT_NO_TARGET_MODE;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_DATA,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],11);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleSprotData.moreSportsTarget.sportDistanceTargeCnt = readbuff[0]<<24|readbuff[1]<<16|readbuff[2]<<8|readbuff[3];
    uteModuleSprotData.moreSportsTarget.sportTimeTargetSec = readbuff[4]<<24|readbuff[5]<<16|readbuff[6]<<8|readbuff[7];
    uteModuleSprotData.moreSportsTarget.sportKcalTarget = readbuff[8]<<8|readbuff[9];
    uteModuleSprotData.moreSportsTarget.isNoTarget = readbuff[10];
}
/**
*@brief     多运动目标提醒保存数据到内存
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleSportMoreSportsTargetSaveData(void)
{
    /*! 保存到文件，dengli.lu, 2022-04-24*/
    void *file;
    uint8_t writebuff[11];
    writebuff[0] = (uteModuleSprotData.moreSportsTarget.sportDistanceTargeCnt>>24)&0xff;
    writebuff[1] = (uteModuleSprotData.moreSportsTarget.sportDistanceTargeCnt>>16)&0xff;
    writebuff[2] = (uteModuleSprotData.moreSportsTarget.sportDistanceTargeCnt>>8)&0xff;
    writebuff[3] = (uteModuleSprotData.moreSportsTarget.sportDistanceTargeCnt)&0xff;
    writebuff[4] = (uteModuleSprotData.moreSportsTarget.sportTimeTargetSec>>24)&0xff;
    writebuff[5] = (uteModuleSprotData.moreSportsTarget.sportTimeTargetSec>>16)&0xff;
    writebuff[6] = (uteModuleSprotData.moreSportsTarget.sportTimeTargetSec>>8)&0xff;
    writebuff[7] = (uteModuleSprotData.moreSportsTarget.sportTimeTargetSec)&0xff;
    writebuff[8] = (uteModuleSprotData.moreSportsTarget.sportKcalTarget>>8)&0xff;
    writebuff[9] = (uteModuleSprotData.moreSportsTarget.sportKcalTarget)&0xff;
    writebuff[10] = uteModuleSprotData.moreSportsTarget.isNoTarget;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_DATA,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],11);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief  设置多运动目标
*@details
*@param[in] ute_module_sports_target_data_t setMoreSportsTarget  传入要设置多运动目标参数的指针
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportSetMoreSportsTargetData(ute_module_sports_target_data_t setMoreSportsTarget)
{
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    memcpy(&uteModuleSprotData.moreSportsTarget, &setMoreSportsTarget, sizeof(ute_module_sports_target_data_t));
#if UTE_MORE_SPORTS_TARGET_SAVE_SUPPORT
    uteModuleSportMoreSportsTargetSaveData();
#endif
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
/**
*@brief  获取多运动目标
*@details
*@param[in] ute_module_sports_target_data_t *sportsTarget  传入获取多运动目标参数的指针
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportGetMoreSportsTargetData(ute_module_sports_target_data_t *sportsTarget)
{
    uteModulePlatformTakeMutex(uteModuleSprotMute);
    memcpy(sportsTarget,&uteModuleSprotData.moreSportsTarget, sizeof(ute_module_sports_target_data_t));
    if(uteModuleSystemtimeGetDistanceMiType())
    {
        sportsTarget->sportDistanceTargeCnt = sportsTarget->sportDistanceTargeCnt * 0.6213712f;
    }
    uteModulePlatformGiveMutex(uteModuleSprotMute);
}
/**
*@brief        多运动目标处理
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleSportMoreSportsTargetsProcess(void)
{
    uint32_t sportDistancemi = 0;
    float distance = 0;
    distance = (float)(uteModuleSprotData.moreSportData.saveData.sportDistanceInteger + (float)(uteModuleSprotData.moreSportData.saveData.sportDistanceDecimals*0.01f));
    distance = distance*1000.0f; //放大1000倍,m
    sportDistancemi = (uint32_t)distance;
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,status =%d", __func__,uteModuleSprotData.moreSportData.status);
    if((uteModuleSprotData.moreSportData.totalSportTime > 0) && (!uteModuleSprotData.moreSportsTarget.isNoTarget))//启动运动目标提醒
    {
        if(0)
        {}
#if UTE_SPORTS_ITTEGER_KM_DISTANCE_TARGET//整公里提醒
        else if(sportDistancemi >= (UTE_SPORTS_ITTEGER_KM_DISTANCE_TARGET*1000))
        {
            if(uteModuleSprotData.moreSportsTarget.isSportIntegerKmHasNotify == false)
            {
                uteModuleSprotData.moreSportsTarget.isSportIntegerKmNotify = true;
                uteModuleSprotData.moreSportsTarget.isSportTimeTargetNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportIntegerHourNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportKcalTargetNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportDistanceTargetNotify = false;
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORT_TARGET_NOTIFY_ID);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,3);
                uteModuleSprotData.moreSportsTarget.isSportIntegerKmHasNotify = true;
            }
        }
#endif
        else if((uteModuleSprotData.moreSportData.totalSportTime >= uteModuleSprotData.moreSportsTarget.sportTimeTargetSec) && (uteModuleSprotData.moreSportsTarget.sportTimeTargetSec > 0))
        {
            if(uteModuleSprotData.moreSportsTarget.isSportTimeTargetHasNotify == false)
            {
                uteModuleSprotData.moreSportsTarget.isSportIntegerKmNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportTimeTargetNotify = true;
                uteModuleSprotData.moreSportsTarget.isSportIntegerHourNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportKcalTargetNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportDistanceTargetNotify = false;
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORT_TARGET_NOTIFY_ID);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,3);
                uteModuleSprotData.moreSportsTarget.isSportTimeTargetHasNotify = true;
            }
        }
#if UTE_SPORTS_ITTEGER_HOUR_TIME_TARGET
        else if(uteModuleSprotData.moreSportData.totalSportTime >= (UTE_SPORTS_ITTEGER_HOUR_TIME_TARGET*3600))
        {
            if(uteModuleSprotData.moreSportsTarget.isSportIntegerHourHasNotify == false)
            {
                uteModuleSprotData.moreSportsTarget.isSportIntegerKmNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportTimeTargetNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportIntegerHourNotify = true;
                uteModuleSprotData.moreSportsTarget.isSportKcalTargetNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportDistanceTargetNotify = false;
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORT_TARGET_NOTIFY_ID);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,3);
                uteModuleSprotData.moreSportsTarget.isSportIntegerHourHasNotify = true;
            }
        }
#endif
        else if((uteModuleSprotData.moreSportData.saveData.sportCaloire >= uteModuleSprotData.moreSportsTarget.sportKcalTarget) && (uteModuleSprotData.moreSportsTarget.sportKcalTarget > 0))
        {
            if(uteModuleSprotData.moreSportsTarget.isSportKcalTargetHasNotify == false)
            {
                uteModuleSprotData.moreSportsTarget.isSportIntegerKmNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportTimeTargetNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportIntegerHourNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportKcalTargetNotify = true;
                uteModuleSprotData.moreSportsTarget.isSportDistanceTargetNotify = false;
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORT_TARGET_NOTIFY_ID);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,3);
                uteModuleSprotData.moreSportsTarget.isSportKcalTargetHasNotify = true;
            }
        }
        else if((sportDistancemi >= uteModuleSprotData.moreSportsTarget.sportDistanceTargeCnt) && (uteModuleSprotData.moreSportsTarget.sportDistanceTargeCnt >0 ))
        {
            if(uteModuleSprotData.moreSportsTarget.isSportDistanceTargetHasNotify == false)
            {
                uteModuleSprotData.moreSportsTarget.isSportIntegerKmNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportTimeTargetNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportIntegerHourNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportKcalTargetNotify = false;
                uteModuleSprotData.moreSportsTarget.isSportDistanceTargetNotify = true;
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORT_TARGET_NOTIFY_ID);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,3);
                uteModuleSprotData.moreSportsTarget.isSportDistanceTargetHasNotify = true;
            }
        }
    }
}
#endif
/**
*@brief        处理运动心率值警告数据
*@details
*@author      denglilu
*@date       2022-03-16
*/
#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
void uteModuleSportMoreSportsHeartWarningProcess(void)
{
    uint8_t heart = (uint8_t)uteModuleHeartGetHeartValue();
    uint32_t warningSecond = uteModuleHeartGetSportHeartWaringSecond();
    if(uteModuleHeartIsWear() && (heart!=0xff) && (heart!=0))
    {
        if(uteModuleHeartGetSportHeartWaringMinIsOpen() || uteModuleHeartGetSportHeartWaringMaxIsOpen())
        {
            if((heart < uteModuleHeartGetSportHeartWaringMinValue())||(heart > uteModuleHeartGetSportHeartWaringMaxValue()))
            {
                warningSecond++;
                if(warningSecond >= UTE_MODULE_HEART_MIN_MAX_WARNING_FRIST_SECOND_CNT)
                {
                    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_HEART_WARNING_SCREEN_ID);
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
                    warningSecond = 0;
                }
            }
            else
            {
                warningSecond = 0;
            }
        }
        else
        {
            warningSecond = 0;
        }
    }
    else
    {
        warningSecond = 0;
    }
    uteModuleHeartSetSportHeartWaringSecond(warningSecond);
}
#endif

/**
*@brief       获取多运动的运动断开连接超时计数
*@details
*@author     casen
*@date       2022-09-04
*/
uint16_t uteModuleSportMoreSportGetDisconnectTimeoutCnt(void)
{
    return uteModuleSprotData.moreSportData.appStartSportDisconnectTimeOutCnt;
}

#if UTE_MODULE_SPORT_CLASSIFICATION_SUPPORT
/**
*@brief       获取多运动的运动分类界面
*@details
*@author     xjc
*@date       2022-11-28
*/
TRAIN_LIST_PAGE_T uteModuleSportMoreSportGetTrainListPage(void)
{
    return uteModuleSprotData.trainListPage;
}
/**
*@brief      设置多运动的运动分类界面
*@details
*@author     xjc
*@date       2022-11-28
*/
void uteModuleSportMoreSportSetTrainListPage(TRAIN_LIST_PAGE_T trainListPage)
{
    uteModuleSprotData.trainListPage = trainListPage;
}
#endif

#if UTE_MODULE_SPORTS_HISTORY_HEART_ZONE_SUPPORT

ute_module_sport_heart_zone_data_t sportHeartZoneData;
void uteModuleSportGetHeartZoneArray(ute_module_sport_heart_zone_data_t *zoneData)
{
    memcpy(zoneData,&sportHeartZoneData,sizeof(ute_module_sport_heart_zone_data_t));
}
uint8_t  uteModuleSportGetHeartZoneRange(uint8_t *heartHistory,uint8_t heartCnt,uint8_t *reduceStatus,uint8_t *burnFatStatus,uint8_t *heartLungStatus,uint8_t *noOxygenStatus,uint8_t *limitStatus)
{
    uint8_t zoneRange = 0;
    uint8_t reduceStatusCounter =0; // 减压
    uint8_t burnFatStatusCounter =0;    //燃脂
    uint8_t heartLungStatusCounter =0;  // 心肺
    uint8_t noOxygenStatusCounter =0;   // 无氧
    uint8_t limitStatusCounter =0;  // 极限
    uint8_t heartHistoryCounter = 0;
    for(uint8_t index =0; index<heartCnt; index++)
    {
        if((heartHistory[index]!=0) &&(heartHistory[index]!=0xff))
        {
            zoneRange = uteModuleHeartGetHeartValueRange(heartHistory[index]);
            UTE_MODULE_LOG(1,"%s,HEART [%d] = %d ,zoneRange = %d",__func__,index,heartHistory[index],zoneRange);
            if(zoneRange!=1)  //过滤静息
            {
                heartHistoryCounter++;
            }
            if(zoneRange == 2)
            {
                reduceStatusCounter++;
            }
            else if(zoneRange == 3)
            {
                burnFatStatusCounter++;
            }
            else if(zoneRange == 4)
            {
                heartLungStatusCounter++;
            }
            else if(zoneRange == 5)
            {
                noOxygenStatusCounter++;
            }
            else if(zoneRange == 0)
            {
                limitStatusCounter++;
            }
        }
    }
    (*reduceStatus) = reduceStatusCounter;
    (*burnFatStatus) = burnFatStatusCounter;
    (*heartLungStatus) = heartLungStatusCounter;
    (*noOxygenStatus) = noOxygenStatusCounter;
    (*limitStatus) = limitStatusCounter;
    return heartHistoryCounter;
}

void  uteModuleSportGetHeartZoneRangePercent(uint8_t heartHistoryCounter,uint8_t reduceStatusCounter,uint8_t burnFatStatusCounter,uint8_t heartLungStatusCounter,uint8_t noOxygenStatusCounter,uint8_t limitStatusCounter)
{
    uint8_t reduceStatusPencent = 0;
    uint8_t burnFatStatusPencent = 0;
    uint8_t heartLungStatusPencent = 0;
    uint8_t noOxygenStatusPencent = 0;
    uint8_t limitStatusPencent = 0;
    if(heartHistoryCounter!=0)
    {
        int8_t zoneTotalPercent = 0,zoneMaxPercent = 0,devPercent = 0;
        reduceStatusPencent = (uint8_t) (reduceStatusCounter*100.0f/heartHistoryCounter+0.5f);
        if(zoneMaxPercent<reduceStatusPencent)
        {
            zoneMaxPercent = reduceStatusPencent;
        }
        zoneTotalPercent += reduceStatusPencent;

        burnFatStatusPencent = (uint8_t) (burnFatStatusCounter*100.0f/heartHistoryCounter+0.5f);
        if(zoneMaxPercent<burnFatStatusPencent)
        {
            zoneMaxPercent = burnFatStatusPencent;
        }
        zoneTotalPercent += burnFatStatusPencent;

        heartLungStatusPencent = (uint8_t) (heartLungStatusCounter*100.0f/heartHistoryCounter+0.5f);
        if(zoneMaxPercent<heartLungStatusPencent)
        {
            zoneMaxPercent = heartLungStatusPencent;
        }
        zoneTotalPercent += heartLungStatusPencent;

        noOxygenStatusPencent = (uint8_t) (noOxygenStatusCounter*100.0f/heartHistoryCounter+0.5f);
        if(zoneMaxPercent<noOxygenStatusPencent)
        {
            zoneMaxPercent = noOxygenStatusPencent;
        }
        zoneTotalPercent += noOxygenStatusPencent;

        limitStatusPencent= (uint8_t) (limitStatusCounter*100.0f/heartHistoryCounter+0.5f);
        if(zoneMaxPercent<limitStatusPencent)
        {
            zoneMaxPercent = limitStatusPencent;
        }
        zoneTotalPercent += limitStatusPencent;
        //Casen add 21-02-25
        if(zoneTotalPercent!=100 && zoneTotalPercent != 0)
        {
            devPercent = 100 - zoneTotalPercent;
            if(0)
            {}
            else if(reduceStatusPencent == zoneMaxPercent)
            {
                reduceStatusPencent += devPercent;
            }
            else if(burnFatStatusPencent == zoneMaxPercent)
            {
                burnFatStatusPencent += devPercent;
            }
            else if(heartLungStatusPencent == zoneMaxPercent)
            {
                heartLungStatusPencent += devPercent;
            }
            else if(noOxygenStatusPencent == zoneMaxPercent)
            {
                noOxygenStatusPencent += devPercent;
            }
            else if(limitStatusPencent == zoneMaxPercent)
            {
                limitStatusPencent += devPercent;
            }
        }
    }
    else
    {
        reduceStatusPencent = 0;
        burnFatStatusPencent = 0;
        heartLungStatusPencent = 0;
        noOxygenStatusPencent = 0;
        limitStatusPencent = 0;
    }
    sportHeartZoneData.reducePer =reduceStatusPencent;
    sportHeartZoneData.burnFatPer =burnFatStatusPencent;
    sportHeartZoneData.heartLungPer =heartLungStatusPencent;
    sportHeartZoneData.noOxygenPer =noOxygenStatusPencent;
    sportHeartZoneData.limitPer =limitStatusPencent;

}

/**
*@brief  读取运动历史心率区间数据
*@details  queryTime 查询的时间
*@author   casen
*@date     2023-02-18
*/
void uteModuleSportReadHistoryHeartZoneData(ute_module_systemtime_time_t *queryTime)
{
    void *file=NULL;
    uint8_t readbuff[33];
    memset(&readbuff[0],0,sizeof(readbuff));
    uint8_t path[30];
    memset(&path[0],0,sizeof(path));
    uint8_t datePath[20];
    memset(&datePath[0],0,sizeof(datePath));
    sprintf((char *)&datePath[0],"%04d%02d%02d%02d%02d",queryTime->year,queryTime->month,queryTime->day,queryTime->hour,queryTime->min);
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,datePath = %s", __func__, datePath);
    uint8_t reduceStatusCounter =0; // 减压
    uint8_t burnFatStatusCounter =0;    //燃脂
    uint8_t heartLungStatusCounter =0;  // 心肺
    uint8_t noOxygenStatusCounter =0;   // 无氧
    uint8_t limitStatusCounter =0;  // 极限
    uint8_t heartHistoryCounter =0;
    uint8_t zoneRange = 0;
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR, dirInfo, NULL);
    uteModulePlatformTakeMutex(uteModuleSprotMute);

    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,dirInfo->filesCnt = %d,", __func__,dirInfo->filesCnt);
    for(uint8_t i = dirInfo->filesCnt; i > 0; i--)
    {
        if(memcmp(&datePath[0],&dirInfo->filesName[i-1][0],12)==0)
        {
            sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR,&dirInfo->filesName[i-1][0]);
            UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,path = %s", __func__, path);
            if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
            {
                uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
                uteModuleFilesystemReadData(file,&readbuff[0],33);
                uint16_t heartTotalLen = (uint16_t)(readbuff[31]<<8|readbuff[32]);
                UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,heartTotalLen = %d,readbuff[31] =%d , readbuff[32] = %d", __func__,heartTotalLen,readbuff[31],readbuff[32]);
                /*读取心率数据*/
                uint8_t tempHeartBuff[24];
                uint16_t heartBuffLength = sizeof(tempHeartBuff)/sizeof(&tempHeartBuff[0]);
                memset(tempHeartBuff,0x00,heartBuffLength);
                uint8_t tempReduce,tempBurnFat,tempHeartLung,tempNoOxygen,tempLimit;
                for(uint16_t i = 0; i<(heartTotalLen/heartBuffLength); i++)
                {
                    tempReduce = tempBurnFat = tempHeartLung = tempNoOxygen = tempLimit = 0;
                    uteModuleFilesystemSeek(file,33+i*heartBuffLength,FS_SEEK_SET);
                    uteModuleFilesystemReadData(file,&tempHeartBuff[0],heartBuffLength);
                    heartHistoryCounter += uteModuleSportGetHeartZoneRange(tempHeartBuff,heartBuffLength,&tempReduce,&tempBurnFat,&tempHeartLung,&tempNoOxygen,&tempLimit);
                    //累计
                    reduceStatusCounter += tempReduce;
                    burnFatStatusCounter += tempBurnFat;
                    heartLungStatusCounter += tempHeartLung;
                    noOxygenStatusCounter += tempNoOxygen;
                    limitStatusCounter += tempLimit;
                }
                memset(tempHeartBuff,0x00,heartBuffLength);
                if(heartTotalLen%heartBuffLength)
                {
                    tempReduce = tempBurnFat = tempHeartLung = tempNoOxygen = tempLimit = 0;
                    uteModuleFilesystemSeek(file,33+(heartTotalLen/heartBuffLength)*heartBuffLength,FS_SEEK_SET);
                    uteModuleFilesystemReadData(file,&tempHeartBuff[0],heartTotalLen%heartBuffLength);
                    heartHistoryCounter += uteModuleSportGetHeartZoneRange(tempHeartBuff,heartTotalLen%heartBuffLength,&tempReduce,&tempBurnFat,&tempHeartLung,&tempNoOxygen,&tempLimit);
                    //累计
                    reduceStatusCounter += tempReduce;
                    burnFatStatusCounter += tempBurnFat;
                    heartLungStatusCounter += tempHeartLung;
                    noOxygenStatusCounter += tempNoOxygen;
                    limitStatusCounter += tempLimit;
                }
                UTE_MODULE_LOG(UTE_LOG_STEP_LVL,"%s ,status,%d,%d,%d,%d,%d ,heartHistoryCounter = %d",__func__,reduceStatusCounter,burnFatStatusCounter,heartLungStatusCounter,noOxygenStatusCounter,limitStatusCounter,heartHistoryCounter);
                uteModuleSportGetHeartZoneRangePercent(heartHistoryCounter,reduceStatusCounter,burnFatStatusCounter,heartLungStatusCounter,noOxygenStatusCounter,limitStatusCounter);
                UTE_MODULE_LOG(UTE_LOG_STEP_LVL,"%s ,StatusPencent %d,%d,%d,%d,%d ",__func__,sportHeartZoneData.reducePer,sportHeartZoneData.burnFatPer,sportHeartZoneData.heartLungPer,sportHeartZoneData.noOxygenPer,sportHeartZoneData.limitPer);
                /*读取心率数据*/

                uteModuleFilesystemCloseFile(file);
                break;
            }
        }
    }
    uteModulePlatformGiveMutex(uteModuleSprotMute);
    uteModulePlatformMemoryFree(dirInfo);
}

/**
*@brief     加载心率区间柱状图
*@details   这里加载的是每行独立的区间图，一般在LONGPAGEVIEW 界面中还要重新设置对应的y.
            也可以通过设置x实现区间比例在同一行.
*@param[in] zoneGramGraph 数组指针，需要传递一个类型是UT_GraphsParam 长度是5的数组地址，对应减压、燃脂、心肺、无氧、极限
*@param[in] color 颜色
*@param[in] x开始坐标
*@param[in] y开始坐标
*@param[in] drawWidth 总的宽度用于计算比例
*@param[in] drawHight 高度
*@return
*@author    casen
*@date      2023-02-21
*/
#ifndef SPORT_HEART_RESUCE_24RGB
#define SPORT_HEART_RESUCE_24RGB 0x97e2fa
#endif
#ifndef SPORT_HEART_BURNFAT_24RGB
#define SPORT_HEART_BURNFAT_24RGB 0xcff979
#endif
#ifndef SPORT_HEART_HEARTLUNG_24RGB
#define SPORT_HEART_HEARTLUNG_24RGB 0xf9de60
#endif
#ifndef SPORT_HEART_NO_OXYGEN_24RGB
#define SPORT_HEART_NO_OXYGEN_24RGB 0xe57352
#endif
#ifndef SPORT_HEART_LIMIT_24RGB
#define SPORT_HEART_LIMIT_24RGB 0xea668c
#endif
void uteModuleSportLoadHeartZoneGrapData(UT_GraphsParam (*zoneGramGraph)[5],int16_t x, int16_t y, uint16_t drawWidth, uint16_t drawHight)
{
    for(uint8_t i = 0; i<5; i++)
    {
        (*zoneGramGraph)[i].x = x;
        (*zoneGramGraph)[i].y = y;
        (*zoneGramGraph)[i].width = drawWidth;
        (*zoneGramGraph)[i].hight = drawHight;
    }
#if 0//UTE_MODULE_LOG_SUPPORT
    sportHeartZoneData.reducePer = 20;
    sportHeartZoneData.burnFatPer = 30;
    sportHeartZoneData.heartLungPer = 10;
    sportHeartZoneData.noOxygenPer = 40;
    sportHeartZoneData.limitPer = 50;
#endif
    (*zoneGramGraph)[0].width = sportHeartZoneData.reducePer*drawWidth/100;
    (*zoneGramGraph)[1].width = sportHeartZoneData.burnFatPer*drawWidth/100;
    (*zoneGramGraph)[2].width = sportHeartZoneData.heartLungPer*drawWidth/100;
    (*zoneGramGraph)[3].width = sportHeartZoneData.noOxygenPer*drawWidth/100;
    (*zoneGramGraph)[4].width = sportHeartZoneData.limitPer*drawWidth/100;

    (*zoneGramGraph)[0].colorData = SPORT_HEART_RESUCE_24RGB;
    (*zoneGramGraph)[1].colorData = SPORT_HEART_BURNFAT_24RGB;
    (*zoneGramGraph)[2].colorData = SPORT_HEART_HEARTLUNG_24RGB;
    (*zoneGramGraph)[3].colorData = SPORT_HEART_NO_OXYGEN_24RGB;
    (*zoneGramGraph)[4].colorData = SPORT_HEART_LIMIT_24RGB;

}
#endif

#if APP_DBG_GSENSOR_DATA
uint8_t sendDbgData[160];
uint8_t sendFrameCnt = 0;
uint32_t sendDbgDataCnt = 0;
/**
*@brief        发送gsensor采集原始数据
*@details
*@author       dengli.lu
*@date       2023-09-15
*/
void uteModuleSportSendGsensorDataToPhone(int16_t tempx, int16_t tempy,int16_t tempz)
{
    if(uteModuleSprotData.appDebugGsensorDataSwitch == false)
    {
        sendFrameCnt = 0;
        sendDbgDataCnt = 0;
        return ;
    }
    if(sendFrameCnt==0)
    {
        sendDbgData[0] = 0x48;
        sendDbgData[1] = 0xff;
        sendDbgData[2] = 0xfe;
    }
    if(sendFrameCnt==25)
    {
        sendDbgData[153] = sendDbgDataCnt>>8&0xff;
        sendDbgData[154] = sendDbgDataCnt&0xff;
        sendDbgData[155] = (mStepSleepParam.currentHourStepCnt>>8 & 0xff);
        sendDbgData[156]= (mStepSleepParam.currentHourStepCnt & 0xff);
        uteModuleProfileBle50SendToPhone(&sendDbgData[0],157);
        memset(&sendDbgData[0],0,160);
        sendFrameCnt = 0;
        sendDbgDataCnt++;
    }
    else
    {
        sendDbgData[3+sendFrameCnt*6+0]=tempx>>8&0xff;
        sendDbgData[3+sendFrameCnt*6+1]=tempx&0xff;
        sendDbgData[3+sendFrameCnt*6+2]=tempy>>8&0xff;
        sendDbgData[3+sendFrameCnt*6+3]=tempy&0xff;
        sendDbgData[3+sendFrameCnt*6+4]=tempz>>8&0xff;
        sendDbgData[3+sendFrameCnt*6+5]=tempz&0xff;
        sendFrameCnt++;
    }
    ute_ble_connect_state_t status;
    uteApplicationCommonGetBleConnectionState(&status);
    if (!status.isConnected)
    {
        memset(&sendDbgData[0],0,160);
        sendFrameCnt = 0;
    }
}
/**
*@brief        设置gsensor采集原始数据开关
*@details
*@author       dengli.lu
*@date       2023-09-15
*/
void uteModuleSportSetAppDebugGsensorDataSwitch(bool appDebugGsensorDataSwitch)
{
    uteModuleSprotData.appDebugGsensorDataSwitch = appDebugGsensorDataSwitch;
}
/**
*@brief        获取gsensor采集原始数据开关
*@details
*@author       dengli.lu
*@date       2023-09-15
*/
bool uteModuleSportGetAppDebugGsensorDataSwitch(void)
{
    return uteModuleSprotData.appDebugGsensorDataSwitch;
}
#endif

#if UTE_MODULE_WATCH_ROTATE_SUPPORT
/**         获取旋转正在测试中
*@brief
*@details
*@author     dengli.lu
*@date       2023-09-12
*/
bool uteModuleSportGetGsensorRotateTesting(void)
{
    return uteModuleSprotData.isGsensorRotateTesting;
}
/**
* @brief        设置旋转正在测试中
* @details
* @param[in]
*@author     dengli.lu
*@date       2023-09-12
 */
void uteModuleSportSetGsensorRotateTesting(bool isGsensorRotateTesting)
{
    uteModuleSprotData.isGsensorRotateTesting = isGsensorRotateTesting;
}
/**
*@brief        获取算法中gsensor旋转次数,每500ms或者每秒获取一次
*@details
*@author       dengli.lu
*@date       2023-09-15
*/
uint16_t uteModuleSportGetGsensorRotateCnt(void)
{
    return getRotateResult();
}
/**
*@brief        清除算法中gsensor旋转次数
*@details
*@author       dengli.lu
*@date       2023-09-15
*/
void uteModuleSportClearGsensorRotateData(void)
{
    clearRotateData();
}
#endif

#if !UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_ELLIPSIS_TIMER_SUPPORT
void uteModuleSprotInputDataBeforeAlgoTimerCallback(void *pxTimer)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_SPORT_ALGO_INPUT_DATA_TIMER,0);
}
#endif

AT(.com_text.ute_sport)
void uteModuleSprotInputDataBeforeAlgoTimerHandler(void)
{
    if (vc30fx_usr_get_work_status())
    {
        return;
    }
#if UTE_MODULE_SPROT_ALGO_AUTO_SWITCH_SYSCLK_SUPPORT
    uint8_t cur_sys_clk = sys_clk_get();
    if (bsp_system_is_sleep())
    {
        if (cur_sys_clk == SYS_24M && uteModuleSprotData.switchSysclkCountdown)
        {
            sleep_set_sysclk(SYS_176M);
        }
    }
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCwmtWearStatusSwitch(uteModuleHeartIsWear()); //
#else
    uteModuleSportInputDataBeforeAlgo();
#if UTE_HEART_VCXX_NO_WEAR_NO_RUN_STEP_ALGO_SUPPORT
    if (uteModuleHeartIsWear())
    {
        uteModuleSportUnWearToWearSwitch();
    }
    else
    {
        uteModuleSportStepTypeSetNone();
    }

#endif
#endif
#if UTE_MODULE_SPROT_ALGO_AUTO_SWITCH_SYSCLK_SUPPORT
    if (bsp_system_is_sleep() && cur_sys_clk > SYS_24M)
    {
        if (uteModuleSprotData.switchSysclkCountdown == 0)
        {
            sleep_set_sysclk(SYS_24M);
        }
    }
#endif
}

/**
 * @brief        开启记步算法定时器
 * @details
 * @param[in]    ms 定时间时长
 * @author       Wang.Luo
 * @date         2023-05-05
 */
void uteModuleSportAlgoTimerStart(uint16_t ms)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,ms=%d",__func__,ms);
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_ELLIPSIS_TIMER_SUPPORT
    enableSportAlgorithmsTimer = true;
#else
    if(uteModuleSportInputDataBeforeAlgoTimer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleSportInputDataBeforeAlgoTimer, "sports input data before algo timer", 1, 400, true, uteModuleSprotInputDataBeforeAlgoTimerCallback);
    }
    uteModulePlatformRestartTimer(&uteModuleSportInputDataBeforeAlgoTimer, ms);
#endif
}
/**
 * @brief        停止记步算法定时器
 * @details
 * @author       Wang.Luo
 * @date         2023-05-05
 */
void uteModuleSprotAlgoTimerStop(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s",__func__);
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_ELLIPSIS_TIMER_SUPPORT
    enableSportAlgorithmsTimer = false;
#else
    uteModulePlatformStopTimer(&uteModuleSportInputDataBeforeAlgoTimer);
    uteModulePlatformDeleteTimer(&uteModuleSportInputDataBeforeAlgoTimer);
    uteModuleSportInputDataBeforeAlgoTimer = NULL;
#endif
}

#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_ELLIPSIS_TIMER_SUPPORT
/**
 * @brief        是否使用定时器运行运动算法
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-02-21
 */
AT(.com_text.ute_sport)
bool uteModuleSportAlgoTimerIsRunning(void)
{
    if(!uteApplicationCommonIsPowerOn())
    {
        return false;
    }
    return enableSportAlgorithmsTimer;
}
#endif

ute_module_more_sports_data_t uteModuleSportGetMoreData(void)
{
    return uteModuleSprotData.moreSportData;
}
#endif