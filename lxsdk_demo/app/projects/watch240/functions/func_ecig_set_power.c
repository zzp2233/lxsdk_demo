#include "include.h"
#include "func.h"
#include "ute_module_smoke.h"
#define TRACE_EN 1
#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

// 组件ID
#define BTN_REST_DISP_TIME 300 // 按钮释放时间 ms 级别
#define MIN_POWER_SINGLE 10    // 单发最小功率
#define MAX_POWER_SINGLE 18    // 单发最大功率
#define MIN_POWER_DOUBLE 20    // 双发最小功率
#define MAX_POWER_DOUBLE 30    // 双发最大功率
#define INDEX_MAX_SINGLE 8     // 单发最大 index
#define INDEX_MAX_DOUBLE 10    // 双发最大 index

enum
{
    COMPO_ID_BTN_ECIG_SET_D_D=0xff,  // 单发
    COMPO_ID_BTN_ECIG_SET_S_D,  // 双发
    COMPO_ID_BTN_ECIG_SET_D_D1, // 单发按钮
    COMPO_ID_BTN_ECIG_SET_S_D1, // 双发按钮
    COMPO_ID_BTN_ECIG_BG_S_D,   // 背景双
    COMPO_ID_BTN_ECIG_BG_D_D,   // 背景单
    COMPO_ID_BTN_ECIG_BG_NUM,   // 单进度条
    COMPO_ID_BTN_ECIG_BG1_NUM,  // 双进度条
    COMPO_ID_BTN_ECIG_SET_ADD,  // 加功按钮
    COMPO_ID_BTN_ECIG_SET_DEL,  // 减功按钮
    COMPO_ID_BTN_ECIG_SET_ADD1, // 加功率
    COMPO_ID_BTN_ECIG_SET_DEL1, // 减功率
};

typedef struct f_setpower_t_
{
    uint32_t tick;
    int current_index; // 当前进度条
    int current_power; // 当前功率值
} f_setpower_t;

// static int stored_power = uteModuleSmokeData.current_power;
// static int stored_index = uteModuleSmokeData.current_index;

// 保存功率和进度条信息
void save_power_and_index(int power, int index)
{
    uteModuleSmokeData.current_power = power;
    uteModuleSmokeData.current_index = index;
    ecig_set_power(power);
}
// 加载功率和进度条信息
void load_power_and_index(int *power, int *index)
{
    *power = uteModuleSmokeData.current_power;
    *index = uteModuleSmokeData.current_index;
    if ((get_gear_func() == 0 && *power < MIN_POWER_SINGLE) ||
        (get_gear_func() == 1 && *power < MIN_POWER_DOUBLE))
    {
        *power = get_gear_func() == 0 ? 15 : 25;
        *index = *power - (get_gear_func() == 0 ? MIN_POWER_SINGLE : MIN_POWER_DOUBLE);
    }
}

// 创建功率窗体
compo_form_t *func_ecig_set_power_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    uint8_t res = ecig_get_res();
    printf("121212122121ecig_get_res()=%d\n",res);

    if((res == 8)||res == 0 )
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_04_BIN);
    }
    else
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_05_BIN);
    }
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-57);
    compo_picturebox_set_visible(picbox, get_gear_func() == 0 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG_D_D);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_NUM_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-57);
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG_S_D);

    compo_button_t *btn1 = compo_button_create(frm);
    compo_button_set_location(btn1, 64, 238, 100, 40);
    compo_setid(btn1, COMPO_ID_BTN_ECIG_SET_D_D);

    compo_button_t *btn2 = compo_button_create(frm);
    compo_button_set_location(btn2, 171, 238, 100, 40);
    compo_setid(btn2, COMPO_ID_BTN_ECIG_SET_S_D);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_00_BIN);
    compo_picturebox_set_pos(picbox, 65, 241);
    compo_picturebox_set_visible(picbox, get_gear_func() == 0 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_D_D1);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_02_BIN);
    compo_picturebox_set_pos(picbox,172, 241);
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_S_D1);

    int power, index;

    load_power_and_index(&f_setpower->current_power, &f_setpower->current_index);
//printf("aaaaaaaaf_setpower->current_inde=%d\n",f_setpower->current_index);
    int total_cnt;
    //printf("res =%d\n",res);

    if(get_gear_func() == 0 &&(( res == 8)||res == 0 ))
    {
        //   printf("111\n");
        total_cnt =9;
    }
    else if(get_gear_func() == 1 &&res == 0 )
    {
        //  printf("222\n");
        total_cnt =11;
    }
    else
    {
        //  printf("333\n");
        total_cnt =11;
    }
    if(f_setpower->current_index > 8 && total_cnt ==9)
        f_setpower->current_index = total_cnt - 1;


    //int total_cnt1 = (get_gear_func1() == 0) ? 9 : 11;
    if(get_gear_func() == 0 &&(( res == 8)||res == 0 ))
    {
        //  printf("444\n");
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN);
    }
    else if  (get_gear_func() == 1 &&res == 0 )
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
        //  printf("555\n");
    }

    else
    {
        // printf("666\n");
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
    }

    compo_picturebox_cut(picbox, f_setpower->current_index, total_cnt);
    compo_picturebox_set_pos(picbox, 120, 82);
    compo_picturebox_set_visible(picbox, get_gear_func() == 0 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG_NUM);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);

    // if (get_gear_func() == 1 && f_setpower->current_index > INDEX_MAX_DOUBLE)
    // {
    //     f_setpower->current_index = INDEX_MAX_DOUBLE;
    // }
    compo_picturebox_cut(picbox, f_setpower->current_index, total_cnt);
    compo_picturebox_set_pos(picbox, 120, 82);
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG1_NUM);

    compo_button_t *btn3 = compo_button_create(frm); // ＋
    compo_button_set_location(btn3, 173, 161,  64, 64);
    compo_button_set_visible(btn3, true);
    compo_setid(btn3, COMPO_ID_BTN_ECIG_SET_ADD);

    compo_button_t *btn4 = compo_button_create(frm); // 减
    compo_button_set_location(btn4,  64, 161, 64, 64);
    compo_button_set_visible(btn4, true);
    compo_setid(btn4, COMPO_ID_BTN_ECIG_SET_DEL);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_ADD_BIN);
    compo_picturebox_set_pos(picbox, 173, 161);
    compo_picturebox_set_visible(picbox, false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_ADD1);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_REDUCE_BIN);
    compo_picturebox_set_pos(picbox, 64, 161);
    compo_picturebox_set_visible(picbox, false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_DEL1);


    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_ADD_BIN);
    compo_picturebox_set_pos(picbox, 173, 161);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_REDUCE_BIN);
    compo_picturebox_set_pos(picbox, 64, 161);

    return frm;
}

// 触摸按钮效果处理
static void func_ecig_set_power_button_touch_handle(void)
{
    int id = compo_get_button_id();
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;
    uint8_t res = ecig_get_res();

    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D1);
    compo_picturebox_t *picbox1 = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_D_D1);
    compo_picturebox_t *picbox2 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_NUM);
    compo_picturebox_t *picbox3 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG1_NUM);
    compo_picturebox_t *picbox4 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_S_D);
    compo_picturebox_t *picbox5 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_D_D);
    compo_picturebox_t *picbox_add = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_ADD1);
    compo_picturebox_t *picbox_del = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_DEL1);
    switch (id)
    {
        case COMPO_ID_BTN_ECIG_SET_S_D:
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
#if ECIG_POWER_CONTROL
            test_2st_gear_func();
#endif
            compo_picturebox_set_visible(picbox1, false);
            compo_picturebox_set_visible(picbox, true);
            compo_picturebox_set_visible(picbox2, false);
            compo_picturebox_set_visible(picbox3, true);
            compo_picturebox_set_visible(picbox5, false);
            compo_picturebox_set_visible(picbox4, true);
            f_setpower->current_index = 25 - MIN_POWER_DOUBLE;
            if (f_setpower->current_index > INDEX_MAX_DOUBLE)
            {
                f_setpower->current_index = INDEX_MAX_DOUBLE;
            }
            f_setpower->current_power = 25;
            compo_picturebox_set(picbox3,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
            compo_picturebox_cut(picbox3, f_setpower->current_index, 11);

            // printf("111Switched to Double mode: total_cnt = 11, current_index = %d, current_power = %d\n",
            //        f_setpower->current_index, f_setpower->current_power);
            break;
        case COMPO_ID_BTN_ECIG_SET_D_D:
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
#if ECIG_POWER_CONTROL
            test_1st_gear_func();
#endif
            compo_picturebox_set_visible(picbox, false);
            compo_picturebox_set_visible(picbox1, true);
            compo_picturebox_set_visible(picbox3, false);
            compo_picturebox_set_visible(picbox2, true);
            compo_picturebox_set_visible(picbox4, false);
            compo_picturebox_set_visible(picbox5, true);
            f_setpower->current_index = 15 - MIN_POWER_SINGLE;
            f_setpower->current_power = 15;
            if(res==8||res==4||res==0)
            {
                //printf("1111111111111111111111111111,f_setpower->current_index=%d\n",f_setpower->current_index);
                compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN);
                compo_picturebox_cut(picbox2, f_setpower->current_index, 9);
            }
            else
            {
                // printf("22222222222222222222222222222222\n");
                compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
                compo_picturebox_cut(picbox2, f_setpower->current_index, 11);
            }

            ecig_set_power(f_setpower->current_power);

            // printf("222Switched to Single mode: total_cnt = 9, current_index = %d, current_power = %d\n",
            //    f_setpower->current_index, f_setpower->current_power);
            break;
        case COMPO_ID_BTN_ECIG_SET_ADD:
            if (get_gear_func() == 0 && (res==4||res==8||res==0))
            {
                if (f_setpower->current_index < INDEX_MAX_SINGLE)
                {
                    // printf("444444444444444444444444444\n");
                    f_setpower->current_index++;
                    f_setpower->current_power++;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN);
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 9);
                }
            }
            else if (get_gear_func() == 0 && (res==6||res==12))
            {
                if (f_setpower->current_index < INDEX_MAX_DOUBLE)
                {
                    // printf("55555555555555555555555555555\n");
                    f_setpower->current_index++;
                    f_setpower->current_power++;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 11);
                }
            }
            else
            {
                if (f_setpower->current_index < INDEX_MAX_DOUBLE)
                {
                    // printf("566666666666666666666666666666666\n");
                    f_setpower->current_index++;
                    f_setpower->current_power++;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_set(picbox3,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
                    compo_picturebox_cut(picbox3, f_setpower->current_index, 11);
                }
            }

            // printf("333Power increased: Mode %s, total_cnt = %d, current_index = %d, current_power = %d\n",
            //        get_gear_func1() == 0 ? "Single" : "Double",
            //        get_gear_func1() == 0 ? 9 : 11, f_setpower->current_index, f_setpower->current_power);
            break;

        case COMPO_ID_BTN_ECIG_SET_DEL:
            if (get_gear_func() == 0 && (res==4||res==8||res==0))
            {
                if (f_setpower->current_index > 0)
                {
                    f_setpower->current_index--;
                    f_setpower->current_power--;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN);
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 9);
                }
            }
            else if (get_gear_func() == 0 && (res==6||res==12))
            {
                if (f_setpower->current_index > 0)
                {
                    f_setpower->current_index--;
                    f_setpower->current_power--;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 11);
                }
            }
            else
            {
                if (f_setpower->current_index > 0)
                {
                    f_setpower->current_index--;
                    f_setpower->current_power--;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_set(picbox3,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
                    compo_picturebox_cut(picbox3, f_setpower->current_index, 11);
                }
            }

            // printf("444Power decreased: Mode %s, total_cnt = %d, current_index = %d, current_power = %d\n",
            //        get_gear_func1() == 0 ? "Single" : "Double",
            //        get_gear_func1() == 0 ? 9 : 11, f_setpower->current_index, f_setpower->current_power);
            break;

        default:
            break;
    }
    // // 新增：按钮操作后强制刷新（确保状态同步）
    // func_ecig_set_power_refresh();
    save_power_and_index(f_setpower->current_power, f_setpower->current_index);
    uteModuleSmokeDataSaveConfig();
}
// 刷新功率设置界面（根据当前模式和电阻值更新显示）
void func_ecig_set_power_refresh(void)
{
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;
    if (!f_setpower) return;

    uint8_t res = ecig_get_res();
    // 获取界面组件
    compo_picturebox_t *single_mode = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_D_D1);
    compo_picturebox_t *double_mode = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D1);
    compo_picturebox_t *single_progress = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_NUM);
    compo_picturebox_t *double_progress = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG1_NUM);
    compo_picturebox_t *single_bg = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_D_D);
    compo_picturebox_t *double_bg = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_S_D);

    // 根据当前模式（单发/双发）更新界面显示
    if (get_gear_func() == 0)    // 单发模式
    {
        compo_picturebox_set_visible(single_mode, true);
        compo_picturebox_set_visible(double_mode, false);
        compo_picturebox_set_visible(single_progress, true);
        compo_picturebox_set_visible(double_progress, false);
        compo_picturebox_set_visible(single_bg, true);
        compo_picturebox_set_visible(double_bg, false);

        // 根据电阻值选择进度条图标
        if (res == 8 || res == 0)
        {
            compo_picturebox_set(single_progress, UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN);
            compo_picturebox_cut(single_progress, f_setpower->current_index, 9);

        }
        else
        {
            compo_picturebox_set(single_progress, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
            compo_picturebox_cut(single_progress, f_setpower->current_index, 11);
        }
        f_setpower->current_power = 15;
    }
    else      // 双发模式
    {
        compo_picturebox_set_visible(double_mode, true);
        compo_picturebox_set_visible(single_mode, false);
        compo_picturebox_set_visible(double_progress, true);
        compo_picturebox_set_visible(single_progress, false);
        compo_picturebox_set_visible(double_bg, true);
        compo_picturebox_set_visible(single_bg, false);

        compo_picturebox_set(double_progress, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);
        compo_picturebox_cut(double_progress, f_setpower->current_index, 11);
        f_setpower->current_power = 25;
    }
    // load_power_and_index(&uteModuleSmokeData.current_power, &uteModuleSmokeData.current_index);
    // printf("2222uteModuleSmokeData.current_power=%d，ecig_get_power()=%d\r\n",uteModuleSmokeData.current_power,ecig_get_power());
}
static void func_ecig_set_power_button_release_handle()
{
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;

    s32 dx, dy;
    compo_picturebox_t *picbox_add = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_ADD1);
    compo_picturebox_t *picbox_del = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_DEL1);

    if (f_setpower->tick <= tick_get() - BTN_REST_DISP_TIME)
    {
        compo_picturebox_set_visible(picbox_add, false);
        compo_picturebox_set_visible(picbox_del, false);
    }

    int id = compo_get_button_id();
    if (id == 0 || !ctp_get_dxy(&dx, &dy))
        return;

    f_setpower->tick = tick_get();

    switch (id)
    {
        case COMPO_ID_BTN_ECIG_SET_ADD:
            compo_picturebox_set_visible(picbox_add, true);
            break;
        case COMPO_ID_BTN_ECIG_SET_DEL:
            compo_picturebox_set_visible(picbox_del, true);
            break;
    }
}

// 功率功能消息处理
static void func_ecig_set_power_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_ecig_set_power_button_touch_handle();
            break;
        // case MSG_CTP_SHORT_LEFT:
        // case MSG_CTP_SHORT_UP:
        // case MSG_CTP_SHORT_RIGHT:
        // case MSG_CTP_SHORT_DOWN:
        //     // func_menu_sub_drag_move();
        //     break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

// 功率功能事件处理
static void func_ecig_set_power_process(void)
{
    func_ecig_set_power_button_release_handle();
    func_process();

    // 新增：检测模式是否变化，变化时强制刷新界面
    static int last_gear = -1;  // 记录上一次模式
    // int current_gear = get_gear_func();
    // if (last_gear != current_gear) {  // 模式变化时
    //     func_ecig_set_power_refresh();  // 调用刷新函数
    //     last_gear = current_gear;  // 更新记录
    // }
}

// 进入功率功能
static void func_ecig_set_power_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_setpower_t));
    func_cb.frm_main = func_ecig_set_power_form_create();
    get_gear_func();
    ecig_get_res();
}

// 退出功率功能
static void func_ecig_set_power_exit(void)
{
    if (func_cb.f_cb != NULL)
    {
        f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;
        save_power_and_index(f_setpower->current_power, f_setpower->current_index);
        func_free(func_cb.f_cb);
        func_cb.f_cb = NULL;
    }
    uteModuleSmokeDataSaveConfig();
    func_cb.last = FUNC_ECIG_SET_POWER;
}

// 功率功能
void func_ecig_set_power(void)
{
    printf("%s\n", __func__);
    func_ecig_set_power_enter();
    while (func_cb.sta == FUNC_ECIG_SET_POWER)
    {
        func_ecig_set_power_process();
        func_ecig_set_power_message(msg_dequeue());
    }
    func_ecig_set_power_exit();
}
