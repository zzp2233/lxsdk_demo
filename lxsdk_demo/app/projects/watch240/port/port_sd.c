#include "include.h"

#if MUSIC_SDCARD_EN
static gpio_t sddet_gpio;

void sdcard_detect_init(void)
{
    gpio_t *g = &sddet_gpio;
    u8 io_num = SD0_DETECT_IO;
    bsp_gpio_cfg_init(g, io_num);

    if (!io_num)
    {
        return;
    }
    else if (io_num == IO_MUX_SDCLK)
    {
        SD_MUX_DETECT_INIT();
    }
    else if (io_num <= IO_MAX_NUM)
    {
        g->sfr[GPIOxDE] |= BIT(g->num);
        g->sfr[GPIOxPU] |= BIT(g->num);
        g->sfr[GPIOxDIR] |= BIT(g->num);
    }
}

AT(.com_text.sdio)
bool sdcard_is_online(void)
{
    gpio_t *g = &sddet_gpio;
    u8 io_num = SD0_DETECT_IO;

    if (!io_num)
    {
        return false;
    }
    else if (io_num == IO_MUX_SDCLK)
    {
        return SD_MUX_IS_ONLINE();
    }
    else
    {
        return (!(g->sfr[GPIOx] & BIT(g->num)));
    }
}

AT(.com_text.sdio)
bool is_det_sdcard_busy(void)
{
    u8 io_num = SD0_DETECT_IO;

    //无SD检测
    if (!io_num)
    {
        return true;
    }

    if (io_num == IO_MUX_SDCLK)             //普通IO或复用SDCLK检测
    {
        return SD_MUX_IS_BUSY();
    }
    return false;
}

AT(.text.sdcard)
void sd_gpio_init(u8 type)
{
    if (type == 0)
    {
        SD_MUX_IO_INIT();
    }
    else if (type == 1)
    {
        SD_CLK_DIR_OUT();
        FUNCMCON0 = SD0_MAPPING;
    }
    else
    {
        if(SD0_DETECT_IO != IO_MUX_SDCMD)
        {
            SD_CLK_DIR_IN();
        }
    }
}

#else
AT(.text.sdcard)
void sd_gpio_init(u8 type)
{
}
AT(.com_text.sdio)
bool is_det_sdcard_busy(void)
{
    return false;
};
#endif          // MUSIC_SDCARD_EN
