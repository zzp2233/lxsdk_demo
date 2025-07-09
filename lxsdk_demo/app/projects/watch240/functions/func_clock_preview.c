#include "include.h"
#include "func.h"
#include "ute_module_gui_common.h"
#include "ute_module_watchonline.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define CUR_PREVIEW_STYLE           PREVIEW_ROTARY_STYLE

extern u8 func_clock_get_max_dialplate_num(void);
extern u32 dialplate_info[];

//根据表盘的基地址获取对应的预览图
u32 func_clock_preview_get_addr(u32 base_addr)
{
    u32 user_addr = base_addr;
#if UTE_MODULE_CUSTOM_WATCHONLINE_UITOOL_SUPPORT
    user_addr += sizeof(watchConfig_t);
#endif
    uitool_header_t uitool_header;
    os_spiflash_read(&uitool_header, user_addr, UITOOL_HEADER);
    for(u16 i=0; i<uitool_header.num; i++)
    {
        uitool_res_t uitool_res = {0};
        os_spiflash_read(&uitool_res, user_addr + UITOOL_HEADER + i * UITOOL_RES_HEADER, UITOOL_RES_HEADER);
        u32 res_addr = user_addr + uitool_res.res_addr;
        //预览图
        if (uitool_res.res_type == UITOOL_TYPE_IMAGE && uitool_res.bond_type == COMPO_BOND_IMAGE_CLOCK_PREVIEW)
        {
            return res_addr;
        }
    }

    return 0;
}

#if (CUR_PREVIEW_STYLE == PREVIEW_ROTARY_STYLE)

/********************************************************************
 * 注意：使用该效果的表盘预览图在生成时需要使能旋转压缩
*********************************************************************/


//组件ID
enum
{
    //列表(横向)
    COMPO_ID_ROTARY = 1,

};

typedef struct f_clock_preview_t_
{
    compo_rotary_t *rotary;
} f_clock_preview_t;

compo_rotary_item_t prev_dial_item[STACKLIST_PIC_COUNT_MAX] = {0};


//获取表盘预览类型
u16 func_clock_preview_get_type(void)
{
    return CUR_PREVIEW_STYLE;
}


//创建表盘预览窗体
compo_form_t *func_clock_preview_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    //获取表盘预览图集合
    u8 dial_count = func_clock_get_max_dialplate_num();

    for (u8 i = 0; i < dial_count; i++)
    {
        prev_dial_item[i].res_addr = func_clock_preview_get_addr(dialplate_info[i]);
        prev_dial_item[i].str_idx = STR_NULL;
    }

    //创建背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(24, 24, 32));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);

    //创建转盘组件
    compo_rotary_t *rotary = compo_rotary_create(frm, prev_dial_item, dial_count);
    compo_setid(rotary, COMPO_ID_ROTARY);
    compo_rotary_update(rotary);

    return frm;
}

//表盘预览功能事件处理
static void func_clock_preview_process(void)
{
    f_clock_preview_t *f_clock_preview = (f_clock_preview_t *)func_cb.f_cb;
    compo_rotary_t *rotary = f_clock_preview->rotary;
    compo_rotary_move(rotary);
    func_process();
    //printf("idx = %d, width = %d, height = %d\n", compo_rotary_get_idx(rotary),
    //       gui_image_get_size(prev_dial_item[compo_rotary_get_idx(rotary)].res_addr).wid,
    //       gui_image_get_size(prev_dial_item[compo_rotary_get_idx(rotary)].res_addr).hei);
    if (compo_rotary_get_sta(rotary) == COMPO_ROTARY_STA_EXIT)
    {
        func_cb.sta = FUNC_CLOCK;
    }
}

//表盘预览功能消息处理
static void func_clock_preview_message(size_msg_t msg)
{
    f_clock_preview_t *f_clock_preview = (f_clock_preview_t *)func_cb.f_cb;
    compo_rotary_t *rotary = f_clock_preview->rotary;

    switch (msg)
    {
        case KU_BACK:
            // sys_cb.dialplate_index = compo_rotary_get_idx(rotary);
            uteModuleGuiCommonSetCurrWatchIndex(compo_rotary_get_idx(rotary));
            break;
        default:
            break;
    }

    if (compo_rotary_message(rotary, msg))
    {
        return;
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
            // sys_cb.dialplate_index = compo_rotary_get_idx(rotary);
            uteModuleGuiCommonSetCurrWatchIndex(compo_rotary_get_idx(rotary));
            compo_rotary_move_control(rotary, COMPO_ROTARY_MOVE_CMD_EXITING);
            break;

        default:
            func_message(msg);
            break;
    }


}

//进入表盘预览功能
static void func_clock_preview_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_clock_preview_t));
    func_cb.frm_main = func_clock_preview_form_create();
    tft_set_temode(0);
    f_clock_preview_t *f_clock_preview = (f_clock_preview_t *)func_cb.f_cb;
    f_clock_preview->rotary = compo_getobj_byid(COMPO_ID_ROTARY);
    compo_rotary_t *rotary = f_clock_preview->rotary;
    if (rotary->type != COMPO_TYPE_ROTARY)
    {
        halt(HALT_GUI_COMPO_ROTARY_TYPE);
    }
    compo_rotary_set_rotation_byidx(rotary, sys_cb.dialplate_index);
    compo_rotary_update(rotary);
    if (func_cb.last == FUNC_CLOCK)
    {
        compo_rotary_move_control(rotary, COMPO_ROTARY_MOVE_CMD_ENTERING);      //入场
    }
}

//退出表盘预览功能
static void func_clock_preview_exit(void)
{
    tft_set_temode(DEFAULT_TE_MODE);
    func_cb.last = FUNC_CLOCK_PREVIEW;
    task_stack_pop();
}

//表盘预览功能
void func_clock_preview(void)
{
    printf("%s\n", __func__);
    func_clock_preview_enter();
    while (func_cb.sta == FUNC_CLOCK_PREVIEW)
    {
        func_clock_preview_process();
        func_clock_preview_message(msg_dequeue());
    }
    func_clock_preview_exit();
}


#endif // (CUR_PREVIEW_STYLE == PREVIEW_ROTARY_STYLE)



