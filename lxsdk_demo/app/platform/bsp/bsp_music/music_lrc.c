#include "include.h"

#if MUSIC_LRC_EN

u8 lrc_buf[512] AT(.lrc_buf);
char lrc_lfn[100] AT(.lrc_buf);
extern char fname_buf[100];

AT(.text.lrc.init)
void bsp_lrc_init(void)
{
    msc_cb.lrc_sta = 0;
    if (!(msc_cb.fname[FUNC_MUSIC_NAME_BUF_LEN - 1] & FB_GOT_FLN)) {
        fs_get_filename(msc_cb.fname, sizeof(fname_buf));        //获取长文件名
        msc_cb.fname[FUNC_MUSIC_NAME_BUF_LEN - 1] |= FB_GOT_FLN;
    }
    memcpy(lrc_lfn, msc_cb.fname, sizeof(fname_buf));
    if (fs_open_lrc(lrc_lfn) == FR_OK) {
        msc_cb.lrc_sta = 1;
        msc_cb.lrc_encoding = lrc_init(lrc_buf);
        if (msc_cb.lrc_sta == LRC_FNAME_DISP_SECS) {
            msc_cb.lrc_update = music_get_lrc(fname_buf, lrc_cur_lable_time());
            if (msc_cb.lrc_update) {
        #if GUI_LCD_EN
                if (msc_cb.lrc_encoding != LRC_GBK) {
                    utf16_convert_to_gbk(fname_buf, 48);
                    fname_buf[FUNC_MUSIC_NAME_BUF_LEN - 1] |= FB_GBK;
                }
        #endif // GUI_LCD_EN
            }
        }
        printf("got lrc file: %d, %d\n", msc_cb.lrc_sta, msc_cb.lrc_encoding);
    }
}

AT(.text.lrc)
void bsp_lrc_get_content(void)
{
    u32 mtime;
    if (msc_cb.lrc_sta == LRC_FNAME_DISP_SECS) {
        mtime = music_get_cur_time() + 3;
        if (music_get_lrc(fname_buf, mtime)) {
            msc_cb.lrc_update = 1;
    #if GUI_LCD_EN
            if (msc_cb.lrc_encoding != LRC_GBK) {
                utf16_convert_to_gbk(fname_buf, 48);
                fname_buf[99] |= FB_GBK;
            }
    #endif // GUI_LCD_EN
            printf("[%02d:%02d.%d] %s\n", mtime/600, (mtime%600)/10, mtime%10, fname_buf);
        }
    }
}

AT(.text.lrc)
void bsp_lrc_sta_process(void)
{
    if (msc_cb.lrc_sta && (msc_cb.lrc_sta != LRC_FNAME_DISP_SECS)) {
        msc_cb.lrc_sta++;
        if (msc_cb.lrc_sta == LRC_FNAME_DISP_SECS) {
            if (lrc_first_lable_time() <= music_get_cur_time()) {
                msc_cb.lrc_update = music_get_lrc(fname_buf, lrc_first_lable_time());
                if (msc_cb.lrc_update) {
            #if GUI_LCD_EN
                    if (msc_cb.lrc_encoding != LRC_GBK) {
                        utf16_convert_to_gbk(fname_buf, 48);
                        fname_buf[99] |= FB_GBK;
                    }
            #endif // GUI_LCD_EN
                }
            } else {
                msc_cb.lrc_sta = LRC_FNAME_DISP_SECS - 1;
            }
        }
    }
}

AT(.text.lrc)
bool bsp_lrc_is_ready(void)
{
    if (msc_cb.lrc_sta == LRC_FNAME_DISP_SECS) {
        return true;
    }
    return false;
}

#endif // MUSIC_LRC_EN
