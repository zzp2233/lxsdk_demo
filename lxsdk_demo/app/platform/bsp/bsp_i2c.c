#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_K(...)            printk(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_K(...)
#endif

#if I2C_SW_EN
//AT(.text.bsp.i2c)
//static void bsp_i2c_delay(void)
//{
//    u8 delay = 60;
//    while (delay--) {
//        asm("nop");
//    }
//}
#define bsp_i2c_delay() delay_us(5)

//ACK: The transmitter releases the SDA line (HIGH->LOW) during the acknowledge clock pulse
AT(.text.bsp.i2c)
void bsp_i2c_tx_ack(void)
{
    I2C_SDA_OUT();
    I2C_SDA_L();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    I2C_SCL_L();
}

AT(.text.bsp.i2c)
bool bsp_i2c_rx_ack(void)
{
    bool ret = false;
    I2C_SDA_IN();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    if (!I2C_SDA_IS_H())
    {
        ret = true;
    }
    I2C_SCL_L();
    return ret;
}

//NACK: The transmitter holds the SDA line (keep HIGH) during the acknowledge clock pulse
AT(.text.bsp.i2c)
void bsp_i2c_tx_nack(void)
{
    I2C_SDA_OUT();
    I2C_SDA_H();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    I2C_SCL_L();
}

//START: A HIGH to LOW transition on the SDA line while SCL is HIGH is one such unique case.
AT(.text.bsp.i2c)
void bsp_i2c_start(void)
{
    I2C_SDA_SCL_OUT();
    I2C_SDA_SCL_H();
    bsp_i2c_delay();
    I2C_SDA_L();
    bsp_i2c_delay();
    I2C_SCL_L();
}

//STOP: A LOW to HIGH transition on the SDA line while SCL is HIGH
AT(.text.bsp.i2c)
void bsp_i2c_stop(void)
{
    I2C_SDA_OUT();
    I2C_SDA_L();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    I2C_SDA_H();
}

//tx 1byte
AT(.text.bsp.i2c)
void bsp_i2c_tx_byte(uint8_t dat)
{
    u8 i;
    I2C_SDA_OUT();
    for (i=0; i<8; i++)
    {
        if (dat & BIT(7))
        {
            I2C_SDA_H();
        }
        else
        {
            I2C_SDA_L();
        }
        bsp_i2c_delay();
        I2C_SCL_H();
        bsp_i2c_delay();
        I2C_SCL_L();
        dat <<= 1;
    }
}

//rx 1byte
AT(.text.bsp.i2c)
uint8_t bsp_i2c_rx_byte(void)
{
    u8 i, dat = 0;
    I2C_SDA_IN();
    for (i=0; i<8; i++)
    {
        bsp_i2c_delay();
        I2C_SCL_H();
        bsp_i2c_delay();
        dat <<= 1;
        if (I2C_SDA_IS_H())
        {
            dat |= BIT(0);
        }
        I2C_SCL_L();
    }
    return dat;
}
#endif //I2C_SW_EN

#if I2C_HW_EN

#if (CHIP_PACKAGE_SELECT == CHIP_5691G || CHIP_PACKAGE_SELECT == CHIP_5691C_F)
static i2c_t HW_IIC0 =
{
    .sfr         = (i2c_sfr_t *) &IIC0CON0,
    .map         = (i2c_map_t *) &FUNCMCON2,
};
i2c_t *HW_IIC = &HW_IIC0;
#elif (CHIP_PACKAGE_SELECT == CHIP_5690G || CHIP_PACKAGE_SELECT == CHIP_5690F)
static i2c_t HW_IIC1 =
{
    .sfr         = (i2c_sfr_t *) &IIC1CON0,
    .map         = (i2c_map_t *) &FUNCMCON2,
};
i2c_t *HW_IIC = &HW_IIC1;
#else
#error choose correct Chip!!!
#endif

void bsp_i2c0_lock(void)
{
    os_i2c0_lock(100);
}

void bsp_i2c0_unlock(void)
{
    os_i2c0_unlock();
}

AT(.com_text.isr.i2c)
void bsp_i2c_isr(void)
{
    if (HW_IIC->sfr->IICxCON0 & BIT(31))
    {
        HW_IIC->sfr->IICxCON0 |=  BIT(29);                   // clear pending
        HW_IIC->sfr->IICxCON0 &= ~BIT(1);                    // disable irq

        HW_IIC->sfr->IICxDMACNT = 0;                         // Disable DMA
    }
}

static u32 bsp_hw_i2c_config(u32 i2c_cfg, u16 dev_addr, u16 reg_addr, u32 dat)
{
    TRACE("%s: cfg=%X dev=%X reg=%X\n", __FUNCTION__, i2c_cfg, dev_addr, reg_addr);

    HW_IIC->sfr->IICxCMDA = (u8)dev_addr | ((u32)(dev_addr >> 8)) << 24 |
                            (u32)((u8)reg_addr) << 8 | (u32)((u8)(reg_addr >> 8)) << 16;

    HW_IIC->sfr->IICxCON1 = i2c_cfg;
    HW_IIC->sfr->IICxDATA = dat;
    HW_IIC->sfr->IICxCON0 |= BIT(28);                     // kick

    u32 ticks = tick_get();
    while ( (!(HW_IIC->sfr->IICxCON0 & BIT(31))))
    {
        WDT_CLR();
        if (tick_check_expire(ticks, 1000))
        {
            func_process();
            ticks = tick_get();
            printf("!!!IIC ERROR dev_addr:0x%X reg_addr:0x%x\n", dev_addr,reg_addr);
            return false;
        }
    }

    HW_IIC->sfr->IICxCON0 |= BIT(29);

    if (i2c_cfg & RDATA)
    {
        return HW_IIC->sfr->IICxDATA;
    }
    else
    {
        return true;
    }
}

void bsp_hw_i2c_tx_byte(u32 i2c_cfg, u16 dev_addr, u16 reg_addr, u32 data)
{
    bsp_i2c0_lock();
    bsp_hw_i2c_config(i2c_cfg, dev_addr, reg_addr, data);
    bsp_i2c0_unlock();
}

void bsp_hw_i2c_tx_buf(u32 i2c_cfg, u16 dev_addr, u16 reg_addr, u8 *buf, u16 len)
{
    int i;
    u32 cfg;
    if (buf == NULL || len == 0)
    {
        return;
    }

    bsp_i2c0_lock();

    for (i = 0; i < len; i++)
    {
        cfg = WDATA;
        if (i == 0)                 //收第1byte
        {
            cfg |= i2c_cfg;
        }
        if (i == (len - 1))         //收最后1byte
        {
            cfg |= STOP_FLAG;
        }
        bsp_hw_i2c_config(cfg | DATA_CNT_1B, dev_addr, reg_addr, buf[i]);
    }

    bsp_i2c0_unlock();
}

void bsp_hw_i2c_rx_buf(u32 i2c_cfg, u16 dev_addr, u16 reg_addr, u8 *buf, u16 len)
{
    int i;
    u32 cfg;
    if (buf == NULL || len == 0)
    {
        return;
    }

    bsp_i2c0_lock();

    for (i = 0; i < len; i++)
    {
        cfg = RDATA;
        if (i == 0)                 //收第1byte
        {
            cfg |= i2c_cfg;
        }
        if (i == (len - 1))         //收最后1byte
        {
            cfg |= STOP_FLAG | NACK;
        }
        buf[i] = bsp_hw_i2c_config(cfg | DATA_CNT_1B, dev_addr, reg_addr, 0);
    }

    bsp_i2c0_unlock();
}

static void i2cx_init(void)
{
#if (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
    uteModulePlatformOutputGpioSet(IO_PE4,false);
    uteModulePlatformOutputGpioSet(IO_PE5,false);
#elif (CHIP_PACKAGE_SELECT == CHIP_5691G)
    uteModulePlatformOutputGpioSet(IO_PE1,false);
    uteModulePlatformOutputGpioSet(IO_PE2,false);
#endif
#if (CHIP_PACKAGE_SELECT == CHIP_5691G)
    CLKCON1 |= BIT(7);      //x26m_clkdiv8
    CLKGAT2 |= BIT(0);      //en iic0 clk
    RSTCON0 |= BIT(3);      //Release IIC0

    GPIOEDIR |= BIT(2) | BIT(1);                    //SCL SDA
    GPIOEPU |= BIT(2) | BIT(1);
    GPIOEDE |= BIT(2) | BIT(1);
    GPIOEFEN |= BIT(2) | BIT(1);

    FUNCMCON2 = (0xf << 8);
    FUNCMCON2 = (5 << 8);

#elif (CHIP_PACKAGE_SELECT == CHIP_5690G || CHIP_PACKAGE_SELECT == CHIP_5690F)
    CLKCON1 |= BIT(8);      //rc2m
    CLKGAT2 |= BIT(1);      //en iic1 clk
    RSTCON0 |= BIT(7);      //Release IIC1

    GPIOEDIR |= BIT(2) | BIT(1);                    //SCL SDA
    GPIOEPU |= BIT(2) | BIT(1);
    GPIOEDE |= BIT(2) | BIT(1);
    GPIOEFEN |= BIT(2) | BIT(1);

    FUNCMCON2 = (0xf << 12);
    FUNCMCON2 = (8 << 12);

#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
    CLKCON1 &= ~BIT(7);      //x26m_clkdiv8
    CLKGAT2 |= BIT(0);      //en iic0 clk
    RSTCON0 |= BIT(3);      //Release IIC0

    GPIOEDIR |= BIT(4) | BIT(5);                    //SCL SDA
    GPIOEPU |= BIT(4) | BIT(5);
    GPIOEDE |= BIT(4) | BIT(5);
    GPIOEFEN |= BIT(4) | BIT(5);

    FUNCMCON2 = (0xf << 8);
    FUNCMCON2 = (4 << 8);
#endif

    /*
        IIC速率计算方式: 时钟频率/分频数
        时钟频率：CLK = CLKCON1 |= BIT(7);        x26m_clkdiv8 24M时钟频率 分频数为8 则时钟频率为3M
        分频数：  DIV = IICxCON0[9:4];            在BIT(4)和BIT(9)之间填参 最小值=0;最大值=63
        IIC速率： S = CLK / (DIV + 1);            3M / (20 + 1) ~= 142KHz
     */

    HW_IIC->sfr->IICxCON0 =     1 << 0 |     //IIC EN
                                0 << 1 |     //IIC INT
                                0 << 2 |     //IIC HOLD CNT [3:2]
                                6 << 4 |    //IIC POSDIV [9:4]
                                1 << 10;     //IIC WSCL_OPT

    //  delay_5ms(1);
}

AT(.text.bsp.i2c)
void i2c_gsensor_init(void)
{
    sys_cb.gsensor_iic_en = true;
#if (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
    uteModulePlatformOutputGpioSet(IO_PE1,false);
    uteModulePlatformOutputGpioSet(IO_PE2,false);
#elif (CHIP_PACKAGE_SELECT == CHIP_5691G)
    uteModulePlatformOutputGpioSet(IO_PF1,false);
    uteModulePlatformOutputGpioSet(IO_PF2,false);
#endif
#if (CHIP_PACKAGE_SELECT == CHIP_5691G)
    CLKCON1 |= BIT(7);      //x26m_clkdiv8
    CLKGAT2 |= BIT(0);      //en iic0 clk
    RSTCON0 |= BIT(3);      //Release IIC0

    GPIOFDIR |= BIT(2) | BIT(1);                    //SCL SDA
    GPIOFPU |= BIT(2) | BIT(1);
    GPIOFDE |= BIT(2) | BIT(1);
    GPIOFFEN |= BIT(2) | BIT(1);

    FUNCMCON2 = (0xf << 8);
    FUNCMCON2 = (8 << 8);
#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
    CLKCON1 &= ~BIT(7);      //rc2m
    CLKGAT2 |= BIT(0);      //en iic0 clk
    RSTCON0 |= BIT(3);      //Release IIC0

    GPIOEDIR |= BIT(1) | BIT(2);                    //SCL SDA
    GPIOEPU |= BIT(1) | BIT(2);
    GPIOEDE |= BIT(1) | BIT(2);
    GPIOEFEN |= BIT(1) | BIT(2);

    FUNCMCON2 = (0xf << 8);
    FUNCMCON2 = (5 << 8);
#endif

    /*
        IIC速率计算方式: 时钟频率/分频数
        时钟频率：CLK = CLKCON1 |= BIT(7);        x26m_clkdiv8 24M时钟频率 分频数为8 则时钟频率为3M
        分频数：  DIV = IICxCON0[9:4];            在BIT(4)和BIT(9)之间填参 最小值=0;最大值=63
        IIC速率： S = CLK / (DIV + 1);            3M / (20 + 1) ~= 142KHz
     */

    HW_IIC->sfr->IICxCON0 =     1 << 0 |     //IIC EN
                                0 << 1 |     //IIC INT
                                0 << 2 |     //IIC HOLD CNT [3:2]
                                6 << 4 |    //IIC POSDIV [9:4]
                                1 << 10;     //IIC WSCL_OPT

    //  delay_5ms(1);
    sys_irq_init(IRQ_I2C_VECTOR, 0, bsp_i2c_isr);
}

#endif

// static void i2cx_init(void)
// {
//     CLKCON1 |= BIT(7);      //x26m_clkdiv8
//     CLKGAT2 |= BIT(0);      //en iic0 clk
//     RSTCON0 |= BIT(3);      //Release IIC0

//     GPIOEDIR |= BIT(2) | BIT(1);                    //SCL SDA
//     GPIOEPU |= BIT(2) | BIT(1);
//     GPIOEDE |= BIT(2) | BIT(1);
//     GPIOEFEN |= BIT(2) | BIT(1);

//     FUNCMCON2 = (0xf << 8);
//     FUNCMCON2 = (5 << 8);

//     /*
//         IIC速率计算方式: 时钟频率/分频数
//         时钟频率：CLK = CLKCON1 |= BIT(7);        x26m_clkdiv8 24M时钟频率 分频数为8 则时钟频率为3M
//         分频数：  DIV = IICxCON0[9:4];            在BIT(4)和BIT(9)之间填参 最小值=0;最大值=63
//         IIC速率： S = CLK / (DIV + 1);            3M / (20 + 1) ~= 142KHz
//      */

//     HW_IIC->sfr->IICxCON0 =     1 << 0 |     //IIC EN
//                                 0 << 1 |     //IIC INT
//                                 0 << 2 |     //IIC HOLD CNT [3:2]
//                                 20 << 4 |    //IIC POSDIV [9:4]
//                                 1 << 10;     //IIC WSCL_OPT

//     delay_5ms(8);
// }


AT(.text.bsp.i2c)
void bsp_i2c_init(void)
{
    sys_cb.gsensor_iic_en = false;
#if I2C_SW_EN
    I2C_SDA_SCL_OUT();
    I2C_SDA_H();
    delay_5ms(2);
#endif

#if I2C_HW_EN
    i2cx_init();
    sys_irq_init(IRQ_I2C_VECTOR, 0, bsp_i2c_isr);
#endif
}
