#include "include.h"
// 240 * 296
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if (GUI_SELECT == GUI_TFT_320_385_GV9B71)
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
    printf("tft_set_window(%d,%d,%d,%d)\n", x0, y0, x1, y1);
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

void tft_spi_gc9b71_init(void)
{
    printf("tft_gc9b71_init\n");

    // HW_Reset();
    // delay_ms(120);

    // WriteComm(0x11);
    // delay_ms(120);

//驱动信息:NV3030B-02-BOE2.01-IPS-GAMMA2.2-20230719.txt
//VCI=IOVCC=2.8V SPI3W2DATA

    WriteComm(0xfe);
    WriteComm(0xef);
    WriteComm(0x80);
    WriteData(0x11);
    WriteComm(0x81);
    WriteData(0x30);
    WriteComm(0x82);
    WriteData(0x09);
    WriteComm(0x83);
    WriteData(0x03);
    WriteComm(0x84);
    WriteData(0x22);
    WriteComm(0x89);
    WriteData(0x18);
    WriteComm(0x8A);
    WriteData(0x40);
    WriteComm(0x8B);
    WriteData(0x0A);



    WriteComm(0x3a);
    WriteData(0x05);
    WriteComm(0x36);
    WriteData(0x40);
    WriteComm(0xEC);
    WriteData(0x07);

    WriteComm(0x35);
    WriteData(0x00);

    WriteComm(0x74);
    WriteData(0x01);
    WriteData(0x80);
    WriteData(0x00);
    WriteData(0x00);
    WriteData(0x00);
    WriteData(0x00);

    WriteComm(0x98);
    WriteData(0x3E);
    WriteComm(0x99);
    WriteData(0x3E);


    WriteComm(0xA1);
    WriteData(0x01);
    WriteData(0x04);

    WriteComm(0xA2);
    WriteData(0x01);
    WriteData(0x04);

    WriteComm(0xCB);
    WriteData(0x02);



    WriteComm(0x7C);
    WriteData(0xB6);
    WriteData(0x24);

    WriteComm(0xAC);
    WriteData(0x74);


    WriteComm(0xF6);
    WriteData(0x80);


    WriteComm(0xB5);
    WriteData(0x09);
    WriteData(0x09);


    WriteComm(0xEB);
    WriteData(0x01);
    WriteData(0x81);



    WriteComm(0x60);
    WriteData(0x38);
    WriteData(0x06);
    WriteData(0x13);
    WriteData(0x56);

    WriteComm(0x63);
    WriteData(0x38);
    WriteData(0x08);
    WriteData(0x13);
    WriteData(0x56);

    WriteComm(0x61);
    WriteData(0x3B);
    WriteData(0x1b);
    WriteData(0x58);
    WriteData(0x38);

    WriteComm(0x62);
    WriteData(0x3B);
    WriteData(0x1b);
    WriteData(0x58);
    WriteData(0x38);







    WriteComm(0x64);
    WriteData(0x38);
    WriteData(0x0a);
    WriteData(0x73);
    WriteData(0x16);
    WriteData(0x13);
    WriteData(0x56);

    WriteComm(0x66);
    WriteData(0x38);
    WriteData(0x0b);
    WriteData(0x73);
    WriteData(0x17);
    WriteData(0x13);
    WriteData(0x56);




    WriteComm(0x68);
    WriteData(0x00);
    WriteData(0x0B);
    WriteData(0x22);
    WriteData(0x0B);
    WriteData(0x22);
    WriteData(0x1C);
    WriteData(0x1C);

    WriteComm(0x69);
    WriteData(0x00);
    WriteData(0x0B);
    WriteData(0x26);
    WriteData(0x0B);
    WriteData(0x26);
    WriteData(0x1C);
    WriteData(0x1C);


    WriteComm(0x6A);
    WriteData(0x15);
    WriteData(0x00);

    WriteComm(0x6E);
    WriteData(0x08);
    WriteData(0x02);
    WriteData(0x1a);
    WriteData(0x00);
    WriteData(0x12);
    WriteData(0x12);
    WriteData(0x11);
    WriteData(0x11);
    WriteData(0x14);
    WriteData(0x14);
    WriteData(0x13);
    WriteData(0x13);
    WriteData(0x04);
    WriteData(0x19);
    WriteData(0x1e);
    WriteData(0x1d);
    WriteData(0x1d);
    WriteData(0x1e);
    WriteData(0x19);
    WriteData(0x04);
    WriteData(0x0b);
    WriteData(0x0b);
    WriteData(0x0c);
    WriteData(0x0c);
    WriteData(0x09);
    WriteData(0x09);
    WriteData(0x0a);
    WriteData(0x0a);
    WriteData(0x00);
    WriteData(0x1a);
    WriteData(0x01);
    WriteData(0x07);

    WriteComm(0x6C);
    WriteData(0xCC);
    WriteData(0x0C);
    WriteData(0xCC);
    WriteData(0x84);
    WriteData(0xCC);
    WriteData(0x04);
    WriteData(0x50);


    WriteComm(0x7D);
    WriteData(0x72);



    WriteComm(0x70);
    WriteData(0x02);
    WriteData(0x03);
    WriteData(0x09);
    WriteData(0x07);
    WriteData(0x09);
    WriteData(0x03);
    WriteData(0x09);
    WriteData(0x07);
    WriteData(0x09);
    WriteData(0x03);

    WriteComm(0x90);
    WriteData(0x06);
    WriteData(0x06);
    WriteData(0x05);
    WriteData(0x06);

    WriteComm(0x93);
    WriteData(0x45);
    WriteData(0xFF);
    WriteData(0x00);


    WriteComm(0xC3);
    WriteData(0x15);
    WriteComm(0xC4);
    WriteData(0x36);
    WriteComm(0xC9);
    WriteData(0x3d);


    WriteComm(0xF0);
    WriteData(0x47);//gVR1_N[5:0]
    WriteData(0x07);//gVR2_N[5:0]
    WriteData(0x0A);//gVR4_N[4:0]
    WriteData(0x0A);//gVR6_N[4:0]
    WriteData(0x00);//gVR0_N[3:0] gVR13_N[3:0] 7
    WriteData(0x29);//gVR20_N[6:0]

    WriteComm(0xF2);
    WriteData(0x47);//gVR1_P[5:0]
    WriteData(0x07);//gVR2_P[5:0]
    WriteData(0x0a);//gVR4_P[4:0]
    WriteData(0x0A);//gVR6_P[4:0]
    WriteData(0x00);//gVR0_P[3:0] gVR13_P[3:0]
    WriteData(0x29);//gVR20_P[6:0]

    WriteComm(0xF1);
    WriteData(0x42);//gVR43_N[6:0]
    WriteData(0x91);//gVR27_N[2:0] gVR57_N[4:0]
    WriteData(0x10);//gVR36_N[2:0] gVR59_N[4:0]
    WriteData(0x2D);//gVR61_N[5:0]
    WriteData(0x2F);// gVR62_N[5:0]
    WriteData(0x6F);//gVR50_N[3:0] gVR63_N[3:0]

    WriteComm(0xF3);
    WriteData(0x42);//gVR43_P[6:0]
    WriteData(0x91);//gVR27_P[2:0] gVR57_P[4:0]
    WriteData(0x10);//gVR36_P[2:0] gVR59_P[4:0]
    WriteData(0x2D);//gVR61_P[5:0]
    WriteData(0x2F);// gVR62_P[5:0]
    WriteData(0x6F);//gVR50_P[3:0] gVR63_P[3:0]



    WriteComm(0xF9);
    WriteData(0x30);

    WriteComm(0xBE);
    WriteData(0x11);

    WriteComm(0xFB);
    WriteData(0x00);
    WriteData(0x00);


    //WriteComm(0xB1);
    //WriteData(0x08);

    WriteComm(0x11);
    delay_ms(120);
    WriteComm(0x29);
    delay_ms(120);
    // WriteComm(0x2C);
    // WriteData(0x00);
    // WriteData(0x00);
    // WriteData(0x00);
    // WriteData(0x00);
    // WriteComm(0x2C);
    // WriteData(0x00);
    // WriteData(0x00);
    // WriteData(0x00);
    // WriteData(0x00);
    // delay_ms(120);
}
#endif
