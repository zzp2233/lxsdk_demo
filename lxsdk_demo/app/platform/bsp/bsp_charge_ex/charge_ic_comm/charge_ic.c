#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)

typedef struct
{
    u32 charge_mode_tick;
    u32 comm_mode_tick;
} charge_ic_t;
charge_ic_t charge_ic;

static u8 charge_ic_mode = BOX_NULL_MODE;

//检查充电IC模式
AT(.text.func.idle.charge)
u8 charge_ic_work_mode(void)
{
    return charge_ic_mode;
}

bool charge_ic_eoc_check_en(void)
{
    bool sta;

    sta = (charge_ic.charge_mode_tick && tick_check_expire(charge_ic.charge_mode_tick, 1000)); //eoc检测文档要求至少需要400ms消抖时间

    return sta;
}

bool charge_ic_eoc_check_inbox(void)
{
    bool sta = false;

    sta = (charge_ic.charge_mode_tick && !tick_check_expire(charge_ic.charge_mode_tick, 2000)); //该时间段内满电可认为出仓

    return sta;
}

u32 charge_ic_comm_mode_tick(void)
{
    return charge_ic.comm_mode_tick;
}

void charge_ic_charge(u8 en)
{
#if CHARGE_ENABLE_GPIO
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, CHARGE_ENABLE_GPIO);

    TRACE("%s: en[%d]\n", __func__, en);

    if (gpio.sfr)
    {
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);     //digital
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);   //as GPIO
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);   //output
        if (en)
        {
            gpio.sfr[GPIOxSET] = BIT(gpio.num); //VOUT_EN开
        }
        else
        {
            gpio.sfr[GPIOxCLR] = BIT(gpio.num); //VOUT_EN关
        }
    }
#endif // CHARGE_ENABLE_GPIO
}

AT(.text.func.idle.charge)
bool charge_ic_work_mode_set(u8 mode)
{
#if !HALL_EN
    if (charge_ic_mode == mode)
    {
//        TRACE("charge_ic_mode no change[%d]\n", mode);
        return false;
    }
#endif

    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, CHARGE_CTRL_GPIO);
    if (!gpio.sfr)
    {
        return false;
    }

    charge_ic_mode = mode;
    charge_ic.charge_mode_tick = 0;
    charge_ic.comm_mode_tick = 0;

    TRACE("%s: mode[%d]\n", __func__, mode);

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    gpio.sfr[GPIOxDE] |= BIT(gpio.num);
    gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
    gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
    switch (mode)
    {
        case BOX_CHARGE_MODE:
            smart_vhouse_tx_dis();
            gpio.sfr[GPIOxCLR] = BIT(gpio.num); //SY_EN开
            charge_ic_charge(1);
            charge_ic.charge_mode_tick = tick_get();
            break;

        case BOX_COMM_MODE:
            gpio.sfr[GPIOxCLR] = BIT(gpio.num); //SY_EN开
            charge_ic_charge(0);
            charge_ic.comm_mode_tick = tick_get();
            smart_vhouse_tx_en();
            break;

        case BOX_CHK_MODE:
            gpio.sfr[GPIOxSET] = BIT(gpio.num); //SY_EN关
            charge_ic_charge(0);
//        delay_5ms(2);
            smart_vhouse_tx_dis();
            break;
    }
#endif

    return true;
}

void charge_ic_process(void)
{
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    static u32 tick500 = 0;
//    static u8 tick500_cnt = 0;
    if (tick_check_expire(tick500, 500))
    {
        tick500 = tick_get();

        bool chg = 0, pgd = 0;
        gpio_t gpio;
        bsp_gpio_cfg_init(&gpio, CHARGE_SDA_GPIO);
        if (gpio.sfr)
        {
            chg = pgd = (gpio.sfr[GPIOx] & BIT(gpio.num)) >> gpio.num;
        }

        if (pgd != charge_ex_cb.pgd)                    //拔插USB
        {
            bsp_charge_ex_usb_insert_callback(pgd);
            TRACE("pgd[%d] DC_IN[%x]\n", pgd, ((RTCCON >> 20) & 0x01));
        }
        charge_ex_cb.chg = chg;
        charge_ex_cb.pgd = pgd;
    }
#endif
}

void charge_ic_sda_init(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, CHARGE_SDA_GPIO);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);    // 0:当作通用GPIO使用
        gpio.sfr[GPIOxDE]  |= BIT(gpio.num);     // 数字IO使能: 0为模拟IO, 1 为数字IO,
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);     // 控制IO的方向:  0为输出, 1为输入.
#if (CHARGE_EX_IC_SELECT != CHARGE_IC_SY8827)    // sy8827方案，SDA_GPIO外部下拉当充电检测用（0：拔出 1：插入）
        gpio.sfr[GPIOxPU]  |= BIT(gpio.num);
#endif
    }
}

void charge_ic_ctrl_init(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, CHARGE_CTRL_GPIO);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);    //0:当作通用GPIO使用
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);     //数字IO使能: 0为模拟IO, 1 为数字IO,
    }
}

void charge_ic_init(void)
{
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    printf("sy8827_init\n");
//    saradc_set_channel(BIT(ADCCH_PB2) | BIT(ADCCH_PB4));//硬件eoc检测
#endif
    charge_ic_sda_init();
    charge_ic_ctrl_init();
    charge_ic_charge(0);

    memset(&charge_ic, 0, sizeof(charge_ic_t));
#if HALL_EN
    charge_ic_work_mode_set(BOX_COMM_MODE);
#else
    charge_ic_work_mode_set(BOX_CHARGE_MODE);
#endif
}

#endif  //(CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
