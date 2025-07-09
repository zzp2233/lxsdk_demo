#include "include.h"
#include "func.h"
#include "ute_module_sport.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define KCAL_ARC_COLOR                61861
#define KM_ARC_COLOR                  65188
#define STEP_ARC_COLOR                1946

enum
{
    ARC_ANIM_STATUS_START = 0,
    ARC_ANIM_STATUS_FW,
    ARC_ANIM_STATUS_BW,
    ARC_ANIM_STATUS_END,
};

enum
{
    ///文本框
    STEP_TXT_VALUE_ID=1,
    KCAL_TXT_VALUE_ID,
    KM_TXT_VALUE_ID,

    KCAL_ARC_ID,
    KM_ARC_ID,
    STEP_ARC_ID,
    KM_TXT_UNIT_ID,
};

typedef struct f_activity_t_
{
    uint32_t tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
} f_activity_t;

#define TXT_SPACING_Y                 widget_text_get_height()+8

///创建活动记录窗体
compo_form_t *func_activity_form_create(void)
{
    ///新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_EVREY_DAY_ACTIVITY]);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I330001_ACTIVITY_BG_BIN);
    ///创建圆弧
    widget_page_t *widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/4.5, pic_bg_area.wid, pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_BG_BIN);
    compo_picturebox_set_pos(arc_pic, pic_bg_area.wid/2, pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_CALORIES_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KCAL_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_DISTANCE_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KM_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_STEPS_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,STEP_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_YUANJIAO_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I330001_ACTIVITY_YUANJIAO_BIN).hei/2);

    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_textbox_t *textbox;

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_KCAL]));///千卡
    compo_textbox_set(textbox, i18n[STR_KCAL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,KCAL_ARC_COLOR);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",distance/100, distance%100);///公里数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    if(uteModuleSystemtimeGetDistanceMiType())
    {
        textbox = compo_textbox_create(frm,strlen(i18n[STR_MILE]));//英里
        compo_textbox_set(textbox, i18n[STR_MILE]);
    }
    else
    {
        textbox = compo_textbox_create(frm,strlen(i18n[STR_KM]));//公里
        compo_textbox_set(textbox, i18n[STR_KM]);
    }
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,KM_ARC_COLOR);
    compo_setid(textbox,KM_TXT_UNIT_ID);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/1.28);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP]));///步数
    compo_textbox_set(textbox, i18n[STR_STEP]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/1.28+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,STEP_ARC_COLOR);

    return frm;
}


//活动记录功能事件处理
static void func_activity_disp_handle(void)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    if(tick_check_expire(f_activity->tick, 5))
    {
        f_activity->tick = tick_get();
        char txt_buf[20];
        uint32_t totalStepCnt = 0;
        uint32_t Step_value = 0;
        uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
        Step_value = totalStepCnt;
        if(Step_value>uteModuleSportGetStepsTargetCnt())
        {
            Step_value = uteModuleSportGetStepsTargetCnt();
        }

        compo_picturebox_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
        compo_picturebox_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
        compo_picturebox_t *arc_step = compo_getobj_byid(STEP_ARC_ID);
        compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
        compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
        compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);
        compo_textbox_t *textbox_km_unit = compo_getobj_byid(KM_TXT_UNIT_ID);

        if(uteModuleSystemtimeGetDistanceMiType())//英里
        {
            compo_textbox_set(textbox_km_unit, i18n[STR_MILE]);
        }
        else
        {
            compo_textbox_set(textbox_km_unit, i18n[STR_KM]);
        }

        if(f_activity->activity_state == 0)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);

            f_activity->arc_step_value+=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value >=1800)
            {
                f_activity->activity_state = 1;
            }
        }
        else if(f_activity->activity_state == 1)
        {

            f_activity->arc_step_value-=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value <= Step_value*1800 /uteModuleSportGetStepsTargetCnt())
            {
                f_activity->activity_state = 2;
            }

        }
        else
        {
            f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
            f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
            f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

            if(f_activity->arc_step_value)
            {
                compo_picturebox_set_visible(arc_kcal,true);
                compo_picturebox_set_visible(arc_km,true);
                compo_picturebox_set_visible(arc_step,true);
            }
            else
            {
                compo_picturebox_set_visible(arc_kcal,false);
                compo_picturebox_set_visible(arc_km,false);
                compo_picturebox_set_visible(arc_step,false);
            }


            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
            compo_textbox_set(textbox_step, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
            compo_textbox_set(textbox_km, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
            compo_textbox_set(textbox_kcal, txt_buf);
        }

    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define KCAL_ARC_COLOR                61861
#define KM_ARC_COLOR                  65188
#define STEP_ARC_COLOR                1946

enum
{
    ARC_ANIM_STATUS_START = 0,
    ARC_ANIM_STATUS_FW,
    ARC_ANIM_STATUS_BW,
    ARC_ANIM_STATUS_END,
};

enum
{
    ///文本框
    STEP_TXT_VALUE_ID=1,
    KCAL_TXT_VALUE_ID,
    KM_TXT_VALUE_ID,

    KCAL_ARC_ID,
    KM_ARC_ID,
    STEP_ARC_ID,
    KM_TXT_UNIT_ID,
};

typedef struct f_activity_t_
{
    uint32_t tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
} f_activity_t;

#define TXT_SPACING_Y                 widget_text_get_height()+8
///创建活动记录窗体
compo_form_t *func_activity_form_create(void)
{
    ///新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I332001_ACTIVITY_BG_BIN);
    ///创建圆弧
    widget_page_t *widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/4.5, pic_bg_area.wid, pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_BG_BIN);
    compo_picturebox_set_pos(arc_pic, pic_bg_area.wid/2, pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_CALORIES_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KCAL_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_DISTANCE_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KM_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_STEPS_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,STEP_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_YUANJIAO_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I332001_ACTIVITY_YUANJIAO_BIN).hei/2);

    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_textbox_t *textbox;

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_KCAL]));///千卡
    compo_textbox_set(textbox, i18n[STR_KCAL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,KCAL_ARC_COLOR);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",distance/100, distance%100);///公里数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    if(uteModuleSystemtimeGetDistanceMiType())
    {
        textbox = compo_textbox_create(frm,strlen(i18n[STR_MILE]));//英里
        compo_textbox_set(textbox, i18n[STR_MILE]);
    }
    else
    {
        textbox = compo_textbox_create(frm,strlen(i18n[STR_KM]));//公里
        compo_textbox_set(textbox, i18n[STR_KM]);
    }
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,KM_ARC_COLOR);
    compo_setid(textbox,KM_TXT_UNIT_ID);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/1.28);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP]));///步数
    compo_textbox_set(textbox, i18n[STR_STEP]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/1.28+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,STEP_ARC_COLOR);

    return frm;
}


//活动记录功能事件处理
static void func_activity_disp_handle(void)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    if(tick_check_expire(f_activity->tick, 5))
    {
        f_activity->tick = tick_get();
        char txt_buf[20];
        uint32_t totalStepCnt = 0;
        uint32_t Step_value = 0;
        uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
        Step_value = totalStepCnt;
        if(Step_value>uteModuleSportGetStepsTargetCnt())
        {
            Step_value = uteModuleSportGetStepsTargetCnt();
        }

        compo_picturebox_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
        compo_picturebox_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
        compo_picturebox_t *arc_step = compo_getobj_byid(STEP_ARC_ID);
        compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
        compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
        compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);
        compo_textbox_t *textbox_km_unit = compo_getobj_byid(KM_TXT_UNIT_ID);

        if(uteModuleSystemtimeGetDistanceMiType())//英里
        {
            compo_textbox_set(textbox_km_unit, i18n[STR_MILE]);
        }
        else
        {
            compo_textbox_set(textbox_km_unit, i18n[STR_KM]);
        }

        if(f_activity->activity_state == 0)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);

            f_activity->arc_step_value+=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value >=1800)
            {
                f_activity->activity_state = 1;
            }
        }
        else if(f_activity->activity_state == 1)
        {

            f_activity->arc_step_value-=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value <= Step_value*1800 /uteModuleSportGetStepsTargetCnt())
            {
                f_activity->activity_state = 2;
            }

        }
        else
        {
            f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
            f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
            f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

            if(f_activity->arc_step_value)
            {
                compo_picturebox_set_visible(arc_kcal,true);
                compo_picturebox_set_visible(arc_km,true);
                compo_picturebox_set_visible(arc_step,true);
            }
            else
            {
                compo_picturebox_set_visible(arc_kcal,false);
                compo_picturebox_set_visible(arc_km,false);
                compo_picturebox_set_visible(arc_step,false);
            }


            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
            compo_textbox_set(textbox_step, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
            compo_textbox_set(textbox_km, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
            compo_textbox_set(textbox_kcal, txt_buf);
        }

    }
}

#endif

// 显示活动记录界面刷新
static void func_activity_process(void)
{
    func_activity_disp_handle();
    func_process();
}

//活动记录功能消息处理
static void func_activity_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入活动记录控制功能
static void func_activity_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_activity_t));
    func_cb.frm_main = func_activity_form_create();
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;
    f_activity->activity_state = 0;
}

//退出活动记录功能
static void func_activity_exit(void)
{
    func_cb.last = FUNC_ACTIVITY;
}

//活动记录功能
void func_activity(void)
{
    printf("%s\n", __func__);
    func_activity_enter();
    while (func_cb.sta == FUNC_ACTIVITY)
    {
        func_activity_process();
        func_activity_message(msg_dequeue());
    }
    func_activity_exit();
}
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
