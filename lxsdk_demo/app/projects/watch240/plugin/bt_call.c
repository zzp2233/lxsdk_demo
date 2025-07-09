#include "include.h"

#if BT_SCO_AINS3_EN
static ains3_cb_t ains3_cb AT(.sco_data.ains3);
#endif

#if BT_SCO_DNN_EN
static dnn_cb_t dnn_cb AT(.sco_data.dnn);
#endif

#if BT_SCO_FAR_NR_EN
static far_nr_cfg_t far_nr_cfg AT(.sco_data.far);
#endif

//每档0.5DB
AT(.rodata.sdadc)
const int aec_dig_gain_tbl[64] = {
    AEC_DIG_P0DB,  AEC_DIG_P0_5DB,  AEC_DIG_P1DB,  AEC_DIG_P1_5DB,  AEC_DIG_P2DB,  AEC_DIG_P2_5DB,  AEC_DIG_P3DB,  AEC_DIG_P3_5DB,
    AEC_DIG_P4DB,  AEC_DIG_P4_5DB,  AEC_DIG_P5DB,  AEC_DIG_P5_5DB,  AEC_DIG_P6DB,  AEC_DIG_P6_5DB,  AEC_DIG_P7DB,  AEC_DIG_P7_5DB,
    AEC_DIG_P8DB,  AEC_DIG_P8_5DB,  AEC_DIG_P9DB,  AEC_DIG_P9_5DB,  AEC_DIG_P10DB, AEC_DIG_P10_5DB, AEC_DIG_P11DB, AEC_DIG_P11_5DB,
    AEC_DIG_P12DB, AEC_DIG_P12_5DB, AEC_DIG_P13DB, AEC_DIG_P13_5DB, AEC_DIG_P14DB, AEC_DIG_P14_5DB, AEC_DIG_P15DB, AEC_DIG_P15_5DB,
    AEC_DIG_P16DB, AEC_DIG_P16_5DB, AEC_DIG_P17DB, AEC_DIG_P17_5DB, AEC_DIG_P18DB, AEC_DIG_P18_5DB, AEC_DIG_P19DB, AEC_DIG_P19_5DB,
    AEC_DIG_P20DB, AEC_DIG_P20_5DB, AEC_DIG_P21DB, AEC_DIG_P21_5DB, AEC_DIG_P22DB, AEC_DIG_P22_5DB, AEC_DIG_P23DB, AEC_DIG_P23_5DB,
    AEC_DIG_P24DB, AEC_DIG_P24_5DB, AEC_DIG_P25DB, AEC_DIG_P25_5DB, AEC_DIG_P26DB, AEC_DIG_P26_5DB, AEC_DIG_P27DB, AEC_DIG_P27_5DB,
    AEC_DIG_P28DB, AEC_DIG_P28_5DB, AEC_DIG_P29DB, AEC_DIG_P29_5DB, AEC_DIG_P30DB, AEC_DIG_P30_5DB, AEC_DIG_P31DB, AEC_DIG_P31_5DB,
};

void bt_sco_dump_init(u8 *sysclk, call_cfg_t *p)
{
    nr_cb_t *nr = &p->nr;
#if BT_SCO_DUMP_EN
    nr->dump_en = DUMP_MIC_TALK | DUMP_MIC_NR;
#if BT_AEC_EN
    if (xcfg_cb.bt_aec_en) {
        nr->dump_en |= DUMP_AEC_FAR;
    }
#endif
#elif BT_SCO_FAR_DUMP_EN
    nr->dump_en = DUMP_FAR_NR_INPUT | DUMP_FAR_NR_OUTPUT;
#elif BT_SCO_EQ_DUMP_EN
    nr->dump_en = DUMP_MIC_TALK | DUMP_MIC_NR | DUMP_EQ_OUTPUT;
#endif
    if (nr->dump_en != 0) {
        if (!xcfg_cb.eq_dgb_uart_en) {
            printf("dump huart xcfg init err!\n");
            nr->dump_en = 0;
        }
    }
}

void bt_sco_aec_init(u8 *sysclk, aec_cfg_t *aec, alc_cb_t *alc)
{

#if BT_AEC_EN
    if (xcfg_cb.bt_aec_en) {
        xcfg_cb.bt_alc_en   = 0;
        aec->aec_en         = 1;
        aec->mode           = 1;
        aec->nlp_bypass     = 0;
        aec->nlp_only       = 0;
        aec->nlp_level      = BT_ECHO_LEVEL;
        aec->nlp_part       = 1;                            //0:xd, 1:de&xd, 2:xd|xe
        aec->comforn_level  = 10;
        aec->comforn_floor  = 300;
        aec->comforn_en     = 0;
        aec->xe_add_corr    = 16384;
        aec->upbin          = 63;                           //调节最高频率2000Hz
        aec->lowbin         = 7;                            //调节最低频率250Hz
        if (!bt_sco_is_msbc()) {
            aec->upbin *= 2;
            aec->lowbin *= 2;
        }
        aec->qidx           = (u16)(0.35f * (1 << 15));     //smaller means more residual echo suppress
        aec->diverge_th     = 3;
        aec->echo_th        = 666;
        aec->gamma          = 30000;
        aec->aggrfact       = (u16)(0.9f * (1 << 15));      //bigger means more residual echo suppress
        aec->dig_gain       = aec_dig_gain_tbl[BT_DIG_GAIN];
        aec->far_offset     = 240 + BT_FAR_OFFSET;
        *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
    }
#endif

#if BT_ALC_EN
    if (xcfg_cb.bt_alc_en) {
        alc->alc_en = 1;
        alc->fade_in_delay = BT_ALC_FADE_IN_DELAY;
        alc->fade_in_step = BT_ALC_FADE_IN_STEP;
        alc->fade_out_delay = BT_ALC_FADE_OUT_DELAY;
        alc->fade_out_step = BT_ALC_FADE_OUT_STEP;
        alc->far_voice_thr = BT_ALC_VOICE_THR;
        if (!bt_sco_is_msbc()) {
            alc->fade_in_delay >>= 1;
            alc->fade_out_delay >>= 1;
            alc->fade_in_step <<= 1;
            alc->fade_out_step <<= 1;
        }
        *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
    }
#endif
}

//远端降噪
void bt_sco_far_nr_init(u8 *sysclk, nr_cb_t *nr)
{
#if BT_SCO_FAR_NR_EN
    nr->nr_cfg_en |= NR_CFG_FAR_EN;
    memset(&far_nr_cfg, 0, sizeof(far_nr_cfg_t));
    nr->far_nr = &far_nr_cfg;
    far_nr_cfg.enable           = BT_SCO_FAR_NR_EN;
    far_nr_cfg.level            = BT_SCO_FAR_NR_LEVEL;
    far_nr_cfg.noise_thr        = BT_SCO_FAR_NOISE_THR;
#endif
}

//MIC近端降噪
void bt_sco_nr_init(u8 *sysclk, nr_cb_t *nr)
{
    bt_sco_near_nr_init(sysclk, nr);
}

void bt_sco_nr_exit(void)
{
    bt_sco_near_nr_exit();
}

#if BT_SCO_AINS3_EN
void bt_sco_ains3_init(u8 *sysclk, nr_cb_t *nr)
{
    nr->nr_type             = NR_TYPE_AINS3;

    memset(&ains3_cb, 0, sizeof(ains3_cb_t));
	ains3_cb.nt              = BT_SCO_AINS3_LEVEL;
	ains3_cb.prior_opt_idx   = 10;
	ains3_cb.music_lev       = 11;
	ains3_cb.music_lev_hi    = 16;
	ains3_cb.music_lev_hi_range = 68;
	ains3_cb.ns_ps_rate      = 1;
	ains3_cb.low_fre_lev     = 7;
	ains3_cb.low_fre_range   = 1;        //range:1:16
	ains3_cb.ns_range_h      = 256;
	ains3_cb.ns_range_l      = 0;
	ains3_cb.noise_db        = -30;      //单位db，例：小于-30db为噪声。
	ains3_cb.noise_db2       = -15;
	ains3_cb.noise_db3       = -20;
	ains3_cb.smooth_en       = 0;

    bt_ains3_init(&ains3_cb);

    *sysclk = *sysclk < SYS_48M ? SYS_48M : *sysclk;
}

void bt_sco_ains3_exit(void)
{
    bt_ains3_exit();
}
#endif

#if BT_SCO_DNN_EN
void bt_sco_dnn_init(u8 *sysclk, nr_cb_t *nr)
{
    nr->nr_type             = NR_TYPE_DNN;

    memset(&dnn_cb, 0, sizeof(dnn_cb_t));
    dnn_cb.nt                   = BT_SCO_DNN_LEVEL;
    dnn_cb.model_select         = BT_SCO_DNN_MODEL_SET;
    dnn_cb.wind_en	            = BT_SCO_DNN_WIND_EN;
//	  dnn_cb.nt_post              = 0;
    dnn_cb.exp_range_H          = 1;
//	  dnn_cb.exp_range_L          = 0;
//    dnn_cb.param_printf         = 1;
//    dnn_cb.min_value            = 50;
//    dnn_cb.nostation_floor      = 0;
//    dnn_cb.wind_thr             = 5;
//    dnn_cb.noise_ps_rate        = 1;
//    dnn_cb.prior_opt_idx        = 3;
    dnn_cb.prior_opt_ada_en       = 1;
//    dnn_cb.wind_level           = 0;						//0：不打开风噪
//	  dnn_cb.wind_range           = 0;
	dnn_cb.low_fre_range          = 16;                     //范围0~16
//	dnn_cb.low_fre_range0         = 1;
//  dnn_cb.pitch_filter_en        = 1;
    dnn_cb.mask_floor             = 1600;
	dnn_cb.mask_floor_r           = 0;
	dnn_cb.music_lev              = 6;
	dnn_cb.gain_expand            = 1024;
	dnn_cb.nn_only                = 1;
	dnn_cb.nn_only_len            = 40;
	dnn_cb.gain_assign            = 19666;
    dnn_cb.preem_en               = 0;

    bt_dnn_init(&dnn_cb);

    *sysclk = *sysclk < SYS_192M ? SYS_192M : *sysclk;
}

void bt_sco_dnn_exit(void)
{
    bt_dnn_exit();
}
#endif
