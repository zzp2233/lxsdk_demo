#include "include.h"
// 466 * 466

//#define Delay(ms) bsp_spide_cs(1);delay_ms(ms)
#if (GUI_SELECT == GUI_OLED_466_ICNA3310B)

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

// cmd 12h 1:cmd      4:addr/data
// cmd 32h 1:cmd/addr 4:data
// cmd 02h 1:cmd      1:addr/data
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
void tft_write_end();

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

void oled_set_brightness(u8 brightness)
{
	tft_write_cmd(0x51);
	tft_write_data(BYTE0(brightness));
	tft_write_end();
}

void oled_466_icna3310b_test(void)
{
 	WriteComm(0xfe);
	WriteData(0x20);
	WriteComm(0xf4);
	WriteData(0x5a);
	WriteComm(0xf5);
	WriteData(0x59);
	WriteComm(0xfe);
	WriteData(0xd0);
	WriteComm(0x4d);
	WriteData(0x7f);
	WriteComm(0x4e);
	WriteData(0x80);
	WriteComm(0xfe);
	WriteData(0x40);
	WriteComm(0x54);
	WriteData(0xaf);
	WriteComm(0xfe);
	WriteData(0x00);
	WriteComm(0xc4);
	WriteData(0x80);
	WriteComm(0x3a);
	WriteData(0x55);
	WriteComm(0x35);
	WriteData(0x00);
	WriteComm(0x53);
	WriteData(0x20);
	WriteComm(0x51);
	WriteData(0x7f);
	WriteComm(0x63);
	WriteData(0xff);

	WriteComm(0x2a);
	WriteData(0x00);
	WriteData(0x06);
	WriteData(0x01);
    WriteData(0xd7);

	WriteComm(0x2b);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x01);
    WriteData(0xd1);

	WriteComm(0x11);

//CommEnd();
	delay_ms(120);
	WriteComm(0x29);
//  	WriteComm(0x2C);

while (1) {
    WDT_CLR();
}

CommEnd();
}


void oled_466_icna3310b_init(void)
{
 	WriteComm(0xfe);
	WriteData(0x20);
	WriteComm(0xf4);
	WriteData(0x5a);
	WriteComm(0xf5);
	WriteData(0x59);
	WriteComm(0xfe);
	WriteData(0x40);
	WriteComm(0x08);
	WriteData(0x0a);
	WriteComm(0xfe);
	WriteData(0x00);
	WriteComm(0xc4);
	WriteData(0x80);
	WriteComm(0x3a);
	WriteData(0x55);
	WriteComm(0x35);
	WriteData(0x00);
	WriteComm(0x53);
	WriteData(0x20);
	WriteComm(0x51);
	WriteData(0xA0);
	WriteComm(0x63);
	WriteData(0xff);

WriteComm(0x35);        //TE On
WriteData(0x00);

	WriteComm(0x11);

CommEnd();
	delay_ms(120);
	WriteComm(0x29);
//  	WriteComm(0x2C);

CommEnd();
}

//uint32_t gc9c01_read_id(void)
//{
//  uint32_t id = 0;
//  ReadComm(0x04);
//  id = id*256 + bsp_spide_rcv_byte();
//  id = id*256 + bsp_spide_rcv_byte();
//  id = id*256 + bsp_spide_rcv_byte();
//  id = id*256 + bsp_spide_rcv_byte();
//
//  CommEnd();
//
//  return id;
//}
#endif
