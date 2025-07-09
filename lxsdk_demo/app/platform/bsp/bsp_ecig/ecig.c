#include "include.h"
#include "ecig.h"
#include "ute_module_message.h"
#include "ute_module_lockScreen.h"
#if ECIG_POWER_CONTROL
#define TRACE_EN                             1




#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
char flag_str[] = " 99999999999999999999return 1\n";
#if TRACE_EN
AT(.com_rodata.str)
const char not_finish_str[] = "#%d:%x-%x\n";
#endif
ecig_cb_t ecig;

#if TRACE_EN
AT(.com_rodata.str)
const char res_str[] = "res %d %d %d %d\n";
extern const char short_circuit_str[];
extern const char info_8s[];
AT(.com_rodata.str)
const char info_det[] = "info_det %d\n";
#endif

AT(.com_text.ecig.init)
uint8_t mic_start_or_not(void)
{
    if(func_cb.sta == FUNC_AGEING && ( ecig.cfg->smoke_start_or_not ==1))
    {
        // printf(flag_str);
        return 1;
    }
    gpio_t gpio;
    gpio_cfg_init(&gpio, ecig.cfg->io_mic);
    if (gpio.sfr)
    {
        return ((gpio.sfr[GPIOx] & BIT(gpio.num)) >> gpio.num);
    }
    return 0;
}

void ecig_mic_io_init(void)
{
    gpio_t gpio;
    gpio_cfg_init(&gpio, ecig.cfg->io_mic);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);
    }
}


AT(.com_text.ecig.init)
uint8_t det1_start_or_not(void)
{
    //gpio_t gpio;
    //gpio_cfg_init(&gpio, ecig.cfg->io_hot_det1);
    if (ecig.cfg->ecig_gpio[ECIG_DET1_IO].sfr)
    {
        return !((ecig.cfg->ecig_gpio[ECIG_DET1_IO].sfr[GPIOx] & BIT(ecig.cfg->ecig_gpio[ECIG_DET1_IO].num)) >> ecig.cfg->ecig_gpio[ECIG_DET1_IO].num);
    }
    return 0;
}

AT(.com_text.ecig.init)
uint8_t det2_start_or_not(void)
{
    //gpio_t gpio;ecig.cfg->smoke_start_or_not
    //gpio_cfg_init(&gpio, ecig.cfg->io_hot_det1);
    if (ecig.cfg->ecig_gpio[ECIG_DET2_IO].sfr)
    {
        return !((ecig.cfg->ecig_gpio[ECIG_DET2_IO].sfr[GPIOx] & BIT(ecig.cfg->ecig_gpio[ECIG_DET2_IO].num)) >> ecig.cfg->ecig_gpio[ECIG_DET2_IO].num);
    }
    return 0;
}

//插拔检测
void ecig_det_io_init(void)
{
    gpio_t gpio;
    if(!ecig.cfg->io_hot_mode)
    {
        return;
    }
    gpio_cfg_init(&gpio, ecig.cfg->io_hot_det1);
    memcpy(&ecig.cfg->ecig_gpio[ECIG_DET1_IO],&gpio,sizeof(gpio_t));
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);
    }
    memset(&gpio,0,sizeof(gpio_t));
    gpio_cfg_init(&gpio, ecig.cfg->io_hot_det2);
    memcpy(&ecig.cfg->ecig_gpio[ECIG_DET2_IO],&gpio,sizeof(gpio_t));
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);
    }
}

AT(.com_text.ecig.init)
void ecig_adgnd_io_set_gnd(void)
{
    //gpio_t gpio;
    //gpio_cfg_init(&gpio, ecig.cfg->io_adgnd);
    if (ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr)
    {
        ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr[GPIOxFEN] &= ~BIT(ecig.cfg->ecig_gpio[ECIG_ADGND_IO].num);
        ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr[GPIOxDE] |= BIT(ecig.cfg->ecig_gpio[ECIG_ADGND_IO].num);
        ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr[GPIOxDIR] &= ~BIT(ecig.cfg->ecig_gpio[ECIG_ADGND_IO].num);
        ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr[GPIOxCLR] = BIT(ecig.cfg->ecig_gpio[ECIG_ADGND_IO].num);
    }
}

AT(.com_text.ecig.init)
void ecig_adgnd_io_deinit(void)
{
    gpio_t gpio;
    gpio_cfg_init(&gpio, ecig.cfg->io_adgnd);
    memcpy(&ecig.cfg->ecig_gpio[ECIG_ADGND_IO],&gpio,sizeof(gpio_t));
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);
    }
}

void ecig_pwm_io_init(void)
{
    gpio_t gpio;
    gpio_cfg_init(&gpio, ecig.cfg->io_pwm1);
    memcpy(&ecig.cfg->ecig_gpio[ECIG_PWM1_IO],&gpio,sizeof(gpio_t));
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxCLR] = BIT(gpio.num);
    }
    gpio_cfg_init(&gpio, ecig.cfg->io_pwm2);
    memcpy(&ecig.cfg->ecig_gpio[ECIG_PWM2_IO],&gpio,sizeof(gpio_t));
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxCLR] = BIT(gpio.num);
    }
}

void ecig_ven_io_init(void)
{
    gpio_t gpio;
    gpio_cfg_init(&gpio, ecig.cfg->io_ven);
    memcpy(&ecig.cfg->ecig_gpio[ECIG_VEN_IO],&gpio,sizeof(gpio_t));
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxCLR] = BIT(gpio.num);
    }
}


AT(.com_text.ecig.pwm)
void ecig_pwm_set(u8 pwm_num, bool pwm_on)
{
    gpio_t gpio;
    if(pwm_num == 1)
    {
        memcpy(&gpio,&ecig.cfg->ecig_gpio[ECIG_PWM1_IO],sizeof(gpio_t));
    }
    else
    {
        memcpy(&gpio,&ecig.cfg->ecig_gpio[ECIG_PWM2_IO],sizeof(gpio_t));
    }
    if (gpio.sfr)
    {
        if (pwm_on)
        {
            gpio.sfr[GPIOxSET] = BIT(gpio.num);
        }
        else
        {
            gpio.sfr[GPIOxCLR] = BIT(gpio.num);
        }
    }
}

AT(.com_text.ecig.pwm)
void ecig_ven_set(u8 pwm_num, bool pwm_on)
{
    //ecig.cfg->ecig_gpio[ECIG_VEN_IO];
    if (ecig.cfg->ecig_gpio[ECIG_VEN_IO].sfr)
    {
        if (pwm_on)
        {
            ecig.cfg->ecig_gpio[ECIG_VEN_IO].sfr[GPIOxSET] = BIT(ecig.cfg->ecig_gpio[ECIG_VEN_IO].num);
        }
        else
        {
            ecig.cfg->ecig_gpio[ECIG_VEN_IO].sfr[GPIOxCLR] = BIT(ecig.cfg->ecig_gpio[ECIG_VEN_IO].num);
        }
    }
}


AT(.com_text.ecig.power)
u32 calculate_power(u32 hot_voltage)
{
    u16 hot_v = (u16)hot_voltage;
    u32 power;
    if (ecig.cfg->adc2_en)
    {
        power = hot_v * hot_v / ecig.hot_res;
    }
    else
    {
        power = hot_v * hot_v / ecig.cfg->res_wire;
    }
    return power;
}


AT(.com_rodata.str)
const char ecig_end[] = "ecig_end \n";


#if TRACE_EN
AT(.com_rodata.str)
const char res_ch[] = "res :%d %d\n";

#endif

AT(.com_text.ecig.res)
void caculate_res(void)
{
    u32 res = 0;
    u16 diff;
    if (ecig.adc1 == 0)
    {
        ecig.hot_res = 0;
        TRACE(res_ch, -1,0);
        return;
    }

    if ((ecig.adc_res1 / ecig.adc1) <= ecig.cfg->open_res_proportion)
    {
        ecig.hot_res = 0;
        //  printf(flag_str,__LINE__);
        ecig.smoke_sta = OPEN_CIRCUIT;
        TRACE(res_ch, -2,(ecig.adc_res1 / ecig.adc1) );
    }
    else if ((ecig.adc_res1 / ecig.adc1) >= ecig.cfg->short_res_proportion)
    {
        ecig.hot_res = 0;
        ecig.smoke_sta = SHORT_CIRCUIT;
        TRACE(res_ch, -3,(ecig.adc_res1 / ecig.adc1) );
    }
    else
    {
        diff = (u16)(ecig.adc_res1 - ecig.adc1);
        res = ecig.adc1 * ecig.cfg->res_diff;
        ecig.hot_res = res / diff;
        ecig.cfg->res_wire = ecig.hot_res;
    }
    TRACE(res_str,1111, ecig.hot_res*1000/8192, ecig.adc1, ecig.adc_res1);
}

AT(.com_text.ecig.res)
void caculate_res2(void)
{
    u32 res = 0;
    u16 diff;
    if (ecig.adc2 == 0)
    {
        ecig.hot_res2 = 0;
        TRACE(res_ch, -1,0);
        return;
    }

    if ((ecig.adc_res2 / ecig.adc2) <= ecig.cfg->open_res_proportion)
    {
        ecig.hot_res2 = 0;
        // printf(flag_str,__LINE__);
        ecig.smoke_sta = OPEN_CIRCUIT;
        TRACE(res_ch, -2,(ecig.adc_res2 / ecig.adc2) );
    }
    else if ((ecig.adc_res2 / ecig.adc2) >= ecig.cfg->short_res_proportion)
    {
        ecig.hot_res2 = 0;
        ecig.smoke_sta = SHORT_CIRCUIT;
        TRACE(res_ch, -3,(ecig.adc_res2 / ecig.adc2) );
    }
    else
    {
        diff = (u16)(ecig.adc_res2 - ecig.adc2);
        res = ecig.adc2 * ecig.cfg->res_diff;
        ecig.hot_res2 = res / diff;
        ecig.cfg->res_wire2 = ecig.hot_res2;
    }
    TRACE(res_str,222, ecig.hot_res2*1000/8192, ecig.adc2, ecig.adc_res2);
}

static u16 cnt_1s = 0;
AT(.com_text.isr)
void timer_hot_single_data_init(void)
{
    ecig_adgnd_io_deinit();
    ECIG_PWM_OFF_FUNC();
    if (!ecig.mic_sta)
    {
        ecig.timer_cycle_cnt = 0;
        ecig.hot_time_cnt = 0;
        ecig.hot_time_flag = 0;
        cnt_1s = 0;
        ecig.timer_switch_acc_cnt = 0;
        ecig.timer_switch_adc_flag = false;
        ecig.smoke_sta = IDLE;
        if(ecig.short_circuit_flag != SHORT_CIRCUIT)
        {
            ecig.short_circuit_flag = IDLE;
        }
    }
    if (ecig.short_circuit_cnt >= 10 || ecig.short_circuit_flag == SHORT_CIRCUIT)            //累计判断短路，短路后不再开启
    {
        ecig.power_on_flag = 0;
        //ecig.short_circuit_flag = 1;
        ecig.hot_time_cnt = 0;
        ecig.hot_time_flag = 0;
        if (cnt_1s == 1)
        {
            printf(short_circuit_str,1,ecig.short_circuit_flag);
            if(ecig.smoke_sta != SHORT_CIRCUIT)
            {
                //    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = SHORT_CIRCUIT;
            }
        }
    }
    if(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)
    {
        if (cnt_1s == 1)
        {
            if(ecig.smoke_sta != SMOKE_TIMEOUT)
            {
                //uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = SMOKE_TIMEOUT;
            }
            printf(info_8s);
        }
    }
}

AT(.com_text.isr)
void timer_hot_dual_data_init(void)
{
    if (!ecig.mic_sta)
    {
        ecig.timer_cycle_cnt = 0;
        ecig.mic_start = 0;
        ecig.hot_time_cnt = 0;
        ecig.hot_time_flag = 0;
        cnt_1s = 0;
        ecig.smoke_sta = IDLE;

        ecig.timer_switch_acc_cnt = 0;
        ecig.timer_switch_adc_flag = false;
        if(ecig.short_circuit_flag != SHORT_CIRCUIT)
        {
            ecig.short_circuit_flag = IDLE;
        }
    }
    if(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)
    {
        if (cnt_1s == 1)
        {
            printf(info_8s);
            if(ecig.smoke_sta != SMOKE_TIMEOUT)
            {
                //  uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = SMOKE_TIMEOUT;
            }
        }
    }
    if (ecig.short_circuit_flag == SHORT_CIRCUIT)
    {
        if (cnt_1s == 1)
        {
            printf(short_circuit_str,0,ecig.short_circuit_flag);
            if(ecig.smoke_sta != SHORT_CIRCUIT)
            {
                // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = SHORT_CIRCUIT;
            }
        }
    }
    ecig_adgnd_io_deinit();
    ECIG_PWM_OFF_FUNC();
    ECIG_VEN_OFF();

}

AT(.com_text.isr)
void timer_hot_dual_caculate_res1(void)//
{
    if (saradc_is_finish() && ecig.timer_switch_adc_flag)
    {
        ecig.timer_switch_adc_flag = false;
        ecig.adc1 = saradc_get_value10(ecig.cfg->adc1_ch);
        ecig.adc_res1 = saradc_get_value10(ecig.cfg->adc_res1_ch);
        caculate_res();
    }
    else
    {
        ECIG_VEN_ON();
        ECIG_PWM_OFF_FUNC();
        ecig_adgnd_io_set_gnd();
        if(!ecig.timer_switch_adc_flag)
        {
            if (!saradc_adc15_is_bg())
            {
                saradc_adc15_analog_select(ADCCH15_ANA_BG);
            }
            saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc_res1_ch) | SADCCH, 0, 0);
            ecig.timer_switch_adc_flag = true;
        }
    }
}

AT(.com_text.isr)
void timer_hot_dual_caculate_res2(void)//
{
    if (saradc_is_finish() && ecig.timer_switch_adc_flag)
    {
        ecig.timer_switch_adc_flag = false;
        ecig.adc2 = saradc_get_value10(ecig.cfg->adc2_ch);
        ecig.adc_res2 = saradc_get_value10(ecig.cfg->adc_res2_ch);
        caculate_res2();
    }
    else
    {
        ECIG_VEN_ON();
        ECIG_PWM_OFF_FUNC();
        ecig_adgnd_io_set_gnd();
        if(!ecig.timer_switch_adc_flag)
        {
            if (!saradc_adc15_is_bg())
            {
                saradc_adc15_analog_select(ADCCH15_ANA_BG);
            }
            saradc_kick_start_do(BIT(ecig.cfg->adc2_ch) | BIT(ecig.cfg->adc_res2_ch) | SADCCH, 0, 0);
            ecig.timer_switch_adc_flag = true;
        }
    }
}

AT(.com_text.isr)
bool timer_hot_det_res(void)
{
    static u32 cnt_det_20ms = 0;
    static u32 cnt_det2_20ms = 0;
    //插拔检测
    if(det1_start_or_not() != ecig.det1_sta)
    {
        //  printf(flag_str,__LINE__);
        if (++cnt_det_20ms >= (ecig.timer_20ms_cnt))      //20ms
        {
            if(det1_start_or_not())//()
            {
                //printf(flag_str,__LINE__);
#if ECIG_ADC2_EN
                if(ecig.timer_switch_acc_cnt < 6)
                {
                    //  printf(flag_str,__LINE__);
                    timer_hot_dual_caculate_res1();
                    ecig.timer_switch_acc_cnt++;
                    return true;
                }
                else if(ecig.timer_switch_acc_cnt >= 6)
                {
                    ecig.det1_sta = !ecig.det1_sta;
                    timer_hot_dual_data_init();
                    ecig.det1_start = true;
                    //   printf(flag_str,__LINE__);
                    //  uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = IN_DEVICE;
                    printf(info_det,ecig.det1_start);
                }
#else
                ecig.det1_sta = !ecig.det1_sta;
                ecig.det1_start = true;
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = IN_DEVICE;
#endif
            }
            else
            {
                //   printf(flag_str,__LINE__);
                ecig.det1_sta = !ecig.det1_sta;
                ecig.det1_start = false;
                ecig_set_res(0);
                // printf(flag_str,__LINE__);
                // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = OPEN_CIRCUIT;
                printf(info_det,ecig.det1_start);
            }

        }
    }
    else
    {
        cnt_det_20ms = 0;
    }

    if(det2_start_or_not() != ecig.det2_sta)
    {
        //  printf(flag_str,__LINE__);
        if (++cnt_det2_20ms >= (ecig.timer_20ms_cnt))      //20ms
        {
            if(det2_start_or_not())
            {
#if ECIG_ADC2_EN
                if(ecig.timer_switch_acc_cnt < 6)
                {
                    //  printf(flag_str,__LINE__);
                    timer_hot_dual_caculate_res2();
                    ecig.timer_switch_acc_cnt++;
                    return true;
                }
                else if(ecig.timer_switch_acc_cnt >= 6)
                {
                    ecig.det2_sta = !ecig.det2_sta;
                    timer_hot_dual_data_init();
                    ecig.det2_start = true;
                    //  printf(flag_str,__LINE__);
                    // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = IN_DEVICE;
                    printf(info_det,ecig.det2_start);
                }
#else
                ecig.det2_sta = !ecig.det2_sta;
                ecig.det2_start = true;
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = IN_DEVICE;
#endif
            }
            else
            {
                ecig.det2_sta = !ecig.det2_sta;
                ecig.det2_start = false;
                ecig_set_res2(0);
                //  printf(flag_str,__LINE__);
                //    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = OPEN_CIRCUIT;
                printf(info_det,ecig.det2_start);
            }

        }
    }
    else
    {
        cnt_det2_20ms = 0;
    }

    if(!ecig.det1_start || !ecig.det2_start)
    {
        //   printf(flag_str,__LINE__);
#if ECIG_ADC2_EN
        timer_hot_dual_data_init();
#else
        timer_hot_single_data_init();
#endif
        return true;
        ecig.short_circuit_flag = IDLE;
    }
    return false;
}



AT(.com_text.isr)
void ecigarette_isr(void)//50us
{
    if(TMR3CON & BIT(9))
    {
        TMR3CPND = BIT(9);
    }
    else
    {
        return;
    }
    if(sys_clk_get() < 2)
    {
        return;
    }

    static u16 cnt_20ms = 0;
    if (mic_start_or_not() != ecig.mic_sta)
    {
        if (++cnt_20ms >= (ecig.timer_20ms_cnt))      //20ms
        {
            ecig.mic_sta = !ecig.mic_sta;
        }
    }
    else
    {
        cnt_20ms = 0;
    }
    if (ecig.mic_sta )
    {
        if (++ecig.cnt_1s >= (ecig.timer_1s_cnt))     //1s
        {
            ecig.hot_time_cnt += (bool)ecig.hot_time_flag;
            ecig.cnt_1s = 0;
        }
    }

#if ECIG_DET_EN
    if(timer_hot_det_res())
    {
        return;
    }
#endif




    static u16 cnt_lock_ms = 0;
    //if(ecig.clock_flag == 1)
    if(uteModulePasswordData.password_flag == 1)
    {
        if(mic_start_or_not())
        {
            if(cnt_lock_ms >= (ecig.timer_20ms_cnt))
            {
                if(cnt_lock_ms == (ecig.timer_20ms_cnt))
                {
                    //发提醒事件
                    func_cb.sta = FUNC_ECIG_REMINDERCLOCK;
                    printf(flag_str);
                }
            }
            else
            {
                cnt_lock_ms++;
            }
        }
        else
        {
            cnt_lock_ms = 0;
        }
        return;
    }
#if ECIG_ADC2_EN
    timer_hot_dual_isr();
#else
    timer_hot_single_isr();
#endif

}


void ecig_timer_init(void)  //使用TMR3
{
    CLKGAT1 |= BIT(8);
    TMR3CNT = 0;
    TMR3PR = 13 * ECIG_TIMER_US - 1;  //ECIG_TIMER_US
    TMR3CPND = BIT(9) | BIT(10) | BIT(12);
    TMR3CON = BIT(7) | (1<<4) | (2<<1) | BIT(0);  // xosc26_div2

    PICPR &= ~BIT(IRQ_TMR3_VECTOR);
    PICEN |= BIT(IRQ_TMR3_VECTOR);
    register_isr(IRQ_TMR3_VECTOR, ecigarette_isr);
}

AT(.com_text.ecig)
bool ecig_is_working(void)
{
    return (ecig.mic_sta && !(ecig.hot_time_cnt >= ecig.cfg->heat_time_max));
}

AT(.com_text.ecig)
u8 ecig_is_working_sta(void)
{
    return ecig.smoke_sta;
}

void ecigarette_init(ecig_cfg_t* cfg)
{
    if (cfg == NULL)
    {
        return;
    }

    memset(&ecig, 0, sizeof(ecig_cb_t));
    ecig.timer_20ms_cnt = (20000 / ECIG_TIMER_US);
    ecig.timer_1s_cnt   = (1000000 / ECIG_TIMER_US);
    ecig.timer_big_cycles_10ms = (10000  / ECIG_TIMER_US);
    ecig.cfg = cfg;

    ecig_pwm_io_init();
    ecig_ven_io_init();
    ecig_adgnd_io_deinit();
    ecig_mic_io_init();
    ecig_det_io_init();
    ecig_hot_detect_init();
    ecig_timer_init();
}
void ecigarette_sleep_wakeup(ecig_cfg_t* cfg)
{
    if (cfg == NULL)
    {
        return;
    }
    ecig_pwm_io_init();
    ecig_mic_io_init();
    ecig_det_io_init();
    ecig_hot_detect_init();
    ecig_timer_init();
}


void ecigarette_exit(void)
{
    ECIG_PWM_OFF_FUNC();
    ECIG_VEN_OFF();
    PICEN &= ~BIT(IRQ_TMR3_VECTOR);
    TMR3CON = 0;
}


AT(.com_text.isr)
void ECIG_PWM_OFF_FUNC(void)
{
    ECIG_PWM1_OFF();
    ECIG_PWM2_OFF();
}
AT(.com_text.isr)
void ECIG_PWM_ON_FUNC(void)
{
    if(!ecig.cfg->smoke_position_swich)
    {
        if(ecig.cfg->smoke_res_swich)
        {
            ECIG_PWM1_ON();
        }
        else
        {
            ECIG_PWM2_ON();
        }
    }
    else
    {
        ECIG_PWM1_ON();
        ECIG_PWM2_ON();
    }

}


///---------------------------------------------（ADC相关）----------------------------------------------------




void ecig_hot_detect_init(void)  //初始化HOT电路ADC功能
{
    saradc_kick_start_do(ecig.cfg->adc2_en ? BIT(ecig.cfg->adc1_ch) : BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc_res1_ch), 0, 0);
    while(!saradc_is_finish());
    ecig_hot_proc();
}

AT(.com_text.ecig.process)
void ecig_hot_proc(void) //读取HOT电路电压
{
    if(!saradc_is_finish())
    {
        TRACE(not_finish_str, 1, SADCCH, BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH);
    }

    u32 adc_vbg = saradc_get_value10(ADCCH_BGOP);
    u32 hot_voltage = saradc_get_value10(ecig.cfg->adc1_ch);
    ecig.AD_hot_voltage_mv = (hot_voltage * ECIG_VBG_VOLTAGE / adc_vbg) * 4 / 3 / ECIG_VBG_VOLTAGE_MULTIPLE;
    ecig.AD_hot_voltage = (ecig.AD_hot_voltage_mv << 13) / 1000;

    saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
}

AT(.com_text.ecig.process)
void ecig_res_proc(void)
{
    if(!saradc_is_finish())
    {
        TRACE(not_finish_str, 4, SADCCH, BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc_res1_ch) | SADCCH);
    }

    ecig.adc1 = saradc_get_value10(ecig.cfg->adc1_ch);
    ecig.adc_res1 = saradc_get_value10(ecig.cfg->adc_res1_ch);

    if (!saradc_adc15_is_bg())
    {
        saradc_adc15_analog_select(ADCCH15_ANA_BG);
    }
    saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc_res1_ch) | SADCCH, 0, 0);
}

AT(.com_text.ecig.process) WEAK
u32 ecig_vbat_get(void)
{
    return saradc_vbat_get_calc_value(saradc_get_value10(ADCCH_VBAT), saradc_get_value10(ADCCH_BGOP),0, 0); //不太准确，需要滤波算法
}

AT(.com_text.ecig.process)
void ecig_vbat_proc(void)
{
    if (!saradc_is_finish())
    {
        TRACE(not_finish_str, 3, SADCCH, BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP) | SADCCH);
    }

    ecig.AD_BAT_voltage_mv = ecig_vbat_get();

    if (!saradc_adc15_is_bg())
    {
        saradc_adc15_analog_select(ADCCH15_ANA_BG);
    }
    saradc_kick_start_do(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
}
///------------------------------------------------------------------------------------------------------------
#endif
