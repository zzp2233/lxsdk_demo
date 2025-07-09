#ifndef     _UTE_DRV_GSENSOR_STK8321_H__
#define     _UTE_DRV_GSENSOR_STK8321_H__
/**
*@file
*@brief        STK8321 驱动头文件
*@details
*@author       zn.zeng
*@date       2021-07-26
*@version      v1.0
*/
#include "ute_project_config.h"
#include "ute_drv_gsensor_common.h"


#if UTE_DRV_STK8321_SUPPORT
#define UTE_DRV_STK8321_SALVEADDR       (0x1F>>1 )
#define STK8321_ACC_50MS   0x18
#define STK8321_ACC_25MS   0x16
#define STK8321_ACC_10MS   0x14
bool uteDrvGsensorStk8321WriteReg(uint8_t reg,uint8_t data);
uint8_t uteDrvGsensorStk8321ReadReg(uint8_t reg,uint8_t *data,uint8_t size);
extern const ute_drv_gsensor_common_config_t drvGsensorStk8321Function;
extern const ute_drv_gsensor_common_config_t drvGsensorStk8325Function;

#endif
#endif // _UTE_DRV_GSENSOR_STK8321_H__

