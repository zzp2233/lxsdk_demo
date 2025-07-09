#ifndef _COMPO_BUTTON_H
#define _COMPO_BUTTON_H

typedef struct compo_button_t_
{
    COMPO_STRUCT_COMMON;
    widget_t *widget;
} compo_button_t;


/**
 * @brief 创建一个按钮
 * @param[in] frm : 窗体指针
 * @return 返回按钮指针
 **/
compo_button_t *compo_button_create(compo_form_t *frm);

/**
 * @brief 根据图像创建一个按钮
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图像资源的地址
 * @return 返回按钮指针
 **/
compo_button_t *compo_button_create_by_image(compo_form_t *frm, u32 res_addr);

/**
 * @brief 设置按钮坐标及大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] btn : 按钮指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 按钮宽度
 * @param[in] height : 按钮高度
 **/
void compo_button_set_location(compo_button_t *btn, s16 x, s16 y, s16 width, s16 height);

/**
 * @brief 设置按钮坐标
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] btn : 按钮指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_button_set_pos(compo_button_t *btn, s16 x, s16 y);

/**
 * @brief 设置按钮透明度
 * @param[in] btn : 按钮指针
 * @param[in] alpha : 透明度
 **/
void compo_button_set_alpha(compo_button_t *btn, u8 alpha);

/**
 * @brief 设置按键是否可见
 * @param[in] btn : 按钮指针
 * @param[in] visible : true  可见
                        false 不可见
 **/
void compo_button_set_visible(compo_button_t *btn, bool visible);

/**
 * @brief 设置按键是否可见
 * @param[in] btn : 按钮指针
 * @param[in] addr : 图像资源的地址
 **/
void compo_button_set_bgimg(compo_button_t *btn, u32 addr);

/**
 * @brief 根据图像在页面上创建一个按钮
 * @param[in] frm : 窗体指针
 * @param[in] widget : 页面
 * @param[in] res_addr : 图像资源的地址
 * @return 返回按钮指针
 **/
compo_button_t *compo_button_create_page_by_image(compo_form_t *frm,widget_page_t *widget, u32 res_addr);

#endif
