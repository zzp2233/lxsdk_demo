#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#if CHARGE_EX_IC_SELECT
//此处根据电池分容数据调整, 电量(%), 电压(mv)
const u16 tbl_box_percent[BOX_PERCENT_MAX] =      //电池放电百分比表
{
//  00
    3100,
//  01    02    03    04    05    06    07    08    09    10
    3160, 3220, 3280, 3340, 3400, 3416, 3432, 3448, 3464, 3480,
//  11    12    13    14    15    16    17    18    19    20
    3490, 3500, 3510, 3520, 3530, 3540, 3550, 3560, 3570, 3580,
//  21    22    23    24    25    26    27    28    29    30
    3585, 3590, 3595, 3600, 3605, 3610, 3615, 3620, 3635, 3630,
//  31    32    33    34    35    36    37    38    39    40
    3636, 3642, 3648, 3654, 3660, 3666, 3672, 3678, 3684, 3690,
//  41    42    43    44    45    46    47    48    49    50
    3694, 3698, 3702, 3706, 3710, 3714, 3718, 3722, 3726, 3730,
//  51    52    53    54    55    56    57    58    59    60
    3735, 3740, 3745, 3750, 3755, 3760, 3765, 3770, 3775, 3780,
//  61    62    63    64    65    66    67    68    69    70
    3786, 3792, 3798, 3804, 3810, 3816, 3822, 3828, 3834, 3840,
//  71    72    73    74    75    76    77    78    79    80
    3848, 3856, 3864, 3872, 3880, 3888, 3896, 3904, 3912, 3920,
//  81    82    83    84    85    86    87    88    89    90
    3930, 3940, 3950, 3960, 3970, 3980, 3990, 4000, 4010, 4020,
//  91    92    93    94    95    96    97    98    99    100
    4036, 4044, 4052, 4060, 4068, 4076, 4084, 4084, 4092, 4100
};

#if BOX_PECENT_TBL_SHOW
//档位显示表
const u8 tbl_box_percent_show[] =
{
    0, BOX_LPWR_PERCENT, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100
};
#endif  //BOX_PECENT_TBL_SHOW

#if BSP_CHARGE_EX_VBAT_CORRECTED
//此处根据打印和电压表的实际读数做修正调整, 打印请打开BOX_VBAT_CORRECTED_TRACE
const int vbat_corrected = 0;                                  //电压采集修正(工作放电), 便于整体加减放电表测试
const int vbat_corrected_charge = 30;                          //电压采集修正(工作充电), 充电有略微虚电, 整体调高
const u8 vbat_corrected_idle = 30;                             //idle模式修正, 系统休眠加灭屏电池电压略高, 整体调高
//const int tbl_vbat_corrected_bright = -30;                   //屏亮度修正
const int vbat_corrected_earphone_charge[EARPHONE_MAX] =       //电压采集修正(耳机充电), 耳机充电电压略低, 整体调低
{
    -15, -15
    };

#endif // BSP_CHARGE_EX_VBAT_CORRECTED

u16 *bsp_charge_ex_get_tbl_percent(void)
{
    return (u16 *)tbl_box_percent;
}

u8 bsp_charge_ex_percent_show_get(void)
{
    u8 percent_show = 0xff;
    u8 percent = bsp_charge_ex_percent_get();

#if BOX_PECENT_TBL_SHOW
    u8 show_size = sizeof(tbl_box_percent_show);
    for (u8 i = 0; i < show_size; i++)
    {
        if (percent >= tbl_box_percent_show[i] && percent < tbl_box_percent_show[i + 1])
        {
            percent_show = tbl_box_percent_show[i];
            break;
        }
    }

    if (percent >= tbl_box_percent_show[show_size - 1])
    {
        percent_show = tbl_box_percent_show[show_size - 1];
    }
    printf("===> percent[%d -> %d%%] show_size[%d]\n", percent, percent_show, show_size);
#else
    percent_show = percent;
#endif
    return percent_show;
}

#if BSP_CHARGE_EX_VBAT_CORRECTED
int bsp_charge_ex_get_correct(void)
{
    int correct_value = 0;

    correct_value = vbat_corrected;
    if (charge_ex_cb.pgd)
    {
        correct_value += vbat_corrected_charge;
    }

    if (bsp_earphone_is_charge(EARPHONE_LEFT))
    {
        correct_value += vbat_corrected_earphone_charge[EARPHONE_LEFT];
    }

    if (bsp_earphone_is_charge(EARPHONE_RIGHT))
    {
        correct_value += vbat_corrected_earphone_charge[EARPHONE_RIGHT];
    }

    if (func_cb.sta == FUNC_IDLE)
    {
        correct_value += vbat_corrected_idle;
    }

    return correct_value;
}
#endif // BSP_CHARGE_EX_VBAT_CORRECTED

#else
const u16 tbl_box_percent[] = {3100};
u8 bsp_charge_ex_percent_show_get(void)
{
    return 0;
}
#endif  //CHARGE_EX_IC_SELECT
