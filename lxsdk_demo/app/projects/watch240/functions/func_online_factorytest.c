#include "include.h"
#include "ute_module_factorytest.h"
#include "ute_module_log.h"
#include "ute_drv_battery_common.h"
#include "ute_module_heart.h"
#include "ute_drv_gsensor_common.h"
#include "ute_module_protocol.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_micrecord.h"

#define TRACE_EN 0

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

///---------------------------------------[UI]--------------------------------------------

#define COLOR_DARKGREEN make_color(90, 170, 90)

// 组件ID
enum
{
    // 标题文本
    COMPO_ID_TXT_TITLE = 1,
    // 漏光测试
    COMPO_ID_TXT_VCXX_PRE,
    COMPO_ID_TXT_VCXX_CUR,
    COMPO_ID_TXT_VCXX_PS,
    COMPO_ID_TXT_VCXX_RESULT,
    // RGB色块
    COMPO_ID_SHAPE_RGB,
    //电池测试
    COMPO_ID_TXT_START_TIME,
    COMPO_ID_TXT_CURR_TIME,
    COMPO_ID_TXT_RUNNING_TIME,
    COMPO_ID_TXT_START_VOLTAGE,
    COMPO_ID_TXT_CURR_VOLTAGE,
    COMPO_ID_TXT_START_PER,
    COMPO_ID_TXT_CURR_PER,
    // 老化模式
    COMPO_ID_TXT_CHARGE_STA,
    COMPO_ID_TXT_HR_VALUE,
    COMPO_ID_TXT_ACC_AXES,
    //tp 九宫格 色块
    COMPO_ID_TP_SHAPE_1,
    COMPO_ID_TP_SHAPE_2,
    COMPO_ID_TP_SHAPE_3,
    COMPO_ID_TP_SHAPE_4,
    COMPO_ID_TP_SHAPE_5,
    COMPO_ID_TP_SHAPE_6,
    COMPO_ID_TP_SHAPE_7,
    COMPO_ID_TP_SHAPE_8,
    COMPO_ID_TP_SHAPE_9,
    //tp 九宫格按键
    COMPO_ID_TP_BTN_1,
    COMPO_ID_TP_BTN_2,
    COMPO_ID_TP_BTN_3,
    COMPO_ID_TP_BTN_4,
    COMPO_ID_TP_BTN_5,
    COMPO_ID_TP_BTN_6,
    COMPO_ID_TP_BTN_7,
    COMPO_ID_TP_BTN_8,
    COMPO_ID_TP_BTN_9,
    // mic 按键
    COMPO_ID_MIC_BTN_START,
    COMPO_ID_MIC_BTN_PLAY,
    COMPO_ID_MIC_RECT_START,
    COMPO_ID_MIC_RECT_PLAY,
};

//mic录音状态
enum
{
    MIC_TEST_STA_EMPTY,
    MIC_TEST_STA_RECORDING,
    MIC_TEST_STA_FULL,
};

typedef struct f_online_factory_test_t_
{
    u8 sta;
    u8 test_item;
    u32 tick;
    u8 music_sec;
    u8 color_idx;
    bool play_need;
    page_tp_move_t *ptm;
} f_online_factory_test_t;

// 漏光测试窗体
static compo_form_t *func_online_factory_test_check_light_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    // 创建文本
    compo_textbox_t *textbox = compo_textbox_create(frm, 40);
    compo_textbox_set(textbox, "漏光测试: 绿灯");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 1 / 6);
    compo_setid(textbox, COMPO_ID_TXT_TITLE);

    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 6);
    compo_textbox_set(textbox, "preV: 0");
    compo_setid(textbox, COMPO_ID_TXT_VCXX_PRE);

    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 6);
    compo_textbox_set(textbox, "curV: 0");
    compo_setid(textbox, COMPO_ID_TXT_VCXX_CUR);

    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 4 / 6);
    compo_textbox_set(textbox, "psV: 0");
    compo_setid(textbox, COMPO_ID_TXT_VCXX_PS);

    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 5 / 6);
    compo_textbox_set_forecolor(textbox, make_color(0xff, 0x00, 0x00));
    compo_textbox_set(textbox, "Fail");
    compo_setid(textbox, COMPO_ID_TXT_VCXX_RESULT);

    return frm;
}

// 漏光测试刷新
static void func_online_factory_test_check_light_process(void)
{
    int preV = 0;
    int curV = 0;
    int psV = 0;
    bool isPass = false;
    char str_buff[40];

    compo_textbox_t *txt_vcxx_title = compo_getobj_byid(COMPO_ID_TXT_TITLE);
    compo_textbox_t *txt_vcxx_pre = compo_getobj_byid(COMPO_ID_TXT_VCXX_PRE);
    compo_textbox_t *txt_vcxx_cur = compo_getobj_byid(COMPO_ID_TXT_VCXX_CUR);
    compo_textbox_t *txt_vcxx_ps = compo_getobj_byid(COMPO_ID_TXT_VCXX_PS);
    compo_textbox_t *txt_vcxx_res = compo_getobj_byid(COMPO_ID_TXT_VCXX_RESULT);

    uteModuleFactoryTestGetVkData(&preV, &curV, &psV, &isPass);

    if (uteModuleFactoryTestGetCheckLightMode() == FACTORY_VCXX_TEST_MODE_RED_LIGHT)
    {
        compo_textbox_set(txt_vcxx_title, "漏光测试: 红灯");
    }
    else if (uteModuleFactoryTestGetCheckLightMode() == FACTORY_VCXX_TEST_MODE_INFRARED)
    {
        compo_textbox_set(txt_vcxx_title, "漏光测试: 红外");
    }
    else
    {
        compo_textbox_set(txt_vcxx_title, "漏光测试: 绿灯");
    }

    memset(str_buff, 0, sizeof(str_buff));
    sprintf(str_buff, "preV: %d", preV);
    compo_textbox_set(txt_vcxx_pre, str_buff);

    memset(str_buff, 0, sizeof(str_buff));
    sprintf(str_buff, "curV: %d", curV);
    compo_textbox_set(txt_vcxx_cur, str_buff);

    memset(str_buff, 0, sizeof(str_buff));
    sprintf(str_buff, "psV: %d", psV);
    compo_textbox_set(txt_vcxx_ps, str_buff);

    if (isPass)
    {
        memset(str_buff, 0, sizeof(str_buff));
        sprintf(str_buff, "Pass");
        compo_textbox_set(txt_vcxx_res, str_buff);
        compo_textbox_set_forecolor(txt_vcxx_res, make_color(0x00, 0xff, 0x00));
    }
    else
    {
        memset(str_buff, 0, sizeof(str_buff));
        sprintf(str_buff, "Fail");
        compo_textbox_set(txt_vcxx_res, str_buff);
        compo_textbox_set_forecolor(txt_vcxx_res, make_color(0xff, 0x00, 0x00));
    }

    reset_guioff_delay();
}

// RGB测试窗口创建
static compo_form_t *func_online_factory_test_screen_rgb_create(void)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;
    compo_form_t *frm = compo_form_create(true);
    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_setid(shape, COMPO_ID_SHAPE_RGB);
    compo_shape_set_color(shape, COLOR_BLACK);
    f_online_factory_test->color_idx = 0;
    return frm;
}

// RGB测试窗口刷新
static void func_online_factory_test_screen_rgb_process(void)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_SHAPE_RGB);

    const uint16_t dis_color[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_WHITE};

    if(tick_check_expire(f_online_factory_test->tick, 1000))
    {
        f_online_factory_test->tick = tick_get();
        f_online_factory_test->color_idx = (f_online_factory_test->color_idx + 1) % (sizeof(dis_color) / sizeof(uint16_t));
        compo_shape_set_color(shape, dis_color[f_online_factory_test->color_idx]);
    }

    reset_guioff_delay();
}

//电量测试窗口创建
static compo_form_t *func_online_factory_test_battery_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    char str_buff[40];
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);

    ute_module_factory_test_data_t *testData = NULL;
    testData = (ute_module_factory_test_data_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_factory_test_data_t));
    uteModuleFactoryTestGetData(testData);

    // 创建文本
    compo_textbox_t *textbox = NULL;

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"开始时间:%04d-%02d-%02d %02d:%02d:%02d",testData->u.battery.startTime.year,testData->u.battery.startTime.month,testData->u.battery.startTime.day,testData->u.battery.startTime.hour,testData->u.battery.startTime.min,testData->u.battery.startTime.sec);
    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set(textbox, str_buff);
    if(UTE_DRV_SCREEN_SHAPE)
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 10, GUI_SCREEN_HEIGHT * 1 / 7, GUI_SCREEN_WIDTH - 20, GUI_SCREEN_HEIGHT / 7);
    }
    else
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 30, GUI_SCREEN_HEIGHT * 1 / 7, GUI_SCREEN_WIDTH - 60, GUI_SCREEN_HEIGHT / 7);
    }
    compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_setid(textbox, COMPO_ID_TXT_START_TIME);

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"当前时间:%04d-%02d-%02d %02d:%02d:%02d",time.year,time.month,time.day,time.hour,time.min,time.sec);
    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set(textbox, str_buff);
    if(UTE_DRV_SCREEN_SHAPE)
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 10, GUI_SCREEN_HEIGHT * 2 / 7, GUI_SCREEN_WIDTH - 20, GUI_SCREEN_HEIGHT / 7);
    }
    else
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 30, GUI_SCREEN_HEIGHT * 2 / 7, GUI_SCREEN_WIDTH - 60, GUI_SCREEN_HEIGHT / 7);
    }
    compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_setid(textbox, COMPO_ID_TXT_CURR_TIME);

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"已运行 %02d:%02d:%02d",(uint16_t)(testData->u.battery.runningTimeSecond / 3600), (uint16_t)((testData->u.battery.runningTimeSecond % 3600) / 60), (uint16_t)(testData->u.battery.runningTimeSecond % 60));
    textbox = compo_textbox_create(frm, 40);
    if(UTE_DRV_SCREEN_SHAPE)
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 10, GUI_SCREEN_HEIGHT * 3 / 7, GUI_SCREEN_WIDTH - 20, GUI_SCREEN_HEIGHT / 7);
    }
    else
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 30, GUI_SCREEN_HEIGHT * 3 / 7, GUI_SCREEN_WIDTH - 60, GUI_SCREEN_HEIGHT / 7);
    }
    compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_textbox_set(textbox, str_buff);
    compo_setid(textbox, COMPO_ID_TXT_RUNNING_TIME);

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"开始电压:%dmV",testData->u.battery.startBatteryVoltage);
    textbox = compo_textbox_create(frm, 40);
    if(UTE_DRV_SCREEN_SHAPE)
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 10, GUI_SCREEN_HEIGHT * 4 / 7, GUI_SCREEN_WIDTH - 20, GUI_SCREEN_HEIGHT / 7);
    }
    else
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 30, GUI_SCREEN_HEIGHT * 4 / 7, GUI_SCREEN_WIDTH - 60, GUI_SCREEN_HEIGHT / 7);
    }
    compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_textbox_set(textbox, str_buff);
    compo_setid(textbox, COMPO_ID_TXT_START_VOLTAGE);

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"当前电压:%dmV",testData->u.battery.currBatteryVoltage);
    textbox = compo_textbox_create(frm, 40);
    if(UTE_DRV_SCREEN_SHAPE)
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 10, GUI_SCREEN_HEIGHT * 5 / 7, GUI_SCREEN_WIDTH - 20, GUI_SCREEN_HEIGHT / 7);
    }
    else
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 30, GUI_SCREEN_HEIGHT * 5 / 7, GUI_SCREEN_WIDTH - 60, GUI_SCREEN_HEIGHT / 7);
    }
    compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_textbox_set(textbox, str_buff);
    compo_setid(textbox, COMPO_ID_TXT_CURR_VOLTAGE);

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"开始电量:%d%%",testData->u.battery.startBatteryLvl);
    textbox = compo_textbox_create(frm, 40);
    if(UTE_DRV_SCREEN_SHAPE)
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 10, GUI_SCREEN_HEIGHT * 6 / 7, GUI_SCREEN_WIDTH - 20, GUI_SCREEN_HEIGHT / 7);
    }
    else
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 30, GUI_SCREEN_HEIGHT * 6 / 7, GUI_SCREEN_WIDTH - 60, GUI_SCREEN_HEIGHT / 7);
    }
    compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_textbox_set(textbox, str_buff);
    compo_setid(textbox, COMPO_ID_TXT_START_PER);

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"当前电量:%d%%",testData->u.battery.currBatteryLvl);
    textbox = compo_textbox_create(frm, 40);
    if(UTE_DRV_SCREEN_SHAPE)
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 10, GUI_SCREEN_HEIGHT * 7 / 7, GUI_SCREEN_WIDTH - 20, GUI_SCREEN_HEIGHT / 7);
    }
    else
    {
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X + 30, GUI_SCREEN_HEIGHT * 7 / 7, GUI_SCREEN_WIDTH - 60, GUI_SCREEN_HEIGHT / 7);
    }
    compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_textbox_set(textbox, str_buff);
    compo_setid(textbox, COMPO_ID_TXT_CURR_PER);

    uteModulePlatformMemoryFree(testData);

    return frm;
}

//电量测试窗口刷新
static void func_online_factory_test_battery_process(void)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;

    if(f_online_factory_test->ptm == NULL)
    {
        f_online_factory_test->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
        page_move_info_t info =
        {
            .page_size =  (GUI_SCREEN_HEIGHT / 7) * 8,
            .page_count = 1,
            .quick_jump_perc = 50,
            .up_over_perc = 5,
            .down_over_perc = 5,
        };
        compo_page_move_init(f_online_factory_test->ptm, func_cb.frm_main->page_body, &info);
    }

    char str_buff[40];
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);

    ute_module_factory_test_data_t *testData = NULL;
    testData = (ute_module_factory_test_data_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_factory_test_data_t));
    uteModuleFactoryTestGetData(testData);

    compo_textbox_t *textbox = NULL;

    if(tick_check_expire(f_online_factory_test->tick, 10000))
    {
        f_online_factory_test->tick = tick_get();
        textbox = compo_getobj_byid(COMPO_ID_TXT_START_TIME);
        memset(str_buff, 0, sizeof(str_buff));
        snprintf(str_buff, sizeof(str_buff),"开始时间:%04d-%02d-%02d %02d:%02d:%02d",testData->u.battery.startTime.year,testData->u.battery.startTime.month,testData->u.battery.startTime.day,testData->u.battery.startTime.hour,testData->u.battery.startTime.min,testData->u.battery.startTime.sec);
        compo_textbox_set(textbox, str_buff);

        textbox = compo_getobj_byid(COMPO_ID_TXT_CURR_TIME);
        memset(str_buff, 0, sizeof(str_buff));
        snprintf(str_buff, sizeof(str_buff),"当前时间:%04d-%02d-%02d %02d:%02d:%02d",time.year,time.month,time.day,time.hour,time.min,time.sec);
        compo_textbox_set(textbox, str_buff);
    }

    textbox = compo_getobj_byid(COMPO_ID_TXT_RUNNING_TIME);
    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"已运行 %02d:%02d:%02d",(uint16_t)(testData->u.battery.runningTimeSecond / 3600), (uint16_t)((testData->u.battery.runningTimeSecond % 3600) / 60), (uint16_t)(testData->u.battery.runningTimeSecond % 60));
    compo_textbox_set(textbox, str_buff);

    textbox = compo_getobj_byid(COMPO_ID_TXT_START_VOLTAGE);
    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"开始电压:%dmV",testData->u.battery.startBatteryVoltage);
    compo_textbox_set(textbox, str_buff);

    textbox = compo_getobj_byid(COMPO_ID_TXT_CURR_VOLTAGE);
    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"当前电压:%dmV",testData->u.battery.currBatteryVoltage);
    compo_textbox_set(textbox, str_buff);

    textbox = compo_getobj_byid(COMPO_ID_TXT_START_PER);
    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"开始电量:%d%%",testData->u.battery.startBatteryLvl);
    compo_textbox_set(textbox, str_buff);

    textbox = compo_getobj_byid(COMPO_ID_TXT_CURR_PER);
    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"当前电量:%d%%",testData->u.battery.currBatteryLvl);
    compo_textbox_set(textbox, str_buff);

    uteModulePlatformMemoryFree(testData);
}

// 老化模式窗口创建
static compo_form_t * func_online_factory_test_ageing_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_GREEN );
    compo_setid(shape,COMPO_ID_SHAPE_RGB);

    char str_buff[40];

    ute_module_factory_test_data_t *testData = NULL;
    testData = (ute_module_factory_test_data_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_factory_test_data_t));
    uteModuleFactoryTestGetData(testData);

    // 创建文本
    compo_textbox_t *textbox = NULL;

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"电池:%s %d%%",uteDrvBatteryCommonGetChargerStatus() == BAT_STATUS_NO_CHARGE ? "未充电" : "充电中", uteDrvBatteryCommonGetLvl());
    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set(textbox, str_buff);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 1 / 6);
    compo_setid(textbox, COMPO_ID_TXT_CHARGE_STA);

    memset(str_buff, 0, sizeof(str_buff));
#if UTE_MODULE_HEART_SUPPORT
    snprintf(str_buff, sizeof(str_buff),"心率:%s %d bpm",uteModuleHeartIsWear() ? "佩戴中" : "未佩戴", testData->u.aging.heart);
#endif
    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set(textbox, str_buff);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 6);
    compo_setid(textbox, COMPO_ID_TXT_HR_VALUE);

    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set(textbox, "G-Sensor:");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 6);

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"X:%d Y:%d Z:%d",testData->u.aging.gsensorX, testData->u.aging.gsensorY, testData->u.aging.gsensorZ);
    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set(textbox, str_buff);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 4 / 6);
    compo_setid(textbox, COMPO_ID_TXT_ACC_AXES);

    textbox = compo_textbox_create(frm, 40);
    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"已运行 %02d:%02d:%02d",(uint16_t)(testData->u.aging.runningTimeSecond / 3600), (uint16_t)((testData->u.aging.runningTimeSecond % 3600) / 60), (uint16_t)(testData->u.aging.runningTimeSecond % 60));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 5 / 6);
    compo_textbox_set(textbox, str_buff);
    compo_setid(textbox, COMPO_ID_TXT_RUNNING_TIME);

    uteModulePlatformMemoryFree(testData);

    return frm;
}

static void func_online_factory_test_ageing_process(void)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_SHAPE_RGB);
    const uint16_t dis_color[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE};
    if(tick_check_expire(f_online_factory_test->tick, 1000))
    {
        f_online_factory_test->tick = tick_get();
#if UTE_MODULE_NEW_AGING_TEST_AUDIO
        if(f_online_factory_test->music_sec == 0)
        {
            func_bt_mp3_res_play(RES_BUF_RING_VOICE_DIAL_MP3, RES_LEN_RING_VOICE_DIAL_MP3);
        }

        if(++f_online_factory_test->music_sec==6)
        {
            f_online_factory_test->music_sec = 0;
        }
#endif
        f_online_factory_test->color_idx = (f_online_factory_test->color_idx + 1) % (sizeof(dis_color) / sizeof(uint16_t));
        compo_shape_set_color(shape, dis_color[f_online_factory_test->color_idx]);
    }

    char str_buff[40];

    ute_module_factory_test_data_t *testData = NULL;
    testData = (ute_module_factory_test_data_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_factory_test_data_t));
    uteModuleFactoryTestGetData(testData);

    // 创建文本
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_CHARGE_STA);

    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"电池:%s %d%%",uteDrvBatteryCommonGetChargerStatus() == BAT_STATUS_NO_CHARGE ? "未充电" : "充电中", uteDrvBatteryCommonGetLvl());
    compo_textbox_set(textbox, str_buff);

    textbox = compo_getobj_byid(COMPO_ID_TXT_HR_VALUE);
    memset(str_buff, 0, sizeof(str_buff));
#if UTE_MODULE_HEART_SUPPORT
    snprintf(str_buff, sizeof(str_buff),"心率:%s %d bpm",uteModuleHeartIsWear() ? "佩戴中" : "未佩戴", testData->u.aging.heart);
#endif
    compo_textbox_set(textbox, str_buff);

    textbox = compo_getobj_byid(COMPO_ID_TXT_ACC_AXES);
    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"X:%d Y:%d Z:%d",testData->u.aging.gsensorX, testData->u.aging.gsensorY, testData->u.aging.gsensorZ);
    compo_textbox_set(textbox, str_buff);

    textbox = compo_getobj_byid(COMPO_ID_TXT_RUNNING_TIME);
    memset(str_buff, 0, sizeof(str_buff));
    snprintf(str_buff, sizeof(str_buff),"已运行 %02d:%02d:%02d",(uint16_t)(testData->u.aging.runningTimeSecond / 3600), (uint16_t)((testData->u.aging.runningTimeSecond % 3600) / 60), (uint16_t)(testData->u.aging.runningTimeSecond % 60));
    compo_textbox_set(textbox, str_buff);

    uteModulePlatformMemoryFree(testData);

    reset_guioff_delay();
}

typedef struct //tp九宫格
{
    s16 x;
    s16 y;
} f_tp_touch_coord_t;

// 创建九宫格窗口
static compo_form_t * func_online_factory_test_tp_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t *shape;
    compo_button_t *btn;

    static const f_tp_touch_coord_t f_tp_touch_coord_xy[] =
    {
        {GUI_SCREEN_CENTER_X + GUI_SCREEN_WIDTH / 3, GUI_SCREEN_CENTER_Y - GUI_SCREEN_HEIGHT / 3},
        {GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - GUI_SCREEN_HEIGHT / 3},
        {GUI_SCREEN_CENTER_X - GUI_SCREEN_WIDTH / 3, GUI_SCREEN_CENTER_Y - GUI_SCREEN_HEIGHT / 3},

        {GUI_SCREEN_CENTER_X + GUI_SCREEN_WIDTH / 3, GUI_SCREEN_CENTER_Y},
        {GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y},
        {GUI_SCREEN_CENTER_X - GUI_SCREEN_WIDTH / 3, GUI_SCREEN_CENTER_Y},

        {GUI_SCREEN_CENTER_X + GUI_SCREEN_WIDTH / 3, GUI_SCREEN_CENTER_Y + GUI_SCREEN_HEIGHT / 3},
        {GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + GUI_SCREEN_HEIGHT / 3},
        {GUI_SCREEN_CENTER_X - GUI_SCREEN_WIDTH / 3, GUI_SCREEN_CENTER_Y + GUI_SCREEN_HEIGHT / 3},
    };

    for (int i = 0; i < 9; i++)
    {
        shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_location(shape, f_tp_touch_coord_xy[i].x, f_tp_touch_coord_xy[i].y, GUI_SCREEN_WIDTH / 3 - 2, GUI_SCREEN_HEIGHT / 3 - 2);
        compo_setid(shape, i + COMPO_ID_TP_SHAPE_1);
        compo_shape_set_color(shape, COLOR_RED);

        btn = compo_button_create(frm);
        compo_button_set_location(btn, f_tp_touch_coord_xy[i].x, f_tp_touch_coord_xy[i].y, GUI_SCREEN_WIDTH / 3 - 2, GUI_SCREEN_HEIGHT / 3 - 2);
        compo_setid(btn, i + COMPO_ID_TP_BTN_1);
    }

    return frm;
}

// 九宫格界面刷新
static void func_online_factory_test_tp_process(void)
{
    ute_module_factory_test_data_t *testData = NULL;
    testData = (ute_module_factory_test_data_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_factory_test_data_t));
    uteModuleFactoryTestGetData(testData);

    for (uint8_t i = 0; i < 9; i++)
    {
        compo_shape_t *shape = compo_getobj_byid(COMPO_ID_TP_SHAPE_1 + i);
        if(testData->u.touch.graph[i])
        {
            compo_shape_set_color(shape, COLOR_GREEN);
        }
        else
        {
            compo_shape_set_color(shape, COLOR_RED);
        }
    }

    uteModulePlatformMemoryFree(testData);
    reset_guioff_delay();
}

// 九宫格界面按键处理
static void func_online_factory_test_tp_click(int bnt_id)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;
    if (f_online_factory_test->test_item == TEST_ITEM_TOUCH)
    {
        if(bnt_id < COMPO_ID_TP_BTN_1 || bnt_id > COMPO_ID_TP_BTN_9)
        {
            return;
        }
        uint8_t index = bnt_id - COMPO_ID_TP_BTN_1;
        printf("func_online_factory_test_tp_click index = %d\n", index);
        ute_module_factory_test_data_t *testData = NULL;
        testData = (ute_module_factory_test_data_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_factory_test_data_t));
        uteModuleFactoryTestGetData(testData);
        testData->u.touch.graph[index] = 1;
        uteModuleFactoryTestSetData(*testData);
        uteModulePlatformMemoryFree(testData);

        uint8_t response[4];
        response[0] = CMD_FACTORY_TEST_MODE;
        response[1] = 0x00;
        response[2] = 0x0a;
        response[3] = index + 1;
        uteModuleProfileBleSendToPhone(&response[0], 4);
    }
}

// 创建咪头喇叭窗口
static compo_form_t * func_online_factory_test_mic_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_textbox_t *textbox = compo_textbox_create(frm, 8);
    compo_textbox_set(textbox, "麦克风、喇叭测试");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT / 5);
    compo_setid(textbox, COMPO_ID_TXT_TITLE);

    //创建矩形
    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_setid(shape, COMPO_ID_MIC_RECT_START);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);
    compo_shape_set_radius(shape, 6);
    compo_shape_set_color(shape, MIC_TEST_EN ? COLOR_DARKGREEN : COLOR_GRAY);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_setid(shape, COMPO_ID_MIC_RECT_PLAY);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);
    compo_shape_set_radius(shape, 6);
//    compo_shape_set_color(shape, MIC_BUF_IS_FULL ? COLOR_DARKGREEN : COLOR_GRAY);
    compo_shape_set_color(shape, COLOR_GRAY);

    textbox = compo_textbox_create(frm, 8);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 5);
    compo_textbox_set(textbox, "录音");

    textbox = compo_textbox_create(frm, 8);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 5);
    compo_textbox_set(textbox, "播放");

    //创建按钮
    compo_button_t *btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_MIC_BTN_START);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_MIC_BTN_PLAY);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 5, GUI_SCREEN_WIDTH / 2, GUI_SCREEN_HEIGHT / 6);

    return frm;
}

//麦克风测试功能事件处理
static void func_online_factory_test_mic_process(void)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;

    if (f_online_factory_test->play_need)
    {
        //  uteModuleMicRecordFactoryPlay();
        compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_MIC_RECT_START), COLOR_DARKGREEN);
        compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_MIC_RECT_PLAY), COLOR_DARKGREEN);
        f_online_factory_test->play_need = false;
    }
    if (f_online_factory_test->sta == MIC_TEST_STA_RECORDING && uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_RECORDED)
    {
        f_online_factory_test->sta = MIC_TEST_STA_FULL;
        f_online_factory_test->play_need = true;
    }

    reset_guioff_delay();
}

static void func_online_factory_test_mic_click(int bnt_id)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;
    if (bnt_id == COMPO_ID_MIC_BTN_START)
    {
        if (MIC_TEST_EN && f_online_factory_test->sta != MIC_TEST_STA_RECORDING)
        {
            compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_MIC_RECT_START), COLOR_GRAY);
            compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_MIC_RECT_PLAY), COLOR_GRAY);
            f_online_factory_test->sta = MIC_TEST_STA_RECORDING;
            uteModuleMicRecordFactoryStart();
        }
    }
    else if (bnt_id == COMPO_ID_MIC_BTN_PLAY)
    {
        if (f_online_factory_test->sta == MIC_TEST_STA_FULL)
        {
            compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_MIC_RECT_START), COLOR_GRAY);
            compo_shape_set_color((compo_shape_t *)compo_getobj_byid(COMPO_ID_MIC_RECT_PLAY), COLOR_GRAY);
            f_online_factory_test->play_need = true;
        }
    }
}

// 创建测试窗口
compo_form_t *func_online_factory_test_form_create(void)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;
    f_online_factory_test->test_item = uteModuleFactoryTestGetCurrTestItem();
    f_online_factory_test->tick = tick_get();

    printf("f_online_factory_test->test_item = %d\n", f_online_factory_test->test_item);

    if (f_online_factory_test->test_item == TEST_ITEM_AGING)
    {
        return func_online_factory_test_ageing_create();
    }
    else if (f_online_factory_test->test_item == TEST_ITEM_BATTERY)
    {
        return func_online_factory_test_battery_create();
    }
    else if (f_online_factory_test->test_item == TEST_ITEM_SCREEN_RGB)
    {
        return func_online_factory_test_screen_rgb_create();
    }
    else if (f_online_factory_test->test_item == TEST_ITEM_VCXX_CHECK_LIGHT)
    {
        return func_online_factory_test_check_light_create();
    }
    else if (f_online_factory_test->test_item == TEST_ITEM_TOUCH)
    {
        return func_online_factory_test_tp_create();
    }
    else if (f_online_factory_test->test_item == TEST_ITEM_MIC_SPEAKER)
    {
        return func_online_factory_test_mic_create();
    }

    compo_form_t *frm = compo_form_create(true);
    compo_textbox_t *textbox = compo_textbox_create(frm, 8);
    compo_textbox_set(textbox, "不支持此项测试！");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 1 / 6);
    compo_setid(textbox, COMPO_ID_TXT_TITLE);
    return frm;
}

// 麦克风测试功能事件处理
static void func_online_factory_test_process(void)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;
    // printf("test_item = %d, CurrTestItem = %d\n", f_online_factory_test->test_item, uteModuleFactoryTestGetCurrTestItem());
    if (f_online_factory_test->test_item != uteModuleFactoryTestGetCurrTestItem())
    {
        if(f_online_factory_test->test_item == TEST_ITEM_MIC_SPEAKER)
        {
            uteModuleMicRecordFactoryExit();
        }
        else if (uteModuleFactoryTestGetCurrTestItem() == TEST_ITEM_MIC_SPEAKER)
        {
            uteModuleMicRecordFactoryEnter();
        }
        if(func_cb.frm_main)
        {
            compo_form_destroy(func_cb.frm_main);
            func_cb.frm_main = func_online_factory_test_form_create();
            if(f_online_factory_test->ptm)
            {
                func_free(f_online_factory_test->ptm);
                f_online_factory_test->ptm = NULL;
            }
        }
    }
    else
    {
        if (f_online_factory_test->test_item == TEST_ITEM_VCXX_CHECK_LIGHT)
        {
            func_online_factory_test_check_light_process();
        }
        else if (f_online_factory_test->test_item == TEST_ITEM_SCREEN_RGB)
        {
            func_online_factory_test_screen_rgb_process();
        }
        else if (f_online_factory_test->test_item == TEST_ITEM_BATTERY)
        {
            func_online_factory_test_battery_process();
        }
        else if (f_online_factory_test->test_item == TEST_ITEM_AGING)
        {
            func_online_factory_test_ageing_process();
        }
        else if (f_online_factory_test->test_item == TEST_ITEM_TOUCH)
        {
            func_online_factory_test_tp_process();
        }
        else if (f_online_factory_test->test_item == TEST_ITEM_MIC_SPEAKER)
        {
            func_online_factory_test_mic_process();
        }

        if(f_online_factory_test->ptm)
        {
            compo_page_move_process(f_online_factory_test->ptm);
        }

        func_process();
    }
}

// 单击按钮
static void func_online_factory_test_button_click(void)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;

    if (f_online_factory_test->test_item == TEST_ITEM_VCXX_CHECK_LIGHT)
    {
        uint8_t light_mode = uteModuleFactoryTestGetCheckLightMode();
        light_mode++;
#if 0
        if (light_mode > FACTORY_VCXX_TEST_MODE_RED_LIGHT)
#else
        if (light_mode > FACTORY_VCXX_TEST_MODE_INFRARED)
#endif
        {
            light_mode = FACTORY_VCXX_TEST_MODE_CROSSTALK;
        }
        uteModuleFactoryTestSetCheckLightMode(light_mode);
        return;
    }

    int id = compo_get_button_id();

    if (f_online_factory_test->test_item == TEST_ITEM_TOUCH)
    {
        func_online_factory_test_tp_click(id);
    }
    else if (f_online_factory_test->test_item == TEST_ITEM_MIC_SPEAKER)
    {
        func_online_factory_test_mic_click(id);
    }
}

// 麦克风测试功能消息处理
static void func_online_factory_test_message(size_msg_t msg)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_online_factory_test_button_click();
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        case MSG_CTP_TOUCH:
            if(f_online_factory_test->ptm)
            {
                compo_page_move_touch_handler(f_online_factory_test->ptm);
            }
            break;


        case KL_BACK:
        {
            uint8_t ret = msgbox("退出当前测试？", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
            if (ret == MSGBOX_RES_OK)
            {
                if (sys_cb.mp3_res_playing)
                {
                    music_control(MUSIC_MSG_STOP);
                }
                uteModuleGuiCommonGoBackLastScreen();
            }
        }
        break;

        default:
            evt_message(msg);
            break;
    }
}

// 进入麦克风测试控制功能
static void func_online_factory_test_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_online_factory_test_t));
    func_cb.frm_main = func_online_factory_test_form_create();

    uteModuleGuiCommonDisplayOffAllowGoBack(false);
}

// 退出麦克风测试功能
static void func_online_factory_test_exit(void)
{
    f_online_factory_test_t *f_online_factory_test = (f_online_factory_test_t *)func_cb.f_cb;
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    if (uteModuleFactoryTestGetCurrTestItem() != TEST_ITEM_NONE)
    {
        uteModuleFactoryTestStop();
    }
    if(f_online_factory_test->ptm)
    {
        func_free(f_online_factory_test->ptm);
        f_online_factory_test->ptm = NULL;
    }
}

// 麦克风测试功能
void func_online_factory_test(void)
{
    printf("%s\n", __func__);
    func_online_factory_test_enter();

    while (func_cb.sta == FUNC_ONLINE_FACTORY_TEST)
    {
        func_online_factory_test_process();
        func_online_factory_test_message(msg_dequeue());
    }
    func_online_factory_test_exit();
}
