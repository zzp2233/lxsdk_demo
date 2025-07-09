/**
*@file
*@brief        本地铃声处理模块
*@details
*@author       dengli.lu
*@date       2022-02-25
*@version      v1.0
*/
#ifndef _UTE_MODULE_LOCALRINGTONE_H_
#define _UTE_MODULE_LOCALRINGTONE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
/*! 本地铃声数据 dengli.lu, 2022-04-24  */
#define UTE_MODULE_FILESYSTEM_LOCAL_RINGTONE_INFO "systemparam/localringtone"


/*!其他铃声开关状态  pcm, 2022-11-11  */
#define UTE_MODULE_FILESYSTEM_OTHER_LOCAL_RINGTONE_POWER_STATUS  "systemparam/otherringtone"

#ifndef UTE_MODULE_LOCALRINGTONE_VOLUME_MAX
#define UTE_MODULE_LOCALRINGTONE_VOLUME_MAX 15
#endif


typedef enum
{
    RINGTON_TYPE_NONE = 0,
    RINGTON_TYPE_CALL, /*来电*/
    RINGTON_TYPE_ALARM, /*闹钟*/
    RINGTON_TYPE_FIND, /*查找*/
    RINGTON_TYPE_FACTORY, /*工厂测试*/
} ute_local_ringtone_type;

typedef struct
{
    uint8_t index;
    uint16_t duration;//秒为单位
    uint16_t totalTime;   //秒为单位
} ute_local_ringtone_set_info_t;
typedef struct
{
    uint8_t localRingtoneVolume;//0~15
    bool isLocalRingtoneMute;//是否静音
    bool localRingtoneMuteStatusBeforeNotdisturb;//勿扰前是否静音
    ute_local_ringtone_type playingRingType; //正在播放声音类型
    uint8_t ringResourceIndex;
    uint16_t ringResourceDuration;
    int16_t ringTime;
    bool OtherLocalRingtoneStatu;//其他铃声响铃开关，true打开，false关闭
    ute_local_ringtone_set_info_t callSetInfo;
    ute_local_ringtone_set_info_t alarmSetInfo;
    ute_local_ringtone_set_info_t findSetInfo;
    ute_local_ringtone_set_info_t factorySetInfo;
} ute_local_ringtone_data_t;

typedef struct
{
    u32 *ptr;
    u32 *len;
} ute_local_ringtone_res_addr_t;

void uteModuleLocalRingtoneInit(void);
void uteModuleLocalRingtoneReadConfig(void);
void uteModuleLocalRingtoneSaveData(void);
void uteModuleLocalRingtoneSetVolume(uint8_t localRingtoneVolume);
uint8_t uteModuleLocalRingtoneGetVolume(void);
void uteModuleLocalRingtoneSetMuteStatus(bool isLocalRingtoneMute,bool needToSaved);
bool uteModuleLocalRingtoneGetMuteStatus(void);
void uteModuleLocalRingtoneSetMuteStatusBeforeNotdisturb(bool localRingtoneMuteStatusBeforeNotdisturb);

void uteModuleLocalRingtoneSetPlayRingType(ute_local_ringtone_type type);
ute_local_ringtone_type uteModuleLocalRingtoneGetPlayRingType(void);
void uteModuleLocalRingtonePlayRingDetail(void);
void uteModuleLocalRingtoneStopRingDetail(void);
void uteModuleLocalRingtonePlayRingTimerStop(void);
void uteModuleLocalRingtoneStopRing(void);
void uteModuleLocalRingtonePlayRing(ute_local_ringtone_type playingRingType);
void uteModuleLocalGetRingtoneInfo(ute_local_ringtone_type ringType,ute_local_ringtone_set_info_t *info);
void uteModuleLocalSetRingtoneInfo(ute_local_ringtone_type ringType,ute_local_ringtone_set_info_t info);
void uteModuleLocalGetRingtoneResInfo(ute_local_ringtone_type ringType, uint32_t *resAddr, uint32_t *resLen);

/*! 其他铃声的开关状态,包括查找手机、闹钟铃声开关 pcm, 2022-11-11  */
#if UTE_MODULE_OTHER_RING_POWER_SUPPORT
void uteModuleLocalRingtoneSaveOtherPowerStatus(void);
void uteModuleLocalRingtoneReadOtherPowerStatus(void);
bool uteModuleLocalRingtoneGetOtherPowerStatus(void);
void uteModuleLocalRingtoneSetOtherPowerStatus(bool isOtherPowerStatus);
#endif

#endif
