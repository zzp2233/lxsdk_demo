/**
 *@file
 *@brief     在线表盘模块
 *@details
 *@author        casen
 *@date        2021-11-26
 *@version       v1.0
 */
#include "ute_module_heart.h"
#include "ute_module_systemtime.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_watchonline.h"
#include "ute_module_protocol.h"
#include "ute_module_crc.h"
#include "ute_module_gui_common.h"
#include "ute_module_platform.h"
#include "ute_module_call.h"
#include "ute_module_sport.h"
#include "ute_module_message.h"
#include "include.h"

#if UTE_MODULE_WATCHONLINE_SUPPORT

const uint32_t uteModuleWatchOnlineMultipleBaseAddress[UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT] = UTE_MODULE_WATCHONLINE_MULTIPLE_BASE_ADDRESS_ARRAYS;

extern void func_clock_recreate_dial(void);

/*! 在线表盘的数据 casen, 2021-11-27  */
ute_module_watchonline_data_t uteModuleWatchOnlineData =
{
    .isStartReceive = false,
    .writeWatchIndex = 0,
};

/*! 接收单笔数据超时指针, 2021-11-27  */
void *uteModuleWatchOnlineRecvTimeoutTimerPointer = NULL;
void *uteModuleWatchOnlineOneSecTimerPointer = NULL;

/**
 * @brief        获取在线表盘地址
 * @details
 * @return       在线表盘地址
 * @author       Wang.Luo
 * @date         2024-11-26
 */
uint32_t uteModuleWatchOnlineGetBaseAddress(uint8_t index)
{
    if (index > sizeof(uteModuleWatchOnlineMultipleBaseAddress) / sizeof(uint32_t) - 1)
    {
        return 0;
    }
    else
    {
        return uteModuleWatchOnlineMultipleBaseAddress[index];
    }
}

/**
 *@brief      在线表盘总大小
 *@details    获取当前同步的在线表盘总的大小
 *@return     online watch totile size
 *@author     ellison
 *@date       2022-06-07
 */
uint32_t uteModuleWatchOnlineGetTotileWatchSize(void)
{
    int watchConfigSize = sizeof(watchConfig_t);
    watchConfig_t watchConfig;
    uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex], watchConfigSize);
    return watchConfig.fileSize;
}

/**
 *@brief      在线表盘同步时的大小
 *@details    获取当前同步的在线表盘已经同步了多少
 *@return     online watch updateing size
 *@author     ellison
 *@date       2022-06-07
 */
uint32_t uteModuleWatchOnlineGetSynchronizeWatchSize(void)
{
    return uteModuleWatchOnlineData.fileSize;
}

/**
 *@brief      在线表盘定时器回调
 *@details
 *@author     casen
 *@date       2021-11-27
 */
void uteModuleWatchOnlineRecvTimeoutTimerCallback(void *pxTimer)
{
    ute_ble_connect_state_t state;
    uteApplicationCommonGetBleConnectionState(&state);
    if (state.isConnected == true)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_WATCHONLINE_RECVTIMEOUT_TIMER, 0);
    }
    else
    {
        uteModulePlatformStopTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
    }
}

void uteModuleWatchOnlineOneSecCallback(void *pxTimer)
{
    ute_ble_connect_state_t state;
    uteApplicationCommonGetBleConnectionState(&state);
    if (state.isConnected == true)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_WATCHONLINE_ONESEC_TIMER, 0);
    }
    else
    {
        uteModulePlatformStopTimer(&uteModuleWatchOnlineOneSecTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineOneSecTimerPointer);
    }
}

void uteModuleWatchOnlineRecvTimeoutMsgHandler(void)
{
    uint8_t response[20];
    uteModuleWatchOnlineData.receiveTimeout = 1; // Casen add 21-04-23
    response[0] = CMD_WATCH_ONLINE;
    response[1] = 0x03;
    response[2] = WATCH_ERR_NOT_CONTINUOUS;
    response[3] = (uteModuleWatchOnlineData.lastReceiveIndex + 1) >> 8 & 0xff;
    response[4] = (uteModuleWatchOnlineData.lastReceiveIndex + 1) & 0xff;
    if (uteModuleWatchOnlineData.isStartReceive == true)
    {
        uteModuleProfileBleSendToPhone(&response[0], 5);
    }
}

void uteModuleWatchOnlineOneSecMsgHandler(void)
{
    if (uteModuleWatchOnlineData.receiveTimeout != 0)
    {
        uint8_t response[3] = {};
        uteModuleWatchOnlineData.receiveTimeout += 1;
        if (uteModuleWatchOnlineData.receiveTimeout == 4)
        {
            uteModulePlatformStopTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
            response[0] = CMD_WATCH_ONLINE;
            response[1] = 0x03;
            response[2] = WATCH_ERR_CONTINUOUS;
            uteModuleProfileBleSendToPhone(&response[0], 3);
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,WATCH_ERR_CONTINUOUS", __func__);
        }
        else if (uteModuleWatchOnlineData.receiveTimeout >= 6)
        {
            uteModuleWatchOnlineData.isStartReceive = false;
            response[0] = CMD_WATCH_ONLINE;

            response[1] = 0x03;
            response[2] = WATCH_ERR_CRC;
            uteModuleProfileBleSendToPhone(&response[0], 3);
            uteModuleWatchOnlineUpddateDefaultWatchIndex();
#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
            /**同步表盘超时时候，出现超时，则退出表盘界面，防止APP同步表盘时退出未被杀死，导致卡在表盘同步界面**/
            //            if((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_CALL_INCOMING_ID ))
            //            {
            //                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_CALL_INCOMING_ID);
            //            }
            //            else if((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_CALL_OUTGOING_ID ))
            //            {
            //                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_CALL_OUTGOING_ID);
            //            }
            //            else if((uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_CALL_ING_ID ))
            //            {
            //                uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_CALL_ING_ID);
            //            }
            //            else
            //            {
            uteModuleGuiCommonGoBackLastScreen();
//            }
#endif
            uteModuleWatchOnlineData.receiveTimeout = 0;
        }
    }
    else
    {
        uteModulePlatformStopTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
        uteModulePlatformStopTimer(&uteModuleWatchOnlineOneSecTimerPointer);
        uteModulePlatformDeleteTimer(&uteModuleWatchOnlineOneSecTimerPointer);
        uteModuleWatchOnlineRecvTimeoutTimerPointer = NULL;
        uteModuleWatchOnlineOneSecTimerPointer = NULL;
    }
}
/**
 *@brief        多个在线表盘配置
 *@details      更新表盘信息
 *@author       zn.zeng
 *@date       2022-06-28
 */
void uteModuleWatchOnlineUpateConfigFromFlash(void)
{
    uint8_t addressCnt = sizeof(uteModuleWatchOnlineMultipleBaseAddress) / sizeof(uint32_t);
    if (addressCnt > UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        uteModuleWatchOnlineData.supportMultipleMaxCnt = UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT;
    }
    else
    {
        uteModuleWatchOnlineData.supportMultipleMaxCnt = addressCnt;
    }
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,supportMultipleMaxCnt=%d\r\n", __func__, uteModuleWatchOnlineData.supportMultipleMaxCnt);
    int watchConfigSize = sizeof(watchConfig_t);
    // int picConfigSize = sizeof(watchConfig_t);
    uteModuleWatchOnlineData.validPicConfigCnt = 0;
    watchConfig_t config;
    uint8_t vailWatch = 0;
    bool isConnectOurApp = uteApplicationCommonIsHasConnectOurApp();
    for (uint8_t i = 0; i < uteModuleWatchOnlineData.supportMultipleMaxCnt; i++)
    {
        uteModulePlatformFlashNorRead((uint8_t *)&config, uteModuleWatchOnlineMultipleBaseAddress[i], watchConfigSize);
        uint16_t headerNum = bsp_uitool_header_phrase(uteModuleWatchOnlineMultipleBaseAddress[i]);
        if (config.isWatchVaild == 0 && headerNum)
        {
            if (isConnectOurApp)
            {
                uteModuleWatchOnlineData.multipleWatchIndexBuff[vailWatch] = i;
                vailWatch++;
            }
            else
            {
                uteModulePlatformFlashNorErase(uteModuleWatchOnlineMultipleBaseAddress[i]);
            }
        }
    }
    uteModuleWatchOnlineData.multipleValidWatchCnt = vailWatch;
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,multipleValidWatchCnt=%d\r\n", __func__, uteModuleWatchOnlineData.multipleValidWatchCnt);
    if (!isConnectOurApp)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,is not connect our app!\r\n", __func__);
    }
    uint8_t watchIndex = 0;
    uteModuleGuiCommonGetCurrWatchIndex(&watchIndex);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,watchIndex=%d\r\n", __func__, watchIndex);
    if (uteModuleWatchOnlineData.multipleValidWatchCnt == 0)
    {
        if (watchIndex >= UTE_MODULE_SCREENS_WATCH_CNT_MAX)
        {
            printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy\r\n");
            uteModuleGuiCommonSetCurrWatchIndex(DEFAULT_WATCH_INDEX);
        }
    }
    else
    {
        if (watchIndex >= UTE_MODULE_SCREENS_WATCH_CNT_MAX)
        {
            uteModuleWatchOnlineData.currDisplayIndex = uteModuleWatchOnlineData.multipleWatchIndexBuff[watchIndex - UTE_MODULE_SCREENS_WATCH_CNT_MAX];
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,currDisplayIndex=%d\r\n", __func__, uteModuleWatchOnlineData.currDisplayIndex);
            uteModulePlatformFlashNorRead((uint8_t *)&uteModuleWatchOnlineData.watchConfig, uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.currDisplayIndex], watchConfigSize);
        }
    }
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,validPicConfigCnt=%d\r\n", __func__, uteModuleWatchOnlineData.validPicConfigCnt);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,validPicConfigCnt=%d,hasbg=%d\r\n", __func__, uteModuleWatchOnlineData.validPicConfigCnt, uteModuleWatchOnlineData.watchConfig.hasBg);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,snNo=%d,pixelWidth=%d,pixelHeight=%d,filesize=%d\r\n", __func__, uteModuleWatchOnlineData.watchConfig.snNo, uteModuleWatchOnlineData.watchConfig.pixelWidth, uteModuleWatchOnlineData.watchConfig.pixelHeight, uteModuleWatchOnlineData.watchConfig.fileSize);
}

void uteModuleWatchOnlineReadDeviceInfo(uint8_t *data)
{
    if (uteModuleWatchOnlineData.multipleValidWatchCnt)
    {
        int watchConfigSize = sizeof(watchConfig_t);
        uteModulePlatformFlashNorRead((uint8_t *)&uteModuleWatchOnlineData.watchConfig, uteModuleWatchOnlineMultipleBaseAddress[0], watchConfigSize);
        data[0] = (uteModuleWatchOnlineData.watchConfig.snNo >> 24) & 0xff;
        data[1] = (uteModuleWatchOnlineData.watchConfig.snNo >> 16) & 0xff;
        data[2] = (uteModuleWatchOnlineData.watchConfig.snNo >> 8) & 0xff;
        data[3] = uteModuleWatchOnlineData.watchConfig.snNo & 0xff;
    }
    else
    {
        data[0] = 0xff;
        data[1] = 0xff;
        data[2] = 0xff;
        data[3] = 0xff;
    }
    data[4] = (UTE_DRV_SCREEN_WIDTH >> 8) & 0xff;
    data[5] = UTE_DRV_SCREEN_WIDTH & 0xff;
#if UTE_DRV_SCREEN_WATCH_ONLINE_HEIGHT_HAND
    data[6] = ((UTE_DRV_SCREEN_HEIGHT - 1) >> 8) & 0xff;
    data[7] = (UTE_DRV_SCREEN_HEIGHT - 1) & 0xff;
#else
    data[6] = (UTE_DRV_SCREEN_HEIGHT >> 8) & 0xff;
    data[7] = UTE_DRV_SCREEN_HEIGHT & 0xff;
#endif

    data[8] = UTE_DRV_SCREEN_SHAPE;
    data[9] = (UTE_MODULE_WATCHONLINE_MAX_SIZE >> 24) & 0xff;
    data[10] = (UTE_MODULE_WATCHONLINE_MAX_SIZE >> 16) & 0xff;
    data[11] = (UTE_MODULE_WATCHONLINE_MAX_SIZE >> 8) & 0xff;
    data[12] = UTE_MODULE_WATCHONLINE_MAX_SIZE & 0xff;
    data[13] = UTE_MODULE_WATCHONLINNE_COMPATIBLE_LEVEL;
    data[14] = UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT;
#if UTE_MODULE_WATCHONLINNE_R_VALUE
    data[15] = UTE_MODULE_WATCHONLINNE_R_VALUE;
#endif
}

void uteModuleWatchOnlineReadyStart(void)
{
    if (uteModuleWatchOnlineRecvTimeoutTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer, "WatchOnlineRecvTimeoutTimer", 1, 300, true, uteModuleWatchOnlineRecvTimeoutTimerCallback);
    }
    if (uteModuleWatchOnlineOneSecTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleWatchOnlineOneSecTimerPointer, "WatchOnlineOneSecTimer", 2, 1000, true, uteModuleWatchOnlineOneSecCallback);
    }
    if (uteModuleWatchOnlineOneSecTimerPointer != NULL)
    {
        uteModulePlatformRestartTimer(&uteModuleWatchOnlineOneSecTimerPointer, 1000);
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.writeWatchIndex=%d,supportMultipleMaxCnt=%d", __func__, uteModuleWatchOnlineData.writeWatchIndex, uteModuleWatchOnlineData.supportMultipleMaxCnt);
    uteModuleWatchOnlineData.receiveCnt = 0;
    uteModuleWatchOnlineData.fileCrc32 = 0xffffffff;
    uteModuleWatchOnlineData.isStartReceive = true;
    uteModuleWatchOnlineData.fileSize = 0;
    uteModuleWatchOnlineData.isStartCrc32 = false;
    uteModuleWatchOnlineData.lastReceiveIndex = -1;
    uteModuleWatchOnlineData.lastEraseAddress = uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex];
    if (uteModuleWatchOnlineData.multipleValidWatchCnt)
        uteModuleWatchOnlineData.multipleValidWatchCnt--; // cxd add 2022-08-24
    uteModuleWatchOnlineData.receiveTimeout = 1;          // Casen add 2020-10-26
    bool isUpdateWatch = uteModuleWatchOnlineUpddateDefaultWatchIndex();

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,need erase lastEraseAddress=0x%x\r\n", __func__, uteModuleWatchOnlineData.lastEraseAddress);

    // 先跳转到主表盘界面
#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
    // if(!uteModuleGuiCommonIsDisplayOn())


    {
        printf("UTE_MOUDLE_SCREENS_SYNC_WATCH_ONLINE_ID\r\n");
        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SYNC_WATCH_ONLINE_ID, 0, __func__);
    }
#else
    if (isUpdateWatch || !uteModuleGuiCommonIsDisplayOn())
    {
        if (uteModuleGuiCommonGetCurrentScreenId() != UTE_MOUDLE_SCREENS_WATCHMAIN_ID)
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
        }
        else
        {
            func_clock_recreate_dial();
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = true;
            }
            reset_sleep_delay_all();
        }
    }
#endif
    // Casen add 2020-02-24
    uteModulePlatformFlashNorErase(uteModuleWatchOnlineData.lastEraseAddress);
}

uint8_t uteModuleWatchOnlineDataWrite(const uint8_t *data, uint32_t size)
{
    uint8_t wConfigSize = sizeof(watchConfig_t);
    uint32_t writeAddress = uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex] + uteModuleWatchOnlineData.fileSize;
    uint32_t eraseAddress;
    eraseAddress = ((writeAddress + size) / 0x1000) * 0x1000;
    if (eraseAddress >= (uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex] + UTE_MODULE_WATCHONLINE_MAX_SIZE))
    {
        uteModuleWatchOnlineData.isStartReceive = false;
        uteModuleWatchOnlineUpddateDefaultWatchIndex();
        return WATCH_ERR_FILE_TOO_LARGE;
    }
    uteModuleWatchOnlineData.receiveCnt++;
    // write to flash
    if (eraseAddress != uteModuleWatchOnlineData.lastEraseAddress)
    {
        uint8_t response[10];
        response[0] = CMD_WATCH_ONLINE;
        response[1] = 0x03;
        response[2] = WATCH_ERR_CONTINUOUS;
        uteModuleWatchOnlineData.lastEraseAddress = eraseAddress;
        uteModulePlatformFlashNorErase(uteModuleWatchOnlineData.lastEraseAddress);
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,need erase lastEraseAddress=0x%x\r\n", __func__, uteModuleWatchOnlineData.lastEraseAddress);
        uteModuleProfileBleSendToPhone(&response[0], 3);
    }
    uteModulePlatformFlashNorWrite((uint8_t *)(&data[0]), writeAddress, size);
    uteModuleWatchOnlineData.fileSize += size;
    // write to flash
    //  crc
    if (uteModuleWatchOnlineData.fileSize > wConfigSize)
    {
        if (uteModuleWatchOnlineData.isStartCrc32)
        {
            uteModuleCrc32Bit(&data[0], size, &uteModuleWatchOnlineData.fileCrc32);
        }
        else
        {
            uint8_t needCrcCnt = 0;
            uteModuleWatchOnlineData.isStartCrc32 = true;
            if ((uteApplicationCommonGetMtuSize() - 1) < wConfigSize)
            {
                needCrcCnt = uteModuleWatchOnlineData.fileSize % wConfigSize;
            }
            else if ((uteApplicationCommonGetMtuSize() - 1) == wConfigSize)
            {
                needCrcCnt = size;
            }
            else
            {
                needCrcCnt = uteModuleWatchOnlineData.fileSize - wConfigSize;
            }
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,needCrcCnt=%d,size=%d\r\n", __func__, needCrcCnt, size);
            uteModuleCrc32Bit(&data[size - needCrcCnt], needCrcCnt, &uteModuleWatchOnlineData.fileCrc32);
        }
    }
    // crc
    //    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,uteModuleWatchOnlineData.fileSize=%d\r\n", __func__,uteModuleWatchOnlineData.fileSize);
    return WATCH_ERR_OK;
}

void uteModuleWatchOnlineSyncData(uint8_t *receive, uint8_t length)
{
    uint8_t response[10];
    if (uteModuleWatchOnlineData.isStartReceive)
    {
        int index = receive[1] << 8 | receive[2];
        uteModuleWatchOnlineData.receiveTimeout = 1; // Casen add 2020-10-26
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "uteModuleWatchOnlineData.lastReceiveIndex=%d,index=%d", uteModuleWatchOnlineData.lastReceiveIndex, index);
        if ((index - uteModuleWatchOnlineData.lastReceiveIndex) != 1)
        {
            uteModulePlatformRestartTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer, 300);
            return;
        }
        uteModulePlatformStopTimer(&uteModuleWatchOnlineRecvTimeoutTimerPointer);
        uteModuleWatchOnlineData.lastReceiveIndex = index;
        if (uteModuleWatchOnlineDataWrite(&receive[3], length - 3) == WATCH_ERR_FILE_TOO_LARGE)
        {
            response[0] = CMD_WATCH_ONLINE;
            response[1] = 0x03;
            response[2] = WATCH_ERR_FILE_TOO_LARGE;
            uteModuleProfileBleSendToPhone(&response[0], 3);
        }
#if 0 // UTE_BT30_CALL_SUPPORT //&& (UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT == false)
        ute_bt_call_data_t callData;
        uteModuleCallGetData(&callData);
        if((uteModuleCallGetBeforeCallStatus() == BT_HFP_OUTGOING_CALL_ONGOING) ||\
           (uteModuleCallGetBeforeCallStatus() == BT_HFP_INCOMING_CALL_ONGOING) ||\
           (callData.state == BT_CALL_ING))
        {
            response[0] = CMD_WATCH_ONLINE;
            response[1] = 0x03;
            response[2] = WATCH_ERR_CRC;
            uteModuleProfileBleSendToPhone(&response[0],3);
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL,"%s,sync watchonline error because BT3.0 is calling!",__func__);
        }
#endif
    }
}
extern bool slider_unlock;
uint8_t uteModuleWatchOnLineTSyncComplete(void)
{
    uint8_t status = WATCH_ERR_OK;
    int watchConfigSize = sizeof(watchConfig_t);
    watchConfig_t watchConfig;
    uteModuleWatchOnlineData.fileCrc32 = uteModuleWatchOnlineData.fileCrc32 ^ 0xffffffff;
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,systemNotSaveVariable.writeWatchIndex=0x%x", __func__, uteModuleWatchOnlineData.writeWatchIndex);
    uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex], watchConfigSize);
    uteModuleWatchOnlineData.isStartReceive = false;
    uteModuleWatchOnlineData.receiveTimeout = 0; // Casen add 20-10-28
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,systemNotSaveVariable.watchParam.fileCrc32=0x%x", __func__, uteModuleWatchOnlineData.fileCrc32);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,systemNotSaveVariable.watchConfig.fileCrc32=0x%x", __func__, watchConfig.fileCrc);
    uint16_t headerNum = bsp_uitool_header_phrase(uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex]);
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,headerNum=%d", __func__, headerNum);
    if (uteModuleWatchOnlineData.fileCrc32 == watchConfig.fileCrc && headerNum > 0)
    {
        watchConfig.isWatchVaild = 0;
        uteModulePlatformFlashNorWrite((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[uteModuleWatchOnlineData.writeWatchIndex], watchConfigSize);
#if ((UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT != 1))
        uteModuleGuiCommonSetCurrWatchIndex(uteModuleWatchOnlineGetWatchindex(uteModuleWatchOnlineData.writeWatchIndex) + UTE_MODULE_SCREENS_WATCH_CNT_MAX - 1);
#else
        uteModuleGuiCommonSetCurrWatchIndex(UTE_MODULE_SCREENS_WATCH_CNT_MAX + uteModuleWatchOnlineData.writeWatchIndex);
#endif
    }
    else
    {
        uteModuleWatchOnlineUpddateDefaultWatchIndex();
        status = WATCH_ERR_CRC;
    }
    uteModuleWatchOnlineUpateConfigFromFlash();
    // if (slider_unlock == false)
    // {
    //     printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\r\n");
    //     uteModuleGuiCommonGoBackLastScreen();
    // }
    /*判断如果再运动中，则会=返回上个界面，防止运动被中断*/
#if UTE_MODULE_SPORT_SUPPORT
    if (uteModuleSportMoreSportIsRuning())
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
    else
#endif
    {
        if (uteModuleGuiCommonGetCurrentScreenId() != UTE_MOUDLE_SCREENS_WATCHMAIN_ID && slider_unlock == true)
        {
            uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_WATCHMAIN_ID, 0, __func__);
        }
        else
        {
            func_clock_recreate_dial();
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = true;
            }
            reset_sleep_delay_all();
        }
    }
    return status;
}

/**
 *@brief        获取在线表盘最大支持数
 *@details
 *@author       zn.zeng
 *@date       2022-06-28
 */
uint8_t uteModuleWatchOnlineGetSupportMultipleCnt(void)
{
    return uteModuleWatchOnlineData.supportMultipleMaxCnt;
}

/**
 *@brief        获取在线表盘有效表盘数
 *@details
 *@author       zn.zeng
 *@date       2022-06-28
 */
uint8_t uteModuleWatchOnlineGetVailWatchCnt(void)
{
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,multipleValidWatchCnt=%d", __func__, uteModuleWatchOnlineData.multipleValidWatchCnt);
    return uteModuleWatchOnlineData.multipleValidWatchCnt;
}

#if ((UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT != 1))
/**
 *@brief       获取当前表盘显示编号
 *@details
 *@param[in] uint8_t index,预览索引
 *@author     pcm
 *@date       2023-01-03
 */
uint8_t uteModuleWatchOnlineGetWatchindex(uint8_t index)
{
    uint8_t addressCnt = sizeof(uteModuleWatchOnlineMultipleBaseAddress) / sizeof(uint32_t);
    if (addressCnt > UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        uteModuleWatchOnlineData.supportMultipleMaxCnt = UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT;
    }
    else
    {
        uteModuleWatchOnlineData.supportMultipleMaxCnt = addressCnt;
    }
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,supportMultipleMaxCnt=%d\r\n", __func__, uteModuleWatchOnlineData.supportMultipleMaxCnt);
    int watchConfigSize = sizeof(watchConfig_t);
    int picConfigSize = sizeof(watchConfig_t);
    uteModuleWatchOnlineData.validPicConfigCnt = 0;
    watchConfig_t config;
    uint8_t vailWatch = 0;
    bool isConnectOurApp = uteApplicationCommonIsHasConnectOurApp();
    for (uint8_t i = 0; i < uteModuleWatchOnlineData.supportMultipleMaxCnt; i++)
    {
        uteModulePlatformFlashNorRead((uint8_t *)&config, uteModuleWatchOnlineMultipleBaseAddress[i], watchConfigSize);
        uint16_t headerNum = bsp_uitool_header_phrase(uteModuleWatchOnlineMultipleBaseAddress[i]);
        if (config.isWatchVaild == 0 && headerNum)
        {
            if (isConnectOurApp)
            {
                uteModuleWatchOnlineData.multipleWatchIndexBuff[vailWatch] = i;
                UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,multipleWatchIndexBuff=%d,vailWatch=%d\r\n", __func__, uteModuleWatchOnlineData.multipleWatchIndexBuff[vailWatch], vailWatch);
                vailWatch++;
            }
            else
            {
                uteModulePlatformFlashNorErase(uteModuleWatchOnlineMultipleBaseAddress[i]);
            }
        }
    }
    uint8_t vailWatchCnt = vailWatch;
    if (vailWatch >= 2)
    {
        for (uint8_t j = 0; j < vailWatch; j++)
        {
            if (uteModuleWatchOnlineData.multipleWatchIndexBuff[j] > index)
            {
                vailWatchCnt--;
            }
        }
        vailWatch = vailWatchCnt;
    }
    //    uteModuleWatchOnlineData.multipleValidWatchCnt = vailWatch;

    //  if (index >= UTE_MODULE_SCREENS_WATCH_CNT_MAX)
    //  {
    //    return uteModuleWatchOnlineData.multipleWatchIndexBuff[index-UTE_MODULE_SCREENS_WATCH_CNT_MAX];
    //  }else{
    UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,vailWatch=%d\r\n", __func__, vailWatch);
    return vailWatch;
    //  }
}
#endif

/**
 *@brief        跳转默认表盘
 *@details     增加判断条件
 *@return ture 为需要刷新为默认表盘
 *@author       zn.zeng
 *@date       2022-06-28
 */
bool uteModuleWatchOnlineUpddateDefaultWatchIndex(void)
{
    uint8_t watchIndex = 0;
    bool isDefault = false;
    uteModuleGuiCommonGetCurrWatchIndex(&watchIndex);
    if ((watchIndex - UTE_MODULE_SCREENS_WATCH_CNT_MAX) == uteModuleWatchOnlineData.writeWatchIndex)
    {
        printf("qwqwqwqwwwwwwwwwwwwwwwwwww\r\n");
        uteModuleGuiCommonSetCurrWatchIndex(DEFAULT_WATCH_INDEX);
        isDefault = true;
    }
    return isDefault;
}
/**
 *@brief        通过索引获取表盘信息
 *@details
 *@param[in] uint8_t index,索引
 *@param[out] uint8_t *data,
 *@author       zn.zeng
 *@date       2022-07-05
 */
void uteModuleWatchOnlineGetInfoWithIndex(uint8_t index, uint8_t *data)
{
    if (index < UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        int watchConfigSize = sizeof(watchConfig_t);
        watchConfig_t watchConfig;
        uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, uteModuleWatchOnlineMultipleBaseAddress[index], watchConfigSize);
        data[0] = (watchConfig.snNo >> 24) & 0xff;
        data[1] = (watchConfig.snNo >> 16) & 0xff;
        data[2] = (watchConfig.snNo >> 8) & 0xff;
        data[3] = watchConfig.snNo & 0xff;
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,error the index=%d is too large\r\n", __func__, index);
    }
}
/**
 *@brief        设置将要更新的表盘数据序号
 *@details
 *@param[in] uint8_t index,索引
 *@author       zn.zeng
 *@date       2022-07-05
 */
void uteModuleWatchOnlineSetWillUpdateDataIndex(uint8_t index)
{
    if (index < UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT)
    {
        uteModuleWatchOnlineData.writeWatchIndex = index;
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,error the index=%d is too large\r\n", __func__, index);
    }
}
#endif // UTE_MODULE_WATCHONLINE_SUPPORT
