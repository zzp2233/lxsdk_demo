#ifndef _UTE_DRV_BOE_S240X284_I183_WV8LZQ_QSPI_H_
#define _UTE_DRV_BOE_S240X284_I183_WV8LZQ_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if DRV_BOE_S240X284_I183_JD9853_WV018LZQ_QSPI_SUPPORT

__STATIC_INLINE void drvScreenBoeWv018lzqPowerOn(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x11,NULL,0);
    uteModulePlatformDelayUs(120000);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x29,NULL,0);
    uteModulePlatformDelayUs(20000);
    // UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenBoeWv018lzqPowerOff(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x28,NULL,0);
    uteModulePlatformDelayUs(20000);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x10,NULL,0);
    uteModulePlatformDelayUs(50000);
    // UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenBoeWv018lzqSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    uint8_t data[4];

    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2A, data, 4);

    // yStart += 20;
    // yEnd   += 20;

    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2B, data, 4);

    // uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
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
    memcpy(&tmp[0], "\x22", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xb2, &tmp[0], 1);
    memcpy(&tmp[0], "\x00\x2d\x00\x55", 4);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xb7, &tmp[0], 4);
    memcpy(&tmp[0], "\x1f\x9a\x55\x73\x63\xf0", 6);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xbb, &tmp[0], 6);
    memcpy(&tmp[0], "\x22\x22", 2);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc0, &tmp[0], 2);
    memcpy(&tmp[0], "\x12", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc1, &tmp[0], 1);
    memcpy(&tmp[0], "\x7d\x07\x14\x06\xc8\x71\x6c\x77", 8);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc3, &tmp[0], 8);
    memcpy(&tmp[0], "\x00\x00\x8e\x79\x1e\x1a\x16\x79\x1e\x1a\x16\x82", 12);
//    memcpy(&tmp[0], "\x04\x08\x8e\x79\x1e\x1a\x16\x79\x1e\x1a\x16\x82", 12);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc4, &tmp[0], 12);
    memcpy(&tmp[0], "\x3f\x2c\x21\x1a\x1f\x20\x1b\x1b\x19\x19\x16\x0a\x08\x03\x05\x00\x3f\x2c\x21\x1a\x1f\x20\x1b\x1b\x19\x19\x16\x0a\x08\x03\x05\x00", 32);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xc8, &tmp[0], 32);
    memcpy(&tmp[0], "\x04\x06\x6b\x0f\x00", 5);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xd0, &tmp[0], 5);
    memcpy(&tmp[0], "\x00\x30", 2);
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
    memcpy(&tmp[0], "\x00\x00\x01\x1b", 4);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2b, &tmp[0], 4);

}
const ute_drv_screen_common_config_t uteDrvScreenBoe240X284Wv018lzqConfig =
{
    .name = "Boe240X284Wv018lzq",
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


