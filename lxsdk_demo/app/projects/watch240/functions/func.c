#include "include.h"
#include "func_menu.h"
#include "func_cover.h"
#include "ute_module_systemtime.h"
#include "ute_language_common.h"
#include "ute_drv_motor.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define FUNC_CREATE_CNT                       ((int)(sizeof(tbl_func_create) / sizeof(tbl_func_create[0])))
#define FUNC_ENTRY_CNT                        ((int)(sizeof(tbl_func_entry) / sizeof(tbl_func_entry[0])))

typedef struct func_t_
{
    int func_idx;//任务编号
    void *func;  //指向任务函数的函数指针
} func_t;

extern void func_menu(void);
extern void func_menustyle(void);
extern void func_clock(void);
extern void func_clock_preview(void);
//extern void func_clock_sub_sidebar(void);
#if UTE_MODULE_SCREENS_UP_MENU_SUPPORT
extern void func_clock_sub_card(void);
#endif // UTE_MODULE_SCREENS_UP_MENU_SUPPORT
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
extern void func_heartrate(void);
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
extern void func_compo_select(void);
extern void func_compo_select_sub(void);
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
extern void func_alarm_clock(void);
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
extern void func_alarm_clock_sub_set(void);
extern void func_alarm_clock_sub_repeat(void);
extern void func_alarm_clock_sub_edit(void);
extern void func_alarm_clock_sub_pop(void);
extern void func_smoke_target_sub_pop(void);
extern void func_disturd_sub_set(void);
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
extern void func_blood_oxygen(void);
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
extern void func_breathe(void);
extern void func_breathe_sub_mode(void);
extern void func_breathe_sub_time(void);
extern void func_breathe_finish(void);
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
extern void func_calculator(void);
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
extern void func_camera(void);
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
extern void func_light(void);
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
extern void func_timer(void);
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
extern void func_sleep(void);
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
extern void func_stopwatch(void);
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
extern void func_stopwatch_sub_record(void);
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
extern void func_weather(void);
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
extern void func_sport(void);

//extern void func_sport_config(void);
extern void func_sport_sub_run(void);
extern void func_sport_sort(void);
extern void func_sport_switching(void);
extern void func_sport_finish(void);
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
extern void func_calendar(void);
#if UTE_MODULE_SCREENS_CALL_SUPPORT
extern void func_call(void);
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
extern void func_call_sub_record(void);
extern void func_call_sub_dial(void);
extern void func_call_sort(void);
#if UTE_MODULE_SCREENS_GAME_SUPPORT
extern void func_game(void);
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
extern void func_style(void);
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
extern void func_findphone(void);
extern void func_altitude(void);
extern void func_map(void);
extern void func_message_info(void);
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
extern void func_scan(void);
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
extern void func_voice(void);
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
//extern void func_compass(void);
#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
extern void func_address_book(void);
#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
extern void func_set_sub_list(void);
extern void func_set_sub_sav(void);
extern void func_set_puffs(void);
extern void func_target_puffs(void);
extern void func_sliding_unlcok_screen(void);
extern void func_set_sub_dousing(void);
extern void func_set_sub_disturd(void);
#if UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
extern void func_set_sub_language(void);
#endif // UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
#if GUI_MODULE_WRIST_SUPPORT
extern void func_set_sub_wrist(void);
#endif
//extern void func_set_sub_time(void);
//extern void func_time_sub_custom(void);
extern void func_set_sub_password(void);
extern void func_password_sub_disp(void);
extern void func_password_sub_select(void);
extern void func_set_sub_about(void);
#if UTE_MODULE_SCREENS_RESTART_SUPPORT
extern void func_set_sub_restart(void);
#endif // UTE_MODULE_SCREENS_RESTART_SUPPORT
#if UTE_MODULE_SCREENS_RESFY_SUPPORT
extern void func_set_sub_rstfy(void);
#endif // UTE_MODULE_SCREENS_RESFY_SUPPORT
extern void func_set_sub_off(void);
//extern void func_set_sub_4g(void);
extern void func_switching_to_menu(void);
//extern void func_volume(void);
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
extern void func_activity(void);
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT

extern void func_bloodsugar(void);
//extern void func_bloodpressure(void);
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
extern void func_flashlight(void);
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
extern void func_charge(void);
extern void func_debug_info(void);

extern void func_music(void);
extern void func_idle(void);
extern void func_bt_update(void);
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
extern void func_bt(void);
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
extern void func_bt_ring(void);
extern void func_bt_call(void);
extern void func_bthid(void);
extern void func_usbdev(void);
extern void func_aux(void);
//extern void func_smartstack(void);
extern void func_modem_call(void);
extern void func_modem_ring(void);
extern void func_message_reply_info(void);
//extern void func_mic_test(void);
//extern void func_tetris(void);
//extern void func_tetris_start(void);
extern void func_bird(void);
extern void func_ota_ui(void);
//extern void func_pressure(void);//压力
//extern void func_pressure_explain(void);//压力说明
extern void func_long_press(void);//关机 重启 SOS
extern void func_ota_update(void);
extern void func_ota_err(void);
extern void func_ota_succ(void);
extern void func_ble_call(void);
//extern void func_set_sub_sos(void);
#if UTE_MODULE_SCREENS_POWERON_SUPPORT
extern void func_power_on(void);//开机
#endif
extern void func_power_on_scan(void);
extern void func_power_on_language(void);
extern void func_test_mode(void);///*出厂测试模式选择*/
extern void func_factory_testing(void);///*工厂测试*/
extern void func_ageing(void);///*老化测试*/
extern void func_audio(void);///*音频测试*/
extern void func_online_factory_test(void);
extern void func_empty(void);

#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
extern void func_up_watch_dial(void);
#endif // UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT


#if ECIG_POWER_CONTROL
extern void func_ecig_set_power(void);
extern void func_ecig_reminder(void);
extern void func_ecig_reminderclock(void);
#endif
#if UTE_MODULE_SCREENS_CALENDAER_SUB_SUPPORT
extern void func_calendar_sub(void);
#endif
extern void func_ecig_vpae_sub(void);

compo_form_t *func_breathe_finish_form_create(void);
compo_form_t *func_up_watch_dial_form_create(void);
compo_form_t *func_power_on_language_form_create(void);
compo_form_t *func_power_on_scan_form_create(void);
compo_form_t *func_set_sub_password_form_create(void);
#if UTE_MODULE_SCREENS_POWERON_SUPPORT
compo_form_t *func_power_on_form_create(void);//开机
#endif
compo_form_t *func_ble_call_form_create(void);
compo_form_t *func_ota_update_form_create(void);
compo_form_t *func_ota_err_form_create(void);
compo_form_t *func_ota_succ_form_create(void);
compo_form_t *func_long_press_form_create(void);//关机 重启 SOS
//compo_form_t *func_pressure_explain_form_create(void);//压力说明
//compo_form_t *func_pressure_form_create(void);//压力
compo_form_t *func_menu_form_create(void);
compo_form_t *func_clock_form_create(void);
//compo_form_t *func_clock_sub_sidebar_form_create(void);
#if UTE_MODULE_SCREENS_UP_MENU_SUPPORT
compo_form_t *func_clock_sub_card_form_create(void);
#endif // UTE_MODULE_SCREENS_UP_MENU_SUPPORT
compo_form_t *func_heartrate_form_create(void);
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
compo_form_t *func_bt_form_create(void);
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
compo_form_t *func_alarm_clock_form_create(void);
compo_form_t *func_alarm_clock_sub_set_form_create(void);
compo_form_t *func_alarm_clock_sub_repeat_form_create(void);
compo_form_t *func_alarm_clock_sub_edit_form_create(void);
compo_form_t *func_alarm_clock_sub_pop_form_create(void);
compo_form_t *func_smoke_target_sub_pop_form_create(void);
compo_form_t *func_blood_oxygen_form_create(void);
compo_form_t *func_breathe_form_create(void);
compo_form_t *func_breathe_sub_mode_form_create(void);
compo_form_t *func_breathe_sub_time_form_create(void);
compo_form_t *func_calculator_form_create(void);
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
compo_form_t *func_camera_form_create(void);
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
compo_form_t *func_light_form_create(void);
compo_form_t *func_timer_form_create(void);
compo_form_t *func_sleep_form_create(void);
compo_form_t *func_stopwatch_form_create(void);
compo_form_t *func_stopwatch_sub_record_form_create(void);
compo_form_t *func_weather_form_create(void);
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
compo_form_t *func_sport_form_create(void);
//compo_form_t *func_sport_config_form_create(void);
compo_form_t *func_sport_sub_run_form_create(void);
compo_form_t *func_sport_switching_form_create(void);
compo_form_t *func_sport_sort_form_create(void);
compo_form_t *func_sport_finish_form_create(void);
#endif
compo_form_t *func_set_sub_disturd_form_create(void);
compo_form_t *func_disturd_sub_set_form_create(void);
compo_form_t *func_call_form_create(void);
compo_form_t *func_call_sub_record_form_create(void);
compo_form_t *func_call_sub_dial_form_create(void);
compo_form_t *func_call_sort_form_create(void);
compo_form_t *func_game_form_create(void);
compo_form_t *func_style_form_create(void);
compo_form_t *func_findphone_form_create(void);
compo_form_t *func_altitude_form_create(void);
compo_form_t *func_map_form_create(void);
compo_form_t *func_message_form_create(void);
compo_form_t *func_scan_form_create(void);
compo_form_t *func_voice_form_create(void);
//compo_form_t *func_compass_form_create(void);
compo_form_t *func_address_book_form_create(void);
compo_form_t *func_set_sub_list_form_create(void);
#if GUI_MODULE_WRIST_SUPPORT
//compo_form_t *func_set_sub_wrist_form_create(void);
#endif
compo_form_t *func_set_sub_sav_form_create(void);
compo_form_t *func_set_puffs_form_create(void);
compo_form_t *func_target_puffs_form_create(void);
compo_form_t *func_sliding_unlcok_screen_form_create(void);
compo_form_t *func_set_sub_dousing_form_create(void);
compo_form_t *func_set_sub_language_form_create(void);
//compo_form_t *func_set_sub_time_form_create(void);
//compo_form_t *func_time_sub_custom_form_create(void);
//compo_form_t *func_set_sub_password_form_create(void);
compo_form_t *func_password_sub_disp_form_create(void);
compo_form_t *func_password_sub_select_form_create(void);
//compo_form_t *func_set_sub_4g_form_create(void);
compo_form_t *func_set_sub_about_form_create(void);
compo_form_t *func_set_sub_restart_form_create(void);
compo_form_t *func_set_sub_rstfy_form_create(void);
compo_form_t *func_set_sub_off_form_create(void);
compo_form_t *func_calender_form_create(void);
//compo_form_t *func_volume_form_create(void);
compo_form_t *func_activity_form_create(void);
//compo_form_t *func_bloodsugar_form_create(void);
//compo_form_t *func_bloodpressure_form_create(void);
compo_form_t *func_flashlight_form_create(void);
compo_form_t *func_charge_form_create(void);
compo_form_t *func_clock_preview_form_create(void);
compo_form_t *func_compo_select_form_create(void);
compo_form_t *func_compo_select_sub_form_create(void);
compo_form_t *func_debug_info_form_create(void);
//compo_form_t * func_smartstack_form_create(void);
compo_form_t *func_music_form_create(void);
compo_form_t *func_usbdev_form_create(void);
compo_form_t *func_recorder_form_create(void);
compo_form_t *func_message_reply_form_create(void);
// compo_form_t *func_mic_test_form_create(void);
//compo_form_t *func_tetris_form_create(void);
//compo_form_t *func_tetris_start_form_create(void);
compo_form_t *func_bird_form_create(void);
//compo_form_t *func_set_sub_sos_form_create(void);
compo_form_t *func_empty_form_create(void);
compo_form_t *func_test_mode_form_create(void);///*出厂测试模式选择*/
compo_form_t *func_factory_testing_create(void);///*工厂测试*/
compo_form_t *func_ageing_create(void);///*老化测试*/
compo_form_t *func_audio_create(void);///*音频测试*/
compo_form_t *func_online_factory_test_form_create(void);
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
compo_form_t *func_calender_sub_form_create(void);
#endif
compo_form_t *func_ecig_vpae_sub_form_create(void);

#if ECIG_POWER_CONTROL
compo_form_t *func_ecig_set_power_form_create(void);
compo_form_t *func_ecig_reminder_form_create(void);
compo_form_t *func_ecig_reminderclock_form_create(void);
#endif

bool func_music_is_play(void);
void func_music_play(bool sta);

func_cb_t func_cb AT(.buf.func_cb);

const func_t tbl_func_create[] =
{
    {FUNC_BLE_CALL,                     func_ble_call_form_create},
    {FUNC_MENU,                         func_menu_form_create},
    {FUNC_MENUSTYLE,                    NULL},
    {FUNC_CLOCK,                        func_clock_form_create},
    {FUNC_CLOCK_PREVIEW,                func_clock_preview_form_create},
//    {FUNC_SIDEBAR,                      func_clock_sub_sidebar_form_create},
#if UTE_MODULE_SCREENS_UP_MENU_SUPPORT
    {FUNC_CARD,                         func_clock_sub_card_form_create},
#endif // UTE_MODULE_SCREENS_UP_MENU_SUPPORT
#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
    {FUNC_UP_WATCH_DIAL,                func_up_watch_dial_form_create},     //在线表盘
#endif // UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT

#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {FUNC_HEARTRATE,                    func_heartrate_form_create},
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {FUNC_BT,                           func_bt_form_create},
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {FUNC_POWER_ON_SCAN,                func_power_on_scan_form_create},
    {FUNC_POWER_ON_LANGUAGE,            func_power_on_language_form_create},
    {FUNC_COMPO_SELECT,                 func_compo_select_form_create},
    {FUNC_COMPO_SELECT_SUB,             func_compo_select_sub_form_create},
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {FUNC_ALARM_CLOCK,                  func_alarm_clock_form_create},
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
    {FUNC_ALARM_CLOCK_SUB_SET,          func_alarm_clock_sub_set_form_create},
    {FUNC_ALARM_CLOCK_SUB_REPEAT,       func_alarm_clock_sub_repeat_form_create},
    {FUNC_ALARM_CLOCK_SUB_EDIT,         func_alarm_clock_sub_edit_form_create},
    {FUNC_ALARM_CLOCK_SUB_POP,          func_alarm_clock_sub_pop_form_create},
    {FUNC_SMOKE_TARGET_SUB_POP,          func_smoke_target_sub_pop_form_create},
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {FUNC_BLOOD_OXYGEN,                 func_blood_oxygen_form_create},
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//    {FUNC_PRESSURE,                     func_pressure_form_create},//压力
//    {FUNC_PRESSURE_EXPLAIN,             func_pressure_explain_form_create},//压力说明
    {FUNC_LONG_PRESS,                   func_long_press_form_create},//关机 重启 SOS
//    {FUNC_BLOODSUGAR,                   func_bloodsugar_form_create},
//    {FUNC_BLOOD_PRESSURE,               func_bloodpressure_form_create},
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {FUNC_BREATHE,                      func_breathe_form_create},
    {FUNC_BREATHE_SUB_MODE,             func_breathe_sub_mode_form_create},
    {FUNC_BREATHE_SUB_TIME,             func_breathe_sub_time_form_create},
    {FUNC_BREATHE_FINISH,               func_breathe_finish_form_create},
    {FUNC_BREATHE_FINISH,               func_breathe_finish},
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {FUNC_CALCULATOR,                   func_calculator_form_create},
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {FUNC_CAMERA,                       func_camera_form_create},
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {FUNC_TIMER,                        func_timer_form_create},
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {FUNC_SLEEP,                        func_sleep_form_create},
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {FUNC_STOPWATCH,                    func_stopwatch_form_create},
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {FUNC_STOPWATCH_SUB_RECORD,         func_stopwatch_sub_record_form_create},
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {FUNC_WEATHER,                      func_weather_form_create},
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {FUNC_SPORT,                        func_sport_form_create},
//    {FUNC_SPORT_CONFIG,                 func_sport_config_form_create},
    {FUNC_SPORT_SUB_RUN,                func_sport_sub_run_form_create},
    {FUNC_SPORT_SWITCH,                 func_sport_switching_form_create},
    {FUNC_SPORT_SORT,                   func_sport_sort_form_create},
    {FUNC_SPORT_FINISH,                 func_sport_finish_form_create},
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {FUNC_GAME,                         func_game_form_create},
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {FUNC_STYLE,                        func_style_form_create},
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    {FUNC_FINDPHONE,                    func_findphone_form_create},
    {FUNC_ALTITUDE,                     func_altitude_form_create},
    {FUNC_MAP,                          func_map_form_create},
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {FUNC_MESSAGE,                      func_message_form_create},
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    {FUNC_SCAN,                         func_scan_form_create},
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {FUNC_VOICE,                        func_voice_form_create},
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
//    {FUNC_COMPASS,                      func_compass_form_create},
#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
    {FUNC_ADDRESS_BOOK,                 func_address_book_form_create},
#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {FUNC_CALL,                         func_call_form_create},
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
    {FUNC_CALL_SUB_RECORD,              func_call_sub_record_form_create},
    {FUNC_CALL_SUB_DIAL,                func_call_sub_dial_form_create},
    {FUNC_CALL_SORT,                    func_call_sort_form_create},
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {FUNC_SETTING,                      func_set_sub_list_form_create},
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {FUNC_CALENDAER,                    func_calender_form_create},
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
////    {FUNC_SET_SUB_SAV,                       func_volume_form_create},
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {FUNC_ACTIVITY,                     func_activity_form_create},
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {FUNC_FLASHLIGHT,                   func_flashlight_form_create},
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {FUNC_LIGHT,                        func_light_form_create},
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {FUNC_SET_SUB_DOUSING,              func_set_sub_dousing_form_create},
#if GUI_MODULE_WRIST_SUPPORT
    {FUNC_SET_SUB_WRIST,                func_set_sub_wrist_form_create},
#endif
    {FUNC_SET_SUB_DISTURD,              func_set_sub_disturd_form_create},
    {FUNC_DISTURD_SUB_SET,              func_disturd_sub_set_form_create},
#if UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
    {FUNC_SET_SUB_LANGUAGE,             func_set_sub_language_form_create},
#endif // UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
//    {FUNC_SET_SUB_TIME,                 func_set_sub_time_form_create},
//    {FUNC_TIME_SUB_CUSTOM,              func_time_sub_custom_form_create},
    {FUNC_SET_SUB_PASSWORD,             func_set_sub_password_form_create},
    {FUNC_PASSWORD_SUB_DISP,            func_password_sub_disp_form_create},
    {FUNC_PASSWORD_SUB_SELECT,          func_password_sub_select_form_create},
    {FUNC_SET_SUB_SAV,                  func_target_puffs_form_create},
    {FUNC_TARGET_PUFFS,                  func_set_sub_sav_form_create},
    {FUNC_SLIDING_UNLOCK_SCREEN,                  func_sliding_unlcok_screen_form_create},
    {FUNC_SET_PUFFS,                  func_set_puffs_form_create},
    {FUNC_SET_SUB_ABOUT,                func_set_sub_about_form_create},
//    {FUNC_SET_SUB_4G,                   func_set_sub_4g_form_create},
#if UTE_MODULE_SCREENS_RESTART_SUPPORT
    {FUNC_SET_SUB_RESTART,              func_set_sub_restart_form_create},
#endif // UTE_MODULE_SCREENS_RESTART_SUPPORT
#if UTE_MODULE_SCREENS_RESFY_SUPPORT
    {FUNC_SET_SUB_RSTFY,                func_set_sub_rstfy_form_create},
#endif // UTE_MODULE_SCREENS_RESFY_SUPPORT
    {FUNC_SET_SUB_OFF,                  func_set_sub_off_form_create},
    {FUNC_CHARGE,                       func_charge_form_create},
    {FUNC_DEBUG_INFO,                   func_debug_info_form_create},
//    {FUNC_SMARTSTACK,                   func_smartstack_form_create},
#if FUNC_MUSIC_EN
    {FUNC_MUSIC,                        func_music_form_create},
#endif
#if FUNC_USBDEV_EN
    {FUNC_USBDEV,                       func_usbdev_form_create},
#endif
#if FUNC_RECORDER_EN
    {FUNC_RECORDER,                     func_recorder_form_create},
#endif
#if FLASHDB_EN
    {FUNC_MESSAGE_REPLY,                func_message_reply_form_create},
#endif
#if MIC_TEST_EN
    // {FUNC_MIC_TEST,                     func_mic_test_form_create},
#endif // MIC_TEST_EN

    {FUNC_BIRD,                         func_bird_form_create},
//    {FUNC_TETRIS,                       func_tetris_form_create},
//    {FUNC_TETRIS_START,                 func_tetris_start_form_create},
    {FUNC_OTA_MODE,                     func_ota_update_form_create},
    {FUNC_OTA_ERROR,                    func_ota_err_form_create},
    {FUNC_OTA_SUCC,                     func_ota_succ_form_create},
#if UTE_MODULE_SCREENS_POWERON_SUPPORT
    {FUNC_POWER_ON,                     func_power_on_form_create},
#endif
    {FUNC_TEST_MODE,                    func_test_mode_form_create},///*出厂测试模式选择*/
    {FUNC_FACTORY_TESTING,              func_factory_testing_create},///*出厂测试模式选择*/
    {FUNC_AGEING,                       func_ageing_create},///*老化测试*/
    {FUNC_AUDIO,                        func_audio_create},///*音频测试*/
    {FUNC_ONLINE_FACTORY_TEST,          func_online_factory_test_form_create},
    {FUNC_EMPTY,                        func_empty_form_create},
#if ECIG_POWER_CONTROL
    {FUNC_ECIG_SET_POWER,               func_ecig_set_power_form_create},
    {FUNC_ECIG_REMINDER,                func_ecig_reminder_form_create},
    {FUNC_ECIG_REMINDERCLOCK,                func_ecig_reminderclock_form_create},
#endif
#if UTE_MODULE_SCREENS_CALENDAER_SUB_SUPPORT
    {FUNC_CALENDAER_SUB,                    func_calender_sub_form_create},
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {FUNC_ECIG_VPAE_SUB,                    func_ecig_vpae_sub_form_create},
};

//任务入口表
const func_t tbl_func_entry[] =
{
    {FUNC_MENU,                         func_menu},                     //主菜单(蜂窝)
    {FUNC_MENUSTYLE,                    func_menustyle},                //主菜单样式选择
    {FUNC_CLOCK,                        func_clock},                    //时钟表盘
    {FUNC_CLOCK_PREVIEW,                func_clock_preview},            //时钟表盘预览
//    {FUNC_SIDEBAR,                      func_clock_sub_sidebar},        //表盘右滑
#if UTE_MODULE_SCREENS_UP_MENU_SUPPORT
    {FUNC_CARD,                         func_clock_sub_card},           //表盘上拉
#endif // UTE_MODULE_SCREENS_UP_MENU_SUPPORT

#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {FUNC_HEARTRATE,                    func_heartrate},                //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {FUNC_ALARM_CLOCK,                  func_alarm_clock},              //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
    {FUNC_ALARM_CLOCK_SUB_SET,          func_alarm_clock_sub_set},      //闹钟--设置
    {FUNC_ALARM_CLOCK_SUB_REPEAT,       func_alarm_clock_sub_repeat},   //闹钟--重复
    {FUNC_ALARM_CLOCK_SUB_EDIT,         func_alarm_clock_sub_edit},     //闹钟--编辑
    {FUNC_ALARM_CLOCK_SUB_POP,          func_alarm_clock_sub_pop},      //闹钟--弹出
    {FUNC_SMOKE_TARGET_SUB_POP,          func_smoke_target_sub_pop},      //
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {FUNC_BLOOD_OXYGEN,                 func_blood_oxygen},             //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
//    {FUNC_PRESSURE,                     func_pressure},                 //压力
//    {FUNC_PRESSURE_EXPLAIN,             func_pressure_explain},         //压力说明
    {FUNC_LONG_PRESS,                   func_long_press},               //关机 重启 SOS界面
//    {FUNC_BLOODSUGAR,                   func_bloodsugar},               //血糖
//    {FUNC_BLOOD_PRESSURE,               func_bloodpressure},            //血压
#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
    {FUNC_UP_WATCH_DIAL,                func_up_watch_dial},             //在线表盘
#endif // UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {FUNC_BREATHE,                      func_breathe},                  //呼吸
    {FUNC_BREATHE_SUB_MODE,             func_breathe_sub_mode},         //呼吸--模式设置
    {FUNC_BREATHE_SUB_TIME,             func_breathe_sub_time},         //呼吸--时间设置
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {FUNC_POWER_ON_SCAN,                func_power_on_scan},             //开机二维码
    {FUNC_POWER_ON_LANGUAGE,            func_power_on_language},        //开机语言

    {FUNC_COMPO_SELECT,                 func_compo_select},             //组件选择
    {FUNC_COMPO_SELECT_SUB,             func_compo_select_sub},         //组件选择子界面
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    {FUNC_CALCULATOR,                   func_calculator},               //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    {FUNC_CAMERA,                       func_camera},                   //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
    {FUNC_LIGHT,                        func_light},                    //亮度调节
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    {FUNC_TIMER,                        func_timer},                    //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {FUNC_SLEEP,                        func_sleep},                    //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {FUNC_STOPWATCH,                    func_stopwatch},                //秒表
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    {FUNC_STOPWATCH_SUB_RECORD,         func_stopwatch_sub_record},     //秒表--秒表记录
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {FUNC_WEATHER,                      func_weather},                  //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {FUNC_SPORT,                        func_sport},                    //运动

//    {FUNC_SPORT_CONFIG,                 func_sport_config},             //运动配置
    {FUNC_SPORT_SUB_RUN,                func_sport_sub_run},            //运动--室内跑步
    {FUNC_SPORT_SWITCH,                 func_sport_switching},          //运动开启动画
    {FUNC_SPORT_SORT,                   func_sport_sort},               //运动变菜单
    {FUNC_SPORT_FINISH,                 func_sport_finish},             //运动变菜单
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {FUNC_GAME,                         func_game},                     //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {FUNC_STYLE,                        func_style},                    //菜单风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    {FUNC_ALTITUDE,                     func_altitude},                 //海拔
    {FUNC_FINDPHONE,                    func_findphone},                //寻找手机
    {FUNC_MAP,                          func_map},                      //地图
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {FUNC_MESSAGE,                      func_message_info},             //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    {FUNC_SCAN,                         func_scan},                     //扫一扫
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {FUNC_VOICE,                        func_voice},                    //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
//    {FUNC_COMPASS,                      func_compass},                  //指南针
#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
    {FUNC_ADDRESS_BOOK,                 func_address_book},             //电话簿
#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {FUNC_CALENDAER,                    func_calendar},                 //日历
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {FUNC_CALL,                         func_call},                     //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
    {FUNC_CALL_SUB_RECORD,              func_call_sub_record},          //电话--最近通话
    {FUNC_CALL_SUB_DIAL,                func_call_sub_dial},            //电话--拨号
    {FUNC_CALL_SORT,                    func_call_sort},                //电话--一级界面
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {FUNC_ACTIVITY,                     func_activity},                 //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {FUNC_FLASHLIGHT,                   func_flashlight},               //手电筒
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {FUNC_SETTING,                      func_set_sub_list},             //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
    {FUNC_SET_SUB_DOUSING,              func_set_sub_dousing},          //设置--熄屏
#if GUI_MODULE_WRIST_SUPPORT
    {FUNC_SET_SUB_WRIST,                func_set_sub_wrist},            //设置--抬腕
#endif
    {FUNC_SET_SUB_DISTURD,              func_set_sub_disturd},          //设置--勿扰
    {FUNC_DISTURD_SUB_SET,              func_disturd_sub_set},          //勿扰--时间设置
    {FUNC_SET_SUB_SAV,                  func_set_sub_sav},              //设置--声音与振动
    {FUNC_SET_PUFFS,                  func_set_puffs},              //设置口数
    {FUNC_TARGET_PUFFS,                  func_target_puffs},              //设置目标口数
    {FUNC_SLIDING_UNLOCK_SCREEN,                  func_sliding_unlcok_screen},              //锁屏界面
#if UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
    {FUNC_SET_SUB_LANGUAGE,             func_set_sub_language},         //设置--语言
#endif // UTE_MODULE_SCREENS_LANGUAGE_SUPPORT
//    {FUNC_SET_SUB_TIME,                 func_set_sub_time},             //设置--时间
//    {FUNC_TIME_SUB_CUSTOM,              func_time_sub_custom},          //设置--自定义时间
    {FUNC_SET_SUB_PASSWORD,             func_set_sub_password},         //设置--密码锁
    {FUNC_PASSWORD_SUB_DISP,            func_password_sub_disp},        //设置--新密码锁设置
    {FUNC_PASSWORD_SUB_SELECT,          func_password_sub_select},      //设置--密码锁确认
    {FUNC_SET_SUB_ABOUT,                func_set_sub_about},            //设置--关于
//    {FUNC_SET_SUB_4G,                   func_set_sub_4g},               //设置--4G
#if UTE_MODULE_SCREENS_RESTART_SUPPORT
    {FUNC_SET_SUB_RESTART,              func_set_sub_restart},          //设置--重启
#endif // UTE_MODULE_SCREENS_RESTART_SUPPORT
#if UTE_MODULE_SCREENS_RESFY_SUPPORT
    {FUNC_SET_SUB_RSTFY,                func_set_sub_rstfy},            //设置--恢复出厂
#endif // UTE_MODULE_SCREENS_RESFY_SUPPORT
    {FUNC_SET_SUB_OFF,                  func_set_sub_off},              //设置--关机
    {FUNC_CHARGE,                       func_charge},                   //充电
    {FUNC_DEBUG_INFO,                   func_debug_info},               //DEBUG
//    {FUNC_SMARTSTACK,                   func_smartstack},               //智能堆栈
#if FUNC_BT_EN
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {FUNC_BT,                           func_bt},
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {FUNC_BT_RING,                      func_bt_ring},
    {FUNC_BT_CALL,                      func_bt_call},
#endif // FUNC_BT_EN
#if FUNC_BT_DUT_EN
    {FUNC_BT_DUT,                       func_bt_dut},
#endif // FUNC_BT_DUT_EN
#if FUNC_MUSIC_EN
    {FUNC_MUSIC,                        func_music},
#endif
#if FUNC_USBDEV_EN
    {FUNC_USBDEV,                       func_usbdev},
#endif
#if FUNC_RECORDER_EN
    {FUNC_RECORDER,                     func_recorder},
#endif
#if FUNC_IDLE_EN
    {FUNC_IDLE,                         func_idle},
#endif
#if FOTA_UI_EN
    {FUNC_OTA_UI_MODE,                  func_ota_ui},
#endif
    {FUNC_MODEM_CALL,                   func_modem_call},
    {FUNC_MODEM_RING,                   func_modem_ring},
    {FUNC_BT_UPDATE,                    func_bt_update},
#if FLASHDB_EN
    {FUNC_MESSAGE_REPLY,                func_message_reply_info},
#endif
#if MIC_TEST_EN
    //  {FUNC_MIC_TEST,                     func_mic_test},
#endif // MIC_TEST_EN
    {FUNC_BIRD,                         func_bird},
//    {FUNC_TETRIS,                       func_tetris},
//    {FUNC_TETRIS_START,                 func_tetris_start},
    {FUNC_OTA_MODE,                     func_ota_update},
    {FUNC_OTA_ERROR,                    func_ota_err},
    {FUNC_OTA_SUCC,                     func_ota_succ},
    {FUNC_BLE_CALL,                     func_ble_call},
//    {FUNC_SET_SUB_SOS,                  func_set_sub_sos},
#if UTE_MODULE_SCREENS_POWERON_SUPPORT
    {FUNC_POWER_ON,                     func_power_on},
#endif
    {FUNC_TEST_MODE,                    func_test_mode}, ///*出厂测试模式选择*/
    {FUNC_FACTORY_TESTING,              func_factory_testing},///*工厂测试*/
    {FUNC_AGEING,                       func_ageing},///*老化测试*/
    {FUNC_AUDIO,                        func_audio},///*音频测试*/
    {FUNC_ONLINE_FACTORY_TEST,          func_online_factory_test},
    {FUNC_EMPTY,                        func_empty},

#if ECIG_POWER_CONTROL
    {FUNC_ECIG_SET_POWER,               func_ecig_set_power},
    {FUNC_ECIG_REMINDER,                func_ecig_reminder},
    {FUNC_ECIG_REMINDERCLOCK,                func_ecig_reminderclock},
#endif
#if UTE_MODULE_SCREENS_CALENDAER_SUB_SUPPORT
    {FUNC_CALENDAER_SUB,                func_calendar_sub},
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    {FUNC_ECIG_VPAE_SUB,                func_ecig_vpae_sub},
};


AT(.text.func.process)  // 编译器指令，将此函数放在特定的内存段(.text.func.process)中  
void func_watch_bt_process(void)  // 函数定义：蓝牙通话状态处理函数，无参数，无返回值  
{  
    uint disp_status = bsp_bt_disp_status();  // 调用BSP层函数获取当前蓝牙显示状态，存储在disp_status变量中  
  
    if (disp_status == BT_STA_OTA)  // 判断蓝牙状态是否为OTA升级状态  
    {  
        sfunc_bt_ota();  // 如果是OTA状态，调用蓝牙OTA升级处理函数  
    }  
#if !CALL_MGR_EN  // 编译条件：如果没有启用通话管理器功能  
    else if (sys_cb.reject_tick)  // 否则，如果系统控制块中的拒接计时器不为0（表示正在拒接通话）  
    {  
        if (tick_check_expire(sys_cb.reject_tick, 3000) || disp_status < BT_STA_INCOMING)  // 检查拒接计时器是否超过3000ms，或者蓝牙状态小于来电状态  
        {  
            sys_cb.reject_tick = 0;  // 清除拒接计时器，结束拒接状态  
        }  
#if BT_VOIP_REJECT_EN  // 编译条件：如果启用了VOIP拒接功能  
    }  
    else if (bt_cb.disp_status == BT_STA_INCOMING && bt_cb.call_type == CALL_TYPE_PHONE)  // 否则，如果蓝牙控制块显示状态为来电且通话类型为普通电话  
    {  
#else  // 如果没有启用VOIP拒接功能  
    }  
    else if (bt_cb.disp_status == BT_STA_INCOMING)  // 否则，如果蓝牙控制块显示状态为来电（不区分通话类型）  
    {  
#endif  
        if (func_cb.sta != FUNC_BT_RING)  // 如果当前功能状态不是蓝牙来电响铃状态  
        {  
            func_cb.sta = FUNC_BT_RING;  // 设置当前功能状态为蓝牙来电响铃  
            msg_enqueue(EVT_MSGBOX_EXIT);  // 向消息队列发送退出消息框事件  
        }  
  
#if BT_VOIP_REJECT_EN  // 编译条件：如果启用了VOIP拒接功能  
    }  
    else if (bt_cb.disp_status >= BT_STA_OUTGOING && bt_cb.call_type == CALL_TYPE_PHONE)  // 否则，如果蓝牙状态大于等于呼出状态且通话类型为普通电话  
    {  
#else  // 如果没有启用VOIP拒接功能  
    }  
    else if (bt_cb.disp_status >= BT_STA_OUTGOING)  // 否则，如果蓝牙状态大于等于呼出状态（包括呼出、通话中等状态）  
    {  
#endif  
        if (func_cb.sta != FUNC_BT_CALL)  // 如果当前功能状态不是蓝牙通话状态  
        {  
            func_cb.sta = FUNC_BT_CALL;  // 设置当前功能状态为蓝牙通话  
            msg_enqueue(EVT_MSGBOX_EXIT);  // 向消息队列发送退出消息框事件  
        }  
    }  
#endif  // 结束CALL_MGR_EN条件编译  
}
/**  
*@brief        定期打印系统调试信息  
*@details      每秒打印一次内存监控、蓝牙状态和线程信息，用于系统调试  
*@author       system  
*@date         2024-07-21  
*/  
void print_info(void)  // 函数定义：调试信息打印函数，用于定期输出系统状态信息  
{  
    static u32 ticks = 0;  // 静态变量：保存上次执行时的时间戳，函数退出后值不会丢失  
    if (tick_check_expire(ticks, 1000))  // 检查距离上次执行是否已过1000ms（1秒）  
    {  
        ticks = tick_get();  // 更新时间戳为当前系统时间  
        extern void mem_monitor_run(void);  // 外部函数声明：内存监控运行函数  
        mem_monitor_run();  // 调用内存监控函数，检查内存使用情况  
        printf("sys_cb.sco_state[%d], bt_cb.call_type[%d], bt_cb.disp_status[%d]\\n", sys_cb.sco_state, bt_cb.call_type, bt_cb.disp_status);  // 打印系统控制块的SCO状态、蓝牙控制块的通话类型和显示状态  
        extern void thread_info_printf(void);  // 外部函数声明：线程信息打印函数  
        thread_info_printf();  // 调用线程信息打印函数，输出当前线程状态  
    }  
}  

  
/**  
*@brief        系统主处理函数  
*@details      处理看门狗、GUI更新、音频编码、传感器、电源管理、蓝牙通信、事件处理等核心系统任务  
*@author       system  
*@date         2024-07-21  
*/  
AT(.text.func.process)  // 编译器指令：将函数放在特定内存段中  
void func_process(void)  // 函数定义：系统主处理函数，负责处理各种系统任务  
{  
    WDT_CLR();  // 清除看门狗定时器，防止系统复位  
    // print_info();  // 注释掉的调试信息打印调用  
  
#if (FUNC_MUSIC_EN || FUNC_RECORDER_EN) && SD_SOFT_DETECT_EN  // 条件编译：如果启用音乐或录音功能且启用SD卡软检测  
    sd_soft_cmd_detect(120);  // SD卡软件检测，参数120为检测间隔  
#endif  
  
    tft_bglight_frist_set_check();  // 检查并设置TFT背光的首次设置  
    // gui 没有休眠才更新 防止客户乱改我们的唤醒流程导致卡主的问题  // 注释：说明GUI更新的条件  
#if FOTA_UI_EN  // 条件编译：如果启用FOTA UI功能  
    if ((!sys_cb.gui_sleep_sta) /*&& (func_cb.sta != FUNC_OTA_UI_MODE)*/ && !sys_cb.flag_halt)  // 如果GUI未休眠且系统未暂停  
    {  
#else  // 如果未启用FOTA UI功能  
    if (!sys_cb.gui_sleep_sta && !sys_cb.flag_halt)  // 如果GUI未休眠且系统未暂停  
    {  
#endif  
        compo_update();                                     //更新组件  // 更新GUI组件状态  
        gui_process();                                      //刷新UI   // 处理GUI刷新  
    }  
  
#if OPUS_ENC_EN  // 条件编译：如果启用OPUS编码功能  
    if (bsp_opus_is_encode())  // 如果OPUS正在编码  
    {  
        reset_sleep_delay_all();  // 重置所有睡眠延时，保持系统活跃  
        u8 temp_buf[80];  // 定义80字节的临时缓冲区  
        memset(temp_buf, 0, sizeof(temp_buf));  // 将缓冲区清零  
        if (bsp_opus_get_enc_frame(temp_buf, sizeof(temp_buf)))  // 获取OPUS编码帧数据  
        {  
            print_r(temp_buf, sizeof(temp_buf));  // 打印编码帧数据（十六进制格式）  
        }  
    }  
#endif//OPUS_ENC_EN  
  
    co_timer_pro(false);  // 协作定时器处理，参数false表示非阻塞模式  
    bsp_sensor_step_pro_isr();  // 传感器步数处理中断服务程序  
  
    if (sys_cb.mp3_res_playing)  // 如果系统正在播放MP3资源文件  
    {  
        mp3_res_process();                                 //提示音后台处理  // 处理MP3提示音播放  
    }  
  
    if (sleep_process(bt_is_allow_sleep))  // 处理系统睡眠，参数为蓝牙是否允许睡眠的函数  
    {  
        bt_cb.disp_status = 0xff;  // 如果进入睡眠，设置蓝牙显示状态为无效值  
    }  
  
    if (sys_cb.remind_tag)   //闹钟响起  // 如果闹钟提醒标志被设置  
    {  
        sys_cb.remind_tag = false;  // 清除闹钟提醒标志  
        gui_cover_msg_enqueue(sys_cb.cover_index);  // 将覆盖消息加入队列  
        printf(">>>REMIND POP\\n");  // 打印闹钟弹出调试信息  
    }  
  
    if (sys_cb.msg_tag)   //有消息弹出  // 如果消息标志被设置  
    {  
        sys_cb.msg_tag = false;  // 清除消息标志  
        app_msg_pop_up(sys_cb.msg_index);  // 弹出应用消息  
        printf(">>>MSG POP\\n");  // 打印消息弹出调试信息  
    }  
  
    if (sys_cb.timer_done)  // 如果定时器完成标志被设置  
    {  
        sys_cb.timer_done = false;  // 清除定时器完成标志  
        msg_enqueue(EVT_MSGBOX_EXIT);  // 向消息队列发送退出消息框事件  
        msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);  // 向消息队列发送退出时钟下拉事件  
        msg_enqueue(EVT_WATCH_TIMER_DONE);  // 向消息队列发送手表定时器完成事件  
        printf(">>>TIMER DONE\\n");  // 打印定时器完成调试信息  
    }  
  
#if VBAT_DETECT_EN  // 条件编译：如果启用电池电压检测  
    bsp_vbat_lpwr_process();  // 处理电池低电压检测  
#endif  
  
#if BT_BACKSTAGE_EN  // 条件编译：如果启用蓝牙后台处理  
    func_watch_bt_process();  // 调用蓝牙状态处理函数  
#endif  
  
#if CALL_MGR_EN  // 条件编译：如果启用通话管理器  
    bsp_call_mgr_process();  // 处理通话管理器  
#endif  
  
    //PWRKEY模拟硬开关关机处理  // 注释：电源键模拟硬件开关关机处理  
    if ((PWRKEY_2_HW_PWRON) && (sys_cb.pwrdwn_hw_flag))  // 如果启用电源键硬件开关且关机标志被设置  
    {  
        func_cb.sta = FUNC_PWROFF;  // 设置功能状态为关机  
        sys_cb.pwrdwn_hw_flag = 0;  // 清除硬件关机标志  
    }  
  
#if CHARGE_EN  // 条件编译：如果启用充电功能  
    if (xcfg_cb.charge_en)  // 如果配置中启用了充电  
    {  
        charge_detect(1);  // 检测充电状态，参数1表示启用检测  
        if (bsp_charge_sta_get())  // 如果检测到充电状态  
        {  
            if (bt_cb.disp_status <= BT_STA_PLAYING && func_cb.sta != FUNC_OTA_UI_MODE && is_fot_start() == 0)  // 如果蓝牙状态小于等于播放状态且不在OTA模式且FOT未启动  
            {  
                if (charge_dc_change_detect())  // 如果检测到充电器插拔变化  
                {  
                    if (sys_cb.gui_sleep_sta)           //亮屏  // 如果GUI处于休眠状态  
                    {  
                        sys_cb.gui_need_wakeup = 1;  // 设置GUI需要唤醒标志  
                    }  
                    reset_sleep_delay_all();  // 重置所有睡眠延时  
                }  
                if (func_cb.sta != FUNC_CHARGE)  // 如果当前功能状态不是充电状态  
                {  
                    msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);  // 发送退出时钟下拉事件  
                    msg_enqueue(EVT_MSGBOX_EXIT);  // 发送退出消息框事件  
                }  
                if(func_cb.sta != FUNC_AGEING &&func_cb.sta != FUNC_FACTORY_TESTING )  // 如果不在老化测试和工厂测试状态  
                    func_cb.sta = FUNC_CHARGE;  // 设置功能状态为充电  
            }  
        }  
    }  
#endif // CHARGE_EN  
  
    if(bt_cb.bt_is_inited)  // 如果蓝牙已初始化  
    {  
        bt_thread_check_trigger();  // 检查蓝牙线程触发  
#if LE_EN  // 条件编译：如果启用低功耗蓝牙  
        ble_app_process();  // 处理BLE应用  
#endif  
#if LE_AB_FOT_EN  // 条件编译：如果启用低功耗蓝牙AB FOT功能  
        bsp_fot_process();  // 处理FOT（固件空中升级）  
        if (is_fot_start() == true)  // 如果FOT已启动  
        {  
            reset_sleep_delay_all()  // 重置所有睡眠延时（注意：这里缺少分号，可能是代码错误）  
            if (sys_cb.gui_sleep_sta)  // 如果GUI处于休眠状态  
            {  
                sys_cb.gui_need_wakeup = 1;  // 设置GUI需要唤醒标志  
            }  
        }  
#endif  
    }  
#if ASR_SELECT  // 条件编译：如果选择了ASR（自动语音识别）  
    if(bsp_asr_init_sta())  // 如果ASR初始化状态为真  
    {  
        delay_5ms(2);  // 延时10ms（2*5ms）  
    }  
#endif  
#if (ASR_SELECT == ASR_YJ && ASR_DEAL_TYPE)  // 条件编译：如果选择了YJ ASR且启用ASR处理类型  
    third_func_process();  // 处理第三方功能  
#endif  
#if (ASR_SELECT == ASR_WS && MONITOR_ASR_WAKEUP_EN)  // 条件编译：如果选择了WS ASR且启用ASR唤醒监控  
    asr_event_process();  // 处理ASR事件  
#endif  
#if (ASR_SELECT == ASR_WS_AIR && MONITOR_ASR_WAKEUP_EN)  // 条件编译：如果选择了WS AIR ASR且启用ASR唤醒监控  
    asr_event_process();  // 处理ASR事件  
#endif  
}

/**  
*@brief        根据任务状态创建对应的窗体  
*@details      通过查找函数表找到对应的窗体创建函数并调用，如果找不到则系统停机  
*@param        sta 任务状态标识符  
*@retval       compo_form_t* 创建的窗体指针  
*@author       system  
*@date         2024-07-21  
*/  
//根据任务名创建窗体。此处调用的创建窗体函数不要调用子任务的控制结构体  
compo_form_t *func_create_form(u8 sta)                                         // 函数定义：根据任务状态创建窗体  
{  
    // printf("%s->sta:%d\\n", __func__, sta);                                 // 注释掉的调试打印  
    compo_form_t *frm = NULL;                                                  // 初始化窗体指针为空  
    compo_form_t *(*func_create)(void) = NULL;                                 // 定义函数指针，指向窗体创建函数  
    for (int i = 0; i < FUNC_CREATE_CNT; i++)                                  // 遍历函数创建表  
    {  
        if (tbl_func_create[i].func_idx == sta)                                // 如果找到匹配的任务状态  
        {  
            func_create = tbl_func_create[i].func;                             // 获取对应的创建函数指针  
            if (func_create != NULL)                                           // 如果函数指针不为空  
            {  
                frm = func_create();                                           // 调用创建函数创建窗体  
            }  
            break;                                                             // 找到后跳出循环  
        }  
    }  
    if (frm == NULL)                                                           // 如果窗体创建失败  
    {  
        // printf("halt %s->sta:%d\\n", __func__, sta);                        // 注释掉的调试打印  
        halt(HALT_FUNC_SORT);                                                  // 系统停机，错误代码为HALT_FUNC_SORT  
    }  
    return frm;                                                                // 返回创建的窗体指针  
  
}

/**  
*@brief        获取指定任务在排序表中的位置  
*@details      在任务排序表中查找指定任务状态的索引位置  
*@param        sta 任务状态标识符  
*@retval       int 任务在排序表中的索引，未找到返回-1  
*@author       system  
*@date         2024-07-21  
*/  
//获取当前任务顺序  
static int func_get_order(u8 sta)                                             // 函数定义：获取任务在排序表中的位置  
{  
    int i;                                                                     // 循环变量  
    for (i=0; i<func_cb.sort_cnt; i++)                                        // 遍历任务排序表  
    {  
        if (sta == func_cb.tbl_sort[i])                                       // 如果找到匹配的任务状态  
        {  
            return i;                                                          // 返回当前索引位置  
        }  
    }  
    return -1;                                                                 // 未找到返回-1  
}

/**  
*@brief        切换到上一个任务  
*@details      根据当前任务在排序表中的位置切换到前一个任务，支持自动和手动切换模式  
*@param        flag_auto 是否为自动切换模式  
*@retval       void  
*@author       system  
*@date         2024-07-21  
*/  
//切换到上一个任务  
void func_switch_prev(bool flag_auto)                                         // 函数定义：切换到上一个任务  
{  
    u16 switch_mode = flag_auto ? (FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO) : FUNC_SWITCH_LR_ZOOM_RIGHT;  // 根据自动标志设置切换模式  
    s8 idx = func_get_order(func_cb.sta);                                     // 获取当前任务在排序表中的索引  

    if (idx <= 0)                                                             // 如果索引小于等于0（已经是第一个或未找到）  
    {  
        return;                                                               // 直接返回，不进行切换  
    }  
    u8 sta = func_cb.tbl_sort[idx - 1];                                       // 获取上一个任务的状态  
    compo_form_t *frm = func_create_form(sta);                                //创建下一个任务的窗体  
    bool res = func_switching(switch_mode, NULL);                             //切换动画  
    compo_form_destroy(frm);                                                  //切换完成或取消，销毁窗体  
    if (res)                                                                  // 如果切换成功  
    {  
        if (sta == FUNC_CLOCK)                                                // 如果切换到时钟界面  
        {  
            func_cb.flag_sort = false;                                        // 清除排序标志  
        }  
        func_cb.sta = sta;                                                    // 更新当前任务状态  
    }  
}

//切换到下一个任务
/**  
  * @brief  切换到下一个任务界面  
  * @param  flag_auto: 是否自动切换（true=自动，false=手动）  
  * @param  flag_loop: 是否循环切换（true=循环到第一个，false=到末尾停止）  
  * @retval None  
    通过 func_get_order(func_cb.sta) 获取当前 FUNC_CLOCK 在排序表中的索引（返回0）
    计算下一个界面：sta = func_cb.tbl_sort[idx + 1]，即 FUNC_ECIG_SET_POWER
    创建新界面的窗体：func_create_form(sta)s
    执行切换动画：func_switching(switch_mode, NULL)
    更新状态：func_cb.sta = sta（即 FUNC_ECIG_SET_POWER）
  */  
void func_switch_next(bool flag_auto, bool flag_loop)  
{  
    // 根据是否自动切换设置切换模式  
    u16 switch_mode = flag_auto ? (FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO) : FUNC_SWITCH_LR_ZOOM_LEFT;  
      
    // 获取当前任务在排序表中的索引位置  
    u8 idx = func_get_order(func_cb.sta);  
    if (idx < 0)                                                    // 如果当前任务不在排序表中  
    {  
        return ;                                                    // 直接返回，不执行切换  
    }  
      
    u8 sta;                                                         // 声明下一个任务状态变量  
    if (idx >= func_cb.sort_cnt - 1)                               // 如果当前已经是最后一个任务  
    {  
        if (flag_loop)                                              // 如果允许循环切换  
        {  
            sta = func_cb.tbl_sort[0];                              // 切换到第一个任务（循环）  
        }  
        else                                                        // 如果不允许循环  
        {  
            return ;                                                // 直接返回，不执行切换  
        }  
    }  
    else                                                            // 如果不是最后一个任务  
    {  
        sta = func_cb.tbl_sort[idx + 1];                            // 切换到下一个任务  
    }  
  
    compo_form_t *frm = func_create_form(sta);                      // 创建下一个任务的窗体  
    bool res = func_switching(switch_mode, NULL);                   // 执行切换动画  
    compo_form_destroy(frm);                                        // 切换完成或取消后，销毁窗体  
      
    if (res)                                                        // 如果切换成功  
    {  
        if (sta == FUNC_CLOCK)                                      // 如果切换到时钟界面  
        {  
            func_cb.flag_sort = false;                              // 清除排序模式标志  
        }  
        else                                                        // 如果切换到其他界面  
        {  
            func_cb.flag_sort = true;                               // 设置排序模式标志  
        }  
        func_cb.sta = sta;                                          // 更新当前任务状态  
    }  
}

//切换
void func_switch_to(u8 sta, u16 switch_mode)
{
    compo_form_t *frm = func_create_form(sta);                                  //创建下一个任务的窗体
    bool res = 0;
    u8 alph = 255;
    if (sta == FUNC_CARD && switch_mode == FUNC_SWITCH_LR_ZOOM_RIGHT)
    {
        res = func_switching(switch_mode, &alph);                               //切换动画
    }
    else
    {
        res = func_switching(switch_mode, NULL);                               //切换动画
    }
    compo_form_destroy(frm);                                                    //切换完成或取消，销毁窗体
    if (res)
    {
        func_cb.sta = sta;
    }
}

/**  
*@brief        切换回主时钟界面  
*@details      使用右侧缩放自动切换模式返回到时钟界面，并清除排序标志  
*@param        void  
*@retval       void  
*@author       system  
*@date         2024-07-21  
*/  
//切换回主时钟  
void func_switch_to_clock(void)                                                // 函数定义：切换回主时钟界面  
{  
    func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);  // 调用通用切换函数，使用右侧缩放自动切换模式切换到时钟界面  
    func_cb.flag_sort = false;                                                 // 清除排序标志，表示不在快捷任务模式  
}

/**  
*@brief        退回到主菜单界面  
*@details      根据不同菜单样式选择相应的切换动画效果，支持多种菜单组件类型  
*@param        void  
*@retval       void  
*@author       system  
*@date         2024-07-21  
*/  
//退回到主菜单  
void func_switch_to_menu(void)                                                 // 函数定义：退回到主菜单界面  
{  
    u16 switch_mode;                                                           // 定义切换模式变量  
    bool flag_frm_menu;                                                        //是否需要创建菜单窗体  
    flag_frm_menu = true;                                                      // 默认需要创建菜单窗体  

    if (func_cb.menu_style == MENU_STYLE_FOOTBALL)                             // 如果菜单样式是足球样式  
    {  
        switch_mode = FUNC_SWITCH_FADE_OUT | FUNC_SWITCH_AUTO;;                // 使用淡出自动切换模式  
        flag_frm_menu = false;                                                 // 不需要创建菜单窗体  
    }  
    else if (func_cb.sta != FUNC_CLOCK || func_cb.menu_style == MENU_STYLE_HONEYCOMB)  // 如果当前不在时钟界面或菜单样式是蜂窝样式  
    {  
        switch_mode = FUNC_SWITCH_ZOOM_EXIT | FUNC_SWITCH_AUTO;                // 使用缩放退出自动切换模式  
    }  
    else if (func_cb.menu_style == MENU_STYLE_WATERFALL)                       // 如果菜单样式是瀑布流样式  
    {  
        switch_mode = FUNC_SWITCH_FADE_OUT | FUNC_SWITCH_AUTO;                 // 使用淡出自动切换模式  
        func_cb.flag_animation = true;                                         //淡出后进入入场动画  
        flag_frm_menu = false;                                                 // 不需要创建菜单窗体  
    }  
    else                                                                       // 其他菜单样式  
    {  
        switch_mode = FUNC_SWITCH_ZOOM_FADE_EXIT | FUNC_SWITCH_AUTO;           // 使用缩放淡出自动切换模式  
    }  
    if (flag_frm_menu)                                                         // 如果需要创建菜单窗体  
    {  
        widget_icon_t *icon;                                                   // 定义图标指针  
        compo_form_t *frm = func_create_form(FUNC_MENU);                       //创建下一个任务的窗体  

        #if (ASR_SELECT && ASR_VOICE_BALL_ANIM)                                        // 条件编译：如果启用ASR语音球动画  
                component_t *compo = compo_get_next((component_t *)frm->anim);         // 获取动画组件的下一个组件  
        #else                                                                          // 如果未启用ASR语音球动画  
                component_t *compo = compo_get_next((component_t *)frm->title);        // 获取标题组件的下一个组件  
        #endif  
  
        if (compo->type == COMPO_TYPE_ICONLIST)                                // 如果组件类型是图标列表  
        {  
            compo_iconlist_t *iconlist = (compo_iconlist_t *)compo;            // 转换为图标列表组件指针  
            icon = compo_iconlist_select_byidx(iconlist, func_cb.menu_idx);    // 根据菜单索引选择图标  
        }  
        else if (compo->type == COMPO_TYPE_LISTBOX)                            // 如果组件类型是列表框  
        {  
            compo_listbox_t *listbox = (compo_listbox_t *)compo;               // 转换为列表框组件指针  
            icon = compo_listbox_select_byidx(listbox, func_cb.menu_idx);      // 根据菜单索引选择图标  
        }  
        else if (compo->type == COMPO_TYPE_DISKLIST)                           // 如果组件类型是圆盘列表  
        {  
            compo_disklist_t *disklist = (compo_disklist_t *)compo;            // 转换为圆盘列表组件指针  
            icon = compo_disklist_select_byidx(disklist, func_cb.menu_idx);    // 根据菜单索引选择图标  
        }  
        else if (compo->type == COMPO_TYPE_KALEIDOSCOPE)                       // 如果组件类型是万花筒  
        {  
            compo_kaleidoscope_t *kale = (compo_kaleidoscope_t *)compo;        // 转换为万花筒组件指针  
            icon = compo_kale_select_byidx(kale, func_cb.menu_idx);            // 根据菜单索引选择图标  
        }  
        else if (compo->type == COMPO_TYPE_RINGS)                              // 如果组件类型是环形菜单  
        {  
            compo_rings_t *rings = (compo_rings_t *)compo;                     // 转换为环形菜单组件指针  
            icon = compo_rings_select_byidx(rings, func_cb.menu_idx);          // 根据菜单索引选择图标  
            if (icon == NULL)                                                  // 如果图标为空  
            {  
                extern u8 func_menu_sub_skyrer_get_first_idx(void);           // 外部函数声明：获取第一个有效索引  
                func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();       // 获取第一个有效菜单索引  
                icon = compo_rings_select_byidx(rings, func_cb.menu_idx);      // 重新根据索引选择图标  
            }  
        }  
        else                                                                   // 如果组件类型不匹配任何已知类型  
        {  
            halt(HALT_GUI_COMPO_ICONLIST_TYPE);                                // 系统停机，错误代码为组件类型错误  
            return;                                                            // 返回，不继续执行  
        }  
        func_switching(switch_mode, icon);                                     //退出动画  
        compo_form_destroy(frm);                                               //切换完成或取消，销毁窗体  
    }  
    else                                                                       // 如果不需要创建菜单窗体  
    {  
        func_switching(switch_mode, NULL);                                     //退出动画  
    }  
    func_cb.sta = FUNC_MENU;                                                   // 设置当前功能状态为菜单  
}


/**  
*@brief        手动退回到主菜单界面  
*@details      手动触发的菜单切换，根据当前状态选择切换动画，只有切换成功才更新状态  
*@param        void  
*@retval       void  
*@author       system  
*@date         2024-07-21  
*/  
//手动退回到主菜单  
void func_switching_to_menu(void)                                              // 函数定义：手动退回到主菜单界面  
{  
    widget_icon_t *icon;                                                       // 定义图标指针  
    u16 switch_mode;                                                           // 定义切换模式变量  
    compo_form_t *frm = func_create_form(FUNC_MENU);                           //创建下一个任务的窗体  

    #if (ASR_SELECT && ASR_VOICE_BALL_ANIM)                                        // 条件编译：如果启用ASR语音球动画  
        component_t *compo = compo_get_next((component_t *)frm->anim);             // 获取动画组件的下一个组件  
    #else                                                                          // 如果未启用ASR语音球动画  
        component_t *compo = compo_get_next((component_t *)frm->title);            // 获取标题组件的下一个组件  
    #endif  

    
    if (compo->type == COMPO_TYPE_ICONLIST)                                    // 如果组件类型是图标列表  
    {  
        compo_iconlist_t *iconlist = (compo_iconlist_t *)compo;                // 转换为图标列表组件指针  
        icon = compo_iconlist_select_byidx(iconlist, func_cb.menu_idx);        // 根据菜单索引选择图标  
    }  
    else if (compo->type == COMPO_TYPE_LISTBOX)                                // 如果组件类型是列表框  
    {  
        compo_listbox_t *listbox = (compo_listbox_t *)compo;                   // 转换为列表框组件指针  
        icon = compo_listbox_select_byidx(listbox, func_cb.menu_idx);          // 根据菜单索引选择图标  
    }  
    else if (compo->type == COMPO_TYPE_DISKLIST)                               // 如果组件类型是圆盘列表  
    {  
        compo_disklist_t *disklist = (compo_disklist_t *)compo;                // 转换为圆盘列表组件指针  
        icon = compo_disklist_select_byidx(disklist, func_cb.menu_idx);        // 根据菜单索引选择图标  
    }  
    else if (compo->type == COMPO_TYPE_KALEIDOSCOPE)                           // 如果组件类型是万花筒  
    {  
        compo_kaleidoscope_t *kale = (compo_kaleidoscope_t *)compo;            // 转换为万花筒组件指针  
        icon = compo_kale_select_byidx(kale, func_cb.menu_idx);                // 根据菜单索引选择图标  
    }  
    else if (compo->type == COMPO_TYPE_RINGS)                                  // 如果组件类型是环形菜单  
    {  
        compo_rings_t *rings = (compo_rings_t *)compo;                         // 转换为环形菜单组件指针  
        icon = compo_rings_select_byidx(rings, func_cb.menu_idx);              // 根据菜单索引选择图标  
        if (icon == NULL)                                                      // 如果图标为空  
        {  
            extern u8 func_menu_sub_skyrer_get_first_idx(void);               // 外部函数声明：获取第一个有效索引  
            func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();           // 获取第一个有效菜单索引  
            icon = compo_rings_select_byidx(rings, func_cb.menu_idx);          // 重新根据索引选择图标  
        }  
    }  
    else                                                                       // 如果组件类型不匹配任何已知类型  
    {  
        halt(HALT_GUI_COMPO_ICONLIST_TYPE);                                    // 系统停机，错误代码为组件类型错误  
        return;                                                                // 返回，不继续执行  
    }  
    if (func_cb.sta != FUNC_CLOCK || func_cb.menu_style == MENU_STYLE_HONEYCOMB)  // 如果当前不在时钟界面或菜单样式是蜂窝样式  
    {  
        switch_mode = FUNC_SWITCH_ZOOM_EXIT;                                   // 使用缩放退出切换模式（非自动）  
    }  
    else                                                                       // 其他情况  
    {  
        switch_mode = FUNC_SWITCH_ZOOM_FADE_EXIT;                              // 使用缩放淡出退出切换模式（非自动）  
    }  
    bool res = func_switching(switch_mode, icon);                              //退出动画  
    compo_form_destroy(frm);                                                   //切换完成或取消，销毁窗体  
    if (res)                                                                   // 如果切换成功  
    {  
        func_cb.sta = FUNC_MENU;                                               // 设置当前功能状态为菜单  
    }  
}

/**  
*@brief        页面滑动回退功能  
*@details      处理右滑手势触发的页面回退，支持任务栈管理和特殊状态处理  
*@param        void  
*@retval       void  
*@author       system  
*@date         2024-07-21  
*/  
//页面滑动回退功能  
void func_backing_to(void)                                                    
{  
    if (func_cb.sta == FUNC_BT_RING || func_cb.sta == FUNC_BT_CALL)           // 如果当前状态是蓝牙来电或通话中  
    {  
                                                                              // 空语句块，表示在通话状态下不处理回退  
        return;                                                               // 直接返回，不执行回退操作  
    }  
  
    u8 stack_top = task_stack_pop();                                          // 从任务栈中弹出上一个任务状态  
  
    if (!stack_top)                                                           // 如果栈顶为空（栈为空或异常情况）  
    {  
        stack_top = FUNC_CLOCK;                                               //异常返回表盘  
    }  
  
    if (stack_top == FUNC_MENU || FUNC_MENUSTYLE == stack_top)                // 如果要返回的是菜单或菜单样式界面  
    {  
        func_switching_to_menu();                                             //右滑缓慢退出任务  
    }  
    else                                                                      // 如果要返回的是其他界面  
    {  
        func_switch_to(stack_top, FUNC_SWITCH_LR_ZOOM_RIGHT);                 //返回上一个界面  
    }  
    if (func_cb.sta != stack_top)                                             //如果页面没切换需要重新入栈  
    {  
        task_stack_push(func_cb.sta);                                         // 将当前状态重新压入任务栈  
    }  
}


/**  
*@brief        页面按键回退功能  
*@details      处理按键触发的页面回退，支持音乐功能特殊处理和自动切换动画  
*@param        void  
*@retval       void  
*@author         
*@date         2024-07-21  
*/  
//页面按键回退功能  
void func_back_to(void)                                                      
{  
    if (func_cb.sta == FUNC_BT_RING || func_cb.sta == FUNC_BT_CALL)          // 如果当前状态是蓝牙来电或通话中  
    {  
        return;                                                              // 直接返回，不执行回退操作  
    }  
  
    u8 stack_top = task_stack_pop();                                         // 从任务栈中弹出上一个任务状态  
  
    if (!stack_top)                                                          // 如果栈顶为空（栈为空或异常情况）  
    {  
        stack_top = FUNC_CLOCK;                                              //异常返回表盘  
    }  
  
    if (stack_top == FUNC_MENU                                               // 如果要返回的是菜单界面  
#if FUNC_MUSIC_EN                                                            // 条件编译：如果启用音乐功能  
        || func_cb.sta == FUNC_MUSIC                                         // 或者当前状态是音乐界面  
#endif  
       )  
    {  
        func_switch_to_menu();                                               //返回主菜单  
    }  
    else                                                                     // 如果要返回的是其他界面  
    {  
        func_switch_to(stack_top, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);    //返回上一个界面  
    }  
}

/**  
*@brief        页面直接回退功能  
*@details      无动画效果的直接页面回退，直接修改状态不执行切换动画  
*@param        void  
*@retval       u8 返回的目标任务状态  
*@author         
*@date         2024-07-21  
*/  
//页面直接回退,无动画效果  
u8 func_directly_back_to(void)                                               
{  
    u8 stack_top = task_stack_pop();                                         // 从任务栈中弹出上一个任务状态  
  
    func_cb.sta = stack_top;                                                 // 直接设置当前功能状态为栈顶状态  
  
    return stack_top;                                                        // 返回目标任务状态  
}

/**  
*@brief        系统事件消息处理函数  
*@details      处理蓝牙、音频、设备插拔、传感器等各种系统事件消息  
*@param        msg 事件消息类型  
*@retval       void  
*@author         
*@date         2024-07-21  
*/  
void evt_message(size_msg_t msg)                                            
{  
    switch (msg)                                                             // 根据消息类型进行分支处理  
    {  
        case EVT_READY_EXCHANGE_MTU:                                         // BLE准备交换MTU事件  
            ble_exchange_mtu_request();                                      // 发起BLE MTU交换请求  
            break;                                                           // 跳出switch语句  
  
#if BT_CONNECT_REQ_FROM_WATCH_EN                                             // 条件编译：如果启用手表主动连接蓝牙功能  
        case EVT_BT_CONNECT_ONCE:                                            // 蓝牙单次连接事件  
            bt_connect();                                                    // 执行蓝牙连接操作  
            break;                                                           // 跳出switch语句  
#endif  
  
#if MUSIC_UDISK_EN                                                           // 条件编译：如果启用U盘音乐功能  
        case EVT_UDISK_INSERT:                                               // U盘插入事件  
            if (dev_is_online(DEV_UDISK))                                    // 如果U盘设备在线  
            {  
                if (dev_udisk_activation_try(0))                            // 尝试激活U盘设备  
                {  
                    sys_cb.cur_dev = DEV_UDISK;                              // 设置当前设备为U盘  
                    func_cb.sta = FUNC_MUSIC;                                // 切换到音乐功能界面  
                }  
            }  
            break;                                                           // 跳出switch语句  
#endif // MUSIC_UDISK_EN  
  
#if MUSIC_SDCARD_EN                                                          // 条件编译：如果启用SD卡音乐功能  
        case EVT_SD_INSERT:                                                  // SD卡插入事件  
            if (dev_is_online(DEV_SDCARD))                                   // 如果SD卡设备在线  
            {  
                sys_cb.cur_dev = DEV_SDCARD;                                 // 设置当前设备为SD卡  
                func_cb.sta = FUNC_MUSIC;                                    // 切换到音乐功能界面  
            }  
            break;                                                           // 跳出switch语句  
#endif // MUSIC_SDCARD_EN  
  
#if FUNC_USBDEV_EN                                                           // 条件编译：如果启用USB设备功能  
        case EVT_PC_INSERT:                                                  // PC连接事件  
            if (dev_is_online(DEV_USBPC))                                    // 如果USB PC设备在线  
            {  
                func_cb.sta = FUNC_USBDEV;                                   // 切换到USB设备功能界面  
            }  
            break;                                                           // 跳出switch语句  
#endif // FUNC_USBDEV_EN  
  
        case EVT_HFP_SET_VOL:                                                // HFP音量设置事件  
            if(sys_cb.incall_flag & INCALL_FLAG_SCO)                        // 如果当前处于SCO通话状态  
            {  
                bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));      // 根据HFP音量值设置系统音量  
                dac_fade_in();                                               // DAC淡入效果  
                printf("HFP SET VOL: %d\\n", sys_cb.hfp_vol);                // 打印HFP音量设置调试信息  
            }  
            break;                                                           // 跳出switch语句  
  
        case EVT_A2DP_SET_VOL:                                               // A2DP音量设置事件  
            if ((sys_cb.incall_flag & INCALL_FLAG_SCO) == 0)                // 如果当前不在SCO通话状态  
            {  
                printf("A2DP SET VOL: %d\\n", sys_cb.vol);                   // 打印A2DP音量设置调试信息  
                bsp_change_volume(sys_cb.vol);                               // 设置系统音量  
                param_sys_vol_write();                                       // 将音量参数写入存储  
                sys_cb.cm_times = 0;                                         // 清零计数器  
                sys_cb.cm_vol_change = 1;                                    // 设置音量变化标志  
                if (bt_cb.music_playing)                                     // 如果蓝牙音乐正在播放  
                {  
                    dac_fade_in();                                           // DAC淡入效果  
                }  
            }  
            break;                                                           // 跳出switch语句  
  
        case EVT_A2DP_MUSIC_PLAY:                                            // A2DP音乐播放事件  
            if (!sbc_is_bypass())                                            // 如果SBC编解码器未被旁路  
            {  
                printf("EVT_A2DP_MUSIC_PLAY\\n");                            // 打印音乐播放调试信息  
                dac_fade_in();                                               //淡入前会先解MUTE  
            }  
            bt_cb.music_playing = true;                                      // 设置蓝牙音乐播放状态为真  
            break;                                                           // 跳出switch语句  
  
        case EVT_A2DP_MUSIC_STOP:                                            // A2DP音乐停止事件  
            if (!sbc_is_bypass())                                            // 如果SBC编解码器未被旁路  
            {  
                printf("EVT_A2DP_MUSIC_STOP\\n");                            // 打印音乐停止调试信息  
                dac_fade_out();                                              //淡出完成会自动MUTE功放  
                // dac_fade_wait();                                          // 注释掉的等待淡出完成  
                // bsp_sys_mute();                                           // 注释掉的系统静音  
            }  
            bt_cb.music_playing = false;                                     // 设置蓝牙音乐播放状态为假  
            break;                                                           // 跳出switch语句  
  
        case EVT_BT_SCAN_START:                                              // 蓝牙扫描启动事件  
            if (bt_get_status() < BT_STA_SCANNING)                          // 如果蓝牙状态小于扫描状态  
            {  
                bt_scan_enable();                                            // 启用蓝牙扫描  
            }  
            break;                                                           // 跳出switch语句  
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP                                         // 条件编译：如果启用UART或SPP均衡器调试  
        case EVT_ONLINE_SET_EQ:                                              // 在线设置均衡器事件  
            eq_parse_cmd();                                                  // 解析均衡器命令  
            break;                                                           // 跳出switch语句  
#endif  
  
#if EQ_MODE_EN                                                               // 条件编译：如果启用均衡器模式  
        case EVT_BT_SET_EQ:                                                  // 蓝牙设置均衡器事件  
            music_set_eq_by_num(sys_cb.eq_mode);                            // 根据均衡器模式号设置音效  
            break;                                                           // 跳出switch语句  
#endif  
  
#if BT_SCO_APP_DBG_EN                                                        // 条件编译：如果启用蓝牙SCO应用调试  
        case EVT_SCO_DBG_COMM_RSP:                                           // SCO调试通信响应事件  
        case EVT_SCO_DBG_TLV_RSP:                                            // SCO调试TLV响应事件  
        case EVT_SCO_DBG_NOTIFY:                                             // SCO调试通知事件  
            sco_app_msg_deal(msg);                                           // 处理SCO应用消息  
            break;                                                           // 跳出switch语句  
#endif  
  
#if (SENSOR_HR_SEL == SENSOR_HR_VC30FX)                                     // 条件编译：如果选择VC30FX心率传感器  
        case EVT_VC30FX_ISR:                                                 // VC30FX中断服务事件  
            vc30fx_usr_device_handler(0, 1);                                // 调用VC30FX设备处理函数  
            break;                                                           // 跳出switch语句  
#endif  
        case EVT_HALT:                                                       // 系统停机事件  
            func_cb.sta = FUNC_BT_UPDATE;       //出现异常后要处理事情
            break;
    }
}

/**  
  * @brief  全局消息处理函数，处理系统中的各种消息事件  
  * @param  msg: 接收到的消息类型  
  * @retval None  
  */  
void func_message(size_msg_t msg)
{  
    switch (msg)                                                    // 根据消息类型执行相应处理  
    {  
        case MSG_CTP_SHORT_LEFT:                                   // 触摸屏短距离左滑消息  
            if (func_cb.sta == FUNC_CLOCK || func_cb.flag_sort)    // 如果当前在时钟界面或排序模式  
            {  
                func_switch_next(false, true);                      // 切换到下一个任务  
            }  
            else                                                    // 其他界面的左滑处理  
            {  
                if(func_cb.left_sta == FUNC_MESSAGE)               // 如果左侧状态是消息界面  
                {  
                    func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_LEFT); // 切换到时钟界面，使用左缩放动画  
                }  
            }  
            break;  
  
        case MSG_CTP_SHORT_RIGHT:                                   // 触摸屏短距离右滑消息  
            if (func_cb.left_sta == func_cb.sta)                    // 如果左侧状态等于当前状态  
            {  
                break;                                              // 不处理，直接跳出  
            }  
            if (func_cb.flag_sort)                                  // 如果处于排序模式  
            {  
                func_switch_prev(false);                            // 切换到上一个任务  
            }  
            else if(func_cb.menu_style == MENU_STYLE_FOOTBALL)      // 如果菜单样式是足球样式  
            {  
                func_back_to();                                     // 直接返回  
            }  
            else                                                    // 其他情况的右滑处理  
            {  
                func_backing_to();                                  // 右滑缓慢退出当前任务  
            }  
            break;  
  
        case MSG_CTP_COVER:                                        // 触摸屏覆盖消息（手掌覆盖屏幕）  
            sys_cb.sleep_delay = 1;                                // 设置100ms后进入休眠  
            break;  
  
        case MSG_QDEC_FORWARD:                                     // 旋转编码器正向旋转消息  
            if (func_cb.sta == FUNC_CLOCK || func_cb.flag_sort)    // 如果在时钟界面或排序模式  
            {  
                msg_queue_detach(MSG_QDEC_FORWARD, 0);             // 防止连续滚动，移除队列中的重复消息  
                func_switch_next(true, true);                       // 切换到下一个任务  
            }  
            break;  
  
        case MSG_QDEC_BACKWARD:                                    // 旋转编码器反向旋转消息  
            if (func_cb.sta == FUNC_CLOCK || func_cb.flag_sort)    // 如果在时钟界面或排序模式  
            {  
                msg_queue_detach(MSG_QDEC_BACKWARD, 0);            // 防止连续滚动，移除队列中的重复消息  
                func_switch_prev(true);                             // 切换到上一个任务  
            }  
            break;  
  
        case KU_BACK:                                              // 返回按键抬起消息  
            if (func_cb.flag_sort)                                 // 如果处于排序模式  
            {  
                func_switch_to_clock();                            // 切换回主时钟界面  
            }  
            else if (func_cb.sta == FUNC_CLOCK)                    // 如果当前在时钟界面  
            {  
                if(sys_cb.dialplate_btf_ready)                     // 如果表盘BTF准备就绪  
                {  
                    sys_cb.dialplate_btf_ready = false;            // 清除BTF准备标志  
                }  
                func_switch_to_menu();                             // 切换到主菜单  
            }  
            else                                                    // 其他界面的返回处理  
            {  
                func_back_to();                                     // 直接退出当前任务  
            }  
            break;  
  
#if SOFT_POWER_ON_OFF                                             // 如果启用软件电源开关功能  
        case KLH_BACK:                                             // 返回键长按消息  
        case KLH_LEFT:                                             // 左键长按消息  
        case KLH_RIGHT:                                            // 右键长按消息  
            func_cb.sta = FUNC_PWROFF;                             // 设置状态为关机  
            break;  
#endif  
  
        case KU_MODE:                                              // 模式按键抬起消息  
            func_cb.sta = FUNC_NULL;                               // 设置状态为空  
            break;  
  
        case KL_BACK:                                              // 返回键长按消息（堆栈后台）  
            if (bt_cb.disp_status < BT_STA_INCOMING && func_cb.sta != FUNC_MENUSTYLE) // 如果蓝牙状态小于来电且不在菜单样式界面  
            {  
                if (func_cb.sta == FUNC_CLOCK)                     // 如果当前在时钟界面  
                {  
                    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;  // 获取时钟功能控制块  
                    if (f_clk->sub_frm)                            // 如果存在子窗体  
                    {  
                        compo_form_destroy(f_clk->sub_frm);        // 销毁下拉界面的双窗体  
                    }  
                } 
                //func_switch_to(FUNC_SMARTSTACK, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
//                func_switch_to(FUNC_LONG_PRESS, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);                

                func_cb.sta = FUNC_LONG_PRESS;                     // 设置状态为长按功能  
            }  
            break;  
  


//        case KU_LEFT:
//            ble_bt_connect();               //ios一键双连测试
//            printf("send sm req\n");
//            break;

//        case KU_LEFT:
//            ble_ancs_start();               //ancs, ams测试
//            printf("ble_ancs_start\n");
//            break;

//#if EQ_MODE_EN
//        case KU_EQ:
//            sys_set_eq();
//            break;
//#endif // EQ_MODE_EN
//
//        case KU_MUTE:
//            if (sys_cb.mute) {
//                bsp_sys_unmute();
//            } else {
//                bsp_sys_mute();
//            }
//            break;
//
        case MSG_SYS_500MS:                                        // 系统500毫秒定时消息  
            break;                                                  // 暂时不处理 


        case MSG_SYS_BOX_500MS:                                    // 充电盒500毫秒定时消息  
#if CHARGE_EX_IC_SELECT                                           // 如果选择了外部充电IC  
            bsp_charge_ex_process();                               // 处理外部充电IC  
#endif  
            break;  
  
        case MSG_SYS_BOX_CHARGE:                                   // 充电盒充电消息  
            bsp_charge_ex_mode_set(BOX_CHARGE_MODE);               // 设置充电盒为充电模式  
            uteModulePlatformDlpsDisable(UTE_MODULE_PLATFORM_DLPS_BIT_EARPHONE); // 禁用耳机深度低功耗睡眠  
            break;  
  
        case MSG_SYS_BOX_INTER:                                    // 充电盒交互消息  
            bsp_charge_ex_mode_set(BOX_CHK_MODE);                  // 设置充电盒为检查模式  
            uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_EARPHONE);  // 启用耳机深度低功耗睡眠  
            break;  
  
        case MSG_SYS_1S:                                           // 系统1秒定时消息  
#if CALL_MGR_EN                                                   // 如果启用通话管理  
            bsp_bt_call_times_inc();                               // 增加蓝牙通话时间计数  
            bsp_modem_call_times_inc();                            // 增加调制解调器通话时间计数  
#endif  
#if BT_HFP_BAT_REPORT_EN                                          // 如果启用蓝牙HFP电池报告  
            bt_hfp_report_bat();                                   // 报告电池状态  
#endif  
#if ECIG_POWER_CONTROL                                            // 如果启用电子烟功率控制  
            if (bsp_ecig_is_working())                             // 如果电子烟正在工作  
            {  
                reset_sleep_delay_all();                           // 重置所有休眠延时（吸烟时不休眠）  
                reset_pwroff_delay();                              // 重置关机延时  
                printf("working_sta %d\\n",bsp_ecig_is_working_sta()); // 打印工作状态  
            }  
#endif  
            break;  
  
#if ECIG_POWER_CONTROL                                            // 如果启用电子烟功率控制  
        case EVT_ECIG_SMOKE_REMINDER:                              // 电子烟吸烟提醒事件  
            if(func_cb.sta !=FUNC_AGEING)                          // 如果不在老化测试状态  
            {  
                if(func_cb.sta != FUNC_ECIG_REMINDER )             // 如果不在电子烟提醒界面  
                {  
                    printf(" func_cb.sta != FUNC_ECIG_REMINDER \\n"); // 打印调试信息  
                    uteTaskGuiStartScreen(FUNC_ECIG_REMINDER, 0, __func__); // 启动电子烟提醒界面  
                }  
                else                                                // 如果已经在电子烟提醒界面  
                {  
                    printf(" func_cb.sta == FUNC_ECIG_REMINDER \\n"); // 打印调试信息  
                    compo_form_destroy(func_cb.frm_main);           // 销毁当前主窗体  
                    func_cb.frm_main = func_ecig_reminder_form_create(); // 重新创建电子烟提醒窗体  
                    uteTaskGuiStartScreen(FUNC_ECIG_REMINDER, 0, __func__); // 重新启动界面  
                }  
            }  
            break;  
#endif  
  
        case EVT_WATCH_SET_COVER:                                  // 手表设置封面事件  
            gui_set_cover_index(sys_cb.cover_index);               // 设置GUI封面索引  
            break;  
  
        case EVT_WATCH_MSG_POP_UP:                                 // 手表消息弹出事件  
            app_ute_msg_pop_up(sys_cb.msg_index);                  // 弹出UTE消息  
            break;  
  
        case MSG_CHECK_LANGUAGE:                                   // APP语言切换消息  
            sys_cb.sta_old = func_cb.sta;                          // 保存当前状态  
            sys_cb.refresh_language_flag = true;                   // 设置语言刷新标志  
            func_cb.sta = FUNC_NULL;                               // 设置状态为空，触发界面重建  
            break;  
  
        case EVT_WATCH_TIMER_DONE:                                 // 计时器完成事件  
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1); // 启动震动马达  
            if(func_cb.sta != FUNC_TIMER)                          // 如果不在计时器界面  
            {  
                sys_cb.cover_index = REMIND_COVER_TIMER_FINISH;     // 设置提醒封面为计时器完成  
                sys_cb.remind_tag = true;                           // 设置提醒标志  
            }  
            sys_cb.timer_left_sec = sys_cb.timer_custom_sec;       // 重置计时器剩余时间  
            break;  
  
        default:                                                    // 默认情况，处理其他消息  
            evt_message(msg);                                       // 调用事件消息处理函数  
            break;  
    }  
  
#if (ASR_SELECT == ASR_YJ && !ASR_DEAL_TYPE)                      // 如果选择了YJ语音识别且不是处理类型  
    third_func_message(msg);                                       // 调用第三方功能消息处理  
#endif  
  
    // 音量调节处理：3秒后写入flash  
    if ((sys_cb.cm_vol_change) && (sys_cb.cm_times >= 6))         // 如果音量发生变化且计数达到6次（3秒）  
    {  
        sys_cb.cm_vol_change = 0;                                  // 清除音量变化标志  
        cm_sync();                                                  // 同步音量设置到flash  
    }  
}
///进入一个功能的总入口
AT(.text.func)
void func_enter(void)
{
    //检查Func Heap
    u32 heap_size = func_heap_get_free_size();
    if (heap_size != HEAP_FUNC_SIZE)
    {
        printf("Func heap leak (%d -> %d): %d\n", func_cb.last, func_cb.sta, heap_size);
        halt(HALT_FUNC_HEAP);
    }

//    gui_box_clear();
    param_sync();
    reset_sleep_delay_all();
    reset_pwroff_delay();
    func_cb.mp3_res_play = NULL;
    func_cb.set_vol_callback = NULL;
    if (func_cb.sta == FUNC_NULL)
    {
        func_cb.sta = FUNC_CLOCK;
    }
//    bsp_clr_mute_sta();
//    sys_cb.voice_evt_brk_en = 1;    //播放提示音时，快速响应事件。
    AMPLIFIER_SEL_D();
}

AT(.text.func)
void func_exit(void)
{
#if ASR_SELECT
    bsp_asr_voice_wake_sta_clr();
#endif
    //销毁窗体
    if (func_cb.frm_main != NULL)
    {
        compo_form_destroy(func_cb.frm_main);
    }
    //释放FUNC控制结构体
    if (func_cb.f_cb != NULL)
    {
        func_free(func_cb.f_cb);
    }
    func_cb.frm_main = NULL;
    func_cb.f_cb = NULL;
}

AT(.text.func)
void func_run(void)  
{  
    void (*func_entry)(void) = NULL;                    // 函数指针，用于存储当前要执行的功能入口函数  
    printf("%s\\n", __func__);  
    memset(func_cb.tbl_sort, 0, sizeof(func_cb.tbl_sort));  // 清空功能排序表  
      
    // 默认跳转空白界面，具体开机跳转界面在uteApplicationCommonStartupSecond中设置  
    func_cb.sta = FUNC_EMPTY;  
      
    // 获取自定义的屏幕排序配置  
    uteModuleGuiCommonGetScreenTblSort(func_cb.tbl_sort, &func_cb.sort_cnt);  
  
    task_stack_init();                                  // 初始化任务堆栈管理  
    latest_task_init();                                 // 初始化最近任务列表  
    sys_cb.sys_init_complete = true;                    // 标记系统初始化完成  
      
    for (;;)                                           // 主循环 - 永不退出  
    {  
        // 处理语言切换请求  
        if(sys_cb.refresh_language_flag)  
        {  
            func_cb.sta = sys_cb.sta_old;              // 恢复到切换前的状态  
            sys_cb.refresh_language_flag = false;  
        }  
  
        func_enter();                                   // 功能进入前的准备工作  
          
        // 遍历功能入口表，查找当前状态对应的功能函数  
        for (int i = 0; i < FUNC_ENTRY_CNT; i++)  
        {  
            if (tbl_func_entry[i].func_idx == func_cb.sta)  
            {  
                task_stack_push(func_cb.sta);          // 将当前任务压入堆栈  
                latest_task_add(func_cb.sta);          // 添加到最近任务列表  
                func_entry = tbl_func_entry[i].func;   // 获取功能入口函数  
                func_entry();                          // 执行功能函数  
                break;  
            }  
        }  
          
        // 处理特殊状态  
        if (func_cb.sta == FUNC_PWROFF)  
        {  
            func_pwroff(1);                            // 执行关机流程  
        }  
        if (func_cb.sta == FUNC_DEEPSLEEP)  
        {  
            func_deepsleep();                          // 执行深度睡眠  
        }  
          
        func_exit();                                   // 功能退出后的清理工作  
    }  
}
