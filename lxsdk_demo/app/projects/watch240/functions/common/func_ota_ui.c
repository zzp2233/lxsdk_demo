#include "include.h"
#include "func.h"
#include "func_idle.h"
#include "bsp_led.h"

#if FOTA_UI_EN

enum
{
    COMPO_ID_PROBAR_UI = 1,
    COMPO_ID_PROBAR_CODE,
};

typedef struct f_progressbar_t_
{
    u32 tick;
    int32_t ui_percent;
    int32_t code_percent;
} f_progressbar_t;

//bsp_fot_progress_get

compo_form_t* func_ota_ui_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    //UI 进度条
    compo_progressbar_t *bar = compo_progressbar_create(frm, PROGRESSBAR_HORIZONTAL);
    compo_setid(bar, COMPO_ID_PROBAR_UI);
    compo_progressbar_set_location(bar, GUI_SCREEN_WIDTH / 4 + 10, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH/2 - 20, 50);
    compo_progressbar_set_range(bar, 0, 100);
    compo_progressbar_set_color(bar, COLOR_BLUE, COLOR_DGRAY);

    //CODE 进度条
    bar = compo_progressbar_create(frm, PROGRESSBAR_HORIZONTAL);
    compo_setid(bar, COMPO_ID_PROBAR_CODE);
    compo_progressbar_set_location(bar, GUI_SCREEN_WIDTH * 3 / 4 - 10, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH/2 - 20, 50);
    compo_progressbar_set_range(bar, 0, 100);
    compo_progressbar_set_color(bar, COLOR_GREEN, COLOR_DGRAY);

    return frm;
}

static void func_ota_ui_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_progressbar_t));
//    f_progressbar_t *f_progressbar = (f_progressbar_t *)func_cb.f_cb;

    func_cb.frm_main = func_ota_ui_create();

    printf("%s\n", __func__);
}

static void func_ota_ui_update(void)
{
    f_progressbar_t *f_progressbar = (f_progressbar_t *)func_cb.f_cb;

    extern fot_progress_t *bsp_fot_progress_get(void);
    fot_progress_t *fot_data = bsp_fot_progress_get();
    if (fot_data->type == FOT_FILE_TYPE_UI)
    {
        f_progressbar->ui_percent = (int32_t)fot_data->percent;
        compo_progressbar_t *bar = compo_getobj_byid(COMPO_ID_PROBAR_UI);
        if (bar == NULL)
        {
            return;
        }
        compo_progressbar_set_value(bar, f_progressbar->ui_percent);
    }
    else if (fot_data->type == FOT_FILE_TYPE_FOT)
    {
        f_progressbar->code_percent = (int32_t)fot_data->percent;
        compo_progressbar_t *bar = compo_getobj_byid(COMPO_ID_PROBAR_CODE);
        if (bar == NULL)
        {
            return;
        }
        compo_progressbar_set_value(bar, f_progressbar->code_percent);
    }

    //printf("fot_data->percent = %d\n", fot_data->percent);

}

AT(.text.func.idle)
void func_ota_ui_process(void)
{
    reset_sleep_delay_all();
    func_ota_ui_update();
    func_process();
}

void func_ota_ui_message(u16 msg)
{
    evt_message(msg);
}

static void func_ota_ui_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_OTA_UI_MODE;
}

AT(.text.func.idle)
void func_ota_ui(void)
{
    printf("%s\n", __func__);

    func_ota_ui_enter();

    while (func_cb.sta == FUNC_OTA_UI_MODE)
    {
        func_ota_ui_message(msg_dequeue());
        func_ota_ui_process();
    }

    func_ota_ui_exit();
}

#endif
