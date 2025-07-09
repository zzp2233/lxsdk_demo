#include "include.h"
#include "chsc6x_platform.h"

#if (CTP_SELECT == CTP_CHSC6X)

#define CHSC6X_I2C_DEVICE               (0x2E) //7bit
#define I2C_WRITE_ADDR(ADDR)             ((ADDR) << 1)              //CTP IIC写地址
#define I2C_READ_ADDR(ADDR)              ((ADDR) << 1 | 1)          //CTP IIC读地址

#if ((CHIP_PACKAGE_SELECT == CHIP_5691C_F)||(CHIP_PACKAGE_SELECT == CHIP_5690F))
static i2c_t __CHSC6XX_IIC1 =
{
    .sfr         = (i2c_sfr_t *) &IIC1CON0,
    .map         = (i2c_map_t *) &FUNCMCON2,
};
i2c_t *CHSC6XX_IICx = &__CHSC6XX_IIC1;

#elif ((CHIP_PACKAGE_SELECT == CHIP_5681C) || (CHIP_PACKAGE_SELECT == CHIP_5682B))
static i2c_t __CHSC6XX_IIC0 =
{
    .sfr         = (i2c_sfr_t *) &IIC0CON0,
    .map         = (i2c_map_t *) &FUNCMCON2,
};
i2c_t *CHSC6XX_IICx = &__CHSC6XX_IIC0;

#endif

void ctp_reset(void);

/**
 * @brief 触摸i2c写函数
 * @param[in] subAddr: 需要写的寄存器地址
 * @param[in] write_data: 写数据
 * @param[in] len: 写数据长度
 * @param[in] addr_is_16bit: 寄存器地址是否是16bit的
 **/
AT(.com_text.ctp)
void tp_i2c_write(uint16_t subAddr, const uint8_t *write_data, u16 len, u8 addr_is_16bit)
{
    u32 ticks;

    if (len == 0)
    {
        CHSC6XX_IICx->sfr->IICxCMDA = ((subAddr & 0xff) << 8) | I2C_WRITE_ADDR(CHSC6X_I2C_DEVICE);
        CHSC6XX_IICx->sfr->IICxCON1 = BIT(12) | BIT(11)  | BIT(10) | BIT(5) | BIT(4) | BIT(3) ;
        uint8_t data[1];
        data[0] = (subAddr & 0xff00) >> 8;
        CHSC6XX_IICx->sfr->IICxDMAADR = DMA_ADR(data);
        CHSC6XX_IICx->sfr->IICxDMACNT = (0 << 16) | BIT(1) |BIT(0);
    }
    else
    {
        CHSC6XX_IICx->sfr->IICxCMDA =  (subAddr << 8) | I2C_WRITE_ADDR(CHSC6X_I2C_DEVICE);

        CHSC6XX_IICx->sfr->IICxCON1 = BIT(12) | BIT(11)  | BIT(10) | BIT(5) | BIT(4) | BIT(3) ;
        if (addr_is_16bit)
        {
            CHSC6XX_IICx->sfr->IICxCON1 |= BIT(6);
        }
        CHSC6XX_IICx->sfr->IICxDMAADR = DMA_ADR(write_data);
        CHSC6XX_IICx->sfr->IICxDMACNT = ((len - 1) << 16) | BIT(1) |BIT(0);
    }

    CHSC6XX_IICx->sfr->IICxCON0 |= BIT(28);                //KICK
    delay_ms(2);

    ticks = tick_get();
    while (!(CHSC6XX_IICx->sfr->IICxCON0 & BIT(31)))
    {
        if (tick_check_expire(ticks, 20))
        {
            printf("tp_i2c_write time out ERROR\n");
            return;
        }
    }
    CHSC6XX_IICx->sfr->IICxCON0 |= BIT(29);                //Clear Pending
}

/**
 * @brief 触摸i2c读函数
 * @param[in] subAddr: 需要读的寄存器地址
 * @param[in] write_data: 读数据
 * @param[in] len: 读数据长度
 * @param[in] addr_is_16bit: 寄存器地址是否是16bit的
 **/
AT(.com_text.ctp)
static void tp_i2c_read(uint16_t subAddr, uint8_t *read_data, u16 len, u8 addr_is_16bit)
{
    u32 ticks;
    CHSC6XX_IICx->sfr->IICxCMDA = (I2C_READ_ADDR(CHSC6X_I2C_DEVICE) << 24) | (subAddr << 8) | I2C_WRITE_ADDR(CHSC6X_I2C_DEVICE);

    CHSC6XX_IICx->sfr->IICxCON1 = BIT(12) | BIT(11) | BIT(9) | BIT(8) | BIT(7) | BIT(5) | BIT(4) | BIT(3) | len;
    if (addr_is_16bit)
    {
        CHSC6XX_IICx->sfr->IICxCON1 |= BIT(6);
    }

    CHSC6XX_IICx->sfr->IICxDMAADR = DMA_ADR(read_data);
    CHSC6XX_IICx->sfr->IICxDMACNT = ((len - 1) << 16) | BIT(0);
    CHSC6XX_IICx->sfr->IICxCON0 |= BIT(28);                //KICK

    ticks = tick_get();
    while (!(CHSC6XX_IICx->sfr->IICxCON0 & BIT(31)))
    {
        if (tick_check_expire(ticks, 20))
        {
            printf("tp_i2c_read time out ERROR\n");
            return;
        }
    }

    if (!chsc6x_is_inited())
    {
        CHSC6XX_IICx->sfr->IICxCON0 |= BIT(29);                //Clear Pending
    }
}

/* RETURN:0->pass else->fail */
int chsc6x_write_reg(uint8_t id, uint8_t regaddr,uint8_t regvalue)
{
    uint8_t buf[2] = {0};

    buf[0] = regaddr;
    buf[1] = regvalue;

    tp_i2c_write(buf[0], &buf[1], 1, 0);
    return 0;
}

/* return: =read lenth succeed; <0 failed
   read reg addr not need
   just used for reading xy cord info*/
AT(.com_text.ctp)
int chsc6x_i2c_read(unsigned char id, unsigned char *p_data, unsigned short lenth)
{

    tp_i2c_read(0x00, p_data, lenth, 0);
    return lenth;
}

/* RETURN:0->pass else->fail */
int chsc6x_read_bytes_u16addr_sub(unsigned char id, unsigned short adr, unsigned char *rxbuf, unsigned short lenth)
{
//    int ret = i2cReadPacket(id,adr,lenth,rxbuf);
//    if(ret == lenth) {
//        return 0;
//    }else{
//        return -1;
//    }
    uint16_t buffer = 0;
    buffer = (adr >> 8) | (adr << 8);
    tp_i2c_read(buffer, rxbuf, lenth, 1);
    return 0;
}

/* RETURN:0->pass else->fail */
static uint8_t txbuf_tmp[128];                                          //Todo: dma memcpy
int chsc6x_write_bytes_u16addr_sub(unsigned char id, unsigned short adr, unsigned char *txbuf, unsigned short lenth)
{
//    int ret = i2cSendPacket(id,adr,lenth,txbuf);
//    if(ret == lenth) {
//        return 0;
//    }else{
//        return -1;
//    }

    if (NULL == txbuf)
    {
        return -1;
    }

    uint16_t buffer = 0;
    uint8_t *txbuf_p = txbuf;

    if ((u32)txbuf > 0x10000000)                                         //flash addr, 临时拷贝
    {
        memcpy(txbuf_tmp, txbuf, lenth);
        txbuf_p = txbuf_tmp;
    }

    buffer = (adr >> 8) | (adr << 8);
    tp_i2c_write(buffer, txbuf_p, lenth, 1);
    return 0;
}

void chsc6x_msleep(int ms)
{
//    tl_delay(32*ms);

    delay_ms(ms);
}

void chsc6x_tp_reset(void)
{
    PORT_CTP_RST_L();
    delay_ms(30);
    PORT_CTP_RST_H();
    delay_ms(30);
//    pd7_out0();
//    tl_delay(950);//30ms
//    pd7_out1();
//    tl_delay(950);//30ms

    // ctp_reset();
}

void chsc6x_tp_reset_active(void)
{
    PORT_CTP_RST_L();
    delay_ms(2);
    PORT_CTP_RST_H();
    delay_ms(21);

//    pd7_out0();
//    tl_delay(950);//30ms
//    pd7_out1();

    // ctp_reset();
}

#endif //(CTP_SELECT == CTP_CHSC6X)
