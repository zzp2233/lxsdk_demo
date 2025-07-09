#ifndef __BSP_CHARGE_EX_H__
#define __BSP_CHARGE_EX_H__

#include "bsp_charge_ex_bat.h"
#include "bsp_charge_ex_hal.h"
#include "bsp_hall.h"
#include "bsp_earphone_manage.h"
#include "bsp_smart_vhouse_host.h"
#include "lp7812c/lp7812c.h"
#include "charge_ic_comm/charge_ic.h"

#define BOX_SOTP_CHARGE                     0                                   //停充检测方法
#define BOX_CURVE_FIT                       1                                   //充电曲线拟合方法

#define STOP_CHARE_START                    10                                  //在每个检测周期中, 停充的开始时间(s)
#define STOP_CHARE_DETECT                   (STOP_CHARE_START + 3)              //在每个检测周期中, 停充后去采集电压的时间(s)
#define STOP_CHARE_PERIOD                   40                                  //停充检测周期(s)

#define BOX_CURVE_FIT_CV_PERCENT            81                                      //设置进恒压的电量(%), 恒压后采用定时的方式加减电量
#define BOX_CURVE_FIT_CV_PERCENT_ADD        1                                       //设置电量百分比累加值(%)
#define BOX_CURVE_FIT_CV_PERCENT_PR_CHARGE  (1*60*1000)                             //设置电量百分比累加周期(ms), 充电
#define BOX_CURVE_FIT_CV_PERCENT_PR         (BOX_CURVE_FIT_CV_PERCENT_PR_CHARGE*5)  //设置电量百分比累加周期(ms), 放电

#define BSP_CHARGE_EX_BOX_DETECT            BOX_CURVE_FIT                       //仓电量显示方法
#define BOX_PECENT_TBL_SHOW                 0                                   //仓电量是否打开按照档位显示
#define BOX_PERCENT_CHANGE_PR               8                                   //仓电量变化间隔(放电, 单位:s)
#define BOX_PERCENT_CHANGE_PR_CHARGE        15                                  //仓电量变化间隔(充电, 单位:s)
#define BSP_CHARGE_EX_VBAT_CORRECTED        1                                   //是否打开电压采集修正


#if VBAT_DETECT_EN
//仓电压均值滤波系数(2^K倍), 该值越大, 电压变化速度越慢
#define BOX_VBAT_AVERAGE_FILTER_K           5                                   //主控采集电压时
#else
#define BOX_VBAT_AVERAGE_FILTER_K           9                                   //外部充电ic采集电压时
#endif

#define BSP_CHARGE_EX_COMM_ALLOW_TICK       500                                 //外部充电ic允许通讯控制延时(ms)

#define PERCENT_RUN_OUT                     0xfe                                                    //电量百分比低于0%
#define PERCENT_NOT_INIT                    0xff                                                    //电量百分比未初始化
#define CHARGE_EX_BAT_RUN_OUT()             (charge_ex_cb.percent == PERCENT_RUN_OUT)               //电量低于0%
#define CHARGE_EX_BAT_IS_LOW()              (bsp_charge_ex_percent_get() <= BOX_LPWR_PERCENT)       //电量低于5%
#define CHARGE_EX_BAT_LOW()                 (sys_cb.bat_low == true)                                //仓处于低电模式
#define CHARGE_EX_COMM_IS_BACKUP()          (charge_ex_cb.mode_backup ? 1:0)
#define charge_ex_mode_is_comm()            (bsp_charge_ex_mode() == BOX_COMM_MODE && !CHARGE_EX_COMM_IS_BACKUP()) //仓处于常规通讯模式

//外部充电ic工作模式
enum
{
    BOX_NULL_MODE = 0x00,       //未初始化
    BOX_CHARGE_MODE,            //充电模式
    BOX_COMM_MODE,              //通讯模式
    BOX_CHK_MODE,               //入仓检测模式
};

typedef struct
{
    bool chg;                                       //0:未充电 1:正在给vbat充电
    bool pgd;                                       //0:无USB  1:有USB插入给vbat充电
    bool l_inbox;
    bool r_inbox;
    u8 ntc;                                         //charge ic上报温度
    u16 v_batm;                                     //charge ic上报电池电量

    u32 vbat_total;
    u32 vbat_update_tick;
    u8 percent;                                     //电量百分比

    bool comm_delay;                                //延时允许通讯标志
    u8 mode_backup;                                 //工作模式备份

    u32 usb_insert_tick;
//#if BOX_OPEN_WAKEUP_EARPHONE
    u32 wkup_0V_tick;
    u32 wkup_5V_tick;
//#endif
#if (BSP_CHARGE_EX_BOX_DETECT == BOX_SOTP_CHARGE)
    u16 stop_charge_cnt;                            //周期计数
    bool stop_charge_detect;
#endif

#if (BSP_CHARGE_EX_BOX_DETECT == BOX_CURVE_FIT)
    u8 percent_cv;                                  //恒压阶段电量百分比
    u16 percent_cv_tick_cnt;                        //恒压阶段电量变化, 累计时间(单位:s)
    u32 percent_cv_tick;                            //恒压阶段电量变化计时(单位:ms)
#endif
} charge_ex_cb_t;
extern charge_ex_cb_t charge_ex_cb;

void bsp_charge_ex_usb_insert_callback(bool insert);
void bsp_charge_ex_vbat_update_callback(u16 v_batm);
void bsp_charge_ex_inbox_callback(bool l_inbox, bool r_inbox);
void bsp_charge_ex_eoc_callback(bool l_eoc, bool r_eoc);
void bsp_charge_ex_init(void);
void bsp_charge_ex_process(void);

void box_vbat_run_out_callback(void);

/**
 * @brief 获取电压值
 * @return 电压值(mv)
 **/
u16 bsp_charge_ex_vbat_get(void);

/**
 * @brief 检测电量是否过低（小于20%）
            若电量低，调低屏幕亮度并设置系统标志位，否则恢复亮度并清除标志位
 * @return 无
 **/
void bsp_charge_ex_bat_low_check(void);

/**
 * @brief 外部充电IC是否允许通讯
 **/
bool bsp_charge_ex_comm_allow(void);

/**
 * @brief VOL VOR输出0V
 * @return 无
 **/
void bsp_charge_ex_open_wakeup(void);

/**
 * @brief VOL VOR输出5V
 * @return 无
 **/
void bsp_charge_ex_5V_wakeup(void);

/**
 * @brief 非通讯模式下发命令备份
 * @return 是否触发备份
 **/
bool bsp_charge_ex_comm_backup(void);

/**
 * @brief 非通讯模式下发命令还原
 * @return 无
 **/
void bsp_charge_ex_comm_recover(void);

/**
 * @brief 设置外部充电ic工作模式
 * @param[in] mode: 工作模式
 * @return 是否设置成功
 **/
bool bsp_charge_ex_mode_set(u8 mode);

/**
 * @brief 外部充电ic设置仓电量
 * @param[in] vbat: 仓电量(mv)
 **/
void bsp_charge_ex_vbat_update(u16 vbat);

/**
 * @brief 获取仓电量
 * @return 仓电量(0~100%); 仓电量低于0%, 电量耗光(0xfe); 仓电量未初始化(0xff);
 **/
u8 bsp_charge_ex_percent_get(void);

#if (BSP_CHARGE_EX_BOX_DETECT == BOX_SOTP_CHARGE)
void bsp_charge_ex_stop_charge_start(void);
void bsp_charge_ex_stop_charge_detect_vbat(u16 vbat);
#endif

#endif /* __BSP_CHARGE_EX_H__*/
