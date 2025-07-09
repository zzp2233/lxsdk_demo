#ifndef _BSP_PWM_H
#define _BSP_PWM_H


typedef enum
{
    GPIO_PA0 = 0, //PWM0
    GPIO_PH3,
    GPIO_PB4,
    GPIO_PE0,
    GPIO_PF0,

    GPIO_PA1,     //PWM1
    GPIO_PA7,
    GPIO_PB5,
    GPIO_PE1,
    GPIO_PF1,

    GPIO_PF6,     //PWM2
    GPIO_PB0,
    GPIO_PB6,
    GPIO_PE2,
    GPIO_PF2,

    GPIO_PH0,     //PWM3
    GPIO_PB1,
    GPIO_PB7,
    GPIO_PE3,
    GPIO_PF3,

    GPIO_PH1,     //PWM4
    GPIO_PB2,
    GPIO_PE6,
    GPIO_PE4,
    GPIO_PF4,

    GPIO_PH2,     //PWM5
    GPIO_PB3,
    GPIO_PE7,
    GPIO_PE5,
    GPIO_PF5,

    GPIO_PWM0_MAX,
} pwm_gpio;

typedef struct _motor_t
{
    pwm_gpio gpio;
    u32 vibration_times;
    u8 vibration_cnt;
    u8 motor_sta;
    u32 interval;
    u8 duty;
    u16 time_cnt;
} motor_t;

/**
 * @brief 设置PWM0的占空比
 * @param[in] gpio   PWM0对应的GPIO选项
 * @param[in] duty   占空比，0-100%
 * @param[in] invert 输出是否反向
 *
 * @return  返回是否成功
 **/
bool bsp_pwm_duty_set(pwm_gpio gpio, u32 duty, bool invert);



/**
 * @brief 关闭pwmx
 * @param[in] gpio   PWMx对应的GPIO选项
 *
 * @return  返回是否成功
 **/
bool bsp_pwm_disable(pwm_gpio gpio);

/**
 * @brief 设置PWM0的占空比
 * @param[in] duty   占空比，0-100%
 * @param[in] vibrationtimes  震动时间,单位100ms
 * @param[in] interval  震动时间间隔,单位100ms
 * @param[in] cnt  震动次数
 * @return    void
 **/
void bsp_motor_set(pwm_gpio gpio, u32 duty, u32 vibrationtimes, u32 interval, u32 cnt);

/**
 * @brief 设置PWM的频率
          硬件PWM共用一个timer5，频率是一样的，只是占空比分开可调;
 * @param[in] freq 设置频率 Hz
 *
 * @return  返回是否成功
 **/
bool bsp_pwm_freq_set(u32 freq);
#endif

