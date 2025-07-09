#ifndef _TFT_H
#define _TFT_H

#include "tft_spi_driver.h"

void tft_set_temode(u8 mode);

void tft_set_window(u16 x0, u16 y0, u16 x1, u16 y1);

void tft_320_st77916_init(void);
void tft_rgbw_320_st77916_init(void);
void tft_360_gc9c01_init(void);
void oled_466_icna3310b_init(void);
uint32_t tft_read_id(void);

void tft_240_jd9853_init(void);
void tft_240_st7789_init(void);
void tft_240_st7789w3_init(void);
void tft_spi_init(void);


void tft_170_560_axs15231B_init(void);

void tft_init(void);
void tft_exit(void);
void tft_bglight_en(void);
void oled_set_brightness(u8 brightness);

void tft_set_baud(u8 baud1, u8 baud2);

typedef struct
{
    //TE控制相关
    bool tft_bglight_kick;      //背光控制
    u8   tft_bglight_duty;      //背光pwm占空比
    u8   tft_bglight_last_duty; //背光pwm上一次占空比
    u8 te_mode;
    u8 te_mode_next;
    bool tft_bglight_first_set;

    u8 te_bglight_cnt;          //在收到需要打开背光控制时，推完第一帧数据后延时打开背光
    u8 despi_baud;
    u8 despi_baud1;
    u8 despi_baud2;
    bool flag_in_frame;
    bool tft_set_baud_kick;     //需要切换时钟，等TFT_END后再切
} tft_cb_t;
extern tft_cb_t tft_cb;

/**
 * @brief 设置oled亮度
 * @param[in] level       亮度等级，使用无极调节时，范围0 ~ 100，否则为1~5
 * @param[in] stepless_en 是否使用无极调节
 *
 * @return  无
 **/
void oled_brightness_set_level(uint8_t level, bool stepless_en);

/**
 * @brief 设置背光亮度
 * @param[in] level       亮度等级，使用无极调节时，范围0 ~ 100，否则为1~5
 * @param[in] stepless_en 是否使用无极调节
 *
 * @return  无
 **/
void tft_bglight_set_level(uint8_t level, bool stepless_en);

/**
 * @brief 首次设置亮度检测
 * @param 无
 *
 * @return  无
 **/
void tft_bglight_frist_set_check(void);

void tft_spi_isr(void);
u8 tft_spi_getbyte(void);
void tft_spi_sendbyte(u8 val);
void tft_spi_send(void *buf, uint len);

void tft_frame_start(void);
void tft_frame_end(void);
void tft_write_data_start(void);
void tft_write_end();
void tft_te_isr(void);
void tick_te_isr(void);
#endif
