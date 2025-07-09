/**
*@file
*@brief        多国语言
*@details
*@author       zn.zeng
*@date       2021-10-23
*@version      v1.0
*/
#include "ute_language_common.h"
#include "ute_module_systemtime.h"
#include "ute_module_log.h"

const char * const *i18n;

/**
 * @brief        切换语言
 * @details
 * @param[in]    langId 语言ID
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-08
 */
void uteLanguageCommonSelect(MULTIPLE_LANGUAGE_ID langId)
{
    switch (langId)
    {
#if SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT
        case CHINESE_LANGUAGE_ID:
            i18n = i18n_zh_rcn;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_ENGLISH_LANGUAGE_SUPPORT
        case ENGLISH_LANGUAGE_ID:
            i18n = i18n_en_rus;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_KOREAN_LANGUAGE_SUPPORT
        case KOREAN_LANGUAGE_ID:
            i18n = i18n_ko_rkr;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_JAPANESE_LANGUAGE_SUPPORT
        case JAPANESE_LANGUAGE_ID:
            i18n = i18n_ja_rjp;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_GERMAN_LANGUAGE_SUPPORT
        case GERMAN_LANGUAGE_ID:
            i18n = i18n_de_rde;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_SPANISH_LANGUAGE_SUPPORT
        case SPANISH_LANGUAGE_ID:
            i18n = i18n_es_res;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_FRENCH_LANGUAGE_SUPPORT
        case FRENCH_LANGUAGE_ID:
            i18n = i18n_fr_rfr;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_ITALIAN_LANGUAGE_SUPPORT
        case ITALIAN_LANGUAGE_ID:
            i18n = i18n_it_rit;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_PORTUGUESE_LANGUAGE_SUPPORT
        case PORTUGUESE_LANGUAGE_ID:
            i18n = i18n_pt_rpt;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_ARABIC_LANGUAGE_SUPPORT
        case ARABIC_LANGUAGE_ID:
            i18n = i18n_ar_ril;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_INDIA_LANGUAGE_SUPPORT
        case INDIA_LANGUAGE_ID:
            i18n = i18n_hi_rin;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_HINDI_LANGUAGE_SUPPORT
        case HINDI_LANGUAGE_ID:
            i18n = i18n_hi_rin;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_POLISH_LANGUAGE_SUPPORT
        case POLISH_LANGUAGE_ID:
            i18n = i18n_pl_rpl;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_RUSSIAN_LANGUAGE_SUPPORT
        case RUSSIAN_LANGUAGE_ID:
            i18n = i18n_ru_rru;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_DUTCH_LANGUAGE_SUPPORT
        case DUTCH_LANGUAGE_ID:
            i18n = i18n_nl_rnl;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_TURKISH_LANGUAGE_SUPPORT
        case TURKISH_LANGUAGE_ID:
            i18n = i18n_tr_rtr;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_BENGALI_LANGUAGE_SUPPORT
        case BENGALI_LANGUAGE_ID:
            i18n = i18n_bn_rbn;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_URDU_LANGUAGE_SUPPORT
        case URDU_LANGUAGE_ID:
            i18n = i18n_ur_rur;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_INDONESUAN_LANGUAGE_SUPPORT
        case INDONESUAN_LANGUAGE_ID:
            i18n = i18n_in_rid;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_PUNJABI_LANGUAGE_SUPPORT
        case PUNJABI_LANGUAGE_ID:
            i18n = i18n_pa_rpa;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_THAI_LANGUAGE_SUPPORT
        case THAI_LANGUAGE_ID:
            i18n = i18n_th_rth;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_CZECH_LANGUAGE_SUPPORT
        case CZECH_LANGUAGE_ID:
            i18n = i18n_cs_rcz;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_TRADITIONAL_CHINESE_LANGUAGE_SUPPORT
        case TRADITIONAL_CHINESE_ID:
            i18n = i18n_zh_rtw;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_ROMANIAN_LANGUAGE_SUPPORT
        case ROMANIAN_LANGUAGE_ID:
            i18n = i18n_ro_rro;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_VIETNAMESE_LANGUAGE_SUPPORT
        case VIETNAMESE_LANGUAGE_ID:
            i18n = i18n_vi_rvn;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_MALAYSIA_LANGUAGE_SUPPORT
        case MALAYSIA_LANGUAGE_ID:
            i18n = i18n_ms_rmy;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_GREEK_LANGUAGE_SUPPORT
        case GREEK_LANGUAGE_ID:
            i18n = i18n_el;
            break;
#endif
        default :
            // if(DEFAULT_LANGUAGE == CHINESE_LANGUAGE_ID)
            // {
            //     i18n = i18n_zh_rcn;
            // }
            //else
        {
            i18n = i18n_en_rus;
        }
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, Language ID 0x%x is not supported", __func__, langId);
        break;
    }
}
