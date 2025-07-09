#include "include.h"
#include "asr.h"

#if (ASR_SELECT == ASR_WS_AIR)

#define TDNN_RAM_EN  0

//汇编原型
//int vector_multadd(int8_t *in_buf, const int8_t *w, int in_dim)
//{
//  int sum = 0;
//  for (int i = 0; i < in_dim; i++) {
//      sum += in_buf[i] * w[i];
//  }
//  return sum;
//}

int vector_multadd(int8_t *in_buf, const int8_t *w, int in_dim);
void spiflash_lock(void);
void spiflash_unlock(void);
bool spiflash_read_kick(void *buf, u32 addr, uint len);
bool spiflash_read_wait(void);

#if ASR_PREFETCH_EN
#define BASE_ADDR   UI_BUF_ASR_WEIGHT_BIN
#define BASE_LEN    UI_LEN_ASR_WEIGHT_BIN
#define W7OD        488


typedef struct
{
    u16 all_frame;  //out_dim
    u16 frame_len;  //in_dim
    u8 load_frame;  //8k中多少个in_dim
} tdnn_prenum_t;

AT(.com_rodata.tdnn)
const tdnn_prenum_t tdnn_prenum[7] =
{
    {128, 120, 68},
    {128, 256, 32},
    {128, 256, 32},
    {128, 256, 32},
    {128, 256, 32},
    {128, 128, 64},
    {W7OD, 128, 64},
};

typedef struct
{
    u8 load_frame;
    u8 *buf;
} tdnn_cache_t;

typedef struct
{
    u16 all_frame;
    u8 index;
    u8 toggle;
    u32 load_addr;
    tdnn_cache_t cache[2];
} tdnn_pretetch_t;

extern const int16_t tdnn_mean_1[128];
extern const int16_t tdnn_bias_1[128];
extern const float tdnn_var_1[128];
extern const int16_t tdnn_mean_2[128];
extern const int16_t tdnn_bias_2[128];
extern const float tdnn_var_2[128];
extern const int16_t tdnn_mean_3[128];
extern const int16_t tdnn_bias_3[128];
extern const float tdnn_var_3[128];
extern const int16_t tdnn_mean_4[128];
extern const int16_t tdnn_bias_4[128];
extern const float tdnn_var_4[128];
extern const int16_t tdnn_mean_5[128];
extern const int16_t tdnn_bias_5[128];
extern const float tdnn_var_5[128];
extern const int16_t tdnn_mean_6[128];
extern const int16_t tdnn_bias_6[128];
extern const float tdnn_var_6[128];
extern const int16_t tdnn_bias_7[W7OD];
extern const float tdnn_scale_7[W7OD];

#if TDNN_RAM_EN
int16_t tdnn_ram_mean1[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias1[128] AT(.ws_asr.test);
float tdnn_ram_var1[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean2[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias2[128] AT(.ws_asr.test);
float tdnn_ram_var2[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean3[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias3[128] AT(.ws_asr.test);
float tdnn_ram_var3[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean4[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias4[128] AT(.ws_asr.test);
float tdnn_ram_var4[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean5[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias5[128] AT(.ws_asr.test);
float tdnn_ram_var5[128] AT(.ws_asr.test);
int16_t tdnn_ram_mean6[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias6[128] AT(.ws_asr.test);
float tdnn_ram_var6[128] AT(.ws_asr.test);
int16_t tdnn_ram_bias7[W7OD] AT(.ws_asr.test);
float tdnn_ram_scale7[W7OD] AT(.ws_asr.test);
#else
// int16_t tdnn_ram_bias[W7OD] AT(.ws_asr.test);
#endif
u8 tdnn_buffer[2][0x2000] AT(.ws_asr.test);
tdnn_pretetch_t tdnn_pretetch AT(.ws_asr.test);

volatile u8 asr_prefetch_kisck;
int sum_buffer[W7OD] AT(.ws_asr.test);

// bool dump_asr_bin = true;
AT(.com_text.tdnn)
void tdnn_compute(int8_t *in_buf, float in_scale, int in_dim, int out_dim,
                  const int8_t *tdnn_weight, const int16_t *tdnn_bias,
                  const int16_t *tdnn_mean, const float *tdnn_var, const float *tdnn_scale,
                  float *out_buf, int last_layer)
{
    int i;
    if (asr_prefetch_kisck)
    {
        return;
    }
    // printf("enter %s\n", __func__);
    // GPIOB |= 0x08;
    tdnn_pretetch_t *t = &tdnn_pretetch;
    const tdnn_prenum_t *p = &tdnn_prenum[t->index];
    u32 frame_len = p->frame_len * p->load_frame;
    // printf("tdnn_weight:%x out_dim:%d in_dim:%d index:%d frame_len:%d\n", tdnn_weight, out_dim, in_dim, t->index, frame_len);

#if TDNN_RAM_EN
    if (tdnn_mean == tdnn_mean_1)
    {
        tdnn_mean = tdnn_ram_mean1;
        tdnn_bias = tdnn_ram_bias1;
        tdnn_var = tdnn_ram_var1;
    }
    else if (tdnn_mean == tdnn_mean_2)
    {
        tdnn_mean = tdnn_ram_mean2;
        tdnn_bias = tdnn_ram_bias2;
        tdnn_var = tdnn_ram_var2;
    }
    else if (tdnn_mean == tdnn_mean_3)
    {
        tdnn_mean = tdnn_ram_mean3;
        tdnn_bias = tdnn_ram_bias3;
        tdnn_var = tdnn_ram_var3;
    }
    else if (tdnn_mean == tdnn_mean_4)
    {
        tdnn_mean = tdnn_ram_mean4;
        tdnn_bias = tdnn_ram_bias4;
        tdnn_var = tdnn_ram_var4;
    }
    else if (tdnn_mean == tdnn_mean_5)
    {
        tdnn_mean = tdnn_ram_mean5;
        tdnn_bias = tdnn_ram_bias5;
        tdnn_var = tdnn_ram_var5;
    }
    else if (tdnn_mean == tdnn_mean_6)
    {
        tdnn_mean = tdnn_ram_mean6;
        tdnn_bias = tdnn_ram_bias6;
        tdnn_var = tdnn_ram_var6;
    }
    else
    {
        tdnn_scale = tdnn_ram_scale7;
        tdnn_bias = tdnn_ram_bias7;
    }
#else
    // if (tdnn_bias == tdnn_bias_1) {
    //     memcpy(tdnn_ram_bias, tdnn_bias_1, sizeof(tdnn_bias_1));
    // } else if (tdnn_bias == tdnn_bias_2) {
    //     memcpy(tdnn_ram_bias, tdnn_bias_2, sizeof(tdnn_bias_2));
    // } else if (tdnn_bias == tdnn_bias_3) {
    //     memcpy(tdnn_ram_bias, tdnn_bias_3, sizeof(tdnn_bias_3));
    // } else if (tdnn_bias == tdnn_bias_4) {
    //     memcpy(tdnn_ram_bias, tdnn_bias_4, sizeof(tdnn_bias_4));
    // } else if (tdnn_bias == tdnn_bias_5) {
    //     memcpy(tdnn_ram_bias, tdnn_bias_5, sizeof(tdnn_bias_5));
    // } else if (tdnn_bias == tdnn_bias_6) {
    //     memcpy(tdnn_ram_bias, tdnn_bias_6, sizeof(tdnn_bias_6));
    // } else {
    //     memcpy(tdnn_ram_bias, tdnn_bias_7, sizeof(tdnn_bias_7));
    // }
    for (i = 0; i< out_dim; i++)
    {
        sum_buffer[i] = tdnn_bias[i];
    }
#endif

    int64_t scale = in_scale * 1000000;

    spiflash_lock();
    for (i = 0; i < out_dim; i++)
    {
        u16 offset = (i * in_dim) % frame_len;
        //load下一帧
        if (offset == 0)
        {
            u16 load_frame = 0;
            t->toggle ^= 1;
            if (t->all_frame == 0)
            {
                t->index++;
                if (t->index >= 7)
                {
                    t->index = 0;
                }
                p = &tdnn_prenum[t->index];
                t->all_frame = p->all_frame;
                load_frame = p->load_frame;
            }
            else
            {
                if (t->all_frame >= p->load_frame)
                {
                    load_frame = p->load_frame;
                }
                else
                {
                    load_frame = t->all_frame;
                }
            }
            u32 load_len = load_frame*p->frame_len;
            spiflash_read_wait();
            spiflash_read_kick(t->cache[t->toggle ^ 1].buf, t->load_addr, load_len);

            // if (dump_asr_bin)
            //     huart_tx(t->cache[t->toggle ^ 1].buf, load_len);

            t->all_frame -= load_frame;
            t->load_addr += load_len;
            if (t->load_addr >= (BASE_ADDR + BASE_LEN))
            {
                t->load_addr = BASE_ADDR;
                // dump_asr_bin = false;
            }
            // printf("addr:%x\n", t->load_addr);
        }
#if TDNN_RAM_EN
        sum_buffer[i] = (int32_t)((int64_t)(vector_multadd(in_buf, (const int8_t *)t->cache[t->toggle].buf + offset, in_dim) * scale) / 1000000) + tdnn_bias[i];
#else
        // sum_buffer[i] = (int32_t)((int64_t)(vector_multadd(in_buf, (const int8_t *)t->cache[t->toggle].buf + offset, in_dim) * scale) / 1000000) + tdnn_ram_bias[i];
        sum_buffer[i] = (int32_t)((int64_t)(vector_multadd(in_buf, (const int8_t *)t->cache[t->toggle].buf + offset, in_dim) * scale) / 1000000) + (int16_t)sum_buffer[i];
#endif
        // if (!last_layer) {
        //     if (z <= 0) {
        //         z = 0;
        //     }
        //     out_buf[i] = (z - tdnn_mean[i]) * tdnn_var[i];
        // } else {
        //     out_buf[i] = z * tdnn_scale[i];
        // }

    }
    spiflash_read_wait();
    spiflash_unlock();

    for (i = 0; i < out_dim; i++)
    {
        if (!last_layer)
        {
            if (sum_buffer[i] <= 0)
            {
                sum_buffer[i] = 0;
            }
            out_buf[i] = (sum_buffer[i] - tdnn_mean[i]) * tdnn_var[i];
        }
        else
        {
            out_buf[i] = sum_buffer[i] * tdnn_scale[i];
        }
    }

    asm("nop");
//    GPIOB &= ~0x08;
}

ALIGNED(512)
void asr_prefetch_init_do(void)
{
    tdnn_pretetch_t *t = &tdnn_pretetch;
    memset(t, 0, sizeof(tdnn_pretetch_t));
    const tdnn_prenum_t *p = &tdnn_prenum[t->index];
    t->load_addr = BASE_ADDR;
    t->cache[0].buf = tdnn_buffer[0];
    t->cache[1].buf = tdnn_buffer[1];

    //先读
    u32 load_len = p->load_frame*p->frame_len;
    spiflash_lock();
    spiflash_read_kick(t->cache[0].buf, t->load_addr, load_len);
    spiflash_read_wait();
    t->cache[0].load_frame = p->load_frame;
    t->all_frame = p->all_frame - p->load_frame;        //已经load了一帧，减掉
    t->load_addr += load_len;

    spiflash_unlock();
#if TDNN_RAM_EN
    memcpy(tdnn_ram_mean1,     tdnn_mean_1,     sizeof(tdnn_mean_1));
    memcpy(tdnn_ram_bias1,     tdnn_bias_1,     sizeof(tdnn_bias_1));
    memcpy(tdnn_ram_var1,      tdnn_var_1,      sizeof(tdnn_var_1));
    memcpy(tdnn_ram_mean2,     tdnn_mean_2,     sizeof(tdnn_mean_2));
    memcpy(tdnn_ram_bias2,     tdnn_bias_2,     sizeof(tdnn_bias_2));
    memcpy(tdnn_ram_var2,      tdnn_var_2,      sizeof(tdnn_var_2));
    memcpy(tdnn_ram_mean3,     tdnn_mean_3,     sizeof(tdnn_mean_3));
    memcpy(tdnn_ram_bias3,     tdnn_bias_3,     sizeof(tdnn_bias_3));
    memcpy(tdnn_ram_var3,      tdnn_var_3,      sizeof(tdnn_var_3));
    memcpy(tdnn_ram_mean4,     tdnn_mean_4,     sizeof(tdnn_mean_4));
    memcpy(tdnn_ram_bias4,     tdnn_bias_4,     sizeof(tdnn_bias_4));
    memcpy(tdnn_ram_var4,      tdnn_var_4,      sizeof(tdnn_var_4));
    memcpy(tdnn_ram_mean5,     tdnn_mean_5,     sizeof(tdnn_mean_5));
    memcpy(tdnn_ram_bias5,     tdnn_bias_5,     sizeof(tdnn_bias_5));
    memcpy(tdnn_ram_var5,      tdnn_var_5,      sizeof(tdnn_var_5));
    memcpy(tdnn_ram_mean6,     tdnn_mean_6,     sizeof(tdnn_mean_6));
    memcpy(tdnn_ram_bias6,     tdnn_bias_6,     sizeof(tdnn_bias_6));
    memcpy(tdnn_ram_var6,      tdnn_var_6,      sizeof(tdnn_var_6));
    memcpy(tdnn_ram_bias7,     tdnn_bias_7,     sizeof(tdnn_bias_7));
    memcpy(tdnn_ram_scale7,    tdnn_scale_7,    sizeof(tdnn_scale_7));
#endif
}

void asr_prefetch_init(void)
{
    asr_prefetch_init_do();
    asr_prefetch_kisck = 5;
    printf(">>>>>>>>>>>>>>>>>>>enter %s\n", __func__);
}

#endif

#if !ASR_PREFETCH_EN
AT(.com_text.tdnn)
void tdnn_compute(int8_t *in_buf, float in_scale, int in_dim, int out_dim,
                  const int8_t *tdnn_weight, const int16_t *tdnn_bias,
                  const int16_t *tdnn_mean, const float *tdnn_var, const float *tdnn_scale,
                  float *out_buf, int last_layer)
{
//    GPIOB |= 0x08;
    for(int i = 0; i < out_dim; i++)
    {
        const int8_t *w = tdnn_weight + i * in_dim;
        int32_t dot_prod = vector_multadd(in_buf, w, in_dim);
        int32_t q = dot_prod * in_scale;
        int32_t z = q + tdnn_bias[i];
        if(!last_layer)
        {
            if(z <= 0)
            {
                z = 0;
            }
            out_buf[i] = (z - tdnn_mean[i]) * tdnn_var[i];
        }
        else
        {
            out_buf[i] = z * tdnn_scale[i];
        }
    }
//    GPIOB &= ~0x08;
}
#endif

#endif //ASR_WS_AIR