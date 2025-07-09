#ifndef _PORT_CTP_H
#define _PORT_CTP_H

#if (CHIP_PACKAGE_SELECT == CHIP_5690G || CHIP_PACKAGE_SELECT == CHIP_5690F)
#define PORT_CTP_SCL            IO_PH2
#define PORT_CTP_SDA            IO_PH1
#define PORT_CTP_MAP_GPIO       7
#define PORT_CTP_IIC_HW         1

#define PORT_CTP_INT            IO_PF5
#define PORT_CTP_INT_VECTOR     PORT_INT2_VECTOR

#if !MODEM_CAT1_EN
#define PORT_CTP_RST            IO_PH3
#define PORT_CTP_RST_H()        GPIOHSET = BIT(3);
#define PORT_CTP_RST_L()        GPIOHCLR = BIT(3);
#else
#define PORT_CTP_RST            IO_PG7
#define PORT_CTP_RST_H()        GPIOGSET = BIT(7);
#define PORT_CTP_RST_L()        GPIOGCLR = BIT(7);
#endif
#elif (CHIP_PACKAGE_SELECT == CHIP_5691G || CHIP_PACKAGE_SELECT == CHIP_5691C_F)

#define PORT_CTP_SCL            IO_PB6
#define PORT_CTP_SDA            IO_PB5
#define PORT_CTP_MAP_GPIO       4
#define PORT_CTP_IIC_HW         1

#define PORT_CTP_INT            IO_PB4
#define PORT_CTP_INT_VECTOR     PORT_INT5_VECTOR

#define PORT_CTP_RST            IO_PB7
#define PORT_CTP_RST_H()        GPIOBSET = BIT(7);
#define PORT_CTP_RST_L()        GPIOBCLR = BIT(7);

#else
#error CHIP_PACKAGE_SELECT defined select error!!!!
#endif

void port_ctp_init(void);
void port_ctp_exit(void);

#endif
