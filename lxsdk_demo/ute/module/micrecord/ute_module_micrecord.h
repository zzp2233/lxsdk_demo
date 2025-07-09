/**
*@file
*@brief        录音模块头文件
*@details
*@author       zn.zeng
*@date       2021-12-21
*@version      v1.0
*/
#ifndef _UTE_MODULE_MICRECORD_H_
#define _UTE_MODULE_MICRECORD_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "include.h"

typedef enum
{
    FACTORY_TEST_RECORD_IDLE,
    FACTORY_TEST_RECORD_RECORDING,   //录音中
    FACTORY_TEST_RECORD_RECORDED,    //录音完成
    FACTORY_TEST_RECORD_PLAYING,     //播放中
} factory_test_record_state_t;

bool put_mic_obuf(u8 *ptr, u32 samples, int ch_mode);
void mic_test_sdadc_process(u8 *ptr, u32 samples, int ch_mode);
void mic_test_init(void);
void mic_test_start(void);
void mic_test_exit(void);
void mic_test_outp(void);
void uteModuleMicRecordFactoryEnter(void);
void uteModuleMicRecordFactoryExit(void);
void uteModuleMicRecordFactoryStart(void);
void uteModuleMicRecordFactoryPlayStart(void);
void uteModulePlayTimerCallback(void *pxTimer);
void func_record_analysis(void);
uint8_t uteModuleMicRecordFactoryGetRecordState(void);
void uteModuleMicRecordFactorySetrecordState(uint8_t state);
#endif //_UTE_MODULE_CRC_H_

