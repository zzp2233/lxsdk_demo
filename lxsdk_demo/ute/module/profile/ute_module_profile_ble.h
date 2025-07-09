/**
*@file
*@brief    ute ble 公共服务头文件
*@details  此服务是小数据量通信用，包含两个特征值
*@author   zn.zeng
*@date     Jul 05, 2021
*@version
*********************************************************************************************************
*/
/* Define to prevent recursive inclusion */
#ifndef _UTE_MODULE_PROFILE_BLE_H_
#define _UTE_MODULE_PROFILE_BLE_H_

#ifdef __cplusplus
extern "C"  {
#endif      /* __cplusplus */

/* Add Includes here */
#include "include.h"

/*! 第一个服务，小数据通信 zn.zeng  modify Jul 06, 2021 */
#define GATT_UUID_UTE_MODULE_PROFILE_BLE_SERVICE                0x55FF
#define GATT_UUID_UTE_MODULE_PROFILE_BLE_CHAR_READ_WRITE        0x33F1
#define GATT_UUID_UTE_MODULE_PROFILE_BLE_CHAR_NOTIFY            0x33F2
/*! 第二个服务，大数据通信 zn.zeng  modify Jul 06, 2021 */
#define GATT_UUID_UTE_MODULE_PROFILE_BLE50_SERVICE                0x56FF
#define GATT_UUID_UTE_MODULE_PROFILE_BLE50_CHAR_READ_WRITE         0x34F1
#define GATT_UUID_UTE_MODULE_PROFILE_BLE50_CHAR_NOTIFY             0x34F2
/*! 第三个服务，公共协议小数据通信 zn.zeng  modify Jul 06, 2021 */
#define GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_SERVICE                0x60FF
#define GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_CHAR_READ_WRITE        0x6001
#define GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_CHAR_NOTIFY            0x6002
/*! 第四个服务，公共协议大数据通信 zn.zeng  modify Jul 06, 2021 */
#define GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE50_SERVICE                0x61FF
#define GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE50_CHAR_READ_WRITE        0x6101
#define GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE50_CHAR_NOTIFY            0x6102

uint8_t uteModuleProfileBleAddService(void);
uint8_t uteModuleProfileBleSendNotify(uint8_t att_handle, void *p_value, uint16_t length);
bool uteModuleProfileBleSendToPhone(uint8_t *data,uint8_t size);
bool uteModuleProfileBle50SendToPhone(uint8_t *data,uint8_t size);
void uteModuleProfileSetPublicProtocol(bool isPublic);
bool uteModuleProfileGetPublicProtocol(void);
#ifdef __cplusplus
}
#endif


#endif /* _SIMPLE_BLE_SERVICE_H_ */
