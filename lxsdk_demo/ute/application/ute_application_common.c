/**
*@file
*@brief       application 公共层
*@details  application 公共层，链接各模块
*@author       zn.zeng
*@date       Jul 01, 2021
*@version      v1.0
*/
#include "include.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_message.h"
#include "ute_module_call.h"
#include "include.h"
#include "ute_drv_motor.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_module_notify.h"
#include "ute_module_notdisturb.h"
#include "ute_module_weather.h"
#include "ute_module_heart.h"
#include "ute_module_bloodoxygen.h"
#include "ute_drv_battery_common.h"
#include "ute_module_sleep.h"
#include "ute_module_sport.h"
#include "ute_module_findphone.h"
#include "ute_module_liftwrist.h"
#include "ute_drv_temperature_common.h"
#include "ute_module_factorytest.h"
#include "ute_module_music.h"
#include "ute_module_gui_common.h"
// #include "ute_module_menstrualcycle.h"
#include "ute_module_newFactoryTest.h"
#include "ute_module_localRingtone.h"
#include "ecig.h"
// #include "ute_module_appbinding.h"
#if 0
#include "ute_drv_keys_common.h"
#include "ute_module_bloodpressure.h"
#include "ute_drv_gsensor_common.h"
#include "ute_module_breathrate.h"
#include "ute_module_screens_common.h"
#include "ute_task_gui.h"
#include "ute_module_call.h"
#include "ute_module_ota.h"
#include "ute_module_stopwatch.h"
#include "ute_module_breathtraining.h"
#include "ute_module_countdown.h"
#include "ute_module_temperature.h"
#include "ute_module_emotionPressure.h"
#include "UTEsecurityCode.h"
#include "ute_picture_font_common.h"
#include "ute_module_lockScreen.h"
#include "ute_module_game.h"
#include "ute_module_calculator.h"
#include "ute_module_earphone.h"
#include "ute_module_quickReply.h"
#include "ute_module_gui_string.h"
#include "ute_module_ecg.h"

#if UTE_SIMPLIFY_WECHAT_SUPPORT
#include "ute_module_simplify_wechat_protocol.h"
#endif

#if UTE_MODULE_PLAYBACK_SUPPORT
#include "ute_module_micrecord.h"
#include "ute_module_earphone.h"
#endif

#if UTE_MODULE_ALI_UPAY_SUPPORT||UTE_MODULE_ALI_UPAY_V02_SUPPORT
#include "ute_module_aliUpay.h"
#endif
#if UTE_MODULE_DRINK_WATER_NOTIFY_SCREEN_SUPPORT
#include "ute_module_drinkwater.h"
#endif
#if UTE_MODULE_SHIP_MODE_SUPPORT
#include "ute_module_ship_mode.h"
#endif
#if UTE_MODULE_LOCK_WATCH_DIAL_SUPPORT
#include "ute_module_lockWatchDial.h"
#endif
#if UTE_MODULE_COMPONENTS_SUPPORT
#include "ute_module_components.h"
#endif
#include "ute_drv_tp_common.h"

#if UTE_MODULE_GPIO_EXCEPTION_WEKEUP_HANDLER_SUPPORT
#include "ute_module_gpio_exception_wakeup_handler.h"
#endif
#if UTE_MODULE_EXHIBITION_MODE_SUPPORT
#include "ute_module_exhibition.h"
#endif
#endif
/*!application 数据 zn.zeng, 2021-07-15  */
ute_application_common_data_t uteApplicationCommonData =
{
    .isSynchronizingData = false,
    .isAncsConnected = false,
};
ute_application_sync_data_param_t sendHistoryDataParam;
/* app common 互斥量 zn.zeng 2022-02-14*/
void *uteApplicationCommonMute;
void *uteApplicationCommonSyncDataTimer = NULL;
/**
*@brief  app系统启动初始化第一步
*@details  第一步只初始化屏，按键，tp，充电，adc，电池，系统时间，电量低不能真正开机
*@author        zn.zeng
*@date        Jul 01, 2021
*/
void uteApplicationCommonStartupFrist(void)
{
    uteModulePlatformCreateMutex(&uteApplicationCommonMute);
    uteModulePlatformDlpsBitReset();
    memset(&uteApplicationCommonData,0,sizeof(ute_application_common_data_t));
    uteApplicationCommonData.isStartupFristFinish = false;
    uteApplicationCommonData.isStartupSecondFinish = false;
    uteApplicationCommonData.isPowerOn = false;
    uteApplicationCommonData.isAppClosed = true;
    uteModuleFilesystemInit();
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_SYSTEMPARM_DIR);
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_LOG_DIR);
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs("./", NULL, NULL);
#endif
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SYSTEMPARM_DIR, NULL, NULL);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_LOG_DIR,NULL, NULL);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR,NULL, NULL);
#endif
    uteModuleSystemtimeInit();
    //bat
    uteDrvBatteryCommonInit();
    //按键
    //uteDrvKeysCommonInit();
#if UTE_MODULE_SHIP_MODE_SUPPORT
    uteModuleShipModeInit();
#endif
    // twi
    //uteModulePlatformTwiInit();
    /*在读取数据前清除数据，防止因修改了数据结构导致读取到异常数据出现重启、死机； jiangquan 2024-01-08*/
    uteApplicationCommonReadSoftwareVersion();
    uteApplicationCommonCheckSoftwareVersion();

    uteApplicationCommonReadConfig();
}
/**
*@brief  app系统启动初始化第二步
*@details 初始化其他硬件，更新系统参数配置，完成开机流程
*@author        zn.zeng
*@date        Jul 01, 2021
*/
void uteApplicationCommonStartupSecond(void)
{
#if UTE_MODULE_SHIP_MODE_SUPPORT
    if(uteModuleShipModeIsBeforeEnter())
    {
        if (uteDrvBatteryCommonIsDelayDisplayCharger())
        {
            uteModuleShipModeExit();
        }
        else
        {
            uteModuleShipModeEverySecond();
        }
        return;
    }
    else if(uteModuleShipModeIsShip() || uteModuleShipModeIsWakeup())
    {
        return;
    }
#endif
    if (!uteApplicationCommonData.isStartupFristFinish)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);

        uteModuleHardfaultInfoSave();

        uteApplicationCommonData.isStartupFristFinish = true;
        //其他硬件初始化
        uteDrvMotorInit();
        // uteModuleSportAlgoTimerStart(UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_TIMER_DURATION);
        //uteModulePlatformQdecInit();
#if UTE_USER_ID_FOR_BINDING_SUPPORT||UTE_MODULE_SCREENS_APP_BINDING_SUPPORT
        uteModuleAppBindingInit();
        ute_application_sn_data_t snData;
        uint32_t size = sizeof(ute_application_sn_data_t);
        uteModulePlatformFlashNorRead((uint8_t *)&snData,UTE_USER_PARAM_ADDRESS,size);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,snData.userId = 0x%08x",__func__,snData.userId);
        if(snData.userId != 0xffffffff)
        {
            uteModuleAppBindingSetHasBindingBefore(HAS_BEEN_CONNECTED);
        }
#endif
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
        uteModuleNewFactoryTestInit();
#endif

        extern void app_ute_remind_init(void);
        app_ute_remind_init();

#if UTE_MODULE_HEART_SUPPORT
        // uteModuleHeartInit();
#endif
#if UTE_MODULE_BLOODPRESSURE_SUPPORT
        uteModuleBloodpressureInit();
#endif
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
        // uteModuleBloodoxygenInit();
#endif
#if UTE_MODULE_BREATHRATE_SUPPORT
        uteModuleBreathrateInit();
#endif
#if UTE_MODULE_TEMPERATURE_SUPPORT
        uteModuleTemperatureInit();
#elif UTE_DRV_BATTERY_CE_AUTH_SUPPORT
        // uteDrvTemperatureCommonInit();
        // uteDrvTemperatureCommonPowerOff();
#else
        //uteModuleTemperatureIoInit();
#endif
#if DRV_HEART_SENSOR_TEMPERATURE_CHECK_VCXXHCI_SUPPORT
        uteModuleVcxxTempCheckInit();
#endif

#if UTE_MODULE_SLIDE_BAR_SUPPORT
        uteModuleSlideBarInit();
#endif
#if UTE_MODULE_ALI_UPAY_SUPPORT||UTE_MODULE_ALI_UPAY_V02_SUPPORT
        uteModuleAliUpayInit();
#endif
#if UTE_MODULE_GPIO_EXCEPTION_WEKEUP_HANDLER_SUPPORT
        uteModuleGpioExceptionInit();
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
        extern void uteModuleCwmInit(void);
        uteModuleCwmInit();
#endif
#if UTE_MODULE_ECG_SUPPORT
        uteModuleEcgInit();
#endif
#if UTE_MODULE_BLOODSUGAR_SUPPORT
        uteModuleBloodsugarInit();
#endif
        uteModuleFactoryTestInit();
        // uteModuleSportInit();
        uteModuleNotifyInit();
        // uteModuleSleepInit();
        uteModuleCallInit();
        //funcPasswordModuleInit();
        //uteModuleOtaInit();
        uteModuleMusicInit();
        //uteModuleStopWatchInit();
        //uteModuleCountDownInit();
        //uteModuleBreathTrainingInit();
#if UTE_MODULE_MENSTRUAL_CYCLE_SUPPORT
        uteModuleMenstrualCycleInit();
#endif
#if UTE_MODULE_PLAYBACK_SUPPORT
        uteModuleMicRecordInit();
#endif
        uteModuleFindPhoneInit();
        //uteModuleGuiStringInit();
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
        uteModuleEmotionPressureInit();
#endif
        uteModuleWeatherInit();
        uteModuleSmokeInit();
        uteModuleNotDisturbInit();
#if UTE_MODULE_DRINK_WATER_NOTIFY_SCREEN_SUPPORT
        //uteModuleDrinkWaterInit();
#endif
        // uteModuleLiftWristInit();
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
        uteModuleSportMoreSportsTargetInit();
#endif
#if UTE_MODULE_LOCK_SCREEN_SUPPORT
        uteModulePasswordInit();
        //  uteModulePasswordLoadToSysCb();
#endif
#if UTE_MODULE_GAME_SUPPORT
        uteModuleGameInit();
#endif
#if UTE_MODULE_CALCULATOR_SUPPORT
        uteModuleCalculatorInit();
#endif
#if UTE_MODULE_LOCAL_RINGTONE_VOLUME_SET
        uteModuleLocalRingtoneInit();
#endif
        //系统参数配置
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
        // uteModuleBloodoxygenReadConfig();
#endif
#if UTE_MODULE_BREATHRATE_SUPPORT
        uteModuleBreathrateReadConfig();
#endif
#if UTE_MODULE_PLAYBACK_SUPPORT
        uteModuleEarphoneInit();
#endif
#if UTE_MODUEL_QUICK_REPLY_SUPPORT
        uteModuleQuickReplyInit();
#endif
#if UTE_MODULE_LOCK_WATCH_DIAL_SUPPORT
        uteModuleLockWatchDialInit();
#endif
#if UTE_MODULE_COMPONENTS_SUPPORT
        uteModuleComponentsInit();
#endif

        uteModulePlatformCreateTimer(&uteApplicationCommonSyncDataTimer, "SYNC DATA timer", 1, UTE_SEND_DATA_TO_PHONE_INVTERVAL, false, uteApplicationCommonSyncDataTimerCallback);
        // uteModulePlatformSendMsgToAppTask(TO_APP_TASK_MSG_UPDATE_ADV_DATA,0);
        // uteModulePlatformSendMsgToAppTask(TO_APP_TASK_MSG_UPDATE_DEV_NAME,0);
        //uteModulePlatformAdvertisingInit();
#if UTE_SIMPLIFY_WECHAT_SUPPORT
        uteModuleSimplifyWechatInit();
#endif
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
        uteModuleEmotionPressureReadConfig();
#endif
        uteModuleSystemtimeRegisterSecond(uteApplicationCommonEverySecond);

        // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isDelayDisplayCharger=%d", __func__,uteDrvBatteryCommonIsDelayDisplayCharger());

        // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,HasBindingBefore=%d , HasConnectOurApp = %d", __func__,uteModuleAppBindingGetHasBindingBefore(),uteApplicationCommonIsHasConnectOurApp());
        if(0)
#if UTE_DRV_BATTERY_USE_UTE_PERCENTAGE_SUPPORT
        {}
        else if(uteDrvBatteryCommonIsDelayDisplayCharger())
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_CHARGER_ID, 0, __func__);
        }
#endif
        else if(uteDrvBatteryCommonGetLvl() >= UTE_DRV_BATTERY_AUTO_POWER_OFF_LVL || CHARGE_DC_IN()) //USB插入
        {
#if UTE_MODULE_NEW_FACTORY_TEST_JUST_CROSS_SUPPORT
            ute_new_factory_test_data_t *data;
            uteModuleNewFactoryTestSetMode(&data);
            data->mode = FACTORY_TEST_MODE_FACTORY;
            data->justDisplayCross = true;
            data->moduleType = FACTORY_MODULE_CROSS;
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_NEW_FACTORY_MODULE_ID, 0, __func__);
#elif UTE_MODULE_NEW_FACTORY_TEST_JUST_SCREEN_TP_SUPPORT
            ute_new_factory_test_data_t *data;
            uteModuleNewFactoryTestSetMode(&data);
            data->mode = FACTORY_TEST_MODE_FACTORY;
            data->justDisplayScreenTp = true;
            data->moduleType = FACTORY_MODULE_RGB;
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_NEW_FACTORY_MODULE_ID, 0, __func__);
#elif UTE_MODULE_NEW_FACTORY_TEST_JUST_HEART_CHECK_LIGHT_SUPPORT
            uteModuleNewFactoryTestInit();
            ute_new_factory_test_data_t *data;
            uteModuleNewFactoryTestSetMode(&data);
            data->mode = FACTORY_TEST_MODE_FACTORY;
            data->justDisplayHeartCheckLight = true;
            data->moduleType = FACTORY_MODULE_HEART_CHECK_LIGHT;
            uteModuleNewFactoryTestSetCheckLightMode(NEW_FACTORY_VCXX_TEST_MODE_CROSSTALK);
            uteModuleHeartStartSingleTesting(TYPE_HEART);
            uteDrvHeartVcxxStartCrosstalktest();
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_NEW_FACTORY_MODULE_ID, 0, __func__);
#else
#if UTE_MODULE_SCREENS_POWERON_SUPPORT
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_POWERON_ID, 0, __func__);
#else
            if(0)
            {}
#if UTE_MODULE_POWERON_LANGUAGE_SELECT_SUPPORT
            else if((uteModuleAppBindingGetHasBindingBefore()!=HAS_BEEN_CONNECTED)&&(!uteApplicationCommonIsHasConnectOurApp()))
            {
#if UTE_MODULE_SCREENS_POWER_ON_LANGUAGE_SELECT_SUPPORT
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_POWER_ON_LANGUAGE_SELECT_ID, 0, __func__);
#else
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_LANGUAGE_SELECT_LIST_ID, 0, __func__);
#endif
            }
#endif
#if (UTE_MODULE_SCREENS_APP_BINDING_SUPPORT)//如果没有开机界面，在这里跳转至绑定界面 dengli.lu 2022-02-28
            else if(uteModuleAppBindingGetHasBindingBefore()!=HAS_BEEN_CONNECTED)
            {
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_APP_BINDING_ID, 0, __func__);
            }
#endif
            else
            {
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
            }
#endif
#endif
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,vbat is too low!!",__func__);
            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_START_POWER_OFF,0);
        }
#if UTE_MODULE_MOTOR_POWER_STATUS_SAVE_SUPPORT
        if(!uteModuleMotorGetVibrationSwitchStatus())
        {
            uteModuleMotorSetIsOpenVibrationStatus(true);
        }
#endif

        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        uteApplicationCommonData.isStartupSecondFinish = true;
        uteApplicationCommonData.isPowerOn = true;
        uteApplicationCommonData.systemPowerOnSecond = 0;
        uteApplicationCommonData.mtuSize = 20;
        //ecig_set_power(15);
        //按键
        // uteDrvKeysCommonInit();
#if SET_THE_LOW_BATTERY_POPUP_TIME
        uteApplicationCommonData.lowBatterySecond=0;//hcj add 2022/9/8
#endif
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,vbat is too low!!", __func__);
    }
}
/**
*@brief  获取上电状态
*@details
*@return 返回true 表示上电完成
*@author        zn.zeng
*@date        Jul 01, 2021
*/
bool uteApplicationCommonIsStartupFinish(void)
{
    return uteApplicationCommonData.isStartupSecondFinish;
}
/**
*@brief  获取开机状态
*@details
*@return 返回true 表示当前是开机状态
*@author        zn.zeng
*@date        2021-07-15
*/
AT(.com_text.ute_app)
bool uteApplicationCommonIsPowerOn(void)
{
    return uteApplicationCommonData.isPowerOn;
}
/**
*@brief  获取个人信息
*@details
*@author        zn.zeng
*@date        2021-07-20
*/
void uteApplicationCommonGetPersonalInfo(ute_personal_info_t *info)
{
    uteModulePlatformTakeMutex(uteApplicationCommonMute);
    memcpy(info,&uteApplicationCommonData.personalInfo,sizeof(ute_personal_info_t));
    uteModulePlatformGiveMutex(uteApplicationCommonMute);
}
/**
*@brief    更新ble连接状态
*@details  接入平台
*@author        zn.zeng
*@date        2021-08-05
*/
void uteApplicationCommonSetBleConnectState(uint8_t connid,bool isConnected)
{
    // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,connectId=%d,isConnected=%d", __func__,connid,isConnected);
    uteApplicationCommonData.bleConnectState.connId = connid;
    uteApplicationCommonData.bleConnectState.isConnected = isConnected;
    uteApplicationCommonData.bleConnectState.connectedSecond = 0;
    uteApplicationCommonData.bleConnectState.isParired = false;
    uteModuleNotifyAncsTimerConnectHandler(isConnected);
    if(!isConnected) /* ellison add ,2022-Jun-15 断开以后回到20 byte 兼容部分手机同步表盘失败*/
    {
        uteApplicationCommonSetMtuSize(20);
#if UTE_MODULE_SCREENS_APP_BINDING_SUPPORT || UTE_USER_ID_FOR_BINDING_SUPPORT || UTE_MODULE_SCREENS_POWER_ON_LANGUAGE_SELECT_SUPPORT // dengli.lu, 2022-03-06
#if (!UTE_APP_BINDING_ONLY_ONCENOTIFY_SUPPORT)
        uteModuleAppBindingSetBindingNotify(false);
#endif
        uteModuleAppBindingSetOurAppConnection(false);
#endif
#if UTE_USER_ID_FOR_BINDING_SUPPORT
        uteModuleAppBindingSetBindingStart(false);
        uteModuleAppBindingSetBindingSecCnt(0);
#endif
    }
    else
    {
#if UTE_MODULE_SCREENS_APP_BINDING_SUPPORT || UTE_USER_ID_FOR_BINDING_SUPPORT || UTE_MODULE_SCREENS_POWER_ON_LANGUAGE_SELECT_SUPPORT // dengli.lu, 2022-03-06
        uteModuleAppBindingSetOurAppConnection(false);
#if (!UTE_APP_BINDING_ONLY_ONCENOTIFY_SUPPORT)
        uteModuleAppBindingSetBindingNotify(false);
#endif
        uteModuleAppBindingSetBindingStart(false);
        uteModuleAppBindingSetBindingSecCnt(0);
#endif
    }

    if(!uteApplicationCommonData.bleConnectState.isConnected)
    {
#if UTE_MODULE_SPORT_SUPPORT
        uteModuleSportSetTakePictureEnable(false);
#endif
        if(!uteApplicationCommonData.isPowerOn)
        {
            // uteModulePlatformSetFastAdvertisingTimeCnt(0);
            uteModulePlatformSendMsgToAppTask(TO_APP_TASK_MSG_STOP_ADV,0);
        }
        // uteModuleOtaDisconectHandler();
        // uteModuleFactoryTestDisconnectHandler();
        // uteApplicationCommonSetHasUpdateConnectionParam(false);
#if UTE_MODULE_SPORT_SUPPORT
        uteModuleSportDisconnectHandler();
#endif
        // if(!uteModulePlatformGetFastAdvertisingTimeCnt())
        // {
        //     uteModulePlatformSetFastAdvertisingTimeCnt(UTE_FAST_ADVERTISE_TIMEOUT);
        // }
#if UTE_SIMPLIFY_WECHAT_SUPPORT
        uteModuleSimplifyWechatSetRunningStatus(false);
        uteModuleSimplifyWechatSetUpdateMeasureDataStatus(false);
        uteModuleSimplifyWechatSetSecCounter(0);
#endif
        uteApplicationCommonData.isSynchronizingData = false;
        uteApplicationCommonData.mtuSize = 20;
#if UTE_MODUEL_QUICK_REPLY_SUPPORT
        uteModuleQuickReplySetStatus(false);
#endif
#if (UTE_MODULE_CYWEE_MOTION_SUPPORT&&(UTE_MODULE_LOG_SUPPORT && UTE_MODULE_RUNING_LOG_SUPPORT))
        uteModuleLogSetSendRuningLogSwitch(false);
#endif
#if BT_ID3_TAG_EN
        if(!bt_a2dp_profile_completely_connected())
        {
            uteModuleMusicResetPlayStatus();
        }
#else
        uteModuleMusicResetPlayStatus();
#endif
    }
    else
    {
        // uteModulePlatformSetFastAdvertisingTimeCnt(0);
        // uteModulePlaformUpdateConnectParam(12,36,55000);
        ble_update_conn_param(12,0,500);
    }
    uteModuleCallBleConnectState(isConnected);
}
/**
*@brief    更新ble配对状态
*@details  接入平台
*@author        zn.zeng
*@date        2021-11-17
*/
void uteApplicationCommonSetBlePariring(bool isSuccess)
{
    uteApplicationCommonData.bleConnectState.isParired = isSuccess;
    uint8_t response[2] = {CMD_SET_ANCS_PARAM,0x01};
    if(!isSuccess)
    {
        response[1] = 0x02;
    }
    uteModuleProfileBleSendToPhone(&response[0],2);

}

#if UTE_MODULE_LOCAL_SET_LIFT_WRIST_SUPPORT&&UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
/**
*@brief  进出定时勿扰的抬腕亮屏处理
*@details
*@return
*@author        raymond
*@date        2022-03-16
*/
void uteApplicationCommonNotDisturbEnterOrExitHandler(ute_module_systemtime_time_t time)
{

    uint16_t hourMin = time.hour<<8|time.min;
    uint16_t startHourMin = 0,endHourMin = 0;
    startHourMin = uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME);
    endHourMin = uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME);
    if(uteModuleNotDisturbIsOpenScheduled()&&uteModuleNotDisturbIsTimeBucket())
    {
        if(hourMin==startHourMin&&time.sec==0) //enter
        {
            uteModuleLiftWristRecordOpenStatus();
            if(uteModuleLiftWristGetOldOpenStatus())
            {
                uteModuleLiftWristSetOpenStatus(false);
            }
            else if(uteModuleLiftWristGetOldScheduledOpenStatus())
            {
                uteModuleLiftWristSetScheduled(false);
            }
        }
        else if(hourMin==endHourMin&&time.sec==59) //exit
        {
            if(uteModuleLiftWristGetOldOpenStatus())
            {
                uteModuleLiftWristSetOpenStatus(true);
            }
            else if(uteModuleLiftWristGetOldScheduledOpenStatus())
            {
                uteModuleLiftWristSetScheduled(true);
            }
        }
    }
}
#endif

/**
*@brief        app每秒函数
*@details       需要注册到主时间，每秒执行一次
*@author       zn.zeng
*@date       2021-08-05
*/
void uteApplicationCommonEverySecond(void)
{
#if UTE_DRV_HEART_LC11S_SUPPORT_RH303F_FOUR_LIGHTS
    if((((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_BLOOD_OXYGEN_ID)&&uteModuleBloodoxygenIsWear())||((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_HEART_RATE_ID)&&uteModuleHeartIsWear()))&&
       uteModuleGuiCommonIsDisplayOn())
    {
        uteModulePlatformOutputGpioSet(P0_0,true);
    }
    else
    {
        uteModulePlatformOutputGpioSet(P0_0,false);
    }
#endif
#if UTE_USER_ID_FOR_BINDING_SUPPORT||UTE_MODULE_SCREENS_APP_BINDING_SUPPORT||UTE_MODULE_SCREENS_POWER_ON_LANGUAGE_SELECT_SUPPORT
    if(uteApplicationCommonData.bleConnectState.isConnected)
    {
#if DUG_VCXX_HEART_SUPPORT
        uteModuleAppBindingSetOurAppConnection(true);
#endif
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,GetOurAppConnection = %d,GetBindingSecCnt = %d", __func__, uteModuleAppBindingGetOurAppConnection(),uteModuleAppBindingGetBindingSecCnt());

        if(!uteModuleAppBindingGetOurAppConnection())
        {
            if(uteModuleAppBindingGetBindingSecCnt()==UTE_VERIFY_OUR_APP_APK_DEFUALT_TIMEOUT)
            {
#if UTE_USER_ID_FOR_BINDING_SUPPORT
                uteModulePlatformBleDisconnect(uteApplicationCommonData.bleConnectState.connId);
#endif
                if(uteModuleAppBindingGetHasBindingBefore()==HAS_BEEN_CONNECTED)
                {
                    uteModuleGuiCommonGoBackLastScreen();
                }
                else
                {
                    uteModuleAppBindingSetBindingStart(false);
#if UTE_MODULE_SCREENS_APP_BINDING_NOTIFY_SUPPORT
                    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_APP_BINDING_NOTIFY_ID);
#elif UTE_MODULE_SCREENS_APP_BINDING_SUPPORT
                    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_APP_BINDING_ID, 0, __func__);
#endif
                }
                uint8_t bindingSecCnt = uteModuleAppBindingGetBindingSecCnt();
                uteModuleAppBindingSetBindingSecCnt(++bindingSecCnt);
            }
            else
            {
                uint8_t bindingSecCnt = uteModuleAppBindingGetBindingSecCnt();
                uteModuleAppBindingSetBindingSecCnt(++bindingSecCnt);
            }
        }
        else
        {
            uteModuleAppBindingSetBindingSecCnt(0);
        }
    }
#endif

//    uteModulePlatformOutputGpioSet(UTE_MODULE_NAND_FLASH_POWER_GPIO_PIN,UTE_MODULE_NAND_FLASH_POWER_ACTION_LEVEL);
    if(uteApplicationCommonData.bleConnectState.isConnected)
    {
        uteApplicationCommonData.bleConnectState.connectedSecond++;
    }
    uteApplicationCommonData.systemPowerOnSecond++;
    // printf("%s,sys_clk_get=%d",__func__,sys_clk_get());

#if ASR_SELECT
    uteModuleAsrModeEverySecond();
#endif

#if SET_THE_LOW_BATTERY_POPUP_TIME
    uteApplicationCommonData.lowBatterySecond++;//hcj add 2022/09/08
#endif
//    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,fastAdvertisingTimeCnt = %d", __func__, uteModulePlatformGetFastAdvertisingTimeCnt());

    /*! dengli.lu, 2021-12-06  当前没有快速广播时开启快广播*/
    // if(uteModulePlatformGetFastAdvertisingTimeCnt())
    // {
    //     uteModulePlatformSetFastAdvertisingTimeCnt(uteModulePlatformGetFastAdvertisingTimeCnt()-1);
    //     if(!uteApplicationCommonData.bleConnectState.isConnected)
    //     {
    //         if(uteModulePlatformGetFastAdvertisingTimeCnt() == (UTE_FAST_ADVERTISE_TIMEOUT-2))
    //         {
    //             uteModulePlatformSendMsgToAppTask(TO_APP_TASK_MSG_START_FAST_ADV,0);
    //         }
    //         if(uteModulePlatformGetFastAdvertisingTimeCnt() == 1)
    //         {
    //             uteModulePlatformSetFastAdvertisingTimeCnt(0);
    //             uteModulePlatformSendMsgToAppTask(TO_APP_TASK_MSG_START_SLOW_ADV,0);
    //         }
    //     }
    // }
#if UTE_SIMPLIFY_WECHAT_SUPPORT
    uteModuleSimplifyWechatSendPort();
#endif

#if (UTE_LOG_TIME_LVL&&UTE_MODULE_LOG_SUPPORT)
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,%04d-%02d-%02d %02d:%02d:%02d", __func__, time.year,time.month,time.day,time.hour,time.min,time.sec);
#endif
#if UTE_MODULE_LOCAL_SET_LIFT_WRIST_SUPPORT&&UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if(time.hour==0&&time.min==0&&time.sec==0)
    {
        ute_quick_switch_t quick;
        uteApplicationCommonGetQuickSwitchStatus(&quick);
        if(quick.isNotDisturb)
        {
            quick.isNotDisturb = false;
            uteApplicationCommonSetQuickSwitchStatus(quick);
        }
        if(uteModuleLiftWristIsOpen())
        {
            uteModuleLiftWristSetOpenStatus(false);
        }

    }
    uteApplicationCommonNotDisturbEnterOrExitHandler(time);
#endif
#if UTE_MODULE_SHIP_MODE_SUPPORT
    uteModuleShipModeEverySecond();
#endif
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    // uteModuleNotDisturbCleanHandOnStatus();
#endif
}
/**
*@brief        获取ble连接状态
*@details
*@param[out] ute_ble_connect_state_t 指针
*@author       zn.zeng
*@date       2021-08-05
*/
void uteApplicationCommonGetBleConnectionState(ute_ble_connect_state_t *state)
{
    memcpy(state,&uteApplicationCommonData.bleConnectState,sizeof(ute_ble_connect_state_t));
}
/**
*@brief    读取配置文件
*@details  只需要在开机的时候执行一次即可
*@author        zn.zeng
*@date        2021-08-18
*/
void uteApplicationCommonReadConfig(void)
{
    uint8_t readbuff[10*SYSTEM_TIME_ALARMS_MAX_CNT];
    // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,start time=%d", __func__,uteModulePlatformGetSystemTick());
    void *file;
    /*! 个人信息 zn.zeng, 2021-08-19  */
    readbuff[0] = DEFAULT_PERSONALINFO_AGE;
    readbuff[1] = DEFAULT_PERSONALINFO_HIGHT;
    readbuff[2] = DEFAULT_PERSONALINFO_ISMAN;
    readbuff[3] = (DEFAULT_PERSONALINFO_WEIGHT>>24)&0xff;
    readbuff[4] = (DEFAULT_PERSONALINFO_WEIGHT>>16)&0xff;
    readbuff[5] = (DEFAULT_PERSONALINFO_WEIGHT>>8)&0xff;
    readbuff[6] = DEFAULT_PERSONALINFO_WEIGHT&0xff;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_PERSONALINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],7);
        uteModuleFilesystemCloseFile(file);
    }
    uteApplicationCommonData.personalInfo.age = readbuff[0];
    uteApplicationCommonData.personalInfo.height = readbuff[1];
    uteApplicationCommonData.personalInfo.isMan = readbuff[2];
    uteApplicationCommonData.personalInfo.weight = readbuff[3]<<24|readbuff[4]<<16|readbuff[5]<<8|readbuff[6];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,age=%d,height=%d,isMan=%d,weight=%d", __func__,uteApplicationCommonData.personalInfo.age,uteApplicationCommonData.personalInfo.height,uteApplicationCommonData.personalInfo.isMan,uteApplicationCommonData.personalInfo.weight);
    /*! 快捷开关状态zn.zeng, 2021-08-24  */
    memset(&readbuff[0],0,12);

    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_QUICK_SWITCHINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],12);
        uteModuleFilesystemCloseFile(file);
    }
    uteApplicationCommonData.quickSwitch.isFindband=readbuff[0];
    uteApplicationCommonData.quickSwitch.isTurnTheWrist=readbuff[1];
    //uteApplicationCommonData.quickSwitch.isSedentary=readbuff[2];
    uteApplicationCommonData.quickSwitch.isNotDisturb=readbuff[3];
    uteApplicationCommonData.quickSwitch.isFindPhone=readbuff[4];
    uteApplicationCommonData.quickSwitch.isRejectCall=readbuff[5];
    uteApplicationCommonData.quickSwitch.isHeart24h=readbuff[6];
    uteApplicationCommonData.quickSwitch.isDisplayTime=readbuff[7];
    uteApplicationCommonData.quickSwitch.isShockTime=readbuff[8];
    uteApplicationCommonData.quickSwitch.isGoalReach=readbuff[9];
    //uteApplicationCommonData.quickSwitch.isDrinkWater=readbuff[10];
    //uteApplicationCommonData.quickSwitch.isHrAbnormalWarnning=readbuff[11];

    /* 连接标志 zn.zeng 2022-01-26*/
    memset(&readbuff[0],0,1);
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HAS_CONNECT_OUR_APP,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteApplicationCommonData.isHasConnectOurApp = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isHasConnectOurApp=%d", __func__,uteApplicationCommonData.isHasConnectOurApp);
    // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,end time=%d", __func__,uteModulePlatformGetSystemTick());
}

/**
*@brief  保存个人信息
*@details
*@param[in] ute_personal_info_t personalInfo
*@author        zn.zeng
*@date        2021-08-19
*/
void uteApplicationCommonSavePersonalInfo(ute_personal_info_t personalInfo)
{
    /*! 保存到文件zn.zeng, 2021-08-19  */
    void *file;
    uint8_t writebuff[7];
    uteModulePlatformTakeMutex(uteApplicationCommonMute);
    memcpy(&uteApplicationCommonData.personalInfo,&personalInfo,sizeof(ute_personal_info_t));
    uteModulePlatformGiveMutex(uteApplicationCommonMute);
    writebuff[0] = uteApplicationCommonData.personalInfo.age;
    writebuff[1] = uteApplicationCommonData.personalInfo.height;
    writebuff[2] = uteApplicationCommonData.personalInfo.isMan;
    writebuff[3] = (uteApplicationCommonData.personalInfo.weight>>24)&0xff;
    writebuff[4] = (uteApplicationCommonData.personalInfo.weight>>16)&0xff;
    writebuff[5] = (uteApplicationCommonData.personalInfo.weight>>8)&0xff;
    writebuff[6] = uteApplicationCommonData.personalInfo.weight&0xff;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_PERSONALINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],7);
        uteModuleFilesystemCloseFile(file);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,age=%d,height=%d,isMan=%d,weight=%d", __func__,uteApplicationCommonData.personalInfo.age,uteApplicationCommonData.personalInfo.height,uteApplicationCommonData.personalInfo.isMan,uteApplicationCommonData.personalInfo.weight);
    }
}
/**
*@brief  保存快捷开关状态信息
*@details
*@author        zn.zeng
*@date        2021-08-24
*/
void uteApplicationCommonSaveQuickSwitchInfo(void)
{
    printf("KSKSKSKSKS%s,len: %d\n",__func__,__LINE__);
    /*! 保存到文件zn.zeng, 2021-08-24*/
    void *file;
    uint8_t writebuff[12];
    writebuff[0] = uteApplicationCommonData.quickSwitch.isFindband;
    writebuff[1] = uteApplicationCommonData.quickSwitch.isTurnTheWrist;
    //writebuff[2] = uteApplicationCommonData.quickSwitch.isSedentary;
    writebuff[3] = uteApplicationCommonData.quickSwitch.isNotDisturb;
    writebuff[4] = uteApplicationCommonData.quickSwitch.isFindPhone;
    writebuff[5] = uteApplicationCommonData.quickSwitch.isRejectCall;
    writebuff[6] = uteApplicationCommonData.quickSwitch.isHeart24h;
    writebuff[7] = uteApplicationCommonData.quickSwitch.isDisplayTime;
    writebuff[8] = uteApplicationCommonData.quickSwitch.isShockTime;
    writebuff[9] = uteApplicationCommonData.quickSwitch.isGoalReach;
    //writebuff[10] = uteApplicationCommonData.quickSwitch.isDrinkWater;
    //writebuff[11] = uteApplicationCommonData.quickSwitch.isHrAbnormalWarnning;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_QUICK_SWITCHINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],12);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief  开始关机
*@details
*@author        zn.zeng
*@date        2021-11-08
*/
void uteApplicationCommonStartPowerOffMsg(void)
{
    if (uteDrvBatteryCommonGetChargerStatus() == BAT_STATUS_CHARGING)
    {
        uteModulePlatformSystemReboot();
        return;
    }
    // uteModuleCountDownStop();
    // uteModuleLocalRingtoneSaveData();//关机的时候保存一次本地铃声配置
#if UTE_MODULE_SCREENS_POWEROFF_SUPPORT
    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_POWEROFF_ID, 0, __func__);
#else
    if(!sys_cb.gui_sleep_sta)
    {
        gui_sleep();
    }
    uteApplicationCommonData.isPowerOn = false;
#endif
    uteApplicationCommonSaveQuickSwitchInfo();
    uteModuleWeatherSaveData();
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportSaveStepData();
#endif
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT&&UTE_MODULE_NOT_DISTURB_POWER_OFF_SAVE_STATUS_SUPPORT
    ute_module_not_disturb_data_t param;
    uteModuleNotDisturbGetParam(&param);
    uteModuleNotDisturbSaveParam(param);
#endif
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
    uteModuleCallEntertranmentVoiceSwitchSaveConfig();
#endif
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
    uteModuleSportSaveTodayTargetNotifyData();
#endif
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
    uteModuleSportSaveTodayEveryHourAllSportStepData();
    uteModuleSportSaveTodayEveryHourAllSportKcalData();
#endif
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isConnected=%d", __func__,uteApplicationCommonData.bleConnectState.isConnected);

    if(uteApplicationCommonData.bleConnectState.isConnected)
    {
//        uteModulePlatformBleDisconnect(uteApplicationCommonData.bleConnectState.connId);
    }
    else
    {
        // uteModulePlatformSendMsgToAppTask(TO_APP_TASK_MSG_STOP_ADV,0);
    }
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartPowerOff();
#endif
    // uteModulePlatformQdecPowerOff();
    // uteModulePlatformPowerOffGpioConfig();
#if UTE_BT30_CALL_SUPPORT
#if UTE_MODULE_BT_POWER_STATUS_SAVE_SUPPORT
    uteModuleCallSaveBtPowerOnOffStatus();
#endif
#if UTE_BT30_AUTO_POWRER_OFF_SUPPORT
    uteModuleCallIsBtAutoCloseSaveConfig();
#endif
    uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_SYSTEM_OFF);
#endif
    // uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);

    if(uteDrvBatteryCommonGetLvl() >= 10)
    {
        uteDrvMotorEnable();
        delay_5ms(UTE_MOTOR_DURATION_TIME / 5);
        uteDrvMotorDisable();
    }

    // uteModulePlatformSetFastAdvertisingTimeCnt(0);

#if UTE_MODULE_SHIP_MODE_SUPPORT
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
    uteModuleNewFactoryTestShipModeEnter();
#endif
#if UTE_MODULE_SHIP_MODE_POWER_OFF_SUPPORT
    uteDrvKeysGpioIntDisable(); /*关闭非电源按键中断,xjc 2022-11-23*/
#endif
#endif
    // uteModuleFactoryTestStop();
    // uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_SCREEN|UTE_MODULE_PLATFORM_DLPS_BIT_MOTOR|UTE_MODULE_PLATFORM_DLPS_BIT_KEYS|UTE_MODULE_PLATFORM_DLPS_BIT_UART);

#if !UTE_MODULE_SCREENS_POWEROFF_SUPPORT
//    uteModuleGuiCommonDisplayOff(true);
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_REAL_POWER_OFF,0);
#endif
}

/**
*@brief  真正关机
*@details
*@author        zn.zeng
*@date        2021-08-20
*/
void uteApplicationCommonRealPowerOffMsg(void)
{
    uteApplicationCommonData.isPowerOn = false;
    func_pwroff(1);
}
/**
*@brief  开机
*@details
*@author        zn.zeng
*@date        2021-11-08
*/
void uteApplicationCommonPowerOn(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
    uteModuleSystemtimeSaveTimeInfo();
    uteModulePlatformSystemReboot();
}

/**
*@brief   同步数据定时器回调函数
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void uteApplicationCommonSyncDataTimerCallback(void *pxTimer)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYNC_DATA_TIMER,0);
}
/**
*@brief   注册同步数据定时器回调函数
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void uteApplicationCommonRegisterSyncDataTimerFunction(void *function)
{
    uteApplicationCommonData.syncDataFunction = (ute_module_sync_data_reg_func_t)function;
}
/**
*@brief   开始同步数据
*@details
*@author        zn.zeng
*@date        2022-04-15
*/
void uteApplicationCommonSyncDataTimerStart(void)
{
    uteModulePlatformRestartTimer(&uteApplicationCommonSyncDataTimer, UTE_SEND_DATA_TO_PHONE_INVTERVAL);
    uteApplicationCommonData.isSyncTimerRunning = true;
    uteApplicationCommonData.isSynchronizingData = true;
}
/**
*@brief   停止同步数据
*@details
*@author        zn.zeng
*@date        2022-04-15
*/
void uteApplicationCommonSyncDataTimerStop(void)
{
    uteModulePlatformStopTimer(&uteApplicationCommonSyncDataTimer);
    uteApplicationCommonData.isSyncTimerRunning = false;
    uteApplicationCommonData.isSynchronizingData = false;
}
/**
*@brief   同步数据定时器msg处理
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void uteApplicationCommonSyncDataTimerMsg(void)
{
    if(!uteApplicationCommonData.bleConnectState.isConnected)
    {
        uteModulePlatformStopTimer(&uteApplicationCommonSyncDataTimer);
        uteApplicationCommonData.isSynchronizingData = false;
        return;
    }
    if (is_le_buff_full(2))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,le_buff_full!!!",__func__);
        uteModulePlatformRestartTimer(&uteApplicationCommonSyncDataTimer, UTE_SEND_DATA_TO_PHONE_INVTERVAL);
        return;
    }
    if(!uteApplicationCommonData.isSyncTimerRunning)
    {
        return;
    }
    uteApplicationCommonData.isSynchronizingData = true;
    uteApplicationCommonData.syncDataFunction();
    if (uteApplicationCommonData.isSyncTimerRunning)
    {
        uteModulePlatformRestartTimer(&uteApplicationCommonSyncDataTimer, UTE_SEND_DATA_TO_PHONE_INVTERVAL);
    }
}
/**
*@brief    发送快捷开关状态值
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void uteApplicationCommonSendQuickSwitchStatus(void)
{
    uint8_t response[20];
    uint32_t setFlag=0;
    memset(&response[0],0x00,20);
    response[0] = CMD_QUICK_SWITCH;
    response[1] = 0x02;
    if(uteApplicationCommonData.quickSwitch.isFindband)
    {
        setFlag|=QUICK_SWITCH_FINDBAND;
    }
    if(uteApplicationCommonData.quickSwitch.isTurnTheWrist)
    {
        setFlag|=QUICK_SWITCH_TURNTHEWRIST;
    }
    // if(uteModuleSportSedentaryOpenCtrl(0,0))
    // {
    //     setFlag|=QUICK_SWITCH_SEDENTARY;
    // }
    if(uteApplicationCommonData.quickSwitch.isNotDisturb)
    {
        setFlag|=QUICK_SWITCH_NOT_DISTURB;
    }
    if(uteApplicationCommonData.quickSwitch.isFindPhone)
    {
        setFlag|=QUICK_SWITCH_ANTILOST;
    }
    if(uteApplicationCommonData.quickSwitch.isRejectCall)
    {
        setFlag|=QUICK_SWITCH_REJECT_CALL;
    }
    if(uteApplicationCommonData.quickSwitch.isHeart24h)
    {
        setFlag|=QUICK_SWITCH_24H_HEART;
    }
    if(uteApplicationCommonData.quickSwitch.isDisplayTime)
    {
        setFlag|=QUICK_SWITCH_DISPLAY_TIME;
    }
    if(uteApplicationCommonData.quickSwitch.isShockTime)
    {
        setFlag|=QUICK_SWITCH_SHOCK_TIME;
    }
    if(uteApplicationCommonData.quickSwitch.isGoalReach)
    {
        setFlag|=QUICK_SWITCH_GOAL_REACH;
    }
    // if(uteModuleDrinkWaterOpenCtrl(0,0))
    // {
    //     setFlag|=QUICK_SWITCH_DRINK_WATER;
    // }
    // if(uteModuleHeartWaringOpenCtrl(0,0))
    // {
    //     setFlag|=QUICK_SWITCH_HR_ABNORMAL_WARNING;
    // }
#if QUICK_SWITCH_LOCAL_IS24HOUR_SUPPORT
    if(uteModuleSystemtime12HOn()==false)
    {
        setFlag|=QUICK_SWITCH_LOCAL_IS24HOUR;
    }
#endif
    response[2] = (setFlag>>24)&0xff;
    response[3] = (setFlag>>16)&0xff;
    response[4] = (setFlag>>8)&0xff;
    response[5] = (setFlag)&0xff;
    uteModuleProfileBleSendToPhone(&response[0],20);
}
/**
*@brief   设置快捷开关状态
*@details
*@author       dengli.lu
*@date       2021-12-11
*/
void uteApplicationCommonSetQuickSwitchStatus(ute_quick_switch_t *quickSwitch)
{
    uteModulePlatformTakeMutex(uteApplicationCommonMute);
    memcpy(&uteApplicationCommonData.quickSwitch, quickSwitch, sizeof(ute_quick_switch_t));
    uteModulePlatformGiveMutex(uteApplicationCommonMute);
    uteApplicationCommonSaveQuickSwitchInfo();
}
/**
*@brief   APP设置快捷开关状态
*@details
*@author       cxd
*@date       2022-08-29
*/
void uteApplicationCommonSetQuickSwitchStatusFromApp(uint8_t *pData)
{
    uint32_t setFlag=0;

    for(uint8_t i=0; i<4; i++)
    {
        setFlag <<= 8;
        setFlag |= pData[i];
    }

    uteApplicationCommonData.quickSwitch.isFindband = (setFlag&QUICK_SWITCH_FINDBAND)?1:0;
    uteApplicationCommonData.quickSwitch.isTurnTheWrist = (setFlag&QUICK_SWITCH_TURNTHEWRIST)?1:0;
    //uteApplicationCommonData.quickSwitch.is = (setFlag&QUICK_SWITCH_SEDENTARY)?1:0;

    // uteModuleSportSedentaryOpenCtrl(1,(setFlag&QUICK_SWITCH_SEDENTARY)?1:0);

    uteApplicationCommonData.quickSwitch.isNotDisturb = (setFlag&QUICK_SWITCH_NOT_DISTURB)?1:0;
    uteApplicationCommonData.quickSwitch.isFindPhone = (setFlag&QUICK_SWITCH_ANTILOST)?1:0;
    uteApplicationCommonData.quickSwitch.isRejectCall = (setFlag&QUICK_SWITCH_REJECT_CALL)?1:0;
    uteApplicationCommonData.quickSwitch.isHeart24h = (setFlag&QUICK_SWITCH_24H_HEART)?1:0;
    uteApplicationCommonData.quickSwitch.isFindband = (setFlag&QUICK_SWITCH_DISPLAY_TIME)?1:0;
    uteApplicationCommonData.quickSwitch.isFindband = (setFlag&QUICK_SWITCH_SHOCK_TIME)?1:0;
    uteApplicationCommonData.quickSwitch.isFindband = (setFlag&QUICK_SWITCH_NOT_DISTURB_MODE)?1:0;
    uteApplicationCommonData.quickSwitch.isGoalReach = (setFlag&QUICK_SWITCH_GOAL_REACH)?1:0;
    //uteApplicationCommonData.quickSwitch.isDrinkWater = (setFlag&QUICK_SWITCH_DRINK_WATER)?1:0;
    // uteModuleDrinkWaterOpenCtrl(1,(setFlag&QUICK_SWITCH_DRINK_WATER)?1:0);
    // uteApplicationCommonData.quickSwitch.isHrAbnormalWarnning = (setFlag&QUICK_SWITCH_HR_ABNORMAL_WARNING)?1:0;
    // uteModuleHeartWaringOpenCtrl(1,(setFlag&QUICK_SWITCH_HR_ABNORMAL_WARNING)?1:0);
    uteApplicationCommonSaveQuickSwitchInfo();
}


/**
*@brief    获取快捷开关状态指针
*@details
*@author        zn.zeng
*@date        2021-08-25
*/
void uteApplicationCommonGetQuickSwitchStatus(ute_quick_switch_t *quickSwitch)
{
    uteModulePlatformTakeMutex(uteApplicationCommonMute);
    memcpy(quickSwitch, &uteApplicationCommonData.quickSwitch, sizeof(ute_quick_switch_t));
    uteModulePlatformGiveMutex(uteApplicationCommonMute);
}
/**
*@brief    获取发送历史参数数据指针
*@details
*@param[out]
*@author        zn.zeng
*@date        2021-08-25
*/
void uteApplicationCommonGetSyncDataParam(ute_application_sync_data_param_t **param)
{
    *param = &sendHistoryDataParam;
}
/**
*@brief   按键事件处理
*@details
*@author        zn.zeng
*@date        2021-07-09
*/
void uteApplicationCommonKeysPressEvent(uint32_t param)
{
#if 0
    bool isLongPress;
    bool isDoublePress;
    uint8_t keyCode =param&0xff;
    if(param&UTE_DRV_KEYS_LONG_PRESS_FLAG)
    {
        isLongPress = true;
#if UTE_FACTORY_TEST_AUDIO_RF_MODE_SUPPORT
        ute_new_factory_test_data_t *data;
        uteModuleNewFactoryTestSetMode(&data);
        if(data->mode == FACTORY_TEST_MODE_RF_DUT)
        {
            uteModulePlatformSystemReboot();
        }
#endif
    }
    else
    {
        isLongPress = false;
    }
    if(param&UTE_DRV_KEYS_DOUBLE_PRESS_FLAG)
    {
        isDoublePress = true;
    }
    else
    {
        isDoublePress = false;
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isLongPress=%d,isDoublePress=%d,keyCode=0x%02x", __func__,isLongPress,isDoublePress,keyCode);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isPowerOn=%d,systemPowerOnSecond=%d", __func__,uteApplicationCommonData.isPowerOn,uteApplicationCommonData.systemPowerOnSecond);
#if UTE_MODULE_SHIP_MODE_SUPPORT
    if(uteModuleShipModeIsWakeup())
    {
#if UTE_MODULE_KEYS_LONG_PRESS_EXIT_SHIP_MODE_SUPPORT
        if(isLongPress)
        {
            uteModuleShipModeExit();
        }
        else
#endif
        {
            uteModuleShipModeEnter();
        }
        return;
    }
#endif
    uteModuleFactoryTestKeysHandler(keyCode);
    if(uteApplicationCommonData.isPowerOn)
    {
        if (uteApplicationCommonData.systemPowerOnSecond > 2)
        {
            ute_task_gui_message_t msg;
            msg.type = MSG_TYPE_DRV_KEYS_REPORT;
            msg.u.keyReprot.keyCode = keyCode;
            msg.u.keyReprot.qDecodeAxisDirection = UTE_DRV_KEY_ROLL_INVALIDE;
            if (isLongPress)
            {
                msg.u.keyReprot.keyType = UTE_DRV_KEY_LONG_PRESS;
            }
            else if (isDoublePress)
            {
                msg.u.keyReprot.keyType = UTE_DRV_KEY_DOUBLE_PRESS;
            }
            else
            {
                msg.u.keyReprot.keyType = UTE_DRV_KEY_SHORT_PRESS;
            }
            uteModuleSprotResetRovllverScreenMode();
            uteTaskGuiSendMsg(&msg);
        }
    }
    else
    {
#if UTE_MODULE_USE_KEY1_FOR_POWER_ON_SUPPORT
        /*! 使用按键1来做长按开机；jiangquan 2023-12-01*/
        if((keyCode==UTE_DRV_KEY1_PRESS)&&isLongPress&&(uteDrvBatteryCommonGetLvl()>=UTE_DRV_BATTERY_AUTO_POWER_OFF_LVL))
#else
        if((keyCode==UTE_DRV_KEY0_PRESS)&&isLongPress&&(uteDrvBatteryCommonGetLvl()>=UTE_DRV_BATTERY_AUTO_POWER_OFF_LVL))
#endif
        {
            uteApplicationCommonPowerOn();
        }
    }
#endif
}

/**
*@brief   设置当前ble连接最大mtu size
*@details
*@author        zn.zeng
*@date        2021-10-26
*/
void uteApplicationCommonSetMtuSize(uint16_t mtu)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,mtu=%d,%d", __func__,mtu,ble_get_gatt_mtu());
    uteApplicationCommonData.mtuSize = mtu;
}
/**
*@brief   获取当前ble连接最大mtu size
*@details
*@author        zn.zeng
*@date        2021-10-26
*/
uint16_t uteApplicationCommonGetMtuSize(void)
{
    return uteApplicationCommonData.mtuSize;
}

/**
*@brief   设置连接间隔更新标志
*@details
*@author  casen
*@date    2021-11-30
*/
void uteApplicationCommonSetHasUpdateConnectionParam(bool hasUpdateConnectionParam)
{
    uteApplicationCommonData.bleConnectState.hasUpdateConnectionParam = hasUpdateConnectionParam;
}
/**
*@brief   获取连接间隔更新标志
*@details
*@author  casen
*@date    2021-11-30
*/
bool uteApplicationCommonGetHasUpdateConnectionParam(void)
{
    return uteApplicationCommonData.bleConnectState.hasUpdateConnectionParam;
}
/*
*@brief   获取当前开机时间计数
*@details
*@author        zn.zeng
*@date        2021-11-30
*/
uint32_t uteApplicationCommonGetSystemPowerOnSecond(void)
{
    return uteApplicationCommonData.systemPowerOnSecond;
}
/**
*@brief  保存已经连接过标志
*@details
*@author        zn.zeng
*@date        2022-01-13
*/
void uteApplicationCommonSaveHasConnectOurApp(void)
{
    /*! 保存到文件zn.zeng, 2021-08-24*/
    void *file;
    uint8_t writebuff[1];
    writebuff[0] = uteApplicationCommonData.isHasConnectOurApp=true;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HAS_CONNECT_OUR_APP,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
}
/**
*@brief  获取连接过标志
*@details
*@author        zn.zeng
*@date        2022-01-13
*/
bool uteApplicationCommonIsHasConnectOurApp(void)
{
    return uteApplicationCommonData.isHasConnectOurApp;
}

/**
* @brief        设置IOS APP关闭标志
* @details
* @param[in]    isAppClosed APP是否关闭
* @author       Wang.Luo
* @date         2022-11-11
*/
void uteApplicationCommonSetAppClosed(bool isAppClosed)
{
    uteApplicationCommonData.isAppClosed = isAppClosed;
    if(isAppClosed)
    {
#if UTE_MODULE_SPORT_SUPPORT
        uteModuleSportDisconnectHandler();
#endif
    }
}

/**
* @brief        获取IOS APP关闭标志
* @details
* @author       Wang.Luo
* @return       true APP已关闭
* @date         2022-11-11
*/
bool uteApplicationCommonIsAppClosed(void)
{
    return uteApplicationCommonData.isAppClosed;
}

/**
*@brief   获取数据同步中标志
*@details
*@author  casen
*@date    2022-04-09
*/
bool uteApplicationCommonGetIsSynchronizingData(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.isSynchronizingData = %d", __func__, uteApplicationCommonData.isSynchronizingData);
    return uteApplicationCommonData.isSynchronizingData;
}

/**
*@brief  保存软件版本信息
*@details
*@author        xjc
*@date        2022-05-05
*/
void uteApplicationCommonSaveSoftwareVersionInfo(void)
{
    /*! 保存到文件xjc, 2022-05-05 */
    void *file;
    uint8_t writebuff[7];
    memcpy(&writebuff[0],&uteApplicationCommonData.softwareVersion[0],7);
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SOFTWARE_VERSION,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],7);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief  从flash读取软件版本信息
*@details
*@author        xjc
*@date        2022-05-05
*/
void uteApplicationCommonReadSoftwareVersion(void)
{
    void *file;
    uint8_t readbuff[7];
    memset(&readbuff[0],0,7);
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SOFTWARE_VERSION,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],7);
        uteModuleFilesystemCloseFile(file);
    }
    memcpy(&uteApplicationCommonData.softwareVersion[0],&readbuff[0],7);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL, &uteApplicationCommonData.softwareVersion[0],7);
}

/**
*@brief   比较软件版本是否更新
*@details
*@author  xjc
*@date    2022-05-05
*/
void uteApplicationCommonCheckSoftwareVersion(void)
{
    uint8_t tempSoftwareVersion[7];
    memcpy(&tempSoftwareVersion[0],UTE_SW_VERSION+(sizeof(UTE_SW_VERSION)-8),7);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,get project version:", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL, &tempSoftwareVersion[0],7);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL, &uteApplicationCommonData.softwareVersion[0],7);
    if((uteDrvBatteryCommonIsDelayDisplayCharger()) || (uteDrvBatteryCommonGetLvl() >= UTE_DRV_BATTERY_AUTO_POWER_OFF_LVL))
    {
#if UTE_COMPARE_SOFTWARE_VERSION_AND_CLEAR_FLASH_SUPPORT
        if(memcmp(&tempSoftwareVersion[0],&uteApplicationCommonData.softwareVersion[0],7) != 0)
        {
#if UTE_USER_ID_FOR_BINDING_SUPPORT
            uteModuleAppBindingClearUserId();
#endif
            uteModuleFilesystemDelAllData();
#if UTE_MODULE_BATTERY_SAVE_LAST_LVL_BEFORE_FACTORY_SUPPORT
            uteDrvBatteryCommonSaveLastLvlToSN1();
#endif
            uteModuleFilesystemInit();
            uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_SYSTEMPARM_DIR);
            memcpy(&uteApplicationCommonData.softwareVersion[0],&tempSoftwareVersion[0],7);
            uteApplicationCommonSaveSoftwareVersionInfo();
            uteModulePlatformSystemReboot();
        }
#endif
    }
}

#if UTE_MODULE_LOCAL_QUICK_SWITCH_SUPPORT
/**
*@brief        本地快捷开关参数信息
*@details
*@author      dengli.lu
*@date        2022-06-17
*/
void uteApplicationCommonLocalQuickSwitchReadConfig(void)
{
    void *file;
    uint8_t readbuff[3];
    /*! 显示参数zn.zeng, 2021-08-20  */
    readbuff[0] = DEFAULT_HEARTWARNING_IS_OPEN;
    readbuff[1] = DEFAULT_STEP_TARGET_OPEN;
    readbuff[2] = DEFAULT_DRINK_WATER_OPNE;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_LOCAL_QUICK_SWITCH,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],3);
        uteModuleFilesystemCloseFile(file);
    }
    uteApplicationCommonData.localQuickSwitch.isHeartWarning = readbuff[0];
    uteApplicationCommonData.localQuickSwitch.isTargetCompleted = readbuff[1];
    uteApplicationCommonData.localQuickSwitch.isDrinkWater = readbuff[2];
}
/**
*@brief  保存本地快捷开关状态
*@details
*@author      dengli.lu
*@date        2022-06-17
*/
void uteApplicationCommonSaveLocalQuickSwitchConfig(void)
{
    /*! 保存到文件zn.zeng, 2021-08-20*/
    void *file;
    uint8_t writebuff[3];
    writebuff[0] = uteApplicationCommonData.localQuickSwitch.isHeartWarning;
    writebuff[1] = uteApplicationCommonData.localQuickSwitch.isTargetCompleted;
    writebuff[2] = uteApplicationCommonData.localQuickSwitch.isDrinkWater;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_LOCAL_QUICK_SWITCH,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],3);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief   设置本地快捷开关状态
*@details
*@author       dengli.lu
*@date       2021-06-20
*/
void uteApplicationCommonSetLocalQuickSwitchStatus(ute_local_quick_switch_t localquickSwitch)
{
    uteModulePlatformTakeMutex(uteApplicationCommonMute);
    memcpy(&uteApplicationCommonData.localQuickSwitch, &localquickSwitch, sizeof(ute_local_quick_switch_t));
    uteModulePlatformGiveMutex(uteApplicationCommonMute);
    uteApplicationCommonSaveLocalQuickSwitchConfig();
}
/**
*@brief    获取本地快捷开关状态指针
*@details
*@author       dengli.lu
*@date       2021-06-20
*/
void uteApplicationCommonGetLocalQuickSwitchStatus(ute_local_quick_switch_t *localquickSwitch)
{
    uteModulePlatformTakeMutex(uteApplicationCommonMute);
    memcpy(localquickSwitch, &uteApplicationCommonData.localQuickSwitch, sizeof(ute_local_quick_switch_t));
    uteModulePlatformGiveMutex(uteApplicationCommonMute);
}
#endif //UTE_MODULE_LOCAL_QUICK_SWITCH_SUPPORT
#if SET_THE_LOW_BATTERY_POPUP_TIME
/*
*@brief   获取当前低电时间计数
*@details
*@author        chengjie.huang
*@date        2022-09-08
*/
uint32_t uteApplicationCommonGetlowBatterySecond(void)
{
    return uteApplicationCommonData.lowBatterySecond;
}

/*
*@brief   设置低电时间计数
*@details
*@author        chengjie.huang
*@date        2022-09-08
*/
void uteApplicationCommonSetlowBatterySecond(uint32_t lowBatterySecond)
{
    uteApplicationCommonData.lowBatterySecond=lowBatterySecond;
}
#endif
#if UTE_APP_READ_CUSTOM_SOFTWARE_VERSON_SUPPORT
/**
*@brief   0xF6客户自定义通用接口发送数据
*@details
*@author  dengli.lu
*@date    2022-08-30
*/
void uteApplicationCommonF6CmdCustomSendData(const uint8_t *data,uint8_t size)
{
    uteApplicationCommonData.customF6Cmd.custF6SendCrc = 0;
    uteApplicationCommonData.customF6Cmd.custF6SendCnt = size;
    uteApplicationCommonData.customF6Cmd.custF6SendOffset = 0;
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,custF6SendCnt=%d,",__func__,uteApplicationCommonData.customF6Cmd.custF6SendCnt);
    memcpy(&uteApplicationCommonData.customF6Cmd.custF6Send[0],&data[0],uteApplicationCommonData.customF6Cmd.custF6SendCnt);
    {
        uint8_t sendBuff[20];
        uint8_t sendCnt;
        sendBuff[0] = CMD_CUST_DEFINE_CMD;
        sendBuff[1] = uteApplicationCommonData.customF6Cmd.custF6SendOffset/18;
        sendCnt = uteApplicationCommonData.customF6Cmd.custF6SendCnt>18?18:uteApplicationCommonData.customF6Cmd.custF6SendCnt;
        memcpy(&sendBuff[2],&uteApplicationCommonData.customF6Cmd.custF6Send[uteApplicationCommonData.customF6Cmd.custF6SendOffset],sendCnt);
        for(int j=1; j<sendCnt+2; j++)
        {
            uteApplicationCommonData.customF6Cmd.custF6SendCrc^= sendBuff[j];
        }
        uteModuleProfileBleSendToPhone(&sendBuff[0],sendCnt+2);
        uteApplicationCommonData.customF6Cmd.custF6SendCnt = uteApplicationCommonData.customF6Cmd.custF6SendCnt - sendCnt;
        uteApplicationCommonData.customF6Cmd.custF6SendOffset = uteApplicationCommonData.customF6Cmd.custF6SendOffset+sendCnt;
    }
}
/**
*@brief   app读取客户自定义版本号
*@details
*@author  dengli.lu
*@date    2022-08-30
*/
void uteApplicationCommonReadCustomSoftwareVersion(uint8_t*receive,uint8_t length)
{
    uint16_t j=0;
    if(receive[1]==0xfd)
    {
        if(uteApplicationCommonData.customF6Cmd.isCustF6Send)
        {
            uteApplicationCommonData.customF6Cmd.isCustF6Send = false;
        }
        else
        {
            uint8_t response[3];
            response[0] = CMD_CUST_DEFINE_CMD;
            response[1] = 0xfd;
            response[2] = uteApplicationCommonData.customF6Cmd.custF6CmdCrc;
            uteModuleProfileBleSendToPhone(&response[0],3);
            uteApplicationCommonF6CmdCustomSendData(&uteApplicationCommonData.customF6Cmd.custF6Cmds[0],uteApplicationCommonData.customF6Cmd.custF6CmdCnt);
            uteApplicationCommonData.customF6Cmd.custF6CmdCnt = 0;
            uteApplicationCommonData.customF6Cmd.custF6CmdCrc = 0;
        }
    }
    else
    {
        if(length>2 &&(receive[2]==0xfa))
        {
            if(receive[1]==0)
            {
                uteApplicationCommonData.customF6Cmd.custF6CmdCnt = 0;
                uteApplicationCommonData.customF6Cmd.custF6CmdCrc = 0;
                uteApplicationCommonData.customF6Cmd.custF6SendCrc=0;
            }
            uteApplicationCommonData.customF6Cmd.isCustF6Send = false;
            memcpy(&uteApplicationCommonData.customF6Cmd.custF6Cmds[uteApplicationCommonData.customF6Cmd.custF6CmdCnt],&receive[2],length-2);
            uteApplicationCommonData.customF6Cmd.custF6CmdCnt = uteApplicationCommonData.customF6Cmd.custF6CmdCnt + length-2;
            for(j=1; j<length; j++)
            {
                uteApplicationCommonData.customF6Cmd.custF6CmdCrc^= receive[j];
            }
            if(uteApplicationCommonData.customF6Cmd.custF6CmdCnt>108)
            {
                uteApplicationCommonData.customF6Cmd.custF6CmdCnt = 0;
                uteApplicationCommonData.customF6Cmd.custF6CmdCrc = 0;
                uteApplicationCommonData.customF6Cmd.custF6SendCrc=0;
            }
            receive[0] = CMD_CUST_DEFINE_CMD;
            //receive[1] = receive[1];
            memcpy(&receive[2], UTE_SW_CUSTOM_VERSION, sizeof(UTE_SW_CUSTOM_VERSION)-1);
            uteModuleProfileBleSendToPhone(&receive[0],2+sizeof(UTE_SW_CUSTOM_VERSION)-1);
            uteApplicationCommonData.customF6Cmd.custF6SendCnt =0;
            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,PCM43*****custF6SendCrc=0x%x", __func__, uteApplicationCommonData.customF6Cmd.custF6SendCrc);
            for(j=1; j<(2+sizeof(UTE_SW_CUSTOM_VERSION)-1); j++)
            {
                uteApplicationCommonData.customF6Cmd.custF6SendCrc^= receive[j];
            }
            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,PCM*****custF6SendCrc=0x%x", __func__, uteApplicationCommonData.customF6Cmd.custF6SendCrc);
        }
        else
        {
            uint8_t sendBuff[20];
            uint8_t sendCnt;
            uteApplicationCommonData.customF6Cmd.isCustF6Send = true;
            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL,"%s,receive[1]=%d,custF6SendOffset=%d,custF6SendCnt=%d,\r\n",__func__,receive[1],uteApplicationCommonData.customF6Cmd.custF6SendOffset,uteApplicationCommonData.customF6Cmd.custF6SendCnt);
            if(uteApplicationCommonData.customF6Cmd.custF6SendCnt>0)
            {
                sendBuff[0] = CMD_CUST_DEFINE_CMD;
                sendBuff[1] = uteApplicationCommonData.customF6Cmd.custF6SendOffset/18;
                sendCnt = uteApplicationCommonData.customF6Cmd.custF6SendCnt>18?18:uteApplicationCommonData.customF6Cmd.custF6SendCnt;
                memcpy(&sendBuff[2],&uteApplicationCommonData.customF6Cmd.custF6Send[uteApplicationCommonData.customF6Cmd.custF6SendOffset],sendCnt);
                for(j=1; j<sendCnt+2; j++)
                {
                    uteApplicationCommonData.customF6Cmd.custF6SendCrc^= sendBuff[j];
                }
                uteModuleProfileBleSendToPhone(&sendBuff[0],sendCnt+2);
                uteApplicationCommonData.customF6Cmd.custF6SendOffset = uteApplicationCommonData.customF6Cmd.custF6SendOffset+sendCnt;
                uteApplicationCommonData.customF6Cmd.custF6SendCnt = uteApplicationCommonData.customF6Cmd.custF6SendCnt - sendCnt;
            }
            else
            {
                sendBuff[0] = CMD_CUST_DEFINE_CMD;
                sendBuff[1] = 0xfd;
                sendBuff[2] = uteApplicationCommonData.customF6Cmd.custF6SendCrc;
                uteModuleProfileBleSendToPhone(&sendBuff[0],3);
                uteApplicationCommonData.isSynchronizingData = false;
            }
        }
    }
}
#endif
#if UTE_MODULE_POWERON_LANGUAGE_SELECT_SUPPORT
/**
*@brief  设置过语言标志
*@details
*@author        yrc
*@date        2022-10-12
*/
bool uteApplicationCommonIsFirtSetting(void)
{
    return uteApplicationCommonData.IsFirtSetting;
}
#endif

#if UTE_DRV_POWER_VDD_CTRL_SUPPORT
/**
 *@brief  设置VddPower的状态
 *@details
 *@author        xjc
 *@date        2022-10-27
 */
void uteApplicationCommonSetVddPower(bool isEnable)
{
    if (isEnable)
    {
        uteApplicationCommonData.VddPowerEnableCnt = 3;
    }
    else
    {
        uteApplicationCommonData.VddPowerEnableCnt = 0;
    }
}

/**
 *@brief  每秒查看关机状态下Vdd Power的状态
 *@details
 *@author        xjc
 *@date        2022-10-27
 */
void uteApplicationCommonVddPowerEverySecond(void)
{
    if (!uteApplicationCommonIsPowerOn())
    {
        if (uteApplicationCommonData.VddPowerEnableCnt > 0)
        {
            if ((--uteApplicationCommonData.VddPowerEnableCnt) == 0)
            {
                uteModulePlatformVddPowerEnable(false);
            }
        }
    }
}
#endif

/**
 *@brief        恢复出厂设置
 *@details
 *@author        xjc
 *@date        2022-10-27
 */
void uteApplicationCommonFactoryReset(void)
{
    // uteDrvScreenCommonDisplayOff();
    gui_sleep();
#if UTE_USER_ID_FOR_BINDING_SUPPORT
    uteModuleAppBindingClearUserId();
#endif
    uteModuleFilesystemDelAllData();
#if UTE_MODULE_BATTERY_SAVE_LAST_LVL_BEFORE_FACTORY_SUPPORT
    uteDrvBatteryCommonSaveLastLvlToSN1();
#endif
    bsp_rtc_recode_set(1);
    uteModulePlatformSystemReboot();
}

/**
 *@brief        关机
 *@details
 *@author        xjc
 *@date        2022-10-27
 */
void uteApplicationCommonPoweroff(void)
{
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT&&UTE_MODULE_SHIP_MODE_POWER_OFF_SUPPORT //关机进入船运模式
    ute_new_factory_test_data_t *data;
    uteModuleNewFactoryTestSetMode(&data);
    data->mode = FACTORY_TEST_MODE_SHIP;
#endif
#if ECIG_POWER_CONTROL
    ecig.poweroff_flag=0;
#endif
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_START_POWER_OFF, 0);

}

/**
 *@brief        重启
 *@details
 *@author        xjc
 *@date        2022-10-27
 */
void uteApplicationCommonRestart(void)
{
//    uteDrvScreenCommonDisplayOff();
    // uteApplicationCommonRealPowerOffMsg(); // 先保存数据再执行重启
    uteApplicationCommonStartPowerOffMsg();
    uteModulePlatformSystemReboot();
}

/**
 *@brief  保存ANCS连接状态
 *@details
 *@author        xjc
 *@date        2022-10-27
 */
void uteApplicationCommonSetAncsConnStatus(bool isConnected)
{
    uteApplicationCommonData.isAncsConnected = isConnected;
}
/**
 *@brief  获取ANCS连接状态
 *@details
 *@author        xjc
 *@date        2022-10-27
 */
bool uteApplicationCommonGetAncsConnStatus(void)
{
    return uteApplicationCommonData.isAncsConnected;
}

/**
 * @brief        获取设备二维码
 * @details
 * @param[in]    len     二维码字符最大长度
 * @param[out]   qrBuff  二维码字符串接收数组
 * @return       二维码字符长度
 * @author       Wang.Luo
 * @date         2024-10-29
 */
uint8_t uteApplicationCommonGetDeviceQrCodeLink(char *qrBuff,uint8_t len)
{
    uint8_t mac[6];
    uint8_t macStr[18];
    uteModulePlatformGetBleMacAddress(mac);
    snprintf((char *)macStr,sizeof(macStr),"%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    uint8_t drvName[32] = {0};
    uint8_t drvNameLen = sizeof(drvName);
    uteModulePlatformGetDevCompleteName(drvName,&drvNameLen);
    memset(qrBuff,0,len);
    snprintf(qrBuff,len,"%s?Name=%s&SW=%s&MAC=%s",UTE_BINDING_QRENCODE_LINK,drvName,UTE_SW_VERSION,macStr);
    uint8_t stringSize = strlen(qrBuff);
    return stringSize;
}

/**
*@brief   获取精确到十位的浮点型结果
*@details
*@author  dengli.lu
*@date   2021-10-26
*/
float ExactDecimalPoint(float data,uint8_t bit)
{
    float dst = 0.0f;
    uint32_t tmp = 0;
    float multiple = 1;
    char i = 0;
    for( i=0; i<bit; i++)
    {
        multiple *= 10.0f;
    }
    data = data + (5.0f/(multiple*10.0f));//四舍五入最后一位小数
    tmp = (data * multiple);//取整
    dst = (float)tmp/multiple;
    return dst;
}

/**
 * @brief        保存重启信息
 * @details
 * @author       Wang.Luo
 * @date         2024-12-30
 */
void uteModuleHardfaultInfoSave(void)
{
    u8 cause = exception_restart_cause();
    if (cause == RESTART_VUSB || cause == RESTART_WKUP || cause == RESTART_FIRST_ON || cause == RESTART_SW || cause == RESTART_UNKNOWN)
    {
        printf("restart reason: %d\n", cause);
        return;
    }
    tm_t rtc_tm;
    rtc_tm = time_to_tm(RTCCNT);
    char *buf = (char *)uteModulePlatformMemoryAlloc(1024);
    memset(buf,0,1024);
    // print_r32(exception_debug_info_get(),32);
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d\n", rtc_tm.year, rtc_tm.mon, rtc_tm.day, rtc_tm.hour, rtc_tm.min, rtc_tm.sec);
    u32 cpu_gprs[32];
    u32 halt_err[16];
    memcpy(cpu_gprs, exception_debug_info_get(), sizeof(cpu_gprs));
    memcpy(halt_err, halt_err_debug_info_get(), sizeof(halt_err));
    strcat(buf, "Cause:\n");
    switch (cause)
    {
        case RESTART_WDT:
            strcat(buf, "RESTART_WDT\n");
            break;
        case RESTART_SW:
            strcat(buf, "RESTART_SW\n");
            break;
        case RESTART_WK10S:
            strcat(buf, "RESTART_WK10S\n");
            break;
        case RESTART_VUSB:
            strcat(buf, "RESTART_VUSB\n");
            break;
        case RESTART_WKUP:
            strcat(buf, "RESTART_WKUP\n");
            break;
        case RESTART_FIRST_ON:
            strcat(buf, "RESTART_FIRST_ON\n");
            break;
        default:
            strcat(buf, "RESTART_UNKNOWN\n");
            printf("unknown restart cause: %d\n", cause);
            break;
    }
    strcat(buf, "cpu_gprs[]:\n");
    for (u8 i = 0; i < 32; i++)
    {
        char tmp[48];
        sprintf(tmp, "[%02d]%08lx ", i, cpu_gprs[i]);
        strcat(buf, tmp);
    }
    strcat(buf, "\n");
    strcat(buf, "halt_err[]:\n");
    for (u8 i = 0; i < 16; i++)
    {
        char tmp[48];
        sprintf(tmp, "[%02d]%08lx ", i, halt_err[i]);
        strcat(buf, tmp);
    }
    strcat(buf, "\n");

    printf("%s\n",buf);

#if UTE_HARDFAULT_INFO_TO_FLASH_SUPPORT
    uint8_t path[42];
    memset(&path[0], 0, sizeof(path));
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR, dirInfo, NULL);
    if (dirInfo->filesCnt >= UTE_HARDFAULT_INFO_TO_FLASH_MAX_CNT)
    {
        memset(&path[0], 0, sizeof(path));
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR, &dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,del file=%s", __func__, &path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0], 0, 42);
    sprintf((char *)&path[0], "%s/%04d%02d%02d%02d%02d", UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR, rtc_tm.year, rtc_tm.mon, rtc_tm.day, rtc_tm.hour, rtc_tm.min);
    void *file;
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &buf[0], strlen(buf));
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(dirInfo);
#endif
    uteModulePlatformMemoryFree(buf);
}

/*!
1、APP发送0xfe0601 开始采集数据，0x33F2 会先返回0xfe0601+4byte(log size)
2、0x34F2 开始不断返回：0xfe0601+2byete(序号)+数据(MTU-5)byte
3、固件每发送64个包,等待120ms这时APP通过0x33F1 发送0xFE0602+2byte（接收到的最新序号）。
4、0x33F1 返回0xFE0601FD 表示返回数据结束。
*/
static void uteModuleHardfaultSendlogData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);

    sendParam->currSendMtuSize = uteApplicationCommonGetMtuSize();
    sendParam->dataBuffSize = sendParam->currSendMtuSize - 5;
    uint16_t sendSize = 0;
    uint8_t *response = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->currSendMtuSize);
    response[0] = CMD_DEBUG_TEST_DATA;
    response[1] = 0x06;
    response[2] = 0x01;
    response[3] = sendParam->currSendMaxIndex >> 8 & 0xFF;
    response[4] = sendParam->currSendMaxIndex & 0xFF;

    char path[42];
    memset(&path[0], 0, sizeof(path));

    if (sendParam->dirInfo.filesCnt > 0 && sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt)
    {
        void *file;
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,read file=%s", __func__, &path[0]);

        if (uteModuleFilesystemOpenFile(&path[0], &file, FS_O_RDONLY))
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,filesCnt=%d,currSendFileIndex=%d", __func__, sendParam->dirInfo.filesCnt, sendParam->currSendFileIndex);
            sendParam->currSendFileSize = uteModuleFilesystemGetFileSize(file);
            uteModuleFilesystemSeek(file, sendParam->currSendFileDataOffset, FS_SEEK_SET);
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,currSendFileSize=%d,currSendFileDataOffset=%d,currSendMtuSize=%d,dataBuffSize=%d", __func__, sendParam->currSendFileSize, sendParam->currSendFileDataOffset, sendParam->currSendMtuSize, sendParam->dataBuffSize);

            if (sendParam->currSendFileDataOffset + sendParam->dataBuffSize <= sendParam->currSendFileSize)
            {
                uteModuleFilesystemReadData(file, &response[5], sendParam->dataBuffSize);
                uteModuleFilesystemCloseFile(file);
                sendParam->currSendFileDataOffset += sendParam->dataBuffSize;
            }
            else
            {
                sendParam->dataBuffSize = sendParam->currSendFileSize - sendParam->currSendFileDataOffset;
                uteModuleFilesystemReadData(file, &response[5], sendParam->dataBuffSize);
                uteModuleFilesystemCloseFile(file);
                sendParam->currSendFileDataOffset = 0;
                sendParam->currSendFileIndex++;
            }
            sendSize = sendParam->dataBuffSize + 5;

            sendParam->currSendMaxIndex++;
        }
    }
    else
    {
        sendParam->currSendFileIndex = sendParam->dirInfo.filesCnt;
    }

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,sendParam->currSendMaxIndex:%d",__func__,sendParam->currSendMaxIndex);

    if(sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt)
    {
        response[3] = 0xfd;
        sendSize = 4;
        uteApplicationCommonSyncDataTimerStop();
        uteModuleFilesystemDelDirectoryAllFiles(UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR);
    }

    uteModuleProfileBle50SendToPhone(response, sendSize);

    uteModulePlatformMemoryFree(response);
}

/**
 * @brief        开始发送Hardfault信息
 * @details
 * @author       Wang.Luo
 * @date         2024-12-30
 */
void uteModuleHardfaultStartSendlogData(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->currSendFileIndex = 0;
    param->currSendFileIndex = 0;
    param->currSendFileSize = 0;
    param->currSendFileDataOffset = 0;
    param->currSendMaxIndex = 1;
    param->currSendMtuSize = uteApplicationCommonGetMtuSize();
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR, &param->dirInfo, NULL);

    uint8_t response[] = {CMD_DEBUG_TEST_DATA, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00};
    if (param->dirInfo.filesCnt > 0)
    {
        void *file;
        char path[42];
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR, &param->dirInfo.filesName[0][0]);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uint32_t fileSize = 0;
            fileSize = uteModuleFilesystemGetFileSize(file) * param->dirInfo.filesCnt;
            uteModuleFilesystemCloseFile(file);
            response[3] = (fileSize >> 24) & 0xFF;
            response[4] = (fileSize >> 16) & 0xFF;
            response[5] = (fileSize >> 8) & 0xFF;
            response[6] = fileSize & 0xFF;
        }
    }
    uteModuleProfileBle50SendToPhone(response, sizeof(response));

    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleHardfaultSendlogData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
}
