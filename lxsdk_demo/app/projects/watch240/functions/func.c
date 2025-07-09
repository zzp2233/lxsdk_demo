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
    int func_idx;
    void *func;
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

AT(.text.func.process)
void func_watch_bt_process(void)
{
    uint disp_status = bsp_bt_disp_status();

    if (disp_status == BT_STA_OTA)
    {
        sfunc_bt_ota();
    }
#if !CALL_MGR_EN
    else if (sys_cb.reject_tick)
    {
        if (tick_check_expire(sys_cb.reject_tick, 3000) || disp_status < BT_STA_INCOMING)
        {
            sys_cb.reject_tick = 0;
        }
#if BT_VOIP_REJECT_EN
    }
    else if (bt_cb.disp_status == BT_STA_INCOMING && bt_cb.call_type == CALL_TYPE_PHONE)
    {
#else
    }
    else if (bt_cb.disp_status == BT_STA_INCOMING)
    {
#endif
        if (func_cb.sta != FUNC_BT_RING)
        {
            func_cb.sta = FUNC_BT_RING;
            msg_enqueue(EVT_MSGBOX_EXIT);
        }

#if BT_VOIP_REJECT_EN
    }
    else if (bt_cb.disp_status >= BT_STA_OUTGOING && bt_cb.call_type == CALL_TYPE_PHONE)
    {
#else
    }
    else if (bt_cb.disp_status >= BT_STA_OUTGOING)
    {
#endif
        if (func_cb.sta != FUNC_BT_CALL)
        {
            func_cb.sta = FUNC_BT_CALL;
            msg_enqueue(EVT_MSGBOX_EXIT);
        }
    }
#endif
}
void print_info(void)
{
    static u32 ticks = 0;
    if (tick_check_expire(ticks, 1000))
    {
        ticks = tick_get();
        extern void mem_monitor_run(void);
        mem_monitor_run();
        printf("sys_cb.sco_state[%d], bt_cb.call_type[%d], bt_cb.disp_status[%d]\n", sys_cb.sco_state, bt_cb.call_type, bt_cb.disp_status);
        extern void thread_info_printf(void);
        thread_info_printf();
    }
}

AT(.text.func.process)
void func_process(void)
{
    WDT_CLR();
    // print_info();

#if (FUNC_MUSIC_EN || FUNC_RECORDER_EN) && SD_SOFT_DETECT_EN
    sd_soft_cmd_detect(120);
#endif

    tft_bglight_frist_set_check();
    // gui 没有休眠才更新 防止客户乱改我们的唤醒流程导致卡主的问题
#if FOTA_UI_EN
    if ((!sys_cb.gui_sleep_sta) /*&& (func_cb.sta != FUNC_OTA_UI_MODE)*/ && !sys_cb.flag_halt)
    {
#else
    if (!sys_cb.gui_sleep_sta && !sys_cb.flag_halt)
    {
#endif
        compo_update();                                     //更新组件
        gui_process();                                      //刷新UI
    }

#if OPUS_ENC_EN
    if (bsp_opus_is_encode())
    {
        reset_sleep_delay_all();
        u8 temp_buf[80];
        memset(temp_buf, 0, sizeof(temp_buf));
        if (bsp_opus_get_enc_frame(temp_buf, sizeof(temp_buf)))
        {
            print_r(temp_buf, sizeof(temp_buf));
        }
    }
#endif//OPUS_ENC_EN

    co_timer_pro(false);
    bsp_sensor_step_pro_isr();

    if (sys_cb.mp3_res_playing)
    {
        mp3_res_process();                                 //提示音后台处理
    }

    if (sleep_process(bt_is_allow_sleep))
    {
        bt_cb.disp_status = 0xff;
    }

    if (sys_cb.remind_tag)   //闹钟响起
    {
        sys_cb.remind_tag = false;
        gui_cover_msg_enqueue(sys_cb.cover_index);
        printf(">>>REMIND POP\n");
    }

    if (sys_cb.msg_tag)   //有消息弹出
    {
        sys_cb.msg_tag = false;
        app_msg_pop_up(sys_cb.msg_index);
        printf(">>>MSG POP\n");
    }

    if (sys_cb.timer_done)
    {
        sys_cb.timer_done = false;
        msg_enqueue(EVT_MSGBOX_EXIT);
        msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
        msg_enqueue(EVT_WATCH_TIMER_DONE);
        printf(">>>TIMER DONE\n");
    }

#if VBAT_DETECT_EN
    bsp_vbat_lpwr_process();
#endif

#if BT_BACKSTAGE_EN
    func_watch_bt_process();
#endif

#if CALL_MGR_EN
    bsp_call_mgr_process();
#endif

    //PWRKEY模拟硬开关关机处理
    if ((PWRKEY_2_HW_PWRON) && (sys_cb.pwrdwn_hw_flag))
    {
        func_cb.sta = FUNC_PWROFF;
        sys_cb.pwrdwn_hw_flag = 0;
    }

#if CHARGE_EN
    if (xcfg_cb.charge_en)
    {
        charge_detect(1);
        if (bsp_charge_sta_get())
        {
            if (bt_cb.disp_status <= BT_STA_PLAYING && func_cb.sta != FUNC_OTA_UI_MODE && is_fot_start() == 0)
            {
                if (charge_dc_change_detect())
                {
                    if (sys_cb.gui_sleep_sta)           //亮屏
                    {
                        sys_cb.gui_need_wakeup = 1;
                    }
                    reset_sleep_delay_all();
                }
                if (func_cb.sta != FUNC_CHARGE)
                {
                    msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
                    msg_enqueue(EVT_MSGBOX_EXIT);
                }
                if(func_cb.sta != FUNC_AGEING &&func_cb.sta != FUNC_FACTORY_TESTING )
                    func_cb.sta = FUNC_CHARGE;
            }
        }
    }
#endif // CHARGE_EN

    if(bt_cb.bt_is_inited)
    {
        bt_thread_check_trigger();
#if LE_EN
        ble_app_process();
#endif
#if LE_AB_FOT_EN
        bsp_fot_process();
        if (is_fot_start() == true)
        {
            reset_sleep_delay_all()
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
        }
#endif
    }
#if ASR_SELECT
    if(bsp_asr_init_sta())
    {
        delay_5ms(2);
    }
#endif
#if (ASR_SELECT == ASR_YJ && ASR_DEAL_TYPE)
    third_func_process();
#endif
#if (ASR_SELECT == ASR_WS && MONITOR_ASR_WAKEUP_EN)
    asr_event_process();
#endif
#if (ASR_SELECT == ASR_WS_AIR && MONITOR_ASR_WAKEUP_EN)
    asr_event_process();
#endif
}

//根据任务名创建窗体。此处调用的创建窗体函数不要调用子任务的控制结构体
compo_form_t *func_create_form(u8 sta)
{
    // printf("%s->sta:%d\n", __func__, sta);
    compo_form_t *frm = NULL;
    compo_form_t *(*func_create)(void) = NULL;
    for (int i = 0; i < FUNC_CREATE_CNT; i++)
    {
        if (tbl_func_create[i].func_idx == sta)
        {
            func_create = tbl_func_create[i].func;
            if (func_create != NULL)
            {
                frm = func_create();
            }
            break;
        }
    }
    if (frm == NULL)
    {
        // printf("halt %s->sta:%d\n", __func__, sta);
        halt(HALT_FUNC_SORT);
    }
    return frm;

}

//获取当前任务顺序
static int func_get_order(u8 sta)
{
    int i;
    for (i=0; i<func_cb.sort_cnt; i++)
    {
        if (sta == func_cb.tbl_sort[i])
        {
            return i;
        }
    }
    return -1;
}

//切换到上一个任务
void func_switch_prev(bool flag_auto)
{
    u16 switch_mode = flag_auto ? (FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO) : FUNC_SWITCH_LR_ZOOM_RIGHT;
    s8 idx = func_get_order(func_cb.sta);
    if (idx <= 0)
    {
        return;
    }
    u8 sta = func_cb.tbl_sort[idx - 1];
    compo_form_t *frm = func_create_form(sta);                                  //创建下一个任务的窗体
    bool res = func_switching(switch_mode, NULL);                               //切换动画
    compo_form_destroy(frm);                                                    //切换完成或取消，销毁窗体
    if (res)
    {
        if (sta == FUNC_CLOCK)
        {
            func_cb.flag_sort = false;
        }
        func_cb.sta = sta;
    }
}

//切换到下一个任务
void func_switch_next(bool flag_auto, bool flag_loop)
{
    u16 switch_mode = flag_auto ? (FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO) : FUNC_SWITCH_LR_ZOOM_LEFT;
    u8 idx = func_get_order(func_cb.sta);
    if (idx < 0)
    {
        return ;
    }
    u8 sta;
    if (idx >= func_cb.sort_cnt - 1)
    {
        if (flag_loop)
        {
            sta = func_cb.tbl_sort[0];
        }
        else
        {
            return ;
        }
    }
    else
    {
        sta = func_cb.tbl_sort[idx + 1];
    }

    compo_form_t *frm = func_create_form(sta);                                  //创建下一个任务的窗体
    bool res = func_switching(switch_mode, NULL);                               //切换动画
    compo_form_destroy(frm);                                                    //切换完成或取消，销毁窗体
    if (res)
    {
        if (sta == FUNC_CLOCK)
        {
            func_cb.flag_sort = false;
        }
        else
        {
            func_cb.flag_sort = true;
        }
        func_cb.sta = sta;
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

//切换回主时钟
void func_switch_to_clock(void)
{
    func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
    func_cb.flag_sort = false;
}


//退回到主菜单
void func_switch_to_menu(void)
{
    u16 switch_mode;
    bool flag_frm_menu;                                                         //是否需要创建菜单窗体
    flag_frm_menu = true;
    if (func_cb.menu_style == MENU_STYLE_FOOTBALL)
    {
        switch_mode = FUNC_SWITCH_FADE_OUT | FUNC_SWITCH_AUTO;;
        flag_frm_menu = false;
    }
    else if (func_cb.sta != FUNC_CLOCK || func_cb.menu_style == MENU_STYLE_HONEYCOMB)
    {
        switch_mode = FUNC_SWITCH_ZOOM_EXIT | FUNC_SWITCH_AUTO;
    }
    else if (func_cb.menu_style == MENU_STYLE_WATERFALL)
    {
        switch_mode = FUNC_SWITCH_FADE_OUT | FUNC_SWITCH_AUTO;
        func_cb.flag_animation = true;                                          //淡出后进入入场动画
        flag_frm_menu = false;
    }
    else
    {
        switch_mode = FUNC_SWITCH_ZOOM_FADE_EXIT | FUNC_SWITCH_AUTO;
    }
    if (flag_frm_menu)
    {
        widget_icon_t *icon;
        compo_form_t *frm = func_create_form(FUNC_MENU);                        //创建下一个任务的窗体
#if (ASR_SELECT && ASR_VOICE_BALL_ANIM)
        component_t *compo = compo_get_next((component_t *)frm->anim);
#else
        component_t *compo = compo_get_next((component_t *)frm->title);
#endif

        if (compo->type == COMPO_TYPE_ICONLIST)
        {
            compo_iconlist_t *iconlist = (compo_iconlist_t *)compo;
            icon = compo_iconlist_select_byidx(iconlist, func_cb.menu_idx);
        }
        else if (compo->type == COMPO_TYPE_LISTBOX)
        {
            compo_listbox_t *listbox = (compo_listbox_t *)compo;
            icon = compo_listbox_select_byidx(listbox, func_cb.menu_idx);
        }
        else if (compo->type == COMPO_TYPE_DISKLIST)
        {
            compo_disklist_t *disklist = (compo_disklist_t *)compo;
            icon = compo_disklist_select_byidx(disklist, func_cb.menu_idx);
        }
        else if (compo->type == COMPO_TYPE_KALEIDOSCOPE)
        {
            compo_kaleidoscope_t *kale = (compo_kaleidoscope_t *)compo;
            icon = compo_kale_select_byidx(kale, func_cb.menu_idx);
        }
        else if (compo->type == COMPO_TYPE_RINGS)
        {
            compo_rings_t *rings = (compo_rings_t *)compo;
            icon = compo_rings_select_byidx(rings, func_cb.menu_idx);
            if (icon == NULL)
            {
                extern u8 func_menu_sub_skyrer_get_first_idx(void);
                func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();
                icon = compo_rings_select_byidx(rings, func_cb.menu_idx);
            }
        }
        else
        {
            halt(HALT_GUI_COMPO_ICONLIST_TYPE);
            return;
        }
        func_switching(switch_mode, icon);                                      //退出动画
        compo_form_destroy(frm);                                                //切换完成或取消，销毁窗体
    }
    else
    {
        func_switching(switch_mode, NULL);                                      //退出动画
    }
    func_cb.sta = FUNC_MENU;
}

//手动退回到主菜单
void func_switching_to_menu(void)
{
    widget_icon_t *icon;
    u16 switch_mode;
    compo_form_t *frm = func_create_form(FUNC_MENU);                            //创建下一个任务的窗体
#if (ASR_SELECT && ASR_VOICE_BALL_ANIM)
    component_t *compo = compo_get_next((component_t *)frm->anim);
#else
    component_t *compo = compo_get_next((component_t *)frm->title);
#endif
    if (compo->type == COMPO_TYPE_ICONLIST)
    {
        compo_iconlist_t *iconlist = (compo_iconlist_t *)compo;
        icon = compo_iconlist_select_byidx(iconlist, func_cb.menu_idx);
    }
    else if (compo->type == COMPO_TYPE_LISTBOX)
    {
        compo_listbox_t *listbox = (compo_listbox_t *)compo;
        icon = compo_listbox_select_byidx(listbox, func_cb.menu_idx);
    }
    else if (compo->type == COMPO_TYPE_DISKLIST)
    {
        compo_disklist_t *disklist = (compo_disklist_t *)compo;
        icon = compo_disklist_select_byidx(disklist, func_cb.menu_idx);
    }
    else if (compo->type == COMPO_TYPE_KALEIDOSCOPE)
    {
        compo_kaleidoscope_t *kale = (compo_kaleidoscope_t *)compo;
        icon = compo_kale_select_byidx(kale, func_cb.menu_idx);
    }
    else if (compo->type == COMPO_TYPE_RINGS)
    {
        compo_rings_t *rings = (compo_rings_t *)compo;
        icon = compo_rings_select_byidx(rings, func_cb.menu_idx);
        if (icon == NULL)
        {
            extern u8 func_menu_sub_skyrer_get_first_idx(void);
            func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();
            icon = compo_rings_select_byidx(rings, func_cb.menu_idx);
        }
    }
    else
    {
        halt(HALT_GUI_COMPO_ICONLIST_TYPE);
        return;
    }
    if (func_cb.sta != FUNC_CLOCK || func_cb.menu_style == MENU_STYLE_HONEYCOMB)
    {
        switch_mode = FUNC_SWITCH_ZOOM_EXIT;
    }
    else
    {
        switch_mode = FUNC_SWITCH_ZOOM_FADE_EXIT;
    }
    bool res = func_switching(switch_mode, icon);                               //退出动画
    compo_form_destroy(frm);                                                    //切换完成或取消，销毁窗体
    if (res)
    {
        func_cb.sta = FUNC_MENU;
    }
}


//页面滑动回退功能
void func_backing_to(void)
{
    if (func_cb.sta == FUNC_BT_RING || func_cb.sta == FUNC_BT_CALL)
    {

        return;
    }

    u8 stack_top = task_stack_pop();

    if (!stack_top)
    {
        stack_top = FUNC_CLOCK;                                 //异常返回表盘
    }

    if (stack_top == FUNC_MENU || FUNC_MENUSTYLE == stack_top)
    {
        func_switching_to_menu();                               //右滑缓慢退出任务
    }
    else
    {
        func_switch_to(stack_top, FUNC_SWITCH_LR_ZOOM_RIGHT);   //返回上一个界面
    }
    if (func_cb.sta != stack_top)                               //如果页面没切换需要重新入栈
    {
        task_stack_push(func_cb.sta);
    }
}

//页面按键回退功能
void func_back_to(void)
{
    if (func_cb.sta == FUNC_BT_RING || func_cb.sta == FUNC_BT_CALL)
    {
        return;
    }

    u8 stack_top = task_stack_pop();

    if (!stack_top)
    {
        stack_top = FUNC_CLOCK;                                 //异常返回表盘
    }

    if (stack_top == FUNC_MENU
#if FUNC_MUSIC_EN
        || func_cb.sta == FUNC_MUSIC
#endif
       )
    {
        func_switch_to_menu();                                  //返回主菜单
    }
    else
    {
        func_switch_to(stack_top, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);    //返回上一个界面
    }
}

//页面直接回退,无动画效果
u8 func_directly_back_to(void)
{
    u8 stack_top = task_stack_pop();

    func_cb.sta = stack_top;

    return stack_top;
}

void evt_message(size_msg_t msg)
{
    switch (msg)
    {
        case EVT_READY_EXCHANGE_MTU:
            ble_exchange_mtu_request();
            break;

#if BT_CONNECT_REQ_FROM_WATCH_EN
        case EVT_BT_CONNECT_ONCE:
            bt_connect();
            break;
#endif

#if MUSIC_UDISK_EN
        case EVT_UDISK_INSERT:
            if (dev_is_online(DEV_UDISK))
            {
                if (dev_udisk_activation_try(0))
                {
                    sys_cb.cur_dev = DEV_UDISK;
                    func_cb.sta = FUNC_MUSIC;
                }
            }
            break;
#endif // MUSIC_UDISK_EN

#if MUSIC_SDCARD_EN
        case EVT_SD_INSERT:
            if (dev_is_online(DEV_SDCARD))
            {
                sys_cb.cur_dev = DEV_SDCARD;
                func_cb.sta = FUNC_MUSIC;
            }
            break;
#endif // MUSIC_SDCARD_EN

#if FUNC_USBDEV_EN
        case EVT_PC_INSERT:
            if (dev_is_online(DEV_USBPC))
            {
                func_cb.sta = FUNC_USBDEV;
            }
            break;
#endif // FUNC_USBDEV_EN

        case EVT_HFP_SET_VOL:
            if(sys_cb.incall_flag & INCALL_FLAG_SCO)
            {
                bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
                dac_fade_in();
                printf("HFP SET VOL: %d\n", sys_cb.hfp_vol);
            }
            break;

        case EVT_A2DP_SET_VOL:
            if ((sys_cb.incall_flag & INCALL_FLAG_SCO) == 0)
            {
                printf("A2DP SET VOL: %d\n", sys_cb.vol);
                bsp_change_volume(sys_cb.vol);
                param_sys_vol_write();
                sys_cb.cm_times = 0;
                sys_cb.cm_vol_change = 1;
                if (bt_cb.music_playing)
                {
                    dac_fade_in();
                }
            }
            break;

        case EVT_A2DP_MUSIC_PLAY:
            if (!sbc_is_bypass())
            {
                printf("EVT_A2DP_MUSIC_PLAY\n");
                dac_fade_in();      //淡入前会先解MUTE
            }
            bt_cb.music_playing = true;
            break;

        case EVT_A2DP_MUSIC_STOP:
            if (!sbc_is_bypass())
            {
                printf("EVT_A2DP_MUSIC_STOP\n");
                dac_fade_out();     //淡出完成会自动MUTE功放
                // dac_fade_wait();
                // bsp_sys_mute();
            }
            bt_cb.music_playing = false;
            break;

        case EVT_BT_SCAN_START:
            if (bt_get_status() < BT_STA_SCANNING)
            {
                bt_scan_enable();
            }
            break;
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
        case EVT_ONLINE_SET_EQ:
            eq_parse_cmd();
            break;
#endif

#if EQ_MODE_EN
        case EVT_BT_SET_EQ:
            music_set_eq_by_num(sys_cb.eq_mode);
            break;
#endif

#if BT_SCO_APP_DBG_EN
        case EVT_SCO_DBG_COMM_RSP:
        case EVT_SCO_DBG_TLV_RSP:
        case EVT_SCO_DBG_NOTIFY:
            sco_app_msg_deal(msg);
            break;
#endif

#if (SENSOR_HR_SEL == SENSOR_HR_VC30FX)
        case EVT_VC30FX_ISR:
            vc30fx_usr_device_handler(0, 1);
            break;
#endif
        case EVT_HALT:
            func_cb.sta = FUNC_BT_UPDATE;       //出现异常后要处理事情
            break;
    }
}

//func common message process
void func_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_SHORT_LEFT:
            if (func_cb.sta == FUNC_CLOCK || func_cb.flag_sort)
            {
                func_switch_next(false, true);                    //切到下一个任务
            }
            else
            {
                //     if (func_cb.last == FUNC_CLOCK && func_cb.left_sta == func_cb.sta && func_cb.left_sta != 0)
                if(func_cb.left_sta == FUNC_MESSAGE)
                {
                    func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_LEFT);
                }
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            if (func_cb.left_sta == func_cb.sta)
            {
                break;
            }
            if (func_cb.flag_sort)
            {
                func_switch_prev(false);                    //切到上一个任务
            }
            else if(func_cb.menu_style == MENU_STYLE_FOOTBALL)
            {
                func_back_to();
            }
            else /*if (ctp_get_sxy().x <= 32)*/
            {
                func_backing_to();                          //右滑缓慢退出任务
            }
            break;

        case MSG_CTP_COVER:
            sys_cb.sleep_delay = 1; //100ms后进入休眠
            break;

        case MSG_QDEC_FORWARD:
            if (func_cb.sta == FUNC_CLOCK || func_cb.flag_sort)
            {
                msg_queue_detach(MSG_QDEC_FORWARD, 0);  //防止不停滚动
                func_switch_next(true, true);                     //切到下一个任务
            }
            break;

        case MSG_QDEC_BACKWARD:
            if (func_cb.sta == FUNC_CLOCK || func_cb.flag_sort)
            {
                msg_queue_detach(MSG_QDEC_BACKWARD, 0);
                func_switch_prev(true);                     //切到下一个任务
            }
            break;

        case KU_BACK:
            if (func_cb.flag_sort)
            {
                func_switch_to_clock();                     //切换回主时钟
            }
            else if (func_cb.sta == FUNC_CLOCK)
            {
                if(sys_cb.dialplate_btf_ready)
                {
                    sys_cb.dialplate_btf_ready = false;
                }
                func_switch_to_menu();                      //退回到主菜单
            }
            else
            {
                func_back_to();                             //直接退出任务
            }
            break;

#if SOFT_POWER_ON_OFF
        case KLH_BACK:
        case KLH_LEFT:
        case KLH_RIGHT:
            func_cb.sta = FUNC_PWROFF;
            break;
#endif

        case KU_MODE:
            func_cb.sta = FUNC_NULL;
            break;

        case KL_BACK:   //堆栈后台
            if (bt_cb.disp_status < BT_STA_INCOMING && func_cb.sta != FUNC_MENUSTYLE)
            {
                if (func_cb.sta == FUNC_CLOCK)  //|| func_cb.flag_sort
                {
                    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
                    if (f_clk->sub_frm)
                    {
                        compo_form_destroy(f_clk->sub_frm);     //下拉界面存在双窗体
                    }
                }
                //func_switch_to(FUNC_SMARTSTACK, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
//                func_switch_to(FUNC_LONG_PRESS, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
                func_cb.sta = FUNC_LONG_PRESS;
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
        case MSG_SYS_500MS:

            break;
        case MSG_SYS_BOX_500MS:
#if CHARGE_EX_IC_SELECT
            // printf("MSG_SYS_BOX_500MS \n");
            bsp_charge_ex_process();
#endif // CHARGE_EX_IC_SELECT
            break;
        case MSG_SYS_BOX_CHARGE:
            //printf("MSG_SYS_BOX_CHARGE \n");
            bsp_charge_ex_mode_set(BOX_CHARGE_MODE);
            // sleep_cb.sys_is_sleep = false;
            uteModulePlatformDlpsDisable(UTE_MODULE_PLATFORM_DLPS_BIT_EARPHONE);
            break;
        case MSG_SYS_BOX_INTER:
            //printf("MSG_SYS_BOX_INTER \n");
            bsp_charge_ex_mode_set(BOX_CHK_MODE);
            // sleep_cb.sys_is_sleep = true;
            uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_EARPHONE);
            break;

        case MSG_SYS_1S:
#if CALL_MGR_EN
            bsp_bt_call_times_inc();
            bsp_modem_call_times_inc();
#endif
#if BT_HFP_BAT_REPORT_EN
            bt_hfp_report_bat();
#endif
#if ECIG_POWER_CONTROL
            if (bsp_ecig_is_working())
            {
                reset_sleep_delay_all();    //吸烟不休眠
                reset_pwroff_delay();
                printf("working_sta %d\n",bsp_ecig_is_working_sta());
            }
#endif
            break;
#if ECIG_POWER_CONTROL
        case EVT_ECIG_SMOKE_REMINDER:
            if(func_cb.sta !=FUNC_AGEING)
            {
                if(func_cb.sta != FUNC_ECIG_REMINDER )
                {
                    printf(" func_cb.sta != FUNC_ECIG_REMINDER \n");
                    // func_cb.sta = FUNC_ECIG_REMINDER;
                    uteTaskGuiStartScreen(FUNC_ECIG_REMINDER, 0, __func__);

                }
                else
                {
                    printf(" func_cb.sta == FUNC_ECIG_REMINDER \n");
                    compo_form_destroy(func_cb.frm_main);
                    func_cb.frm_main = func_ecig_reminder_form_create();
                    uteTaskGuiStartScreen(FUNC_ECIG_REMINDER, 0, __func__);

                }
            }

            break;
#endif

        case EVT_WATCH_SET_COVER:
            gui_set_cover_index(sys_cb.cover_index);
            break;

        case EVT_WATCH_MSG_POP_UP:
            app_ute_msg_pop_up(sys_cb.msg_index);
            break;

        case MSG_CHECK_LANGUAGE://APP语言切换
//            compo_form_destroy(func_cb.frm_main);
//            func_cb.frm_main = func_create_form(func_cb.sta);
            sys_cb.sta_old = func_cb.sta;
            sys_cb.refresh_language_flag = true;
//            func_switch_to(FUNC_CLOCK, 0);
            func_cb.sta = FUNC_NULL;
            break;

        case EVT_WATCH_TIMER_DONE:      //计时器响铃
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            if(func_cb.sta != FUNC_TIMER)
            {
                sys_cb.cover_index = REMIND_COVER_TIMER_FINISH;
                sys_cb.remind_tag = true;
            }
            sys_cb.timer_left_sec = sys_cb.timer_custom_sec;
//            msgbox("计时已经结束",NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_RES_NONE);
            break;

        default:
            evt_message(msg);
            break;
    }
#if (ASR_SELECT == ASR_YJ && !ASR_DEAL_TYPE)
    third_func_message(msg);
#endif
    //调节音量，3秒后写入flash
    if ((sys_cb.cm_vol_change) && (sys_cb.cm_times >= 6))
    {
        sys_cb.cm_vol_change = 0;
        cm_sync();
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
    void (*func_entry)(void) = NULL;
    printf("%s\n", __func__);
    memset(func_cb.tbl_sort, 0, sizeof(func_cb.tbl_sort));
    //默认跳转空白界面，具体开机跳转界面在uteApplicationCommonStartupSecond
    func_cb.sta = FUNC_EMPTY;//FUNC_OTA_SUCC;//FUNC_OTA_MODE;//FUNC_CLOCK;//FUNC_OTA_UI_MODE;//FUNC_OTA_MODE;//;//
    // 获取自定义排序
    uteModuleGuiCommonGetScreenTblSort(func_cb.tbl_sort, &func_cb.sort_cnt);

    task_stack_init();  //任务堆栈
    latest_task_init(); //最近任务
    sys_cb.sys_init_complete = true;
    for (;;)
    {
        if(sys_cb.refresh_language_flag) //刷新语言
        {
            // printf("sta_old = %d\n", sys_cb.sta_old);
//            func_switch_to(sys_cb.sta_old, 0);
            func_cb.sta = sys_cb.sta_old;
            sys_cb.refresh_language_flag = false;
        }

        func_enter();
        for (int i = 0; i < FUNC_ENTRY_CNT; i++)
        {
            if (tbl_func_entry[i].func_idx == func_cb.sta)
            {
                task_stack_push(func_cb.sta);
                latest_task_add(func_cb.sta);
                func_entry = tbl_func_entry[i].func;
                func_entry();
                break;
            }
        }
        if (func_cb.sta == FUNC_PWROFF)
        {
            func_pwroff(1);
        }
        if (func_cb.sta == FUNC_DEEPSLEEP)
        {
            func_deepsleep();
        }
        func_exit();
    }
}
