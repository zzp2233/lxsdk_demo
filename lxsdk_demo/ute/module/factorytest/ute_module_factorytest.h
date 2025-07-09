/**
*@file
*@brief        工厂测试模块
*@details
*@author       zn.zeng
*@date       2021-11-23
*@version      v1.0
*/
#ifndef _UTE_MODULE_FACTORYTEST_H_
#define _UTE_MODULE_FACTORYTEST_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"
#include "ute_module_message.h"
#include "include.h"

typedef enum
{
    TEST_ITEM_NONE,
    TEST_ITEM_VCXX_CHECK_LIGHT,
    TEST_ITEM_AGING,
    TEST_ITEM_BATTERY,
    TEST_ITEM_SCREEN_RGB,
    TEST_ITEM_TOUCH,
    TEST_ITEM_GYROSCOPE,
    TEST_ITEM_MAGNETIC,
    TEST_ITEM_ALI_SE_IC,
    TEST_ITEM_GNSS,
    TEST_ITEM_GNSS_FIND_SAT,
    TEST_ITEM_NXP_NFC,
    TEST_ITEM_PSRAM,
    TEST_ITEM_MIC_SPEAKER,
    TEST_ITEM_SCREEN_HOLD_BRIGHT,
    TEST_ITEM_MOTOR,
    TEST_ITEM_PCBA_TP,
    TEST_ITEM_MAX_CNT,
} ute_module_factory_test_item_t;

enum
{
    FACTORY_VCXX_TEST_MODE_CROSSTALK,
    FACTORY_VCXX_TEST_MODE_INFRARED,
    FACTORY_VCXX_TEST_MODE_RED_LIGHT,
    FACTORY_VCXX_TEST_MODE_BIO_A,
    FACTORY_VCXX_TEST_MODE_BIO_B,
    FACTORY_VCXX_TEST_MODE_MAX,
};

typedef struct
{
    uint16_t pre;
    uint16_t current;
    uint16_t bioA;
    uint16_t bioB;
    int bioDiff;
    uint8_t ps;
    bool isPass;
    uint8_t preValueBuff[10];
    uint8_t currentValueBuff[10];
    uint8_t bioAValueBuff[12];
    uint8_t bioBValueBuff[12];
    uint8_t bioDiffValueBuff[12];
    uint8_t psValueBuff[10];
    uint8_t testMode;
} ute_module_factory_test_vcxx_t;

typedef struct
{
    uint8_t heart;
    int16_t gsensorX;
    int16_t gsensorY;
    int16_t gsensorZ;
    uint32_t runningTimeSecond;
    uint8_t heartValueBuff[6];
    uint8_t gsensorXBuff[8];
    uint8_t gsensorYBuff[8];
    uint8_t gsensorZBuff[8];
    uint8_t hourBuff[6];
    uint8_t minBuff[4];
    uint8_t secBuff[4];
} ute_module_factory_test_aging_t;

typedef struct
{
    ute_module_systemtime_time_t startTime;
    uint16_t startBatteryVoltage;
    uint8_t startBatteryLvl;
    uint16_t currBatteryVoltage;
    uint8_t currBatteryLvl;
    uint32_t runningTimeSecond;
    uint8_t hourBuff[6];
    uint8_t minBuff[4];
    uint8_t secBuff[4];
    uint8_t startBatteryVoltageBuff[8];
    uint8_t currBatteryVoltageBuff[8];
    uint8_t startBatteryLvlBuff[6];
    uint8_t currBatteryLvlBuff[6];
    uint8_t startTimeBuff[38];
    uint8_t currTimeBuff[38];
} ute_module_factory_test_battery_t;

typedef struct
{
    uint32_t runningTimeSecond;
} ute_module_factory_test_screen_t;

typedef struct
{
    uint8_t graph[9];
} ute_module_factory_test_touch_t;

typedef struct
{
    uint32_t runningTimeSecond;
} ute_module_factory_test_mic_speaker_t;

typedef struct
{
    uint8_t currItem;/*! 当前测试项 zn.zeng, 2021-11-23  */
    bool isKeysTesting;
    union
    {
        ute_module_factory_test_vcxx_t vcxx;
        ute_module_factory_test_aging_t aging;
        ute_module_factory_test_battery_t battery;
        ute_module_factory_test_screen_t screen;
        ute_module_factory_test_touch_t touch;
        ute_module_factory_test_mic_speaker_t micSpeaker;
    } u;
} ute_module_factory_test_data_t;

void uteModuleFactoryTestInit(void);
void uteModuleFactoryTestFillSupportFuntion0(uint8_t *funtion);
void uteModuleFactoryTestFillSupportFuntion1(uint8_t *funtion);
void uteModuleFactoryTestStartTestItem(ute_module_factory_test_item_t item,bool isOpen);
void uteModuleFactoryTestGetData(ute_module_factory_test_data_t *testData);
void uteModuleFactoryTestSetData(ute_module_factory_test_data_t testData);
void uteModuleFactoryTestKeysHandler(uint16_t keycode);
void uteModuleFactoryTestDisconnectHandler(void);
void uteModuleFactoryTestStop(void);
uint8_t uteModuleFactoryTestGetCurrTestItem(void);
void uteModuleFactoryTestGetVkData(int *preV,int *curV, int *psV,bool *isPass);
void uteModuleFactoryTestSetVkData(int preV,int curV, int psV);
void uteModuleFactoryTestProtocol(uint8_t*receive,uint8_t length);

void uteModuleFactoryTestSetCheckLightMode(uint8_t mode);
uint8_t uteModuleFactoryTestGetCheckLightMode(void);
#endif //_UTE_MODULE_FACTORYTEST_H_

