#ifndef __BSP_HALL_H__
#define __BSP_HALL_H__

#if HALL_EN
#define bsp_hall_close_box()    (hall_cb.status == 0)
#define bsp_hall_open_box()     (hall_cb.status == 1)
#else
#define bsp_hall_close_box()    1
#define bsp_hall_open_box()     1
#endif // HALL_EN

#if HALL_EN
enum
{
    HALL_NO_ACTION,
    HALL_OPEN_BOX,
    HALL_CLOSE_BOX,
};
#else
enum
{
    BOX_UART_COMM_IDLE,
    BOX_UART_COMM_START,
    BOX_UART_COMM_FINISH,
    BOX_UART_COMM_TIMEOUT,
};
#endif

typedef struct
{
    u8 status;                  //0:关盖; 1:开盖;
    u32 charge_tick;            //充电定时
    u32 charge_start_tick;      //开始充电计时，用于7812 eoc检测在仓功能
#if !HALL_EN
    u8  noear_inbox_flg;
#endif
} hall_cb_t;
extern hall_cb_t hall_cb;

/**
 * @brief 开盖时执行的用户操作
 **/
void bsp_hall_open_action(void);

/**
 * @brief 开盖执行回调
 **/
void bsp_hall_open_callback(void);

/**
 * @brief 关盖时执行的用户操作
 **/
void bsp_hall_close_action(void);

/**
 * @brief 关盖执行回调
 **/
void bsp_hall_close_callback(void);

/**
 * @brief 霍尔模块初始化
 **/
void bsp_hall_init(void);

/**
 * @brief 霍尔开关盖检测
 * return hall_action枚举
 **/
u8 bsp_hall_action_check(void);

/**
 * @brief 霍尔模块进程
 **/
void bsp_hall_process(void);

//无hall充电仓方案
void bsp_no_hall_idle_enter_callback(void);
void bsp_no_hall_idle_exit_callback(void);
void bsp_no_hall_earphone_outbox_callback(u8 index);
void bsp_no_hall_earphone_inbox_callback(u8 index);
void bsp_no_hall_process(void);
#if !HALL_EN
void bsp_no_hall_pair_action(void);
/**
 * @brief 唤醒耳机后执行的操作
 **/
void bsp_no_hall_wakeup_action(void);
void bsp_no_hall_init(void);
void bsp_box_set_comm_timeout_flg(u8 flg);
#endif

#endif // __BSP_HALL_H__
