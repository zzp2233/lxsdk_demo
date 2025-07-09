#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define MAX_WORD_CNT                    32                          //标题最多32个字符

#define GUI_PAGE_HEAD_HEIGHT            (GUI_SCREEN_HEIGHT / 8)
#define GUI_PAGE_BODY_HEIGHT            (GUI_SCREEN_HEIGHT - GUI_PAGE_HEAD_HEIGHT)
#define GUI_PAGE_BODY_CENTER_Y          (GUI_PAGE_HEAD_HEIGHT + GUI_PAGE_BODY_HEIGHT / 2)

#define GUI_PAGE_TITLE_WIDTH            (GUI_SCREEN_WIDTH -50)
#define GUI_PAGE_TIME_WIDTH             (GUI_SCREEN_WIDTH - FORM_TITLE_RIGHT - GUI_SCREEN_WIDTH/12)//(GUI_SCREEN_WIDTH / 3)

/**
 * @brief 创建窗体
          窗体为其他组件的容器
 * @param[in] flag_top : 是否放在界面的顶层
                         false在底层
                         true在顶层
 * @return 返回窗体指针
 **/
compo_form_t *compo_form_create(bool flag_top)
{
    compo_form_t *frm = compo_pool_create(flag_top);
    widget_page_t *page = widget_pool_create(flag_top);

    widget_icon_t *icon = widget_icon_create(page, 0);
    widget_set_pos(icon, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    widget_set_size(icon, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);

    widget_page_t *page_body = widget_page_create(page);
//    widget_text_t *time = widget_text_create(page, 5);

    widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    widget_set_location(page_body, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);

//    widget_set_align_center(time, false);
//    widget_set_location(time, FORM_TITLE_RIGHT, GUI_PAGE_HEAD_HEIGHT - FORM_TITLE_HEIGHT, GUI_PAGE_TIME_WIDTH, FORM_TITLE_HEIGHT);
//    widget_text_set_font(time, UI_BUF_FONT_FORM_TIME);
//    widget_set_visible(time, false);


    frm->type = COMPO_TYPE_FORM;
    frm->page = page;
    frm->icon = icon;

    frm->page_body = page;

    compo_textbox_t *time = compo_textbox_create(frm, MAX_WORD_CNT);
    compo_textbox_set_align_center(time, false);
    compo_textbox_set_location(time, FORM_TITLE_RIGHT, GUI_PAGE_HEAD_HEIGHT - FORM_TITLE_HEIGHT, GUI_PAGE_TIME_WIDTH, FORM_TITLE_HEIGHT);
    compo_textbox_set_multiline(time, false);
    compo_textbox_set_autosize(time, false);
    compo_textbox_set_visible(time, false);
    frm->time = time;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    compo_textbox_t *title = compo_textbox_create(frm, MAX_WORD_CNT);
    frm->page_body = page_body;
    compo_textbox_set_font(title, UI_BUF_FONT_FORM_TIME);
    compo_textbox_set_align_center(title, false);
    compo_textbox_set_location(title, FORM_TITLE_LEFT, GUI_PAGE_HEAD_HEIGHT - FORM_TITLE_HEIGHT, GUI_PAGE_TITLE_WIDTH, FORM_TITLE_HEIGHT);
    compo_textbox_set_multiline(title, false);
    compo_textbox_set_autosize(title, false);
    compo_textbox_set_align_center(title, false);
    compo_textbox_set_autoroll_mode(title, TEXT_AUTOROLL_MODE_NULL);
    compo_textbox_set_visible(title, true);
    frm->title = title;
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    compo_textbox_t *title = compo_textbox_create(frm, MAX_WORD_CNT);
    frm->page_body = page_body;
    compo_textbox_set_font(title, UI_BUF_FONT_FORM_TIME);
    compo_textbox_set_location(title, GUI_SCREEN_CENTER_X, FORM_TITLE_HEIGHT-widget_text_get_max_height(), 120, 34);
    compo_textbox_set_autoroll_mode(title, TEXT_AUTOROLL_MODE_NULL);
    frm->title = title;
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

    widget_icon_t *title_icon = widget_icon_create(page, 0);
    widget_set_pos(title_icon, FORM_TITLE_LEFT + 10, GUI_PAGE_HEAD_HEIGHT - FORM_TITLE_HEIGHT / 2 - 2);
    widget_set_size(title_icon, FORM_TITLE_HEIGHT, FORM_TITLE_HEIGHT);
    widget_set_visible(title_icon, false);
    frm->title_icon = title_icon;

#if (ASR_SELECT && ASR_VOICE_BALL_ANIM)
    //语音悬浮球
    frm->page_body = page;
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_ASR_VOICE_BIN);
    frm->page_body = page_body;
    compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, (GUI_SCREEN_HEIGHT - (GUI_SCREEN_HEIGHT >> 2)));
    compo_animation_set_radix(animation, 5);
    compo_animation_set_interval(animation, 10);
    widget_set_top(animation->page, true);
#if (ASR_SELECT == ASR_WS_AIR || ASR_SELECT == ASR_WS)
    compo_animation_set_visible(animation, bsp_asr_voice_wake_sta_get());
#else
    compo_animation_set_visible(animation, false);
#endif
    frm->anim = animation;
#endif

    return frm;
}

/**
 * @brief 销毁窗体
 * @param[in] frm : 窗体指针
 **/
void compo_form_destroy(compo_form_t *frm)
{
    if (frm == NULL)
    {
        return;
    }
    widget_pool_clear(frm->page);
    compo_pool_clear(frm);
}

/**
 * @brief 设置窗体坐标及大小
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 窗体宽度
 * @param[in] height : 窗体高度
 **/
void compo_form_set_location(compo_form_t *frm, s16 x, s16 y, s16 width, s16 height)
{
    if (frm == NULL || frm->page == NULL)
    {
        halt(HALT_GUI_COMPO_FORM_PTR);
    }
    widget_set_location(frm->page, x, y, width, height);
}


/**
 * @brief 设置窗体坐标
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_form_set_pos(compo_form_t *frm, s16 x, s16 y)
{
    if (frm == NULL || frm->page == NULL)
    {
        halt(HALT_GUI_COMPO_FORM_PTR);
    }
    widget_set_pos(frm->page, x, y);
}

/**
 * @brief 窗体缩放
          以中心点缩放
 * @param[in] frm : 窗体指针
 * @param[in] width : 缩放后的窗体宽度
 * @param[in] height : 缩放后的窗体高度
 **/
void compo_form_scale_to(compo_form_t *frm, s16 width, s16 height)
{
    if (frm == NULL || frm->page == NULL)
    {
        halt(HALT_GUI_COMPO_FORM_PTR);
    }
    widget_page_scale_to(frm->page, width, height);
}

/**
 * @brief 窗体设置Alpha
 * @param[in] frm : 窗体指针
 * @param[in] alpha : 透明度
 **/
void compo_form_set_alpha(compo_form_t *frm, u8 alpha)
{
    if (frm == NULL || frm->page == NULL)
    {
        halt(HALT_GUI_COMPO_FORM_PTR);
    }
    widget_set_alpha(frm->page, alpha);
}

/**
 * @brief 更新窗体信息
 * @param[in] frm : 窗体指针
 **/
static void compo_form_page_update(compo_form_t *frm)
{
    if (frm->mode == 0)
    {
        compo_textbox_set_visible(frm->title, false);
        widget_set_location(frm->page_body, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
        widget_page_set_client(frm->page_body, 0, 0);
    }
    else
    {
        compo_textbox_set_visible(frm->title, (frm->mode & COMPO_FORM_MODE_SHOW_TITLE) != 0);
//        widget_set_visible(frm->time, (frm->mode & COMPO_FORM_MODE_SHOW_TIME) != 0);
//        compo_textbox_set_visible(frm->time, (frm->mode & COMPO_FORM_MODE_SHOW_TIME) != 0);
        widget_set_visible(frm->title_icon, (frm->mode & COMPO_FORM_MODE_SHOW_ICON) != 0);
//        if (frm->mode & COMPO_FORM_MODE_SHOW_TIME) {
//            widget_set_visible(frm->time, true);
//            compo_cb.rtc_update = true;
//        }
        widget_set_location(frm->page_body, GUI_SCREEN_CENTER_X, GUI_PAGE_BODY_CENTER_Y, GUI_SCREEN_WIDTH, GUI_PAGE_BODY_HEIGHT);
        widget_page_set_client(frm->page_body, 0, -GUI_PAGE_HEAD_HEIGHT);
    }
}

/**
 * @brief 设置窗体标题栏
          通常和compo_form_set_title（窗体模式）一起调用
 * @param[in] frm : 窗体指针
 * @param[in] title : 标题文本
 **/
void compo_form_set_title(compo_form_t *frm, const char *title)
{
    if (title != NULL)
    {
        frm->mode |= COMPO_FORM_MODE_SHOW_TITLE;
        compo_textbox_set(frm->title, title);

        compo_textbox_t *compo_textbox = frm->title;

        area_t text_area = widget_text_get_area(compo_textbox->txt);
        rect_t textbox_rect = widget_get_location(compo_textbox->txt);
        if (text_area.wid > textbox_rect.wid)
        {
            compo_textbox->roll_cb.mode = TEXT_AUTOROLL_MODE_SROLL_CIRC;
            compo_textbox->roll_cb.direction = -1;
        }

        compo_form_page_update(frm);
    }
}

/**
 * @brief 设置窗体模式
          通常和compo_form_set_title（窗体标题栏）一起调用
 * @param[in] frm : 窗体指针
 * @param[in] mode : COMPO_FORM_MODE_SHOW_TITLE  BIT(0) 标题栏显示文字
                     COMPO_FORM_MODE_SHOW_TIME   BIT(1) 标题栏显示时间
 **/
void compo_form_set_mode(compo_form_t *frm, int mode)
{
    frm->mode = mode;
    compo_form_page_update(frm);
}

/**
 * @brief 设置窗体标题居中
 * @param[in] frm : 窗体指针
 * @param[in] align_center : 是否居中
 **/
void compo_form_set_title_center(compo_form_t *frm, bool align_center)
{
    compo_textbox_set_align_center(frm->title, align_center);
    compo_textbox_set_location(frm->title, GUI_SCREEN_CENTER_X, GUI_PAGE_HEAD_HEIGHT - (FORM_TITLE_HEIGHT >> 1), GUI_PAGE_TITLE_WIDTH, FORM_TITLE_HEIGHT);
}

/**
 * @brief 窗体中添加图片
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图片资源的地址
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_form_add_image(compo_form_t *frm, u32 res_addr, s16 x, s16 y)
{
    //背景图不需要复杂功能，使用widget_icon更省Buffer
    widget_icon_t *icon = widget_icon_create(frm->page_body, res_addr);
    widget_set_pos(icon, x, y);
}

/**
 * @brief 窗体中设置背景
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图片资源的地址
 **/
void compo_form_set_bg(compo_form_t *frm, u32 res_addr)
{
    widget_icon_set(frm->icon, res_addr);
}

/**
 * @brief title的图标
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图片资源的地址
 **/
void compo_form_set_title_icon(compo_form_t *frm, u32 res_addr)
{
    if (widget_text_get_area(frm->title->txt).wid)
    {
        widget_set_pos(frm->title_icon, FORM_TITLE_LEFT + widget_text_get_area(frm->title->txt).wid +
                       gui_image_get_size(res_addr).wid / 2 + 5, GUI_PAGE_HEAD_HEIGHT - FORM_TITLE_HEIGHT / 2 - 2);
        widget_set_size(frm->title_icon, FORM_TITLE_HEIGHT, FORM_TITLE_HEIGHT);
    }

    widget_icon_set(frm->title_icon, res_addr);
    compo_form_page_update(frm);
}

void compo_form_set_title_txt_color(compo_form_t *frm, u16 color)
{
    if (frm->title != NULL)
    {
        if (widget_get_visble(frm->title->txt))
        {
            widget_text_set_color(frm->title->txt, color);
        }
    }
}
/**
 * @brief 语音悬浮球
 * @param[in] sta: true:显示；flase：不显示
 */
#if (ASR_SELECT && ASR_VOICE_BALL_ANIM)
void compo_form_set_ai_voice_anim(u8 sta)
{
    if (func_cb.sta == FUNC_CLOCK)
    {
        f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
        if (f_clk->sub_frm && f_clk->sta == FUNC_CLOCK_SUB_DROPDOWN)
        {
            if (f_clk->sub_frm->anim)
            {
                compo_animation_set_visible(f_clk->sub_frm->anim, sta);
                if (func_cb.frm_main)
                {
                    if (func_cb.frm_main->anim)
                    {
                        compo_animation_set_visible(func_cb.frm_main->anim, sta);
                    }
                }
            }
        }
    }

    if (func_cb.frm_main)
    {
        if (func_cb.frm_main->anim)
        {
            compo_animation_set_visible(func_cb.frm_main->anim, sta);
        }
    }
}
#endif

