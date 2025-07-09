/**
*@file
*@brief        字符编码处理模块
*@details
*@author       zn.zeng
*@date       2021-08-24
*@version      v1.0
*/

#include "ute_module_charencode.h"
#include "ute_module_log.h"
#include "include.h"
#if UTE_THRID_CHAR_ENCODE_THAI_SUPPORT
#include"Lang_Thai_Arialuni.h"
#endif
#if UTE_THRID_CHAR_ENCODE_BENGALI_SUPPORT
#include"Lang_Bengali_Noto.h"
#endif
#if UTE_THRID_CHAR_ENCODE_HEBREW_SUPPORT
#include"Lang_Hebrew_Noto.h"
#endif
#if UTE_THRID_CHAR_ENCODE_ARABIC_SUPPORT
#include"Lang_Arabic_Noto.h"
#endif
#if UTE_THRID_CHAR_ENCODE_DEVANAGARI_SUPPORT
#include"Lang_Devanagari.h"
#endif
/**
*@brief     unicode 转换成 utf-8
*@details   字集 unicode ucs-2
*@param[in] uint8_t *unicode 输入的数据指针
*@param[in] uint32_t unicodeLen 输入的数据长度
*@param[out] uint8_t *utf8 输出的数据指针
*@param[out] uint16_t *uft8Len 输出的数据长度
*@param[in] uint16_t limitLen *utf8 输出的数据指针的最大长度
*@author        zn.zeng
*@date        2021-08-24
*/
void uteModuleCharencodeUnicodeConversionUtf8(uint8_t *unicode,uint16_t unicodeLen,uint8_t *utf8,uint16_t *uft8Len,uint16_t limitLen)
{
    uint16_t k=0;
    int i;
    uint16_t unic;
    if(unicode==NULL||utf8==NULL||unicodeLen==0)
    {
        *uft8Len = 0;
        return;
    }
    for(i=0; i<unicodeLen/2; i++)
    {
        unic = unicode[2*i]<<8|unicode[2*i+1];
        if((unic==0xa0)||(unic<0x20))
        {
            unic = 0x20;
        }
        if(unic<=0x007f)
        {
            if(k+1>limitLen) break;
            utf8[k] = (unic&0x7f);
            k = k+1;
        }
        else if((unic>=0x0080)&&(unic<=0x07ff))
        {
            if(k+2>limitLen) break;
            utf8[k] = ((unic >> 6) & 0x1F) | 0xC0;
            utf8[k+1] = (unic & 0x3F) | 0x80;
            k = k+2;
        }
        else if((unic>=0x0800)&&(unic<=0xffff))
        {
            if(k+3>limitLen) break;
            utf8[k] = ((unic >>12) & 0x0F) | 0xE0;
            utf8[k+1] = ((unic >>6) & 0x3F) | 0x80;
            utf8[k+2] = (unic & 0x3F) | 0x80;
            k = k+3;
        }
        /*  // unicode ucs-4
        else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )
             {
                     // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                     *(utf8+3) = (unic & 0x3F) | 0x80;
                     *(utf8+2) = ((unic >>  6) & 0x3F) | 0x80;
                     *(utf8+1) = ((unic >> 12) & 0x3F) | 0x80;
                     *utf8      = ((unic >> 18) & 0x07) | 0xF0;
                     return 4;
             }
             else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )
             {
                     // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                     *(utf8+4) = (unic & 0x3F) | 0x80;
                     *(utf8+3) = ((unic >>  6) & 0x3F) | 0x80;
                     *(utf8+2) = ((unic >> 12) & 0x3F) | 0x80;
                     *(utf8+1) = ((unic >> 18) & 0x3F) | 0x80;
                     *utf8      = ((unic >> 24) & 0x03) | 0xF8;
                     return 5;
             }
             else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )
             {
                     // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
                     *(utf8+5) = (unic & 0x3F) | 0x80;
                     *(utf8+4) = ((unic >>  6) & 0x3F) | 0x80;
                     *(utf8+3) = ((unic >> 12) & 0x3F) | 0x80;
                     *(utf8+2) = ((unic >> 18) & 0x3F) | 0x80;
                     *(utf8+1) = ((unic >> 24) & 0x3F) | 0x80;
                     *utf8      = ((unic >> 30) & 0x01) | 0xFC;
                     return 6;
             }
        */
    }
    *uft8Len = k;
}
/**
*@brief      获取utf8编码占用字节数
*@details
*@param[in]   utf8 编码的第一个byte值
*@author        zn.zeng
*@date        2021-08-24
*/
int uteModuleCharencodeGetUtf8ByteNum(uint8_t firstCh)
{
    uint8_t temp = 0x80;
    int num = 0;

    while (temp & firstCh)
    {
        num++;
        temp = (temp >> 1);
    }
    return num;
}

/**
*@brief      剪切 utf8 字节,防止剪切字符不完整导致不显示或者乱码
*@details   字集 utf8
*@param[in] uint8_t *input_utf8 输入的数据指针
*@param[in] uint16_t *len 输入需要剪切的字节长度(输入长度必现大于6个字节)
*@param[out] uint8_t *output_utf8 输出的数据指针
*@param[out] uint16_t utf8_len 输出剪切后的数据长度
*@author        lkl
*@date        2023-06-14
*/
bool uteModuleCharencodeGetUtf8String(uint8_t*input_utf8,int16_t len,uint8_t*output_utf8,uint16_t *utf8_len)
{
    if(input_utf8==NULL  ) //字符串为NULL 直接返回
    {
        return false;
    }

    int16_t new_utf8_len = len; //赋初值
    if(len<6)
    {
        len = 0;
    }
    else
    {
        len = len-6; //UTF_8 最大是6个字节确保
    }

    int8_t num = 0;

    while(new_utf8_len>len)
    {
        num = uteModuleCharencodeGetUtf8ByteNum(input_utf8[new_utf8_len]);
        if(num==0)//特殊字符
        {
            break;
        }
        else if(num==1)
        {
            new_utf8_len -= 1;
        }
        else //完整字符 2/3/4/5/6
        {
            break;
        }

    }
    //   UTE_MODULE_LOG(UTE_LOG_NOTIFY_LVL, "%s,new_utf8_len=[%d]", __func__,new_utf8_len);



    memcpy(output_utf8,input_utf8,new_utf8_len);
    *utf8_len = new_utf8_len;

    // strncpy(&output_utf8[new_utf8_len],"...",3);
    // *utf8_len = new_utf8_len+3;
//    memcpy(&output_utf8[new_utf8_len],"\x2e\x2e\x2e\x1e\x1e\x1e\x00",7);
    // memcpy(&output_utf8[new_utf8_len],"\x2e\x2e\x2e\x00",4);
    // *utf8_len = new_utf8_len+4;


    // UTE_MODULE_LOG_BUFF(UTE_LOG_NOTIFY_LVL,new_utf8,new_utf8_len);
    return true;
}

/**
*@brief      utf8 转换成 unicode
*@details   字集 unicode ucs-2
*@param[in] uint8_t *unicode 输出的数据指针
*@param[in] uint16_t *unicodeLen 输出的数据长度，输入unicode 缓存的最大长度byte
*@param[out] uint8_t *utf8 输入的数据指针
*@param[out] uint16_t uft8Len 输入的数据长度 byte
*@author        zn.zeng
*@date        2021-08-24
*/
void uteModuleCharencodeUtf8ConversionUnicode(uint8_t *unicode,uint16_t *unicodeLen,uint8_t *utf8,uint16_t uft8Len)
{
    uint16_t k = 0;
    uint16_t tmpUnicode = 0;
    int num=0;
//     UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,&utf8[0],uft8Len);
    for (uint32_t i = 0; i < uft8Len; i++)
    {
        num = uteModuleCharencodeGetUtf8ByteNum((uint8_t)utf8[i]);
        switch (num)
        {
            case 0:
            {
                tmpUnicode = *(utf8 + i);
                unicode[k] = tmpUnicode>>8&0xff;
                unicode[k+1] = tmpUnicode&0xff;
                k=k+2;
                break;
            }
            case 1:
            {
                //TODO
                //can not enter here
                //k++;
                break;
            }
            case 2:
            {
                uint8_t b1,b2;
                b1 = utf8[i];
                b2 = utf8[i+1];
                if ( (b2 & 0xc0) != 0x80 )
                {
                    i = i + 1;
                    break;
                }
                tmpUnicode = (((b1 << 6) + (b2 & 0x3F)) & 0xff) | (((b1 >> 2) & 0x07) << 8);
                unicode[k] = tmpUnicode>>8&0xff;
                unicode[k+1] = tmpUnicode&0xff;
                k=k+2;
                i = i + 1;
                break;
            }
            case 3:
            {
                tmpUnicode = ((utf8[i + 1] & 0x03) << 6) + (utf8[i + 2] & 0x3F);
                tmpUnicode |= (uint16_t)(((utf8[i] & 0x0F) << 4) | ((utf8[i + 1] >> 2) & 0x0F)) << 8;
                unicode[k] = tmpUnicode>>8&0xff;
                unicode[k+1] = tmpUnicode&0xff;
                k=k+2;
                i = i + 2;
                break;
            }
            case 4:
            {
                //TODO
                //白色方框//0x2b1b//白色方块
                unicode[k] = 0x2b;
                unicode[k+1] = 0x1c;
                k=k+2;
                i = i + 3;
                break;
            }
            case 5:
            {
                //TODO
                //白色方框//0x2b1b//白色方块
                unicode[k] = 0x2b;
                unicode[k+1] = 0x1c;
                k=k+2;
                i = i + 4;
                break;
            }
            case 6:
            {
                //TODO
                //白色方框//0x2b1b//白色方块
                unicode[k] = 0x2b;
                unicode[k+1] = 0x1c;
                k=k+2;
                i = i + 5;
                break;
            }
            default:
                break;
        }
    }
    *unicodeLen = k;
}


/**
*********************************************************
*@name:uteLangageArabConversion
*@param:(uint8_t *buff,uint8_t buffLength,uint8_t *outPutBuff)
uint8_t *buff,输入的字符串
uint8_t buffLength,出入字符串的长度
uint8_t *outPutBuff，转换后的字符串
*@return:void
*@Describe:
*@author://ellison  mark Jan 17, 2019
*@last modify://ellison  mark Jan 17, 2019
*********************************************************
**/
void uteModuleCharencodArabConversion(uint8_t *buff,uint8_t buffLength)
{
    uint16_t tempUnicode = 0x00;
    for(uint8_t j=0; j<buffLength/2/2; j++)
    {
        tempUnicode = *(uint16_t *)&buff[j*2];
        *(uint16_t *)&buff[j*2] = *(uint16_t *)&buff[(buffLength/2-1-j)*2];
        *(uint16_t *)&buff[(buffLength/2-1-j)*2] = tempUnicode;
    }
}

const uint16_t uteModuleArabicPositionList[][4]= // first, last, middle, alone // 0x621 ~0x64A
{
    { 0xfe80, 0xfe80, 0xfe80, 0xfe80}, // 0x621
    { 0xfe82, 0xfe81, 0xfe82, 0xfe81},
    { 0xfe84, 0xfe83, 0xfe84, 0xfe83},
    { 0xfe86, 0xfe85, 0xfe86, 0xfe85},
    { 0xfe88, 0xfe87, 0xfe88, 0xfe87},
    { 0xfe8a, 0xfe8b, 0xfe8c, 0xfe89},
    { 0xfe8e, 0xfe8d, 0xfe8e, 0xfe8d},
    { 0xfe90, 0xfe91, 0xfe92, 0xfe8f},  // 0x628
    { 0xfe94, 0xfe93, 0xfe93, 0xfe93},
    { 0xfe96, 0xfe97, 0xfe98, 0xfe95},  // 0x62A
    { 0xfe9a, 0xfe9b, 0xfe9c, 0xfe99},
    { 0xfe9e, 0xfe9f, 0xfea0, 0xfe9d},
    { 0xfea2, 0xfea3, 0xfea4, 0xfea1},
    { 0xfea6, 0xfea7, 0xfea8, 0xfea5},
    { 0xfeaa, 0xfea9, 0xfeaa, 0xfea9},
    { 0xfeac, 0xfeab, 0xfeac, 0xfeab},  // 0x630
    { 0xfeae, 0xfead, 0xfeae, 0xfead},
    { 0xfeb0, 0xfeaf, 0xfeb0, 0xfeaf},
    { 0xfeb2, 0xfeb3, 0xfeb4, 0xfeb1},
    { 0xfeb6, 0xfeb7, 0xfeb8, 0xfeb5},
    { 0xfeba, 0xfebb, 0xfebc, 0xfeb9},
    { 0xfebe, 0xfebf, 0xfec0, 0xfebd},
    { 0xfec2, 0xfec3, 0xfec4, 0xfec1},
    { 0xfec6, 0xfec7, 0xfec8, 0xfec5},  // 0x638
    { 0xfeca, 0xfecb, 0xfecc, 0xfec9},
    { 0xfece, 0xfecf, 0xfed0, 0xfecd},  //0x63A
    { 0x63b, 0x63b, 0x63b, 0x63b},
    { 0x63c, 0x63c, 0x63c, 0x63c},
    { 0x63d, 0x63d, 0x63d, 0x63d},
    { 0x63e, 0x63e, 0x63e, 0x63e},
    { 0x63f, 0x63f, 0x63f, 0x63f},
    { 0x640, 0x640, 0x640, 0x640},      // 0x640
    { 0xfed2, 0xfed3, 0xfed4, 0xfed1},
    { 0xfed6, 0xfed7, 0xfed8, 0xfed5},
    { 0xfeda, 0xfedb, 0xfedc, 0xfed9},
    { 0xfede, 0xfedf, 0xfee0, 0xfedd},
    { 0xfee2, 0xfee3, 0xfee4, 0xfee1},
    { 0xfee6, 0xfee7, 0xfee8, 0xfee5},
    { 0xfeea, 0xfeeb, 0xfeec, 0xfee9},
    { 0xfeee, 0xfeed, 0xfeee, 0xfeed},  // 0x648
    { 0xfef0, 0xfeef, 0xfef0, 0xfeef},
    { 0xfef2, 0xfef3, 0xfef4, 0xfef1},  // 0x64A
};

const uint16_t uteModuleArabicRuleSet1List[23]=
{
    0x62c, 0x62d, 0x62e, 0x647, 0x639, 0x63a, 0x641, 0x642,
    0x62b, 0x635, 0x636, 0x637, 0x643, 0x645, 0x646, 0x62a,
    0x644, 0x628, 0x64a, 0x633, 0x634, 0x638, 0x626
};

const uint16_t uteModuleArabicRuleSet2List[35]=
{
    0x62c, 0x62d, 0x62e, 0x647, 0x639, 0x63a, 0x641, 0x642,
    0x62b, 0x635, 0x636, 0x637, 0x643, 0x645, 0x646, 0x62a,
    0x644, 0x628, 0x64a, 0x633, 0x634, 0x638, 0x626,
    0x627, 0x623, 0x625, 0x622, 0x62f, 0x630, 0x631, 0x632,
    0x648, 0x624, 0x629, 0x649
};

const uint16_t uteModuleArabicSpecs[][2]=
{
    {0xFEF5, 0xFEF6}, // 0x622
    {0xFEF7, 0xFEF8}, // 0x623
    {0xFEF9, 0xFEFA}, // 0x625
    {0xFEFB, 0xFEFC}, // 0x627
};
bool uteModuleCharencodIsNeedDeformationArabicCode(uint16_t unicode)
{
    bool result =false;
    if((unicode >=0x0621) && (unicode <= 0x064A))
    {
        result =true;
    }
    return result;
}
bool uteModuleCharencodIsArabicUnicodeInSet1(uint16_t unicode)
{
    bool result = false;
    uint8_t i = 0;
    for( i = 0; i < 23; i++)
    {
        if(unicode==uteModuleArabicRuleSet1List[i])
        {
            result =true;
            return result;
        }
    }
    return result;
}

bool uteModuleCharencodIsArabicUnicodeInSet2(uint16_t unicode)
{
    bool result = false;
    for(uint8_t i = 0; i < 35; i++)
    {
        if(unicode==uteModuleArabicRuleSet2List[i])
        {
            result =true;
            return result;
        }
    }
    return result;
}
uint16_t uteModuleCharencodeGetArabicSpeceVaule(uint16_t preUnicode,uint16_t nextUnicode)
{
    uint16_t resultUnicode = 0;
    uint8_t index  = 0;
    if(nextUnicode == 0x0622 || nextUnicode == 0x0623)
    {
        index = nextUnicode - 0x0622;
    }
    else if(nextUnicode == 0x0625)
    {
        index = 2;
    }
    else if(nextUnicode == 0x0627)
    {
        index = 3;
    }
    if(uteModuleCharencodIsArabicUnicodeInSet1(preUnicode))
    {
        resultUnicode = uteModuleArabicSpecs[index][1];
    }
    else
    {
        resultUnicode = uteModuleArabicSpecs[index][0];
    }
    return resultUnicode;
}

uint16_t uteArabicLanguageDeformationRule(uint8_t *buffArabic,uint8_t buffLength,uint8_t *outPutBuff)
{
    uint16_t preUnicode = 0,nextUnicode = 0;
    uint16_t unicode = 0;
    uint16_t len = 0;
    uint16_t tempUnicode = 0;
    bool noDeformationFlag = false; //0x0621~0x64A 之外的字符串
    uint16_t noDeformationStart = 0,noDeformationLength = 0;
    for(uint16_t i = 0; i<buffLength/2; i++)
    {
        uint8_t deformationState = ARABIC_ALONE;
        preUnicode = (i>0)?((buffArabic[(i-1)*2]<<8&0xff00)|buffArabic[(i-1)*2+1]):(0x00);
        nextUnicode =  (i<(buffLength/2-1))?((buffArabic[(i+1)*2]<<8&0xff00)|buffArabic[(i+1)*2+1]):(0x00);
        unicode = (buffArabic[i*2]<<8&0xff00)|buffArabic[i*2+1];
        if(uteModuleCharencodIsNeedDeformationArabicCode(unicode))
        {
            if(noDeformationFlag == true)
            {
                noDeformationFlag = false;
                uteModuleCharencodArabConversion(&outPutBuff[noDeformationStart],noDeformationLength);
                noDeformationLength = 0;
            }
            if(unicode == 0x0644 && (nextUnicode && (nextUnicode==0x0622||nextUnicode==0x0623||nextUnicode==0x0625||nextUnicode==0x0627)))
            {
                tempUnicode = uteModuleCharencodeGetArabicSpeceVaule(preUnicode,nextUnicode);
                i+=1;  //根据规则2,0x0644后跟着的内容会一起合并,所以后面一个不需要判断了
            }
            else
            {
                if(preUnicode && uteModuleCharencodIsArabicUnicodeInSet1(preUnicode))
                {
                    deformationState = ARABIC_FIRST;
                }
                if(nextUnicode && uteModuleCharencodIsArabicUnicodeInSet2(nextUnicode))
                {
                    if(deformationState == ARABIC_FIRST)
                    {
                        deformationState = ARABIC_MIDDLE;
                    }
                    else
                    {
                        deformationState = ARABIC_LAST;
                    }
                }
                tempUnicode = uteModuleArabicPositionList[unicode - 0x0621][deformationState];
            }
            outPutBuff[len] = tempUnicode>>8 & 0xFF;
            outPutBuff[len+1] = tempUnicode&0x00FF;
            len += 2;
        }
        else if((unicode >= 0x0660 && unicode<= 0x0669)||(unicode>=0x000A && unicode<=0x007F)) //阿拉伯语印度文数字是不用反转的。
        {
            if(noDeformationFlag == false)
            {
                noDeformationFlag = true;
                noDeformationStart = len;
                noDeformationLength = 2;
            }
            else
            {
                noDeformationLength += 2;
            }
            outPutBuff[len] = unicode>>8 & 0xFF;
            outPutBuff[len+1] = unicode&0x00FF;
            len += 2;
        }
        else
        {
            if(noDeformationFlag == true)
            {
                noDeformationFlag = false;
                uteModuleCharencodArabConversion(&outPutBuff[noDeformationStart],noDeformationLength);
                noDeformationLength = 0;
            }
            outPutBuff[len] = unicode>>8 & 0xFF;
            outPutBuff[len+1] = unicode&0x00FF;
            len += 2;
        }
    }
    if(noDeformationFlag == true)
    {
        uteModuleCharencodArabConversion(&outPutBuff[noDeformationStart],noDeformationLength);
        noDeformationLength = 0;
    }
    //根据阿拉伯语的显示从右向左,故这里要反转一下数据, 因为上面单独对阿拉伯文数字进行了反转，这里在反转就正过来了
    uteModuleCharencodArabConversion(&outPutBuff[0],len);
    return len;
}
void uteModuleCharencodeLittleBigEndian16BitSwitch(uint8_t *buff,uint32_t byteSize)
{
    uint32_t i=0;
    for(i=0; i<byteSize/2; i++)
    {
        uint8_t tmp = buff[2*i];
        buff[2*i] = buff[2*i+1];
        buff[2*i+1] = tmp;
    }
}

bool uteModuleCharencodeIsArbicUnicode(uint16_t unicode)
{
    bool isAr = false;
#if UTE_THRID_CHAR_ENCODE_ARABIC_SUPPORT
    isAr = uteModuleCharencodeIsArabicChar(unicode);
#else
    if(((unicode<=0x06FF)&&(unicode>=0x0600))||\
       ((unicode<=0xfbff)&&(unicode>=0xfb50))||\
       ((unicode<=0xfeff)&&(unicode>=0xfe70)))
    {
        isAr = true;
    }
#endif
    return isAr;
}

/**
*********************************************************
*@name:uteArabConversion
*@param:void
*@return:void
*@Describe: 转换字符串中的阿拉伯语言，
*@author://ellison  mark Jan 17, 2019
*@last modify://ellison  mark Jan 17, 2019
*********************************************************
**/
bool uteModuleCharencodeArabUnicodeStringConversion(uint16_t *input,uint32_t inputSize,uint16_t *subtmp,uint16_t *output,uint32_t *outputSize)
{
    uint32_t i,offset;
    int ArBicUnicodeStart = -1;
    bool isArBicRightToLeft = false;
    offset = 0;
#if UTE_THRID_CHAR_ENCODE_ARABIC_SUPPORT
    for (i = 0; i < inputSize; i++)
    {
        if (uteModuleCharencodeIsArbicUnicode(input[i]))
        {
            isArBicRightToLeft = true;
        }
        output[i] = input[i];
    }
    *outputSize = inputSize;
#else
    for (i = 0; i < inputSize; i++)
    {
        if (uteModuleCharencodeIsArbicUnicode(input[i]))
        {
            isArBicRightToLeft = true;
            if (ArBicUnicodeStart == -1)
            {
                ArBicUnicodeStart = i;
            }
            else if (i == (inputSize - 1))
            {
                i++;
                goto RE_MEMCPY;
            }
            if (inputSize == 1)
            {
                i++;
                goto RE_MEMCPY;
            }
            else if ((i == (inputSize - 1)) && (inputSize > 1) && (!uteModuleCharencodeIsArbicUnicode(input[i-1])))
            {
                i++;
                goto RE_MEMCPY;
            }
        }
        else
        {
        RE_MEMCPY:
            if (ArBicUnicodeStart == -1)
            {
                output[offset] = input[i];
                offset++;
            }
            else // arbic
            {
                uint16_t arbicSize;
                memcpy(&subtmp[0], &input[ArBicUnicodeStart], (i - ArBicUnicodeStart) * 2);
                uteModuleCharencodeLittleBigEndian16BitSwitch((uint8_t *)&subtmp[0], (i - ArBicUnicodeStart) * 2);
                arbicSize = uteArabicLanguageDeformationRule((uint8_t *)&subtmp[0], (i - ArBicUnicodeStart) * 2, (uint8_t *)&output[offset]);
                uteModuleCharencodArabConversion((uint8_t *)&output[offset], arbicSize);
                uteModuleCharencodeLittleBigEndian16BitSwitch((uint8_t *)&output[offset], arbicSize);
                offset = offset + arbicSize / 2;
                ArBicUnicodeStart = -1;
                if (i < inputSize)
                {
                    goto RE_MEMCPY;
                }
            }
        }
    }
    *outputSize = offset;
#endif
    return isArBicRightToLeft;
}

bool uteModuleCharencodeIsArBicAloneUnicode(uint16_t unicode)
{
    bool isAr = false;
    for(int i=0; i<42; i++)
    {
        if(uteModuleArabicPositionList[i][3]==unicode)
        {
            isAr = true;
            break;
        }
    }
    return isAr;
}
bool uteModuleCharencodeIsPunctuationUnicode(uint16_t unicode)
{
    if(// 中文标点、全角标点
        ((unicode>=0xff01)&&(unicode<=0xff0f))
        ||((unicode>=0xff1a)&&(unicode<=0xff20))
        ||((unicode>=0x2013)&&(unicode<=0x2014))
        ||((unicode>=0x2018)&&(unicode<=0x2019))
        ||((unicode>=0x201c)&&(unicode<=0x201d))
        ||(unicode==0x2026)
        ||((unicode>=0x3001)&&(unicode<=0x3002))
        ||((unicode>=0x3008)&&(unicode<=0x3011))
        ||((unicode>=0x3014)&&(unicode<=0x3015))
        //ascii 标点
        ||((unicode>=0x0020)&&(unicode<=0x002f))
        ||((unicode>=0x003A)&&(unicode<=0x0040))
        ||((unicode>=0x005b)&&(unicode<=0x0060))
        ||((unicode>=0x007b)&&(unicode<=0x007e))
    )
    {
        return true;
    }

    return false;
}

/******
*function: uteModuleCharencodeIsVietnamUnicodeChar
*param:uint16_t unicode
*decs:判断是否是越南语的字符
*******/
static const uint16_t uteModuleCharencodeVietnamUnicodeList[44]=
{
    0x103,0xe2,0x111,0xea,
    0xf4,0x1a1,0x1b0,0x1af,
    0x102,0xc2,0x110,0xca,
    0xd4,0x1a0,0xfd,0xdd,
    0xe0,0xe8,0xec,0xf2,0xf9,
    0xc0,0xc8,0xcc,0xd2,0xd9,
    0xe1,0xe9,0xed,0xf3,0xfa,
    0xc1,0xc9,0xcd,0xd3,0xda,
    0xe3,0x129,0xf5,0x169,
    0xc3,0x128,0xd5,0x168
};
static bool uteModuleCharencodeIsVietnamUnicodeChar(uint16_t unicode)
{
    bool isVietnam =false;
    if( (unicode>=0x1EA0) &&(unicode<=0x1EF9))
    {
        isVietnam =true;
    }
    else
    {
        for (uint8_t index = 0;  index < 44;  index++)
        {
            if (unicode ==uteModuleCharencodeVietnamUnicodeList[index])
            {
                isVietnam =true;
                break;
            }
        }
    }
    return isVietnam;
}

bool uteModuleCharencodeIsOneByteUnicodeChar(uint16_t unicode)
{
    bool result =false;
    if((unicode<0x7f)//C0控制符及基本拉丁文
       ||((unicode<0x97f)&&(unicode>0x900)) // 印度语
       ||((unicode<0x6ff)&&(unicode>0x600))||((unicode<0x77f)&&(unicode>0x750))||((unicode<=0xfefc)&&(unicode>=0xfb50))// 阿拉伯语
       ||((unicode<=0xff)&&(unicode>=0x80))//德语法语--》C1控制拉丁语补充-1
       ||((unicode<=0x17f)&&(unicode>=0x100)) //拉丁文扩展-A
       ||((unicode<=0x24f)&&(unicode>=0x180)) //拉丁文扩展-B
       ||((unicode<=0x2C7E)&&(unicode>=0x2C60))//拉丁文扩展-C
       ||((unicode<=0xa7ff)&&(unicode>=0xa720))//拉丁文扩展-D
       ||((unicode<0x1eff)&&(unicode>=0x1e00))// 拉丁文扩充附加
       ||((unicode<=0x52f)&&(unicode>=0x400)) // 俄语
       ||((unicode<=0x5ff)&&(unicode>=0x590)) // 希伯莱文
       ||(uteModuleCharencodeIsVietnamUnicodeChar(unicode))// 越南语
       ||((unicode<=0x209f)&&(unicode>=0x2070)) // 上标下标
       ||((unicode<=0x03ff)&&(unicode>=0x0370)) // 希腊文及科普特文
       ||((unicode<=0x1fff)&&(unicode>=0x1f00)) // 希腊语扩充
       ||((unicode<=0x0e7f)&&(unicode>=0x0e00)) // 泰文 (Thai)
       ||(unicode == 0x0323) //i?
       ||(unicode==0x0530) //?(0x0530)
      )
    {
        result =true;
    }
    return result;
}
#if UTE_THRID_WU_CHAR_ENCODE_COMMON_SUPPORT
sbn_glyph_t lastThridCharInfo;
/**
*@brief        获取泰文字库信息
*@details
*@param[in] uint16_t unicode,读取的编码
*@param[in] uint8_t enlargement ,放大倍数，为2时使用32x32,其他数值使用16x16
*@param[out] sbn_glyph_t *charInfo,读取到的字符信息
*@return bool true 为有匹配数据
*@author       zn.zeng
*@date       2022-10-21
*/
bool uteModuleCharencodeThridWuReadEncodeCharInfo(uint16_t unicode,uint8_t enlargement,sbn_glyph_t *charInfo)
{
    bool isHasChar = false;
    GUI_FONT_HEAD_NF *headInfo;
    // uint8_t g_magic[5];
    // memset(g_magic, 0, 5);
    if (0)
    {
    }
#if UTE_THRID_CHAR_ENCODE_THAI_SUPPORT
    else if (uteModuleCharencodeIsThaiChar(unicode))
    {
        charInfo->font16x16Address = UTE_THRID_CHAR_ENCODE_THAI_16X16_ADDRESS;
        charInfo->font24x24Address = UTE_THRID_CHAR_ENCODE_THAI_24X24_ADDRESS;
        charInfo->font32x32Address = UTE_THRID_CHAR_ENCODE_THAI_32X32_ADDRESS;
    }
#endif
#if UTE_THRID_CHAR_ENCODE_BENGALI_SUPPORT
    else if (uteModuleCharencodeIsBengaliChar(unicode))
    {
        charInfo->font16x16Address = UTE_THRID_CHAR_ENCODE_BENGALI_16X16_ADDRESS;
        charInfo->font24x24Address = UTE_THRID_CHAR_ENCODE_BENGALI_24X24_ADDRESS;
        charInfo->font32x32Address = UTE_THRID_CHAR_ENCODE_BENGALI_32X32_ADDRESS;
    }
#endif
#if UTE_THRID_CHAR_ENCODE_HEBREW_SUPPORT
    else if (uteModuleCharencodeIsHebrewChar(unicode))
    {
        charInfo->font16x16Address = UTE_THRID_CHAR_ENCODE_HEBREW_16X16_ADDRESS;
        charInfo->font24x24Address = UTE_THRID_CHAR_ENCODE_HEBREW_24X24_ADDRESS;
        charInfo->font32x32Address = UTE_THRID_CHAR_ENCODE_HEBREW_32X32_ADDRESS;
    }
#endif
#if UTE_THRID_CHAR_ENCODE_ARABIC_SUPPORT
    else if (uteModuleCharencodeIsArabicChar(unicode))
    {
        charInfo->font16x16Address = UTE_THRID_CHAR_ENCODE_ARABIC_16X16_ADDRESS;
        charInfo->font24x24Address = UTE_THRID_CHAR_ENCODE_ARABIC_24X24_ADDRESS;
        charInfo->font32x32Address = UTE_THRID_CHAR_ENCODE_ARABIC_32X32_ADDRESS;
    }
#endif
#if UTE_THRID_CHAR_ENCODE_DEVANAGARI_SUPPORT
    else if (uteModuleCharencodeIsDevanagariChar(unicode))
    {
        charInfo->font16x16Address = UTE_THRID_CHAR_ENCODE_DEVANAGARI_16X16_ADDRESS;
        charInfo->font24x24Address = UTE_THRID_CHAR_ENCODE_DEVANAGARI_24X24_ADDRESS;
        charInfo->font32x32Address = UTE_THRID_CHAR_ENCODE_DEVANAGARI_32X32_ADDRESS;
    }
#endif
    else
    {
        return isHasChar;
    }
    uint32_t baseAddress = charInfo->font16x16Address;
    if (enlargement == 2)
    {
        baseAddress = charInfo->font32x32Address;
    }
    // UTE_MODULE_LOG(1,"%s,baseAddress=%x",__func__,baseAddress);
    // memcpy(g_magic, (const void *)baseAddress, 4);
    headInfo = (GUI_FONT_HEAD_NF *)(baseAddress + 4);
    // UTE_MODULE_LOG(1,"%s,unicode=0x%x,nSection=%d,font_ascent=%d,font_descent=%d",__func__,unicode,headInfo->nSection,headInfo->font_ascent,headInfo->font_descent);
    for (uint8_t i = 0; i < headInfo->nSection; i++)
    {
        FL_SECTION_INF *sectionInfo = (FL_SECTION_INF *)(baseAddress + 4 + sizeof(GUI_FONT_HEAD_NF) + i * sizeof(FL_SECTION_INF));
        // UTE_MODULE_LOG(1,"%s,i=%d,First=0x%x,Last=0x%x,OffAddr=0x%x",__func__,i,sectionInfo->First,sectionInfo->Last,sectionInfo->OffAddr);
        if ((unicode >= sectionInfo->First) && (unicode <= sectionInfo->Last))
        {
            uint32_t charAddressOffset = 0;
            memcpy(&charAddressOffset, (const void *)(baseAddress + sectionInfo->OffAddr + FONT_INDEX_TAB_SIZE * (unicode - sectionInfo->First)), 4);
            uint8_t charInfoBuff[SBN_GLYPH_HEAD_LEN];
            memcpy(charInfoBuff, (const void *)(charAddressOffset + baseAddress), SBN_GLYPH_HEAD_LEN);
            charInfo->bbx.width = charInfoBuff[0];
            charInfo->bbx.height = charInfoBuff[1];
            charInfo->bbx.x_offset = charInfoBuff[2];
            charInfo->bbx.y_offset = charInfoBuff[3];
            charInfo->dwidth = charInfoBuff[4];
            charInfo->bytes = charInfoBuff[ID_SBN_GLYPH_BYTES_LO] | charInfoBuff[ID_SBN_GLYPH_BYTES_HI] << 8;
            charInfo->nBytesPerLine = (charInfoBuff[0] + 7) / 8;
            charInfo->font_ascent = headInfo->font_ascent;
            charInfo->bbxHeight = headInfo->bbx.height;
            charInfo->unicode = unicode;
            // 字符坐标微调
            if ((charInfo->dwidth == 0) && (lastThridCharInfo.dwidth != 0))
            {
                int aligns = BE_ALIGN_NULL;
                if (0)
                {
                }
#if UTE_THRID_CHAR_ENCODE_BENGALI_SUPPORT
                else if (uteModuleCharencodeIsBengaliChar(unicode))
                {
                    aligns = Bengali_Noto_Repositioning(lastThridCharInfo.unicode, charInfo->unicode);
                }
#endif
#if UTE_THRID_CHAR_ENCODE_HEBREW_SUPPORT
                else if (uteModuleCharencodeIsHebrewChar(unicode))
                {
                    aligns = Hebrew_Noto_Repositioning(lastThridCharInfo.unicode, charInfo->unicode);
                }
#endif
#if UTE_THRID_CHAR_ENCODE_DEVANAGARI_SUPPORT
                else if (uteModuleCharencodeIsDevanagariChar(unicode))
                {
                    aligns = Deva_Noto_Repositioning(lastThridCharInfo.unicode, charInfo->unicode);
                }
#endif
                if (aligns == BE_ALIGN_CENTER)
                {
                    charInfo->bbx.x_offset = -(lastThridCharInfo.dwidth+charInfo->bbx.width)/2;
                }
                else if(aligns == BE_ALIGN_LEFT)
                {
                    charInfo->bbx.x_offset = -lastThridCharInfo.dwidth;
                }
                else if(aligns == BE_ALIGN_RIGHT)
                {
                    charInfo->bbx.x_offset = -charInfo->bbx.width;
                }
                else if(aligns == BE_ALIGN_LOWER)
                {

                }
            }
            charInfo->dotMatrixDataAddress = (charAddressOffset + baseAddress + SBN_GLYPH_HEAD_LEN);
            // UTE_MODULE_LOG(1,"%s,charInfo->dotMatrixDataAddress=0x%x",__func__,charInfo->dotMatrixDataAddress);
            memcpy(&lastThridCharInfo,charInfo,sizeof(sbn_glyph_t));
            isHasChar = true;
            break;
        }
    }
    return isHasChar;
}
/**
*@brief        处理数据读取到的buff
*@details
*@param[in] uint8_t *buffOutput,输出buff
*@param[in] sbn_glyph_t *charInfo,读取到的字符信息
*@author       zn.zeng
*@date       2022-10-26
*/
void uteModuleCharencodeThridWuFillCharBuff(uint8_t *buffOutput,sbn_glyph_t *charInfo)
{
    uint8_t startLine = charInfo->font_ascent - charInfo->bbx.height - charInfo->bbx.y_offset;
    // if(charInfo->unicode==0x5d9)
    // {
    //     UTE_MODULE_LOG(1, "%s,startLine=0x%x,bbxHeight=%d", __func__, startLine,charInfo->bbxHeight);
    //     UTE_MODULE_LOG(1, "%s,charInfo->nBytesPerLine=%d", __func__, charInfo->nBytesPerLine);
    //     UTE_MODULE_LOG(1, "%s,charInfo->bytes=%d", __func__, charInfo->bytes);
    // }
    // UTE_MODULE_LOG(1, "%s,startLine=0x%x,bbxHeight=%d", __func__, startLine,charInfo->bbxHeight);
    uint8_t readBuff[288];
    memset(readBuff,0,288);
    if(charInfo->bytes>288)
    {
        return;
    }
    memcpy(readBuff,(void *)charInfo->dotMatrixDataAddress,charInfo->bytes);
    for (uint8_t i = startLine; i < charInfo->bbxHeight; i++)
    {
        for(uint8_t j = 0; j<charInfo->nBytesPerLine; j++)
        {
            buffOutput[charInfo->nBytesPerLine*i+j] = readBuff[charInfo->nBytesPerLine*(i - startLine)+j];
        }
    }
}
#endif
#if UTE_THRID_CHAR_ENCODE_THAI_SUPPORT
/**
*@brief        获取泰文字库信息
*@details
*@author       zn.zeng
*@date       2022-10-21
*/
bool uteModuleCharencodeIsThaiChar(uint16_t unicode)
{
    // if (Thai_Arialuni_IsString((const WORD *)unicode))
    if(((unicode<=0x0e7f)&&(unicode>=0x0e00))
       ||((unicode<=0x2014)&&(unicode>=0x2013))
       ||((unicode<=0x2019)&&(unicode>=0x2018))
       ||((unicode<=0x201d)&&(unicode>=0x201c))
       ||(unicode==0x2022)
       ||(unicode==0x2026)
       ||(unicode==0x20ac)
       ||((unicode<=0xf71d)&&(unicode>=0xf700))
      )
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief        泰文字符串转换
*@details
*@param[inout] uint16_t *input 输入输出字符串指针，unicode编码
*@param[out] uint32_t *inputSize，转换后的长度
*@author       zn.zeng
*@date       2022-10-27
*/
void uteModuleCharencodeThaiUnicodeStringConversion(uint16_t *input,uint32_t *inputSize)
{
    uint16_t tmpBuff[(UTE_DISPLAY_STRING_BUFF_MAX_SIZE+2) / 2];
    memset(tmpBuff,0,(UTE_DISPLAY_STRING_BUFF_MAX_SIZE+2));
    memcpy(tmpBuff,input,*inputSize*2);
    Thai_Arialuni_Convert(tmpBuff,input);
    *inputSize = wcsLen(input);
}
#endif
#if UTE_THRID_CHAR_ENCODE_BENGALI_SUPPORT
/**
*@brief        获取孟加拉语字库信息
*@details
*@author       zn.zeng
*@date       2022-11-16
*/
bool uteModuleCharencodeIsBengaliChar(uint16_t unicode)
{
    if(((unicode>=0x0980)&&(unicode<=0x09ff))||
       ((unicode>=0xE100)&&(unicode<=0xE2DC)))
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief        孟加拉语字符串转换
*@details
*@param[inout] uint16_t *input 输入输出字符串指针，unicode编码
*@param[out] uint32_t *inputSize，转换后的长度
*@author       zn.zeng
*@date       2022-11-16
*/
void uteModuleCharencodeBengaliUnicodeStringConversion(uint16_t *input,uint32_t *inputSize)
{
    // UTE_MODULE_LOG(1, "%s,*inputSize=%d", __func__,*inputSize);
    // UTE_MODULE_LOG_BUFF(1, input, *inputSize*2);
    uint16_t tmpBuff[(UTE_DISPLAY_STRING_BUFF_MAX_SIZE+2) / 2];
    memset(tmpBuff, 0, (UTE_DISPLAY_STRING_BUFF_MAX_SIZE+2));
    memcpy(tmpBuff, input, *inputSize * 2);

    Bengali_Noto_Convert(tmpBuff, input);
    *inputSize = wcsLen(input);

    // UTE_MODULE_LOG_BUFF(1, input, *inputSize*2);
}
#endif
#if UTE_THRID_CHAR_ENCODE_HEBREW_SUPPORT
/**
*@brief        获取希伯来语字库信息
*@details
*@author       zn.zeng
*@date       2022-12-07
*/
bool uteModuleCharencodeIsHebrewChar(uint16_t unicode)
{
    if(((unicode>=0x0591)&&(unicode<=0x05ff))||
       ((unicode>=0x200c)&&(unicode<=0x2010))||
       ((unicode>=0x20aa)&&(unicode<=0x20aa))||
       ((unicode>=0x25cc)&&(unicode<=0x25cc))||
       ((unicode>=0xf7f0)&&(unicode<=0xf7f2))||
       ((unicode>=0xfb1d)&&(unicode<=0xfb4f))
      )
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief        希伯来语语字符串转换
*@details
*@param[inout] uint16_t *input 输入输出字符串指针，unicode编码
*@param[out] uint32_t *inputSize，转换后的长度
*@return bool ,是否包含希伯来语
*@author       zn.zeng
*@date       2022-12-07
*/
bool uteModuleCharencodeHebrewUnicodeStringConversion(uint16_t *input,uint32_t *inputSize)
{
    // UTE_MODULE_LOG(1, "%s,*inputSize=%d", __func__,*inputSize);
    // UTE_MODULE_LOG_BUFF(1, input, *inputSize*2);
    bool isHasHebrew = false;
    uint16_t tmpBuff[(UTE_DISPLAY_STRING_BUFF_MAX_SIZE+2) / 2];
    memset(tmpBuff, 0, (UTE_DISPLAY_STRING_BUFF_MAX_SIZE+2));
    memcpy(tmpBuff, input, *inputSize * 2);
    Hebrew_Noto_Convert(tmpBuff, input);
    *inputSize = wcsLen(input);
    // UTE_MODULE_LOG_BUFF(1, input, *inputSize*2);
    for(uint32_t i=0; i<*inputSize; i++)
    {
        if (uteModuleCharencodeIsHebrewChar(input[i]))
        {
            isHasHebrew = true;
        }
    }
    return isHasHebrew;
}
#endif
#if UTE_THRID_CHAR_ENCODE_ARABIC_SUPPORT
/**
*@brief        获取阿拉伯语字库信息
*@details
*@author       zn.zeng
*@date       2022-12-07
*/
bool uteModuleCharencodeIsArabicChar(uint16_t unicode)
{
    if (((unicode >= 0x0600) && (unicode <= 0x6ff)) ||
        ((unicode >= 0xfb50) && (unicode <= 0xfefc)))
    {
        if (((unicode >= 0xFDFE) && (unicode <= 0xFE6F)) ||
            ((unicode >= 0xfDC8) && (unicode <= 0xfDEF))||
            ((unicode >= 0xFBC2) && (unicode <= 0xFBD2))||
            ((unicode >= 0xFD40) && (unicode <= 0xFD4F)))
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief        阿拉伯语波斯语字符串转换
*@details
*@param[inout] uint16_t *input 输入输出字符串指针，unicode编码
*@param[out] uint32_t *inputSize，转换后的长度
*@author       zn.zeng
*@date       2022-12-07
*/
void uteModuleCharencodeArabicUnicodeStringConversion(uint16_t *input,uint32_t *inputSize)
{
    // UTE_MODULE_LOG(1, "%s,*inputSize=%d", __func__,*inputSize);
    // UTE_MODULE_LOG_BUFF(1, input, *inputSize*2);
    uint16_t tmpBuff[(UTE_DISPLAY_STRING_BUFF_MAX_SIZE+2) / 2];
    memset(tmpBuff, 0, (UTE_DISPLAY_STRING_BUFF_MAX_SIZE+2));
    memcpy(tmpBuff, input, *inputSize * 2);
    Arabic_Noto_Convert(tmpBuff, input,1);
    *inputSize = wcsLen(input);
    // UTE_MODULE_LOG_BUFF(1, input, *inputSize*2);
}
#endif
#if UTE_THRID_CHAR_ENCODE_DEVANAGARI_SUPPORT
bool uteModuleCharencodeIsDevanagariChar(uint16_t unicode)
{
    if ((unicode >= 0x0900) && (unicode <= 0x97f) ||
        (unicode >= 0xe300) && (unicode <= 0xe58c))
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool uteModuleCharencodeDevanagariUnicodeStringConversion(uint16_t *input,uint32_t *inputSize)
{
    // UTE_MODULE_LOG(1, "%s,*inputSize=%d", __func__,*inputSize);
    // UTE_MODULE_LOG_BUFF(1, input, *inputSize*2);
    bool isHasDevanagari = false;
    uint16_t tmpBuff[(UTE_DISPLAY_STRING_BUFF_MAX_SIZE + 2) / 2];
    memset(tmpBuff, 0, (UTE_DISPLAY_STRING_BUFF_MAX_SIZE + 2));
    memcpy(tmpBuff, input, *inputSize * 2);
    Deva_Convert(tmpBuff, input);
    *inputSize = wcsLen(input);
    // UTE_MODULE_LOG_BUFF(1, input, *inputSize*2);
    for (uint32_t i = 0; i < *inputSize; i++)
    {
        if (uteModuleCharencodeIsDevanagariChar(input[i]))
        {
            isHasDevanagari = true;
        }
    }
    return isHasDevanagari;
}
#endif

/**
 * @brief        替换子字符串
 * @details
 * @param[in]    original 原始字符串
 * @param[in]    replace 需要替换的原始子字符串
 * @param[in]    replacement 替换后的子字符串
 * @param[out]   result 替换后的字符串
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-12-19
 */
void uteModuleCharencodeReplaceSubString(const char *original, char *result, char *replace, char *replacement)
{
    const char *pos = original; // 用于遍历原始字符串
    uint8_t replaceLen = strlen(replace);
    uint8_t replacementLen = strlen(replacement);

    // 如果 replacement 长度超过 replace 长度，则截断 replacement
    if (replacementLen > replaceLen)
    {
        replacement[replaceLen] = '\0';
        replacementLen = replaceLen;
    }

    // 清空结果字符串
    result[0] = '\0';

    while ((pos = strstr(pos, replace)) != NULL)
    {
        // 复制替换子字符串之前的部分
        strncat(result, original, pos - original);
        // 复制替换的子字符串
        strcat(result, replacement);
        // 更新原始字符串指针
        pos += replaceLen;
        original = pos;
    }

    // 复制剩余部分
    strcat(result, original);
}