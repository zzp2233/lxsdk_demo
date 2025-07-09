#ifndef __BSP_CHARGE_EX_HAL_H__
#define __BSP_CHARGE_EX_HAL_H__

#endif // __BSP_CHARGE_EX_HAL_H__

/**
 * @brief 获取充电状态
 * @return 是否充电
 **/
bool bsp_charge_ex_is_charge(void);

/**
 * @brief 是否正在唤醒耳机
 **/
bool bsp_charge_ex_is_waking_up(void);

/**
 * @brief 获取满电检测状态
 * @return 是否正在满电检测
 **/
bool bsp_charge_ex_eoc_check_en(void);

/**
 * @brief 获取满电检测用途
 * @return 是否使用eoc检测耳机是否在仓
 **/
bool bsp_charge_ex_eoc_check_inbox(void);

/**
 * @brief 外部充电IC是否允许通讯
 **/
bool bsp_charge_ex_comm_allow(void);

/**
 * @brief 获取外部充电ic工作模式
 * @return 工作模式
 **/
u8 bsp_charge_ex_mode(void);

/**
 * @brief 设置外部充电ic工作模式
 * @param[in] mode: 工作模式
 * @return 是否设置成功
 **/
bool bsp_charge_ex_hal_mode_set(u8 mode);

/**
 * @brief 控制外部充电ic充电开关
 * @param[in] en: 充电开关
 **/
void bsp_charge_ex_charge(u8 en);

/**
 * @brief 外部充电IC进程
 **/
void bsp_charge_ex_hal_process(void);

/**
 * @brief 外部充电IC初始化
 **/
void bsp_charge_ex_ic_init(void);
