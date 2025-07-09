#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

/**
 * @brief 创建进度条组件
 * @param[in] frm : 窗体指针
 * @return 返回进度条指针
 **/
compo_progressbar_t *compo_progressbar_create(compo_form_t *frm, u16 progressbar_vh)
{
    compo_progressbar_t *compo_progressbar = compo_create(frm, COMPO_TYPE_PROGRESSBAR);
    if (compo_progressbar == NULL)
    {
        halt(HALT_GUI_COMPO);
    }
    compo_progressbar->bar = widget_bar_create(frm->page_body);
    widget_bar_set_value(compo_progressbar->bar, 0);
    widget_bar_set_vh(compo_progressbar->bar, progressbar_vh);
    return compo_progressbar;
}

/**
 * @brief 设置进度条的坐标及大小(指的是进度条组件自身的大小，而非进度条宽度)
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] arc : 进度条指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 进度条组件宽度
 * @param[in] height : 进度条组件高度
 * @return 无
 **/
void compo_progressbar_set_location(compo_progressbar_t *bar, s16 x, s16 y, s16 width, s16 height)
{
    if (bar == NULL)
    {
        halt(HALT_GUI_COMPO);
    }
    widget_set_location(bar->bar, x, y, width, height);
}

/**
 * @brief 设置进度条的大小（指的是进度条组件自身的大小，而非进度条宽度)
 * @param[in] arc : 进度条指针
 * @param[in] width : 进度条组件宽度
 * @param[in] height : 进度条组件高度
 * @return 无
 **/
void compo_progressbar_set_size(compo_progressbar_t *bar, s16 widget, s16 height)
{
    if (bar == NULL)
    {
        halt(HALT_GUI_COMPO);
    }
    widget_set_size(bar->bar, widget, height);
}


/**
 * @brief 设置进度条的范围(start_angle和end_angle可任意角度)
 * @param[in] arc : 进度条指针
 * @param[in] start_angle : 相对于0°顺时针方向角度 0 ~ 3600°
 * @param[in] end_angle : 相对于0°顺时针方向角度 0 ~ 3600°
 * @return 无
 **/
void compo_progressbar_set_range(compo_progressbar_t *bar, int32_t min, int32_t max)
{
    if (bar == NULL)
    {
        halt(HALT_GUI_COMPO);
    }
    bar->min = min;
    bar->max = max;
    widget_bar_set_range(bar->bar, min, max);
}

/**
 * @brief 通过设置进度条的值
 * @param[in] arc : 进度条指针
 * @param[in] value : 0 ~ ARC_VALUE_MAX
 * @return 无
 **/
void compo_progressbar_set_value(compo_progressbar_t *bar, int32_t value)
{
    if (bar == NULL)
    {
        halt(HALT_GUI_COMPO);
    }
    widget_bar_set_value(bar->bar, value);
}

/**
 * @brief 设置进度条的颜色
 * @param[in] arc : 进度条指针
 * @param[in] content_color : 设置的角度范围内的颜色
 * @param[in] bg_color : 设置的角度范围外的颜色
 * @return 无
 **/
void compo_progressbar_set_color(compo_progressbar_t *bar, u16 content_color, u16 bg_color)
{
    if (bar == NULL)
    {
        halt(HALT_GUI_COMPO);
    }
    widget_bar_set_color(bar->bar, content_color, bg_color);
}

/**
 * @brief 设置进度条截面是否进度条化
 * @param[in] arc : 进度条指针
 * @param[in] start_onoff: 开始角度位置 1:进度条化 0:直线化
 * @param[in] end_onoff  : 结束角度位置 1:进度条化 0:直线化
 * @return 无
 * 进度条化后目前缩放对齐有点偏差
 **/
void compo_progressbar_set_edge_circle(compo_progressbar_t *bar, u16 r)
{
    if (bar == NULL)
    {
        halt(HALT_GUI_COMPO);
    }
    widget_bar_set_rabardius(bar->bar, r);
}


