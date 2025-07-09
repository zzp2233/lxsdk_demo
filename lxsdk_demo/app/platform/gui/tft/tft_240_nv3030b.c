#include "include.h"
// 240 * 296

#if (GUI_SELECT == 0)
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

void tft_spi_nv3030b_init(void)
{
    printf("tft_nv3030b_init\n");

    // HW_Reset();
    // delay_ms(120);

    // WriteComm(0x11);
    // delay_ms(120);

//驱动信息:NV3030B-02-BOE2.01-IPS-GAMMA2.2-20230719.txt
    WriteComm(0xFD);
    WriteData(0x06);
    WriteData(0x08);

    WriteComm(0x61);//
    WriteData(0x07);//
    WriteData(0x07);//

    WriteComm(0x73);//
    WriteData(0x70);
    WriteComm(0x73);//
    WriteData(0x00);
    //bias
    WriteComm(0x62);//bias setting
    WriteData(0x00);//01
    WriteData(0x44);//04 44
    WriteData(0x40);//44 65 40
    WriteComm(0x63);//VGL
    WriteData(0x41);//
    WriteData(0x07);//
    WriteData(0x12);//
    WriteData(0x12);//
    //VSP
    WriteComm(0x65);//Pump1=4.7MHz //PUMP1 VSP
    WriteData(0x09);//D6-5:pump1_clk[1:0] clamp 28 2b
    WriteData(0x17);//6.26
    WriteData(0x21);
    //VSN
    WriteComm(0x66); //pump=2 AVCL
    WriteData(0x09); //clamp 08 0b 09
    WriteData(0x17); //10
    WriteData(0x21);
    //add source_neg_time
    WriteComm(0x67);//pump_sel
    WriteData(0x20);//21 20
    WriteData(0x40);
    //gamma vap/van
    WriteComm(0x68);//gamma vap/van
    WriteData(0x90);//
    WriteData(0x30);//
    WriteData(0x1C);//VCOM
    WriteData(0x27);//
    WriteComm(0xb1);//frame rate
    WriteData(0x0F);//0x0f fr_h[5:0] 0F
    WriteData(0x02);//0x02 fr_v[4:0] 02
    WriteData(0x01);//0x04 fr_div[2:0] 03
    WriteComm(0xB4);
    WriteData(0x01); //00:column 01:dot
    ////porch
    WriteComm(0xB5);
    WriteData(0x02);//0x02 vfp[6:0]
    WriteData(0x02);//0x02 vbp[6:0]
    WriteData(0x0a);//0x0A hfp[6:0]
    WriteData(0x14);//0x14 hbp[6:0]
    WriteComm(0xB6);
    WriteData(0x44);//04
    WriteData(0x01);//01
    WriteData(0x9f);//
    WriteData(0x00);//
    WriteData(0x02);//
    ////gamme sel
    WriteComm(0xdf);//
    WriteData(0x11);//gofc_gamma_en_sel=1

    WriteComm(0xE2);
    WriteData(0x0d);//vrp0[5:0] V63
    WriteData(0x0F);//vrp1[5:0] V62
    WriteData(0x11);//vrp2[5:0] V61
    WriteData(0x33);//vrp3[5:0] V2
    WriteData(0x36);//vrp4[5:0] V1
    WriteData(0x3f);//vrp5[5:0] V0
    WriteComm(0xE5);
    WriteData(0x3f);//vrn0[5:0] V0
    WriteData(0x37);//vrn1[5:0] V1
    WriteData(0x33);//vrn2[5:0] V2
    WriteData(0x12);//vrn3[5:0] V61
    WriteData(0x10);//vrn4[5:0] V62
    WriteData(0x04);//vrn5[5:0] V63
    WriteComm(0xE1);
    WriteData(0x2C);//prp0[6:0] V51
    WriteData(0x74);//prp1[6:0] V15
    WriteComm(0xE4);
    WriteData(0x74);//prn0[6:0] V15
    WriteData(0x2B);//prn1[6:0] V51
    WriteComm(0xE0);
    WriteData(0x08);//pkp0[4:0] V60
    WriteData(0x07);//pkp1[4:0] V56
    WriteData(0x0D);//pkp2[4:0] V45
    WriteData(0x13);//pkp3[4:0] V37
    WriteData(0x11);//pkp4[4:0] V29 //
    WriteData(0x13);//pkp5[4:0] V21
    WriteData(0x0E);//pkp6[4:0] V7
    WriteData(0x14);//pkp7[4:0] V3
    WriteComm(0xE3);
    WriteData(0x17);//pkn0[4:0] V3
    WriteData(0x0F);//pkn1[4:0] V7
    WriteData(0x14);//pkn2[4:0] V21
    WriteData(0x11);//pkn3[4:0] V29 //
    WriteData(0x13);//pkn4[4:0] V37
    WriteData(0x0C);//pkn5[4:0] V45
    WriteData(0x06);//pkn6[4:0] V56
    WriteData(0x04);//pkn7[4:0] V60
    //GAMMA---------------------------------/////////////
    WriteComm(0xE6);
    WriteData(0x00);
    WriteData(0xff);//SC_EN_START[7:0] f0
    WriteComm(0xE7);
    WriteData(0x01);//CS_START[3:0] 01
    WriteData(0x04);//scdt_inv_sel cs_vp_en
    WriteData(0x03);//CS1_WIDTH[7:0] 12
    WriteData(0x03);//CS2_WIDTH[7:0] 12
    WriteData(0x00);//PREC_START[7:0] 06
    WriteData(0x12);//PREC_WIDTH[7:0] 12
    WriteComm(0xE8); //source
    WriteData(0x00); //VCMP_OUT_EN 81-vcmp/vref_output
    WriteData(0x70); //chopper_sel[6:4]
    WriteData(0x00); //gchopper_sel[6:4] 60
    ////gate
    WriteComm(0xEc);
    WriteData(0x52);//50
    WriteComm(0xF1);
    WriteData(0x01);//TE
    WriteData(0x01);
    WriteData(0x02);
    WriteComm(0xF6);
    WriteData(0x01);
    WriteData(0x30);//epf[1:0]
    WriteData(0x00);
    WriteData(0x00);//SPI2L: 40
    WriteComm(0xfd);
    WriteData(0xfa);
    WriteData(0xfc);
    WriteComm(0x3a);
    WriteData(0x55);//565
    WriteComm(0x35);
    WriteData(0x00);
    WriteComm(0x36);//bgr_[3]
    WriteData(0x00);//c0
    WriteComm(0x11); // exit sleep
    delay_ms(200);
    WriteComm(0x29); // display on
    delay_ms(20);
}
#endif