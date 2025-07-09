#ifndef _BSP_BLE_H
#define _BSP_BLE_H

#include "app.h"
#include "ble.h"
#include "bsp_ams.h"
#include "bsp_ancs.h"

typedef struct
{
    bool ble_connected;
    bool ams_connected;
    bool ancs_connected;
    u32 ansc_uid; // ute add
} ble_cb_t;
extern ble_cb_t ble_cb;

/**
 * @brief 发起一键双连
 **/
void ble_bt_connect(void);

#define ble_is_connected()              (bool)(ble_cb.ble_connected)            //ble是否已连接
#define ble_ams_is_connected()          (bool)(ble_cb.ams_connected)            //ams是否已连接
#define ble_ancs_is_connected()         (bool)(ble_cb.ancs_connected)           //ancs是否已连接

#endif
