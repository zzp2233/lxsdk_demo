/**
*@file
*@brief        BLE平台api文件
*@details  和BLE平台相关的io操作等
*@author       zn.zeng
*@date       Jun 29, 2021
*@version      v1.0
*/
#include "ute_module_platform.h"
#include "ute_module_log.h"
#include "ute_project_config.h"
#include "ute_module_message.h"
#include "ute_task_application.h"
#include "ute_module_systemtime.h"
#include "ute_module_call.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_protocol.h"
#include "ute_drv_motor.h"
#include "include.h"

ute_module_platform_adv_data_t uteModulePlatformAdvData;

static uint32_t uteModulePlatformDlpsBit = 0;
//AT(.sleep_text.ute_sleep_bit)

/**
*@brief   us延时函数
*@details   小于10ms使用此函数
*@param[in] uint32_t us  延时us数
*@return  void
*@author        zn.zeng
*@date        2023-03-22
*/
void uteModulePlatformDelayUs(uint32_t us)
{
    delay_us(us);
}
/**
*@brief   ms延时函数
*@details   大于于10ms使用此函数
*@param[in] uint32_t ms  延时ms数
*@return  void
*@author        zn.zeng
*@date        2023-03-22
*/
void uteModulePlatformDelayMs(uint32_t ms)
{
    delay_ms(ms);
}
/**
*@brief   系统复位重启
*@details
*@author        zn.zeng
*@date        2023-03-22
*/
void uteModulePlatformSystemReboot(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s",__func__);
    // WDT_RST();
    //主动写入软件复位原因
    LVDCON &= ~(0x1f << 8);
    LVDCON |= BIT(12);
    LVDCON |= (1 << 8);
}
static ute_module_platform_timer_t uteModulePlatformTimerData[UTE_MODULE_PLATFORM_TIMER_MAX] AT(.disp.ute);
uint8_t uteModulePlatformTimerCounter = 0;
static void uteModulePlatformTimerCallback(co_timer_t *timer, void *param)
{
    uteModulePlatformTimerCallback_t func = (uteModulePlatformTimerCallback_t)param;
    func(NULL);
}
/**
*@brief   创建定时器
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
bool uteModulePlatformCreateTimer(void **pp_handle, const char *p_timer_name, uint32_t timer_id,
                                  uint32_t interval_ms, bool reload, void (*p_timer_callback)(void *))
{
    bool result = false;
    /*在创建的定时器指针要为NULL*/
    if(*pp_handle != NULL)
    {
        return false;
    }
    /*在timer 数为0时清空数据*/
    if(uteModulePlatformTimerCounter == 0)
    {
        for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
        {
            uteModulePlatformTimerData[i].isCreate = false;
            uteModulePlatformTimerData[i].reLoad = false;
        }
    }
    /*通过声明的全局数组返回对应指针*/
    if(uteModulePlatformTimerCounter<UTE_MODULE_PLATFORM_TIMER_MAX)
    {
        for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
        {
            if(uteModulePlatformTimerData[i].isCreate == false)
            {
                /*返回一个指针*/
                (*pp_handle) = &uteModulePlatformTimerData[i];
                uteModulePlatformTimerData[i].duration = interval_ms;
                uteModulePlatformTimerData[i].reLoad = reload;
                uteModulePlatformTimerData[i].isCreate = true;
                uteModulePlatformTimerData[i].currTimerCallback = (uint32_t)p_timer_callback;
                uteModulePlatformTimerCounter++;
                if(reload)
                {
                    result = co_timer_set(&uteModulePlatformTimerData[i].timer, interval_ms, TIMER_REPEAT, LEVEL_LOW_PRI, uteModulePlatformTimerCallback,(void*)uteModulePlatformTimerData[i].currTimerCallback);
                }
                else
                {
                    result = co_timer_set(&uteModulePlatformTimerData[i].timer, interval_ms, TIMER_ONE_SHOT, LEVEL_LOW_PRI, uteModulePlatformTimerCallback,(void*)uteModulePlatformTimerData[i].currTimerCallback);
                }
                co_timer_set_sleep(&uteModulePlatformTimerData[i].timer,true);
                co_timer_enable(&uteModulePlatformTimerData[i].timer,false);
                break;
            }
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,result=%d,pp_handle=%p,uteModulePlatformHrTimerCounter = %d",__func__,result,*pp_handle,uteModulePlatformTimerCounter);
    return result;
}
/**
*@brief   重新启动定时器
*@details
*@author  casen
*@date    2023-03-09
*/
bool uteModulePlatformRestartTimer(void **pp_handle, uint32_t interval_ms)
{
    bool result = false;
    if(interval_ms==0 || (pp_handle == NULL || (*pp_handle) == NULL))
    {
        return false;
    }
    for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
    {
        if(&uteModulePlatformTimerData[i] == (*pp_handle))
        {
            if(uteModulePlatformTimerData[i].isCreate)
            {
                if(uteModulePlatformTimerData[i].duration==interval_ms)
                {
                    result = co_timer_reset(&uteModulePlatformTimerData[i].timer);
                }
                else
                {
                    uteModulePlatformTimerData[i].duration = interval_ms;
                    co_timer_del(&uteModulePlatformTimerData[i].timer);
                    if(uteModulePlatformTimerData[i].reLoad)
                    {
                        result = co_timer_set(&uteModulePlatformTimerData[i].timer, interval_ms, TIMER_REPEAT, LEVEL_LOW_PRI, uteModulePlatformTimerCallback,(void*)uteModulePlatformTimerData[i].currTimerCallback);
                    }
                    else
                    {
                        result = co_timer_set(&uteModulePlatformTimerData[i].timer, interval_ms, TIMER_ONE_SHOT, LEVEL_LOW_PRI, uteModulePlatformTimerCallback,(void*)uteModulePlatformTimerData[i].currTimerCallback);
                    }
                    co_timer_set_sleep(&uteModulePlatformTimerData[i].timer,true);
                }
                result = co_timer_enable(&uteModulePlatformTimerData[i].timer,true);
            }
            break;
        }
    }
    return result;
}
/**
*@brief   停止定时器
*@details
*@author        zn.zeng
*@date        2021-08-31
*/
bool uteModulePlatformStopTimer(void **pp_handle)
{
    bool result = false;
    if(pp_handle == NULL || (*pp_handle) == NULL)
    {
        return false;
    }
    for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
    {
        if((&uteModulePlatformTimerData[i]) == (*pp_handle))
        {
            if(uteModulePlatformTimerData[i].isCreate)
            {
                co_timer_enable(&uteModulePlatformTimerData[i].timer,false);
                result = true;
                break;
            }
        }
    }
    return result;
}
/**
*@brief   删除定时器
*@details
*@author        zn.zeng
*@date        2021-08-31
*/
bool uteModulePlatformDeleteTimer(void **pp_handle)
{
    bool result = false;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,pp_handle = %p,uteModulePlatformTimerCounter=%d",__func__,*pp_handle,uteModulePlatformTimerCounter);
    if(pp_handle == NULL || (*pp_handle) == NULL)
    {
        return result;
    }
    for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
    {
        if((&uteModulePlatformTimerData[i]) == (*pp_handle))
        {
            /*清除对应数据*/
            (*pp_handle) = NULL;
            uteModulePlatformTimerData[i].isCreate = false;
            uteModulePlatformTimerData[i].reLoad = false;
            co_timer_del(&uteModulePlatformTimerData[i].timer);
            result = true;
            uteModulePlatformTimerCounter--;
            break;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,result=%d",__func__,result);
    return result;
}
/**
*@brief   关闭系统看门狗
*@details
*@author        zn.zeng
*@date        2021-11-26
*/
void uteModulePlatformWdgDisable(void)
{
    WDT_DIS();
}
/**
*@brief   打开系统看门狗
*@details
*@author        zn.zeng
*@date        2021-11-26
*/
void uteModulePlatformWdgStart(void)
{
    WDT_EN();
}
/**
*@brief   系统看门狗喂狗
*@details
*@author        zn.zeng
*@date        2021-11-26
*/
void uteModulePlatformWdgFeed(void)
{
    WDT_CLR();
}

static ute_module_platform_mutex_t uteModulePlatformMutexBuff[UTE_MODULE_PLATFORM_MUTEX_MAX] AT(.disp.ute);
uint8_t uteModulePlatformMutexCnt = 0;
/**
*@brief   创建互斥量
*@details
*@return 成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformCreateMutex(void **pp_handle)
{
    bool result = false;

    if (uteModulePlatformMutexCnt == 0)
    {
        for (uint8_t i = 0; i < UTE_MODULE_PLATFORM_MUTEX_MAX; i++)
        {
            uteModulePlatformMutexBuff[i].isCreate = false;
            uteModulePlatformMutexBuff[i].mutex = NULL;
        }
    }

    if (uteModulePlatformMutexCnt < UTE_MODULE_PLATFORM_MUTEX_MAX)
    {
        for (uint8_t i = 0; i < UTE_MODULE_PLATFORM_MUTEX_MAX; i++)
        {
            if (!uteModulePlatformMutexBuff[i].isCreate)
            {
                uteModulePlatformMutexBuff[i].mutex = os_mutex_create("ute", OS_IPC_FLAG_FIFO);
                if (uteModulePlatformMutexBuff[i].mutex != NULL)
                {
                    *pp_handle = &uteModulePlatformMutexBuff[i];
                    uteModulePlatformMutexBuff[i].isCreate = true;
                    uteModulePlatformMutexCnt++;
                    result = true;
                }
                else
                {
                    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, create error", __func__);
                }
                break;
            }
        }
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, uteModulePlatformMutexCnt=%d, is max", __func__, uteModulePlatformMutexCnt);
    }
    return result;
}

/**
*@brief   删除互斥量
*@details
*@return 成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformDeleteMutex(void **pp_handle)
{
    bool result = false;
    if (pp_handle == NULL || (*pp_handle) == NULL)
    {
        return result;
    }

    for (uint8_t i = 0; i < UTE_MODULE_PLATFORM_MUTEX_MAX; i++)
    {
        if (&uteModulePlatformMutexBuff[i] == (*pp_handle))
        {
            if (uteModulePlatformMutexBuff[i].mutex != NULL)
            {
                os_err_t ret = os_mutex_delete(uteModulePlatformMutexBuff[i].mutex);
                if (ret == OS_EOK)
                {
                    (*pp_handle) = NULL;
                    uteModulePlatformMutexBuff[i].isCreate = false;
                    uteModulePlatformMutexBuff[i].mutex = NULL;
                    uteModulePlatformMutexCnt--;
                    result = true;
                }
                else
                {
                    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, delete error, ret=%d", __func__, ret);
                }
            }
            break;
        }
    }
    return result;
}

/**
*@brief   使用互斥量
*@details  超时500ms
*@return  成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformTakeMutex(void *pp_handle)
{
    bool result = false;
    if (pp_handle == NULL)
    {
        return result;
    }

    for (uint8_t i = 0; i < UTE_MODULE_PLATFORM_MUTEX_MAX; i++)
    {
        if (&uteModulePlatformMutexBuff[i] == pp_handle)
        {
            if (uteModulePlatformMutexBuff[i].mutex != NULL)
            {
                os_err_t ret = os_mutex_take(uteModulePlatformMutexBuff[i].mutex, 500);
                if (ret == OS_EOK)
                {
                    result = true;
                }
                else
                {
                    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, take error, ret=%d", __func__, ret);
                }
            }
            break;
        }
    }
    return result;
}

/**
*@brief   释放互斥量
*@details
*@return  成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformGiveMutex(void *pp_handle)
{
    bool result = false;
    if (pp_handle == NULL)
    {
        return result;
    }

    for (uint8_t i = 0; i < UTE_MODULE_PLATFORM_MUTEX_MAX; i++)
    {
        if (&uteModulePlatformMutexBuff[i] == pp_handle)
        {
            if (uteModulePlatformMutexBuff[i].mutex != NULL)
            {
                os_err_t ret = os_mutex_release(uteModulePlatformMutexBuff[i].mutex);
                if (ret == OS_EOK)
                {
                    result = true;
                }
                else
                {
                    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, release error, ret=%d", __func__, ret);
                }
            }
            break;
        }
    }
    return result;
}

/**
* @brief   nor flash 读函数
*@param[in] uint8_t   *data 数据指针
*@param[in] uint32_t addr  读取地址
*@param[in] uint32_t len 读取长度
* @return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformFlashNorRead(uint8_t   *data, uint32_t addr, uint32_t len)
{
    os_spiflash_read( (void *)data,addr, len);
}
/**
* @brief   nor flash 写函数
*@param[in] uint8_t   *data 数据指针
*@param[in] uint32_t addr  写地址
*@param[in] uint32_t len 写长度
* @return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformFlashNorWrite(uint8_t *data, uint32_t addr, uint32_t len)
{
    if(addr<UTE_SYSTEM_USER_DATA_START_ADDRESS)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,err addr =0x%x", __func__,addr);
        return;
    }
    os_spiflash_program((void *)data,addr,len);
}
/**
*@brief   nor flash 擦除
*@details 擦除一个SECTOR，4K大小
*@param[in] uint32_t addr  擦除地址
* @return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformFlashNorErase(uint32_t addr)
{
    if(addr<UTE_SYSTEM_USER_DATA_START_ADDRESS)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,err addr =0x%x", __func__,addr);
        return;
    }
    os_spiflash_erase(addr);
}

#if UTE_LOG_SYSTEM_LVL
AT(.com_text.ute_msg.val)
char ute_msg_fail_str[]="%s,send msg fail,type=%d\n";
#endif

/**
* @brief   发送消息到app task
*@param[in] uint16_t type ，消息类型
*@param[in] uint32_t param ，消息参数，可以传递一个4byte的参数
* @return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
AT(.com_text.ute_msg)
void uteModulePlatformSendMsgToUteApplicationTask(uint16_t type, uint32_t param)
{
    ute_task_application_message_t p_msg;
    p_msg.type= type;
    p_msg.param = param;
    bool isSend = uteTaskApplicationSendMsg(&p_msg);
    if (!isSend)
    {
#if UTE_LOG_SYSTEM_LVL
        printf(ute_msg_fail_str,"uteModulePlatformSendMsgToUteApplicationTask",type);
#endif
    }
}
/**
*@brief   发送消息到app task
*@param[in] uint16_t type ，消息类型
*@param[in] uint32_t param ，消息参数，可以传递一个4byte的参数
*@return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
AT(.com_text.ute_msg)
void uteModulePlatformSendMsgToAppTask(uint16_t type, uint32_t param)
{
    ute_task_application_message_t p_msg;
    p_msg.type= type;
    p_msg.param = param;
    bool isSend = uteTaskApplicationSendMsg(&p_msg);
    if (!isSend)
    {
#if UTE_LOG_SYSTEM_LVL
        printf(ute_msg_fail_str,"uteModulePlatformSendMsgToAppTask",type);
#endif
    }
}
uint64_t uteModulePlatformSystemTickCnt = 0;
#if UTE_MODULE_CREATE_SYS_1S_TIMER_SUPPORT
void *uteModulePlatformRtcTimer=NULL;
void uteModulePlatformRtcTimerHandler(void *pxTimer)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_TIME_SEC_BASE, 0);
}
#endif
/**
*@brief RTC初始化函数
*@details RTC初始化函数，使用RTC，每秒产生一个消息，作为系统每秒的时间
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformRtcInit(void)
{
#if UTE_MODULE_CREATE_SYS_1S_TIMER_SUPPORT
    uteModulePlatformCreateTimer(&uteModulePlatformRtcTimer,"rtc",0,1000,true,uteModulePlatformRtcTimerHandler);
#endif
}
/**
*@brief RTC start函数
*@details RTC开始运行
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformRtcStart(void)
{
#if UTE_MODULE_CREATE_SYS_1S_TIMER_SUPPORT
    uteModulePlatformRestartTimer(&uteModulePlatformRtcTimer,1000);
#endif
}
/**
*@brief RTC 设置时间函数
*@details
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformRtcSetTime(uint16_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec)
{
    tm_t set;
    set.year = year;
    set.mon = month;
    set.day = day;
    set.hour = hour;
    set.min = min;
    set.sec = sec;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,%04d-%02d-%02d,%02d:%02d:%02d",__func__,set.year,set.mon,set.day,set.hour,set.min,set.sec);
    rtc_clock_set(set);
}
/**
*@brief RTC 校准时间函数
*@details
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformCalibrationSystemTimer(void)
{
#if UTE_LOG_TIME_LVL
    tm_t getRtcTime = rtc_clock_get();
    ute_module_systemtime_time_t getSystemTime;
    uteModuleSystemtimeGetTime(&getSystemTime);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL,"%s,rtc    %04d-%02d-%02d,%02d:%02d:%02d",__func__,getRtcTime.year,getRtcTime.mon,getRtcTime.day,getRtcTime.hour,getRtcTime.min,getRtcTime.sec);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL,"%s,system %04d-%02d-%02d,%02d:%02d:%02d",__func__,getSystemTime.year,getSystemTime.month,getSystemTime.day,getSystemTime.hour,getSystemTime.min,getSystemTime.sec);
#else
    return;
#endif
}
/**
*@brief 系统滴答计数
*@details 系统滴答计数，1/32000，基于系统RTC
*@return 返回计数值，最大为 0xFFFFFF，24位
*@author        zn.zeng
*@date        Jul 05, 2021
*/
uint32_t uteModulePlatformGetSystemTick(void)
{
    return uteModulePlatformSystemTickCnt;
}
#if !UTE_MODULE_USER_MALLOC_SUPPORT
/**
*@brief   动态申请内存
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void *uteModulePlatformMemoryAlloc(size_t size)
{
    if(size == 0)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,error size is 0", __func__);
        return NULL;
    }
    return ab_malloc(size);
}
/**
*@brief   释放动态申请的内存
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void uteModulePlatformMemoryFree(void * p)
{
    if (p != NULL)
    {
        ab_free(p);
    }
}
#endif
/**
*@brief   设置gpio输出
*@details
*@param[in] uint8_t pinNum
*@param[in] bool isHeight   输出高低电平
*@author        zn.zeng
*@date        2021-09-07
*/
__SCREEN_COMMON void uteModulePlatformOutputGpioSet(uint8_t pinNum, bool isHeight)
{
    if(pinNum==0)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,error pinNum is 0", __func__);
        return;
    }
    uint8_t gprop = (pinNum-1)/8;
    uint8_t pin = (pinNum-1)%8;
    if(isHeight)
    {
        if(gprop==0)
        {
            GPIOAFEN &= ~BIT(pin);
            GPIOADE  |= BIT(pin);
            GPIOADIR &= ~BIT(pin);
            GPIOASET = BIT(pin);
        }
        else if(gprop==1)
        {
            GPIOBFEN &= ~BIT(pin);
            GPIOBDE  |= BIT(pin);
            GPIOBDIR &= ~BIT(pin);
            GPIOBSET = BIT(pin);
        }
        else if(gprop==2)
        {
            GPIOEFEN &= ~BIT(pin);
            GPIOEDE  |= BIT(pin);
            GPIOEDIR &= ~BIT(pin);
            GPIOESET = BIT(pin);
        }
        else if(gprop==3)
        {
            GPIOFFEN &= ~BIT(pin);
            GPIOFDE  |= BIT(pin);
            GPIOFDIR &= ~BIT(pin);
            GPIOFSET = BIT(pin);
        }
        else if(gprop==5)
        {
            GPIOHFEN &= ~BIT(pin);
            GPIOHDE  |= BIT(pin);
            GPIOHDIR &= ~BIT(pin);
            GPIOHSET = BIT(pin);
        }

    }
    else
    {
        if(gprop==0)
        {
            GPIOAFEN &= ~BIT(pin);
            GPIOADE  |= BIT(pin);
            GPIOADIR &= ~BIT(pin);
            GPIOACLR = BIT(pin);
        }
        else if(gprop==1)
        {
            GPIOBFEN &= ~BIT(pin);
            GPIOBDE  |= BIT(pin);
            GPIOBDIR &= ~BIT(pin);
            GPIOBCLR = BIT(pin);
        }
        else if(gprop==2)
        {
            GPIOEFEN &= ~BIT(pin);
            GPIOEDE  |= BIT(pin);
            GPIOEDIR &= ~BIT(pin);
            GPIOECLR = BIT(pin);
        }
        else if(gprop==3)
        {
            GPIOFFEN &= ~BIT(pin);
            GPIOFDE  |= BIT(pin);
            GPIOFDIR &= ~BIT(pin);
            GPIOFCLR = BIT(pin);
        }
        else if(gprop==5)
        {
            GPIOHFEN &= ~BIT(pin);
            GPIOHDE  |= BIT(pin);
            GPIOHDIR &= ~BIT(pin);
            GPIOHCLR = BIT(pin);
        }
    }
}
#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
/**
*@brief   qspi 初始化
*@details  四通道
*@author         zn.zeng
*@date     2024-04-01
*/
void uteModulePlatformScreenQspiInit(void)
{
    //clk init
    CLKDIVCON2 = (CLKDIVCON2 & ~(BIT(4) * 0xF)) | BIT(4) * 2;   // TFTDE div
    CLKGAT2 |= BIT(4);
    RSTCON0 |= BIT(8);

    GPIOHFEN &= ~BIT(6);                        // CS
    GPIOHDE  |=  BIT(6);
    GPIOHSET  =  BIT(6);
    GPIOHDIR &= ~BIT(6);


    GPIOAFEN |=  BIT(4);                        // CLK
    GPIOADE  |=  BIT(4);
    GPIOACLR  =  BIT(4);
    GPIOADIR &= ~BIT(4);

    GPIOAFEN |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));  // D0/D1/D2/D3
    GPIOADE  |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));
    GPIOADIR |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));

    FUNCMCON2 = BIT(28);

    //te
    //TE port interrupt
    port_irq_register(UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_VECTORn, tft_te_isr);
    port_wakeup_init(UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_PIN,UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_TRIGGER, UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_PULL);               //开内部上拉, 下降沿唤醒

    //TICK Timer init
    CLKGAT0 |= BIT(28);
    delay_us(1);                                        //set CLKGAT0需要时间生效
    TICK0CON = BIT(6) | BIT(5) | BIT(2);                //div64[6:4], xosc26m[3:1]
    TICK0PR = 0xFFFF;
    TICK0CNT = 0;
    TICK0CON |= BIT(0);                                 //TICK EN

    TICK1CON = BIT(7) | BIT(6) | BIT(5) | BIT(2);       //TIE, div64[6:4], xosc26m[3:1]
    TICK1PR = (int)((XOSC_CLK_HZ / 1000) * TFT_TE_CYCLE_DELAY) / 64;
    TICK1CNT = 0;
    sys_irq_init(IRQ_TE_TICK_VECTOR, 0, tick_te_isr);

    static bool frist_init_flag = false;
    if (frist_init_flag == false) // 第一次初始化tft
    {
        tft_cb.te_mode = 0; // 初始化
        tft_cb.te_mode_next = 0;
        tft_set_temode(DEFAULT_TE_MODE);
    }
    else
    {
        // 不是第一次初始化tft
        tft_set_temode(tft_cb.te_mode); // 还原之前的TE
    }
    frist_init_flag = true;

    DESPICON = BIT(27) | BIT(9) | BIT(7) | BIT(3) | BIT(2) | BIT(0);                //[28:27]IN RGB565, [25]RGBW EN, [9]MultiBit, [7]IE, [3:2]1BIT, [0]EN

    sys_irq_init(IRQ_DESPI_VECTOR, 0, tft_spi_isr);
}
/**
*@brief   qspi 写命令
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
__SCREEN_COMMON void uteModulePlatformScreenQspiWriteCmd(uint8_t *buf, uint32_t len)
{
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
    uteModulePlatformDelayUs(1);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~BIT(3);                        //1BIT
    for(uint32_t i=0; i<len; i++)
    {
        tft_spi_sendbyte(buf[i]);
    }
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
}

/**
*@brief   qspi 读命令
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
__SCREEN_COMMON void uteModulePlatformScreenQspiReadCmd(uint8_t cmd,uint8_t *buf, uint32_t len)
{
    DESPIBAUD = 15;
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
    uteModulePlatformDelayUs(1);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~BIT(3);                        //1BIT
    tft_spi_sendbyte(0x03);
    tft_spi_sendbyte(0x00);
    tft_spi_sendbyte(cmd);
    tft_spi_sendbyte(0x00);

    for(uint32_t i=0; i<len; i++)
    {
        buf[i] = tft_spi_getbyte();
    }

    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
    DESPIBAUD = tft_cb.despi_baud;
}

/**
*@brief   Qspi 写gram数据
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
__SCREEN_COMMON void uteModulePlatformScreenQspiWriteGram(uint8_t cmd)
{
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
    uteModulePlatformDelayUs(1);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~BIT(3);                        //1BIT
    tft_spi_sendbyte(0x12);
    DESPICON |= BIT(3);                         //4BIT
    tft_spi_sendbyte(0x00);
    tft_spi_sendbyte(cmd);
    tft_spi_sendbyte(0x00);
}
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
/**
*@brief   dspi 初始化
*@details  双通道写
*@author         zn.zeng
*@date     2024-04-01
*/
void uteModulePlatformScreenDspiInit(void)
{
    //clk init
    CLKDIVCON2 = (CLKDIVCON2 & ~(BIT(4) * 0xF)) | BIT(4) * 2;   // TFTDE div
    CLKGAT2 |= BIT(4);
    RSTCON0 |= BIT(8);

    GPIOAFEN &= ~BIT(5);                        // CS
    GPIOADE  |=  BIT(5);
    GPIOASET  =  BIT(5);
    GPIOADIR &= ~BIT(5);

    GPIOAFEN |=  BIT(4);                        // CLK
    GPIOADE  |=  BIT(4);
    GPIOACLR  =  BIT(4);
    GPIOADIR &= ~BIT(4);

    // GPIOAFEN |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));  // D0/D1/D2/D3
    // GPIOADE  |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));
    // GPIOADIR |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));
    GPIOAFEN |= (BIT(2)|BIT(3));  // D0/D1
    GPIOADE  |= (BIT(2)|BIT(3));
    GPIOADIR |= (BIT(2)|BIT(3));

    FUNCMCON2 = BIT(28);

    //te
    //TE port interrupt
    port_irq_register(UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_VECTORn, tft_te_isr);
    port_wakeup_init(UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_PIN,UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_TRIGGER, UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_PULL);               //开内部上拉, 下降沿唤醒

    //TICK Timer init
    CLKGAT0 |= BIT(27);
    TICK0CON = BIT(6) | BIT(5) | BIT(2);                //div64[6:4], xosc26m[3:1]
    TICK0PR = 0xFFFF;
    TICK0CNT = 0;
    TICK0CON |= BIT(0);                                 //TICK EN

    TICK1CON = BIT(7) | BIT(6) | BIT(5) | BIT(2);       //TIE, div64[6:4], xosc26m[3:1]
    TICK1PR = (int)((XOSC_CLK_HZ / 1000) * TFT_TE_CYCLE_DELAY) / 64;
    TICK1CNT = 0;
    sys_irq_init(IRQ_TE_TICK_VECTOR, 0, tick_te_isr);

    tft_cb.te_mode = 0;                             //初始化
    tft_cb.te_mode_next = 0;

    tft_set_temode(DEFAULT_TE_MODE);

    sys_irq_init(IRQ_DESPI_VECTOR, 0, tft_spi_isr);

    DESPICON = BIT(27) | BIT(9) | BIT(7)| BIT(0);    //[28:27]IN RGB565, [9]MultiBit, [7]IE, [3:2]4BIT, [0]EN

    DESPIBAUD = tft_cb.despi_baud;

    tft_cb.tft_bglight_kick = true;

}
/**
*@brief   dspi 写命令
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformScreenDspiWriteCmd(uint8_t *buf, uint32_t len)
{
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
    uteModulePlatformDelayUs(2);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~(3<<2);
    DESPICON |= BIT(18);                        //1BIT 3wire control singnal
    DESPICON &= ~BIT(26);
    DESPICON &= ~BIT(19);                       // DC为 0
    DESPICON &= ~BIT(9);
    tft_spi_sendbyte(buf[0]);
    DESPICON |= BIT(19);
    for(uint32_t i=1; i<len; i++)
    {
        tft_spi_sendbyte(buf[i]);
    }
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
}
/**
*@brief   dspi 写gram数据
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformDspiWriteGram(uint8_t cmd)
{
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~(3<<2);
    DESPICON |= BIT(18);                        //9bit 1BIT 3wire control singnal
    DESPICON &= ~BIT(26);                       // 双通道关闭
    DESPICON &= ~BIT(19);                       // DC为 0
    tft_spi_sendbyte(cmd);

    DESPICON |= (2<<2);
    DESPICON |= BIT(18);
    DESPICON |= BIT(26);
    DESPICON |= BIT(19);                        // DC为 1
    DESPICON |= BIT(9);

}
/**
*@brief   dspi 读寄存器命令
*@details
*@author         zn.zeng
*@date     2024-04-09
*/
void uteModulePlatformScreenDspiReadCmd(uint8_t cmd,uint8_t *buf, uint32_t len,uint8_t dummyClockByte)
{
    DESPIBAUD = 32;
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~(3<<2);
    DESPICON |= BIT(18);                        //1BIT 3wire control singnal
    DESPICON &= ~BIT(26);
    DESPICON &= ~BIT(19);                       // DC为 0
    DESPICON &= ~BIT(9);
    tft_spi_sendbyte(cmd);

    DESPICON &= ~BIT(18);
    DESPICON |= BIT(2);
    for(uint32_t i=0; i<dummyClockByte; i++)
    {
        tft_spi_getbyte();
    }
    for(uint32_t i=0; i<len; i++)
    {
        buf[i] = tft_spi_getbyte();
    }
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
    DESPIBAUD = tft_cb.despi_baud;
}
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
void uteModulePlatformPwmInit(pwm_gpio id,uint8_t pinNum,uint8_t duty,uint32_t rateHz)
{
    bsp_pwm_freq_set(rateHz); /*! 使用多路pwm时，平台不支持设置频率，会影响其他路,wang.luo 2024-10-23 */
    bsp_pwm_duty_set(id,duty,false);
}
/**
*@brief   PWM 打开输出
*@details  不实现，init 就开始了
*@param[in] pwm_gpio id
*@author        zn.zeng
*@date        2021-10-12
*/
void uteModulePlatformPwmEnable(pwm_gpio id)
{

}
/**
*@brief   PWM 关闭输出
*@details
*@param[in] uint8_t id ,timer id ,0~7
*@author        zn.zeng
*@date        2021-10-12
*/
void uteModulePlatformPwmDisable(pwm_gpio id,uint8_t pinNum)
{
    bsp_pwm_disable(id);
}

/**
*@brief   获取BLE蓝牙地址
*@details
*@param[out] uint8_t *mac  6byte指针
* @return  void
*@author        zn.zeng
*@date        Jul 05, 2021
*/
void uteModulePlatformGetBleMacAddress(uint8_t *mac)
{
    uint8_t mac_addr[6];
    // get mac addr
    ble_get_local_bd_addr(mac_addr);
    memcpy(&mac[0], &mac_addr[0], 6);
}

/**
* @brief        修改蓝牙地址
* @details      修改后重启生效
* @param[in]    uint8_t *mac 6byte
* @return       ture 修改成功 false 修改失败
* @author       Wang.Luo
* @date         2022-09-30
*/
bool uteModulePlatformSetBleMacAddress(uint8_t *mac)
{
    memcpy(xcfg_cb.bt_addr, &mac[0], 6);
    return true;
}

/**
*@brief   更新ble广播数据
*@details 更新ble广播数据，输入不同广播类型和数据更新
*@param[in]
* @return  void
*@author        zn.zeng
*@date        Jul 05, 2021
*/
void uteModulePlatformAdvDataModifySub(bool isScanData, uint8_t advFlag, const uint8_t *newAdvData, uint8_t newAdvDataLen, ADV_MODIFY_TYPE type)
{
    int i = 0;
    uint8_t advData[31];
    uint8_t new_advdata[31] = {0x00};
    uint8_t new_advdata_index = 0;
    uint8_t content_len = 0; // 如0503F1F2F3F4.contnet_len =5;
    uint8_t current_len = 0;
    uint8_t new_data_len = 0;
    uint8_t adv_len = 0;
    bool valid_adv_flag = false;
    ADV_PARSE_STATE parse_state = ADV_LENGTH;

    new_data_len = newAdvDataLen;

    memset(advData, 0x00, 31);

    if (isScanData)
    {
        memcpy(&advData[0], &uteModulePlatformAdvData.scanData[0], 31);
    }
    else
    {
        memcpy(&advData[0], &uteModulePlatformAdvData.advData[0], 31);
    }

    // 遍历广播数据
    for (; i < 31; i++)
    {
        if (new_advdata_index > (31 - 1))
            break;
        switch (parse_state)
        {
            case ADV_LENGTH:
            {
                if (advData[i] == 0x00)
                {
                    goto MODIFY_ADVDATA_END;
                }
                parse_state = ADV_FLAG;
                new_advdata[new_advdata_index++] = advData[i];
                content_len = advData[i];
                current_len = 0;
                adv_len += (content_len + 1);
                if (new_advdata_index + content_len > 31)
                {
                    new_advdata[new_advdata_index - 1] = 31 - new_advdata_index;
                    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "2_Advertisement interception !!!");
                }
            }
            break;
            case ADV_FLAG:
            {
                parse_state = ADV_LENGTH;
                // 广播的Flag 与将要添加的新广播Flag相等时做对应的处理
                if (advData[i] == advFlag)
                {
                    valid_adv_flag = true;
                    parse_state = ADV_NEW_DATA;

                    if (type == ADV_APPEND) // 如果是附加广播要注意剩余的长度
                    {
                        if ((adv_len + new_data_len) > 31)
                        {
                            new_data_len = 31 - adv_len;
                        }
                        new_advdata[new_advdata_index - 1] = new_data_len + content_len;
                    }
                    else if (type == ADV_REPLACE) // 如果替换对应的广播注意原先的长度
                    {
                        if (((adv_len - (content_len - 1)) + new_data_len) > 31)
                        {
                            new_data_len = 31 - (adv_len - (content_len - 1));
                        }
                        new_advdata[new_advdata_index - 1] = new_data_len + 1;
                    }
                }
                else
                {
                    parse_state = ADV_DATA;
                }
                new_advdata[new_advdata_index++] = advData[i];
                current_len++;
            }
            break;
            case ADV_DATA:
            {
                new_advdata[new_advdata_index++] = advData[i];
                current_len++;
                if (current_len == content_len)
                {
                    parse_state = ADV_LENGTH;
                }
            }
            break;
            case ADV_NEW_DATA:
            {
                if (type == ADV_APPEND)
                {
                    new_advdata[new_advdata_index++] = advData[i];
                }
                else if (type == ADV_REPLACE)
                {
                }
                current_len++;
                if (current_len == content_len)
                {
                    for (uint8_t j = 0; j < new_data_len; j++)
                    {
                        if (new_advdata_index > (31 - 1))
                            break;
                        new_advdata[new_advdata_index++] = *newAdvData;
                        newAdvData++;
                    }
                    parse_state = ADV_LENGTH;
                }
            }
            break;
            default:
                break;
        }
    }
MODIFY_ADVDATA_END:
    if (valid_adv_flag != true)
    {
        if (new_advdata_index < 31 - 1)
        {
            if (new_advdata_index + new_data_len + 2 <= 31)
            {
                new_advdata[new_advdata_index++] = new_data_len + 1;
                new_advdata[new_advdata_index++] = advFlag;
                memcpy(&new_advdata[new_advdata_index], newAdvData, new_data_len);
                new_advdata_index += new_data_len;
            }
            else if (new_advdata_index + new_data_len + 2 > 31 && 31 > (new_advdata_index + 2))
            {
                new_data_len = 31 - new_advdata_index - 2;
                new_advdata[new_advdata_index++] = new_data_len + 1;
                new_advdata[new_advdata_index++] = advFlag;
                memcpy(&new_advdata[new_advdata_index], newAdvData, new_data_len);
                new_advdata_index += new_data_len;
                UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "1_Advertisement interception !!! ");
            }
        }
        else
        {
            new_advdata_index = 0;
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "Advertisement not enough space !!! ");
        }
    }
    if (new_advdata_index)
    {
        memcpy(advData, new_advdata, new_advdata_index);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,isScanData:%d,new_advdata_index:%d",__func__,isScanData,new_advdata_index);
        UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,advData,new_advdata_index);
        if (isScanData)
        {
            memcpy(&uteModulePlatformAdvData.scanData[0], advData, new_advdata_index);
            uteModulePlatformAdvData.scanLen = new_advdata_index;
        }
        else
        {
            memcpy(&uteModulePlatformAdvData.advData[0], advData, new_advdata_index);
            uteModulePlatformAdvData.advLen = new_advdata_index;
        }
    }
}

/**
*@brief   更新ble广播数据
*@details
*@return  void
*@author        zn.zeng
*@date        Jul 05, 2021
*/
void uteModulePlatformAdvDataUpdate(void)
{
    /*! adv data zn.zeng  modify Jul 05, 2021 */
    //uteModulePlatformAdvDataInit();

    ble_set_adv_data(uteModulePlatformAdvData.advData, 31);
    ble_set_scan_rsp_data(uteModulePlatformAdvData.scanData, 31);
}

/**
*@brief   获取广播剩余长度
*@details
*@return  void
*@author        zn.zeng
*@date        2021-12-08
*/
uint8_t uteModulePlatformGetAdvertismentRemainLength(void)
{
    uint8_t i = 31 - uteModulePlatformAdvData.advLen;
    return i;
}

uint8_t uteModulePlatformGetScanRspRemainLength(void)
{
    uint8_t i = 31 - uteModulePlatformAdvData.scanLen;
    return i;
}

/**
*@brief   更新蓝牙名
*@details
*@return  void
*@author        zn.zeng
*@date        2021-08-18
*/
static uint8_t devName[32];
static uint8_t devNameSize=0;
static uint8_t devCompleteNameSize=0; // 带设备ID
void uteModulePlatformUpdateDevName(void)
{
    uint8_t name[32] ;
    memset(name, 0, 32);
    memset(devName, 0, 32);
    devNameSize = 0;
    uint8_t size=0;
#if UTE_PC_TOOL_WIRTE_BT_NAME_SUPPORT
    uint16_t snDataLen = sizeof(ute_application_sn_data_t);
    ute_application_sn_data_t *snData = uteModulePlatformMemoryAlloc(snDataLen);
    memset(snData, 0, snDataLen);
    uteModulePlatformFlashNorRead(snData, UTE_USER_PARAM_ADDRESS, snDataLen);
    if (snData->bleDevNameLen > 0 && snData->bleDevNameLen <= sizeof(devName))
    {
        size = snData->bleDevNameLen;
    }
#endif
    if (size == 0)
    {
#if UTE_PC_TOOL_WIRTE_BT_NAME_SUPPORT
        size = strlen(xcfg_cb.le_name);
        if (size > 0)
        {
            memcpy(&name[0], xcfg_cb.le_name, size);
            memset(snData->bleDevName, 0, sizeof(snData->bleDevName));
            memcpy(snData->bleDevName, xcfg_cb.le_name, size);
            snData->bleDevNameLen = size;
            uteModulePlatformFlashNorErase(UTE_USER_PARAM_ADDRESS);
            uteModulePlatformFlashNorWrite(snData, UTE_USER_PARAM_ADDRESS, sizeof(ute_application_sn_data_t));
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,DevName from xcfg_cb, size=%d", __func__,size);
        }
        else
#endif
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,size is error", __func__);
            memcpy(&name[0], DEFAULT_BLE_DEV_NAME, strlen(DEFAULT_BLE_DEV_NAME));
            size = strlen(DEFAULT_BLE_DEV_NAME);
        }
    }
#if UTE_PC_TOOL_WIRTE_BT_NAME_SUPPORT
    else
    {
        memcpy(&name[0], snData->bleDevName, size);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,DevName from flash, size=%d", __func__,size);
    }
    uteModulePlatformMemoryFree(snData);
#endif
    devNameSize = size;
#if UTE_APP_DISPLAY_NAME_ID_SUPPORT
    {
        uint8_t device_MAC_address_buf[6];
        uint8_t BleId[12];
        uint8_t len = 0;
        int remainLength = uteModulePlatformGetAdvertismentRemainLength();
        // int remainLength = uteModulePlatformGetScanRspRemainLength();
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,remainLength=%d", __func__,remainLength);
        uteModulePlatformGetBleMacAddress(&device_MAC_address_buf[0]);
        for(int i=0; i<6; i++)
        {
            BleId[2*i] = (device_MAC_address_buf[i]/16)+ (((device_MAC_address_buf[i]/16)>9)?(-10 +'A'):(+'0'));
            BleId[2*i+1] = (device_MAC_address_buf[i]%16)+ (((device_MAC_address_buf[i]%16)>9)?(-10 +'A'):(+'0'));
        }
        if((size+9+2)<=(remainLength))   //total char "(namelen+0x09)+...+(ID-XXXX)" 12byte
        {

            memcpy(&BleId[4],"(ID-",4);
            len = 9;
            memcpy(&name[size],&BleId[4],len-1);
            memcpy(&name[size+len-1],")",1);
            size = size + len;
        }
        else if((size+5+2)<=(remainLength)) //total char "(namelen+0x07)+...+-XXXX" 7byte
        {
            BleId[7]='-';
            memcpy(&name[size],&BleId[7],5);
            size = size + 5;
        }
        else if((size+4+2)<=(remainLength))
        {
            BleId[7]='-';
            memcpy(&name[size],&BleId[7],4);
            size = size + 4;
        }
        else if((size+2)<=(remainLength))
        {

        }
        else
        {
            if(remainLength>2)
            {
                size = remainLength-2;
            }
        }
    }
#endif
    name[size] = 0;
    memcpy(devName,&name[0],size);
    devCompleteNameSize = size;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,name =%s,size=%d", __func__, name,size);
    uteModulePlatformAdvDataModifySub(false, 0x09, name, size, ADV_REPLACE);
    ble_set_gap_name((char *)name,size);
#if UTE_BT30_CALL_SUPPORT
#if UTE_MODULE_BT_ONCE_PAIR_CONNECT_SUPPORT
    uteModuleCallSetBtDevName(name,size);
#endif
#endif
}

/**
*@brief   获取蓝牙名
*@details
*@param[out] uint8_t *name
*@param[in] uint8_t *size，输入最大buff大小，输出实际大小
*@return  void
*@author        zn.zeng
*@date        2022-01-20
*/
void uteModulePlatformGetDevName(uint8_t *name,uint8_t *size)
{
    uint8_t buffSize = *size;
    memset(name, 0, buffSize);
    if(buffSize>=devNameSize)
    {
        *size = devNameSize;
    }
    memcpy(&name[0],devName,*size);
}

/**
 * @brief        获取完整蓝牙名
 * @details      蓝牙名称带设备ID
 * @param[out]   uint8_t *name
 * @param[in]    uint8_t *size，输入最大buff大小，输出实际大小
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-16
 */
void uteModulePlatformGetDevCompleteName(uint8_t *name,uint8_t *size)
{
    uint8_t buffSize = *size;
    memset(name, 0, buffSize);
    if(buffSize>=devCompleteNameSize)
    {
        *size = devCompleteNameSize;
    }
    memcpy(&name[0],devName,*size);
}

uint8_t uteModulePlatformGetAdvData(uint8_t *advData, uint8_t advDataLen)
{
    if(advData && advDataLen)
    {
        memset(advData, 0, advDataLen);
        memcpy(advData, &uteModulePlatformAdvData.advData[0], advDataLen > 31 ? 31 : advDataLen);
        return uteModulePlatformAdvData.advLen;
    }
    return 0;
}

uint8_t uteModulePlatformGetScanRspData(uint8_t *scanRsp, uint8_t scanRspLen)
{
    if(scanRsp && scanRspLen)
    {
        memset(scanRsp, 0, scanRspLen);
        memcpy(scanRsp, &uteModulePlatformAdvData.scanData[0], scanRspLen > 31 ? 31 : scanRspLen);
        return uteModulePlatformAdvData.scanLen;
    }
    return 0;
}

void uteModulePlatformAdvDataInit(void)
{
    memset(&uteModulePlatformAdvData, 0, sizeof(uteModulePlatformAdvData));

    uteModulePlatformAdvDataModifySub(false, 0x01, (const uint8_t *)"\x06", 1, ADV_APPEND);
    uteModulePlatformAdvDataModifySub(false, 0x03, (const uint8_t *)UTE_ADVERTISE_DATA, strlen(UTE_ADVERTISE_DATA), ADV_APPEND);
    uteModulePlatformAdvDataModifySub(false, 0x03, (const uint8_t *)UTE_APP_DISPLAY_BT_NAME_ADVERTISE_DATA, strlen(UTE_APP_DISPLAY_BT_NAME_ADVERTISE_DATA), ADV_APPEND);

    /*! scan rsp data  zn.zeng  modify Jul 05, 2021 */
    uint8_t mac[6];
    uteModulePlatformGetBleMacAddress(mac);
    uteModulePlatformAdvDataModifySub(true, 0xff, (const uint8_t *)"\x00\x00", 2, ADV_APPEND);
    uteModulePlatformAdvDataModifySub(true, 0xff, mac, 6, ADV_APPEND);

    uteModulePlatformUpdateDevName();
}

/**
*@brief   使能休眠
*@details
*@param[in] uint32_t bit ,功能位
*@author        zn.zeng
*@date        2021-10-16
*/
//AT(.sleep_text.ute_sleep_ctrl)
void uteModulePlatformDlpsEnable(uint32_t bit)
{
#if UTE_LOG_SYSTEM_LVL
    uint32_t lastDlpsBit = uteModulePlatformDlpsBit;
#endif
    uteModulePlatformDlpsBit &= ~bit;
    if (uteModulePlatformDlpsBit  == 0)
    {
        sys_cb.sleep_delay = 0;
        // sys_cb.guioff_delay = 0;
    }
#if UTE_LOG_SYSTEM_LVL
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,bit:%04x %04x --> %04x",__func__,bit,lastDlpsBit,uteModulePlatformDlpsBit);
#endif
}
/**
*@brief   关闭休眠
*@details
*@param[in] uint32_t bit ,功能位
*@author        zn.zeng
*@date        2021-10-16
*/
//AT(.sleep_text.ute_sleep_ctrl)
void uteModulePlatformDlpsDisable(uint32_t bit)
{
#if UTE_LOG_SYSTEM_LVL
    uint32_t lastDlpsBit = uteModulePlatformDlpsBit;
#endif
    uteModulePlatformDlpsBit |= bit;
#if UTE_LOG_SYSTEM_LVL
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,bit:%04x %04x --> %04x",__func__,bit,lastDlpsBit,uteModulePlatformDlpsBit);
#endif
}

/**
 * @brief        是否允许休眠
 * @details
 * @return       true:允许休眠，false:不允许休眠
 * @author       Wang.Luo
 * @date         2024-10-23
 */
//AT(.sleep_text.ute_sleep_ctrl)
uint32_t uteModulePlatformNotAllowSleep(void)
{
    return uteModulePlatformDlpsBit;
}

/**
 * @brief        重置休眠位
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-10-23
 */
//AT(.sleep_text.ute_sleep_ctrl)
void uteModulePlatformDlpsBitReset(void)
{
    uteModulePlatformDlpsBit = 0;
}

/**
*@brief   BLE拒接来电
*@details
*@author  casen
*@date    2021-12-31
*/
void uteModulePlatformRejectIncall(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,quick.isRejectCall=%d",__func__,quick.isRejectCall);
#if !UTE_BT30_CALL_CUST_NO_NEED_SUPPORT /* 解决关闭BT功能后 无法挂断电话问题 guoguo 2024-08-14 */
    if (quick.isRejectCall)
#endif
    {
        uint8_t hangUpCmd[2];
        hangUpCmd[0] = CMD_SEND_KEYCODE;
        hangUpCmd[1] = 0x02;
        uteModuleProfileBleSendToPhone(&hangUpCmd[0], 2);
        /*! Ancs Reject Incall*/
        if (ble_ancs_is_connected())
        {
            ble_ancs_remote_action(ble_cb.ansc_uid, true);
        }
    }
    if(uteDrvMotorGetRunningStatus())
    {
        uteDrvMotorStop();
    }
}