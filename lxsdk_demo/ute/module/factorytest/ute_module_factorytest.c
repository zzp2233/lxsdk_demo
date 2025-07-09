/**
*@file
*@brief        工厂测试模块
*@details
*@author       zn.zeng
*@date       2021-11-23
*@version      v1.0
*/
#include "include.h"
#include "ute_module_factorytest.h"
#include "ute_module_log.h"
#include "ute_module_systemtime.h"
#include "ute_drv_gsensor_common.h"
#include "ute_drv_motor.h"
#include "ute_module_heart.h"
#include "ute_drv_battery_common.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_module_gui_common.h"
#include "ute_module_sport.h"
#include "ute_module_temperature.h"
#include "ute_application_common.h"
#include "ute_module_call.h"
#include "ute_drv_temperature_common.h"
#include "ute_module_micrecord.h"

ute_module_factory_test_data_t uteModuleFactoryTestData;

/**
*@brief  工厂测试每秒处理函数
*@details
*@author        zn.zeng
*@date        2021-11-23
*/
void uteModuleFactoryTestEverySecond(void)
{
    if (uteModuleFactoryTestData.currItem == TEST_ITEM_NONE)
    {
        return;
    }
    if (uteModuleFactoryTestData.currItem == TEST_ITEM_VCXX_CHECK_LIGHT)
    {
        if (uteModuleFactoryTestData.u.vcxx.testMode == FACTORY_VCXX_TEST_MODE_BIO_B)
        {
            // uteDrvHeartVcxxGetCrosstalktestValue(&uteModuleFactoryTestData.u.vcxx.pre, &uteModuleFactoryTestData.u.vcxx.current, &uteModuleFactoryTestData.u.vcxx.ps, &uteModuleFactoryTestData.u.vcxx.bioB, &uteModuleFactoryTestData.u.vcxx.isPass);
            // uteModuleFactoryTestData.u.vcxx.bioDiff = abs(uteModuleFactoryTestData.u.vcxx.bioB - uteModuleFactoryTestData.u.vcxx.bioA);
            // if (uteModuleFactoryTestData.u.vcxx.bioDiff >= DRV_HEART_VCXX_BIO_PASS_VALUE)
            // {
            //     uteModuleFactoryTestData.u.vcxx.isPass = true;
            // }
            // else
            // {
            //     uteModuleFactoryTestData.u.vcxx.isPass = false;
            // }
        }
        else
        {
            if (uteModuleFactoryTestData.u.vcxx.testMode == FACTORY_VCXX_TEST_MODE_INFRARED)
            {
                if ((uteModuleFactoryTestData.u.vcxx.ps >= DRV_HEART_VCXX_PS_MIN_VALUE) && (uteModuleFactoryTestData.u.vcxx.current >= DRV_HEART_VCXX_LIGHT_I_MIN_VALUE))
                {
                    uteModuleFactoryTestData.u.vcxx.isPass = true;
                }
                else
                {
                    uteModuleFactoryTestData.u.vcxx.isPass = false;
                }
            }
            else
            {
                if ((uteModuleFactoryTestData.u.vcxx.pre <= DRV_HEART_VCXX_LIGHT_LEAKGE_DIFF_VALUE) && (uteModuleFactoryTestData.u.vcxx.current >= DRV_HEART_VCXX_LIGHT_I_MIN_VALUE))
                {
                    uteModuleFactoryTestData.u.vcxx.isPass = true;
                }
                else
                {
                    uteModuleFactoryTestData.u.vcxx.isPass = false;
                }
            }
        }
    }

    if (uteModuleFactoryTestData.currItem == TEST_ITEM_AGING)
    {
#if UTE_MODULE_HEART_SUPPORT
        if(!uteModuleHeartIsSingleTesting())
        {

            uteModuleHeartStartSingleTesting(TYPE_HEART);

        }

        if (uteModuleHeartIsWear())
        {
            uteModuleFactoryTestData.u.aging.heart = (uint8_t)uteModuleHeartGetHeartValue();
        }
        else
        {
            uteModuleFactoryTestData.u.aging.heart = 0;
        }
        int16_t x, y, z;
        uteDrvGsensorCommonGetAccXyz(&x, &y, &z);
        ute_drv_gsensor_common_axis_bit_change_t bitChange;
        bitChange.inputXaxis = &x;
        bitChange.inputYaxis = &y;
        bitChange.inputZaxis = &z;
        bitChange.outputXaxis = &uteModuleFactoryTestData.u.aging.gsensorX;
        bitChange.outputYaxis = &uteModuleFactoryTestData.u.aging.gsensorY;
        bitChange.outputZaxis = &uteModuleFactoryTestData.u.aging.gsensorZ;
        uteDrvGsensorCommonXYZaxisDataBitChange(&bitChange, 1, GSENSOR_DATA_BIT_STEP);
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
        uteModuleFactoryTestData.u.aging.runningTimeSecond++;
#endif
    }
    else if (uteModuleFactoryTestData.currItem == TEST_ITEM_BATTERY)
    {
        uteModuleFactoryTestData.u.battery.currBatteryVoltage = uteDrvBatteryCommonGetVoltage();
        uteModuleFactoryTestData.u.battery.currBatteryLvl = uteDrvBatteryCommonGetLvl();
        uteModuleFactoryTestData.u.battery.runningTimeSecond++;
    }
    else if (uteModuleFactoryTestData.currItem == TEST_ITEM_SCREEN_RGB)
    {
        uteModuleFactoryTestData.u.screen.runningTimeSecond++;
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
    }
    else if (uteModuleFactoryTestData.currItem == TEST_ITEM_MIC_SPEAKER)
    {
        uteModuleFactoryTestData.u.micSpeaker.runningTimeSecond++;
    }
}

/**
*@brief  工厂测试初始化
*@details
*@author        zn.zeng
*@date        2021-11-23
*/
void uteModuleFactoryTestInit(void)
{
    memset(&uteModuleFactoryTestData,0,sizeof(ute_module_factory_test_data_t));
    uteModuleSystemtimeRegisterSecond(uteModuleFactoryTestEverySecond);
}

/**
*@brief  填充支持测试项0
*@details
*@author        zn.zeng
*@date        2021-11-23
*/
void uteModuleFactoryTestFillSupportFuntion0(uint8_t *funtion)
{
    funtion[0] = 0;
#if 1//UTE_DRV_HEART_VCXX_SUPPORT
    funtion[0] |= 0x01;
#endif
    funtion[0] |= 0x04;
    funtion[0] |= 0x08;
    // funtion[0] |= 0x10; //mac地址修改
#if UTE_FACTORY_TEST_AUDIO_RF_MODE_SUPPORT
    funtion[0] |= 0x20;
#endif
}

/**
*@brief  填充支持测试项1
*@details
*@author        zn.zeng
*@date        2021-11-23
*/
void uteModuleFactoryTestFillSupportFuntion1(uint8_t *funtion)
{
    funtion[0] = 0;
#if UTE_DRV_PWRKEY_SUPPORT || UTE_DRV_ADCKEY_SUPPORT || UTE_DRV_IOKEY_SUPPORT
    funtion[0] |= 0x01;
#endif
    funtion[0] |= 0x02;
    funtion[0] |= 0x04;
    funtion[0] |= 0x08;
#if DRV_GYROSCOPE_SUPPORT
    funtion[0] |= 0x10;
#endif
#if DRV_MAGNETIC_SUPPORT
    funtion[0] |= 0x20;
#endif
#if UTE_ALI_SE_IC_SUPPORT
    funtion[0] |= 0x40;
#endif
#if UTE_GNSS_SUPPORT
    funtion[0] |= 0x80;
#endif
#if DRV_NXP_NFC_SUPPORT
    funtion[1] |= 0x01;
#endif
#if DRV_PSRAM_SUPPORT
    funtion[1] |= 0x02;
#endif
#if UTE_BT30_CALL_SUPPORT
    funtion[1] |= 0x04;
#endif
//马达
    funtion[1] |= 0x08;
//gsensor
    funtion[1] |= 0x10;
#if UTE_MODULE_TEMPERATURE_SUPPORT
    funtion[1] |= 0x20;
#endif
//charge status & adc
    funtion[1] |= 0x40;
//PCBA TP
    funtion[1] |= 0x80;
    uteModuleFactoryTestData.isKeysTesting = true;
}

/**
*@brief  开始测试项
*@details
*@param[in] uint8_t item,测试项
*@param[in] bool isOpen，打开还是关闭
*@author        zn.zeng
*@date        2021-11-23
*/
void uteModuleFactoryTestStartTestItem(ute_module_factory_test_item_t item,bool isOpen)
{
    uteModuleFactoryTestData.currItem = item;
    if(item==TEST_ITEM_VCXX_CHECK_LIGHT)
    {
        uteModuleFactoryTestData.u.vcxx.testMode = FACTORY_VCXX_TEST_MODE_CROSSTALK;
        if(isOpen)
        {
#if UTE_MODULE_HEART_SUPPORT
            uteModuleHeartStartSingleTesting(TYPE_FACTORY0);
#endif
//            uteDrvHeartVcxxStartCrosstalktest();
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_FACTORY_TEST_ID, 0, __func__);
        }
        else
        {
            // uteModuleHeartStopSingleTesting(TYPE_HEART);
            uteModuleFactoryTestDisconnectHandler();
        }
    }
    else if(item==TEST_ITEM_AGING)
    {
        uteModuleFactoryTestData.u.aging.runningTimeSecond = 0;
        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_FACTORY_TEST_ID, 0, __func__);
#if UTE_MODULE_HEART_SUPPORT
        uteModuleHeartStartSingleTesting(TYPE_HEART);
#endif
    }
    else if(item==TEST_ITEM_BATTERY)
    {
        uteModuleFactoryTestData.u.battery.startBatteryVoltage = uteDrvBatteryCommonGetVoltage();
        uteModuleFactoryTestData.u.battery.startBatteryLvl = uteDrvBatteryCommonGetLvl();
        uteModuleFactoryTestData.u.battery.runningTimeSecond = 0;
        uteModuleSystemtimeGetTime(&uteModuleFactoryTestData.u.battery.startTime);
        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_FACTORY_TEST_ID, 0, __func__);
    }
    else if(item==TEST_ITEM_SCREEN_RGB)
    {
        if(isOpen)
        {
            uteModuleFactoryTestData.u.screen.runningTimeSecond = 0;
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_FACTORY_TEST_ID, 0, __func__);
        }
        else
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
            uteModuleFactoryTestDisconnectHandler();
        }
    }
    else if(item==TEST_ITEM_TOUCH)
    {
        memset(uteModuleFactoryTestData.u.touch.graph, 0, sizeof(uteModuleFactoryTestData.u.touch.graph));
        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_FACTORY_TEST_ID, 0, __func__);
    }
    else if(item==TEST_ITEM_MIC_SPEAKER)
    {
        uteModuleFactoryTestData.u.micSpeaker.runningTimeSecond = 0;
        if(isOpen)
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_FACTORY_TEST_ID, 0, __func__);
            uteModuleMicRecordFactoryEnter();
            // uteModulePlatformSendMsgToAppTask(TO_APP_TASK_FACTORY_TEST_MIC_SPEAKER_START,0);
        }
        else
        {
            // uteModulePlatformSendMsgToAppTask(TO_APP_TASK_FACTORY_TEST_MIC_SPEAKER_STOP,0);
            uteModuleMicRecordFactoryExit();
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
        }
    }
    else if(item==TEST_ITEM_SCREEN_HOLD_BRIGHT)
    {
        if(isOpen)
        {
            uteModuleFactoryTestData.u.screen.runningTimeSecond = 0;
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_FACTORY_TEST_ID, 0, __func__);
        }
        else
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
        }
    }
    else if(item==TEST_ITEM_MOTOR)
    {
        uteModuleGuiCommonDisplayOff(false);
        if(isOpen)
        {
            uteDrvMotorEnable();
        }
        else
        {
            uteDrvMotorDisable();
        }
    }
    else if(item==TEST_ITEM_PCBA_TP)
    {
        if(isOpen)
        {
            uteModuleFactoryTestData.u.screen.runningTimeSecond = 0;
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_FACTORY_TEST_ID, 0, __func__);
        }
        else
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
        }
    }
}

/**
*@brief  获取测试数据
*@details
*@author        zn.zeng
*@date        2021-11-23
*/
void uteModuleFactoryTestGetData(ute_module_factory_test_data_t *testData)
{
    memcpy(testData,&uteModuleFactoryTestData,sizeof(ute_module_factory_test_data_t));
}

/**
*@brief  设置测试数据
*@details
*@author        zn.zeng
*@date        2022-02-16
*/
void uteModuleFactoryTestSetData(ute_module_factory_test_data_t testData)
{
    memcpy(&uteModuleFactoryTestData,&testData,sizeof(ute_module_factory_test_data_t));
}

/**
*@brief  获取当前测试项目
*@details
*@author        zn.zeng
*@date        2022-02-16
*/
uint8_t uteModuleFactoryTestGetCurrTestItem(void)
{
    return uteModuleFactoryTestData.currItem;
}

/**
*@brief  协议转到当前文件处理
*@details
*@author        casen
*@date        2022-02-23
*/
void uteModuleFactoryTestProtocol(uint8_t*receive,uint8_t length)
{
    uint16_t option =0;
    uint8_t response[20];
    memset(response,0x00,20);
    option =(receive[1]<<8&0xff00) | receive[2];
    memcpy(&response[0],&receive[0],length);
    if(option==0xfff0)
    {
        uteModuleFactoryTestFillSupportFuntion0(&response[3]);
        uteModuleProfileBleSendToPhone(&response[0],20);
    }
    else if(option==0xfff1)
    {
        uteModuleFactoryTestFillSupportFuntion1(&response[3]);
        uteModuleProfileBleSendToPhone(&response[0],20);
    }
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
    else if(option==0xfff2)
    {
        extern void uteModuleNewFactoryStartSendlogData();
        uteModuleNewFactoryStartSendlogData();
    }
#endif
    else if(option==0x0001)
    {
#if DRV_HEART_SENSOR_TEMPERATURE_CHECK_VCXXHCI_SUPPORT
        vcxxPowerOnResumeInitialCalibrationFrequencyAndExceptionFlag(true);
        uteAppInitializeVCxxSensorTemperature();
#endif
        uteModuleFactoryTestStartTestItem(TEST_ITEM_VCXX_CHECK_LIGHT,true);
        // response[3] = uteDrvHeartVcxxIsOscCheckFinish();
        uteModuleProfileBleSendToPhone(&response[0],4);
    }
    else if(option==0x0002)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_VCXX_CHECK_LIGHT,false);
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x0003)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_SCREEN_HOLD_BRIGHT,true);
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x0004)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_SCREEN_HOLD_BRIGHT,false);
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x0005)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_AGING,true);
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x0006)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_BATTERY,true);
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x0007)
    {
        response[3] = UTE_DRV_PWRKEY_MAX_CNT;
        uteModuleFactoryTestData.isKeysTesting = true;
        uteModuleProfileBleSendToPhone(&response[0],4);
    }
    else if(option==0x0008)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_SCREEN_RGB,true);
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x0009)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_SCREEN_RGB,false);
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x000a)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_TOUCH,true);
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x000b)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_GYROSCOPE,true);
    }
    else if(option==0x000C)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_MAGNETIC,true);
    }
    else if(option==0x000D)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_ALI_SE_IC,true);
    }
    else if(option==0x000E)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_GNSS,true);
    }
    else if(option==0x000F)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_GNSS_FIND_SAT,true);
    }
    else if(option==0x0010)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_NXP_NFC,true);
    }
    else if(option==0x0011)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_PSRAM,true);
    }
    else if(option==0x0012)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_MIC_SPEAKER,receive[3]);
        uteModuleProfileBleSendToPhone(&response[0],4);
    }
    else if(option==0x0013)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_MOTOR,receive[3]);
        uteModuleProfileBleSendToPhone(&response[0],4);
    }
    else if(option==0x0014)
    {
#if UTE_MODULE_SLEEP
        int16_t xx,yy,zz,accvalue;
        uteDrvGsensorCommonGetAccXyz(&xx,&yy,&zz);
        ute_drv_gsensor_common_axis_bit_change_t bitChange;
        bitChange.inputXaxis = &xx;
        bitChange.inputYaxis = &yy;
        bitChange.inputZaxis = &zz;
        bitChange.outputXaxis = &xx;
        bitChange.outputYaxis = &yy;
        bitChange.outputZaxis = &zz;
        uteDrvGsensorCommonXYZaxisDataBitChange(&bitChange,1,GSENSOR_DATA_BIT_STEP);
        accvalue = SqrtByBisection(xx*xx+yy*yy+zz*zz);
        response[3] = uteDrvGsensorCommonGetAccRange();
        response[4] = (xx>>8)&0xff;
        response[5] = xx&0xff;
        response[6] = (yy>>8)&0xff;
        response[7] = yy&0xff;
        response[8] = (zz>>8)&0xff;
        response[9] = zz&0xff;
        response[10] = (accvalue>>8)&0xff;
        response[11] = accvalue&0xff;
        uteModuleProfileBleSendToPhone(&response[0],12);
#endif
    }
    else if(option==0x0015)
    {
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,receive = %d", __func__,receive[3]);
#if UTE_MODULE_TEMPERATURE_SUPPORT
        if(receive[3] == 0x01)
        {
            uteModuleTemperatureStartTesting(TEMPERATURE_MODE_APP_GET_AMBIENT);
        }
        else
        {
            uteModuleTemperatureStopTesting();
            uteModuleProfileBleSendToPhone(&response[0],4);
        }
#endif
    }
    else if(option==0x0016)
    {
        uint16_t currBatteryVoltage;
        uint8_t currBatteryLvl;
        ute_drv_battery_charger_status_t  status;
        currBatteryVoltage = uteDrvBatteryCommonGetVoltage();
        currBatteryLvl = uteDrvBatteryCommonGetLvl();
        status = uteDrvBatteryCommonGetChargerStatus();
        response[3] = currBatteryVoltage>>0x08;
        response[4] = currBatteryVoltage&0xFF;
        response[5] = currBatteryLvl;
        response[6] = status;
        uteModuleProfileBleSendToPhone(&response[0],7);
    }
    else if(option==0x0017)
    {
        uteModuleFactoryTestStartTestItem(TEST_ITEM_PCBA_TP,receive[3]);
        uteModuleProfileBleSendToPhone(&response[0],4);
    }
    else if(option==0x0018)
    {
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,receive = %d", __func__,receive[3]);
#if UTE_MODULE_TEMPERATURE_SUPPORT
        uteModuleTemperatureStopTesting();
#endif
#if UTE_MODULE_HEART_SUPPORT
        uteModuleHeartStopSingleTesting(TYPE_HEART);
#endif
        // uteDrvHeartVcxxStopSample();
        // uteDrvHeartVcxxStartSample();

        // uteModuleGuiCommonDisplayOff(false);
        sys_cb.guioff_delay = 0;
        gui_sleep();

        if(receive[3] == 0x01)
        {
            uteModuleProfileBleSendToPhone(&receive[0],4);
        }
        else if(receive[3] == 0x02)
        {
            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_START_POWER_OFF,0);
        }
    }
    else if (option==0x0019)
    {
        uint8_t macAddr[6] = {0};
        bool isSuccess = false;
        memcpy(response,receive,3);
        if(length > 8)
        {
            memcpy(macAddr,&receive[3],6);
            isSuccess = uteModulePlatformSetBleMacAddress(macAddr);
        }
        response[3] = isSuccess?0x01:0x00;
        uteModuleProfileBleSendToPhone(&response[0],4);
        if(isSuccess)
        {
            delay_5ms(200);
            uteModulePlatformSystemReboot();
        }
    }
#if UTE_FACTORY_TEST_AUDIO_RF_MODE_SUPPORT
    else if(option==0x001A)
    {
        ute_new_factory_test_data_t *data;
        uteModuleNewFactoryTestSetMode(&data);
        data->mode = FACTORY_TEST_MODE_AUDIO;
        memset(&data->moduleResult, 0, sizeof(data->moduleResult));
        if(!uteModuleCallBtGetPowerStatus())
        {
            uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
        }
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
        if(!uteModuleCallIsEntertranmentVoiceOn())
        {
            uteModuleCallChangeEntertranmentVoiceSwitchStatus();
        }
#endif
        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_NEW_FACTORY_MODE_SELECT_ID, 0, __func__);
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x001B)
    {
        ute_new_factory_test_data_t *data;
        uteModuleNewFactoryTestSetMode(&data);
        if(data->mode == FACTORY_TEST_MODE_AUDIO)
        {
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
            if((UTE_MODULE_BT_ENTERTRANMENT_VOICE_DEFAULT == false) && (uteModuleCallIsEntertranmentVoiceOn()))
            {
                uteModuleCallChangeEntertranmentVoiceSwitchStatus();
            }
#endif
            if(uteModuleCallBtGetPowerStatus())
            {
                uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_BUTTON);
            }
        }
        data->mode = FACTORY_TEST_MODE_NULL;
        uteModuleProfileBleSendToPhone(&response[0],3);
    }
    else if(option==0x001C)
    {
        uteModuleProfileBleSendToPhone(&response[0],3);
        uteModulePlatformDelayUs(200000);
        uteModulePlatformSwitchRfDutTest();
    }
#endif
}

/**
*@brief  按键测试处理
*@details
*@author        zn.zeng
*@date        2021-11-24
*/
void uteModuleFactoryTestKeysHandler(uint16_t keycode)
{
    if (uteModuleFactoryTestData.isKeysTesting)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,0x%x",__func__,keycode);
        uint8_t sendKeyValue[5] = {CMD_FACTORY_TEST_MODE, 0x00, 0x07, 0x00, 0x00};
#if UTE_DRV_PWRKEY_MAX_CNT
        sendKeyValue[3] = UTE_DRV_PWRKEY_MAX_CNT;
#endif
        if (keycode == UTE_DRV_PWRKEY_KEY0)
        {
            sendKeyValue[4] = 0;
            uteModuleProfileBleSendToPhone(&sendKeyValue[0], 5);
        }
        else if (keycode == UTE_DRV_PWRKEY_KEY1)
        {
            sendKeyValue[4] = 1;
            uteModuleProfileBleSendToPhone(&sendKeyValue[0], 5);
        }
        else if (keycode == UTE_DRV_PWRKEY_KEY2)
        {
            sendKeyValue[4] = 2;
            uteModuleProfileBleSendToPhone(&sendKeyValue[0], 5);
        }
        else if (keycode == UTE_DRV_PWRKEY_KEY3)
        {
            sendKeyValue[4] = 3;
            uteModuleProfileBleSendToPhone(&sendKeyValue[0], 5);
        }
        else if (keycode == UTE_DRV_PWRKEY_KEY4)
        {
            sendKeyValue[4] = 4;
            uteModuleProfileBleSendToPhone(&sendKeyValue[0], 5);
        }
    }
}

/**
*@brief  工厂测试断开连接处理
*@details
*@author        zn.zeng
*@date        2021-11-24
*/
void uteModuleFactoryTestDisconnectHandler(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,currItem = %d", __func__,uteModuleFactoryTestData.currItem);
    if(uteModuleFactoryTestData.currItem!=TEST_ITEM_NONE&&(uteModuleFactoryTestData.currItem!=TEST_ITEM_AGING)&&(uteModuleFactoryTestData.currItem!=TEST_ITEM_BATTERY))
    {
        uteModuleFactoryTestStop();
    }
    uteModuleFactoryTestData.isKeysTesting = false;
}

/**
*@brief  工厂测试退出所有模式
*@details
*@author        zn.zeng
*@date        2021-11-24
*/
void uteModuleFactoryTestStop(void)
{
    if(uteModuleFactoryTestData.currItem==TEST_ITEM_MIC_SPEAKER)
    {
        uteModuleMicRecordFactoryExit();
    }
    uteModuleFactoryTestData.currItem = TEST_ITEM_NONE;
    uteModuleFactoryTestData.isKeysTesting = false;
    if(uteApplicationCommonIsPowerOn())
    {
#if UTE_MODULE_HEART_SUPPORT
        uteModuleHeartStopSingleTesting(TYPE_FACTORY0);

        if(task_stack_contains(FUNC_TEST_MODE) == false)
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
        }
#endif
    }
}

void uteModuleFactoryTestSetCheckLightMode(uint8_t mode)
{
    uteModuleFactoryTestData.u.vcxx.testMode = mode;
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartStartSingleTesting(TYPE_FACTORY0);
#endif
#if (!UTE_DRV_HEART_VC9202_VP60A2_SUPPORT)&&(UTE_DRV_HEART_VC30S_SUPPORT&&CFG_FULL_FUNCTION)
    if(mode == FACTORY_VCXX_TEST_MODE_BIO_A)
    {
        uteModuleFactoryTestData.u.vcxx.testMode = FACTORY_VCXX_TEST_MODE_CROSSTALK;
    }
#endif
}

uint8_t uteModuleFactoryTestGetCheckLightMode(void)
{
    return uteModuleFactoryTestData.u.vcxx.testMode;
}

/**
*@brief  获取测试维客数据
*@details
*@author     casen
*@date       2022-02-25
*/
void uteModuleFactoryTestGetVkData(int *preV,int *curV, int *psV,bool *isPass)
{
    (*preV) = uteModuleFactoryTestData.u.vcxx.pre;
    (*curV) = uteModuleFactoryTestData.u.vcxx.current;
    (*psV) = uteModuleFactoryTestData.u.vcxx.ps;
    (*isPass) = uteModuleFactoryTestData.u.vcxx.isPass;
}

void uteModuleFactoryTestSetVkData(int preV,int curV, int psV)
{
    uteModuleFactoryTestData.u.vcxx.pre = preV;
    uteModuleFactoryTestData.u.vcxx.current = curV;
    uteModuleFactoryTestData.u.vcxx.ps = psV;
}

