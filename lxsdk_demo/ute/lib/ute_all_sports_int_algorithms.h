/**
 ****************************************************************************************
 *
 * @file ute_step_int_algorithms.h
 *
 * @brief Application utility functions header file.
 *
 * Copyright (C) 2014-11-03
 *
 ****************************************************************************************
 */

#ifndef _UTE_STEP_INT_ALGORITHMS_H_
#define _UTE_STEP_INT_ALGORITHMS_H_
#include "ute_project_config.h"
#include "ute_step_int_algorithms_common.h"
#define UTE_ALL_SPORTS_INT_ALGORITHMS_SUPPORT 1 //需要在项目中打开这个宏定义
#if (UTE_ALL_SPORTS_INT_ALGORITHMS_SUPPORT)

/******
**24种运动模式类型
*   1)开启运动模式以后，算法mUTESportDatas不会产生步数的运动：仰卧起坐，开合跳，划船，游泳，动感单车，骑行，跳绳；
* 2)算法mUTESportDatas会有跑步和走路的数据：walk，running，treadmill，climbing；
* 3)其他的运动，默认会在mStepSleepParam普通的计步算法中产生计步数据，可以根据客户需求获取或者去掉这个部分产生的数据
*******/

typedef enum
{
    SPORT_TYPE_NONE =0X00,
    SPORT_TYPE_RUNNING =0x01, //跑步
    SPORT_TYPE_RIDE_BIKE =0X02, // 骑行
    SPORT_TYPE_JUMP_ROPE =0X03,//跳绳
    SPORT_TYPE_SWIMMING =0X04, // 游泳
    SPORT_TYPE_BADMINTON =0X05, // 羽毛球
    SPORT_TYPE_TABLE_TENNIS=0X06, //乒乓球
    SPORT_TYPE_TENNIS =0X07, //网球
    SPORT_TYPE_CLIMBING =0X08, //爬山
    SPORT_TYPE_WALKING =0X09, //徒步
    SPORT_TYPE_BASKETBALL =0X0A, //篮球
    SPORT_TYPE_FOOTBALL =0X0B, //足球
    SPORT_TYPE_BASEBALL =0X0C, //棒球
    SPORT_TYPE_VOLLEYBALL =0X0D,//排球
    SPORT_TYPE_CRICKET = 0X0E, //板球
    SPORT_TYPE_RUGBY = 0X0F,  //橄榄球
    SPORT_TYPE_HOCKEY =0X10,// 曲棍球
    SPORT_TYPE_DANCE = 0X11, //跳舞
    SPORT_TYPE_SPINNING = 0X12, //动感单车
    SPORT_TYPE_YOGA = 0X13, //瑜伽
    SPORT_TYPE_SIT_UP =0X14, //仰卧起坐
    SPORT_TYPE_TREADMILL =0X15, // 跑步机
    SPORT_TYPE_GYMNASTICS =0X16, //体操
    SPORT_TYPE_BOATING = 0X17, // 划船
    SPORT_TYPE_JUMPING_JACK = 0X18,//开合跳
    SPORT_TYPE_FREE_TRAINING = 0X19,//自由训练20191231
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    SPORT_TYPE_INDOOR_WALK         =0x1A, //室内走路
    SPORT_TYPE_INDOOR_RUN          =0x1B, //室内跑步
    SPORT_TYPE_STRENGTH_TRAINING   =0x1C, //力量训练
    SPORT_TYPE_STEP_TRAINING       =0x1D, //踏步
    SPORT_TYPE_HORSE_RIDING        =0x1E, //骑马
    SPORT_TYPE_ELLIPTICAL_TRAINER  =0x1F, //椭圆机
    SPORT_TYPE_TAI_CHI             =0x20, //太极
    SPORT_TYPE_SHUTTLECOCK         =0x21, //毽球
    SPORT_TYPE_BOXING              =0x22, //拳击
    SPORT_TYPE_OUTDOOR_WALK        =0x23, //户外健走
    SPORT_TYPE_TRAIL_RUNNING       =0x24, //越野跑
    SPORT_TYPE_SKIING              =0x25, //滑雪
    SPORT_TYPE_ICE_HOCKEY          =0x26, //冰球
    SPORT_TYPE_TAEKWONDO           =0x27, //跆拳道
    SPORT_TYPE_VO2MAX_TEST         =0x28, //最大摄氧量测试
    SPORT_TYPE_ROWING_MACHING      =0x29, //划船机
    SPORT_TYPE_AIR_WALKER          =0x2A, //漫步机
    SPORT_TYPE_HIKING              =0x2B, //徒步
    SPORT_TYPE_ATHLETICS           =0x2C, //田径
    SPORT_TYPE_WAIST_TRAINING      =0x2D, //腰腹训练
    SPORT_TYPE_KARATE              =0x2E, //空手道
    SPORT_TYPE_COOLDOWN            =0x2F, //整理放松
    SPORT_TYPE_CROSS_TRAINING      =0x30, //交叉训练
    SPORT_TYPE_PILATES             =0x31, //普拉提
    SPORT_TYPE_CROSS_FIT           =0x32, //交叉配合
    SPORT_TYPE_FUNCTIONAL_TRAINING =0x33, //功能性训练
    SPORT_TYPE_PHYSICAL_TRAINING   =0x34, //体能训练
    SPORT_TYPE_ARCHERY             =0x35, //射箭
    SPORT_TYPE_FLEXIBILITY         =0x36, //柔韧度
    SPORT_TYPE_MIXED_CARDIO        =0x37, //混合有氧
    SPORT_TYPE_LATIN_DANCE         =0x38, //拉丁舞
    SPORT_TYPE_STREET_DANCE        =0x39, //街舞
    SPORT_TYPE_KICKBOXING          =0x3A, //自由搏击
    SPORT_TYPE_BARRE               =0x3B, //芭蕾
    SPORT_TYPE_AUSTRALIAN_FOOTBALL =0x3C, //澳式足球
    SPORT_TYPE_MARTIAL_ARTS        =0x3D, //武术
    SPORT_TYPE_STAIRS              =0x3E, //爬楼
    SPORT_TYPE_HANDBALL            =0x3F, //手球
    SPORT_TYPE_BOWLING             =0x40, //保龄球
    SPORT_TYPE_RACQUETBALL         =0x41, //壁球
    SPORT_TYPE_CURLING             =0x42, //冰壶
    SPORT_TYPE_HUNTING             =0x43, //打猎
    SPORT_TYPE_SNOWBOARDING        =0x44, //单板滑雪
    SPORT_TYPE_PLAY                =0x45, //休闲运动
    SPORT_TYPE_AMERICAN_FOOTBALL   =0x46, //美式橄榄球
    SPORT_TYPE_HAND_CYCLING        =0x47, //手摇车
    SPORT_TYPE_FISHING             =0x48, //钓鱼
    SPORT_TYPE_DISC_SPORTS         =0x49, //飞盘
    SPORT_TYPE_RUGGER              =0x4A, //橄榄球
    SPORT_TYPE_GOLF                =0x4B, //高尔夫
    SPORT_TYPE_FOLK_DANCE          =0x4C, //民族舞
    SPORT_TYPE_DOWNHILL_SKIING     =0x4D, //高山滑雪
    SPORT_TYPE_SNOW_SPORTS         =0x4E, //雪上运动
    SPORT_TYPE_MIND_BODY           =0x4F, //舒缓冥想类运动
    SPORT_TYPE_CORE_TRAINING       =0x50, //核心训练
    SPORT_TYPE_SKATING             =0x51, //滑冰
    SPORT_TYPE_FITNESS_GAMING      =0x52, //健身游戏
    SPORT_TYPE_AEROBICS            =0x53, //健身操
    SPORT_TYPE_GROUP_TRAINING      =0x54, //团体操
    SPORT_TYPE_KENDO               =0x55, //搏击操
    SPORT_TYPE_LACROSSE            =0x56, //长曲棍球
    SPORT_TYPE_ROLLING             =0x57, //泡沫轴筋膜放松
    SPORT_TYPE_WRESTLING           =0x58, //摔跤
    SPORT_TYPE_FENCING             =0x59, //击剑
    SPORT_TYPE_SOFTBALL            =0x5A, //垒球
    SPORT_TYPE_SINGLE_BAR          =0x5B, //单杠
    SPORT_TYPE_PARALLEL_BARS       =0x5C, //双杠
    SPORT_TYPE_ROLLER_SKATING      =0x5D, //轮滑
    SPORT_TYPE_HULA_HOOP           =0x5E, //呼啦圈
    SPORT_TYPE_DARTS               =0x5F, //飞镖
    SPORT_TYPE_PICKLEBALL          =0x60, //匹克球
    SPORT_TYPE_HIIT                =0x61, //HIIT
    SPORT_TYPE_SHOOTING            =0x62, //射击
    SPORT_TYPE_JUDO                =0x63, //柔道
    SPORT_TYPE_TRAMPOLINE          =0x64, //蹦床
    SPORT_TYPE_SKATEBOARDING       =0x65, //滑板
    SPORT_TYPE_HOVERBOARD          =0x66, //平衡车
    SPORT_TYPE_BLADING             =0x67, //溜旱冰
    SPORT_TYPE_PARKOUR             =0x68, //跑酷
    SPORT_TYPE_DIVING              =0x69, //跳水
    SPORT_TYPE_SURFING             =0x6A, //冲浪
    SPORT_TYPE_SNORKELING          =0x6B, //浮潜
    SPORT_TYPE_PULL_UP             =0x6C, //引体向上
    SPORT_TYPE_PUSH_UP             =0x6D, //俯卧撑
    SPORT_TYPE_PLANKING            =0x6E, //平板支撑
    SPORT_TYPE_ROCK_CLIMBING       =0x6F, //攀岩
    SPORT_TYPE_HIGHTJUMP           =0x70, //跳高
    SPORT_TYPE_BUNGEE_JUMPING      =0x71, //蹦极
    SPORT_TYPE_LONGJUMP            =0x72, //跳远
    SPORT_TYPE_MARATHON            =0x73, //马拉松
//    SPORT_TYPE_FLEXIBILITY_TRAINING  =0x74, //灵活性训练116
//    SPORT_TYPE_AEROBICS              =0x75, //有氧运动117
//    SPORT_TYPE_SIDEPLANK             =0x75, //侧板118
//    SPORT_TYPE_MIXED_CARDIO          =0x76, //混合有氧运动119
#if UTE_MODULE_SCREENS_WORKOUTS_LIST_SUPPORT
    SPORT_TYPE_OTHER               =0xFD, //其他，判断此类型，跳转到其他界面
#endif
#else
    SPORT_TYPE_CYWEE_SWIM_GNSS= 0X1A,//第三方游泳算法，带gps数据 20201009
    SPORT_TYPE_CYWEE_SWIM = 0X1B,//第三方游泳算法 20201009
#endif
    SPORT_TYPE_MAX,
} sport_mode_type;


/***************
***24种运动的数据结构
***
****************/
typedef struct
{
    bool   isStartSport;
    bool isMan;
    uint8_t privateHeight;
    uint8_t privateWeight;
    uint8_t  sportType;
    uint16_t  sportKcal;  // 运动卡路里
    uint16_t  sportSteps; // 运动步数
    uint16_t  sportSpeed; // 配速，单位是：一公里用多少秒
    uint32_t  sportTimeCnt; // 运动时间计时
    uint16_t  swimmingCurrentCnt; // 游泳次数
    uint16_t  jumpRopeCurrentCnt; // 跳绳次数
    float sportDistance;   //运动距离，单位km
    struct prf_date_time startSportTime;
    struct prf_date_time stopSportTime;
} ute_sport_mode_data_t;
#if UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_SUPPORT
extern ute_sport_mode_data_t mUTESportDatas;

/***
*操作步骤
*1、调用enableUTESport，传入sportType，启动对应的运动模式
*2、每秒更新获取运动数据getUTESportData，获取全局变量ute_sport_mode_data_t mUTESportDatas的参数，如果对应的运动没有该参数，那么对于为0
*3、调用disableUTESport停止当前运动，当前数据会保存到内存中，直到下次重启运动或系统重启才会清零；
*****/
void enableUTESport(sport_mode_type sportType,struct prf_date_time initTime,bool isMan,uint8_t privateWeight,uint8_t privateHeight);
void disableUTESport(void);
void getUTESportData(struct prf_date_time currentTime);

void app_jump_rope_algorithms_input_data(uint8_t gsensorRang,int16_t *x,int16_t *y,int16_t *z,uint16_t *accValue);  //
#endif
#endif // UTE_STEP_SLEEP_ALGORITHMS_SUPPORT
#endif
