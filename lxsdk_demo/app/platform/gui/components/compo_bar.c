#include "include.h"
#include "compo_bar.h"

#define TRACE_EN 1

#if TRACE_EN
#define TRACE(msg, ...)  do{printf("[ERROR][%s][%d]: ", __FUNCTION__, __LINE__); \
                            printf(msg, ##__VA_ARGS__); printf("\r\n");\
                         }while(0);
#else
#define TRACE(msg, ...)
#endif

#define BAR_VALUE_DEFAULT   0x7fffffff

/**
 * @brief 创建一个进度条组件
 * @param[in] frm : 窗体指针
 * @return 返回进度条句柄
 **/
compo_bar_t *compo_bar_create(compo_form_t *frm)
{
    compo_bar_t *compo_bar = compo_create(frm, COMPO_TYPE_BAR);
    compo_bar->bar = widget_bar_create(frm->page_body);
    compo_bar->cur_value = BAR_VALUE_DEFAULT;
    compo_bar->end_value = BAR_VALUE_DEFAULT;

    return compo_bar;
}

/**
 * @brief 设置进度条的坐标及大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] bar : 进度条句柄
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 进度条组件宽度
 * @param[in] height : 进度条组件高度
 * @return 无
 **/
void compo_bar_set_location(compo_bar_t *bar, s16 x, s16 y, s16 width, s16 height)
{
    widget_set_location(bar->bar, x, y, width, height);
}

/**
 * @brief 进度条刷新
 * @param[in] bar : 进度条句柄
 * @param[in] forward_dir_update : 是否刷新前进方向
 * @param[in] range_update : 是否刷新范围
 * @return 无
 **/
static void compo_bar_refresh(compo_bar_t *bar, bool forward_dir_update, bool range_update)
{
    /* 对于gui库的widget_bar，仅使用默认前进方向(左到右，上到下)，且min绝对小于max */
    int32_t min = bar->start_value < bar->end_value ? bar->start_value : bar->end_value;
    int32_t max = bar->start_value > bar->end_value ? bar->start_value : bar->end_value;
    if (bar->end_value != BAR_VALUE_DEFAULT && bar->cur_value != BAR_VALUE_DEFAULT)
    {
        if (bar->cur_value < min || bar->cur_value > max)
        {
            TRACE("cur value over range!%d %d",bar->cur_value < min,bar->cur_value > max);
            halt(HALT_GUI_COMPO);
        }
    }
    //更新方向
    if (forward_dir_update)
    {
        if (bar->forward_dir)
        {
            widget_bar_set_color(bar->bar, bar->bg_color, bar->content_color);
            if(bar->alpha1 || bar->alpha2)
            {
                widget_bar_set_alpha(bar->bar, bar->alpha2, bar->alpha1);
            }
        }
        else
        {
            widget_bar_set_color(bar->bar, bar->content_color, bar->bg_color);
            if(bar->alpha1 || bar->alpha2)
            {
                widget_bar_set_alpha(bar->bar, bar->alpha1, bar->alpha2);
            }
        }
    }
    //更新范围
    if (range_update)
    {
        widget_bar_set_range(bar->bar, min, max);
    }
    //刷新值
    if (bar->forward_dir)
    {
        // printf("cur_value:%d,min_value:%d,max_value:%d\n",
        //     max - abs(bar->cur_value - bar->start_value), min, max);
        widget_bar_set_value(bar->bar, max - abs(bar->cur_value - bar->start_value));
    }
    else
    {
        // printf("cur_value:%d,min_value:%d,max_value:%d\n",
        //     min + abs(bar->cur_value - bar->start_value), min, max);
        widget_bar_set_value(bar->bar, min + abs(bar->cur_value - bar->start_value));
    }
}

/**
 * @brief 设置进度条范围
 * @param[in] bar : 进度条句柄
 * @param[in] start_value : 进度条前进方向起始值
 * @param[in] end_value : 进度条前进方向结束值
 * @return 无
 **/
void compo_bar_set_range(compo_bar_t *bar, int32_t start_value, int32_t end_value)
{
    bar->start_value = start_value;
    bar->end_value = end_value;

    compo_bar_refresh(bar, false, true);
}

/**
 * @brief 设置进度条的值
 * @param[in] bar : 进度条句柄
 * @param[in] value : 设置进度条范围内的值
 * @return 无
 **/
void compo_bar_set_value(compo_bar_t *bar, int32_t value)
{
    bar->cur_value = value;
    compo_bar_refresh(bar, false, false);
}

/**
 * @brief 设置进度条的显示方向
 * @param[in] bar : 进度条句柄
 * @param[in] dir : 显示方向：COMPO_BAR_DIR_VERTICAL or COMPO_BAR_DIR_HORIZONTAL
 * @return 无
 **/
void compo_bar_set_dir(compo_bar_t *bar, bool dir)
{
    widget_bar_set_vh(bar->bar, dir);
    bar->dir = dir;
}

/**
 * @brief 设置进度条的前进方向
 * @param[in] bar : 进度条句柄
 * @param[in] forward_dir : 前进方向：
 *                          dir=COMPO_BAR_DIR_VERTICAL   0:从上到下  1:从下到上
 *                          dir=COMPO_BAR_DIR_HORIZONTAL 0:从左到右  1:从右到左
 * @return 无
 **/
void compo_bar_set_forward_dir(compo_bar_t *bar, bool forward_dir)
{
    bar->forward_dir = forward_dir;
    compo_bar_refresh(bar, true, false);
}

/**
 * @brief 设置进度条的颜色
 * @param[in] bar : 进度条句柄
 * @param[in] content_color : 设置值区域的颜色
 * @param[in] bg_color: 设置值外的颜色
 * @return 无
 **/
void compo_bar_set_color(compo_bar_t *bar, u16 content_color, u16 bg_color)
{
    bar->bg_color = bg_color;
    bar->content_color = content_color;
    compo_bar_refresh(bar, true, false);
}

/**
 * @brief 设置进度条的alpha
 * @param[in] bar : 进度条句柄
 * @param[in] alpha1 : 设置值区域的alpha
 * @param[in] alpha2 : 设置值外的alpha
 * @return 无
 **/
void compo_bar_set_alpha(compo_bar_t *bar,u8 alpha1, u16 alpha2)
{
    bar->alpha1 = alpha1;
    bar->alpha2 = alpha2;
    compo_bar_refresh(bar, true, false);
}


/**
 * @brief 设置进度条的圆角半径
 * @param[in] bar : 进度条句柄
 * @param[in] r : 圆角半径
                  当半大于高或宽的一半时，底层自动把半径设置为其中最小的；(高或宽的一半)
 * @return 无
 **/
void compo_bar_set_rabardius(compo_bar_t *bar, u16 r)
{
    widget_bar_set_rabardius(bar->bar, r);
}

/**
 * @brief 设置进度条是否按下
 * @param[in] roller : 进度条句柄
 * @param[in] body_value : 是否使能触摸进度条本身才能滑动
 * @return 是否有效
 **/
bool compo_bar_move_status(compo_bar_t *bar, bool body_touch)
{
    point_t cur_point = ctp_get_sxy();
    rect_t rect = widget_get_absolute((widget_t *)bar->bar);
    bool ret = true;

    if (abs_s(cur_point.x - rect.x) * 2 <= rect.wid && abs_s(cur_point.y - rect.y) * 2 <= rect.hei)
    {
        bar->move_ctr.touch_point = cur_point;
    }
    else
    {
        if (body_touch)
        {
            ret = false;
        }
    }
    bar->move_ctr.touch_flag = ret;

    return ret;
}

/**
 * @brief 进度条滑动控制 (在func中轮询)
 * @param[in] bar : 进度条句柄
 * @return 无
 **/
void compo_bar_move_ctr(compo_bar_t *bar)
{
    if (bar->move_ctr.touch_flag)
    {
        s32 dx, dy;
        bool touch_flag = ctp_get_dxy(&dx, &dy);
        int32_t min = bar->start_value < bar->end_value ? bar->start_value : bar->end_value;
        int32_t max = bar->start_value > bar->end_value ? bar->start_value : bar->end_value;

        if (touch_flag)
        {
            s32 offset = 0;
            s32 cur_move = bar->dir == COMPO_BAR_DIR_HORIZONTAL ? dx : dy;

            offset = cur_move - bar->move_ctr.last_move;
            if (0 == offset)
            {
                return ;
            }
            if (bar->forward_dir)
            {
                offset = -offset;
            }
            bar->move_ctr.move_flag = true;
            bar->move_ctr.last_move = cur_move;

            rect_t rect = widget_get_absolute((widget_t *)bar->bar);
            int32_t change_value = (bar->end_value - bar->start_value) * offset / (bar->dir == COMPO_BAR_DIR_HORIZONTAL ? rect.wid : rect.hei);

            change_value += bar->cur_value;
            if (change_value > max)
            {
                change_value = max;
            }
            else if (change_value < min)
            {
                change_value = min;
            }
            if (bar->cur_value != change_value)
            {
                compo_bar_set_value(bar, change_value);
                if (bar->move_ctr.value_change_cb)
                {
                    bar->move_ctr.value_change_cb(change_value);
                }
            }
        }
        else
        {
            if (!bar->move_ctr.move_flag)
            {
                if (bar->move_ctr.touch_point.x != 0 || bar->move_ctr.touch_point.y != 0)
                {
                    rect_t rect = widget_get_absolute((widget_t *)bar->bar);
                    s32 offset = bar->dir == COMPO_BAR_DIR_HORIZONTAL ? (bar->move_ctr.touch_point.x - rect.x + rect.wid / 2) : (bar->move_ctr.touch_point.y - rect.y + rect.hei / 2);
                    if (bar->forward_dir)
                    {
                        offset = bar->dir == COMPO_BAR_DIR_HORIZONTAL ? (rect.wid - offset) : (rect.hei - offset);
                    }
                    int32_t change_value = (bar->end_value - bar->start_value) * offset / (bar->dir == COMPO_BAR_DIR_HORIZONTAL ? rect.wid : rect.hei);
                    change_value += bar->start_value;
                    if (change_value != bar->cur_value)
                    {
                        compo_bar_set_value(bar, change_value);
                        if (bar->move_ctr.value_change_cb)
                        {
                            bar->move_ctr.value_change_cb(change_value);
                        }
                    }
                }
            }
            bar->move_ctr.touch_flag = false;
            bar->move_ctr.move_flag = false;
            bar->move_ctr.last_move = 0;
        }
    }
}

/**
 * @brief 设置进度条是否按下
 * @param[in] roller : 进度条句柄
 * @param[in] cb : 因滑动或点击导致值变化时调用
 * @return 无
 **/
void compo_bar_set_value_change_cb(compo_bar_t *bar, value_change_cb_t func)
{
    bar->move_ctr.value_change_cb = func;
}
