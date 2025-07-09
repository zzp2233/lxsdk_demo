/**
*@file
*@brief        模块系统消息
*@details  系统消息处理连接层
*@author       zn.zeng
*@date       Jun 29, 2021
*@version      v1.0
*/

#include "ute_module_message.h"
#include "ute_module_log.h"
#include "ute_module_systemtime.h"
#include "ute_drv_motor.h"
#include "ute_module_notify.h"
#include "ute_application_common.h"
#include "ute_module_sport.h"
#include "ute_module_gui_common.h"
#include "ute_drv_gsensor_common.h"
#include "ute_module_heart.h"
#include "ute_module_watchonline.h"
#include "ute_module_factorytest.h"
#include "ute_module_call.h"
#include "ute_module_localRingtone.h"
#include "ute_module_micrecord.h"
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
#include "ute_module_emotionPressure.h"
#endif

/**
*@brief  消息模块消息处理函数
*@details  处理ute自己的消息，此函数需要接到平台消息处理函数，运行在ute application task
*@param[in] ute_task_application_message_t *msg
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModuleMessageUteApplicationTaskHandler(ute_task_application_message_t *msg)
{
    uint16_t type = msg->type;
    uint32_t param = msg->param;

    switch (type)
    {
        case MSG_TYPE_SYSTEM_TIME_SEC_BASE:
        {
            uteModuleSystemtimeSecondCb();
        }
        break;
        case MSG_TYPE_DRV_MOTOR_START:
        {
            uteDrvMotorStartHandlerMsg(param);
        }
        break;
        case MSG_TYPE_DRV_MOTOR_STOP:
        {
            uteDrvMotorStopHandlerMsg();
        }
        break;
        case MSG_TYPE_DRV_MOTOR_HANDLE_TIMER:
        {
            uteDrvMotorTimerHandlerMsg();
        }
        break;
        case MSG_TYPE_HNAD_SCREEN_ON_NOTIFY:
        {
            uteModuleGuiCommonHandScreenOnMsg();
        }
        break;
        case MSG_TYPE_HNAD_SCREEN_OFF_NOTIFY:
        {
            uteModuleGuiCommonHandScreenOffMsg();
        }
        break;
        case MSG_TYPE_MODULE_NOTIFY_NOTIFYCATTION:
        {
            uteModuleNotifyNotifycationHandlerMsg();
        }
        break;
        case MSG_TYPE_SYSTEM_START_POWER_OFF:
        {
            uteApplicationCommonStartPowerOffMsg();
        }
        break;
        case MSG_TYPE_SYSTEM_REAL_POWER_OFF:
        {
            uteApplicationCommonRealPowerOffMsg();
        }
        break;
        case MSG_TYPE_MODULE_GUI_CLEAR_DEPTH:
        {
            uteModuleGuiCommonDisplayDepthClearTop(true);
        }
        break;
        case MSG_TYPE_SYNC_DATA_TIMER:
        {
            uteApplicationCommonSyncDataTimerMsg();
        }
        break;
        case MSG_TYPE_TAKE_PICTURE_NOTIFY:
        {
            uteModuleTakePictureHandlerMsg();
        }
        break;
        case MSG_TYPE_SEDENTARY_NOTIFY:
        {
            // uteModuleSportSedentaryMsg();
        }
        break;
        case MSG_TYPE_MODULE_SPORT_START_MORE_SPORTS:
        {
            // uteModuleSportStartMoreSportsMsgHandler(param);
        }
        break;
        case MSG_TYPE_MODULE_SPORT_STOP_MORE_SPORTS:
        {
            // uteModuleSportStopMoreSportsMsgHandler();
        }
        break;
        case MSG_TYPE_DRV_SPORT_COUNTDOWN_TIMER:
        {
            // uteModuleSprotCountdownTimerHandlerMsg();
        }
        break;
        case MSG_TYPE_DRV_SPORT_ALGO_INPUT_DATA_TIMER:
        {
            // uteModuleSprotInputDataBeforeAlgoTimerHandler();
        }
        break;
#if UTE_MODULE_SPORT_TAKE_PICTURE_OPEN_APP_SCREEN
        case MSG_TYPE_TAKE_PICTURE_OPEN_APP_SCREEN:
        {

            uteModuleSportOpenAppTakePictureScreenHandler();

        }
        break;
        case MSG_TYPE_TAKE_PICTURE_CLOSE_APP_SCREEN:
        {

            uteModuleSportCloseAppTakePictureScreenHandler();

        }
        break;
#endif
        case MSG_TYPE_HEART_ALGO_HANDLER:
        {
            // vc30fx_usr_device_handler(0, 1);
            // uteDrvHeartVC30FXHeartOrBloodOxygenAlgoInputData();
        }
        break;
        case MSG_TYPE_HEART_START_SINGLE_TESTING:
        {
            // uteModuleHeartStartSingleTestingMsgHandler(param);
        }
        break;
        case MSG_TYPE_HEART_STOP_SINGLE_TESTING:
        {
            // uteModuleHeartStopSingleTestingMsgHandler(param);
        }
        break;
        case MSG_TYPE_RESET_ROVLLVER_SCREEN_MODE:
        {
#if UTE_MODULE_SPORT_SUPPORT
            uteModuleSprotResetRovllverScreenMode();
#endif
        }
        break;
#if UTE_MODULE_WATCHONLINE_SUPPORT
        case MSG_TYPE_MODULE_WATCHONLINE_RECVTIMEOUT_TIMER:
        {
            uteModuleWatchOnlineRecvTimeoutMsgHandler();
        }
        break;
        case MSG_TYPE_MODULE_WATCHONLINE_ONESEC_TIMER:
        {
            uteModuleWatchOnlineOneSecMsgHandler();
        }
        break;
#endif
        case MSG_TYPE_DRV_KEY_HANDLER:
        {
            uteModuleFactoryTestKeysHandler(param);
#if UTE_MODULE_SPORT_SUPPORT
            uteModuleSprotResetRovllverScreenMode();
#endif
        }
        break;
        case MSG_TYPE_MODULE_NOTIFY_ANCS_START_PAIR:
        {
            uteModuleNotifyAncsStartPairHandlerMsg();
        }
        break;
        case MSG_TYPE_CALL_DISABLE_NOTIFY:
        {
            uteModuleNotifyCallDisableHandlerMsg();
        }
        break;
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
        case MSG_TYPE_EMOTION_PRESSURE_START_SINGLE_TESTING:
        {
            uteModuleEmotionPressureStartSingleTestingMsgHandler(param);
        }
        break;
        case MSG_TYPE_EMOTION_PRESSURE_STOP_SINGLE_TESTING:
        {
            uteModuleEmotionPressureStopSingleTestingMsgHandler(param);
        }
        break;
#endif
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
        case TO_APP_TASK_CONNECT_A2DP:
        {
            uteModuleCallConnectA2DPProfile();
        }
        break;
        case TO_APP_TASK_DISCONNECT_A2DP:
        {
            uteModuleCallDisconnectA2DPProfile();
        }
        break;
#endif
        case TO_APP_TASK_PLAY_RING:
        {
            uteModuleLocalRingtonePlayRingDetail();
        }
        break;
        case TO_APP_TASK_STOP_RING:
        {
            uteModuleLocalRingtoneStopRingDetail();
        }
        break;
        case MSG_TYPE_FACTORY_WRITE_RECORD_DATA_TO_FLASH:
            //uteModuleMicRecordFactoryWriteDataToFlash();
            break;

        case MSG_TYPE_FACTORY_PLAY_RECORD:
            //uteModuleMicRecordFactoryPlay();
            break;
#if UTE_DRV_SCREEN_ESD_TE_INT_ERROR_RESET_SUPPORT
        case MSG_TYPE_SCREEN_ESD_TE_INT_ERROR_RESET:
            uteDrvScreenEsdTeIntErrorCheckHandlerMsg();
            break;
#endif
        case MSG_TYPE_SMOKE_REMIND:
        {
            if(uteApplicationCommonIsPowerOn() && func_cb.sta != FUNC_AGEING)
            {
                printf("uteTaskGuiStartScreen(FUNC_ECIG_REMINDER\n");
                uteTaskGuiStartScreen(FUNC_ECIG_REMINDER, 0, __func__);
            }
        }
        break;
        default:
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,unknown msg,type=%d", __func__, type);
            break;
    }
}



