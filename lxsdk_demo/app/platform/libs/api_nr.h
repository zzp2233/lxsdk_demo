#ifndef _API_NR_H
#define _API_NR_H

//数字音量
#define _MAX_GAIN               (0x8000)

//10^(n/20), n为DB数                                                  n
#define AEC_DIG_P0DB            (int)(_MAX_GAIN * 1.000000)           //0db
#define AEC_DIG_P0_5DB          (int)(_MAX_GAIN * 1.059254)           //0.5db
#define AEC_DIG_P1DB            (int)(_MAX_GAIN * 1.122018)           //1db
#define AEC_DIG_P1_5DB          (int)(_MAX_GAIN * 1.188502)           //1.5db
#define AEC_DIG_P2DB            (int)(_MAX_GAIN * 1.258925)           //2db
#define AEC_DIG_P2_5DB          (int)(_MAX_GAIN * 1.333521)           //2.5db
#define AEC_DIG_P3DB            (int)(_MAX_GAIN * 1.412538)           //3db
#define AEC_DIG_P3_5DB          (int)(_MAX_GAIN * 1.496236)           //3.5db
#define AEC_DIG_P4DB            (int)(_MAX_GAIN * 1.584893)           //4db
#define AEC_DIG_P4_5DB          (int)(_MAX_GAIN * 1.678804)           //4.5db
#define AEC_DIG_P5DB            (int)(_MAX_GAIN * 1.778279)           //5db
#define AEC_DIG_P5_5DB          (int)(_MAX_GAIN * 1.883649)           //5.5db
#define AEC_DIG_P6DB            (int)(_MAX_GAIN * 1.995262)           //6db
#define AEC_DIG_P6_5DB          (int)(_MAX_GAIN * 2.113489)           //6.5db
#define AEC_DIG_P7DB            (int)(_MAX_GAIN * 2.238721)           //7db
#define AEC_DIG_P7_5DB          (int)(_MAX_GAIN * 2.371374)           //7.5db
#define AEC_DIG_P8DB            (int)(_MAX_GAIN * 2.511886)           //8db
#define AEC_DIG_P8_5DB          (int)(_MAX_GAIN * 2.660725)           //8.5db
#define AEC_DIG_P9DB            (int)(_MAX_GAIN * 2.818383)           //9db
#define AEC_DIG_P9_5DB          (int)(_MAX_GAIN * 2.985383)           //9.5db
#define AEC_DIG_P10DB           (int)(_MAX_GAIN * 3.162278)           //10db
#define AEC_DIG_P10_5DB         (int)(_MAX_GAIN * 3.349654)           //10.5db
#define AEC_DIG_P11DB           (int)(_MAX_GAIN * 3.548134)           //11db
#define AEC_DIG_P11_5DB         (int)(_MAX_GAIN * 3.758374)           //11.5db
#define AEC_DIG_P12DB           (int)(_MAX_GAIN * 3.981072)           //12db
#define AEC_DIG_P12_5DB         (int)(_MAX_GAIN * 4.216965)           //12.5db
#define AEC_DIG_P13DB           (int)(_MAX_GAIN * 4.466836)           //13db
#define AEC_DIG_P13_5DB         (int)(_MAX_GAIN * 4.731513)           //13.5db
#define AEC_DIG_P14DB           (int)(_MAX_GAIN * 5.011872)           //14db
#define AEC_DIG_P14_5DB         (int)(_MAX_GAIN * 5.308844)           //14.5db
#define AEC_DIG_P15DB           (int)(_MAX_GAIN * 5.623413)           //15db
#define AEC_DIG_P15_5DB         (int)(_MAX_GAIN * 5.956621)           //15.5db
#define AEC_DIG_P16DB           (int)(_MAX_GAIN * 6.309573)           //16db
#define AEC_DIG_P16_5DB         (int)(_MAX_GAIN * 6.683439)           //16.5db
#define AEC_DIG_P17DB           (int)(_MAX_GAIN * 7.079458)           //17db
#define AEC_DIG_P17_5DB         (int)(_MAX_GAIN * 7.498942)           //17.5db
#define AEC_DIG_P18DB           (int)(_MAX_GAIN * 7.943282)           //18db
#define AEC_DIG_P18_5DB         (int)(_MAX_GAIN * 8.413951)           //18.5db
#define AEC_DIG_P19DB           (int)(_MAX_GAIN * 8.912509)           //19db
#define AEC_DIG_P19_5DB         (int)(_MAX_GAIN * 9.440609)           //19.5db
#define AEC_DIG_P20DB           (int)(_MAX_GAIN * 10.000000)          //20db
#define AEC_DIG_P20_5DB         (int)(_MAX_GAIN * 10.592537)          //20.5db
#define AEC_DIG_P21DB           (int)(_MAX_GAIN * 11.220185)          //21db
#define AEC_DIG_P21_5DB         (int)(_MAX_GAIN * 11.885022)          //21.5db
#define AEC_DIG_P22DB           (int)(_MAX_GAIN * 12.589254)          //22db
#define AEC_DIG_P22_5DB         (int)(_MAX_GAIN * 13.335214)          //22.5db
#define AEC_DIG_P23DB           (int)(_MAX_GAIN * 14.125375)          //23db
#define AEC_DIG_P23_5DB         (int)(_MAX_GAIN * 14.962357)          //23.5db
#define AEC_DIG_P24DB           (int)(_MAX_GAIN * 15.848932)          //24db
#define AEC_DIG_P24_5DB         (int)(_MAX_GAIN * 16.788040)          //24.5db
#define AEC_DIG_P25DB           (int)(_MAX_GAIN * 17.782794)          //25db
#define AEC_DIG_P25_5DB         (int)(_MAX_GAIN * 18.836491)          //25.5db
#define AEC_DIG_P26DB           (int)(_MAX_GAIN * 19.952623)          //26db
#define AEC_DIG_P26_5DB         (int)(_MAX_GAIN * 21.134890)          //26.5db
#define AEC_DIG_P27DB           (int)(_MAX_GAIN * 22.387211)          //27db
#define AEC_DIG_P27_5DB         (int)(_MAX_GAIN * 23.713737)          //27.5db
#define AEC_DIG_P28DB           (int)(_MAX_GAIN * 25.118864)          //28db
#define AEC_DIG_P28_5DB         (int)(_MAX_GAIN * 26.607251)          //28.5db
#define AEC_DIG_P29DB           (int)(_MAX_GAIN * 28.183829)          //29db
#define AEC_DIG_P29_5DB         (int)(_MAX_GAIN * 29.853826)          //29.5db
#define AEC_DIG_P30DB           (int)(_MAX_GAIN * 31.622777)          //30db
#define AEC_DIG_P30_5DB         (int)(_MAX_GAIN * 33.496544)          //30.5db
#define AEC_DIG_P31DB           (int)(_MAX_GAIN * 35.481339)          //31db
#define AEC_DIG_P31_5DB         (int)(_MAX_GAIN * 37.583740)          //31.5db

#define DUMP_MIC_TALK                       BIT(0)      //主MIC数据
#define DUMP_MIC_FF                         BIT(1)      //副MIC数据
#define DUMP_MIC_NR                         BIT(2)      //降噪后数据
#define DUMP_AEC_INPUT                      BIT(4)      //AEC输入数据
#define DUMP_AEC_FAR                        BIT(5)      //AEC远端数据
#define DUMP_AEC_OUTPUT                     BIT(6)      //AEC输出数据
#define DUMP_FAR_NR_INPUT                   BIT(8)      //远端降噪输入数据
#define DUMP_FAR_NR_OUTPUT                  BIT(9)      //远端降噪输出数据
#define DUMP_EQ_OUTPUT                      BIT(10)     //MIC EQ输出数据


//call param
enum NR_TYPE
{
    NR_TYPE_NONE            = 0,
    NR_TYPE_AINS3,
    NR_TYPE_DNN,
};

enum NR_CFG_EN
{
    NR_CFG_FAR_EN           = BIT(0),           //使能远端降噪
    NR_CFG_SCO_FADE_EN      = BIT(3),           //使能通话前500ms淡入
};

typedef struct
{
    u16 nt;
    u8  nt_post;
    s16 exp_range_H;
    s16 exp_range_L;
    u8  model_select;
    u16 min_value;
    u16 nostation_floor;
    u8  wind_thr;
    u8  wind_en;
    u8  noise_ps_rate;
    u8  prior_opt_idx;
    u8  prior_opt_ada_en;
    u8  param_printf;                           //使能参数打印
    u8  wind_level;
    u16 wind_range;
    u16 low_fre_range;
    u16 low_fre_range0;
    u8  pitch_filter_en;
    u16 mask_floor;
    u8  mask_floor_r;
    u8  music_lev;
    u16 gain_expand;
    u8  nn_only;
    u16 nn_only_len;
    u16 gain_assign;
    u8  preem_en;
} dnn_cb_t;

typedef struct
{
    s16 nt;
    u8  prior_opt_idx;
    u8  music_lev;
    u16 music_lev_hi_range;
    u16 music_lev_hi;
    u16 ns_ps_rate;
    u8  low_fre_lev;
    u16 low_fre_range;
    u16 ns_range_l;
    u16 ns_range_h;
    s32 noise_db;
    s32 noise_db2;
    s32 noise_db3;
    u8 smooth_en;
} ains3_cb_t;

typedef struct
{
    u8 enable;
    u8 level;
    u8 noise_thr;
    u8 resv;
} far_nr_cfg_t;

typedef struct
{
    u8 nr_type;                                 //近端降噪类型
    u8 nr_cfg_en;                               //远端降噪、喇叭声降噪、500ms淡入等降噪配置
    u8 level;                                   //降噪强度
    u8 resv;
    u32 dump_en;
    void *far_nr;                               //远端降噪算法配置
} nr_cb_t;

typedef struct
{
    union
    {
        struct
        {
            u32 aec_en          : 1;
            u32 nlp_bypass      : 1;        //aec nlp bypass
            u32 nlp_only        : 1;        //aec nlp only select
            u32 comforn_en      : 1;        //comfortable noise enable
            u32 resv0           : 1;
            u32 mode            : 1;        //auto or manual mode
            u32 int_en          : 1;
            u32 nlp_level       : 4;        //aec nlp level
            u32 nlp_part        : 2;        //nlp mode choose
            u32 diverge_th      : 4;        //filter diverge threshold
            u32 comforn_level   : 4;        //range: 0~10
            u32 comforn_floor   : 10;       //range: 0~1000
            u32 resv1           : 1;
        };
        u32 aeccon0;
    };
    u16 aggrfact;
    u16 xe_add_corr;
    u8 upbin;                               //hnl valid upbin
    u8 lowbin;                              //hnl valid lowbin
    u16 qidx;                               //ref bin
    u16 gamma;                              //sxd/de smooth parameter
    u32 dig_gain;                           //0DB: 0x8000
    u32 echo_th;
    u16 far_offset;
    u8 rfu[2];
} aec_cfg_t;

typedef struct
{
    u8 alc_en;
    u8 rfu[1];
    u8 fade_in_step;
    u8 fade_out_step;
    u8 fade_in_delay;
    u8 fade_out_delay;
    s32 far_voice_thr;
} alc_cb_t;

typedef struct
{
    aec_cfg_t aec;
    alc_cb_t  alc;
    nr_cb_t   nr;
    u8 mic_eq_en;
    u8 mic_mav_en;
    u8 rfu[2];
    u32 post_gain;                  //后置数据增益
} call_cfg_t;

#if BT_SCO_APP_DBG_EN
void sco_audio_init(void);
void sco_audio_exit(void);

void bt_sco_app_dbg_init();
void bt_sco_dbg_connect_ack(void);
bool bt_sco_app_dbg_proc(u8 *data,u8 len);

void sco_app_msg_deal(u8 msg);
#endif // BT_SCO_APP_DBG_EN

bool bt_sco_dnn_is_en(void);
void bt_ains3_init(void *alg_cb);
void bt_ains3_exit(void);
void bt_dnn_init(void *alg_cb);
void bt_dnn_exit(void);
#endif // _API_NR_H
