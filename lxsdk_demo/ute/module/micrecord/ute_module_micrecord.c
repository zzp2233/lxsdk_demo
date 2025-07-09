/**
 *@file
 *@brief        录音模块
 *@details
 *@author       zn.zeng
 *@date       2021-12-21
 *@version      v1.0
 */

#include "include.h"
#include "ute_module_micrecord.h"
#include "ute_module_log.h"
#include "ute_module_charencode.h"
#include "ute_module_systemtime.h"
#include "ute_module_message.h"
// #include "ute_module_music.h"

#define TRACE_EN UTE_LOG_MICRECORD_LVL

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if MIC_TEST_EN
#define FACTORY_RECORD_DATA_START (UTE_OTA_TMP_ADDRESS + 100 * 1024)
#define MIC_TEST_VOL        (VOL_MAX / 2)
#define MIC_BUF_IS_FULL     mic_cb.rec_buf_full

#define FACTORY_RECORD_DATA_LENGTH (128 * 1024)
#define FACTORY_RECORD_DATA_END (FACTORY_RECORD_DATA_START + FACTORY_RECORD_DATA_LENGTH)

#if (FACTORY_RECORD_DATA_LENGTH + 100 * 1024) > UTE_OTA_TMP_SIZE
#error "FACTORY_RECORD_DATA_LENGTH > UTE_OTA_TMP_SIZE"
#endif

#define RECORD_FLASH_BUF_LENGTH 4096

//mic数据缓冲区
typedef struct
{
    u8 rec_buf[MIC_TEST_BUF_SIZE];
    u32 rec_datalen;
    bool rec_buf_full;
    volatile u32 samples;
    volatile int ch_mode;
    volatile u8 iis_buf_size;
    u32 mic_data_r_size;
} mic_testbuf_t;

AT(.mic_test_buf)
mic_testbuf_t mic_cb;
//record数据环形缓冲区
typedef struct
{
    char *pHead;        //缓冲区头
    char *pVaildRead;   //读指针
    char *pVaildWrite;  //写指针
    char *pTail;        //缓冲区尾
} record_ringbuf_t;
#if MIC_DATA_TO_FLASH
u8 test_buf[256];
static record_ringbuf_t record_rx_ringbuf;               // rx数据环形缓冲区
static void record_ringbuf_init(record_ringbuf_t *ringbuf, char *buf_data, int buf_len);
static void record_ringbuf_write(record_ringbuf_t *ringbuf, char *pBuff, int add_len);
static void record_ringbuf_read(record_ringbuf_t *ringbuf, char *pBuff, int read_len);
void func_record_analysis(void);
AT(.com_text.str)
char mic_test_str01[] = ">>> %d %d\n";
#endif
AT(.com_text.str)
char mic_test_str[] = "data full!!!\n";


// AT(.mic_test_buf) // 复用aram
static uint8_t record_flash_buf[RECORD_FLASH_BUF_LENGTH];
void *uteModuleMicRecordTimerPointer = NULL;

typedef struct
{
    uint16_t record_flash_buf_write_count;
    uint32_t record_flash_data_write_length;
    uint16_t record_flash_buf_read_count;
    uint32_t record_flash_data_read_length;
    uint8_t record_state;
} factory_test_earphone_data_t;

static factory_test_earphone_data_t factory_test_earphone_data;

AT(.com_text.func.mic_test)
bool put_mic_obuf(u8 *ptr, u32 samples, int ch_mode)
{
    mic_cb.samples = samples;
    mic_cb.ch_mode = ch_mode;
#if MIC_DATA_TO_FLASH
    if (mic_cb.rec_datalen > MIC_TEST_BUF_SIZE*5 - samples * 2)
    {
        TRACE(mic_test_str);
        return true;
    }
    //TRACE(mic_test_str01,samples*2,mic_cb.rec_datalen);
    //print_r(ptr,samples*2);
    mic_cb.iis_buf_size ++;
    record_ringbuf_write(&record_rx_ringbuf, (char *)ptr, samples * 2);
#else
    if (mic_cb.rec_datalen > MIC_TEST_BUF_SIZE - samples * 2)
    {
        TRACE(mic_test_str);
        return true;
    }
    memcpy(&mic_cb.rec_buf[mic_cb.rec_datalen], ptr, samples * 2);
#endif

    mic_cb.rec_datalen += samples * 2;
    return false;
}

AT(.com_text.func.mic_test)
void mic_test_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    if (mic_cb.rec_buf_full == false && put_mic_obuf(ptr, samples, ch_mode))
    {
        mic_cb.rec_buf_full = true;
    }
#if MIC_DATA_TO_FLASH
    func_record_analysis();
#endif
}

void mic_test_init(void)
{
    TRACE("-->%s\n",__func__);
    bt_audio_bypass();  //复用aram，bypass蓝牙音乐
    memset(&mic_cb, 0, sizeof(mic_testbuf_t));
}

void mic_test_start(void)
{
    TRACE("-->%s\n",__func__);
    mic_cb.rec_datalen = 0;
    mic_cb.rec_buf_full = false;
    mic_cb.mic_data_r_size = 0;
#if MIC_DATA_TO_FLASH
    for(u8 i = 0; i < 25; i++)
    {
        os_spiflash_erase(FACTORY_RECORD_DATA_START + i*FLASH_SECTOR_SIZE);
    }
#endif
    audio_path_init(AUDIO_PATH_SPEAKER);
    audio_path_start(AUDIO_PATH_SPEAKER);
}

void mic_test_exit(void)
{
    TRACE("-->%s\n",__func__);
    audio_path_exit(AUDIO_PATH_SPEAKER);
    bt_audio_enable();
}

void mic_test_outp(void)
{
    TRACE("-->%s\n",__func__);
    bool mute_bkp;
    bsp_change_volume(MIC_TEST_VOL);
    mute_bkp = bsp_get_mute_sta();
    if (mute_bkp)
    {
        bsp_sys_unmute();
    }
    else
    {
        dac_fade_in();
    }
    dac_spr_set(SPR_16000);

//    u32 tick = tick_get();
    for (int i = 0; i < mic_cb.rec_datalen; i += 2)   //阻塞播放
    {
        s16 *ptr16 = (s16 *)&mic_cb.rec_buf[i];
        obuf_put_one_sample(*ptr16, *ptr16);
    }
//    printf("tick[%d]\n", tick_get() - tick);

    dac_fade_out();
    dac_fade_wait();

    bsp_change_volume(sys_cb.vol);
    if (mute_bkp)
    {
        bsp_sys_mute();
    }
}

#else

#define MIC_BUF_IS_FULL     0
#define mic_test_init()
#define mic_test_start()
#define mic_test_exit()
#define mic_test_outp()

#endif // MIC_TEST_EN

void uteModuleMicRecordFactoryEnter(void)
{
    mic_test_init();
    factory_test_earphone_data.record_state = MIC_BUF_IS_FULL ? FACTORY_TEST_RECORD_RECORDED : FACTORY_TEST_RECORD_IDLE;
#if MIC_DATA_TO_FLASH
    record_ringbuf_init(&record_rx_ringbuf, (char *)mic_cb.rec_buf, sizeof(mic_cb.rec_buf));
#endif
}

/**
 *@brief  工厂模式结束测试
 *@details
 *@author        zn.zeng
 *@date        2022-01-10
 */
void uteModuleMicRecordFactoryExit(void)
{
    UTE_MODULE_LOG(UTE_LOG_MICRECORD_LVL, "%s", __func__);
    mic_test_exit();
}

void uteModuleMicRecordFactoryTimerCallback(void *pxTimer)
{
    printf("%s", __func__);
    uteModulePlatformDeleteTimer(&uteModuleMicRecordTimerPointer);
    uteModuleMicRecordTimerPointer = NULL;
    factory_test_earphone_data.record_state = FACTORY_TEST_RECORD_RECORDED;
}

/**
 *@brief  工厂模式开始测试
 *@details
 *@author        zn.zeng
 *@date        2021-12-21
 */
void uteModuleMicRecordFactoryStart(void)
{
    factory_test_earphone_data.record_state = FACTORY_TEST_RECORD_RECORDING;
    if (uteModuleMicRecordTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleMicRecordTimerPointer, "MicRecord", 1,
                                     UTE_MODULE_MIC_FACTORY_TEST_RECORDING_TIME * 1000, false, uteModuleMicRecordFactoryTimerCallback);
    }
    uteModulePlatformRestartTimer(&uteModuleMicRecordTimerPointer, UTE_MODULE_MIC_FACTORY_TEST_RECORDING_TIME * 1000);
    if (MIC_TEST_EN && factory_test_earphone_data.record_state != FACTORY_TEST_RECORD_RECORDING)
    {
        mic_test_start();
        factory_test_earphone_data.record_state = FACTORY_TEST_RECORD_RECORDING;
    }

    mic_test_start();
    // printf("1111111111111\r\n");
}

/**
 * @brief        工厂模式开始播放测试数据
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-05-10
 */
void uteModuleMicRecordFactoryPlayStart(void)
{
#if MIC_DATA_TO_FLASH
    bool mute_bkp;
    bool first_flag = true;
    bsp_change_volume(MIC_TEST_VOL);
    mute_bkp = bsp_get_mute_sta();
    if (mute_bkp)
    {
        bsp_sys_unmute();
    }
    else
    {
        dac_fade_in();
    }
    dac_spr_set(SPR_16000);
    printf("bo >>>>>>>>>>>>>>>>>>[%d] %d\n",mic_cb.mic_data_r_size*2*120,mic_cb.mic_data_r_size);
    for(u32 i = 0; i < mic_cb.mic_data_r_size*120; i++)
    {
        // if((i%8) == 0 && i > 0){
        //         printf("[%d]\n",i*2);
        // }
        // if((i%120 == 0 && (i%8) == 0)&& i > 0){
        //     printf("\n>>>>>>>>>>>>>>>> %d %d\n",i*2,mic_cb.mic_data_r_size*120*2);
        // }
        os_spiflash_read(test_buf,FACTORY_RECORD_DATA_START + i*2,2);

        //printf("%02x %02x ",test_buf[0],test_buf[1]);

        obuf_put_one_sample((s16)((test_buf[1]<<8)+test_buf[0]), (s16)((test_buf[1]<<8)+test_buf[0]));
        WDT_CLR();
    }
    dac_fade_out();
    dac_fade_wait();

    bsp_change_volume(sys_cb.vol);
    if (mute_bkp)
    {
        bsp_sys_mute();
    }
    printf("\n bo <<<<<<<<<<<<<<<<<<<<[%d]\n",mic_cb.mic_data_r_size*2*120);
#endif
}


#if MIC_DATA_TO_FLASH
static void record_ringbuf_init(record_ringbuf_t *ringbuf, char *buf_data, int buf_len)
{
    if (ringbuf == NULL || buf_data == NULL || buf_len == 0)
    {
        return;
    }

    record_ringbuf_t *record_ringbuf = ringbuf;

    if (record_ringbuf->pHead == NULL)
    {
        record_ringbuf->pHead = (char *)buf_data;
    }

    memset(record_ringbuf->pHead, 0, buf_len);

    record_ringbuf->pVaildRead = record_ringbuf->pHead;
    record_ringbuf->pVaildWrite = record_ringbuf->pHead;
    record_ringbuf->pTail = record_ringbuf->pHead + buf_len;
}

AT(.com_text.record_ext)
static void record_ringbuf_write(record_ringbuf_t *ringbuf, char *pBuff, int add_len)
{
    if(ringbuf->pHead == NULL)
    {
        return;
    }

    if (add_len > ringbuf->pTail - ringbuf->pHead)
    {
        return;
    }

    record_ringbuf_t *record_ringbuf = ringbuf;

    if (record_ringbuf->pVaildWrite + add_len > record_ringbuf->pTail)
    {
        int pre_len = record_ringbuf->pTail - record_ringbuf->pVaildWrite; //剩余的空闲空间长度
        int last_len = add_len - pre_len;   //需要覆盖写的数据长度
        memcpy(record_ringbuf->pVaildWrite, pBuff, pre_len);
        memcpy(record_ringbuf->pHead, pBuff + pre_len, last_len);

        record_ringbuf->pVaildWrite = record_ringbuf->pHead + last_len;
    }
    else
    {
        memcpy(record_ringbuf->pVaildWrite, pBuff, add_len);
        record_ringbuf->pVaildWrite += add_len;
    }
}

AT(.com_text.record_ext)
static void record_ringbuf_read(record_ringbuf_t *ringbuf, char *pBuff, int read_len)
{
    if(ringbuf->pHead == NULL)
    {
        TRACE("[%s]:buf is null!", __func__);
        return;
    }

    if (read_len > ringbuf->pTail - ringbuf->pHead)
    {
        TRACE("[%s]:new reed buf size is too long!", __func__);
        return;
    }

    record_ringbuf_t *record_ringbuf = ringbuf;

    if (record_ringbuf->pVaildRead + read_len > record_ringbuf->pTail)
    {
        int pre_len = record_ringbuf->pTail - record_ringbuf->pVaildRead;
        int last_len = read_len - pre_len;
        memcpy(pBuff, record_ringbuf->pVaildRead, pre_len);
        memcpy(pBuff + pre_len, record_ringbuf->pHead, last_len);

        record_ringbuf->pVaildRead = record_ringbuf->pHead + last_len;
    }
    else
    {
        memcpy(pBuff, record_ringbuf->pVaildRead, read_len);
        record_ringbuf->pVaildRead += read_len;
    }
}



void func_record_analysis(void)
{
    if (mic_cb.iis_buf_size == 0)
    {
        return;
    }

    u16 samples = mic_cb.samples*2;

    //printf("\n>>>>>>>>>>>>>>>>>> \n");
#if 0
    for (int i = 0; i < samples; i += 2)   //阻塞播放
    {
        record_ringbuf_read(&record_rx_ringbuf, (char *)test_buf, 2);
        //obuf_put_one_sample((test_buf[0]<<8)+test_buf[1], (test_buf[0]<<8)+test_buf[1]);
        os_spiflash_program(test_buf,FACTORY_RECORD_DATA_START + mic_cb.mic_data_r_size*2,2);
        //printf("%x %x ",test_buf[0],test_buf[1]);
        mic_cb.mic_data_r_size++;
    }
#else
    record_ringbuf_read(&record_rx_ringbuf, (char *)test_buf, samples);
    os_spiflash_program(test_buf,FACTORY_RECORD_DATA_START + mic_cb.mic_data_r_size*samples,samples);
    mic_cb.mic_data_r_size++;
#endif
    //record_ringbuf_read(&record_rx_ringbuf, (char *)test_buf, samples*4);
    //print_r(test_buf,samples);
    //printf("\n<<<<<<<<<<<<<<<<<<< [%d]\n",mic_cb.mic_data_r_size*samples);
    mic_cb.iis_buf_size--;
    //printf("samples %d iis_buf_size %d \n",samples,mic_cb.iis_buf_size);
    //print_r(test_buf,samples);
}

#endif

/**
 * @brief        工厂模式获取录音状态
 * @details
 * @return       uint8_t
 * @author       Wang.Luo
 * @date         2025-05-10
 */
uint8_t uteModuleMicRecordFactoryGetRecordState(void)
{
    return factory_test_earphone_data.record_state;
}

void uteModuleMicRecordFactorySetrecordState(uint8_t state)
{
    factory_test_earphone_data.record_state = state;
}
