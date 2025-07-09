#ifndef _BSP_UART_H
#define _BSP_UART_H



void uart1_putc(char ch);
void uart1_puts(const char *str);
void uart1_isr(void);
bool bsp_hsuart_init(void);
void hsuart_send(void);
void hsuart_isr_rece_handle(void);
void uart1_init(u32 baud);
void vusb_uart1_init(u32 baud);

#endif
