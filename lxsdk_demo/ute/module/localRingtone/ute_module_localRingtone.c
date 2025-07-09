/**
*@file
*@brief        铃声音量处理模块
*@details
*@author       dengli.lu
*@date       2022-04-24
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_localRingtone.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_application_common.h"
#include "ute_drv_motor.h"
#include "ute_drv_battery_common.h"
#include "ute_module_notdisturb.h"
#include "ute_module_music.h"
#include "include.h"

ute_local_ringtone_data_t uteModuleLocalRingtoneData;
void *uteModuleLocalRingtonePlayRingTimer = NULL;

// const ute_local_ringtone_res_addr_t uteModuleLocalRingtoneNumTbl[] = UTE_MODULE_LOCAL_DEFAULT_RINGTONE;

/**
*@brief  初始化函数
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneInit(void)
{
    memset(&uteModuleLocalRingtoneData,0,sizeof(ute_local_ringtone_data_t));
    uteModuleLocalRingtoneReadConfig();
    uteModuleLocalRingtoneSetVolume(uteModuleLocalRingtoneData.localRingtoneVolume);
#if UTE_MODULE_OTHER_RING_POWER_SUPPORT
    uteModuleLocalRingtoneReadOtherPowerStatus();
#endif
}

/**
*@brief     RingVolume 读取绑定数据到内存
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneReadConfig(void)
{
    /*! 保存到文件dengli.lu, 2022-04-24*/
    void *file;
    uint8_t readbuff[23];
    memset(readbuff,0,23);
    readbuff[0] = UTE_MODULE_LOCAL_RINGTONE_DEFAULT_VOLUME;
    readbuff[1] = UTE_MODULE_LOCAL_RINGTONE_DEFAULT_MUTE;
    readbuff[2] = UTE_MODULE_LOCAL_RINGTONE_DEFAULT_MUTE;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOCAL_RINGTONE_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],23);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleLocalRingtoneData.localRingtoneVolume = readbuff[0];
    uteModuleLocalRingtoneData.isLocalRingtoneMute = readbuff[1];
    uteModuleLocalRingtoneData.localRingtoneMuteStatusBeforeNotdisturb = readbuff[2];
    uteModuleLocalRingtoneData.callSetInfo.index = readbuff[3];
    uteModuleLocalRingtoneData.callSetInfo.duration = readbuff[4]<<8|readbuff[5];
    uteModuleLocalRingtoneData.callSetInfo.totalTime = readbuff[6]<<8|readbuff[7];
    uteModuleLocalRingtoneData.alarmSetInfo.index = readbuff[8];
    uteModuleLocalRingtoneData.alarmSetInfo.duration = readbuff[9]<<8|readbuff[10];
    uteModuleLocalRingtoneData.alarmSetInfo.totalTime = readbuff[11]<<8|readbuff[12];
    uteModuleLocalRingtoneData.findSetInfo.index = readbuff[13];
    uteModuleLocalRingtoneData.findSetInfo.duration = readbuff[14]<<8|readbuff[15];
    uteModuleLocalRingtoneData.findSetInfo.totalTime = readbuff[16]<<8|readbuff[17];
    uteModuleLocalRingtoneData.factorySetInfo.index = readbuff[18];
    uteModuleLocalRingtoneData.factorySetInfo.duration = readbuff[19]<<8|readbuff[20];
    uteModuleLocalRingtoneData.factorySetInfo.totalTime = readbuff[21]<<8|readbuff[22];
    if (readbuff[3] == 0)
    {
        uteModuleLocalRingtoneData.callSetInfo.index = UTE_MODULE_BT_RINGTONE_HF_CALL_IN_INDEX;
        uteModuleLocalRingtoneData.callSetInfo.duration = UTE_MODULE_BT_LOCAL_RINGTONE_DURATION;
        uteModuleLocalRingtoneData.callSetInfo.totalTime = UTE_MODULE_BT_LOCAL_RINGTONE_DURATION*2;
    }
    if (readbuff[8] == 0)
    {
        uteModuleLocalRingtoneData.alarmSetInfo.index = UTE_MODULE_LOCAL_RINGTONE_ALARM_INDEX;
        uteModuleLocalRingtoneData.alarmSetInfo.duration = UTE_MODULE_LOCAL_RINGTONE_ALARM_DURATION;
        uteModuleLocalRingtoneData.alarmSetInfo.totalTime = UTE_MODULE_LOCAL_RINGTONE_ALARM_TIME;
    }
    if (readbuff[13] == 0)
    {
        uteModuleLocalRingtoneData.findSetInfo.index = UTE_MODULE_LOCAL_RINGTONE_FIND_INDEX;
        uteModuleLocalRingtoneData.findSetInfo.duration = UTE_MODULE_LOCAL_RINGTONE_FIND_DURATION;
        uteModuleLocalRingtoneData.findSetInfo.totalTime = UTE_MODULE_LOCAL_RINGTONE_FIND_TIME;
    }
    if (readbuff[18] == 0)
    {
        uteModuleLocalRingtoneData.factorySetInfo.index = UTE_MODULE_NEW_FACTORY_TEST_RING_INDEX;
        uteModuleLocalRingtoneData.factorySetInfo.duration = UTE_MODULE_NEW_FACTORY_TEST_RING_DURATION;
        uteModuleLocalRingtoneData.factorySetInfo.totalTime = UTE_MODULE_NEW_FACTORY_TEST_RING_TIME;
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.localRingtoneVolume=%d", __func__,uteModuleLocalRingtoneData.localRingtoneVolume);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.callSetInfo=0x%x", __func__,uteModuleLocalRingtoneData.callSetInfo.index);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.alarmSetInfo=0x%x", __func__,uteModuleLocalRingtoneData.alarmSetInfo.index);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.findSetInfo=0x%x", __func__,uteModuleLocalRingtoneData.findSetInfo.index);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.factorySetInfo=0x%x", __func__,uteModuleLocalRingtoneData.factorySetInfo.index);
}
/**
*@brief     RingVolume 保存绑定数据到内存
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneSaveData(void)
{
    /*! 保存到文件，dengli.lu, 2022-04-24*/
    void *file;
    uint8_t writebuff[23];
    writebuff[0] = uteModuleLocalRingtoneData.localRingtoneVolume;
    writebuff[1] = uteModuleLocalRingtoneData.isLocalRingtoneMute;
    writebuff[2] = uteModuleLocalRingtoneData.localRingtoneMuteStatusBeforeNotdisturb;
    writebuff[3] = uteModuleLocalRingtoneData.callSetInfo.index;
    writebuff[4] = uteModuleLocalRingtoneData.callSetInfo.duration>>8&0xff;
    writebuff[5] = uteModuleLocalRingtoneData.callSetInfo.duration&0xff;
    writebuff[6] = uteModuleLocalRingtoneData.callSetInfo.totalTime >> 8 & 0xff;
    writebuff[7] = uteModuleLocalRingtoneData.callSetInfo.totalTime & 0xff;
    writebuff[8] = uteModuleLocalRingtoneData.alarmSetInfo.index;
    writebuff[9] = uteModuleLocalRingtoneData.alarmSetInfo.duration>>8&0xff;
    writebuff[10] = uteModuleLocalRingtoneData.alarmSetInfo.duration & 0xff;
    writebuff[11] = uteModuleLocalRingtoneData.alarmSetInfo.totalTime >> 8 & 0xff;
    writebuff[12] = uteModuleLocalRingtoneData.alarmSetInfo.totalTime & 0xff;
    writebuff[13] = uteModuleLocalRingtoneData.findSetInfo.index;
    writebuff[14] = uteModuleLocalRingtoneData.findSetInfo.duration >> 8 & 0xff;
    writebuff[15] = uteModuleLocalRingtoneData.findSetInfo.duration & 0xff;
    writebuff[16] = uteModuleLocalRingtoneData.findSetInfo.totalTime >> 8 & 0xff;
    writebuff[17] = uteModuleLocalRingtoneData.findSetInfo.totalTime & 0xff;
    writebuff[18] = uteModuleLocalRingtoneData.factorySetInfo.index;
    writebuff[19] = uteModuleLocalRingtoneData.factorySetInfo.duration >> 8 & 0xff;
    writebuff[20] = uteModuleLocalRingtoneData.factorySetInfo.duration & 0xff;
    writebuff[21] = uteModuleLocalRingtoneData.factorySetInfo.totalTime >> 8 & 0xff;
    writebuff[22] = uteModuleLocalRingtoneData.factorySetInfo.totalTime & 0xff;

    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOCAL_RINGTONE_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],23);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief  设置音量
*@details
*@param[in] uint8_t localRingtoneVolume
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneSetVolume(uint8_t localRingtoneVolume)
{
    uteModuleLocalRingtoneData.localRingtoneVolume = localRingtoneVolume;
    // xcfg_cb.warning_volume = localRingtoneVolume;
}
/**
*@brief  获取音量
*@details
*@return uint8_t localRingtoneVolume
*@author        dengli.lu
*@date        2022-04-24
*/
uint8_t uteModuleLocalRingtoneGetVolume(void)
{
    return uteModuleLocalRingtoneData.localRingtoneVolume;
}
/**
*@brief  设置静音
*@details
*@param[in] bool isLocalRingtoneMute
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneSetMuteStatus(bool isLocalRingtoneMute, bool needToSaved)
{
    uteModuleLocalRingtoneData.isLocalRingtoneMute = isLocalRingtoneMute;
    if(needToSaved)
    {
        uteModuleLocalRingtoneSaveData();
    }
}
/**
*@brief  获取静音状态
*@details
*@return bool isLocalRingtoneMute
*@author        dengli.lu
*@date        2022-04-24
*/
bool uteModuleLocalRingtoneGetMuteStatus(void)
{
    return uteModuleLocalRingtoneData.isLocalRingtoneMute;
}
/**
*@brief  设置勿扰前静音状态
*@details
*@param[in] bool isLocalRingtoneMute
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneSetMuteStatusBeforeNotdisturb(bool localRingtoneMuteStatusBeforeNotdisturb)
{
    uteModuleLocalRingtoneData.localRingtoneMuteStatusBeforeNotdisturb = localRingtoneMuteStatusBeforeNotdisturb;
}

/**
*@brief     保存播放的声音类型
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtoneSetPlayRingType(ute_local_ringtone_type type)
{
    /*!来电时有其他铃声在响，停止其它铃声的定时器*/
    if(type == RINGTON_TYPE_CALL)
    {
        if(uteModuleLocalRingtoneData.playingRingType > RINGTON_TYPE_CALL)
        {
            uteModuleLocalRingtonePlayRingTimerStop();
        }
    }
    uteModuleLocalRingtoneData.playingRingType = type;
}

/**
*@brief     获取播放的声音类型
*@details
*@author    xjc
*@date      2022-05-12
*/
ute_local_ringtone_type uteModuleLocalRingtoneGetPlayRingType(void)
{
    return uteModuleLocalRingtoneData.playingRingType;
}

/**
*@brief     开始播放铃声处理函数
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtonePlayRingDetail(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
#if 0
    // app_audio_tone_play(uteModuleLocalRingtoneData.ringResourceIndex, false, false);
    if(uteModuleLocalRingtoneData.ringResourceIndex > sizeof(uteModuleLocalRingtoneNumTbl)/sizeof(uteModuleLocalRingtoneNumTbl[0]) - 1)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, ringResourceIndex is error!", __func__);
        uteModuleLocalRingtoneData.ringResourceIndex = 0;
    }
    func_bt_mp3_res_play(uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.ringResourceIndex].ptr, uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.ringResourceIndex].len);
#endif
}

/**
*@brief     停止播放铃声处理函数
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtoneStopRingDetail(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
#if 0
    uteModuleLocalRingtonePlayRingTimerStop();
    uteModuleLocalRingtoneSetPlayRingType(RINGTON_TYPE_NONE);

    printf("get_music_dec_sta:%d\n",get_music_dec_sta());
    if(get_music_dec_sta() == MUSIC_STOP)
    {

    }

    music_control(MUSIC_MSG_STOP);
#endif
}

/**
*@brief     停止播放铃声定时器
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtonePlayRingTimerStop(void)
{
    uteModulePlatformStopTimer(&uteModuleLocalRingtonePlayRingTimer);
    uteModulePlatformDeleteTimer(&uteModuleLocalRingtonePlayRingTimer);
    uteModuleLocalRingtonePlayRingTimer = NULL;
}

/**
*@brief     播放铃声定时器回调函数
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtonePlayRingTimerCallback(void *pxTimer)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
    uteModulePlatformSendMsgToAppTask(TO_APP_TASK_PLAY_RING,0);
    uteModuleLocalRingtoneData.ringTime -= uteModuleLocalRingtoneData.ringResourceDuration;
    if(uteModuleLocalRingtoneData.ringTime>0)
    {
        uteModulePlatformRestartTimer(&uteModuleLocalRingtonePlayRingTimer, uteModuleLocalRingtoneData.ringResourceDuration*1000);
    }
    else
    {
        uteModuleLocalRingtoneStopRing();
    }
}

/**
*@brief     停止播放铃声
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtoneStopRing(void)
{
    if(uteModuleLocalRingtoneData.playingRingType != RINGTON_TYPE_NONE)
    {
        uteModulePlatformSendMsgToAppTask(TO_APP_TASK_STOP_RING,0);
    }
}

/**
*@brief     开始播放铃声
*@details
*@param[in] playingRingType 铃声类型
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtonePlayRing(ute_local_ringtone_type playingRingType)
{
    if(uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_CALL)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.playingRingType is Call, return!", __func__);
        return;
    }
#if UTE_MODULE_OTHER_RING_POWER_SUPPORT
    if(!uteModuleLocalRingtoneGetOtherPowerStatus()&&(playingRingType==RINGTON_TYPE_ALARM||playingRingType==RINGTON_TYPE_FIND))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.uteModuleLocalRingtoneGetOtherPowerStatus=true return!", __func__);
        return;
    }
#endif
    uteModuleLocalRingtoneData.playingRingType = playingRingType;
    if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_CALL)
    {
        uteModuleLocalRingtoneData.ringResourceIndex = uteModuleLocalRingtoneData.callSetInfo.index;
        uteModuleLocalRingtoneData.ringResourceDuration = uteModuleLocalRingtoneData.callSetInfo.duration;
        uteModuleLocalRingtoneData.ringTime = uteModuleLocalRingtoneData.callSetInfo.totalTime;
    }
    else if(uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_ALARM)
    {
        uteModuleLocalRingtoneData.ringResourceIndex = uteModuleLocalRingtoneData.alarmSetInfo.index;
        uteModuleLocalRingtoneData.ringResourceDuration = uteModuleLocalRingtoneData.alarmSetInfo.duration;
        uteModuleLocalRingtoneData.ringTime = uteModuleLocalRingtoneData.alarmSetInfo.totalTime;
    }
    else if(uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_FIND)
    {
        uteModuleLocalRingtoneData.ringResourceIndex = uteModuleLocalRingtoneData.findSetInfo.index;
        uteModuleLocalRingtoneData.ringResourceDuration = uteModuleLocalRingtoneData.findSetInfo.duration;
        uteModuleLocalRingtoneData.ringTime = uteModuleLocalRingtoneData.findSetInfo.totalTime;
    }
    else
    {
        uteModuleLocalRingtoneData.ringResourceIndex = uteModuleLocalRingtoneData.factorySetInfo.index;
        uteModuleLocalRingtoneData.ringResourceDuration = uteModuleLocalRingtoneData.factorySetInfo.duration;
        uteModuleLocalRingtoneData.ringTime = uteModuleLocalRingtoneData.factorySetInfo.totalTime;
    }
    uteModulePlatformSendMsgToAppTask(TO_APP_TASK_PLAY_RING,0);
    if(uteModuleLocalRingtonePlayRingTimer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleLocalRingtonePlayRingTimer, "uteModuleLocalRingtonePlayRingTimer",1, 1000, false, uteModuleLocalRingtonePlayRingTimerCallback);
    }
    uteModulePlatformRestartTimer(&uteModuleLocalRingtonePlayRingTimer, uteModuleLocalRingtoneData.ringResourceDuration*1000);
#if UTE_MODULE_PLAYBACK_SUPPORT
    /*2022 08-17  闹钟开始 暂停本地音乐*/
    if(uteModuleMusicGetPlayerPaused()==false)
    {
        uteModuleMusicSetPlayerPaused(true,UTE_MUSIC_PLAY_CHANNEL_LOCAL_TO_SPEAKER);
    }
#endif
}

/**
*@brief     获取铃声信息
*@details
*@param[in] ute_local_ringtone_type ringType,铃声类型
*@param[out] ute_local_ringtone_set_info_t *info 铃声信息
*@author    zn.zeng
*@date      2022-06-19
*/
void uteModuleLocalGetRingtoneInfo(ute_local_ringtone_type ringType,ute_local_ringtone_set_info_t *info)
{
    if (ringType == RINGTON_TYPE_CALL)
    {
        memcpy(info, &uteModuleLocalRingtoneData.callSetInfo, sizeof(ute_local_ringtone_set_info_t));
    }
    else if (ringType == RINGTON_TYPE_ALARM)
    {
        memcpy(info, &uteModuleLocalRingtoneData.alarmSetInfo, sizeof(ute_local_ringtone_set_info_t));
    }
    else if (ringType == RINGTON_TYPE_FIND)
    {
        memcpy(info, &uteModuleLocalRingtoneData.findSetInfo, sizeof(ute_local_ringtone_set_info_t));
    }
    else if (ringType == RINGTON_TYPE_FACTORY)
    {
        memcpy(info, &uteModuleLocalRingtoneData.factorySetInfo, sizeof(ute_local_ringtone_set_info_t));
    }
}
/**
*@brief     设置铃声信息
*@details
*@param[in] ute_local_ringtone_type ringType,铃声类型
*@param[in] ute_local_ringtone_set_info_t info 铃声信息
*@author    zn.zeng
*@date      2022-06-19
*/
void uteModuleLocalSetRingtoneInfo(ute_local_ringtone_type ringType,ute_local_ringtone_set_info_t info)
{
    if (ringType == RINGTON_TYPE_CALL)
    {
        memcpy(&uteModuleLocalRingtoneData.callSetInfo, &info, sizeof(ute_local_ringtone_set_info_t));
    }
    else if (ringType == RINGTON_TYPE_ALARM)
    {
        memcpy(&uteModuleLocalRingtoneData.alarmSetInfo, &info, sizeof(ute_local_ringtone_set_info_t));
    }
    else if (ringType == RINGTON_TYPE_FIND)
    {
        memcpy(&uteModuleLocalRingtoneData.findSetInfo, &info, sizeof(ute_local_ringtone_set_info_t));
    }
    else if (ringType == RINGTON_TYPE_FACTORY)
    {
        memcpy(&uteModuleLocalRingtoneData.factorySetInfo, &info, sizeof(ute_local_ringtone_set_info_t));
    }
    uteModuleLocalRingtoneSaveData();
}

/**
 * @brief        获取对应铃声类型的资源文件
 * @details
 * @author       Wang.Luo
 * @date         2025-01-17
 */
void uteModuleLocalGetRingtoneResInfo(ute_local_ringtone_type ringType, uint32_t *resAddr, uint32_t *resLen)
{
#if 0
    if (ringType == RINGTON_TYPE_CALL)
    {
        *resAddr = uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.callSetInfo.index].ptr;
        *resLen = uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.callSetInfo.index].len;
    }
    else if (ringType == RINGTON_TYPE_ALARM)
    {
        *resAddr = uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.alarmSetInfo.index].ptr;
        *resLen = uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.alarmSetInfo.index].len;
    }
    else if (ringType == RINGTON_TYPE_FIND)
    {
        *resAddr = uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.findSetInfo.index].ptr;
        *resLen = uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.findSetInfo.index].len;
    }
    else if (ringType == RINGTON_TYPE_FACTORY)
    {
        *resAddr = uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.factorySetInfo.index].ptr;
        *resLen = uteModuleLocalRingtoneNumTbl[uteModuleLocalRingtoneData.factorySetInfo.index].len;
    }
#endif
}

/*! 其他铃声的开关状态,包括查找手机、闹钟铃声开关 pcm, 2022-11-11  */
#if UTE_MODULE_OTHER_RING_POWER_SUPPORT
/**
*@brief     保存其他铃声的开关状态
*@details
*@author    pcm
*@date      2022-11-11
*/
void uteModuleLocalRingtoneSaveOtherPowerStatus(void)
{
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleLocalRingtoneData.OtherLocalRingtoneStatu;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_OTHER_LOCAL_RINGTONE_POWER_STATUS,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief     读取其他铃声的开关状态
*@details
*@author    pcm
*@date      2022-11-11
*/
void uteModuleLocalRingtoneReadOtherPowerStatus(void)
{
    void *file;
    uint8_t readbuff[2];
    readbuff[0] = UTE_MODULE_OTHER_RING_POWER_DEFAULT_ON;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_OTHER_LOCAL_RINGTONE_POWER_STATUS,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleLocalRingtoneData.OtherLocalRingtoneStatu = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,uteModuleLocalRingtoneData.OtherLocalRingtoneStatu=%d", __func__,uteModuleLocalRingtoneData.OtherLocalRingtoneStatu);
}

/**
*@brief  获取其他铃声开关状态
*@details
*@return
*@author      pcm
*@date        2022-11-11
*/
bool uteModuleLocalRingtoneGetOtherPowerStatus(void)
{
    return uteModuleLocalRingtoneData.OtherLocalRingtoneStatu;
}
/**
*@brief  设置其他铃声开关状态
*@details
*@param[in]
*@author      pcm
*@date        2022-11-11
*/
void uteModuleLocalRingtoneSetOtherPowerStatus(bool isOtherPowerStatus)
{
    uteModuleLocalRingtoneData.OtherLocalRingtoneStatu = isOtherPowerStatus;
    uteModuleLocalRingtoneSaveOtherPowerStatus();
}
#endif
