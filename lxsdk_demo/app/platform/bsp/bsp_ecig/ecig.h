#ifndef _ECIG_H
#define _ECIG_H

#define ECIG_VBG_VOLTAGE             sys_trim.vbg_volt
#define ECIG_VBG_VOLTAGE_MULTIPLE    10                 //放大倍数

typedef enum
{
    IDLE = 0,
    SMOKING,
    SHORT_CIRCUIT,//短路
    OPEN_CIRCUIT,//拔出
    LOW_POWER,//低电量
    IN_DEVICE,//插入
    SMOKE_TIMEOUT,//超时
} SMOKE_STA;

typedef enum
{
    ECIG_PWM1_IO = 0,
    ECIG_PWM2_IO,
    ECIG_MIC_IO,
    ECIG_VEN_IO,
    ECIG_DET1_IO,
    ECIG_DET2_IO,
    ECIG_ADGND_IO,
    ECIG_MAX_IO,
} ECIG_IO;


#define ECIG_PWM1_ON()     ecig_pwm_set(1, 1)
#define ECIG_PWM1_OFF()    ecig_pwm_set(1, 0)
#define ECIG_PWM2_ON()     ecig_pwm_set(2, 1)
#define ECIG_PWM2_OFF()    ecig_pwm_set(2, 0)
#define ECIG_VEN_ON()      ecig_ven_set(1, 1)
#define ECIG_VEN_OFF()     ecig_ven_set(1, 0)



void ecig_hot_detect_init(void);
void ecig_hot_proc(void);
void ecig_hot_proc_vddio(void);
void ecig_res_proc(void);
void ecig_vbat_proc(void);

u32 saradc_vbat_get_calc_value(u32 vbat2, u32 bg, u32 vrtc_val, u32 vrtc_first);
u32 ecig_vbat_get(void);
void caculate_res(void);
void caculate_res2(void);
void ecig_adgnd_io_set_gnd(void);
void ecig_ven_io_init(void);
//烟控配置
typedef struct
{
    u8 io_pwm1;
    u8 io_mic;
    u8 io_adgnd;
    u8 adc1_ch;
    u8 adc_res1_ch;
    u8 adc2_en;                 //是否有ADC2通路
    u8 io_pwm2;
    u8 io_ven;
    u8 adc2_ch;
    u8 adc_res2_ch;
    u16 res_diff;               //阻抗检测分压电阻值（Ω，8192倍）
    u16 res_wire;               //无ADC2通路时电阻丝为固定阻值（Ω，8192倍）
    u16 res_wire2;

    u8 io_hot_det1;              //烟弹检测io
    u8 io_hot_det2;              //烟弹检测io
    u8 io_hot_mode;             //检测到是否开检测模式
    bool hot_det_flag;          //烟弹检测标志

    u8 aim_power;               //目标功率（W）
    u16 aim_voltage;             //目标电压
    u8 smoke_power;               //抽烟功率（W）
    u8 heat_time_max;           //单次加热最长时间（s）
    u8 short_res_prop;          //短路保护，电热丝阻值和MOS内阻（100mΩ）的最小比例，阻值相近乘10提高精度，10--0.1Ω视作短路
    u16 open_res_prop;          //开路保护，电热丝阻值和MOS内阻（100mΩ）的最大比例，阻值相近乘10提高精度，200--2Ω视作开路（无ADC2通路时有效）
    u8  short_res_proportion;   //电阻短路时adc比例
    u8  open_res_proportion;    //电阻断路时adc比例
    bool smoke_res_swich;       //一档时候，吸一口换一路
    u8 smoke_position_swich;    //一档:0x00二档:0x01三档:0x02
    gpio_t ecig_gpio[ECIG_MAX_IO];
    u32 smoking_count;                              //抽烟口数
    bool smoke_start_or_not;       //是否抽吸
} ecig_cfg_t;
extern ecig_cfg_t ecig_cfg;
typedef struct
{
    ecig_cfg_t *cfg;
    u32 AD_hot_voltage_mv;      //VAT（真实值）
    u32 AD_hot_voltage;         //VAT（定点值）
    u32 AD_hot_voltage_mv2;      //VAT（真实值）
    u32 AD_hot_voltage2;         //VAT（定点值）
    u32 AD_BAT_voltage_mv;      //VBAT
    u32 p_current;              //当前功率
    u32 p_prev;                 //前一次功率
    u8 power_on_flag;           //PWM开启标志位
    u8 hot_time_flag;           //开始8S计时标志
    u16 hot_time_cnt;           //PWM计时第几秒
    //u8 timer_100us_cnt;         //100us计时
    u16 timer_cycle_cnt;         //周期计数
    u8 timer_switch_adc_flag;
    u8 timer_switch_acc_cnt;
    u8 timer_big_cycles_10ms;        //检测大周期 10MS
    u16 timer_20ms_cnt;         //20msMIC滤波时间
    u16 timer_1s_cnt;           //1S MIC吸烟时间
    u16 cnt_1s;
    u8 short_circuit_flag;      //短路标志位
    u8 short_circuit_cnt;       //短路累计次数，超过10次算短路
    u8 mic_sta;                 //mic状态（20ms防抖)
    u8 mic_start;               //mic start信号，仅双路adc使用
    u16 hot_res;                //双路adc测量的电热丝阻值，仅双路adc使用
    u16 hot_res2;
    u32 adc1;
    u32 adc_res1;
    u32 adc2;
    u32 adc_res2;
    u8  smoke_sta;              //烟控状态
    u8 det1_sta;                 //det状态（20ms防抖）插拔检测
    u8 det2_sta;                 //det状态（20ms防抖）插拔检测
    u8 det1_start;               //det start信号
    u8 det2_start;               //det start信号
    u8 clock_flag;               //童锁标志位
    u8 poweroff_flag;               //关机标志位
} ecig_cb_t;

extern ecig_cb_t ecig;


void ecigarette_init(ecig_cfg_t* cfg);
void ecigarette_sleep_wakeup(ecig_cfg_t* cfg);
void ecigarette_exit(void);
bool ecig_is_working(void);
u8 ecig_is_working_sta(void);
void ecig_pwm_set(u8 pwm_num, bool pwm_on);
void ecig_ven_set(u8 pwm_num, bool pwm_on);
u32 calculate_power(u32 hot_voltage);
void ecig_adgnd_io_deinit(void);
uint8_t mic_start_or_not(void);
void ECIG_PWM_OFF_FUNC(void);
void ECIG_PWM_ON_FUNC(void);
#endif  //_ECIG_H
