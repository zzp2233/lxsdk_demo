#ifndef __BSP_CHARGE_VHOUSE_HOST_CMD_H__
#define __BSP_CHARGE_VHOUSE_HOST_CMD_H__

#define CMD_TX_PR                   50          //命令最小发送间隔
#define CMD_TIMEOUT_TICK            100         //命令超时时间(ms)
#define BREATH_TX_PR                500         //心跳包发送间隔
#define BREATH_TIMEOUT_CNT          3           //心跳包超时阈值次数

enum
{
    CMD_FLOW_NULL = 0,
    CMD_FLOW_OPEN_BOX,
    CMD_FLOW_CLOSE_BOX,
};

/**
 * @brief 获取队列指令大小
 * @return 队列指令大小
 **/
int vhouse_queue_size(void);

/**
 * @brief 命令码超时记录清除
 **/
void bsp_smart_vhouse_cmd_timeout_clr(void);

/**
 * @brief 命令码队列重置
 **/
void bsp_smart_vhouse_cmd_reset(void);

/**
 * @brief 命令码有回复
 **/
void bsp_smart_vhouse_cmd_ack(u8 channel);

/**
 * @brief 命令码立即发送
 * @param[in] cmd: 发送码
 * @param[in] channel: 发送给的耳机通道
 * @param[in] send_buf: 数据缓冲区
 * @param[in] len: 数据缓冲区长度
 **/
void bsp_smart_vhouse_cmd_sned(u8 cmd, u8 channel, u8 *send_buf, u8 len);

/**
 * @brief 命令码进入发送队列, 可带buf
 * @param[in] cmd: 发送码
 * @param[in] channel: 发送给的耳机通道
 * @param[in] buf: 数据缓冲区
 * @param[in] len: 数据缓冲区长度
 **/
u8 bsp_smart_vhouse_cmd_buf_enqueue(u8 cmd, u8 channel, u8 *buf, u8 len);

/**
 * @brief 命令码进入发送队列, 不重复入队
 * @param[in] cmd: 发送码
 * @param[in] channel: 发送给的耳机通道
 **/
bool bsp_smart_vhouse_cmd_enqueue_no_repeat(u8 cmd, u8 channel);

/**
 * @brief 命令码进入发送队列
 * @param[in] cmd: 发送码
 * @param[in] channel: 发送给的耳机通道
 **/
void bsp_smart_vhouse_cmd_enqueue(u8 cmd, u8 channel);

/**
 * @brief 充电时查询耳机电量
 **/
void bsp_smart_vhouse_charge_vbat_get(void);

/**
 * @brief 处理是否发送命令码队列事务
 **/
void bsp_smart_vhouse_queue_process(void);

void bsp_smart_vhouse_host_cmd_process(void);
void bsp_smart_vhouse_host_cmd_init(void);

u8 chargebox_ota_smart_vhouse_send(u8 *buf, u8 len);

#endif  //__BSP_CHARGE_VHOUSE_HOST_CMD_H__
