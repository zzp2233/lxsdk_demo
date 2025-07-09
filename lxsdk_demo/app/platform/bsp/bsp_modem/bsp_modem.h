#ifndef __BSP_MODEM_H__
#define __BSP_MODEM_H__

#define SENDER_STA_UP()                                                        \
    do {                                                                       \
        GPIOFSET = BIT(2);                                                     \
    } while (0)

#define SENDER_STA_DOWN()                                                      \
    do {                                                                       \
        GPIOFCLR = BIT(2);                                                     \
    } while (0)

typedef struct
{
    char number[16];
    u8 disp_status;
    u8 spr;
} bsp_modem_t;

extern bsp_modem_t modem_cb;

void bsp_modem_init(void);
void bsp_modem_exit(void);
int bsp_modem_get_init_flag(void);

void bsp_modem_sleep_enter(void);
void bsp_modem_sleep_exit(void);

void bsp_iis_stop(void);

void bsp_modem_audio_start(void);
void bsp_modem_audio_stop(void);

void modem_aft_send_cmd(void);

void bsp_modem_call_times_inc(void);
u16 bsp_modem_call_get_times(void);
#endif
