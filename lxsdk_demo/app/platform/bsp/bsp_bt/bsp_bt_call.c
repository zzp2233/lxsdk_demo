#include "include.h"
#include "ute_module_call.h"

static call_cfg_t bt_voice_cb AT(.sco_data);

#if BT_VOIP_REJECT_EN
//是否拒绝通话建立前的SCO连接请求，0-不拒绝，1-拒绝SCO建立，2-SCO建立后移除
uint8_t cfg_bt_reject_sco_before_incall = 0;
//是否拒绝网络通话SCO连接请求
bool cfg_bt_reject_sco_with_voip = false;
#endif

void bt_call_init(call_cfg_t *p);
void bt_call_exit(void);
void bt_sco_magic_init(void);
void bt_sco_magic_exit(void);
bool hfp_hf_check_is_3way(void);

AT(.rodata.mic_gain)
const int mic_gain_tbl[16] =
{
    AEC_DIG_P0DB,
    AEC_DIG_P1DB,
    AEC_DIG_P2DB,
    AEC_DIG_P3DB,
    AEC_DIG_P4DB,
    AEC_DIG_P5DB,
    AEC_DIG_P6DB,
    AEC_DIG_P7DB,
    AEC_DIG_P8DB,
    AEC_DIG_P9DB,
    AEC_DIG_P10DB,
    AEC_DIG_P11DB,
    AEC_DIG_P12DB,
    AEC_DIG_P13DB,
    AEC_DIG_P14DB,
    AEC_DIG_P15DB,
};

#if BT_SCO_APP_DBG_EN
call_cfg_t *bt_voice_ptr = &bt_voice_cb;

//通话APP设置MIC模拟增益
AT(.text.sco_app_dbg.app)
void sco_app_set_anl_gain(u8 gain)
{
    xcfg_cb.bt_anl_gain = gain;
}

//通话APP设置MIC数字增益
AT(.text.sco_app_dbg.app)
void sco_app_set_dig_gain(u8 gain)
{
    xcfg_cb.bt_dig_gain = gain;
}

//通话APP设置后置增益
AT(.text.sco_app_dbg.app)
void sco_app_set_post_gain(u8 gain)
{
    xcfg_cb.mic_post_gain = gain;
}

//通话APP设置降噪算法使能
AT(.text.sco_app_dbg.app)
void sco_app_set_nr_en(bool en)
{
    xcfg_cb.bt_sco_nr_en = en;
}

//通话APP设置降噪算法参数
AT(.text.sco_app_dbg.app)
void sco_app_set_nr_level(u8 level)
{
    xcfg_cb.bt_sco_nr_level = level;
}

//通话APP设置汽车喇叭声降噪使能
AT(.text.sco_app_dbg.app)
void sco_app_set_trumpet_nr_en(bool en)
{
    xcfg_cb.bt_sco_nr_trumpet_en = en;
}

//通话APP设置AEC算法使能
AT(.text.sco_app_dbg.app)
void sco_app_set_aec_en(bool en)
{
    xcfg_cb.bt_aec_en = en;
}

//通话APP设置AEC回声消除级别
AT(.text.sco_app_dbg.app)
void sco_app_set_aec_echo_level(u8 level)
{
    xcfg_cb.bt_echo_level = level;
}

//通话APP设置AEC远端补偿值
AT(.text.sco_app_dbg.app)
void sco_app_set_aec_far_offset(u8 offset)
{
    xcfg_cb.bt_far_offset = offset;
}

//用于获取应用层的值
AT(.text.sco_app_dbg.app)
u8 bt_sco_dbg_call_var_get(u8 index)
{
    switch (index)
    {
        case 0:
            return xcfg_cb.bt_anl_gain;
        case 1:
            return xcfg_cb.bt_dig_gain;
        case 2:
            return xcfg_cb.mic_post_gain;
        default:
            return 0;
    }
}
#endif


#if BT_SCO_DBG_EN
void sdadc_set_digital_gain(u8 gain);
void sco_audio_set_param(u8 *ptr, u8 size)
{
    xcfg_cb.bt_anl_gain = ptr[3];
    set_mic_analog_gain(ptr[3], CH_MIC_PF0);

    xcfg_cb.bt_dig_gain = ptr[4] & 0x3f;
    sdadc_set_digital_gain(xcfg_cb.bt_dig_gain);

    xcfg_cb.mic_post_gain = ptr[5] & 0x0f;

    xcfg_cb.bt_sco_nr_level = ptr[6] & 0x0f;

    xcfg_cb.bt_echo_level = ptr[8] & 0x7;
    bt_voice_cb.aec.nlp_level = xcfg_cb.bt_echo_level;

    xcfg_cb.bt_far_offset = ptr[9];
    bt_voice_cb.aec.far_offset = ptr[9];

//    printf("anl gain: %d\n", xcfg_cb.bt_anl_gain);
//    printf("dig gain: %d\n", xcfg_cb.bt_dig_gain);
//    printf("post gain: %d\n", xcfg_cb.mic_post_gain);
//    printf("echo level: %d\n", xcfg_cb.bt_echo_level);
//    printf("far offset: %d\n", xcfg_cb.bt_far_offset);
}
#endif // BT_SCO_DBG_EN

#if FUNC_BT_EN

static void bt_sco_eq_drc_init(call_cfg_t *p)
{
    u32 mic_eq_addr, mic_eq_len;
    u32 mic_drc_addr, mic_drc_len;
    if (bt_sco_is_msbc())
    {
        mic_eq_addr  = RES_BUF_EQ_BT_MIC_16K_EQ;
        mic_eq_len   = RES_LEN_EQ_BT_MIC_16K_EQ;
        mic_drc_addr = RES_BUF_DRC_BT_MIC_16K_DRC;
        mic_drc_len  = RES_LEN_DRC_BT_MIC_16K_DRC;
    }
    else
    {
        mic_eq_addr  = RES_BUF_EQ_BT_MIC_8K_EQ;
        mic_eq_len   = RES_LEN_EQ_BT_MIC_8K_EQ;
        mic_drc_addr = RES_BUF_DRC_BT_MIC_8K_DRC;
        mic_drc_len  = RES_LEN_DRC_BT_MIC_8K_DRC;
    }
    if (mic_set_eq_by_res(mic_eq_addr, mic_eq_len))
    {
        p->mic_eq_en = 1;
        mic_set_drc_by_res(mic_drc_addr, mic_drc_len);      //mic drc
    }
    if (xcfg_cb.mic_post_gain)
    {
        p->post_gain = mic_gain_tbl[xcfg_cb.mic_post_gain];
    }
    mic_set_post_gain(mic_gain_tbl[xcfg_cb.mic_post_gain] << 8);

#ifdef RES_BUF_EQ_CALL_NORMAL_EQ
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
    if (!xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_spp_en)
#endif
    {
        music_set_eq_by_res(RES_BUF_EQ_CALL_NORMAL_EQ, RES_LEN_EQ_CALL_NORMAL_EQ);
    }
#else
    music_eq_off();
#endif
#ifdef RES_BUF_DRC_CALL_DAC_DRC
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
    if (!xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_spp_en)
#endif
    {
        if (!music_set_drc_by_res(RES_BUF_DRC_CALL_DAC_DRC, RES_LEN_DRC_CALL_DAC_DRC))
        {
            music_drc_off();
        }
    }
#else
    music_drc_off();
#endif
}

static void bt_sco_eq_drc_exit(void)
{
    mic_drc_off();
    mic_eq_off();
    music_set_eq_by_num(sys_cb.eq_mode);
    music_set_drc_by_res(RES_BUF_DRC_DAC_DRC, RES_LEN_DRC_DAC_DRC);
}

static void bt_call_alg_init(void)
{
    u8 sysclk = SYS_24M;
    memset(&bt_voice_cb, 0, sizeof(call_cfg_t));

    ///AEC初始化
    bt_sco_aec_init(&sysclk, &bt_voice_cb.aec, &bt_voice_cb.alc);

    ///通话MIC端降噪算法初始化
    bt_sco_nr_init(&sysclk, &bt_voice_cb.nr);

    ///通话DAC端降噪算法初始化
    bt_sco_far_nr_init(&sysclk, &bt_voice_cb.nr);

    ///算法数据dump初始化
    bt_sco_dump_init(&sysclk, &bt_voice_cb);

    ///DAC、MIC EQ/DRC初始化
    bt_sco_eq_drc_init(&bt_voice_cb);

#if BT_SCO_MAV_EN
    bt_voice_cb.mic_mav_en = 1;
    bt_sco_magic_init();
#endif
    sysclk = SYS_192M;
    sys_clk_req(INDEX_VOICE, sysclk);

    bt_call_init(&bt_voice_cb);
}

static void bt_call_alg_exit(void)
{
    bt_sco_nr_exit();

    bt_call_exit();

    bt_sco_eq_drc_exit();

#if BT_SCO_MAV_EN
    bt_sco_magic_exit();
#endif

    sys_clk_free(INDEX_VOICE);
}

void hfp_hf_call_notice(uint32_t evt)
{
    switch (evt)
    {
        case BT_NOTICE_INCOMING:
            printf("===>>> InComing, is 3way:%d\n", hfp_hf_check_is_3way());
            uteModuleCallSetBeforeCallStatus(1);
            bsp_call_mgr_send(CALL_MGR_BT_INCOM);
            break;
        case BT_NOTICE_OUTGOING:
            printf("===>>> OutGoing, is 3way:%d\n", hfp_hf_check_is_3way());
            uteModuleCallSetBeforeCallStatus(0);
            bsp_call_mgr_send(CALL_MGR_BT_OUTGO);
            break;
        case BT_NOTICE_INCALL:
            printf("===>>> InCall,   is 3way:%d\n", hfp_hf_check_is_3way());
            uteModuleCallResetCallingTimeSecond();
#if CALL_MGR_EN
            bt_cb.incall_flag = 1;
#endif
            bsp_call_mgr_send(CALL_MGR_BT_INCALL);
            break;
        case BT_NOTICE_ENDCALL:
            printf("===>>> EndCall\n");
            bsp_call_mgr_send(CALL_MGR_BT_ENDCALL);
            bt_cb.number_sta = false;
            bt_cb.call_type = CALL_TYPE_NONE;

            //保存通话记录
            if (sys_cb.refresh_language_flag == false)    //切换语言的时候不保存
            {
                memset(sys_cb.pbap_result_Name, 0, sizeof(sys_cb.pbap_result_Name));
                uteModuleCallUpdateRecordsData();
            }

#if CALL_MGR_EN
            bt_cb.incall_flag = 0;
            bt_cb.times = 0;
#endif
            break;
        case BT_NOTICE_CALL_NUMBER:
            printf("===>>> Number: %s\n", hfp_get_last_call_number(0));
            uteModuleCallSetContactsNumberAndName((uint8_t*)hfp_get_last_call_number(0), strlen(hfp_get_last_call_number(0)), NULL, 0);
            bt_cb.number_sta = true;
#if CALL_MGR_EN
            // 三方来电 延迟更新号码
            if (!bsp_call_mgr_need_update())
#endif
            {
                msg_enqueue(EVT_CALL_NUMBER_UPDATE);
            }
            break;
    }
}

void bsp_bt_call_times_inc(void)
{
#if CALL_MGR_EN
    if (bt_cb.incall_flag)
    {
        bt_cb.times++;
    }
#endif
}

void bsp_bt_call_process(bsp_call_mgr_msg_t msg)
{
#if CALL_MGR_EN
    switch (msg)
    {
        case CALL_MGR_BT_INCOM:
            bsp_iis_stop();
            func_cb.sta = FUNC_BT_RING;
            break;
        case CALL_MGR_BT_OUTGO:
            bsp_iis_stop();
            func_cb.sta = FUNC_BT_CALL;
            break;
        case CALL_MGR_BT_INCALL:
            func_cb.sta = FUNC_BT_CALL;
            break;
        case CALL_MGR_BT_ENDCALL:
            break;
        default:
            break;
    }
#endif
}

#if BT_HFP_3WAY_CTRL_EN

///三方来电号码更新控制
bool hfp_hf_3way_number_update_control(void)
{
    return true;
}

/**
 * @brief clcc 解析回调
 *
 * @param idx 通话序列索引（下标从1开始）
 * @param dir 0:去电 1:来电
 * @param status 0:活动中
 *               1:挂起
 *               2:拨号中（仅去电）
 *               3:提醒中（仅去电）
 *               4:来电中（仅来电）
 *               5:等待中（仅来电）
 * @param mode 0:通话 1:数据 2:传真
 * @param mpty 0:不是多方呼叫 1:是多方呼叫
 * @param number 号码
 * @param type
 */
void hfp_hf_parse_clcc_cb(uint8_t idx, uint8_t dir, uint8_t status, uint8_t mode, uint8_t mpty, char *number, uint8_t type)
{
    printf("===>>> clcc: idx:%d, dir:%d, status:%d, mode:%d, mpty:%d, number:%s, type:%d\n", idx, dir, status, mode, mpty, number, type);
}

#endif //HFP_3WAY_CONTROL_EN

ALIGNED(64)
void sco_set_incall_flag(u8 bit)
{
    GLOBAL_INT_DISABLE();
    sys_cb.incall_flag |= bit;
    GLOBAL_INT_RESTORE();
}

ALIGNED(128)
bool sco_clr_incall_flag(u8 bit)
{
    bool ret = false;
    GLOBAL_INT_DISABLE();
    if(sys_cb.incall_flag == INCALL_FLAG_FADE)
    {
        ret = true;
    }
    sys_cb.incall_flag &= ~bit;
    GLOBAL_INT_RESTORE();
    return ret;
}

ALIGNED(64)
bool sco_get_incall_flag(u8 bit)
{
    return (sys_cb.incall_flag & bit);
}


void sco_audio_init(void)
{
#if BT_SCO_APP_DBG_EN
    CLKGAT1 |= BIT(27);
#endif

#if (ASR_SELECT && ASR_FULL_SCENE)
#if ASR_AND_SIRI_PARALLEL_EN
    if (bt_get_siri_status())
    {
        audio_path_exit(AUDIO_PATH_ASR);
        bt_siri_and_asr_parallel_en_setter(true);
    }
    else
#endif
    {
        bsp_asr_stop();
    }
#endif
    sco_set_incall_flag(INCALL_FLAG_SCO);
    bt_call_alg_init();

#if BT_SCO_APP_DBG_EN
    bt_sco_app_dbg_init();                      //通话debug模式中，会重新初始化一遍通话参数
#endif // BT_SCO_APP_DBG_EN

    dac_set_anl_offset(1);
    audio_path_init(AUDIO_PATH_BTMIC);
    audio_path_start(AUDIO_PATH_BTMIC);
    // bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
    bsp_change_volume(VOL_MAX);     //通话改成默认最大音量
    dac_fade_in();
}

void sco_audio_exit(void)
{
    dac_fade_out();
    dac_aubuf_clr();
    audio_path_exit(AUDIO_PATH_BTMIC);
    bt_call_alg_exit();
    dac_set_anl_offset(0);
    sco_clr_incall_flag(INCALL_FLAG_SCO);
    bsp_change_volume(sys_cb.vol);

#if (ASR_SELECT && ASR_FULL_SCENE)
#if ASR_AND_SIRI_PARALLEL_EN
    if (bt_get_siri_status())
    {
        audio_path_init(AUDIO_PATH_ASR);
        audio_path_start(AUDIO_PATH_ASR);
        bt_siri_and_asr_parallel_en_setter(false);
    }
    else
#endif
    {
        bsp_asr_start();
    }
#endif

#if BT_SCO_APP_DBG_EN
    CLKGAT1 &= ~BIT(27);
#endif
}

void modem_call_init(void)
{
    CLKGAT1 |= BIT(4)|BIT(5)|BIT(27);
    CLKGAT2 |= BIT(18);

    sco_set_incall_flag(INCALL_FLAG_SCO);
    bt_call_alg_init();
    dac_set_anl_offset(1);
    audio_path_init(AUDIO_PATH_MODEMMIC);
    audio_path_start(AUDIO_PATH_MODEMMIC);
    // bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
    bsp_change_volume(VOL_MAX);             //通话改成默认最大音量
    dac_fade_in();
}

void modem_call_exit(void)
{
    dac_fade_out();
    dac_aubuf_clr();
    audio_path_exit(AUDIO_PATH_MODEMMIC);
    bt_call_alg_exit();
    dac_set_anl_offset(0);
    sco_clr_incall_flag(INCALL_FLAG_SCO);
    bsp_change_volume(sys_cb.vol);

    CLKGAT1 &= ~(BIT(4)|BIT(5)|BIT(27));
    CLKGAT2 &= ~BIT(18);
}

void bsp_bt_call_enter(void)
{
#if (ASR_SELECT && ASR_FULL_SCENE)
    bsp_asr_stop();
#endif
    sco_set_incall_flag(INCALL_FLAG_CALL);
    if(sys_cb.incall_flag == INCALL_FLAG_FADE)
    {
        // bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
        bsp_change_volume(VOL_MAX);                 //通话改成默认最大音量
        dac_fade_in();
    }
}

void bsp_bt_call_exit(void)
{
    bool vol_change = sco_clr_incall_flag(INCALL_FLAG_CALL);
    if(vol_change)
    {
        u32 tick = tick_get();
        while (!tick_check_expire(tick, 800))        //等待sco退出
        {
            bt_thread_check_trigger();
            delay_5ms(2);
            if (!sys_cb.incall_flag)
            {
                break;
            }
        }
        bsp_change_volume(sys_cb.vol);
    }

#if !CALL_MGR_EN
    hfp_hf_call_notice(BT_NOTICE_ENDCALL);
#endif
#if (ASR_SELECT && ASR_FULL_SCENE)
    bsp_asr_start();
#endif
}

#endif //FUNC_BT_EN
