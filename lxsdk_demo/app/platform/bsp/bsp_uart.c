#include "include.h"

typedef void (*huart_loop_rx_once_cb_t)(u16 cnt);

void huart_loop_init(u16 len, huart_loop_rx_once_cb_t cb);
void huart_loop_deinit(void);

#define UART_EXAMPLE_EN   0    ///普通uart示例程序开关
#define HUART_BAUD                              1500000
#define HUART_BAUD_VAL                          (((XOSC_CLK_HZ + (HUART_BAUD / 2)) / HUART_BAUD) - 1)
#define HSUART_DMA_BUF_LEN                      128
#define IRQ_HUART_VECTOR                        15      //huart

#if  UART_EXAMPLE_EN
static uint8_t hsuart_dma_rx_buf[HSUART_DMA_BUF_LEN];
static uint8_t hsuart_dma_tx_buf[HSUART_DMA_BUF_LEN];
static uint16_t example_hsuart_rec_len;
static uint8_t example_hsuart_buf[256];

u32 uart_buf = 0;
u8 uart_rx_flag = 0;

///发送数据
AT(.com_text.uart)
void uart1_putc(char ch)
{
    while(!(UART1CON & BIT(8)));
    UART1DATA = ch;
}

AT(.com_text.uart)
void hsuart_send(uint8_t count)
{
    HSUT0CON &= ~BIT(2);                            //RX IE Dis
    HSUT0CON |= BIT(3);                             //TX IE En
    HSUT0TXADR = DMA_ADR(hsuart_dma_tx_buf);
    HSUT0TXCNT = count;
}

///打印字符串
AT(.com_text.uart)
void uart1_puts(const char *str)
{
    while(*str)
    {
        uart1_putc(*str++);
    }
}

///串口中断处理
AT(.com_text.uart)
void uart1_isr(void)
{
    if(UART1CON & BIT(9))       //接收完成
    {
        UART1CPND |= BIT(9);
        //需要把数据保存起来
//        uart_buf = UART1DATA;
//        uart_rx_flag = 1;
    }
}

void uart1_rx_printf(void)
{
    if(uart_rx_flag)
    {
        printf("\nuart_buf = 0x%X\n", uart_buf);
        uart_rx_flag = 0;
    }
}

///串口初始化
AT(.text.at_mode)
void uart1_init(u32 baud)
{
    u32 baud_cfg;
    //PA3_RX,PA4_TX(G2)
    GPIOAFEN |= BIT(3)|BIT(4);
    GPIOADIR |= BIT(3);
    GPIOADIR &= ~BIT(4);
    GPIOAPU |= BIT(3)|BIT(4);
    GPIOADE |= BIT(3)|BIT(4);
    //mapping uart1(tx/rx) -> G2
    FUNCMCON0 |= (0xf << 28)|(0xf << 24);
    FUNCMCON0 |= (0x2 << 28)|(0x2 << 24);

    CLKGAT0 |= BIT(14);                                 //enable uart1 clk

    UART1CON = 0;
    UART1CON |= BIT(5);                                 //clock src select uart_inc
    CLKCON1 |= BIT(14);                                 //uart_inc select x26m_clkdiv2
    baud_cfg = ((XOSC_CLK_HZ/2+ baud/2)/baud) - 1;         //baud_cfg=(串口时钟/波特率)-1; 四舍五入
    UART1BAUD = (baud_cfg << 16)|baud_cfg;

    UART1CON |= (BIT(0)|BIT(2)|BIT(7));                 //使能uart,接收中断

    register_isr(IRQ_UART_VECTOR, uart1_isr);
    PICPR &= ~BIT(IRQ_UART_VECTOR);
    PICEN |= BIT(IRQ_UART_VECTOR);
}

//vusb作串口收发用
AT(.text.at_mode)
void vusb_uart1_init(u32 baud)
{
    u32 baud_cfg;

    //VUSB_TX+RX(G3)
    PWRCON0 |= BIT(30);
    RTCCON &= ~BIT(6);
    //单线收发map
    FUNCMCON0 |= (0xf << 28) | (0xf << 24);
    FUNCMCON0 |= (0x7 << 28) | (0x3 << 24);             //单线时RXMAP配0x7

    CLKGAT0 |= BIT(21);                                 //enable uart1 clk

    UART1CON = 0;
    UART1CON |= BIT(5);                                 //clock src select uart_inc
    CLKCON1 |= BIT(14);                                 //uart_inc select x26m_clkdiv2
    baud_cfg = ((XOSC_CLK_HZ/2+ baud/2)/baud) - 1;         //baud_cfg=(串口时钟/波特率)-1; 四舍五入
    UART1BAUD = (baud_cfg << 16)|baud_cfg;
    UART1CON |= (BIT(0) | BIT(2) | BIT(6) | BIT(7));            //使能uart,接收中断,BIT(6)单线

    register_isr(IRQ_UART_VECTOR, uart1_isr);
    PICPR &= ~BIT(IRQ_UART_VECTOR);
    PICEN |= BIT(IRQ_UART_VECTOR);
}

// AT(.com_text.xxx)
// const char dbg_str[] = "0x%x ";
// AT(.com_text.xxx)
// const char dbg_str2[] = "\nrx done \n";

// AT(.com_text.uart)
// void hsuart_isr_rece_handle(void)
// {
//     if(HSUT0CON & BIT(12)) {   //hsuart rx
//      for(int i =0;i<HSUART_DMA_BUF_LEN;i++){
//          printf(dbg_str,hsuart_dma_rx_buf[i]);
//      }
//      printf(dbg_str2);
//         HSUT0CPND = BIT(0);
//         HSUT0CPND = BIT(12);
//         HSUT0FIFOCNT = 0;
//         HSUT0CPND = BIT(15);
//         HSUT0RXADR = DMA_ADR(&hsuart_dma_rx_buf);
//         HSUT0RXCNT = 4 * HSUART_DMA_BUF_LEN;
//     }
// }

// bool bsp_hsuart_init(void)
// {
//  printf("bsp_hsuart_init() enter\n");

//     CLKCON0 = (CLKCON0 & ~(7 << 17)) | (0 << 17);   //HUART select xosc26_clk
//     CLKGAT0 |= BIT(11);                             //HUART CLK enable

//     //PE7_TX,PF5_RX(G2)

//  GPIOFFEN |= BIT(5);
//     GPIOFDIR |= BIT(5);
//     GPIOFPU |= BIT(5);
//     GPIOFDE |= BIT(5);

//     GPIOEFEN |= BIT(7);
//  GPIOEDIR &= ~BIT(7);
//     GPIOEPU |= BIT(7);
//     GPIOEDE |= BIT(7);

//     //mapping uart1(tx/rx) -> G6/G7
//     FUNCMCON0 |= (0xf << 16)|(0xf << 20);
//     FUNCMCON0 |= (0x6 << 16)|(0x7 << 20);


//     HSUT0BAUD = (HUART_BAUD_VAL << 16) | HUART_BAUD_VAL;
//     HSUT0TMRCNT = 32;
//     HSUT0CON = (BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(7) | BIT(10));   //enable hsuart dma
//     HSUT0CPND = (BIT(0) | BIT(1) | BIT(12) | BIT(15));

//     HSUT0RXADR = DMA_ADR(hsuart_dma_rx_buf);
//     HSUT0RXCNT = 4 * HSUART_DMA_BUF_LEN;

//     // huart_init_do(HUART_TR_PE7, HUART_TR_PF5, HUART_BAUD, hsuart_dma_rx_buf, HSUART_DMA_BUF_LEN);
//     // HSUT0CON &= ~BIT(6);
//     // HSUT0CON |= BIT(2);
//     // HSUT0CON &= ~BIT(9);

//     register_isr(IRQ_HUART_VECTOR, hsuart_isr_rece_handle);              // 注册串口中断
//     PICPR &= ~BIT(IRQ_HUART_VECTOR);                       //配置中断优先级
//     PICEN |= BIT(IRQ_HUART_VECTOR);

// #if 0 //串口发送
//  //发送实例
//  for(int i = 0;i<HSUART_DMA_BUF_LEN;i++){
//      hsuart_dma_tx_buf[i] = i;
//  }

//  hsuart_send(HSUART_DMA_BUF_LEN);
// #endif
//     return true;
// }

// static au_stm_t uart_stm;
u8 huart_isr_buf[HSUART_DMA_BUF_LEN / 2];
AT(.com_text.uart)
void hsuart_isr_recv_handle(u16 cnt)
{
    for (u16 i = 0; i < cnt; i++)
    {
        uart_isr_buf[i] = huart_getchar();
    }
    // push into fifo (optional)
    // puts_stm_buf(&uart_stm, uart_isr_buf, cnt);

    // send msg
    // msg_enqueue(MSG);
}

bool bsp_hsuart_init(void)
{
#define HUART_TR_PE7 5
#define HUART_TR_PF5 6
    huart_init_do(HUART_TR_PE7, HUART_TR_PF5, HUART_BAUD, hsuart_dma_rx_buf, HSUART_DMA_BUF_LEN);

    // 停止位
    HSUT0CON &= ~BIT(9);
    // 剩下不要动
    HSUT0CON |= BIT(2);
    HSUT0CON |= BIT(17); // half

    huart_loop_init(HSUART_DMA_BUF_LEN, hsuart_isr_recv_handle);
}

#endif
