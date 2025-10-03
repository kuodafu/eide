#pragma once
#include <windows.h>

// 获取易语言版本, 返回0表示获取失败
// 返回的版本格式: 
// 5.91 = 591
// 5.92 = 592, 5.92有两个版本, 有一个返回592, 一个返回5922
// 5.95 = 595
// 以此类推
static int GetEVersion()
{
    LPBYTE pStart = (LPBYTE)GetModuleHandleW(0);
    if (pStart == NULL)
        return 0;

    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pStart;
    if (pDos->e_magic != IMAGE_DOS_SIGNATURE)
        return 0;
    IMAGE_NT_HEADERS32* pNt = (IMAGE_NT_HEADERS32*)(pStart + pDos->e_lfanew);
    if (pNt->Signature != IMAGE_NT_SIGNATURE)
        return 0;

    const BYTE pattern[]    = { 0xb8, 0x43, 0x36, 0x54, 0x98 };
    const int pattern_size  = sizeof(pattern);
    BYTE next[pattern_size] = { 0 };

    LPBYTE pEnd = pStart + pNt->OptionalHeader.SizeOfImage - sizeof(pattern);
    pStart += pNt->OptionalHeader.SizeOfHeaders;
    int find_size = (int)(pEnd - pStart);

    int j = 0;
    for (int i = 1; i < pattern_size; i++)
    {
        while (j > 0 && pattern[j] != pattern[i])
            j = next[j - 1];
        if (pattern[j] == pattern[i])
            j++;
        next[i] = j;
    }


    auto pfn_KMPSearch = [&]() -> int
    {
        j = 0;
        for (int i = 0; i < find_size; i++)
        {
            while (j > 0 && pattern[j] != pStart[i])
                j = next[j - 1];
            if (pattern[j] == pStart[i])
                j++;
            if (j == pattern_size)
            {
                return i - pattern_size + 1;
            }
        }
        return -1;
    };


    //int offset = -1;
    //for (int i = 0; i < find_size; i++)
    //{
    //    if (std::memcmp(pStart + i, pattern, sizeof(pattern)) == 0)
    //    {
    //        offset = i;
    //        break;
    //    }
    //}

    int offset = pfn_KMPSearch();

    enum EVERSION
    {
        // CRCKEY地址
        // CRC_KEY B843365498
        ADDRESS_ELANG_CRC_KEY_511 = 0x004DE890,
        ADDRESS_ELANG_CRC_KEY_520 = 0x004DFB40,
        ADDRESS_ELANG_CRC_KEY_530 = 0x004E5380,
        ADDRESS_ELANG_CRC_KEY_541 = 0x004ECA20,
        ADDRESS_ELANG_CRC_KEY_550 = 0x004EE370,
        ADDRESS_ELANG_CRC_KEY_560 = 0x004F3610,
        ADDRESS_ELANG_CRC_KEY_570 = 0x004F5180,
        ADDRESS_ELANG_CRC_KEY_571 = 0x004F5160,
        ADDRESS_ELANG_CRC_KEY_580 = 0x004F5620,
        ADDRESS_ELANG_CRC_KEY_581 = 0x004F5710,
        ADDRESS_ELANG_CRC_KEY_590 = 0x004FD800,
        ADDRESS_ELANG_CRC_KEY_592 = 0x004FFBB0,
        ADDRESS_ELANG_CRC_KEY_5922 = 0x004FFB00,
        ADDRESS_ELANG_CRC_KEY_593 = 0x00501910,
        ADDRESS_ELANG_CRC_KEY_595 = 0x00501BA0,
    };

    EVERSION addr = (EVERSION)(pNt->OptionalHeader.ImageBase + offset + 0x1000);
    switch (addr)
    {
    case ADDRESS_ELANG_CRC_KEY_511:
        return 511;
    case ADDRESS_ELANG_CRC_KEY_520:
        return 520;
    case ADDRESS_ELANG_CRC_KEY_530:
        return 530;
    case ADDRESS_ELANG_CRC_KEY_541:
        return 541;
    case ADDRESS_ELANG_CRC_KEY_550:
        return 550;
    case ADDRESS_ELANG_CRC_KEY_560:
        return 560;
    case ADDRESS_ELANG_CRC_KEY_570:
        return 570;
    case ADDRESS_ELANG_CRC_KEY_571:
        return 571;
    case ADDRESS_ELANG_CRC_KEY_580:
        return 580;
    case ADDRESS_ELANG_CRC_KEY_581:
        return 581;
    case ADDRESS_ELANG_CRC_KEY_590:
        return 590;
    case ADDRESS_ELANG_CRC_KEY_592:
        return 592;
    case ADDRESS_ELANG_CRC_KEY_5922:
        return 5922;
    case ADDRESS_ELANG_CRC_KEY_593:
        return 593;
    case ADDRESS_ELANG_CRC_KEY_595:
        return 595;
    default:
        break;
    }

    return  0;
}