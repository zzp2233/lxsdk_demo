#include "ute_module_profile_ble.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_protocol.h"

#if (USE_APP_TYPE == USE_UTE_APP)

/*! 最大通信byte zn.zeng Jul 05, 2021 */
#define UTE_MODULE_PORFILE_READ_MAX_BYTES 20
/*! 此变量读取功能标识 zn.zeng Jul 05, 2021 */
static uint8_t uteModuleProfileCharReadValueBuff[UTE_MODULE_PORFILE_READ_MAX_BYTES];

///////////////////////////////////////////////////////////////////////////
// #define AB_MATE_VID     2           //广播包协议版本号
// #define AB_MATE_BID     0x000000    //代理商和客户ID，0表示原厂bluetrum
// #define ADV_VID_POS     (4 + 3)
// #define ADV_MAC_POS     (7 + 3)
// #define ADV_FMASK_POS   (13 + 3)
// #define ADV_BID_POS     (14 + 3)

// const uint8_t adv_data_const[] = {
//     // Flags general discoverable, BR/EDR not supported
//     0x02, 0x01, 0x06,

//     // Manufacturer Specific Data
//     //len type  CID        VID   PID        MAC                            FMASK  BID
//     0x10, 0xff, 0x42,0x06, AB_MATE_VID, 0x02,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,  0x00,0x00,0x00,
// };

// const uint8_t scan_data_const_name[] = {
//     // Complete Local Name
//     0x0C, 0x09, 'B','T','5','6','8','0','-','B','L','E',
// };

// const uint8_t scan_data_const[] = {
//     0x09, 0x16, 0x02, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };

// void func_camera_jpeg_rx(u8 *buf, u16 len);

u32 ble_get_scan_data(u8 *scan_buf, u32 buf_size)
{
    uint8_t data_len = uteModulePlatformGetScanRspData(scan_buf, buf_size);

    printf("--->scan_buf:");
    print_r(scan_buf, data_len);

    return data_len;
}

// bool ble_change_name(char *le_name)
// {
//     char name_cache[32] = {0};
//     uint8_t scan_data_len = 0;
//     uint8_t scan_data[31] = {0};
//     memcpy(name_cache, xcfg_cb.le_name, sizeof(xcfg_cb.le_name));
//     memset(xcfg_cb.le_name, 0, sizeof(xcfg_cb.le_name));
//     memcpy(xcfg_cb.le_name, le_name, strlen(le_name));
//     scan_data_len = ble_get_scan_data(scan_data, 31);
//     bool ret = ble_set_scan_rsp_data((uint8_t*)scan_data, scan_data_len);
//     if (!ret) {
//         printf("scan data over length!\n");
//         memcpy(xcfg_cb.le_name, name_cache, sizeof(name_cache));
//     }
//     return ret;
// }

u32 ble_get_adv_data(u8 *adv_buf, u32 buf_size)
{
    printf("%s\n", __func__);

    uint8_t data_len = uteModulePlatformGetAdvData(adv_buf, buf_size);

    printf("--->adv_buf:");
    print_r(adv_buf, data_len);

    return data_len;
}

/***
*   ble tx buf set
*/

#define MAX_NOTIFY_NUM          8
#define MAX_NOTIFY_LEN          256     //max=247
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

static bool uteModuleProfileIsPublicProtocol = false;

static const uint8_t ute_ble_service_primay_uuid[2] = {GATT_UUID_UTE_MODULE_PROFILE_BLE_SERVICE & 0xff, GATT_UUID_UTE_MODULE_PROFILE_BLE_SERVICE >> 8};
const uint8_t ute_ble_read_write_uuid[2] = {GATT_UUID_UTE_MODULE_PROFILE_BLE_CHAR_READ_WRITE & 0xff, GATT_UUID_UTE_MODULE_PROFILE_BLE_CHAR_READ_WRITE >> 8};
const uint8_t ute_ble_notify_uuid[2] = {GATT_UUID_UTE_MODULE_PROFILE_BLE_CHAR_NOTIFY & 0xff, GATT_UUID_UTE_MODULE_PROFILE_BLE_CHAR_NOTIFY >> 8};

static const uint8_t ute_ble5_service_primay_uuid[2]= {GATT_UUID_UTE_MODULE_PROFILE_BLE50_SERVICE & 0xff, GATT_UUID_UTE_MODULE_PROFILE_BLE50_SERVICE >> 8};
const uint8_t ute_ble5_read_write_uuid[2]= {GATT_UUID_UTE_MODULE_PROFILE_BLE50_CHAR_READ_WRITE & 0xff, GATT_UUID_UTE_MODULE_PROFILE_BLE50_CHAR_READ_WRITE >> 8};
const uint8_t ute_ble5_notify_uuid[2]= {GATT_UUID_UTE_MODULE_PROFILE_BLE50_CHAR_NOTIFY & 0xff, GATT_UUID_UTE_MODULE_PROFILE_BLE50_CHAR_NOTIFY >> 8};

// 55ff
static const gatts_uuid_base_st uuid_ute_ble_primay_base =
{
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble_service_primay_uuid,
};
static const gatts_uuid_base_st gatt_ute_ble_notify_base =
{
    .props = ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble_notify_uuid,
};
static const gatts_uuid_base_st gatt_ute_ble_read_write_base =
{
    .props = ATT_WRITE | ATT_READ,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble_read_write_uuid,
};

// 56ff
static const gatts_uuid_base_st uuid_ute_ble5_primay_base =
{
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble5_service_primay_uuid,
};
static const gatts_uuid_base_st gatt_ute_ble5_notify_base =
{
    .props = ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble5_notify_uuid,
};
static const gatts_uuid_base_st gatt_ute_ble5_read_write_base =
{
    .props = ATT_WRITE_WITHOUT_RESPONSE | ATT_READ,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble5_read_write_uuid,
};

static gatts_service_base_st gatts_ute_ble_notify_base;
static gatts_service_base_st gatts_ute_ble_read_write_base;
static gatts_service_base_st gatts_ute_ble5_notify_base;
static gatts_service_base_st gatts_ute_ble5_read_write_base;

#if UTE_SERVICE_PUBLIC_BLE_SUPPORT

static const uint8_t ute_ble_public_service_primay_uuid[2] = {GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_SERVICE & 0xff, GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_SERVICE >> 8};
const uint8_t ute_ble_public_read_write_uuid[2] = {GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_CHAR_READ_WRITE & 0xff, GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_CHAR_READ_WRITE >> 8};
const uint8_t ute_ble_public_notify_uuid[2] = {GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_CHAR_NOTIFY & 0xff, GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE_CHAR_NOTIFY >> 8};

static const uint8_t ute_ble5_public_service_primay_uuid[2]= {GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE50_SERVICE & 0xff, GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE50_SERVICE >> 8};
const uint8_t ute_ble5_public_read_write_uuid[2]= {GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE50_CHAR_READ_WRITE & 0xff, GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE50_CHAR_READ_WRITE >> 8};
const uint8_t ute_ble5_public_notify_uuid[2]= {GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE50_CHAR_NOTIFY & 0xff, GATT_UUID_UTE_MODULE_PROFILE_PUBLIC_BLE50_CHAR_NOTIFY >> 8};

// 60ff
static const gatts_uuid_base_st uuid_ute_ble_public_primay_base =
{
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble_public_service_primay_uuid,
};
static const gatts_uuid_base_st gatt_ute_ble_public_notify_base =
{
    .props = ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble_public_notify_uuid,
};
static const gatts_uuid_base_st gatt_ute_ble_public_read_write_base =
{
    .props = ATT_WRITE | ATT_READ
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble_public_read_write_uuid,
};

// 61ff
static const gatts_uuid_base_st uuid_ute_ble5_public_primay_base =
{
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble5_public_service_primay_uuid,
};
static const gatts_uuid_base_st gatt_ute_ble5_public_notify_base =
{
    .props = ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble5_public_notify_uuid,
};
static const gatts_uuid_base_st gatt_ute_ble5_public_read_write_base =
{
    .props = ATT_WRITE_WITHOUT_RESPONSE | ATT_READ
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = ute_ble5_public_read_write_uuid,
};

static gatts_service_base_st gatts_ute_ble_public_notify_base;
static gatts_service_base_st gatts_ute_ble_public_read_write_base;
static gatts_service_base_st gatts_ute_ble5_public_notify_base;
static gatts_service_base_st gatts_ute_ble5_public_read_write_base;

#endif

//FOTA
#if LE_AB_FOT_EN
const uint8_t fota_uuid16[2]= {0x15, 0xff};
static const gatts_uuid_base_st uuid_fota_base =
{
    .props = ATT_READ|ATT_WRITE_WITHOUT_RESPONSE,
    .type  = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid  = fota_uuid16,
};
static gatts_service_base_st gatts_app_notify_base;
static int gatt_callback_fota(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    if(fot_app_connect_auth(ptr,len))
    {
        fot_recv_proc(ptr,len);
    }
    return 0;
}

static ble_gatt_characteristic_cb_info_t gatts_fota_cb_info =
{
    .att_write_callback_func = gatt_callback_fota,
};

bool ble_fot_send_packet(u8 *buf, u8 len)
{
    return ble_tx_notify(gatts_app_notify_base.handle, buf, len);
}

const uint8_t app_primay_uuid16[2]= {0x12, 0xff};
static const gatts_uuid_base_st uuid_app_primay_base =
{
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_primay_uuid16,
};

const uint8_t app_write_uuid16[2]= {0x13, 0xff};
static const gatts_uuid_base_st uuid_app_write_base =
{
    .props = ATT_WRITE,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_write_uuid16,
};

const uint8_t app_notify_uuid16[2]= {0x14, 0xff};
static const gatts_uuid_base_st uuid_app_notify_base =
{
    .props = ATT_NOTIFY,
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = app_notify_uuid16,
};
static gatts_service_base_st gatts_app_notify_base;

static ble_gatt_characteristic_cb_info_t gatts_app_notify_cb_info =
{
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
};

//app wirte callback
static int gatt_callback_app(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    u8 wptr = ble_cmd_cb.cmd_wptr & BLE_CMD_BUF_MASK;

    printf("BLE_RX [%d]: \n", len);
    print_r(ptr, len);

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

static ble_gatt_characteristic_cb_info_t gatts_app_write_cb_info =
{
    .att_write_callback_func = gatt_callback_app,
};

#endif

typedef int (*ble_gatt_callback_func)(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len);

static int uteModuleProfileBleWriteCallback(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    if(is_fot_start())
    {
        printf("fot_start,return\n");
        return 0;
    }

    u8 wptr = ble_cmd_cb.cmd_wptr & BLE_CMD_BUF_MASK;

//    printf("BLE_RX len[%d] handle[%d],wptr:%d,cmd_wptr:%d\n", len, handle,wptr,ble_cmd_cb.cmd_wptr);
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

static int uteModuleProfileBleReadCallback(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,handle:%d", __func__, handle);

    if (ptr)
    {
        if (handle == gatts_ute_ble_read_write_base.handle)
        {
            uteModuleProtocolReadFunctionSupport(&uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            UTE_MODULE_LOG_BUFF(UTE_LOG_PROTOCOL_LVL, &uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            memcpy(ptr, &uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            return UTE_MODULE_PORFILE_READ_MAX_BYTES;
        }
        else if (handle == gatts_ute_ble5_read_write_base.handle)
        {
            uteModuleProtocolReadExpandFunctionSupport(&uteModuleProfileCharReadValueBuff[0],UTE_MODULE_PORFILE_READ_MAX_BYTES);
            UTE_MODULE_LOG_BUFF(UTE_LOG_PROTOCOL_LVL, &uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            memcpy(ptr, &uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            return UTE_MODULE_PORFILE_READ_MAX_BYTES;
        }
#if UTE_SERVICE_PUBLIC_BLE_SUPPORT
        else if (handle == gatts_ute_ble_public_read_write_base.handle)
        {
            uteModuleProtocolReadFunctionSupport(&uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            UTE_MODULE_LOG_BUFF(UTE_LOG_PROTOCOL_LVL, &uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            memcpy(ptr, &uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            return UTE_MODULE_PORFILE_READ_MAX_BYTES;
        }
        else if (handle == gatts_ute_ble5_public_read_write_base.handle)
        {
            uteModuleProtocolReadExpandFunctionSupport(&uteModuleProfileCharReadValueBuff[0],UTE_MODULE_PORFILE_READ_MAX_BYTES);
            UTE_MODULE_LOG_BUFF(UTE_LOG_PROTOCOL_LVL, &uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            memcpy(ptr, &uteModuleProfileCharReadValueBuff[0], UTE_MODULE_PORFILE_READ_MAX_BYTES);
            return UTE_MODULE_PORFILE_READ_MAX_BYTES;
        }
#endif
    }

    return 0;
}

static ble_gatt_characteristic_cb_info_t gatts_app_protocol_rx_cb_info =
{
    .att_write_callback_func = uteModuleProfileBleWriteCallback,
    .att_read_callback_func = uteModuleProfileBleReadCallback,
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
        // ble_uart_service_write(ptr, len);
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

    if (handle == gatts_ute_ble_read_write_base.handle || handle == gatts_ute_ble5_read_write_base.handle
#if UTE_SERVICE_PUBLIC_BLE_SUPPORT
        || handle == gatts_ute_ble_public_read_write_base.handle || handle == gatts_ute_ble5_public_read_write_base.handle
#endif
       )
    {
        bool isPublic = false;
#if UTE_SERVICE_PUBLIC_BLE_SUPPORT
        if (handle == gatts_ute_ble_public_read_write_base.handle || handle == gatts_ute_ble5_public_read_write_base.handle)
        {
            isPublic = true;
        }
#endif
        uteModuleProfileSetPublicProtocol(isPublic);
        if(len > 0)
        {
            uteModuleProtocolFromPhone(ptr, len, isPublic);
        }
    }
}

bool ble_app_watch_need_wakeup(void)
{
    return false;
}

uint8_t uteModuleProfileBleSendNotify(uint8_t att_handle, void *p_value, uint16_t length)
{
    return ble_tx_notify(att_handle, p_value, length);
}

/**
*@brief  小数据通信服务发送数据到应用端
*@details   公共小数据通信服务也使用这个发送，通过一个转换表，转换命令
*@param[in] uint8_t *data 要发送的数据指针
*@param[in] uint8_t size 要发送的数据长度
*@return 返回值为是否成功，true为成功
*@author        zn.zeng
*@date         Jul 06, 2021
*/
bool uteModuleProfileBleSendToPhone(uint8_t *data,uint8_t size)
{
    uint8_t isRet = false;
    if(is_fot_start())
    {
        printf("fot_start,return\n");
        return false;
    }
    if (!ble_is_connect())
    {
        return false;
    }
#if UTE_USER_ID_FOR_BINDING_SUPPORT
    if(!uteModuleAppBindingGetOurAppConnection())
    {
        if((data[0] == CMD_USER_ID_FOR_BINDING)||(data[0] == PUBLIC_CMD_USER_ID_FOR_BINDING))
        {
            // binding
            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,send data binding", __func__);
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,send data return", __func__);
            return isRet;
        }
    }
#endif
#if UTE_SERVICE_PUBLIC_BLE_SUPPORT
    if(uteModuleProfileIsPublicProtocol)
    {
        uteModuleProtocolConversionCmd(data,TYPE_TO_PUBLIC);
        isRet = uteModuleProfileBleSendNotify(gatts_ute_ble_public_notify_base.handle, data, size);
    }
    else
#endif
    {
        isRet = uteModuleProfileBleSendNotify(gatts_ute_ble_notify_base.handle, data, size);
    }
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,isRet:0x%x", __func__,isRet);
    UTE_MODULE_LOG_BUFF(UTE_LOG_PROTOCOL_LVL,data,size);
    return isRet == 0 ? true : false;
}

/**
*@brief  大数据通信服务发送数据到应用端
*@details   公共小数据通信服务也使用这个发送，通过一个转换表，转换命令
*@param[in] uint8_t *data 要发送的数据指针
*@param[in] uint8_t size 要发送的数据长度
*@return 返回值为是否成功，true为成功
*@author        zn.zeng
*@date         Jul 06, 2021
*/
bool uteModuleProfileBle50SendToPhone(uint8_t *data,uint8_t size)
{
    bool isRet = false;
    if(is_fot_start())
    {
        printf("fot_start,return\n");
        return false;
    }
    if (!ble_is_connect())
    {
        return false;
    }
#if UTE_SERVICE_PUBLIC_BLE_SUPPORT
    if(uteModuleProfileIsPublicProtocol)
    {
        uteModuleProtocolConversionCmd(data,TYPE_TO_PUBLIC);
        isRet = uteModuleProfileBleSendNotify(gatts_ute_ble5_public_notify_base.handle, data, size);
    }
    else
#endif
    {
        isRet = uteModuleProfileBleSendNotify(gatts_ute_ble5_notify_base.handle, data, size);
    }
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,isRet:0x%x", __func__,isRet);
    UTE_MODULE_LOG_BUFF(UTE_LOG_PROTOCOL_LVL,data,size);
    return isRet == 0 ? true : false;
}

/**
*@brief  设置公共协议通讯
*@details
*@param[in] bool isPublic
*@author        zn.zeng
*@date         2021-08-17
*/
void uteModuleProfileSetPublicProtocol(bool isPublic)
{
    uteModuleProfileIsPublicProtocol = isPublic;
}
bool uteModuleProfileGetPublicProtocol(void)
{
    return uteModuleProfileIsPublicProtocol;
}

//----------------------------------------------------------------------------
//
uint8_t uteModuleProfileBleAddService(void)
{
    int ret = 0;

    ble_set_gap_name(xcfg_cb.le_name, strlen(xcfg_cb.le_name) + 1);

    // 55ff
    ret |= ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                 uuid_ute_ble_primay_base.uuid,
                                 uuid_ute_ble_primay_base.type,
                                 NULL);
    ret |= ble_gatts_characteristic_add(gatt_ute_ble_read_write_base.uuid,
                                        gatt_ute_ble_read_write_base.type,
                                        gatt_ute_ble_read_write_base.props,
                                        &gatts_ute_ble_read_write_base.handle,
                                        &gatts_app_protocol_rx_cb_info); // characteristic
    ret |= ble_gatts_characteristic_add(gatt_ute_ble_notify_base.uuid,
                                        gatt_ute_ble_notify_base.type,
                                        gatt_ute_ble_notify_base.props,
                                        &gatts_ute_ble_notify_base.handle,
                                        &gatts_app_protocol_tx_cb_info); // characteristic
    // 56ff
    ret |= ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                 uuid_ute_ble5_primay_base.uuid,
                                 uuid_ute_ble5_primay_base.type,
                                 NULL);
    ret |= ble_gatts_characteristic_add(gatt_ute_ble5_read_write_base.uuid,
                                        gatt_ute_ble5_read_write_base.type,
                                        gatt_ute_ble5_read_write_base.props,
                                        &gatts_ute_ble5_read_write_base.handle,
                                        &gatts_app_protocol_rx_cb_info); // characteristic
    ret |= ble_gatts_characteristic_add(gatt_ute_ble5_notify_base.uuid,
                                        gatt_ute_ble5_notify_base.type,
                                        gatt_ute_ble5_notify_base.props,
                                        &gatts_ute_ble5_notify_base.handle,
                                        &gatts_app_protocol_tx_cb_info); // characteristic

#if UTE_SERVICE_PUBLIC_BLE_SUPPORT
    // 60ff
    ret |= ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                 uuid_ute_ble_public_primay_base.uuid,
                                 uuid_ute_ble_public_primay_base.type,
                                 NULL);
    ret |= ble_gatts_characteristic_add(gatt_ute_ble_public_read_write_base.uuid,
                                        gatt_ute_ble_public_read_write_base.type,
                                        gatt_ute_ble_public_read_write_base.props,
                                        &gatts_ute_ble_public_read_write_base.handle,
                                        &gatts_app_protocol_rx_cb_info); // characteristic
    ret |= ble_gatts_characteristic_add(gatt_ute_ble_public_notify_base.uuid,
                                        gatt_ute_ble_public_notify_base.type,
                                        gatt_ute_ble_public_notify_base.props,
                                        &gatts_ute_ble_public_notify_base.handle,
                                        &gatts_app_protocol_tx_cb_info); // characteristic
    // 61ff
    ret |= ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                 uuid_ute_ble5_public_primay_base.uuid,
                                 uuid_ute_ble5_public_primay_base.type,
                                 NULL);
    ret |= ble_gatts_characteristic_add(gatt_ute_ble5_public_read_write_base.uuid,
                                        gatt_ute_ble5_public_read_write_base.type,
                                        gatt_ute_ble5_public_read_write_base.props,
                                        &gatts_ute_ble5_public_read_write_base.handle,
                                        &gatts_app_protocol_rx_cb_info); // characteristic
    ret |= ble_gatts_characteristic_add(gatt_ute_ble5_public_notify_base.uuid,
                                        gatt_ute_ble5_public_notify_base.type,
                                        gatt_ute_ble5_public_notify_base.props,
                                        &gatts_ute_ble5_notify_base.handle,
                                        &gatts_app_protocol_tx_cb_info); // characteristic
#endif

//AB APP Ota Service
#if LE_AB_FOT_EN
    ret |= ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                 uuid_app_primay_base.uuid,
                                 uuid_app_primay_base.type,
                                 NULL);            //PRIMARY

    ret |= ble_gatts_characteristic_add(uuid_app_write_base.uuid,
                                        uuid_app_write_base.type,
                                        uuid_app_write_base.props,
                                        NULL,
                                        &gatts_app_write_cb_info);      //characteristic

    ret |= ble_gatts_characteristic_add(uuid_app_notify_base.uuid,
                                        uuid_app_notify_base.type,
                                        uuid_app_notify_base.props,
                                        &gatts_app_notify_base.handle,
                                        &gatts_app_notify_cb_info);      //characteristic

    ret |= ble_gatts_characteristic_add(uuid_fota_base.uuid,
                                        uuid_fota_base.type,
                                        uuid_fota_base.props,
                                        NULL,
                                        &gatts_fota_cb_info);      //characteristic
#endif

    if (ret != BLE_GATTS_SUCCESS)
    {
        printf("gatt err: %d\n", ret);
    }

    return ret;
}

//----------------------------------------------------------------------------
//
void ble_app_watch_init(void)
{
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL,"%s",__func__);
    uteModulePlatformAdvDataInit();

    uteModuleProfileBleAddService();
}

void ble_app_watch_disconnect_callback(void)
{
    uteApplicationCommonSetBleConnectState(0,false);
}

void ble_app_watch_connect_callback(void)
{
    uteApplicationCommonSetBleConnectState(0,true);
}

void ble_app_watch_client_cfg_callback(u16 handle, u8 cfg)
{
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL,"%s,handle=0x%x,cfg=%d",__func__,handle,cfg);
    if (handle == gatts_ute_ble_read_write_base.handle || handle == gatts_ute_ble5_read_write_base.handle
#if UTE_SERVICE_PUBLIC_BLE_SUPPORT
        || handle == gatts_ute_ble_public_read_write_base.handle || handle == gatts_ute_ble5_public_read_write_base.handle
#endif
       )
    {
        uteApplicationCommonSetAppClosed(cfg == 0 ? true : false);
    }
    else if (handle == gatts_app_notify_base.handle) // && cfg == 0
    {
        fot_ble_disconnect_callback();
    }
}

#endif // (USE_APP_TYPE == USE_AB_APP)
