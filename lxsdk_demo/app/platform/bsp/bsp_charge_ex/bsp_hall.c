#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

hall_cb_t hall_cb;

#if HALL_EN

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
#define HALL_CHARGE_START_DELAY     3000
#else
#define HALL_CHARGE_START_DELAY     2000    //关盖开始充电延时, 单位(ms)
#endif

AT(.text.func.idle.hall)
void bsp_hall_open_action(void)
{
    TRACE("%s\n", __func__);

    bsp_charge_ex_mode_set(BOX_COMM_MODE);
    bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_OPEN_WINDOW, LEFT_CHANNEL_USER);
    bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_OPEN_WINDOW, RIGHT_CHANNEL_USER);

    //开盖获取对耳地址
    if (bsp_earphone_all_inbox() && earphone.tws_addr_flag == false)  //双耳入仓且本次开机未连接过耳机
    {
        bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PAIR, RIGHT_CHANNEL_USER);
    }

    if (ble_host_scan_allow())
    {
        ble_host_delay_start_scan(100);
    }
    else
    {
        ble_host_delay_start_scan(1000);
    }

    reset_sleep_delay_all();
}

AT(.text.func.idle.hall)
void bsp_hall_open_callback(void)
{
    TRACE("%s\n", __func__);

    hall_cb.status = 1;
    hall_cb.charge_tick = 0;
    hall_cb.charge_start_tick = 0;

    bsp_smart_vhouse_cmd_reset();

    msg_enqueue(EVT_MSG_HALL_OPEN);
#if BOX_OPEN_WAKEUP_EARPHONE
    charge_ex_cb.wkup_0V_tick = 0;
    charge_ex_cb.wkup_5V_tick = 0;
#endif
}

void bsp_hall_close_action(void)
{
    TRACE("%s\n", __func__);

    if (bsp_earphone_all_inbox())
    {
        ble_host_stop_scan();
    }

#if HALL_CHARGE_START_DELAY
    bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_CLOSE_WINDOW, RIGHT_CHANNEL_USER);
    if (CHARGE_EX_BAT_IS_LOW())                     //仓低电发关机码
    {
        bsp_smart_vhouse_cmd_reset();
        bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PWROFF, LEFT_CHANNEL_USER);
        bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PWROFF, RIGHT_CHANNEL_USER);
    }
    else
    {
        if (bsp_earphone_all_inbox())               //关盖查询信息, 检查耳机是否是一对
        {
            bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_GET_EAR_ADDR, LEFT_CHANNEL_USER);
            bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_GET_EAR_ADDR, RIGHT_CHANNEL_USER);
        }
    }
#endif // HALL_CHARGE_START_DELAY

    hall_cb.charge_tick = tick_get();
}

AT(.text.func.idle.hall)
void bsp_hall_close_callback(void)
{
#if GUI_SELECT
    if (hall_cb.status)
    {
        if (!tft_bglight_kick_get())  //息屏
        {
            sys_cb.sleep_delay = 0;
            sys_cb.guioff_delay = 0;
        }
    }
#endif // GUI_SELECT

    TRACE("%s\n", __func__);
    hall_cb.status = 0;
    bsp_smart_vhouse_cmd_reset();
    msg_enqueue(EVT_MSG_HALL_CLOSE);
#if BOX_OPEN_WAKEUP_EARPHONE
    if (CHARGE_EX_BAT_IS_LOW())
    {
        bsp_hall_close_action();
    }
    else
    {
        bsp_charge_ex_5V_wakeup();
    }
#else
    bsp_hall_close_action();
#endif
}

AT(.sleep_text.sleep)
u8 bsp_hall_action_check(void)
{
    static uint8_t hall_open_cnt = 0;
    static uint8_t hall_close_cnt = 0;

    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, HALL_GPIO);

    if (gpio.sfr)
    {
        uint8_t hall_level = gpio.sfr[GPIOx] & BIT(gpio.num);

        if (hall_cb.status != 1 && hall_level)
        {
            hall_close_cnt = 0;
//            TRACE("hall_level[%d]\n", hall_level);
            if (hall_open_cnt++ > 10)
            {
                bsp_hall_open_callback();
                return HALL_OPEN_BOX;
            }
        }
        else if (hall_cb.status != 0 && !hall_level)
        {
            hall_open_cnt = 0;
//            TRACE("hall_level[%d]\n", hall_level);
            if (hall_close_cnt++ > 10)
            {
                bsp_hall_close_callback();
                return HALL_CLOSE_BOX;
            }
        }
    }
    return HALL_NO_ACTION;
}

AT(.text.func.idle.charge)
void bsp_hall_process(void)
{
    bsp_hall_action_check();

    if (hall_cb.charge_tick && tick_check_expire(hall_cb.charge_tick, HALL_CHARGE_START_DELAY))     //关盖延时打开充电
    {
        hall_cb.charge_tick = 0;
//        smart_vhouse_tx_dis();                                    //关闭comm脚避免漏电
        if (CHARGE_EX_BAT_IS_LOW())                                 //低电不充电, 维持电压
        {
            TRACE("BOX CLOSE to comm\n");
            bsp_charge_ex_hal_mode_set(BOX_COMM_MODE);
        }
        else
        {
            TRACE("BOX CLOSE to charge\n");
            hall_cb.charge_start_tick = tick_get();
            bsp_charge_ex_mode_set(BOX_CHARGE_MODE);
        }
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
        for (u8 i = 0; i < BREATH_TIMEOUT_CNT; i++)                   //进充电立即查询耳机是否在充
        {
            bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_CLOSE_WIN_GET_VBAT, LEFT_CHANNEL_USER);
            bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_CLOSE_WIN_GET_VBAT, RIGHT_CHANNEL_USER);
        }
#endif
        msg_enqueue(EVT_EARPHONE_BAT_UPDATE);
    }
}

void bsp_hall_init(void)
{
    gpio_t gpio;

    bsp_gpio_cfg_init(&gpio, HALL_GPIO);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);       //0:当作通用GPIO使用
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);         //数字IO使能: 0为模拟IO, 1 为数字IO,
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);        //控制IO的方向:  0为输出, 1为输入.
        gpio.sfr[GPIOxPU] |= BIT(gpio.num);
    }

    memset(&hall_cb, 0, sizeof(hall_cb));
    hall_cb.charge_tick = tick_get();               //开机倒计时进充电
}

#else   //无hall充电仓方案

void bsp_box_set_comm_timeout_flg(u8 flg)
{
    hall_cb.noear_inbox_flg |= flg;
}

u8 bsp_box_get_comm_timeout_flg(void)
{
    return hall_cb.noear_inbox_flg;
}

void bsp_no_hall_pair_action(void)
{
    hall_cb.charge_tick = 0;
    hall_cb.noear_inbox_flg = 0;            //亮屏默认耳机未出仓

    if (earphone.paired == false)
    {
        if (CHARGE_EX_BAT_IS_LOW())
        {
            bsp_no_hall_wakeup_action();
        }
        else
        {
            bsp_charge_ex_5V_wakeup();      //唤醒耳机配对
        }
    }
}

void bsp_no_hall_idle_enter_callback(void)
{
    printf("%s\n",__func__);
    if (bsp_earphone_all_inbox() || (bsp_earphone_is_charge(EARPHONE_LEFT) && bsp_earphone_is_charge(EARPHONE_RIGHT)))   //耳机充满出仓需要开启ble广播连接耳机
    {
        ble_host_stop_scan();
    }

    bsp_smart_vhouse_cmd_reset();
    //灭屏进入充电模式
    if (bsp_charge_ex_mode() != BOX_CHARGE_MODE)
    {
        hall_cb.charge_tick = tick_get();
    }

    if (CHARGE_EX_BAT_IS_LOW())                     //仓低电发关机码
    {
        printf("low bat ,pwr off earphone!\n");
        if (CHARGE_EX_BAT_IS_LOW())
        {
            bsp_no_hall_wakeup_action();
        }
        else
        {
            bsp_charge_ex_5V_wakeup();
        }
    }
}

void bsp_no_hall_idle_exit_callback(void)
{
    printf("%s\n",__func__);

    bsp_no_hall_pair_action();

    if ((!bsp_earphone_is_charge(EARPHONE_LEFT)) || (!bsp_earphone_is_charge(EARPHONE_RIGHT)))   //耳机充满出仓需要开启ble广播连接耳机
    {
        if (ble_host_scan_allow())
        {
            printf("box start scan ble!\n");
            ble_host_delay_start_scan(1000);
        }
    }

    bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_CLOSE_WIN_GET_VBAT, LEFT_CHANNEL_USER);
    bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_CLOSE_WIN_GET_VBAT, RIGHT_CHANNEL_USER);
}

void bsp_no_hall_earphone_inbox_callback(u8 index)
{
    if (bsp_earphone_all_inbox())
    {
        ble_host_stop_scan();
    }

    if (func_cb.sta != FUNC_IDLE)
    {
        if (bsp_charge_ex_mode() == BOX_COMM_MODE)
        {
            //获取对耳地址
            if (bsp_earphone_is_inbox(EARPHONE_LEFT) && (index == 0))
            {
                if (bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_GET_EAR_ADDR, LEFT_CHANNEL_USER))
                {
                    printf("CMD_L_GET_EAR_ADDR\n");
                }
            }
            if (bsp_earphone_is_inbox(EARPHONE_RIGHT) && (index == 1))
            {
                if (bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_GET_EAR_ADDR, RIGHT_CHANNEL_USER))
                {
                    printf("CMD_R_GET_EAR_ADDR\n");
                }
            }
        }
    }
}

void bsp_no_hall_earphone_outbox_callback(u8 index)
{
#if BOX_BLE_MASTER_SLAVE_EN
//    printf("%s\n",__func__);
    if (sys_cb.gui_sleep_sta == 0)
    {
        msg_enqueue(EVT_EAR_OUTBOX_START_SCAN_BLE);
    }
#endif
}

void bsp_no_hall_wakeup_action(void)
{
    printf("%s: LOW[%d]\n",__func__, CHARGE_EX_BAT_IS_LOW());
    bsp_smart_vhouse_cmd_reset();
    bsp_charge_ex_mode_set(BOX_COMM_MODE);

    if (CHARGE_EX_BAT_IS_LOW())                     //仓低电发关机码
    {
        bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PWROFF, LEFT_CHANNEL_USER);
        bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PWROFF, RIGHT_CHANNEL_USER);
    }
    else
    {
        bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_GET_EAR_ADDR, LEFT_CHANNEL_USER);
        bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_GET_EAR_ADDR, RIGHT_CHANNEL_USER);
    }
    hall_cb.charge_tick = tick_get();
};

AT(.text.func.idle.charge)
void bsp_no_hall_process(void)
{
    static u32 ticks = 0;

    if (bsp_charge_ex_is_waking_up())
    {
        return;
    }

    if (tick_check_expire(ticks, 1000))
    {
        ticks = tick_get();

        if (!CHARGE_EX_BAT_IS_LOW())
        {
            if (charge_ex_mode_is_comm())  //通讯模式已配对, 进充电模式
            {
                if (bsp_earphone_is_paired())
                {
                    printf("paired, BOX to charge\n");
                    bsp_smart_vhouse_cmd_reset();
                    bsp_charge_ex_mode_set(BOX_CHARGE_MODE);
                }
            }
        }
    }

    //延时进充电
    if (hall_cb.charge_tick && tick_check_expire(hall_cb.charge_tick, 5000))
    {
        hall_cb.charge_tick = 0;
        if (CHARGE_EX_BAT_IS_LOW())
        {
            TRACE("TICK to comm\n");
            bsp_charge_ex_mode_set(BOX_COMM_MODE);
            bsp_smart_vhouse_cmd_reset();
            bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PWROFF, LEFT_CHANNEL_USER);
            bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PWROFF, RIGHT_CHANNEL_USER);
        }
        else
        {
            TRACE("TICK to charge\n");
            bsp_charge_ex_mode_set(BOX_CHARGE_MODE);
        }
    }

    //检查出仓, 发起ble连接与充电
    if (charge_ex_mode_is_comm())
    {
        if (!CHARGE_EX_BAT_IS_LOW())
        {
            if ((bsp_box_get_comm_timeout_flg() & LEFT_CHANNEL_USER) && (bsp_box_get_comm_timeout_flg() & RIGHT_CHANNEL_USER))   //双耳不在仓内
            {

                if (ble_host_scan_allow())
                {
                    ble_host_delay_start_scan(100);
                }
                else
                {
                    ble_host_delay_start_scan(1000);
                }
                printf("ear timeout, ble start, box charge\n");
                bsp_smart_vhouse_cmd_reset();
                bsp_charge_ex_mode_set(BOX_CHARGE_MODE);
            }
        }
    }
    else if ((bsp_charge_ex_mode() == BOX_CHARGE_MODE))
    {
        if (CHARGE_EX_BAT_IS_LOW())
        {
            bsp_charge_ex_mode_set(BOX_COMM_MODE);
        }
    }
}

void bsp_no_hall_init(void)
{
    printf("%s\n",__func__);

    hall_cb.charge_tick = 0;
    hall_cb.noear_inbox_flg = 0;

    if (bsp_earphone_is_paired())
    {
        bsp_charge_ex_mode_set(BOX_CHARGE_MODE);
    }
    else
    {
        bsp_charge_ex_5V_wakeup();   //唤醒耳机组队
    }
}
#endif  //HALL_EN
