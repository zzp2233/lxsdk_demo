#ifndef _BSP_I2C_H
#define _BSP_I2C_H

#define DATA_CNT_1B     1
#define DATA_CNT_2B     2
#define DATA_CNT_3B     3
#define DATA_CNT_4B     4

#define START_FLAG0     BIT(3)
#define DEV_ADDR0       BIT(4)
#define REG_ADDR_0      BIT(5)
#define REG_ADDR_1      (BIT(5) | BIT(6))

#define START_FLAG1     BIT(7)
#define DEV_ADDR1       BIT(8)
#define RDATA           BIT(9)
#define WDATA           BIT(10)

#define STOP_FLAG       BIT(11)
#define NACK            BIT(12)

typedef struct
{
    volatile uint32_t IICxCON0;
    volatile uint32_t IICxCON1;
    volatile uint32_t IICxCMDA;
    volatile uint32_t IICxDATA;
    volatile uint32_t IICxDMAADR;
    volatile uint32_t IICxDMACNT;
} i2c_sfr_t;

typedef struct
{
    volatile uint32_t FUNCMCONx;
} i2c_map_t;

typedef struct
{
    i2c_sfr_t *sfr;
    i2c_map_t *map;
} i2c_t;

void bsp_i2c_init(void);
void bsp_i2c_start(void);
void bsp_i2c_stop(void);
void bsp_i2c_tx_byte(uint8_t dat);
uint8_t bsp_i2c_rx_byte(void);
bool bsp_i2c_rx_ack(void);
void bsp_i2c_tx_ack(void);
void bsp_i2c_tx_nack(void);
void i2c_gsensor_init(void);

void bsp_hw_i2c_rx_buf(u32 i2c_cfg, u16 dev_addr, u16 reg_addr, u8 *buf, u16 len);
void bsp_hw_i2c_tx_buf(u32 i2c_cfg, u16 dev_addr, u16 reg_addr, u8 *buf, u16 len);
void bsp_hw_i2c_tx_byte(u32 i2c_cfg, u16 dev_addr, u16 reg_addr, u32 data);

void os_i2c0_lock(uint32_t ms);
void os_i2c0_unlock(void);

#endif
