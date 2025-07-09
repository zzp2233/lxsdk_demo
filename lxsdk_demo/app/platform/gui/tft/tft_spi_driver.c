﻿#include "include.h"

#if (GUI_SELECT == GUI_TFT_SPI)

#define WriteComm(v) tft_write_cmd(v)
#define WriteData(v) tft_write_data(v)

void tft_write_cmd(u8 cmd);
void tft_write_data(u8 data);

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

#if TFT_SPI_DRIVER

void tft_spi_jd9853_init(void)
{
#if (TFT_SPI_DRIVER == TFT_SPI_JD9853_V1)
//驱动信息: JD9853_BOE1.45_WV015GES-NB80_172x320_230831_johnson.c
//20230831 ADD RAMBIST setting
//20230718 johnson first release
    //SET_PASSWD
    WriteComm(0xDF);
    WriteData(0x98);
    WriteData(0x53);

    WriteComm(0xB2);
    WriteData(0x23);

    WriteComm(0xB7);
    WriteData(0x00);
    WriteData(0x47);
    WriteData(0x00);
    WriteData(0x6F);

    WriteComm(0xBB);
    WriteData(0x1C);
    WriteData(0x1A);
    WriteData(0x55);
    WriteData(0x73);
    WriteData(0x6F);
    WriteData(0xF0);

    WriteComm(0xC0);
    WriteData(0x44);
    WriteData(0xA4);

    WriteComm(0xC1);
    WriteData(0x12);

    WriteComm(0xC3);
    WriteData(0x7D);
    WriteData(0x07);
    WriteData(0x14);
    WriteData(0x06);
    WriteData(0xCF);
    WriteData(0x71);
    WriteData(0x72);
    WriteData(0x77);

    WriteComm(0xC4);
    WriteData(0x00);//00=60Hz 06=57Hz 08=51Hz
    WriteData(0x00);
    WriteData(0xA0);//LN=320  Line
    WriteData(0x79);
    WriteData(0x0B);
    WriteData(0x0A);
    WriteData(0x16);
    WriteData(0x79);
    WriteData(0x0B);
    WriteData(0x0A);
    WriteData(0x16);
    WriteData(0x82);

    //SET_R_GAMMA 20230718 VOP5.25 G2.2

    WriteComm(0xC8);
    WriteData(0x3F);
    WriteData(0x32);
    WriteData(0x29);
    WriteData(0x29);
    WriteData(0x27);
    WriteData(0x2B);
    WriteData(0x27);
    WriteData(0x28);
    WriteData(0x28);
    WriteData(0x26);
    WriteData(0x25);
    WriteData(0x17);
    WriteData(0x12);
    WriteData(0x0D);
    WriteData(0x04);
    WriteData(0x00);
    WriteData(0x3F);
    WriteData(0x32);
    WriteData(0x29);
    WriteData(0x29);
    WriteData(0x27);
    WriteData(0x2B);
    WriteData(0x27);
    WriteData(0x28);
    WriteData(0x28);
    WriteData(0x26);
    WriteData(0x25);
    WriteData(0x17);
    WriteData(0x12);
    WriteData(0x0D);
    WriteData(0x04);
    WriteData(0x00);

    WriteComm(0xD0);
    WriteData(0x04);
    WriteData(0x06);
    WriteData(0x6B);
    WriteData(0x0F);
    WriteData(0x00);

    WriteComm(0xD7);
    WriteData(0x00);
    WriteData(0x30);

    WriteComm(0xE6);
    WriteData(0x14);

    WriteComm(0xDE);
    WriteData(0x01);

    WriteComm(0xB7);
    WriteData(0x03);
    WriteData(0x13);
    WriteData(0xEF);
    WriteData(0x35);
    WriteData(0x35);

    WriteComm(0xC1);
    WriteData(0x14);
    WriteData(0x15);
    WriteData(0xC0);

    WriteComm(0xC2);
    WriteData(0x06);
    WriteData(0x3A);

    WriteComm(0xC4);
    WriteData(0x72);
    WriteData(0x12);

    WriteComm(0xBE);
    WriteData(0x00);

    WriteComm(0xDE);
    WriteData(0x02);

    WriteComm(0xE5);
    WriteData(0x00);
    WriteData(0x02);
    WriteData(0x00);

    WriteComm(0xE5);
    WriteData(0x01);
    WriteData(0x02);
    WriteData(0x00);

    WriteComm(0xDE);
    WriteData(0x00);

    WriteComm(0x35);
    WriteData(0x00);

    WriteComm(0x3A);
//    WriteData(0x06);//06=RGB666；05=RGB565
    WriteData(0x05);

    WriteComm(0x2A);
    WriteData(0x00);
    WriteData(0x22);//Start_X=34
    WriteData(0x00);
    WriteData(0xCD);//End_X=205

    WriteComm(0x2B);
    WriteData(0x00);
    WriteData(0x00);//Start_Y=0
    WriteData(0x01);
    WriteData(0x3F);//End_Y=319

    WriteComm(0x11);

    delay_ms(120);

    WriteComm(0xDE);
    WriteData(0x02);

    WriteComm(0xE5);
    WriteData(0x00);
    WriteData(0x02);
    WriteData(0x00);

    WriteComm(0xDE);
    WriteData(0x00);

    WriteComm(0x29);

    delay_ms(10);
#endif // (TFT_SPI_DRIVER == TFT_SPI_JD9853_V1)
}

void tft_spi_gc9307_init(void)
{
//驱动信息:J9308+IVO231_GAMMA22（新代码）.txt
#if (TFT_SPI_DRIVER == SPI_GC9307_V1)
    printf("tft_spi_gc9307_init\n");
    // delay_ms(50);

    // HW_Reset();

    // delay_ms(50);

    // delay_ms(120);

    // WriteComm(0xfe);
    // WriteComm(0xef);

    // WriteComm(0x36);
    // WriteData(0x48);

    // WriteComm(0x3a);
    // WriteData(0x05);

    // WriteComm(0x84);
    // WriteData(0x40);
    // WriteComm(0x86);
    // WriteData(0x98);
    // WriteComm(0x89);
    // WriteData(0x03);
    // WriteComm(0x8b);
    // WriteData(0x80);
    // WriteComm(0x8d);
    // WriteData(0x33);
    // WriteComm(0x8e);
    // WriteData(0x0f);


    // WriteComm(0xB6);
    // WriteData(0x00);
    // WriteData(0x00);
    // WriteData(0x23);

    // WriteComm(0xe8);
    // WriteData(0x13);
    // WriteData(0x00);

    // WriteComm(0xe9);
    // WriteData(0x08);//2data

    // WriteComm(0xff);
    // WriteData(0x62);

    // WriteComm(0x99);
    // WriteData(0x3e);
    // WriteComm(0x9d);
    // WriteData(0x4b);

    // WriteComm(0x98);
    // WriteData(0x3e);
    // WriteComm(0x9c);
    // WriteData(0x4b);

    // WriteComm(0xc3);
    // WriteData(0x20);
    // WriteComm(0xc4);
    // WriteData(0x10);
    // WriteComm(0xc9);
    // WriteData(0x35);

    // WriteComm(0xF0);//GAMMA
    // WriteData(0x1a);
    // WriteData(0x1d);
    // WriteData(0x0a);
    // WriteData(0x08);
    // WriteData(0xf5);
    // WriteData(0x39);

    // WriteComm(0xF2);
    // WriteData(0x1a);
    // WriteData(0x1d);
    // WriteData(0x0a);
    // WriteData(0x08);
    // WriteData(0xf5);
    // WriteData(0x39);

    // WriteComm(0xF1);
    // WriteData(0x4c);
    // WriteData(0x9e);
    // WriteData(0x5e);
    // WriteData(0x29);
    // WriteData(0x2f);
    // WriteData(0xaf);

    // WriteComm(0xF3);
    // WriteData(0x4c);
    // WriteData(0x9e);
    // WriteData(0x5e);
    // WriteData(0x29);
    // WriteData(0x2f);
    // WriteData(0xaf);//2.2

    // WriteComm(0x35);
    // WriteData(0x00);//TE
    // WriteComm(0x44);
    // WriteData(0x00);
    // WriteData(0x0a);//TE scan line
    // WriteComm(0x11);
    // delay_ms(120);
    // WriteComm(0x29);
    // WriteComm(0x2c);


    delay_ms(50);

    HW_Reset();

    delay_ms(50);

    delay_ms(120);

    WriteComm(0xfe);
    WriteComm(0xef);

    WriteComm(0x36);
    WriteData(0x48);

    WriteComm(0x3a);
    WriteData(0x05);

    WriteComm(0x86);
    WriteData(0x98);
    WriteComm(0x89);
    WriteData(0x03);
    WriteComm(0x8b);
    WriteData(0x00);
    WriteComm(0x8d);
    WriteData(0x33);
// #if (GUI_MODE_SELECT == MODE_4WIRE_8BIT)
//     WriteData(0x11);    //4w-8b-1l spi
// #elif (GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
//     WriteData(0x13);    //3w-9b-2l despi, 格科微内部寄存器, 功能未知
// #endif
    WriteComm(0x8e);
    WriteData(0x0f);
    WriteComm(0xec);
    WriteData(0x33);
    WriteData(0x0c);
    WriteData(0xd1);
    WriteComm(0xc9);
    WriteData(0x05);

    WriteComm(0xe8);
    WriteData(0x12);
    WriteData(0x00);

    WriteComm(0xe9);
    WriteData(0x08);

    WriteComm(0xc3);
    WriteData(0x22);
    WriteComm(0xc4);
    WriteData(0x16);

    WriteComm(0xff);
    WriteData(0x62);
    WriteComm(0x35);
    WriteData(0x00);

    WriteComm(0x99);
    WriteData(0x3e);
    WriteComm(0x9d);
    WriteData(0x4b);
    WriteComm(0x98);
    WriteData(0x3e);
    WriteComm(0x9c);
    WriteData(0x4b);

    WriteComm(0xf0);
    WriteData(0xc3);
    WriteData(0x05);
    WriteData(0x08);
    WriteData(0x07);
    WriteData(0x04);
    WriteData(0x26);

    WriteComm(0xf2);
    WriteData(0xc3);
    WriteData(0x05);
    WriteData(0x08);
    WriteData(0x07);
    WriteData(0x04);
    WriteData(0x26);

    WriteComm(0xf1);
    WriteData(0x3f);
    WriteData(0x7f);
    WriteData(0x6f);
    WriteData(0x35);
    WriteData(0x36);
    WriteData(0x4f);

    WriteComm(0xf3);
    WriteData(0x3f);
    WriteData(0x7f);
    WriteData(0x6f);
    WriteData(0x35);
    WriteData(0x36);
    WriteData(0x4f);


    WriteComm(0xe9);
    WriteData(0x08);//2data

    WriteComm(0x11);
    delay_ms(120);
    WriteComm(0x29);
    WriteComm(0x2c);

#endif // (TFT_SPI_DRIVER == SPI_GC9307_V1)
}

void tft_240_st7789_init(void)
{
#if (TFT_SPI_DRIVER == SPI_ST7789_V1)
    printf("tft_st7789_init\n");
    // HW_Reset();

    // delay_ms(120);

    // WriteComm(0x11);

    // delay_ms(120);

    // WriteComm(0xB2);
    // WriteData(0x0C);
    // WriteData(0x0C);
    // WriteData(0x00);
    // WriteData(0x33);
    // WriteData(0x33);

    // WriteComm(0x35);
    // WriteData(0x00);

    // WriteComm(0x36);
    // WriteData(0x00);

    // WriteComm(0x3A);
    // WriteData(0x05);

    // WriteComm(0xB7);
    // WriteData(0x06);

    // WriteComm(0xBB);
    // WriteData(0x1A);

    // WriteComm(0xC0);
    // WriteData(0x2C);

    // WriteComm(0xC2);
    // WriteData(0x01);

    // WriteComm(0xC3);
    // WriteData(0x0F);

    // WriteComm(0xC6);
    // WriteData(0x0F);

    // WriteComm(0xD0);
    // WriteData(0xA7);

    // WriteComm(0xD0);
    // WriteData(0xA4);
    // WriteData(0xA1);

    // WriteComm(0xD6);
    // WriteData(0xA1);

    // WriteComm(0xE0);
    // WriteData(0xF0);
    // WriteData(0x00);
    // WriteData(0x08);
    // WriteData(0x07);
    // WriteData(0x08);
    // WriteData(0x04);
    // WriteData(0x33);
    // WriteData(0x44);
    // WriteData(0x4A);
    // WriteData(0x39);
    // WriteData(0x12);
    // WriteData(0x12);
    // WriteData(0x2C);
    // WriteData(0x33);

    // WriteComm(0xE1);
    // WriteData(0xF0);
    // WriteData(0x08);
    // WriteData(0x0D);
    // WriteData(0x0C);
    // WriteData(0x0A);
    // WriteData(0x26);
    // WriteData(0x33);
    // WriteData(0x44);
    // WriteData(0x4A);
    // WriteData(0x36);
    // WriteData(0x14);
    // WriteData(0x14);
    // WriteData(0x2E);
    // WriteData(0x34);

    // WriteComm(0xE7);
    // WriteData(0x11); //2DATA 打开

    // WriteComm(0x21);

    // WriteComm(0x29);

    // WriteComm(0x2C);

//~~~~~~~~~~~~~~~~~~~~~~~~`
    HW_Reset();

    delay_ms(120);

    WriteComm( 0x11);

    delay_ms(120);

    WriteComm( 0xB2);
    WriteData( 0x0C);
    WriteData( 0x0C);
    WriteData( 0x00);
    WriteData( 0x33);
    WriteData( 0x33);

    WriteComm( 0xB0);
    WriteData( 0x00);
    WriteData( 0xE0);

    WriteComm( 0x35);
    WriteData( 0x00);

    WriteComm( 0x36);
    WriteData( 0x00);

    WriteComm( 0x3A);
    WriteData( 0x05);

    WriteComm( 0xB7);
    WriteData( 0x56);

    WriteComm( 0xBB);
    WriteData( 0x20);

    WriteComm( 0xC0);
    WriteData( 0x2C);

    WriteComm( 0xC2);
    WriteData( 0x01);

    WriteComm( 0xC3);
    WriteData( 0x0F);

    WriteComm( 0xC6);
    WriteData( 0x0F);

    WriteComm( 0xD0);
    WriteData( 0xA7);

    WriteComm( 0xD0);
    WriteData( 0xA4);
    WriteData( 0xA1);

    WriteComm( 0xD6);
    WriteData( 0xA1);

    WriteComm( 0xD3);
    WriteData( 0x01);

    WriteComm( 0xE0);
    WriteData( 0xF0);
    WriteData( 0x03);
    WriteData( 0x09);
    WriteData( 0x08);
    WriteData( 0x08);
    WriteData( 0x04);
    WriteData( 0x33);
    WriteData( 0x54);
    WriteData( 0x49);
    WriteData( 0x39);
    WriteData( 0x14);
    WriteData( 0x14);
    WriteData( 0x2E);
    WriteData( 0x33);

    WriteComm( 0xE1);
    WriteData( 0xF0);
    WriteData( 0x09);
    WriteData( 0x0D);
    WriteData( 0x0A);
    WriteData( 0x09);
    WriteData( 0x15);
    WriteData( 0x32);
    WriteData( 0x44);
    WriteData( 0x49);
    WriteData( 0x36);
    WriteData( 0x12);
    WriteData( 0x12);
    WriteData( 0x2C);
    WriteData( 0x34);

    WriteComm(0xE7);
    WriteData(0x11); //2DATA 打开

    WriteComm( 0x21);

    WriteComm( 0x29);

    WriteComm( 0x2C);
#endif // (TFT_SPI_DRIVER == SPI_ST7789_V1)
}
#endif // TFT_SPI_DRIVER

#endif //(GUI_SELECT == GUI_TFT_SPI)
