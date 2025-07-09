#include "config.h"

ENTRY(_start)

/* Define the flash max size */
__max_flash_size = FLASH_CODE_SIZE;

__base = 0x10000000;

/* Define the area */
__bram_vma = 0x10C00; //加大6K个ble_cache
__cram_vma = 0x1D000; //0x1AC00;
__sram_vma = 0x21000; //0x1EC00;
__disp_vma = 0x50000;
__aram_vma = 0x68000;
__dram_vma = 0x6C000;
__eram_vma = 0x6F000;

__comm_vma = __sram_vma;
__sram_size = 197k-6k;
__disp_size = 96k;

//复用6K AINS3的空间
__dnn_vma = __cram_vma + 0x2800;

MEMORY
{
    init            : org = __base,             len = 512
    flash(rx)       : org = __base + 512,       len = __max_flash_size
    dnn_comm(rx)    : org = __dnn_vma,          len = 6k

    stack           : org = 0x10800,            len = 1k
    sram            : org = __sram_vma,         len = __sram_size
    disp            : org = __disp_vma,         len = __disp_size
    aram            : org = __aram_vma,         len = 16k
    bram            : org = __bram_vma,         len = 40k+6k
    cram            : org = __cram_vma,         len = 16k
    dram            : org = __dram_vma,         len = 12k
    eram            : org = __eram_vma,         len = 2560
}

SECTIONS
{
    .init : {
        __code_start_eq = .;
        KEEP(*(.reset))
        *(.text.com_eq.*)
        . = ALIGN(512);
        __code_end_eq = .;
        . = ALIGN(512);
    } > init

    .comm : {
        __comm_start = .;
        KEEP(*(.vector))
        *(.plt)
        *(.com_text.tft_spi)
        *(.com_text*)
        *(.com_text.bb.leisr)
        *(.com_text.stack.handler)
        *(.com_rodata*)
        *(.srodata*)
        *bsp_halt.o(.rodata*)
        *debug.o(.rodata*)
        *ele_image.o(.rodata*)
        *(.data*)
        *(.sdata*)
        *\ext_scan.o(.rodata*)
        *\ble_evt.o(.rodata*)
        *\hci_transport_h3.o(.rodata*)
        *gui.o(.rodata*)
#if 0
        *ute_all_sports_int_algorithms3_0.o(.text*)
        *ute_all_sports_int_algorithms3_0.o(.bss*)
        *ute_all_sports_int_algorithms3_0.o(.sbss*)
        *ute_all_sports_int_algorithms3_0.o(.rodata*)

        *ute_sleep_algorithms_on_band.o(.text*)
        *ute_sleep_algorithms_on_band.o(.bss*)
        *ute_sleep_algorithms_on_band.o(.sbss*)
        *ute_sleep_algorithms_on_band.o(.rodata*)
#endif
        __comm_end = .;
        . = ALIGN(512);
    } > sram AT > flash

    .dnn_comm : {
        *(.dnn_text*)
        *(.dnn_rodata.nn.input*)
        *(.dnn_rodata.rodata*)
        . = ALIGN(512);
    } > dnn_comm AT > flash

    .flash : {
        __code_start_stream = .;
        *(.com_stream.text*)
        . = ALIGN(512);
        __code_end_stream = .;

        __code_start_record = .;
        *(.com_rec*)
        . = ALIGN(512);
        __code_end_record = .;

        __code_start_btdec_com = .;
        *(.btdec*)
        . = ALIGN(512);
        __code_end_btdec_com = .;

        __code_start_mp3dec = .;
        *(.mp3dec*)
        . = ALIGN(512);
        __code_end_mp3dec = .;

        __code_start_sbcdec = .;
        *(.sbcdec*)
        *(.sbc_com*)
        . = ALIGN(512);
        __code_end_sbcdec = .;

        __code_start_sbcenc = .;
        *(.rodata.sbcenc*)
        *(.text.sbcenc*)
        . = ALIGN(512);
        __code_end_sbcenc = .;

        __code_start_mp3enc = .;
        *(.text.mp3enc*)
        *(.rodata.mp3enc*)
        . = ALIGN(512);
        __code_end_mp3enc = .;

        __code_start_mp2enc = .;
        *(.text.mp2enc*)
        *(.rodata.mp2enc*)
        . = ALIGN(512);
        __code_end_mp2enc = .;

        __code_start_wavdec = .;
        *(.wavdec*)
        . = ALIGN(512);
        __code_end_wavdec = .;

        __code_start_wmadec = .;
        *(.wmadec.bitstream)
        *(.wmadec.windows)
        *(.wmadec*)
        . = ALIGN(512);
        __code_end_wmadec = .;

        __code_start_apedec = .;
        *(.apedec*)
        . = ALIGN(512);
        __code_end_apedec = .;

        __code_start_flacdec = .;
        *(.flacdec*)
        . = ALIGN(512);
        __code_end_flacdec = .;

        __code_start_usbdev = .;
        *(.usbdev*)
        . = ALIGN(512);
        __code_end_usbdev = .;

        __code_start_wma_rlc44q = .;
        *(.wma_huff_rlc44q_table)
        . = ALIGN(512);
        __code_end_wma_rlc44q = .;

        __code_start_wma_rlc44o = .;
        *(.wma_huff_rlc44o_table)
        . = ALIGN(512);
        __code_end_wma_rlc44o = .;

        __code_start_wma_rlc16 = .;
        *(.wma_huff_rlc16_table)
        . = ALIGN(512);
        __code_end_wma_rlc16 = .;

        __code_start_sniff = .;
        *(.text.sniff.bb.entry)
        . = ALIGN(512);
        __code_end_sniff   = .;

        __code_start_sleep = .;
        *(.saradc_text*)
        *(.sleep_text*)
        *(.text.sleep.rf)
        *(.text.charge_com*)
#if 0
        *modle5_1.o(.text*)
        *modle5_2.o(.text*)
        *modle5_3.o(.text*)
        *modle5_4.o(.text*)
        *modle5_5.o(.text*)
        *modle5_6.o(.text*)
        *modle5_7.o(.text*)
        *modle5_8.o(.text*)
        *modle5_9.o(.text*)
        *modle5_10.o(.text*)
        *modle5_11.o(.text*)
        *modle5_12.o(.text*)
        *modle5_13.o(.text*)
        *modle5_14.o(.text*)
        *modle5_15.o(.text*)
        *modle5_16.o(.text*)
        *modle5_17.o(.text*)
        *modle5_18.o(.text*)
        *modle5_19.o(.text*)
        *modle5_20.o(.text*)
        /* *spo2Algo.o(.text*)
        *connect_ir.o(.text*)
        *connect_r.o(.text*)
        *hp_filter_ir.o(.text*)
        *hp_filter_r.o(.text*)
        *spo2_cal.o(.text*)
        *acPower_add_doublefre.o(.text*)
        *average_value.o(.text*)
        *calculate_equivalent_margin.o(.text*)
        *corresponding_point_in_freq.o(.text*)
        *pick_invalid_groups.o(.text*)
        *frequency_spectrum.o(.text*)
        *pick_invalid_groups_add.o(.text*)
        *spectrum_concentration.o(.text*)
        *spo2Algspectrum_concentrationo.o(.text*)
        *freq_production.o(.text*) */
        /* *spo2Algo.o(.text*)
        *spo2Algo.o(.text*)
        *spo2Algo.o(.text*)
        *spo2Algo.o(.text*) */
#endif
        . = ALIGN(512);
        __code_end_sleep = .;

        __code_start_pwrsave = .;
        *(.text.pwroff*)
        . = ALIGN(512);
        __code_end_pwrsave = .;

        __code_start_bb_test = .;
        *(.bb_test*)
        *(.bb_test_rodata*)
        . = ALIGN(512);
        __code_end_bb_test = .;

        __code_start_bt_voice = .;
        *(.bt_voice*)
        . = ALIGN(512);
        __code_end_bt_voice = .;

        __code_start_bt_aec = .;
        *(.text.aec.*)
        . = ALIGN(512);
        __code_end_bt_aec = .;

        __code_start_bt_alc = .;
        *(.text.alc.*)
        . = ALIGN(512);
        __code_end_bt_alc = .;

        __code_start_nr_far = .;
        *(.text.ains_far.*)
        *(.rodata.ains_far.*)
        . = ALIGN(512);
        __code_end_nr_far  = .;

        __code_start_wsola = .;
        *(.text.mav.*)
        *(.rodata.mav.*)
        . = ALIGN(512);
        __code_end_wsola   = .;

         __code_start_piano = .;
        *(.piano_com*)
        . = ALIGN(512);
        __code_end_piano = .;

        __code_start_fota = .;
        *(.text.fot.cache*)
        . = ALIGN(512);
        __code_end_fota = .;

        __code_start_opus = .;
        *(.text.opus.asm)
        *(.text.opus.enc.asm)
        *(.text.opus.proc)
        *(.text.opus.enc.proc)
        *(.text.opus.ec_tell)
        *(.text.opus.enc.entenc)
        *(.text.opus.mathops)
        *(.rodata.opus*)
        . = ALIGN(512);
        __code_end_opus = .;

        __code_start_update = .;
        *(.text.update*)
        . = ALIGN(512);
        __code_end_update = .;

        __code_start_rf = .;
        *(.text.rf_init*)
        . = ALIGN(512);
        __code_end_rf = .;

        __code_start_ains3 = .;
        *(.ains_text*)
        *(.ains_rodata*)
        . = ALIGN(512);
        __code_end_ains3 = .;

        __code_start_dnn = .;
        *(.dnn_rodata*)
         . = ALIGN(512);
        __code_end_dnn = .;

        #if (ASR_SELECT == ASR_WS_AIR)
        __code_start_ir = .;
        *(.kws_ir*)
        . = ALIGN(512);
        __code_end_ir = .;

        *(.ws_text.tdnn_weight_1)
        *(.ws_text.tdnn_weight_2)
        *(.ws_text.tdnn_weight_2)
        *(.ws_text.tdnn_weight_3)
        *(.ws_text.tdnn_weight_4)
        *(.ws_text.tdnn_weight_5)
        *(.ws_text.tdnn_weight_6)
        *(.ws_text.tdnn_weight_7)
        #endif

        *(.irq_init.aligned)
        *(.text*)
        __code_end_all = .; //USER_APP_USE
        *(.rodata*)
        *(.rela*)
        LONG(0)
        . = ALIGN(512);

    } > flash

    .stack (NOLOAD) : {
        __irq_stack_start = .;
        . = 0x400;
        __irq_stack = .;
    } > stack
    __irq_stack_size = __irq_stack - __irq_stack_start;

    .disp (NOLOAD) : {
        __disp_start = .;
        *(.disp.*)
        __disp_end = .;
    } > disp
    __disp_stack_size = __disp_end - __disp_start;

    .bss (NOLOAD) : {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        *(.sbss*)
        *(.buf*)
        *(.dac_obuf)
        *(.sdadc_buf)
        *(.mavsco_buf*)
        *(.nr_far_buf.*)
        *(.heap.os)
        *(.heap.func)
#if BT_SCO_APP_DBG_EN
        *(.nr_buf*)
#endif // BT_SCO_APP_DBG_EN
        *(.dnn_data*)
       __hr_start = .;

#if 0
        __hr_3300_3605_start = .;
        *algo.o(.bss*)
        *modle5_13.o(.bss*)
        *modle5_17.o(.bss*)
        *modle5_18.o(.bss*)
        *modle5_2.o(.bss*)
        *modle5_5.o(.bss*)
        *modle5_6.o(.bss*)
        *spo2Algo.o(.bss*)
        *spo2_cal.o(.bss*)
        *acPower_add_doublefre.o(.bss*)
        *(.hrs3300_buf)

        *(.hr_buf)
        *(.hr_data*)
        *hx3605_hrs_agc.o(.bss*)
        *hx3605_spo2_agc.o(.bss*)
        __hr_3300_3605_stop = .;
#endif

        __hr_stop = .;
        . = ALIGN(512);
        __heap_custom_start = .;
        *(.heap.custom)

         __asr_buf_start = .;
        #if (ASR_SELECT == ASR_WS)
            *(.ws_asr.buf)
            *(.ws_asr.sum)
            *(.ws_asr.feat)
            *(.ws_asr*)
            *(.asr_dump*)
            *(.ws_asr.test)
            *(.asr_record)
            *(.asr_sco)
            *(.ws_text.strong)
        #elif (ASR_SELECT == ASR_YJ)
            *(.asr.matrix)
            *(.asr.buff)
            *(.asr.pieces)
            *(.asr.save)
            *(.asr*)
            *(.asr_record)
	    	*(.asr_sco)
        #elif (ASR_SELECT == ASR_WS_AIR)
            *(.ws_asr.buf)
            *(.ws_asr.sum)
            *(.ws_asr.feat)
            *(.ws_asr*)
            *(.asr_dump*)
            *(.ws_asr.test)
            *(.asr_record)
	    	*(.asr_sco)
            *(.ws_text.strong)

            *(.ir_prd.rcv)
            *(.ir_rmt.dat)
            *(.ir_pwm.out)
        #endif
        __asr_buf_end = .;

        __bss_end = .;
    } > sram
    __bss_size = __bss_end - __bss_start;
    __asr_buf_size = __asr_buf_end - __asr_buf_start;

    .bram __bram_vma (NOLOAD) : {
        __bram_start = .;
        *(.btmem.bthw)
        *(.btmem*)
        *(.btstack_hci.buf)
        *(.ble_buf*)
        *(.ble_cache*)
        *(.ancs_client*)
        *(.ams_client*)
        *(.rtc_cache)
        __bram_end = .;
    } > bram
    __bram_size = __bram_end - __bram_start;


    .a2dp_cram __cram_vma (NOLOAD) : {
        . = 0x660;
        *(.a2dp_play*)
        *(.a2dp_cache1)
        *(.bt_id3_buf)
        . = 0x1200;
    } > cram

    .sco_cram __cram_vma (NOLOAD) : {
        *(.sbcbuf.dec)
        . = 0x660;                      //通话msbc解码会用到, 故不能复用
        *(.sbcenc.buf)
        . = 0x900;
        *(.sco_data*)
        *(.sco_cache.buf)
        *(.sco_hci)
        *(.plc_buf)
        *(.sco_dump*)
        . = 0x2800;
        *(.ains_buf*)
    } > cram

    .music_cram __cram_vma (NOLOAD) : {
        . = 0x1200;
        *(.avio_buf)
        *(.diskio_buf)
        *(.usb_buf.*)
        *(.fname.buf)
        *(.pff.buf)
        *(.pff_buf*)
        *(.udev_buf*)
        *(.msc_stm_buf)
        *(.sbcio_buf.dec)
        *(.rec_buf.wav)
        *(.adpcm_buf*)
    } > cram

    .aec_ram __aram_vma (NOLOAD) : {
        *(.aec_buf)
        . = 0x4000;
    } > aram

    .aram_sbc __aram_vma (NOLOAD) : {
        *(.sbc_cache_buf)
        . = 0x4000;
    } > aram

    .aram_ctp __aram_vma (NOLOAD) : {
        *(.ctp_ota*)
        . = 0x4000;
    } > aram

    .aram_music __aram_vma (NOLOAD) : {
        *(.mp3buf.dec)
        *(.mp3_id3_buf)
        *(.wavres.buf)
        *(.lrc_buf*)
        *(.pff.scan.tempbuf)
        . = 0x4000;
    } > aram

    .aram_wma __aram_vma (NOLOAD) : {
        *(.wma_coefs_buf0)
        *(.wma_coefs_buf)
        *(.wma_ctx_buf)
        *(.wma_wincb_buf)
        *(.wma_id3_buf)
    } > aram

    .aram_flac __aram_vma (NOLOAD) : {
        *(.flac_decoded_buf)
    } > aram

    .aram_mp2enc __aram_vma (NOLOAD) : {
        *(.mp2enc_buf.*)
        . = 0x4000;
    } > aram

    .aram_mp3enc __aram_vma (NOLOAD) : {
        *(.mp3enc_buf.*)
        . = 0x4000;
    } > aram

    .aram_fcc __aram_vma (NOLOAD) : {
        . = 0x1000;
        *(.fcc_buf.huart)
        *(.fcc_buf*)
    } > aram

    .aram_contacts __aram_vma (NOLOAD) : {
        *(.kvdb_buf*)
    } > aram

    .dram_upd __dram_vma (NOLOAD) : {
        . = 0x2800;
        *(.upd_buf*)
        *(.ota_buf*)
        . = 0x3000;
    } > dram

    .aram_opus __aram_vma (NOLOAD) : {
        *(.opus_buf.stack)
    } > aram

#if MIC_TEST_EN
    .aram_mic_test __aram_vma (NOLOAD) : {
        //. = 0x600;
        *(.mic_test_buf)
        . = 0x4000;
    } > aram
#endif // MIC_TEST_EN

    /*
    .aram_alipay __aram_vma (NOLOAD) : {
        *(.qr_buff)
        . = 0x4000;
    } > aram
    */

    .dram_mp3 __dram_vma (NOLOAD) : {
        *(.mp3_decram_buf)
        . = 0x3000;
    } > dram

    .dram_ape __dram_vma (NOLOAD) : {
        *(.apebuf.cb)
        *(.apebuf.filter)
        *(.apebuf.decode)
        *(.apebuf.seektbl)
        . = 0x3000;
    } > dram

    .dram_wma __dram_vma (NOLOAD) : {
        *(.wmabuf*)
        . = 0x3000;
    } > dram

    .dram_flac __dram_vma (NOLOAD) : {
        *(.flacbuf*)
        *(.flac_decoded_buf1)
        *(.wavbuf.dec)
        . = 0x3000;
    } > dram

    .dram_sbc __dram_vma (NOLOAD) : {
        *(.sbc_decram_buf)
        . = 0xD38;
     }  > dram

     .dram_sco __dram_vma (NOLOAD) : {
        *(.wavenc.sco)
        . = 0xD38;                              //0xd38之前为msbc解码空间
        *(.tsco_cache)
        *(.sco_cache*)
        *(.sco_data*)
        *(.sco_hci)
        *(.plc_buf)
        *(.plc_data)
        . = 0x3000;
    } > dram

    .dram_rec __dram_vma (NOLOAD) : {
        . = 0xD38;
        *(.rec_buf.*)
        . = 0x3000;
    } > dram

    .dram_opus_enc __dram_vma (NOLOAD) : {
        *(.opus.buf.bsp)
        *(.opus_buf.input)
        *(.opus_buf.enc*)
        . = 0x3000;
    } > dram

    .dram_opus_dec __dram_vma (NOLOAD) : {
        *(.opus_buf.dec*)
    } > dram

    .dram_usb __dram_vma (NOLOAD) : {
        *(.usb.device.desc.buf)
        *(.ude.aubuf)
        . = 0x3000;
    } > dram

    .dram_qr __dram_vma (NOLOAD) : {
        . = 0xD38;
        *(.qr_buff)
        . = 0x3000;
    } > dram

    .dram_qr_ex __dram_vma (NOLOAD) : {
        *(.qr_ex_buff)
        . = 0x3000;
    } > dram

    .dram_fot __dram_vma (NOLOAD) : {
        *(.fot_data.*)
    } > dram

    .eram_mp3enc __eram_vma (NOLOAD) : {
        *(.mp3_enc_hwbuf)
    } > eram
}

/* Calc the lma */
__bank_size = SIZEOF(.flash);
__comm_lma = LOADADDR(.comm);
__comm_size = SIZEOF(.comm);
__dnn_lma = LOADADDR(.dnn_comm);
__dnn_size = SIZEOF(.dnn_comm);
