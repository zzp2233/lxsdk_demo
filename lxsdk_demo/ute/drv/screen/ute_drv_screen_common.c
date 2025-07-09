/**
*@file
*@brief     屏驱动文件
*@details    屏驱动公共层文件
*@author        zn.zeng
*@date        2021-09-06
*@version       v1.0
*/
#include "ute_drv_screen_common.h"
#include "ute_module_log.h"
#include "gui.h"


#if DRV_TFT_S280X284_I18_ST7789P3_XHL183BA2401_DSPI_SUPPORT
#include "ute_drv_tft_s240x284_i18_st7789p3_xhl183ba2401.h"
#elif UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_SUPPORT
#include "ute_drv_tft_s240x284_nv3030b_hy201068avc.h"
#elif DRV_BOE_S240X284_I183_JD9853_WV018LZQ_QSPI_SUPPORT
#include "ute_drv_boe_s240x284_i183_jd9853_wv018lzq_qspi.h"
#elif DRV_BOE_S240X296_I183_JD9853_WV018LZQ_QSPI_SUPPORT
#include "ute_drv_boe_s240x296_i183_jd9853_wv018lzq_qspi.h"
#elif UTE_DRV_TFT_S240X284_NV3030B_ZD183G1196_QSPI_SUPPORT
#include "ute_drv_tft_s240x284_nv3030b_zd183g1196.h"
#elif UTE_DRV_TFT_S360X360_NV3030B_HY139071A_QSPI_SUPPORT
#include "ute_drv_tft_s360X360_nv3030b_hy139071a.h"
#elif UTE_DRV_TFT_S240X296NV3030B_ZD183G1196_QSPI_SUPPORT
#include "ute_drv_boe_s240x296_i183_jd9853_wv018lzq_qspi.h"
#elif UTE_DRV_TFT_S360X360_NV3030B_ZD138G1616_QSPI_SUPPORT
#include "ute_drv_tft_s360X360_nv3030b_zd138g1616.h"
#endif

/*! 配置屏的接口zn.zeng, 2021-09-06  */
const ute_drv_screen_common_config_t *uteDrvScreenCommonFunction=NULL;
ute_drv_screen_common_data_t uteDrvScreenCommonData;

/**
*@brief   屏gpio初始化函数
*@details
*@author        zn.zeng
*@date        2021-09-06
*/
void uteDrvScreenCommonGpioInit(void)
{

}
/**
*@brief   屏接口初始化函数
*@details
*@author        zn.zeng
*@date        2022-05-05
*/
void uteDrvScreenCommonInterfaceInit(void)
{
#if DRV_TFT_S280X284_I18_ST7789P3_XHL183BA2401_DSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft280X284St7789p3Xhl183ba2401Config;
#elif UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X284Nv3030BHy201068AvcConfig;
#elif DRV_BOE_S240X284_I183_JD9853_WV018LZQ_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenBoe240X284Wv018lzqConfig;
#elif DRV_BOE_S240X296_I183_JD9853_WV018LZQ_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenBoe240X296Wv018lzqConfig;
#elif UTE_DRV_TFT_S240X296NV3030B_ZD183G1196_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenBoe240X296Wv018lzqConfig;
#elif UTE_DRV_TFT_S240X284_NV3030B_ZD183G1196_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X284Nv3030BZd183g1196Config;
#elif UTE_DRV_TFT_S360X360_NV3030B_HY139071A_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft360X360Nv3030BHy139071aConfig;
#elif UTE_DRV_TFT_S360X360_NV3030B_ZD138G1616_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft360X360Nv3030BZd138g1616Config;
#endif
#if UTE_DRV_8080_FOR_SCREEN_SUPPORT
    uteModulePlatform8080Init();
#elif UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
    uteModulePlatformScreenQspiInit();
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
    uteModulePlatformScreenDspiInit();
#endif
    uteDrvScreenCommonGpioInit();
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,%s", __func__,uteDrvScreenCommonFunction->name);
}

/**
 * @brief     屏幕通用初始化函数（统一入口）
 * @details   负责初始化屏幕通信接口、执行屏幕型号识别、调用屏幕驱动初始化函数，
 *            并根据配置是否清屏、是否点亮背光，是 BOE/NV3030B 等屏幕驱动的框架核心。
 * @author    zn.zeng
 * @date      2021-09-06
 */
void uteDrvScreenCommonInit(void)
{
    uteDrvScreenCommonInterfaceInit();                                        // 初始化屏幕底层接口，如QSPI/SPI接口、IO引脚（CS, DC, RST）

#if UTE_LOG_DRV_SCREEN_LVL
    uteDrvScreenCommonReadId();                                              // 如果开启日志等级，读取屏幕ID（用于调试或识别型号）
#else
    DESPIBAUD = tft_cb.despi_baud;                                           // 否则直接设置QSPI通信波特率（用tft_cb结构体保存配置）
#endif

    if(uteDrvScreenCommonFunction->init)                                     // 若已注册具体屏幕的 init 函数指针（例如 drvScreenBoeWv018lzqInit）
    {
        uteDrvScreenCommonFunction->init();                                  // 调用屏幕专属初始化函数（通常设置寄存器、gamma等）
    }

#if UTE_DRV_CLEAR_SCREEN_BEFORE_BACKLIGHT_ON_SUPPORT
    uteDrvScreenCommonClearWindow(0, 0, UTE_DRV_SCREEN_WIDTH, 
                                  UTE_DRV_SCREEN_HEIGHT, 0);                 // 清除整屏内容为颜色值0（黑屏），避免亮屏时花屏
#endif

    if(uteDrvScreenCommonFunction->powerOn)                                  // 如果注册了 powerOn 函数
    {
        uteDrvScreenCommonFunction->powerOn();                               // 调用屏幕上电函数（如发送0x11和0x29命令）
    }

    uteDrvScreenCommonData.isInit = true;                                    // 设置屏幕初始化完成标志位
    uteDrvScreenCommonData.currBackLightPercent = 0;                         // 初始化背光亮度为0%，待后续调用setBackLight设置亮度
}

/**
*@brief   设置屏reset 脚电平
*@details
*@author        zn.zeng
*@date        2021-09-06
*/
__SCREEN_COMMON void uteDrvScreenCommonSetResetPin(bool isHeight)
{
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_RST_GPIO_PIN,isHeight);
}
/**
*@brief   设置屏电源电平
*@details
*@author        zn.zeng
*@date        2021-09-06
*/
void uteDrvScreenCommonSetPowerEnable(bool isEnable)
{
#if UTE_DRV_SCREEN_POWER_CTRL_SUPPORT
    if (UTE_DRV_SCREEN_ENABLE_ACTIVE_LEVEL)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_POWER_GPIO_PIN, isEnable);
    }
    else
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_POWER_GPIO_PIN, !isEnable);
    }
#endif
#ifdef UTE_DRV_SCREEN_VCI_EN_GPIO_PIN
    if (UTE_DRV_SCREEN_VCI_EN_ACTIVE_LEVEL)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_VCI_EN_GPIO_PIN, isEnable);
    }
    else
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_VCI_EN_GPIO_PIN, !isEnable);
    }
#endif
#ifdef UTE_DRV_SCREEN_IM1_GPIO_PIN
    if (UTE_DRV_SCREEN_IM1_SELECT)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_IM1_GPIO_PIN, isEnable);
    }
    else
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_IM1_GPIO_PIN, !isEnable);
    }
#endif
#ifdef UTE_DRV_SCREEN_IM0_GPIO_PIN
    if (UTE_DRV_SCREEN_IM0_SELECT)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_IM0_GPIO_PIN, isEnable);
    }
    else
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_IM0_GPIO_PIN, !isEnable);
    }
#endif
    if (!isEnable)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_RST_GPIO_PIN, false);
    }
}
/**
*@brief   设置显示区域
*@details
*@author        zn.zeng
*@date        2021-09-07
*/
__SCREEN_COMMON void uteDrvScreenCommonSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    uteDrvScreenCommonFunction->setWindow(xStart,yStart,xEnd-1,yEnd-1);
}
/**
*@brief   清屏函数
*@details
*@author        zn.zeng
*@date        2021-09-07
*/
void uteDrvScreenCommonClearWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd,uint16_t color)
{
    if(uteDrvScreenCommonFunction->setWindow)
    {
        uteDrvScreenCommonFunction->setWindow(xStart,yStart,xEnd,yEnd);
    }
    gui_clear_screen(color,(yEnd+1-yStart)*(xEnd+1-xStart));
}
#if UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_SUPPORT
/**
*@brief   Sgm3132yde8g背光
*@details   16级，0~15
*@param[in] uint8_t percent   0~100% ，为0时关闭背光
*@author        zn.zeng
*@date        2021-09-08
*/
void uteDrvScreenCommonSgm3132yde8g(uint8_t percent)
{
    // There are totally 16 steps of current that could be set by users.
    uint8_t brightLevel = 0;
    percent = (percent > 100) ? 100 : percent;
    brightLevel = 16 - (uint8_t)((percent/100.0f) * 16)+(16-UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_MAX_LVL);
    if(brightLevel>=(UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_MAX_LVL-1))
    {
        brightLevel = (UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_MAX_LVL-1);
    }
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,false);
    uteModulePlatformDelayUs(5000);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,true);
    uteModulePlatformDelayUs(50);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,false);
    uteModulePlatformDelayUs(10);
    while(brightLevel)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,true);
        uteModulePlatformDelayUs(10);
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,false);
        uteModulePlatformDelayUs(5);
        brightLevel--;
    }
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,true);
}
#endif
/**
*@brief   打开关闭背光
*@details
*@param[in] uint8_t percent   0~100% ，为0时关闭背光
*@author        zn.zeng
*@date        2021-09-08
*/
void uteDrvScreenCommonOpenBacklight(uint8_t percent)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,percent=%d,currBackLightPercent=%d", __func__,percent,uteDrvScreenCommonData.currBackLightPercent);
    if(percent!=0)
    {
        if(percent!=uteDrvScreenCommonData.currBackLightPercent)
        {
#if (UTE_DRV_SCREEN_BACKLIGHT_PWM_SUPPORT||UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_SUPPORT)
            uteModulePlatformDelayMs(30);
#endif
            if(uteDrvScreenCommonFunction->setBackLight!=NULL)
            {
                uteDrvScreenCommonFunction->setBackLight(percent);
            }
            else
            {
#if UTE_DRV_SCREEN_BACKLIGHT_PWM_SUPPORT
                uint8_t pwm_percent = percent;
#ifdef SCREEN_BACK_LIGHT_INCREASE_PERCENT
                pwm_percent = (uint8_t)(pwm_percent*SCREEN_BACK_LIGHT_INCREASE_PERCENT);
#endif
                uteModulePlatformPwmInit(UTE_DRV_SCREEN_BACKLIGHT_PWM_ID,UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,pwm_percent,UTE_DRV_SCREEN_BACKLIGHT_PWM_HZ);
                uteModulePlatformPwmEnable(UTE_DRV_SCREEN_BACKLIGHT_PWM_ID);
#elif UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_SUPPORT
                uteDrvScreenCommonSgm3132yde8g(percent);
#endif
            }
        }
    }
    else
    {
#if UTE_DRV_SCREEN_BACKLIGHT_PWM_SUPPORT
        uteModulePlatformPwmDisable(UTE_DRV_SCREEN_BACKLIGHT_PWM_ID,UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN);
#elif UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_SUPPORT
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,false);
#endif
    }
    uteDrvScreenCommonData.currBackLightPercent = percent;
}
/**
*@brief   关闭显示
*@details
*@author        zn.zeng
*@date        2021-09-24
*/

void uteDrvScreenCommonDisplayOff(void)
{
    uteDrvScreenCommonOpenBacklight(0);
    uteDrvScreenCommonFunction->powerOff();
    uteDrvScreenCommonData.isInit = false;
}

__SCREEN_COMMON void uteModulePlatformScreenWriteDataStart(void)
{
#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
    uteModulePlatformScreenQspiWriteGram(0x2c);
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
    uteModulePlatformDspiWriteGram(0x2c);
#endif
}

__SCREEN_COMMON uint32_t uteDrvScreenCommonReadId(void)
{
    uint32_t id = 0;
#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
    id = uteDrvScreenCommonGc9c01QspiReadId();
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT

#endif
    id = ((id & 0xFF000000) >> 24) |
         ((id & 0x00FF0000) >> 8) |
         ((id & 0x0000FF00) << 8) |
         ((id & 0x000000FF) << 24);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,id=0x%x", __func__,id);
    return id;
}

#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT

/**
*@brief   gc9c01 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
__SCREEN_COMMON void uteDrvScreenCommonGc9c01QspiWriteCmdParams(uint8_t cmd,uint8_t *data,uint8_t size)
{
    static uint8_t sdat[36];// = {0x02, 0x00, cmd, 0x00,};
    sdat[0] = 0x02;
    sdat[1] = 0x00;
    sdat[2] = cmd;
    sdat[3] = 0x00;
    if(size>32)
    {
        return;
    }
    if(size>0)
    {
        memcpy(&sdat[4],data,size);
    }
    uteModulePlatformScreenQspiWriteCmd(sdat, size+4);
}

/**
*@brief   gc9c01 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
__SCREEN_COMMON void uteDrvScreenCommonGc9c01QspiWriteCmdParam(uint8_t cmd,uint8_t data)
{
    uint8_t sdata[2] = {0,0};
    sdata[0] = data;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(cmd,&sdata[0],1);
}
/**
*@brief   ft2308 qspi write
*@details  16bit cmd
*@author        zn.zeng
*@date        2022-04-19
*/
__SCREEN_COMMON void uteDrvScreenCommonFt2308QspiWrite16bitCmdParams(uint16_t cmd,uint8_t *data,uint8_t size)
{
    static uint8_t sdat[36];// = {0x02, 0x00, cmd, 0x00,};
    sdat[0] = 0x02;
    sdat[1] = 0x00;
    sdat[2] = cmd>>8&0xff;
    sdat[3] = cmd&0xff;
    if(size>32)
    {
        return;
    }
    if(size>0)
    {
        memcpy(&sdat[4],data,size);
    }
    uteModulePlatformScreenQspiWriteCmd(sdat, size+4);
}
/**
*@brief   ft2308 qspi write
*@details  8bit cmd
*@author        zn.zeng
*@date        2022-04-19
*/
__SCREEN_COMMON void uteDrvScreenCommonFt2308QspiWrite8bitCmdParams(uint8_t cmd,uint8_t *data,uint8_t size)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(cmd,data,size);
}
/**
*@brief   NV3030B qspi write
*@details  8bit cmd
*@author        zn.zeng
*@date        2022-04-19
*/
__SCREEN_COMMON void uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(uint8_t cmd,uint8_t *data,uint8_t size)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(cmd,data,size);
}

__SCREEN_COMMON uint32_t uteDrvScreenCommonGc9c01QspiReadId(void)
{
    uint32_t id = 0;
    uteModulePlatformScreenQspiReadCmd(0x04,(uint8_t *)&id,4);
    return id;
}

#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
/**
*@brief   st7789p3 dspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
void uteDrvScreenCommonSt7789p3DspiWriteCmdParams(uint8_t cmd,uint8_t *data,uint8_t size)
{
    uint8_t sdat[36];
    sdat[0] = cmd;
    if(size>32)
    {
        return;
    }
    if(size>0)
    {
        memcpy(&sdat[1],data,size);
    }
    uteModulePlatformScreenDspiWriteCmd(sdat, size+1);
}
/**
*@brief   st7789p3 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
void uteDrvScreenCommonSt7789p3DspiWriteCmdParam(uint8_t cmd,uint8_t data)
{
    uint8_t sdata[2] = {0,0};
    sdata[0] = data;
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(cmd,&sdata[0],1);
}
#endif
/*! ESD测试屏幕TE中断异常复位处理,wang.luo 2023-03-21 */
#if UTE_DRV_SCREEN_ESD_TE_INT_ERROR_RESET_SUPPORT
void *screenEsdTeIntErrorCheckTimerPointer = NULL;
uint8_t  EsdTeIntErrorCheckCnt=0;
/**
*@brief        延时初始化处理函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckHandlerMsg(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,", __func__);
    if (uteApplicationCommonIsPowerOn())
    {
        if (uteModuleGuiCommonIsDisplayOn())
        {
            EsdTeIntErrorCheckCnt++;
            /*! 灭屏*/
            uteModuleGuiCommonDisplayOff(true);
            /*! 关闭屏幕电源*/
            uteDrvScreenCommonSetPowerEnable(false);
            uteModulePlatformDelayUs(50 * 1000);
            /*! 亮屏*/
            ute_task_gui_message_t msg;
            msg.type = MSG_TYPE_HNAD_SCREEN_ON_NOTIFY;
            uteTaskGuiSendMsg(&msg);
        }
        else
        {
            uteDrvScreenEsdTeIntErrorCheckTimerStop();
        }
    }
    else
    {
        uteDrvScreenEsdTeIntErrorCheckTimerStop();
    }
}
/**
*@brief        延时初始化回调函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckTimerCallback(void *pxTimer)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SCREEN_ESD_TE_INT_ERROR_RESET,0);
}
/**
*@brief        延时初始化定时器创建函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckTimerCreate(void)
{
    if(screenEsdTeIntErrorCheckTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&screenEsdTeIntErrorCheckTimerPointer, "spd2010ErrorCheckTimer",1, 1000, false, uteDrvScreenEsdTeIntErrorCheckTimerCallback);
    }
}
/**
*@brief   启动初始化定时器创建函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckTimerStart(uint16_t ms)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,%d ms", __func__,ms);
    uteDrvScreenEsdTeIntErrorCheckTimerCreate();
    uteModulePlatformRestartTimer(&screenEsdTeIntErrorCheckTimerPointer, ms);
}
/**
*@brief   停止初始化定时器创建函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckTimerStop(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
    uteModulePlatformStopTimer(&screenEsdTeIntErrorCheckTimerPointer);
    uteModulePlatformDeleteTimer(&screenEsdTeIntErrorCheckTimerPointer);
    screenEsdTeIntErrorCheckTimerPointer = NULL;
}
#endif

