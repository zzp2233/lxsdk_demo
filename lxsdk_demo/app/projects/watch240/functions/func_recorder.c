#include "include.h"
#include "func.h"
#include "func_recorder.h"

#if FUNC_RECORDER_EN

enum {
    COMPO_ID_BTN_PLAY = 1,
    COMPO_ID_TXT_MUSIC_NAME,
    COMPO_ID_TXT_TIME_NAME,
};

typedef struct {
    u8 start;
    u32 start_sec;
    u32 frame_cnt;
} f_rec_t;

extern char fname_buf[100];

void func_recorder_set_play_btn_pic(void)
{
    f_rec_t *f_rec = (f_rec_t *)func_cb.f_cb;
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    if (f_rec->start) {
        compo_button_set_bgimg(btn, UI_BUF_MUSIC_PAUSE_BIN);
    } else {
        compo_button_set_bgimg(btn, UI_BUF_MUSIC_PLAY_BIN);
    }
}

void func_recorder_set_title_name(void *buf, u16 id)
{
    compo_textbox_t *tilte_txt = compo_getobj_byid(id);
    compo_textbox_set(tilte_txt, buf);
}

void func_recorder_set_time_text(u32 cur_sec)
{
    char time_str[12];
    tm_t tm;
    f_rec_t *f_rec = (f_rec_t *)func_cb.f_cb;

    if (f_rec->start) {
        compo_textbox_t *tilte_txt = compo_getobj_byid(COMPO_ID_TXT_TIME_NAME);
        tm = time_to_tm(cur_sec - f_rec->start_sec);
        sprintf(time_str, "%02d:%02d:%02d", tm.hour, tm.min, tm.sec);
        compo_textbox_set(tilte_txt, time_str);
    }
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_recorder_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_RECORDER]);

    //text
    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 80, 0, 0);
    compo_textbox_set_autosize(name_txt, true);
    compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);
    compo_textbox_set(name_txt, "  ");

    compo_textbox_t *time_txt = compo_textbox_create(frm, 12);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 20, 0, 0);
    compo_textbox_set_autosize(time_txt, true);
    compo_setid(time_txt, COMPO_ID_TXT_TIME_NAME);
    compo_textbox_set(time_txt, "  ");

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_PLAY_BIN);
    compo_setid(btn, COMPO_ID_BTN_PLAY);
    compo_button_set_pos(btn, 160, 300);

    return frm;
}

//触摸单击按钮
static void func_recorder_button_click(void)
{
    f_rec_t *f_rec = (f_rec_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id) {

    case COMPO_ID_BTN_PLAY:
        if (!f_rec->start) {
            bsp_record_start();
            f_rec->start = 1;
            f_rec->start_sec = RTCCNT;
            func_recorder_set_title_name(fname_buf, COMPO_ID_TXT_MUSIC_NAME);
        } else {
            bsp_record_stop();
            f_rec->start = 0;
            func_recorder_set_title_name("  ", COMPO_ID_TXT_MUSIC_NAME);
            func_recorder_set_title_name("  ", COMPO_ID_TXT_TIME_NAME);
        }
        func_recorder_set_play_btn_pic();
        break;

    default:
        break;
    }
}

//本地音乐消息处理
static void func_recorder_message(size_msg_t msg)
{
    switch (msg) {
    case MSG_CTP_CLICK:
        func_recorder_button_click();                       //单击按钮
        break;

    case MSG_SYS_500MS:
        func_recorder_set_time_text(RTCCNT);                //录音时间
        break;

    default:
        func_message(msg);
        break;
    }
}

AT(.com_text.func.recorder)
void recorder_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
#if FUNC_REC_EN
    f_rec_t *f_rec = (f_rec_t *)func_cb.f_cb;
    if (f_rec->start) {
        if (f_rec->frame_cnt < 4) {
            f_rec->frame_cnt++;
        }
        if (f_rec->frame_cnt > 3) {
            puts_rec_obuf(ptr, (u16)(samples << (1 + ch_mode)));
        }
    }
#endif

//#if MIC_EQ_EN
//    sdadc_pcm_peri_eq(ptr,samples);
//#endif
//    sdadc_pcm_2_dac(ptr, samples, ch_mode);               //推DAC播放
}

#if FUNC_REC_EN
void mic_rec_start(void)
{
    f_rec_t *f_rec = (f_rec_t *)func_cb.f_cb;
    audio_path_init(AUDIO_PATH_RECORDER);
    audio_path_start(AUDIO_PATH_RECORDER);
    f_rec->frame_cnt = 0;
}

void mic_rec_stop(void)
{
    audio_path_exit(AUDIO_PATH_RECORDER);
}
#endif

void func_recorder_process(void)
{
    func_process();
    bsp_record_process();
}

static void func_recorder_enter(void)
{
    f_rec_t *f_rec;
    func_cb.f_cb = func_zalloc(sizeof(f_rec_t));
    func_cb.frm_main = func_recorder_form_create();
    f_rec = (f_rec_t *)func_cb.f_cb;
    f_rec->start = 0;
    func_recorder_set_play_btn_pic();

#if BT_BACKSTAGE_MUSIC_EN
    bt_audio_bypass();
#endif

#if FUNC_REC_EN
    rec_src.spr = SPR_16000;
    rec_src.nchannel = 1;
    rec_src.rec_type = REC_MP3;
    rec_src.bitrate = 48000;
    rec_src.source_start = mic_rec_start;
    rec_src.source_stop  = mic_rec_stop;
#endif
}

static void func_recorder_exit(void)
{
#if BT_BACKSTAGE_MUSIC_EN
    bt_audio_enable();
#endif
}

void func_recorder(void)
{
    printf("%s\n", __func__);

    func_recorder_enter();

    while (func_cb.sta == FUNC_RECORDER) {
        func_recorder_process();
        func_recorder_message(msg_dequeue());
    }

    func_recorder_exit();
}

#endif  //FUNC_RECORDER_EN
