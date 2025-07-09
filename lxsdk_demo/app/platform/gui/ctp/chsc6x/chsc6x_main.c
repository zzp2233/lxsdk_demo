#include "include.h"
#include "chsc6x_comp.h"
#include "chsc6x_platform.h"
#include "ute_application_common.h"

#if (CTP_SELECT == CTP_CHSC6X)

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
AT(.com_rodata)
const char chsc6x_str_err1[] = "CHSC641X_MAX_POINTS_NUM more than two\n";
AT(.com_rodata)
const char chsc6x_str_err2[] = "chsc6x: chsc6x_read_touch_info iic err! rd_len=%d, ret=%d \n";
AT(.com_rodata)
const char chsc6x_str_point[] = "chsc6x[%d]:  X:%d, Y:%d, point_num:%d,flag:%d, id:%d \n";
#else
#define TRACE(...)
#endif

struct ts_event
{
    unsigned short x; /*x coordinate */
    unsigned short y; /*y coordinate */
    int flag; /* touch event flag: 0 -- down; 1-- up; 2 -- contact */
    int id;   /*touch ID */
};

struct ts_event events[CHSC6X_MAX_POINTS_NUM];
bool is_init = false;

AT(.com_text.ctp)
void chsc6x_read_touch_info(void)
{
    int ret;
    int rd_len = 0;
#if TRACE_EN
    unsigned char point_num;
#endif
    unsigned char read_buf[6];
//    struct ts_event events[CHSC6X_MAX_POINTS_NUM];

    if(1 == CHSC6X_MAX_POINTS_NUM)
    {
        rd_len = 3;
    }
    else if(2 == CHSC6X_MAX_POINTS_NUM)
    {
        if ((CHSC6X_RES_MAX_X < 255) && (CHSC6X_RES_MAX_Y < 255) )
        {
            rd_len = 5;
        }
        else
        {
            rd_len = 6;
        }
    }
    else
    {
        TRACE(chsc6x_str_err1);
        return;
    }

    ret = chsc6x_i2c_read(CHSC6X_I2C_ID, read_buf, rd_len);
//    ret = chsc6x_read_bytes_u16addr_sub(CHSC6X_I2C_ID, 0x00, read_buf, rd_len);
    if(rd_len == ret)
    {
#if TRACE_EN
        point_num = read_buf[0] & 0x03;
#endif
        if(1 == CHSC6X_MAX_POINTS_NUM)
        {
            events[0].x = (unsigned short)(((read_buf[0] & 0x40) >> 6) << 8) | (unsigned short)read_buf[1];
            events[0].y = (unsigned short)(((read_buf[0] & 0x80) >> 7) << 8) | (unsigned short)read_buf[2];

            events[0].flag= (read_buf[0] >> 4) & 0x03;
            events[0].id = (read_buf[0] >>2) & 0x01;
            TRACE(chsc6x_str_point, 0, events[0].x, events[0].y, point_num, events[0].flag, events[0].id);

        }
        else if(2 == CHSC6X_MAX_POINTS_NUM)
        {
            if ((CHSC6X_RES_MAX_X > 255) || (CHSC6X_RES_MAX_Y > 255) )
            {
                events[0].x = (unsigned short)((read_buf[5] & 0x01) << 8) | (unsigned short)read_buf[1];
                events[0].y = (unsigned short)((read_buf[5] & 0x02) << 7) | (unsigned short)read_buf[2];

                events[0].flag = (read_buf[0] >> 4) & 0x03;
                events[0].id = (read_buf[0] >>2) & 0x01;
                TRACE(chsc6x_str_point, 1, events[0].x, events[0].y, point_num, events[0].flag, events[0].id);

                events[1].x = (unsigned short)((read_buf[5] & 0x04) << 6) | (unsigned short)read_buf[3];
                events[1].y = (unsigned short)((read_buf[5] & 0x08) << 5) | (unsigned short)read_buf[4];

                events[1].flag = (read_buf[0] >> 6) & 0x03;
                events[1].id = (read_buf[0] >>3) & 0x01;
                TRACE(chsc6x_str_point, 2, events[0].x, events[0].y, point_num, events[0].flag, events[0].id);
            }
            else
            {
                events[0].x = read_buf[1];
                events[0].y = read_buf[2];

                events[0].flag = (read_buf[0] >> 4) & 0x03;
                events[0].id = (read_buf[0] >>2) & 0x01;
                TRACE(chsc6x_str_point, 3, events[0].x, events[0].y, point_num, events[0].flag, events[0].id);

                events[1].x = read_buf[3];
                events[1].y = read_buf[4];

                events[1].flag = (read_buf[0] >> 6) & 0x03;
                events[1].id = (read_buf[0] >>3) & 0x01;
                TRACE(chsc6x_str_point, 4, events[0].x, events[0].y, point_num, events[0].flag, events[0].id);
            }
        }
    }
    else
    {
        TRACE(chsc6x_str_err2, rd_len, ret);
    }
}

AT(.com_text.ctp)
void ctp_chsc6x_readkick(void)
{
    chsc6x_read_touch_info();
}

AT(.com_text.ctp)
bool ctp_chsc6x_get_point(s32 *x, s32 *y)
{
#if CTP_POINT_FLIP
    *x = COMPO_GET_POS_X(events[0].x, events[0].y);
    *y = COMPO_GET_POS_Y(events[0].x, events[0].y);
#else
    *x = events[0].x;
    *y = events[0].y;
#endif

    return ((events[0].flag & 0x2) >> 1);
}

AT(.com_text.ctp)
bool chsc6x_is_inited(void)
{
    return is_init;
}

void chsc6x_resume(void)
{
    chsc6x_info("touch_resume");
    chsc6x_tp_reset();
}

void chsc6x_suspend(void)
{
    unsigned char buft[1] = {0};
    int ret = -1;
    chsc6x_tp_reset();
    ret = chsc6x_write_bytes_u16addr_sub(CHSC6X_I2C_ID, 0xa503, buft, 0);
    if(ret == 0)
    {
        chsc6x_info("touch_suspend OK \r\n");
    }
    else
    {
        chsc6x_info("touch_suspend failed \r\n");
    }
}

void chsc6x_gestrue(void)
{
    int ret = -1;
    chsc6x_tp_reset();
    ret = chsc6x_write_reg(CHSC6X_I2C_ID, 0xD0, 0x01);
    if(0 != ret)
    {
        chsc6x_err("Enable gestrue failed! \r\n");
    }
}

void chsc6x_dbcheck(void)
{
    unsigned char buft[1] = {0};
    int ret = -1;
    chsc6x_tp_reset();
    ret = chsc6x_write_bytes_u16addr_sub(CHSC6X_I2C_ID, 0xd001, buft, 0);   //0xd001 close
    if(ret == 0)
    {
        chsc6x_info("Enable dbcheck OK \r\n");
    }
    else
    {
        chsc6x_info("Enable dbcheck failed \r\n");
    }
}

void chsc6x_palmcheck(void)
{
    unsigned char buft[1] = {0};
    int ret = -1;
    chsc6x_tp_reset();
    ret = chsc6x_write_bytes_u16addr_sub(CHSC6X_I2C_ID, 0xd101, buft, 0);   //0xd100 close
    if(ret == 0)
    {
        chsc6x_info("Enable palmcheck OK \r\n");
    }
    else
    {
        chsc6x_info("Enable palmcheck failed \r\n");
    }
}

bool ctp_chsc6x_init(void)
{
    int i = 0;
    int ret = 0;
    unsigned char fw_update_ret_flag = 0; //1:update OK, !0 fail
    struct ts_fw_infos fw_infos;

    if (uteApplicationCommonGetSystemPowerOnSecond() > 5) //chsc6x 获取TP信息太慢，影响亮屏速度，只在开机时获取一次
    {
        return true;
    }

    for(i = 0; i < 3; i++)
    {
        ret = chsc6x_tp_dect(&fw_infos, &fw_update_ret_flag);
        if(1 == ret)
        {
#if CHSC6X_AUTO_UPGRADE /* If need update FW */
            printf("chsc6x_tp_dect succeed!\r\n");
            if(1 == fw_update_ret_flag)
            {
                printf("update fw succeed! \r\n");
                break;
            }
            else
            {
                printf("update fw failed! \r\n");
            }
#else
            break;
#endif
        }
        else
        {
            printf("chsc6x_tp_dect failed! i = %d \r\n", i);
        }
    }

    is_init = true;
    return true;
}
#else
void chsc6x_gestrue(void) {}
void chsc6x_dbcheck(void) {}
void chsc6x_suspend(void) {}
#endif  //(CTP_SELECT == CTP_CHSC6X)
