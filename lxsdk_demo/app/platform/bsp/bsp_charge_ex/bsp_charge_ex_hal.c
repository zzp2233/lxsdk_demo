#include "include.h"

u16 bsp_charge_ex_vbat_get(void)
{
#if VBAT_DETECT_EN
    return sys_cb.vbat;         //主控采集
#else
    return charge_ex_cb.v_batm; //外部充电ic采集
#endif
}

bool bsp_charge_ex_is_charge(void)
{
    return charge_ex_cb.pgd;
}

bool bsp_charge_ex_is_waking_up(void)
{
#if BOX_OPEN_WAKEUP_EARPHONE
    if (charge_ex_cb.wkup_0V_tick || charge_ex_cb.wkup_5V_tick)
    {
        return true;
    }
#endif
    return false;
}

AT(.text.func.idle.charge)
bool bsp_charge_ex_eoc_check_en(void)
{
    bool en = false;

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C || CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
    en = lp7812c_eoc_check_en();
#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    en = charge_ic_eoc_check_en();
#endif

    return en;
}

AT(.text.func.idle.charge)
bool bsp_charge_ex_eoc_check_inbox(void)
{
    bool sta = false;

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C)
    sta = lp7812c_eoc_check_inbox();
#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    sta = charge_ic_eoc_check_inbox();
#endif

    return sta;
}


AT(.text.func.idle.charge)
bool bsp_charge_ex_comm_allow(void)
{
    bool comm_allow = true;

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C || CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
    if (charge_ex_cb.comm_delay && !tick_check_expire(lp7812c_comm_mode_tick(), 500))  //充电模式切通讯模式, 需要延时才能收到完整rx包
    {
        comm_allow = false;
    }
#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    if (charge_ex_cb.comm_delay && !tick_check_expire(charge_ic_comm_mode_tick(), 300))
    {
        comm_allow = false;
    }
#endif

    if (bsp_charge_ex_is_waking_up())     //唤醒耳机过程中不允许通讯
    {
//        printf("wkup earphone, disallow tx!\n");
        comm_allow = false;
    }

    return comm_allow;
}

AT(.text.func.idle.charge)
u8 bsp_charge_ex_mode(void)
{
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C || CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
    return lp7812c_work_mode();
#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    return charge_ic_work_mode();
#else
    return 0;
#endif
}

AT(.text.func.idle.charge)
bool bsp_charge_ex_hal_mode_set(u8 mode)
{
    bool ret = false;

#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C || CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
    ret = lp7812c_work_mode_set(mode);
#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    ret = charge_ic_work_mode_set(mode);
#endif

    return ret;
}

void bsp_charge_ex_charge(u8 en)
{
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C || CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
    lp7812c_charge(en);
#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    charge_ic_charge(en);
#endif
}

void bsp_charge_ex_hal_process(void)
{
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C || CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
    lp7812c_process();
#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    charge_ic_process();
#endif
}

void bsp_charge_ex_ic_init(void)
{
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LP7812C || CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
    lp7812c_init();
#elif (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    charge_ic_init();
#endif
}
