/**
*@file
*@brief     flash map公共文件
*@details
*@author        zn.zeng
*@date        2024-03-22
*/
#ifndef _UTE_FLASH_MAP_COMMON_H_
#define _UTE_FLASH_MAP_COMMON_H_
#include "ute_project_config.h"

#if UTE_FLASH_128MBIT_1024K_MAP_SUPPORT
#include "ute_flash_64mbit_1024k_map.h"
#else
#include "ute_flash_64mbit_1280k_map.h"
#endif

#endif /* _UTE_FLASH_MAP_COMMON_H_ */
