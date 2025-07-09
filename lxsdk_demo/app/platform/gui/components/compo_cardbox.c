#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define MAX_WORD_CNT                            50                          //每条列表项最多16个字符

/**
 * @brief 创建一个卡片控件
 * @param[in] frm : 窗体指针
 * @param[in] rect_num : 卡片包含的矩形背景数量
 * @param[in] icon_num : 卡片包含的图片数量
 * @param[in] text_num : 卡片包含的文本数量
 * @param[in] org_wid, org_hei : 卡片原始宽高
 * @return 返回卡片指针
 **/
compo_cardbox_t *compo_cardbox_create(compo_form_t *frm, u8 rect_num, u8 icon_num, u8 text_num, u16 org_wid, u16 org_hei)
{
    int i;
    compo_cardbox_t *cardbox = compo_create(frm, COMPO_TYPE_CARDBOX);
    widget_page_t *page = widget_page_create(frm->page_body);
    widget_set_size(page, org_wid, org_hei);
    widget_page_set_client(page, org_wid / 2, org_hei / 2); //卡片内控件参考坐标：卡片中心
    cardbox->page = page;
    rect_num = MIN(rect_num, CARD_RECT_MAX);
    for (i=0; i<rect_num; i++)
    {
        cardbox->rect[i] = widget_rect_create(page);
    }
    icon_num = MIN(icon_num, CARD_ICON_MAX);
    for (i=0; i<icon_num; i++)
    {
        cardbox->icon[i] = widget_image_create(page, 0);
    }
    text_num = MIN(text_num, CARD_TEXT_MAX);
    for (i=0; i<text_num; i++)
    {
        cardbox->text[i] = widget_text_create(page, MAX_WORD_CNT);
        widget_text_set_autosize(cardbox->text[i], false);
    }

    return cardbox;
}

/**
 * @brief 设置卡片（圆角）矩形背景颜色
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 矩形指针存储序号
 * @param[in] color : 色值
 **/
void compo_cardbox_rect_set_color(compo_cardbox_t *cardbox, u8 idx, u16 color)
{
    if (cardbox->rect[idx] && idx < CARD_RECT_MAX)
    {
        widget_rect_set_color(cardbox->rect[idx], color);
    }
}

/**
 * @brief 设置卡片（圆角）矩形背景尺寸
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 矩形指针存储序号
 * @param[in] x,y,wid,hei,r : 位置、宽、高、圆角半径
 **/
void compo_cardbox_rect_set_location(compo_cardbox_t *cardbox, u8 idx, s16 x, s16 y, u16 wid, u16 hei, u16 r)
{
    if (cardbox->rect[idx] && idx < CARD_RECT_MAX)
    {
        widget_set_location(cardbox->rect[idx], x, y, wid, hei);
        widget_rect_set_radius(cardbox->rect[idx], r);
    }
}


/**
 * @brief 设置卡片控件的图片裁剪
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 图片指针存储序号
 * @param[in] index : 裁剪序号
 * @param[in] total_cnt : 分割总数
 **/
void compo_cardbox_icon_cut(compo_cardbox_t *cardbox, u8 idx, u8 index, u8 total_cnt)
{
    widget_image_t *img = cardbox->icon[idx] ;
    if (img && idx < CARD_ICON_MAX && total_cnt && index < total_cnt)
    {
        rect_t rect = widget_get_location(img);
        area_t area = widget_image_get_size(img);
        widget_image_cut(img, 0, index * (area.hei / total_cnt), area.wid, area.hei / total_cnt);
        widget_set_size(img, rect.wid, rect.hei);
    }
}

/**
 * @brief 设置卡片控件的图片
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 图片指针存储序号
 * @param[in] x,y,wid,hei : 位置和大小
 **/
void compo_cardbox_icon_set_location(compo_cardbox_t *cardbox, u8 idx, s16 x, s16 y, u16 wid, u16 hei)
{
    if (cardbox->icon[idx] && idx < CARD_ICON_MAX)
    {
        widget_set_location(cardbox->icon[idx], x, y, wid, hei);
    }
}

/**
 * @brief 设置卡片控件的图片
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 图片指针存储序号
 * @param[in] x,y,wid,hei : 位置和大小
 **/
void compo_cardbox_icon_set_pos(compo_cardbox_t *cardbox, u8 idx, s16 x, s16 y)
{
    if (cardbox->icon[idx] && idx < CARD_ICON_MAX)
    {
        widget_set_pos(cardbox->icon[idx], x, y);
    }
}

/**
 * @brief 更改卡片控件的图片
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 图片指针存储序号
 * @param[in] res_addr : 资源地址
 **/
void compo_cardbox_icon_set(compo_cardbox_t *cardbox, u8 idx, u32 res_addr)
{
    if (cardbox->icon[idx] && idx < CARD_ICON_MAX)
    {
        widget_image_set(cardbox->icon[idx], res_addr);
    }
}

/**
 * @brief 设置卡片控件的文本
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 文本指针存储序号
 * @param[in] x,y,wid,hei : 位置和大小（默认非autosize）
 **/
void compo_cardbox_text_set_location(compo_cardbox_t *cardbox, u8 idx, s16 x, s16 y, u16 wid, u16 hei)
{
    if (cardbox->text[idx] && idx < CARD_TEXT_MAX)
    {
        widget_set_location(cardbox->text[idx], x, y, wid, hei);
    }
}

/**
 * @brief 设置卡片控件的文本前景色
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 文本指针存储序号
 * @param[in] color : 色值
 **/
void compo_cardbox_text_set_forecolor(compo_cardbox_t *cardbox, u8 idx, u16 color)
{
    if (cardbox->text[idx] && idx < CARD_TEXT_MAX)
    {
        widget_text_set_color(cardbox->text[idx], color);
    }
}

/**
 * @brief 更新卡片控件的文本内容
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 文本指针存储序号
 * @param[in] str : 文本字符串指针
 **/
void compo_cardbox_text_set(compo_cardbox_t *cardbox, u8 idx, const char *str)
{
    if (cardbox->text[idx] && idx < CARD_TEXT_MAX)
    {
        widget_text_set(cardbox->text[idx], str);
    }
}

/**
 * @brief 设置卡片控件的文本是否中心点对齐
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 文本指针存储序号
 * @param[in] str : 文本字符串指针
 **/
void compo_cardbox_text_set_align_center(compo_cardbox_t *cardbox, u8 idx, bool align_center)
{
    if (cardbox->text[idx] && idx < CARD_TEXT_MAX)
    {
        widget_set_align_center(cardbox->text[idx], align_center);
    }
}

/**
 * @brief 更换卡片控件的文本字库
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 文本指针存储序号
 * @param[in] font_res_addr : 设置字库资源地址
 **/
void compo_cardbox_text_set_font(compo_cardbox_t *cardbox, u8 idx, u32 font_res_addr)
{
    if (cardbox->text[idx] && idx < CARD_TEXT_MAX)
    {
        widget_text_set_font(cardbox->text[idx], font_res_addr);
    }
}

/**
 * @brief 设置卡片内文本框是否自适应尺寸
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 文本指针存储序号
 * @param[in] autosize : 是否自适应尺寸
 **/
void compo_cardbox_text_set_autosize(compo_cardbox_t *cardbox, u8 idx, bool autosize)
{
    if (cardbox->text[idx] && idx < CARD_TEXT_MAX)
    {
        widget_text_set_autosize(cardbox->text[idx], autosize);
    }
}

/**
 * @brief 获取卡片内文本字体区域
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 文本指针存储序号
 * @return 文本内容的宽度和高度
 **/
area_t compo_cardbox_text_get_area(compo_cardbox_t *cardbox, u8 idx)
{
    area_t area = {0};
    if (cardbox->text[idx] && idx < CARD_TEXT_MAX)
    {
        area = widget_text_get_area(cardbox->text[idx]);
    }
    return area;
}

/**
 * @brief 设置卡片控件的位置和缩放大小
 * @param[in] cardbox : 卡片指针
 * @param[in] x,y,wid,hei : 位置和缩放大小
 **/
void compo_cardbox_set_location(compo_cardbox_t *cardbox, s16 x, s16 y, u16 wid, u16 hei)
{
    widget_page_scale_to(cardbox->page, wid, hei);
    widget_set_pos(cardbox->page, x, y);
}

/**
 * @brief 设置卡片控件的位置
 * @param[in] cardbox : 卡片指针
 * @param[in] x,y : 位置
 **/
void compo_cardbox_set_pos(compo_cardbox_t *cardbox, s16 x, s16 y)
{
    widget_set_pos(cardbox->page, x, y);
}

/**
 * @brief 获取卡片控件的位置和缩放大小
 * @param[in] cardbox : 卡片指针
 * @param[in] x,y,wid,hei : 位置和缩放大小
 **/
rect_t compo_cardbox_get_location(compo_cardbox_t *cardbox)
{
    rect_t rect = {0};
    rect = widget_get_location(cardbox->page);

    return rect;
}

/**
 * @brief 获取卡片绝对位置和大小
 * @param[in] cardbox : 卡片指针
 * @return 返回卡片page绝对位置和大小
 **/
rect_t compo_cardbox_get_absolute(compo_cardbox_t *cardbox)
{
    return widget_get_absolute(cardbox->page);
}

/**
 * @brief 获取卡片内矩形的绝对位置和大小
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 矩形指针存储序号
 * @return 返回相应序号图片的绝对位置和大小
 **/
rect_t compo_cardbox_get_rect_absolute(compo_cardbox_t *cardbox, u8 idx)
{
    rect_t rect = {0};
    if (cardbox->rect[idx] && idx < CARD_RECT_MAX)
    {
        rect = widget_get_absolute(cardbox->rect[idx]);
    }
    return rect;
}

/**
 * @brief 获取卡片内图片的绝对位置和大小
 * @param[in] cardbox : 卡片指针
 * @param[in] idx : 图片指针存储序号
 * @return 返回相应序号图片的绝对位置和大小
 **/
rect_t compo_cardbox_get_icon_absolute(compo_cardbox_t *cardbox, u8 idx)
{
    rect_t rect = {0};
    if (cardbox->icon[idx] && idx < CARD_ICON_MAX)
    {
        rect = widget_get_absolute(cardbox->icon[idx]);
    }
    return rect;
}

/**
 * @brief 设置卡片中图片可见性
 * @param[in] cardbox : 卡片指针
 **/
void compo_cardbox_icon_set_visible(compo_cardbox_t *cardbox, u8 idx, bool visible)
{
    if (cardbox->icon[idx] && idx < CARD_ICON_MAX)
    {
        widget_set_visible(cardbox->icon[idx], visible);
    }
}

/**
 * @brief 设置卡片中文字可见性
 * @param[in] cardbox : 卡片指针
 **/
void compo_cardbox_text_set_visible(compo_cardbox_t *cardbox, u8 idx, bool visible)
{
    if (cardbox->text[idx] && idx < CARD_TEXT_MAX)
    {
        widget_set_visible(cardbox->text[idx], visible);
    }
}

/**
 * @brief 设置卡片可见性
 * @param[in] cardbox : 卡片指针
 **/
void compo_cardbox_set_visible(compo_cardbox_t *cardbox, bool visible)
{
    widget_set_visible(cardbox->page, visible);
}

/**
 * @brief 获取卡片可见性
 * @param[in] cardbox : 卡片指针
 **/
bool compo_cardbox_get_visible(compo_cardbox_t *cardbox)
{
    return widget_get_visble(cardbox->page);
}


/**
 * @brief 设置相对坐标
 * @param[in] cardbox : 卡片指针
 **/
void compo_cardbox_set_client(compo_cardbox_t *cardbox, s16 x, s16 y)
{
    widget_page_set_client(cardbox->page, x, y);
}


/**
 * @brief 判断点击区域是否在卡片内
 * @param[in] cardbox : 卡片指针
 * @param[in] pt : 点击区域
 * @return true or false
 **/
bool compo_cardbox_btn_is(compo_cardbox_t *cardbox, point_t pt)
{
    rect_t rect = compo_cardbox_get_absolute(cardbox);

    if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
    {
        return true;
    }

    return false;
}


/**
 * @brief 设置透明度
 * @param[in] cardbox : 卡片指针
* @param[in] alpha : 透明度(0 ~ 0xff)
 **/
void compo_cardbox_set_alpha(compo_cardbox_t *cardbox, u8 alpha)
{
    widget_set_alpha(cardbox->page, alpha);
}

/**
 * @brief 卡片控件文本滚动刷新
        注意：1.文本需要set_align_center为false，否则初始状态无法保证左对齐；
        2.界面文本控件较多时可能出现C203蓝屏，需减小MAX_WORD_CNT或增大GUI_ELE_BUF_SIZE
 * @param[in] cardbox : 卡片指针
 * @return 无
 **/
void compo_cardbox_text_scroll_process(compo_cardbox_t* cardbox, bool auto_scroll)
{
    for (int i=0; i<CARD_TEXT_MAX; i++)
    {
        widget_text_t* txt = cardbox->text[i];

//        printf("[%d] txt[%x] ", i, txt);
        if (txt)
        {

            rect_t rect_card = widget_get_absolute(cardbox->page);
            if (auto_scroll)
            {
//                printf("reload[%d] ", rect_card.y < (-rect_card.hei / 2) || rect_card.y > (GUI_SCREEN_HEIGHT + rect_card.hei / 2));
                if (rect_card.y < (-rect_card.hei / 2) || rect_card.y > (GUI_SCREEN_HEIGHT + rect_card.hei / 2))    //卡片超出屏幕，滚动重置
                {
                    cardbox->roll_cb[i].mode = TEXT_AUTOROLL_MODE_NULL;
                    widget_text_set_client(txt, 0, 0);
                    break;
                }
            }
//            printf("mode[%d] ", cardbox->roll_cb[i].mode);
            if (cardbox->roll_cb[i].mode == TEXT_AUTOROLL_MODE_NULL)    //重置
            {
//                printf("visble[%d] ", widget_get_visble(txt));
                if (widget_get_visble(txt))
                {
                    area_t text_area = widget_text_get_area(txt);
                    rect_t textbox_rect = widget_get_location(txt);
//                    printf("area[%d,%d] ", text_area.wid, textbox_rect.wid);
                    if (text_area.wid > textbox_rect.wid)
                    {
                        memset(&cardbox->roll_cb[i], 0, sizeof(compo_roll_cb_t));
                        cardbox->roll_cb[i].tick = tick_get();
                        cardbox->roll_cb[i].mode = TEXT_AUTOROLL_MODE_SROLL_CIRC;
                        cardbox->roll_cb[i].direction = -1;
                        if (widget_get_align_center(txt))
                        {
                            cardbox->roll_cb[i].offset = text_area.wid/2 - textbox_rect.wid/2;
                            widget_text_set_client(txt, cardbox->roll_cb[i].offset, 0);
                        }
                        widget_text_set_autoroll_mode(txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
                    }
                    else
                    {
                        widget_text_set_autoroll_mode(txt, TEXT_AUTOROLL_MODE_NULL);
                    }
                }
            }
        }
//        printf("\n");
    }
//    printf("\n");

}

/**
 * @brief 卡片控件文本0滚动暂停/继续
 * @param[in] cardbox : 卡片指针
 * @param[in] pause : true暂停，false继续
 * @return 无
 **/
void compo_cardbox_text_scroll_pause(compo_cardbox_t *cardbox, bool pause)
{
    for (int i=0; i<CARD_TEXT_MAX; i++)
    {
        widget_text_t* txt = cardbox->text[i];
        if (txt)
        {
            cardbox->roll_cb[i].direction = pause ? 0 : -1;
        }
    }
}

/**
 * @brief 卡片控件文本滚动重置（赋值后需要重新计算）
 * @param[in] cardbox : 卡片指针
 * @return 无
 **/
void compo_cardbox_text_scroll_reset(compo_cardbox_t *cardbox)
{
    for (int i=0; i<CARD_TEXT_MAX; i++)
    {
        widget_text_t* txt = cardbox->text[i];
        if (txt)
        {
            cardbox->roll_cb[i].mode = TEXT_AUTOROLL_MODE_NULL;
            widget_text_set_client(txt, 0, 0);
        }
    }
}


/**
 * @brief 卡片控件文本中心坐标映射到左上角
 * @param[in] cardbox : 卡片指针
              idx : 卡片内容id
              cx : 中心对其时的x坐标
              cy : 中心对其时的y坐标
              cwid : 文本框宽度
              chei : 文本框高度
 * @return 无
 **/
void compo_cardbox_text_map_center2left_location(compo_cardbox_t *cardbox, u8 idx, s16 cx, s16 cy, u16 cwid, u16 chei)
{
    compo_cardbox_text_set_align_center(cardbox, idx, false);
    compo_cardbox_text_set_location(cardbox, idx, cx-cwid/2, cy-chei/2, cwid, chei);
}


