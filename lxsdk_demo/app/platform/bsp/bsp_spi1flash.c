#include "include.h"

#if FLASH_EXTERNAL_EN

#define SPI1_CS_PDIR            GPIOHDIR
#define SPI1_CS_PORTSET         GPIOHSET
#define SPI1_CS_PORTCLR         GPIOHCLR
#define SPI1_CS                 7
#define SPI1_CS_EN()            SPI1_CS_PORTCLR = BIT(SPI1_CS)
#define SPI1_CS_DIS()           SPI1_CS_PORTSET = BIT(SPI1_CS)

#define SF_WRITESSR             0x01                    //SPIFlash写状态寄存器
#define SF_PROGRAM              0x02                    //SPIFlash编程
#define SF_READ                 0x03                    //SPIFlash读取
#define SF_READSSR              0x05                    //SPIFlash读状态寄存器
#define SF_WRITE_EN             0x06                    //SPIFlash写使能
#define SF_ERASE                0x20                    //SPIFlash擦除

#define SPI1_TX()                SPI1CON &= ~BIT(4)
#define SPI1_RX()                SPI1CON |= BIT(4);  delay_us(5)


struct _type_spi_cb
{
    u8 read_cmd[12];
    u8 read_cmd_cnt;
    u8 sf_read;
    u8 dummy;
    bool mode4io;
} spi1_cb;

///SPIFlash Delay
AT(.text.spi1flash)
void spi1_delay(void)
{
    uint cnt = 20;
    while (cnt--)
    {
        asm("nop");
    }
}

///SPI接口获取1Byte数据
AT(.text.spi1flash)
u8 spi1_getbyte(void)
{
    SPI1CON |= BIT(4);                              //RX
    SPI1BUF = 0xff;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
    return SPI1BUF;
}


///SPI接口发送1Byte数据
AT(.text.spi1flash)
void spi1_sendbyte(u8 data)
{
    SPI1CON &= ~BIT(4);                             //TX
    SPI1BUF = data;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
}

///SPIFlash写使能
AT(.com_text.spiflash)
void spi1flash_write_enable(void)
{
    SPI1_CS_EN();
    spi1_sendbyte(SF_WRITE_EN);
    SPI1_CS_DIS();
    spi1_delay();
}

///SPIFlash读状态寄存器
AT(.text.util.spiflash)
uint spi1flash_readssr(void)
{
    uint ssr;
    SPI1_CS_EN();
    spi1_sendbyte(SF_READSSR);
    ssr = spi1_getbyte();
    SPI1_CS_DIS();
    spi1_delay();
    return ssr;
}

///SPIFlash等待BUSY
AT(.text.spi1flash)
void spi1flash_waitbusy(void)
{
    do
    {
        spi1_delay();
    }
    while (spi1flash_readssr() & 0x01);
}

///SPIFlash写状态寄存器
AT(.text.spi1flash)
void spi1flash_writessr(u16 ssr)
{
    spi1flash_write_enable();
    SPI1_CS_EN();
    spi1_sendbyte(SF_WRITESSR);
    spi1_sendbyte(BYTE0(ssr));
    spi1_sendbyte(BYTE1(ssr));
    SPI1_CS_DIS();
    spi1flash_waitbusy();
}

///发送SPIFlash的地址
AT(.text.spi1flash)
void spi1flash_sendaddr(u32 addr)
{
    spi1_sendbyte(addr >> 16);
    spi1_sendbyte(addr >> 8);
    spi1_sendbyte(addr);
}



///初始化SPIFlash接口
AT(.text.spi1flash)
void spi1flash_init(u8 sf_read, u8 dummy, bool mode4io)
{
    memset(spi1_cb.read_cmd, 0xff, sizeof(spi1_cb.read_cmd));
    spi1_cb.sf_read = sf_read;
    spi1_cb.dummy = dummy;
    spi1_cb.mode4io = mode4io;
    if (mode4io)
    {
        spi1_cb.read_cmd[0] = ((sf_read & BIT(7)) >> 3) | ((sf_read & BIT(6)) >> 6);
        spi1_cb.read_cmd[1] = ((sf_read & BIT(5)) >> 1) | ((sf_read & BIT(4)) >> 4);
        spi1_cb.read_cmd[2] = ((sf_read & BIT(3)) << 1) | ((sf_read & BIT(2)) >> 2);
        spi1_cb.read_cmd[3] = ((sf_read & BIT(1)) << 3) | (sf_read & BIT(0));
        spi1_cb.read_cmd_cnt = 8 + dummy;
    }
    else
    {
        spi1_cb.read_cmd[0] = sf_read;
        spi1_cb.read_cmd_cnt = 4 + dummy;
    }
}

///SPIFlash编程
AT(.text.spi1flash)
void spi1flash_program_do(void *buf, u32 addr, uint len)
{
    spi1flash_write_enable();

    SPI1_CS_EN();
    spi1_sendbyte(SF_PROGRAM);
    spi1flash_sendaddr(addr);

    SPI1CON &= ~BIT(4);                             //TX
    SPI1DMAADR = DMA_ADR(buf);
    SPI1DMACNT = len;
    while (!(SPI1CON & BIT(16)));                   //Wait pending

    SPI1_CS_DIS();
    spi1flash_waitbusy();
}

AT(.text.spi1flash)
void spi1flash_program(void *buf, u32 addr, uint len)
{

    if (NULL == buf || 0 == len)
    {
        return ;
    }
    uint32_t page_remain;

    page_remain = 256 - addr%256;

    if(len <= page_remain)
    {
        page_remain = len;
    }
    while(1)
    {
        WDT_CLR();
        if (0 == page_remain)
        {
            break;
        }
        spi1flash_program_do(buf,addr,page_remain);

        if(len == page_remain)
            break;
        else
        {
            buf += page_remain;
            addr += page_remain;

            len -= page_remain;
            if(len > 256)
                page_remain = 256;
            else
                page_remain = len;
        }
    }
}

///SPIFlash擦除
AT(.text.spi1flash)
void spi1flash_erase(u32 addr)
{
    spi1flash_write_enable();

    SPI1_CS_EN();
    spi1_sendbyte(SF_ERASE);
    spi1flash_sendaddr(addr);
    SPI1_CS_DIS();

    spi1flash_waitbusy();
}

void bsp_spi1flash_init(void)
{
    GPIOHFEN &= ~BIT(7);
    GPIOHDE |= BIT(7);
    GPIOHDIR &= ~BIT(7);
    GPIOHCLR = BIT(7);

    GPIOHDIR |= (BIT(6));
    GPIOHFEN |= (BIT(4) | BIT(5) | BIT(6));
    GPIOHDE |= (BIT(4) | BIT(5) | BIT(6));

    GPIOHDIR &= ~(BIT(4) | BIT(5));                            //CLK OUT

    GPIOFDE |= (BIT(1) | BIT(2));
    GPIOFDIR |= (BIT(1) | BIT(2));
    GPIOFFEN |= (BIT(1) | BIT(1));

    CLKGAT1 |= BIT(12);
    FUNCMCON1 = 0xf0;
    FUNCMCON1 = (6 << 4);

    SPI1BAUD = 1;
    SPI1CON = BIT(0);                                  //enable
    spi1flash_init(0x0b, 1, false);

    spi1flash_writessr(0x02 << 8);       //QE EN
    spi1flash_init(0xeb, 2, true);

    SPI1CON |= BIT(10);                                 //output data and sample data is at the same clock edge
    SPI1CON |= BIT(3) | BIT(2);                         //enable 4bit mode

}

///SPIFlash读取
AT(.com_text.spiflash)
bool spi1flash_read(void *buf, u32 addr, uint len)
{
    SPI1_CS_EN();
    SPI1_TX();
    spi1_sendbyte(SF_READ);
    spi1flash_sendaddr(addr);

    SPI1_RX();                              //Set RX
    SPI1DMAADR = DMA_ADR(buf);
    SPI1DMACNT = len;
    while (!(SPI1CON & BIT(16)));                   //Wait pending
    SPI1_CS_DIS();

    return true;
}

///SPIFlash Read Kick
AT(.com_text.spiflash)
bool spi1flash_read_kick(void *buf, u32 addr, uint len)
{
    SPI1_CS_EN();
    if (spi1_cb.mode4io)
    {
        SPI1CON |= BIT(9);                          //MultBits
        spi1_cb.read_cmd[4] = (u8)(addr >> 16);
        spi1_cb.read_cmd[5] = (u8)(addr >> 8);
        spi1_cb.read_cmd[6] = (u8)(addr);
    }
    else
    {
        spi1_cb.read_cmd[1] = (u8)(addr >> 16);
        spi1_cb.read_cmd[2] = (u8)(addr >> 8);
        spi1_cb.read_cmd[3] = (u8)(addr);
    }
    SPI1CON &= ~BIT(4);                             //TX
    SPI1DMAADR = DMA_ADR(spi1_cb.read_cmd);
    SPI1DMACNT = spi1_cb.read_cmd_cnt;
    while (!(SPI1CON & BIT(16)));                   //Wait pending

    SPI1CON |= BIT(9) | BIT(4);                     //MultBits, RX
    SPI1DMAADR = DMA_ADR(buf);
    SPI1DMACNT = len;
    return true;
}

///SPIFlash Read Wait
AT(.com_text.spiflash)
bool spi1flash_read_wait(void)
{
    while (!(SPI1CON & BIT(16)));                   //Wait pending
    SPI1CON &= ~BIT(9);                             //OneBit
    SPI1_CS_DIS();
    return true;
}

#endif


