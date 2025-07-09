#ifndef __CHARGE_IC_H__
#define __CHARGE_IC_H__

u8 charge_ic_work_mode(void);
bool charge_ic_eoc_check_en(void);
bool charge_ic_eoc_check_inbox(void);

/**
* @brief charge_ic充电使能开关
* @param[in] en:开关选择
**/
void charge_ic_charge(u8 en);

/**
* @brief 获取comm模式开始时间
**/
u32 charge_ic_comm_mode_tick(void);

/**
* @brief 设置7812c工作模式
**/
bool charge_ic_work_mode_set(uint8_t mode);

void charge_ic_process(void);
void charge_ic_init(void);

#endif /* __CHARGE_IC_H__*/
