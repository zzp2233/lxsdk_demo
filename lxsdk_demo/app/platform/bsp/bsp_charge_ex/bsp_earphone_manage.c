#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#if SMART_VHOUSE_HOST_EN

earphone_t earphone;

void bsp_earphone_emit_notice(uint evt, u32 param)
{
    switch(evt)
    {
        case EARPHONE_NOTICE_LEFT_INBOX:
            printf("EARPHONE_NOTICE_LEFT_INBOX\n");
            break;
        case EARPHONE_NOTICE_LEFT_OUTBOX:
            printf("EARPHONE_NOTICE_LEFT_OUTBOX\n");
            break;
        case EARPHONE_NOTICE_RIGHT_INBOX:
            printf("EARPHONE_NOTICE_RIGHT_INBOX\n");
            break;
        case EARPHONE_NOTICE_RIGHT_OUTBOX:
            printf("EARPHONE_NOTICE_RIGHT_OUTBOX\n");
            break;
    }
#if HALL_EN
    msg_enqueue(EVT_MSG_INOUT_BOX);
#endif
}

void bsp_earphone_inbox_callback(u8 index)
{
//    printf("%s\n", __func__);

//    if (tick_check_expire(earphone.outbox_tick, 3000) && bsp_earphone_all_inbox() && !ble_host_is_connected()) {//出仓超过一定时间, 且所有耳机插入, 且无蓝牙连接, 发配对码
//        bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PAIR, RIGHT_CHANNEL_USER);
//    }

    if (bsp_earphone_all_inbox())                       //入仓音乐自动暂停
    {
        ab_mate_host_music_set_sta(false);
    }

#if HALL_EN
    if (CHARGE_EX_BAT_IS_LOW() && bsp_hall_close_box())  //耳机已关机时关盖唤醒, 检测到插入发关机码
    {
        bsp_smart_vhouse_cmd_reset();
        bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PWROFF, LEFT_CHANNEL_USER);
        bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PWROFF, RIGHT_CHANNEL_USER);
    }
#else
    bsp_no_hall_earphone_inbox_callback(index);
#endif
}

void bsp_earphone_outbox_callback(u8 index)
{
//    printf("%s\n", __func__);

    bsp_earphone_vbat_var_clr(index);
#if !HALL_EN
    bsp_no_hall_earphone_outbox_callback(index);
#endif
}

void bsp_earphone_inbox(u8 index, bool inbox)
{
//     printf("%s: index:%d inbox:%d\n", __func__, index, inbox);

    if (earphone.inbox[index] != inbox)
    {
//        printf("ear[%d] = inbox[%d]\n", index, inbox);
        earphone.inbox[index] = inbox;
#if HALL_EN
        msg_enqueue(EVT_EARPHONE_BAT_UPDATE);   //更新耳机充电图标
#endif
        if (index == EARPHONE_LEFT)
        {
            bsp_earphone_emit_notice(EARPHONE_NOTICE_LEFT_OUTBOX + inbox, 0);
        }
        else if (index == EARPHONE_RIGHT)
        {
            bsp_earphone_emit_notice(EARPHONE_NOTICE_RIGHT_OUTBOX + inbox, 0);
        }

        if (inbox)
        {
            bsp_earphone_inbox_callback(index);
        }
        else
        {
            bsp_earphone_outbox_callback(index);
        }
    }
}

u8 bsp_earphone_vbat_get(u8 side)
{
    u8 v = bsp_earphone_vbat_get_real(side);
    if (side < EARPHONE_MAX)
    {
#if (EARPHONE_VBAT_STEP > 1)
        if (v <= 100)
        {
            return (((v + (EARPHONE_VBAT_STEP - 1)) / EARPHONE_VBAT_STEP) * EARPHONE_VBAT_STEP);
        }
#endif
        return v;
    }
    else
    {
        return 0;
    }
}

u8 bsp_earphone_get_index_bych(u8 channel)
{
    u8 index = EARPHONE_INDEX_ERR;

    if (channel == LEFT_CHANNEL_USER)
    {
        index = EARPHONE_LEFT;
    }
    else if (channel == RIGHT_CHANNEL_USER)
    {
        index = EARPHONE_RIGHT;
    }

#if TRACE_EN
    if (index == EARPHONE_INDEX_ERR)
    {
        TRACE("get_index_bych err!\n");
    }
#endif
    return index;
}

void bsp_earphone_set_vbat(u8 index, u8 vbat)
{
    if (index == EARPHONE_INDEX_ERR)
    {
        return;
    }

    if (earphone.vbat[index] == 0)              //电量未初始化
    {
        earphone.vbat[index] = vbat;
    }
    else
    {
        if (bsp_earphone_is_charge(index))      //耳机充电时仅允许电量上升
        {
            if (earphone.vbat[index] < vbat)
            {
                earphone.vbat[index] = vbat;
            }
        }
        else
        {
            if (earphone.vbat[index] > vbat)    //耳机不充电时仅允许电量下降
            {
                earphone.vbat[index] = vbat;
            }
        }
    }
}

void bsp_earphone_ble_vbat(u8 index, u8 vbat)
{
    if (vbat == 0)     //不允许ble电量清0
    {
        return;
    }

    bsp_earphone_set_vbat(index, vbat);
}

void bsp_earphone_play_set(u8 play)
{
    printf("%s:%d->%d %d \n", __func__, earphone.play_sta_raw, play, earphone.play_sta);

    if (earphone.play_sta_raw != play)
    {
        earphone.play_sta_raw = play;
        if (tick_check_expire(earphone.play_change_tick, 2000))  //一段时间内连续变化, 延时刷新状态
        {
            earphone.play_change_tick = 0;
        }
        else
        {
            earphone.play_change_tick = tick_get();
        }

        if (earphone.play_change_tick)
        {
            earphone.play_change_delay_tick = tick_get();
        }
        else
        {
            earphone.play_sta = play;
            msg_enqueue(EVT_EARPHONE_MUSIC_UPDATE);
            earphone.play_change_tick = tick_get();
            earphone.play_change_delay_tick = 0;
            printf("EVT_EARPHONE_MUSIC_UPDATE 1\n");
        }
    }
}

void bsp_earphone_save_addr(u8 *buf, u8 len)
{
    if (len > EARPHONE_TWS_ADDR_LEN)
    {
        return;
    }

    if (array_is_null(buf, len))
    {
        printf("TWS ADDR is null!\n");
        return;
    }

    printf("TWS ADDR[%d]:");
    print_r(buf, len);

    if(!array_is_equal(&earphone.tws_addr[0], buf, len))
    {
        printf("TWS addr change\n");

        ble_host_stop_scan();
        if (ble_host_is_connected())
        {
            ble_disconnect(HCI_ROLE_MASTER);
            ble_host_delay_start_scan(2000);        //耳机地址改变, 断开重新回连
        }
        memcpy(&earphone.tws_addr[0], buf, len);
#if (USE_APP_TYPE == USE_APP_CHARGEBOX)
        if (!ble_is_connected())
        {
            ble_set_adv_data_from_hook();
            ble_adv_en();
        }
#endif // USE_APP_CHARGEBOX
    }
}

void bsp_earphone_pair_finish(void)
{
    printf("%s\n", __func__);
#if LE_AB_MATE_MASTER_EN
    if (earphone.tws_addr_flag == false)
    {
        printf("TWS ADDR save:\n");
        print_r(&earphone.tws_addr[0], EARPHONE_TWS_ADDR_LEN);
        cm_write(&earphone.tws_addr[0], PARAM_EARPHONE_TWS_ADDR, EARPHONE_TWS_ADDR_LEN);
    }
#endif
    earphone.tws_addr_flag = true;

    if (bsp_earphone_link_key_paired() && !bsp_earphone_is_paired())
    {
        earphone.paired = true;
        cm_write(&earphone.paired, PARAM_EARPHONE_PAIRED, 1);
        msg_enqueue(EVT_CHARGEBOX_PAIR_FINISH);
        printf("earphone_is_paired!\n");
    }
}

void bsp_earphone_pair_clr(void)
{
    memset(&earphone.tws_addr[0], 0, EARPHONE_TWS_ADDR_LEN);
    memset(earphone.bt_addr[EARPHONE_LEFT], 0, 6);
    memset(earphone.bt_addr[EARPHONE_RIGHT], 0, 6);

    cm_write(&earphone.tws_addr[0], PARAM_EARPHONE_TWS_ADDR, 6);
    cm_write(earphone.bt_addr[EARPHONE_LEFT], PARAM_EARPHONE_L_ADDR, 6);
    cm_write(earphone.bt_addr[EARPHONE_RIGHT], PARAM_EARPHONE_R_ADDR, 6);

    earphone.paired = false;
}

AT(.text.func.idle.charge)
bool bsp_earphone_with_inbox(void)
{
    return (earphone.inbox[EARPHONE_LEFT] || earphone.inbox[EARPHONE_RIGHT]);
}

bool bsp_earphone_with_charge(void)
{
    //是否有耳机充电
    if (!bsp_hall_close_box() || sys_cb.bat_low)
    {
        return false;
    }
    if (bsp_earphone_is_inbox(EARPHONE_LEFT) && !bsp_earphone_vbat_is_full(EARPHONE_LEFT))
    {
        return true;
    }
    if (bsp_earphone_is_inbox(EARPHONE_RIGHT) && !bsp_earphone_vbat_is_full(EARPHONE_RIGHT))
    {
        return true;
    }
    return false;
}

bool bsp_earphone_all_inbox(void)
{
    return (earphone.inbox[EARPHONE_LEFT] && earphone.inbox[EARPHONE_RIGHT]);
}

void bsp_earphone_manage_process(void)
{
    if (earphone.play_change_delay_tick && tick_check_expire(earphone.play_change_tick, 2000))      //一段时间内播放状态无变化, 则保存
    {
        printf("bsp_earphone_play_set delay %d->%d\n", earphone.play_sta_raw, earphone.play_sta);
        earphone.play_change_delay_tick = 0;
        earphone.play_sta = earphone.play_sta_raw;
        msg_enqueue(EVT_EARPHONE_MUSIC_UPDATE);
    }
}

void bsp_earphone_manage_init(void)
{
    memset(&earphone, 0, sizeof(earphone_t));
    cm_read(&earphone.tws_addr[0], PARAM_EARPHONE_TWS_ADDR, 6);
    cm_read(&earphone.bt_addr[EARPHONE_LEFT][0], PARAM_EARPHONE_L_ADDR, 6);
    cm_read(&earphone.bt_addr[EARPHONE_RIGHT][0], PARAM_EARPHONE_R_ADDR, 6);

    u8 vbat_read = cm_read8(PARAM_EARPHONE_VBAT_L);
    earphone.vbat[EARPHONE_LEFT] = vbat_read == 100 ? 100 : 0;
    vbat_read = cm_read8(PARAM_EARPHONE_VBAT_R);
    earphone.vbat[EARPHONE_RIGHT] = vbat_read == 100 ? 100 : 0;

    if (cm_read8(PARAM_EARPHONE_PAIRED))
    {
        earphone.paired = true;
    };

#if HALL_EN
    if (!sys_cb.rtc_first_pwron)
    {
        u8 l_inbox_read = cm_read8(PARAM_EARPHONE_INBOX_L);
        earphone.inbox[EARPHONE_LEFT] = l_inbox_read;
        u8 r_inbox_read = cm_read8(PARAM_EARPHONE_INBOX_R);
        earphone.inbox[EARPHONE_RIGHT] = r_inbox_read;
        TRACE("l_inbox:%d,r_inbox:%d\n",l_inbox_read,r_inbox_read);
    }
    else
    {
        TRACE("rtc first pwron not use inbox status\n");
    }
#else
    if (earphone.paired)
    {
        msg_enqueue(EVT_EAR_OUTBOX_START_SCAN_BLE);
    }
    TRACE("%s: paired[%x]\n",__func__, earphone.paired);
#endif
}

#else   //非智能仓方案

earphone_t earphone;

void bsp_earphone_inbox(u8 index, bool inbox) {};

#if BOX_BLE_MASTER_SLAVE_EN

u8 bsp_earphone_vbat_get(u8 side)
{
    u8 v = bsp_earphone_vbat_get_real(side);
    if (side < EARPHONE_MAX)
    {
#if (EARPHONE_VBAT_STEP > 1)
        if (v <= 100)
        {
            return (((v + (EARPHONE_VBAT_STEP - 1)) / EARPHONE_VBAT_STEP) * EARPHONE_VBAT_STEP);
        }
#endif
        return v;
    }
    else
    {
        return 0;
    }
}

void bsp_earphone_ble_vbat(u8 index, u8 vbat)
{
    if (vbat == 0)     //不允许ble电量清0
    {
        return;
    }

    earphone.vbat[index] = vbat;
}

void bsp_earphone_play_set(u8 play)
{
    printf("%s:%d->%d %d \n", __func__, earphone.play_sta_raw, play, earphone.play_sta);

    if (earphone.play_sta_raw != play)
    {
        earphone.play_sta_raw = play;
        if (tick_check_expire(earphone.play_change_tick, 2000))  //一段时间内连续变化, 延时刷新状态
        {
            earphone.play_change_tick = 0;
        }
        else
        {
            earphone.play_change_tick = tick_get();
        }

        if (earphone.play_change_tick)
        {
            earphone.play_change_delay_tick = tick_get();
        }
        else
        {
            earphone.play_sta = play;
            msg_enqueue(EVT_EARPHONE_MUSIC_UPDATE);
            earphone.play_change_tick = tick_get();
            earphone.play_change_delay_tick = 0;
            printf("EVT_EARPHONE_MUSIC_UPDATE 1\n");
        }
    }
}

void bsp_earphone_save_addr(u8 *buf, u8 len)
{
    if (len > EARPHONE_TWS_ADDR_LEN)
    {
        return;
    }

    if (array_is_null(buf, len))
    {
        printf("TWS ADDR is null!\n");
        return;
    }

    printf("TWS ADDR[%d]:");
    print_r(buf, len);

    if(!array_is_equal(&earphone.tws_addr[0], buf, len))
    {
        printf("TWS addr change\n");
        ble_host_stop_scan();
        if (ble_host_is_connected())
        {
            ble_disconnect(HCI_ROLE_MASTER);
            ble_host_delay_start_scan(2000);        //耳机地址改变, 断开重新回连
        }
        memcpy(&earphone.tws_addr[0], buf, len);
    }
}

void bsp_earphone_pair_finish(void)
{
    printf("%s\n", __func__);
#if LE_AB_MATE_MASTER_EN
    if (earphone.tws_addr_flag == false)
    {
        printf("TWS ADDR save:\n");
        print_r(&earphone.tws_addr[0], EARPHONE_TWS_ADDR_LEN);
        cm_write(&earphone.tws_addr[0], PARAM_EARPHONE_TWS_ADDR, EARPHONE_TWS_ADDR_LEN);
    }
#endif
    earphone.tws_addr_flag = true;

    if (bsp_earphone_link_key_paired() && !bsp_earphone_is_paired())
    {
        earphone.paired = true;
        cm_write(&earphone.paired, PARAM_EARPHONE_PAIRED, 1);
        msg_enqueue(EVT_CHARGEBOX_PAIR_FINISH);
        printf("earphone_is_paired!\n");
    }
}

void bsp_earphone_pair_clr(void)
{
    memset(&earphone.tws_addr[0], 0, EARPHONE_TWS_ADDR_LEN);
    memset(earphone.bt_addr[EARPHONE_LEFT], 0, 6);
    memset(earphone.bt_addr[EARPHONE_RIGHT], 0, 6);

    cm_write(&earphone.tws_addr[0], PARAM_EARPHONE_TWS_ADDR, 6);
    cm_write(earphone.bt_addr[EARPHONE_LEFT], PARAM_EARPHONE_L_ADDR, 6);
    cm_write(earphone.bt_addr[EARPHONE_RIGHT], PARAM_EARPHONE_R_ADDR, 6);

    earphone.paired = false;
}

AT(.text.func.idle.charge)
bool bsp_earphone_with_inbox(void)
{
    return false;
}
bool bsp_earphone_with_charge(void)
{
    return false;   //是否有耳机充电
}
bool bsp_earphone_all_inbox(void)
{
    return false;
}

void bsp_earphone_manage_process(void)
{
    if (earphone.play_change_delay_tick && tick_check_expire(earphone.play_change_tick, 2000))      //一段时间内播放状态无变化, 则保存
    {
        printf("bsp_earphone_play_set delay %d->%d\n", earphone.play_sta_raw, earphone.play_sta);
        earphone.play_change_delay_tick = 0;
        earphone.play_sta = earphone.play_sta_raw;
        msg_enqueue(EVT_EARPHONE_MUSIC_UPDATE);
    }
}

void bsp_earphone_manage_init(void)
{
    memset(&earphone, 0, sizeof(earphone_t));
    cm_read(&earphone.tws_addr[0], PARAM_EARPHONE_TWS_ADDR, 6);
    cm_read(&earphone.bt_addr[EARPHONE_LEFT][0], PARAM_EARPHONE_L_ADDR, 6);
    cm_read(&earphone.bt_addr[EARPHONE_RIGHT][0], PARAM_EARPHONE_R_ADDR, 6);

    u8 vbat_read = cm_read8(PARAM_EARPHONE_VBAT_L);
    earphone.vbat[EARPHONE_LEFT] = vbat_read == 100 ? 100 : 0;
    vbat_read = cm_read8(PARAM_EARPHONE_VBAT_R);
    earphone.vbat[EARPHONE_RIGHT] = vbat_read == 100 ? 100 : 0;

    if (cm_read8(PARAM_EARPHONE_PAIRED))
    {
        earphone.paired = true;
    };

#if HALL_EN
    if (!sys_cb.rtc_first_pwron)
    {
        u8 l_inbox_read = cm_read8(PARAM_EARPHONE_INBOX_L);
        earphone.inbox[EARPHONE_LEFT] = l_inbox_read;
        u8 r_inbox_read = cm_read8(PARAM_EARPHONE_INBOX_R);
        earphone.inbox[EARPHONE_RIGHT] = r_inbox_read;
        TRACE("l_inbox:%d,r_inbox:%d\n",l_inbox_read,r_inbox_read);
    }
    else
    {
        TRACE("rtc first pwron not use inbox status\n");
    }
#else
    if (earphone.paired)
    {
        msg_enqueue(EVT_EAR_OUTBOX_START_SCAN_BLE);
    }
    TRACE("%s: paired[%x]\n",__func__, earphone.paired);
#endif
}
#endif
#endif  //SMART_VHOUSE_HOST_EN
