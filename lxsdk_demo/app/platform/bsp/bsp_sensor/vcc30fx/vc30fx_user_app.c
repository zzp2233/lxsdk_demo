/**********************************************************************************************
 * @Copyright(c) 2023, Chengdu vcaresensor Microelectronics Corporation. All rights reserved.
 * @Author: Tse
 * @Date: 2023-06-09 11:57:36
 * @LastEditTime: 2024-01-19 15:07:52
 * @LastEditors: Tse
 * @Description:
 **********************************************************************************************/
#if (SENSOR_HR_SEL == SENSOR_HR_VC30FX)
#if UTE_MODULE_HEART_SUPPORT
/* 驱动头文件 */
#include "vc30fx_driver_com.h"
#include "vc30fx_sample_result.h"
#include "vc30fx_user_app.h"
/* 算法头文件 */
#include "algo.h"
#include "spo2Algo_16bit.h"

#include "include.h"
#include "ute_module_sport.h"
#include "ute_drv_gsensor_common.h"
#include "ute_module_message.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_factorytest.h"

#include "vc30fx_driver.h"
extern vc30fx_clk_info clk_info;            /* oscclk_calibration_infomation */

// #include "vcSportMotionIntAlgo.h"

/* ble_debug,蓝牙发送原始数据 */
/* vcare_ppg_device */
static vcare_ppg_device_t vc30fx_dev = {"vc30fx_sc", 0};
/* G-Sensor Data */
static gsensor_axes vc30fx_gsensor_data = {0};

InitParamTypeDef vc30fx_data = {600, 0};

static uint32_t hw_timer_count = 0;

// InitParamTypeDef vc30fx_data = {400, WORK_MODE_HR};

#if DUG_VCXX_HEART_SUPPORT
extern void vc30fx_send_orginal_data(vcare_ppg_device_t *pdev, gsensor_axes *pgsensor, unsigned char ppgsize);
#endif

static void vc30fx_get_board_gsensor_data(unsigned short int *pgsensor_len, unsigned short int ppg_count);
static unsigned int vc30fx_gsensor_actuating_quantity(int x_axis, int y_axis, int z_axis);
static int vc30fx_heart_rate_calculate(vcare_ppg_device_t *dev);
static int vc30fx_spo2_calculate(vcare_ppg_device_t *dev);

#define VC30FS_WRITE_ADDR(ADDR)         ((ADDR) << 1)
#define VC30FS_READ_ADDR_UPDATE(ADDR)   ((ADDR) << 1 | 1)
#define VC30FS_READ_ADDR(ADDR)          ((ADDR) << 1 | 1) << 8 | ((ADDR) << 1)
#define VC30FS_IIC_7BITS_ADDR            0x33

/****************************************************************************
 * @description: 调试信息
 * @param {unsigned int} info
 * @param {char} *fmt
 * @return {*}
 ****************************************************************************/
extern void log_direct(unsigned int info, const char *fmt, ...);
int (*vc30fx_dbglog_user)(const char *, ...) = NULL;

void vc30fx_pwr_en(void)        //PF5
{
    if(vc30fx_clk_calc_status() == 0)
    {
        uteModulePlatformDlpsDisable(UTE_MODULE_PLATFORM_DLPS_BIT_HEART); //禁用睡眠，睡眠下无法测量
    }

    // if(!vc30fx_dev.dev_work_status)
    // {
    //     uteModulePlatformOutputGpioSet(IO_PF5,true);
    // }

#if (SENSOR_STEP_SEL != SENSOR_STEP_NULL)
    sc7a20_500ms_callback_en(false);
#else
    bsp_i2c_init();
    uteModuleSprotAlgoTimerStop();
#endif
}

void vc30fx_pwr_dis(void)       //PF5
{
    if(vc30fx_dev.dev_work_status)
    {
        vc30fx_dev.dev_work_status = 0;
    }

    printf("vc30fx_pwr_dis\n");
    // drv_calibration_clk_clear();
    // uteModulePlatformOutputGpioSet(IO_PF5,false);
#if (SENSOR_STEP_SEL != SENSOR_STEP_NULL)
    sc7a20_500ms_callback_en(true);
#else
    i2c_gsensor_init();
    uteModuleSportAlgoTimerStart(UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_TIMER_DURATION);
#endif
#if (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
    uteModulePlatformOutputGpioSet(IO_PE4,false);
    uteModulePlatformOutputGpioSet(IO_PE5,false);
#elif (CHIP_PACKAGE_SELECT == CHIP_5691G)
    uteModulePlatformOutputGpioSet(IO_PE1,false);
    uteModulePlatformOutputGpioSet(IO_PE2,false);
#endif
    // vc30fx_msg_set_look(false);

    uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_HEART); //恢复睡眠
}

AT(.com_text.vc30fx)
void uteDrvHeartVC30FXHeartOrBloodOxygenAlgoInputData(void)
{
    uint8_t sportType = uteModuleSportMoreSportGetType();
    if (sportType == SPORT_TYPE_NONE)
    {
        sportType = SPORT_TYPE_PERIODIC_MONITORING;
    }
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT /*! 运动模式适配心率, xjc 2022-08-31*/
    if (sportType > SPORT_TYPE_FREE_TRAINING)
    {
        if ((sportType == SPORT_TYPE_INDOOR_WALK) ||
            (sportType == SPORT_TYPE_STEP_TRAINING) ||
            (sportType == SPORT_TYPE_OUTDOOR_WALK) ||
            (sportType == SPORT_TYPE_HIKING)) // 室内走路/踏步/户外健走/徒步
        {
            sportType = SPORT_TYPE_WALKING;
        }
        else if ((sportType == SPORT_TYPE_INDOOR_RUN) ||
                 (sportType == SPORT_TYPE_PARKOUR) ||
                 (sportType == SPORT_TYPE_MARATHON)) // 室内跑步/跑酷/马拉松
        {
            sportType = SPORT_TYPE_RUNNING;
        }
        else
        {
            sportType = SPORT_TYPE_FREE_TRAINING;
        }
    }
    if (!uteModuleSportMoreSportIsRuning())
    {
        sportType = SPORT_TYPE_INDOOR_WALK; // SPORT_TYPE_NONE;
    }

#endif
// get_vc30fx_device();
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
    if(uteModuleNewFactoryTestGetMode()!= FACTORY_TEST_MODE_NULL)
    {
        // 工厂模式下，必须用1模式的测试血氧
        UTE_MODULE_LOG(1, "gyj BloodOxygen mode 1",);
        vc30fx_usr_device_handler(sportType,1);
    }
    else
#endif
    {
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
        if (uteModuleBloodoxygenIsBloodOxygenAutoTesting())
        {
            //定时测试时候，使用模式2测试血氧，即切换动态测试
            vc30fx_usr_device_handler(sportType,2);
        }
        else
#endif
        {
            //手动测试时候，使用模式1测试血氧，即切换静态测试
            vc30fx_usr_device_handler(sportType,1);
        }
    }
}

/****************************************************************************
 * @description: 驱动参数保存/读取
 * @param {unsigned int} size
 * @return {*}
 ****************************************************************************/
int vc30fx_param_write_flash(unsigned char *pdata, unsigned int size)
{
    return 0;
}
int vc30fx_param_read_flash(unsigned char *pdata, unsigned int size)
{
    return 0;
}

/****************************************************************************
 * @description: 内存申请
 * @param {unsigned int} size
 * @return {*}
 ****************************************************************************/
void *vc30fx_malloc(unsigned int size)
{
    return malloc(size);
}
/****************************************************************************
 * @description: 内存释放
 * @param {void} *ptr
 * @return {*}
 ****************************************************************************/
void vc30fx_free(void *ptr)
{
    if (ptr)
    {
        free(ptr);
    }
}
/****************************************************************************
 * @description:
 * @param {unsigned char} regaddr
 * @param {unsigned char*} pbuf
 * @param {unsigned short int} size
 * @return {*}
 ****************************************************************************/
int vc30fx_read_register(unsigned char regaddr, unsigned char *pbuf, unsigned short int size)
{
    /* iic/spi read */
    /*****************************************************************************
     * When I2C is used, write Slave Address and write flag to vcHrxx firstly,
     * then write len byte data to register(s) start from startAddress
     * to register(s) pointed by pRegisters.
     * return:  0(read success),-1(read fail)
     ******************************************************************************/

    if(sys_cb.gsensor_iic_en)
    {
        bsp_i2c_init();
    }
    uint32_t i2c_cfg = START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 | DEV_ADDR1;
    bsp_hw_i2c_rx_buf(i2c_cfg, VC30FS_READ_ADDR(0x33), regaddr, pbuf,size);
    return 0;
}

int vc30fx_write_register(unsigned char regaddr, unsigned char *pbuf, unsigned short int size)
{
    /* iic/spi write */
    /*****************************************************************************
     * When I2C is used, write Slave Address and read flag to vcHrxx firstly,
     * then read len byte data from register(s) start from startAddress
     * to register(s) pointed by pRegisters.
     * return:  0(write success),-1(write fail)
     ******************************************************************************/

    if(sys_cb.gsensor_iic_en)
    {
        bsp_i2c_init();
    }
    uint32_t i2c_cfg = START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | WDATA;
    bsp_hw_i2c_tx_buf(i2c_cfg, VC30FS_WRITE_ADDR(0x33), regaddr, pbuf,size);
    return 0;
}

AT(.com_text.vc30fx)
static void vc30fx_timer_count(void)
{
    hw_timer_count++;
}

unsigned int vc30fx_get_cputimer_tick(void)
{
    /*  return RTC(timer count value) */
    hw_timer_count = cc_time_count() / 32;
    return hw_timer_count;
}

/*********************************************************************************************************/
/* user gsensor code */
#define ENABLE_GSENSOR 1
const unsigned char arry10[] = {0, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12};
const unsigned char arry20[] = {0, 2, 3, 5, 6, 8, 9, 11, 12, 13, 15, 16, 18, 19, 21, 22, 24, 25, 27, 28};

AT(.com_text.vc30fx)
static void vc30fx_get_board_gsensor_data(unsigned short int *pgsensor_len, unsigned short int ppg_count)
{
    unsigned char gsensor_len = 0;
    vcare_memset(&vc30fx_gsensor_data, 0, sizeof(vc30fx_gsensor_data));
    // u8 id = 0;
    // vc30fx_read_register(0x00,&id,1);
    // printf("==========id = %d\n", id);
#if ENABLE_GSENSOR
    int i = 0;

    static int16_t xx[UTE_DRV_GSENSOR_AXIS_DATA_MAX],yy[UTE_DRV_GSENSOR_AXIS_DATA_MAX],zz[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
    memset(xx,0,UTE_DRV_GSENSOR_AXIS_DATA_MAX*sizeof(int16_t));
    memset(yy,0,UTE_DRV_GSENSOR_AXIS_DATA_MAX*sizeof(int16_t));
    memset(zz,0,UTE_DRV_GSENSOR_AXIS_DATA_MAX*sizeof(int16_t));

    ute_drv_gsensor_common_axis_data_t *data = NULL;
    uteDrvGsensorCommonReadFifo(&data);
    gsensor_len = data->frameCnt;

    ute_drv_gsensor_common_axis_bit_change_t axisBitChange;
    axisBitChange.inputXaxis = &data->accXaxis[0];
    axisBitChange.inputYaxis = &data->accYaxis[0];
    axisBitChange.inputZaxis = &data->accZaxis[0];
    axisBitChange.outputXaxis = &xx[0];
    axisBitChange.outputYaxis = &yy[0];
    axisBitChange.outputZaxis = &zz[0];
    for(uint8_t i=data->frameCnt; i<UTE_DRV_GSENSOR_AXIS_DATA_MAX; i++)
    {
        data->accXaxis[i] = data->accXaxis[data->frameCnt-1];
        data->accYaxis[i] = data->accYaxis[data->frameCnt-1];
        data->accZaxis[i] = data->accZaxis[data->frameCnt-1];
    }
    // change g-sensor bit
    uteDrvGsensorCommonXYZaxisDataBitChange(&axisBitChange,50,GSENSOR_DATA_BIT_VCXX);

    // AxesRaw_t gsensor_buff[40] = {0};
    /* Gsensor精度要求 ReadGsensorFIFO(+-4G11bit or +-8G12bit 256/g) 一个fifo周期为1sec,10组ppg数据，需要获取1sec内10组gsensor数据(100ms*10)，用于匹配10组ppg信号*/
    // stk8321_get_fifo_data(&gsensor_len, gsensor_buff);

    VCARE_DBG_LOG("gsensor_len[%d]-ppgsize[%d]", gsensor_len, ppg_count);
    if (0 == ppg_count)
    {
        /* 不抽样 */
        *pgsensor_len = gsensor_len;
        for (i = 0; i < *pgsensor_len; i++)
        {
#if (BOARD_VERSION == BOARD_NEW_BOARD)
            vc30fx_gsensor_data.xData[i] = xx[i];
            vc30fx_gsensor_data.yData[i] = yy[i];
            vc30fx_gsensor_data.zData[i] = zz[i];
#elif (BOARD_VERSION == BOARD_OLD_BOARD)
            vc30fx_gsensor_data.xData[i] = xx[i];
            vc30fx_gsensor_data.yData[i] = yy[i];
            vc30fx_gsensor_data.zData[i] = zz[i];
#endif
        }
    }
    /* 此demo中gsensor的采样频率高于ppg采样，所以正常情况下，gsensor的数量一定是大于ppg的数据。特殊情况需要考虑ppg数量大于gsensor数量。需要对gsensor数据进行补长 */
    if ((gsensor_len >= ppg_count) && (ppg_count != 0))
    {
        *pgsensor_len = ppg_count;
        /* ppglen = 10 抽样 */
        if (10 == ppg_count)
        {
            for (i = 0; i < ppg_count; i++)
            {
#if (BOARD_VERSION == BOARD_NEW_BOARD)
                vc30fx_gsensor_data.xData[i] = xx[arry10[i]];
                vc30fx_gsensor_data.yData[i] = yy[arry10[i]];
                vc30fx_gsensor_data.zData[i] = zz[arry10[i]];
#elif (BOARD_VERSION == BOARD_OLD_BOARD)
                vc30fx_gsensor_data.xData[i] = xx[arry10[i]];
                vc30fx_gsensor_data.yData[i] = yy[arry10[i]];
                vc30fx_gsensor_data.zData[i] = zz[arry10[i]];
#endif
            }
        }
        else /* ppglen = 20 、25抽样 */
        {
            for (i = 0; i < ppg_count; i++)
            {
#if (BOARD_VERSION == BOARD_NEW_BOARD)
                vc30fx_gsensor_data.xData[i] = xx[arry20[i]];
                vc30fx_gsensor_data.yData[i] = yy[arry20[i]];
                vc30fx_gsensor_data.zData[i] = zz[arry20[i]];
#elif (BOARD_VERSION == BOARD_OLD_BOARD)
                vc30fx_gsensor_data.xData[i] = xx[arry20[i]];
                vc30fx_gsensor_data.yData[i] = yy[arry20[i]];
                vc30fx_gsensor_data.zData[i] = zz[arry20[i]];
#endif
            }
        }
    }
    else if ((gsensor_len < ppg_count) && (gsensor_len != 0)) /* gsensor不足补全 */
    {
        *pgsensor_len = ppg_count;
        for (i = 0; i < gsensor_len; i++)
        {
#if (BOARD_VERSION == BOARD_NEW_BOARD)
            vc30fx_gsensor_data.xData[i] = xx[i];
            vc30fx_gsensor_data.yData[i] = yy[i];
            vc30fx_gsensor_data.zData[i] = zz[i];
#elif (BOARD_VERSION == BOARD_OLD_BOARD)
            vc30fx_gsensor_data.xData[i] = xx[i];
            vc30fx_gsensor_data.yData[i] = yy[i];
            vc30fx_gsensor_data.zData[i] = zz[i];
#endif
        }
        for (i = gsensor_len; i < ppg_count; i++)
        {
#if (BOARD_VERSION == BOARD_NEW_BOARD)
            vc30fx_gsensor_data.xData[i] = xx[gsensor_len - 1];
            vc30fx_gsensor_data.yData[i] = yy[gsensor_len - 1];
            vc30fx_gsensor_data.zData[i] = zz[gsensor_len - 1];
#elif (BOARD_VERSION == BOARD_OLD_BOARD)
            vc30fx_gsensor_data.xData[i] = xx[gsensor_len - 1];
            vc30fx_gsensor_data.yData[i] = yy[gsensor_len - 1];
            vc30fx_gsensor_data.zData[i] = zz[gsensor_len - 1];
#endif
        }
    }

#endif
}

/****************************************************************************
 * @description: gsensor判断是否存在运动
 * @param {int} x_axis
 * @param {int} y_axis
 * @param {int} z_axis
 * @return {*}
 ****************************************************************************/
AT(.com_text.vc30fx)
static unsigned int vc30fx_gsensor_actuating_quantity(int x_axis, int y_axis, int z_axis)
{
    unsigned int ret_atc = 0;
    unsigned int abs_sum_current = 0;
    static unsigned int abs_sum_last = 0;
    if (x_axis < 0)
    {
        x_axis = -x_axis;
    }
    if (y_axis < 0)
    {
        y_axis = -y_axis;
    }
    if (z_axis < 0)
    {
        z_axis = -z_axis;
    }
    abs_sum_current = x_axis + y_axis + z_axis;
    if (abs_sum_last && abs_sum_current)
    {
        if (((abs_sum_last > abs_sum_current) ? (abs_sum_last - abs_sum_current) : (abs_sum_current - abs_sum_last)) >= 40)
        {
            VCARE_DBG_LOG("ACTIVE...");
            ret_atc = 1;
        }
    }
    abs_sum_last = abs_sum_current;
    vc30fx_drv_set_acce_wear_status(&vc30fx_dev, ret_atc);
    return ret_atc;
}

/****************************************************************************
 * @description: ppg数据加入心率算法
 * @param {vcare_ppg_device_t} *pdev
 * @return {*}
 ****************************************************************************/
AT(.com_text.vc30fx)
static int vc30fx_heart_rate_calculate(vcare_ppg_device_t *pdev)
{
    int heartRate = 0;
    unsigned char reliability = 0;
    AlgoInputData_t algoInputData_t = {0};
    AlgoOutputData_t algoOutputData = {0};
    vc30fx_sample_info_t *result_info = NULL;
    vc30fx_sample_slot_t *result = NULL;
    int i = 0;
    if (!pdev)
    {
        return -1;
    }
    heartRate = pdev->heart_rate;
    result_info = (vc30fx_sample_info_t *)pdev->result;
    result = &result_info->slot_result[0]; /* 驱动中初始化slot0为心率绿光ppg */
    for (i = 0; i < result->ppg_num; i++)
    {
#if UTE_DRV_HEART_VCXX_ARM_XY_EXCHANGE||SC7A20H_ROLLOVER_HAND_SCREEN_XY_EXCHANGE_SUPPORT
        algoInputData_t.axes.x = vc30fx_gsensor_data.yData[i];
        algoInputData_t.axes.y = vc30fx_gsensor_data.xData[i];
        algoInputData_t.axes.z = vc30fx_gsensor_data.zData[i];
#else
        algoInputData_t.axes.x = vc30fx_gsensor_data.xData[i];
        algoInputData_t.axes.y = vc30fx_gsensor_data.yData[i];
        algoInputData_t.axes.z = vc30fx_gsensor_data.zData[i];
#endif
        algoInputData_t.envSample = result->env_data;
        algoInputData_t.ppgSample = result->ppg_buffer[i];
        Algo_Input(&algoInputData_t, 1000 / 25, (AlgoSportMode)pdev->heart_algo_mode, 1, 1);
        VCARE_DBG_LOG("[%d] ppg=%d, x=%d, y=%d, z=%d", i, algoInputData_t.ppgSample,
                      algoInputData_t.axes.x, algoInputData_t.axes.y, algoInputData_t.axes.z);
    }
    Algo_Output(&algoOutputData);
    heartRate = algoOutputData.hrData;
    reliability = algoOutputData.reliability;
    if (-1 == heartRate)
    {
        Algo_Init();
        /* call heart algo wear status check,[-1] */
        vc30fx_drv_set_algo_wear_status(pdev, heartRate);
        VCARE_DBG_LOG("ALGO_STA=-1");
    }
    VCARE_DBG_LOG("ALGO-HR[%d][%d]", heartRate, reliability);
    // printf("ALGO-HR[%d][%d]\n", heartRate, reliability);
    return heartRate;
}

/****************************************************************************
 * @description: 血氧PPG数据加入血氧算法
 * @param {vcare_ppg_device_t} *pdev
 * @return {*}
 ****************************************************************************/
static int vc30fx_spo2_calculate(vcare_ppg_device_t *pdev)
{
    vc30fx_sample_info_t *result_info = NULL;
    vc30fx_sample_slot_t *result_ir = NULL, *result_red = NULL;
    int spo2CalValue = -1;
    int spo2Value = 0;
    unsigned char sportFlag = 0;
    if (!pdev)
    {
        return -1;
    }
    spo2Value = pdev->spo2;
    result_info = (vc30fx_sample_info_t *)pdev->result;
    result_ir = &result_info->slot_result[0];  /* 驱动中初始化slot0为红外ppg */
    result_red = &result_info->slot_result[1]; /* 驱动中初始化slot1为红光ppg */
    for (int i = 0; i < result_ir->ppg_num; i++)
    {
        spo2CalValue = spo2Algo_16bit(result_red->ppg_buffer[i], result_ir->ppg_buffer[i], vc30fx_dev.spo2_algo_mode);
#if UTE_DRV_HEART_VCXX_ARM_XY_EXCHANGE||SC7A20H_ROLLOVER_HAND_SCREEN_XY_EXCHANGE_SUPPORT
        // sportFlag = vcSportMotionCalculate( vc30fx_gsensor_data.yData[i], vc30fx_gsensor_data.xData[i],vc30fx_gsensor_data.zData[i]);
#else
        // sportFlag = vcSportMotionCalculate(vc30fx_gsensor_data.xData[i], vc30fx_gsensor_data.yData[i], vc30fx_gsensor_data.zData[i]);
#endif
        if (-1 != (int)spo2CalValue && spo2CalValue <= 100 && 0 == sportFlag)
        {
            spo2Value = (int)spo2CalValue;
        }
        VCARE_DBG_LOG("[%d] ppg_ir=%d, ppg_red=%d, x=%d, y=%d, z=%d", i, result_ir->ppg_buffer[i], result_red->ppg_buffer[i],
                      vc30fx_gsensor_data.xData[i], vc30fx_gsensor_data.yData[i], vc30fx_gsensor_data.zData[i]);
    }
    VCARE_DBG_LOG("ALGO-SPO2[%d][%d]", (unsigned short int)spo2Value, spo2CalValue);
    return spo2Value;
}

/********************************************************************************************************
 * 温度功能部分，需要进行对数运算，必须包含math库
 ********************************************************************************************************/
#include "math.h"
/* fix Rref = 10K R */
#define REF_R 10
/* NTC parameter */
#define NTC_B 3380
#define NTC_T (273.15f + 25) /* 25摄氏度下的绝对温度 */
#define NTC_R 10             /* 10K */
/* vc30fx parameter */
const unsigned char vc30fx_temperature_current[] = {0, 2, 4, 8, 12, 16, 20, 24};
static signed short int get_ntc_temperature(float ntc_value)
{
    signed short int temperature = 0;
    float tmp = 0.0f;
    tmp = 1 / (1 / NTC_T + (log(ntc_value / NTC_R) / NTC_B)) - 273.15f;
    temperature = (signed short int)(tmp * 10);
    VCARE_DBG_LOG("[CALC_TMP]=%d(%4.2f)", temperature, tmp);
    return temperature;
}
static void vc30fx_temperature_ntc_calculate(vcare_ppg_device_t *pdev)
{
    unsigned char ext_I = 0;
    float ext_R = 0.0f;
    vc30fx_sample_info_t *presult_t = (vc30fx_sample_info_t *)pdev->result;
    if(presult_t->extra_result.tempext_enwork)
    {
        /* 采样值转换为电阻值 */
        VCARE_DBG_LOG("reg_tempinn=%d, reg_tempext=%d, reg_tempref=%d", presult_t->extra_result.tempinn_data, presult_t->extra_result.tempext_data, presult_t->extra_result.tempref_data);
        ext_I = vc30fx_temperature_current[presult_t->extra_result.tempext_setcur];
        if (0 != ext_I)
        {
            ext_R = (float)(20 * REF_R * presult_t->extra_result.tempext_data) / (float)(presult_t->extra_result.tempref_data * ext_I);
            VCARE_DBG_LOG("ext_R=%f", ext_R);
            pdev->temperature1 = get_ntc_temperature(ext_R);
        }
    }
}

/****************************************************************************
 * @description: 定时器校验当前IC状态,如果发生异常，则重启LDO后初始化
 * @return {*}
 ****************************************************************************/
void vc30fx_usr_check_temperature_abnormal(void)
{
    unsigned char check_same_ret = 0, check_freq_ret = 0, check_temp_ret = 0;
    unsigned short check_cnt1 = 0, check_cnt2 = 0;
    check_same_ret = vc30fx_drv_read_check_temperature_abnormal(&vc30fx_dev, &check_freq_ret, &check_temp_ret, &check_cnt1, &check_cnt2);
    vc30fx_temperature_ntc_calculate(&vc30fx_dev);
    vc30fx_sample_info_t *result_info = (vc30fx_sample_info_t *)vc30fx_dev.result;
    VCARE_DBG_LOG("status(%d),psbio_0(%d,%d),cnt=%d,%d,same_ret=%d,freq_ret=%d,temp_ret=%d, temp_ext=%d",vc30fx_dev.dev_work_status,
                  result_info->slot_result[2].u.ps_data, result_info->extra_result.bioext_data, check_cnt1, check_cnt2,
                  check_same_ret, check_freq_ret, check_temp_ret, vc30fx_dev.temperature1);

    if (1 == vc30fx_dev.dev_work_status &&                                                          /* IC处于工作状态 */
        ((result_info->slot_result[2].u.ps_data==0 && (result_info->extra_result.bioext_data==0&&result_info->extra_result.bioext_enwork) ) /* ps和bio同时异常，或者RCOSC异常 */
         || (check_cnt1 == 0 && check_cnt2 == 0)
         || (1 == check_same_ret)                           /* bio-cnt其中之一已经不再变化 */
         || (1 == check_freq_ret)                                                                   /* 频率温度异常 */
         || (1 == check_temp_ret)                                                                   /* 内部温度异常 */
         || vc30fx_dev.temperature1 >= 600))                                                        /* NTC温度异常，如果硬件NC则会固定250 */
    {
        /* 1.close ldo, 2.delay(200ms), 3.open ldo, 4.delay(10ms), 5.init_dev */
        //extern void vcare_ldo_power_ctrl(unsigned char onoff);
        //vcare_ldo_power_ctrl(0);
        //platform_delay_ms(200);
        //vcare_ldo_power_ctrl(1);
        //platform_delay_ms(10);
        //vc30fx_drv_init_start_work(&vc30fx_dev, vc30fx_dev.workmode, result_info->ppg_div, result_info->ps_div, result_info->fifo_div );
    }
}

/****************************************************************************
 * @description: 中断事件处理事件
 * @param {unsigned char} heart_algo_mode
 * @param {unsigned char} spo2_algo_mode
 * @return {*}
 ****************************************************************************/
AT(.com_text.vc30fx)
void vc30fx_usr_device_handler( unsigned char heart_algo_mode, unsigned char spo2_algo_mode )
{
    // GPIOBCLR = BIT(0);
    unsigned short int ppg_num = 0;
    unsigned short int gsensor_num = 0;
    vc30fx_sample_info_t *sample_result_info_ptr = (vc30fx_sample_info_t *)vc30fx_dev.result;
    if(!vc30fx_dev.dev_work_status) return;
    VCARE_DBG_LOG("------------------");
    vc30fx_dev.heart_algo_mode = heart_algo_mode;
    vc30fx_dev.spo2_algo_mode = spo2_algo_mode;
    vc30fx_drv_get_result_handler(&vc30fx_dev);
    vc30fx_dev.wear = vc30fx_drv_get_wear_status(&vc30fx_dev);
    // printf("clk_calc_status:%d,wear:%d,workmode:%d\n",clk_info.clk_calc_status, vc30fx_dev.wear, vc30fx_dev.workmode);
    if (vc30fx_drv_is_ps_event(&vc30fx_dev) || vc30fx_drv_is_fifo_event(&vc30fx_dev))
    {
        //disp_data.wear_sta = vc30fx_dev.wear;
        //disp_data.disp_bio_val = sample_result_info_ptr->extra_result.bioext_data;
        //disp_data.disp_bio_abs = sample_result_info_ptr->extra_result.bioabs_calc;
        //VCARE_DBG_LOG("bio_absgap=%d, bioext=%d", disp_data.disp_bio_abs, disp_data.disp_bio_val );
    }

    switch (vc30fx_dev.workmode)
    {
        case WORK_MODE_HR:
            if (vc30fx_drv_is_fifo_event(&vc30fx_dev))
            {
                ppg_num = sample_result_info_ptr->slot_result[0].ppg_num;
                if (ppg_num > 40)
                    break;
                vc30fx_get_board_gsensor_data(&gsensor_num, ppg_num);
                if (gsensor_num)
                {
                    vc30fx_gsensor_actuating_quantity(vc30fx_gsensor_data.xData[0], vc30fx_gsensor_data.yData[0], vc30fx_gsensor_data.zData[0]);
                }
                if (WEAR_STA_HOLD == vc30fx_dev.wear)
                {
                    vc30fx_dev.heart_rate = vc30fx_heart_rate_calculate(&vc30fx_dev);
#if DUG_VCXX_HEART_SUPPORT
                    vc30fx_send_orginal_data(&vc30fx_dev, &vc30fx_gsensor_data, ppg_num);
#endif
                    uteModuleSportInputDataBeforeAlgo();

                }
                else
                {
#if DUG_VCXX_HEART_SUPPORT
                    vc30fx_send_orginal_data(&vc30fx_dev, &vc30fx_gsensor_data, ppg_num?ppg_num:1);
#endif
                    vc30fx_dev.heart_rate = 0;
                    Algo_Init();
                }
                //disp_data.hr_rate = vc30fx_dev.heart_rate;
                bsp_sensor_hrs_data_save(vc30fx_dev.heart_rate);
            }
            break;
        case WORK_MODE_SPO2:
            if (vc30fx_drv_is_fifo_event(&vc30fx_dev))
            {
                ppg_num = sample_result_info_ptr->slot_result[1].ppg_num;
                if (ppg_num > 40)
                    break;
                vc30fx_get_board_gsensor_data(&gsensor_num, ppg_num);
                if (WEAR_STA_HOLD == vc30fx_dev.wear)
                {
                    vc30fx_dev.spo2 = vc30fx_spo2_calculate(&vc30fx_dev);
#if DUG_VCXX_HEART_SUPPORT
                    vc30fx_send_orginal_data(&vc30fx_dev, &vc30fx_gsensor_data, ppg_num);
#endif
                    uteModuleSportInputDataBeforeAlgo();

                }
                else
                {
                    vc30fx_dev.spo2 = 0;
                    spo2AlgoInit_16bit();
                    //      vcSportMotionAlgoInit();
                }
                //disp_data.spo2_val = vc30fx_dev.spo2;
                bsp_sensor_spo2_data_save(vc30fx_dev.spo2);
            }
            break;
        case WORK_MODE_HRV:
            if (vc30fx_drv_is_fifo_event(&vc30fx_dev))
            {
                ppg_num = sample_result_info_ptr->slot_result[0].ppg_num;
                if (ppg_num > 40)
                    break;
                vc30fx_get_board_gsensor_data(&gsensor_num, ppg_num);
                if (WEAR_STA_HOLD == vc30fx_dev.wear)
                {
                    /*wear is hold,call_algo_func*/
#if DUG_VCXX_HEART_SUPPORT
                    vc30fx_send_orginal_data(&vc30fx_dev, &vc30fx_gsensor_data, ppg_num);
#endif
                    uteModuleSportInputDataBeforeAlgo();

                }
                else
                {
                    /*wear is drop,clear_algo_data*/
                }
            }
            break;
        case WORK_MODE_STRESS:
            if (vc30fx_drv_is_fifo_event(&vc30fx_dev))
            {
                ppg_num = sample_result_info_ptr->slot_result[0].ppg_num;
                if (ppg_num > 40)
                    break;
                vc30fx_get_board_gsensor_data(&gsensor_num, ppg_num);
                if (WEAR_STA_HOLD == vc30fx_dev.wear)
                {
                    /*wear is hold,call_algo_func*/
#if DUG_VCXX_HEART_SUPPORT
                    vc30fx_send_orginal_data(&vc30fx_dev, &vc30fx_gsensor_data, ppg_num);
#endif

                    uteModuleSportInputDataBeforeAlgo();

                }
                else
                {
                    /*wear is drop,clear_algo_data*/
                }
                //disp_data.hr_rate = vc30fx_dev.heart_rate;
            }
            break;
        case WORK_MODE_BLOODPRESSURE:
            if (vc30fx_drv_is_fifo_event(&vc30fx_dev))
            {
                ppg_num = sample_result_info_ptr->slot_result[0].ppg_num;
                if (ppg_num > 40)
                    break;
                vc30fx_get_board_gsensor_data(&gsensor_num, ppg_num);
                if (WEAR_STA_HOLD == vc30fx_dev.wear)
                {
                    /*wear is hold,call_algo_func*/
#if DUG_VCXX_HEART_SUPPORT
                    vc30fx_send_orginal_data(&vc30fx_dev, &vc30fx_gsensor_data, ppg_num);
#endif
                    uteModuleSportInputDataBeforeAlgo();

                }
                else
                {
                    /*wear is drop,clear_algo_data*/
                }
            }
            break;
        case WORK_MODE_WEAR:
            if (vc30fx_drv_is_fifo_event(&vc30fx_dev))
            {
                vc30fx_get_board_gsensor_data(&gsensor_num, ppg_num);
                if (gsensor_num)
                {
                    vc30fx_gsensor_actuating_quantity(vc30fx_gsensor_data.xData[0], vc30fx_gsensor_data.yData[0], vc30fx_gsensor_data.zData[0]);

                    if (WEAR_STA_HOLD == vc30fx_dev.wear)
                    {
                        uteModuleSportInputDataBeforeAlgo();
                    }
                }
#if DUG_VCXX_HEART_SUPPORT
                vc30fx_send_orginal_data(&vc30fx_dev, &vc30fx_gsensor_data, sample_result_info_ptr->slot_result[2].ppg_num);
#endif
            }
            break;
        case WORK_MODE_TEMPERATURE:
            vc30fx_temperature_ntc_calculate(&vc30fx_dev);
#if DUG_VCXX_HEART_SUPPORT
            vc30fx_send_orginal_data(&vc30fx_dev, &vc30fx_gsensor_data, 1);
#endif
            //disp_data.temperature_value0 = vc30fx_dev.temperature1;
            break;
        case WORK_MODE_FACTORY0:
            //disp_data.gImx = sample_result_info_ptr->slot_result[0].slot_current;
            //disp_data.gPre = sample_result_info_ptr->slot_result[0].u.pre_data;
            //disp_data.ps = sample_result_info_ptr->slot_result[2].u.ps_data;
            //disp_data.rImx = sample_result_info_ptr->slot_result[1].slot_current;
            //disp_data.rPre = sample_result_info_ptr->slot_result[1].u.pre_data;
            if(uteModuleFactoryTestGetCheckLightMode() == FACTORY_VCXX_TEST_MODE_RED_LIGHT)
            {
                uteModuleFactoryTestSetVkData(sample_result_info_ptr->slot_result[1].u.pre_data, sample_result_info_ptr->slot_result[1].slot_current, sample_result_info_ptr->slot_result[2].u.ps_data);
            }
            else
            {
                uteModuleFactoryTestSetVkData(sample_result_info_ptr->slot_result[0].u.pre_data, sample_result_info_ptr->slot_result[0].slot_current, sample_result_info_ptr->slot_result[2].u.ps_data);
            }
            // printf("pre_data;%d,slot_current:%d,ps_data:%d\n",sample_result_info_ptr->slot_result[0].u.pre_data,sample_result_info_ptr->slot_result[0].slot_current,sample_result_info_ptr->slot_result[2].u.ps_data);

            break;
        case WORK_MODE_FACTORY1:
            //disp_data.iImx = sample_result_info_ptr->slot_result[0].slot_current;
            //disp_data.iPre = sample_result_info_ptr->slot_result[0].u.pre_data;
            //disp_data.rImx = sample_result_info_ptr->slot_result[1].slot_current;
            //disp_data.rPre = sample_result_info_ptr->slot_result[1].u.pre_data;
            uteModuleFactoryTestSetVkData(sample_result_info_ptr->slot_result[0].u.pre_data, sample_result_info_ptr->slot_result[0].slot_current, sample_result_info_ptr->slot_result[2].u.ps_data);
            break;
        default:
            break;
    }
    /* 同步处理数据，清理fifo_buffer内存，为下次采样做准备 */
    vc30fx_drv_finished_sync(&vc30fx_dev);

#if UTE_HEART_VCXX_NO_WEAR_NO_RUN_STEP_ALGO_SUPPORT
    if (WEAR_STA_HOLD == vc30fx_dev.wear)
    {
        uteModuleSportUnWearToWearSwitch();
    }
    else
    {
        uteModuleSportStepTypeSetNone();
    }
#endif

    // if (sleep_cb.sys_is_sleep) {
    //     sleep_set_sysclk(SYS_24M);
    // }
    // if (func_cb.sta != FUNC_HEARTRATE) {
    //     printf("handler done uteModulePlatformDlpsEnable(0x00000080);\n");
    //     uteModulePlatformDlpsEnable(0x00000080);     //算法完成直接休眠
    // }
    // GPIOBSET = BIT(0);
}

bool vc30fx_sleep_isr = false;

AT(.com_text.vc30fx)
void vc30fx_isr(void)
{
    // if(sleep_cb.sys_is_sleep)
    // {
    //     vc30fx_sleep_isr = true;
    // }
    // else
    // {
    // msg_enqueue(EVT_VC30FX_ISR);
    // if (sleep_cb.sys_is_sleep) {
    //     sleep_set_sysclk(SYS_192M);
    // }
    //GPIOBCLR = BIT(1);
    if (vc30fx_clk_calc_status() == 0 || !sleep_cb.sys_is_sleep)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HEART_ALGO_HANDLER,0);
    }
    //     vc30fx_sleep_isr = false;
    // }
    if (vc30fx_clk_calc_status() == 1 && sleep_cb.sys_is_sleep)
    {
        vc30fx_sleep_isr = true;
    }
    // vc30fx_usr_device_handler(0, 1);
}

/*********************************************************************************************************
 * USER_API
 ********************************************************************************************************/
/****************************************************************************
 * @description: 初始化IC并启动工作
 * @param {InitParamTypeDef} *pinitconfig
 * @return {*}
 ****************************************************************************/
u8 vc30fx_usr_device_init( InitParamTypeDef *pinitconfig )
{
    unsigned short int ppgdiv = 0;
    unsigned char psdiv = 0;
    unsigned char fifodiv = 0;
    unsigned char frequency=25; /* default frequency=25hz */
    /* ppg_freq=20K/ppgdiv, ps_freq=ppg_freq/psdiv, fifodiv=psfreq/fifodiv
     *  fifo_max=128byte. The amount of byte by once sampling must not be greater than fifo_max(128bytes).
     */
    extern unsigned short int VC30Fx_CFG_MCU_TICK_FREQUENCY; /* mcu tick frequency parameter */
    extern unsigned short int VC30Fx_WEAR_PARAM_BIO_ABS_HOLD; /* factory calibration parameter-hold */
    extern unsigned short int VC30Fx_WEAR_PARAM_BIO_ABS_DROP; /* factory calibration parameter-drop */
    VC30Fx_CFG_MCU_TICK_FREQUENCY=31250;
    VC30Fx_WEAR_PARAM_BIO_ABS_HOLD=200; /* save_diff*8/10 */
    VC30Fx_WEAR_PARAM_BIO_ABS_DROP=160; /* save_diff*5/10 */

    vc30fx_dev.workmode = (work_mode)pinitconfig->work_mode;
    switch (pinitconfig->work_mode)
    {
        case WORK_MODE_HR:
            bsp_sensor_hrs_data_save(0);
        case WORK_MODE_SPO2:
            bsp_sensor_spo2_data_save(0);
        case WORK_MODE_WEAR:
        case WORK_MODE_FACTORY0:
        case WORK_MODE_FACTORY1:
            frequency=25;
            break;
        case WORK_MODE_STRESS:
            frequency=50;
            break;
        case WORK_MODE_HRV:
        case WORK_MODE_BLOODPRESSURE:
            frequency=100;
            break;
        default:
            break;
    }
    ppgdiv = 20000 / frequency; /* ppg_freq = frequency */
#if (VC30Fx_PS_WORK_FREQUENCY_CONFIG==1)
    psdiv = pinitconfig->sampling_gap_time*25/1000; /* 520ms ~ ps_div=13, fifo_div=1, 注意设置初始化参数的时间时，要估算一次fifo长度和fifo总长度的关系 */
    fifodiv = 1;
    if( WORK_MODE_FACTORY0==pinitconfig->work_mode || WORK_MODE_FACTORY1==pinitconfig->work_mode )
    {
        frequency=50;
        ppgdiv = 20000 / frequency;
        psdiv = pinitconfig->sampling_gap_time*25/1000; /* 注意：别修改psdiv，在有活体的配置里面，会影响活体的时间参数.所以此处通过提高一倍ppg频率，进而加快FAT模式的中断数据更新 */
        fifodiv=1;
    }
#elif(VC30Fx_PS_WORK_FREQUENCY_CONFIG==0) /* ps=5hz */
    if (frequency == 25)
    {
        psdiv = 5;
        fifodiv = 4; /* ppg=25hz, psbio=5hz, fifo=1.25hz,fifotime=800ms */
        if( WORK_MODE_SPO2==pinitconfig->work_mode)
        {
            fifodiv=2; /* 提高fifo的频率，到400ms，留有一次丢fifo没处理的余量，不然发生异常时覆盖后解析会异常，可能会导致摘下重新佩戴 */
        }
    }
    else if (frequency == 50)
    {
        psdiv = 10;
        fifodiv = 2; /* ppg=50hz, psbio=5hz, fifo=2.5hz,fifotime=400ms */
    }
    else if (frequency == 100)
    {
        psdiv = 20;
        fifodiv = 1; /* ppg=100hz, psbio=5hz, fifo=5hz, fifotime=200ms */
    }
    if( WORK_MODE_HR==pinitconfig->work_mode || WORK_MODE_WEAR==pinitconfig->work_mode )
    {
        fifodiv = pinitconfig->sampling_gap_time*50/10000;
    }
    if( WORK_MODE_FACTORY0==pinitconfig->work_mode || WORK_MODE_FACTORY1==pinitconfig->work_mode )
    {
        fifodiv=1;
    }
    if( pinitconfig->work_mode==WORK_MODE_WEAR )
    {
        ppgdiv = 800; /* 25hz */
        psdiv  = 5;   /* 5hz  ~ 400ms */
        fifodiv= 10;  /* 0.5hz ~2sec  */
    }
#endif
    vc30fx_drv_init_start_work(&vc30fx_dev, pinitconfig->work_mode, ppgdiv, psdiv, fifodiv);
    vc30fx_dev.dev_work_status = 1;
    /* 如果需要保持测试值，比如24h的心率模式，初始化时则不清除记录值心率值，避免初始化后出现一段为0的情况 */
    vc30fx_dev.heart_rate = 0;
    vc30fx_dev.stress=0;
    vc30fx_dev.hrv = 0;
    vc30fx_dev.spo2 = 0;
    vc30fx_dev.blood_pressure=0;
    vc30fx_dev.ecg=0;
    vc30fx_dev.blood_pressure=0;
    Algo_Init();
    spo2AlgoInit_16bit();
//  vcSportMotionAlgoInit();
    VCARE_DBG_LOG("work_mode=%d", pinitconfig->work_mode );
    /* 如果使用定时器，必须适配模式的定时器执行时间，否则无法正确解析数据 */
    //unsigned short int timer_set_ms=(1000/frequency)*psdiv*fifodiv;
    hw_timer_count = 0;
    // bsp_hw_timer_set(HW_TIMER2, 32, vc30fx_timer_count);
    cc_time_init();
#if (CHIP_PACKAGE_SELECT == CHIP_5691G)
    extab_user_isr_set(IO_PG6, RISE_EDGE, IOUD_SEL_PD, vc30fx_isr);
    extab_user_isr_mode_set(IO_PG6, MODE_BOTH_AWAKEN_SLEEP);
#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
    extab_user_isr_set(IO_PE7, RISE_EDGE, IOUD_SEL_PD, vc30fx_isr);
    extab_user_isr_mode_set(IO_PE7, MODE_BOTH_AWAKEN_SLEEP);
#endif
    return (pinitconfig->work_mode != -1);
}

/****************************************************************************
 * @description: 获取IC及驱动相关参数
 * @param {unsigned char} *pchip_id
 * @param {unsigned char} *pvers_id
 * @param {char} *
 * @param {char} *
 * @return {*}
 ****************************************************************************/
void vc30fx_usr_get_dev_info(unsigned char *pchip_id, unsigned char *pvers_id, char **drv_name, char **dev_name)
{
    /*
    VC30FS:chip_id=0x25, vers_id=0x10
    VC12S :chip_id=0x27, vers_id=0x10, 0x30
    */
    vc30fx_drv_get_hwsw_infomation(drv_name, pchip_id, pvers_id);
}
/****************************************************************************
 * @description: 开始工作
 * @return {*}
 ****************************************************************************/
void vc30fx_usr_start_work(void)
{
    vc30fx_drv_wakeup_work(&vc30fx_dev);
    vc30fx_dev.dev_work_status = 1;
}
/****************************************************************************
 * @description: 停止工作
 * @return {*}
 ****************************************************************************/
int vc30fx_usr_stop_work(void)
{
    int ret = -1;
    ret = vc30fx_drv_sleep_work(&vc30fx_dev);
    vc30fx_dev.dev_work_status = 0;
    printf("vc30fx_usr_stop_work ret=%d\n", ret);
    return ret;
}
/****************************************************************************
 * @description: 软复位
 * @return {*}
 ****************************************************************************/
void vc30fx_usr_soft_reset(void)
{
    vc30fx_drv_software_reset(&vc30fx_dev);
    vc30fx_dev.dev_work_status = 0;
}
/****************************************************************************
 * @description: 获取工作状态
 * @return {*}
 ****************************************************************************/
AT(.com_text.vc30fx)
bool vc30fx_usr_get_work_status(void)
{
    return vc30fx_dev.dev_work_status;
}
/****************************************************************************
 * @description: 获取工作模式
 * @return {*}
 ****************************************************************************/
AT(.com_text.vc30fx)
work_mode vc30fx_usr_get_work_mode(void)
{
    return vc30fx_dev.workmode;
}
/****************************************************************************
 * @description: 活体IC参数重置校准
 * @return {*}
 ****************************************************************************/
void vc30fx_usr_reset_bio_param(void)
{
    vc30fx_drv_clear_reset_bio_param(&vc30fx_dev);
}
/****************************************************************************
 * @description: 获取佩戴状态
 * @return {*}
 ****************************************************************************/
int vc30fx_usr_get_wear_status(void)
{
    if (WEAR_STA_HOLD == vc30fx_dev.wear)
    {
        return 1;
    }
    return 0;
}

static const dev_ops vc30fx_ops =
{
    .dev_init = vc30fx_usr_device_init,
    .get_dev_info = vc30fx_usr_get_dev_info,
    .start_work = vc30fx_usr_start_work,
    .stop_work = vc30fx_usr_stop_work,
    .soft_reset = vc30fx_usr_soft_reset,
    .get_events = NULL,
    .set_slot_enable = NULL,
    .reset_bio_param = vc30fx_usr_reset_bio_param,
    .get_wear_status = vc30fx_usr_get_wear_status,
    .dev_irq_handler = vc30fx_usr_device_handler,
    .dev_check_temp_error = vc30fx_usr_check_temperature_abnormal,
};

vcare_ppg_device_t *get_vc30fx_device(void)
{
    vc30fx_dev.ops = &vc30fx_ops;
    /* !!! delete the device need call vc30fx_drv_memroy_free() free device the memroy */
    if (vc30fx_drv_memroy_alloc())
    {
        return NULL;
    }
    return &vc30fx_dev;
}
#endif
/************************************************************************
 * 以下为MCU外设初始化，任务调用
 *  example function task_demo_application
 ***********************************************************************/
#if 0
/* 系统头文件 */
#include "os_task.h"
//.....
/* 驱动头文件 */
#include "task_vcare_demo.h"
#include "vc30fx_user_app.h"
/* 心率算法头文件 */
#include "algo.h"

#define DEMO_TASK_PRIORITY 2          //!< Task priorities
#define DEMO_TASK_STACK_SIZE 1024 * 3 //!< Task stack size
void *demo_task_handle;               //!< APP Task handle
void *demo_queue_handle;
static void board_init(void);
void task_demo_application(void *pvParameters)
{
    T_IO_MSG demo_msg;
    AlgoSportMode demo_heart_algo_mode = SPORT_TYPE_NORMAL;
    unsigned char demo_spo2_algo_mode = 0;
    InitParamTypeDef init_config= { 800, WORK_MODE_HR };
    vcare_ppg_device_t *ppg_dev = NULL;
    os_msg_queue_create(&demo_queue_handle, 0x10, sizeof(T_IO_MSG));
    /* 通过芯片固定的id寄存器去获取ID，辨别不同的ppg设备，并且获取对应的设备对象 */
    unsigned char id0=0,id1=0;
    i2c_read(0x00,&id0,1);
    i2c_read(0x01,&id1,1);
    if( id0==0x25 || id0==0x27) /* id0=0x25,0x27 ~ VC30FS/VC30FC */
    {
        ppg_dev = get_vc30fx_device();
    }
    if( ppg_dev )
    {
        board_init();
        init_config.work_mode = WORK_MODE_HR;
        //init_config.work_mode = WORK_MODE_SPO2;
        //init_config.work_mode = WORK_MODE_HRV;
        //init_config.work_mode = WORK_MODE_BLOODPRESSURE;
        //init_config.work_mode = WORK_MODE_WEAR;
        //init_config.work_mode = WORK_MODE_TEMPERATURE;
        //init_config.work_mode = WORK_MODE_FACTORY0;
        //init_config.work_mode = WORK_MODE_FACTORY1;
        ppg_dev->ops->dev_init( &init_config );
    }
    while (1)
    {
        if (os_msg_recv(demo_queue_handle, &demo_msg, 0xFFFFFFFF) == true)
        {
            switch (demo_msg.subtype)
            {
                case DEMO_MSG_VC_IRQ_EVENT:
                    ppg_dev->ops->dev_irq_handler( demo_heart_algo_mode, demo_spo2_algo_mode );
                    break;
                case DEMO_MSG_KEY_IRQ_EVENT:
                    //调用初始化切换工作模式,调用ppg_dev->ops->dev_init(...)进行模式切换
                    //demo_work_mode_switch();
                    break;
                default:
                    break;
            }
        }
    } /* while(1) */
}

void task_demo_creat(void)
{
    //example app creat
    if (os_task_create(&demo_task_handle, "demo", task_demo_application, NULL, DEMO_TASK_STACK_SIZE, DEMO_TASK_PRIORITY) != true)
    {
        VCARE_DBG_ERR(" demo_task creat is failed\r\n ");
    }
}

#endif /* end of example function */
#endif //#if (SENSOR_HR_SEL == SENSOR_HR_VC30FX)
