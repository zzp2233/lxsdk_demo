#ifndef _UTE_DRV_TFT_S240X284_NV3030B_ZD183G1196_H_
#define _UTE_DRV_TFT_S240X284_NV3030B_ZD183G1196_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S240X284_NV3030B_ZD183G1196_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft240X284Nv3030BZd183g1196PowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29,NULL,0);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Nv3030BZd183g1196PowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Nv3030BZd183g1196SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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

__STATIC_INLINE void drvScreenTft240X284Nv3030BZd183g1196Init(void)
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
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFD, &tmp[0], 2);

    memcpy(&tmp[0], "\x07", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x61, &tmp[0], 1);

    memcpy(&tmp[0], "\x70", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x73, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x73, &tmp[0], 1);

    // bias相关
    memcpy(&tmp[0], "\x00\x44\x40", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x62, &tmp[0], 3);

    // VSP相关
    memcpy(&tmp[0], "\x08\x10\x21", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x65, &tmp[0], 3);

    // VSN相关
    memcpy(&tmp[0], "\x08\x10\x21", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x66, &tmp[0], 3);

    // add source_neg_time相关
    memcpy(&tmp[0], "\x21\x40", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x67, &tmp[0], 2);

    // gamma vap/van相关
    memcpy(&tmp[0], "\x9F\x30\x27\x21", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x68, &tmp[0], 4);

    memcpy(&tmp[0], "\x0f\x02\x01", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb1, &tmp[0], 3);

    memcpy(&tmp[0], "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB4, &tmp[0], 1);

    // porch相关
    memcpy(&tmp[0], "\x02\x02\x0a\x14", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB5, &tmp[0], 4);

    memcpy(&tmp[0], "\x44\x01\x9f\x00\x02", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB6, &tmp[0], 5);

    // source相关
    memcpy(&tmp[0], "\x00\xff", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE6, &tmp[0], 2);

    memcpy(&tmp[0], "\x01\x04\x03\x03\x00", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE7, &tmp[0], 5);

    memcpy(&tmp[0], "\x00\x70\x00", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE8, &tmp[0], 3);

    // gate相关
    memcpy(&tmp[0], "\x52", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEc, &tmp[0], 1);

    // gamme sel相关
    memcpy(&tmp[0], "\x11", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xdf, &tmp[0], 1);

    // gamma_test1 A1#_wangly相关
    memcpy(&tmp[0], "\x06\x05\x0b\x12\x10\x10\x10\x15", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe0, &tmp[0], 8);

    memcpy(&tmp[0], "\x15\x10\x11\x0e\x12\x0d\x06\x06", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe3, &tmp[0], 8);

    memcpy(&tmp[0], "\x35\x75", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe1, &tmp[0], 2);

    memcpy(&tmp[0], "\x74\x35", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe4, &tmp[0], 2);

    memcpy(&tmp[0], "\x22\x22\x21\x35\x36\x3f", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe2, &tmp[0], 6);

    memcpy(&tmp[0], "\x3f\x35\x34\x21\x22\x22", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe5, &tmp[0], 6);

    memcpy(&tmp[0], "\x01\x01\x02", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF1, &tmp[0], 3);

    memcpy(&tmp[0], "\x09\x30\x00\x00", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF6, &tmp[0], 4);

    memcpy(&tmp[0], "\xfa\xfc", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfd, &tmp[0], 3);

    memcpy(&tmp[0], "\x55", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3a, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, &tmp[0], 1);

    memcpy(&tmp[0], "\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, &tmp[0], 1);
}

const ute_drv_screen_common_config_t uteDrvScreenTft240X284Nv3030BZd183g1196Config =
{
    .name = "Tft240X284Nv3030BZd183g1196Qspi",
    .init = drvScreenTft240X284Nv3030BZd183g1196Init,
    .powerOn = drvScreenTft240X284Nv3030BZd183g1196PowerOn,
    .powerOff = drvScreenTft240X284Nv3030BZd183g1196PowerOff,
    .setWindow = drvScreenTft240X284Nv3030BZd183g1196SetWindow,
    .setBackLight = NULL,
};

#endif
#endif /* _UTE_DRV_TFT_S240X284_I18_ST7789P3_XHL183BA2401_QSPI_H_ */


