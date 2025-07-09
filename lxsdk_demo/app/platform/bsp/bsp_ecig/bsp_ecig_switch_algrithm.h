
#ifndef _BSP_ECIG_SWITCH_ALGRITHM_H
#define _BSP_ECIG_SWITCH_ALGRITHM_H
#if ECIG_POLLING_CONTROL
void timer_hot_dual_isr(void);//40us
void timer_hot_single_isr(void);
void ecig_clear_short_flag(bool state);
#endif
#endif  //_ECIG_H

