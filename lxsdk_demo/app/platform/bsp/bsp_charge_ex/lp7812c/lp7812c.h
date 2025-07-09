#ifndef __LP7812C_H__
#define __LP7812C_H__

typedef union
{
    uint16_t byte;
    struct
    {
        uint16_t B0 : 1;
        uint16_t B1 : 1;
        uint16_t B2 : 1;
        uint16_t B3 : 1;
        uint16_t B4 : 1;
        uint16_t B5 : 1;
        uint16_t B6 : 1;
        uint16_t B7 : 1;
        uint16_t B8 : 1;
        uint16_t B9 : 1;
        uint16_t B10 : 1;
        uint16_t B11 : 1;
        uint16_t B12 : 1;
        uint16_t B13 : 1;
        uint16_t B14 : 1;
    } fields;
} BIT16_Typedef;

uint8_t CRC5_Cal(uint32_t source);

u8 lp7812c_work_mode(void);
bool lp7812c_eoc_check_en(void);
bool lp7812c_eoc_check_inbox(void);

/**
* @brief lp7812c充电使能开关
* @param[in] en:开关选择
**/
void lp7812c_charge(u8 en);

/**
* @brief 获取comm模式开始时间
**/
u32 lp7812c_comm_mode_tick(void);

/**
* @brief 设置7812c工作模式
**/
bool lp7812c_work_mode_set(uint8_t mode);

void lp7812c_process(void);
void lp7812c_init(void);
void lp7812c_init_exit(void);
#endif /* __LP7812C_H__*/
