#ifndef _BSP_BT_H
#define _BSP_BT_H

#include "pbap.h"

//标准HID键
#define HID_KEY_ENTER       0x28

//自定义HID键
#define HID_KEY_VOL_DOWN    0x00EA
#define HID_KEY_VOL_UP      0x00E9
#define HID_KEY_IOS_HOME    0x0040      //ios home
#define HID_KEY_IOS_POWER   0x0030      //ios 锁屏
#define HID_KEY_IOS_LAYOUT  0x01AE      //ios Keyboard Layout

typedef enum
{
    IMG_POINT_UP = 0,           //上滑
    IMG_POINT_DOWN,             //下滑
    IMG_POINT_LEFT,             //左滑
    IMG_POINT_RIGHT,            //右滑
    IMG_POINT_DD,
} img_point_type;

enum bt_call_type_t
{
    CALL_TYPE_NONE,                            //无
    CALL_TYPE_PHONE,                           //普通通话
    CALL_TYPE_NETWORK,                         //网络通话
};

enum bt_id3_tag_type_t
{
    BT_ID3_TAG_TITLE,
    BT_ID3_TAG_ARTIST,
    BT_ID3_TAG_ALBUM,
};

typedef struct
{
    u16 warning_status;
    u8 disp_status;
    u8 hid_menu_flag;
    u8 hid_discon_flag;
    u8 ring_stop;
    u8 bt_is_inited;
    u8 bt_form_created;
    bool music_playing;
    bool res_bt_bypass;         //播本地资源, 挂起bt音乐标志
    bool number_sta;            //通话号码状态, false: not ready; true: ready;
    u8 rec_pause    : 1;
    u8 need_pairing : 1;
    u8 incall_flag  : 1;

    u8 call_type;               //通话类型
    u16 times;
} bsp_bt_t;
extern bsp_bt_t bt_cb;
extern uint8_t cfg_bt_sniff_clk_sel;

void bsp_bt_init(void);
void bsp_bt_close(void);
void bsp_bt_vol_change(void);
void bsp_bt_hid_photo(void);
bool bsp_bt_pwrkey5s_check(void);
void bsp_bt_pwrkey5s_clr(void);
void bsp_bt_hid_tog_conn(void);
void bsp_bt_hid_screen_left(void);
void bsp_bt_hid_screen_right(void);
void bt_name_suffix_set(void);
uint bsp_bt_get_hfp_vol(uint hfp_vol);
u16 bsp_bt_chkclr_warning(u16 bits);
void bsp_bt_switch_voice_lang(void);
void bsp_bt_warning(void);
void bsp_bt_status(void);
uint bsp_bt_disp_status(void);
void bt_spp_cmd_process(u8 *ptr, u16 size, u8 type);

void bt_music_rec_init(void);
bool bt_rec_status_process(void);
void bt_music_rec_continue(void);
void bt_sco_rec_init(void);

void bt_redial_init(void);
void bt_redial_reset(uint8_t index);
void bt_update_redial_number(uint8_t index, char *buf, u32 len);
const char *bt_get_last_call_number(uint8_t index);

/**
 * @brief 蓝牙调整手机通话音量级数
 * up_flag: 0:减一级; 1:加一级;
*/
void bt_call_volume_change(u8 up_flag);

/**
 * @brief 蓝牙设置手机音量加一级
*/
void bt_volume_up(void);

/**
 * @brief 蓝牙设置手机音量减一级
*/
void bt_volume_down(void);

void bt_set_music_sta(u8 status);

//bt hid
void user_finger_down(void);                    //向下滑
void user_finger_up(void);                      //向上滑
void user_finger_p(void);                       //单击
void user_finger_pp(void);                      //双击
void bt_hid_point_user(img_point_type type);    //视频APP，翻页等功能, img_point_type 选择动作

//call
void hfp_hf_call_notice(uint32_t evt);
void bsp_bt_call_enter(void);                   //进入呼出/通话页面调用
void bsp_bt_call_exit(void);                    //退出呼出/通话页面调用

//call ring
void bsp_bt_ring_enter(void);                   //进入来电响铃页面调用
void bsp_bt_ring_exit(void);                    //退出来电响铃页面调用
void bsp_bt_ring_process(void);                 //来电响铃页面进程调用

void bsp_bt_call_times_inc(void);

#if BT_ID3_TAG_EN
//id3
void bt_id3_tag_update_cb_reg(void *func_cb);
void bt_id3_tag_update_cb_unreg(void);
#else
#define bt_id3_tag_update_cb_reg(x)
#define bt_id3_tag_update_cb_unreg()
#endif // BT_ID3_TAG_EN

void bsp_bt_trun_off(void);
void bsp_bt_trun_on(void);

#endif //_BSP_BT_H
