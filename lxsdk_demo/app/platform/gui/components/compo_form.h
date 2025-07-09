#ifndef _COMPO_FORM_H
#define _COMPO_FORM_H

#define COMPO_FORM_MODE_SHOW_TITLE              BIT(0)          //标题栏显示文字
#define COMPO_FORM_MODE_SHOW_TIME               BIT(1)          //标题栏显示时间
#define COMPO_FORM_MODE_SHOW_ICON               BIT(2)          //标题栏显示自定义状态图标

#include "compo_animation.h"

typedef struct compo_textbox_t_
{
    COMPO_STRUCT_COMMON;
    widget_text_t *txt;
    compo_roll_cb_t roll_cb;
    bool multiline;             //多行
    bool center_top;            //居顶对齐
} compo_textbox_t;


typedef struct compo_form_t_
{
    COMPO_STRUCT_COMMON;
    widget_page_t *page;
    widget_page_t *page_body;
    widget_icon_t *icon;
    widget_icon_t *title_icon;
    compo_textbox_t *title;
//    widget_text_t *time;
    compo_textbox_t *time;
    int mode;
#if (ASR_SELECT && ASR_VOICE_BALL_ANIM)
    compo_animation_t *anim;
#endif
} compo_form_t;

/**
 * @brief 创建窗体
          窗体为其他组件的容器
 * @param[in] flag_top : 是否放在界面的顶层
                         false在底层
                         true在顶层
 * @return 返回窗体指针
 **/
compo_form_t *compo_form_create(bool flag_top);

/**
 * @brief 销毁窗体
 * @param[in] frm : 窗体指针
 **/
void compo_form_destroy(compo_form_t *frm);

/**
 * @brief 设置窗体坐标及大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 窗体宽度
 * @param[in] height : 窗体高度
 **/
void compo_form_set_location(compo_form_t *frm, s16 x, s16 y, s16 width, s16 height);

/**
 * @brief 设置窗体坐标
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_form_set_pos(compo_form_t *frm, s16 x, s16 y);

/**
 * @brief 窗体缩放
          以中心点缩放
 * @param[in] frm : 窗体指针
 * @param[in] width : 缩放后的窗体宽度
 * @param[in] height : 缩放后的窗体高度
 **/
void compo_form_scale_to(compo_form_t *frm, s16 width, s16 height);

/**
 * @brief 窗体设置Alpha
 * @param[in] frm : 窗体指针
 * @param[in] alpha : 透明度
 **/
void compo_form_set_alpha(compo_form_t *frm, u8 alpha);

/**
 * @brief 设置窗体标题栏
          通常和compo_form_set_title（窗体模式）一起调用
 * @param[in] frm : 窗体指针
 * @param[in] title : 标题文本
 **/
void compo_form_set_title(compo_form_t *frm, const char *title);

/**
 * @brief 设置窗体模式
          通常和compo_form_set_title（窗体标题栏）一起调用
 * @param[in] frm : 窗体指针
 * @param[in] mode : COMPO_FORM_MODE_SHOW_TITLE  BIT(0) 标题栏显示文字
                     COMPO_FORM_MODE_SHOW_TIME   BIT(1) 标题栏显示时间
 **/
void compo_form_set_mode(compo_form_t *frm, int mode);

/**
 * @brief 设置窗体标题居中
 * @param[in] frm : 窗体指针
 * @param[in] align_center : 是否居中
 **/
void compo_form_set_title_center(compo_form_t *frm, bool align_center);

/**
 * @brief 窗体中添加图片
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图片资源的地址
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_form_add_image(compo_form_t *frm, u32 res_addr, s16 x, s16 y);

/**
 * @brief 窗体中设置背景
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图片资源的地址
 **/
void compo_form_set_bg(compo_form_t *frm, u32 res_addr);

/**
 * @brief title的图标
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图片资源的地址
 **/
void compo_form_set_title_icon(compo_form_t *frm, u32 res_addr);

void compo_form_set_title_txt_color(compo_form_t *frm, u16 color);
/**
 * @brief 语音悬浮球
 * @param[in] sta: true:显示；flase：不显示
 */
void compo_form_set_ai_voice_anim(u8 sta);

#endif
