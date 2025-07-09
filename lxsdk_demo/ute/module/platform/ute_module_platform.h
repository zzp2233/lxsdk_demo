/**
*@file
*@brief     BLE平台头文件
*@details   和BLE平台相关的头文件包含，写在此文件
*@author        zn.zeng
*@date        Jun 29, 2021
*@version       v1.0
*/

#ifndef _UTE_MODULE_PLATFORM_H_
#define _UTE_MODULE_PLATFORM_H_
#include "include.h"
#include "rt_thread.h"
#include "ute_flash_map_common.h"
#include "api_gpio.h"
#include "bsp_sys.h"
#include "tft.h"
#include "port_tft.h"
#include "config_extra.h"
#include "ute_module_mem.h"
#include "rt_thread.h"

#define __STATIC_INLINE  static __inline
#define __SCREEN_COMMON AT(.com_text.tft_spi)

#define UTE_MODULE_PLATFORM_DLPS_BIT_SCREEN        0x00000020
#define UTE_MODULE_PLATFORM_DLPS_BIT_MOTOR         0x00000040
#define UTE_MODULE_PLATFORM_DLPS_BIT_HEART         0x00000080
#define UTE_MODULE_PLATFORM_DLPS_BIT_UART          0x00000100
#define UTE_MODULE_PLATFORM_DLPS_BIT_SYNC          0x00000200
#define UTE_MODULE_PLATFORM_DLPS_BIT_ASR           0x00000400
#define UTE_MODULE_PLATFORM_DLPS_BIT_EARPHONE          0x00000800

#define UTE_MODULE_PLATFORM_TIMER_MAX    8
#define UTE_MODULE_PLATFORM_MUTEX_MAX    6
typedef struct
{
    co_timer_t timer;
    uint32_t currTimerCallback;
    uint32_t duration;
    bool reLoad;
    bool isCreate;
} ute_module_platform_timer_t;
typedef void (*uteModulePlatformTimerCallback_t)(void *pxTimer);
typedef struct
{
    os_mutex_t mutex;
    bool isCreate;
} ute_module_platform_mutex_t;

/**
 * @brief       ble adv 更新状态
 * @details
 */
typedef enum
{
    ADV_LENGTH = 0,
    ADV_FLAG,
    ADV_DATA,
    ADV_NEW_DATA,
} ADV_PARSE_STATE;
/**
 * @brief       ble adv 更新方式
 * @details 是追加还是替换
 */
typedef enum
{
    ADV_APPEND = 0,
    ADV_REPLACE,
} ADV_MODIFY_TYPE;

typedef struct
{
    uint8_t advData[31];
    uint8_t advLen;
    uint8_t scanData[31];
    uint8_t scanLen;
} ute_module_platform_adv_data_t;
/**
*@brief   us延时函数
*@details   小于10ms使用此函数
*@param[in] uint32_t us  延时us数
*@return  void
*@author        zn.zeng
*@date        2023-03-22
*/
void uteModulePlatformDelayUs(uint32_t us);
/**
*@brief   ms延时函数
*@details   大于于10ms使用此函数
*@param[in] uint32_t ms  延时ms数
*@return  void
*@author        zn.zeng
*@date        2023-03-22
*/
void uteModulePlatformDelayMs(uint32_t ms);
/**
*@brief   系统复位重启
*@details
*@author        zn.zeng
*@date        2023-03-22
*/
void uteModulePlatformSystemReboot(void);
/**
*@brief   创建定时器
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
bool uteModulePlatformCreateTimer(void **pp_handle, const char *p_timer_name, uint32_t timer_id,
                                  uint32_t interval_ms, bool reload, void (*p_timer_callback)(void *));
/**
*@brief   重新启动定时器
*@details
*@author  casen
*@date    2023-03-09
*/
bool uteModulePlatformRestartTimer(void **pp_handle, uint32_t interval_ms);
/**
*@brief   停止定时器
*@details
*@author        zn.zeng
*@date        2021-08-31
*/
bool uteModulePlatformStopTimer(void **pp_handle);
/**
*@brief   删除定时器
*@details
*@author        zn.zeng
*@date        2021-08-31
*/
bool uteModulePlatformDeleteTimer(void **pp_handle);
/**
*@brief   关闭系统看门狗
*@details
*@author        zn.zeng
*@date        2021-11-26
*/
void uteModulePlatformWdgDisable(void);
/**
*@brief   打开系统看门狗
*@details
*@author        zn.zeng
*@date        2021-11-26
*/
void uteModulePlatformWdgStart(void);
/**
*@brief   系统看门狗喂狗
*@details
*@author        zn.zeng
*@date        2021-11-26
*/
void uteModulePlatformWdgFeed(void);
/**
*@brief   创建互斥量
*@details
*@return 成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformCreateMutex(void **pp_handle);
/**
*@brief   删除互斥量
*@details
*@return 成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformDeleteMutex(void **pp_handle);
/**
*@brief   使用互斥量
*@details  超时500ms
*@return  成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformTakeMutex(void *pp_handle);
/**
*@brief   释放互斥量
*@details
*@return  成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformGiveMutex(void *pp_handle);
/**
* @brief   nor flash 读函数
*@param[in] uint8_t   *data 数据指针
*@param[in] uint32_t addr  读取地址
*@param[in] uint32_t len 读取长度
*@return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformFlashNorRead(uint8_t   *data, uint32_t addr, uint32_t len);
/**
* @brief   nor flash 写函数
*@param[in] uint8_t   *data 数据指针
*@param[in] uint32_t addr  写地址
*@param[in] uint32_t len 写长度
*@return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformFlashNorWrite(uint8_t *data, uint32_t addr, uint32_t len);
/**
*@brief   nor flash 擦除
*@details 擦除一个SECTOR，4K大小
*@param[in] uint32_t addr  擦除地址
*@return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformFlashNorErase(uint32_t addr);
/**
* @brief   发送消息到app task
*@param[in] uint16_t type ，消息类型
*@param[in] uint32_t param ，消息参数，可以传递一个4byte的参数
* @return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformSendMsgToUteApplicationTask(uint16_t type, uint32_t param);
/**
*@brief   发送消息到app task
*@param[in] uint16_t type ，消息类型
*@param[in] uint32_t param ，消息参数，可以传递一个4byte的参数
*@return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformSendMsgToAppTask(uint16_t type, uint32_t param);
/**
*@brief RTC初始化函数
*@details RTC初始化函数，使用RTC，每秒产生一个消息，作为系统每秒的时间
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformRtcInit(void);
/**
*@brief RTC start函数
*@details RTC开始运行
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformRtcStart(void);
/**
*@brief RTC 设置时间函数
*@details R
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformRtcSetTime(uint16_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec);
/**
*@brief RTC 校准时间函数
*@details
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformCalibrationSystemTimer(void);
#if !UTE_MODULE_USER_MALLOC_SUPPORT
/**
*@brief   动态申请内存
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void *uteModulePlatformMemoryAlloc(size_t size);
/**
*@brief   释放动态申请的内存
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void uteModulePlatformMemoryFree(void * p);
#endif
/**
*@brief   设置gpio输出
*@details
*@param[in] uint8_t pinNum
*@param[in] bool isHeight   输出高低电平
*@author        zn.zeng
*@date        2021-09-07
*/
void uteModulePlatformOutputGpioSet(uint8_t pinNum, bool isHeight);

#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
/**
*@brief   qspi 初始化
*@details  四通道
*@author         zn.zeng
*@date     2024-04-01
*/
void uteModulePlatformScreenQspiInit(void);
/**
*@brief   qspi 写命令
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformScreenQspiWriteCmd(uint8_t *buf, uint32_t len);
/**
*@brief   qspi 读命令
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformScreenQspiReadCmd(uint8_t cmd,uint8_t *buf, uint32_t len);
/**
*@brief   qspi 写gram数据
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformScreenQspiWriteGram(uint8_t cmd);
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
/**
*@brief   dspi 初始化
*@details  双通道
*@author         zn.zeng
*@date     2024-04-01
*/
void uteModulePlatformScreenDspiInit(void);
/**
*@brief   dspi 写命令
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformScreenDspiWriteCmd(uint8_t *buf, uint32_t len);
/**
*@brief   dspi 写gram数据
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformDspiWriteGram(uint8_t cmd);
/**
*@brief   dspi 读寄存器命令
*@details
*@author         zn.zeng
*@date     2024-04-09
*/
void uteModulePlatformScreenDspiReadCmd(uint8_t cmd,uint8_t *buf, uint32_t len,uint8_t dummyClockByte);
#endif
/**
*@brief   PWM 初始化
*@details
*@param[in] pwm_gpio id ,
*@param[in] uint8_t pinNum ,输出pin
*@param[in] uint8_t duty, 占空比
*@param[in] uint32_t rateHz, 频率
*@author        zn.zeng
*@date        2021-10-12
*/
void uteModulePlatformPwmInit(pwm_gpio id,uint8_t pinNum,uint8_t duty,uint32_t rateHz);
/**
*@brief   PWM 打开输出
*@details  不实现，init 就开始了
*@param[in] pwm_gpio id
*@author        zn.zeng
*@date        2021-10-12
*/
void uteModulePlatformPwmEnable(pwm_gpio id);
/**
*@brief   PWM 关闭输出
*@details
*@param[in] uint8_t id ,timer id ,0~7
*@author        zn.zeng
*@date        2021-10-12
*/
void uteModulePlatformPwmDisable(pwm_gpio id,uint8_t pinNum);

/**
*@brief   更新蓝牙名
*@details
*@return  void
*@author        zn.zeng
*@date        2021-08-18
*/
void uteModulePlatformUpdateDevName(void);
/**
*@brief   获取广播剩余长度
*@details
*@return  void
*@author        zn.zeng
*@date        2021-12-08
*/
uint8_t uteModulePlatformGetAdvertismentRemainLength(void);
uint8_t uteModulePlatformGetScanRspRemainLength(void);
/**
*@brief   更新ble广播数据
*@details
*@return  void
*@author        zn.zeng
*@date        Jul 05, 2021
*/
void uteModulePlatformAdvDataUpdate(void);
/**
*@brief   更新ble广播数据
*@details 更新ble广播数据，输入不同广播类型和数据更新
*@param[in]
* @return  void
*@author        zn.zeng
*@date        Jul 05, 2021
*/
void uteModulePlatformAdvDataModifySub(bool isScanData, uint8_t advFlag, const uint8_t *newAdvData, uint8_t newAdvDataLen, ADV_MODIFY_TYPE type);

/**
*@brief   获取蓝牙名
*@details
*@param[out] uint8_t *name
*@param[in] uint8_t *size，输入最大buff大小，输出实际大小
*@return  void
*@author        zn.zeng
*@date        2022-01-20
*/
void uteModulePlatformGetDevName(uint8_t *name,uint8_t *size);
/**
 * @brief        获取完整蓝牙名
 * @details      蓝牙名称带设备ID
 * @param[out]   uint8_t *name
 * @param[in]    uint8_t *size，输入最大buff大小，输出实际大小
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-16
 */
void uteModulePlatformGetDevCompleteName(uint8_t *name,uint8_t *size);
/**
*@brief   获取BLE蓝牙地址
*@details
*@param[out] uint8_t *mac  6byte指针
* @return  void
*@author        zn.zeng
*@date        Jul 05, 2021
*/
void uteModulePlatformGetBleMacAddress(uint8_t *mac);

/**
* @brief        修改蓝牙地址
* @details      修改后重启生效
* @param[in]    uint8_t *mac 6byte
* @return       ture 修改成功 false 修改失败
* @author       Wang.Luo
* @date         2022-09-30
*/
bool uteModulePlatformSetBleMacAddress(uint8_t *mac);

uint8_t uteModulePlatformGetAdvData(uint8_t *advData, uint8_t advDataLen);

uint8_t uteModulePlatformGetScanRspData(uint8_t *scanRsp, uint8_t scanRspLen);

void uteModulePlatformAdvDataInit(void);

void uteModulePlatformDlpsEnable(uint32_t bit);
void uteModulePlatformDlpsDisable(uint32_t bit);
uint32_t uteModulePlatformNotAllowSleep(void);
void uteModulePlatformDlpsBitReset(void);

void uteModulePlatformRejectIncall(void);

#endif //_UTE_MODULE_PLATFORM_H_

