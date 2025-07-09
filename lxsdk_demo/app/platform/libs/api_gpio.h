#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H

enum
{
    GPIOxSET    =   0,
    GPIOxCLR,
    GPIOx,
    GPIOxDIR,
    GPIOxDE,
    GPIOxFEN,
    GPIOxDRV,
    GPIOxPU,
    GPIOxPD,
    GPIOxPU200K,
    GPIOxPD200K,
    GPIOxPU300,
    GPIOxPD300,
};

typedef struct
{
    psfr_t sfr;                                             //GPIO SFR ADDR
    u8 num;
    u8 type;                                                //type = 1, 没有300R的强上下拉电阻。 type = 0, 普通IO, 有内部300R上下拉电阻。
    u16 pin;
} gpio_t;

#define bsp_gpio_cfg_init(x, y)         gpio_cfg_init(x, y)

void gpio_cfg_init(gpio_t *g, u8 io_num);                   //根据GPIO number初始化GPIO结构体
u8 get_adc_gpio_num(u8 adc_ch);
void adcch_io_pu10k_enable(u8 adc_ch);

///port wakeup and interrupt

#define PORT_INT0_VECTOR            0
#define PORT_INT1_VECTOR            1
#define PORT_INT2_VECTOR            2
#define PORT_INT3_VECTOR            3
#define PORT_INT4_VECTOR            4
#define PORT_INT5_VECTOR            5
#define PORT_INT6_VECTOR            6
#define PORT_INT7_VECTOR            7

void port_var_init(void);
void port_irq_register(int irq_num, isr_t isr);
void port_irq_free(int irq_num);
u8 port_wakeup_get_status(void);
bool port_wakeup_init(u8 io_num, u8 edge, u8 pupd_sel);     //参数edge: 0->上升沿, 1->c,  参数pupd_sel: 0->不开内部上拉, 1->开内部上拉, 2->开内部下拉
bool port_wakeup_exit(u8 io_num);
void wko_wakeup_init(u8 edge);
void wko_wakeup_exit(void);
bool port_wko_is_wakeup(void);
//上升沿 ： 中断7  下降沿：中断6
void port_wakeup_all_init(u8 io_num, u8 edge, u8 pupd_sel);

#endif // _BSP_GPIO_H
