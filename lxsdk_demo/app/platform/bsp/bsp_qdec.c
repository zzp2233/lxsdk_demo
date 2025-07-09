#include "include.h"

#if USER_KEY_QDEC_EN

AT(.com_text.qdec)
void qdec_key_msg_enqueue(u8 msg)
{
    reset_sleep_delay_all();        //复位休眠计数
    msg_enqueue(msg);
}

/*
*   旋转编码器Quadrate Decode
*   QDEC_MAP_G1: A -> PB0, B -> PB1
*   QDEC_MAP_G2: A -> PE1, B -> PE2
*   QDEC_MAP_G3: A -> PE6, B -> PE7
*/

AT(.com_text.qdec)
void qdec_isr(void)
{
    u32 qdeccon = QDECCON;
    if (qdeccon & BIT(1))                   //interrupt is enbale?
    {
        if (qdeccon & BIT(30))              //forward
        {
            QDECCPND = BIT(30);
            qdec_key_msg_enqueue(MSG_QDEC_FORWARD);
        }
        if (qdeccon & BIT(31))              //reverse
        {
            QDECCPND = BIT(31);
            qdec_key_msg_enqueue(MSG_QDEC_BACKWARD);
        }
    }
}

void bsp_qdec_init(void)
{
    CLKGAT2 |= BIT(2);                      //qdec clk enable
#if (USER_QDEC_MAPPING == QDEC_MAP_G1)
    GPIOBDIR |= 0x03;
    GPIOBDE |= 0x03;
    GPIOBFEN |= 0x03;
    GPIOBPU |= 0x03;
#elif (USER_QDEC_MAPPING == QDEC_MAP_G2)
    GPIOEDIR |= 0x06;
    GPIOEDE |= 0x06;
    GPIOEFEN |= 0x06;
    GPIOEPU |= 0x06;
#elif (USER_QDEC_MAPPING == QDEC_MAP_G3)
    GPIOEDIR |= 0xc0;
    GPIOEDE |= 0xc0;
    GPIOEFEN |= 0xc0;
    GPIOEPU |= 0xc0;
#endif
    sys_irq_init(IRQ_QDEC_VECTOR, 0, qdec_isr);
    FUNCMCON2 = USER_QDEC_MAPPING;
    QDECCON = 45 << 3;                      //quadrate decode filter length
    QDECCPND = BIT(30) | BIT(31);
    QDECCON = BIT(0) | BIT(1);              //qdec decode enable, interrupt enable
}

#elif USER_ADKEY_QDEC_EN

/*
*   QDEC_A串接5.1K，QEC_B串接10K，接到同一IO上。IO开内部10K上拉，用ADC来采电压。
*   正旋：3.3v -- 1.113v -- 0.825v -- 1.65v -- 3.3v
*   正旋: AH_BH - AL_BH -- AL_BL -- AH_BL -- AH_BH
*   反旋：3.3v -- 1.65v -- 0.825v -- 1.113v -- 3.3v
*   反旋：AH_BH - AH_BL -- AL_BL -- AL_BH -- AH_BH
*/

enum
{
    QEC_AH_BH,                              //A输出高电平，B输出高电平，空闲状态
    QEC_AL_BH,                              //A输出低电平，B输出高电平
    QEC_AL_BL,                              //A输出低电平，B输出低电平
    QEC_AH_BL,                              //A输出高电平，B输出低电平
};

typedef struct
{
    u8 status[3];
    u8 cnt;
    u8 pre_sta;
} qdec_cb_t;
static qdec_cb_t qdec_cb;

AT(.com_rodata.pwrkey.table)
const adkey_tbl_t qdec_adkey_table[6] =
{
    {0x30, QEC_AH_BH},
    {0x4C, QEC_AL_BL},
    {0x6A, QEC_AL_BH},
    {0x8A, QEC_AH_BL},
    {0xFF, QEC_AH_BH},
};

void bsp_qdec_init(void)
{
    u8 io_num = get_adc_gpio_num(USER_QDEC_ADCH);

    memset(&qdec_cb, 0, sizeof(qdec_cb_t));
    if (io_num != IO_NONE)
    {
        gpio_t gpio;
        gpio_cfg_init(&gpio, io_num);
        gpio.sfr[GPIOxDE] |= gpio.pin;
        gpio.sfr[GPIOxDIR] |= gpio.pin;
        gpio.sfr[GPIOxPU] |= gpio.pin;
        saradc_set_channel(BIT(USER_QDEC_ADCH));
    }
}

AT(.com_text.qdec)
void qdec_key_msg_enqueue(u8 msg)
{
    reset_sleep_delay_all();        //复位休眠计数
    msg_enqueue(msg);
}

//每毫秒ADC采样值进行处理
AT(.com_text.qdec)
void bsp_qdec_adc_process(u8 adc_val)
{
    u8 num = 0;
    u8 qdec_cur_sta;

    while ((u8)adc_val > qdec_adkey_table[num].adc_val)
    {
        num++;
    }
    qdec_cur_sta = qdec_adkey_table[num].usage_id;
    if (qdec_cur_sta != qdec_cb.pre_sta)
    {
        if (qdec_cur_sta != QEC_AH_BH)
        {
            if (qdec_cb.pre_sta == QEC_AH_BH)
            {
                qdec_cb.cnt = 0;
            }
            if (qdec_cb.cnt < 3)
            {
                qdec_cb.status[qdec_cb.cnt++] = qdec_cur_sta;
            }
        }
        qdec_cb.pre_sta = qdec_cur_sta;
    }
    else
    {
        if (qdec_cur_sta == QEC_AH_BH)                  //结束
        {

#if !USER_ADKEY_QDEC_NO_STD
            if (qdec_cb.cnt == 3 && qdec_cb.status[1] == QEC_AL_BL)
            {
                if (qdec_cb.status[0] == QEC_AL_BH && qdec_cb.status[2] == QEC_AH_BL)
                {
                    qdec_key_msg_enqueue(MSG_QDEC_FORWARD);
//                    uart_putchar('F');
                }
                else if (qdec_cb.status[2] == QEC_AL_BH && qdec_cb.status[0] == QEC_AH_BL)
                {
                    qdec_key_msg_enqueue(MSG_QDEC_BACKWARD);
//                    uart_putchar('R');
                }
            }
#else
            if (qdec_cb.cnt >= 2)
            {
                if (qdec_cb.status[0] == QEC_AL_BH)
                {
                    if (qdec_cb.status[1] == QEC_AL_BL)
                    {
                        qdec_key_msg_enqueue(MSG_QDEC_FORWARD);
//                        uart_putchar('F');
                    }
                }
                else if (qdec_cb.status[0] == QEC_AH_BL)
                {
                    if (qdec_cb.status[1] == QEC_AL_BL)
                    {
                        qdec_key_msg_enqueue(MSG_QDEC_BACKWARD);
//                        uart_putchar('R');
                    }
                }
            }
#endif
            memset(&qdec_cb, 0, sizeof(qdec_cb_t));
        }
    }
}

#elif USER_ADKEY_QDEC_EN

/*
*   QDEC_A串接5.1K，QEC_B串接10K，接到同一IO上。IO开内部10K上拉，用ADC来采电压。
*   正旋：3.3v -- 1.113v -- 0.825v -- 1.65v -- 3.3v
*   正旋: AH_BH - AL_BH -- AL_BL -- AH_BL -- AH_BH
*   反旋：3.3v -- 1.65v -- 0.825v -- 1.113v -- 3.3v
*   反旋：AH_BH - AH_BL -- AL_BL -- AL_BH -- AH_BH
*/

enum
{
    QEC_AH_BH,                              //A输出高电平，B输出高电平，空闲状态
    QEC_AL_BH,                              //A输出低电平，B输出高电平
    QEC_AL_BL,                              //A输出低电平，B输出低电平
    QEC_AH_BL,                              //A输出高电平，B输出低电平
};

typedef struct
{
    u8 status[3];
    u8 cnt;
    u8 pre_sta;
} qdec_cb_t;
static qdec_cb_t qdec_cb;

AT(.com_rodata.pwrkey.table)
const adkey_tbl_t qdec_adkey_table[6] =
{
    {0x30, QEC_AH_BH},
    {0x4C, QEC_AL_BL},
    {0x6A, QEC_AL_BH},
    {0x8A, QEC_AH_BL},
    {0xFF, QEC_AH_BH},
};

void bsp_qdec_init(void)
{
    u8 io_num = get_adc_gpio_num(USER_QDEC_ADCH);

    memset(&qdec_cb, 0, sizeof(qdec_cb_t));
    if (io_num != IO_NONE)
    {
        gpio_t gpio;
        gpio_cfg_init(&gpio, io_num);
        gpio.sfr[GPIOxDE] |= gpio.pin;
        gpio.sfr[GPIOxDIR] |= gpio.pin;
        gpio.sfr[GPIOxPU] |= gpio.pin;
        saradc_set_channel(BIT(USER_QDEC_ADCH));
    }
}

AT(.com_text.qdec)
void qdec_key_msg_enqueue(u8 msg)
{
    reset_sleep_delay_all();        //复位休眠计数
    msg_enqueue(msg);
}

//每毫秒ADC采样值进行处理
AT(.com_text.qdec)
void bsp_qdec_adc_process(u8 adc_val)
{
    u8 num = 0;
    u8 qdec_cur_sta;

    while ((u8)adc_val > qdec_adkey_table[num].adc_val)
    {
        num++;
    }

    qdec_cur_sta = qdec_adkey_table[num].usage_id;
    if (qdec_cur_sta != qdec_cb.pre_sta)
    {
        if (qdec_cur_sta != QEC_AH_BH)
        {
            if (qdec_cb.pre_sta == QEC_AH_BH)
            {
                qdec_cb.cnt = 0;
            }
            if (qdec_cb.cnt < 3)
            {
                qdec_cb.status[qdec_cb.cnt++] = qdec_cur_sta;
            }
        }
        qdec_cb.pre_sta = qdec_cur_sta;
    }
    else
    {
        if (qdec_cur_sta == QEC_AH_BH)                  //结束
        {

#if !USER_ADKEY_QDEC_NO_STD
            if (qdec_cb.cnt == 3 && qdec_cb.status[1] == QEC_AL_BL)
            {
                if (qdec_cb.status[0] == QEC_AL_BH && qdec_cb.status[2] == QEC_AH_BL)
                {
                    qdec_key_msg_enqueue(MSG_QDEC_FORWARD);
//                    uart_putchar('F');
                }
                else if (qdec_cb.status[2] == QEC_AL_BH && qdec_cb.status[0] == QEC_AH_BL)
                {
                    qdec_key_msg_enqueue(MSG_QDEC_BACKWARD);
//                    uart_putchar('R');
                }
            }
#else
            if (qdec_cb.cnt >= 2)
            {
                if (qdec_cb.status[0] == QEC_AL_BH)
                {
                    if (qdec_cb.status[1] == QEC_AL_BL)
                    {
                        qdec_key_msg_enqueue(MSG_QDEC_FORWARD);
//                        uart_putchar('F');
                    }
                }
                else if (qdec_cb.status[0] == QEC_AH_BL)
                {
                    if (qdec_cb.status[1] == QEC_AL_BL)
                    {
                        qdec_key_msg_enqueue(MSG_QDEC_BACKWARD);
//                        uart_putchar('R');
                    }
                }
            }
#endif
            memset(&qdec_cb, 0, sizeof(qdec_cb_t));
        }
    }
}
#elif USER_IO_QEDC_EN
static u8 wheel_port_last_state;
static gpio_t qdec_a, qdec_b;

AT(.com_text.qdec)
static void qdec_key_msg_enqueue(u8 msg)
{
    reset_sleep_delay_all();        //复位休眠计数
    msg_enqueue(msg);
}

void bsp_qdec_init(void)
{
    printf("bsp_qdec_init\n");
    bsp_gpio_cfg_init(&qdec_a, QEDC_IO_A);
    if (qdec_a.sfr == NULL)
    {
        return;
    }

    qdec_a.sfr[GPIOxDE] |= BIT(qdec_a.num);
    qdec_a.sfr[GPIOxDIR] |= BIT(qdec_a.num);
    qdec_a.sfr[GPIOxPU] |= BIT(qdec_a.num);

    bsp_gpio_cfg_init(&qdec_b, QEDC_IO_B);
    if (qdec_b.sfr == NULL)
    {
        return;
    }

    qdec_b.sfr[GPIOxDE] |= BIT(qdec_b.num);
    qdec_b.sfr[GPIOxDIR] |= BIT(qdec_b.num);
    qdec_b.sfr[GPIOxPU] |= BIT(qdec_b.num);

    wheel_port_last_state = (qdec_a.sfr[GPIOx] & BIT(qdec_a.num)) ? BIT(6) : 0;
    wheel_port_last_state |= (qdec_b.sfr[GPIOx] & BIT(qdec_b.num)) ?BIT(4) : 0;
}

#define  WHEEL_LOG       0      //特征值打印,可调试半码和全码用,勿删！

#if WHEEL_LOG
AT(.com_rodata.dbg)
char deg_log[]="qdec:0x%x,0x%x\n";
char deg_send[] = "QDEC_MSG_F(%d)\n";
#endif

AT(.com_text.qdec)
void bsp_qdec_io_process(void)
{
    static u8 up_cnt = 0, dn_cnt = 0;
    u8 wheel_z = 0;
#if WHEEL_LOG
    static u8 wheel_z_bk, wheel_port_last_state_bk;
#endif

    wheel_z = (qdec_a.sfr[GPIOx] & BIT(qdec_a.num)) ? BIT(6) : 0;
    wheel_z |= (qdec_b.sfr[GPIOx] & BIT(qdec_b.num)) ?BIT(4) : 0;

#if WHEEL_LOG
    if((wheel_z_bk != wheel_z) || (wheel_port_last_state_bk != wheel_port_last_state))
    {
        wheel_z_bk = wheel_z;
        wheel_port_last_state_bk = wheel_port_last_state;
        printf(deg_log, wheel_port_last_state, wheel_z);
    }
#endif

    if((wheel_port_last_state == 0x50 && wheel_z == 0x40)
       // || (wheel_port_last_state == 0x10 && wheel_z == 0x50)
      )
    {
        dn_cnt = 0;
        up_cnt++;
        if(up_cnt >= 1)     //2
        {
            up_cnt = 0;
            qdec_key_msg_enqueue(MSG_QDEC_FORWARD);
#if WHEEL_LOG
            printf(deg_send,1);
#endif
        }
    }
    else if((wheel_port_last_state == 0x50 && wheel_z == 0x10)
            //   ||(wheel_port_last_state == 0x40 && wheel_z == 0x50)
           )
    {
        up_cnt = 0;
        dn_cnt++;
        if(dn_cnt >= 1)     //2
        {
            dn_cnt = 0;
            qdec_key_msg_enqueue(MSG_QDEC_BACKWARD);
#if WHEEL_LOG
            printf(deg_send,0);
#endif
        }
    }
    wheel_port_last_state = wheel_z;
}

#else
void bsp_qdec_init(void)
{

}
#endif // USER_ADKEY_QDEC_EN
