#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>

using namespace std;

typedef struct tag_map
{
    const uint8_t  keyLen; // Key长度
    const uint16_t *pKey; // Unicode序列
    const uint16_t *pValue; // 变形后的自定义字符的unicode组合, 0 结尾
} TAG_MAP;

const static TAG_MAP s_cvt_map_km[] =
{
    {
        7, (const uint16_t[]) { 0x1784, 0x17D2, 0x1782, 0x17D2, 0x179A, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE22B, 0xE005, 0x17C7, 0
        }
    }, // ង្គ្រោះ
    {
        7, (const uint16_t[]) { 0x17A0, 0x17CA, 0x17D2, 0x179C, 0x17D2, 0x179A, 0x17B8 }, (const uint16_t[])
        {
            0xE0F5, 0x17B8, 0xE020, 0
        }
    }, // ហ៊្វ្រី
    {
        6, (const uint16_t[]) { 0x17A0, 0x17D2, 0x179C, 0x17D2, 0x179A, 0x17B8 }, (const uint16_t[])
        {
            0xE0F5, 0x17B8, 0xE020, 0
        }
    }, // ហ្វ្រី
    {
        6, (const uint16_t[]) { 0x1784, 0x17D2, 0x1780, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE205, 0xE003, 0
        }
    }, // ង្ក្រា
    {
        6, (const uint16_t[]) { 0x1784, 0x17D2, 0x1782, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE205, 0xE005, 0
        }
    }, // ង្គ្រា
    {
        6, (const uint16_t[]) { 0x1793, 0x17D2, 0x179A, 0x17D2, 0x178F, 0x17B8 }, (const uint16_t[])
        {
            0xE0F8, 0x17B8, 0xE013, 0
        }
    }, // ន្រ្តី
    {
        6, (const uint16_t[]) { 0x1793, 0x17D2, 0x178F, 0x17D2, 0x179A, 0x17B8 }, (const uint16_t[])
        {
            0xE0F8, 0x17B8, 0xE013, 0
        }
    }, // ន្ត្រី
    {
        6, (const uint16_t[]) { 0x1798, 0x17D2, 0x179B, 0x17C9, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE1C2, 0x17C9, 0xE01F, 0x17C7, 0
        }
    }, // ម្ល៉ោះ
    {
        6, (const uint16_t[]) { 0x179F, 0x17D2, 0x178F, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE202, 0xE00E, 0
        }
    }, // ស្ត្រា
    {
        6, (const uint16_t[]) { 0x179F, 0x17D2, 0x178F, 0x17D2, 0x179A, 0x17C1 }, (const uint16_t[])
        {
            0xE109, 0xE04D, 0
        }
    }, // ស្ត្រេ
    {
        6, (const uint16_t[]) { 0x179F, 0x17D2, 0x179A, 0x17D2, 0x1782, 0x17B8 }, (const uint16_t[])
        {
            0xE0F4, 0x17B8, 0xE005, 0
        }
    }, // ស្រ្គី
    {
        6, (const uint16_t[]) { 0x179F, 0x17D2, 0x179A, 0x17D2, 0x178F, 0x17B8 }, (const uint16_t[])
        {
            0xE0F4, 0x17B8, 0xE04D, 0
        }
    }, // ស្រ្តី
    {
        6, (const uint16_t[]) { 0x17A0, 0x17D2, 0x179A, 0x17D2, 0x179C, 0x17B8 }, (const uint16_t[])
        {
            0xE0F5, 0x17B8, 0xE020, 0
        }
    }, // ហ្រ្វី

    {
        5, (const uint16_t[]) { 0x1781, 0x17D2, 0x1785, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE1BB, 0xE008, 0x17C7, 0
        }
    }, // ខ្ចោះ
    {
        5, (const uint16_t[]) { 0x1781, 0x17D2, 0x1789, 0x17BB, 0x17C6 }, (const uint16_t[])
        {
            0x1781, 0x17C6, 0xE00C, 0xE024, 0
        }
    }, // ខ្ញុំ
    {
        5, (const uint16_t[]) { 0x1781, 0x17D2, 0x1798, 0x17B6, 0x17C6 }, (const uint16_t[])
        {
            0xE14F, 0x17C6, 0xE057, 0
        }
    }, // ខ្មាំ
    {
        5, (const uint16_t[]) { 0x1782, 0x17D2, 0x179A, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE208, 0x17C7, 0
        }
    }, // គ្រោះ
    {
        5, (const uint16_t[]) { 0x1783, 0x17d2, 0x1798, 0x17bb, 0x17c6 }, (const uint16_t[])
        {
            0x1783, 0x17C6, 0xE230, 0xE231, 0
        }
    }, // ឃ្មុំ
    {
        5, (const uint16_t[]) { 0x1784, 0x17D2, 0x1780, 0x17C1, 0x17C7 }, (const uint16_t[])
        {
            0xE082, 0xE01B, 0x17C7, 0
        }
    }, // ង្កេះ
    {
        5, (const uint16_t[]) { 0x1785, 0x17D2, 0x1793, 0x17C1, 0x17C7 }, (const uint16_t[])
        {
            0xE07D, 0xE053, 0x17C7, 0
        }
    }, // ច្នេះ
    {
        5, (const uint16_t[]) { 0x1785, 0x17D2, 0x179A, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE20A, 0x17C7, 0
        }
    }, // ច្រោះ
    {
        5, (const uint16_t[]) { 0x1786, 0x17D2, 0x1780, 0x17B9, 0x17C7 }, (const uint16_t[])
        {
            0x1786, 0x17B8, 0xE003, 0x17C7, 0
        }
    }, // ឆ្កឹះ
    {
        5, (const uint16_t[]) { 0x1786, 0x17D2, 0x1793, 0x17B6, 0x17C6 }, (const uint16_t[])
        {
            0xE154, 0x17C6, 0xE053, 0
        }
    }, // ឆ្នាំ
    {
        5, (const uint16_t[]) { 0x1788, 0x17D2, 0x1798, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE1B2, 0xE057, 0x17C7, 0
        }
    }, // ឈ្មោះ
    {
        5, (const uint16_t[]) { 0x178E, 0x17D2, 0x178E, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE086, 0xE012, 0x17B6, 0x17C7, 0
        }
    }, // ណ្ណោះ
    {
        5, (const uint16_t[]) { 0x178E, 0x17D2, 0x178F, 0x17BB, 0x17C7 }, (const uint16_t[])
        {
            0x178E, 0xE00E, 0x17BB, 0x17C7, 0
        }
    }, // ណ្តុះ
    {
        5, (const uint16_t[]) { 0x178E, 0x17D2, 0x178F, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE086, 0xE013, 0x17B6, 0x17C7, 0
        }
    }, // ណ្តោះ
    {
        5, (const uint16_t[]) { 0x1793, 0x17d2, 0x179b, 0x17b9, 0x17c7 }, (const uint16_t[])
        {
            0x1793, 0xE029, 0xE070, 0x17c7, 0
        }
    }, // ន្លឹះ
    {
        5, (const uint16_t[]) { 0x1794, 0x17CA, 0x17D2, 0x179B, 0x17BC }, (const uint16_t[])
        {
            0x1794, 0x17CA, 0xE01F, 0x17BC, 0
        }
    }, // ប៊្លូ
    {
        5, (const uint16_t[]) { 0x1795, 0x17D2, 0x178F, 0x17BB, 0x17C6 }, (const uint16_t[])
        {
            0x1795, 0x17C6, 0xE00E, 0x17BB, 0
        }
    }, // ផ្តុំ
    {
        5, (const uint16_t[]) { 0x1795, 0x17D2, 0x1791, 0x17BB, 0x17C6 }, (const uint16_t[])
        {
            0x1795, 0x17C6, 0xE015, 0x17BB, 0
        }
    }, // ផ្ទុំ
    {
        5, (const uint16_t[]) { 0x1796, 0x17D2, 0x1799, 0x17BB, 0x17C7 }, (const uint16_t[])
        {
            0x1796, 0xE01D, 0x17BB, 0x17C7, 0
        }
    }, // ព្យុះ
    {
        5, (const uint16_t[]) { 0x1798, 0x17D2, 0x179A, 0x17BB, 0x17C7 }, (const uint16_t[])
        {
            0xE0F2, 0x17BB, 0x17C7, 0
        }
    }, // ម្រុះ
    {
        5, (const uint16_t[]) { 0x1798, 0x17D2, 0x179B, 0x17B6, 0x17C6 }, (const uint16_t[])
        {
            0xE166, 0x17C6, 0xE01F, 0
        }
    }, // ម្លាំ
    {
        5, (const uint16_t[]) { 0x179A, 0x17D2, 0x178A, 0x17B7, 0x17CD }, (const uint16_t[])
        {
            0x179A, 0xE033, 0xE00E, 0
        }
    }, // រ្ដិ៍
    {
        5, (const uint16_t[]) { 0x179F, 0x17D2, 0x178A, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE0F4, 0xE00E, 0
        }
    }, // ស្ដ្រ
    {
        5, (const uint16_t[]) { 0x1784, 0x17D2, 0x17A0, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0x17C1, 0xE152, 0xE05A,0x17C7, 0
        }
    }, // ង្ហោ
    {
        5, (const uint16_t[]) { 0x1787, 0x17D2, 0x179A, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE0FD, 0x17C4, 0x17C7, 0
        }
    }, // ជ្រោះ
    {
        5, (const uint16_t[]) { 0x179F, 0x17D2, 0x178F, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE0F4, 0xE00E, 0
        }
    }, // ស្ត្រ
    {
        5, (const uint16_t[]) { 0x179F, 0x17D2, 0x179A, 0x17D2, 0x178F }, (const uint16_t[])
        {
            0xE0F4, 0xE00E, 0
        }
    }, // ស្រ្ត
    {
        5, (const uint16_t[]) { 0x179F, 0x17D2, 0x17A2, 0x17B6, 0x178F }, (const uint16_t[])
        {
            0xE16D, 0xE023, 0x178F, 0
        }
    }, // ស្អាត
    {
        5, (const uint16_t[]) { 0x1795, 0x17D2, 0x179B, 0x17BB, 0x17C6 }, (const uint16_t[])
        {
            0x1795, 0x17C6, 0xE01F, 0x17BB, 0
        }
    }, // ផ្លុំ
    {
        4, (const uint16_t[]) { 0x1780, 0x17d2, 0x178a, 0x17B6 }, (const uint16_t[])
        {
            0xE14E, 0xE00E, 0
        }
    }, // ក្ដា
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x178A, 0x17B7 }, (const uint16_t[])
        {
            0x1780, 0x17B7, 0xE00E, 0
        }
    }, // ក្ដិ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x178A, 0x17B8 }, (const uint16_t[])
        {
            0x1780, 0x17B8, 0xE00E, 0
        }
    }, // ក្ដី
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x178F, 0x17B6 }, (const uint16_t[])
        {
            0xE14E, 0xE00E, 0
        }
    }, // ក្តា
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x178F, 0x17B7 }, (const uint16_t[])
        {
            0x1780, 0x17B7, 0xE00E, 0
        }
    }, // ក្តិ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x178F, 0x17B8 }, (const uint16_t[])
        {
            0x1780, 0x17B8, 0xE00E, 0
        }
    }, // ក្តី
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x178F, 0x17C5 }, (const uint16_t[])
        {
            0xE1CE, 0xE00E, 0
        }
    }, // ក្តៅ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x1793, 0x17BB }, (const uint16_t[])
        {
            0x1780, 0xE017, 0x17BB, 0
        }
    }, // ក្នុ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x1794, 0x17B6 }, (const uint16_t[])
        {
            0x1780, 0xE174, 0
        }
    }, // ក្បា
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x1794, 0x17BC }, (const uint16_t[])
        {
            0x1780, 0xE018, 0xE14B, 0
        }
    }, // ក្បូ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x1794, 0x17C2 }, (const uint16_t[])
        {
            0x17C2, 0x1780, 0xE018, 0
        }
    }, // ក្បែ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x1798, 0x17C1 }, (const uint16_t[])
        {
            0xE078, 0xE057, 0
        }
    }, // ក្មេ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE1F2, 0
        }
    }, // ក្រា
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179A, 0x17BB }, (const uint16_t[])
        {
            0xE0E4, 0x17BB, 0
        }
    }, // ក្រុ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179A, 0x17C4 }, (const uint16_t[])
        {
            0xE0F9, 0x17B6, 0
        }
    }, // ក្រោ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179A, 0x17C4 }, (const uint16_t[])
        {
            0xE207, 0
        }
    }, // ក្រោ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179A, 0x17C5 }, (const uint16_t[])
        {
            0xE0F9, 0xE22F, 0
        }
    }, // ក្រៅ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE14E, 0xE01F, 0
        }
    }, // ក្លា
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179B, 0x17BC }, (const uint16_t[])
        {
            0x1780, 0xE01F, 0x17BC, 0
        }
    }, // ក្លូ
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179C, 0x17B6 }, (const uint16_t[])
        {
            0xE15D, 0xE059, 0
        }
    }, // ក្វា
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179F, 0x17B6 }, (const uint16_t[])
        {
            0x1780, 0xE021, 0x17B6, 0
        }
    }, // ក្សា
    {
        4, (const uint16_t[]) { 0x1780, 0x17D2, 0x179F, 0x17B8 }, (const uint16_t[])
        {
            0x1780, 0xE021, 0x17B8, 0
        }
    }, // ក្សី
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x1787, 0x17B6 }, (const uint16_t[])
        {
            0xE14F, 0xE00A, 0
        }
    }, // ខ្ជា
    {
        4, (const uint16_t[]) {  0x1781, 0x17D2, 0x1793, 0x17B6 }, (const uint16_t[])
        {
            0xE14F, 0xE053, 0
        }
    }, // ខ្ន
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x1798, 0x17C2 }, (const uint16_t[])
        {
            0xE09D, 0xE057, 0
        }
    }, // ខ្មែ
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x1798, 0x17C5 }, (const uint16_t[])
        {
            0xE1CF, 0xE057, 0
        }
    }, // ខ្មៅ
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE14F, 0xE058, 0
        }
    }, // ខ្លា
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x179B, 0x17B8 }, (const uint16_t[])
        {
            0x1781, 0x17B8, 0xE058, 0
        }
    }, // ខ្លី
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x179B, 0x17BD }, (const uint16_t[])
        {
            0x1781, 0xE058, 0xE14C, 0
        }
    }, // ខ្លួ
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x179B, 0x17C2 }, (const uint16_t[])
        {
            0xE09D, 0xE01F, 0
        }
    }, // ខ្លែ
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x179B, 0x17C7 }, (const uint16_t[])
        {
            0x1781, 0xE058, 0x17C7, 0
        }
    }, // ខ្លះ
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x179C, 0x17B6 }, (const uint16_t[])
        {
            0xE14F, 0xE020, 0
        }
    }, // ខ្វា
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x179F, 0x17B6 }, (const uint16_t[])
        {
            0x1781, 0xE176, 0
        }
    }, // ខ្សា
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x179F, 0x17C2 }, (const uint16_t[])
        {
            0x17C2, 0x1781, 0xE021, 0
        }
    }, // ខ្សែ
    {
        4, (const uint16_t[]) { 0x1781, 0x17D2, 0x179F, 0x17C4 }, (const uint16_t[])
        {
            0xE089, 0xE176, 0
        }
    }, // ខ្សោ
    {
        4, (const uint16_t[]) { 0x1782, 0x17D2, 0x1782, 0x17B8 }, (const uint16_t[])
        {
            0x1782, 0x17B8, 0xE005, 0
        }
    }, // គ្គី
    {
        4, (const uint16_t[]) { 0x1782, 0x17D2, 0x1793, 0x17B6 }, (const uint16_t[])
        {
            0xE150, 0xE017, 0
        }
    }, // គ្នា
    {
        4, (const uint16_t[]) { 0x1782, 0x17D2, 0x1793, 0x17B8 }, (const uint16_t[])
        {
            0x1782, 0x17B8, 0xE017, 0
        }
    }, // គ្នី
    {
        4, (const uint16_t[]) { 0x1782, 0x17D2, 0x1798, 0x17B6 }, (const uint16_t[])
        {
            0xE15D, 0xE057, 0
        }
    }, // គ្មា
    {
        4, (const uint16_t[]) { 0x1782, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE1F3, 0
        }
    }, // គ្រា
    {
        4, (const uint16_t[]) { 0x1782, 0x17D2, 0x179B, 0x17C1 }, (const uint16_t[])
        {
            0xE07A, 0xE058, 0
        }
    }, // គ្លេ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x1780, 0x17BE }, (const uint16_t[])
        {
            0xE07C, 0x17B8, 0xE003, 0
        }
    }, // ង្កើ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x1782, 0x17B6 }, (const uint16_t[])
        {
            0xE152, 0xE047, 0
        }
    }, // ង្គា
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x1782, 0x17BB }, (const uint16_t[])
        {
            0x1784, 0xE01B, 0x17BB, 0
        }
    }, // ង្គុ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x1782, 0x17C4 }, (const uint16_t[])
        {
            0xE1AE, 0xE005, 0
        }
    }, // ង្គោ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x179A, 0x17B8 }, (const uint16_t[])
        {
            0xE0E6, 0x17B8, 0
        }
    }, // ង្រី
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x179A, 0x17C0 }, (const uint16_t[])
        {
            0xE0FD, 0xE035, 0
        }
    }, // ង្រៀ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x179C, 0x17B6 }, (const uint16_t[])
        {
            0xE152, 0xE059, 0
        }
    }, // ង្វា
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x179C, 0x17B7 }, (const uint16_t[])
        {
            0x1784, 0x17B7, 0xE020, 0
        }
    }, // ង្វិ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x179C, 0x17C1 }, (const uint16_t[])
        {
            0xE07C, 0xE020, 0
        }
    }, // ង្វេ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x179F, 0x17B6 }, (const uint16_t[])
        {
            0x1784, 0xE176, 0
        }
    }, // ង្សា
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x17A0, 0x17B6 }, (const uint16_t[])
        {
            0xE152, 0xE022, 0
        }
    }, // ង្ហា
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x17A0, 0x17BC }, (const uint16_t[])
        {
            0x1784, 0xE022, 0x17BC, 0
        }
    }, // ង្ហូ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x17A0, 0x17BD }, (const uint16_t[])
        {
            0x1784, 0xE022, 0xE14C, 0
        }
    }, // ង្ហួ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x17A0, 0x17BE }, (const uint16_t[])
        {
            0xE07C, 0x17B8, 0xE022, 0
        }
    }, // ង្ហើ
    {
        4, (const uint16_t[]) { 0x1784, 0x17D2, 0x17A2, 0x17BB }, (const uint16_t[])
        {
            0x1784, 0xE023, 0xE17D, 0
        }
    }, // ង្អុ
    {
        4, (const uint16_t[]) { 0x1785, 0x17B7, 0x17D2, 0x1786 }, (const uint16_t[])
        {
            0x1785, 0x17B7, 0xE009, 0
        }
    }, // ចិ្ឆ
    {
        4, (const uint16_t[]) { 0x1785, 0x17D2, 0x1786, 0x17C1 }, (const uint16_t[])
        {
            0xE07D, 0xE04A, 0
        }
    }, // ច្ឆេ
    {
        4, (const uint16_t[]) { 0x1785, 0x17D2, 0x1786, 0x17D0 }, (const uint16_t[])
        {
            0x1785, 0x17D0, 0xE04A, 0
        }
    }, // ច្ឆ័
    {
        4, (const uint16_t[]) { 0x1785, 0x17D2, 0x1794, 0x17B6 }, (const uint16_t[])
        {
            0x1785, 0xE174, 0
        }
    }, // ច្បា
    {
        4, (const uint16_t[]) { 0x1785, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE1F5, 0
        }
    }, // ច្រា
    {
        4, (const uint16_t[]) { 0x1785, 0x17D2, 0x179A, 0x17BE }, (const uint16_t[])
        {
            0xE0FC, 0x17B8, 0
        }
    }, // ច្រើ
    {
        4, (const uint16_t[]) { 0X1786, 0X17D2, 0X1793, 0X17C1 }, (const uint16_t[])
        {
            0xE07E, 0xE053, 0
        }
    }, // ឆ្នេ
    {
        4, (const uint16_t[]) { 0x1786, 0x17D2, 0x1784, 0x17B6 }, (const uint16_t[])
        {
            0xE160, 0xE007, 0
        }
    }, // ឆ្ងា
    {
        4, (const uint16_t[]) { 0x1786, 0x17D2, 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE154, 0xE058, 0
        }
    }, // ឆ្លា
    {
        4, (const uint16_t[]) { 0x1786, 0x17D2, 0x179B, 0x17BE }, (const uint16_t[])
        {
            0xE08A, 0x17B8, 0xE01F, 0
        }
    }, // ឆ្លើ
    {
        4, (const uint16_t[]) { 0x1786, 0x17D2, 0x179C, 0x17C1 }, (const uint16_t[])
        {
            0xE07E, 0xE020, 0
        }
    }, // ឆ្វេ
    {
        4, (const uint16_t[]) { 0x1786, 0x17D2, 0x17A2, 0x17B9 }, (const uint16_t[])
        {
            0x1786, 0x17B8, 0xE023, 0
        }
    }, // ឆ្អឹ
    {
        4, (const uint16_t[]) { 0x1787, 0x17D2, 0x1789, 0x17B6 }, (const uint16_t[])
        {
            0xE155, 0xE04C, 0
        }
    }, // ជ្ញា
    {
        4, (const uint16_t[]) { 0x1787, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE1F6, 0
        }
    }, // ជ្រា
    {
        4, (const uint16_t[]) { 0x1787, 0x17D2, 0x179A, 0x17BE }, (const uint16_t[])
        {
            0xE0FD, 0x17B8, 0
        }
    }, // ជ្រើ
    {
        4, (const uint16_t[]) { 0x1787, 0x17D2, 0x179A, 0x17C5 }, (const uint16_t[])
        {
            0xE21D, 0
        }
    }, // ជ្រៅ
    {
        4, (const uint16_t[]) { 0x1788, 0x17D2, 0x1793, 0x17C7 }, (const uint16_t[])
        {
            0x1788, 0xE017, 0x17C7, 0
        }
    }, // ឈ្នះ
    {
        4, (const uint16_t[]) { 0x1788, 0x17D2, 0x179B, 0x17B8 }, (const uint16_t[])
        {
            0x1788, 0x17B8, 0xE01F, 0
        }
    }, // ឈ្លី
    {
        4, (const uint16_t[]) { 0x1789, 0x17D2, 0x1785, 0x17C1 }, (const uint16_t[])
        {
            0xE1C0, 0xE049, 0
        }
    }, // ញ្ចេ
    {
        4, (const uint16_t[]) { 0x1789, 0x17D2, 0x1787, 0x17B6 }, (const uint16_t[])
        {
            0xE177, 0xE04B, 0
        }
    }, // ញ្ជា
    {
        4, (const uint16_t[]) { 0x1789, 0x17D2, 0x1787, 0x17B8 }, (const uint16_t[])
        {
            0xE02E, 0x17B8, 0xE00A, 0
        }
    }, // ញ្ជី
    {
        4, (const uint16_t[]) { 0x1789, 0x17D2, 0x1787, 0x17BD }, (const uint16_t[])
        {
            0xE02E, 0xE00A, 0xE14C, 0
        }
    }, // ញ្ជួ
    {
        4, (const uint16_t[]) { 0x1789, 0x17D2, 0x1787, 0x17BE }, (const uint16_t[])
        {
            0xE1C0, 0x17B8, 0xE00A, 0
        }
    }, // ញ្ជើ
    {
        4, (const uint16_t[]) { 0x1789, 0x17D2, 0x1789, 0x17B6 }, (const uint16_t[])
        {
            0xE177, 0xE00D, 0
        }
    }, // ញ្ញា
    {
        4, (const uint16_t[]) { 0x1789, 0x17D2, 0x17A0, 0x17B6 }, (const uint16_t[])
        {
            0xE177, 0xE05A, 0
        }
    }, // ញ្ហា
    {
        4, (const uint16_t[]) { 0x178A, 0x17D2, 0x178B, 0x17B6 }, (const uint16_t[])
        {
            0xE158, 0xE00F, 0
        }
    }, // ដ្ឋា
    {
        4, (const uint16_t[]) { 0x178A, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE1F8, 0
        }
    }, // ដ្រា
    {
        4, (const uint16_t[]) { 0x178E, 0x17D2, 0x178B, 0x17B6 }, (const uint16_t[])
        {
            0xE15C, 0xE00F, 0
        }
    }, // ណ្ឋា
    {
        4, (const uint16_t[]) { 0x178E, 0x17D2, 0x178E, 0x17B6 }, (const uint16_t[])
        {
            0xE15C, 0xE012, 0
        }
    }, // ណ្ណា
    {
        4, (const uint16_t[]) { 0x178E, 0x17D2, 0x178F, 0x17B6 }, (const uint16_t[])
        {
            0xE15C, 0xE00E, 0
        }
    }, // ណ្តា
    {
        4, (const uint16_t[]) { 0x178E, 0x17D2, 0x178F, 0x17BE }, (const uint16_t[])
        {
            0xE086, 0x17B8, 0xE013, 0
        }
    }, // ណ្តើ
    {
        4, (const uint16_t[]) { 0x178F, 0x17D2, 0x1790, 0x17B7 }, (const uint16_t[])
        {
            0x178F, 0x17B7, 0xE014, 0
        }
    }, // ត្ថិ
    {
        4, (const uint16_t[]) { 0x178F, 0x17D2, 0x1794, 0x17B7 }, (const uint16_t[])
        {
            0x178F, 0xE018, 0x17B7, 0
        }
    }, // ត្បិ
    {
        4, (const uint16_t[]) { 0x178F, 0x17D2, 0x1794, 0x17BC }, (const uint16_t[])
        {
            0x178F, 0xE018, 0xE14B, 0
        }
    }, // ត្បូ
    {
        4, (const uint16_t[]) { 0x178F, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE1FA, 0
        }
    }, // ត្រា
    {
        4, (const uint16_t[]) { 0x178F, 0x17D2, 0x179A, 0x17B8 }, (const uint16_t[])
        {
            0xE0EC, 0x17B8, 0
        }
    }, // ត្រី
    {
        4, (const uint16_t[]) { 0x178F, 0x17D2, 0x179A, 0x17BB }, (const uint16_t[])
        {
            0xE0EC, 0x17BB, 0
        }
    }, // ត្រុ
    {
        4, (const uint16_t[]) { 0x178F, 0x17D2, 0x179A, 0x17BC }, (const uint16_t[])
        {
            0xE0EC, 0x17BC, 0
        }
    }, // ត្រូ
    {
        4, (const uint16_t[]) { 0x178F, 0x17D2, 0x179A, 0x17C2 }, (const uint16_t[])
        {
            0xE113, 0
        }
    }, // ត្រែ
    {
        4, (const uint16_t[]) { 0x1790, 0x17D2, 0x1784, 0x17C3 }, (const uint16_t[])
        {
            0xE0D0, 0xE048, 0
        }
    }, // ថ្ងៃ
    {
        4, (const uint16_t[]) { 0x1790, 0x17D2, 0x1793, 0x17B6 }, (const uint16_t[])
        {
            0xE15E, 0xE053, 0
        }
    }, // ថ្នា
    {
        4, (const uint16_t[]) { 0x1790, 0x17D2, 0x179B, 0x17BB }, (const uint16_t[])
        {
            0x1790, 0xE01F, 0x17BB, 0
        }
    }, // ថ្លុ
    {
        4, (const uint16_t[]) { 0x1790, 0x17D2, 0x179B, 0x17C2 }, (const uint16_t[])
        {
            0xE0AC, 0xE01F, 0
        }
    }, // ថ្លែ
    {
        4, (const uint16_t[]) { 0x1790, 0x17D2, 0x179C, 0x17B6 }, (const uint16_t[])
        {
            0xE15E, 0xE020, 0
        }
    }, // ថ្វា
    {
        4, (const uint16_t[]) { 0x1791, 0x17d2, 0x1794, 0x17be }, (const uint16_t[])
        {
            0xE233, 0
        }
    }, // ទ្បើ
    {
        4, (const uint16_t[]) { 0x1791, 0x17D2, 0x1799, 0x17B6 }, (const uint16_t[])
        {
            0x1791, 0xE175, 0
        }
    }, // ទ្យា
    {
        4, (const uint16_t[]) { 0X1791, 0X17D2, 0X1799, 0X17BB }, (const uint16_t[])
        {
            0x1791, 0xE01D, 0xE030, 0
        }
    }, // ទ្យុ
    {
        4, (const uint16_t[]) { 0x1791, 0x17D2, 0x179C, 0x17C1 }, (const uint16_t[])
        {
            0xE089, 0xE020, 0
        }
    }, // ទ្វេ
    {
        4, (const uint16_t[]) { 0x1792, 0x17D2, 0x1793, 0x17BC }, (const uint16_t[])
        {
            0x1792, 0xE017, 0x17BC, 0
        }
    }, // ធ្នូ
    {
        4, (const uint16_t[]) { 0x1792, 0x17D2, 0x1798, 0x17C3 }, (const uint16_t[])
        {
            0xE0D2, 0xE057, 0
        }
    }, // ធ្មៃ
    {
        4, (const uint16_t[]) { 0x1792, 0x17D2, 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE160, 0xE01F, 0
        }
    }, // ធ្លា
    {
        4, (const uint16_t[]) { 0x1792, 0x17D2, 0x179C, 0x17BE }, (const uint16_t[])
        {
            0xE08A, 0x17B8, 0xE020, 0
        }
    }, // ធ្វើ
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x178F, 0x17B7 }, (const uint16_t[])
        {
            0x1793, 0x17B7, 0xE013, 0
        }
    }, // ន្តិ
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x178F, 0x17C2 }, (const uint16_t[])
        {
            0xE0AF, 0xE00E, 0
        }
    }, // ន្តែ
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x1790, 0x17C2 }, (const uint16_t[])
        {
            0xE0AF, 0xE014, 0
        }
    }, // ន្ថែ
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x1791, 0x17B6 }, (const uint16_t[])
        {
            0xE161, 0xE015, 0
        }
    }, // ន្ទា
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x1798, 0x17B6 }, (const uint16_t[])
        {
            0xE161, 0xE057, 0
        }
    }, // ន្មា
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x1799, 0x17B6 }, (const uint16_t[])
        {
            0x1793, 0xE175, 0
        }
    }, // ន្យា
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x179B, 0x17BA }, (const uint16_t[])
        {
            0x1793, 0x17BA, 0xE01F, 0
        }
    }, // ន្លឺ
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x179B, 0x17C2 }, (const uint16_t[])
        {
            0xE0AF, 0xE058, 0
        }
    }, // ន្លែ
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x179B, 0x17C4 }, (const uint16_t[])
        {
            0xE1BD, 0xE058, 0
        }
    }, // ន្លោ
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x179F, 0x17BB }, (const uint16_t[])
        {
            0x1793, 0xE021, 0x17BB, 0
        }
    }, // ន្សុ
    {
        4, (const uint16_t[]) { 0x1793, 0x17D2, 0x179F, 0x17C6 }, (const uint16_t[])
        {
            0x1793, 0xE021, 0x17C6, 0
        }
    }, // ន្សំ
    {
        4, (const uint16_t[]) { 0x1794, 0x17D2, 0x178F, 0x17B6 }, (const uint16_t[])
        {
            0xE153, 0xE00E, 0
        }
    }, // ប្តា
    {
        4, (const uint16_t[]) { 0x1794, 0x17D2, 0x178F, 0x17BC }, (const uint16_t[])
        {
            0x1794, 0xE00E, 0x17BC, 0
        }
    }, // ប្តូ
    {
        4, (const uint16_t[]) { 0x1794, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE1FE, 0
        }
    }, // ប្រា
    {
        4, (const uint16_t[]) { 0x1794, 0x17D2, 0x179A, 0x17C1 }, (const uint16_t[])
        {
            0xE105, 0
        }
    }, // ប្រេ
    {
        4, (const uint16_t[]) { 0x1794, 0x17D2, 0x179A, 0x17BE }, (const uint16_t[])
        {
            0xE105, 0x17B8, 0
        }
    }, // ប្រើ
    {
        4, (const uint16_t[]) { 0x1794, 0x17d2, 0x179a, 0x17c0 }, (const uint16_t[])
        {
            0xE107, 0x17C0, 0
        }
    }, // ប្រៀ
    {
        4, (const uint16_t[]) { 0x1794, 0x17D2, 0x179A, 0x17C2 }, (const uint16_t[])
        {
            0xE10E, 0
        }
    }, // ប្រែ
    {
        4, (const uint16_t[]) { 0x1794, 0x17D2, 0x17A2, 0x17BC }, (const uint16_t[])
        {
            0x1794, 0xE023, 0xE14B, 0
        }
    }, // ប្អូ
    {
        4, (const uint16_t[]) { 0x1794, 0x17d2, 0x179b, 0x17bc }, (const uint16_t[])
        {
            0x1794, 0xE01F, 0xE025, 0
        }
    }, // ប្លូ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x1780, 0x17B6 }, (const uint16_t[])
        {
            0xE158, 0xE045, 0
        }
    }, // ផ្កា
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x1789, 0x17BE }, (const uint16_t[])
        {
            0xE07E, 0x17B8, 0xE00C, 0
        }
    }, // ផ្ញើ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x178A, 0x17B6 }, (const uint16_t[])
        {
            0xE155, 0xE00E, 0
        }
    }, // ផ្ដា
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x178A, 0x17BE }, (const uint16_t[])
        {
            0xE07F, 0x17B8, 0xE00E, 0
        }
    }, // ផ្ដើ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x178A, 0x17C1 }, (const uint16_t[])
        {
            0xE07F, 0xE00E, 0
        }
    }, // ផ្ដេ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x178F, 0x17B6 }, (const uint16_t[])
        {
            0xE155, 0xE00E, 0
        }
    }, // ផ្តា
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x178F, 0x17BE }, (const uint16_t[])
        {
            0xE07F, 0x17B8, 0xE00E, 0
        }
    }, // ផ្តើ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x178F, 0x17C4 }, (const uint16_t[])
        {
            0xE1BF, 0xE00E, 0
        }
    }, // ផ្តោ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x1791, 0x17B6 }, (const uint16_t[])
        {
            0xE163, 0xE015, 0
        }
    }, // ផ្ទា
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x1791, 0x17BB }, (const uint16_t[])
        {
            0x1795, 0xE015, 0x17BB, 0
        }
    }, // ផ្ទុ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x1791, 0x17C3 }, (const uint16_t[])
        {
            0xE0B1, 0xE015, 0
        }
    }, // ផ្ទៃ
    {
        4, (const uint16_t[]) { 0x1795, 0x17d2, 0x1791, 0x17c7 }, (const uint16_t[])
        {
            0x1795, 0xE015, 0x17c7, 0
        }
    }, // ផ្ទះ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x1793, 0x17BC }, (const uint16_t[])
        {
            0x1795, 0xE017, 0xE14B, 0
        }
    }, // ផ្នូ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x1793, 0x17C2 }, (const uint16_t[])
        {
            0xE0B1, 0xE053, 0
        }
    }, // ផ្នែ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE163, 0xE01F, 0
        }
    }, // ផ្លា
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x179B, 0x17BC }, (const uint16_t[])
        {
            0x1795, 0xE01F, 0x17BC, 0
        }
    }, // ផ្លូ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x179F, 0x17B6 }, (const uint16_t[])
        {
            0x1795, 0xE176, 0
        }
    }, // ផ្សា
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x179F, 0x17C1 }, (const uint16_t[])
        {
            0xE07F, 0xE021, 0
        }
    }, // ផ្សេ
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x17A2, 0x17B6 }, (const uint16_t[])
        {
            0xE163, 0xE023, 0
        }
    }, // ផ្អា
    {
        4, (const uint16_t[]) { 0x1795, 0x17D2, 0x17A2, 0x17C2 }, (const uint16_t[])
        {
            0xE0A3, 0xE023, 0
        }
    }, // ផ្អែ
    {
        4, (const uint16_t[]) { 0x1796, 0x17D2, 0x1799, 0x17B6 }, (const uint16_t[])
        {
            0x1796, 0xE175, 0
        }
    }, // ព្យា
    {
        4, (const uint16_t[]) { 0x1796, 0x17D2, 0x1799, 0x17BD }, (const uint16_t[])
        {
            0x1796, 0xE01D, 0x17BD, 0
        }
    }, // ព្យួ
    {
        4, (const uint16_t[]) { 0x1796, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE1FF, 0
        }
    }, // ព្រា
    {
        4, (const uint16_t[]) { 0x1796, 0x17D2, 0x179A, 0x17B7 }, (const uint16_t[])
        {
            0xE01E, 0x1796, 0x17B7, 0
        }
    }, // ព្រិ
    {
        4, (const uint16_t[]) { 0x1796, 0x17D2, 0x179A, 0x17B9 }, (const uint16_t[])
        {
            0xE01E, 0x1796, 0x17B9, 0
        }
    }, // ព្រឹ
    {
        4, (const uint16_t[]) { 0x1796, 0x17D2, 0x179A, 0x17BD }, (const uint16_t[])
        {
            0xE0F1, 0x17BD, 0
        }
    }, // ព្រួ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x1785, 0x17B6 }, (const uint16_t[])
        {
            0xE165, 0xE049, 0
        }
    }, // ភ្ចា
    {
        4, (const uint16_t[]) { 0x1786, 0x17D2, 0x1798, 0x17B6 }, (const uint16_t[])
        {
            0xE154, 0xE057, 0
        }
    }, // ឆ្មា
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x1787, 0x17B6 }, (const uint16_t[])
        {
            0xE165, 0xE04B, 0
        }
    }, // ភ្ជា
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x1789, 0x17B6 }, (const uint16_t[])
        {
            0xE165, 0xE00C, 0
        }
    }, // ភ្ញា
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x1789, 0x17C0 }, (const uint16_t[])
        {
            0xE08F, 0xE04C, 0xE035, 0
        }
    }, // ភ្ញៀ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x1793, 0x17C2 }, (const uint16_t[])
        {
            0xE0B3, 0xE017, 0
        }
    }, // ភ្នែ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x1793, 0x17C6 }, (const uint16_t[])
        {
            0x1797, 0x17C6, 0xE017, 0
        }
    }, // ភ្នំ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x1798, 0x17C6 }, (const uint16_t[])
        {
            0x1797, 0x17C6, 0xE01C, 0
        }
    }, // ភ្មំ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE165, 0xE01F, 0
        }
    }, // ភ្លា
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x179B, 0x17BA }, (const uint16_t[])
        {
            0x1797, 0x17BA, 0xE01F, 0
        }
    }, // ភ្លឺ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x179B, 0x17BE }, (const uint16_t[])
        {
            0xE08F, 0x17B8, 0xE01F, 0
        }
    }, // ភ្លើ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x179B, 0x17C0 }, (const uint16_t[])
        {
            0xE08F, 0xE01F, 0xE035, 0
        }
    }, // ភ្លៀ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x179B, 0x17C1 }, (const uint16_t[])
        {
            0xE08F, 0xE01F, 0
        }
    }, // ភ្លេ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x179B, 0x17C2 }, (const uint16_t[])
        {
            0xE0B3, 0xE01F, 0
        }
    }, // ភ្លែ
    {
        4, (const uint16_t[]) { 0x1797, 0x17D2, 0x179B, 0x17C5 }, (const uint16_t[])
        {
            0xE08F, 0xE01F, 0xE22F, 0
        }
    }, // ភ្លៅ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1780, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0xE003, 0
        }
    }, // ម្កា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1782, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0xE005, 0
        }
    }, // ម្គា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1784, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0xE007, 0
        }
    }, // ម្ងា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1785, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0xE008, 0
        }
    }, // ម្ចា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x178A, 0x17B8 }, (const uint16_t[])
        {
            0x1798, 0x17B8, 0xE00E, 0
        }
    }, // ម្ដី
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x178A, 0x17C1 }, (const uint16_t[])
        {
            0xE090, 0xE04D, 0
        }
    }, // ម្ដេ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x178F, 0x17C5 }, (const uint16_t[])
        {
            0xE1E6, 0xE04D, 0
        }
    }, // ម្តៅ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1793, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0xE017, 0
        }
    }, // ម្នា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1794, 0x17B8 }, (const uint16_t[])
        {
            0x1798, 0x17B8, 0xE018, 0
        }
    }, // ម្បី
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1794, 0x17BB }, (const uint16_t[])
        {
            0x1798, 0xE018, 0x17BB, 0
        }
    }, // ម្បុ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1796, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0xE055, 0
        }
    }, // ម្ពា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1796, 0x17BB }, (const uint16_t[])
        {
            0x1798, 0xE055, 0x17BB, 0
        }
    }, // ម្ពុ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1796, 0x17D0 }, (const uint16_t[])
        {
            0x1798, 0x17D0, 0xE01A, 0
        }
    }, // ម្ព័
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x1797, 0x17C8 }, (const uint16_t[])
        {
            0x1798, 0xE056, 0x17C8, 0
        }
    }, // ម្ភៈ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE200, 0
        }
    }, // ម្រា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179A, 0x17B7 }, (const uint16_t[])
        {
            0xE0F2, 0x17B7, 0
        }
    }, // ម្រិ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179A, 0x17BE }, (const uint16_t[])
        {
            0xE107, 0x17B8, 0
        }
    }, // ម្រើ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179A, 0x17C1 }, (const uint16_t[])
        {
            0xE107, 0
        }
    }, // ម្រេ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179A, 0x17C4 }, (const uint16_t[])
        {
            0xE215, 0
        }
    }, // ម្រោ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0xE01F, 0
        }
    }, // ម្លា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179B, 0x17BE }, (const uint16_t[])
        {
            0xE090, 0x17B8, 0xE01F, 0
        }
    }, // ម្លើ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179B, 0x17C1 }, (const uint16_t[])
        {
            0xE090, 0xE058, 0
        }
    }, // ម្លេ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179B, 0x17C2 }, (const uint16_t[])
        {
            0xE0B4, 0xE01F, 0
        }
    }, // ម្លែ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179B, 0x17C3 }, (const uint16_t[])
        {
            0xE0D8, 0xE01F, 0
        }
    }, // ម្លៃ
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179F, 0x17B6 }, (const uint16_t[])
        {
            0x1798, 0xE176, 0
        }
    }, // ម្សា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x179F, 0x17B7 }, (const uint16_t[])
        {
            0x179E, 0xE021, 0xE03D, 0
        }
    }, // ម្ស
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x17A2, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0xE05B, 0
        }
    }, // ម្អា
    {
        4, (const uint16_t[]) { 0x1798, 0x17D2, 0x17A2, 0x17B7 }, (const uint16_t[])
        {
            0x1798, 0x17B7, 0xE05B, 0
        }
    }, // ម្អិ
    {
        4, (const uint16_t[]) { 0x179A, 0x178A, 0x17D2, 0x178B }, (const uint16_t[])
        {
            0x179A, 0x178A, 0xE00F, 0
        }
    }, // រដ្ឋ
    {
        4, (const uint16_t[]) { 0x179B, 0x17D2, 0x1794, 0x17BD }, (const uint16_t[])
        {
            0x179B, 0xE018, 0x17BD, 0
        }
    }, // ល្បួ
    {
        4, (const uint16_t[]) { 0x179B, 0x17D2, 0x1794, 0x17BF }, (const uint16_t[])
        {
            0xE093, 0xE018, 0xE034, 0
        }
    }, // ល្បឿ
    {
        4, (const uint16_t[]) { 0x179B, 0x17D2, 0x1794, 0x17C2 }, (const uint16_t[])
        {
            0xE0B7, 0xE018, 0
        }
    }, // ល្បែ
    {
        4, (const uint16_t[]) { 0x179B, 0x17D2, 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE169, 0xE01F, 0
        }
    }, // ល្លា
    {
        4, (const uint16_t[]) { 0x179B, 0x17D2, 0x17A0, 0x17C2 }, (const uint16_t[])
        {
            0xE0B7, 0xE022, 0
        }
    }, // ល្ហែ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1780, 0x17C1 }, (const uint16_t[])
        {
            0xE097, 0xE003, 0
        }
    }, // ស្កេ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1782, 0x17B6 }, (const uint16_t[])
        {
            0xE16D, 0xE047, 0
        }
    }, // ស្គា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1782, 0x17B8 }, (const uint16_t[])
        {
            0x179F, 0x17B8, 0xE005, 0
        }
    }, // ស្គី
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1784, 0x17B6 }, (const uint16_t[])
        {
            0xE16D, 0xE007, 0
        }
    }, // ស្ងា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1784, 0x17C0 }, (const uint16_t[])
        {
            0xE097, 0xE007, 0xE035, 0
        }
    }, // ស្ងៀ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1789, 0x17C2 }, (const uint16_t[])
        {
            0xE0BB, 0xE00C, 0
        }
    }, // ស្ញែ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x178A, 0x17B8 }, (const uint16_t[])
        {
            0x179F, 0x17B8, 0xE00E, 0
        }
    }, // ស្ដី
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x178A, 0x17BB }, (const uint16_t[])
        {
            0x179F, 0xE00E, 0x17BB, 0
        }
    }, // ស្ដុ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x178A, 0x17C1 }, (const uint16_t[])
        {
            0xE097, 0xE00E, 0
        }
    }, // ស្ដេ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x178B, 0x17B6 }, (const uint16_t[])
        {
            0xE16D, 0xE00F, 0
        }
    }, // ស្ឋា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x178F, 0x17B6 }, (const uint16_t[])
        {
            0xE16D, 0xE00E, 0
        }
    }, // ស្តា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x178F, 0x17C1 }, (const uint16_t[])
        {
            0xE097, 0xE00E, 0
        }
    }, // ស្តេ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1790, 0x17B6 }, (const uint16_t[])
        {
            0xE16D, 0xE050, 0
        }
    }, // ស្ថា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1791, 0x17BC }, (const uint16_t[])
        {
            0x179F, 0xE015, 0x17BC, 0
        }
    }, // ស្ទូ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1791, 0x17BE }, (const uint16_t[])
        {
            0xE097, 0x17B8, 0xE015, 0
        }
    }, // ស្ទើ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1793, 0x17B6 }, (const uint16_t[])
        {
            0xE16D, 0xE017, 0
        }
    }, // ស្នា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1793, 0x17BC }, (const uint16_t[])
        {
            0x179F, 0xE053, 0x17BC, 0
        }
    }, // ស្នូ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1794, 0x17C2 }, (const uint16_t[])
        {
            0xE0BB, 0xE018, 0
        }
    }, // ស្បែ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x1798, 0x17BE }, (const uint16_t[])
        {
            0xE097, 0x17B8, 0xE057, 0
        }
    }, // ស្មើ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE202, 0
        }
    }, // ស្រា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179A, 0x17BB }, (const uint16_t[])
        {
            0xE0F4, 0x17BB, 0
        }
    }, // ស្រុ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179A, 0x17BC }, (const uint16_t[])
        {
            0xE0F4, 0x17BC, 0
        }
    }, // ស្រួ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179A, 0x17BD }, (const uint16_t[])
        {
            0xE0F4, 0x17BD, 0
        }
    }, // ស្រួ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE16D, 0xE01F, 0
        }
    }, // ស្លា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179B, 0x17BB }, (const uint16_t[])
        {
            0x179F, 0xE01F, 0x17BB, 0
        }
    }, // ស្លុ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179C, 0x17C2 }, (const uint16_t[])
        {
            0xE0BB, 0xE059, 0
        }
    }, // ស្វែ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179C, 0x17C7 }, (const uint16_t[])
        {
            0x179F, 0xE020, 0x17C7, 0
        }
    }, // ស្វះ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179C, 0x17D0 }, (const uint16_t[])
        {
            0x179F, 0x17D0, 0xE020, 0
        }
    }, // ស្វ័
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x179F, 0x17B6 }, (const uint16_t[])
        {
            0x179F, 0xE021, 0x17B6, 0
        }
    }, // ស្សា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x17A2, 0x17B6 }, (const uint16_t[])
        {
            0xE16D, 0xE023, 0
        }
    }, // ស្អា
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x17A2, 0x17C2 }, (const uint16_t[])
        {
            0xE0BB, 0xE05B, 0
        }
    }, // ស្អែ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x17A2, 0x17C6 }, (const uint16_t[])
        {
            0x179F, 0x17C6, 0xE023, 0
        }
    }, // ស្អំ
    {
        4, (const uint16_t[]) { 0x179F, 0x17D2, 0x178F, 0x17B8 }, (const uint16_t[])
        {
            0x179F, 0x17B8, 0xE013, 0
        }
    }, // ស្តី
    {
        4, (const uint16_t[]) { 0x17A0, 0x17CA, 0x17B6, 0x1793 }, (const uint16_t[])
        {
            0xE16E, 0x17CA, 0x1793, 0
        }
    }, // ហ៊ាន
    {
        4, (const uint16_t[]) { 0x17A0, 0x17D2, 0x1782, 0x17B6 }, (const uint16_t[])
        {
            0xE16E, 0xE005, 0
        }
    }, // ហ្គា
    {
        4, (const uint16_t[]) { 0x17A0, 0x17D2, 0x1782, 0x17C1 }, (const uint16_t[])
        {
            0xE098, 0xE005, 0
        }
    }, // ហ្គេ
    {
        4, (const uint16_t[]) { 0x17A0, 0x17D2, 0x1782, 0x17C4 }, (const uint16_t[])
        {
            0xE1CA, 0xE005, 0
        }
    }, // ហ្គោ
    {
        4, (const uint16_t[]) { 0x17A0, 0x17D2, 0x1793, 0x17CE }, (const uint16_t[])
        {
            0x17A0, 0x17CE, 0xE017, 0
        }
    }, // ហ្ន៎
    {
        4, (const uint16_t[]) { 0x17A0, 0x17D2, 0x179C, 0x17B6 }, (const uint16_t[])
        {
            0xE16E, 0xE020, 0
        }
    }, // ហ្វា
    {
        4, (const uint16_t[]) { 0x17A0, 0x17D2, 0x179C, 0x17B7 }, (const uint16_t[])
        {
            0x17A0, 0x17B7, 0xE020, 0
        }
    }, // ហ្វិ
    {
        4, (const uint16_t[]) { 0x17A0, 0x17D2, 0x179C, 0x17B9 }, (const uint16_t[])
        {
            0x17A0, 0x17B8, 0xE020, 0
        }
    }, // ហ្វឹ
    {
        4, (const uint16_t[]) { 0x17A0, 0x17D2, 0x179C, 0x17C2 }, (const uint16_t[])
        {
            0xE0BC, 0xE020, 0
        }
    }, // ហ្វែ
    {
        4, (const uint16_t[]) { 0x17A0, 0x17D2, 0x179F, 0x17C2 }, (const uint16_t[])
        {
            0xE0BC, 0xE021, 0
        }
    }, // ហ្សែ
    {
        4, (const uint16_t[]) { 0x17A2, 0x17D2, 0x179C, 0x17B8 }, (const uint16_t[])
        {
            0x17A3, 0x17B8, 0xE020, 0
        }
    }, // អ្វី

    {
        3, (const uint16_t[]) { 0x1780, 0x17D2, 0x1780 }, (const uint16_t[])
        {
            0x1780, 0xE003, 0
        }
    }, // ក្ក
    {
        3, (const uint16_t[]) { 0x1780, 0x17D2, 0x1781 }, (const uint16_t[])
        {
            0x1780, 0xE004, 0
        }
    }, // ក្ខ
    {
        3, (const uint16_t[]) { 0x1780, 0x17D2, 0x1794 }, (const uint16_t[])
        {
            0x1780, 0xE018, 0
        }
    }, // ក្ប
    {
        3, (const uint16_t[]) { 0x1780, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x1780, 0xE01D, 0
        }
    }, // ក្យ
    {
        3, (const uint16_t[]) { 0x1780, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE0E4, 0
        }
    }, // ក្រ
    {
        3, (const uint16_t[]) { 0x1780, 0x17D2, 0x179F }, (const uint16_t[])
        {
            0x1780, 0xE021, 0
        }
    }, // ក្ស
    {
        3, (const uint16_t[]) { 0x1780, 0x17D2, 0x17A2 }, (const uint16_t[])
        {
            0x1780, 0xE023, 0
        }
    }, // ក្អ
    {
        3, (const uint16_t[]) { 0x1781, 0x17BB, 0x17BD }, (const uint16_t[])
        {
            0x1781, 0xE14C, 0
        }
    }, // ខុួ
    {
        3, (const uint16_t[]) { 0x1781, 0x17D2, 0x1796 }, (const uint16_t[])
        {
            0x1781, 0xE01A, 0
        }
    }, // ខ្ព
    {
        3, (const uint16_t[]) { 0x1781, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x1781, 0xE01D, 0
        }
    }, // ខ្យ
    {
        3, (const uint16_t[]) { 0x1781, 0x17D2, 0x179C }, (const uint16_t[])
        {
            0x1781, 0xE020, 0
        }
    }, // ខ្វ
    {
        3, (const uint16_t[]) { 0x1782, 0x17D2, 0x1782 }, (const uint16_t[])
        {
            0x1782, 0xE047, 0
        }
    }, // គ្គ
    {
        3, (const uint16_t[]) { 0x1782, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE0E5, 0
        }
    }, // គ្រ
    {
        3, (const uint16_t[]) { 0x1784, 0x17D2, 0x1780 }, (const uint16_t[])
        {
            0x1784, 0xE01B, 0
        }
    }, // ង្ក
    {
        3, (const uint16_t[]) { 0x1784, 0x17D2, 0x1782 }, (const uint16_t[])
        {
            0x1784, 0xE005, 0
        }
    }, // ង្គ
    {
        3, (const uint16_t[]) { 0x1784, 0x17D2, 0x1783 }, (const uint16_t[])
        {
            0x1784, 0xE006, 0
        }
    }, // ង្ឃ
    {
        3, (const uint16_t[]) { 0x1784, 0x17D2, 0x179F }, (const uint16_t[])
        {
            0x1784, 0xE021, 0
        }
    }, // ង្ស
    {
        3, (const uint16_t[]) { 0x1784, 0x17D2, 0x17A2 }, (const uint16_t[])
        {
            0x1784, 0xE023, 0
        }
    }, // ង្អ
    {
        3, (const uint16_t[]) { 0x1785, 0x17CB, 0x17C5 }, (const uint16_t[])
        {
            0xE1E3, 0x17CB, 0
        }
    }, // ច់ៅ
    {
        3, (const uint16_t[]) { 0x1785, 0x17D2, 0x1785 }, (const uint16_t[])
        {
            0x1785, 0xE008, 0
        }
    }, // ច្ច
    {
        3, (const uint16_t[]) { 0x1785, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE01E, 0x1785, 0
        }
    }, // ព្រិ
    {
        3, (const uint16_t[]) { 0x1786, 0x17D2, 0x179B }, (const uint16_t[])
        {
            0x1786, 0xE01F, 0
        }
    }, // ឆ្ល
    {
        3, (const uint16_t[]) { 0x1787, 0x17D2, 0x1787 }, (const uint16_t[])
        {
            0x1787, 0xE00A, 0
        }
    }, // ជ្ជ
    {
        3, (const uint16_t[]) { 0x1787, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x1795, 0xE01D, 0
        }
    }, // ជ្យ
    {
        3, (const uint16_t[]) { 0x1787, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE0E8, 0
        }
    }, // ជ្រ
    {
        3, (const uint16_t[]) { 0x1789, 0x17D2, 0x1785 }, (const uint16_t[])
        {
            0xE164, 0xE008, 0
        }
    }, // ញ្ច
    {
        3, (const uint16_t[]) { 0x1789, 0x17D2, 0x1788 }, (const uint16_t[])
        {
            0xE02E, 0xE00B, 0
        }
    }, // ញ្ឈ
    {
        3, (const uint16_t[]) { 0x1789, 0x17D2, 0x1789 }, (const uint16_t[])
        {
            0xE02E, 0xE00D, 0
        }
    }, // ញ្ញ
    {
        3, (const uint16_t[]) { 0x178A, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE1AE, 0x17C7, 0
        }
    }, // ដោះ
    {
        3, (const uint16_t[]) { 0x178A, 0x17D2, 0x178B }, (const uint16_t[])
        {
            0x178A, 0xE00F, 0
        }
    }, // ដ្ឋ
    {
        3, (const uint16_t[]) { 0x178A, 0x17D2, 0x178F }, (const uint16_t[])
        {
            0x178A, 0xE00E, 0
        }
    }, // ដ្ឋ
    {
        3, (const uint16_t[]) { 0x178E, 0x17D2, 0x178A }, (const uint16_t[])
        {
            0x178E, 0xE00E, 0
        }
    }, // ណ្ដ
    {
        3, (const uint16_t[]) { 0x178E, 0x17D2, 0x17A0 }, (const uint16_t[])
        {
            0x178E, 0xE022, 0
        }
    }, // ណ្ហ
    {
        3, (const uint16_t[]) { 0x178F, 0x17B7, 0x17CD }, (const uint16_t[])
        {
            0x178F, 0xE044, 0
        }
    }, // តិ៍
    {
        3, (const uint16_t[]) { 0x178F, 0x17D2, 0x178F }, (const uint16_t[])
        {
            0x178F, 0xE00E, 0
        }
    }, // ត្ត
    {
        3, (const uint16_t[]) { 0x178F, 0x17D2, 0x1790 }, (const uint16_t[])
        {
            0x1780, 0xE050, 0
        }
    }, // ត្ថ
    {
        3, (const uint16_t[]) { 0x178F, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x178F, 0xE01D, 0
        }
    }, // ត្យ
    {
        3, (const uint16_t[]) { 0x178F, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE0EC, 0
        }
    }, // ត្រ
    {
        3, (const uint16_t[]) { 0x178F, 0x17D2, 0x179C }, (const uint16_t[])
        {
            0x178F, 0xE020, 0
        }
    }, // ត្វ
    {
        3, (const uint16_t[]) { 0x178F, 0x17D2, 0x179F }, (const uint16_t[])
        {
            0x178F, 0xE021, 0
        }
    }, // ត្ស
    {
        3, (const uint16_t[]) { 0x1790, 0x17D2, 0x1798 }, (const uint16_t[])
        {
            0x1790, 0xE01C, 0
        }
    }, // ថ្ម
    {
        3, (const uint16_t[]) { 0x1791, 0x17B7, 0x17CD }, (const uint16_t[])
        {
            0x1791, 0xE033, 0
        }
    }, // ទិ៍
    {
        3, (const uint16_t[]) { 0x1791, 0x17D2, 0x1792 }, (const uint16_t[])
        {
            0x1791, 0xE016, 0
        }
    }, // ទ្ធ
    {
        3, (const uint16_t[]) { 0x1791, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE0ED, 0
        }
    }, // ទ្រ
    {
        3, (const uint16_t[]) { 0x1792, 0x17B8, 0x17C6 }, (const uint16_t[])
        {
            0x1795, 0x17B8, 0x17C6, 0
        }
    }, // ធីំ
    {
        3, (const uint16_t[]) { 0x1792, 0x17D2, 0x1784 }, (const uint16_t[])
        {
            0x1792, 0xE007, 0
        }
    }, // ធ្ង
    {
        3, (const uint16_t[]) { 0x1792, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x1792, 0xE01D, 0
        }
    }, // ធ្យ
    {
        3, (const uint16_t[]) { 0x1793, 0x1793, 0x17d0 }, (const uint16_t[])
        {
            0x1793, 0xE02D, 0
        }
    }, // ន័
    {
        3, (const uint16_t[]) { 0x1793, 0x17C1, 0x17C7 }, (const uint16_t[])
        {
            0xE08B, 0x17C7, 0
        }
    }, // នេះ
    {
        3, (const uint16_t[]) { 0x1793, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE08B, 0x17B6, 0x17C7, 0
        }
    }, // នោះ
    {
        3, (const uint16_t[]) { 0x1793, 0x17D2, 0x178F }, (const uint16_t[])
        {
            0x1793, 0xE00E, 0
        }
    }, // ន្ត
    {
        3, (const uint16_t[]) { 0x1793, 0x17D2, 0x1790 }, (const uint16_t[])
        {
            0x1793, 0xE014, 0
        }
    }, // ន្ថ
    {
        3, (const uint16_t[]) { 0x1793, 0x17D2, 0x1791 }, (const uint16_t[])
        {
            0x1793, 0xE015, 0
        }
    }, // ន្ទ
    {
        3, (const uint16_t[]) { 0x1793, 0x17D2, 0x1792 }, (const uint16_t[])
        {
            0x1793, 0xE016, 0
        }
    }, // ន្ធ
    {
        3, (const uint16_t[]) { 0x1793, 0x17D2, 0x1793 }, (const uint16_t[])
        {
            0x1793, 0xE017, 0
        }
    }, // ន្ន
    {
        3, (const uint16_t[]) { 0x1793, 0x17D2, 0x1798 }, (const uint16_t[])
        {
            0x1793, 0xE057, 0
        }
    }, // ន្ម
    {
        3, (const uint16_t[]) { 0x1793, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x1793, 0xE01D, 0
        }
    }, // ន្យ
    {
        3, (const uint16_t[]) { 0x1793, 0x17D2, 0x179B }, (const uint16_t[])
        {
            0x1793, 0xE01F, 0
        }
    }, // ន្ល
    {
        3, (const uint16_t[]) { 0x1794, 0x17C1, 0x17C7 }, (const uint16_t[])
        {
            0xE08C, 0x17C7, 0
        }
    }, // បេះ
    {
        3, (const uint16_t[]) { 0x1794, 0x17C4, 0x17C7 }, (const uint16_t[])
        {
            0xE1BE, 0x17C7, 0
        }
    }, // បោះ
    {
        3, (const uint16_t[]) { 0x1794, 0x17C9, 0x17B6 }, (const uint16_t[])
        {
            0xE162, 0x17C9, 0
        }
    }, // ប៉ា
    {
        3, (const uint16_t[]) { 0x1794, 0x17D2, 0x1794 }, (const uint16_t[])
        {
            0x1794, 0xE018, 0
        }
    }, // ប្ប
    {
        3, (const uint16_t[]) { 0x1794, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE01E, 0x1794, 0
        }
    }, // ប្រ
    {
        3, (const uint16_t[]) { 0x1795, 0x17D2, 0x1782 }, (const uint16_t[])
        {
            0x1795, 0xE005, 0
        }
    }, // ផ្គ
    {
        3, (const uint16_t[]) { 0x1795, 0x17D2, 0x1785 }, (const uint16_t[])
        {
            0x1795, 0xE008, 0
        }
    }, // ផ្ច
    {
        3, (const uint16_t[]) { 0x1795, 0x17D2, 0x178A }, (const uint16_t[])
        {
            0x1795, 0xE00E, 0
        }
    }, // ផ្ដ
    {
        3, (const uint16_t[]) { 0x1795, 0x17D2, 0x179F }, (const uint16_t[])
        {
            0x1795, 0xE021, 0
        }
    }, // ផ្ស
    {
        3, (const uint16_t[]) { 0x1796, 0x17D2, 0x1791 }, (const uint16_t[])
        {
            0x1796, 0xE015, 0
        }
    }, // ព្ទ
    {
        3, (const uint16_t[]) { 0x1796, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x1796, 0xE01D, 0
        }
    }, // ព្យ
    {
        3, (const uint16_t[]) { 0x1796, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE01E, 0x1796, 0
        }
    }, // ព្រ
    {
        3, (const uint16_t[]) { 0x1796, 0x17D2, 0x179C }, (const uint16_t[])
        {
            0x1796, 0xE020, 0
        }
    }, // ព្វ
    {
        3, (const uint16_t[]) { 0x1798, 0x17C9, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0xE042, 0
        }
    }, // ម៉ា
    {
        3, (const uint16_t[]) { 0x1798, 0x17C9, 0x17BA }, (const uint16_t[])
        {
            0x1798, 0x17BA, 0x17BB, 0
        }
    }, // ម៉ឺ
    {
        3, (const uint16_t[]) { 0x1798, 0x17C9, 0x17C2 }, (const uint16_t[])
        {
            0xE0B4, 0xE042, 0
        }
    }, // ម៉ែ
    {
        3, (const uint16_t[]) { 0x1798, 0x17C9, 0x17C4 }, (const uint16_t[])
        {
            0xE1C2, 0xE042, 0
        }
    }, // ម៉ោ
    {
        3, (const uint16_t[]) { 0x1798, 0x17D2, 0x178F }, (const uint16_t[])
        {
            0x1798, 0xE00E, 0
        }
    }, // ម្ត
    {
        3, (const uint16_t[]) { 0x1798, 0x17D2, 0x1796 }, (const uint16_t[])
        {
            0x1798, 0xE01A, 0
        }
    }, // ម្ព
    {
        3, (const uint16_t[]) { 0x1798, 0x17D2, 0x1797 }, (const uint16_t[])
        {
            0x1798, 0xE056, 0
        }
    }, // ម្ភ
    {
        3, (const uint16_t[]) { 0x1798, 0x17D2, 0x1798 }, (const uint16_t[])
        {
            0x1798, 0xE057, 0
        }
    }, // ម្ម
    {
        3, (const uint16_t[]) { 0x1798, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE01E, 0x1798, 0
        }
    }, // ម្រ
    {
        3, (const uint16_t[]) { 0x1799, 0x17C9, 0x17C2 }, (const uint16_t[])
        {
            0xE0B5, 0x17C9, 0
        }
    }, // យ៉ែ
    {
        3, (const uint16_t[]) { 0x1799, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x1783, 0xE01D, 0
        }
    }, // យ្យ
    {
        3, (const uint16_t[]) { 0x179A, 0x17B6, 0x17C6 }, (const uint16_t[])
        {
            0xE168, 0x17C6, 0
        }
    }, // រាំ
    {
        3, (const uint16_t[]) { 0x179A, 0x17C9, 0x17B6 }, (const uint16_t[])
        {
            0xE168, 0xE22D, 0
        }
    }, // រ៉ា
    {
        3, (const uint16_t[]) { 0x179B, 0x17D2, 0x1794 }, (const uint16_t[])
        {
            0x179B, 0xE018, 0
        }
    }, // ល្ប
    {
        3, (const uint16_t[]) { 0x179B, 0x17D2, 0x1798 }, (const uint16_t[])
        {
            0x179B, 0xE01C, 0
        }
    }, // ល្ម
    {
        3, (const uint16_t[]) { 0x179B, 0x17D2, 0x17A2 }, (const uint16_t[])
        {
            0x179B, 0xE023, 0
        }
    }, // ល្អ
    {
        3, (const uint16_t[]) { 0x179F, 0x17CA, 0x17B8 }, (const uint16_t[])
        {
            0x179F, 0x17B8, 0xE17D, 0
        }
    }, // ស៊ី
    {
        3, (const uint16_t[]) { 0x179F, 0x17CA, 0x17BE }, (const uint16_t[])
        {
            0xE097, 0x17B8, 0x17BB, 0
        }
    }, // ស៊ើ
    {
        3, (const uint16_t[]) { 0x179F, 0x17D2, 0x1780 }, (const uint16_t[])
        {
            0x179F, 0xE05C, 0
        }
    }, // ស្ក
    {
        3, (const uint16_t[]) { 0x179F, 0x17D2, 0x1784 }, (const uint16_t[])
        {
            0x179F, 0xE007, 0
        }
    }, // ស្ង
    {
        3, (const uint16_t[]) { 0x179F, 0x17D2, 0x178F }, (const uint16_t[])
        {
            0x179F, 0xE00E, 0
        }
    }, // ស្ត
    {
        3, (const uint16_t[]) { 0x179F, 0x17D2, 0x1791 }, (const uint16_t[])
        {
            0x179F, 0xE015, 0
        }
    }, // ស្ទ
    {
        3, (const uint16_t[]) { 0x179F, 0x17D2, 0x1794 }, (const uint16_t[])
        {
            0x179F, 0xE018, 0
        }
    }, // ស្ប
    {
        3, (const uint16_t[]) { 0x179F, 0x17D2, 0x1798 }, (const uint16_t[])
        {
            0x179F, 0xE01C, 0
        }
    }, // ស្ម
    {
        3, (const uint16_t[]) { 0x179F, 0x17D2, 0x179A }, (const uint16_t[])
        {
            0xE0F4, 0
        }
    }, // ស្រ
    {
        3, (const uint16_t[]) { 0x179F, 0x17D2, 0x179F }, (const uint16_t[])
        {
            0x179F, 0xE021, 0
        }
    }, // ស្ស
    {
        3, (const uint16_t[]) { 0x17A2, 0x17D2, 0x1793 }, (const uint16_t[])
        {
            0x17A3, 0xE017, 0
        }
    }, // អ្ន
    {
        3, (const uint16_t[]) { 0x17B1, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x17B1, 0xE01D, 0
        }
    }, // ឱ្យ
    {
        3, (const uint16_t[]) { 0x17B2, 0x17D2, 0x1799 }, (const uint16_t[])
        {
            0x17B2, 0xE01D, 0
        }
    }, // ឲ្យ
    {
        3, (const uint16_t[]) { 0x1798, 0x17D2, 0x1794 }, (const uint16_t[])
        {
            0x1798, 0xE018, 0
        }
    }, // ម្ប
    {
        2, (const uint16_t[]) { 0x1780, 0x17B6 }, (const uint16_t[])
        {
            0xE14E, 0
        }
    }, // កា
    {
        2, (const uint16_t[]) { 0x1780, 0x17BE }, (const uint16_t[])
        {
            0xE078, 0x17B8, 0
        }
    }, // កើ
    {
        2, (const uint16_t[]) { 0x1780, 0x17C1 }, (const uint16_t[])
        {
            0xE078, 0
        }
    }, // កេ
    {
        2, (const uint16_t[]) { 0x1780, 0x17C2 }, (const uint16_t[])
        {
            0xE09C, 0
        }
    }, // កែ
    {
        2, (const uint16_t[]) { 0x1780, 0x17C4 }, (const uint16_t[])
        {
            0xE1AA, 0
        }
    }, // កោ
    {
        2, (const uint16_t[]) { 0x1781, 0x17B6 }, (const uint16_t[])
        {
            0xE14F, 0
        }
    }, // ខា
    {
        2, (const uint16_t[]) { 0x1781, 0x17C1 }, (const uint16_t[])
        {
            0xE079, 0
        }
    }, // ខេ
    {
        2, (const uint16_t[]) { 0x1781, 0x17C2 }, (const uint16_t[])
        {
            0xE09D, 0
        }
    }, // ខែ
    {
        2, (const uint16_t[]) { 0x1781, 0x17C5 }, (const uint16_t[])
        {
            0xE1CF, 0
        }
    }, // ខៅ
    {
        2, (const uint16_t[]) { 0x1782, 0x17B6 }, (const uint16_t[])
        {
            0xE150, 0
        }
    }, // គា
    {
        2, (const uint16_t[]) { 0x1782, 0x17C1 }, (const uint16_t[])
        {
            0xE07A, 0
        }
    }, // គេ
    {
        2, (const uint16_t[]) { 0x1782, 0x17C4 }, (const uint16_t[])
        {
            0xE1AC, 0
        }
    }, // គោ
    {
        2, (const uint16_t[]) { 0x1783, 0x17B6 }, (const uint16_t[])
        {
            0xE151, 0
        }
    }, // ឃា
    {
        2, (const uint16_t[]) { 0x1783, 0x17BE }, (const uint16_t[])
        {
            0xE07B, 0x17B8, 0
        }
    }, // ឃើ
    {
        2, (const uint16_t[]) { 0x1784, 0x17B6 }, (const uint16_t[])
        {
            0xE152, 0
        }
    }, // ងា
    {
        2, (const uint16_t[]) { 0x1784, 0x17BE }, (const uint16_t[])
        {
            0xE07C, 0x17B8, 0
        }
    }, // ងើ
    {
        2, (const uint16_t[]) { 0x1785, 0x17B6 }, (const uint16_t[])
        {
            0xE153, 0
        }
    }, // ចា
    {
        2, (const uint16_t[]) { 0x1785, 0x17C0 }, (const uint16_t[])
        {
            0xE07D, 0xE035, 0
        }
    }, // ចៀ
    {
        2, (const uint16_t[]) { 0x1785, 0x17C1 }, (const uint16_t[])
        {
            0xE07D, 0
        }
    }, // ចេ
    {
        2, (const uint16_t[]) { 0x1785, 0x17C2 }, (const uint16_t[])
        {
            0xE0A1, 0
        }
    }, // ចែ
    {
        2, (const uint16_t[]) { 0x1785, 0x17C3 }, (const uint16_t[])
        {
            0xE0C5, 0
        }
    }, // ចៃ
    {
        2, (const uint16_t[]) { 0x1785, 0x17C4 }, (const uint16_t[])
        {
            0xE1AF, 0
        }
    }, // ចោ
    {
        2, (const uint16_t[]) { 0x1785, 0x17C5 }, (const uint16_t[])
        {
            0xE1D3, 0
        }
    }, // ចៅ
    {
        2, (const uint16_t[]) { 0x1786, 0x17B6 }, (const uint16_t[])
        {
            0xE154, 0
        }
    }, // ឆា
    {
        2, (const uint16_t[]) { 0x1786, 0x17C0 }, (const uint16_t[])
        {
            0xE07E, 0xE035, 0
        }
    }, // ឆៀ
    {
        2, (const uint16_t[]) { 0x1787, 0x17B6 }, (const uint16_t[])
        {
            0xE155, 0
        }
    }, // ជា
    {
        2, (const uint16_t[]) { 0x1787, 0x17BE }, (const uint16_t[])
        {
            0xE08D, 0x17B8, 0
        }
    }, // ជើ
    {
        2, (const uint16_t[]) { 0x1787, 0x17BF }, (const uint16_t[])
        {
            0xE08A, 0xE034, 0
        }
    }, // ជឿ
    {
        2, (const uint16_t[]) { 0x1787, 0x17C1 }, (const uint16_t[])
        {
            0xE08D, 0
        }
    }, // ជេ
    {
        2, (const uint16_t[]) { 0x1787, 0x17C4 }, (const uint16_t[])
        {
            0xE1BF, 0
        }
    }, // ជោ
    {
        2, (const uint16_t[]) { 0x1788, 0x17B6 }, (const uint16_t[])
        {
            0xE156, 0
        }
    }, // ឈា
    {
        2, (const uint16_t[]) { 0x1788, 0x17BE }, (const uint16_t[])
        {
            0xE080, 0x17B8, 0
        }
    }, // ឈើ
    {
        2, (const uint16_t[]) { 0x1789, 0x17C2 }, (const uint16_t[])
        {
            0xE0A5, 0
        }
    }, // ញែ
    {
        2, (const uint16_t[]) { 0x178A, 0x17B6 }, (const uint16_t[])
        {
            0xE158, 0
        }
    }, // ដា
    {
        2, (const uint16_t[]) { 0x178A, 0x17BE }, (const uint16_t[])
        {
            0xE082, 0x17B8, 0
        }
    }, // ដើ
    {
        2, (const uint16_t[]) { 0x178A, 0x17C1 }, (const uint16_t[])
        {
            0xE082, 0
        }
    }, // ដេ
    {
        2, (const uint16_t[]) { 0x178A, 0x17C2 }, (const uint16_t[])
        {
            0xE0A6, 0
        }
    }, // ដែ
    {
        2, (const uint16_t[]) { 0x178A, 0x17C3 }, (const uint16_t[])
        {
            0xE0CA, 0
        }
    }, // ដៃ
    {
        2, (const uint16_t[]) { 0x178A, 0x17C4 }, (const uint16_t[])
        {
            0xE1B4, 0
        }
    }, // ដោ
    {
        2, (const uint16_t[]) { 0x178A, 0x17C5 }, (const uint16_t[])
        {
            0xE1D8, 0
        }
    }, // ដៅ
    {
        2, (const uint16_t[]) { 0x178B, 0x17B6 }, (const uint16_t[])
        {
            0xE159, 0
        }
    }, // ឋា
    {
        2, (const uint16_t[]) { 0x178C, 0x17B6 }, (const uint16_t[])
        {
            0xE15A, 0
        }
    }, // ឌា
    {
        2, (const uint16_t[]) { 0x178C, 0x17BE }, (const uint16_t[])
        {
            0xE084, 0x17B8, 0
        }
    }, // ឌើ
    {
        2, (const uint16_t[]) { 0x178C, 0x17C2 }, (const uint16_t[])
        {
            0xE0A8, 0
        }
    }, // ឌែ
    {
        2, (const uint16_t[]) { 0x178E, 0x17B6 }, (const uint16_t[])
        {
            0xE15C, 0
        }
    }, // ណា
    {
        2, (const uint16_t[]) { 0x178E, 0x17BE }, (const uint16_t[])
        {
            0xE086, 0x17B8, 0
        }
    }, // ណើ
    {
        2, (const uint16_t[]) { 0x178E, 0x17C2 }, (const uint16_t[])
        {
            0x17C2, 0x178E, 0
        }
    }, // ណែ
    {
        2, (const uint16_t[]) { 0x178E, 0x17C4 }, (const uint16_t[])
        {
            0xE1B8, 0
        }
    }, // ណោ
    {
        2, (const uint16_t[]) { 0x178F, 0x17B6 }, (const uint16_t[])
        {
            0xE15D, 0
        }
    }, // តា
    {
        2, (const uint16_t[]) { 0x178F, 0x17BE }, (const uint16_t[])
        {
            0xE087, 0x17B8, 0
        }
    }, // តើ
    {
        2, (const uint16_t[]) { 0x178F, 0x17BF }, (const uint16_t[])
        {
            0xE087, 0xE034, 0
        }
    }, // តឿ
    {
        2, (const uint16_t[]) { 0x178F, 0x17C1 }, (const uint16_t[])
        {
            0xE087, 0
        }
    }, // តេ
    {
        2, (const uint16_t[]) { 0x178F, 0x17C2 }, (const uint16_t[])
        {
            0xE0AB, 0
        }
    }, // តែ
    {
        2, (const uint16_t[]) { 0x178F, 0x17C3 }, (const uint16_t[])
        {
            0xE0CF, 0
        }
    }, // តៃ
    {
        2, (const uint16_t[]) { 0x1790, 0x17B6 }, (const uint16_t[])
        {
            0xE15E, 0
        }
    }, // ថា
    {
        2, (const uint16_t[]) { 0x1790, 0x17C1 }, (const uint16_t[])
        {
            0xE088, 0
        }
    }, // ថេ
    {
        2, (const uint16_t[]) { 0x1790, 0x17C2 }, (const uint16_t[])
        {
            0xE0AC, 0
        }
    }, // ថែ
    {
        2, (const uint16_t[]) { 0x1790, 0x17C3 }, (const uint16_t[])
        {
            0xE0D0, 0
        }
    }, // ថៃ
    {
        2, (const uint16_t[]) { 0x1791, 0x17B6 }, (const uint16_t[])
        {
            0xE15F, 0
        }
    }, // ទា
    {
        2, (const uint16_t[]) { 0x1791, 0x17C0 }, (const uint16_t[])
        {
            0xE089, 0x17C0, 0
        }
    }, // ទៀ
    {
        2, (const uint16_t[]) { 0x1791, 0x17C1 }, (const uint16_t[])
        {
            0xE089, 0
        }
    }, // ទេ
    {
        2, (const uint16_t[]) { 0x1791, 0x17C3 }, (const uint16_t[])
        {
            0xE0D1, 0
        }
    }, // ទៃ
    {
        2, (const uint16_t[]) { 0x1791, 0x17C4 }, (const uint16_t[])
        {
            0xE1BB, 0
        }
    }, // ទោ
    {
        2, (const uint16_t[]) { 0x1791, 0x17C5 }, (const uint16_t[])
        {
            0xE1DF, 0
        }
    }, // ទៅ
    {
        2, (const uint16_t[]) { 0x1792, 0x17B6 }, (const uint16_t[])
        {
            0xE160, 0
        }
    }, // ធា
    {
        2, (const uint16_t[]) { 0x1792, 0x17C0 }, (const uint16_t[])
        {
            0xE08D, 0x17C0, 0
        }
    }, // ធៀ
    {
        2, (const uint16_t[]) { 0x1793, 0x17B6 }, (const uint16_t[])
        {
            0xE161, 0
        }
    }, // នា
    {
        2, (const uint16_t[]) { 0x1793, 0x17BE }, (const uint16_t[])
        {
            0xE08B, 0x17B8, 0
        }
    }, // នើ
    {
        2, (const uint16_t[]) { 0x1793, 0x17BF }, (const uint16_t[])
        {
            0xE08B, 0xE034, 0
        }
    }, // នឿ
    {
        2, (const uint16_t[]) { 0x1793, 0x17C1 }, (const uint16_t[])
        {
            0xE08B, 0
        }
    }, // នេ
    {
        2, (const uint16_t[]) { 0x1793, 0x17C3 }, (const uint16_t[])
        {
            0xE0D3, 0
        }
    }, // នៃ
    {
        2, (const uint16_t[]) { 0x1793, 0x17C5 }, (const uint16_t[])
        {
            0xE1E1, 0
        }
    }, // នៅ
    {
        2, (const uint16_t[]) { 0x1794, 0x17B6 }, (const uint16_t[])
        {
            0xE162, 0
        }
    }, // បា
    {
        2, (const uint16_t[]) { 0x1794, 0x17BE }, (const uint16_t[])
        {
            0xE08C, 0x17B8, 0
        }
    }, // បើ
    {
        2, (const uint16_t[]) { 0x1794, 0x17C0 }, (const uint16_t[])
        {
            0xE08C, 0xE035, 0
        }
    }, // បៀ
    {
        2, (const uint16_t[]) { 0x1794, 0x17C1 }, (const uint16_t[])
        {
            0xE08C, 0
        }
    }, // បេ
    {
        2, (const uint16_t[]) { 0x1794, 0x17C2 }, (const uint16_t[])
        {
            0xE0B0, 0
        }
    }, // បែ
    {
        2, (const uint16_t[]) { 0x1794, 0x17C4 }, (const uint16_t[])
        {
            0xE1BE, 0
        }
    }, // បោ
    {
        2, (const uint16_t[]) { 0x1794, 0x17C5 }, (const uint16_t[])
        {
            0xE1E2, 0
        }
    }, // បៅ
    {
        2, (const uint16_t[]) { 0x1795, 0x17B6 }, (const uint16_t[])
        {
            0xE155, 0
        }
    }, // ផា
    {
        2, (const uint16_t[]) { 0x1795, 0x17C2 }, (const uint16_t[])
        {
            0xE0A2, 0
        }
    }, // ផែ
    {
        2, (const uint16_t[]) { 0x1796, 0x17B6 }, (const uint16_t[])
        {
            0xE02E, 0
        }
    }, // ពា
    {
        2, (const uint16_t[]) { 0x1796, 0x17BE }, (const uint16_t[])
        {
            0xE08E, 0x17B8, 0
        }
    }, // ពើ
    {
        2, (const uint16_t[]) { 0x1796, 0x17C1 }, (const uint16_t[])
        {
            0xE08E, 0
        }
    }, // ពេ
    {
        2, (const uint16_t[]) { 0x1796, 0x17C4 }, (const uint16_t[])
        {
            0xE09B, 0
        }
    }, // ពោ
    {
        2, (const uint16_t[]) { 0x1797, 0x17B6 }, (const uint16_t[])
        {
            0xE165, 0
        }
    }, // ភា
    {
        2, (const uint16_t[]) { 0x1797, 0x17C1 }, (const uint16_t[])
        {
            0xE08F, 0
        }
    }, // ភេ
    {
        2, (const uint16_t[]) { 0x1797, 0x17C5 }, (const uint16_t[])
        {
            0xE1E5, 0
        }
    }, // ភៅ
    {
        2, (const uint16_t[]) { 0x1798, 0x17B6 }, (const uint16_t[])
        {
            0xE166, 0
        }
    }, // មា
    {
        2, (const uint16_t[]) { 0x1798, 0x17BE }, (const uint16_t[])
        {
            0xE090, 0x17B8, 0
        }
    }, // មើ
    {
        2, (const uint16_t[]) { 0x1798, 0x17C0 }, (const uint16_t[])
        {
            0xE090, 0xE035, 0
        }
    }, // មៀ
    {
        2, (const uint16_t[]) { 0x1798, 0x17C1 }, (const uint16_t[])
        {
            0xE090, 0
        }
    }, // មេ
    {
        2, (const uint16_t[]) { 0x1798, 0x17C2 }, (const uint16_t[])
        {
            0xE0B4, 0
        }
    }, // មែ
    {
        2, (const uint16_t[]) { 0x1799, 0x17B6 }, (const uint16_t[])
        {
            0xE167, 0
        }
    }, // យា
    {
        2, (const uint16_t[]) { 0x1799, 0x17BE }, (const uint16_t[])
        {
            0xE091, 0x17B8, 0
        }
    }, // យើ
    {
        2, (const uint16_t[]) { 0x1799, 0x17C1 }, (const uint16_t[])
        {
            0xE091, 0
        }
    }, // យេ
    {
        2, (const uint16_t[]) { 0x1799, 0x17C4 }, (const uint16_t[])
        {
            0xE1C3, 0
        }
    }, // យោ
    {
        2, (const uint16_t[]) { 0x179A, 0x17B6 }, (const uint16_t[])
        {
            0xE168, 0
        }
    }, // រា
    {
        2, (const uint16_t[]) { 0x179A, 0x17BE }, (const uint16_t[])
        {
            0xE092, 0x17B8, 0
        }
    }, // រើ
    {
        2, (const uint16_t[]) { 0x179A, 0x17BF }, (const uint16_t[])
        {
            0xE092, 0xE034, 0
        }
    }, // រឿ
    {
        2, (const uint16_t[]) { 0x179A, 0x17C0 }, (const uint16_t[])
        {
            0xE092, 0xE035, 0
        }
    }, // រៀ
    {
        2, (const uint16_t[]) { 0x179A, 0x17C1 }, (const uint16_t[])
        {
            0xE092, 0
        }
    }, // រេ
    {
        2, (const uint16_t[]) { 0x179A, 0x17C4 }, (const uint16_t[])
        {
            0xE1C4, 0
        }
    }, // រោ
    {
        2, (const uint16_t[]) { 0x179B, 0x17B6 }, (const uint16_t[])
        {
            0xE169, 0
        }
    }, // លា
    {
        2, (const uint16_t[]) { 0x179B, 0x17BE }, (const uint16_t[])
        {
            0xE093, 0x17B8, 0
        }
    }, // លើ
    {
        2, (const uint16_t[]) { 0x179B, 0x17BF }, (const uint16_t[])
        {
            0xE093, 0xE034, 0
        }
    }, // លឿ
    {
        2, (const uint16_t[]) { 0x179B, 0x17C0 }, (const uint16_t[])
        {
            0xE093, 0xE035, 0
        }
    }, // លៀ
    {
        2, (const uint16_t[]) { 0x179B, 0x17C1 }, (const uint16_t[])
        {
            0xE093, 0
        }
    }, // លេ
    {
        2, (const uint16_t[]) { 0x179B, 0x17C2 }, (const uint16_t[])
        {
            0xE0B7, 0
        }
    }, // លែ
    {
        2, (const uint16_t[]) { 0x179B, 0x17C4 }, (const uint16_t[])
        {
            0xE1C5, 0
        }
    }, // លោ
    {
        2, (const uint16_t[]) { 0x179C, 0x17B6 }, (const uint16_t[])
        {
            0xE16A, 0
        }
    }, // វា
    {
        2, (const uint16_t[]) { 0x179C, 0x17BE }, (const uint16_t[])
        {
            0xE094, 0x17B8, 0
        }
    }, // វើ
    {
        2, (const uint16_t[]) { 0x179C, 0x17C1 }, (const uint16_t[])
        {
            0xE094, 0
        }
    }, // វេ
    {
        2, (const uint16_t[]) { 0x179C, 0x17C2 }, (const uint16_t[])
        {
            0xE0B8, 0
        }
    }, // វែ
    {
        2, (const uint16_t[]) { 0x179C, 0x17C3 }, (const uint16_t[])
        {
            0xE0DC, 0
        }
    }, // វៃ
    {
        2, (const uint16_t[]) { 0x179F, 0x17B6 }, (const uint16_t[])
        {
            0xE16D, 0
        }
    }, // សា
    {
        2, (const uint16_t[]) { 0x179F, 0x17BE }, (const uint16_t[])
        {
            0xE097, 0x17B8, 0
        }
    }, // សើ
    {
        2, (const uint16_t[]) { 0x179F, 0x17C0 }, (const uint16_t[])
        {
            0xE097, 0xE035, 0
        }
    }, // សៀ
    {
        2, (const uint16_t[]) { 0x179F, 0x17C1 }, (const uint16_t[])
        {
            0xE097, 0
        }
    }, // សេ
    {
        2, (const uint16_t[]) { 0x179F, 0x17C2 }, (const uint16_t[])
        {
            0xE0BB, 0
        }
    }, // សែ
    {
        2, (const uint16_t[]) { 0x179F, 0x17C3 }, (const uint16_t[])
        {
            0xE0DF, 0
        }
    }, // សៃ
    {
        2, (const uint16_t[]) { 0x179F, 0x17C4 }, (const uint16_t[])
        {
            0xE097, 0x17B6, 0
        }
    }, // សោ
    {
        2, (const uint16_t[]) { 0x179F, 0x17C5 }, (const uint16_t[])
        {
            0xE1ED, 0
        }
    }, // សៅ
    {
        2, (const uint16_t[]) { 0x17A0, 0x17B6 }, (const uint16_t[])
        {
            0xE16E, 0
        }
    }, // ហា
    {
        2, (const uint16_t[]) { 0x17A0, 0x17BE }, (const uint16_t[])
        {
            0xE098, 0x17B8, 0
        }
    }, // ហើ
    {
        2, (const uint16_t[]) { 0x17A0, 0x17C0 }, (const uint16_t[])
        {
            0xE098, 0xE035, 0
        }
    }, // ហៀ
    {
        2, (const uint16_t[]) { 0x17A0, 0x17C1 }, (const uint16_t[])
        {
            0xE098, 0
        }
    }, // ហេ
    {
        2, (const uint16_t[]) { 0x17A0, 0x17C2 }, (const uint16_t[])
        {
            0xE0BC, 0
        }
    }, // ហែ
    {
        2, (const uint16_t[]) { 0x17A0, 0x17C5 }, (const uint16_t[])
        {
            0xE1EE, 0
        }
    }, // ហៅ
    {
        2, (const uint16_t[]) { 0x17A1, 0x17B6 }, (const uint16_t[])
        {
            0x1791, 0xE174, 0
        }
    }, // ឡា
    {
        2, (const uint16_t[]) { 0x17A1, 0x17BE }, (const uint16_t[])
        {
            0xE099, 0x17B8, 0
        }
    }, // ឡើ
    {
        2, (const uint16_t[]) { 0x17A1, 0x17C1 }, (const uint16_t[])
        {
            0xE099, 0
        }
    }, // ឡេ
    {
        2, (const uint16_t[]) { 0x17A1, 0x17C2 }, (const uint16_t[])
        {
            0xE0BD, 0
        }
    }, // ឡែ
    {
        2, (const uint16_t[]) { 0x17A1, 0x17C3 }, (const uint16_t[])
        {
            0xE0E1, 0
        }
    }, // ឡៃ
    {
        2, (const uint16_t[]) { 0x17A1, 0x17C4 }, (const uint16_t[])
        {
            0xE099, 0x17B6, 0
        }
    }, // ឡោ
    {
        2, (const uint16_t[]) { 0x17A2, 0x17B6 }, (const uint16_t[])
        {
            0xE170, 0
        }
    }, // អា
    {
        2, (const uint16_t[]) { 0x17A2, 0x17BE }, (const uint16_t[])
        {
            0xE09A, 0x17B8, 0
        }
    }, // អើ
    {
        2, (const uint16_t[]) { 0x17A2, 0x17C0 }, (const uint16_t[])
        {
            0xE09A, 0x17C0, 0
        }
    }, // អៀ
    {
        2, (const uint16_t[]) { 0x17A2, 0x17C1 }, (const uint16_t[])
        {
            0xE09A, 0
        }
    }, // អេ
    {
        2, (const uint16_t[]) { 0x17A2, 0x17C2 }, (const uint16_t[])
        {
            0xE0BE, 0
        }
    }, // អែ
    {
        2, (const uint16_t[]) { 0x17A2, 0x17C4 }, (const uint16_t[])
        {
            0xE1CC, 0
        }
    }, // អោ
    {
        2, (const uint16_t[]) { 0x17C0, 0x17D7 }, (const uint16_t[])
        {
            0x17C0, 0x17D4, 0
        }
    }, // ៀៗ
};

const static TAG_MAP s_cvt_map_hi[] =
{
    // 中间0x902变形情况
    {
        2, (const uint16_t[]) { 0x0948, 0x0902 }, (const uint16_t[])
        {
            0xE13F, 0
        }
    }, // मैं 的头花
    // 中间0x93C变形情况
    {
        4, (const uint16_t[]) { 0x092B, 0x093C, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0xE06D, 0x0932, 0
        }
    }, // फ़्ल
    {
        2, (const uint16_t[]) { 0x0916, 0x093C }, (const uint16_t[])
        {
            0x0959, 0
        }
    }, // ख़

    // 中间0x94D变形情况
    {
        12, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x0924, 0x094D, 0x0930, 0x094D, 0x0930, 0x094D, 0x092F, 0x093F }, (const uint16_t[])
        {
            0xE0EB, 0xE088, 0x094D, 0x0930, 0x094D, 0xE1DA, 0x092F, 0
        }
    }, // क्ष्त्र्र्यि
    {
        11, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x092F, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0EB, 0xE04B, 0xE092, 0xE252, 0x092F, 0
        }
    },  // क्ष्य्य्र्य
    {
        11, (const uint16_t[]) { 0x091c, 0x094d, 0x091e, 0x094d, 0x0930, 0x094d, 0x092f, 0x094d, 0x092f, 0x094d, 0x092f }, (const uint16_t[])
        {
            0xE0EC, 0xE04B, 0x092F, 0x094D, 0x092F, 0
        }
    },  // ज्ञ्र्य्य्य
    {
        10, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F, 0x094D }, (const uint16_t[])
        {
            0xE0EC, 0xE04B, 0x092F, 0x094D, 0
        }
    },  // ज्ञ्र्य्य्
    {
        10, (const uint16_t[]) { 0x0930, 0x094D, 0x092E, 0x00A0, 0x091C, 0x094B, 0x0921, 0x093C, 0x0947, 0x0902 }, (const uint16_t[])
        {
            0x092E, 0xE030, 0x0020, 0xE039, 0xE149, 0x095C, 0xE13C, 0
        }
    }, // र्म जोड़ें

    {
        9, (const uint16_t[]) { 0x0928, 0x094D, 0x0928, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0xE045, 0xE0E0, 0x092F, 0x094D, 0
        }
    },  // न्न्य्र्य्य
    {
        9, (const uint16_t[]) { 0x0928, 0x094D, 0x0927, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x094F }, (const uint16_t[])
        {
            0xE045, 0xE0D9, 0xE04B, 0x092F, 0
        }
    },  // न्ध्र्य्य
    {
        9, (const uint16_t[]) { 0x0922, 0x094D, 0x092F, 0x094D, 0x092F, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE03F, 0xE04B, 0xE04B, 0xE04B, 0x092F, 0
        }
    },  // ढ्य्य्य्य
    {
        9, (const uint16_t[]) { 0x091F, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE0D8, 0xE04B, 0xE092, 0
        }
    },  // ट्र्य्य्र
    {
        9, (const uint16_t[]) { 0x0918, 0x094d, 0x0930, 0x094d, 0x092f, 0x094d, 0x092f, 0x094d, 0x0930 }, (const uint16_t[])
        {
            0xE0C5, 0xE04B, 0xE092, 0
        }
    },  // घ्र्य्य्र
    {
        9, (const uint16_t[]) { 0x0916, 0x094D, 0x092F, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE033, 0xE04B, 0xE0E0, 0x092F, 0
        }
    },  // ख्य्य्र्य
    {
        9, (const uint16_t[]) { 0x0930, 0x094D, 0x092E, 0x094D, 0x092F, 0x094D, 0x092E, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04A, 0xE04B, 0xE04A, 0x092F, 0xE030
        }
    },  // र्म्य्म्य
    {
        9, (const uint16_t[]) { 0x0930, 0x094D, 0x092E, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0, 0xE04A, 0xE0E0, 0x092F, 0xE030, 0
        }
    }, // र्म्य्र्य
    {
        9, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x092E, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE056, 0xE0DF, 0x092F, 0
        }
    }, // क्ष्म्र्य
    {
        9, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x093E, 0x0928, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE02F, 0x093E, 0xE0DA, 0x092F, 0
        }
    }, // ज्ञान्र्य
    {
        9, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x094D, 0x0930, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0EC, 0x0930, 0x094D, 0x092F, 0
        }
    }, // ज्ञ्र्र्य
    {
        9, (const uint16_t[]) { 0x0928, 0x094D, 0x092F, 0x093E, 0x0928, 0x094D, 0x0930, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE045, 0x092F, 0x093E, 0xE0DA, 0x0930, 0
        }
    }, // न्यान्र्र
    {
        9, (const uint16_t[]) { 0x092A, 0x094D, 0x0930, 0x094D, 0x092F, 0x093E, 0x0930, 0x094D, 0x0925 }, (const uint16_t[])
        {
            0xE0DB, 0x092F, 0x093E, 0x0925, 0xE030, 0
        }
    }, // प्र्यार्थ
    {
        9, (const uint16_t[]) { 0x0938, 0x094D, 0x0924, 0x094D, 0x0930, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE052, 0xE0D6, 0x0930, 0x094D, 0x092F, 0
        }
    }, // स्त्र्र्य

    {
        8, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x0924, 0x094D, 0x0930, 0x094D }, (const uint16_t[])
        {
            0xE056, 0xE088, 0x094D, 0
        }
    }, // क्ष्त्र्

    {
        7, (const uint16_t[]) { 0x0939, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0E8, 0xE04B, 0x092F, 0
        }
    },  // ह्र्य्य
    {
        7, (const uint16_t[]) { 0x0939, 0x094D, 0x092E, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE053, 0xE04A, 0xE04B, 0x092F, 0
        }
    },  // ह्म्य्य
    {
        7, (const uint16_t[]) { 0x0938, 0x094D, 0x0935, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE052, 0xE04F, 0xE04B, 0x092F, 0
        }
    },  // स्व्य्य
    {
        7, (const uint16_t[]) { 0x0938, 0x094D, 0x0924, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE052, 0xE0D6, 0x092F, 0
        }
    },  // स्त्र्य
    {
        7, (const uint16_t[]) { 0x0938, 0x094D, 0x0915, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE052, 0xE032, 0x092F, 0
        }
    },  // स्क्र्य
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0E5, 0xE04B, 0x092F, 0
        }
    },  // श्र्य्य
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x0930, 0x094D, 0x092E, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0E5, 0xE04A, 0x092F, 0
        }
    },  // श्र्म्य
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x092E, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE050, 0xE04A, 0xE092, 0
        }
    },  // श्म्य्र
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x091A, 0x094D, 0x0930, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE050, 0xE0C8, 0x092E, 0
        }
    },  // श्च्र्म
    {
        7, (const uint16_t[]) { 0x0935, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0E4, 0xE04B, 0x092F, 0
        }
    },  // व्र्य्य
    {
        7, (const uint16_t[]) { 0x0935, 0x094D, 0x092F, 0x094D, 0x0935, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE04F, 0xE04B, 0xE096, 0
        }
    },  // व्य्व्र
    {
        7, (const uint16_t[]) { 0x0935, 0x094D, 0x092F, 0x094D, 0x0935, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04F, 0xE04B, 0xE04F, 0x092F, 0
        }
    },  // व्य्व्य
    {
        7, (const uint16_t[]) { 0x0933, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04E, 0xE0E0, 0x092F, 0
        }
    },  // ळ्य्र्य
    {
        7, (const uint16_t[]) { 0x0932, 0x094D, 0x0932, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04D, 0xE04D, 0xE04B, 0x092F, 0
        }
    },  // ल्ल्य्य
    {
        7, (const uint16_t[]) { 0x0932, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04D, 0xE0E0, 0x092F, 0
        }
    },  // ल्य्र्य
    {
        7, (const uint16_t[]) { 0x092F, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04B, 0xE0E0, 0x092F, 0
        }
    },  // य्य्र्य
    {
        7, (const uint16_t[]) { 0x092E, 0x094D, 0x092D, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04A, 0xE0DE, 0x092F, 0
        }
    },  // म्भ्र्य
    {
        7, (const uint16_t[]) { 0x092E, 0x094D, 0x092C, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE04A, 0xE048, 0xE092, 0
        }
    },  // म्ब्य्र
    {
        7, (const uint16_t[]) { 0x092E, 0x094D, 0x092A, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04A, 0xE0DB, 0x092F, 0
        }
    },  // म्प्र्य
    {
        7, (const uint16_t[]) { 0x092D, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0DE, 0xE04B, 0x092F, 0
        }
    },  // भ्र्य्य
    {
        7, (const uint16_t[]) { 0x092D, 0x094D, 0x0930, 0x094D, 0x092E, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0DE, 0xE04A, 0x092F, 0
        }
    },  // भ्र्म्य
    {
        7, (const uint16_t[]) { 0x092C, 0x094D, 0x0927, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE048, 0xE0D9, 0x092F, 0
        }
    },  // ब्ध्र्य
    {
        7, (const uint16_t[]) { 0x092B, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0DC, 0xE04B, 0x092F, 0
        }
    },  // फ्र्य्य
    {
        7, (const uint16_t[]) { 0x092A, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0DB, 0xE04B, 0x092F, 0
        }
    },  // प्र्य्य
    {
        7, (const uint16_t[]) { 0x092A, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE046, 0xE0E0, 0x092F, 0
        }
    },  // प्य्र्य
    {
        7, (const uint16_t[]) { 0x092A, 0x094D, 0x0924, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE046, 0xE0D6, 0x092F, 0
        }
    },  // प्त्र्य
    {
        7, (const uint16_t[]) { 0x0928, 0x094D, 0x092F, 0x094D, 0x0928, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0xE04B, 0xE045, 0x092F, 0
        }
    },  // न्य्न्य
    {
        7, (const uint16_t[]) { 0x0928, 0x094D, 0x0928, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0xE0DA, 0x092F, 0
        }
    },  // न्न्र्य
    {
        7, (const uint16_t[]) { 0x0928, 0x094D, 0x0928, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0xE045, 0xE04B, 0x092F, 0
        }
    },  // न्न्य्य
    {
        7, (const uint16_t[]) { 0x0928, 0x094D, 0x0927, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0xE0D9, 0x092F, 0
        }
    },  // न्ध्र्य
    {
        7, (const uint16_t[]) { 0x0928, 0x094D, 0x0926, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0xE0D8, 0x092F, 0
        }
    },  // न्द्र्य
    {
        7, (const uint16_t[]) { 0x0927, 0x094D, 0x0930, 0x094D, 0x092E, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0D9, 0xE04A, 0x092F, 0
        }
    },  // ध्र्म्य
    {
        7, (const uint16_t[]) { 0x0926, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0D8, 0xE04B, 0x092F, 0
        }
    },  // द्र्य्य
    {
        7, (const uint16_t[]) { 0x0926, 0x094D, 0x0917, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE043, 0xE0C4, 0x092F, 0
        }
    },  // द्ग्र्य
    {
        7, (const uint16_t[]) { 0x0925, 0x094D, 0x092F, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE042, 0xE04B, 0xE04B, 0x092F, 0
        }
    },  // थ्य्य्य
    {
        7, (const uint16_t[]) { 0x0924, 0x094D, 0x0938, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE041, 0xE0E7, 0x092F, 0
        }
    },  // त्स्र्य
    {
        7, (const uint16_t[]) { 0x0924, 0x094D, 0x092F, 0x094D, 0x092E, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE041, 0xE04B, 0xE04A, 0x092E, 0
        }
    },  // त्य्म्म
    {
        7, (const uint16_t[]) { 0x0924, 0x094D, 0x0924, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE041, 0xE0D6, 0x092F, 0
        }
    },  // त्त्र्य
    {
        7, (const uint16_t[]) { 0x0922, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE03F, 0xE04B, 0xE092, 0
        }
    },  // ढ्य्य्र
    {
        7, (const uint16_t[]) { 0x0921, 0x094D, 0x0922, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE03E, 0xE03F, 0x1CED, 0xE04B, 0x092F, 0
        }
    },  // ड्ढ्य्य
    {
        7, (const uint16_t[]) { 0x091F, 0x094D, 0x091F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE03C, 0xE0CD, 0x092F, 0
        }
    },  // ट्ट्र्य
    {
        7, (const uint16_t[]) { 0x091C, 0x094D, 0x0935, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE039, 0xE04F, 0xE092, 0
        }
    },  // ज्व्य्र
    {
        7, (const uint16_t[]) { 0x091C, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0CA, 0xE04B, 0x092F, 0
        }
    },  // ज्र्य्य
    {
        7, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0EC, 0x092F, 0
        }
    },  // ज्ञ्र्य
    {
        7, (const uint16_t[]) { 0x091C, 0x094D, 0x091C, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE039, 0xE0CA, 0x092F, 0
        }
    },  // ज्ज्र्य
    {
        7, (const uint16_t[]) { 0x091B, 0x094D, 0x092F, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE038, 0xE04B, 0xE092, 0
        }
    },  // छ्य्य्र
    {
        7, (const uint16_t[]) { 0x091A, 0x094D, 0x091A, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE037, 0xE037, 0xE092, 0
        }
    },  // च्च्य्र
    {
        7, (const uint16_t[]) { 0x0918, 0x094D, 0x092E, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE035, 0xE04A, 0xE092, 0
        }
    },  // घ्म्य्र
    {
        7, (const uint16_t[]) { 0x0917, 0x094D, 0x0930, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE0C4, 0xE092, 0
        }
    },  // ग्र्य्र
    {
        7, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0EB, 0x092F, 0
        }
    },  // क्ष्र्य
    {
        7, (const uint16_t[]) { 0x0915, 0x094D, 0x0915, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE032, 0xE0C2, 0x092F, 0
        }
    },  // क्क्र्य
    {
        7, (const uint16_t[]) { 0x0915, 0x094D, 0x0915, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE032, 0xE032, 0xE04B, 0x092F, 0
        }
    },  // क्क्य्य
    {
        7, (const uint16_t[]) { 0x0917, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE034, 0xE0E0, 0x092F, 0
        }
    },  // ग्य्र्य
    {
        7, (const uint16_t[]) { 0x0927, 0x094d, 0x0935, 0x094d, 0x0930, 0x094d, 0x092f }, (const uint16_t[])
        {
            0xE044, 0xE0E4, 0x092F, 0
        }
    }, // ध्व्र्य
    {
        7, (const uint16_t[]) { 0x092c, 0x094d, 0x0930, 0x094d, 0x092f, 0x094d, 0x092f }, (const uint16_t[])
        {
            0xE0DD, 0xE04B, 0x092F, 0
        }
    }, // ब्र्य्य
    {
        7, (const uint16_t[]) { 0x0937, 0x094d, 0x0925, 0x094d, 0x092f, 0x094d, 0x0930 }, (const uint16_t[])
        {
            0xE051, 0xE042, 0xE092, 0
        }
    }, // ष्थ्य्र
    {
        7, (const uint16_t[]) { 0x0930, 0x094d, 0x092d, 0x094d, 0x0930, 0x094d, 0x092f }, (const uint16_t[])
        {
            0xE0DE, 0x092F, 0xE030, 0
        }
    }, // र्भ्र्य
    {
        7, (const uint16_t[]) { 0x0915, 0x094D, 0x0932, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE032, 0xE0E2, 0x092F, 0
        }
    }, // क्ल्र्य
    {
        7, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE056, 0xE092, 0
        }
    }, // क्ष्य्र
    {
        7, (const uint16_t[]) { 0x0917, 0x094D, 0x0930, 0x094D, 0x0930, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE07B, 0x094D, 0xE0E0, 0xE031, 0
        }
    }, // ग्र्र्र
    {
        7, (const uint16_t[]) { 0x0918, 0x094D, 0x0928, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE044, 0xE0DA, 0x092F, 0
        }
    }, // घ्न्र्य
    {
        7, (const uint16_t[]) { 0x0915, 0x094D, 0x0932, 0x094D, 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE032, 0xE04D, 0xE04D, 0x092F, 0
        }
    }, // क्ल्ल्य
    {
        7, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x094D, 0x0924, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE057, 0xE088, 0
        }
    }, // ज्ञ्त्र
    {
        7, (const uint16_t[]) { 0x0924, 0x094D, 0x0930, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0D6, 0x0930, 0x094D, 0x092F, 0
        }
    }, // त्र्र्य
    {
        7, (const uint16_t[]) { 0x0926, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE043, 0xE0E0, 0x092F, 0
        }
    }, // द्य्र्य
    {
        7, (const uint16_t[]) { 0x0927, 0x094D, 0x0928, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE044, 0xE0DA, 0x092F, 0
        }
    }, // ध्न्र्य
    {
        7, (const uint16_t[]) { 0x0928, 0x094D, 0x0924, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0xE088, 0x092F, 0
        }
    }, // न्त्र्य
    {
        7, (const uint16_t[]) { 0x0928, 0x094D, 0x092F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0xE0E0, 0x092F, 0
        }
    }, // न्य्र्य
    {
        7, (const uint16_t[]) { 0x092A, 0x094D, 0x0930, 0x094D, 0x0927, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0DB, 0xE044, 0x092F, 0
        }
    }, // प्र्ध्य
    {
        7, (const uint16_t[]) { 0x092E, 0x094D, 0x0928, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04A, 0xE0DF, 0x092F, 0
        }
    }, // म्न्र्य
    {
        7, (const uint16_t[]) { 0x0932, 0x094D, 0x0932, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04D, 0xE0E2, 0x092F, 0
        }
    }, // ल्ल्र्य
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x091A, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE050, 0xE037, 0xE092, 0
        }
    }, // श्च्य्र
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x091A, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE050, 0xE0C8, 0x092F, 0
        }
    }, // श्च्र्य
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x0932, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE050, 0xE0E2, 0x092F, 0
        }
    }, // श्ल्र्य
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x0932, 0x094D, 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE050, 0xE04D, 0xE04D, 0x092F, 0
        }
    }, // श्ल्ल्य
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x0932, 0x094D, 0x0932, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE050, 0xE04D, 0xE094, 0
        }
    }, // श्ल्ल्र
    {
        7, (const uint16_t[]) { 0x0936, 0x094D, 0x0935, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE050, 0xE0E4, 0x092F, 0
        }
    }, // श्व्र्य
    {
        7, (const uint16_t[]) { 0x0937, 0x094D, 0x092A, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE051, 0xE0DB, 0x092F, 0
        }
    }, // ष्प्र्य
    {
        7, (const uint16_t[]) { 0x0937, 0x094D, 0x092A, 0x094D, 0x0930, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE051, 0xE08D, 0x1CED, 0x0930, 0
        }
    }, // ष्प्र्र
    {
        7, (const uint16_t[]) { 0x0938, 0x094D, 0x0916, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE052, 0xE0C3, 0x092F, 0
        }
    }, // स्ख्र्य
    {
        7, (const uint16_t[]) { 0x0938, 0x094D, 0x091F, 0x093E, 0x0930, 0x094D, 0x091F }, (const uint16_t[])
        {
            0xE052, 0x091F, 0x093E, 0x091F, 0xE030, 0
        }
    }, // स्टार्ट
    {
        7, (const uint16_t[]) { 0x0915, 0x0949, 0x0930, 0x094D, 0x0921, 0x00A0, 0x0928 }, (const uint16_t[])
        {
            0x0915, 0x0949, 0xE22C, 0x20, 0x0928, 0xE030, 0
        }
    }, // कॉर्ड न
    {
        7, (const uint16_t[]) { 0x0938, 0x094D, 0x092A, 0x0940, 0x0928, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0xE052, 0x092A, 0xE208, 0xE1E7, 0x0928, 0
        }
    }, // स्पीनिं
    {
        7, (const uint16_t[]) { 0x092C, 0x094B, 0x0930, 0x094D, 0x0921, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0x092C, 0x094B, 0xE1FE, 0x0921, 0
        }
    }, // बोर्डिं
    {
        7, (const uint16_t[]) { 0x0926, 0x0930, 0x094D, 0x092D, 0x00A0, 0x0915, 0x0947 }, (const uint16_t[])
        {
            0x0926, 0x092D, 0xE030, 0x0020, 0x0915, 0x0947, 0
        }
    }, // दर्भ के

    {
        6, (const uint16_t[]) { 0x0937, 0x094D, 0x091F, 0x094D, 0x0930, 0x0940 }, (const uint16_t[])
        {
            0xE1AF, 0x0940, 0
        }
    }, // ष्ट्री
    {
        6, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x0935, 0x0930 }, (const uint16_t[])
        {
            0xE056, 0x0935, 0x0930, 0
        }
    },  // क्ष्वर
    {
        6, (const uint16_t[]) { 0x0938, 0x094D, 0x091F, 0x094D, 0x0930, 0x0948 }, (const uint16_t[])
        {
            0xE052, 0xE083, 0x0948, 0
        }
    }, // स्ट्रै
    {
        6, (const uint16_t[]) { 0x0938, 0x094D, 0x091F, 0x094D, 0x0930, 0x094B }, (const uint16_t[])
        {
            0xE052, 0xE083, 0x094B, 0
        }
    }, // स्ट्रो
    {
        6, (const uint16_t[]) { 0x0938, 0x094D, 0x091F, 0x094D, 0x0930, 0x0940 }, (const uint16_t[])
        {
            0xE052, 0xE0CD, 0xE208, 0
        }
    }, // स्ट्री
    {
        6, (const uint16_t[]) { 0x0930, 0x094D, 0x0927, 0x093E, 0x0930, 0x093F }, (const uint16_t[])
        {
            0x0927, 0x093E, 0xE030, 0xE1D8, 0x0930, 0
        }
    }, // र्धारि
    {
        6, (const uint16_t[]) { 0x0930, 0x094D, 0x092E, 0x00A0, 0x0915, 0x094B }, (const uint16_t[])
        {
            0x092E, 0xE030, 0x0915, 0xE149, 0
        }
    }, // र्म को
    {
        6, (const uint16_t[]) { 0x0938, 0x094D, 0x092A, 0x094D, 0x0932, 0x0947 }, (const uint16_t[])
        {
            0xE052, 0xE046, 0xE04D, 0x094B, 0
        }
    }, // स्प्ले
    {
        6, (const uint16_t[]) { 0x0938, 0x094D, 0x0915, 0x094D, 0x091F, 0x093F }, (const uint16_t[])
        {
            0xE1D7, 0xE052, 0xE032, 0x091F, 0
        }
    }, // स्क्टि
    {
        6, (const uint16_t[]) { 0x0938, 0x094D, 0x0915, 0x094D, 0x0935, 0x093E }, (const uint16_t[])
        {
            0xE052, 0xE032, 0x0935, 0x093E, 0
        }
    }, // स्क्वा
    {
        6, (const uint16_t[]) { 0x0930, 0x094D, 0x092E, 0x00A0, 0x091C, 0x094B }, (const uint16_t[])
        {
            0x092E, 0xE030, 0xE000, 0xE039, 0x093E, 0x094B, 0
        }
    }, // र्म जो
    {
        6, (const uint16_t[]) { 0x0930, 0x094D, 0x092D, 0x00A0, 0x0915, 0x0947 }, (const uint16_t[])
        {
            0x092D, 0x0020, 0x0915, 0x0947, 0
        }
    }, // र्भ के
    {
        6, (const uint16_t[]) { 0x0930, 0x094D, 0x092B, 0X093C, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0xE1FE, 0x095E, 0
        }
    }, // र्फ़िं
    {
        6, (const uint16_t[]) { 0x0937, 0x094D, 0x0915, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1E2, 0xE0E6, 0x0915, 0
        }
    },  // ष्क्रि
    {
        6, (const uint16_t[]) { 0x0926, 0x0930, 0x094D, 0x092D, 0x093F, 0x0924 }, (const uint16_t[])
        {
            0x0926, 0xE1F5, 0xE049, 0x093E, 0x0924, 0
        }
    }, // दर्भित
    {
        5, (const uint16_t[]) { 0x092B, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0DC, 0x092F, 0
        }
    },  // फ्र्य
    {
        5, (const uint16_t[]) { 0x0939, 0x094D, 0x092E, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE053, 0xE04A, 0x092F, 0
        }
    },  // ह्म्य्य
    {
        5, (const uint16_t[]) { 0x0939, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0E8, 0x092F, 0
        }
    },  // ह्र्य
    {
        5, (const uint16_t[]) { 0x0939, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE053, 0xE04B, 0x092F, 0
        }
    },  // ह्य्य
    {
        5, (const uint16_t[]) { 0x0939, 0x094D, 0x092E, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE053, 0xE04A, 0x092E, 0
        }
    },  // ह्म्म
    {
        5, (const uint16_t[]) { 0x0938, 0x094D, 0x092A, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE052, 0xE08D, 0
        }
    },  // स्प्र
    {
        5, (const uint16_t[]) { 0x0938, 0x094D, 0x0925, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE052, 0xE042, 0x092E, 0
        }
    },  // स्थ्म
    {
        5, (const uint16_t[]) { 0x0938, 0x094D, 0x0915, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE052, 0xE032, 0x092F, 0
        }
    },  // स्क्य
    {
        5, (const uint16_t[]) { 0x0937, 0x094D, 0x0923, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE051, 0xE040, 0x092F, 0
        }
    },  // ष्ण्य
    {
        5, (const uint16_t[]) { 0x0937, 0x094D, 0x0920, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE1B1, 0
        }
    },  // ष्ठ्य
    {
        5, (const uint16_t[]) { 0x0937, 0x094D, 0x0915, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE051, 0xE032, 0x092F, 0
        }
    },  // ष्क्य
    {
        5, (const uint16_t[]) { 0x0936, 0x094D, 0x0935, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE050, 0xE096, 0
        }
    },  // श्व्र
    {
        5, (const uint16_t[]) { 0x0936, 0x094D, 0x0924, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE050, 0xE041, 0x092F, 0
        }
    },  // श्त्य
    {
        5, (const uint16_t[]) { 0x0936, 0x094D, 0x091A, 0x094D, 0x091B }, (const uint16_t[])
        {
            0xE050, 0xE037, 0x091B, 0
        }
    },  // श्च्छ
    {
        5, (const uint16_t[]) { 0x0935, 0x094D, 0x0939, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE04F, 0xE09A, 0
        }
    },  // व्ह्र
    {
        5, (const uint16_t[]) { 0x0935, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE04F, 0xE092, 0
        }
    },  // व्य्र
    {
        5, (const uint16_t[]) { 0x0935, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04F, 0xE04B, 0x092F, 0
        }
    },  // व्य्य
    {
        5, (const uint16_t[]) { 0x0932, 0x094D, 0x0939, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04D, 0xE1BC, 0
        }
    },  // ल्ह्य
    {
        5, (const uint16_t[]) { 0x0932, 0x094D, 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04D, 0xE04D, 0x092F, 0
        }
    },  // ल्ल्य
    {
        5, (const uint16_t[]) { 0x0932, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04D, 0xE04B, 0x092F, 0
        }
    },  // ल्य्य
    {
        5, (const uint16_t[]) { 0x0932, 0x094D, 0x0915, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE04D, 0xE079, 0
        }
    },  // ल्क्र
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0935, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04F, 0x092F, 0xE030, 0
        }
    },  // र्व्य
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0932, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0xE04D, 0x0932, 0xE030, 0
        }
    },  // र्ल्ल
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x092E, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04A, 0x092F, 0xE030, 0
        }
    },  // र्म्य
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x092D, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE090, 0xE030, 0
        }
    },  // र्भ्र
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x092A, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE046, 0x092F, 0xE030, 0
        }
    },  // र्प्य
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0928, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0x092F, 0xE030, 0
        }
    },  // र्न्य
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0928, 0x094D, 0x0928 }, (const uint16_t[])
        {
            0xE19C, 0xE030, 0
        }
    },  // र्न्न
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0917, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE034, 0x092F, 0xE030, 0
        }
    },  // र्ग्य
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0916, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE033, 0x092F, 0xE030, 0
        }
    },  // र्ख्य
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0915, 0x094D, 0x0937 }, (const uint16_t[])
        {
            0xE02E, 0xE030, 0
        }
    },  // र्क्ष
    {
        5, (const uint16_t[]) { 0x092E, 0x094D, 0x092E, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04A, 0xE04A, 0x092F, 0
        }
    },  // म्म्य
    {
        5, (const uint16_t[]) { 0x092E, 0x094D, 0x092C, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04A, 0xE048, 0x092F, 0
        }
    },  // म्ब्य
    {
        5, (const uint16_t[]) { 0x092D, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE049, 0xE092, 0
        }
    },  // भ्य्र
    {
        5, (const uint16_t[]) { 0x092D, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE049, 0xE04B, 0x092F, 0
        }
    },  // भ्य्य
    {
        5, (const uint16_t[]) { 0x092C, 0x094D, 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE048, 0xE04D, 0x092F, 0
        }
    },  // ब्ल्य
    {
        5, (const uint16_t[]) { 0x092C, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE048, 0xE04B, 0x092F, 0
        }
    },  // ब्य्य
    {
        5, (const uint16_t[]) { 0x092C, 0x094D, 0x0927, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE048, 0xE044, 0x0935, 0
        }
    },  // ब्ध्व
    {
        5, (const uint16_t[]) { 0x092C, 0x094D, 0x0927, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE048, 0xE08B, 0
        }
    },  // ब्ध्र
    {
        5, (const uint16_t[]) { 0x092C, 0x094D, 0x0927, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE048, 0xE044, 0x092F, 0
        }
    },  // ब्ध्य
    {
        5, (const uint16_t[]) { 0x092B, 0x094D, 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE047, 0xE04D, 0x092F, 0
        }
    },  // फ्ल्य
    {
        5, (const uint16_t[]) { 0x092A, 0x094D, 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE046, 0xE04D, 0x092F, 0
        }
    },  // प्ल्य
    {
        5, (const uint16_t[]) { 0x0928, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0xE04B, 0x092F, 0
        }
    },  // न्य्य
    {
        5, (const uint16_t[]) { 0x0928, 0x094D, 0x0924, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE045, 0xE088, 0
        }
    },  // न्त्र
    {
        5, (const uint16_t[]) { 0x0927, 0x094D, 0x0935, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE044, 0xE096, 0
        }
    },  // ध्व्र
    {
        5, (const uint16_t[]) { 0x0926, 0x094D, 0x092E, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE043, 0xE04A, 0x092E, 0
        }
    },  // द्म्म
    {
        5, (const uint16_t[]) { 0x0926, 0x094D, 0x0927, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE043, 0xE044, 0x0935, 0
        }
    },  // द्ध्व
    {
        5, (const uint16_t[]) { 0x0926, 0x094D, 0x0927, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE18F, 0
        }
    },  // द्ध्य
    {
        5, (const uint16_t[]) { 0x0924, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0D6, 0x092F, 0
        }
    },  // त्र्य
    {
        5, (const uint16_t[]) { 0x0924, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE041, 0xE04B, 0x092F, 0
        }
    },  // त्य्य
    {
        5, (const uint16_t[]) { 0x0924, 0x094D, 0x0925, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE041, 0xE042, 0x092F, 0
        }
    },  // त्थ्य
    {
        5, (const uint16_t[]) { 0x0923, 0x094D, 0x0923, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE040, 0xE040, 0x092F, 0
        }
    },  // ण्ण्य
    {
        5, (const uint16_t[]) { 0x0923, 0x094D, 0x0921, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE040, 0xE085, 0
        }
    },  // ण्ड्र
    {
        5, (const uint16_t[]) { 0x0922, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0D3, 0x092F, 0
        }
    },  // ढ्र्य
    {
        5, (const uint16_t[]) { 0x0922, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE03F, 0xE092, 0
        }
    },  // ढ्य्र
    {
        5, (const uint16_t[]) { 0x0921, 0x094D, 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE03E, 0xE04D, 0x092F, 0
        }
    },  // ड्ल्य
    {
        5, (const uint16_t[]) { 0x0921, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE03E, 0xE04B, 0x092F, 0
        }
    },  // ड्य्य
    {
        5, (const uint16_t[]) { 0x0921, 0x094D, 0x092E, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE03E, 0xE04A, 0x092E, 0
        }
    },  // ड्म्म
    {
        5, (const uint16_t[]) { 0x091F, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0CD, 0x092F, 0
        }
    },  // ट्र्य
    {
        5, (const uint16_t[]) { 0x091D, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0CB, 0x092F, 0
        }
    },  // झ्र्य
    {
        5, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE057, 0x0935, 0
        }
    },  // ज्ञ्व
    {
        5, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE057, 0x092F, 0
        }
    },  // ज्ञ्य
    {
        5, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x094D, 0x091C }, (const uint16_t[])
        {
            0xE057, 0x091C, 0
        }
    },  // ज्ञ्ज
    {
        5, (const uint16_t[]) { 0x091C, 0x094D, 0x091C, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE039, 0xE039, 0x092F, 0
        }
    },  // ज्ज्य
    {
        5, (const uint16_t[]) { 0x091B, 0x094D, 0x0935, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE038, 0xE096, 0
        }
    },  // छ्व्र
    {
        5, (const uint16_t[]) { 0x091A, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE037, 0xE04B, 0x092F, 0
        }
    },  // च्य्य
    {
        5, (const uint16_t[]) { 0x0919, 0x094D, 0x092E, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE036, 0xE04A, 0x092E, 0
        }
    },  // ङ्म्म
    {
        5, (const uint16_t[]) { 0x0918, 0x094D, 0x0935, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE044, 0xE096, 0
        }
    },  // घ्व्र
    {
        5, (const uint16_t[]) { 0x0918, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE044, 0xE04B, 0x092F, 0
        }
    },  // घ्य्य
    {
        5, (const uint16_t[]) { 0x0917, 0x094D, 0x0927, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE034, 0xE044, 0x092F, 0
        }
    },  // ग्ध्य
    {
        5, (const uint16_t[]) { 0x0916, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0C3, 0x092F, 0
        }
    },  // ख्र्य
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x0923 }, (const uint16_t[])
        {
            0xE056, 0x0923, 0
        }
    },  // क्ष्ण
    {
        5, (const uint16_t[]) { 0x0932, 0x094D, 0x0932, 0x094D, 0x0939},  (const uint16_t[])
        {
            0xE04D, 0xE04D, 0x0939, 0
        }
    },  // ल्ल्ह

    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE056, 0x092E, 0
        }
    }, // क्ष्म
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0924, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE032, 0xE088, 0
        }
    }, // क्त्र
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE09E, 0
        }
    },  // क्ष्र
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x092E, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE04A, 0x092E, 0xE030
        }
    }, // र्म्म
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0926, 0x094D, 0x0927 }, (const uint16_t[])
        {
            0xE188, 0xE030, 0
        }
    },  // र्द्ध
    {
        5, (const uint16_t[]) { 0x0937, 0x094D, 0x0920, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE1B2, 0
        }
    }, // ष्ठ्र
    {
        5, (const uint16_t[]) { 0x0938, 0x094D, 0x0924, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE052, 0xE088, 0
        }
    }, // स्त्र
    {
        5, (const uint16_t[]) { 0x0937, 0x094d, 0x092e, 0x094d, 0x092e }, (const uint16_t[])
        {
            0xE051, 0xE04A, 0x092E, 0
        }
    }, // ष्म्म
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x092E, 0x094B, 0x0902 }, (const uint16_t[])
        {
            0x092E, 0xE14A, 0
        }
    }, // र्मों
    {
        5, (const uint16_t[]) { 0x091C, 0x094D, 0x092F, 0x094B, 0x0902 }, (const uint16_t[])
        {
            0xE039, 0x092F, 0xE148, 0
        }
    }, // ज्यों
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0936, 0x094B, 0x0902 }, (const uint16_t[])
        {
            0xE050, 0x093E, 0xE14A, 0
        }
    }, // र्शों
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0C2, 0x092F, 0
        }
    }, // क्र्य
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0932, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE032, 0xE094, 0
        }
    }, // क्ल्र
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE056, 0x092F, 0
        }
    }, // क्ष्य
    {
        5, (const uint16_t[]) { 0x0919, 0x094D, 0x0930, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE0C6, 0x0930, 0
        }
    }, // ङ्र्र
    {
        5, (const uint16_t[]) { 0x091A, 0x094D, 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE037, 0xE092, 0
        }
    }, // च्य्र
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE032, 0xE04D, 0xE092, 0
        }
    }, // क्ल्य
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0C2, 0x092F, 0
        }
    }, // क्र्य
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE056, 0x092F, 0
        }
    }, // क्ष्य
    {
        5, (const uint16_t[]) { 0x091B, 0x094D, 0x0935, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE038, 0xE04F, 0x092F, 0
        }
    }, // छ्व्य
    {
        5, (const uint16_t[]) { 0x091A, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0C8, 0x092F, 0
        }
    }, // च्र्य
    {
        5, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE09F, 0
        }
    }, // ज्ञ्र
    {
        5, (const uint16_t[]) { 0x092C, 0x0930, 0x094D, 0x0930, 0x0947 }, (const uint16_t[])
        {
            0x092C, 0x0930, 0xE13D, 0
        }
    }, // बर्रे
    {
        5, (const uint16_t[]) { 0x092D, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0DE, 0x092F, 0
        }
    }, // भ्र्य
    {
        5, (const uint16_t[]) { 0x092E, 0x094D, 0x092D, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04A, 0xE049, 0x092F, 0
        }
    }, // म्भ्य
    {
        5, (const uint16_t[]) { 0x092E, 0x094D, 0x092D, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE04A, 0xE1D2, 0
        }
    }, // म्भ्र
    {
        5, (const uint16_t[]) { 0x092E, 0x094D, 0x092E, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE1A1, 0
        }
    }, // म्म्र
    {
        5, (const uint16_t[]) { 0x092E, 0x094D, 0x0932, 0x093E, 0x0928 }, (const uint16_t[])
        {
            0xE04A, 0x0932, 0x093E, 0x0928, 0
        }
    }, // म्लान
    {
        5, (const uint16_t[]) { 0x092E, 0x094D, 0x0932, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE04A, 0xE094, 0
        }
    }, // म्ल्र
    {
        5, (const uint16_t[]) { 0x092F, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04B, 0xE04B, 0x092F, 0
        }
    }, // य्य्य
    {
        5, (const uint16_t[]) { 0x0932, 0x094D, 0x0932, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE04D, 0xE094, 0
        }
    }, // ल्ल्र
    {
        5, (const uint16_t[]) { 0x0935, 0x094D, 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE0E4, 0x092F, 0
        }
    }, // व्र्य
    {
        5, (const uint16_t[]) { 0x0936, 0x094D, 0x091A, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE050, 0xE037, 0x092F, 0
        }
    }, // श्च्य
    {
        5, (const uint16_t[]) { 0x0936, 0x094D, 0x091A, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE050, 0xE0A5, 0
        }
    }, // श्च्र
    {
        5, (const uint16_t[]) { 0x0936, 0x094D, 0x092E, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE050, 0xE091, 0
        }
    }, // श्म्र
    {
        5, (const uint16_t[]) { 0x0936, 0x094D, 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE050, 0xE04D, 0x092F, 0
        }
    }, // श्ल्य
    {
        5, (const uint16_t[]) { 0x0936, 0x094D, 0x0932, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE050, 0xE094, 0
        }
    }, // श्ल्र
    {
        5, (const uint16_t[]) { 0x0937, 0x094D, 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE051, 0xE04B, 0x092F, 0
        }
    }, // ष्य्य
    {
        5, (const uint16_t[]) { 0x0938, 0x094D, 0x0916, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE052, 0xE033, 0x092F, 0
        }
    }, // स्ख्य
    {
        5, (const uint16_t[]) { 0x0938, 0x094D, 0x0925, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE052, 0xE042, 0x092F, 0
        }
    }, // स्थ्य
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0921, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0xE1FD, 0x0921, 0
        }
    }, // र्डिं
    {
        5, (const uint16_t[]) { 0x0938, 0x094D, 0x0915, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE052, 0xE079, 0
        }
    }, // स्क्र
    {
        5, (const uint16_t[]) { 0x092C, 0x094B, 0x0930, 0x094D, 0x0921 }, (const uint16_t[])
        {
            0x092C, 0x094B, 0x0921, 0xE030, 0
        }
    }, // बोर्ड
    {
        5, (const uint16_t[]) { 0x092A, 0x094D, 0x0930, 0x0936, 0x093F }, (const uint16_t[])
        {
            0xE08D, 0xE1DD, 0xE050, 0x093E, 0
        }
    }, // प्रशि
    {
        5, (const uint16_t[]) { 0x095E, 0x094D, 0x091F, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0xE1EE, 0xE06D, 0x091F, 0
        }
    }, // फ़्टिं
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x092B, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0xE1FE, 0x092B, 0
        }
    }, // र्फिं
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x095E, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0xE1FE, 0x095E, 0
        }
    }, // र्फ़िं
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0938, 0x094D, 0x0921 }, (const uint16_t[])
        {
            0xE032, 0xE052, 0x0921, 0
        }
    }, // क्स्ड
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0938, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0xE1EE, 0xE032, 0x0938, 0
        }
    }, // क्सिं
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0915, 0x094C, 0x0930 }, (const uint16_t[])
        {
            0x0915, 0xE14C, 0x0930, 0
        }
    }, // र्कौर
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0935, 0x094B, 0x0902 }, (const uint16_t[])
        {
            0xE032, 0x0935, 0x093E, 0xE13C, 0
        }
    }, // क्वों
    {
        5, (const uint16_t[]) { 0x0915, 0x094D, 0x0938, 0x094D, 0x091F }, (const uint16_t[])
        {
            0xE032, 0xE052, 0x091F, 0
        }
    }, // क्स्ट
    {
        5, (const uint16_t[]) { 0x0932, 0x094D, 0x091F, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0xE1EE, 0xE04D, 0x091F, 0
        }
    }, // ल्टिं
    {
        5, (const uint16_t[]) { 0x0930, 0x094D, 0x0932, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0xE200, 0x0932, 0
        }
    }, // र्लिं
    {
        5, (const uint16_t[]) { 0x092B, 0x093C, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0x093F, 0xE0B5, 0
        }
    }, // फ़्रि
    {
        5, (const uint16_t[]) { 0x0928, 0x094D, 0x0939, 0x0947, 0x0902 }, (const uint16_t[])
        {
            0xE045, 0x0939, 0xE13C, 0
        }
    }, // न्हें

    {
        4, (const uint16_t[]) { 0x0917, 0x094D, 0x092E, 0x093F }, (const uint16_t[])
        {
            0xE1EE, 0xE034, 0x092E, 0
        }
    }, // ग्मि
    {
        4, (const uint16_t[]) { 0x0932, 0x094D, 0x091F, 0x0940 }, (const uint16_t[])
        {
            0xE04D, 0x091F, 0x0940, 0
        }
    }, // ल्टी
    {
        4, (const uint16_t[]) { 0x091F, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0x093F, 0xE083, 0
        }
    }, // ट्रि
    {
        4, (const uint16_t[]) { 0x092B, 0x093F, 0x0915, 0x0947 }, (const uint16_t[])
        {
            0xE1DA, 0x092B, 0x0915, 0x0947, 0
        }
    }, // फिके
    {
        4, (const uint16_t[]) { 0x0909, 0x092E, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0x0909, 0xE091, 0
        }
    }, // उम्र
    {
        4, (const uint16_t[]) { 0x0926, 0x094D, 0x0930, 0x0941 }, (const uint16_t[])
        {
            0xE08A, 0x0941, 0
        }
    }, // द्रु
    {
        4, (const uint16_t[]) { 0x0936, 0x094D, 0x0915, 0x093C }, (const uint16_t[])
        {
            0xE050, 0x0958, 0
        }
    }, // श्क़
    {
        4, (const uint16_t[]) { 0x091C, 0x093C, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE05F, 0x092F, 0
        }
    },  // ज़्य
    {
        4, (const uint16_t[]) { 0x092E, 0x094D, 0x092E, 0x093F }, (const uint16_t[])
        {
            0xE1D7, 0xE04A, 0x092E, 0
        }
    },  // म्मि
    {
        4, (const uint16_t[]) { 0x0915, 0x094D, 0x0924, 0x093F }, (const uint16_t[])
        {
            0xE1DC, 0xE169, 0
        }
    },  // क्ति
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x0924, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x0924, 0xE030, 0
        }
    },  // र्ति
    {
        4, (const uint16_t[]) { 0x0915, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0xE079, 0
        }
    },  // क्रि
    {
        4, (const uint16_t[]) { 0x0935, 0x0930, 0x094D, 0x0924 }, (const uint16_t[])
        {
            0x0935, 0x0924, 0xE030
        }
    },  // वर्त
    {
        4, (const uint16_t[]) { 0x0938, 0x0930, 0x094D, 0x0917 }, (const uint16_t[])
        {
            0x0938, 0xE034, 0x093E, 0xE030
        }
    },  // सर्ग
    {
        4, (const uint16_t[]) { 0x092B, 0x094D, 0x092B, 0x093F }, (const uint16_t[])
        {
            0xE1D7, 0xE047, 0x092B, 0
        }
    },  // फ्फि
    {
        4, (const uint16_t[]) { 0x0915, 0x0930, 0x094D, 0x0937 }, (const uint16_t[])
        {
            0x0915, 0x0937, 0xE030, 0
        }
    }, // कर्ष
    {
        4, (const uint16_t[]) { 0x0926, 0x094D, 0x0935, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0xE18C, 0
        }
    }, // द्वि
    {
        4, (const uint16_t[]) { 0x0937, 0x094D, 0x091F, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0xE1AD, 0
        }
    }, // ष्टि
    {
        4, (const uint16_t[]) { 0x091A, 0x094D, 0x091A, 0x093F }, (const uint16_t[])
        {
            0xE1D7, 0xE037, 0xE037, 0x093E, 0
        }
    }, // च्चि
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x092E, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x092E, 0xE030, 0
        }
    }, // र्मि
    {
        4, (const uint16_t[]) { 0x0938, 0x094D, 0x091F, 0x0947 }, (const uint16_t[])
        {
            0xE052, 0x091F, 0x0947, 0
        }
    }, // स्टे
    {
        4, (const uint16_t[]) { 0x0938, 0x094D, 0x0925, 0x093F }, (const uint16_t[])
        {
            0xE1D7, 0xE052, 0x0925, 0
        }
    }, // स्थि
    {
        4, (const uint16_t[]) { 0x0938, 0x094D, 0x092C, 0x0940 }, (const uint16_t[])
        {
            0xE052, 0x092C, 0x0940, 0
        }
    }, // स्बी
    {
        4, (const uint16_t[]) { 0x0915, 0x094D, 0x0937, 0x093F }, (const uint16_t[])
        {
            0xE1DD, 0xE02E, 0
        }
    }, // क्षि
    {
        4, (const uint16_t[]) { 0x0924, 0x094D, 0x0915, 0x0943 }, (const uint16_t[])
        {
            0xE041, 0x0915, 0x0943, 0
        }
    }, // त्कृ
    {
        4, (const uint16_t[]) { 0x0924, 0x094D, 0x092E, 0x093F }, (const uint16_t[])
        {
            0xE1D7, 0xE041, 0x092E, 0
        }
    }, // त्मि
    {
        4, (const uint16_t[]) { 0x0937, 0x094D, 0x0923, 0x094B }, (const uint16_t[])
        {
            0xE051, 0x0923, 0x093E, 0x0947, 0
        }
    }, // ष्णो
    {
        4, (const uint16_t[]) { 0x0938, 0x094D, 0x091C, 0x093F }, (const uint16_t[])
        {
            0xE1D7, 0xE052, 0x091C, 0
        }
    }, // स्जि
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x0936, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0xE050, 0x093E, 0xE030, 0
        }
    }, // र्शि
    {
        4, (const uint16_t[]) { 0x0924, 0x094D, 0x0924, 0x093F }, (const uint16_t[])
        {
            0xE1DD, 0xE184, 0
        }
    }, // त्ति
    {
        4, (const uint16_t[]) { 0x0932, 0x094D, 0x092A, 0x093F }, (const uint16_t[])
        {
            0xE1D7, 0xE04D, 0x092A, 0
        }
    }, // ल्पि
    {
        4, (const uint16_t[]) { 0x092C, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0xE08F, 0
        }
    }, // ब्रि
    {
        4, (const uint16_t[]) { 0x0926, 0x094D, 0x0927, 0x093F }, (const uint16_t[])
        {
            0xE1DD, 0xE188, 0
        }
    }, // द्धि
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x0925, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x0925, 0xE030, 0
        }
    }, // र्थि
    {
        4, (const uint16_t[]) { 0x092E, 0x094D, 0x0930, 0x093E }, (const uint16_t[])
        {
            0xE091, 0x093E, 0
        }
    }, // म्रा
    {
        4, (const uint16_t[]) { 0x0938, 0x094D, 0x0938, 0x093E }, (const uint16_t[])
        {
            0xE052, 0x0938, 0x093E, 0
        }
    }, // स्सा
    {
        4, (const uint16_t[]) { 0x0919, 0x094D, 0x0915, 0x093F }, (const uint16_t[])
        {
            0xE036, 0xE1DB, 0x0915, 0
        }
    }, // ङ्कि
    {
        4, (const uint16_t[]) { 0x0919, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0xE07D, 0
        }
    }, // ङ्रि
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x092F, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x092F, 0xE030, 0
        }
    }, // र्यि
    {
        4, (const uint16_t[]) { 0x092B, 0x093C, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE0B5, 0
        }
    }, // फ़्र
    {
        4, (const uint16_t[]) { 0x0938, 0x094D, 0x0932, 0x093E }, (const uint16_t[])
        {
            0xE052, 0x0932, 0x093E, 0
        }
    }, // स्ला
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x092D, 0x093F }, (const uint16_t[])
        {
            0xE1F5, 0xE049, 0x093E, 0
        }
    }, // र्भि
    {
        4, (const uint16_t[]) { 0x092A, 0x094D, 0x0924, 0x093F }, (const uint16_t[])
        {
            0xE1E2, 0xE046, 0x0924, 0
        }
    }, // प्ति
    {
        4, (const uint16_t[]) { 0x0932, 0x094D, 0x0915, 0x0940 }, (const uint16_t[])
        {
            0xE04D, 0x0915, 0xE20A, 0
        }
    }, // ल्की
    {
        4, (const uint16_t[]) { 0x0908, 0x00A0, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1E2, 0x0908, 0x20, 0x0930, 0
        }
    }, // ई रि
    {
        4, (const uint16_t[]) { 0x095E, 0x094D, 0x0930, 0x0940 }, (const uint16_t[])
        {
            0x095E, 0xE209, 0
        }
    }, // फ़्री
    {
        4, (const uint16_t[]) { 0x095E, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x095E, 0
        }
    }, // फ़्रि
    {
        4, (const uint16_t[]) { 0x0928, 0x094D, 0x0926, 0x0940 }, (const uint16_t[])
        {
            0xE045, 0x0926, 0xE208, 0
        }
    }, // न्दी
    {
        4, (const uint16_t[]) { 0x0932, 0x094D, 0x0915, 0x093E }, (const uint16_t[])
        {
            0xE04D, 0x0915, 0x093E, 0
        }
    }, // ल्का
    {
        4, (const uint16_t[]) { 0x0932, 0x094D, 0x0915, 0x093F }, (const uint16_t[])
        {
            0xE1EE, 0xE04D, 0x0915, 0
        }
    }, // ल्कि
    {
        4, (const uint16_t[]) { 0x0930, 0x095E, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0x0930, 0xE06D, 0x0932, 0
        }
    }, // रफ़्ल
    {
        4, (const uint16_t[]) { 0x0938, 0x094D, 0x0932, 0x0947 }, (const uint16_t[])
        {
            0xE052, 0x0932, 0x0947, 0
        }
    }, // स्ले
    {
        4, (const uint16_t[]) { 0x092C, 0x0932, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0x092C, 0xE1E8, 0x0932, 0
        }
    }, // बलिं
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x0927, 0x093E }, (const uint16_t[])
        {
            0x0927, 0x093E, 0xE030, 0
        }
    }, // र्धा
    {
        4, (const uint16_t[]) { 0x0915, 0x092E, 0x093F, 0x0902 }, (const uint16_t[])
        {
            0x0915, 0xE1E6, 0x092E, 0
        }
    }, // कमिं
    {
        4, (const uint16_t[]) { 0x0949, 0x0930, 0x094D, 0x0921 }, (const uint16_t[])
        {
            0x0949, 0x0921, 0xE030, 0
        }
    }, // ॉर्ड
    {
        4, (const uint16_t[]) { 0x092A, 0x094D, 0x091F, 0x093F }, (const uint16_t[])
        {
            0xE1DF, 0xE046, 0x091F, 0
        }
    }, // प्टि
    {
        4, (const uint16_t[]) { 0x091C, 0x094D, 0x091E, 0x093F }, (const uint16_t[])
        {
            0xE1DC, 0xE02F, 0
        }
    }, // ज्ञि
    {
        4, (const uint16_t[]) { 0x0936, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1DE, 0xE097, 0
        }
    }, // श्रि
    {
        4, (const uint16_t[]) { 0x0924, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0xE088, 0
        }
    }, // त्रि
    {
        4, (const uint16_t[]) { 0x0917, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0xE07B, 0
        }
    }, // ग्रि
    {
        4, (const uint16_t[]) { 0x092A, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0xE08D, 0
        }
    }, // प्रि
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x0917, 0x093F }, (const uint16_t[])
        {
            0xE1F2, 0x0917, 0
        }
    }, // र्गि
    {
        4, (const uint16_t[]) { 0x0926, 0x0930, 0x094D, 0x092D }, (const uint16_t[])
        {
            0x0926, 0x092D, 0xE030, 0
        }
    }, // दर्भ
    {
        4, (const uint16_t[]) { 0x0935, 0x0930, 0x094D, 0x0921 }, (const uint16_t[])
        {
            0x0935, 0x0921, 0xE030, 0
        }
    }, // वर्ड
    {
        4, (const uint16_t[]) { 0x0926, 0x0930, 0x094D, 0x091C }, (const uint16_t[])
        {
            0x0926, 0xE039, 0x093E, 0xE030, 0
        }
    }, // दर्ज
    {
        4, (const uint16_t[]) { 0x092B, 0x093C, 0x094D, 0x091F }, (const uint16_t[])
        {
            0xE06D, 0x091F, 0
        }
    }, // फ़्ट
    {
        4, (const uint16_t[]) { 0x092B, 0x093C, 0x094D, 0x0924 }, (const uint16_t[])
        {
            0xE06D, 0xE041, 0x093E, 0
        }
    }, // फ़्त
    {
        4, (const uint16_t[]) { 0x090F, 0x00A0, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1D7, 0x090F, 0x00A0, 0x0930, 0
        }
    }, // ए रि
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x0921, 0x093F }, (const uint16_t[])
        {
            0xE1F2, 0x0921, 0
        }
    }, // र्डि
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x0937, 0x093E }, (const uint16_t[])
        {
            0x0937, 0x093E, 0xE030, 0
        }
    }, // र्षा
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x0921, 0x094D }, (const uint16_t[])
        {
            0xE03E, 0xE030, 0
        }
    }, // र्ड्
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x095E, 0x0940 }, (const uint16_t[])
        {
            0x095E, 0xE210, 0
        }
    }, // र्फ़ी
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x092B, 0x093E }, (const uint16_t[])
        {
            0x092B, 0x093E, 0xE030, 0
        }
    }, // र्फा
    {
        4, (const uint16_t[]) { 0x0936, 0x094D, 0x091A, 0x093F }, (const uint16_t[])
        {
            0xE1DC, 0xE1A9, 0
        }
    }, // श्चि
    {
        4, (const uint16_t[]) { 0x0921, 0x094D, 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1DD, 0xE085, 0
        }
    }, // ड्रि
    {
        4, (const uint16_t[]) { 0x0921, 0x094D, 0x0930, 0x093E }, (const uint16_t[])
        {
            0xE085, 0x093E, 0
        }
    }, // ड्रा
    {
        4, (const uint16_t[]) { 0x0932, 0x094D, 0x092B, 0x093C }, (const uint16_t[])
        {
            0xE04D, 0x095E, 0
        }
    }, // ल्फ़
    {
        4, (const uint16_t[]) { 0x0915, 0x094D, 0x0935, 0x0947 }, (const uint16_t[])
        {
            0xE032, 0xE04F, 0x094B, 0
        }
    }, // क्वे
    {
        4, (const uint16_t[]) { 0x0928, 0x094D, 0x0921, 0x094B }, (const uint16_t[])
        {
            0xE045, 0x0921, 0x094B, 0
        }
    }, // न्डो
    {
        4, (const uint16_t[]) { 0x0939, 0x00A0, 0x0921, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x0939, 0x0020, 0x0921, 0
        }
    }, // ह डि
    {
        4, (const uint16_t[]) { 0x0932, 0x094D, 0x0915, 0x094B }, (const uint16_t[])
        {
            0xE04D, 0x0915, 0x094B, 0
        }
    }, // ल्को
    {
        4, (const uint16_t[]) { 0x0930, 0x094D, 0x0935, 0x093E }, (const uint16_t[])
        {
            0x0935, 0x093E, 0xE030, 0
        }
    }, // र्वा
    {
        4, (const uint16_t[]) { 0x0938, 0x094D, 0x0935, 0x093F }, (const uint16_t[])
        {
            0x093F, 0xE052, 0x0935, 0
        }
    }, // स्वि
    {
        3, (const uint16_t[]) { 0x091C, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE039, 0x092F, 0
        }
    }, // ज्य
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x0921 }, (const uint16_t[])
        {
            0x0921, 0xE030, 0
        }
    }, // र्ड
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x091F }, (const uint16_t[])
        {
            0xE052, 0x091F, 0
        }
    }, // स्ट
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x0915 }, (const uint16_t[])
        {
            0x0915, 0xE030, 0
        }
    }, // र्क
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0x0935, 0xE030, 0
        }
    }, // र्व
    {
        3, (const uint16_t[]) { 0x0924, 0x094D, 0x092A }, (const uint16_t[])
        {
            0xE041, 0x092A, 0
        }
    }, // त्प
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x0936 }, (const uint16_t[])
        {
            0xE050, 0x093E, 0xE030, 0
        }
    }, // र्श
    {
        3, (const uint16_t[]) { 0x0921, 0x093C, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x095C, 0
        }
    }, // ड़ि
    {
        3, (const uint16_t[]) { 0x0915, 0x094D, 0x0938 }, (const uint16_t[])
        {
            0xE032, 0x0938, 0
        }
    },  // क्स
    {
        3, (const uint16_t[]) { 0x0924, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE041, 0x0935, 0
        }
    },  // त्व
    {
        3, (const uint16_t[]) { 0x0924, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE041, 0x092F, 0
        }
    },  // त्य
    {
        3, (const uint16_t[]) { 0x092C, 0x094D, 0x092C }, (const uint16_t[])
        {
            0xE048, 0x092C, 0
        }
    },  // ब्ब
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x0923 }, (const uint16_t[])
        {
            0x0923, 0xE030, 0
        }
    },  // र्ण
    {
        3, (const uint16_t[]) { 0x0932, 0x094D, 0x092A }, (const uint16_t[])
        {
            0xE04D, 0x092A, 0
        }
    },  // ल्प
    {
        3, (const uint16_t[]) { 0x0926, 0x094D, 0x092D }, (const uint16_t[])
        {
            0xE18B, 0
        }
    },  // द्भ

    {
        3, (const uint16_t[]) { 0x0924, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE088, 0
        }
    }, // त्र
    {
        3, (const uint16_t[]) { 0x091C, 0x094D, 0x091E }, (const uint16_t[])
        {
            0xE02F, 0
        }
    }, // ज्ञ
    {
        3, (const uint16_t[]) { 0x0936, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE097, 0
        }
    }, // श्र
    {
        3, (const uint16_t[]) { 0x0935, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE1A8, 0
        }
    }, // व्य
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE099, 0
        }
    }, // स्र
    {
        3, (const uint16_t[]) { 0x0915, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE079, 0
        }
    }, // क्र
    {
        3, (const uint16_t[]) { 0x092A, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE08D, 0
        }
    }, // प्र
    {
        3, (const uint16_t[]) { 0x0917, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE07B, 0
        }
    }, // ग्र
    {
        3, (const uint16_t[]) { 0x091A, 0x094D, 0x091B }, (const uint16_t[])
        {
            0xE170, 0
        }
    }, // च्छ
    {
        3, (const uint16_t[]) { 0x092C, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE08F, 0
        }
    }, // ब्र
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x0925 }, (const uint16_t[])
        {
            0xE1B3, 0
        }
    }, // स्थ
    {
        3, (const uint16_t[]) { 0x092B, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE08E, 0
        }
    }, // फ्र
    {
        3, (const uint16_t[]) { 0x091C, 0x094D, 0x091C }, (const uint16_t[])
        {
            0xE173, 0
        }
    }, // ज्ज
    {
        3, (const uint16_t[]) { 0x0921, 0x094D, 0x0921 }, (const uint16_t[])
        {
            0xE17E, 0
        }
    }, // ड्ड
    {
        3, (const uint16_t[]) { 0x0927, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE08B, 0
        }
    }, // ध्र
    {
        3, (const uint16_t[]) { 0x0925, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE089, 0
        }
    }, // थ्र
    {
        3, (const uint16_t[]) { 0x0936, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE1AA, 0
        }
    }, // श्व
    {
        3, (const uint16_t[]) { 0x0922, 0x094D, 0x0922 }, (const uint16_t[])
        {
            0xE182, 0
        }
    }, // ढ्ढ
    {
        3, (const uint16_t[]) { 0x091F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE083, 0
        }
    }, // ट्र
    {
        3, (const uint16_t[]) { 0x091D, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE081, 0
        }
    }, // झ्र
    {
        3, (const uint16_t[]) { 0x0918, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE07C, 0
        }
    }, // घ्र
    {
        3, (const uint16_t[]) { 0x0924, 0x094D, 0x0924 }, (const uint16_t[])
        {
            0xE184, 0
        }
    }, // त्त
    {
        3, (const uint16_t[]) { 0x0926, 0x094D, 0x0927 }, (const uint16_t[])
        {
            0xE188, 0
        }
    }, // द्ध
    {
        3, (const uint16_t[]) { 0x0915, 0x094D, 0x0924 }, (const uint16_t[])
        {
            0xE169, 0
        }
    }, // क्त
    {
        3, (const uint16_t[]) { 0x0928, 0x094D, 0x0928 }, (const uint16_t[])
        {
            0xE19C, 0
        }
    }, // न्न
    {
        3, (const uint16_t[]) { 0x0935, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE096, 0
        }
    }, // व्र
    {
        3, (const uint16_t[]) { 0x0936, 0x094D, 0x091A }, (const uint16_t[])
        {
            0xE1A9, 0
        }
    }, // श्च
    {
        3, (const uint16_t[]) { 0x092E, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04A, 0x092F, 0
        }
    }, // म्य
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x092F }, (const uint16_t[])
        {
            0x092F, 0xE030, 0
        }
    }, // र्य
    {
        3, (const uint16_t[]) { 0x0932, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04D, 0x092F, 0
        }
    }, // ल्य
    {
        3, (const uint16_t[]) { 0x0915, 0x094D, 0x0915 }, (const uint16_t[])
        {
            0xE032, 0x0915, 0
        }
    }, // क्क
    {
        3, (const uint16_t[]) { 0x0917, 0x094D, 0x0917 }, (const uint16_t[])
        {
            0xE034, 0x0917, 0
        }
    }, // ग्ग
    {
        3, (const uint16_t[]) { 0x0928, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE045, 0x092F, 0
        }
    }, // न्य
    {
        3, (const uint16_t[]) { 0x0928, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE045, 0x0935, 0
        }
    }, // न्व
    {
        3, (const uint16_t[]) { 0x0935, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE04F, 0x0935, 0
        }
    }, // व्व
    {
        3, (const uint16_t[]) { 0x091B, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE038, 0x0935, 0
        }
    }, // छ्व
    {
        3, (const uint16_t[]) { 0x0916, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE033, 0x092F, 0
        }
    }, // ख्य
    {
        3, (const uint16_t[]) { 0x091C, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE039, 0x0935, 0
        }
    }, // ज्व
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x0915 }, (const uint16_t[])
        {
            0xE052, 0x0915, 0
        }
    }, // स्क
    {
        3, (const uint16_t[]) { 0x092A, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0xE046, 0x0932, 0
        }
    }, // प्ल
    {
        3, (const uint16_t[]) { 0x092F, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE04B, 0x092F, 0
        }
    }, // य्य
    {
        3, (const uint16_t[]) { 0x0917, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0xE034, 0x0932, 0
        }
    }, // ग्ल
    {
        3, (const uint16_t[]) { 0x0917, 0x094D, 0x0938 }, (const uint16_t[])
        {
            0xE034, 0x0938, 0
        }
    }, // ग्स
    {
        3, (const uint16_t[]) { 0x0924, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE041, 0x092E, 0
        }
    }, // त्म
    {
        3, (const uint16_t[]) { 0x092B, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE047, 0x092F, 0
        }
    }, // फ्य
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE052, 0x092E, 0
        }
    }, // स्म
    {
        3, (const uint16_t[]) { 0x092E, 0x094D, 0x092C }, (const uint16_t[])
        {
            0xE04A, 0x092C, 0
        }
    }, // म्ब
    {
        3, (const uint16_t[]) { 0x0915, 0x094D, 0x092f }, (const uint16_t[])
        {
            0xE032, 0x092F, 0
        }
    }, // क्य
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x0924 }, (const uint16_t[])
        {
            0xE052, 0x0924, 0
        }
    }, // स्त
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE052, 0x0935, 0
        }
    }, // स्व
    {
        3, (const uint16_t[]) { 0x0924, 0x094D, 0x0925 }, (const uint16_t[])
        {
            0xE041, 0xE042, 0x093E
        }
    }, // त्थ

    {
        3, (const uint16_t[]) { 0x091A, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE07E, 0
        }
    }, // च्र
    {
        3, (const uint16_t[]) { 0x091C, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE080, 0
        }
    }, // ज्र
    {
        3, (const uint16_t[]) { 0x0939, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE1BC, 0
        }
    }, // ह्य
    {
        3, (const uint16_t[]) { 0x0915, 0x094D, 0x0937 }, (const uint16_t[])
        {
            0xE02E, 0
        }
    }, // क्ष
    {
        3, (const uint16_t[]) { 0x0936, 0x094D, 0x0928 }, (const uint16_t[])
        {
            0xE1AC, 0
        }
    }, // श्न
    {
        3, (const uint16_t[]) { 0x0919, 0x094D, 0x0917 }, (const uint16_t[])
        {
            0xE16C, 0
        }
    }, // ङ्ग
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE1B5, 0
        }
    }, // स्य
    {
        3, (const uint16_t[]) { 0x0936, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE050, 0x092F, 0
        }
    }, // श्य
    {
        3, (const uint16_t[]) { 0x092D, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE049, 0x092F, 0
        }
    }, // भ्य
    {
        3, (const uint16_t[]) { 0x0927, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE044, 0x092F, 0
        }
    }, // ध्य
    {
        3, (const uint16_t[]) { 0x092A, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE046, 0x092F, 0
        }
    }, // प्य
    {
        3, (const uint16_t[]) { 0x0917, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE034, 0x092F, 0
        }
    }, // ग्य
    {
        3, (const uint16_t[]) { 0x092A, 0x094D, 0x0924 }, (const uint16_t[])
        {
            0xE046, 0x0924, 0
        }
    }, // प्त
    {
        3, (const uint16_t[]) { 0x092A, 0x094D, 0x092A }, (const uint16_t[])
        {
            0xE046, 0x092A, 0
        }
    }, // प्प

    {
        3, (const uint16_t[]) { 0x0915, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0xE032, 0x0932, 0
        }
    }, // क्ल
    {
        3, (const uint16_t[]) { 0x0917, 0x094D, 0x0927 }, (const uint16_t[])
        {
            0xE034, 0x0927, 0
        }
    }, // ग्ध
    {
        3, (const uint16_t[]) { 0x0917, 0x094D, 0x0928 }, (const uint16_t[])
        {
            0xE07B, 0
        }
    }, // ग्न
    {
        3, (const uint16_t[]) { 0x0919, 0x094D, 0x0915 }, (const uint16_t[])
        {
            0xE036, 0x0915, 0
        }
    }, // ङ्क
    {
        3, (const uint16_t[]) { 0x091A, 0x094D, 0x091A }, (const uint16_t[])
        {
            0xE16F, 0
        }
    }, // च्च
    {
        3, (const uint16_t[]) { 0x091B, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE07F, 0
        }
    }, // छ्र
    {
        3, (const uint16_t[]) { 0x091C, 0x094D, 0x091D }, (const uint16_t[])
        {
            0xE039, 0x091D, 0
        }
    }, // ज्झ
    {
        3, (const uint16_t[]) { 0x091E, 0x094D, 0x091A }, (const uint16_t[])
        {
            0xE03B, 0xE037, 0x093E
        }
    }, // ञ्च
    {
        3, (const uint16_t[]) { 0x091F, 0x094D, 0x091F }, (const uint16_t[])
        {
            0xE176, 0
        }
    }, // ट्ट
    {
        3, (const uint16_t[]) { 0x091F, 0x094D, 0x0920 }, (const uint16_t[])
        {
            0xE178, 0
        }
    }, // ट्ठ
    {
        3, (const uint16_t[]) { 0x0923, 0x094D, 0x0921 }, (const uint16_t[])
        {
            0xE040, 0x0921, 0
        }
    }, // ण्ड
    {
        3, (const uint16_t[]) { 0x0923, 0x094D, 0x0923 }, (const uint16_t[])
        {
            0xE040, 0x0923, 0
        }
    }, // ण्ण
    {
        3, (const uint16_t[]) { 0x0925, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE049, 0x0935, 0
        }
    }, // थ्व
    {
        3, (const uint16_t[]) { 0x0926, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE193, 0
        }
    }, // द्य
    {
        3, (const uint16_t[]) { 0x0926, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE08A, 0
        }
    }, // द्र
    {
        3, (const uint16_t[]) { 0x0926, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE18C, 0
        }
    }, // द्व
    {
        3, (const uint16_t[]) { 0x0927, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE044, 0x0935, 0
        }
    }, // ध्व
    {
        3, (const uint16_t[]) { 0x0928, 0x094D, 0x0924 }, (const uint16_t[])
        {
            0xE045, 0xE184, 0
        }
    }, // न्त
    {
        3, (const uint16_t[]) { 0x0928, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE045, 0x092E, 0
        }
    }, // न्म
    {
        3, (const uint16_t[]) { 0x092B, 0x094D, 0x092B }, (const uint16_t[])
        {
            0xE047, 0x092B, 0
        }
    }, // फ्फ
    {
        3, (const uint16_t[]) { 0x092C, 0x094D, 0x0926 }, (const uint16_t[])
        {
            0xE048, 0x0926, 0
        }
    }, // ब्द
    {
        3, (const uint16_t[]) { 0x092D, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE090, 0
        }
    }, // भ्र
    {
        3, (const uint16_t[]) { 0x092E, 0x094D, 0x092A }, (const uint16_t[])
        {
            0xE04A, 0x092A, 0
        }
    }, // म्प
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x0925 }, (const uint16_t[])
        {
            0x0925, 0xE030, 0
        }
    }, // र्थ
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x0926 }, (const uint16_t[])
        {
            0x0926, 0xE030, 0
        }
    }, // र्द
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x0927 }, (const uint16_t[])
        {
            0x0927, 0xE030, 0
        }
    }, // र्ध
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x092E }, (const uint16_t[])
        {
            0x092E, 0xE030, 0
        }
    }, // र्म
    {
        3, (const uint16_t[]) { 0x0932, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0xE04D, 0x0932, 0
        }
    }, // ल्ल
    {
        3, (const uint16_t[]) { 0x0933, 0x094D, 0x0917 }, (const uint16_t[])
        {
            0xE04E, 0x0917, 0
        }
    }, // ळ्ग
    {
        3, (const uint16_t[]) { 0x0936, 0x094D, 0x091B }, (const uint16_t[])
        {
            0xE050, 0xE229, 0
        }
    }, // श्छ
    {
        3, (const uint16_t[]) { 0x0936, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0xE050, 0x0932, 0
        }
    }, // श्ल
    {
        3, (const uint16_t[]) { 0x0936, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0xE238, 0x0932, 0
        }
    }, // श्ल
    {
        3, (const uint16_t[]) { 0x0937, 0x094D, 0x091F }, (const uint16_t[])
        {
            0xE1AD, 0
        }
    }, // ष्ट
    {
        3, (const uint16_t[]) { 0x0937, 0x094D, 0x0920 }, (const uint16_t[])
        {
            0xE1B0, 0
        }
    }, // ष्ठ
    {
        3, (const uint16_t[]) { 0x0937, 0x094D, 0x0935 }, (const uint16_t[])
        {
            0xE051, 0x0935, 0
        }
    }, // ष्व
    {
        3, (const uint16_t[]) { 0x0939, 0x094D, 0x0928 }, (const uint16_t[])
        {
            0xE1BA, 0
        }
    }, // ह्न
    {
        3, (const uint16_t[]) { 0x0939, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE09A, 0
        }
    },  // ह्र
    {
        3, (const uint16_t[]) { 0x0920, 0x094D, 0x0920 }, (const uint16_t[])
        {
            0xE17B, 0
        }
    },  // ठ्ठ
    {
        3, (const uint16_t[]) { 0x0921, 0x094D, 0x0922 }, (const uint16_t[])
        {
            0xE17D, 0
        }
    },  // ड्ढ
    {
        3, (const uint16_t[]) { 0x0922, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE086, 0
        }
    },  // ढ्र
    {
        3, (const uint16_t[]) { 0x0926, 0x094D, 0x0918 }, (const uint16_t[])
        {
            0xE188, 0
        }
    },  // द्घ
    {
        3, (const uint16_t[]) { 0x092C, 0x094D, 0x0932 }, (const uint16_t[])
        {
            0xE048, 0x0932, 0
        }
    },  // ब्ल
    {
        3, (const uint16_t[]) { 0x092F, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE092, 0
        }
    },  // य्र
    {
        3, (const uint16_t[]) { 0x0937, 0x094D, 0x0915 }, (const uint16_t[])
        {
            0xE051, 0x0915, 0
        }
    },  // ष्क
    {
        3, (const uint16_t[]) { 0x0937, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE051, 0x092E, 0
        }
    },  // ष्म
    {
        3, (const uint16_t[]) { 0x0939, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE1BB, 0
        }
    },  // ह्म
    {
        3, (const uint16_t[]) { 0x0916, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE033, 0x092F, 0
        }
    }, // ख्य
    {
        3, (const uint16_t[]) { 0x0916, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE07A, 0x0, 0
        }
    }, // ख्र
    {
        3, (const uint16_t[]) { 0x0917, 0x0942, 0x0901 }, (const uint16_t[])
        {
            0x0917, 0xE257, 0x0942, 0
        }
    }, // गूँ
    {
        3, (const uint16_t[]) { 0x091A, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE037, 0x092F, 0
        }
    }, // च्य
    {
        3, (const uint16_t[]) { 0x091B, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE172, 0x0, 0
        }
    }, // छ्य
    {
        3, (const uint16_t[]) { 0x091D, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE03A, 0x092F, 0
        }
    }, // झ्य
    {
        3, (const uint16_t[]) { 0x091E, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE082, 0x0, 0
        }
    }, // ञ्र
    {
        3, (const uint16_t[]) { 0x0921, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE180, 0x0, 0
        }
    }, // ड्य
    {
        3, (const uint16_t[]) { 0x0922, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE183, 0x0, 0
        }
    }, // ढ्य
    {
        3, (const uint16_t[]) { 0x0923, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE040, 0x092F, 0
        }
    }, // ण्य
    {
        3, (const uint16_t[]) { 0x0923, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE087, 0x0, 0
        }
    }, // ण्र
    {
        3, (const uint16_t[]) { 0x0924, 0x094D, 0x0938 }, (const uint16_t[])
        {
            0xE041, 0x0938, 0
        }
    }, // त्स
    {
        3, (const uint16_t[]) { 0x0925, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE042, 0x092F, 0
        }
    }, // थ्य
    {
        3, (const uint16_t[]) { 0x0926, 0x094D, 0x0926 }, (const uint16_t[])
        {
            0xE190, 0x0, 0
        }
    }, // द्द
    {
        3, (const uint16_t[]) { 0x092C, 0x094D, 0x0927 }, (const uint16_t[])
        {
            0xE048, 0x0927, 0
        }
    }, // ब्ध
    {
        3, (const uint16_t[]) { 0x092C, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE048, 0x092F, 0
        }
    }, // ब्य
    {
        3, (const uint16_t[]) { 0x092E, 0x094D, 0x092D }, (const uint16_t[])
        {
            0xE1A4, 0
        }
    }, // म्भ
    {
        3, (const uint16_t[]) { 0x0933, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE095, 0
        }
    }, // ळ्र
    {
        3, (const uint16_t[]) { 0x0937, 0x094D, 0x0930 }, (const uint16_t[])
        {
            0xE098, 0
        }
    }, // ष्र
    {
        3, (const uint16_t[]) { 0x0915, 0x094D, 0x091F }, (const uint16_t[])
        {
            0xE032, 0x091F, 0
        }
    }, // क्ट
    {
        3, (const uint16_t[]) { 0x092E, 0x0947, 0x0902 }, (const uint16_t[])
        {
            0xE04A, 0xE148, 0
        }
    }, // में
    {
        3, (const uint16_t[]) { 0x0932, 0x094D, 0x092B }, (const uint16_t[])
        {
            0xE04D, 0xE08E, 0
        }
    }, // ल्फ
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x092A }, (const uint16_t[])
        {
            0xE052, 0x092A, 0
        }
    }, // स्प
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x091F }, (const uint16_t[])
        {
            0x091F, 0xE030, 0
        }
    }, // र्ट
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x0928 }, (const uint16_t[])
        {
            0xE052, 0x0928, 0
        }
    }, // स्न
    {
        3, (const uint16_t[]) { 0x0935, 0x094D, 0x0936 }, (const uint16_t[])
        {
            0xE04F, 0xE050, 0x093E, 0
        }
    }, // व्श
    {
        3, (const uint16_t[]) { 0x092B, 0x094D, 0x091F }, (const uint16_t[])
        {
            0xE047, 0x091F, 0
        }
    }, // फ्ट
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x095E }, (const uint16_t[])
        {
            0x095E, 0xE030, 0
        }
    }, // र्फ़
    {
        3, (const uint16_t[]) { 0x095B, 0x094D, 0x092F }, (const uint16_t[])
        {
            0xE05F, 0x092F, 0
        }
    }, // ज़्य
    {
        3, (const uint16_t[]) { 0x0915, 0x094D, 0x0936 }, (const uint16_t[])
        {
            0xE032, 0x0936, 0
        }
    }, // क्श
    {
        3, (const uint16_t[]) { 0x0935, 0x094D, 0x0939 }, (const uint16_t[])
        {
            0xE04F, 0x0939, 0
        }
    }, // व्ह
    {
        3, (const uint16_t[]) { 0x092A, 0x094D, 0x0938 }, (const uint16_t[])
        {
            0xE046, 0x0938, 0
        }
    }, // प्स
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x0938 }, (const uint16_t[])
        {
            0xE052, 0x0938, 0
        }
    }, // स्स
    {
        3, (const uint16_t[]) { 0x092B, 0x093F, 0x091F }, (const uint16_t[])
        {
            0xE1DA, 0x092B, 0x091F, 0
        }
    }, // फिट
    {
        3, (const uint16_t[]) { 0x092B, 0x093F, 0x0930 }, (const uint16_t[])
        {
            0xE1DA, 0x092B, 0x0930, 0
        }
    }, // फिर
    {
        3, (const uint16_t[]) { 0x0938, 0x094D, 0x0921 }, (const uint16_t[])
        {
            0xE052, 0x0921, 0
        }
    }, // स्ड
    {
        3, (const uint16_t[]) { 0x092E, 0x094D, 0x092E }, (const uint16_t[])
        {
            0xE04A, 0x092E, 0
        }
    }, // म्म
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x0918 }, (const uint16_t[])
        {
            0x0918, 0xE030, 0
        }
    }, // र्घ
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x091A }, (const uint16_t[])
        {
            0x091A, 0xE030, 0
        }
    }, // र्च
    {
        3, (const uint16_t[]) { 0x0930, 0x094D, 0x091C }, (const uint16_t[])
        {
            0xE039, 0x093E, 0xE030, 0
        }
    }, // र्ज
    {
        3, (const uint16_t[]) { 0x092E, 0x094D, 0x0928 }, (const uint16_t[])
        {
            0xE04A, 0xE045, 0x093E, 0
        }
    }, // म्न
    {
        3, (const uint16_t[]) { 0x092B, 0x093C, 0x094B }, (const uint16_t[])
        {
            0x095E, 0x094B, 0
        }
    }, // फ़ो
    {
        3, (const uint16_t[]) { 0x095C, 0x0947, 0x0902 }, (const uint16_t[])
        {
            0x095C, 0xE13C, 0
        }
    }, // ड़ें
    {
        3, (const uint16_t[]) { 0x091A, 0x0947, 0x0902 }, (const uint16_t[])
        {
            0x091A, 0xE13C, 0
        }
    }, // चें
    {
        3, (const uint16_t[]) { 0x0928, 0x0947, 0x0902 }, (const uint16_t[])
        {
            0xE045, 0xE148, 0
        }
    }, // नें

    {
        2, (const uint16_t[]) { 0x0939, 0x0941 }, (const uint16_t[])
        {
            0xE10E, 0
        }
    },  // हु
    {
        2, (const uint16_t[]) { 0x0930, 0x0941 }, (const uint16_t[])
        {
            0xE11C, 0
        }
    },  // रु
    {
        2, (const uint16_t[]) { 0x0930, 0x0942 }, (const uint16_t[])
        {
            0xE11D, 0
        }
    },  // रू
    {
        2, (const uint16_t[]) { 0x0915, 0x0940 }, (const uint16_t[])
        {
            0x0915, 0xE20A, 0
        }
    }, // की
    {
        2, (const uint16_t[]) { 0x0939, 0x0943 }, (const uint16_t[])
        {
            0xE112, 0
        }
    }, // हृ
    {
        2, (const uint16_t[]) { 0x0915, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x0915, 0
        }
    }, // कि
    {
        2, (const uint16_t[]) { 0x0917, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x0917, 0
        }
    }, // गि
    {
        2, (const uint16_t[]) { 0x091A, 0x093F }, (const uint16_t[])
        {
            0xE1DC, 0x091A, 0
        }
    }, // चि
    {
        2, (const uint16_t[]) { 0x091C, 0x093F }, (const uint16_t[])
        {
            0xE1DE, 0x091C, 0
        }
    }, // जि
    {
        2, (const uint16_t[]) { 0x091F, 0x093F }, (const uint16_t[])
        {
            0xE1D9, 0x091F, 0
        }
    }, // टि
    {
        2, (const uint16_t[]) { 0x0921, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x0921, 0
        }
    }, // डि
    {
        2, (const uint16_t[]) { 0x0924, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x0924, 0
        }
    }, // ति
    {
        2, (const uint16_t[]) { 0x0925, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x0925, 0
        }
    }, // थि
    {
        2, (const uint16_t[]) { 0x0926, 0x093F }, (const uint16_t[])
        {
            0xE1D9, 0x0926, 0
        }
    }, // दि
    {
        2, (const uint16_t[]) { 0x092A, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x092A, 0
        }
    }, // पि
    {
        2, (const uint16_t[]) { 0x0923, 0x093F }, (const uint16_t[])
        {
            0xE1DE, 0x0923, 0
        }
    }, // णि
    {
        2, (const uint16_t[]) { 0x092C, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x092C, 0
        }
    }, // बि
    {
        2, (const uint16_t[]) { 0x092F, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x092F, 0
        }
    }, // यि
    {
        2, (const uint16_t[]) { 0x0930, 0x093F }, (const uint16_t[])
        {
            0xE1D8, 0x0930, 0
        }
    }, // रि
    {
        2, (const uint16_t[]) { 0x0932, 0x093F }, (const uint16_t[])
        {
            0xE1DC, 0x0932, 0
        }
    }, // लि
    {
        2, (const uint16_t[]) { 0x0935, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x0935, 0
        }
    }, // वि
    {
        2, (const uint16_t[]) { 0x0936, 0x093F }, (const uint16_t[])
        {
            0xE1DC, 0x0936, 0
        }
    }, // शि
    {
        2, (const uint16_t[]) { 0x0938, 0x093F }, (const uint16_t[])
        {
            0xE1DC, 0x0938, 0
        }
    }, // सि
    {
        2, (const uint16_t[]) { 0x0939, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x0939, 0
        }
    }, // हि
    {
        2, (const uint16_t[]) { 0x0916, 0x093F }, (const uint16_t[])
        {
            0xE1DF, 0x0916, 0
        }
    }, // खि
    {
        2, (const uint16_t[]) { 0x091B, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x091B, 0
        }
    }, // छि
    {
        2, (const uint16_t[]) { 0x0920, 0x093F }, (const uint16_t[])
        {
            0xE1D9, 0x0920, 0
        }
    }, // ठि
    {
        2, (const uint16_t[]) { 0x0922, 0x093F }, (const uint16_t[])
        {
            0xE1D9, 0x0922, 0
        }
    }, // ढि
    {
        2, (const uint16_t[]) { 0x0927, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x0927, 0
        }
    }, // धि
    {
        2, (const uint16_t[]) { 0x092D, 0x093F }, (const uint16_t[])
        {
            0xE1DC, 0x092D, 0
        }
    }, // भि
    {
        2, (const uint16_t[]) { 0x0928, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x0928, 0
        }
    }, // नि
    {
        2, (const uint16_t[]) { 0x092E, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x092E, 0
        }
    }, // मि
    {
        2, (const uint16_t[]) { 0x0937, 0x093F }, (const uint16_t[])
        {
            0xE1DA, 0x0937, 0
        }
    }, // षि
    {
        2, (const uint16_t[]) { 0x095E, 0x093F }, (const uint16_t[])
        {
            0xE1DB, 0x095E, 0
        }
    }, // फ़ि
    {
        2, (const uint16_t[]) { 0x092B, 0x093C }, (const uint16_t[])
        {
            0x095E, 0
        }
    }, // फ़
    {
        2, (const uint16_t[]) { 0x0915, 0x0940 }, (const uint16_t[])
        {
            0x0915, 0xE20A, 0
        }
    }, // की

    // 中间0x9CD变形情况
};

void init_file_km(string filename)
{
	ofstream ofs;
	ofs.open(filename);
	ofs << R"(class TagMap:
    def __init__(self, p_key: bytes, p_value: bytes):
        self.p_key = p_key
        self.p_value = p_value)" << endl << endl << endl;
	ofs << R"(s_cvt_map_km = [)";
	ofs.close();
}

void init_file_hi(string filename)
{
	ofstream ofs;
	ofs.open(filename);
	ofs << R"(class TagMap:
    def __init__(self, p_key: bytes, p_value: bytes):
        self.p_key = p_key
        self.p_value = p_value)" << endl << endl << endl;
	ofs << R"(s_cvt_map_hi = [)";
	ofs.close();
}

int main()
{
    ofstream ofs;
    char strbuffer[32]{};
    init_file_km("Khmer.py");
    ofs.open("Khmer.py", ios::app);
    for (uint16_t i = 0; i < sizeof(s_cvt_map_km)/sizeof(s_cvt_map_km[0]); i++)
    {
        ofs << R"(
    TagMap(
        p_key=b')";
        for (int j = 0; j < s_cvt_map_km[i].keyLen; ++j)
        {
            sprintf(strbuffer, R"(\x%02X\x%02X)", s_cvt_map_km[i].pKey[j] >> 8, s_cvt_map_km[i].pKey[j] & 0x00FF);
            ofs << strbuffer;
        }
        ofs << "'," << endl;
        ofs << R"(        p_value=b')";
        int k = 0;
        while(s_cvt_map_km[i].pValue[k] != 0)
        {
            sprintf(strbuffer, R"(\x%02X\x%02X)", s_cvt_map_km[i].pValue[k] >> 8, s_cvt_map_km[i].pValue[k] & 0x00FF);
            ofs << strbuffer;
            k++;
        }
        ofs << "'" << endl;
        if (i != sizeof(s_cvt_map_km)/sizeof(s_cvt_map_km[0]) - 1)
            ofs << R"(    ),)";
        else
            ofs << R"(    ))" << endl;
    }
    ofs << R"(])" << endl;
    ofs.close();

    init_file_hi("Hindi.py");
    ofs.open("Hindi.py", ios::app);
    for (uint16_t i = 0; i < sizeof(s_cvt_map_hi)/sizeof(s_cvt_map_hi[0]); i++)
    {
        ofs << R"(
    TagMap(
        p_key=b')";
        for (int j = 0; j < s_cvt_map_hi[i].keyLen; ++j)
        {
            sprintf(strbuffer, R"(\x%02X\x%02X)", s_cvt_map_hi[i].pKey[j] >> 8, s_cvt_map_hi[i].pKey[j] & 0x00FF);
            ofs << strbuffer;
        }
        ofs << "'," << endl;
        ofs << R"(        p_value=b')";
        int k = 0;
        while(s_cvt_map_hi[i].pValue[k] != 0)
        {
            sprintf(strbuffer, R"(\x%02X\x%02X)", s_cvt_map_hi[i].pValue[k] >> 8, s_cvt_map_hi[i].pValue[k] & 0x00FF);
            ofs << strbuffer;
            k++;
        }
        ofs << "'" << endl;
        if (i != sizeof(s_cvt_map_hi)/sizeof(s_cvt_map_hi[0]) - 1)
            ofs << R"(    ),)";
        else
            ofs << R"(    ))" << endl;
    }
    ofs << R"(])" << endl;
    ofs.close();
}
