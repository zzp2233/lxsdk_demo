#include "include.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define FACTORY_RECORD_DATA_START (UTE_OTA_TMP_ADDRESS + 100 * 1024)
#if MIC_TEST_EN


#define MIC_TEST_VOL        (VOL_MAX / 2)
#define MIC_BUF_IS_FULL     mic_cb.rec_buf_full

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

#endif  //MIC_TEST_EN

///---------------------------------------[UI]--------------------------------------------

#define COLOR_DARKGREEN    make_color(90, 170, 90)

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_START = 1,
    COMPO_ID_BTN_EXIT,
    COMPO_ID_BTN_PLAY,
    //矩形
    COMPO_ID_RECT_START,
    COMPO_ID_RECT_EXIT,
    COMPO_ID_RECT_PLAY,
};

//mic录音状态
enum
{
    MIC_TEST_STA_EMPTY,
    MIC_TEST_STA_RECORDING,
    MIC_TEST_STA_FULL,
};

typedef struct f_mic_test_t_
{
    u8 sta;
    bool play_need;
} f_mic_test_t;

//创建麦克风测试窗体
compo_form_t *func_mic_test_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建矩形
    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_setid(shape, COMPO_ID_RECT_START);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);
    compo_shape_set_radius(shape, 6);
    compo_shape_set_color(shape, MIC_TEST_EN ? COLOR_DARKGREEN : COLOR_GRAY);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_setid(shape, COMPO_ID_RECT_EXIT);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);
    compo_shape_set_radius(shape, 6);
    compo_shape_set_color(shape, COLOR_DARKGREEN);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_setid(shape, COMPO_ID_RECT_PLAY);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 4 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);
    compo_shape_set_radius(shape, 6);
//    compo_shape_set_color(shape, MIC_BUF_IS_FULL ? COLOR_DARKGREEN : COLOR_GRAY);
    compo_shape_set_color(shape, COLOR_GRAY);

    //创建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 8);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT / 5);
    compo_textbox_set(txt, "mic-test");

    txt = compo_textbox_create(frm, 8);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 5);
    compo_textbox_set(txt, "start");

    txt = compo_textbox_create(frm, 8);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 5);
    compo_textbox_set(txt, "exit");

    txt = compo_textbox_create(frm, 8);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 4 / 5);
    compo_textbox_set(txt, "play");

    //创建按钮
    compo_button_t *btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_START);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_EXIT);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_PLAY);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 4 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);

    return frm;
}

//麦克风测试功能事件处理
static void func_mic_test_process(void)
{
#if MIC_DATA_TO_FLASH
    func_record_analysis();
#endif
    func_process();

    f_mic_test_t *f_mic_test = (f_mic_test_t *)func_cb.f_cb;

    if (f_mic_test->play_need)
    {
#if !MIC_DATA_TO_FLASH
        mic_test_outp();
#endif
        compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_RECT_START), COLOR_DARKGREEN);
        compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_RECT_EXIT), COLOR_DARKGREEN);
        compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_RECT_PLAY), COLOR_DARKGREEN);
        f_mic_test->play_need = false;
    }

    if (f_mic_test->sta == MIC_TEST_STA_RECORDING && MIC_BUF_IS_FULL)
    {
        compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_RECT_EXIT), COLOR_GRAY);
        f_mic_test->sta = MIC_TEST_STA_FULL;
        f_mic_test->play_need = true;
        //  printf(">>>>>>>>>>>>>>>----------- MIC_TEST_STA_FULL [%d]\n",mic_cb.rec_datalen);
    }
}

//单击按钮
static void func_mic_test_button_click(void)
{
    f_mic_test_t *f_mic_test = (f_mic_test_t *)func_cb.f_cb;
    compo_shape_t *shape_start = compo_getobj_byid(COMPO_ID_RECT_START);
    compo_shape_t *shape_exit = compo_getobj_byid(COMPO_ID_RECT_EXIT);
    compo_shape_t *shape_play = compo_getobj_byid(COMPO_ID_RECT_PLAY);
    int id = compo_get_button_id();
    switch(id)
    {
        case COMPO_ID_BTN_START:
            if (MIC_TEST_EN && f_mic_test->sta != MIC_TEST_STA_RECORDING)
            {
                compo_shape_set_color(shape_start, COLOR_GRAY);
                compo_shape_set_color(shape_play, COLOR_GRAY);
                mic_test_start();
                f_mic_test->sta = MIC_TEST_STA_RECORDING;
            }
            break;

        case COMPO_ID_BTN_EXIT:
            func_directly_back_to();
            break;

        case COMPO_ID_BTN_PLAY:
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

            if (f_mic_test->sta == MIC_TEST_STA_FULL)
            {
                compo_shape_set_color(shape_start, COLOR_GRAY);
                compo_shape_set_color(shape_exit, COLOR_GRAY);
                compo_shape_set_color(shape_play, COLOR_GRAY);
                f_mic_test->play_need = true;
            }
        }
        break;

        default:
            break;
    }
}

//麦克风测试功能消息处理
static void func_mic_test_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_mic_test_button_click();
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入麦克风测试控制功能
static void func_mic_test_enter(void)
{
#if (ASR_SELECT && ASR_FULL_SCENE)
    bsp_asr_stop();
#endif

    mic_test_init();

    func_cb.f_cb = func_zalloc(sizeof(f_mic_test_t));
    func_cb.frm_main = func_mic_test_form_create();

    f_mic_test_t *f_mic_test = (f_mic_test_t *)func_cb.f_cb;
    f_mic_test->sta = MIC_BUF_IS_FULL ? MIC_TEST_STA_FULL : MIC_TEST_STA_EMPTY;
#if MIC_DATA_TO_FLASH
    record_ringbuf_init(&record_rx_ringbuf, (char *)mic_cb.rec_buf, sizeof(mic_cb.rec_buf));
#endif
}

//退出麦克风测试功能
static void func_mic_test_exit(void)
{
    mic_test_exit();

}

//麦克风测试功能
void func_mic_test(void)
{
    printf("%s\n", __func__);
    func_mic_test_enter();

    while (func_cb.sta == FUNC_MIC_TEST)
    {
        func_mic_test_process();
        func_mic_test_message(msg_dequeue());
    }
    func_mic_test_exit();
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
