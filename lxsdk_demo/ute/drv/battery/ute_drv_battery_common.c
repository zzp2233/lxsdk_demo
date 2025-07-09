/**
*@file
*@brief     电池驱动文件
*@details    电池驱动公共层文件
*@author        zn.zeng
*@date        Jul 05, 2021
*@version       v1.0
*/
#include "ute_drv_battery_common.h"
#include "ute_module_log.h"
// #include "ute_module_screens_common.h"
#include "ute_drv_motor.h"
#include "ute_module_systemtime.h"
#include "ute_application_common.h"
#include "ute_module_message.h"
// #include "ute_module_temperature.h"
// #include "ute_drv_temperature_common.h"
#include "include.h"
#include "msgbox.h"
#include "ute_module_sport.h"
#include "ute_module_music.h"
#include "ute_module_protocol.h"
#include "ute_module_call.h"
// #include "ute_module_newFactoryTest.h"
#include "func_cover.h"

#if UTE_DRV_BATTERY_USE_UTE_PERCENTAGE_SUPPORT

ute_drv_battery_common_data_t uteDrvBatteryCommonData;

#if UTE_MODULE_BATTERY_RECORD_BAT_LVL_SUPPORT

/**
*@brief         读取电池config
*@details
*@author       xjc
*@date       2022-01-25
*/
void uteDrvBatteryCommonReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[2];
    /*! 读取记录的电池电量 xjc, 2022-01-25*/
    readbuff[0]=uteDrvBatteryCommonData.lastLvl;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_RECORD_BAT_PERCENT,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    if(readbuff[0]==0)
    {
        /****recordLvl必须是大于0，否则没有意义,lastLvl在之前的地方已经读取一次，所以不为0****/
        if(uteDrvBatteryCommonData.lastLvl >0)
        {
            uteDrvBatteryCommonData.recordLvl = uteDrvBatteryCommonData.lastLvl;
        }
    }
    else
    {
        uteDrvBatteryCommonData.recordLvl = readbuff[0];
    }
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,recordLvl=%d", __func__,uteDrvBatteryCommonData.recordLvl);
}

/**
*@brief        比较电池电量
*@details
*@author       xjc
*@date       2022-01-25
*/
void uteDrvBatteryCommonCompareBatLvl(void)
{
    uteDrvBatteryCommonReadConfig();
    /****recordLvl必须是大于0，否则没有意义****/
    if (uteDrvBatteryCommonData.recordLvl == 0)
    {
        uteDrvBatteryCommonData.recordLvl = uteDrvBatteryCommonData.lastLvl;
    }
    else
    {
        /*! 读取到的电量比记录的电量大,并且相差不超过20%，则认为是电量读取不准确,使用记录的值 xjc, 2022-01-25*/
        uint8_t diff = 0;
        if (uteDrvBatteryCommonData.lastLvl >= uteDrvBatteryCommonData.recordLvl)
        {
            diff = uteDrvBatteryCommonData.lastLvl - uteDrvBatteryCommonData.recordLvl;
        }
        else
        {
            diff = uteDrvBatteryCommonData.recordLvl - uteDrvBatteryCommonData.lastLvl;
        }

        if ((diff > 0) && (diff <= 20))
        {
            uteDrvBatteryCommonData.lastLvl = uteDrvBatteryCommonData.recordLvl;
        }
    }
#if UTE_MODULE_BATTERY_SAVE_LAST_LVL_BEFORE_FACTORY_SUPPORT
    uteDrvBatteryCommonReadLastLvlFromSN1();
#endif
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,lastLvl=%d", __func__, uteDrvBatteryCommonData.lastLvl);
}

/**
*@brief        电池电量变化时，保存电池电量
*@details
*@author      xjc
*@date       2022-01-25
*/
void uteDrvBatteryCommonSaveBatteryLvl(void)
{
    if (uteApplicationCommonIsStartupFinish() && (uteDrvBatteryCommonData.recordLvl != uteDrvBatteryCommonData.lastLvl))
    {
        void *file;
        uint8_t writeBuff[2];
        memset(&writeBuff[0], 0, sizeof(writeBuff));
        /*! 保存到文件xjc, 2022-01-25*/
        if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_RECORD_BAT_PERCENT, &file, FS_O_RDONLY | FS_O_WRONLY | FS_O_CREAT))
        {
            writeBuff[0] = uteDrvBatteryCommonData.lastLvl;
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
            uteModuleFilesystemWriteData(file, &writeBuff[0], 1);
            uteModuleFilesystemCloseFile(file);
        }
        //电池电量变化时发送到app dengli.lu 2022/09/30
        uint8_t response[3]= {0};
        response[0] = CMD_READ_BATTERY_LVL;
        response[1] = uteDrvBatteryCommonData.lastLvl;
        response[2] = 0x01;//表示ble主动上发指令
        if(!uteApplicationCommonGetIsSynchronizingData())
        {
            uteModuleProfileBleSendToPhone(&response[0],3);
        }
        uteDrvBatteryCommonData.recordLvl = uteDrvBatteryCommonData.lastLvl;
    }
}
#endif

/**
*@brief 充电状态变化回调
*@details
*STATE_CHARGER_START = 0,
*STATE_CHARGER_PRE_CHARGE,1
*STATE_CHARGER_FAST_CHARGE,2
*STATE_CHARGER_FINISH,3
*STATE_CHARGER_ERROR,4
*STATE_CHARGER_END,5
*@author        zn.zeng
*@date        2021-11-08
*/
void uteDrvBatteryCommonChargerChangeCb(uint8_t state)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,state=%d", __func__,state);
#if CHARGE_EN
    if (state < 1)
    {
        uteDrvBatteryCommonData.chargerStatus = BAT_STATUS_NO_CHARGE;
    }
    else if(state < 2)
    {
        if(0)
        {}
#if UTE_MODULE_BATTERY_LINEAR_CONSUME_SUPPORT
        else if(uteDrvBatteryCommonData.lvl == 100)
        {
            uteDrvBatteryCommonData.chargerStatus = BAT_STATUS_CHARGED;
        }
#endif
        else
        {
            uteDrvBatteryCommonData.chargerStatus = BAT_STATUS_CHARGING;
        }
    }
    else
    {
        uteDrvBatteryCommonData.chargerStatus = BAT_STATUS_CHARGED;
    }
#endif

#if UTE_MODULE_BATTERY_CHARGED_DISPLAY_ON_SUPPORT
    if((uteDrvBatteryCommonData.chargerStatus == BAT_STATUS_CHARGED) && uteApplicationCommonIsPowerOn())
    {
        if (!uteDrvBatteryCommonData.alreadyDisplayOn)
        {
            uteDrvBatteryCommonData.alreadyDisplayOn = true;
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_CHARGER_ID);
        }
    }
#endif
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,uteDrvBatteryCommonData.chargerStatus=%d", __func__,uteDrvBatteryCommonData.chargerStatus);
}
/**
*@brief 更新电池数据
*@details
*@author        zn.zeng
*@date        2021-12-15
*/
uint8_t uteDrvBatteryCommonCalibrationBatLvl(uint16_t voltage,int16_t current,ute_drv_battery_charger_status_t status)
{
    uint8_t lvl = 0;
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,status=%d,uteApplicationCommonIsStartupFinish=%d", __func__,status,uteApplicationCommonIsStartupFinish());
    if((status==BAT_STATUS_NO_CHARGE)||(status==BAT_STATUS_CHARGED)||!uteApplicationCommonIsStartupFinish())
    {
        uint16_t tmpVoltage = voltage;
        if(tmpVoltage<UTE_DRV_BATTERY_000)
        {
            tmpVoltage = UTE_DRV_BATTERY_000;
        }
        else if(tmpVoltage>UTE_DRV_BATTERY_100)
        {
            tmpVoltage = UTE_DRV_BATTERY_100;
        }
        if(tmpVoltage<UTE_DRV_BATTERY_010)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_000)*10.0f/(UTE_DRV_BATTERY_010-UTE_DRV_BATTERY_000);
        }
        else if(tmpVoltage<UTE_DRV_BATTERY_020)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_010)*10.0f/(UTE_DRV_BATTERY_020-UTE_DRV_BATTERY_010)+10;
        }
        else if(tmpVoltage<UTE_DRV_BATTERY_030)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_020)*10.0f/(UTE_DRV_BATTERY_030-UTE_DRV_BATTERY_020)+20;
        }
        else if(tmpVoltage<UTE_DRV_BATTERY_040)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_030)*10.0f/(UTE_DRV_BATTERY_040-UTE_DRV_BATTERY_030)+30;
        }
        else if(tmpVoltage<UTE_DRV_BATTERY_050)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_040)*10.0f/(UTE_DRV_BATTERY_050-UTE_DRV_BATTERY_040)+40;
        }
        else if(tmpVoltage<UTE_DRV_BATTERY_060)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_050)*10.0f/(UTE_DRV_BATTERY_060-UTE_DRV_BATTERY_050)+50;
        }
        else if(tmpVoltage<UTE_DRV_BATTERY_070)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_060)*10.0f/(UTE_DRV_BATTERY_070-UTE_DRV_BATTERY_060)+60;
        }
        else if(tmpVoltage<UTE_DRV_BATTERY_080)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_070)*10.0f/(UTE_DRV_BATTERY_080-UTE_DRV_BATTERY_070)+70;
        }
        else if(tmpVoltage<UTE_DRV_BATTERY_090)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_080)*10.0f/(UTE_DRV_BATTERY_090-UTE_DRV_BATTERY_080)+80;
        }
        else if(tmpVoltage<UTE_DRV_BATTERY_100)
        {
            lvl = (tmpVoltage-UTE_DRV_BATTERY_090)*10.0f/(UTE_DRV_BATTERY_100-UTE_DRV_BATTERY_090)+90;
        }
        else
        {
            lvl = 100;
        }
        UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,lvl=%d,tmpVoltage=%d", __func__,lvl,tmpVoltage);
        uteDrvBatteryCommonData.chargerTimeSecond = -1;
        if(status==BAT_STATUS_CHARGED)
        {
            uteDrvBatteryCommonData.chargedTimeSecond++;
        }
        else
        {
            uteDrvBatteryCommonData.chargedTimeSecond = 0;
        }

    }
    else
    {
        if(uteDrvBatteryCommonData.chargerTimeSecond==-1)
        {
            uteDrvBatteryCommonData.startChargerLvl = uteDrvBatteryCommonData.lastLvl;
            uteDrvBatteryCommonData.chargerTimeSecond = 0;
            uteDrvBatteryCommonData.chargerMAS = 0;
            uteDrvBatteryCommonData.chargedTimeSecond = 0;
        }
        uteDrvBatteryCommonData.chargerTimeSecond++;
        if (current > (UTE_DRV_BATTERY_ELECTRICITY_POWER_MAH))
        {
            current = UTE_DRV_BATTERY_ELECTRICITY_POWER_MAH;
        }
        uteDrvBatteryCommonData.chargerMAS = uteDrvBatteryCommonData.chargerMAS+current;
        UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,.startChargerLvl=%d,chargerTimeSecond=%d,current=%d,chargerMAS=%d", __func__,uteDrvBatteryCommonData.startChargerLvl,uteDrvBatteryCommonData.chargerTimeSecond,current,uteDrvBatteryCommonData.chargerMAS);
#if UTE_MODULE_BATTERY_COMPENSATION_SUPPORT
        lvl = uteDrvBatteryCommonData.startChargerLvl + (uint8_t)(uteDrvBatteryCommonData.chargerMAS*100.0f/(3600.0f*(UTE_DRV_BATTERY_ELECTRICITY_POWER_MAH*UTE_MODULE_BATTERY_COMPENSATION_PERCENT)));
        if (status == BAT_STATUS_CHARGING)
        {
            lvl = (lvl > 99) ? 99 : lvl;
        }
#else
        lvl = uteDrvBatteryCommonData.startChargerLvl + (uint8_t)(uteDrvBatteryCommonData.chargerMAS*100.0f/(3600.0f*UTE_DRV_BATTERY_ELECTRICITY_POWER_MAH));
#endif
    }
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,lvl=%d,.chargedTimeSecond=%d,lastLvl=%d", __func__,lvl,uteDrvBatteryCommonData.chargedTimeSecond,uteDrvBatteryCommonData.lastLvl);
    return lvl;
}
/**
*@brief 更新电池数据
*@details
*@author        zn.zeng
*@date        2021-12-15
*/
void uteDrvBatteryCommonUpdateBatteryInfo(void)
{
#if CHARGE_EN
    uteDrvBatteryCommonData.lvl = sys_cb.vbat_percent;
    uteDrvBatteryCommonData.voltage = sys_cb.vbat;//vbat_cb.vbat_adc_last;
#if UTE_DRV_BATTERY_CUSTOM_CHARGE_CURRENT_SUPPORT
    int16_t current = UTE_DRV_BATTERY_CHARGE_CURRENT;
    if (current <= 5 || current >= 320)
    {
        current = CHARGE_CONSTANT_CURR * 5 + 5;
    }
#else
    int16_t current = CHARGE_CONSTANT_CURR * 5 + 5;
#endif
    if(!sys_cb.chg_on || uteDrvBatteryCommonData.chargerStatus == BAT_STATUS_NO_CHARGE)
    {
        uteDrvBatteryCommonData.current = 0;
    }
    else if (uteDrvBatteryCommonData.voltage > UTE_DRV_BATTERY_090)
    {
        uteDrvBatteryCommonData.current = CHARGE_TRICKLE_CURR * 5 + 5;
    }
    else if (uteDrvBatteryCommonData.voltage > UTE_DRV_BATTERY_080)
    {
        uteDrvBatteryCommonData.current = current * 80 / 100;
    }
    else
    {
        uteDrvBatteryCommonData.current = current;
    }
#endif

#if UTE_MODULE_BATTERY_RECORD_BAT_LVL_SUPPORT
    if ((uteApplicationCommonGetSystemPowerOnSecond() < 10)&&(uteDrvBatteryCommonData.lastLvl!=0))
    {
        return;
    }
#endif
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,lvl=%d,voltage=%d,current=%d", __func__,uteDrvBatteryCommonData.lvl,uteDrvBatteryCommonData.voltage,uteDrvBatteryCommonData.current);

#if 1//UTE_DRV_BATTERY_USE_UTE_PERCENTAGE_SUPPORT
    uteDrvBatteryCommonData.lvl = uteDrvBatteryCommonCalibrationBatLvl(uteDrvBatteryCommonData.voltage,uteDrvBatteryCommonData.current,uteDrvBatteryCommonData.chargerStatus);
#endif

    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,lvl=%d,chargerStatus=%d", __func__,uteDrvBatteryCommonData.lvl,uteDrvBatteryCommonData.chargerStatus);
    if(uteDrvBatteryCommonData.lvl>=100)
    {
        uteDrvBatteryCommonData.lvl = 100;
    }
    /*!xjc delete, 2022-05-04*/
#if (!UTE_MODULE_BATTERY_CHARGED_DISPLAY_ON_SUPPORT)
    if((uteDrvBatteryCommonData.chargerStatus == BAT_STATUS_CHARGING)&&(uteDrvBatteryCommonData.lvl == 100))
    {
#if UTE_MODULE_BATTERY_LINEAR_CONSUME_SUPPORT
        if (uteDrvBatteryCommonData.bat100PercentTimeout == 0)
#endif
        {
            uteDrvBatteryCommonData.lvl = 99;
        }
    }
#endif
    if (uteDrvBatteryCommonData.chargerStatus == BAT_STATUS_CHARGED)
    {
        uteDrvBatteryCommonData.lvl = 100;
#if UTE_MODULE_BATTERY_LINEAR_CONSUME_SUPPORT
        uteDrvBatteryCommonData.bat100PercentTimeout = 2;
#endif
    }
    if (uteApplicationCommonIsStartupFinish() && (uteDrvBatteryCommonData.chargerStatus == BAT_STATUS_NO_CHARGE))
    {
        if (uteDrvBatteryCommonData.lvlCnt < UTE_DRV_BATTERY_AVG_LEVEL_BUFF_MAX)
        {
            uteDrvBatteryCommonData.lvlBuff[uteDrvBatteryCommonData.lvlCnt] = uteDrvBatteryCommonData.lvl;
            uteDrvBatteryCommonData.lvlCnt++;
        }
        else
        {
            uint32_t sum = 0;
            uint8_t minVaule = 0xff;
            uint8_t maxVaule = 0;
            uint8_t i = 0;
            uint8_t percent = 0;
            for (i = 0; i < UTE_DRV_BATTERY_AVG_LEVEL_BUFF_MAX; i++)
            {
                sum = sum + uteDrvBatteryCommonData.lvlBuff[i];
                if (uteDrvBatteryCommonData.lvlBuff[i] < minVaule)
                {
                    minVaule = uteDrvBatteryCommonData.lvlBuff[i];
                }
                if (uteDrvBatteryCommonData.lvlBuff[i] > maxVaule)
                {
                    maxVaule = uteDrvBatteryCommonData.lvlBuff[i];
                }
            }
            memset(&uteDrvBatteryCommonData.lvlBuff[0], 0, UTE_DRV_BATTERY_AVG_LEVEL_BUFF_MAX);
            percent = (sum - minVaule - maxVaule) / (UTE_DRV_BATTERY_AVG_LEVEL_BUFF_MAX - 2);
            UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,percent=%d,uteDrvBatteryCommonData.lastLvl=%d", __func__, percent, uteDrvBatteryCommonData.lastLvl);
            if ((percent < uteDrvBatteryCommonData.lastLvl) || (uteDrvBatteryCommonData.lastLvl == 0))
            {
#if UTE_MODULE_BATTERY_LINEAR_CONSUME_SUPPORT
                if (uteDrvBatteryCommonData.bat100PercentTimeout > 0)
                {
                    uteDrvBatteryCommonData.bat100PercentTimeout--;
                    uteDrvBatteryCommonData.lastLvl = 100;
                }
                else
                {
#if UTE_MODULE_BATTERY_SMOOTH_PERCENTAGE_SUPPORT
                    if (uteDrvBatteryCommonData.lastLvl > percent)
                    {
                        uteDrvBatteryCommonData.lastLvl = (uteDrvBatteryCommonData.lastLvl - 1 > percent) ? (uteDrvBatteryCommonData.lastLvl - 1) : percent;
                    }
                    else
#endif
                    {
                        uteDrvBatteryCommonData.lastLvl = percent;
                    }
                }
#else
#if UTE_MODULE_BATTERY_SMOOTH_PERCENTAGE_SUPPORT
                if (uteDrvBatteryCommonData.lastLvl > percent)
                {
                    uteDrvBatteryCommonData.lastLvl = (uteDrvBatteryCommonData.lastLvl - 1 > percent) ? (uteDrvBatteryCommonData.lastLvl - 1) : percent;
                }
                else
#endif
                {
                    uteDrvBatteryCommonData.lastLvl = percent;
                }
#endif
            }
            uteDrvBatteryCommonData.lvlCnt = 0;
        }
    }
    else
    {
        uteDrvBatteryCommonData.lvlCnt = 0;
        uteDrvBatteryCommonData.lastLvl = uteDrvBatteryCommonData.lvl;
    }
#if UTE_MODULE_BATTERY_RECORD_BAT_LVL_SUPPORT
    uteDrvBatteryCommonSaveBatteryLvl();
#endif
}
#if 0//UTE_DRV_BATTERY_CE_AUTH_SUPPORT
/**
*@brief    电池CE认证流程
*@details
*@author        zn.zeng
*@date        2022-01-03
*/
#include "ute_module_newFactoryTest.h"
#include "ute_module_gui_common.h"
void uteDrvBatteryCommonCEAuthHandler(ute_module_systemtime_time_t time)
{
    bool isStartTest = false;
    if(uteDrvBatteryCommonData.chargerStatus==BAT_STATUS_NO_CHARGE)
    {
        if((time.sec%10)==0)
        {
            isStartTest = true;
        }
#if UTE_DRV_CHARGE_CURRENT_SECTION_WITH_TEMPERATURE
        uteDrvBatteryCommonData.chargeCurrentValue = 0;
#endif
    }
    else
    {
        isStartTest = true;
    }
    if(uteDrvBatteryCommonData.ceParam.isChecking)
    {
        if(uteModuleTemperatureGetTestMode()==TEMPERATURE_MODE_AMBIENT_CHECK)
        {
            uteDrvBatteryCommonData.ceParam.ambitent = uteDrvTemperatureCommonGetAmbientValue();
            uteModuleTemperatureStopTesting();
        }
        UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,.ambitent=%f", __func__,uteDrvBatteryCommonData.ceParam.ambitent);
#ifdef UTE_DRV_NEW_FACTORY_TEST_BATTERY_CE_AUTH_NOT_ALLOW_TEMPERATURE
        ute_new_factory_test_data_t *data;
        uteModuleNewFactoryTestSetMode(&data);
        if (data->mode == FACTORY_TEST_MODE_AGING)
        {
            if(uteDrvBatteryCommonData.ceParam.ambitent>=UTE_DRV_NEW_FACTORY_TEST_BATTERY_CE_AUTH_NOT_ALLOW_TEMPERATURE)
            {
                uteModulePlatformOutputGpioSet(UTE_DRV_CHARGE_POWER_GPIO_PIN,!UTE_DRV_CHARGE_POWER_ACTION_LEVEL);
            }
            else if(uteDrvBatteryCommonData.ceParam.ambitent<=UTE_DRV_NEW_FACTORY_TEST_BATTERY_CE_AUTH_ALLOW_TEMPERATURE)
            {
                uteModulePlatformOutputGpioSet(UTE_DRV_CHARGE_POWER_GPIO_PIN,UTE_DRV_CHARGE_POWER_ACTION_LEVEL);
            }
        }
        else
#endif
        {
#if UTE_DRV_BATTERY_CE_AUTH_NOT_ALLOW_MIN_TEMPERATURE_SUPPORT
            if((uteDrvBatteryCommonData.ceParam.ambitent<=UTE_DRV_BATTERY_CE_AUTH_NOT_ALLOW_MIN_TEMPERATURE) || (uteDrvBatteryCommonData.ceParam.ambitent>=UTE_DRV_BATTERY_CE_AUTH_NOT_ALLOW_TEMPERATURE))
            {
                uteModulePlatformOutputGpioSet(UTE_DRV_CHARGE_POWER_GPIO_PIN,!UTE_DRV_CHARGE_POWER_ACTION_LEVEL);
            }
            else if((uteDrvBatteryCommonData.ceParam.ambitent>=UTE_DRV_BATTERY_CE_AUTH_ALLOW_MIN_TEMPERATURE) && (uteDrvBatteryCommonData.ceParam.ambitent<=UTE_DRV_BATTERY_CE_AUTH_ALLOW_TEMPERATURE))
            {
                uteModulePlatformOutputGpioSet(UTE_DRV_CHARGE_POWER_GPIO_PIN,UTE_DRV_CHARGE_POWER_ACTION_LEVEL);
            }
#else
            if(uteDrvBatteryCommonData.ceParam.ambitent>=UTE_DRV_BATTERY_CE_AUTH_NOT_ALLOW_TEMPERATURE)
            {
                uteModulePlatformOutputGpioSet(UTE_DRV_CHARGE_POWER_GPIO_PIN,!UTE_DRV_CHARGE_POWER_ACTION_LEVEL);
            }
            else if(uteDrvBatteryCommonData.ceParam.ambitent<=UTE_DRV_BATTERY_CE_AUTH_ALLOW_TEMPERATURE)
            {
                uteModulePlatformOutputGpioSet(UTE_DRV_CHARGE_POWER_GPIO_PIN,UTE_DRV_CHARGE_POWER_ACTION_LEVEL);
            }
#endif
        }
#if UTE_DRV_CHARGE_CURRENT_SECTION_WITH_TEMPERATURE
        if(uteDrvBatteryCommonData.ceParam.ambitent>=UTE_DRV_CHARGE_SECTION_TEMPERATURE_LEVEL)
        {
            if(uteDrvBatteryCommonData.chargeCurrentValue != UTE_DRV_CHARGE_SECTION_TEMPERATURE_CURRENT_HIGH)
            {
                uteModulePlatformSetChargeCurrent(UTE_DRV_CHARGE_SECTION_TEMPERATURE_CURRENT_HIGH);
                uteDrvBatteryCommonData.chargeCurrentValue = UTE_DRV_CHARGE_SECTION_TEMPERATURE_CURRENT_HIGH;
            }
        }
        else
        {
            if(uteDrvBatteryCommonData.chargeCurrentValue != UTE_DRV_CHARGE_SECTION_TEMPERATURE_CURRENT_LOW)
            {
                uteModulePlatformSetChargeCurrent(UTE_DRV_CHARGE_SECTION_TEMPERATURE_CURRENT_LOW);
                uteDrvBatteryCommonData.chargeCurrentValue = UTE_DRV_CHARGE_SECTION_TEMPERATURE_CURRENT_LOW;
            }
        }
#endif
        uteDrvBatteryCommonData.ceParam.isChecking = false;
    }
    else if(isStartTest)
    {
        if(!uteModuleTemperatureIsTesting())
        {
            uteModuleTemperatureStartTesting(TEMPERATURE_MODE_AMBIENT_CHECK);
            uteDrvTemperatureCommonEverySecond();
        }
        else
        {
            uteDrvBatteryCommonData.ceParam.ambitent = uteDrvTemperatureCommonGetAmbientValue();
        }
        uteDrvBatteryCommonData.ceParam.isChecking = true;
    }
}
/**
*@brief    获取环境温度
*@details
*@author        zn.zeng
*@date        2022-01-03
*/
float uteDrvBatteryCommonGetCEAuthAmbientValue(void)
{
    return uteDrvBatteryCommonData.ceParam.ambitent;
}
#endif
/**
*@brief     电池是否进入低电状态函数
*@details
*@author        xjc
*@date        2022-1-3
*/
void uteDrvBatteryCommonIsEnterLowBattery(void)
{
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
#if UTE_DRV_BATTERY_LOW_POWER_FIRST_PERECNT
    if ((uteApplicationCommonIsPowerOn())&&(uteApplicationCommonIsStartupFinish())&&\
        (uteApplicationCommonGetSystemPowerOnSecond()>8)&&(uteDrvBatteryCommonData.chargerStatus == BAT_STATUS_NO_CHARGE)&&\
        (uteDrvBatteryCommonGetLvl()>=UTE_DRV_BATTERY_LOW_POWER_PERECNT)&&(uteDrvBatteryCommonGetLvl() < UTE_DRV_BATTERY_LOW_POWER_FIRST_PERECNT))
    {
        if (!uteDrvBatteryCommonData.isHasLowPowerFirstNotify)
        {
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_LOW_BATTERY_NOTIFY_ID);
            uteDrvBatteryCommonData.isHasLowPowerFirstNotify = true;
        }
        return;
    }
    else
    {
        uteDrvBatteryCommonData.isHasLowPowerFirstNotify = false;
    }
#endif
    if (
        (uteApplicationCommonIsPowerOn()) &&
        (uteApplicationCommonIsStartupFinish()) &&
        (uteDrvBatteryCommonGetLvl()>UTE_DRV_BATTERY_AUTO_POWER_OFF_LVL)&&
        (uteDrvBatteryCommonGetLvl()<UTE_DRV_BATTERY_LOW_POWER_PERECNT)&&
#if SET_THE_LOW_BATTERY_POPUP_TIME
        (uteApplicationCommonGetlowBatterySecond()>SET_THE_LOW_BATTERY_POPUP_TIME)&&
#else
        (uteApplicationCommonGetSystemPowerOnSecond()>8) &&
#endif
        // (uteModuleGuiCommonGetCurrentScreenId() != UTE_MOUDLE_SCREENS_POWEROFF_ID)&&
        (uteDrvBatteryCommonData.chargerStatus == BAT_STATUS_NO_CHARGE)
#if UTE_MODULE_SPORT_SUPPORT
        &&(!uteModuleSportMoreSportIsRuning()
          ) /*! 运动中在运动处理函数跳转到低电界面，xjc 2022-02-18*/
#endif
    )
    {
        if (!uteDrvBatteryCommonData.isHasLowPowerNotify)
        {
#if UTE_MODULE_PLAYBACK_SUPPORT
            /*2022 08-17  低电量警报的时候，暂停本地音乐*/

            if(uteModuleMusicGetPlayerPaused()==false)
            {
                uint8_t channel =  uteModuleMusicGetPlayChannel();

                if((channel == UTE_MUSIC_PLAY_CHANNEL_LOCAL_TO_EARPHONE )&&(uteModuleMusicGetPlayerPaused()==false))
                {
                    uteModuleMusicCtrlPausedBeforeOperationStop();
                    uteModuleModuleMusicSetPauseplaybackStatusState(UTE_MUSIC_LOWPOWER);
                    return;
                }
                uteModuleMusicCtrlPausedBeforeOperationStop();

            }
#endif

            /*! 低电提醒之前结束运动再跳转到低电界面, xjc 2022-01-26*/
#if UTE_MODULE_SPORT_SUPPORT
            if(uteModuleSportMoreSportIsRuning())
            {
                uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CLOSE);
                uteModuleSportStopMoreSports();
            }
#endif
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_COVER_LOW_BATTERY;
            sys_cb.remind_tag = true;
            //msgbox((char*)i18n[STR_LOW_BATTERY], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_LOW_BATTERY);
            uteDrvBatteryCommonData.isHasLowPowerNotify = true;
#if SET_THE_LOW_BATTERY_POPUP_TIME
            uteApplicationCommonSetlowBatterySecond(0);
#endif
            /*2022 08-08  低电暂停音乐*/
            if(uteModuleMusicGetPlayerPaused()==false)
            {
                uteModuleMusicSetPlayerPaused(true,UTE_MUSIC_PLAY_CHANNEL_LOCAL_TO_SPEAKER);
            }
#if UTE_BT30_CALL_SUPPORT
#if UTE_MODULE_BT_LOW_BAT_NOT_ALLOW_POWER_ON_SUPPORT
            if (uteModuleCallBtIsPowerOn())
            {
                uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_BUTTON);
            }
#endif
#endif
        }
    }
    else
    {
        uteDrvBatteryCommonData.isHasLowPowerNotify = false;
    }
#endif
}

/**
*@brief 电池每秒函数
*@details
*@author        zn.zeng
*@date        2021-12-15
*/
void uteDrvBatteryCommonEverySecond(void)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    bool isStartAutoPowerOff = false;

    vbat_percent_process();
    // charge_timer_callback();

#if CHARGE_EN
    if(uteDrvBatteryCommonData.chargerStatus != bsp_charge_sta_get())
    {
        uteDrvBatteryCommonChargerChangeCb(bsp_charge_sta_get());
    }
#endif

    if(uteDrvBatteryCommonData.chargerStatus==BAT_STATUS_NO_CHARGE)
    {
        if((time.sec%10)==0)
        {
            uteDrvBatteryCommonUpdateBatteryInfo();
            if(uteApplicationCommonIsPowerOn()&&(uteDrvBatteryCommonGetLvl()<UTE_DRV_BATTERY_AUTO_POWER_OFF_LVL)&&uteApplicationCommonGetSystemPowerOnSecond() > 10)
            {
                isStartAutoPowerOff = true;
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_START_POWER_OFF,0);
            }
        }
    }
    else
    {
        uteDrvBatteryCommonUpdateBatteryInfo();
    }
    if(!isStartAutoPowerOff)
    {
#if 0 //UTE_DRV_BATTERY_CE_AUTH_SUPPORT
        if(uteApplicationCommonIsPowerOn()||uteDrvBatteryCommonData.ceParam.isChecking)
        {
            uteDrvBatteryCommonCEAuthHandler(time);
        }
        if(!uteApplicationCommonIsPowerOn())//dengli.lu 2022.07.23防止高温情况下关机，导致无法充电开机
        {
            uteModulePlatformOutputGpioSet(UTE_DRV_CHARGE_POWER_GPIO_PIN,UTE_DRV_CHARGE_POWER_ACTION_LEVEL);
        }
#endif
        uteDrvBatteryCommonIsEnterLowBattery();
    }
#if UTE_MODULE_NEW_FACTORY_AGING_RESTART_CHARGE_SUPPORT
    ute_new_factory_test_data_t *data;
    uteModuleNewFactoryTestSetMode(&data);
    if(data->mode == FACTORY_TEST_MODE_AGING)
    {
        if(uteDrvBatteryCommonData.isPlugIn)
        {
            if(uteDrvBatteryCommonData.chargerStatus == BAT_STATUS_CHARGED)
            {
                uteDrvBatteryCommonData.fullFactoryAgingLvl=100;
            }
        }
        else
        {
            uteDrvBatteryCommonData.fullFactoryAgingLvl=0;
        }
        if(uteDrvBatteryCommonData.fullFactoryAgingLvl>uteDrvBatteryCommonData.lastLvl)
        {
            uteDrvBatteryCommonData.chargerTimeSecond = -1;
            uteModulePlatformChargeRestart();
            uteDrvBatteryCommonData.fullFactoryAgingLvl=0;
        }
    }
#endif
}
/**
*@brief 电池初始化
*@details
*@author        zn.zeng
*@date        2021-11-08
*/
void uteDrvBatteryCommonInit(void)
{
    bool isFastPlugIn = false;

    bsp_vbat_percent_init();

    if(uteDrvBatteryCommonData.isPlugIn)
    {
        isFastPlugIn = true;
    }
    memset(&uteDrvBatteryCommonData,0,sizeof(ute_drv_battery_common_data_t));
    uteDrvBatteryCommonData.chargerStatus = BAT_STATUS_NO_CHARGE;
    uteModuleSystemtimeRegisterSecond(uteDrvBatteryCommonEverySecond);
    uteDrvBatteryCommonUpdateBatteryInfo();
    uteDrvBatteryCommonData.lastLvl = uteDrvBatteryCommonData.lvl;
#if UTE_MODULE_BATTERY_RECORD_BAT_LVL_SUPPORT
    uteDrvBatteryCommonCompareBatLvl();
#endif
    sys_cb.vbat_percent = uteDrvBatteryCommonData.lastLvl;
    uteDrvBatteryCommonData.isDelayDisplayCharger = false;
    uteDrvBatteryCommonData.startChargerLvl = 0;
    uteDrvBatteryCommonData.chargerTimeSecond = -1;
#if UTE_MODULE_BATTERY_CHARGED_DISPLAY_ON_SUPPORT
    uteDrvBatteryCommonData.alreadyDisplayOn = false;
#endif
    if(isFastPlugIn)
    {
        uteDrvBatteryCommonData.isPlugIn = true;
        uteDrvBatteryCommonData.isDelayDisplayCharger = true;
    }
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s, lastLvl:%d", __func__, uteDrvBatteryCommonData.lastLvl);
}
/**
*@brief 获取当前电压
*@details 获取当前电压，单位为mV
*@return 返回当前电池电压值
*@author        zn.zeng
*@date        Jul 05, 2021
*/
uint16_t uteDrvBatteryCommonGetVoltage(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,.voltage=%d", __func__,uteDrvBatteryCommonData.voltage);
    return uteDrvBatteryCommonData.voltage;
}
/**
*@brief 获取当前电电量
*@details 获取当前电量，0~100%
*@return 返回当前电量
*@author        zn.zeng
*@date        2021-08-18
*/
uint8_t uteDrvBatteryCommonGetLvl(void)
{
    return uteDrvBatteryCommonData.lastLvl;
}
/**
*@brief 获取当前电池的最大电压
*@details 获取当前电压，单位为mV
*@return 返回当前电池电压值
*@author        zn.zeng
*@date        Jul 05, 2021
*/
uint16_t uteDrvBatteryCommonGetMaxVoltage(void)
{
    return UTE_DRV_BATTERY_MAX_VOLTAGE;
}
/**
*@brief 获取当前充电状态
*@details
*@return ute_drv_battery_charger_status_t status
*@author        zn.zeng
*@date        2021-11-08
*/
ute_drv_battery_charger_status_t uteDrvBatteryCommonGetChargerStatus(void)
{
    return uteDrvBatteryCommonData.chargerStatus;
}
/**
*@brief    设置充电是否插入
*@details
*@author        zn.zeng
*@date        2021-12-17
*/
AT(.text.charge_com.sta)
void uteDrvBatteryCommonSetChargerPlug(bool isPlugIn)
{
    uteDrvBatteryCommonData.isPlugIn = isPlugIn;
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,isPlugIn=%d", __func__, isPlugIn);
    if(!uteDrvBatteryCommonData.isPlugIn)
    {
#if UTE_MODULE_BATTERY_CHARGED_DISPLAY_ON_SUPPORT
        uteDrvBatteryCommonData.alreadyDisplayOn = false;
#endif
        uteDrvBatteryCommonData.chargerStatus = BAT_STATUS_NO_CHARGE;
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
        if(uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_NEW_FACTORY_AGING_ID ||
           uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_NEW_FACTORY_MODULE_ID ||
           uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_NEW_FACTORY_MODE_SELECT_ID ||
           uteModuleGuiCommonGetCurrentScreenId() == FUNC_ONLINE_FACTORY_TEST)
        {
            return;
        }
#endif
        if(uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_CHARGER_ID)
        {
            uteModuleGuiCommonGoBackLastScreen();
        }
    }
    else
    {
        if(uteApplicationCommonIsStartupFinish()&&(!uteApplicationCommonIsPowerOn()))
        {
            uteApplicationCommonPowerOn();
        }
        else
        {
            if(uteApplicationCommonIsStartupFinish())
            {
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
                if(uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_NEW_FACTORY_AGING_ID ||
                   uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_NEW_FACTORY_MODULE_ID ||
                   uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_NEW_FACTORY_MODE_SELECT_ID ||
                   uteModuleGuiCommonGetCurrentScreenId() == FUNC_ONLINE_FACTORY_TEST)
                {
                    return;
                }
#endif
                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_CHARGER_ID, 0, __func__);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            }
            else
            {
                uteDrvBatteryCommonData.isDelayDisplayCharger= true;
            }
        }
    }
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s,uteDrvBatteryCommonData.isDelayDisplayCharger=%d", __func__,  uteDrvBatteryCommonData.isDelayDisplayCharger);
}
/**
*@brief    获取充电是否插入
*@details
*@author        xjc
*@date        2022-05-09
*/
AT(.text.charge_com.sta)
bool uteDrvBatteryCommonGetChargerPlug(void)
{
    return uteDrvBatteryCommonData.isPlugIn;
}
/**
*@brief    开机时是否延时显示
*@details
*@author        zn.zeng
*@date        2021-12-31
*/
bool uteDrvBatteryCommonIsDelayDisplayCharger(void)
{
    return uteDrvBatteryCommonData.isDelayDisplayCharger;
}
/**
*@brief    设置充电时间
*@details
*@author        ldl
*@date        2023-11-10
*/
void uteDrvBatteryCommonSetChargerTimeSecond(int chargerTimeSecond)
{
    uteDrvBatteryCommonData.chargerTimeSecond = chargerTimeSecond;
}
/**
*@brief 获取当前电池电量等级，即第几格电量
*@details
*@param[in] uint8_t  totalIndex  总的电量格数
*@return    uint8_t  batIndex    当前电量格数
*@author        xjc
*@date        2021-12-27
*/
uint8_t uteDrvBatteryCommonGetBatteryIndex(uint8_t totalIndex)
{
    uint8_t batIndex = 0;
    ute_drv_battery_charger_status_t chargerStatus;
    // uteDrvBatteryCommonData.lastLvl=100;
    batIndex = uteDrvBatteryCommonData.lastLvl / (100/(totalIndex-1));
    chargerStatus = uteDrvBatteryCommonGetChargerStatus();
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if(uteDrvBatteryCommonData.lastLvl%(100/(totalIndex-1)))
    {
        if((uteDrvBatteryCommonData.lastLvl<UTE_DRV_BATTERY_LOW_POWER_PERECNT)&&(chargerStatus==BAT_STATUS_NO_CHARGE))
        {
            // 电池电量小于低电值，让电池显示感叹号
        }
        else
        {
            batIndex = batIndex+1;
        }
    }
    if(batIndex>(totalIndex-1))
    {
        batIndex = totalIndex-1;
    }
    if(chargerStatus==BAT_STATUS_CHARGING)
    {
        if (time.sec%2)
        {
            if(batIndex>0)
            {
                batIndex = batIndex-1;
            }
        }
    }
    return batIndex;
}

#if UTE_MODULE_BATTERY_CHARGED_DISPLAY_ON_SUPPORT
/**
*@brief     获取充满电后是否亮屏过
*@details
*@author        xjc
*@date        2022-04-20
*/
bool uteDrvBatteryCommonGetAlreadyDisplayOnParam(void)
{
    return uteDrvBatteryCommonData.alreadyDisplayOn;
}
#endif

#if UTE_MODULE_BATTERY_SAVE_LAST_LVL_BEFORE_FACTORY_SUPPORT
/**
*@brief      保存电量信息
*@details    每次恢复出厂设置前保存电量信息到UTE_BLE_SN1_ADDRESS
*@author        xjc
*@date        2023-02-03
*/
void uteDrvBatteryCommonSaveLastLvlToSN1(void)
{
    ute_application_sn_data_t snData;
    uteModulePlatformFlashNorRead((uint8_t *)&snData,UTE_USER_PARAM_ADDRESS,sizeof(ute_application_sn_data_t));
    uteModulePlatformFlashNorErase(UTE_USER_PARAM_ADDRESS);
    snData.beforeFactoryBatLvl = uteDrvBatteryCommonData.lastLvl;
    uteModulePlatformFlashNorWrite((uint8_t *)&snData,UTE_USER_PARAM_ADDRESS,sizeof(ute_application_sn_data_t));
}
/**
*@brief      恢复电量信息
*@details    每次恢复出厂设置后第一次开机恢复电量信息
*@author        xjc
*@date        2023-02-03
*/
void uteDrvBatteryCommonReadLastLvlFromSN1(void)
{
    ute_application_sn_data_t snData;
    uteModulePlatformFlashNorRead((uint8_t *)&snData,UTE_USER_PARAM_ADDRESS,sizeof(ute_application_sn_data_t));
    if (snData.beforeFactoryBatLvl != 0 && snData.beforeFactoryBatLvl != 0xFF)
    {
        uteDrvBatteryCommonData.lastLvl = snData.beforeFactoryBatLvl;
        snData.beforeFactoryBatLvl = 0xFF;
        uteModulePlatformFlashNorErase(UTE_USER_PARAM_ADDRESS);
        uteModulePlatformFlashNorWrite((uint8_t *)&snData,UTE_USER_PARAM_ADDRESS,sizeof(ute_application_sn_data_t));
    }
}
#endif

#else

/**
*@brief 电池每秒函数
*@details
*@author        zn.zeng
*@date        2021-12-15
*/
void uteDrvBatteryCommonEverySecond(void)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    //bool isStartAutoPowerOff = false;

    vbat_percent_process();
    // charge_timer_callback();

    if(uteDrvBatteryCommonGetChargerStatus() == BAT_STATUS_NO_CHARGE)
    {
        if((time.sec%10)==0)
        {
            if(uteApplicationCommonIsPowerOn()&&(uteDrvBatteryCommonGetLvl()<UTE_DRV_BATTERY_AUTO_POWER_OFF_LVL)&&uteApplicationCommonGetSystemPowerOnSecond() > 10)
            {
                //isStartAutoPowerOff = true;
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_START_POWER_OFF,0);
            }
        }
    }

}

/**
*@brief 电池初始化
*@details
*@author        zn.zeng
*@date        2021-11-08
*/
void uteDrvBatteryCommonInit(void)
{
    uteModuleSystemtimeRegisterSecond(uteDrvBatteryCommonEverySecond);
    bsp_vbat_percent_init();
    UTE_MODULE_LOG(UTE_LOG_DRV_BAT_LVL, "%s", __func__);
}

/**
*@brief 获取当前电压
*@details 获取当前电压，单位为mV
*@return 返回当前电池电压值
*@author        zn.zeng
*@date        Jul 05, 2021
*/
uint16_t uteDrvBatteryCommonGetVoltage(void)
{
    return sys_cb.vbat;
}
/**
*@brief 获取当前电电量
*@details 获取当前电量，0~100%
*@return 返回当前电量
*@author        zn.zeng
*@date        2021-08-18
*/
uint8_t uteDrvBatteryCommonGetLvl(void)
{
    return bsp_vbat_percent_get();
}
/**
*@brief 获取当前电池的最大电压
*@details 获取当前电压，单位为mV
*@return 返回当前电池电压值
*@author        zn.zeng
*@date        Jul 05, 2021
*/
uint16_t uteDrvBatteryCommonGetMaxVoltage(void)
{
    return UTE_DRV_BATTERY_MAX_VOLTAGE;
}
/**
*@brief 获取当前充电状态
*@details
*@return ute_drv_battery_charger_status_t status
*@author        zn.zeng
*@date        2021-11-08
*/
ute_drv_battery_charger_status_t uteDrvBatteryCommonGetChargerStatus(void)
{
    return sys_cb.charge_sta ;
}

/**
*@brief 获取当前电池电量等级，即第几格电量
*@details
*@param[in] uint8_t  totalIndex  总的电量格数
*@return    uint8_t  batIndex    当前电量格数
*@author        xjc
*@date        2021-12-27
*/
uint8_t uteDrvBatteryCommonGetBatteryIndex(uint8_t totalIndex)
{
    uint8_t batIndex = 0;
    ute_drv_battery_charger_status_t chargerStatus;
    batIndex = uteDrvBatteryCommonGetLvl() / (100 / (totalIndex - 1));
    chargerStatus = uteDrvBatteryCommonGetChargerStatus();
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if (uteDrvBatteryCommonGetLvl() % (100 / (totalIndex - 1)))
    {
        if ((uteDrvBatteryCommonGetLvl() < UTE_DRV_BATTERY_LOW_POWER_PERECNT) && (chargerStatus == BAT_STATUS_NO_CHARGE))
        {
            // 电池电量小于低电值，让电池显示感叹号
        }
        else
        {
            batIndex = batIndex + 1;
        }
    }
    if (batIndex > (totalIndex - 1))
    {
        batIndex = totalIndex - 1;
    }
    if (chargerStatus == BAT_STATUS_CHARGING)
    {
        if (time.sec % 2)
        {
            if (batIndex > 0)
            {
                batIndex = batIndex - 1;
            }
        }
    }
    return batIndex;
}

#endif

