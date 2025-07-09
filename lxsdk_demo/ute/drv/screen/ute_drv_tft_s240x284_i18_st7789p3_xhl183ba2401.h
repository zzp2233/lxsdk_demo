#ifndef _UTE_DRV_TFT_S280X284_I18_ST7789P3_XHL183BA2401_DSPI_H_
#define _UTE_DRV_TFT_S280X284_I18_ST7789P3_XHL183BA2401_DSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if DRV_TFT_S280X284_I18_ST7789P3_XHL183BA2401_DSPI_SUPPORT

__STATIC_INLINE void drvScreenTft280X284St7789p3Xhl183ba2401PowerOn(void)
{
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x29,NULL,0);
//    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft280X284St7789p3Xhl183ba2401PowerOff(void)
{
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
//    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft280X284St7789p3Xhl183ba2401SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    uint8_t data[4];
    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x2A, data, 4);
    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x2B, data, 4);
    uteModulePlatformDspiWriteGram(0x2c);
}
__STATIC_INLINE void drvScreenTft280X284St7789p3Xhl183ba2401Init(void)
{
    uint8_t tmp[34];
    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(1000);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayUs(1000);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(120000);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x11, NULL, 0);
    uteModulePlatformDelayUs(120000);
    //id
    uint8_t idBuff[3]= {0,0,0};
    uteModulePlatformScreenDspiReadCmd(0xda,&idBuff[0],1,0);
    uteModulePlatformScreenDspiReadCmd(0xdb,&idBuff[1],1,0);
    uteModulePlatformScreenDspiReadCmd(0xdc,&idBuff[2],1,0);
    uint32_t id = idBuff[0]|idBuff[1]<<8|idBuff[2]<<16;
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,da~dc,id=0x%x", __func__,id);
    uteModulePlatformScreenDspiReadCmd(0x04,&idBuff[0],3,1);
    id = idBuff[0]|idBuff[1]<<8|idBuff[2]<<16;
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,04,id=0x%x", __func__,id);
    //
    memcpy(&tmp[0],"\x00",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x36, &tmp[0], 1);
    memcpy(&tmp[0],"\x05",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x3a, &tmp[0], 1);
    memcpy(&tmp[0],"\x0c\x0c\x00\x33\x33",5);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xB2, &tmp[0], 5);
    memcpy(&tmp[0],"\x05",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xb7, &tmp[0], 1);
    memcpy(&tmp[0],"\x21",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xbb, &tmp[0], 1);
    memcpy(&tmp[0],"\x2c",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xc0, &tmp[0], 1);
    memcpy(&tmp[0],"\x01",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xc2, &tmp[0], 1);
    memcpy(&tmp[0],"\x15",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xc3, &tmp[0], 1);
    memcpy(&tmp[0],"\x0f",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xc6, &tmp[0], 1);
    memcpy(&tmp[0],"\xa7\xa1",2);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xd0, &tmp[0], 2);
    memcpy(&tmp[0],"\xa4\xa1",2);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xd0, &tmp[0], 2);
    memcpy(&tmp[0],"\xa1",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xd6, &tmp[0], 1);

    memcpy(&tmp[0],"\xf0\x05\x0e\x08\x0a\x17\x39\x54\x4e\x37\x12\x12\x31\x37",14);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xe0, &tmp[0], 14);

    memcpy(&tmp[0],"\xf0\x10\x14\x0d\x0b\x05\x39\x44\x4d\x38\x14\x14\x2e\x35",14);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xe1, &tmp[0], 14);

    memcpy(&tmp[0],"\x23\x00\x00",3);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xe4, &tmp[0], 3);

    memcpy(&tmp[0],"\x00",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x35, &tmp[0], 1);
    memcpy(&tmp[0],"\x10",1);
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0xe7, &tmp[0], 1);

    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(0x21, NULL, 0);
}
const ute_drv_screen_common_config_t uteDrvScreenTft280X284St7789p3Xhl183ba2401Config =
{
    .name = "Tft280X284St7789p3xhl183ba2401dspi",
    .init = drvScreenTft280X284St7789p3Xhl183ba2401Init,
    .powerOn = drvScreenTft280X284St7789p3Xhl183ba2401PowerOn,
    .powerOff = drvScreenTft280X284St7789p3Xhl183ba2401PowerOff,
    .setWindow = drvScreenTft280X284St7789p3Xhl183ba2401SetWindow,
    .setBackLight = NULL,
    .refreshRate = 60,
    .startLine = 0,
    .gramMaxLine = 306,
};
#endif
#endif /* _UTE_DRV_TFT_S280X284_I18_ST7789P3_XHL183BA2401_DSPI_H_ */


