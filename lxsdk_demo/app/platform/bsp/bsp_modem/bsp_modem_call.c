#include "include.h"
#include "api_iis.h"

void bsp_iis_init(u8 spr);
void modem_call_init(void);
void modem_call_exit(void);
void bsp_iis_stop(void);
bool cat1_is_bypass(void);

void bsp_modem_call_get_current_call(void);

#if MODEM_CAT1_EN
#define FLAG_CHECK_CLCC_OK (1 << 0)
#define FLAG_IS_IDLE       (1 << 1)
#define FLAG_INCALL        (1 << 2)

typedef struct
{
    u16 times;
    u8  flags;
} modem_call_t;

static modem_call_t modem_call_cb;

static void bsp_modem_call_filter(bsp_call_mgr_msg_t msg, bool is_confirm)
{
    if (!is_confirm && msg == CALL_MGR_MODEM_ENDCALL)
    {
        bsp_modem_call_get_current_call();
        return;
    }
    bsp_call_mgr_send(msg);
}

void bsp_modem_call_init(void)
{
    memset(&modem_call_cb, 0, sizeof(modem_call_cb));
}

void bsp_modem_call_check_ok(bool is_ok)
{
    if ((modem_call_cb.flags & (FLAG_CHECK_CLCC_OK | FLAG_IS_IDLE)) ==
        (FLAG_CHECK_CLCC_OK | FLAG_IS_IDLE))
    {
        printf("%s send ENDCALL\n", __func__);
        modem_call_cb.flags &= ~FLAG_CHECK_CLCC_OK;
        bsp_call_mgr_send(CALL_MGR_MODEM_ENDCALL);
    }
}

void bsp_modem_call_get_current_call(void)
{
    // send clcc
    modem_call_get_current_call();
    modem_call_cb.flags |= FLAG_CHECK_CLCC_OK | FLAG_IS_IDLE;
}

void bsp_modem_call_process(bsp_call_mgr_msg_t msg)
{
    printf("%s %d\n", __func__, msg);
    switch (msg)
    {
        case CALL_MGR_MODEM_INCOM:
            func_cb.sta          = FUNC_MODEM_RING;
            modem_cb.disp_status = BT_STA_INCOMING;
            break;
        case CALL_MGR_MODEM_OUTGO:

            func_cb.sta          = FUNC_MODEM_CALL;
            modem_cb.disp_status = BT_STA_OUTGOING;
            break;
        case CALL_MGR_MODEM_INCALL:
            modem_call_cb.flags |= FLAG_INCALL;

            func_cb.sta          = FUNC_MODEM_CALL;
            modem_cb.disp_status = BT_STA_INCALL;
            break;
        case CALL_MGR_MODEM_ENDCALL:
            modem_cb.disp_status = BT_STA_CONNECTED;
            memset(modem_cb.number, 0, sizeof(modem_cb.number));
            modem_call_cb.times = 0;
            modem_call_cb.flags &= ~FLAG_INCALL;
            break;
        default:
            break;
    }
}

void modem_call_notice_ring(void *param)
{
    struct call_notice_ring *notice = param;
    if (notice->mode == 1)
    {
        // if 非 音源-4G->DAC
        // return
        modem_cb.spr = SPR_16000;
        if (notice->arg == 8000)
        {
            modem_cb.spr = SPR_8000;
        }

        bsp_call_mgr_set_audio_path(CALL_AUDIO_PATH_MODEM2DAC);
    }
    else if (notice->mode == 0)
    {
        // if 音源-无音频
        bsp_call_mgr_set_audio_path(CALL_AUDIO_PATH_NONE);

        // else if 音源-蓝牙->DAC
        // bsp_call_mgr_set_audio_path(CALL_AUDIO_PATH_BTHF2DAC);
    }
}

void modem_call_notice_state(void *param)
{
    struct call_notice_state *notice = param;
    if (notice->dir)
    {
        // 被叫
    }
    else
    {
        // 主叫
    }

    printf("%s %d\n", __func__, notice->stat);
    switch (notice->stat)
    {
        case MODEM_CALL_STATUS_ACTIVE:
        case MODEM_CALL_STATUS_HOLD:
            bsp_modem_call_filter(CALL_MGR_MODEM_INCALL, false);
            break;
        case MODEM_CALL_STATUS_CALLING:
            bsp_modem_call_filter(CALL_MGR_MODEM_OUTGO, false);
            memcpy(modem_cb.number, notice->number, sizeof(modem_cb.number));
            break;
        case MODEM_CALL_STATUS_ALERTING:
            // mp3_res_play_kick(RES_BUF_RING_RING_MP3, RES_LEN_RING_RING_MP3);
            break;
        case MODEM_CALL_STATUS_INCOMING:
        case MODEM_CALL_STATUS_WAITING:
            bsp_modem_call_filter(CALL_MGR_MODEM_INCOM, false);
            memcpy(modem_cb.number, notice->number, sizeof(modem_cb.number));
            break;
        case MODEM_CALL_STATUS_RELEASED:
            bsp_modem_call_filter(CALL_MGR_MODEM_ENDCALL, false);
            break;
    }
}

void modem_call_notice_current_call(void *param)
{
    struct call_notice_current_call *notice = param;
    modem_call_cb.flags &= ~FLAG_IS_IDLE;

    switch (notice->stat)
    {
        case MODEM_CALL_STATUS_ACTIVE:
        case MODEM_CALL_STATUS_HOLD:
            bsp_modem_call_filter(CALL_MGR_MODEM_INCALL, true);
            memcpy(modem_cb.number, notice->number, sizeof(modem_cb.number));
            break;
        case MODEM_CALL_STATUS_CALLING:
            bsp_modem_call_filter(CALL_MGR_MODEM_OUTGO, true);
            memcpy(modem_cb.number, notice->number, sizeof(modem_cb.number));
            break;
        case MODEM_CALL_STATUS_ALERTING:
            break;
        case MODEM_CALL_STATUS_INCOMING:
            bsp_modem_call_filter(CALL_MGR_MODEM_INCOM, true);
            memcpy(modem_cb.number, notice->number, sizeof(modem_cb.number));
            break;
        case MODEM_CALL_STATUS_WAITING:
            break;
        case MODEM_CALL_STATUS_RELEASED:
            break;
    }
}

void bsp_modem_call_times_inc(void)
{
    if (modem_call_cb.flags & FLAG_INCALL)
    {
        modem_call_cb.times++;
    }
}

u16 bsp_modem_call_get_times(void)
{
    return modem_call_cb.times;
}

bool bt_sco_is_user_control(void)
{
    return true;
}

bool bt_sco_is_user_reject(void)
{
    if (!cat1_is_bypass())
    {
        return true;
    }

    // 音源-蓝牙的判断
    // if ...
    return false;
}
#else
void bsp_modem_call_get_current_call(void) {}
void bsp_modem_call_process(bsp_call_mgr_msg_t msg) {}
u16  bsp_modem_call_get_times(void)
{
    return 0;
}
#endif
