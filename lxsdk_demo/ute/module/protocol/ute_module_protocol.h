/**
*@file
*@brief     协议模块
*@details   和应用端通讯协议
*@author        zn.zeng
*@date        2021-08-17
*@version       v1.0
*/

#ifndef _UTE_MODULE_PROTOCOL_H_
#define _UTE_MODULE_PROTOCOL_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_profile_ble.h"

#define WURAO_NO_SCREEN_MODE                0x01
#define WURAO_NO_MOTOR_MODE                 0x02
#define WURAO_NO_NOTIF_MODE                 0x04
#define WURAO_NO_CALL_MODE                  0x08


#define CMD_SET_PARAM_24HOUR_KM             0xA0
#define CMD_READ_SW_VERSION                 0xA1
#define CMD_READ_BATTERY_LVL                0xA2
#define CMD_SET_PARAM_DATE_TIME             0xA3
#define CMD_SET_BLENAME                     0xA8
#define CMD_SET_PARAM_STEP_SOON             0xA9
#define CMD_READ_SYSTEM_STATUS              0xAA
#define CMD_MOTOR_ALARM_PARAM               0xAB
#define CMD_FACTORY_RESET                   0xAD
#define CMD_READ_MAC                        0xAE
#define CMD_SET_MULTIPLE_LANGUAGE           0xAF
#define CMD_READ_HOUR_STEP_DATA             0xB1
#define CMD_START_SYN_SETP_DATA             0xB2
#define CMD_START_SYN_SLEEP_DATA            0xB3
#define CMD_READ_SWIMMING_DATA              0xB7
#define CMD_SET_SWIMMING                    0xB8
#define CMD_READ_JUMP_ROPE_DATA             0xB9
#define CMD_UV_TEST_AND_READ                0xBA
#define CMD_READ_MSG_CONTENT_SUPPORT        0xBB
#define CMD_SET_JUMP_ROPE                   0xBC
#define CMD_SET_HEART_HEADPHONE             0xBD
#define CMD_QUICK_SWITCH                    0xBE
#define CMD_CALL_STATUS                     0xC1
#define CMD_TAKE_PICTURE_MODE               0xC4
#define CMD_MSG_CONTENT_MODE0               0xC5
#define CMD_MSG_CONTENT_MODE1               0xC6
#define CMD_SET_BP_THEORY_TEST              0xC7
#define CMD_START_BP_THEORY_DATA            0xC8
#define CMD_SET_CURR_DAY_WEATHER_PARAM      0XCA
#define CMD_SET_7_DAYS_WEATHER_PARAM        0XCB
#define CMD_SEND_KEYCODE                    0xD1
#define CMD_SET_ANCS_PARAM                  0xD2
#define CMD_SET_JIUZUO_PARAM                0xD3
#define CMD_SET_DRINK_WATER_PARAM           0xD4
#define CMD_SET_HEART_MODE                  0xD6
#define CMD_WURAO_PARAM_MODE                0xD7
#define CMD_MOBILE_PLATFORM                 0xD8  //Casen 2020-04-08
#define CMD_MODIFY_SLEEP_TIME               0xDA
#define CMD_SOCIAL_APP_SELECT               0xDB
#define CMD_HEART_TEST                      0xE5
#define CMD_START_SYN_HEART_DATA            0xE6
#define CMD_FACTORY_GSENSOR_TEST            0xE7
#define CMD_FACTORY_POWER_OFF               0XE8
#define CMD_BODY_COMPOSITION_ANALYSIS       0xE9
#define CMD_CYWEE_READ_STEP_DATA            0xEB  //Casen add 22-11-03 赛维同步步数数据指令
#define CMD_CYWEE_READ_NAPSLEEP_DATA        0xEC  //Casen add 22-11-03 赛维同步睡眠数据指令
#define CMD_RIDE_BIKE                       0xF3
#define CMD_PLSY_BALL                       0xF4
#define CMD_USER_CUSTOM_SCREEN              0xF9

#define CMD_CUSTOM_ID                       0xF1
#define CMD_SN_ID                           0xF2

#define CMD_CUST_DEFINE_CMD                 0xF6
#define CMD_HEART_24HOURS_AUTO_TEST         0xF7
#define CMD_HEART_HAND_WEAR_CHECK_CALIBATION 0xFB

#define CMD_SPORT_MODE_AND_SPORT_HEART_RATE 0XFD//ellison  mark May 23, 2018

#define CMD_DEBUG_TEST_DATA                 0xFE     //数据采样，调试协议，不对外开放

#define CMD_FACTORY_TEST_MODE               0x30
#define CMD_SET_WOMEN_MENSTRUAL_CYCLE       0x29
#define CMD_WATCH_ONLINE                    0x26
#define CMD_WATCH_ONLINE_BLE5_0             0X27

#define CMD_SLEEP_ON_BAND                   0x31
#define CMD_SEND_SLEEP_ON_BAND_DATAS        0X32

#define CMD_USER_ID_FOR_BINDING             0X33

#define CMD_SPO2_TEST                       0x34
#define CMD_START_SYN_SPO2_DATA             0x35
#define CMD_BREATH_RATE_TEST                0X3B

#define CMD_MUSIC_CONTENT_CTRL              0x3A

#define CMD_VITA_BP_DEMARCATE_CTRL          0x3E

#define CMD_TODAY_TARGET_CTRL               0x3F

//hkp  mark April 28, 2020
#define CMD_TEMPERATURE_HEAD                0x24

#define CMD_ECG_DATA_CMD                    0x28

#define CMD_GNSS_CTRL                       0x47
#define CMD_GNSS_DATA                       0x48

#define CMD_SYNC_CONTACTS                   0x37
#define CMD_BT30_CTRL                       0x38

#define CMD_EMOTION_PRESSURE_TEST           0x44

#define CMD_QUICK_REPLY_GET_PHONE_NUMBER    0x45
#define CMD_QUICK_REPLY_CONTENT             0x46
#define CMD_SYNC_CYWEE_SWIM_DATA            0x4A

#define CMD_SPORTS_TARGET_SELECT            0x50
#define CMD_QUICK_REPLY_CTRL                0x52
#define CMD_SET_BP_DN02_TEST                0x55
#define CMD_SET_BLOODSUGAR_TEST             0x5B
#define CMD_SET_HRV_TEST                    0x5C
#define CMD_SET_VARIOUS_MEASURE_TEST        0x5D
#define CMD_ALIUPAY_CTRL                    0x60
#define CMD_METABOLISM_CTRL                 0x66
#define CMD_GOTO_SCREEN                     0x9F

#define PUBLIC_CMD_SET_PARAM_24HOUR_KM      0x01
#define PUBLIC_CMD_READ_SW_VERSION          0x02
#define PUBLIC_CMD_READ_BATTERY_LVL         0x03
#define PUBLIC_CMD_SET_PARAM_DATE_TIME      0x04
//#define PUBLIC_CMD_SET_BLENAME        0xA8
#define PUBLIC_CMD_SET_PARAM_STEP_SOON      0x05
//#define   PUBLIC_CMD_READ_SYSTEM_STATUS   0xAA
#define PUBLIC_CMD_MOTOR_ALARM_PARAM        0x06
#define PUBLIC_CMD_FACTORY_RESET            0x07
#define PUBLIC_CMD_READ_MAC                 0x08
//#define PUBLIC_CMD_SET_MULTIPLE_LANGUAGE  0xAF
#define PUBLIC_CMD_READ_HOUR_STEP_DATA      0x09
#define PUBLIC_CMD_START_SYN_SETP_DATA      0x0A
#define PUBLIC_CMD_START_SYN_SLEEP_DATA     0x0B
//#define   PUBLIC_CMD_READ_SWIMMING_DATA       0xB7
//#define   PUBLIC_CMD_SET_SWIMMING             0xB8
//#define   PUBLIC_CMD_READ_JUMP_ROPE_DATA      0xB9
//#define   PUBLIC_CMD_UV_TEST_AND_READ         0xBA
//#define   PUBLIC_CMD_READ_MSG_CONTENT_SUPPORT 0xBB
//#define   PUBLIC_CMD_SET_JUMP_ROPE            0xBC
//#define PUBLIC_CMD_SET_HEART_HEADPHONE        0xBD
#define PUBLIC_CMD_QUICK_SWITCH             0x0C
#define PUBLIC_CMD_CALL_STATUS              0x0D
#define PUBLIC_CMD_TAKE_PICTURE_MODE        0x0E
#define PUBLIC_CMD_MSG_CONTENT_MODE0        0x0F
#define PUBLIC_CMD_MSG_CONTENT_MODE1        0x10
//#define   PUBLIC_CMD_SET_BP_THEORY_TEST   0xC7
//#define   PUBLIC_CMD_START_BP_THEORY_DATA 0xC8
//#define   PUBLIC_CMD_SET_CURR_DAY_WEATHER_PARAM   0XCA
#define PUBLIC_CMD_SET_7_DAYS_WEATHER_PARAM 0X11
#define PUBLIC_CMD_SEND_KEYCODE             0x12
#define PUBLIC_CMD_SET_ANCS_PARAM           0x13
#define PUBLIC_CMD_SET_JIUZUO_PARAM         0x14
//#define   PUBLIC_CMD_SET_HEART_MODE       0xD6
#define PUBLIC_CMD_WURAO_PARAM_MODE         0x15
//#define PUBLIC_CMD_MODIFY_SLEEP_TIME      0xDA
#define PUBLIC_CMD_HEART_TEST               0x16
//#define   PUBLIC_CMD_START_SYN_HEART_DATA 0xE6
#define PUBLIC_CMD_FACTORY_POWER_OFF        0X17
//#define PUBLIC_CMD_CUSTOM_ID              0xF1
//#define PUBLIC_CMD_SN_ID                  0xF2
//#define PUBLIC_CMD_RIDE_BIKE              0xF3
//#define PUBLIC_CMD_PLSY_BALL              0xF4
#define PUBLIC_CMD_HEART_24HOURS_AUTO_TEST  0x18
//#define PUBLIC_CMD_CUST_DEFINE_CMD        0xF6
//#define   PUBLIC_CMD_USER_CUSTOM_SCREEN   0xF9
//#define PUBLIC_CMD_HEART_HAND_WEAR_CHECK_CALIBATION 0xFB
//#define PUBLIC_CMD_BODY_COMPOSITION_ANALYSIS  0xE9


#define PUBLIC_CMD_SPORT_MODE_AND_SPORT_HEART_RATE  0X19//ellison  mark May 23, 2018

//#define PUBLIC_CMD_FACTORY_TEST_MODE              0x30
//#define PUBLIC_CMD_SET_WOMEN_MENSTRUAL_CYCLE      0x29
#define PUBLIC_CMD_WATCH_ONLINE             0x1A
#define PUBLIC_CMD_WATCH_ONLINE_BLE5_0      0X1B
#define PUBLIC_CMD_FACTORY_TEST_MODE        0x1C
#define PUBLIC_CMD_SLEEP_ON_BAND            0x1D
#define PUBLIC_CMD_SEND_SLEEP_ON_BAND_DATAS 0x1E
#define PUBLIC_CMD_TEST_DATAS               0X1F
#define PUBLIC_CMD_USER_ID_FOR_BINDING      0X20
#define PUBLIC_CMD_MOBILE_PLATFORM          0X21

#define PUBLIC_CMD_SET_MULTIPLE_LANGUAGE    0x25

#define PUBLIC_CMD_GNSS_CTRL                0x81
#define PUBLIC_CMD_GNSS_DATA                0x82

#define PUBLIC_CMD_NFC_CTRL                 0x83
#define PUBLIC_CMD_NFC_DATA                 0x84
#define PUBLIC_CMD_SPO2_TEST                0x86

/*! 指令转化类型枚举zn.zeng, 2021-08-17*/
typedef enum
{
    TYPE_FIND_INDEX,
    TYPE_TO_PUBLIC,
    TYPE_TO_PRIVATE,
} ute_module_protocol_conversion_type_t;

typedef void (*ute_module_protocol_cmd_func_t)(uint8_t*receive,uint8_t length);

/*! 指令转化表数据结构zn.zeng, 2021-08-17*/
typedef struct
{
    uint8_t privateCmd;
    uint8_t publicCmd;
    ute_module_protocol_cmd_func_t function;
} ute_module_protocol_cmd_list_t;
uint16_t uteModuleProtocolConversionCmd(uint8_t *cmdHead,ute_module_protocol_conversion_type_t tpye);
void uteModuleProtocolFromPhone(uint8_t *receive,uint8_t length,bool isPublic);
void uteModuleProtocolReadFunctionSupport(uint8_t *data,uint8_t size);
void uteModuleProtocolMoreSportCtrl(uint8_t*receive,uint8_t length);
void uteModuleProtocolReadExpandFunctionSupport(uint8_t *data,uint8_t size);
#endif //_UTE_MODULE_PROTOCOL_H_

