#ifndef     _UTE_DRV_GSENSOR_SC7A20E_H__
#define     _UTE_DRV_GSENSOR_SC7A20E_H__
/**
*@file
*@brief        SC7A20E 驱动头文件
*@details
*@author       zn.zeng
*@date       2021-07-26
*@version      v1.0
*/
#include "ute_project_config.h"
#include "ute_drv_gsensor_common.h"


#if UTE_DRV_GSENSOR_SC7A20E_SUPPORT
#define UTE_DRV_SC7A20E_SALVEADDR       (0X18) // 7BIT
#define SC7A20E_ACC_50MS   0x30
#define SC7A20E_ACC_25MS   0x40
#define SC7A20E_ACC_10MS   0x50
extern const ute_drv_gsensor_common_config_t drvGsensorSc7a20eFunction;
#endif
#endif // _UTE_DRV_GSENSOR_SC7A20E_H__

