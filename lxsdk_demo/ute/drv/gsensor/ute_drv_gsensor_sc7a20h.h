#ifndef     _UTE_DRV_GSENSOR_SC7A20H_H__
#define     _UTE_DRV_GSENSOR_SC7A20H_H__
/**
*@file
*@brief        SC7A20H 驱动头文件
*@details
*@author       zn.zeng
*@date       2023-06-24
*@version      v1.0
*/
#include "ute_project_config.h"
#include "ute_drv_gsensor_common.h"


#if UTE_DRV_GSENSOR_SC7A20H_SUPPORT
#if 1
#define UTE_DRV_SC7A20H_SALVEADDR       (0X18) // 7BIT
#else
#define UTE_DRV_SC7A20H_SALVEADDR       (0X19) // 7BIT
#endif
#define SC7A20H_ACC_50MS   0x30
#define SC7A20H_ACC_25MS   0x40
#define SC7A20H_ACC_10MS   0x50
extern const ute_drv_gsensor_common_config_t drvGsensorSc7a20hFunction;
#endif
#endif // _UTE_DRV_GSENSOR_SC7A20H_H__

