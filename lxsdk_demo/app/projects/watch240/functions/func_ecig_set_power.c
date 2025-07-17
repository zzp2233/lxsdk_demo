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
#define MAX_MIN_POWER_DOUBLE (MAX_POWER_DOUBLE + MIN_POWER_DOUBLE) / 2 // 中间功率

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

/**  
  * @brief  保存功率和进度条信息到全局数据结构  
  * @param  power: 功率值  
  * @param  index: 进度条索引值  
  * @retval None  
  */  
void save_power_and_index(int power, int index)  
{  
    uteModuleSmokeData.current_power = power;           // 保存当前功率值到全局数据结构  
    uteModuleSmokeData.current_index = index;           // 保存当前进度条索引到全局数据结构  
    ecig_set_power(power);                              // 调用底层函数设置实际硬件功率  
}


/**  
  * @brief  从全局数据结构加载功率和进度条信息  
  * @param  power: 指向功率值的指针  
  * @param  index: 指向进度条索引的指针  
  * @retval None  
  */  
void load_power_and_index(int *power, int *index)  
{  
    *power = uteModuleSmokeData.current_power;          // 从全局数据结构读取当前功率值  
    *index = uteModuleSmokeData.current_index;          // 从全局数据结构读取当前进度条索引  
      
    // 检查功率值是否低于最小值，如果是则设置为默认值  
    if ((get_gear_func() == 0 && *power < MIN_POWER_SINGLE) ||      // 单发模式且功率低于单发最小值  
        (get_gear_func() == 1 && *power < MIN_POWER_DOUBLE))        // 或双发模式且功率低于双发最小值  
    {  
        *power = get_gear_func() == 0 ? 15 : MAX_MIN_POWER_DOUBLE;  // 设置默认功率值：单发15W，双发25W  
        *index = *power - (get_gear_func() == 0 ? MIN_POWER_SINGLE : MIN_POWER_DOUBLE); // 计算对应的进度条索引  
    }  
}

/**  
  * @brief  创建电子烟功率设置界面窗体  
  * @param  None  
  * @retval compo_form_t*: 返回创建的窗体指针  
  */  
compo_form_t *func_ecig_set_power_form_create(void)  
{  
    // 创建主窗体  
    compo_form_t *frm = compo_form_create(true);                    // 创建新窗体，参数true表示启用  
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;        // 获取功率设置功能控制块指针  
  
    // 创建背景图片  
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_BG_BIN);  
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);  // 设置背景图片位置为屏幕中心  
  
    // 创建单发/双发切换按钮的图标  
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_01_BIN);  // 单发按钮图标  
    compo_picturebox_set_pos(picbox, 62, 241);                      // 设置单发按钮位置  
      
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_03_BIN);  // 双发按钮图标  
    compo_picturebox_set_pos(picbox, 179, 241);                     // 设置双发按钮位置  
  
    // 获取电阻值并根据电阻值选择显示的图标  
    uint8_t res = ecig_get_res();                                   // 获取当前电阻值  
    printf("121212122121ecig_get_res()=%d\\n", res);                // 调试输出电阻值  
  
    // 根据电阻值选择不同的显示图标  
    if ((res == 8) || res == 0)                                    // 电阻值为8欧姆或0欧姆  
    {  
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_04_BIN);  // 使用04图标  
    }  
    else                                                            // 其他电阻值  
    {  
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_05_BIN);  // 使用05图标  
    }  
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 57); // 设置图标位置  
    compo_picturebox_set_visible(picbox, get_gear_func() == 0 ? true : false);       // 单发模式时显示  
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG_D_D);                 // 设置组件ID  
  
    // 根据不同项目配置创建双发模式的数字显示图标  
#if PROJECT_ABE012_SUPPORT                                          // ABE012项目支持  
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_NUM_BIN);  
#elif PROJECT_ABE013_SUPPORT                                        // ABE013项目支持  
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_NUM25_35_BIN);  
#elif PROJECT_ABF012_SUPPORT                                        // ABF012项目支持  
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_NUM30_40_BIN);  
#endif  
  
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 57); // 设置双发图标位置  
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);       // 双发模式时显示  
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG_S_D);                 // 设置组件ID  
  
    // 创建单发/双发切换按钮  
    compo_button_t *btn1 = compo_button_create(frm);                // 创建单发按钮  
    compo_button_set_location(btn1, 64, 238, 100, 40);             // 设置按钮位置和大小(x, y, width, height)  
    compo_setid(btn1, COMPO_ID_BTN_ECIG_SET_D_D);                  // 设置按钮ID  
  
    compo_button_t *btn2 = compo_button_create(frm);                // 创建双发按钮  
    compo_button_set_location(btn2, 171, 238, 100, 40);            // 设置按钮位置和大小  
    compo_setid(btn2, COMPO_ID_BTN_ECIG_SET_S_D);                  // 设置按钮ID  
  
    // 创建按钮状态指示图标  
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_00_BIN);     // 单发选中状态图标  
    compo_picturebox_set_pos(picbox, 62, 241);                     // 设置位置  
    compo_picturebox_set_visible(picbox, get_gear_func() == 0 ? true : false);      // 单发模式时显示  
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_D_D1);               // 设置组件ID  
  
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_02_BIN);     // 双发选中状态图标  
    compo_picturebox_set_pos(picbox, 179, 241);                    // 设置位置  
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);      // 双发模式时显示  
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_S_D1);               // 设置组件ID  
  
    // 加载当前功率设置  
    int power, index;                                               // 声明临时变量  
    load_power_and_index(&f_setpower->current_power, &f_setpower->current_index);   // 加载功率和索引值  
  
    // 根据模式和电阻值确定进度条总数  
    int total_cnt;                                                  // 进度条总段数  
      
    if (get_gear_func() == 0 && ((res == 8) || res == 0))         // 单发模式且电阻为8欧姆或0欧姆  
    {  
        total_cnt = 9;                                              // 设置为9段  
    }  
    else if (get_gear_func() == 1 && res == 0)                    // 双发模式且电阻为0欧姆  
    {  
        total_cnt = 11;                                             // 设置为11段  
    }  
    else                                                            // 其他情况  
    {  
        total_cnt = 11;                                             // 默认设置为11段  
    }  
      
    // 边界检查：如果当前索引超出范围，则调整到最大值  
    if (f_setpower->current_index > 8 && total_cnt == 9)  
        f_setpower->current_index = total_cnt - 1;                  // 调整到最大索引  
  
    // 创建单发模式的功率进度条  
    if (get_gear_func() == 0 && ((res == 8) || res == 0))         // 单发模式特定电阻值  
    {  
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN);  // 单发进度条图标  
    }  
    else if (get_gear_func() == 1 && res == 0)                    // 双发模式特定电阻值  
    {  
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);    // 双发进度条图标  
    }  
    else                                                            // 其他情况  
    {  
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);    // 默认双发进度条图标  
    }  
  
    compo_picturebox_cut(picbox, f_setpower->current_index, total_cnt);             // 切割图片显示当前进度  
    compo_picturebox_set_pos(picbox, 120, 90);                                      // 设置进度条位置  
    compo_picturebox_set_visible(picbox, get_gear_func() == 0 ? true : false);      // 单发模式时显示  
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG_NUM);                                  // 设置组件ID  
  
    // 创建双发模式的功率进度条  
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);       // 双发进度条图标  
    compo_picturebox_cut(picbox, f_setpower->current_index, total_cnt);                     // 切割图片显示当前进度  
    compo_picturebox_set_pos(picbox, 120, 90);                                              // 设置进度条位置  
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);              // 双发模式时显示  
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG1_NUM);                                         // 设置组件ID  
  
    // 创建功率调节按钮  
    compo_button_t *btn3 = compo_button_create(frm);               // 创建增加功率按钮  
    compo_button_set_location(btn3, 173, 161, 64, 64);             // 设置按钮位置和大小  
    compo_button_set_visible(btn3, true);                          // 设置按钮可见  
    compo_setid(btn3, COMPO_ID_BTN_ECIG_SET_ADD);                  // 设置按钮ID  
  
    compo_button_t *btn4 = compo_button_create(frm);               // 创建减少功率按钮  
    compo_button_set_location(btn4, 64, 161, 64, 64);              // 设置按钮位置和大小  
    compo_button_set_visible(btn4, true);                          // 设置按钮可见  
    compo_setid(btn4, COMPO_ID_BTN_ECIG_SET_DEL);                  // 设置按钮ID  
  
    // 创建按钮按下时的视觉反馈图标  
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_ADD_BIN);         // 增加按钮

    // if (get_gear_func() == 1 && f_setpower->current_index > INDEX_MAX_DOUBLE)
    // {
    //     f_setpower->current_index = INDEX_MAX_DOUBLE;
    // }

    //双发模式进度条设置
    compo_picturebox_cut(picbox, f_setpower->current_index, total_cnt);         // 切割进度条图片，显示当前功率进度  
    compo_picturebox_set_pos(picbox, 120, 82);                                  // 设置双发进度条位置为(120, 82)  
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);  // 只在双发模式时显示此进度条  
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG1_NUM);                             // 设置组件ID为双发进度条标识

    //创建功率调节按钮，
    compo_button_t *btn3 = compo_button_create(frm);                       // 创建增加功率按钮（+按钮）  
    compo_button_set_location(btn3, 173, 161, 64, 64);                     // 设置按钮位置(173,161)和大小(64x64像素)  
    compo_button_set_visible(btn3, true);                                  // 设置按钮为可见状态  
    compo_setid(btn3, COMPO_ID_BTN_ECIG_SET_ADD);                          // 设置按钮ID为增加功率按钮标识  
    
    compo_button_t *btn4 = compo_button_create(frm);                       // 创建减少功率按钮（-按钮）  
    compo_button_set_location(btn4, 64, 161, 64, 64);                      // 设置按钮位置(64,161)和大小(64x64像素)  
    compo_button_set_visible(btn4, true);                                  // 设置按钮为可见状态  
    compo_setid(btn4, COMPO_ID_BTN_ECIG_SET_DEL);                          // 设置按钮ID为减少功率按钮标识

    //创建按钮反馈图标
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_ADD_BIN);    // 创建增加按钮的反馈图标  
    compo_picturebox_set_pos(picbox, 173, 161);                            // 设置反馈图标位置，与增加按钮重叠  
    compo_picturebox_set_visible(picbox, false);                           // 初始状态设为不可见  
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_ADD1);                       // 设置组件ID为增加按钮反馈图标  
    
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_REDUCE_BIN); // 创建减少按钮的反馈图标  
    compo_picturebox_set_pos(picbox, 64, 161);                             // 设置反馈图标位置，与减少按钮重叠  
    compo_picturebox_set_visible(picbox, false);                           // 初始状态设为不可见  
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_DEL1);                       // 设置组件ID为减少按钮反馈图标

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_ADD_BIN);
    compo_picturebox_set_pos(picbox, 173, 161);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DUAL_ICON_REDUCE_BIN);
    compo_picturebox_set_pos(picbox, 64, 161);

    return frm;
}

/**  
  * @brief  处理电子烟功率设置界面的触摸按钮事件  
  * @param  None  
  * @retval None  
  */  
static void func_ecig_set_power_button_touch_handle(void)  
{  
    int id = compo_get_button_id();                                 // 获取被点击的按钮ID  
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;        // 获取功率设置功能控制块指针  
    uint8_t res = ecig_get_res();                                   // 获取当前电阻值  
  
    // 获取界面中各个图片框组件的指针  
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D1);     // 双发选中状态图标  
    compo_picturebox_t *picbox1 = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_D_D1);    // 单发选中状态图标  
    compo_picturebox_t *picbox2 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_NUM);      // 单发进度条  
    compo_picturebox_t *picbox3 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG1_NUM);     // 双发进度条  
    compo_picturebox_t *picbox4 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_S_D);      // 双发背景图标  
    compo_picturebox_t *picbox5 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_D_D);      // 单发背景图标  
    compo_picturebox_t *picbox_add = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_ADD1); // 增加按钮反馈图标  
    compo_picturebox_t *picbox_del = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_DEL1); // 减少按钮反馈图标  
      
    switch (id)                                                     // 根据按钮ID执行相应操作  
    {  
        case COMPO_ID_BTN_ECIG_SET_S_D:                            // 双发模式按钮被点击  
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1); // 启动震动马达反馈  
#if ECIG_POWER_CONTROL                                             // 如果启用电子烟功率控制  
            test_2st_gear_func();                                   // 调用双发档位设置函数  
#endif  
            // 切换界面显示状态：隐藏单发相关，显示双发相关  
            compo_picturebox_set_visible(picbox1, false);           // 隐藏单发选中图标  
            compo_picturebox_set_visible(picbox, true);             // 显示双发选中图标  
            compo_picturebox_set_visible(picbox2, false);           // 隐藏单发进度条  
            compo_picturebox_set_visible(picbox3, true);            // 显示双发进度条  
            compo_picturebox_set_visible(picbox5, false);           // 隐藏单发背景  
            compo_picturebox_set_visible(picbox4, true);            // 显示双发背景  
              
            // 设置双发模式的默认功率参数  
            f_setpower->current_index = 25 - MIN_POWER_DOUBLE;      // 计算进度条索引（25W - 双发最小功率）  
            if (f_setpower->current_index > INDEX_MAX_DOUBLE)       // 边界检查  
            {  
                f_setpower->current_index = INDEX_MAX_DOUBLE;       // 限制在最大索引范围内  
            }  
            f_setpower->current_power = 25;                         // 设置默认功率为25W  
              
            // 更新双发进度条显示  
            compo_picturebox_set(picbox3,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN); // 设置进度条图标  
            compo_picturebox_cut(picbox3, f_setpower->current_index, 11);            // 切割显示当前进度（总共11段）  
            break;  
  
        case COMPO_ID_BTN_ECIG_SET_D_D:                            // 单发模式按钮被点击  
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1); // 启动震动马达反馈  
#if ECIG_POWER_CONTROL                                             // 如果启用电子烟功率控制  
            test_1st_gear_func();                                   // 调用单发档位设置函数  
#endif  
            // 切换界面显示状态：隐藏双发相关，显示单发相关  
            compo_picturebox_set_visible(picbox, false);            // 隐藏双发选中图标  
            compo_picturebox_set_visible(picbox1, true);            // 显示单发选中图标  
            compo_picturebox_set_visible(picbox3, false);           // 隐藏双发进度条  
            compo_picturebox_set_visible(picbox2, true);            // 显示单发进度条  
            compo_picturebox_set_visible(picbox4, false);           // 隐藏双发背景  
            compo_picturebox_set_visible(picbox5, true);            // 显示单发背景  
              
            // 设置单发模式的默认功率参数  
            f_setpower->current_index = 15 - MIN_POWER_SINGLE;      // 计算进度条索引（15W - 单发最小功率）  
            f_setpower->current_power = 15;                         // 设置默认功率为15W  
              
            // 根据电阻值选择不同的进度条图标和段数  
            if(res==8||res==4||res==0)                              // 特定电阻值（8Ω、4Ω、0Ω）  
            {  
                compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN); // 使用单发专用进度条  
                compo_picturebox_cut(picbox2, f_setpower->current_index, 9);               // 切割显示（9段）  
            }  
            else                                                    // 其他电阻值  
            {  
                compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);   // 使用双发进度条  
                compo_picturebox_cut(picbox2, f_setpower->current_index, 11);              // 切割显示（11段）  
            }  
              
            ecig_set_power(f_setpower->current_power);              // 设置硬件功率  
            break;  
  
        case COMPO_ID_BTN_ECIG_SET_ADD:                            // 增加功率按钮被点击  
            if (get_gear_func() == 0 && (res==4||res==8||res==0))  // 单发模式且特定电阻值  
            {  
                if (f_setpower->current_index < INDEX_MAX_SINGLE)   // 检查是否未达到单发最大值  
                {  
                    f_setpower->current_index++;                    // 进度条索引递增  
                    f_setpower->current_power++;                    // 功率值递增  
                    ecig_set_power(f_setpower->current_power);      // 设置硬件功率  
                    compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN); // 更新进度条图标  
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 9);               // 更新进度显示  
                }  
            }  
            else if (get_gear_func() == 0 && (res==6||res==12))    // 单发模式且其他电阻值  
            {  
                if (f_setpower->current_index < INDEX_MAX_DOUBLE)   // 检查是否未达到双发最大值  
                {  
                    f_setpower->current_index++;                    // 进度条索引递增  
                    f_setpower->current_power++;                    // 功率值递增  
                    ecig_set_power(f_setpower->current_power);      // 设置硬件功率  
                    compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);   // 更新进度条图标  
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 11);              // 更新进度显示  
                }  
            }  
            else                                                    // 双发模式  
            {  
                if (f_setpower->current_index < INDEX_MAX_DOUBLE)   // 检查是否未达到双发最大值  
                {  
                    f_setpower->current_index++;                    // 进度条索引递增  
                    f_setpower->current_power++;                    // 功率值递增  
                    ecig_set_power(f_setpower->current_power);      // 设置硬件功率  
                    compo_picturebox_set(picbox3,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);   // 更新双发进度条  
                    compo_picturebox_cut(picbox3, f_setpower->current_index, 11);              // 更新进度显示  
                }  
            }  
            break;  
  
        case COMPO_ID_BTN_ECIG_SET_DEL:                            // 减少功率按钮被点击  
            if (get_gear_func() == 0 && (res==4||res==8||res==0))  // 单发模式且特定电阻值  
            {  
                if (f_setpower->current_index > 0)                  // 检查是否大于最小值  
                {  
                    f_setpower->current_index--;                    // 进度条索引递减  
                    f_setpower->current_power--;                    // 功率值递减  
                    ecig_set_power(f_setpower->current_power);      // 设置硬件功率  
                    compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN); // 更新进度条图标  
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 9);               // 更新进度显示  
                }  
            }  
            else if (get_gear_func() == 0 && (res==6||res==12))    // 单发模式且其他电阻值  
            {  
                if (f_setpower->current_index > 0)                  // 检查是否大于最小值  
                {  
                    f_setpower->current_index--;                    // 进度条索引递减  
                    f_setpower->current_power--;                    // 功率值递减  
                    ecig_set_power(f_setpower->current_power);      // 设置硬件功率  
                    compo_picturebox_set(picbox2,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);   // 更新进度条图标  
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 11);              // 更新进度显示  
                }  
            }  
            else                                                    // 双发模式  
            {  
                if (f_setpower->current_index > 0)                  // 检查是否大于最小值  
                {  
                    f_setpower->current_index--;                    // 进度条索引递减  
                    f_setpower->current_power--;                    // 功率值递减  
                    ecig_set_power(f_setpower->current_power);      // 设置硬件功率  
                    compo_picturebox_set(picbox3,UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);   // 更新双发进度条  
                    compo_picturebox_cut(picbox3, f_setpower->current_index, 11);              // 更新进度显示  
                }  
            }  
            break;  
  
        default:                                                    // 未处理的按钮ID  
            break;  
    }  
      
    // 保存当前设置并写入配置文件  
    save_power_and_index(f_setpower->current_power, f_setpower->current_index);     // 保存功率和索引到全局数据  
    uteModuleSmokeDataSaveConfig();                                 // 将配置保存到存储器  
}


/**  
  * @brief  刷新电子烟功率设置界面显示（根据当前模式和电阻值更新界面）  
  * @param  None  
  * @retval None
  * 这个刷新函数的主要作用是：
    状态检查：  检查功能控制块指针有效性，获取当前电阻值
    组件获取：  通过ID获取界面中所有相关的图片框组件
    模式切换：  根据当前档位（单发/双发）显示或隐藏对应的UI组件
    进度条更新：根据电阻值和模式选择合适的进度条图标，并切割显示当前功率进度
    默认值设置：为不同模式设置相应的默认功率值
    这个函数通常在模式切换或外部状态变化时被调用，确保界面显示与当前的电子烟状态保持同步。在代码中可以看到它被 调用，用于处理语音控制的模式切换。
  *   
  */  
void func_ecig_set_power_refresh(void)  
{  
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;        // 获取功率设置功能控制块指针  
    if (!f_setpower) return;                                        // 空指针检查，防止程序崩溃  
  
    uint8_t res = ecig_get_res();                                   // 获取当前电阻值，用于判断进度条类型  
      
    // 获取界面中各个组件的指针  
    compo_picturebox_t *single_mode = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_D_D1);    // 单发模式选中图标  
    compo_picturebox_t *double_mode = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D1);    // 双发模式选中图标  
    compo_picturebox_t *single_progress = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_NUM);  // 单发模式进度条  
    compo_picturebox_t *double_progress = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG1_NUM); // 双发模式进度条  
    compo_picturebox_t *single_bg = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_D_D);        // 单发模式背景图标  
    compo_picturebox_t *double_bg = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_S_D);        // 双发模式背景图标  
  
    // 根据当前模式（单发/双发）更新界面显示  
    if (get_gear_func() == 0)                                       // 单发模式（档位函数返回0）  
    {  
        // 显示单发模式相关组件，隐藏双发模式组件  
        compo_picturebox_set_visible(single_mode, true);            // 显示单发选中状态图标  
        compo_picturebox_set_visible(double_mode, false);           // 隐藏双发选中状态图标  
        compo_picturebox_set_visible(single_progress, true);        // 显示单发进度条  
        compo_picturebox_set_visible(double_progress, false);       // 隐藏双发进度条  
        compo_picturebox_set_visible(single_bg, true);              // 显示单发背景图标  
        compo_picturebox_set_visible(double_bg, false);             // 隐藏双发背景图标  
  
        // 根据电阻值选择进度条图标类型和段数  
        if (res == 8 || res == 0)                                   // 电阻值为8Ω或0Ω时  
        {  
            compo_picturebox_set(single_progress, UI_BUF_I330001_POWER1_SINGLE_ICON_POWER_BIN); // 使用单发专用进度条图标  
            compo_picturebox_cut(single_progress, f_setpower->current_index, 9);                // 切割显示当前进度（9段）  
        }  
        else                                                         // 其他电阻值  
        {  
            compo_picturebox_set(single_progress, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);   // 使用双发进度条图标  
            compo_picturebox_cut(single_progress, f_setpower->current_index, 11);               // 切割显示当前进度（11段）  
        }  
        f_setpower->current_power = 15;                              // 设置单发模式默认功率为15W  
    }  
    else                                                             // 双发模式（档位函数返回1）  
    {  
        // 显示双发模式相关组件，隐藏单发模式组件  
        compo_picturebox_set_visible(double_mode, true);            // 显示双发选中状态图标  
        compo_picturebox_set_visible(single_mode, false);           // 隐藏单发选中状态图标  
        compo_picturebox_set_visible(double_progress, true);        // 显示双发进度条  
        compo_picturebox_set_visible(single_progress, false);       // 隐藏单发进度条  
        compo_picturebox_set_visible(double_bg, true);              // 显示双发背景图标  
        compo_picturebox_set_visible(single_bg, false);             // 隐藏单发背景图标  
  
        // 双发模式统一使用双发进度条图标  
        compo_picturebox_set(double_progress, UI_BUF_I330001_POWER1_DUAL_ICON_POWER_BIN);      // 设置双发进度条图标  
        compo_picturebox_cut(double_progress, f_setpower->current_index, 11);                  // 切割显示当前进度（11段）  
        f_setpower->current_power = 25;                             // 设置双发模式默认功率为25W  
    }  
      
    // 以下为注释掉的调试代码，用于加载和打印功率信息  
    // load_power_and_index(&uteModuleSmokeData.current_power, &uteModuleSmokeData.current_index);  
    // printf("2222uteModuleSmokeData.current_power=%d，ecig_get_power()=%d\\r\\n",uteModuleSmokeData.current_power,ecig_get_power());  
}
/**  
  * @brief  处理电子烟功率设置界面的按钮释放事件和视觉反馈  
  *         管理按钮的视觉反馈效果，确保按钮反馈图标在按下后显示300ms然后自动隐藏
  * @param  None  
  * @retval None  
  */  
static void func_ecig_set_power_button_release_handle()  
{  
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;        // 获取功率设置功能控制块指针  
  
    s32 dx, dy;                                                     // 声明触摸偏移量变量  
    compo_picturebox_t *picbox_add = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_ADD1); // 获取增加按钮反馈图标  
    compo_picturebox_t *picbox_del = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_DEL1); // 获取减少按钮反馈图标  
  
    // 检查按钮反馈显示时间是否超时  
    if (f_setpower->tick <= tick_get() - BTN_REST_DISP_TIME)        // 如果当前时间减去记录时间大于等于显示时间(300ms)  
    {  
        compo_picturebox_set_visible(picbox_add, false);            // 隐藏增加按钮反馈图标  
        compo_picturebox_set_visible(picbox_del, false);            // 隐藏减少按钮反馈图标  
    }  
  
    int id = compo_get_button_id();                                 // 获取当前被按下的按钮ID  
    if (id == 0 || !ctp_get_dxy(&dx, &dy))                          // 如果没有按钮被按下或触摸屏无偏移量  
        return;                                                     // 直接返回，不处理  
  
    f_setpower->tick = tick_get();                                  // 更新时间戳，用于控制反馈图标显示时间  
  
    switch (id)                                                     // 根据按钮ID显示对应的反馈图标  
    {  
        case COMPO_ID_BTN_ECIG_SET_ADD:                             // 增加功率按钮被按下  
            compo_picturebox_set_visible(picbox_add, true);         // 显示增加按钮反馈图标  
            break;    
        case COMPO_ID_BTN_ECIG_SET_DEL:                             // 减少功率按钮被按下  
            compo_picturebox_set_visible(picbox_del, true);         // 显示减少按钮反馈图标  
            break;  
    }  
}

/**  
  * @brief  处理电子烟功率设置界面的消息事件  
  * @param  msg: 接收到的消息类型  
  * @retval None  
  */  
static void func_ecig_set_power_message(size_msg_t msg)  
{  
    switch (msg)                                                    // 根据消息类型执行相应处理  
    {  
        case MSG_CTP_CLICK:                                        // 触摸屏点击消息  
            func_ecig_set_power_button_touch_handle();              // 调用按钮触摸处理函数  
            break;  
          
        // 以下为注释掉的滑动手势处理代码  
        // case MSG_CTP_SHORT_LEFT:                                // 短距离左滑  
        // case MSG_CTP_SHORT_UP:                                  // 短距离上滑  
        // case MSG_CTP_SHORT_RIGHT:                               // 短距离右滑  
        // case MSG_CTP_SHORT_DOWN:                                // 短距离下滑  
        //     // func_menu_sub_drag_move();                       // 调用菜单拖拽移动函数  
        //     break;  
  
        case MSG_CTP_LONG:                                         // 触摸屏长按消息  
            break;                                                  // 暂时不处理长按事件  
  
        default:                                                    // 其他消息类型  
            func_message(msg);                                      // 调用通用消息处理函数  
            break;  
    }  
}

/**  
  * @brief  电子烟功率设置界面的主循环处理函数  
  * @param  None  
  * @retval None  
  */  
static void func_ecig_set_power_process(void)  
{  
    func_ecig_set_power_button_release_handle();                   // 处理按钮释放事件和视觉反馈  
    func_process();                                                 // 调用通用功能处理函数  
  
    // 新增：检测模式是否变化，变化时强制刷新界面  
    static int last_gear = -1;                                     // 静态变量，记录上一次的档位模式  
      
    // 以下为注释掉的模式变化检测代码  
    // int current_gear = get_gear_func();                         // 获取当前档位模式  
    // if (last_gear != current_gear) {                            // 如果模式发生变化  
    //     func_ecig_set_power_refresh();                          // 调用界面刷新函数  
    //     last_gear = current_gear;                               // 更新记录的模式值  
    // }  
}

/**  
  * @brief  进入电子烟功率设置功能，初始化相关资源  
  * @param  None  
  * @retval None  
  */  
static void func_ecig_set_power_enter(void)  
{  
    func_cb.f_cb = func_zalloc(sizeof(f_setpower_t));               // 分配功率设置控制块内存并清零  
    func_cb.frm_main = func_ecig_set_power_form_create();           // 创建功率设置界面窗体  
    get_gear_func();                                                // 获取当前档位模式（单发/双发）  
    ecig_get_res();                                                 // 获取当前电阻值  
}

/**  
  * @brief  退出电子烟功率设置功能，清理资源并保存配置  
  * @param  None  
  * @retval None  
  */  
static void func_ecig_set_power_exit(void)  
{  
    if (func_cb.f_cb != NULL)                                       // 检查控制块指针是否有效  
    {  
        f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;    // 获取功率设置控制块指针  
        save_power_and_index(f_setpower->current_power, f_setpower->current_index); // 保存当前功率和索引到全局数据  
        func_free(func_cb.f_cb);                                    // 释放控制块内存  
        func_cb.f_cb = NULL;                                        // 清空指针，防止野指针  
    }  
    uteModuleSmokeDataSaveConfig();                                 // 将配置数据保存到存储器  
    func_cb.last = FUNC_ECIG_SET_POWER;                             // 记录最后访问的功能ID  
}
/**  
  * @brief  电子烟功率设置功能主函数，控制整个功能的生命周期  
  * @param  None  
  * @retval None  
  */  
void func_ecig_set_power(void)  
{  
    printf("%s\\n", __func__);                                       // 打印函数名用于调试  
    func_ecig_set_power_enter();                                    // 调用进入函数，初始化资源  
      
    while (func_cb.sta == FUNC_ECIG_SET_POWER)                      // 当功能状态为功率设置时循环运行  
    {  
        func_ecig_set_power_process();                              // 处理按钮释放事件和界面更新  
        func_ecig_set_power_message(msg_dequeue());                 // 处理消息队列中的触摸事件  
    }  
      
    func_ecig_set_power_exit();                                     // 退出时清理资源并保存配置  
}