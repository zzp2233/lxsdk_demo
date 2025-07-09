

#ifndef _PORT_TFT_H
#define _PORT_TFT_H

#define PORT_TFT_INT            IO_PA6
#define PORT_TFT_INT_VECTOR     PORT_INT1_VECTOR

//RST
#define PORT_TFT_RST            IO_PA7
#define PORT_TFT_RST_H()        GPIOASET = BIT(7);//GPIOBSET = BIT(7);
#define PORT_TFT_RST_L()        GPIOACLR = BIT(7);//GPIOBCLR = BIT(7);

//CS
#define PORT_TFT_CS             IO_PH6
#define TFT_SPI_CS_EN()         GPIOHCLR = BIT(6);
#define TFT_SPI_CS_DIS()        GPIOHSET = BIT(6);

//CLK
#define PORT_TFT_CLK            IO_PA4


#if (CHIP_PACKAGE_SELECT == CHIP_5691G)
#define PORT_TFT_BL             GPIO_PE3
#define LCD_BL_EN()             {GPIOEDIR &= ~BIT(3); GPIOEDE  |= BIT(3); GPIOESET  = BIT(3);}
#define LCD_BL_DIS()            {GPIOEDIR |= BIT(3); GPIOEDE  &= ~BIT(3);}
#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
#define PORT_TFT_BL             GPIO_PE6
#define LCD_BL_EN()             {GPIOEDIR &= ~BIT(6); GPIOEDE  |= BIT(6); GPIOESET  = BIT(6);}
#define LCD_BL_DIS()            {GPIOEDIR |= BIT(6); GPIOEDE  &= ~BIT(6);}
#elif (CHIP_PACKAGE_SELECT == CHIP_5690G || CHIP_PACKAGE_SELECT == CHIP_5690F)
#define PORT_TFT_BL             GPIO_PE4
#define LCD_BL_EN()             {GPIOEDIR &= ~BIT(4); GPIOEDE  |= BIT(4); GPIOESET  = BIT(4);}
#define LCD_BL_DIS()            {GPIOEDIR |= BIT(4); GPIOEDE  &= ~BIT(4);}
#else
#error CHIP_PACKAGE_SELECT error!!!
#endif

//LCD-EN 外部供电给屏
#define LCD_ON()          {GPIOHDIR &= ~BIT(7); GPIOHDE  |= BIT(7); GPIOHSET  = BIT(7);}
#define LCD_OFF()         {GPIOHDIR &= ~BIT(7); GPIOHDE  |= BIT(7); GPIOHCLR  = BIT(7);}

//RS
#define TFT_SPI_DATA_EN()       GPIOASET = BIT(3);
#define TFT_SPI_CMD_EN()        GPIOACLR = BIT(3);

#if (GUI_MODE_SELECT == MODE_4WIRE_8BIT)
#define   DC_ENABLE()           {GPIOAFEN &= ~BIT(3); GPIOADE |=  BIT(3);GPIOASET = BIT(3); GPIOADIR &= ~BIT(3);} // DC脚 设置输出
#define   DC_CMD_EN()           TFT_SPI_CMD_EN();           // DC 拉低
#define   DC_DATA_EN()          TFT_SPI_DATA_EN();          // DC 拉高
#elif (GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
#define   DC_ENABLE()           {GPIOAFEN |= BIT(3); GPIOADE |=  BIT(3); GPIOADIR |= BIT(3);} //DC脚 设置输入
#define   DC_CMD_EN()           {DESPICON &= ~BIT(19);}     // DC 拉低
#define   DC_DATA_EN()          {DESPICON |= BIT(19);}      // DC 拉高
#elif (GUI_MODE_SELECT == MODE_QSPI)
//D0 -> PA2
#define PORT_TFT_D0          BIT(2)
#define PORT_TFT_GPIO_D0     IO_PA2
//D1 -> PA3
#define PORT_TFT_D1     BIT(3)
#define PORT_TFT_GPIO_D1     IO_PA3
//D2 -> PA1
#define PORT_TFT_D2     BIT(1)
#define PORT_TFT_GPIO_D2     IO_PA1
//D3 -> PA0
#define PORT_TFT_D3     BIT(0)
#define PORT_TFT_GPIO_D3     IO_PA0
#endif

void port_tft_init(void);
void port_tft_exit(void);
void port_tft_reset(void);

#endif
