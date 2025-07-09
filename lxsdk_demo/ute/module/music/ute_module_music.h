/**
*@details
*@author       zn.zeng
*@date       2021-11-20
*@version      v1.0
*/
#ifndef _UTE_MODULE_MUSIC_H_
#define _UTE_MODULE_MUSIC_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"

typedef enum
{
    UTE_MUSIC_PLAY_CHANNEL_PHONE_TO_SPEAKER=0,
    UTE_MUSIC_PLAY_CHANNEL_LOCAL_TO_SPEAKER,
    UTE_MUSIC_PLAY_CHANNEL_LOCAL_TO_EARPHONE,
    UTE_MUSIC_PLAY_CHANNEL_MAX,
} ute_module_music_play_channel_t;

typedef enum
{
    UTE_MUSIC_NONE=0,    //空闲时候
    UTE_MUSIC_MICRECORD, //录音时候
    UTE_MUSIC_CHARGE,    //充电时候
    UTE_MUSIC_LOCALRINGTONE,    //本地铃声播放时候
    UTE_MUSIC_CALL,    //来电时候
    UTE_MUSIC_OTA,    //OTA时候
    UTE_MUSIC_LOWPOWER, //低电时候
    UTE_MUSIC_USB_PLUG_IN, //插入充电器时候
    UTE_MUSIC_TTS_PLAY, //语音合成
} UTE_MUSIC_PAUSE_PLAYBACK_T;

typedef enum
{
    UTE_UI_MUSIC_CTRL_VOL=0, // 进入音量设置
    UTE_UI_MUSIC_CTRL_VOL_UP,// 音量加
    UTE_UI_MUSIC_CTRL_VOL_DOWN,// 音量减
    UTE_UI_MUSIC_CTRL_PREV,// 上一曲
    UTE_UI_MUSIC_CTRL_NEXT,// 下一曲
    UTE_UI_MUSIC_CTRL_PLAY_AND_PAUSED,// 播放暂停
    UTE_UI_MUSIC_CTRL_EARPHONE,// 耳机断开或连接
    UTE_UI_MUSIC_CTRL_LOOP_SWITCH,// 歌曲循环的模式切换
    UTE_UI_MUSIC_CTRL_LYRICS,// 歌词
    UTE_UI_MUSIC_CTRL_SONG_LIST,// 进入音乐播放列表
    UTE_UI_MUSIC_CTRL_CHANNEL_SWITCH,  // 声音路径切换
    UTE_UI_MUSIC_CTRL_CHANNEL_LOCAL,// 本地播放
    UTE_UI_MUSIC_CTRL_CHANNEL_PHONE,// 手机播放
    UTE_UI_MUSIC_CTRL_GOTO_CHANNEL_SWITCH,// 进入声音路径切换界面
    UTE_UI_MUSIC_CTRL_GOTO_CEARPHONE_SEARCH_LIST,  //进入搜索耳机设备界面
    UTE_UI_MUSIC_CTRL_GOTO_EARPHONE_BOND_LIST,  //跳转绑定耳机列表
    UTE_UI_MUSIC_CTRL_MAX,
} UTE_MUSIC_UI_CTRL_T;

typedef enum
{
    UTE_UI_MUSIC_MAIN=0,
    UTE_UI_MUSIC_VOLUME,
    UTE_UI_MUSIC_MODE_SWITCH,
} UTE_MUSIC_UI_VIEW_T;

typedef enum
{
    UTE_MUSIC_LOOP_ORDER=0,
    UTE_MUSIC_LOOP_RANDOM,
    UTE_MUSIC_LOOP_SINGLE,
    UTE_MUSIC_LOOP_MAX,
} ute_module_music_loop_mode_t;

typedef struct
{
    uint8_t volume;
    ute_module_music_loop_mode_t loopMode;
    bool isPaused;
    uint32_t playerCurrTimeMs;
    uint32_t playerTatolTimeMs;
    uint8_t playerArtist[UTE_MUSIC_ARTLIST_MAX_SIZE];
    uint16_t playerArtistSize;
    uint8_t playerTitle[UTE_MUSIC_TITLE_MAX_SIZE];
    uint16_t playerTitleSize;
    uint8_t androidCrc;
    uint32_t avrvpConnCmplRtc;
    ute_module_music_play_channel_t playChannel;
    bool isPlayingA2dp;
    UTE_MUSIC_UI_VIEW_T musicControlView; // 0:main  1:volume  2:mode switch
    bool isUserSwitchPlayChanel; // 用户手动切换播放通道，这里用来判断是手动切换还是自动
    uint16_t swtichMusicModeprogress; //切换模式的进程，因为进程可能会堵塞，要按照顺序切换；
    UTE_MUSIC_PAUSE_PLAYBACK_T PauseplaybackStatus;
} ute_module_music_data_t;

void uteModuleMusicInit(void);
void uteModuleMusicSetPlayerPaused(bool isPaused,ute_module_music_play_channel_t chn);
bool uteModuleMusicGetPlayerPaused(void);
uint32_t uteModuleMusicAmsConversionPlayerTimeMs(uint8_t *data,uint16_t size);
void uteModuleMusicSetPlayerCurrTime(uint32_t ms);
void uteModuleMusicSetPlayerTotalTime(uint32_t ms);
void uteModuleMusicSetPlayerArtist(uint8_t *data,uint16_t size);
void uteModuleMusicSetPlayerTitle(uint8_t *data,uint16_t size);
void uteModuleMusicSetPlayerVolume(uint8_t volume);
uint8_t uteModuleMusicGetPlayerVolume(void);
void uteModuleMusicGetPlayerTitle(uint8_t *data,uint16_t *size);
void uteModuleMusicGetPlayerArtistSize(uint8_t *data,uint16_t *size);
void uteModuleMusicGetPlayerData(ute_module_music_data_t *data);
void uteModuleMusicSetPlayerData(ute_module_music_data_t data);
void uteModuleMusicResetPlayStatus(void);
void uteModuleMusicCtrlPaused(bool isNeedVibration);
void uteModuleMusicCtrl(bool isCutNext,bool isCutManual,bool isNeedShake); //add by pcm  2022-07-30
void uteModuleMusicCtrlVolumeIncrease(bool isNeedShake);
void uteModuleMusicCtrlVolumeDecrease(bool isNeedShake);
void uteModuleMusicAvrcpConnCmpl(void);
bool uteModuleMusicIsSetMaxVolume(void);
ute_module_music_play_channel_t uteModuleMusicGetPlayChannel(void);
void uteModuleMusicSetPlayingA2dpStatus(bool isPlaying);
bool uteModuleMusicGetPlayingA2dpStatus(void);
void uteModuleMusicSetControlView(UTE_MUSIC_UI_VIEW_T controlView);
UTE_MUSIC_UI_VIEW_T uteModuleMusicGetControlView(void);
#endif //_UTE_MODULE_MUSIC_H_

