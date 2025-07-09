#include "include.h"
#include "ute_module_smoke.h"
#if ECIG_POWER_CONTROL

ecig_cfg_t ecig_cfg;

void bsp_ecig_init(void)
{
    printf("%s\n", __func__);
    ecig_cfg_t *p = &ecig_cfg;
    memset(p, 0, sizeof(ecig_cfg_t));

    p->io_pwm1 = ECIG_PWM1_GPIO;
    p->io_mic = ECIG_MIC_GPIO;

    p->io_adgnd = ECIG_GND_GPIO;
    p->adc1_ch = ECIG_ADC1_GPIO;

#if ECIG_ADC2_EN
    p->adc2_en = 1;
    p->io_pwm2 = ECIG_PWM2_GPIO,
       p->io_ven = ECIG_VEN_GPIO;
    p->adc_res1_ch = ECIG_ADC_RES1_GPIO;
    p->adc2_ch = ECIG_ADC2_GPIO;
    p->adc_res2_ch = ECIG_ADC_RES2_GPIO;
#if DEVELOPMENT_BOARD_TYPE == DEVELOPMENT_BOARD_USER
    p->res_diff = (u16)(10.0* 8192);
    p->short_res_proportion = 22;                       //计算方法：res_diff/res_min + 1(去掉小数部分),例res_diff = 4.7Ω，res_min = 0.4Ω
    p->open_res_proportion  = 4;                        //计算方法：res_diff/res_max + 1(有小数进一),例res_diff = 4.7Ω，res_max = 2Ω
#else
    p->res_diff = (u16)(4.7* 8192);
    p->short_res_proportion = 12;                       //计算方法：res_diff/res_min + 1(去掉小数部分),例res_diff = 4.7Ω，res_min = 0.4Ω
    p->open_res_proportion  = 4;
#endif
#else
    p->res_wire = (u16)(1.0 * 8192);

#endif
#if ECIG_DET_EN
    p->io_hot_det1 = ECIG_DET1_GPIO;
    p->io_hot_det2 = ECIG_DET2_GPIO;
    p->io_hot_mode = true;
#endif

    //p->aim_power = 5,
    //ecig_set_power(15);
    ecig_cfg.aim_voltage = 3290;
    p->heat_time_max = 8;
    p->short_res_prop = 14;
    p->open_res_prop = 320;
    test_1st_gear_func();
    ecigarette_init(p); //需要用到硬件定时器TMR3
}

void bsp_ecig_sleep_wakeup(void)
{
    ecig_cfg_t *p = &ecig_cfg;
    ecigarette_sleep_wakeup(p);
}




AT(.com_text.ecig.process)
u32 ecig_vbat_get(void)
{
    return bsp_vbat_get_voltage(0);
}
typedef struct
{
    u16 power;   // 功率值
    u16 voltage; // 对应电压值
} PowerVoltagePair;
//单发0.8欧
const PowerVoltagePair power_voltage_singe_limi_0_8[]=
{
    {10,2690},
    {11,2740},
    {12,2790},
    {13,2840},
    {14,3050},
    {15,3290},
    {16,3480},
    {17,3530},
    {18,3570},
};
//单发0.8欧
const PowerVoltagePair power_voltage_dual_limi_0_4[]=
{
    {20,3010},
    {21,3060},
    {22,3110},
    {23,3160},
    {24,3260},
    {25,3310},
    {26,3410},
    {27,3440},
    {28,3460},
    {29,3510},
    {30,3560},
};
//单发1.2
const PowerVoltagePair power_voltage_singe_limi_1_2[]=
{
    {10,3350},
    {11,3400},
    {12,3440},
    {13,3480},
    {14,3520},
    {15,3560},
    {16,3600},
    {17,3640},
    {18,3680},
    {19,3720},
    {20,3760},
};
//双发1.2
const PowerVoltagePair power_voltage_dual_limi_1_2[]=
{
    {20,2940},
    {21,2960},
    {22,2980},
    {23,3000},
    {24,3020},
    {25,3040},
    {26,3060},
    {27,3080},
    {28,3100},
    {29,3120},
    {30,3140},
};
u8 ecig_get_power(void)
{
    return uteModuleSmokeData.current_power ;
}
AT(.com_text.isr)
void ecig_set_res(u8 value)
{
    ecig_cfg.res_wire = value;
}
AT(.com_text.isr)
void ecig_set_res2(u8 value)
{
    ecig_cfg.res_wire2 = value;
}

#define DEBUG 0
uint8_t ecig_get_res(void)
{
#if DEBUG
    printf("%s, res_wire=%d\n", __func__, ecig_cfg.res_wire);
#endif
    // 使用更大的数据类型防止溢出，并提高精度
    uint32_t data_temp = (uint32_t)ecig_cfg.res_wire * 1000;
    uint32_t result = data_temp / 8192;
    uint16_t data_temp1 = 0;
    uint8_t data = 0;
    if(result<1000&&result>0)
    {
        // 对 result 进行四舍五入处理
        data_temp1 = (uint16_t)((result + 10) / 10);
        data = (uint8_t)((data_temp1 + 10) / 10);
    }
    else if(result>=1000)
    {
        // 对 result 进行四舍五入处理
        data_temp1 = (uint16_t)((result + 15) / 10);
        data = (uint8_t)((data_temp1 + 15) / 10);
    }

#if DEBUG
    printf("%s, data_temp1=%d,result=%lu\n", __func__,data_temp1, result);
    printf("%s, data=%d\n", __func__, data);
#endif
    if(data>=11 && data<=13)
        data =12;
    if(data>=7 && data<=9)
        data =8;
    if(ecig_cfg.smoke_position_swich == 0)
    {
        return data;
    }
    else
    {
        return data/2;
    }

//printf("%s, data=%u\n", __func__, data);  //
}


//单发
void test_1st_gear_func(void)
{
    printf("%s \n",__func__);
    uteModuleSmokeData.smoke_position_swich = 0;
    ecig_cfg.smoke_position_swich = 0;
}


void ecig_set_power(u16 value)
{
    uint8_t res = ecig_get_res();
    printf("get_gear_func()==%d && res== %d\n",get_gear_func(),res);
    if(get_gear_func()==0 && res== 8)
    {
        // ecig_cfg.aim_voltage = 2650 ;
        // uteModuleSmokeData.current_power=value;
        for (int i = 0; i < sizeof(power_voltage_singe_limi_0_8) / sizeof(power_voltage_singe_limi_0_8[0]); i++)
        {
            if (power_voltage_singe_limi_0_8[i].power == value)
            {
                ecig_cfg.aim_voltage = power_voltage_singe_limi_0_8[i].voltage;
                break;
            }
        }
    }
    else if(get_gear_func()==1 && res== 4)
    {
        for (int i = 0; i < sizeof(power_voltage_dual_limi_0_4) / sizeof(power_voltage_dual_limi_0_4[0]); i++)
        {
            if (power_voltage_dual_limi_0_4[i].power == value)
            {
                ecig_cfg.aim_voltage = power_voltage_dual_limi_0_4[i].voltage;
                break;
            }
        }
    }
    else if(get_gear_func()==0 && res== 12)
    {
        for (int i = 0; i < sizeof(power_voltage_singe_limi_1_2) / sizeof(power_voltage_singe_limi_1_2[0]); i++)
        {
            if (power_voltage_singe_limi_1_2[i].power == value)
            {

                ecig_cfg.aim_voltage = power_voltage_singe_limi_1_2[i].voltage;
                break;
            }
        }
    }
    else if(get_gear_func()==1 && res== 6)
    {
        for (int i = 0; i < sizeof(power_voltage_dual_limi_1_2) / sizeof(power_voltage_dual_limi_1_2[0]); i++)
        {
            if (power_voltage_dual_limi_1_2[i].power == value)
            {

                ecig_cfg.aim_voltage = power_voltage_dual_limi_1_2[i].voltage;
                break;
            }
        }
    }
    uteModuleSmokeData.current_power = value;
}

//双发
void test_2st_gear_func(void)
{
    printf("%s \n",__func__);
    uteModuleSmokeData.smoke_position_swich = 1;
    ecig_cfg.smoke_position_swich = 1;
}

//单发
u8 get_gear_func(void)
{
    ecig_cfg.smoke_position_swich = uteModuleSmokeData.smoke_position_swich;
    //  printf("%s ,%d\n",__func__,ecig_cfg.smoke_position_swich);
    return ecig_cfg.smoke_position_swich;

}
u8 get_gear_func1(void)
{
    // printf("%s \n",__func__);
    return uteModuleSmokeData.smoke_position_swich;

}

#else
void ecig_set_power(u8 value)
{

}

u8 ecig_get_power(void)
{
    return 0;
}

void ecig_set_res(u8 value)
{

}

u8 ecig_get_res(void)
{
    return 0;
}
u8 get_gear_func1(void)
{


}
u8 get_gear_func(void)
{


}
void test_1st_gear_func(void)
{

}
void test_2st_gear_func(void)
{

}
#endif
