/**
 *@file
 *@brief        睡眠
 *@details
 *@author       zn.zeng
 *@date       2021-10-25
 *@version      v1.0
 */

#include "ute_module_sleep.h"
#include "ute_module_log.h"
#include "ute_module_heart.h"
#include "ute_module_filesystem.h"
#include "ute_application_common.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_flash_map_common.h"
#include "ute_module_systemtime.h"
#include "ute_module_platform.h"

#if UTE_MODULE_SLEEP
/*! 睡眠算法数据zn.zeng, 2021-10-26  */
ute_sleep_data_t uteSleepAlgoData;
ute_module_sleep_data_t uteModuleSleepData;
/* 睡眠互斥量 zn.zeng 2022-02-14*/
void *uteModuleSleepMute;

/**
 *@brief       睡眠初始化
 *@details
 *@author        zn.zeng
 *@date        2021-10-26
 */
void uteModuleSleepInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_SLEEP_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SLEEP_DIR, NULL, NULL);
#endif
    uteModulePlatformCreateMutex(&uteModuleSleepMute);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCWMSleepInit();
#endif
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    memset(&uteModuleSleepData, 0, sizeof(ute_module_sleep_data_t));
    memset(&uteSleepAlgoData, 0, sizeof(ute_sleep_data_t));
    uteSleepAlgoData.maxRecord = UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE;
    uteSleepAlgoData.saveData.recordIndex = 0;
    uteSleepAlgoData.saveData.year = time.year;
    uteSleepAlgoData.saveData.mon = time.month;
    uteSleepAlgoData.saveData.day = time.day;
    uteSleepOnBandSetPoint(&uteSleepAlgoData);
    void *file;
    uint8_t path[20];
    memset(&path[0], 0, 20);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_SLEEP_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read file=%s", __func__, &path[0]);
    uint32_t currTimeMinSec = time.hour*3600+time.min*60+time.sec;
    bool isNotGetUp = false;
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))  //读取数据
    {
        if ((currTimeMinSec >= (18 * 3600)) || (currTimeMinSec < (6 * 3600))) //>=18:00  <6:00 // 非起床状态
        {
            isNotGetUp = true;
            UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,not get up", __func__);
        }
        else// 已经是起床状态
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &(uteSleepAlgoData.saveData), sizeof(ute_sleep_data_save_t));
            if (uteSleepAlgoData.saveData.recordIndex != 0)
            {
                uteSleepAlgoData.isGetUp = true;
            }
            else
            {
                uteSleepAlgoData.isGetUp = false;
            }
        }
        uteModuleFilesystemCloseFile(file);
    }
#if UTE_OPTIMIZE_SLEEP_SUPPORT
    else
    {
        if ((currTimeMinSec >= (18 * 3600)) || (currTimeMinSec < (6 * 3600))) //>=18:00  <6:00 // 非起床状态
        {
            isNotGetUp = true;
            UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,not get up", __func__);
        }
    }
#endif
    if (isNotGetUp)
    {
        // if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SLEEP_TMP_DATA, &file, FS_O_RDONLY))
        // {
        //     uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        //     uteModuleFilesystemReadData(file, &uteSleepAlgoData, sizeof(ute_sleep_data_t));
        //     uteModuleFilesystemCloseFile(file);
        //     UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read tmp data", __func__);
        // }
        uteSleepAlgoData.isGetUp = false;
    }
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
    uteSleepAlgoData.isGetUp = false;
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    uteModuleCwmReadNapSleepDataFromFs(time);
#endif
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currTimeMinSec=%d,uteSleepAlgoData.isGetUp=%d", __func__,currTimeMinSec,uteSleepAlgoData.isGetUp);
}
/**
 *@brief       睡眠数据保存
 *@details
 *@author        zn.zeng
 *@date        2021-10-26
 */
void uteModuleSleepSaveSleepData(ute_module_systemtime_time_t time)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = sizeof(ute_sleep_data_save_t);
    uint8_t path[40];
    memset(&path[0], 0, sizeof(path));
    uint16_t currTimeHourMin = time.hour << 8 | time.min;
    /*! 18~24点不保存数据 zn.zeng 2022-03-22*/
    if (currTimeHourMin > 0x1200)
    {
        return;
    }
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
#if 0 // test data
    static uint8_t testDay = 0;
    // time.day = time.day+testDay;
    time.day = 26;
    uteSleepAlgoData.saveData.year = time.year;
    uteSleepAlgoData.saveData.mon = time.month;
    uteSleepAlgoData.saveData.day = time.day;
    for(int k=0; k<50; k++)
    {
        uteSleepAlgoData.saveData.sleep_record[k].startTime.hour = testDay;
        uteSleepAlgoData.saveData.sleep_record[k].startTime.min = testDay;
        uteSleepAlgoData.saveData.sleep_record[k].sleepFlag = rand()%3;
        uteSleepAlgoData.saveData.sleep_record[k].period = k;
        uteSleepAlgoData.saveData.sleep_record[k].state = 4;
    }
    if (testDay % 2)
    {
        uteSleepAlgoData.saveData.recordIndex = 25;
    }
    else
    {
        uteSleepAlgoData.saveData.recordIndex = 50;
    }
    testDay++;
#endif // test data
    sprintf((char *)&path[0], "%04d%02d%02d", time.year, time.month, time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SLEEP_DIR, dirInfo, NULL);
    if ((dirInfo->filesCnt >= UTE_MODULE_SLEEP_DATA_MAX_DAYS) && (memcmp(&path[0], &dirInfo->filesName[0][0], 8) != 0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-10-26*/
        memset(&path[0], 0, sizeof(path));
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_SLEEP_DIR, &dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,del file=%s", __func__, &path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0], 0, sizeof(path));
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_SLEEP_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,dirInfo->filesCnt=%d", __func__, dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,save file=%s", __func__, &path[0]);
    /*! 保存当前数据zn.zeng, 2021-10-26  */
    void *file;
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &(uteSleepAlgoData.saveData), buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,open file err", __func__);
    }
    uteModulePlatformMemoryFree(dirInfo);
}
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
/**
 *@brief       通过获取小睡睡眠数据来画图
 *@details  按时间日期读取
 *@return true 为存在数据
 *@author        zn.zeng
 *@date        2021-10-26
 */
bool uteModuleSleepReadDayData(uint16_t year, uint8_t mon, uint8_t day, ute_sleep_data_save_t *pread)
{
    bool result = false;
    uint8_t dataIndx = 0;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,saveNapData.recordIndex=%d", __func__, uteCwmSleepData.saveNapData.recordIndex);
    if ((uteCwmSleepData.saveNapData.recordIndex > 0))  //(systemTime == readTime) &&
    {
        bool sleepStart = false;
        uint16_t firstStartMin = 0;
        uint8_t startIdx = 0;
        for(uint8_t i=0; i<uteCwmSleepData.saveNapData.recordIndex; i++)
        {
            if(uteCwmSleepData.saveNapData.sleep_record[i].state == LIGHT_SLEEP && sleepStart == false)
            {
                sleepStart = true;
                firstStartMin = uteCwmSleepData.saveNapData.sleep_record[i].startTime.hour*60+uteCwmSleepData.saveNapData.sleep_record[i].startTime.min;
                startIdx = i;
            }
            if((uteCwmSleepData.saveNapData.sleep_record[i].state == STOP_SLEEP || i==(uteCwmSleepData.saveNapData.recordIndex-1)) && sleepStart == true)
            {
                sleepStart = false;
                uint16_t currStatusStartTime =  uteCwmSleepData.saveNapData.sleep_record[i].startTime.hour*60+uteCwmSleepData.saveNapData.sleep_record[i].startTime.min;
                uint16_t diffMin = (currStatusStartTime+24*60-firstStartMin)%(24*60);
                if(diffMin>=3*60)
                {
                    dataIndx += 1;
                    memset((void*)pread,0,sizeof(ute_sleep_data_save_t));
                    pread->year = uteCwmSleepData.saveNapData.year;
                    pread->mon = uteCwmSleepData.saveNapData.mon;
                    pread->day = uteCwmSleepData.saveNapData.day;
                    pread->recordIndex = i - startIdx + 1;
                    memcpy(&pread->sleep_record[0],&uteCwmSleepData.saveNapData.sleep_record[startIdx],sizeof(sleep_record_t)*pread->recordIndex);
                    pread->getUpTime.hour = currStatusStartTime / 60;
                    if (pread->getUpTime.hour > 24)
                    {
                        pread->getUpTime.hour = pread->getUpTime.hour - 24;
                    }
                    pread->getUpTime.min = currStatusStartTime % 60;
                }
            }
        }
    }

    if(dataIndx>=1)
    {
        result = true;
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.dataIndx=%d", __func__, dataIndx);

    return result;
}
#else
/**
 *@brief       睡眠数据读取
 *@details  按时间日期读取
 *@return true 为存在数据
 *@author        zn.zeng
 *@date        2021-10-26
 */
bool uteModuleSleepReadDayData(uint16_t year, uint8_t mon, uint8_t day, ute_sleep_data_save_t *pread)
{
    uint32_t currTime = year << 16 | mon << 8 | day;
    uint32_t readTime = 0;
    uint16_t dayMin;
    uint16_t size = sizeof(ute_sleep_data_save_t);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint32_t systemTime = time.year << 16 | time.month << 8 | time.day;
    pread->recordIndex = 0;
    if (currTime == systemTime)
    {
        dayMin = time.hour * 60 + time.min;
        if ((dayMin > 1080) || (dayMin < 300)) // 18:00~5:00
        {
            return false;
        }
    }
    readTime = uteSleepAlgoData.saveData.year << 16 | uteSleepAlgoData.saveData.mon << 8 | uteSleepAlgoData.saveData.day;
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read buff currTime=%x,readTime=%x", __func__, currTime, readTime);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,uteSleepAlgoData.saveData.recordIndex = %d", __func__, uteSleepAlgoData.saveData.recordIndex);
    if ((currTime == readTime) && (uteSleepAlgoData.saveData.recordIndex > 0))
    {
        uteModulePlatformTakeMutex(uteModuleSleepMute);
        memcpy(pread, &uteSleepAlgoData.saveData, size);
        if ((!uteSleepAlgoData.isGetUp) && (uteSleepAlgoData.saveData.recordIndex < UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE))
        {
            if (uteSleepAlgoData.saveData.sleep_record[uteSleepAlgoData.saveData.recordIndex].period > 0)
            {
                pread->recordIndex = pread->recordIndex + 1;
            }
        }

        dayMin = pread->sleep_record[pread->recordIndex - 1].startTime.hour * 60 + pread->sleep_record[pread->recordIndex - 1].startTime.min + pread->sleep_record[pread->recordIndex - 1].period;
        pread->getUpTime.hour = dayMin / 60;
        if (pread->getUpTime.hour > 24)
        {
            pread->getUpTime.hour = pread->getUpTime.hour - 24;
        }
        pread->getUpTime.min = dayMin % 60;
        uteModulePlatformGiveMutex(uteModuleSleepMute);
        return true;
    }
    void *file;
    uint8_t path[40];
    memset(&path[0], 0, sizeof(path));
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_SLEEP_DIR, year, mon, day);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read file=%s", __func__, &path[0]);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, pread, size);
        uteModuleFilesystemCloseFile(file);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,pread->recordIndex=%d", __func__, pread->recordIndex);
        if (pread->recordIndex > 0)
        {
            dayMin = pread->sleep_record[pread->recordIndex - 1].startTime.hour * 60 + pread->sleep_record[pread->recordIndex - 1].startTime.min + pread->sleep_record[pread->recordIndex - 1].period;
            pread->getUpTime.hour = dayMin / 60;
            if (pread->getUpTime.hour > 24)
            {
                pread->getUpTime.hour = pread->getUpTime.hour - 24;
            }
            pread->getUpTime.min = dayMin % 60;
            return true;
        }
    }
    pread->recordIndex = 0;
    return false;
}
#endif

#if UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT
/**
 *@brief       睡眠数据接入
 *@details    每秒跑一次，从sport 接入
 *@author        zn.zeng
 *@date        2021-10-26
 */
void uteModuleSleepSaveSampleData(uint16_t minStepCnt, uint8_t minSleepCnt, ute_module_systemtime_time_t time, uint8_t avgHeart)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint8_t tmp[8];
    tmp[0] = (minStepCnt >> 8) & 0xff;
    tmp[1] = minStepCnt & 0xff;
    tmp[2] = time.month;
    tmp[3] = time.day;
    tmp[4] = time.hour;
    tmp[5] = time.min;
    tmp[6] = minSleepCnt;
    tmp[7] = avgHeart;
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOG_SLEEP_SAMPLE_DATA, &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
        int32_t size = uteModuleFilesystemGetFileSize(file);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,size=%d", __func__, size);
        if (size >= (11520*2)) //大于5(57600)天数据，删除数据重新开始保存
        {
            UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,seek set", __func__);
            uteModuleFilesystemCloseFile(file);
            uteModuleFilesystemDelFile(UTE_MODULE_FILESYSTEM_LOG_SLEEP_SAMPLE_DATA);
            uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOG_SLEEP_SAMPLE_DATA, &file, FS_O_WRONLY | FS_O_CREAT);
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        }
        uteModuleFilesystemWriteData(file, &tmp[0], 8);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
 *@brief        开始发送睡眠采样数据
 *@details
 *@author       zn.zeng
 *@date       2021-12-29
 */
void uteModuleSleepSendSleepSampleData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t sendBuff[247];
    uint8_t cpySize = 0;
    uint8_t sendSize = 0;
    memset(&sendBuff[0], 0, 247);
    sendBuff[0] = CMD_SLEEP_ON_BAND;
    sendBuff[1] = 0x03;
    uint16_t mtu = uteApplicationCommonGetMtuSize();
    cpySize = ((mtu - 2) / 8) * 8;
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOG_SLEEP_SAMPLE_DATA, &file, FS_O_RDONLY))
    {
        if (sendParam->currSendFileSize == 0)
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
            sendParam->currSendFileSize = uteModuleFilesystemGetFileSize(file);
        }
        if ((sendParam->currSendFileDataOffset + cpySize) > sendParam->currSendFileSize)
        {
            cpySize = sendParam->currSendFileSize - sendParam->currSendFileDataOffset;
        }
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,cpySize=%d,currSendFileDataOffset=%d,currSendFileSize=%d", __func__, cpySize, sendParam->currSendFileDataOffset, sendParam->currSendFileSize);
        uteModuleFilesystemSeek(file, sendParam->currSendFileDataOffset, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &sendBuff[2], cpySize);
        uteModuleFilesystemCloseFile(file);
        sendSize = cpySize + 2;
        sendParam->currSendFileDataOffset = sendParam->currSendFileDataOffset + cpySize;
        uteModuleProfileBle50SendToPhone(&sendBuff[0], sendSize);
        if (sendParam->currSendFileDataOffset == sendParam->currSendFileSize)
        {
            goto SLEEP_SAMPLE_SEND_FINISH;
        }
    }
    else
    {
    SLEEP_SAMPLE_SEND_FINISH:
        uteApplicationCommonSyncDataTimerStop();
        sendBuff[1] = 0x04;
        sendSize = 2;
        uteModuleProfileBleSendToPhone(&sendBuff[0], sendSize);
    }
}

/**
 *@brief        准备开始发送睡眠采样数据
 *@details
 *@author       zn.zeng
 *@date       2021-12-29
 */
void uteModuleSleepStartSendSleepSampleData(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->currSendFileDataOffset = 0;
    param->currSendFileSize = 0;
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSleepSendSleepSampleData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,", __func__);
}
/**
 *@brief        保存睡眠debug数据
 *@details     每小时保存一次debug数据
 *@author       zn.zeng
 *@date       2022-03-29
 */
void uteModuleSleepSaveLogData(ute_module_systemtime_time_t time)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    void *file;
    uint8_t path[30];
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR, dirInfo, NULL);
    if (dirInfo->filesCnt >= 24)
    {
        memset(&path[0], 0, 30);
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR, &dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,del file=%s", __func__, &path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0], 0, 30);
    sprintf((char *)&path[0], "%s/%04d%02d%02d%02d",UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR,time.year, time.month, time.day, time.hour);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,%s", __func__,&path[0]);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &(uteSleepAlgoData.saveData), sizeof(ute_sleep_data_save_t));
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(dirInfo);
}
/**
 *@brief        开始发送睡眠log数据
 *@details
 *@author       zn.zeng
 *@date       2022-03-29
 */
void uteModuleSleepSendSleepLogData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t sendBuff[247];
    uint8_t cpySize = 0;
    uint8_t sendSize = 0;
    memset(&sendBuff[0], 0, 247);
    sendBuff[0] = CMD_DEBUG_TEST_DATA;
    sendBuff[1] = 0x07;
    sendBuff[2] = 0x02;
    uint16_t mtu = uteApplicationCommonGetMtuSize();
    cpySize = mtu - 3;
    void *file;
    uint8_t path[30];
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    if (sendParam->dirInfo.filesCnt > 0)
    {
        memset(&path[0], 0, 30);
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,%s", __func__,&path[0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currSendFileIndex=%d,currSendFileDataOffset=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendFileDataOffset);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,mtu=%d,dataBuffSize=%d,cpySize=%d", __func__,mtu,sendParam->dataBuffSize,cpySize);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &dataBuff[0], sendParam->dataBuffSize);
            uteModuleFilesystemCloseFile(file);
        }
        uint32_t lastSize = sendParam->dataBuffSize - sendParam->currSendFileDataOffset;
        if ( lastSize < cpySize)
        {
            cpySize = lastSize;
        }
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,lastSize=%d,cpySize=%d", __func__,lastSize,cpySize);
        memcpy(&sendBuff[3],&dataBuff[sendParam->currSendFileDataOffset], cpySize);
        sendSize = cpySize + 3;
        uteModuleProfileBle50SendToPhone(&sendBuff[0], sendSize);
        sendParam->currSendFileDataOffset = sendParam->currSendFileDataOffset + cpySize;
        if (sendParam->currSendFileDataOffset == sendParam->dataBuffSize)
        {
            sendParam->currSendFileDataOffset = 0;
            sendParam->currSendFileIndex++;
            if (sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt)
            {
                goto SEND_SLEEP_LOG_FINISH;
            }
        }
    }
    else
    {
    SEND_SLEEP_LOG_FINISH:
        sendSize = 3;
        sendBuff[2] = 0x03;
        uteApplicationCommonSyncDataTimerStop();
        uteModuleProfileBleSendToPhone(&sendBuff[0], sendSize);
    }
    uteModulePlatformMemoryFree(dataBuff);
}
/**
 *@brief        准备开始发送睡眠log数据
 *@details
 *@author       zn.zeng
 *@date       2022-03-29
 */
void uteModuleSleepStartSendSleepLogData(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    param->dataBuffSize = sizeof(ute_sleep_data_save_t);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_LOG_SLEEP_DEBUG_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSleepSendSleepLogData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_STEP_LVL, "%s,", __func__);
}
#endif
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
/**
 *@brief        读取模拟数据
 *@details
 *@return false 为读取结束
 *@author       zn.zeng
 *@date       2021-12-29
 */
bool uteModuleSleepReadSimulationData(ute_module_systemtime_time_t *time, ute_step_sleep_param_t *sleepData, uint8_t *heart)
{
    static uint32_t cnt = 0;
    static bool isSleepReadSimulationDataFinish = false;
    if (isSleepReadSimulationDataFinish || (cnt > 2880))
    {
        // return false;
        isSleepReadSimulationDataFinish = false;
        cnt = 0;
        uteSleepAlgoData.isGetUp = false;
    }
    uint8_t tmp[8];
    uteModulePlatformWdgFeed();
    uteModulePlatformFlashNorRead(&tmp[0], OTA_TMP_ADDR + cnt * 8, 8);
    if (tmp[0] == 0xff)
    {
        isSleepReadSimulationDataFinish = true;
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,finish cnt=%d", __func__, cnt);
        return false;
    }
    time->hour = tmp[4];
    time->min = tmp[5];
    *heart = tmp[7];
    sleepData->currentMinuteSleepTurnCnt = tmp[6];
    sleepData->currentMinuteStepCnt = tmp[0] >> 8 | tmp[1];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,cnt=%d", __func__, cnt);
    cnt++;
    return true;
}
#endif
/**
 *@brief       睡眠的情况下保存临时数据，用于开关机恢复
 *@details    每秒跑一次，从sport 接入
 *@author        zn.zeng
 *@date        2022-04-12
 */
void uteModuleSleepSaveTmpDataForPowerOff(void)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SLEEP_TMP_DATA, &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &uteSleepAlgoData, sizeof(ute_sleep_data_t));
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,", __func__);
}

/**
 *@brief       睡眠数据接入
 *@details    每秒跑一次，从sport 接入
 *@author        zn.zeng
 *@date        2021-10-26
 */
void uteModuleSleepDataInputSecond(ute_step_sleep_param_t *sleepData, ute_module_systemtime_time_t time, bool isStepping)
{
#if UTE_MODULE_HEART_SUPPORT
    uint8_t heart = (uint8_t)uteModuleHeartGetHeartValue();
#else
    uint8_t heart = 0;
#endif
    if ((heart != 0) && (uteModuleSleepData.heartCnt < 60))
    {
        uteModuleSleepData.oneMinHeartValue[uteModuleSleepData.heartCnt] = heart;
        uteModuleSleepData.heartCnt++;
    }
    if (time.sec == 59)
    {
        uint8_t avgHeart = 0;
        if (sleepData->currentMinuteSleepTurnCnt > 250)
        {
            sleepData->currentMinuteSleepTurnCnt = 250;
        }
        if (isStepping)
        {
            sleepData->currentMinuteSleepTurnCnt = 0xff;
        }
        if (uteModuleSleepData.heartCnt != 0)
        {
            uint32_t oneMinHeartSun = 0;
            for (uint8_t i = 0; i < uteModuleSleepData.heartCnt; i++)
            {
                oneMinHeartSun += uteModuleSleepData.oneMinHeartValue[i];
            }
            avgHeart = oneMinHeartSun / uteModuleSleepData.heartCnt;
            //没有实时心率时不能传固定心率值
            if (get_random(2))
            {
                avgHeart += get_random(3);
            }
            else
            {
                avgHeart -= get_random(3);
            }
        }
#if UTE_MODULE_HEART_SUPPORT
        if (!uteModuleHeartIsWear())
        {
            avgHeart = 0;
            sleepData->currentMinuteStepCnt = 0;
            sleepData->currentMinuteSleepTurnCnt = 0xfd;
        }
        if (!uteModuleHeartIsAutoTesting())
        {
            avgHeart = 40;
        }
#else
        avgHeart = 0;
        sleepData->currentMinuteStepCnt = 0;
        sleepData->currentMinuteSleepTurnCnt = 0xfd;
#endif
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,simulation data start time=%d", __func__, uteModulePlatformGetSystemTick());
        static bool isSimulationFinish = false;
        if (isSimulationFinish)
        {
            goto SIMULATION_FINISH;
        }
        while (uteModuleSleepReadSimulationData(&time, sleepData, &avgHeart))//一次性模拟
            // if(uteModuleSleepReadSimulationData(&time, sleepData, &avgHeart))//真实模拟，每分钟跑一次
        {
#endif
            uint32_t currTimeSec = time.hour*3600+time.min*60+time.sec;
#if UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT
            uteModuleSleepSaveSampleData(sleepData->currentMinuteStepCnt, sleepData->currentMinuteSleepTurnCnt, time, avgHeart);
#endif
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
            if (currTimeSec == (18*3600-1))
            {
                uteModuleSleepSaveSleepData(time);
                uteSleepOnBandClearSleepStatus();
                uteSleepAlgoData.saveData.recordIndex = 0;
                uteSleepAlgoData.isGetUp = false;
                uteModuleSleepData.isCurrHasSleepData = false;
            }
#else
            UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currTimeSec=%d,.isGetUp=%d", __func__,currTimeSec,uteSleepAlgoData.isGetUp);
            UTE_MODULE_LOG_BUFF(UTE_LOG_SLEEP_LVL,(uint8_t *)&(uteSleepAlgoData.saveData),sizeof(ute_sleep_data_save_t));
            if (!uteSleepAlgoData.isGetUp)
            {
                UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,avgHeart=%d,recordIndex=%d,.heartCnt=%d", __func__, avgHeart, uteSleepAlgoData.saveData.recordIndex, uteModuleSleepData.heartCnt);
                uteModulePlatformTakeMutex(uteModuleSleepMute);
                uteSleepAnalysisOnBand(time.hour, time.min, sleepData->currentMinuteSleepTurnCnt, sleepData->currentMinuteStepCnt, avgHeart, 0x01);
                uteSleepAlgoData.saveData.recordIndex = uteSleepOnBandGetRecordIndex();
                UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currentMinuteSleepTurnCnt=%d,currentMinuteStepCnt=%d,.isStepping=%d,.isGetUp=%d", __func__, sleepData->currentMinuteSleepTurnCnt, sleepData->currentMinuteStepCnt, isStepping, uteSleepAlgoData.isGetUp);
                if (uteSleepAlgoData.isGetUp)
                {
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
                    for (uint8_t i = 0; i < 50; i++)
                    {
                        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,%02d:%02d state=%d,period=%d", __func__, uteSleepAlgoData.saveData.sleep_record[i].startTime.hour, uteSleepAlgoData.saveData.sleep_record[i].startTime.min, uteSleepAlgoData.saveData.sleep_record[i].state, uteSleepAlgoData.saveData.sleep_record[i].period);
                    }
#endif
                    if (currTimeSec < (18*3600-1))//<18:00
                    {
                        if(uteSleepAlgoData.saveData.recordIndex>0)
                        {
                            uteModuleSleepData.isCurrHasSleepData = true;
                        }
                        uteModuleSleepSaveSleepData(time);
                        uteSleepOnBandClearSleepStatus();
                        uteSleepAlgoData.saveData.recordIndex = 0;
                    }
                }
                else
                {
                    if (currTimeSec == (18*3600-1))
                    {
                        uteModuleSleepSaveSleepData(time);
                        uteSleepOnBandClearSleepStatus();
                        uteSleepAlgoData.saveData.recordIndex = 0;
                        uteSleepAlgoData.isGetUp = false;
                        uteModuleSleepData.isCurrHasSleepData = false;
                    }
                }
                // uteModuleSleepSaveTmpDataForPowerOff();
                uteModulePlatformGiveMutex(uteModuleSleepMute);
            }
            else
            {
#if UTE_OPTIMIZE_SLEEP_SUPPORT
                if (currTimeSec >= (18*3600)||(currTimeSec < (6*3600))) //add by pcm 20220812 在18~6点这个时间段内，如果isGetUp为true，则都进来重新设置一次，然后清除睡眠数据
#else
                if (currTimeSec == (18*3600-1))  //delete by pcm 20220812
#endif
                {
                    uteSleepOnBandClearSleepStatus();
                    uteSleepAlgoData.saveData.recordIndex = 0;
                    uteSleepAlgoData.isGetUp = false;
                    uteModuleSleepData.isCurrHasSleepData = false;
                }
            }
#endif
            uteSleepAlgoData.saveData.year = time.year;
            uteSleepAlgoData.saveData.mon = time.month;
            uteSleepAlgoData.saveData.day = time.day;
#if UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT
            if(time.min == 59)
            {
                uteModuleSleepSaveLogData(time);
            }
#endif
#if UTE_MODULE_SLEEP_RUN_SIMULATION_SUPPORT
        }
        // else
        // {
        //     isSimulationFinish = true;//真实模拟，每分钟跑一次
        // }
    SIMULATION_FINISH:
        isSimulationFinish = true; //一次性模拟
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isSimulationFinish=%d,simulation data end time=%d", __func__,isSimulationFinish,uteModulePlatformGetSystemTick());
#endif
        sleepData->currentMinuteSleepTurnCnt = 0;
        sleepData->currentMinuteStepCnt = 0;
        uteModuleSleepData.heartCnt = 0;
    }
}

/**
 *@brief        开始发送睡眠数据
 *@details
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepSendSleepHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[40];
    uint8_t response[247];
    uint16_t sendSize = 0;
    uint16_t mtuSize = uteApplicationCommonGetMtuSize();
    sendParam->dataBuffSize = sizeof(ute_sleep_data_save_t);
    ute_sleep_data_save_t *pRead = (ute_sleep_data_save_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset((void*)pRead, 0, sendParam->dataBuffSize);
    memset(&path[0], 0, sizeof(path));
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__, sendParam->currSendFileIndex, sendParam->currSendHourIndex, sendParam->dirInfo.filesCnt);

    if (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt)
    {
        void *file;
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_SLEEP_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read file=%s", __func__, &path[0]);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,(void*)pRead, sendParam->dataBuffSize);
            uteModuleFilesystemCloseFile(file);
        }
        if (sendParam->currSendHourIndex == 0)
        {
            response[0] = CMD_SLEEP_ON_BAND;
            response[1] = 0x01;
            response[2] = (pRead->year >> 8) & 0xff;
            response[3] = pRead->year & 0xff;
            response[4] = pRead->mon;
            response[5] = pRead->day;
            response[6] = pRead->recordIndex;
            sendSize = 7;
            uteModuleProfileBleSendToPhone(&response[0], sendSize);
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,pRead->recordIndex=%d,currSendHourIndex=%d", __func__, pRead->recordIndex, sendParam->currSendHourIndex);
    if (sendParam->currSendHourIndex < pRead->recordIndex)
    {
        response[0] = CMD_SEND_SLEEP_ON_BAND_DATAS;
        sendSize = 1;
        for (; sendParam->currSendHourIndex < pRead->recordIndex; sendParam->currSendHourIndex++)
        {
            if ((sendSize + 6) > mtuSize)
            {
                break;
            }
            response[sendSize] = pRead->sleep_record[sendParam->currSendHourIndex].startTime.hour;
            response[sendSize + 1] = pRead->sleep_record[sendParam->currSendHourIndex].startTime.min;
            response[sendSize + 2] = pRead->sleep_record[sendParam->currSendHourIndex].state;
            response[sendSize + 3] = pRead->sleep_record[sendParam->currSendHourIndex].sleepFlag;
            response[sendSize + 4] = pRead->sleep_record[sendParam->currSendHourIndex].period >> 8 & 0xff;
            response[sendSize + 5] = pRead->sleep_record[sendParam->currSendHourIndex].period & 0xff;
            sendSize += 6;
        }
        uteModuleProfileBle50SendToPhone(&response[0], sendSize);
    }
    if (sendParam->currSendHourIndex == pRead->recordIndex)
    {
        if (sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt)
        {
            uteApplicationCommonSyncDataTimerStop();
            response[0] = CMD_SLEEP_ON_BAND;
            response[1] = 0x02;
            sendSize = 2;
            uteModuleProfileBleSendToPhone(&response[0], sendSize);
        }
        else
        {
            sendParam->currSendHourIndex = 0;
            sendParam->currSendFileIndex++;
        }
    }
    uteModulePlatformMemoryFree((void*)pRead);
}

/**
 *@brief        准备开始发送睡眠数据
 *@details
 *@param[in]  需要同步的数据时间戳
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepStartSendSleepHistoryData(ute_module_systemtime_time_t time)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    if (!uteSleepAlgoData.isGetUp)
    {
        if (uteSleepAlgoData.saveData.recordIndex < UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
        {
            if (uteSleepAlgoData.saveData.sleep_record[uteSleepAlgoData.saveData.recordIndex].period > 0)
            {
                uteSleepAlgoData.saveData.recordIndex = uteSleepAlgoData.saveData.recordIndex + 1;
            }
        }
        ute_module_systemtime_time_t readTime;
        uteModuleSystemtimeGetTime(&readTime);
        uteModuleSleepSaveSleepData(readTime);
    }
    param->crc = 0;
    param->time = time;
    if (param->time.year == 0)
    {
        param->isAllData = true;
    }
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_SLEEP_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSleepSendSleepHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,", __func__);
}

/**
 *@brief        获取当前睡眠数据
 *@details
 *@param[out]  ute_module_sleep_display_data_t *sleepDisplayData ,数据指针
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepGetCurrDayDataDisplay(ute_module_sleep_display_data_t *sleepDisplayData)
{
    sleepDisplayData->lightSleepMin = 0;
    sleepDisplayData->deepSleepMin = 0;
    sleepDisplayData->wakeSleepMin = 0;
    sleepDisplayData->totalSleepMin = 0;
#if UTE_REM_SLEEP_SUPPORT
    sleepDisplayData->RemSleepMin = 0;
#endif
    sleepDisplayData->SleepMin = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    ute_sleep_data_save_t *pRead = (ute_sleep_data_save_t *)uteModulePlatformMemoryAlloc(sizeof(ute_sleep_data_save_t));
    memset((void*)pRead,0,sizeof(ute_sleep_data_save_t));
    if (uteModuleSleepReadDayData(time.year, time.month, time.day, pRead))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,recordIndex=%d", __func__, pRead->recordIndex);
        for (int k = 0; k < pRead->recordIndex; k++)
        {
            if (pRead->sleep_record[k].state == LIGHT_SLEEP)
            {
                sleepDisplayData->lightSleepMin += pRead->sleep_record[k].period;
            }
            else if (pRead->sleep_record[k].state == DEEP_SLEEP)
            {
                sleepDisplayData->deepSleepMin += pRead->sleep_record[k].period;
            }
            else if (pRead->sleep_record[k].state == AWAKE_SLEEP)
            {
                sleepDisplayData->wakeSleepMin += pRead->sleep_record[k].period;
            }
#if UTE_REM_SLEEP_SUPPORT
            else if (pRead->sleep_record[k].state == REM_SLEEP)
            {
                sleepDisplayData->RemSleepMin += pRead->sleep_record[k].period;
            }
#endif
        }

#if UTE_REM_SLEEP_SUPPORT
        sleepDisplayData->SleepMin = sleepDisplayData->lightSleepMin + sleepDisplayData->deepSleepMin;
#if UTE_MODULE_SLEEP_NOT_AWAKE_DATA_SUPPORT
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->RemSleepMin;
#else
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->wakeSleepMin+sleepDisplayData->RemSleepMin;
#endif
        sleepDisplayData->fallAsSleepTime.hour = pRead->sleep_record[0].startTime.hour;
        sleepDisplayData->fallAsSleepTime.min = pRead->sleep_record[0].startTime.min;
        sleepDisplayData->getUpSleepTime.hour = pRead->getUpTime.hour;
        sleepDisplayData->getUpSleepTime.min = pRead->getUpTime.min;
#else
        sleepDisplayData->SleepMin = sleepDisplayData->lightSleepMin + sleepDisplayData->deepSleepMin;
#if UTE_MODULE_SLEEP_NOT_AWAKE_DATA_SUPPORT
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin;
#else
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->wakeSleepMin;
#endif
        sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->wakeSleepMin;
        sleepDisplayData->fallAsSleepTime.hour = pRead->sleep_record[0].startTime.hour;
        sleepDisplayData->fallAsSleepTime.min = pRead->sleep_record[0].startTime.min;
        sleepDisplayData->getUpSleepTime.hour = pRead->getUpTime.hour;
        sleepDisplayData->getUpSleepTime.min = pRead->getUpTime.min;
#endif
        memcpy(sleepDisplayData->sleep_record, pRead->sleep_record, sizeof(sleep_record_t) * UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE);
        sleepDisplayData->recordCnt = pRead->recordIndex;
    }
    else
    {
        memset(&sleepDisplayData->fallAsSleepTime, 0x00, sizeof(timestmap_t));
        memset(&sleepDisplayData->getUpSleepTime, 0x00, sizeof(timestmap_t));
    }

#if UTE_LOG_GUI_LVL // test

    static uint8_t sleepDatas[] =
    {
        0x01,0x2F,0x02,0x01,0x00,0x0B,
        0x01,0x3A,0x01,0x01,0x00,0x18,
        0x02,0x16,0x02,0x01,0x00,0x3A,
        0x03,0x14,0x04,0x01,0x00,0x02,
        0x03,0x16,0x02,0x01,0x00,0x1B,
        0x03,0x31,0x04,0x01,0x00,0x04,
        0x03,0x35,0x02,0x01,0x00,0x1A,
        0x04,0x13,0x01,0x01,0x00,0x15,
        0x04,0x29,0x0,0x01,0x00,0x30,
        0x05,0x1D,0x04,0x01,0x00,0x02,
        0x05,0x1F,0x02,0x01,0x00,0x09,
        0x05,0x28,0x01,0x01,0x00,0x15,
        0x06,0x01,0x02,0x01,0x00,0x3C,
        0x07,0x01,0x01,0x01,0x00,0x09,
        0x07,0x0A,0x04,0x01,0x00,0x03,
        0x07,0x0D,0x02,0x01,0x00,0x3B
    };

    sleepDisplayData->lightSleepMin = 0;
    sleepDisplayData->deepSleepMin = 0;
    sleepDisplayData->wakeSleepMin = 0;
    sleepDisplayData->totalSleepMin = 0;
#if UTE_REM_SLEEP_SUPPORT
    sleepDisplayData->RemSleepMin = 0;
#endif

    sleepDisplayData->recordCnt = sizeof(sleepDatas) / 6;

    for (uint8_t i = 0; i < sleepDisplayData->recordCnt; i++)
    {
        sleepDisplayData->sleep_record[i].startTime.hour = sleepDatas[6 * i];
        sleepDisplayData->sleep_record[i].startTime.min = sleepDatas[6 * i + 1];
        sleepDisplayData->sleep_record[i].state = sleepDatas[6 * i + 2];
        sleepDisplayData->sleep_record[i].sleepFlag = sleepDatas[6 * i + 3];
        sleepDisplayData->sleep_record[i].period = sleepDatas[6 * i + 4] << 8 | sleepDatas[6 * i + 5];

        if (i == 0)
        {
            sleepDisplayData->fallAsSleepTime.hour = sleepDisplayData->sleep_record[i].startTime.hour;
            sleepDisplayData->fallAsSleepTime.min = sleepDisplayData->sleep_record[i].startTime.min;
        }
        else if (i == sleepDisplayData->recordCnt - 1)
        {
            uint16_t getUpSleepTime = sleepDisplayData->sleep_record[i].startTime.hour * 60 + sleepDisplayData->sleep_record[i].startTime.min + sleepDisplayData->sleep_record[i].period;
            sleepDisplayData->getUpSleepTime.hour = getUpSleepTime / 60;
            sleepDisplayData->getUpSleepTime.min = getUpSleepTime % 60;
        }

        if (sleepDisplayData->sleep_record[i].state == LIGHT_SLEEP)
        {
            sleepDisplayData->lightSleepMin += sleepDisplayData->sleep_record[i].period;
        }
        else if (sleepDisplayData->sleep_record[i].state == DEEP_SLEEP)
        {
            sleepDisplayData->deepSleepMin += sleepDisplayData->sleep_record[i].period;
        }
        else if (sleepDisplayData->sleep_record[i].state == AWAKE_SLEEP)
        {
            sleepDisplayData->wakeSleepMin += sleepDisplayData->sleep_record[i].period;
        }
#if UTE_REM_SLEEP_SUPPORT
        else if (sleepDisplayData->sleep_record[i].state == REM_SLEEP)
        {
            sleepDisplayData->RemSleepMin += sleepDisplayData->sleep_record[i].period;
        }
#endif
    }
#if UTE_REM_SLEEP_SUPPORT
    sleepDisplayData->SleepMin = sleepDisplayData->lightSleepMin + sleepDisplayData->deepSleepMin;
#if UTE_MODULE_SLEEP_NOT_AWAKE_DATA_SUPPORT
    sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->RemSleepMin;
#else
    sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->wakeSleepMin + sleepDisplayData->RemSleepMin;
#endif
#else
    sleepDisplayData->SleepMin = sleepDisplayData->lightSleepMin + sleepDisplayData->deepSleepMin;
#if UTE_MODULE_SLEEP_NOT_AWAKE_DATA_SUPPORT
    sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin;
#else
    sleepDisplayData->totalSleepMin = sleepDisplayData->SleepMin + sleepDisplayData->wakeSleepMin;
#endif
#endif
#endif

    // uteModuleSleepSetCurrDayDataGraphs(pRead,sleepDisplayData);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,deepSleepMin=%d,lightSleepMin=%d,wakeSleepMin=%d", __func__, sleepDisplayData->deepSleepMin, sleepDisplayData->lightSleepMin, sleepDisplayData->wakeSleepMin);
    uteModulePlatformMemoryFree(pRead);
}

/**
 *@brief        时间变化
 *@details   跨天数，跨18:00时要处理清零
 *@param[in] ute_module_systemtime_time_t curr,当前时间
 *@param[in] ute_module_systemtime_time_t newSet,将要设置的时间
 *@author       zn.zeng
 *@date       2022-04-20
 */
void uteModuleSleepSystemtimeChange(ute_module_systemtime_time_t curr,ute_module_systemtime_time_t newSet)
{
    uint32_t newTime = newSet.year<<16|newSet.month<<8|newSet.day;
    uint32_t currTime = curr.year<<16|curr.month<<8|curr.day;
    bool isNeedSave = false;
    if (!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    if(newTime!=currTime)
    {
        isNeedSave = true;
    }
    else
    {
        uint16_t newTimeHourMin = newSet.hour << 8 | newSet.min;
        uint16_t currTimeHourMin = curr.hour << 8 | curr.min;
        // 18:00 ->0x1200
        if ((currTimeHourMin < 0x1200) && (newTimeHourMin >= 0x1200))
        {
            isNeedSave = true;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,isNeedSave=%d", __func__,isNeedSave);
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
    if(isNeedSave)
    {
        if (!uteSleepAlgoData.isGetUp)
        {
            uteModuleSleepSaveSleepData(curr);
        }
        uteSleepOnBandClearSleepStatus();
        uteSleepAlgoData.isGetUp = false;
        uteSleepAlgoData.saveData.recordIndex = 0;
    }
#endif
}

/**
 *@brief        获取当天起床标志
 *@details
 *@author       zn.zeng
 *@date       2022-04-20
 */
bool uteModuleSleepCurrDayIsGetUp(void)
{
    return uteSleepAlgoData.isGetUp;
}
/**
 *@brief        是否有睡眠
 *@details
 *@author       zn.zeng
 *@date       2022-04-20
 */
bool uteModuleSleepCurrDayIsHasSleep(void)
{
    return uteModuleSleepData.isCurrHasSleepData;
}

#if UTE_MODULE_CYWEE_MOTION_SUPPORT
/*
 *赛维睡眠算法输出处理
 *
 */
static uint16_t lastSleepStatusStartTime = 0;  //上一个状态开始时间
static uint16_t notifySleepStatusTime = 0;  //通知睡眠的时间,当做睡眠起始时间
/*
isSleep true:表示睡眠解析 false:表示小睡解析
*/
void uteModuleSleepAnalysis(uteCwmSleepOnceData_t uteCwmSleepOnceData,bool isSleep)
{
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL,"isSleep = %d uteCwmSleepOnceData.sleepStatus = %d",isSleep,uteCwmSleepOnceData.sleepStatus);

    if(isSleep == true)
    {
        if(uteModuleCwmIsSleeping() == false &&  uteCwmSleepOnceData.sleepStatus != CWM_SLEEP_STATUS_SLEEP_NOTIFICATION)
        {
            return;
        }
    }
    else
    {
        if(uteModuleCwmIsNaping()==false &&  uteCwmSleepOnceData.sleepStatus != 0)
        {
            return;
        }
    }
    ute_sleep_data_save_t *pTempSleepData = NULL;

    pTempSleepData = &uteCwmSleepData.saveNapData;

    uteCwmSleepData.sleepStatus = uteCwmSleepOnceData.sleepStatus;
    uint8_t saveIndex = pTempSleepData->recordIndex;
    uint16_t diffMin = 0;

    switch(uteCwmSleepOnceData.sleepStatus)
    {
        case CWM_SLEEP_STATUS_FALL_SLEEP:   //入睡当做浅睡处理
        case CWM_SLEEP_STATUS_LIGHT_SLEEP:
        {
            if(uteCwmSleepOnceData.sleepStatus == CWM_SLEEP_STATUS_FALL_SLEEP)
            {
                notifySleepStatusTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                lastSleepStatusStartTime = notifySleepStatusTime;
                /*上一段睡眠如果超过3小时,新的入睡时间与它间隔小于等于30分钟,用清醒状态连接*/
                if(uteCwmSleepData.lastSleepIsLongTime == true)
                {
                    if(saveIndex > 0)
                    {
                        uint16_t lastSleepStopTime = pTempSleepData->sleep_record[saveIndex - 1].startTime.hour*60+pTempSleepData->sleep_record[saveIndex - 1].startTime.min;
                        diffMin = (notifySleepStatusTime+24*60-lastSleepStopTime)%(24*60);
                        if(diffMin<=30 && pTempSleepData->sleep_record[saveIndex - 1].state == STOP_SLEEP)
                        {

                            // pTempSleepData->sleep_record[saveIndex - 1].startTime.min = uteCwmSleepOnceData.min;
                            pTempSleepData->sleep_record[saveIndex - 1].state = AWAKE_SLEEP;
                            pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
                            lastSleepStatusStartTime = lastSleepStopTime;
                        }
                    }
                    uteCwmSleepData.lastSleepIsLongTime = false;
                }
            }
            if(saveIndex >= UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
            {
                break;
            }
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[saveIndex].state = LIGHT_SLEEP;
            pTempSleepData->sleep_record[saveIndex].period = 0;
            if(saveIndex > 0 )
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
            }
            lastSleepStatusStartTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
            pTempSleepData->recordIndex++;
            if(isSleep == false)
            {
                uteModuleCwmIsSetNaping(true);
            }
        }
        break;
        case CWM_SLEEP_STATUS_DEEP_SLEEP:
        {
            if(saveIndex >= UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
            {
                break;
            }
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[saveIndex].state = DEEP_SLEEP;
            pTempSleepData->sleep_record[saveIndex].period = 0;
            if(saveIndex > 0 )
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
            }
            lastSleepStatusStartTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
            pTempSleepData->recordIndex++;
        }
        break;
        case CWM_SLEEP_STATUS_AWAKE:
        {
            if(saveIndex >= UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
            {
                break;
            }
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[saveIndex].state = AWAKE_SLEEP;
            pTempSleepData->sleep_record[saveIndex].period = 0;
            if(saveIndex > 0 )
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
            }
            lastSleepStatusStartTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
            pTempSleepData->recordIndex++;
        }
        break;
        case CWM_SLEEP_STATUS_REM:
        {
            if(saveIndex >= UTE_SLEEP_ON_BAND_MAX_RECORD_SIZE)
            {
                break;
            }
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[saveIndex].state = REM_SLEEP;
            pTempSleepData->sleep_record[saveIndex].period = 0;
            if(saveIndex > 0 )
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
            }
            lastSleepStatusStartTime = uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
            pTempSleepData->recordIndex++;
        }
        break;
        case CWM_SLEEP_STATUS_SLEEP_NOTIFICATION:
        {
            uteModuleCwmIsSetSleeping(true);
        }
        break;
        case CWM_SLEEP_STATUS_CLOSE_NAP_SLEEP: //结束小睡
        case CWM_SLEEP_STATUS_STOP_SLEEP:
        {
            uint8_t saveIndex = pTempSleepData->recordIndex;
            uint16_t diffTotalMin = 0;
            ute_module_systemtime_time_t time;
            uteModuleSystemtimeGetTime(&time);
            uint32_t currTimeSec = time.hour*3600+time.min*60+time.sec;
            if(uteCwmSleepOnceData.sleepStatus == CWM_SLEEP_STATUS_STOP_SLEEP)
            {
                uteModuleCwmIsSetSleeping(false);
            }
            else
            {
                uteModuleCwmIsSetNaping(false);
            }
            uint16_t currTimeHourMin = time.hour << 8 | time.min;
            if(saveIndex > 0)
            {
                uint16_t currStatusStartTime =  uteCwmSleepOnceData.hour*60+uteCwmSleepOnceData.min;
                diffMin = (currStatusStartTime+24*60-lastSleepStatusStartTime)%(24*60);
                pTempSleepData->sleep_record[saveIndex - 1].period = diffMin;
                diffTotalMin = (currStatusStartTime+24*60-notifySleepStatusTime)%(24*60);
            }
#if UTE_MODULE_RUNING_LOG_SUPPORT
            UTE_MODULE_LOG_BLE(UTE_LOG_CYWEE_MOTION_LVL, "lastStatusDiffMin = %d",diffMin);
#endif
            pTempSleepData->sleep_record[saveIndex].startTime.hour = uteCwmSleepOnceData.hour;
            pTempSleepData->sleep_record[saveIndex].startTime.min = uteCwmSleepOnceData.min;
            pTempSleepData->sleep_record[pTempSleepData->recordIndex].state = STOP_SLEEP;
            pTempSleepData->recordIndex++;
            uteModuleSleepSaveNapSleepData();
            if(diffTotalMin>=3*60) //标记这是长睡眠
            {
                uteCwmSleepData.lastSleepIsLongTime = true;
            }
            /*! 18~24点不算夜间睡眠*/
            bool saveFlag = true;
            if (currTimeHourMin > 0x1200)
            {
                saveFlag = false;
            }
            if(diffTotalMin>=3*60 && saveFlag && uteSleepAlgoData.isGetUp == false) //大于3小时睡眠 & 当前时间不大于18:00
            {
                uteSleepAlgoData.isGetUp = true;
                if (currTimeSec < (18*3600-1))//<18:00
                {
                    if(pTempSleepData->recordIndex>0)
                    {
                        uteModuleSleepData.isCurrHasSleepData = true;
                    }
                }
            }
        }
        break;
        case CWM_SLEEP_STATUS_REQ_LAST_DATA:
        {
            //通过SCL_REQ_SLEEPING_DATA 查询的最后一条数据都是清醒,这里过滤掉
            uint8_t saveIndex = pTempSleepData->recordIndex;
            if(saveIndex>0)
            {
                if(pTempSleepData->sleep_record[saveIndex-1].state == AWAKE_SLEEP)
                {
                    pTempSleepData->sleep_record[saveIndex-1].startTime.hour = 0;
                    pTempSleepData->sleep_record[saveIndex-1].startTime.min = 0;
                    pTempSleepData->sleep_record[saveIndex-1].state = INIDLE;
                    pTempSleepData->sleep_record[saveIndex-1].period = 0;
                    pTempSleepData->recordIndex --;
                    //过滤查询睡眠返回的最后一条数之后,在把前一条数据时间更新。
                    if(pTempSleepData->recordIndex > 0)
                    {
                        lastSleepStatusStartTime = pTempSleepData->sleep_record[pTempSleepData->recordIndex-1].startTime.hour * 60 + pTempSleepData->sleep_record[pTempSleepData->recordIndex-1].startTime.min;
                    }
                }
            }
        }
        break;
        default:
            break;
    }
}
/**
 *@brief    小睡睡眠数据读取
 *@details  按时间日期读取
 *@return true 为存在数据
 *@author   casen
 *@date     2022-10-18
 */
bool uteModuleSleepReadDayNapData(ute_module_sleep_nap_display_data_t *pread)
{
    uint32_t readTime = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint32_t systemTime = time.year << 16 | time.month << 8 | time.day;
    pread->recordCnt = 0;
    uint8_t dataIndx = 0;
#if (1&&UTE_MODULE_LOG_SUPPORT) //test
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 14;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 21;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = LIGHT_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex ++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 14;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 53;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = STOP_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 19;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 01;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = LIGHT_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 21;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 53;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = STOP_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 22;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 01;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = LIGHT_SLEEP;
    uteCwmSleepData.saveNapData.recordIndex++;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.hour = 23;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].startTime.min = 53;
    uteCwmSleepData.saveNapData.sleep_record[uteCwmSleepData.saveNapData.recordIndex].state = STOP_SLEEP;

#endif
    readTime = uteCwmSleepData.saveNapData.year << 16 | uteCwmSleepData.saveNapData.mon << 8 | uteCwmSleepData.saveNapData.day;
    if ((uteCwmSleepData.saveNapData.recordIndex > 0))  //(systemTime == readTime) &&
    {
        bool sleepStart = false;
        for(uint8_t i=0; i<uteCwmSleepData.saveNapData.recordIndex; i++)
        {
            if(uteCwmSleepData.saveNapData.sleep_record[i].state == LIGHT_SLEEP && sleepStart == false)
            {
                sleepStart = true;
                pread->data[dataIndx].startTime = uteCwmSleepData.saveNapData.sleep_record[i].startTime;
            }
            if(uteCwmSleepData.saveNapData.sleep_record[i].state == STOP_SLEEP && sleepStart == true)
            {
                sleepStart = false;
                pread->data[dataIndx].endTime = uteCwmSleepData.saveNapData.sleep_record[i].startTime;
                uint16_t lastStatusStartTime =  pread->data[dataIndx].startTime.hour*60+pread->data[dataIndx].startTime.min;
                uint16_t currStatusStartTime =  pread->data[dataIndx].endTime.hour*60+pread->data[dataIndx].endTime.min;
                pread->data[dataIndx].min = (currStatusStartTime+24*60-lastStatusStartTime)%(24*60);
                if(pread->data[dataIndx].min<3*60)
                {
                    dataIndx += 1;
                }
            }
            if(dataIndx>=UTE_SLEEP_NAP_DATA_DISPLAY_MAX_RECORD)
            {
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_CYWEE_MOTION_LVL, "%s,dataIndx=%d", __func__, dataIndx);
        pread->recordCnt = dataIndx;
        return true;
    }
    return false;
}

/**
 *@brief       赛维开始发送睡眠数据
 *@details
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepSendCwmNapSleepHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[32];
    uint8_t response[247];
    uint16_t sendSize = 0;
    uint16_t mtuSize = uteApplicationCommonGetMtuSize();
    sendParam->dataBuffSize = sizeof(ute_sleep_data_save_t);
    ute_sleep_data_save_t *pRead = (ute_sleep_data_save_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset((void*)pRead, 0, sendParam->dataBuffSize);
    memset(&path[0], 0, 32);
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__, sendParam->currSendFileIndex, sendParam->currSendHourIndex, sendParam->dirInfo.filesCnt);

    if (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt)
    {
        void *file;
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_CWM_SLEEP_NAP_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,read file=%s", __func__, &path[0]);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,(void*)pRead, sendParam->dataBuffSize);
            uteModuleFilesystemCloseFile(file);
        }
        if (sendParam->currSendHourIndex == 0)
        {
            response[0] = CMD_CYWEE_READ_NAPSLEEP_DATA;
            response[1] = 0x01;
            response[2] = (pRead->year >> 8) & 0xff;
            response[3] = pRead->year & 0xff;
            response[4] = pRead->mon;
            response[5] = pRead->day;
            response[6] = pRead->recordIndex;
            sendSize = 7;
            uteModuleProfileBleSendToPhone(&response[0], sendSize);
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,pRead->recordIndex=%d,currSendHourIndex=%d", __func__, pRead->recordIndex, sendParam->currSendHourIndex);
    if (sendParam->currSendHourIndex < pRead->recordIndex)
    {
        response[0] = CMD_CYWEE_READ_NAPSLEEP_DATA;
        response[1] = 0x02;
        sendSize = 2;
        for (; sendParam->currSendHourIndex < pRead->recordIndex; sendParam->currSendHourIndex++)
        {
            if ((sendSize + 6) > mtuSize)
            {
                break;
            }
            response[sendSize] = pRead->sleep_record[sendParam->currSendHourIndex].startTime.hour;
            response[sendSize + 1] = pRead->sleep_record[sendParam->currSendHourIndex].startTime.min;
            response[sendSize + 2] = pRead->sleep_record[sendParam->currSendHourIndex].state;
            response[sendSize + 3] = pRead->sleep_record[sendParam->currSendHourIndex].sleepFlag;
            response[sendSize + 4] = pRead->sleep_record[sendParam->currSendHourIndex].period >> 8 & 0xff;
            response[sendSize + 5] = pRead->sleep_record[sendParam->currSendHourIndex].period & 0xff;
            sendSize += 6;
        }
        uteModuleProfileBle50SendToPhone(&response[0], sendSize);
    }
    if (sendParam->currSendHourIndex == pRead->recordIndex)
    {
        if (sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt)
        {
            uteApplicationCommonSyncDataTimerStop();
            response[0] = CMD_CYWEE_READ_NAPSLEEP_DATA;
            response[1] = 0x03;
            sendSize = 2;
            uteModuleProfileBleSendToPhone(&response[0], sendSize);
        }
        else
        {
            sendParam->currSendHourIndex = 0;
            sendParam->currSendFileIndex++;
        }
    }
    uteModulePlatformMemoryFree((void*)pRead);
}

/**
 *@brief        准备开始发送睡眠数据
 *@details
 *@param[in]  需要同步的数据时间戳
 *@author       zn.zeng
 *@date       2021-10-26
 */
void uteModuleSleepStartSendCwmNapSleepHistoryData(ute_module_systemtime_time_t time)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);

    param->crc = 0;
    param->time = time;
    if (param->time.year == 0)
    {
        param->isAllData = true;
    }
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CWM_SLEEP_NAP_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSleepSendCwmNapSleepHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_SLEEP_LVL, "%s,", __func__);
}

#endif
#endif
