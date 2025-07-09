#include "include.h"
#include "api.h"
#include "ute_module_music.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN


//设置音量的回调函数, setting_type: 0=同步音量, 1=手机设置音量, 2=按键设置音量
bool dev_vol_set_cb(uint8_t dev_vol, uint8_t media_index, uint8_t setting_type)
{
    if(setting_type & BIT(3))
    {
        sys_cb.hfp_vol = dev_vol;
    }
    else
    {
        sys_cb.vol = a2dp_vol_conver(dev_vol);
        uint8_t volume = sys_cb.vol * 6.25;
        uteModuleMusicSetPlayerVolume(volume);
    }

    TRACE("dev_vol_set_cb: %d(%x, %d, %d)\n", sys_cb.vol, dev_vol, media_index, setting_type);

    if(setting_type & BIT(3))
    {
        TRACE("dev_vol_set_cb  EVT_HFP_SET_VOL\n");
        // msg_enqueue(EVT_HFP_SET_VOL);
        if(sys_cb.incall_flag & INCALL_FLAG_SCO)
        {
            bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
            dac_fade_in();
            printf("HFP SET VOL: %d\n", sys_cb.hfp_vol);
        }
    }
    else
    {
        TRACE("dev_vol_set_cb  EVT_A2DP_SET_VOL\n");
        // msg_enqueue(EVT_A2DP_SET_VOL);
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
    }

    return true;
}

