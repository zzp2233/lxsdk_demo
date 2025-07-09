#include "ble_hid_service.h"
#include "../ble.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

static ble_hid_tasks_typedef ble_hid_tasks;
static int hid_report_map_read_callback(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len);

static u8 report_map_android[]=
{
#if LE_HID_CONSUMER
    0x05, 0x0C,         // Usage Page (Consumer)
    0x09, 0x01,         // Usage (Consumer Control)
    0xA1, 0x01,         // Collection (Application)
    0x85, 0x01,         //   Report Id (1)
    0x1A, 0x00, 0x00,   //   Usage Minimum (Unassigned)
    0x2A, 0x9C, 0x02,   //   Usage Maximum (AC Distribute Vertically)
    0x16, 0x00, 0x00,   //   Logical minimum (0)
    0x26, 0x9C, 0x02,   //   Logical maximum (668)
    0x75, 0x10,         //   Report Size (16)
    0x95, 0x01,         //   Report Count (1)
    0x81, 0x00,         //   Input (Data,Array,Absolute,Bit Field)
    0xC0,               // End Collection
#endif // LE_HID_CONSUMER

#if LE_HID_DIGITIZER
    0x05, 0x0D,         // Usage Page (Digitizer)
    0x09, 0x02,         // Usage (Pen)
    0xA1, 0x01,         // Collection (Application)
    0x85, 0x02,         //   Report ID (2)
    0x09, 0x22,         //   Usage (Finger)
    0xA1, 0x02,         //   Collection (Logical)
    0x09, 0x42,         //     Usage (Tip Switch)
    0x15, 0x00,         //     Logical Minimum (0)
    0x25, 0x01,         //     Logical Maximum (1)
    0x75, 0x01,         //     Report Size (1)
    0x95, 0x01,         //     Report Count (1)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x32,         //     Usage (In Range)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x06,         //     Report Count (6)
    0x81, 0x03,         //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,         //     Report Size (8)
    0x09, 0x51,         //     Usage (Contact identifier)
    0x95, 0x01,         //     Report Count (1)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,         //     Usage Page (Generic Desktop Ctrls)
    0x26, 0xFF, 0x0F,   //     Logical Maximum (4095)
    0x75, 0x10,         //     Report Size (16)
    0x55, 0x0E,         //     Unit Exponent (-2)
    0x65, 0x33,         //     Unit (System: English Linear, Length: Inch)
    0x09, 0x30,         //     Usage (X)
    0x35, 0x00,         //     Physical Minimum (0)
    0x46, 0xB5, 0x04,   //     Physical Maximum (1205)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x46, 0x8A, 0x03,   //     Physical Maximum (906)
    0x09, 0x31,         //     Usage (Y)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,               //   End Collection
    0x05, 0x0D,         //   Usage Page (Digitizer)
    0x09, 0x54,         //   Usage (Contact count)
    0x95, 0x01,         //   Report Count (1)
    0x75, 0x08,         //   Report Size (8)
    0x81, 0x02,         //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x08,         //   Report ID (8)
    0x09, 0x55,         //   Usage (0x55)
    0x25, 0x05,         //   Logical Maximum (5)
    0xB1, 0x02,         //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,               // End Collection
#endif // LE_HID_DIGITIZER

#if LE_HID_KEYBOARD
    0x05, 0x01,         // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,         // USAGE (Keyboard)
    0xA1, 0x01,         // COLLECTION (Application)
    0x85, 0x04,         //   Report Id (4)
    0x05, 0x07,         //   USAGE_PAGE (Keyboard)
    0x19, 0xE0,         //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xE7,         //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,         //   LOGICAL_MINIMUM (0)
    0x25, 0x01,         //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,         //   REPORT_SIZE (1)
    0x95, 0x08,         //   REPORT_COUNT (8)
    0x81, 0x02,         //   INPUT (Data,Var,Abs)
    0x95, 0x01,         //   REPORT_COUNT (1)
    0x75, 0x08,         //   REPORT_SIZE (8)
    0x15, 0x00,         //   LOGICAL_MINIMUM (0)
    0x25, 0x65,         //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,         //   USAGE_PAGE (Keyboard)
    0x19, 0x00,         //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,         //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,         //   INPUT (Data,Ary,Abs)
    0xC0,               // END_COLLECTION
#endif // LE_HID_KEYBOARD
};

static u8 report_map_ios[]=
{
#if LE_HID_CONSUMER
    0x05, 0x0C,         // Usage Page (Consumer)
    0x09, 0x01,         // Usage (Consumer Control)
    0xA1, 0x01,         // Collection (Application)
    0x85, 0x01,         //   Report Id (1)
    0x1A, 0x00, 0x00,   //   Usage Minimum (Unassigned)
    0x2A, 0x9C, 0x02,   //   Usage Maximum (AC Distribute Vertically)
    0x16, 0x00, 0x00,   //   Logical minimum (0)
    0x26, 0x9C, 0x02,   //   Logical maximum (668)
    0x75, 0x10,         //   Report Size (16)
    0x95, 0x01,         //   Report Count (1)
    0x81, 0x00,         //   Input (Data,Array,Absolute,Bit Field)
    0xC0,               // End Collection
#endif // LE_HID_CONSUMER

#if LE_HID_DIGITIZER
    0x05, 0x0D,         // Usage Page (Digitizer)
    0x09, 0x02,         // Usage (Pen)
    0xA1, 0x01,         // Collection (Application)
    0x85, 0x02,         //   Report ID (2)
    0x09, 0x22,         //   Usage (Finger)
    0xA1, 0x02,         //   Collection (Logical)
    0x09, 0x42,         //     Usage (Tip Switch)
    0x15, 0x00,         //     Logical Minimum (0)
    0x25, 0x01,         //     Logical Maximum (1)
    0x75, 0x01,         //     Report Size (1)
    0x95, 0x01,         //     Report Count (1)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x32,         //     Usage (In Range)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x06,         //     Report Count (6)
    0x81, 0x03,         //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x75, 0x08,         //     Report Size (8)
    0x09, 0x51,         //     Usage (Contact identifier)
    0x95, 0x01,         //     Report Count (1)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,         //     Usage Page (Generic Desktop Ctrls)
    0x26, 0xFF, 0x0F,   //     Logical Maximum (4095)
    0x75, 0x10,         //     Report Size (16)
    0x55, 0x0E,         //     Unit Exponent (-2)
    0x65, 0x33,         //     Unit (System: English Linear, Length: Inch)
    0x09, 0x30,         //     Usage (X)
    0x35, 0x00,         //     Physical Minimum (0)
    0x46, 0xB5, 0x04,   //     Physical Maximum (1205)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x46, 0x8A, 0x03,   //     Physical Maximum (906)
    0x09, 0x31,         //     Usage (Y)
    0x81, 0x02,         //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,               //   End Collection
    0x05, 0x0D,         //   Usage Page (Digitizer)
    0x09, 0x54,         //   Usage (Contact count)
    0x95, 0x01,         //   Report Count (1)
    0x75, 0x08,         //   Report Size (8)
    0x81, 0x02,         //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x85, 0x08,         //   Report ID (8)
    0x09, 0x55,         //   Usage (0x55)
    0x25, 0x05,         //   Logical Maximum (5)
    0xB1, 0x02,         //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,               // End Collection
#endif // LE_HID_DIGITIZER

#if LE_HID_MOUSE
    0x05, 0x01,         // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,         // USAGE (Mouse)
    0xa1, 0x01,         // COLLECTION (Application)
    0x09, 0x01,         //   USAGE (Pointer)
    0xa1, 0x00,         //   COLLECTION (Physical)
    0x85, 0x03,         //     REPORT_ID (3)
    0x05, 0x09,         //     USAGE_PAGE (Button)
    0x19, 0x01,         //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,         //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,         //     LOGICAL_MINIMUM (0)
    0x25, 0x01,         //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,         //     REPORT_SIZE (1)
    0x95, 0x03,         //     REPORT_COUNT (3)
    0x81, 0x02,         //     INPUT (Data,Var,Abs)
    0x75, 0x05,         //     REPORT_SIZE (5)
    0x95, 0x01,         //     REPORT_COUNT (1)
    0x81, 0x01,         //     INPUT (Cnst,Ary,Abs)
    0x05, 0x01,         //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,         //     USAGE (X)
    0x09, 0x31,         //     USAGE (Y)
    0x35, 0x00,         //     Physical minimum
    0x46, 0xa0, 0x00,   //     Physical maximum
    0x15, 0x81,         //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,         //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,         //     REPORT_SIZE (8)
    0x95, 0x02,         //     REPORT_COUNT (2)
    0x81, 0x06,         //     INPUT (Data,Var,Rel)
    0xc0,               //   END_COLLECTION
    0xc0,               // END_COLLECTION
#endif

#if LE_HID_KEYBOARD
    0x05, 0x01,         // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,         // USAGE (Keyboard)
    0xA1, 0x01,         // COLLECTION (Application)
    0x85, 0x04,         //   Report Id (4)
    0x05, 0x07,         //   USAGE_PAGE (Keyboard)
    0x19, 0xE0,         //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xE7,         //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,         //   LOGICAL_MINIMUM (0)
    0x25, 0x01,         //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,         //   REPORT_SIZE (1)
    0x95, 0x08,         //   REPORT_COUNT (8)
    0x81, 0x02,         //   INPUT (Data,Var,Abs)
    0x95, 0x01,         //   REPORT_COUNT (1)
    0x75, 0x08,         //   REPORT_SIZE (8)
    0x15, 0x00,         //   LOGICAL_MINIMUM (0)
    0x25, 0x65,         //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,         //   USAGE_PAGE (Keyboard)
    0x19, 0x00,         //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,         //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,         //   INPUT (Data,Ary,Abs)
    0xC0,               // END_COLLECTION
#endif // LE_HID_KEYBOARD
};

// Device Information service
static const uint8_t di_service_primay_uuid[2] = {0x0A, 0x18};
static const gatts_uuid_base_st di_primary_base =
{
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = di_service_primay_uuid,
};

// PnP ID
static const uint8_t pnp_id_uuid[2] = {0x50, 0x2A};
static const gatts_uuid_base_st pnp_id_base =
{
    .props = ATT_READ | ATT_INDICATE,
    .type  = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid  = pnp_id_uuid,
};
const uint8_t pnp_info[7] = {0x02, 0xAC, 0x05, 0x2C, 0x02, 0x01, 0x00};
static ble_gatt_characteristic_cb_info_t gatts_pnp_id_base_cb_info =
{
    .client_config = GATT_CLIENT_CONFIG_INDICATE,
    .value         = (uint8_t *)pnp_info,
    .value_len     = sizeof(pnp_info),
};

// Human Interface Device service
static const uint8_t hid_service_primay_uuid[2] = {0x12, 0x18};
static const gatts_uuid_base_st hid_primary_base =
{
    .type = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid = hid_service_primay_uuid,
};

// HID Information
static const uint8_t hid_information_uuid[2] = {0x4A, 0x2A};
static const gatts_uuid_base_st hid_info_base =
{
    .props = ATT_READ | ATT_WRITE_WITHOUT_RESPONSE,
    .type  = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid  = hid_information_uuid,
};
const uint8_t hid_info[4] = {0x11, 0x01, 0x00, 0x00};
static ble_gatt_characteristic_cb_info_t gatts_hid_information_cb_info =
{
    .value         = (uint8_t *)hid_info,
    .value_len     = sizeof(hid_info),
};

// Report Map
static const uint8_t hid_report_map_uuid[2] = {0x4B, 0x2A};
static const gatts_uuid_base_st hid_rpt_map_base =
{
    .props = ATT_READ | ATT_DYNAMIC | 0xF000,
    .type  = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid  = hid_report_map_uuid,
};
static ble_gatt_characteristic_cb_info_t gatts_hid_rpt_map_cb_info =
{
    .att_read_callback_func = hid_report_map_read_callback,
};

// HID Control Point
static const uint8_t hid_control_point_uuid[2] = {0x4C, 0x2A};
static const gatts_uuid_base_st hid_ctrl_base =
{
    .props = ATT_WRITE_WITHOUT_RESPONSE,
    .type  = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid  = hid_control_point_uuid,
};

// Report
static const uint8_t hid_report_uuid[2] = {0x4D, 0x2A};
static const gatts_uuid_base_st hid_rpt_base =
{
    .props = ATT_READ | ATT_NOTIFY,
    .type  = BLE_GATTS_UUID_TYPE_16BIT,
    .uuid  = hid_report_uuid,
};
static ble_gatt_characteristic_cb_info_t gatts_hid_rpt_base_cb_info =
{
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
};

static gatts_service_base_st hid_rpt1_gatts;
static gatts_service_base_st hid_rpt2_gatts;
static gatts_service_base_st hid_rpt3_gatts;
static gatts_service_base_st hid_rpt4_gatts;

// Report Reference
static const uint8_t report_reference_uuid[2] = {0x08, 0x29};
static const gatts_uuid_base_st rpt_ref_base =
{
    .props = ATT_READ | ATT_NOTIFY | ATT_DYNAMIC,
    .uuid  = report_reference_uuid,
};
const uint8_t rpt1_ref_data[2] = {0x01, 0x1};
const uint8_t rpt2_ref_data[2] = {0x02, 0x1};
const uint8_t rpt3_ref_data[2] = {0x03, 0x1};
const uint8_t rpt4_ref_data[2] = {0x04, 0x1};

static ble_gatt_characteristic_cb_info_t gatts_rpt1_ref_cb_info =
{
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
    .value         = (uint8_t *)rpt1_ref_data,
    .value_len     = sizeof(rpt1_ref_data),
};
static ble_gatt_characteristic_cb_info_t gatts_rpt2_ref_cb_info =
{
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
    .value         = (uint8_t *)rpt2_ref_data,
    .value_len     = sizeof(rpt2_ref_data),
};
static ble_gatt_characteristic_cb_info_t gatts_rpt3_ref_cb_info =
{
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
    .value         = (uint8_t *)rpt3_ref_data,
    .value_len     = sizeof(rpt3_ref_data),
};
static ble_gatt_characteristic_cb_info_t gatts_rpt4_ref_cb_info =
{
    .client_config = GATT_CLIENT_CONFIG_NOTIFY,
    .value         = (uint8_t *)rpt4_ref_data,
    .value_len     = sizeof(rpt4_ref_data),
};

static int hid_report_map_read_callback(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len)
{
    printf("%s\n",__func__);
    u8 hid_report_map_len = 0;
    u8 *p_hid_report_map;

    if (ble_hid_peer_device_is_ios())
    {
        hid_report_map_len = sizeof(report_map_ios);
        p_hid_report_map = report_map_ios;
    }
    else
    {
        hid_report_map_len = sizeof(report_map_android);
        p_hid_report_map = report_map_android;
    }

    if (ptr)
    {
        if (hid_report_map_len < len)
        {
            hid_report_map_len = len;
        }
        memcpy(ptr, p_hid_report_map, hid_report_map_len);
        return hid_report_map_len;
    }

    return 0;
}

int ble_hid_service_init(void)
{
    int ret = BLE_GATTS_SUCCESS;

    memset(&ble_hid_tasks, 0x00, sizeof(ble_hid_tasks_typedef));

    // Device Information service
    ret |= ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                 di_primary_base.uuid,
                                 di_primary_base.type,
                                 NULL);

    ret |= ble_gatts_characteristic_add(pnp_id_base.uuid,
                                        pnp_id_base.type,
                                        pnp_id_base.props,
                                        NULL,
                                        &gatts_pnp_id_base_cb_info);

    // Human Interface Device service
    ret |= ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                 hid_primary_base.uuid,
                                 hid_primary_base.type,
                                 NULL);

    ret |= ble_gatts_characteristic_add(hid_info_base.uuid,
                                        hid_info_base.type,
                                        hid_info_base.props,
                                        NULL,
                                        &gatts_hid_information_cb_info);

    ret |= ble_gatts_characteristic_add(hid_ctrl_base.uuid,
                                        hid_ctrl_base.type,
                                        hid_ctrl_base.props,
                                        NULL,
                                        NULL);

    ret |= ble_gatts_characteristic_add(hid_rpt_map_base.uuid,
                                        hid_rpt_map_base.type,
                                        hid_rpt_map_base.props,
                                        NULL,
                                        &gatts_hid_rpt_map_cb_info);

    ret |= ble_gatts_characteristic_add(hid_rpt_base.uuid,
                                        hid_rpt_base.type,
                                        hid_rpt_base.props,
                                        &hid_rpt1_gatts.handle,
                                        &gatts_hid_rpt_base_cb_info);

    ret |= ble_gatts_descriptor_add(rpt_ref_base.uuid,
                                    rpt_ref_base.props,
                                    NULL,
                                    &gatts_rpt1_ref_cb_info);

    ret |= ble_gatts_characteristic_add(hid_rpt_base.uuid,
                                        hid_rpt_base.type,
                                        hid_rpt_base.props,
                                        &hid_rpt2_gatts.handle,
                                        &gatts_hid_rpt_base_cb_info);

    ret |= ble_gatts_descriptor_add(rpt_ref_base.uuid,
                                    rpt_ref_base.props,
                                    NULL,
                                    &gatts_rpt2_ref_cb_info);

    ret |= ble_gatts_characteristic_add(hid_rpt_base.uuid,
                                        hid_rpt_base.type,
                                        hid_rpt_base.props,
                                        &hid_rpt3_gatts.handle,
                                        &gatts_hid_rpt_base_cb_info);

    ret |= ble_gatts_descriptor_add(rpt_ref_base.uuid,
                                    rpt_ref_base.props,
                                    NULL,
                                    &gatts_rpt3_ref_cb_info);

    ret |= ble_gatts_characteristic_add(hid_rpt_base.uuid,
                                        hid_rpt_base.type,
                                        hid_rpt_base.props,
                                        &hid_rpt4_gatts.handle,
                                        &gatts_hid_rpt_base_cb_info);

    ret |= ble_gatts_descriptor_add(rpt_ref_base.uuid,
                                    rpt_ref_base.props,
                                    NULL,
                                    &gatts_rpt4_ref_cb_info);

    printf("%s,ret:%d\n",__func__,ret);
    return ret;
}

bool ble_hid_task_enqueue(BLE_HID_CMD_ID_TYPEDEF id, u8 *buffer, u8 len)
{
    u8 next_head = (ble_hid_tasks.head_idx + 1) % BLE_HID_CMD_QUEUE_SIZE;

    if (next_head == ble_hid_tasks.rear_idx)
    {
        /* Queue is full, do nothing. */
        return false;
    }
    else
    {
        ble_hid_tasks.queue[ble_hid_tasks.head_idx].id = id;
        ble_hid_tasks.queue[ble_hid_tasks.head_idx].len = len;
        memcpy(ble_hid_tasks.queue[ble_hid_tasks.head_idx].buffer, buffer, len);
        ble_hid_tasks.head_idx = next_head;
    }

    return true;
}

// 循环中不断执行
void ble_hid_service_proc(void)
{
    static u32 delay_cnt;

    if (tick_check_expire(delay_cnt, BLE_HID_POLLING_INTERVAL_MS))
    {
        if (ble_hid_tasks.head_idx != ble_hid_tasks.rear_idx)
        {
            delay_cnt = tick_get();
            ble_hid_cmd_typedef *ble_hid_cmd = &ble_hid_tasks.queue[ble_hid_tasks.rear_idx];

            switch (ble_hid_cmd->id)
            {
#if LE_HID_CONSUMER
                case BLE_HID_CMD_ID_CONSUMER:
                    TRACE("[TASK] consumer report:");
                    TRACE_R(ble_hid_cmd->buffer, ble_hid_cmd->len);
                    printf("handle:%d\n",hid_rpt1_gatts.handle);
                    ble_tx_notify(hid_rpt1_gatts.handle, ble_hid_cmd->buffer, ble_hid_cmd->len);
                    break;
#endif

#if LE_HID_DIGITIZER
                case BLE_HID_CMD_ID_DIGITIZER:
                    TRACE("[TASK] digitizer report:");
                    TRACE_R(ble_hid_cmd->buffer, ble_hid_cmd->len);
                    printf("handle:%d\n",hid_rpt2_gatts.handle);
                    ble_tx_notify(hid_rpt2_gatts.handle, ble_hid_cmd->buffer, ble_hid_cmd->len);
                    break;
#endif

#if LE_HID_MOUSE
                case BLE_HID_CMD_ID_MOUSE:
                    TRACE("[TASK] mouse report:");
                    TRACE_R(ble_hid_cmd->buffer, ble_hid_cmd->len);
                    printf("handle:%d\n",hid_rpt3_gatts.handle);
                    ble_tx_notify(hid_rpt3_gatts.handle, ble_hid_cmd->buffer, ble_hid_cmd->len);
                    break;
#endif

#if LE_HID_KEYBOARD
                case BLE_HID_CMD_ID_KEYBOARD:
                    TRACE("[TASK] keyboard report:");
                    TRACE_R(ble_hid_cmd->buffer, ble_hid_cmd->len);
                    printf("handle:%d\n",hid_rpt4_gatts.handle);
                    ble_tx_notify(hid_rpt4_gatts.handle, ble_hid_cmd->buffer, ble_hid_cmd->len);
#endif

                default:
                    break;
            }

            ble_hid_tasks.rear_idx = (ble_hid_tasks.rear_idx + 1) % BLE_HID_CMD_QUEUE_SIZE;
        }
    }
}

bool ble_hid_peer_device_is_ios(void)
{
    return true;
}

