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
#include "ute_drv_gsensor_stk8321.h"
#include "ute_drv_gsensor_sc7a20e.h"
#include "ute_drv_gsensor_sc7a20h.h"
/*! 驱动配置函数指针zn.zeng, 2021-07-23  */
const ute_drv_gsensor_common_config_t *uteDrvGsensorCommonFunction=NULL;
/*!数据 zn.zeng, 2021-07-26  */
ute_drv_gsensor_common_data_t uteDrvGsensorCommonData;
/*! 兼容不同sensor 指针列表 zn.zeng, 2021-07-26  */
const ute_drv_gsensor_common_config_t *uteDrvGsensorConfigList[]=
{
#if UTE_DRV_STK8321_SUPPORT
    &drvGsensorStk8321Function,
    &drvGsensorStk8325Function,
#endif
#if UTE_DRV_GSENSOR_SC7A20E_SUPPORT
    &drvGsensorSc7a20eFunction,
#endif
#if UTE_DRV_GSENSOR_SC7A20H_SUPPORT
    &drvGsensorSc7a20hFunction,
#endif
};
/**
*@brief        gsensor读取id
*@details      spi和i2c接口度使用此函数
*@param[in]    uint8_t slaveAdress  i2c器件地址
*@param[in]    uint8_t reg  器件地址读取寄存器
*@return true为成功
*@author       zn.zeng
*@date       2021-07-24
*/
uint8_t uteDrvGsensorCommonReadId(uint8_t slaveAddress,uint8_t reg)
{
    uint8_t readId = 0;
    if(!sys_cb.gsensor_iic_en)
    {
        i2c_gsensor_init();
    }
    // uteModulePlatformTwiWriteRead(UTE_DRV_GSENSOR_SPI_TWI_ID,slaveAddress,&reg,1,&readId,1);
    uint32_t i2c_cfg = START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 | DEV_ADDR1;
    bsp_hw_i2c_rx_buf(i2c_cfg, GSENSOR_READ_ADDR(slaveAddress), reg, &readId, 1);

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,slaveAddress=0x%x,reg=0x%x,readId=0x%02x", __func__,slaveAddress,reg,readId);
#if UTE_DRV_STK8321_SUPPORT
    if((readId == 0x21) || (readId == 0x25))/*读到错误ID，0x14寄存器需要写一次0xB6做Gsensor软复位，然后重新读取一次ID，dengli.lu 2024-03-18*/
    {
        uteDrvGsensorStk8321WriteReg(0x14,0xB6);
        uteModulePlatformDelayUs(50000);
        uteModulePlatformTwiWriteRead(UTE_DRV_GSENSOR_SPI_TWI_ID,slaveAddress,&reg,1,&readId,1);
    }
    if(readId == 0x26)/*STK8321系列的一款型号，ID也是0x26(和7A20E一样),可读0x28寄存器判断，dengli.lu 2024-03-18*/
    {
        uint8_t regValue = 0;
        uteDrvGsensorStk8321ReadReg(0x28,&regValue,1);
        if(regValue == 0x14)
        {
            uteDrvGsensorStk8321WriteReg(0x14,0xB6);
            uteModulePlatformDelayUs(50000);
            uteModulePlatformTwiWriteRead(UTE_DRV_GSENSOR_SPI_TWI_ID,slaveAddress,&reg,1,&readId,1);//原厂补充修改，在此改为重新读取一次ID dengli.lu 2024-04-17*/
        }
    }
#endif
    return readId;
}
/**
*@brief        gsensor acc驱动初始化
*@details
*@param[in]     ute_drv_gsensor_acc_rate_t sampleRate  采样率
*@param[in]    uint8_t accRange  加速度范围
*@return true为成功
*@author       zn.zeng
*@date       2021-07-23
*/
bool uteDrvGsensorCommonInit(ute_drv_gsensor_acc_rate_t accRate,uint8_t accRange)
{
    bool initResult =false;
    uteDrvGsensorCommonData.accRate = accRate;
    uteDrvGsensorCommonData.accRange = accRange;
    uint8_t listSize = sizeof(uteDrvGsensorConfigList)/4;
    if(listSize==0)
    {
//          "err!";
        return true;
    }
    uteDrvGsensorCommonFunction = uteDrvGsensorConfigList[0];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,listSize=%d", __func__,listSize);
    uint8_t id = 0;
    for(uint8_t i=0; i<listSize; i++)
    {
        id = uteDrvGsensorCommonReadId(uteDrvGsensorConfigList[i]->slaveAddress,uteDrvGsensorConfigList[i]->idReg);
        if(id==uteDrvGsensorConfigList[i]->id)
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,name=%s", __func__,uteDrvGsensorConfigList[i]->name);
            uteDrvGsensorCommonFunction = uteDrvGsensorConfigList[i];
            break;
        }
    }
    if(id==0)
    {
        // 读取不到使用8321的 function
        uteDrvGsensorCommonFunction = uteDrvGsensorConfigList[0];
    }
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    return true;
#else
    uteDrvGsensorCommonFunction->accInit(accRate,accRange);
#endif
    return initResult;
}
/**
*@brief        转换xyz轴数据
*@details      转换xyz轴数据，使用不同的bit
*@param[in]     传入输入输出指针
*@param[in]    转换类型
*@author       zn.zeng
*@date       2021-07-23
*/
AT(.com_text.ute_drv_gsensor)
void uteDrvGsensorCommonXYZaxisDataBitChange(ute_drv_gsensor_common_axis_bit_change_t *bitChange,uint8_t size,ute_drv_gsensor_data_bit_change_type_t type)
{
    int8_t offsetBit = 0;
    uint8_t dataBit = uteDrvGsensorCommonFunction->dataBit;
    if(type==GSENSOR_DATA_BIT_STEP)
    {

        if(uteDrvGsensorCommonData.accRange ==4)//+-4G
        {
            offsetBit = dataBit-10;
        }
        else // +-8G
        {
            offsetBit = dataBit-9;
        }
    }
    else if(type==GSENSOR_DATA_BIT_VCXX)
    {
        offsetBit = dataBit-11;
    }
    for(uint8_t i =0 ; i<size; i++)
    {
        if (offsetBit >= 0)
        {
            bitChange->outputXaxis[i] = bitChange->inputXaxis[i] >> offsetBit;
            bitChange->outputYaxis[i] = bitChange->inputYaxis[i] >> offsetBit;
            bitChange->outputZaxis[i] = bitChange->inputZaxis[i] >> offsetBit;
        }
        else
        {
            bitChange->outputXaxis[i] = bitChange->inputXaxis[i] << (-offsetBit);
            bitChange->outputYaxis[i] = bitChange->inputYaxis[i] << (-offsetBit);
            bitChange->outputZaxis[i] = bitChange->inputZaxis[i] << (-offsetBit);
        }
    }
}
/**
*@brief        获取当前acc 的采样范围
*@details      +-G
*@return  返回采样范围
*@author       zn.zeng
*@date       2021-07-26
*/
uint8_t uteDrvGsensorCommonGetAccRange(void)
{
    return uteDrvGsensorCommonData.accRange;
}
/**
*@brief        获取当前acc 的采样频率
*@details
*@return  返回采样频率
*@author       zn.zeng
*@date       2021-07-26
*/
AT(.com_text.ute_drv_gsensor)
ute_drv_gsensor_acc_rate_t uteDrvGsensorCommonGetAccRate(void)
{
    return uteDrvGsensorCommonData.accRate;
}
/**
*@brief        获取当前acc 的x y z 实时数据
*@details
*@author       zn.zeng
*@date       2021-07-26
*/
void uteDrvGsensorCommonGetAccXyz(int16_t *x,int16_t *y,int16_t *z)
{
    uteDrvGsensorCommonFunction->getAccData(x,y,z);
}
/**
*@brief        gsensor睡眠
*@details
*@author       zn.zeng
*@date       2021-07-26
*/
void uteDrvGsensorCommonSleep(void)
{
    uteDrvGsensorCommonFunction->deepSleep();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
}
/**
*@brief        gsensor 清除fifo数据
*@details
*@author       zn.zeng
*@date       2021-07-26
*/
AT(.com_text.ute_drv_gsensor)
void uteDrvGsensorCommonClearFifo(void)
{
    uteDrvGsensorCommonFunction->clearFifo();
    uteDrvGsensorCommonData.axisData.isHasReadFifo = false;
}
/**
*@brief        gsensor 读取fifo数据
*@details
*@param[out] ute_drv_gsensor_common_axis_data_t * 返回数据指针，全局变量
*@author       zn.zeng
*@date       2021-07-26
*/
AT(.com_text.ute_drv_gsensor)
void uteDrvGsensorCommonReadFifo(ute_drv_gsensor_common_axis_data_t **data)
{
    if(!uteDrvGsensorCommonData.axisData.isHasReadFifo)
    {
        memset(&uteDrvGsensorCommonData.axisData,0,sizeof(ute_drv_gsensor_common_axis_data_t));
        uteDrvGsensorCommonFunction->getFifo(&uteDrvGsensorCommonData.axisData);
        uteDrvGsensorCommonData.axisData.isHasReadFifo = true;
    }
    *data = &uteDrvGsensorCommonData.axisData;
}
/**
*@brief        获取当前使用的gsensor id
*@details
*@author       zn.zeng
*@date       2021-07-26
*/
uint8_t uteDrvGsensorCommonGetId(void)
{
    return uteDrvGsensorCommonFunction->id;
}

