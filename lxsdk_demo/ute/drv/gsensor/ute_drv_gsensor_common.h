/**
*@file
*@brief        gsensor公共层驱动头文件
*@details       包括加速度、陀螺、地磁的驱动公共层
*@author       zn.zeng
*@date       2021-07-23
*@version      v1.0
*/


#ifndef _UTE_DRV_GSENSOR_COMMON_H_
#define _UTE_DRV_GSENSOR_COMMON_H_
#include "ute_module_platform.h"
#include "ute_project_config.h"
#define  UTE_DRV_GSENSOR_AXIS_DATA_MAX 50
#define  UTE_DRV_GSENSOR_ID_SC7A20   0x11
#define  UTE_DRV_GSENSOR_ID_SC7A22   0x13
#define  UTE_DRV_GSENSOR_ID_STK8321  0x23
#define  UTE_DRV_GSENSOR_ID_STK8325  0x25
#define  UTE_DRV_GSENSOR_ID_ICM40607 0x38
#define  UTE_DRV_GSENSOR_ID_BMI160   0xD1
#define  UTE_DRV_GSENSOR_ID_BMI270   0x24
#define  UTE_DRV_GSENSOR_ID_SC7A20E  0x26
#define  UTE_DRV_GSENSOR_ID_SC7A20H  0x28
/*! acc 采样率zn.zeng, 2021-07-26  */

#define GSENSOR_WRITE_ADDR(ADDR)            ((ADDR) << 1)
#define GSENSOR_READ_ADDR_UPDATE(ADDR)      ((ADDR) << 1 | 1)
#define GSENSOR_READ_ADDR(ADDR)             ((ADDR) << 1 | 1) << 8 | ((ADDR) << 1)

typedef enum
{
    GSENSOR_RATE_20HZ,
    GSENSOR_RATE_40HZ,
    GSENSOR_RATE_100HZ,
} ute_drv_gsensor_acc_rate_t;
/*! acc 数据转化bit  zn.zeng, 2021-07-26  */
typedef enum
{
    GSENSOR_DATA_BIT_STEP,
    GSENSOR_DATA_BIT_VCXX,
} ute_drv_gsensor_data_bit_change_type_t;

/*!各轴读取数据 zn.zeng, 2021-07-26  */
typedef struct
{
    int16_t accXaxis[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
    int16_t accYaxis[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
    int16_t accZaxis[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
#if UTE_DRV_GYROSCOPE_SUPPORT
    int16_t gyrXaxis[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
    int16_t gyrYaxis[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
    int16_t gyrZaxis[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
    uint16_t gyrRange;
#endif
#if UTE_DRV_GSENSOR_TWI_MAGNETIC_SUPPORT
    int16_t magXaxis[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
    int16_t magYaxis[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
    int16_t magZaxis[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
    uint16_t magRange;
#endif
    uint8_t frameCnt;
    uint8_t accRange;
    uint8_t adcBit;
    bool isHasReadFifo;
} ute_drv_gsensor_common_axis_data_t;

typedef bool (*ute_drv_gsensor_common_acc_init_t)(ute_drv_gsensor_acc_rate_t accRate,uint8_t accRange);
typedef void (*ute_drv_gsensor_common_get_acc_data_t)(int16_t *x, int16_t *y, int16_t *z);
typedef void (*ute_drv_gsensor_common_get_fifo_t)(ute_drv_gsensor_common_axis_data_t *axisData);
typedef void (*ute_drv_gsensor_common_deep_sleep_t)(void);
typedef void (*ute_drv_gsensor_common_clear_fifo_t)(void);
typedef uint8_t (*ute_drv_gsensor_common_get_acc_range_t)(void);

#if DRV_GYROSCOPE_SUPPORT
typedef bool (*ute_drv_acc_gyr_common_init_t)(ute_drv_gsensor_acc_rate_t sampleRate,uint8_t accRange,uint16_t gyrRange);
typedef void (*ute_drv_gyr_common_get_data_t)(int16_t *x, int16_t *y, int16_t *z);
typedef void (*ute_drv_gyr_common_sleep_t)(void);
typedef uint16_t (*ute_drv_gyr_common_get_range_t)(void);
typedef bool (*ute_drv_gyr_common_is_use_t)(void);
#endif

#if UTE_DRV_GSENSOR_TWI_MAGNETIC_SUPPORT
typedef void (*ute_drv_mag_common_int_t)(void);
typedef void (*ute_drv_mag_common_sleep_t)(void);
typedef bool (*ute_drv_mag_common_write_reg_t)(uint8_t reg,uint8_t data);
typedef bool (*ute_drv_mag_common_read_reg_t)(uint8_t reg,uint8_t *data,uint8_t size);
typedef void (*ute_drv_mag_common_set_auto_mode_t)(uint8_t reg);

#endif
/*! 配置gsensor函数指针数据结构 zn.zeng, 2021-07-23  */
typedef struct
{
    uint8_t id;/*! 器件id zn.zeng, 2021-07-23  */
    uint8_t slaveAddress;/*! i2c器件7bit地址 zn.zeng, 2021-07-23  */
    uint8_t idReg;/*! 器件读取id的寄存器 zn.zeng, 2021-07-23  */
    char *name;/*! 器件型号zn.zeng, 2021-07-24  */
    uint8_t dataBit;
    ute_drv_gsensor_common_acc_init_t accInit;
    ute_drv_gsensor_common_get_acc_data_t getAccData;
    ute_drv_gsensor_common_get_fifo_t getFifo;
    ute_drv_gsensor_common_deep_sleep_t deepSleep;
    ute_drv_gsensor_common_clear_fifo_t clearFifo;
    ute_drv_gsensor_common_get_acc_range_t getAccRange;
#if UTE_DRV_GYROSCOPE_SUPPORT
    ute_drv_acc_gyr_common_init_t accGyrInit;
    ute_drv_gyr_common_get_data_t getGyrData;
    ute_drv_gyr_common_sleep_t gyrSleep;
    ute_drv_gyr_common_get_range_t gyrGetRange;
    ute_drv_gyr_common_is_use_t gyrIsUse;
#endif
#if UTE_DRV_GSENSOR_TWI_MAGNETIC_SUPPORT
    ute_drv_mag_common_int_t magInit;
    ute_drv_mag_common_sleep_t magSleep;
    ute_drv_mag_common_write_reg_t magWriteReg;
    ute_drv_mag_common_read_reg_t magReadReg;
    ute_drv_mag_common_set_auto_mode_t magSetAutoMode;
#endif
} ute_drv_gsensor_common_config_t;

typedef struct
{
    int16_t *inputXaxis;
    int16_t *inputYaxis;
    int16_t *inputZaxis;
    int16_t *outputXaxis;
    int16_t *outputYaxis;
    int16_t *outputZaxis;
} ute_drv_gsensor_common_axis_bit_change_t;
typedef struct
{
    uint8_t accRange;
    ute_drv_gsensor_acc_rate_t accRate;
    ute_drv_gsensor_common_axis_data_t axisData;
} ute_drv_gsensor_common_data_t;

uint8_t uteDrvGsensorCommonReadId(uint8_t slaveAdress,uint8_t reg);
bool uteDrvGsensorCommonInit(ute_drv_gsensor_acc_rate_t accRate,uint8_t accRange);
void uteDrvGsensorCommonXYZaxisDataBitChange(ute_drv_gsensor_common_axis_bit_change_t *bitChange,uint8_t size,ute_drv_gsensor_data_bit_change_type_t type);
uint8_t uteDrvGsensorCommonGetAccRange(void);
ute_drv_gsensor_acc_rate_t uteDrvGsensorCommonGetAccRate(void);
void uteDrvGsensorCommonGetAccXyz(int16_t *x,int16_t *y,int16_t *z);
void uteDrvGsensorCommonSleep(void);
void uteDrvGsensorCommonClearFifo(void);
void uteDrvGsensorCommonReadFifo(ute_drv_gsensor_common_axis_data_t **data);
uint8_t uteDrvGsensorCommonGetId(void);

#endif // _UTE_DRV_GSENSOR_COMMON_H_

