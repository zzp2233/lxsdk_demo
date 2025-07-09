
#ifndef __CHSC6X_MAIN_H__
#define __CHSC6X_MAIN_H__
// #include <stdint.h>

// #define OS_OK                    (0)
//#define OS_TPERROR                 (1)
#define TP_RETRY_CNT             (2)
#define TP_RETRY_CNT2            (3)
#define TP_RETRY_CNT3            (5)

#define CHSC6X_I2C_DEVICE  (0x2E) //7bit

typedef struct sm_touch_dev
{
    int int_pin;
    int rst_pin;
}sm_touch_dev, *psm_touch_dev;


typedef enum 
{
    OS_OK = 0,
    OS_TPERROR = 1,
    OS_ERR_DCHECK = 2,
    OS_ERR_TIMEOUT = 3,
    OS_ERR_COUNT = 4,
    OS_ERR_X = 5,
}err_TypeID;

struct ts_event {
     uint16_t x; /*x coordinate */
     uint16_t y; /*y coordinate */
    int flag; /* touch event flag: 0 -- down; 1-- up; 2 -- contact */
    int id;   /*touch ID */
};

bool chsc6x_is_inited(void);
#endif


