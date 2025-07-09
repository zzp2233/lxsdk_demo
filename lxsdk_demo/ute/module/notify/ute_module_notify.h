/**
*@file
*@brief        消息提醒处理模块头文件
*@details
*@author       zn.zeng
*@date       2021-08-24
*@version      v1.0
*/
#ifndef _UTE_MODULE_NOTIFY_H_
#define _UTE_MODULE_NOTIFY_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"
/*! 消息推送数据保存目录 zn.zeng, 2021-08-24  */
#define UTE_MODULE_FILESYSTEM_NOTIFY_DIR "notify"
#define UTE_MODULE_FILESYSTEM_NOTIFY_ANCS_DIR "ancs"
#define UTE_MODULE_FILESYSTEM_NOTIFY_ANCS_FLAG "flag"

/*! 消息推送类型表 zn.zeng, 2021-08-25  */
enum
{
    MSG_CALL   = 0x00,
    MSG_QQ     = 0x01,
    MSG_WECHAT = 0x02,
    MSG_MMS    = 0x03,
    MSG_OTHER = 0x04,
    MSG_Facebook =0x05,
    MSG_Twitter =0x06,
    MSG_WhatsApp =0x07,
    MSG_Skype =0x08,
    MSG_FacebookMessenger=0x09,
    MSG_Hangouts =0x0A,
    MSG_Line =0x0B,
    MSG_Linkedin =0x0c,
    MSG_Instagram =0x0d,
    MSG_Viber =0x0e,
    MSG_KakaoTalk =0x0F,
    MSG_VKontakte =0x10,
    MSG_Snapchat =0x11,
    MSG_Googleplus =0x12,
    MSG_Gmail =0x13,
    MSG_Flickr =0x14,
    MSG_Tumblr =0x15,
    MSG_Pinterest =0x16,
    MSG_YouTube =0x17,
    MSG_Telegram = 0x18,
    MSG_Truecaller =0x19,
    MSG_Paytm =0x1A,
    MSG_Zalo = 0x1b,
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
    MSG_imo =0x1C,
    MSG_MicrosoftTeams =0x1D,
    MSG_Outlook =0x1e,
    MSG_Swiggy =0x1f,
    MSG_Zomato =0x20,
    MSG_Gpay =0x21,
    MSG_PhonePe =0x22,
    MSG_Hotstar =0x23,
    MSG_PrimeVideo =0x24,
    MSG_Flipkart =0x25,
    MSG_Amazon =0x26,
    MSG_Myntra =0x27,
    MSG_NoiseApp =0x28,
    MSG_DailyHunt =0x29,
    MSG_Inshorts =0x2a,
    MSG_BookMyShow =0x2b,

    MSG_Calendar =0x2c,
    MSG_JioTv =0x2d,
    MSG_MakeMyTrip =0x2e,
    MSG_Netflix =0x2f,
    MSG_Ola =0x30,
    MSG_ReflexApp =0x31,
    MSG_Uber =0x32,
    MSG_YTMusic =0x33,

    //add 2022-11-22
    MSG_WhatsApp_Business =0x34,
    MSG_Dunzo  =0x35,
    MSG_Gaana     =0x36,
    MSG_Google_Drive =0x37,
    MSG_googlechat =0x38,
    MSG_Wynkmusic =0x39,
    MSG_Yahoo =0x3A,
    MSG_TitanSmartWorld =0x3B,
    MSG_Slack =0x3C,
    MSG_Spotify =0x3D,
#endif
    MSG_MAX_CNT,
};
/*! ancs ios app id zn.zeng, 2021-11-18  */
#define APP_ID_SMS "MobileSMS"
#define APP_ID_QQ ".mqq"
#define APP_ID_WECHAT ".tencent.xin"
#define APP_ID_CALL "mobilephone"
#define APP_ID_FACEBOOK "Facebook"
#define APP_ID_TWITTER "Tweetie"
#define APP_ID_WHATSAPP "WhatsApp"
#define APP_ID_MESSENGER "facebook.Messenger"
#define APP_ID_SKYPE "tomskype"
#define APP_ID_SKYPE_FOREGN "skype"
#define APP_ID_HANGOUTS "hangouts"
#define APP_ID_LINE "line"
#define APP_ID_LinkedIn "linkedin"
#define APP_ID_Instagram "instagram"
#define APP_ID_Viber "viber"
#define APP_ID_KakaoTalk "KakaoTalk"
#define APP_ID_VKontakte "vkclient"
#define APP_ID_Snapchat "picaboo"
#define APP_ID_GooglePlus "GooglePlus"
#define APP_ID_Gmail "Gmail"
#define APP_ID_Flickr "flickr"
#define APP_ID_Tumblr "tumblr"
#define APP_ID_Pinterest "pinterest"
#define APP_ID_YouTube "youtube"
#define APP_ID_Telegram "telegra"
#define APP_ID_Zalo "zingalo"  //vn.com.vng.zingalo

#define APP_ID_Truecaller "TrueCallerOther"
#define APP_ID_Paytm0 "one97.paytm"
#define APP_ID_Paytm1 "one97.merchant"
#define APP_ID_Paytm2 "one97.hero"
//#define APP_ID_Zalo "vng.zingalo"
//#define APP_ID_imo0 "babypenguin.imo"
//#define APP_ID_imo1 "babypenguin.imolive"
//#define APP_ID_MicrosoftTeams "microsoft.skype.teams"

#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
#define APP_ID_imo0                          "babypenguin.imo"
#define APP_ID_imo1                          "babypenguin.imolive"
#define APP_ID_MicrosoftTeams                "microsoft.skype.teams"
#define APP_ID_Outlook                       "Office.Outlook"
#define APP_ID_Swiggy                        "bundl.swiggy"
#define APP_ID_Zomato                        "zomato"
#define APP_ID_Gpay                          "paisa"
#define APP_ID_PhonePe                       "PhonePeApp"
#define APP_ID_Hotstar                       "hotstar"
#define APP_ID_PrimeVideo                    "AIVApp"
#define APP_ID_Flipkart                      "flipkart"
#define APP_ID_Amazon                        "AmazonIN"
#define APP_ID_Myntra                        "Myntra"
#define APP_ID_NoiseApp                      "noise"
#define APP_ID_DailyHunt                     "eternoinfotech.newshunt"
#define APP_ID_Inshorts                      "nis.app"
#define APP_ID_BookMyShow                    "BookMyShow.com"


#define APP_ID_Calendar                      "mobilecal"   //ios:com.apple.mobilecal 安卓：com.google.android.calendar
#define APP_ID_JioTv                         "jioplay"    // com.jio.jioplay
#define APP_ID_MakeMyTrip                    "MMT"    //com.Iphone.MMT
#define APP_ID_Netflix                       "Netflix"    // com.netflix.Netflix
#define APP_ID_Ola                           "OlaCabs"  // olacabs.OlaCabs
#define APP_ID_ReflexApp                     "reflex"  //com.titan.fastrack.reflex
#define APP_ID_Uber                          "UberClient"    //com.ubercab.UberClient
#define APP_ID_YTMusic                       "youtubemusic"  //com.google.ios.youtubemusic

//#define APP_ID_OT_Messager                   "ot.Messager"
#define APP_ID_WhatsApp_Business             "whatsapp.WhatsAppSMB"  // iOS：net.whatsapp.WhatsAppSMB，安卓：com.whatsapp.w4b
#define APP_ID_Dunzo                         "in.dunzo.lattelite"  //Ios： in.dunzo.lattelite，安卓：com.dunzo.user
#define APP_ID_Gaana                         "til.gaana"    //Ios：com.til.gaana  安卓：com.gaana
#define APP_ID_Google_Drive                  "google.Drive"  //Ios：com.google.Drive  安卓：com.google.android.apps.docs
#define APP_ID_googlechat                    "google.Dynamite"//iOS - com.google.Dynamite   安卓com.google.android.apps.dynamite
#define APP_ID_Wynkmusic                     "Bharti.AirtelMusicApp"  //ios：com.Bharti.AirtelMusicApp  安卓：com.bsbportal.music
#define APP_ID_Yahoo                         "yahoo.Aerogram"    //ios：com.yahoo.Aerogram  安卓：com.yahoo.mobile.client.android.mail
#define APP_ID_TitanSmartWorld               "titan.smartworld"   //ios：com.titan.smartworld  安卓：com.titan.smartworld
#define APP_ID_Slack                         "tinyspeck.chatlyio" //ios：com.tinyspeck.chatlyio，安卓：com.Slack
#define APP_ID_Spotify                       "spotify.client"  //ios：com.spotify.client 安卓：com.spotify.music
#endif

#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
//ancsAdditionalOpenFlag[17]
#define ANCS_OPEN_SMS 0x01
#define ANCS_OPEN_QQ 0x02
#define ANCS_OPEN_WECHAT 0x04
#define ANCS_OPEN_INCALL 0x08
#define ANCS_OPEN_OTHER 0x10
#define ANCS_OPEN_FACEBOOK 0x20
#define ANCS_OPEN_TWITTER 0x40
#define ANCS_OPEN_WHATSAPP 0x80

//ancsAdditionalOpenFlag[16]
#define ANCS_OPEN_MESSENGER 0x01
#define ANCS_OPEN_LINE 0x02
#define ANCS_OPEN_SKYPE 0x04
#define ANCS_OPEN_HANGOUTS 0x08
#define ANCS_OPEN_LINKEDIN 0x10
#define ANCS_OPEN_INSTAGRAM 0x20
#define ANCS_OPEN_VIBER 0x40
#define ANCS_OPEN_KAKAOTALK 0x80

//ancsAdditionalOpenFlag[15]
#define ANCS_OPEN_VKONTAKTE 0x01
#define ANCS_OPEN_SNAPCHAT 0x02
#define ANCS_OPEN_GOOGLEPLUS 0x04
#define ANCS_OPEN_GMAIL 0x08
#define ANCS_OPEN_FLICKR 0x10
#define ANCS_OPEN_TUMBLR 0x20
#define ANCS_OPEN_PINTEREST 0x40
#define ANCS_OPEN_YOUTUBE 0x80

//ancsAdditionalOpenFlag[14]
#define ANCS_OPEN_TELEGRAM 0x01
#define ANCS_OPEN_TRUECALLER 0x02
#define ANCS_OPEN_PAYTM 0x04
#define ANCS_OPEN_ZALO 0x08
#define ANCS_OPEN_IMO 0x10
#define ANCS_OPEN_MICROSOFTTEAMS 0x20
#define ANCS_OPEN_OUTLOOK 0x40
#define ANCS_OPEN_SWIGGY 0x80

//ancsAdditionalOpenFlag[13]
#define ANCS_OPEN_ZOMATO 0x01
#define ANCS_OPEN_GPAY 0x02
#define ANCS_OPEN_PHONEPE 0x04
#define ANCS_OPEN_HOTSTAR 0x08
#define ANCS_OPEN_PRIMEVIDEO 0x10
#define ANCS_OPEN_FLIPKART 0x20
#define ANCS_OPEN_AMAZON 0x40
#define ANCS_OPEN_MYNTRA 0x80

//ancsAdditionalOpenFlag[12]
#define ANCS_OPEN_NOISEAPP 0x01
#define ANCS_OPEN_DAILYHUNT 0x02
#define ANCS_OPEN_INSHORTS 0x04
#define ANCS_OPEN_BOOKMYSHOW 0x08
#define ANCS_OPEN_CALENDAR 0x10
#define ANCS_OPEN_JIOTV 0x20
#define ANCS_OPEN_MAKEMYTRIP 0x40
#define ANCS_OPEN_NETFLIX 0x80

//ancsAdditionalOpenFlag[11]
#define ANCS_OPEN_OLA 0x01
#define ANCS_OPEN_REFLEXAPP 0x02
#define ANCS_OPEN_UBER   0x04
#define ANCS_OPEN_YTMUSIC     0x08
#define ANCS_OPEN_WHATSAPP_BUSINESS 0x10
#define ANCS_OPEN_DUNZO 0x20
#define ANCS_OPEN_GAANA 0x40
#define ANCS_OPEN_GOOGLE_DRIVE 0x80

//ancsAdditionalOpenFlag[10]
#define ANCS_OPEN_GOOGLECHAT 0x01
#define ANCS_OPEN_WYNKMUSIC   0x02
#define ANCS_OPEN_YAHOO 0x04
#define ANCS_OPEN_TITANSMARTWORLD 0x08
#define ANCS_OPEN_SLACK 0x10
#define ANCS_OPEN_SPOTIFY 0x20

#else
enum
{
    ANCS_OPEN_SMS =0x00000001,
    ANCS_OPEN_QQ = 0x00000002,
    ANCS_OPEN_WECHAT =0x00000004,
    ANCS_OPEN_INCALL =0x00000008,
    ANCS_OPEN_OTHER =0x00000010,
    ANCS_OPEN_FACEBOOK =0x00000020,
    ANCS_OPEN_TWITTER =0x00000040,
    ANCS_OPEN_WHATSAPP =0x00000080,
    ANCS_OPEN_MESSENGER =0x00000100,
    ANCS_OPEN_LINE =0x00000200,
    ANCS_OPEN_SKYPE =0x00000400,
    ANCS_OPEN_HANGOUTS =0x00000800,

    ANCS_OPEN_LINKEDIN   =0X00001000,
    ANCS_OPEN_INSTAGRAM =0X00002000,
    ANCS_OPEN_VIBER =0X00004000,
    ANCS_OPEN_KAKAOTALK =0X00008000,
    ANCS_OPEN_VKONTAKTE =0X00010000,
    ANCS_OPEN_SNAPCHAT =0X00020000,
    ANCS_OPEN_GOOGLEPLUS =0X00040000,
    ANCS_OPEN_GMAIL =0X00080000,
    ANCS_OPEN_FLICKR =0X00100000,
    ANCS_OPEN_TUMBLR =0X00200000,
    ANCS_OPEN_PINTEREST =0X00400000,
    ANCS_OPEN_YOUTUBE =0X00800000,

//    ANCS_OPEN_TELEGRAM =0X01000000,
//    ANCS_OPEN_ZALO =0x08000000,
    ANCS_OPEN_TELEGRAM  = 0X01000000,
    ANCS_OPEN_TRUECALLER = 0X02000000,
    ANCS_OPEN_PAYTM = 0X04000000,
    ANCS_OPEN_ZALO  =0X08000000,
    ANCS_OPEN_IMO  =0X10000000,
    ANCS_OPEN_MICROSOFTTEAMS  =0X20000000,
};



#endif
/*! 消息推送数据结构zn.zeng, 2021-11-09  */
typedef struct
{
    uint16_t size;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t type;
    uint8_t titleSize;
    uint8_t content[UTE_NOTIFY_MSG_CONTENT_MAX_SIZE];
} ute_module_notify_one_data_t;
/*! 消息推送数据结构zn.zeng, 2021-08-24  */
typedef struct
{
    ute_module_notify_one_data_t currNotify;
    ute_module_notify_one_data_t historyNotify[UTE_MODULE_NOTIFY_SAVE_CNT];
    uint8_t totalNotifyCnt;
    uint8_t displayIndex;
    uint32_t ancsSetOpenFlag;
    bool ancsHasOpen;
    bool isNewNotify;
    uint8_t titleBuff[UTE_NOTIFY_MSG_CONTENT_MAX_SIZE];

#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
    uint8_t ancsAdditionalOpenCurr[18];
    uint8_t ancsAdditionalOpenFlag[18];
#endif
} ute_module_notify_data_t;
void uteModuleNotifyCallDisableHandlerMsg(void);
void uteModuleNotifyCallUnansweredHandlerMsg(void);
void uteModuleNotifyGetData(ute_module_notify_data_t *data);
void uteModuleNotifyNotifycationHandlerMsg(void);
void uteModuleNotifyInit(void);
void uteModuleNotifySetAncsInfo(uint8_t attId,uint8_t *buff,uint16_t length);
void uteModuleNotifyAncsSetFlag(uint32_t flag);
void uteModuleNotifyAncsGetFlag(uint32_t *flag);
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
void uteModuleNotifyAncsSetAdditionalFlag(bool flag,uint8_t*receive,uint8_t length);
void uteModuleNotifyAncsGetAdditionalFlag(uint8_t*receive);
#endif

void uteModuleNotifyAncsStartPair(void);
void uteModuleNotifyAncsStartPairHandlerMsg(void);
void uteModuleNotifyAncsTimerConnectHandler(bool isConnected);
void uteModuleNotifyDelAllHistoryData(bool isDelAll);
void uteModuleNotifySetNewNotifyFlag(bool isNewNotify);
bool uteModuleNotifyIsNewNotifyDisplay(void);
uint8_t uteModuleNotifyGetTotalNotifyCnt(void);
void uteModuleNotifyGetDataPointer(ute_module_notify_data_t **data);
void uteModuleNotifySetDisplayIndex(uint8_t displayIndex);

#endif //_UTE_MODULE_CRC_H_

