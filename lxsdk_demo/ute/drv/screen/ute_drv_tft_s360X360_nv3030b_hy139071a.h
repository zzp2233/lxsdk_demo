#ifndef _UTE_DRV_TFT_S360X360_NV3030B_HY139071A_H_
#define _UTE_DRV_TFT_S360X360_NV3030B_HY139071A_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S360X360_NV3030B_HY139071A_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft360X360Nv3030BHy139071aPowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29,NULL,0);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft360X360Nv3030BHy139071aPowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft360X360Nv3030BHy139071aSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    yStart += 0;
    yEnd += 0;

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

__STATIC_INLINE void drvScreenTft360X360Nv3030BHy139071aInit(void)
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

    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfe, NULL, 0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xef, NULL, 0);

    memcpy(tmp, "\x11", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x80, tmp, 1);
    memcpy(tmp, "\x30", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x81, tmp, 1);
    memcpy(tmp, "\x09", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x82, tmp, 1);
    memcpy(tmp, "\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x83, tmp, 1);
    memcpy(tmp, "\x72", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x84, tmp, 1);
    memcpy(tmp, "\x18", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x89, tmp, 1);
    memcpy(tmp, "\x40", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8A, tmp, 1);
    memcpy(tmp, "\x0A", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8B, tmp, 1);

    memcpy(tmp, "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3a, tmp, 1);

    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, tmp, 1);

    memcpy(tmp, "\x07", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEC, tmp, 1);

    memcpy(tmp, "\x01\x80\x00\x00\x00\x00", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x74, tmp, 6);

    memcpy(tmp, "\x3E", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x98, tmp, 1);
    memcpy(tmp, "\x3E", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x99, tmp, 1);

    memcpy(tmp, "\x01\x04", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA1, tmp, 2);

    memcpy(tmp, "\x01\x04", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA2, tmp, 2);

    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCB, tmp, 1);

    memcpy(tmp, "\xB6\x24", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7C, tmp, 2);

    memcpy(tmp, "\x34", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xAC, tmp, 1);

    memcpy(tmp, "\x80", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF6, tmp, 1);

    memcpy(tmp, "\x0D\x0D", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB5, tmp, 2);

    // 反扫相关代码被注释，暂不处理

    memcpy(tmp, "\x01\x67", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEB, tmp, 2);

    memcpy(tmp, "\x38\x0E\x1F\x56", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x60, tmp, 4);

    memcpy(tmp, "\x38\x10\x1F\x56", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x63, tmp, 4);

    memcpy(tmp, "\x38\x12\x72\xEA\x1F\x56", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x64, tmp, 6);

    memcpy(tmp, "\x38\x16\x72\xEE\x1F\x56", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x66, tmp, 6);

    memcpy(tmp, "\x00\x0A\xEE\x0A\xEE\x1C\x1C", 7);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x68, tmp, 7);

    memcpy(tmp, "\x00\x0A\xF2\x0A\xF2\x1C\x1C", 7);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x69, tmp, 7);

    memcpy(tmp, "\x07\x01\x1A\x00\x0B\x0B\x09\x09\x13\x13\x11\x11\x19\x1E\x1D\x00\x00\x1D\x1E\x19\x12\x12\x14\x14\x0A\x0A\x0C\x0C\x00\x1A\x02\x08", 30);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6E, tmp, 30);

    memcpy(tmp, "\xCC\x0C\xCC\x84\xCC\x04\x5F", 7);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6C, tmp, 7);

    memcpy(tmp, "\x72", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7D, tmp, 1);

    memcpy(tmp, "\x02\x03\x09\x07\x09\x03\x09\x07\x09\x03", 10);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x70, tmp, 10);

    memcpy(tmp, "\x06\x06\x05\x06", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x90, tmp, 4);

    memcpy(tmp, "\x45\xFF\x00", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x93, tmp, 3);

    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC3, tmp, 1);
    memcpy(tmp, "\x36", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, tmp, 1);
    memcpy(tmp, "\x3f", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC9, tmp, 1);

    memcpy(tmp, "\x4a\x09\x0a\x06\x00\x2e", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, tmp, 6);

    memcpy(tmp, "\x4a\x0a\x0a\x05\x00\x2e", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF2, tmp, 6);

    memcpy(tmp, "\x47\xd3\x9c\x2D\x2F\xCF", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF1, tmp, 6);

    memcpy(tmp, "\x46\xd2\x9c\x2D\x2F\xCF", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF3, tmp, 6);

    memcpy(tmp, "\x11", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBE, tmp, 1);

    memcpy(tmp, "\x00\x00", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFB, tmp, 2);

    memcpy(tmp, "\x0A", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB4, tmp, 1);

    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, tmp, 1);

    memcpy(tmp, "\x00\x90", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x44, tmp, 2);

    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfe, NULL, 0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xee, NULL, 0);
}

const ute_drv_screen_common_config_t uteDrvScreenTft360X360Nv3030BHy139071aConfig =
{
    .name = "Tft360X360Nv3030BHy139071aQspi",
    .init = drvScreenTft360X360Nv3030BHy139071aInit,
    .powerOn = drvScreenTft360X360Nv3030BHy139071aPowerOn,
    .powerOff = drvScreenTft360X360Nv3030BHy139071aPowerOff,
    .setWindow = drvScreenTft360X360Nv3030BHy139071aSetWindow,
    .setBackLight = NULL,
};

#endif
#endif /* _UTE_DRV_TFT_S240X284_I18_ST7789P3_XHL183BA2401_QSPI_H_ */


