/**
*@file
*@brief       application 公共层头文件
*@details  application 公共层，链接各模块
*@author       zn.zeng
*@date       Jul 01, 2021
*@version      v1.0
*/


#ifndef _UTE_APPLICATION_COMMON_H_
#define _UTE_APPLICATION_COMMON_H_
#include "ute_module_platform.h"
#include "ute_project_config.h"
//#include "ute_module_heart.h"
#include "ute_module_filesystem.h"
#include "ute_module_systemtime.h"
//#include "ute_language_common.h"

/** 系统参数文件夹，写入闹钟，亮屏时长等参数*/
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_DIR "systemparam"
/*保存部分log数据目录 zn.zeng 2021-12-29*/
#define UTE_MODULE_FILESYSTEM_LOG_DIR "log"
/*!写入个人信息参数 zn.zeng, 2021-08-19  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_PERSONALINFO  "systemparam/personalInfo"
/*!写入显示信息参数 zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_DISPLAYINFO  "systemparam/displayinfo"
/*!写入快捷开关状态 zn.zeng, 2021-08-23  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_QUICK_SWITCHINFO "systemparam/quickSwitchInfo"
/*写入连接过app标志 zn.zeng 2022-01-13*/
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_HAS_CONNECT_OUR_APP "systemparam/hasConnectOurApp"
/*写入软件版本号 xjc 2022-05-05*/
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_SOFTWARE_VERSION "systemparam/softVersion"
/*写入NFC开关,用于图标显示 ldl 2023-08-30*/
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_NFC_SWITCH "systemparam/nfcSwitch"
/*! 写入一级界显示开关、顺序,wang.luo 2024-11-16 */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_SCREEN_TBL_SORT "systemparam/screenTblSort"
/*! 写入重启原因,wang.luo 2024-12-30 */
#define UTE_MODULE_FILESYSTEM_RESTART_INFO_DIR "restartInfo"

/*快捷开关表! zn.zeng, 2021-08-24  */
#define    QUICK_SWITCH_FINDBAND   0x01000000
#define    QUICK_SWITCH_TURNTHEWRIST   0x02000000
#define    QUICK_SWITCH_SEDENTARY   0x04000000
#define    QUICK_SWITCH_NOT_DISTURB   0x08000000
#define    QUICK_SWITCH_ANTILOST  0x10000000
#define    QUICK_SWITCH_REJECT_CALL    0x20000000
#define    QUICK_SWITCH_24H_HEART   0x40000000
#define    QUICK_SWITCH_DISPLAY_TIME   0x80000000
#define    QUICK_SWITCH_SHOCK_TIME   0x00010000
#define    QUICK_SWITCH_NOT_DISTURB_MODE   0x00020000
#define    QUICK_SWITCH_GOAL_REACH   0x00040000
#define    QUICK_SWITCH_DRINK_WATER   0x00080000
#define    QUICK_SWITCH_HR_ABNORMAL_WARNING   0x00100000
#define    QUICK_SWITCH_LOCAL_WURAO_SET_TIME   0x00200000
#define    QUICK_SWITCH_LOCAL_IS24HOUR   0x00400000


/*! 个人信息数据结构zn.zeng, 2021-07-20  */
typedef struct
{
    bool isMan;/*! 男，女zn.zeng, 2021-07-20  */
    uint8_t age;/*! 年龄zn.zeng, 2021-07-20  */
    uint8_t height;/*! 身高cm zn.zeng, 2021-07-20  */
    uint32_t weight;/*! 体重 单位是g zn.zeng, 2021-07-20  */
} ute_personal_info_t;
/*! ble连接状态zn.zeng, 2021-08-05  */
typedef struct
{
    uint8_t connId;/*! 连接id zn.zeng, 2021-08-05  */
    bool isConnected;
    bool isParired;/*! 是否配对 zn.zeng, 2021-11-17  */
    bool hasUpdateConnectionParam ; /*! 已经发起请求, casen 2021-11-30  */
    uint32_t connectedSecond;
} ute_ble_connect_state_t;

typedef struct
{
    bool isFindband;             //查找手环
    bool isTurnTheWrist;   //翻腕亮屏开关
    //bool isSedentary;      //久坐提醒开关
    bool isNotDisturb;      //勿扰开关
    bool isFindPhone;      //查找手机开关
    bool isRejectCall;         //长按拒接开关
    bool isHeart24h;       //24小时开关
    bool isDisplayTime;       //可以设置亮屏时间
    bool isShockTime;         //可以震动单次时长
    bool isGoalReach;         //目标达成提示开关
    //bool isDrinkWater;      //喝水提醒开关
    //bool isHrAbnormalWarnning;   //心率异常告警开关
} ute_quick_switch_t;
/*! 发送历史数据结构zn.zeng, 2021-08-25  */
typedef struct
{
    uint8_t crc;
    bool isAllData;
    uint8_t currSendFileIndex;
    uint8_t currSendHourIndex;
    uint16_t currSendMinIndex;
    uint16_t currSendMaxIndex;
    uint32_t currSendFileSize;
    uint32_t currSendFileDataOffset;
    uint16_t currSendMtuSize; //单条能发送的最大长度
    ute_module_filesystem_dir_t dirInfo;
    ute_module_systemtime_time_t time;
    //uint8_t dataBuff[2884];
    uint16_t dataBuffSize;
} ute_application_sync_data_param_t;

#if 1//UTE_FACTORY_CALIBRATION_GSENSOR_SUPPORT //defalut open
typedef struct
{
    int16_t offsetX;
    int16_t offsetY;
    int16_t offsetZ;
} gsensor_offset_data_t;
#endif

typedef struct
{
    // 新增变量必须加在最后面
    uint32_t userId;
    gsensor_offset_data_t gSensorCalibrationData; // g-sensor 校验数据
    uint8_t sc7a20hRetReadValue;
    uint8_t sc7a20hTempBuffer[29];
    uint8_t bioSaveBuf[10];
    uint8_t beforeFactoryBatLvl;
    uint8_t bleDevNameLen;
    uint8_t bleDevName[32];
} ute_application_sn_data_t;

typedef void (*ute_module_sync_data_reg_func_t)(void);

#if UTE_APP_READ_CUSTOM_SOFTWARE_VERSON_SUPPORT
typedef struct
{
    bool isCustF6Send;
    uint8_t custF6CmdCnt;
    uint8_t custF6CmdCrc;
    uint8_t custF6SendCrc;
    uint8_t custF6SendCnt;
    uint8_t custF6SendOffset;
    uint8_t custF6Cmds[108];
    uint8_t custF6Send[108];
} ute_custom_f6_cmd_t;
#endif
/*! app 公共数据结构zn.zeng, 2021-07-15  */
typedef struct
{
    bool isStartupFristFinish;
    bool isStartupSecondFinish;
    bool isPowerOn;
    ute_personal_info_t personalInfo;
    ute_ble_connect_state_t bleConnectState;
    ute_module_sync_data_reg_func_t syncDataFunction;/*! 读取历史数据函数指针zn.zeng, 2021-08-23  */
    bool isSyncTimerRunning;
    ute_quick_switch_t quickSwitch;
    uint16_t mtuSize;
    uint32_t systemPowerOnSecond;
    bool isHasConnectOurApp;
    bool isSynchronizingData;
    uint8_t softwareVersion[7];
#if SET_THE_LOW_BATTERY_POPUP_TIME
    uint32_t lowBatterySecond;/*! 低电计时器chengjie.huang,2022-09-08*/
#endif
#if UTE_MODULE_LOCAL_QUICK_SWITCH_SUPPORT
    ute_local_quick_switch_t localQuickSwitch;
#endif
#if UTE_APP_READ_CUSTOM_SOFTWARE_VERSON_SUPPORT
    ute_custom_f6_cmd_t customF6Cmd;
#endif
#if UTE_MODULE_POWERON_LANGUAGE_SELECT_SUPPORT
    bool IsFirtSetting;  /*首次开机设置语言标志 yrc,2021-10-12*/
#endif
#if UTE_DRV_POWER_VDD_CTRL_SUPPORT
    int8_t VddPowerEnableCnt;
#endif
    bool isAppClosed; /*! APP连接断开,wang.luo 2022-11-11 */
    bool isAncsConnected;
    bool nfcSwitchOpen;/*NFC开关，用于图标显示*/
} ute_application_common_data_t;

void uteApplicationCommonStartupFrist(void);
void uteApplicationCommonStartupSecond(void);
bool uteApplicationCommonIsStartupFinish(void);
bool uteApplicationCommonIsPowerOn(void);
void uteApplicationCommonGetPersonalInfo(ute_personal_info_t *info);
void uteApplicationCommonSetBleConnectState(uint8_t connid,bool isConnected);
void uteApplicationCommonSetBlePariring(bool isSuccess);
void uteApplicationCommonEverySecond(void);
void uteApplicationCommonGetBleConnectionState(ute_ble_connect_state_t *state);
void uteApplicationCommonReadConfig(void);
void uteApplicationCommonSavePersonalInfo(ute_personal_info_t personalInfo);
void uteApplicationCommonStartPowerOffMsg(void);
void uteApplicationCommonRealPowerOffMsg(void);
void uteApplicationCommonPowerOn(void);
void uteApplicationCommonSyncDataTimerCallback(void *pxTimer);
void uteApplicationCommonRegisterSyncDataTimerFunction(void *function);
void uteApplicationCommonSyncDataTimerStart(void);
void uteApplicationCommonSyncDataTimerStop(void);
void uteApplicationCommonSyncDataTimerMsg(void);
void uteApplicationCommonSendQuickSwitchStatus(void);
void uteApplicationCommonSetQuickSwitchStatus(ute_quick_switch_t *quickSwitch);
void uteApplicationCommonGetQuickSwitchStatus(ute_quick_switch_t *quickSwitch);
void uteApplicationCommonSetQuickSwitchStatusFromApp(uint8_t *pData);
void uteApplicationCommonGetSyncDataParam(ute_application_sync_data_param_t **param);
void uteApplicationCommonKeysPressEvent(uint32_t param);
void uteApplicationCommonSetMtuSize(uint16_t mtu);
uint16_t uteApplicationCommonGetMtuSize(void);
void uteApplicationCommonSetHasUpdateConnectionParam(bool hasUpdateConnectionParam);
bool uteApplicationCommonGetHasUpdateConnectionParam(void);
uint32_t uteApplicationCommonGetSystemPowerOnSecond(void);
void uteApplicationCommonSaveHasConnectOurApp(void);
bool uteApplicationCommonIsHasConnectOurApp(void);
bool uteApplicationCommonGetIsSynchronizingData(void);
void uteApplicationCommonSaveSoftwareVersionInfo(void);
void uteApplicationCommonReadSoftwareVersion(void);
void uteApplicationCommonCheckSoftwareVersion(void);
#if SET_THE_LOW_BATTERY_POPUP_TIME
uint32_t uteApplicationCommonGetlowBatterySecond(void);//hcj add 2022/09/08
void uteApplicationCommonSetlowBatterySecond(uint32_t lowBatterySecond);//hcj add 2022/09/08
#endif
#if UTE_MODULE_LOCAL_QUICK_SWITCH_SUPPORT
void uteApplicationCommonLocalQuickSwitchReadConfig(void);
void uteApplicationCommonSaveLocalQuickSwitchConfig(void);
void uteApplicationCommonSetLocalQuickSwitchStatus(ute_local_quick_switch_t localquickSwitch);
void uteApplicationCommonGetLocalQuickSwitchStatus(ute_local_quick_switch_t *localquickSwitch);
#endif
#if UTE_APP_READ_CUSTOM_SOFTWARE_VERSON_SUPPORT
void uteApplicationCommonReadCustomSoftwareVersion(uint8_t*receive,uint8_t length);
#endif
#if UTE_MODULE_POWERON_LANGUAGE_SELECT_SUPPORT
bool uteApplicationCommonIsFirtSetting(void);
#endif
#if UTE_DRV_POWER_VDD_CTRL_SUPPORT
void uteApplicationCommonSetVddPower(bool isEnable);
void uteApplicationCommonVddPowerEverySecond(void);
#endif
void uteApplicationCommonSetAppClosed(bool isAppClosed);
bool uteApplicationCommonIsAppClosed(void);
void uteApplicationCommonFactoryReset(void);
void uteApplicationCommonPoweroff(void);
void uteApplicationCommonRestart(void);

void uteApplicationCommonSetAncsConnStatus(bool isConnected);
bool uteApplicationCommonGetAncsConnStatus(void);
uint8_t uteApplicationCommonGetDeviceQrCodeLink(char *qrBuff,uint8_t len);
void uteModuleHardfaultInfoSave(void);
void uteModuleHardfaultStartSendlogData(void);

float ExactDecimalPoint(float data,uint8_t bit);

#endif //_UTE_APPLICATION_COMMON_H_

