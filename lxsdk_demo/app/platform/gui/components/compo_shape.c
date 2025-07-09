#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

/**
 * @brief 创建一个形状
 * @param[in] frm : 窗体指针
 * @param[in] shape_type : COMPO_SHAPE_TYPE_RECTANGLE 矩形
 * @return 返回矩形指针
 **/
compo_shape_t *compo_shape_create(compo_form_t *frm, u8 shape_type)
{
    compo_shape_t *shape = compo_create(frm, COMPO_TYPE_SHAPE);
    void *rect = widget_rect_create(frm->page_body);
    shape->rect = rect;
    shape->shape_type = shape_type;
    return shape;
}

/**
 * @brief 设置形状的坐标及大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] shape : 矩形指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 矩形宽度
 * @param[in] height : 矩形高度
 **/
void compo_shape_set_location(compo_shape_t *shape, s16 x, s16 y, s16 width, s16 height)
{
    widget_set_location(shape->rect, x, y, width, height);
}

/**
 * @brief 获取形状的坐标及大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] shape : 矩形指针
 * @param[out] x : x轴坐标
 * @param[out] y : y轴坐标
 * @param[out] width : 矩形宽度
 * @param[out] height : 矩形高度
 **/
rect_t compo_shape_get_location(compo_shape_t *shape)
{
    rect_t rect = widget_get_location(shape->rect);
    return rect;
}

/**
 * @brief 设置形状的大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] shape : 矩形指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 矩形宽度
 * @param[in] height : 矩形高度
 **/
void compo_shape_set_size(compo_shape_t *shape, s16 width, s16 height)
{
    widget_set_size(shape->rect, width, height);
}

/**
 * @brief 设置形状的坐标
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] shape : 矩形指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_shape_set_pos(compo_shape_t *shape, s16 x, s16 y)
{
    widget_set_pos(shape->rect, x, y);
}

/**
 * @brief 设置形状的颜色
 * @param[in] shape : 矩形指针
 * @param[in] color : 颜色
 **/
void compo_shape_set_color(compo_shape_t *shape, u16 color)
{
    widget_rect_set_color(shape->rect, color);
}

/**
 * @brief 设置形状的透明度
 * @param[in] shape : 矩形指针
 * @param[in] alpha : 透明度
 **/
void compo_shape_set_alpha(compo_shape_t *shape, u8 alpha)
{
    widget_set_alpha(shape->rect, alpha);
}

/**
 * @brief 设置形状是否可见
 * @param[in] shape : 矩形指针
 * @param[in] visible : true  可见
                        false 不可见
 **/
void compo_shape_set_visible(compo_shape_t *shape, bool visible)
{
    widget_set_visible(shape->rect, visible);
}

/**
 * @brief 获取矩形可见性
 * @param[in] shape : 矩形指针
 * @return 可见性
 **/
bool compo_shape_get_visible(compo_shape_t *shape)
{
    return widget_get_visble(shape->rect);
}


/**
 * @brief 设置形状（矩形）圆角半径
 * @param[in] shape : 矩形指针
 * @param[in] r : 半径
 **/
void compo_shape_set_radius(compo_shape_t *shape, u16 r)
{
    if (shape->shape_type == COMPO_SHAPE_TYPE_RECTANGLE)
    {
        widget_rect_set_radius(shape->rect, r);
    }
}
/**
 * @brief 创建一个形状
 * @param[in] frm : 窗体指针
 * @param[in] shape_type : COMPO_SHAPE_TYPE_RECTANGLE 矩形
 * @param[in] page : 页面指针
 * @return 返回矩形指针
 **/
compo_shape_t *compo_shape_create_for_page(compo_form_t *frm, void* page, u8 shape_type)
{
    compo_shape_t *shape = compo_create(frm, COMPO_TYPE_SHAPE);
    void *rect = widget_rect_create(page);
    shape->rect = rect;
    shape->shape_type = shape_type;
    return shape;
}
