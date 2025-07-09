#ifndef _GUI_H
#define _GUI_H

#include "components.h"
#include "tft.h"
#include "ctp.h"

void gui_init(void);
void gui_sleep(void);
void gui_wakeup(void);
void gui_halt(u32 halt_no);
void de_fill_rgb565(void *buf, u16 color, int cnt);
void de_fill_num(void *buf, u32 num, int ln);
void gui_clear_screen(uint16_t color,uint32_t size);

#endif
