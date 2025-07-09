/**
*@file
*@brief        sc7a20h 驱动文件
*@details
*@author       zn.zeng
*@date       2023-06-24
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_drv_gsensor_common.h"
#include "ute_project_config.h"
#include "ute_module_message.h"
#include "ute_module_platform.h"
#include "ute_drv_gsensor_sc7a20h.h"
#include "ute_module_platform.h"
#include "SL_Acc_Mtp_WR.h"
#include "ute_application_common.h"

#if UTE_DRV_GSENSOR_SC7A20H_SUPPORT

bool isSleep=false;//当前是否休眠
static uint8_t buffer[29] = {0};
static uint8_t validValue = 0;

bool uteDrvGsensorSc7a20hWriteReg(uint8_t reg,uint8_t data);
bool uteDrvGsensorSc7a20hReadReg(uint8_t reg,uint8_t *data,uint8_t size);

/********客户需要进行的IIC&SPI接口封包函数************************/
unsigned char SL_SC7A20_I2c_Spi_Write(bool sl_spi_iic,unsigned char reg, unsigned char dat)
{
    uteDrvGsensorSc7a20hWriteReg(reg,dat);
    return true;
}

unsigned char SL_SC7A20_I2c_Spi_Read(bool sl_spi_iic,unsigned char reg, unsigned char len, unsigned char *buf)
{
    uteDrvGsensorSc7a20hReadReg(reg, buf, len);
    return true;
}
/**SL_SC7A20_I2c_Spi_Write 函数中， sl_spi_iic:0=spi  1=i2c  Reg：寄存器地址   dat：寄存器的配置值*******************/
/**SL_SC7A20_I2c_Spi_Write 函数 是一个单次写的函数*******************************************************************/
/***SL_SC7A20_I2c_Spi_Read 函数中， sl_spi_iic:0=spi  1=i2c Reg 同上，len:读取数据长度，buf:存储数据首地址（指针）***/
/***SL_SC7A20_I2c_Spi_Read 函数 是可以进行单次读或多次连续读取的函数*************************************************/


/**
*@brief           sc7a20h 写寄存器
*@details
*@param[in] uint8_t reg  寄存器
*@param[in] uint8_t data  寄存器值
*@return  返回true为成功
*@author       zn.zeng
*@date       2022-06-25
*/
bool uteDrvGsensorSc7a20hWriteReg(uint8_t reg,uint8_t data)
{
    if(!sys_cb.gsensor_iic_en)
    {
        i2c_gsensor_init();
    }
    bool ret = true;
    uint32_t i2c_cfg = START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | WDATA | STOP_FLAG;
    bsp_hw_i2c_tx_byte(i2c_cfg, GSENSOR_WRITE_ADDR(UTE_DRV_SC7A20H_SALVEADDR), reg, data);
    return ret;
}
/**
*@brief        sc7a20h读寄存器
*@details
*@param[in] uint8_t reg  寄存器
*@param[in] uint8_t *data  寄存器值
*@param[in] uint8_t size 读取长度
*@return  返回true为成功
*@author       zn.zeng
*@date       2022-06-25
*/
bool uteDrvGsensorSc7a20hReadReg(uint8_t reg,uint8_t *data,uint8_t size)
{
    if(!sys_cb.gsensor_iic_en)
    {
        i2c_gsensor_init();
    }
    uint32_t i2c_cfg = START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 | DEV_ADDR1;
    bsp_hw_i2c_rx_buf(i2c_cfg, GSENSOR_READ_ADDR(UTE_DRV_SC7A20H_SALVEADDR), reg, data, size);
    return true;
}
/**
*@brief        sc7a20h 进入深睡模式
*@details      不做任何采样
*@author       zn.zeng
*@date       2022-06-25
*/
void uteDrvGsensorSc7a20hDeepSleep(void)
{
    uint8_t readData=0xff;
#if 1
    uteDrvGsensorSc7a20hWriteReg(0x20,0x00);
    uteDrvGsensorSc7a20hWriteReg(0x57,0x0c);
    isSleep = true;
#else
    uteDrvGsensorSc7a20hWriteReg(0x20,0x08);
#endif
    uteDrvGsensorSc7a20hReadReg(0x20,&readData,1);
    if(readData!=0)
    {
        //UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,readData=0x%02x",__func__,readData);
    }
}
/**
*@brief        sc7a20h 软件复位
*@details
*@author       zn.zeng
*@date       2022-06-25
*/
void uteDrvGsensorSc7a20hSoftwareReset(void)
{
    uint8_t readData=0xff;
    uteDrvGsensorSc7a20hWriteReg(0x23, 0x88);
    uteModulePlatformDelayMs(100);
    uteDrvGsensorSc7a20hWriteReg(0x24, 0x80);
    uteDrvGsensorSc7a20hWriteReg(0x68, 0xa5);
    uteModulePlatformDelayMs(200);
    uteDrvGsensorSc7a20hReadReg(0x23,&readData,1);
    if(readData!=0)
    {
        //UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,readData=0x%02x",__func__,readData);
    }
}
/**
*@brief        sc7a20h acc 初始化
*@details
*@param[in] ute_drv_gsensor_acc_rate_t accRate acc采样率
*@param[in] uint8_t accRange 采样范围
*@return  返回true为成功
*@author       zn.zeng
*@date       2022-06-25
*/
bool uteDrvGsensorSc7a20hAccInit(ute_drv_gsensor_acc_rate_t accRate,uint8_t accRange)
{
    uint8_t accPmSleepMs = 0;
    if(accRate==GSENSOR_RATE_20HZ)
    {
        accPmSleepMs = SC7A20H_ACC_50MS;
    }
    else if(accRate==GSENSOR_RATE_40HZ)
    {
        accPmSleepMs = SC7A20H_ACC_25MS;
    }
    else if(accRate==GSENSOR_RATE_100HZ)
    {
        accPmSleepMs = SC7A20H_ACC_10MS;
    }
    uteDrvGsensorSc7a20hDeepSleep();
    //uteDrvGsensorSc7a20hSoftwareReset();
    if(accRange==8)
    {
        uteDrvGsensorSc7a20hWriteReg(0x23,0x88|0x20);// +-8G

    }
    else
    {
        uteDrvGsensorSc7a20hWriteReg(0x23,0x88|0x10);// +-4G
    }
#if 1
    //uteDrvGsensorSc7a20hWriteReg(0x2e,0x80);
#else
    uteDrvGsensorSc7a20hWriteReg(0x2e,0x80);
#endif
    uteDrvGsensorSc7a20hWriteReg(0x21,0x00);
    uteDrvGsensorSc7a20hWriteReg(0x22,0x00);
#if 1
    uteDrvGsensorSc7a20hWriteReg(0x24,0x40);
#else
    uteDrvGsensorSc7a20hWriteReg(0x24,0xC0);
#endif
    uteDrvGsensorSc7a20hWriteReg(0x2e,0x9f);
    uteModulePlatformDelayMs(1);
#if 1
    uteDrvGsensorSc7a20hWriteReg(0x1f,0x00);
    uteDrvGsensorSc7a20hWriteReg(0x20,0x07|accPmSleepMs);
#else
    uteDrvGsensorSc7a20hWriteReg(0x20,0x0f|accPmSleepMs);
#endif
    uteModulePlatformDelayMs(10);
#if 1
    uteDrvGsensorSc7a20hWriteReg(0x57,0x08);//内部上拉电阻断开  //降低关机后功耗
#else
    uteDrvGsensorSc7a20hWriteReg(0x57,0x00);//内部上拉电阻接通
#endif
    isSleep = false;
#if UTE_MODULE_LOG_SUPPORT
    // uint8_t buff[192] = {0};
    // for(uint8_t i=0; i<128; i++)
    // {
    //     uteDrvGsensorSc7a20hReadReg(i,&buff[i],1);
    //     UTE_MODULE_LOG(1, "7a20h init log-----------buff[%x] = %x",i,buff[i]);
    // }
    // // if(timenew.sec%5 == 0)
    // // {
    // //     for(uint8_t i=0; i<128; i++)
    // //     {
    // //         UTE_MODULE_LOG(1, "7a20h init log-----------buff[i] = %x",i,buff[i]);
    // //     }
    // // }
#endif
    /*开机时读取一次，用于判断是否写入过正确buff dengli.lu 2024.04.17*/
    ute_application_sn_data_t snData;
    uteModulePlatformFlashNorRead((uint8_t *)&snData,UTE_USER_PARAM_ADDRESS,sizeof(ute_application_sn_data_t));
    validValue = snData.sc7a20hRetReadValue;
    return true;
}
/**
*@brief        sc7a20h 读取acc x y z 轴的实时数据
*@details
*@param[in]   (int16_t *x, int16_t *y, int16_t *z)
*@author       zn.zeng
*@date       2022-06-25
*/
AT(.com_text.ute_drv_gsensor)
void uteDrvGsensorSc7a20hReadAccXyz(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t buff[6] = {0};
    int16_t temp = 0;
    uteDrvGsensorSc7a20hReadReg(0x61|0x80,&buff[0],6);
    temp = (int16_t)((buff[0] << 8) | buff[1]);
    *x = temp>>4;
    temp = (int16_t)((buff[2] << 8) | buff[3]);
    *y = temp>>4;
    temp = (int16_t)((buff[4] << 8) | buff[5]);
    *z = temp>>4;
// //UTE_MODULE_LOG(1, "factory gsensor log-----------7a20h,x=%d,y=%d,z=%d",*x,*y,*z);
}
/**
*@brief        sc7a20h 清除fifo数据
*@details
*@author       zn.zeng
*@date       2022-06-25
*/
AT(.com_text.ute_drv_gsensor)
void uteDrvGsensorSc7a20hClearFifo(void)
{
    uteDrvGsensorSc7a20hWriteReg(0x2e,0x00);
    uteDrvGsensorSc7a20hWriteReg(0x2e,0x9f);
}
/**
*@brief     判断当前的三轴数据是否异常,数据一直不变就认为不正常。
*@details
*@param[in] int16_t x,
*@param[in] int16_t y,
*@param[in] int16_t z,
*@return  true为异常，false为正常。
*@author       ellison
*@date       2023-12-05
*/
#define SENSOR_EXCEPTION_COUNTER_MAX (32*3) //三帧数据
#define SENSOR_EXCEPTION_OVERVALUE_COUNTER_MAX (32*5) //五帧数据 3.5s
static int16_t previousX = 0;
static int16_t previousY = 0;
static int16_t previousZ = 0;
static uint16_t sameXCount = 0;
static uint16_t sameYCount = 0;
static uint16_t sameZCount = 0;
static uint16_t overvalueXCount = 0;
static uint16_t overvalueYCount = 0;
static uint16_t overvalueZCount = 0;
#define VALUE_MAX (665) //512+512*0.3 //±30%

AT(.com_text.ute_drv_gsensor)
bool uteDrvGsensorSc7a20hCheckIfAnyUnchanged(int16_t x, int16_t y, int16_t z)
{
    bool result = false;
    if(isSleep)
    {
        return result;
    }
    if ((x == previousX) || (y == previousY) || (z == previousZ))
    {
        if((x == previousX))
        {
            previousX = x;
            sameXCount++;
        }

        if((y == previousY))
        {
            previousY = y;
            sameYCount++;
        }

        if((z == previousZ))
        {
            previousZ = z;
            sameZCount++;
        }

        if((x==0) && (y==0) &&(z==0))
        {
            sameXCount++;
            sameYCount++;
            sameZCount++;
        }
        if((sameXCount >=SENSOR_EXCEPTION_COUNTER_MAX)
           ||(sameYCount >=SENSOR_EXCEPTION_COUNTER_MAX)
           ||(sameZCount >=SENSOR_EXCEPTION_COUNTER_MAX)
          )
        {
            result = true;    // 返回异常
        }

    }
    else
    {
        previousX = x;
        previousY = y;
        previousZ = z;
        sameXCount =0;
        sameYCount =0;
        sameZCount =0;
    }
// UTE_MODULE_LOG(1, "7a20h log---sameXCount=%d,sameYCount=%d,sameZCount=%d,  result=%d",sameXCount,sameYCount,sameZCount,result);
    return result; // 返回假
}
/**
*@brief     判断当前的三轴数据是否超出4G值,数据大于五帧的数据就认为不正常。
*@details
*@param[in] int16_t x,
*@param[in] int16_t y,
*@param[in] int16_t z,
*@return  true为异常，false为正常。
*@author       ellison
*@date       2023-12-05
*/
bool uteDrvGsensorSc7a20hCheckIfXYZOverValue(int16_t x, int16_t y, int16_t z)
{
    bool result = false;
    if(isSleep)
    {
        return result;
    }
    if ((abs(x)>=VALUE_MAX || abs(y)>=VALUE_MAX || abs(z)>=VALUE_MAX))
    {
        if((abs(x)>=VALUE_MAX))
        {
            overvalueXCount++;
        }
        else
        {
            overvalueXCount = 0;
        }

        if((abs(y)>=VALUE_MAX))
        {
            overvalueYCount++;
        }
        else
        {
            overvalueYCount = 0;
        }

        if((abs(z)>=VALUE_MAX))
        {
            overvalueZCount++;
        }
        else
        {
            overvalueZCount = 0;
        }

        if((overvalueXCount >=SENSOR_EXCEPTION_OVERVALUE_COUNTER_MAX)
           ||(overvalueYCount >=SENSOR_EXCEPTION_OVERVALUE_COUNTER_MAX)
           ||(overvalueZCount >=SENSOR_EXCEPTION_OVERVALUE_COUNTER_MAX)
          )
        {
            result = true;    // 返回异常
        }

    }
    else
    {
        overvalueXCount =0;
        overvalueYCount =0;
        overvalueZCount =0;
    }
//UTE_MODULE_LOG(1, "7a20h log---overvalueXCount=%d,sameYCount=%d,sameZCount=%d,  result=%d",overvalueXCount,overvalueYCount,overvalueZCount,result);
    return result; // 返回假
}
/**
*@brief        sc7a20h 读取fifo数据
*@details
*@param  ute_drv_gsensor_common_axis_data_t *axisData  数据指针
*@author       zn.zeng
*@date       2022-06-25
*/
ute_drv_gsensor_common_axis_data_t recordLastAxisData;
AT(.com_text.ute_drv_gsensor)
void uteDrvGsensorSc7a20hReadFifo(ute_drv_gsensor_common_axis_data_t *axisData)
{
    uint8_t buff[192] = {0};
    uint8_t frame_cnt = 0;
    bool isException =false;
#if 1  /*MTP写入和CRC校验*/
    uint8_t retReadValue = 0;
    uint8_t retWriteValue = 0;

    retReadValue = SL_SC7A20_MTP_READ(2,buff,0); //0下拉 1上拉
    // UTE_MODULE_LOG(1, "7a20hReadFifo log---,retReadValue=%d",retReadValue);
    // UTE_MODULE_LOG_BUFF(1,buff,29);
    if(retReadValue == 29)
    {
        if(validValue != 29)
        {
            /*把正确的buff(retReadValue等于29时是正确的)存起来，只存这一次，下次异常时再重新写入 dengli.lu 2024.04.17*/
            ute_application_sn_data_t snData;
            uteModulePlatformFlashNorRead((uint8_t *)&snData,UTE_USER_PARAM_ADDRESS,sizeof(ute_application_sn_data_t));
            uteModulePlatformFlashNorErase(UTE_USER_PARAM_ADDRESS);
            memcpy(snData.sc7a20hTempBuffer,buff,29);
            validValue = 29;
            snData.sc7a20hRetReadValue = 29;
            uteModulePlatformFlashNorWrite((uint8_t *)&snData,UTE_USER_PARAM_ADDRESS,sizeof(ute_application_sn_data_t));
        }
    }
    else
    {
        if(validValue == 29)
        {
            /*读取错误时(retReadValue不等于29时是错误的)，把正确的buff从flash中读出来重新写入 dengli.lu 2024.04.17*/
            ute_application_sn_data_t snData;
            uteModulePlatformFlashNorRead((uint8_t *)&snData,UTE_USER_PARAM_ADDRESS,sizeof(ute_application_sn_data_t));
            retWriteValue = SL_SC7A20_MTP_WRITE(2,snData.sc7a20hTempBuffer);
        }
        // UTE_MODULE_LOG(1, "7a20hReadFifo log---,retWriteValue=%d",retWriteValue);
        // return ;
    }
#endif

    uteDrvGsensorSc7a20hReadReg(0x2f,&buff[0],1);
    frame_cnt=buff[0]&0x3f;
    if(frame_cnt>32)
    {
        frame_cnt = 32;
    }
#if 1
    //uteDrvGsensorSc7a20hReadReg(0x69,&buff[0],frame_cnt*6);
    //uteDrvGsensorSc7a20hReadReg(0xa8,&buff[0],frame_cnt*6);
#else
    uteDrvGsensorSc7a20hReadReg(0x69,&buff[0],frame_cnt*6);
    uteDrvGsensorSc7a20hReadReg(0xa8,&buff[0],frame_cnt*6);
#endif
    for(uint8_t i=0; i<frame_cnt; i++)
    {
#if 1
        uteDrvGsensorSc7a20hReadReg(0xa8,&buff[0],6);
        int16_t temp = 0;
        temp = (int16_t)((buff[1] << 8) | buff[0]);
#if 0//UTE_MODULE_LOG_SUPPORT
        axisData->accXaxisNew[i] = temp;
#endif
        axisData->accXaxis[i] = temp >> 4;
        temp = (int16_t)((buff[3] << 8) | buff[2]);
#if 0//UTE_MODULE_LOG_SUPPORT
        axisData->accYaxisNew[i] = temp;
#endif
        axisData->accYaxis[i] = temp >> 4;
        temp = (int16_t)((buff[5] << 8) | buff[4]);
#if 0//UTE_MODULE_LOG_SUPPORT
        axisData->accZaxisNew[i] = temp;
#endif
        axisData->accZaxis[i] = temp >> 4;
#else
        int16_t temp = 0;
        temp = (int16_t)((buff[6 * i + 0] << 8) | buff[6 * i + 1]);
        axisData->accXaxis[i] = temp >> 4;

        temp = (int16_t)((buff[6 * i + 2] << 8) | buff[6 * i + 3]);
        axisData->accYaxis[i] = temp >> 4;

        temp = (int16_t)((buff[6 * i + 4] << 8) | buff[6 * i + 5]);
        axisData->accZaxis[i] = temp >> 4;
#endif
#if 1
        isException =uteDrvGsensorSc7a20hCheckIfAnyUnchanged(axisData->accXaxis[i],axisData->accYaxis[i],axisData->accZaxis[i]);
        isException = isException || uteDrvGsensorSc7a20hCheckIfXYZOverValue(axisData->accXaxis[i],axisData->accYaxis[i],axisData->accZaxis[i]);
        if(isException)
        {
            //reset
            // UTE_MODULE_LOG(1, "7a20h log---uteDrvGsensorSc7a20hSoftwareReset isException=%d",isException);

            uteDrvGsensorSc7a20hSoftwareReset();
            uteDrvGsensorSc7a20hAccInit(UTE_DRV_GSENSOR_DEFAULT_ACC_RATE_VALUE,UTE_DRV_GSENSOR_DEFAULT_ACC_RANGE_VALUE);
            break;
        }
#endif
#if UTE_MODULE_LOG_SUPPORT
        // UTE_MODULE_LOG(1, "7a20hReadFifo log---,buff[0]=%x, buff[1]=%x,buff[2]=%x, buff[3]=%x,buff[4]=%x, buff[5]=%x",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]);
        // UTE_MODULE_LOG(1, "7a20hReadFifo log---,i=%02d, frame_cnt=%02d, x=%x, y=%x, z=%x, %d-%d-%d",i,frame_cnt,axisData->accXaxisNew[i],axisData->accYaxisNew[i],axisData->accZaxisNew[i],abs(axisData->accXaxis[i]),abs(axisData->accYaxis[i]),abs(axisData->accZaxis[i]));
// UTE_MODULE_LOG(1, "7a20hReadFifo log---,x=%x, y=%x, z=%x",axisData->accXaxis[i],axisData->accYaxis[i],axisData->accZaxis[i]);
#endif
    }
    axisData->frameCnt = frame_cnt;
    axisData->accRange = uteDrvGsensorCommonGetAccRange();
    axisData->adcBit = 12;
    if(isException)
    {
        /*记录上一帧数据，异常时把上一帧的数据赋值给当前帧 dengli.lu 2024.01.16*/
        memcpy(axisData,&recordLastAxisData,sizeof(ute_drv_gsensor_common_axis_data_t));
    }
    else
    {
        memset(&recordLastAxisData,0,sizeof(ute_drv_gsensor_common_axis_data_t));
        memcpy(&recordLastAxisData,axisData,sizeof(ute_drv_gsensor_common_axis_data_t));
    }
}
const ute_drv_gsensor_common_config_t drvGsensorSc7a20hFunction=
{
    .id = UTE_DRV_GSENSOR_ID_SC7A20H,
    .slaveAddress = UTE_DRV_SC7A20H_SALVEADDR,
    .idReg = 0x70,
    .name = "sc7a20h",
    .accInit = uteDrvGsensorSc7a20hAccInit,
    .getAccData = uteDrvGsensorSc7a20hReadAccXyz,
    .getFifo = uteDrvGsensorSc7a20hReadFifo,
    .deepSleep = uteDrvGsensorSc7a20hDeepSleep,
    .clearFifo = uteDrvGsensorSc7a20hClearFifo,
    .dataBit = 12,
};
#endif
