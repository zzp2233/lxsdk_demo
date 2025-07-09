#ifndef _UTE_DRV_BOE_S240X284_I183_WV8LZQ_QSPI_H_
#define _UTE_DRV_BOE_S240X284_I183_WV8LZQ_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if DRV_BOE_S240X296_I183_JD9853_WV018LZQ_QSPI_SUPPORT

/**
 * @brief     打开 BOE WV018LZQ 屏幕电源的初始化流程（支持NV3030B驱动芯片）
 * @note      此函数向屏幕发送开机指令序列，适用于使用 QSPI 接口通信的屏幕驱动。
 * @retval    无
 */
__STATIC_INLINE void drvScreenBoeWv018lzqPowerOn(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x11, NULL, 0);      // 发送 Sleep Out 命令，唤醒屏幕（0x11为常见唤醒命令）
    uteModulePlatformDelayUs(120000);                                    // 延时 120ms，确保唤醒完成（根据 BOE 屏数据手册要求）

    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x29, NULL, 0);      // 发送 Display ON 命令，打开显示（常用于唤醒后点亮屏幕）
    uteModulePlatformDelayUs(50000);                                     // 延时 50ms，等待显示稳定
    // UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);           // 日志打印当前函数名称（调试用）
}

/**
 * @brief     关闭 BOE WV018LZQ 屏幕显示并进入休眠状态
 * @note      此函数用于关闭屏幕显示并进入低功耗状态（支持NV3030B驱动芯片）
 * @retval    无
 */
__STATIC_INLINE void drvScreenBoeWv018lzqPowerOff(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x28, NULL, 0);      // 发送 Display OFF 命令，关闭屏幕显示内容
    uteModulePlatformDelayUs(20000);                                     // 延时 20ms，等待关屏生效

    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x10, NULL, 0);      // 发送 Sleep In 命令，使屏幕进入休眠状态，节省功耗
    uteModulePlatformDelayUs(50000);                                     // 延时 50ms，等待屏幕进入睡眠模式
    // UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);           // 日志打印当前函数名称（调试用）
}


/**
 * @brief     设置 BOE WV018LZQ 屏幕的显示窗口（裁剪区域）
 * @note      此函数通过 QSPI 接口发送 CASET（0x2A）和 RASET（0x2B）指令来设置绘图区域
 * @param     xStart      起始 X 坐标（列）
 * @param     yStart      起始 Y 坐标（行）
 * @param     xEnd        结束 X 坐标（列）
 * @param     yEnd        结束 Y 坐标（行）
 * @retval    无
 */
__STATIC_INLINE void drvScreenBoeWv018lzqSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    uint8_t data[4];                                                       // 临时缓冲数组，用于存放4字节坐标参数（高8位 + 低8位）

    data[0] = xStart >> 8;                                                 // data[0] = 起始X坐标高8位
    data[1] = xStart & 0xff;                                               // data[1] = 起始X坐标低8位
    data[2] = xEnd   >> 8;                                                 // data[2] = 结束X坐标高8位
    data[3] = xEnd   & 0xff;                                               // data[3] = 结束X坐标低8位
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2A, data, 4);        // 发送 0x2A 指令（Column Address Set，设置列坐标）

    // yStart += 20;                                                       // 可选偏移调整（部分屏幕需要调整GRAM起始行）
    // yEnd   += 20;                                                       // 若取消注释表示绘图区域整体下移20行

    data[0] = yStart >> 8;                                                 // data[0] = 起始Y坐标高8位
    data[1] = yStart & 0xff;                                               // data[1] = 起始Y坐标低8位
    data[2] = yEnd   >> 8;                                                 // data[2] = 结束Y坐标高8位
    data[3] = yEnd   & 0xff;                                               // data[3] = 结束Y坐标低8位
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2B, data, 4);        // 发送 0x2B 指令（Row Address Set，设置行坐标）

    // uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);   // 可选：控制CS脚状态，部分平台需手动拉低片选
}

__STATIC_INLINE void drvScreenBoeWv018lzqInit(void)
{
    uint8_t tmp[34];
    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(20000);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayUs(20000);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(20000);
//    IF8080_SwitchMode(IF8080_MODE_MANUAL);
    memcpy(&tmp[0], "\x98\x53", 2);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xdf, &tmp[0], 2);
    memcpy(&tmp[0], "\x00", 1);//06
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xde, &tmp[0], 1);
    memcpy(&tmp[0], "\x23", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xb2, &tmp[0], 1);
    memcpy(&tmp[0], "\x00\x2d\x00\x55", 4);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xb7, &tmp[0], 4);
    memcpy(&tmp[0], "\x1c\x1a\x55\x73\x6f\xf0", 6);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xbb, &tmp[0], 6);
    memcpy(&tmp[0], "\x44\xa4", 2);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc0, &tmp[0], 2);
    memcpy(&tmp[0], "\x12", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc1, &tmp[0], 1);
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc2, &tmp[0], 1);
    memcpy(&tmp[0], "\x7d\x07\x14\x06\xc8\x71\x6c\x77", 8);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc3, &tmp[0], 8);
    memcpy(&tmp[0], "\x00\x00\xa0\x79\x13\x1a\x16\x79\x13\x1a\x16\x82", 12);
//    memcpy(&tmp[0], "\x04\x08\x8e\x79\x1e\x1a\x16\x79\x1e\x1a\x16\x82", 12);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc4, &tmp[0], 12);


    memcpy(&tmp[0], "\x3f\x35\x2e\x27\x2d\x2e\x27\x28\x26\x24\x1f\x10\x0b\x08\x03\x01\x3f\x35\x2e\x27\x2d\x2e\x27\x28\x26\x24\x1f\x10\x0b\x08\x03\x01", 32);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc8, &tmp[0], 32);
    memcpy(&tmp[0], "\x04\x06\x6b\x0f\x00", 5);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xd0, &tmp[0], 5);
    memcpy(&tmp[0], "\x30\x30", 2);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xd7, &tmp[0], 2);
    memcpy(&tmp[0], "\x14",1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xe6, &tmp[0], 1);
    memcpy(&tmp[0], "\x01", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xde, &tmp[0], 1);
    memcpy(&tmp[0], "\x03\x13\xef\x35\x35", 5);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xb7, &tmp[0], 5);
    memcpy(&tmp[0], "\x14\x15\xc0", 3);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc1, &tmp[0], 3);
    memcpy(&tmp[0], "\x06\x3a", 2);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc2, &tmp[0], 2);//add
    memcpy(&tmp[0], "\x72\x12", 2);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc4, &tmp[0], 2);
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xbe, &tmp[0], 1);
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xde, &tmp[0], 1);
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x35, &tmp[0], 1);
    memcpy(&tmp[0], "\x05", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x3a, &tmp[0], 1);//add
    memcpy(&tmp[0], "\x00\x00\x00\xef", 4);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2a, &tmp[0], 4);
    memcpy(&tmp[0], "\x00\x0c\x01\x33", 4);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2b, &tmp[0], 4);

}
const ute_drv_screen_common_config_t uteDrvScreenBoe240X296Wv018lzqConfig =
{
    .name = "Boe240X296Wv018lzq",
    .init = drvScreenBoeWv018lzqInit,
    .powerOn = drvScreenBoeWv018lzqPowerOn,
    .powerOff = drvScreenBoeWv018lzqPowerOff,
    .setWindow = drvScreenBoeWv018lzqSetWindow,
    .setBackLight = NULL,
    .refreshRate = 60,
    .startLine = 0,
    .gramMaxLine = 284,
};
#endif
#endif /* _UTE_DRV_BOE_S240X284_I183_WV8LZQ_QSPI_H_ */


