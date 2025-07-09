#include "include.h"
#include "ute_module_message.h"
#include "ute_module_lockScreen.h"
#if ECIG_POWER_CONTROL

#if ECIG_POLLING_CONTROL

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
const char vol_hot_str[] = "123123  %d %d %d\n";
AT(.com_rodata.str)
const char short_circuit_str[] = "short circuit! %d sta [%d]\n";
AT(.com_rodata.str)
const char info_8s[] = "8S warning\n";
AT(.com_rodata.str)
const char hot_str11[] = "smoke_res_swich%d\n";
AT(.com_rodata.str)
const char vol_hot_test_str[] = " %d %d %d %d %d %d\n";

#endif

void ecig_clear_short_flag(bool state)
{
    ecig.short_circuit_flag = state;
}

AT(.com_text.isr)
void timer_hot_hot_vol(void)//
{
    u32 adc_vbg = saradc_get_value10(ADCCH_BGOP);
    u32 hot_voltage = saradc_get_value10(ecig.cfg->adc1_ch);
    u32 hot_voltage2 = saradc_get_value10(ecig.cfg->adc2_ch);
    // TRACE(hot_str, adc_vbg,hot_voltage,hot_voltage2);
#if DEVELOPMENT_BOARD_TYPE == DEVELOPMENT_BOARD_USER
    ecig.AD_hot_voltage_mv = (hot_voltage * ECIG_VBG_VOLTAGE / adc_vbg) * 48 / 33 / ECIG_VBG_VOLTAGE_MULTIPLE;
    ecig.AD_hot_voltage_mv2 = (hot_voltage2 * ECIG_VBG_VOLTAGE / adc_vbg) * 48 / 33 / ECIG_VBG_VOLTAGE_MULTIPLE;
    TRACE(hot_str, 9999,ecig.AD_hot_voltage_mv,ecig.AD_hot_voltage_mv2);
#else
    ecig.AD_hot_voltage_mv = (hot_voltage * ECIG_VBG_VOLTAGE / adc_vbg) * 4 / 3 / ECIG_VBG_VOLTAGE_MULTIPLE;
#endif
    ecig.AD_hot_voltage = (ecig.AD_hot_voltage_mv << 13) / 1000;
    ecig.AD_hot_voltage2 = (ecig.AD_hot_voltage_mv2 << 13) / 1000;
}
//获取电阻ADC
AT(.com_text.isr)
void timer_hot_res_kick(void)//
{
    if (!saradc_adc15_is_bg())
    {
        saradc_adc15_analog_select(ADCCH15_ANA_BG);
    }
    saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc_res1_ch)| BIT(ecig.cfg->adc2_ch) | BIT(ecig.cfg->adc_res2_ch) | SADCCH, 0, 0);
}
//电阻计算
AT(.com_text.isr)
void timer_hot_res_cul(void)//
{
    if (saradc_is_finish() && ecig.timer_switch_adc_flag)
    {
        ecig.timer_switch_acc_cnt ++;
        ecig.timer_switch_adc_flag = false;
        //电阻1
        ecig.adc1 = saradc_get_value10(ecig.cfg->adc1_ch);
        ecig.adc_res1 = saradc_get_value10(ecig.cfg->adc_res1_ch);
        caculate_res();
        //电阻2
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
            timer_hot_res_kick();
            ecig.timer_switch_adc_flag = true;
        }
    }
}
//计算电池电量
AT(.com_text.isr)
void timer_hot_bat_vol(void)//
{
    if (saradc_is_finish() && ecig.timer_switch_adc_flag)
    {
        ecig.timer_switch_acc_cnt ++;
        ecig.timer_switch_adc_flag = false;

        ecig.AD_BAT_voltage_mv = ecig_vbat_get();
        if(ecig.AD_BAT_voltage_mv < 3300)                   //欠压保护
        {
            ECIG_PWM_OFF_FUNC();
            ecig.smoke_sta = LOW_POWER;
            ecig.power_on_flag = 0;
            // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
            msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
            sys_cb.smoke_index = LOW_POWER;
            printf(hot_str,13,ecig.AD_BAT_voltage_mv,0);
        }
        else
        {
            //ECIG_PWM1_ON();
            ecig.smoke_sta = SMOKING;
            ecig.power_on_flag = 1;
            //printf(hot_str,14,ecig.AD_BAT_voltage_mv,0);
        }
        //printf(hot_str,55,ecig.AD_BAT_voltage_mv,ecig.timer_cycle_cnt);
    }
    else
    {
        if(!ecig.timer_switch_adc_flag)
        {
            ecig.timer_switch_adc_flag = true;
            ECIG_PWM_OFF_FUNC();
            if (!saradc_adc15_is_bg())
            {
                saradc_adc15_analog_select(ADCCH15_ANA_BG);
            }
            saradc_kick_start_do(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
        }

        //printf(hot_str,66,ecig.AD_BAT_voltage_mv,ecig.timer_cycle_cnt);
    }
}


//开始工作进行判断
AT(.com_text.isr)
bool timer_hot_mic_work(void)
{
    if (ecig.mic_sta //mic工作状态
        && (ecig.hot_time_cnt < ecig.cfg->heat_time_max)//吸烟超时时间
        && (ecig.smoke_sta != LOW_POWER) //低电量不工作
        && (ecig.short_circuit_flag != SHORT_CIRCUIT) //短路不工作
        && (ecig.poweroff_flag != 0) //关机立马不工作
        && (uteModulePasswordData.password_flag != 1) //童锁立马不工作
        &&  (ecig.short_circuit_flag != OPEN_CIRCUIT))//开路也不工作
    {
        return true;
    }
    return false;
}



AT(.com_text.isr)
void timer_hot_dual_isr(void)//
{

    if (timer_hot_mic_work())
    {
        if (ecig.mic_sta != ecig.mic_start)
        {
            ecig.timer_cycle_cnt++;
            ecig.hot_time_flag++;
            switch(ecig.timer_switch_acc_cnt)
            {
                case 0://采集阻值
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    //电阻计算
                    timer_hot_res_cul();
                    break;
                case 6:
                {
                    ECIG_VEN_OFF();
                    ecig.timer_cycle_cnt = 0;
                    ecig.timer_switch_acc_cnt = 0;
                    if (!ecig.hot_res)
                    {
                        ECIG_PWM_OFF_FUNC();
                        ecig.power_on_flag = 0;
                        ecig.short_circuit_flag = ecig.smoke_sta;//短路开路
                        printf(short_circuit_str,-1,ecig.short_circuit_flag);
                        //  uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        sys_cb.smoke_index = ecig.smoke_sta;
                        //break;
                    }
                    else
                    {
                        ecig.smoke_sta = SMOKING;
                        ecig.power_on_flag = 1;
                        //uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        sys_cb.smoke_index = SMOKING;
                    }
                    printf(hot_str,6,ecig.hot_res,0xff);
                    ecig.mic_start = 1;
                }
                break;
            }
        }
        else
        {
            switch(ecig.timer_switch_acc_cnt)
            {
                case 0:////获取电量
                case 1:
                {
                    timer_hot_bat_vol();
                }
                break;
                case 2:
                case 3:////等一段时间电压稳定，做VDDIO校准
                case 4:
                case 5:
                {
                    if (saradc_is_finish() && ecig.timer_switch_adc_flag)
                    {
                        ecig.timer_switch_adc_flag = false;
                        timer_hot_hot_vol();
                        //TRACE(hot_str, ecig.timer_switch_acc_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                        ecig.timer_switch_acc_cnt ++;
                    }
                    else
                    {
                        ecig.timer_switch_adc_flag = true;
                        ECIG_PWM_ON_FUNC();
                        saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc2_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                    }
                    //单发
                    if(!ecig.cfg->smoke_position_swich)
                    {
                        if(ecig.cfg->smoke_res_swich)
                        {
                            // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                            ecig.p_current = (u16)ecig.AD_hot_voltage_mv + ecig.p_prev;
                        }
                        else
                        {
                            ecig.p_current = (u16)ecig.AD_hot_voltage_mv2+ ecig.p_prev;
                        }
                    }
                    else
                    {
                        //双发
                        // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                        // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage2) >> 13) + ecig.p_current);
                        //  ecig.p_current = (u16)ecig.AD_hot_voltage_mv + ecig.p_prev;
                        //  ecig.p_current = (u16)ecig.AD_hot_voltage_mv2  + ecig.p_current;
                        ecig.p_current = ((u16)ecig.AD_hot_voltage_mv2 +(u16)ecig.AD_hot_voltage_mv)/2 + ecig.p_current;
                    }
                    ecig.p_prev = ecig.p_current;
                }
                break;
                case 6:
                {
                    if (saradc_is_finish() && ecig.timer_switch_adc_flag && ecig.power_on_flag)
                    {
                        ecig.timer_switch_acc_cnt  = 6;
                        ecig.timer_switch_adc_flag = false;
                        timer_hot_hot_vol();
                        TRACE(hot_str, 77, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                        if (( (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) <= ecig.cfg->short_res_prop) && (ecig.cfg->smoke_position_swich || ecig.cfg->smoke_res_swich))||
                            ((ecig.AD_hot_voltage_mv2 * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2) <= ecig.cfg->short_res_prop ) && (ecig.cfg->smoke_position_swich || !ecig.cfg->smoke_res_swich) ))     //short circuit
                        {
                            //短路判断
                            ECIG_PWM_OFF_FUNC();
                            ecig.power_on_flag = 0;/*  */
                            ecig.smoke_sta = SHORT_CIRCUIT;
                            ecig.short_circuit_flag = ecig.smoke_sta;
                            //  uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                            msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                            sys_cb.smoke_index = SHORT_CIRCUIT;
                            TRACE(hot_str, 88,ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                            TRACE(hot_str,  ecig.cfg->smoke_res_swich,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv),ecig.AD_hot_voltage_mv * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));

                            TRACE(hot_str, 888, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv2);
                            TRACE(hot_str, ecig.cfg->smoke_position_swich,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2),ecig.AD_hot_voltage_mv2 * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2));

                        }
                        // else if (((ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) >= ecig.cfg->open_res_prop) && (ecig.cfg->smoke_position_swich || ecig.cfg->smoke_res_swich)) ||
                        //          ((ecig.AD_hot_voltage_mv2 * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2) >= ecig.cfg->open_res_prop) && (ecig.cfg->smoke_position_swich || !ecig.cfg->smoke_res_swich)))     //检阻部分已有开路保护，可以屏蔽
                        // {
                        //     //开路判断
                        //     ECIG_PWM_OFF_FUNC();
                        //     ecig.power_on_flag = 0;/*  */
                        //     ecig.smoke_sta = OPEN_CIRCUIT;
                        //     ecig.short_circuit_flag = ecig.smoke_sta;
                        //     msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        //     sys_cb.smoke_index = OPEN_CIRCUIT;
                        //     TRACE(hot_str, 99,ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                        //     TRACE(hot_str,  ecig.cfg->smoke_res_swich,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv),ecig.AD_hot_voltage_mv * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));

                        //     TRACE(hot_str, 999, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv2);
                        //     TRACE(hot_str, ecig.cfg->smoke_res_swich,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2),ecig.AD_hot_voltage_mv2 * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2));

                        // }
                        // else
                        // {
                        ecig.power_on_flag = 1;
                        //TRACE(hot_str, ecig.timer_cycle_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                        //  }
                    }
                    else
                    {
                        if(ecig.power_on_flag)
                        {
                            ECIG_PWM_ON_FUNC();
                            if(!ecig.timer_switch_adc_flag)
                            {
                                ecig.timer_switch_adc_flag = true;
                                saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc2_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                                //TRACE(hot_str, ecig.timer_cycle_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                            }
                        }

                    }

                    if (ecig.power_on_flag)
                    {
                        //ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                        //TRACE(hot_str11,ecig.cfg->smoke_res_swich );
                        //单发
                        if(!ecig.cfg->smoke_position_swich)
                        {
                            if(ecig.cfg->smoke_res_swich)
                            {
                                // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                                ecig.p_current = (u16)ecig.AD_hot_voltage_mv + ecig.p_prev;
                            }
                            else
                            {
                                // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage2) >> 13) + ecig.p_prev);
                                ecig.p_current = (u16)ecig.AD_hot_voltage_mv2  + ecig.p_prev;
                            }
                        }
                        else
                        {
                            //双发
                            // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                            // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage2) >> 13) + ecig.p_current);
                            // ecig.p_current = (u16)ecig.AD_hot_voltage_mv + ecig.p_prev;
                            // ecig.p_current = (u16)ecig.AD_hot_voltage_mv2 + ecig.p_current;
                            ecig.p_current = ((u16)ecig.AD_hot_voltage_mv +(u16)ecig.AD_hot_voltage_mv2)/2 + ecig.p_prev;
                        }
                        // if (ecig.p_current >= ecig.cfg->aim_power * ecig.timer_big_cycles_10ms)
                        //  printf(hot_str,ecig.AD_hot_voltage_mv,ecig.timer_cycle_cnt,12340);
                        // printf(vol_hot_test_str,ecig.p_current,ecig.AD_hot_voltage_mv,ecig.AD_hot_voltage_mv2,ecig.timer_cycle_cnt, ecig.cfg->aim_voltage,ecig.cfg->smoke_res_swich);
                        if (ecig.p_current >= ecig.cfg->aim_voltage * ecig.timer_big_cycles_10ms)
                        {
                            // printf(vol_hot_str,ecig.AD_hot_voltage_mv,ecig.timer_cycle_cnt, ecig.cfg->aim_voltage);
                            if (ecig.power_on_flag)
                            {
                                //TRACE(hot_str, ecig.timer_cycle_cnt,ecig.cfg->aim_power, ecig.p_current);
                                //                                TRACE(hot_str, 12, ecig.p_prev);
                            }
                            ECIG_PWM_OFF_FUNC();
                            ecig.power_on_flag = 0;
                        }
                        else
                        {
                            ecig.p_prev = ecig.p_current;
                        }
                    }
                }
                break;

            }
            if (ecig.timer_cycle_cnt == ecig.timer_big_cycles_10ms)
            {
                ecig.timer_cycle_cnt = 0;
                ecig.timer_switch_acc_cnt = 0;
                ECIG_PWM_OFF_FUNC();
                //ecig_vbat_proc();//获取电量
                ecig.power_on_flag = 1;
                ecig.p_current = 0;
                ecig.p_prev = 0;
                return;
            }
            ecig.timer_cycle_cnt++;
        }
    }
    else
    {
        if (!ecig.mic_sta)
        {
            ecig.mic_start = 0;
            ecig.hot_time_cnt = 0;
            ecig.hot_time_flag = 0;
            ecig.cnt_1s = 0;
            if(ecig.smoke_sta != IDLE)
            {
                ecig.cfg->smoke_res_swich = !ecig.cfg->smoke_res_swich;
                //  TRACE(hot_str11,ecig.cfg->smoke_res_swich );
            }
            ecig.smoke_sta = IDLE;

            ecig.timer_cycle_cnt = 0;
            ecig.timer_switch_acc_cnt = 0;
            ecig.timer_switch_adc_flag = false;
            if(ecig.short_circuit_flag /* != SHORT_CIRCUIT */)
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
                    // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
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
}


AT(.com_text.isr) WEAK
void timer_hot_single_isr(void)//
{

}

#endif

#endif
