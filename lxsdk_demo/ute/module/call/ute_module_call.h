/**
*@file
*@brief        电话相关模块
*@details
*@author       zn.zeng
*@date       2021-10-28
*@version      v1.0
*/
#ifndef _UTE_MODULE_CALL_H_
#define _UTE_MODULE_CALL_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"
#include "ute_module_call.h"

/*! 通话模块保存目录 casen, 2021-12-06  */
#define UTE_MODULE_FILESYSTEM_CALL_DATA_DIR "calldata"
/*! 电话簿数据保存目录 casen, 2021-12-06  */
#if UTE_MODUEL_CALL_ADDRESSBOOK_EXPANSION_LENGTH_SUPPORT
#define UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR "addressbookV2.0"
#else
#define UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR "addressbook"
#endif
/*! 通话记录数据保存目录 casen, 2021-12-06  */
#define UTE_MODULE_FILESYSTEM_CALL_RECORDS_DIR "callrecords"
/*! 写入call参数  zn.zeng, 2022-01-07  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALLINFO  "systemparam/callInfo"
/*! 写入app控制参数  casen, 2022-03-24  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALL_CTRL_APPDATA  "systemparam/callCtrlAppData"
/*! BK开关状态  xjc, 2022-04-07  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_BK_ON_OFF_STATUS  "systemparam/bkStatus"
/*! 娱乐开关状态  xjc, 2022-04-27  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_ENTERTRANMENT_VOICE_ON_OFF_STATUS  "systemparam/entertranment"
/*!BT自动关闭状态  casen, 2022-08-04  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_BT_AUTO_CLOSE_STATUS  "systemparam/btAutoClose"
/*! SOS联系人保存目录 xjc, 2022-07-06  */
#define UTE_MODULE_FILESYSTEM_CALL_SOS_CONTACT_DATA "sosContact"

#define UTE_CALL_DIAL_NUMBERS_MAX 21
#define UTE_CALL_NAME_MAX 60

#if UTE_MODUEL_CALL_ADDRESSBOOK_EXPANSION_LENGTH_SUPPORT
#define UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH     41
#define UTE_MODULE_CALL_ADDRESSBOOK_NUMBER_MAX_LENGTH   16
#define UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH     57
#define UTE_MODULE_CALL_ADDRESSBOOK_MAX_COUNT           70
#else
#define UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH     21
#define UTE_MODULE_CALL_ADDRESSBOOK_NUMBER_MAX_LENGTH   16
#define UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH     37
#define UTE_MODULE_CALL_ADDRESSBOOK_MAX_COUNT           100
#endif

#define UTE_MODULE_CALL_RECORDS_MAX_COUNT               20

#if UTE_MODULE_BT_ONCE_PAIR_CONNECT_SUPPORT
typedef enum
{
    UTE_BT_DISCONNECT_IDLE   = 0x00,
    UTE_BT_DISCONNECT_CANCEL_PAIR = 0X01, //取消配对
    UTE_BT_DISCONNECT_CLOSE = 0X02, //手机关闭蓝牙
} UTE_BT_DISCONNECT_REASON;
#endif
typedef enum
{
    UTE_BT_POWER_OFF = 0,
    UTE_BT_POWER_ON_NORMAL   = 1,
    UTE_BT_POWER_ON_VOICE_ASSISTANT   = 2,
    UTE_BT_POWER_ON_FORM_BLE_CALL_NOTIFY = 3,
} ute_bt_power_on_type_t;
/*! call数据zn.zeng, 2021-10-28  */
typedef struct
{
    uint8_t number[UTE_CALL_DIAL_NUMBERS_MAX];
    uint8_t numberSize;
    uint8_t name[UTE_CALL_NAME_MAX];
    uint8_t nameSize;
    uint8_t state;
    uint32_t callingTimeSecond;
    uint8_t callingTimeString[9];
    uint8_t volumeCurrLevel;
    uint8_t volumeMinLevel;
    uint8_t volumeMaxLevel;
    bool isMicMute;
    bool isBandAnswer;//手环接通
    bool isBandDial;  //手环拨号
    uint32_t inOutToActiveRtc;//来电到接通的rtc计数
    bool isAppAudio;//是否是微信QQ登语音通话
    uint8_t beforeCallStatus;
    ute_module_systemtime_time_t recordTime;  //记录来电时间
    bool scoConnected;
    int16_t scoConnectCnt;
    bool isCallDialTimeoutFlag;  //拨号盘判断标志
    bool isGotoIncomingScreen;
    bool isAutoWakupFromBle;
    bool needClearNumberAndName;
} ute_bt_call_data_t;
/*! 语音助手状态casen , 2023-02-21  */
typedef enum
{
    UTE_BT_VOICE_ASSISTANT_IDLE  = 0x00,
    UTE_BT_VOICE_ASSISTANT_WATCH = 0X01, //手表端唤起
    UTE_BT_VOICE_ASSISTANT_PHONE = 0X02, //手机端唤起
} UTE_BT_VOICE_ASSISTANT_STATE;

/**
 * @brief       bt audio 数据结构
 * @details
 */
typedef struct
{
    uint8_t address[6];
    bool isConnected;
    bool isPowerOn; // 用于手表界面判断当前的BT是否关闭
    uint32_t powerOnTimeSecond;
    uint32_t idleTimerSecond;
    bool isPlayingA2dp;
    ute_bt_call_data_t callData;
#if UTE_MODULE_BT_ONCE_PAIR_CONNECT_SUPPORT
    UTE_BT_DISCONNECT_REASON disconnectReason;
#endif
    bool muteActive; //静音键是否有效
    uint32_t muteRecordSecond; //记录音源切换的时间
    UTE_BT_VOICE_ASSISTANT_STATE voiceAssistantListening;
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
    bool isEntertranmentVoiceOn; // true:开启,false:关闭
#endif
    bool errorRejectStatus;
    uint8_t errorRejectCnt;
    ute_bt_power_on_type_t powerOnType;
    bool isAclConnectStatus;  //acl connect 建立标志
    bool isBtAutoClose;   //表示BT自动关闭标志
    bool isWaitinglinkback; // 等待回链，true等待，false无需等待
    uint8_t isAllProfileLinkStatus; //所有的BT连接状态，用于判断当前是正在否断开了
#if UTE_MODULE_SCREENS_BT_DISCONNECT_NOTIFY_SUPPORT
    uint8_t needRemindBtDisconnectCnt;
#endif
    bool isBleLinkBackFlag;
} ute_module_call_data_t;

/**
 * @brief     电话簿数据结构
 * @details   用于电话簿列表格式化显示
 */
typedef struct
{
    uint8_t numberAscii[UTE_MODULE_CALL_ADDRESSBOOK_NUMBER_MAX_LENGTH];
    uint8_t nameUnicode[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH];
    uint8_t numberAsciiLen;
    uint8_t nameUnicodeLen;
} ute_module_call_addressbook_t;

typedef enum
{
    CALL_MISSED = 0,
    CALL_RECEIVED,
    CALL_DIALED,
} CALL_TYPE_T;

typedef enum
{
    CALL_OUTGOING = 0,
    CALL_INCOMING,
    CALL_INVALID,
} CALL_STATUS_T;

/**
 * @brief     通话记录数据结构
 * @details   用于记录列表格式化显示
 */
typedef struct
{
    uint8_t numberAscii[UTE_CALL_DIAL_NUMBERS_MAX];
    uint8_t numberAsciiLen;
    uint8_t nameUnicode[UTE_CALL_NAME_MAX];
    uint8_t nameUnicodeLen;
    CALL_TYPE_T callType; // 0:MISSED 1:RECEIVED 2:DIALED
    ute_module_systemtime_time_t callTime; //来电时间
} ute_module_call_records_t;

typedef struct
{
    bool appCallForwarding; //app 来电手表转接
    uint8_t phoneOS ;       //手机系统 0x01 IOS 0x02 android
    bool btAutomaticPair;   //BT通过BLE自主配对
    uint32_t uuid;          //唯一标识,用于区分手机
    bool isSetBondFlag;
    bool isUnbindAndClearBingdingKeys;
} ute_module_call_app_ctrl_data_t;

typedef enum
{
    UTE_BT_POWER_OFF_BUTTON = 0x00, //按键
    UTE_BT_POWER_OFF_SYSTEM_OFF = 0X01, //关机
    UTE_BT_POWER_OFF_APP_UNBIND = 0X02, //APP解绑BLE的时候，不需要特别关闭蓝牙，
    UTE_BT_POWER_OFF_AUTO = 0X03,
} UTE_BT_POWER_OFF_REASON;

#if UTE_BT30_CALL_SUPPORT
bool uteModuleCallBtIsConnected(void);
void uteModuleCallInit(void);
void uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_REASON reason);
void uteModuleCallBtPowerOn(ute_bt_power_on_type_t type);
void uteModuleCallBtUpdateKeyConnectAddress(uint8_t *addr);
#if UTE_MODULE_BT_ONCE_PAIR_CONNECT_SUPPORT
void uteModuleProtocolCtrlBT(uint8_t*receive,uint8_t length);
void uteModuleCallBtReadCtrlAppData(void);
void uteModuleCallSetBtDevName(uint8_t *name,uint8_t size);
#endif
void uteModuleCallBtSaveCtrlAppData(void);
bool uteModuleCallBtGetAutomaticPair(void);
bool uteModuleCallBtGetAppCallForwarding(void);

#if UTE_MODULE_BT_POWER_STATUS_SAVE_SUPPORT
void uteModuleCallSaveBtPowerOnOffStatus(void);
void uteModuleCallReadBtPowerOnOffStatus(void);
#endif

bool uteModuleCallIsScoConnected(void);
#endif //UTE_BT30_CALL_SUPPORT

void uteModuleCallBtSetBondFlag(bool isSet);
bool uteModuleCallBtIsPowerOn(void);
bool uteModuleCallGetIsBtAutoClose(void);

bool uteModuleCallIsCurrentConnectionIphone(void);
bool uteModuleCallIsHasConnection(void);
void uteModuleCallBleConnectState(bool isConnected);

void uteModuleCallUpdateRecordsData(void);
void uteModuleCallSetInComingNumberName(uint8_t *number,uint8_t numberSize,uint8_t *name,uint8_t nameSize);
void uteModuleCallSetContactsNumberAndName(uint8_t *number,uint8_t numberSize,uint8_t *name,uint8_t nameSize);
void uteModuleCallGetAllAddressBookContactContent(uint16_t totalLen,ute_module_call_addressbook_t *pData);
void uteModuleCallParseAddressBookContactNameAndNumber(uint8_t *data,uint8_t *name,uint8_t *nameLen,uint8_t *number,uint8_t *numberLen);
uint16_t uteModuleCallGetAddressBookSize(void);
void uteModuleCallDeleteAllBookData(void);
void uteModuleCallSyncAddressBookEnd(void);
void uteModuleCallSyncAddressBookData(uint8_t *receive,uint8_t length);
void uteModuleCallSyncAddressBookStart(void);
void uteModuleCallClearNumberAndName(void);
bool uteModuleCallGetAddressBookContactName(uint8_t *number,uint8_t numberSize,uint8_t *name,uint8_t *nameLen);
void uteModuleCallSetBeforeCallStatus(uint8_t status);
void uteModuleCallResetCallingTimeSecond(void);
uint32_t uteModuleCallGetCallingTimeSecond(void);
void uteModuleCallGetData(ute_bt_call_data_t *data);
uint8_t uteModuleCallGetCallRecordsSize(ute_module_call_records_t *pData);
bool uteModuleCallDeleteCallRecords(void);
void uteModuleCallIsBtAutoCloseSaveConfig(void);

#if UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT
void uteModuleCallSyncSosContactStart(void);
void uteModuleCallSyncSosContactData(uint8_t *receive,uint8_t length);
uint16_t uteModuleCallGetSosContactSize(void);
void uteModuleCallGetSosContact(ute_module_call_addressbook_t *pData);
#endif

#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
void uteModuleCallConnectA2DPProfile(void);
void uteModuleCallDisconnectA2DPProfile(void);
void uteModuleCallEntertranmentVoiceSwitchSaveConfig(void);
void uteModuleCallEntertranmentVoiceSwitchReadConfig(void);
void uteModuleCallChangeEntertranmentVoiceSwitchStatus(void);
bool uteModuleCallIsEntertranmentVoiceOn(void);
#endif

#endif //_UTE_MODULE_BT_AUDIO_H_
