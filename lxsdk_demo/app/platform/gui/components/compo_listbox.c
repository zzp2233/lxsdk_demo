#include "include.h"
#include "ute_module_call.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define LISTBOX_ITEM_SIZE_THRESHOLD             (GUI_SCREEN_HEIGHT / 4)     //菜单大小保持区
#define LISTBOX_ITEM_SIZE_THRESHOLD_CIRCLE      (GUI_SCREEN_HEIGHT / 6)     //弧线菜单大小保持区
#define LISTBOX_STYLE_CIRCLE_R                  (GUI_SCREEN_WIDTH * 2)      //更表弧线半径

#define LISTBOX_MAX_ITEM_CNT                    UTE_MODULE_CALL_ADDRESSBOOK_MAX_COUNT
#define MIN_WORD_CNT                            32
#define MAX_WORD_CNT                            MAX(MAX(MAX(MAX(MIN_WORD_CNT, UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH), UTE_MODULE_CALL_ADDRESSBOOK_NUMBER_MAX_LENGTH), UTE_CALL_DIAL_NUMBERS_MAX), UTE_CALL_NAME_MAX)                          //每条列表项最多32个字符
#define LIST_CUSTOM_AREA_X_MIN                  90                          //点击列表中X坐标的最小差值
#define LIST_CUSTOM_AREA_X_MAX                  140                         //点击列表中X坐标的最大差值
#define LIST_CUSTOM_AREA_Y_MIN                 -110                         //点击列表中Y坐标的最小差值
#define LIST_CUSTOM_AREA_Y_MAX                 -60                          //点击列表中Y坐标的最大差值

//移动相关控制
#define FOCUS_AUTO_TICK_EXPIRE              10                  //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                     5                   //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV                 16

#define DRAG_AUTO_SPEED                     80                  //拖动松手后的速度

/**
 * @brief 创建一个列表控件
 * @param[in] frm : 窗体指针
 * @param[in] style : COMPO_LISTBOX_STYLE_MENU_NORMAL, 普通菜单
                      COMPO_LISTBOX_STYLE_MENU_CIRCLE, 圆屏弧形菜单
                      COMPO_LISTBOX_STYLE_MENU_FOURGRID, 横向四宫格菜单
 * @return 返回列表指针
 **/
compo_listbox_t *compo_listbox_create(compo_form_t *frm, u32 style)
{
    int i;
    compo_listbox_t *listbox = compo_create(frm, COMPO_TYPE_LISTBOX);
    widget_page_t *page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    listbox->style = style;
    listbox->page = page;
    listbox->item_width = GUI_SCREEN_WIDTH;
    for (i=0; i<LISTBOX_ITEM_CNT; i++)
    {
        widget_page_t *item_page = widget_page_create(page);
        widget_icon_t *item_bgimg = widget_icon_create(item_page, 0);
        widget_icon_t *item_icon = widget_icon_create(item_page, 0);
        widget_text_t *item_text = widget_text_create(item_page, MAX_WORD_CNT);
        widget_text_t *item_text2 = widget_text_create(item_page, MAX_WORD_CNT);
        widget_icon_t *item_icon2 = widget_icon_create(item_page, 0);

        widget_set_align_center(item_bgimg, false);
        widget_set_pos(item_bgimg, 0, 0);
        widget_set_align_center(item_text, false);
        widget_set_align_center(item_text2, false);

        listbox->item_page[i] = item_page;
        listbox->item_bgimg[i] = item_bgimg;
        listbox->item_icon[i] = item_icon;
        listbox->item_text[i] = item_text;
        listbox->item_text2[i] = item_text2;
        listbox->item_icon2[i] = item_icon2;
    }

    switch (style)
    {
        case COMPO_LISTBOX_STYLE_MENU_FOURGRID:
            if (LISTBOX_ITEM_CNT < 10)
            {
                halt(HALT_GUI_COMPO_LISTBOX_CREATE);
            }
            break;
        default:
            break;
    }

    return listbox;
}

//更新列表框初始化
static void compo_listbox_init_update(compo_listbox_t *listbox)
{
    int i;
    int font_height = widget_text_get_height()+2;
    int font_x = (listbox->icon_area.wid >> 1) + (font_height >> 2) + listbox->line_center_y;
    int font_y = (listbox->line_height - font_height) >> 1;
    int icon_x, icon_y = 0;
    int font_w = listbox->item_width - font_x - (font_height >> 2);
    int font_h = 0;
    for (i=0; i<LISTBOX_ITEM_CNT; i++)
    {
        if (i == 0)
        {
            if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
            {
                listbox->line_height = listbox->icon_area.wid + font_height * 2+16;
//                listbox->item_width = listbox->icon_area.wid;
                listbox->item_width = 68;
                listbox->line_space = GUI_SCREEN_WIDTH / 2 - listbox->icon_area.wid * 3 / 2-13;
                listbox->line_height_total = listbox->line_space + listbox->icon_area.wid;
                listbox->total_height = listbox->line_height_total * (listbox->item_cnt - 1) + listbox->line_height;
                listbox->cycle_height = listbox->line_height_total * (listbox->item_cnt / 2 + 1);
                icon_x = listbox->item_width / 2;
                icon_y = listbox->item_width / 2;
                font_x = listbox->item_width / 2;
                font_y = listbox->line_center_y + listbox->icon_area.wid + font_height / 2+6;
                font_w = listbox->item_width;
//                font_w = 76;
                font_h = font_height;
            }
            else
            {
                rect_t location = widget_get_location(listbox->item_bgimg[i]);
                if (listbox->style == COMPO_LISTBOX_STYLE_TITLE_TWO_TEXT)
                {
                    listbox->line_height = max(max(max(font_height*2, listbox->item_height), listbox->icon_area.hei), location.hei);
                }
                else
                {
                    listbox->line_height = max(max(max(font_height, listbox->item_height), listbox->icon_area.hei), location.hei);
                }
                listbox->line_space = listbox->line_height / 8;
                listbox->line_height_total = listbox->line_height + listbox->line_space;
                listbox->line_center_y = listbox->line_height >> 1;
                listbox->total_height = listbox->line_height_total * (listbox->item_cnt - 1) + listbox->line_height;
                listbox->cycle_height = listbox->line_height_total * listbox->item_cnt;
                icon_x = (listbox->icon_area.wid * 2) / 3;
                if (icon_x == 0)
                {
                    icon_x = listbox->line_center_y / 2;
                }
                else
                {
                    int icon_x2 = (listbox->line_center_y * 3) / 4;
                    if (icon_x < icon_x2)
                    {
                        icon_x = icon_x2;
                    }
                }
                font_x = (listbox->icon_area.wid >> 1) + (font_height >> 2) + icon_x;
                if (listbox->style == COMPO_LISTBOX_STYLE_TITLE_TWO_TEXT)
                {
                    font_y = (listbox->line_height/2 - font_height) >> 1;
                }
                else
                {
                    font_y = (listbox->line_height - font_height) >> 1;
                }
                font_w = listbox->item_width - font_x - (font_height >> 1);
                if (listbox->res_sta_icon1)
                {
                    area_t area = gui_image_get_size(listbox->res_sta_icon1);
//                    int icon_x = listbox->item_width - area.wid;
                    font_w = font_w - area.wid;
                }
            }
        }

        if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
        {
            widget_set_size(listbox->item_page[i], listbox->item_width, listbox->line_height);
            widget_set_pos(listbox->item_icon[i], icon_x, icon_y);
            widget_set_location(listbox->item_text[i], font_x, font_y, font_w, font_h);//    widget_set_location(listbox->item_text[i], icon_x/2, listbox->line_height - font_height, font_w, listbox->line_height);
            listbox->flag_text_center = true;
        }
        else
        {
            widget_set_size(listbox->item_page[i], listbox->item_width, listbox->line_height);

            if (listbox->style == COMPO_LISTBOX_STYLE_CUM_SPORT_LIST)
            {
                widget_set_pos(listbox->item_icon[i], icon_x, listbox->line_height / 3);
            }
            else
            {
                widget_set_pos(listbox->item_icon[i], icon_x, listbox->line_center_y);
            }

            if (listbox->style == COMPO_LISTBOX_STYLE_TITLE_TWO_TEXT)
            {
                widget_set_location(listbox->item_text[i], font_x, font_y, font_w, listbox->line_height/2);//    widget_set_location(listbox->item_text[i], icon_x/2, listbox->line_height - font_height, font_w, listbox->line_height);
                widget_set_location(listbox->item_text2[i], font_x, font_y+listbox->line_height/2, font_w, listbox->line_height/2);//    widget_set_location(listbox->item_text[i], icon_x/2, listbox->line_height - font_height, font_w, listbox->line_height);
                //printf("item y [%d,%d] line[%d, %d, %d]\n", font_y, font_y*5, listbox->line_height, listbox->item_height, widget_get_location(listbox->item_bgimg[i]).hei);
            }
            else if (listbox->style == COMPO_LISTBOX_STYLE_CUM_SPORT_LIST)
            {
                font_x = icon_x - listbox->icon_area.wid/2;
                font_y =  listbox->line_height / 3 + listbox->icon_area.hei/2 +(GUI_SCREEN_HEIGHT/14);
                widget_set_location(listbox->item_text[i], font_x, font_y, font_w, widget_text_get_max_height());//    widget_set_location(listbox->item_text[i], icon_x/2, listbox->line_height - font_height, font_w, listbox->line_height);
            }
            else if (listbox->style == COMPO_LISTBOX_STYLE_TITLE_STOPWATCH_RECORD)
            {
                widget_set_location(listbox->item_text[i],  0, font_y-listbox->line_space/2, widget_get_location(listbox->item_bgimg[i]).wid, listbox->line_height);//    widget_set_location(listbox->item_text[i], icon_x/2, listbox->line_height - font_height, font_w, listbox->line_height);
                widget_set_location(listbox->item_text2[i], 0, font_y-listbox->line_space/2, widget_get_location(listbox->item_bgimg[i]).wid, listbox->line_height);//    widget_set_location(listbox->item_text[i], icon_x/2, listbox->line_height - font_height, font_w, listbox->line_height);
                widget_text_set_right_align(listbox->item_text2[i], true);
            }
            else
            {
                widget_set_location(listbox->item_text[i], font_x, font_y, font_w, listbox->line_height);//    widget_set_location(listbox->item_text[i], icon_x/2, listbox->line_height - font_height, font_w, listbox->line_height);
            }
        }
    }
    //listbox->sidx = INT_MIN;
    listbox->sidx = -listbox->item_cnt;
}

/**
 * @brief 设置列表控件内容
 * @param[in] listbox : 列表指针
 * @param[in] item : 存放列表信息，图片、文字等
 * @param[in] item_cnt : 存放列表信息的数量
 **/
void compo_listbox_set(compo_listbox_t *listbox, compo_listbox_item_t const *item, int item_cnt)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SET);
    }
    listbox->item = item;
    listbox->item_cnt = item_cnt;
    if (item_cnt <= 0 || item_cnt > LISTBOX_MAX_ITEM_CNT)
    {
        halt(HALT_GUI_COMPO_LISTBOX_CREATE);
    }
    listbox->icon_area = gui_image_get_size(item[0].res_addr);
    compo_listbox_init_update(listbox);
}

/**
 * @brief 设置列表框背景
 * @param[in] listbox : 列表指针
 * @param[in] res_addr : 图像资源地址
 **/
void compo_listbox_set_bgimg(compo_listbox_t *listbox, u32 res_addr)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SET_BGIMG);
    }
    listbox->item_width = gui_image_get_size(res_addr).wid;
    int i;
    for (i=0; i<LISTBOX_ITEM_CNT; i++)
    {
        widget_icon_set(listbox->item_bgimg[i], res_addr);
        widget_set_pos(listbox->item_bgimg[i],0, 0);
    }
    compo_listbox_init_update(listbox);
}

/**
 * @brief 设置列表框中心焦点背景（高亮效果）
          注意需要在设置背景后调用
 * @param[in] listbox : 列表指针
 * @param[in] res_addr : 图像资源地址
 **/
void compo_listbox_set_focus_bgimg(compo_listbox_t *listbox, u32 focus_res_addr)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SET_BGIMG);
    }
    widget_icon_set(listbox->item_bgimg[LISTBOX_ITEM_CNT/2], focus_res_addr);
}

/**
 * @brief 设置状态图标 ON, OFF
 * @param[in] listbox : 列表指针
 * @param[in] res_addr1 : 图像资源地址
 * @param[in] res_addr2 : 图像资源地址
 **/
void compo_listbox_set_sta_icon(compo_listbox_t *listbox, u32 res_addr1, u32 res_addr2)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SET_STA_ICON);
    }
    int i;
    listbox->res_sta_icon1 = res_addr1;
    listbox->res_sta_icon2 = res_addr2;
    if (res_addr1 != 0)
    {
        area_t area = gui_image_get_size(res_addr1);
        int icon_x = listbox->item_width - area.wid/2 - 10;
        for (i=0; i<LISTBOX_ITEM_CNT; i++)
        {
            widget_set_pos(listbox->item_icon2[i], icon_x, listbox->line_center_y);
        }
        compo_listbox_init_update(listbox);
    }
}

/**
 * @brief 设置获取位变量回调函数
 * @param[in] listbox : 列表指针
 * @param[in] get_bit : 获取位变量回调函数
 **/
void compo_listbox_set_bithook(compo_listbox_t *listbox, u8 (*get_bit)(uint n))
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SET_BITHOOK);
    }
    listbox->get_bit = get_bit;
}

/**
 * @brief 根据列表编号获取Y坐标
 * @param[in] listbox : 列表指针
 * @param[in] idx : 菜单编号
 * @return y坐标
 **/
int compo_listbox_gety_byidx(compo_listbox_t *listbox, int idx)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_GET_Y);
    }
    if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
    {
        return idx / 2 * listbox->line_height_total;        //返回x坐标
    }
    else
    {
        return (listbox->line_center_y + listbox->line_height_total * idx);
    }
}

/**
 * @brief 按Y坐标设置列表框焦点
 * @param[in] listbox : 列表指针
 * @param[in] y : Y坐标
 **/
void compo_listbox_set_focus(compo_listbox_t *listbox, s16 y)
{
    int iy;
    rect_t location;
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SET_FOCUS);
    }

    if (!listbox->flag_cycle)
    {
        if (y < 0)
        {
            y = 0;
        }
        else if (y > listbox->total_height)
        {
            y = listbox->total_height;
        }
    }
    listbox->ofs_y = y;
    location = widget_get_location(listbox->page);
    if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
    {
        iy = location.x - listbox->ofs_y;
        widget_page_set_client(listbox->page, iy, location.hei/2);
    }
    else
    {
        iy = location.y - listbox->ofs_y;
        widget_page_set_client(listbox->page, 0, iy);
    }
}


/**
 * @brief 将选择的图标设为焦点
 * @param[in] listbox : 列表指针
 * @param[in] idx : 图标编号
 **/
void compo_listbox_set_focus_byidx(compo_listbox_t *listbox, int idx)
{
    int y = compo_listbox_gety_byidx(listbox, idx);
    compo_listbox_set_focus(listbox, y);
}

/**
 * @brief 更新列表框Widget
 * @param[in] listbox : 列表指针
 **/
void compo_listbox_update(compo_listbox_t *listbox)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_UPDATE);
    }
    int i;
    int cidx, sidx;
    int local_num = 0;
    //listbox->sidx = INT_MIN;

    if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
    {
        cidx = (listbox->ofs_y / listbox->line_height_total) * 2;
        listbox->focus_icon_idx = cidx;
        sidx = cidx - (LISTBOX_ITEM_CNT / 2 - 2);
    }
    else
    {
        cidx = listbox->ofs_y / listbox->line_height_total;
        cidx -= listbox->ofs_y < 0 ? 1 : 0;  //逆向（循环滑动）
        listbox->focus_icon_idx = cidx;
        sidx = cidx - LISTBOX_ITEM_CNT / 2;
    }
    for (i=0; i<LISTBOX_ITEM_CNT; i++)
    {
        int idx = sidx + i;
        int idx_tmp;
        u32 icon2;

        if (!listbox->flag_cycle)
        {
            if (idx < 0 || idx >= listbox->item_cnt)
            {
                widget_set_visible(listbox->item_page[i], false);
                listbox->item_idx[i] = -1;
                continue;
            }
            else
            {
                listbox->item_idx[i] = idx;
                widget_set_visible(listbox->item_page[i], true);
            }
        }
        else
        {
            idx_tmp = idx + (idx < 0 ? listbox->item_cnt : (idx >= listbox->item_cnt ? -listbox->item_cnt : 0));
            while (idx_tmp < 0 || idx_tmp >= listbox->item_cnt)
            {
                idx_tmp = idx_tmp + (idx_tmp < 0 ? listbox->item_cnt : (idx_tmp >= listbox->item_cnt ? -listbox->item_cnt : 0));
            }
            listbox->item_idx[i] = idx_tmp;
        }

        compo_listbox_item_t const *item = &listbox->item[listbox->item_idx[i]];
        if (listbox->flag_alike_icon)           //使用同一图标
        {
            widget_icon_set(listbox->item_icon[i], listbox->alike_icon);
        }
        else
        {
            widget_icon_set(listbox->item_icon[i], item->res_addr);
        }
        if (listbox->flag_text_modify == 0)         //默认用i18n里的文本内容
        {
            widget_text_set(listbox->item_text[i], i18n[item->str_idx]);
        }
        else if (listbox->flag_text_modify == 1)
        {
            if (listbox->flash_read_flag)
            {
                local_num = listbox->focus_icon_idx >= 5 ? listbox->focus_icon_idx - 5 : 0;
            }
            compo_listbox_custom_item_t *custom_item = &listbox->custom_item[listbox->item_idx[i] - local_num];
            widget_text_set(listbox->item_text[i], custom_item->str_txt);
        }
        else if (listbox->flag_text_modify == 2)            //用于通话记录一行显示
        {
            static char str_txt[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH + UTE_MODULE_CALL_ADDRESSBOOK_NUMBER_MAX_LENGTH] = {0};

            if (listbox->set_icon_callback != NULL)
            {
                u32 icon_res = listbox->set_icon_callback(listbox->item_idx[i]);
                widget_icon_set(listbox->item_icon[i], icon_res);
            }

            if (listbox->set_text_modify_by_idx_callback != NULL && str_txt != NULL)
            {
                if (listbox->set_text_modify_by_idx_callback(listbox->item_cnt, str_txt, listbox->item_idx[i]))
                {
                    widget_set_visible(listbox->item_page[i], true);
                    widget_text_set(listbox->item_text[i], str_txt);
                }
                else
                {
                    widget_set_visible(listbox->item_page[i], false);
                }
            }
        }
        else if (listbox->flag_text_modify == 3)            //用于电话簿两行显示
        {
            static char str_txt_name[MAX_WORD_CNT+10] = {0};
            static char str_txt_number[MAX(UTE_MODULE_CALL_ADDRESSBOOK_NUMBER_MAX_LENGTH, MIN_WORD_CNT)] = {0};
            memset(str_txt_name, 0, sizeof(str_txt_name));
            memset(str_txt_number, 0, sizeof(str_txt_number));

            if (listbox->set_icon_callback != NULL)
            {
                u32 icon_res = listbox->set_icon_callback(listbox->item_idx[i]);
                widget_icon_set(listbox->item_icon[i], icon_res);
            }

            if (listbox->set_text_modify_by_idx_callback2 != NULL && str_txt_name != NULL && str_txt_number != NULL)
            {
                if (listbox->set_text_modify_by_idx_callback2(listbox->item_cnt, str_txt_name, sizeof(str_txt_name), str_txt_number, sizeof(str_txt_number), listbox->item_idx[i]))
                {
                    widget_set_visible(listbox->item_page[i], true);
                    widget_text_set(listbox->item_text[i], str_txt_name);
                    if (listbox->set_text1_color_callback != NULL)
                    {
                        widget_text_set_color(listbox->item_text[i], listbox->set_text1_color_callback(listbox->item_idx[i]));
                    }
                    widget_text_set(listbox->item_text2[i], str_txt_number);
                    if (listbox->set_text2_color_callback != NULL)
                    {
                        widget_text_set_color(listbox->item_text2[i], listbox->set_text2_color_callback(listbox->item_idx[i]));
                    }
                    else
                    {
                        widget_text_set_color(listbox->item_text2[i], COLOR_GRAY);
                    }
                }
                else
                {
                    widget_set_visible(listbox->item_page[i], false);
                }
//                printf("[%d,%d] %s:%s\n", sizeof(str_txt_name), sizeof(str_txt_number), str_txt_name, str_txt_number);
            }
        }
        widget_set_align_center(listbox->item_text[i], listbox->flag_text_center);
        widget_set_align_center(listbox->item_text2[i], listbox->flag_text_center);
        if (listbox->style != COMPO_LISTBOX_STYLE_SELECT && listbox->style != COMPO_LISTBOX_STYLE_LANGUAGE)
        {
            switch (item->item_mode)
            {
                case COMPO_LISTBOX_ITEM_MODE_NORMAL:
                    //普通菜单
                    widget_icon_set(listbox->item_icon2[i], 0);
                    break;

                case COMPO_LISTBOX_ITEM_MODE_SWITCH:
                    //选择开关
                    icon2 = (listbox->get_bit != NULL && listbox->get_bit(item->vidx) != 0) ? listbox->res_sta_icon1 : listbox->res_sta_icon2;
                    widget_icon_set(listbox->item_icon2[i], icon2);
                    break;

                default:
                    halt(HALT_GUI_COMPO_LISTBOX_ITEM_MODE);
                    break;
            }
        }

        int lnx = GUI_SCREEN_CENTER_X;
        int lny = listbox->line_center_y + idx * listbox->line_height_total;
        int dy = lny - listbox->ofs_y;                  //离中心距离
        int udy = abs_s(dy);
        int item_wid = listbox->item_width;
        int item_hei = listbox->line_height;
        int delta_hei;
        int udy_th = LISTBOX_ITEM_SIZE_THRESHOLD;
        bool flag_scale = false;
        switch (listbox->style)
        {
            case COMPO_LISTBOX_STYLE_MENU_NORMAL:
            case COMPO_LISTBOX_STYLE_CUM_SPORT_LIST:
                //普通菜单
                flag_scale = (udy > udy_th);
                break;

            case COMPO_LISTBOX_STYLE_MENU_CIRCLE:
                //圆屏菜单弧形样式
                lnx += LISTBOX_STYLE_CIRCLE_R - sqrt64(LISTBOX_STYLE_CIRCLE_R * LISTBOX_STYLE_CIRCLE_R - dy * dy);
                udy_th = LISTBOX_ITEM_SIZE_THRESHOLD_CIRCLE;
                flag_scale = (udy > udy_th);
                break;

            case COMPO_LISTBOX_STYLE_TITLE:
            case COMPO_LISTBOX_STYLE_TITLE_TWO_TEXT:
            case COMPO_LISTBOX_STYLE_TITLE_STOPWATCH_RECORD:
                //带标题的列表，只在底部做缩放
                flag_scale = (dy > udy_th);
                break;

            case COMPO_LISTBOX_STYLE_TITLE_NORMAL:
                //带标题的列表，头部底部不做缩放
                flag_scale = false;
                break;

            case COMPO_LISTBOX_STYLE_SELECT:
                flag_scale = (dy > udy_th);
            case COMPO_LISTBOX_STYLE_LANGUAGE:
                //选择模式 语言选择模式
                icon2 = (listbox->get_bit != NULL && listbox->get_bit(item->vidx) != 0) ? listbox->res_sta_icon1 : listbox->res_sta_icon2;
                widget_icon_set(listbox->item_icon2[i], icon2);
                break;

            case COMPO_LISTBOX_STYLE_MENU_FOURGRID:
                flag_scale = false;
                lnx = idx / 2 * listbox->line_height_total;
                if (idx % 2 == 0)       //上图标
                {
                    lny = -listbox->line_height / 4;
                }
                else                    //下图标
                {
                    lny = listbox->line_height * 3 / 4;
                }
                break;

            default:
                halt(HALT_GUI_COMPO_LISTBOX_STYLE);
                break;
        }
        if (flag_scale)
        {
            item_wid = item_wid - item_wid * (udy - udy_th) / (GUI_SCREEN_HEIGHT);
            item_hei = item_hei * item_wid / listbox->item_width;
            delta_hei = (listbox->line_height - item_hei) >> 1;
            if (dy > 0)
            {
                lny -= delta_hei;
            }
            else
            {
                lny += delta_hei;
            }
        }
        widget_page_scale_to(listbox->item_page[i], item_wid, item_hei);
        widget_set_pos(listbox->item_page[i], lnx, lny);
        // 更新时钟尺寸
        if (listbox->page_time != NULL && listbox->idx_time == listbox->item_idx[i])
        {
            listbox->time_bg = listbox->item_icon[i];
            rect_t rect_abs;
            rect_t rect_loc = widget_get_location(listbox->item_icon[i]);
            rect_t rect_loc_page = widget_get_location(listbox->page);

            rect_abs.wid = item_wid * rect_loc.wid / listbox->item_width;
            rect_abs.hei = item_hei * rect_loc.hei / listbox->line_height;
            s16 icon_ofs = ((listbox->icon_area.wid * 2) / 3 * item_wid / listbox->item_width);        //icon在item_page中的偏移
            s16 item_ofs = ((listbox->item_width - item_wid) / 2);                                     //item_page缩放后，对比原来大小的偏移
            s16 page_ofs = (lnx - rect_loc_page.x + (rect_loc_page.wid - listbox->item_width) / 2);    //item_page相对于listbox_page的偏移
            rect_abs.x = icon_ofs + item_ofs + page_ofs;

            widget_page_scale_to(listbox->page_time, rect_abs.wid, rect_abs.hei);
            if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
            {
                lny = lny - widget_text_get_height();
            }
            else
            {
                lnx = rect_abs.x;
            }
            widget_set_pos(listbox->page_time, lnx, lny);
        }
    }

    //滚动控制器
    if (sidx != listbox->sidx || listbox->txt_roll_need_rst)
    {
        int i_end = 0;
        i = 0;
        if (sidx > listbox->sidx)               //向上滑动
        {
            int k = sidx - listbox->sidx;       //向上滑动的列表行数

            //if(k <= INT_MIN) {
            if(k <= -listbox->item_cnt)         //正常运行不可能运行到这
            {
                listbox->sidx = 0;
                k = LISTBOX_ITEM_CNT;
            }

            for (i=0; i<LISTBOX_ITEM_CNT-k; i++)        //将下一行列表项对应的滚动控制器 赋值给 上一行列表对应的滚动控制器
            {
                listbox->roll_cb[i] = listbox->roll_cb[i + k];
            }
            i_end = LISTBOX_ITEM_CNT;
        }
        else if (sidx < listbox->sidx)          //向下滑动
        {
            int k = listbox->sidx - sidx;       //向下滑动的列表行数
            for (i_end=LISTBOX_ITEM_CNT; i_end>k; i_end--)      //将上一行列表项对应的滚动控制器 赋值给 下一行列表对应的滚动控制器
            {
                listbox->roll_cb[i_end - 1] = listbox->roll_cb[i_end - 1 - k];
            }
            i = 0;
        }

        for (; i<i_end; i++)                    //对移动超出列表项个数一半的滚动控制器重新赋值
        {
            memset(&listbox->roll_cb[i], 0, sizeof(compo_roll_cb_t));
            listbox->roll_cb[i].tick = tick_get();
            /*
            widget_text_t *txt = listbox->item_text[i];
            if (widget_get_visble(txt)) {
                area_t text_area = widget_text_get_area(txt);
                rect_t textbox_rect = widget_get_location(txt);
                if (text_area.wid > textbox_rect.wid) {
                    listbox->roll_cb[i].mode = TEXT_AUTOROLL_MODE_SROLL_CIRC;
                    listbox->roll_cb[i].direction = -1;
                    if (widget_get_align_center(txt)) {
                        listbox->roll_cb[i].offset = (text_area.wid - textbox_rect.wid) / 2;
                    }
                    widget_text_set_autoroll_mode(listbox->item_text[i], TEXT_AUTOROLL_MODE_SROLL_CIRC);
                } else {
                    widget_text_set_autoroll_mode(listbox->item_text[i], TEXT_AUTOROLL_MODE_NULL);
                }
            }
            */
        }

        for (i=0; i<LISTBOX_ITEM_CNT; i++)          //判断列表项的中是否可以滚动
        {
            if (listbox->txt_roll_need_rst)
            {
                memset(&listbox->roll_cb[i], 0, sizeof(compo_roll_cb_t));
                listbox->roll_cb[i].tick = tick_get();
            }
            widget_text_t *txt = listbox->item_text[i];
            if (widget_get_visble(txt))
            {
                area_t text_area = widget_text_get_area(txt);
//                rect_t textbox_rect = widget_get_location(txt);
                area_t textbox_rect = widget_text_get_box_area_rel(txt);
                if (text_area.wid > textbox_rect.wid)
                {
                    listbox->roll_cb[i].mode = TEXT_AUTOROLL_MODE_SROLL_CIRC;
                    listbox->roll_cb[i].direction = -1;
                    if (widget_get_align_center(txt))
                    {
                        if(listbox->roll_cb[i].mode != TEXT_AUTOROLL_MODE_SROLL_CIRC)
                        {
                            listbox->roll_cb[i].offset = (text_area.wid - textbox_rect.wid) / 2;
                        }
                    }
                    widget_text_set_autoroll_mode(listbox->item_text[i], TEXT_AUTOROLL_MODE_SROLL_CIRC);
                }
                else
                {
                    widget_text_set_autoroll_mode(listbox->item_text[i], TEXT_AUTOROLL_MODE_NULL);
                }
            }
        }
        for (i=0; i<LISTBOX_ITEM_CNT; i++)
        {
            widget_text_set_client(listbox->item_text[i], listbox->roll_cb[i].offset, 0);
        }
        listbox->sidx = sidx;
        listbox->txt_roll_need_rst = false;
    }
}

/**
 * @brief 更新列表框Widget，同时重置所有文本滚动（item中str突变时需要调用，防止C208蓝屏）
 * @param[in] listbox : 列表指针
 **/
void compo_listbox_update_with_text_scroll_rst(compo_listbox_t *listbox)
{
    listbox->txt_roll_need_rst = true;
    compo_listbox_update(listbox);
}

/**
 * @brief 按坐标选择菜单项
 * @param[in] listbox : 列表指针
 * @param[in] pt : 坐标
 * @return 返回菜单项索引
 **/
int compo_listbox_select(compo_listbox_t *listbox, point_t pt)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SELECT);
    }
    int i;
    rect_t rect = widget_get_absolute(listbox->page);
    s16 fot_x, fot_y;
    fot_x = pt.x - rect.x;
    fot_y = pt.y - rect.y;
    if (abs_s(pt.y - rect.y) * 2 < rect.hei)
    {
        for (i=0; i<LISTBOX_ITEM_CNT; i++)
        {
            if (listbox->item_idx[i] < 0)
            {
                continue;
            }
            widget_page_t *page = listbox->item_page[i];
            rect = widget_get_absolute(page);
            if ( (fot_x  >= LIST_CUSTOM_AREA_X_MIN) && (fot_x <= LIST_CUSTOM_AREA_X_MAX) && (fot_y >= LIST_CUSTOM_AREA_Y_MIN) && (fot_y <= LIST_CUSTOM_AREA_Y_MAX))
            {
                listbox->flag_area = 1;
            }

            if ((abs_s(pt.y - rect.y) * 2 <= rect.hei))
            {
                if ((listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID && (abs_s(pt.x - rect.x) * 2 <= rect.wid)) || (listbox->style != COMPO_LISTBOX_STYLE_MENU_FOURGRID))
                {
                    listbox->sel_page = listbox->item_page[i];
                    listbox->sel_icon = listbox->item_icon[i];
                    listbox->sel_icon2 = listbox->item_icon2[i];
                    listbox->sel_idx = listbox->item_idx[i];
                    listbox->flag_sel_icon2 = false;
                    rect = widget_get_absolute(listbox->sel_icon2);
                    if (rect.wid > 0 && pt.x >= (rect.x - (rect.wid >> 1)))
                    {
                        listbox->flag_sel_icon2 = true;                 //点击在右边状态图标区域
                    }
                    return listbox->sel_idx;
                }
            }
        }
    }
    listbox->sel_icon = NULL;
    listbox->sel_icon2 = NULL;
    listbox->sel_idx = -1;
    listbox->flag_sel_icon2 = false;
    return -1;
}

/**
 * @brief 按坐标选择菜单项
 * @param[in] listbox : 列表指针
 * @return 返回是否选择状态图标
 **/
bool compo_listbox_is_sel_sta(compo_listbox_t *listbox)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SELECT);
    }
    return listbox->flag_sel_icon2;
}

/**
 * @brief 按索引选择图标
 * @param[in] listbox : 列表指针
 * @param[in] idx : 编号
 * @return 返回图标指针
 **/
widget_icon_t *compo_listbox_select_byidx(compo_listbox_t *listbox, int idx)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SELECT);
    }
    int i;
    for (i=0; i<LISTBOX_ITEM_CNT; i++)
    {
        if (listbox->item_idx[i] == idx)
        {
            listbox->sel_page = listbox->item_page[i];
            listbox->sel_icon = listbox->item_icon[i];
            listbox->sel_icon2 = listbox->item_icon2[i];
            listbox->sel_idx = idx;
            return listbox->sel_icon;
        }
    }
    listbox->sel_icon = NULL;
    listbox->sel_icon2 = NULL;
    listbox->sel_idx = -1;
    listbox->flag_sel_icon2 = false;
    return NULL;
}

/**
 * @brief 获取选择图标的区域
 * @param[in] listbox : 列表指针
 * @return 返回图标坐标和大小
 **/
rect_t compo_listbox_get_sel_rect(compo_listbox_t *listbox)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_GET_SELECT);
    }
    rect_t rect = {0};
    if (listbox->sel_page == NULL || listbox->sel_icon == NULL)
    {
        return rect;
    }
    rect_t page_rect = widget_get_location(listbox->sel_page);
    rect = widget_get_location(listbox->sel_icon);
    rect.y = rect.y - listbox->ofs_y + page_rect.y;
    return rect;
}

/**
 * @brief 列表框拖动与移动初始化
 * @param[in] listbox : 列表指针
 * @return 返回COMPO_LISTBOX_STA状态枚举
 **/
u8 compo_listbox_get_sta(compo_listbox_t *listbox)
{
    compo_listbox_move_cb_t *mcb = listbox->mcb;
    if (mcb == NULL)
    {
        return COMPO_LISTBOX_STA_IDLE;
    }
    if (mcb->flag_drag)
    {
        return COMPO_LISTBOX_STA_DARG;
    }
    else if (mcb->flag_move_auto)
    {
        return COMPO_LISTBOX_STA_MOVE;
    }
    else
    {
        return COMPO_LISTBOX_STA_IDLE;
    }
}

/**
 * @brief 列表框拖动与移动初始化
 * @param[in] listbox : 列表指针
 **/
void compo_listbox_move_init(compo_listbox_t *listbox)
{
    compo_listbox_move_cb_t *mcb = listbox->mcb;
    if (mcb == NULL)
    {
        return;
    }
    mcb->focus_y = listbox->ofs_y;
    if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
    {
        mcb->first_y = compo_listbox_gety_byidx(listbox, 3);
        mcb->last_y = compo_listbox_gety_byidx(listbox, listbox->item_cnt - 3);
    }
    else
    {
        mcb->first_y = compo_listbox_gety_byidx(listbox, 0);
        mcb->last_y = compo_listbox_gety_byidx(listbox, listbox->item_cnt - 1);
    }

}

/**
 * @brief 列表框拖动与移动初始化
          用于自定义首尾图标位置的修改
 * @param[in] listbox : 列表指针
 * @param[in] first_y : 首图标坐标
 * @param[in] last_y : 尾图标坐标
 **/
void compo_listbox_move_init_modify(compo_listbox_t *listbox, s32 first_y, s32 last_y)
{
    compo_listbox_move_cb_t *mcb = listbox->mcb;
    if (mcb == NULL)
    {
        return;
    }
    mcb->focus_y = listbox->ofs_y;
    mcb->first_y = first_y;
    mcb->last_y = last_y;
}

/**
 * @brief 列表框拖动与移动处理
 * @param[in] listbox : 列表指针
 **/
void compo_listbox_move(compo_listbox_t *listbox)
{
    compo_listbox_move_cb_t *mcb = listbox->mcb;
    if (mcb == NULL)
    {
        return;
    }
    if (mcb->flag_drag)
    {
        s32 dx;
        s32 dy = mcb->focus_dy;
        if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
        {
            mcb->flag_drag = ctp_get_dxy(&mcb->focus_dy, &dx);
        }
        else
        {
            mcb->flag_drag = ctp_get_dxy(&dx, &mcb->focus_dy);
        }
        if (mcb->flag_drag)
        {
            //拖动菜单图标
            mcb->focus_ystep = mcb->focus_dy - dy;
            if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
            {
                int Max_x = (GUI_SCREEN_WIDTH/3*(listbox->item_cnt/2+listbox->item_cnt%2))-GUI_SCREEN_CENTER_X;
                if(mcb->focus_y > Max_x)
                {
                    mcb->focus_y = Max_x;
                }
            }
            compo_listbox_set_focus(listbox, mcb->focus_y - mcb->focus_dy);
            compo_listbox_update(listbox);
        }
        else
        {
            //抬手后开始自动移动
            s32 last_dy;
            if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
            {
                last_dy = ctp_get_last_dxy().x;
            }
            else
            {
                last_dy = ctp_get_last_dxy().y;
            }
            mcb->focus_y = listbox->ofs_y;
            mcb->flag_move_auto = true;
            if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
            {
                mcb->moveto_idx = listbox->focus_icon_idx - (last_dy * DRAG_AUTO_SPEED / GUI_SCREEN_WIDTH);
            }
            else
            {
                mcb->moveto_idx = listbox->focus_icon_idx - (last_dy * DRAG_AUTO_SPEED / GUI_SCREEN_HEIGHT);
            }
            if (mcb->moveto_idx > listbox->item_cnt && !listbox->flag_cycle)
            {
                mcb->moveto_idx = listbox->item_cnt;
            }
            else if (mcb->moveto_idx < -1 && !listbox->flag_cycle)
            {
                mcb->moveto_idx = -1;
            }
            mcb->moveto_y = compo_listbox_gety_byidx(listbox, mcb->moveto_idx);
            mcb->tick = tick_get();
        }


    }
    if (mcb->flag_move_auto)
    {
        //自动移动
        if (mcb->focus_y == mcb->moveto_y)
        {
            if (mcb->focus_y < mcb->first_y && !listbox->flag_cycle)
            {
                mcb->moveto_y = mcb->first_y;             //超过第1个，回弹
            }
            else if (mcb->focus_y > mcb->last_y && !listbox->flag_cycle)
            {
                mcb->moveto_y = mcb->last_y;              //超过最后1个，回弹
            }
            else
            {
                mcb->flag_move_auto = false;              //移动完成
                listbox->ofs_y %= listbox->cycle_height;  //循环滑动防止ofs_y溢出
                if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
                {
                    listbox->focus_icon_idx = (listbox->ofs_y / listbox->line_height_total) * 2;
                }
                else
                {
                    listbox->focus_icon_idx = listbox->ofs_y / listbox->line_height_total - (listbox->ofs_y < 0 ? 1 : 0);
                }
                mcb->focus_y = listbox->ofs_y;
            }
        }
        else if (tick_check_expire(mcb->tick, FOCUS_AUTO_TICK_EXPIRE))
        {
            s32 dy;
            mcb->tick = tick_get();
            dy = mcb->moveto_y - mcb->focus_y;
            if (dy > 0)
            {
                if (dy > FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy > FOCUS_AUTO_STEP)
                {
                    dy = FOCUS_AUTO_STEP;
                }
                else
                {
                    dy = 1;
                }
            }
            else
            {
                if (dy < -FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy < -FOCUS_AUTO_STEP)
                {
                    dy = -FOCUS_AUTO_STEP;
                }
                else
                {
                    dy = -1;
                }
            }
            mcb->focus_y += dy;
            compo_listbox_set_focus(listbox, mcb->focus_y);
            compo_listbox_update(listbox);
        }
    }

}

/**
 * @brief 列表框拖动与移动控制
 * @param[in] listbox : 列表指针
 * @param[in] cmd : 控制命令
 **/
void compo_listbox_move_control(compo_listbox_t *listbox, int cmd)
{
    compo_listbox_move_cb_t *mcb = listbox->mcb;
    if (mcb == NULL)
    {
        return;
    }
    switch (cmd)
    {
        case COMPO_LISTBOX_MOVE_CMD_DRAG:
            //开始拖动
            mcb->flag_drag = true;
            mcb->flag_move_auto = false;
            listbox->ofs_y %= listbox->cycle_height;  //循环滑动防止ofs_y溢出
            if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
            {
                listbox->focus_icon_idx = (listbox->ofs_y / listbox->line_height_total) * 2;
            }
            else
            {
                listbox->focus_icon_idx = listbox->ofs_y / listbox->line_height_total - (listbox->ofs_y < 0 ? 1 : 0);
            }
            mcb->focus_y = listbox->ofs_y;
            break;

        case COMPO_LISTBOX_MOVE_CMD_FORWARD:
            //向前滚动
            if (!mcb->flag_move_auto)
            {
                mcb->flag_move_auto = true;
                mcb->moveto_idx = listbox->focus_icon_idx;
            }
            if (mcb->moveto_idx <= listbox->item_cnt - 1 || listbox->flag_cycle)
            {
                if (listbox->style == COMPO_LISTBOX_STYLE_MENU_FOURGRID)
                {
                    mcb->moveto_idx+=2;
                }
                else
                {
                    mcb->moveto_idx++;
                }
                mcb->moveto_y = compo_listbox_gety_byidx(listbox, mcb->moveto_idx);
            }
            break;

        case COMPO_LISTBOX_MOVE_CMD_BACKWARD:
            //向后滚动
            if (!mcb->flag_move_auto)
            {
                mcb->flag_move_auto = true;
                mcb->moveto_idx = listbox->focus_icon_idx;
            }
            if (mcb->moveto_idx >= 0 || listbox->flag_cycle)
            {
                mcb->moveto_idx--;
                mcb->moveto_y = compo_listbox_gety_byidx(listbox, mcb->moveto_idx);
            }
            break;

        default:
            halt(HALT_GUI_COMPO_LISTBOX_MOVE_CMD);
            break;
    }
}

/**
 * @brief 设置列表坐标及大小
          用于限制page的大小，超出page的地方不显示出来
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] btn : 列表指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 * @param[in] width : 列表宽度
 * @param[in] height : 列表高度
 **/
void compo_listbox_set_location(compo_listbox_t *listbox, s16 x, s16 y, s16 width, s16 height)
{
    widget_set_location(listbox->page, x, y, width, height);
}

/**
 * @brief 设置列表控件item的高度
          用于无图标和背景图时，自定义列表中item的高度
 * @param[in] listbox : 列表指针
 * @param[in] item_height : item高度
 **/
void compo_listbox_set_item_height(compo_listbox_t *listbox, int item_height)
{
    listbox->item_height = item_height;
    compo_listbox_init_update(listbox);
}

/**
 * @brief 设置列表控件的文本坐标、大小、是否居中显示
          注意：需在compo_listbox_update前一步调用，避免被compo_listbox_init_update刷新了
 * @param[in] listbox : 列表指针
 * @param[in] item : 存放列表信息，图片、文字等
 * @param[in] item_cnt : 存放列表信息的数量
 **/
void compo_listbox_set_item_text(compo_listbox_t *listbox, s16 x, s16 y, s16 width, s16 height, bool align_center)
{
    listbox->flag_text_center = align_center;
    for (int i=0; i<LISTBOX_ITEM_CNT; i++)
    {
        widget_set_location(listbox->item_text[i], x, y, width, height);
    }
}

/**
 * @brief 是否修改列表控件的文本内容
 * @param[in] listbox : 列表指针
 * @param[in] custom_item : 存放自定义文本的数组
 **/
void compo_listbox_set_text_modify(compo_listbox_t *listbox, compo_listbox_custom_item_t *custom_item)
{
    listbox->flag_text_modify = true;
    listbox->custom_item = custom_item;
}

/**
 * @brief 是否使能列表循环滑动功能
 * @param[in] listbox : 列表指针
 * @param[in] cycle : true 使能循环滑动
                      false 关闭循环滑动
 **/
void compo_listbox_cycle_en(compo_listbox_t *listbox, bool cycle)
{
    listbox->flag_cycle = cycle;
}

/**
 * @brief 向集合中添加时分秒针
          cx cy为旋转中心点, x y为目标坐标
 * @param[in] listbox : 图标集指针
 * @param[in] type : COMPO_ICONLIST_TIME_TYPE_HOUR, 时
                     COMPO_ICONLIST_TIME_TYPE_MIN,  分
                     COMPO_ICONLIST_TIME_TYPE_SEC,  秒
 * @param[in] res_addr : 图像资源地址
 * @param[in] cx : x轴坐标
 * @param[in] cy : y轴坐标
 **/
void compo_listbox_add_time(compo_listbox_t *listbox, u8 type, u32 res_addr, s16 cx, s16 cy)
{
    if (listbox->page_time == NULL)
    {
        listbox->page_time = widget_page_create(listbox->page);
        widget_set_location(listbox->page_time, 0, 0, listbox->icon_area.wid, listbox->icon_area.hei);
        widget_page_set_client(listbox->page_time, listbox->icon_area.wid / 2, listbox->icon_area.hei / 2);
    }
    widget_image_t *img = widget_image_create(listbox->page_time, res_addr);
    widget_image_set_rotation_center(img, cx, cy);

    switch (type)
    {
        case COMPO_ICONLIST_TIME_TYPE_HOUR:
            listbox->hour = img;
            break;

        case COMPO_ICONLIST_TIME_TYPE_MIN:
            listbox->min = img;
            break;

        case COMPO_ICONLIST_TIME_TYPE_SEC:
            listbox->sec = img;
            break;

        default:
            halt(HALT_GUI_COMPO_ICONLIST_TIME_TYPE);
            return;
    }
    compo_cb.rtc_update = true;                 //创建时钟组件，需要同步更新时间
}

/**
 * @brief 设置图标集指针的起始角度
 * @param[in] listbox : 图标集指针
 * @param[in] angle : 原图的起始角度
 **/
void compo_listbox_set_start_angle(compo_listbox_t *listbox, s16 angle)
{
    listbox->start_angle = angle;
}

/**
 * @brief 设置图标集合时钟序号
 * @param[in] listbox : 图标集指针
 * @param[in] idx : 时钟序号
 **/
void compo_listbox_set_time_idx(compo_listbox_t *listbox, u8 idx)
{

    listbox->idx_time = idx;
}

/**
 * @brief 通过回调函数传出的idx, 用户自行判断要传入显示的字符，用于通话记录场景
 * @param[in] listbox : 图标集指针
 * @param[in] callback : 回调函数
 **/
void compo_listbox_set_text_modify_by_idx_callback(compo_listbox_t *listbox, void* callback)
{
    listbox->flag_text_modify = 2;
    listbox->set_text_modify_by_idx_callback = callback;
}

/**
 * @brief 通过回调函数传出的idx, 用户自行判断要传入显示的字符，用于联系人通讯录场景
 * @param[in] listbox : 图标集指针
 * @param[in] callback : 回调函数
 **/
void compo_listbox_set_text_modify_by_idx_callback2(compo_listbox_t *listbox, void* callback)
{
    listbox->flag_text_modify = 3;
    listbox->set_text_modify_by_idx_callback2 = callback;
}

/**
 * @brief 列表图标全部采用相同的
 * @param[in] listbox : 图标集指针
 * @param[in] res : 图片资源
 **/
void compo_listbox_set_alike_icon(compo_listbox_t *listbox, u32 res)
{
    listbox->flag_alike_icon = true;
    listbox->icon_area = gui_image_get_size(res);
    listbox->alike_icon = res;
    compo_listbox_init_update(listbox);
}

/**
 * @brief 列表是否可以显示
 * @param[in] listbox : 图标集指针
 * @param[in] visible : 是否可以显示
 **/
void compo_listbox_set_visible(compo_listbox_t *listbox, bool visible)
{
    widget_set_visible(listbox->page, visible);
}

/**
 * @brief 设置列表控件图标区域
 * @param[in] listbox : 列表指针
 * @param[in] area : 区域大小
 **/
void compo_listbox_set_icon_area(compo_listbox_t *listbox, area_t area)
{
    if (listbox == NULL)
    {
        halt(HALT_GUI_COMPO_LISTBOX_SET);
    }
    listbox->icon_area = area;
    compo_listbox_init_update(listbox);
}

/**
 * @brief 通过回调函数传出的idx, 用户自行判断要传入显示的图标，用于联系人通讯录场景
 * @param[in] listbox : 图标集指针
 * @param[in] callback : 回调函数
 **/
void compo_listbox_set_icon_callback(compo_listbox_t *listbox, void* callback)
{
    listbox->set_icon_callback = callback;
}

/**
 * @brief 通过回调函数传出的idx, 用户自行判断要传入显示的字符颜色，用于联系人通讯录场景
 * @param[in] listbox : 图标集指针
 * @param[in] callback : 回调函数
 **/
void compo_listbox_set_text1_color_callback(compo_listbox_t *listbox, void* callback)
{
    listbox->set_text1_color_callback = callback;
}

void compo_listbox_set_text2_color_callback(compo_listbox_t *listbox, void* callback)
{
    listbox->set_text2_color_callback = callback;
}

