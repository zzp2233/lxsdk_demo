#include "include.h"
#include "lwip/sys.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/dhcp.h"
#include "lwip/sockets.h"
#include "lwip/prot/dhcp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "arch/sys_arch.h"
#include "rt_thread.h"

#define L2CAP_DEFAULT_MTU 672

/* Short name used for netif in lwIP */
#define IFNAME0     'b'
#define IFNAME1     't'

#define log_debug   printf
#define log_info    printf
#define log_error   printf

typedef uint8_t bd_addr_t[6];

#if BT_PANU_EN

static os_mq_t bnep_lwip_outgoing_mbox = NULL;
static struct netif btstack_netif;

// next packet only modified from btstack context
static void *bnep_lwip_outgoing_next_packet;

void lwip_sys_init(void);
void bt_thread_check_trigger(void);
char *bd_addr_to_str(bd_addr_t addr);
void bnep_network_send_packet(const uint8_t *packet, uint16_t size);

static uint16_t bnep_lwip_pbuf_copy_partial(void *pbuf, void *dest, uint16_t dest_size)
{
    struct pbuf *p   = pbuf;
    uint16_t     len = minu(dest_size, p->tot_len);
    pbuf_copy_partial(p, dest, len, 0);
    return len;
}

static void bnep_lwip_free_pbuf(struct pbuf *p)
{
    pbuf_free_callback(p);
}

static void dhcp_status_callback(struct netif *netif)
{
    struct dhcp *dhcp = netif_dhcp_data(netif);
    switch (dhcp->state)
    {
        case DHCP_STATE_OFF:
            printf("DHCP state: off\n");
            break;
        case DHCP_STATE_REQUESTING:
            printf("DHCP state: request\n");
            break;
        case DHCP_STATE_BOUND:
            printf("DHCP state: bound\n");
            printf("IP : %s\n", ipaddr_ntoa(&netif->ip_addr));
            printf("gw : %s\n", ipaddr_ntoa(&netif->gw));
            printf("netmask : %s\n", ipaddr_ntoa(&netif->netmask));
            {
                // ip_addr_t ping_addr;
                // IP_ADDR4(&ping_addr, 180, 76, 76, 76);
                // void ping_init(const ip_addr_t *ping_addr);
                // ping_init(&ping_addr);

                // sys_thread_new("http_thread", http_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
            }
            break;
        // 其他状态可根据需要添加
        default:
            printf("DHCP state: %d\n", dhcp->state);
            break;
    }
}

static void bnep_lwip_outgoing_packet_processed(void)
{
    // free pbuf
    bnep_lwip_free_pbuf(bnep_lwip_outgoing_next_packet);
    // mark as done
    bnep_lwip_outgoing_next_packet = NULL;
}

AT(.com_text.bneq_lwip)
NO_INLINE static void bneq_netif_set_flag(int is_up)
{
    GLOBAL_INT_DISABLE();
    btstack_netif.flags = (btstack_netif.flags & ~NETIF_FLAG_LINK_UP) |
                          (NETIF_FLAG_LINK_UP * is_up);
    GLOBAL_INT_RESTORE();
}

static int bnep_lwip_outgoing_init_queue(void)
{
    bnep_lwip_outgoing_mbox =
        os_mq_create("lb", 4, TCP_SND_QUEUELEN, OS_IPC_FLAG_FIFO);
    if (bnep_lwip_outgoing_mbox == NULL)
    {
        return 1;
    }
    return 0;
}

static void bnep_lwip_outgoing_reset_queue(void)
{
    if (bnep_lwip_outgoing_mbox)
    {
        os_mq_delete(bnep_lwip_outgoing_mbox);
        bnep_lwip_outgoing_mbox = NULL;
    }
}

static void bnep_lwip_outgoing_queue_packet(struct pbuf *p)
{
    u32 addr = (u32)p;
    os_mq_send(bnep_lwip_outgoing_mbox, &addr, 4);
}

static struct pbuf *bnep_lwip_outgoing_pop_packet(void)
{
    if (bnep_lwip_outgoing_mbox == NULL)
    {
        return NULL;
    }
    u32          addr = 0;
    struct pbuf *p    = NULL;
    if (os_mq_recv(bnep_lwip_outgoing_mbox, &addr, 4, 0) != 0)
    {
        return NULL;
    }
    p = (struct pbuf *)addr;
    return p;
}

static int bnep_lwip_outgoing_packets_empty(void)
{
    return (bnep_lwip_outgoing_mbox->entry == 0);
}

static void bnep_lwip_packet_init(void)
{
    bnep_lwip_outgoing_next_packet = NULL;
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    log_info("low_level_output: packet %p, len %u, total len %u\n",
             p,
             p->len,
             p->tot_len);

    // // bnep up?
    // if (bnep_cid == 0)
    //     return ERR_OK;
    // if (!(netif->flags & NETIF_FLAG_LINK_UP))
    //     return ERR_OK;

    // inc refcount
    pbuf_ref(p);

    // queue empty now?
    int queue_empty = bnep_lwip_outgoing_packets_empty();

    // queue up
    bnep_lwip_outgoing_queue_packet(p);

    // trigger processing if queue was empty (might be new packet)
    if (queue_empty)
    {
        bt_thread_check_trigger();
    }

    return ERR_OK;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
static err_t bnep_lwip_netif_init(struct netif *netif)
{
    // interface short name
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

    // mtu
    netif->mtu = L2CAP_DEFAULT_MTU;

    /* device capabilities */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...)
     */
    netif->output = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif
    netif->linkoutput = low_level_output;

    return ERR_OK;
}

static void bnep_lwip_init(void)
{
    log_debug("%s\n", __func__);
    bnep_lwip_outgoing_init_queue();
    //bnep_lwip_outgoing_next_packet = NULL;
    lwip_sys_init();

    ip4_addr_t fsl_netif0_ipaddr, fsl_netif0_netmask, fsl_netif0_gw;
#if 1
    // when using DHCP Client, no address
    IP4_ADDR(&fsl_netif0_ipaddr, 0U, 0U, 0U, 0U);
    IP4_ADDR(&fsl_netif0_netmask, 0U, 0U, 0U, 0U);
#else
    // when playing DHCP Server, set address
    IP4_ADDR(&fsl_netif0_ipaddr, 192U, 168U, 7U, 1U);
    IP4_ADDR(&fsl_netif0_netmask, 255U, 255U, 255U, 0U);
#endif
    IP4_ADDR(&fsl_netif0_gw, 0U, 0U, 0U, 0U);

    // input function differs for sys vs nosys
    netif_input_fn input_function;
    input_function = tcpip_input;

    netif_add(&btstack_netif,
              &fsl_netif0_ipaddr,
              &fsl_netif0_netmask,
              &fsl_netif0_gw,
              NULL,
              bnep_lwip_netif_init,
              input_function);
    netif_set_default(&btstack_netif);

    // IPv6 ULA 地址的设置
    ip6_addr_t ip6addr;
    ip6addr.addr[0] = PP_HTONL(0xfd000000); // ULA 前缀 fd00::
    ip6addr.addr[1] = PP_HTONL(0x00000000); // 中位部分
    ip6addr.addr[2] = PP_HTONL(0x00000000); // 中位部分
    ip6addr.addr[3] = PP_HTONL(0x00000001); // 低位部分

    // 设置 ULA IPv6 地址
    netif_ip6_addr_set(&btstack_netif, 0, &ip6addr);

    // 启用 IPv6 地址
    netif_ip6_addr_set_state(&btstack_netif, 0, IP6_ADDR_VALID);

    // dhcp_start(&btstack_netif);
    netif_set_status_callback(&btstack_netif, dhcp_status_callback);
}

/**
 * @brief Bring up network interfacd
 * @param network_address
 * @return 0 if ok
 */
static int bnep_lwip_netif_up(bd_addr_t network_address)
{
    log_info("bnep_lwip_netif_up start addr %s\n",
             bd_addr_to_str(network_address));

    // set mac address
    btstack_netif.hwaddr_len = 6;
    memcpy(btstack_netif.hwaddr, network_address, 6);

    // link is up
    bneq_netif_set_flag(1);

    // if up
    netif_set_up(&btstack_netif);

    dhcp_start(&btstack_netif);
    return 0;
}

static int bnep_lwip_netif_down(void)
{
    log_info("bnep_lwip_netif_down");

    // link is down
    bneq_netif_set_flag(0);

    netif_set_down(&btstack_netif);
    return 0;
}

AT(.com_text.stack.run_loop)
static void bnep_lwip_discard_packets(void)
{
    // discard current packet
    if (bnep_lwip_outgoing_next_packet)
    {
        bnep_lwip_outgoing_packet_processed();
    }

    // reset queue
    bnep_lwip_outgoing_reset_queue();
}

static void bnep_lwip_netif_process_packet(const uint8_t *packet, uint16_t size)
{
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    // log_debug("%s alloc=%x\n", __func__, p);

    if (!p)
        return;

    /* store packet in pbuf chain */
    struct pbuf *q = p;
    while (q != NULL && size)
    {
        memcpy(q->payload, packet, q->len);
        packet += q->len;
        size -= q->len;
        q = q->next;
    }

    if (size != 0)
    {
        log_error("failed to copy data into pbuf");
        bnep_lwip_free_pbuf(p);
        return;
    }

    /* pass all packets to ethernet_input, which decides what packets it supports */
    int res = btstack_netif.input(p, &btstack_netif);
    if (res != ERR_OK)
    {
        log_error("bnep_lwip_netif_process_packet: IP input error\n");
        bnep_lwip_free_pbuf(p);
        p = NULL;
    }
}

void bnep_network_init(void)
{
    bnep_lwip_packet_init();
    bnep_lwip_init();
}

void bnep_network_up(bd_addr_t addr)
{
    bnep_lwip_netif_up(addr);
}

void bnep_network_down(void)
{
    bnep_lwip_discard_packets();
    bnep_lwip_netif_down();
}

void bnep_network_process_packet(const uint8_t *packet, uint16_t size)
{
    bnep_lwip_netif_process_packet(packet, size);
}

AT(.com_text.stack.run_loop)
void bnep_network_outgoing_process(void)
{
    uint16_t len;

    // previous packet not sent yet
    if (bnep_lwip_outgoing_next_packet)
    {
        return;
    }

    bnep_lwip_outgoing_next_packet = bnep_lwip_outgoing_pop_packet();
    if (bnep_lwip_outgoing_next_packet == NULL)
    {
        return;
    }

    void *buf = mem_malloc(L2CAP_DEFAULT_MTU);
    if (buf == NULL)
    {
        log_error("%s malloc failed!\n");
        return;
    }

    // flatten into our buffer
    len = bnep_lwip_pbuf_copy_partial(bnep_lwip_outgoing_next_packet,
                                      buf,
                                      L2CAP_DEFAULT_MTU);
    // request can send now
    bnep_network_send_packet(buf, len);
}

void bnep_network_packet_sent(uint8_t *buf)
{
    log_debug("bnep_lwip_packet_sent: %p\n", bnep_lwip_outgoing_next_packet);

    mem_free(buf);

    // release current packet
    bnep_lwip_outgoing_packet_processed();

    // more ?
    if (bnep_lwip_outgoing_packets_empty())
    {
        return;
    }
    bt_thread_check_trigger();
}

#endif
