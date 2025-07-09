#ifndef __BSP_EARPHONE_MANAGE_H__
#define __BSP_EARPHONE_MANAGE_H__

#define EARPHONE_TWS_ADDR_LEN           6   //耳机tws地址长度
#define EARPHONE_VBAT_STEP              1   //耳机电量变化步进值

#define bsp_earphone_is_inbox(x)        (earphone.inbox[x])                                                                     //耳机是否插入
#define bsp_earphone_with_outbox()      ((earphone.inbox[EARPHONE_LEFT] == 0) || (earphone.inbox[EARPHONE_RIGHT] == 0))         //是否有耳机拔出/在仓外
#define bsp_earphone_is_charge(x)       (bsp_hall_close_box() && earphone.inbox[x] && (sys_cb.bat_low == false))                //左/右耳机是否在充电
#define bsp_earphone_all_eoc()          (bsp_earphone_vbat_is_full(EARPHONE_LEFT) && bsp_earphone_vbat_is_full(EARPHONE_RIGHT)) //耳机全满电

#define bsp_earphone_link_key_paired()  (earphone.link_key_sum[EARPHONE_RIGHT] == earphone.link_key_sum[EARPHONE_RIGHT])    //耳机配对信息一致
#define bsp_earphone_is_paired()        (earphone.paired == true)                                                           //仓耳是否配对完成

#define bsp_earphone_vbat_get_real(x)   (earphone.vbat[x] & 0x7f)                                                           //获取耳机电池电量（真实上报值）
#define bsp_earphone_vbat_is_full(x)    (earphone.charge_sta[x] == EARPHONE_FULL_CHARGE)                                    //耳机电池满电
#define bsp_earphone_vbat_var_clr(x)    {earphone.charge_sta[x] = EARPHONE_NO_CHARGE;}                                      //耳机电池状态清除
#define bsp_earphone_vbat_inited()      ((earphone.vbat[EARPHONE_LEFT] != 0xff) && (earphone.vbat[EARPHONE_RIGHT] != 0xff)) //耳机电池电量是否初始化

//耳机通知
enum
{
    EARPHONE_NOTICE_LEFT_OUTBOX,                     //左耳拔出
    EARPHONE_NOTICE_LEFT_INBOX,                      //左耳插入
    EARPHONE_NOTICE_RIGHT_OUTBOX,                    //右耳拔出
    EARPHONE_NOTICE_RIGHT_INBOX,                     //右耳插入
};

//耳机通道
enum
{
    EARPHONE_LEFT,
    EARPHONE_RIGHT,
    EARPHONE_MAX,
    EARPHONE_INDEX_ERR,
};

//耳机地址类型
enum
{
    EARPHONE_ADDR_NULL = 0,
    EARPHONE_ADDR_LEFT,
    EARPHONE_ADDR_RIGHT,
    EARPHONE_ADDR_TWS,
    EARPHONE_ADDR_MAX,
};

//耳机充电类型
enum
{
    EARPHONE_NO_CHARGE = 0,             //未充电
    EARPHONE_CHARGING,                  //充电中
    EARPHONE_FULL_CHARGE,               //充满, 充电截止
    EARPHONE_CHARGE_MAX,
};

typedef struct
{
    bool inbox[EARPHONE_MAX];           //耳机插入状态
    bool paired;                        //耳机完成配对标志

    u8 vbat[EARPHONE_MAX];              //耳机电量, BIT(7) 是否有新更新, BTT(0)~BIT(6) 电量;
    u8 charge_sta[EARPHONE_MAX];        //耳机充电状态, 0:充电关闭， 1：充电开启， 2：充满;

    u8 bt_addr[EARPHONE_MAX][6];        //耳机本地蓝牙地址
    u8 tws_addr[EARPHONE_TWS_ADDR_LEN]; //耳机tws地址
    u8 tws_addr_flag;                   //耳机tws地址标志, 0:未连接过 1:已连接过
    u16 link_key_sum[EARPHONE_MAX];     //配对信息校验和
    u8 play_sta;                        //播放状态, 0:pause; 1:play;
    u8 play_sta_raw;                    //过滤前的播放状态
    u32 play_change_tick;
    u32 play_change_delay_tick;

} earphone_t;
extern earphone_t earphone;

/**
 * @brief 是否有耳机充电
 **/
bool bsp_earphone_with_charge(void);

/**
 * @brief 查询耳机电量
 * @param[in] side : 左耳/右耳
 **/
u8 bsp_earphone_vbat_get(u8 side);

/**
 * @brief 是否有耳机在仓内
 **/
bool bsp_earphone_with_inbox(void);

/**
 * @brief 是否所有耳机在仓内
 **/
bool bsp_earphone_all_inbox(void);

/**
 * @brief 耳机拔插更新
 * @param[in] index : 耳机索引序号
 * @param[in] inbox : 是否插入
 **/
void bsp_earphone_inbox(u8 index, bool inbox);

/**
 * @brief 智能仓串口通道转耳机索引
 **/
u8 bsp_earphone_get_index_bych(u8 channel);

/**
 * @brief 耳机电量更新
 * @param[in] index : 耳机索引序号
 * @param[in] vbat : 耳机电量; bit7:是否正在充电;
 **/
void bsp_earphone_set_vbat(u8 index, u8 vbat);

/**
 * @brief 耳机通过ble更新电量
 * @param[in] index : 耳机索引序号
 * @param[in] vbat : 耳机电量; bit7:是否正在充电;
 **/
void bsp_earphone_ble_vbat(u8 index, u8 vbat);

/**
 * @brief 耳机播放状态更新
 * @param[in] play : 播放状态
 **/
void bsp_earphone_play_set(u8 play);

/**
 * @brief 耳机ble地址更新
 **/
void bsp_earphone_save_addr(u8 *buf, u8 len);

/**
 * @brief 耳机ble配对完成
 **/
void bsp_earphone_pair_finish(void);

/**
 * @brief 清除仓耳配对记录
 **/
void bsp_earphone_pair_clr(void);

/**
 * @brief 耳机数据管理器进程
 **/
void bsp_earphone_manage_process(void);

/**
 * @brief 耳机数据管理器初始化
 **/
void bsp_earphone_manage_init(void);

#endif
