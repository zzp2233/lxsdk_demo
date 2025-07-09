#include "include.h"
#include "func.h"
#include "ute_module_log.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define SET_LIST_CNT                       ((int)(sizeof(tbl_list_data) / sizeof(tbl_list_data[0])))
#define LIST_ITEM_CNT_MAX                  (sizeof(tbl_list_data) / sizeof(tbl_list_data[0]))

typedef struct f_compo_select_sub_t_
{
    compo_listbox_item_t *p_list_data;
} f_compo_select_sub_t;

enum
{
    COMPO_ID_LISTBOX = 1,
};
#if 0
const compo_listbox_item_t tbl_list_data[] =
{
    {STR_SETTING_CALENDAR,                           .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_SPORT_ON},     //运动
    {STR_CALCULATOR,                            .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_SLEEP_ON},     //睡眠
#if 1//UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {STR_EVREY_DAY_ACTIVITY,               .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_ACTIVITY_ON},  //活动记录
#endif
    {STR_ALARM_CLOCK,                       .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_HEART_ON},     //心率
    {STR_BLOOD_OXYGEN,                     .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_SPO2_ON},      //血氧
    {STR_PHONE,                            .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_BT_CALL_ON},   //电话
    {STR_MUSIC,                            .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_MUSIC_ON},     //音乐
    {STR_WEATHER,                          .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_WEATHER_ON},   //天气

};
const u8 SYS_CTL_ON_TO_FUNC_STA_TABLE[] =
{
#if 1//UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    SYS_CTL_FUNC_ACTIVITY_ON,       FUNC_ECIG_SET_POWER,//FUNC_ACTIVITY,
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    SYS_CTL_FUNC_HEART_ON,          FUNC_ALARM_CLOCK,
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    SYS_CTL_FUNC_SLEEP_ON,          FUNC_CALCULATOR,
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    SYS_CTL_FUNC_SPO2_ON,           FUNC_BLOOD_OXYGEN,
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    SYS_CTL_FUNC_MUSIC_ON,          FUNC_BT,
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    SYS_CTL_FUNC_SPORT_ON,          FUNC_CALENDAER,//FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
    SYS_CTL_FUNC_BT_CALL_ON,        FUNC_CALL_SORT,//FUNC_CALL,
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    SYS_CTL_FUNC_WEATHER_ON,        FUNC_WEATHER,
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT

};

static u8 list_data_sort[LIST_ITEM_CNT_MAX] =
{
#if 1//UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    SYS_CTL_FUNC_ACTIVITY_ON,
#endif
    SYS_CTL_FUNC_HEART_ON,
    SYS_CTL_FUNC_SLEEP_ON,
    SYS_CTL_FUNC_SPO2_ON,
    SYS_CTL_FUNC_MUSIC_ON,
    SYS_CTL_FUNC_SPORT_ON,

    SYS_CTL_FUNC_BT_CALL_ON,
    SYS_CTL_FUNC_WEATHER_ON,
};
#else

const compo_listbox_item_t tbl_list_data[] =
{
    {STR_SETTING_CALENDAR,                 .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_CALENDAR_ON},     //运动
    {STR_CALCULATOR,                       .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_CALCUL_ON},     //睡眠
#if 1//UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {STR_ECIG_SET_POWER,               .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_SET_POWER_ON},  //活动记录
#endif
    {STR_ALARM_CLOCK,                       .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_ALARM_ON},     //心率
    {STR_ECIG_RECORD,                     .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_ECIG_P_ON},      //血氧
    {STR_PHONE,                            .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_BT_CALL_ON},   //电话
    {STR_MUSIC,                            .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_MUSIC_ON},     //音乐
    {STR_WEATHER,                          .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_WEATHER_ON},   //天气

};
const u8 SYS_CTL_ON_TO_FUNC_STA_TABLE[] =
{
    SYS_CTL_FUNC_SET_POWER_ON,      FUNC_ECIG_SET_POWER,//FUNC_ACTIVITY,
    SYS_CTL_FUNC_ALARM_ON,          FUNC_ALARM_CLOCK,
    SYS_CTL_FUNC_CALCUL_ON,         FUNC_CALCULATOR,
    SYS_CTL_FUNC_ECIG_P_ON,         FUNC_ECIG_VPAE_SUB,
    SYS_CTL_FUNC_MUSIC_ON,          FUNC_BT,
    SYS_CTL_FUNC_CALENDAR_ON,       FUNC_CALENDAER_SUB,//FUNC_SPORT,
    SYS_CTL_FUNC_BT_CALL_ON,        FUNC_CALL_SORT,//FUNC_CALL,
    SYS_CTL_FUNC_WEATHER_ON,        FUNC_WEATHER,

};

static u8 list_data_sort[LIST_ITEM_CNT_MAX] =
{

    SYS_CTL_FUNC_SET_POWER_ON,
    SYS_CTL_FUNC_ALARM_ON,
    SYS_CTL_FUNC_CALCUL_ON,
    SYS_CTL_FUNC_ECIG_P_ON,
    SYS_CTL_FUNC_MUSIC_ON,
    SYS_CTL_FUNC_CALENDAR_ON,

    SYS_CTL_FUNC_BT_CALL_ON,
    SYS_CTL_FUNC_WEATHER_ON,
};


#endif

//根据vidx信息获取list_data
static const compo_listbox_item_t *get_tbl_list_data_by_vidx(u16 vidx)
{
    for (u8 i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (vidx == tbl_list_data[i].vidx)
        {
            return (compo_listbox_item_t *)&tbl_list_data[i];
        }
    }

    return NULL;
}

//通过界面id获取SYS_CTL_ON_TO_FUNC_STA_TABLE中对应的控制位
static s16 get_sys_ctl_by_func(u8 func)
{
    for (size_t i = 0; i < ARRAY_SIZE(SYS_CTL_ON_TO_FUNC_STA_TABLE); i += 2)
    {
        if (SYS_CTL_ON_TO_FUNC_STA_TABLE[i + 1] == func)
        {
            return SYS_CTL_ON_TO_FUNC_STA_TABLE[i];
        }
    }
    return -1; // 返回 -1 表示未找到
}

// 查询界面id的位置
static s16 get_func_index(int func)
{
    for (size_t i = 0; i < ARRAY_SIZE(SYS_CTL_ON_TO_FUNC_STA_TABLE); i += 2)
    {
        if (SYS_CTL_ON_TO_FUNC_STA_TABLE[i + 1] == func)
        {
            return i + 1; // 返回 FUNC 值的位置
        }
    }
    return -1; // 返回 -1 表示未找到
}

//根据实际界面重新排序list_data_sort
void reorder_list_data_sort(void)
{
    u8 *tmp_list_data_sort = ab_zalloc(LIST_ITEM_CNT_MAX);
    u8 tmp_sort_cnt = 0;
    // 查询当前界面排序对应的控制位
    for (uint8_t i = 0; i < MAX_FUNC_SORT_CNT; i++)
    {
        if (tmp_sort_cnt >= LIST_ITEM_CNT_MAX)
        {
            break;
        }
        if (func_cb.tbl_sort[i])
        {
            s16 index = get_sys_ctl_by_func(func_cb.tbl_sort[i]);
            TRACE("index:%d\n", index);
            if (index >= 0)
            {
                tmp_list_data_sort[tmp_sort_cnt] = index;
                tmp_sort_cnt += 1;
                // 根据实际界面打开控制位
                u16 func_index = get_func_index(func_cb.tbl_sort[i]);
                if (func_index > 0)
                {
                    bsp_sys_set_ctlbit(SYS_CTL_ON_TO_FUNC_STA_TABLE[func_index - 1], true);
                }
            }
        }
        else
        {
            break;
        }
    }
    // 重新排序list_data_sort
    if (tmp_sort_cnt == 0)
    {
        return;
    }
    else if (tmp_sort_cnt == LIST_ITEM_CNT_MAX)
    {
        memcpy(list_data_sort, tmp_list_data_sort, LIST_ITEM_CNT_MAX);
    }
    else
    {
        u8 *result_array = ab_zalloc(LIST_ITEM_CNT_MAX);
        u8 *used_flags = ab_zalloc(LIST_ITEM_CNT_MAX);
        u8 result_index = 0;
        for (u8 i = 0; i < tmp_sort_cnt; i++)
        {
            for (u8 j = 0; j < LIST_ITEM_CNT_MAX; j++)
            {
                if (!used_flags[j] && list_data_sort[j] == tmp_list_data_sort[i])
                {
                    result_array[result_index++] = list_data_sort[j];
                    used_flags[j] = 1; // 标记为已使用
                    break;
                }
            }
        }
        for (u8 j = 0; j < LIST_ITEM_CNT_MAX; j++)
        {
            if (!used_flags[j])
            {
                result_array[result_index++] = list_data_sort[j];
            }
        }
        memcpy(list_data_sort, result_array, LIST_ITEM_CNT_MAX);
        ab_free(result_array);
        ab_free(used_flags);
    }
    ab_free(tmp_list_data_sort);
}

//根据排序表更新list_data
static void func_compo_list_data_update(void)
{
    f_compo_select_sub_t *f_compo_select_sub = (f_compo_select_sub_t *)func_cb.f_cb;
    compo_listbox_item_t *p_list_data = f_compo_select_sub->p_list_data;
    int i;

    for (i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (list_data_sort[i] != p_list_data[i].vidx)
        {
            memcpy(&p_list_data[i], get_tbl_list_data_by_vidx(list_data_sort[i]), sizeof(compo_listbox_item_t));
        }
    }
}

//获取已添加数量
static u8 list_data_sort_get_add_cnt(void)
{
    int i;
    u8 cnt = 0;

    for (i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (bsp_sys_get_ctlbit(list_data_sort[i]))
        {
            cnt ++;
            printf("ctlbit [%d,%d]\n", bsp_sys_get_ctlbit(list_data_sort[i]), cnt);
        }
        else
        {
            break;
        }
    }

    return cnt;
}

//添加
static void list_data_sort_add(u8 vidx)
{
    u8 list_data_sort_tmp[LIST_ITEM_CNT_MAX] = {0};
    u8 i = 0, index = 0;
    u8 add_cnt = list_data_sort_get_add_cnt();

    if (add_cnt)
    {
        memcpy(list_data_sort_tmp, list_data_sort, add_cnt * sizeof(list_data_sort_tmp[0]));
    }
    list_data_sort_tmp[add_cnt] = vidx;
    index = add_cnt + 1;
    for (i = add_cnt; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (vidx != list_data_sort[i])
        {
            list_data_sort_tmp[index ++] = list_data_sort[i];
        }
    }

    memcpy(list_data_sort, list_data_sort_tmp, sizeof(list_data_sort));
    func_compo_list_data_update();
}

//删除
static void list_data_sort_del(u8 vidx)
{
    printf("%s\n", __func__);
    u8 list_data_sort_tmp[LIST_ITEM_CNT_MAX] = {0};
    u8 i = 0, index = LIST_ITEM_CNT_MAX - 1;

    list_data_sort_tmp[index --] = vidx;
    for (i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (vidx != list_data_sort[LIST_ITEM_CNT_MAX - 1 - i])
        {
            list_data_sort_tmp[index --] = list_data_sort[LIST_ITEM_CNT_MAX - 1 - i];
        }
    }

    memcpy(list_data_sort, list_data_sort_tmp, sizeof(list_data_sort));
    func_compo_list_data_update();
}

//向上
static void list_data_sort_up(u8 vidx)
{
    printf("%s\n", __func__);
    u8 pos = 0;
    u8 add_cnt = list_data_sort_get_add_cnt();

    if (add_cnt < 2)
    {
        return ;
    }

    for (u8 i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (list_data_sort[i] == vidx)
        {
            if (i == 0)
            {
                pos = add_cnt - 1;
            }
            else
            {
                pos = i - 1;
            }
            u8 vidx_tmp = list_data_sort[pos];
            list_data_sort[pos] = vidx;
            list_data_sort[i] = vidx_tmp;
            break;
        }
    }

    func_compo_list_data_update();
}


//创建组件选择窗体
compo_form_t *func_compo_select_sub_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_COMPO_SELECT]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    if (LIST_ITEM_CNT_MAX > (MAX_FUNC_SORT_CNT - 2) || LIST_ITEM_CNT_MAX != sizeof(SYS_CTL_ON_TO_FUNC_STA_TABLE) / 2)
    {
        printf("%s:%d,err\n", __func__, __LINE__);
        halt(HALT_GUI_COMPO_LISTBOX_CREATE);
    }

    printf("tbl_sort [");
    for(int i=0; i<sizeof(func_cb.tbl_sort)/sizeof(func_cb.tbl_sort[0]);  i++)
    {
        printf("%d ", func_cb.tbl_sort[i]);
    }
    printf("]\n");

    compo_listbox_set_bithook(listbox, bsp_sys_get_ctlbit);

    return frm;
}

//单击按钮
static void func_compo_select_sub_click(void)
{
    compo_listbox_t *listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    f_compo_select_sub_t *f_compo_select_sub = (f_compo_select_sub_t *)func_cb.f_cb;

    bool del = true;
    point_t cur_point = ctp_get_sxy();

    int idx = compo_listbox_select(listbox, cur_point);
    if (idx < 0)
    {
        return ;
    }
    u8 vidx = f_compo_select_sub->p_list_data[idx].vidx;

    bool bit_sta = bsp_sys_get_ctlbit(vidx);
    if (!bit_sta)
    {
        list_data_sort_add(vidx);
        bsp_sys_reverse_ctlbit(vidx);
    }
    else
    {
        if (del)
        {
            list_data_sort_del(vidx);
            bsp_sys_reverse_ctlbit(vidx);
        }
        else
        {
            list_data_sort_up(vidx);
        }
    }

    compo_listbox_update_with_text_scroll_rst(listbox);
}
static void func_compo_select_sub_init(void)
{
    //根据实际显示界面重新排序
    reorder_list_data_sort();

    f_compo_select_sub_t *f_compo_select_sub = (f_compo_select_sub_t *)func_cb.f_cb;
    f_compo_select_sub->p_list_data = func_zalloc(LIST_ITEM_CNT_MAX * sizeof(compo_listbox_item_t));
    if (NULL == f_compo_select_sub->p_list_data)
    {
        printf("%s:%d,err\n", __func__, __LINE__);
        halt(HALT_GUI_COMPO_LISTBOX_CREATE);
    }
    func_compo_list_data_update();

    compo_listbox_t *listbox = compo_getobj_byid(COMPO_ID_LISTBOX);

    compo_listbox_set(listbox, f_compo_select_sub->p_list_data, LIST_ITEM_CNT_MAX);
    u8 menu_idx = func_cb.menu_idx;
    if (menu_idx < 1)
    {
        menu_idx = 1;
    }
    compo_listbox_set_focus_byidx(listbox, menu_idx);
    compo_listbox_set_sta_icon(listbox,UI_BUF_I330001_FIRSTORDER_DELETE_BIN, UI_BUF_I330001_FIRSTORDER_ADD_BIN);
    compo_listbox_update(listbox);

    compo_listbox_move(listbox);
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2));
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define SET_LIST_CNT                       ((int)(sizeof(tbl_list_data) / sizeof(tbl_list_data[0])))
#define LIST_ITEM_CNT_MAX                  (sizeof(tbl_list_data) / sizeof(tbl_list_data[0]))

typedef struct f_compo_select_sub_t_
{
    compo_listbox_item_t *p_list_data;
} f_compo_select_sub_t;

enum
{
    COMPO_ID_LISTBOX = 1,
};
const compo_listbox_item_t tbl_list_data[] =
{
    {STR_SPORTS,                           .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_SPORT_ON},     //运动
    {STR_SLEEP,                            .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_SLEEP_ON},     //睡眠
    {STR_EVREY_DAY_ACTIVITY,               .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_ACTIVITY_ON},  //活动记录
    {STR_HEART_RATE,                       .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_HEART_ON},     //心率
    {STR_BLOOD_OXYGEN,                     .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_SPO2_ON},      //血氧
    {STR_PHONE,                            .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_BT_CALL_ON},   //电话
    {STR_MUSIC,                            .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_MUSIC_ON},     //音乐
    {STR_WEATHER,                          .item_mode = COMPO_LISTBOX_ITEM_MODE_SWITCH, .vidx = SYS_CTL_FUNC_WEATHER_ON},   //天气

};
const u8 SYS_CTL_ON_TO_FUNC_STA_TABLE[] =
{
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    SYS_CTL_FUNC_ACTIVITY_ON,       FUNC_ACTIVITY,
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    SYS_CTL_FUNC_HEART_ON,          FUNC_HEARTRATE,
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    SYS_CTL_FUNC_SLEEP_ON,          FUNC_SLEEP,
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    SYS_CTL_FUNC_SPO2_ON,           FUNC_BLOOD_OXYGEN,
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    SYS_CTL_FUNC_MUSIC_ON,          FUNC_BT,
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    SYS_CTL_FUNC_SPORT_ON,          FUNC_SPORT_SORT,//FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
    SYS_CTL_FUNC_BT_CALL_ON,        FUNC_CALL_SORT,//FUNC_CALL,
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    SYS_CTL_FUNC_WEATHER_ON,        FUNC_WEATHER,
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT

};

static u8 list_data_sort[LIST_ITEM_CNT_MAX] =
{
    SYS_CTL_FUNC_ACTIVITY_ON,
    SYS_CTL_FUNC_HEART_ON,
    SYS_CTL_FUNC_SLEEP_ON,
    SYS_CTL_FUNC_SPO2_ON,
    SYS_CTL_FUNC_MUSIC_ON,
    SYS_CTL_FUNC_SPORT_ON,

    SYS_CTL_FUNC_BT_CALL_ON,
    SYS_CTL_FUNC_WEATHER_ON,
};

//根据vidx信息获取list_data
static const compo_listbox_item_t *get_tbl_list_data_by_vidx(u16 vidx)
{
    for (u8 i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (vidx == tbl_list_data[i].vidx)
        {
            return (compo_listbox_item_t *)&tbl_list_data[i];
        }
    }

    return NULL;
}

//通过界面id获取SYS_CTL_ON_TO_FUNC_STA_TABLE中对应的控制位
static s16 get_sys_ctl_by_func(u8 func)
{
    for (size_t i = 0; i < ARRAY_SIZE(SYS_CTL_ON_TO_FUNC_STA_TABLE); i += 2)
    {
        if (SYS_CTL_ON_TO_FUNC_STA_TABLE[i + 1] == func)
        {
            return SYS_CTL_ON_TO_FUNC_STA_TABLE[i];
        }
    }
    return -1; // 返回 -1 表示未找到
}

// 查询界面id的位置
static s16 get_func_index(int func)
{
    for (size_t i = 0; i < ARRAY_SIZE(SYS_CTL_ON_TO_FUNC_STA_TABLE); i += 2)
    {
        if (SYS_CTL_ON_TO_FUNC_STA_TABLE[i + 1] == func)
        {
            return i + 1; // 返回 FUNC 值的位置
        }
    }
    return -1; // 返回 -1 表示未找到
}

//根据实际界面重新排序list_data_sort
void reorder_list_data_sort(void)
{
    u8 *tmp_list_data_sort = ab_zalloc(LIST_ITEM_CNT_MAX);
    u8 tmp_sort_cnt = 0;
    // 查询当前界面排序对应的控制位
    for (uint8_t i = 0; i < MAX_FUNC_SORT_CNT; i++)
    {
        if (tmp_sort_cnt >= LIST_ITEM_CNT_MAX)
        {
            break;
        }
        if (func_cb.tbl_sort[i])
        {
            s16 index = get_sys_ctl_by_func(func_cb.tbl_sort[i]);
            TRACE("index:%d\n", index);
            if (index >= 0)
            {
                tmp_list_data_sort[tmp_sort_cnt] = index;
                tmp_sort_cnt += 1;
                // 根据实际界面打开控制位
                u16 func_index = get_func_index(func_cb.tbl_sort[i]);
                if (func_index > 0)
                {
                    bsp_sys_set_ctlbit(SYS_CTL_ON_TO_FUNC_STA_TABLE[func_index - 1], true);
                }
            }
        }
        else
        {
            break;
        }
    }
    // 重新排序list_data_sort
    if (tmp_sort_cnt == 0)
    {
        return;
    }
    else if (tmp_sort_cnt == LIST_ITEM_CNT_MAX)
    {
        memcpy(list_data_sort, tmp_list_data_sort, LIST_ITEM_CNT_MAX);
    }
    else
    {
        u8 *result_array = ab_zalloc(LIST_ITEM_CNT_MAX);
        u8 *used_flags = ab_zalloc(LIST_ITEM_CNT_MAX);
        u8 result_index = 0;
        for (u8 i = 0; i < tmp_sort_cnt; i++)
        {
            for (u8 j = 0; j < LIST_ITEM_CNT_MAX; j++)
            {
                if (!used_flags[j] && list_data_sort[j] == tmp_list_data_sort[i])
                {
                    result_array[result_index++] = list_data_sort[j];
                    used_flags[j] = 1; // 标记为已使用
                    break;
                }
            }
        }
        for (u8 j = 0; j < LIST_ITEM_CNT_MAX; j++)
        {
            if (!used_flags[j])
            {
                result_array[result_index++] = list_data_sort[j];
            }
        }
        memcpy(list_data_sort, result_array, LIST_ITEM_CNT_MAX);
        ab_free(result_array);
        ab_free(used_flags);
    }
    ab_free(tmp_list_data_sort);
}

//根据排序表更新list_data
static void func_compo_list_data_update(void)
{
    f_compo_select_sub_t *f_compo_select_sub = (f_compo_select_sub_t *)func_cb.f_cb;
    compo_listbox_item_t *p_list_data = f_compo_select_sub->p_list_data;
    int i;

    for (i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (list_data_sort[i] != p_list_data[i].vidx)
        {
            memcpy(&p_list_data[i], get_tbl_list_data_by_vidx(list_data_sort[i]), sizeof(compo_listbox_item_t));
        }
    }
}

//获取已添加数量
static u8 list_data_sort_get_add_cnt(void)
{
    int i;
    u8 cnt = 0;

    for (i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (bsp_sys_get_ctlbit(list_data_sort[i]))
        {
            cnt ++;
            printf("ctlbit [%d,%d]\n", bsp_sys_get_ctlbit(list_data_sort[i]), cnt);
        }
        else
        {
            break;
        }
    }

    return cnt;
}

//添加
static void list_data_sort_add(u8 vidx)
{
    u8 list_data_sort_tmp[LIST_ITEM_CNT_MAX] = {0};
    u8 i = 0, index = 0;
    u8 add_cnt = list_data_sort_get_add_cnt();

    if (add_cnt)
    {
        memcpy(list_data_sort_tmp, list_data_sort, add_cnt * sizeof(list_data_sort_tmp[0]));
    }
    list_data_sort_tmp[add_cnt] = vidx;
    index = add_cnt + 1;
    for (i = add_cnt; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (vidx != list_data_sort[i])
        {
            list_data_sort_tmp[index ++] = list_data_sort[i];
        }
    }

    memcpy(list_data_sort, list_data_sort_tmp, sizeof(list_data_sort));
    func_compo_list_data_update();
}

//删除
static void list_data_sort_del(u8 vidx)
{
    printf("%s\n", __func__);
    u8 list_data_sort_tmp[LIST_ITEM_CNT_MAX] = {0};
    u8 i = 0, index = LIST_ITEM_CNT_MAX - 1;

    list_data_sort_tmp[index --] = vidx;
    for (i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (vidx != list_data_sort[LIST_ITEM_CNT_MAX - 1 - i])
        {
            list_data_sort_tmp[index --] = list_data_sort[LIST_ITEM_CNT_MAX - 1 - i];
        }
    }

    memcpy(list_data_sort, list_data_sort_tmp, sizeof(list_data_sort));
    func_compo_list_data_update();
}

//向上
static void list_data_sort_up(u8 vidx)
{
    printf("%s\n", __func__);
    u8 pos = 0;
    u8 add_cnt = list_data_sort_get_add_cnt();

    if (add_cnt < 2)
    {
        return ;
    }

    for (u8 i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (list_data_sort[i] == vidx)
        {
            if (i == 0)
            {
                pos = add_cnt - 1;
            }
            else
            {
                pos = i - 1;
            }
            u8 vidx_tmp = list_data_sort[pos];
            list_data_sort[pos] = vidx;
            list_data_sort[i] = vidx_tmp;
            break;
        }
    }

    func_compo_list_data_update();
}


//创建组件选择窗体
compo_form_t *func_compo_select_sub_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_COMPO_SELECT]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    if (LIST_ITEM_CNT_MAX > (MAX_FUNC_SORT_CNT - 2) || LIST_ITEM_CNT_MAX != sizeof(SYS_CTL_ON_TO_FUNC_STA_TABLE) / 2)
    {
        printf("%s:%d,err\n", __func__, __LINE__);
        halt(HALT_GUI_COMPO_LISTBOX_CREATE);
    }

    printf("tbl_sort [");
    for(int i=0; i<sizeof(func_cb.tbl_sort)/sizeof(func_cb.tbl_sort[0]);  i++)
    {
        printf("%d ", func_cb.tbl_sort[i]);
    }
    printf("]\n");

    compo_listbox_set_bithook(listbox, bsp_sys_get_ctlbit);

    return frm;
}

//单击按钮
static void func_compo_select_sub_click(void)
{
    compo_listbox_t *listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    f_compo_select_sub_t *f_compo_select_sub = (f_compo_select_sub_t *)func_cb.f_cb;

    bool del = true;
    point_t cur_point = ctp_get_sxy();

    int idx = compo_listbox_select(listbox, cur_point);
    if (idx < 0)
    {
        return ;
    }
    u8 vidx = f_compo_select_sub->p_list_data[idx].vidx;

    bool bit_sta = bsp_sys_get_ctlbit(vidx);
    if (!bit_sta)
    {
        list_data_sort_add(vidx);
        bsp_sys_reverse_ctlbit(vidx);
    }
    else
    {
        if (del)
        {
            list_data_sort_del(vidx);
            bsp_sys_reverse_ctlbit(vidx);
        }
        else
        {
            list_data_sort_up(vidx);
        }
    }

    compo_listbox_update_with_text_scroll_rst(listbox);
}
static void func_compo_select_sub_init(void)
{
    //根据实际显示界面重新排序
    reorder_list_data_sort();

    f_compo_select_sub_t *f_compo_select_sub = (f_compo_select_sub_t *)func_cb.f_cb;
    f_compo_select_sub->p_list_data = func_zalloc(LIST_ITEM_CNT_MAX * sizeof(compo_listbox_item_t));
    if (NULL == f_compo_select_sub->p_list_data)
    {
        printf("%s:%d,err\n", __func__, __LINE__);
        halt(HALT_GUI_COMPO_LISTBOX_CREATE);
    }
    func_compo_list_data_update();

    compo_listbox_t *listbox = compo_getobj_byid(COMPO_ID_LISTBOX);

    compo_listbox_set(listbox, f_compo_select_sub->p_list_data, LIST_ITEM_CNT_MAX);
    u8 menu_idx = func_cb.menu_idx;
    if (menu_idx < 1)
    {
        menu_idx = 1;
    }
    compo_listbox_set_focus_byidx(listbox, menu_idx);
    compo_listbox_set_sta_icon(listbox,UI_BUF_I332001_FIRSTORDER_DELETE_BIN, UI_BUF_I332001_FIRSTORDER_ADD_BIN);
    compo_listbox_update(listbox);

    compo_listbox_move(listbox);
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2));
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//组件选择功能事件处理
static void func_compo_select_sub_process(void)
{
    compo_listbox_t *listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_move(listbox);
    func_process();
}


//组件选择功能消息处理
static void func_compo_select_sub_message(size_msg_t msg)
{
    compo_listbox_t *listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_compo_select_sub_click();
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}


//进入组件选择功能
static void func_compo_select_sub_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_compo_select_sub_t));
    func_cb.frm_main = func_compo_select_sub_form_create();
    func_compo_select_sub_init();
}

//退出组件选择功能
static void func_compo_select_sub_exit(void)
{
//    printf("%s\n", __func__);
    func_cb.last = FUNC_COMPO_SELECT_SUB;

    f_compo_select_sub_t *f_compo_select_sub = (f_compo_select_sub_t *)func_cb.f_cb;
    func_free(f_compo_select_sub->p_list_data);

    compo_listbox_t *listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    func_free(listbox->mcb);

    u8 index = 1;
    memset(&func_cb.tbl_sort[index], 0, sizeof(func_cb.tbl_sort)/sizeof(func_cb.tbl_sort[0]) - index);
    printf("exit add tbl_sort = [");
    for (u8 i = 0; i < LIST_ITEM_CNT_MAX; i++)
    {
        if (bsp_sys_get_ctlbit(list_data_sort[i]))
        {
            for (u8 j = 0; j < sizeof(SYS_CTL_ON_TO_FUNC_STA_TABLE) / 2; j++)
            {
                if (SYS_CTL_ON_TO_FUNC_STA_TABLE[2 * j] == list_data_sort[i])
                {
                    func_cb.tbl_sort[index ++] = SYS_CTL_ON_TO_FUNC_STA_TABLE[2 * j + 1];
                    printf("%d ", func_cb.tbl_sort[index-1]);
                    break;
                }
            }
        }
    }
    printf("]\n");
    func_cb.tbl_sort[index ++] = FUNC_COMPO_SELECT;
    func_cb.sort_cnt = index;
    if (sys_cb.refresh_language_flag == false && func_cb.sta != FUNC_CLOCK)
    {
        func_cb.flag_sort = true;
    }

//    printf("EXIT tbl_sort [");
//    for(int i=0; i<sizeof(func_cb.tbl_sort)/sizeof(func_cb.tbl_sort[0]);  i++) {
//        printf("%d ", func_cb.tbl_sort[i]);
//    }
//    printf("]\n");

    printf("%s->index[%d], sort[%d]\n", __func__, index, func_cb.flag_sort);

    // 保存排序
    uteModuleGuiCommonSavescreenTblSort(func_cb.tbl_sort, func_cb.sort_cnt);
}

//组件选择功能
void func_compo_select_sub(void)
{
    printf("%s\n", __func__);
    func_compo_select_sub_enter();
    while (func_cb.sta == FUNC_COMPO_SELECT_SUB)
    {
        func_compo_select_sub_process();
        func_compo_select_sub_message(msg_dequeue());
    }
    func_compo_select_sub_exit();
}
