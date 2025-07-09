#ifndef _API_PWR_H_
#define _API_PWR_H_

void pmu_init(u8 buck_en);
void set_buck_mode(u8 buck_en);
void power_gate_3v3_on(void);               //PF4 power gate 3v3打开
void power_gate_3v3_off(void);              //PF4 power gate 3v3关闭
void hr_vdd_ldo_on(void);                   //开VDDHR
void hr_vdd_ldo_off(void);                  //关VDDHR
#endif // _API_PWR_H_

