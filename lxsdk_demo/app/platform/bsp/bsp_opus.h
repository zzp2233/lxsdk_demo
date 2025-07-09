#ifndef __BSP_OPUS_H
#define __BSP_OPUS_H

#define OPUS_ENC_START      0x80
#define OPUS_ENC_STOP       0x81

//enum{
//    NR_TYPE_NONE = 0,
//    NR_TYPE_SMIC,
//    NR_TYPE_DMIC,
//};

extern void bsp_opus_encode_start(void);
extern void bsp_opus_encode_stop(void);
extern bool bsp_opus_get_enc_frame(u8 *buff, u16 len);
extern bool bsp_opus_is_encode(void);
u16 opus_enc_data_len_get(void);
void opus_enc_test(void);
void opus_sdadc_process(u8 *ptr, u32 samples, int ch_mode);

extern bool opus_start;
extern bool opus_enc_done;

#endif
