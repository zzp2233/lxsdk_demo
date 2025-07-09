/**
*@file
*@brief        gui 公共层
*@details
*@author       zn.zeng
*@date       2021-09-03
*@version      v1.0
*/

#ifndef _UTE_MODULE_GUI_COMMON_H_
#define _UTE_MODULE_GUI_COMMON_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "typedef.h"
#include "include.h"

#define UTE_MOUDLE_SCREENS_FACTORY_TEST_ID FUNC_ONLINE_FACTORY_TEST
#define UTE_MOUDLE_SCREENS_WATCHMAIN_ID FUNC_CLOCK
#define UTE_MOUDLE_SCREENS_CHARGER_ID FUNC_CHARGE
#define UTE_MOUDLE_SCREENS_POWERON_ID FUNC_POWER_ON
#define UTE_MOUDLE_SCREENS_SYNC_WATCH_ONLINE_ID FUNC_UP_WATCH_DIAL
#define UTE_MOUDLE_SCREENS_NEW_FACTORY_AGING_ID FUNC_AGEING
#define UTE_MOUDLE_SCREENS_NEW_FACTORY_MODULE_ID FUNC_FACTORY_TESTING
#define UTE_MOUDLE_SCREENS_NEW_FACTORY_MODE_SELECT_ID FUNC_TEST_MODE
#define UTE_MOUDLE_SCREENS_APP_BINDING_ID FUNC_POWER_ON_SCAN
#define UTE_MOUDLE_SCREENS_POWER_ON_LANGUAGE_SELECT_ID FUNC_POWER_ON_LANGUAGE
#define UTE_MOUDLE_SCREENS_LANGUAGE_SELECT_LIST_ID FUNC_LANGUAGE

/*! 控制显示内容相关参数zn.zeng, 2021-08-18  */
typedef struct
{
    uint8_t displayOffTimeSecond;
    uint8_t backLightPercent;
    bool isFahrenheit;
    uint8_t currWatchIndex; //当前表盘的配置索引
    uint8_t currWatchMaxIndex; //表盘总个数配置
    uint8_t screenTblSort[MAX_FUNC_SORT_CNT];
    uint8_t screenSortCnt;
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    uint8_t currScreenSaverIndex;
    bool isScreenSaverSetOn;
#endif
} ute_display_ctrl_t;

typedef struct
{
    bool isDisplayOn;
    bool isGoBackDisplay;
    ute_display_ctrl_t displayCtrl;
    int themeTypeId;//主题类型,蜂窝,列表,转圈等
    bool isPowerSavingOpen;
    bool isSwitchToMenu;
} ute_module_gui_common_t;

typedef struct
{
    int16_t x;
    int16_t y;
} UTE_UI_Point_t;

void uteModuleGuiCommonInit(void);
void uteModuleGuiCommonGetCurrWatchIndex(uint8_t *index);
uint8_t uteModuleGuiCommonGetCurrWatchMaxIndex(void);
void uteModuleGuiCommonSetCurrWatchIndex(uint8_t index);
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
void uteModuleGuiCommonSetCurrSreenSaverIndex(uint8_t index);
bool uteModuleGuiCommonIsScreenSaverSetOn(void);
void uteModuleGuiCommonScreenSaverConfigInit(void);
void uteModuleGuiCommonGetScreenSaverConfig(UI_WatchConfigTypeDef **config,uint8_t index);
void uteModuleGuiCommonGetCurrScreenSaverIndex(uint8_t *index);
void uteModuleGuiCommonSetCurrSreenSaverIndex(uint8_t index);
void uteModuleGuiCommonGetSreenSaverPreviewsIndex(uint8_t **index0,uint8_t **index1,uint8_t **index2);
bool uteModuleGuiCommonIsScreenSaverSetOn(void);
void uteModuleGuiCommonScreenSaverSetSwitch(void);
void uteModuleGuiCommonStopTimerForScreenSaver(void);
void uteModuleGuiCommonSetInScreenSaver(bool isScreenSaver);
bool uteModuleGuiCommonIsInScreenSaver(void);
#endif
void uteModuleGuiCommonDisplayExternalClearDepth(void);
void uteModuleGuiCommonSetDisplayTemperatureUtil(bool isFahrenheit);
void uteModuleGuiCommonGetDisplayInfo(ute_display_ctrl_t *displayInfo);
void uteModuleGuiCommonSaveConfig(void);
void uteModuleGuiCommonSetBackLightPercent(uint8_t setBackLightPercent);
void uteModuleGuiCommonSetDisplayOffTime(uint8_t displayOffTimeSecond);
uint8_t uteModuleGuiCommonGetDisplayOffTime(void);
void uteModuleGuiCommonDisplayOff(bool isPowerOff);
void uteModuleGuiCommonSetThemeTypeId(int themeTypeId);
int uteModuleGuiCommonGetThemeTypeId(void);
void uteModuleGuiCommonGoBackLastScreen(void);
void uteModuleGuiCommonGoToMenuScreen(void);
bool uteModuleGuiCommonIsDisplayOn(void);
void uteModuleGuiCommonDisplayDepthClearTop(bool isAllClear);
int uteModuleGuiCommonGetCurrentScreenId(void);
int uteModuleGuiCommonGetLastScreenId(void);
void uteTaskGuiStartScreen(uint8_t screenId, uint16_t switchMode, const char *format, ...);
void uteTaskGuiStackRemoveScreenId(uint8_t screenId);
bool uteModuleGuiCommonIsSwitchToMenu(void);
void uteModuleGuiCommonSetSwitchToMenu(bool isSwitchToMenu);

uint8_t uteModuleGuiCommonGetBackLightPercent(void);
bool uteModuleGuiCommonIsDontNeedNotificationGuiScreen(void);
uint8_t uteModuleGuiCommonGetVailWatchCnt(void);
#if UTE_MODULE_POWER_SAVING_SUPPORT
void uteModuleGuiCommonSetPowerSavingModeOpen(bool isPowerSavingModeOpen);
bool uteModuleGuiCommonGetPowerSavingModeOpen(void);
#endif
#if UTE_CUSTOM_HAND_SCREEN_ON_DISPLAY_OFF_TIME
void uteResetHandScreenOnFlag(void);
#endif
bool uteModuleGuiCommonIsAllowHandGestureDisplayOff(void);
void uteModuleGuiCommonDisplayOffAllowGoBack(bool allow);
void uteModuleGuiCommonHandScreenOnMsg(void);
void uteModuleGuiCommonHandScreenOffMsg(void);
void uteModuleGuiCommonSavescreenTblSort(uint8_t *tblSort, uint8_t sortCnt);
void uteModuleGuiCommonGetScreenTblSort(uint8_t *tblSort, uint8_t *sortCnt);
void uteModuleGuiCommonWatchConfigInit(void);
#endif //_UTE_MODULE_GUI_COMMON_H_
