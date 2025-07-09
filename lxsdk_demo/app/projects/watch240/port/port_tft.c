
#include "include.h"
#include "port_tft.h"

void port_tft_init(void)
{
    port_gpio_set_out(PORT_TFT_CS,1);

    port_gpio_set_func(PORT_TFT_CLK,0);

#if (GUI_MODE_SELECT == MODE_4WIRE_8BIT || GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
    GPIOAFEN |= BIT(2);                         // D0
    GPIOADE  |= BIT(2);
    GPIOADIR |= BIT(2);

    DC_ENABLE();
#else
    //GPIOAFEN |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));  // D0/D1/D2/D3
    //GPIOADE  |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));
    //GPIOADIR |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));
    port_gpio_set_func(PORT_TFT_GPIO_D0,0);
    port_gpio_set_func(PORT_TFT_GPIO_D1,0);
    port_gpio_set_func(PORT_TFT_GPIO_D2,0);
    port_gpio_set_func(PORT_TFT_GPIO_D3,0);
#endif

    FUNCMCON2 = BIT(28);
}

void port_tft_reset(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, PORT_TFT_RST);
    if (gpio.sfr == NULL)
    {
        return;
    }

    gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
    gpio.sfr[GPIOxDE] |= BIT(gpio.num);
    gpio.sfr[GPIOxSET] = BIT(gpio.num);
    gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
    delay_ms(10);
    gpio.sfr[GPIOxCLR] = BIT(gpio.num);
    delay_ms(20);
    gpio.sfr[GPIOxSET] = BIT(gpio.num);
    delay_ms(50);
}

void port_tft_exit(void)
{
#if (GUI_MODE_SELECT == MODE_4WIRE_8BIT || GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
    需要配置对应协议IO
#elif GUI_MODE_SELECT == MODE_QSPI

    port_gpio_disable(PORT_TFT_CLK);
    port_gpio_disable(PORT_TFT_CS);
    port_gpio_disable(PORT_TFT_RST);

    port_gpio_disable(PORT_TFT_GPIO_D0);
    port_gpio_disable(PORT_TFT_GPIO_D1);
    port_gpio_disable(PORT_TFT_GPIO_D2);
    port_gpio_disable(PORT_TFT_GPIO_D3);

#else
    需要配置对应协议IO
#endif


}
