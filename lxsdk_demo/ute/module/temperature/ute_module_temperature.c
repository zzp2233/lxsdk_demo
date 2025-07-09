/**
*@file
*@brief        体温模块
*@details
*@author       zn.zeng
*@date       2021-12-01
*@version      v1.0
*/

#include "ute_module_temperature.h"
#include "ute_module_log.h"
// #include "ute_temperature_algorithms.h"
#include "ute_module_filesystem.h"
#include "ute_drv_temperature_common.h"
#include "ute_module_heart.h"
#include "ute_module_protocol.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_crc.h"
#include "ute_drv_motor.h"

#if UTE_MODULE_TEMPERATURE_SUPPORT
/*! 体温数据结构zn.zeng, 2021-12-02  */
ute_module_temperature_data_t uteModuleTemperatureData;
/**
*@brief        读取参数
*@details
*@author       zn.zeng
*@date       2021-12-04
*/
void uteModuleTemperatureReadConfig(void)
{
    /*! 保存到文件zn.zeng, 2021-12-04*/
    void *file;
    uint8_t readbuff[19];
    memset(readbuff,0,19);
    readbuff[0] = UTE_MODULE_TEMPERATURE_AUTO_TEST_OPEN;
    readbuff[1] = (UTE_MODULE_TEMPERATURE_AUTO_TEST_INTERVAL_SECOND>>24)&0xff;
    readbuff[2] = (UTE_MODULE_TEMPERATURE_AUTO_TEST_INTERVAL_SECOND>>16)&0xff;
    readbuff[3] = (UTE_MODULE_TEMPERATURE_AUTO_TEST_INTERVAL_SECOND>>8)&0xff;
    readbuff[4] = (UTE_MODULE_TEMPERATURE_AUTO_TEST_INTERVAL_SECOND)&0xff;
    readbuff[5] = UTE_MODULE_TEMPERATURE_AUTO_LIMIT_TIME_OPEN;
    readbuff[6] = (UTE_MODULE_TEMPERATURE_AUTO_LIMIT_TIME_START_SECOND>>24)&0xff;
    readbuff[7] = (UTE_MODULE_TEMPERATURE_AUTO_LIMIT_TIME_START_SECOND>>16)&0xff;
    readbuff[8] = (UTE_MODULE_TEMPERATURE_AUTO_LIMIT_TIME_START_SECOND>>8)&0xff;
    readbuff[9] = (UTE_MODULE_TEMPERATURE_AUTO_LIMIT_TIME_START_SECOND)&0xff;
    readbuff[10] = (UTE_MODULE_TEMPERATURE_AUTO_LIMIT_TIME_STOP_SECOND>>24)&0xff;
    readbuff[11] = (UTE_MODULE_TEMPERATURE_AUTO_LIMIT_TIME_STOP_SECOND>>16)&0xff;
    readbuff[12] = (UTE_MODULE_TEMPERATURE_AUTO_LIMIT_TIME_STOP_SECOND>>8)&0xff;
    readbuff[13] = (UTE_MODULE_TEMPERATURE_AUTO_LIMIT_TIME_STOP_SECOND)&0xff;
    readbuff[14] = UTE_MODULE_TEMPERATURE_WARNING_OPEN;
    readbuff[15] = (UTE_MODULE_TEMPERATURE_WARNING_LOW_VALUE>>8)&0xff;
    readbuff[16] = (UTE_MODULE_TEMPERATURE_WARNING_LOW_VALUE)&0xff;
    readbuff[17] = (UTE_MODULE_TEMPERATURE_WARNING_HEIGHT_VALUE>>8)&0xff;
    readbuff[18] = (UTE_MODULE_TEMPERATURE_WARNING_HEIGHT_VALUE)&0xff;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_TEMPERATURE_SET_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],19);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleTemperatureData.isAutoTestOpen = readbuff[0];
    uteModuleTemperatureData.autoTestIntervalSecond = readbuff[1]<<24|readbuff[2]<<16|readbuff[3]<<8|readbuff[4];
    uteModuleTemperatureData.isAutoTestLimitTimeOpen = readbuff[5];
    uteModuleTemperatureData.autoTestLimitStartTimeSecond = readbuff[6]<<24|readbuff[7]<<16|readbuff[8]<<8|readbuff[9];
    uteModuleTemperatureData.autoTestLimitStopTimeSecond = readbuff[10]<<24|readbuff[11]<<16|readbuff[12]<<8|readbuff[13];
    uteModuleTemperatureData.isTemperatureWarningOpen = readbuff[14];
    uteModuleTemperatureData.warningLowValue = readbuff[15]<<8|readbuff[16];
    uteModuleTemperatureData.warningHeightValue = readbuff[17]<<8|readbuff[18];
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,isAutoTestOpen=%d,autoTestIntervalSecond=%d", __func__,uteModuleTemperatureData.isAutoTestOpen,uteModuleTemperatureData.autoTestIntervalSecond);
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,isAutoTestLimitTimeOpen=%d,autoTestLimitStartTimeSecond=%d,autoTestLimitStopTimeSecond=%d", __func__,uteModuleTemperatureData.isAutoTestLimitTimeOpen,uteModuleTemperatureData.autoTestLimitStartTimeSecond,uteModuleTemperatureData.autoTestLimitStopTimeSecond);
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,isTemperatureWarningOpen=%d,warningLowValue=%d,warningHeightValue=%d", __func__,uteModuleTemperatureData.isTemperatureWarningOpen,uteModuleTemperatureData.warningLowValue,uteModuleTemperatureData.warningHeightValue);
}

/**
*@brief   体温模块初始化
*@details
*@author        zn.zeng
*@date        2021-12-01
*/
void uteModuleTemperatureInit(void)
{
    memset(&uteModuleTemperatureData,0,sizeof(ute_module_temperature_data_t));
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR, NULL, NULL);
#endif
    // UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,lib version=%s", __func__,getTemperatureLibVersion());
    uteModuleTemperatureReadConfig();
    uteDrvTemperatureCommonInit();
    uteDrvTemperatureCommonPowerOff();
    uteModuleSystemtimeRegisterSecond(uteModuleTemperatureEverySecond);
}
/**
*@brief        发送实时数据给app
*@details
*@author       zn.zeng
*@date       2021-12-02
*/
void uteModuleTemperatureSendRealTimeDataToPhone(ute_module_systemtime_time_t time)
{
    uint8_t response[20];
    uint16_t bodyTemp = (uint16_t)(uteModuleTemperatureData.bodyTemperature*100.0f);
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"%s,.bodyTemp=%d",__func__,bodyTemp);

    if(uteModuleTemperatureData.testMode==TEMPERATURE_MODE_BAND_SINGLE)
    {
        if(bodyTemp!=0)
        {
            response[0] = CMD_TEMPERATURE_HEAD;
            response[1] = 0xf0;
            response[2] = time.year >> 8 & 0xff;
            response[3] = time.year & 0xff;
            response[4] = time.month;
            response[5] = time.day;
            response[6] = time.hour;
            response[7] = time.min;
            response[8] = time.sec;
            response[9] = bodyTemp >> 8 & 0xff;
            response[10] = bodyTemp & 0xff;
            uteModuleProfileBleSendToPhone(&response[0],11);
        }
    }
    else if(uteModuleTemperatureData.testMode==TEMPERATURE_MODE_APP_SINGLE)
    {
        if(uteModuleTemperatureData.testingSecond>25)
        {
            response[0] = CMD_TEMPERATURE_HEAD;
            response[1] = 0x01;
            response[2] = time.year >> 8 & 0xff;
            response[3] = time.year & 0xff;
            response[4] = time.month;
            response[5] = time.day;
            response[6] = time.hour;
            response[7] = time.min;
            response[8] = time.sec;
            response[9] = bodyTemp >> 8 & 0xff;
            response[10] = bodyTemp & 0xff;
            uteModuleProfileBleSendToPhone(&response[0],11);
            uteModuleTemperatureStopTesting();
        }
    }
}
/**
*@brief        自动测试流程
*@details
*@author       zn.zeng
*@date       2021-12-02
*/
void uteModuleTemperatureAutoTestProcess(ute_module_systemtime_time_t time)
{
    uint32_t second = time.hour*3600+time.min*60+time.sec;
    bool isAllowAutoTest = false;
    if(uteModuleTemperatureData.isAutoTestLimitTimeOpen)
    {
        if(uteModuleTemperatureData.autoTestLimitStartTimeSecond<uteModuleTemperatureData.autoTestLimitStopTimeSecond)
        {
            if((second>=uteModuleTemperatureData.autoTestLimitStartTimeSecond)&&(second<=uteModuleTemperatureData.autoTestLimitStopTimeSecond))
            {
                isAllowAutoTest = true;

            }
            else
            {
                isAllowAutoTest = false;
            }
        }
        else
        {
            if((second<=uteModuleTemperatureData.autoTestLimitStartTimeSecond)||(second>=uteModuleTemperatureData.autoTestLimitStopTimeSecond))
            {
                isAllowAutoTest = true;

            }
            else
            {
                isAllowAutoTest = false;
            }
        }
    }
    else
    {
        isAllowAutoTest= true;
    }
//    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"%s,.isAllowAutoTest=%d,autoTestIntervalSecond=%d,second=%d",__func__,isAllowAutoTest,uteModuleTemperatureData.autoTestIntervalSecond,second);
    if(isAllowAutoTest)
    {
        if((second%uteModuleTemperatureData.autoTestIntervalSecond)==0)
        {
            uteModuleTemperatureStartTesting(TEMPERATURE_MODE_BAND_AUTO);
        }
    }
}
/**
*@brief        警告流程
*@details
*@author       zn.zeng
*@date       2021-12-02
*/
void uteModuleTemperatureWarningProcess(ute_module_systemtime_time_t time)
{
    if(uteModuleTemperatureData.isTemperatureWarningOpen)
    {
        uint16_t bodyTemp = (uint16_t)(uteModuleTemperatureData.bodyTemperature*100.0f);
//        bodyTemp = 3800;//test
        if(((bodyTemp<=uteModuleTemperatureData.warningLowValue)||(bodyTemp>=uteModuleTemperatureData.warningHeightValue))&&bodyTemp>3400)
        {
            UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"%s,.warningSecond=%d,isHasTemperatureWarning = %d",__func__,uteModuleTemperatureData.warningSecond,uteModuleTemperatureData.isHasTemperatureWarning);
            uteModuleTemperatureData.warningSecond++;
            if(uteModuleTemperatureData.warningSecond>60)
            {
                if(!uteModuleTemperatureData.isHasTemperatureWarning)
                {
                    uteModuleTemperatureData.isHasTemperatureWarning = true;
                    // notify
                    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"%s,warning!!",__func__);
#if UTE_MODULE_SCREENS_TEMPERATURE_WARNING_SUPPORT
                    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_TEMPERATURE_WARNING_ID, 0, __func__);
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
#endif
                }
            }
        }
        else
        {
            uteModuleTemperatureData.warningSecond = 0;
            uteModuleTemperatureData.isHasTemperatureWarning = false;
        }
    }
    else
    {
        uteModuleTemperatureData.isHasTemperatureWarning = false;
    }
}
/**
*@brief        体温每秒函数
*@details       需要注册到主时间，每秒执行一次
*@author       zn.zeng
*@date       2021-12-02
*/
void uteModuleTemperatureEverySecond(void)
{
    if(!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if(uteModuleTemperatureData.isAutoTestOpen)
    {
        uteModuleTemperatureAutoTestProcess(time);
    }
    if((uteModuleTemperatureData.testMode==TEMPERATURE_MODE_APP_SINGLE)||
       (uteModuleTemperatureData.testMode==TEMPERATURE_MODE_BAND_SINGLE)||
       (uteModuleTemperatureData.testMode==TEMPERATURE_MODE_BAND_AUTO))
    {
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"%s,.testingSecond=%d",__func__,uteModuleTemperatureData.testingSecond);
        if(uteModuleTemperatureIsWear())
        {
            if(uteModuleTemperatureData.testingSecond>UTE_MODULE_TEMPERATURE_TEST_TIMEOUT_SECOND)
            {
                uteModuleTemperatureStopTesting();
            }
            else
            {
                uteDrvTemperatureCommonEverySecond();
                uteModuleTemperatureData.ambitentTemperature  = uteDrvTemperatureCommonGetAmbientValue();
                uteModuleTemperatureData.skinTemperature = uteDrvTemperatureCommonGetSkinValue();
                uteModuleTemperatureData.bodyTemperature = uteBodyTemperatureAnalysis(uteModuleTemperatureData.skinTemperature,uteModuleTemperatureData.ambitentTemperature);
                uteModuleTemperatureSendRealTimeDataToPhone(time);
                if((uteModuleTemperatureData.testMode==TEMPERATURE_MODE_BAND_AUTO)&&(uteModuleTemperatureData.bodyTemperature>1.0f))
                {
                    uteModuleTemperatureStopTesting();
                }
                uteModuleTemperatureWarningProcess(time);
                uteModuleTemperatureData.testingSecond++;
            }
        }
        else
        {
            uteModuleTemperatureStopTesting();
        }
    }
    else if(uteModuleTemperatureData.testMode==TEMPERATURE_MODE_APP_GET_AMBIENT)
    {
        if(uteModuleTemperatureData.testingSecond>10)
        {
            uteModuleTemperatureStopTesting();
        }
        else
        {
            uteDrvTemperatureCommonEverySecond();
            uteModuleTemperatureData.ambitentTemperature  = uteDrvTemperatureCommonGetAmbientValue();
            uteModuleTemperatureData.skinTemperature = uteDrvTemperatureCommonGetSkinValue();
            uint8_t response[20];
            response[0] = CMD_FACTORY_TEST_MODE;
            response[1] = 0x00;
            response[2] = 0x15;
            response[3] = 0x01;
            response[4] = (uteModuleTemperatureData.ambitentTemperature *100.0f)/100;
            response[5] = (uint16_t)(uteModuleTemperatureData.ambitentTemperature *100.0f)%100;
            response[6] = (uteModuleTemperatureData.skinTemperature *100.0f)/100;;
            response[7] = (uint16_t)(uteModuleTemperatureData.skinTemperature *100.0f)%100;
            if(uteModuleTemperatureData.testingSecond<3)
            {
                uteModuleProfileBleSendToPhone(&response[0],8);
            }
        }
    }
}
/**
*@brief        开始单次测试
*@details
*@author       zn.zeng
*@date       2021-07-16
*/
void uteModuleTemperatureStartTesting(ute_module_temperature_mode_t mode)
{
    if(uteModuleTemperatureData.testMode == mode)
    {
        return ;
    }
    if(mode==TEMPERATURE_MODE_BAND_AUTO)
    {
        uteModuleTemperatureData.isNeedSaveAutoTestData = true;
    }
    uteModuleTemperatureData.testingSecond = 0;
    uteModuleTemperatureData.testMode = mode;
    uteModuleTemperatureData.warningSecond = 0;
    if((mode == TEMPERATURE_MODE_APP_SINGLE) || (mode == TEMPERATURE_MODE_BAND_AUTO)||(mode == TEMPERATURE_MODE_BAND_SINGLE))
    {
        uteModuleTemperatureData.bodyTemperature = 0.0f;
    }
    uteModuleTemperatureData.isHasTemperatureWarning = false;
    if(mode==TEMPERATURE_MODE_AMBIENT_CHECK)
    {
        uteDrvTemperatureCommonPowerOn(true);
    }
    else
    {
        uteDrvTemperatureCommonPowerOn(false);
    }
    clearTemperatureParam();
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"%s,mode=%d",__func__,mode);
}
/**
*@brief        结束单次测试
*@details
*@author       zn.zeng
*@date       2021-07-19
*/
void uteModuleTemperatureStopTesting(void)
{
    if(uteModuleTemperatureData.isNeedSaveAutoTestData)
    {
        uteModuleTemperatureSaveBodyTemperatureData();
    }
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL,"%s",__func__);
    uteModuleTemperatureData.testMode = TEMPERATURE_MODE_NONE;
    uteModuleTemperatureData.testingSecond = 0;
    uteModuleTemperatureData.isNeedSaveAutoTestData = false;
    uteDrvTemperatureCommonPowerOff();
}
/**
*@brief        是否佩戴
*@details
*@author       zn.zeng
*@date       2021-07-19
*/
bool uteModuleTemperatureIsWear(void)
{
    bool isWear = false;
#if UTE_MODULE_HEART_SUPPORT
    isWear = uteModuleHeartIsWear();
#endif
#if UTE_DRV_TEMPERATURE_VITA_HUB_SUPPORT
    if ((uteModuleTemperatureData.testMode != TEMPERATURE_MODE_NONE) && uteModuleBloodpressureVitaGetStartType() != VITA_START_TEMPER_MEASURE)
    {
        isWear = false;
    }
#endif
    return isWear;
}
/**
*@brief        读取当前测试状态
*@details
*@author       zn.zeng
*@date       2021-08-25
*/
bool uteModuleTemperatureIsTesting(void)
{
    bool isTesting = false;
    if(uteModuleTemperatureData.testMode!=TEMPERATURE_MODE_NONE)
    {
        isTesting = true;
    }
    return isTesting;
}
/**
*@brief        读取当前皮肤温度
*@details
*@author       zn.zeng
*@date       2021-12-03
*/

float uteModuleTemperatureGetSkinTemperature(void)
{
    return uteModuleTemperatureData.skinTemperature;
}
/**
*@brief        读取当前环境温度
*@details
*@author       zn.zeng
*@date       2021-12-03
*/

float uteModuleTemperatureGetAmbitentTemperature(void)
{
    return uteModuleTemperatureData.ambitentTemperature;
}
/**
*@brief        读取当前体温
*@details
*@author       zn.zeng
*@date       2021-12-03
*/

float uteModuleTemperatureGetBodyTemperature(void)
{
    return uteModuleTemperatureData.bodyTemperature;
}
/**
*@brief        保存测试参数
*@details
*@author       zn.zeng
*@date       2021-12-04
*/
void uteModuleTemperatureSaveConfig(void)
{
    /*! 保存到文件zn.zeng, 2021-12-04*/
    void *file;
    uint8_t writebuff[19];
    writebuff[0] = uteModuleTemperatureData.isAutoTestOpen;
    writebuff[1] = (uteModuleTemperatureData.autoTestIntervalSecond>>24)&0xff;
    writebuff[2] = (uteModuleTemperatureData.autoTestIntervalSecond>>16)&0xff;
    writebuff[3] = (uteModuleTemperatureData.autoTestIntervalSecond>>8)&0xff;
    writebuff[4] = (uteModuleTemperatureData.autoTestIntervalSecond)&0xff;
    writebuff[5] = uteModuleTemperatureData.isAutoTestLimitTimeOpen;
    writebuff[6] = (uteModuleTemperatureData.autoTestLimitStartTimeSecond>>24)&0xff;
    writebuff[7] = (uteModuleTemperatureData.autoTestLimitStartTimeSecond>>16)&0xff;
    writebuff[8] = (uteModuleTemperatureData.autoTestLimitStartTimeSecond>>8)&0xff;
    writebuff[9] = (uteModuleTemperatureData.autoTestLimitStartTimeSecond)&0xff;
    writebuff[10] = (uteModuleTemperatureData.autoTestLimitStopTimeSecond>>24)&0xff;
    writebuff[11] = (uteModuleTemperatureData.autoTestLimitStopTimeSecond>>16)&0xff;
    writebuff[12] = (uteModuleTemperatureData.autoTestLimitStopTimeSecond>>8)&0xff;
    writebuff[13] = (uteModuleTemperatureData.autoTestLimitStopTimeSecond)&0xff;
    writebuff[14] = uteModuleTemperatureData.isTemperatureWarningOpen;
    writebuff[15] = (uteModuleTemperatureData.warningLowValue>>8)&0xff;
    writebuff[16] = (uteModuleTemperatureData.warningLowValue)&0xff;
    writebuff[17] = (uteModuleTemperatureData.warningHeightValue>>8)&0xff;
    writebuff[18] = (uteModuleTemperatureData.warningHeightValue)&0xff;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_TEMPERATURE_SET_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],19);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,isAutoTestOpen=%d,autoTestIntervalSecond=%d", __func__,uteModuleTemperatureData.isAutoTestOpen,uteModuleTemperatureData.autoTestIntervalSecond);
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,isAutoTestLimitTimeOpen=%d,autoTestLimitStartTimeSecond=%d,autoTestLimitStopTimeSecond=%d", __func__,uteModuleTemperatureData.isAutoTestLimitTimeOpen,uteModuleTemperatureData.autoTestLimitStartTimeSecond,uteModuleTemperatureData.autoTestLimitStopTimeSecond);
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,isTemperatureWarningOpen=%d,warningLowValue=%d,warningHeightValue=%d", __func__,uteModuleTemperatureData.isTemperatureWarningOpen,uteModuleTemperatureData.warningLowValue,uteModuleTemperatureData.warningHeightValue);
}
/**
*@brief        设置自动测试测试间隔时间
*@details
*@param[in] bool isOpen,自动测试是否打开
*@param[in] uint32_t intervalSecond,测试时间间隔，单位为秒
*@author       zn.zeng
*@date       2021-12-03
*/
void uteModuleTemperatureSetAutoTestInterval(bool isOpen,uint32_t intervalSecond)
{
    uteModuleTemperatureData.isAutoTestOpen = isOpen;
    uteModuleTemperatureData.autoTestIntervalSecond = intervalSecond;
    uteModuleTemperatureSaveConfig();
}
/**
*@brief        设置自动测试时间段
*@details
*@param[in] bool isOpen,自动测试时间段是否打开
*@param[in] uint32_t startSecond,测试时间段开始时间，单位为秒
*@param[in] uint32_t stopSecond,测试时间段结束时间，单位为秒
*@author       zn.zeng
*@date       2021-12-03
*/
void uteModuleTemperatureSetAutoTestLimitTime(bool isOpen,uint32_t startSecond,uint32_t stopSecond)
{
    uteModuleTemperatureData.isAutoTestLimitTimeOpen = isOpen;
    uteModuleTemperatureData.autoTestLimitStartTimeSecond = startSecond;
    uteModuleTemperatureData.autoTestLimitStopTimeSecond = stopSecond;
    uteModuleTemperatureSaveConfig();
}
/**
*@brief        设置警告参数
*@details
*@param[in] bool isOpen,是否打开警告
*@param[in] uint16_t low,低温警告值,单位为摄氏度的100倍
*@param[in] uint32_t height,高温警告值,单位为摄氏度的100倍
*@author       zn.zeng
*@date       2021-12-03
*/
void uteModuleTemperatureSetWarningParam(bool isOpen,uint16_t low,uint16_t height)
{
    uteModuleTemperatureData.isTemperatureWarningOpen = isOpen;
    uteModuleTemperatureData.warningLowValue = low;
    uteModuleTemperatureData.warningHeightValue = height;
    uteModuleTemperatureSaveConfig();
}

/**
*@brief        保存自动测试体温数据
*@details      每天保存一个文件，10分钟内只有一条数据
*@author       zn.zeng
*@date       2021-12-04
*/
void uteModuleTemperatureSaveBodyTemperatureData(void)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = 12*24+4;
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
    uint8_t path[25];
    memset(&readBuff[0],0xff,buffSize);
    memset(&path[0],0,25);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0],"%04d%02d%02d",time.year,time.month,time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR, dirInfo, NULL);
    if((dirInfo->filesCnt>=UTE_MODULE_TEMPERATURE_SAVE_DATA_MAX_DAYS)&&(memcmp(&path[0],&dirInfo->filesName[0][0],8)!=0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-12-04*/
        memset(&path[0],0,25);
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR,&dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile(&path[0]);
    }
    memset(&path[0],0,25);
    sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR,time.year,time.month,time.day);
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,save file=%s", __func__,&path[0]);
    /*! 保存当前数据zn.zeng, 2021-12-04  */
    void *file;
    // read
    if(uteModuleFilesystemOpenFile(&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    else
    {
        readBuff[0] = time.year>>8&0xff;
        readBuff[1] = time.year&0xff;
        readBuff[2] = time.month;
        readBuff[3] = time.day;
    }
    if(uteModuleFilesystemOpenFile(&path[0],&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uint16_t cacheOffset = 0;
        uint16_t bodyTemp = (uint16_t)(uteModuleTemperatureData.bodyTemperature*100.0f);
        cacheOffset = 4+2*((time.hour*3600+time.min*60+time.sec)/600);//每10分钟一个数据
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,cacheOffset=%d", __func__,cacheOffset);
        readBuff[cacheOffset] = (bodyTemp>>8)&0xff;
        readBuff[cacheOffset+1] = bodyTemp&0xff;
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(readBuff);
    uteModulePlatformMemoryFree(dirInfo);
}
/**
*@brief        开始发送体温数据
*@details
*@author       zn.zeng
*@date       2021-12-04
*/
void uteModuleTemperatureSendHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[25];
    uint8_t response[20];
    uint8_t sendSize = 0;
    uint8_t i = 0;
    response[0] = CMD_TEMPERATURE_HEAD;
    response[1] = 0xfa;
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&dataBuff[0],0,sendParam->dataBuffSize);
    memset(&path[0],0,25);
    uint8_t *tempDataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&tempDataBuff[0],0,sendParam->dataBuffSize);
    if(sendParam->dirInfo.filesCnt>0)
    {
        sendSize = 19;
        void *file;
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR,&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,read file=%s", __func__,&path[0]);
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex);
        if(uteModuleFilesystemOpenFile(&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
            memcpy(&dataBuff[0], tempDataBuff, 4);      /*!获取年月日 , xjc 2022-03-03*/
            memcpy(&dataBuff[4], &tempDataBuff[4], 144*2); /*!获取当天00：10~23：50的数据, xjc 2022-03-03*/
            uteModuleFilesystemCloseFile(file);
        }
//        if ((sendParam->currSendFileIndex + 1) < sendParam->dirInfo.filesCnt)
//        {
//            memset(&tempDataBuff[0],0,sendParam->dataBuffSize);
//            sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex + 1][0]);
//            if (uteModuleFilesystemOpenFile(&path[0], &file, FS_O_RDONLY))
//            {
//                uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
//                memcpy(&dataBuff[sendParam->dataBuffSize - 2], &tempDataBuff[4], 2); /*!获取后一天00：00的数据, xjc 2022-03-03*/
//                uteModuleFilesystemCloseFile(file);
//            }
//        }
        for(i=sendParam->currSendHourIndex; i<24; i++)
        {
            if(memcmp(&dataBuff[4+12*i],"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",12)!=0)
            {
                uint8_t sendHour = i;
                if(sendHour==24)
                {
                    ute_module_systemtime_time_t timeAdd;
                    timeAdd.year = dataBuff[0]<<8|dataBuff[1];
                    timeAdd.month = dataBuff[2];
                    timeAdd.day = dataBuff[3];
                    uteModuleSystemtimeInputDateCalDay(&timeAdd,1);
                    dataBuff[0] = (timeAdd.year>>8)&0xff;
                    dataBuff[1] = (timeAdd.year)&0xff;
                    dataBuff[2] = timeAdd.month;
                    dataBuff[3] = timeAdd.day;
                    sendHour = 0;
                }
                memcpy(&response[2],&dataBuff[0],4);//year month day
                response[6] = sendHour;
                sendParam->currSendHourIndex=i+1;
                memcpy(&response[7],&dataBuff[4+12*i],12);
                break;
            }
        }
        if(i==24)
        {
            sendParam->currSendHourIndex = 24;
            sendSize = 0;
        }
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,find currSendHourIndex=%d", __func__,sendParam->currSendHourIndex);

        if(sendParam->currSendHourIndex>=24)
        {
            sendParam->currSendFileIndex++;
            if(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt)
            {
                sendParam->currSendHourIndex = 0;
            }
            else
            {
                sendParam->currSendHourIndex=24;
            }
        }
    }
    else
    {
        sendParam->currSendHourIndex=24;
    }
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex,sendParam->dirInfo.filesCnt);
    if((sendParam->currSendFileIndex==sendParam->dirInfo.filesCnt)
       &&(sendParam->currSendHourIndex==24))
    {
        sendSize = 3;
        response[1] = 0xfd;
        response[2] = sendParam->crc;
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s", __func__);
        uteApplicationCommonSyncDataTimerStop();
    }
    uteModulePlatformMemoryFree(tempDataBuff);
    if(sendSize==19)
    {
        uteModuleCrc8Bit(&sendParam->crc, &response[2],17);
    }
    if(sendSize!=0)
    {
        uteModuleProfileBleSendToPhone(&response[0],sendSize);
    }
    uteModulePlatformMemoryFree(dataBuff);
}

/**
*@brief        准备开始发送体温数据
*@details
*@param[in]  需要同步的数据时间戳
*@author       zn.zeng
*@date       2021-12-04
*/
void uteModuleTemperatureStartSendHistoryData(ute_module_systemtime_time_t time)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->time = time;
    if(param->time.year==0)
    {
        param->isAllData = true;
    }
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    param->currSendMinIndex = 0;
    param->dataBuffSize = 12*24+4;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleTemperatureSendHistoryData);
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s", __func__);
    uteApplicationCommonSyncDataTimerStart();
}
/**
*@brief        删除体温率测试数据
*@details
*@author       zn.zeng
*@date       2021-12-04
*/
void uteModuleTemperatureDelHistoryData(void)
{
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s", __func__);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR,dirInfo, NULL);
    for(uint8_t i=0; i<dirInfo->filesCnt; i++)
    {
        uint8_t path[30];
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_TEMPERATURE_DIR,&dirInfo->filesName[i][0]);
        UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile(&path[0]);
    }
    uteModulePlatformMemoryFree(dirInfo);
}
/**
*@brief        获取当前测试模式
*@details
*@author       zn.zeng
*@date       2022-01-03
*/
ute_module_temperature_mode_t uteModuleTemperatureGetTestMode(void)
{
    return uteModuleTemperatureData.testMode;
}


#else
/**
*@brief        没有体温功能，但是有板子上有体温器件，需要配置体温管脚,否则会漏电
*@details
*@author       xjc
*@date       2021-12-31
*/
void uteModuleTemperatureIoInit(void)
{
    UTE_MODULE_LOG(UTE_LOG_TEMPERATURE_LVL, "%s", __func__);
#ifdef UTE_MODULE_TEMPERATURE_POWER_GPIO_PIN
    uteModulePlatformOutputGpioSet(UTE_MODULE_TEMPERATURE_POWER_GPIO_PIN,!UTE_MODULE_TEMPERATURE_POWER_ACTION_LEVEL);
#endif
}
#endif

