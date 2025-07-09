#include "include.h"

u32 getcfg_vddbt_sel(void)
{
    return xcfg_cb.vddbt_sel;
}

u32 getcfg_vddio_sel(void)
{
    return xcfg_cb.vddio_sel;
}

u32 getcfg_vddbt_capless_en(void)
{
    return xcfg_cb.vddbt_capless_en;
}

u32 getcfg_micbias_method_sel(void)
{
    return xcfg_cb.mic_bias_method;
}


