#include "include.h"
#include "func.h"
#include "func_usbdev.h"

#if FUNC_USBDEV_EN

enum {
    COMPO_ID_BTN_PREV = 1,
    COMPO_ID_BTN_NEXT,
    COMPO_ID_BTN_PLAY,
    COMPO_ID_BTN_VOL_UP,
    COMPO_ID_BTN_VOL_DOWN,
    COMPO_ID_TXT_MUSIC_NAME,
};

typedef struct {
    u8 vol;
    u8 cur_dev;
    u8 dev_change;
    u8 rw_sta;
    u8 db_level;
} f_ude_t;

void uda_set_by_pass(u8 bypass);
u32 ude_volume_to_db(u8 vol);
void ude_set_sys_volume_do(u8 level);

//创建USB DEVICE播放器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_usbdev_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MUSIC]);

    //text
    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 100, 0, 0);
    compo_textbox_set_autosize(name_txt, true);
    compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);
    compo_textbox_set(name_txt, "USB DEVICE");

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_PREV_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_PREV);
    compo_button_set_pos(btn, 53, 248);

    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_PLAY_BIN);
    compo_setid(btn, COMPO_ID_BTN_PLAY);
    compo_button_set_pos(btn, 160, 245);

    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_NEXT_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_NEXT);
    compo_button_set_pos(btn, 267, 248);

    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_VOLUME_DOWN_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_VOL_DOWN);
    compo_button_set_pos(btn, 62, 340);

    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_VOLUME_UP_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_VOL_UP);
    compo_button_set_pos(btn, 258, 340);

    return frm;
}

//触摸单击按钮
static void func_usbdev_button_click(void)
{
    int id = compo_get_button_id();
    switch (id) {
    case COMPO_ID_BTN_PREV:
        ude_hid_key_send(UDE_HID_PREVFILE, 1);
        break;

    case COMPO_ID_BTN_NEXT:
        ude_hid_key_send(UDE_HID_NEXTFILE, 1);
        break;

    case COMPO_ID_BTN_PLAY:
        ude_hid_key_send(UDE_HID_PLAYPAUSE, 1);
        break;

    case COMPO_ID_BTN_VOL_UP:
        ude_hid_key_send(UDE_HID_VOLUP, 1);
        break;

    case COMPO_ID_BTN_VOL_DOWN:
        ude_hid_key_send(UDE_HID_VOLDOWN, 1);
        break;

    default:
        break;
    }
}


//本地音乐消息处理
static void func_usbdev_message(size_msg_t msg)
{
    f_ude_t *f_ude = (f_ude_t *)func_cb.f_cb;

    switch (msg) {
    case MSG_CTP_CLICK:
        func_usbdev_button_click();                         //单击按钮
        break;

    case EVT_PC_REMOVE:
        func_cb.sta = FUNC_NULL;
        break;

#if UDE_STORAGE_EN
    case EVT_SD_INSERT:
        ude_sdcard_insert(DEV_SDCARD);
        break;

    case EVT_SD_REMOVE:
        ude_sdcard_remove(DEV_SDCARD);
        break;
#endif // UDE_STORAGE_EN

    case EVT_UDE_SET_VOL:
        ude_set_sys_volume_do(f_ude->db_level);
        break;

    default:
        func_message(msg);
        break;
    }
}

AT(.text.func.usbdev)
void ude_mic_start(void)
{
    audio_path_init(AUDIO_PATH_USBMIC);
    audio_path_start(AUDIO_PATH_USBMIC);
    dac_spr_set(SPR_48000);
}

AT(.text.func.usbdev)
void ude_mic_stop(void)
{
    audio_path_exit(AUDIO_PATH_USBMIC);
}

AT(.usbdev.com)
u8 ude_get_sys_volume(void)
{
    f_ude_t *f_ude = (f_ude_t *)func_cb.f_cb;
    return f_ude->db_level;
}

AT(.usbdev.com)
void ude_set_sys_volume(u8 vol)
{
    f_ude_t *f_ude = (f_ude_t *)func_cb.f_cb;
    if (f_ude->db_level != vol) {
        f_ude->db_level = vol;
        msg_enqueue(EVT_UDE_SET_VOL);
    }
}

AT(.usbdev.com)
u16 ude_get_vol_db_value(u8 db_level)
{
    return dac_dvol_tbl_db[db_level];
}

u32 ude_volume_to_db(u8 vol)
{
    u32 level;
    if (vol > VOL_MAX) {
        vol = VOL_MAX;
    }
    level = dac_dvol_table[vol];
    return 60 - level;
}

void ude_set_sys_volume_do(u8 level)
{
    s32 db_num;
    db_num = 60 - level;
    if (db_num > 60) {
        db_num = 60;
    }
    if (db_num < 0) {
        db_num = 0;
    }
    dac_set_dvol(dac_dvol_tbl_db[db_num]);
}

#if UDE_STORAGE_EN
AT(.text.func.usbdev)
void ude_sdcard_insert(u8 dev)
{
    f_ude_t *f_ude = (f_ude_t *)func_cb.f_cb;
    if (f_ude->cur_dev != dev) {
        ude_sd_remove();
        f_ude->cur_dev = dev;
        fsdisk_callback_init(f_ude->cur_dev);
        sd0_init();
    } else {
        sd0_init();
    }
}

AT(.text.func.usbdev)
void ude_sdcard_remove(u8 dev)
{
    f_ude_t *f_ude = (f_ude_t *)func_cb.f_cb;
    if (f_ude->cur_dev != dev) {
        ude_sd_remove();
        f_ude->cur_dev = 0xff;
    }
}
#endif // UDE_STORAGE_EN

AT(.text.func.usbdev)
void func_usbdev_process(void)
{
    func_process();
    usb_device_process();
}

static void func_usbdev_enter(void)
{
    f_ude_t *f_ude;
    if (!dev_is_online(DEV_USBPC)) {
        func_cb.sta = FUNC_NULL;
        return;
    }

#if BT_BACKSTAGE_MUSIC_EN
    bt_audio_bypass();
    if (bt_is_playing()) {
        bt_music_pause();
    }
#endif
    func_cb.f_cb = func_zalloc(sizeof(f_ude_t));
    func_cb.frm_main = func_usbdev_form_create();
    func_cb.mp3_res_play = NULL;
    f_ude = (f_ude_t *)func_cb.f_cb;
    f_ude->cur_dev = 0xff;

#if UDE_SPEAKER_EN
    adpll_spr_set(1);
    dac_spr_set(SPR_48000);         //samplerate 48K
    DACDIGCON0 |= BIT(6);           //Src0 Sample Rate Synchronization Enable
    f_ude->db_level = ude_volume_to_db(VOL_MAX);
    bsp_change_volume(VOL_MAX);
    dac_fade_in();
    aubuf0_gpdma_init(0);
#endif // UDE_SPEAKER_EN

#if UDE_STORAGE_EN
    if (dev_is_online(DEV_SDCARD)) {
        f_ude->cur_dev = DEV_SDCARD;
        fsdisk_callback_init(f_ude->cur_dev);
        fs_mount();
    }
#endif // UDE_STORAGE_EN
    usb_device_enter(UDE_ENUM_TYPE);
}

static void func_usbdev_exit(void)
{
    usb_device_exit();
#if UDE_SPEAKER_EN
    DACDIGCON0 &= ~BIT(6);
    adpll_spr_set(DAC_OUT_SPR);
    bsp_change_volume(sys_cb.vol);
    dac_fade_out();
#endif // UDE_SPEAKER_EN
#if UDE_STORAGE_EN
    if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
        sd0_stop(1);
    }
#endif // UDE_STORAGE_EN
    func_cb.last = FUNC_USBDEV;
#if BT_BACKSTAGE_MUSIC_EN
    bt_audio_enable();
#endif
    if (func_cb.sta == FUNC_NULL) {
        func_back_to();
    }
}

AT(.text.func.usbdev)
void func_usbdev(void)
{
    printf("%s\n", __func__);

    func_usbdev_enter();

    while (func_cb.sta == FUNC_USBDEV) {
        func_usbdev_process();
        func_usbdev_message(msg_dequeue());
    }

    func_usbdev_exit();
}
#else
AT(.text.func.usbdev)
void ude_mic_start(void){}
AT(.text.func.usbdev)
void ude_mic_stop(void){}
AT(.usbdev.com)
u8 ude_get_sys_volume(void){return 0;}
AT(.usbdev.com)
void ude_set_sys_volume(u8 vol){}
#endif // FUNC_USBDEV_EN
