#include "ble_hid_service.h"

bool ble_hid_send_touch_cmd(u8 touch, u16 x, u16 y)
{
    ble_digitizer_report_typedef ble_digitizer_report;
    memset(&ble_digitizer_report, 0x00, sizeof(ble_digitizer_report_typedef));
    ble_digitizer_report.tip_switch = touch;
    ble_digitizer_report.in_range = touch;
    ble_digitizer_report.contact_identifier = 0;
    ble_digitizer_report.x = x;
    ble_digitizer_report.y = y;
    ble_digitizer_report.contact_count = touch;

    return ble_hid_task_enqueue(BLE_HID_CMD_ID_DIGITIZER, (u8 *)&ble_digitizer_report, sizeof(ble_digitizer_report_typedef));
}

//void ble_hid_send_touch_cmd_kick(u8 touch, u16 x, u16 y, u8 kick_cfg)
//{
//    ble_digitizer_report_typedef ble_digitizer_report;
//    ble_digitizer_report.tip_switch = touch;
//    ble_digitizer_report.in_range = touch;
//    ble_digitizer_report.contact_identifier = 0;
//    ble_digitizer_report.x = x;
//    ble_digitizer_report.y = y;
//    ble_digitizer_report.contact_count = touch;
//
//    ble_hid_report_for_handle_kick(ATT_CHARACTERISTIC_2a4d_02_VALUE_HANDLE, (u8*)&ble_digitizer_report, sizeof(ble_digitizer_report), kick_cfg);
//}

bool ble_hid_send_mouse_cmd(bool click, s8 x, s8 y)
{
    ble_mouse_report_typedef ble_mouse_report;
    memset(&ble_mouse_report, 0x00, sizeof(ble_mouse_report_typedef));
    ble_mouse_report.button1 = click;
    ble_mouse_report.button2 = 0;
    ble_mouse_report.button3 = 0;
    ble_mouse_report.x = x;
    ble_mouse_report.y = y;

    return ble_hid_task_enqueue(BLE_HID_CMD_ID_MOUSE, (u8 *)&ble_mouse_report, sizeof(ble_mouse_report_typedef));
}

bool ble_hid_send_consumer_cmd(u16 control_cmd)
{
    return ble_hid_task_enqueue(BLE_HID_CMD_ID_CONSUMER, (u8 *)&control_cmd, 2);
}

bool ble_hid_send_keyboard_cmd(u8 modifier, u8 key_code)
{
    ble_keyboard_report_typedef ble_keyboard_report;
    memset(&ble_keyboard_report, 0x00, sizeof(ble_keyboard_report_typedef));
    ble_keyboard_report.modifier = modifier;
    ble_keyboard_report.key_code = key_code;

    return ble_hid_task_enqueue(BLE_HID_CMD_ID_KEYBOARD, (u8 *)&ble_keyboard_report, sizeof(ble_keyboard_report_typedef));
}

void ble_hid_send_mouse_location_cmd(u8 dir)
{
/*
    0-------1
    |   +   |
    2-------3
*/
    u8 idx = dir % 4;
    u8 location[][4] = {
        {HID_MOUSE_X_MIN, HID_MOUSE_Y_MIN},
        {HID_MOUSE_X_MAX, HID_MOUSE_Y_MIN},
        {HID_MOUSE_X_MIN, HID_MOUSE_Y_MAX},
        {HID_MOUSE_X_MAX, HID_MOUSE_Y_MAX}
    };

    for (u8 i = 0; i < 4; i++) {
        ble_hid_send_mouse_cmd(0, location[idx][0], location[idx][1]);
    }
}

