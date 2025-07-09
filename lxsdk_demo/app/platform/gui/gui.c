#include "include.h"
#include "ute_drv_screen_common.h"
#include "ute_module_log.h"
#include "ute_module_gui_common.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//推屏缓存大小(双份)
#define GUI_LINES_BUF_SIZE              (GUI_SCREEN_WIDTH * GUI_LINES_CNT * 2 * 2)

//以下缓存大小一般情况下不需要修改
#define GUI_ELE_BUF_SIZE                4096 + 2096                    //element缓存
#define GUI_WGT_BUF_SIZE                16384                    //widget缓存
#define GUI_MAXSIZE_TEMPBUF             0x7000                  //中间临时计算缓存14k
#define GUI_MAXSIZE_PARBUF              8096                    //PAR解码缓存
#define GUI_MAX_FONT_SIZE               128                     //单个字最大尺寸

static u8 gui_lines_buf[GUI_LINES_BUF_SIZE] AT(.disp.buf);      //推屏缓存(双份)
static u8 gui_temp_buf[GUI_MAXSIZE_TEMPBUF] AT(.disp.buf);      //中间计算缓存
static u8 gui_element_buf[GUI_ELE_BUF_SIZE] AT(.disp.buf);      //Element Buf
static u8 gui_widget_buf[GUI_WGT_BUF_SIZE] AT(.disp.buf);       //Widgets Buf

//GUI初始化配置表
static const gui_init_param_t tbl_gui_init_param =
{
    .screen_width = GUI_SCREEN_WIDTH,
    .screen_height = GUI_SCREEN_HEIGHT,
    .element_buf = gui_element_buf,
    .widget_buf = gui_widget_buf,
    .element_buf_size = GUI_ELE_BUF_SIZE,
    .widget_buf_size = GUI_WGT_BUF_SIZE,
    .temp_buf = gui_temp_buf,
    .temp_buf_size = GUI_MAXSIZE_TEMPBUF,
    .lines_buf = gui_lines_buf,
    .lines_buf_size = GUI_LINES_BUF_SIZE,
    .lines_count = GUI_LINES_CNT,
    .maxsize_parbuf = GUI_MAXSIZE_PARBUF,
    .font_res_addr = UI_BUF_FONT_SYS,
    .max_font_size = GUI_MAX_FONT_SIZE,
    .font_wspace = GUI_FONT_W_SPACE,
    .font_hspace = GUI_FONT_H_SPACE,
};

void hr_vdd_ldo_noPB2_on(void)
{
    PWRCON3 =(PWRCON3 & ~(0x1F<<13))|(28<<13);
    PWRCON3 |= BIT(12);
    PWRCON3 &= ~BIT(26);
    PWRCON3|=BIT(18);

    delay_us(330);
    PWRCON3 &= ~BIT(12);
}

//GUI相关初始化
void gui_init(void)
{
    power_gate_3v3_on();
    hr_vdd_ldo_noPB2_on();
    // hr_vdd_ldo_on();
#if(DEVELOPMENT_BOARD_TYPE)
    LCD_ON();
#endif
    ctp_init();
    tft_init();
#if FLASH_EXTERNAL_EN
    bsp_spi1flash_init();
#endif
    os_gui_init(&tbl_gui_init_param);
    compos_init();
    sys_cb.sleep_en = 1;            //允许进休眠
    sys_cb.gui_sleep_sta = 0;
    uteModuleGuiCommonDisplayOff(false);
}
extern bool slider_unlock;
void gui_sleep(void)
{
    if (!sys_cb.gui_sleep_sta)
    {
        os_gui_draw_w4_done();      //关tft前要等当前帧刷完
        tft_exit();
        ctp_exit();
        power_gate_3v3_off();
        hr_vdd_ldo_off();
#if(DEVELOPMENT_BOARD_TYPE)
        LCD_OFF();
#endif

        sys_cb.gui_sleep_sta = 1;
        tft_cb.tft_bglight_first_set = false;
        uteModuleGuiCommonDisplayOff(true);
        slider_unlock = false;
        printf("%s,slider_unlock=%d\n", __func__,slider_unlock);
    }
}

void gui_wakeup(void)
{
    if (sys_cb.gui_sleep_sta)
    {
        power_gate_3v3_on();
        hr_vdd_ldo_noPB2_on();
        // hr_vdd_ldo_on();
        // ecig_pwm_io_init();
#if(DEVELOPMENT_BOARD_TYPE)
        LCD_ON();
#endif
        //tft_init();
        ctp_init();
        tft_init();
        gui_widget_refresh();
        // uteDrvScreenCommonInit();
        sys_cb.gui_sleep_sta = 0;
        uteModuleGuiCommonDisplayOff(false);

        if (func_cb.sta != FUNC_SLIDING_UNLOCK_SCREEN &&func_cb.sta != FUNC_UP_WATCH_DIAL)
        {
            printf("gui_wakeup\n");
            func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
        }
    }
}

AT(.com_text.gui)
bool gui_get_auto_power_en(void)
{
    return GUI_AUTO_POWER_EN;
}

////是否打开字库打印
//bool unicode_show_info(void)
//{
//    return true;
//}

bool gui_font_get_align_top(void)
{
    return true;
}

bool qr_encode_use_malloc(void)
{
    return true;
}

//蓝屏
AT(.com_text.hwio)
void gui_halt(u32 halt_no)
{
    int i;
    tft_bglight_en();               //打开背光
    tft_frame_end();
    for (i=0; i<20000; i++)
    {
        asm("nop");                  //足够的延时，保证前面SPI推完
    }
    tft_frame_start();
    for (i=0; i<GUI_SCREEN_HEIGHT; i++)
    {
        de_fill_rgb565(gui_lines_buf, COLOR_BLUE, GUI_SCREEN_WIDTH);
        if (i >= GUI_SCREEN_CENTER_Y - 5 && i < GUI_SCREEN_CENTER_Y + 5)
        {
            de_fill_num(gui_lines_buf + GUI_SCREEN_CENTER_X * 2 - 96, halt_no, i - (GUI_SCREEN_CENTER_Y - 5));
        }
        tft_spi_send(gui_lines_buf, GUI_SCREEN_WIDTH * 2);
    }
    tft_frame_end();
}

#if 0 //UTE_LOG_GUI_LVL
//打印刷屏时长
AT(.com_text.gui)
bool gui_get_tick(void)
{
    return true;
}
#endif

// zzn add 20240401
void gui_clear_screen(uint16_t color,uint32_t size)
{
    uint32_t section = size/GUI_SCREEN_WIDTH;
    uint32_t lastPix = size%GUI_SCREEN_WIDTH;
    for(uint32_t i=0; i<section; i++)
    {
        de_fill_rgb565(gui_lines_buf, color, GUI_SCREEN_WIDTH);
        tft_spi_send(gui_lines_buf, GUI_SCREEN_WIDTH * 2);
    }
    if(lastPix!=0)
    {
        de_fill_rgb565(gui_lines_buf, color, lastPix);
        tft_spi_send(gui_lines_buf, lastPix * 2);
    }
}
// zzn add 20240401
