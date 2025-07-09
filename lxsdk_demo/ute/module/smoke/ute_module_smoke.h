/**
*@file
*@brief        抽烟模块
*@details
*@author       zn.zeng
*@date       2021-08-25
*@version      v1.0
*/
#ifndef _UTE_MODULE_SMOKE_H_
#define _UTE_MODULE_SMOKE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"

#define UTE_MODULE_FILESYSTEM_SMOKE_DATA_DIR "smoke"
#define UTE_MODULE_FILESYSTEM_SMOKE_LEVEL_FILE "smoke/data"

typedef struct
{
    uint32_t total_smoking_count; // 抽吸口数
    uint32_t target_smoking_count; // 抽吸目标口数
    uint32_t smoking_count_per_hour[24];  // 存储每小时的口数
    uint32_t smoking_count_per_day[7];
    int current_power;                     // 当前功率（单/双发）
    u16 current_index; // 当前进度条索引
    u8  resistance_type;
    u8 smoke_position_swich;// （单/双发）
} ute_module_smoke_data_t;
extern ute_module_smoke_data_t uteModuleSmokeData;

typedef struct
{
    int power_curr_nubmer;
    u16 Heating_wire_power;
    int power_watt_index;
} smoke_app_data_t;


void uteModuleSmokeDataReadConfig(void);
void uteModuleSmokeDataSaveConfig(void);
void uteModuleSetSomkeCount(uint32_t count);
uint32_t uteModuleGetSomkeSomkeCount(void);
void uteModuleGetSmokingCountPerHour(uint32_t *counts);
void uteModuleGetSmokingCountPerDay(uint32_t *counts);
void check_and_update_day();
void check_and_update_week();
uint32_t get_today_smoking_count();
uint32_t get_this_week_smoking_count();
void uteModuleResetTotalSmokeCount(void);
void save_current_power_to_file(int power);

#endif
