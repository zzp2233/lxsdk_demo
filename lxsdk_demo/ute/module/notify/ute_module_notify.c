/**
*@file
*@brief        消息提醒处理模块
*@details
*@author       zn.zeng
*@date       2021-08-24
*@version      v1.0
*/

#include "ute_module_notify.h"
#include "ute_module_log.h"
#include "ute_module_filesystem.h"
#include "ute_module_call.h"
#include "ute_application_common.h"
#include "ute_drv_motor.h"
#include "ute_module_message.h"
#include "ute_module_charencode.h"
#include "ute_module_notdisturb.h"
#include "func_cover.h"
#include "ute_module_platform.h"

// #include "ute_module_screens_common.h"

// #include "ute_module_gui_string.h"
// #include "ute_module_quickReply.h"

/*! 消息数据zn.zeng, 2021-08-24  */
ute_module_notify_data_t uteModuleNotifyData;
void *uteModuleNotifyAncsPairTimer=NULL;
/* module nofiy 互斥量 zn.zeng 2022-02-14*/
void *uteModuleNotifyMute;

/**
*@brief     notify 读取所有消息到内存
*@details
*@author        zn.zeng
*@date        2021-11-09
*/
void uteModuleNotifyReadHistoryData(void)
{
    uint16_t buffSize = sizeof(ute_module_notify_one_data_t);
    uint8_t path[25];
    memset(&path[0],0,25);
    void *file;
    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s", __func__);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_NOTIFY_DIR, dirInfo, NULL);
    uteModulePlatformTakeMutex(uteModuleNotifyMute);
    uint8_t vailCnt = 0;
    for(int i = dirInfo->filesCnt; i>0; i--)
    {
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_NOTIFY_DIR, dirInfo->filesName[i-1]);
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,(void*)&uteModuleNotifyData.historyNotify[vailCnt],buffSize);
            uteModuleFilesystemCloseFile(file);
            vailCnt++;
        }
    }
    uteModuleNotifyData.totalNotifyCnt = vailCnt;
    uteModulePlatformGiveMutex(uteModuleNotifyMute);
    uteModulePlatformMemoryFree(dirInfo);
    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,.totalNotifyCnt=%d", __func__,uteModuleNotifyData.totalNotifyCnt);
}

/**
*@brief     ancs读取打开flag
*@details
*@author        zn.zeng
*@date        2021-11-18
*/
void uteModuleNotifyAncsReadFlag(void)
{
    void *file;
    uint8_t path[15];
    uint8_t readBuff[4];
    memset(&path[0],0,15);
    uteModuleNotifyData.ancsSetOpenFlag = 0;
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_NOTIFY_ANCS_DIR,UTE_MODULE_FILESYSTEM_NOTIFY_ANCS_FLAG);
    memset(&readBuff[0],0,4);
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file,(void*)&readBuff[0],4);
        uteModuleFilesystemCloseFile(file);
        uteModuleNotifyData.ancsSetOpenFlag = readBuff[0]<<24|readBuff[1]<<16|readBuff[2]<<8|readBuff[3];
    }
}
/**
*@brief     notify 初始化
*@details
*@author        zn.zeng
*@date        2021-08-24
*/
void uteModuleNotifyInit(void)
{
    memset(&uteModuleNotifyData,0,sizeof(ute_module_notify_data_t));
    uteModulePlatformCreateMutex(&uteModuleNotifyMute);
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_NOTIFY_DIR);
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_NOTIFY_ANCS_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_NOTIFY_DIR, NULL, NULL);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_NOTIFY_ANCS_DIR, NULL, NULL);
#endif
    uteModuleNotifyReadHistoryData();
    uteModuleNotifyAncsReadFlag();
}
/**
*@brief     挂断电话消息处理
*@details
*@author        zn.zeng
*@date        2021-08-24
*/
void uteModuleNotifyCallDisableHandlerMsg(void)
{
#if UTE_BT30_CALL_SUPPORT
    if(uteModuleCallBtIsConnected())
    {

    }
    else
#endif
    {
        if (uteModuleGuiCommonGetCurrentScreenId() == FUNC_BLE_CALL)
        {
            uteModuleGuiCommonGoBackLastScreen();
        }

        if(uteDrvMotorGetRunningStatus())
        {
            uteDrvMotorStop();
        }
        uteModuleCallClearNumberAndName();
    }
}
/**
*@brief     未接来电消息处理
*@details
*@author        zn.zeng
*@date        2021-08-24
*/
void uteModuleNotifyCallUnansweredHandlerMsg(void)
{

}
/**
*@brief     获取消息数据
*@details
*@author        zn.zeng
*@date        2021-08-24
*/
void uteModuleNotifyGetData(ute_module_notify_data_t *data)
{
    uteModulePlatformTakeMutex(uteModuleNotifyMute);
    memcpy(data,&uteModuleNotifyData,sizeof(ute_module_notify_data_t));
    uteModulePlatformGiveMutex(uteModuleNotifyMute);
}
/**
*@brief     获取消息数据指针
*@details
*@author        zn.zeng
*@date        2021-08-24
*/
void uteModuleNotifyGetDataPointer(ute_module_notify_data_t **data)
{
    *data = &uteModuleNotifyData;
}
/**
*@brief     保存当前消息推送内容
*@details
*@author        zn.zeng
*@date        2021-08-24
*/
void uteModuleNotifySaveNotifycationData(void)
{
    uint16_t buffSize = sizeof(ute_module_notify_one_data_t);
    uint8_t path[30];
    memset(&path[0],0,30);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0],"%04d%02d%02d%02d%02d%02d",time.year,time.month,time.day,time.hour,time.min,time.sec);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_NOTIFY_DIR, dirInfo, NULL);
    if((dirInfo->filesCnt>=UTE_MODULE_NOTIFY_SAVE_CNT)&&(memcmp(&path[0],&dirInfo->filesName[0][0],14)!=0))
    {
        /*! 删除最旧一条的数据zn.zeng, 2021-08-24*/
        memset(&path[0],0,30);
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_NOTIFY_DIR,&dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0],0,30);
    sprintf((char *)&path[0],"%s/%04d%02d%02d%02d%02d%02d",UTE_MODULE_FILESYSTEM_NOTIFY_DIR,time.year,time.month,time.day,time.hour,time.min,time.sec);
    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,save file=%s", __func__,&path[0]);
    /*! 保存当前数据zn.zeng, 2021-08-24  */
    void *file;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleNotifyData.currNotify.year = time.year;
        uteModuleNotifyData.currNotify.month = time.month;
        uteModuleNotifyData.currNotify.day = time.day;
        uteModuleNotifyData.currNotify.hour = time.hour;
        uteModuleNotifyData.currNotify.min = time.min;
        uteModuleNotifyData.currNotify.sec = time.sec;
        uteModuleFilesystemWriteData(file,(void*)&uteModuleNotifyData.currNotify,buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleNotifyReadHistoryData();
    uteModulePlatformMemoryFree(dirInfo);
}

/**
*@brief     消息推送处理
*@details   uincode 编码
*@author        zn.zeng
*@date        2021-08-24
*/
void uteModuleNotifyNotifycationHandlerMsg(void)
{
    if(!uteModuleNotDisturbIsAllowNotify() && uteModuleNotifyData.currNotify.type == MSG_CALL)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,is not allow notify", __func__);
        return;
    }

    if(uteModuleNotifyData.currNotify.size == 0)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,currNotify.size=%d", __func__,uteModuleNotifyData.currNotify.size);
        return;
    }

    uint8_t *utf8StrTemp = uteModulePlatformMemoryAlloc(UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
    memset(utf8StrTemp,0,UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
    uint16_t utf8StrLen = 0;

    /*! 末尾增加“...”zn.zeng, 2021-08-24  */
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,uteModuleNotifyData.currNotify.size = %d,mAXsize = %d,", __func__,uteModuleNotifyData.currNotify.size,UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
    if(ble_ancs_is_connected()) // ios消息是utf8编码，不需要处理
    {
        if(uteModuleNotifyData.currNotify.size >= UTE_NOTIFY_MSG_CONTENT_MAX_SIZE)
        {
            memcpy(&utf8StrTemp[0],&uteModuleNotifyData.currNotify.content[0],UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
            memset(&uteModuleNotifyData.currNotify.content[0],0,UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
            uteModuleCharencodeGetUtf8String(&utf8StrTemp[0],UTE_NOTIFY_MSG_CONTENT_MAX_SIZE,&uteModuleNotifyData.currNotify.content[0],&uteModuleNotifyData.currNotify.size);
        }
    }
    else
    {
        if(uteModuleNotifyData.currNotify.size>=(UTE_NOTIFY_MSG_CONTENT_MAX_SIZE-6))
        {
            memcpy(&uteModuleNotifyData.currNotify.content[(UTE_NOTIFY_MSG_CONTENT_MAX_SIZE-6)],"\x00\x2e\x00\x2e\x00\x2e",6);
            uteModuleNotifyData.currNotify.size = UTE_NOTIFY_MSG_CONTENT_MAX_SIZE;
        }
        UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,", __func__);
        UTE_MODULE_LOG_BUFF(UTE_LOG_NOTIFY_LVL,&uteModuleNotifyData.currNotify.content[0],uteModuleNotifyData.currNotify.size);

        uteModuleCharencodeUnicodeConversionUtf8(uteModuleNotifyData.currNotify.content,uteModuleNotifyData.currNotify.size,utf8StrTemp,&utf8StrLen,UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);

        memset(uteModuleNotifyData.currNotify.content,0,UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);

        if (utf8StrLen <= UTE_NOTIFY_MSG_CONTENT_MAX_SIZE)
        {
            memcpy(uteModuleNotifyData.currNotify.content,utf8StrTemp,utf8StrLen);
            uteModuleNotifyData.currNotify.size = utf8StrLen;
        }
        else
        {
            uteModuleCharencodeGetUtf8String(&utf8StrTemp[0],UTE_NOTIFY_MSG_CONTENT_MAX_SIZE,&uteModuleNotifyData.currNotify.content[0],&uteModuleNotifyData.currNotify.size);
        }

        UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL,"%s,currNotify.size=%d",__func__,uteModuleNotifyData.currNotify.size);
        UTE_MODULE_LOG_BUFF(UTE_LOG_NOTIFY_LVL,&uteModuleNotifyData.currNotify.content[0],uteModuleNotifyData.currNotify.size);
    }

    uteModulePlatformMemoryFree(utf8StrTemp);

    /*! 来电提醒不保存 zn.zeng, 2021-08-25  */
    if(uteModuleNotifyData.currNotify.type!=MSG_CALL)
    {
        uteModuleNotifySaveNotifycationData();
        uteModuleNotifyData.isNewNotify = true;
        uteModuleNotifyData.displayIndex = 0;

        if(!uteModuleNotDisturbIsAllowNotify())
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,is not allow notify", __func__);
            return;
        }

#if UTE_MODULE_GUI_TESTING_NOT_GOTO_NOTIFICATION_SCREEN_SUPPORT
        if(uteModuleGuiCommonIsDontNeedNotificationGuiScreen())
        {
            /*! 以上界面不弹出消息提醒界面，但是需要保存消息内容 ,xjc 2022-05-23*/
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,other testing,is not allow notify", __func__);
            return;
        }
#endif
        // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_PHONE_APP_NOTIFY_ID);

        //需要弹出通知界面
        //sys_cb.msg_index = MSG_COVER_WECHAT;
        sys_cb.msg_index = uteModuleNotifyData.currNotify.type;
        sys_cb.msg_tag = true;
    }
    else
    {
#if UTE_BT30_CALL_SUPPORT
        if(uteModuleCallBtIsConnected())
        {
        }
        else
#endif
        {
            uteModuleCallSetInComingNumberName(NULL,0,&uteModuleNotifyData.currNotify.content[0],uteModuleNotifyData.currNotify.size);
            uteTaskGuiStartScreen(FUNC_BLE_CALL, 0, __func__);
        }
    }
}
/**
*@brief     ancs 消息内容填入缓存
*@details
*@param[in] uint8_t *buff
*@param[in] uint16_t length
*@author        zn.zeng
*@date        2021-11-16
*/
void uteModuleNotifyAncsPushContect(uint8_t *buff,uint16_t length,bool isTitle)
{
    uint16_t size = length;
    uint8_t startIndex = 0;

    //跳过以 E2 80 开头的连续 3 字节控制字符
    if (length > 2 && buff[0] == 0xE2 && buff[1] == 0x80)
    {
        size -= 3;
        startIndex += 3;
        for (uint16_t i = 3; i + 2 < length; i += 3)
        {
            if (buff[i] == 0xE2 && buff[i + 1] == 0x80)
            {
                size -= 3;
                startIndex += 3;
            }
            else
            {
                break;
            }
        }
        if (size > 2 && buff[length - 3] == 0xE2 && buff[length - 2] == 0x80)
        {
            size -= 3;
        }
    }

    /*! 标题后面添加：连接 xjc 2022-01-12*/
    if (isTitle)
    {
        if (uteModuleNotifyData.currNotify.type == MSG_Gmail) // replace '•' with '-'
        {
            for (int i = 0; i < length - 2; i++)
            {
                if (buff[i] == 0xe2 && buff[i + 1] == 0x80 && buff[i + 2] == 0xa2)
                {
                    buff[i] = '-';
                    for (int j = i + 1; j < length - 2; j++)
                    {
                        buff[j] = buff[j + 2];
                    }
                    size -= 2;
                    break;
                }
            }
        }

        if (uteModuleNotifyData.currNotify.type != MSG_CALL)
        {
            buff[startIndex + size] = ':';
            size++;
#if UTE_ANCS_NOTIFY_TITLE_ADD_SPACE_SUPPORT
            buff[startIndex + size] = ' ';
            size++;
#endif
        }
    }

    uint16_t hasSize = UTE_NOTIFY_MSG_CONTENT_MAX_SIZE-uteModuleNotifyData.currNotify.size;
    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,currNotify.size=%d,hasSize=%d,startIndex=%d,size=%d", __func__,uteModuleNotifyData.currNotify.size,hasSize,startIndex,size);

    if(hasSize >= size)
    {
        hasSize = size;
    }

    memcpy(&uteModuleNotifyData.currNotify.content[uteModuleNotifyData.currNotify.size],&buff[startIndex],hasSize);

    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,hasSize=%d", __func__,hasSize);
    UTE_MODULE_LOG_BUFF(UTE_LOG_NOTIFY_LVL,&uteModuleNotifyData.currNotify.content[uteModuleNotifyData.currNotify.size],hasSize);
    uteModuleNotifyData.currNotify.size = uteModuleNotifyData.currNotify.size + hasSize;

    if(isTitle)
    {
        uteModuleNotifyData.currNotify.titleSize = hasSize;
    }
}
/**
*@brief     ancs保存打开flag
*@details
*@param[in] uint32_t flag
*@author        zn.zeng
*@date        2021-11-18
*/
void uteModuleNotifyAncsSaveFlag(uint32_t flag)
{
    void *file;
    uint8_t path[15];
    uint8_t writeBuff[4];
    memset(&path[0],0,15);
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_NOTIFY_ANCS_DIR,UTE_MODULE_FILESYSTEM_NOTIFY_ANCS_FLAG);
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        writeBuff[0] = (flag>>24)&0xff;
        writeBuff[1] = (flag>>16)&0xff;
        writeBuff[2] = (flag>>8)&0xff;
        writeBuff[3] = flag&0xff;
        uteModuleFilesystemWriteData(file,(void*)&writeBuff[0],4);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief     ancs设置打开flag
*@details
*@param[in] uint32_t flag
*@author        zn.zeng
*@date        2021-11-18
*/
void uteModuleNotifyAncsSetFlag(uint32_t flag)
{
    uteModuleNotifyData.ancsSetOpenFlag = flag;
    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,flag=0x%p", __func__,flag);

    if(uteModuleNotifyData.ancsSetOpenFlag)
    {
        uteModuleNotifyAncsStartPair();
    }

#if (!APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT)
    uteModuleNotifyAncsSaveFlag(flag);
#endif
}
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
/**
*@brief     ancs设置打开flag(新协议，设置的是数组)
*@details
*@param[in] uint8_t ancsAdditionalOpenFlag[18]
*@author        pcm
*@date        2022-10-27
*/
void uteModuleNotifyAncsSetAdditionalFlag(bool flag,uint8_t*receive,uint8_t length)
{
//    uteModuleNotifyData.ancsSetOpenFlag = flag;
    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,flag=0x%p", __func__,flag);

    if(flag)
    {
        for (uint8_t i = sizeof(uteModuleNotifyData.ancsAdditionalOpenFlag); i > 0; i--)
        {
            uteModuleNotifyData.ancsAdditionalOpenFlag[i-1] |= receive[i+1];
        }
    }
    else
    {
        for (uint8_t i = sizeof(uteModuleNotifyData.ancsAdditionalOpenFlag); i > 0; i--)
        {
            uteModuleNotifyData.ancsAdditionalOpenFlag[i-1] &= ~receive[i+1];
        }
    }
}
/**
*@brief     ancs设置打开flag(新协议，设置的是数组)
*@details
*@param[in] uint8_t ancsAdditionalOpenFlag[18]
*@author        pcm
*@date        2022-10-27
*/
void uteModuleNotifyAncsGetAdditionalFlag(uint8_t*receive)
{
    memcpy(&receive[0],uteModuleNotifyData.ancsAdditionalOpenFlag,sizeof(uteModuleNotifyData.ancsAdditionalOpenFlag));
}
#endif

/**
*@brief     ancs获取打开flag
*@details
*@param[in] uint32_t flag
*@author        zn.zeng
*@date        2021-11-18
*/
void uteModuleNotifyAncsGetFlag(uint32_t *flag)
{
    *flag = uteModuleNotifyData.ancsSetOpenFlag;
}

/**
*@brief     开始配对处理
*@details
*@author        zn.zeng
*@date        2021-11-18
*/
void uteModuleNotifyAncsStartPairHandlerMsg(void)
{
    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s, ancsConnected:%d,BtConnected:%d,automaticPair:%d,HasConnection:%d", __func__, ble_ancs_is_connected(), uteModuleCallBtIsConnected(), uteModuleCallBtGetAutomaticPair(), uteModuleCallIsHasConnection());
    if (!ble_ancs_is_connected() || (!uteModuleCallBtIsConnected() && uteModuleCallBtIsPowerOn()))
    {
        if (!uteModuleCallBtGetAutomaticPair())
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,ancs pair", __func__);
            ble_ancs_start();
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,one pair", __func__);
            bsp_change_bt_mac();
            ble_bt_connect();
        }
    }
}

/**
*@brief     ancs开始配对
*@details
*@author        zn.zeng
*@date        2021-11-18
*/
void uteModuleNotifyAncsStartPair(void)
{
    uteModulePlatformRestartTimer(&uteModuleNotifyAncsPairTimer,10000);
}

/**
*@brief   ancs连接配对回调函数
*@details
*@author        zn.zeng
*@date        2021-11-16
*/
void uteModuleNotifyAncsStartPairTimerCallback(void *pxTimer)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_NOTIFY_ANCS_START_PAIR,0);
}

/**
*@brief     连接断开定时器处理
*@details
*@author        zn.zeng
*@date        2021-11-18
*/
void uteModuleNotifyAncsTimerConnectHandler(bool isConnected)
{
    if(isConnected)
    {
        if(uteModuleNotifyAncsPairTimer == NULL)
        {
            uteModulePlatformCreateTimer(&uteModuleNotifyAncsPairTimer, "ancsPariTimer",1, 5000, false, uteModuleNotifyAncsStartPairTimerCallback);
        }
    }
    else
    {
        if(uteModuleNotifyAncsPairTimer != NULL)
        {
            uteModulePlatformStopTimer(&uteModuleNotifyAncsPairTimer);
            uteModulePlatformDeleteTimer(&uteModuleNotifyAncsPairTimer);
            uteModuleNotifyAncsPairTimer = NULL;
        }
    }
}

/**
*@brief     ancs 系统接口
*@details
*@param[in] uint8_t attId
*@param[in] uint8_t *buff
*@param[in] uint16_t length
*@author        zn.zeng
*@date        2021-11-16
*/
void uteModuleNotifySetAncsInfo(uint8_t attId,uint8_t *buff,uint16_t length)
{
    // if(uteModuleNotDisturbIsAllowNotify())
    if(length > 0)
    {
        UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,.attId=%d,uteModuleNotifyData.ancsSetOpenFlag=0x%p", __func__,attId,uteModuleNotifyData.ancsSetOpenFlag);

        UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL,"%s,len:%d,%s,",__func__,length,buff);
        UTE_MODULE_LOG_BUFF(UTE_LOG_NOTIFY_LVL,buff,length);

        if(attId==ANCS_ATT_TYPE_APP_NAME)
        {
            uteModuleNotifyData.ancsHasOpen = false;
            if(strstr((const char*)buff,APP_ID_SMS))// SMS
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_SMS &uteModuleNotifyData.ancsAdditionalOpenFlag[17])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[17] = ANCS_OPEN_SMS;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_SMS)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type = MSG_MMS;
            }
            else if(strstr((const char*)buff,APP_ID_CALL))
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_INCALL&uteModuleNotifyData.ancsAdditionalOpenFlag[17])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[17] = ANCS_OPEN_INCALL;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_INCALL)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
                if(sys_cb.ancs_missed_call) //未接来电不提醒
                {
                    sys_cb.ancs_missed_call = false;
                    uteModuleNotifyData.ancsHasOpen = false;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_CALL;
            }
            else if(strstr((const char*)buff,APP_ID_QQ))// qq
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_QQ &uteModuleNotifyData.ancsAdditionalOpenFlag[17])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[17] = ANCS_OPEN_QQ;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_QQ)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_QQ;
            }
            else if(strstr((const char*)buff,APP_ID_WECHAT))// wechat
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_WECHAT &uteModuleNotifyData.ancsAdditionalOpenFlag[17])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[17] = ANCS_OPEN_WECHAT;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_WECHAT)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_WECHAT;
            }
            else if(strstr((const char*)buff,APP_ID_FACEBOOK))
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_FACEBOOK&uteModuleNotifyData.ancsAdditionalOpenFlag[17])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[17] = ANCS_OPEN_FACEBOOK;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                //Facebook
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_FACEBOOK)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Facebook;
            }
            else if(strstr((const char*)buff,APP_ID_TWITTER))
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_TWITTER&uteModuleNotifyData.ancsAdditionalOpenFlag[17])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[17] = ANCS_OPEN_TWITTER;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                // Twitter
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_TWITTER)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Twitter;
            }
            else if(strstr((const char*)buff,APP_ID_WHATSAPP) && !strstr((const char*)buff,"whatsapp.WhatsAppSMB"))//
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_WHATSAPP&uteModuleNotifyData.ancsAdditionalOpenFlag[17])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[17] = ANCS_OPEN_WHATSAPP;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                // WhatsApp
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_WHATSAPP)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_WhatsApp;
            }
            else if(strstr((const char*)buff,APP_ID_MESSENGER))
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_MESSENGER &uteModuleNotifyData.ancsAdditionalOpenFlag[16])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[16] = ANCS_OPEN_MESSENGER;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                //Messenger
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_MESSENGER)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_FacebookMessenger;
            }
            else if(strstr((const char*)buff,APP_ID_LINE))
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_LINE &uteModuleNotifyData.ancsAdditionalOpenFlag[16])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[16] = ANCS_OPEN_LINE;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                //Line
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_LINE)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Line;
            }
            else if(strstr((const char*)buff,APP_ID_SKYPE)||(strstr((const char*)buff,APP_ID_SKYPE_FOREGN)))
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_SKYPE &uteModuleNotifyData.ancsAdditionalOpenFlag[16])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[16] = ANCS_OPEN_SKYPE;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                //Skype
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_SKYPE)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Skype;
            }
            else if(strstr((const char*)buff,APP_ID_HANGOUTS))//
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_HANGOUTS&uteModuleNotifyData.ancsAdditionalOpenFlag[16])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[16] = ANCS_OPEN_HANGOUTS;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                //Hangouts
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_HANGOUTS)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Hangouts;
            }
            else if(strstr((const char*)buff,APP_ID_LinkedIn))// Linkedin
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_LINKEDIN & uteModuleNotifyData.ancsAdditionalOpenFlag[16])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[16] = ANCS_OPEN_LINKEDIN;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_LINKEDIN)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Linkedin;
            }
            else if(strstr((const char*)buff,APP_ID_Instagram))// Instagram
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_INSTAGRAM &uteModuleNotifyData.ancsAdditionalOpenFlag[16])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[16] = ANCS_OPEN_INSTAGRAM;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_INSTAGRAM)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Instagram;
            }
            else if(strstr((const char*)buff,APP_ID_Viber))// Viber
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_VIBER & uteModuleNotifyData.ancsAdditionalOpenFlag[16])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[16] = ANCS_OPEN_VIBER;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_VIBER)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Viber;
            }
            else if(strstr((const char*)buff,APP_ID_KakaoTalk)) // KakaoTalk
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_KAKAOTALK &uteModuleNotifyData.ancsAdditionalOpenFlag[16])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[16] = ANCS_OPEN_KAKAOTALK;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_KAKAOTALK)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_KakaoTalk;
            }
            else if(strstr((const char*)buff,APP_ID_VKontakte)) //VKontakte
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_VKONTAKTE &uteModuleNotifyData.ancsAdditionalOpenFlag[15])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[15] = ANCS_OPEN_VKONTAKTE;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_VKONTAKTE)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_VKontakte;
            }
            else if(strstr((const char*)buff,APP_ID_Snapchat))// Snapchat
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_SNAPCHAT &uteModuleNotifyData.ancsAdditionalOpenFlag[15])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[15] = ANCS_OPEN_SNAPCHAT;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_SNAPCHAT)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Snapchat;
            }
            else if(strstr((const char*)buff,APP_ID_GooglePlus)) // Googleplus
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_GOOGLEPLUS &uteModuleNotifyData.ancsAdditionalOpenFlag[15])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[15] = ANCS_OPEN_GOOGLEPLUS;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_GOOGLEPLUS)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Googleplus;
            }
            else if(strstr((const char*)buff,APP_ID_Gmail)) //mail
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_GMAIL &uteModuleNotifyData.ancsAdditionalOpenFlag[15])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[15] = ANCS_OPEN_GMAIL;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_GMAIL)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Gmail;
            }
            else if(strstr((const char*)buff,APP_ID_Flickr))//Flickr
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_FLICKR &uteModuleNotifyData.ancsAdditionalOpenFlag[15])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[15] = ANCS_OPEN_FLICKR;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_FLICKR)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Flickr;
            }
            else if(strstr((const char*)buff,APP_ID_Tumblr))//Tumblr
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_TUMBLR &uteModuleNotifyData.ancsAdditionalOpenFlag[15])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[15] = ANCS_OPEN_TUMBLR;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_TUMBLR)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Tumblr;
            }
            else if(strstr((const char*)buff,APP_ID_Pinterest))//Pinterest
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_PINTEREST &uteModuleNotifyData.ancsAdditionalOpenFlag[15])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[15] = ANCS_OPEN_PINTEREST;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_PINTEREST)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Pinterest;
            }
            else if(strstr((const char*)buff,APP_ID_YouTube))//YouTube
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if( ANCS_OPEN_YOUTUBE &uteModuleNotifyData.ancsAdditionalOpenFlag[15])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[15] = ANCS_OPEN_YOUTUBE;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_YOUTUBE)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_YouTube;
            }
            else if(strstr((const char*)buff,APP_ID_Telegram))//telegram
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_TELEGRAM &uteModuleNotifyData.ancsAdditionalOpenFlag[14])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[14] = ANCS_OPEN_TELEGRAM;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_TELEGRAM)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Telegram;
            }
#if SOCIAL_APP_PAYTM_SUPPORT
            else if((strstr((const char*)buff,APP_ID_Paytm0))||(strstr((const char*)buff,APP_ID_Paytm1))||(strstr((const char*)buff,APP_ID_Paytm2)))//Zalo
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_PAYTM &uteModuleNotifyData.ancsAdditionalOpenFlag[14])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[14] = ANCS_OPEN_PAYTM;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(ANCS_OPEN_PAYTM&uteModuleNotifyData.ancsOpenFlag)
                {

                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type = MSG_Paytm;
            }
#endif
#if SOCIAL_APP_ZALO_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Zalo))//Zalo
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_ZALO &uteModuleNotifyData.ancsAdditionalOpenFlag[14])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[14] = ANCS_OPEN_ZALO;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_ZALO)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Zalo;
            }
#endif

#if SOCIAL_APP_IMO_SUPPORT
            else if((strstr((const char*)buff,APP_ID_imo0))||(strstr((const char*)buff,APP_ID_imo1)))//imo
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_IMO &uteModuleNotifyData.ancsAdditionalOpenFlag[14])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[14] = ANCS_OPEN_IMO;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_IMO)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_imo;
            }
#endif
#if SOCIAL_APP_TRUECALLER_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Truecaller))//Truecaller
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_TRUECALLER &uteModuleNotifyData.ancsAdditionalOpenFlag[14])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[14] = ANCS_OPEN_TRUECALLER;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_TRUECALLER)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Truecaller;
            }
#endif
#if SOCIAL_APP_MICROSOFT_TEAMS_SUPPORT
            else if(strstr((const char*)buff,APP_ID_MicrosoftTeams))//MicrosoftTeams
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_MICROSOFTTEAMS &uteModuleNotifyData.ancsAdditionalOpenFlag[14])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[14] = ANCS_OPEN_MICROSOFTTEAMS;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_MICROSOFTTEAMS)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_MicrosoftTeams;
            }
#endif

#if SOCIAL_APP_OUTLOOK_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Outlook))//outlook
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(ANCS_OPEN_OUTLOOK &uteModuleNotifyData.ancsAdditionalOpenFlag[14])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[14] = ANCS_OPEN_OUTLOOK;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_Outlook;
            }
#endif
#if SOCIAL_APP_SWIGGY_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Swiggy))//Swiggy
            {
                if(ANCS_OPEN_SWIGGY &uteModuleNotifyData.ancsAdditionalOpenFlag[14])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[14] = ANCS_OPEN_SWIGGY;
                    uteModuleNotifyData.ancsHasOpen = true;
                }
                uteModuleNotifyData.currNotify.type =MSG_Swiggy;
            }
#endif
#if SOCIAL_APP_ZOMATO_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Zomato))//Zomato
            {
                if(ANCS_OPEN_ZOMATO &uteModuleNotifyData.ancsAdditionalOpenFlag[13])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[13] = ANCS_OPEN_ZOMATO;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Zomato;
            }
#endif
#if SOCIAL_APP_GPAY_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Gpay))//Gpay
            {
                if(ANCS_OPEN_GPAY &uteModuleNotifyData.ancsAdditionalOpenFlag[13])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[13] = ANCS_OPEN_GPAY;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Gpay;
            }
#endif
#if SOCIAL_APP_PHONEPE_SUPPORT
            else if(strstr((const char*)buff,APP_ID_PhonePe))//PhonePe
            {
                if(ANCS_OPEN_PHONEPE &uteModuleNotifyData.ancsAdditionalOpenFlag[13])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[13] = ANCS_OPEN_PHONEPE;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_PhonePe;
            }
#endif
#if SOCIAL_APP_HOTSTAR_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Hotstar))//Hotstar
            {
                if(ANCS_OPEN_HOTSTAR &uteModuleNotifyData.ancsAdditionalOpenFlag[13])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[13] = ANCS_OPEN_HOTSTAR;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Hotstar;
            }
#endif
#if SOCIAL_APP_PRIMEVIDEO_SUPPORT
            else if(strstr((const char*)buff,APP_ID_PrimeVideo))//PrimeVideo
            {
                if(ANCS_OPEN_PRIMEVIDEO &uteModuleNotifyData.ancsAdditionalOpenFlag[13])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[13] = ANCS_OPEN_PRIMEVIDEO;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_PrimeVideo;
            }
#endif
#if SOCIAL_APP_FLIPKART_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Flipkart))//Flipkart
            {
                if(ANCS_OPEN_FLIPKART &uteModuleNotifyData.ancsAdditionalOpenFlag[13])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[13] = ANCS_OPEN_FLIPKART;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Flipkart;
            }
#endif
#if SOCIAL_APP_AMAZON_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Amazon))//Amazon
            {
                if(ANCS_OPEN_AMAZON &uteModuleNotifyData.ancsAdditionalOpenFlag[13])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[13] = ANCS_OPEN_AMAZON;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Amazon;
            }
#endif
#if SOCIAL_APP_MYNTRA_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Myntra))//Myntra
            {
                if(ANCS_OPEN_MYNTRA &uteModuleNotifyData.ancsAdditionalOpenFlag[13])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[13] = ANCS_OPEN_MYNTRA;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Myntra;
            }
#endif
#if SOCIAL_APP_NOISEAPP_SUPPORT
            else if(strstr((const char*)buff,APP_ID_NoiseApp))//NoiseApp
            {
                if(ANCS_OPEN_NOISEAPP &uteModuleNotifyData.ancsAdditionalOpenFlag[12])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[12] = ANCS_OPEN_NOISEAPP;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_NoiseApp;
            }
#endif
#if SOCIAL_APP_DAILYHUNT_SUPPORT
            else if(strstr((const char*)buff,APP_ID_DailyHunt))//DailyHunt
            {
                if(ANCS_OPEN_DAILYHUNT &uteModuleNotifyData.ancsAdditionalOpenFlag[12])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[12] = ANCS_OPEN_DAILYHUNT;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_DailyHunt;
            }
#endif
#if SOCIAL_APP_INSHORTS_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Inshorts))//Inshorts
            {
                if(ANCS_OPEN_INSHORTS &uteModuleNotifyData.ancsAdditionalOpenFlag[12])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[12] = ANCS_OPEN_INSHORTS;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Inshorts;
            }
#endif
#if SOCIAL_APP_BOOKMYSHOW_SUPPORT
            else if(strstr((const char*)buff,APP_ID_BookMyShow))//BookMyShow
            {
                if(ANCS_OPEN_BOOKMYSHOW &uteModuleNotifyData.ancsAdditionalOpenFlag[12])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[12] = ANCS_OPEN_BOOKMYSHOW;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_BookMyShow;
            }
#endif
#if SOCIAL_APP_CALENDAR_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Calendar))//APP_ID_Calendar
            {
                if(ANCS_OPEN_CALENDAR &uteModuleNotifyData.ancsAdditionalOpenFlag[12])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[12] = ANCS_OPEN_CALENDAR;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Calendar;
            }
#endif
#if SOCIAL_APP_JIOTV_SUPPORT
            else if(strstr((const char*)buff,APP_ID_JioTv))//APP_ID_JioTv
            {
                if(ANCS_OPEN_JIOTV &uteModuleNotifyData.ancsAdditionalOpenFlag[12])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[12] = ANCS_OPEN_JIOTV;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_JioTv;
            }
#endif
#if SOCIAL_APP_MAKEMYTRIP_SUPPORT
            else if(strstr((const char*)buff,APP_ID_MakeMyTrip))//APP_ID_MakeMyTrip
            {
                if(ANCS_OPEN_MAKEMYTRIP &uteModuleNotifyData.ancsAdditionalOpenFlag[12])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[12] = ANCS_OPEN_MAKEMYTRIP;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_MakeMyTrip;
            }
#endif
#if SOCIAL_APP_NETFLIX_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Netflix))//APP_ID_Netflix
            {
                if(ANCS_OPEN_NETFLIX &uteModuleNotifyData.ancsAdditionalOpenFlag[12])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[12] = ANCS_OPEN_NETFLIX;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Netflix;
            }
#endif
#if SOCIAL_APP_OLA_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Ola))//APP_ID_Ola
            {
                if(ANCS_OPEN_OLA &uteModuleNotifyData.ancsAdditionalOpenFlag[11])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[11] = ANCS_OPEN_OLA;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Ola;
            }
#endif
#if SOCIAL_APP_REFLEXAPP_SUPPORT
            else if(strstr((const char*)buff,APP_ID_ReflexApp))//APP_ID_ReflexApp
            {
                if(ANCS_OPEN_REFLEXAPP &uteModuleNotifyData.ancsAdditionalOpenFlag[11])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[11] = ANCS_OPEN_REFLEXAPP;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_ReflexApp;
            }
#endif
#if SOCIAL_APP_UBER_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Uber))//MSG_Uber
            {
                if(ANCS_OPEN_UBER &uteModuleNotifyData.ancsAdditionalOpenFlag[11])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[11] = ANCS_OPEN_UBER;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Uber;
            }
#endif
#if SOCIAL_APP_YTMUSIC_SUPPORT
            else if(strstr((const char*)buff,APP_ID_YTMusic))//APP_ID_YTMusic
            {
                if(ANCS_OPEN_YTMUSIC &uteModuleNotifyData.ancsAdditionalOpenFlag[11])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[11] = ANCS_OPEN_YTMUSIC;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_YTMusic;
            }
#endif
#if SOCIAL_APP_WHATSAPP_BUSINESS_SUPPORT
            else if(strstr((const char*)buff,APP_ID_WhatsApp_Business))//APP_ID_WhatsApp_Business
            {
                if(ANCS_OPEN_WHATSAPP_BUSINESS &uteModuleNotifyData.ancsAdditionalOpenFlag[11])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[11] = ANCS_OPEN_WHATSAPP_BUSINESS;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_WhatsApp_Business;
            }
#endif
#if SOCIAL_APP_DUNZO_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Dunzo))//APP_ID_Dunzo
            {
                if(ANCS_OPEN_DUNZO &uteModuleNotifyData.ancsAdditionalOpenFlag[11])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[11] = ANCS_OPEN_DUNZO;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Dunzo;
            }
#endif
#if SOCIAL_APP_GAANA_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Gaana))//APP_ID_Gaana
            {
                if(ANCS_OPEN_GAANA &uteModuleNotifyData.ancsAdditionalOpenFlag[11])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[11] = ANCS_OPEN_GAANA;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Gaana;
            }
#endif
#if SOCIAL_APP_GOOGLE_DRIVE_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Google_Drive))//APP_ID_Google_Drive
            {
                if(ANCS_OPEN_GOOGLE_DRIVE &uteModuleNotifyData.ancsAdditionalOpenFlag[11])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[11] = ANCS_OPEN_GOOGLE_DRIVE;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Google_Drive;
            }
#endif
#if SOCIAL_APP_GOOGLECHAT_SUPPORT
            else if(strstr((const char*)buff,APP_ID_googlechat))//APP_ID_googlechat
            {
                if(ANCS_OPEN_GOOGLECHAT &uteModuleNotifyData.ancsAdditionalOpenFlag[10])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[10] = ANCS_OPEN_GOOGLECHAT;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_googlechat;
            }
#endif
#if SOCIAL_APP_WYNKMUSIC_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Wynkmusic))//APP_ID_Wynkmusic
            {
                if(ANCS_OPEN_WYNKMUSIC &uteModuleNotifyData.ancsAdditionalOpenFlag[10])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[10] = ANCS_OPEN_WYNKMUSIC;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Wynkmusic;
            }
#endif
#if SOCIAL_APP_YAHOO_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Yahoo))//APP_ID_Yahoo
            {
                if(ANCS_OPEN_YAHOO &uteModuleNotifyData.ancsAdditionalOpenFlag[10])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[10] = ANCS_OPEN_YAHOO;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Yahoo;
            }
#endif
#if SOCIAL_APP_TITANSMARTWORLD_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Yahoo))//APP_ID_Yahoo
            {
                if(ANCS_OPEN_TITANSMARTWORLD &uteModuleNotifyData.ancsAdditionalOpenFlag[10])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[10] = ANCS_OPEN_TITANSMARTWORLD;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_TitanSmartWorld;
            }
#endif
#if SOCIAL_APP_SLACK_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Slack))//APP_ID_Slack
            {
                if(ANCS_OPEN_SLACK &uteModuleNotifyData.ancsAdditionalOpenFlag[10])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[10] = ANCS_OPEN_SLACK;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Slack;
            }
#endif
#if SOCIAL_APP_SPOTIFY_SUPPORT
            else if(strstr((const char*)buff,APP_ID_Spotify))//APP_ID_Spotify
            {
                if(ANCS_OPEN_SPOTIFY &uteModuleNotifyData.ancsAdditionalOpenFlag[10])
                {
                    uteModuleNotifyData.ancsAdditionalOpenCurr[10] = ANCS_OPEN_SPOTIFY;
                    uteModuleNotifyData.ancsHasOpen = true;

                }
                uteModuleNotifyData.currNotify.type =MSG_Spotify;
            }
#endif

            else
            {
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
                if(uteModuleNotifyData.ancsAdditionalOpenFlag[17]&ANCS_OPEN_OTHER)// other
                {
                    if(ANCS_OPEN_OTHER&uteModuleNotifyData.ancsAdditionalOpenFlag[17])
                    {
                        uteModuleNotifyData.ancsAdditionalOpenCurr[17] = ANCS_OPEN_OTHER;
                        uteModuleNotifyData.ancsHasOpen = true;
                    }
                }
#else
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_OTHER)
                {
                    uteModuleNotifyData.ancsHasOpen = true;
                }
#endif
                uteModuleNotifyData.currNotify.type =MSG_OTHER;
            }
            if(uteModuleNotifyData.ancsHasOpen)
            {
                memset(&uteModuleNotifyData.currNotify.content[0],0,UTE_NOTIFY_MSG_CONTENT_MAX_SIZE);
                uteModuleNotifyData.currNotify.size = 0;
            }
        }
        else if(attId==ANCS_ATT_TYPE_TITLE)//get ANCS title
        {
            if(uteModuleNotifyData.ancsHasOpen)
            {
                uteModuleNotifyAncsPushContect(buff,length,true);
            }
        }
        else if(attId==ANCS_ATT_TYPE_MESSAGE)
        {
            if(uteModuleNotifyData.ancsHasOpen)
            {
                if(uteModuleNotifyData.ancsSetOpenFlag&ANCS_OPEN_INCALL && uteModuleNotifyData.currNotify.type == MSG_CALL)
                {
#if UTE_BT30_CALL_SUPPORT
                    if(!uteModuleCallBtIsConnected())
                    {
#if !UTE_MODULE_NOTIFY_START_MOTOR_INTO_SCREEN_SUPPORT
#if !UTE_MODULE_SCREENS_CALL_INCOMING_MOTOT_ALWAYS_ON_SUPPORT
                        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,10);
#endif
#endif
                    }
#endif
                }
                else
                {
#if !UTE_MODULE_NOTIFY_START_MOTOR_INTO_SCREEN_SUPPORT
#if UTE_MODULE_GUI_TESTING_NOT_GOTO_NOTIFICATION_SCREEN_SUPPORT
                    if(!uteModuleGuiCommonIsDontNeedNotificationGuiScreen())
                    {
                        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    }
#else
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
#endif
#endif
                    uteModuleNotifyAncsPushContect(buff,length,false);
                }
                // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_NOTIFY_NOTIFYCATTION,0);
            }
        }
        else if (attId == ANCS_ATT_TYPE_DATE)
        {
            tm_t notifyTm;
            u32 date, time;
            uint32_t notifySecCnt = 0;
            tm_t systemTm;
            uint32_t systemSecCnt = 0;
            ute_module_systemtime_time_t systemTime;
            uteModuleSystemtimeGetTime(&systemTime);

            sscanf((char*)buff, "%dT%d", &date, &time);
            notifyTm.year = date / 10000;
            notifyTm.mon  = (date % 10000) / 100;
            notifyTm.day  = date % 100;
            notifyTm.hour = time / 10000;
            notifyTm.min  = (time % 10000) / 100;
            notifyTm.sec  = time % 100;
            notifySecCnt = tm_to_time(notifyTm);

            systemTm.year = systemTime.year;
            systemTm.mon  = systemTime.month;
            systemTm.day  = systemTime.day;
            systemTm.hour = systemTime.hour;
            systemTm.min  = systemTime.min;
            systemTm.sec  = systemTime.sec;
            systemSecCnt = tm_to_time(systemTm);

            // if (ABS(systemSecCnt - notifySecCnt) < 60 * 5)
            uint32_t diff = systemSecCnt > notifySecCnt ? (systemSecCnt - notifySecCnt) : (notifySecCnt - systemSecCnt);
            if(diff < 60 * 5)
            {
                if (uteModuleNotifyData.ancsHasOpen && uteModuleNotifyData.currNotify.size > 0)
                {
                    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_NOTIFY_NOTIFYCATTION, 0);
                }
            }
            else
            {
                UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,message obsolete!!! systemSecCnt=%d, notifySecCnt=%d", __func__, systemSecCnt, notifySecCnt);
            }
        }
    }
}

/**
*@brief     设置消息显示索引
*@details
*@author        dengli.lu
*@date        2021-11-19
*/
void uteModuleNotifySetDisplayIndex(uint8_t displayIndex)
{
    uteModuleNotifyData.displayIndex = displayIndex;
}

/**
*@brief        删除历史消息数据
*@details
*@param[in] bool isDelAll,true:全部删除;false:删除当前条内容
*@author       dengli.lu
*@date       2021-11-23
*/
void uteModuleNotifyDelAllHistoryData(bool isDelAll)
{
    uint8_t path[30];
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_NOTIFY_DIR,dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,dirInfo->filesCnt = %d", __func__,dirInfo->filesCnt);
    if(isDelAll)
    {
        for(uint8_t i=0; i<dirInfo->filesCnt; i++)
        {
            memset(&path[0],0,30);
            sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_NOTIFY_DIR,&dirInfo->filesName[i][0]);
            UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,del file=%s", __func__,&path[0]);
            uteModuleFilesystemDelFile((char *)&path[0]);
        }
        uint32_t ancsSetOpenFlag = uteModuleNotifyData.ancsSetOpenFlag;
        bool ancsHasOpen = uteModuleNotifyData.ancsHasOpen;
        memset(&uteModuleNotifyData,0,sizeof(ute_module_notify_data_t));
        uteModuleNotifyData.ancsSetOpenFlag=ancsSetOpenFlag;
        uteModuleNotifyData.ancsHasOpen=ancsHasOpen;
    }
    else
    {
        memset(&path[0],0,30);
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_NOTIFY_DIR,&dirInfo->filesName[dirInfo->filesCnt - uteModuleNotifyData.displayIndex - 1][0]);
        UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,del file = %s,uteModuleNotifyData.displayIndex = %d,", __func__,&path[0],uteModuleNotifyData.displayIndex);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    uteModulePlatformMemoryFree(dirInfo);
    uteModuleNotifyReadHistoryData();
}

/**
*@brief        设置是否为新消息
*@details
*@param[in] bool isNewNotify,true:是新消息;false:不是新消息
*@author       xjc
*@date       2021-12-25
*/
void uteModuleNotifySetNewNotifyFlag(bool isNewNotify)
{
    uteModuleNotifyData.isNewNotify = isNewNotify;
}

/**
*@brief        判断是否为新消息
*@details
*@return bool isNewNotify,true:是新消息;false:不是新消息
*@author       xjc
*@date       2021-12-25
*/
bool uteModuleNotifyIsNewNotifyDisplay(void)
{
    return uteModuleNotifyData.isNewNotify;
}

/**
*@brief        存在消息的条数
*@details
*@return uint8_t
*@author       zn.zeng
*@date       2022-02-15
*/
uint8_t uteModuleNotifyGetTotalNotifyCnt(void)
{
    return uteModuleNotifyData.totalNotifyCnt;
}

uint8_t uteModuleNotifyGetMaxNotifyCnt(void)
{
    return UTE_MODULE_NOTIFY_SAVE_CNT;
}
