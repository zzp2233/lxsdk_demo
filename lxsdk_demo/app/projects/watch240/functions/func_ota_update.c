#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
enum
{
    ROCKET_ID=1,
    ROCKET_BASE_ID,
    PROGRESS_BAR_ID,
    UPDATING_TXT_ID,
};

typedef struct f_ota_update_t_
{
    u32 launch_roket_time;
} f_ota_update_t;



compo_form_t *func_ota_update_form_create(void)
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
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_UPGRADING]);

    return frm;
}

//显示升级界面处理
static void func_ota_update_disp(void)
{
    static u32 ticks = 0;
//    f_ota_update_t *f_ota_update = (f_ota_update_t *)func_cb.f_cb;

    if (tick_check_expire(ticks, 100))
    {
        ticks = tick_get();
        reset_sleep_delay_all();
//        f_ota_update->launch_roket_time -=10;
//        if(f_ota_update->launch_roket_time < 40)
//        {
//            f_ota_update->launch_roket_time = GUI_SCREEN_HEIGHT*0.5;
//        }
//
//        compo_picturebox_t *picbox = compo_getobj_byid(ROCKET_ID);
//        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, f_ota_update->launch_roket_time);

        extern fot_progress_t *bsp_fot_progress_get(void);
        fot_progress_t *fot_data = bsp_fot_progress_get();

        if(fot_data->type == FOT_FILE_TYPE_FOT)
        {
//            compo_arc_t *arc = compo_getobj_byid(PROGRESS_BAR_ID);
            compo_progressbar_t *bar = compo_getobj_byid(PROGRESS_BAR_ID);
            if(fot_data->percent)
            {
                if(fot_data->percent >= 97)
                {
                    fot_data->percent = 100;
                }
                compo_progressbar_set_value(bar,fot_data->percent);
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum
{
    ROCKET_ID=1,
    ROCKET_BASE_ID,
    PROGRESS_BAR_ID,
    UPDATING_TXT_ID,
};

typedef struct f_ota_update_t_
{
    u32 launch_roket_time;
} f_ota_update_t;



compo_form_t *func_ota_update_form_create(void)
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
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_UPGRADING]);

    return frm;
}

//显示升级界面处理
static void func_ota_update_disp(void)
{
    static u32 ticks = 0;
//    f_ota_update_t *f_ota_update = (f_ota_update_t *)func_cb.f_cb;

    if (tick_check_expire(ticks, 100))
    {
        ticks = tick_get();
        reset_sleep_delay_all();
//        f_ota_update->launch_roket_time -=10;
//        if(f_ota_update->launch_roket_time < 40)
//        {
//            f_ota_update->launch_roket_time = GUI_SCREEN_HEIGHT*0.5;
//        }
//
//        compo_picturebox_t *picbox = compo_getobj_byid(ROCKET_ID);
//        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, f_ota_update->launch_roket_time);

        extern fot_progress_t *bsp_fot_progress_get(void);
        fot_progress_t *fot_data = bsp_fot_progress_get();

        if(fot_data->type == FOT_FILE_TYPE_FOT)
        {
//            compo_arc_t *arc = compo_getobj_byid(PROGRESS_BAR_ID);
            compo_progressbar_t *bar = compo_getobj_byid(PROGRESS_BAR_ID);
            if(fot_data->percent)
            {
                compo_progressbar_set_value(bar,fot_data->percent);
            }
        }
    }
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//刷新显示
static void func_ota_update_process(void)
{

    func_ota_update_disp();
    func_process();
}

//进入升级界面
static void func_ota_update_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_ota_update_t));
    func_cb.frm_main = func_ota_update_form_create();
//    f_ota_update_t *f_ota_update = (f_ota_update_t *)func_cb.f_cb;

//    f_ota_update->launch_roket_time = GUI_SCREEN_HEIGHT*0.7;
}

//退出升级界面
static void func_ota_update_exit(void)
{
//    f_ota_update_t *f_ota_update = (f_ota_update_t *)func_cb.f_cb;
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_OTA_MODE;
}

//升级界面
void func_ota_update(void)
{
    printf("%s\n", __func__);
    func_ota_update_enter();
    reset_sleep_delay_all();
    while (func_cb.sta == FUNC_OTA_MODE)
    {
        func_ota_update_process();

    }
    func_ota_update_exit();
}
