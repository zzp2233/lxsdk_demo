/**
*@file
*@brief     血氧模块
*@details
*@author        zn.zeng
*@date        2021-07-21
*@version       v1.0
*/
#include "ute_module_heart.h"
#include "ute_module_systemtime.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_protocol.h"
#include "ute_module_crc.h"
#include "ute_drv_motor.h"
#include "ute_module_sport.h"
#include "ute_drv_battery_common.h"
#include "ute_module_factoryTest.h"
// #include "ute_drv_heart_vcxx_common.h"
// #include "ute_module_emotionPressure.h"

#if UTE_MODULE_BLOODOXYGEN_SUPPORT
/*! 血压数据结构zn.zeng, 2021-07-23  */
ute_module_bloodoxygen_data_t uteModuleBloodoxygenData;
#if DUG_VCXX_BLOOD_OXYGEN_SUPPORT
// #undef UTE_MODULE_BLOODOXYGEN_TEST_TIMEOUT_SECOND
// #define UTE_MODULE_BLOODOXYGEN_TEST_TIMEOUT_SECOND 0xFFFE
#endif
/**
*@brief        初始化函数
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, NULL, NULL);
#endif
    memset(&uteModuleBloodoxygenData,0,sizeof(ute_module_bloodoxygen_data_t));
    uteModuleSystemtimeRegisterSecond(uteModuleBloodoxygenEverySecond);
}
/**
*@brief        读取config
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[8];
    readbuff[0] = UTE_MODULE_BLOODOXYGEN_IS_AUTO_OPEN;
    readbuff[1] = UTE_MODULE_BLOODOXYGEN_IS_AUTO_TIME_BUCKET_OPEN;
    readbuff[2] = (UTE_MODULE_BLOODOXYGEN_AUTO_INTERVAL_MIN>>8)&0xff;
    readbuff[3] = (UTE_MODULE_BLOODOXYGEN_AUTO_INTERVAL_MIN)&0xff;
    readbuff[4] = (UTE_MODULE_BLOODOXYGEN_TIME_BUCKET_START>>8)&0xff;
    readbuff[5] = (UTE_MODULE_BLOODOXYGEN_TIME_BUCKET_START)&0xff;
    readbuff[6] = (UTE_MODULE_BLOODOXYGEN_TIME_BUCKET_END>>8)&0xff;
    readbuff[7] = (UTE_MODULE_BLOODOXYGEN_TIME_BUCKET_END)&0xff;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SET_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],8);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleBloodoxygenData.isAutoTesting=readbuff[0];
    uteModuleBloodoxygenData.isAutoTimeBucketTesting=readbuff[1];
    uteModuleBloodoxygenData.intervalMin=readbuff[2]<<8|readbuff[3];
    uteModuleBloodoxygenData.startTimeHourMin=readbuff[4]<<8|readbuff[5];
    uteModuleBloodoxygenData.endTimeHourMin=readbuff[6]<<8|readbuff[7];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isAutoTesting=%d,intervalMin=%d", __func__,uteModuleBloodoxygenData.isAutoTesting,uteModuleBloodoxygenData.intervalMin);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__,uteModuleBloodoxygenData.startTimeHourMin,uteModuleBloodoxygenData.endTimeHourMin);

}

/**
*@brief        血氧每秒函数
*@details       需要注册到主时间，每秒执行一次
*@author       zn.zeng
*@date       2021-07-23
*/
void uteModuleBloodoxygenEverySecond(void)
{
    if(!uteApplicationCommonIsPowerOn())
    {
        return;
    }

    static bool isNeedAutoTest = false;

    if(uteModuleBloodoxygenData.isSingleTesting)
    {
        uteModuleBloodoxygenData.value = uteModuleHeartGetBloodOxygenValue();
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s,.value=%d",__func__,uteModuleBloodoxygenData.value);
        if(uteModuleBloodoxygenIsWear() && (uteModuleHeartGetWorkMode() == WORK_MODE_SPO2) && vc30fx_usr_get_work_status())
        {
#if !DUG_VCXX_BLOOD_OXYGEN_SUPPORT
            UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s,testingSecond:%d",__func__,uteModuleBloodoxygenData.testingSecond);
            if(uteModuleBloodoxygenData.testingSecond > UTE_MODULE_BLOODOXYGEN_TEST_TIMEOUT_SECOND)
            {
                if(!uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                }
                uteModuleBloodoxygenStopSingleTesting();
            }
            else
            {
                if (uteModuleBloodoxygenData.testingSecond > UTE_MODULE_BLOODOXYGEN_TEST_DEFAULT_SECOND &&
                    uteModuleBloodoxygenData.value > 0 && uteModuleBloodoxygenData.value < 0xff)
                {
                    if(!uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
                    {
                        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    }
                    uteModuleBloodoxygenStopSingleTesting();
                }
                uteModuleBloodoxygenData.testingSecond++;
            }
#endif
        }
        else
        {
            if(uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
            {
                isNeedAutoTest = false;
            }
            uteModuleBloodoxygenStopSingleTesting();
        }
    }

    /*! 自动测试逻辑 zn.zeng, 2021-08-31  */
    if(uteModuleBloodoxygenData.isAutoTesting)
    {
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        uint32_t oneDaySec = time.hour*3600+time.min*60+time.sec;
        if(uteModuleBloodoxygenData.isAutoTimeBucketTesting)
        {
            if(((oneDaySec%(60*uteModuleBloodoxygenData.intervalMin))==0) && !isNeedAutoTest)
            {
                uint16_t currentTime = ((time.hour<<8&0xff00)|time.min);
                if(uteModuleBloodoxygenData.endTimeHourMin>uteModuleBloodoxygenData.startTimeHourMin)
                {
                    if((currentTime>=uteModuleBloodoxygenData.startTimeHourMin)&&(currentTime<=uteModuleBloodoxygenData.endTimeHourMin))
                    {
                        isNeedAutoTest = true;
                    }
                }
                else
                {
                    if((currentTime>=uteModuleBloodoxygenData.startTimeHourMin)||(currentTime<=uteModuleBloodoxygenData.endTimeHourMin))
                    {
                        isNeedAutoTest = true;
                    }
                }
            }
        }
        else
        {
            if((oneDaySec%(60*uteModuleBloodoxygenData.intervalMin))==0 && !isNeedAutoTest)
            {
                isNeedAutoTest = true;
            }
        }
        if (//uteModuleBloodoxygenIsWear() &&
            (uteDrvBatteryCommonGetChargerStatus() == BAT_STATUS_NO_CHARGE) &&
            (!uteModuleSportMoreSportIsRuning()) &&
            (uteApplicationCommonGetSystemPowerOnSecond() > 5) &&
            (uteModuleFactoryTestGetCurrTestItem() == TEST_ITEM_NONE) &&
            // (!uteModuleBreathrateIsTesting()) &&
            // (!uteModuleGetBreathTrainingStatus()) &&
            (!uteModuleHeartIsSingleTesting())
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
            && (!uteModuleEmotionPressureIsTesting())
#endif
        )
        {
            if(isNeedAutoTest && !uteModuleBloodoxygenData.isSingleTesting)
            {
                uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag = true;
                uteModuleBloodoxygenStartSingleTesting();
            }
            else
            {
                isNeedAutoTest = false;
            }
        }
    }
}
/**
*@brief        开始单次测试
*@details
*@author       zn.zeng
*@date       2021-07-21
*/
void uteModuleBloodoxygenStartSingleTesting(void)
{
    if(vc30fx_usr_get_work_status() && uteModuleHeartGetWorkMode() == WORK_MODE_SPO2)
    {
        uteModuleBloodoxygenData.isSingleTesting = true;
        return;
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s",__func__);
    uteModuleBloodoxygenData.isSingleTesting = true;
    uteModuleBloodoxygenData.value = 0;
    uteModuleBloodoxygenData.testingSecond = 0;
    if (!uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
    {
        uint8_t startCmdRsp[2] = {CMD_SPO2_TEST,0x11};
        uteModuleProfileBleSendToPhone(&startCmdRsp[0],2);
    }
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartStartSingleTesting(TYPE_BLOODOXYGEN);
#endif
}
/**
*@brief        结束单次测试
*@details
*@author       zn.zeng
*@date       2021-07-21
*/
void uteModuleBloodoxygenStopSingleTesting(void)
{
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s",__func__);
    if(uteModuleHeartGetWorkMode() != WORK_MODE_SPO2 || !vc30fx_usr_get_work_status())
    {
        uteModuleBloodoxygenData.isSingleTesting = false;
        return;
    }
    uint8_t stopCmd[5];
    stopCmd[0] =CMD_SPO2_TEST;
    stopCmd[1] =0x00;
    uteModuleBloodoxygenData.value = uteModuleHeartGetBloodOxygenValue();
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s,.value=%d",__func__,uteModuleBloodoxygenData.value);
    if(uteModuleBloodoxygenData.value > 0)
    {
        stopCmd[2] =0x00;
        stopCmd[3] =uteModuleBloodoxygenData.value ;
#if UTE_MODULE_BLOODGXYGEN_REAL_TIME_MAX_AND_MIN_VAULE_SUPPORT
        uteModuleBloodoxygenData.lastValue = uteModuleBloodoxygenData.value;
        if (uteModuleBloodoxygenData.dayMinValue == 0)
        {
            uteModuleBloodoxygenData.dayMinValue = 100;
        }
        if (uteModuleBloodoxygenData.lastValue > uteModuleBloodoxygenData.dayMaxValue)
        {
            uteModuleBloodoxygenData.dayMaxValue = uteModuleBloodoxygenData.lastValue;
        }
        if (uteModuleBloodoxygenData.lastValue < uteModuleBloodoxygenData.dayMinValue)
        {
            uteModuleBloodoxygenData.dayMinValue = uteModuleBloodoxygenData.lastValue;
        }
#endif
    }
    else
    {
        stopCmd[2] =0xff;
        stopCmd[3] =0xff;
        uteModuleBloodoxygenData.value = 0xff;
    }
    if (!uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
    {
        uteModuleProfileBleSendToPhone(&stopCmd[0],4);
    }
    /*! 保存数据zn.zeng, 2021-07-23  */
    uteModuleBloodoxygenAutoSaveOxygenData();
    uteModuleBloodoxygenData.isSingleTesting = false;
    uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag = false;
    uteModuleBloodoxygenData.testingSecond = 0;
#if UTE_MODULE_HEART_SUPPORT
    uteModuleHeartStopSingleTesting(TYPE_BLOODOXYGEN);
#endif
}
/**
*@brief        是否佩戴
*@details
*@author       zn.zeng
*@date       2021-07-21
*/
bool uteModuleBloodoxygenIsWear(void)
{
    bool isWear = false;
    isWear = uteModuleHeartIsWear();
    return isWear;
}
/**
*@brief        获取血氧值
*@details
*@author       zn.zeng
*@date       2021-08-05
*/
uint8_t uteModuleBloodoxygenGetValue(void)
{
    return uteModuleBloodoxygenData.value;
}

/**
 * @brief        设置血氧值
 * @details
 * @param[in]    value 血氧值
 * @author       Wang.Luo
 * @date         2022-11-29
 */
void uteModuleBloodoxygenSetValue(uint8_t value)
{
    uteModuleBloodoxygenData.value = value;
}

/**
*@brief        获取血氧测试状态
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
bool uteModuleBloodoxygenIsTesting(void)
{
    return uteModuleBloodoxygenData.isSingleTesting;
}
/**
*@brief        保存血氧设置参数
*@details
*@param[in] bool isAutoTesting  是否打开自动测试
*@param[in] uint16_t intervalMin  测试间隔，单位为分钟
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenSaveAutoIntervalParam(bool isAutoTesting,uint16_t intervalMin)
{
    uteModuleBloodoxygenData.isAutoTesting = isAutoTesting;
    uteModuleBloodoxygenData.intervalMin = intervalMin;
    /*! 保存到文件zn.zeng, 2021-08-30*/
    void *file;
    uint8_t writebuff[8];
    writebuff[0] = uteModuleBloodoxygenData.isAutoTesting;
    writebuff[1] = uteModuleBloodoxygenData.isAutoTimeBucketTesting;
    writebuff[2] = (uteModuleBloodoxygenData.intervalMin>>8)&0xff;
    writebuff[3] = (uteModuleBloodoxygenData.intervalMin)&0xff;
    writebuff[4] = (uteModuleBloodoxygenData.startTimeHourMin>>8)&0xff;
    writebuff[5] = (uteModuleBloodoxygenData.startTimeHourMin)&0xff;
    writebuff[6] = (uteModuleBloodoxygenData.endTimeHourMin>>8)&0xff;
    writebuff[7] = (uteModuleBloodoxygenData.endTimeHourMin)&0xff;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SET_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],8);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,isAutoTesting=%d,intervalMin=%d,isAutoTimeBucketTesting=%d", __func__,uteModuleBloodoxygenData.isAutoTesting,uteModuleBloodoxygenData.intervalMin,uteModuleBloodoxygenData.isAutoTimeBucketTesting);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__,uteModuleBloodoxygenData.startTimeHourMin,uteModuleBloodoxygenData.endTimeHourMin);

}
/**
*@brief        保存血氧设置参数
*@details
*@param[in] bool isAutoTimeBucketTesting  是否打开时间段自动测试
*@param[in] uint16_t startTimeHourMin 开始时间，高8bit为小时，低8bit为分钟
*@param[in] uint16_t endTimeHourMin 结束时间，高8bit为小时，低8bit为分钟
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenSaveAutoTimeBucketParam(bool isAutoTimeBucketTesting,uint16_t startTimeHourMin,uint16_t endTimeHourMin)
{
    uteModuleBloodoxygenData.isAutoTimeBucketTesting = isAutoTimeBucketTesting;
    uteModuleBloodoxygenData.startTimeHourMin = startTimeHourMin;
    uteModuleBloodoxygenData.endTimeHourMin = endTimeHourMin;
    /*! 保存到文件zn.zeng, 2021-08-30*/
    void *file;
    uint8_t writebuff[8];
    writebuff[0] = uteModuleBloodoxygenData.isAutoTesting;
    writebuff[1] = uteModuleBloodoxygenData.isAutoTimeBucketTesting;
    writebuff[2] = (uteModuleBloodoxygenData.intervalMin>>8)&0xff;
    writebuff[3] = (uteModuleBloodoxygenData.intervalMin)&0xff;
    writebuff[4] = (uteModuleBloodoxygenData.startTimeHourMin>>8)&0xff;
    writebuff[5] = (uteModuleBloodoxygenData.startTimeHourMin)&0xff;
    writebuff[6] = (uteModuleBloodoxygenData.endTimeHourMin>>8)&0xff;
    writebuff[7] = (uteModuleBloodoxygenData.endTimeHourMin)&0xff;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SET_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],8);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,isAutoTesting=%d,intervalMin=%d,isAutoTimeBucketTesting=%d", __func__,uteModuleBloodoxygenData.isAutoTesting,uteModuleBloodoxygenData.intervalMin,uteModuleBloodoxygenData.isAutoTimeBucketTesting);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__,uteModuleBloodoxygenData.startTimeHourMin,uteModuleBloodoxygenData.endTimeHourMin);
}
/**
*@brief        保存血氧数据数据，每10分钟一条数据
*@details      每天保存一个文件，自动保存
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenAutoSaveOxygenData(void)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = 6*24+4;
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
    uint8_t path[42];
    memset(&readBuff[0],0xff,buffSize);
    memset(&path[0],0,42);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0],"%04d%02d%02d",time.year,time.month,time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, dirInfo, NULL);
    if((dirInfo->filesCnt>=UTE_MODULE_BLOODOXYGEN_SAVE_DATA_MAX_DAYS)&&(memcmp(&path[0],&dirInfo->filesName[0][0],8)!=0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-08-25*/
        memset(&path[0],0,42);
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,&dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0],0,42);
    sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,time.year,time.month,time.day);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,save file=%s", __func__,&path[0]);
    /*! 保存当前数据zn.zeng, 2021-08-30  */
    void *file;
    // read
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
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
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uint16_t cacheOffset = 0;
        cacheOffset = 4+(time.hour*60+time.min)/10;
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,cacheOffset=%d , uteModuleBloodoxygenData.value = %d", __func__,cacheOffset,uteModuleBloodoxygenData.value);
        if(uteModuleBloodoxygenData.value!=0xFF && uteModuleBloodoxygenData.value!=0x00)
        {
            readBuff[cacheOffset] = uteModuleBloodoxygenData.value;
        }
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(readBuff);
    uteModulePlatformMemoryFree(dirInfo);
}
/**
*@brief        开始发送24小时血氧数据
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenSendHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[42];
    uint8_t response[20];
    uint8_t sendSize = 0;
    response[0] = CMD_SPO2_TEST;
    response[1] = 0xfa;
    memset(&path[0],0,42);
    uint8_t *tempDataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&tempDataBuff[0],0,sendParam->dataBuffSize);
    memset(&dataBuff[0],0,sendParam->dataBuffSize);
    if(sendParam->dirInfo.filesCnt>0)
    {
    SEND_NEXT_DATA:
        sendSize = 20;
        bool isHasData = false;
        void *file;
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,read file=%s", __func__,&path[0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,currSendFileIndex=%d,currSendMinIndex=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendMinIndex);
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
            memcpy(&dataBuff[0], tempDataBuff, 4);      /*!获取年月日 , xjc 2022-03-03*/
            memcpy(&dataBuff[4], &tempDataBuff[5], 143); /*!获取当天00：10~23：50的数据, xjc 2022-03-03*/
            uteModuleFilesystemCloseFile(file);
        }
        if ((sendParam->currSendFileIndex + 1) < sendParam->dirInfo.filesCnt)
        {
            memset(&tempDataBuff[0],0,sendParam->dataBuffSize);
            sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex + 1][0]);
            if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
            {
                uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
                memcpy(&dataBuff[sendParam->dataBuffSize - 1], &tempDataBuff[4], 1); /*!获取后一天00：00的数据, xjc 2022-03-03*/
                uteModuleFilesystemCloseFile(file);
            }
        }
        uint16_t invalidMin = 0;
        for(uint16_t i=sendParam->currSendMinIndex; i<144;)
        {
            if((memcmp(&dataBuff[4+i],"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",12)==0)||(memcmp(&dataBuff[4+i],"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",12)==0))
            {
                invalidMin=invalidMin+12;
                i = i+12;
            }
            else
            {
                uint8_t sendHour = i/6+2;
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
                response[7] = 0;
                memcpy(&response[8],&dataBuff[4+i],12);
                invalidMin=invalidMin+12;
                i = i+12;
                isHasData = true;
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,invalidMin=%d", __func__,invalidMin);
        sendParam->currSendMinIndex+=invalidMin;
        if(sendParam->currSendMinIndex>=144)
        {
            sendParam->currSendFileIndex++;
            if(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt)
            {
                sendParam->currSendMinIndex = 0;
            }
            else
            {
                sendParam->currSendMinIndex=144;
            }
        }
        if((!isHasData)&&(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt))
        {
            goto SEND_NEXT_DATA;
        }
    }
    else
    {
        sendParam->currSendMinIndex=144;
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex,sendParam->dirInfo.filesCnt);
    if((sendParam->currSendFileIndex==sendParam->dirInfo.filesCnt)
       &&(sendParam->currSendMinIndex==144))
    {
        sendSize = 4;
        response[1] = 0xfa;
        response[2] = 0xfd;
        response[3] = sendParam->crc;
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,stop", __func__);
        uteApplicationCommonSyncDataTimerStop();
    }
    if(sendSize==20)
    {
        uteModuleCrc8Bit(&sendParam->crc, &response[2],18);
    }
    uteModulePlatformMemoryFree(tempDataBuff);
    uteModulePlatformMemoryFree(dataBuff);
    uteModuleProfileBleSendToPhone(&response[0],sendSize);
}
/**
*@brief        准备开始发送血氧数据
*@details
*@param[in]  需要同步的数据时间戳
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenStartSendAutoTestHistoryData(ute_module_systemtime_time_t time)
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
    param->dataBuffSize = 6*24+4;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleBloodoxygenSendHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s", __func__);

}
/**
*@brief        删除血氧数据
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenDelHistoryData(void)
{
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s", __func__);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,dirInfo, NULL);
    for(uint8_t i=0; i<dirInfo->filesCnt; i++)
    {
        uint8_t path[40];
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,&dirInfo->filesName[i][0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    uteModulePlatformMemoryFree(dirInfo);
}

/**
*@brief        是否正在测量血氧
*@details
*@author       xjc
*@date       2022-01-18
*/
AT(.com_text.ute_spo2)
bool uteModuleBloodoxygenIsBloodOxygenAutoTesting(void)
{
    return uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag;
}

void uteModuleBloodoxygenSetBloodOxygenAutoTesting(bool isAutoTest)
{
    uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag = isAutoTest;
}

/**
*@brief        是否正在测量血氧
*@details
*@author       xjc
*@date       2022-01-18
*/
uint8_t uteModuleBloodoxygenGetTestingSecond(void)
{
    return uteModuleBloodoxygenData.testingSecond;
}

#if UTE_DISPLAY_BLOODGXYGEN_HISTORY_HISTOGRAM_SUPPORT

/*! 血氧画图的最小值，例如：要画的图表范围从85~100，则MINIMUM=85,wang.luo 2023-03-15 */
#ifndef UTE_DISPLAY_BLOODGXYGEN_MINIMUM
#define UTE_DISPLAY_BLOODGXYGEN_MINIMUM 0
#endif

/**
*@brief 设置血氧柱状图具体的坐标
*@details
*@author       hcj
*@date       2022-04-19
*/
bool uteModuleBloodoxygenSetBloodoxygenHistoryGraph(UT_GraphsParam *BloodoxygenHistoryGraph, uint8_t BloodoxygenHistoryGraphCount,uint8_t *BloodoxygenHistoryData,uint8_t BloodoxygenHistoryDataLen,int16_t x,int16_t y,uint8_t drawWidth,uint8_t intervalWidth,uint16_t hightRange)
{
    uint8_t BloodoxygenData = 0;
    uint8_t rangeIndex = 0;
    if(BloodoxygenHistoryGraphCount>BloodoxygenHistoryDataLen)
    {
        return false;
    }
#if UTE_LOG_GUI_LVL // test
    for(uint8_t i=0; i<144; i++)
    {
        BloodoxygenHistoryData[i] = 80+rand()%20;
    }
#endif
    uint8_t tempBloodxygenHistoryData[144];
    uint8_t m = 144/BloodoxygenHistoryGraphCount;
    memset(tempBloodxygenHistoryData, 0, sizeof(tempBloodxygenHistoryData));
#if !UTE_MODULE_BLOODGXYGEN_REAL_TIME_MAX_AND_MIN_VAULE_SUPPORT
    uteModuleBloodoxygenData.dayMaxValue = 0;
    uteModuleBloodoxygenData.dayMinValue = 100;
#endif
    uint8_t dayMaxValue = 0;
    uint8_t dayMinValue = 100;
    for (uint8_t i = 0; i < BloodoxygenHistoryGraphCount; i++)
    {
        uint8_t oxygenValue = 0;
        uint16_t oxygenValidValue = 0; //总心率值
        uint8_t averageOxygenValueCnt = 0;
        for (int j = m; j > 0; j--)
        {
            oxygenValue = BloodoxygenHistoryData[i * m + (j - 1)];
            if ((oxygenValue <= 100) && (oxygenValue >= 60))
            {
                oxygenValidValue += oxygenValue;
                averageOxygenValueCnt++;
            }
        }
        tempBloodxygenHistoryData[i] = oxygenValidValue/averageOxygenValueCnt;
    }
    memcpy(BloodoxygenHistoryData, tempBloodxygenHistoryData, BloodoxygenHistoryGraphCount);
    for(uint8_t i = 0; i<BloodoxygenHistoryGraphCount; i++)
    {
        if(BloodoxygenHistoryData[i]==0xFF)  BloodoxygenHistoryData[i]=0;

        if (BloodoxygenHistoryData[i] != 0)
        {
            if (BloodoxygenHistoryData[i] > dayMaxValue)
            {
                dayMaxValue = BloodoxygenHistoryData[i];
            }
            if (BloodoxygenHistoryData[i] < dayMinValue)
            {
                dayMinValue = BloodoxygenHistoryData[i];
            }
        }
        BloodoxygenData = BloodoxygenHistoryData[i] > UTE_DISPLAY_BLOODGXYGEN_MINIMUM ? BloodoxygenHistoryData[i] - UTE_DISPLAY_BLOODGXYGEN_MINIMUM : 0;
        BloodoxygenHistoryGraph[i].colorData = BLLODOXYGEN_LANGUAGE_24COLOR;

        //宽高通过图片的对应比例算出.比如下面高度一共hightRange个像素点，心率范围值取0~200
        BloodoxygenHistoryGraph[i].hight = BloodoxygenData*hightRange/(100-UTE_DISPLAY_BLOODGXYGEN_MINIMUM);
        BloodoxygenHistoryGraph[i].width = drawWidth;
        BloodoxygenHistoryGraph[i].x = x+i*drawWidth+i*intervalWidth;
        BloodoxygenHistoryGraph[i].y = y - BloodoxygenHistoryGraph[i].hight;
    }
#if UTE_MODULE_BLOODGXYGEN_REAL_TIME_MAX_AND_MIN_VAULE_SUPPORT
    if (uteModuleBloodoxygenData.dayMaxValue == 0 && dayMaxValue != 0)
#endif
    {
        uteModuleBloodoxygenData.dayMinValue = dayMinValue;
        uteModuleBloodoxygenData.dayMaxValue = dayMaxValue;
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,dayMaxValue = %d ,dayMinValue = %d", __func__,uteModuleBloodoxygenData.dayMaxValue,uteModuleBloodoxygenData.dayMinValue);
    return true;
}
/**
*@brief        获取当天最大最小血氧值
*@details
*@author       zn.zeng
*@date       2022-09-03
*/
void uteModuleBloodoxygenGetMinMaxValue(uint8_t *min,uint8_t *max)
{
    *min = uteModuleBloodoxygenData.dayMinValue;
    *max = uteModuleBloodoxygenData.dayMaxValue;
}
/**
*@brief      获取当天血氧历史数据，用于血氧柱状图显示
*@details    当天有数据返回true,没有则返回false
*@author     xjc
*@date       2021-12-22
*/
bool uteModuleBloodoxygenGetTodayHistoryData(UT_GraphsParam *BloodoxygenHistoryGraph, uint8_t BloodoxygenHistoryGraphCount, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange)
{
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s", __func__);
    void *file;
    uint8_t path[40];
    uint8_t BloodoxygenHistoryData[144]; //一天24h的数据，十分钟一条，一共144条
    uint8_t tempBloodoxygenHistoryData[6 * 24 + 4];
    uint16_t buffSize = 6 * 24 + 4;
    ute_module_systemtime_time_t time;
    memset(BloodoxygenHistoryData, 0, 144);
    memset(BloodoxygenHistoryGraph,0, sizeof(UT_GraphsParam)*BloodoxygenHistoryGraphCount);
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,read file=%s", __func__, &path[0]);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file, tempBloodoxygenHistoryData, buffSize);
        uteModuleFilesystemCloseFile(file);
        memcpy(BloodoxygenHistoryData, &tempBloodoxygenHistoryData[4], 144);
        uteModuleBloodoxygenSetBloodoxygenHistoryGraph(BloodoxygenHistoryGraph, BloodoxygenHistoryGraphCount, BloodoxygenHistoryData, 144, x, y, drawWidth, intervalWidth, hightRange);
        return true;
    }
    else
    {
#if UTE_LOG_GUI_LVL // test 
        uteModuleBloodoxygenSetBloodoxygenHistoryGraph(BloodoxygenHistoryGraph, BloodoxygenHistoryGraphCount, BloodoxygenHistoryData, 144, x, y, drawWidth, intervalWidth, hightRange);
#endif
        return false;
    }
}
#endif

#endif //UTE_MODULE_BLOODOXYGEN_SUPPORT
