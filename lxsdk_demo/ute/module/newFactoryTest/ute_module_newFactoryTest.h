/**
*@file
*@brief        老化测试处理模块
*@details
*@author       xjc
*@date       2022-02-09
*@version      v1.0
*/
#ifndef _UTE_MODULE_NEW_FACTORY_TEST_H_
#define _UTE_MODULE_NEW_FACTORY_TEST_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"
#include "ute_module_factorytest.h"
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT

typedef enum
{
    FACTORY_TEST_MODE_NULL = 0,
    FACTORY_TEST_MODE_AGING = 1,    // 1: 工厂老化模式
    FACTORY_TEST_MODE_BAT = 2,      // 2: 电池模式
    FACTORY_TEST_MODE_FACTORY = 3,  // 3: 工厂测试模式
    FACTORY_TEST_MODE_AUDIO = 4,    // 4: 工厂音频测试模式
    FACTORY_TEST_MODE_RF_DUT = 5,   // 5: 工厂射频测试模式
    FACTORY_TEST_MODE_PC_AUDIO = 6, // 6: 工厂音频调试模式
    FACTORY_TEST_MODE_SHIP = 7,     // 7: 船运模式
    FACTORY_TEST_MODE_VITA_HUB = 8, // 8：长桑血压模块
} FACTORY_TEST_MODE;

typedef enum
{
    FACTORY_AGING_TEST_MODE1 = 0,
    FACTORY_AGING_TEST_MODE2 = 1,
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
    FACTORY_AGING_TEST_MODE3 = 2,
#endif
    FACTORY_AGING_TEST_MODE_MAX,
} FACTORY_AGING_TEST_MODE;

typedef enum
{
    VITA_HUB_TEST_NULL = 0,
    VITA_HUB_TEST_BP,
#if UTE_DRV_VITA_ECG_SUPPORT
    VITA_HUB_TEST_ECG,
#endif
    VITA_HUB_BP_CERTIFICATION,
} VITA_HUB_TEST_MODE;

typedef enum
{
    FACTORY_MODULE_VERSION= 0,
    FACTORY_MODULE_CROSS,
    FACTORY_MODULE_RGB,
    FACTORY_MODULE_TP,
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
    FACTORY_MODULE_HEART_CHECK_LIGHT,
#endif
#if UTE_MODULE_HEART_SUPPORT
    FACTORY_MODULE_HEART,
#endif
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
    FACTORY_MODULE_SPO2,
#endif
#if UTE_MODULE_BLOODPRESSURE_SUPPORT && UTE_MODULE_NEW_FACTORY_MODULE_BLOODPRESSURE_SUPPORT
    FACTORY_MODULE_BP,
#endif
#if UTE_MODULE_SPORT_SUPPORT
    FACTORY_MODULE_GSENSOR,
#endif
    FACTORY_MODULE_MOTOR,
    FACTORY_MODULE_CHARGING,
    FACTORY_MODULE_MIC_SPEAKER,
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
    FACTORY_MODULE_SPEAKER,
#endif
    FACTORY_MODULE_KEY,
    FACTORY_MODULE_MAX,
} FACTORY_MODULE_TYPE_T;

#ifndef UTE_MODULE_NEW_FACTORY_MODULE_MAX
#define UTE_MODULE_NEW_FACTORY_MODULE_MAX FACTORY_MODULE_MAX
#endif

typedef enum
{
    MODULE_TEST_RESULT_NULL = 0,
    MODULE_TEST_RESULT_PASS = 1,
    MODULE_TEST_RESULT_FAIL = 2,
} FACTORY_MODULE_TEST_RESULT_T;

enum
{
    NEW_FACTORY_VCXX_TEST_MODE_CROSSTALK,
    NEW_FACTORY_VCXX_TEST_MODE_INFRARED,
    NEW_FACTORY_VCXX_TEST_MODE_RED_LIGHT,
    NEW_FACTORY_VCXX_TEST_MODE_BIO_A,
    NEW_FACTORY_VCXX_TEST_MODE_BIO_B,
    NEW_FACTORY_VCXX_TEST_MODE_MAX,
};

typedef enum
{
    FACTORY_AGING_TIME_2_HOUR = 0,
    FACTORY_AGING_TIME_4_HOUR,
    FACTORY_AGING_TIME_6_HOUR,
    FACTORY_AGING_TIME_9_HOUR,
    FACTORY_AGING_TIME_12_HOUR,
    FACTORY_AGING_TIME_18_HOUR,
    FACTORY_AGING_TIME_24_HOUR,
    FACTORY_AGING_TIME_48_HOUR,
    FACTORY_AGING_TIME_MAX,
} FACTORY_AGING_TIME_TYPE_T;

//发给APP数据的引索值
typedef enum
{
    SEND_APP_DATA_LENGTH_OFFSET = 0,
    SEND_APP_DATA_CONTENT_OFFSET = 1,
    SEND_APP_DATA_OFFSET_MAX,
} FACTORY_SEND_APP_DATA_OFFSET_T;

typedef struct
{
    uint8_t sendAppData[FACTORY_MODULE_MAX][48];
} ute_new_factory_test_send_app_data_t;

typedef struct
{
    uint8_t keyUp;
    uint8_t keyDown;
    uint8_t keyUpCnt;
    uint8_t keyDownCnt;
    uint8_t key0;
    uint8_t key1;
    uint8_t key2;
} ute_module_factory_test_key_t;

#if UTE_MODULE_NEW_FACTORY_AGING_REPORT_SUPPORT
/*! 保存多运动列表显示的运动数量跟类型 xjc, 2022-03-29  */
#define UTE_MODULE_FILESYSTEM_NEW_FACTORY_AGING_REPORT_DATA "systemparam/agingReport"
typedef struct
{
    bool isPass;
    uint8_t totalHour;
} ute_module_factory_aging_record_t;
#endif

typedef struct
{
    FACTORY_TEST_MODE mode; // 1:工厂模式, 2：电池模式, 3: 工厂测试模式
    ute_module_systemtime_time_t startTime;
    uint8_t startBatPersent;    // 开始电量 %
    uint16_t startBatVoltage;   // 开始电压 mV
    uint16_t currentBatVoltage; // 当前电压 mV
    uint32_t secondCount;       // 运行的时间 秒
    uint32_t maxCount;          // 设置的时间 秒
    FACTORY_MODULE_TYPE_T moduleType;
    uint8_t moduleResult[UTE_MODULE_NEW_FACTORY_MODULE_MAX];
    FACTORY_AGING_TEST_MODE factoryAgingTestMode;
    FACTORY_AGING_TIME_TYPE_T agingTimeType;
    uint16_t tpFirmwareCheckSum; //TP校验码,用做版本号显示
#if UTE_MODULE_NEW_FACTORY_TEST_RING_MOTOR_SCREEN_SUPPORT
    bool isMode3MotorAwaysVibration;
    bool isMode3ScreenAwaysBright;
#endif
#if UTE_MODULE_NEW_FACTORY_MODULE_TEST_ALLOW_RETEST_SUPPORT
    bool isRetesting;
#endif
    union
    {
        ute_module_factory_test_vcxx_t vcxx;
        ute_module_factory_test_mic_speaker_t micSpeaker;
    } u;
    ute_module_factory_test_key_t key;
#if UTE_MODULE_NEW_FACTORY_AGING_REPORT_SUPPORT
    ute_module_factory_aging_record_t agingRecord[FACTORY_AGING_TEST_MODE_MAX];
    bool isEnterAgingReportPage;
#endif
} ute_new_factory_test_data_t;

typedef struct
{
    uint8_t Smoke_R;     //烟弹阻值
    uint8_t Smoke_Module;//抽吸模式
    uint8_t Smoke_Hot;//发热丝状态
    uint16_t Moto_Count;//震动次数
    uint16_t Smoke_Count;//抽吸口数
    bool play_flag;
}
ute_smoke_factory_test_data_t;

void uteModuleNewFactoryTestInit(void);
void uteModuleNewFactoryTestResetParam(void);
void uteModuleNewFactoryTestSetMode(ute_new_factory_test_data_t **data);
FACTORY_TEST_MODE uteModuleNewFactoryTestGetMode(void);
void uteModuleNewFactoryTestSystemReset(void);
void uteModuleNewFactoryTestSetTpVersion(uint16_t version);
uint16_t uteModuleNewFactoryTestGetTpVersion(void);
void uteModuleNewFactoryTestEverySecond(void);
void uteModuleNewFactoryTestGetVkData(int *preV,int *curV, int *psV,bool *isPress);
void uteModuleNewFactoryTestGetSendAppDataPointer(ute_new_factory_test_send_app_data_t ** pAppData);
#if UTE_MODULE_NEW_FACTORY_TEST_RING_MOTOR_SCREEN_SUPPORT
void uteModuleNewFactorySetMode3MotorStatus(bool isMode3MotorAwaysVibration);
bool uteModuleNewFactoryGetMode3MotorStatus(void);
void uteModuleNewFactorySetMode3ScreenStatus(bool isMode3ScreenAwaysBright);
bool uteModuleNewFactoryGetMode3ScreenStatus(void);
#endif
void uteModuleNewFactoryTestSetCheckLightMode(uint8_t mode);
uint8_t uteModuleNewFactoryTestGetCheckLightMode(void);
void uteModuleNewFactoryTestGetCheckLightBioValue(uint16_t *bioA,uint16_t *bioB,uint16_t *bioDiff);

#if UTE_MODULE_SHIP_MODE_SUPPORT
void uteModuleNewFactoryTestShipModeEnter(void);
#endif

void uteModuleNewFactoryTestSetKeyValue(ute_module_factory_test_key_t key);
void uteModuleNewFactoryTestResSetKeyValue(void);
ute_module_factory_test_key_t uteModuleNewFactoryTestGetKeyValue(void);

uint16_t uteModuleNewFactoryAgingTestMotoCount(void);
uint16_t uteModuleNewFactoryAgingTestSmokeCount(void);
#if UTE_MODULE_NEW_FACTORY_AGING_REPORT_SUPPORT
void uteModuleNewFactoryTestReadAgingReportData(void);
void uteModuleNewFactoryTestSaveAgingReportData(FACTORY_AGING_TEST_MODE mode, uint8_t totalHour,bool isPass);
void uteModuleNewFactoryTestGetAgingReportData(ute_module_factory_aging_record_t *agingRecord);
void uteModuleNewFactoryTestSetEnterAgingReportPage(bool isEnterAgingReport);
bool uteModuleNewFactoryTestGetEnterAgingReportPage(void);
#endif

#endif
#endif
