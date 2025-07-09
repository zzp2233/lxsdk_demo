#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"

#if UTE_MODULE_SCREENS_CALENDAER_SUB_SUPPORT

typedef struct f_calendar_t_
{
    u16 today_year;
    u8 today_mon;
    u8 today_day;
    u16 update_year;
    u8 update_mon;
} f_calendar_t;

// enum
// {
//     CALE_SUNDAY = 0,
//     CALE_MONDAY,
//     CALE_TUESDAY,
//     CALE_WEDNESDAY,
//     CALE_THURSDAY,
//     CALE_FRIDAY,
//     CALE_SATURDAY,
// } e_cale_week;



#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

const uint16_t Calendar_Week_ID[]=
{
    STR_SUNDAY, // 周日
    STR_MONDAY, // 周一
    STR_TUESDAY, // 周二
    STR_WEDNESDAY, // 周三
    STR_THURSDAY, // 周四
    STR_FRIDAY, // 周五
    STR_SATURDAY, // 周六
};

//组件ID
enum
{
    COMPO_ID_BTN_CALENDAR_ENTER = 1,
};
//创建日历主界面
compo_form_t *func_calender_sub_form_create(void)
{
    char txt_buf[30];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_CALENDAR]);

    ///设置图片
    compo_form_add_image(frm,UI_BUF_I330001_CALENDAR_BG_BIN,120,131);


    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if (time.hour >= 12)      //2 PM, 1 AM
    {
        sprintf(txt_buf, "%s", i18n[STR_PM]);
    }
    else
    {
        sprintf(txt_buf, "%s", i18n[STR_AM]);
    }
    //2 PM, 1 AM
    compo_textbox_t *textbox = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,60, 80,64,36);
    compo_textbox_set(textbox,txt_buf);


    //WeeK
    memset(txt_buf,0,sizeof(txt_buf));
    sprintf(txt_buf, "%s", i18n[Calendar_Week_ID[time.week]]);
    compo_textbox_t *text_week_box = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(text_week_box, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(text_week_box, false);
    compo_textbox_set_location(text_week_box,147, 80,90,36);
    compo_textbox_set(text_week_box,txt_buf);

    //MM-DD
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d-%02d",time.month,time.day);
    compo_textbox_t *text_data_box = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(text_data_box, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(text_data_box, false);
    compo_textbox_set_location(text_data_box,60, 123,140,45);
    compo_textbox_set(text_data_box,txt_buf);


    compo_button_t * btn = compo_button_create_by_image(frm,UI_BUF_I330001_CALENDAR_BTN_ICON_BIN);
    compo_setid(btn, COMPO_ID_BTN_CALENDAR_ENTER);
    compo_button_set_pos(btn, 119, 239);





    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//切换日期并刷新
static void func_calendar_sub_date_update(bool next)
{

}

//单击按钮
static void func_calendar_sub_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_CALENDAR_ENTER:
            func_cb.flag_sort = false;
            func_cb.sta = FUNC_CALENDAER;
            //    func_switch_to(FUNC_COMPO_SELECT_SUB, FUNC_SWITCH_DIRECT);
            break;
        default:
            break;
    }


}

//公共事件处理
static void func_calendar_sub_comm_process(void)
{
    func_process();
}

//消息处理
static void func_calendar_sub_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_calendar_sub_button_click();
            break;

        case MSG_QDEC_FORWARD:
            // func_calendar_date_update(true);
            break;

        case MSG_QDEC_BACKWARD:
            // func_calendar_date_update(false);
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入日历功能
static void func_calendar_sub_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_calendar_t));
    f_calendar_t *f_calendar = (f_calendar_t *)func_cb.f_cb;

    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);

    f_calendar->today_year = time.year;
    f_calendar->today_mon = time.month;
    f_calendar->today_day = time.day;

    f_calendar->update_year = f_calendar->today_year;
    f_calendar->update_mon = f_calendar->today_mon;
    func_cb.flag_sort = true;

    func_cb.frm_main = func_calender_sub_form_create();
    //刷新文本内容
    // func_calender_sub_refresh(time.year, time.month, time.day);
}

//退出日历功能
static void func_calendar_sub_exit(void)
{
    func_cb.last = FUNC_CALENDAER_SUB;
}

//日历功能
void func_calendar_sub(void)
{
    printf("%s\n", __func__);
    func_calendar_sub_enter();
    while (func_cb.sta == FUNC_CALENDAER_SUB)
    {
        func_calendar_sub_comm_process();
        func_calendar_sub_message(msg_dequeue());
    }
    func_calendar_sub_exit();
}


#endif
