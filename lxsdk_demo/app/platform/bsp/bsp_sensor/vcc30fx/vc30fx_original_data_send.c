/**********************************************************************************************
 * @Copyright(c) 2024, Chengdu vcaresensor Microelectronics Corporation. All rights reserved.
 * @Author: Tse
 * @Date: 2024-02-04 14:54:19
 * @LastEditTime: 2024-03-29 15:53:05
 * @LastEditors: Tse
 * @Description:
 **********************************************************************************************/
/* system include */
//#include "os_timer.h"
//#include <simple_ble_service.h>
#include "ute_module_profile_ble.h"
#include "ute_project_config.h"
//#include "wristband_global_data.h"
//extern RtkWristBandSysType_t RtkWristbandSys;
/* vcare include */
#include "vcare_device_com.h"
#include "vc30fx_sample_result.h"
#include "ute_application_common.h"

#if DUG_VCXX_HEART_SUPPORT
static unsigned char ble_buf[2000] = {0};
static unsigned short int buf_size = 0;
static unsigned short int buf_index = 0;
#define BLE_PACK_SIZE 200
#define TIMER_BLE_GAP 10


void uteModuleDugStartSendHrData(void);

//static void debug_orginal_data_send(unsigned short int size);
/*********************************************************************************************************
 * 调试蓝牙数据格式：
 * [0~7]slot0_8bytes
 * {
 *  .[0~2]ppg_data(h,m,l)
 *  .[3]ppg_adcbit=(bit-16)/2
 *  .[4]resistance
 *  .[5]current
 *  .[6~7]env_bit14(h,l)
 * }
 * [ 8~15]slot1_8bytes{...}
 * [16~23]slot2_8bytes{...}
 * [24~25]bio0_2bytes(h,l)
 * [26~27]bio1_2bytes(h,l)
 * [28~29]ps_slot3_2bytes(h,l)
 * [30~35]gsensorx/y/z(h,l) or tempRefADC, tempExtADC, temperature
 * [36]hr
 * [37]spo2
 * [38]algo_mode
 * [39]wear_bioret(bit7) | algoret(bit6) | acceret(bit5) | wear_drv(bit3~bit0)
 *
 * #define FRAME_SIZE 40
 *********************************************************************************************************/
#define ORIGINAL_DATA_DEBUG_ENABLE (1)
#if ORIGINAL_DATA_DEBUG_ENABLE

#define HR_PPG_SLOT 0
#define IR_PPG_SLOT 1
#define RD_PPG_SLOT 2
#define PS_PPG_SLOT 3
/* 单采样帧字节数 */
#define FRAME_SIZE 40

extern void vc30fx_drv_get_wear_detail_result( unsigned char *bio_ret, unsigned char *algo_ret, unsigned char *acce_ret );

void vc30fx_send_orginal_data(vcare_ppg_device_t *pdev, gsensor_axes *pgsensor, unsigned char ppgsize)
{
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char slot_k = 0;
    unsigned char psfreq_index = 0;
    /* wear all result param */
    unsigned char bio_result=0, algo_result=0, acce_result=0;
    vc30fx_sample_info_t *presult = (vc30fx_sample_info_t *)pdev->result;
    memset(ble_buf, 0, sizeof(ble_buf));
    if( ppgsize>=64 )
    {
        ppgsize=64;
    }
    if( ppgsize>=sizeof(ble_buf)/FRAME_SIZE )
    {
        ppgsize=sizeof(ble_buf)/FRAME_SIZE;
    }
    vc30fx_drv_get_wear_detail_result( &bio_result, &algo_result, &acce_result );
    for (i = 0; i < ppgsize; i++)
    {
        j = 0;
        /* 0-23,slot_ppg：3*8=24 */
        for (slot_k = 0; slot_k < 3; slot_k++)
        {
            if (slot_k == 2 && presult->slot_result[slot_k].slot_enwork==2)
            {
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
            }
            else
            {
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].ppg_buffer[i] >> 16) & 0xff;
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].ppg_buffer[i] >> 8) & 0xff;
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].ppg_buffer[i] >> 0) & 0xff;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = presult->slot_result[slot_k].slot_resistance;
                ble_buf[i * FRAME_SIZE + j++] = presult->slot_result[slot_k].slot_current;
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].env_data >> 8) & 0xff;
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].env_data >> 0) & 0xff;
            }
        }
        /*24-29 bio:2*2 + ps：2 */
        psfreq_index = i / presult->ps_div;
        if (presult->extra_result.biodata_storefifo)
        {
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioinn_buffer[psfreq_index] >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioinn_buffer[psfreq_index] >> 0) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioext_buffer[psfreq_index] >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioext_buffer[psfreq_index] >> 0) & 0xff;
        }
        else
        {
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioinn_data >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioinn_data >> 0) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioext_data >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioext_data >> 0) & 0xff;
        }
        if (presult->slot_result[2].slot_storefifo && presult->slot_result[2].slot_enwork==2 )
        {
            ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[2].ppg_buffer[psfreq_index] >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[2].ppg_buffer[psfreq_index] >> 0) & 0xff;
        }
        else
        {
            ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[2].u.ps_data >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[2].u.ps_data >> 0) & 0xff;
        }
        /* 30-35 xyz：2*3=6*/
#if 1
#if UTE_DRV_HEART_VCXX_ARM_XY_EXCHANGE||SC7A20H_ROLLOVER_HAND_SCREEN_XY_EXCHANGE_SUPPORT
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->yData[i] >> 8;//X高位
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->yData[i] & 0xff;//X低
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->xData[i] >> 8;//Y高位
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->xData[i] & 0xff;//Y低位
#else
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->xData[i] >> 8;//X高位
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->xData[i] & 0xff;//X低
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->yData[i] >> 8;//Y高位
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->yData[i] & 0xff;//Y低位
#endif
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->zData[i] >> 8;//Z高位
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->zData[i] & 0xff;//Z低位
#else  /* temp result */
        ble_buf[i * FRAME_SIZE + j++] = presult->extra_result.tempref_data >> 8;
        ble_buf[i * FRAME_SIZE + j++] = presult->extra_result.tempref_data & 0xff;
        ble_buf[i * FRAME_SIZE + j++] = presult->extra_result.tempext_data >> 8;
        ble_buf[i * FRAME_SIZE + j++] = presult->extra_result.tempext_data & 0xff;
        ble_buf[i * FRAME_SIZE + j++] = pdev->temperature >> 8;
        ble_buf[i * FRAME_SIZE + j++] = pdev->temperature & 0xff;
#endif
        /* 36-spo2,37-hr,38-algo_mode */
        ble_buf[i * FRAME_SIZE + j++] = pdev->heart_rate;
        ble_buf[i * FRAME_SIZE + j++] = pdev->spo2;
        ble_buf[i * FRAME_SIZE + j++] = pdev->heart_algo_mode;
        /* [39]wear_bioret(bit7) | algoret(bit6) | acceret(bit5) | wear_drv(bit3~bit0) */
        ble_buf[i * FRAME_SIZE + j++] = (bio_result<<7)|(algo_result<<6)|(acce_result<<5)| (pdev->wear & 0xf);
    }
    buf_size=(ppgsize * FRAME_SIZE);
    buf_index=0;
    uteModuleDugStartSendHrData();
//       debug_orginal_data_send(ppgsize * FRAME_SIZE);
    // UTE_MODULE_LOG(1, "%s,", __func__);
}

#endif

//每秒发送一次
static int uteModuleHrSendStandDugVcxxHrData(void)
{
//    debugSecond=*HrDataSize;
    VCARE_DBG_LOG("buf_index=%d,buf_size=%d",buf_index,buf_size);

    unsigned short int residue_size = 0;
    // 数据发送已经完成
    if (buf_index >= buf_size)
    {
        uteApplicationCommonSyncDataTimerStop();
        return 0;
    }
    // 判断剩余数据长度是否还需要分包
    residue_size = buf_size - buf_index;
    VCARE_DBG_LOG("residue_size=%d,buf_size=%d,",residue_size,buf_size);
    if (residue_size <= BLE_PACK_SIZE)
    {
        VCARE_DBG_LOG("timer_send_data_function return 0");
        uteModuleProfileBle50SendToPhone(&ble_buf[buf_index], residue_size);
        uteApplicationCommonSyncDataTimerStop();
    }
    else
    {
        VCARE_DBG_LOG("timer_send_data_function return 1");
        uteModuleProfileBle50SendToPhone(&ble_buf[buf_index], BLE_PACK_SIZE);
        buf_index += BLE_PACK_SIZE;

    }
    return 1;
}

//准备发送原始数据
void uteModuleDugStartSendHrData(void)
{
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleHrSendStandDugVcxxHrData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(1, "%s,", __func__);
}

#endif
