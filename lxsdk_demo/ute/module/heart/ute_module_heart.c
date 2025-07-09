/**
*@file
*@brief     心率模块
*@details   心率模块相关
*@author        zn.zeng
*@date        2021-07-12
*@version       v1.0
*/
#include "ute_module_heart.h"
// #include "ute_drv_heart_vcxx_common.h"
#include "ute_module_systemtime.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_protocol.h"
#include "ute_module_crc.h"
#include "ute_module_gui_common.h"
#include "ute_drv_motor.h"
#include "ute_module_filesystem.h"
#include "ute_module_bloodoxygen.h"
#include "ute_drv_battery_common.h"
#include "ute_module_sport.h"
#include "ute_module_message.h"
#include "ute_module_factoryTest.h"

#if UTE_MODULE_HEART_SUPPORT
// #include "ute_module_sleep.h"
/*! 心率模块数据zn.zeng, 2021-07-13  */
ute_module_heart_data_t uteModuleHeartData;
/**
*@brief         心率初始化
*@details
*@author       zn.zeng
*@date       2021-08-26
*/
void uteModuleHeartInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR);
#if UTE_MODULE_HEART_WEEK_STATIC_SUPPORT
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_DIR);
#endif
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR, NULL, NULL);
#if UTE_MODULE_HEART_WEEK_STATIC_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_DIR, NULL, NULL);
#endif
#endif
    memset(&uteModuleHeartData,0,sizeof(ute_module_heart_data_t));
    uteModuleHeartData.lastIsWear = true;
    uteModuleHeartReadConfig();
#if UTE_DRV_HEART_VCXX_SUPPORT
    uteDrvHeartVcxxInit();
#else
    bsp_sensor_hr_stop();
#endif
    uteModuleSystemtimeRegisterSecond(uteModuleHeartEverySecond);
    /*add by pcm 2023-06-06 把心率温度检测延迟到心率模块初始化结束之后再开始检测*/
#if DRV_HEART_SENSOR_TEMPERATURE_CHECK_VCXXHCI_SUPPORT
    uteModuleVcxxTempCheckEverySecondInit();
#endif
}
/**
*@brief         读取心率config
*@details
*@author       zn.zeng
*@date       2021-08-26
*/
void uteModuleHeartReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[3];
    /*! 心率警告值zn.zeng, 2021-08-20   */
#if UTE_MODULE_HEART_MIN_MAX_WARNING_VALUE_SUPPORT
    readbuff[0] = DEFAULT_HEARTWARNING_IS_OPEN;
    readbuff[1] = DEFAULT_HEARTWARNING_MIN_VALUE;
    readbuff[2] = DEFAULT_HEARTWARNING_MAX_VALUE;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEARTWARNINGINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],3);
        uteModuleFilesystemCloseFile(file);
    }
    ute_module_heart_warning_t heartWarning;
    heartWarning.isOpen = readbuff[0];
    heartWarning.setMinHeart = readbuff[1];
    heartWarning.setMaxHeart = readbuff[2];
    memcpy(&uteModuleHeartData.warning,&heartWarning,sizeof(ute_module_heart_warning_t));
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,heartwarning isOpen=%d,min=%d,max=%d", __func__,heartWarning.isOpen,heartWarning.setMinHeart,heartWarning.setMaxHeart);
#endif
#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
    uteModuleHeartReadSportHeartWaringInfo();
#endif
    /*! 24小时自动测试参数zn.zeng, 2021-08-30  */
    readbuff[0] = DEFAULT_HEART24AUTO_TEST_IS_OPEN;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEART_SET_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleHeartData.isAutoTesting = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,isAutoTesting=%d", __func__,uteModuleHeartData.isAutoTesting);

    /*! 心率最大值最小值平均值,xjc, 2022-01-17  */
    readbuff[0] = 0;
    readbuff[1] = 0xFF;
    readbuff[2] = 0;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEART_MAX_MIN_AVG_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],3);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleHeartData.currentDayMaxHeart = readbuff[0];
    uteModuleHeartData.currentDayMinHeart = readbuff[1];
    uteModuleHeartData.currentDayAvgHeart = readbuff[2];
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,currentDayMaxHeart=%d,currentDayMinHeart=%d,currentDayAvgHeart=%d", __func__,uteModuleHeartData.currentDayMaxHeart,uteModuleHeartData.currentDayMinHeart,uteModuleHeartData.currentDayAvgHeart);
#if UTE_MODULE_HEART_WEEK_STATIC_SUPPORT
    uteModuleHeartrReadStaticHeartData();
#endif
}
/**
*@brief        同步心率最大最小跟平均值
*@details
*@author       zn.zeng
*@date       2021-07-12
*/
void uteModuleHeartSyncMaxMinAvgHeartValue(ute_module_systemtime_time_t time)
{
#if !UTE_HEART_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
    if(uteModuleHeartIsWear()&&(uteModuleHeartData.type == TYPE_HEART))
#endif
    {
        if((uteModuleHeartData.currentDayMinHeart>UTE_MODULE_HEART_MIN_VALUE)&&(uteModuleHeartData.currentDayMinHeart<UTE_MODULE_HEART_MAX_VALUE))
        {
            uint8_t heartAvgData[11];
            heartAvgData[0] = CMD_HEART_24HOURS_AUTO_TEST;
            heartAvgData[1] = 0x04;
            heartAvgData[2] = time.year>>8&0xff;
            heartAvgData[3] = time.year&0xff;
            heartAvgData[4] = time.month;
            heartAvgData[5] = time.day;
            heartAvgData[6] = time.hour;
            heartAvgData[7] = time.min;
            heartAvgData[8] = uteModuleHeartData.currentDayMaxHeart;
            heartAvgData[9] = uteModuleHeartData.currentDayMinHeart;
            heartAvgData[10] = (uint8_t)uteModuleHeartData.currentDayAvgHeart;
#if !DUG_VCXX_HEART_SUPPORT
#if UTE_HEART_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
            uteModuleProfileBleSendToPhone(&heartAvgData[0], 11);
#else
            uint16_t screenId = uteModuleGuiCommonGetCurrentScreenId();
            if(uteModuleGuiCommonIsDisplayOn()  && screenId == FUNC_HEARTRATE)
            {
                uteModuleProfileBleSendToPhone(&heartAvgData[0], 11);
            }
#endif
#endif
        }
    }
}
/**
*@brief        心率每秒函数
*@details       需要注册到主时间，每秒执行一次
*@author       zn.zeng
*@date       2021-07-12
*/
void uteModuleHeartEverySecond(void)
{
    if(!uteApplicationCommonIsPowerOn())
    {
        return;
    }
#if UTE_DRV_HEART_VCXX_SUPPORT
    uteDrvHeartVcxxCheckErrorEveryScond();
    uteDrvHeartVcxxGetAlgoHrData(&uteModuleHeartData.heartValue);
#endif

    if(vc30fx_usr_get_work_status() && (vc30fx_usr_get_work_mode() == WORK_MODE_HR || vc30fx_usr_get_work_mode() == WORK_MODE_HRSPO2))
    {
        uteModuleHeartData.heartValue = bsp_sensor_hrs_data_get();
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,uteModuleHeartData.heartValue=%d", __func__,uteModuleHeartData.heartValue);
    }

    /*! 每秒发一次心率值发给app端 zn.zeng, 2021-07-15   */
    uint8_t heartData[4] = {CMD_HEART_TEST,0x11,0,0};
    heartData[0] = CMD_HEART_TEST;
    heartData[1] = 0x11;
    heartData[2] = 0;
    heartData[3] = (uint8_t)uteModuleHeartData.heartValue;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    /*! 24h自动测试逻辑zn.zeng, 2021-07-16  */
    if(uteModuleHeartIsAutoTesting())
    {
        uint16_t oneDayMin = 0;
        oneDayMin = time.hour*60+time.min;
        /*! 每10分钟保存一次心率值并且发给app端 zn.zeng, 2021-07-15 */

        static bool isNeedAutoTest = false;
        if(((oneDayMin % 10) == 0) && (time.sec == 0))  //
        {
            isNeedAutoTest = true;
        }

        if(!uteModuleBloodoxygenIsTesting() &&
           (uteDrvBatteryCommonGetChargerStatus() == BAT_STATUS_NO_CHARGE)
           && (uteModuleFactoryTestGetCurrTestItem() == TEST_ITEM_NONE)
           && uteModuleHeartGetWorkMode() != WORK_MODE_HR
          )
        {
            if(isNeedAutoTest && !uteModuleHeartData.isAutoTestFlag)
            {
                uteModuleHeartStartSingleTesting(TYPE_HEART);
                uteModuleHeartData.isAutoTestFlag = true;
                uteModuleHeartData.autoTestSecond = 0;
            }
            else
            {
                isNeedAutoTest = false;
            }
        }

        if(uteModuleHeartData.isAutoTestFlag)
        {
            if (uteModuleHeartData.autoTestSecond >= UTE_MODULE_HEART_AUTO_TEST_TIMEOUT_SECOND
                && uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_CLOSE
                && uteModuleGuiCommonGetCurrentScreenId() != FUNC_HEARTRATE
               )
            {
#if UTE_MODULE_HEART_SUPPORT
                uteModuleHeartStopSingleTesting(TYPE_HEART);
#endif
                uteModuleHeartData.isAutoTestFlag = false;
                uteModuleHeartData.autoTestSecond = 0;
            }
            else if (!uteModuleHeartIsWear() && uteModuleHeartData.autoTestSecond >= 10)
            {
#if UTE_MODULE_HEART_SUPPORT
                uteModuleHeartStopSingleTesting(TYPE_HEART);
#endif
                uteModuleHeartData.isAutoTestFlag = false;
                uteModuleHeartData.autoTestSecond = 0;
                isNeedAutoTest = false;
            }
            else
            {
                uteModuleHeartData.autoTestSecond++;
            }
        }

#if !DUG_VCXX_HEART_SUPPORT
        /*! 心率界面每秒发一次心率值发给app端 zn.zeng, 2021-07-15   */
        if(uteModuleHeartIsWear()&&(uteModuleHeartData.type == TYPE_HEART))
        {
#if UTE_MODULE_HEART_SYNC_VALUE_IN_WATTCHMAIN_SUPPORT
            if(uteModuleGuiCommonGetCurrentScreenId() == FUNC_HEARTRATE || uteModuleGuiCommonGetCurrentScreenId() == FUNC_CLOCK)
#else
            if(uteModuleGuiCommonGetCurrentScreenId() == FUNC_HEARTRATE)
#endif
            {
                uteModuleProfileBleSendToPhone(&heartData[0],4);
            }
        }
#endif
    }
    /*! 单次测试逻辑zn.zeng, 2021-07-16  */
    if(uteModuleHeartData.isSingleTesting)
    {
        if(uteModuleHeartData.type == TYPE_HEART)
        {
            if(!uteModuleHeartIsWear())
            {
                if(uteModuleHeartData.notWearSecond==1)
                {
                    uint8_t stopSingleCmd[2];
                    stopSingleCmd[0] = CMD_HEART_TEST;
                    stopSingleCmd[1] = 0x00;
                    uteModuleProfileBleSendToPhone(&stopSingleCmd[0],2);
                    uteModuleHeartData.notWearSecond=2;
                }
                else
                {
                    uteModuleHeartData.notWearSecond++;
                }
            }
            else
            {
                if(uteModuleHeartData.notWearSecond==2)
                {
                    uint8_t stopSingleCmd[2];
                    stopSingleCmd[0] = CMD_HEART_TEST;
                    stopSingleCmd[1] = 0x11;
                    uteModuleProfileBleSendToPhone(&stopSingleCmd[0],2);
                }
                /*! 每秒发一次心率值发给app端 zn.zeng, 2021-07-15   */
#if !DUG_VCXX_HEART_SUPPORT
                if(uteModuleGuiCommonGetCurrentScreenId() == FUNC_HEARTRATE)
                {
                    uteModuleProfileBleSendToPhone(&heartData[0],4);
                }
#endif
                uteModuleHeartData.notWearSecond = 0;
            }
        }
    }
    else
    {
        uteModuleHeartData.notWearSecond = 0;
    }
#if (!UTE_HEART_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT)
    /*! 同步手环和手机端的最大最小平均值 zn.zeng, 2021-07-16  */
    uteModuleHeartSyncMaxMinAvgHeartValue(time);
    /*! 计算心率最大最小平均值 zn.zeng, 2021-07-16  */
    uteModuleHeartAvgHeartOfCurrentDayProcess((uint8_t)uteModuleHeartData.heartValue);
#endif
    /*! 换天清除心率最大最小平均值 zn.zeng, 2021-07-16  */
    uteModuleHeartClearAvgHeartOfCurrentDay(&time);
    /*! 心率最大最小值报警 zn.zeng, 2021-07-16  */
    uteModuleHeartMinMaxWarningValueProcess();
#if UTE_MODULE_HEART_WEEK_STATIC_SUPPORT
    uteModuleHeartWeekStaticSecond(time);
#endif
#if UTE_MODULE_HEART_UNWEAR_RESET_HEART_VALUE_SUPPORT
    if ((uteModuleHeartData.heartValue != 0) && (uteModuleHeartData.heartValue != 0xFF) && (!uteModuleHeartIsWear()))
    {
        uteModuleHeartData.heartValue = 0xFF;
    }
#endif
}
/**
*@brief        心率读取数据
*@details
*@author       zn.zeng
*@date       2021-07-13
*/
void uteModuleHeartReadValue(void)
{
#if UTE_DRV_HEART_VCXX_SUPPORT
    uteDrvHeartVcxxReadValue();
#endif
}
/**
*@brief        不需要心率灯亮条件
*@details
*@return ture 为不需要心率灯亮
*@author       zn.zeng
*@date       2021-07-13
*/
bool uteModuleHeartIsNotNeedHeartLight(void)
{
    return false;
}
/**
*@brief        是否需要打开心率测试
*@details
*@return ture 为需要打开心率测试
*@author       zn.zeng
*@date       2021-07-13
*/
bool uteModuleHeartIsNeedTesting(void)
{
    bool isSingleTesting = false;
    if((uteModuleHeartData.isAutoTesting||uteModuleHeartData.isSingleTesting))
    {
        isSingleTesting = true;
    }
    return isSingleTesting;
}
/**
*@brief        当前自动测试模式是否打开
*@details
*@return
*@author       zn.zeng
*@date       2021-07-15
*/
bool uteModuleHeartIsAutoTesting(void)
{
    return uteModuleHeartData.isAutoTesting;
}
/**
*@brief        当前单次测试模式是否打开
*@details
*@return
*@author       zn.zeng
*@date       2021-08-30
*/
bool uteModuleHeartIsSingleTesting(void)
{
    return uteModuleHeartData.isSingleTesting;
}

/**
*@brief        读取心率值
*@details
*@author       zn.zeng
*@date       2021-07-15
*/
int uteModuleHeartGetHeartValue(void)
{
    return uteModuleHeartData.heartValue;
}
/**
 * @brief        设置心率值
 * @details
 * @param[in]    heartValue 心率值
 * @author       Wang.Luo
 * @date         2022-11-29
 */
void uteModuleHeartSetHeartValue(int heartValue)
{
    if(heartValue >= UTE_MODULE_HEART_MIN_VALUE && heartValue <= UTE_MODULE_HEART_MAX_VALUE)
    {
        uteModuleHeartData.heartValue = heartValue;
    }
    else
    {
        uteModuleHeartData.heartValue = 0;
    }
}
/**
*@brief        读取心率最大值
*@details
*@author       dengli.lu
*@date       2021-10-25
*/
uint8_t uteModuleHeartGetMaxHeartValue(void)
{
    return uteModuleHeartData.currentDayMaxHeart;
}
/**
*@brief        读取心率最大值
*@details
*@author       dengli.lu
*@date       2021-10-25
*/
uint8_t uteModuleHeartGetMinHeartValue(void)
{
    return uteModuleHeartData.currentDayMinHeart;
}
/**
*@brief        读取心率最大值
*@details
*@author       dengli.lu
*@date       2021-10-25
*/
uint8_t uteModuleHeartGetAvgHeartValue(void)
{
    return uteModuleHeartData.currentDayAvgHeart;
}

/**
*@brief        开始单次测试
*@details
*@param ute_module_heart_type_t type 启动类型
*@author       zn.zeng
*@date       2021-07-16
*/
void uteModuleHeartStartSingleTesting(ute_module_heart_type_t type)
{
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,type=%d", __func__, type);
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HEART_START_SINGLE_TESTING, (uint32_t)type);
}

// /**
// *@brief        开始单次测试消息处理函数
// *@details
// *@param ute_module_heart_type_t type 启动类型
// *@author       zn.zeng
// *@date       2021-07-16
// */
void uteModuleHeartStartSingleTestingMsgHandler(uint32_t param)
{
    ute_module_heart_type_t type = (ute_module_heart_type_t)param;
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL,"%s,input type:%d, curr type:%d",__func__,type,uteModuleHeartData.type);
    if(uteModuleHeartData.type != type)
    {
#if UTE_MODULE_HEART_SUPPORT
        uteModuleHeartStopSingleTesting(uteModuleHeartData.type);
#endif
    }
    uteModuleHeartData.type = type;
    if(uteModuleHeartData.type==TYPE_BLOODOXYGEN)
    {
        uteModuleHeartData.isSingleTesting = false;
#if UTE_DRV_HEART_VCXX_SUPPORT
        uteDrvHeartVcxxBloodoxygenStartSample();
#else
        bsp_sensor_hr_init(WORK_MODE_SPO2);
#endif
    }
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
    else if(uteModuleHeartData.type==TYPE_EMOTION_PRESSURE)
    {
        uteModuleHeartData.isSingleTesting = true;
#if UTE_DRV_HEART_VCXX_SUPPORT
        uteDrvHeartVcxxEmotionPressureStartSample();
#endif
    }
#endif
    else if (uteModuleHeartData.type == TYPE_FACTORY0)
    {
        bsp_sensor_hr_init(WORK_MODE_FACTORY0);
    }
    else if (uteModuleHeartData.type == TYPE_FACTORY1)
    {
        bsp_sensor_hr_init(WORK_MODE_FACTORY1);
    }
    else if (uteModuleHeartData.type == TYPE_WEAR)
    {
        bsp_sensor_hr_init(WORK_MODE_WEAR);
    }
    else
    {
        if(uteModuleHeartData.type==TYPE_BREATHRATE)
        {
#if UTE_DRV_BREATHRATE_VCXX_SUPPORT
            uteDrvHeartVcxxStartBreathrate();
#else
            bsp_sensor_hr_init(WORK_MODE_HR);
#endif
        }
        // if(uteModuleHeartData.isAutoTesting && uteModuleHeartData.isAutoTestFlag)
        // {
        //     return;
        // }
        // if(uteModuleHeartData.isSingleTesting)
        // {
        //     return;
        // }
        uteModuleHeartData.isSingleTesting = true;
#if UTE_DRV_HEART_VCXX_SUPPORT
        if(!uteModuleHeartData.isAutoTesting)
        {
            uteDrvHeartVcxxStartSample();
        }
#else
        bsp_sensor_hr_init(WORK_MODE_HR);
#endif
        if(uteModuleHeartData.type==TYPE_HEART)
        {
            uint8_t startCmdRsp[2]= {CMD_HEART_TEST,0x11};
            uteModuleProfileBleSendToPhone(&startCmdRsp[0],2);
        }
    }
}

/**
*@brief        结束单次测试
*@details
*@param ute_module_heart_type_t type 关闭类型
*@author       zn.zeng
*@date       2021-07-16
*/
void uteModuleHeartStopSingleTesting(ute_module_heart_type_t type)
{
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,type=%d", __func__, type);
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HEART_STOP_SINGLE_TESTING, (uint32_t)type);
}

// /**
// *@brief        结束单次测试消息处理函数
// *@details
// *@param      uint32_t param，结束类型
// *@author       zn.zeng
// *@date       2021-07-16
// */
void uteModuleHeartStopSingleTestingMsgHandler(uint32_t param)
{
    ute_module_heart_type_t type = (ute_module_heart_type_t)param;
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL,"%s,input type:%d, curr type:%d",__func__,type,uteModuleHeartData.type);
    /*当前测试类型与传入要停止的类型不一致时，不处理，xjc 2022-06-01*/
    if(uteModuleHeartData.type != type)
    {
        return;
    }
    if(uteModuleHeartData.type==TYPE_BLOODOXYGEN)
    {
        uteModuleHeartData.type = TYPE_HEART;
        uteModuleHeartData.isSingleTesting = false;
#if UTE_DRV_HEART_VCXX_SUPPORT
        uteDrvHeartVcxxStartSample();
#else
        // bsp_sensor_hr_init(WORK_MODE_HR);
        bsp_sensor_hr_stop();
#endif
    }
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
    else if(uteModuleHeartData.type==TYPE_EMOTION_PRESSURE)
    {
        uteModuleHeartData.type = TYPE_HEART;
        uteModuleHeartData.isSingleTesting = false;
#if UTE_DRV_HEART_VCXX_SUPPORT
        uteDrvHeartVcxxStartSample();
#endif
        Algo_Init();
    }
#endif
    else
    {
        if(uteModuleHeartData.type==TYPE_BREATHRATE)
        {
#if UTE_DRV_BREATHRATE_VCXX_SUPPORT
            uteDrvHeartVcxxStopBreathrate();
#else
            // bsp_sensor_hr_init(WORK_MODE_HR);
#endif
        }
        // if(uteModuleHeartData.isAutoTesting)
        // {
        //     uteModuleHeartData.type = TYPE_HEART;
        //     return;
        // }
        // if(!uteModuleHeartData.isSingleTesting)
        // {
        //     return;
        // }
        uteModuleHeartData.isSingleTesting = false;
        uteModuleHeartData.isAutoTestFlag = false;
        bool isWear = uteModuleHeartIsWear();
#if UTE_DRV_HEART_VCXX_SUPPORT
        uteDrvHeartVcxxStopSample();
#else
        // bsp_sensor_hr_init(WORK_MODE_HR);
        bsp_sensor_hr_stop();
#endif
        if(type==TYPE_HEART)
        {
            uint8_t stopCmd[5] = {CMD_HEART_TEST,0x00,0x00,0x00,0x00};
            stopCmd[3] =(uint8_t)uteModuleHeartData.heartValue;
            if(stopCmd[3]==0)
            {
                stopCmd[2] = 0xff;
            }
            if(isWear)
            {
                uteModuleProfileBleSendToPhone(&stopCmd[0],4);

                if(uteModuleHeartData.heartValue > 0) //&& uteModuleHeartData.isAutoTestFlag
                {
                    ute_module_systemtime_time_t time;
                    uteModuleSystemtimeGetTime(&time);
                    uint8_t everyTenMinReport[9];
                    everyTenMinReport[0] = CMD_HEART_24HOURS_AUTO_TEST;
                    everyTenMinReport[1] = 0x03;
                    everyTenMinReport[2] = time.year>>8&0xff;
                    everyTenMinReport[3] = time.year&0xff;
                    everyTenMinReport[4] = time.month;
                    everyTenMinReport[5] = time.day;
                    everyTenMinReport[6] = time.hour;
                    everyTenMinReport[7] = time.min/10;
                    everyTenMinReport[8] = (uint8_t)uteModuleHeartData.heartValue;
#if !DUG_VCXX_HEART_SUPPORT
                    uteModuleProfileBleSendToPhone(&everyTenMinReport[0],9);
#endif
                    /*!保存数据 zn.zeng, 2021-07-15  */
                    uteModuleHeartAutoSaveHeartData();
                    /*!保存心率最大最小平均值数据 xjc, 2022-01-17  */
                    uteModuleHeartSaveHeartMaxMinAvgData();
                }
            }
            else
            {
                uteModuleProfileBleSendToPhone(&stopCmd[0],2);
            }
        }
        else
        {
            uteModuleHeartData.type = TYPE_HEART;
        }
    }
}

/**
*@brief        是否佩戴
*@details
*@return true为佩戴
*@author       zn.zeng
*@date       2021-07-16
*/
bool uteModuleHeartIsWear(void)
{
    bool isWear= false;
#if UTE_DRV_HEART_VCXX_SUPPORT
    isWear= uteDrvHeartVcxxIsWear();
#else
    if(vc30fx_usr_get_work_status())
    {
        isWear= bsp_sensor_hr_wear_sta_get();
        uteModuleHeartData.lastIsWear = isWear;
    }
    else
    {
        isWear = uteModuleHeartData.lastIsWear;
        if(!uteModuleHeartData.isAutoTesting)
        {
            isWear= true;
            uteModuleHeartData.lastIsWear = isWear;
        }
    }
#endif
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL,"%s,isWear:%d",__func__,isWear);
    return isWear;
}
/**
*@brief        计算当天的平均心率数值
*@details
*@return uint8_t heart 传入心率值
*@author       zn.zeng
*@date       2021-07-16
*/
void uteModuleHeartAvgHeartOfCurrentDayProcess(uint8_t heart)
{
    // UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,heart=%d,.currentDayMinHeart=%d,currentDayMaxHeart=%d",__func__,heart, uteModuleHeartData.currentDayMinHeart,uteModuleHeartData.currentDayMaxHeart);
    if(heart!=0)
    {
        if(heart<uteModuleHeartData.currentDayMinHeart)
        {
            uteModuleHeartData.currentDayMinHeart = heart;
        }
        if(heart>uteModuleHeartData.currentDayMaxHeart)
        {
            uteModuleHeartData.currentDayMaxHeart = heart;
        }
        if(uteModuleHeartData.avgHeartCnt == 0)
        {
            uteModuleHeartData.currentDayAvgHeart = heart;
        }
        else
        {
            uteModuleHeartData.currentDayAvgHeart = ((float)(uteModuleHeartData.currentDayAvgHeart*uteModuleHeartData.avgHeartCnt)+heart)/(uteModuleHeartData.avgHeartCnt+1.0f);
        }
        uteModuleHeartData.avgHeartCnt++ ;
    }
}

/**
*@brief        清除当天的平均心率数值
*@details
*@return ute_module_systemtime_time_t *time 传入时间指针
*@author       zn.zeng
*@date       2021-07-16
*/
void uteModuleHeartClearAvgHeartOfCurrentDay(ute_module_systemtime_time_t *time)
{
    if((time->hour==0)&&(time->min==0)&&(time->sec==0))
    {
        uteModuleHeartData.currentDayMaxHeart = 0;
        uteModuleHeartData.currentDayMinHeart = 0xff;
        uteModuleHeartData.currentDayAvgHeart = 0;
        uteModuleHeartData.avgHeartCnt = 0;
    }
}
/**
*@brief        处理心率值警告数据
*@details
*@author       zn.zeng
*@date       2021-07-16
*/
void uteModuleHeartMinMaxWarningValueProcess(void)
{
#if UTE_MODULE_HEART_MIN_MAX_WARNING_VALUE_SUPPORT
    uint8_t heart = uteModuleHeartData.heartValue;
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL,".warning.isOpen = %d",uteModuleHeartData.warning.isOpen);
    if(uteModuleHeartIsWear()&&(heart>0)&&uteModuleHeartData.warning.isOpen&&(heart!=255)&&uteModuleHeartData.isAutoTesting)
    {
        if((heart<uteModuleHeartData.warning.setMinHeart)||(heart>uteModuleHeartData.warning.setMaxHeart))
        {
            uteModuleHeartData.warning.warningSecond++;
            if(uteModuleHeartData.warning.warningSecond>=UTE_MODULE_HEART_MIN_MAX_WARNING_FRIST_SECOND_CNT)
            {
                if((((uteModuleHeartData.warning.warningSecond-UTE_MODULE_HEART_MIN_MAX_WARNING_FRIST_SECOND_CNT)%UTE_MODULE_HEART_MIN_MAX_WARNING_RELOAD_SECOND_CNT)==0))
                {
#if UTE_MODULE_SCREENS_HEART_WARNING_SUPPORT
#if QUICK_SWITCH_HR_ABNORMAL_WARNNING_SUPPORT
                    //ute_quick_switch_t quick;
                    //uteApplicationCommonGetQuickSwitchStatus(&quick);
                    //if(quick.isHrAbnormalWarnning)
                    if(uteModuleHeartWaringOpenCtrl(0, 0))
#endif
                    {
                        uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_HEART_WARNING_SCREEN_ID);
                        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,5);
                    }
#endif
                    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,warningSecond=%d", __func__,uteModuleHeartData.warning.warningSecond);
                }
            }
        }
        else
        {
            uteModuleHeartData.warning.warningSecond = 0;
        }
    }
    else
    {
        uteModuleHeartData.warning.warningSecond = 0;
    }
#endif
}
/**
*@brief  保存心率报警范围参数信息
*@details
*@param[in] ute_module_heart_warning_t value
*@author        zn.zeng
*@date        2021-08-20
*/
void uteModuleHeartSaveHeartWaringInfo(ute_module_heart_warning_t *value)
{
#if UTE_MODULE_HEART_MIN_MAX_WARNING_VALUE_SUPPORT
    /*! 保存到文件zn.zeng, 2021-08-20*/
    void *file;
    uint8_t writebuff[3];

    if(value)memcpy(&uteModuleHeartData.warning,value,sizeof(ute_module_heart_warning_t));
    uteModuleHeartData.warning.warningSecond = 0;
    writebuff[0] = uteModuleHeartData.warning.isOpen;
    writebuff[1] = uteModuleHeartData.warning.setMinHeart;
    writebuff[2] = uteModuleHeartData.warning.setMaxHeart;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEARTWARNINGINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],3);
        uteModuleFilesystemCloseFile(file);
        //memcpy(&uteModuleHeartData.warning,&value,sizeof(ute_module_heart_warning_t));
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,isOpen=%d,min=%d,max=%d", __func__,value->isOpen,value->setMinHeart,value->setMaxHeart);
    }
#else
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,undinfe ", __func__);
#endif
}

/**
*@brief        获取血氧值
*@details
*@author       zn.zeng
*@date       2021-07-23
*/
uint8_t uteModuleHeartGetBloodOxygenValue(void)
{
#if UTE_DRV_HEART_VCXX_SUPPORT
    uteModuleHeartData.bloodoxygenValue = uteDrvHeartVcxxGetBloodoxygenValue();
#else
    uteModuleHeartData.bloodoxygenValue = bsp_sensor_spo2_data_get();
#endif
    return uteModuleHeartData.bloodoxygenValue;
}
/**
*@brief        获取呼吸率数值
*@details
*@author       zn.zeng
*@date       2021-08-16
*/
uint8_t uteModuleHeartGetBreathrateValue(void)
{
#if UTE_DRV_BREATHRATE_VCXX_SUPPORT
    return uteDrvHeartVcxxGetBreathrateValue();
#else
    return 0;
#endif
}
/**
*@brief        保存心率最大最小值跟平均值数据，每10分钟保存一次
*@details
*@author       xjc
*@date       2022-01-17
*/
void uteModuleHeartSaveHeartMaxMinAvgData(void)
{
    /*! 保存到文件，xjc, 2022-01-17*/
    void *file;
    uint8_t writebuff[3];
    writebuff[0] = uteModuleHeartData.currentDayMaxHeart;
    writebuff[1] = uteModuleHeartData.currentDayMinHeart;
    writebuff[2] = uteModuleHeartData.currentDayAvgHeart;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEART_MAX_MIN_AVG_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],3);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief        保存心率数据数据，每10分钟一条数据
*@details      每天保存一个文件，自动保存
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleHeartAutoSaveHeartData(void)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = 6*24+4;
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
    uint8_t path[40];
    memset(&readBuff[0],0xff,buffSize);
    memset(&path[0],0,40);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0],"%04d%02d%02d",time.year,time.month,time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR, dirInfo, NULL);
    if((dirInfo->filesCnt>=UTE_MODULE_HEART_SAVE_DATA_MAX_DAYS)&&(memcmp(&path[0],&dirInfo->filesName[0][0],8)!=0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-08-25*/
        memset(&path[0],0,40);
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR,&dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0],0,40);
    sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR,time.year,time.month,time.day);
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,save file=%s", __func__,&path[0]);
    /*! 保存当前数据zn.zeng, 2021-08-30  */
    void *file;
    // read
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    else
    {
        readBuff[0] = time.year>>8&0xff;
        readBuff[1] = time.year&0xff;
        readBuff[2] = time.month;
        readBuff[3] = time.day;
    }
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uint16_t cacheOffset = 0;
        cacheOffset = 4+(time.hour*60+time.min)/10;
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,cacheOffset=%d", __func__,cacheOffset);
        readBuff[cacheOffset] = uteModuleHeartData.heartValue;
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(readBuff);
    uteModulePlatformMemoryFree(dirInfo);
}
/**
*@brief        开始发送24小时心率数据
*@details
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleHeartSendHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[30];
    uint8_t response[20];
    uint8_t sendSize = 0;
    response[0] = CMD_HEART_24HOURS_AUTO_TEST;
    memset(&path[0],0,30);
    uint8_t *tempDataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&tempDataBuff[0],0,sendParam->dataBuffSize);
    memset(&dataBuff[0],0,sendParam->dataBuffSize);
    if(sendParam->dirInfo.filesCnt>0)
    {
    SEND_NEXT_DATA:
        sendSize = 18;
        bool isHasData = false;
        void *file;
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR,&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,read file=%s", __func__,&path[0]);
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,currSendFileIndex=%d,currSendMinIndex=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendMinIndex);
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
            memcpy(&dataBuff[0], tempDataBuff, 4);      /*!获取年月日 , xjc 2022-03-03*/
            memcpy(&dataBuff[4], &tempDataBuff[5], 143); /*!获取当天00：10~23：50的数据, xjc 2022-03-03*/
            uteModuleFilesystemCloseFile(file);
        }
        if ((sendParam->currSendFileIndex + 1) < sendParam->dirInfo.filesCnt)
        {
            memset(&tempDataBuff[0],0,sendParam->dataBuffSize);
            sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex + 1][0]);
            if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
            {
                uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
                memcpy(&dataBuff[sendParam->dataBuffSize - 1], &tempDataBuff[4], 1); /*!获取后一天00：00的数据, xjc 2022-03-03*/
                uteModuleFilesystemCloseFile(file);
            }
        }
        uint16_t invalidMin = 0;
        for(uint16_t i=sendParam->currSendMinIndex; i<144;)
        {
            if((memcmp(&dataBuff[4+i],"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",12)==0)||(memcmp(&dataBuff[4+i],"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",12)==0))
            {
                invalidMin=invalidMin+12;
                i = i+12;
            }
            else
            {
                uint8_t sendHour = i/6+2;
                if(sendHour==24)
                {
                    ute_module_systemtime_time_t timeAdd;
                    timeAdd.year = dataBuff[0]<<8|dataBuff[1];
                    timeAdd.month = dataBuff[2];
                    timeAdd.day = dataBuff[3];
                    uteModuleSystemtimeInputDateCalDay(&timeAdd,1);
                    dataBuff[0] = (timeAdd.year>>8)&0xff;
                    dataBuff[1] = (timeAdd.year)&0xff;
                    dataBuff[2] = timeAdd.month;
                    dataBuff[3] = timeAdd.day;
                    sendHour = 0;
                }
                memcpy(&response[1],&dataBuff[0],4);//year month day
                response[5] = sendHour;
                memcpy(&response[6],&dataBuff[4+i],12);
                invalidMin=invalidMin+12;
                i = i+12;
                isHasData = true;
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,invalidMin=%d", __func__,invalidMin);
        sendParam->currSendMinIndex+=invalidMin;
        if(sendParam->currSendMinIndex>=144)
        {
            sendParam->currSendFileIndex++;
            if(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt)
            {
                sendParam->currSendMinIndex = 0;
            }
            else
            {
                sendParam->currSendMinIndex=144;
            }
        }
        if((!isHasData)&&(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt))
        {
            goto SEND_NEXT_DATA;
        }
    }
    else
    {
        sendParam->currSendMinIndex=144;
    }
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex,sendParam->dirInfo.filesCnt);
    if((sendParam->currSendFileIndex==sendParam->dirInfo.filesCnt)
       &&(sendParam->currSendMinIndex==144))
    {
        sendSize = 3;
        response[1] = 0xfd;
        response[2] = sendParam->crc;
        uteApplicationCommonSyncDataTimerStop();
    }
    if(sendSize==18)
    {
        uteModuleCrc8Bit(&sendParam->crc, &response[1],17);
    }
    uteModulePlatformMemoryFree(tempDataBuff);
    uteModulePlatformMemoryFree(dataBuff);
    uteModuleProfileBleSendToPhone(&response[0],sendSize);
}
/**
*@brief        准备开始发送心率数据
*@details
*@param[in]  需要同步的数据时间戳
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleHeartStartSendHeartAutoTestHistoryData(ute_module_systemtime_time_t time)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->time = time;
    if(param->time.year==0)
    {
        param->isAllData = true;
    }
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    param->currSendMinIndex = 0;
    param->dataBuffSize = 6*24+4;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleHeartSendHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,filesCnt=%d", __func__,param->dirInfo.filesCnt);

}
/**
*@brief        设置保存自动测试模式
*@details
*@param[in]  bool isAutoTesting
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleHeartSetAutoTesting(bool isAutoTesting)
{
    uteModuleHeartData.isAutoTesting = isAutoTesting;
    /*! 保存到文件zn.zeng, 2021-08-30*/
    void *file;
    uint8_t writebuff[1];
    writebuff[0] = uteModuleHeartData.isAutoTesting;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEART_SET_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isHeart24h = uteModuleHeartData.isAutoTesting;
    uteApplicationCommonSetQuickSwitchStatus(&quick);
}

/**
*@brief        心率关机
*@details
*@author       zn.zeng
*@date       2021-08-30
*/
void uteModuleHeartPowerOff(void)
{
#if UTE_DRV_HEART_VCXX_SUPPORT
#if UTE_DRV_HEART_VC31B_VC32S_VC51B_VC52S_SUPPORT
    uteDrvHeartVcxxCommonFunction->reducePsSampleToSavePower(false);
#else
    uteDrvHeartVcxxStopSample();
#endif
    uteDrvHeartVcxxSetPowerEnable(false);
#else
    // vc30fx_usr_stop_work();
    // vc30fx_pwr_dis();
#endif
}

/**
*@brief        获取工作模式
*@details
*@author       xjc
*@date       2022-03-14
*/
uint8_t uteModuleHeartGetWorkMode(void)
{
    uint8_t result = 0;
#if UTE_DRV_HEART_VCXX_SUPPORT
    result = uteDrvHeartVcxxGetWorkMode();
#else
    result = vc30fx_usr_get_work_mode();
#endif
    return result ;
}

/**
*@brief 获取心率等级范围
*@details 判断心率区间的范围，一共6个等级
*@return 返回值  1表示静息状态
                2表示减压状态
                3表示燃脂状态
                4表示心肺状态
                5表示无氧状态
                0表示极限状态
*@author       xjc
*@date       2021-12-23
*/
uint8_t uteModuleHeartGetHeartValueRange(uint8_t heart)
{
    uint8_t rangeIndex = 0;
    uint8_t range[5],maxHeat;
    ute_personal_info_t info;
    uteApplicationCommonGetPersonalInfo(&info);
    maxHeat = 220-info.age;
    range[0] = (uint8_t)(maxHeat*0.5f);
    range[1] = (uint8_t)(maxHeat*0.6f);
    range[2] = (uint8_t)(maxHeat*0.7f);
    range[3] = (uint8_t)(maxHeat*0.8f);
    range[4] = (uint8_t)(maxHeat*0.9f);
    for(uint8_t i=0; i<5; i++)
    {
        if(heart<=range[i])
        {
            rangeIndex = i+1;
            break;
        }
    }
    return rangeIndex;
}

/**
*@brief        获取心率预警最大值
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
uint8_t uteModuleHeartGetHeartWaringMaxValue(void)
{
    return uteModuleHeartData.warning.setMaxHeart;
}
/**
*@brief        获取心率预警最小值
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
uint8_t uteModuleHeartGetHeartWaringMinValue(void)
{
    return uteModuleHeartData.warning.setMinHeart;
}

bool uteModuleHeartWaringOpenCtrl(bool isSet,bool isOpen)
{
    if(isSet)
    {
        uteModuleHeartData.warning.isOpen=isOpen;
        uteModuleHeartSaveHeartWaringInfo(NULL);
    }

    return uteModuleHeartData.warning.isOpen;
}
void uteModuleHeartWaringOpenSwitch(void)
{
    uteModuleHeartData.warning.isOpen=!uteModuleHeartData.warning.isOpen;
    uteModuleHeartSaveHeartWaringInfo(NULL);
}

/**
*@brief      处理加载当天心率历史数据，用于心率柱状图显示
*@details    当天有数据返回true,没有则返回false
*@author     xjc
*@date       2021-12-22
*/
static bool uteModuleHeartLoadTodayHistoryData(uint8_t *heartHistoryGraph, uint8_t heartHistoryGraphCount, uint8_t *heartHistoryData, uint8_t heartHistoryDataLen)
{
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,heartHistoryGraphCount=%d", __func__, heartHistoryGraphCount);
#if UTE_LOG_GUI_LVL // test
    for (uint8_t i = 0; i < 144; i++)
    {
        heartHistoryData[i] = 50 + get_random(150);
    }
#endif

    /*! 心率历史数据每隔10分钟显示 xjc, 2022-03-21  */
#if UTE_HEART_HISTOTY_DISPLAY_EACH_20_MIN_SUPPORT
    uint8_t tempHeartHistoryData[72];
    memset(tempHeartHistoryData, 0, sizeof(tempHeartHistoryData));
    for (uint8_t i = 0; i < 72; i++)
    {
        tempHeartHistoryData[i] = heartHistoryData[i * 2];
    }
    memcpy(heartHistoryGraph, tempHeartHistoryData, 72);
#elif APP_HEART_HISTORY_EACH_HOUR_AVERAGE_VALUE_SUPPORT // 24小时，每个小时的平均值显示，共24条,包括预留24小时的最大和最小值计算
    uint8_t tempHeartHistoryData[144];
    uint8_t m = 144 / heartHistoryGraphCount;
    memset(tempHeartHistoryData, 0, sizeof(tempHeartHistoryData));
    for (uint8_t i = 0; i < heartHistoryGraphCount; i++)
    {
        uint8_t heart = 0;
        uint16_t HeartValidValue = 0; // 总心率值
        uint8_t averageHeartvalueCnt = 0;
        for (int j = m; j > 0; j--)
        {
            heart = heartHistoryData[i * m + (j - 1)];
            if ((heart != 255) && (heart != 0))
            {
                HeartValidValue += heart;
                averageHeartvalueCnt++;
                //                UTE_MODULE_LOG(1, "%s,j=%d,heart=%d,HeartValidValue=%d,averageHeartvalueCnt=%d", __func__,j,heart,HeartValidValue,averageHeartvalueCnt);
            }
        }
        tempHeartHistoryData[i] = HeartValidValue / averageHeartvalueCnt;
        //              UTE_MODULE_LOG(1, "%s,33333 i=%d,tempHeartHistoryData[i]=%d,HeartValidValue=%d,averageHeartvalueCnt=%d", __func__,i,tempHeartHistoryData[i],HeartValidValue,averageHeartvalueCnt);
    }
    memcpy(heartHistoryGraph, tempHeartHistoryData, heartHistoryGraphCount);
#else
    uint8_t tempHeartHistoryData[144];
    uint8_t m = 144 / heartHistoryGraphCount;
    memset(tempHeartHistoryData, 0, sizeof(tempHeartHistoryData));
    for (uint8_t i = 0; i < heartHistoryGraphCount; i++)
    {
        uint8_t heart = 0;
        for (int j = m; j > 0; j--)
        {
            heart = heartHistoryData[i * m + (j - 1)];
            if ((heart != 255) && (heart != 0))
            {
                break;
            }
        }
        if(heart == 0xff)
        {
            tempHeartHistoryData[i] = 0;
        }
        else
        {
            tempHeartHistoryData[i] = heart;
        }
    }
    memcpy(heartHistoryGraph, tempHeartHistoryData, heartHistoryGraphCount);
#endif

    /*! 心率最大最小值数字显示跟随柱状图中的最大最小值 xjc, 2021-12-23  */
#if UTE_HEART_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
    uteModuleHeartData.avgHeartCnt = 0;
    uteModuleHeartData.currentDayMinHeart = 255;
    uteModuleHeartData.currentDayMaxHeart = 0;
    for (uint8_t i = 0; i < heartHistoryGraphCount; i++)
    {
        if (heartHistoryGraph[i] != 0 && heartHistoryGraph[i] != 0xFF)
        {
            uteModuleHeartAvgHeartOfCurrentDayProcess(heartHistoryGraph[i]);
        }
    }
    /*! 同步手环和手机端的最大最小平均值 xjc, 2021-12-23  */
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uteModuleHeartSyncMaxMinAvgHeartValue(time);
#endif

    return false;
}

/**
*@brief      获取当天心率历史数据，用于心率柱状图显示
*@details    当天有数据返回true,没有则返回false
*@author     xjc
*@date       2021-12-22
*/
bool uteModuleHeartGetTodayHistoryData(uint8_t *heartHistoryGraph, uint8_t heartHistoryGraphCount)
{
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s", __func__);
    void *file;
    uint8_t path[40];
    uint8_t *heartHistoryData = (uint8_t *)uteModulePlatformMemoryAlloc(144); //一天24h的数据，十分钟一条，一共144条
    ute_module_systemtime_time_t time;
    memset(heartHistoryData, 0, 144);
    memset(heartHistoryGraph,0, heartHistoryGraphCount);
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,read file=%s", __func__, &path[0]);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,4,FS_SEEK_SET);
        uteModuleFilesystemReadData(file, heartHistoryData, 144);
        uteModuleFilesystemCloseFile(file);
        uteModuleHeartLoadTodayHistoryData(heartHistoryGraph, heartHistoryGraphCount, heartHistoryData, 144);
        uteModulePlatformMemoryFree(heartHistoryData);
        return true;
    }
    else
    {
#if UTE_LOG_GUI_LVL // test
        uteModuleHeartLoadTodayHistoryData(heartHistoryGraph, heartHistoryGraphCount, heartHistoryData, 144);
        uteModulePlatformMemoryFree(heartHistoryData);
#endif
        return false;
    }
}

#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
/**
*@brief     读取运动心率预警数据
*@details
*@author        dengli.lu
*@date        2022-03-08
*/
void uteModuleHeartReadSportHeartWaringInfo(void)
{
    /*! 读取数据*/
    void *file;
    uint8_t readbuff[4];
    memset(&readbuff[0],0,sizeof(readbuff));
    readbuff[0] = DEFAULT_SPORT_HEARTWARNING_MAX_VALUE;
    readbuff[1] = DEFAULT_SPORT_MAX_HEARTWARNING_IS_OPEN;
    readbuff[2] = DEFAULT_SPORT_HEARTWARNING_MIN_VALUE;
    readbuff[3] = DEFAULT_SPORT_MIN_HEARTWARNING_IS_OPEN;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEART_SPORTS_WARNING_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],4);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleHeartData.sportWarning.maxHeartValue = readbuff[0];
    uteModuleHeartData.sportWarning.isMaxHeartOpen = readbuff[1];
    uteModuleHeartData.sportWarning.minHeartValue = readbuff[2];
    uteModuleHeartData.sportWarning.isMinHeartOpen = readbuff[3];
}
/**
*@brief        保存当运动心率预警的数据
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleHeartSaveSportHeartWaringInfo(void)
{
    void *file;
    uint8_t writeBuff[4];
    memset(&writeBuff[0],0,sizeof(writeBuff));
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_HEART_SPORTS_WARNING_INFO, &file, FS_O_RDONLY|FS_O_WRONLY|FS_O_CREAT))
    {
        writeBuff[0] = uteModuleHeartData.sportWarning.maxHeartValue;
        writeBuff[1] = uteModuleHeartData.sportWarning.isMaxHeartOpen;
        writeBuff[2] = uteModuleHeartData.sportWarning.minHeartValue;
        writeBuff[3] = uteModuleHeartData.sportWarning.isMinHeartOpen;
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writeBuff[0],4);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief        设置运动心率预警的数据
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleHeartSetSportHeartWaringInfo(uint8_t maxHeartValue,bool isMaxHeartOpen,uint8_t minHeartValue,bool isMinHeartOpen)
{
    uteModuleHeartData.sportWarning.maxHeartValue = maxHeartValue;
    uteModuleHeartData.sportWarning.isMaxHeartOpen = isMaxHeartOpen;
    uteModuleHeartData.sportWarning.minHeartValue = minHeartValue;
    uteModuleHeartData.sportWarning.isMinHeartOpen = isMinHeartOpen;
    uteModuleHeartSaveSportHeartWaringInfo();
}
/**
*@brief        获取运动心率预警最大值
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
uint8_t uteModuleHeartGetSportHeartWaringMaxValue(void)
{
    return uteModuleHeartData.sportWarning.maxHeartValue;
}
/**
*@brief        获取运动心率最大预警开关状态
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleHeartGetSportHeartWaringMaxIsOpen(void)
{
    return uteModuleHeartData.sportWarning.isMaxHeartOpen;
}
/**
*@brief        获取运动心率预警最小值
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
uint8_t uteModuleHeartGetSportHeartWaringMinValue(void)
{
    return uteModuleHeartData.sportWarning.minHeartValue;
}
/**
*@brief        获取运动心率最小预警开关状态
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
bool uteModuleHeartGetSportHeartWaringMinIsOpen(void)
{
    return uteModuleHeartData.sportWarning.isMinHeartOpen;
}
/**
*@brief        设置运动心率预警秒数
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
void uteModuleHeartSetSportHeartWaringSecond(uint32_t warningSecond)
{
    uteModuleHeartData.sportWarning.warningSecond = warningSecond;
}
/**
*@brief        获取运动心率最小预警秒数
*@details
*@author      dengli.lu
*@date       2022-03-08
*/
uint32_t uteModuleHeartGetSportHeartWaringSecond(void)
{
    return uteModuleHeartData.sportWarning.warningSecond;
}
#endif
/**
*@brief       系统时间变化
*@details
*@author      zn.zeng
*@date       2022-09-04
*/
void uteModuleHeartSystemtimeChange(ute_module_systemtime_time_t curr,ute_module_systemtime_time_t newSet)
{
#if UTE_MODULE_HEART_WEEK_STATIC_SUPPORT
    if ((curr.year != newSet.year) || (curr.month != newSet.month) || (curr.day != newSet.day))
    {
        uteModuleHeartrReadStaticHeartData();
    }
#endif
}
#if UTE_MODULE_HEART_WEEK_STATIC_SUPPORT
/**
*@brief       静息心率处理
*@details
*@author      zn.zeng
*@date       2022-09-04
*/
void uteModuleHeartWeekStaticSecond(ute_module_systemtime_time_t time)
{
    if (time.sec==59)
    {
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,uteModuleSleepCurrDayIsHasSleep()=%d", __func__,uteModuleSleepCurrDayIsHasSleep());
        if (uteModuleSleepCurrDayIsHasSleep())
        {
            uint32_t totalSecond = time.hour*3600+time.min*60+time.sec;
            UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,uteModuleSleepCurrDayIsGetUp()=%d,totalSecond=%d,.dayStaticMin=%d", __func__,uteModuleSleepCurrDayIsGetUp(),totalSecond,uteModuleHeartData.dayStaticMin);
            if (uteModuleSleepCurrDayIsGetUp() ||((totalSecond>41400)&&(totalSecond<43200)))
            {
                if(uteModuleHeartData.dayStaticMin==0)
                {
                    uteModuleHeartData.dayStaticHeart = 255;
                }
                UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,.dayStaticHeart=%d,heartValue=%d", __func__,uteModuleHeartData.dayStaticHeart,uteModuleHeartData.heartValue);
                if (uteModuleHeartData.dayStaticMin < 30)
                {
                    uteModuleHeartData.dayStaticMin++;
                    if ((uteModuleHeartData.heartValue!=0&&uteModuleHeartData.heartValue!=0xff))//&&(uteModuleHeartData.heartValue<uteModuleHeartData.dayStaticHeart)
                    {
                        uteModuleHeartData.dayStaticHeart = uteModuleHeartData.heartValue;
                    }
                }
                else if(uteModuleHeartData.dayStaticMin == 30)
                {
                    uteModuleHeartData.dayStaticMin++;
                    if (uteModuleHeartData.isAutoTesting)
                    {
                        if (uteModuleHeartData.dayStaticHeart == 255)
                        {
                            uteModuleHeartData.weekDayStaticHeart[6] = 0;
                        }
                        else
                        {
                            uteModuleHeartData.weekDayStaticHeart[6] = uteModuleHeartData.dayStaticHeart;
                        }
                        if(uteModuleHeartData.weekDayStaticHeart[6]>120)
                        {
                            uteModuleHeartData.weekDayStaticHeart[6] = 120;
                        }
                    }
                    else
                    {
                        uteModuleHeartData.weekDayStaticHeart[6] = 70+rand()%10;
                    }
                    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,.weekDayStaticHeart[6]=%d", __func__,uteModuleHeartData.weekDayStaticHeart[6]);
                    //save
                    uteModuleHeartrSaveStaticHeartData();
#if APP_MODULE_HEART_RESTING_HEARTRATE_SUPPORT
                    uteModuleHeartRestingHeartrateSend();
#endif
                }
            }
            else
            {
                uteModuleHeartData.dayStaticMin = 0;
            }
        }
        else
        {
            uteModuleHeartData.dayStaticMin = 0;
            uteModuleHeartData.dayStaticHeart = 0;
        }
    }
    //过零点之后再去读
    if((time.hour==0)&&(time.min==0)&&time.sec==02)
    {
        uteModuleHeartrReadStaticHeartData();
    }
}
/**
*@brief     加载周每天的静息心率柱状图
*@details
*@param[in] weekDayStaticHeartGraph
*@param[in] color 颜色
*@param[in] x开始坐标
*@param[in] y开始坐标
*@param[in] drawWidth 宽度
*@param[in] intervalWidth 之间的间隔
*@param[in] hightRange 对应的像素高度
*@author      zn.zeng
*@date       2022-09-04
*/
void uteModuleHeartLoadWeekDayStaticHeartData(UT_GraphsParam *weekDayStaticHeartGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    memset(weekDayStaticHeartGraph, 0, sizeof(UT_GraphsParam) * 7);
#if UTE_LOG_GUI_LVL
    for (uint8_t i = 0; i < 7; i++)
    {
        uteModuleHeartData.weekDayStaticHeart[i] = 80+rand()%20;
    }
#endif
    for (uint8_t i = 0; i < 7; i++)
    {
        uint8_t heart = uteModuleHeartData.weekDayStaticHeart[i];
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,heart=%d", __func__,heart);
        weekDayStaticHeartGraph[i].colorData = color;
        weekDayStaticHeartGraph[i].width = drawWidth;
        weekDayStaticHeartGraph[i].hight = heart*hightRange / 120;
        weekDayStaticHeartGraph[i].x = x + (drawWidth + intervalWidth) * i;
        weekDayStaticHeartGraph[i].y = y - weekDayStaticHeartGraph[i].hight;
    }
}
/**
*@brief       读取静息心率历史数值
*@details
*@author      zn.zeng
*@date       2022-09-15
*/
void uteModuleHeartrReadStaticHeartData(void)
{
    void *file;
    uint8_t path[30];
    ute_module_systemtime_time_t time, readTime;
    uteModuleSystemtimeGetTime(&time);
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,%04d%02d%02d,week=%d", __func__, time.year, time.month, time.day, time.week);
    memset(&uteModuleHeartData.weekDayStaticHeart[0], 0, 7);
    for (uint8_t i = 0; i < 7; i++)
    {
        memcpy(&readTime, &time, sizeof(ute_module_systemtime_time_t));
        uteModuleSystemtimeInputDateIncreaseDecreaseDay(&readTime, i - 6);
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,readTime,%04d%02d%02d,week=%d", __func__, readTime.year, readTime.month, readTime.day, readTime.week);
        memset(&path[0], 0, 30);
        sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_DIR, readTime.year, readTime.month, readTime.day);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &uteModuleHeartData.weekDayStaticHeart[i], 1);
            uteModuleFilesystemCloseFile(file);
        }
        else
        {
            uteModuleHeartData.weekDayStaticHeart[i] = 0;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,finish,weekDayStaticHeart:", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_HEART_LVL,&uteModuleHeartData.weekDayStaticHeart[0],7);
}
/**
*@brief       保存静息心率历史数值
*@details
*@author      zn.zeng
*@date       2022-09-15
*/
void uteModuleHeartrSaveStaticHeartData(void)
{
    void *file;
    uint8_t path[30];
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0],"%04d%02d%02d",time.year,time.month,time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_DIR, dirInfo, NULL);
    if((dirInfo->filesCnt>=UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_MAX_FILE)&&(memcmp(&path[0],&dirInfo->filesName[0][0],8)!=0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2022-09-15*/
        memset(&path[0],0,30);
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_DIR,&dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0],0,30);
    sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_DIR,time.year,time.month,time.day);
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,save file=%s", __func__,&path[0]);
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&uteModuleHeartData.weekDayStaticHeart[6],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(dirInfo);
}
/**
 *@brief       获取当天静息心率
 *@details
 *@author      zn.zeng
 *@date       2022-09-15
 */
uint8_t uteModuleHeartrGetCurrDayStaticHeartData(void)
{
    return uteModuleHeartData.weekDayStaticHeart[6];
}
/**
 *@brief       获取最近7天平均静息心率
 *@details
 *@author      zn.zeng
 *@date       2022-09-15
 */
uint8_t uteModuleHeartrGetLast7DayAvgStaticHeartData(void)
{
    uint8_t i=0;
    uint8_t vail = 0;
    uint16_t sum = 0;
    uint8_t avg = 0;
    for(i=0; i<7; i++)
    {
        if ((uteModuleHeartData.weekDayStaticHeart[i] != 0)&&(uteModuleHeartData.weekDayStaticHeart[i] != 255))
        {
            sum = sum + uteModuleHeartData.weekDayStaticHeart[i];
            vail++;
        }
    }
    avg = sum/vail;
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,sum=%d,vail=%d,avg=%d", __func__,sum,vail,avg);
    return avg;
}
#endif

#if APP_MODULE_HEART_RESTING_HEARTRATE_SUPPORT
/**
 *@brief       发送静息心率
 *@details
 *@author      wuhuowang
 *@date       2022-12-01
 */
void uteModuleHeartRestingHeartrateSend(void)
{
    uint8_t restingHRReport[9] = {0};
    ute_module_systemtime_time_t time;
    uint8_t staticHRValue = 0;

    staticHRValue = uteModuleHeartrGetCurrDayStaticHeartData();
    //需要给APP返回值 屏蔽这个判断
//    if (staticHRValue > 0 && staticHRValue != 255)
    {
        uteModuleSystemtimeGetTime(&time);
        restingHRReport[0] = CMD_HEART_24HOURS_AUTO_TEST;
        restingHRReport[1] = 0x05;
        restingHRReport[2] = time.year >> 8 & 0xff;
        restingHRReport[3] = time.year & 0xff;
        restingHRReport[4] = time.month;
        restingHRReport[5] = time.day;
        restingHRReport[6] = time.hour;
        restingHRReport[7] = time.min;
        restingHRReport[8] = staticHRValue;
        uteModuleProfileBleSendToPhone(&restingHRReport[0], 9);
    }
}
/**
 *@brief       获取指定小时最大最小心率
 *@details
  *@param[in] uint8_t  第几小时
 *@author      wuhuowang
 *@date       2022-12-01
 */
void uteModuleHeartCalcEveryHourMaxMInHeartData(uint8_t hour)
{
    uint8_t path[30];
    uint16_t i = 0;
    void *file;
    ute_module_systemtime_time_t time;
    uint8_t *tempDataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(12 * 24 + 4);

    memset(&tempDataBuff[0], 0, 12 * 24 + 4);
    uteModuleSystemtimeGetTime(&time);
    memset(&path[0], 0, 30);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_HEART_AUTO_DATA_DIR, time.year, time.month, time.day);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file, &tempDataBuff[0], 12 * 24 + 4);
        uteModuleFilesystemCloseFile(file);
    }
    for (i = 12 * hour + 4; i < 12 * hour + 16; i ++)
    {
        if (tempDataBuff[i] == 0 || tempDataBuff[i] == 0xFF)
        {
        }
        else
        {
            if (tempDataBuff[i] > uteModuleHeartData.maxHourheartValue)
                uteModuleHeartData.maxHourheartValue = tempDataBuff[i];
            if (uteModuleHeartData.minHourheartValue == 0)
                uteModuleHeartData.minHourheartValue = tempDataBuff[i];
            else if (uteModuleHeartData.minHourheartValue < tempDataBuff[i])
                uteModuleHeartData.minHourheartValue = tempDataBuff[i];
        }
    }
    uteModulePlatformMemoryFree(tempDataBuff);
}
/**
 *@brief       发送每小时最大最小心率
 *@details
 *@param[in] uint8_t  第几小时
 *@author      wuhuowang
 *@date       2022-12-01
 */
void uteModuleHeartSendEveryHourMaxMInHeartData(uint8_t hour)
{
    uint8_t restingHRReport[9] = {0};
    ute_module_systemtime_time_t time;

    uteModuleHeartData.maxHourheartValue = 0;
    uteModuleHeartData.minHourheartValue = 0;
    uteModuleHeartCalcEveryHourMaxMInHeartData(hour);
    uteModuleSystemtimeGetTime(&time);
    restingHRReport[0] = CMD_HEART_24HOURS_AUTO_TEST;
    restingHRReport[1] = 0x07;
    restingHRReport[2] = (time.year >> 8) & 0xff;
    restingHRReport[3] = time.year & 0xff;
    restingHRReport[4] = time.month;
    restingHRReport[5] = time.day;
    restingHRReport[6] = hour;
    restingHRReport[7] = uteModuleHeartData.maxHourheartValue;
    restingHRReport[8] = uteModuleHeartData.minHourheartValue;
    uteModuleProfileBleSendToPhone(&restingHRReport[0], 9);
}
/**
 *@brief       发送历史静息心率
 *@details
 *@author      wuhuowang
 *@date       2022-12-01
 */
void uteModuleHeartSendHistoryRestingHeartData(void)
{
    uint8_t restingHRReport[20] = {0};
    void *file;
    uint8_t path[30];
    ute_module_systemtime_time_t time, readTime;
    bool hasData = false;
    uint8_t sendLen = 0;
    uint8_t i = 0;
    uint8_t weekDayStaticHeart[1] = {0};
    uteModuleSystemtimeGetTime(&time);
    restingHRReport[sendLen ++] = CMD_HEART_24HOURS_AUTO_TEST;
    restingHRReport[sendLen ++] = 0x08;
    restingHRReport[sendLen ++] = 0x00;
    restingHRReport[sendLen ++] = 0x01;
    for (i = 6; i > 0; i --)
    {
        memcpy(&readTime, &time, sizeof(ute_module_systemtime_time_t));
        uteModuleSystemtimeInputDateIncreaseDecreaseDay(&readTime, i - 6);
        memset(&path[0], 0, 30);
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,year=%d,month=%d,day=%d", __func__,readTime.year,readTime.month,readTime.day);
        sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_WEEK_DAY_STATIC_HEART_DIR, readTime.year, readTime.month, readTime.day);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &weekDayStaticHeart[0], 1);
            uteModuleFilesystemCloseFile(file);
            hasData = true;
            restingHRReport[sendLen ++] = (readTime.year >> 8) & 0xFF;
            restingHRReport[sendLen ++] = readTime.year & 0xFF;
            restingHRReport[sendLen ++] = readTime.month;
            restingHRReport[sendLen ++] = readTime.day;
            restingHRReport[sendLen ++] = weekDayStaticHeart[0];
            if (sendLen >= 19)
                break;
        }
    }
    if (hasData == true)
    {
        uteModuleProfileBleSendToPhone(&restingHRReport[0], sendLen);
        uteModulePlatformDelayMs(50);
        memset(restingHRReport, 0, sizeof(restingHRReport));
        restingHRReport[0] = CMD_HEART_24HOURS_AUTO_TEST;
        restingHRReport[1] = 0x08;
        restingHRReport[2] = 0xFD;
        uteModuleProfileBleSendToPhone(&restingHRReport[0], 3);
    }
    else
    {
        memset(restingHRReport, 0, sizeof(restingHRReport));
        restingHRReport[0] = CMD_HEART_24HOURS_AUTO_TEST;
        restingHRReport[1] = 0x08;
        restingHRReport[2] = 0xFD;
        uteModuleProfileBleSendToPhone(&restingHRReport[0], 3);
    }
}
#endif

#endif /* UTE_MODULE_HEART_SUPPORT */


