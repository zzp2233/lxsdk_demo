/**
*@file
*@brief       application 公共层头文件
*@details  application 公共层，链接各模块
*@author       zn.zeng
*@date       Jul 01, 2021
*@version      v1.0
*/
#ifndef _UTE_TASK_APPLICATION_H_
#define _UTE_TASK_APPLICATION_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "include.h"

typedef struct
{
    uint16_t type;
    uint32_t param;
    uint32_t dataLen;
    void *buff;
} ute_task_application_message_t;
typedef struct
{
    void *msgpool;
    uint32_t msgSize;
    uint32_t msgMaxCnt;
    uint32_t flag;
    uint8_t cnt;
} ute_task_application_mq_t;
void uteTaskApplicationInit(void);
bool uteTaskApplicationSendMsg(ute_task_application_message_t *pMsg);

#endif //_UTE_TASK_APPLICATION_H_
