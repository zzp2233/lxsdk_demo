#include "include.h"
#include "ute_drv_battery_common.h"
#include "ute_drv_temperature_common.h"

#if CHARGE_EN

#define TRACE_EN                UTE_LOG_DRV_BAT_LVL

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define DC_IN()                  ((RTCCON >> 20) & 0x01)            //VUSB Online state:    0->not online, 1->online

static charge_cfg_t charge_cfg;

#if TRACE_EN
AT(.com_rodata.tkey)
const char charge_on_str[] = "Charge on\n";
AT(.com_rodata.tkey)
const char charge_off_str[] = "Charge off: %d\n";
#endif

static void charge_timer_callback (co_timer_t *timer, void *param);

AT(.text.charge_com.sta)
void bsp_charge_sta(u8 sta)
{
#if UTE_DRV_BATTERY_USE_UTE_PERCENTAGE_SUPPORT
    if(sys_cb.charge_sta != sta)
    {
        if(sta == 0 && uteDrvBatteryCommonGetChargerPlug())
        {
            uteDrvBatteryCommonSetChargerPlug(false);
        }
        else if (sta > 0 && !uteDrvBatteryCommonGetChargerPlug())
        {
            uteDrvBatteryCommonSetChargerPlug(true);
        }
    }
#endif
    if (sta == 1)
    {
        //充电开启
        sys_cb.charge_sta = 1;
        sys_cb.charge_bled_flag = 0;
#if !UTE_DRV_BATTERY_USE_UTE_PERCENTAGE_SUPPORT
        if (bt_cb.disp_status <= BT_STA_PLAYING && func_cb.sta != FUNC_OTA_UI_MODE && is_fot_start() == 0)
        {
            if (func_cb.sta != FUNC_CHARGE)
            {
                msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
                msg_enqueue(EVT_MSGBOX_EXIT);
                msg_enqueue(EVT_CLOCK_SUB_SIDE_EXIT);
            }
            func_cb.sta = FUNC_CHARGE;
        }
#endif
        TRACE(charge_on_str);
    }
    else
    {
        //充电关闭
        TRACE(charge_off_str, sta);
        sys_cb.charge_sta = 0;
        sys_cb.charge_bled_flag = 0;
        if (sta == 2)
        {
            sys_cb.charge_sta = 2;
        }
    }
}

void bsp_charge_off(void)
{
    if (charge_cfg.chag_sta > CHAG_STA_OFF)
    {
        charge_stop(0);
    }
}

AT(.text.charge_com.det)
u8 charge_dc_detect(void)
{
    if (!xcfg_cb.charge_en)
    {
        return 0;
    }
    return (RTCCON >> 20) & 0x01;
}

AT(.text.charge_com.det)
bool charge_dc_change_detect(void)
{
    static u8 last_dc_state = 0;
    if (last_dc_state != charge_dc_detect())
    {
        last_dc_state = charge_dc_detect();
        return true;
    }
    return false;
}

void bsp_charge_set_stop_time(u16 stop_time)
{
    charge_cfg.stop_time = stop_time;
}


/**
 * 充电初始化
*/
void bsp_charge_init(void)
{
    charge_cfg_t *p = &charge_cfg;
    static co_timer_t charge_timeer;
    memset(p, 0, sizeof(charge_cfg_t));

    if (xcfg_cb.charge_en)
    {
        p->trick_curr_en = CHARGE_TRICK_EN;
        p->dcin_reset = CHARGE_DC_RESET;
        p->buck_mode_en = BUCK_MODE_EN*xcfg_cb.buck_mode_en;
        p->trick_stop_volt = CHARGE_TRICK_STOP_VOLT;
        p->stop_volt = CHARGE_STOP_VOLT;
        p->follow_volt = CHARGE_VOLT_FOLLOW;
#if UTE_DRV_BATTERY_CUSTOM_CHARGE_CURRENT_SUPPORT
        p->const_curr = UTE_DRV_BATTERY_CHARGE_CURRENT / 5 - 1;
        if(p->const_curr > 63 || p->const_curr == 0)
        {
            p->const_curr = CHARGE_CONSTANT_CURR;
        }
#else
        p->const_curr = CHARGE_CONSTANT_CURR;
#endif
        printf("bsp_charge_init,const_curr:%d ma\n",p->const_curr*5+5);
        p->trick_curr = CHARGE_TRICKLE_CURR;
        p->stop_curr = CHARGE_STOP_CURR;
        p->stop_curr_thd = 80;
        p->bled_on_pr = 0;
        p->detect_cnt = 5;
        p->stop_time = 36000;                               //默认60分钟

        RTCCON3 &= ~BIT(12);                                //RTCCON3[12], INBOX Wakeup disable
        RTCCON7 &= ~BIT(6);                                 //PWRUPICH = 20mA
        RTCCON8 |= BIT(4);                                  //inbox voltage selection.  1'b1,  1.7v;   1'b0, 1.1v
        sys_cb.chg_on = 1;
        charge_init(p);
    }

    co_timer_set(&charge_timeer, 5000, TIMER_REPEAT, LEVEL_LOW_PRI, charge_timer_callback, NULL);
    co_timer_set_sleep(&charge_timeer, true);
}


/**
 * 充电开关，原子操作
*/
ALIGNED(128)
static void charge_ctrl(bool onoff)
{
    GLOBAL_INT_DISABLE();
    RTCCON8 = onoff ? (RTCCON8 & ~BIT(1)) : (RTCCON8 | BIT(1));
    sys_cb.chg_on = onoff;
    GLOBAL_INT_RESTORE();
}


/**
 * 充电保护管理，续充，NTC保护 5s执行一次
 */
static void charge_timer_callback (co_timer_t *timer, void *param)
{
    static charge_sta_m charge_full_flag = CHARGE_NO_FULL;
    static uint16_t refill_count = 0;
    static uint16_t refill_curr_cnt = 0;
#if (CHARGE_USER_NTC_EN && TS_MODE_EN) || UTE_DRV_BATTERY_CE_AUTH_SUPPORT
    static uint16_t charge_ntc_stop_flag = 0;
    static int16_t ambientTemperature = 0;
#endif

#if TS_MODE_EN
    adc_cb.ntc_temp = ts_adc2ntc(adc_cb.vts);
    ambientTemperature = adc_cb.ntc_temp;
    TRACE("VTS:%d NTC:%d\n", adc_cb.vts, ambientTemperature);
#elif UTE_DRV_BATTERY_CE_AUTH_SUPPORT
    if(uteDrvTemperatureCommonIsInitSuccessful())
    {
        uteDrvTemperatureCommonPowerOn(true);
        uteDrvTemperatureCommonEverySecond();
        ambientTemperature = (int16_t)uteDrvTemperatureCommonGetAmbientValue();
    }
    else
    {
        ambientTemperature = 25;
    }
    TRACE("NTC:%d\n", ambientTemperature);
#endif // TS_MODE_EN

    TRACE("DC_IN:%d charge_sta:%d VBAT:%d VBAT2:%d\n", CHARGE_DC_IN(), sys_cb.charge_sta, adc_cb.vbat_val, adc_cb.vbat2);

    switch(sys_cb.charge_sta)
    {
        case 0:             //移开5v
            charge_full_flag = CHARGE_NO_FULL;
#if (CHARGE_USER_NTC_EN && TS_MODE_EN) || UTE_DRV_BATTERY_CE_AUTH_SUPPORT
            charge_ntc_stop_flag = 0;
#endif
            charge_ctrl(1);
            break;
        case 1:             //充电中
            //NTC
#if (CHARGE_USER_NTC_EN && TS_MODE_EN) || UTE_DRV_BATTERY_CE_AUTH_SUPPORT
            if((charge_ntc_stop_flag== 0)  && ((ambientTemperature > CHARGE_NTC_ADC_MAX_TEMP) || (ambientTemperature < CHARGE_NTC_ADC_MIN_TEMP)))
            {
                //温度异常 停止充
                TRACE("NTC STOP CHARGE:%d\n", ambientTemperature);
                charge_ntc_stop_flag = 1;
                charge_ctrl(0);
            }

            if((charge_ntc_stop_flag== 1)  && (ambientTemperature < CHARGE_NTC_ADC_MAX_RE_TEMP) && (ambientTemperature > CHARGE_NTC_ADC_MIN_RE_TEMP))
            {
                //温度恢复 继续充
                TRACE("NTC Continue CHARGE:%d\n", ambientTemperature);
                charge_ntc_stop_flag = 0;
                charge_ctrl(1);
            }
#endif
            break;
        case 2:             //充电满
#if (CHARGE_USER_NTC_EN && TS_MODE_EN) || UTE_DRV_BATTERY_CE_AUTH_SUPPORT
            charge_ntc_stop_flag = 0;
#endif
            if(charge_full_flag == CHARGE_NO_FULL)
            {
                charge_full_flag = CHARGE_FULL;
                //停止充
                charge_ctrl(0);
            }
            else if(charge_full_flag == CHARGE_FULL)      //充满状态，检测电池是否放电到续充
            {
                if(sys_cb.vbat < CHARGE_VBAT_REFILL)
                {
                    TRACE("start refill:%d\n", sys_cb.vbat);
                    charge_full_flag = CHARGE_REFILL;
                    refill_count = 0;
                    refill_curr_cnt = 0;
                    charge_ctrl(1);
                }
            }
            else if(charge_full_flag == CHARGE_REFILL)   //续充判断是否充满
            {
                refill_count++;

                if (PWRCON1 & BIT(30))   //I
                {
                    refill_curr_cnt++;
                }
                else
                {
                    if (refill_count % 20 == 0)  //每100s一个计数周期
                    {
                        refill_curr_cnt = 0;
                    }
                }

                if((refill_count > 240) || (refill_curr_cnt > 10)) // 限制最多充20分钟 电流满足10s
                {
                    TRACE("stop refill:%d %d\n", refill_count, refill_curr_cnt);
                    charge_full_flag = CHARGE_FULL;
                    //停止充
                    charge_ctrl(0);
                }
            }
            break;
    }
}


void charge_set_detect_cnt(u8 value)
{
    charge_cfg.detect_cnt = value;
}


/**
 *   充电状态函数
 *   返回充电状态
 *   0 : 5V没接上
     1 : 充电中
     2 ：充满
 */
u8 bsp_charge_sta_get(void)
{
    return sys_cb.charge_sta;
}

#endif // CHARGE_EN
