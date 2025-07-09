#include "include.h"
#include "func.h"
#include "func_bt.h"


#define TXT_X_MIN 20
enum
{
    COMPO_ID_TXT_NUMBER = 0xff,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_BTN_ANSWER,
    COMPO_ID_BTN_REJECT,
};

typedef struct f_bt_call_t_
{
    char pbap_result_Name[50];//存放来电与接听联系人名字
    char tmp_pbap_result_Name[50];//存放来电与接听联系人名字
} f_bt_ring_t;

void func_bt_ring_number_update(void)
{
    if (bt_cb.number_sta)
    {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
        bt_pbap_lookup_number((char*)hfp_get_last_call_number(0));
    }
}
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    //printf("%s\n", __func__);
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);

    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 74-widget_text_get_height()/2, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    compo_textbox_set_align_center(name_txt, false);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_height()/2);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 106-widget_text_get_height()/2, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);
    compo_textbox_set_align_center(number_txt, false);
    txt_leng = widget_text_get_area(number_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    compo_textbox_set_pos(number_txt,txt_x,106-widget_text_get_height()/2);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
//    compo_setid(txt, COMPO_ID_TXT_IN_CALL);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X,138-widget_text_get_height()/2,GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);
    compo_textbox_set_align_center(txt, false);
    txt_leng = widget_text_get_area(txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    compo_textbox_set_pos(txt,txt_x,138-widget_text_get_height()/2);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_CALL_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, 240);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, 240);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_IN_CALL]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_textbox_set(txt, i18n[STR_IN_CALL]);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_CALL_CALLING_CALL_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, 196+98/2, 279);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, 60-98/2, 279);


    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT


// 判断从指定位置开始的字节序列是否是一个完整的UTF - 8字符
static int is_complete_utf8_char(const char *str, int pos)
{
    unsigned char byte = str[pos];
    if ((byte & 0x80) == 0)
    {
        // 单字节字符，肯定是完整的
        return 1;
    }
    int num_bytes = 0;
    if ((byte & 0xE0) == 0xC0)
    {
        num_bytes = 2;
    }
    else if ((byte & 0xF0) == 0xE0)
    {
        num_bytes = 3;
    }
    else if ((byte & 0xF8) == 0xF0)
    {
        num_bytes = 4;
    }
    for (int i = 1; i < num_bytes; i++)
    {
        if ((str[pos + i] & 0xC0)!= 0x80)
        {
            return 0;
        }
    }
    return 1;
}

// 截取有用数据并在尾部补上三个...
void truncate_and_append(const char *src, char *dst, int dst_size)
{
    int i = 0;
    int j = 0;
    while (i < strlen(src) && j < dst_size - 1)
    {
        if (is_complete_utf8_char(src, i))
        {
            dst[j++] = src[i++];
        }
        else
        {
            break;
        }
    }
    if (j < dst_size - 3)
    {
        if (strlen(src) > PBAP_MAX_NAME_LEN)
        {
            dst[j++] = '.';
            dst[j++] = '.';
            dst[j++] = '.';
        }
    }
    dst[j] = '\0';
}

void func_bt_ring_up_date_process(void)
{
//    compo_textbox_t *name_txt  = compo_getobj_byid(COMPO_ID_TXT_NAME);
//    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);

    f_bt_ring_t *f_bt_ring = (f_bt_ring_t*)func_cb.f_cb;

    if(strcmp(f_bt_ring->pbap_result_Name, sys_cb.pbap_result_Name)!=0)
    {
        s16 txt_leng;
        s16 txt_x=0;

        memcpy(f_bt_ring->pbap_result_Name, sys_cb.pbap_result_Name, 50);

        memset(f_bt_ring->tmp_pbap_result_Name, '\0', sizeof(f_bt_ring->tmp_pbap_result_Name));
        truncate_and_append(sys_cb.pbap_result_Name, f_bt_ring->tmp_pbap_result_Name, sizeof(f_bt_ring->tmp_pbap_result_Name));

        compo_textbox_t *name_txt  = compo_getobj_byid(COMPO_ID_TXT_NAME);
        compo_textbox_set(name_txt, f_bt_ring->tmp_pbap_result_Name);

        txt_leng = widget_text_get_area(name_txt->txt).wid;
        txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
        if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
        compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_height()/2);
    }

}
void func_bt_ring_process(void)
{
    func_bt_ring_up_date_process();
    bsp_bt_ring_process();

#if !CALL_MGR_EN
    if(bt_cb.disp_status != BT_STA_INCOMING)  //退出来电页面
    {
        func_directly_back_to();
        if (bt_cb.disp_status == BT_STA_OUTGOING || bt_cb.disp_status == BT_STA_INCALL)
        {
            func_cb.sta = FUNC_BT_CALL;
        }
    }
#endif
}

static void func_bt_ring_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_ANSWER:
            printf("COMPO_ID_BTN_ANSWER\n");
            // 初始化麦克风音频路径
            audio_path_init(AUDIO_PATH_BTMIC);
            audio_path_start(AUDIO_PATH_BTMIC);
            bt_call_answer_incoming();
            delay_ms(100);
            break;

        case COMPO_ID_BTN_REJECT:
            printf("COMPO_ID_BTN_REJECT\n");
            bt_call_terminate();
            break;

        default:
            break;
    }

}

//蓝牙通话消息处理
static void func_bt_ring_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_bt_ring_click();                         //单击按钮
            break;

        case EVT_CALL_NUMBER_UPDATE:
            func_bt_ring_number_update();
            break;

        case MSG_SYS_500MS:
            reset_sleep_delay_all();                           //来电不休眠
            break;

        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_SHORT_LEFT:
            break;

        default:
            func_message(msg);
            break;
    }
}

void func_bt_ring_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_bt_ring_t));
    func_cb.frm_main = func_bt_ring_form_create();

    func_cb.mp3_res_play = func_bt_mp3_res_play;
    bsp_bt_ring_enter();

    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = 1;
    }

    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,0xff);
}

void func_bt_ring_exit(void)
{
    bsp_bt_ring_exit();
    uteDrvMotorStop();

    func_cb.last = FUNC_BT_RING;
}

void func_bt_ring(void)
{
    u16 interval = 0, latency = 0, tout = 0;

    printf("%s\n", __func__);

    // Adjust BLE connection parameter when incoming
    if (ble_is_connect() && (ble_get_conn_interval() < 400))
    {
        interval = ble_get_conn_interval();
        latency = ble_get_conn_latency();
        tout = ble_get_conn_timeout();
        ble_update_conn_param(480, 0, 500);
    }

    func_bt_ring_enter();
    while (func_cb.sta == FUNC_BT_RING)
    {
        func_bt_ring_process();
        func_bt_ring_message(msg_dequeue());
    }
    func_bt_ring_exit();

    // Restore BLE connection parameter when call stop
    if (bt_cb.disp_status != BT_STA_INCALL)
    {
        if (interval | latency | tout)
        {
            ble_update_conn_param(interval, latency, tout);
        }
    }
}
