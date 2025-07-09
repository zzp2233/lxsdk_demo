#include "include.h"
#include "func.h"
#include "ute_module_watchonline.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT

#define SWITCH_TIME      2000

enum
{
    ROCKET_ID=1,
    ROCKET_BASE_ID,
    PROGRESS_BAR_ID,
    UPDATING_TXT_ID,
    UPDATING_TXT_TIP_ID,
};
enum
{
    UPGRADE_SUCCESSFUL=0,//成功
    UPGRADE_FAILED,//失败
    UPGRADING,//升级中
};

typedef struct f_up_watch_dial_t_
{
    u8 state;
    u32 ticks;
    u32 switch_page_time;
} f_up_watch_dial_t;


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
compo_form_t *func_up_watch_dial_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_OTA_00_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 35+112/2);
    compo_setid(picbox,ROCKET_ID);

    //CODE 进度条
    compo_progressbar_t * bar = compo_progressbar_create(frm, PROGRESSBAR_HORIZONTAL);
    compo_setid(bar, PROGRESS_BAR_ID);
    compo_progressbar_set_location(bar, GUI_SCREEN_CENTER_X, 224+10/2, 196, 10);
    compo_progressbar_set_range(bar, 0, 100);
    compo_progressbar_set_edge_circle(bar, 10/2);
    compo_progressbar_set_color(bar, make_color(35,141,255), make_color(51,51,51));

    //TXT1 升级中
    compo_textbox_t* txt = compo_textbox_create(frm, 40);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_INS]);

    txt = compo_textbox_create(frm,  40);
    compo_setid(txt, UPDATING_TXT_TIP_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 224+10/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_APP_AGIAN]);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);
    compo_textbox_set_visible(txt, false);

    return frm;
}
extern bool slider_unlock;
//显示升级界面处理
static void func_up_watch_dial_disp(void)
{
    f_up_watch_dial_t *f_up_watch_dial = (f_up_watch_dial_t *)func_cb.f_cb;

    if(f_up_watch_dial->state  != UPGRADING)
    {
        if(f_up_watch_dial->switch_page_time+SWITCH_TIME >= tick_get())
        {
            printf("22222222222222222222slider_unlock=%d\r\n",slider_unlock);
            if(slider_unlock == true)
                func_cb.sta = FUNC_CLOCK;
            else
                func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
        }

        // return;
    }

    if (tick_check_expire(f_up_watch_dial->ticks, 100))
    {
        f_up_watch_dial->ticks = tick_get();
        reset_sleep_delay_all();

        compo_progressbar_t *bar    = compo_getobj_byid(PROGRESS_BAR_ID);
        compo_textbox_t* txt_tip    = compo_getobj_byid(UPDATING_TXT_TIP_ID);
        compo_textbox_t* txt_state  = compo_getobj_byid(UPDATING_TXT_ID);
        compo_picturebox_t * picbox = compo_getobj_byid(ROCKET_ID);
        if(ble_is_connect())//ble状态
        {
            uint32_t progress = uteModuleWatchOnlineGetSynchronizeWatchSize()*100/uteModuleWatchOnlineGetTotileWatchSize();
            printf("progress:%d,SynchronizeWatchSize:%d,TotileWatchSize:%d\n",progress,uteModuleWatchOnlineGetSynchronizeWatchSize(),uteModuleWatchOnlineGetTotileWatchSize());
            if (progress>=100)
            {
                progress=100;
                f_up_watch_dial->state  = UPGRADE_SUCCESSFUL;
//                compo_textbox_set_visible(txt_tip, true);
                compo_textbox_set(txt_state, i18n[STR_SYNC_SUC]);
                compo_picturebox_set(picbox, UI_BUF_I330001_OTA_00_BIN);
                compo_progressbar_set_color(bar, make_color(0,0,0), make_color(0,0,0));
                f_up_watch_dial->switch_page_time = tick_get();
            }
            compo_progressbar_set_value(bar,progress);
        }
        else
        {
            f_up_watch_dial->state  = UPGRADE_FAILED;
            compo_textbox_set_visible(txt_tip, true);
            compo_textbox_set(txt_state, i18n[STR_SYNC_FAIL]);
            compo_picturebox_set(picbox, UI_BUF_I330001_OTA_02_BIN);
            compo_progressbar_set_color(bar, make_color(0,0,0), make_color(0,0,0));
            f_up_watch_dial->switch_page_time = tick_get();
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
compo_form_t *func_up_watch_dial_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

//    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_OTA_00_BIN);
//    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 35+112/2);
//    compo_setid(picbox,ROCKET_ID);

    //CODE 进度条
    compo_progressbar_t * bar = compo_progressbar_create(frm, PROGRESSBAR_HORIZONTAL);
    compo_setid(bar, PROGRESS_BAR_ID);
    compo_progressbar_set_location(bar, GUI_SCREEN_CENTER_X, 224+10/2, 196, 10);
    compo_progressbar_set_range(bar, 0, 100);
    compo_progressbar_set_edge_circle(bar, 10/2);
    compo_progressbar_set_color(bar, make_color(35,141,255), make_color(51,51,51));

    //TXT1 升级中
    compo_textbox_t* txt = compo_textbox_create(frm, 40);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_INS]);

    txt = compo_textbox_create(frm,  40);
    compo_setid(txt, UPDATING_TXT_TIP_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 224+10/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_APP_AGIAN]);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);
    compo_textbox_set_visible(txt, false);

    return frm;
}

//显示升级界面处理
static void func_up_watch_dial_disp(void)
{
    f_up_watch_dial_t *f_up_watch_dial = (f_up_watch_dial_t *)func_cb.f_cb;

    if(f_up_watch_dial->state  != UPGRADING)
    {
        if(f_up_watch_dial->switch_page_time+SWITCH_TIME >= tick_get())
        {
            func_cb.sta = FUNC_CLOCK;
        }

        return;
    }

    if (tick_check_expire(f_up_watch_dial->ticks, 100))
    {
        f_up_watch_dial->ticks = tick_get();
        reset_sleep_delay_all();

        compo_progressbar_t *bar    = compo_getobj_byid(PROGRESS_BAR_ID);
        compo_textbox_t* txt_tip    = compo_getobj_byid(UPDATING_TXT_TIP_ID);
        compo_textbox_t* txt_state  = compo_getobj_byid(UPDATING_TXT_ID);
        compo_picturebox_t * picbox = compo_getobj_byid(ROCKET_ID);
        if(ble_is_connect())//ble状态
        {
            uint32_t progress = uteModuleWatchOnlineGetSynchronizeWatchSize()*100/uteModuleWatchOnlineGetTotileWatchSize();
            printf("progress:%d,SynchronizeWatchSize:%d,TotileWatchSize:%d\n",progress,uteModuleWatchOnlineGetSynchronizeWatchSize(),uteModuleWatchOnlineGetTotileWatchSize());
            if (progress>=100)
            {
                progress=100;
                f_up_watch_dial->state  = UPGRADE_SUCCESSFUL;
//                compo_textbox_set_visible(txt_tip, true);
                compo_textbox_set(txt_state, i18n[STR_SYNC_SUC]);
//                compo_picturebox_set(picbox, UI_BUF_I330001_OTA_00_BIN);
                compo_progressbar_set_color(bar, make_color(0,0,0), make_color(0,0,0));
                f_up_watch_dial->switch_page_time = tick_get();
            }
            compo_progressbar_set_value(bar,progress);
        }
        else
        {
            f_up_watch_dial->state  = UPGRADE_FAILED;
            compo_textbox_set_visible(txt_tip, true);
            compo_textbox_set(txt_state, i18n[STR_SYNC_FAIL]);
//            compo_picturebox_set(picbox, UI_BUF_I330001_OTA_02_BIN);
            compo_progressbar_set_color(bar, make_color(0,0,0), make_color(0,0,0));
            f_up_watch_dial->switch_page_time = tick_get();
        }
    }
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT


//刷新显示
static void func_up_watch_dial_process(void)
{
    func_up_watch_dial_disp();
    func_process();
}

//进入升级界面
static void func_up_watch_dial_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_up_watch_dial_t));
    func_cb.frm_main = func_up_watch_dial_form_create();
    f_up_watch_dial_t *f_up_watch_dial = (f_up_watch_dial_t *)func_cb.f_cb;
    f_up_watch_dial->state = UPGRADING;
    f_up_watch_dial->switch_page_time = 0;
}

//退出升级界面
static void func_up_watch_dial_exit(void)
{
    func_cb.last = FUNC_UP_WATCH_DIAL;
}

//升级界面
void func_up_watch_dial(void)
{
    printf("%s\n", __func__);
    func_up_watch_dial_enter();
    reset_sleep_delay_all();
    while (func_cb.sta == FUNC_UP_WATCH_DIAL)
    {
        func_up_watch_dial_process();
//        func_message(msg_dequeue());
    }
    func_up_watch_dial_exit();
}
#endif
