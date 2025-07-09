#include "include.h"
#include "app_ab_link.h"

#if (USE_APP_TYPE == USE_AB_APP)
///////////////////////////////////////////////////////////////////////////
#define AB_MATE_VID     2           //广播包协议版本号
#define AB_MATE_BID     0x000000    //代理商和客户ID，0表示原厂bluetrum
#define ADV_VID_POS     (4 + 3)
#define ADV_MAC_POS     (7 + 3)
#define ADV_FMASK_POS   (13 + 3)
#define ADV_BID_POS     (14 + 3)

const uint8_t adv_data_const[] =
{
    // Flags general discoverable, BR/EDR not supported
    0x02, 0x01, 0x06,

    // Manufacturer Specific Data
    //len type  CID        VID   PID        MAC                            FMASK  BID
    0x10, 0xff, 0x42,0x06, AB_MATE_VID, 0x02,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,  0x00,0x00,0x00,
};

const uint8_t scan_data_const_name[] =
{
    // Complete Local Name
    0x0C, 0x09, 'B','T','5','6','8','0','-','B','L','E',
};

const uint8_t scan_data_const[] =
{
    0x09, 0x16, 0x02, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void func_camera_jpeg_rx(u8 *buf, u16 len);

u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size)
{
    u8 ble_addr[6];
    memset(scan_buf, 0, buf_size);
    u32 data_len = sizeof(scan_data_const_name);
    memcpy(scan_buf, scan_data_const_name, data_len);

    //读取BLE配置的蓝牙名称
    int len_name;
    len_name = strlen(xcfg_cb.le_name);

    if (len_name > 0)
    {
        memcpy(&scan_buf[2], xcfg_cb.le_name, len_name);
        data_len = 2 + len_name;
        scan_buf[0] = len_name + 1;
    }

    memcpy(&scan_buf[data_len], scan_data_const, sizeof(scan_data_const));
    data_len += sizeof(scan_data_const);

    ble_get_local_bd_addr(ble_addr);
    memcpy(scan_buf + data_len - 6, ble_addr, 6);

    printf("--->scan_buf:");
    print_r(scan_buf, data_len);

    return data_len;
}

bool ble_change_name(char *le_name)
{
    char name_cache[32] = {0};
    uint8_t scan_data_len = 0;
    uint8_t scan_data[31] = {0};
    memcpy(name_cache, xcfg_cb.le_name, sizeof(xcfg_cb.le_name));
    memset(xcfg_cb.le_name, 0, sizeof(xcfg_cb.le_name));
    memcpy(xcfg_cb.le_name, le_name, strlen(le_name));
    scan_data_len = ble_get_scan_data(scan_data, 31);
    bool ret = ble_set_scan_rsp_data((uint8_t*)scan_data, scan_data_len);
    if (!ret)
    {
        printf("scan data over length!\n");
        memcpy(xcfg_cb.le_name, name_cache, sizeof(name_cache));
    }
    return ret;
}

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    printf("%s\n", __func__);
    u8 edr_addr[6];
    u32 data_len = sizeof(adv_data_const);
    u32 bid = AB_MATE_BID;

    memset(adv_buf, 0, buf_size);

    // get adv const
    memcpy(adv_buf, adv_data_const, data_len);

    // get mac addr
    ble_get_local_bd_addr(edr_addr);

    //广播包协议从版本1之后，经典蓝牙地址都做个简单的加密操作，不直接暴露地址
    if(AB_MATE_VID > 1)
    {
        for(u8 i = 0; i < 6; i++)
        {
            edr_addr[i] ^= 0xAD;
        }
    }

    memcpy(&adv_buf[ADV_MAC_POS], edr_addr, 6);
    memcpy(&adv_buf[ADV_BID_POS], &bid, 3);

    printf("--->adv_buf:");
    print_r(adv_buf, data_len);

    return data_len;
}

/***
*   ble tx buf set
*/

#define MAX_NOTIFY_NUM          4
#define MAX_NOTIFY_LEN          69     //max=247
#define NOTIFY_POOL_SIZE       (MAX_NOTIFY_LEN + sizeof(struct txbuf_tag)) * MAX_NOTIFY_NUM

AT(.ble_cache.att)
uint8_t notify_tx_pool[NOTIFY_POOL_SIZE];

void ble_txpkt_init(void)
{
    txpkt_init(&notify_tx, notify_tx_pool, MAX_NOTIFY_NUM, MAX_NOTIFY_LEN);
    notify_tx.send_kick = ble_send_kick;
}

/***
*   ble rx buf set
*/
#define BLE_CMD_BUF_LEN     8
#define BLE_CMD_BUF_MASK    (BLE_CMD_BUF_LEN - 1)
#define BLE_RX_BUF_LEN      256

struct ble_cmd_t
{
    u8 len;
    u16 handle;
    u8 buf[BLE_RX_BUF_LEN];
};

static struct ble_cmd_cb_t
{
    struct ble_cmd_t cmd[BLE_CMD_BUF_LEN];
    u8 cmd_rptr;
    u8 cmd_wptr;
    bool wakeup;
} ble_cmd_cb AT(.ble_cache.cmd);

//--------------------------------------------------------
//app protocol
static const uint8_t uart_service_primay_uuid128[16]= {0x9d,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x01,0x00,0x40,0x6e};
const uint8_t tx_uuid128[16]= {0x9d,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x03,0x00,0x40,0x6e};
const uint8_t rx_uuid128[16]= {0x9d,0xca,0xdc,0x24,0x0e,0xe5,0xa9,0xe0,0x93,0xf3,0xa3,0xb5,0x02,0x00,0x40,0x6e};

static const gatts_uuid_base_st uuid_tx_primay_base =
{
    .type = BLE_GATTS_UUID_TYPE_128BIT,
    .uuid = uart_service_primay_uuid128,
};

static const gatts_uuid_base_st gatt_tx_base =
{
    .props = ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_128BIT,
    .uuid = tx_uuid128,
};


static const gatts_uuid_base_st gatt_rx_base =
{
    .props = ATT_WRITE_WITHOUT_RESPONSE,
    .type = BLE_GATTS_UUID_TYPE_128BIT,
    .uuid = rx_uuid128,
};

static gatts_service_base_st gatts_tx_base;
static gatts_service_base_st gatts_rx_base;

AT(.sleep_text.sleep)
int app_protocol_tx(u8 *buf, u8 len)
{
    if (!ble_is_connect())
    {
        return false;
    }

#if FUNC_CAMERA_TRANS_EN
    if ((buf[0] != 0xaa) && (buf[1] != 55))
#endif
    {
        static u8 seq_num = 0;
        buf[0] = seq_num;
        seq_num++;
        if (seq_num > 0xf)
        {
            seq_num = 0;
        }
    }

    printf("app_tx:");
    print_r(buf, len);

    return ble_tx_notify(gatts_tx_base.handle, buf, len);
}

typedef int (*ble_gatt_callback_func)(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len);

static int gatt_callback_app(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    u8 wptr = ble_cmd_cb.cmd_wptr & BLE_CMD_BUF_MASK;

//    printf("BLE_RX len[%d] handle[%d]\n", len, handle);
//    print_r(ptr, len);

    ble_cmd_cb.cmd_wptr++;
    if (len > BLE_RX_BUF_LEN)
    {
        len = BLE_RX_BUF_LEN;
    }
    memcpy(ble_cmd_cb.cmd[wptr].buf, ptr, len);
    ble_cmd_cb.cmd[wptr].len = len;
    ble_cmd_cb.cmd[wptr].handle = handle;
    ble_cmd_cb.wakeup = true;

    return 0;
}

static ble_gatt_characteristic_cb_info_t gatts_app_protocol_rx_cb_info =
{
    .att_write_callback_func = gatt_callback_app,
};

static ble_gatt_characteristic_cb_info_t gatts_app_protocol_tx_cb_info =
{
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
};

void ble_app_blue_fit_rx_callback(u8 *ptr, u16 len)
{
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if FUNC_CAMERA_TRANS_EN
    if (func_cb.sta == FUNC_CAMERA)
    {
        func_camera_jpeg_rx(ptr, len);
    }
    else
#endif
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {
        printf("app_rx len[%d]:\n", len);
        print_r(ptr, len);
        ble_uart_service_write(ptr, len);
    }

}

//----------------------------------------------------------------------------
void ble_app_watch_process(void)
{
    if (ble_cmd_cb.cmd_rptr == ble_cmd_cb.cmd_wptr)
    {
        ble_cmd_cb.wakeup = false;
        return;
    }

    u8 rptr = ble_cmd_cb.cmd_rptr & BLE_CMD_BUF_MASK;
    ble_cmd_cb.cmd_rptr++;
    u8 *ptr = ble_cmd_cb.cmd[rptr].buf;
    u8 len = ble_cmd_cb.cmd[rptr].len;
    u16 handle = ble_cmd_cb.cmd[rptr].handle;

    if (handle == gatts_rx_base.handle)
    {
        ble_app_blue_fit_rx_callback(ptr, len);
    }
}

bool ble_app_watch_need_wakeup(void)
{
    return false;
}

//----------------------------------------------------------------------------
//
void ble_app_gatts_service_init(void)
{
    int ret = 0;

    ble_set_gap_name(xcfg_cb.le_name, strlen(xcfg_cb.le_name)+1);

    ret |= ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                 uuid_tx_primay_base.uuid,
                                 uuid_tx_primay_base.type,
                                 NULL);

    ret |= ble_gatts_characteristic_add(gatt_tx_base.uuid,
                                        gatt_tx_base.type,
                                        gatt_tx_base.props,
                                        &gatts_tx_base.handle,
                                        &gatts_app_protocol_tx_cb_info);      //characteristic

    ret |= ble_gatts_characteristic_add(gatt_rx_base.uuid,
                                        gatt_rx_base.type,
                                        gatt_rx_base.props,
                                        &gatts_rx_base.handle,
                                        &gatts_app_protocol_rx_cb_info);      //characteristic


    if(ret != BLE_GATTS_SUCCESS)
    {
        printf("gatt err: %d\n", ret);
        return;
    }
}


//----------------------------------------------------------------------------
//
void ble_app_watch_init(void)
{
    ble_app_gatts_service_init();
}

void ble_app_watch_disconnect_callback(void)
{
    bind_sta_set(BIND_NULL);
}

void ble_app_watch_connect_callback(void)
{

}

void ble_app_watch_client_cfg_callback(u16 handle, u8 cfg)
{
    if (cfg)
    {
        ab_app_sync_info();
    }
}

#endif // (USE_APP_TYPE == USE_AB_APP)
