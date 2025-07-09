#include "include.h"
#include "bt_fota.h"
#include "ute_module_call.h"
#include "ute_application_common.h"

#if LE_AB_FOT_EN

#define FOT_DEBUG_EN                    0

#if FOT_DEBUG_EN
#define FOT_DEBUG(...)                  printf(__VA_ARGS__)
#define FOT_DEBUG_R(...)                print_r(__VA_ARGS__)
#else
#define FOT_DEBUG(...)
#define FOT_DEBUG_R(...)
#endif

/**********************************************/
#define FOT_STA_INIT            BIT(0)
#define FOT_STA_START           BIT(1)
#define FOT_STA_PAUSE           BIT(2)

/**********************************************/
#define FOT_FLAG_UPDATE_OK      BIT(0)
#define FOT_FLAG_SYS_RESET      BIT(1)
#define FOT_FLAG_ROLE_SWITCH    BIT(2)
#define FOT_FLAG_APP_CONNECT    BIT(3)
#define FOT_FLAG_CLK_SET        BIT(4)
#define FOT_FLAG_UPDATE_EXIT    BIT(5)

/**********************************************/
#define FOT_NOTIFY_STA          0x90
#define FOT_GET_INFO            0x91
#define FOT_GET_INFO_TLV        0x92
#define FOT_OUT_DATA_START      0xA0
#define FOT_OUT_DATA_CONTINUE   0x20

/**********************************************/
#define FOT_CMD_POS             0
#define FOT_SEQ_POS             1
#define FOT_ADDR_POS            2
#define FOT_DATA_LEN_POS        6
#define DATA_START_POS          10
#define DATA_CONTINUE_POS       2

/**********************************************/
#define FOT_BLOCK_LEN           8192        //压缩升级必须是512的倍数，目前只做了512倍数的处理


extern bool ble_fot_send_packet(u8 *buf, u16 len);
extern u16 get_spp_mtu_size(void);
extern uint8_t sys_clk_get_cur(void);
u16 dev_version = 0x0001; //设备版本号

AT(.com_text.const)
static const u8 fot_auth_data[] = {0xCC, 0xAA, 0x55, 0xEE, 0x12, 0x19, 0xE4};

fot_s fot_var;

static fot_progress_t fot_progress;

static u8 fot_data[FOT_BLOCK_LEN] AT(.fot_data.buf);

static u8 fot_seq = 0;
static u8 fot_remote_seq = 0;
static u8 fot_sta = 0;
static u8 fot_flag = 0;
static u16 fot_connect_sta = 0;

fot_progress_t *bsp_fot_progress_get(void);

void *fot_wait_bt_off_timer = NULL;
static uint8_t *reply_info_tmp_buff = NULL;
static uint8_t reply_info_len = 0;

u8 is_fot_update_en(void)
{
    return 1;
}

void app_fota_pack_write(void *buf, u32 len)
{
    u32 total_use_len = 0;

    while (len && (ota_pack_get_err() == FOT_ERR_OK))
    {
        ota_pack_write(buf + total_use_len);
        len -= 512;
        total_use_len += 512;
    }
}

AT(.text.fot.cache)
void app_fota_write(void *buf, u32 addr, u32 len)
{
    app_fota_pack_write(buf, len);
}

AT(.text.fot.cache)
u8 app_fota_get_err(void)
{
    return ota_pack_get_err();
}

AT(.text.fot.cache)
void app_fota_verify(void)
{
    ota_pack_verify();
}

bool app_fota_breakpoint_info_read(void)
{
    return ota_pack_breakpoint_info_read();
}

void app_fota_init(void)
{
    ota_pack_init();
    load_code_fota();
}

u32 app_fota_get_curaddr(void)
{
    return ota_pack_get_curaddr();
}

void app_fota_update_done(void)
{
    ota_pack_done();
}

bool app_fota_is_write_done(void)
{
    return ota_pack_is_write_done();
}

AT(.text.fot.cache)
static void fot_sent_proc(u8 *buf,u8 len)
{
    if((fot_flag & FOT_FLAG_APP_CONNECT) == 0)
    {
        return;
    }

    FOT_DEBUG("fot tx:");
    FOT_DEBUG_R(buf,len);

#if LE_AB_FOT_EN
    if(ble_is_connect())
    {
        ble_fot_send_packet(buf,len);
    }
#endif
}

AT(.text.fot.update)
static void fot_reply_info_tlv(u8 *buf,u8 len)
{
    u8 read_offset = 0;
    u8 write_offset = 0;
    u8 rsp[32];
    u8 val_len = 0;

    if((buf == NULL) || (len == 0))
    {
        return;
    }

    rsp[write_offset++] = FOT_GET_INFO_TLV;
    rsp[write_offset++] = fot_seq++;

    while(read_offset < len)
    {
        switch(buf[read_offset])
        {
            case INFO_DEV_VER:
                FOT_DEBUG("INFO_DEV_VER\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_DEV_VER;
                rsp[write_offset++] = 2;
                rsp[write_offset++] = dev_version & 0xff;
                rsp[write_offset++] = (dev_version >> 8) & 0xff;
                break;

            case INFO_DEV_FEATURE:
                FOT_DEBUG("INFO_DEV_FEATURE\n");
                {
                    u16 dev_ability = 0;
                    val_len = buf[read_offset + 1];
                    rsp[write_offset++] = INFO_DEV_FEATURE;
                    rsp[write_offset++] = 2;
                    rsp[write_offset++] = dev_ability & 0xff;
                    rsp[write_offset++] = (dev_ability >> 8) & 0xff;
                }
                break;

            case INFO_DEV_CONNECT_STA:
                FOT_DEBUG("INFO_DEV_CONNECT_STA\n");
                val_len = buf[read_offset + 1];
                rsp[write_offset++] = INFO_DEV_CONNECT_STA;
                rsp[write_offset++] = 2;
                rsp[write_offset++] = fot_connect_sta & 0xff;
                rsp[write_offset++] = (fot_connect_sta >> 8) & 0xff;
                break;

            default:
                val_len = buf[read_offset + 1];
                break;
        }
        read_offset += (2 + val_len);
    }

    if(write_offset > sizeof(rsp))
    {
        FOT_DEBUG("fot:rsp buf overflow!!!\n");
        while(1);
    }

    fot_sent_proc(rsp,write_offset);
}


AT(.text.fot.update)
static void fot_reply_dev_version(void)
{
    u8 data[5];

    data[0] = FOT_GET_INFO;
    data[1] = fot_seq++;
    data[2] = INFO_DEV_VER;
    memcpy(&data[3],&dev_version,2);
    fot_sent_proc(data,5);
}

AT(.text.fot.cache)
static void fot_dev_notify_sta(u8 sta)
{
    u8 buf[3];

    buf[0] = FOT_NOTIFY_STA;
    buf[1] = fot_seq++;
    buf[2] = sta;

    fot_sent_proc(buf,3);
}

AT(.text.fot.cache)
static void fot_dev_notify_seq_err(u8 sta)
{
    u8 buf[3];

    buf[0] = FOT_NOTIFY_STA;
    buf[1] = fot_seq;
    buf[2] = sta;

    fot_sent_proc(buf,3);
}

AT(.text.fot.update)
u8 is_fot_start(void)
{
    return (fot_sta & FOT_STA_START) ? 1:0;
}

AT(.text.fot.update)
void bsp_fot_init(void)
{
#if (ASR_SELECT && ASR_FULL_SCENE)
    bsp_asr_stop();
#endif

    u8  dev_version_str[] = SW_VERSION;
    u16 version_temp = 0;

    memset(&fot_var,0,sizeof(fot_var));
    memset(&fot_progress, 0, sizeof(fot_progress));

    dev_version = 0x00;
    version_temp = dev_version_str[1]-'0';
    version_temp <<= 12;
    dev_version |= version_temp;

    version_temp = dev_version_str[3]-'0';
    version_temp <<= 8;
    dev_version |= version_temp;

    version_temp = dev_version_str[5]-'0';
    version_temp <<= 4;
    dev_version |= version_temp;

    fot_sta = FOT_STA_INIT;
}

void bsp_fot_exit(void)
{
    fot_sta = 0;
    fot_seq = 0;
    fot_remote_seq = 0;
    fot_flag &= ~FOT_FLAG_APP_CONNECT;

    if(fot_var.sys_clk)
    {
        sys_clk_set(fot_var.sys_clk);
    }

    unlock_code_fota();


    if (func_cb.sta == FUNC_OTA_MODE)
    {
        task_stack_init();
//        func_cb.sta = FUNC_CLOCK;           //OTA退出返回表盘
        func_cb.sta = FUNC_OTA_ERROR;         //OTA升级失败界面
    }
    else if (func_cb.sta == FUNC_OTA_UI_MODE)
    {
        task_stack_init();
        func_cb.sta = FUNC_BT_UPDATE;       //UI OTA进入错误界面
    }
}

void fot_update_pause(void)
{
    if(fot_sta & FOT_STA_START)
    {
        fot_sta = FOT_STA_PAUSE;
        fot_dev_notify_sta(FOT_UPDATE_PAUSE);
    }
}

void fot_update_continue(void)
{
    if(fot_sta & FOT_STA_PAUSE)
    {
        bsp_fot_init();
        fot_dev_notify_sta(FOT_UPDATE_CONTINUE);
    }
}

u16 bsp_fot_mtu_get(void)
{
    u16 packet_len = 0;
#if LE_AB_FOT_EN
    if(ble_is_connect())
    {
        packet_len = ble_get_gatt_mtu();
    }
#endif
    return packet_len;
}

AT(.text.fot.update)
static void fot_reply_update_request(void)
{
    u16 flash_remote_ver;
    u32 hash;
    u32 addr = 0;
    u32 block_len = FOT_BLOCK_LEN;
    u16 packet_len = 0;
    u8 data[14];
    u8 need_update = 1;

    if(bt_get_status() >= BT_STA_INCOMING)
    {
        need_update = 0;
        fot_flag |= FOT_FLAG_UPDATE_EXIT;
        goto fot_req_reply;
    }

#if LE_AB_FOT_EN
    if(ble_is_connect())
    {
        ble_update_conn_param(12,0,400);
    }
#endif

    packet_len = bsp_fot_mtu_get();

    FOT_DEBUG("fot_packet_len:%d\n",packet_len);

    app_fota_init();

    fot_flag |= FOT_FLAG_CLK_SET;
    fot_sta |= FOT_STA_START;

    FOT_DEBUG("hash_val:0x%x\n",fot_var.hash);

    param_fot_remote_ver_read((u8*)&flash_remote_ver);
    param_fot_hash_read((u8*)&hash);

    FOT_DEBUG("flash hash val:0x%x\n",hash);/*  */

    // if((fot_var.hash != 0xFFFFFFFF) && (flash_remote_ver == fot_var.remote_ver) && (hash == fot_var.hash))
    // {
    //     param_fot_type_read(&fot_var.type);
    //     if(app_fota_breakpoint_info_read() == true)
    //     {
    //         addr = app_fota_get_curaddr();
    //     }
    //     printf("===================>func_cb.sta = FUNC_PWROFF;\n");
    //     func_cb.sta = FUNC_PWROFF;
    //     return;
    // }
    if((fot_var.hash != 0xFFFFFFFF) && (flash_remote_ver == fot_var.remote_ver) && (hash == fot_var.hash))
    {
        // param_fot_type_read(&fot_var.type);
        // if(app_fota_breakpoint_info_read() == true)
        // {
        //     addr = app_fota_get_curaddr();
        // }
        extern bool uteApplicationCommonIsAppClosed(void);
        if(uteApplicationCommonIsAppClosed()) //如果是ute app连接，则不关机
        {
            printf("===================>func_cb.sta = FUNC_PWROFF\n");
            func_cb.sta = FUNC_PWROFF;
        }
    }


fot_req_reply:
    data[0] = FOT_GET_INFO;
    data[1] = fot_seq++;
    data[2] = INFO_UPDATE_REQ;
    memcpy(&data[3],&addr,4);
    memcpy(&data[7],&block_len,4);
    memcpy(&data[11],&packet_len,2);
    data[13] = need_update;
    fot_sent_proc(data,14);
}

void ble_disconnect_callback(void)
{
#if LE_AB_FOT_EN

    fot_ble_disconnect_callback();

#endif
}

#if LE_AB_FOT_EN
void fot_ble_disconnect_callback(void)
{
    FOT_DEBUG("--->fot_ble_disconnect_callback\n");

    bsp_fot_exit();

}

void fot_ble_connect_callback(void)
{
    FOT_DEBUG("--->fot_ble_connect_callback\n");
}
#endif

void fot_spp_connect_callback(void)
{
    FOT_DEBUG("--->fot_spp_connect_callback\n");
}

void fot_spp_disconnect_callback(void)
{
    FOT_DEBUG("--->fot_spp_disconnect_callback\n");

    bsp_fot_exit();

}

AT(.text.fot.cache)
void fot_wait_bt_off_timer_callback(void * pxTimer)
{
    if(bt_get_status() > BT_STA_SCANNING)
    {
        FOT_DEBUG("fot:bt on, wait...\n");
        uteModulePlatformRestartTimer(&fot_wait_bt_off_timer, 100);
    }
    else
    {
        FOT_DEBUG("fot:bt off, start...\n");
        fot_reply_info_tlv(&reply_info_tmp_buff[0],reply_info_len);

        uteModulePlatformMemoryFree(reply_info_tmp_buff);
        reply_info_tmp_buff = NULL;

        uteModulePlatformStopTimer(&fot_wait_bt_off_timer);
        uteModulePlatformDeleteTimer(&fot_wait_bt_off_timer);
        fot_wait_bt_off_timer = NULL;
    }
}

AT(.text.fot.cache)
void fot_recv_proc(u8 *buf, u16 len)
{
    u32 addr;
    u32 recv_data_len;
    u8 cmd;

    FOT_DEBUG("====>fot_recv_proc,len=%d\n",len);
    FOT_DEBUG_R(buf,len);
    FOT_DEBUG("fot_remote_seq:%d, buf[FOT_SEQ_POS]:%d\n",fot_remote_seq,buf[FOT_SEQ_POS]);

    if(fot_remote_seq != buf[FOT_SEQ_POS])
    {
        if(!memcmp(fot_auth_data,buf,7))
        {
            return;
        }
        FOT_DEBUG("remote seq err:%d,%d\n",fot_remote_seq,buf[FOT_SEQ_POS]);
        fot_dev_notify_sta(FOT_ERR_SEQ);
        fot_flag |= FOT_FLAG_UPDATE_EXIT;
        return;
    }

    // if(fot_remote_seq != buf[FOT_SEQ_POS]){
    //     if(memcmp(fot_auth_data, buf, 7)){      //接入码先过掉
    //         FOT_DEBUG("remote seq err:%d,%d\n",fot_remote_seq,buf[FOT_SEQ_POS]);
    //         fot_dev_notify_sta(FOT_ERR_SEQ);
    //         fot_flag |= FOT_FLAG_UPDATE_EXIT;
    //     }
    //     return;
    // }

    fot_remote_seq++;

    if((fot_sta & FOT_STA_INIT) == 0)
    {
        return;
    }

    cmd = buf[FOT_CMD_POS];

    switch(cmd)
    {
        case FOT_GET_INFO_TLV:
            // fot_reply_info_tlv(&buf[2],len-2);

            if (uteModuleCallBtIsPowerOn())
            {
                uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_AUTO); // 音频解码和OTA复用内存
            }

            // 等待BT关闭
            if (reply_info_tmp_buff == NULL)
            {
                reply_info_tmp_buff = (uint8_t *)uteModulePlatformMemoryAlloc(len);
            }

            if (reply_info_tmp_buff)
            {
                memset(reply_info_tmp_buff, 0, len);
                reply_info_len = len - 2;
                memcpy(reply_info_tmp_buff, &buf[2], reply_info_len);
            }
            else
            {
                FOT_DEBUG("fot_recv_proc: reply_info_tmp_buff malloc fail\n");
                break;
            }

            if (fot_wait_bt_off_timer == NULL)
            {
                bool ret = uteModulePlatformCreateTimer(&fot_wait_bt_off_timer, "fot_wait_bt_off_timer", 1, 100, false, fot_wait_bt_off_timer_callback);
                if (!ret)
                {
                    uteModulePlatformMemoryFree(reply_info_tmp_buff);
                    reply_info_tmp_buff = NULL;
                    FOT_DEBUG("fot_wait_bt_off_timer create fail\n");
                    break;
                }
            }
            uteModulePlatformRestartTimer(&fot_wait_bt_off_timer, 100);

#if UTE_USER_ID_FOR_BINDING_SUPPORT
            uteModuleAppBindingSetOurAppConnection(true);
#endif

            break;

        case FOT_GET_INFO:
            if(buf[2] == INFO_DEV_VER)
            {
                fot_reply_dev_version();
            }
            else if(buf[2] == INFO_UPDATE_REQ)
            {
                memcpy(&fot_var.remote_ver,&buf[3],2);
                memcpy(&fot_var.hash,&buf[5],4);
                fot_reply_update_request();
            }
            break;

        case FOT_OUT_DATA_START:
            if(fot_var.remain_len)
            {
                FOT_DEBUG("--->len err:%d\n",fot_var.remain_len);
                fot_dev_notify_sta(FOT_ERR_DATA_LEN);
                fot_flag |= FOT_FLAG_UPDATE_EXIT;
                return;
            }
            recv_data_len = len-DATA_START_POS;
            memcpy(&fot_var.total_len,&buf[FOT_DATA_LEN_POS],4);
            fot_var.remain_len = fot_var.total_len - recv_data_len;
            memcpy(&addr,&buf[FOT_ADDR_POS],4);
            memcpy(&fot_data[fot_var.data_pos],&buf[DATA_START_POS],recv_data_len);
            fot_var.data_pos += recv_data_len;
            if(fot_var.remain_len == 0)
            {
                fot_var.fot_recv_ok = 1;
                fot_var.data_pos = 0;
            }
            break;

        case FOT_OUT_DATA_CONTINUE:
            recv_data_len = len-DATA_CONTINUE_POS;
            if(fot_var.remain_len < recv_data_len)
            {
                recv_data_len =  fot_var.remain_len;
            }
            fot_var.remain_len -= recv_data_len;
            memcpy(&fot_data[fot_var.data_pos],&buf[DATA_CONTINUE_POS],recv_data_len);
            fot_var.data_pos += recv_data_len;
            if(fot_var.remain_len == 0)
            {
                fot_var.fot_recv_ok = 1;
                fot_var.data_pos = 0;
            }
            break;
    }
}

#if FOTA_UI_EN
static const u8 ui_ota_res_sign[8] = {0x44, 0x55, 0x46, 0x2D, 0x55, 0x49, 0x41, 0x42};
static u8 fot_data_buff[512] AT(.fot_data.buf);

//256对写入
AT(.text.fot.cache)
void bsp_fot_ui_write_page(void *buf)
{
    if((fot_var.ui_updata_address & 0xFFF) == 0)
    {
        os_spiflash_erase(fot_var.ui_updata_address);
    }
    os_spiflash_program(buf, fot_var.ui_updata_address, 256);
    fot_var.ui_updata_address += 256;
}

AT(.text.fot.cache)
void bsp_fot_ui_updata_do(u8 *buf, u32 len)
{
    uart_putchar('*');
    if(fot_var.ui_recv_remain)
    {
        if((len + fot_var.ui_recv_remain) >= 256)
        {
            memcpy(&fot_data_buff[fot_var.ui_recv_remain], buf, 256-fot_var.ui_recv_remain);
            bsp_fot_ui_write_page(fot_data_buff);
            len -= 256-fot_var.ui_recv_remain;
            buf += 256-fot_var.ui_recv_remain;
            fot_var.ui_recv_remain = 0;
        }
        else
        {
            memcpy(&fot_data_buff[fot_var.ui_recv_remain], buf, len);
            fot_var.ui_recv_remain += len;
            len = 0;
            return;
        }
    }

    while(len >= 256)
    {
        bsp_fot_ui_write_page(buf);
        buf += 256;
        len -= 256;
    }

    if(len)
    {
        memcpy(fot_data_buff, buf, len);
        fot_var.ui_recv_remain = len;
    }
}

AT(.text.fot.cache)
void bsp_fot_code_updata_do(u8 *buf, u32 len)
{
    uart_putchar('.');
    if(fot_var.ui_recv_remain)
    {
        if((len + fot_var.ui_recv_remain) >= 512)
        {
            memcpy(&fot_data_buff[fot_var.ui_recv_remain], buf, 512-fot_var.ui_recv_remain);
            app_fota_write(fot_data_buff, app_fota_get_curaddr(), 512);
            len -= 512-fot_var.ui_recv_remain;
            buf += 512-fot_var.ui_recv_remain;
            fot_var.ui_recv_remain = 0;
        }
        else
        {
            memcpy(&fot_data_buff[fot_var.ui_recv_remain], buf, len);
            fot_var.ui_recv_remain += len;
            len = 0;
            return;
        }
    }

    while(len >= 512)
    {
        app_fota_write(buf, app_fota_get_curaddr(), 512);
        buf += 512;
        len -= 512;
    }

    if(len)
    {
        memcpy(fot_data_buff, buf, len);
        fot_var.ui_recv_remain = len;
    }
}
#endif

AT(.text.fot.cache)
void bsp_fot_process(void)
{
    if(fot_sta & FOT_STA_START)
    {
        if(fot_var.fot_recv_ok)
        {
            fot_var.fot_recv_ok = 0;
#if FOTA_UI_EN
            u8 *buff_ptr = fot_data;

            if(fot_var.ota_recv_size == 0)
            {
                //print_r(fot_data, fot_var.total_len);
                fot_var.ui_updata_address = FLASH_UI_BASE;
                fot_var.ui_recv_remain    = 0;
                if(memcmp(ui_ota_res_sign, fot_data, 8) == 0)
                {
                    u32 temp = 0;
                    u8 i = 0;
                    for(i=8; i<23; i++)
                    {
                        if(fot_data[i] == 0x0D)
                        {
                            break;
                        }
                        else
                        {
                            temp = temp *10 + fot_data[i]-'0';
                        }
                    }
                    fot_var.ui_res_len = temp;
                    fot_var.is_ui_flag = 1;
                    i += 2;

                    //增加代码大小 批处理需要新的
                    if((fot_data[i] != 'F') || (fot_data[i+1] != 'O') || (fot_data[i+2] != 'N'))
                    {
                        temp = 0;
                        for(; i<32; i++)
                        {
                            if(fot_data[i] == 0x0D)
                            {
                                break;
                            }
                            else
                            {
                                temp = temp *10 + fot_data[i]-'0';
                            }
                        }
                        fot_var.code_res_len = temp;
                        i += 2;
                    }
                    fot_var.total_len -= i;
                    buff_ptr += i;

                    func_cb.sta = FUNC_OTA_UI_MODE;         //进入UI OTA模式，避免UI刷死机
                    printf("UI SIZE:%d CODE SIZE:%d %d\n", fot_var.ui_res_len, fot_var.code_res_len, i+2);
                }
            }

            if(fot_var.is_ui_flag)
            {
                if((fot_var.ota_recv_size + fot_var.total_len) <= fot_var.ui_res_len)
                {
                    bsp_fot_ui_updata_do(buff_ptr, fot_var.total_len);
                }
                else
                {
                    if(fot_var.ota_recv_size < fot_var.ui_res_len)
                    {
                        bsp_fot_ui_updata_do(buff_ptr, fot_var.ui_res_len-fot_var.ota_recv_size);
                        if(fot_var.ui_recv_remain)
                        {
                            bsp_fot_ui_write_page(fot_data_buff);
                            fot_var.ui_recv_remain = 0;
                        }
                        bsp_fot_code_updata_do(fot_data+(fot_var.ui_res_len-fot_var.ota_recv_size), fot_var.total_len - (fot_var.ui_res_len-fot_var.ota_recv_size));
                    }
                    else
                    {
                        bsp_fot_code_updata_do(fot_data, fot_var.total_len);
                    }
                }
            }
            else
            {
                if (func_cb.sta != FUNC_OTA_UI_MODE && func_cb.sta != FUNC_OTA_MODE)
                {
                    func_cb.sta = FUNC_OTA_MODE;         //进入OTA升级界面
                }
                app_fota_write(fot_data, app_fota_get_curaddr(), fot_var.total_len);
            }
            fot_var.ota_recv_size += fot_var.total_len;
#else
            func_cb.sta = FUNC_OTA_MODE;         //进入OTA升级界面
            app_fota_write(fot_data, app_fota_get_curaddr(), fot_var.total_len);
#endif
            bsp_fot_progress_get();
            if(app_fota_is_write_done())
            {
                app_fota_verify();
                if(app_fota_get_err())
                {
                    fot_dev_notify_sta(app_fota_get_err());
                    fot_flag |= FOT_FLAG_UPDATE_EXIT;
                }
                else
                {
                    FOT_DEBUG("--->fot update success\n");
                    app_fota_update_done();
                    fot_flag |= (FOT_FLAG_UPDATE_OK | FOT_FLAG_SYS_RESET);
                    fot_var.tick = tick_get();
                    fot_dev_notify_sta(FOT_UPDATE_DONE);
                }
            }
            else
            {
                fot_dev_notify_sta(app_fota_get_err());
                if(app_fota_get_err())
                {
                    printf("err:0x%x\n", app_fota_get_err());
                    fot_flag |= FOT_FLAG_UPDATE_EXIT;
                }
            }
        }
    }

    if(fot_flag & FOT_FLAG_SYS_RESET)
    {
        if(tick_check_expire(fot_var.tick,3000))
        {
            extern void param_fot_hash_write(u8 *param);
            extern void param_fot_remote_ver_write(u8 *param);
            param_fot_hash_write((u8*)&fot_var.hash);
            param_fot_remote_ver_write((u8*)&fot_var.remote_ver);
            param_sync();

            fot_flag &= ~FOT_FLAG_SYS_RESET;
            FOT_DEBUG("-->fota update ok,sys reset\n");
            ble_disconnect();
            ble_adv_dis();
            if (func_cb.sta == FUNC_OTA_UI_MODE)
            {
                // WDT_RST();
                uteApplicationCommonRestart();
            }
            else
            {
                func_cb.sta = FUNC_OTA_SUCC;
            }
        }
    }

    if(fot_flag & FOT_FLAG_CLK_SET)
    {
        FOT_DEBUG("--->FOT_FLAG_CLK_SET\n");
        fot_var.sys_clk = sys_clk_get_cur();
        sys_clk_set(SYS_132M);
        fot_flag &= ~FOT_FLAG_CLK_SET;
    }

    if(fot_flag & FOT_FLAG_UPDATE_EXIT)
    {
        FOT_DEBUG("-->FOT_FLAG_UPDATE_EXIT");
        bsp_fot_exit();
        fot_flag &= ~FOT_FLAG_UPDATE_EXIT;
    }
}


AT(.com_text.fot)
u8 fot_app_connect_auth(uint8_t *packet, uint16_t size)
{
    if(!fot_sta)
    {
        if(size == 7 && !memcmp(fot_auth_data, packet, 7))      //sizeof(fot_auth_data)
        {
            fot_flag |= FOT_FLAG_APP_CONNECT;
            bsp_fot_init();
        }
    }

    return fot_sta;
}

AT(.com_text.fot)
bool bsp_fot_is_connect(void)
{
    return (fot_sta>0);
}

AT(.text.fot.cache)
fot_progress_t *bsp_fot_progress_get(void)
{
    fot_progress_t *p = &fot_progress;

    if (fot_var.is_ui_flag)
    {
        if (fot_var.ota_recv_size < fot_var.ui_res_len)
        {
            p->type = FOT_FILE_TYPE_UI;
            p->files_all_size = fot_var.ui_res_len;
            p->files_recv_size = fot_var.ota_recv_size;
        }
        else
        {
            if (ota_pack_get_total_size() == 0 && ota_pack_get_curaddr() == 0)
            {
                p->type = FOT_FILE_TYPE_UI;
                p->files_all_size = fot_var.ui_res_len;
                p->files_recv_size = fot_var.ui_res_len;
            }
            else
            {
                p->type = FOT_FILE_TYPE_FOT;
                p->files_all_size = ota_pack_get_total_size();
                p->files_recv_size = ota_pack_get_curaddr();
            }
        }
    }
    else
    {
        p->type = FOT_FILE_TYPE_FOT;
        p->files_all_size = ota_pack_get_total_size();
        p->files_recv_size = ota_pack_get_curaddr();
    }

    if (p->files_all_size)
    {
        p->percent = p->files_recv_size * 100 / p->files_all_size;
    }

    return p;
}

#else
WEAK void fot_update_pause(void)
{

}

WEAK void fot_update_continue(void)
{

}

#endif  //FOT_EN
