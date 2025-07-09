#include "include.h"
#include "app.h"
#include "ute_module_notify.h"

void ble_app_init(void)
{
#if LE_AB_LINK_APP_EN
    ble_app_ab_link_init();
#endif

#if USE_APP_TYPE
    ble_app_watch_init();
#endif

#if LE_ANCS_CLIENT_EN
    ble_ancs_var_init();
#endif
}

void ble_app_process(void)
{
#if LE_AB_LINK_APP_EN
    ble_app_ab_link_process();
#endif

#if USE_APP_TYPE
    ble_app_watch_process();
#endif
}

bool ble_app_need_wakeup(void)
{
    bool ret = false;

#if LE_AB_LINK_APP_EN
    ret |= ble_app_ab_link_need_wakeup();
#endif

#if USE_APP_TYPE
    ret |= ble_app_watch_need_wakeup();
#endif

    return ret;
}

void ble_app_disconnect_callback(void)
{
#if USE_APP_TYPE
    ble_app_watch_disconnect_callback();
#endif
}

void app_phone_type_set(u8 type)
{

}

u8 app_phone_type_get(void)
{
    return 1;
}

//一键双连应用示例
void app_once_connect(u8 ble_app_ios)
{
#if LE_SM_SC_EN
    if (app_phone_type_get() != ble_app_ios)
    {
        app_phone_type_set(ble_app_ios);
        bsp_change_bt_mac();
    }

    if (ble_app_ios && !bt_is_connected())          //IOS
    {
        if (bt_nor_get_link_info(NULL))             //如果存在配对信息
        {
            sys_cb.bt_reconn_flag = true;
            bt_connect();
        }
        else
        {
            ble_bt_connect();                       //一键双联
        }
    }
#endif // LE_SM_SC_EN
}

void ble_app_connect_callback(void)
{
#if USE_APP_TYPE
    ble_app_watch_connect_callback();
#endif

//    //一键双链接IOS示例程序,需要打开宏LE_PAIR_EN, LE_SM_SC_EN
//    app_once_connect(true);
//    //一键双链接安卓示例程序,需要打开宏LE_PAIR_EN, LE_SM_SC_EN
//    app_once_connect(false);

}

void ble_app_client_cfg_callback(u16 handle, u8 cfg)
{
#if USE_APP_TYPE
    ble_app_watch_client_cfg_callback(handle, cfg);
#endif
}

void ble_app_ancs_client_notifiy_callback(u8 id, const char *att_name, const char *att_content)
{
#if LE_ANCS_CLIENT_EN
    // ble_app_watch_ancs_client_notifiy_process(id, att_name, att_content);
    uteModuleNotifySetAncsInfo(id,(uint8_t *)att_content,strlen(att_content));
#endif
}
