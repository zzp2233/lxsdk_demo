/**
*@file
*@brief        查找手机处理模块
*@details
*@author       xjc
*@date       2022-01-18
*@version      v1.0
*/
#ifndef _UTE_MODULE_FINDPHONE_H_
#define _UTE_MODULE_FINDPHONE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"

typedef enum
{
    FIND_PHONE_STOP = 0,
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
    FIND_PHONE_RING = 1,
#endif // UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
} T_FIND_PHONE_STATUS;

typedef struct
{
    T_FIND_PHONE_STATUS status;
} ute_findphone_data_t;

void uteModuleFindPhoneInit(void);
void uteModuleFindPhoneSetStatus(T_FIND_PHONE_STATUS status);
uint8_t uteModuleFindPhoneGetStatus(void);
void uteModuleFindPhoneStartRing(void);
void uteModuleFindPhoneStopRing(void);
#endif
