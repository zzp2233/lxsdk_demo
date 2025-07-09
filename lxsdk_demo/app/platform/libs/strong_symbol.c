/**********************************************************************
*
*   strong_symbol.c
*   定义库里面部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

#if (UART0_PRINTF_SEL == PRINTF_NONE)
void wdt_irq_init(void) {}
#endif

#if !FUNC_USBDEV_EN
void usb_dev_isr(void) {}
void ude_ep_reset(void) {}
void ude_control_flow(void) {}
void ude_isoc_tx_process(void) {}
void ude_isoc_rx_process(void) {}
void lock_code_usbdev(void) {}
#endif //FUNC_USBDEV_EN

#if !REC_MP3_SUPPORT
void mpa_encode_kick_start(void) {}
void mpa_encode_frame(void) {}
bool mpa_encode_init(u32 spr, u32 nchannel, u32 bitrate)
{
    return false;
}
void mpa_encode_exit(void) {}
#endif

#if !REC_SBC_SUPPORT
bool sbc_encode_init(u8 spr, u8 nch)
{
    return false;
}
u16 sbc_encode_frame(u8 *buf, u16 len)
{
    return 0;
}
void sbc_encode_process(void) {}
void sbc_encode_exit(void) {}
#endif

#if !REC_ADPCM_SUPPORT
void adpcm_encode_process(void) {}
#endif

#if !MUSIC_WAV_SUPPORT
int wav_dec_init(void)
{
    return 0;
}
bool wav_dec_frame(void)
{
    return false;
}
void lock_code_wavdec(void) {}
#endif

#if !MUSIC_WMA_SUPPORT
int wma_dec_init(void)
{
    return 0;
}
bool wma_dec_frame(void)
{
    return false;
}
void lock_code_wmadec(void) {}
#endif

#if !MUSIC_APE_SUPPORT
int ape_dec_init(void)
{
    return 0;
}
bool ape_dec_frame(void)
{
    return false;
}
void lock_code_apedec(void) {}
#endif

#if !MUSIC_FLAC_SUPPORT
int flac_dec_init(void)
{
    return 0;
}
bool flac_dec_frame(void)
{
    return false;
}
void lock_code_flacdec(void) {}
#endif

#if !MUSIC_SBC_SUPPORT
int sbcio_dec_init(void)
{
    return 0;
}
bool sbcio_dec_frame(void)
{
    return false;
}
AT(.sbcdec.code)
void codec_sbcdec_init(void *param) {}
AT(.sbcdec.code)
void codec_sbcdec_update(void) {}
#endif

void bt_music_rec_start(void) {}
void bt_music_rec_stop(void) {}

#if !USB_SUPPORT_EN
void usb_isr(void) {}
void usb_init(void) {}
#endif

#if !MUSIC_UDISK_EN
bool udisk_read(void *buf, u32 lba)
{
    return false;
}
bool udisk_write(void* buf, u32 lba)
{
    return false;
}
bool usb_host_init(void)
{
    return false;
}
bool dev_udisk_activation_try(u8 mode)
{
    return false;
}
bool uhs_wait_pending(uint timeout)
{
    return false;
}
void os_sem_uhs_post(void) {}
void os_sem_uhs_trytake(void) {}
#endif

#if ((!SD_SUPPORT_EN) && (!FUNC_USBDEV_EN))
void sd_disk_init(void) {}
void sdctl_isr(void) {}
void sd_disk_switch(u8 index) {}
bool sd0_stop(bool type)
{
    return false;
}

bool sd0_init(void)
{
    return false;
}
bool sd0_read(void *buf, u32 lba)
{
    return false;
}
bool sd0_write(void* buf, u32 lba)
{
    return false;
}
#endif

#if !FUNC_MUSIC_EN
u32 fs_get_file_size(void)
{
    return 0;
}
void fs_save_file_info(unsigned char *buf) {}
void fs_load_file_info(unsigned char *buf) {}
#endif

#if ((!MUSIC_UDISK_EN) && (!SD_SUPPORT_EN))
FRESULT fs_open(const char *path, u8 mode)
{
    return 0;
}
FRESULT fs_read (void* buff, UINT btr, UINT* br)
{
    return 0;
}
FRESULT fs_lseek (DWORD ofs, u8 whence)
{
    return 0;
}
void os_sem_usb_sd_init(void) {}
#endif

#if (!(MUSIC_WMA_SUPPORT | MUSIC_APE_SUPPORT | MUSIC_FLAC_SUPPORT | MUSIC_WAV_SUPPORT))
void msc_stream_start(u8 *ptr) {}
void msc_stream_end(void) {}
int msc_stream_read(void *buf, unsigned int size)
{
    return 0;
}
bool msc_stream_seek(unsigned int ofs, int whence)
{
    return false;
}
void msc_stream_fill(void) {}
void os_stream_fill(void) {}
void os_stream_read(void) {}
void os_stream_seek(void) {}
void os_stream_end(void) {}
#endif

#if (!USB_SUPPORT_EN && !SD_SUPPORT_EN)
void dev_init(void)
{
    sd_disable();
    usb_disable();
}

AT(.com_text.strong)
bool dev_is_online(u16 dev_num)
{
    return false;
}
#endif

#if !USB_SD_UPDATE_EN
int updatefile_init(const char *file)
{
    return -1;
}
void updateproc(void) {}
#endif

#if !BT_FCC_TEST_EN
void bt_uart_init(void)
{
}
AT(.com_text.stack.uart_isr)
bool bt_uart_isr(void)
{
    return false;
}
#endif

#if !MIC_EQ_EN
AT(.com_text.mic_eq)
void sdadc_pcm_peri_eq(u8 *ptr, u32 samples) {}
#endif

#if !WARNING_WAVRES_PLAY
bool wav_res_stop(void)
{
    return false;
}
void wav_res_play_kick(u32 addr, u32 len) {}
void wav_res_dec_process(void) {}
bool wav_res_is_play(void)
{
    return false;
}
AT(.rodata.wavres.buf)
const int wavres_cb = 0;
#endif

#if !BT_HFP_MSBC_EN
AT(.sbcdec.code)
bool btmsbc_fill_callback(u8 *eptr_in, u8 *eptr_out, u8 *dptr_in, u8 *dptr_out)
{
    return false;
}
void msbc_kick_start(void) {}
void bt_msbc_process(void) {}
void bt_msbc_pcm_output(u16 *output) {}
void msbc_init(void) {}
void msbc_exit(void) {}
bool msbc_encode_init(void)
{
    return false;
}
void msbc_encode_exit(void) {}
u16 bt_msbc_encode_frame(u8 *buf, u16 len)
{
    return 0;
}
#endif

#if (UART0_PRINTF_SEL == PRINTF_NONE)
AT(.com_text.err)
void sys_error_hook(u8 err_no)
{
    WDT_RST();
    while(1);
}

AT(.com_text.exception) WEAK
void exception_isr(void)
{
    sys_error_hook(2);
}
#endif

#if (!BT_SCO_DUMP_EN && !BT_SCO_EQ_DUMP_EN && !BT_SCO_FAR_DUMP_EN)
void sco_dump_init(void) {}
AT(.com_text.sco_dump)
void bt_sco_huart_tx_done(void) {}
AT(.bt_voice.sco_dump)
bool sco_huart_putcs(u8 type, u8 frame_num, void *buf, uint len)
{
    return false;
}
AT(.bt_voice.sco_dump)
void bt_sco_2ch_dump(s16 *in, s16 *out, u32 len) {}
AT(.bt_voice.sco_dump)
void bt_sco_3ch_dump(s16 *near, s16 *far, s16 *out, u32 len) {}
AT(.bt_voice.sco_dump)
void bt_sco_dump(u32 index, s16 *ptr, u32 len) {}
#endif

#if !BT_SCO_MAV_EN
AT(.com_text.mav)
void mav_kick_start(void) {}
void magic_voice_process(void) {}
void magic_voice_init(void *cfg) {}
#endif

#if !BT_SCO_FAR_NR_EN
void nr_far_init(u16 noise_thr, u16 nr_level) {}
void nr_far_process(s16 *data) {}
void bt_sco_far_nr_process(u8 *buf, u32 samples) {}
#endif

#if !DAC_DRC_EN
AT(.com_text.dac.dnr)
void dac_dnr_process(void) {}
#endif

#if !DAC_AUTO_ONOFF_EN
AT(.com_text.dac)
void dac_set_power_on_off(u32 status) {}
#endif

#if !GUI_USE_ARC
typedef void graphics_t;
typedef void dc_t;
typedef void draw_param_t;
typedef void ele_arc_t;
typedef void ele_circle_t;
typedef void widget_circle_t;

//画圆弧接口
AT(.text.element)
bool draw_arc(graphics_t *g, const draw_param_t *param)
{
    return true;
}
AT(.com_text.element)
bool ele_arc_dcout(dc_t *dc, const ele_arc_t *element)
{
    return true;
}
AT(.text.element)
bool draw_circle(graphics_t *g, const draw_param_t *param)
{
    return true;
}
AT(.com_text.element)
bool ele_circle_dcout(dc_t *dc, const ele_circle_t *element)
{
    return true;
}
widget_arc_t *widget_arc_create(widget_page_t *parent)
{
    return NULL;
}
void widget_arc_set_angles(widget_arc_t *arc, u16 start_angle, u16 end_angle) {}
void widget_arc_set_color(widget_arc_t *arc, u16 color_intra, u16 color_outre) {}
void widget_arc_set_alpha(widget_arc_t *arc, u8 alpha_intra, u8 alpha_outre) {}
void widget_arc_set_width(widget_arc_t *arc, u16 arc_width) {}
void widget_arc_set_edge_circle(widget_arc_t *arc, bool onoff) {}
widget_circle_t *widget_circle_create(widget_page_t *parent)
{
    return NULL;
}
void widget_circle_set_acolor(widget_circle_t *circle, u16 color_intra, u8 alpha_intra) {}
#endif

//线程总堆栈
#if MEM_HEAP_SIZE
u8 mem_heap[MEM_HEAP_SIZE] AT(.heap.os);
AT(.text.startup.init)
u32 system_get_mem_heap_size(void)
{
    return MEM_HEAP_SIZE;
}
#endif // MEM_HEAP_SIZE

//MAIN线程栈大小设置
#if OS_THREAD_MAIN_STACK
AT(.text.startup.init)
u32 system_get_main_stack_size(void)
{
    return OS_THREAD_MAIN_STACK;
}
#endif // OS_THREAD_MAIN_STACK

#if OS_THREAD_MAIN_PRIOPITY
AT(.text.startup.init)
u32 system_get_main_stack_priopity(void)
{
    return OS_THREAD_MAIN_PRIOPITY;
}
#endif // OS_THREAD_MAIN_PRIOPITY

//MUSIC线程栈大小设置
#if OS_THREAD_MUSIC_STACK
AT(.text.startup.init)
u32 system_get_music_stack_size(void)
{
    return OS_THREAD_MUSIC_STACK;
}
#endif // OS_THREAD_MUSIC_STACK

#if !OPUS_ENC_EN
void opus_enc_process(void) {};
#endif

#if FLASHDB_EN
u32 onc_addr = FLASH_AB_PARAM_ADDR - 16 * 1024;
size_t onc_size = 16 * 1024;
#endif

#if BT_HID_ONLY_FOR_IOS_EN
bool bt_hid_disable_for_andriod(void)
{
    return true;
}
#endif

//32k-64k擦除
#if !FLASH_ERASE_32K_64K
AT(.com_text.spiflash)
void spiflash_write_enable(void) {}
AT(.com_text.spiflash)
uint spiflash_readssr(void)
{
    return 0;
}
AT(.com_text.spiflash)
void spiflash_waitbusy(void) {}
AT(.com_text.spiflash)
void spiflash_sendaddr(u32 addr) {}
AT(.com_text.spiflash)
void spiflash_erase_select(u32 addr, u8 cmd) {}
AT(.com_text.spiflash)
void os_spiflash_erase_32k(u32 addr) {}
AT(.com_text.spiflash)
void os_spiflash_erase_64k(u32 addr) {}
#endif

#if !FLASH_EXTERNAL_EN
AT(.com_text.spiflash)
bool gui_is_use_ext_flash(void)
{
    return false;
}
AT(.com_text.spiflash)
void gui_set_use_ext_flash(bool en) {}
AT(.com_text.spiflash)
bool spi1flash_read(void *buf, u32 addr, uint len)
{
    return true;
}
AT(.com_text.gui)
bool lnf_read_ex(u32 *lnf_cb, u32 *ln_info, int ln, int cnt)
{
    return false;
}
AT(.com_text.gui)
bool spiflash_read_lninfo_ex(u32 *ln_info, u32 par_idx_start, int data_start, int avg_size, int idx_size, uint ln, uint cnt, uint max_size)
{
    return false;
}
AT(.com_text.gui)
bool de_line_spiflash_depar_kick_ex(void *raw_buf, int raw_len, void *par_buf, int par_size, int first_klen, u32 par_mode, bool flag_par_kick)
{
    return false;
}
#endif

void emit_timer_reset(void) {}

#if !BT_PANU_EN
void btstack_panu_connect(void) {}
void btstack_panu_disconnect(void) {}
void bnep_network_init(void) {}
void bnep_network_up(bd_addr_t addr) {}
void bnep_network_down(void) {}
void bnep_network_process_packet(const uint8_t *packet, uint16_t size) {}
AT(.com_text.stack.run_loop)
void bnep_network_outgoing_process(void) {}
void bnep_network_packet_sent(uint8_t *buf) {}

u8 bnep_get_psm(void)
{
    return 0;
}

void bnep_audio_init(void) {}
void bnep_audio_exit(void) {}
void bnep_audio_write(void *buf, unsigned int size) {}
void bnep_audio_start(void) {}
int web_stream_read(void *buf, unsigned int size)
{
    return 0;
}
bool web_stream_seek(unsigned int ofs, int whence)
{
    return false;
}

AT(.com_text.stack.thread)
bool thread_btstack_get_wdt_clr_en(void)
{
    return false;
}

#endif


#if 0

static u32 tft_timeout_cnt = 0;

AT(.com_text.tft_spi)
void tft_spi_sendbyte(u8 val)
{
    DESPICON &= ~BIT(7);                        //IE DIS
    DESPICON &= ~BIT(4);                        //TX
    DESPIBUF = val;
    u32 ticks = tick_get();
    while (!(DESPICON & BIT(16)))                //Wait pending
    {
        if (tick_check_expire(ticks, 50))
        {
            tft_timeout_cnt++;
            break;
        }
    }
    DESPICPND = BIT(16);                        //Clear pending
    DESPICON |= BIT(7);                         //IE EN
}

AT(.com_text.tft_spi)
u8 tft_spi_getbyte(void)
{
    u8 val;
    DESPICON &= ~BIT(7);                        //IE DIS
    DESPICON |= BIT(4);                         //RX
    DESPIBUF = 0xFF;
    u32 ticks = tick_get();
    while (!(DESPICON & BIT(16)))                //Wait pending
    {
        if (tick_check_expire(ticks, 50))
        {
            tft_timeout_cnt++;
            break;
        }
    }
    val = DESPIBUF;
    DESPICPND = BIT(16);                        //Clear pending
    DESPICON |= BIT(7);                         //IE EN
    return val;
}

AT(.com_text.tft_spi)
void tft_spi_send(void *buf, uint len)
{
    DESPICON &= ~BIT(7);                        //IE DIS
    DESPIDMAADR = DMA_ADR(buf);
    DESPIDMACNT = len;
    u32 ticks = tick_get();
    while ( !(DESPICON & BIT(16)) )
    {
        if (tick_check_expire(ticks, 50))
        {
            tft_timeout_cnt++;
            break;
        }
    }
    DESPICON |= BIT(7);                         //IE EN
}

AT(.com_text.tft_spi)
u8 get_tft_spi_timeout(void)
{
    return tft_timeout_cnt;
}

AT(.com_text.tft_spi)
void clr_tft_spi_timeout(void)
{
    tft_timeout_cnt = 0;
}
#endif
