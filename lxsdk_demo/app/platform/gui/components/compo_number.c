#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

/**
 * @brief 根据数字图像资源创建一个数字
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 数字图片资源地址
 * @param[in] num_cnt : 表示该数字最大位数
 * @return 返回数字指针
 **/
compo_number_t *compo_number_create(compo_form_t *frm, u32 res_addr, int num_cnt)
{
    int i;
    if (num_cnt <= 0 || num_cnt > MAX_NUMBER_CNT)
    {
        halt(HALT_GUI_COMPO_NUMBER_CNT);
    }

    compo_number_t *num = compo_create(frm, COMPO_TYPE_NUMBER);
    widget_page_t *page = frm->page_body;
    num->flag_zfill = false;
    num->num_cnt = num_cnt;
    num->max_val = 1;
    num->visible = true;

    for (i=0; i<num_cnt; i++)
    {
        //需要进行图像裁剪
        widget_image_t *img = widget_image_create(page, res_addr);
        if (num->num_wid == 0)
        {
            area_t area = gui_image_get_size(res_addr);
            num->num_wid = area.wid;
            num->num_hei = area.hei / 10;
            num->total_hei = area.hei;
        }
        widget_set_size(img, num->num_wid, num->num_hei);
        num->img_num[i] = img;
        num->max_val *= 10;
    }

    widget_image_t *img_sym = widget_image_create(page, res_addr);      //符号位
    widget_set_size(img_sym, num->num_wid, num->num_hei);
    num->img_sym = img_sym;

    num->flag_sym = false;      //默认不使用符号
    num->flag_float = false;
    num->flag_zero_replace = false;
    num->radix = 10;
    num->max_val--;
    compo_number_set_margin(num, num->num_wid / 8);
    return num;
}

/**
 * @brief 根据数字图像资源创建一个数字
 * @param[in] frm : 窗体指针
 * @param[in] parent : 父页面
 * @param[in] res_addr : 数字图片资源地址
 * @param[in] num_cnt : 表示该数字最大位数
 * @return 返回数字指针
 **/
compo_number_t *compo_number_create_for_page(compo_form_t *frm, widget_page_t* parent, u32 res_addr, int num_cnt)
{
    int i;
    if (num_cnt <= 0 || num_cnt > MAX_NUMBER_CNT)
    {
        halt(HALT_GUI_COMPO_NUMBER_CNT);
    }

    compo_number_t *num = compo_create(frm, COMPO_TYPE_NUMBER);
    widget_page_t *page = parent;
    num->flag_zfill = false;
    num->num_cnt = num_cnt;
    num->max_val = 1;
    num->visible = true;

    for (i=0; i<num_cnt; i++)
    {
        //需要进行图像裁剪
        widget_image_t *img = widget_image_create(page, res_addr);
        if (num->num_wid == 0)
        {
            area_t area = gui_image_get_size(res_addr);
            num->num_wid = area.wid;
            num->num_hei = area.hei / 10;
            num->total_hei = area.hei;
        }
        widget_set_size(img, num->num_wid, num->num_hei);
        num->img_num[i] = img;
        num->max_val *= 10;
    }

    widget_image_t *img_sym = widget_image_create(page, res_addr);      //符号位
    widget_set_size(img_sym, num->num_wid, num->num_hei);
    num->img_sym = img_sym;

    num->flag_sym = false;      //默认不使用符号
    num->flag_float = false;
    num->flag_zero_replace = false;
    num->radix = 10;
    num->max_val--;
    compo_number_set_margin(num, num->num_wid / 8);
    return num;
}

/**
 * @brief 设置数字框裁剪个数
 * @param[in] num : 数字指针
 * @param[in] radix : 裁剪图片资源拼图个数
 * @param[in] type : 数字图片资源是否带符号
 **/
void compo_number_set_radix(compo_number_t *num, u8 radix, bool type)
{
    if(radix == 14)
    {
        num->radix = radix;
    }
    else if (type)
    {
        num->radix = radix-1;
    }
    num->flag_sym = type;
    num->num_hei = num->total_hei / radix;
    num->max_val = 1;
    for (int i=0; i<num->num_cnt; i++)
    {
        widget_set_size(num->img_num[i], num->num_wid, num->num_hei);
        num->max_val *= num->radix;
    }
    num->max_val--;
}

//更新内容
static void compo_number_update(compo_number_t *num)
{
    int i;
    int val = num->value;
    int total_len;

    if (num->num_layout)
    {
        total_len = num->num_hei * num->num_cnt + num->x_margin * (num->num_cnt - 1);   //total_hei
    }
    else
    {
        total_len = num->num_wid * num->num_cnt + num->x_margin * (num->num_cnt - 1);   //total_wid
    }

    if (num->value < 0)     //当显示数据为负数时
    {
        total_len = num->num_wid * num->num_cnt + num->x_margin * num->num_cnt;
        val = abs_s(val);
    }

    if (num->visible)
    {
        //显示数字
        int znum = num->num_cnt - 1;
        int x, y;
        int v = 0;

        if (num->num_layout)
        {
            x = num->x;
            y = num->y + ((total_len - num->num_hei) >> 1);
        }
        else
        {
            x = num->x + ((total_len - num->num_wid) >> 1);
            y = num->y;
        }

        //该对齐方式用于根据 实际 位数去对齐，高位补0操作时不支持
        if (!num->flag_zfill)
        {
            char val_str[MAX_NUMBER_CNT + 1];
            u8 val_len = 0;
            if(num->flag_float)
            {
                val_len = num->decimal_cnt + 1;
                u16 tmp = num->value / soft_pow(10,num->decimal_cnt);
                sprintf(val_str, "%d", tmp);
                val_len +=  strlen(val_str);
            }
            else if (num->flag_zero_replace && num->value == 0)
            {
                val_len = 2;
            }
            else
            {
                sprintf(val_str, "%d", num->value);
                val_len = strlen(val_str);
            }

            if (num->num_layout)
            {
                if (num->num_align == 1)
                {
                    y = num->y + ((num->num_hei + num->x_margin) * (val_len - 1) >> 1);
                }
            }
            else
            {
                if (num->num_align == 1)            //实际位数的中心对齐
                {
                    x = num->x + ((num->num_wid + num->x_margin) * (val_len - 1) >> 1);
                }
                else if (num->num_align == 2)       //左对齐
                {
                    x = num->x + (num->num_wid + num->x_margin) * (val_len - 1);
                }
                else if (num->num_align == 3)       //右对齐
                {
                    x = num->x;
                }
            }
        }

        for (i=znum; i>=0; i--)
        {
            if(num->radix == 14) // 兼容 -./ 开头的切图
            {
                if(num->flag_float && ((num->num_cnt - 1) - num->decimal_cnt == i))
                {
                    widget_image_cut(num->img_num[i], 0, num->num_hei, num->num_wid, num->num_hei);
                }
                else if (num->flag_zero_replace && num->value == 0)
                {
                    widget_image_cut(num->img_num[i], 0, 0, num->num_wid, num->num_hei);
                }
                else
                {
                    v = val % 10;
                    val /= 10;
                    widget_image_cut(num->img_num[i], 0, (v + 3) * num->num_hei, num->num_wid, num->num_hei);
                }
            }
            else
            {
                v = val % num->radix;
                val /= num->radix;
                widget_image_cut(num->img_num[i], 0, v * num->num_hei, num->num_wid, num->num_hei);
            }

            widget_set_pos(num->img_num[i], x, y);
            widget_set_visible(num->img_num[i], true);

            if (num->num_layout)
            {
                y -= num->num_hei + num->x_margin;
            }
            else
            {
                x -= num->num_wid + num->x_margin;
            }

            if (v > 0)
            {
                znum = i;
            }
        }
        num->znum = znum;
        if (!num->flag_zfill)       //隐藏高位
        {
            for (i=0; i<znum; i++)
            {
                uint16_t integer = 0;
                if(num->flag_float && num->decimal_cnt > 0)
                {
                    integer = num->value / soft_pow(10,num->decimal_cnt);
                }
                if (num->radix == 14 && num->flag_float && integer == 0)
                {
                    if(i < (num->num_cnt - 1) - num->decimal_cnt - 1)
                    {
                        widget_set_visible(num->img_num[i], false);
                    }
                    else
                    {
                        widget_set_visible(num->img_num[i], true);
                    }
                }
                else if (num->radix == 14 && num->flag_zero_replace && num->value == 0)
                {
                    if (num->num_cnt > 2)
                    {
                        if (i < num->num_cnt - 2)
                        {
                            widget_set_visible(num->img_num[i], false);
                        }
                        else
                        {
                            widget_set_visible(num->img_num[i], true);
                        }
                    }
                    else
                    {
                        widget_set_visible(num->img_num[i], true);
                    }
                }
                else
                {
                    widget_set_visible(num->img_num[i], false);
                }
            }
            if (num->radix == 14 && num->value < 0 && znum != 0)         //显示符号,只有负号才会显示符号位
            {
                widget_image_cut(num->img_num[i-1], 0, 0, num->num_wid, num->num_hei);
                widget_set_visible(num->img_num[i-1], true);
            }
            else if (num->flag_sym && num->value < 0 && znum != 0)         //显示符号,只有负号才会显示符号位
            {
                widget_image_cut(num->img_num[i-1], 0, 10 * num->num_hei, num->num_wid, num->num_hei);
                widget_set_visible(num->img_num[i-1], true);
            }
        }
        if (num->radix == 14 && num->value < 0 && (znum == 0 || num->flag_zfill))         //显示符号,只有负号才会显示符号位
        {
            widget_image_cut(num->img_sym, 0, 0, num->num_wid, num->num_hei);
            widget_set_pos(num->img_sym, x, y);
            widget_set_visible(num->img_sym, true);
        }
        else if (num->flag_sym && num->value < 0 && (znum == 0 || num->flag_zfill))         //显示符号,只有负号才会显示符号位
        {
            widget_image_cut(num->img_sym, 0, 10 * num->num_hei, num->num_wid, num->num_hei);
            widget_set_pos(num->img_sym, x, y);
            widget_set_visible(num->img_sym, true);
        }
        else
        {
            widget_set_visible(num->img_sym, false);
        }
    }
    else
    {
        //隐藏
        for (i=0; i<num->num_cnt; i++)
        {
            widget_set_visible(num->img_num[i], false);
        }
    }
}

/**
 * @brief 设置数字值
 * @param[in] num : 数字指针
 * @param[in] val : 设置的数值
 **/
void compo_number_set(compo_number_t *num, int val)
{
    if (abs_s(val) > num->max_val)
    {
        halt(HALT_GUI_COMPO_NUMBER_VALUE);
    }

    if (num->value != val)
    {
        num->value = val;
        compo_number_update(num);
    }
}

/**
 * @brief 设置坐标
          注意：该设置默认的坐标是以中心点作为参考点
 * @param[in] num : 数字指针
 * @param[in] x : x轴坐标
 * @param[in] y : y轴坐标
 **/
void compo_number_set_pos(compo_number_t *num, s16 x, s16 y)
{
    num->x = x;
    num->y = y;
    compo_number_update(num);
}

/**
 * @brief 设置是否为小数
 * @param[in] num : 数字指针
 * @param[in] flag_float : 是否为小数
 * @param[in] decimal_cnt : 保留小数位数
 **/
void compo_number_set_float(compo_number_t *num, bool flag_float, u8 decimal_cnt)
{
    num->flag_float = flag_float;
    num->decimal_cnt = decimal_cnt;
    compo_number_update(num);
}

/**
 * @brief 设置是否替换数字0为-
 * @param[in] num : 数字指针
 * @param[in] replace : 是否替换
 **/
void compo_number_set_zero_replace(compo_number_t *num, bool replace)
{
    num->flag_zero_replace = replace;
    compo_number_update(num);
}

/**
 * @brief 设置透明度
 * @param[in] num : 数字指针
 * @param[in] alpha : 透明度
 **/
void compo_number_set_alpha(compo_number_t *num, u8 alpha)
{
    int i;
    for (i=0; i<num->num_cnt; i++)
    {
        widget_set_alpha(num->img_num[i], alpha);
    }
}

/**
 * @brief 设置字间距
 * @param[in] num : 数字指针
 * @param[in] margin : 字间距
 **/
void compo_number_set_margin(compo_number_t *num, int margin)
{
    num->x_margin = margin;
    compo_number_update(num);
}

/**
 * @brief 设置数字高位是否填0
 * @param[in] num : 数字指针
 * @param[in] flag : true  高位补0
                     false 高位不补0
 **/
void compo_number_set_zfill(compo_number_t *num, bool flag)
{
    num->flag_zfill = flag;
    compo_number_update(num);
}

/**
 * @brief 设置数字对齐方式
 * @param[in] num : 数字指针
 * @param[in] num_align : 0  默认按设置位数的中心点对齐
                          1  按实际val位数的中心点对齐
                          2  左对齐
                          3  右对齐
 **/
void compo_number_set_align(compo_number_t *num, u8 num_align)
{
    num->num_align = num_align;
    compo_number_update(num);
}

/**
 * @brief 设置数字框是否可见
 * @param[in] num : 数字指针
 * @param[in] visible : true  可见
                        false 不可见
 **/
void compo_number_set_visible(compo_number_t *num, bool visible)
{
    if (num->visible != visible)
    {
        num->visible = visible;
        compo_number_update(num);
    }
}

/**
 * @brief 获取数字框位置和大小
 * @param[in] num : 数字指针
 * @return 返回数字框位置和大小
 **/
rect_t compo_number_get_location(compo_number_t *num)
{
    rect_t rect = {0};
    rect.x = num->x;
    rect.y = num->y;
    rect.wid = num->num_wid * num->num_cnt + num->x_margin * (num->num_cnt - 1);
    rect.hei = num->num_hei;
    return rect;
}
/**
 * @brief 获取数字框位置和大小
 * @param[in] num : 数字指针
 * @return 返回数字框位置和大小
 **/
rect_t compo_number_get_rel_location(compo_number_t *num)
{
    rect_t rect = {0};
    rect.x = num->x;
    rect.y = num->y;
    if (num->flag_sym && num->value < 0)
    {
        rect.wid = num->num_wid * (num->num_cnt - num->znum + 1) + num->x_margin * (num->num_cnt - num->znum + 1 - 1);
    }
    else
    {
        rect.wid = num->num_wid * (num->num_cnt - num->znum) + num->x_margin * (num->num_cnt - num->znum - 1);
    }
    rect.hei = num->num_hei;
    return rect;
}

/**
 * @brief 设置数字排列方式
 * @param[in] num : 数字指针
 * @param[in] margin : 0  默认横向排列; 1  竖向排列;
 **/
void compo_number_set_layout(compo_number_t *num, u8 margin)
{
    num->num_layout = margin;
    compo_number_update(num);
}
