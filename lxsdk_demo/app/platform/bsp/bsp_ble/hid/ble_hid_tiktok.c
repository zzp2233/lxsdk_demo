#include "ble_hid_service.h"
#include "hid_usage.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

bool ble_hid_event_tiktok(u16 msg)
{
    int android_pos_x = 1500;
    int android_pos_y = 2500;

    uint8_t is_android = (!ble_hid_peer_device_is_ios());

    switch (msg & BLE_HID_MSG_MASK) {
    // 单击
    case BLE_HID_MSG_CLICK:
        TRACE("BLE_HID_MSG_CLICK\n");
        if (is_android) {
            ble_hid_send_touch_cmd(1, android_pos_x, android_pos_y);
            ble_hid_send_touch_cmd(0, android_pos_x, android_pos_y);
        } else {
            //定位
            ble_hid_send_mouse_location_cmd(0);
            ble_hid_send_mouse_cmd(0, 60, 60);
            ble_hid_send_mouse_cmd(0, 0, 60);

            //点击
            ble_hid_send_mouse_cmd(1, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);
        }
        break;

    // 双击
    case BLE_HID_MSG_DOUBLE:
        TRACE("BLE_HID_MSG_DOUBLE\n");
        if (is_android) {
            ble_hid_send_touch_cmd(1, android_pos_x, android_pos_y);
            ble_hid_send_touch_cmd(0, android_pos_x, android_pos_y);

            ble_hid_send_touch_cmd(0, android_pos_x, android_pos_y);
            ble_hid_send_touch_cmd(0, android_pos_x, android_pos_y);
            ble_hid_send_touch_cmd(0, android_pos_x, android_pos_y);

            ble_hid_send_touch_cmd(1, android_pos_x, android_pos_y);
            ble_hid_send_touch_cmd(0, android_pos_x, android_pos_y);
        } else {
            //定位
            ble_hid_send_mouse_location_cmd(0);
            ble_hid_send_mouse_cmd(0, 60, 60);
            ble_hid_send_mouse_cmd(0, 0, 60);

            //点击
            ble_hid_send_mouse_cmd(1, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);

            //等待
            ble_hid_send_mouse_cmd(0, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);

            //点击
            ble_hid_send_mouse_cmd(1, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);
        }
        break;

    // 连点
    case BLE_HID_MSG_PP_HOLD:
        TRACE("BLE_HID_MSG_PP_HOLD\n");
        if (is_android) {
            ble_hid_send_touch_cmd(1, android_pos_x, android_pos_y);
            ble_hid_send_touch_cmd(0, android_pos_x, android_pos_y);
        } else {
            //定位
            ble_hid_send_mouse_location_cmd(0);
            ble_hid_send_mouse_cmd(0, 60, 60);
            ble_hid_send_mouse_cmd(0, 0, 60);

            //点击
            ble_hid_send_mouse_cmd(1, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);
        }
        break;

    case BLE_HID_MSG_K1_HOLD:
        TRACE("BLE_HID_MSG_K1_HOLD\n");
        ble_hid_send_consumer_cmd(HID_CCC_VCR_PLUS);
        ble_hid_send_consumer_cmd(HID_CCC_RELEASE);
        break;

    case BLE_HID_MSG_K2_HOLD:
        TRACE("BLE_HID_MSG_K2_HOLD\n");
        ble_hid_send_consumer_cmd(HID_CCC_AC_HOME);
        ble_hid_send_consumer_cmd(HID_CCC_RELEASE);
        break;

    case BLE_HID_MSG_VOLUME_UP:
        TRACE("BLE_HID_MSG_VOLUME_UP\n");
        ble_hid_send_consumer_cmd(HID_CCC_VOLUME_INCREMENT);
        ble_hid_send_consumer_cmd(HID_CCC_RELEASE);
        break;

    case BLE_HID_MSG_VOLUME_DOWN:
        TRACE("BLE_HID_MSG_VOLUME_DOWN\n");
        ble_hid_send_consumer_cmd(HID_CCC_VOLUME_DECREMENT);
        ble_hid_send_consumer_cmd(HID_CCC_RELEASE);
        break;

    //上一个视频
    case BLE_HID_MSG_UP:
        TRACE("BLE_HID_MSG_UP\n");
        if (is_android) {
            ble_hid_send_touch_cmd(1, android_pos_x, 1300);
            ble_hid_send_touch_cmd(1, android_pos_x, 1600);
            ble_hid_send_touch_cmd(1, android_pos_x, 1900);
            ble_hid_send_touch_cmd(1, android_pos_x, 2200);
            ble_hid_send_touch_cmd(1, android_pos_x, 2500);
            ble_hid_send_touch_cmd(1, android_pos_x, 2800);
            ble_hid_send_touch_cmd(1, android_pos_x, 3100);
            ble_hid_send_touch_cmd(1, android_pos_x, 3400);
            ble_hid_send_touch_cmd(1, android_pos_x, 3600);
            ble_hid_send_touch_cmd(0, android_pos_x, 3800);
            ble_hid_send_touch_cmd(0, android_pos_x, 3800);
        } else {
            //定位
            ble_hid_send_mouse_location_cmd(1);
            ble_hid_send_mouse_cmd(0, -120, 120);

            //滑动
            ble_hid_send_mouse_cmd(1, 0, 50);
            ble_hid_send_mouse_cmd(1, 0, 50);
            ble_hid_send_mouse_cmd(1, 0, 50);
            ble_hid_send_mouse_cmd(0, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);
        }
        break;

    //下一个视频
    case BLE_HID_MSG_DOWN:
        TRACE("BLE_HID_MSG_DOWN\n");
        if (is_android) {
            ble_hid_send_touch_cmd(1, android_pos_x, 3600);
            ble_hid_send_touch_cmd(1, android_pos_x, 3400);
            ble_hid_send_touch_cmd(1, android_pos_x, 3100);
            ble_hid_send_touch_cmd(1, android_pos_x, 3000);
            ble_hid_send_touch_cmd(1, android_pos_x, 2800);
            ble_hid_send_touch_cmd(1, android_pos_x, 2500);
            ble_hid_send_touch_cmd(1, android_pos_x, 2200);
            ble_hid_send_touch_cmd(1, android_pos_x, 1900);
            ble_hid_send_touch_cmd(1, android_pos_x, 1600);
            ble_hid_send_touch_cmd(0, android_pos_x, 1300);
            ble_hid_send_touch_cmd(0, android_pos_x, 1300);
        } else {
            //定位
            ble_hid_send_mouse_location_cmd(2);
            ble_hid_send_mouse_cmd(0, 120, -120);

            //滑动
            ble_hid_send_mouse_cmd(1, 0, -50);
            ble_hid_send_mouse_cmd(1, 0, -50);
            ble_hid_send_mouse_cmd(1, 0, -50);
            ble_hid_send_mouse_cmd(0, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);
        }

        break;

    case BLE_HID_MSG_LEFT:
        TRACE("BLE_HID_MSG_LEFT\n");
        if (is_android) {
            ble_hid_send_touch_cmd(1, 1500, android_pos_y);
            ble_hid_send_touch_cmd(1, 2000, android_pos_y);
            ble_hid_send_touch_cmd(1, 2500, android_pos_y);
            ble_hid_send_touch_cmd(1, 3000, android_pos_y);
            ble_hid_send_touch_cmd(1, 3600, android_pos_y);
            ble_hid_send_touch_cmd(0, 3800, android_pos_y);
            ble_hid_send_touch_cmd(0, 3800, android_pos_y);
        } else {
            //定位
            ble_hid_send_mouse_location_cmd(2);
            ble_hid_send_mouse_cmd(0, 50, -120);

            //滑动
            ble_hid_send_mouse_cmd(1, 50, 0);
            ble_hid_send_mouse_cmd(1, 50, 0);
            ble_hid_send_mouse_cmd(1, 50, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);
        }
        break;

    case BLE_HID_MSG_RIGHT:
        TRACE("BLE_HID_MSG_RIGHT\n");
        if (is_android) {
            ble_hid_send_touch_cmd(1, 2500,  android_pos_y);
            ble_hid_send_touch_cmd(1, 2000,  android_pos_y);
            ble_hid_send_touch_cmd(1, 1500,   android_pos_y);
            ble_hid_send_touch_cmd(1, 1000,   android_pos_y);
            ble_hid_send_touch_cmd(1, 500,   android_pos_y);
            ble_hid_send_touch_cmd(0, 300,  android_pos_y);
            ble_hid_send_touch_cmd(0, 300,  android_pos_y);
        } else {
            //定位
            ble_hid_send_mouse_location_cmd(3);
            ble_hid_send_mouse_cmd(0, -50, -120);

            //滑动
            ble_hid_send_mouse_cmd(1, -50, 0);
            ble_hid_send_mouse_cmd(1, -50, 0);
            ble_hid_send_mouse_cmd(1, -50, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);
            ble_hid_send_mouse_cmd(0, 0, 0);
        }
        break;

    default:
        /* event invalid */
        return false;
    }

    return true;

}

