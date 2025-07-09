/**
 *@file
 *@brief        老化测试处理模块
 *@details
 *@author       xjc
 *@date       2022-02-09
 *@version      v1.0
 */
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_newFactoryTest.h"
#include "ute_application_common.h"
#include "ute_drv_motor.h"
#include "ute_module_gui_common.h"
#include "ute_module_heart.h"
#include "ute_module_localRingtone.h"

#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
/*! zn.zeng,2022-03-16*/
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
// #include "ute_module_localRingtone.h"

/*! zn.zeng,2022-03-16*/
ute_new_factory_test_data_t uteModuleNewFactoryTestData;
ute_smoke_factory_test_data_t uteModuleSmokeFactoryData;

/**
 *@brief     发给对端的测试数据暂定32byte ascii 数据
 *@details   类型+测试结果+数据长度+内容
 */
ute_new_factory_test_send_app_data_t uteModuleNewFactorySendAppData;
/**
 *@brief  初始化函数
 *@details
 *@author        xjc
 *@date        2022-02-09
 */
void uteModuleNewFactoryTestInit(void)
{
    uteModuleNewFactoryTestResetParam();
    uteModuleSystemtimeRegisterSecond(uteModuleNewFactoryTestEverySecond);
}
/**
 *@brief  复位变量
 *@details
 *@author        xjc
 *@date        2022-02-09
 */
void uteModuleNewFactoryTestResetParam(void)
{
    uint16_t tpFirmwareCheckSum = 0;
    if (uteModuleNewFactoryTestData.tpFirmwareCheckSum != 0 && uteModuleNewFactoryTestData.tpFirmwareCheckSum != 0xFFFF)
    {
        tpFirmwareCheckSum = uteModuleNewFactoryTestData.tpFirmwareCheckSum;
    }
    memset(&uteModuleNewFactoryTestData, 0, sizeof(ute_new_factory_test_data_t));
    memset(&uteModuleSmokeFactoryData, 0, sizeof(ute_smoke_factory_test_data_t));
    uteModuleNewFactoryTestData.tpFirmwareCheckSum = tpFirmwareCheckSum;
}
/**
 *@brief  设置工厂模式
 *@details
 *@author        xjc
 *@date        2022-02-09
 */
void uteModuleNewFactoryTestSetMode(ute_new_factory_test_data_t **data)
{
    *data = &uteModuleNewFactoryTestData;
}
/**
 *@brief  获取测试维客数据
 *@details
 *@author     casen
 *@date       2022-02-25
 */
void uteModuleNewFactoryTestGetVkData(int *preV, int *curV, int *psV, bool *isPass)
{
    (*preV) = uteModuleNewFactoryTestData.u.vcxx.pre;
    (*curV) = uteModuleNewFactoryTestData.u.vcxx.current;
    (*psV) = uteModuleNewFactoryTestData.u.vcxx.ps;
    (*isPass) = uteModuleNewFactoryTestData.u.vcxx.isPass;
}

void uteModuleNewFactoryTestSetVkData(int preV, int curV, int psV)
{
    uteModuleNewFactoryTestData.u.vcxx.pre = preV;
    uteModuleNewFactoryTestData.u.vcxx.current = curV;
    uteModuleNewFactoryTestData.u.vcxx.ps = psV;
}

/**
 *@brief  获取工厂模式
 *@details
 *@author        xjc
 *@date        2022-02-09
 */
FACTORY_TEST_MODE uteModuleNewFactoryTestGetMode(void)
{
    return uteModuleNewFactoryTestData.mode;
}

/**
 *@brief        复位系统
 *@details
 *@author        xjc
 *@date        2022-02-09
 */
void uteModuleNewFactoryTestSystemReset(void)
{
    uteApplicationCommonFactoryReset();
}

/**
 *@brief         设置TP校验码
 *@details
 *@author        xjc
 *@date        2022-03-09
 */
void uteModuleNewFactoryTestSetTpVersion(uint16_t version)
{
    uteModuleNewFactoryTestData.tpFirmwareCheckSum = version;
}

/**
 *@brief         获取TP校验码
 *@details
 *@author        xjc
 *@date        2022-03-09
 */
uint16_t uteModuleNewFactoryTestGetTpVersion(void)
{
    return uteModuleNewFactoryTestData.tpFirmwareCheckSum;
}

/**
 *@brief  老化马达震动次数
 *@details
 *@author        lkl
 *@date        2022-02-09
 */
uint16_t uteModuleNewFactoryAgingTestMotoCount(void)
{
    return uteModuleSmokeFactoryData.Moto_Count;
}
/**
 *@brief  抽吸次数
 *@details
 *@author        lkl
 *@date        2022-02-09
 */
uint16_t uteModuleNewFactoryAgingTestSmokeCount(void)
{
    return uteModuleSmokeFactoryData.Smoke_Count;
}
/**
 *@brief        每秒函数
 *@details       需要注册到主时间，每秒执行一次
 *@author        xjc
 *@date        2022-02-09
 */
void uteModuleNewFactoryTestEverySecond(void)
{
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
    if (uteModuleNewFactoryTestData.moduleType == FACTORY_MODULE_HEART_CHECK_LIGHT)
    {
        if (uteModuleNewFactoryTestData.u.vcxx.testMode == NEW_FACTORY_VCXX_TEST_MODE_RED_LIGHT)
        {
            ute_module_systemtime_time_t time;
            uteModuleSystemtimeGetTime(&time);
            if (time.sec % 2)
            {
                // uteDrvHeartVcxxStartHrMaxTest();
            }
            else
            {
                // uteDrvHeartVcxxStartSpo2MaxTest();
            }
        }
        else if (uteModuleNewFactoryTestData.u.vcxx.testMode == NEW_FACTORY_VCXX_TEST_MODE_BIO_B)
        {
            // uteDrvHeartNewFactoryVcxxGetCrosstalktestValue(&uteModuleNewFactoryTestData.u.vcxx.pre,&uteModuleNewFactoryTestData.u.vcxx.current,&uteModuleNewFactoryTestData.u.vcxx.ps,&uteModuleNewFactoryTestData.u.vcxx.bioB,&uteModuleNewFactoryTestData.u.vcxx.isPass);
            uteModuleNewFactoryTestData.u.vcxx.bioDiff = abs(uteModuleNewFactoryTestData.u.vcxx.bioB - uteModuleNewFactoryTestData.u.vcxx.bioA);
            // if(uteModuleNewFactoryTestData.u.vcxx.bioDiff>=DRV_HEART_VCXX_BIO_PASS_VALUE)
            // {
            //     uteModuleNewFactoryTestData.u.vcxx.isPass = true;
            // }
            // else
            // {
            //     uteModuleNewFactoryTestData.u.vcxx.isPass = false;
            // }
        }
        else
        {
            // uteDrvHeartNewFactoryVcxxGetCrosstalktestValue(&uteModuleNewFactoryTestData.u.vcxx.pre,&uteModuleNewFactoryTestData.u.vcxx.current,&uteModuleNewFactoryTestData.u.vcxx.ps,&uteModuleNewFactoryTestData.u.vcxx.bioA,&uteModuleNewFactoryTestData.u.vcxx.isPass);
        }
    }
#endif
    // printf("%s,mode=%d,secondCount=%d,maxCount=%d,factoryAgingTestMode=%d\r\n", __func__, uteModuleNewFactoryTestData.mode, uteModuleNewFactoryTestData.secondCount, uteModuleNewFactoryTestData.maxCount, uteModuleNewFactoryTestData.factoryAgingTestMode);

    // printf("SCREEN_ID=%d\r\n",uteModuleGuiCommonGetCurrentScreenId());
    if ((uteModuleNewFactoryTestData.mode == FACTORY_TEST_MODE_AGING) &&
        (uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_NEW_FACTORY_AGING_ID))
    {
        if (uteModuleNewFactoryTestData.factoryAgingTestMode == FACTORY_AGING_TEST_MODE2) // 老化测试模式2
        {
            if (uteModuleNewFactoryTestData.secondCount < uteModuleNewFactoryTestData.maxCount)
            {
                // 屏幕10分钟亮一次，震动一次
                if ((uteModuleNewFactoryTestData.secondCount % (60 * 10) == 0) && (uteModuleNewFactoryTestData.secondCount > 0))
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
                    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_NEW_FACTORY_AGING_ID, 0, __func__);
                }
                // 心率5分钟测一次，每次测1分钟
            }
            else
            {
                // if(uteModuleHeartIsSingleTesting())
                {
                    // uteModuleHeartStopSingleTesting(TYPE_HEART);
                }
            }
        }
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
        else if (uteModuleNewFactoryTestData.factoryAgingTestMode == FACTORY_AGING_TEST_MODE3) // 老化测试模式3
        {
            if (uteModuleNewFactoryTestData.mode == FACTORY_TEST_MODE_AGING)
            {
                if (uteModuleNewFactoryTestData.secondCount <= uteModuleNewFactoryTestData.maxCount)
                {
                    if ((uteModuleLocalRingtoneGetPlayRingType() == RINGTON_TYPE_NONE))
                    {
                        uteModuleLocalRingtonePlayRing(RINGTON_TYPE_FACTORY);
                    }
#if UTE_MODULE_NEW_FACTORY_TEST_RING_MOTOR_SCREEN_SUPPORT
                    if (uteModuleNewFactoryTestData.isMode3MotorAwaysVibration)
                    {
                        if ((uteModuleNewFactoryTestData.secondCount % 3) == 0)
                        {
                            uteDrvMotorStart(500, 500, 1);
                        }
                    }
#endif
                }
                else
                {
                    if ((uteModuleLocalRingtoneGetPlayRingType() == RINGTON_TYPE_FACTORY))
                    {
                        uteModuleLocalRingtoneStopRing();
                    }
                }
            }
        }
#endif
        else
        {
            // 老化测试模式1
            //  if ((!uteModuleHeartIsSingleTesting()) && uteModuleGuiCommonIsDisplayOn())
            //  {
            //      // uteModuleHeartStartSingleTesting(TYPE_HEART);
            //  }

            if (uteModuleNewFactoryTestData.secondCount < uteModuleNewFactoryTestData.maxCount)
            {
                if ((uteModuleNewFactoryTestData.secondCount % 3) == 0)
                {
                    //a=1;
                    //  printf("uteModuleSmokeFactoryData.play_flag = %d\n", uteModuleSmokeFactoryData.play_flag);
                    if(uteModuleSmokeFactoryData.play_flag ==0)
                    {
                        //  printf("PlayRing\n");
                        func_bt_mp3_res_play(RES_BUF_RING_REDIAL_MP3, RES_LEN_RING_REDIAL_MP3);
                    }
                    else
                    {
                        // printf("PlayRingstop111\r\n");
                        music_control(MUSIC_MSG_STOP);
                    }
                    // uteDrvMotorStart(500,500,1);
                    //   printf("MOTOR\n");
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
                    uteModuleSmokeFactoryData.Moto_Count++;
                }

#if ECIG_POWER_CONTROL
                // printf("ecig.cfg->smoke_res_swich=%d,timer_hot_mic_work()=%d,Smoke_Count=%d\r\n",ecig.cfg->smoke_res_swich,timer_hot_mic_work(),uteModuleNewFactoryTestData.secondCount);
                if ((uteModuleNewFactoryTestData.secondCount == 1 ||
                     (uteModuleNewFactoryTestData.secondCount > 0 && uteModuleNewFactoryTestData.secondCount % 10 == 0)) && det1_start_or_not()==1)
                {
                    ecig.cfg->smoke_start_or_not = 1; // 开启烟雾
                    uteModuleSmokeFactoryData.Smoke_Count++; // 增加抽吸口数

                }
                else if (uteModuleNewFactoryTestData.secondCount == 2 ||
                         (uteModuleNewFactoryTestData.secondCount > 1 && uteModuleNewFactoryTestData.secondCount % 10 == 1))
                {
                    ecig.cfg->smoke_start_or_not = 0; // 关闭烟雾
                }
#endif
                uteModuleNewFactoryTestData.secondCount++;
            }
            else
            {
                if ((uteModuleLocalRingtoneGetPlayRingType() == RINGTON_TYPE_FACTORY))
                {
                    //  printf("PlayRingstop\r\n");
                    music_control(MUSIC_MSG_STOP);
                }
            }
        }

#if UTE_MODULE_NEW_FACTORY_AGING_REPORT_SUPPORT
        if (uteModuleNewFactoryTestData.secondCount == uteModuleNewFactoryTestData.maxCount)
        {
            uteModuleNewFactoryTestSaveAgingReportData(uteModuleNewFactoryTestData.factoryAgingTestMode, (uteModuleNewFactoryTestData.maxCount / 3600), true);
        }
#endif
    }
    /*! 在表盘界面时，如果测试模式没有关闭，则关闭测试模式 */
    if (uteModuleGuiCommonIsDisplayOn() && (uteModuleGuiCommonGetCurrentScreenId() == FUNC_CLOCK))
    {
        if (uteModuleNewFactoryTestData.mode != FACTORY_TEST_MODE_NULL)
        {
            uteModuleNewFactoryTestData.mode = FACTORY_TEST_MODE_NULL;
        }
    }
}

/**
 *@brief  获取需要发送给APP的数据指针
 *@details
 *@author     casen
 *@date       2022-03-15
 */
void uteModuleNewFactoryTestGetSendAppDataPointer(ute_new_factory_test_send_app_data_t **pAppData)
{
    // APP_PRINT_INFO1("GetSendAppDataPointer LEN = %d",sizeof(uteModuleNewFactorySendAppData));
    memset(&uteModuleNewFactorySendAppData, 0x00, sizeof(uteModuleNewFactorySendAppData));
    (*pAppData) = &uteModuleNewFactorySendAppData;
}

void uteModuleNewFactorySendAppDataFunc(void)
{
    uint8_t sendBuff[247];
    memset(sendBuff, 0, 247);
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    sendBuff[0] = CMD_FACTORY_TEST_MODE;
    sendBuff[1] = 0xff;
    sendBuff[2] = 0xf2;
    sendBuff[3] = param->currSendMinIndex >> 8 & 0xff;
    sendBuff[4] = param->currSendMinIndex & 0xff;
    if (param->currSendMinIndex > 0 && param->currSendMinIndex <= param->currSendMaxIndex)
    {
        memcpy(&sendBuff[5], &uteModuleNewFactorySendAppData.sendAppData[param->currSendMinIndex - 1][SEND_APP_DATA_CONTENT_OFFSET], uteModuleNewFactorySendAppData.sendAppData[param->currSendMinIndex - 1][SEND_APP_DATA_LENGTH_OFFSET]);
    }
    uteModuleProfileBle50SendToPhone(&sendBuff[0], 5 + uteModuleNewFactorySendAppData.sendAppData[param->currSendMinIndex - 1][SEND_APP_DATA_LENGTH_OFFSET]);
    param->currSendMinIndex++;
    if (param->currSendMinIndex > param->currSendMaxIndex) // 通过序号结束
    {
        sendBuff[3] = 0xfd;
        uteModuleProfileBleSendToPhone(&sendBuff[0], 4);
        uteApplicationCommonSyncDataTimerStop();
    }
}
/**
 *@brief        准备开始发送hardfault 数据
 *@details
 *@author       zn.zeng
 *@date       2022-03-16
 */
void uteModuleNewFactoryStartSendlogData(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    uint8_t response[7];
    param->crc = 0;
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    param->currSendMinIndex = 1;
    param->currSendFileDataOffset = 0;

    response[0] = CMD_FACTORY_TEST_MODE;
    response[1] = 0xff;
    response[2] = 0xf2;
    uint32_t flashReadSize = 0;
    ute_ble_connect_state_t state;
    uteApplicationCommonGetBleConnectionState(&state);
    if (!state.isConnected)
    {
        goto SEND_NEW_FACTORY_FINISH;
    }
    for (uint8_t i = 0; i < FACTORY_AGING_TIME_MAX; i++)
    {
        flashReadSize += uteModuleNewFactorySendAppData.sendAppData[i][SEND_APP_DATA_LENGTH_OFFSET];
    }
    if (flashReadSize != 0)
    {
        response[3] = flashReadSize >> 24 & 0xff;
        response[4] = flashReadSize >> 16 & 0xff;
        response[5] = flashReadSize >> 8 & 0xff;
        response[6] = flashReadSize >> 0 & 0xff;
        uteModuleProfileBleSendToPhone(&response[0], 7);
        uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleNewFactorySendAppDataFunc);
        uteApplicationCommonSyncDataTimerStart();
    }
    else
    {
    SEND_NEW_FACTORY_FINISH:
        response[3] = 0xfd;
        uteModuleProfileBleSendToPhone(&response[0], 4);
    }
    param->currSendFileSize = flashReadSize;
    param->currSendMaxIndex = FACTORY_MODULE_MAX;

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,", __func__);
}
#if UTE_MODULE_NEW_FACTORY_TEST_RING_MOTOR_SCREEN_SUPPORT
/**
 *@brief  设置马达状态
 *@details
 *@return bool isMotorAwaysVibration
 *@author        dengli.lu
 *@date        2022-04-24
 */
void uteModuleNewFactorySetMode3MotorStatus(bool isMode3MotorAwaysVibration)
{
    uteModuleNewFactoryTestData.isMode3MotorAwaysVibration = isMode3MotorAwaysVibration;
}
/**
 *@brief  获取马达状态
 *@details
 *@return bool isMotorAwaysVibration
 *@author        dengli.lu
 *@date        2022-04-24
 */
bool uteModuleNewFactoryGetMode3MotorStatus(void)
{
    return uteModuleNewFactoryTestData.isMode3MotorAwaysVibration;
}
/**
 *@brief  设置屏幕状态
 *@details
 *@return bool isMotorAwaysVibration
 *@author        dengli.lu
 *@date        2022-04-24
 */
void uteModuleNewFactorySetMode3ScreenStatus(bool isMode3ScreenAwaysBright)
{
    uteModuleNewFactoryTestData.isMode3ScreenAwaysBright = isMode3ScreenAwaysBright;
}
/**
 *@brief  获取屏幕状态
 *@details
 *@return bool isMotorAwaysVibration
 *@author        dengli.lu
 *@date        2022-04-24
 */
bool uteModuleNewFactoryGetMode3ScreenStatus(void)
{
    return uteModuleNewFactoryTestData.isMode3ScreenAwaysBright;
}
#endif // UTE_MODULE_NEW_FACTORY_TEST_RING_MOTOR_SCREEN_SUPPORT
/**
 *@brief  设置心率检测模式
 *@details
 *@author        dengli.lu
 *@date        2022-09-02
 */
void uteModuleNewFactoryTestSetCheckLightMode(uint8_t mode)
{
    uteModuleNewFactoryTestData.u.vcxx.testMode = mode;
    if (mode != NEW_FACTORY_VCXX_TEST_MODE_RED_LIGHT)
    {
        // uteDrvHeartVcxxStartCrosstalktest();
    }
#if !UTE_DRV_HEART_VC9202_VP60A2_SUPPORT
    if (mode == NEW_FACTORY_VCXX_TEST_MODE_BIO_A)
    {
        uteModuleNewFactoryTestData.u.vcxx.testMode = NEW_FACTORY_VCXX_TEST_MODE_CROSSTALK;
    }
#endif
}
/**
 *@brief  获取心率检测模式
 *@details
 *@author        dengli.lu
 *@date        2022-09-02
 */
uint8_t uteModuleNewFactoryTestGetCheckLightMode(void)
{
    return uteModuleNewFactoryTestData.u.vcxx.testMode;
}
/**
 *@brief        vcxx系列获取漏光bio值
 *@details
 *@author       dengli.li
 *@date       2022-09-02
 */
void uteModuleNewFactoryTestGetCheckLightBioValue(uint16_t *bioA, uint16_t *bioB, uint16_t *bioDiff)
{
    *bioA = uteModuleNewFactoryTestData.u.vcxx.bioA;
    *bioB = uteModuleNewFactoryTestData.u.vcxx.bioB;
    *bioDiff = uteModuleNewFactoryTestData.u.vcxx.bioDiff;
}

/**
 *@brief        设置按键Key值
 *@details
 *@author       xjc
 *@date         2022-10-12
 */
void uteModuleNewFactoryTestSetKeyValue(ute_module_factory_test_key_t key)
{
    memcpy(&uteModuleNewFactoryTestData.key, &key, sizeof(ute_module_factory_test_key_t));
}
/**
 *@brief        重置按键Key值
 *@details
 *@author       xjc
 *@date         2022-10-12
 */
void uteModuleNewFactoryTestResSetKeyValue(void)
{
    memset(&uteModuleNewFactoryTestData.key, 0, sizeof(ute_module_factory_test_key_t));
}
/**
 *@brief        读取按键Key值
 *@details
 *@author       xjc
 *@date         2022-10-12
 */
ute_module_factory_test_key_t uteModuleNewFactoryTestGetKeyValue(void)
{
    return uteModuleNewFactoryTestData.key;
}

#if UTE_MODULE_SHIP_MODE_SUPPORT
/**
 *@brief        是否要进入船运模式
 *@details
 *@author       xjc
 *@date         2022-11-02
 */
void uteModuleNewFactoryTestShipModeEnter(void)
{
    if (uteModuleNewFactoryTestData.mode == FACTORY_TEST_MODE_SHIP)
    {
        uteModuleShipModeEnter();
        uteModuleNewFactoryTestResetParam();
    }
}
#endif

#if UTE_MODULE_NEW_FACTORY_AGING_REPORT_SUPPORT
/**
 *@brief     读取老化测试报告
 *@details
 *@author        xjc
 *@date        2023-05-27
 */
void uteModuleNewFactoryTestReadAgingReportData(void)
{
    void *file = NULL;
    uint8_t path[30];
    memset(&path[0], 0, 30);
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_NEW_FACTORY_AGING_REPORT_DATA, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &uteModuleNewFactoryTestData.agingRecord[0], sizeof(uteModuleNewFactoryTestData.agingRecord));
        uteModuleFilesystemCloseFile(file);
    }
    for (uint8_t i = 0; i < FACTORY_AGING_TEST_MODE_MAX; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.agingRecord[%d].testTime=%d,isPass=%d", __func__, i, uteModuleNewFactoryTestData.agingRecord[i].testTime, uteModuleNewFactoryTestData.agingRecord[i].isPass);
    }
}

/**
 *@brief     保存老化测试报告
 *@details
 *@author        xjc
 *@date        2023-05-27
 */
void uteModuleNewFactoryTestSaveAgingReportData(FACTORY_AGING_TEST_MODE mode, uint8_t totalHour, bool isPass)
{
    void *file;
#if PROJECT_RH303AU_SUPPORT
    uteModuleNewFactoryTestReadAgingReportData();
#endif
    uteModuleNewFactoryTestData.agingRecord[mode].totalHour = totalHour;
    uteModuleNewFactoryTestData.agingRecord[mode].isPass = isPass;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_NEW_FACTORY_AGING_REPORT_DATA, &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &uteModuleNewFactoryTestData.agingRecord[0], sizeof(uteModuleNewFactoryTestData.agingRecord));
        uteModuleFilesystemCloseFile(file);
    }
}

/**
 *@brief     获取老化测试报告
 *@details
 *@author        xjc
 *@date        2023-05-27
 */
void uteModuleNewFactoryTestGetAgingReportData(ute_module_factory_aging_record_t *agingRecord)
{
    memcpy(agingRecord, &uteModuleNewFactoryTestData.agingRecord[0], sizeof(uteModuleNewFactoryTestData.agingRecord));
}

/**
 *@brief     是否进入老化报告界面
 *@details
 *@author        xjc
 *@date        2023-05-27
 */
void uteModuleNewFactoryTestSetEnterAgingReportPage(bool isEnterAgingReport)
{
    uteModuleNewFactoryTestData.isEnterAgingReportPage = isEnterAgingReport;
}
/**
 *@brief     读取是否进入老化报告界面
 *@details
 *@author        xjc
 *@date        2023-05-27
 */
bool uteModuleNewFactoryTestGetEnterAgingReportPage(void)
{
    return uteModuleNewFactoryTestData.isEnterAgingReportPage;
}
#endif

#endif
