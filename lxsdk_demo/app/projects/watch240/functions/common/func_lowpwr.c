#include "include.h"
#include "func.h"
#include "ute_module_message.h"
#include "ute_project_config.h"
#include "ute_module_sport.h"

bool power_off_check(void);
void lock_code_pwrsave(void);
void unlock_code_pwrsave(void);
bool lp_xosc_check(void);

AT(.com_text.tft_restore_printf)
static const char tft_restore_printf[] = "GUI DESPI INIT ERR =======> To Do Restore TFT\n";

AT(.com_text.sleep)
void lowpwr_tout_ticks(void)
{
    //当屏幕初始化SPI等待不到pending，就重新断电再初始化一次屏幕
#if 0
    u8 get_tft_spi_timeout(void);
    if (get_tft_spi_timeout())
    {
        printf(tft_restore_printf);
        reset_sleep_delay_all();
        gui_sleep();
        delay_5ms(1);
        gui_wakeup();
    }
#endif
    if(sys_cb.sleep_delay != -1L && sys_cb.sleep_delay > 0)
    {
        sys_cb.sleep_delay--;
    }
    if(sys_cb.guioff_delay != -1L && sys_cb.guioff_delay > 0)
    {
        sys_cb.guioff_delay--;
    }
    if(sys_cb.pwroff_delay != -1L && sys_cb.pwroff_delay > 0)
    {
        sys_cb.pwroff_delay--;
    }
}

AT(.com_text.sleep)
bool sys_sleep_check(u32 *sleep_time)
{
    u32 co_min = co_timer_get_min_time(true)*2;

    if(*sleep_time > co_min)
    {
        *sleep_time = co_min;
    }

    if(*sleep_time < 4)
    {
        *sleep_time = 4;
    }

    if(*sleep_time > sys_cb.sleep_wakeup_time)
    {
        *sleep_time = sys_cb.sleep_wakeup_time;
        return true;
    }
    return false;
}

//休眠中ble断开/连接/传输是否需要退出休眠
AT(.com_text.sleep)
bool ble_is_allow_wkup(void)
{
    return LE_ALLOW_WKUP_EN;
}

AT(.sleep_text.sleep)
void sys_sleep_cb(u8 lpclk_type)
{
    //注意！！！！！！！！！！！！！！！！！
    //此函数只能调用sleep_text或com_text函数

    //此处关掉影响功耗的模块
    u32 gpiogde = GPIOGDE;
    if ((gpiogde & BIT(6)) && ((WKPINMAP >> 14) & 0x3) == 0x1)
    {
        GPIOGDE = BIT(2) | BIT(4) | BIT(6);                  //SPICS, SPICLK
    }
    else
    {
        GPIOGDE = BIT(2) | BIT(4);                  //SPICS, SPICLK
    }

    sys_enter_sleep(lpclk_type);                //enter sleep

    //唤醒后，恢复模块功能
    GPIOGDE = gpiogde;
}

AT(.sleep_text.sleep)
void sleep_set_sysclk(uint8_t sys_clk)
{
    uint8_t cur_sys_clk = sys_clk_get();

    if (sys_clk < SYS_24M || cur_sys_clk == sys_clk)
    {
        return;
    }

    if (sys_clk > SYS_24M)
    {
        if (cur_sys_clk <= SYS_24M)
        {
            CLKCON0 = (CLKCON0 & ~(0x03 << 2)) | (0x01 << 2); //sysclk select xosc26m_clk
            RSTCON0 &= ~BIT(4);                         //pllsdm disable
            adpll_init(DAC_OUT_SPR);                    //enable adpll
            adda_clk_source_sel(0);                     //adda_clk48_a select pll0
        }
        sys_clk_set(sys_clk);
    }
    else
    {
        sys_clk_set(SYS_24M);
        DACDIGCON0 &= ~BIT(0);                      //disable digital dac
        adda_clk_source_sel(1);                     //adda_clk48_a select xosc52m
        PLL0CON &= ~(BIT(18) | BIT(6));             //pll0 sdm & analog disable
    }
}

//用于未连接ble休眠后,当ble连接上后更新连接参数用
AT(.sleep_text.sleep)
void sleep_ble_param_check(void)
{
    if (sys_cb.flag_sleep_ble_status != ble_is_connect())
    {
        if ((sys_cb.flag_sleep_ble_status == false) && ble_is_connect())
        {
            ble_update_conn_param(400, 0, 500);     //interval: 400*1.25ms = 500ms
        }
        sys_cb.flag_sleep_ble_status = ble_is_connect();
    }
}


//休眠定时器，500ms进一次
AT(.sleep_text.sleep)
uint32_t sleep_timer(void)
{
    uint32_t ret = 0;

    sys_cb.sleep_counter++;
    if (sys_cb.sleep_counter == 10)
    {
        sys_cb.sleep_counter = 0;

#if LPWR_WARNING_VBAT
        u32 rtccon8 = RTCCON8;
#if CHARGE_VOL_DYNAMIC_DET
        if (RTCCON & BIT(20))                   //vusb is online?
        {
            gradient_process(1);
            RTCCON8 |= BIT(1);                  //charge stop
            delay_5ms(2);
            gradient_process(0);
        }
#endif
        saradc_init();
        saradc_adc15_analog_channel_sleep();
        delay_us(600);
        saradc_kick_start(0, 0);
        while(!bsp_saradc_process(0));
        sys_cb.vbat = bsp_vbat_get_voltage(1);
        RTCCON8 = rtccon8;
        if (sys_cb.vbat < LPWR_WARNING_VBAT)
        {
            //低电需要唤醒sniff mode
            ret = 2;
        }
        saradc_exit();
#endif
    }

#if !UTE_MODULE_CREATE_SYS_1S_TIMER_SUPPORT
    if(sys_cb.sleep_counter % 2 == 0)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_TIME_SEC_BASE, 0);
    }
#endif

#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_ELLIPSIS_TIMER_SUPPORT
    if(uteModuleSportAlgoTimerIsRunning())
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_SPORT_ALGO_INPUT_DATA_TIMER,0);
    }
#endif

#if CHARGE_EN
    if (xcfg_cb.charge_en)
    {
        charge_detect(0);
    }
#endif // CHARGE_EN

    sys_cb.sleep_wakeup_time = -1L;

    if(sys_cb.pwroff_delay != -1L)
    {
        if(sys_cb.pwroff_delay > 5)
        {
            sys_cb.pwroff_delay -= 5;
        }
        else
        {
            sys_cb.pwroff_delay = 0;
            return 1;
        }
    }

    if ((PWRKEY_2_HW_PWRON) && (!IS_PWRKEY_PRESS()))
    {
        ret = 1;
    }

    rtc_sleep_process();
    sleep_ble_param_check();

    return ret;
}
extern bool slider_unlock;
extern bool vc30fx_sleep_isr;
AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    uint32_t status, ret;
    uint32_t usbcon0, usbcon1;
    u8 pa_de, pb_de, pe_de, pf_de, pg_de, ph_de;
    u16 adc_ch;
    uint32_t sysclk;
    u32 wkie, wkpnd;
    bool wko_wkup_flag;
    bool gui_need_wkp = false;
#if LE_EN
    u16 interval = 0;
    u16 latency = 0;
    u16 tout = 0;
    u16 adv_interval = 0;
#endif
    sys_cb.flag_sleep_ble_status = ble_is_connect();

#if VBAT_DETECT_EN
    if (bsp_vbat_get_lpwr_status() == 2)             //低电不进sniff mode
    {
        return;
    }
#endif


    // slider_unlock = false;
    sleep_cb.sys_is_sleep = true;
    sys_cb.gui_need_wakeup = 0;
    bt_enter_sleep();
#if LE_EN
    adv_interval = ble_get_adv_interval();
    ble_set_adv_interval(1600);                  //interval: 500 * 0.625ms = 500ms
    if (ble_is_connect())                       //ble已连接
    {
        interval = ble_get_conn_interval();
        latency = ble_get_conn_latency();
        tout = ble_get_conn_timeout();
        ble_update_conn_param(400, 0, 500);     //interval: 400*1.25ms = 500ms
    }
#endif
#if BT_SINGLE_SLEEP_LPW_EN
    if (!bt_is_connected())                     //蓝牙未连接
    {
        if (bt_get_scan())                      //双模休眠
        {
            bt_update_bt_scan_param(4096, 8, 4096, 12);
        }
        else
        {
            bt_update_bt_scan_param(4096, 0, 4096, 0);
            bt_scan_disable();
        }
    }
#else
    if (!bt_is_connected())                     //蓝牙未连接
    {
        bt_update_bt_scan_param(4096, 12, 2048, 12);
    }
#endif

#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif
    bsp_loudspeaker_mute();
#if ECIG_POWER_CONTROL
    bsp_ecig_exit();
#endif
#if CHARGE_EX_IC_SELECT
    lp7812c_init_exit();
#endif
#if (ASR_SELECT && ASR_FULL_SCENE)
    bsp_asr_stop();
#endif
    dac_power_off();                            //dac power down

    sys_set_tmr_enable(0, 0);

    adc_ch = bsp_saradc_exit();                //close saradc及相关通路模拟
    saradc_clr_channel(adc_ch);
    saradc_set_channel(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP));

#if CHARGE_EN
    bsp_charge_set_stop_time(3600);
    charge_set_detect_cnt(1);
#if CHARGE_VOL_DYNAMIC_DET
    if(sys_cb.chg_on)
    {
        gradient_process(0);
        RTCCON8 &= ~BIT(1);                                  //charge open
    }
#endif
#endif

    usbcon0 = USBCON0;                          //需要先关中断再保存
    usbcon1 = USBCON1;
    USBCON0 = BIT(5);
    USBCON1 = 0;

    gui_sleep();

#if MODEM_CAT1_EN
    bsp_modem_sleep_enter();
#endif

    sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);
    DACDIGCON0 &= ~BIT(0);                      //disable digital dac
    adda_clk_source_sel(1);                     //adda_clk48_a select xosc52m
    PLL0CON &= ~(BIT(18) | BIT(6));             //pll0 sdm & analog disable
    rtc_sleep_enter();

    //io analog input
    pa_de = GPIOADE;
    pb_de = GPIOBDE;
    pe_de = GPIOEDE;
    pf_de = GPIOFDE;
    pg_de = GPIOGDE;
    ph_de = GPIOHDE;
    GPIOADE = 0;
    GPIOBDE = BIT(3);
    GPIOGDE = 0x3F;                             //MCP FLASH
    GPIOHDE = 0;//功耗低了但是灭屏无法充电
    // GPIOHDE = 0| BIT(4); //功耗高，但是灭屏可以充电
    u32 pf_keep = 0;
    printf("bsp_sensor_init_sta_NO\n");
    GPIOEDE = 0;
    GPIOFDE = pf_keep;

    // port_gpio_set_out(IO_PH5,0);
    // port_gpio_set_out(IO_PB1,0);
    // port_gpio_set_in(IO_PB0,GPIOxPU200K);
    // port_gpio_set_in(IO_PA5,GPIOxPU200K);

    wkie = WKUPCON & BIT(16);
    WKUPCON &= ~BIT(16);                        //休眠时关掉WKIE


    sleep_wakeup_config();

    sys_cb.sleep_counter = 0;
    sys_cb.sleep_wakeup_time = -1L;
    sys_cb.gui_screen_wakeup = false;
    while(bt_is_sleep())
    {
        WDT_CLR();
        bt_thread_check_trigger();
        status = bt_sleep_proc();
        // GPIOBSET = BIT(1);
        // GPIOBCLR = BIT(0);

        if(status == 1)
        {
#if UTE_MODULE_HEART_SUPPORT
            if(vc30fx_sleep_isr)
            {
                sleep_set_sysclk(SYS_192M);
            }
#endif
            ret = sleep_timer();
            if(ret)
            {
                if (ret == 1)
                {
                    func_cb.sta = FUNC_PWROFF;
                }
                break;
            }
        }
        wkpnd = port_wakeup_get_status();
        wko_wkup_flag = port_wko_is_wakeup();

        port_int_sleep_process(&wkpnd);
        // bsp_sensor_step_lowpwr_pro();
#if UTE_MODULE_HEART_SUPPORT
        if(vc30fx_sleep_isr)
        {
            // GPIOBSET = BIT(0);
            // sleep_set_sysclk(SYS_176M);
            // printf("sleep hr enter\n");
            // vc30fx_usr_device_handler(0, 1);
            uteDrvHeartVC30FXHeartOrBloodOxygenAlgoInputData();
            // GPIOBCLR = BIT(0);
            // printf("sleep hr exit\n");
            vc30fx_sleep_isr = false;
            sleep_set_sysclk(SYS_24M);
        }
#endif
#if ECIG_POWER_CONTROL
        //吸烟唤醒
        // printf("1MIC wakeup: %x %x\n", wkpnd,BIT(ECIG_MIC_INT));
#if (CHIP_PACKAGE_SELECT == CHIP_5690F)
        if (wkpnd & BIT(ECIG_MIC_INT))
        {
#elif (CHIP_PACKAGE_SELECT == CHIP_5690G)
        if (wkpnd & BIT(PORT_INT6_VECTOR))
        {
#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
        if (wkpnd & BIT(PORT_INT4_VECTOR))
        {
#elif (CHIP_PACKAGE_SELECT == CHIP_5691G)
        if (wkpnd & BIT(PORT_INT6_VECTOR))
        {
#else
        if (wkpnd & BIT(PORT_INT4_VECTOR))
        {
#endif
            printf("MIC wakeup: %x\n", wkpnd);
            //gui_need_wkp = true;
            sys_cb.gui_need_wakeup = true;
            break;
        }

        if (wkpnd & BIT(ECIG_DET1_INT) || wkpnd & BIT(ECIG_DET2_INT))
        {

            printf("DET wakeup: %x\n", wkpnd);
            // gui_need_wkp = true;
            sys_cb.gui_need_wakeup = true;
            sys_cb.gui_screen_wakeup = true;
            // func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
            break;
        }
#endif
        if(wkpnd & BIT(PORT_INT1_VECTOR))
        {
            sys_cb.gui_need_wakeup = true;
            printf("box wakeup: %x\n", wkpnd);
            break;
        }

        if ((RTCCON9 & BIT(2)) || (RTCCON10 & BIT(2)) || wko_wkup_flag)
        {
            printf("wko wakeup\n");
            gui_need_wkp = true;
            //   func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
            break;
        }
#if LE_EN
        if (ble_app_need_wakeup())
        {
            printf("ble_app_need_wakeup\n");
            gui_need_wkp = true;
            //   func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
            break;
        }
#endif

        if (sys_cb.remind_tag)
        {
            printf("remind wakeup\n");
            gui_need_wkp = true;
            break;
        }

        if (sys_cb.msg_tag)
        {
            printf("msg wakeup\n");
            gui_need_wkp = true;
            break;
        }

        if(sys_cb.hand_screen_on)
        {
            sys_cb.hand_screen_on = false;
            printf("ute hand wakeup\n");
            gui_need_wkp = true;
            break;
        }

        if (co_timer_pro(true))
        {
            printf("co_timer_pro_wakeup\n");
            break;
        }

        if (bt_cb.call_type)
        {
            printf("call_wakeup\n");
            break;
        }

        if (charge_dc_change_detect())
        {
            printf("dc change\n");
            gui_need_wkp = true;
            break;
        }

        if (sys_cb.timer_done)
        {
            printf("watch timer done\n");
            gui_need_wkp = true;
            break;
        }

        //ute add
        if(uteModulePlatformNotAllowSleep())
        {
            if (uteModulePlatformNotAllowSleep() & UTE_MODULE_PLATFORM_DLPS_BIT_SCREEN)
            {
                gui_need_wkp = true;
                printf("%s,ute_wakeup_screen\n",__func__);
            }
            printf("ute_wakeup:%04x\n",uteModulePlatformNotAllowSleep());
            break;
        }

        if(sys_cb.gui_need_wakeup)
        {
            sys_cb.gui_need_wakeup = 0;
            gui_need_wkp = true;
            func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
            printf("gui_need_wakeup\n");
            break;
        }
    }
    //  u32 tick = tick_get();
    //  printf("tick %d \n",tick);
    RTCCON9 = BIT(7) | BIT(5) | BIT(2);         //clr port, bt, wko wakeup pending
    RTCCON3 &= ~(BIT(17) | BIT(13));            //disable port, bt wakeup
    sleep_wakeup_exit();
    WKUPCON |= wkie;                            //还原WKIE

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOEDE = pe_de;
    GPIOFDE = pf_de;
    GPIOGDE = pg_de;
    GPIOHDE = ph_de;
    USBCON0 = usbcon0;
    USBCON1 = usbcon1;

    bt_update_bt_scan_param_default();
#if BT_SINGLE_SLEEP_LPW_EN
    if(!bt_is_connected() && bt_get_scan())     //单模
    {
        bt_scan_enable();
    }
#endif
    rtc_sleep_exit();
    CLKCON0 = (CLKCON0 & ~(0x03 << 2)) | (0x01 << 2); //sysclk select xosc26m_clk
    RSTCON0 &= ~BIT(4);                         //pllsdm disable
    adpll_init(DAC_OUT_SPR);                    //enable adpll
    adda_clk_source_sel(0);                     //adda_clk48_a select pll0
    sys_clk_set(sysclk);
    saradc_set_channel(adc_ch);
    bsp_saradc_init();
#if CHARGE_EN
    bsp_charge_set_stop_time(18000);
    bsp_charge_sta(sys_cb.charge_sta);          //update充灯状态
    charge_set_detect_cnt(5);
#if CHARGE_VOL_DYNAMIC_DET
    if(sys_cb.chg_on)
    {
        gradient_process(0);
        RTCCON8 &= ~BIT(1);                     //charge open
    }
#endif
#endif // CHARGE_EN
    //sys_set_tmr_enable(1, 1);
    if (DAC_FAST_SETUP_EN)
    {
        bsp_loudspeaker_mute();
    }
#if MODEM_CAT1_EN
    bsp_modem_sleep_exit();
#endif
    // printf("44tick %d \n",tick_get()-tick);
    if (gui_need_wkp)
    {
        printf("11gui_wakeup\n");
        gui_wakeup();
        func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
    }
    else
    {
        //sys_clk_set(SYS_24M);
    }

    // printf("33tick %d \n",tick_get()-tick);
    sys_set_tmr_enable(1, 1);

#if LE_EN
    ble_set_adv_interval(adv_interval);
    if (interval | latency | tout)
    {
        ble_update_conn_param(interval, latency, tout); //还原连接参数
    }
#endif
    dac_restart();
    bsp_change_volume(sys_cb.vol);
    //printf("22tick %d \n",tick_get()-tick);
#if (ASR_SELECT && ASR_FULL_SCENE)
    bsp_asr_start();
#endif
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
#if ECIG_POWER_CONTROL
    bsp_ecig_sleep_wakeup();
#endif
#if CHARGE_EX_IC_SELECT
    bsp_charge_ex_init();
#endif
    bt_exit_sleep();
    sleep_cb.sys_is_sleep = false;
    // printf("11tick %d \n",tick_get()-tick);
    // func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
    printf("sleep_exit\n");
}

AT(.text.lowpwr.sleep)
bool sleep_process(is_sleep_func is_sleep)
{
    if (sys_cb.gui_need_wakeup && sys_cb.gui_sleep_sta)
    {
        gui_wakeup();                   //按键亮屏
        reset_sleep_delay_all();
        sys_cb.gui_need_wakeup = 0;
        return false;
    }

    if(sys_cb.hand_screen_on && sys_cb.gui_sleep_sta)
    {
        gui_wakeup();                   //按键亮屏
        reset_sleep_delay_all();
        sys_cb.hand_screen_on = 0;
        return false;
    }

#if LE_EN
    if (ble_app_need_wakeup())
    {
        reset_sleep_delay_all();
        reset_pwroff_delay();
        return false;
    }
#endif
    if ((*is_sleep)())
    {
        if (!sys_cb.sleep_en)
        {
            reset_sleep_delay_all();
            return false;
        }

        if(uteModulePlatformNotAllowSleep()) //ute add
        {
            if ((uteModulePlatformNotAllowSleep() & UTE_MODULE_PLATFORM_DLPS_BIT_SCREEN))
            {
                if(sys_cb.gui_sleep_sta)
                {
                    gui_wakeup();
                    reset_sleep_delay_all();
                    sys_cb.gui_need_wakeup = 0;
                    printf("%s,ute_wakeup_screen\n",__func__);
                    return false;
                }
            }
            else if (sys_cb.guioff_delay == 0 && !sys_cb.gui_sleep_sta)
            {
                gui_sleep();                //仅熄屏
            }
            reset_sleep_delay();
            reset_pwroff_delay();
            return false;
        }
        if (sys_cb.sleep_delay == 0)
        {
            if(sys_cb.guioff_delay == 0) /*! 亮屏时不休眠,wang.luo 2024-10-21 */
            {
                sfunc_sleep();              //熄屏且进入休眠
                reset_sleep_delay_all();
                reset_pwroff_delay();
                return true;
            }
        }
    }
    else
    {
        if (sys_cb.guioff_delay == 0 && !sys_cb.gui_sleep_sta)
        {
            gui_sleep();                //仅熄屏
        }
        reset_sleep_delay();
        reset_pwroff_delay();
    }
    return false;
}

////红外关机进入sleep mode
//AT(.text.lowpwr.sleep)
//void func_sleepmode(void)
//{
//
//}

///解决充满关机等待5V掉电，超时1分钟
AT(.text.pwroff.vusb)
u8 sfunc_pwrdown_w4_vusb_offline(void)
{
//    u32 timeout = 10000, off_cnt = 0;
//    while (timeout--) {
//        WDT_CLR();
//        delay_us(330);                          //5ms
//        if (!CHARGE_DC_IN()) {
//            off_cnt++;
//        } else {
//            off_cnt = 0;
//        }
//        if (off_cnt > 5) {
//            return 1;                           //VUSB已掉线，打开VUSB唤醒
//        }
//    }
    return 0;                                   //关VUSB唤醒
}

AT(.text.pwroff.enter)
void sfunc_power_save_enter(void)
{
#if LP_XOSC_CLOCK_EN
    bool lp_xosc_clock_err = lp_xosc_check();
#endif

    sys_clk_set(SYS_24M);
    LOUDSPEAKER_MUTE_DIS();
    WDT_CLR();
#if CHARGE_EN
    if (xcfg_cb.charge_en)
    {
        bsp_charge_off();
    }
#endif
    set_buck_mode(0);                           //关机前切回ldo mode
    vusb_delay_rst_dis();
    RTCCON4 = (RTCCON4 & ~(3 << 5)) | (1 << 5); //RI_VCORE_SVBT = 0, RI_VCORE_SVIO = 1
    USBCON0 = BIT(5);
    USBCON1 = 0;
    GPIOADE = 0;
    GPIOBDE = 0;
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOGDE = 0x3F;                             //MCP FLASH
    GPIOHDE = 0;
    RTCCON8 &= ~BIT(15);                        //RI_EN_VUSBDIV = 0

#if !LP_XOSC_CLOCK_EN
    RTCCON15 &= ~(3 << 13);                     //disable LP_XOSC
    if (sys_cb.flag_shipping_mode)
    {
        RTCCON0 &= ~(BIT(0) | BIT(18) | BIT(2));    //RI_EN_SNIFF = 0, RI_EN_RINGOSC = 0, 关掉RTC_RC
    }
#else
    RTCCON0 &= ~(BIT(0) | BIT(18) | BIT(2));    //RI_EN_SNIFF = 0, RI_EN_RINGOSC = 0, 关掉RTC_RC
    if (lp_xosc_clock_err)
    {
        RTCCON15 &= ~(3 << 13);                     //disable LP_XOSC
    }
#endif

    PICCONCLR = BIT(0);                         //Globle IE disable
    CLKCON0 |= BIT(0);                          //enable RC
    CLKCON0 = (CLKCON0 & ~(0x3 << 2));          //system clk select RC

    RSTCON0 &= ~(BIT(4) | BIT(6));              //pllsdm, adda_clk
    CLKGAT0 &= ~(BIT(12) | BIT(14));            //disable dac clk
    PLL0CON &= ~(BIT(6) | BIT(18));             //PLL0 disable
    XOSCCON &= ~((0xf<<24) | (3 << 6) | (7 << 10) | BIT(5));//X24 output, xosc, xosc24m disable
    PLL0CON &= ~(BIT(12) | BIT(0) | BIT(1));    //PLL0 & XOSC关电
    RTCCON9 = 0xfff;                            //Clr pending
}

//硬开关方案，低电时，进入省电状态
AT(.text.pwroff)
void sfunc_lowbat_do(void)
{
    sfunc_power_save_enter();
    WDT_DIS();
    RTCCON3 |= BIT(11);                         //VUSB Wakeup enable
    while(1)
    {
        LPMCON |= BIT(0);                       //Sleep mode enable
        asm("nop");
        asm("nop");
        asm("nop");
        if (RTCCON10 & BIT(3))                  //VUSB wake up pending
        {
            WDT_RST();
        }
    }
}

//软开关方案，POWER键/低电时，进入关机状态
AT(.text.pwroff.pwrdwn)
void sfunc_pwrdown_do(u8 vusb_wakeup_en)
{
#if !LP_XOSC_CLOCK_EN
    if (!sys_cb.flag_shipping_mode)
    {
        rtc_set_alarm_wakeup(900);              //15min
    }
#endif

    sys_set_tmr_enable(0, 0);
    sfunc_power_save_enter();
    if (!vusb_wakeup_en)
    {
        RTCCON8 = (RTCCON8 & ~BIT(6)) | BIT(1); //disable charger function
        vusb_wakeup_en = sfunc_pwrdown_w4_vusb_offline();
    }
    RTCCON1 &= ~(BIT(5) | BIT(7));              //BIT(7): VRTC voltage for ADC, BIT(5):WK pin analog enable, output WKO voltage for ADC
    RTCCON11 = (RTCCON11 & ~0x03) | BIT(2);     //WK PIN filter select 8ms

    uint rtccon3 = RTCCON3 & ~BIT(11);
    uint rtccon1 = RTCCON1 & ~0x1f;
#if CHARGE_EN
    if ((xcfg_cb.charge_en) && (vusb_wakeup_en))
    {
        rtccon3 |= BIT(11);                     //VUSB wakeup enable
    }
#endif
    RTCCON3 = rtccon3 & ~BIT(10);               //关WK PIN，再打开，以清除Pending
    rtccon3 = RTCCON3 & ~0x87;                  //Disable VDDCORE VDDIO VDDBUCK VDDIO_AON
    rtccon3 &= ~BIT(22);                        //RI_EN_VLCORE = 0,
    rtccon3 &= ~(3 << 20);                      //RL2VDD12 = 0, C2VDD12 = 0
    rtccon3 |= BIT(6) | BIT(4) | BIT(19);       //PDCORE, PDCORE2, PDCORE3
    rtccon3 |= BIT(10);                         //WK pin wake up enable
//    rtccon3 |= BIT(16);                         //RTC_WDT wake ep enable
    rtccon1 |= BIT(0) | BIT(4);                 //WK 90K pull up enable
    WDT_DIS();
    RTCCON12 |= (0x3<<6);                       //DIS RTC_WDT
    RTCCON &= ~(3 << 1);                        //避免关机时rtc配置来不及生效问题
    RTCCON |= BIT(5);                           //PowerDown Reset，如果有Pending，则马上Reset
    RTCCON1 = rtccon1;
    RTCCON3 = rtccon3;
    LPMCON |= BIT(0);                           //sleep mode
    LPMCON |= BIT(1);                           //idle mode
    asm("nop");
    asm("nop");
    asm("nop");
    while (1);
}

void sfunc_pwrdown(u8 vusb_wakeup_en)
{
    lock_code_pwrsave();
    sfunc_pwrdown_do(vusb_wakeup_en);
}

void sfunc_lowbat(void)
{
    lock_code_pwrsave();
    sfunc_lowbat_do();
}

AT(.text.lowpwr.pwroff)
void func_pwroff(int pwroff_tone_en)
{
    printf("%s\n", __func__);

#if (ASR_SELECT && ASR_FULL_SCENE)
    bsp_asr_stop();
#endif

#if !LP_XOSC_CLOCK_EN
    if (!sys_cb.flag_shipping_mode)
    {

        sniff_rc_init();
        rtc_rc_sleep_enter();
        rtc_calibration_write(PARAM_RTC_CAL_ADDR);
        cm_write8(PARAM_RTC_CAL_VALID, 1);
        cm_sync();
        rtc_printf();
    }
#endif

#if WARNING_POWER_OFF
    if (SOFT_POWER_ON_OFF)
    {
        mp3_res_play(RES_BUF_POWEROFF_MP3, RES_LEN_POWEROFF_MP3);
    }
#endif // WARNING_POWER_OFF

//    gui_off();

    if (SOFT_POWER_ON_OFF)
    {
        if (!PWRKEY_2_HW_PWRON)
        {
            while (IS_PWRKEY_PRESS())       //等待PWRKWY松开
            {
                delay_5ms(1);
                WDT_CLR();
            }
        }
        dac_power_off();                    //dac power down
        if (CHARGE_DC_IN())
        {
            if (power_off_check())          //充电过程中等待结束再关机
            {
                return;
            }
        }
        bsp_saradc_exit();                  //close saradc及相关通路模拟
        if ((PWRKEY_2_HW_PWRON) && (sys_cb.poweron_flag))
        {
            RTCCON1 |= BIT(6);              //WK PIN High level wakeup
        }
        sfunc_pwrdown(1);
    }
    else
    {
        dac_power_off();                    //dac power down
        bsp_saradc_exit();                  //close saradc及相关通路模拟

        sfunc_lowbat();                     //低电关机进入Sleep Mode
    }
}

//主动写入 软件 复位原因
void sw_reset_kick(uint8_t source)
{
    LVDCON &= ~(0x1f << 8);
    LVDCON |= BIT(12);
    LVDCON |= (source << 8);
}

/****************************************************************************************************************
* Deepsleep mode模式
* VDDCORE关闭、VDDIO用ano电源域、lp_top用vrtc11供电、关掉VDDBT、模拟模块置成复位状态
* 支持所有引脚唤醒，WKO，VUSB插入，INBOX或者RTC定时唤醒，MCLR引脚复位等
* 唤醒后VDDCORE重新上电复位，重新load flash程序跑起来
*               开rtc计时(时钟选择sniff rc)     开rtc计时(时钟选择ring rc)   不开rtc计时
* 基础电流           12~13uA                    8~9uA                       5~6uA
*使用方式：调用 func_deepsleep()函数即可进入该模式, 唤醒方式可根据具体情况修改唤醒源
*注: 如果使用带屏幕的开发板测试, 由于屏幕用到了PA1, 如果配置了PA1唤醒, 会导致关机关不下去
****************************************************************************************************************/

typedef struct deepsleep_cfg_t_
{
    bool touch_wkup_en;
    bool hall_wkup_en;
    bool rtc_wkup_en;
} deepsleep_cfg_t;
deepsleep_cfg_t deepsleep;

#if BOX_RTC_EN
//关机RTC走时的时钟选择, 二选一
#define SNF_RC_EN       1       //关机走时使用sniff rc, 走时比较准, 开了这个功耗高6~7μa
#define RING_RC_EN      0       //关机走时使用ring rc, 走不准, 功耗高3~4μa
#endif

//关机时VDDIO LDO选择, 二选一
#define VDDIO_EN        0       //关机时使用标准VDDIO, 3.3V, 功耗在45~46μa(开了sniff rc), 需要vddio供电时使用
#define VDDIO_AON_EN    1       //关机时使用VDDIO_AON, 档位2.8V, 功耗在12~13μa(开了sniff rc)

#define PWRKEY_WAKEUP   0       //pwrkey唤醒 WKO (io唤醒)
// #define TOUCH_WAKEUP    1       //触摸唤醒
// #define HALL_WAKEUP     HALL_EN //HALL唤醒
#define VUSB_WAKEUP     1       //VUSB唤醒 (io唤醒 PB1)
#define WKO_WAKEUP      1       //wko pin唤醒

void deepsleep_touch_wkup_set(bool en)
{
    deepsleep.touch_wkup_en = en;
}

void deepsleep_hall_wkup_set(bool en)
{
    deepsleep.hall_wkup_en = en;
}

void deepsleep_rtc_wkup_set(bool en)
{
    deepsleep.rtc_wkup_en = en;
}

void deepsleep_var_init(void)
{
    deepsleep_touch_wkup_set(true);
    deepsleep_hall_wkup_set(true);
    deepsleep_rtc_wkup_set(true);
}

AT(.com_text.lp.deepsleep)
void deepsleep_save_to_cache(u8 *src_addr, u32 length)
{
    //需要把RAM0的程序copy到ICACHE RAM上面跑
    u32 i;
    u8 *cp_dst_ptr, *cp_src_ptr;
    cp_src_ptr = (u8*) src_addr;
    cp_dst_ptr = (u8*) 0x70000;
    for(i = 0; i < length; i++)
    {
        *cp_dst_ptr++ = *cp_src_ptr++;
    }

    asm("j 0x70000");
    while(1);
}

AT(.com_text.lp.deepsleep)
void port_wkup_init(void)
{
    PORTINTEDG = 0;
    PORTINTEN = 0;


#if TOUCH_WAKEUP
    if (deepsleep.touch_wkup_en)
    {
        port_wakeup_all_init(PORT_CTP_INT, 1, 1);
    }
#endif // TOUCH_WAKEUP

#if HALL_WAKEUP
    if (deepsleep.hall_wkup_en)
    {
        bsp_gpio_de_en(HALL_GPIO);
        if (bsp_gpio_get_sta(HALL_GPIO))
        {
#if HALL_VDD_SEL
            port_wakeup_all_init(HALL_GPIO, 1, 1);
#else
            port_wakeup_all_init(HALL_GPIO, 1, 0);
#endif
        }
        else
        {
            port_wakeup_all_init(HALL_GPIO, 0, 0);
        }
    }
#endif

//#if VUSB_WAKEUP
//#if (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
//    port_wakeup_all_init(CHARGE_SDA_GPIO, 0, 0);
//#else
//    port_wakeup_all_init(CHARGE_SDA_GPIO, 1, 1);
//#endif
//#endif

#if PWRKEY_WAKEUP
    //pwrkey 唤醒
    RTCCON11 &= ~BIT(4);
    RTCCON1 |= BIT(0) | BIT(2) | BIT(4);    //OPEN WKO_IE  pull_up
    WKPINMAP |= (0x3 << 0);
    WKUPEDG |= BIT(0);                      //fall edge
    WKUPCON |= BIT(0);                      //0 enable
    WKUPCPND |= BIT(16);                    //clear 0 pending
#endif

#if (PWRKEY_WAKEUP || TOUCH_WAKEUP || HALL_WAKEUP || VUSB_WAKEUP)
    WKUPCON |= 1<<20;                       //fall filter select
#endif

#if HALL_WAKEUP
    if (deepsleep.hall_wkup_en)
    {
        WKUPCON |= 1<<22;                   //rise filter select
    }
#endif
    WKUPCON |= BIT(17);                     //wakup sniff enable

#if WKO_WAKEUP
    RTCCON1 |= BIT(0);                      //wk0 pin ie enable
    RTCCON1 &= ~BIT(6);                     //wk0 pin low level wakup
    RTCCON1 |= BIT(4);                      //wk0 pin pullup_en en
    RTCCON1 &= ~BIT(1);                     //wk0 pin pulldown dis
    RTCCON11 |= BIT(4);                     //wko_protect en
    RTCCON11 = (RTCCON11 & ~(0x3 << 0)) | (0x0 << 0);   //filter time select
    RTCCON11 |= BIT(2);                     //wko filt en
#endif
}

AT(.com_text.lp.deepsleep)
void deepsleep_port_state(void)
{
    //关机下不需要用到的IO，都配置为模拟IO，避免漏电
    GPIOADE  = 0x0;
    GPIOBDE  = 0x0;
    GPIOGDE  = 0x0;
    GPIOEDE  = 0x0;
    GPIOFDE  = 0x0;

    GPIOAFEN = 0x0;
    GPIOBFEN = 0x0;
    GPIOGFEN = 0x0;
    GPIOEFEN = 0x0;
    GPIOFFEN = 0x0;

    GPIOAPU  = 0x0;
    GPIOBPU  = 0x0;
    GPIOGPU  = 0x0;
    GPIOEPU  = 0x0;
    GPIOFPU  = 0x0;

    GPIOAPD  = 0x0;
    GPIOBPD  = 0x0;
    GPIOGPD  = 0x0;
    GPIOEPD  = 0x0;
    GPIOFPD  = 0x0;

    port_wkup_init();

    PWRCON1 &= ~(0xF<<14);    //disable flash PG
    GPIOGDE = BIT(2);
    GPIOGPU = BIT(2);         //pull up cs

    WKUPCPND = 0xff<<16;     //clear pendind
    CLKGAT0 &= ~BIT(15);     //disable lp_top clock
    RTCCON5 &= ~BIT(12);     //IO interface disable, io控制寄存器无法写
}

AT(.com_text.lp.deepsleep)
void sfunc_deepsleep_do(void)
{
    CLKCON0 |= BIT(0);                              //rosc_en
    CLKCON0 = (CLKCON0 & ~(0x3<<2)) | (0x0<<2);     //sys_clk sel rc2m
    CLKGAT1 &= ~BIT(29);                            //disable x24m_clken
    RTCCON15 &= ~(BIT(13) | BIT(14) | BIT(19));     //disable lpwr_rtc
    XOSCCON = 0;
    PLL0CON = 0;

    CLKGAT0 &= ~(3<<2);     //disable ram0/1 clock
    RTCCON5 &= ~BIT(11);    //lpif_en interface disable
    RTCCON5 |= BIT(18);     //power_flag

    RTCCON0 = (RTCCON0 & ~(0x7<<22)) | (0x1<<22);
    RTCCON4 = (RTCCON4 & ~(0x3f<<24)) | (4<<24) | (4<<27);

    RTCCON3 = (RTCCON3 & ~(0x1<<24)) | (0x0<<24);   //wakeup disable
    RTCCON3 = (RTCCON3 & ~(0x3ff<<8)) | (0x0<<8);
    RTCCON9 = 0xffff;       //wakeup pending clear
    RTCCON |= BIT(5);       //PowerDown Reset，如果有Pending，则马上Reset

    RTCCON3  = //BIT(19)      //pd_core3
        BIT(17)         //wk1_en
//            | BIT(11)       //VUSB wkup enable
#if WKO_WAKEUP
        | BIT(10)       //wakeup pin wakeup enable
#endif
//            |  BIT(9)       //rtc1s_wken
#if BOX_RTC_EN
        | BIT(8)        //RTC alarm wakeup enable
#endif
#if VDDIO_EN
        |  BIT(1)       //VDDIO LDO
#elif VDDIO_AON_EN
        |  BIT(7)       //en vddio_aon
#endif
//             | BIT(6)    //pd_core
        |  BIT(4)      //pd_core2
        |  BIT(3)      //rl2vlc   不用IO唤醒可以关掉这个，功耗可以省4uA左右
        ;
    LPMCON |= BIT(0);                           //sleep mode
    LPMCON |= BIT(1);                           //idle mode
    while(1);
}

//AT(.com_text.str)
//const char str2[] = "RTCCON0: %x, RTCCON3: %x, RTCCON4: %x, RTCCON5: %x, RTCCON12: %x\n";
//
//AT(.com_text.str)
//const char str3[] = "GPIOAPU: %x, GPIOBPU: %x, GPIOGPU: %x\n";

AT(.com_text.lp.deepsleep)
void sfunc_deepsleep(void)
{
#if BOX_RTC_EN
    if (deepsleep.rtc_wkup_en)
    {
        rtc_set_alarm_wakeup(900);    //15min
    }
#endif
    PICCONCLR = BIT(0);     //关总中断
    WDT_DIS();

#if RING_RC_EN
    RTCCON0 &= ~(0x3f<<8);
    RTCCON0 |= BIT(0);                          //enable PMU RC
    RTCCON0 &= ~BIT(18);                        //disable SNIFF RC
    RTCCON0 = (RTCCON0 & ~(3<<8)) | (1<<8);     //CLK32K_RTC Select RC2M_RTC
#elif SNF_RC_EN
    RTCCON0 &= ~(0x3f<<8);                      //2m bt,tk,rtc disable
    RTCCON0 |= BIT(18);                         //enable SNIFF RC
    RTCCON0 = (RTCCON0 & ~(0x3<<16)) | (0x1<<16);   //SNF_BIAS      //同步zxl分支s1113
    RTCCON0 = (RTCCON0 & ~(3<<8)) | (2<<8);     //CLK32K_RTC Select SNF_RC_RTC
#else
    RTCCON0 &= ~(0x3f<<8);
    RTCCON0 &= ~BIT(18);                            //disable SNIFF RC
    RTCCON0 &= ~BIT(0);                             //disable PMU RC
    RTCCON0 = (RTCCON0 & ~(0x3<<16)) | (0x0<<16);   //SNF_BIAS
    RTCCON0 &= ~BIT(25);                            //CAP2PLL
#endif

//    printf(str2, RTCCON0, RTCCON3, RTCCON4, RTCCON5, RTCCON12);
//    printf(str3, GPIOAPU, GPIOBPU, GPIOGPU);

    //配置关机下，VRTC 数字的工作电压；RTCCON BIT0为0则选择RTCCON4 BIT26:24; 为1则选择BIT 29:27
    RTCCON4 = (RTCCON4 & ~(7<<24)) | (4<<24);

#if VDDIO_AON_EN
    //VDDIO LDO从正常LDO切换到VDDIO_AON
    RTCCON3 |= BIT(7);    //VDDIO_AON enable
    RTCCON3 &= ~BIT(1);
//    RTCCON4 |= BIT(23);    //VDDIO_AON SEL 3.3V
    RTCCON4 = (RTCCON4 & ~(7<<24)) | (1<<24);
#elif VDDIO_EN
    PWRCON0 = (PWRCON0 & ~(0xf<<5)) | (0x9<<5);   //VDDIO=3.3v
    RTCCON4 = (RTCCON4 & ~(3<<17)) | (0<<17);     //VIO_TC
#endif

    RTCCON11 |= BIT(4);  //wko protect
    RTCCON4 &= ~BIT(22); //rtcbg_uvm disable
    RTCCON4 &= ~BIT(16); //VIOPD
//    RTCCON8 = (RTCCON8 & ~(0X1F<<24)) | (0XF<<24);  //BGTRIM  //基准电压被调偏, 会导致关机后VDDIO被拉低0.1V左右
    RTCCON1 = 0;

    RTCCON12 = 0xfa;  //disable wkp10s, gpio, wdt reset

////rtc1s wakeup
//    RTCCON0 |= BIT(0);
//    RTCCON0 &= ~BIT(6);
//    RTCCON0 = (RTCCON0 & ~(0x3 << 8)) | (0x1<<8);
//    RTCCON2 = 0x7ffff;
//    RTCCNT = 0;

    deepsleep_port_state();

    //需要把RAM0的程序copy到ICACHE RAM上面跑
    deepsleep_save_to_cache((u8 *)sfunc_deepsleep_do, 500);
}

AT(.text.lowpwr.deepsleep)
void func_deepsleep(void)
{
    printf("%s RTCCON4[%x]\n", __func__, RTCCON4);

//    bsp_charge_ex_mode_set(BOX_COMM_MODE);
//    bsp_smart_vhouse_cmd_sned(VHOUSE_CMD_PWROFF, LEFT_CHANNEL_USER, 0, 0);
//    bsp_smart_vhouse_cmd_sned(VHOUSE_CMD_PWROFF, RIGHT_CHANNEL_USER, 0, 0);

    if (bt_cb.bt_is_inited)
    {
        if (ble_is_connect())
        {
            ble_disconnect();
        }
        else
        {
            bt_disconnect(0);
        }
        bt_off();
        bt_cb.bt_is_inited = 0;
    }

//    bsp_charge_ex_hal_mode_set(BOX_CHK_MODE);   //入仓检测模式
    delay_5ms(160);

//#if TOUCH_WAKEUP
//    if (deepsleep.touch_wkup_en) {
//        ctp_int_sleep();
//    } else {
//        ctp_sleep();
//    }
//#else
//    ctp_sleep();
//#endif

    gui_sleep();

    rtc_printf();

    rtc_sleep_enter();
//#if BOX_RTC_EN
//    if (deepsleep.rtc_wkup_en) {
//        rtc_calibration_write(PARAM_RTC_CAL_ADDR);
//        cm_write8(PARAM_RTC_CAL_VALID, 1);
//    }
//#endif
//    cm_write(&earphone.tws_addr[0], PARAM_EARPHONE_TWS_ADDR, 6);
//    cm_write(&earphone.bt_addr[EARPHONE_LEFT][0], PARAM_EARPHONE_L_ADDR, 6);
//    cm_write(&earphone.bt_addr[EARPHONE_RIGHT][0], PARAM_EARPHONE_R_ADDR, 6);
//    if (sys_cb.deepsleep_is_pwroff) {
//        cm_write8(PARAM_DEEP_SLEEP_FLAG, 0);
//    } else {
//        cm_write8(PARAM_DEEP_SLEEP_FLAG, 1);
//    }
//    cm_write8(PARAM_CV_PERCENT_TICKS_CNT, charge_ex_cb.percent_cv_tick_cnt + ((tick_get() - charge_ex_cb.percent_cv_tick)/1000));
//    cm_write8(PARAM_EARPHONE_VBAT_L, earphone.vbat[EARPHONE_LEFT]);
//    cm_write8(PARAM_EARPHONE_VBAT_R, earphone.vbat[EARPHONE_RIGHT]);
//#if HALL_EN
//    cm_write8(PARAM_EARPHONE_INBOX_L, earphone.inbox[EARPHONE_LEFT]);
//    cm_write8(PARAM_EARPHONE_INBOX_R, earphone.inbox[EARPHONE_RIGHT]);
//    cm_write8(PARAM_BOX_HALL_STA, bsp_hall_open_box());
//#endif
//    param_box_data_write();
//    cm_write(&earphone.paired, PARAM_EARPHONE_PAIRED, 1);
//    cm_sync();

    rtc_printf();

    dac_power_off();                    //dac power down

    sys_set_tmr_enable(0, 0);           //关闭定时器
    bsp_hw_timer_del(HW_TIMER3);

    if (CHARGE_DC_IN())
    {
        if (power_off_check())          //充电过程中等待结束再关机
        {
            return;
        }
    }
    bsp_saradc_exit();                  //close saradc及相关通路模拟

    sys_clk_set(SYS_24M);
    sfunc_deepsleep();
}
