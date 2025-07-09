#ifndef __COMPO_BAR_H
#define __COMPO_BAR_H

enum
{
    COMPO_BAR_DIR_VERTICAL,        //垂直方向
    COMPO_BAR_DIR_HORIZONTAL,      //水平方向
};

typedef void (*value_change_cb_t)(int32_t cur_value);

typedef struct
{
    value_change_cb_t value_change_cb;
    uint32_t last_move;
    point_t touch_point;
    bool touch_flag;
    bool move_flag;
} __move_ctr_t;

typedef struct compo_bar_t_
{
    COMPO_STRUCT_COMMON;
    widget_bar_t *bar;         //控件主体
    int32_t start_value;       //前进方向起始值
    int32_t end_value;         //前进方向结束值
    int32_t cur_value;         //当前值
    u16 bg_color;              //背景色
    u16 content_color;         //内容色
    bool dir;                  //显示方向，纵向或横向
    bool forward_dir;          //前进方向，默认从上到下，从左到右
    u8 alpha1;                 //当前值alph
    u8 alpha2;                 //背景色alph
    __move_ctr_t move_ctr;     //滑动控制
} compo_bar_t;

/**
 * @brief 创建一个进度条组件
 * @param[in] frm : 窗体指针
 * @return 返回进度条句柄
 **/
compo_bar_t *compo_bar_create(compo_form_t *frm);

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
void compo_bar_set_location(compo_bar_t *bar, s16 x, s16 y, s16 width, s16 height);

/**
 * @brief 设置进度条范围
 * @param[in] bar : 进度条句柄
 * @param[in] start_value : 前进方向起始值
 * @param[in] end_value : 前进方向结束值
 * @return 无
 **/
void compo_bar_set_range(compo_bar_t *bar, int32_t start_value, int32_t end_value);

/**
 * @brief 设置进度条的值
 * @param[in] bar : 进度条句柄
 * @param[in] value : 设置进度条范围内的值
 * @return 无
 **/
void compo_bar_set_value(compo_bar_t *bar, int32_t value);

/**
 * @brief 设置进度条的显示方向
 * @param[in] bar : 进度条句柄
 * @param[in] min : 方向：COMPO_BAR_DIR_VERTICAL or COMPO_BAR_DIR_HORIZONTAL
 * @return 无
 **/
void compo_bar_set_dir(compo_bar_t *bar, bool dir);

/**
 * @brief 设置进度条的前进方向
 * @param[in] bar : 进度条句柄
 * @param[in] forward_dir : 前进方向：default:0
 *                          dir=COMPO_BAR_DIR_VERTICAL   0:从上到下  1:从下到上
 *                          dir=COMPO_BAR_DIR_HORIZONTAL 0:从左到右  1:从右到左
 * @return 无
 **/
void compo_bar_set_forward_dir(compo_bar_t *bar, bool forward_dir);

/**
 * @brief 设置进度条的颜色
 * @param[in] bar : 进度条句柄
 * @param[in] content_color : 设置值区域的颜色
 * @param[in] bg_color: 设置值外的颜色
 * @return 无
 **/
void compo_bar_set_color(compo_bar_t *bar, u16 content_color, u16 bg_color);
/**
 * @brief 设置进度条的alpha
 * @param[in] bar : 进度条句柄
 * @param[in] alpha1 : 设置值区域的alpha
 * @param[in] alpha2 : 设置值外的alpha
 * @return 无
 **/
void compo_bar_set_alpha(compo_bar_t *bar,u8 alpha1, u16 alpha2);
/**
 * @brief 设置进度条的圆角半径
 * @param[in] bar : 进度条句柄
 * @param[in] r : 圆角半径
                  当半大于高或宽的一半时，底层自动把半径设置为其中最小的；(高或宽的一半)
 * @return 无
 **/
void compo_bar_set_rabardius(compo_bar_t *bar, u16 r);

/**
 * @brief 设置进度条是否按下
 * @param[in] bar : 进度条句柄
 * @param[in] body_value : 是否使能触摸进度条本身才能滑动
 *
 * @return 是否有效
 **/
bool compo_bar_move_status(compo_bar_t *bar, bool body_touch);

/**
 * @brief 进度条滑动控制 (在func中轮询)
 * @param[in] bar : 进度条句柄
 * @return 无
 **/
void compo_bar_move_ctr(compo_bar_t *bar);

/**
 * @brief 设置进度条是否按下
 * @param[in] roller : 进度条句柄
 * @param[in] cb : 因滑动或点击导致值变化时调用
 * @return 无
 **/
void compo_bar_set_value_change_cb(compo_bar_t *bar, value_change_cb_t func);

#endif

