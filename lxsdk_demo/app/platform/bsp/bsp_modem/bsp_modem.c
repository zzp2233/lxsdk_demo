#include "include.h"
#include "api.h"
#include "api_modem.h"
#include "api_iis.h"

void mp3_res_play_kick(u32 addr, u32 len);

void modem_call_init(void);
void modem_call_exit(void);
void sco_buffer_reset(void);

int contacts_kvdb_init(void);
int contacts_kvdb_set(const char *name, const char *number);
int contacts_kvdb_get_number(const char *name, char *number, u16 max_num_len);
u32 contacts_kvdb_get_list(u32 flash_addr, u32 max_len);

int  bsp_modem_get_contains(u16 from, u16 to);
int  bsp_modem_update_contains(void);
void bsp_modem_sta_init(void);

void bsp_modem_call_init(void);
void bsp_modem_call_check_ok(bool is_ok);
void modem_call_notice_state(void *param);
void modem_call_notice_ring(void *param);
void modem_call_notice_current_call(void *param);

bsp_modem_t modem_cb;

#if MODEM_CAT1_EN
static u8 init_flag = 0;

volatile bool modem_bypass_flag = true;

bool cat1_is_bypass(void)
{
    return modem_bypass_flag;
}

// static u8 state = 0;

// void modem_test_machine(void)
// {
//     printf("%s %d\n", __func__, state);
//     switch (state) {
//     case 0:
//         modem_net_read_plmn_select();
//         break;
//     case 1:
//         modem_net_read_eps_regist();
//         break;
//     case 2:
//         modem_mtcas_req_sq();
//         break;
//     case 3:
//         modem_general_req_psn_id(TE_PSN_IMEI);
//         break;
//     }
//     state++;
// }

void bsp_modem_init(void)
{
    if (!init_flag)
    {
        init_flag         = 1;
        modem_bypass_flag = true;
        printf("%s\n", __func__);

        modem_platform_init();
        GPIOFFEN &= ~(BIT(3));
        GPIOFDE |= (BIT(3));
        GPIOFDIR &= ~(BIT(3));
        GPIOF |= (BIT(3));

        modem_cb.spr = SPR_16000;
        bsp_modem_call_init();
        bsp_modem_sta_init();
        modem_platform_init_end();
        printf("init end\n");

        // TODO
        // SENDER_STA_UP();
#if 0
        bsp_modem_update_contains();
        contacts_kvdb_init();
        modem_at_test();
        delay_ms(10);
        bsp_modem_update_contains();
#endif
    }
}

void bsp_modem_exit(void)
{
    if (init_flag)
    {
        init_flag = 0;
        SENDER_STA_DOWN();
        modem_platform_exit();
        GPIOFFEN &= ~(BIT(3));
        GPIOFDE |= (BIT(3));
        GPIOFDIR &= ~(BIT(3));
        GPIOFCLR = (BIT(3));
    }
}

int bsp_modem_get_init_flag(void)
{
    return init_flag;
}

u8 bsp_modem_get_spr(void)
{
    return modem_cb.spr;
}

void modem_call_notice(uint evt, void *param, u8 type)
{
    // print_r(param, 24);

    switch (evt)
    {
        case MODEM_CALL_NOTICE_INFO:
            break;
        case MODEM_CALL_NOTICE_STATE:
            modem_call_notice_state(param);
            break;
        case MODEM_CALL_NOTICE_START:
            break;
        case MODEM_CALL_NOTICE_NET_CONN:
            break;
        case MODEM_CALL_NOTICE_CALL_CONN:
            break;
        case MODEM_CALL_NOTICE_END:
            break;
        case MODEM_CALL_NOTICE_RING:
            modem_call_notice_ring(param);
            break;
        case MODEM_CALL_NOTICE_CURRENT_CALL:
            modem_call_notice_current_call(param);
            break;
    }
}

void modem_sms_notice(uint evt, void *param, u8 type)
{
    switch (evt)
    {
        case MODEM_SMS_NOTICE_DELIVER:
        {
            char *ud = ((struct sms_notice_deliver *)param)->ud;
            printf("recv sms: %s\n", ud);
#if FLASHDB_EN
            msg_tsdb_append_user("DuanXin",
                                 ((struct sms_notice_deliver *)param)->oa,
                                 ud);
#endif
            break;
        }

        default:
            break;
    }
}

void modem_mtcas_notice(uint evt, void *param, u8 type)
{
    switch (evt)
    {
        case MODME_MTCAS_NOTICE_READ_PB:
        {
            struct mtcas_notice_read_pb *notice = param;
            printf("(%d %s %d %s)\n",
                   notice->index,
                   notice->number,
                   notice->type,
                   notice->name);
#if FLASHDB_EN
            contacts_kvdb_set(notice->name, notice->number);
#endif
            break;
        }
        case MODME_MTCAS_NOTICE_REQ_SQ:
        {
            struct mtcas_notice_req_sq *notice = param;
            printf("signal quality %d\n", notice->rssi);
            break;
        }

        default:
            break;
    }
}

void modem_general_notice(uint evt, void *param, u8 type)
{
    switch (evt)
    {
        case MODEM_GENERAL_REQ_IMS_ID:
            printf("IMSI %s\n", param);
            break;
        case MODEM_GENERAL_REQ_PSN_ID:
        {
            printf("IMEI %s\n", param);
            break;
        }
        case MODEM_GENERAL_REQ_ICCID:
        {
            struct general_notice_req_iccid *notice = param;
            printf("ICCID %s\n", notice->iccid);
            break;
        }
        default:
            break;
    }
}

void modem_net_notice(uint evt, void *param, u8 type)
{
    printf("%s type:%d\n", __func__, type);
    switch (evt)
    {
        case MODEM_NET_STATUS_REPORT:
        {
            struct net_notice_status_report *notice = param;
            printf("STATUS_REPORT %d %d %s %s %s\n",
                   notice->time_zone,
                   notice->sav_time,
                   notice->oper_long,
                   notice->oper_short,
                   notice->univer_time);
            break;
        }
        case MODEM_NET_EPS_REGISTRAION:
        {
            if (type == AT_TYPE_ACTIVE_REPORT)
            {
                struct net_notice_eps_registraion_report *notice = param;
                printf("EPS_REGISTRAION %d %s %s\n",
                       notice->stat,
                       notice->tac,
                       notice->ci);
            }
            else if (type == AT_TYPE_CMD_READ)
            {
                struct net_notice_eps_registraion_read *notice = param;
                printf("EPS_REGISTRAION %d %d %s %s\n",
                       notice->n,
                       notice->stat,
                       notice->tac,
                       notice->ci);
            }
            break;
        }
        case MODEM_NET_PLMN_SELECTION:
        {
            struct net_notice_plmn_selection *notice = param;
            printf("PLMN_SELECTION %d %d %s %d %d\n",
                   notice->mode,
                   notice->format,
                   notice->oper,
                   notice->stat,
                   notice->act);
            break;
        }
    }
}

void modem_cmd_notice(bool is_ok, u32 error_code)
{
    printf("%s %d %d\n", __func__, is_ok, error_code);
    bsp_modem_call_check_ok(is_ok);
}

uint32_t iis_buf[60 * 4] AT(.sco_cache.buf);

void bsp_iis_init(u8 spr)
{
    printf("%s\n", __func__);

    bt_hfp_switch_to_phone();
    RSTCON0 &= ~BIT(1);
    RSTCON0 |= BIT(1);

    iis_spr_set(spr);

    modem_bypass_flag = false;
    iis_base_init(1, true, false, false, true, true, false);
    IISCON0 |= BIT(18);
    IISCON0 |= BIT(19);

    iis_dma_init(iis_buf, iis_buf + 60 * 2, 60);

    sco_buffer_reset();
    aubuf0_gpdma_init(0);
}

void bsp_iis_stop(void)
{
    iis_stop();
    modem_bypass_flag = true;
}

void bsp_modem_audio_start(void)
{
    bsp_iis_init(modem_cb.spr);
    modem_call_init();
    dac_spr_set(modem_cb.spr);
    dac_fade_wait();
    iis_start();
    bsp_loudspeaker_unmute();
}

void bsp_modem_audio_stop(void)
{
    bsp_loudspeaker_mute();
    bsp_iis_stop();
    modem_call_exit();
}

int bsp_modem_get_contains(u16 from, u16 to)
{
    contacts_kvdb_init();
    modem_mtcas_get_contains(from, to);
    return 0;
}

int bsp_modem_update_contains(void)
{
    contacts_kvdb_init();
    u32 addr = FLASH_UI_BASE + FLASH_UI_SIZE + 512;
    contacts_kvdb_get_list(addr, 1024);
    return 0;
}
#else
AT(.com_text)
void iis_isr(void) {}
void bsp_iis_stop(void) {}

int bsp_modem_get_init_flag(void)
{
    return 0;
}
void bsp_modem_init(void) {}
void bsp_modem_exit(void) {}
#endif
