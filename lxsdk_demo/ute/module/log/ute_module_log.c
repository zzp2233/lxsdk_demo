/**
*@file
*@brief     ute log 模块C文件
*@details    log 模块，打印函数转换
*@author        zn.zeng
*@date        Jun 29, 2021
*@version       v1.0
*/
#include "ute_module_log.h"

#if UTE_MODULE_LOG_SUPPORT
#if UTE_MODULE_RUNING_LOG_SUPPORT
#include "ute_module_profile_ble.h"
#include "ute_application_common.h"
uint8_t uteModuleRuningLogSwitch = false;
static uint8_t uteModuleLogRuningArray[UTE_MODULE_LOG_RUNING_MAX_BUFF_SIZE];
#endif
/** log打印的tag字符串 */
const uint8_t uteModuleLogTagString[][31] =
{
    {" "},
    {"LOG_SYSTEM"},
    {"LOG_FILESYSTEM"},
    {"LOG_KEYS"},
    {"LOG_HEART"},
    {"LOG_BP"},
    {"LOG_OXYGEN"},
    {"LOG_STEP"},
    {"LOG_PROTOCOL"},
    {"LOG_NOTIFY"},
    {"LOG_WEATHER"},
    {"LOG_BREATHRATE"},
    {"LOG_GUI"},
    {"LOG_DRV_SCREEN"},
    {"LOG_DRV_TP"},
    {"LOG_DRV_MOTOR"},
    {"LOG_SLEEP"},
    {"LOG_BT_AUDIO"},
    {"LOG_DRV_BAT"},
    {"LOG_MUSIC"},
    {"LOG_WATCHONLINE"},
    {"LOG_TEMPERATURE"},
    {"LOG_CALL"},
    {"LOG_MICRECORD"},
    {"LOG_TIME"},
    {"LOG_EMOTION_PRESSURE"},
    {"LOG_DRV_PSRAM"},
    {"LOG_EARPHONE"},
    {"LOG_ALI_UPAY"},
    {"LOG_CWM"},
    {"LOG_MEMORY"},
};
#if UTE_MODULE_RUNING_LOG_SUPPORT
static uint8_t writeLogTobuff(uint8_t *pLogData, uint8_t logLen);
static uint8_t readLogFormBuff(uint8_t *pData);
static void uteModuleLogBleSend(void);

static uint32_t sendTimes = 0;

void uteModuleLogSetSendRuningLogSwitch(bool status)
{
    UTE_MODULE_LOG(1,"%s LogSwitch = %d ,status = %d",__func__,uteModuleRuningLogSwitch,status);

    if (status)
    {
        le_update_conn_param(0, 12, 36, 0,500, 12 * 2 - 2, 36 * 2 - 2);
        uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleLogBleSend);
        uteApplicationCommonSyncDataTimerStart();
    }
    else
    {
        uteApplicationCommonSyncDataTimerStop();
    }
    uteModuleRuningLogSwitch = status;
}

void uteModuleLogRuningPrintf(uint8_t log_lvl, const char * format,...)
{
    if ((log_lvl != 0) && (log_lvl < 30))
    {
        va_list    args;

        va_start(args, format);

        vsprintf((char *)&uteModuleLogRuningArray[0], format, args);

        va_end(args);

        if(uteModuleRuningLogSwitch)
        {
            if(uteApplicationCommonGetHasUpdateConnectionParam() == false)
            {
                le_update_conn_param(0, 12, 36, 0,500, 12 * 2 - 2, 36 * 2 - 2);
                UTE_MODULE_LOG(1,"le_update_conn_param");
            }
            uteApplicationCommonSetHasUpdateConnectionParam(true); //调整蓝牙连接间隔，处理丢包问题
            //加入缓存
            if(strstr((char *)&uteModuleLogRuningArray[0],"IDX(0)") ==0)  //过滤 IDX_ACCEL 的日志
            {
                writeLogTobuff(uteModuleLogRuningArray, strlen((char *)&uteModuleLogRuningArray[0]));
            }
        }
        else
        {
            UTE_MODULE_LOG(log_lvl, "runingLog %s", &uteModuleLogRuningArray[0]);
        }
    }
}

void uteModuleLogBleSend(void)
{
    while(uteModulePlatformIsAllowBleSend())
    {
        strncpy((char *)uteModuleLogRuningArray, "\xFE\x08\x02",3);
        uint8_t logLen = readLogFormBuff(&uteModuleLogRuningArray[4]);
        if(logLen > 200-4)      //200是协议中设定的单条数据最大长度
        {
            logLen = 200-4;
        }
        uteModuleLogRuningArray[3] = logLen+4;
        if(logLen==0)
        {
            break;
        }
        //发送数据
        uteModuleProfileBle50SendToPhone(uteModuleLogRuningArray,uteModuleLogRuningArray[3]);
        sendTimes++;
    }

}

#define BLE_LOG_BUFF_SIZE     6*1024
static uint8_t blelogbuff[BLE_LOG_BUFF_SIZE];//log缓存区
static uint8_t *writebuff = &blelogbuff[0];
static uint8_t *readbuff = &blelogbuff[0];
static uint8_t bleSaveLogCount = 0;
uint8_t writeLogTobuff(uint8_t *pLogData, uint8_t logLen)
{
    if(writebuff+logLen+2 >= &blelogbuff[0]+BLE_LOG_BUFF_SIZE)    //如果这一次写会刚好填满缓存或溢出
    {
        writebuff[0] = 0;//长度给0，表示这一条数据回到开头写
        writebuff = &blelogbuff[0];
    }
    if(((uint32_t)writebuff+BLE_LOG_BUFF_SIZE-(uint32_t)readbuff)%(BLE_LOG_BUFF_SIZE)+logLen+2 < BLE_LOG_BUFF_SIZE)    //判断剩余空间是否足够
    {
        if(logLen > (0xFF-2))
        {
            logLen = 0xFF-2;
        }
        writebuff[0] = logLen+2;
        memcpy(&writebuff[1], pLogData, logLen);
        writebuff[1+logLen] = '\0';
        writebuff+=(2+logLen);
        bleSaveLogCount++;
    }
    else    //剩余空间不足
    {
        UTE_MODULE_LOG(1, "error: ble log bufffer overflower,w:%p,r:%p,len:%d,sendtimes:%d", writebuff,readbuff, logLen+2,sendTimes);
        bleSaveLogCount = 0;//已加入缓存数据条数清零
        writebuff = &blelogbuff[0];
        readbuff = &blelogbuff[0];
        uint8_t overFlowerChar[] = "error: ble log bufffer overflower";
        writeLogTobuff(overFlowerChar, sizeof(overFlowerChar));
    }
    return 0;
}
uint8_t readLogFormBuff(uint8_t *pData)
{
    if(!pData || bleSaveLogCount==0)
    {
        return 0;
    }
    if(readbuff[0] == 0)    //长度为零，表示这一条数据需要回到开头读
    {
        readbuff = &blelogbuff[0];
        if(readbuff[0] == 0)
        {
            return 0;
        }
    }
    uint8_t dataLen = readbuff[0]-2;
    memcpy(pData, &readbuff[1],dataLen);
    readbuff+=readbuff[0];
    bleSaveLogCount--;
    return dataLen;
}

#endif

#endif /* UTE_MODULE_LOG_SUPPORT */




