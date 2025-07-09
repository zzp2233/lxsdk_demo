/*
 * @brief:
 * @LastEditors: ljf
 * @LastEditTime: 2025-05-07 09:29:49
 * @FilePath: \AB5691_SDK\app\projects\watch240\config.h
 * @Date: 2025-04-16 10:52:40
 */
/*****************************************************************************
 * Module    : Config
 * File      : config.h
 * Function  : SDK配置文件
 *****************************************************************************/

#ifndef USER_CONFIG_H
#define USER_CONFIG_H
#include "ute_project_config.h"
#include "config_define.h"
#include "ute_flash_map_common.h"

/*****************************************************************************
 * Module    : Function选择相关配置
 *****************************************************************************/
#define MAX_FUNC_SORT_CNT               20   //最大支持左右快捷切换任务的个数

#define FUNC_BT_EN                      1   //是否打开蓝牙功能
#define FUNC_BT_DUT_EN                  0   //是否打开蓝牙的独立DUT测试模式
#define FUNC_MUSIC_EN                   0   //是否打开MUSIC功能
#define FUNC_RECORDER_EN                0   //是否打开录音机功能
#define FUNC_USBDEV_EN                  0   //是否打开USB DEVICE功能
#define FUNC_CAMERA_TRANS_EN            0   //是否打开相机传输功能,需要一张图片的RGB数据缓存
#define FUNC_IDLE_EN                    0   //是否打开IDLE功能

/*****************************************************************************
 * Module    : 系统功能选择配置
 *****************************************************************************/
#define BUCK_MODE_EN                    1                           //是否BUCK MODE
#define SYS_CLK_SEL                     SYS_48M                     //选择系统时钟
#define POWKEY_10S_RESET                xcfg_cb.powkey_10s_reset
#define SOFT_POWER_ON_OFF               1                           //是否使用软开关机功能
#define PWRKEY_2_HW_PWRON               0                           //用PWRKEY模拟硬开关
#define LP_XOSC_CLOCK_EN                0                           //是否使能低功耗晶振用于RTC CLOCK，支持关机时钟功能。(单脚晶振不支持低功耗晶振)
#define GUI_AUTO_POWER_EN               1                           //是否使能刷图动态调节时钟，打开后系统时钟默认设置为SYS_CLK_SEL，刷图时调节为192M

#define CHIP_PACKAGE_SELECT             UTE_CHIP_PACKAGE_SELECT                  //芯片封装选择 5691G/5691C_F

// #define GUI_SELECT                      GUI_TFT_240_JD9853W3//DISPLAY_UTE//GUI_TFT_320_385_GV9B71                 //GUI Display Select
// #define CTP_SELECT                      UTE_DRV_CTP_SELECT                   //CTP Select
// #define GUI_MODE_SELECT                 MODE_4WIRE_8BIT
#define GUI_SELECT                      GUI_TFT_320_ST77916//GUI_TFT_240_296_NV3030B//GUI_TFT_320_385_GV9B71                 //GUI Display Select
#define CTP_SELECT                      UTE_DRV_CTP_SELECT                   //CTP Select
#define GUI_MODE_SELECT                MODE_QSPI

#if (GUI_SELECT == GUI_TFT_SPI)                                     //当使用spi通用接口时,
#define GUI_MODE_SELECT                 MODE_3WIRE_9BIT_2LINE             //屏幕接口选择
#define TFT_SPI_DRIVER                  SPI_GC9307_V1           //驱动选择
#define TFT_SPI_LCD_WIDTH               240
#define TFT_SPI_LCD_HEIGHT              284
#define TFT_SPI_LCD_OFS_X               0
#define TFT_SPI_LCD_OFS_Y               20
#endif

#define CTP_SUPPORT_COVER               1                           //是否支持盖手息屏功能，需要确认屏幕是否支持
#define DEFAULT_TE_MODE                 1                           //默认1 TE模式, 0为2 TE模式, 1为1 TE模式, 2为复杂界面专用模式
#define UART0_PRINTF_SEL                PRINTF_PB3                  //选择UART打印信息输出IO，或关闭打印信息输出
#define SYS_INIT_VOLUME                 xcfg_cb.sys_init_vol        //系统默认音量

#define HEAP_FUNC_SIZE                  4096                        //FUNC HEAP SIZE
#define CUSTOMER_HEAP_SIZE              (16 * 1024)                 //malloc 空间大小

#define TS_MODE_EN                      0                           //内部NTC模块是否开启
#define MODEM_CAT1_EN                   0                           //cat1测试功能
#define CALL_MGR_EN                     0                           //统一管理cat1与蓝牙通话（开了cat1需要打开）

/*****************************************************************************
 * Module    : GUI相关配置
 *****************************************************************************/
#define COMPO_BUF_SIZE                  (2048 + 1024)                  //组件BUF大小(2个BUF)
#define TFT_TE_CYCLE                    16.67                   //TE周期时间 (ms)
#define TFT_TE_CYCLE_DELAY              (TFT_TE_CYCLE / 3)
#define GUI_LINES_CNT                   20                      //单次推屏行数
#define GUI_FONT_W_SPACE                0                       //字的间距
#define GUI_FONT_H_SPACE                0                       //全局字的行间距,0:不设置, 其他:设置文本行最小间距
#define GUI_USE_ARC                     1                       //是否使用圆弧控件

#define FORM_TITLE_HEIGHT               TITLE_BAR_HIGH                      //窗体标题高度
#define FORM_TITLE_LEFT                 (GUI_SCREEN_WIDTH / 9)
#define FORM_TITLE_RIGHT                (GUI_SCREEN_WIDTH -  GUI_SCREEN_WIDTH / 3 - 10)

#define UI_BUF_FONT_SYS                 UI_BUF_0FONT_FONT_BIN           //系统字体
#define UI_BUF_FONT_FORM_TIME           UI_BUF_0FONT_FONT_BIN//UI_BUF_0FONT_FONT_ASC_BIN       //窗体标题栏时间字体

/*****************************************************************************
 * Module    : 场景切换相关配置
 *****************************************************************************/
#define GUI_SIDE_MENU_WIDTH             (GUI_SCREEN_WIDTH / 2)  //边菜单的宽度

/*****************************************************************************
 * Module    : FLASH MAP 说明
 *****************************************************************************
 **0x0
 *4K                           (BOOT1)
 *4K                           (BOOT2)
 **0x2000
 *FLASH_CODE_SIZE              (app.bin)
 *FLASH_UI_SIZE                (ui.bin)
 ***
 *FLASH_UPDATE_PACKAGE_SIZE    (压缩升级包存放区域)
 ***
 *FLASHDB_ADDR                 （FLASHDB_EN == 1 时，16K）
 ***
 *FLASH_AB_PARAM_ADDR          (若USE_APP_TYPE == USE_AB_APP, 则有app协议保存数据)
 ***
 *FLASH_CM_SIZE                (参数区)
 *1.程序使用空间从0x2000地址开始存放，使用大小为FLASH_CODE_SIZE
 *2.UI资源文件可自定义大小，起始地址要在code区域后，不能有地址重叠
 *3.如需压缩升级，升级包存放地址要在UI.bin资源之后
 *4.参数区起始地址为：FLASH_SIZE - FLASH_CM_SIZE
 *****************************************************************************/

/*****************************************************************************
 * Module    : FLASH配置
 *****************************************************************************/
#define FLASH_SIZE                      UTE_FLASH_SIZE            //根据芯片信息配置实际FLASH SIZE
#define FLASH_CODE_SIZE                 UTE_CODE_SIZE             //程序使用空间大小
#define FLASH_UI_BASE                   UTE_UI_ADDRESS            //UI资源起始地址
#define FLASH_UI_SIZE                   UTE_UI_SIZE              //UI资源大小
#define FLASH_CM_SIZE                   UTE_FACTORY_PARAM_SIZE   //CM参数区大小, 参数区至少20k
#define FLASH_ERASE_4K                  1                       //是否支持4K擦除
#define FLASH_DUAL_READ                 0                       //是否支持2线模式
#define FLASH_QUAD_READ                 0                       //是否支持4线模式
#define SPIFLASH_SPEED_UP_EN            1                       //SPI FLASH提速。
#define FLASH_ERASE_32K_64K             0                       //是否支持32k和64k擦除
#define FLASH_EXTERNAL_EN               0                       //是否支持外挂flash, 可存放UI资源
/*****************************************************************************
 * Module    : 蓝牙功能配置
 *****************************************************************************/
#define BT_BQB_RF_EN                    0   //BR/EDR DUT测试模式，为方便测试不自动回连（仅用于BQB RF测试）
#define BT_FCC_TEST_EN                  0   //蓝牙FCC测试使能，默认PB3 波特率1500000通信（仅用于FCC RF测试）
#define BT_LINK_INFO_PAGE1_EN           0   //是否使用PAGE1回连信息（打开后可以最多保存8个回连信息）
#define BT_POWER_UP_RECONNECT_TIMES     0   //上电回连次数
#define BT_TIME_OUT_RECONNECT_TIMES     20  //掉线回连次数
#define BT_SIMPLE_PAIR_EN               1   //是否打开蓝牙简易配对功能（关闭时需要手机端输入PIN码）
#define BT_DISCOVER_CTRL_EN             1   //是否使用按键打开可被发现（需自行添加配对键处理才能被连接配对）
#define BT_DISCOVER_TIMEOUT             100 //按键打开可被发现后，多久后仍无连接自动关闭，0不自动关闭，单位100ms
#define BT_ANTI_LOST_EN                 0   //是否打开蓝牙防丢报警
#define BT_DUT_MODE_EN                  0   //正常连接模式，是否使能DUT测试
#define BT_LOCAL_ADDR                   0   //蓝牙是否使用本地地址，0使用配置工具地址
#define BT_LOW_LATENCY_EN               0   //是否打开蓝牙低延时切换功能

#define BT_2ACL_EN                      0   //是否支持连接两部手机（TWS不支持）
#define BT_2ACL_AUTO_SWITCH             0   //连接两部手机时是否支持点击播放切换到对应的手机
#define BT_A2DP_EN                      1   //是否打开蓝牙音乐服务
#define BT_HFP_EN                       1   //是否打开蓝牙通话服务
#define BT_HSP_EN                       0   //是否打开蓝牙HSP通话服务
#define BT_PBAP_EN                      1   //是否打开蓝牙电话簿服务
#define BT_MAP_EN                       0   //是否打开蓝牙短信服务(用于获取设备时间，支持IOS/Android)
#define BT_SPP_EN                       1   //是否打开蓝牙串口服务
#define BT_ID3_TAG_EN                   1   //是否打开蓝牙ID3功能
#define BT_HID_EN                       1   //是否打开蓝牙HID服务
#define BT_HID_TYPE                     0   //选择HID服务类型: 0=自拍器(VOL+, 部分Android不能拍照), 1=自拍器(VOL+和ENTER, 影响IOS键盘使用), 2=游戏手柄
#define BT_HID_MANU_EN                  0   //蓝牙HID是否需要手动连接/断开
#define BT_HID_DISCON_DEFAULT_EN        0   //蓝牙HID服务默认不连接，需要手动进行连接。
#define BT_HID_VOL_CTRL_EN              0   //是否支持HID调手机音量功能（需同时打开BT_HID_EN和BT_A2DP_VOL_CTRL_EN）
#define BT_HFP_CALL_PRIVATE_SWITCH_EN   1   //是否打开按键切换私密接听与蓝牙接听功能
#define BT_HFP_CALL_PRIVATE_FORCE_EN    0   //是否强制使用私密接听（仅在手机接听，不通过蓝牙外放）
#define BT_HFP_RECORD_DEVICE_VOL_EN     0   //是否支持分别记录不同连接设备的通话音量
#define BT_HFP_RING_NUMBER_EN           1   //是否支持来电报号
#define BT_HFP_INBAND_RING_EN           1   //是否支持手机来电铃声（部分android不支持，默认用本地RING提示音）
#define BT_HFP_BAT_REPORT_EN            1   //是否支持电量显示
#define BT_HFP_MSBC_EN                  1   //是否打开宽带语音功能
#define BT_HFP_3WAY_CTRL_EN             1   //是否使能三方通话管理
#define BT_HFP_SWITCH_EN                1   //是否使能通话切换功能，包括主动切换和哪边接听哪边出声
#define BT_VOIP_REJECT_EN               1   //网络电话不建立SCO功能使能,使用时需A2DP断开 (网络电话：微信通话，QQ通话等)
#define BT_A2DP_PROFILE_DEFAULT_EN      1   //蓝牙音频服务是否默认打开
#define BT_A2DP_VOL_CTRL_EN             1   //是否支持音量与手机同步，（默认使用AVRCP协议，打开BT_HID_VOL_CTRL_EN后使用HID协议）
#define BT_A2DP_RECORD_DEVICE_VOL_EN    1   //是否支持分别记录不同连接设备的音量，使用设备时恢复当前设备音量
#define BT_A2DP_VOL_REST_EN             0   //是否支持连接不支持同步音量手机时复位音量
#define BT_A2DP_AVRCP_PLAY_STATUS_EN    0   //是否支持手机播放状态同步，可加快播放暂停响应速度
#define BT_A2DP_RECON_EN                0   //是否支持A2DP控制键（播放/暂停、上下曲键）回连
#define BT_A2DP_SUPTO_RESTORE_PLAY_EN   1   //是否支持蓝牙超距回连恢复播放
#define BT_A2DP_EXCEPT_RESTORE_PLAY_EN  0   //是否支持异常复位后回连恢复播放
#define BT_AVDTP_DYN_LATENCY_EN         1   //是否支持根据信号环境动态调整延迟
#define BT_SCO_DBG_EN                   0   //是否打开无线调试通话参数功能
#define BT_CONNECT_REQ_FROM_WATCH_EN    1   //是否使能一键双连BT连接请求由手表发起
#define BT_RF_EXT_CTL_EN                0   //是否打开rf动态功耗调节
#define BT_HID_ONLY_FOR_IOS_EN          0   //是否使能安卓手机不建立HID


#define BT_SINGLE_SLEEP_LPW_EN          0   //是否打开单模进休眠关bt省电

/*****************************************************************************
 * Module    : BLE功能配置
 *****************************************************************************/
#define LE_EN                           1   //是否打开BLE功能
#define LE_PAIR_EN                      1   //是否使能BLE的加密配对
#define LE_SM_SC_EN                     1   //是否使能BLE的加密连接，需同时打开LE_PAIR_EN。一键双联需要打开此配置。
#define LE_ADV_POWERON_EN               1   //是否上电默认打开BLE广播
#define LE_BQB_RF_EN                    0   //BLE DUT测试模式，使用串口通信（仅用于BQB RFPHY测试）
#define LE_ALLOW_WKUP_EN                1   //休眠中ble断开/连接/传输是否需要退出休眠

//gatt 配置
#define LE_ATT_NUM                      45  //最大支持多少条gatt属性, att_handle 1 ~ LE_ATT_NUM

//APP 功能相关(APP同时只能打开1个)
#define LE_AB_LINK_APP_EN               0           //是否打开AB-LINK APP控制功能
#define USE_APP_TYPE                    USE_UTE_APP//USE_NULL_APP  //选择手表应用app类型

//ANCS
#define LE_ANCS_CLIENT_EN               1   //是否打开ANCS Clients
#define LE_ANCS_MANUAL_EN               1   //是否需要手动打开ancs, 需要调用发起ancs连接的相关接口
//AMS
#define LE_AMS_CLIENT_EN                1   //是否打开AMS Clients

#define LE_ADV0_EN                      0   //是否打开无连接广播功能
#define LE_WIN10_POPUP                  0   //是否打开win10 swift pair快速配对

//FOTA功能配置
#define LE_AB_FOT_EN                    1   //是否打开BLE FOTA服务,需同时打开LE_AB_LINK_APP_EN
#define AB_FOT_TYPE_PACK                1   //FOTA压缩升级（代码做压缩处理，升级完成需做解压才可正常运行）
#define SW_VERSION                      "V0.0.1"   //只能使用数字0-9,ota需要转码
#define HW_VERSION                      "V0.0.1"   //只能使用数字0-9,ota需要转码
#define FLASH_PKG_START                 UTE_OTA_TMP_ADDRESS   //升级压缩包存放起始地址
#define FLASH_PKG_SIZE                  UTE_OTA_TMP_SIZE   //升级压缩包大小
#define FOTA_UI_EN                      1          //是否支持UI升级，需要用一个批处理打包UI+FOT

//HID功能配置
#define LE_HID_EN                       0   //BLE HID总开关
#define LE_HID_CONSUMER                 1   //BLE HID Consumer功能
#define LE_HID_DIGITIZER                1   //BLE HID Digitizer功能
#define LE_HID_MOUSE                    1   //BLE HID Mouse功能
#define LE_HID_KEYBOARD                 0   //BLE HID Keybarod功能

#define LE_HID_TEST                     0   //BLE HID测试

/*****************************************************************************
 * Module    : 通话功能配置
 *****************************************************************************/
//通话参数
#define BT_SCO_DUMP_EN                  0                           //是否打开上行降噪算法数据dump功能（双MIC优先用），dump:算法前主麦 + 算法前副麦 + 算法后
#define BT_SCO_FAR_DUMP_EN              0                           //是否打开通话下行数据dump功能，dump:算法前 + 算法后
#define BT_SCO_EQ_DUMP_EN               0                           //是否打开上行EQ的数据dump功能（单MIC优先用），dump:算法前主麦 + 算法后 + EQ后

#define BT_SCO_MAV_EN                   0                           //是否打开蓝牙通话变声功能

#define BT_PLC_EN                       1
#define BT_ANL_GAIN                     xcfg_cb.bt_anl_gain         //MIC模拟增益(0~23)
#define BT_DIG_GAIN                     xcfg_cb.bt_dig_gain         //MIC数字增益(0-63), step: 0.5db
#define BT_CALL_MAX_GAIN                xcfg_cb.bt_call_max_gain    //配置通话时DAC最大模拟增益

#define BT_AEC_EN                       1
#define BT_ECHO_LEVEL                   xcfg_cb.bt_echo_level       //回声消除级别（级别越高，回声衰减越明显，但通话效果越差）(0~15)
#define BT_FAR_OFFSET                   xcfg_cb.bt_far_offset       //远端补偿值(0~255)

#define BT_ALC_EN                       1                           //是否使能ALC
#define BT_ALC_FADE_IN_DELAY            26                          //近端淡入延时(n*7.5ms)
#define BT_ALC_FADE_IN_STEP             1                           //近端淡入速度(64ms)
#define BT_ALC_FADE_OUT_DELAY           2                           //远端淡入延时(n*7.5ms)
#define BT_ALC_FADE_OUT_STEP            16                          //远端淡入速度(4ms)
#define BT_ALC_VOICE_THR                0x30000

//通话近端降噪算法(耳机MIC采集数据降噪)
#define BT_SCO_AINS3_EN                 1                           //是否打开MIC的AINS3降噪(AINS3与DNN二选一, AINS3占用资源少)
#define BT_SCO_AINS3_LEVEL              xcfg_cb.bt_sco_nr_level     //0-30级

#define BT_SCO_DNN_EN                   0                           //是否打开自研单麦DNN降噪算法
#define BT_SCO_DNN_MODEL_SET            0                           //自研单麦DNN模型选择，0：标准模型，1：标准模型+6K
#define BT_SCO_DNN_MODE                 1                           //DNN模式，模式0，模式1（建议）
#define BT_SCO_DNN_LEVEL                6                           //降噪量：0-30级
#define BT_SCO_DNN_WIND_EN              0                           //默认关闭


//通话远端降噪算法(接收远端手机的通话数据降噪)
#define BT_SCO_FAR_NR_EN                0                           //是否打开远端降噪算法(Code: 5.5KB, Ram: 2.1KB)
#define BT_SCO_FAR_NR_LEVEL             3                           //强度: 0~5
#define BT_SCO_FAR_NOISE_THR            1                           //范围: 0~20

//（注：量产版本一定要关掉该功能，仅用于调试，会影响稳定性，开启该功能要预留 1.9K Ram）
#define BT_SCO_APP_DBG_EN               0                           //是否打开通话算法APP调试功能，配合通话调试APP使用，配置工具需要打开蓝牙串口服务和EQ蓝牙串口调试


/*****************************************************************************
 * Module    : SD/UDISK音乐功能配置
 *****************************************************************************/
#define MUSIC_UDISK_EN                  0   //是否支持播放UDISK
#define MUSIC_SDCARD_EN                 1   //是否支持播放SDCARD
#define USB_SD_UPDATE_EN                0   //是否支持UDISK/SD的离线升级

#define MUSIC_WAV_SUPPORT               0   //是否支持WAV格式解码
#define MUSIC_WMA_SUPPORT               0   //是否支持WMA格式解码
#define MUSIC_APE_SUPPORT               0   //是否支持APE格式解码
#define MUSIC_FLAC_SUPPORT              0   //是否支持FLAC格式解码
#define MUSIC_SBC_SUPPORT               0   //是否支持SBC格式解码(SD/UDISK的SBC歌曲, 此宏不影响蓝牙音乐)

#define MUSIC_FOLDER_SELECT_EN          1   //文件夹选择功能
#define MUSIC_AUTO_SWITCH_DEVICE        1   //双设备循环播放
#define MUSIC_BREAKPOINT_EN             1   //音乐断点记忆播放
#define MUSIC_QSKIP_EN                  1   //快进快退功能
#define MUSIC_PLAYMODE_NUM              4   //音乐播放模式总数
#define MUSIC_MODE_RETURN               1   //退出音乐模式之后是否返回原来的模式
#define MUSIC_PLAYDEV_BOX_EN            0   //是否显示“USB”, "SD"界面
#define MUSIC_ID3_TAG_EN                0   //是否获取MP3 ID3信息
#define MUSIC_REC_FILE_FILTER           0   //是否区分录音文件与非录音文件分别播放
#define MUSIC_LRC_EN                    0   //是否支持歌词显示
#define MUSIC_NAVIGATION_EN             0   //音乐文件导航功能(LCD点阵屏功能)
#define MUSIC_ENCRYPT_EN                0   //是否支持加密MP3文件播放(使用MusicEncrypt.exe工具进行MP3加密)
#define MUSIC_MP3_LOOPBACK_EN           0   //是否开启MP3音乐单曲无缝循环播放

#define MUSIC_ENCRYPT_KEY               12345   //MusicEncrypt.exe工具上填的加密KEY

/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#define FUNC_REC_EN                     1   //录音功能总开关
#define REC_FAST_PLAY                   0   //播卡播U下快速播放最新的录音文件(双击REC)

#define REC_MP3_SUPPORT                 1   //是否支持MP3音频格式录音
#define REC_WAV_SUPPORT                 0   //是否支持WAV音频格式录音
#define REC_ADPCM_SUPPORT               0   //是否支持ADPCM音频格式录音
#define REC_SBC_SUPPORT                 0   //是否支持SBC音频格式录音


/*****************************************************************************
* Module    : DAC配置控制
******************************************************************************/
#define DAC_CH_SEL                      xcfg_cb.dac_sel             //DAC_MONO ~ DAC_VCMBUF_DUAL
#define DAC_FAST_SETUP_EN               1                           //DAC快速上电，有噪声需要外部功放MUTE
#define DAC_MAX_GAIN                    xcfg_cb.dac_max_gain        //配置DAC最大模拟增益，默认设置为dac_vol_table[VOL_MAX]
#define DAC_OUT_SPR                     DAC_OUT_48K                 //dac out sample rate
#define DAC_LDOH_SEL                    xcfg_cb.dacaud_ldo_sel
#define DACVDD_BYPASS_EN                xcfg_cb.dacaud_bypass_en    //DACVDD Bypass
#define DAC_PULL_DOWN_DELAY             80                          //控制DAC隔直电容的放电时间, 无电容时可设为0，减少开机时间。
#define DAC_DNR_EN                      0                           //是否使能动态降噪
#define DAC_DRC_EN                      1                           //是否使能DRC功能
#define DAC_AUTO_ONOFF_EN               1                           //是否DAC自动关闭模拟电源（不播放时自动关掉省电）

/*****************************************************************************
* Module    : 音频压缩算法配置
******************************************************************************/
#define OPUS_ENC_EN                     0   //是否打开opus压缩算法

/*****************************************************************************
 * Module    : EQ相关配置
 *****************************************************************************/
#define EQ_MODE_EN                      0           //是否调节EQ MODE (POP, Rock, Jazz, Classic, Country)
#define MIC_EQ_EN                       UTE_CONNECT_PC_AUDIO_TEST           //是否调节MIC_EQ (通话模式)
#define EQ_DBG_IN_UART                  UTE_CONNECT_PC_AUDIO_TEST           //是否使能UART在线调节EQ
#define EQ_DBG_IN_SPP                   UTE_CONNECT_PC_AUDIO_TEST           //是否使能SPP在线调节EQ


/*****************************************************************************
 * Module    : User按键配置 (可以同时选择多组按键)
 *****************************************************************************/
#define USER_PWRKEY                     UTE_DRV_PWRKEY_SUPPORT           //PWRKEY的使用，0为不使用
#define USER_ADKEY                      UTE_DRV_ADCKEY_SUPPORT           //ADKEY的使用， 0为不使用
#define USER_IOKEY                      UTE_DRV_IOKEY_SUPPORT            //IOKEY的使用， 0为不使用

#define USER_KEY_QDEC_EN                0           //旋钮, 硬件正交解码, A,B输出分别接一个IO
#define USER_QDEC_MAPPING               QDEC_MAP_G1 //选择硬件正交解码的mapping, 每组map的IO固定，详见define处说明

#define USER_ADKEY_QDEC_EN              0           //旋钮, A,B串不同电阻接到同一个IO口上，软件ADC采集并解码
#define USER_ADKEY_QDEC_NO_STD          0           //是否使用非标准电平判断(适用编码器漏电平时)
#define USER_QDEC_ADCH                  ADCCH_PA0   //选择旋钮的ADC通道

#define USER_IO_QEDC_EN                 1           //旋钮, 使用两个普通IO方式
#define QEDC_IO_A                       IO_PB0
#define QEDC_IO_B                       IO_PB1

#define USER_MULTI_PRESS_EN             1           //按键多击检测使能
#define USER_MULTI_KEY_TIME             4           //按键多击响应时间（单位100ms）

#define USER_PWRON_KEY_SEL              0           //定义为开关机的PWRKEY按键编号, 范围: 0 ~ 2
#define PWRON_PRESS_TIME                1500        //长按PWRKEY多长时间开机？
#define PWROFF_PRESS_TIME               18          //长按PWRKEY多长时间关机 3: 1.5秒, 6: 2秒, 9: 2.5秒, 12: 3秒, 15: 3.5秒, 18: 4秒, 24: 5秒

#define ADKEY_CH                        ADCCH_PE7   //ADKEY的ADC通路选择
#define IS_PWRKEY_PRESS()               (0 == (RTCCON & BIT(19)))

/*****************************************************************************
 * Module    : 电量检测及低电
 *****************************************************************************/
#define VBAT_DETECT_EN                  1           //电池电量检测功能
#define VUSB_DETECT_EN                  0           //充电电压检测功能
#define VBAT2_ADCCH                     ADCCH_VBAT  //ADCCH_VBAT为内部1/2电压通路，带升压应用需要外部ADC通路检测1/2电池电压
#define VBAT_FILTER_USE_PEAK            0           //电池检测滤波选则://0 取平均值.//1 取峰值(适用于播放音乐时,电池波动比较大的音箱方案).
#define LPWR_WARNING_VBAT               UTE_DRV_BATTERY_010        //低电提醒电压   0：表示关闭此功能
#define LPWR_OFF_VBAT                   UTE_DRV_BATTERY_POWER_ON_VOLTAGE        //低电关机电压   0：表示关闭此功能
#define LOWPWR_REDUCE_VOL_EN            1           //低电时是否降低音量
#define LPWR_WARING_TIMES               0xff        //报低电次数
#define LPWR_WARNING_PERIOD             30          //低电播报周期(单位：秒)

/*****************************************************************************
 * Module    : 充电功能选择
 *****************************************************************************/
#define CHARGE_EN                       1                               //是否打开充电功能
#define CHARGE_TRICK_EN                 xcfg_cb.charge_trick_en         //是否打开涓流充电功能
#define CHARGE_DC_RESET                 xcfg_cb.charge_dc_reset         //是否打开DC插入复位功能
#define CHARGE_DC_NOT_PWRON             xcfg_cb.charge_dc_not_pwron     //DC插入，是否软开机。 1: DC IN时不能开机
#define CHARGE_DC_IN()                  charge_dc_detect()
#define CHARGE_INBOX()                  ((RTCCON >> 22) & 0x01)

#define CHARGE_STOP_CURR                xcfg_cb.charge_stop_curr        //电流范围0~37.5mA, 配置值范围: 0~15, 步进2.5mA
#define CHARGE_CONSTANT_CURR            xcfg_cb.charge_constant_curr    //电流范围0~320mA, 配置值范围: 0~63, 步进5mA
#define CHARGE_TRICKLE_CURR             xcfg_cb.charge_trickle_curr
#define CHARGE_STOP_VOLT                UTE_DRV_BATTERY_CHARGE_STOP_VOLT  //充电截止电压：0:4.2v；1:4.35v; 2:4.4v; 3:4.45v
#define CHARGE_TRICK_STOP_VOLT          1                               //涓流截止电压：0:2.9v; 1:3v
#define CHARGE_VOLT_FOLLOW              0                               //是否打开跟随快充：0:NONE; 1:187.5mV; 2:62.5mV; 3:375mV

#define CHARGE_VOL_DYNAMIC_DET          1                               //是否打开充电时候，动态检测电池电压功能；打开后，充5s停10ms
//充电辅助设置项
#define CHARGE_USER_NTC_EN              UTE_DRV_BATTERY_CE_AUTH_SUPPORT //充电是否使用NTC参数
#define CHARGE_VBAT_REFILL              (UTE_DRV_BATTERY_MAX_VOLTAGE - 100) //充满后停止充电，电池掉到指定电压后续充

#define CHARGE_NTC_ADC_MAX_TEMP         UTE_DRV_BATTERY_CE_AUTH_NOT_ALLOW_TEMPERATURE  //设置最小温度 摄氏度      53
#define CHARGE_NTC_ADC_MAX_RE_TEMP      UTE_DRV_BATTERY_CE_AUTH_ALLOW_TEMPERATURE  //设置恢复温度 摄氏度     48
#define CHARGE_NTC_ADC_MIN_TEMP         UTE_DRV_BATTERY_CE_AUTH_NOT_ALLOW_LOW_TEMPERATURE  //设置最高温度 摄氏度0
#define CHARGE_NTC_ADC_MIN_RE_TEMP      UTE_DRV_BATTERY_CE_AUTH_ALLOW_LOW_TEMPERATURE  //设置恢复温度 摄氏度5
/*****************************************************************************
 * Module    : 硬件I2C配置
 *****************************************************************************/
#define I2C_HW_EN                       0           //是否使能硬件I2C功能

/*****************************************************************************
 * Module    : 软件I2C配置
 *****************************************************************************/
#define I2C_SW_EN                       0           //是否使能软件I2C功能

#define I2C_SCL_IN()                    {GPIOEDIR |= BIT(5); GPIOEPU  |= BIT(5);}
#define I2C_SCL_OUT()                   {GPIOEDE |= BIT(5); GPIOEDIR &= ~BIT(5);}
#define I2C_SCL_H()                     {GPIOESET = BIT(5);}
#define I2C_SCL_L()                     {GPIOECLR = BIT(5);}

#define I2C_SDA_IN()                    {GPIOEDIR |= BIT(7); GPIOEPU  |= BIT(7);}
#define I2C_SDA_OUT()                   {GPIOEDE |= BIT(7); GPIOEDIR &= ~BIT(7);}
#define I2C_SDA_H()                     {GPIOESET = BIT(7);}
#define I2C_SDA_L()                     {GPIOECLR = BIT(7);}
#define I2C_SDA_IS_H()                  (GPIOE & BIT(7))

#define I2C_SDA_SCL_OUT()               {I2C_SDA_OUT(); I2C_SCL_OUT();}
#define I2C_SDA_SCL_H()                 {I2C_SDA_H(); I2C_SCL_H();}

/*****************************************************************************
 * Module    : 传感器配置
 *****************************************************************************/
#define SENSOR_STEP_SEL                 SENSOR_STEP_NULL
#define SENSOR_HR_SEL                   SENSOR_HR_NULL//SENSOR_HR_VC30FX
#define SENSOR_GEO_SEL                  SENSOR_GEO_NULL

/*****************************************************************************
 * Module    : Loudspeaker mute检测配置
 *****************************************************************************/
#define LOUDSPEAKER_MUTE_EN             1           //是否使能功放MUTE
#define LOUDSPEAKER_MUTE_INIT()         loudspeaker_mute_init()
#define LOUDSPEAKER_MUTE_DIS()          loudspeaker_disable()
#define LOUDSPEAKER_MUTE()              loudspeaker_mute()
#define LOUDSPEAKER_UNMUTE()            loudspeaker_unmute()
#define LOUDSPEAKER_MUTE_PORT           IO_PE0
#define LOUDSPEAKER_HIGH_MUTE           0           //高电平为MUTE状态
#define LOUDSPEAKER_UNMUTE_DELAY        6           //UNMUTE延时配置，单位为5ms

#define AMP_CTRL_AB_D_EN                0           //功放AB/D类控制
#define AMP_CTRL_AB_D_PORT              IO_NONE     //控制IO
#define AMP_CTRL_AB_D_TYPE              0           //0:独立IO电平控制, 1:mute脉冲控制
#define AMPLIFIER_SEL_INIT()            amp_sel_cfg_init(AMP_CTRL_AB_D_PORT)
#define AMPLIFIER_SEL_D()               amp_sel_cfg_d()
#define AMPLIFIER_SEL_AB()              amp_sel_cfg_ab()

/*****************************************************************************
 * Module    : SD0配置
 *****************************************************************************/
#define SD0_MAPPING                     SD0MAP_G4   //选择SD0 mapping
#define SD_SOFT_DETECT_EN               1           //是否使用软件检测 (SD发命令检测)
#define SD0_DETECT_IO                   IO_NONE

#define SD_IS_SOFT_DETECT()             1             //配置工具中选则63是软件检测.
#define SD_DETECT_INIT()                sdcard_detect_init()
#define SD_IS_ONLINE()                  sdcard_is_online()
#define IS_DET_SD_BUSY()                is_det_sdcard_busy()

#define SD0_LDO_EN_IO                   IO_PG7
#define SD0_LDO_EN_EN()                 {GPIOGFEN &= ~BIT(7); GPIOGDE |= BIT(7); GPIOGDIR &= ~BIT(7); GPIOGCLR = BIT(7);}
#define SD0_LDO_EN_DIS()                {GPIOGFEN &= ~BIT(7); GPIOGDE |= BIT(7); GPIOGDIR &= ~BIT(7); GPIOGSET = BIT(7);}
/*****************************************************************************
 * Module    : usb device 功能选择
 *****************************************************************************/
#define UDE_STORAGE_EN                 1
#define UDE_SPEAKER_EN                 1
#define UDE_HID_EN                     1
#define UDE_MIC_EN                     1

/*****************************************************************************
 * Module    : 提示音 功能选择
 *****************************************************************************/
#define WARNING_TONE_EN                 1            //是否打开提示音功能, 总开关
#define WARING_MAXVOL_MP3               0            //最大音量提示音WAV或MP3选择， 播放WAV可以与MUSIC叠加播放。
#define WARNING_WAVRES_PLAY             0            //是否支持WAV提示音播放
#define WARNING_VOLUME                  xcfg_cb.warning_volume   //播放提示音的音量级数
#define LANG_SELECT                     LANG_EN      //提示音语言选择

#define WARNING_POWER_ON                0
#define WARNING_POWER_OFF               0
#define WARNING_BT_INCALL               1            //是否打开蓝牙来电提示音

/*****************************************************************************
 * Module    :线程大小配置
 *****************************************************************************/
#define MEM_HEAP_SIZE                   16*1024 + (2048+2048 + 512) + UTE_TASK_APPLICATION_STACK_SIZE          //用于分配全部线程的总栈内存大小, 如果设置为 0, 采用中科平台库内置默认大小 16*1024
#define OS_THREAD_MAIN_STACK            2048 + 1536 + (2048 + 512)        //分配main线程栈内存大小, 如果设置为 0, 采用中科平台库内置默认大小 2048 + 1536
#define OS_THREAD_MUSIC_STACK           896 + (512) * OPUS_ENC_EN

#define OS_THREAD_MAIN_PRIOPITY         0 //main线程优先级 大于 25 小于 31

/*****************************************************************************
 * Module    : 录音功能配置
 *****************************************************************************/
#define MIC_TEST_EN                     1               //麦克风录音测试功能
#define MIC_DATA_TO_FLASH               1
#if MIC_DATA_TO_FLASH
#define MIC_TEST_BUF_SIZE               (15 * 1024)     //18k可录音530ms左右（复用aram）
#else
#define MIC_TEST_BUF_SIZE               (15 * 1024)     //18k可录音530ms左右（复用aram）
#endif

/*****************************************************************************
 * Module    : 语音方案
 *****************************************************************************/
#define ASR_NULL                        0
#define ASR_WS                          1 //华镇
#define ASR_YJ                          2 //友杰
#define ASR_WS_AIR                      3 //华镇空调伴侣

/*****************************************************************************
 * Module    : 语音方案选择
 *****************************************************************************/
#define ASR_SELECT                      ASR_YJ
#define ASR_FULL_SCENE                  1               //全场景模式
#define ASR_API_CHECK_TIME              0               //API执行时间检测
#define ASR_SAMPLE                      240             //MIC采样率
#define ASR_GAIN                        (12 << 6)       //MIC增益
#define ASR_DEAL_TYPE                   1               //事件处理方式 1:轮询; 0:消息
#define ASR_VOICE_BALL_ANIM             1               //悬浮球动画
#define ASR_USBKEY_PSD                  0               //加密狗
#define ASR_AND_SIRI_PARALLEL_EN        1               //当siri开启时，mic数据同时送入siri和asr引擎
#define ASR_SIRI_AUTO_CLOSE             1               //自动关闭siri
#define ASR_SIRI_AUTO_CLOSE_COUNTDOWN   15              //多少秒后自动关闭siri
#define ASR_SIRI_SCO_DELAY_EN           0               //SIRI 延时输出
#define ASR_MIC_DATA_DUMP_EN            0               //语音识别，输出mic原始音频到bluetrum_voice_record
#if ASR_SELECT
#undef SYS_CLK_SEL
#undef GUI_AUTO_POWER_EN
#define SYS_CLK_SEL                 SYS_192M        //时钟设置为192M
#define GUI_AUTO_POWER_EN           0               //关闭动态调节时钟
#endif
#if (ASR_SELECT == ASR_WS)
#undef ASR_SAMPLE
#undef ASR_GAIN
#define ASR_SAMPLE                  120 * 2
#define ASR_GAIN                    (12 << 6)
#undef FLASH_CODE_SIZE
#undef FLASH_UI_BASE
#undef FLASH_UI_SIZE
#define ASR_PREFETCH_EN             0                       //是否打开预取
#if !ASR_PREFETCH_EN
#define FLASH_CODE_SIZE             1004k                   //程序使用空间大小
#define FLASH_UI_BASE               0xFB000                 //UI资源起始地址
#define FLASH_UI_SIZE               0x300000                //UI资源大小
#else
#define FLASH_CODE_SIZE             784k                   //程序使用空间大小
#define FLASH_UI_BASE               0xC4000                 //UI资源起始地址
#define FLASH_UI_SIZE               0x337000                //UI资源大小
#endif
#endif
#if (ASR_SELECT == ASR_YJ)
#undef ASR_SAMPLE
#undef ASR_GAIN
#define ASR_SAMPLE                  160 * 2
#define ASR_GAIN                    (6 << 6)
// #undef FLASH_CODE_SIZE
// #undef FLASH_UI_BASE
// #undef FLASH_UI_SIZE
// #define FLASH_CODE_SIZE             784k                    //程序使用空间大小
// #define FLASH_UI_BASE               0xC4000                 //UI资源起始地址
// #define FLASH_UI_SIZE               0x337000                //UI资源大小
#endif
#if (ASR_SELECT == ASR_WS_AIR)
#undef ASR_SAMPLE
#undef ASR_GAIN
#define ASR_SAMPLE                  120 * 2
#define ASR_GAIN                    (8 << 6)
#undef FLASH_CODE_SIZE
#undef FLASH_UI_BASE
#undef FLASH_UI_SIZE
#undef FLASH_SIZE
#define FLASH_CODE_SIZE             2024K                   //程序使用空间大小
#define FLASH_UI_BASE               0x1FA000                 //UI资源起始地址
#define FLASH_UI_SIZE               0x348000                //UI资源大小
#define FLASH_SIZE                  FSIZE_8M
#endif

/*****************************************************************************
 * Module    : 分配asr线程栈内存大小
 *****************************************************************************/
#define OS_THREAD_ASR_PRIORITY          28         //优先级
#define OS_THREAD_ASR_STACK             2048     //堆栈大小
#define OS_THREAD_ASR_TICK              -1


/*****************************************************************************
 * Module    : 电子烟功能配置
 *****************************************************************************/
//开发板选型
#define DEVELOPMENT_BOARD_WATCH        0//手表
#define DEVELOPMENT_BOARD_ECIG         1//电子烟
#define DEVELOPMENT_BOARD_USER         2//用户板子
#define DEVELOPMENT_BOARD_TYPE          DEVELOPMENT_BOARD_USER
//
#define ECIG_POWER_CONTROL              1               //是否开启恒功率控制
#define ECIG_ADC2_EN                    1               //是否有ADC2通路
#define ECIG_DET_EN                     1               //是否有检测插拔电路
#define ECIG_POLLING_CONTROL            1               //0:轮休判断，1：计数判断
#define ECIG_TIME_BIG_CYCLES            10000
#if ECIG_POLLING_CONTROL
#define ECIG_TIMER_US                   100              //50us
#else
#define ECIG_TIMER_US                   100             //100us
#endif


/*****************************************************************************
 * Module    : 充电IC 功能选择
 *****************************************************************************/
#define CHARGE_EX_IC_SELECT             CHARGE_IC_LP7812C   //充电模块IC选择
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
#define CHARGE_SDA_GPIO                 IO_PB7              //充电IC单线数据脚（DCIN-DET）
#define CHARGE_CTRL_GPIO                IO_PF0              //充电IC模式控制脚（SY-EN）
#define CHARGE_ENABLE_GPIO              IO_PB5              //充电启停控制脚（VOUT-EN）
#else
#define CHARGE_SDA_GPIO                 IO_PA5              //充电IC单线数据脚
#define CHARGE_CTRL_GPIO                IO_PH4              //充电IC模式控制脚
#define SDA_STATUS()                    (GPIOA & BIT(5))
#define CHARGE_ENABLE_GPIO              IO_NONE             //充电启停控制脚
#endif

/*****************************************************************************
 * Module    :  智能充电仓UART通讯 功能选择
 *****************************************************************************/
#define SMART_VHOUSE_HOST_EN            0               //是否打开智能充电仓串口主机功能
#define SMART_VHOUSE_HOST_UART_SEL      UART1_GROUP
#define SMART_VHOUSE_HOST_HS_EN         0               //是否打开智能充电仓串口高速通讯模式, 智能仓方案使用, 与测试盒快测快配独立使用
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
#define SMART_VHOUSE_HOST_RX            IO_NONE
#define SMART_VHOUSE_HOST_TX            IO_PB1          //串口通讯tx、rx复用引脚
#else
#define SMART_VHOUSE_HOST_RX            IO_PB0          //串口通讯rx脚配置, 不同串口需要在bsp_uart.h打开不同串口模块, 默认使用uart1;
#define SMART_VHOUSE_HOST_TX            IO_PB1          //串口通讯tx脚配置
#endif

/*****************************************************************************
 * Module    : 霍尔开关 功能选择
 *****************************************************************************/
#define HALL_EN                         0               //霍尔检测总开关
#define HALL_GPIO                       IO_PE0          //霍尔检测IO口选择
#define HALL_VDD_SEL                    0               //霍尔供电方式, 0:vbat; 1:vddio;

/*****************************************************************************
 * Module    :蜂鸣器 模拟pwm 功能选择
 *****************************************************************************/
#define BUZZER_EN                       0              //霍尔检测总开关

#define BUZZER_GPIO                IO_PF3
#define BUZZER_GPIO_INIT()        {GPIOFFEN &= ~BIT(3); GPIOFDE |= BIT(3); GPIOFDIR &= ~BIT(3);}
#define BUZZER_GPIO_DEINIT()      {GPIOFFEN &= ~BIT(3); GPIOFDE &= ~BIT(3); GPIOFDIR |= BIT(3);}
#define BUZZER_GPIO_EN()          {GPIOFSET = BIT(3);}
#define BUZZER_GPIO_DIS()         {GPIOFCLR = BIT(3);}




/*****************************************************************************
 * Module    : 充电仓 功能选择
 *****************************************************************************/
#define BOX_LPWR_PERCENT                5                       //仓低电模式电量(不给耳机充电)
#define BOX_LPWR_BRIGHTNESS             3                       //仓低电模式亮度(%)

#define BOX_OPEN_WAKEUP_EARPHONE        0                       //仓是否打开唤醒耳机操作
#define BOX_WKUP_OV_DURATION            600                     //掉0V持续时间(ms)
#define BOX_CLOSE_WKUP_5V_EN            0                       //仓关盖拉唤醒, 是否需要拉5V

#define BOX_GUI_ROTATE_DISP             1                       //是否打开旋转显示
#define BOX_RTC_EN                      1                       //是否打开时间显示; (无时间显示时深睡无需时间校准)
#define LPWR_IDLE_EN                    0                       //是否低电不关机, 进idle模式
#define DEEPSLEEP_REPLACE_PWROFF        1                       //关机功能是否使用深睡替代

#define TFT_SLEEP_CMD_EN                0                       //休眠前是否发送tft休眠指令（tft、tp共用vddio供电时使用）

#define BOX_BLE_MASTER_SLAVE_EN         0                       //是否有ble主从
#define BGLIGHT_LEVEL_CNT               3                       //背光亮度等级数（不包括0）
#define BGLIGHT_LV_DEFAULT              50                      //默认背光亮度（0-100）

#include "config_extra.h"

#endif // USER_CONFIG_H
