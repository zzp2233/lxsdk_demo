#ifndef _BSP_MUSIC_H
#define _BSP_MUSIC_H

#include "bsp_record.h"
#include "bsp_fs.h"
#include "music_lrc.h"
#include "music_id3_tag.h"
#include "music_res.h"
#include "bsp_func_music.h"

enum
{
    NORMAL_MODE,
    SINGLE_MODE,
    FLODER_MODE,
    RANDOM_MODE,
};

typedef struct
{
    u8 min;                     //minute
    u8 sec;                     //second
} msc_time_t;

typedef struct
{
    u32 frame_count;            //current frame count
    u32 file_ptr;               //file ptr of current frame
    u16 fname_crc;              //file name crc
} msc_breakpiont_t;

typedef struct
{
    u8 pause        : 1,
    file_change  : 1,
    dev_change   : 2,
    //cur_dev      : 2,
    brkpt_flag   : 1,
    prev_flag    : 1;
    u8 cur_dev;

    u8 rec_type     : 1,
    rec_scan     : 2,
    encrypt      : 1;

    u8 type;                    //音乐格式
    u32 bit_rate;               //音乐码率

    char *fname;

    msc_time_t alltime;         //music file total time
    msc_time_t curtime;         //music current playing time

    u16 file_num;
    u16 file_total;

    u16 dir_num;                //directory current number
    u16 dir_total;              //directory total number

#if MUSIC_BREAKPOINT_EN
    msc_breakpiont_t brkpt;     //music breakpoint info
#endif // MUSIC_BREAKPOINT_EN

#if MUSIC_LRC_EN
    u8 lrc_sta;
    u8 lrc_encoding;
#endif // MUSIC_LRC_EN

    u8 fname_update : 1,
    lrc_update   : 1;
} bsp_msc_t;
extern bsp_msc_t msc_cb;


bool pf_scan_music(u8 new_dev);
void music_playmode_next(void);

bool bsp_music_switch_device(u8 cur_dev);           //切换下一个播放设备
bool bsp_music_auto_next_device(u8 cur_dev);        //自动切换下一个播放设备
void bsp_music_switch_file(u8 direction);
void bsp_music_auto_next_file(void);
bool bsp_music_filter_switch(u8 rec_type);          //切换播放录音文件或不播放录音文件
void bsp_music_select_file(u16 file_num);           //选择指定编号文件播放
void bsp_music_switch_dir(u8 direction);            //文件夹切换  direction: 0->上一文件夹,    1->下一文件夹
void bsp_music_select_dir(u16 sel_num);             //选择指定编号文件夹播放
void bsp_music_play_pause(void);                    //播放/暂停切换


#define bsp_music_prev()            bsp_music_switch_file(0)        //上一曲
#define bsp_music_next()            bsp_music_switch_file(1)        //下一曲

#if MUSIC_BREAKPOINT_EN
void bsp_music_breakpoint_init(void);
void bsp_music_breakpoint_save(void);
void bsp_music_breakpoint_clr(void);

#define music_breakpoint_init()     bsp_music_breakpoint_init()
#define music_breakpoint_save()     bsp_music_breakpoint_save()
#define music_breakpoint_clr()      bsp_music_breakpoint_clr()
#else
#define music_breakpoint_init()
#define music_breakpoint_save()
#define music_breakpoint_clr()
#endif // MUSIC_BREAKPOINT_EN

#endif //_BSP_MUSIC_H
