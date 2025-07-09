#ifndef __BLE_HID_SERVICE_H
#define __BLE_HID_SERVICE_H

#include "include.h"

#define BLE_HID_CMD_BUFFER_LENGTH           8
#define BLE_HID_CMD_QUEUE_SIZE              30
#define BLE_HID_POLLING_INTERVAL_MS         30

/**
 * @brief The limit for x/y position, this value is determined by report map table.
 */
#define HID_DIGITIZER_X_MIN                 0
#define HID_DIGITIZER_X_MAX                 1000
#define HID_DIGITIZER_Y_MIN                 0
#define HID_DIGITIZER_Y_MAX                 1000

/**
 * @brief The limit for x/y.
 */
#define HID_MOUSE_X_MIN                     -127
#define HID_MOUSE_X_MAX                     127
#define HID_MOUSE_Y_MIN                     -127
#define HID_MOUSE_Y_MAX                     127

/**
 * @brief The keyboard modifier definition.
 */
#define HID_KM_NONE                         0x00
#define HID_KM_LEFT_CTRL                    0x01
#define HID_KM_LEFT_SHIFT                   0x02
#define HID_KM_LEFT_ALT                     0x04
#define HID_KM_LEFT_GUI                     0x08
#define HID_KM_RIGHT_CTRL                   0x01
#define HID_KM_RIGHT_SHIFT                  0x02
#define HID_KM_RIGHT_ALT                    0x04
#define HID_KM_RIGHT_GUI                    0x08

typedef enum {
    BLE_HID_CMD_ID_CONSUMER,
    BLE_HID_CMD_ID_DIGITIZER,
    BLE_HID_CMD_ID_MOUSE,
    BLE_HID_CMD_ID_KEYBOARD,
} BLE_HID_CMD_ID_TYPEDEF;

typedef struct PACKED {
    uint8_t         tip_switch  : 1;
    uint8_t         in_range    : 1;
    const uint8_t   reserve     : 6;
    uint8_t         contact_identifier;
    uint16_t        x;              // X position
    uint16_t        y;              // Y position
    uint8_t         contact_count;
} ble_digitizer_report_typedef;

typedef struct PACKED {
    uint8_t         button1 : 1;    // Left Button
    uint8_t         button2 : 1;    // Right Button
    uint8_t         button3 : 1;    // Central Button
    uint8_t         reserve : 5;
    int8_t          x;              // X offset
    int8_t          y;              // Y offset
} ble_mouse_report_typedef;

typedef struct {
    uint8_t modifier;
    uint8_t key_code;
} ble_keyboard_report_typedef;

typedef struct {
    BLE_HID_CMD_ID_TYPEDEF  id;
    uint8_t                 len;
    uint8_t                 buffer[BLE_HID_CMD_BUFFER_LENGTH];
} ble_hid_cmd_typedef;

typedef struct {
    uint8_t                 head_idx;
    uint8_t                 rear_idx;
    ble_hid_cmd_typedef     queue[BLE_HID_CMD_QUEUE_SIZE];
} ble_hid_tasks_typedef;

#define BLE_HID_MSG_ID_SELFIE           0x0000
#define BLE_HID_MSG_ID_TIKTOK           0x0100
#define BLE_HID_MSG_ID_TEST             0x0200
#define BLE_HID_MSG_ID_MASK             0x0f00

typedef enum {
    BLE_HID_MSG_NONE,

    /* TikTok Page turner Message */
    BLE_HID_MSG_CLICK                   = 0x0101,
    BLE_HID_MSG_DOUBLE,
    BLE_HID_MSG_UP,             //上一个视频
    BLE_HID_MSG_DOWN,           //下一个视频
    BLE_HID_MSG_LEFT,
    BLE_HID_MSG_RIGHT,
    BLE_HID_MSG_ROUGH_MOVE,
    BLE_HID_MSG_CAREFUL_MOVE,
    BLE_HID_MSG_VOLUME_UP,
    BLE_HID_MSG_VOLUME_DOWN,
    BLE_HID_MSG_PP_HOLD,
    BLE_HID_MSG_K1_HOLD,
    BLE_HID_MSG_K2_HOLD,

    BLE_HID_MSG_MASK                    = 0x0fff,
} BLE_HID_MSG_TYPEDEF;

int ble_hid_service_init(void);
bool ble_hid_task_enqueue(BLE_HID_CMD_ID_TYPEDEF id, u8 *buffer, u8 len);
void ble_hid_service_proc(void);
bool ble_hid_peer_device_is_ios(void);

bool ble_hid_send_touch_cmd(u8 touch, u16 x, u16 y);
//void ble_hid_send_touch_cmd_kick(u8 touch, u16 x, u16 y, u8 kick_cfg);

bool ble_hid_send_mouse_cmd(bool click, s8 x, s8 y);
bool ble_hid_send_consumer_cmd(u16 control_cmd);
bool ble_hid_send_keyboard_cmd(u8 modifier, u8 key_code);
void ble_hid_send_mouse_location_cmd(u8 dir);

//抖音神器事件
bool ble_hid_event_tiktok(u16 msg);

#endif
