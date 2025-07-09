#include "include.h"
#include "func.h"
#include "func_music.h"

#if FUNC_MUSIC_EN

enum
{
    COMPO_ID_BTN_PREV = 1,
    COMPO_ID_BTN_NEXT,
    COMPO_ID_BTN_PLAY,
    COMPO_ID_BTN_VOL_UP,
    COMPO_ID_BTN_VOL_DOWN,
    COMPO_ID_TXT_MUSIC_NAME,
    COMPO_ID_TXT_MUSIC_TIME,
    COMPO_ID_PIC_MUSIC_VOL,
    COMPO_ID_PLAY_PROC,
};

typedef struct f_music_t_
{
    u8 vol;
    char fname_buf[FUNC_MUSIC_NAME_BUF_LEN];
    bool flag_play;
} f_music_t;

char fname_buf[FUNC_MUSIC_NAME_BUF_LEN] AT(.fname.buf);

void music_slot_kick(void);

void func_music_set_tilte_name(void *buf)
{
    compo_textbox_t *tilte_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_NAME);
    compo_textbox_set(tilte_txt, buf);
}

static void func_music_filter_set(void)
{
#if MUSIC_REC_FILE_FILTER
    if (msc_cb.rec_type)
    {
        fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_only_record_file_filter, music_only_record_dir_filter);       //只播放录音文件
    }
    else
    {
        fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_file_filter, music_rm_record_dir_filter);                     //不播放录音文件
    }
#else
    fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_file_filter, music_dir_filter);                                   //播放全部文件
#endif // MUSIC_REC_FILE_FILTER
}

void func_music_set_music_time_text(void)
{
    char time_str[20];
    if (!msc_cb.pause && msc_cb.alltime.min != 0xff)
    {
        compo_textbox_t *time_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_TIME);
        sprintf(time_str, "%02d:%02d / %02d:%02d", msc_cb.curtime.min, msc_cb.curtime.sec, msc_cb.alltime.min, msc_cb.alltime.sec);
        compo_textbox_set(time_txt, time_str);
    }
}

static void func_music_init(void)
{
    SD0_LDO_EN_EN();    //打开SD供电

#if BT_BACKSTAGE_EN && MUSIC_UDISK_EN
    if (dev_is_online(DEV_UDISK))
    {
        udisk_resume();
    }
#endif
#if BT_BACKSTAGE_MUSIC_EN
    bt_audio_bypass();
    if (bt_is_playing())
    {
        bt_music_pause();
    }
#endif

    func_music_filter_set();
    fsdisk_callback_init(sys_cb.cur_dev);
    memset(&msc_cb, 0, sizeof(msc_cb));
    msc_cb.fname = &fname_buf[0];

    led_music_busy();

    bool mount = fs_mount();
    bool scan = pf_scan_music(1);

    printf("mount:%d, scan:%d\n", mount, scan);

    if ((!mount) || (!scan))
    {
        msc_cb.cur_dev = sys_cb.cur_dev;
        if (!bsp_music_auto_next_device(msc_cb.cur_dev))
        {
            msc_cb.cur_dev = sys_cb.cur_dev;
            func_cb.sta = FUNC_NULL;
            return;
        }
    }

    func_cb.mp3_res_play = bsp_func_music_mp3_res_play;
    msc_cb.cur_dev = sys_cb.cur_dev;
    param_msc_num_read();
    msc_cb.file_change = 1;
    msc_cb.brkpt_flag = 1;

}

void func_music_play(bool sta)
{
    printf("%s:%d\n", __func__, sta);

    if (sta == false)
    {
        music_breakpoint_save();
        music_control(MUSIC_MSG_STOP);
        if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1))
        {
            sd0_stop(1);
        }
#if BT_EMIT_EN
//        bt_emit_disable();
#endif
#if BT_BACKSTAGE_MUSIC_EN
        bt_audio_enable();
#endif
        sys_cb.local_music_sta = false;
    }
    else if (sta == true)
    {
        if (dev_is_online(DEV_UDISK) || dev_is_online(DEV_SDCARD))
        {
            func_music_init();
#if BT_EMIT_EN
            if (bt_is_connected())
            {
                bt_music_avdtp_start();
                delay_5ms(2);
                //debug
                sys_cb.vol = 10;
                bsp_set_volume(sys_cb.vol);
                bsp_bt_vol_change();
                bt_music_slave_set_volume();
                bt_emit_enable();
            }

#endif
            sys_cb.local_music_sta = true;
        }
    }


}

bool func_music_is_play(void)
{
    return sys_cb.local_music_sta;
}

void func_music_set_play_btn_pic(void)
{
    compo_arc_t *arc = compo_getobj_byid(COMPO_ID_PLAY_PROC);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    if (msc_cb.pause)
    {
        compo_button_set_bgimg(btn, UI_BUF_MUSIC_PLAY_BIN);
        compo_arc_set_visible(arc, false);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_MUSIC_PAUSE_BIN);
        compo_arc_set_visible(arc, true);
    }
}

void func_music_set_vol_btn_pic(void)
{
    f_music_t *f_msc = (f_music_t *)func_cb.f_cb;
    compo_picturebox_t *vol_pic;

    if (f_msc->vol == sys_cb.vol)
    {
        return;
    }
    f_msc->vol = sys_cb.vol;

    vol_pic = compo_getobj_byid(COMPO_ID_PIC_MUSIC_VOL);
    switch (f_msc->vol)
    {
        case 0:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME1_BIN);
            compo_picturebox_set_visible(vol_pic, false);
            break;

        case 1 ... 3:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME1_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;

        case 4 ... 6:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME2_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;
        case 7 ... 9:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME3_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;
        case 10 ... 11:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME4_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;
        case 12 ... 16:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME5_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;
    }
}

//创建本地音乐播放器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_music_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MUSIC]);

    //歌名
    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 100, 0, 0);
    compo_textbox_set_autosize(name_txt, true);
    compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);
    compo_textbox_set(name_txt, "    ");

    //歌词
    compo_textbox_t *time_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 50, GUI_SCREEN_WIDTH, 50);
    compo_textbox_set_autosize(time_txt, true);
    compo_setid(time_txt, COMPO_ID_TXT_MUSIC_TIME);
    compo_textbox_set(time_txt, "  ");

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

    compo_picturebox_t *vol_pic = compo_picturebox_create(frm, UI_BUF_MUSIC_VOLUME1_BIN);
    compo_setid(vol_pic, COMPO_ID_PIC_MUSIC_VOL);
    compo_picturebox_set_pos(vol_pic, 110, 335);
    widget_set_align_center(vol_pic->img, false);
    compo_picturebox_set_visible(vol_pic, false);

    //my test
    compo_arc_t *arc = compo_arc_create(frm);
    compo_setid(arc, COMPO_ID_PLAY_PROC);
    compo_arc_set_alpha(arc, 0xff, 0);
    compo_arc_set_location(arc, 160, 245, 111, 111);
    compo_arc_set_width(arc, 3);
    compo_arc_set_rotation(arc, 0);
    compo_arc_set_angles(arc, 0, 3600);
    compo_arc_set_color(arc, make_color(241, 64, 202), make_color(241, 64, 202));
    compo_arc_set_value(arc, 0);

    return frm;
}

//触摸单击按钮
static void func_music_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_PREV:
            bsp_music_prev();
            break;

        case COMPO_ID_BTN_NEXT:
            bsp_music_next();
            break;

        case COMPO_ID_BTN_PLAY:
            bsp_music_play_pause();
            func_music_set_play_btn_pic();
            break;

        case COMPO_ID_BTN_VOL_UP:
            bsp_set_volume(bsp_volume_inc(sys_cb.vol));
#if BT_EMIT_EN
            bt_music_slave_set_volume();
#endif
            break;

        case COMPO_ID_BTN_VOL_DOWN:
            bsp_set_volume(bsp_volume_dec(sys_cb.vol));
#if BT_EMIT_EN
            bt_music_slave_set_volume();
#endif
            break;

        default:
            break;
    }
}

//本地音乐消息处理
static void func_music_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_music_button_click();                         //单击按钮
            break;

        case EVT_SD_REMOVE:
            func_music_remove_device(DEV_SDCARD);
            break;

        case EVT_UDISK_REMOVE:
            func_music_remove_device(DEV_UDISK);
            break;

        case EVT_SD_INSERT:
            func_music_play(true);
            func_music_insert_device(DEV_SDCARD);
            break;

        case EVT_UDISK_INSERT:
            func_music_insert_device(DEV_UDISK);
            break;

        case KU_BACK:

            break;

        case MSG_SYS_500MS:
        {
            compo_arc_t *arc = compo_getobj_byid(COMPO_ID_PLAY_PROC);
            u16 total_time = music_get_total_time();
            u16 cur_time = music_get_cur_time() / 10;
            u16 proc = ARC_VALUE_MAX * cur_time / total_time;
//            printf("total:%d, cur:%d, proc:%d\n", total_time, cur_time, proc);
            compo_arc_set_value(arc, proc);
            func_music_set_play_btn_pic();
        }
        break;

        case MSG_SYS_1S:

            break;

        default:
            func_message(msg);
            break;
    }
}

AT(.text.func.music)
void func_music_mp3_res_play(u32 addr, u32 len)
{
    u32 cur_time;

    if (len == 0)
    {
        return;
    }

    msc_breakpiont_t brkpt;
    music_get_breakpiont(&brkpt);           //保存当前播放位置
    cur_time = music_get_cur_time();
    music_control(MUSIC_MSG_STOP);

    mp3_res_play(addr, len);

    music_decode_init();
    music_set_jump(&brkpt);                 //恢复播放位置
    music_set_cur_time(cur_time);
    if (msc_cb.pause)
    {
        music_control(MUSIC_MSG_PAUSE);
    }
    else
    {
        music_control(MUSIC_MSG_PLAY);
    }
}

AT(.text.func.music)
void func_music_remove_device(u8 dev)
{
    if (msc_cb.cur_dev == dev)
    {
        if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_UDISK))
        {
            msc_cb.dev_change = 1;
            music_breakpoint_save();
        }
        else
        {
            func_cb.sta = FUNC_NULL;
        }
    }
}

AT(.text.func.music)
void func_music_insert_device(u8 dev)
{
    sys_cb.cur_dev = dev;
    msc_cb.dev_change = 2;
    music_breakpoint_save();
}

AT(.text.func.music)
void func_music_get_curtime(void)
{
    u16 cur_sec, min, sec;
    cur_sec = music_get_cur_time() / 10;
    min = cur_sec / 60;
    sec = cur_sec % 60;
    if (msc_cb.curtime.min != min || msc_cb.curtime.sec != sec)
    {
        msc_cb.curtime.min = min;
        msc_cb.curtime.sec = sec;
        func_music_set_music_time_text();
    }
}

AT(.text.func.music)
void func_music_file_new(void)
{
    f_music_t *f_msc = (f_music_t *)func_cb.f_cb;

    if (msc_cb.file_change)
    {
        msc_cb.file_change = 0;
        music_control(MUSIC_MSG_STOP);
        if (fs_open_num(msc_cb.file_num))
        {
            memset(msc_cb.fname, 0, sizeof(fname_buf));
            fs_get_short_fname(msc_cb.fname, 0);
            msc_cb.dir_num = fs_get_dirs_count();        //获取当前文件所在文件夹编号
            msc_cb.alltime.min = 0xff;
            msc_cb.alltime.sec = 0;
            msc_cb.curtime.min = 0;
            msc_cb.curtime.sec = 0;
            msc_cb.pause = 0;
            msc_cb.encrypt = 0;

#if MUSIC_ENCRYPT_EN
            if (!music_is_encrypt(MUSIC_ENCRYPT_KEY))
            {
                msc_cb.encrypt = 1;
            }
#endif // MUSIC_ENCRYPT_EN

            msc_cb.fname_update = 1;
            fs_get_filename(msc_cb.fname, sizeof(fname_buf));        //获取长文件名
            msc_cb.fname[99] |= FB_GOT_FLN;                          //已获取长文件名
            memset(f_msc->fname_buf, 0, sizeof(f_msc->fname_buf));
            if (msc_cb.fname[99] & FB_GBK)
            {
                memcpy(f_msc->fname_buf, msc_cb.fname, msc_cb.fname[98]);
            }
            else
            {
                sys_unicode_to_utf8((u8 *)f_msc->fname_buf, (u16 *)msc_cb.fname, msc_cb.fname[98]);
            }
            func_music_set_tilte_name(f_msc->fname_buf);
            memset(msc_cb.fname, 0, sizeof(fname_buf));
            fs_get_short_fname(msc_cb.fname, 0);

#if MUSIC_LRC_EN
            bsp_lrc_init();
#endif
#if MUSIC_ID3_TAG_EN
            if (strstr(msc_cb.fname, ".mp3") != NULL)
            {
                get_mp3_id3_tag();
            }
#endif

            msc_cb.type = music_decode_init();
            if(msc_cb.type != NONE_TYPE)
            {
#if MUSIC_ID3_TAG_EN
                if (strstr(msc_cb.fname, ".wma") != NULL)
                {
                    get_wma_id3_tag();
                }
#endif
                printf("music decoding [%s]\n", msc_cb.fname);
                printf("new file number: %d, %d\n", msc_cb.file_num, msc_cb.file_total);
                param_msc_num_write();
//                gui_box_show_num(msc_cb.file_num);
                music_breakpoint_init();
                param_sync();
                led_music_play();
                music_control(MUSIC_MSG_PLAY);             //开始播放
                delay_5ms(1);
            }
        }
    }
    msc_cb.brkpt_flag = 0;
}

AT(.text.func.music)
void func_music_device_new(void)
{
    u8 dev_change = msc_cb.dev_change;
    if (msc_cb.dev_change)
    {
        msc_cb.dev_change = 0;
        music_control(MUSIC_MSG_STOP);
        if ((dev_change == 1) && (!bsp_music_switch_device(msc_cb.cur_dev)))
        {
            return;
        }

        led_music_busy();
        fsdisk_callback_init(sys_cb.cur_dev);
        if ((!fs_mount()) || (!pf_scan_music(1)))
        {
            //还原到原设备
            sys_cb.cur_dev = msc_cb.cur_dev;
            fsdisk_callback_init(sys_cb.cur_dev);
            if ((!fs_mount()) || (!pf_scan_music(0)))
            {
                func_cb.sta = FUNC_NULL;
                led_idle();
                return;
            }
        }
        led_idle();
        msc_cb.cur_dev = sys_cb.cur_dev;
        param_msc_num_read();
        msc_cb.brkpt_flag = 1;
        msc_cb.file_change = 1;
    }
}

AT(.text.func.music)
void func_music_process(void)
{
    u16 total_time;

    func_process();

    func_music_set_vol_btn_pic();
    func_music_file_new();
    music_slot_kick();
    func_music_get_curtime();
    if ((get_music_dec_sta() == MUSIC_STOP) && (msc_cb.dev_change == 0))
    {
        if (dev_is_online(msc_cb.cur_dev))            //设备拔出结束解码不自动切换下一曲
        {
            music_breakpoint_clr();
            if ((msc_cb.prev_flag) && ((msc_cb.alltime.min == 0xff)
                                       || ((msc_cb.curtime.min == 0) && (msc_cb.curtime.sec == 0))))
            {
                //错误文件或播放小于2S保持切换方向
                bsp_music_switch_file(0);
            }
            else
            {
                bsp_music_auto_next_file();
            }
        }
    }

    if (msc_cb.alltime.min == 0xff)
    {
        total_time = music_get_total_time();
        if (total_time != 0xffff)
        {
            msc_cb.alltime.min = total_time / 60;
            msc_cb.alltime.sec = total_time % 60;
            func_music_set_music_time_text();
            printf("[%s] total time: %02d:%02d\n\n", msc_cb.fname, msc_cb.alltime.min, msc_cb.alltime.sec);
        }
    }

    func_music_device_new();

#if MUSIC_LRC_EN
    bsp_lrc_get_content();
#endif // MUSIC_LRC_EN
}

static void func_music_enter(void)
{
#if 1
//    msg_queue_clear();
    func_cb.f_cb = func_zalloc(sizeof(f_music_t));
    func_cb.frm_main = func_music_form_create();

    if (1)
    {
        if (func_music_is_play())
        {
            printf("msc:already play, file_num[%d]\n", msc_cb.file_num);
            fs_get_filename_for_number(msc_cb.file_num, 1, msc_cb.fname, sizeof(fname_buf));
            msc_cb.fname[FUNC_MUSIC_NAME_BUF_LEN - 1] |= FB_GOT_FLN; //已获取长文件名
            func_music_set_tilte_name(msc_cb.fname);
        }
        else
        {
            func_music_play(true);
        }
    }
    else
    {
        printf("msc:bt not connect\n");
    }
    return;
#else
    SD0_LDO_EN_EN();
    msg_queue_clear();
    if (!dev_is_online(DEV_UDISK) && !dev_is_online(DEV_SDCARD))
    {
        func_cb.sta = FUNC_NULL;
        return;
    }

#if BT_BACKSTAGE_EN && MUSIC_UDISK_EN
    if (dev_is_online(DEV_UDISK))
    {
        udisk_resume();
    }
#endif
#if BT_BACKSTAGE_MUSIC_EN
    bt_audio_bypass();
    if (bt_is_playing())
    {
        bt_music_pause();
    }
#endif
    func_music_filter_set();
    fsdisk_callback_init(sys_cb.cur_dev);
    memset(&msc_cb, 0, sizeof(msc_cb));
    msc_cb.fname = &fname_buf[0];

    func_cb.f_cb = func_zalloc(sizeof(f_music_t));
    func_cb.frm_main = func_music_form_create();

    led_music_busy();
    if ((!fs_mount()) || (!pf_scan_music(1)))
    {
        msc_cb.cur_dev = sys_cb.cur_dev;
        if (!bsp_music_auto_next_device(msc_cb.cur_dev))
        {
            msc_cb.cur_dev = sys_cb.cur_dev;
            func_cb.sta = FUNC_NULL;
            led_idle();
            return;
        }
    }

    led_idle();
    func_cb.mp3_res_play = func_music_mp3_res_play;
    msc_cb.cur_dev = sys_cb.cur_dev;
    param_msc_num_read();
    msc_cb.file_change = 1;
    msc_cb.brkpt_flag = 1;
    func_music_set_play_btn_pic();
#endif
}

static void func_music_exit(void)
{
    func_music_play(false);
#if BT_BACKSTAGE_MUSIC_EN
    bt_audio_enable();
#endif
#if BT_BACKSTAGE_EN && MUSIC_UDISK_EN
    if (dev_is_online(DEV_UDISK))
    {
        udisk_suspend();
    }
#endif
    if (func_cb.sta == FUNC_NULL)
    {
        func_back_to();
    }
    func_cb.last = FUNC_MUSIC;
}


AT(.text.func.music)
void func_music(void)
{
    printf("%s\n", __func__);

    func_music_enter();

    while (func_cb.sta == FUNC_MUSIC)
    {
        func_music_process();
        func_music_message(msg_dequeue());
    }
    func_music_exit();
}
#endif // FUNC_MUSIC_EN
