#include "sl_watch_application.h"
#include "ute_module_message.h"
#include "include.h"

#if  (SENSOR_STEP_SEL == SENSOR_STEP_SC7A20)

#if SL_Sensor_Algo_Release_Enable==0x00
#include "usart.h"
extern signed short          SL_DEBUG_DATA[10][128];
extern unsigned char         SL_DEBUG_DATA_LEN;
extern unsigned char         sl_sleep_sys_cnt;
#endif

#if SL_Sensor_Algo_Release_Enable==0x00
extern signed short          SL_ACCEL_DATA_Buf[3][32];
extern unsigned char         SL_FIFO_ACCEL_PEDO_NUM;
signed short                 hp_buf[32];
#endif


#define SL_SC7A20_SPI_IIC_MODE  1
/***SL_SC7A20_SPI_IIC_MODE==0  :SPI*******/
/***SL_SC7A20_SPI_IIC_MODE==1  :IIC*******/

#define SL_SC7A20_CALIBRATION_EN  0
/***SL_SC7A20_CALIBRATION_EN==0  :DISABLE******/
/***SL_SC7A20_CALIBRATION_EN==1  :ENABLE*******/

static signed char sl_init_status = 0x00;
unsigned char sl_acc_7bit_iic_addr =0x00;
/**Please modify the following parameters according to the actual situation**/
signed char SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_SWAY_INIT(void)
{
//    printf("%s\n", __func__);
#if SL_Sensor_Algo_Release_Enable==0x00
    unsigned char sl_version_value = 0x00;
#endif
//  bool sl_cal_xy_flag=0;
//  unsigned char sl_cal_fifo=0;
//  signed char sl_cal_value_xy[2]={0,0};
//  bool  fifo_status       =0;
    unsigned char  sl_person_para[4]    = {178, 60, 26, 1};
    unsigned char  Turn_Wrist_Para[4]= {1,3,1,1}; //refer pdf
    // unsigned char  sl_int_para[4]       = {1, 0, 2, 2};     //sl int config buf
    // SL_Watch_All_Para_Init();

    // if (sc7a20_info.user.custom == false) {               //默认值
    //    sc7a20_info.user.sex    = sl_person_para[3];
    //    sc7a20_info.user.age    = sl_person_para[2];
    //    sc7a20_info.user.weight = sl_person_para[1];
    //    sc7a20_info.user.height = sl_person_para[0];
    // } else {
    //    sl_person_para[3] = sc7a20_info.user.sex;
    //    sl_person_para[2] = sc7a20_info.user.age;
    //    sl_person_para[1] = sc7a20_info.user.weight;
    //    sl_person_para[0] = sc7a20_info.user.height;
    // }
    // sl_acc_7bit_iic_addr=SL_ACC_Find_I2C_Address();
#if SL_Sensor_Algo_Release_Enable==0x00
    USART_printf( USART1, "sl_acc_7bit_iic_addr=0x%x!\r\n",sl_acc_7bit_iic_addr);
//  sl_version_value==0x11  SC7A20
//  sl_version_value==0x26  SC7A20E
    SL_SC7A20_I2c_Spi_Read(SL_SC7A20_SPI_IIC_MODE, SL_SC7A20_VERSION_ADDR,1, &sl_version_value);
    USART_printf( USART1, "Version ID=0x%x!\r\n",sl_version_value);
    if(sl_version_value==SL_SC7A20H_VERSION_VALUE)
        USART_printf( USART1, "SC7A20H!\r\n");
    else if(sl_version_value==SL_SC7A20E_VERSION_VALUE)
        USART_printf( USART1, "SC7A20E!\r\n");
    else //SPI error
        USART_printf( USART1, "SC7A20!\r\n");
#endif

    /*****if use spi mode please config first******/
#if SL_SC7A20_SPI_IIC_MODE == 0//spi
    //init sensor
    sl_init_status=SL_SC7A20_Driver_Init(0,0x04,1);//spi or iic select
    //0x04-->close i2c pull up resister (must)
#else//i2c
    //init sensor
    sl_init_status=SL_SC7A20_Driver_Init(1,0x08,1);//spi or iic select
    //0x08-->close sdo pull up
#endif
    /**********sc7a20 fifo test***************/
    //fifo_status=SL_SC7A20_FIFO_TEST();

    /**********set pedo sensitivity***********/
    SL_PEDO_TH_SET(36,12,1,15,80);//36 12 0 30 50for motor
    /*which axis used to pedo depend on algo**/
    SL_PEDO_SET_AXIS(3);
    /**********set int para*******************/
    SL_PEDO_INT_SET(2,1,1);//0 or 1 sleep is different
    /**********set motion para****************/
    SL_Pedo_Person_Inf_Init(&sl_person_para[0]);//personal para init
    /**********set turn wrist para************/
    SL_Turn_Wrist_Init(&Turn_Wrist_Para[0]);
    /**********set sleep sensitivity**********/
    SL_Sleep_Para(120,10,1);
    /**********set free fall para*************/
    SL_SC7A20_FreeFall_Init(5,2,300);
    /**********set pinch recognition para*****/
    SL_Pinch_Recognition_Init(5,10,100,10,60);//SC7A20E(10,20,100,10,60),(5,10,40,20,60)
#if SL_SC7A20_CALIBRATION_EN==1
    sl_cal_xy_flag=SL_SC7A20_CAL_FUN(&sl_cal_fifo,&sl_cal_value_xy[0]);
#if SL_Sensor_Algo_Release_Enable==0x00
    USART_printf( USART1, "cal_flag=%d sl_cal_fifo=%d cal_x=%d cal_y=%d!\r\n",sl_cal_xy_flag,sl_cal_fifo,sl_cal_value_xy[0],sl_cal_value_xy[1]);
#endif
    SL_SC7A20_CAL_WRITE(sl_cal_value_xy[0],sl_cal_value_xy[1]);
#endif

    printf("sl_init_status ID = 0x%x\n",sl_init_status);
    return sl_init_status;
}
//extern void delay_nms(u16 time);
void sl_fifo_delay(void)
{
    /*Add the delay function of the project to meet the slow axis requirements of FIFO length*/
    /*FIFO LEN==12*/
    // delay_ms(1);
}

static unsigned int   SL_STEP                  = 0;
static unsigned int   SL_STEP_TEMP             = 0;
static unsigned char  SL_STEP_MOTION           = 0;
static unsigned int   SL_DISTANCE              = 0;
static unsigned int   SL_KCAL                  = 0;
static unsigned char  SL_CADENCE_STEP          = 0;//step per minutes
static unsigned short SL_CADENCE_AMP           = 0;//Equal scale scaling
static unsigned char  SL_CADENCE_DEGREE        = 0;//degree
static signed char    SL_WRIST_ENABLE          = 0;
static signed char    SL_WRIST                 = 0;
static signed char    SL_TWICE_WRIST           = 0;
static unsigned char  SL_SLEEP_ACTIVE          = 0;
//static unsigned char  SL_SLEEP_STATUS          = 0;
//static unsigned char  SL_SLEEP_ADOM            = 0;
static unsigned char  SL_SLEEP_CNT             = 0;
static bool           SL_CLOCK_STATUS          = 0;
static bool           SL_SWAY_STATUS           = 0;
static bool           SL_SWAY_ENABLE           = 0;
static unsigned int   step_init                = 0;
static unsigned int   cal_init                 = 0;
static unsigned int   distance_init            = 0;

static unsigned char  SL_DIRECTION_STATUS      = 0;
static bool           SL_FREEFALL_ENABLE       = 1;
static bool           SL_FREEFALL_RES          = 1;
static unsigned char  SL_PINCH_STATUS          = 0;
unsigned char         SL_PHONE_ROLLING         = 0;
unsigned char         SL_PHONE_ROLLING_TEMP    = 1;

static unsigned char  SL_INT_TIME_CNT          = 0;
#define SL_INT_TIME_TH   12  //per lsb is 0.5s
unsigned char SL_MCU_SLEEP_ALGO_FUNCTION(void);
unsigned int  SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_SWAY_ALGO(void)     //SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_INT_SWAY_ALGO
{
    // if (bsensor.step.int_en) {
    //     return -1;
    // }
#if SL_Sensor_Algo_Release_Enable==0x00
    unsigned char         sl_i;
#endif
    signed short          SC7A20_XYZ_Buf[3][32];
//  signed short          SC7A20_XYZ_Dat[3];
    unsigned char         SC7A20_FIFO_NUM = 0;
    bool                  SC7A20_INT_STATUS;
    signed short          SC7A20_PINCH_Buf[32];
    unsigned char         SC7A20_PINCH_Len;


    if((sl_init_status!=SL_SC7A20_VERSION_VALUE)&&(sl_init_status!=SL_SC7A20E_VERSION_VALUE)&&(sl_init_status!=SL_SC7A20H_VERSION_VALUE))
        return 0;//initial fail

    if(SL_PHONE_ROLLING==0)
    {
        if(SL_PHONE_ROLLING_TEMP==1) SC7A20_ODR_SET(1);
        SL_PHONE_ROLLING_TEMP=SL_PHONE_ROLLING;

        /*******read int status******/
        SC7A20_INT_STATUS=SL_INT_STATUS_READ();
        if((SC7A20_INT_STATUS==1)||(SL_STEP_TEMP!= SL_STEP))
        {
            SL_INT_TIME_CNT =0;
            SL_STEP_TEMP    =SL_STEP;
        }
        else
        {
            if(SL_INT_TIME_CNT<200)
            {
                SL_INT_TIME_CNT++;
            }
        }

//            signed short          SC7A20_XYZ_Buf_test[3][20];

        if(SL_INT_TIME_CNT<SL_INT_TIME_TH)//6s
        {
            /*******get sc7a20 FIFO data******/
            SC7A20_FIFO_NUM= SL_SC7A20_Read_FIFO();
            /*******get pedo value************/
            SL_STEP= SL_Watch_Kcal_Pedo_Algo(0);//不打开马达或音乐时调用该函数
            //SL_STEP= SL_Watch_Kcal_Pedo_Algo(1);//打开马达或音乐时调用该函数
#if SL_Sensor_Algo_Release_Enable==0x00
            USART_printf( USART1, "####SL_STEP=%d! LEN=%d\r\n",SL_STEP,SC7A20_FIFO_NUM);
            //      USART_printf( USART2, "####SL_STEP=%d!\r\n",SL_STEP);
#endif
            /*********get sleep active degree value*********/
            SL_SLEEP_ACTIVE      = SL_Sleep_Active_Degree(0);

            /*******get accel orginal data and length*******/
            SC7A20_FIFO_NUM      = SL_SC7A20_GET_FIFO_Buf(&SC7A20_XYZ_Buf[0][0],&SC7A20_XYZ_Buf[1][0],&SC7A20_XYZ_Buf[2][0],0);
            // printf("========>   xyz[%d]\n", SC7A20_FIFO_NUM);
            // for(u8 sl_i=0;sl_i<SC7A20_FIFO_NUM;sl_i++)
            // {
            //  printf("FIFO dat_x=%d dat_y=%d dat_z=%d!\r\n",SC7A20_XYZ_Buf[0][sl_i],SC7A20_XYZ_Buf[1][sl_i],SC7A20_XYZ_Buf[2][sl_i]);
            // }


#if SL_Sensor_Algo_Release_Enable==0x00
            //      for(sl_i=0;sl_i<SC7A20_FIFO_NUM;sl_i++)
            //      {
            //          USART_printf( USART1, "FIFO dat_x=%d dat_y=%d dat_z=%d!\r\n",SC7A20_XYZ_Buf[0][sl_i],SC7A20_XYZ_Buf[1][sl_i],SC7A20_XYZ_Buf[2][sl_i]);
            //      }
#endif

//   sc7a20_info.gsensor_axis.x = SC7A20_XYZ_Buf_test[0][0];
//   sc7a20_info.gsensor_axis.y = SC7A20_XYZ_Buf_test[1][0];
//   sc7a20_info.gsensor_axis.z = SC7A20_XYZ_Buf_test[2][0];

            /* SL_Pedo_GetMotion_Status */
            SL_STEP_MOTION       = SL_Pedo_GetMotion_Status();
            /* SL_Pedo_Step_Get_Distance */
            SL_DISTANCE          = SL_Pedo_Step_Get_Distance();
            /* SL_Pedo_Step_Get_KCal */
            SL_KCAL              = SL_Pedo_Step_Get_KCal();
            /*****average step per minutes****/
            SL_CADENCE_STEP      = SL_Pedo_Step_Get_Step_Per_Min();
            /*****average amp per step****/
            SL_CADENCE_AMP       = SL_Pedo_Step_Get_Avg_Amp();
            /*****motion degree****/
            SL_CADENCE_DEGREE    = SL_Pedo_Step_Get_Motion_Degree();
            // printf("step [%d, %d, %d]\n", SL_STEP, SL_KCAL, SL_DISTANCE);
            if ((SL_STEP != sc7a20_info.gsensor_data.step) || (SL_KCAL != sc7a20_info.gsensor_data.cal) || (SL_DISTANCE != sc7a20_info.gsensor_data.distance))
            {
                sc7a20_info.gsensor_data.step        = SL_STEP;
                sc7a20_info.gsensor_data.cal         = SL_KCAL;
                sc7a20_info.gsensor_data.distance    = SL_DISTANCE;
                printf("======= step [%d, %d, %d]\n", SL_STEP, SL_KCAL, SL_DISTANCE);
            }

//      if(SL_STEP>200)
            //      {
            /**reset pedo value**/
            //          SL_Pedo_Kcal_ResetStepCount();
            //      }

            /*******get wrist value******/
            SL_WRIST= SL_Watch_Wrist_Algo();
            // printf("SL_WRIST:%d\n", SL_WRIST);

            if(SL_WRIST == 1)
            {
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HNAD_SCREEN_ON_NOTIFY,0);
            }
            else if(SL_WRIST == 2)
            {
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HNAD_SCREEN_OFF_NOTIFY,0);
            }

            SL_TWICE_WRIST=SL_Turn_Wrist_Twice_Status();

            /*******get overturn value******/
            SL_CLOCK_STATUS=SL_Get_Clock_Status(1);//open overturn monitor
            if(SL_CLOCK_STATUS==1)//overturn success
            {
                SL_Get_Clock_Status(0);//close overturn monitor
            }

            /*******get sway value******/
            if(SL_SWAY_ENABLE==1)
            {
                /**this function will disable pedo function**/
                SL_SWAY_STATUS=SL_Get_Phone_Answer_Status(4,5);//get sway value
#if SL_Sensor_Algo_Release_Enable==0x00
                if(SL_SWAY_STATUS==1)
                {
                    USART_printf( USART1, "SL_SWAY_STATUS=%d!\r\n",SL_SWAY_STATUS);
                }
#endif
            }
            SL_DIRECTION_STATUS= SL_Get_Direction_Status();
            /*******get freefall value******/
            if(SL_FREEFALL_ENABLE==1)
            {
                SL_FREEFALL_RES= SL_SC7A20_Get_FreeFall_Status();
#if SL_Sensor_Algo_Release_Enable==0x00
                USART_printf( USART1, "free_res=%x\r\n",SL_FREEFALL_RES);
#endif
            }
        }
        else
        {
            //sc7a20 's data for heart rate algo
            //SC7A20_FIFO_NUM= SL_SC7A20_Read_FIFO();
            //SC7A20_FIFO_NUM= SL_SC7A20_GET_FIFO_Buf(&SC7A20_XYZ_Buf[0][0],&SC7A20_XYZ_Buf[1][0],&SC7A20_XYZ_Buf[2][0],0);
            SL_Pedo_WorkMode_Reset();
            //SL_Turn_Wrist_WorkMode_Reset();please not use this function
            SC7A20_FIFO_NUM=0;
        }

        /*******get sleep status value******/
        SL_SLEEP_CNT++;
#if SL_Sensor_Algo_Release_Enable==0x00
        if(SL_SLEEP_CNT>39)//1min
#else
        if(SL_SLEEP_CNT>119)//1min
#endif
        {
#if SL_Sensor_Algo_Release_Enable==0
            sl_sleep_sys_cnt++;
            if(sl_sleep_sys_cnt>59)//min
            {
                sl_sleep_sys_cnt=0;
                sl_sleep_sys_hour++;//hour
                if(sl_sleep_sys_hour==24)
                    sl_sleep_sys_hour=0;
            }
#endif
            SL_SLEEP_CNT=0;
            SL_MCU_SLEEP_ALGO_FUNCTION();
        }
    }
    else//SL_PHONE_ROLLING==1
    {
        SL_INT_TIME_CNT=0;
        if(SL_PHONE_ROLLING_TEMP==0) SC7A20_ODR_SET(2);
        SL_PHONE_ROLLING_TEMP=SL_PHONE_ROLLING;
        SL_SC7A20_All_FIFO_Read_SET();
        /*******get sc7a20 FIFO data******/
        SC7A20_FIFO_NUM= SL_SC7A20_Read_FIFO();
        /*******get accel orginal data and length*******/
        SC7A20_FIFO_NUM= SL_SC7A20_GET_FIFO_Buf(&SC7A20_XYZ_Buf[0][0],&SC7A20_XYZ_Buf[1][0],&SC7A20_XYZ_Buf[2][0],0);
#if 0
        for(sl_i=0; sl_i<SC7A20_FIFO_NUM; sl_i++)
        {
            USART_printf( USART1, "FIFO dat_x=%d dat_y=%d dat_z=%d!\r\n",SC7A20_XYZ_Buf[0][sl_i],SC7A20_XYZ_Buf[1][sl_i],SC7A20_XYZ_Buf[2][sl_i]);
        }
#endif
        /*******get wrist value******/
        SL_WRIST= SL_Watch_Wrist_Algo();
        SL_PINCH_STATUS=SL_Pinch_Recognition_Algo(&SC7A20_PINCH_Len,&SC7A20_PINCH_Buf[0]);
#if SL_Sensor_Algo_Release_Enable==0x00
        USART_printf( USART1, "SL_PINCH_STATUS=%d!\r\n",SL_PINCH_STATUS);
        for(sl_i=0; sl_i<SC7A20_PINCH_Len; sl_i++)
        {
            USART_printf( USART1, "SC7A20_PINCH_Len=%d data=%d!\r\n",SC7A20_PINCH_Len,SC7A20_PINCH_Buf[sl_i]);
        }
#endif
    }

//  SL_SC7A20_GET_DATA(&SC7A20_XYZ_Dat[0],&SC7A20_XYZ_Dat[1],&SC7A20_XYZ_Dat[2]);
#if SL_Sensor_Algo_Release_Enable==0x00
//  USART_printf( USART1, "NOW dat_x=%d dat_y=%d dat_z=%d!\r\n",SC7A20_XYZ_Dat[0],SC7A20_XYZ_Dat[1],SC7A20_XYZ_Dat[2]);
    SL_DEBUG_DATA_LEN=SC7A20_FIFO_NUM;
    for(sl_i=0; sl_i<SL_DEBUG_DATA_LEN; sl_i++)
    {
//      USART_printf( USART1, "SC7A20_FIFO_NUM=%d dat_x=%d dat_y=%d dat_z=%d!\r\n",SC7A20_FIFO_NUM,SC7A20_XYZ_Buf[0][sl_i],SC7A20_XYZ_Buf[1][sl_i],SC7A20_XYZ_Buf[2][sl_i]);

        SL_DEBUG_DATA[0][sl_i]=SC7A20_XYZ_Buf[0][sl_i];
        SL_DEBUG_DATA[1][sl_i]=SC7A20_XYZ_Buf[1][sl_i];
        SL_DEBUG_DATA[2][sl_i]=SC7A20_XYZ_Buf[2][sl_i];
        SL_DEBUG_DATA[3][sl_i]=SL_STEP;
        SL_DEBUG_DATA[4][sl_i]=SL_WRIST;
//        SL_DEBUG_DATA[4][sl_i]=SL_CLOCK_STATUS;
//      SL_DEBUG_DATA[5][sl_i]=SL_SLEEP_STATUS;
        SL_DEBUG_DATA[5][sl_i]=SL_CLOCK_STATUS;//SL_SWAY_STATUS SL_CLOCK_STATUS SL_DIRECTION_STATUS
        SL_DEBUG_DATA[6][sl_i]=SL_STEP_MOTION;//SL_TWICE_WRIST SL_PINCH_STATUS
        SL_DEBUG_DATA[7][sl_i]=SL_DISTANCE;
//      SL_DEBUG_DATA[7][sl_i]=hp_buf[sl_i];
        SL_DEBUG_DATA[8][sl_i]=SL_KCAL;
        SL_DEBUG_DATA[9][sl_i]=SL_CADENCE_STEP;
//        SL_DEBUG_DATA[8][sl_i]=SL_CADENCE_AMP;
//        SL_DEBUG_DATA[9][sl_i]=SL_CADENCE_DEGREE;
    }
#endif

    (void)SC7A20_FIFO_NUM;

    return SL_STEP;
}

#if SL_Sensor_Algo_Release_Enable==0x00
unsigned short sl_sleep_counter = 0;
#endif

#if 1           //睡眠相关

static unsigned char  SL_SLEEP_STATUS          = 0;
static unsigned char  SL_SLEEP_ADOM            = 0;

#define SL_SLEEP_DEEP_TH   6
#define SL_SLEEP_LIGHT_TH  3


static unsigned char sl_sleep_sys_hour;
/***Call this function regularly for 1 minute***/
//AT(.text.lowpwr.sleep)
unsigned char SL_MCU_SLEEP_ALGO_FUNCTION(void)
{

    /*******get sleep status value******/
    // SL_SLEEP_STATUS = SL_Sleep_GetStatus(sl_sleep_sys_hour);
    SL_SLEEP_STATUS = SL_Sleep_GetStatus(compo_cb.tm.hour);
    SL_SLEEP_ACTIVE = SL_Sleep_Get_Active_Degree();
    SL_SLEEP_ADOM  = SL_Adom_GetStatus();

#if SL_Sensor_Algo_Release_Enable==0x00
    sl_sleep_counter++;
    USART_printf( USART1, "step=%d! sys_time=%d!\r\n",SL_STEP,sl_sleep_sys_hour);
    USART_printf( USART1, "T=%d,sleep_status:%d,sleep_adom:%d!\r\n",sl_sleep_counter,SL_SLEEP_STATUS,SL_SLEEP_ADOM);
    USART_printf( USART1, "SL_SLEEP_ACTIVE:%d!\r\n",SL_SLEEP_ACTIVE);
#endif
//    static u16 cnt = 0;
//     printf("cnt[%d] SL_SLEEP_STATUS[%d] SL_SLEEP_ACTIVE[%d] SL_SLEEP_ADOM[%d] sys_hour[%d]\n",
//           cnt, SL_SLEEP_STATUS, SL_SLEEP_ACTIVE, SL_SLEEP_ADOM, compo_cb.tm.hour);
//    cnt++;

    if (SL_SLEEP_STATUS < SL_SLEEP_LIGHT_TH)
    {
        return 0;//0 1 2 3
    }
    else if (SL_SLEEP_STATUS < SL_SLEEP_DEEP_TH)
    {
        return 1;//4 5 6
    }
    else
    {
        return 2;//7
    }
}

//AT(.text.lowpwr.sleep)
void sl_update_sleep_sys_hour(unsigned char hour)
{
    sl_sleep_sys_hour = hour;
}

#endif

void sl_var_set(void)
{
    step_init       = sc7a20_info.gsensor_data.step;
    cal_init        = sc7a20_info.gsensor_data.cal;
    distance_init   = sc7a20_info.gsensor_data.distance;
}

/**** clear the current sl data ****/
//AT(.text.lowpwr.sleep)
void sl_var_clr(void)
{
    // printf("%s\n", __func__);
    sc7a20_info.gsensor_data.step            = 0;
    sc7a20_info.gsensor_data.cal             = 0;
    sc7a20_info.gsensor_data.distance        = 0;
    sc7a20_info.gsensor_data.yest_day        = compo_cb.tm.day;

    sl_var_set();
    SL_Pedo_Kcal_ResetStepCount();
}

void sl_var_init(void)
{
    memset(&sc7a20_info, 0, sizeof(sc7a20_info_t));

    if ((sc7a20_info.gsensor_data.step == -1 && sc7a20_info.gsensor_data.cal == 0xffff) ||
        (sc7a20_info.gsensor_data.yest_day != compo_cb.tm.day))
    {
        sl_var_clr();
    }

    sl_var_set();
}

///**** 根据日期刷新当前数据 ****/
//AT(.text.lowpwr.sleep)
void sl_data_update(void)
{
    //隔天更新一次
    if (compo_cb.tm.hour == 0 && sc7a20_info.gsensor_data.yest_day != compo_cb.tm.day)
    {
        sl_var_clr();
    }
}

void gsensor_use_info_set(uint8_t sex, uint8_t age, uint8_t weight, uint8_t height, bool custom)
{
    sc7a20_info.user.custom = custom;
    if (custom)
    {
        sc7a20_info.user.sex    = sex;
        sc7a20_info.user.age    = age;
        sc7a20_info.user.weight = weight;
        sc7a20_info.user.height = height;
    }
    else
    {

    }
}

//抬手亮屏功能状态设置
void sc7a20_wrist_set(char wrist)
{
    SL_WRIST_ENABLE = wrist;
}

//抬手亮屏功能状态获取
char sc7a20_wrist_get(void)
{
    return SL_WRIST_ENABLE;
}

//抬手亮屏是否触发
char sc7a20_wkup_get(void)
{
    return SL_WRIST;
}

//注意启用摇晃功能会无计步，使用完需关闭
void sc7a20_sway_set(bool sway)
{
    SL_SWAY_ENABLE = sway;
}

//摇晃功能状态
bool sc7a20_sway_get(void)
{
    return SL_SWAY_ENABLE;
}

//是否摇晃
bool sc7a20_sway_status_get(void)
{
    return SL_SWAY_STATUS;
}

//当前步数
UINT sc7a20_cur_steps_get(void)
{
    return SL_STEP;
}

//当前卡路里
UINT sc7a20_cur_kcal_get(void)
{
    return SL_KCAL;
}

//当前距离
UINT sc7a20_cur_distance_get(void)
{
    return SL_DISTANCE;
}

#endif //SENSOR_STEP_SEL





// #include "sl_watch_application.h"
// #include "include.h"

// #if  (SENSOR_STEP_SEL == SENSOR_STEP_SC7A20)

// #if SL_Sensor_Algo_Release_Enable==0x00
// #include "usart.h"
// extern signed short          SL_DEBUG_DATA[10][128];
// extern unsigned char         SL_DEBUG_DATA_LEN;
// #endif

// #if SL_Sensor_Algo_Release_Enable==0x00
// extern signed short          SL_ACCEL_DATA_Buf[3][32];
// extern unsigned char         SL_FIFO_ACCEL_PEDO_NUM;
// signed short                 hp_buf[32];
// #endif


// #define SL_SC7A20_SPI_IIC_MODE  1
// /***SL_SC7A20_SPI_IIC_MODE==0  :SPI*******/
// /***SL_SC7A20_SPI_IIC_MODE==1  :IIC*******/

// static signed char sl_init_status = 0x00;
// // unsigned char sl_acc_7bit_iic_addr =0x00;
// /**Please modify the following parameters according to the actual situation**/
// signed char SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_SWAY_INIT(void)
// {
// //    printf("%s\n", __func__);
// #if SL_Sensor_Algo_Release_Enable==0x00
//     unsigned char sl_version_value = 0x00;
// #endif
// //  bool  fifo_status       =0;
//     unsigned char  sl_person_para[4]    = {178, 60, 26, 1};
//     unsigned char  Turn_Wrist_Para[3]   = {1, 2, 1};        //wrist config buf
//     unsigned char  sl_int_para[4]       = {1, 0, 2, 2};     //sl int config buf

//     if (sc7a20_info.user.custom == false) {               //默认值
//        sc7a20_info.user.sex    = sl_person_para[3];
//        sc7a20_info.user.age    = sl_person_para[2];
//        sc7a20_info.user.weight = sl_person_para[1];
//        sc7a20_info.user.height = sl_person_para[0];
//     } else {
//        sl_person_para[3] = sc7a20_info.user.sex;
//        sl_person_para[2] = sc7a20_info.user.age;
//        sl_person_para[1] = sc7a20_info.user.weight;
//        sl_person_para[0] = sc7a20_info.user.height;
//     }
//     // sl_acc_7bit_iic_addr=SL_ACC_Find_I2C_Address();
// #if SL_Sensor_Algo_Release_Enable==0x00
//  USART_printf( USART1, "sl_acc_7bit_iic_addr=0x%x!\r\n",sl_acc_7bit_iic_addr);
// //   sl_version_value==0x11  SC7A20
// //   sl_version_value==0x26  SC7A20E
//  SL_SC7A20_I2c_Spi_Read(SL_SC7A20_SPI_IIC_MODE, SL_SC7A20_VERSION_ADDR,1, &sl_version_value);
//      USART_printf( USART1, "Version ID=0x%x!\r\n",sl_version_value);
//  if(sl_version_value==SL_SC7A20H_VERSION_VALUE)
//      USART_printf( USART1, "SC7A20H!\r\n");
//  else if(sl_version_value==SL_SC7A20E_VERSION_VALUE)
//      USART_printf( USART1, "SC7A20E!\r\n");
//  else //SPI error
//      USART_printf( USART1, "SC7A20!\r\n");
// #endif

//     /*****if use spi mode please config first******/
// #if SL_SC7A20_SPI_IIC_MODE == 0//spi
//  //init sensor
//  sl_init_status=SL_SC7A20_Driver_Init(0,0x04,1);//spi or iic select
//  //0x04-->close i2c pull up resister (must)
// #else//i2c
//  //init sensor
//      sl_init_status=SL_SC7A20_Driver_Init(1,0x08,1);//spi or iic select
//  //0x08-->close sdo pull up
// #endif
//     /**********sc7a20 fifo test***************/
//     //fifo_status=SL_SC7A20_FIFO_TEST();

//     /**********set pedo sensitivity***********/
//     SL_PEDO_TH_SET(30, 10, 1, 30, 50); //36 12 0 30 50for motor
//     /*which axis used to pedo depend on algo**/
//     SL_PEDO_SET_AXIS(3);
//     /**********set int para*******************/
//  // SL_PEDO_INT_SET(2,1,0);//0 or 1 sleep is different
//     /**********set motion para****************/
//     SL_Pedo_Person_Inf_Init(&sl_person_para[0]);//personal para init
//     /**********set turn wrist para************/
//     SL_Turn_Wrist_Init(&Turn_Wrist_Para[0]);
//     /**********set sleep sensitivity**********/
//     SL_Sleep_Para(SLEEP_WEAR_CHECK_CNT, 10, 1);

// #if SL_SC7A20_CALIBRATION_EN==1
//  sl_cal_xy_flag=SL_SC7A20_CAL_FUN(&sl_cal_fifo,&sl_cal_value_xy[0]);
// #if SL_Sensor_Algo_Release_Enable==0x00
//  USART_printf( USART1, "cal_flag=%d sl_cal_fifo=%d cal_x=%d cal_y=%d!\r\n",sl_cal_xy_flag,sl_cal_fifo,sl_cal_value_xy[0],sl_cal_value_xy[1]);
// #endif
//  SL_SC7A20_CAL_WRITE(sl_cal_value_xy[0],sl_cal_value_xy[1]);
// #endif

//     printf("sl_init_status ID = 0x%x\n",sl_init_status);
//     return sl_init_status;
// }

// static unsigned int   SL_STEP                  = 0;
// static unsigned int   SL_STEP_TEMP             = 0;
// static unsigned char  SL_STEP_MOTION           = 0;
// static unsigned int   SL_DISTANCE              = 0;
// static unsigned int   SL_KCAL                  = 0;
// static unsigned char  SL_CADENCE_STEP          = 0;//step per minutes
// static unsigned short SL_CADENCE_AMP           = 0;//Equal scale scaling
// static unsigned char  SL_CADENCE_DEGREE        = 0;//degree
// static signed char    SL_WRIST_ENABLE          = 0;
// static signed char    SL_WRIST                 = 0;
// static unsigned char  SL_INT_CNT               = 0;//counter
// static unsigned char  SL_SLEEP_ACTIVE          = 0;
// static bool           SL_CLOCK_STATUS          = 0;
// static bool           SL_SWAY_STATUS           = 0;
// static bool           SL_SWAY_ENABLE           = 0;
// static unsigned int   step_init                = 0;
// static unsigned int   cal_init                 = 0;
// static unsigned int   distance_init            = 0;
// // static unsigned char  SL_DIRECTION_STATUS      = 0;

// //AT(.text.lowpwr.sleep)
// signed int  SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_SWAY_ALGO(void)     //SL_SC7A20_PEDO_KCAL_WRIST_SLEEP_INT_SWAY_ALGO
// {
//     // if (bsensor.step.int_en) {
//     //     return -1;
//     // }
// #if SL_Sensor_Algo_Release_Enable==0x00
//     unsigned char         sl_i;
//     signed short          SC7A20_XYZ_Buf[3][14];
// #endif
//     // unsigned char         SC7A20_FIFO_NUM;
//     bool                  SL_ACTION;

//     signed short          SC7A20_XYZ_Buf_test[3][20];

//  // if((sl_init_status!=SL_SC7A20_VERSION_VALUE)&&(sl_init_status!=SL_SC7A20E_VERSION_VALUE)&&(sl_init_status!=SL_SC7A20H_VERSION_VALUE))
//      // return 0;//initial fail

//     /*******get sc7a20 FIFO data******/
//     SL_SC7A20_Read_FIFO();
// //   SC7A20_FIFO_NUM= SL_SC7A20_Read_FIFO();
// //    printf("SC7A20_FIFO_NUM = %d\n",SC7A20_FIFO_NUM);

//     /*******get pedo value************/
//     SL_STEP = SL_Watch_Kcal_Pedo_Algo(0) + step_init; //不打开马达或音乐时调用该函数
//     //SL_STEP= SL_Watch_Kcal_Pedo_Algo(1);//打开马达或音乐时调用该函数

//     /*********get sleep active degree value*********/
//     SL_SLEEP_ACTIVE      = SL_Sleep_Active_Degree(0);

// #if SL_Sensor_Algo_Release_Enable==0x00
//     /*******get accel orginal data and length*******/
//     SC7A20_FIFO_NUM      = SL_SC7A20_GET_FIFO_Buf(&SC7A20_XYZ_Buf[0][0], &SC7A20_XYZ_Buf[1][0], &SC7A20_XYZ_Buf[2][0], 0);
// #endif
//     // SC7A20_FIFO_NUM      = SL_SC7A20_GET_FIFO_Buf(&SC7A20_XYZ_Buf_test[0][0], &SC7A20_XYZ_Buf_test[1][0], &SC7A20_XYZ_Buf_test[2][0], 0);
//     SL_SC7A20_GET_FIFO_Buf(&SC7A20_XYZ_Buf_test[0][0], &SC7A20_XYZ_Buf_test[1][0], &SC7A20_XYZ_Buf_test[2][0], 0);
//     printf("========>   xyz [%d, %d, %d]\n", SC7A20_XYZ_Buf_test[0][0], SC7A20_XYZ_Buf_test[1][0], SC7A20_XYZ_Buf_test[2][0]);
// #if 0
//  for(sl_i=0;sl_i<SC7A20_FIFO_NUM;sl_i++)
//  {
//      USART_printf( USART1, "FIFO dat_x=%d dat_y=%d dat_z=%d!\r\n",SC7A20_XYZ_Buf[0][sl_i],SC7A20_XYZ_Buf[1][sl_i],SC7A20_XYZ_Buf[2][sl_i]);
//  }
// #endif

//    sc7a20_info.gsensor_axis.x = SC7A20_XYZ_Buf_test[0][0];
//    sc7a20_info.gsensor_axis.y = SC7A20_XYZ_Buf_test[1][0];
//    sc7a20_info.gsensor_axis.z = SC7A20_XYZ_Buf_test[2][0];

//     /* SL_Pedo_GetMotion_Status */
//     SL_STEP_MOTION       = SL_Pedo_GetMotion_Status();
//     /* SL_Pedo_Step_Get_Distance */
//     SL_DISTANCE          = SL_Pedo_Step_Get_Distance() / 10 + distance_init;
//     /* SL_Pedo_Step_Get_KCal */
//     SL_KCAL              = SL_Pedo_Step_Get_KCal() + cal_init;
//     /*****average step per minutes****/
//     SL_CADENCE_STEP      = SL_Pedo_Step_Get_Step_Per_Min();
//     /*****average amp per step****/
//     SL_CADENCE_AMP       = SL_Pedo_Step_Get_Avg_Amp();
//     /*****motion degree****/
//     SL_CADENCE_DEGREE    = SL_Pedo_Step_Get_Motion_Degree();
//     printf("step [%d, %d, %d]\n", SL_STEP, SL_KCAL, SL_DISTANCE);
//     if ((SL_STEP != sc7a20_info.gsensor_data.step) || (SL_KCAL != sc7a20_info.gsensor_data.cal) || (SL_DISTANCE != sc7a20_info.gsensor_data.distance)) {
//         sc7a20_info.gsensor_data.step        = SL_STEP;
//         sc7a20_info.gsensor_data.cal         = SL_KCAL;
//         sc7a20_info.gsensor_data.distance    = SL_DISTANCE;
//         printf("======= step [%d, %d, %d]\n", SL_STEP, SL_KCAL, SL_DISTANCE);
//     }

//     // if (SL_STEP>200) {
//     // /**reset pedo value**/
//     //     SL_Pedo_Kcal_ResetStepCount();
//     // }

//     /*******get wrist value******/
//     if (SL_WRIST_ENABLE) {
//         SL_WRIST = SL_Watch_Wrist_Algo();
//         if (SL_WRIST != 0) {
//             printf("SL_WRIST[%d]\n", SL_WRIST);
//         }
//     }
//     // SL_ACTION = SL_SC7A20_Action_Degree();

//     /*******get int config******/
//     if ((SL_STEP_TEMP == SL_STEP) && (SL_ACTION == 0)) { //step no change
//         SL_INT_CNT++;
//         if (SL_INT_CNT > 10) { //1-->0.5s  10--5s
//             SL_INT_CNT  = 0;
//             //close timer of  0.5s
//             // SL_Close_Sc7a20_Timer();
//             //if 5s no step, open sensor int function
//             //printf("open_sc7a20_int\n");
//             sc7a20_int_enable();
//             //open mcu int function-->
//             //mcu in to sleep status
//             SL_Pedo_WorkMode_Reset();
//             //SL_Turn_Wrist_WorkMode_Reset();//please not use this function
//         }
//     } else { //step changed
//         SL_STEP_TEMP = SL_STEP;
//         SL_INT_CNT  = 0;
//     }
//     /*******get overturn value******/
//     SL_CLOCK_STATUS = SL_Get_Clock_Status(1); //open overturn monitor
//     if (SL_CLOCK_STATUS == 1) { //overturn success
//         SL_Get_Clock_Status(0);//close overturn monitor
//     }

//     /*******get sway value******/
//     if (SL_SWAY_ENABLE == 1) {
//         /**this function will disable pedo function**/
//         SL_SWAY_STATUS = SL_Get_Phone_Answer_Status(2, 3); //get sway value
//         printf("SL_SWAY_STATUS[%d]\n", SL_SWAY_STATUS);
//     }

//     // SL_DIRECTION_STATUS= SL_Get_Direction_Status();

//     // SL_ACTION = SL_SC7A20_Action_Degree();    //运动等级
//     //printf("Action_Degree = %d\n",SL_ACTION);


// #if SL_Sensor_Algo_Release_Enable==0x00
//     SL_DEBUG_DATA_LEN = SC7A20_FIFO_NUM;
//     for (sl_i = 0; sl_i < SL_DEBUG_DATA_LEN; sl_i++) {
//         SL_DEBUG_DATA[0][sl_i] = SC7A20_XYZ_Buf[0][sl_i];
//         SL_DEBUG_DATA[1][sl_i] = SC7A20_XYZ_Buf[1][sl_i];
//         SL_DEBUG_DATA[2][sl_i] = SC7A20_XYZ_Buf[2][sl_i];
//         SL_DEBUG_DATA[3][sl_i] = SL_STEP;
//         SL_DEBUG_DATA[4][sl_i] = SL_WRIST;
// //        SL_DEBUG_DATA[4][sl_i]=SL_CLOCK_STATUS;
//         SL_DEBUG_DATA[5][sl_i] = SL_SLEEP_STATUS;
// //        SL_DEBUG_DATA[5][sl_i]=SL_SWAY_STATUS;
//         SL_DEBUG_DATA[6][sl_i] = SL_STEP_MOTION;
// //      SL_DEBUG_DATA[7][sl_i]=SL_DISTANCE;
//         SL_DEBUG_DATA[7][sl_i] = hp_buf[sl_i];
//         SL_DEBUG_DATA[8][sl_i] = SL_KCAL;
//         SL_DEBUG_DATA[9][sl_i] = SL_CADENCE_STEP;
// //        SL_DEBUG_DATA[8][sl_i]=SL_CADENCE_AMP;
// //        SL_DEBUG_DATA[9][sl_i]=SL_CADENCE_DEGREE;
//     }
// #endif
//     return SL_STEP;
// }

// #if SL_Sensor_Algo_Release_Enable==0x00
// unsigned short sl_sleep_counter = 0;
// #endif

// #if 1           //睡眠相关

// static unsigned char  SL_SLEEP_STATUS          = 0;
// static unsigned char  SL_SLEEP_ADOM            = 0;

// #define SL_SLEEP_DEEP_TH   6
// #define SL_SLEEP_LIGHT_TH  3


// static unsigned char sl_sleep_sys_hour;
// /***Call this function regularly for 1 minute***/
// //AT(.text.lowpwr.sleep)
// unsigned char SL_MCU_SLEEP_ALGO_FUNCTION(void)
// {

//     /*******get sleep status value******/
//     // SL_SLEEP_STATUS = SL_Sleep_GetStatus(sl_sleep_sys_hour);
//     SL_SLEEP_STATUS = SL_Sleep_GetStatus(compo_cb.tm.hour);
//     SL_SLEEP_ACTIVE = SL_Sleep_Get_Active_Degree();
//     SL_SLEEP_ADOM  = SL_Adom_GetStatus();

// #if SL_Sensor_Algo_Release_Enable==0x00
//  sl_sleep_counter++;
//  USART_printf( USART1, "step=%d! sys_time=%d!\r\n",SL_STEP,sl_sleep_sys_hour);
//  USART_printf( USART1, "T=%d,sleep_status:%d,sleep_adom:%d!\r\n",sl_sleep_counter,SL_SLEEP_STATUS,SL_SLEEP_ADOM);
//  USART_printf( USART1, "SL_SLEEP_ACTIVE:%d!\r\n",SL_SLEEP_ACTIVE);
// #endif
//     static u16 cnt = 0;
//     printf("cnt[%d] SL_SLEEP_STATUS[%d] SL_SLEEP_ACTIVE[%d] SL_SLEEP_ADOM[%d] sys_hour[%d]\n",
//            cnt, SL_SLEEP_STATUS, SL_SLEEP_ACTIVE, SL_SLEEP_ADOM, compo_cb.tm.hour);
//     cnt++;

//     if (SL_SLEEP_STATUS < SL_SLEEP_LIGHT_TH) {
//         return 0;//0 1 2 3
//     } else if (SL_SLEEP_STATUS < SL_SLEEP_DEEP_TH) {
//         return 1;//4 5 6
//     } else {
//         return 2;//7
//     }
// }

// //AT(.text.lowpwr.sleep)
// void sl_update_sleep_sys_hour(unsigned char hour)
// {
//     sl_sleep_sys_hour = hour;
// }

// #endif

// void sl_var_set(void)
// {
//     step_init       = sc7a20_info.gsensor_data.step;
//     cal_init        = sc7a20_info.gsensor_data.cal;
//     distance_init   = sc7a20_info.gsensor_data.distance;
// }

// /**** clear the current sl data ****/
// //AT(.text.lowpwr.sleep)
// void sl_var_clr(void)
// {
//     printf("%s\n", __func__);
//     sc7a20_info.gsensor_data.step            = 0;
//     sc7a20_info.gsensor_data.cal             = 0;
//     sc7a20_info.gsensor_data.distance        = 0;
//     sc7a20_info.gsensor_data.yest_day        = compo_cb.tm.day;

//     sl_var_set();
//     SL_Pedo_Kcal_ResetStepCount();
// }

// void sl_var_init(void)
// {
//     memset(&sc7a20_info, 0, sizeof(sc7a20_info_t));

//     if ((sc7a20_info.gsensor_data.step == -1 && sc7a20_info.gsensor_data.cal == 0xffff) ||
//         (sc7a20_info.gsensor_data.yest_day != compo_cb.tm.day)) {
//         sl_var_clr();
//     }

//     sl_var_set();
// }

// ///**** 根据日期刷新当前数据 ****/
// //AT(.text.lowpwr.sleep)
// void sl_data_update(void)
// {
//     //隔天更新一次
//     if (compo_cb.tm.hour == 0 && sc7a20_info.gsensor_data.yest_day != compo_cb.tm.day) {
//         sl_var_clr();
//     }
// }

// void sl_fifo_delay(void)
// {
//  /*Add the delay function of the project to meet the slow axis requirements of FIFO length*/
//  /*FIFO LEN==12*/
// //   delay_nms(300);
// }

// void gsensor_use_info_set(uint8_t sex, uint8_t age, uint8_t weight, uint8_t height, bool custom)
// {
//     sc7a20_info.user.custom = custom;
//     if (custom) {
//         sc7a20_info.user.sex    = sex;
//         sc7a20_info.user.age    = age;
//         sc7a20_info.user.weight = weight;
//         sc7a20_info.user.height = height;
//     } else {

//     }
// }

// //抬手亮屏功能状态设置
// void sc7a20_wrist_set(char wrist)
// {
//     SL_WRIST_ENABLE = wrist;
// }

// //抬手亮屏功能状态获取
// char sc7a20_wrist_get(void)
// {
//     return SL_WRIST_ENABLE;
// }

// //抬手亮屏是否触发
// char sc7a20_wkup_get(void)
// {
//     return SL_WRIST;
// }

// //注意启用摇晃功能会无计步，使用完需关闭
// void sc7a20_sway_set(bool sway)
// {
//     SL_SWAY_ENABLE = sway;
// }

// //摇晃功能状态
// bool sc7a20_sway_get(void)
// {
//     return SL_SWAY_ENABLE;
// }

// //是否摇晃
// bool sc7a20_sway_status_get(void)
// {
//     return SL_SWAY_STATUS;
// }

// //当前步数
// UINT sc7a20_cur_steps_get(void)
// {
//     return SL_STEP;
// }

// //当前卡路里
// UINT sc7a20_cur_kcal_get(void)
// {
//     return SL_KCAL;
// }

// //当前距离
// UINT sc7a20_cur_distance_get(void)
// {
//     return SL_DISTANCE;
// }

// #endif //SENSOR_STEP_SEL


