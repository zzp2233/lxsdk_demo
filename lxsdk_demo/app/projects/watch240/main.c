#include "include.h"

#if (GUI_SELECT == GUI_TFT_170_560_AXS15231B)
//样机只能到20M以下
const u8 tbl_despi_clk1[] =
{
    [SYS_88M] = 4,          //20M
    [SYS_132M] = 6,         //20M
    [SYS_144M] = 7,         //20M
    [SYS_176M] = 8,         //20M
    [SYS_192M] = 4,         //20M
};
const u8 tbl_despi_clk2[] =
{
    [SYS_88M] = 4,          //20M
    [SYS_132M] = 6,         //20M
    [SYS_144M] = 7,         //20M
    [SYS_176M] = 8,         //20M
    [SYS_192M] = 4,         //20M
};
#else
//320x385 60Hz 数据量为14.784MByte
const u8 tbl_despi_clk1[] =
{
    [SYS_88M] = 1,          //44M
    [SYS_132M] = 3,         //33M
    [SYS_144M] = 3,         //36M
    [SYS_176M] = 4,         //35M
    [SYS_192M] = 6,         //38M
};
const u8 tbl_despi_clk2[] =
{
    [SYS_88M] = 2,          //29.3M
    [SYS_132M] = 4,         //26.4M
    [SYS_144M] = 4,         //28.8M
    [SYS_176M] = 5,         //29.3M
    [SYS_192M] = 15,         //27.4M
};
#endif

//根据波特率和系统时钟计算DIV
u8 sys_get_despi_div(u32 baud, u32 sys_clk)
{
    u32 sys_clk_freq = 0;
    switch (sys_clk)
    {
        case SYS_48M:
            sys_clk_freq = 48000000;
            break;
        case SYS_88M:
            sys_clk_freq = 88000000;
            break;
        case SYS_132M:
            sys_clk_freq = 132000000;
            break;
        case SYS_144M:
            sys_clk_freq = 144000000;
            break;
        case SYS_176M:
            sys_clk_freq = 176000000;
            break;
        case SYS_192M:
            sys_clk_freq = 192000000;
            break;
        default:
            return 0;
    }
    u8 div = 0;
    if (sys_clk_freq >= baud)
    {
        div = sys_clk_freq / baud - 1;
    }
    return div;
}

//设置DESPI CLK接口
void sys_set_despi_baud_hook(u32 sys_clk)
{
#if (GUI_SELECT == DISPLAY_UTE)
    u8 despi_clk1 = sys_get_despi_div(UTE_DRV_SCREEN_1TE_SPI_CLK, sys_clk);
    u8 despi_clk2 = sys_get_despi_div(UTE_DRV_SCREEN_2TE_SPI_CLK, sys_clk);
    tft_set_baud(despi_clk1, despi_clk2);
#else
    tft_set_baud(tbl_despi_clk1[sys_clk], tbl_despi_clk2[sys_clk]);
#endif
}

const uint8_t *bt_rf_get_inq_param(void)
{
    return NULL;
}

u8 get_chip_package(void)
{
    return 0;
}

const uint8_t *bt_rf_get_param(void)
{
    return (const uint8_t *)&xcfg_cb.rf_pa_gain;
}

//默认OS_THREAD_MAIN_PRIORITY 30
#if ASR_SELECT
AT(.text.startup.init)
u32 system_get_main_stack_priopity(void)
{
    return 27;
}
#endif

#if ASR_USBKEY_PSD
// const u8 tbl_secret_key[16] = {0xA8, 0xE7, 0xB7, 0x74, 0xD1, 0x04, 0x97, 0x3E, 0x8E, 0xB4, 0x7E, 0x4A, 0x36, 0x99, 0xF2, 0xD7};
// const u8 tbl_secret_key[16] = {0xF6, 0x7C, 0xB6, 0xBD, 0x50, 0xE8, 0x33, 0xAF, 0x05, 0x15, 0x12, 0x76, 0x18, 0x56, 0x90, 0x99};
// const u8* get_alg_tbl(void)
// {
//     return tbl_secret_key;
// }

WEAK const u8* get_soft_key(void)
{
#if ASR_USBKEY_PSD
    return xcfg_cb.asr_soft_key;
#endif
    return NULL;
}
#endif


//正常启动Main函数
int main(void)
{
    u32 rst_reason, rtccon10;

    rst_reason = LVDCON;
    rtccon10 = RTCCON10;
    printf("Hello AB569X: %08x\n", rst_reason);
    if (rst_reason & BIT(24))
    {
        // bsp_rtc_recode_set(1);
        printf("SW reset\n");
    }
    else if (rst_reason & BIT(19))
    {
        if (rtccon10 & BIT(10))
        {
            printf("WKO10S reset\n");
        }
        else
        {
            printf("RTC_WDT reset\n");
        }
    }
    else if (rst_reason & BIT(18))
    {
        // bsp_rtc_recode_set(1);
        printf("WKUP reset\n");
    }
    else if (rst_reason & BIT(17) || rtccon10 & BIT(3))
    {
        // bsp_rtc_recode_set(1);
        printf("VUSB reset\n");
    }
    else if (rst_reason & BIT(16))
    {
        // bsp_rtc_recode_set(1);
        printf("WDT reset\n");
    }
#if ASR_SELECT
    thread_asr_create();
#endif //ASR_STACK_EN
    bsp_sys_init();
//    GPIOEDIR &= ~(BIT(7) | BIT(2) | BIT(1));
//    GPIOEFEN &= ~(BIT(7) | BIT(2) | BIT(1));
//    GPIOEDE |= (BIT(7) | BIT(2) | BIT(1));

    // GPIOBFEN &= ~BIT(0);                        // CS
    // GPIOBDE  |=  BIT(0);
    // GPIOBDIR &= ~BIT(0);
    // GPIOBSET = BIT(0);

    // GPIOBFEN &= ~BIT(1);                        // CS
    // GPIOBDE  |=  BIT(1);
    // GPIOBDIR &= ~BIT(1);
    // GPIOBSET = BIT(1);
    // printf("GPIOA & BIT(7) = %d\n", GPIOA & BIT(7));
    // GPIOASET = BIT(7);
    // printf("GPIOA & BIT(7) = %d\n", GPIOA & BIT(7));

    func_run();
    return 0;
}

//升级完成
void update_complete(int mode)
{
    printf("update_complete: %d\n", mode);
    bsp_update_init();
    if (mode == 0)
    {
        WDT_DIS();
        while (1);
    }
    WDT_RST();
}
