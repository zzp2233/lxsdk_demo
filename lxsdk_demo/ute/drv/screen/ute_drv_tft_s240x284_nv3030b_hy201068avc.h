#ifndef _UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_H_
#define _UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft240X284Nv3030BHy201068AvcPowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29,NULL,0);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Nv3030BHy201068AvcPowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Nv3030BHy201068AvcSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    yStart += 12;
    yEnd += 12;

    uint8_t data[4];
    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2A, data, 4);
    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2B, data, 4);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
}

__STATIC_INLINE void drvScreenTft240X284Nv3030BHy201068AvcInit(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
    uint8_t tmp[34];
    
    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(1000);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayUs(5000);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(120000);

    memcpy(&tmp[0], "\x06\x08", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfd, &tmp[0], 2);
    memcpy(&tmp[0], "\x07\x04", 2);//06
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x61, &tmp[0], 2);
    memcpy(&tmp[0], "\x00\x44\x40", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x62, &tmp[0], 3);
    memcpy(&tmp[0], "\x41\x07\x12\x12", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x63, &tmp[0], 4);
    memcpy(&tmp[0], "\x37", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x64, &tmp[0], 1);
    memcpy(&tmp[0], "\x09\x10\x21", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x65, &tmp[0], 3);
    memcpy(&tmp[0], "\x09\x10\x21", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x66, &tmp[0], 3);
    memcpy(&tmp[0], "\x20\x40", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x67, &tmp[0], 2);
    memcpy(&tmp[0], "\x90\x4c\x7c\x06", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x68, &tmp[0], 4);
    memcpy(&tmp[0], "\x0f\x02\x01", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb1, &tmp[0], 3);
    memcpy(&tmp[0], "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb4, &tmp[0], 1);
    memcpy(&tmp[0], "\x02\x02\x0a\x14", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb5, &tmp[0], 4);
    memcpy(&tmp[0], "\x04\x01\x9f\x00\x02",5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb6, &tmp[0], 5);
    memcpy(&tmp[0], "\x11", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xdf, &tmp[0], 1);//add
    memcpy(&tmp[0], "\x03\x00\x00\x26\x27\x3f", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe2, &tmp[0], 6);//add
    memcpy(&tmp[0], "\x3f\x27\x26\x00\x00\x03", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe5, &tmp[0], 6);
    memcpy(&tmp[0], "\x00\x57", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe1, &tmp[0], 2);
    memcpy(&tmp[0], "\x58\x00", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe4, &tmp[0], 2);
    memcpy(&tmp[0], "\x01\x03\x0d\x0e\x0e\x0c\x15\x19", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe0, &tmp[0], 8);
    memcpy(&tmp[0], "\x1a\x16\x0c\x0f\x0e\x0d\x02\x01", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe3, &tmp[0], 8);
    memcpy(&tmp[0], "\x00\xff", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe6, &tmp[0], 2);
    memcpy(&tmp[0], "\x01\x04\x03\x03\x00\x12", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe7, &tmp[0], 6);
    memcpy(&tmp[0], "\x00\x70\x00", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe8, &tmp[0], 3);
    memcpy(&tmp[0], "\x52", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xec, &tmp[0], 1);
    memcpy(&tmp[0], "\x01\x01\x02", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf1, &tmp[0], 3);
    memcpy(&tmp[0], "\x09\x10\x00\x00", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf6, &tmp[0], 4);
    memcpy(&tmp[0], "\xfa\xfc", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfd, &tmp[0], 2);
    memcpy(&tmp[0], "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3a, &tmp[0], 1);
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, &tmp[0], 1);
    memcpy(&tmp[0], "\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, &tmp[0], 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x21, NULL, 0);
}

const ute_drv_screen_common_config_t uteDrvScreenTft240X284Nv3030BHy201068AvcConfig =
{
    .name = "Tft240X284Nv3030BHy201068AvcQspi",
    .init = drvScreenTft240X284Nv3030BHy201068AvcInit,
    .powerOn = drvScreenTft240X284Nv3030BHy201068AvcPowerOn,
    .powerOff = drvScreenTft240X284Nv3030BHy201068AvcPowerOff,
    .setWindow = drvScreenTft240X284Nv3030BHy201068AvcSetWindow,
    .setBackLight = NULL,
};

#endif
#endif /* _UTE_DRV_TFT_S240X284_I18_ST7789P3_XHL183BA2401_QSPI_H_ */


