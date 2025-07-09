#include "include.h"

///默认参数，可以通过接口函数改变
#define SCROLLBAR_VALUE_MAX     1000
#define SCROLLBAR_BG_COLOR      make_color(113,121,139)
#define SCROLLBAR_FG_COLOR      make_color(187,184,204)
#define SCROLLBAR_BTN_RATIO     8
#define SCROLL_REBOUND_RATIO    3                       //滚动条内部按钮回弹最大比例

/**
 * @brief 创建一个滚动条
 * @param[in] frm : 窗体指针
 * @param[in] type : 滚动条类型
 * @return  返回滚动条指针
 */
compo_scroll_t* compo_scroll_create(compo_form_t* frm, u8 type)
{
    compo_scroll_t *compo_scroll = compo_create(frm, COMPO_TYPE_SCROLLBAR);
    if (NULL == compo_scroll) {
        halt(HALT_GUI_COMPO_ARC_FAIL);
    }
    compo_scroll->stype = type;
    compo_scroll->content_ratio = SCROLLBAR_BTN_RATIO;

    switch (type) {
    case SCROLL_TYPE_VERTICAL ... SCROLL_TYPE_HORIZONTAL:
        compo_scroll->scroll_bg = widget_rect_create(frm->page_body);
        compo_scroll->scroll_fg = widget_rect_create(frm->page_body);
        break;

    case SCROLL_TYPE_ARC:
        compo_scroll->scroll_bg = widget_arc_create(frm->page_body);
        compo_scroll->scroll_fg = widget_arc_create(frm->page_body);
        compo_scroll->scroll_arc.rotation_offset = 2700;
        break;
    }

    return compo_scroll;

}

/**
 * @brief 设置滚动条的宽度, 当滚动条为圆弧滚动条时，后面的可变参数则为设置圆弧半径
 * @param[in] scroll : 滚动条指针
 * @param[in] width : 滚动条的宽度
 * @param[in] ... : 可选参数 -> arg1 : 当滚动条为圆弧滚动条时，可变参数则为设置圆弧半径
 */
void compo_scroll_set_w_r(compo_scroll_t *scroll, u16 width, ...)
{
    if (NULL == scroll) {
        halt(HALT_GUI_COMPO_ARC_NULL);
    }

    va_list param;
    va_start(param, width);

    switch (scroll->stype) {
    case SCROLL_TYPE_VERTICAL ... SCROLL_TYPE_HORIZONTAL:
        scroll->scroll_rect.width = width;
        break;

    case SCROLL_TYPE_ARC: {
        u32 r = va_arg(param, u32);

        widget_set_size(scroll->scroll_bg, 2*r, 2*r);
        widget_set_size(scroll->scroll_fg, 2*r, 2*r);

        widget_arc_set_width(scroll->scroll_bg, width);
        widget_arc_set_width(scroll->scroll_fg, width);
    }
        break;

    default:
        break;
    }

    va_end(param);


}

/**
 * @brief 设置滚动条的内部滚动按钮占滚动条整体的比例大小
 * @param[in] scroll : 滚动条指针
 * @param[in] ratio : 滚动条内按钮占滚动条比例 1 / ratio
 */
void compo_scroll_set_content_ratio(compo_scroll_t* scroll, u8 ratio)
{
    if (NULL == scroll) {
        halt(HALT_GUI_COMPO_ARC_FAIL);
    }

    if (ratio <= 0) {
        ratio = SCROLLBAR_BTN_RATIO;
    }
    scroll->content_ratio = ratio;
}

/**
 * @brief 设置滚动条的可滚动范围
 * @param[in] scroll : 滚动条指针
 * @param[in] ... : 可选参数
 *                  当滚动条类型为 矩形横向/竖向滚动条时 arg1 -> 设置矩形滚动条的范围
 *                  当滚动条类型为 圆弧滚动条时  arg1 -> 设置圆弧的显示开始角度
                                                   arg2 -> 设置圆弧的显示结束角度
 */
void compo_scroll_set_range(compo_scroll_t* scroll, ...)
{
    if (NULL == scroll) {
        halt(HALT_GUI_COMPO_ARC_FAIL);
    }

    va_list param;
    va_start(param, scroll);

    switch (scroll->stype) {
    case SCROLL_TYPE_VERTICAL ... SCROLL_TYPE_HORIZONTAL:
        scroll->scroll_rect.scope = va_arg(param, u32);
        scroll->fg_range = scroll->scroll_rect.scope / scroll->content_ratio;

        widget_rect_set_color(scroll->scroll_bg, SCROLLBAR_BG_COLOR);
        widget_rect_set_color(scroll->scroll_fg, SCROLLBAR_FG_COLOR);

        widget_set_alpha(scroll->scroll_bg, 0xff);
        widget_set_alpha(scroll->scroll_fg, 0xff);

        break;
    case SCROLL_TYPE_ARC: {
            scroll->scroll_arc.bg_start_angle = va_arg(param, u32);
            scroll->scroll_arc.bg_end_angle = va_arg(param, u32);

            if (scroll->scroll_arc.bg_start_angle == scroll->scroll_arc.bg_end_angle) {
                break;
            }
            widget_arc_set_color(scroll->scroll_bg, SCROLLBAR_BG_COLOR, COLOR_BLUE);
            widget_arc_set_alpha(scroll->scroll_bg, 0xff, 0);

            u32 offset = abs_s(scroll->scroll_arc.bg_end_angle - scroll->scroll_arc.bg_start_angle) / scroll->content_ratio;
            scroll->fg_range = offset;
            scroll->scroll_arc.fg_start_angle = scroll->scroll_arc.bg_start_angle;
            scroll->scroll_arc.fg_end_angle = scroll->scroll_arc.fg_start_angle + offset;
            widget_arc_set_color(scroll->scroll_fg, SCROLLBAR_FG_COLOR, COLOR_BLUE);
            widget_arc_set_alpha(scroll->scroll_fg, 0xff, 0);
        } break;
    default:
        halt(HALT_GUI_COMPO_ARC_TYPE);
        break;
    }

    va_end(param);
}

/**
 * @brief 设置滚动条的颜色
 * @param[in] scroll : 滚动条指针
 * @param[in] content_color : 滚动条内滚动按钮颜色
 * @param[in] bg_color : 滚动条背景颜色
 */
void compo_scroll_set_color(compo_scroll_t* scroll, u16 content_color, u16 bg_color)
{
    if (NULL == scroll) {
        halt(HALT_GUI_COMPO_ARC_FAIL);
    }

    switch (scroll->stype) {
    case SCROLL_TYPE_VERTICAL ... SCROLL_TYPE_HORIZONTAL:
        widget_rect_set_color(scroll->scroll_bg, bg_color);
        widget_rect_set_color(scroll->scroll_fg, content_color);
        break;

    case SCROLL_TYPE_ARC:
        widget_arc_set_color(scroll->scroll_bg, bg_color, 0);
        widget_arc_set_alpha(scroll->scroll_bg, 0xff, 0);

        widget_arc_set_color(scroll->scroll_fg, content_color, 0);
        widget_arc_set_alpha(scroll->scroll_fg, 0xff, 0);
        break;

    default:
        break;
    }

}

/**
 * @brief 设置滚动条位置
 * @param[in] scroll : 滚动条指针
 * @param[in] x : 当滚动条为矩形滚动条时，设置滚动条 x 坐标（以滚动条的左上角为原点坐标）
 *                当滚动条为圆弧滚动条时，设置滚动条 x 坐标（以圆弧的中心为原点坐标）
 * @param[in] y : 当滚动条为矩形滚动条时，设置滚动条 y 坐标（以滚动条的左上角为原点坐标）
 *                当滚动条为圆弧滚动条时，设置滚动条 y 坐标（以圆弧的中心为原点坐标）
 */
void compo_scroll_set_pos(compo_scroll_t* scroll, s16 x, s16 y)
{
    if (NULL == scroll) {
        halt(HALT_GUI_COMPO_ARC_FAIL);
    }

    switch (scroll->stype) {
    case SCROLL_TYPE_ARC:
        widget_set_pos(scroll->scroll_bg, x, y);
        widget_set_pos(scroll->scroll_fg, x, y);
        break;

    case SCROLL_TYPE_VERTICAL ... SCROLL_TYPE_HORIZONTAL:
        widget_set_align_center(scroll->scroll_bg, false);
        widget_set_align_center(scroll->scroll_fg, false);
        scroll->scroll_rect.bg_xy.x = x;
        scroll->scroll_rect.bg_xy.y = y;
        break;
    }
}

/**
 * @brief 设置滚动条滚动按钮滚动的位置
 * @param[in] scroll : 滚动条指针
 * @param[in] value : 滚动值，最大为 SCROLLBAR_VALUE_MAX + 回弹值（回弹值通过设置回弹比例自动计算）
 *                            最小为 0 - 回弹值（回弹值通过设置回弹比例自动计算）
 */
void compo_scroll_set_value(compo_scroll_t* scroll, int value)
{
    if (NULL == scroll) {
        halt(HALT_GUI_COMPO_ARC_FAIL);
    }

    u32 fg_range = scroll->fg_range;
    if (value > SCROLLBAR_VALUE_MAX) {
        if (abs_s(SCROLLBAR_VALUE_MAX - value) <= scroll->fg_range / SCROLL_REBOUND_RATIO) {
            fg_range = scroll->fg_range - abs_s(SCROLLBAR_VALUE_MAX - value);
        } else {
            fg_range = scroll->fg_range - scroll->fg_range / SCROLL_REBOUND_RATIO;
        }
        value = SCROLLBAR_VALUE_MAX;
    } else if (value < 0) {
        if (abs_s(value) <= scroll->fg_range / SCROLL_REBOUND_RATIO) {
            fg_range = scroll->fg_range - abs_s(value);
        } else {
            fg_range = scroll->fg_range - scroll->fg_range / SCROLL_REBOUND_RATIO;
        }
        value = 0;
    }

    scroll->value = value;

    switch (scroll->stype) {
    case SCROLL_TYPE_ARC:
        widget_arc_set_angles(scroll->scroll_bg, scroll->scroll_arc.bg_start_angle + scroll->scroll_arc.rotation_offset % 3600,
                            scroll->scroll_arc.bg_end_angle + scroll->scroll_arc.rotation_offset % 3600);

        scroll->scroll_arc.fg_start_angle = scroll->scroll_arc.bg_start_angle;
        scroll->scroll_arc.fg_start_angle = scroll->scroll_arc.fg_start_angle +
                                    (abs_s(scroll->scroll_arc.bg_end_angle - scroll->scroll_arc.bg_start_angle) - fg_range) *
                                    value / SCROLLBAR_VALUE_MAX;
        scroll->scroll_arc.fg_end_angle = scroll->scroll_arc.fg_start_angle + fg_range;

        widget_arc_set_angles(scroll->scroll_fg, scroll->scroll_arc.fg_start_angle + scroll->scroll_arc.rotation_offset % 3600,
                            scroll->scroll_arc.fg_end_angle + scroll->scroll_arc.rotation_offset % 3600);
        break;

    case SCROLL_TYPE_VERTICAL:
        widget_set_size(scroll->scroll_bg, scroll->scroll_rect.width, scroll->scroll_rect.scope);
        widget_set_size(scroll->scroll_fg, scroll->scroll_rect.width, fg_range);


        widget_set_pos(scroll->scroll_bg, scroll->scroll_rect.bg_xy.x, scroll->scroll_rect.bg_xy.y);

        scroll->scroll_rect.fg_xy.x = scroll->scroll_rect.bg_xy.x;
        scroll->scroll_rect.fg_xy.y = scroll->scroll_rect.bg_xy.y;
        scroll->scroll_rect.fg_xy.y = scroll->scroll_rect.fg_xy.y +
                                    (abs_s(scroll->scroll_rect.scope) - fg_range) * value / SCROLLBAR_VALUE_MAX;
        widget_set_pos(scroll->scroll_fg, scroll->scroll_rect.fg_xy.x, scroll->scroll_rect.fg_xy.y);

        break;

    case SCROLL_TYPE_HORIZONTAL:
        widget_set_size(scroll->scroll_bg, scroll->scroll_rect.scope, scroll->scroll_rect.width);
        widget_set_size(scroll->scroll_fg, fg_range, scroll->scroll_rect.width);


        widget_set_pos(scroll->scroll_bg, scroll->scroll_rect.bg_xy.x, scroll->scroll_rect.bg_xy.y);

        scroll->scroll_rect.fg_xy.y = scroll->scroll_rect.bg_xy.y;
        scroll->scroll_rect.fg_xy.x = scroll->scroll_rect.bg_xy.x;
        scroll->scroll_rect.fg_xy.x = scroll->scroll_rect.fg_xy.x +
                                    (abs_s(scroll->scroll_rect.scope) - fg_range) * value / SCROLLBAR_VALUE_MAX;
        widget_set_pos(scroll->scroll_fg, scroll->scroll_rect.fg_xy.x, scroll->scroll_rect.fg_xy.y);
        break;
    }

}

/**
 * @brief 设置滚动条的边角是否圆弧化
 * @param[in] scroll : 滚动条指针
 * @param[in] circle : 是否圆弧化
 */
void compo_scroll_set_edge_circle(compo_scroll_t* scroll, bool circle)
{
    if (NULL == scroll) {
        halt(HALT_GUI_COMPO_ARC_FAIL);
    }

    switch (scroll->stype) {
    case SCROLL_TYPE_ARC:
        widget_arc_set_edge_circle(scroll->scroll_bg, circle, circle);
        widget_arc_set_edge_circle(scroll->scroll_fg, circle, circle);
        break;

    case SCROLL_TYPE_VERTICAL:
        widget_rect_set_radius(scroll->scroll_bg, scroll->scroll_rect.width);
        widget_rect_set_radius(scroll->scroll_fg, scroll->scroll_rect.width);
        break;

    case SCROLL_TYPE_HORIZONTAL:
        widget_rect_set_radius(scroll->scroll_bg, scroll->scroll_rect.width);
        widget_rect_set_radius(scroll->scroll_fg, scroll->scroll_rect.width);
        break;

    default:
        break;
    }
}
