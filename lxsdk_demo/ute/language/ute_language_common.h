/**
*@file
*@brief        多国语言
*@details
*@author       zn.zeng
*@date       2021-10-23
*@version      v1.0
*/
#ifndef _UTE_LANGUAGE_COMMON_H_
#define _UTE_LANGUAGE_COMMON_H_
#include "ute_module_platform.h"
#include "ute_project_config.h"
#include "ute_multi_language.h"

/*! 语言id表 zn.zeng, 2021-08-24  */
typedef enum
{
    CHINESE_LANGUAGE_ID = 0x01,   // 中文 0x01
    ENGLISH_LANGUAGE_ID,          // 英文 0x02
    KOREAN_LANGUAGE_ID,           // 韩文 0x03
    JAPANESE_LANGUAGE_ID,         // 日文 0x04
    GERMAN_LANGUAGE_ID,           // 德文 0x05
    SPANISH_LANGUAGE_ID,          // 西班牙文 0x06
    FRENCH_LANGUAGE_ID,           // 法文 0x07
    ITALIAN_LANGUAGE_ID,          // 意大利文 0x08
    PORTUGUESE_LANGUAGE_ID,       // 葡萄牙文 0x09
    ARABIC_LANGUAGE_ID,           // 阿拉伯文 0x0A
    INDIA_LANGUAGE_ID,            // India   0x0B
    HINDI_LANGUAGE_ID,            // 印地语  0x0C
    POLISH_LANGUAGE_ID,           // 波兰语  0x0D
    RUSSIAN_LANGUAGE_ID,          // 俄语  0x0E
    DUTCH_LANGUAGE_ID,            // 荷兰语 0x0F
    TURKISH_LANGUAGE_ID,          // 土耳其语  x10
    BENGALI_LANGUAGE_ID,          // 孟加拉语  x11  Bengali
    URDU_LANGUAGE_ID,             // 乌尔都语   x12  Urdu
    INDONESUAN_LANGUAGE_ID,       // 印度尼西亚语（爪哇语）  x13  Indonesian
    PUNJABI_LANGUAGE_ID,          // 旁遮普语  x14   Punjabi
    THAI_LANGUAGE_ID,             // 泰文  x15   Thai
    CZECH_LANGUAGE_ID,            // 捷克语  x16  Czech
    TRADITIONAL_CHINESE_ID,       // 繁体中文 x17 traditional Chinese
    HEBREW_LANGUAGE_ID,           // 希伯来语 x18
    SLOVAK_LANGUAGE_ID = 0x19,    // 斯洛伐克语 0x19
    HUNGARIAN_LANGUAGE_ID = 0x1a, // 匈牙利语 0x1a
    ROMANIAN_LANGUAGE_ID = 0x1B,  // 罗马尼亚 x1B
    BULGARIAN_LANGUAGE_ID = 0X26, // 保加利亚语 0X26  38
    PERSIAN_LANGUAGE_ID = 0x28,   // 波斯语 x28
    KHMWE_LANGUAGE_ID = 0x2A,     // 高棉语 x2A
    HRVATSKA_LANGUAGE_ID = 0X33,   // 克罗地亚语 0X33  51
    DANISH_LANGUAGE_ID = 0x39,     // 丹麦语 0X39
    MALAYSIA_LANGUAGE_ID = 0x40,   // 马来西亚语 0x40
    SWEDISH_LANGUAGE_ID = 0x4a,    // 瑞典语（svenska） 0x4a
    SLOVENIAN_LANGUAGE_ID = 0X4d,  // 斯洛文尼亚语 0X4d 77
    UKRAINE_LANGUAGE_ID = 0x58,    // 乌克兰语 0x58
    GREEK_LANGUAGE_ID = 0x5a,      // 希腊语 x5a Greek
    VIETNAMESE_LANGUAGE_ID = 0x63, // 越南语Vietnamese 0X63
    LANGUAGE_ID_MAX,
} MULTIPLE_LANGUAGE_ID;

void uteLanguageCommonSelect(MULTIPLE_LANGUAGE_ID langId);

#endif//_UTE_LANGUAGE_COMMON_H_

