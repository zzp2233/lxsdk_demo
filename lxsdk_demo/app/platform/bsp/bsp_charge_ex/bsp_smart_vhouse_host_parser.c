#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#if SMART_VHOUSE_HOST_EN
vh_packet_t vh_send_packet;

AT(.com_text.bsp.uart) //TODO
u8 bsp_smart_vhouse_packet_parse(vh_packet_t *p, u8 data)
{
    u8 recv_valid  = 0, parse_done = 0;
    u8 cnt = p->cnt;

    if ((cnt == 0) && (data == 0x55))
    {
        recv_valid = 1;
    }
    else if ((cnt == 1) && (data == 0xAA))
    {
        p->header = 0xAA55;
        recv_valid = 1;
    }
    else if ((cnt == 2) && (data == VHOUSE_DISTINGUISH))
    {
        p->distinguish = data;
        recv_valid = 1;
    }
    else if (cnt == 3)
    {
        p->cmd = data;
        recv_valid = 1;
    }
    else if (cnt == 4)
    {
        if (data <= VH_DATA_LEN)
        {
            p->length = data;
            recv_valid = 1;
        }
    }
    else if ((cnt > 4) && (cnt <= (p->length + 1 + 4)))
    {
        recv_valid = 1;
        if (cnt > p->length + 4)
        {
            p->checksum = data;
            parse_done = 1;                                     //匹配完整充电仓命令包
            recv_valid = 0;
        }
        else
        {
            p->buf[cnt - 5] = data;
        }
    }

    if (recv_valid)
    {
        p->crc = crc8_tbl[p->crc ^ data];
        p->cnt++;
    }
    else
    {
        if ((parse_done) && (p->crc != p->checksum))
        {
            parse_done = 0;
        }
        p->cnt = 0;
        p->crc = 0;
    }

    return parse_done;
}

void bsp_smart_vhouse_get_vbat(vh_packet_t *packet, u8 channel)
{
    u8 index = bsp_earphone_get_index_bych(channel);

    if (index == EARPHONE_INDEX_ERR)
    {
        return;
    }

    bsp_earphone_set_vbat(index, packet->buf[2]);

    if (packet->length > 3)     //有上报充电状态
    {
        if (packet->buf[3] >= EARPHONE_CHARGE_MAX)
        {
            printf("err: earphone chg sta\n");
            return;
        }
        earphone.charge_sta[index] = packet->buf[3];

        //    //耳机发100%即认为充满, 不等充电截止
        //    if ((packet->buf[2] & 0x7f) == 100) {
        //        earphone.charge_sta[index] = EARPHONE_FULL_CHARGE;
        //    }

        //耳机充电截止
        if (packet->buf[3] == EARPHONE_FULL_CHARGE)
        {
            earphone.vbat[index] = 100;
        }
    }

    TRACE("vbat[%x]:%d%% sta[%x]\n", channel, packet->buf[2] & 0x7f, earphone.charge_sta[index], length);
}

static void bsp_smart_vhouse_cmd_parse(vh_packet_t *packet)
{
    static u8 clear_cnt = 0;

#if TRACE_EN
    if (packet->cmd != VHOUSE_CMD_GET_VBAT)
    {
        TRACE("recv: cmd[%x] channel[%x]\n", packet->cmd, packet->buf[0]);
//        TRACE_R(packet->buf, packet->length);
    }
#endif
    switch (packet->cmd)
    {
        case VHOUSE_CMD_CLOSE_WIN_GET_VBAT:
        case VHOUSE_CMD_GET_VBAT:
            bsp_smart_vhouse_get_vbat(packet, packet->buf[0]);
            msg_enqueue(EVT_EARPHONE_BAT_UPDATE);
            break;

        case VHOUSE_CMD_GET_TWS_BTADDR:
//            bsp_smart_vhouse_cmd_sned(packet->cmd, packet->buf[0], packet->buf, packet->length);//会冲突
            bsp_smart_vhouse_cmd_buf_enqueue(packet->cmd, packet->buf[0], packet->buf, packet->length);

            if (packet->buf[0] == LEFT_CHANNEL_USER)
            {
                bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PAIR, LEFT_CHANNEL_USER);
                printf("R_ear ADDR:");
                print_r(&packet->buf[7], 6);
                memcpy(earphone.bt_addr[EARPHONE_RIGHT], &packet->buf[7], 6);
                msg_enqueue(EVT_CMD_PAIR_ACK);
            }
            else if (packet->buf[0] == RIGHT_CHANNEL_USER)
            {
                printf("L_ear ADDR:");
                print_r(&packet->buf[7], 6);
                memcpy(earphone.bt_addr[EARPHONE_LEFT], &packet->buf[7], 6);
                bsp_earphone_save_addr(&packet->buf[1], EARPHONE_TWS_ADDR_LEN);
            }
            break;

        case VHOUSE_CMD_CLEAR_PAIR:
            if (packet->buf[0] == RIGHT_CHANNEL_USER)
            {
                if (bsp_earphone_is_inbox(EARPHONE_LEFT))
                {
                    bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_CLEAR_PAIR, LEFT_CHANNEL_USER);
                }
            }
            clear_cnt++;
            if(clear_cnt >= 2)
            {
                printf("VHOUSE_CMD_CLEAR_PAIR\n");
                clear_cnt = 0;
                bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PAIR, RIGHT_CHANNEL_USER);
            }
            break;

        case VHOUSE_CMD_SUCCESS:
//            bsp_charge_ex_mode_set(BOX_CHK_MODE);    //修复TWS组队后仓保持在通讯模式的问题
//            bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_GET_EAR_ADDR, RIGHT_CHANNEL_USER);
            break;

        case VHOUSE_CMD_CLOSE_WINDOW:
            bsp_earphone_set_vbat(bsp_earphone_get_index_bych(packet->buf[0]), packet->buf[2]);
            msg_enqueue(EVT_EARPHONE_BAT_UPDATE);
            break;

        case VHOUSE_CMD_OPEN_WINDOW:
            bsp_earphone_set_vbat(bsp_earphone_get_index_bych(packet->buf[0]), packet->buf[2]);
            msg_enqueue(EVT_EARPHONE_BAT_UPDATE);
            break;

        case VHOUSE_CMD_GET_EAR_ADDR:
        {
            bool need_pair = false;
            u16 link_key_sum = ((packet->buf[15] << 8) | packet->buf[14]);

            //单耳开盖保存单边tws addr || 无tws addr
            if ((!bsp_earphone_all_inbox() && bsp_hall_open_box()) || array_is_null(&earphone.tws_addr[0], EARPHONE_TWS_ADDR_LEN))
            {
                bsp_earphone_save_addr(&packet->buf[1], EARPHONE_TWS_ADDR_LEN);
            }

            if (packet->buf[0] == RIGHT_CHANNEL_USER)
            {
                printf("*R_ear ADDR new:");
                print_r(&packet->buf[7], 6);
                printf("*R_ear ADDR org:");
                print_r(&earphone.bt_addr[EARPHONE_RIGHT][0], 6);
                printf("*R_Link_key_sum:%x\n", link_key_sum);

                if (bsp_earphone_all_inbox())  //双耳入仓关盖发配对码
                {
                    if (!array_is_equal(&earphone.bt_addr[EARPHONE_RIGHT][0], &packet->buf[7], 6))
                    {
                        need_pair = true;
                    }

                    if (packet->length > 14)  //带配对信息校验和
                    {
                        if (earphone.link_key_sum[EARPHONE_LEFT] != link_key_sum ||
                            (earphone.link_key_sum[EARPHONE_LEFT] == 0) || (link_key_sum == 0))
                        {
                            need_pair = true;
                        }
                    }
                }
                memcpy(earphone.bt_addr[EARPHONE_RIGHT], &packet->buf[7], 6);
                earphone.link_key_sum[EARPHONE_RIGHT] = link_key_sum;
            }
            else if (packet->buf[0] == LEFT_CHANNEL_USER)
            {
                printf("*L_ear ADDR new:");
                print_r(&packet->buf[7], 6);
                printf("*L_ear ADDR org:");
                print_r(&earphone.bt_addr[EARPHONE_LEFT][0], 6);
                printf("*L_Link_key_sum:%x\n", link_key_sum);

                if (bsp_earphone_all_inbox())
                {
                    if (!array_is_equal(&earphone.bt_addr[EARPHONE_LEFT][0], &packet->buf[7], 6))
                    {
                        need_pair = true;
                    }
                }
                memcpy(earphone.bt_addr[EARPHONE_LEFT], &packet->buf[7], 6);
                earphone.link_key_sum[EARPHONE_LEFT] = link_key_sum;
            }

            if (need_pair && !vhouse_cmd_inqueue(VHOUSE_CMD_PAIR))
            {
                printf("Not a pair\n");
                bsp_smart_vhouse_cmd_enqueue(VHOUSE_CMD_PAIR, RIGHT_CHANNEL_USER);
            }
            msg_enqueue(EVT_CMD_GET_EAR_ADDR_ACK);
            break;
        }

#if CHARGEBOX_OTA_EN
        case VHOUSE_CMD_OTA_EAR:
            chargebox_ota_recv_ear_data(packet);
            break;
#endif

        default:
            break;
    }

    if (packet->buf[0] == LEFT_CHANNEL_USER)
    {
        bsp_smart_vhouse_cmd_ack(EARPHONE_LEFT);
    }
    else if (packet->buf[0] == RIGHT_CHANNEL_USER)
    {
        bsp_smart_vhouse_cmd_ack(EARPHONE_RIGHT);
    }

    if (vhouse_queue_size())
    {
        bsp_smart_vhouse_queue_process();
    }
    else
    {
        bsp_charge_ex_comm_recover();
    }
}

AT(.text.func.idle)
void bsp_smart_vhouse_host_parser_process(void)
{
    u8 ch;
    u8 parse_done;
    vh_packet_t *packet = &vh_send_packet;

    while (bsp_smart_vhouse_uart_get(&ch))
    {
        WDT_CLR();
        parse_done = bsp_smart_vhouse_packet_parse(packet, ch);

        if ((parse_done) && (packet->checksum == mod_crc8_maxim((u8 *)packet, 5 + packet->length)))
        {
            bsp_smart_vhouse_cmd_parse(packet);
        }
//        printf("ch[%x] parse_done[%d]\n", ch, parse_done);
    }
}

#endif  //SMART_VHOUSE_HOST_EN
