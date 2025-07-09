#include "include.h"

void bt_aec_process(u8 *ptr, u32 samples, int ch_mode);
void bt_adc_process(u8 *ptr, u32 samples, int ch_mode);
void bt_alc_process(u8 *ptr, u32 samples, int ch_mode);
void speaker_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void bt_sco_tx_process(u8 *ptr, u32 samples, int ch_mode);
void usbmic_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void recorder_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void mic_test_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
u8 bsp_modem_get_spr(void);
void bsp_speech_recognition_sdadc_process(u8 *ptr, u32 samples, int ch_mode);

void bt_sco_tx_delay_init(void);
void bt_sco_tx_delay_set(void *p);

#if MIC_TEST_EN
#define speaker_sdadc_callback  mic_test_sdadc_process
#else
#define speaker_sdadc_callback  sdadc_dummy
#endif // MIC_TEST_EN

#if UDE_MIC_EN
#define usbmic_sdadc_callback   usbmic_sdadc_process
#else
#define usbmic_sdadc_callback   sdadc_dummy
#endif // UDE_MIC_EN

#if BT_AEC_EN
#define bt_sdadc_callback    bt_aec_process
#elif BT_ALC_EN
#define bt_sdadc_callback    bt_alc_process
#else
#define bt_sdadc_callback    bt_adc_process
#endif

#if FUNC_RECORDER_EN
#define recorder_sdadc_callback     recorder_sdadc_process
#else
#define recorder_sdadc_callback     sdadc_dummy
#endif

#if OPUS_ENC_EN
#define opus_sdadc_callback   opus_sdadc_process
#else
#define opus_sdadc_callback   sdadc_dummy
#endif

#if ASR_SELECT
#define asr_sdadc_callback     bsp_speech_recognition_sdadc_process
#else
#define asr_sdadc_callback     sdadc_dummy
#endif

//MIC analog gain: 0~13(共14级), step 3DB (3db ~ +42db)
//adadc digital gain: 0~63, step 0.5 DB, 保存在gain的低6bit
const sdadc_cfg_t rec_cfg_tbl[] =
{
    /*   通道,              采样率,     增益,        通路控制,     保留,   样点数,   回调函数*/
    {CH_MIC_PF0,   SPR_16000,  (12 << 6),   ADC2DAC_EN,     0,      240,    speaker_sdadc_callback},        /* SPEAKER */
    {CH_MIC_PF0,   SPR_8000,   (12 << 6),   ADC2DAC_EN,     0,      480,    bt_sdadc_callback},             /* BTMIC   */
    {CH_MIC_PF0,   SPR_48000,  (12 << 6),   ADC2DAC_EN,     0,      128,    usbmic_sdadc_callback},
    {CH_MIC_PF0,   SPR_16000,  (12 << 6),   ADC2DAC_EN,     0,      256,    recorder_sdadc_callback},       /* RECORDER*/
    {CH_MIC_PF0,   SPR_16000,  (12 << 6),   ADC2DAC_EN,     0,      480,    bt_sdadc_callback},             /* IIS     */
    {CH_MIC_PF0,   SPR_16000,  (12 << 6),   ADC2DAC_EN,     0,      128,    opus_sdadc_callback},           /* opus    */
    {CH_MIC_PF0,   SPR_16000,  ASR_GAIN,    ADC2DAC_EN,     0,      ASR_SAMPLE,     asr_sdadc_callback},    /* ASR     */

};

void audio_path_init(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));
    sys_cb.audio_path = path_idx;
    if (path_idx == AUDIO_PATH_BTMIC)
    {

#if BT_AEC_EN
        if (xcfg_cb.bt_aec_en)
        {
            cfg.callback = bt_aec_process;
        }
        else
#endif
#if BT_ALC_EN
            if (xcfg_cb.bt_alc_en)
            {
                cfg.callback = bt_alc_process;
            }
            else
#endif
            {
                cfg.callback = bt_adc_process;
            }
        if (bt_sco_is_msbc() || bt_sco_dnn_is_en() || bt_siri_and_asr_parallel_en())                 //如果开了msbc或dnn，则采样率设为16k
        {
            cfg.sample_rate = SPR_16000;
        }
        if (xcfg_cb.bt_aec_en)
        {
            cfg.gain = (u16)BT_ANL_GAIN << 6;                       //开AEC后，数字GAIN放到AEC里做。
        }
        else
        {
            cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
        }
#if ASR_SELECT && ASR_SIRI_SCO_DELAY_EN
        if (bt_siri_and_asr_parallel_en())
        {
            bt_sco_tx_delay_init();
        }
#endif
    }
    else if (path_idx == AUDIO_PATH_MODEMMIC)
    {
#if MODEM_CAT1_EN
        cfg.sample_rate = bsp_modem_get_spr();
#endif
        if (xcfg_cb.bt_aec_en)
        {
            cfg.gain = (u16)BT_ANL_GAIN << 6;                       //开AEC后，数字GAIN放到AEC里做。
        }
        else
        {
            cfg.gain = ((u16)BT_ANL_GAIN << 6) | BT_DIG_GAIN;
        }
    }

    sdadc_init(&cfg);
    if (path_idx == AUDIO_PATH_BTMIC)
    {
        if (!bt_sco_is_msbc() && (bt_sco_dnn_is_en() || bt_siri_and_asr_parallel_en()))                //dnn: 走窄带通话时，ADC为16K，DAC为8K
        {
            dac_spr_set(SPR_8000);
        }
    }
}

void audio_path_start(u8 path_idx)
{
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));
    sdadc_start(cfg.channel);
    sys_cb.audio_path = path_idx;
}

void audio_path_exit(u8 path_idx)
{
#if ASR_SELECT && ASR_SIRI_SCO_DELAY_EN
    if (path_idx == AUDIO_PATH_BTMIC)
    {
        bt_sco_tx_delay_set(NULL);
    }
#endif
    sdadc_cfg_t cfg;
    memcpy(&cfg, &rec_cfg_tbl[path_idx], sizeof(sdadc_cfg_t));
    sdadc_exit(cfg.channel);
    adpll_spr_set(DAC_OUT_SPR);
    sys_cb.audio_path = path_idx;
}


void bt_call_audio_path_start(void)
{
    sdadc_start(CH_MIC_PF0);
}

void bt_call_audio_path_exit(void)
{
    sdadc_exit(CH_MIC_PF0);
    // adpll_spr_set(DAC_OUT_SPR);
}

u8 get_mic_dig_gain(void)
{
    return BT_DIG_GAIN;
}

//使能mic
void mic_mute(bool en)
{
    sdadc_analog_mic_mute(en);
}
u8 sco_pcm_dump_pass_cnt = 0;

//设置要跳过多少次处理PCM函数的次数
AT(.com_text.bt_voice.sco.pcm)
void bt_sco_pcm_set_dump_pass_cnt(u8 cnt)
{
    sco_pcm_dump_pass_cnt = cnt;
}

//获取还剩下要跳过多少次处理PCM函数的次数
AT(.com_text.bt_voice.sco.pcm)
u8 bt_sco_pcm_dump_pass_cnt_sub1(bool sub)
{
    if (sco_pcm_dump_pass_cnt > 0 && sub == true)
    {
        sco_pcm_dump_pass_cnt--;
    }

    return sco_pcm_dump_pass_cnt;
}

//-------------------------------------------------------------------------------

#if ASR_SELECT && ASR_SIRI_SCO_DELAY_EN

#define SCO_TX_OBUF_LEN    4*1024

typedef struct
{
    u32 sample;
    int ch_mode;
    u32 tick;
    u16 delay;
    au_stm_t *stm;
    u8 *obuf;
    u8 *temp;
    u32 obuf_len;
    u32 temp_len;
} sco_tx_delay_t;

sco_tx_delay_t sco_tx_delay AT(.asr_sco);
au_stm_t sco_tx_stm AT(.asr_sco);
u8 sco_tx_obuf[SCO_TX_OBUF_LEN] AT(.asr_sco);
u8 sco_tx_temp[SCO_TX_OBUF_LEN >> 2] AT(.asr_sco);

void bt_sco_tx_delay_init(void)
{
    memset(&sco_tx_obuf, 0, sizeof(sco_tx_obuf));
    memset(&sco_tx_temp, 0, sizeof(sco_tx_temp));
    memset(&sco_tx_delay, 0, sizeof(sco_tx_delay));
    memset(&sco_tx_stm, 0, sizeof(sco_tx_stm));
    sco_tx_stm.buf = sco_tx_stm.rptr = sco_tx_stm.wptr = sco_tx_obuf;
    sco_tx_stm.size = SCO_TX_OBUF_LEN;
    sco_tx_delay.temp_len = SCO_TX_OBUF_LEN >> 2;
    sco_tx_delay.obuf_len = SCO_TX_OBUF_LEN;
    sco_tx_delay.obuf = sco_tx_obuf;
    sco_tx_delay.temp = sco_tx_temp;
    sco_tx_delay.stm  = &sco_tx_stm;
    sco_tx_delay.delay = 100;//ms
    sco_tx_delay.tick = tick_get();
    bt_sco_tx_delay_set(&sco_tx_delay);
}

#endif