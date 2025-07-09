#include "include.h"
#include "port_ctp.h"

void port_ctp_init(void)
{
    gpio_t gpio;

    bsp_gpio_cfg_init(&gpio, PORT_CTP_SCL);
    if (gpio.sfr) {
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);
        gpio.sfr[GPIOxPU] |= BIT(gpio.num);
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxFEN] |= BIT(gpio.num);
    }

    bsp_gpio_cfg_init(&gpio, PORT_CTP_SDA);
    if (gpio.sfr) {
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);
        gpio.sfr[GPIOxPU] |= BIT(gpio.num);
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxFEN] |= BIT(gpio.num);
    }

    bsp_gpio_cfg_init(&gpio, PORT_CTP_RST);
    if (gpio.sfr) {
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxSET] = BIT(gpio.num);
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
    }

    i2c_t *CTP_IIC = ctp_iic_reg_get();
#if (PORT_CTP_IIC_HW == 0)
    CTP_IIC->map->FUNCMCONx = (PORT_CTP_MAP_GPIO << 8);
#elif (PORT_CTP_IIC_HW == 1)
    CTP_IIC->map->FUNCMCONx = (PORT_CTP_MAP_GPIO << 12);
#endif
}

void port_ctp_exit(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, PORT_CTP_SCL);
    if (gpio.sfr) {
        gpio.sfr[GPIOxPU] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);
    }

    bsp_gpio_cfg_init(&gpio, PORT_CTP_SDA);
    if (gpio.sfr) {
        gpio.sfr[GPIOxPU] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);
    }

    bsp_gpio_cfg_init(&gpio, PORT_CTP_RST);
    if (gpio.sfr) {
        gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);
    }
}
