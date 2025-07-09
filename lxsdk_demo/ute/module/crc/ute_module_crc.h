/**
*@file
*@brief        CRC校验模块头文件
*@details
*@author       zn.zeng
*@date       2021-08-24
*@version      v1.0
*/
#ifndef _UTE_MODULE_CRC_H_
#define _UTE_MODULE_CRC_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
void uteModuleCrc8Bit(uint8_t *crc, uint8_t *data,uint32_t size);
void uteModuleCrc32Bit(const uint8_t *data,uint32_t size,uint32_t *crc32);
#endif //_UTE_MODULE_CRC_H_

