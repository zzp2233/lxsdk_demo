#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//立方体
#define CUBE_RADIUS_MAX             60                                                             //切图正方形一倍
#define CUBE_RADIUS_MIN             45                                                             //切图正方形一半
#define KALE_EDGE_SPACE             2                                                              //边缘距离
#define KALE_ICON_SPACE             2                                                              //图标距离
#define KALE_ICON_OUTER_SIZE        (gui_image_get_size(tbl_menu_cube[0].res_addr).wid)            //外圈图标大小
#define KALE_ICON_INNER_SIZE        (KALE_ICON_OUTER_SIZE / 5 * 2)                                 //内圈图标大小
#define KALE_REFRASH_EXPIRE         4                                                              //刷新间隔 uint：ms
#define KALE_SWITCH_DR              2                                                              //单次变更R值
#define KALE_ROTATE_ANGLE           25


//立方体图标列表(固定6项)
static const compo_cube_item_t tbl_menu_cube[] =
{
    {UI_BUF_I330001_3D_MENU_THEM_BIN,             FUNC_NULL},
    {UI_BUF_I330001_3D_MENU_FINDPHONE_BIN,         FUNC_NULL},
    {UI_BUF_I330001_3D_MENU_MUSIC_BIN,             FUNC_NULL},
    {UI_BUF_I330001_3D_MENU_POWER_BIN,             FUNC_NULL},
    {UI_BUF_I330001_3D_MENU_SETING_BIN,             FUNC_NULL},
    {UI_BUF_I330001_3D_MENU_WEATHER_BIN,             FUNC_NULL},
};

enum
{
    COMPO_ID_CUBE = 1,
};

//立方体表盘
compo_form_t *func_3d_batter_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);       //菜单一般创建在底层

    //创建立方体菜单
    compo_cube_t *cube = compo_cube_create(frm, CUBE_RADIUS_MAX, tbl_menu_cube, CUBE_ITEM_CNT);
    compo_cube_set_type(cube, COMPO_CUBE_TYPE_POWER);
    compo_cube_add_element(cube, 0, UI_BUF_I330001_3D_BATTER_LIGHT_BIN, 18);

    compo_cube_set_pos(cube, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y );
    compo_setid(cube, COMPO_ID_CUBE);
    return frm;
}


static bool click = false;

enum
{
    BATTER_3D_CLICK_STA_NONE,   //None
    BATTER_3D_CLICK_STA_DEC,    //缩小
    BATTER_3D_CLICK_STA_INC,    //放大
    BATTER_3D_CLICK_STA_END,    //结束
    BATTER_3D_CLICK_STA_STOP,   //停顿
};
#define F_FOCUS_AUTO_STEP                     10                                   //松手后自动对齐焦点单位时间步进
#define F_FOCUS_AUTO_STEP_DIV                 18
//地图功能事件处理
void func_3d_batter_process(void)
{
    compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
    compo_cube_move(cube);

    static u32 tick = 0;
    static u8 sta = BATTER_3D_CLICK_STA_NONE; //0:None 1:开始缩小; 2:开始放大; 3:结束; 4:停顿
    u32 width_max = cube->ele[0].location.wid;//gui_image_get_size(tbl_menu_ele[0].res_addr).wid / 2;
    static u32 width;
    static u8 stop_cnt = 0;
    // printf("%s,click=%d,flag_move_auto=%d\n",__func__, click,cube->move_cb.flag_move_auto);
    if (cube->move_cb.flag_move_auto==0)
    {
        if(tick_check_expire(cube->move_cb.tick, 18))
        {
            s32 da;
            cube->move_cb.tick = tick_get();
            da = 20;
            compo_cube_roll(cube, da);
            compo_cube_update(cube);
        }
    }


    if (click)
    {
        if (tick_check_expire(tick, 100))
        {
            tick = tick_get();

            switch (sta)
            {
                case BATTER_3D_CLICK_STA_NONE:
                    width = width_max;
                    if (cube->radius >= CUBE_RADIUS_MAX)
                    {
                        sta = BATTER_3D_CLICK_STA_DEC;
                    }
                    break;

                case BATTER_3D_CLICK_STA_DEC:
                    cube->radius--;
                    //1:x = CUBE_RADIUS_MIN * 1414/1000 : (s32)cube->radius * 1414 / 1000;
                    if ((((s32)cube->radius * 1414 / 1000) / (CUBE_RADIUS_MIN * 1414/1000)) <= width_max)
                    {
                        width--;
                        if (width < 0)
                        {
                            width = 0;
                        }
                    }
                    if (cube->radius <= CUBE_RADIUS_MIN)
                    {
                        width = 0;
                        sta = BATTER_3D_CLICK_STA_STOP;
                    }
                    break;

                case BATTER_3D_CLICK_STA_INC:
                    cube->radius++;

                    if ((((s32)cube->radius * 1414 / 1000) / (CUBE_RADIUS_MIN * 1414/1000)) <= width_max)
                    {
                        width++;
                        if (width > width_max)
                        {
                            width = width_max;
                        }
                    }

                    if (cube->radius >= CUBE_RADIUS_MAX)
                    {
                        width = width_max;
                        sta = BATTER_3D_CLICK_STA_END;
                    }
                    break;

                case BATTER_3D_CLICK_STA_END:
                    click = false;
                    sta = BATTER_3D_CLICK_STA_NONE;
                    break;

                case BATTER_3D_CLICK_STA_STOP:
                    stop_cnt++;
                    if (stop_cnt > 5)
                    {
                        sta = 2;
                        stop_cnt = 0;
                    }
                    break;
            }

            for (int i=0; i<CUBE_ITEM_CNT; i++)
            {
                widget_image3d_set_r(cube->item_img[i], cube->radius);
            }

            for (int i=0; i<CUBE_ITEM_ELE_CNT; i++)
            {
                if (cube->ele[i].img3d != NULL)
                {
                    widget_set_size(cube->ele[i].img3d, width, cube->ele[i].location.hei);
                }
            }
        }
    }
}

void func_3d_batter_click(void)
{
    if (click == false)
    {
        click = true;
    }
}

//地图功能消息处理
void func_3d_batter_message(size_msg_t msg)
{
    point_t pt = ctp_get_sxy();
    s16 cube_limit_x = (GUI_SCREEN_WIDTH - 240) / 2;
    s16 cube_limit_y = (GUI_SCREEN_HEIGHT - 240) / 2;
    bool flag_cube_touch_x = (pt.x >= cube_limit_x) && (pt.x <= (cube_limit_x + 240));
    bool flag_cube_touch_y = (pt.y >= cube_limit_y) && (pt.y <= (cube_limit_y + 240));

    if (flag_cube_touch_x && flag_cube_touch_y)
    {
        if (msg == MSG_CTP_TOUCH)
        {
            compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
            //移动过程中，触屏停止。重新进入到开始拖动模式
            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_DRAG);
        }
        else if (msg >= MSG_CTP_SHORT_LEFT && msg <= MSG_CTP_SHORT_DOWN)
        {
            return;
        }

    }
//    static bool time_visible = 0;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_3d_batter_click();
            break;

//    case MSG_QDEC_FORWARD:                              //向前滚动菜单
//        {
//            compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
//            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_FORWARD);
//        }
//        break;

//    case MSG_QDEC_BACKWARD:                             //向后滚动菜单
//        {
//            compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
//            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_BACKWARD);
//        }
//        break;

        case MSG_SYS_500MS:

//    //秒跳动处理
//        {
//            {
//                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TIME_DOT);
//                compo_textbox_set_visible(txt, time_visible);
//                time_visible ^= 1;
//            }
//        }
//
            break;

        default:
            func_message(msg);
            break;
    }
}
