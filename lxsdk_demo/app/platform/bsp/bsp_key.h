#ifndef _BSP_KEY_H
#define _BSP_KEY_H

typedef u8 size_msg_t;  // 定义消息类型为8位无符号整数，用于系统消息传递

//按键防抖参数定义
#define KEY_SCAN_TIMES          6           // 按键防抖的扫描次数：连续6次检测到按键才确认按下  
#define KEY_UP_TIMES            6           // 抬键防抖的扫描次数：连续6次检测到抬起才确认释放  
#define KEY_LONG_TIMES          150         // 长按键的次数：按住150个扫描周期判定为长按  
#define KEY_HOLD_TIMES          35          // 连按的频率次数：长按后每35个周期触发一次连续事件  
#define KEY_LONG_HOLD_TIMES     (KEY_LONG_TIMES + KEY_HOLD_TIMES)  // 长按保持总时间

//按键事件类型定义
#define KEY_SHORT               0x00        // 短按事件  
#define KEY_SHORT_UP            0x10        // 短按抬起事件  
#define KEY_LONG                0x20        // 长按事件  
#define KEY_LONG_UP             0x30        // 长按抬起事件  
#define KEY_HOLD                0x40        // 保持按下事件（长按后的连续触发）  
#define KEY_LHOLD               0x50        // 长时间保持事件  
#define KEY_DOUBLE              0x60        // 双击事件  
#define KEY_THREE               0x70        // 三击事件  
#define KEY_SHORT_UP_DELAY      0x80        // 短按延时抬起事件  
#define KEY_TYPE_MASK           0xF0        // 按键类型掩码（高4位）  
#define KEY_USAGE_MASK          0x0F        // 按键用途掩码（低4位）

//触摸屏消息定义，CTP Message, 范围: 0xA0 ~ 0xAF  
#define MSG_CTP_TOUCH           0xA0        // 触摸接触事件  
#define MSG_CTP_CLICK           0xA1        // 单击事件  
#define MSG_CTP_SHORT_LEFT      0xA2        // 短划左滑事件  
#define MSG_CTP_SHORT_UP        0xA3        // 短划上滑事件  
#define MSG_CTP_SHORT_RIGHT     0xA4        // 短划右滑事件  
#define MSG_CTP_SHORT_DOWN      0xA5        // 短划下滑事件  
#define MSG_CTP_LONG            0xA6        // 长划事件  
#define MSG_CTP_LONG_LEFT       0xA7        // 长划左滑事件  
#define MSG_CTP_LONG_UP         0xA8        // 长划上滑事件  
#define MSG_CTP_LONG_RIGHT      0xA9        // 长划右滑事件  
#define MSG_CTP_LONG_DOWN       0xAA        // 长划下滑事件  
#define MSG_CTP_LONG_LIFT       0xAB        // 长按抬起事件  
#define MSG_CTP_COVER           0xAC        // 覆盖事件（可能是手掌误触检测）  
  
#define MSG_CHECK_LANGUAGE      0xAD        // 语言切换检查事件  
#define MSG_REBUILD_FUNC        0xAE        // 重建界面功能事件

//编码器（旋转编码器，如表冠）  
#define MSG_QDEC_FORWARD        0xBF        // 编码器向前转动  
#define MSG_QDEC_BACKWARD       0xBE        // 编码器向后转动

//系统事件消息定义，Event Message, 范围：0xC0 ~ 0xFF
#define MSG_SYS_1S              0xff        // 系统1秒定时事件  
#define MSG_SYS_500MS           0xfe        // 系统500毫秒定时事件  
#define EVT_SD_INSERT           0xfd        // SD卡插入事件  
#define EVT_SD_REMOVE           0xfc        // SD卡移除事件  
#define EVT_SD1_INSERT          0xfb        // SD卡1插入事件  
#define EVT_SD1_REMOVE          0xfa        // SD卡1移除事件  
#define EVT_UDISK_INSERT        0xf9        // U盘插入事件  
#define EVT_UDISK_REMOVE        0xf8        // U盘移除事件  
#define EVT_PC_INSERT           0xf7        // PC连接插入事件  
#define EVT_PC_REMOVE           0xf6        // PC连接移除事件  
#define EVT_HFP_SET_VOL         0xf5        // HFP（免提协议）音量设置事件  
#define EVT_A2DP_SET_VOL        0xf4        // A2DP（高级音频分发协议）音量设置事件

//蓝牙音频相关事件
#define EVT_A2DP_MUSIC_PLAY     0xf3        // A2DP音乐播放事件：蓝牙音乐开始播放  
#define EVT_A2DP_MUSIC_STOP     0xf2        // A2DP音乐停止事件：蓝牙音乐停止播放  
#define EVT_BT_SCAN_START       0xf1        // 蓝牙扫描开始事件：开始搜索蓝牙设备  
#define EVT_ONLINE_SET_EQ       0xf0        // 在线EQ设置事件：通过网络设置音效均衡器

//音频控制相关事件 
#define EVT_KEY_2_UNMUTE        0xef        // 按键2解除静音事件：取消静音状态  
#define EVT_BT_SET_LANG_ID      0xee        // 蓝牙设置语言ID事件：切换语言设置  
#define EVT_BT_SET_EQ           0xed        // 蓝牙设置EQ事件：通过蓝牙调整音效  
#define EVT_ECHO_LEVEL          0xec        // 回声等级事件：调整通话回声消除级别  
#define EVT_MIC_VOL             0xeb        // 麦克风音量事件：调整麦克风音量  
#define EVT_MUSIC_VOL           0xea        // 音乐音量事件：调整音乐播放音量  
#define EVT_UDE_SET_VOL         0xe9        // UDE音量设置事件：特定音频设备音量调节  
#define EVT_CALL_NUMBER_UPDATE  0xe8        // 通话号码更新事件：来电号码信息更新

//蓝牙和媒体信息事件
#define EVT_READY_EXCHANGE_MTU  0xe7        // 准备交换MTU事件：蓝牙连接参数协商  
  
#define EVT_BT_CONNECT_ONCE     0xe5        // 蓝牙单次连接事件：执行一次蓝牙连接尝试  
#define EVT_ID3_TITLE_UPDATE    0xe4        // ID3标题更新事件：音乐文件标题信息更新  
#define EVT_ID3_ARTIST_UPDATE   0xe3        // ID3艺术家更新事件：音乐文件艺术家信息更新  
#define EVT_HALT                0xe2        // 系统停止事件：系统异常或故障停止  
#define EVT_4G_INIT_DONE        0xe1        // 4G初始化完成事件：4G模块初始化成功  
#define EVT_JPEG_DECODE         0xe0        // JPEG解码事件：图片解码处理

//调试和传感器事件
#if BT_SCO_APP_DBG_EN                       // 蓝牙SCO调试功能启用时  
#define EVT_SCO_DBG_COMM_RSP    0xdf        // SCO调试通信响应事件：调试命令响应  
#define EVT_SCO_DBG_TLV_RSP     0xde        // SCO调试TLV响应事件：TLV格式调试响应  
#define EVT_SCO_DBG_NOTIFY      0xdd        // SCO调试通知事件：调试状态通知  
#endif // BT_SCO_APP_DBG_EN  
#define EVT_VC30FX_ISR          0xdc        // VC30FX中断服务事件：心率传感器中断处理

//界面控制事件
#define EVT_CLOCK_DROPDOWN_EXIT 0xc0        // 时钟下拉菜单退出事件：关闭下拉菜单  
#define EVT_WATCH_SET_COVER     0xc1        // 手表设置封面事件：设置表盘封面  
#define EVT_MSGBOX_EXIT         0xc2        // 消息框退出事件：关闭消息对话框  
#define EVT_WATCH_MSG_POP_UP    0xc3        // 手表消息弹出事件：显示消息通知  
#define EVT_WATCH_TIMER_DONE    0xc4        // 手表计时器完成事件：定时器到时提醒  
#define EVT_CLOCK_SUB_SIDE_EXIT 0xc5        // 时钟侧边栏退出事件：关闭侧边菜单

//电子烟控制事件
#define EVT_ECIG_SMOKE_REMINDER 0xd7        // 电子烟吸烟提醒事件：吸烟状态提醒

//充电盒和耳机管理事件
#define MSG_SYS_BOX_INTER           0xd5    // 系统充电盒交互事件：充电盒状态交互  
#define MSG_SYS_BOX_CHARGE          0xd4    // 系统充电盒充电事件：充电盒充电状态  
#define MSG_SYS_BOX_500MS           0xd3    // 系统充电盒500毫秒事件：充电盒定时事件  
#define EVT_EARPHONE_MUSIC_UPDATE   0xd2    // 耳机音乐更新事件：耳机播放状态更新  
#define EVT_BOX_BAT_UPDATE          0xd1    // 充电盒电量更新事件：充电盒电量变化  
#define EVT_MSG_INOUT_BOX           0xd0    // 消息进出充电盒事件：充电盒通信消息



//普通按键定义，可以支持短按，长按，双击等。范围：0 ~ 0xf
#define NO_KEY                  0x00
#define KEY_1                   0x01
#define KEY_2                   0x02
#define KEY_3                   0x03
#define KEY_4                   0x04
#define KEY_5                   0x05
#define KEY_6                   0x06
#define KEY_7                   0x07
#define KEY_8                   0x08

#define KEY_BACK                KEY_1       // 返回键（映射到按键1）  
#define KEY_LEFT                KEY_2       // 左键（映射到按键2）  
#define KEY_RIGHT               KEY_3       // 右键（映射到按键3）  
#define KEY_VOL_UP              KEY_4       // 音量加键（映射到按键4）  
#define KEY_VOL_DOWN            KEY_5       // 音量减键（映射到按键5）  
#define KEY_NEXT                KEY_6       // 下一个键（映射到按键6）  
#define KEY_PREV                KEY_7       // 上一个键（映射到按键7）  
#define KEY_MODE                KEY_8       // 模式键（映射到按键8）

#define K_BACK                  (KEY_BACK | KEY_SHORT)          // 返回键短按  
#define KU_BACK                 (KEY_BACK | KEY_SHORT_UP)       // 返回键短按抬起  
#define KU_DELAY_BACK           (KEY_BACK | KEY_SHORT_UP_DELAY) // 返回键延迟抬起  
#define KL_BACK                 (KEY_BACK | KEY_LONG)           // 返回键长按  
#define KLU_BACK                (KEY_BACK | KEY_LONG_UP)        // 返回键长按抬起  
#define KH_BACK                 (KEY_BACK | KEY_HOLD)           // 返回键保持按下  
#define KD_BACK                 (KEY_BACK | KEY_DOUBLE)         // 返回键双击  
#define KTH_BACK                (KEY_BACK | KEY_THREE)          // 返回键三击  
#define KLH_BACK                (KEY_BACK | KEY_LHOLD)          // 返回键长时间保持

#define K_LEFT                  (KEY_LEFT | KEY_SHORT)
#define KU_LEFT                 (KEY_LEFT | KEY_SHORT_UP)
#define KU_DELAY_LEFT           (KEY_LEFT | KEY_SHORT_UP_DELAY)
#define KL_LEFT                 (KEY_LEFT | KEY_LONG)
#define KLU_LEFT                (KEY_LEFT | KEY_LONG_UP)
#define KH_LEFT                 (KEY_LEFT | KEY_HOLD)
#define KD_LEFT                 (KEY_LEFT | KEY_DOUBLE)
#define KLH_LEFT                (KEY_LEFT | KEY_LHOLD)

#define K_RIGHT                 (KEY_RIGHT | KEY_SHORT)
#define KU_RIGHT                (KEY_RIGHT | KEY_SHORT_UP)
#define KU_DELAY_RIGHT          (KEY_RIGHT | KEY_SHORT_UP_DELAY)
#define KL_RIGHT                (KEY_RIGHT | KEY_LONG)
#define KLU_RIGHT               (KEY_RIGHT | KEY_LONG_UP)
#define KH_RIGHT                (KEY_RIGHT | KEY_HOLD)
#define KD_RIGHT                (KEY_RIGHT | KEY_DOUBLE)
#define KTH_RIGHT               (KEY_RIGHT | KEY_THREE)
#define KLH_RIGHT               (KEY_RIGHT | KEY_LHOLD)

#define K_VOL_UP                (KEY_VOL_UP | KEY_SHORT)
#define KU_VOL_UP               (KEY_VOL_UP | KEY_SHORT_UP)
#define KL_VOL_UP               (KEY_VOL_UP | KEY_LONG)
#define KLU_VOL_UP              (KEY_VOL_UP | KEY_LONG_UP)
#define KH_VOL_UP               (KEY_VOL_UP | KEY_HOLD)
#define KD_VOL_UP               (KEY_VOL_UP | KEY_DOUBLE)

#define K_VOL_DOWN              (KEY_VOL_DOWN | KEY_SHORT)
#define KU_VOL_DOWN             (KEY_VOL_DOWN | KEY_SHORT_UP)
#define KL_VOL_DOWN             (KEY_VOL_DOWN | KEY_LONG)
#define KLU_VOL_DOWN            (KEY_VOL_DOWN | KEY_LONG_UP)
#define KH_VOL_DOWN             (KEY_VOL_DOWN | KEY_HOLD)
#define KD_VOL_DOWN             (KEY_VOL_DOWN | KEY_DOUBLE)

#define K_MODE                  (KEY_MODE | KEY_SHORT)
#define KU_MODE                 (KEY_MODE | KEY_SHORT_UP)
#define KL_MODE                 (KEY_MODE | KEY_LONG)
#define KLU_MODE                (KEY_MODE | KEY_LONG_UP)
#define KH_MODE                 (KEY_MODE | KEY_HOLD)
#define KD_MODE                 (KEY_MODE | KEY_DOUBLE)


//按键消抖参数
typedef struct  
{  
    u16 scan_cnt;       // 扫描计数：按键扫描的次数阈值，用于确认按键按下  
    u16 up_cnt;         // 抬起计数：按键抬起的次数阈值，用于确认按键释放  
    u16 long_cnt;       // 长按计数：判定为长按所需的计数值  
    u16 hold_cnt;       // 保持计数：判定为保持按下所需的计数值  
} key_shake_tbl_t;

typedef struct  
{  
    u8 adc_val;         // ADC采样值：通过电阻分压得到的ADC值  
    u8 usage_id;        // 用途ID：对应的按键功能ID  
} adkey_tbl_t;

extern const adkey_tbl_t adkey_table[];        // ADC按键映射表  
extern const adkey_tbl_t adkey2_table[];       // 第二组ADC按键映射表  
extern const adkey_tbl_t pwrkey_table[6];      // 电源键映射表  
extern const u8 key_config_table[];           // 按键配置表  
  
void io_key_init(void);                       // IO按键初始化  
u8 get_iokey(void);                           // 获取IO按键状态  
void key_var_init(void);                      // 按键变量初始化  
void key_init(void);                          // 按键系统初始化  
u8 bsp_key_scan(void);                        // 按键扫描函数  
u8 get_double_key_time(void);                 // 获取双击时间间隔


#endif // _BSP_KEY_H
