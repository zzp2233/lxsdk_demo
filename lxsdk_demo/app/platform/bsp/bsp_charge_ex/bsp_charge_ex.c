#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

charge_ex_cb_t charge_ex_cb;

#if CHARGE_EX_IC_SELECT

void bsp_charge_ex_percent_cv_process(void);
bool bsp_charge_ex_percent_init(void);

void bsp_charge_ex_usb_insert_callback(bool insert)
{
//    printf("%s\n", __func__);
    msg_enqueue(EVT_BOX_BAT_UPDATE);            //更新电量图标
    if (insert)                                 //插入USB
    {
#if FUNC_USBDEV_EN
        if (dev_is_online(DEV_USBPC))
        {
            return;
        }
#endif
        bsp_charge_ex_charge(1);
//        func_popup(FUNC_CHARGE);

#if (BSP_CHARGE_EX_BOX_DETECT == BOX_SOTP_CHARGE)
        bsp_charge_ex_stop_charge_start();
#endif
    }
}

void bsp_charge_ex_vbat_update_callback(u16 v_batm)
{
//    printf("%s\n", __func__);
#if !VBAT_DETECT_EN
#if (BSP_CHARGE_EX_BOX_DETECT == BOX_SOTP_CHARGE)
    if (bsp_charge_ex_is_charge())
    {
        bsp_charge_ex_stop_charge_detect_vbat(v_batm);
    }
    else
    {
        bsp_charge_ex_vbat_update(v_batm);
    }
#else
    bsp_charge_ex_vbat_update(v_batm);
#endif
#endif
}

void bsp_charge_ex_inbox_callback(bool l_inbox, bool r_inbox)
{
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C)
    if (bsp_hall_open_box())
    {
        bsp_earphone_inbox(EARPHONE_LEFT, l_inbox);
        bsp_earphone_inbox(EARPHONE_RIGHT, r_inbox);
        if(r_inbox)
        {
            // printf(" msg_enqueue(MSG_SYS_BOX_CHARGE) >>>>> DisableDisableDisable\n");
            msg_enqueue(MSG_SYS_BOX_CHARGE);
        }
    }
#endif
//    printf("inbox: l[%d] r[%d]\n", l_inbox, r_inbox);
}

void bsp_charge_ex_eoc_callback(bool l_eoc, bool r_eoc)
{
    printf("%s > 0\n",__func__);
    if (!bsp_charge_ex_eoc_check_en())
    {
        // printf("nnnnnnnnn\r\n");
        return;
    }

#if BOX_OPEN_WAKEUP_EARPHONE
    if (bsp_charge_ex_is_waking_up())     //唤醒耳机过程中不允许更新eoc
    {
        return;
    }
#endif

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C && HALL_EN)
    bool l_inbox = !l_eoc;     //未满电认为插入, 但满电不可认为拔出, 因为耳机可能在满电状态入仓误判出仓
    bool r_inbox = !r_eoc;

    if (bsp_charge_ex_eoc_check_inbox())
    {
        if (l_inbox)
        {
            bsp_earphone_inbox(EARPHONE_LEFT, l_inbox);
        }
        if (r_inbox)
        {
            bsp_earphone_inbox(EARPHONE_RIGHT, r_inbox);
        }
    }
#endif
    if (l_eoc)
    {
        // printf("ffffffffff\r\n");
        earphone.vbat[EARPHONE_LEFT] = 100;

    }

    if (r_eoc)
    {
        printf("%s > 1 \n",__func__);
        msg_enqueue(MSG_SYS_BOX_INTER);
        earphone.vbat[EARPHONE_RIGHT] = 100;

    }
//    printf("eoc: l[%d] r[%d]\n", l_eoc, r_eoc);
}

AT(.text.func.idle.charge)
void bsp_charge_ex_open_wakeup(void)
{
    TRACE("%s\n", __func__);

#if BOX_OPEN_WAKEUP_EARPHONE
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, SMART_VHOUSE_HOST_TX);

    bsp_charge_ex_mode_set(BOX_COMM_MODE);

    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);    //lp7812c通讯模式下关闭COMM脚, 即可拉到0V
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxPU]  &= ~BIT(gpio.num);
        gpio.sfr[GPIOxCLR] = BIT(gpio.num);

        charge_ex_cb.wkup_0V_tick = tick_get();
    }
#endif
}

void bsp_charge_ex_5V_wakeup(void)
{
    charge_ex_cb.wkup_0V_tick = 0;
    charge_ex_cb.wkup_5V_tick = 0;

    TRACE("%s\n",__func__);
#if BOX_CLOSE_WKUP_5V_EN
    charge_ex_cb.wkup_5V_tick = tick_get();
    bsp_charge_ex_mode_set(BOX_CHARGE_MODE);
#else
    if (bsp_hall_close_box())
    {
        bsp_charge_ex_open_wakeup();
    }
    else
    {
        charge_ex_cb.wkup_5V_tick = tick_get();
        bsp_charge_ex_mode_set(BOX_CHARGE_MODE);
    }
#endif
}

void bsp_charge_ex_open_wakeup_end(void)
{
    TRACE("%s\n", __func__);
    smart_vhouse_tx_en();                       //还原COMM脚配置
}

void bsp_charge_ex_wakeup_earphone_process(void)
{
    u32 wkup_5V_tick = 200;
    u32 wkup_0V_tick = BOX_WKUP_OV_DURATION;

#if HALL_EN
    if (CHARGE_EX_BAT_IS_LOW())
    {
        wkup_5V_tick = 2000;        //低电时延长拉5V时间, 避免关盖恰好发关机码时开盖 耳机不开机;
    }
#else
    if (CHARGE_EX_BAT_IS_LOW())
    {
        wkup_5V_tick = 3500;
    }
    else
    {
        wkup_0V_tick = BOX_WKUP_OV_DURATION + 2000;
    }
#endif // HALL_EN

    if (charge_ex_cb.wkup_5V_tick && tick_check_expire(charge_ex_cb.wkup_5V_tick, wkup_5V_tick))    //拉5V唤醒结束
    {
        charge_ex_cb.wkup_5V_tick = 0;
        bsp_charge_ex_open_wakeup();
    }

    if (charge_ex_cb.wkup_0V_tick && tick_check_expire(charge_ex_cb.wkup_0V_tick, wkup_0V_tick))    //拉0V唤醒结束
    {
        charge_ex_cb.wkup_0V_tick = 0;
        bsp_charge_ex_open_wakeup_end();
#if HALL_EN
        if (bsp_hall_open_box())
        {
            bsp_hall_open_action();
        }
        else
        {
            bsp_hall_close_action();
        }
#else
        bsp_no_hall_wakeup_action();
#endif
    }
}

bool bsp_charge_ex_comm_backup(void)
{
    u8 mode = bsp_charge_ex_mode();

    if (mode != BOX_COMM_MODE && mode != BOX_NULL_MODE)
    {
        charge_ex_cb.mode_backup = bsp_charge_ex_mode();
        bsp_charge_ex_mode_set(BOX_COMM_MODE);

        if (mode == BOX_CHARGE_MODE)
        {
            charge_ex_cb.comm_delay = true;
        }

        TRACE("mode save[%d] tick[%d]\n", charge_ex_cb.mode_backup, tick_get());
        return true;
    }

    return false;
}

void bsp_charge_ex_comm_recover(void)
{
    if (charge_ex_cb.mode_backup != BOX_NULL_MODE)
    {
        TRACE("mode recover[%d]\n", charge_ex_cb.mode_backup);

        bsp_charge_ex_mode_set(charge_ex_cb.mode_backup);
        charge_ex_cb.mode_backup = BOX_NULL_MODE;
        charge_ex_cb.comm_delay = false;
    }
}

AT(.text.func.idle.charge)
bool bsp_charge_ex_mode_set(u8 mode)
{
    if (!charge_ex_cb.wkup_5V_tick)
    {
        if (mode == BOX_CHARGE_MODE && CHARGE_EX_BAT_IS_LOW())      //仓低电不允许给耳机充电
        {
            return false;
        }
    }

    if (charge_ex_cb.mode_backup)                                   //当前处于通讯模式备份状态, 备份新设置的模式值
    {
        if ((bsp_charge_ex_mode() == BOX_COMM_MODE) && (mode != BOX_COMM_MODE) &&
            charge_ex_cb.mode_backup != mode)
        {
            TRACE("mode new save[%d]\n", charge_ex_cb.mode_backup);
            charge_ex_cb.mode_backup = mode;
        }
    }

    return bsp_charge_ex_hal_mode_set(mode);
}

void bsp_charge_ex_vbat_update(u16 vbat)
{
#if !VBAT_DETECT_EN
    u16 *v_batm = &charge_ex_cb.v_batm;
    u32 *vbat_total = &charge_ex_cb.vbat_total;

    if (*v_batm == 0)
    {
        *v_batm = cm_read16(PARAM_BOX_VBAT);
        if (*v_batm < tbl_box_percent[0] || *v_batm > tbl_box_percent[100])   //值不合法
        {
            *v_batm = vbat;
        }

        *vbat_total = *v_batm << BOX_VBAT_AVERAGE_FILTER_K;
        TRACE("===> init vbat: pgd[%d] vbat[%d->%d]\n",
              charge_ex_cb.pgd, *v_batm, *vbat_total >> BOX_VBAT_AVERAGE_FILTER_K);
        return;
    }

    //默认的取平均值算法.
    *vbat_total = (*vbat_total) - (*v_batm) + vbat;
    *v_batm = *vbat_total >> BOX_VBAT_AVERAGE_FILTER_K;
#endif
}

static u8 bsp_charge_ex_percent_lookup(void)
{
    u8 vbat_procent = PERCENT_RUN_OUT;
    u16 vbat = bsp_charge_ex_vbat_get();
    int correct_value = bsp_charge_ex_get_correct();

#if BOX_VBAT_CORRECTED_TRACE
    printf("Vbat_Lookup: vbat[%d] Correct[%d] ", vbat, correct_value);
#endif
    for (u8 i = BOX_PERCENT_MAX; i > 0; i--)
    {
        if (vbat >= (tbl_box_percent[i - 1] + correct_value))
        {
            vbat_procent = i - 1;
            break;
        }
    }

#if BOX_VBAT_CORRECTED_TRACE
    printf("(%d-->%d%%)\n", vbat-correct_value, vbat_procent);
#endif
    return vbat_procent;
}

void bsp_charge_ex_percent_update(void)
{
    static u16 tick_pr = 0;
    static u32 percent_tick = 0;
    u8 percent = PERCENT_NOT_INIT;
    u8 *percent_cb = &charge_ex_cb.percent;

    if (bsp_charge_ex_vbat_get() == 0)
    {
        *percent_cb = percent;
        return;
    }

    if (tick_pr)      //电量变化时间滤波
    {
        if (tick_check_expire(percent_tick, tick_pr*1000))
        {
            tick_pr = 0;
        }
        else
        {
//            printf("percent change time disallow[%d]\n", tick_pr);
            return;
        }
    }

#if (BSP_CHARGE_EX_BOX_DETECT == BOX_CURVE_FIT)
    static bool cv_flag = false;

    if (cv_flag)
    {
        bsp_charge_ex_percent_cv_process();
        percent = charge_ex_cb.percent_cv;
        if (percent < BOX_CURVE_FIT_CV_PERCENT)                         //切恒流(查表)
        {
            cv_flag = false;
            charge_ex_cb.percent_cv_tick = 0;
            TRACE("CC init vbat[%d]\n", charge_ex_cb.v_batm);
        }
    }
    else
    {
        percent = bsp_charge_ex_percent_lookup();
        if (percent >= BOX_CURVE_FIT_CV_PERCENT && percent <= 100)      //切恒压(定时累加)
        {
            charge_ex_cb.percent_cv = percent;

            cv_flag = true;
            charge_ex_cb.percent_cv_tick = tick_get();
            TRACE("CV init[%d%%]\n", percent);
        }
    }

#else
    percent = bsp_charge_ex_percent_lookup();
#endif

    if (*percent_cb == PERCENT_NOT_INIT)
    {
        if (!bsp_charge_ex_percent_init())
        {
            *percent_cb = percent;
        }
        msg_enqueue(EVT_BOX_BAT_UPDATE);
        //TRACE("PERCENT init[%d] percent[%d] flag[%d]\n",
        //  *percent_cb, percent, cm_read8(PARAM_DEEP_SLEEP_FLAG));
    }
    else
    {
        if (percent == PERCENT_RUN_OUT)
        {
            *percent_cb = percent;
        }
        else
        {
            if (*percent_cb == PERCENT_RUN_OUT)
            {
                *percent_cb = percent;
            }
            if (bsp_charge_ex_is_charge())
            {
                if (percent > *percent_cb)
                {
                    *percent_cb += 1;
                    percent_tick = tick_get();
                    tick_pr = BOX_PERCENT_CHANGE_PR_CHARGE;
                    msg_enqueue(EVT_BOX_BAT_UPDATE);
#if BOX_VBAT_CORRECTED_TRACE
                    TRACE("PERCENT++[%d]\n", *percent_cb);
#endif
                }
            }
            else
            {
                if (percent < *percent_cb)
                {
                    *percent_cb -= 1;
                    percent_tick = tick_get();
                    tick_pr = BOX_PERCENT_CHANGE_PR;
                    msg_enqueue(EVT_BOX_BAT_UPDATE);
#if BOX_VBAT_CORRECTED_TRACE
                    TRACE("PERCENT--[%d]\n", *percent_cb);
#endif
                }
            }
        }
    }
}

AT(.text.func.idle.charge)
u8 bsp_charge_ex_percent_get(void)
{
    return charge_ex_cb.percent;
}

void bsp_charge_ex_bat_low_check(void)
{
#if GUI_SELECT
    static u8 duty_bkp;
    static u32 lpwr_change_tick = 0;

    if (lpwr_change_tick)  //由于改了亮度, 电压会抖动, 一定时间内不允许修改低电模式状态
    {
        if (tick_check_expire(lpwr_change_tick, 30*1000))
        {
            lpwr_change_tick = 0;
        }
        return;
    }

    if (CHARGE_EX_BAT_IS_LOW())                                     //进入低电
    {
        if (!sys_cb.bat_low)
        {
            if (tft_cb.tft_bglight_kick || tft_cb.te_bglight_cnt || tft_cb.tft_bglight_kick)    //刚开机屏幕还没亮, 避免误备份亮度
            {
                duty_bkp = tft_cb.tft_bglight_duty ? tft_cb.tft_bglight_duty : BGLIGHT_LV_DEFAULT;
                tft_cb.tft_bglight_duty = BOX_LPWR_BRIGHTNESS;
            }
            else
            {
                duty_bkp = tft_cb.tft_bglight_duty;
                tft_bglight_set_level(BOX_LPWR_BRIGHTNESS, true);   //调低亮度
            }
            sys_cb.bat_low = true;
            lpwr_change_tick = tick_get();
//            func_popup(FUNC_CHARGE);                                //弹出低电提示界面
            bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_PWROFF, LEFT_CHANNEL_USER);       //仓低电耳机关机, 发关机码
            bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_PWROFF, RIGHT_CHANNEL_USER);
        }
    }
    else                                                            //退出低电
    {
        if (sys_cb.bat_low)
        {
            tft_bglight_set_level(duty_bkp, true);                  //还原亮度
            sys_cb.bat_low = false;
            lpwr_change_tick = tick_get();

            if (bsp_hall_close_box())
            {
                hall_cb.charge_tick = tick_get();                   //恢复充电
            }
        }
    }
#endif // GUI_SELECT
}

#if (BSP_CHARGE_EX_BOX_DETECT == BOX_SOTP_CHARGE)
void bsp_charge_ex_stop_charge_start(void)
{
    charge_ex_cb.stop_charge_cnt = 0;
    charge_ex_cb.usb_insert_tick = tick_get();   //开始停充检测模式
    TRACE("stop_charge tick\n");
}

void bsp_charge_ex_stop_charge_detect_vbat(u16 vbat)
{
    bool *stop_charge_detect = &charge_ex_cb.stop_charge_detect;

    if (*stop_charge_detect)
    {
        bsp_charge_ex_vbat_update(vbat);
        *stop_charge_detect = false;

        bsp_charge_ex_charge(1);
        TRACE("stop detect vbat[%d]\n", charge_ex_cb.v_batm);
    }
}

static void bsp_charge_ex_stop_charge_process(void)
{
    u32 *usb_insert_tick = &charge_ex_cb.usb_insert_tick;
    u16 *cnt = &charge_ex_cb.stop_charge_cnt;
    bool *stop_charge_detect = &charge_ex_cb.stop_charge_detect;

    if (charge_ex_cb.pgd)
    {

        if (tick_check_expire(*usb_insert_tick, 1000))
        {
            *usb_insert_tick = tick_get();

            (*cnt)++;
            if (*cnt == STOP_CHARE_START)
            {
                bsp_charge_ex_charge(0);
                TRACE("stop_charge for detect\n");
            }
            else if (*cnt == STOP_CHARE_DETECT)
            {
                *stop_charge_detect = true;
                TRACE("stop_charge get_vbat\n");
            }
            else if (*cnt >= STOP_CHARE_PERIOD)
            {
                *cnt = 0;
                TRACE("stop_charge next_cycle\n");
            }
//            printf("stop_charge_detect %d\n", *cnt);
        }

    }
}
#elif (BSP_CHARGE_EX_BOX_DETECT == BOX_CURVE_FIT)
void bsp_charge_ex_percent_cv_process(void)
{
    bool tick_cnt_clr = false;
    u8 *percent_cv = &charge_ex_cb.percent_cv;
    u16 *percent_cv_tick_cnt = &charge_ex_cb.percent_cv_tick_cnt;
    u32 *percent_cv_tick = &charge_ex_cb.percent_cv_tick;
    u32 cv_percnet_pr = BOX_CURVE_FIT_CV_PERCENT_PR;

    if (bsp_charge_ex_is_charge())
    {
        cv_percnet_pr = BOX_CURVE_FIT_CV_PERCENT_PR_CHARGE;
    }

    if (*percent_cv_tick_cnt)
    {
        u32 percent_cv_tick = (*percent_cv_tick_cnt)*1000;
        if (percent_cv_tick < cv_percnet_pr)
        {
            cv_percnet_pr -= percent_cv_tick;   //减去上一次开机累计的恒压计时时间
        }
        else
        {
            cv_percnet_pr = 0;                  //累计的恒压计时时间大于一次周期的时间, 立即更新
        }
        tick_cnt_clr = true;
    }

    if (*percent_cv_tick && tick_check_expire(*percent_cv_tick, cv_percnet_pr))
    {
        *percent_cv_tick = tick_get();

        if (bsp_charge_ex_is_charge())
        {
            if (*percent_cv < 100)
            {
                *percent_cv += BOX_CURVE_FIT_CV_PERCENT_ADD;
            }
        }
        else
        {
            if (*percent_cv >= BOX_CURVE_FIT_CV_PERCENT)
            {
                *percent_cv -= BOX_CURVE_FIT_CV_PERCENT_ADD;
            }
        }

        if (tick_cnt_clr)
        {
            *percent_cv_tick_cnt = 0;
        }
#if BOX_VBAT_CORRECTED_TRACE
        bsp_charge_ex_percent_lookup();
#endif
        printf("cv_process: percent[%d]\n", *percent_cv);
    }
}
#endif

WEAK void box_vbat_run_out_callback(void)
{

}

void bsp_charge_ex_process(void)
{
    bsp_charge_ex_hal_process();
    // bsp_charge_ex_percent_update();

#if (BSP_CHARGE_EX_BOX_DETECT == BOX_SOTP_CHARGE)
    bsp_charge_ex_stop_charge_process();
#endif

    if (CHARGE_EX_BAT_RUN_OUT() && !bsp_charge_ex_is_charge())    //电量耗光, 关机
    {
        box_vbat_run_out_callback();
    }


#if BOX_OPEN_WAKEUP_EARPHONE
    bsp_charge_ex_wakeup_earphone_process();
#endif
}

bool bsp_charge_ex_percent_init(void)
{
    u8 percent = cm_read8(PARAM_BOX_PERCENT);

    if ((percent == 0) || (percent > 100) ||            //数值不合理
        cm_read8(PARAM_DEEP_SLEEP_FLAG) == 2)           //仓储(真关机), 开机重新采集电量
    {
        return false;
    }

    charge_ex_cb.percent = percent;
    msg_enqueue(EVT_BOX_BAT_UPDATE);

    TRACE("PERCENT init[%d] percent[%d] flag[%d]\n",
          charge_ex_cb.percent, percent, cm_read8(PARAM_DEEP_SLEEP_FLAG));
    return true;
}

void bsp_charge_ex_init(void)
{
    memset(&charge_ex_cb, 0, sizeof(charge_ex_cb_t));
    charge_ex_cb.percent = PERCENT_NOT_INIT;
    charge_ex_cb.percent_cv_tick_cnt = cm_read8(PARAM_CV_PERCENT_TICKS_CNT);
    bsp_charge_ex_percent_init();
#if BOX_BLE_MASTER_SLAVE_EN
    bsp_earphone_manage_init();

#endif
    bsp_charge_ex_ic_init();
}

#else
void bsp_charge_ex_5V_wakeup(void) {}
bool bsp_charge_ex_comm_backup(void)
{
    return false;
}
void bsp_charge_ex_comm_recover(void) {};

AT(.text.func.idle.charge)
bool bsp_charge_ex_mode_set(u8 mode)
{
    return false;
}
AT(.text.func.idle.charge)
void bsp_charge_ex_open_wakeup(void) {};
AT(.text.func.idle.charge)
u8 bsp_charge_ex_percent_get(void)
{
    return 0;
}

void bsp_charge_ex_bat_low_check(void) {};
#endif  //CHARGE_EX_IC_SELECT
