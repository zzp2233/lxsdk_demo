/**
*@file
*@brief        字符编码处理模块
*@details
*@author       zn.zeng
*@date       2021-08-24
*@version      v1.0
*/
#ifndef _UTE_MODULE_CHARENCODE_H_
#define _UTE_MODULE_CHARENCODE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"

enum Deformation
{
    ARABIC_FIRST = 0,
    ARABIC_LAST,
    ARABIC_MIDDLE,
    ARABIC_ALONE,
};
#if UTE_THRID_WU_CHAR_ENCODE_COMMON_SUPPORT
#define PIXELS_PER_BYTE                 8
#define FONT_INDEX_TAB_SIZE             4    //单个字符对应的字体检索信息长度为 4Byte  (b0~b25: 记录点阵信息的起始地址, b26~b31: 记录当前字符的象素宽度)

#define SBN_GLYPH_HEAD_LEN      7   // 详见 sbn_glyph_t (不含bitmap指针）
#define ID_SBN_GLYPH_DWIDTH     4
#define ID_SBN_GLYPH_BYTES_LO   5
#define ID_SBN_GLYPH_BYTES_HI   6
/*针对 not fixed 存储格式*/
typedef struct
{
    unsigned char width;
    unsigned char height;
    int8_t x_offset;
    int8_t y_offset;
} font_bbx_t;
typedef struct gui_font_head_not_fixed
{
//  unsigned char   magic[4];   //'U'('S', 'M'), 'F', 'L', X---Unicode(Simple or MBCS) Font Library, X: Version (def: b7 == 1).
    unsigned int    file_size;
    unsigned int    head_size;
    unsigned char   ScanMode;
    unsigned char   nSection;  /* Section number */
    unsigned short  wCpFlag;    /* Codepage flag:  bit0~bit13 */
    unsigned char   bpp;        /* Bits per pixel. */
    unsigned char   zip;      // zip
    unsigned char   resvered[2];
    font_bbx_t bbx;              /* Font bounding box. */
    int font_ascent;           /* Font ascent. */
    int font_descent;          /* Font descent.   */
    int glyphs_size;
    int glyphs_used;
} GUI_FONT_HEAD_NF;
typedef struct tagFlSectionInfo
{
    uint16_t First;   // 第一个字符
    uint16_t Last;    // 最后一个字符
    uint32_t OffAddr; // 第一个字符指向检索表的偏移位置
} FL_SECTION_INF;
typedef struct
{
    font_bbx_t bbx;
    unsigned char dwidth;
    unsigned short bytes;
    //unsigned char *bitmap;   // zzn del
    unsigned char nBytesPerLine;   // zzn add
    int font_ascent;
    unsigned char bbxHeight;
    uint32_t dotMatrixDataAddress;
    uint32_t font16x16Address;
    uint32_t font24x24Address;
    uint32_t font32x32Address;
    uint16_t unicode;
} sbn_glyph_t;
#endif
void uteModuleCharencodeUnicodeConversionUtf8(uint8_t *unicode,uint16_t unicodeLen,uint8_t *utf8,uint16_t *uft8Len,uint16_t limitLen);
void uteModuleCharencodeUtf8ConversionUnicode(uint8_t *unicode,uint16_t *unicodeLen,uint8_t *utf8,uint16_t uft8Len);
bool uteModuleCharencodeGetUtf8String(uint8_t*input_utf8,int16_t len,uint8_t*output_utf8,uint16_t *utf8_len);
bool uteModuleCharencodeArabUnicodeStringConversion(uint16_t *input,uint32_t inputSize,uint16_t *subtmp,uint16_t *output,uint32_t *outputSize);
bool uteModuleCharencodeIsPunctuationUnicode(uint16_t unicode);
bool uteModuleCharencodeIsArbicUnicode(uint16_t unicode);
bool uteModuleCharencodeIsOneByteUnicodeChar(uint16_t unicode);
void uteModuleCharencodeLittleBigEndian16BitSwitch(uint8_t *buff,uint32_t byteSize);
#if UTE_THRID_WU_CHAR_ENCODE_COMMON_SUPPORT
bool uteModuleCharencodeThridWuReadEncodeCharInfo(uint16_t unicode,uint8_t enlargement,sbn_glyph_t *charInfo);
void uteModuleCharencodeThridWuFillCharBuff(uint8_t *buffOutput,sbn_glyph_t *charInfo);
#endif
#if UTE_THRID_CHAR_ENCODE_THAI_SUPPORT
bool uteModuleCharencodeIsThaiChar(uint16_t unicode);
void uteModuleCharencodeThaiUnicodeStringConversion(uint16_t *input,uint32_t *inputSize);
#endif
#if UTE_THRID_CHAR_ENCODE_BENGALI_SUPPORT
bool uteModuleCharencodeIsBengaliChar(uint16_t unicode);
void uteModuleCharencodeBengaliUnicodeStringConversion(uint16_t *input,uint32_t *inputSize);
#endif
#if UTE_THRID_CHAR_ENCODE_HEBREW_SUPPORT
bool uteModuleCharencodeIsHebrewChar(uint16_t unicode);
bool uteModuleCharencodeHebrewUnicodeStringConversion(uint16_t *input,uint32_t *inputSize);
#endif
#if UTE_THRID_CHAR_ENCODE_ARABIC_SUPPORT
bool uteModuleCharencodeIsArabicChar(uint16_t unicode);
void uteModuleCharencodeArabicUnicodeStringConversion(uint16_t *input,uint32_t *inputSize);
#endif
#if UTE_THRID_CHAR_ENCODE_DEVANAGARI_SUPPORT
bool uteModuleCharencodeIsDevanagariChar(uint16_t unicode);
bool uteModuleCharencodeDevanagariUnicodeStringConversion(uint16_t *input,uint32_t *inputSize);
#endif
void uteModuleCharencodeReplaceSubString(const char *original, char *result, char *replace, char *replacement);
#endif //_UTE_MODULE_CHARENCODE_H_

