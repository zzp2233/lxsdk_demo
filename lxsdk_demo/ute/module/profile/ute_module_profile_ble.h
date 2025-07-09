/**
*@file
*@brief    ute ble ��������ͷ�ļ�
*@details  �˷�����С������ͨ���ã�������������ֵ
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

/*! ��һ������С����ͨ�� zn.zeng  modify Jul 06, 2021 */
#define GATT_UUID_UTE_MODULE_PROFILE_BLE_SERVICE                0x55FF
#define GATT_UUID_UTE_MODULE_PROFILE_BLE_CHAR_READ_WRITE        0x33F1
#define GATT_UUID_UTE_MODULE_PROFILE_BLE_CHAR_NOTIFY            0x33F2
/*! �ڶ������񣬴�����ͨ�� zn.zeng  modify Jul 06, 2021 */
#define GATT_UUID_UTE_MODULE_PROFILE_BLE50_SERVICE                0x56FF
#define GATT_UUID_UTE_MODULE_PROFILE_BLE50_CHAR_READ_WRITE         0x34F1
#define GATT_UUID_UTE_MODULE_PROFILE_BLE50_CHAR_NOTIFY             0x34F2
/*! ���������񣬹���Э��С����ͨ�� zn.zeng  modify Jul 06, 2021 */
#define GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_SERVICE                0x60FF
#define GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_CHAR_READ_WRITE        0x6001
#define GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_CHAR_NOTIFY            0x6002
/*! ���ĸ����񣬹���Э�������ͨ�� zn.zeng  modify Jul 06, 2021 */
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
