#ifndef __CONFIG_EXTRA_H__
#define __CONFIG_EXTRA_H__

#include "ute_project_config.h"

/*****************************************************************************
 * Module    : 系统功能配置
 *****************************************************************************/

#ifndef FLASHDB_EN
#define FLASHDB_EN 0
#endif

#ifndef WATCH320_TEST_EN
#define WATCH320_TEST_EN      0
#endif

#define UI_ADDR_BASE(n)                 (FLASH_UI_BASE + n)
#define UI_EX_ADDR_BASE(n)              (0x10000000 + n)        //mask

#ifndef SYS_MODE_BREAKPOINT_EN
#define SYS_MODE_BREAKPOINT_EN      0
#endif


#ifndef SPIFLASH_SPEED_UP_EN
#define SPIFLASH_SPEED_UP_EN         1
#endif


#ifndef WARNING_WAVRES_PLAY
#define WARNING_WAVRES_PLAY              0
#endif

#ifndef MIC_EQ_EN
#define MIC_EQ_EN                        0
#endif

#if (GUI_SELECT == GUI_TFT_320_ST77916)
#define GUI_SCREEN_WIDTH                240
#define GUI_SCREEN_HEIGHT               296
#define GUI_SCREEN_OFS_X                0
#define GUI_SCREEN_OFS_Y                12
#define GUI_USE_TFT
#elif (GUI_SELECT == GUI_TFT_RGBW_320_ST77916)
#define GUI_SCREEN_WIDTH                240
#define GUI_SCREEN_HEIGHT               385
#define GUI_SCREEN_OFS_X                0
#define GUI_SCREEN_OFS_Y                0
#define GUI_USE_TFT
#elif (GUI_SELECT == GUI_TFT_360_GC9C01)
#define GUI_SCREEN_WIDTH                360
#define GUI_SCREEN_HEIGHT               360
#define GUI_SCREEN_OFS_X                0
#define GUI_SCREEN_OFS_Y                0
#define GUI_USE_TFT
#elif (GUI_SELECT == GUI_OLED_466_ICNA3310B)
#define GUI_SCREEN_WIDTH                466
#define GUI_SCREEN_HEIGHT               466
#define GUI_SCREEN_OFS_X                6
#define GUI_SCREEN_OFS_Y                0
#define GUI_USE_OLED
#elif (GUI_SELECT == GUI_TFT_JD9853)
#define GUI_SCREEN_WIDTH                240
#define GUI_SCREEN_HEIGHT               284
#define GUI_SCREEN_OFS_X                0
#define GUI_SCREEN_OFS_Y                0
#define GUI_USE_TFT
#elif (GUI_SELECT == GUI_TFT_SPI)
#if (TFT_SPI_DRIVER == SPI_GC9307_V1)
#define GUI_SCREEN_WIDTH                SPI_GC9307_WIDTH
#define GUI_SCREEN_HEIGHT               SPI_GC9307_HIGHT
#define GUI_SCREEN_OFS_X                SPI_GC9307_OFS_X
#define GUI_SCREEN_OFS_Y                SPI_GC9307_OFS_Y
#define GUI_USE_TFT
#elif (TFT_SPI_DRIVER == SPI_JD9853_V1)
#define GUI_SCREEN_WIDTH                SPI_JD9853_WIDTH
#define GUI_SCREEN_HEIGHT               SPI_JD9853_HIGHT
#define GUI_SCREEN_OFS_X                SPI_JD9835_OFS_X
#define GUI_SCREEN_OFS_Y                SPI_JD9835_OFS_Y
#define GUI_USE_TFT
#elif (TFT_SPI_DRIVER == SPI_ST7789_V1)
#define GUI_SCREEN_WIDTH                SPI_ST7789_WIDTH
#define GUI_SCREEN_HEIGHT               SPI_ST7789_HIGHT
#define GUI_SCREEN_OFS_X                SPI_ST7789_OFS_X
#define GUI_SCREEN_OFS_Y                SPI_ST7789_OFS_Y
#define GUI_USE_TFT
#endif  //#if (TFT_SPI_DRIVER == SPI_GC9307_V1)
#elif (GUI_SELECT == GUI_TFT_170_560_AXS15231B)
#define GUI_SCREEN_WIDTH                120
#define GUI_SCREEN_HEIGHT               480
#define GUI_SCREEN_OFS_X                0//24
#define GUI_SCREEN_OFS_Y                0//40
#elif (GUI_SELECT == GUI_TFT_240_296_NV3030B)
#define GUI_SCREEN_WIDTH                240
#define GUI_SCREEN_HEIGHT               296
#define GUI_SCREEN_OFS_X                0//24
#define GUI_SCREEN_OFS_Y                0//40
#define GUI_USE_TFT
#elif (GUI_SELECT == GUI_TFT_320_385_GV9B71)
#define GUI_SCREEN_WIDTH                320
#define GUI_SCREEN_HEIGHT               386
#define GUI_SCREEN_OFS_X                0//24
#define GUI_SCREEN_OFS_Y                0//40
#define GUI_USE_TFT
#elif (GUI_SELECT == DISPLAY_UTE)
#define GUI_SCREEN_WIDTH                UTE_DRV_SCREEN_WIDTH
#define GUI_SCREEN_HEIGHT               UTE_DRV_SCREEN_HEIGHT
#define GUI_SCREEN_OFS_X                0
#define GUI_SCREEN_OFS_Y                0
#define GUI_USE_TFT
#elif (GUI_SELECT == GUI_TFT_240_JD9853W3)
#define GUI_SCREEN_WIDTH                240
#define GUI_SCREEN_HEIGHT               296
#define GUI_SCREEN_OFS_X                0//24
#define GUI_SCREEN_OFS_Y                0//40
#else
#define GUI_SCREEN_WIDTH                UTE_DRV_SCREEN_WIDTH
#define GUI_SCREEN_HEIGHT               UTE_DRV_SCREEN_HEIGHT
#define GUI_SCREEN_OFS_X                0
#define GUI_SCREEN_OFS_Y                0
#define GUI_USE_TFT
#endif

#define GUI_SCREEN_CENTER_X                 (GUI_SCREEN_WIDTH / 2)
#define GUI_SCREEN_CENTER_Y                 (GUI_SCREEN_HEIGHT / 2)

#if (CTP_SELECT == CTP_CST8X)
#define TP_IIC_ADDR                     0x15
#define TP_IIC_UPDATE_ADDR              0x6A
#elif (CTP_SELECT == CTP_CHSC6X)
#define TP_IIC_ADDR                     0x2E

#endif

#ifndef MODEM_CAT1_EN
#define MODEM_CAT1_EN 0
#endif

#if MODEM_CAT1_EN
#define CALL_MGR_EN 1
#endif

/*****************************************************************************
 * Module    : 音乐功能配置
 *****************************************************************************/
#if !FUNC_MUSIC_EN
#undef  MUSIC_UDISK_EN
#undef  MUSIC_SDCARD_EN

#undef  MUSIC_WAV_SUPPORT
#undef  MUSIC_WMA_SUPPORT
#undef  MUSIC_APE_SUPPORT
#undef  MUSIC_FLAC_SUPPORT
#undef  MUSIC_SBC_SUPPORT
#undef  MUSIC_ID3_TAG_EN
#undef  MUSIC_BREAKPOINT_EN

#define MUSIC_UDISK_EN              0
#define MUSIC_SDCARD_EN             0
#define MUSIC_WAV_SUPPORT           0
#define MUSIC_WMA_SUPPORT           0
#define MUSIC_APE_SUPPORT           0
#define MUSIC_FLAC_SUPPORT          0
#define MUSIC_SBC_SUPPORT           0
#define MUSIC_ID3_TAG_EN            0
#define MUSIC_BREAKPOINT_EN         0
#endif // FUNC_MUSIC_EN

/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#if !FUNC_RECORDER_EN
#undef FUNC_REC_EN
#define FUNC_REC_EN                0
#endif

#if !FUNC_REC_EN
#undef  REC_FAST_PLAY
#undef  BT_HFP_REC_EN
#undef  REC_MP3_SUPPORT
#undef  REC_WAV_SUPPORT
#undef  REC_ADPCM_SUPPORT
#undef  REC_SBC_SUPPORT

#define REC_FAST_PLAY               0
#define BT_HFP_REC_EN               0
#define  REC_MP3_SUPPORT            0
#define  REC_WAV_SUPPORT            0
#define  REC_ADPCM_SUPPORT          0
#define  REC_SBC_SUPPORT            0
#endif //FUNC_REC_EN


#if !CHARGE_EN
#undef  CHARGE_TRICK_EN
#undef  CHARGE_DC_IN
#undef  CHARGE_DC_NOT_PWRON
#define CHARGE_TRICK_EN             0
#define CHARGE_DC_NOT_PWRON         0
#define CHARGE_DC_IN()              0
#endif

#ifndef PWROFF_PRESS_TIME
#define PWROFF_PRESS_TIME           3               //1:大约1秒    3: 1.5s,  6: 2s,  9: 2.5s, 12: 3s
#endif // PWROFF_PRESS_TIME

#ifndef PWRON_PRESS_TIME
#define PWRON_PRESS_TIME            1500           //1.5s
#endif // PWRON_PRESS_TIME

#ifndef USER_MULTI_KEY_TIME
#define USER_MULTI_KEY_TIME             1
#endif // USER_MULTI_KEY_TIME

#ifndef SYS_LIMIT_VOLUME
#define SYS_LIMIT_VOLUME            SYS_INIT_VOLUME
#endif // SYS_LIMIT_VOLUME

#ifndef LINEIN_2_PWRDOWN_TONE_EN
#define LINEIN_2_PWRDOWN_TONE_EN    0   //插入Linein直接关机，是否播放关机提示音。
#endif // LINEIN_2_PWRDOWN_TONE_EN

#ifndef LPWR_WARING_TIMES
#define LPWR_WARING_TIMES           0xff
#endif // LPWR_WARING_TIMES

#if !BUZZER_EN
#undef BUZZER_INIT
#define BUZZER_INIT()
#undef BUZZER_ON
#define BUZZER_ON()
#undef BUZZER_OFF
#define BUZZER_OFF()
#endif

#ifndef PWRON_FRIST_BAT_EN
#define PWRON_FRIST_BAT_EN          1
#endif // PWRON_FRIST_BAT_EN

#if !LOUDSPEAKER_MUTE_EN
#undef LOUDSPEAKER_MUTE_INIT
#define LOUDSPEAKER_MUTE_INIT()
#undef LOUDSPEAKER_MUTE_DIS
#define LOUDSPEAKER_MUTE_DIS()
#undef LOUDSPEAKER_MUTE
#define LOUDSPEAKER_MUTE()
#undef LOUDSPEAKER_UNMUTE
#define LOUDSPEAKER_UNMUTE()
#undef LOUDSPEAKER_UNMUTE_DELAY
#define LOUDSPEAKER_UNMUTE_DELAY        0
#undef LOUDSPEAKER_MUTE_PORT
#define LOUDSPEAKER_MUTE_PORT           IO_NONE
#endif

#if !AMP_CTRL_AB_D_EN
#undef  AMP_CTRL_AB_D_PORT
#undef  AMP_CTRL_AB_D_TYPE
#undef  AMPLIFIER_SEL_INIT
#undef  AMPLIFIER_SEL_D
#undef  AMPLIFIER_SEL_AB

#define AMP_CTRL_AB_D_PORT              IO_NONE
#define AMP_CTRL_AB_D_TYPE              0
#define AMPLIFIER_SEL_INIT()
#define AMPLIFIER_SEL_D()
#define AMPLIFIER_SEL_AB()
#endif

#if !VBAT_DETECT_EN
#undef LPWR_WARNING_VBAT
#undef LPWR_OFF_VBAT
#define LPWR_WARNING_VBAT               0
#define LPWR_OFF_VBAT                   0
#endif

/*****************************************************************************
 * Module    : 蓝牙相关配置
 *****************************************************************************/
#ifndef BT_A2DP_RECON_EN
#define BT_A2DP_RECON_EN            0
#endif

#ifndef BT_BACKSTAGE_EN
#define BT_BACKSTAGE_EN             1   //蓝牙后台管理
#endif

#ifndef BT_BACKSTAGE_MUSIC_EN
#define BT_BACKSTAGE_MUSIC_EN       1   //蓝牙音乐支持后台播放
#endif


#if !LE_EN
#undef LE_PAIR_EN
#undef LE_SM_SC_EN
#undef LE_AB_LINK_APP_EN
#undef LE_BQB_RF_EN
#define LE_PAIR_EN              0
#define LE_SM_SC_EN             0
#define LE_AB_LINK_APP_EN       0
#define LE_BQB_RF_EN            0
#endif

#if LE_BQB_RF_EN
#undef BT_FCC_TEST_EN
#undef USER_KEY_QDEC_EN
#define BT_FCC_TEST_EN          1   //LE_BQB和FCC共用huart_init
#define USER_KEY_QDEC_EN        0
#endif

#if LE_EN
#define BT_DUAL_MODE_EN         1
#else
#define BT_DUAL_MODE_EN         0
#endif

#if USE_APP_TYPE
#undef LE_AB_LINK_APP_EN
#define LE_AB_LINK_APP_EN           0
#endif // USE_APP_TYPE

#ifndef BT_PAIR_SLEEP_EN
#define BT_PAIR_SLEEP_EN            1
#endif // BT_PAIR_SLEEP_EN

#ifndef BT_HFP_RINGS_BEFORE_NUMBER
#define BT_HFP_RINGS_BEFORE_NUMBER   0
#endif


#if BT_FCC_TEST_EN    //FCC 默认PB3 (USB_DP) 波特率1500000通信, 关闭用到PB3的程序
#undef FUNC_USBDEV_EN
#undef MUSIC_UDISK_EN
#define FUNC_USBDEV_EN             0
#define MUSIC_UDISK_EN             0
#if (UART0_PRINTF_SEL == PRINTF_PB3)
#undef UART0_PRINTF_SEL
#define UART0_PRINTF_SEL  PRINTF_NONE
#endif
#endif

#if BT_FCC_TEST_EN
#undef WORK_MODE
#if LE_BQB_RF_EN
#define WORK_MODE           MODE_BQB_RF_BLE
#else
#define WORK_MODE           MODE_FCC_TEST
#endif
#elif BT_BQB_RF_EN
#define WORK_MODE           MODE_BQB_RF_BREDR
#elif LE_BQB_RF_EN
#define WORK_MODE           MODE_BQB_RF_BLE
#elif BT_DUT_MODE_EN
#define WORK_MODE           MODE_NORMAL_DUT
#else
#define WORK_MODE           MODE_NORMAL
#endif

#if !BT_HID_EN
#undef BT_HID_TYPE
#undef BT_HID_MENU_EN
#undef BT_HID_DISCON_DEFAULT_EN
#define BT_HID_TYPE                 0
#define BT_HID_MENU_EN              0
#define BT_HID_DISCON_DEFAULT_EN    0
#endif

/*****************************************************************************
 * Module    :  UART功能选择
 *****************************************************************************/
#if BT_SCO_DUMP_EN || BT_SCO_FAR_DUMP_EN || BT_SCO_EQ_DUMP_EN
#undef  EQ_DBG_IN_UART
#define EQ_DBG_IN_UART              1
#endif

/*****************************************************************************
 * Module    : uart0 printf 功能自动配置(自动关闭SD卡，USB)
 *****************************************************************************/
#if (UART0_PRINTF_SEL == PRINTF_PB3)
#undef  FUNC_USBDEV_EN
#undef  MUSIC_UDISK_EN
#define FUNC_USBDEV_EN          0
#define MUSIC_UDISK_EN          0
#elif (UART0_PRINTF_SEL == PRINTF_NONE)
//关闭所以打印信息
#undef printf
#undef vprintf
#undef print_r
#undef print_r16
#undef print_r32
#undef printk
#undef vprintk
#undef print_kr
#undef print_kr16
#undef print_kr32

#define printf(...)
#define vprintf(...)
#define print_r(...)
#define print_r16(...)
#define print_r32(...)
#define printk(...)
#define vprintk(...)
#define print_kr(...)
#define print_kr16(...)
#define print_kr32(...)
#endif

#if !MUSIC_SDCARD_EN
#undef SDCLK_MUX_DETECT_SD
#undef SD_DETECT_INIT
#undef SD_IS_ONLINE
#undef IS_DET_SD_BUSY

#define SD_DETECT_INIT()
#define SDCLK_MUX_DETECT_SD         0
#define SD_IS_ONLINE()              0
#define IS_DET_SD_BUSY()            0
#endif

/*****************************************************************************
 * Module    : usb device 功能配置
 *****************************************************************************/
#if !FUNC_USBDEV_EN
#undef  UDE_STORAGE_EN
#undef  UDE_SPEAKER_EN
#undef  UDE_HID_EN
#undef  UDE_MIC_EN
#undef  UDE_ENUM_TYPE

#define UDE_STORAGE_EN              0
#define UDE_SPEAKER_EN              0
#define UDE_HID_EN                  0
#define UDE_MIC_EN                  0
#define UDE_ENUM_TYPE               0
#else
#define UDE_ENUM_TYPE               (UDE_STORAGE_EN*0x01 + UDE_SPEAKER_EN*0x02 + UDE_HID_EN*0x04 + UDE_MIC_EN*0x08)
#endif

#if ((!MUSIC_UDISK_EN) && (!MUSIC_SDCARD_EN))
#undef  USB_SD_UPDATE_EN
#define USB_SD_UPDATE_EN               0
#endif

#define USB_SUPPORT_EN                 (MUSIC_UDISK_EN | FUNC_USBDEV_EN)
#define SD_SUPPORT_EN                  MUSIC_SDCARD_EN

#if !SD_SUPPORT_EN
#undef  UDE_STORAGE_EN
#define UDE_STORAGE_EN                 0
#endif

/*****************************************************************************
 * Module    : 提示音配置
 *****************************************************************************/
#if (!WARNING_TONE_EN)
#undef WARNING_POWER_ON
#undef WARNING_POWER_OFF
#undef WARNING_BT_INCALL
#undef BT_HFP_INBAND_RING_EN

#define WARNING_POWER_ON               0
#define WARNING_POWER_OFF              0
#define WARNING_BT_INCALL              0
#define BT_HFP_INBAND_RING_EN          0

#endif

#ifndef UPD_FILENAME
#define UPD_FILENAME                   "fw5000.upd"
#endif

/*****************************************************************************
* Module    : 有冲突或功能上不能同时打开的宏
*****************************************************************************/


/*****************************************************************************
* Module    : 计算FunctionKey
*****************************************************************************/

/*****************************************************************************
* Module    : 传感器选择
*****************************************************************************/
#define SENSOR_STEP_NULL                         0
#define SENSOR_STEP_EN                           0x100
#define SENSOR_STEP_SC7A20                       (SENSOR_STEP_EN | 0x0)
#define SENSOR_STEP_MSA310                       (SENSOR_STEP_EN | 0x1)
#define SENSOR_STEP_INTER                        (SENSOR_STEP_EN | 0x2)

#define SENSOR_HR_NULL                           0
#define SENSOR_HR_EN                             0x200
#define SENSOR_HR_TYHX_HRS3300                   (SENSOR_HR_EN | 0x0)                       //天易合芯HRS3300心率血压传感器
#define SENSOR_HR_TYHX_HX3605                    (SENSOR_HR_EN | 0x1)                       //天易合芯HX3605心率血氧传感器
#define SENSOR_HR_VC30FX                         (SENSOR_HR_EN | 0x2)

#define SENSOR_GEO_NULL                           0
#define SENSOR_GEO_EN                             0x300

/*****************************************************************************
* Module    : 加密芯片
*****************************************************************************/
#define SECURITY_VENDOR_HS                    0     //宏思
#define SECURITY_VENDOR_HED                   1     //华大

/*****************************************************************************
 * Module    : 友杰语音配置起始
 *****************************************************************************/
#define ET_CONFIG                       1
#define ET_WAKEUP_EN                    1
#define ET_ASR_EN                       1
#define ET_VAD_CONTINUE_DELAY_EN        0
#define ET_LOG_OPEN                     1
#define ET_ASR_DUMP_EN                  0
#define THIRD_UI_LIGHT_EN               1
#define ET_WARNING_MP3_PLAY             1
#define ET_WARNING_TONE_EN              1
#define ET_ASR_MAIN_THREAD_EN           0 //识别在主线程中

#define ET_LANG_CHINESE                 1   //1穿戴中文
#define ET_LANG_ENGLISH                 2   ////1穿戴中文;2穿戴英文;

#define ET_UI_PROJ_NAME                 "C0XX"
#define ET_UI_VERSION                   "20240909132800"
#define ET_MODE_LANG_CHOICE             ET_LANG_ENGLISH  //选择当前模型语言

/************ 友杰分贝VAD唤醒参数配置控制 **************************************/
#define ET_ASR_VAD_EN  0   //是否打开分贝Vad
#define ET_VAD_WAKE_WORD_HOLD_EN   1 //是否打开唤醒词激活30秒期间关闭分贝vad
#if 0
//根据麦克风灵敏度和识别需求距离调节
#define VAD_POS_CNT_LIMT  55 //识别为人声的阈值，单位分贝DB,范围0到100
#define VAD_NEG_CNT_LIMT  (VAD_POS_CNT_LIMT - 2) //识别为非人声的阈值，单位分贝DB,范围0到100
#else
//根据麦克风灵敏度和识别需求距离调节
#define VAD_POS_CNT_LIMT  66 //识别为人声的阈值，单位分贝DB,范围0到100
#define VAD_NEG_CNT_LIMT  (VAD_POS_CNT_LIMT - 2) //识别为非人声的阈值，单位分贝DB,范围0到100
#endif // 0
#define VAD_KWS_WORD_LIMT 320 //限制KWS唤醒的最长时间（单位：ms），300次*12ms = 3600ms = 3.6s

/************ 友杰识别参数配置 *************************************************/
#define ET_ASR_INIT_MAX_DELAY           3
#define ET_ASR_CLOSE_MAX_DELAY          2 // 3s delay close
#define ET_ASR_MUSIC_VOL_NDB            4
#define ET_ASR_PHONE_RING_EN            1
#define ET_ONLY_KEY_OPEN_ASR_MODE       0   //只能按键打开识别模式

#define ET_OPEN_ASR_PREFETCH            1

#if ET_MODE_LANG_CHOICE == ET_LANG_ENGLISH
#define ET_USED_USBKEY  ET_LANG_ENGLISH  //类型:1穿戴中文;2穿戴英文;
#elif ET_MODE_LANG_CHOICE == ET_LANG_CHINESE
#define ET_USED_USBKEY  ET_LANG_CHINESE  //类型:1穿戴中文;2穿戴英文;
#else
#define ET_USED_USBKEY  0
#endif // ET_MODE_LANG_CHOICE

/************ 友杰唤醒词功能支持配置 ********************************************/
#define ET_A_WAKE_WORD_EN               1  //开启唤醒词功能
#define ET_WAKE_TIME_DEFAULT            15  //15秒

/************ 友杰日志打印 *****************************************************/
#if ET_LOG_OPEN
#define DBG_SET                         1   // 是否开启ET打印
#define ET_LOG(flags, fmt, arg...) \
    do {                        \
        if (flags)              \
            printf(fmt, ##arg); \
    } while (0)

#define ET_LOGI(fmt, arg...)   ET_LOG(DBG_SET, "[ET I] "fmt, ##arg)
//#define LOGW(fmt, arg...)   ET_LOG(DBG_SET, "[ET W] "fmt, ##arg)
//#define LOGE(fmt, arg...)   ET_LOG(DBG_SET, "[ET E] "fmt, ##arg)
#define ET_MONITOR_LOG                  1
#else
#define LOGI(fmt, arg...)
#define LOGW(fmt, arg...)
#define LOGE(fmt, arg...)
#define ET_MONITOR_LOG                  0
#endif // ET_LOG_OPEN

/************ 友杰加密狗 *****************************************************/
#if (ET_USED_USBKEY == ET_LANG_CHINESE)
#define ET_USED_USBKEY_PSD              0xDD00348F
#elif (ET_USED_USBKEY == ET_LANG_ENGLISH)
#define ET_USED_USBKEY_PSD              0x6564CE1E
#endif //ET_USED_USBKEY
/*****************************************************************************
 * Module    : 友杰语音配置结束
 *****************************************************************************/

/*****************************************************************************
 * Module    : 华镇纯软语音配置
 *****************************************************************************/
#define MONITOR_ASR_WAKEUP_EN           1   // 语音唤醒
#define MONITOR_ASR_WAKEUP_TIME         10  // 单位：秒
#define WS_SOFT_USBKEY_PSD              0x89209F1F

/*****************************************************************************
 * Module    : 华镇空调伴侣语音配置
 *****************************************************************************/
#define IR_AIR_FUNC                     1   //空调伴侣
#define MONITOR_ASR_WAKEUP_EN           1
#define MONITOR_AIR_DEMO                1
#define WS_AIR_SOFT_USBKEY_PSD          0xDDC431AA

/*****************************************************************************
 * Module    : 语音识别其他配置
 *****************************************************************************/
#if (ASR_SELECT == ASR_WS)
#undef ASR_USBKEY_PSD
#define ASR_USBKEY_PSD                  WS_SOFT_USBKEY_PSD
#undef ASR_SIRI_AUTO_CLOSE_COUNTDOWN
#define ASR_SIRI_AUTO_CLOSE_COUNTDOWN   MONITOR_ASR_WAKEUP_TIME
#elif (ASR_SELECT == ASR_YJ)
#undef ASR_USBKEY_PSD
#define ASR_USBKEY_PSD                  ET_USED_USBKEY_PSD
#undef ASR_SIRI_AUTO_CLOSE_COUNTDOWN
#define ASR_SIRI_AUTO_CLOSE_COUNTDOWN   ET_WAKE_TIME_DEFAULT
#elif (ASR_SELECT == ASR_WS_AIR)
#undef ASR_USBKEY_PSD
#define ASR_USBKEY_PSD                  WS_AIR_SOFT_USBKEY_PSD
#undef ASR_SIRI_AUTO_CLOSE_COUNTDOWN
#define ASR_SIRI_AUTO_CLOSE_COUNTDOWN   ET_WAKE_TIME_DEFAULT
#endif


#endif // __CONFIG_EXTRA_H__

