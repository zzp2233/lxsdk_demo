#ifndef __BT_FOTA_H
#define __BT_FOTA_H
typedef enum
{
    INFO_DEV_VER  = 1,
    INFO_UPDATE_REQ,
    INFO_DEV_FEATURE,
    INFO_DEV_CONNECT_STA,
    INFO_EDR_ADDR,
    INFO_DEV_CHANNEL,
} DEV_INFO_E;

typedef struct __attribute__((packed))
{
    u16 remote_ver;
    u32 total_len;
    u32 remain_len;
    u32 hash;
    u16 data_pos;
    u8 fot_recv_ok;
    u32 addr;
    u32 tick;
    u8 type;
    u8 sys_clk;
    u32 ota_recv_size;      //ota接收大小
    u32 ui_res_len;         //UI资源大小
    u32 ui_updata_address;  //更新时使用的地址
    u32 ui_recv_remain;     //256对齐
    u8  is_ui_flag;         //是否是UI数据，1是，0不是
    u32 code_res_len;       //代码区间大小
}
fot_s;

typedef enum
{
    FOT_FILE_TYPE_UI = 1,
    FOT_FILE_TYPE_FOT,
} FOT_FILE_TYPE;

typedef struct
{
    FOT_FILE_TYPE  type;        //文件类型
    u8  percent;                //文件进度百分比
    u32 files_all_size;         //文件长度
    u32 files_recv_size;        //文件接收长度
} fot_progress_t;


void bsp_fot_init(void);
void fot_recv_proc(u8 *buf, u16 len);
void bsp_fot_process(void);
void fot_update_pause(void);
void fot_update_continue(void);
void fot_tws_connect_callback(void);
void fot_tws_disconnect_callback(void);
u8 is_fot_start(void);
void bsp_fot_exit(void);
bool bsp_fot_is_connect(void);
u8 fot_app_connect_auth(uint8_t *packet, uint16_t size);
void fot_spp_connect_callback(void);
void fot_spp_disconnect_callback(void);
void fot_ble_disconnect_callback(void);
void fot_ble_connect_callback(void);

#endif // __BT_FOTA_H
