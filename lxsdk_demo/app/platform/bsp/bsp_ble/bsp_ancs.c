#include "include.h"
#include "bsp_ancs.h"
#include "ute_module_message.h"

#if (LE_ANCS_CLIENT_EN && LE_EN)

void ble_ancs_client_notifiy_callback(u8 id, const char *att_name, const char *att_content)
{
    printf("id:%d\n", id);
    printf("Notification:%s - %s\n", att_name, att_content);

    ble_app_ancs_client_notifiy_callback(id, att_name, att_content);
}

void ble_ancs_notifiy_info_callback(uint32_t uid, uint8_t event_id, uint8_t event_flags, uint8_t category_id, uint8_t category_count)
{
    ble_cb.ansc_uid = uid;
    printf("uid:%x, event_id:%x, event_flags:%x, category_id:%x, category_count:%x\n", uid, event_id, event_flags, category_id, category_count);

    if(bsp_system_is_sleep())
    {
        sys_cb.need_wakeup_flag = true;
    }

    if(event_flags | (1 << 2) != 0x04)
    {
        if(event_id == 2 && category_id == 1) //来电接听/挂断
        {
            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_CALL_DISABLE_NOTIFY,0);
        }
        else if (event_id == 0 && category_id == 1) //来电
        {
            sys_cb.ancs_missed_call = false;
        }
        else if (event_id == 0 && category_id == 2) //未接来电
        {
            sys_cb.ancs_missed_call = true;
        }
    }
}

#endif // (LE_ANCS_CLIENT_EN && LE_EN)

