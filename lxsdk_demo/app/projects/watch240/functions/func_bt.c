#include "include.h"
#include "func.h"
#include "func_bt.h"
#include "ute_module_music.h"

#define TRACE_EN 0

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_MUSIC_SUPPORT

#define TITLE_BUF_LEN UTE_MUSIC_TITLE_MAX_SIZE    // 歌名buf长度
#define ARTIST_BUF_LEN UTE_MUSIC_ARTLIST_MAX_SIZE // 歌手/歌手buf长度
#define PROGRESS_BAR_LENGTH 134
#define BTN_REST_DISP_TIME 300 // 按钮释放时间 ms 级别

enum
{
    COMPO_ID_BTN_PREV = 1,
    COMPO_ID_BTN_NEXT,
    COMPO_ID_BTN_PLAY,
    COMPO_ID_BTN_NOPLAY,
    COMPO_ID_BTN_PLAYING,
    COMPO_ID_BTN_VOL_UP,
    COMPO_ID_BTN_VOL_DOWN,
    COMPO_ID_BTN_VOL1_UP,
    COMPO_ID_BTN_VOL1_DOWN,
    COMPO_ID_TXT_MUSIC_NAME,
    COMPO_ID_TXT_MUSIC_LYRIC,
    COMPO_ID_SHAPE_MUSIC_VOL,
};

typedef struct f_bt_t_
{
    uint32_t tick;
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
    char title_buf_old[TITLE_BUF_LEN];
    char artist_buf_old[ARTIST_BUF_LEN];
    bool refresh_data;
} f_bt_t;

/*****************************************************************************
 * 1.当BT连接时，优先同步id3信息，控制通道走BT
 * 2.当只连接BLE时，IOS走AMS服务，安卓走私有协议
 *****************************************************************************/

#if (BT_ID3_TAG_EN || LE_AMS_CLIENT_EN)
// static void func_bt_music_title_refresh(void *buf);
// static void func_bt_music_artist_refresh(void *buf);
#endif

// static void func_bt_music_play_btnpic_refresh(u8 sta);
// static void func_bt_music_vol_btnpic_refresh(u8 vol);

// static void bt_id3_tag_update_handle(u8 type, char *str)
// {
//    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
//    if (BT_ID3_TAG_TITLE == type)
//    {
//        memcpy(f_bt->title_buf, str, TITLE_BUF_LEN-1);
//        msg_enqueue(EVT_ID3_TITLE_UPDATE);
//    }
//    else if (BT_ID3_TAG_ARTIST == type)
//    {
//        memcpy(f_bt->artist_buf, str, ARTIST_BUF_LEN-1);
//        msg_enqueue(EVT_ID3_ARTIST_UPDATE);
//    }
// }

/*****************************************************************************
 *          BT or BLE interface
 *****************************************************************************/

void func_bt_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0)
    {
        return;
    }
    bt_cb.res_bt_bypass = true;

    if (!sbc_is_bypass())
    {
        bt_audio_bypass();
    }
    mp3_res_play(addr, len);
}

void func_bt_mp3_play_restore(void)
{
    if (bt_cb.res_bt_bypass)
    {
        bt_cb.res_bt_bypass = false;
        bt_audio_enable();
    }
}

void func_bt_init(void)
{
    if (!bt_cb.bt_is_inited)
    {
        msg_queue_clear();
        dis_auto_pwroff();
        bsp_bt_init();
        bt_redial_init();
        bt_cb.bt_is_inited = 1;
    }
}

void func_bt_chk_off(void)
{
    if ((func_cb.sta != FUNC_BT) && (bt_cb.bt_is_inited))
    {
        bt_disconnect(1);
        bt_off();
        bt_cb.bt_is_inited = 0;
    }
}

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
/*****************************************************************************
 *          func_bt_music(UI)
 *****************************************************************************/
// 创建蓝牙音乐播放器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_form_create(void)
{
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
    uint16_t title_size_leng = 0;
    uint16_t artist_size_leng = 0;
    memset(title_buf, 0, sizeof(title_buf));
    memset(artist_buf, 0, sizeof(artist_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)title_buf, &title_size_leng);
    uteModuleMusicGetPlayerArtistSize((uint8_t *)artist_buf, &artist_size_leng);
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_textbox_t *txt;

    compo_picturebox_t *pic;

    // 设置标题栏
    /// 设置标题栏名字///
    // if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    // {
    pic = compo_picturebox_create(frm, UI_BUF_I330001_MUSICC_BG1_BIN); ///* 背景图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    pic = compo_picturebox_create(frm, UI_BUF_I330001_MUSICC_MUSIC_BG_BIN); /// 音量进度条背景
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3);
    pic = compo_picturebox_create(frm, UI_BUF_I330001_MUSICC_SHENGYING_SMALL1_BIN); ///* 音量减*/
    compo_picturebox_set_pos(pic,45, 261);
    pic = compo_picturebox_create(frm, UI_BUF_I330001_MUSICC_SHENGYING_BIG1_BIN); ///* 音量加*/
    compo_picturebox_set_pos(pic,193, 261);
    txt = compo_textbox_create(frm, strlen(i18n[STR_MUSIC]));
    //    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
    compo_textbox_set(txt, i18n[STR_MUSIC]);
    compo_textbox_set_location(txt, GUI_SCREEN_WIDTH / 12, GUI_SCREEN_HEIGHT / 21.8, GUI_SCREEN_WIDTH * 2 / 5, GUI_SCREEN_HEIGHT / (284 / 28));
    compo_textbox_set(txt, i18n[STR_MUSIC]);
    compo_textbox_set_align_center(txt, false);
    // }
    // else
    // {
    //     txt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
    //     //    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
    //     compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
    //     compo_textbox_set_autoroll_mode(txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    //     compo_textbox_set_location(txt, GUI_SCREEN_WIDTH / 12, GUI_SCREEN_HEIGHT / 21.8, GUI_SCREEN_WIDTH * 2 / 5, GUI_SCREEN_HEIGHT / (284 / 28));
    //     compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
    //     compo_textbox_set_align_center(txt, false);
    //     pic = compo_picturebox_create(frm, UI_BUF_I330001_MUSICC_BLUETOOTH_DISCONNECT_BIN); ///* 未连接图*/
    //     compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    // }

    // 歌词歌名
    //  if(bt_a2dp_profile_completely_connected() || ble_is_connect())
    //  {
    //      //歌词/歌手
    //      compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    //      compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH, 50);
    //      compo_textbox_set_autoroll_mode(name_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    //      compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);

    //     compo_textbox_set(name_txt,artist_buf);
    //     if(title_size_leng == 0)
    //     {
    //         compo_textbox_set(name_txt, i18n[STR_UNKNOWN]);
    //     }
    //     compo_textbox_set_forecolor(name_txt, COLOR_GRAY);

    //     //歌名
    //     compo_textbox_t *lyric_txt = compo_textbox_create(frm, 50);
    //     compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/1.8, GUI_SCREEN_WIDTH, 50);
    //     compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    //     compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);

    //     compo_textbox_set(lyric_txt,title_buf);
    //     if(artist_size_leng == 0)
    //     {
    //         compo_textbox_set(lyric_txt, i18n[STR_UNKNOWN]);
    //     }
    // }
    // else
    // {
    //     compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    //     compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH, 50);
    //     compo_textbox_set_autoroll_mode(name_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    //     compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);
    //     compo_textbox_set(name_txt, i18n[STR_UNKNOWN]);
    //     compo_textbox_set_forecolor(name_txt, COLOR_GRAY );
    //     compo_textbox_set_visible(name_txt, false);

    //     compo_textbox_t *lyric_txt = compo_textbox_create(frm, 50);
    //     compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_WIDTH, 50);
    //     compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    //     compo_textbox_set(lyric_txt, i18n[STR_VOICE_BT_NOT_CONNECT]);
    //     compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);
    // }

    // 新建按钮
    compo_button_t *btn;
    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSICC_PREVIOUS_SONG_BIN); /// 上一曲
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_PREVIOUSSONG01_BIN); /// 上一曲
    }
    compo_setid(btn, COMPO_ID_BTN_PREV);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 1.5, GUI_SCREEN_CENTER_Y);
    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_MUSICC_PAUSE1_BIN);
        compo_picturebox_set_pos(picbox, 214, 112);
        compo_picturebox_set_visible(picbox, true);
        compo_setid(picbox, COMPO_ID_BTN_NOPLAY);

        picbox = compo_picturebox_create(frm, UI_BUF_I330001_MUSICC_PLAY_BACK1_BIN);
        compo_picturebox_set_pos(picbox, 158, 66);
        compo_picturebox_set_visible(picbox, true);
        compo_setid(picbox, COMPO_ID_BTN_PLAYING);
    }

    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSICC_PAUSE_BIN); /// 暂停   流程1
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSICC_PAUSE_BIN); /// 暂停
    }
    compo_setid(btn, COMPO_ID_BTN_PLAY);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }
    if (bt_cb.music_playing)
    {
        compo_button_set_bgimg(btn, UI_BUF_I330001_MUSICC_PLAY_BACK_BIN);
    }
    else
    {
        if (bt_a2dp_profile_completely_connected() || ble_is_connect())
        {
            // 流程2
            compo_button_set_bgimg(btn, UI_BUF_I330001_MUSICC_PAUSE_BIN);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I330001_MUSICC_PAUSE_BIN);
        }
    }

    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSICC_NEXT_SONG_BIN); /// 下一曲
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_NEXTSONG01_BIN); /// 下一曲
    }
    compo_setid(btn, COMPO_ID_BTN_NEXT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 1.5, GUI_SCREEN_CENTER_Y);

    btn = compo_button_create(frm); //音量减
    compo_button_set_location(btn,45, 261,80,80);
    compo_setid(btn, COMPO_ID_BTN_VOL1_DOWN);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_MUSIC_VOLUME01_BIN);
    compo_picturebox_set_pos(picbox,45, 261);
    compo_picturebox_set_visible(picbox, (bt_a2dp_profile_completely_connected() || ble_is_connect()) ? false : true);
    compo_setid(picbox, COMPO_ID_BTN_VOL_DOWN);

    btn = compo_button_create(frm);// 音量加
    compo_button_set_location(btn,193, 261,80,80);
    compo_setid(btn, COMPO_ID_BTN_VOL1_UP);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_MUSIC_VOLUME03_BIN);
    compo_picturebox_set_pos(picbox,195, 261);
    compo_picturebox_set_visible(picbox, (bt_a2dp_profile_completely_connected() || ble_is_connect()) ? false : true);
    compo_setid(picbox, COMPO_ID_BTN_VOL_UP);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); /// 音量进度条
    compo_setid(shape, COMPO_ID_SHAPE_MUSIC_VOL);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3, PROGRESS_BAR_LENGTH, 6);
    compo_shape_set_radius(shape, 3);

    uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
    if (vol > 16)
        vol = 16;
    compo_shape_set_location(shape, (GUI_SCREEN_WIDTH - PROGRESS_BAR_LENGTH) / 2 + vol * (PROGRESS_BAR_LENGTH / 16) / 2, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3, vol * (PROGRESS_BAR_LENGTH / 16), 6);

    // if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    // {
    //     compo_shape_set_visible(shape, true);
    // }
    // else
    // {
    //     compo_shape_set_visible(shape, false);
    // }

    return frm;
}

static void func_bt_music_refresh_disp(void)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    compo_button_t *btn_prev = compo_getobj_byid(COMPO_ID_BTN_PREV);
    compo_button_t *btn_next = compo_getobj_byid(COMPO_ID_BTN_NEXT);
    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PLAY);

    // 歌词，歌名
    //  compo_textbox_t *tilte_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_LYRIC);
    // compo_textbox_t *tilte_art_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_NAME);
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_SHAPE_MUSIC_VOL);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_BTN_NOPLAY);
    compo_picturebox_t *picbox1 = compo_getobj_byid(COMPO_ID_BTN_PLAYING);

    if (!bt_a2dp_profile_completely_connected() && !ble_is_connect())
    {
        // 歌词，歌名
        //   compo_textbox_set_visible(tilte_art_txt, false);
        //  compo_textbox_set_visible(tilte_txt, false);

        // compo_textbox_set_location(tilte_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_WIDTH, 50);
        // compo_textbox_set_autoroll_mode(tilte_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        // compo_textbox_set(tilte_txt, i18n[STR_VOICE_BT_NOT_CONNECT]);
        // compo_setid(tilte_txt, COMPO_ID_TXT_MUSIC_LYRIC);
        // compo_shape_set_visible(shape, false);
        // compo_button_set_visible(btn_play, false);
        // compo_button_set_visible(btn_next, false);
        // compo_button_set_visible(btn_prev, false);
        // compo_button_set_visible(btn_up, false);
        // compo_button_set_visible(btn_down, false);
        // memset(f_bt->title_buf, 1, sizeof(f_bt->title_buf_old));
        // memset(f_bt->artist_buf, 1, sizeof(f_bt->artist_buf_old));
        f_bt->refresh_data = true;
        return;
    }
    else
    {
        compo_picturebox_set_visible(picbox, false);
        compo_picturebox_set_visible(picbox1, false);
        // compo_shape_set_visible(shape, true);
        // 歌词，歌名
        //   compo_textbox_set_visible(tilte_art_txt, true);
        //   compo_textbox_set_location(tilte_art_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH, 50);
        //   compo_textbox_set_location(tilte_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/1.8, GUI_SCREEN_WIDTH, 50);
    }
    uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
    //    printf("value:%d\n",vol);
    //    vol = vol/6*6;
    uint16_t title_size_leng = 0;
    uint16_t artist_size_leng = 0;
    memset(f_bt->title_buf, 0, sizeof(f_bt->title_buf));
    memset(f_bt->artist_buf, 0, sizeof(f_bt->artist_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)f_bt->title_buf, &title_size_leng);
    uteModuleMusicGetPlayerArtistSize((uint8_t *)f_bt->artist_buf, &artist_size_leng);

    // 歌词，歌名
    //      if(strcmp(f_bt->title_buf, f_bt->title_buf_old)!=0 || title_size_leng == 0 || f_bt->refresh_data) //歌名刷新
    //      {
    //          f_bt->refresh_data = false;
    //  //        printf("1111111111111111111111111111\n");
    //          memcpy(f_bt->title_buf_old, f_bt->title_buf, sizeof(f_bt->title_buf));
    //          compo_textbox_set(tilte_txt, f_bt->title_buf);
    //          if(title_size_leng == 0)
    //          {
    //              compo_textbox_set(tilte_txt, i18n[STR_UNKNOWN]);
    //          }
    //      }
    //      if(strcmp(f_bt->artist_buf, f_bt->artist_buf_old)!=0 || artist_size_leng==0 || f_bt->refresh_data) //歌手刷新
    //      {
    //          f_bt->refresh_data = false;+
    //  //        printf("2222222222222222222222222222\n");
    //          memcpy(f_bt->artist_buf_old, f_bt->artist_buf, sizeof(f_bt->artist_buf));
    //          compo_textbox_set(tilte_art_txt, f_bt->artist_buf);
    //          if(artist_size_leng == 0)
    //          {
    //              compo_textbox_set(tilte_art_txt, i18n[STR_UNKNOWN]);
    //          }

    //     }

    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }
    if (bt_cb.music_playing)
    {
        compo_picturebox_set_visible(picbox1, true);
        compo_button_set_bgimg(btn_play, UI_BUF_I330001_MUSICC_PLAY_BACK_BIN);
    }
    else
    {
        // 流程3 流程5 流程7
        compo_picturebox_set_visible(picbox, true);
        compo_button_set_bgimg(btn_play, UI_BUF_I330001_MUSICC_PAUSE_BIN);
    }
    if (vol > 16)
        vol = 16;
    compo_shape_set_location(shape, 4 + (GUI_SCREEN_WIDTH - PROGRESS_BAR_LENGTH) / 2 + vol * (PROGRESS_BAR_LENGTH / 16) / 2, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3, vol * (PROGRESS_BAR_LENGTH / 16), 6);
}

/// 按钮释放
static void func_bt_button_release_handle()
{
    if (!bt_a2dp_profile_completely_connected() && !ble_is_connect())
        return;

    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    s32 dx, dy;

    compo_button_t *btn_prev = compo_getobj_byid(COMPO_ID_BTN_PREV);
    compo_button_t *btn_next = compo_getobj_byid(COMPO_ID_BTN_NEXT);
    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    compo_picturebox_t *picbox1 = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
    compo_picturebox_t *picbox2 = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);

    //    func_bt_music_refresh_disp();

    if (f_bt->tick <= tick_get() - BTN_REST_DISP_TIME)
    {

        compo_button_set_bgimg(btn_prev, UI_BUF_I330001_MUSICC_PREVIOUS_SONG_BIN);
        compo_button_set_bgimg(btn_next, UI_BUF_I330001_MUSICC_NEXT_SONG_BIN);
        compo_picturebox_set_visible(picbox1, false);
        compo_picturebox_set_visible(picbox2, false);
        if (bt_cb.music_playing == false)
        {
            compo_button_set_bgimg(btn_play, UI_BUF_I330001_MUSICC_PAUSE_BIN);
        }

    }

    int id = compo_get_button_id();
    if (id == 0 || !ctp_get_dxy(&dx, &dy))
        return;

    f_bt->tick = tick_get();

    switch (id)
    {
        case COMPO_ID_BTN_PREV:
            compo_button_set_bgimg(btn_prev, UI_BUF_I330001_MUSIC_PREVIOUSSONG01_BIN);
            break;
        case COMPO_ID_BTN_NEXT:
            compo_button_set_bgimg(btn_next, UI_BUF_I330001_MUSIC_NEXTSONG01_BIN);
            break;
        case COMPO_ID_BTN_PLAY:
            if (bt_cb.music_playing == false)
            {
                compo_button_set_bgimg(btn_play, UI_BUF_I330001_MUSICC_PAUSE_BIN);
            }
            break;
        case COMPO_ID_BTN_VOL1_UP:
            compo_picturebox_set_visible(picbox1, true);
            compo_picturebox_set_visible(picbox2, false);
            break;
        case COMPO_ID_BTN_VOL1_DOWN:
            compo_picturebox_set_visible(picbox1, false);
            compo_picturebox_set_visible(picbox2, true);
            break;
    }
}

// 单击按钮
static void func_bt_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_PREV:
            uteModuleMusicCtrl(false, true, false);
            break;

        case COMPO_ID_BTN_NEXT:
            uteModuleMusicCtrl(true, true, false);
            break;

        case COMPO_ID_BTN_PLAY:
            printf("music_play111111111111111111\n"); // 流程4 流程6
            uteModuleMusicCtrlPaused(false);
            if (ble_is_connect())
            {
                bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
            }
            printf("bt_cb.music_playing=%d\n", bt_is_playing());
            break;

        case COMPO_ID_BTN_VOL1_UP:
            uteModuleMusicCtrlVolumeIncrease(false);
            break;

        case COMPO_ID_BTN_VOL1_DOWN:
            uteModuleMusicCtrlVolumeDecrease(false);
            break;

        default:
            break;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
/*****************************************************************************
 *          func_bt_music(UI)
 *****************************************************************************/
// 创建蓝牙音乐播放器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_form_create(void)
{
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
    uint16_t title_size_leng = 0;
    uint16_t artist_size_leng = 0;
    memset(title_buf, 0, sizeof(title_buf));
    memset(artist_buf, 0, sizeof(artist_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)title_buf, &title_size_leng);
    uteModuleMusicGetPlayerArtistSize((uint8_t *)artist_buf, &artist_size_leng);

    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    // 设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MUSIC]);
    compo_form_add_image(frm, UI_BUF_I332001_MUSIC_FRAME_2313_BIN, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        // 歌词/歌手
        compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 2.5, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(name_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);

        compo_textbox_set(name_txt, artist_buf);
        if (title_size_leng == 0)
        {
            compo_textbox_set(name_txt, i18n[STR_UNKNOWN]);
        }
        compo_textbox_set_forecolor(name_txt, COLOR_GRAY);

        // 歌名
        compo_textbox_t *lyric_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 1.8, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);

        compo_textbox_set(lyric_txt, title_buf);
        if (artist_size_leng == 0)
        {
            compo_textbox_set(lyric_txt, i18n[STR_UNKNOWN]);
        }
    }
    else
    {
        compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 2.5, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(name_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);
        compo_textbox_set(name_txt, i18n[STR_UNKNOWN]);
        compo_textbox_set_forecolor(name_txt, COLOR_GRAY);
        compo_textbox_set_visible(name_txt, false);

        compo_textbox_t *lyric_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 2, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_textbox_set(lyric_txt, i18n[STR_VOICE_BT_NOT_CONNECT]);
        compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);
    }

    // 新建按钮
    compo_button_t *btn;
    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_PREVIOUSSONG02_BIN); /// 上一曲
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_PREVIOUSSONG01_BIN); /// 上一曲
    }
    compo_setid(btn, COMPO_ID_BTN_PREV);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 1.5, GUI_SCREEN_CENTER_Y);

    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_PLAY01_BIN); /// 暂停 播放
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_PLAY02_BIN); /// 暂停 播放
    }
    compo_setid(btn, COMPO_ID_BTN_PLAY);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    if (ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }
    if (bt_cb.music_playing)
    {
        compo_button_set_bgimg(btn, UI_BUF_I332001_MUSIC_PAUSED_BIN);
    }
    else
    {
        if (bt_a2dp_profile_completely_connected() || ble_is_connect())
        {
            compo_button_set_bgimg(btn, UI_BUF_I332001_MUSIC_PLAY01_BIN);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I332001_MUSIC_PLAY02_BIN);
        }
    }

    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_NEXTSONG02_BIN); /// 下一曲
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_NEXTSONG01_BIN); /// 下一曲
    }
    compo_setid(btn, COMPO_ID_BTN_NEXT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 1.5, GUI_SCREEN_CENTER_Y);

    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_VOLUME02_BIN); /// 音量减
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_VOLUME01_BIN); /// 音量减
    }
    compo_setid(btn, COMPO_ID_BTN_VOL_DOWN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 1.3, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3);

    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_VOLUME04_BIN); /// 音量加
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_VOLUME03_BIN); /// 音量加
    }
    compo_setid(btn, COMPO_ID_BTN_VOL_UP);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 1.3, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); /// 音量进度条背景
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3, PROGRESS_BAR_LENGTH, 6);
    compo_shape_set_radius(shape, 3);
    compo_shape_set_color(shape, make_color(0x33, 0x33, 0x33));

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); /// 音量进度条
    compo_setid(shape, COMPO_ID_SHAPE_MUSIC_VOL);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3, PROGRESS_BAR_LENGTH, 6);
    compo_shape_set_radius(shape, 3);
    if (bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        compo_shape_set_visible(shape, true);
    }
    else
    {
        compo_shape_set_visible(shape, false);
    }
    uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
    if (vol > 16)
        vol = 16;
    compo_shape_set_location(shape, (GUI_SCREEN_WIDTH - PROGRESS_BAR_LENGTH) / 2 + vol * (PROGRESS_BAR_LENGTH / 16) / 2, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3, vol * (PROGRESS_BAR_LENGTH / 16), 6);

    return frm;
}

static void func_bt_music_refresh_disp(void)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    compo_button_t *btn_prev = compo_getobj_byid(COMPO_ID_BTN_PREV);
    compo_button_t *btn_next = compo_getobj_byid(COMPO_ID_BTN_NEXT);
    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    compo_button_t *btn_up = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
    compo_button_t *btn_down = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);
    compo_textbox_t *tilte_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_LYRIC);
    compo_textbox_t *tilte_art_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_NAME);
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_SHAPE_MUSIC_VOL);

    if (!bt_a2dp_profile_completely_connected() && !ble_is_connect())
    {
        compo_textbox_set_visible(tilte_art_txt, false);

        compo_textbox_set_location(tilte_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 2, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(tilte_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_textbox_set(tilte_txt, i18n[STR_VOICE_BT_NOT_CONNECT]);
        compo_setid(tilte_txt, COMPO_ID_TXT_MUSIC_LYRIC);
        compo_shape_set_visible(shape, false);
        compo_button_set_bgimg(btn_play, UI_BUF_I332001_MUSIC_PLAY02_BIN);
        compo_button_set_bgimg(btn_next, UI_BUF_I332001_MUSIC_NEXTSONG01_BIN);
        compo_button_set_bgimg(btn_prev, UI_BUF_I332001_MUSIC_PREVIOUSSONG01_BIN);
        compo_button_set_bgimg(btn_up, UI_BUF_I332001_MUSIC_VOLUME03_BIN);
        compo_button_set_bgimg(btn_down, UI_BUF_I332001_MUSIC_VOLUME01_BIN);
        memset(f_bt->title_buf, 1, sizeof(f_bt->title_buf_old));
        memset(f_bt->artist_buf, 1, sizeof(f_bt->artist_buf_old));
        f_bt->refresh_data = true;
        return;
    }
    else
    {
        compo_shape_set_visible(shape, true);
        compo_textbox_set_visible(tilte_art_txt, true);
        compo_textbox_set_location(tilte_art_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 2.5, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_location(tilte_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 1.8, GUI_SCREEN_WIDTH, 50);
    }
    uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
    //    printf("value:%d\n",vol);
    //    vol = vol/6*6;
    uint16_t title_size_leng = 0;
    uint16_t artist_size_leng = 0;
    memset(f_bt->title_buf, 0, sizeof(f_bt->title_buf));
    memset(f_bt->artist_buf, 0, sizeof(f_bt->artist_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)f_bt->title_buf, &title_size_leng);
    uteModuleMusicGetPlayerArtistSize((uint8_t *)f_bt->artist_buf, &artist_size_leng);

    if (strcmp(f_bt->title_buf, f_bt->title_buf_old) != 0 || title_size_leng == 0 || f_bt->refresh_data) // 歌名刷新
    {
        f_bt->refresh_data = false;
        //        printf("1111111111111111111111111111\n");
        memcpy(f_bt->title_buf_old, f_bt->title_buf, sizeof(f_bt->title_buf));
        compo_textbox_set(tilte_txt, f_bt->title_buf);
        if (title_size_leng == 0)
        {
            compo_textbox_set(tilte_txt, i18n[STR_UNKNOWN]);
        }
    }
    if (strcmp(f_bt->artist_buf, f_bt->artist_buf_old) != 0 || artist_size_leng == 0 || f_bt->refresh_data) // 歌手刷新
    {
        f_bt->refresh_data = false;
        //        printf("2222222222222222222222222222\n");
        memcpy(f_bt->artist_buf_old, f_bt->artist_buf, sizeof(f_bt->artist_buf));
        compo_textbox_set(tilte_art_txt, f_bt->artist_buf);
        if (artist_size_leng == 0)
        {
            compo_textbox_set(tilte_art_txt, i18n[STR_UNKNOWN]);
        }
    }

    if (ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }
    if (bt_cb.music_playing)
    {
        compo_button_set_bgimg(btn_play, UI_BUF_I332001_MUSIC_PAUSED_BIN);
    }
    else
    {
        compo_button_set_bgimg(btn_play, UI_BUF_I332001_MUSIC_PLAY01_BIN);
    }
    if (vol > 16)
        vol = 16;
    compo_shape_set_location(shape, (GUI_SCREEN_WIDTH - PROGRESS_BAR_LENGTH) / 2 + vol * (PROGRESS_BAR_LENGTH / 16) / 2, GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_Y / 1.3, vol * (PROGRESS_BAR_LENGTH / 16), 6);
}

/// 按钮释放
static void func_bt_button_release_handle()
{
    if (!bt_a2dp_profile_completely_connected() && !ble_is_connect())
        return;

    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    s32 dx, dy;

    compo_button_t *btn_prev = compo_getobj_byid(COMPO_ID_BTN_PREV);
    compo_button_t *btn_next = compo_getobj_byid(COMPO_ID_BTN_NEXT);
    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    compo_button_t *btn_up = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
    compo_button_t *btn_down = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);

    //    func_bt_music_refresh_disp();

    if (f_bt->tick <= tick_get() - BTN_REST_DISP_TIME)
    {
        compo_button_set_bgimg(btn_prev, UI_BUF_I332001_MUSIC_PREVIOUSSONG02_BIN);
        compo_button_set_bgimg(btn_next, UI_BUF_I332001_MUSIC_NEXTSONG02_BIN);
        if (bt_cb.music_playing == false)
        {
            //            compo_button_set_bgimg(btn_play,UI_BUF_I332001_MUSIC_PLAY01_BIN );
        }
        compo_button_set_bgimg(btn_up, UI_BUF_I332001_MUSIC_VOLUME04_BIN);
        compo_button_set_bgimg(btn_down, UI_BUF_I332001_MUSIC_VOLUME02_BIN);
    }

    int id = compo_get_button_id();
    if (id == 0 || !ctp_get_dxy(&dx, &dy))
        return;

    f_bt->tick = tick_get();

    switch (id)
    {
        case COMPO_ID_BTN_PREV:
            compo_button_set_bgimg(btn_prev, UI_BUF_I332001_MUSIC_PREVIOUSSONG01_BIN);
            break;
        case COMPO_ID_BTN_NEXT:
            compo_button_set_bgimg(btn_next, UI_BUF_I332001_MUSIC_NEXTSONG01_BIN);
            break;
        case COMPO_ID_BTN_PLAY:
            if (bt_cb.music_playing == false)
            {
                //                compo_button_set_bgimg(btn_play,UI_BUF_I332001_MUSIC_PLAY02_BIN );
            }
            break;
        case COMPO_ID_BTN_VOL_UP:
            compo_button_set_bgimg(btn_up, UI_BUF_I332001_MUSIC_VOLUME03_BIN);
            break;
        case COMPO_ID_BTN_VOL_DOWN:
            compo_button_set_bgimg(btn_down, UI_BUF_I332001_MUSIC_VOLUME01_BIN);
            break;
    }
}

// 单击按钮
static void func_bt_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_PREV:
            uteModuleMusicCtrl(false, true, false);
            break;

        case COMPO_ID_BTN_NEXT:
            uteModuleMusicCtrl(true, true, false);
            break;

        case COMPO_ID_BTN_PLAY:
            //            printf("music_play111111111111111111\n");
            uteModuleMusicCtrlPaused(false);
            if (ble_is_connect())
            {
                bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
            }
            //            printf("bt_cb.music_playing=%d\n",bt_cb.music_playing);
            break;

        case COMPO_ID_BTN_VOL_UP:
            uteModuleMusicCtrlVolumeIncrease(false);
            break;

        case COMPO_ID_BTN_VOL_DOWN:
            uteModuleMusicCtrlVolumeDecrease(false);
            break;

        default:
            break;
    }
}

#endif UI控制宏

void func_bt_sub_process(void)
{
    bsp_bt_status();
#if LE_HID_EN
    ble_hid_service_proc();
#endif // LE_HID_EN
}

void func_bt_process(void)
{
    func_bt_button_release_handle();
    func_process();
    func_bt_sub_process();

    if (sys_cb.pwroff_delay == 0)
    {
        func_cb.sta = FUNC_PWROFF;
        return;
    }
}

// 蓝牙音乐消息处理
static void func_bt_message_do(size_msg_t msg)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            if (!uteModuleCallBtIsConnected() && !ble_is_connect())
                return;
            func_bt_button_click(); // 单击按钮
            //            func_bt_music_refresh_disp();
            break;

        case MSG_SYS_500MS:
            //            printf("1111111111111111111111\n");
            func_bt_music_refresh_disp();
//            if (bt_a2dp_profile_completely_connected() && !sys_cb.gui_sleep_sta)
//            {
#if BT_ID3_TAG_EN
            if (bt_a2dp_profile_completely_connected())
            {
                bt_music_paly_status_info();
            }
#endif
            //                if (f_bt->bt_play_sta != bt_cb.music_playing)
            //                {
            //                    f_bt->bt_play_sta = bt_cb.music_playing;
            //                    func_bt_music_play_btnpic_refresh(bt_cb.music_playing);
            //                }
            //                bt_vol_update();
            //            }
            break;

        //        case EVT_ID3_TITLE_UPDATE:
        ////            func_bt_music_refresh_disp();
        ////            func_bt_music_title_refresh(f_bt->title_buf);
        //            break;

        //        case EVT_ID3_ARTIST_UPDATE:
        ////            func_bt_music_refresh_disp();
        ////            func_bt_music_artist_refresh(f_bt->artist_buf);
        //            break;

        default:
            func_message(msg);
            break;
    }
}
#if LE_HID_TEST
// AT(.text.func.bt.msg)
void func_ble_hid_message_do(u16 msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_bt_button_click(); // 单击按钮
            break;
        default:
            func_message(msg);
            break;
    }
}
#endif
AT(.text.func.bt.msg)
void func_bt_message(u16 msg)
{
#if LE_HID_TEST
    func_ble_hid_message_do(msg);
#else
    func_bt_message_do(msg);
#endif
}

void func_bt_enter(void)
{
    f_bt_t *f_bt;
    func_cb.f_cb = func_zalloc(sizeof(f_bt_t));
    func_cb.frm_main = func_bt_form_create();
    f_bt = (f_bt_t *)func_cb.f_cb;

//    func_cb.mp3_res_play = func_bt_mp3_res_play;
//    bt_cb.bt_form_created = 1;
//    f_bt->bt_play_sta = bt_cb.music_playing;
//    f_bt->ams_play_sta = false;
//    f_bt->vol = 0;
//    if (sys_cb.music_title_init)
//    {
//        memcpy(f_bt->title_buf, sys_cb.title_buf, sizeof(sys_cb.title_buf));
//        msg_enqueue(EVT_ID3_TITLE_UPDATE);
//    }
//    if (sys_cb.music_artist_init)
//    {
//        memcpy(f_bt->artist_buf, sys_cb.artist_buf, sizeof(sys_cb.artist_buf));
//        msg_enqueue(EVT_ID3_ARTIST_UPDATE);
//    }
//    printf("%s->%d\n", __func__, f_bt->bt_play_sta);
//    func_bt_music_play_btnpic_refresh(f_bt->bt_play_sta);
// #if LE_AMS_CLIENT_EN
//    if (ble_ams_is_connected())
//    {
//        if (ble_ams_cb.play_state)
//        {
//            f_bt->ams_play_sta = true;
//            func_bt_music_play_btnpic_refresh(1);
//        }
//        f_bt->ams_vol = ble_ams_cb.vol;
//        func_bt_music_vol_btnpic_refresh(f_bt->ams_vol);
//        if (strlen(ble_ams_cb.app_name))
//        {
//            compo_form_set_title(func_cb.frm_main, ble_ams_cb.app_name);
//        }
//    }
//    ble_ams_sta_update_cb_reg(ble_ams_sta_update_handle);
// #endif
// bt_id3_tag_update_cb_reg(bt_id3_tag_update_handle);
#if BT_ID3_TAG_EN
    if (bt_a2dp_profile_completely_connected())
    {
        bt_music_get_id3_tag();
    }
#endif

#if !BT_BACKSTAGE_MUSIC_EN
    func_bt_init();
    bt_audio_enable();
#if DAC_DNR_EN
    dac_dnr_set_sta(1);
    sys_cb.dnr_sta = 1;
#endif
#endif // !BT_BACKSTAGE_MUSIC_EN
}

void func_bt_exit(void)
{
#if !BT_BACKSTAGE_MUSIC_EN
    dac_fade_out();
#if DAC_DNR_EN
    dac_dnr_set_sta(0);
    sys_cb.dnr_sta = 0;
#endif
    bt_audio_bypass();

#if !BT_BACKSTAGE_EN
    bt_disconnect(1);
    bt_off();
    bt_cb.bt_is_inited = 0;
#else
    if (bt_get_status() == BT_STA_PLAYING && !bt_is_testmode()) // 蓝牙退出停掉音乐
    {
        delay_5ms(10);
        if (bt_get_status() == BT_STA_PLAYING) // 再次确认play状态
        {
            u32 timeout = 850; // 8.5s
            bt_music_pause();
            while (bt_get_status() == BT_STA_PLAYING && timeout > 0)
            {
                timeout--;
                delay_5ms(2);
            }
        }
    }
#endif // !BT_BACKSTAGE_EN
#endif // !BT_BACKSTAGE_MUSIC_EN
    ble_ams_sta_update_cb_unreg();
    bt_id3_tag_update_cb_unreg();

    //    bt_cb.bt_form_created = 0;
    bt_cb.rec_pause = 0;
    func_cb.last = FUNC_BT;
}

void func_bt(void)
{
    printf("%s\n", __func__);
    func_bt_enter();
    while (func_cb.sta == FUNC_BT)
    {
        func_bt_process();
        func_bt_message(msg_dequeue());
    }
    func_bt_exit();
}

#endif
