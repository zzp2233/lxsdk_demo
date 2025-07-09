//该头文件由软件自动生成，请勿随意修改！
#ifndef _XCFG_H
#define _XCFG_H

#define XCFG_EN             1

typedef struct __attribute__((packed)) _xcfg_cb_t {
    u32 powkey_10s_reset                 : 1;   //POWKEY 10s复位系统
    u16 sys_sleep_time;                         //自动休眠时间: 不休眠: 0, 5秒钟后: 5, 10秒钟后: 10, 20秒钟后: 20, 30秒钟后: 30, 45秒钟后: 45, 1分钟后: 60, 2分钟后: 120, 3分钟后: 180, 4分钟后: 240, 5分钟后: 300, 6分钟后: 360, 7分钟后: 420, 8分钟后: 480, 9分钟后: 540, 10分钟后: 600, 15分钟后: 900, 20分钟后: 1200, 25分钟后: 1500, 30分钟后: 1800, 45分钟后: 2700, 1小时后: 3600
    u8 vol_max;                                 //音量级数: 0:16级音量, 1:32级音量
    u8 sys_init_vol;                            //开机默认音量
    u8 warning_volume;                          //提示音播放音量
    u8 osc_both_cap;                            //OSC基础电容: 0:0PF, 1:6PF
    u8 uosci_cap;                               //自定义OSCI电容
    u8 uosco_cap;                               //自定义OSCO电容
    u32 ft_osc_cap_en                    : 1;   //优先使用产测电容值
    u32 eq_dgb_spp_en                    : 1;   //EQ调试（蓝牙串口）
    u32 eq_dgb_uart_en                   : 1;   //EQ调试（UART）
    u32 eq_uart_sel                      : 2;   //EQ调试串口选择: VUSB: 0, PA1: 1, PA7: 2, PB3: 3
    u32 buck_mode_en                     : 1;   //BUCK MODE
    u32 vddbt_capless_en                 : 1;   //VDDBT省电容
    u32 vddio_sel                        : 4;   //VDDIO电压: 3.1V: 7, 3.2V: 8, 3.3V: 9, 3.4V: 10, 3.5V: 11, 3.6V: 12
    u32 vddbt_sel                        : 4;   //VDDBT电压: 1.35V: 10, 1.4V: 11, 1.45V: 12, 1.5V: 13, 1.55V: 14, 1.6V: 15
    u32 dac_sel                          : 2;   //DAC声道选择: 差分单声道: 0, 单端单声道: 1
    u32 dacaud_ldo_sel                   : 3;   //VDDDAC电压: 2.5V: 0, 2.6V: 1, 2.7V: 2, 2.8V: 3, 2.9V: 4, 3.0V: 5, 3.1V: 6, 3.2V: 7
    u32 dacaud_bypass_en                 : 1;   //省VDDDAC方案
    u8 dac_max_gain;                            //DAC最大音量: 0:0DB, 1:-1DB, 2:-2DB, 3:-3DB, 4:-4DB, 5:-5DB, 6:-6DB, 7:-7DB
    u8 bt_call_max_gain;                        //通话最大音量: 0:0DB, 1:-1DB, 2:-2DB, 3:-3DB, 4:-4DB, 5:-5DB, 6:-6DB, 7:-7DB
    u32 charge_en                        : 1;   //充电使能
    u32 charge_trick_en                  : 1;   //涓流充电使能
    u32 charge_dc_reset                  : 1;   //插入DC复位系统
    u32 charge_dc_not_pwron              : 1;   //插入DC禁止软开机
    u32 charge_stop_curr                 : 4;   //充电截止电流: 2.5mA: 1, 5mA: 2, 7.5mA: 3, 10mA: 4, 12.5mA: 5, 15mA: 6, 17.5mA: 7, 20mA: 8, 25mA: 10, 30mA: 12, 35mA: 14
    u32 charge_constant_curr             : 6;   //恒流充电电流: 15mA: 2, 20mA: 3, 25mA: 4, 30mA: 5, 35mA: 6, 40mA: 7, 45mA: 8, 50mA: 9, 55mA: 10, 60mA: 11, 65mA: 12, 70mA: 13, 75mA: 14, 80mA: 15, 90mA: 17, 100mA: 19, 110mA: 21, 120mA: 23, 130mA: 25, 140mA: 27, 150mA: 29, 160mA: 31, 170mA: 33, 180mA: 35, 190mA: 37, 200mA: 39, 210mA: 41, 220mA: 43, 230mA: 45, 240mA: 47, 250mA: 49, 260mA: 51, 270mA: 53, 280mA: 55, 290mA: 57, 300mA: 59, 310mA: 61, 320mA: 63
    u32 charge_trickle_curr              : 6;   //涓流充电电流: 5mA: 0, 10mA: 1, 20mA: 3, 25mA: 4, 30mA: 5, 35mA: 6, 40mA: 7
    char bt_name[32];                           //蓝牙名称
    u8 bt_addr[6];                              //蓝牙地址
    u32 bt_rf_txpwr_recon                : 3;   //降低回连TXPWR: 不降低: 0, 降低3dbm: 1, 降低6dbm: 2, 降低9dbm: 3
    u32 bt_a2dp_en                       : 1;   //音乐播放功能
    u32 bt_a2dp_vol_ctrl_en              : 1;   //音乐音量同步
    u32 bt_sco_en                        : 1;   //通话功能
    u32 bt_hfp_private_en                : 1;   //私密接听功能
    u32 bt_hfp_ring_number_en            : 1;   //来电报号功能
    u32 bt_hfp_inband_ring_en            : 1;   //来电播放手机铃声
    u32 bt_spp_en                        : 1;   //串口功能
    u32 ble_en                           : 1;   //BLE控制功能
    char le_name[32];                           //BLE名称
    u8 rf_pa_gain;                              //PA_GAIN0
    u8 rf_mix_gain;                             //MIX_GAIN0
    u8 rf_dig_gain;                             //DIG_GAIN0
    u8 rf_pa_cap;                               //GL_PA_CAP
    u8 rf_mix_cap;                              //GL_MIX_CAP
    u8 rf_txdbm;                                //GL_TX_DBM
    u32 mic_bias_method                  : 2;   //MIC偏置电路配置: None: 0, MIC省电阻电容: 2
    u32 bt_anl_gain                      : 4;   //MIC模拟增益: 0 (3DB): 0, 1 (6DB): 1, 2 (9DB): 2, 3 (12DB): 3, 4 (15DB): 4, 5 (18DB): 5, 6 (21DB): 6, 7 (24DB): 7, 8 (27DB): 8, 9 (30DB): 9, 10 (33DB): 10, 11 (36DB): 11, 12 (39DB): 12, 13 (42DB): 13, 14 (45DB): 14
    u8 bt_dig_gain;                             //MIC数字增益(0~32DB)
    u32 mic_post_gain                    : 4;   //MIC后置数字增益
    u32 bt_sco_nr_en                     : 1;   //近端降噪
    u32 bt_sco_nr_level                  : 5;   //近端降噪级别
    u32 bt_sco_nr_trumpet_en             : 1;   //近端降噪去喇叭声
    u32 bt_aec_en                        : 1;   //AEC功能
    u32 bt_echo_level                    : 4;   //AEC回声消除级别
    u8 bt_far_offset;                           //AEC远端补偿值
    u32 bt_alc_en                        : 1;   //ALC功能
    char serial_2[8];                           //序列号前两个字节
    u8 serial_num[6];                           //序列号
    u8 xm_keep_start[0];                        //For Keep Area Start
    u8 osci_cap;                                //产测OSCI电容
    u8 osco_cap;                                //产测OSCO电容
    u8 soft_key[20];                            //授权密钥
    u8 asr_soft_key[20];                        //语音识别授权密钥
    u8 xm_keep_end[0];                          //For Keep Area End
} xcfg_cb_t;

extern xcfg_cb_t xcfg_cb;
#endif
