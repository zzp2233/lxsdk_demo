/**
*@file
*@brief        ute app 的task
*@details
*@author       zn.zeng
*@date       2023-03-25
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_task_application.h"
#include "ute_module_filesystem.h"
#include "ute_module_message.h"
#include "ute_module_systemtime.h"
#include "ute_application_common.h"
#include "ute_module_gui_common.h"

#if UTE_TASK_USER_SUPPORT

/*! ute app task的指针 zn.zeng, 2022-03-01  */
os_mq_t uteTaskApplicationMsgQueueHandle;
static os_thread_t uteTaskApplicationHandle;

void uteTaskApplicationMsgHandler(ute_task_application_message_t *msg)
{
    uteModuleMessageUteApplicationTaskHandler(msg);
}

void uteTaskApplicationMain(void *param)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,enter ute app thread",__func__);

#if UTE_FACTORY_TEST_AUDIO_RF_MODE_SUPPORT
    ute_new_factory_test_data_t *data;
    uteModuleNewFactoryTestSetMode(&data);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,data->mode = %d",__func__,data->mode);
    if(data->mode == FACTORY_TEST_MODE_RF_DUT)
    {
        uteDrvKeysCommonInit();
    }
    else
#endif
    {
#if UTE_MODULE_USER_MALLOC_SUPPORT
        uteModulePlatformMemoryInitPool();
#endif
        uteApplicationCommonStartupFrist();
        uteModuleGuiCommonInit();
    }
    while (1)
    {
        ute_task_application_message_t msg;
        if (os_mq_recv(uteTaskApplicationMsgQueueHandle, &msg, sizeof(msg),0xFFFFFFFF) == OS_EOK)
        {
            // printf("uteTaskApplicationMsgHandler,type=%d\n",msg.type);
            uteModulePlatformWdgFeed();
            uteTaskApplicationMsgHandler(&msg);
        }
    }
}

/**
*@brief        发送msg到ute app task
*@detail
*@author       zn.zeng
*@date       2022-03-01
*/
AT(.com_text.ute_msg)
bool uteTaskApplicationSendMsg(ute_task_application_message_t *pMsg)
{
    if(os_mq_send(uteTaskApplicationMsgQueueHandle, pMsg, sizeof(ute_task_application_message_t)) != OS_EOK)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
*@brief        ute app task 初始化，创建task
*@details
*@author       zn.zeng
*@date       2023-03-25
*/
void uteTaskApplicationInit(void)
{
    uteTaskApplicationMsgQueueHandle = os_mq_create("ute_msg", sizeof(ute_task_application_message_t), UTE_TASK_APPLICATION_MESSAGE_MAX_CNT, OS_IPC_FLAG_FIFO);
    uteTaskApplicationHandle = os_thread_create(
                                   "ute",
                                   uteTaskApplicationMain,
                                   0,
                                   UTE_TASK_APPLICATION_STACK_SIZE,
                                   UTE_TASK_APPLICATION_PRIORITY,
                                   -1);
    if (uteTaskApplicationHandle != 0)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,startup ute app thread",__func__);
        os_thread_startup(uteTaskApplicationHandle);
    }
}

#else

#define UTE_TASK_APPLICATION_PRIORITY             24
#define UTE_TASK_APPLICATION_MESSAGE_MAX_CNT      0x20
// #define UTE_TASK_APPLICATION_STACK_SIZE           0x2000
void *uteTaskApplicationHandle;
ute_task_application_mq_t uteTaskApplicationMsgQueueHandle;
static uint8_t taskAppMqBuff[UTE_TASK_APPLICATION_MESSAGE_MAX_CNT*sizeof(ute_task_application_message_t)] AT(.disp.ute); // mq msg buff
void uteTaskApplicationMsgHandler(ute_task_application_message_t *msg)
{
    uteModuleMessageUteApplicationTaskHandler(msg);
}
/**
*@brief        task 消息队列初始化函数
*@details
*@author       zn.zeng
*@date       2022-03-01
*/
void uteTaskMqInit(ute_task_application_mq_t *mq, const char *name, void *msgpool, uint32_t msgSize, uint32_t msgMaxCnt, uint32_t flag)
{
    mq->msgpool = msgpool;
    mq->msgSize = msgSize;
    mq->msgMaxCnt = msgMaxCnt;
    mq->flag = flag;
    mq->cnt = 0;
}
/**
*@brief        task 消息队列发送函数
*@details
*@author       zn.zeng
*@date       2022-03-01
*/
bool uteTaskMqSend(ute_task_application_mq_t *mq,void *msg,uint32_t msgSize)
{
    bool ret = false;
    if(mq->cnt<mq->msgMaxCnt)
    {
        if(msgSize==mq->msgSize)
        {
            uint32_t cpyAddress = ((uint32_t)mq->msgpool + mq->cnt*mq->msgSize);
            memcpy((void*)cpyAddress,msg,msgSize);
            mq->cnt++;
            uteModulePlatformRestartTimer(&uteTaskApplicationHandle,20);
            ret = true;
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,msg size is error",__func__);
            ret = false;
        }
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,msg buff is full",__func__);
        ret = false;
    }
    return ret;
}
/**
*@brief        task 消息队列接收函数
*@details
*@author       zn.zeng
*@date       2022-03-01
*/
bool uteTaskMqRecv(ute_task_application_mq_t *mq,void *msg,uint32_t msgSize)
{
    bool ret = false;
    if(mq->cnt!=0)
    {
        memcpy(msg,mq->msgpool,msgSize);
        if(mq->cnt>1)
        {
            for (uint8_t i = 0; i < mq->cnt; i++)
            {
                uint32_t cpyAddress0 = ((uint32_t)mq->msgpool + i*mq->msgSize);
                uint32_t cpyAddress1 = ((uint32_t)mq->msgpool + (i+1)*mq->msgSize);
                memcpy((void*)cpyAddress0,(void*)cpyAddress1,mq->msgSize);
            }
        }
        mq->cnt--;
        ret = true;
    }
    else
    {
        ret = false;
    }
    return ret;
}
void uteTaskApplicationWhileRun(void *pxTimer)
{
    ute_task_application_message_t msg;
    if(uteTaskMqRecv(&uteTaskApplicationMsgQueueHandle,&msg,sizeof(ute_task_application_message_t)))
    {
        uteTaskApplicationMsgHandler(&msg);
    }
    else
    {
        uteModulePlatformStopTimer(&uteTaskApplicationHandle);
    }
}
/**
*@brief        task main 函数
*@details
*@author       zn.zeng
*@date       2022-03-01
*/
void uteTaskApplicationMain(void *param)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s",__func__);
    uteTaskMqInit(&uteTaskApplicationMsgQueueHandle,"app task",(void *)&taskAppMqBuff[0],sizeof(ute_task_application_message_t),UTE_TASK_APPLICATION_MESSAGE_MAX_CNT,OS_IPC_FLAG_FIFO);
#if UTE_FACTORY_TEST_AUDIO_RF_MODE_SUPPORT
    ute_new_factory_test_data_t *data;
    uteModuleNewFactoryTestSetMode(&data);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,data->mode = %d",__func__,data->mode);
    if(data->mode == FACTORY_TEST_MODE_RF_DUT)
    {
        uteDrvKeysCommonInit();
    }
    else
#endif
    {
#if UTE_MODULE_USER_MALLOC_SUPPORT
        uteModulePlatformMemoryInitPool();
#endif
        uteApplicationCommonStartupFrist();
        uteModuleGuiCommonInit();
    }
    uteModulePlatformCreateTimer(&uteTaskApplicationHandle,"task",0,20,true,uteTaskApplicationWhileRun);
}

/**
*@brief        ute app task 初始化，创建task
*@details
*@author       zn.zeng
*@date       2023-03-25
*/
void uteTaskApplicationInit(void)
{
    uteTaskApplicationMain(NULL);
}
/**
*@brief        发送msg到ute app task
*@detail
*@author       zn.zeng
*@date       2022-03-01
*/
AT(.com_text.isr)
bool uteTaskApplicationSendMsg(ute_task_application_message_t *pMsg)
{
    if(uteTaskMqSend(&uteTaskApplicationMsgQueueHandle, pMsg, sizeof(ute_task_application_message_t)) == false)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,fail to queue,type=%d",__func__,pMsg->type);
        return false;
    }
    else
    {
        return true;
    }
}

#endif