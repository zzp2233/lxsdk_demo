#ifndef __BT_CALL
#define __BT_CALL

void bt_sco_aec_init(u8 *sysclk, aec_cfg_t *aec, alc_cb_t *alc);
void bt_sco_far_nr_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_nr_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_nr_exit(void);

void bt_sco_ains3_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_ains3_exit(void);
void bt_sco_dnn_init(u8 *sysclk, nr_cb_t *nr);
void bt_sco_dnn_exit(void);

void bt_sco_dump_init(u8 *sysclk, call_cfg_t *p);

#if BT_SCO_AINS3_EN
#define bt_sco_near_nr_init(a, b)               bt_sco_ains3_init(a, b)
#define bt_sco_near_nr_exit()                   bt_sco_ains3_exit()
#elif BT_SCO_DNN_EN
#define bt_sco_near_nr_init(a, b)               bt_sco_dnn_init(a, b)
#define bt_sco_near_nr_exit()                   bt_sco_dnn_exit()
#else
#define bt_sco_near_nr_init(a, b)
#define bt_sco_near_nr_exit()
#endif

#endif
