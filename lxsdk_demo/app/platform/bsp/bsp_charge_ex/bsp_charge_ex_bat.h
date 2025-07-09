#ifndef __BSP_CHARGE_EX_BAT_H__
#define __BSP_CHARGE_EX_BAT_H__

#define BOX_VBAT_CORRECTED_TRACE        0       //是否打开电压修正值打印

enum
{
    BOX_PERCENT_0,
    BOX_PERCENT_10,
    BOX_PERCENT_20,
    BOX_PERCENT_30,
    BOX_PERCENT_40,
    BOX_PERCENT_50,
    BOX_PERCENT_60,
    BOX_PERCENT_70,
    BOX_PERCENT_80,
    BOX_PERCENT_90,
    BOX_PERCENT_100,
    BOX_PERCENT_MAX = 101,
};

extern const u16 tbl_box_percent[];

/**
 * @brief 获取仓显示的电量档位
 * @return 电量
 **/
u8 bsp_charge_ex_percent_show_get(void);

/**
 * @brief 获取电压修正值
 * @return 电压修正值/补偿值
 **/
int bsp_charge_ex_get_correct(void);

#endif // __BSP_CHARGE_EX_BAT_H__
