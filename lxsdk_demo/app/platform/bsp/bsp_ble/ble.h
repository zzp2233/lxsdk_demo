#ifndef __BLE_H
#define __BLE_H

/**
 * @brief ����bt��ַΪbt_get_local_bd_addr�ķ���ֵ
 **/
void bsp_change_bt_mac(void);

/**
 * @brief ��ȡ����ble��ַ
 **/
void ble_get_local_bd_addr(u8 *addr);

#endif  //__BLE_H
