/**
*@file
*@brief        电话相关模块
*@details    和平台接口相关
*@author       zn.zeng
*@date       2021-10-28
*@version      v1.0
*/

#include "ute_module_call.h"
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_gui_common.h"
#include "ute_application_common.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_module_charencode.h"
#include "include.h"

#ifndef GAP_DEVICE_NAME_LEN
#define GAP_DEVICE_NAME_LEN 32
#endif // !GAP_DEVICE_NAME_LEN

ute_module_call_data_t uteModuleCallData;
void *uteModuleCallOnGoingTimerPointer = NULL;
void *uteModuleCallActiveTimerPointer = NULL;
//Casen add 22-04-18
void *uteModuleCallOutGoingChangeScoTimerPointer = NULL;
uint8_t retryOutgoingChangeScoCount = 0;
uint8_t retryActiveChangeScoCount = 0;

//Casen add 22-06-23
void *uteModuleCallDialTimerPointer = NULL;

//zn.zeng add 22-07-04
void *uteModuleCallPbapTimerOutPointer = NULL;

static uint8_t uteModuleBtDeviceName[GAP_DEVICE_NAME_LEN];

ute_module_call_app_ctrl_data_t uteModuleCallAppCtrlData =
{
    .appCallForwarding = false,
    .phoneOS = 0x01,
    .btAutomaticPair = true,
    .uuid = 0x00,
};
bool isBtPbapNameUpdate = false;  //通过PBAP更新了来电名
/**
*@brief      用于判断协议HFP，A2DP是否链接
*@details    A2DP只有在用户打开开关的时候才判断，否则不判断;这个是比较好的方式用于判断BT是否链接手机
*@author     ellison
*@date       ellison add ,2022-Sep-05 21:55
*/
bool isHfpAndA2dpProfileBothConnected(void)
{
    bool result =false;
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
    if(uteModuleCallData.isEntertranmentVoiceOn)
    {
        result = bt_a2dp_profile_completely_connected();
    }
#endif
    result = hfp_is_connected();
    return result;
}

/**
*@brief  bt 参数保存
*@details
*@author        zn.zeng
*@date        2022-01-07
*/
void uteModuleCallBtSaveConfig(void)
{
    void *file;
    uint8_t readbuff[6];
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALLINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        memcpy(&readbuff[0],&uteModuleCallData.address[0],6);
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,mac=", __func__);
        UTE_MODULE_LOG_BUFF(UTE_LOG_CALL_LVL,&uteModuleCallData.address[0],6);
        uteModuleFilesystemWriteData(file,&readbuff[0],6);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief  bt 参数读取
*@details
*@author        zn.zeng
*@date        2022-01-07
*/
void uteModuleCallBtReadConfig(void)
{
    void *file;
    uint8_t readbuff[6];
    memset(readbuff,0,6);
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALLINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],6);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,readbuff=", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_CALL_LVL,&readbuff[0],6);
    memcpy(&uteModuleCallData.address[0],&readbuff[0],6);
}

/**
*@brief  bt连接标志
*@details
*@author        zn.zeng
*@date        2021-11-04
*/
bool uteModuleCallBtIsConnected(void)
{
#if 1
    return isHfpAndA2dpProfileBothConnected();
#else
    return bt_is_connected();
#endif
}

/**
*@brief  更新KEY获取的手机蓝牙地址
*@details  为了防止用户首次没有打开BT，在后续打开时，没有获取到蓝牙地址；而不能每隔10s的回链
*@param[in] uint8_t *addr，蓝牙地址数据，大小为6byte
*@author       ellison
*@date        2022-09-21
*/
void uteModuleCallBtUpdateKeyConnectAddress(uint8_t *addr)
{
    if(memcmp(&uteModuleCallData.address[0],&addr[0],6)!=0)
    {
        memcpy(&uteModuleCallData.address[0],&addr[0],6);
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,mac=", __func__);
        UTE_MODULE_LOG_BUFF(UTE_LOG_CALL_LVL,&uteModuleCallData.address[0],6);
        uteModuleCallBtSaveConfig();
    }
}

/**
*@brief  call bt关机
*@details 1、当消息为UTE_BT_POWER_OFF_APP_UNBIND时，需要等清理了佩戴信息才关机
*@param[in] reason, BT关闭原因
*@author        zn.zeng
*@date        2021-11-08
*/
void uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_REASON reason)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,reason=%d",__func__,reason);
    bsp_bt_trun_off();
    if(reason == UTE_BT_POWER_OFF_APP_UNBIND)
    {
        bt_nor_delete_link_info();
    }

    if(UTE_BT_POWER_OFF_AUTO != reason)
    {
        if(UTE_BT_POWER_OFF_APP_UNBIND == reason)
        {
            uteModuleCallData.isBtAutoClose = true;
        }
        else
        {
            uteModuleCallData.isBtAutoClose = false;
        }
    }
    else
    {
        uteModuleCallData.isBtAutoClose = true;
    }

    uteModuleCallData.isPowerOn = false;
    uteModuleCallData.callData.isAutoWakupFromBle = false;
}

/**
*@brief  call bt开机，可连接
*@details
*@author        zn.zeng
*@date        2021-11-08
*/
void uteModuleCallBtPowerOn(ute_bt_power_on_type_t type)
{
    uteModuleCallData.powerOnType = type;
    if (type != UTE_BT_POWER_OFF)
    {
        bsp_bt_trun_on();

        uteModuleCallData.callData.isAppAudio = false;
        uteModuleCallData.callData.inOutToActiveRtc = 0;
        uteModuleCallData.powerOnTimeSecond = 0;
        uteModuleCallData.idleTimerSecond = 0;
        uteModuleCallData.isPowerOn = true;
    }
    if(type==UTE_BT_POWER_ON_FORM_BLE_CALL_NOTIFY)
    {
        uteModuleCallData.callData.isAutoWakupFromBle = true;
    }
}

/**
*@brief BT自动睡眠参数保存
*@details
*@author        xjc
*@date        2022-04-27
*/
void uteModuleCallIsBtAutoCloseSaveConfig(void)
{
    printf("KSKSKSKSKS%s,len: %d\n",__func__,__LINE__);
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleCallData.isBtAutoClose;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BT_AUTO_CLOSE_STATUS,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief  BT自动睡眠参数读取
*@details
*@author      casen
*@date        2022-08-04
*/
void uteModuleCallIsBtAutoCloseReadConfig(void)
{
    void *file;
    uint8_t readbuff[2];
    readbuff[0] = false;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BT_AUTO_CLOSE_STATUS,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleCallData.isBtAutoClose = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,.isBtAutoClose=%d", __func__,uteModuleCallData.isBtAutoClose);
}
/**
*@brief BT自动睡眠参数获取
*@details
*@author      caesn
*@date        2022-08-17
*/
bool uteModuleCallGetIsBtAutoClose(void)
{
    return uteModuleCallData.isBtAutoClose;
}

/**
*@brief  call 每秒函数
*@details
*@author        zn.zeng
*@date        2021-12-08
*/
void uteModuleCallEverySecond(void)
{
    if(!uteApplicationCommonIsPowerOn())
    {
        return;
    }
#if UTE_MODULE_LOW_BATTERY_AUTO_POWER_OFF_BT_SUPPORT
    if(uteModuleCallData.isPowerOn && (uteDrvBatteryCommonGetLvl() < UTE_DRV_BATTERY_LOW_POWER_PERECNT))
    {
        uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_AUTO);
        uteModuleCallData.isBtAutoClose = true;
    }
#endif

    if(uteModuleCallData.isPowerOn)
    {
        // 防止状态错误
        if (!bt_get_scan())
        {
            bsp_bt_trun_on();
        }
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
        if(!uteModuleCallIsEntertranmentVoiceOn() && bt_a2dp_profile_completely_connected())
        {
            bt_a2dp_profile_dis();
        }
        else if (bt_hfp_is_connected() && uteModuleCallIsEntertranmentVoiceOn() && !bt_a2dp_profile_completely_connected())
        {
            bt_a2dp_profile_en();
        }
#endif
        uteModuleCallData.powerOnTimeSecond++;
        if(isHfpAndA2dpProfileBothConnected())
        {
            uteModuleCallData.powerOnTimeSecond = 0;
            uint disp_status = bsp_bt_disp_status();
            if(disp_status == BT_STA_INCALL)
            {
                uteModuleCallData.callData.callingTimeSecond ++;
            }
        }
        else
        {
            if (uteModuleCallData.isBleLinkBackFlag) // 回连ble时回连BT
            {
                uteModuleCallData.isBleLinkBackFlag = false;
                if(!bt_is_connected() && bt_get_curr_scan() == BT_STA_SCANNING)
                {
                    printf("%s,ble link back,bt connect\r\n", __func__);
                    bt_connect();
                }
            }
            UTE_MODULE_LOG(UTE_LOG_BT_AUDIO_LVL, "%s,.powerOnTimeSecond = %d", __func__, uteModuleCallData.powerOnTimeSecond);
        }

#if UTE_BT30_AUTO_POWRER_OFF_SUPPORT
        if (uteModuleCallData.powerOnTimeSecond > (UTE_BT30_AUTO_POWRER_OFF_TIME_SECOND + 10)&&(!isHfpAndA2dpProfileBothConnected()))
        {
            uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_AUTO);
            uteModuleCallData.isBtAutoClose = true;
        }
#endif
    }
    else
    {
        uteModuleCallData.powerOnTimeSecond = 0;
        // 防止状态错误
        if (bt_get_scan())
        {
            bsp_bt_trun_off();
        }
    }
}

/**
*@brief  call 初始化
*@details
*@param[in] uint16_t type ，消息类型
*@author        zn.zeng
*@date        2021-11-06
*/
void uteModuleCallInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_CALL_DATA_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CALL_DATA_DIR, NULL, NULL);
#endif
    uteModuleSystemtimeRegisterSecond(uteModuleCallEverySecond);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,line=%d,reset mac address",__func__,__LINE__);
    memset(&uteModuleCallData,0,sizeof(ute_module_call_data_t));
    uteModuleCallData.isPowerOn = false;

    uteModuleCallBtReadConfig();
#if UTE_MODULE_BT_ONCE_PAIR_CONNECT_SUPPORT
    uteModuleCallBtReadCtrlAppData();
#endif
#if UTE_MODULE_BT_POWER_STATUS_SAVE_SUPPORT
    uteModuleCallReadBtPowerOnOffStatus();
    uteModuleCallIsBtAutoCloseReadConfig();

    bsp_change_bt_mac();

#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
    uteModuleCallEntertranmentVoiceSwitchReadConfig();
#endif
    uteModuleCallData.isEntertranmentVoiceOn =true;

    if(uteModuleCallData.isPowerOn || uteModuleCallData.isBtAutoClose)
    {
        uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
    }
    else
    {
        uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_BUTTON);
    }
#else
    uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
#endif
}

/**
*@brief   获取BT蓝牙名
*@details
*@param[out] uint8_t *name
*@return    void
*@author    casen
*@date      2022-03-26
*/
void uteModuleCallGetBtDevName(uint8_t *name,uint8_t limitSize)
{
    uint8_t nameLen = strlen((char *)&uteModuleBtDeviceName);
    if(limitSize>nameLen)
    {
        limitSize = nameLen;
    }
    memcpy(name,uteModuleBtDeviceName,limitSize);
}

/**
*@brief   设置BT蓝牙名
*@details
*@param[out] uint8_t *name
*@return    void
*@author    casen
*@date      2022-03-26
*/
void uteModuleCallSetBtDevName(uint8_t *name,uint8_t size)
{
    if(size > (sizeof(xcfg_cb.bt_name)-1))
    {
        size = sizeof(xcfg_cb.bt_name) - 1;
    }
    memset(xcfg_cb.bt_name,0x00,sizeof(xcfg_cb.bt_name));
    memcpy(xcfg_cb.bt_name,name,size);
}

/**
*@brief     BT设备基础信息
*@details   用于返回给APP
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author     casen
*@date       2022-03-23
*/
void uteModuleProtocolGetBtInfo(uint8_t *response,uint8_t *length)
{
    //回应0x3801 + BT3.0Name(20byte)+BT3.0MAC(6byte)+BT开关状态（1byte）+BT配对状态（1byte）
    uint8_t totalByte = 0;
    uteModuleCallGetBtDevName(&response[totalByte],GAP_DEVICE_NAME_LEN);
    totalByte += 20;
    bt_get_local_bd_addr(&response[totalByte]);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,BT Mac:",__func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,&response[totalByte],6);
    totalByte += 6;
    response[totalByte] = uteModuleCallBtIsPowerOn();
    totalByte += 1;
    if(memcmp(uteModuleCallData.address,"\x00\x00\x00\x00\x00\x00",6)!=0)
    {
        response[totalByte] = 1; //配对过设备
    }
    else
    {
        response[totalByte] = 0; //未配对过设备
    }
    totalByte += 1;
    response[totalByte] = isHfpAndA2dpProfileBothConnected();//uteModuleCallData.isConnected;
    totalByte += 1;
    (*length) = totalByte;
}

/*****
*@brief     获取当前链接的手机是否是ios
*@details
*@return true是iPhone，否则不是
*@author     ellison
*@date       2023-03-24
*/
bool uteModuleCallIsCurrentConnectionIphone(void)
{
    return (uteModuleCallAppCtrlData.phoneOS==0x01);
}

/**
 * @brief        获取当前是否有连接过手机
 * @details
 * @return       true有，否则没有
 * @author       Wang.Luo
 * @date         2024-12-05
 */
bool uteModuleCallIsHasConnection(void)
{
    return memcmp(uteModuleCallData.address,"\x00\x00\x00\x00\x00\x00",6);
}

/**
*@brief     控制BT设备数据
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author     casen
*@date       2022-03-23
*/
void uteModuleProtocolCtrlBT(uint8_t*receive,uint8_t length)
{
    uint8_t *response ;
    response = uteModulePlatformMemoryAlloc(255);
    memset(response,0x00,255);
    memcpy(response,receive,length);
    if(receive[1]==0x01)       //首次绑定
    {
        if(length>2)
        {
            uteModuleCallAppCtrlData.phoneOS = receive[2];      // 1:IOS
        }
        if(length>3)
        {
            uteModuleCallAppCtrlData.btAutomaticPair = receive[3];      // 是否自动配对
        }
        if(length>4)               // 判断链接ID
        {
            uint32_t tempUuid = (uint32_t)receive[4]<<24|receive[5]<<16|receive[6]<<8|receive[7];
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,.uuid = 0x%x tempUuid = 0x%x",__func__,uteModuleCallAppCtrlData.uuid,tempUuid);
            if(uteModuleCallAppCtrlData.uuid != tempUuid)           // 换设备，ID更改
            {
                uteModuleCallAppCtrlData.uuid = tempUuid;
                if(memcmp(uteModuleCallData.address,"\x00\x00\x00\x00\x00\x00",6)!=0)
                {
                    if(isHfpAndA2dpProfileBothConnected() == false)
                    {
                        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.phoneOS=%d,reset mac address",__func__,uteModuleCallAppCtrlData.phoneOS);
                        // 暂时关闭这个清零，因为现在的ios会发送这个导致底层的蓝牙佩戴被清零掉
                        if(uteModuleCallAppCtrlData.phoneOS==0x02)
                        {
                            bt_nor_delete_link_info();
                        }
                        memset(uteModuleCallData.address,0x00,6);
                    }
                }
#if UTE_MODULE_BT_LOW_BAT_NOT_ALLOW_POWER_ON_SUPPORT
                if ((uteModuleCallBtIsPowerOn() == false) && (uteDrvBatteryCommonGetLvl() >= UTE_DRV_BATTERY_LOW_POWER_PERECNT))
                {
                    uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
                }
#else
                if(uteModuleCallBtIsPowerOn() ==false)
                {
                    uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
                }
                bt_abort_reconnect(); //终止回连
#endif
                if(uteModuleCallAppCtrlData.phoneOS == 0x01)
                {
                    printf("======================>ble_bt_connect\n");
                    //一键双连
                    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_NOTIFY_ANCS_START_PAIR,0);
                    // app_phone_type_set(uteModuleCallIsCurrentConnectionIphone());
                    // bsp_change_bt_mac();
                    // ble_bt_connect();
                }
            }
        }
        uteModuleCallBtSaveCtrlAppData();
        uint8_t totalByte = 0;
        uteModuleProtocolGetBtInfo(&response[2],&totalByte);
        uteModuleProfileBle50SendToPhone(&response[0],totalByte+2);
    }
    else if(receive[1]==0x02)
    {
        uteModuleCallAppCtrlData.uuid = 0;
        if(receive[2] == 0x00) //turn off       //解绑
        {
            if(uteModuleCallBtIsPowerOn())
            {
                uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_APP_UNBIND);
            }
            else
            {
                bt_nor_delete_link_info();
            }
        }
        else
        {
#if UTE_MODULE_BT_LOW_BAT_NOT_ALLOW_POWER_ON_SUPPORT
            if ((uteModuleCallBtIsPowerOn() == false) && (uteDrvBatteryCommonGetLvl() >= UTE_DRV_BATTERY_LOW_POWER_PERECNT))
            {
                uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
            }
#else
            if(uteModuleCallBtIsPowerOn() ==false)
            {
                uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
                bt_abort_reconnect(); //终止回连
            }
#endif
        }
        uteModuleProfileBle50SendToPhone(&receive[0],3);
    }
    else if(receive[1]==0x03)
    {
        // uteModuleCallAppCtrlData.appCallForwarding = receive[3];
        uint8_t totalByte = 0;
        uteModuleProtocolGetBtInfo(&response[2],&totalByte);
        uteModuleProfileBle50SendToPhone(&response[0],totalByte+2);
    }
    uteModulePlatformMemoryFree(response);
}

/**
*@brief 获取来电转手表接听
*@details
*@author   casen
*@date     2022-03-24
*/
bool uteModuleCallBtGetAppCallForwarding(void)
{
    return uteModuleCallAppCtrlData.appCallForwarding;
}

/**
*@brief  获取允许BT通过BLE配对标志
*@details
*@author   casen
*@date     2022-03-24
*/
bool uteModuleCallBtGetAutomaticPair(void)
{
    return (uteModuleCallAppCtrlData.btAutomaticPair||uteModuleCallAppCtrlData.isSetBondFlag);
    //return uteModuleCallAppCtrlData.btAutomaticPair;
}

/**
*@brief   set
*@details
*@author   zn.zeng
*@date     2022-06-30
*/
void uteModuleCallBtSetBondFlag(bool isSet)
{
    uteModuleCallAppCtrlData.isSetBondFlag = isSet;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isSetBondFlag=%d", __func__,uteModuleCallAppCtrlData.isSetBondFlag);
}

/**
*@brief  ble 连接状态接口
*@details
*@author        zn.zeng
*@date        2022-06-10
*/
void uteModuleCallBleConnectState(bool isConnected)
{
#if UTE_BT30_AUTO_POWRER_OFF_SUPPORT
    if (isConnected)
    {
        /*Casen 22-08-04 自动休眠 & 配对地址不为空*/
        if (!uteModuleCallBtIsPowerOn() && (uteModuleCallData.isBtAutoClose == true)) //&& memcmp(uteModuleCallData.address, "\x00\x00\x00\x00\x00\x00", 6)
        {
            uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
            // bt_abort_reconnect(); //终止回连
            return;
        }
    }
#endif
    if (isConnected && !bt_is_connected() && bt_get_curr_scan() == BT_STA_SCANNING) // 回连ble时回连BT
    {
        uteModuleCallData.isBleLinkBackFlag = true;
    }
}

/**
*@brief  app 控制参数保存
*@details
*@author      casen
*@date       2022-03-24
*/
void uteModuleCallBtSaveCtrlAppData(void)
{
    void *file;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALL_CTRL_APPDATA,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&uteModuleCallAppCtrlData,sizeof(uteModuleCallAppCtrlData));
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief 控制参数读取
*@details
*@author  casen
*@date     2022-03-24
*/
void uteModuleCallBtReadCtrlAppData(void)
{
    void *file;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALL_CTRL_APPDATA,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&uteModuleCallAppCtrlData,sizeof(uteModuleCallAppCtrlData));
        uteModuleFilesystemCloseFile(file);
    }
}

#if UTE_MODULE_BT_POWER_STATUS_SAVE_SUPPORT
/**
*@brief     保存BK开关状态
*@details
*@author    xjc
*@date      2022-04-07
*/
void uteModuleCallSaveBtPowerOnOffStatus(void)
{
    printf("KSKSKSKSKS%s,len: %d\n",__func__,__LINE__);
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleCallData.isPowerOn;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BK_ON_OFF_STATUS,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief     读取BK开关状态
*@details
*@author    xjc
*@date      2022-04-07
*/
void uteModuleCallReadBtPowerOnOffStatus(void)
{
    void *file;
    uint8_t readbuff[2];
    readbuff[0] = UTE_MODULE_BT_POWER_DEFAULT_ON;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BK_ON_OFF_STATUS,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleCallData.isPowerOn = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,uteModuleCallData.isPowerOn=%d", __func__,uteModuleCallData.isPowerOn);
}
#endif


/*
*@brief  获取sco连接状态
*@details
*@author        xjc
*@date        2022-06-01
*/
bool uteModuleCallIsScoConnected(void)
{
    return sys_cb.sco_state;
}

/**
*@brief  bt power on 的状态
*@details
*@author      zn.zeng
*@date        2022-07-01
*/
bool uteModuleCallBtIsPowerOn(void)
{
    return uteModuleCallData.isPowerOn;
}

#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
/**
*@brief     连接A2DP
*@details
*@author        xjc
*@date        2022-04-27
*/
void uteModuleCallConnectA2DPProfile(void)
{
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s", __func__);
    if(bt_is_connected() && !bt_a2dp_profile_completely_connected())
    {
        bt_a2dp_profile_en();
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,bt_is_connected=%d,bt_a2dp_profile_completely_connected=%d", __func__,bt_is_connected(),bt_a2dp_profile_completely_connected());
    }
}

/**
*@brief     断开A2DP
*@details
*@author        xjc
*@date        2022-04-27
*/
void uteModuleCallDisconnectA2DPProfile(void)
{
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s", __func__);
    if(bt_a2dp_profile_completely_connected())
    {
        bt_a2dp_profile_dis();
    }
}

/**
*@brief 娱乐声音开关参数保存
*@details
*@author        xjc
*@date        2022-04-27
*/
void uteModuleCallEntertranmentVoiceSwitchSaveConfig(void)
{
    printf("KSKSKSKSKS%s,len: %d\n",__func__,__LINE__);
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleCallData.isEntertranmentVoiceOn;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_ENTERTRANMENT_VOICE_ON_OFF_STATUS,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief  娱乐声音开关参数读取
*@details
*@author        xjc
*@date        2022-04-27
*/
void uteModuleCallEntertranmentVoiceSwitchReadConfig(void)
{
    void *file;
    uint8_t readbuff[2];
    readbuff[0] = UTE_MODULE_BT_ENTERTRANMENT_VOICE_DEFAULT;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_ENTERTRANMENT_VOICE_ON_OFF_STATUS,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleCallData.isEntertranmentVoiceOn = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,.isEntertranmentVoiceOn=%d", __func__,uteModuleCallData.isEntertranmentVoiceOn);
}

/**
*@brief     设置娱乐声音开关
*@details
*@author        xjc
*@date        2022-04-27
*/
void uteModuleCallChangeEntertranmentVoiceSwitchStatus(void)
{
    uteModuleCallData.isEntertranmentVoiceOn = !uteModuleCallData.isEntertranmentVoiceOn;
    // if(uteModuleCallBtIsPowerOn())
    {
        if(uteModuleCallData.isEntertranmentVoiceOn)
        {
            uteModulePlatformSendMsgToAppTask(TO_APP_TASK_CONNECT_A2DP,0);
        }
        else
        {
            uteModulePlatformSendMsgToAppTask(TO_APP_TASK_DISCONNECT_A2DP,0);
        }
    }
}
/**
*@brief     获取娱乐声音开关
*@details
*@author        xjc
*@date        2022-04-27
*/
bool uteModuleCallIsEntertranmentVoiceOn(void)
{
    return uteModuleCallData.isEntertranmentVoiceOn;
}
#endif

/**
*@brief  来电提醒填入号码和名字
*@details
*@author        zn.zeng
*@date        2021-11-06
*/
void uteModuleCallSetInComingNumberName(uint8_t *number,uint8_t numberSize,uint8_t *name,uint8_t nameSize)
{
    if((name!=NULL)&&(nameSize!=0))
    {
        memset(&uteModuleCallData.callData.name[0],0,UTE_CALL_NAME_MAX);
        if(nameSize>UTE_CALL_NAME_MAX)
        {
            uteModuleCallData.callData.nameSize = UTE_CALL_NAME_MAX;
            uteModuleCharencodeGetUtf8String(&name[0],UTE_CALL_NAME_MAX,&uteModuleCallData.callData.name[0],(uint16_t *)&uteModuleCallData.callData.nameSize);
        }
        else
        {
            uteModuleCallData.callData.nameSize = nameSize;
            memcpy(&uteModuleCallData.callData.name[0],&name[0],uteModuleCallData.callData.nameSize);
        }
    }
    if((number!=NULL)&&(numberSize!=0))
    {
        memset(&uteModuleCallData.callData.number[0],0,UTE_CALL_DIAL_NUMBERS_MAX);
        if(numberSize>UTE_CALL_DIAL_NUMBERS_MAX)
        {
            uteModuleCallData.callData.numberSize = UTE_CALL_DIAL_NUMBERS_MAX;
            uteModuleCharencodeGetUtf8String(&number[0],UTE_CALL_DIAL_NUMBERS_MAX,&uteModuleCallData.callData.number[0],(uint16_t *)&uteModuleCallData.callData.numberSize);
        }
        else
        {
            uteModuleCallData.callData.numberSize = numberSize;
            memcpy(&uteModuleCallData.callData.number[0],&number[0],uteModuleCallData.callData.numberSize);
        }
    }
    //APP_PRINT_INFO2("%s,numberSize=%d", TRACE_STRING(__func__),uteModuleCallData.callData.numberSize);
}

/**
*@brief  BT模块填入号码和联系人名信息
*@details  number :ascii name:UTF8
*@author   casen
*@date     2021-12-08
*/
void uteModuleCallSetContactsNumberAndName(uint8_t *number,uint8_t numberSize,uint8_t *name,uint8_t nameSize)
{
    if((name!=NULL)&&(nameSize!=0))
    {
        isBtPbapNameUpdate = true;
        memset(uteModuleCallData.callData.name,0,UTE_CALL_NAME_MAX);
        if(numberSize > UTE_CALL_NAME_MAX)
        {
            uteModuleCallData.callData.nameSize = UTE_CALL_NAME_MAX;
            uteModuleCharencodeGetUtf8String(&name[0],UTE_CALL_NAME_MAX,&uteModuleCallData.callData.name[0],(uint16_t *)&uteModuleCallData.callData.nameSize);
        }
        else
        {
            uteModuleCallData.callData.nameSize = nameSize;
            memcpy(&uteModuleCallData.callData.name[0],&name[0],uteModuleCallData.callData.nameSize);
        }
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,.callData.nameSize = %d",__func__,uteModuleCallData.callData.nameSize);
    }
    if((number!=NULL)&&(numberSize!=0))
    {
        memset(&uteModuleCallData.callData.number[0],0,UTE_CALL_DIAL_NUMBERS_MAX);
        if(numberSize>UTE_CALL_DIAL_NUMBERS_MAX)
        {
            uteModuleCallData.callData.numberSize = UTE_CALL_DIAL_NUMBERS_MAX;
            uteModuleCharencodeGetUtf8String(&number[0],UTE_CALL_DIAL_NUMBERS_MAX,&uteModuleCallData.callData.number[0],(uint16_t *)&uteModuleCallData.callData.numberSize);
        }
        else
        {
            uteModuleCallData.callData.numberSize = numberSize;
            memcpy(&uteModuleCallData.callData.number[0],&number[0],uteModuleCallData.callData.numberSize);
        }
    }

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,%s,numberSize=%d", __func__, &uteModuleCallData.callData.number[0], uteModuleCallData.callData.numberSize);
}

/**
*@brief    清除号码和联系人名信息
*@details  number :ascii name:UTF8
*@author   casen
*@date     2021-12-08
*/
void uteModuleCallClearNumberAndName(void)
{
    isBtPbapNameUpdate = false;
    uteModuleCallData.callData.needClearNumberAndName = false;
    memset(&uteModuleCallData.callData.number[0],0,UTE_CALL_DIAL_NUMBERS_MAX);
    uteModuleCallData.callData.numberSize = 0;
    memset(&uteModuleCallData.callData.name[0],0,UTE_CALL_NAME_MAX);
    uteModuleCallData.callData.nameSize = 0;
}

/**
*@brief  获取拨号打电话信息
*@details
*@author        zn.zeng
*@date        2021-10-28
*/
void uteModuleCallGetData(ute_bt_call_data_t *data)
{
    memcpy(data,&uteModuleCallData.callData,sizeof(ute_bt_call_data_t));
}

/**
*@brief    准备同步通讯录
*@details
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallSyncAddressBookStart(void)
{
    uint8_t path[40];
    memset(&path[0],0,40);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,dirInfo->filesCnt = %d", __func__,dirInfo->filesCnt);
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,del file=%s", __func__,&path[0]);
    uteModuleFilesystemDelFile((char *)&path[0]);
    uteModulePlatformMemoryFree(dirInfo);
}

/**
*@brief    同步通讯录数据
*@details
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallSyncAddressBookData(uint8_t *receive,uint8_t length)
{
    uint8_t receive_onece_dataNum = receive[2];
    uint8_t *buff = (uint8_t *)uteModulePlatformMemoryAlloc(receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
    uint8_t dataIndex = 3,contactLen = 0,phoneNumLen = 0,contactSaveLen = 0;
    memset(buff,0x00,receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
    for(uint8_t i=0; i<receive_onece_dataNum; i++)
    {
        phoneNumLen = receive[dataIndex+2];
        contactLen =  receive[dataIndex+3];
        contactSaveLen = (contactLen>(UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH-1))?(UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH-1):contactLen;
        memcpy(&buff[i*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH],&receive[dataIndex+2],(phoneNumLen+contactSaveLen+2));
        dataIndex = dataIndex + (phoneNumLen + contactLen + 4);
    }
    uint8_t path[40];
    memset(&path[0],0,40);
    uint16_t contactsTotalSize = 0;
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    void *file;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY | FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&contactsTotalSize,2);

        uteModuleFilesystemSeek(file,2+(contactsTotalSize)*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&buff[0],receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
        //APP_PRINT_INFO1("name = %b",TRACE_BINARY(receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH, &buff[0]));
        contactsTotalSize += receive_onece_dataNum;
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,contactsTotalSize = %d", __func__,contactsTotalSize);
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&contactsTotalSize,2);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(buff);
}

/**
*@brief    同步通讯录结束
*@details
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallSyncAddressBookEnd(void)
{
    uteModuleCallGetAddressBookSize();
#if UTE_MODULE_SCREENS_SYNC_BOOK_SUCCESS_SUPPORT
    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SYNC_BOOK_SUCCESS_ID);
#endif
}

/**
*@brief    全部删除通讯录数据
*@details
*@author   pcm
*@date     2022-11-30
*/
void uteModuleCallDeleteAllBookData(void)
{
    uint8_t path[40];
    memset(&path[0],0,40);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,dirInfo->filesCnt = %d", __func__,dirInfo->filesCnt);
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR, UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,del file=%s", __func__,&path[0]);
    uteModuleFilesystemDelFile((char *)&path[0]);
    uteModulePlatformMemoryFree(dirInfo);
}
/**
*@brief    获取通讯录联系人个数
*@details
*@author   casen
*@date     2021-12-06
*/
uint16_t uteModuleCallGetAddressBookSize(void)
{
    uint8_t path[40];
    memset(&path[0],0,40);
    uint16_t contactsTotalSize = 0;
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR, UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    void *file;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&contactsTotalSize,2);
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,contactsTotalSize = %d", __func__,contactsTotalSize);
        uteModuleFilesystemCloseFile(file);
    }
    return contactsTotalSize;
}

/**
*@brief    解析通讯录联系人名称
*@details  data: 格式号码长度+名字长度+号码(ascii)+联系人(unicode)
           name: 名字内容，unicode 编码
                     nameLen : 名字长度
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallParseAddressBookContactNameAndNumber(uint8_t *data,uint8_t *name,uint8_t *nameLen,uint8_t *number,uint8_t *numberLen)
{
    uint8_t contactNameLen = 0, contactNumberLen = 0;
    contactNumberLen= data[0];
    contactNameLen = data[1];
    memcpy(number,&data[2],contactNumberLen);
    memcpy(name,&data[2+contactNumberLen],contactNameLen);
    *nameLen = contactNameLen;
    *numberLen = contactNumberLen;
}

/**
*@brief    获取通讯录联系人所有数据
*@details  totalLen: 记录总数
           data: 格式号码长度+名字长度+号码(ascii)+联系人(unicode)
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallGetAllAddressBookContactContent(uint16_t totalLen,ute_module_call_addressbook_t *pData)
{
    uint8_t path[40];
    memset(&path[0],0,40);
    uint8_t tempBuff[UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH];
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    void *file;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        for(uint16_t i = 0; i<totalLen; i++)
        {
            uteModuleFilesystemSeek(file,2+i*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&tempBuff[0],UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
            uteModuleCallParseAddressBookContactNameAndNumber(tempBuff,&pData[i].nameUnicode[0],&pData[i].nameUnicodeLen,&pData[i].numberAscii[0],&pData[i].numberAsciiLen);
            UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,i=%d;nameUnicodeLen=%d,numberAsciiLen=%d", __func__,i,pData[i].nameUnicodeLen,pData[i].numberAsciiLen);
        }
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief    根据号码匹配获取通讯录联系人姓名
*@details  totalLen: 记录总数
           data: 格式号码长度+名字长度+号码(ascii)+联系人(unicode)
*@author   casen
*@date     2021-12-06
*/
bool uteModuleCallGetAddressBookContactName(uint8_t *number,uint8_t numberSize,uint8_t *name,uint8_t *nameLen)
{
    uint8_t path[25];
    memset(&path[0],0,25);
    bool isMatch = false;
    uint8_t tempBuff[UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH];
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    void *file;
    uint16_t totalLen = uteModuleCallGetAddressBookSize();
//  &uteModuleCallData.callData.number[0],uteModuleCallData.callData.numberSize
    ute_module_call_addressbook_t tempData;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        for(uint16_t i = 0; i<totalLen; i++)
        {
            uteModuleFilesystemSeek(file,2+i*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&tempBuff[0],UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
            uteModuleCallParseAddressBookContactNameAndNumber(tempBuff,&tempData.nameUnicode[0],&tempData.nameUnicodeLen,&tempData.numberAscii[0],&tempData.numberAsciiLen);
            UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,i=%d;nameUnicodeLen=%d,numberAsciiLen=%d", __func__,i,tempData.nameUnicodeLen,tempData.numberAsciiLen);
            if(isBtPbapNameUpdate == true) break;
            if(memcmp(number,tempData.numberAscii,numberSize) == 0 && numberSize!=0 && numberSize == tempData.numberAsciiLen)
            {
                uteModuleCharencodeUnicodeConversionUtf8(&tempData.nameUnicode[0],tempData.nameUnicodeLen,name,(uint16_t *)nameLen,UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH);
                isMatch = true;
                break;
            }
        }
        uteModuleFilesystemCloseFile(file);
    }
    return isMatch;
}

/**
 * @brief        设置通话状态
 * @details
 * @param[in]    CALL_STATUS_T status 来电或去电
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-14
 */
void uteModuleCallSetBeforeCallStatus(uint8_t status)
{
    uteModuleCallData.callData.beforeCallStatus = status;
    uteModuleCallData.callData.callingTimeSecond = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    memcpy(&uteModuleCallData.callData.recordTime,&time,sizeof(ute_module_systemtime_time_t));
}

/**
 * @brief        重置通话计时
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-03-19
 */
void uteModuleCallResetCallingTimeSecond(void)
{
    uteModuleCallData.callData.callingTimeSecond = 0;
}

/**
 * @brief        获取当前通话时间
 * @details
 * @return       当前通话时间 秒数
 * @author       Wang.Luo
 * @date         2025-03-19
 */
uint32_t uteModuleCallGetCallingTimeSecond(void)
{
    return uteModuleCallData.callData.callingTimeSecond;
}

/**
*@brief     更新通话记录数据
*@details   挂断时调用一次
*@author    casen
*@date      2021-12-08
*/
void uteModuleCallUpdateRecordsData(void)
{
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s", __func__);
    uint16_t callRerordsSize = sizeof(ute_module_call_records_t);
    ute_module_call_records_t tempData ;
    memset(&tempData,0x00,callRerordsSize);
    uint8_t *readBuff;
    uint16_t buffSize = UTE_MODULE_CALL_RECORDS_MAX_COUNT*callRerordsSize+1;
    uint8_t recordCnt = 0;
    readBuff = uteModulePlatformMemoryAlloc(buffSize);
    memset(readBuff,0x00,buffSize);
#if UTE_BT30_CALL_SUPPORT
    if(uteModuleCallData.callData.beforeCallStatus == CALL_INCOMING)
    {
        if(uteModuleCallData.callData.callingTimeSecond>0)
        {
            tempData.callType = CALL_RECEIVED;
        }
        else
        {
            tempData.callType = CALL_MISSED;
        }
    }
    else if(uteModuleCallData.callData.beforeCallStatus == CALL_OUTGOING)
    {
        tempData.callType = CALL_DIALED;
    }
    else //Casen add 22-02-14 接通电话之后再来电无法得知具体状态的处理
    {
        goto CALL_UPDATE_END;
    }
    printf("===>%s,callType=%d\n", __func__, tempData.callType);
    uteModuleCallData.callData.beforeCallStatus = CALL_INVALID;
#endif
    if(uteModuleCallData.callData.numberSize!=0)
    {
        memcpy(&tempData.numberAscii[0],&uteModuleCallData.callData.number[0],uteModuleCallData.callData.numberSize);
        tempData.numberAsciiLen = uteModuleCallData.callData.numberSize;
    }
    if(uteModuleCallData.callData.nameSize!=0)
    {
        memcpy(&tempData.nameUnicode[0],&uteModuleCallData.callData.name[0],uteModuleCallData.callData.nameSize);
        tempData.nameUnicodeLen = uteModuleCallData.callData.nameSize;
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,nameSize=%d", __func__,uteModuleCallData.callData.nameSize);
    }
    if(uteModuleCallData.callData.numberSize==0 && uteModuleCallData.callData.nameSize == 0)
    {
        goto CALL_UPDATE_END;
    }
    //save time
    if(uteModuleCallData.callData.recordTime.year!=0)
    {
        //保存来电时间
        memcpy(&tempData.callTime,&uteModuleCallData.callData.recordTime,sizeof(ute_module_systemtime_time_t));
    }
    uint8_t path[25];
    memset(&path[0],0,25);
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_RECORDS_DIR);
    void *file;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
        if(recordCnt<=UTE_MODULE_CALL_RECORDS_MAX_COUNT)
        {
            recordCnt = readBuff[buffSize-1];
        }
    }
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,start recordCnt=%d", __func__,recordCnt);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,callType=%d", __func__,tempData.callType);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,%04d-%02d-%02d %02d:%02d", __func__,tempData.callTime.year,tempData.callTime.month,tempData.callTime.day, tempData.callTime.hour,tempData.callTime.min);
    if(recordCnt<UTE_MODULE_CALL_RECORDS_MAX_COUNT)
    {
        memcpy(&readBuff[recordCnt*callRerordsSize],&tempData,callRerordsSize);
        recordCnt++;
    }
    else
    {
        for(uint8_t i =1 ; i<UTE_MODULE_CALL_RECORDS_MAX_COUNT; i++)
        {
            memcpy(&readBuff[(i-1)*callRerordsSize],&readBuff[i*callRerordsSize],callRerordsSize);
        }
        memcpy(&readBuff[(UTE_MODULE_CALL_RECORDS_MAX_COUNT-1)*callRerordsSize],&tempData,callRerordsSize);
    }
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,end recordCnt=%d", __func__,recordCnt);
    readBuff[buffSize-1] = recordCnt;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
CALL_UPDATE_END:
    uteModuleCallData.callData.callingTimeSecond = 0;
    uteModuleCallData.callData.numberSize = 0;
    uteModuleCallData.callData.nameSize = 0;
    uteModulePlatformMemoryFree(readBuff);
}

/**
*@brief    获取通话记录数
*@details
*@author   casen
*@date     2021-12-06
*/
uint8_t uteModuleCallGetCallRecordsSize(ute_module_call_records_t *pData)
{
    uint16_t callRerordsSize = sizeof(ute_module_call_records_t);
    uint8_t path[25];
    memset(&path[0],0,25);
    uint8_t recordsCnt = 0;
    uint16_t buffSize = UTE_MODULE_CALL_RECORDS_MAX_COUNT*callRerordsSize+1;
    uint8_t *readBuff;
    readBuff = uteModulePlatformMemoryAlloc(buffSize);
    memset(readBuff,0x00,buffSize);
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_RECORDS_DIR);
    void *file;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
        recordsCnt = readBuff[buffSize-1];
    }
    if(pData != NULL)
    {
        for(uint8_t i=0; i<recordsCnt; i++)
        {
            memcpy((void*)&pData[i],&readBuff[callRerordsSize*(recordsCnt-i-1)],callRerordsSize);
        }
    }
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,recordsCnt = %d", __func__,recordsCnt);
    uteModulePlatformMemoryFree(readBuff);
    return recordsCnt;
}

/**
*@brief    删除通话记录数
*@details
*@author   xjc
*@date     2022-10-10
*/
bool uteModuleCallDeleteCallRecords(void)
{
    uint8_t path[25];
    memset(&path[0],0,25);
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_RECORDS_DIR);
    return uteModuleFilesystemDelFile((char *)&path[0]);
}

#if UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT
/**
*@brief    准备同步sos联系人
*@details  先删除sos联系人保存文件
*@author   xjc
*@date     2022-07-06
*/
void uteModuleCallSyncSosContactStart(void)
{
    char path[25];
    memset(&path[0],0,25);
    sprintf(&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_SOS_CONTACT_DATA);
    uteModuleFilesystemDelFile(&path[0]);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,del file=%s", __func__,&path[0]);
}

/**
*@brief    同步sos联系人
*@details
*@author   xjc
*@date     2022-07-06
*/
void uteModuleCallSyncSosContactData(uint8_t *receive,uint8_t length)
{
    uint8_t receive_onece_dataNum = receive[2];
    uint8_t *buff = (uint8_t *)uteModulePlatformMemoryAlloc(receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
    uint8_t dataIndex = 3,contactLen = 0,phoneNumLen = 0,contactSaveLen = 0;
    memset(buff,0x00,receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
    for(uint8_t i=0; i<receive_onece_dataNum; i++)
    {
        phoneNumLen = receive[dataIndex+2];
        contactLen =  receive[dataIndex+3];
        contactSaveLen = (contactLen>(UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH-1))?(UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH-1):contactLen;
        memcpy(&buff[i*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH],&receive[dataIndex+2],(phoneNumLen+contactSaveLen+2));
        dataIndex = dataIndex + (phoneNumLen + contactLen + 4);
    }
    char path[25];
    memset(&path[0],0,25);
    uint16_t contactsTotalSize = 0;
    sprintf(&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_SOS_CONTACT_DATA);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,path=%s", __func__,&path[0]);
    void *file;
    if (uteModuleFilesystemOpenFile(&path[0], &file, FS_O_RDONLY | FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&contactsTotalSize,2);

        uteModuleFilesystemSeek(file,2+(contactsTotalSize)*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&buff[0],receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
        //APP_PRINT_INFO1("name = %b",TRACE_BINARY(receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH, &buff[0]));
        contactsTotalSize += receive_onece_dataNum;
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,contactsTotalSize = %d", __func__,contactsTotalSize);
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&contactsTotalSize,2);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(buff);
}

/**
*@brief    获取sos联系人个数
*@details
*@author   xjc
*@date     2022-07-06
*/
uint16_t uteModuleCallGetSosContactSize(void)
{
    char path[25];
    memset(&path[0],0,25);
    uint16_t contactsTotalSize = 0;
    sprintf(&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_SOS_CONTACT_DATA);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,path=%s", __func__,&path[0]);
    void *file;
    if(uteModuleFilesystemOpenFile(&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&contactsTotalSize,2);
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,contactsTotalSize = %d", __func__,contactsTotalSize);
        uteModuleFilesystemCloseFile(file);
    }
    return contactsTotalSize;
}

/**
*@brief    获取sos联系人数据
*@details
*@author   xjc
*@date     2022-07-06
*/
void uteModuleCallGetSosContact(ute_module_call_addressbook_t *pData)
{
    char path[25];
    memset(&path[0],0,25);
    uint8_t tempBuff[UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH];
    sprintf(&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_SOS_CONTACT_DATA);
    void *file;
    if(uteModuleFilesystemOpenFile(&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,2,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&tempBuff[0],UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
        uteModuleCallParseAddressBookContactNameAndNumber(tempBuff,&pData->nameUnicode[0],&pData->nameUnicodeLen,&pData->numberAscii[0],&pData->numberAsciiLen);
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,nameUnicodeLen=%d,numberAsciiLen=%d", __func__,pData->nameUnicodeLen,pData->numberAsciiLen);
        uteModuleFilesystemCloseFile(file);

    }
}
#endif //end UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT
