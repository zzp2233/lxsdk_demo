#include "include.h"


/*        G1    G2      G3      G4      G5

  PWM0:   PA0   PH3     PB4     PE0     PF0
  PWM1:   PA1   PA7     PB5     PE1     PF1
  PWM2:   PF6   PB0     PB6     PE2     PF2
  PWM3:   PH0   PB1     PB7     PE3     PF3
  PWM4:   PH1   PB2     PE6     PE4     PF4
  PWM5:   PH2   PB3     PE7     PE5     PF5
*/
static const int pwm_gpio_reg[GPIO_PWM0_MAX] =
{
    IO_PA0, IO_PH3, IO_PB4, IO_PE0, IO_PF0,
    IO_PA1, IO_PA7, IO_PB5, IO_PE1, IO_PF1,
    IO_PF6, IO_PB0, IO_PB6, IO_PE2, IO_PF2,
    IO_PH0, IO_PB1, IO_PB7, IO_PE3, IO_PF3,
    IO_PH1, IO_PB2, IO_PE6, IO_PE4, IO_PF4,
    IO_PH2, IO_PB3, IO_PE7, IO_PE5, IO_PF5,
};

static bool pwm_freq_is_init = 0;

static co_timer_t motor_timer;
static motor_t motor_st;

/**
 * @brief 设置PWM的频率
          硬件PWM共用一个timer5，频率是一样的，只是占空比分开可调;
 * @param[in] freq 设置频率 Hz
 *
 * @return  返回是否成功
 **/
bool bsp_pwm_freq_set(u32 freq)
{
    TMR5CON  = 0;
    TMR5PR   = 2000000/freq;
    TMR5CPND = BIT(9);
    TMR5CNT  = 0;
    TMR5CON  = (4<<1)  | BIT(0);  // RC2M

    pwm_freq_is_init = 1;

    return true;
}


/**
 * @brief 设置PWM0的占空比
 * @param[in] gpio   PWM0对应的GPIO选项
 * @param[in] duty   占空比，0-100%
 * @param[in] invert 输出是否反向
 *
 * @return  返回是否成功
 **/
bool bsp_pwm_duty_set(pwm_gpio gpio, u32 duty, bool invert)
{
    gpio_t gpio_reg;
    int group_num, group_id;
    volatile uint32_t *duty_reg;

    if(gpio >= GPIO_PWM0_MAX)
    {
        return false;
    }

    if(duty > 100)
    {
        duty = 100;
    }

    if(pwm_freq_is_init == 0)
    {
        bsp_pwm_freq_set(20000);            //如果没初始化频率，默认设置为20K
    }

    //IO Init
    bsp_gpio_cfg_init(&gpio_reg, pwm_gpio_reg[gpio]);
    gpio_reg.sfr[GPIOxDE] |= BIT(gpio_reg.num);
    gpio_reg.sfr[GPIOxPU] &= ~BIT(gpio_reg.num);
    gpio_reg.sfr[GPIOxDIR] &= ~ BIT(gpio_reg.num);

    group_num = gpio/5;
    group_id  = gpio - group_num*5;

    TMR5CON |= BIT(16 + group_num*2);
    TMR5CON |= invert << (17 + group_num*2);
    duty_reg  = (void *)&TMR5DUTY0 + group_num*4;
    *duty_reg = TMR5PR*duty/100;
    FUNCMCON1 = (FUNCMCON1 & (~(0xf << (8 + group_num*4)))) | ((group_id+1) << (8 + group_num*4));

    return true;
}

/**
 * @brief 关闭pwmx
 * @param[in] gpio   PWMx对应的GPIO选项
 *
 * @return  返回是否成功
 **/
bool bsp_pwm_disable(pwm_gpio gpio)
{
    gpio_t gpio_reg;
    int pwm_num;

    if(gpio >= GPIO_PWM0_MAX)
    {
        return false;
    }

    //IO deinit
    bsp_gpio_cfg_init(&gpio_reg, pwm_gpio_reg[gpio]);
    gpio_reg.sfr[GPIOxDE] &= ~BIT(gpio_reg.num);
    gpio_reg.sfr[GPIOxDIR] |= BIT(gpio_reg.num);

    //TMR5CON deinit
    pwm_num = gpio / 5;
    TMR5CON &= ~BIT(16 + pwm_num * 2);
    FUNCMCON1 = (FUNCMCON1 & (~(0xf << (8 + pwm_num * 4))));

    pwm_freq_is_init = false;

    return true;
}

//co_timer_callback_t bsp_motor_time_callback(co_timer_t *timer, void *param)
//{
//    static u32 time_cnt;
//    time_cnt++;
//    //震动时间
//    if(motor_st.vibration_times == time_cnt && motor_st.motor_sta == true){
//        bsp_pwm_disable(motor_st.gpio);
//        motor_st.vibration_cnt--;
//        time_cnt = 0;
//        motor_st.motor_sta  = false;
//    }
//    if (motor_st.interval == time_cnt && motor_st.motor_sta == false) {
//        bsp_pwm_duty_set(motor_st.gpio, motor_st.duty, false);
//        time_cnt = 0;
//        motor_st.motor_sta  = true;
//    }
//    if(motor_st.vibration_cnt == 0){
//        bsp_pwm_disable(motor_st.gpio);
//        co_timer_del(&motor_timer);
//    }
//    return 0;
//}

co_timer_callback_t bsp_motor_time_callback(co_timer_t *timer, void *param)
{
    motor_st.time_cnt++;
    if(motor_st.motor_sta == true && motor_st.time_cnt >= motor_st.vibration_times)
    {
        if (motor_st.interval > 0)
        {
            bsp_pwm_disable(motor_st.gpio);
            motor_st.motor_sta = false;
        }
        else
        {
            motor_st.vibration_cnt--;
        }
        motor_st.time_cnt = 0;
    }
    if (motor_st.motor_sta == false && motor_st.time_cnt >= motor_st.interval)
    {
        motor_st.vibration_cnt--;
        if (motor_st.vibration_cnt)
        {
            bsp_pwm_duty_set(motor_st.gpio, motor_st.duty, false);
            motor_st.motor_sta = true;
        }
        motor_st.time_cnt = 0;
    }
    if(motor_st.vibration_cnt == 0)
    {
        bsp_pwm_disable(motor_st.gpio);
        co_timer_del(&motor_timer);
    }
    return 0;
}

/**
 * @brief 设置PWM0的占空比
 * @param[in] gpio   PWM IO
 * @param[in] duty   占空比，0-100%
 * @param[in] vibrationtimes  震动时间,单位100ms
 * @param[in] interval  震动时间间隔,单位100ms
 * @param[in] cnt  震动次数
 * @return    void
 **/
void bsp_motor_set(pwm_gpio gpio, u32 duty, u32 vibrationtimes, u32 interval, u32 cnt)
{
    if(cnt == 0 || vibrationtimes == 0)
    {
        return;
    }
    printf("%s: %d\n", __func__, duty);
    motor_st.vibration_times = vibrationtimes;
    motor_st.interval = interval;
    motor_st.duty = duty;
    motor_st.vibration_cnt = cnt;
    motor_st.motor_sta = true;
    motor_st.gpio = gpio;
    bsp_pwm_duty_set(motor_st.gpio, duty, false);
    co_timer_set(&motor_timer, 100, true, false, (void*)bsp_motor_time_callback, 0);
    return;
}
