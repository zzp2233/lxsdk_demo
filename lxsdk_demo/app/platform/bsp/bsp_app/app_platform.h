#ifndef _APP_PLATFROM_H_
#define _APP_PLATFROM_H_

void app_platform_init(void);                                         //app平台相关初始化
void app_platform_process(void);                                      //app平台进程

void app_ute_remind_init(void);
void app_msg_pop_up(uint8_t index);
void app_ute_msg_pop_up(uint8_t index);


#endif  //_APP_PLATFROM_H_
