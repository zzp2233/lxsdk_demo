#include "include.h"

#if (GUI_SELECT == GUI_TFT_SPI)

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

void tft_write_cmd(u8 cmd);
void tft_write_data(u8 data);
void tft_write_end(void);

#define WriteComm(v) tft_write_cmd(v)
#define ReadComm(v)  ReadComm03(v)

AT(.com_text.tft_spi)
void tft_write_end()
{
    TFT_SPI_CS_DIS();
}

//0x02: 1CMD 1ADDR 1DATA
AT(.com_text.tft_spi)
void tft_write_cmd(u8 cmd)
{
    TFT_SPI_CS_DIS();                     // 先拉高 CS，避免干扰
    delay_us(1);                          // 确保时序稳定
    TFT_SPI_CS_EN();                      // 重新拉低 CS 开始传输
#if (GUI_MODE_SELECT == MODE_3WIRE_9BIT)
    DC_CMD_EN();
    tft_spi_sendbyte(cmd);
#elif (GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
    DC_CMD_EN();
    tft_spi_sendbyte(cmd);
#elif (GUI_MODE_SELECT == MODE_4WIRE_8BIT)
    DC_CMD_EN();
    tft_spi_sendbyte(cmd);
#elif (GUI_MODE_SELECT == MODE_QSPI)
    // QSPI 模式下，写命令需先使用 1bit 传输控制字节
    DESPICON &= ~BIT(3);                  // [3] 清零，切换为 1bit 模式
    tft_spi_sendbyte(0x02);               // 写命令控制字 (0x02)
    tft_spi_sendbyte(0x00);               // 空地址位
    tft_spi_sendbyte(cmd);                // 实际要发送的命令
    tft_spi_sendbyte(0x00);               // 空周期
#endif
}

#if (GUI_MODE_SELECT == MODE_QSPI)
// 0x12：1 个命令 + 4 字节地址 + 4 字节数据
AT(.com_text.tft_spi)
void tft_write_cmd12(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();
    DESPICON &= ~BIT(3);                  // 单线发送控制字
    tft_spi_sendbyte(0x12);               // 特殊写命令
    DESPICON |= BIT(3);                   // 恢复为 4bit 模式
    tft_spi_sendbyte(0x00);               // 地址 0
    tft_spi_sendbyte(cmd);                // 写入命令
    tft_spi_sendbyte(0x00);               // 空周期
}
#endif


AT(.com_text.tft_spi)
void tft_write_data(u8 data)
{
#if (GUI_MODE_SELECT == MODE_3WIRE_9BIT)
    TFT_SPI_CS_EN();
    DC_DATA_EN();
    tft_spi_sendbyte(data);
    TFT_SPI_CS_DIS();
#elif (GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
    TFT_SPI_CS_EN();
    DC_DATA_EN();
    tft_spi_sendbyte(data);
    TFT_SPI_CS_DIS();
#elif (GUI_MODE_SELECT == MODE_4WIRE_8BIT)
    TFT_SPI_CS_EN();
    DC_DATA_EN();
    tft_spi_sendbyte(data);
    TFT_SPI_CS_DIS();
#elif (GUI_MODE_SELECT == MODE_QSPI)
    // QSPI 模式下，此时 DESPICON 已经处于 4bit 状态
    // 直接发送数据即可
    tft_spi_sendbyte(data);
#endif
}

//0x03: 1CMD 1ADDR 1DATA
AT(.com_text.tft_spi)
void tft_read_cmd(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();

#if (GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
    DC_CMD_EN();
    tft_spi_sendbyte(cmd);
#elif (GUI_MODE_SELECT == MODE_4WIRE_8BIT)
    DC_CMD_EN();
    tft_spi_sendbyte(cmd);
#elif (GUI_MODE_SELECT == MODE_QSPI)
    // 设置为 1bit 输入输出模式
    DESPICON = (DESPICON & ~(3 << 2)) | (0x1 << 2);   // [3:2] = 01
    tft_spi_sendbyte(0x03);       // 读命令控制字
    tft_spi_sendbyte(0x00);       // 地址 0
    tft_spi_sendbyte(cmd);        // 要读取的寄存器
    tft_spi_sendbyte(0x00);       // 空周期
#else
    tft_spi_sendbyte(cmd);
#endif
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
#if (GUI_MODE_SELECT == MODE_3WIRE_9BIT)
    tft_write_cmd(0x2C);
    DC_DATA_EN();
#elif (GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
    tft_write_cmd(0x2C);
    DC_DATA_EN();
    DC_DISABLE();
    //3w-9b 2line mode
    DESPICON |= BIT(26) | BIT(9) | BIT(3); //[26]dual en, [9]MultiBit, [3:2]2BIT data bus
#elif (GUI_MODE_SELECT == MODE_4WIRE_8BIT)
    tft_write_cmd(0x2C);
    DC_DATA_EN();
#elif (GUI_MODE_SELECT == MODE_QSPI)
    tft_write_cmd12(0x2C);      //TFT_RAMWR
#endif
}

#if 0 //纯色图推屏测试
static u8 debug_tft_dma_buf[320*2];
void spi_display_point(uint16_t x1, uint16_t y1, uint16_t color)
{
    delay_5ms(200);
    LCD_BL_EN();

    tft_set_window(0, 0, x1, y1);
    tft_frame_start();

    printf("%s DESPICON[%x]\n", __func__, DESPICON);
#if 0//DMA buf
    for (int i = 0; i <= x1*2; i += 2)
    {
        debug_tft_dma_buf[i + 1] = color >> 8;      //dataH => SDA
        debug_tft_dma_buf[i] = color & 0xff;        //dataL => DC
    }

//    printf("data_len[%d] = %d*%d*2\n", x1*y1*2, x1, y1);
//    print_r(debug_tft_dma_buf, 8);

    tft_spi_send(debug_tft_dma_buf, 10);
    for (int i = 0; i <= 240; i++)
    {
        WDT_CLR();
        tft_spi_send(debug_tft_dma_buf, 320*2);
    }
#else
    for (int x = 0; x <= x1; x++)
    {
        for (int y = 0; y <= y1; y++)
        {
            WDT_CLR();
            WriteData(color >> 8);
            WriteData(color);
        }
    }
#endif
    tft_write_end();
    printf("%s done\n", __func__);
}
#endif

uint32_t tft_read_id(void)
{
    uint32_t id = 0;
    tft_read_cmd(0x04);
#if (GUI_MODE_SELECT == MODE_4WIRE_8BIT || GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
    DC_DATA_EN();
#endif
    id = tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    tft_write_end();
    return id;
}

void tft_spi_init(void)
{
#if (TFT_SPI_DRIVER == SPI_JD9853_V1)
    tft_spi_jd9853_init();
#elif (TFT_SPI_DRIVER == SPI_GC9307_V1)
    tft_spi_gc9307_init();
#elif (TFT_SPI_DRIVER == SPI_ST7789_V1)
    tft_240_st7789_init();
#endif
}

#endif //(GUI_SELECT == GUI_TFT_SPI)

