/**
*@file
*@brief     协议模块
*@details   和应用端通讯协议
*@author        zn.zeng
*@date        2021-08-17
*@version       v1.0
*/
#include "include.h"
#include "ute_module_message.h"
#include "ute_module_log.h"
#include "ute_module_systemtime.h"
#include "ute_module_protocol.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_filesystem.h"
#include "ute_module_gui_common.h"
#include "ute_module_call.h"
#include "ute_drv_motor.h"
#include "ute_module_notify.h"
#include "ute_module_weather.h"
#include "ute_module_heart.h"
#include "func_cover.h"
#include "ute_module_bloodoxygen.h"
#include "ute_drv_battery_common.h"
#include "ute_module_sport.h"
#include "ute_module_sleep.h"
#include "ute_module_notdisturb.h"
#include "ute_drv_gsensor_common.h"
#include "ute_module_findphone.h"
#include "ute_module_watchonline.h"
#include "ute_module_factorytest.h"
#include "ute_module_music.h"
#include "func_cover.h"

/**
*@brief        设置时间12H或者24H格式，公里英里设置
*@details      解析协议
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-18
*/
void uteModuleProtocolSetParamHourKmFormat(uint8_t*receive,uint8_t length)
{
    if(length>=3)
    {
        bool is12Hour = false;
        bool isMi = false;
        if(receive[1]==0x02)
        {
            isMi = true;
        }
        if(receive[2]==0x02)
        {
            is12Hour = true;
        }
        uteModuleSystemtimeSetTimeFormat(is12Hour,isMi);
        uteModuleProfileBleSendToPhone(&receive[0],length);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,error length=%d", __func__,length);
    }
}
/**
*@brief        绑定提醒
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       dengli.lu
*@date       2021-08-16
*/
void uteAppCmdVerificationBindingNotify()
{
    // UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,uteModuleAppBindingGetOurAppConnection = %d,uteModuleAppBindingGetBindingNotify=%d", __func__,uteModuleAppBindingGetOurAppConnection(),uteModuleAppBindingGetBindingNotify());
//     uteModuleAppBindingSetOurAppConnection(true);
//     uteModuleAppBindingSetHasBindingBefore(HAS_BEEN_CONNECTED);
//     if(!uteModuleAppBindingGetBindingNotify())
//     {
//         uteModuleAppBindingSetBindingNotify(true);
// #if !UTR_APP_BINDING_NOT_NOTIFY_SUPPORT
//         uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
//         uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_APP_BINDING_NOTIFY_ID);
// #endif
//     }
}
/**
*@brief        获取软件版本号
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-16
*/
void uteModuleProtocolReadSoftwareVersion(uint8_t*receive,uint8_t length)
{
    uint8_t response[20]= {0};
    response[0] = receive[0];
    memcpy(&response[1],UTE_SW_VERSION,sizeof(UTE_SW_VERSION));
    uteModuleProfileBleSendToPhone(&response[0],sizeof(UTE_SW_VERSION));
#if UTE_MODULE_SCREENS_APP_BINDING_NOTIFY_SUPPORT
    uteAppCmdVerificationBindingNotify();
#endif
}
/**
*@brief        获取电池电量
*@details    范围在0~100%
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-18
*/
void uteModuleProtocolReadBatteryLvl(uint8_t*receive,uint8_t length)
{
    uint8_t response[2]= {0};
    response[0] = receive[0];
    response[1] = uteDrvBatteryCommonGetLvl();
    uteModuleProfileBleSendToPhone(&response[0],2);
}
/**
*@brief        设置时间日期解析
*@details      包括时区
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-18
*/
void uteModuleProtocolSetDateTime(uint8_t*receive,uint8_t length)
{
    if(length>=8)
    {
        ute_module_systemtime_time_t set;
        uteModuleSystemtimeGetTime(&set);
        set.year = receive[1]<<8|receive[2];
        set.month = receive[3];
        set.day = receive[4];
        set.hour = receive[5];
        set.min = receive[6];
        set.sec = receive[7];
        if(length==9)
        {
            int8_t timeZone = receive[8];
            if(timeZone>=-12&&timeZone<=12)
            {
                set.zone = timeZone;
            }
        }
        else
        {
            set.zone = 8;
        }
        uteModuleSystemtimeSetTime(set);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
        uteModuleCwmTimeUpdate();
#endif
        uteModuleProfileBleSendToPhone(&receive[0],length);
        if(!uteApplicationCommonIsHasConnectOurApp())
        {
            uteApplicationCommonSaveHasConnectOurApp();
#if UTE_MODULE_BT_ONCE_PAIR_CONNECT_SUPPORT
            if(uteModuleCallBtIsPowerOn() == false)
            {
                uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
                bt_abort_reconnect(); //终止回连
            }
#endif
        }
#if UTE_MODULE_SCREENS_APP_BINDING_NOTIFY_SUPPORT
        uteAppCmdVerificationBindingNotify();
#endif
#if UTE_MODULE_ALI_UPAY_SUPPORT||UTE_MODULE_ALI_UPAY_V02_SUPPORT
        uteModuleAliUpayGetBindingStatus();
        uteModuleAliUpaySystemUpayTimeDone();
#endif
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,error length=%d", __func__,length);
    }
}
/**
*@brief        设置ble名称
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-18
*/
void uteModuleProtocolSetBleName(uint8_t*receive,uint8_t length)
{
    if (length>1)
    {
        uint16_t snDataLen = sizeof(ute_application_sn_data_t);
        ute_application_sn_data_t *snData = uteModulePlatformMemoryAlloc(snDataLen);
        memset(snData, 0, snDataLen);
        uteModulePlatformFlashNorRead(snData, UTE_USER_PARAM_ADDRESS, snDataLen);
        uint8_t nameLen = length - 1 < sizeof(snData->bleDevName) ? length - 1 : snData->bleDevName;
        memcpy(snData->bleDevName, &receive[1], nameLen);
        snData->bleDevNameLen = nameLen;
        uteModulePlatformFlashNorErase(UTE_USER_PARAM_ADDRESS);
        uteModulePlatformFlashNorWrite(snData, UTE_USER_PARAM_ADDRESS, sizeof(ute_application_sn_data_t));
        uteModulePlatformMemoryFree(snData);
        uteModuleProfileBleSendToPhone(&receive[0], 1);
    }
}
/**
*@brief        设置其他参数解析
*@details      参数：身高、体重、亮屏时间、目标步数、抬手亮屏开关、
*              最大心率报警值、支持心率辅助睡眠开关（无效）、年龄、性别、防丢功能开关（无效）、
*              中英文设置（无效）、温度单位切换、最小心率报警值
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-18
*/
void uteModuleProtocolSetOtherParam(uint8_t*receive,uint8_t length)
{
    if (length==19)
    {
        uint8_t response[2]= {0};
        response[0] = receive[0];
        ute_personal_info_t personal;
        uteApplicationCommonGetPersonalInfo(&personal);
        personal.age = receive[13];
        personal.height = receive[2];
        if(receive[14]==0x01)
        {
            personal.isMan = true;
        }
        else
        {
            personal.isMan = false;
        }
        personal.weight = receive[4]*1000+receive[3]*10;
        uteApplicationCommonSavePersonalInfo(personal);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
        cwmUserInfoUpdate();    //同步用户信息给赛维算法
#endif
        bool isHandScreenOn = false;
        bool isFahrenheit = false;
        uint32_t stepsTargetCnt =receive[6]<<24|receive[7]<<16|receive[8]<<8|receive[9];
        if(receive[10]==0x01)
        {
            isHandScreenOn = true;
        }
        else
        {
            isHandScreenOn = false;
        }
        if(receive[17] == 0x01)
        {
            isFahrenheit = false;
        }
        else if(receive[17] == 0x02)
        {
            isFahrenheit= true;
        }
#if (!QUICK_SWITCH_DISPLAY_TIME_SUPPORT)
        uint8_t displayTime = receive[5];
        uteModuleGuiCommonSetDisplayOffTime(displayTime);
#endif
        uteModuleGuiCommonSetDisplayTemperatureUtil(isFahrenheit);
#if UTE_MODULE_LOCAL_SET_LIFT_WRIST_SUPPORT
        isHandScreenOn = uteModuleSportGetIsOpenHandScreenOn();
#endif
#if UTE_MODULE_SPORT_SUPPORT
        uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandScreenOn,stepsTargetCnt);
#endif
#if UTE_MODULE_HEART_SUPPORT
        ute_module_heart_warning_t heartWarn;
        heartWarn.setMinHeart = receive[18];
        heartWarn.setMaxHeart = receive[11];
        if((heartWarn.setMaxHeart>=0xfe)||(heartWarn.setMinHeart==0))
        {
            heartWarn.isOpen = false;
        }
        if((receive[11]>99) && (receive[11]<=200 ))
        {
            heartWarn.isOpen =true;
            heartWarn.setMaxHeart = receive[11];
        }
        if(length>=19)
        {
            if((receive[18]<=100) && (receive[18]>0))
            {
                heartWarn.isOpen =true;
                heartWarn.setMinHeart = receive[18];
            }
        }
        uteModuleHeartSaveHeartWaringInfo(&heartWarn);
#endif
        uteModuleProfileBleSendToPhone(&response[0],1);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,error length=%d", __func__,length);
    }
}
/**
*@brief        恢复出厂设置
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-20
*/
void uteModuleProtocolFactoryReset(uint8_t *receive, uint8_t length)
{
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "---->uteModuleProtocolFactoryReset!!!!!!!!");
#if UTE_USER_ID_FOR_BINDING_SUPPORT
    uteModuleAppBindingClearUserId();
#endif
    uteModuleFilesystemDelAllData();
#if UTE_MODULE_BATTERY_SAVE_LAST_LVL_BEFORE_FACTORY_SUPPORT
    uteDrvBatteryCommonSaveLastLvlToSN1();
#endif
    bsp_rtc_recode_set(1);
    if (length == 1)
    {
        uteModulePlatformSystemReboot();
    }
    else if (receive[1] == 0x01)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_START_POWER_OFF, 0);
#if UTE_MODULE_SHIP_MODE_SUPPORT
        uteModuleShipModeEnter();
#endif
    }
}

/**
*@brief        设置闹钟参数或者控制震动
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-21
*/
void uteModuleProtocolSetAlarmOrCtrlMotor(uint8_t*receive,uint8_t length)
{
#if 1

    if (length>=8)
    {
        if(receive[1]==0x00)/*!震动控制 zn.zeng, 2021-08-21  */
        {
            if ((receive[4] == 0x01) && (receive[5] == 0x02) && (receive[6] == 0x07) && (receive[7] == 0x01))
            {
                /*!APP查找手环时亮屏 xjc 2022-01-12*/
                sys_cb.cover_index = REMIND_COVER_FIND_WATCH;
                sys_cb.remind_tag = true;

                if(0)
                {}
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
                else if(uteModuleGuiCommonIsInScreenSaver())
                {
                    uteTaskGuiStartScreen(uteModuleGuiCommonGetTempUIConfigId());
                }
#endif
                else
                {
#if !UTE_MODULE_FIND_WRISTAWAKE_SCREEN_SUPPORT
                    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HNAD_SCREEN_ON_NOTIFY, 0);
#endif
                }

#if UTE_MODULE_LOCAL_RINGTONE_FIND_SUPPORT
#if UTE_MODULE_MOTOR_POWER_STATUS_SAVE_SUPPORT
                if(!uteModuleMotorGetVibrationSwitchStatus())
                {
                    uteModuleMotorSetIsOpenVibrationStatus(true);
                }
#endif
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,UTE_MODULE_LOCAL_RINGTONE_FIND_DURATION);
                if(!uteModuleLocalRingtoneGetMuteStatus())
                {
                    uteModuleLocalRingtonePlayRing(RINGTON_TYPE_FIND);
                }
#else
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,receive[5]);
#endif
#if UTE_MODULE_FIND_WRISTAWAKE_SCREEN_SUPPORT
                extern ute_module_gui_common_t uteModuleGuiCommonData;
                if(uteModuleGuiCommonData.pCurrUIConfig->screenId != UTE_MOUDLE_SCREENS_FACTORY_TEST_ID)
                {
                    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_FIND_WRITAWAKE_SCREEN_ID);
                }
#endif
            }
            else if(receive[5]==10)
            {
                /*!来电的时候，震动在界面里面处理，zn.zeng 2022-07-01*/
            }
            else
            {
#if UTE_MODULE_NOTIFY_START_MOTOR_INTO_SCREEN_SUPPORT
                if(receive[5] != 0x01) //消息提醒震动
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,receive[5]);
                }
#elif UTE_MODULE_GUI_TESTING_NOT_GOTO_NOTIFICATION_SCREEN_SUPPORT
                if(receive[5] == 0x01) //消息提醒震动
                {
                    if(!uteModuleGuiCommonIsDontNeedNotificationGuiScreen())
                    {
                        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,receive[5]);
                    }
                }
                else
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,receive[5]);
                }
#else
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,receive[5]);
#endif
            }
        }
        else/*! 设置闹钟参数 zn.zeng, 2021-08-21  */
        {
#if (!UTE_MODULE_LOCAL_ALARM_SUPPORT)
            if((receive[8]>0)&&(receive[8]<=SYSTEM_TIME_ALARMS_MAX_CNT))
            {
                ute_module_systemtime_one_alarm_t alarm;
                alarm.year = 0;
                alarm.month = 0;
                alarm.day = 0;
                alarm.weekDay = receive[1]&0x7f;
                alarm.hour = receive[2];
                alarm.min = receive[3];
                alarm.sec = 0;
                alarm.isSingle = false;
                alarm.durationTimeSec = receive[5];
                if(alarm.durationTimeSec!=0)
                {
                    alarm.isOpen = true;
                }
                else
                {
                    alarm.isOpen = false;
                }
                uteModuleSystemtimeSaveAlarmInfo(alarm,receive[8]-1);
            }
#endif
        }
        uteModuleProfileBleSendToPhone(&receive[0],2);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,error length=%d", __func__,length);
    }

#endif

}
/**
*@brief        读取ble mac 地址
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleProtocolReadBleMac(uint8_t*receive,uint8_t length)
{
    uint8_t response[7];
    memset(&response[0],0x00,7);
    response[0] = receive[0];
    uteModulePlatformGetBleMacAddress(&response[1]);
    uteModuleProfileBleSendToPhone(&response[0],7);
}
/**
*@brief        设置多国语言
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleProtocolSetMultipleLanguage(uint8_t*receive,uint8_t length)
{
    uint8_t response[20];
    memset(&response[0],0,20);
    response[0] = receive[0];
    response[1] = receive[1];
    if(receive[1]==0xaa)
    {
        response[2] = uteModuleSystemtimeReadLanguage();
        response[19] |= SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT<<0;
        response[19] |= SCREEN_TITLE_MULTIPLE_ENGLISH_LANGUAGE_SUPPORT<<1;
        response[19] |= SCREEN_TITLE_MULTIPLE_KOREAN_LANGUAGE_SUPPORT<<2;
        response[19] |= SCREEN_TITLE_MULTIPLE_JAPANESE_LANGUAGE_SUPPORT<<3;
        response[19] |= SCREEN_TITLE_MULTIPLE_GERMAN_LANGUAGE_SUPPORT<<4;
        response[19] |= SCREEN_TITLE_MULTIPLE_SPANISH_LANGUAGE_SUPPORT<<5;
        response[19] |= SCREEN_TITLE_MULTIPLE_FRENCH_LANGUAGE_SUPPORT<<6;
        response[19] |= SCREEN_TITLE_MULTIPLE_ITALIAN_LANGUAGE_SUPPORT<<7;

        response[18] |= SCREEN_TITLE_MULTIPLE_PORTUGUESE_LANGUAGE_SUPPORT<<0;
        response[18] |= SCREEN_TITLE_MULTIPLE_ARABIC_LANGUAGE_SUPPORT<<1;
        response[18] |= SCREEN_TITLE_MULTIPLE_INDIA_LANGUAGE_SUPPORT<<2;
        response[18] |= SCREEN_TITLE_MULTIPLE_HINDI_LANGUAGE_SUPPORT<<3;
        response[18] |= SCREEN_TITLE_MULTIPLE_POLISH_LANGUAGE_SUPPORT<<4;
        response[18] |= SCREEN_TITLE_MULTIPLE_RUSSIAN_LANGUAGE_SUPPORT<<5;
        response[18] |= SCREEN_TITLE_MULTIPLE_DUTCH_LANGUAGE_SUPPORT<<6;
        response[18] |= SCREEN_TITLE_MULTIPLE_TURKISH_LANGUAGE_SUPPORT<<7;

        response[17] |= SCREEN_TITLE_MULTIPLE_BENGALI_LANGUAGE_SUPPORT<<0;
        response[17] |= SCREEN_TITLE_MULTIPLE_URDU_LANGUAGE_SUPPORT<<1;
        response[17] |= SCREEN_TITLE_MULTIPLE_INDONESUAN_LANGUAGE_SUPPORT<<2;
        response[17] |= SCREEN_TITLE_MULTIPLE_PUNJABI_LANGUAGE_SUPPORT<<3;
        response[17] |= SCREEN_TITLE_MULTIPLE_THAI_LANGUAGE_SUPPORT<<4;
        response[17] |= SCREEN_TITLE_MULTIPLE_CZECH_LANGUAGE_SUPPORT<<5;
        response[17] |= SCREEN_TITLE_MULTIPLE_TRADITIONAL_CHINESE_LANGUAGE_SUPPORT<<6;
#ifdef SCREEN_TITLE_MULTIPLE_HEBREW_LANGUAGE_SUPPORT
        response[17] |= SCREEN_TITLE_MULTIPLE_HEBREW_LANGUAGE_SUPPORT<<7;
#endif
#if SCREEN_TITLE_MULTIPLE_SLOVAK_LANGUAGE_SUPPORT//斯洛伐克语 0x19
        response[16] |= SCREEN_TITLE_MULTIPLE_SLOVAK_LANGUAGE_SUPPORT << 0;
#endif
#if SCREEN_TITLE_MULTIPLE_HUNGARIAN_LANGUAGE_SUPPORT//匈牙利语 0x1a
        response[16] |= SCREEN_TITLE_MULTIPLE_HUNGARIAN_LANGUAGE_SUPPORT << 1;
#endif
#ifdef SCREEN_TITLE_MULTIPLE_ROMANIAN_LANGUAGE_SUPPORT
        response[16] |= SCREEN_TITLE_MULTIPLE_ROMANIAN_LANGUAGE_SUPPORT   << 2;
#endif
#if SCREEN_TITLE_MULTIPLE_BULGARIAN_LANGUAGE_SUPPORT //保加利亚语 0X26  38
        response[15] |= SCREEN_TITLE_MULTIPLE_BULGARIAN_LANGUAGE_SUPPORT << 5;
#endif
#if SCREEN_TITLE_MULTIPLE_PERSIAN_LANGUAGE_SUPPORT
        response[15] |= SCREEN_TITLE_MULTIPLE_PERSIAN_LANGUAGE_SUPPORT << 7;
#endif
#ifdef SCREEN_TITLE_MULTIPLE_KHMER_LANGUAGE_SUPPORT
        response[14] |= SCREEN_TITLE_MULTIPLE_KHMER_LANGUAGE_SUPPORT << 2;
#endif
#if SCREEN_TITLE_MULTIPLE_HRVATSKA_LANGUAGE_SUPPORT //克罗地亚语 0X33  51
        response[13] |= SCREEN_TITLE_MULTIPLE_HRVATSKA_LANGUAGE_SUPPORT << 2;
#endif
#ifdef SCREEN_TITLE_MULTIPLE_DANISH_LANGUAGE_SUPPORT//丹麦语 0X39
        response[12] |= SCREEN_TITLE_MULTIPLE_DANISH_LANGUAGE_SUPPORT<<0;
#endif
#ifdef SCREEN_TITLE_MULTIPLE_MALAYSIA_LANGUAGE_SUPPORT //马来西亚语 0x40
        response[12] |= SCREEN_TITLE_MULTIPLE_MALAYSIA_LANGUAGE_SUPPORT << 7;
#endif
#ifdef SCREEN_TITLE_MULTIPLE_SLOVENIAN_LANGUAGE_SUPPORT //斯洛文尼亚语 0X4d 77
        response[10] |= SCREEN_TITLE_MULTIPLE_SLOVENIAN_LANGUAGE_SUPPORT << 4;
#endif

#ifdef SCREEN_TITLE_MULTIPLE_UKRAINIAN_LANGUAGE_SUPPORT//乌克兰 0x58  88
        response[9] |= SCREEN_TITLE_MULTIPLE_UKRAINIAN_LANGUAGE_SUPPORT << 7;
#endif
#ifdef SCREEN_TITLE_MULTIPLE_SWEDISH_LANGUAGE_SUPPORT//瑞典语（svenska） 0x4a
        response[10] |= SCREEN_TITLE_MULTIPLE_SWEDISH_LANGUAGE_SUPPORT << 1;
#endif
#ifdef SCREEN_TITLE_MULTIPLE_VIETNAMESE_LANGUAGE_SUPPORT
        response[7] |= SCREEN_TITLE_MULTIPLE_VIETNAMESE_LANGUAGE_SUPPORT << 2;
#endif
#ifdef SCREEN_TITLE_MULTIPLE_GREEK_LANGUAGE_SUPPORT
        response[8] |= SCREEN_TITLE_MULTIPLE_GREEK_LANGUAGE_SUPPORT << 1;
#endif
        uteModuleProfileBleSendToPhone(&response[0],20);
    }
    else if(receive[1]==0xab)
    {
        if(receive[2]<LANGUAGE_ID_MAX)
        {
#if UTE_MODULE_SCREENS_NO_FOLLOW_APP_LANGUAGE_SELECT_SUPPORT
            response[2] = receive[2];
            ute_module_systemtime_time_t set;
            uteModuleSystemtimeGetTime(&set);
            if(set.AppSetlanguageId==response[2])
            {

                response[2]=set.languageId;
                uteModuleSystemtimeSetLanguage(response[2]);

            }
            else
            {
                uteModuleSystemtimeSetLanguage(response[2]);
                uteModuleSystemtimeAPPSetLanguage(response[2]);
            }
#else
            response[2] = receive[2];
            uteModuleSystemtimeSetLanguage(receive[2]);
#endif
        }
        else
        {
            response[2] = uteModuleSystemtimeReadLanguage()&0xff;
        }
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
}

/**
*@brief        读取实时步数信息
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleProtocolReadReadlTimeStepInfo(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportSendRealTimeStepInfo();
#endif
}
/**
*@brief        发送计步历史数据
*@details         带时间戳参数
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleProtocolReadStepHistoryData(uint8_t*receive,uint8_t length)
{
    ute_module_systemtime_time_t time;
    memset(&time,0,sizeof(ute_module_systemtime_time_t));
    if(length>8)
    {
        time.year = receive[2]<<8|receive[3];
        time.month = receive[4];
        time.day = receive[5];
        time.hour = receive[6];
        time.min = receive[7];
        time.sec = receive[8];
    }
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportStartSendStepHistoryData(time);
#endif
}

/**
*@brief        查询消息内容支持
*@details         兼容指令
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleProtocolReadMsgContentSupport(uint8_t*receive,uint8_t length)
{
    uint8_t response[2];
    response[0] = receive[0];
    response[1] = 0x01;
    uteModuleProfileBleSendToPhone(&response[0],2);
}
/**
*@brief        获取当前快捷开关的状态
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-23
*/
void uteModuleProtocolReadQuickSwitchStatus(uint8_t*receive,uint8_t length)
{
    uint8_t response[20];
    uint32_t setFlag=0;
    memset(&response[0],0x00,20);
    response[0] = receive[0];
    response[1] = receive[1];
    if(receive[1] == 0x01) //获取支持的快捷开关
    {
#if QUICK_SWITCH_FINDBAND_SUPPORT
        setFlag|=QUICK_SWITCH_FINDBAND;
#endif
#if QUICK_SWITCH_TURNTHEWRIST_SUPPORT
        setFlag|=QUICK_SWITCH_TURNTHEWRIST;
#endif
#if QUICK_SWITCH_SEDENTARY_SUPPORT
        setFlag|=QUICK_SWITCH_SEDENTARY;
#endif
#if QUICK_SWITCH_NOT_DISTURB_SUPPORT
        setFlag|=QUICK_SWITCH_NOT_DISTURB;
#endif
#if QUICK_SWITCH_ANTILOST_SUPPORT
        setFlag|=QUICK_SWITCH_ANTILOST;
#endif
#if QUICK_SWITCH_REJECT_CALL_SUPPORT
        setFlag|=QUICK_SWITCH_REJECT_CALL;
#endif
#if QUICK_SWITCH_24H_HEART_SUPPORT
        setFlag|=QUICK_SWITCH_24H_HEART;
#endif
#if QUICK_SWITCH_DISPLAY_TIME_SUPPORT
        setFlag|=QUICK_SWITCH_DISPLAY_TIME;
#endif
#if QUICK_SWITCH_SHOCK_TIME_SUPPORT
        setFlag|=QUICK_SWITCH_SHOCK_TIME;
#endif

#if QUICK_SWITCH_GOAL_REACH_SUPPORT
        setFlag|=QUICK_SWITCH_GOAL_REACH;
#endif
#if QUICK_SWITCH_DRINK_WATER_SUPPORT
        setFlag|=QUICK_SWITCH_DRINK_WATER;
#endif
#if QUICK_SWITCH_HR_ABNORMAL_WARNNING_SUPPORT
        setFlag|=QUICK_SWITCH_HR_ABNORMAL_WARNING;
#endif
#if QUICK_SWITCH_LOCAL_WURAO_SET_TIME_SUPPORT
        setFlag|=QUICK_SWITCH_LOCAL_WURAO_SET_TIME;
#endif
#if QUICK_SWITCH_LOCAL_IS24HOUR_SUPPORT
        setFlag|=QUICK_SWITCH_LOCAL_IS24HOUR;
#endif

        response[2] = (setFlag>>24)&0xff;
        response[3] = (setFlag>>16)&0xff;
        response[4] = (setFlag>>8)&0xff;
        response[5] = (setFlag)&0xff;
        uteModuleProfileBleSendToPhone(&response[0],20);
    }
    else if(receive[1] == 0x02) // 获取手表快捷开关的状态
    {
        uteApplicationCommonSendQuickSwitchStatus();
    }
    else if(receive[1] == 0x03) // 设置手表快捷开关的状态或参数
    {
        uteApplicationCommonSetQuickSwitchStatusFromApp(&receive[2]);
    }
}
/**
*@brief        电话状态控制
*@details   仅仅针对android，不带通话的项目
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-24
*/
void uteModuleProtocolCallStatusCtrl(uint8_t*receive,uint8_t length)
{
    uint8_t response[2];
    response[0] = receive[0];
    response[1] = receive[1];
    if(receive[1] == 0x04) /*!挂断电话 zn.zeng, 2021-08-24  */
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_CALL_DISABLE_NOTIFY,0);
    }
    else if(receive[1] == 0x05)/*! 未接电话zn.zeng, 2021-08-24  */
    {
        // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_CALL_UNANSWERED_NOTIFY,0);
    }
    uteModuleProfileBleSendToPhone(&response[0],2);
}

/**
*@brief          摇一摇拍照消息处理
*@details
*@author
*@date
*/
void uteModuleTakePictureHandlerMsg(void)
{
    uint8_t response[2] = {CMD_TAKE_PICTURE_MODE,0x02};
    response[0] = CMD_TAKE_PICTURE_MODE;
    uteModuleProfileBleSendToPhone(&response[0],2);
}
/**
*@brief          摇一摇拍照打开app相机界面消息处理
*@details
*@author       dengli.lu
*@date       2022-08-23
*/

void uteModuleSportOpenAppTakePictureScreenHandler(void)
{
    uint8_t response[2];
    memset(&response[0],0,2);
    response[0] = CMD_SEND_KEYCODE;
    response[1] = 0x11;
    uteModuleProfileBleSendToPhone(&response[0],2);
}
/**
*@brief          摇一摇拍照退出app相机界面消息处理
*@details
*@author       dengli.lu
*@date       2022-08-23
*/

void uteModuleSportCloseAppTakePictureScreenHandler(void)
{
    uint8_t response[2];
    memset(&response[0],0,2);
    response[0] = CMD_SEND_KEYCODE;
    response[1] = 0x0F;
    uteModuleProfileBleSendToPhone(&response[0],2);
}
/**
*@brief        摇一摇拍照状态控制
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-24
*/
void uteModuleProtocolTakePictureCtrl(uint8_t*receive,uint8_t length)
{
    if(receive[1] == 0x01)
    {
#if DRV_HID_PHOTO
        uteModuleNotifyAncsClearQueueRestart();
#endif
#if UTE_MODULE_SPORT_SUPPORT
        uteModuleSportSetTakePictureEnable(true);
#endif
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
        uteTaskGuiStartScreen(FUNC_CAMERA, 0, __func__);
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
    }
    else
    {
        //  uteModuleGuiCommonGoBackLastScreen();
#if UTE_MODULE_SPORT_SUPPORT
        if(uteModuleSportIsTakePicture())
        {
            uteModuleSportSetTakePictureEnable(false);
            uteModuleGuiCommonGoBackLastScreen();
        }
#endif
    }
}
/**
*@brief        消息内容接收
*@details    android 使用，推送下来为unicode 编码
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-24
*/
void uteModuleProtocolMsgContent(uint8_t*receive,uint8_t length)
{
    ute_module_notify_data_t *notifyPointer = NULL;
    uteModuleNotifyGetDataPointer(&notifyPointer);
    static bool isReceiving = false;
    uint8_t response[4];
    if(receive[0]==CMD_MSG_CONTENT_MODE0)
    {
        if(receive[1]==0x00)
        {
            isReceiving = true;
            notifyPointer->currNotify.type = receive[2];
            notifyPointer->currNotify.size = length-4;
            memset(&notifyPointer->currNotify.content[0],0,UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
            memcpy(&notifyPointer->currNotify.content[0],&receive[4],length-4);
            response[0] = receive[0];
            response[1] = receive[1];
            uteModuleProfileBleSendToPhone(&response[0],2);
        }
        else if(receive[1]==0xfd)
        {
            response[0] = receive[0];
            response[1] = receive[1];
            response[2] = notifyPointer->currNotify.type;
            response[3] = notifyPointer->currNotify.size;
            if(isReceiving && notifyPointer->currNotify.type!=MSG_MMS)
            {
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_NOTIFY_NOTIFYCATTION,0);
            }
            uteModuleProfileBleSendToPhone(&response[0],4);
            isReceiving = false;
        }
        else
        {
            response[0] = receive[0];
            response[1] = receive[1];
            if(isReceiving && (notifyPointer->currNotify.size+length-2)<=UTE_NOTIFY_MSG_CONTENT_MAX_SIZE)
            {
                memcpy(&notifyPointer->currNotify.content[notifyPointer->currNotify.size],&receive[2],length-2);
                notifyPointer->currNotify.size = notifyPointer->currNotify.size+length-2;
            }
            uteModuleProfileBleSendToPhone(&response[0],2);
        }
    }
    else if(receive[0]==CMD_MSG_CONTENT_MODE1)
    {
        if(receive[1]==0x00)
        {
            if(!isReceiving)
            {
                notifyPointer->currNotify.type = receive[2];
                notifyPointer->currNotify.titleSize = length-4;
                memset(&notifyPointer->titleBuff[0],0,UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
                memcpy(&notifyPointer->titleBuff[0],&receive[4],length-4);
            }
            response[0] = receive[0];
            response[1] = receive[1];
            uteModuleProfileBleSendToPhone(&response[0],2);
        }
        else if(receive[1]==0xfd)
        {
            response[0] = receive[0];
            response[1] = receive[1];
            response[2] = notifyPointer->currNotify.type;
            response[3] = notifyPointer->currNotify.titleSize;
            uteModuleProfileBleSendToPhone(&response[0],4);
            if(!isReceiving)
            {
                notifyPointer->titleBuff[notifyPointer->currNotify.titleSize] = 0x00;
                notifyPointer->titleBuff[notifyPointer->currNotify.titleSize+1] = 0x3a;
                notifyPointer->currNotify.titleSize +=2;
                //title+nontent ,单单针对短信
                uint8_t cpySize = notifyPointer->currNotify.size;
                uint8_t tmpBuff[UTE_NOTIFY_MSG_CONTENT_MAX_SIZE];
                memset(&tmpBuff[0],0,UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
                if((cpySize+notifyPointer->currNotify.titleSize)>UTE_NOTIFY_MSG_CONTENT_MAX_SIZE)
                {
                    cpySize = UTE_NOTIFY_MSG_CONTENT_MAX_SIZE-notifyPointer->currNotify.titleSize;
                }
                memcpy(&tmpBuff[0],&notifyPointer->titleBuff[0],notifyPointer->currNotify.titleSize);
                memcpy(&tmpBuff[notifyPointer->currNotify.titleSize],&notifyPointer->currNotify.content[0],cpySize);
                notifyPointer->currNotify.size = cpySize+notifyPointer->currNotify.titleSize;
                memcpy(&notifyPointer->currNotify.content[0],&tmpBuff[0],notifyPointer->currNotify.size);
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_NOTIFY_NOTIFYCATTION,0);
            }
        }
        else
        {
            response[0] = receive[0];
            response[1] = receive[1];
            if(!isReceiving && (notifyPointer->currNotify.titleSize+length-2)<=UTE_NOTIFY_MSG_CONTENT_MAX_SIZE)
            {
                memcpy(&notifyPointer->titleBuff[notifyPointer->currNotify.titleSize],&receive[2],length-2);
                notifyPointer->currNotify.titleSize = notifyPointer->currNotify.titleSize+length-2;
            }
            uteModuleProfileBleSendToPhone(&response[0],2);
        }
    }
}
/**
*@brief        控制理论血压测试
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-25
*/
void uteModuleProtocolTheoryBloodpressureCtrl(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_BLOODPRESSURE_SUPPORT
    uint8_t response[3];
    if(receive[1]==0x00)
    {
        uteModuleBloodpressureStopSingleTesting();
    }
    else if(receive[1]==0x11)
    {
        uteModuleBloodpressureStartSingleTesting();
        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_BLOOD_PRESSURE_ID);
    }
    else if(receive[1]==0xdd)
    {
        response[0] = receive[0];
        response[1] = receive[1];
        if(uteModuleBloodpressureIsTesting())
        {
            response[2] = 0x11;
        }
        else
        {
            response[2] = 0xff;
        }
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
#if UTE_MODULE_BLOODPRESSURE_AUTO_TEST_SUPPORT
    else if (receive[1] == 0x02)
    {
        uteModuleBloodpressureSetAutoIntervalParam(receive[2] > 24 ? 0 : receive[2]);
        uteModuleProfileBleSendToPhone(&receive[0], 3);
    }
#endif
#if UTE_MODULE_BLOODPRESSURE_MORNING_TEST_SUPPORT
    else if (receive[1] == 0x03)
    {
        uteModuleBloodpressureSetMorningTestParam(receive[2] == 0x11 ? true : false);
        uteModuleProfileBleSendToPhone(&receive[0], 3);
    }
#endif
#if UTE_MODULE_BLOODPRESSURE_AUTO_TEST_TIME_BUCKET_SUPPORT
    else if (receive[1] == 0x04)
    {
        uint16_t startTimeHourMin = receive[3]<<8 | receive[4];
        uint16_t endTimeHourMin = receive[5]<<8 | receive[6];
        uteModuleBloodpressureSetAutoTestTimeBucketParam(receive[2],startTimeHourMin,endTimeHourMin);
        uteModuleProfileBleSendToPhone(&receive[0], 3);
    }
#endif
#if UTE_MODULE_BLOODPRESSURE_DESIGNATED_TIME_TEST_SUPPORT
    else if (receive[1] == 0x05)
    {
        uteModuleBloodpressureSetDesignatedTimeTestInfo(receive[2], receive[3], receive[4], receive[5]);
        uteModuleProfileBleSendToPhone(&receive[0], 3);
    }
#endif
#else
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,error undefine", __func__);
#endif
}
/**
*@brief        读取血压测试值
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-25
*/
void uteModuleProtocolReadTheoryBloodpressureHistoryData(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_BLOODPRESSURE_SUPPORT
    ute_module_systemtime_time_t time;
    memset(&time,0,sizeof(ute_module_systemtime_time_t));
    if(length>8)
    {
        time.year = receive[2]<<8|receive[3];
        time.month = receive[4];
        time.day = receive[5];
        time.hour = receive[6];
        time.min = receive[7];
        time.sec = receive[8];
    }
    uteModuleBloodpressureStartSendHistoryData(time);
#else
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,error undefine", __func__);
#endif
}

/**
*@brief        设置7天天气数据
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-25
*/
void uteModuleProtocolSetWeatherData(uint8_t*receive,uint8_t length)
{
    ute_module_weather_data_t data;
    uteModuleWeatherGetData(&data);
    if(receive[1]==0x01)
    {
        data.DayWeather[0] = receive[2]<<8|receive[3];
        data.fristDayPm25 = receive[7]<<8|receive[8];
        data.fristDayAqi= receive[9]<<8|receive[10];
#if UTE_MODULE_WEATHER_HUMIDITY_ULTRAVIOLET_SUPPORT
        data.fristDayWeatherHumidity = receive[19];
        data.fristDayWeatherUltraviolet = receive[20];
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,fristDayWeatherHumidity = %d,fristDayWeatherUltraviolet=%d", __func__,data.fristDayWeatherHumidity,data.fristDayWeatherUltraviolet);
#endif
        if(receive[4]&0x80)
        {
            data.fristDayCurrTemperature = -(receive[4]&~0x80);
        }
        else
        {
            data.fristDayCurrTemperature = receive[4];
        }
        if(receive[5]&0x80)
        {
            data.dayTemperatureMax[0] = -(receive[5]&~0x80);
        }
        else
        {
            data.dayTemperatureMax[0] = receive[5];
        }
        if(receive[6]&0x80)
        {
            data.dayTemperatureMin[0] = -(receive[6]&~0x80);
        }
        else
        {
            data.dayTemperatureMin[0] = receive[6];
        }
        uteModuleWeatherSetData(data);
    }
    else if(receive[1]==0x02)
    {
        for(uint8_t j=0; j<4; j++)
        {
            data.DayWeather[1+j] = receive[2+4*j]<<8|receive[2+4*j+1];
            if(receive[2+4*j+2]&0x80)
            {
                data.dayTemperatureMax[1+j] = -(receive[2+4*j+2]&~0x80);
            }
            else
            {
                data.dayTemperatureMax[1+j] = receive[2+4*j+2];
            }
            if(receive[2+4*j+3]&0x80)
            {
                data.dayTemperatureMin[1+j] = -(receive[2+4*j+3]&~0x80);
            }
            else
            {
                data.dayTemperatureMin[1+j] = receive[2+4*j+3];
            }
        }
        uteModuleWeatherSetData(data);
    }
    else if(receive[1]==0x03)
    {
        for(uint8_t j=0; j<2; j++)
        {
            data.DayWeather[5+j] = receive[2+4*j]<<8|receive[2+4*j+1];
            if(receive[2+4*j+2]&0x80)
            {
                data.dayTemperatureMax[5+j] = -(receive[2+4*j+2]&~0x80);
            }
            else
            {
                data.dayTemperatureMax[5+j] = receive[2+4*j+2];
            }
            if(receive[2+4*j+3]&0x80)
            {
                data.dayTemperatureMin[5+j] = -(receive[2+4*j+3]&~0x80);
            }
            else
            {
                data.dayTemperatureMin[5+j] = receive[2+4*j+3];
            }
        }
        uteModuleSystemtimeGetTime(&data.updateTime);
        data.currDay = data.updateTime.day;
        uteModuleWeatherSetData(data);
    }
#if UTE_MODULE_WEATHER_CITY_NAME_NEW_SUPPORT
    else if(receive[1]==0xff)
    {
        uteModuleWeatherSetCityName(&receive[3],receive[2]);
    }
#endif
    uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],2);
}
/**
*@brief        按键事件设置
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-25
*/
void uteModuleProtocolSendKeycode(uint8_t*receive,uint8_t length)
{
    if (receive[1] == 0x0A) //find phone
    {
        if (length == 4) /*! 手机停止的时候会返回停止响铃指令，xjc 2022-01-18*/
        {
            if (receive[2] == 0x01)
            {
                uint8_t response[4];
                if (receive[3] == 0x01)
                {
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
                    uteModuleFindPhoneSetStatus(FIND_PHONE_RING);
#endif // UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
                }
                else if (receive[3] == 0x00)
                {
                    uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
                }
                response[0] = receive[0];
                response[1] = receive[1];
                response[2] = receive[2];
                response[3] = 0xFD;
                uteModuleProfileBleSendToPhone((uint8_t *)&response[0], 4);
            }
        }
        else
        {
            uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],3);
        }
    }
    /*! 配合 ams 功能，同步播放状态 和音量 zn.zeng, 2021-08-26  */
    else if(receive[1] == 0x0C)
    {
        if(receive[2] == 0x01)
        {
            uteModuleMusicSetPlayerPaused(false,UTE_MUSIC_PLAY_CHANNEL_PHONE_TO_SPEAKER);
        }
        else if(receive[2] == 0x02)
        {
            uteModuleMusicSetPlayerPaused(true,UTE_MUSIC_PLAY_CHANNEL_PHONE_TO_SPEAKER);
        }
        receive[2] = 0x00;
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],3);
    }
    else if (receive[1] == 0x0D || receive[1] == 0x0E) // volume increase decrease
    {
        if (receive[2] <= 100)
        {
#if BT_ID3_TAG_EN
            if (!bt_a2dp_profile_completely_connected())
#endif
            {
                uteModuleMusicSetPlayerVolume(receive[2]);
            }
        }
    }
}
/**
*@brief       iOS ANCS消息推送开关设置
*@details    单单iOS使用
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-25
*/
void uteModuleProtocoliOSAncsNotifySwitch(uint8_t*receive,uint8_t length)
{
    uint32_t flag = 0;
    uteModuleNotifyAncsGetFlag(&flag);
    if(receive[1]==0x01)//open
    {
        flag |= ((receive[2]<<24 ) |(receive[3]<<16 ) |(receive[4]<<8 ) |(receive[5]));
    }
    else if(receive[1]==0x02)//close
    {
        flag&= ~((receive[2]<<24 ) |(receive[3]<<16 ) |(receive[4]<<8 ) |(receive[5]));
    }
    uteModuleNotifyAncsSetFlag(flag);
//      #if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
//      uint8_t ancsAdditionalOpenFlag[18]={0};
//      memcpy(&receive[0],&ancsAdditionalOpenFlag[0],18);
//      #endif
    uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],length);
}

/**
*@brief     支持应用通知选择显示功能
*@details
*@param[in] uint8_t *receive
*@param[in] uint8_t length
*@author    xjc
*@date      2022-01-08
*/
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
void uteModuleProtocolSocialAppSelectParam(uint8_t*receive,uint8_t length)
{
    uint32_t flag = 0;
    uint8_t responseData[20];
    memset(responseData, 0x00, 20);
//    uteModuleNotifyAncsGetFlag(&flag);
    switch (receive[1])
    {
        case 0x01://open
        {
            // UTE_MODULE_LOG(1,"open receive[16-19] = 0x%02x,0x%02x,0x%02x,0x%02x",receive[16],receive[17],receive[18],receive[19]);
            flag |= ((receive[16] << 24) | (receive[17] << 16) | (receive[18] << 8) | (receive[19]));
            uteModuleNotifyAncsSetFlag(flag);
            uteModuleNotifyAncsSetAdditionalFlag(true,(uint8_t *)&receive[0], length);
            uteModuleProfileBleSendToPhone((uint8_t *)&receive[0], length);
        }
        break;
        case 0x02://close
        {
            // UTE_MODULE_LOG(1,"close receive[16-19] = 0x%02x,0x%02x,0x%02x,0x%02x",receive[16],receive[17],receive[18],receive[19]);
            flag &= ~((receive[16] << 24) | (receive[17] << 16) | (receive[18] << 8) | (receive[19]));
            uteModuleNotifyAncsSetFlag(flag);
            uteModuleNotifyAncsSetAdditionalFlag(false,(uint8_t *)&receive[0], length);
            uteModuleProfileBleSendToPhone((uint8_t *)&receive[0], length);
        }
        break;
        case 0xAA: //search
        {
            responseData[0] = receive[0];
            responseData[1] = receive[1];

            responseData[19] |= ANCS_OPEN_SMS;
            responseData[19] |= ANCS_OPEN_QQ;
            responseData[19] |= ANCS_OPEN_WECHAT;
            responseData[19] |= ANCS_OPEN_INCALL;
            responseData[19] |= ANCS_OPEN_OTHER;
            responseData[19] |= ANCS_OPEN_FACEBOOK;
            responseData[19] |= ANCS_OPEN_TWITTER;
            responseData[19] |= ANCS_OPEN_WHATSAPP;

            responseData[18] |= ANCS_OPEN_MESSENGER;
            responseData[18] |= ANCS_OPEN_LINE;
            responseData[18] |= ANCS_OPEN_SKYPE;
            responseData[18] |= ANCS_OPEN_HANGOUTS;
            responseData[18] |= ANCS_OPEN_LINKEDIN;
            responseData[18] |= ANCS_OPEN_INSTAGRAM;
            responseData[18] |= ANCS_OPEN_VIBER;
            responseData[18] |= ANCS_OPEN_KAKAOTALK;

            responseData[17] |= ANCS_OPEN_VKONTAKTE;
            responseData[17] |= ANCS_OPEN_SNAPCHAT;
            responseData[17] |= ANCS_OPEN_GOOGLEPLUS;
            responseData[17] |= ANCS_OPEN_GMAIL;
            responseData[17] |= ANCS_OPEN_FLICKR;
            responseData[17] |= ANCS_OPEN_TUMBLR;
            responseData[17] |= ANCS_OPEN_PINTEREST;
            responseData[17] |= ANCS_OPEN_YOUTUBE;

            responseData[16] |= ANCS_OPEN_TELEGRAM;
            responseData[16] |= ANCS_OPEN_TRUECALLER;
            responseData[16] |= ANCS_OPEN_PAYTM;
            responseData[16] |= ANCS_OPEN_ZALO;
            responseData[16] |= ANCS_OPEN_IMO;
            responseData[16] |= ANCS_OPEN_MICROSOFTTEAMS;
            responseData[16] |= ANCS_OPEN_OUTLOOK;
            responseData[16] |= ANCS_OPEN_SWIGGY;

            responseData[15] |= ANCS_OPEN_ZOMATO;
            responseData[15] |= ANCS_OPEN_GPAY;
            responseData[15] |= ANCS_OPEN_PHONEPE;
            responseData[15] |= ANCS_OPEN_HOTSTAR;
            responseData[15] |= ANCS_OPEN_PRIMEVIDEO;
            responseData[15] |= ANCS_OPEN_FLIPKART;
            responseData[15] |= ANCS_OPEN_AMAZON;
            responseData[15] |= ANCS_OPEN_MYNTRA;

            responseData[14] |= ANCS_OPEN_NOISEAPP;
            responseData[14] |= ANCS_OPEN_DAILYHUNT;
            responseData[14] |= ANCS_OPEN_INSHORTS;
            responseData[14] |= ANCS_OPEN_BOOKMYSHOW;

            responseData[14] |= ANCS_OPEN_CALENDAR;
            responseData[14] |= ANCS_OPEN_JIOTV;
            responseData[14] |= ANCS_OPEN_MAKEMYTRIP;
            responseData[14] |= ANCS_OPEN_NETFLIX;

            responseData[13] |= ANCS_OPEN_OLA;
            responseData[13] |= ANCS_OPEN_REFLEXAPP;
            responseData[13] |= ANCS_OPEN_UBER;
            responseData[13] |= ANCS_OPEN_YTMUSIC;
            responseData[13] |= ANCS_OPEN_WHATSAPP_BUSINESS;
            responseData[13] |= ANCS_OPEN_DUNZO;
            responseData[13] |= ANCS_OPEN_GAANA;
            responseData[13] |= ANCS_OPEN_GOOGLE_DRIVE;

            responseData[12] |= ANCS_OPEN_GOOGLECHAT;
            responseData[12] |= ANCS_OPEN_WYNKMUSIC;
            responseData[12] |= ANCS_OPEN_YAHOO;
            responseData[12] |= ANCS_OPEN_TITANSMARTWORLD;
            responseData[12] |= ANCS_OPEN_SLACK;
            responseData[12] |= ANCS_OPEN_SPOTIFY;


            uteModuleProfileBleSendToPhone(&responseData[0], 20);
        }
    }
}
#else
void uteModuleProtocolSocialAppSelectParam(uint8_t*receive,uint8_t length)
{
    uint32_t flag = 0;
    uint8_t responseData[20];
    memset(responseData, 0x00, 20);
    uteModuleNotifyAncsGetFlag(&flag);
    switch (receive[1])
    {
        case 0x01://open
        {
            // UTE_MODULE_LOG(1,"open receive[16-19] = 0x%02x,0x%02x,0x%02x,0x%02x",receive[16],receive[17],receive[18],receive[19]);
            flag |= ((receive[16] << 24) | (receive[17] << 16) | (receive[18] << 8) | (receive[19]));
            uteModuleNotifyAncsSetFlag(flag);
            uteModuleProfileBleSendToPhone((uint8_t *)&receive[0], length);
        }
        break;
        case 0x02://close
        {
            // UTE_MODULE_LOG(1,"close receive[16-19] = 0x%02x,0x%02x,0x%02x,0x%02x",receive[16],receive[17],receive[18],receive[19]);
            flag &= ~((receive[16] << 24) | (receive[17] << 16) | (receive[18] << 8) | (receive[19]));
            uteModuleNotifyAncsSetFlag(flag);
            uteModuleProfileBleSendToPhone((uint8_t *)&receive[0], length);
        }
        break;
        case 0xAA: //search
        {
            responseData[0] = receive[0];
            responseData[1] = receive[1];

            responseData[19] |= SOCIAL_APP_MMS_SUPPORT << 0;
            responseData[19] |= SOCIAL_APP_QQ_SUPPORT << 1;
            responseData[19] |= SOCIAL_APP_WECHAT_SUPPORT << 2;
            responseData[19] |= SOCIAL_APP_CALL_SUPPORT << 3;
            responseData[19] |= SOCIAL_APP_OTHER_SUPPORT << 4;
            responseData[19] |= SOCIAL_APP_FACEBOOK_SUPPORT << 5;
            responseData[19] |= SOCIAL_APP_TWITTER_SUPPORT << 6;
            responseData[19] |= SOCIAL_APP_WHATSAPP_SUPPORT << 7;

            responseData[18] |= SOCIAL_APP_FACEBOOK_MESSENGER_SUPPORT << 0;
            responseData[18] |= SOCIAL_APP_LINE_SUPPORT << 1;
            responseData[18] |= SOCIAL_APP_SKYPE_SUPPORT << 2;
            responseData[18] |= SOCIAL_APP_HANGOUTS_SUPPORT << 3;
            responseData[18] |= SOCIAL_APP_LINKED_IN_SUPPORT << 4;
            responseData[18] |= SOCIAL_APP_INSTAGRAM_SUPPORT << 5;
            responseData[18] |= SOCIAL_APP_VIBER_SUPPORT << 6;
            responseData[18] |= SOCIAL_APP_KAKAO_TALK_SUPPORT << 7;

            responseData[17] |= SOCIAL_APP_VKONTAKTE_SUPPORT << 0;
            responseData[17] |= SOCIAL_APP_SNAPCHAT_SUPPORT << 1;
            responseData[17] |= SOCIAL_APP_GOOGLE_PLUS_SUPPORT << 2;
            responseData[17] |= SOCIAL_APP_GMAIL_SUPPORT << 3;
            responseData[17] |= SOCIAL_APP_FLICKR_SUPPORT << 4;
            responseData[17] |= SOCIAL_APP_TUMBLR_SUPPORT << 5;
            responseData[17] |= SOCIAL_APP_PINTEREST_SUPPORT << 6;
            responseData[17] |= SOCIAL_APP_YOUTUBE_SUPPORT << 7;

            responseData[16] |= SOCIAL_APP_TELEGRAM_SUPPORT << 0;
            responseData[16] |= SOCIAL_APP_ZALO_SUPPORT << 3;
            uteModuleProfileBleSendToPhone(&responseData[0], 20);
        }
    }
}
#endif

/**
*@brief       设置久坐提醒参数
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-25
*/
void uteModuleProtocolSetSedentaryRemind(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_SPORT_SUPPORT
    ute_module_sport_sedentary_param_t param;
    memset(&param,0,sizeof(ute_module_sport_sedentary_param_t));
    if(receive[1]==0x01)
    {
        param.isOpen = true;
        param.setMin = receive[2];
        param.sedentaryRemindSecCnt = 0;
        if(length >7)
        {
            param.startPeriodHour = receive[7];
            param.startPeriodMin= receive[8];
            param.endPeriodHour= receive[9];
            param.endPeriodMin= receive[10];
            param.isOpenNoonSleep =  receive[11];
#if UTE_MODULE_SEDENTARY_NOTIFY_SET_NOONSLEEP_TIME
            param.noonSleepStartTime = receive[12]*60+receive[13];
            param.noonSleepEndTime = receive[14]*60+receive[15];
#endif
        }
    }
    else
    {
        param.isOpen = false;
    }
    uteModuleProfileBleSendToPhone(&receive[0],2);
    uteModuleSportSaveSedentaryParam(&param);
#endif
}
/**
*@brief       设置勿扰参数
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-25
*/
void uteModuleProtocolSetNotDisturParam(uint8_t*receive,uint8_t length)
{
    ute_module_not_disturb_data_t param;
    memset(&param,0,sizeof(ute_module_not_disturb_data_t));
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    uteModuleNotDisturbGetParam(&param);
#endif
    if(receive[1]&0x08)
    {
        param.isRejectCall = true;
    }
    else
    {
        param.isRejectCall = false;
    }
#if (!UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT)
    if(receive[1]&0x04)
    {
        param.isNormalNotNotify = true;
    }
    else
    {
        param.isNormalNotNotify = false;
    }
    if(receive[1]&0x02)
    {
        param.isNormalNotVibration = true;
    }
    else
    {
        param.isNormalNotVibration = false;
    }
    param.startHourMin = receive[2]<<8|receive[3];
    param.endHourMin= receive[4]<<8|receive[5];
    if(receive[6]==0x01)
    {
        param.isOpen = true;
    }
    else
    {
        param.isOpen = false;
    }
#endif
    uteModuleProfileBleSendToPhone(&receive[0],length);
    uteModuleNotDisturbSaveParam(param);
}
/**
*@brief       应用端通知手环前后台状态
*@details   暂未使用
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleProtocolFormAppFrontOrBack(uint8_t*receive,uint8_t length)
{

}
/**
*@brief       心率测试
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleProtocolHeartTestCtrl(uint8_t*receive,uint8_t length)
{
    uint8_t response[3];
    response[0] = receive[0];
    response[1] = receive[1];
    if(receive[1]==0x11)
    {
#if UTE_MODULE_HEART_SUPPORT
        uteModuleHeartStartSingleTesting(TYPE_HEART);
#endif
    }
    else if(receive[1]==0x00)
    {
#if UTE_MODULE_HEART_SUPPORT
        uteModuleHeartStopSingleTesting(TYPE_HEART);
#endif
    }
    else if(receive[1]==0xaa)
    {
#if UTE_MODULE_HEART_SUPPORT
        if(uteModuleHeartIsNeedTesting())
        {
            response[2] = 0x11;
        }
        else
        {
            response[2] = 0xff;
        }
#endif
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
}
/**
*@brief       gsenoser数据挤压检查指令
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleProtocolCheckGsensorData(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_SLEEP
    uint8_t response[10];
    int16_t xx,yy,zz,accvalue;
    uteDrvGsensorCommonGetAccXyz(&xx,&yy,&zz);
    ute_drv_gsensor_common_axis_bit_change_t bitChange;
    bitChange.inputXaxis = &xx;
    bitChange.inputYaxis = &yy;
    bitChange.inputZaxis = &zz;
    bitChange.outputXaxis = &xx;
    bitChange.outputYaxis = &yy;
    bitChange.outputZaxis = &zz;
    uteDrvGsensorCommonXYZaxisDataBitChange(&bitChange,1,GSENSOR_DATA_BIT_STEP);
    accvalue = SqrtByBisection(xx*xx+yy*yy+zz*zz);
    response[0] = CMD_FACTORY_GSENSOR_TEST;
    response[1] = uteDrvGsensorCommonGetAccRange();
    response[2] = (xx>>8)&0xff;
    response[3] = xx&0xff;
    response[4] = (yy>>8)&0xff;
    response[5] = yy&0xff;
    response[6] = (zz>>8)&0xff;
    response[7] = zz&0xff;
    response[8] = (accvalue>>8)&0xff;
    response[9] = accvalue&0xff;
    uteModuleProfileBleSendToPhone(&response[0],10);
#endif
}

/**
*@brief       关机指令
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleProtocolSetPowerOff(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_APP_CONTROL_RESTART_SUPPORT//dengli.lu 手机控制手环重启
    uint8_t response[2];
    response[0] = receive[0];
    response[1] = receive[1];
    if(receive[1]==0x12)
    {
        uteModuleProfileBleSendToPhone((uint8_t *)&response[0],2);
        uteApplicationCommonRestart();
    }
    else
#endif
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_START_POWER_OFF,0);
    }
}
/**
*@brief       心率24小时自动测试历史数据
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleProtocolHeartAutoTestHistoryData(uint8_t*receive,uint8_t length)
{
    if(receive[1]==0x01)
    {
#if UTE_MODULE_HEART_SUPPORT
        uteModuleHeartSetAutoTesting(true);
#endif
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],2);
    }
    else if(receive[1]==0x02)
    {
#if UTE_MODULE_HEART_SUPPORT
        uteModuleHeartSetAutoTesting(false);
#endif
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],2);
    }
    else if(receive[1]==0xfa)
    {
        ute_module_systemtime_time_t time;
        memset(&time,0,sizeof(ute_module_systemtime_time_t));
        if(length>8)
        {
            time.year = receive[2]<<8|receive[3];
            time.month = receive[4];
            time.day = receive[5];
            time.hour = receive[6];
            time.min = receive[7];
            time.sec = receive[8];
        }
#if UTE_MODULE_HEART_SUPPORT
        uteModuleHeartStartSendHeartAutoTestHistoryData(time);
#endif
    }
#if APP_MODULE_HEART_RESTING_HEARTRATE_SUPPORT
    else if (receive[1] == 0x05)
    {
        uteModuleHeartRestingHeartrateSend();
    }
    else if (receive[1] == 0x07)
    {
        ute_module_systemtime_time_t time;

        uteModuleSystemtimeGetTime(&time);
        uteModuleHeartSendEveryHourMaxMInHeartData(time.hour);
    }
    else if (receive[1] == 0x08)
    {
        uteModuleHeartSendHistoryRestingHeartData();
    }
#endif
}
/**
*@brief       血氧控制和数据同步
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleProtocolBloodoxygenCtrl(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
    if(receive[1]==0x11)
    {
        if(uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE || uteModuleSportMoreSportGetStatus() != ALL_SPORT_STATUS_CLOSE)
        {
            uint8_t stopCmd[5];
            stopCmd[0] =CMD_SPO2_TEST;
            stopCmd[1] =0x00;
            stopCmd[2] =0xff;
            stopCmd[3] =0xff;
            uteModuleProfileBleSendToPhone(&stopCmd[0],4);
            return;
        }
        //每次测量重新开始，以防用户想测试低氧下不来
        if(uteModuleBloodoxygenIsTesting())
        {
            uteModuleBloodoxygenStopSingleTesting();
        }
        if(!uteModuleGuiCommonIsDisplayOn() || uteModuleGuiCommonGetCurrentScreenId() != FUNC_BLOOD_OXYGEN)
        {
            uteTaskGuiStartScreen(FUNC_BLOOD_OXYGEN, 0, __func__);
        }
        uteModuleBloodoxygenStartSingleTesting();
    }
    else if(receive[1]==0x00)
    {
        uteModuleBloodoxygenStopSingleTesting();
    }
    else if(receive[1]==0x03)/*! 自动测试开关+采样间隔 zn.zeng, 2021-08-30  */
    {
        bool isAutoTesting = receive[2];
        uint16_t intervalMin = receive[3]<<8|receive[4];
        uteModuleBloodoxygenSaveAutoIntervalParam(isAutoTesting,intervalMin);
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],5);
    }
    else if(receive[1]==0x04)/*!设置自动测试时间段 zn.zeng, 2021-08-30  */
    {
        bool isAutoTimeBucketTesting = receive[2];
        uint16_t startTimeHourMin = receive[3]<<8|receive[4];
        uint16_t endTimeHourMin = receive[5]<<8|receive[6];
        uteModuleBloodoxygenSaveAutoTimeBucketParam(isAutoTimeBucketTesting,startTimeHourMin,endTimeHourMin);
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],7);
    }
    else if(receive[1]==0xaa)/*! 查询测试状态  zn.zeng, 2021-08-30  */
    {
        uint8_t response[3];
        response[0] = receive[0];
        response[1] = receive[1];
        if(uteModuleBloodoxygenIsTesting())
        {
            response[2] = 0x11;
        }
        else
        {
            response[2] = 0xff;
        }
        uteModuleProfileBleSendToPhone((uint8_t *)&response[0],3);
    }
    else if(receive[1]==0x0c)/*! 删除历史数据  zn.zeng, 2021-08-30  */
    {
        uteModuleBloodoxygenDelHistoryData();
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],2);
    }
    else if(receive[1]==0xfa)/*! 同步历史数据 zn.zeng, 2021-08-31  */
    {
        ute_module_systemtime_time_t time;
        memset(&time,0,sizeof(ute_module_systemtime_time_t));
        if(length>8)
        {
            time.year = receive[2]<<8|receive[3];
            time.month = receive[4];
            time.day = receive[5];
            time.hour = receive[6];
            time.min = receive[7];
            time.sec = receive[8];
        }
        uteModuleBloodoxygenStartSendAutoTestHistoryData(time);
    }
#else
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,undefine ", __func__);
#endif

}
/**
*@brief       呼吸率控制和数据同步
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleProtocolBreathrateCtrl(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_BREATHRATE_SUPPORT
    if(receive[1]==0x11)
    {
        uteModuleBreathrateStartSingleTesting();
    }
    else if(receive[1]==0x00)
    {
        uteModuleBreathrateStopSingleTesting();
    }
    else if(receive[1]==0x03)/*! 自动测试开关+采样间隔 zn.zeng, 2021-08-30  */
    {
        bool isAutoTesting = receive[2];
        uint16_t intervalMin = receive[3]<<8|receive[4];
        uteModuleBreathrateSaveAutoIntervalParam(isAutoTesting,intervalMin);
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],5);
    }
    else if(receive[1]==0x04)/*!设置自动测试时间段 zn.zeng, 2021-08-30  */
    {
        bool isAutoTimeBucketTesting = receive[2];
        uint16_t startTimeHourMin = receive[3]<<8|receive[4];
        uint16_t endTimeHourMin = receive[5]<<8|receive[6];
        uteModuleBreathrateSaveAutoTimeBucketParam(isAutoTimeBucketTesting,startTimeHourMin,endTimeHourMin);
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],7);
    }
    else if(receive[1]==0xaa)/*! 查询测试状态  zn.zeng, 2021-08-30  */
    {
        uint8_t response[3];
        response[0] = receive[0];
        response[1] = receive[1];
        if(uteModuleBreathrateIsTesting())
        {
            response[2] = 0x11;
        }
        else
        {
            response[2] = 0xff;
        }
        uteModuleProfileBleSendToPhone((uint8_t *)&response[0],3);
    }
    else if(receive[1]==0x0c)/*! 删除历史数据  zn.zeng, 2021-08-30  */
    {
        uteModuleBreathrateDelHistoryData();
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],2);
    }
    else if(receive[1]==0xfa)/*! 同步历史数据 zn.zeng, 2021-08-31  */
    {
        ute_module_systemtime_time_t time;
        memset(&time,0,sizeof(ute_module_systemtime_time_t));
        if(length>8)
        {
            time.year = receive[2]<<8|receive[3];
            time.month = receive[4];
            time.day = receive[5];
            time.hour = receive[6];
            time.min = receive[7];
            time.sec = receive[8];
        }
        uteModuleBreathrateStartSendAutoTestHistoryData(time);
    }
#else
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,undefine ", __func__);
#endif

}

/**
*@brief       读取睡眠历史数据
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-10-26
*/
void uteModuleProtocolSleepReadHistoryData(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_SLEEP
    if(receive[1]==0x01)
    {
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        uteModuleSleepStartSendSleepHistoryData(time);
    }
#if UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT
    else if(receive[1]==0x03)
    {
        uteModuleSleepStartSendSleepSampleData();
    }
#endif
#endif
}

#if UTE_MODULE_TEMPERATURE_SUPPORT
/**
*@brief       体温相关协议控制指令
*@details
*@param[in] uint8_t *receive
*@param[in] uint8_t length
*@author       casen
*@date       2021-12-03
*/
void uteModuleProtocolTemperatureCtrl(uint8_t*receive,uint8_t length)
{
    if(receive[1]==0x01)
    {
        uteModuleTemperatureStartTesting(TEMPERATURE_MODE_APP_SINGLE);
        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TEMPERATURE_ID);
    }
    else if(receive[1]==0x03)/*! 自动测试开关+测试间隔 zn.zeng, 2021-12-03  */
    {
        uint32_t second = (receive[3]<<8|receive[4])*60;
        uteModuleTemperatureSetAutoTestInterval(receive[2],second);
        uteModuleProfileBleSendToPhone(&receive[0],4);
    }
    else if(receive[1]==0x04)//设置自动测试时间段
    {
        uint32_t startSecond = receive[3]*3600+receive[4]*60;
        uint32_t stopSecond = receive[5]*3600+receive[6]*60;
        uteModuleTemperatureSetAutoTestLimitTime(receive[2],startSecond,stopSecond);
        uteModuleProfileBleSendToPhone(&receive[0],7);
    }
    else if(receive[1]==0x05)//删除历史数据
    {
        uteModuleTemperatureDelHistoryData();
        uteModuleProfileBleSendToPhone((uint8_t *)&receive[0],2);
    }
    else if(receive[1]==0x06)//设置警告参数
    {
        uint16_t height = (receive[3]<<8|receive[4]);
        uint16_t low = (receive[5]<<8|receive[6]);
        uteModuleTemperatureSetWarningParam(receive[2],low,height);
        uteModuleProfileBleSendToPhone(&receive[0],7);
    }
    else if(receive[1]==0xfa)//读取历史数据
    {
        ute_module_systemtime_time_t time;
        memset(&time,0,sizeof(ute_module_systemtime_time_t));
        if(length>2)
        {
            time.year = receive[2]<<8|receive[3];
            time.month = receive[4];
            time.day = receive[5];
            time.hour = receive[6];
            time.min = receive[7];
            time.sec = receive[8];
        }
        uteModuleTemperatureStartSendHistoryData(time);
    }
}
#endif

/**
*@brief       多运动控制指令
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-11-13
*/
void uteModuleProtocolMoreSportCtrl(uint8_t*receive,uint8_t length)
{
#if UTE_MODULE_SPORT_SUPPORT
    uint8_t response[20];
    memset(response,0x00,20);
    bool isAppStart = uteModuleSportMoreSportIsAppStart();
    uint8_t sportStatus = uteModuleSportMoreSportGetStatus();
    if(receive[1]==0x11)//open sport
    {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT/* ellison add ,2022-Jun-14 低电量不给启动运动，并且显示低电量提醒 */
        uint8_t batLvl = uteDrvBatteryCommonGetLvl();
        if(batLvl < UTE_DRV_BATTERY_LOW_POWER_PERECNT)
        {
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_COVER_LOW_BATTERY;
            sys_cb.remind_tag = true;
//            msgbox((char*)i18n[STR_LOW_BATTERY], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_LOW_BATTERY);
            return;
        }
#endif
        if(uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE)
        {
            // memcpy(&response[0],&receive[0],4);
            // response[1] = ALL_SPORT_STATUS_CLOSE;
            // uteModuleProfileBleSendToPhone(&response[0],4);
            return;
        }
        uteModuleSportStartMoreSports(receive[2],receive[3],true);
    }
    else if(receive[1]==0x00)//close sport
    {
        if(sportStatus==ALL_SPORT_STATUS_CLOSE||(isAppStart==false))
        {
            memcpy(&response[0],&receive[0],4);
            response[1] = sportStatus;
            uteModuleProfileBleSendToPhone(&response[0],4);
        }
        else
        {
            uteModuleSportStopMoreSports();
        }
    }
    else if(receive[1]==0xaa)
    {
        response[0] = receive[0];
        response[1] = 0xAA;
        response[2] = sportStatus;
        response[3] =uteModuleSportMoreSportGetType();
        uteModuleProfileBleSendToPhone(&response[0],4);
    }
    else if(receive[1]==ALL_SPORT_STATUS_PAUSE)
    {
        if(isAppStart)
        {
            uteModuleSportMoreSportSetStatus(ALL_SPORT_STATUS_PAUSE);
            uteModuleSportSyncAppMoreSportData(receive);
            uteModuleProfileBleSendToPhone(&receive[0],13);
            //跳转运动暂停界面
            // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORTS_PAUSED_ID);

        }
    }
    else if(receive[1]==ALL_SPORT_STATUS_CONTINUE)
    {
        if(isAppStart)
        {
            if((sportStatus!=ALL_SPORT_STATUS_OPEN)&&(sportStatus>0))
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                //跳转运动详情界面
                // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SPORTS_DETAIL_ID);
            }
            uteModuleSportMoreSportSetStatus(ALL_SPORT_STATUS_CONTINUE);
            uteModuleSportSyncAppMoreSportData(receive);
            uteModuleProfileBleSendToPhone(&receive[0],13);
        }
    }
    else if(receive[1]==ALL_SPORT_STATUS_UPDATE_DATA)
    {
        if(sportStatus==ALL_SPORT_STATUS_PAUSE)
        {
            //如果是暂停状态,就不在更新时间
            return;
        }
        if(isAppStart)
        {
            uteModuleSportMoreSportSetStatus(ALL_SPORT_STATUS_UPDATE_DATA);
            uteModuleSportSyncAppMoreSportData(receive);
            uteModuleProfileBleSendToPhone(&receive[0],13);
        }
    }
    else if(receive[1]==0xfa)
    {
        ute_module_systemtime_time_t time;
        memset(&time,0,sizeof(ute_module_systemtime_time_t));
        if(length == 8)
        {
            time.year = receive[2]<<8|receive[3];
            time.month = receive[4];
            time.day = receive[5];
            time.hour = receive[6];
            time.min = receive[7];
        }
        uteModuleSportStartSendMoreSportHistoryData(time);
    }
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    else if(receive[1]==0x48) /*! 自定义运动列表显示功能，xjc 2022-03-29*/
    {
        uteModuleSportHundredSportCmd(receive,length);
    }
#endif
#endif
}

/**
*@brief       android 音乐控制指令
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-11-20
*/
void uteModuleProtocolMusicCtrl(uint8_t*receive,uint8_t length)
{
    ute_module_music_data_t musicData;
    uteModuleMusicGetPlayerData(&musicData);
    if(receive[1]==0x01)
    {
        musicData.androidCrc = 0;
        if(receive[2]==0x01) //paused
        {
            musicData.isPaused = true;
        }
        else if(receive[2]==0x02) //play
        {
            musicData.isPaused = false;
        }
        if(receive[3] != 0xFF)
        {
//            musicData.playerMode = receive[3];
            musicData.loopMode = (ute_module_music_loop_mode_t)receive[3];
        }
        if(receive[4] != 0xFF)
        {
            musicData.volume= receive[4];
        }
        if((*((uint32_t *)&receive[5]) != 0xFFFFFFFF) && (*((uint32_t *)&receive[9]) != 0xFFFFFFFF))
        {
            musicData.playerTatolTimeMs = receive[5]<<24|receive[6]<<16|receive[7]<<8|receive[8];
            musicData.playerCurrTimeMs = receive[9]<<24|receive[10]<<16|receive[11]<<8|receive[12];
        }
        for(uint8_t i = 0; i<length; i++)
        {
            musicData.androidCrc^=receive[i];
        }
        uteModuleMusicSetPlayerData(musicData);
    }
    else if(receive[1]==0x02)
    {
//        musicData.playerMode = receive[2];
        musicData.loopMode = (ute_module_music_loop_mode_t)receive[2];
        for(uint8_t i = 0; i<length; i++)
        {
            musicData.androidCrc^=receive[i];
        }
    }
    else if(receive[1]==0x03)
    {
        musicData.playerTatolTimeMs = receive[2]<<24|receive[3]<<16|receive[4]<<8|receive[5];
        musicData.playerCurrTimeMs = receive[6]<<24|receive[7]<<16|receive[8]<<8|receive[9];
        for(uint8_t i = 0; i<length; i++)
        {
            musicData.androidCrc^=receive[i];
        }
    }
    else if(receive[1]==0x04)
    {
        for(uint8_t i = 0; i<length; i++)
        {
            musicData.androidCrc^=receive[i];
        }
        uteModuleMusicSetPlayerData(musicData);
        uteModuleMusicSetPlayerArtist(&receive[2],length-2);
    }
    else if(receive[1]==0x05)
    {
        for(uint8_t i = 0; i<length; i++)
        {
            musicData.androidCrc^=receive[i];
        }
        uteModuleMusicSetPlayerData(musicData);
    }
    else if(receive[1]==0x06)
    {
        for(uint8_t i = 0; i<length; i++)
        {
            musicData.androidCrc^=receive[i];
        }
        uteModuleMusicSetPlayerData(musicData);
        uteModuleMusicSetPlayerTitle(&receive[3],length-3);
    }
    else if(receive[1]==0xfa)
    {
        uint8_t response[3];
        response[0] = receive[0];
        response[1] = receive[1];
        response[2] = musicData.androidCrc;
        uteModuleProfileBleSendToPhone(&response[0],3);
        uteModuleMusicSetPlayerData(musicData);
        musicData.androidCrc = 0;
    }
}

/**
*@brief       在线表盘开始结束控制指令
*@details
*@param[in] uint8_t *receive
*@param[in] uint8_t length
*@author       casen
*@date       2021-11-26
*/
void uteModuleProtocolWatchOnlineCtrl(uint8_t*receive,uint8_t length)
{
    uint8_t response[20];
    memset(response,0x00,20);
    response[0] = receive[0];
    response[1] = receive[1];

    if(receive[1]==0x01) // 读取表盘信息
    {
        uteModuleWatchOnlineReadDeviceInfo(&response[2]);
        uteModuleProfileBleSendToPhone(&response[0],20);
    }
    else if(receive[1]==0x02) // 开始同步表盘数据
    {
        uteModuleWatchOnlineReadyStart();
        uteModuleProfileBleSendToPhone(&response[0],2);
    }
    else if(receive[1]==0x03) // 结束同步表盘数据
    {
        response[0] = receive[0];
        response[1] = receive[1];
        response[2] = uteModuleWatchOnLineTSyncComplete();
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(receive[1]==0x05)
    {
        response[0] = receive[0];
        response[1] = receive[1];
        response[2] = receive[2];
        uteModuleWatchOnlineGetInfoWithIndex(receive[2], &response[3]);
        uteModuleProfileBleSendToPhone(&response[0], 7);
    }
    else if(receive[1]==0x06)
    {
        response[0] = receive[0];
        response[1] = receive[1];
        response[2] = receive[2];
        uteModuleWatchOnlineSetWillUpdateDataIndex(receive[2]);
        uteModuleProfileBleSendToPhone(&response[0], 3);
    }
}

void uteModuleProtocolWatchOnlineData(uint8_t*receive,uint8_t length)
{
    uteModuleWatchOnlineSyncData(receive,length);
}

/**
*@brief       日常活动目标设置，站立时长、运动时长、卡路里、步数、距离目标提醒
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       dengli.lu
*@date       2022-03-29
*/
void uteModuleProtocolTodayTargetCtrl(uint8_t*receive,uint8_t length)
{
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
    ute_module_target_notify_data_t targetNotifyData;
    uteModuleSportGetTodayTargetNotifyData(&targetNotifyData);
    if(receive[1]==0x01)//站立时长
    {
        if(receive[2]==0x00)
        {
            targetNotifyData.isStandTimeTargetNotifyOpen = false;
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
        else if(receive[2]==0x01)
        {
            targetNotifyData.isStandTimeTargetNotifyOpen = true;
            uint16_t standTime = targetNotifyData.currStandMin;
            if(standTime < (receive[3]<<8|receive[4]))//当天已经提醒，那么需要设置比之前大才要提醒
            {
                targetNotifyData.isStandTimeTargetHasNotify = false;
                targetNotifyData.isTodayAllTargetHasNotify = false;
            }
            targetNotifyData.standTimeTargetSetMin = receive[3]<<8|receive[4];
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
        else if(receive[2]==0x02)//读取七天历史记录
        {
            uteModuleSportStartSendStandTimeHistoryData();
        }
    }
    else if(receive[1]==0x02)//运动时长
    {
        if(receive[2]==0x00)
        {
            targetNotifyData.isSportTimeTargetNotifyOpen = false;
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
        else if(receive[2]==0x01)
        {
            targetNotifyData.isSportTimeTargetNotifyOpen = true;
            if((targetNotifyData.currentDayAllSportTimeSec/60) < (receive[3]<<8|receive[4]))
            {
                //当天已经提醒，那么需要设置比之前大才要提醒
                targetNotifyData.isSportTimeTargetHasNotify = false;
                targetNotifyData.isTodayAllTargetHasNotify = false;
            }
            targetNotifyData.sportTimeTargetSetMin = receive[3]<<8|receive[4];
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
    }
    else if(receive[1]==0x03)//kcal 目标
    {
        if(receive[2]==0x00)
        {
            targetNotifyData.isTodayKcalTargetNotifyOpen = false;
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
        else if(receive[2]==0x01)
        {
            targetNotifyData.isTodayKcalTargetNotifyOpen = true;
            uint16_t kcalValue = uteModuleSportGetCurrDayKcalData();
            if(kcalValue< (receive[3]<<8|receive[4]))
            {
                //当天已经提醒，那么需要设置比之前大才要提醒
                targetNotifyData.isTodayKcalTargetHasNotify = false;
                targetNotifyData.isTodayAllTargetHasNotify = false;
            }
            targetNotifyData.todayKcalTarget = receive[3]<<8|receive[4];
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
    }
    else if(receive[1]==0x04)//步数目标
    {
        if(receive[2]==0x00)
        {
            targetNotifyData.isTodayStepTargetNotifyOpen = false;
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
        else if(receive[2]==0x01)
        {
            targetNotifyData.isTodayStepTargetNotifyOpen = true;
            uint32_t setStep = receive[3]<<24|receive[4]<<16|receive[5]<<8|receive[6];
            bool isHandOpen = uteModuleSportGetIsOpenHandScreenOn();
            if(uteModuleSportGetStepsTargetCnt() < setStep)
            {
                uteModuleSportSetIsTargetCompleted(false);
                targetNotifyData.isTodayAllTargetHasNotify = false;
            }
            uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,setStep);
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
    }
    else if(receive[1]==0x05)//距离目标
    {
        if(receive[2]==0x00)
        {
            targetNotifyData.isTodayDistanceTargetNotifyOpen = false;
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
        else if(receive[2]==0x01)
        {
            targetNotifyData.isTodayDistanceTargetNotifyOpen = true;
            uint16_t tempDistance = uteModuleSportGetCurrDayDistanceData();
            if(uteModuleSystemtimeGetDistanceMiType())
            {
                tempDistance = (float)tempDistance / 0.6213712f;//先转换为KM，在转换为米
            }
            uint32_t totalDistance = tempDistance/100*1000 + tempDistance%100*10;
            if(totalDistance < (receive[3]<<24|receive[4]<<16|receive[5]<<8|receive[6]))
            {
                //当天已经提醒，那么需要设置比之前大才要提醒
                targetNotifyData.isTodayDistanceTargetHasNotify = false;
                targetNotifyData.isTodayAllTargetHasNotify = false;
            }
            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,distance = %d,", __func__,receive[3]<<24|receive[4]<<16|receive[5]<<8|receive[6]);
            targetNotifyData.todayDistanceTarget = receive[3]<<24|receive[4]<<16|receive[5]<<8|receive[6];
            uteModuleProfileBleSendToPhone(&receive[0],length);
        }
    }
    uteModuleSportSetTodayTargetNotifyData(targetNotifyData);
#endif
}

/**
*@brief     快捷回复数据
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       xjc
*@date       2022-07-01
*/
void uteModuleProtocolQuickReply(uint8_t*receive,uint8_t length)
{
#if UTE_MODUEL_QUICK_REPLY_SUPPORT
    uteModuleQuickReplyCmd(receive,length);
#endif
}

/**
*@brief     同步通讯录
*@details
*@param[in] uint8_t *receive
*@param[in] uint8_t length
*@author    casen
*@date      2021-12-6
*/
void uteModuleProtocolSyncAddressBook(uint8_t*receive,uint8_t length)
{
    if(receive[1] == 0xFA)
    {
        uteModuleCallSyncAddressBookStart();
        uteModuleProfileBleSendToPhone(&receive[0],3);
    }
    else if(receive[1] == 0xFB)
    {
        uteModuleCallSyncAddressBookData(receive,length);
        uteModuleProfileBleSendToPhone(&receive[0],3);
    }
    else if(receive[1] == 0xFC)
    {
        uteModuleCallSyncAddressBookEnd();
        uteModuleProfileBleSendToPhone(&receive[0],3);
    }
#if UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT
    else if(receive[1] == 0xAA)
    {
        uint8_t response[3];
        response[0] = receive[0];
        response[1] = receive[1];
        response[2] = UTE_MODUEL_CALL_SOS_CONTACT_MAX;
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(receive[1] == 0xAB)
    {
        uteModuleCallSyncSosContactStart();
        uteModuleProfileBleSendToPhone(&receive[0],3);
    }
    else if(receive[1] == 0xAC)
    {
        uteModuleCallSyncSosContactStart();
        uteModuleCallSyncSosContactData(receive,length);
        uteModuleProfileBleSendToPhone(&receive[0],3);
    }
    else if(receive[1] == 0xAD)
    {
        uteModuleProfileBleSendToPhone(&receive[0],4);
    }
    else if(receive[1] == 0xAE)
    {
        uteModuleCallSyncSosContactStart();
        uteModuleProfileBleSendToPhone(&receive[0],2);
    }
#endif
}

/**
*@brief       工厂测试模式
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2021-11-23
*/
void uteModuleProtocolFactoryTest(uint8_t*receive,uint8_t length)
{
    uteModuleFactoryTestProtocol(receive,length);
}

/**
*@brief     debug数据
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       zn.zeng
*@date       2022-03-16
*/
void uteModuleProtocolDebugData(uint8_t*receive,uint8_t length)
{
    uint8_t response[10];
    memset(response, 0, sizeof(response));
#if UTE_HARDFAULT_INFO_TO_FLASH_SUPPORT
    if(receive[1]==0x06)
    {
        if(receive[2]==0x01)
        {
            uteModuleHardfaultStartSendlogData();
        }
    }
#endif
#if UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT
    if(receive[1]==0x07)
    {
        if(receive[2]==0x01)
        {
#if UTE_MODULE_SLEEP
            uteModuleSleepStartSendSleepLogData();
#endif
        }
    }
#endif
#if UTE_MODULE_RUNING_LOG_SUPPORT && UTE_MODULE_LOG_SUPPORT
    if(receive[1]==0x08)
    {
        if(receive[2]==0x01 || receive[2]==0x00)
        {
            uteModuleLogSetSendRuningLogSwitch(receive[2]);
            response[0] = receive[0];
            response[1] = receive[1];
            response[2] = receive[2];
            uteModuleProfileBleSendToPhone(&response[0],3);
        }
    }
#endif
#if APP_DBG_GSENSOR_DATA
    if(receive[1] == 0x04)
    {
        if(receive[2] == 0x00)
        {
            uteModuleSportSetAppDebugGsensorDataSwitch(false);
        }
        else if(receive[2] == 0x01)
        {
            uteModuleSportSetAppDebugGsensorDataSwitch(true);
        }
    }
#endif
}


/*!指令转化列表 zn.zeng, 2021-08-17  */
const ute_module_protocol_cmd_list_t uteModuleProtocolCmdList[]=
{
    {.privateCmd =0,.publicCmd=0,.function=NULL},
    {.privateCmd = CMD_SET_PARAM_24HOUR_KM,.publicCmd=PUBLIC_CMD_SET_PARAM_24HOUR_KM,.function=uteModuleProtocolSetParamHourKmFormat},
    {.privateCmd = CMD_READ_SW_VERSION,.publicCmd=PUBLIC_CMD_READ_SW_VERSION,.function=uteModuleProtocolReadSoftwareVersion},
    {.privateCmd = CMD_READ_BATTERY_LVL,.publicCmd=PUBLIC_CMD_READ_BATTERY_LVL,.function=uteModuleProtocolReadBatteryLvl},
    {.privateCmd = CMD_SET_PARAM_DATE_TIME,.publicCmd=PUBLIC_CMD_SET_PARAM_DATE_TIME,.function=uteModuleProtocolSetDateTime},
    {.privateCmd = CMD_SET_BLENAME,.publicCmd=CMD_SET_BLENAME,.function=uteModuleProtocolSetBleName},
    {.privateCmd = CMD_SET_PARAM_STEP_SOON,.publicCmd=PUBLIC_CMD_SET_PARAM_STEP_SOON,.function=uteModuleProtocolSetOtherParam},
    {.privateCmd = CMD_MOTOR_ALARM_PARAM,.publicCmd=PUBLIC_CMD_MOTOR_ALARM_PARAM,.function=uteModuleProtocolSetAlarmOrCtrlMotor},
    {.privateCmd = CMD_FACTORY_RESET,.publicCmd=PUBLIC_CMD_FACTORY_RESET,.function=uteModuleProtocolFactoryReset},
    {.privateCmd = CMD_READ_MAC,.publicCmd=PUBLIC_CMD_READ_MAC,.function=uteModuleProtocolReadBleMac},
    {.privateCmd = CMD_SET_MULTIPLE_LANGUAGE,.publicCmd=PUBLIC_CMD_SET_MULTIPLE_LANGUAGE,.function=uteModuleProtocolSetMultipleLanguage},
    {.privateCmd = CMD_READ_HOUR_STEP_DATA,.publicCmd=PUBLIC_CMD_READ_HOUR_STEP_DATA,.function=uteModuleProtocolReadReadlTimeStepInfo},
    {.privateCmd = CMD_START_SYN_SETP_DATA,.publicCmd=PUBLIC_CMD_START_SYN_SETP_DATA,.function=uteModuleProtocolReadStepHistoryData},
    {.privateCmd = CMD_READ_MSG_CONTENT_SUPPORT,.publicCmd=CMD_READ_MSG_CONTENT_SUPPORT,.function=uteModuleProtocolReadMsgContentSupport},
    {.privateCmd = CMD_QUICK_SWITCH,.publicCmd=PUBLIC_CMD_QUICK_SWITCH,.function=uteModuleProtocolReadQuickSwitchStatus},
    {.privateCmd = CMD_CALL_STATUS,.publicCmd=PUBLIC_CMD_CALL_STATUS,.function=uteModuleProtocolCallStatusCtrl},
    {.privateCmd = CMD_TAKE_PICTURE_MODE,.publicCmd=PUBLIC_CMD_TAKE_PICTURE_MODE,.function=uteModuleProtocolTakePictureCtrl},
    {.privateCmd = CMD_MSG_CONTENT_MODE0,.publicCmd=PUBLIC_CMD_MSG_CONTENT_MODE0,.function=uteModuleProtocolMsgContent},
    {.privateCmd = CMD_MSG_CONTENT_MODE1,.publicCmd=PUBLIC_CMD_MSG_CONTENT_MODE1,.function=uteModuleProtocolMsgContent},
    {.privateCmd = CMD_SET_BP_THEORY_TEST,.publicCmd=CMD_SET_BP_THEORY_TEST,.function=uteModuleProtocolTheoryBloodpressureCtrl},
    {.privateCmd = CMD_START_BP_THEORY_DATA,.publicCmd=CMD_START_BP_THEORY_DATA,.function=uteModuleProtocolReadTheoryBloodpressureHistoryData},
    {.privateCmd = CMD_SET_7_DAYS_WEATHER_PARAM,.publicCmd=PUBLIC_CMD_SET_7_DAYS_WEATHER_PARAM,.function=uteModuleProtocolSetWeatherData},
    {.privateCmd = CMD_SEND_KEYCODE,.publicCmd=PUBLIC_CMD_SEND_KEYCODE,.function=uteModuleProtocolSendKeycode},
    {.privateCmd = CMD_SET_ANCS_PARAM,.publicCmd=PUBLIC_CMD_SET_ANCS_PARAM,.function=uteModuleProtocoliOSAncsNotifySwitch},
    {.privateCmd = CMD_SET_JIUZUO_PARAM,.publicCmd=PUBLIC_CMD_SET_JIUZUO_PARAM,.function=uteModuleProtocolSetSedentaryRemind},
    {.privateCmd = CMD_WURAO_PARAM_MODE,.publicCmd=PUBLIC_CMD_WURAO_PARAM_MODE,.function=uteModuleProtocolSetNotDisturParam},
    {.privateCmd = CMD_MOBILE_PLATFORM,.publicCmd=PUBLIC_CMD_MOBILE_PLATFORM,.function=uteModuleProtocolFormAppFrontOrBack},
    {.privateCmd = CMD_HEART_TEST,.publicCmd=PUBLIC_CMD_HEART_TEST,.function=uteModuleProtocolHeartTestCtrl},
    {.privateCmd = CMD_FACTORY_GSENSOR_TEST,.publicCmd=CMD_FACTORY_GSENSOR_TEST,.function=uteModuleProtocolCheckGsensorData},
    {.privateCmd = CMD_FACTORY_POWER_OFF,.publicCmd=PUBLIC_CMD_FACTORY_POWER_OFF,.function=uteModuleProtocolSetPowerOff},
    {.privateCmd = CMD_HEART_24HOURS_AUTO_TEST,.publicCmd=PUBLIC_CMD_HEART_24HOURS_AUTO_TEST,.function=uteModuleProtocolHeartAutoTestHistoryData},
    {.privateCmd = CMD_SPO2_TEST,.publicCmd=PUBLIC_CMD_SPO2_TEST,.function=uteModuleProtocolBloodoxygenCtrl},
    {.privateCmd = CMD_BREATH_RATE_TEST,.publicCmd=CMD_BREATH_RATE_TEST,.function=uteModuleProtocolBreathrateCtrl},
    {.privateCmd = CMD_SLEEP_ON_BAND,.publicCmd=PUBLIC_CMD_SLEEP_ON_BAND,.function=uteModuleProtocolSleepReadHistoryData},
    {.privateCmd = CMD_SEND_SLEEP_ON_BAND_DATAS,.publicCmd=PUBLIC_CMD_SEND_SLEEP_ON_BAND_DATAS,.function=uteModuleProtocolSleepReadHistoryData},
    {.privateCmd = CMD_SPORT_MODE_AND_SPORT_HEART_RATE,.publicCmd=PUBLIC_CMD_SPORT_MODE_AND_SPORT_HEART_RATE,.function=uteModuleProtocolMoreSportCtrl},
    // {.privateCmd = CMD_SET_WOMEN_MENSTRUAL_CYCLE,.publicCmd=CMD_SET_WOMEN_MENSTRUAL_CYCLE,.function=uteModuleProtocolWomenMenstrualCycle},
    {.privateCmd = CMD_MUSIC_CONTENT_CTRL,.publicCmd=CMD_MUSIC_CONTENT_CTRL,.function=uteModuleProtocolMusicCtrl},
    {.privateCmd = CMD_FACTORY_TEST_MODE,.publicCmd=PUBLIC_CMD_FACTORY_TEST_MODE,.function=uteModuleProtocolFactoryTest},
    {.privateCmd = CMD_WATCH_ONLINE,.publicCmd=PUBLIC_CMD_WATCH_ONLINE,.function=uteModuleProtocolWatchOnlineCtrl},
    {.privateCmd = CMD_WATCH_ONLINE_BLE5_0,.publicCmd=PUBLIC_CMD_WATCH_ONLINE_BLE5_0,.function=uteModuleProtocolWatchOnlineData},
#if UTE_MODULE_TEMPERATURE_SUPPORT
    {.privateCmd = CMD_TEMPERATURE_HEAD,.publicCmd=CMD_TEMPERATURE_HEAD,.function=uteModuleProtocolTemperatureCtrl},
#endif
    {.privateCmd = CMD_SYNC_CONTACTS,.publicCmd=CMD_SYNC_CONTACTS,.function=uteModuleProtocolSyncAddressBook},
    {.privateCmd = CMD_SOCIAL_APP_SELECT,.publicCmd=CMD_SOCIAL_APP_SELECT,.function=uteModuleProtocolSocialAppSelectParam},
    // {.privateCmd = CMD_EMOTION_PRESSURE_TEST,.publicCmd=CMD_EMOTION_PRESSURE_TEST,.function=uteModuleProtocolEmotionPressureCtrl},
    // {.privateCmd = CMD_USER_ID_FOR_BINDING,.publicCmd=PUBLIC_CMD_USER_ID_FOR_BINDING,.function=uteModuleProtocolAppBindingCtrl},
#if UTE_MODULE_UNIT_TEST_FUNCTION_DATAS_SUPPORT
    {.privateCmd = CMD_DEBUG_TEST_DATA,.publicCmd=CMD_DEBUG_TEST_DATA,.function=uteModuleUnitTestDatasReceiveHandler},
#else
    {.privateCmd = CMD_DEBUG_TEST_DATA,.publicCmd=CMD_DEBUG_TEST_DATA,.function=uteModuleProtocolDebugData},
#endif
    {.privateCmd = CMD_QUICK_REPLY_GET_PHONE_NUMBER,.publicCmd=CMD_QUICK_REPLY_GET_PHONE_NUMBER,.function=uteModuleProtocolQuickReply},
    {.privateCmd = CMD_QUICK_REPLY_CONTENT,.publicCmd=CMD_QUICK_REPLY_CONTENT,.function=uteModuleProtocolQuickReply},
    {.privateCmd = CMD_QUICK_REPLY_CTRL,.publicCmd=CMD_QUICK_REPLY_CTRL,.function=uteModuleProtocolQuickReply},
    // {.privateCmd = CMD_SET_DRINK_WATER_PARAM,.publicCmd=CMD_SET_DRINK_WATER_PARAM,.function=uteModuleProtocolSetDrinkWater},
    {.privateCmd = CMD_TODAY_TARGET_CTRL,.publicCmd=CMD_TODAY_TARGET_CTRL,.function=uteModuleProtocolTodayTargetCtrl},
    // {.privateCmd = CMD_SYNC_CYWEE_SWIM_DATA,.publicCmd=CMD_SYNC_CYWEE_SWIM_DATA,.function=uteModuleProtocolSyncCyweeSwimData},
    // {.privateCmd = CMD_SPORTS_TARGET_SELECT,.publicCmd=CMD_SPORTS_TARGET_SELECT,.function=uteModuleProtocolSportsTargetSelect},
    // {.privateCmd = CMD_CUST_DEFINE_CMD,.publicCmd=CMD_CUST_DEFINE_CMD,.function=uteModuleProtocolCustomF6CmdHandle},
    // {.privateCmd = CMD_GOTO_SCREEN,.publicCmd=CMD_GOTO_SCREEN,.function=uteModuleProtocolGotoScreenCmdHandle},
#if UTE_MODULE_BT_ONCE_PAIR_CONNECT_SUPPORT
    {.privateCmd = CMD_BT30_CTRL,.publicCmd=CMD_BT30_CTRL,.function=uteModuleProtocolCtrlBT},
#endif
};

/**
*@brief        公共指令和私有指令转化
*@details
*@param[in] uint8_t *cmdHead  指令指针
*@param[in] ute_module_protocol_conversion_type_t tpye  转化
*@return 返回指令表里面的索引
*@author       zn.zeng
*@date       2021-08-16
*/
uint16_t uteModuleProtocolConversionCmd(uint8_t *cmdHead,ute_module_protocol_conversion_type_t tpye)
{
    uint16_t listSize = sizeof(uteModuleProtocolCmdList)/sizeof(ute_module_protocol_cmd_list_t);
    uint8_t cmd = cmdHead[0];
    for(uint16_t i=1; i<listSize; i++)
    {
        if(tpye==TYPE_TO_PUBLIC)
        {
            if(cmd == uteModuleProtocolCmdList[i].privateCmd)
            {
                cmdHead[0] = uteModuleProtocolCmdList[i].publicCmd;
                return i;
            }
        }
        else if(tpye==TYPE_TO_PRIVATE)
        {
            if(cmd == uteModuleProtocolCmdList[i].publicCmd)
            {
                cmdHead[0] = uteModuleProtocolCmdList[i].privateCmd;
                return i;
            }
        }
        else if(tpye==TYPE_FIND_INDEX)
        {
            if(cmd == uteModuleProtocolCmdList[i].privateCmd)
            {
                return i;
            }
        }
    }
    return 0;
}
/**
*@brief        协议解析
*@details   普通指令和BLE5.0的指令全部走此函数，所有指令转化成私有指令处理
*@param[in] uint8_t *receive  数据指针
*@param[in] uint8_t length  数据长度
*@param[in] bool isPublic  是否是公共协议，指令头不一样
*@author       zn.zeng
*@date       2021-08-16
*/
void uteModuleProtocolFromPhone(uint8_t *receive,uint8_t length,bool isPublic)
{
    uint16_t index = 0;
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,receive=", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_PROTOCOL_LVL,receive,length);
    /*Casen 22-04-09*/
    // uteModulePlaformUpdateConnectParam(12,36,10*1000);
    /*Casen 22-04-09*/

//#if UTE_USER_ID_FOR_BINDING_SUPPORT
//    if (uteModuleAppBindingGetOurAppConnection())
//    {
//        // binding ok ,isOurAppApkConnection receive cmd
//    }
//    else
//    {
//        if((receive[0] == CMD_USER_ID_FOR_BINDING)||(receive[0] == PUBLIC_CMD_USER_ID_FOR_BINDING))
//        {
//            // binding
//            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL,"%s,receice data binding",__func__);
//        }
//        else
//        {
//#if UTE_LOG_CONNECTION_PROTOCOL_LVL&&UTE_LOG_SYSTEM_LVL
    // uteModuleAppBindingSetOurAppConnection(true);
//#endif
//            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL,"%s,receice data return",__func__);
//            return;
//        }
//    }
//#endif

    if(isPublic)
    {
        index = uteModuleProtocolConversionCmd(receive,TYPE_TO_PRIVATE);
    }
    else
    {
        index = uteModuleProtocolConversionCmd(receive,TYPE_FIND_INDEX);
    }
    if(index!=0)
    {
        if(uteModuleProtocolCmdList[index].function!=NULL)
        {
            uteModuleProtocolCmdList[index].function(receive,length);
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,error index is %d,function is null", __func__,index);
        }
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,error index is 0 ---> 0x%x", __func__, receive[0]);
    }
}
/**
*@brief        读取功能标志
*@details
*@param[in] uint8_t *data  功能控制支持位，每个bit代表一个功能
*@author       zn.zeng
*@date       2021-08-19
*/
void uteModuleProtocolReadFunctionSupport(uint8_t *data,uint8_t size)
{
    memset(data,0,size);
    // data[0]|= 0x08; //瑞昱平台标识
#if UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT
    data[1]|= 0x02;
#endif
#if ((UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT>1)&&UTE_MODULE_WATCHONLINE_SUPPORT)
    data[1]|= 0x04;
#endif
#if UTE_REM_SLEEP_SUPPORT
    data[1]|= 0x08;
#endif
#if UTE_DRV_BLOODPRESSURE_DN02_SUPPORT
    data[1]|= 0x10;
#endif
#if UTE_MODULE_SEDENTARY_NOTIFY_SET_NOONSLEEP_TIME
    data[2]|= 0x01;
#endif
#if UTE_BT30_CALL_SUPPORT
    data[2]|= 0x04;
#endif
#if UTE_MODUEL_CALL_ADDRESSBOOK_EXPANSION_LENGTH_SUPPORT
    data[2]|= 0x08;
#endif
#if 1//UTE_MODULE_SCREENS_LANGUAGE_SELECT_LIST_SUPPORT
    data[2]|= 0x20;
#endif
    data[2]|= 0x40;
#if UTE_MODULE_WEATHER_HUMIDITY_ULTRAVIOLET_SUPPORT
    data[3]|= 0x08;
#endif
#if UTE_MODUEL_QUICK_REPLY_SUPPORT
    data[3]|= 0x02;
#endif
#if UTE_MODULE_APP_CONTROL_RESTART_SUPPORT
    data[3]|= 0x10;
#endif
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    data[3]|= 0x20;
#endif
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
    data[3]|= 0x80;
#endif
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
    data[4]|= 0x01;
#endif
#if UTE_MODULE_MUTE_PHONE_INCOM_SUPPORT
    data[4]|= 0x40;
#endif

#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
    data[4]|= 0x20;
#endif
#if UTE_MODULE_WEATHER_CITY_NAME_NEW_SUPPORT
    data[4]|= 0x80;
#endif
#if UTE_MODULE_PROTOCOL_COMMON_F6_CMD_CUSTOM_SUPPORT
    data[5]|= 0x02;
#endif
    data[5]|= 0x04;
#if UTE_MODULE_DRINK_WATER_NOTIFY_SCREEN_SUPPORT
    data[5]|=0x08;
#endif
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
    data[5]|= 0x20;
#endif
#if UTE_DRV_BLOODPRESSURE_VITA_UART_SUPPORT  //长桑血压标定
    data[5] |= 0x10;
    data[3] |= 0x04;
#endif
    data[6]|= 0x01;
    data[6]|= 0x08;  //支持应用通知开关选择显示
    data[6]|= 0x10; //安卓音乐信息
#if UTE_BT30_CALL_SUPPORT
    data[6]|= 0x20;
#endif
#if UTE_MODULE_BREATHRATE_SUPPORT
    data[6]|= 0x40;
#endif
    data[7]|= 0x01;
    data[7]|= 0x02;
#if UTE_USER_ID_FOR_BINDING_SUPPORT
    data[7]|= 0x04;
#endif
#if UTE_MODULE_SCREENS_TEMPERATURE_SUPPORT //不用UTE_MODULE_TEMPERATURE_SUPPORT判断 xjc mark 20220107
    data[7]|= 0x08;
#endif
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
    data[7]|= 0x40;
#endif
#if UTE_MODULE_ECG_ALGORITHM_SUPPORT && UTE_MODULE_ECG_SUPPORT
    data[7]|= 0x80;
#endif
    data[8]|= 0x01;
    data[8]|= 0x04;
    data[8]|= 0x10;
#if UTE_MODULE_SPORT_FREE_TRAINING_SUPPORT
    data[8] |= 0x20;
#endif
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
    data[8] |= 0x40;
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    data[9] |= 0x01;
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    data[9] |= 0x02;
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    data[9] |= 0x04;
#endif
    data[9] |= 0x40;
#if UTE_MODULE_HEART_MIN_MAX_WARNING_VALUE_SUPPORT
    data[9]|= 0x08;
#endif
#if UTE_MODULE_BLOODPRESSURE_AUTO_TEST_SUPPORT
    data[9]|= 0x10;
#endif
    data[9]|= 0x20;
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    data[10] |= 0x01;
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    data[10] |= 0x02;
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    data[10] |= 0x04;
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    data[10] |= 0x08;
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    data[10] |= 0x10;
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    data[10] |= 0x20;
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    data[10] |= 0x40;
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    data[10] |= 0x80;
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    data[11] |= 0x02;
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    data[11] |= 0x04;
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    data[11] |= 0x08;
#endif
    data[11] |= 0x10;
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    data[11] |= 0x20;
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    data[11] |= 0x40;
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    data[11] |= 0x80;
#endif
#if UTE_MODULE_WATCHONLINE_SUPPORT
    data[12]|= 0x01;
#endif
    data[12]|= 0x08;
    data[12]|= 0x10;
#if UTE_MODULE_ECG_SUPPORT&&!APP_ECG_ONLY_LOCAL_TEST_SUPPORT
    data[12]|= 0x20;
#endif
    data[12]|= 0x40;
#if UTE_MODULE_MENSTRUAL_CYCLE_SUPPORT
    data[12]|= 0x80;
#endif
    data[13]|= 0x01;
    data[13]|= 0x20;
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    data[15]|= 0x01;
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    data[15]|= 0x02;
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    data[15]|= 0x04;
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    data[15]|= 0x08;
#endif
    data[15]|= 0x10;
    data[15]|= 0x40;
    data[16]|= 0x08;
    data[16]|= 0x10;
    data[16]|= 0x20;  //手环查找手机
    data[16]|= 0x40;
    data[17]|= 0x01;
    data[17]|= 0x02;
    data[17]|= 0x08;
#if UTE_MODULE_BLOODPRESSURE_SUPPORT
    data[17]|= 0x10;
#endif
    data[17]|=0x40; //来电拒接
#if (!UTE_MODULE_CYWEE_MOTION_SUPPORT) //使用赛维算法就屏蔽计步和跑步区分
    data[18]|= 0x01;
#endif
    data[18]|= 0x20;
#if UTE_MODULE_SPORT_JUMP_SUPPORT||UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    data[18]|= 0x40;
#endif
    data[18]|= 0x80;
    data[19]|= 0x04;
//#if !UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    data[19]|= 0x08;
//#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    data[19]|= 0x10;
#endif
#if !UTE_MODULE_LOCAL_SET_LIFT_WRIST_SUPPORT
    data[19]|= 0x40;
#endif
#if UTE_MODULE_HEART_MIN_MAX_WARNING_VALUE_SUPPORT
    data[19]|= 0x80;
#endif

}


/**
*@brief     34F1拓展的读取功能标志位
*@details
*@param[in] uint8_t *data  功能控制支持位，每个bit代表一个功能
*@author    casen
*@date      2022-11-03
*/
void uteModuleProtocolReadExpandFunctionSupport(uint8_t *data,uint8_t size)
{
    memset(data,0,size);
    /*------------前两字节不允许变动----------------*/
    uint16_t mtuSize = uteApplicationCommonGetMtuSize();
    data[0] = (mtuSize>>8)&0xff;
    data[1] = mtuSize&0xff;
    /*------------前两字节不允许变动----------------*/
    data[2] |= 0x08; //中科平台标识
#if APP_MODULE_HEART_RESTING_HEARTRATE_SUPPORT
    data[19] |= 0x02;
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    data[19] |= 0x20;
#endif
#if UTE_MODULE_ECG_SUPPORT && UTE_DRV_VITA_ECG_SUPPORT
    data[18] |= 0x10;
#endif
#if UTE_MODULE_BLOODSUGAR_SUPPORT
    data[17] |= 0x02;
#endif
#if UTE_MODULE_HRV_SUPPORT
    data[17] |= 0x04;
#endif
#if UTE_MODULE_VARIOUS_MEASURE_SUPPORT
    data[17] |= 0x08;
#endif
#if UTE_MODULE_ALI_UPAY_V02_SUPPORT
    data[16] |= 0x08;
#endif
#if UTE_MODULE_BLOODPRESSURE_MORNING_TEST_SUPPORT
    data[15] |= 0x04;
#endif
#if CYWEE_USE_LIB_P850
    data[15] |= 0x02;
#endif

#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
    data[17] |= 0x40; //维客情绪压力
#endif
#if UTE_MODULE_BLOODPRESSURE_DESIGNATED_TIME_TEST_SUPPORT
    data[15] |= 0x20;
#endif
#if UTE_MODULE_SHUTANG_METABOLISM_ALGORITHM_SUPPORT
    data[15] |= 0x80;
#endif
}


