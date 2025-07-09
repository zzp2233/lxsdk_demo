#include "include.h"

#if SMART_VHOUSE_HOST_EN

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#define VHOUSE_CMD_QUEUE_SIZE       20          //队列指令最大数量
#define VHOUSE_CMD_WITH_BUF_LEN     40          //带buf的指令数据缓冲区长度

#define bsp_smart_vhouse_is_busy()              (vhouse_cmd_cb.send_tick ? 1 : 0)

typedef struct
{
    u8 cmd;
    u8 channel;
} vhouse_cmd_queue_item_t;

typedef struct
{
    array_queue_t cmd;                //命令队列
    array_queue_t channel;            //命令的通道队列
    u8 cmd_array[VHOUSE_CMD_QUEUE_SIZE];
    u8 channel_array[VHOUSE_CMD_QUEUE_SIZE];

    u8 with_buf_cmd;                  //带buf的指令, 标志
    u8 buf[VHOUSE_CMD_WITH_BUF_LEN];  //带buf的指令, 数据缓冲区, 队列取命令时读取
    u8 buf_len;                       //带buf的指令, 数据缓冲区长度
} vhouse_cmd_queue_t;

typedef struct
{
    u32 send_tick;
    u32 recv_tick;
    u8  recv_index;
    u8 vbat_timeout_cnt[2];

    u8 last_tx_cmd;
    u8 last_tx_channel;
} vhouse_cmd_cb_t;

static vhouse_cmd_queue_t vhouse_cmd_queue;
static vhouse_cmd_cb_t vhouse_cmd_cb;
#if !HALL_EN
extern void bsp_box_set_comm_timeout_flg(u8 flg);
#endif

AT(.text.func.idle.queue)
void vhouse_queue_init(void)
{
    mod_queue_creat(&vhouse_cmd_queue.cmd);
    mod_queue_creat(&vhouse_cmd_queue.channel);
}

int vhouse_queue_size(void)
{
    return mod_queue_size(&vhouse_cmd_queue.cmd);
}

AT(.text.func.idle.queue)
void vhouse_cmd_enqueue(vhouse_cmd_queue_item_t *item)
{
//    printf("%s %d %d\n", __func__, vhouse_cmd_queue.cmd.front, vhouse_cmd_queue.cmd.rear);
//    printf("%s %d %d\n", __func__, item->cmd, item->channel);
    mod_enqueue(&vhouse_cmd_queue.cmd, item->cmd);
    mod_enqueue(&vhouse_cmd_queue.channel, item->channel);
}

void vhouse_cmd_dequeue(vhouse_cmd_queue_item_t *item)
{
//    printf("%s %d %d\n", __func__, vhouse_cmd_queue.cmd.front, vhouse_cmd_queue.cmd.rear);
    item->cmd = mod_dequeue(&vhouse_cmd_queue.cmd);
    item->channel = mod_dequeue(&vhouse_cmd_queue.channel);
}

bool vhouse_cmd_inqueue(u8 cmd)
{
    return mod_inqueue(&vhouse_cmd_queue.cmd, cmd);
}

bool vhouse_cmd_is_inqueue(u8 cmd, u8 channel)
{
//    print_r(vhouse_cmd_queue.cmd_array, VHOUSE_CMD_QUEUE_SIZE);
//    print_r(vhouse_cmd_queue.channel_array, VHOUSE_CMD_QUEUE_SIZE);

    for (u8 i = 0; i < VHOUSE_CMD_QUEUE_SIZE; i++)
    {
        if (vhouse_cmd_queue.cmd_array[i] == cmd && vhouse_cmd_queue.channel_array[i] == channel)
        {
            if (mod_queue_idx_is_valid(&vhouse_cmd_queue.cmd, i) && mod_queue_idx_is_valid(&vhouse_cmd_queue.channel, i))
            {
                TRACE("repeat: cmd[%x] ch[%x] %d\n", cmd, channel, i);
                return true;
            }
        }
    }

    return false;
}

u8 bsp_smart_vhouse_send(u8 cmd, u8 *buf, u8 len)
{
    while (!tick_check_expire(uart1_cb.ticks, 5))   //接收数据后等待一段时间再发送
    {
        WDT_CLR();
    }

#if CHARGE_EX_IC_SELECT
    if (bsp_charge_ex_mode() != BOX_COMM_MODE || (!bsp_charge_ex_comm_allow()))  //非通讯模式进行串口收发会死机
    {
        printf("[%d %d %d]not comm, disallow tx!\n", bsp_charge_ex_mode(), bsp_charge_ex_is_waking_up(), charge_ex_cb.comm_delay);
        return 1;
    }
#endif

    if(len > VH_DATA_LEN)
    {
        return 1;
    }

    if (!smart_vhouse_tx_is_en())
    {
        printf("not en to tx\n");
        return 1;
    }

    vh_packet_t vh_packet_cb;
    vh_packet_cb.header = SMART_BOX_HEADER;
    vh_packet_cb.distinguish = SMART_BOX_DISTINGUISH;
    vh_packet_cb.cmd = cmd;
    vh_packet_cb.length = len;

    memset(vh_packet_cb.buf, 0, VH_DATA_LEN);
    vh_packet_cb.crc = 0;

    u8 *p = buf;
    for(u8 i = 0; i < len; i++)
    {
        vh_packet_cb.buf[i] = *p;
        p++;
    }

    p = (u8*)&vh_packet_cb.header;

    smart_vhouse_rx_dis();                                  //禁用RX, 避免串扰
    for(u8 i = 0; i < len + SMART_BOX_HEAD_LEN; i++)
    {
        vh_packet_cb.crc = crc8_tbl[vh_packet_cb.crc ^ (*p)];
        bsp_charge_vhouse_uart_send(*p);
        p++;
    }

    bsp_charge_vhouse_uart_send(vh_packet_cb.crc);

    while(!(UART1CON & BIT(8)));
    smart_vhouse_rx_en();                                   //使能RX, 发送完成
    vhouse_cmd_cb.recv_tick = tick_get();
    bsp_smart_vhouse_uart_set_recv_ear_respon();

    return 0;
}

#if CHARGEBOX_OTA_EN
AT(.com_text.charge_box)
u8 chargebox_ota_smart_vhouse_send(u8 *buf, u8 len)
{
    while (!tick_check_expire(uart1_cb.ticks, 5))
    {
        WDT_CLR();
    }

//    if(!sys_cb.hall_status){
//        printf("sys_cb.hall_status %d\n",sys_cb.hall_status);
//        return 1;
//    }

    u8 crc = 0;
    u8 *p = buf;
    smart_vhouse_rx_dis();                                  //禁用RX, 避免串扰
    GLOBAL_INT_DISABLE();
    for(u8 i = 0; i < len; i++)
    {
        crc =  crc8_tbl[crc ^ (*p)];
        // bsp_charge_vhouse_uart_send(*p);
        uart1_putchar(*p);
        p++;
        if(i == len -1)
        {
            // bsp_charge_vhouse_uart_send(crc);
            uart1_putchar(crc);

        }
    }
    GLOBAL_INT_RESTORE();
    smart_vhouse_rx_en();                                   //使能RX, 发送完成
//    printf("box_send[%d],crc[%x]: ", len, crc);
//    print_r(buf,len);
    return 0;
}

#endif

void bsp_smart_vhouse_cmd_sned(u8 cmd, u8 channel, u8 *send_buf, u8 len)
{
    u8 buf[VH_DATA_LEN];

#if TRACE_EN
    if (cmd != VHOUSE_CMD_GET_VBAT)
    {
        TRACE("send: cmd[%x] channel[%x] tick[%d]\n", cmd, channel, tick_get());
    }
#endif
    vhouse_cmd_cb.send_tick = tick_get();

    switch (cmd)
    {
        case VHOUSE_CMD_CLOSE_WIN_GET_VBAT:
        case VHOUSE_CMD_GET_VBAT:
//        printf("VHOUSE_CMD_GET_VBAT L[%x] R[%x]\n", earphone.vbat[EARPHONE_LEFT], earphone.vbat[EARPHONE_RIGHT]);
            buf[0] = channel;
            buf[1] = (u8)((hfp_get_bat_level_ex() & 0x7f) | (charge_ex_cb.chg << 7));
            buf[2] = (channel == LEFT_CHANNEL_USER) ? earphone.vbat[EARPHONE_RIGHT] : earphone.vbat[EARPHONE_LEFT]; //发送对耳声道的电量, 用于仓内互传
            bsp_smart_vhouse_send(cmd, buf, 3);
            break;

        case VHOUSE_CMD_PAIR:
            buf[0] = channel;
            printf("===> VHOUSE_CMD_PAIR\n");
#if LE_ADV0_EN
            ble_get_local_bd_addr(&buf[1]);
            bsp_smart_vhouse_send(cmd, buf, 7);
#else
            bsp_smart_vhouse_send(cmd, buf, 1);
#endif
            break;

        case VHOUSE_CMD_CLEAR_PAIR:
            buf[0] = channel;
            bsp_smart_vhouse_send(cmd, buf, 1);
            break;

        case VHOUSE_CMD_GET_TWS_BTADDR:
            TRACE("VHOUSE_CMD_GET_TWS_BTADDR\n");
            bsp_smart_vhouse_send(cmd, send_buf, len);
            break;

        case VHOUSE_CMD_CLOSE_WINDOW:
        case VHOUSE_CMD_OPEN_WINDOW:
            TRACE("VHOUSE_CMD_xxx_WINDOW\n");
            buf[0] = channel;
            buf[1] = (u8)((hfp_get_bat_level_ex() & 0x7f) | (charge_ex_cb.chg << 7));
            buf[2] = (channel == LEFT_CHANNEL_USER) ? earphone.vbat[EARPHONE_LEFT] : earphone.vbat[EARPHONE_RIGHT];
            bsp_smart_vhouse_send(cmd, buf, 3);
            break;

        case VHOUSE_CMD_PWROFF:
            buf[0] = channel;
            printf("===> VHOUSE_CMD_PWROFF[%x]\n", channel);
            bsp_smart_vhouse_send(cmd, buf, 1);
            break;

        case VHOUSE_CMD_GET_EAR_ADDR:
            TRACE("VHOUSE_CMD_GET_EAR_ADDR\n");
            buf[0] = channel;
#if LE_ADV0_EN
            ble_get_local_bd_addr(&buf[1]);
            bsp_smart_vhouse_send(cmd, buf, 7);
#else
            bsp_smart_vhouse_send(cmd, buf, 1);
#endif
            break;

        default:
            break;
    }
}

AT(.text.func.idle.queue)
bool bsp_smart_vhouse_cmd_enqueue_no_repeat(u8 cmd, u8 channel)
{
    if (vhouse_cmd_is_inqueue(cmd, channel))
    {
        return false;
    }

//    TRACE("%s: cmd[%x] channel[%x]\n", __func__, cmd, channel);
    bsp_smart_vhouse_cmd_enqueue(cmd, channel);
    return true;
}

AT(.text.func.idle.queue)
void bsp_smart_vhouse_cmd_enqueue(u8 cmd, u8 channel)
{
    vhouse_cmd_queue_item_t item;
    item.cmd = cmd;
    item.channel = channel;

    bsp_charge_ex_comm_backup();
    vhouse_cmd_enqueue(&item);
}

u8 bsp_smart_vhouse_cmd_buf_enqueue(u8 cmd, u8 channel, u8 *buf, u8 len)
{
    if (vhouse_cmd_queue.with_buf_cmd)  //目前最大只支持一条
    {
        printf("cmd with buf overflow!\n");
        return 1;
    }

    if (len > VHOUSE_CMD_WITH_BUF_LEN)
    {
        printf("cmd with buf len overflow!\n");
        return 2;
    }

    vhouse_cmd_queue.with_buf_cmd = cmd;
    vhouse_cmd_queue.buf_len = len;
    memcpy(vhouse_cmd_queue.buf, buf, len);

    bsp_smart_vhouse_cmd_enqueue(cmd, channel);
    return 0;
}

AT(.text.func.idle.vhouse)
void bsp_smart_vhouse_cmd_timeout_clr(void)
{
//    printf("%s\n", __func__);
    vhouse_cmd_cb.send_tick = 0;
    vhouse_cmd_cb.recv_tick = 0;
}

AT(.text.func.idle.vhouse)
void bsp_smart_vhouse_cmd_reset(void)
{
//    printf("%s\n", __func__);
    bsp_smart_vhouse_cmd_timeout_clr();
    vhouse_cmd_queue.with_buf_cmd = 0;
    vhouse_cmd_cb.vbat_timeout_cnt[0] = 0;
    vhouse_cmd_cb.vbat_timeout_cnt[1] = 0;
    charge_ex_cb.mode_backup = 0;
    vhouse_queue_init();
}

void bsp_smart_vhouse_cmd_ack(u8 channel)
{
//    printf("%s\n", __func__);
    vhouse_cmd_cb.vbat_timeout_cnt[channel] = 0;

    bsp_smart_vhouse_cmd_timeout_clr();
    bsp_earphone_inbox(channel, true);
}

void bsp_smart_vhouse_cmd_timeout(u8 channel)
{
    u8 tout_cnt = BREATH_TIMEOUT_CNT;
    vhouse_cmd_cb.vbat_timeout_cnt[channel]++;

    if (vhouse_cmd_cb.vbat_timeout_cnt[channel] >= tout_cnt)
    {
        TRACE("cmd timeout [%x]\n", channel);
#if HALL_EN
        if (vhouse_cmd_cb.last_tx_cmd == VHOUSE_CMD_GET_VBAT)
        {
            bsp_earphone_inbox(channel, false);
        }
        else if (hall_cb.charge_start_tick)     //充电中，通信超时3次判断为满电，不出仓
        {
            if (bsp_earphone_is_inbox(channel) && !CHARGE_EX_BAT_IS_LOW())
            {
                printf("timeout-->bat full: %d\n", channel);
                earphone.charge_sta[channel] = EARPHONE_FULL_CHARGE;
                earphone.vbat[channel] = 100;
            }
        }
#else
        //亮屏无耳机在仓内
        if (channel == 0)
        {
            bsp_box_set_comm_timeout_flg(LEFT_CHANNEL_USER);
        }
        else
        {
            bsp_box_set_comm_timeout_flg(RIGHT_CHANNEL_USER);
        }
        bsp_earphone_inbox(channel, false);
#endif
    }
}

AT(.text.func.idle)
void bsp_smart_vhouse_cmd_ack_timeout(void)
{
    u16 timeout_tick = CMD_TIMEOUT_TICK;
    u8 recv_data = 0;
    bool comm_recover_check = false;            //是否需要通讯模式还原检查

    if (vhouse_cmd_cb.send_tick && tick_check_expire(vhouse_cmd_cb.send_tick, timeout_tick))
    {
        vhouse_cmd_cb.send_tick = 0;
        if (vhouse_cmd_cb.last_tx_cmd == VHOUSE_CMD_GET_VBAT ||
            vhouse_cmd_cb.last_tx_cmd == VHOUSE_CMD_CLOSE_WIN_GET_VBAT ||
            vhouse_cmd_cb.last_tx_cmd == VHOUSE_CMD_PWROFF)
        {
            if (vhouse_cmd_cb.last_tx_channel == LEFT_CHANNEL_USER)
            {
                bsp_smart_vhouse_cmd_timeout(EARPHONE_LEFT);
            }
            else if (vhouse_cmd_cb.last_tx_channel == RIGHT_CHANNEL_USER)
            {
                bsp_smart_vhouse_cmd_timeout(EARPHONE_RIGHT);
            }
        }

        comm_recover_check = true;
//        printf("tx timeout[%d %d %d %d]\n",
//               vhouse_cmd_cb.last_tx_cmd, vhouse_cmd_cb.last_tx_cmd, vhouse_queue_size(), mod_queue_is_empty(&vhouse_cmd_queue.cmd));
    }

    //耳机繁忙仅回复单字节数据处理
    if (vhouse_cmd_cb.recv_tick && tick_check_expire(vhouse_cmd_cb.recv_tick, 12))
    {
        if (bsp_smart_vhouse_uart_is_finish_get_data())
        {
            recv_data = bsp_smart_vhouse_uart_get_buf_data();
            if (recv_data == LEFT_CHANNEL_USER)
            {
                bsp_smart_vhouse_cmd_ack(EARPHONE_LEFT);
                comm_recover_check = true;
            }
            if (recv_data == RIGHT_CHANNEL_USER)
            {
                bsp_smart_vhouse_cmd_ack(EARPHONE_RIGHT);
                comm_recover_check = true;
            }
//            printf("tx ack:%x\n", recv_data);
        }

        bsp_smart_vhouse_uart_recv_ear_clear();
        vhouse_cmd_cb.recv_tick = 0;
    }

    if (comm_recover_check && !vhouse_queue_size())
    {
        bsp_charge_ex_comm_recover();
    }
}

void bsp_smart_vhouse_vbat_get(void)
{
    if (func_cb.sta == FUNC_IDLE)
    {
        return;
    }

    if (CHARGE_EX_COMM_IS_BACKUP())
    {
        return;
    }

    if (bsp_hall_open_box() && bsp_charge_ex_mode() == BOX_COMM_MODE && !bsp_smart_vhouse_is_busy())   //亮屏时通讯模式心跳包
    {
        bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_GET_VBAT, LEFT_CHANNEL_USER);
        bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_GET_VBAT, RIGHT_CHANNEL_USER);
    }

    return;
}

void bsp_smart_vhouse_charge_vbat_get(void)
{
#if HALL_EN
    if (bsp_hall_open_box())
    {
        return;
    }
#endif

    bool l_need_pwroff = false;
    bool r_need_pwroff = false;

    //低电
    if (CHARGE_EX_BAT_IS_LOW())
    {
        l_need_pwroff = true;
        r_need_pwroff = true;
    }

    //耳机满电
    if (bsp_earphone_vbat_is_full(EARPHONE_LEFT))
    {
        l_need_pwroff = true;
    }

    if (bsp_earphone_vbat_is_full(EARPHONE_RIGHT))
    {
        r_need_pwroff = true;
    }

    TRACE("%s: need_pwroff[%d %d]\n", __func__, l_need_pwroff, r_need_pwroff);

    if (l_need_pwroff)
    {
        bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_PWROFF, LEFT_CHANNEL_USER);
    }
    else
    {
        bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_CLOSE_WIN_GET_VBAT, LEFT_CHANNEL_USER);
    }

    if (r_need_pwroff)
    {
        bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_PWROFF, RIGHT_CHANNEL_USER);
    }
    else
    {
        bsp_smart_vhouse_cmd_enqueue_no_repeat(VHOUSE_CMD_CLOSE_WIN_GET_VBAT, RIGHT_CHANNEL_USER);
    }
}

AT(.text.func.idle)
static u8 bsp_smart_vhouse_cmd_allow_check(void)
{
    if (mod_queue_is_empty(&vhouse_cmd_queue.cmd))
    {
        return 1;
    }

    if (vhouse_cmd_cb.send_tick)
    {
        return 2;
    }

#if CHARGE_EX_IC_SELECT
    if (!bsp_charge_ex_comm_allow())
    {
        if(vhouse_cmd_cb.send_tick)
        {
            vhouse_cmd_cb.send_tick = 0;
        }
//        printf("cmd is not allow to send\n");
        return 3;
    }
#endif

    return 0;
}

AT(.text.func.idle)
void bsp_smart_vhouse_queue_process(void)
{
    u8 ret = bsp_smart_vhouse_cmd_allow_check();

//    static u8 ret_last = 0;
//    if (ret_last != ret && ret != 0) {
//        ret_last = ret;
//        printf("cmd_disallow[%d]\n", ret);
//    }

    if (ret)
    {
        return;
    }

    //limit cmd tx by tick
    static u32 cmd_tx_tick = 0;
    if (!tick_check_expire(cmd_tx_tick, CMD_TX_PR))
    {
//        printf("limit cmd tx by tick\n");
        return;
    }
    cmd_tx_tick = tick_get();

    u8 cmd, channel;
    vhouse_cmd_queue_item_t item;

    vhouse_cmd_dequeue(&item);
    cmd = item.cmd;
    channel = item.channel;

    if (vhouse_cmd_queue.with_buf_cmd == cmd)       //带buf的指令
    {
        vhouse_cmd_queue.with_buf_cmd = 0;
        bsp_smart_vhouse_cmd_sned(cmd, channel, vhouse_cmd_queue.buf, vhouse_cmd_queue.buf_len);
    }
    else
    {
        bsp_smart_vhouse_cmd_sned(cmd, channel, NULL, 0);
    }

    vhouse_cmd_cb.last_tx_cmd = cmd;
    vhouse_cmd_cb.last_tx_channel = channel;
}

//------------------------------------------------

AT(.text.func.idle)
void bsp_smart_vhouse_host_cmd_process(void)
{
    static u32 ticks = 0;
    static u16 cnt = 0;
    u16 check_pr = 60000/BREATH_TX_PR; //60s

    if (tick_check_expire(ticks, BREATH_TX_PR))
    {
        ticks = tick_get();
        cnt++;

        bsp_smart_vhouse_vbat_get();
#if !HALL_EN
        if (func_cb.sta != FUNC_IDLE)       //亮屏加快查询间隔
        {
            check_pr = 3000/BREATH_TX_PR;
        }
#endif
        if ((cnt % check_pr == 0))          //间隔检查耳机电量(不唤醒充电), 满电发关机码
        {
            bsp_smart_vhouse_charge_vbat_get();
        }
    }

    bsp_smart_vhouse_queue_process();
    bsp_smart_vhouse_cmd_ack_timeout();
}

void bsp_smart_vhouse_host_cmd_init(void)
{
    printf("%s\n", __func__);

    memset(&vhouse_cmd_queue, 0, sizeof(vhouse_cmd_queue_t));

    vhouse_cmd_queue.cmd.array = vhouse_cmd_queue.cmd_array;
    vhouse_cmd_queue.cmd.capacity = VHOUSE_CMD_QUEUE_SIZE;
    vhouse_cmd_queue.channel.array = vhouse_cmd_queue.channel_array;
    vhouse_cmd_queue.channel.capacity = VHOUSE_CMD_QUEUE_SIZE;

    vhouse_queue_init();
}

#else
void bsp_smart_vhouse_cmd_sned(u8 cmd, u8 channel, u8 *send_buf, u8 len) {}
AT(.text.func.idle.vhouse)
void bsp_smart_vhouse_cmd_reset(void) {}
AT(.text.func.idle.queue)
bool bsp_smart_vhouse_cmd_enqueue_no_repeat(u8 cmd, u8 channel)
{
    return false;
}
AT(.text.func.idle.queue)
void bsp_smart_vhouse_cmd_enqueue(u8 cmd, u8 channel) {}
void bsp_smart_vhouse_charge_vbat_get(void) {}
#endif // SMART_VHOUSE_HOST_EN
