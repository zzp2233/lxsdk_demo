/**
*@file
*@brief        查找手机处理模块
*@details
*@author       xjc
*@date       2022-01-18
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_findphone.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_application_common.h"
#include "ute_drv_motor.h"

ute_findphone_data_t uteModuleFindPhoneData;

/**
*@brief  初始化函数
*@details
*@author        xjc
*@date        2022-01-18
*/
void uteModuleFindPhoneInit(void)
{
    memset(&uteModuleFindPhoneData, 0, sizeof(ute_findphone_data_t));
}

/**
*@brief  设置查找手机的状态
*@details
*@author        xjc
*@date        2022-01-18
*/
void uteModuleFindPhoneSetStatus(T_FIND_PHONE_STATUS status)
{
    uteModuleFindPhoneData.status = status;
}

/**
*@brief  获取查找手机的状态
*@details
*@author        xjc
*@date        2022-01-18
*/
uint8_t uteModuleFindPhoneGetStatus(void)
{
    return (uint8_t)uteModuleFindPhoneData.status;
}

/**
*@brief        发送指令让手机开始响铃
*@details
*@author        xjc
*@date        2022-01-18
*/
void uteModuleFindPhoneStartRing(void)
{
    ute_ble_connect_state_t connectStatus;
    uteApplicationCommonGetBleConnectionState(&connectStatus);
    if (connectStatus.isConnected)
    {
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
        uteModuleFindPhoneSetStatus(FIND_PHONE_RING);
#endif // UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
#if CUSTOMIZE_THE_NUMBER_OF_VIBRATIONS
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,6);
#else
#if !UTE_MODULE_FINEPHONE_NO_MOTOR_SUPPORT
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
#endif
#endif
        uint8_t deviceFindPhone[2] = {CMD_SEND_KEYCODE, 0x0A};
        uteModuleProfileBleSendToPhone(&deviceFindPhone[0], 2);
    }
}

/**
*@brief        发送指令让手机停止响铃
*@details      该指令未使用
*@author        xjc
*@date        2022-01-18
*/
void uteModuleFindPhoneStopRing(void)
{
    ute_ble_connect_state_t connectStatus;
    uteApplicationCommonGetBleConnectionState(&connectStatus);
    if (connectStatus.isConnected)
    {
        uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
        uint8_t response[4];
        response[0] = CMD_SEND_KEYCODE;
        response[1] = 0x0A;
        response[2] = 0x01;
        response[3] = 0x00;
        uteModuleProfileBleSendToPhone(&response[0], 4);
    }
}
