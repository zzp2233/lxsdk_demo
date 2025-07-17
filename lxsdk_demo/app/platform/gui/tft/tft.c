#include "include.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"
#include "ute_module_gui_common.h"
#include "ute_application_common.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

tft_cb_t tft_cb;

AT(.com_text.tft_spi)
static void tft_te_refresh(void)
{
    TICK0CNT = 0;
    compo_cb.rtc_cnt = RTCCNT;
    compo_cb.rtc_cnt2 = RTCCON2;
    compo_cb.rtc_update = true;
    os_gui_draw();
}

AT(.com_text.tft_spi)
void tft_bglight_en(void)
{
    LCD_BL_EN();            //LCD BL(开背光)
}

AT(.com_text.tft_spi)
void tft_te_isr(void)
{
    if (WKUPEDG & BIT(16+PORT_TFT_INT_VECTOR))
    {
        WKUPCPND = BIT(16+PORT_TFT_INT_VECTOR);
        bool flag_mode_nochange = true;
        if (tft_cb.te_mode != tft_cb.te_mode_next)
        {
            tft_cb.te_mode = tft_cb.te_mode_next;
            tft_cb.despi_baud = (tft_cb.te_mode > 0) ? tft_cb.despi_baud1 : tft_cb.despi_baud2;
            if (tft_cb.flag_in_frame)
            {
                tft_cb.tft_set_baud_kick = true;
            }
            else
            {
                DESPIBAUD = tft_cb.despi_baud;
            }
            flag_mode_nochange = false;
        }
        if (tft_cb.te_mode == 1)
        {
            //1TE MODE
            TICK1CNT = 0;
            TICK1CON |= BIT(0);         //TICK EN
        }
        else
        {
            //>1TE MODE, 如果Mode change停一个TE
            if (flag_mode_nochange)
            {
                tft_te_refresh();
            }
        }
        //延时打开背光
        if (tft_cb.te_bglight_cnt > 0)
        {
            tft_cb.te_bglight_cnt--;
            if (tft_cb.te_bglight_cnt == 0)
            {
                tft_cb.tft_bglight_first_set = true;
            }
        }
    }
}

AT(.com_text.tft_spi)
void tick_te_isr(void)
{
    if (TICK1CON & BIT(9))
    {
        TICK1CON &= ~BIT(0);
        TICK1CPND = BIT(9);
        tft_te_refresh();
    }
}

//将TE量化到0~100, 由于TE中断不及时，可能偶尔会超过100
AT(.com_text.tft_spi)
int tft_te_getnorm(void)
{
    return TICK0CNT * 100 * 64 / (int)((XOSC_CLK_HZ / 1000) * TFT_TE_CYCLE);
}

//设置1TE / 2TE的波特率
void tft_set_baud(u8 baud1, u8 baud2)
{
    tft_cb.despi_baud1 = baud1;
    tft_cb.despi_baud2 = baud2;
    tft_cb.despi_baud = tft_cb.te_mode ? tft_cb.despi_baud1 : tft_cb.despi_baud2;
    DESPIBAUD = tft_cb.despi_baud;
}

AT(.com_text.tft_spi)
void tft_frame_start(void)
{
    tft_cb.flag_in_frame = true;
#if (GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
    //3w-9bit 1line mode
    DESPICON = BIT(27) | BIT(18) | BIT(7) | BIT(0);     //[28:27]IN RGB565, [18]3w-9b despi mode enable, [7]IE, [3:2]1bit in, 1bit out, [0]EN
#endif
#if (GUI_SELECT == DISPLAY_UTE)
    uteModulePlatformScreenWriteDataStart();
#else
    //tft_write_cmd12(0x2C);      //TFT_RAMWR
    tft_write_data_start();
    // uteDrvScreenCommonSetWindow(0,0,UTE_DRV_SCREEN_WIDTH,UTE_DRV_SCREEN_HEIGHT);
#if (GUI_SELECT == GUI_TFT_240_ST7789)
    TFT_SPI_DATA_EN();
#endif
#endif
}

AT(.com_text.tft_spi)
void tft_frame_end(void)
{
#if (GUI_SELECT == DISPLAY_UTE)
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
#else
    tft_write_end();
#endif
    tft_cb.flag_in_frame = false;
    if (tft_cb.tft_bglight_kick)
    {
        tft_cb.tft_bglight_kick = false;
        tft_cb.te_bglight_cnt = 3; //3TE后打开背光
    }
    if (tft_cb.tft_set_baud_kick)
    {
        tft_cb.tft_set_baud_kick = false;
        DESPIBAUD = tft_cb.despi_baud;
    }
}


//OLED设置背光
void oled_brightness_set_level(uint8_t level, bool stepless_en)
{
    sys_cb.light_level = level;
    if (!stepless_en)
    {
        level = level * 50;
    }
    else
    {
        uint16_t level_tmp = 0xff * level / 100;
        level = (uint8_t)level_tmp;
    }

    oled_set_brightness(level);
}


//设置背光亮度
void tft_bglight_set_level(uint8_t level, bool stepless_en)
{
    int8_t  base_duty = 0;  //根据限流电阻调整占空比增益
    int8_t  duty = 0;

    if(!stepless_en)
    {
        level = level * (DEFAULT_BACK_LIGHT_PERCENT_MAX / (DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE));
    }
    if(DEFAULT_BACK_LIGHT_PERCENT_MAX < level)
    {
        level = DEFAULT_BACK_LIGHT_PERCENT_MAX;
    }
    else if (DEFAULT_BACK_LIGHT_PERCENT_MIN > level)
    {
        level = DEFAULT_BACK_LIGHT_PERCENT_MIN;
    }

    duty = base_duty + level;
    tft_cb.tft_bglight_duty = duty;
    if (tft_cb.tft_bglight_last_duty != tft_cb.tft_bglight_duty)
    {
#if (GUI_SELECT == DISPLAY_UTE)
        uteDrvScreenCommonOpenBacklight(tft_cb.tft_bglight_duty);
#else
        bsp_pwm_freq_set(177);
        bsp_pwm_duty_set(PORT_TFT_BL, tft_cb.tft_bglight_duty, false);
#endif
        tft_cb.tft_bglight_last_duty = tft_cb.tft_bglight_duty;
        sys_cb.light_level = tft_cb.tft_bglight_duty / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
    }
}

//背光亮度初始设置检测
void tft_bglight_frist_set_check(void)
{
    if(!tft_cb.tft_bglight_first_set)
    {
        return ;
    }
    if(!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    extern bool msgbox_frist_set_check(void);
    if (msgbox_frist_set_check())
    {
        return;
    }
    tft_cb.tft_bglight_first_set = false;

    tft_cb.tft_bglight_last_duty = 0;
    //todo:后续根据客户定制调整
    if(0 == tft_cb.tft_bglight_duty)
    {
        tft_cb.tft_bglight_duty = uteModuleGuiCommonGetBackLightPercent();
    }
#ifdef GUI_USE_TFT
    tft_bglight_set_level(tft_cb.tft_bglight_duty,true);
#endif
}

//设置TE MODE, 0为2 TE模式, 1为1 TE模式, 2为复杂界面专用模式
void tft_set_temode(u8 mode)
{
    tft_cb.te_mode_next = mode;
    printf("tft_set_temode:%d\n",mode);
}

/**
 * @brief   初始化 TFT 屏幕（包括时钟、IO、SPI、TE中断、驱动IC 等）
 * @note    适配多种屏幕型号与接口模式，支持 RGB、SPI、OLED、RGBW 等
 * @retval  None
 */
void tft_init(void)
#if 0
    void clr_tft_spi_timeout(void);
    clr_tft_spi_timeout();                                                    // 可选逻辑：清除 TFT SPI 超时标志，默认关闭
#endif

#if (GUI_SELECT != DISPLAY_UTE)                                              // 如果不是UTE平台（即使用 SPI/RGB 接口）

    // ----------------------------------------------
    // 1. 时钟初始化
    // ----------------------------------------------
    CLKDIVCON2 = (CLKDIVCON2 & ~(BIT(4) * 0xF)) | BIT(4) * 2;                 // 设置 TFTDE 分频器时钟，分频因子 = 2
    CLKGAT2 |= BIT(4);                                                       // 打开 TFTDE 时钟门控
    RSTCON0 |= BIT(8);                                                       // 释放 TFTDE 模块复位

    port_tft_init();                                                         // 初始化 TFT 所使用的引脚（IO口功能/方向配置）

    // ----------------------------------------------
    // 2. TE信号中断（VSync）配置
    // ----------------------------------------------
    port_irq_register(PORT_TFT_INT_VECTOR, tft_te_isr);                      // 注册 TE 引脚中断回调函数
    port_wakeup_init(PORT_TFT_INT, 1, 1);                                    // 配置 TE 引脚为下降沿唤醒，中断唤醒模式

    // ----------------------------------------------
    // 3. Tick定时器配置（用于TE同步）
    // ----------------------------------------------
    CLKGAT0 |= BIT(28);                                                      // 使能 Tick 模块时钟
    delay_us(1);                                                             // 延时等待时钟配置生效

    TICK0CON = BIT(6) | BIT(5) | BIT(2);                                     // TICK0：使用 xosc26m 时钟，分频64，禁中断
    TICK0PR = 0xFFFF;                                                        // TICK0周期寄存器最大
    TICK0CNT = 0;
    TICK0CON |= BIT(0);                                                      // 启动 TICK0 计时器

    TICK1CON = BIT(7) | BIT(6) | BIT(5) | BIT(2);                            // TICK1：使能中断+分频64+xosc26m
    TICK1PR = (int)((XOSC_CLK_HZ / 1000) * TFT_TE_CYCLE_DELAY) / 64;         // 设置 TICK1 比较值，对应 TE周期
    TICK1CNT = 0;
    sys_irq_init(IRQ_TE_TICK_VECTOR, 0, tick_te_isr);                        // 注册 TE Tick 的中断处理函数

    // ----------------------------------------------
    // 4. TE 模式设置（首次 or 恢复）
    // ----------------------------------------------
    static bool frist_init_flag = false;
    if (frist_init_flag == false)                                            // 第一次初始化TFT
    {
        tft_cb.te_mode = 0;                                                  // 清除 TE 状态
        tft_cb.te_mode_next = 0;
        tft_set_temode(DEFAULT_TE_MODE);                                     // 设置默认 TE 模式
    }
    else                                                                     // 第二次初始化（恢复）
    {
        tft_set_temode(tft_cb.te_mode);                                      // 恢复上一次 TE 模式
    }
    frist_init_flag = true;

    // ----------------------------------------------
    // 5. 执行屏幕硬件复位
    // ----------------------------------------------
    port_tft_reset();

    // ----------------------------------------------
    // 6. 配置 DESPI 控制寄存器
    // ----------------------------------------------
#if (GUI_SELECT == GUI_TFT_240_ST7789)
    DESPICON = BIT(27) | BIT(9) | BIT(7) | BIT(0);                           // RGB565, MultiBit, 中断使能, 1BIT传输, 启用
#elif (GUI_SELECT == GUI_TFT_JD9853)
    DESPICON = BIT(27) | BIT(9) | BIT(7) | BIT(3) | BIT(2) | BIT(0);         // RGB565, MultiBit, 中断, 4BIT传输
#elif (GUI_SELECT == GUI_TFT_RGBW_320_ST77916)
    DESPICON = BIT(27) | BIT(25) | BIT(9) | BIT(7) | BIT(3) | BIT(2) | BIT(0); // 启用RGBW，RGB565，MultiBit，中断
#elif (GUI_SELECT == GUI_TFT_240_296_NV3030B)
    DESPICON = BIT(27) | BIT(9) | BIT(7) | BIT(3) | BIT(2) | BIT(0);
#elif (GUI_SELECT == GUI_TFT_320_385_GV9B71)
    DESPICON = BIT(27) | BIT(9) | BIT(7) | BIT(3) | BIT(2) | BIT(0);
#elif (GUI_SELECT == GUI_TFT_SPI)
    DESPICON = BIT(27) | BIT(7) | BIT(0);                                    // SPI 模式下基本配置
#if (GUI_MODE_SELECT == MODE_3WIRE_9BIT || GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
    DESPICON |= BIT(18);                                                    // 启用 3线 9bit 模式
#elif (GUI_MODE_SELECT == MODE_QSPI)
    DESPICON |= BIT(9) | BIT(3) | BIT(2);                                    // QSPI 模式：MultiBit，4BIT in/out
#endif
#else
    DESPICON = BIT(27) | BIT(9) | BIT(7) | BIT(3) | BIT(2) | BIT(0);         // 默认：RGB565, MultiBit, 中断, 4BIT
#endif

    sys_irq_init(IRQ_DESPI_VECTOR, 0, tft_spi_isr);                          // 注册 DESPI 的中断服务函数（SPI传输完成）

    DESPIBAUD = 15;                                                          // 先设置 SPI 波特率为较低频率，用于读ID
    TRACE("TFT ID: %x\n", tft_read_id());                                    // 打印读取到的屏幕ID
    DESPIBAUD = tft_cb.despi_baud;                                           // 设置为用户配置的最终波特率

#endif  // GUI_SELECT != DISPLAY_UTE

// ----------------------------------------------
// 7. 根据屏幕型号调用对应初始化函数（驱动IC）
// ----------------------------------------------
#if (GUI_SELECT == GUI_TFT_320_ST77916)
    tft_320_st77916_init();
#elif (GUI_SELECT == GUI_TFT_RGBW_320_ST77916)
    tft_rgbw_320_st77916_init();
#elif (GUI_SELECT == GUI_TFT_360_GC9C01)
    tft_360_gc9c01_init();
#elif (GUI_SELECT == GUI_OLED_466_ICNA3310B)
    oled_466_icna3310b_init();
#elif (GUI_SELECT == GUI_TFT_JD9853)
    tft_240_jd9853_init();
#elif (GUI_SELECT == GUI_TFT_240_ST7789)
    tft_240_st7789_init();
#elif (GUI_SELECT == GUI_TFT_240_ST7789W3)
    tft_240_st7789w3_init();
#elif (GUI_SELECT == GUI_TFT_240_296_NV3030B)
    tft_spi_nv3030b_init();
#elif (GUI_SELECT == GUI_TFT_320_385_GV9B71)
    tft_spi_gc9b71_init();
#elif (GUI_SELECT == GUI_TFT_SPI)
    tft_spi_init();
#elif (GUI_SELECT == GUI_TFT_170_560_AXS15231B)
    tft_170_560_axs15231B_init();
#elif (GUI_SELECT == GUI_TFT_240_JD9853W3)
    tft_240_jd9853w3_init();
#elif (GUI_SELECT == DISPLAY_UTE)
    uteDrvScreenCommonInit();                                                // 若为UTE平台，调用公共初始化函数
#else
#error "Please Select GUI Display"
#endif

// ----------------------------------------------
// 8. 设置屏幕窗口区域（显示区域）
// ----------------------------------------------
#if (GUI_SELECT == DISPLAY_UTE)
    uteDrvScreenCommonSetWindow(0, 12, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT + 12); // UTE平台偏移12
#else
    tft_set_window(0, 0, GUI_SCREEN_WIDTH - 1, GUI_SCREEN_HEIGHT - 1);            // 其他平台设置窗口为全屏
#endif

#if (GUI_SELECT == GUI_TFT_240_JD9853W3)
    LCD_BL_EN();                                                             // 特定屏幕初始化后开启背光引脚
#endif

// ----------------------------------------------
// 9. 设置延迟打开背光标志位
// ----------------------------------------------
tft_cb.tft_bglight_kick = true;                                             // 标志：可以在其他地方延迟开启背光

#ifdef GUI_USE_OLED
    oled_brightness_set_level(5, false);                                    // OLED屏幕设置初始亮度
#endif





void tft_exit(void)
{
#ifdef GUI_USE_TFT
#if (GUI_SELECT == DISPLAY_UTE)
    uteDrvScreenCommonDisplayOff();
#else
    bsp_pwm_disable(PORT_TFT_BL); //关背光
#endif
#endif
    port_tft_exit();
    port_irq_free(PORT_TFT_INT_VECTOR);
    port_wakeup_exit(PORT_TFT_INT);
    DESPICON = 0;
    TICK0CON = 0;
    TICK1CON = 0;
    RSTCON0 &= ~BIT(8);
    CLKGAT2 &= ~BIT(4);
}

