#include "include.h"
// 320 * 385

//#define Delay(ms) bsp_spide_cs(1);delay_ms(ms)

#if (GUI_SELECT == GUI_TFT_JD9853)


#define WriteData(v) tft_write_data(v)
#define CommEnd()    tft_write_end()
// cmd 12h 1:cmd      4:addr/data
// cmd 32h 1:cmd/addr 4:data
// cmd 02h 1:cmd      1:addr/data
#define WriteComm12(v) \
({                   \
    bsp_spide_cs(1);         \
    bsp_spide_cs(0);         \
    bsp_spide_bus_mode(SPIDE_1IO);   \
    bsp_spide_snd_byte(0x12);\
    bsp_spide_bus_mode(SPIDE_4IO);   \
    bsp_spide_snd_byte(0x00);\
    bsp_spide_snd_byte(v);   \
    bsp_spide_snd_byte(0x00);\
})
#define WriteComm32(v) \
({                   \
    bsp_spide_cs(1);         \
    bsp_spide_cs(0);         \
    bsp_spide_bus_mode(SPIDE_1IO);   \
    bsp_spide_snd_byte(0x32);\
    bsp_spide_snd_byte(0x00);\
    bsp_spide_snd_byte(v);   \
    bsp_spide_snd_byte(0x00);\
    bsp_spide_bus_mode(SPIDE_4IO);   \
})
#define WriteComm02(v) \
({                   \
    bsp_spide_cs(1);         \
    bsp_spide_cs(0);         \
    bsp_spide_bus_mode(SPIDE_1IO);   \
    bsp_spide_snd_byte(0x02);\
    bsp_spide_snd_byte(0x00);\
    bsp_spide_snd_byte(v);   \
    bsp_spide_snd_byte(0x00);\
})
#define ReadComm03(v) \
({                   \
    bsp_spide_cs(1);         \
    bsp_spide_cs(0);         \
    bsp_spide_bus_mode(SPIDE_1IO);   \
    bsp_spide_snd_byte(0x03);\
    bsp_spide_snd_byte(0x00);\
    bsp_spide_snd_byte(v);   \
    bsp_spide_snd_byte(0x00);\
})

AT(.com_text.tft_spi)
bool tft_rgbw_is_support(void)
{
    return true;
}

//0x02: 1CMD 1ADDR 1DATA
AT(.com_text.tft_spi)
void tft_write_cmd(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();
    DESPICON &= ~BIT(3);                        //1BIT
    tft_spi_sendbyte(0x02);
    tft_spi_sendbyte(0x00);
    tft_spi_sendbyte(cmd);
    tft_spi_sendbyte(0x00);
}

//0x12: 1CMD 4ADDR 4DATA
AT(.com_text.tft_spi)
void tft_write_cmd12(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();
    DESPICON &= ~BIT(3);                        //1BIT
    tft_spi_sendbyte(0x12);
    DESPICON |= BIT(3);                         //4BIT
    tft_spi_sendbyte(0x00);
    tft_spi_sendbyte(cmd);
    tft_spi_sendbyte(0x00);
}

//0x03: 1CMD 1ADDR 1DATA
AT(.com_text.tft_spi)
void tft_read_cmd(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();
    DESPICON &= ~BIT(3);                        //1BIT
    tft_spi_sendbyte(0x03);
    tft_spi_sendbyte(0x00);
    tft_spi_sendbyte(cmd);
    tft_spi_sendbyte(0x00);
}

AT(.com_text.tft_spi)
void tft_write_data(u8 data)
{
    tft_spi_sendbyte(data);
}

AT(.com_text.tft_spi)
void tft_write_end()
{
    TFT_SPI_CS_DIS();
}

AT(.com_text.tft_spi)
void tft_set_window(u16 x0, u16 y0, u16 x1, u16 y1)
{
    x0 += GUI_SCREEN_OFS_X;
    x1 += GUI_SCREEN_OFS_X;
    y0 += GUI_SCREEN_OFS_Y;
    y1 += GUI_SCREEN_OFS_Y;

    tft_write_cmd(0x2A);        //TFT_CASET
    tft_write_data(BYTE1(x0));
    tft_write_data(BYTE0(x0));
    tft_write_data(BYTE1(x1));
    tft_write_data(BYTE0(x1));

    tft_write_cmd(0x2B);        //TFT_PASET
    tft_write_data(BYTE1(y0));
    tft_write_data(BYTE0(y0));
    tft_write_data(BYTE1(y1));
    tft_write_data(BYTE0(y1));
    tft_write_end();
}

AT(.com_text.tft_spi)
void tft_write_data_start(void)
{
    tft_write_cmd12(0x2C);      //TFT_RAMWR
}

void tft_write_cmd(u8 cmd);
void tft_write_data(u8 data);
void tft_write_end(void);

#define WriteComm(v) tft_write_cmd(v)
#define ReadComm(v)  ReadComm03(v)

uint32_t tft_read_id(void)
{
    uint32_t id = 0;
    tft_read_cmd(0x04);
    id = tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    tft_write_end();
    return id;
}


void st7789_display_point(uint16_t x1, uint16_t y1, uint16_t color)
{
    TFT_SPI_DATA_EN();
    delay_5ms(200);
    printf("st7789_display_point_test_nomal\n");
    tft_set_window(0, 0, x1, y1);
    tft_frame_start();
    for (int x = 0; x <= x1; x++)
    {
        for (int y = 0; y <= y1; y++)
        {
            WDT_CLR();
            WriteData(color >> 8);
            WriteData(color);
        }
    }
    tft_write_end();

}

void HW_Reset(void)
{

    GPIOAFEN &= ~BIT(3);                            //RS
    GPIOADE  |= BIT(3);
    GPIOASET = BIT(3);
    GPIOADIR &= ~BIT(3);

    /*GPIOEFEN &= ~BIT(7);                            //RESET
    GPIOEDE  |= BIT(7);
    GPIOESET = BIT(7);
    GPIOEDIR &= ~BIT(7);
    delay_ms(10);
    GPIOECLR = BIT(7);
    delay_ms(20);
    GPIOESET = BIT(7);
    delay_ms(50);*/
}


void tft_240_jd9853_init(void)
{
    printf("tft_240_jd9853_init\n");

    //SET_PASSWD
    WriteComm(0xDF);
    WriteData(0x98);
    WriteData(0x53);

    //VGPOW_SET
    WriteComm(0xB2);
    WriteData(0x23);

    //GAMMA_SET
    WriteComm(0xB7);
    WriteData(0x00);
    WriteData(0x21);
    WriteData(0x00);
    WriteData(0x49);

    //DCDC_SEL
    WriteComm(0xBB);
    WriteData(0x1C);
    WriteData(0x9A);
    WriteData(0x55);
    WriteData(0x73);
    WriteData(0x6F);
    WriteData(0xF0);

    //SETSTBA
    WriteComm(0xC0);
    WriteData(0x22);
    WriteData(0x22);

    //SETPANEL
    WriteComm(0xC1);
    WriteData(0x12);

    //SETRGBCYC
    WriteComm(0xC3);
    WriteData(0x7D);
    WriteData(0x07);
    WriteData(0x14);
    WriteData(0x06);
    WriteData(0xC8);
    WriteData(0x71);
    WriteData(0x6C);
    WriteData(0x77);

    //SET_TCON
    WriteComm(0xC4);
    WriteData(0x00); // 00 60hz  01 50hz 02 42hz  粗调
    WriteData(0x00);
    WriteData(0x94);
    WriteData(0x79);
    WriteData(0x25);   // 每加大一个，减少0.17hz
    WriteData(0x0A);
    WriteData(0x16);
    WriteData(0x79);
    WriteData(0x25);
    WriteData(0x0A);
    WriteData(0x16);
    WriteData(0x82);

    //SET_R_GAMMA
    WriteComm(0xC8);
    WriteData(0x3F);
    WriteData(0x35);
    WriteData(0x2E);
    WriteData(0x29);
    WriteData(0x2C);
    WriteData(0x2D);
    WriteData(0x27);
    WriteData(0x27);
    WriteData(0x24);
    WriteData(0x24);
    WriteData(0x21);
    WriteData(0x15);
    WriteData(0x11);
    WriteData(0x0B);
    WriteData(0x08);
    WriteData(0x00);
    WriteData(0x3F);
    WriteData(0x35);
    WriteData(0x2E);
    WriteData(0x29);
    WriteData(0x2C);
    WriteData(0x2D);
    WriteData(0x27);
    WriteData(0x27);
    WriteData(0x24);
    WriteData(0x24);
    WriteData(0x21);
    WriteData(0x15);
    WriteData(0x11);
    WriteData(0x0B);
    WriteData(0x08);
    WriteData(0x00);

    //SET_GD
    WriteComm(0xD0);
    WriteData(0x04);
    WriteData(0x06);
    WriteData(0x6B);
    WriteData(0x0F);
    WriteData(0x00);

    //RAMCTRL
    WriteComm(0xD7);
    WriteData(0x30);
    WriteData(0x30);

    //
    WriteComm(0xE6);
    WriteData(0x04);

    //page1
    WriteComm(0xDE);
    WriteData(0x01);

    //DCDC_OPT
    WriteComm(0xB7);
    WriteData(0x03);
    WriteData(0x13);
    WriteData(0xEF);
    WriteData(0x35);
    WriteData(0x35);

    //SETRGBCYC2
    WriteComm(0xC1);
    WriteData(0x10);
    WriteData(0x15);
    WriteData(0xC0);

    //
    WriteComm(0xC2);
    WriteData(0x06);
    WriteData(0x3A);
    WriteData(0xE7);

    //SET_GAMMAOP
    WriteComm(0xC4);
    WriteData(0x72);
    WriteData(0x12);

    //GAMMA_POWER_TEST
    WriteComm(0xBE);
    WriteData(0x00);

    //page0
    WriteComm(0xDE);
    WriteData(0x00);

    WriteComm(0x35);
    WriteData(0x00);

    WriteComm(0x3A);
    WriteData(0x05);

    WriteComm(0x2A);
    WriteData(0x00);
    WriteData(0x00);
    WriteData(0x00);
    WriteData(0xEF);

    WriteComm(0x2B);
    WriteData(0x00);
    WriteData(0x00);
    WriteData(0x01);
    WriteData(0x27);

    WriteComm(0x11);
    CommEnd();
    delay_ms(120);
    WriteComm(0x29);
    delay_ms(50);
    CommEnd();

}

#endif
