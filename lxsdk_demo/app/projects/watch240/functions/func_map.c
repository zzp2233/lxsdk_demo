
#include "include.h"
#include "func.h"

#include "func_3d_batter.h"
#include "ute_module_smoke.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_map_t_
{

} f_map_t;

//创建地图窗体
compo_form_t *func_map_form_create(void)
{
    //新建窗体
    // compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_MAP]);

    // //创建按键
    // compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_ICON_MAP_BIN);
    // compo_button_set_pos(btn, 160, 180);

    compo_form_t * frm = func_3d_batter_form_create();

    return frm;
}

//地图功能事件处理
static void func_map_process(void)
{
    func_3d_batter_process();
    func_process();
}

//地图功能消息处理
static void func_map_message(size_msg_t msg)
{
    // switch (msg) {
    // case MSG_CTP_CLICK:
    //     break;

    // case MSG_CTP_SHORT_UP:
    //     break;

    // case MSG_CTP_SHORT_DOWN:
    //     break;

    // case MSG_CTP_LONG:
    //     break;

    // default:
    //     func_message(msg);
    //     break;
    // }

    func_3d_batter_message(msg);

}

//进入地图功能
static void func_map_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_map_t));
    func_cb.frm_main = func_map_form_create();
}

//退出地图功能
static void func_map_exit(void)
{
    func_cb.last = FUNC_MAP;
}

//地图功能
void func_map(void)
{
    printf("%s\n", __func__);
    func_map_enter();
    while (func_cb.sta == FUNC_MAP)
    {
        func_map_process();
        func_map_message(msg_dequeue());
    }
    func_map_exit();
}
