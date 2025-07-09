#ifndef _BSP_H
#define _BSP_H

#include "bsp_halt.h"
#include "bsp_sys.h"
#include "bsp_hw_timer.h"
#include "bsp_pwm.h"
#include "bsp_key.h"
#include "bsp_dac.h"
#include "bsp_param.h"
#include "bsp_audio.h"
#include "bsp_music.h"
#include "bsp_bt.h"
#include "bsp_eq.h"
#include "bsp_piano.h"
#include "bsp_i2c.h"
#include "bsp_ble.h"
#include "bsp_app.h"
#include "bsp_charge.h"
#include "bsp_saradc.h"
#include "bsp_vbat.h"
#include "bsp_rtc.h"
#include "bsp_sensor.h"
#include "bsp_port_int.h"
#include "bsp_sleep.h"
#include "bsp_vusb.h"
#include "bsp_uitool_phrase.h"
#include "bsp_modem.h"
#include "bsp_opus.h"
#include "bsp_call_mgr.h"
#include "bsp_spi1flash.h"

#if ECIG_POWER_CONTROL
#include "bsp_ecig.h"
#include "ecig.h"
#include "bsp_ecig_polling_algrithm.h"
#include "bsp_ecig_switch_algrithm.h"
#endif
#include "bsp_gpio.h"
#include "bsp_charge_ex.h"


#if BT_TWS_EN
#define TICK_MAX_DELAY                  TWS_TICK_MAX_DELAY
#define TICKN_GET()                     tws_time_tickn_get()
#define TICKN_IS_EXPIRE(tickn)          tws_time_tickn_expire(tickn)

#define TICK_TIME_GET(t, d)             tws_time_get(t, d)
#define TICK_ADD(a, b)                  TWS_TICK_ADD(a, b)
#define TICK_SUB(a, b)                  TWS_TICK_SUB(a, b)
#define TICK_GEQ(a, b)                  TWS_TICK_GEQ(a, b)
#define TICK_2_TICKN(tick, tickoff)     TWS_TICK_2_TICKN(tick, tickoff)
#define TICKN_2_TICK(tickn, tickoff)    TWS_TICKN_2_TICK(tickn, tickoff)
#else
#define TICK_MAX_DELAY                  0x7fffffff
#define TICKN_GET()                     tick_get()
#define TICKN_IS_EXPIRE(tickn)          (tickn - tick_get() >= TICK_MAX_DELAY)

#define TICK_TIME_GET(t, d)             (t)->tickn = tick_get() + d
#define TICK_ADD(a, b)                  (a + b)
#define TICK_SUB(a, b)                  (a - b)
#define TICK_GEQ(a, b)                  ((a - b) < TICK_MAX_DELAY)
#define TICK_2_TICKN(tick, tickoff)     tick        //非TWS时tick和tickn相等
#define TICKN_2_TICK(tickn, tickoff)    tickn       //非TWS时tick和tickn相等
#endif // BT_TWS_EN
#endif
