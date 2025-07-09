#include "include.h"

#if FUNC_MUSIC_EN

void func_music_set_tilte_name(void *buf);

AT(.text.func.music)
void bsp_func_music_get_name(char *name, char *fname)
{
    if (msc_cb.fname[FUNC_MUSIC_NAME_BUF_LEN - 1] & FB_GBK) {
        memcpy(name, fname, msc_cb.fname[FUNC_MUSIC_NAME_BUF_LEN - 2]);
    } else {
        sys_unicode_to_utf8((u8 *)name, (u16 *)fname, msc_cb.fname[FUNC_MUSIC_NAME_BUF_LEN - 2]);
    }
}

AT(.text.func.music)
void bsp_func_music_mp3_res_play(u32 addr, u32 len)
{
    u32 cur_time;

    if (len == 0) {
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
    if (msc_cb.pause) {
        music_control(MUSIC_MSG_PAUSE);
    } else {
        music_control(MUSIC_MSG_PLAY);
    }
}

AT(.text.func.music)
void bsp_func_music_remove_device(u8 dev)
{
    if (msc_cb.cur_dev == dev) {
        if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_UDISK)) {
            msc_cb.dev_change = 1;
            music_breakpoint_save();
        } else {
            func_cb.sta = FUNC_NULL;printf("===> 10 sta[%d]\n", 0);
        }
    }
}

AT(.text.func.music)
void bsp_func_music_insert_device(u8 dev)
{
    sys_cb.cur_dev = dev;
    msc_cb.dev_change = 2;
    music_breakpoint_save();
}

AT(.text.func.music)
void bsp_func_music_get_curtime(void)
{
    u16 cur_sec, min, sec;
    cur_sec = music_get_cur_time() / 10;
    min = cur_sec / 60;
    sec = cur_sec % 60;
    if (msc_cb.curtime.min != min || msc_cb.curtime.sec != sec) {
        msc_cb.curtime.min = min;
        msc_cb.curtime.sec = sec;
//        func_music_set_music_time_text();
    }
}

AT(.text.func.music)
void bsp_func_music_file_new(void)
{
    if (msc_cb.file_change) {
        msc_cb.file_change = 0;
        music_control(MUSIC_MSG_STOP);

        printf("===> msc_cb.file_num[%d]\n", msc_cb.file_num);
        if (fs_open_num(msc_cb.file_num)) {
            msc_cb.dir_num = fs_get_dirs_count();        //获取当前文件所在文件夹编号
            msc_cb.alltime.min = 0xff;
            msc_cb.alltime.sec = 0;
            msc_cb.curtime.min = 0;
            msc_cb.curtime.sec = 0;
            msc_cb.pause = 0;
            msc_cb.encrypt = 0;

#if MUSIC_ENCRYPT_EN
            if (!music_is_encrypt(MUSIC_ENCRYPT_KEY)) {
                msc_cb.encrypt = 1;
            }
#endif // MUSIC_ENCRYPT_EN

            memset(msc_cb.fname, 0, sizeof(fname_buf));
//            msc_cb.fname_update = 1;
            fs_get_filename(msc_cb.fname, sizeof(fname_buf));        //获取长文件名
            msc_cb.fname[FUNC_MUSIC_NAME_BUF_LEN - 1] |= FB_GOT_FLN; //已获取长文件名

            func_music_set_tilte_name(msc_cb.fname);

            memset(msc_cb.fname, 0, sizeof(fname_buf));
            fs_get_short_fname(msc_cb.fname, 0);
//            fs_get_filename(msc_cb.fname, 10);
#if MUSIC_LRC_EN
            bsp_lrc_init();
#endif
#if MUSIC_ID3_TAG_EN
            if (strstr(msc_cb.fname, ".mp3") != NULL) {
                get_mp3_id3_tag();
            }
#endif

            msc_cb.type = music_decode_init();
            if(msc_cb.type != NONE_TYPE) {
#if MUSIC_ID3_TAG_EN
                if (strstr(msc_cb.fname, ".wma") != NULL) {
                    get_wma_id3_tag();
                }
#endif
                printf("music decoding [%s]\n", msc_cb.fname);
                printf("new file number: file_num[%d] file_total[%d] dir_num[%d]\n",
                       msc_cb.file_num, msc_cb.file_total, msc_cb.dir_num);

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
void bsp_func_music_device_new(void)
{
    u8 dev_change = msc_cb.dev_change;
    if (msc_cb.dev_change) {
        msc_cb.dev_change = 0;
        music_control(MUSIC_MSG_STOP);
        if ((dev_change == 1) && (!bsp_music_switch_device(msc_cb.cur_dev))) {
            return;
        }

        led_music_busy();
        fsdisk_callback_init(sys_cb.cur_dev);
        if ((!fs_mount()) || (!pf_scan_music(1))) {
            //还原到原设备
            sys_cb.cur_dev = msc_cb.cur_dev;
            fsdisk_callback_init(sys_cb.cur_dev);
            if ((!fs_mount()) || (!pf_scan_music(0))) {
                func_cb.sta = FUNC_NULL;printf("===> 11 sta[%d]\n", 0);
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

#endif // FUNC_MUSIC_EN



