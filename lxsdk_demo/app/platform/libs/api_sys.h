#ifndef _API_SYS_H_
#define _API_SYS_H_

#define IRQ_TMR1_VECTOR                 4
#define IRQ_TMR2_VECTOR                 5
#define IRQ_IRRX_VECTOR                 6
#define IRQ_QDEC_VECTOR                 6
#define IRQ_UART_VECTOR                 14
#define IRQ_TMR3_VECTOR                 16
#define IRQ_TMR4_VECTOR                 17
#define IRQ_TMR5_VECTOR                 18
#define IRQ_SPI_VECTOR                  20
#define IRQ_PORT_VECTOR                 26
#define IRQ_DESPI_VECTOR                27
#define IRQ_I2C_VECTOR                  29
#define IRQ_CTP_VECTOR                  30
#define IRQ_TE_TICK_VECTOR              31

typedef enum RESTART_CAUSE_TYPE_
{
    RESTART_WDT = 0,            //看门狗复位
    RESTART_SW,                 //软件复位
    RESTART_WK10S,              //长按10s复位
    RESTART_VUSB,               //充电复位开机
    RESTART_WKUP,               //按键开机
    RESTART_FIRST_ON,           //第一次上电开机
    RESTART_UNKNOWN,            //未知开机，保留项目
} RESTART_CAUSE_TYPE;

typedef struct
{
    u8  vbg;                //trim到0.6V的寄存器配置值      RI_BGTRIM,      RTCCON8[28:24]
    u8  vbat;               //trim到4.2V的寄存器配置值      RI_BGCH_TRIM,   RTCCON8[14 10]
    u16 chg_icoef;          //充电电流换算系数
    u16 vbg_volt;           //vbg目标值
    u16 vbat_coef;
    u8  vbg_lv;             //vbg trim精度等级
    u8  resv[3];
} sys_trim_t;
extern sys_trim_t sys_trim;

typedef void (*isr_t)(void);

extern u16 tmr5ms_cnt;

void sys_set_tmr_enable(bool tmr5ms_en, bool tmr1ms_en);
u32 sys_get_rand_key(void);

void xosc_init(void);

/*
*   pr = 1, 高优先级中断, 中断入口函数isr放公共区且要加FIQ, 例如：
*   AT(.com_text.isr) FIQ
*   void timer3_isr(void) {}
*
*   pr = 0, 低优先级中断,不用加FIQ, 函数放入公共区, 例如：
*   AT(.com_text.isr)
*   void timer3_isr(void) {}
*
*   中断函数需要尽量精简，否则影响系统效率。推荐用低优先级中断，需要很高响应速度时才用高优先级中断
*/
bool sys_irq_init(int vector, int pr, isr_t isr);           //初始化一个中断, pr为优先级

u16 get_random(u16 num);                                    //获取[0, num-1]之间的随机数
void sys_unicode_to_utf8(u8 *u, u16 *p, int max_word_cnt);      //unicode to utf8
u8 utf8_char_size(u8 code);
uint8_t * bt_get_package_rf_param(u8 package);
void vddio_voltage_configure(void);
void dbg_clk_out(u32 type, u32 div);
u16 bt_get_mic_pmaxow(void);                                //获取mic能量值(峰峰值)
void sdadc_analog_mic_mute(bool en);


/**
 * 给客户获取死机信息复位，是可以在任何时候调用的，建议客户放在一个隐藏触发界面显示，辅助debug
 * 返回的u32类型的指针，指向u32 cpu_gprs[32]数组
 * cpu_gprs[23] 存储开机时候 RTCCON0 值
   cpu_gprs[24] 存储开机时候 RTCCON10 值
   cpu_gprs[25] 存储开机时候 RTCCON9 值
   cpu_gprs[26] 存储开机时候 LVDCON 值
   以上信息可以用来判断开机的状态，是看门狗复位，或是上电开机，或是低电复位等状态；

   其他异常开机辅助：
   1、{cpu_gprs[30] 这个值为0x55555555，表明是看门狗复位；}
      cpu_gprs[31] 存储了复位的地方PC地址；

   2、{cpu_gprs[30] 非0x55555555，表明是执行代码异常复位：}
      cpu_gprs[31] 存储了复位的地方PC地址；
      cpu_gprs[30] 存储异常原因，可以来区分复位原因，比如除0，取指错误等
      cpu_gprs[29] 存储PCERR寄存器,可以理解为出事前的位置
      cpu_gprs[28] 存储PCST寄存器，可以理解为出事前前的位置
*/
u32* exception_debug_info_get(void);


/**
 * 判断重启或开机原因
 * 返回重启或开机原因，参考RESTART_CAUSE_TYPE说明
 */
RESTART_CAUSE_TYPE exception_restart_cause(void);

/**
 * 调试辅助函数
 * 函数作用:设置目标地址，用于查看哪个PC地址修改了目标地址的值
 * addr:目标地址
 * 使用示例:int point = 0; watch_point_set((u32)&point));
 * 日志查看:当PC指针修改目标地址的值时返回如下打印信息:
 *          watch_point: 参数1, 参数2, 参数3
 *          参数1:PC地址
 *          参数2:WPTPND
 *          参数3:目标地址的值
 *          上述日志打印表示 参数1 该PC地址修改目标地址的值，并且返回修改后的值 参数3
 */
void watch_point_set(u32 addr);

/**
 * @brief 设置halt类型
 */
void halt_err_set(u32 halt_no);

/**
 * @brief 获取halt类型及相关信息 全局调用
 * 返回的u32类型的指针，指向u32 halt_err[16]数组
 * halt_err[0]:halt类型
 * 异常复位信息[1] - [6]如下
 * [1]、halt_err[0] 8001 CPU_ERR
 *      halt_err[1] = 1;该位为1时表示程序执行异常复位
 *      halt_err[2] 存储了复位的地方PC地址；
 *      halt_err[3] 存储异常原因，可以来区分复位原因，比如除0，取指错误等
 *      halt_err[4] 存储PCERR寄存器,可以理解为出事前的位置
 *      halt_err[5] 存储PCST寄存器，可以理解为出事前前的位置
 *
 * [2]、halt_err[0] 8001 CPU_ERR
 *      halt_err[1] = 2;该值为2时表示中断执行函数位于flash异常复位 CACHE 中断IRQ异常
 *      halt_err[2] 是否在中断中 1:是，0:否
 *      halt_err[3] 断点异常
 *      halt_err[4] 异常PC地址
 *      halt_err[5] miss地址 flash 访问地址
 *
 * [3]、halt_err[0] 8001 CPU_ERR
 *      halt_err[1] = 3;该值为3时表示关了全局中断后访问flash资源 CACHE 中断IRQ异常
 *      halt_err[2] 异常PC地址
 *      halt_err[2] miss地址 flash 访问地址
 *
 * [4]、halt_err[0] 8002 表示看门狗复位
 *      halt_err[1] 存储PCERR寄存器,可以理解为出事前的位置
 *      halt_err[2] 存储PCST寄存器，可以理解为出事前前的位置
 *
 * [5]、halt_err[0] 8101 CACHE CRC错误
 *      halt_err[1] 异常CRC校验值
 *      halt_err[2] 正确CRC校验值
 *      halt_err[3] cache地址
 *      halt_err[4] load代码地址
 *
 * [6]、halt_err[0] 8102 堆栈空间容量不足
 *      halt_err[0] 内存大小
 *      halt_err[1] 内存池标号
 *
 * [7]、halt_err[0] 为其他，请在halt头文件中查询
 */
u32* halt_err_debug_info_get(void);

#endif // _API_SYS_H_

