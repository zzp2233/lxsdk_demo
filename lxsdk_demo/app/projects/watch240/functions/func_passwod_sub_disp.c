#include "include.h"
#include "func.h"
#include "ute_module_lockScreen.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_NUM0 = 1,
    COMPO_ID_BTN_NUM1,
    COMPO_ID_BTN_NUM2,
    COMPO_ID_BTN_NUM3,
    COMPO_ID_BTN_NUM4,
    COMPO_ID_BTN_NUM5,
    COMPO_ID_BTN_NUM6,
    COMPO_ID_BTN_NUM7,
    COMPO_ID_BTN_NUM8,
    COMPO_ID_BTN_NUM9,
    COMPO_ID_BTN_DEL_CLICK,

    //图像
    COMPO_ID_PIC_PASSWORD_ZERO,
    COMPO_ID_PIC_PASSWORD_ONE,
    COMPO_ID_PIC_PASSWORD_TWS,
    COMPO_ID_PIC_PASSWORD_THR,

    //文本
    COMPO_ID_TXT_NEWPASSWORD,

    //数字
    COMPO_ID_NUM_DISP_ZERO,
    COMPO_ID_NUM_DISP_ONE,
    COMPO_ID_NUM_DISP_TWS,
    COMPO_ID_NUM_DISP_THR,
};

typedef struct f_password_sub_disp_t_
{
    u8 value[4];
    u8 cnt;
} f_password_sub_disp_t;

#define PASSWORD_DISP_PIC_ITEM_CNT    ((int)(sizeof(tbl_password_disp_pic_item) / sizeof(tbl_password_disp_pic_item[0])))

typedef struct password_disp_pic_item_t_
{
    u32 res_addr;
    u16 pic_id;
    s16 x;
    s16 y;
    bool visible_en;
} password_disp_pic_item_t;

//图片item，创建时遍历一下
static const password_disp_pic_item_t tbl_password_disp_pic_item[] =
{
    {UI_BUF_I330001_CHILD_LOCK_00_BIN,     COMPO_ID_PIC_PASSWORD_ZERO,        80,    40,    false},
    {UI_BUF_I330001_CHILD_LOCK_00_BIN,     COMPO_ID_PIC_PASSWORD_ONE,         110,    40,    false},
    {UI_BUF_I330001_CHILD_LOCK_00_BIN,     COMPO_ID_PIC_PASSWORD_TWS,         140,    40,    false},
    {UI_BUF_I330001_CHILD_LOCK_00_BIN,     COMPO_ID_PIC_PASSWORD_THR,         170,    40,    false},
};


#define PASSWORD_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_password_disp_btn_item) / sizeof(tbl_password_disp_btn_item[0])))

typedef struct password_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    s16 x;
    s16 y;
} password_disp_btn_item_t;

//按钮item，创建时遍历一下
static const password_disp_btn_item_t tbl_password_disp_btn_item[] =
{
    {UI_BUF_I330001_CHILD_LOCK_NUM_01_BIN,             COMPO_ID_BTN_NUM1,         45,     84},
    {UI_BUF_I330001_CHILD_LOCK_NUM_02_BIN,             COMPO_ID_BTN_NUM2,         120,    84},
    {UI_BUF_I330001_CHILD_LOCK_NUM_03_BIN,             COMPO_ID_BTN_NUM3,         195,    84},
    {UI_BUF_I330001_CHILD_LOCK_NUM_04_BIN,             COMPO_ID_BTN_NUM4,         45,     140},
    {UI_BUF_I330001_CHILD_LOCK_NUM_05_BIN,             COMPO_ID_BTN_NUM5,         120,    140},
    {UI_BUF_I330001_CHILD_LOCK_NUM_06_BIN,             COMPO_ID_BTN_NUM6,         195,    140},
    {UI_BUF_I330001_CHILD_LOCK_NUM_07_BIN,   COMPO_ID_BTN_NUM7,         45,     198},
    {UI_BUF_I330001_CHILD_LOCK_NUM_08_BIN,             COMPO_ID_BTN_NUM8,         120,    198},
    {UI_BUF_I330001_CHILD_LOCK_NUM_09_BIN, COMPO_ID_BTN_NUM9,         195,    198},
    {UI_BUF_I330001_CHILD_LOCK_NUM_00_BIN,   COMPO_ID_BTN_NUM0,         120,    252},
    {UI_BUF_I330001_CHILD_LOCK_DELETE_BIN, COMPO_ID_BTN_DEL_CLICK,    195,    252},
};

typedef struct password_num_item_t_
{
    u32 res_addr;
    int num_cnt;
    u16 num_id;
    int val;
    s16 x;
    s16 y;
    bool zfill_en;
    bool visible_en;
} password_num_item_t;

#define PASSWORD_NUM_ITEM_CNT                       1


//创建密码--开启密码锁显示窗体
compo_form_t *func_password_sub_disp_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 6);
    compo_setid(txt, COMPO_ID_TXT_NEWPASSWORD);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, 50, 34);
    compo_textbox_set(txt, i18n[STR_ENTER_CUR]);
    compo_textbox_set_visible(txt, true);

    if(uteModulePasswordData.password_cnt == 4 && uteModulePasswordData.password_change)
    {
        compo_textbox_set(txt, i18n[STR_OLD_PASSWORD]);
    }
    else if(uteModulePasswordData.password_cnt == 0 && !uteModulePasswordData.password_change)
    {
        compo_textbox_set(txt, i18n[STR_NEW_PASSWORD]);
    }

    //新建图像
    compo_picturebox_t *pic_click;
    for (u8 idx = 0; idx < PASSWORD_DISP_PIC_ITEM_CNT; idx++)
    {
        pic_click = compo_picturebox_create(frm, tbl_password_disp_pic_item[idx].res_addr);
        compo_setid(pic_click, tbl_password_disp_pic_item[idx].pic_id);
        compo_picturebox_set_pos(pic_click, tbl_password_disp_pic_item[idx].x, tbl_password_disp_pic_item[idx].y);
        compo_picturebox_set_visible(pic_click, tbl_password_disp_pic_item[idx].visible_en);
    }

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < PASSWORD_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_password_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn, tbl_password_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_password_disp_btn_item[idx_btn].x, tbl_password_disp_btn_item[idx_btn].y);
    }

    //创建数字
    s16 txt_num_x = 70;
    s16 txt_num_y = 10;
    s16 txt_num_x_offset = 30;
    char buf[13];
    compo_textbox_t *txt_num;
    for (u8 idx = 0; idx < PASSWORD_NUM_ITEM_CNT; idx++)
    {
        txt_num = compo_textbox_create(frm, 7);
        compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_38_BIN);
        compo_setid(txt_num, COMPO_ID_NUM_DISP_ZERO + idx);
        compo_textbox_set_align_center(txt_num, false);
        compo_textbox_set_pos(txt_num, txt_num_x + (idx * txt_num_x_offset), txt_num_y);
        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%d%d%d%d",
                 uteModulePasswordData.password_value[0],
                 uteModulePasswordData.password_value[1],
                 uteModulePasswordData.password_value[2],
                 uteModulePasswordData.password_value[3]);
        compo_textbox_set(txt_num, buf);
        compo_textbox_set_visible(txt_num, false);
    }

    return frm;
}

//开启密码锁功能事件处理
static void func_password_sub_disp_process(void)
{
    func_process();
}
void debug_print_password_info()
{
    printf("System password count: %d\n", uteModulePasswordData.password_cnt);
    printf("System password value: ");
    for(int i = 0; i < uteModulePasswordData.password_cnt; i++)
    {
        printf("%d ", uteModulePasswordData.password_value[i]);
    }
    printf("\n");
}
//单击按钮
// 单击按钮
static void func_password_sub_disp_button_click(void)
{
    int id = compo_get_button_id();
    f_password_sub_disp_t *password = (f_password_sub_disp_t *)func_cb.f_cb;
    u8 psd_ctn = 0;
    char buf[13];
    // 获取数字组件的地址
    compo_textbox_t *txt_num  = compo_getobj_byid(COMPO_ID_NUM_DISP_ZERO);
    // 获取图片组件的地址
    compo_picturebox_t *pic_zer = compo_getobj_byid(COMPO_ID_PIC_PASSWORD_ZERO);
    compo_picturebox_t *pic_one = compo_getobj_byid(COMPO_ID_PIC_PASSWORD_ONE);
    compo_picturebox_t *pic_tws = compo_getobj_byid(COMPO_ID_PIC_PASSWORD_TWS);
    compo_picturebox_t *pic_thr = compo_getobj_byid(COMPO_ID_PIC_PASSWORD_THR);

    compo_textbox_t *txt  = compo_getobj_byid(COMPO_ID_TXT_NEWPASSWORD);
    printf("Button clicked: id = %d\n", id); // 添加调试信息
    switch (id)
    {
        case COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM9:

            compo_textbox_set_visible(txt, false);
            if(password->cnt < 4)
            {
                // 确保索引不会越界
                if (password->cnt >= 0 && password->cnt < 4)
                {
                    password->value[password->cnt++] = id - 1;
                }
                else
                {
                    printf("Invalid password count: %d\n", password->cnt);
                }
            }
            compo_textbox_set_visible(txt_num, true);
            printf("Password count after num click: %d\n", password->cnt); // 添加调试信息
            break;

        case COMPO_ID_BTN_DEL_CLICK:
            compo_textbox_set_visible(txt, false);
            if(password->cnt > 0)
            {
                password->cnt--;
            }
            printf("Password count after del click: %d\n", password->cnt); // 添加调试信息
            break;

        default:
            compo_textbox_set_visible(txt, false);
            break;
    }
// 新增状态检查提前返回
    if (func_cb.sta != FUNC_PASSWORD_SUB_DISP)
    {
        return; // 状态已改变时立即退出
    }
    printf("Checking password count: %d\n", password->cnt); // 添加调试信息
    if(password->cnt == 1)
    {
        compo_picturebox_set_visible(pic_zer, false);
        compo_picturebox_set_visible(pic_one, true);
        compo_picturebox_set_visible(pic_tws, true);
        compo_picturebox_set_visible(pic_thr, true);

        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%d", password->value[0]);
        compo_textbox_set(txt_num, buf);
        printf("Password count is 1, updated textbox\n"); // 添加调试信息
    }
    else if(password->cnt == 2)
    {
        compo_picturebox_set_visible(pic_zer, false);
        compo_picturebox_set_visible(pic_one, false);
        compo_picturebox_set_visible(pic_tws, true);
        compo_picturebox_set_visible(pic_thr, true);

        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%d%d", password->value[0], password->value[1]);
        compo_textbox_set(txt_num, buf);
        printf("Password count is 2, updated textbox\n"); // 添加调试信息
    }
    else if(password->cnt == 3)
    {
        compo_picturebox_set_visible(pic_zer, false);
        compo_picturebox_set_visible(pic_one, false);
        compo_picturebox_set_visible(pic_tws, false);
        compo_picturebox_set_visible(pic_thr, true);

        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%d%d%d", password->value[0], password->value[1], password->value[2]);
        compo_textbox_set(txt_num, buf);
        printf("Password count is 3, updated textbox\n"); // 添加调试信息
    }
    else if(password->cnt == 4)
    {
        compo_picturebox_set_visible(pic_zer, false);
        compo_picturebox_set_visible(pic_one, false);
        compo_picturebox_set_visible(pic_tws, false);
        compo_picturebox_set_visible(pic_thr, false);

        memset(buf, 0, sizeof(buf));
        snprintf(buf, sizeof(buf), "%d%d%d%d", password->value[0], password->value[1], password->value[2], password->value[3]);
        compo_textbox_set(txt_num, buf);
        printf("Password count is 4, updated textbox\n"); // 添加调试信息
        debug_print_password_info();
        if(uteModulePasswordData.password_cnt == 0 && password->cnt == 4 && !uteModulePasswordData.password_change)
        {
            uteModulePasswordData.password_cnt = password->cnt;
            for(int i = 0; i < password->cnt; i++)
            {
                uteModulePasswordData.password_value[i] = password->value[i];
            }
            uteModulePasswordDataSaveConfig();
            func_cb.sta = FUNC_PASSWORD_SUB_SELECT;
            printf("Case 1: Updated password and changed state\n"); // 添加调试信息
            // 新增：清除UI状态后立即返回
            compo_picturebox_set_visible(pic_zer, false);
            compo_picturebox_set_visible(pic_one, false);
            compo_picturebox_set_visible(pic_tws, false);
            compo_picturebox_set_visible(pic_thr, false);
            compo_textbox_set(txt_num, "");
            return; // 关键修改：状态改变后立即退出
        }
        else if(uteModulePasswordData.password_cnt == 4 && password->cnt == 4 && !uteModulePasswordData.password_change)
        {
            psd_ctn = 0;
            // 使用独立循环进行验证
            for(int j = 0; j < 4; j++)
            {
                if(uteModulePasswordData.password_value[j] == password->value[j])
                {
                    psd_ctn++;
                }
            }
            // 在循环结束后判断
            if(psd_ctn == 4)
            {
                uteModulePasswordData.password_cnt = 0;
                func_cb.sta = FUNC_SET_SUB_PASSWORD;
                printf("Password matched, changed state\n");
                return; // 立即退出
            }
        }
        else if(uteModulePasswordData.password_cnt == 0 && password->cnt == 4 && uteModulePasswordData.password_change)
        {
            uteModulePasswordData.password_cnt = password->cnt;
            for(int i = 0; i < password->cnt; i++)
            {
                uteModulePasswordData.password_value[i] = password->value[i];
            }
            // 新增：调用保存函数
            uteModulePasswordDataSaveConfig();
            func_cb.sta = FUNC_PASSWORD_SUB_SELECT;
            printf("Case 3: Updated password and changed state\n"); // 添加调试信息
        }
        else if(uteModulePasswordData.password_cnt == 4 && password->cnt == 4 && uteModulePasswordData.password_change)
        {
            for(int j = 0; j < password->cnt; j++)
            {
                if(uteModulePasswordData.password_value[j] == password->value[j])
                {
                    psd_ctn++;
                }
            }
            printf("Case 4: Calculated match count: %d\n", psd_ctn); // 添加调
            if(psd_ctn == 4)
            {
                // 原密码验证通过，准备输入新密码
                uteModulePasswordData.password_cnt = 0;
                password->cnt = 0;
                compo_textbox_set(txt, i18n[STR_NEW_PASSWORD]);
                compo_textbox_set_visible(txt, true);
                printf("Password matched, reset and updated textbox\n");
            }
        }
        compo_picturebox_set_visible(pic_zer, false);
        compo_picturebox_set_visible(pic_one, false);
        compo_picturebox_set_visible(pic_tws, false);
        compo_picturebox_set_visible(pic_thr, false);

        compo_textbox_set(txt_num, "");

        if(psd_ctn == 4)
        {
            // 密码验证通过
            if(uteModulePasswordData.password_change)
            {
                // 修改密码流程：标记为待保存新密码
                uteModulePasswordData.password_change = 0;
                // 注意：新密码将在用户输入后保存（在上面的场景3中处理）
            }
            else
            {
                // 其他场景：保持逻辑不变
                uteModulePasswordData.password_cnt = 0;
                password->cnt = 0;
                compo_textbox_set(txt, i18n[STR_NEW_PASSWORD]);
                compo_textbox_set_visible(txt, true);
                printf("Password matched, reset and updated textbox\n");
            }
        }
        else
        {
            compo_textbox_set(txt, i18n[STR_ERROR_PASSWORD]);
            compo_textbox_set_visible(txt, true);
            password->cnt = 0;
            printf("Password mismatched, reset and updated textbox\n"); // 添加调试信息
        }
    }
    else
    {
        compo_picturebox_set_visible(pic_zer, true);
        compo_picturebox_set_visible(pic_one, true);
        compo_picturebox_set_visible(pic_tws, true);
        compo_picturebox_set_visible(pic_thr, true);

        compo_textbox_set(txt_num, "");
        printf("Password count is not 1-4, reset textbox and pictures\n"); // 添加调试信息
    }
}

//开启密码锁功能消息处理
static void func_password_sub_disp_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_password_sub_disp_button_click();
            break;

        case MSG_CTP_SHORT_RIGHT:
            if(uteModulePasswordData.password_change && uteModulePasswordData.password_cnt == 0)
            {
                uteModulePasswordData.password_cnt = 4;
                uteModulePasswordData.password_flag = true;
                // ecig.clock_flag = 1;
                // printf("111111111111111111111111111111uteModulePasswordData.password_flag = 1\n");
            }
            else
            {
                uteModulePasswordData.password_flag = false;
                // ecig.clock_flag = 0;
                //  printf("2222222222222222222222222222222uteModulePasswordData.password_flag = 0\n");
            }

            func_message(msg);
            break;

        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        default:
            func_message(msg);
            break;
    }
    uteModulePasswordDataSaveConfig();
}

//进入开启密码锁功能
static void func_password_sub_disp_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_password_sub_disp_t));
    func_cb.frm_main = func_password_sub_disp_form_create();
}

//退出开启密码锁功能
static void func_password_sub_disp_exit(void)
{
    f_password_sub_disp_t *password = (f_password_sub_disp_t *)func_cb.f_cb;
    func_cb.last = FUNC_PASSWORD_SUB_DISP;
    password->cnt = 0;
}

//开启密码锁功能
void func_password_sub_disp(void)
{
    printf("%s\n", __func__);
    func_password_sub_disp_enter();
    while (func_cb.sta == FUNC_PASSWORD_SUB_DISP)
    {
        func_password_sub_disp_process();
        func_password_sub_disp_message(msg_dequeue());
    }
    func_password_sub_disp_exit();
}
