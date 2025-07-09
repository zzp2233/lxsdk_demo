#ifndef _BSP_FUNC_MUSIC_H
#define _BSP_FUNC_MUSIC_H

#define FUNC_MUSIC_NAME_BUF_LEN      100  //歌名长度

extern char fname_buf[FUNC_MUSIC_NAME_BUF_LEN];

/**
 * @brief 获取可显示的歌名
 **/
void bsp_func_music_get_name(char *name, char *fname);

/**
 * @brief 音乐模式下播放提示音
 **/
void bsp_func_music_mp3_res_play(u32 addr, u32 len);

/**
 * @brief 音乐模式下移除设备
 **/
void bsp_func_music_remove_device(u8 dev);

/**
 * @brief 音乐模式下插入设备
 **/
void bsp_func_music_insert_device(u8 dev);

/**
 * @brief 获取本地音乐播放当前时间
 **/
void bsp_func_music_get_curtime(void);

/**
 * @brief 创建新音乐文件
 **/
void bsp_func_music_file_new(void);

/**
 * @brief 创建新设备
 **/
void bsp_func_music_device_new(void);

#endif  //_BSP_FUNC_MUSIC_H
