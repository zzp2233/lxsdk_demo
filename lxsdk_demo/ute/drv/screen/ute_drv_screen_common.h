/**
*@file
*@brief     屏驱动文件
*@details    屏驱动公共层文件
*@author        zn.zeng
*@date        2021-09-06
*@version       v1.0
*/


#ifndef _UTE_DRV_SCREEN_COMMON_H_
#define _UTE_DRV_SCREEN_COMMON_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"

/**
 * @brief    屏幕驱动函数接口类型定义（用于屏幕通用抽象封装）
 */
typedef void (*ute_drv_screen_common_init_t)(void);                          // 屏幕初始化函数指针类型（如初始化寄存器、QSPI接口等）
typedef void (*ute_drv_screen_common_power_on_t)(void);                      // 屏幕上电函数指针类型（如发送0x11和0x29）
typedef void (*ute_drv_screen_common_power_off_t)(void);                     // 屏幕下电函数指针类型（如发送0x28和0x10）
typedef void (*ute_drv_screen_common_set_window_t)(uint16_t xStart, 
                                                   uint16_t yStart, 
                                                   uint16_t xEnd, 
                                                   uint16_t yEnd);           // 设置绘图窗口的函数指针类型（裁剪区域、坐标刷新）
typedef void (*ute_drv_screen_common_set_BackLight_t)(uint8_t percent);      // 设置屏幕背光亮度函数指针类型（0~100%）

#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
typedef void (*ute_drv_screen_common_idle_mode_t)(bool enable);              // 设置屏保/休眠模式启用/关闭的函数指针
#endif


/**
 * @brief    屏幕驱动函数配置结构体（用于注册不同屏幕驱动模块）
 */
typedef struct
{
    const char* name;                                                        // 屏幕名称（用于调试、匹配型号，如 "boe_wv018lzq"）

    ute_drv_screen_common_init_t init;                                       // 屏幕初始化函数（如配置寄存器）
    ute_drv_screen_common_power_on_t powerOn;                                // 上电打开屏幕函数
    ute_drv_screen_common_power_off_t powerOff;                              // 下电关闭屏幕函数
    ute_drv_screen_common_set_window_t setWindow;                            // 设置显示窗口区域（用于部分刷新）
    ute_drv_screen_common_set_BackLight_t setBackLight;                      // 设置背光亮度函数（正常模式）

#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    ute_drv_screen_common_idle_mode_t idleMode;                              // 进入或退出屏保/低功耗模式函数
    ute_drv_screen_common_set_BackLight_t setBackLightIdleMode;              // 设置屏保模式下的背光亮度
#endif

    uint8_t  refreshRate;                                                    // 屏幕刷新率（单位 Hz，影响帧率）
    uint16_t startLine;                                                      // 屏幕GRAM起始行（有些控制器如ST7789支持配置起始扫描行）
    uint16_t gramMaxLine;                                                    // 屏幕GRAM最大行数（控制器最大可刷新行数）
} ute_drv_screen_common_config_t;


typedef struct
{
    uint8_t currBackLightPercent;
    bool isInit;
} ute_drv_screen_common_data_t;

void uteDrvScreenCommonInit(void);
void uteDrvScreenCommonSetResetPin(bool isHeight);
void uteDrvScreenCommonSetPowerEnable(bool isEnable);
void uteDrvScreenCommonSetPower(bool isPoweron);
void uteDrvScreenCommonSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void uteDrvScreenCommonClearWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd,uint16_t color);
void uteDrvScreenCommonOpenBacklight(uint8_t percent);
void uteDrvScreenCommonSetBacklightInIdleMode(void);
void uteDrvScreenCommonIdleMode(bool enable);
void uteDrvScreenCommonDisplayOff(void);
uint32_t uteDrvScreenCommonReadId(void);
#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
void uteDrvScreenCommonGc9c01QspiWriteCmdParams(uint8_t cmd,uint8_t *data,uint8_t size);
void uteModulePlatformScreenWriteDataStart(void);
void uteDrvScreenCommonGc9c01QspiWriteCmdParam(uint8_t cmd,uint8_t data);
void uteDrvScreenCommonFt2308QspiWrite16bitCmdParams(uint16_t cmd,uint8_t *data,uint8_t size);
void uteDrvScreenCommonFt2308QspiWrite8bitCmdParams(uint8_t cmd,uint8_t *data,uint8_t size);
void uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(uint8_t cmd,uint8_t *data,uint8_t size);
uint32_t uteDrvScreenCommonGc9c01QspiReadId(void);
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
/**
*@brief   st7789p3 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
void uteDrvScreenCommonSt7789p3DspiWriteCmdParams(uint8_t cmd,uint8_t *data,uint8_t size);
/**
*@brief   st7789p3 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
void uteDrvScreenCommonSt7789p3DspiWriteCmdParam(uint8_t cmd,uint8_t data);
#endif
#if UTE_DRV_SCREEN_ESD_TE_INT_ERROR_RESET_SUPPORT
extern uint8_t  EsdTeIntErrorCheckCnt;
void uteDrvScreenEsdTeIntErrorCheckHandlerMsg(void);
void uteDrvScreenEsdTeIntErrorCheckTimerCallback(void *pxTimer);
void uteDrvScreenEsdTeIntErrorCheckTimerCreate(void);
void uteDrvScreenEsdTeIntErrorCheckTimerStart(uint16_t ms);
void uteDrvScreenEsdTeIntErrorCheckTimerStop(void);
#endif
#endif //_UTE_DRV_SCREEN_COMMON_H_

