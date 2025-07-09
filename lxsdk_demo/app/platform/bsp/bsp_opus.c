#include "include.h"
#include "bsp_opus.h"

#define OPUS_ENC_TIME   0
#define OPUS_DEC_TEST   0

#if OPUS_ENC_EN

extern u16 opus_pcm_len(void);
extern int opus_enc_frame(s16 *pcm, u8 *packet);
extern void opus_kick_start(void);
extern int sco_set_mic_gain_after_aec(void);
extern void mic_post_gain_process_s(s16 *ptr, int gain, int samples);
bool opus_enc_init(u32 spr, u32 nch, u32 bitrate);
extern void opus_enc_exit(void);
void opus_enc_process(void);

#if OPUS_ENC_TIME
static u32 opus_tick;
static bool opus_tick_start = true;
#endif

static u8 opus_sysclk, opus_init = 0;
bool opus_start;
static uint8_t opus_skip_frame = 0;

//---------------------------------------------------------------------
//
#define OPUS_ONCE_SIZE (opus_pcm_len() * 2)

#define OPUS_IN_BUF_SIZE        (320 * 2 * 2)
#define OPUS_OUT_BUF_SIZE       (80 * 2 * 2)

static u8 opus_in_buf[OPUS_IN_BUF_SIZE] AT(.opus.buf.bsp);
static u8 opus_out_buf[OPUS_OUT_BUF_SIZE] AT(.opus.buf.bsp);
static au_stm_t opus_in_stm AT(.opus.buf.bsp);
static au_stm_t opus_out_stm AT(.opus.buf.bsp);
bool opus_enc_done = false;
static u8 opus_packet[320*2] AT(.opus.buf.bsp);

AT(.com_text)
void remove_stm_buf(au_stm_t *stm, u16 len)
{
    GLOBAL_INT_DISABLE();
    stm->wptr -= len;
    stm->len -= len;
    if (stm->wptr < stm->buf) {
        stm->wptr = stm->wptr + stm->size;
    }
    GLOBAL_INT_RESTORE();
}

//--------------------------------------------------------------------------------
AT(.com_text.opus)
void opus_skip_frame_do(s16 *ptr, u32 samples)
{
    opus_skip_frame--;
    memset(ptr, 0, samples * 2);
}

//--------------------------------------------------------------------------------
//128点输入一次
AT(.com_text.sndp)
WEAK void opus_nr_process(s16 *buf)
{

}

//---------------------------------------------------------------------
AT(.com_text.opus)
static void opus_stm_put_do(u8 *buf, u16 len, au_stm_t *stm, char c)
{
    if ((stm->len + len) > stm->size) {
        remove_stm_buf(stm, len);
        uart_putchar(c);
    }

    puts_stm_buf(stm, buf, len);
}

AT(.com_text.opus)
static bool opus_stm_get_do(u8 *buf, u16 len, au_stm_t *stm)
{
    return gets_stm_buf(stm, buf, len);
}

AT(.com_text.opus)
void opus_put_pcm(u8 *buf, u16 len)
{
    opus_stm_put_do(buf, len, &opus_in_stm, '!');
}

AT(.com_text.opus)
static bool opus_get_pcm(u8 *buf, u16 len)
{
    return opus_stm_get_do(buf, len, &opus_in_stm);
}

//缓存压缩好的数据
AT(.com_text.opus)
void opus_put_frame(u8 *buf, u16 len)
{
    opus_stm_put_do(buf, len, &opus_out_stm, '@');
}

AT(.com_text.opus)
bool opus_get_frame(u8 *buf, u16 len)
{
	return opus_stm_get_do(buf, len, &opus_out_stm);
}

//---------------------------------------------------------------------

void opus_start_init(u8 nr_type)
{
    au_stm_t *stm = &opus_out_stm;
    stm->buf = stm->wptr = stm->rptr = opus_out_buf;
    stm->size = OPUS_OUT_BUF_SIZE;
    stm->len = 0;

    stm = &opus_in_stm;
    stm->buf = stm->wptr = stm->rptr = opus_in_buf;
    stm->size = OPUS_IN_BUF_SIZE;
    stm->len = 0;
}

void opus_stop_end(void)
{

}

//---------------------------------------------------------------------
//opus_sdadc_process

AT(.com_text.opus)
WEAK void opus_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    if (ch_mode != 0) {
        uart_putchar('*');
        return;
    }

#if OPUS_ENC_TIME
    if (opus_tick_start) {
        opus_tick = co_timer_tick_get();
        opus_tick_start = false;
    }
#endif
    if (opus_skip_frame) {
        opus_skip_frame_do((s16*)ptr, samples);
        return;
    }

    opus_put_pcm(ptr, samples * 2);

    if (opus_in_stm.len >= OPUS_ONCE_SIZE ) {
        opus_kick_start();
    }

#if TME_APP_EN
    tme_opus_frame_energy_cal(ptr,samples);
#endif
}

AT(.com_text.opus)
void opus_enc_process(void)
{
    int rlen;
    if (!opus_get_pcm(opus_packet, OPUS_ONCE_SIZE)) {
        uart_putchar('#');
        return;
    }
#if OPUS_ENC_TIME
    GPIOESET = BIT(3);
#endif //OPUS_ENC_TIME
    rlen = opus_enc_frame((void *)opus_packet, opus_packet);
#if OPUS_ENC_TIME
    GPIOECLR = BIT(3);
#endif //OPUS_ENC_TIME

    // 640 / 8 = 80
    if(rlen < 0 || rlen != 80) {
        // uart_putchar('&');
        printf("opus encode failed: %d\n",rlen);
    } else {
//        print_r(opus_packet, rlen);

//        printf("opus:%d\n", rlen);
        opus_put_frame(opus_packet, rlen);
    }
#if OPUS_ENC_TIME
    printf("opus_enc_time = %dms\n", (co_timer_tick_get() - opus_tick) * 8 / 5);
    opus_tick_start = true;
#endif
}

AT(.text.opus.bsp)
bool bsp_opus_is_encode(void)
{
    return (opus_init == 1);
}

u16 opus_enc_data_len_get(void)
{
    return opus_out_stm.len;
}


AT(.text.opus.bsp)
void bsp_opus_encode_start(void)
{
    // printf("opus_init:%d, bt_cb.disp_status:%d\n", opus_init, bt_cb.disp_status);
    if (opus_init == 0) { // && bt_cb.disp_status < BT_STA_INCOMING
        printf("--->bsp_opus_encode_start\n");
        // bt_tws_user_key(OPUS_ENC_START);
        bt_audio_bypass();
        if(opus_enc_init(SPR_16000, 1, 32000)){
            opus_sysclk = sys_clk_get();
            sys_clk_set(SYS_192M);
            opus_skip_frame = 125; //丢掉MIC刚启动时不稳定的数据
            opus_start_init(NR_TYPE_NONE);
            audio_path_init(AUDIO_PATH_OPUS);
            audio_path_start(AUDIO_PATH_OPUS);
            opus_init = 1;
        }
     }
}

AT(.text.opus.bsp)
void bsp_opus_encode_stop(void)
{
    if (opus_init) {
        printf("--->bsp_opus_encode_stop\n");
        opus_init = 0;
        opus_skip_frame = 0;
        // bt_tws_user_key(OPUS_ENC_STOP);
        audio_path_exit(AUDIO_PATH_OPUS);
        opus_enc_exit();
        opus_stop_end();
        sys_clk_set(opus_sysclk);
        bt_audio_enable();
    }
}

AT(.text.opus.bsp)
bool bsp_opus_get_enc_frame(u8 *buff, u16 len)
{
    if(opus_init && (buff != NULL)){
        return opus_get_frame(buff, len);
    }

    return false;
}

#if 1
static uint8_t sine_1k16k[88] = {
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
    0x11, 0x11, 0x11, 0x11, 0x11,
    // 0x00, 0x00, 0x2D, 0x12, 0xFB, 0x23, 0x10, 0x35, 0x10, 0x45, 0xAA, 0x53, 0x93, 0x60, 0x85, 0x6B,
    // 0x4C, 0x74, 0xB6, 0x7A, 0xA3, 0x7E, 0xFF, 0x7F, 0xC5, 0x7E, 0xF7, 0x7A, 0xAC, 0x74, 0x05, 0x6C,
    // 0x29, 0x61, 0x5D, 0x54, 0xD3, 0x45, 0xE3, 0x35, 0xDD, 0x24, 0x12, 0x13, 0xEA, 0x00, 0xBB, 0xEE,
    // 0xE4, 0xDC, 0xC7, 0xCB, 0xB4, 0xBB, 0x06, 0xAD, 0x08, 0xA0, 0xF9, 0x94, 0x17, 0x8C, 0x8E, 0x85,
    // 0x7F, 0x81, 0x01, 0x80, 0x1E, 0x81, 0xC7, 0x84, 0xF5, 0x8A, 0x81, 0x93, 0x3B, 0x9E, 0xF8, 0xAA,
    // 0x69, 0xB9, 0x48, 0xC9, 0x47, 0xDA, 0x04, 0xEC,
};

void opus_enc_test(void)
{
    au_stm_t *stm = &opus_in_stm;

    opus_enc_init(SPR_16000, 1, 32000);
    opus_start_init(NR_TYPE_NONE);

#if OPUS_ENC_TIME
    GPIOEFEN &= ~BIT(3);           //TEST TIME
    GPIOEDE  |= BIT(3);
    GPIOESET = BIT(3);
    GPIOEDIR &= ~BIT(3);

    GPIOEFEN &= ~BIT(1);           //TEST TIME
    GPIOEDE  |= BIT(1);
    GPIOESET = BIT(1);
    GPIOEDIR &= ~BIT(1);
#endif // OPUS_ENC_TIME
    printf("%s start\n", __func__);
    while (1) {
#if OPUS_ENC_TIME
        GPIOECLR = BIT(1);
#endif // OPUS_ENC_TIME
        WDT_CLR();
        opus_put_pcm(sine_1k16k, 88);
        if (stm->len >= OPUS_ONCE_SIZE) {
#if OPUS_ENC_TIME
            GPIOESET = BIT(1);
#endif // OPUS_ENC_TIME
            opus_enc_process();
        }

        u8 temp_buf[40];
        if (opus_get_frame(temp_buf, sizeof(temp_buf))) {
            print_r(temp_buf, sizeof(temp_buf));
        }
    }
}

#if OPUS_DEC_TEST
#define OPUS_DEC_TEST_BUF_SIZE  6400
static u8 opus_dec_buf[OPUS_DEC_TEST_BUF_SIZE];
static u32 opus_datalen;
static s16 temp_buf_dec[320]    AT(.opus_buf.dec.test);

void opus_dec_test(void)
{
    opus_datalen = 0;
    memset(opus_dec_buf, 0, OPUS_DEC_TEST_BUF_SIZE);

    bsp_opus_encode_start();
    while (opus_datalen + 80 <= OPUS_DEC_TEST_BUF_SIZE) {
        WDT_CLR();
        if (bsp_opus_is_encode()) {
            reset_sleep_delay_all();
            if(bsp_opus_get_enc_frame(&opus_dec_buf[opus_datalen], 80)) {
                opus_datalen += 80;
            }
            if (opus_datalen >= OPUS_DEC_TEST_BUF_SIZE) {
                printf("data full!!\n");
                bsp_opus_encode_stop();
                print_r(opus_dec_buf, sizeof(opus_dec_buf));
                break;
            }
        }
    }

    opus_dec_init(SPR_16000, 1);
    opus_datalen = 0;
    while (opus_datalen + 80 <= OPUS_DEC_TEST_BUF_SIZE) {
        WDT_CLR();
        memset(temp_buf_dec, 0, sizeof(temp_buf_dec));
        if(opus_dec_frame(&temp_buf_dec, &opus_dec_buf[opus_datalen], 80)) {
            print_r(temp_buf_dec, sizeof(temp_buf_dec));
            for (u16 i = 0; i < 320; i++) {
                obuf_put_one_sample(temp_buf_dec[i], temp_buf_dec[i]);
            }
            opus_datalen += 80;
        }
    }
}
#endif

#endif
#endif
