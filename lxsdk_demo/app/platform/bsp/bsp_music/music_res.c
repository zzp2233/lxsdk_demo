#include "include.h"

void mp3_res_play_kick(u32 addr, u32 len);
void wav_res_play_kick(u32 addr, u32 len);
void wav_res_dec_process(void);
bool wav_res_is_play(void);
bool wav_res_stop(void);
void mp3_res_play_exit(void);

#if DAC_DNR_EN
static u8 msc_dnr_sta;
#endif

co_timer_t mute_timer;
void loudspeaker_mute_callback(void)
{
    bsp_loudspeaker_mute();
    co_timer_del(&mute_timer);
}

u8 mp3_res_process(void)
{
    if (sys_cb.mp3_res_playing)
    {
        if (get_music_dec_sta() == MUSIC_STOP)
        {
            printf("mp3 ring stop:%d\n", sys_cb.mute);
            sys_cb.mp3_res_playing = false;
            music_control(MUSIC_MSG_STOP);
            bsp_change_volume(sys_cb.vol);

            if (music_set_eq_is_done())
            {
                music_set_eq_by_num(sys_cb.eq_mode); // 恢复 EQ
            }
            mp3_res_play_exit();

            if (sys_cb.mute)
            {
                printf("co_timer_set(&mute_timer, 10, TIMER_ONE_SHOT, 0, loudspeaker_mute_callback, NULL);\n");
                dac_fade_out();
                co_timer_set(&mute_timer, 10, TIMER_ONE_SHOT, 0, loudspeaker_mute_callback, NULL);
            }

#if DAC_DNR_EN
            dac_dnr_set_sta(msc_dnr_sta);
#endif

            func_bt_mp3_play_restore();

#if (ASR_SELECT == ASR_YJ && ET_WAKEUP_EN)
#if 1
            et_start_asr();
#else
            set_et_play_tip_mode(0);
#endif // 0
#endif // ET_WAKEUP_EN

            return 0;
        }

    }
    return 1;
}

void mp3_res_play_do(u32 addr, u32 len, bool sync)
{
    if (len == 0 || sco_get_incall_flag(INCALL_FLAG_SCO))
    {
#if ASR_SELECT && ASR_AND_SIRI_PARALLEL_EN
        if (bsp_asr_init_sta())
        {
            sys_cb.asr_mp3_delay = true;
        }
#endif
        return;
    }

    printf("mp3 ring: %x, %d\n", addr, sys_cb.mute);
#if DAC_DNR_EN
    msc_dnr_sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif

    if (sys_cb.mute)
    {
        bsp_loudspeaker_unmute();
    }

    if (get_music_dec_sta() != MUSIC_STOP)   //避免来电响铃/报号未完成，影响get_music_dec_sta()状态
    {
        music_control(MUSIC_MSG_STOP);
    }

    bsp_change_volume(WARNING_VOLUME);
    if (music_set_eq_is_done())
    {
        music_set_eq_by_num(0); // EQ 设置为 normal
    }

#if (ASR_SELECT == ASR_YJ && ET_WAKEUP_EN)
#if 1
    et_stop_asr();
#else
    set_et_play_tip_mode(1);
#endif // 0
#endif // ET_WAKEUP_EN

    mp3_res_play_kick(addr, len);
    sys_cb.mp3_res_playing = true;
}

void mp3_res_play(u32 addr, u32 len)
{
    mp3_res_play_do(addr, len, 0);
}

void mp3_res_play_block(u32 addr, u32 len)
{
    bt_audio_bypass();

    mp3_res_play(addr, len);
    while(mp3_res_process())
    {
        bt_thread_check_trigger();
        WDT_CLR();
    }

    bt_audio_enable();
}

#if WARNING_WAVRES_PLAY
void wav_res_play_do(u32 addr, u32 len, bool sync)
{
    if (len == 0)
    {
        return;
    }

#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif

#if (ASR_SELECT == ASR_YJ && ET_WAKEUP_EN)
#if 1
    et_stop_asr();
#else
    set_et_play_tip_mode(1);
#endif // 0
#endif // ET_WAKEUP_EN

    wav_res_play_kick(addr, len);
    while (wav_res_is_play())
    {
        bt_thread_check_trigger();
        wav_res_dec_process();
        WDT_CLR();
    }
    wav_res_stop();

#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
#if (ASR_SELECT == ASR_YJ && ET_WAKEUP_EN)
#if 1
    et_start_asr();
#else
    set_et_play_tip_mode(0);
#endif // 0
#endif // ET_WAKEUP_EN
}

void wav_res_play(u32 addr, u32 len)
{
    wav_res_play_do(addr, len, 0);
}

#endif

