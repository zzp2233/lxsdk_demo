#include "include.h"
#if ECIG_POWER_CONTROL
#if !ECIG_POLLING_CONTROL
#define TRACE_EN                             1
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


#if TRACE_EN
AT(.com_rodata.str)
const char hot_str[] = "%d %d %d\n";
AT(.com_rodata.str)
const char vol_hot_str[] = "%d %d %d\n";
AT(.com_rodata.str)
const char short_circuit_str[] = "short circuit! %d sta [%d]\n";
AT(.com_rodata.str)
const char info_8s[] = "8S warning\n";
#endif

AT(.com_text.isr) WEAK
void timer_hot_dual_isr(void)//100us
{
    if (ecig.mic_sta && (ecig.hot_time_cnt < ecig.cfg->heat_time_max)&& (ecig.smoke_sta != LOW_POWER)
        && (ecig.short_circuit_flag != SHORT_CIRCUIT) &&  (ecig.short_circuit_flag != OPEN_CIRCUIT))
    {
        if (ecig.mic_sta != ecig.mic_start)
        {
            if (ecig.timer_cycle_cnt < 6)
            {
                ECIG_VEN_ON();
                ECIG_PWM1_OFF();
                ecig_res_proc();
                caculate_res();
                ecig.timer_cycle_cnt++;
                ecig.hot_time_flag++;
                ecig_adgnd_io_set_gnd();
            }
            else if (ecig.timer_cycle_cnt == 6)
            {
                ECIG_VEN_OFF();
                ecig.timer_cycle_cnt++;
                if (!ecig.hot_res)
                {
                    ECIG_PWM1_OFF();
                    ecig.power_on_flag = 0;
                    ecig.short_circuit_flag = ecig.smoke_sta;//短路开路
                    printf(short_circuit_str,-1,ecig.short_circuit_flag);
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = ecig.smoke_sta;
                }
                else
                {
                    ecig.smoke_sta = SMOKING;
                    ecig.power_on_flag = 1;
                }
                ecig.mic_start = 1;
                ecig.timer_cycle_cnt = 0;
            }
        }
        else
        {
            if(ecig.timer_cycle_cnt < 2)  //获取电量
            {
                //ecig_hot_proc();
                ECIG_PWM1_OFF();
                ecig_vbat_proc();
                if(ecig.AD_BAT_voltage_mv < 3300)                   //欠压保护
                {
                    ECIG_PWM1_OFF();
                    ecig.smoke_sta = LOW_POWER;
                    ecig.power_on_flag = 0;
                    printf(hot_str,13,ecig.AD_BAT_voltage_mv,0);
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = LOW_POWER;
                }
                else
                {
                    //ECIG_PWM1_ON();
                    ecig.smoke_sta = SMOKING;
                    ecig.power_on_flag = 1;

                    //printf(hot_str,14,ecig.AD_BAT_voltage_mv,0);
                }
                //printf(hot_str,14,ecig.AD_BAT_voltage_mv,ecig.timer_cycle_cnt);

            }
            else if (ecig.timer_cycle_cnt < 6)    //四个t等电压稳定，做VDDIO校准
            {
                ECIG_PWM1_ON();
                ecig_hot_proc();
                ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                ecig.p_prev = ecig.p_current;
                //TRACE(vol_hot_str, ecig.timer_cycle_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
            }
            else
            {
                if (ecig.timer_cycle_cnt == 6)
                {
                    //为了体验感，全功率无法达到目标时，也要全功率加热
//                        if ((calculate_power(ecig.AD_hot_voltage) >> 13) < ecig.cfg->aim_power) {      //是否欠压
//                            ECIG_PWM1_OFF();
//                            ecig.power_on_flag = 0;
//                            TRACE(hot_str, 6, ecig.AD_hot_voltage);
//                        } else {
//                            ecig.power_on_flag = 1;
//                        }
                }
                if (ecig.power_on_flag)
                {
                    ECIG_PWM1_ON();
                    ecig_hot_proc();
                    if (ecig.timer_cycle_cnt == 6)
                    {
                        //TRACE(vol_hot_str, ecig.timer_cycle_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                    }
                    if (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) <= ecig.cfg->short_res_prop)     //short circuit
                    {
                        ECIG_PWM1_OFF();
                        ecig.power_on_flag = 0;/*  */
                        ecig.smoke_sta = SHORT_CIRCUIT;
                        ecig.short_circuit_flag = ecig.smoke_sta;
                        TRACE(hot_str, 7, ecig.AD_hot_voltage_mv,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                        TRACE(hot_str, 8, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                        TRACE(hot_str, ecig.AD_hot_voltage_mv,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv),ecig.AD_hot_voltage_mv * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        sys_cb.smoke_index = SHORT_CIRCUIT;
                    }
                    else if (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) >= ecig.cfg->open_res_prop)       //检阻部分已有开路保护，可以屏蔽
                    {
                        ECIG_PWM1_OFF();
                        ecig.power_on_flag = 0;/*  */
                        ecig.smoke_sta = OPEN_CIRCUIT;
                        ecig.short_circuit_flag = ecig.smoke_sta;
                        TRACE(hot_str, 8, ecig.AD_hot_voltage_mv,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                        TRACE(hot_str, 9, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                        TRACE(hot_str, ecig.AD_hot_voltage_mv,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv),ecig.AD_hot_voltage_mv * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        sys_cb.smoke_index = OPEN_CIRCUIT;
                    }
                    else
                    {
                        ecig.power_on_flag = 1;
                        if(sys_cb.smoke_index != SMOKING)
                        {
                            msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                            sys_cb.smoke_index = SMOKING;
                        }
                        //TRACE(hot_str, 10, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                    }
                    ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                    if (ecig.p_current >= ecig.cfg->aim_power * ecig.timer_big_cycles_10ms)
                    {
                        if (ecig.power_on_flag)
                        {
                            //TRACE(hot_str, 11,ecig.cfg->aim_power, ecig.p_current);
//                                TRACE(hot_str, 12, ecig.p_prev);
                        }
                        ECIG_PWM1_OFF();
                        ecig.power_on_flag = 0;
                    }
                    else
                    {
                        ecig.p_prev = ecig.p_current;
                    }
                }
                else
                {
                    ecig_vbat_proc();
                }
                if (ecig.timer_cycle_cnt == ecig.timer_big_cycles_10ms)
                {
                    ecig.timer_cycle_cnt = 0;
                    ECIG_PWM1_OFF();
                    //ecig_vbat_proc();//获取电量
                    ecig.power_on_flag = 1;
                    ecig.p_current = 0;
                    ecig.p_prev = 0;
                    return;
                }
            }
            ecig.timer_cycle_cnt++;
        }
    }
    else
    {
        if (!ecig.mic_sta)
        {
            ecig.timer_cycle_cnt = 0;
            ecig.mic_start = 0;
            ecig.hot_time_cnt = 0;
            ecig.hot_time_flag = 0;
            ecig.cnt_1s = 0;
            ecig.smoke_sta = IDLE;
            if(ecig.short_circuit_flag != SHORT_CIRCUIT)
            {
                ecig.short_circuit_flag = IDLE;
            }
//                ecig.short_circuit_flag = 0;
            //printf(ecig_end);
        }
        if(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)
        {
            if (ecig.cnt_1s == 1)
            {
                printf(info_8s);
                if(ecig.smoke_sta != SMOKE_TIMEOUT)
                {
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SMOKE_TIMEOUT;
                }
            }
        }
        if (ecig.short_circuit_flag == SHORT_CIRCUIT)
        {
            if (ecig.cnt_1s == 1)
            {
                printf(short_circuit_str,0,ecig.short_circuit_flag);
                if(ecig.smoke_sta != SHORT_CIRCUIT)
                {
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SHORT_CIRCUIT;
                }
            }
        }
        ecig_adgnd_io_deinit();
        ECIG_PWM1_OFF();
        ECIG_VEN_OFF();
    }
}


AT(.com_text.isr) WEAK
void timer_hot_single_isr(void)//100us
{
    if (ecig.mic_sta && (ecig.hot_time_cnt < ecig.cfg->heat_time_max) && (ecig.short_circuit_cnt < 10) && (ecig.smoke_sta != LOW_POWER)
        && (ecig.short_circuit_flag != SHORT_CIRCUIT) &&  (ecig.short_circuit_flag != OPEN_CIRCUIT))
    {
        ecig.hot_time_flag++;
        ecig.timer_cycle_cnt++;
        ecig_adgnd_io_set_gnd();
        //第一个周期不开pwm，检验VBAT是否欠压
        if (ecig.timer_cycle_cnt <= 2)
        {
            ECIG_PWM1_OFF();
            ecig_vbat_proc();
        }
        else if (ecig.timer_cycle_cnt == 3)
        {
            if (ecig.AD_BAT_voltage_mv < 3300)                    //欠压保护
            {
                ECIG_PWM1_OFF();
                ecig.power_on_flag = 0;
                ecig.smoke_sta = LOW_POWER;
                TRACE(hot_str, 11, ecig.AD_BAT_voltage_mv,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = LOW_POWER;
            }
            else
            {
                ecig.power_on_flag = 1;
                ECIG_PWM1_ON();
                ecig_hot_proc();
                if(sys_cb.smoke_index != SMOKING)
                {
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SMOKING;
                }

            }
        }
        //第3-6三个t打开pwm1，让vat稳定，再进行开路、短路检测
        else if (ecig.timer_cycle_cnt < 7)                        //前四个t用来做开、短路检测、VDDIO校准
        {
            ECIG_PWM1_ON();
            ecig_hot_proc();
            ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
            ecig.p_prev = ecig.p_current;
            //TRACE(vol_hot_str, ecig.timer_cycle_cnt, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
        }
        else if (ecig.timer_cycle_cnt == 7)
        {
            ecig_hot_proc();
            if (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) <= ecig.cfg->short_res_prop)           //短路保护
            {
                ecig.short_circuit_cnt++;
                ECIG_PWM1_OFF();
                ecig.power_on_flag = 0;
                ecig.smoke_sta = SHORT_CIRCUIT;
                ecig.short_circuit_flag = ecig.smoke_sta;
                TRACE(hot_str, 12, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = SHORT_CIRCUIT;
            }
            else if (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) >= ecig.cfg->open_res_prop)       //开路保护
            {
                ECIG_PWM1_OFF();
                ecig.power_on_flag = 0;
                ecig.smoke_sta = OPEN_CIRCUIT;
                ecig.short_circuit_flag = ecig.smoke_sta;
                TRACE(hot_str, 13, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                sys_cb.smoke_index = OPEN_CIRCUIT;
            }
            else
            {
                ecig.power_on_flag = 1;
                ecig.smoke_sta = SMOKING;
                //TRACE(vol_hot_str, ecig.timer_cycle_cnt, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                //TRACE(hot_str, ecig.timer_cycle_cnt, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                //TRACE(hot_str, 15, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
            }
            ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
            ecig.p_prev = ecig.p_current;
            //为了体验感，全功率无法达到目标时，也要全功率加热
//                if ((calculate_power(ecig.AD_hot_voltage) >> 13) < ecig.cfg->aim_power) {                              //是否欠压
//                    ECIG_PWM1_OFF();
//                    ecig.power_on_flag = 0;
//                } else {
//                    ecig.power_on_flag = 1;
//                }
        }
        else        //第8-101个t，正常加热流程为拉高pwm1，判断短路，计算功率
        {
            if (ecig.power_on_flag)
            {
                ECIG_PWM1_ON();
                ecig_hot_proc();
                //TRACE(vol_hot_str, ecig.timer_cycle_cnt, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                if (ecig.AD_hot_voltage_mv * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) <= ecig.cfg->short_res_prop)        //短路保护
                {
                    ecig.short_circuit_cnt++;
                    ECIG_PWM1_OFF();
                    ecig.power_on_flag = 0;
                    ecig.smoke_sta = SHORT_CIRCUIT;
                    ecig.short_circuit_flag = ecig.smoke_sta;
                    TRACE(hot_str, 16, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SHORT_CIRCUIT;
                }
                else
                {
                    ecig.power_on_flag = 1;
                }
                ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                if (ecig.p_current >= ecig.cfg->aim_power * ecig.timer_big_cycles_10ms)
                {
                    ECIG_PWM1_OFF();
                    ecig.power_on_flag = 0;
                }
                else
                {
                    ecig.p_prev = ecig.p_current;
                }
            }
            else        //不加热的时候采集vbat
            {
                ecig_vbat_proc();
            }
            if (ecig.timer_cycle_cnt == (ecig.timer_big_cycles_10ms + 1))     //第101个t，清零
            {
                ecig.timer_cycle_cnt = 0;
                ECIG_PWM1_OFF();
                //TRACE(hot_str, 32, ecig.cfg->aim_power,ecig.p_current);
                //ecig_vbat_proc();
                ecig.power_on_flag = 1;
                ecig.p_current = 0;
                ecig.p_prev = 0;
                ecig.short_circuit_cnt = 0;
            }
        }
    }
    else
    {
        ecig_adgnd_io_deinit();
        ECIG_PWM1_OFF();
        if (!ecig.mic_sta)
        {
            ecig.timer_cycle_cnt = 0;
            ecig.hot_time_cnt = 0;
            ecig.hot_time_flag = 0;
            ecig.cnt_1s = 0;
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
            if (ecig.cnt_1s == 1)
            {
                printf(short_circuit_str,1,ecig.short_circuit_flag);
                if(ecig.smoke_sta != SHORT_CIRCUIT)
                {
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SHORT_CIRCUIT;
                }
            }
        }
        if(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)
        {
            if (ecig.cnt_1s == 1)
            {
                printf(info_8s);
                if(ecig.smoke_sta != SMOKE_TIMEOUT)
                {
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SMOKE_TIMEOUT;
                }
            }
        }
    }
}



#endif
#endif
