#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "api_gui.h"
#include "../../../../ute/module/smoke/ute_module_smoke.h"
#if UTE_MODULE_SCREENS_CALENDAER_SUB_SUPPORT





#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT



//组件ID
enum
{
    COMPO_ID_BTN_CALENDAR_ENTER = 1,
    COMPO_ID_CHART_DAILY,  // 每天口数图表
    COMPO_ID_CHART_WEEKLY, // 本周口数图表
    COMPO_ID_SCROLLBAR,
};
uint16_t ecig_date[24];

typedef struct f_ecig_t_
{
    page_tp_move_t *ptm;
    uint32_t tick;
    u8 last_hr;
    u8 max_hr;
    u8 min_hr;
    bool heart_pic_state;
    u8 heart_pic_size;
} f_ecig_t;


compo_form_t *func_ecig_vpae_sub_form_create(void)
{
    char txt_buf[50];

    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ECIG_RECORD]);

    ///设置图片
    // compo_form_add_image(frm,UI_BUF_I330001_VAPE_RECORD_GROUP_808_BIN,120,196);
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_VAPE_RECORD_GROUP_808_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 196);




    //创建图表
    compo_chartbox_t *chart_daily = compo_chartbox_create(frm, 1, 24);///图表内的柱形图
    compo_chartbox_set_location(chart_daily, GUI_SCREEN_CENTER_X+13, 196,228,105);
    compo_chartbox_set_pixel(chart_daily, 1);
    compo_setid(chart_daily, COMPO_ID_CHART_DAILY);

    chart_t chart_info_daily;
    chart_info_daily.y = 8;
    chart_info_daily.width = 6;

    // 在设置图表值的循环中添加范围检查
    for (int i = 0; i < 24; i++)
    {
        int base_x = i * chart_info_daily.width + 2 * i;
        if (i > 17)
        {
            chart_info_daily.x = base_x + 12;
        }
        else if (i > 11)
        {
            chart_info_daily.x = base_x + 8;
        }
        else if (i > 5)
        {
            chart_info_daily.x = base_x + 4;
        }
        else
        {
            chart_info_daily.x = base_x;
        }

        // 增加最大限制，防止图表高度超出显示范围
        uint16_t max_height = 100; // 根据实际图表高度调整
        chart_info_daily.height = (ecig_date[i] * 0.73 > max_height) ? max_height : (ecig_date[i] * 0.73);
        compo_chartbox_set_value(chart_daily, i, chart_info_daily, make_color(255, 116, 169));
    }
    ///设置图片

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_VAPE_RECORD_P_TODAY_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 73);

    //当天口数  文字
    memset(txt_buf,0,sizeof(txt_buf));
    sprintf(txt_buf, "%s", i18n[STR_ECIG_P]);
    compo_textbox_t *textbox_p = compo_textbox_create(frm,strlen(i18n[STR_ECIG_P]));
    compo_textbox_set_font(textbox_p, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(textbox_p, false);
    compo_textbox_set_location(textbox_p,80, 62,64,36);
    compo_textbox_set(textbox_p,txt_buf);

    // 获取当天口数总计
    uint32_t daily_total = get_today_smoking_count();

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",daily_total);
    compo_textbox_t *textbox_num = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(textbox_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_align_center(textbox_num, false);
    compo_textbox_set_location(textbox_num,130+20, 63,44,36);
    compo_textbox_set(textbox_num,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    sprintf(txt_buf, "%s", i18n[STR_ECIG_TODAT_P]);
    compo_textbox_t *textbox_today = compo_textbox_create(frm,strlen(i18n[STR_ECIG_TODAT_P]));
    compo_textbox_set_font(textbox_today, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(textbox_today, false);
    compo_textbox_set_location(textbox_today,72, 105,120,36);
    compo_textbox_set(textbox_today,txt_buf);
    compo_textbox_set_forecolor(textbox_today,make_color(255,116,169));

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_VAPE_RECORD_T_TODAYS_VAPE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+33);

    //本周口数  文字
    memset(txt_buf,0,sizeof(txt_buf));
    sprintf(txt_buf, "%s", i18n[STR_ECIG_P]);
    compo_textbox_t *textbox2_p = compo_textbox_create(frm,strlen(i18n[STR_ECIG_P]));
    compo_textbox_set_font(textbox2_p, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(textbox2_p, false);
    compo_textbox_set_location(textbox2_p,80, GUI_SCREEN_HEIGHT+22,64,36);
    compo_textbox_set(textbox2_p,txt_buf);

    // 获取本周口数总计
    uint32_t weekly_total = get_this_week_smoking_count();

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d", weekly_total);
    compo_textbox_t *textbox2_num = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(textbox2_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_align_center(textbox2_num, false);
    compo_textbox_set_location(textbox2_num,130+20, GUI_SCREEN_HEIGHT+23,44,36);
    compo_textbox_set(textbox2_num,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    sprintf(txt_buf, "%s", i18n[STR_ECIG_7_DAY]);
    compo_textbox_t *textbox_15today = compo_textbox_create(frm,strlen(i18n[STR_ECIG_7_DAY]));
    compo_textbox_set_font(textbox_15today, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(textbox_15today, false);
    compo_textbox_set_location(textbox_15today,72, GUI_SCREEN_HEIGHT+65,120,36);
    compo_textbox_set(textbox_15today,txt_buf);
    compo_textbox_set_forecolor(textbox_15today,make_color(0,242,214));


    picbox = compo_picturebox_create(frm, UI_BUF_I330001_VAPE_RECORD_BG_WEEK_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+156);

// 创建本周口数图表
    compo_chartbox_t *chart_weekly = compo_chartbox_create(frm, 1, 7);
    compo_chartbox_set_location(chart_weekly, GUI_SCREEN_CENTER_X + 13, GUI_SCREEN_HEIGHT + 156, 228, 105);
    compo_chartbox_set_pixel(chart_weekly, 1);
    compo_setid(chart_weekly, COMPO_ID_CHART_WEEKLY);

    chart_t chart_info_weekly;
    chart_info_weekly.y = 8;
    chart_info_weekly.width = 6;

    for (int i = 0; i < 7; i++)
    {
        int base_x = i * (chart_info_weekly.width + 26);
        chart_info_weekly.x = base_x +2;
        chart_info_weekly.height = uteModuleSmokeData.smoking_count_per_day[i] * 0.73;
        compo_chartbox_set_value(chart_weekly, i, chart_info_weekly, make_color(0, 242, 214));
    }
    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
void update_chart_with_current_time()
{
    uint32_t smoking_counts[24];
    uteModuleGetSmokingCountPerHour(smoking_counts);

    // 获取当前小时
    ute_module_systemtime_time_t current_time;
    uteModuleSystemtimeGetTime(&current_time);
    uint8_t current_hour = current_time.hour;

    // 更新ecig_date数组，确保时间顺序正确
    for (int i = 0; i < 24; i++)
    {
        int index = (current_hour - i + 24) % 24; // 按当前时间倒序排列
        ecig_date[i] = smoking_counts[index];
    }
}


static void func_ecig_vpae_sub_date_update(bool next)
{

}

//单击按钮
static void func_ecig_vpae_sub_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_CALENDAR_ENTER:
            //  func_cb.flag_sort = false;
            //  func_cb.sta = FUNC_CALENDAER;
            break;
        default:
            break;
    }


}

//公共事件处理
static void func_ecig_vpae_sub_comm_process(void)
{
    f_ecig_t *f_ecig = (f_ecig_t *)func_cb.f_cb;
    compo_page_move_process(f_ecig->ptm);
    func_process();
    check_and_update_day();
    check_and_update_week();
    update_chart_with_current_time();
}

//消息处理
static void func_ecig_vpae_sub_message(size_msg_t msg)
{
    f_ecig_t *f_ecig = (f_ecig_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_ecig->ptm);
            break;

        default:
            func_message(msg);
            break;
    }
}


static void func_ecig_vpae_sub_enter(void)
{
    update_chart_with_current_time();
    uint32_t smoking_counts[24];
    uteModuleGetSmokingCountPerHour(smoking_counts);
    for (int i=0; i<24; i++)ecig_date[i]=smoking_counts[i];
    func_cb.f_cb = func_zalloc(sizeof(f_ecig_t));
    func_cb.frm_main = func_ecig_vpae_sub_form_create();
    f_ecig_t *f_ecig = (f_ecig_t *)func_cb.f_cb;
    f_ecig->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = GUI_SCREEN_HEIGHT - (GUI_SCREEN_HEIGHT / 8),
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = 100,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_ecig->ptm, func_cb.frm_main->page_body, &info);
    func_cb.flag_animation = false;
    check_and_update_day();
    check_and_update_week();
    //刷新文本内容
    // func_calender_sub_refresh(time.year, time.month, time.day);
}


static void func_ecig_vpae_sub_exit(void)
{
    f_ecig_t *f_ecig = (f_ecig_t *)func_cb.f_cb;
    if (f_ecig->ptm)
    {
        func_free(f_ecig->ptm);
    }
    // func_cb.last = FUNC_ECIG_VPAE_SUB;
}


void func_ecig_vpae_sub(void)
{
    printf("%s\n", __func__);
    func_ecig_vpae_sub_enter();
    while (func_cb.sta == FUNC_ECIG_VPAE_SUB)
    {
        func_ecig_vpae_sub_comm_process();
        func_ecig_vpae_sub_message(msg_dequeue());
    }
    func_ecig_vpae_sub_exit();
}


#endif
