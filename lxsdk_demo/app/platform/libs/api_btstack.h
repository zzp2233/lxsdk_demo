/*
 *  api_btstack.h
 *
 *  Created by zoro on 2017-8-24.
 */
#ifndef _API_BTSTACK_H
#define _API_BTSTACK_H

//工作模式
#define MODE_NORMAL                     0       //正常连接模式，关闭DUT测试
#define MODE_NORMAL_DUT                 1       //正常连接模式，使能DUT测试
#define MODE_FCC_TEST                   2       //FCC测试模式，通过串口通信
#define MODE_BQB_RF_BREDR               3       //BR/EDR DUT测试模式，通过蓝牙通信
#define MODE_BQB_RF_BLE                 4       //BLE DUT测试模式，通过串口通信
#define MODE_IODM_TEST                  5       //IODM测试模式，通过串口通信

//蓝牙功能
#define PROF_A2DP                       0x0007  //蓝牙音乐功能
#define PROF_HID                        0x0018  //蓝牙键盘功能
#define PROF_HFP                        0x0060  //蓝牙通话功能
#define PROF_SPP                        0x0080  //蓝牙串口功能
#define PROF_PBAP                       0x0100  //蓝牙电话本功能
#define PROF_HSP                        0x0200
#define PROF_MAP                        0x0400  //蓝牙短息功能

//蓝牙特性
#define HFP_BAT_REPORT                  0x01    //是否支持手机电量显示
#define HFP_3WAY_CALL                   0x02    //是否支持三通电话
#define HFP_INBAND_RING_TONE            0x04    //是否支持手机来电铃声
#define HFP_CALL_PRIVATE                0x08    //是否打开强制私密接听
#define HFP_SIRI_CMD                    0x10    //是否打开siri控制命令
#define HFP_EC_AND_NR                   0x20    //是否打开手机端回音和降噪
#define HFP_RING_NUMBER_EN              0x40    //是否支持来电报号
#define A2DP_AVRCP_VOL_CTRL             0x01    //是否支持手机音量控制同步
#define A2DP_AVRCP_PLAY_STATUS_IOS      0x02    //是否支持IOS手机播放状态同步，可加快播放暂停响应速度，蓝牙后台建议打开；注意：打开后微信小视频会无声
#define A2DP_AVRCP_PLAY_STATUS          0x04    //是否支持手机播放状态同步，可加快播放暂停响应速度，蓝牙后台建议打开；
#define A2DP_RESTORE_PLAYING            0x08    //是否支持掉线回连后恢复播放
#define A2DP_AVDTP_DELAY_REPORT         0x10    //是否支持AVDTP delay report功能
#define A2DP_AVDTP_DYN_LATENCY          0x20    //是否支持动态延迟控制功能
#define A2DP_AVDTP_EXCEPT_REST_PLAY     0x40    //是否支持异常复位后恢复连接和播放状态功能
//a2dp特性扩展1
#define A2DP_AVRCP_RECORD_DEVICE_VOL    0x01    //分别记录不同连接设备的音量
#define A2DP_RESET_DEVICE_VOL           0x02    //不支持音量同步手机，连接恢复音量
//hfp特性扩展
#define HFP_RECORD_DEVICE_VOL           0x01    //分别记录不同连接设备的音量

//蓝牙编解码
#define CODEC_SBC                       0x01
#define CODEC_AAC                       0x02
#define CODEC_MSBC                      0x04
#define CODEC_PLC                       0x08
#define CODEC_LHDCV5                    0x10
#define CODEC_LHDCV4                    0x20
#define CODEC_LDAC                      0x40


//删除配对信息TAG
#define BT_INFO_TAG_NOR                 "NULL"   //删除手机
#define BT_INFO_TAG_UNPAIR              "UPAR"   //删除TWS，并断开
#define BT_INFO_TAG_QTEST               "QTET"   //快测删除TWS配对信息
#define BT_INFO_TAG_IODM                "IODM"   //IODM删除TWS配对信息
#define BT_INFO_TAG_CHARGE_BOX          "CBOX"   //充电仓删除TWS配对信息
#define BT_INFO_TAG_USER                "USER"   //用户删除TWS配对信息

//蓝牙状态
enum
{
    BT_STA_OFF,                                 //蓝牙模块已关闭
    BT_STA_INITING,                             //初始化中
    BT_STA_IDLE,                                //蓝牙模块打开，未连接

    BT_STA_SCANNING,                            //扫描中
    BT_STA_DISCONNECTING,                       //断开中
    BT_STA_CONNECTING,                          //连接中

    BT_STA_CONNECTED,                           //已连接
    BT_STA_PLAYING,                             //播放
    BT_STA_INCOMING,                            //来电响铃
    BT_STA_OUTGOING,                            //正在呼出
    BT_STA_INCALL,                              //通话中
    BT_STA_OTA,                                 //OTA升级中
};

//通话状态
enum
{
    BT_CALL_IDLE,                               //
    BT_CALL_INCOMING,                           //来电响铃
    BT_CALL_OUTGOING,                           //正在呼出
    BT_CALL_ACTIVE,                             //通话中
    BT_CALL_3WAY_CALL,                          //三通电话或两部手机通话
};

//蓝牙通知
enum
{
    BT_NOTICE_INIT_FINISH,                      //蓝牙初始化完成
    BT_NOTICE_CONNECT_START,                    //开始回连手机, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_CONNECT_FAIL,                     //回连手机失败, param[0]=reason, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_DISCONNECT,                       //蓝牙断开,     param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_LOSTCONNECT,                      //蓝牙连接丢失, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_CONNECTED,                        //蓝牙连接成功, param[0]=feat,index, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_SCO_SETUP,
    BT_NOTICE_SCO_FAIL,
    BT_NOTICE_SCO_KILL,
    BT_NOTICE_INCOMING,                         //来电
    BT_NOTICE_RING,                             //来电响铃
    BT_NOTICE_OUTGOING,                         //去电
    BT_NOTICE_CALL_NUMBER,                      //来电/去电号码
    BT_NOTICE_INCALL,                           //建立通话
    BT_NOTICE_ENDCALL,                          //结束通话
    BT_NOTICE_NETWORK_CALL,                     //网络通话
    BT_NOTICE_PHONE_CALL,                       //手机通话

    BT_NOTICE_SET_SPK_GAIN,                     //设置通话音量
    BT_NOTICE_CALL_CHANGE_DEV,                  //1拖2时改变了通话设备
    BT_NOTICE_MUSIC_PLAY,                       //蓝牙音乐开始播放
    BT_NOTICE_MUSIC_STOP,                       //蓝牙音乐停止播放
    BT_NOTICE_MUSIC_CHANGE_VOL,                 //手机端改变蓝牙音乐音量, param[0]=down/up, param[1]=index, param[7:2]=bd_addr
    BT_NOTICE_MUSIC_SET_VOL,                    //手机端设置蓝牙音乐音量, param[0]=a2dp_vol, param[1]=index, param[7:2]=bd_addr
    BT_NOTICE_MUSIC_CHANGE_DEV,                 //1拖2时改变了播放设备, 例如从A手机切换到B手机, param[0]=a2dp_vol, param[1]=index, param[7:2]=bd_addr
    BT_NOTICE_HID_CONN_EVT,                     //HID服务连接事件
    BT_NOTICE_A2DP_CONN_EVT,                    //A2DP服务连接事件
    BT_NOTICE_HFP_CONN_EVT,                     //HFP服务连接事件
    BT_NOTICE_RECON_FINISH,                     //回连完成, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_ABORT_STATUS,                     //中止状态, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_NORCONNECT_FAIL,                  //手机发起连接到一半失败, param[0]=status, param[1]=reason, param[7:2]=bd_addr
    BT_NOTICE_LOW_LATENCY_STA,                  //低延时状态切换

    ///PBAP相关消息
    BT_NOTICE_PBAP_CONNECTED,
    BT_NOTICE_PBAP_GET_PHONEBOOK_SIZE_COMPLETE, //获取电话薄数量完成
    BT_NOTICE_PBAP_PULL_PHONEBOOK_COMPLETE,     //获取电话薄完成
    BT_NOTICE_PBAP_DISCONNECT,

    BT_NOTICE_HFP_HF_ERROR,                     //通话错误，比如拨打错误电话号码
    BT_NOTICE_FAST_MUSIC_STATUS,               //快速上报音乐播放暂停状态
    BT_NOTICE_RSSI_REPORT,                      //上报连接RSSI
    BT_NOTICE_INQUIRY_FINISH,                    //搜索完成返回状态
    BT_NOTICE_BT_INFO,                          //返回蓝牙地址以及蓝牙名字信息
};

//param[0]=feat,index
enum
{
    FEAT_TWS_FLAG       = 0x80,
    FEAT_TWS_ROLE       = 0x40,
    FEAT_TWS_MUTE_FLAG  = 0x20,
    FEAT_TWS_FIRST_ROLE = 0x10,
    FEAT_INCOME_CON     = 0x08,
    FEAT_FIRST_CON      = 0x04,
    FEAT_INDEX_MASK     = 0x03,
};

//控制消息
enum
{
    BT_CTL_VOL_CHANGE               = 0,        //音量调整，之后通过回调函数设置音量
    BT_CTL_PLAY_PAUSE,                          //切换播放、暂停
    BT_CTL_VOL_UP,                              //音乐加音量，之后通过回调函数调节音量
    BT_CTL_VOL_DOWN,                            //音乐减音量，之后通过回调函数调节音量
    BT_CTL_2ACL_PALY_SWITCH,                    //一拖二播放切换
    BT_CTL_GET_ID3_TAG,
    BT_CTL_GET_PLAY_STATUS_INFO,
    BT_CTL_MSC_RES3,

    BT_CTL_CALL_REDIAL,                         //回拨电话（最后一次通话）
    BT_CTL_CALL_REDIAL_NUMBER,                  //回拨电话（从hfp_get_outgoing_number获取号码）
    BT_CTL_CALL_ANSWER_INCOM,                   //接听来电（三通时挂起当前通话）
    BT_CTL_CALL_ANSWER_INCOM_REJ_OTHER,         //接听来电（三通时挂断当前通话）
    BT_CTL_CALL_TERMINATE,                      //挂断通话或来电
    BT_CTL_CALL_SWAP,                           //切换三通电话
    BT_CTL_CALL_PRIVATE_SWITCH,                 //切换私密通话
    BT_CTL_HFP_REPORT_BAT,                      //报告电池电量
    BT_CTL_HFP_MIC_GAIN,                        //设置通话麦克风音量
    BT_CTL_HFP_AT_CMD,                          //发送AT命令（从hfp_get_at_cmd获取命令）
    BT_CTL_HFP_SIRI_SW,                         //唤出/关闭siri
    BT_CTL_HFP_REMOTE_PHONE_NUM,
    BT_CTL_HFP_SWITCH_TO_PHONE,
    BT_CTL_HFP_SWITCH_TO_WATCH,
    BT_CTL_HFP_CUSTOM_AT_CMD,
    BT_CTL_CALL_ANSWER_INCOM_HOLD_OTHER,
    BT_CTL_HFP_RES3,

    BT_CTL_TWS_SWITCH,                          //主从切换
    BT_CTL_NOR_CONNECT,
    BT_CTL_NOR_DISCONNECT,
    BT_CTL_HID_CONNECT,
    BT_CTL_HID_DISCONNECT,
    BT_CTL_CONN_RES1,
    BT_CTL_CONN_RES2,

    BT_CTL_BLE_ADV_DISABLE,                     //关闭BLE 广播
    BT_CTL_BLE_ADV_ENABLE,                      //打开BLE 广播
    BT_CTL_BLE_RES1,
    BT_CTL_BLE_RES2,

    BT_CTL_FOT_RESP,
    BT_CTL_FCC_TEST,
    BT_CTL_LOW_LATENCY_EN,
    BT_CTL_LOW_LATENCY_DIS,
    BT_CTL_EAR_STA_CHANGE,
    BT_CTL_NR_STA_CHANGE,
    BT_CTL_AAP_USER_DATA,
    BT_CTL_A2DP_CONNECT_AND_DISCONNECT,
    BT_CTL_A2DP_PROFILE_EN,
    BT_CTL_A2DP_PROFILE_DIS,
    BT_CTL_HFP_CONNECT,
    BT_CTL_HFP_DISCONNECT,
    BT_CTL_UPD_BT_SCAN_PARAM,
    BT_CTL_FORCE_CLOSE_SIRI,
    BT_CTL_FORCE_OPEN_SIRI,
    BT_CTL_SNIFF_DROP_OUT,
    BT_CTL_HFP_SPK_GAIN,                        //设置通话扬声器音量
    BT_CTL_HID_PROFILE_EN,
    BT_CTL_EMIT_INQ,
    BT_CTL_EMIT_CANCEL,
    BT_CTL_AVDTP_START,
    BT_CTL_AVDTP_SUSPEND,
    BT_CTL_SALVE_SET_VOLUME,
    BT_CTL_MAX,

    BT_CTL_A2DP_VOLUME_UP           = 0xff0041,     //音量加
    BT_CTL_A2DP_VOLUME_DOWN         = 0xff0042,     //音量减
    BT_CTL_A2DP_MUTE                = 0xff0043,     //MUTE
    BT_CTL_A2DP_PLAY                = 0xff0044,     //播放
    BT_CTL_A2DP_STOP                = 0xff0045,     //停止
    BT_CTL_A2DP_PAUSE               = 0xff0046,     //暂停
    BT_CTL_A2DP_RECORD              = 0xff0047,
    BT_CTL_A2DP_REWIND              = 0xff0048,     //快退
    BT_CTL_A2DP_FAST_FORWARD        = 0xff0049,     //快进
    BT_CTL_A2DP_EJECT               = 0xff004a,
    BT_CTL_A2DP_FORWARD             = 0xff004b,     //下一曲
    BT_CTL_A2DP_BACKWARD            = 0xff004c,     //上一曲
    BT_CTL_A2DP_REWIND_END          = 0xff00c8,     //结束快退
    BT_CTL_A2DP_FAST_FORWARD_END    = 0xff00c9,     //结束快进

    BT_CTL_NO                       = 0xffffff,
};

enum bt_msg_comm_t
{
    COMM_BT_CTL0,                               //无传参的消息
    COMM_BT_START_WORK,                         //蓝牙开始工作
    COMM_BT_SET_SCAN,                           //设置可被发现/可被连接
    COMM_BT_CONNECT,                            //连接蓝牙
    COMM_BT_DISCONNECT,                         //断开蓝牙
    COMM_BT_ABORT_RECONNECT,                    //中止回连
    COMM_BT_ATT_MSG,
    COMM_BT_SET_SCAN_INTV,                      //设置PAGE SCAN PARAM
    COMM_BT_SET_A2DP,
    COMM_BT_ABORT_PAGE_PSCAN,                   //中止PAGE和PAGESCAN
    COMM_LE_AMS_REMOTE_CTRL,                    //LE AMS remote ctl
    COMM_BT_AUTO_SNIFF,
};

enum bt_msg_hid_t
{
    HID_KEYBOARD,
    HID_CONSUMER,
    HID_TOUCH_SCREEN,
    HID_MOUSE,
};

enum bt_msg_pbap_t
{
    BT_PBAP_CTRL,
    BT_PBAP_SELECT_PHONEBOOK,
    BT_PBAP_GET_PHONEBOOK_SIZE,
    BT_PBAP_PULL_PHONEBOOK,
    BT_PBAP_SET_PATH,
};

//蓝牙消息
enum bt_msg_t
{
    BT_MSG_CTRL                 = 0,            //蓝牙控制消息
    BT_MSG_COMM,
    BT_MSG_RES1,
    BT_MSG_TWS,                                 //TWS消息
    BT_MSG_ADV0,
    BT_MSG_A2DP,                                //A2DP消息
    BT_MSG_HFP,                                 //HFP消息
    BT_MSG_HSP,                                 //HSP消息
    BT_MSG_HID,                                 //HID消息，KEYPAD按键/CONSUMER按键/触摸屏
    BT_MSG_PBAP,                                //PBAP消息
    BT_MSG_LECLT,                               //BLE CLIENT消息
    BT_MSG_MAP,                                 //MAP消息 获取时间
    BT_MSG_MAX,
    BT_MSG_RES                  = 0xf0,         //0xf0~0xff保留给传参较多的api
};

enum sync_info_enum
{
    SYNC_INFO_SETTING,                  //VOL/LANG/EQ/BAT/...
    SYNC_INFO_LEDCNT,                   //同步 led cnt
    SYNC_INFO_EQPARAM,
    SYNC_INFO_FOT_DATA,
    SYNC_INFO_RING_STA,
    SYNC_INFO_CUSTOM_DATA,
};

//tws_status
enum
{
    TWS_STA_FLAG        = 0x80,
    TWS_STA_ROLE        = 0x40,
    TWS_STA_NOR_FLAG    = 0x01,
};

//LE Addr Type, public or random(static or non resolvable or resolvable)
enum
{
    GAP_RANDOM_ADDRESS_TYPE_OFF = 0,    //Public
    GAP_RANDOM_ADDRESS_TYPE_STATIC,
    GAP_RANDOM_ADDRESS_NON_RESOLVABLE,
    GAP_RANDOM_ADDRESS_RESOLVABLE,
};

//LE状态
enum
{
    LE_STA_STANDBY,
    LE_STA_ADVERTISING,                         //正在广播
    LE_STA_CONNECTION,                          //已连接
};

//LE 通知
enum
{
    LE_NOTICE_CONNECTED,                        //连接成功
    LE_NOTICE_DISCONNECT,                       //断开成功
    LE_NOTICE_CONN_PARAM_UPDATE,                //连接参数更新
    LE_NOTICE_DATA_LEN_CHANGE,                  //收发长度改变
    LE_NOTICE_CLINET_CFG,                       //客户端配置特性

    LE_NOTICE_ANCS_CONN_EVT,                    //ancs client连接事件
    LE_NOTICE_AMS_CONN_EVT,                     //ams  client连接事件
};

//LE GATT 服务相关
/**
    BLE GATTS ERR return
*/
#define BLE_GATTS_SUCCESS                                       0x00
#define BLE_GATTS_SRVC_TYPE_ERR                                 0x01
#define BLE_GATTS_SRVC_RULES_ERR                                0x02
#define BLE_GATTS_SRVC_PROPS_ERR                                0x03
#define BLE_GATTS_SRVC_ATT_FULL_ERR                             0x04
#define BLE_GATTS_SRVC_PROFILE_FULL_ERR                         0x05


/**
    att property flag
*/
#define ATT_BROADCAST                                           0x01
#define ATT_READ                                                0x02
#define ATT_WRITE_WITHOUT_RESPONSE                              0x04
#define ATT_WRITE                                               0x08
#define ATT_NOTIFY                                              0x10
#define ATT_INDICATE                                            0x20
#define ATT_AUTHENTICATED_SIGNED_WRITE                          0x40
#define ATT_EXTENDED_PROPERTIES                                 0x80
#define ATT_DYNAMIC                                             0x100
#define ATT_UUID128                                             0x200
#define ATT_AUTHENTICATION_REQUIRED                             0x400
#define ATT_AUTHORIZATION_REQUIRED                              0x800


/**
    define group GATT Server Service Types
*/
typedef enum
{
    BLE_GATTS_SRVC_TYPE_PRIMARY  =   0x00,                //Primary Service
    BLE_GATTS_SRVC_TYPE_SEVONDARY=   0x01,                //Secondary Service
    BLE_GATTS_SRVC_TYPE_INCLUDE  =   0x02,                //include Type
} ble_gatts_service_type;

/**
    define group GATT Server UUID Types
*/
typedef enum
{
    BLE_GATTS_UUID_TYPE_16BIT  =   0x00,                //UUID 16BIT
    BLE_GATTS_UUID_TYPE_128BIT =   0x01,                //UUID 128BIT
} ble_gatts_uuid_type;

/**
    define GATT handler callback
    con_handle: connect handle
    handle    : attribute handle
    flag      : call back flag
*/
#define ATT_CB_FALG_OFFSET         (0xffff)
#define ATT_CB_FALG_DIR_WRITE      (0x10000)
typedef int (*ble_gatt_callback_func)(uint16_t con_handle, uint16_t handle, uint32_t flag, uint8_t *ptr, uint16_t len);

/**
    define GATT service base
*/
typedef struct gatts_service_base
{
    uint16_t handle;
} gatts_service_base_st;


/**
    define GATT service uuid
*/
typedef struct gatts_uuid_base
{
    uint16_t props;
    uint8_t type;
    const uint8_t *uuid;
} gatts_uuid_base_st;
/**
    define GATT service call back info
*/
typedef struct
{
    uint16_t client_config;                         // att config, 1:notify enable; 2:indicate enable;
    uint16_t value_len;
    ble_gatt_callback_func att_write_callback_func; // 特征值的写操作回调
    ble_gatt_callback_func att_read_callback_func;  // 特征值的读操作回调, 注意要设置value_len
    uint8_t *value;                                 // read return data, when att_read_callback_func is NULL
} ble_gatt_characteristic_cb_info_t;

#define GATT_CLIENT_CONFIG_NOTIFY       1
#define GATT_CLIENT_CONFIG_INDICATE     2

/**
    define group GATT Server profile struct
*/
typedef struct gatts_profile_list
{
    void *item;
    uint16_t  profile_start_handle;
    uint16_t  profile_end_handle;
    const uint8_t   *profile_date;
    uint16_t profile_date_size;
} ble_gatts_profile_list_st;

#define BD_ADDR_LENGTH 6
typedef uint8_t bd_addr_t[BD_ADDR_LENGTH];

extern uint8_t cfg_bt_work_mode;
extern uint8_t cfg_bt_max_acl_link;
extern bool cfg_bt_dual_mode;
extern bool cfg_bt_tws_mode;
extern uint8_t cfg_bt_scan_ctrl_mode;
extern bool cfg_bt_simple_pair_mode;
extern bool cfg_bt_voip_reject_en;
extern bool cfg_bt_hfp_switch_en;
extern uint16_t cfg_bt_support_profile;
extern uint16_t cfg_bt_support_codec;
extern uint8_t cfg_bt_hid_type;
extern uint8_t cfg_bt_connect_times;
extern uint8_t cfg_bt_pwrup_connect_times;
extern uint16_t cfg_bt_sup_to_connect_times;

extern uint8_t cfg_bt_rf_def_txpwr;
extern uint8_t cfg_bt_page_txpwr;
extern uint8_t cfg_bt_inq_txpwr;
extern uint8_t cfg_ble_page_txpwr;
extern uint8_t cfg_ble_page_rssi_thr;

extern uint8_t cfg_bt_a2dp_feature;
extern uint8_t cfg_bt_a2dp_feature1;
extern uint8_t cfg_bt_hfp_feature;
extern uint8_t cfg_bt_hfp_feature1;
extern uint8_t cfg_bt_tws_pair_mode;
extern uint16_t cfg_bt_tws_feat;
extern uint8_t cfg_bt_tws_not_auto_connect;
extern uint8_t cfg_bt_hci_disc_only_spp;

extern uint8_t cfg_bt_lhdc_codec_feature;

extern const uint16_t link_info_page_size;
extern bool cfg_bt_emit_mode;
extern bool cfg_bt_connect_name_en;

#define bt_is_scan_ctrl()                       cfg_bt_scan_ctrl_mode
#define bt_a2dp_is_vol_ctrl()                   (cfg_bt_a2dp_feature & A2DP_AVRCP_VOL_CTRL)

#define BT_OPT_AFH_UNKNOW_DIS               BIT(0)
#define BT_OPT_SCO_ACK_EN                   BIT(1)
extern const uint8_t cfg_bb_bt_opt;

//baseband
bool bb_timer_init(uint16_t intv);
void bb_timer_kill(void);

u32 bb_timer_get(void);                         //周期为625us的本地时钟

//control
void bb_run_loop(void);
void bt_fcc_init(void);
void bt_init(void);                             //初始化蓝牙变量
int bt_setup(void);                             //打开蓝牙模块
void bt_off(void);                              //关闭蓝牙模块
void bt_wakeup(void);                           //唤醒蓝牙模块
void bt_start_work(uint8_t opcode, uint8_t scan_en); //蓝牙开始工作，opcode: 0=回连, 1=不回连
void bt_send_msg_do(uint msg);                  //蓝牙控制消息，参数详见bt_msg_t
void bt_thread_check_trigger(void);
void bt_audio_bypass(void);                     //蓝牙SBC/SCO通路关闭
void bt_audio_enable(void);                     //蓝牙SBC/SCO通路使能
void bt_get_stack_local_name(char* name);
void bt_set_stack_local_name(const char* name);
void bt_set_sco_far_delay(void *buf, uint size, uint delay);

void bt_set_scan(uint8_t scan_en);              //打开/关闭可被发现和可被连接, bit0=可被发现, bit1=可被连接
uint8_t bt_get_scan(void);                      //获取设置的可被发现可被连接状态（已连接时设置完不会立即生效，需要等断开连接）
uint8_t bt_get_curr_scan(void);                 //获取当前可被发现可被连接状态
void bt_connect(void);                          //蓝牙设备回连, 回连次数在cfg_bt_connect_times配置
void bt_connect_address(void);                  //蓝牙设备回连地址, 回连地址在bt_get_connect_addr函数设置
void bt_disconnect_address(void);               //蓝牙设备断开地址, 断开地址在bt_get_disconnect_addr函数设置
void bt_disconnect(uint reason);                //蓝牙设备断开, reason: 0=单独断开（入仓）; 1=断开并同步关机（按键/自动关机）;用户单独调用断开，并不关机reason=0xff
void bt_hid_connect(void);                      //蓝牙HID服务回连
void bt_hid_disconnect(void);                   //蓝牙HID服务断开
int bt_hid_is_connected(void);
bool bt_hid_is_ready_to_discon(void);

//status
uint bt_get_disp_status(void);                  //获取蓝牙的当前显示状态, V060
uint bt_get_status(void);                       //获取蓝牙的当前状态
uint8_t bt_get_scan(void);                      //判断当前可被连接可被扫描状态
uint8_t bt_get_curr_scan(void);                 //获取实时可被连接可被扫描状态
uint bt_get_call_indicate(void);                //获取通话的当前状态
uint bt_get_siri_status(void);                  //获取SIRI当前状态, 0=SIRI已退出, 1=SIRI已唤出
uint bt_get_force_siri_status(void);            //获取SIRI当前状态，0=SIRI已退出, 1=SIRI已唤出，配合bt_hfp_siri_close和bt_hfp_siri_open使用
bool bt_is_calling(void);                       //判断是否正在通话
bool bt_is_playing(void);                       //判断是否正在播放
bool bt_is_ring(void);                          //判断是否正在响铃
bool bt_is_testmode(void);                      //判断当前蓝牙是否处于测试模式
bool bt_is_sleep(void);                         //判断蓝牙是否进入休眠状态
bool bt_is_allow_sleep(void);                   //判断蓝牙是否允许进入休眠状态
bool bt_is_connected(void);                     //判断蓝牙是否已连接（TWS副耳被连接，或主耳与手机已连接）
bool bt_is_ios_device(void);                    //判断当前播放的是否ios设备
bool bt_is_support_vol_ctrl(void);              //判断当前播放的是否支持音量同步
uint32_t bt_sleep_proc(void);
void bt_enter_sleep(void);
void bt_exit_sleep(void);
void bt_updata_local_name(char *bt_name);
bool sco_is_connected(void);
bool bt_sco_is_msbc(void);                      //判断当前通话是否是宽带通话
bool sbc_is_bypass(void);
bool bt_is_low_latency(void);                   //判断蓝牙是否在低延时状态
bool bt_is_silence(void);
bool bt_decode_is_aac(void);                    //判断蓝牙解码是否是aac
uint8_t bt_get_connected_num(void);             //一拖二获取当前连接了几台设备
uint8_t bt_get_cur_a2dp_media_index(void);      //一拖二获取当前播放设备的index
bool bt_a2dp_profile_completely_connected(void);//根据profile来判断A2DP是否已连接完全
void bt_reset_addr(void);                       //重置bt地址, 值通过bt_get_local_bd_addr设置;

//info
bool bt_get_link_btname(uint8_t index, char *name, uint8_t max_size); //index: 0=link0, 1=link1, 0xff=auto(default link0)
void bt_get_local_bd_addr(u8 *addr);
void bt_read_conn_rssi(void);                   //获取当前连接RSSI


void bt_nor_connect(void);                      //回连手机
void bt_nor_disconnect(void);                   //断开手机
bool bt_nor_get_link_info(uint8_t *bd_addr);    //获取手机配对信息，bd_addr=NULL时仅查询是否存在回连信息
bool bt_nor_get_link_info_addr(uint8_t *bd_addr, uint8_t order);    //获取第n个手机配对信息，bd_addr=NULL时仅查询是否存在回连信息
void bt_nor_delete_link_info(void);             //删除手机配对信息
void bt_nor_unpair_device(void);                //删除手机配对信息并断开

///四个参数分别为iscan interval，iscan窗大小，pscan interval，pscan窗大小，默认值为4096,18,2048,18
///iscan对应搜索窗，pascn对应连接窗，调整interval跟窗大小会影响对应搜索及连接速度
void bt_update_bt_scan_param(uint16_t inq_scan_int, uint16_t inq_scan_win, uint16_t page_scan_int, uint16_t page_scan_win);
void bt_update_bt_scan_param_default(void);
void ble_send_sm_req_for_android(void);


#define bt_send_msg(ogf, ocf)                   bt_send_msg_do((ogf<<24) | (ocf))
#define bt_ctrl_msg(msg)                        bt_send_msg(BT_MSG_CTRL, msg)
#define bt_comm_msg(msg, param)                 bt_send_msg(BT_MSG_COMM, (msg<<16) | (u16)(param))
#define bt_tws_msg(msg, param)                  bt_send_msg(BT_MSG_TWS, (msg<<16) | (u16)(param))
#define bt_hid_msg(msg, param)                  bt_send_msg(BT_MSG_HID, (msg<<16) | (u16)(param))
#define bt_pbap_msg(msg, param)                 bt_send_msg(BT_MSG_PBAP, (msg<<16) | (u16)param)

//蓝牙连接
#define bt_scan_enable()                        bt_set_scan(0x03)                       //打开扫描
#define bt_scan_disable()                       bt_set_scan(0x00)                       //关闭扫描
#define bt_set_scan_param(ps_intv,is_intv)      bt_comm_msg(COMM_BT_SET_SCAN_INTV, (ps_intv << 8) | is_intv)  //设置scan 参数，0x100*(intv+1)
#define bt_abort_reconnect()                    bt_comm_msg(COMM_BT_ABORT_RECONNECT, 0xffff)        //终止回接
#define bt_abort_reconnect_silence(feat)        bt_comm_msg(COMM_BT_ABORT_RECONNECT, (u16)feat)     //终止回接，没有消息回调。feat:0=手机, BT_FEAT_TWS=TWS
#define bt_auto_sniff_set(val)                  bt_comm_msg(COMM_BT_AUTO_SNIFF,(u16)val)                 //配置sniff自动模式：0:关闭 1:开启


//蓝牙音乐
#define bt_music_play()                         bt_ctrl_msg(BT_CTL_A2DP_PLAY)               //播放
#define bt_music_pause()                        bt_ctrl_msg(BT_CTL_A2DP_PAUSE)              //暂停
#define bt_music_play_pause()                   bt_ctrl_msg(BT_CTL_PLAY_PAUSE)              //切换播放/暂停
#define bt_music_stop()                         bt_ctrl_msg(BT_CTL_A2DP_STOP)               //停止
#define bt_music_prev()                         bt_ctrl_msg(BT_CTL_A2DP_BACKWARD)           //上一曲
#define bt_music_next()                         bt_ctrl_msg(BT_CTL_A2DP_FORWARD)            //下一曲
#define bt_music_rewind()                       bt_ctrl_msg(BT_CTL_A2DP_REWIND)             //开始快退
#define bt_music_rewind_end()                   bt_ctrl_msg(BT_CTL_A2DP_REWIND_END)         //结束快退
#define bt_music_fast_forward()                 bt_ctrl_msg(BT_CTL_A2DP_FAST_FORWARD)       //开始快进
#define bt_music_fast_forward_end()             bt_ctrl_msg(BT_CTL_A2DP_FAST_FORWARD_END)   //结束快进
#define bt_music_vol_change()                   bt_ctrl_msg(BT_CTL_VOL_CHANGE)              //调节音乐音量，之后通过回调函数a2dp_vol_set_cb设置音量
#define bt_music_vol_up()                       bt_ctrl_msg(BT_CTL_VOL_UP)                  //音乐加音量，之后通过回调函数a2dp_vol_adj_cb调节音量
#define bt_music_vol_down()                     bt_ctrl_msg(BT_CTL_VOL_DOWN)                //音乐减音量，之后通过回调函数a2dp_vol_adj_cb调节音量
#define bt_music_play_switch()                  bt_ctrl_msg(BT_CTL_2ACL_PALY_SWITCH)        //一拖二切换播放手机
#define bt_music_get_id3_tag()                  bt_ctrl_msg(BT_CTL_GET_ID3_TAG);            //主动查询id3 tag信息
#define bt_music_paly_status_info()             bt_ctrl_msg(BT_CTL_GET_PLAY_STATUS_INFO);   //获取歌曲时长单位ms,当前播放位置,播放状态信息
#define bt_low_latency_enable()                 bt_ctrl_msg(BT_CTL_LOW_LATENCY_EN)          //蓝牙使能低延时
#define bt_low_latency_disable()                bt_ctrl_msg(BT_CTL_LOW_LATENCY_DIS)         //蓝牙关闭低延时
#define bt_fcc_test_start()                     bt_ctrl_msg(BT_CTL_FCC_TEST)                //FCC test模式

//蓝牙通话
#define bt_call_redial_last_number()            bt_ctrl_msg(BT_CTL_CALL_REDIAL)         //电话回拨（最后一次通话）
#define bt_call_answer_incoming()               bt_ctrl_msg(BT_CTL_CALL_ANSWER_INCOM)   //接听电话，三通时挂起当前通话
#define bt_call_answer_incom_rej_other()        bt_ctrl_msg(BT_CTL_CALL_ANSWER_INCOM_REJ_OTHER)     //接听电话，三通时挂断当前通话，1拖2时挂断当前的手机通话
#define bt_call_answer_incom_hold_other()       bt_ctrl_msg(BT_CTL_CALL_ANSWER_INCOM_HOLD_OTHER)    //接听电话，三通时挂起当前通话，1拖2时挂起当前的手机通话
#define bt_call_terminate()                     bt_ctrl_msg(BT_CTL_CALL_TERMINATE)      //挂断电话
#define bt_call_swap()                          bt_ctrl_msg(BT_CTL_CALL_SWAP)           //切换三通电话
#define bt_call_private_switch()                bt_ctrl_msg(BT_CTL_CALL_PRIVATE_SWITCH) //切换私密通话
#define bt_call_redial_number()                 bt_ctrl_msg(BT_CTL_CALL_REDIAL_NUMBER)
#define bt_call_get_remote_phone_number()       bt_ctrl_msg(BT_CTL_HFP_REMOTE_PHONE_NUM) //获取远端号码
#define bt_hfp_siri_switch()                    bt_ctrl_msg(BT_CTL_HFP_SIRI_SW)         //开关SIRI, android需要在语音助手中打开“蓝牙耳机按键启动”, ios需要打开siri功能
#define bt_hfp_report_bat()                     bt_ctrl_msg(BT_CTL_HFP_REPORT_BAT)
#define bt_hfp_set_spk_gain()                   bt_ctrl_msg(BT_CTL_HFP_SPK_GAIN)
#define bt_hfp_send_at_cmd()                    bt_ctrl_msg(BT_CTL_HFP_AT_CMD)
#define bt_hfp_send_custom_at_cmd()             bt_ctrl_msg(BT_CTL_HFP_CUSTOM_AT_CMD)
#define bt_hfp_switch_to_phone()                bt_ctrl_msg(BT_CTL_HFP_SWITCH_TO_PHONE) //通话切到手机
#define bt_hfp_switch_to_watch()                bt_ctrl_msg(BT_CTL_HFP_SWITCH_TO_WATCH) //通话切在手表
#define bt_hfp_siri_close()                     bt_ctrl_msg(BT_CTL_FORCE_CLOSE_SIRI)    //把siri关闭，暴力测试专属，siri状态改用bt_get_force_siri_status
#define bt_hfp_siri_open()                      bt_ctrl_msg(BT_CTL_FORCE_OPEN_SIRI)     //把siri打开，暴力测试专属，siri状态改用bt_get_force_siri_status
#define bt_sniff_drop_out()                     bt_ctrl_msg(BT_CTL_SNIFF_DROP_OUT)      //退出SNIFF模式

//服务控制
#define bt_a2dp_profile_en()                    bt_ctrl_msg(BT_CTL_A2DP_PROFILE_EN)     //打开A2DP服务
#define bt_a2dp_profile_dis()                   bt_ctrl_msg(BT_CTL_A2DP_PROFILE_DIS)    //关闭A2DP服务
#define bt_hfp_profile_en()                     bt_ctrl_msg(BT_CTL_HFP_CONNECT)         //打开HFP服务
#define bt_hfp_profile_dis()                    bt_ctrl_msg(BT_CTL_HFP_DISCONNECT)      //关闭HFP服务
#define bt_hid_profile_en()                     bt_ctrl_msg(BT_CTL_HID_PROFILE_EN)      //打开HID服务

//PBAP
#define bt_pbap_connect()                       bt_pbap_msg(BT_PBAP_CTRL, 1)
#define bt_pbap_disconnect()                    bt_pbap_msg(BT_PBAP_CTRL, 0)
#define bt_pbap_select_phonebook(book, sim)     bt_pbap_msg(BT_PBAP_SELECT_PHONEBOOK, (sim<<8) | (u8)book)
// sim - 1:选择SIM卡，0:本机
// book- 0:pb, 1:fav, 2-ich, 3:och, 4-mch, 5-cch, 6-spd
// 若不配置，则选择默认值为本机pb
#define bt_pbap_get_phonebook_size()            bt_pbap_msg(BT_PBAP_GET_PHONEBOOK_SIZE, 0)
#define bt_pbap_pull_phonebook_whole()          bt_pbap_msg(BT_PBAP_PULL_PHONEBOOK, 0)
#define bt_pbap_pull_phonebook_single(idx)      bt_pbap_msg(BT_PBAP_PULL_PHONEBOOK, idx)
// 按编号获取联系人信息
// idx不为零，如果为零则直接获取整个电话本信息
#define bt_pbap_set_path()                      bt_pbap_msg(BT_PBAP_SET_PATH, 0)

//pkt
typedef void (*kick_func_t)(void);

struct txbuf_tag
{
    uint8_t *ptr;
    uint16_t len;
    uint16_t handle;
} __attribute__ ((packed)) ;

struct txpkt_tag
{
    struct txbuf_tag *buf;
    uint16_t buf_size;
    uint8_t total;
    volatile uint8_t cnt;
    uint8_t rptr;
    volatile uint8_t wptr;
    kick_func_t send_kick;
};
extern struct txpkt_tag notify_tx;

uint32_t txpkt_init(struct txpkt_tag *txpkt, void *mem_pool, uint8_t total, uint16_t buf_size);

//init
void hfp_hf_init(void);
void a2dp_init(void);
void aap_init(void);
uint8_t sdp_add_service(void *item);
void sdp_rmv_service(uint32_t service_record_handle);
uint bt_get_hfp_feature(void);
bool bt_hfp_is_connected(void);

//a2dp
bool a2dp_is_playing_fast(void);
uint8_t a2dp_vol_reverse(uint vol);                 //将系统音量转换为a2dp_vol
uint8_t a2dp_vol_conver(uint8_t a2dp_vol);          //将a2dp_vol转换为系统音量级数
//spp
enum
{
    SPP_SERVICE_CH0,     //channel0，是默认SPP UUID 服务通路
    SPP_SERVICE_CH1,     //channel1，自定义UUID,默认用GFPS
    SPP_SERVICE_CH2,     //channel2, 自定义UUID，未使用
};
void spp_init(void);
extern struct txpkt_tag spp_tx_ch0;
extern struct txpkt_tag spp_tx_ch1;
extern struct txpkt_tag spp_tx_ch2;
void spp_send_kick(void);
int bt_spp_tx(uint8_t ch, uint8_t *packet, uint16_t len);
void spp_support_mul_server(uint8_t support);
bool spp_is_connect(void);
bool spp_is_connected_with_channel(uint8_t ch);  //判断某一个SPP通路是否连接，目前只有 0,1,2 共3个通路
void spp_disconnect(void);

//hid
void hid_device_init(void);
bool bt_hid_send(void *buf, uint len, bool auto_release);                                           //自定义HID数组
bool bt_hid_send_key(uint type, uint keycode);                                                      //标准HID按键
#define bt_hid_key(keycode)                     bt_hid_send_key(HID_KEYBOARD, keycode)              //标准HID键, 如Enter
#define bt_hid_consumer(keycode)                bt_hid_send_key(HID_CONSUMER, keycode)              //自定义HID键, 如VOL+ VOL-
#define bt_hid_touch_screen(keycode)            bt_hid_send_key(HID_TOUCH_SCREEN, keycode)          //触屏
bool bt_hid_touch_screen_set_key(void *ts);

#define bt_hid_finger_select_ios()              bt_hid_msg(HID_TOUCH_SCREEN, 1)                //抖音视频选择IOS系统
#define bt_hid_finger_select_andriod()          bt_hid_msg(HID_TOUCH_SCREEN, 2)                //抖音视频选择andriod系统
/**
 * @brief 模拟触点函数
   注意:IOS 范围是-2047-2048 ，安卓是0-4096;
        IOS设备，x,y是相对位置，比如10,10是相对当前位置移动10,10;
        安卓设备，x，y是绝对位置，10,10是在手机10,10的位置上;
 * @param is_press  1按下，0抬起
 * @param x 模拟触点横坐标
 * @param y 模拟触点纵坐标
 **/
void bt_hid_point_pos(bool is_press, s16 x, s16 y);

//goep
void goep_client_init(void);
//pbap
void pbap_client_init(void);
void bt_pbap_start(u8 type);                    //type：获取本地号码:BIT(0)、获取来电号码:BIT(1)、获取去电号码:BIT(2)、获取未接号码:BIT(3)
void bt_pbap_abort(void);                       //终止电话本的获取
u32 bt_pbap_get_sta(void);                      //获取电话本状态，0：IDLE； 1：获取中
void bt_pbap_lookup_number(char *phone_number); //根据电话号码获取联系人名字
//map
void map_client_init(void);
void bt_map_start(void);                        //MAP的获取
void bt_map_abort(void);                        //终止MAP的获取
//hsp
void hsp_hs_init(void);
void hsp_hs_init_var(void);
void bt_hsp_call_switch(void);                  //挂断/接听
void bt_hsp_sco_conn(void);                     //建立HSP SCO连接
void bt_hsp_sco_disconn(void);                  //断开HSP SCO连接

/*****************************************************************************
 * SIMPLE KEYBOARD API
 *****************************************************************************/
#define bt_hid_simple_keyboard(keycode)         bt_hid_send_key(HID_SIMPLE_KEYBOARD, keycode)       //简易键盘
#define SIMPLE_KEYBOARD_VOL_UP      0x01
#define SIMPLE_KEYBOARD_VOL_DOWN    0x02
#define SIMPLE_KEYBOARD_NEXT        0x10
#define SIMPLE_KEYBOARD_PREV        0x20
#define SIMPLE_KEYBOARD_PLAY        0x88
#define SIMPLE_KEYBOARD_PLAY_UP     0x66
#define SIMPLE_KEYBOARD_RELEASE     0x00
void btstack_hid_simple_keyboard(uint);

/*****************************************************************************
 * BLE API
 *****************************************************************************/
#define ble_adv_dis()                           bt_ctrl_msg(BT_CTL_BLE_ADV_DISABLE)
#define ble_adv_en()                            bt_ctrl_msg(BT_CTL_BLE_ADV_ENABLE)
void ble_set_adv(u8 chanel, u8 type);
void ble_set_adv_interval(u16 interval);
bool ble_set_adv_data(const u8 *adv_buf, u32 size);
bool ble_set_scan_rsp_data(const u8 *scan_rsp_buf, u32 size);
void ble_update_conn_param(u16 interval, u16 latency, u16 timeout);
u8 ble_get_status(void);
void ble_disconnect(void);
bool ble_is_connect(void);
u16 ble_get_gatt_mtu(void);
void ble_set_adv_interval(u16 interval);
void ble_disconnect(void);
void ble_send_kick(void);
int ble_tx_notify(u16 att_handle, u8* buf, u8 len);
uint8_t ble_set_delta_gain(void);
void ble_update_conn_param(u16 interval, u16 latency, u16 timeout);
bool ble_set_adv_data(const u8 *adv_buf, u32 size);
void ble_send_sm_req(void);
void ble_exchange_mtu_request(void);
void ble_set_gap_name(char *gap_name, u8 len);
u16 ble_get_conn_interval(void);                //N*1.25ms
u16 ble_get_conn_latency(void);
u16 ble_get_conn_timeout(void);                 //N*10ms
u16 ble_get_adv_interval(void);                 //N*625us

//init gatt
//profile_table: profile cache buf
//cb_info_table_p: call back info cache
void ble_gatts_init(uint8_t *profile_table, uint16_t profile_table_size,
                    ble_gatt_characteristic_cb_info_t **cb_info_table_p,
                    uint16_t gatt_max_att);
int ble_gatts_service_add(ble_gatts_service_type service_type, const uint8_t *service_uuid, ble_gatts_uuid_type uuid_type, uint8_t *service_handle);
int ble_gatts_characteristic_add(const uint8_t *att_uuid, ble_gatts_uuid_type uuid_type, uint16_t props,
                                 uint16_t *att_handle,
                                 ble_gatt_characteristic_cb_info_t *cb_info);
int ble_gatts_descriptor_add(const uint8_t *att_uuid, uint16_t props,
                             uint16_t *att_handle,
                             ble_gatt_characteristic_cb_info_t *cb_info);
bool ble_gatt_init_att_info(uint16_t att_handle, ble_gatt_characteristic_cb_info_t *att_cb_info);
int ble_gatts_inlcude_service_add(ble_gatts_uuid_type uuid_type, const uint8_t *service_uuid, uint16_t start_handle, uint16_t end_handle, uint16_t *att_handle);
int ble_gatts_add_profile_date(ble_gatts_profile_list_st *gatts_profile);
int ble_gatts_attribute_add(const uint8_t *att_data, uint16_t *att_handle);
bool ble_gatts_profile_mg_alloc_att_num_check(uint8_t att_num);

/**
 * @brief 检测BLE Notify的缓存是否满了
 * @param[in] rsvd_num 这个参数的意思是这样的，比如您设置1，如果蓝牙底层可以用来缓存的数量小于或等于1，就返回1表示满了。
 * 这个作用的目的是方便客户预留固定数量的force数量；举个栗子：有两个GATT，一个是Notify状态的，一个是Notfiy 音频数据的，由于音频数据大
 * 会有机会把协议栈填满导致没有buff给Notify状态使用了，所以就引出这个函数用来给音频发送前使用，设置为1则可以保证每次至少有1个buff给Notify那边
 * 而不至于阻塞；
 * return : 1表示设置数量满了，0没满
 **/
bool is_le_buff_full(uint rsvd_num);



/**
 * @brief ams复位，设备端控制手机端播放暂停等
 * opcode: 0播放，1暂停，2播放暂停，3下一曲，4上一去，5音量+，6音量-，详细参考AMS服务
*/
void ble_ams_remote_ctrl(uint8_t opcode);

/**
 * @brief 发起ancs连接(不可与ble_bt_connect交叉使用)
*/
void ble_ancs_start(void);

/**
 * @brief 断开ancs连接(不可与ble_bt_connect交叉使用)
*/
void ble_ancs_stop(void);

/**
 * @brief ANCS Pefrom Notifction Action
 * 控制手机端的操作，比如来电可以控制是否接听，拒接等
 * uid: 通信连接ID，可以通过ANCS回调获取，用于保证是在同一个事件上
 * opcode: 0 确认(接听)  1否定(拒接)
*/
void ble_ancs_remote_action(uint32_t uid, bool opcode);

bool hfp_hf_is_company_apple(void);                     ///判断当前连接设备是否为Apple


/*****************************************************************************
 * BLE无连接广播相关（通道0，与BLE连接相互独立）
 *****************************************************************************/
#define BLE_ADV0_EN_BIT             0x01
#define BLE_ADV0_MS_VAR_BIT         0x02
#define BLE_ADV0_ADDR_PUBIC_BIT          0x04

extern const uint8_t cfg_ble_adv0_en;

void ble_adv0_set_intv(u16 intv);                       //设置广播间隔，单位625us
void ble_adv0_set_ctrl(uint opcode);                    //0=关闭广播, 1=打开广播, 2=更新广播数据（打开时直接广播，关闭时仅更新buffer）
uint8_t ble_adv0_get_adv_en(void);

void ble_adv0_update_adv(void);
void ble_adv0_idx_update(void);

#endif //_API_BTSTACK_H
