#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C || CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)

#define CAPTURE_DEBUG_EN    0

#if CAPTURE_DEBUG_EN
AT(.com_rodata.isr)
const char str_1[] = "#A-->%d\n";
AT(.com_rodata.isr)
const char str_2[] = "#B-->%d\n";
AT(.com_rodata.isr)
const char str_3[] = "#C-->%d\n";
AT(.com_rodata.isr)
const char str_4[] = "#D-->%d\n";
AT(.com_rodata.isr)
const char str_5[] = "#E-->%d\n";
AT(.com_rodata.isr)
const char str_6[] = "#F-->%d\n";
AT(.com_rodata.isr)
const char T_CMD[] = "#cap->%x\n";
#endif

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C)
const char str_chip[] = "lp7812c";
#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
const char str_chip[] = "lx3318bs";
#endif

//脉冲周期1ms
enum
{
    CHARGE_STA_PRE = 0,
    CHARGE_STA_PRE_START1,    //1ms      包头       1个低电平
    CHARGE_STA_PRE_START2,    //1ms      包头       1个高电平
    CHARGE_STA_START,         //10ms     包头      10个低电平
    CHARGE_STA_DATA0,         //19ms     数据      19个电平周期，19个bit
    CHARGE_STA_DATA1,         //19ms     数据      19个电平周期，19个bit
    CHARGE_STA_STOP,          //1ms      包尾       1个高电平
};

typedef struct
{
    u32 charge_mode_tick;           //进充电模式的时间
    u32 comm_mode_tick;             //进通讯模式的时间
} lp7812c_t;
lp7812c_t lp7812c;

BIT16_Typedef Byte_BIT16;
static u8 lp7812c_mode = BOX_NULL_MODE;
static u32 charge_ic_data;

//检查充电IC模式
AT(.text.func.idle.charge)
u8 lp7812c_work_mode(void)
{
    return lp7812c_mode;
}

bool lp7812c_eoc_check_en(void)
{
    bool sta;

    sta = (lp7812c.charge_mode_tick && tick_check_expire(lp7812c.charge_mode_tick, 1000)); //eoc检测文档要求至少需要400ms消抖时间

    return sta;
}

bool lp7812c_eoc_check_inbox(void)
{
    bool sta;

    sta = (lp7812c.charge_mode_tick && !tick_check_expire(lp7812c.charge_mode_tick, 2000) && !tick_check_expire(hall_cb.charge_start_tick, 2500)); //该时间段内满电可认为出仓

    return sta;
}

u32 lp7812c_comm_mode_tick(void)
{
    return lp7812c.comm_mode_tick;
}

void lp7812c_charge(u8 en)
{
#if CHARGE_ENABLE_GPIO
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, CHARGE_ENABLE_GPIO);

//    TRACE("%s_charge: en[%d]\n", str_chip, en);

    if (gpio.sfr)
    {
        if(en)
        {
            gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);
        }
        else
        {
            gpio.sfr[GPIOxDE] |= BIT(gpio.num);
            gpio.sfr[GPIOx] |= BIT(gpio.num);
        }
    }
#endif // CHARGE_ENABLE_GPIO
}

AT(.text.func.idle.charge)
bool lp7812c_work_mode_set(u8 mode)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, CHARGE_CTRL_GPIO);
    if (!gpio.sfr)
    {
        return false;
    }

    lp7812c_mode = mode;
    lp7812c.charge_mode_tick = 0;
    lp7812c.comm_mode_tick = 0;

    TRACE("%s_work_mode_set: %d\n", str_chip, mode);

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C)
    switch (mode)
    {

        case BOX_CHARGE_MODE:
            gpio.sfr[GPIOxDE] |= BIT(gpio.num);
            gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);   //控制IO的方向:  0为输出, 1为输入.
            gpio.sfr[GPIOx] |= BIT(gpio.num);
            lp7812c.charge_mode_tick = tick_get();
            smart_vhouse_tx_dis();
            break;

        case BOX_COMM_MODE:
            gpio.sfr[GPIOxDE] |= BIT(gpio.num);
            gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);   //控制IO的方向:  0为输出, 1为输入.
            gpio.sfr[GPIOx] &= ~BIT(gpio.num);
            lp7812c.comm_mode_tick = tick_get();
            smart_vhouse_tx_en();
            break;

        case BOX_CHK_MODE:
            gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);
            gpio.sfr[GPIOxDIR] |= BIT(gpio.num);   //控制IO的方向:  0为输出, 1为输入.
            gpio.sfr[GPIOxPU] &= ~BIT(gpio.num);
            gpio.sfr[GPIOxPU200K] &= ~BIT(gpio.num);
            gpio.sfr[GPIOxPU300] &= ~BIT(gpio.num);
            gpio.sfr[GPIOxPD] &= ~BIT(gpio.num);
            gpio.sfr[GPIOxPD200K] &= ~BIT(gpio.num);
            gpio.sfr[GPIOxPD300] &= ~BIT(gpio.num);
            smart_vhouse_tx_dis();
            break;
    }

#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
    gpio.sfr[GPIOxPU200K] &= ~BIT(gpio.num);
    gpio.sfr[GPIOxPU300]  &= ~BIT(gpio.num);
    gpio.sfr[GPIOxPD200K] &= ~BIT(gpio.num);
    gpio.sfr[GPIOxPD300]  &= ~BIT(gpio.num);
    gpio.sfr[GPIOxPU]     &= ~BIT(gpio.num);
    gpio.sfr[GPIOxPD]     &= ~BIT(gpio.num);
    gpio.sfr[GPIOxDE]     |= BIT(gpio.num);

    switch (mode)
    {
        case BOX_CHARGE_MODE:
            gpio.sfr[GPIOxDIR] |= BIT(gpio.num);   //控制IO的方向:  0为输出, 1为输入.
            gpio.sfr[GPIOxPU]  |= BIT(gpio.num);
            gpio.sfr[GPIOxPD]  |= BIT(gpio.num);
            lp7812c.charge_mode_tick = tick_get();
            smart_vhouse_tx_dis();
            break;

        case BOX_COMM_MODE:
            gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);   //控制IO的方向:  0为输出, 1为输入.
            gpio.sfr[GPIOx]    |= BIT(gpio.num);
            lp7812c.comm_mode_tick = tick_get();
            smart_vhouse_tx_en();
            break;

        case BOX_CHK_MODE:
            gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);   //控制IO的方向:  0为输出, 1为输入.
            gpio.sfr[GPIOx]    &= ~BIT(gpio.num);
            delay_5ms(2);
            smart_vhouse_tx_dis();
            break;
    }
#endif

    return true;
}

uint8_t CRC5_Cal(uint32_t source)
{
    u8 i, buf[5];
    uint16_t res;
    Byte_BIT16.byte = 0;
    source = source & 0x7FFF;
    for (i = 0; i < 15; i++)
    {
        res = ((source >> (14 - i)) & 0x01) << i;
        Byte_BIT16.byte |= res;
    }

    buf[0] = (1 ^ 1 ^ 1 ^ 1 ^ Byte_BIT16.fields.B0 ^ Byte_BIT16.fields.B4 ^ Byte_BIT16.fields.B5 ^ Byte_BIT16.fields.B8 ^ Byte_BIT16.fields.B10 ^ Byte_BIT16.fields.B12 ^ Byte_BIT16.fields.B13 ^ Byte_BIT16.fields.B14);
    buf[1] = (1 ^ 1 ^ 1 ^ Byte_BIT16.fields.B0 ^ Byte_BIT16.fields.B1 ^ Byte_BIT16.fields.B4 ^ Byte_BIT16.fields.B6 ^ Byte_BIT16.fields.B8 ^ Byte_BIT16.fields.B9 ^ Byte_BIT16.fields.B10 ^ Byte_BIT16.fields.B11 ^ Byte_BIT16.fields.B12);
    buf[2] = (1 ^ 1 ^ 1 ^ 1 ^ Byte_BIT16.fields.B1 ^ Byte_BIT16.fields.B2 ^ Byte_BIT16.fields.B5 ^ Byte_BIT16.fields.B7 ^ Byte_BIT16.fields.B9 ^ Byte_BIT16.fields.B10 ^ Byte_BIT16.fields.B11 ^ Byte_BIT16.fields.B12 ^ Byte_BIT16.fields.B13);
    buf[3] = (1 ^ 1 ^ 1 ^ 1 ^ 1 ^ Byte_BIT16.fields.B2 ^ Byte_BIT16.fields.B3 ^ Byte_BIT16.fields.B6 ^ Byte_BIT16.fields.B8 ^ Byte_BIT16.fields.B10 ^ Byte_BIT16.fields.B11 ^ Byte_BIT16.fields.B12 ^ Byte_BIT16.fields.B13 ^ Byte_BIT16.fields.B14);
    buf[4] = (1 ^ 1 ^ 1 ^ 1 ^ Byte_BIT16.fields.B3 ^ Byte_BIT16.fields.B4 ^ Byte_BIT16.fields.B7 ^ Byte_BIT16.fields.B9 ^ Byte_BIT16.fields.B11 ^ Byte_BIT16.fields.B12 ^ Byte_BIT16.fields.B13 ^ Byte_BIT16.fields.B14);

    return (buf[4] << 4) | (buf[3] << 3) | (buf[2] << 2) | (buf[1] << 1) | (buf[0] << 0);
}

void lp7812c_sda_parse(void)
{
    u32 cap_val;
    u8 crc_result;
    cap_val = charge_ic_data;
    crc_result = CRC5_Cal(cap_val);

    if (crc_result == cap_val >> 15)
    {
        bool chg = cap_val & 0x1;
        bool pgd = (cap_val >> 1) & 0x1;
        bool l_inbox = (cap_val >> 2) & 0x1;
        bool l_eoc =  (cap_val >> 3) & 0x1;
        bool r_inbox = (cap_val >> 4) & 0x1;
        bool r_eoc =  (cap_val >> 5) & 0x1;

        u8 ntc = (cap_val >> 6) & 0x7;
        u16 v_batm = (((cap_val >> 9) & 0x1f) * 50) + 2900;

        if (pgd != charge_ex_cb.pgd)                    //拔插USB
        {
            bsp_charge_ex_usb_insert_callback(pgd);
        }

        charge_ex_cb.chg = chg;
        charge_ex_cb.pgd = pgd;
        charge_ex_cb.ntc = ntc;
        charge_ex_cb.l_inbox = l_inbox;
        charge_ex_cb.r_inbox = r_inbox;
        bsp_charge_ex_vbat_update_callback(v_batm);
        // printf(",r_inbox=%d r_eoc %d\n",r_inbox,r_eoc);

//        TRACE("7812: vbat[%d]\n",v_batm);
//        TRACE("crc[%x] cap[%x] mode[%d]\n", crc_result, cap_val, lp7812c_work_mode());
//        TRACE("ntc[%x]\n", charge_ex_cb.ntc);
//        TRACE("l[%d] l_eoc[%d] r[%d] r_eoc[%d]\n",
//              (cap_val >> 2) & 0x1, (cap_val >> 3) & 0x1, (cap_val >> 4) & 0x1, (cap_val >> 5) & 0x1);

        if (lp7812c_work_mode() == BOX_CHK_MODE)
        {
            // printf("BOX_CHK_MODE>>> 0\n");
            bsp_charge_ex_inbox_callback(((cap_val >> 2) & 0x1), ((cap_val >> 4) & 0x1));
        }
        else if (lp7812c_work_mode() == BOX_CHARGE_MODE)
        {
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
            bsp_charge_ex_inbox_callback(((cap_val >> 2) & 0x1), ((cap_val >> 4) & 0x1));   //3318在充电模式也能判断inbox
#endif
            bsp_charge_ex_eoc_callback(((cap_val >> 3) & 0x1), ((cap_val >> 5) & 0x1));
        }

    }
    else
    {
        // printf("crc_error: %x %x\n", crc_result, cap_val);
        return;
    }
}

AT(.com_text.timer)
void capture_pluse(u8 new_sta)
{
    static u32 time_cnt = 0;
    static u8 prev_sta = 0, charge_sta = 0;
    static u32 cap_val = 0;
    static u8 bit_19_counter = 0;

    if(prev_sta != new_sta)
    {
        prev_sta = new_sta;
        if(charge_sta == CHARGE_STA_PRE && time_cnt > 16 && time_cnt < 50)
        {
            charge_sta = CHARGE_STA_PRE_START1;
#if CAPTURE_DEBUG_EN
            TRACE(str_1,time_cnt);
#endif
        }
        else if(charge_sta == CHARGE_STA_PRE_START1 && time_cnt > 16 && time_cnt < 50)
        {
            charge_sta = CHARGE_STA_PRE_START2;
#if CAPTURE_DEBUG_EN
            TRACE(str_2,time_cnt);
#endif
        }
        else if(charge_sta == CHARGE_STA_PRE_START2 && time_cnt > 266 && time_cnt < 400)
        {
            charge_sta = CHARGE_STA_START;
#if CAPTURE_DEBUG_EN
            TRACE(str_3,time_cnt);
#endif
        }
        else if(charge_sta == CHARGE_STA_START && bit_19_counter <= 19)
        {
            if(time_cnt > 23 && time_cnt < 33)
            {
                charge_sta = CHARGE_STA_DATA1;
            }
            else if(time_cnt > 0 && time_cnt < 5)
            {
                charge_sta = CHARGE_STA_DATA0;
            }
#if CAPTURE_DEBUG_EN
            TRACE(str_4,time_cnt);
#endif
        }
        else if(charge_sta == CHARGE_STA_DATA0 && time_cnt > 23 && time_cnt < 33)
        {
            charge_sta = CHARGE_STA_START;
            cap_val &= ~BIT(bit_19_counter);
            bit_19_counter++;
#if CAPTURE_DEBUG_EN
            TRACE(str_5,time_cnt);
#endif
        }
        else if(charge_sta == CHARGE_STA_DATA1 && time_cnt > 0 && time_cnt < 5)
        {
            charge_sta = CHARGE_STA_START;
            cap_val |= BIT(bit_19_counter);
            bit_19_counter++;
#if CAPTURE_DEBUG_EN
            TRACE(str_6,time_cnt);
#endif
        }

        if(bit_19_counter > 19)
        {
            charge_sta = CHARGE_STA_STOP;
            charge_ic_data = cap_val;
#if CAPTURE_DEBUG_EN
            TRACE(T_CMD, cap_val);                      //打印数据
#endif
        }
        time_cnt = 0;
    }
    else
    {
        time_cnt++;
        if(time_cnt > 666)
        {
            charge_sta = CHARGE_STA_PRE;
            cap_val = 0;
            bit_19_counter = 0;
        }
    }
}

static bool io_test_flag = false;
AT(.com_rodata.isr)
const char io_test_str_1[] = "io\n";
AT(.com_text.isr)
void timer3_isr(void)
{
    // if (TMR4CON & BIT(16)) {
    //     TMR4CPND = BIT(16);              //Clear Pending
    // }
    // //PB4
    // if(io_test_flag){
    //     io_test_flag = false;
    //     GPIOBSET = BIT(4);
    // } else {
    //     io_test_flag = true;
    //     GPIOBCLR = BIT(4);
    // }
    //printf(io_test_str_1);
    //这里搞个io拉高拉低，看看频率对不对。逻辑分析仪看看
    capture_pluse(SDA_STATUS());
}

void lp7812c_process(void)
{
    lp7812c_sda_parse();
}

void lp7812c_sda_init(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, CHARGE_SDA_GPIO);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);    // 0:当作通用GPIO使用
        gpio.sfr[GPIOxDE]  |= BIT(gpio.num);     // 数字IO使能: 0为模拟IO, 1 为数字IO,
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);     // 控制IO的方向:  0为输出, 1为输入.
        gpio.sfr[GPIOxPU]  |= BIT(gpio.num);
    }
}

void lp7812c_ctrl_init(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, CHARGE_CTRL_GPIO);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);    //0:当作通用GPIO使用
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);     //数字IO使能: 0为模拟IO, 1 为数字IO,
    }
}

void lp7812c_en_init(void)
{
#if CHARGE_ENABLE_GPIO
    gpio_t gpio;

    bsp_gpio_cfg_init(&gpio, CHARGE_ENABLE_GPIO);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);    //0:当作通用GPIO使用
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);   //控制IO的方向:  0为输出, 1为输入.
    }
#endif // CHARGE_ENABLE_GPIO
}

void lp7812c_init(void)
{
    printf("%s_init\n", str_chip);

    lp7812c_sda_init();
    lp7812c_ctrl_init();
    lp7812c_en_init();

    memset(&lp7812c, 0, sizeof(lp7812c_t));
    lp7812c_work_mode_set(BOX_CHK_MODE);
//    port_gpio_set_out(IO_PB4,1);
    bsp_hw_timer_set(HW_TIMER4, 29, timer3_isr);
}


void lp7812c_init_exit(void)
{
    bsp_hw_timer_del(HW_TIMER4);
}


#endif  //(CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C || CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
