

#ifndef _BSP_ECIG_H
#define _BSP_ECIG_H


#if (CHIP_PACKAGE_SELECT == CHIP_5690F)
#if (DEVELOPMENT_BOARD_TYPE != DEVELOPMENT_BOARD_USER)
#define ECIG_PWM1_GPIO  IO_PE5
#define ECIG_MIC_GPIO   IO_PB5
#define ECIG_PWM2_GPIO  IO_PE4
#define ECIG_GND_GPIO   IO_PG7
#define ECIG_ADC1_GPIO  ADCCH_PE7
#define ECIG_ADC2_GPIO  ADCCH_PE6
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PH7
#endif
#else
#define ECIG_PWM1_GPIO  IO_PB2
#define ECIG_MIC_GPIO   IO_PF3
#define ECIG_MIC_INT   PORT_INT6_VECTOR
#define ECIG_VEN_GPIO   IO_PE5
#define ECIG_PWM2_GPIO  IO_PF6
#define ECIG_GND_GPIO   IO_PG7
#define ECIG_ADC1_GPIO  ADCCH_PE7
#define ECIG_ADC_RES1_GPIO  ADCCH_PE6
#define ECIG_ADC2_GPIO  ADCCH_PF2
#define ECIG_ADC_RES2_GPIO  ADCCH_PF1
#if ECIG_DET_EN
#define ECIG_DET1_GPIO   IO_PE3
#define ECIG_DET1_INT    PORT_INT4_VECTOR
#define ECIG_DET2_GPIO   IO_PE2
#define ECIG_DET2_INT    PORT_INT5_VECTOR
#endif
#endif
#elif (CHIP_PACKAGE_SELECT == CHIP_5690G)
#define ECIG_PWM1_GPIO  IO_PE5
#define ECIG_MIC_GPIO   IO_PF3
#define ECIG_PWM2_GPIO  IO_PE4
#define ECIG_GND_GPIO   IO_PG7
#define ECIG_ADC1_GPIO  ADCCH_PE7
#define ECIG_ADC2_GPIO  ADCCH_PE6
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PH7
#endif
#elif (CHIP_PACKAGE_SELECT == CHIP_5691G)
#define ECIG_PWM1_GPIO  IO_PF1
#define ECIG_MIC_GPIO   IO_PF3
#define ECIG_PWM2_GPIO  IO_PF2
#define ECIG_GND_GPIO   IO_PB5
#define ECIG_ADC1_GPIO  ADCCH_PB4
#define ECIG_ADC2_GPIO  ADCCH_PB7
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PB6
#endif
#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
#define ECIG_PWM1_GPIO  IO_PE1
#define ECIG_MIC_GPIO   IO_PE3
#define ECIG_PWM2_GPIO  IO_PE2
#define ECIG_GND_GPIO   IO_PB5
#define ECIG_ADC1_GPIO  ADCCH_PB4
#define ECIG_ADC2_GPIO  ADCCH_PB7
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PB6
#endif
#else
#define ECIG_PWM1_GPIO  IO_PF1
#define ECIG_MIC_GPIO   IO_PF3
#define ECIG_PWM2_GPIO  IO_PF2
#define ECIG_GND_GPIO   IO_PB5
#define ECIG_ADC1_GPIO  ADCCH_PB4
#define ECIG_ADC2_GPIO  ADCCH_PB7
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PB6
#endif
#endif

#define ECIG_SET_VOLTAGE_DATA   2600 //2.60mv
#define bsp_ecig_exit()         ecigarette_exit()
#define bsp_ecig_is_working()   ecig_is_working()   //正在吸烟（mic高电平）

#define bsp_ecig_is_working_sta()   ecig_is_working_sta()   //正在吸烟各种状态

enum
{
    REMINDER_ECIG_SMOKING,  //吸烟
    REMINDER_ECIG_SHORT,    //短路
    REMINDER_ECIG_UP,       //拔出
    REMINDER_ECIG_IN,       //插入
};

void bsp_ecig_init(void);
void bsp_ecig_sleep_wakeup(void);
void ecig_set_power(u16 value);
u8 ecig_get_power(void);
void ecig_set_res(u8 value);
u8 ecig_get_res(void);
void ecig_set_res2(u8 value);
//一档
void test_1st_gear_func(void);
void test_2st_gear_func(void);
u8 get_gear_func(void);
u8 get_gear_func1(void);
#endif // _BSP_ECIG_H
