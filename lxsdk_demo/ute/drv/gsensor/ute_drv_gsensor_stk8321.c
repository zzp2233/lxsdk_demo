/**
*@file
*@brief        gsensor公共层驱动文件
*@details       包括加速度、陀螺、地磁的驱动公共层
*@author       zn.zeng
*@date       2021-07-23
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_drv_gsensor_common.h"
#include "ute_project_config.h"
#include "ute_module_message.h"
#include "ute_module_platform.h"
#include "ute_drv_gsensor_stk8321.h"
#include "ute_module_platform.h"

#if UTE_DRV_STK8321_SUPPORT
/*! skt8321寄存器值zn.zeng, 2021-07-26  */
static uint8_t regValueStk8321[10][2];
/*! skt8321寄存器写入计数zn.zeng, 2021-07-26  */
static uint8_t writeStk8321RegCnt = 0;
/*! skt8321读取fifo异常计数zn.zeng, 2021-07-26  */
static uint8_t readFifoErrorCnt = 0;
/**
*@brief        stk8321写寄存器
*@details
*@param[in] uint8_t reg  寄存器
*@param[in] uint8_t data  寄存器值
*@return  返回true为成功
*@author       zn.zeng
*@date       2021-07-26
*/
bool uteDrvGsensorStk8321WriteReg(uint8_t reg,uint8_t data)
{
    uint8_t i;
    bool ret = false;
    uint8_t value[2];
    value[0] = reg;
    value[1] = data;
    ret = uteModulePlatformTwiWrite(UTE_DRV_GSENSOR_SPI_TWI_ID,UTE_DRV_STK8321_SALVEADDR,&value[0],2);
    for(i=0; i<10; i++)
    {
        if(regValueStk8321[i][0]==reg)
        {
            regValueStk8321[i][1] = data;
            break;
        }
    }
    if(i==10)
    {
        regValueStk8321[writeStk8321RegCnt][0] = reg;
        regValueStk8321[writeStk8321RegCnt][1] = data;
        writeStk8321RegCnt++;
    }
    return ret;
}
/**
*@brief        stk8321读寄存器
*@details
*@param[in] uint8_t reg  寄存器
*@param[in] uint8_t *data  寄存器值
*@param[in] uint8_t size 读取长度
*@return  返回true为成功
*@author       zn.zeng
*@date       2021-07-26
*/
uint8_t uteDrvGsensorStk8321ReadReg(uint8_t reg,uint8_t *data,uint8_t size)
{
    return uteModulePlatformTwiWriteRead(UTE_DRV_GSENSOR_SPI_TWI_ID,UTE_DRV_STK8321_SALVEADDR,&reg,1,data,size);;
}
/**
*@brief        stk8321 acc 初始化
*@details
*@param[in] ute_drv_gsensor_acc_rate_t accRate acc采样率
*@param[in] uint8_t accRange 采样范围
*@return  返回true为成功
*@author       zn.zeng
*@date       2021-07-26
*/
bool uteDrvGsensorStk8321AccInit(ute_drv_gsensor_acc_rate_t accRate,uint8_t accRange)
{
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    return true;
#else
    uint8_t accPmSleepMs = 0;
    if(accRate==GSENSOR_RATE_20HZ)
    {
        accPmSleepMs = STK8321_ACC_50MS;
    }
    else if(accRate==GSENSOR_RATE_40HZ)
    {
        accPmSleepMs = STK8321_ACC_25MS;
    }
    else if(accRate==GSENSOR_RATE_100HZ)
    {
        accPmSleepMs = STK8321_ACC_10MS;
    }
    uteDrvGsensorStk8321WriteReg(0x14,0xB6);
    uteModulePlatformDelayUs(50000);
    //防干扰处理 //ellison  mark Jul 17, 2019
#if DRV_GSENSOR_SPI_SUPPORT
    uint8_t regValue = 0;
    uteDrvGsensorStk8321ReadReg(0x70,&regValue,1); // read IC version
    UTE_LOG(UTE_LOG_SYSTEM_LVL,"%s,regValue=0x%x",__func__,regValue);
    if(regValue==0xE1)   // new version
    {
        uteDrvGsensorStk8321WriteReg(0x33,0x5A);
    }
#endif
    uteDrvGsensorStk8321WriteReg(0x11,0x80);

    if(accRange==8)
    {
        uteDrvGsensorStk8321WriteReg(0x0f,0x08);// +-8G
    }
    else
    {
        uteDrvGsensorStk8321WriteReg(0x0f,0x05);// +-4G
    }
    uteDrvGsensorStk8321WriteReg(0x20,0x00);
    uteDrvGsensorStk8321WriteReg(0x21,0x00);
    uteDrvGsensorStk8321WriteReg(0x11,0x60|accPmSleepMs);
    uteDrvGsensorStk8321WriteReg(0x3D,0x20);
    uteDrvGsensorStk8321WriteReg(0x3E,0xC0);
    return true;
#endif
}
/**
*@brief        stk8321 进入深睡模式
*@details      不做任何采样
*@author       zn.zeng
*@date       2021-07-26
*/
void uteDrvGsensorStk8321DeepSleep(void)
{
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    return;
#else
    uteDrvGsensorStk8321WriteReg(0x20,0x00);
    uteModulePlatformDelayUs(450);
    uteDrvGsensorStk8321WriteReg(0x11,0x7f);
    uteModulePlatformDelayUs(450);
#endif
}
/**
*@brief        stk8321 读取acc x y z 轴的实时数据
*@details
*@param[in]   (int16_t *x, int16_t *y, int16_t *z)
*@author       zn.zeng
*@date       2021-07-26
*/
void uteDrvGsensorStk8321ReadAccXyz(int16_t *x, int16_t *y, int16_t *z)
{
#if 0//UTE_MODULE_CYWEE_MOTION_SUPPORT
    *x = 1;
    *y = 1;
    *z = 128;
#else
    uint8_t buff[6] = {0};
    int16_t temp = 0;
    uteDrvGsensorStk8321ReadReg(0x02,&buff[0],6);
    temp = (int16_t)((buff[1] << 8) | buff[0]);
    *x = temp>>4;
    temp = (int16_t)((buff[3] << 8) | buff[2]);
    *y = temp>>4;
    temp = (int16_t)((buff[5] << 8) | buff[4]);
    *z = temp>>4;
#endif
}
/**
*@brief        stk8321 清除fifo数据
*@details
*@author       zn.zeng
*@date       2021-07-26
*/
void uteDrvGsensorStk8321ClearFifo(void)
{
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    return;
#else

    uteDrvGsensorStk8321WriteReg(0x3e,0xc0);
    uteModulePlatformDelayUs(450);
#endif
}
/**
*@brief        stk8321 读取fifo数据
*@details
*@param  ute_drv_gsensor_common_axis_data_t *axisData  数据指针
*@author       zn.zeng
*@date       2021-07-26
*/
void uteDrvGsensorStk8321ReadFifo(ute_drv_gsensor_common_axis_data_t *axisData)
{
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    axisData->frameCnt = 15;
    axisData->accRange = uteDrvGsensorCommonGetAccRange();
    axisData->adcBit = 12;
#else
    uint8_t buff[30] = {0};
    int16_t temp = 0;
    uint8_t frame_cnt = 0;
    uint8_t lastCnt = 0;
    uint8_t j = 0;
    uint8_t i = 0;
    uint8_t num =0;
    uint8_t status;
    status = uteDrvGsensorStk8321ReadReg(0x0c,&buff[0],1);
    if(status == false)
    {
        frame_cnt = 0;
        goto READ_FINISH;
    }
    frame_cnt=buff[0]&0x7f;
    if(frame_cnt>32)
    {
        frame_cnt = 32;
    }
    lastCnt = frame_cnt%5;
    for(i=0; i<frame_cnt/5; i++)
    {
        status = uteDrvGsensorStk8321ReadReg(0x3f,&buff[0],30);
        if(status == false)
        {
            frame_cnt = 0;
            goto READ_FINISH;
        }
        for(j=0; j<5; j++)
        {
            temp = (int16_t)((buff[6*j+1] << 8) | buff[6*j+0]);
            axisData->accXaxis[num]= temp>>4;
            temp = (int16_t)((buff[6*j+3] << 8) | buff[6*j+2]);
            axisData->accYaxis[num] = temp>>4;
            temp = (int16_t)((buff[6*j+5] << 8) | buff[6*j+4]);
            axisData->accZaxis[num] = temp>>4;
            num++;
        }
    }
    if(lastCnt>0)
    {
        status = uteDrvGsensorStk8321ReadReg(0x3f,&buff[0],6*lastCnt);
        if(status == false)
        {
            frame_cnt = 0;
            goto READ_FINISH;
        }
        for(i=0; i<lastCnt; i++)
        {
            temp = (int16_t)((buff[6*i+1] << 8) | buff[6*i+0]);
            axisData->accXaxis[num]= temp>>4;
            temp = (int16_t)((buff[6*i+3] << 8) | buff[6*i+2]);
            axisData->accYaxis[num] = temp>>4;
            temp = (int16_t)((buff[6*i+5] << 8) | buff[6*i+4]);
            axisData->accZaxis[num] = temp>>4;
            num++;
        }
    }
READ_FINISH:
    axisData->frameCnt = frame_cnt;
    axisData->accRange = uteDrvGsensorCommonGetAccRange();
    axisData->adcBit = 12;
    //
    if(readFifoErrorCnt>5)
    {
        for(i=0; i<writeStk8321RegCnt; i++)
        {
            uint8_t readReg;
            uteDrvGsensorStk8321ReadReg(regValueStk8321[i][0],&readReg,1);
            if((regValueStk8321[i][1]!=readReg)&&(regValueStk8321[i][0]!=0x14))
            {
                UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,regValueStk8321[i][0]=0x%x,regValueStk8321[i][1]=0x%x,readReg=0x%x",__func__,regValueStk8321[i][0],regValueStk8321[i][1],readReg);
                break;
            }
        }
        if(i!=writeStk8321RegCnt)
        {
            uteDrvGsensorStk8321AccInit(uteDrvGsensorCommonGetAccRate(),uteDrvGsensorCommonGetAccRange());
        }
        readFifoErrorCnt = 0;
    }
    else
    {
        readFifoErrorCnt++;
    }
#endif
}
const ute_drv_gsensor_common_config_t drvGsensorStk8321Function=
{
    .id = UTE_DRV_GSENSOR_ID_STK8321,
    .slaveAddress = UTE_DRV_STK8321_SALVEADDR,
    .idReg = 0x00,
    .name = "STK8321",
    .accInit = uteDrvGsensorStk8321AccInit,
    .getAccData = uteDrvGsensorStk8321ReadAccXyz,
    .getFifo = uteDrvGsensorStk8321ReadFifo,
    .deepSleep = uteDrvGsensorStk8321DeepSleep,
    .clearFifo = uteDrvGsensorStk8321ClearFifo,
    .dataBit = 12,
};
const ute_drv_gsensor_common_config_t drvGsensorStk8325Function=
{
    .id = UTE_DRV_GSENSOR_ID_STK8325,
    .slaveAddress = UTE_DRV_STK8321_SALVEADDR,
    .idReg = 0x00,
    .name = "STK8325",
    .accInit = uteDrvGsensorStk8321AccInit,
    .getAccData = uteDrvGsensorStk8321ReadAccXyz,
    .getFifo = uteDrvGsensorStk8321ReadFifo,
    .deepSleep = uteDrvGsensorStk8321DeepSleep,
    .clearFifo = uteDrvGsensorStk8321ClearFifo,
    .dataBit = 12,
};

#endif
