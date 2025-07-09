#include "include.h"

#define FS_CRC_SEED         0xffff

uint calc_crc(void *buf, uint len, uint seed);

#if FUNC_MUSIC_EN

bsp_msc_t msc_cb AT(.buf.music);

//扫描全盘文件
bool pf_scan_music(u8 new_dev)
{
    if (new_dev)
    {
#if USB_SD_UPDATE_EN
        func_update();                                  //尝试升级
#endif // USB_SD_UPDATE_EN
    }

#if REC_FAST_PLAY
    msc_cb.rec_scan = BIT(0);
    sys_cb.rec_num = 0;
    sys_cb.ftime = 0;
#endif // REC_FAST_PLAY

    msc_cb.file_total = fs_get_total_files();
    if (!msc_cb.file_total)
    {
        msc_cb.dir_total = 0;
        return false;
    }

#if REC_FAST_PLAY
    msc_cb.rec_scan = 0;
#endif // REC_FAST_PLAY

    msc_cb.dir_total = fs_get_dirs_count();          //获取文件夹总数
    return true;
}

AT(.text.func.music)
bool bsp_music_switch_device(u8 cur_dev)
{
    if (cur_dev == DEV_SDCARD)
    {
        if (dev_is_online(DEV_UDISK))
        {
            sys_cb.cur_dev = DEV_UDISK;
            return true;
        }
    }
    else if (cur_dev == DEV_UDISK)
    {
        if (dev_is_online(DEV_SDCARD))
        {
            sys_cb.cur_dev = DEV_SDCARD;
            return true;
        }
    }
    return false;
}

AT(.text.func.music)
bool bsp_music_auto_next_device(u8 cur_dev)
{
    if (!bsp_music_switch_device(cur_dev))
    {
        return false;
    }
    led_music_busy();
    fsdisk_callback_init(sys_cb.cur_dev);
    if ((!fs_mount()) || (!pf_scan_music(1)))
    {
        //挂载失败或无文件时还原到原设备
        fsdisk_callback_init(cur_dev);
        if ((!fs_mount()) || (!pf_scan_music(1)))
        {
            led_idle();
            return false;
        }
        return false;
    }
    led_idle();
    return true;
}

//direction: 0->上一曲,    1->下一曲
AT(.text.func.music)
void bsp_music_switch_file(u8 direction)
{
    u16 dir_snum, dir_lnum;
    music_control(MUSIC_MSG_STOP);
    switch (sys_cb.play_mode)
    {
        case NORMAL_MODE:
        case SINGLE_MODE:
            if (direction)
            {
#if MUSIC_AUTO_SWITCH_DEVICE
                //auto switch to next device
                if (msc_cb.file_num == msc_cb.file_total)
                {
                    if (bsp_music_auto_next_device(msc_cb.cur_dev))
                    {
                        msc_cb.cur_dev = sys_cb.cur_dev;
                        msc_cb.file_num = 1;
                        msc_cb.file_change = 1;
                        return;
                    }
                }
#endif // MUSIC_AUTO_SWITCH_DEVICE

                msc_cb.file_num++;
                if (msc_cb.file_num > msc_cb.file_total)
                {
                    msc_cb.file_num = 1;
                }
            }
            else
            {
#if MUSIC_AUTO_SWITCH_DEVICE
                //auto switch to next device
                if (msc_cb.file_num == 1)
                {
                    if (bsp_music_auto_next_device(msc_cb.cur_dev))
                    {
                        msc_cb.cur_dev = sys_cb.cur_dev;
                        msc_cb.file_num = msc_cb.file_total;
                        msc_cb.file_change = 1;
                        return;
                    }
                }
#endif // MUSIC_AUTO_SWITCH_DEVICE

                msc_cb.file_num--;
                if ((msc_cb.file_num < 1) || (msc_cb.file_num > msc_cb.file_total))
                {
                    msc_cb.file_num = msc_cb.file_total;
                }
            }
            break;

        case FLODER_MODE:
            dir_snum = fs_get_dir_fstart();                     //获取当前文件夹起始文件编号
            dir_lnum = dir_snum + fs_getdir_files() - 1;        //获取当前文件夹结束文件编号
            if (direction)
            {
                msc_cb.file_num++;
                if (msc_cb.file_num > dir_lnum)
                {
                    msc_cb.file_num = dir_snum;
                }
            }
            else
            {
                msc_cb.file_num--;
                if ((msc_cb.file_num < dir_snum) || (msc_cb.file_num > dir_lnum))
                {
                    msc_cb.file_num = dir_lnum;
                }
            }
            printf("Floder play mode: %d,%d\n", msc_cb.file_num, dir_lnum);
            break;

        case RANDOM_MODE:
            msc_cb.file_num = get_random(msc_cb.file_total) + 1;
            break;
    }

    msc_cb.file_change = 1;
    if (direction)
    {
        msc_cb.prev_flag = 0;
    }
    else
    {
        msc_cb.prev_flag = 1;
    }
}

AT(.text.func.music)
void bsp_music_auto_next_file(void)
{
    msc_cb.file_change = 1;
    msc_cb.prev_flag = 0;
    if (sys_cb.play_mode != SINGLE_MODE)
    {
        bsp_music_switch_file(1);
    }
}

//选择指定编号文件播放
AT(.text.func.music)
void bsp_music_select_file(u16 file_num)
{
    bool ret = false;
//    printf("%s: file_num[%d] total[%d]\n", __func__, file_num, msc_cb.file_total);

    if ((file_num < 1) || (file_num > msc_cb.file_total))
    {
        return;
    }

    msc_cb.file_num = file_num;
    music_control(MUSIC_MSG_STOP);         //先结束当前播放
    ret = fs_open_num(msc_cb.file_num);
    if (!ret)
    {
        msc_cb.file_change = 1;
        msc_cb.prev_flag = 0;
    }
}

#if MUSIC_FOLDER_SELECT_EN
//direction: 0->上一文件夹,    1->下一文件夹
AT(.text.func.music)
void bsp_music_switch_dir(u8 direction)
{
    u16 dir_file_num;
    if (msc_cb.dir_total <= 1)
    {
        return;
    }
    if (direction)
    {
        msc_cb.dir_num++;
    }
    else
    {
        msc_cb.dir_num--;
    }
    if (msc_cb.dir_num > msc_cb.dir_total)
    {
        msc_cb.dir_num = 1;
    }
    else if (msc_cb.dir_num < 1)
    {
        msc_cb.dir_num = msc_cb.dir_total;
    }
    music_control(MUSIC_MSG_STOP);         //先结束当前播放
    dir_file_num = fs_open_dir_num(msc_cb.dir_num);
    if (dir_file_num > 0)
    {
        msc_cb.file_num = dir_file_num;
    }
    msc_cb.file_change = 1;
    msc_cb.prev_flag = 0;
    bsp_clr_mute_sta();
}

//选择指定编号文件夹播放
AT(.text.func.music)
void bsp_music_select_dir(u16 sel_num)
{
    u16 dir_file_num;
    if ((sel_num < 1) || (sel_num > msc_cb.dir_total))
    {
        return;
    }
    msc_cb.dir_num = sel_num;
    music_control(MUSIC_MSG_STOP);         //先结束当前播放
    dir_file_num = fs_open_dir_num(msc_cb.dir_num);
    if (dir_file_num > 0)
    {
        msc_cb.file_num = dir_file_num;
    }
    msc_cb.file_change = 1;
    msc_cb.prev_flag = 0;
}

#endif // MUSIC_FOLDER_SELECT_EN

#if MUSIC_REC_FILE_FILTER
//切换播放录音文件或不播放录音文件
AT(.text.func.music)
bool bsp_music_filter_switch(u8 rec_type)
{
    u16 file_num = msc_cb.file_num;

    music_control(MUSIC_MSG_STOP);
    msc_cb.file_change = 1;
    if (rec_type)
    {
        fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_only_record_file_filter, music_only_record_dir_filter);       //只播放录音文件
        if (!pf_scan_music(0))
        {
            //无录音文件，还原到正常音乐
            fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_file_filter, music_rm_record_dir_filter);                 //不播放录音文件
            pf_scan_music(0);
            msc_cb.file_num = file_num;
            return false;
        }
    }
    else
    {
        fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_file_filter, music_rm_record_dir_filter);                     //不播放录音文件
        if (!pf_scan_music(0))
        {
            fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_only_record_file_filter, music_only_record_dir_filter);   //只播放录音文件
            pf_scan_music(0);
            msc_cb.file_num = file_num;
            return false;
        }
    }
    msc_cb.file_num = 1;
    return true;
}
#endif // MUSIC_REC_FILE_FILTER

void bsp_music_play_pause(void)
{
    if (msc_cb.pause)
    {
        msc_cb.pause = 0;
        music_control(MUSIC_MSG_PLAY);
        led_music_play();
        printf("music playing\n");
    }
    else
    {
        msc_cb.pause = 1;
        bsp_clr_mute_sta();
        music_control(MUSIC_MSG_PAUSE);
        led_idle();
        printf("music pause\n");
    }
}

#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_clr(void)
{
    msc_cb.brkpt.file_ptr = 0;
    msc_cb.brkpt.frame_count = 0;
    param_msc_breakpoint_write();
    param_sync();
    //printf("%s\n", __func__);
}

void bsp_music_breakpoint_init(void)
{
    int clr_flag = 0;
    if (msc_cb.brkpt_flag)
    {
        msc_cb.brkpt_flag = 0;
        param_msc_breakpoint_read();
        //printf("restore: %d, %d, %04x\n", msc_cb.brkpt.file_ptr, msc_cb.brkpt.frame_count, msc_cb.brkpt.fname_crc);
        if (calc_crc(msc_cb.fname, 8, FS_CRC_SEED) == msc_cb.brkpt.fname_crc)
        {
            music_set_jump(&msc_cb.brkpt);
        }
        else
        {
            clr_flag = 1;
        }
    }
    msc_cb.brkpt.fname_crc = calc_crc(msc_cb.fname, 8, FS_CRC_SEED);
    msc_cb.brkpt.file_ptr = 0;
    msc_cb.brkpt.frame_count = 0;
    if (clr_flag)
    {
        param_msc_breakpoint_write();
    }
}

void bsp_music_breakpoint_save(void)
{
    music_get_breakpiont(&msc_cb.brkpt);
    param_msc_breakpoint_write();
    param_sync();
    //printf("save: %d, %d, %04x\n", msc_cb.brkpt.file_ptr, msc_cb.brkpt.frame_count, msc_cb.brkpt.fname_crc);
}
#endif // MUSIC_BREAKPOINT_EN


#endif // FUNC_MUSIC_EN



