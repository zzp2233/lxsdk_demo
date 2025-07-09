/*
Copyright (c) 2017 Silan MEMS. All Rights Reserved.
*/

#ifndef SL_ACC_MTP_WR_DRV__H__
#define SL_ACC_MTP_WR_DRV__H__

//#include "SL_Watch_Pedo_Kcal_Wrist_Sleep_Sway_L_Algorithm.h"
// typedef enum {FALSE = 0,TRUE = !FALSE} bool;

/********客户需要进行的IIC&SPI接口封包函数************************/
extern unsigned char SL_SC7A20_I2c_Spi_Write(bool sl_spi_iic,unsigned char reg, unsigned char dat);
extern unsigned char SL_SC7A20_I2c_Spi_Read(bool sl_spi_iic,unsigned char reg, unsigned char len, unsigned char *buf);
/**SL_SC7A20_I2c_Spi_Write 函数中， sl_spi_iic:0=spi  1=i2c  Reg：寄存器地址   dat：寄存器的配置值*******************/
/**SL_SC7A20_I2c_Spi_Write 函数 是一个单次写的函数*******************************************************************/
/***SL_SC7A20_I2c_Spi_Read 函数中， sl_spi_iic:0=spi  1=i2c Reg 同上，len:读取数据长度，buf:存储数据首地址（指针）***/
/***SL_SC7A20_I2c_Spi_Read 函数 是可以进行单次读或多次连续读取的函数*************************************************/


/*****加测项目客户端读取数据保存校准值*******************************/
unsigned char SL_SC7A20_MTP_READ(unsigned char sensor_flag,unsigned char *sl_cal_save_buf,unsigned char sl_pull_up_mode_set);
/**SC7A20 输入参数：数组首地址，要求数组长度30********/
/**SC7A20E输入参数：数组首地址，要求数组长度13********/
/**SC7A20H输入参数：数组首地址，要求数组长度29********/
/**sensor_flag:0:SC7A20 1:SC7A20E 2:SC7A20H***********/

/***********SC7A20返回参数情况说明********************/
/**Return:  1     备注说明:15寄存器错误***************/
/**Return:  2     备注说明:16寄存器错误***************/
/**Return:  3     备注说明:屏幕不需要点亮*************/
/**Return: 30     备注说明:校准值正常，可以保存；校准值错误，需要重新写入*******/
/***********SC7A20E返回参数情况说明*******************/
/**Return:  1     备注说明:60寄存器错误***************/
/**Return: 13     备注说明:校准值正常，可以保存；校准值错误，需要重新写入*******/
/***********SC7A20H返回参数情况说明*******************/
/**Return:  1     备注说明:5F寄存器错误***************/
/**Return: 29     备注说明:校准值正常，可以保存；校准值错误，需要重新写入*******/


/*****加测项目客户端判断数据是否正常，异常时需要再次写入************/
unsigned char SL_SC7A20_MTP_WRITE(unsigned char sensor_flag,unsigned char *sl_cal_save_buf);
/**SC7A20 输入参数：数组首地址，要求数组长度30********/
/**SC7A20E输入参数：数组首地址，要求数组长度13********/
/**SC7A20H输入参数：数组首地址，要求数组长度29********/
/**sensor_flag:0:SC7A20 1:SC7A20E 2:SC7A20H***********/

/***********返回参数情况说明*******/
/**Return:0 备注说明:写入校准值验证失败，写入失败*****/
/**Return:1 备注说明:写入校准值验证成功，写入成功*****/




#endif/****SL_ACC_MTP_WR_DRV__H__*********/



