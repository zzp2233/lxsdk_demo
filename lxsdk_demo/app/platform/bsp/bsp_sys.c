#include "include.h"
#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_task_application.h"
#include "ute_drv_gsensor_common.h"
#include "ute_module_message.h"
#include "ute_module_sport.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_EVT(...)          printf(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_EVT(...)
#endif

xcfg_cb_t xcfg_cb;
sys_cb_t sys_cb AT(.buf.bsp.sys_cb);
volatile int micl2gnd_flag;
volatile u32 ticks_50ms;

uint32_t cfg_dev_feat = (DEV_FEAT_SDCARD*MUSIC_SDCARD_EN) | (DEV_FEAT_USB*(MUSIC_UDISK_EN|FUNC_USBDEV_EN));
uint8_t cfg_spiflash_speed_up_en = SPIFLASH_SPEED_UP_EN;        //SPI FLASH提速。部份FLASH不支持提速

extern u32 __comm_start, __comm_end, __comm_size, __sram_size;
extern u32 __disp_stack_size, __disp_size, __disp_start, __disp_end;
extern u32 __bss_start, __bss_end, __bss_size;

void sd_detect(void);
void tbox_uart_isr(void);
#if USER_IO_QEDC_EN
void bsp_qdec_io_process(void);
#endif
static u8 heap_func[HEAP_FUNC_SIZE] AT(.heap.func);

#if TRACE_EN
AT(.com_text.str_sddet)
const char str_sd_insert[] = "->sd insert\n";
AT(.com_text.str_sddet)
const char str_sd_remove[] = "->sd remove\n";
AT(.com_text.str_usbdet)
const char str_udisk_insert[] = "->udisk insert\n";
AT(.com_text.str_usbdet)
const char str_udisk_remove[] = "->udisk remove\n";
AT(.com_text.str_usbdet)
const char str_pc_insert[] = "->pc insert\n";
AT(.com_text.str_usbdet)
const char str_pc_remove[] = "->pc remove\n";
#endif

#if (FUNC_MUSIC_EN || FUNC_RECORDER_EN) && SD_SOFT_DETECT_EN
void sd_soft_cmd_detect(u32 check_ms); //check_ms 时间间隔检测一次.  //主循环中执行检测.

AT(.text.bsp.sys)
void sd_soft_detect_poweron_check(void)  //开机检测
{
    if (!SD_IS_SOFT_DETECT())    //配置工具中是否配置sd检测.
    {
        return;
    }
    dev_delay_times(DEV_SDCARD, 1);  //检测到1次成功, 则认为SD在线.
    u8 i = 5;
    while(i--)
    {
        sd_soft_cmd_detect(0);
        if (dev_is_online(DEV_SDCARD))
        {
            break;
        }
        delay_ms(10);
    }
    dev_delay_times(DEV_SDCARD, 3);
}

AT(.text.bsp.sys)
void sd_soft_cmd_detect(u32 check_ms) //check_ms 时间间隔检测一次.  //主循环中执行检测.
{
    static u32 check_ticks = 0;
    if (!SD_IS_SOFT_DETECT())    //配置工具中是否配置sd检测.
    {
        return;
    }
    if (tick_check_expire(check_ticks, check_ms) || (0 == check_ticks))    //每隔100ms才检测一次.
    {
        check_ticks = tick_get();
    }
    else
    {
        return;
    }

//    printf(sd_str);

    if (sd_soft_detect())
    {
        if (dev_online_filter(DEV_SDCARD))
        {
            sd_insert();
            msg_enqueue(EVT_SD_INSERT);
            printf("sd soft insert\n");
        }
    }
    else
    {
        if (dev_offline_filter(DEV_SDCARD))
        {
            sd_remove();
            msg_enqueue(EVT_SD_REMOVE);
        }
    }
}

#endif

#if MUSIC_SDCARD_EN
AT(.com_text.detect)
void sd_detect(void)
{
    if ((!dev_sdcard_is_support()) || (IS_DET_SD_BUSY()))
    {
        return;
    }

#if SD_SOFT_DETECT_EN
    if (SD_IS_SOFT_DETECT())
    {
        return;
    }
#endif

    if (SD_IS_ONLINE())
    {
        if (dev_online_filter(DEV_SDCARD))
        {
            sd_insert();
            msg_enqueue(EVT_SD_INSERT);
            TRACE_EVT(str_sd_insert);
        }
    }
    else
    {
        if (dev_offline_filter(DEV_SDCARD))
        {
            sd_remove();
            msg_enqueue(EVT_SD_REMOVE);
            TRACE_EVT(str_sd_remove);
        }
    }
}
#endif // MUSIC_SDCARD_EN

#if USB_SUPPORT_EN
AT(.com_text.detect)
void usb_detect(void)
{
    if (!dev_usb_is_support())
    {
        return;
    }

#if FUNC_USBDEV_EN
    u8 usb_sta = usbchk_connect(USBCHK_OTG);
#else
    u8 usb_sta = usbchk_connect(USBCHK_ONLY_HOST);
#endif

    if (usb_sta == USB_UDISK_CONNECTED)
    {
        if (dev_online_filter(DEV_UDISK))
        {
            udisk_insert();
            msg_enqueue(EVT_UDISK_INSERT);
            TRACE_EVT(str_udisk_insert);
        }
    }
    else
    {
        if (dev_offline_filter(DEV_UDISK))
        {
            udisk_remove();
            msg_enqueue(EVT_UDISK_REMOVE);
            TRACE_EVT(str_udisk_remove);
        }
    }
#if FUNC_USBDEV_EN
    if (usb_sta == USB_PC_CONNECTED)
    {
        if (dev_online_filter(DEV_USBPC))
        {
            msg_enqueue(EVT_PC_INSERT);
            TRACE_EVT(str_pc_insert);
        }
    }
    else
    {
        if (dev_offline_filter(DEV_USBPC))
        {
            msg_enqueue(EVT_PC_REMOVE);
            pc_remove();
            TRACE_EVT(str_pc_remove);
        }
    }
#endif
}
#endif // USB_SUPPORT_EN

#if PWRKEY_2_HW_PWRON
//软开机模拟硬开关，松开PWRKEY就关机。
AT(.com_text.detect)
void pwrkey_2_hw_pwroff_detect(void)
{
    static int off_cnt = 0;
    if (!PWRKEY_2_HW_PWRON)
    {
        return;
    }

    if (IS_PWRKEY_PRESS())
    {
        off_cnt = 0;
    }
    else
    {
        if (off_cnt < 10)
        {
            off_cnt++;
        }
        else if (off_cnt == 10)
        {
            //pwrkey已松开，需要关机
            off_cnt = 20;
            sys_cb.pwrdwn_hw_flag = 1;
            sys_cb.poweron_flag = 0;
        }
    }
}
#endif // PWRKEY_2_HW_PWRON

//timer tick interrupt(1ms)
AT(.com_text.timer)
void usr_tmr1ms_isr(void)
{
#if LED_DISP_EN
    port_2led_scan();
#endif // LED_DISP_EN

    bsp_saradc_tmr1ms_process();
#if USER_IO_QEDC_EN
    bsp_qdec_io_process();
#endif
    plugin_tmr1ms_isr();
}

//timer tick interrupt(5ms)
AT(.com_text.timer)
void usr_tmr5ms_isr(void)
{
    tmr5ms_cnt++;
    //5ms timer process
    dac_fade_process();
    bsp_key_scan();
#if PWRKEY_2_HW_PWRON
    pwrkey_2_hw_pwroff_detect();
#endif

    plugin_tmr5ms_isr();

#if MUSIC_SDCARD_EN
    sd_detect();
#endif

#if USB_SUPPORT_EN
    usb_detect();
#endif

    //20ms timer process
    if ((tmr5ms_cnt % 4) == 0)
    {
#if DAC_DNR_EN
        dac_dnr_process();
#endif // DAC_DNR_EN
    }

    //50ms timer process
    if ((tmr5ms_cnt % 10) == 0)
    {
        co_timer_pro(2);
        ticks_50ms++;
#if LED_DISP_EN
        led_scan();
#endif
    }

    //100ms timer process
    if ((tmr5ms_cnt % 20) == 0)
    {
#if UDE_HID_EN
        if (func_cb.sta == FUNC_USBDEV)
        {
            ude_tmr_isr();
        }
#endif // UDE_HID_EN
        lowpwr_tout_ticks();
        if (sys_cb.lpwr_cnt > 0)
        {
            sys_cb.lpwr_cnt++;
        }
    }

    //500ms timer process
    if ((tmr5ms_cnt % 100) == 0)
    {
        msg_enqueue(MSG_SYS_500MS);
        msg_enqueue(MSG_SYS_BOX_500MS);
        sys_cb.cm_times++;
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_ELLIPSIS_TIMER_SUPPORT
        if(uteModuleSportAlgoTimerIsRunning())
        {
            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_SPORT_ALGO_INPUT_DATA_TIMER,0);
        }
#endif
    }

    //1s timer process
    if ((tmr5ms_cnt % 200) == 0)
    {
        msg_enqueue(MSG_SYS_1S);
        tmr5ms_cnt = 0;
        sys_cb.lpwr_warning_cnt++;
#if !UTE_MODULE_CREATE_SYS_1S_TIMER_SUPPORT
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_TIME_SEC_BASE, 0);
#endif
    }
}

AT(.com_text.bsp.sys)
void bsp_loudspeaker_mute(void)
{
    LOUDSPEAKER_MUTE();
    sys_cb.loudspeaker_mute = 1;
    delay_5ms(4);
    dac_set_power_on_off(0);
}

AT(.com_text.bsp.sys)
void bsp_loudspeaker_unmute(void)
{
    sys_cb.loudspeaker_mute = 0;
    LOUDSPEAKER_UNMUTE();
}

//手动控制mute才需要调用(其它功放MUTE控制在淡入淡出流程里)
AT(.text.bsp.sys)
void bsp_sys_mute(void)
{
    if (!sys_cb.mute)
    {
        sys_cb.mute = 1;
        dac_fade_out();
        bsp_loudspeaker_mute();
#if DAC_DNR_EN
        dac_dnr_set_sta(0);
#endif
    }
}

AT(.text.bsp.sys)
void bsp_sys_unmute(void)
{
    if (sys_cb.mute)
    {
        sys_cb.mute = 0;
        bsp_loudspeaker_unmute();
        dac_fade_in();
#if DAC_DNR_EN
        dac_dnr_set_sta(sys_cb.dnr_sta);
#endif
    }
}

AT(.text.bsp.sys)
void bsp_clr_mute_sta(void)
{
    if (sys_cb.mute)
    {
        sys_cb.mute = 0;
#if DAC_DNR_EN
        dac_dnr_set_sta(sys_cb.dnr_sta);
#endif
    }
}

AT(.text.bsp.sys)
bool bsp_get_mute_sta(void)
{
    return sys_cb.mute;
}

//此接口用于更换不同规格晶振后出现的休眠蓝牙掉线问题，原因是驱动电流太小导致晶振稳定时间变长导致
//解决办法是重定义以下接口以增加晶振的驱动电流，达到快速稳定的目的
//50uA/step, 非线性, 档位越大, 逐渐变为20+uA/step, 默认是2, 即100ua
//#define DI_XOSC_DRV(bit)                     XOSCCON = (XOSCCON & ~(31<<0)) | ((bit)<<0)
//void xosc_driver_init(void)
//{
//     DI_XOSC_DRV(2);
//}

//是否使能船运模式
AT(.text.bsp.sys)
void bsp_set_shipping_mode(bool en)
{
    sys_cb.flag_shipping_mode = en;
}

////PWRKEY 引脚通过开关80K上拉/10K下拉, 来实现输出高低电平, 注意高电平为1.8V
//AT(.com_text.bsp.sys)
//void powerkey_out_set(bool high)  //里面调用到的函数都在公共区.
//{
//    if (high) {
//        RTCCON1 &= ~(BIT(1)| BIT(2) | BIT(3)) ;  //bit1: 10K PD DISABLE  //bit2,bit3 = 00, PU IS 80K
//        RTCCON1 |= BIT(4) ;                      //PU ENABLE,
//    } else {
//        RTCCON1 &= ~BIT(4) ;      //PU DISABLE
//        RTCCON1 |= BIT(1) ;       //10K PD ENABLE
//        if (!(RTCCON11 & BIT(4))){ //PD enable protect bit.
//            RTCCON11 |= BIT(4);
//        }
//    }
//}

AT(.text.bsp.sys)
u8 get_cur_sysclk(void)
{
    return SYS_CLK_SEL;
}

AT(.text.bsp.sys.init)
bool rtc_init(void)
{
    u32 temp;
    sys_cb.rtc_first_pwron = 0;
    printf("RTCCON0: %x, RTCCNT: %x\n", RTCCON0, RTCCNT);

    temp = RTCCON0;
    temp |= BIT(9) | BIT(8);                        //rtc clk sel x26m div
    temp &= ~BIT(6);                                //enable clk32k_rtc
    temp &= ~BIT(2);                                //disable osc_32k out
    temp &= ~BIT(14);                               //x24div select x24m_clkdiv12
#if LP_XOSC_CLOCK_EN
    temp |= BIT(14);                                //xoscdiv select lp_xosc_2m
#endif
    RTCCON0 = temp;
    RTCCON2 = XOSC_CLK_HZ / 12 / 100 - 1;           //XOSC_CLK_HZ / 12 / 100 - 1
    if (temp & BIT(7))
    {
        temp &= ~BIT(7);                            //clear first poweron flag
        RTCCON0 = temp;
        sys_cb.rtc_first_pwron = 1;
        rtc_clock_init();
        printf("rtc 1st pwrup\n");
        return false;
    }

    temp = bsp_rtc_recode_get();
    if (temp)
    {
        RTCCNT = temp;
        printf("rtc wdt or 10s wk_rst,load ram cache rtc.[rtccnt:%d]\n",temp);
    }

    return true;
}

//UART0打印信息输出GPIO选择，UART0默认G1(PA7)
void uart0_mapping_sel(void)
{
    //等待uart0发送完成
    if(UART0CON & BIT(0))
    {
        while (!(UART0CON & BIT(8)));
    }
    GPIOBPU  &= ~(BIT(2) | BIT(3) | BIT(4));
    FUNCMCON0 = (0xf << 12) | (0xf << 8);           //clear uart0 mapping

#if (UART0_PRINTF_SEL == PRINTF_PB3)
    GPIOBDE  |= BIT(3);
    GPIOBPU  |= BIT(3);
    GPIOBDIR |= BIT(3);
    GPIOBFEN |= BIT(3);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB3;           //RX0 Map To TX0, TX0 Map to G1
#elif (UART0_PRINTF_SEL == PRINTF_PA1)
    GPIOADE  |= BIT(1);
    GPIOAPU  |= BIT(1);
    GPIOADIR |= BIT(1);
    GPIOAFEN |= BIT(1);
    GPIOADRV |= BIT(1);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA1;           //RX0 Map To TX0, TX0 Map to G2
#elif (UART0_PRINTF_SEL == PRINTF_PA3)
    GPIOADE  |= BIT(3);
    GPIOAPU  |= BIT(3);
    GPIOADIR |= BIT(3);
    GPIOAFEN |= BIT(3);
    GPIOADRV |= BIT(3);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA3;           //RX0 Map To TX0, TX0 Map to G3
#elif (UART0_PRINTF_SEL == PRINTF_PE3)
    GPIOEDE  |= BIT(3);
    GPIOEPU  |= BIT(3);
    GPIOEDIR |= BIT(3);
    GPIOEFEN |= BIT(3);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PE3;           //RX0 Map To TX0, TX0 Map to G4
#elif (UART0_PRINTF_SEL == PRINTF_PE5)
    GPIOEDE  |= BIT(5);
    GPIOEPU  |= BIT(5);
    GPIOEDIR |= BIT(5);
    GPIOEFEN |= BIT(5);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PE5;           //RX0 Map To TX0, TX0 Map to G5
#elif (UART0_PRINTF_SEL == PRINTF_PE7)
    GPIOEDE  |= BIT(7);
    GPIOEPU  |= BIT(7);
    GPIOEDIR |= BIT(7);
    GPIOEFEN |= BIT(7);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PE7;           //RX0 Map To TX0, TX0 Map to G6
#elif (UART0_PRINTF_SEL == PRINTF_PF5)
    GPIOFDE  |= BIT(5);
    GPIOFPU  |= BIT(5);
    GPIOFDIR |= BIT(5);
    GPIOFFEN |= BIT(5);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PF5;           //RX0 Map To TX0, TX0 Map to G7
#elif (UART0_PRINTF_SEL == PRINTF_VUSB)
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_VUSB;          //RX0 Map To TX0, TX0 Map to G8
#endif
}


AT(.rodata.vol)
const u8 maxvol_tbl[2] = {16, 32};

//开user timer前初始化的内容
AT(.text.bsp.sys.init)
static void bsp_var_init(void)
{
    memset(&sys_cb, 0, sizeof(sys_cb));
    func_heap_init(heap_func, HEAP_FUNC_SIZE);

    sys_cb.loudspeaker_mute = 1;
    sys_cb.ms_ticks = tick_get();
    xcfg_cb.vol_max = maxvol_tbl[xcfg_cb.vol_max];
    sys_cb.hfp2sys_mul = (xcfg_cb.vol_max + 2) / 16;
    if (SYS_INIT_VOLUME > xcfg_cb.vol_max)
    {
        SYS_INIT_VOLUME = xcfg_cb.vol_max;
    }
    if (WARNING_VOLUME > xcfg_cb.vol_max)
    {
        WARNING_VOLUME = xcfg_cb.vol_max;
    }

#if (WORK_MODE == MODE_BQB_RF_BREDR)
    xcfg_cb.sys_sleep_time =  0;
#endif

    sys_cb.sleep_time = -1L;
    sys_cb.pwroff_time = -1L;
    sys_cb.guioff_delay = GUI_OFF_DELAY_TIME;
    if (xcfg_cb.sys_sleep_time != 0)
    {
        sys_cb.sleep_time = (u32)xcfg_cb.sys_sleep_time * 10;   //100ms为单位
    }
//    if (xcfg_cb.sys_off_time != 0) {
//        sys_cb.pwroff_time = (u32)xcfg_cb.sys_off_time * 10;    //100ms为单位
//    }

    sys_cb.sleep_delay = -1L;
    sys_cb.pwroff_delay = -1L;
    sys_cb.sleep_en = 0;
    sys_cb.lpwr_warning_times = LPWR_WARING_TIMES;

    if ((!xcfg_cb.ft_osc_cap_en) || (xcfg_cb.osci_cap == 0) || (xcfg_cb.osco_cap == 0) ||//没有过产测时，使用自定义OSC电容
        (xcfg_cb.osci_cap != xcfg_cb.osco_cap) || (xcfg_cb.osci_cap > 63) || (xcfg_cb.osco_cap > 63))       //产测值非法继续使用默认值
    {
        xcfg_cb.osci_cap = xcfg_cb.uosci_cap;
        xcfg_cb.osco_cap = xcfg_cb.uosco_cap;
    }

    port_var_init();
    key_var_init();
    plugin_var_init();
    msg_queue_init();
    saradc_var_init();
    sdadc_var_init();

    dev_init();
#if FUNC_REC_EN
    bsp_record_var_init();
#endif
#if (MUSIC_UDISK_EN || MUSIC_SDCARD_EN)
    fs_var_init();
#endif

    dac_cb_init((DAC_CH_SEL & 0x0f) | ((u8)DAC_LDOH_SEL << 4) | (0x100 * DAC_FAST_SETUP_EN) | (0x200 * DACVDD_BYPASS_EN));
}

//获取系统控制位
u8 bsp_sys_get_ctlbit(uint n)
{
    if (n >= SYS_CTL_TOTAL_BITS)
    {
        halt(HALT_BSP_SYS_CTLBITS);
    }
    int by = n / 8;
    int bi = n % 8;
    return ((sys_cb.ctl_bits[by] >> bi) & 1);
}

//设置系统控制位
void bsp_sys_set_ctlbit(uint n, u8 v)
{
    if (n >= SYS_CTL_TOTAL_BITS)
    {
        halt(HALT_BSP_SYS_CTLBITS);
    }
    int by = n / 8;
    int bi = n % 8;
    sys_cb.ctl_bits[by] = (sys_cb.ctl_bits[by] & ~BIT(bi)) | (v << bi);
}

//反转系统控制位
void bsp_sys_reverse_ctlbit(uint n)
{
    if (n >= SYS_CTL_TOTAL_BITS)
    {
        halt(HALT_BSP_SYS_CTLBITS);
    }
    int by = n / 8;
    int bi = n % 8;
    sys_cb.ctl_bits[by] ^= BIT(bi);
}

AT(.text.bsp.sys.init)
static void bsp_io_init(void)
{
    GPIOADE = 0;    //UART
    GPIOBDE = 0;
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOGDE = 0x3F; //MCP FLASH

    uart0_mapping_sel();        //调试UART IO选择或关闭

    LOUDSPEAKER_MUTE_INIT();
    AMPLIFIER_SEL_INIT();
#if MUSIC_SDCARD_EN
    SD_DETECT_INIT();
#endif // MUSIC_SDCARD_EN
}

void bsp_get_xosc_xcfg(u8 *osci_cap, u8 *osco_cap, u8 *both_cap)
{
    *osci_cap = xcfg_cb.osci_cap;
    *osco_cap = xcfg_cb.osco_cap;
    *both_cap = xcfg_cb.osc_both_cap;
}

AT(.text.bsp.sys.init)
void bsp_update_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb)))             //获取配置参数
    {
        printf("xcfg init error\n");
    }

    printf("TBOX OTA Update Finish!\n");
}


/*
AT(.com_rodata.isr)
const char str_t3[] = "T3 ";

AT(.com_text.isr) FIQ
void timer3_isr(void)
{
    if (TMR3CON & BIT(16)) {
        TMR3CPND = BIT(16);              //Clear Pending
    }
    printk(str_t3);
}

void timer3_init(void)
{
    TMR3CON =  BIT(7);                  //Timer overflow interrupt enable
    TMR3CNT = 0;
    TMR3PR  = 1000000 / 2 - 1;          //500ms, select xosc26_div 1M clk
    TMR3CON |= BIT(2) | BIT(0);         //Timer works in Counter Mode
    sys_irq_init(IRQ_TMR3_VECTOR, 1, timer3_isr);
}
*/

#if !LP_XOSC_CLOCK_EN
void rtc_printf(void)
{
    tm_t rtc_tm = time_to_tm(RTCCNT);               //更新时间结构体
    printf("time:  %d.%02d.%02d   %02d:%02d:%02d\n", (rtc_tm.year+0), rtc_tm.mon, rtc_tm.day,rtc_tm.hour, rtc_tm.min, rtc_tm.sec);
    printf("### RTCCON0:%x\n", RTCCON0);
}

void rtc_pwd_calibration(void)
{
    param_init(sys_cb.rtc_first_pwron);

    u32 rtccon9 = RTCCON9;
    if((rtccon9 & 1) == 1)                                //1000s起来一次校准rc rtc
    {
        RTCCON9 = 0xfff;                                                    //Clr pending
        if (cm_read8(PARAM_RTC_CAL_VALID) == 1)
        {
            sniff_rc_init();
            rtc_calibration_read(PARAM_RTC_CAL_ADDR);
            rtc_sleep_process();
            rtc_calibration_write(PARAM_RTC_CAL_ADDR);
            cm_write8(PARAM_RTC_CAL_VALID, 1);
            cm_sync();
            rtc_printf();
        }
        sfunc_pwrdown(1);
        return;
    }
    //RTCCON9 = 0xfff;                                                    //Clr pending
}
#endif

void print_comm_info(void)
{
    printf("-------------------sram info----------------------\n");
    printf("sram total = 0x%X, used = 0x%X, remain = 0x%X\n", (u32)&__sram_size, (u32)&__comm_size+(u32)&__bss_size, (u32)&__sram_size - (u32)&__comm_size-(u32)&__bss_size);
    printf("disp total = 0x%X, used = 0x%X, remain = 0x%X\n", (u32)&__disp_size, (u32)&__disp_stack_size, (u32)&__disp_size - (u32)&__disp_stack_size);
    printf("comm: 0x%X ~ 0x%X  size: 0x%X\n", (u32)&__comm_start, (u32)&__comm_end, (u32)&__comm_size);
    printf("bss : 0x%X ~ 0x%X  size: 0x%X\n", (u32)&__bss_start, (u32)&__bss_end, (u32)&__bss_size);
    printf("--------------------------------------------------\n");
}

AT(.text.bsp.sys.init)
void bsp_sys_init(void)
{
    /// config
    if (!xcfg_init(&xcfg_cb, sizeof(xcfg_cb)))             //获取配置参数
    {
        printf("xcfg init error\n");
    }
    print_comm_info();

    // io init
    bsp_io_init();

    // var init
    bsp_var_init();

#if USE_APP_TYPE
    // app_platform_init();
#endif

#if !LP_XOSC_CLOCK_EN
    rtc_pwd_calibration();
#endif

    // power init
    pmu_init(BUCK_MODE_EN*xcfg_cb.buck_mode_en);

    // clock init
    adpll_init(DAC_OUT_SPR);
    sys_clk_set(SYS_CLK_SEL);

    // peripheral init
    param_init(sys_cb.rtc_first_pwron);

    //晶振配置
    xosc_init();

    plugin_init();

    if (POWKEY_10S_RESET)
    {
        RTCCON10 = BIT(10);                     //clear 10s pending
        RTCCON12 &= ~0x0f;                      //10S Reset Enable
    }
    else
    {
        RTCCON12 = (RTCCON12 & ~0xf) | 0x0a;    //10S Reset Disable
    }

#if CHARGE_EN
    if (xcfg_cb.charge_en)
    {
        bsp_charge_init();
    }
#endif // CHARGE_EN

    key_init();

    rtc_init();
    /// enable user timer for display & dac
    sys_set_tmr_enable(1, 1);

    bt_init();

#if BT_BACKSTAGE_EN
    func_bt_init();
#endif

#if CALL_MGR_EN
    bsp_call_mgr_init();
#endif
    dac_init();

    bsp_change_volume(sys_cb.vol);

#if WARNING_POWER_ON
    mp3_res_play(RES_BUF_POWERON_MP3, RES_LEN_POWERON_MP3);
#endif // WARNING_POWER_ON

#if BT_BACKSTAGE_MUSIC_EN
    bt_audio_enable();
#if DAC_DNR_EN
    dac_dnr_set_sta(1);
    sys_cb.dnr_sta = 1;
#endif
#endif
    func_cb.sta = FUNC_CLOCK;

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
    eq_dbg_init();
#endif // EQ_DBG_IN_UART

#if FLASHDB_EN
    printf("flashdb enable\n");
    contacts_kvdb_init();
    msg_tsdb_init();
#endif

#if BT_SCO_APP_DBG_EN
    sco_audio_init();   //先初始化一遍通话参数
    sco_audio_exit();
#endif

    mic_bias_trim_w4_done();
    dac_set_power_on_off(0);            //需要放到MIC TRIM后才能关DAC
#if (SENSOR_STEP_SEL != SENSOR_STEP_NULL || SENSOR_HR_SEL != SENSOR_HR_NULL || SENSOR_GEO_SEL != SENSOR_GEO_NULL)
    i2c_gsensor_init();
    //bsp_sensor_pe2_pwr_pg_on();         //需放在IIC初始化之后，未使用外设时注意关闭
    uteDrvGsensorCommonInit(UTE_DRV_GSENSOR_DEFAULT_ACC_RATE_VALUE,UTE_DRV_GSENSOR_DEFAULT_ACC_RANGE_VALUE);
    bsp_sensor_step_init();             //步数传感器初始化
    bsp_i2c_init();
    bsp_sensor_hr_init(0);
#endif
    // bsp_vbat_percent_init();            //放最后电压稍微稳定点

#if (FUNC_MUSIC_EN || FUNC_RECORDER_EN) && SD_SOFT_DETECT_EN
    CLKGAT0 |= BIT(14);
    CLKCON1 &= ~BIT(10);
    CLKGAT0 |= BIT(9);
    SD0_LDO_EN_EN();
    sd_soft_detect_poweron_check();
#endif

    sys_cb.sys_init_complete = false;

    uteTaskApplicationInit();

    gui_init();
#if ASR_SELECT
    bsp_asr_init();                     //语音初始化
#endif
#if ECIG_POWER_CONTROL
    bsp_ecig_init();
#endif
    uteModulePlatformOutputGpioSet(IO_PH4,true);
#if !UTE_MULTIPLE_LANGUAGE_SUPPORT
    lang_select(LANG_ZH);
#endif
    // ecig_cfg.smoke_power = 15;
#if CHARGE_EX_IC_SELECT
    bsp_charge_ex_init();
#endif
}

