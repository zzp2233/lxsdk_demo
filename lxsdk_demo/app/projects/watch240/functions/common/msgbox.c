#include "include.h"
#include "func_cover.h"
#include "ute_project_config.h"
#include "ute_drv_battery_common.h"
#include "ute_drv_motor.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define ANIMATION_TICK_EXPIRE           18                          //对话框动画切换单位时间(ms)
#define ANIMATION_STEP                  (GUI_SCREEN_WIDTH / 8)      //步进
#define MSGBOX_MAX_TXT_LEN              100
#define MSGBOX_EXIT_TICK_EXPIRE         5000                        //定时退出单位时间(ms)

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_OK = 1,
    COMPO_ID_BTN_CANCEL,
    COMPO_ID_BTN_DELETE,
    COMPO_ID_BTN_REMIND_LATER,
};

//enum {
//    MSG_TYPE_WECHAT = 1,   //微信消息
//    MSG_TYPE_COVER,        //弹窗消息
//};

typedef struct msg_cb_t_
{
    compo_form_t *frm;
    int res;
    u32 tick;
    s32 animation_dx;
    bool show;
    bool flag_animation;
    bool flag_entering;
    u32 exit_tick;
    u32 hand_exit_flag;
    char msg_type;
    u8 enter_msg_sta;
} msg_cb_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 GUI_SCREEN_CENTER_X,
                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20);
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH-10, 128-20);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
//                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
//                compo_textbox_set_autoroll_mode(txt_msg, 2);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, true);
                compo_textbox_set_pos(txt_time, GUI_SCREEN_CENTER_X,
                                      func_cover_get_time_txt_y(msg_type));              //调整文本位置
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3.5, GUI_SCREEN_WIDTH/1.15, GUI_SCREEN_HEIGHT/5);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 28,
                                 GUI_SCREEN_CENTER_Y/3.5);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,
                                           GUI_SCREEN_CENTER_Y/3.5-widget_text_get_height()-2,
                                           145,widget_text_get_height()+3);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,GUI_SCREEN_CENTER_Y/3.5+3);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,0xff);
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I330001_FINGWATCH_WATCH_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                compo_animation_set_radix(animation,3);
                compo_animation_set_interval(animation,30);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_OTA_04_BIN);
                compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.7);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,GUI_SCREEN_WIDTH/1.2,30);//调整文本位置
                compo_textbox_set(txt_msg, i18n[STR_LOW_BATTERY]);
                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/4.5,60,30);//调整文本位置
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_32_BIN);
                char level[4];
                memset(level,0,sizeof(level));
                sprintf(level,"%d%%",uteDrvBatteryCommonGetLvl());
                compo_textbox_set(txt_title, level);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE01_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,246 );
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 128);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X/1.5);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {

                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_set_multiline(txt_msg, false);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,
                                           func_cover_get_title_txt_y(msg_type)-(widget_text_get_area(txt_title->txt).hei-widget_text_get_height()),
                                           widget_text_get_area(txt_msg->txt).wid,
                                           widget_text_get_height()); //调整文本位置
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>GUI_SCREEN_HEIGHT/2.5)
            {
                txt_leng.hei = GUI_SCREEN_HEIGHT/2.5;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-widget_text_get_height(),txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK00_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_OK00_BIN).hei/2 - 10);
            break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_CLOSE00_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_OK01_BIN).hei/2 - 20);
            break;
        case MSGBOX_MODE_BTN_OKCANCEL1:      //清除口数是否
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_SET_PUFFS_CANCEL00_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_CLOSE00_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I330001_SET_PUFFS_OK00_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_OK01_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_CLOSE00_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK00_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_OK01_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);  //需更替为删除图标
                compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20,
                                           gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).wid - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei, gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei);
                compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_SOON_BIN);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_CLOSE00_BIN).hei/2 - 20);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_SOON_BIN).hei/2 - 20);
            break;
        case MSGBOX_MODE_BTN_FACTORR://工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); //PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);//透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); //FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);//透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        case MSGBOX_MODE_BTN_DELETE_SUCCES://口数删除成功
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_DELETE_SUCCES]));///PASS
            compo_textbox_set(textbox, i18n[STR_DELETE_SUCCES]);
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+60);
            compo_textbox_set_forecolor(textbox, COLOR_WHITE);

            compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_SET_PUFFS_OK01_BIN);
            compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-40);

        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 GUI_SCREEN_CENTER_X,28+46/2);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,86+11);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,130+96/2,300,96);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_pos(txt_time, GUI_SCREEN_CENTER_X,86+11);
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, 80, 240, 80);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),97,80);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg,130,50,170,36);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, 130,88);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,0xff);
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I332001_FINGWATCH_WATCH_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                compo_animation_set_radix(animation,3);
                compo_animation_set_interval(animation,30);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                // printf("555555555555555555555555\n");
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I332001_OTA_LOW_ELECTRICITY_BIN);
                compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.7);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 284+32/2,200,30);//调整文本位置
                compo_textbox_set(txt_msg, i18n[STR_LOW_BATTERY]);
                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, 238+26/2,GUI_SCREEN_WIDTH,30);//调整文本位置
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_32_BIN);
                char level[4];
                memset(level,0,sizeof(level));
                sprintf(level,"%d%%",uteDrvBatteryCommonGetLvl());
                compo_textbox_set(txt_title, level);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,234+40);
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,28/2+140 );
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_64_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,75+18);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {
                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,150);
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_64_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_t *txt_alarm_clock = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_alarm_clock, GUI_SCREEN_CENTER_X,72+34/2);
                compo_textbox_set(txt_alarm_clock, i18n[STR_ALARM_CLOCK]);
                compo_textbox_set_forecolor(txt_alarm_clock,make_color(0xff,0x83,0x00));

                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,160,70,32); //调整文本位置
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           220, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       258, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            printf("data:%s\n",msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>136)
            {
                txt_leng.hei = 136;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,258,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 136/2+72,258,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 68/2+100,
                                       275, 70);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set_multiline_drag(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 234+40);
            break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, 68+40,234+40);

            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, 212+40,234+40);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn,80/2+68,80/2+234);

            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_OK_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn,80/2+212,80/2+234);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);  //需更替为删除图标
                compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20,
                                           gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).wid - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei, gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei);
                compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_SOON_BIN);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, 274);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4,274);
            break;
        case MSGBOX_MODE_BTN_FACTORR:///工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS"));///PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);///透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8, GUI_SCREEN_WIDTH / 2, widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL"));///FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);///透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8, GUI_SCREEN_WIDTH / 2, widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 GUI_SCREEN_CENTER_X,
                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20);
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH-10, 128-20);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
//                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
//                compo_textbox_set_autoroll_mode(txt_msg, 2);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, true);
                compo_textbox_set_pos(txt_time, GUI_SCREEN_CENTER_X,
                                      func_cover_get_time_txt_y(msg_type));              //调整文本位置
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3.5, GUI_SCREEN_WIDTH/1.15, GUI_SCREEN_HEIGHT/5);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 28,
                                 GUI_SCREEN_CENTER_Y/3.5);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,
                                           GUI_SCREEN_CENTER_Y/3.5-widget_text_get_height()-2,
                                           145,widget_text_get_height()+3);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,GUI_SCREEN_CENTER_Y/3.5+3);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,0xff);
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I332001_FINGWATCH_WATCH_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                compo_animation_set_radix(animation,3);
                compo_animation_set_interval(animation,30);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                compo_picturebox_t *picbox = compo_picturebox_create(frm, 0);
                compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.7);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,GUI_SCREEN_WIDTH/1.2,30);//调整文本位置
                compo_textbox_set(txt_msg, i18n[STR_LOW_BATTERY]);
                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/4.5,230,30);//调整文本位置
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_32_BIN);
                char level[4];
                memset(level,0,sizeof(level));
                sprintf(level,"%d%%",uteDrvBatteryCommonGetLvl());
                compo_textbox_set(txt_title, level);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,246 );
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 128);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X/1.5);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {

                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_set_multiline(txt_msg, false);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,
                                           func_cover_get_title_txt_y(msg_type)-(widget_text_get_area(txt_title->txt).hei-widget_text_get_height()),
                                           widget_text_get_area(txt_msg->txt).wid,
                                           widget_text_get_height()); //调整文本位置
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>GUI_SCREEN_HEIGHT/2.5)
            {
                txt_leng.hei = GUI_SCREEN_HEIGHT/2.5;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-widget_text_get_height(),txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set_multiline_drag(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_OK01_BIN).hei/2 - 10);
            break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_CLOSE_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_OK01_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn,80/2+68,80/2+234);

            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn,80/2+212,80/2+234);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);  //需更替为删除图标
                compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20,
                                           gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).wid - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei, gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei);
                compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_SOON_BIN);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_CLOSE_BIN).hei/2 - 20);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_SOON_BIN).hei/2 - 20);
            break;
        case MSGBOX_MODE_BTN_FACTORR:///工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); //PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);//透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); //FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);//透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}
#endif //
//单击按钮
static void msgbox_button_click(void)
{
    int id = compo_get_button_id();
    msg_cb_t *msg_cb = func_cb.msg_cb;
    switch (id)
    {
        case COMPO_ID_BTN_OK:
            msg_cb->res = MSGBOX_RES_OK;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case COMPO_ID_BTN_CANCEL:
            msg_cb->res = MSGBOX_RES_CANCEL;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case COMPO_ID_BTN_DELETE:
            msg_cb->res = MSGBOX_RES_DELETE;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case COMPO_ID_BTN_REMIND_LATER:
            msg_cb->res = MSGBOX_RES_REMIND_LATER;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        default:
            break;
    }
}

//对话框消息处理
static void msgbox_message(size_msg_t msg)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
//            printf("MSG_CTP_CLICK\n");
            if (!msg_cb->flag_animation)
            {
                if (msg_cb->msg_type == MSGBOX_MSG_TYPE_BRIEF)
                {
                    point_t pt = ctp_get_sxy();
                    if (abs_s(pt.y - GUI_SCREEN_CENTER_Y/3.5) * 2 <= GUI_SCREEN_HEIGHT/5) //简略消息位置判断
                    {
                        msg_cb->res = MSGBOX_RES_ENTER_DETAIL_MSG;
                    }
                    else
                    {

                    }
                    msg_cb->flag_animation = true;
                    msg_cb->flag_entering = false;
                }
                else
                {
                    msgbox_button_click();                         //单击按钮
                }
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
        case KU_BACK:
            if (tick_check_expire(msg_cb->tick, 1000))  //1s钟后才能手动退出弹窗
            {
                msg_cb->hand_exit_flag =true;
            }
            if(msg_cb->hand_exit_flag == false)
            {
                return;
            }
            printf("KU_BACK\n");
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case EVT_MSGBOX_EXIT:
            printf("EVT_MSGBOX_EXIT\n");
            msg_cb->res = MSGBOX_RES_EXIT;
            msg_cb->show = false;
            break;
        case MSG_CHECK_LANGUAGE:
            printf("EVT_MSGBOX_EXIT\n");
            msg_cb->res = MSGBOX_RES_EXIT;
            msg_cb->show = false;
            msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
            break;
        default:
            evt_message(msg);
    }
}


//对话框事件处理
static void msgbox_process(void)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    u32 wid, hei;

    if (msg_cb->flag_animation)
    {
        if (tick_check_expire(msg_cb->tick, ANIMATION_TICK_EXPIRE))
        {
            msg_cb->tick = tick_get();

            if (msg_cb->flag_entering)
            {
                msg_cb->animation_dx += ANIMATION_STEP;
            }
            else
            {
                msg_cb->animation_dx -= ANIMATION_STEP;
            }
            if (msg_cb->animation_dx <= 0)
            {
                msg_cb->flag_animation = false;
                msg_cb->show = false;
                wid = 1;
                hei = 1;
            }
            else if (msg_cb->animation_dx >= GUI_SCREEN_WIDTH)
            {
                msg_cb->flag_animation = false;
                wid = GUI_SCREEN_WIDTH;
                hei = GUI_SCREEN_HEIGHT;
            }
            else
            {
                wid = msg_cb->animation_dx;
                hei = muls_shift16(GUI_SCREEN_HEIGHT, (wid << 16) / GUI_SCREEN_WIDTH);
            }
            compo_form_scale_to(msg_cb->frm, wid, hei);
        }
    }
    else
    {

        uint32_t msgbox_exit_time = MSGBOX_EXIT_TICK_EXPIRE;
        if (msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER && sys_cb.cover_index == REMIND_COVER_ALARM)          //闹钟覆盖界面
        {
            msgbox_exit_time = UTE_LOCAL_ALARM_DEFAULT_RING_TIMES * 1000;
        }
        else if (msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER && sys_cb.cover_index == REMIND_GCOVER_BT_NOT_CONNECT)  //蓝牙未连接弹窗
        {
            if (bt_is_connected())              //蓝牙连接后立刻退出这个弹窗
            {
                msg_cb->flag_animation = true;
                msg_cb->flag_entering = false;
                msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;

                goto __exit;
            }
        }
        else if (msg_cb->msg_type == MSGBOX_MSG_TYPE_DETAIL)
        {
            goto __exit;
        }
        else if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH && msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER)
        {
            // printf("11111111111111111111111\n");
            reset_sleep_delay_all();
            if(uteDrvMotorGetRunningStatus() == false)
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            }
            if (tick_check_expire(msg_cb->exit_tick, 150*1000))    //查找手表
            {
                msg_cb->exit_tick = tick_get();
                msg_cb->flag_animation = true;
                msg_cb->flag_entering = false;
                msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;
            }
            goto __exit;
        }


        if (tick_check_expire(msg_cb->exit_tick, msgbox_exit_time))   //定时退出
        {
            msg_cb->exit_tick = tick_get();
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;
        }
    }

__exit:
    func_process();
}

//进入对话框
static void msgbox_enter(compo_form_t *frm)
{
    msg_cb_t *msg_cb = func_zalloc(sizeof(msg_cb_t));
    func_cb.msg_cb = msg_cb;
    msg_cb->frm = frm;
    msg_cb->show = true;
    msg_cb->flag_animation = true;
    msg_cb->flag_entering = true;
    msg_cb->tick = tick_get();
    msg_cb->exit_tick = tick_get();
    compo_form_scale_to(msg_cb->frm, 1, 1);
}

//退出对话框
static void msgbox_exit(void)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    compo_form_destroy(msg_cb->frm);
    func_free(msg_cb);
    func_cb.msg_cb = NULL;
    uteDrvMotorStop();
}


//界面sta切换到下列ID强制退出弹窗
static void msgbox_check_force_exit(void)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    if (func_cb.msgbox_enter_sta != func_cb.sta)
    {
        printf("%s -> EVT_MSGBOX_EXIT\n", __func__);
        msg_cb->res = MSGBOX_RES_EXIT;
        msg_cb->show = false;
    }
}

//对话框
int msgbox(char *msg, char *title, char* time, int mode, char msg_type)
{
    func_cb.msgbox_enter_sta = func_cb.sta;

    msg_cb_t *msg_cb;
    msgbox_enter(msgbox_frm_create(msg, title, time, mode, msg_type));
    msg_cb = func_cb.msg_cb;
    msg_cb->msg_type = msg_type;
    msg_cb->enter_msg_sta = func_cb.sta;
    while (msg_cb->show)
    {
        msgbox_message(msg_dequeue());
        msgbox_process();
        msgbox_check_force_exit();
    }
    int res = msg_cb->res;
    msgbox_exit();                      //这里释放到msg_cb结构体
    func_cb.msgbox_enter_sta = 0;
    return res;
}


bool msgbox_frist_set_check(void)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    if (msg_cb != NULL)
    {
        if (msg_cb->enter_msg_sta != func_cb.sta)
        {
            return true;
        }
    }
    return false;
}

