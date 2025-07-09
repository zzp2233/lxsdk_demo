/**
*@file
*@brief        sc7a20e 驱动文件
*@details
*@author       zn.zeng
*@date       2022-06-25
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_drv_gsensor_common.h"
#include "ute_project_config.h"
#include "ute_module_message.h"
#include "ute_module_platform.h"
#include "ute_drv_gsensor_sc7a20e.h"
#include "ute_module_platform.h"

#if UTE_DRV_GSENSOR_SC7A20E_SUPPORT
/**
*@brief           sc7a20e 写寄存器
*@details
*@param[in] uint8_t reg  寄存器
*@param[in] uint8_t data  寄存器值
*@return  返回true为成功
*@author       zn.zeng
*@date       2022-06-25
*/
bool uteDrvGsensorSc7a20eWriteReg(uint8_t reg,uint8_t data)
{
    bool ret = false;
    uint8_t value[2];
    value[0] = reg;
    value[1] = data;
    ret = uteModulePlatformTwiWrite(UTE_DRV_GSENSOR_SPI_TWI_ID,UTE_DRV_SC7A20E_SALVEADDR,&value[0],2);
    return ret;
}
/**
*@brief        sc7a20e读寄存器
*@details
*@param[in] uint8_t reg  寄存器
*@param[in] uint8_t *data  寄存器值
*@param[in] uint8_t size 读取长度
*@return  返回true为成功
*@author       zn.zeng
*@date       2022-06-25
*/
uint8_t uteDrvGsensorSc7a20eReadReg(uint8_t reg,uint8_t *data,uint8_t size)
{
    return uteModulePlatformTwiWriteRead(UTE_DRV_GSENSOR_SPI_TWI_ID,UTE_DRV_SC7A20E_SALVEADDR,&reg,1,data,size);;
}
/**
*@brief        sc7a20e 进入深睡模式
*@details      不做任何采样
*@author       zn.zeng
*@date       2022-06-25
*/
void uteDrvGsensorSc7a20eDeepSleep(void)
{
    uint8_t readData=0xff;
    uteDrvGsensorSc7a20eWriteReg(0x20,0x00);
    uteDrvGsensorSc7a20eReadReg(0x20,&readData,1);
    if(readData!=0)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,readData=0x%02x",__func__,readData);
    }
}
/**
*@brief        sc7a20e 软件复位
*@details
*@author       zn.zeng
*@date       2022-06-25
*/
void uteDrvGsensorSc7a20eSoftwareReset(void)
{
    uint8_t readData=0xff;
    uteDrvGsensorSc7a20eWriteReg(0x23, 0x88);
    uteModulePlatformDelayMs(100);
    uteDrvGsensorSc7a20eWriteReg(0x24, 0x80);
    uteDrvGsensorSc7a20eWriteReg(0x68, 0xa5);
    uteModulePlatformDelayMs(200);
    uteDrvGsensorSc7a20eReadReg(0x23,&readData,1);
    if(readData!=0)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,readData=0x%02x",__func__,readData);
    }
}
/**
*@brief        sc7a20e acc 初始化
*@details
*@param[in] ute_drv_gsensor_acc_rate_t accRate acc采样率
*@param[in] uint8_t accRange 采样范围
*@return  返回true为成功
*@author       zn.zeng
*@date       2022-06-25
*/
bool uteDrvGsensorSc7a20eAccInit(ute_drv_gsensor_acc_rate_t accRate,uint8_t accRange)
{
    uint8_t accPmSleepMs = 0;
    if(accRate==GSENSOR_RATE_20HZ)
    {
        accPmSleepMs = SC7A20E_ACC_50MS;
    }
    else if(accRate==GSENSOR_RATE_40HZ)
    {
        accPmSleepMs = SC7A20E_ACC_25MS;
    }
    else if(accRate==GSENSOR_RATE_100HZ)
    {
        accPmSleepMs = SC7A20E_ACC_10MS;
    }
    uteDrvGsensorSc7a20eDeepSleep();
    uteDrvGsensorSc7a20eSoftwareReset();
    uteDrvGsensorSc7a20eWriteReg(0x1f,0x40);//低功耗模式
    if(accRange==8)
    {
        uteDrvGsensorSc7a20eWriteReg(0x23,0x88|0x20);// +-8G
    }
    else
    {
        uteDrvGsensorSc7a20eWriteReg(0x23,0x88|0x10);// +-4G
    }
    uteDrvGsensorSc7a20eWriteReg(0x2e,0x00);
    uteDrvGsensorSc7a20eWriteReg(0x21,0x00);
    uteDrvGsensorSc7a20eWriteReg(0x22,0x00);
    uteDrvGsensorSc7a20eWriteReg(0x24,0x80);
    uteDrvGsensorSc7a20eWriteReg(0x25,0x80);
    uteDrvGsensorSc7a20eWriteReg(0x0e,0x00);
    uteDrvGsensorSc7a20eWriteReg(0x2e,0x9f);
    uteModulePlatformDelayMs(1);
    uteDrvGsensorSc7a20eWriteReg(0x20,0x07|accPmSleepMs);
    uteModulePlatformDelayMs(10);
    uteDrvGsensorSc7a20eWriteReg(0x57,0x00);//内部上拉电阻打开
    uteDrvGsensorSc7a20eWriteReg(0x58,0x02);//Z轴符号
    return true;
}
/**
*@brief        sc7a20e 读取acc x y z 轴的实时数据
*@details
*@param[in]   (int16_t *x, int16_t *y, int16_t *z)
*@author       zn.zeng
*@date       2022-06-25
*/
void uteDrvGsensorSc7a20eReadAccXyz(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buff[3] = {0};
    uteDrvGsensorSc7a20eReadReg(0x28,&buff[0],1);
    uteDrvGsensorSc7a20eReadReg(0x2a,&buff[1],1);
    uteDrvGsensorSc7a20eReadReg(0x2c,&buff[2],1);
    *x = buff[0];
    *y = buff[1];
    *z = buff[2];
}
/**
*@brief        sc7a20e 清除fifo数据
*@details
*@author       zn.zeng
*@date       2022-06-25
*/
void uteDrvGsensorSc7a20eClearFifo(void)
{
    uteDrvGsensorSc7a20eWriteReg(0x2e,0x00);
    uteDrvGsensorSc7a20eWriteReg(0x2e,0x9f);
}
/**
*@brief        sc7a20e 读取fifo数据
*@details
*@param  ute_drv_gsensor_common_axis_data_t *axisData  数据指针
*@author       zn.zeng
*@date       2022-06-25
*/
void uteDrvGsensorSc7a20eReadFifo(ute_drv_gsensor_common_axis_data_t *axisData)
{
    uint8_t buff[96] = {0};
    uint8_t frame_cnt = 0;
    uteDrvGsensorSc7a20eReadReg(0x2f,&buff[0],1);
    frame_cnt=buff[0]&0x3f;
    if(frame_cnt>32)
    {
        frame_cnt = 32;
    }
    uteDrvGsensorSc7a20eReadReg(0x69,&buff[0],frame_cnt*3);
    for(uint8_t i=0; i<frame_cnt; i++)
    {
        axisData->accYaxis[i] = (int8_t)buff[i * 3 + 0];
        axisData->accZaxis[i] = (int8_t)buff[i * 3 + 1];
        axisData->accXaxis[i] = (int8_t)buff[i * 3 + 2];
    }
    axisData->frameCnt = frame_cnt;
    axisData->accRange = uteDrvGsensorCommonGetAccRange();
    axisData->adcBit = 8;
}
const ute_drv_gsensor_common_config_t drvGsensorSc7a20eFunction=
{
    .id = UTE_DRV_GSENSOR_ID_SC7A20E,
    .slaveAddress = UTE_DRV_SC7A20E_SALVEADDR,
    .idReg = 0x70,
    .name = "sc7a20e",
    .accInit = uteDrvGsensorSc7a20eAccInit,
    .getAccData = uteDrvGsensorSc7a20eReadAccXyz,
    .getFifo = uteDrvGsensorSc7a20eReadFifo,
    .deepSleep = uteDrvGsensorSc7a20eDeepSleep,
    .clearFifo = uteDrvGsensorSc7a20eClearFifo,
    .dataBit = 8,
};
#endif
