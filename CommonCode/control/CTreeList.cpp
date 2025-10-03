#include "CTreeList.h"
#pragma comment(lib, "Comctl32.lib")
#include <windowsx.h>
#include <hook/apiHook.h>
#ifdef _DEBUG
#include <assist/CFileRW.h>
#endif
#include <MemDC.h>
#include <control/Ctips.h>

#define TREELIST_HOT_LIST 1     // 为1则热点是按列来处理
inline static bool _md_create(HMD& hmd, int width, int height)
{
    if (hmd.height == height && hmd.width == width)
        return true;
    if (width <= 0)width = 1;
    if (height <= 0)height = 1;
    hmd = { 0 };
    hmd.width = width;
    hmd.height = height;
    hmd.hdc = CreateCompatibleDC(0);
    if (hmd.hdc)
    {
        BITMAPINFO bi = { 0 };
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biSizeImage = width * height * bi.bmiHeader.biBitCount / 8;
        hmd.hBitmap = CreateDIBSection(hmd.hdc, &bi, 0, &hmd.lpBits, 0, 0);
        if (hmd.hBitmap)
        {
            DeleteObject(SelectObject(hmd.hdc, hmd.hBitmap));
            true;
        }
        else
        {
            DeleteDC(hmd.hdc);
            hmd = { 0 };
        }
    }
    return false;
}
inline static bool _md_destroy(HMD& hmd)
{
    if (hmd.hdc)
    {
        DeleteObject(hmd.hBitmap);
        DeleteDC(hmd.hdc);
        hmd = { 0 };
        return true;
    }
    return false;
}
const unsigned char s_ImageTreep[] =
{
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x01, 0x54,
0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x49, 0x2C, 0x87, 0x7B, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47,
0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC,
0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0E, 0xC3, 0x00, 0x00, 0x0E, 0xC3, 0x01, 0xC7,
0x6F, 0xA8, 0x64, 0x00, 0x00, 0x0F, 0x99, 0x49, 0x44, 0x41, 0x54, 0x78, 0x5E, 0xED, 0x9B, 0x0B, 0x74, 0x54, 0xD5, 0xB9,
0xC7, 0xBF, 0x7D, 0x66, 0xE6, 0x4C, 0x12, 0x32, 0x99, 0xC9, 0x63, 0x98, 0x99, 0x24, 0x28, 0x04, 0x14, 0x05, 0xC2, 0xA3,
0x2A, 0xC8, 0x4A, 0x10, 0xBD, 0x96, 0xF2, 0xA8, 0x04, 0x7C, 0x80, 0xB5, 0xBD, 0xEB, 0x4A, 0x6F, 0xAB, 0xB7, 0x68, 0x28,
0x08, 0xF5, 0x16, 0x09, 0x26, 0x2B, 0x31, 0x20, 0x6B, 0xDD, 0x06, 0xF4, 0x16, 0x9B, 0xDE, 0xB4, 0xF6, 0xB5, 0x7A, 0xBD,
0x0B, 0x23, 0x22, 0xB8, 0x5A, 0xA5, 0x3E, 0xB8, 0x98, 0x20, 0xCA, 0x43, 0x9E, 0x46, 0x1E, 0x0A, 0xF2, 0x48, 0x86, 0x3C,
0x48, 0xC2, 0x24, 0x24, 0x99, 0xD7, 0xD9, 0xFD, 0xBE, 0x3D, 0x67, 0xE2, 0x64, 0x32, 0x99, 0x64, 0x12, 0x4C, 0x5A, 0x99,
0xDF, 0x5A, 0x67, 0xED, 0xFD, 0x7D, 0xE7, 0xFB, 0x66, 0x9F, 0x99, 0x9C, 0xFC, 0xCF, 0x7E, 0x9C, 0x0D, 0x51, 0xA2, 0x44,
0x89, 0x12, 0xE5, 0xDA, 0xC0, 0xD4, 0x32, 0x4A, 0x14, 0x78, 0xE3, 0x8D, 0x37, 0x2A, 0x24, 0x49, 0xCA, 0x56, 0xCD, 0x5E,
0x51, 0x14, 0xA5, 0x72, 0xE1, 0xC2, 0x85, 0x33, 0xA8, 0x9E, 0x5C, 0x78, 0x0A, 0x73, 0x35, 0x11, 0xE4, 0x7A, 0x2B, 0x2F,
0x17, 0xDC, 0xEC, 0xCB, 0x7D, 0xEE, 0x8B, 0x4A, 0x09, 0x20, 0x4B, 0x9C, 0xE8, 0x03, 0x81, 0xB9, 0x39, 0x95, 0x96, 0x0A,
0xCC, 0xED, 0x7B, 0xBB, 0x00, 0x95, 0x3B, 0xB2, 0x6B, 0x7D, 0xB9, 0xBF, 0xB9, 0xA5, 0x02, 0xFF, 0x03, 0xFA, 0x9C, 0x0B,
0x1C, 0x73, 0x1F, 0x3B, 0x31, 0xE0, 0x76, 0x83, 0x48, 0xC2, 0x63, 0x24, 0x1E, 0xC9, 0x78, 0xC8, 0x78, 0xB8, 0xF0, 0xB8,
0x8C, 0xC7, 0x97, 0x78, 0x34, 0xE2, 0xD1, 0x23, 0xB6, 0xE7, 0x0E, 0x67, 0x73, 0x26, 0x3D, 0xC9, 0x19, 0xE0, 0xE7, 0x72,
0x33, 0xFE, 0x3B, 0xD7, 0x33, 0x0E, 0x15, 0x8C, 0x2B, 0x2F, 0xD9, 0x9F, 0x9D, 0x5C, 0xA9, 0x86, 0x85, 0x64, 0x20, 0xB9,
0x2B, 0x57, 0xAE, 0xE4, 0x6A, 0xB5, 0x1B, 0x1B, 0x37, 0x6E, 0x0C, 0xAB, 0x29, 0x73, 0x7F, 0x77, 0x29, 0x53, 0x51, 0x34,
0xF9, 0x9C, 0xF3, 0xE9, 0x8C, 0xB1, 0x34, 0x2C, 0xAB, 0xB1, 0xDC, 0x2B, 0x49, 0xDE, 0xA2, 0xB7, 0xFE, 0xDD, 0x7A, 0x4C,
0x0D, 0x0B, 0xC9, 0x43, 0xDB, 0x94, 0x4C, 0x89, 0xF3, 0x7C, 0x06, 0x7C, 0x3A, 0x5E, 0x6F, 0x1A, 0x5E, 0x77, 0x35, 0x07,
0xB6, 0x57, 0x61, 0xAC, 0xE8, 0xB5, 0xFB, 0xA5, 0xB0, 0xB9, 0xE9, 0x25, 0xCE, 0x4C, 0xAF, 0x46, 0x93, 0x8F, 0x39, 0xD3,
0x19, 0xE6, 0x72, 0xCC, 0xC5, 0xCF, 0xD8, 0xAB, 0xF1, 0x7A, 0x8B, 0x2E, 0xAE, 0xD2, 0x87, 0xCD, 0x7D, 0x6A, 0x6D, 0x51,
0x26, 0x53, 0x94, 0x7C, 0x86, 0xD7, 0x0C, 0x78, 0xCD, 0x80, 0xD7, 0xCC, 0xF1, 0x9A, 0xB9, 0x24, 0x15, 0x6D, 0x2A, 0xCE,
0xEF, 0x92, 0xDB, 0xE3, 0x97, 0x5F, 0xBE, 0x7C, 0xF9, 0xF4, 0x9A, 0x9A, 0x9A, 0x7D, 0xE5, 0xE5, 0xE5, 0x5E, 0xD5, 0x35,
0xA8, 0x24, 0xFE, 0xF2, 0xF4, 0xF4, 0x26, 0xCB, 0x98, 0x7D, 0xB0, 0x98, 0x0D, 0x49, 0xFB, 0xD7, 0x23, 0x3B, 0x76, 0xEC,
0xE0, 0xF3, 0xE7, 0xCF, 0x57, 0xAD, 0xDE, 0x79, 0xF3, 0xCD, 0x37, 0x21, 0x27, 0x27, 0x47, 0xDC, 0x43, 0xE6, 0xE7, 0xBE,
0xE0, 0x75, 0x6B, 0x33, 0x84, 0xBF, 0x2F, 0x0C, 0x2F, 0x3E, 0x03, 0xF5, 0xCF, 0x8E, 0xEE, 0xCC, 0xFD, 0xC9, 0xB8, 0x76,
0xE1, 0xEF, 0x0B, 0xBF, 0xAE, 0x8A, 0xED, 0xCC, 0x5D, 0x58, 0x69, 0xE9, 0xF1, 0x9F, 0xBB, 0x27, 0xDE, 0xC8, 0xAE, 0x15,
0xB9, 0x39, 0xBF, 0xBD, 0x25, 0xE2, 0xDC, 0x1D, 0x3F, 0x3E, 0x31, 0xE0, 0x76, 0x55, 0xA8, 0x3E, 0x61, 0xCD, 0x9A, 0x35,
0xF3, 0x16, 0x2D, 0x7E, 0x78, 0xAE, 0x2D, 0xD5, 0x36, 0x41, 0x96, 0xF5, 0x46, 0x97, 0xCB, 0x79, 0xC5, 0x5E, 0x63, 0x3F,
0x5E, 0xFE, 0xEA, 0x96, 0xB7, 0xD6, 0xAF, 0x5F, 0xFF, 0x57, 0x8C, 0xF9, 0x14, 0x0F, 0xD4, 0xE3, 0x00, 0x0A, 0x76, 0x69,
0x2D, 0x72, 0xF2, 0x8B, 0xB7, 0xA7, 0xC6, 0x3C, 0xF1, 0x48, 0x66, 0x02, 0x4C, 0x4D, 0x8B, 0x81, 0xA4, 0x58, 0x0D, 0x34,
0xB6, 0x7B, 0x61, 0x5F, 0x75, 0x07, 0xFC, 0xDF, 0x31, 0x07, 0x1C, 0xA8, 0xE9, 0xF8, 0x55, 0x6D, 0x8A, 0x6B, 0x05, 0xFC,
0xC7, 0xED, 0x6E, 0x35, 0xCB, 0x87, 0x9A, 0x8B, 0xB5, 0x89, 0x28, 0x2A, 0xA5, 0x32, 0x57, 0x76, 0x27, 0xB8, 0x75, 0xF5,
0x0E, 0x9D, 0xDB, 0xEC, 0x62, 0xD2, 0x4C, 0x14, 0x9B, 0xA5, 0x78, 0xEE, 0x68, 0xA8, 0x5C, 0xBF, 0x90, 0x0E, 0xB7, 0x58,
0x60, 0xF6, 0x9C, 0xB9, 0x60, 0xB3, 0xD9, 0x40, 0xAF, 0xD7, 0x83, 0xD3, 0xE9, 0x04, 0xBB, 0xDD, 0x0E, 0x3B, 0xDF, 0x7E,
0x0B, 0xEA, 0x6A, 0x6B, 0x45, 0x6C, 0xB0, 0xB0, 0x2E, 0x7A, 0x95, 0xCB, 0x57, 0xAE, 0xD4, 0x3F, 0x8D, 0xD5, 0xFC, 0x07,
0x47, 0xB7, 0xC9, 0x77, 0xA4, 0x6B, 0xC1, 0x92, 0x20, 0xC3, 0x15, 0xA7, 0x04, 0x9F, 0x5C, 0x52, 0xE0, 0x4F, 0x47, 0xC5,
0x83, 0xA4, 0xC8, 0x68, 0x34, 0x6F, 0x28, 0x0F, 0xFA, 0x9F, 0xA7, 0x5C, 0x49, 0xEB, 0x15, 0xB9, 0x3F, 0x18, 0xD5, 0x24,
0xDF, 0x10, 0xEF, 0xC4, 0xEF, 0xCB, 0xA0, 0xD5, 0xA3, 0x85, 0xF3, 0x57, 0xF5, 0xF0, 0xFB, 0x93, 0xF1, 0x22, 0x57, 0xF1,
0x68, 0xBA, 0xE5, 0x8E, 0x2B, 0xE0, 0x72, 0xB3, 0xC9, 0xFB, 0x34, 0x67, 0x3C, 0xFF, 0x87, 0xE9, 0xE7, 0xE4, 0x7B, 0x47,
0x79, 0x61, 0x84, 0x49, 0x86, 0xBA, 0xAB, 0x0C, 0x76, 0x57, 0xEB, 0x60, 0xF3, 0xC9, 0xE1, 0x2E, 0xC6, 0x59, 0x51, 0x4D,
0x9A, 0x66, 0x43, 0xB0, 0xD6, 0x14, 0x14, 0x14, 0xC8, 0x2D, 0x6E, 0xF6, 0x34, 0x3E, 0x48, 0xF3, 0x47, 0x8D, 0x48, 0x95,
0xAD, 0x36, 0x2B, 0x98, 0x93, 0x8C, 0xD0, 0xEC, 0x68, 0x03, 0xFB, 0x25, 0x3B, 0x9C, 0x3E, 0x73, 0xDE, 0x85, 0x7F, 0xC5,
0xA2, 0x0B, 0xA7, 0xC7, 0x6D, 0x28, 0x2F, 0x5F, 0x2C, 0x72, 0xF1, 0x21, 0x1B, 0x1A, 0x7C, 0x6A, 0xBC, 0x6C, 0xB5, 0x5A,
0x5F, 0xC7, 0x0F, 0xD5, 0xAA, 0xAE, 0x41, 0x45, 0xE1, 0xFC, 0xE5, 0xC4, 0xBA, 0x53, 0xAF, 0x43, 0x01, 0x1F, 0xF4, 0xF6,
0x17, 0x2C, 0x58, 0xB0, 0x04, 0x8F, 0x3F, 0xA8, 0xE6, 0xA0, 0x63, 0xDC, 0x7C, 0x72, 0x49, 0xE2, 0xE6, 0x53, 0x43, 0xD6,
0xFE, 0x50, 0x51, 0xF4, 0xC0, 0xF8, 0x5E, 0x8F, 0xFE, 0xB2, 0x2D, 0xEB, 0x92, 0x5A, 0x8B, 0x9C, 0xED, 0x3F, 0xFA, 0x4C,
0xAD, 0x45, 0x4E, 0x98, 0x76, 0x27, 0x96, 0x6F, 0xDD, 0x9A, 0xFB, 0x9F, 0xAB, 0x9F, 0x79, 0x3E, 0x63, 0xF4, 0xE8, 0x99,
0xB1, 0xB1, 0x71, 0xC9, 0x1A, 0x8D, 0x46, 0x4B, 0x25, 0xD9, 0xE4, 0x7F, 0xED, 0xB5, 0xAD, 0xCB, 0x30, 0x2E, 0xD3, 0x17,
0xFE, 0x15, 0x56, 0x39, 0xB9, 0xF4, 0xDF, 0x26, 0x19, 0x9F, 0xF8, 0xDF, 0x87, 0x52, 0x61, 0xFE, 0xD8, 0x78, 0xB0, 0xC4,
0x6B, 0x41, 0xA7, 0x61, 0xA2, 0x24, 0x9B, 0xFC, 0x8F, 0xE2, 0x79, 0x6B, 0x83, 0xFC, 0x2B, 0x35, 0xA5, 0x13, 0xCA, 0xC5,
0x9E, 0x96, 0xB7, 0x76, 0x4D, 0xE6, 0x5D, 0x75, 0x79, 0x93, 0x5E, 0xB9, 0xB8, 0x76, 0x4A, 0x75, 0x55, 0xE1, 0x04, 0x17,
0x95, 0x64, 0x93, 0x1F, 0x7B, 0x80, 0x9E, 0x50, 0xB9, 0x44, 0x56, 0xF6, 0x0C, 0x58, 0xFA, 0xC4, 0x93, 0x90, 0x91, 0x91,
0x01, 0xB1, 0xB1, 0xB1, 0x80, 0x23, 0x1A, 0x51, 0x92, 0x4D, 0x7E, 0x3A, 0x1F, 0x0A, 0x87, 0xA3, 0xBE, 0x24, 0xCB, 0xD2,
0x56, 0x5C, 0x9C, 0xD5, 0x2E, 0x2F, 0x98, 0x60, 0x80, 0x51, 0xD6, 0x44, 0x30, 0x18, 0x12, 0x60, 0x84, 0xD9, 0x00, 0x39,
0x13, 0x12, 0xA0, 0x34, 0x27, 0x4E, 0x9E, 0x95, 0x21, 0x15, 0x63, 0xDC, 0x0B, 0x6A, 0x4A, 0x27, 0x4C, 0xA3, 0x94, 0xCC,
0xB6, 0x39, 0x8A, 0x7F, 0x3E, 0xAE, 0x46, 0x9E, 0x62, 0xF6, 0x82, 0xD5, 0x14, 0x07, 0x31, 0xB1, 0xC3, 0xC0, 0x9C, 0x10,
0x03, 0xDF, 0xB2, 0x02, 0x3C, 0x37, 0xAD, 0x45, 0x9E, 0x7F, 0xA3, 0xB3, 0x18, 0xE3, 0xBA, 0xE5, 0x36, 0x9B, 0x3C, 0x25,
0xF7, 0x24, 0xD5, 0x14, 0xFF, 0xE1, 0xCE, 0xD3, 0xF2, 0x0F, 0xA7, 0xE8, 0xE0, 0x26, 0xAB, 0x09, 0xE2, 0x86, 0x61, 0xFB,
0x16, 0x03, 0x3C, 0x3A, 0x45, 0x0F, 0xDB, 0xE7, 0x5D, 0x96, 0xE7, 0xA5, 0x39, 0x8A, 0x53, 0xAB, 0x3D, 0xDD, 0x72, 0x5B,
0x5C, 0xAC, 0xC4, 0x92, 0x18, 0x5F, 0x7C, 0xE7, 0xED, 0x53, 0xE4, 0xF1, 0xB7, 0x8E, 0x05, 0xAB, 0x39, 0x05, 0x74, 0x3A,
0x3D, 0x98, 0x53, 0x12, 0x61, 0xE2, 0xF8, 0x71, 0x70, 0xCF, 0x5D, 0x59, 0x72, 0xFA, 0xF0, 0xC4, 0xE2, 0x11, 0x63, 0xAA,
0x3A, 0x73, 0x7B, 0x14, 0x54, 0xFC, 0xE3, 0xCE, 0xC5, 0xE2, 0xB6, 0xC6, 0xC6, 0xC6, 0xAD, 0x43, 0x21, 0xAA, 0x4C, 0xD2,
0xCE, 0x55, 0x38, 0xBB, 0x2D, 0xD1, 0x7C, 0x6A, 0xEB, 0x60, 0x8B, 0x2A, 0x3E, 0x4C, 0x96, 0xE3, 0x8D, 0xF2, 0xE8, 0x50,
0x89, 0xAA, 0x84, 0xED, 0x33, 0x89, 0x3D, 0x7A, 0x3D, 0x8A, 0xEA, 0xD7, 0x81, 0x5F, 0xD4, 0xFA, 0x23, 0xAA, 0x7E, 0x31,
0xED, 0x8F, 0xA8, 0x86, 0x69, 0x37, 0xE9, 0x99, 0x67, 0x9E, 0x99, 0xFD, 0x9D, 0xEF, 0xCC, 0x7E, 0x1C, 0xEF, 0xB5, 0x2E,
0x3D, 0x39, 0x3F, 0xE4, 0x9F, 0x35, 0x7B, 0xF6, 0x63, 0x14, 0x87, 0x26, 0x4D, 0x0B, 0x08, 0x6C, 0xEB, 0x8E, 0x65, 0x7D,
0xCB, 0x16, 0xF3, 0xE3, 0xB5, 0x33, 0x93, 0x7B, 0x1C, 0x5E, 0x92, 0x3F, 0x0F, 0xCF, 0x53, 0x1C, 0xC5, 0xFB, 0xBC, 0xBE,
0x5C, 0x1C, 0x62, 0xDF, 0x7A, 0x69, 0xED, 0xA4, 0x9F, 0x62, 0x03, 0xA1, 0x7B, 0xD8, 0xE8, 0xBF, 0x94, 0x37, 0x69, 0x39,
0xC5, 0x05, 0xE6, 0x52, 0xEF, 0xD4, 0x3C, 0xDC, 0x02, 0xDF, 0x9E, 0x35, 0x4B, 0xF5, 0x84, 0x86, 0xCE, 0x53, 0x5C, 0xE0,
0xB4, 0xC0, 0x9C, 0x97, 0x6B, 0xB3, 0x6F, 0x8C, 0x77, 0xE7, 0x3E, 0x32, 0x81, 0xA1, 0x90, 0x9A, 0x60, 0xDB, 0x59, 0x09,
0x9E, 0xFA, 0x5B, 0x3B, 0x3C, 0xBC, 0xA5, 0x05, 0x96, 0xFD, 0xB5, 0x05, 0x5E, 0xF9, 0xD4, 0x29, 0x7A, 0xAB, 0x8F, 0x4D,
0x8B, 0x87, 0xB1, 0xC9, 0x52, 0x2E, 0xC5, 0xAB, 0xA9, 0xB0, 0xE8, 0x75, 0x9E, 0x9D, 0x99, 0xD8, 0x91, 0x9B, 0x65, 0xBE,
0x02, 0x16, 0x53, 0x2C, 0xEC, 0x77, 0x24, 0xC0, 0x8B, 0xC7, 0x0D, 0xF0, 0xD3, 0xDD, 0x06, 0xD8, 0x78, 0x24, 0x0E, 0x2A,
0x6B, 0x65, 0x48, 0x8C, 0x93, 0x60, 0xD6, 0x48, 0x37, 0x4C, 0x4E, 0x71, 0xE7, 0x52, 0xBC, 0x9A, 0x0A, 0xB6, 0x17, 0x5C,
0xD9, 0x19, 0x71, 0x57, 0x73, 0x57, 0x4D, 0x69, 0x85, 0x9B, 0x52, 0x8D, 0xF0, 0xEB, 0xD3, 0x09, 0xF0, 0xC0, 0x4E, 0x13,
0x4C, 0xDB, 0x62, 0x82, 0x05, 0x7F, 0x89, 0x87, 0x4D, 0x47, 0xB4, 0x90, 0x6E, 0xD2, 0x41, 0xFE, 0x0C, 0x05, 0x6E, 0x31,
0xBA, 0x72, 0x29, 0x5E, 0x4D, 0x85, 0x95, 0x79, 0x05, 0xD9, 0xC6, 0x18, 0x96, 0x3B, 0x31, 0x33, 0x13, 0x86, 0x9B, 0x93,
0xA0, 0xEA, 0xC4, 0x49, 0x78, 0xEF, 0xDD, 0xBF, 0xC1, 0xB6, 0xBF, 0xEC, 0x84, 0x77, 0x76, 0xBE, 0x0D, 0x87, 0x8E, 0x1C,
0x05, 0x73, 0xB2, 0x09, 0xA6, 0x4E, 0x9D, 0x8A, 0xED, 0x6B, 0x72, 0x29, 0x9E, 0xF2, 0x7A, 0x14, 0xD4, 0x92, 0x92, 0x92,
0x73, 0xB2, 0x2C, 0xD3, 0x0F, 0x3A, 0x24, 0xA2, 0xDA, 0x9C, 0x9B, 0x71, 0x8E, 0x31, 0x6D, 0xD6, 0x50, 0x88, 0x2A, 0xDE,
0x4C, 0xF7, 0xE0, 0x71, 0x38, 0x52, 0x51, 0xA5, 0x9E, 0xED, 0xC2, 0x85, 0x0B, 0x0F, 0x61, 0x69, 0x52, 0x5D, 0x9D, 0x90,
0x4F, 0x3D, 0xB7, 0x44, 0x75, 0xF5, 0x88, 0xE2, 0xD2, 0x89, 0xF6, 0x23, 0x15, 0x55, 0xD1, 0xB3, 0x7D, 0xE9, 0xD4, 0x21,
0xE3, 0xA6, 0xB3, 0xDD, 0xDA, 0x27, 0x9F, 0x38, 0x87, 0x31, 0xAA, 0xEB, 0xBA, 0x20, 0x50, 0xCC, 0xEE, 0xDF, 0x83, 0xDD,
0x99, 0x08, 0x08, 0x14, 0xD1, 0x05, 0x2F, 0xDF, 0xAA, 0xD6, 0xFA, 0x46, 0x2F, 0xED, 0x8E, 0x5C, 0xF4, 0xBD, 0xEF, 0x51,
0x87, 0xA5, 0x57, 0xD4, 0x38, 0x9A, 0x63, 0x15, 0xE0, 0x30, 0xFD, 0xC9, 0xEF, 0x4F, 0x4C, 0x50, 0xAD, 0xF0, 0x50, 0x1C,
0xDE, 0x47, 0xB9, 0xAA, 0x29, 0x72, 0x25, 0xCE, 0x4A, 0x55, 0x33, 0x2C, 0x14, 0x17, 0x98, 0x4B, 0xCC, 0x99, 0xDB, 0xA7,
0x4B, 0xEE, 0x16, 0xC7, 0x39, 0x5B, 0x3A, 0x23, 0xCD, 0x03, 0x46, 0x83, 0x01, 0x5E, 0x3B, 0xCD, 0x61, 0xC7, 0x71, 0x0F,
0xD8, 0x9B, 0x39, 0x28, 0x0A, 0x88, 0x72, 0xFB, 0x31, 0x17, 0xFC, 0xF9, 0x58, 0x3B, 0x68, 0x35, 0x12, 0xCC, 0x19, 0x1B,
0x23, 0xE2, 0xD5, 0x54, 0x7C, 0x38, 0x28, 0x4B, 0xEF, 0x48, 0x6E, 0x83, 0x61, 0xD8, 0x0B, 0xFE, 0xA8, 0x71, 0x18, 0xBC,
0x7A, 0x42, 0x0F, 0x67, 0x9A, 0x24, 0xFC, 0x2E, 0x20, 0x4A, 0xB2, 0x2B, 0xEC, 0x3A, 0xEC, 0x29, 0x33, 0xC8, 0x4E, 0xF5,
0x8A, 0x78, 0x5F, 0x26, 0xE6, 0xE2, 0xE7, 0xCC, 0xB3, 0x35, 0x40, 0x42, 0x7C, 0x3C, 0x6C, 0xAE, 0x8A, 0x83, 0x2D, 0x27,
0x0C, 0x50, 0xE3, 0xD0, 0xE0, 0xE7, 0x83, 0x28, 0xB7, 0x7C, 0x16, 0x0F, 0x25, 0x87, 0x34, 0xD8, 0x2E, 0x83, 0xC5, 0x37,
0x75, 0x88, 0x78, 0x35, 0x15, 0x2F, 0x9A, 0x2D, 0x35, 0xDB, 0x46, 0x40, 0x8C, 0x3E, 0x06, 0x8E, 0x1D, 0xAF, 0x82, 0x2F,
0x2F, 0xDA, 0xB1, 0xC7, 0x2A, 0xB4, 0x41, 0x94, 0x64, 0x1F, 0x38, 0x78, 0x08, 0xB4, 0x5A, 0x2D, 0xA4, 0xDE, 0x90, 0x21,
0xE2, 0x29, 0xAD, 0x47, 0x41, 0x25, 0x22, 0x11, 0x55, 0x9A, 0x73, 0x5D, 0xB1, 0x62, 0x45, 0x15, 0x96, 0x9D, 0x7F, 0xFC,
0x60, 0x56, 0xAD, 0x5A, 0x75, 0xA3, 0x1A, 0x33, 0x5D, 0x75, 0x85, 0x25, 0x12, 0x51, 0xA5, 0x39, 0x57, 0xE3, 0x7F, 0x9F,
0xAA, 0x32, 0x96, 0x9E, 0xED, 0xB1, 0x7D, 0xD3, 0xE6, 0x33, 0x37, 0x52, 0x0C, 0xC5, 0xAA, 0xAE, 0x90, 0x6C, 0xDF, 0xBE,
0xBD, 0x19, 0x7F, 0xB8, 0xFE, 0x88, 0xEA, 0xDD, 0xD8, 0xB1, 0x98, 0x8C, 0xC7, 0xAE, 0x40, 0x51, 0xA5, 0x3A, 0xF9, 0xE8,
0x1C, 0x9A, 0x77, 0xFB, 0xBC, 0x3D, 0x73, 0xE5, 0xA9, 0x51, 0xCD, 0xFD, 0x11, 0x55, 0x09, 0x98, 0x68, 0x5F, 0x92, 0xDD,
0xBB, 0x02, 0x45, 0x95, 0xEA, 0xE4, 0x13, 0xE7, 0x30, 0x46, 0x75, 0x7F, 0xE3, 0xF9, 0x07, 0x15, 0x53, 0x22, 0x25, 0x2D,
0x35, 0x8D, 0xEE, 0x85, 0x5E, 0x49, 0xB5, 0xDA, 0x26, 0x62, 0x41, 0x0B, 0x56, 0x3E, 0x38, 0xFF, 0x97, 0xE9, 0x23, 0x62,
0x55, 0x23, 0x3C, 0xD3, 0xD2, 0x45, 0xDC, 0x5D, 0xC2, 0x20, 0x30, 0xD7, 0x2B, 0xC1, 0xBB, 0xAA, 0x15, 0x16, 0x17, 0xDE,
0x2F, 0x58, 0x7C, 0x95, 0x8B, 0xA4, 0xA5, 0xA5, 0xA9, 0xB5, 0xF0, 0xD0, 0xDC, 0x6A, 0x20, 0xA8, 0x41, 0x33, 0xC7, 0x5B,
0x34, 0x20, 0xEB, 0xF5, 0xF0, 0xF1, 0x05, 0x0F, 0x28, 0x41, 0x7D, 0x63, 0xB2, 0x3F, 0x3A, 0xEF, 0x9B, 0xAE, 0x9D, 0x60,
0xD1, 0x8A, 0x78, 0x61, 0x20, 0x0C, 0xF8, 0x4C, 0x6B, 0x9C, 0x13, 0xB4, 0xB2, 0x0C, 0x07, 0x6A, 0x75, 0x42, 0x48, 0x03,
0x21, 0x7B, 0x3F, 0xFA, 0x89, 0xB4, 0x78, 0x8F, 0x88, 0x17, 0x06, 0xC2, 0x19, 0xCC, 0x9C, 0x96, 0xE6, 0x15, 0xED, 0x7E,
0x50, 0x13, 0x17, 0x34, 0x11, 0xED, 0x9B, 0x98, 0xDE, 0x5D, 0xED, 0xFB, 0x2D, 0xA7, 0xA5, 0x2A, 0x22, 0x5E, 0x18, 0x08,
0xE3, 0x7C, 0xA6, 0x19, 0x87, 0xF8, 0x5A, 0x9D, 0x0E, 0x2E, 0xD7, 0xD9, 0x85, 0x90, 0x06, 0x42, 0x76, 0x43, 0x6D, 0x8D,
0xA8, 0x5B, 0xAD, 0x56, 0x11, 0x4F, 0xF5, 0xB0, 0x82, 0x4A, 0x04, 0x8A, 0x6A, 0x73, 0x73, 0xF3, 0x2B, 0x3E, 0x6F, 0x77,
0x4C, 0x26, 0xD3, 0x7E, 0xAF, 0xD7, 0x7B, 0x1A, 0xAB, 0x15, 0xCB, 0x96, 0x2D, 0x4B, 0xF7, 0x79, 0xBF, 0x82, 0xC4, 0xD4,
0xE5, 0x72, 0xED, 0xA1, 0x18, 0x5A, 0xEC, 0x52, 0xDD, 0xBD, 0x12, 0x28, 0xAA, 0x49, 0x96, 0xCF, 0x7B, 0x6C, 0xBF, 0xA9,
0x61, 0xCC, 0x7E, 0x49, 0xE2, 0xA7, 0x41, 0xF1, 0x54, 0x24, 0xBD, 0x78, 0xAA, 0x5B, 0xFB, 0x24, 0xA6, 0x9C, 0x7B, 0xF6,
0x50, 0x8C, 0x58, 0xEC, 0xEA, 0x85, 0xFE, 0x88, 0x2A, 0xE6, 0x2C, 0x51, 0x14, 0xE5, 0x8F, 0x24, 0x5C, 0x24, 0xA0, 0x24,
0xA4, 0x81, 0x62, 0x4A, 0xE7, 0x28, 0x46, 0x0D, 0x0F, 0x4B, 0x7F, 0x44, 0xB5, 0x29, 0xF7, 0xE6, 0x25, 0x5C, 0xE1, 0xA2,
0x7D, 0xBF, 0xA8, 0x06, 0x8A, 0x29, 0x9D, 0xA3, 0x18, 0x35, 0xFC, 0x1B, 0x45, 0xF0, 0xB0, 0x3A, 0x12, 0x31, 0x0D, 0x1E,
0xCE, 0x47, 0x22, 0xA6, 0xFD, 0x6C, 0x57, 0x1F, 0x13, 0x13, 0xD3, 0x6D, 0x14, 0x11, 0x0A, 0x14, 0x03, 0xEA, 0x8E, 0xD2,
0xEA, 0xBF, 0x0F, 0xC6, 0x2C, 0x29, 0x71, 0x1A, 0xD5, 0x08, 0x4F, 0x62, 0x0C, 0xF6, 0xE4, 0x18, 0x4F, 0x51, 0x4D, 0x91,
0x5B, 0x97, 0x37, 0xD1, 0xB7, 0x6A, 0xD4, 0x0B, 0x96, 0x36, 0x7D, 0x43, 0x97, 0x5C, 0x84, 0x16, 0xA0, 0xFA, 0x42, 0x88,
0x38, 0x73, 0xD2, 0x30, 0x1D, 0x4D, 0x23, 0x42, 0xAD, 0x23, 0x58, 0x12, 0x7D, 0xF8, 0xFD, 0x09, 0x78, 0xCD, 0x88, 0x59,
0x18, 0x3E, 0xCC, 0x06, 0x3D, 0x13, 0x73, 0xB5, 0x67, 0xB1, 0x47, 0x1A, 0x0A, 0xBF, 0x7F, 0x98, 0xAF, 0xD9, 0x2E, 0xB9,
0xE6, 0x78, 0x2D, 0x48, 0xD8, 0xAE, 0xBD, 0x25, 0xF4, 0x6F, 0xE6, 0xF7, 0x27, 0x0D, 0xEB, 0xDA, 0x2E, 0x07, 0x66, 0x36,
0x1A, 0xE2, 0x45, 0xBB, 0xAD, 0xEE, 0xD0, 0x93, 0x2B, 0x7E, 0x7F, 0x82, 0x61, 0x98, 0x88, 0xA7, 0x7A, 0xAF, 0x82, 0x4A,
0xA0, 0x10, 0x7A, 0xF1, 0xC7, 0x70, 0xE3, 0x3F, 0xA5, 0xEF, 0x51, 0x10, 0x82, 0xC2, 0xC2, 0x42, 0x4F, 0x52, 0x52, 0xD2,
0x83, 0x28, 0x1A, 0x9F, 0xE0, 0x45, 0x54, 0x06, 0x8A, 0xAA, 0x5F, 0x4C, 0xB1, 0x7A, 0x90, 0x62, 0x22, 0x7D, 0x73, 0x40,
0xF2, 0x7A, 0xF0, 0xC1, 0x0A, 0x6E, 0x1C, 0xB2, 0xF4, 0xD8, 0x3E, 0x14, 0x32, 0x4F, 0x53, 0xFD, 0xCD, 0x0F, 0x4A, 0x5C,
0xF9, 0x84, 0x6B, 0x79, 0x65, 0xA0, 0xA8, 0x76, 0x8A, 0x29, 0xE3, 0x07, 0x29, 0xE6, 0xEB, 0x7C, 0x73, 0x20, 0x58, 0x54,
0xD5, 0x23, 0x22, 0x31, 0x1D, 0x08, 0xC1, 0xA2, 0x7A, 0x3D, 0x89, 0x69, 0x70, 0x49, 0xF4, 0x55, 0x4C, 0x83, 0x4B, 0xA2,
0xAF, 0x62, 0xDA, 0x8F, 0x76, 0x9D, 0x1D, 0x1D, 0x1D, 0xCD, 0x6A, 0x3D, 0x2C, 0x2E, 0xA7, 0xD3, 0x41, 0x85, 0xCF, 0x42,
0x38, 0xAF, 0x6D, 0x68, 0xEB, 0xDB, 0xED, 0xDB, 0xD4, 0xA1, 0xD0, 0x30, 0xB6, 0x41, 0x35, 0x45, 0xEE, 0xF0, 0x75, 0x47,
0x2D, 0xAA, 0x15, 0x96, 0xDA, 0x38, 0x67, 0x4A, 0x97, 0x5C, 0x84, 0x56, 0xF3, 0xFB, 0x42, 0x88, 0xB8, 0xFA, 0x36, 0x8F,
0x06, 0xF5, 0x9C, 0x81, 0x25, 0x21, 0xB4, 0x38, 0xF9, 0xFD, 0x0E, 0xBC, 0x66, 0xA4, 0x5E, 0x18, 0x3E, 0xEA, 0x9D, 0x8A,
0x56, 0xE4, 0x8E, 0x4A, 0x0C, 0xEE, 0x63, 0xFA, 0xF0, 0xFB, 0xAF, 0xFA, 0x9A, 0xED, 0x92, 0xEB, 0x70, 0xFA, 0xDA, 0xB5,
0x19, 0x42, 0xFF, 0x66, 0x7E, 0x7F, 0xE3, 0xD5, 0xAE, 0xED, 0x62, 0x4F, 0xB7, 0xBE, 0xAD, 0x1D, 0x3F, 0x10, 0x2F, 0x2B,
0x5E, 0x17, 0xFA, 0x21, 0xE0, 0xF7, 0x3B, 0x5A, 0xAE, 0x8A, 0x78, 0xAA, 0xF7, 0x2A, 0xA8, 0x24, 0x8C, 0x24, 0x90, 0xD8,
0xB3, 0x3C, 0x69, 0x34, 0x1A, 0x1F, 0x56, 0xDD, 0x21, 0x09, 0x10, 0xD5, 0x23, 0x7E, 0x51, 0x0D, 0x16, 0x53, 0x8A, 0xF1,
0x45, 0xF7, 0x0D, 0x12, 0x46, 0x12, 0x48, 0x26, 0xF1, 0x93, 0x4D, 0x29, 0xAE, 0xB0, 0xED, 0xFB, 0x45, 0x15, 0x1F, 0x17,
0x47, 0xFC, 0xA2, 0xDA, 0x4D, 0x4C, 0x31, 0x46, 0x8D, 0x0E, 0x4B, 0x7F, 0x7B, 0x96, 0x44, 0x90, 0xA8, 0xF6, 0x4B, 0x4C,
0x07, 0xD2, 0xB3, 0x0C, 0x14, 0xD5, 0xFE, 0xE4, 0xFF, 0xB3, 0x11, 0x28, 0x5E, 0x91, 0x88, 0x29, 0x11, 0x28, 0x9A, 0x91,
0x88, 0x29, 0x31, 0x80, 0x76, 0x1B, 0x6A, 0xAA, 0xAB, 0x8F, 0xA8, 0xF5, 0xB0, 0xD8, 0xED, 0x76, 0x7A, 0xCF, 0x91, 0xDE,
0x4B, 0x55, 0x61, 0xBB, 0xF6, 0x5E, 0xE8, 0xDB, 0x2B, 0x66, 0x1F, 0x5F, 0x14, 0x71, 0x1F, 0x08, 0x43, 0xC0, 0x70, 0x18,
0xCF, 0xEF, 0x55, 0x8D, 0xB0, 0x68, 0x3C, 0x5A, 0x9A, 0x1A, 0x0A, 0xC8, 0x05, 0xA8, 0xAE, 0xAE, 0x56, 0x6B, 0xE1, 0xA1,
0x57, 0xA8, 0x02, 0x41, 0x3D, 0xDB, 0xFD, 0x59, 0x03, 0x0E, 0xA9, 0x71, 0x98, 0x7C, 0xE7, 0x0D, 0x3A, 0x90, 0x82, 0x34,
0x95, 0x6C, 0xF2, 0x13, 0x9F, 0xD6, 0x79, 0x45, 0xBC, 0x30, 0x10, 0xEC, 0xF9, 0xED, 0xB6, 0xB7, 0xCB, 0x22, 0xF7, 0x0E,
0x8B, 0x9B, 0xF4, 0xAD, 0x0B, 0x64, 0x93, 0x9F, 0xA8, 0xB9, 0xAA, 0x15, 0xF1, 0xC2, 0x40, 0x18, 0x87, 0xDD, 0x07, 0x6A,
0x69, 0xCE, 0x94, 0xC3, 0xCC, 0xB4, 0xF6, 0x6E, 0x62, 0x47, 0x36, 0xF9, 0x89, 0x7D, 0x76, 0x14, 0x5E, 0x8C, 0x17, 0x06,
0xC2, 0x19, 0xDB, 0x5D, 0x57, 0x8F, 0x1A, 0x89, 0x9A, 0x99, 0x62, 0x49, 0x15, 0xA2, 0x1C, 0x08, 0xD9, 0xE4, 0x27, 0xEA,
0xEA, 0xEA, 0x44, 0x3C, 0xD5, 0xC3, 0x0A, 0xAA, 0x5F, 0x4C, 0x49, 0x20, 0x51, 0x0C, 0xEF, 0x43, 0x31, 0xFC, 0xEA, 0x49,
0xD9, 0x03, 0x01, 0xA2, 0x7A, 0x02, 0x7B, 0xB5, 0x15, 0xD7, 0x42, 0x4C, 0x49, 0x20, 0x1B, 0xEB, 0x6E, 0xBE, 0x0F, 0x16,
0x4F, 0xE8, 0xB5, 0x7D, 0xBF, 0xA8, 0xE2, 0xD7, 0x3F, 0xC1, 0x25, 0xA8, 0x18, 0x6C, 0x31, 0xF5, 0xE3, 0x17, 0xD5, 0xC1,
0x16, 0x53, 0x3F, 0x7E, 0x51, 0xFD, 0xA6, 0x8B, 0xA9, 0x9F, 0x60, 0x11, 0xEB, 0x8B, 0x98, 0xFA, 0x09, 0x16, 0xCF, 0xBE,
0x88, 0xA9, 0x9F, 0x7E, 0xB6, 0xFB, 0x25, 0xBD, 0x67, 0x8A, 0xF7, 0x46, 0xE8, 0xAE, 0x8F, 0x0A, 0x9D, 0xA7, 0x38, 0xAC,
0xD2, 0x4B, 0xFE, 0x02, 0x7A, 0xF1, 0x9E, 0xDE, 0x33, 0x0D, 0x9F, 0xE9, 0x9B, 0x97, 0xA4, 0x38, 0x8A, 0x57, 0x5D, 0xA2,
0x2E, 0xDE, 0x33, 0x2D, 0xC0, 0xFF, 0x8C, 0x70, 0xE0, 0x79, 0x8A, 0x0B, 0xCC, 0x25, 0xE8, 0x3D, 0x53, 0x12, 0xA7, 0x70,
0xD0, 0x79, 0x8A, 0x0B, 0x84, 0x31, 0x5E, 0xFA, 0xFE, 0x59, 0x0E, 0xD8, 0x29, 0x83, 0x7F, 0xCD, 0x8C, 0x85, 0x05, 0x99,
0x32, 0xD8, 0x4C, 0x34, 0x8C, 0xC7, 0x1E, 0x22, 0x96, 0x64, 0x93, 0x9F, 0xD8, 0x79, 0xCA, 0x25, 0xE2, 0x85, 0x81, 0x70,
0x90, 0x4A, 0x3F, 0xAE, 0xD5, 0xD3, 0x6F, 0x01, 0x33, 0x6C, 0x6E, 0x58, 0x7C, 0x8B, 0x13, 0x32, 0xB0, 0x47, 0x4A, 0xF2,
0x46, 0x25, 0xD9, 0xE4, 0x27, 0x2A, 0x6A, 0xF4, 0x22, 0x5E, 0x18, 0x08, 0xC7, 0xCF, 0xD9, 0x7E, 0xCE, 0x08, 0x0A, 0xB6,
0xBB, 0x6A, 0x8A, 0x17, 0x1E, 0xBE, 0xB5, 0x15, 0x52, 0x13, 0x84, 0x60, 0x8B, 0x92, 0x6C, 0xF2, 0x13, 0xE5, 0x5F, 0xE0,
0xB0, 0x3D, 0xA0, 0x5D, 0xFC, 0xB1, 0x4A, 0x6B, 0xAB, 0xCF, 0x89, 0x76, 0x6F, 0xBF, 0x6D, 0x0A, 0x8C, 0x4C, 0xB7, 0x81,
0x41, 0xF6, 0x09, 0x29, 0x95, 0x64, 0x93, 0x9F, 0xA8, 0x39, 0x7F, 0x46, 0xC4, 0x53, 0xBD, 0xC7, 0x1F, 0x96, 0x16, 0x97,
0x50, 0x4C, 0xF7, 0xE2, 0x07, 0x92, 0x98, 0x46, 0x24, 0x86, 0xAA, 0xA8, 0xE6, 0x60, 0x95, 0xE6, 0x1F, 0x3F, 0xEC, 0x8F,
0x98, 0xD2, 0xE2, 0x92, 0x57, 0xCB, 0xF6, 0x92, 0x98, 0x46, 0x22, 0x86, 0x02, 0x8C, 0x6D, 0x34, 0xBB, 0x73, 0x40, 0x03,
0x87, 0x35, 0x1A, 0xF6, 0x61, 0xA4, 0xF9, 0x03, 0x15, 0x53, 0x3F, 0x94, 0xDB, 0x9F, 0xFC, 0x6B, 0x35, 0x4C, 0xA7, 0xDC,
0xEB, 0x41, 0x4C, 0xFD, 0xF8, 0xC5, 0x2C, 0x12, 0x31, 0xF5, 0xE3, 0x17, 0xD1, 0x48, 0xC4, 0xD4, 0x4F, 0x3F, 0xDA, 0x6D,
0xA4, 0x97, 0xF6, 0xDF, 0x7B, 0xE7, 0x9D, 0xDF, 0xA2, 0x00, 0x85, 0x54, 0x28, 0xF2, 0xD3, 0x79, 0xF5, 0xE5, 0xFE, 0xCE,
0x1D, 0x53, 0xB4, 0x8B, 0xE9, 0x60, 0x75, 0xC7, 0x4B, 0xCF, 0x7F, 0x70, 0x99, 0x3A, 0x4F, 0x21, 0x21, 0x3F, 0x9D, 0xA7,
0xB8, 0xC0, 0x5D, 0x4F, 0x54, 0xC7, 0x5E, 0xD8, 0x11, 0xAB, 0x7C, 0x74, 0x13, 0x36, 0xD0, 0xB5, 0xCB, 0xE5, 0x07, 0xFD,
0x74, 0x9E, 0xE2, 0x02, 0x73, 0xE9, 0x45, 0x7D, 0x7A, 0x69, 0xFF, 0xBD, 0x77, 0xC3, 0xAF, 0x69, 0xD1, 0x79, 0x8A, 0x0B,
0x7C, 0xB1, 0xFF, 0xED, 0x1F, 0x59, 0x2A, 0x3F, 0x6F, 0x66, 0x65, 0xBF, 0x3F, 0xD0, 0x0E, 0x2D, 0x4E, 0x45, 0x88, 0xE7,
0xE6, 0xEF, 0x26, 0x40, 0xF9, 0x23, 0x46, 0x51, 0x92, 0xDD, 0xEA, 0xE2, 0xF0, 0xBB, 0x83, 0x1D, 0xF0, 0x59, 0xBD, 0xBB,
0x8C, 0xE2, 0xD5, 0x54, 0x28, 0x7F, 0x80, 0x55, 0x1E, 0xBE, 0xAC, 0x2F, 0xDB, 0x75, 0x41, 0x0F, 0x6D, 0xA8, 0x9B, 0x24,
0x9E, 0x2B, 0x27, 0xB7, 0xC1, 0x2F, 0xEF, 0x69, 0x15, 0x25, 0xD9, 0xED, 0x6E, 0x06, 0xEF, 0x5F, 0x90, 0xE1, 0x70, 0x3D,
0x2B, 0xA3, 0x78, 0x35, 0x15, 0xEC, 0x2B, 0xE4, 0xCA, 0x93, 0x2D, 0xB1, 0x65, 0xCF, 0x7F, 0xA8, 0x85, 0xE6, 0x76, 0x2E,
0xC4, 0x73, 0xC7, 0x7D, 0xAD, 0xB0, 0xFF, 0x91, 0x2B, 0xA2, 0x24, 0xFB, 0x4A, 0x07, 0xC0, 0xFA, 0xBD, 0x3A, 0xA8, 0x6A,
0x94, 0xCA, 0x28, 0x5E, 0x4D, 0x85, 0x8D, 0xEB, 0x0A, 0x2B, 0x9B, 0xDA, 0xBC, 0x65, 0x07, 0x0E, 0x1E, 0x80, 0x96, 0xD6,
0x36, 0x21, 0x9E, 0x73, 0xE6, 0x7D, 0x17, 0x1E, 0x5A, 0x30, 0x4F, 0x94, 0x64, 0xD3, 0x50, 0xFF, 0xA3, 0x8F, 0xF7, 0xC1,
0xE5, 0x56, 0x77, 0x19, 0xC5, 0x53, 0x5E, 0xE8, 0x1F, 0x15, 0xA1, 0xD5, 0x78, 0x5A, 0x40, 0xEA, 0x8F, 0x18, 0x5E, 0x0B,
0x68, 0x35, 0x5E, 0x2C, 0x20, 0x45, 0x2A, 0xA6, 0xD7, 0x00, 0xEC, 0xA1, 0x92, 0x08, 0xDD, 0x3D, 0x10, 0x31, 0x1D, 0x08,
0xF4, 0x6A, 0x13, 0xAD, 0xC6, 0x0F, 0xB6, 0x18, 0x46, 0x77, 0x4A, 0xF5, 0xCE, 0x35, 0xDC, 0x29, 0x45, 0x9D, 0x99, 0xF1,
0x9D, 0x3B, 0xA5, 0x6C, 0xB6, 0x4C, 0x5A, 0x80, 0xA2, 0x39, 0x53, 0x1A, 0xE6, 0x87, 0xDD, 0x29, 0xF5, 0x3F, 0x07, 0x74,
0x96, 0x06, 0xF9, 0x05, 0xFF, 0x4E, 0x29, 0x5A, 0xCD, 0xA7, 0x05, 0x28, 0x9A, 0x33, 0xA5, 0x61, 0x7E, 0xD8, 0x9D, 0x52,
0x6A, 0x2E, 0xD6, 0xC4, 0x4E, 0x29, 0xAF, 0xD6, 0xF3, 0xFF, 0xB4, 0x00, 0x45, 0x73, 0xA6, 0x34, 0xCC, 0xFF, 0x3A, 0x77,
0x4A, 0x39, 0x1C, 0x0D, 0xB4, 0xE5, 0x74, 0xF5, 0x4F, 0xA6, 0xC6, 0x6A, 0xA6, 0x8E, 0x90, 0xC1, 0x14, 0xCB, 0x84, 0xD0,
0xED, 0xAF, 0x76, 0x43, 0xE9, 0x47, 0x6D, 0x5E, 0xEC, 0x44, 0x6C, 0x48, 0x48, 0x48, 0x29, 0x2A, 0x5F, 0xCC, 0xBA, 0x8C,
0x44, 0x29, 0x97, 0x69, 0x94, 0x7C, 0xEC, 0xB9, 0xAE, 0x7E, 0x7C, 0xBC, 0x53, 0x33, 0xCA, 0xE8, 0x05, 0x83, 0x9E, 0xA3,
0x38, 0x33, 0x38, 0xEB, 0xD0, 0x40, 0xD9, 0x71, 0xBD, 0x17, 0x9F, 0x03, 0x1B, 0xB8, 0x57, 0xEA, 0x96, 0x4B, 0x3B, 0xA5,
0x9A, 0x12, 0x3D, 0xF9, 0xF8, 0x80, 0x58, 0xFD, 0xB3, 0x49, 0x0E, 0xCD, 0xBD, 0x23, 0x39, 0x98, 0xE3, 0x19, 0xD4, 0xB7,
0x72, 0x78, 0xFF, 0xBC, 0x04, 0xFF, 0x75, 0xC8, 0xE0, 0xC5, 0x47, 0xCB, 0x86, 0xC4, 0x26, 0x6D, 0x51, 0x55, 0x61, 0xD7,
0x5C, 0xDA, 0x29, 0xD5, 0xEA, 0x82, 0x7C, 0x54, 0xC9, 0xD5, 0x63, 0xC7, 0x64, 0x68, 0xD2, 0xD3, 0x47, 0x40, 0x92, 0xC9,
0x00, 0x8D, 0xCD, 0x2D, 0x70, 0xF1, 0xE2, 0x05, 0x38, 0xF9, 0xF9, 0x19, 0x2F, 0x3E, 0xBD, 0x36, 0xC4, 0xCB, 0x50, 0x84,
0x1A, 0x29, 0x72, 0xBB, 0x7C, 0xE9, 0x40, 0xE8, 0xD5, 0x26, 0x5A, 0xB9, 0x1F, 0x0A, 0x31, 0x25, 0xC4, 0xD6, 0xD3, 0x86,
0x31, 0xFB, 0x07, 0x5B, 0x4C, 0xAF, 0x67, 0x22, 0xDD, 0xCB, 0x8F, 0xEC, 0x41, 0x41, 0x15, 0xF1, 0x11, 0xEF, 0xE5, 0xC7,
0xDC, 0xCB, 0xCF, 0x8E, 0xF6, 0xE5, 0x46, 0xBA, 0x97, 0x3F, 0x20, 0x37, 0xD2, 0x3D, 0xF5, 0xA8, 0x08, 0x7B, 0xB6, 0x67,
0xFB, 0x5E, 0x1C, 0x8F, 0x78, 0x2F, 0x3F, 0xE6, 0xA2, 0xA0, 0x0E, 0xB8, 0xDD, 0x20, 0x06, 0xBC, 0x97, 0x1F, 0xAB, 0x77,
0xD1, 0x8A, 0xBC, 0xBA, 0x88, 0xF4, 0x41, 0x5F, 0xF6, 0xE3, 0x0F, 0x24, 0x77, 0x20, 0x7B, 0xF9, 0x67, 0x95, 0xD5, 0x4E,
0xC6, 0x7B, 0x2C, 0x0F, 0xFB, 0xE5, 0xD9, 0x38, 0x7A, 0xB6, 0x62, 0x79, 0x09, 0x4B, 0x9A, 0x56, 0x5C, 0xF7, 0xCE, 0xE3,
0x96, 0xC3, 0x6A, 0x58, 0x48, 0x16, 0x6D, 0xE3, 0x93, 0xF1, 0xFA, 0xF2, 0x18, 0xD0, 0xCB, 0xFB, 0x0C, 0x87, 0x03, 0xFC,
0x12, 0x07, 0x56, 0x89, 0xDF, 0x63, 0x5D, 0xF9, 0xFD, 0x2C, 0x6C, 0x6E, 0xEA, 0x26, 0xE7, 0x64, 0xCE, 0x34, 0x79, 0x8C,
0x63, 0x2E, 0xC3, 0x5C, 0x8E, 0xB9, 0x8C, 0x61, 0x8F, 0xDD, 0xBB, 0xAE, 0xE6, 0x29, 0x7D, 0xD8, 0xDC, 0x9F, 0xAD, 0x29,
0x98, 0xAC, 0x00, 0xCB, 0xC3, 0x6A, 0x36, 0x7E, 0x39, 0x2B, 0x7E, 0x79, 0x9A, 0x30, 0xC7, 0x7B, 0x96, 0xAF, 0xFB, 0xC5,
0xFA, 0xC2, 0xB0, 0xB9, 0x51, 0xA2, 0x44, 0x89, 0x12, 0x25, 0x4A, 0x94, 0x28, 0x51, 0xA2, 0x0C, 0x25, 0x00, 0x7F, 0x07,
0x97, 0xBC, 0x86, 0xE6, 0x93, 0xB8, 0x71, 0x53, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,

};

static LRESULT CALLBACK static_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPTREELIST_DATA pData = (LPTREELIST_DATA)GetPropW(hWnd, L"proc");
    if (!pData)
        return DefWindowProcW(hWnd, message, wParam, lParam);
    if (pData->pThis->GetTreeWindow() == hWnd)
    {
        CWndBase* pBase = pData->pThis;
        return pBase->WndProc(pData->oldProcTree, hWnd, message, wParam, lParam);
    }
    if (pData->hHead == hWnd)
    {
        return pData->pThis->WndProc_header(pData->oldProcHeader, hWnd, message, wParam, lParam);
    }
    switch (message)
    {
    //case WM_PAINT:
        //return pData->pThis->OnPaintHeader(pData->oldProcTree, hWnd, message, wParam, lParam);
    case WM_SIZE:
    {
        //ShowWindow(pData->hHead, SW_HIDE);
        pData->hHead;
        int cx = LOWORD(lParam);
        int cy = HIWORD(lParam);
        pData->width = cx;
        pData->height = cy;
        pData->rcHead.right = pData->width;
        pData->rcHead.bottom = pData->nHeadHeight;
        if (pData->hCanvas)
        {
            _canvas_resize(pData->hCanvas, pData->width, pData->height);
        }

        SCROLLINFO si = { 0 };
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL | SIF_POS;
        BOOL bRet = GetScrollInfo(pData->pThis->GetTreeWindow(), SB_HORZ, &si);
        if (bRet && pData->rcHead.left != -si.nPos)
        {
            pData->rcHead.left = -si.nPos;
            pData->rcHead.right = pData->width + si.nPos;

            //InvalidateRect(m_data->hHead, 0, 0);
        }
        pData->rcHead.right = pData->width + si.nPos;
        MoveWindow(pData->pThis->GetTreeWindow(), 0, pData->nHeadHeight, cx, cy - pData->nHeadHeight, TRUE);
        MoveWindow(pData->hHead, pData->rcHead.left, pData->rcHead.top, pData->rcHead.right, pData->rcHead.bottom, TRUE);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH hbr = GetStockBrush(WHITE_BRUSH);
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, hbr);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (pData->hHead == hdr->hwndFrom)
        {
            return pData->pThis->OnHeadNotify(pData->oldProcStatic, hWnd, message, wParam, lParam);
        }
        else if (pData->pThis->GetTreeWindow() == hdr->hwndFrom)
        {
            return pData->pThis->OnTreeNotify(pData->oldProcStatic, hWnd, message, wParam, lParam);
        }
        return CallWindowProcW(pData->oldProcStatic, hWnd, message, wParam, lParam);
    }
    default:
        return CallWindowProcW(pData->oldProcStatic, hWnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CTreeList::WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_DESTROY:

        break;
    case WM_SIZE:
        return __OnSize(oldProc, hWnd, message, wParam, lParam);
    case WM_MOUSEWHEEL:
    case WM_VSCROLL:
    {
        LRESULT ret = CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        ::InvalidateRect(hWnd, 0, 0);
        return ret;
    }
    case WM_HSCROLL:
        return OnHScroll(oldProc, hWnd, message, wParam, lParam);
    case WM_MOUSEMOVE:
        return __OnMouseMove(oldProc, hWnd, message, wParam, lParam);
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDOWN:
        return __OnMouseLDown(oldProc, hWnd, message, wParam, lParam);
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if (message == WM_LBUTTONUP && m_evt.pLUp)
            m_evt.pLUp(x, y);
        if (message == WM_RBUTTONUP && m_evt.pRUp)
            m_evt.pRUp(x, y);
        return __OnMouseLUp(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONDBLCLK:
    {
        int size = m_header.size();
        if (!size) return 0;

        int x = GET_X_LPARAM(lParam);
        int max = m_header.at(size - 1).rc.right;
        if (x > max)
            return 0;
        if (m_evt.pDblclk)
        {
            int tmp = 0;
            if (m_evt.pDblclk(m_data->index))
                return 0;
        }
        break;
    }
    case SBM_SETSCROLLINFO:
    {
        LPSCROLLINFO pos = (LPSCROLLINFO)lParam;
        break;
    }
    case TVM_SETEXTENDEDSTYLE:
        OnStyleChenged(true, (DWORD)lParam);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    case WM_STYLECHANGING:
    {
        if (wParam == GWL_STYLE)
        {
            LPSTYLESTRUCT s = ((LPSTYLESTRUCT)lParam);
            OnStyleChenged(false, s->styleNew);
            if (((s->styleNew & TVS_LINESATROOT) != 0) || ((s->styleNew & TVS_HASBUTTONS) != 0))
            {
                s->styleNew &= ~TVS_LINESATROOT;
                s->styleNew &= ~TVS_HASBUTTONS;
            }
        }
        
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_NCMOUSELEAVE:
    case WM_MOUSELEAVE:
        return __OnMouseLeave(oldProc, hWnd, message, wParam, lParam);
    case WM_PAINT:
        return __OnPaint(oldProc, hWnd, message, wParam, lParam);
    case WM_KILLFOCUS:
    {
        LPHTREELISTITEM data = (LPHTREELISTITEM)GetItemNum(m_data->index);
        if (data)
        {
            if (GetUIItem(data) & STATE::FOCUS)
            {
                SetUIItem(m_data->index, data, STATE::FOCUS, true, true);
            }
        }
        break;
    }
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        break;
    }
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    return 0;
}

LRESULT CTreeList::WndProc_header(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        return OnPaintHeader(oldProc, hWnd, message, wParam, lParam);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    case WM_SIZE:
    {
        _canvas_resize(m_data->hCanvas_Head, LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_MOUSEMOVE:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if ((wParam & MK_LBUTTON) == 0)
        {
            int len = m_header.size();
            for (int i = 0; i < len; i++)
            {
                TREELIST_COL_DATA& data = m_header[i];
                if (x > data.rc.left && x < data.rc.right)
                {
                    if (m_headerIndexHot != i)
                    {
                        m_headerIndexHot = i;
                        ::InvalidateRect(hWnd, 0, 0);
                    }
                    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
                }
            }
            if (m_headerIndexHot != -1)
                ::InvalidateRect(hWnd, 0, 0);
            m_headerIndexHot = -1;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONDOWN:
    {
        if (m_headerIndexHot != -1)
        {
            m_headerIsDown = true;
            ::InvalidateRect(hWnd, 0, 0);
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_CAPTURECHANGED:
    case WM_LBUTTONUP:
    {
        if (m_headerIndexHot != -1)
            ::InvalidateRect(hWnd, 0, 0);
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        int len = m_header.size();
        if (len > 0)
        {
            TREELIST_COL_DATA& first = m_header[0];
            TREELIST_COL_DATA& last = m_header[m_header.size() - 1];
            if (y < 0 || y > first.rc.bottom - first.rc.top || x < first.rc.left || x > last.rc.right)
            {
                m_headerIndexHot = -1;
                ::InvalidateRect(hWnd, 0, 0);
            }
        }
        m_headerIsDown = false;
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_MOUSELEAVE:
    {
        m_headerIndexHot = -1;
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_DESTROY:
    {
        break;
    }
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        break;
    }
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}


LRESULT CTreeList::OnHeadNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR hdr = (LPNMHDR)lParam;
    switch (hdr->code)
    {
    case HDN_TRACK:
    case HDN_BEGINTRACK:
    {
        //LPNMHEADERW head = (LPNMHEADERW)lParam;
        //wstr str;
        //str.Format(L"msg=%d, iItem=%d, iButton=%d\n", hdr->code, head->iItem, head->iButton);
        //OutputDebugStringW(str.c_str());
        break;
    }
    case HDN_FILTERCHANGE:
    case HDN_GETDISPINFO:
    {
        LPNMHEADERW head = (LPNMHEADERW)lParam;
        wstr str;
        str.Format(L"msg=%s, iItem=%d, iButton=%d\n", hdr->code == HDN_GETDISPINFO ? L"HDN_GETDISPINFO" : L"HDN_FILTERCHANGE", head->iItem, head->iButton);
        //OutputDebugStringW(str.c_str());
        break;
    }
    case HDN_ITEMCHANGING:
    {
        LPNMHEADERW head = (LPNMHEADERW)lParam;
        if (((head->pitem->mask & HDI_WIDTH) == HDI_WIDTH) && head->pitem->cxy < 50)
            head->pitem->cxy = 50;
        //wstr str;
        //str.dbg(L"即将改变, iItem=%d, iButton=%d, pos = %d\n", head->iItem, head->iButton, head->pitem->cxy);
        return FALSE;
        //返回FALSE允许更改, 或者返回TRUE阻止更改。
    }
    case HDN_ITEMCHANGED:
    {
        LPNMHEADERW head = (LPNMHEADERW)lParam;
        if (((head->pitem->mask & HDI_WIDTH) == HDI_WIDTH))
            __modify_col_width(head->iItem, head->pitem->cxy);
        //wstr str;
        //str.dbg(L"改变完毕, iItem=%d, iButton=%d, pos = %d\n", head->iItem, head->iButton, head->pitem->cxy);
        break;
    }
    case HDN_BEGINDRAG:
    {
        //LPNMHEADERW head = (LPNMHEADERW)lParam;
        //wstr::dbg(L"开始拖动, 拖动的索引 = %d, iButton = %d\n", head->iItem, head->iButton);
        return 1;
    }
    //case HDN_ENDDRAG:
    //{
    //    LPNMHEADERW head = (LPNMHEADERW)lParam;
    //    wstr::dbg(L"结束拖动, 拖动的索引 = %d, iButton = %d\n", head->iItem, head->iButton);
    //    //
    //    int count = Header_GetItemCount(hdr->hwndFrom);
    //    HDITEMW hdi = { 0 };
    //    hdi.mask = HDI_ORDER;
    //    int* mmm = new int[count];
    //    Header_GetOrderArray(hdr->hwndFrom, count, mmm);
    //    for (int i = 0; i < count; i++)
    //    {
    //        Header_GetItem(hdr->hwndFrom, i, &hdi);
    //        wstr::dbg(L"第%d个成员的位置 = %d, mmm = %d\n", i, hdi.iOrder, mmm[i]);
    //    }
    //    return 0;
    //}
    case NM_RELEASEDCAPTURE:

        break;
    case NM_CUSTOMDRAW:
    {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

        //_str::dbg(" --------------------------------- 表头绘画 NM_CUSTOMDRAW\n");

        static HBRUSH hbrWhite = GetStockBrush(WHITE_BRUSH);
        LPNMTBCUSTOMDRAW dw = (LPNMTBCUSTOMDRAW)lParam;
        switch (dw->nmcd.dwDrawStage)
        {
        case CDDS_PREERASE:         // 在擦除周期开始之前
            //_str::dbg("lParam = %d, 擦除周期开始之前\n", lParam);
            break;
        case CDDS_POSTERASE:        // 擦除周期完成后
            //_str::dbg("lParam = %d, 擦除周期完成后\n", lParam);
            break;
        case CDDS_PREPAINT:         // 在绘画周期开始之前
            //_str::dbg("lParam = %d, 在绘画周期开始之前\n", lParam);
            FillRect(dw->nmcd.hdc, &dw->nmcd.rc, hbrWhite);
            return CDRF_DODEFAULT;
            return CDRF_NOTIFYPOSTERASE;
        case CDDS_POSTPAINT:        // 绘画周期完成后
            //_str::dbg("lParam = %d, 绘画周期完成后\n", lParam);
            break;
        case CDDS_ITEM:             // 指示dwItemSpec， uItemState和lItemlParam成员有效
        case CDDS_ITEMPREERASE:     // 删除项目之前
        case CDDS_ITEMPOSTERASE:    // 删除项目后
        case CDDS_ITEMPREPAINT:     // 在绘制项目之前
            //_str::dbg("lParam = %d, 在绘制项目之前\n", lParam);
            break;
        case CDDS_ITEMPOSTPAINT:    // 绘制项目后
            //_str::dbg("lParam = %d, 绘制项目后\n", lParam);
            break;
        case CDDS_SUBITEM:          // 如果正在绘制子项目，请与CDDS_ITEMPREPAINT或CDDS_ITEMPOSTPAINT组合的标志。如果这只会设置CDRF_NOTIFYITEMDRAW从CDDS_PREPAINT返回
            break;
        default:
            return CDRF_DODEFAULT;
        }
        //dw->nStringBkMode = TRANSPARENT;
        //dw->nHLStringBkMode = TRANSPARENT;
        //dw->hbrMonoDither = GetStockBrush(WHITE_BRUSH);
        //dw->clrText = 255;

        //return TBCDRF_BLENDICON;
        return CDRF_DODEFAULT;
        break;
    }
    default:
    {
        break;
    }
    }
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

LRESULT CTreeList::OnTreeNotify(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR hdr = (LPNMHDR)lParam;
    switch (hdr->code)
    {
    case TVN_DELETEITEM:
    {
        LPNMTREEVIEWW tv = (LPNMTREEVIEWW)lParam;
        LPHTREELISTITEM data = (LPHTREELISTITEM)tv->itemOld.lParam;
        if (m_evt.pDelete)
            m_evt.pDelete(tv->itemOld.hItem, data);
        if (data)delete data;

        if (m_hItems)
        {
            delete[] m_hItems;
            m_hItems = 0;
            m_hItemsCount = 0;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case NM_CLICK:
    case NM_RCLICK:
    case TVN_SELCHANGED:
    {
        LPNMTREEVIEWW item = (LPNMTREEVIEWW)lParam;
        LPHTREELISTITEM data = (LPHTREELISTITEM)item->itemNew.lParam;
        HTREEITEM hItem = item->itemNew.hItem;

        if (hdr->code == TVN_SELCHANGED && GetUIItem(data) == 0)
            return 0;
        if (hdr->code == NM_RCLICK || hdr->code == NM_CLICK)
        {
            TVHITTESTINFO ht;
            GetCursorPos(&ht.pt);
            ScreenToClient(hdr->hwndFrom, &ht.pt);
            hItem = TreeView_HitTest(m_data->hTree, &ht);
            if (!hItem) break;
            
            data = (LPHTREELISTITEM)GetItemNum(hItem);
        }
        //wstr::dbg(L"项目被单击, 坐标项=%s, 按下项=%s\n", 
        //    hItem?((LPHTREELISTITEM)GetItemNum(hItem))->draw->title.c_str():L"null",
        //    m_data->indexDown?((LPHTREELISTITEM)GetItemNum(m_data->indexDown))->draw->title.c_str(): L"null");
        if (hItem == m_data->indexDown) // 放开的表项是点击的表项, 增加一个焦点状态
        {
            if (m_data->index != m_data->indexDown)     // 按下项目和选中项目不一样, 去除选中项目按下状态
                SetUIItem(m_data->index, STATE::DOWN, true, true);  // 去除上一次选中表项的按下状态
            if (!(m_data->indexColIcon != -1 && data->draw[m_data->indexColIcon].type == TLVTYPE_STATIC))
            {
                SetUIItem(hItem, data, STATE::FOCUS, false, true);      // 增加按下表项焦点状态
            }
            m_data->index = hItem;      // 修改当前选中项

            if (m_isHotList)
            {
                // 获取选中项的下一项同级项目
                HTREEITEM hRoot = m_hotGroup[0].hItem;
                HTREEITEM hNext = TreeView_GetNextItem(m_data->hTree, hItem, TVGN_NEXT);
                HTREEITEM hItemTmp = hItem;
                while (!hNext && hItem != hRoot && hItemTmp)
                {
                    hItemTmp = TreeView_GetParent(m_data->hTree, hItemTmp);
                    hNext = TreeView_GetNextItem(m_data->hTree, hItemTmp, TVGN_NEXT);
                }
                m_data->hNext = hNext;
            }

            __OnItemClick(hItem, data, m_data->indexCol, m_data->indexColIcon, hdr->code == NM_RCLICK);
        }
        else if (m_data->indexDown)     // 放开的不是点击的表项
        {
            // 原来的选中项, 按下时删除了焦点状态, 放开在加回去
            SetUIItem(m_data->index, STATE::FOCUS, false, true);

            // 按下的表项, 按下时增加了按下状态, 放开在去除
            SetUIItem(hItem, data, STATE::DOWN, true, true);
        }
        m_data->indexDown = 0;
        break;
    }
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

bool CTreeList::OnStyleChenged(bool isEx, DWORD style)
{
    if (isEx)
    {
        if (style & TVS_EX_PARTIALCHECKBOXES)
            m_data->CheckType = 2;
        else if (style & TVS_EX_EXCLUSIONCHECKBOXES)
            m_data->CheckType = 3;
        else if (style & TVS_EX_DIMMEDCHECKBOXES)
            m_data->CheckType = 4;
    }
    else
    {
        m_data->isExpand = ((style & TVS_LINESATROOT) != 0) || ((style & TVS_HASBUTTONS) != 0);  // 有加减框
        m_data->isTips = ((style & TVS_NOTOOLTIPS) == 0);     // 没有这个标志表示有提示工具
        if (style & TVS_CHECKBOXES)
            m_data->CheckType = 1;
    }
    return 0;
}


LRESULT CTreeList::IndexChanged(HTREEITEM hItem, LPHTREELISTITEM data, int indexCol, int indexColIcon,  int x, int y)
{
    //wstr::dbg(L"表项被改变, 列索引=%d,图标索引= %d, 项目=%s\n", indexCol, indexColIcon, data->draw->title.c_str());
    if (!data || indexCol == -1 || indexCol >= data->count)return 0;
    if (m_evt.pChenged)
    {
        if (m_evt.pChenged(hItem, data, indexCol, indexColIcon))
            return 0;
    }

    HTREELISTITEM_SUBDATA& subData = data->draw[indexCol];
    LPEX_FONT hFont = subData.hFont ? subData.hFont : m_data->hFont;
    wstr& str = subData.title;
    TREELIST_COL_DATA& header = m_header[indexCol];
    float width = (float)(subData.width && subData.isAbsWidth ? subData.width : header.width);
    if (subData.rcText.right + 20 > width)
    {
        m_tips.SetTipsText(m_data->hTree, str.c_str());
    }
    else
    {
        m_tips.SetTipsText(m_data->hTree, 0);
        m_tips.hide();
    }
    //_canvas_calctextsize(0, hFont, str.c_str(), str.size(), );
    return LRESULT();
}

int CTreeList::enumItem()
{
    m_hotGroup.resize(m_data->count);
    int index = 0;
    HTREEITEM hItem = TreeView_GetRoot(m_data->hTree);
    bool done = false;
    while (hItem)
    {
        HOT_GROUP_ARY& data = m_hotGroup[index];
        data.hItem = hItem;
        data.data = (LPHTREELISTITEM)GetItemNum(hItem);
        data.data->index = index++;
        HTREEITEM hItemTmp = hItem;
        hItem = TreeView_GetNextItem(m_data->hTree, hItem, TVGN_CHILD);    // 取第一个子
        if (hItem == 0)
        {
            hItem = TreeView_GetNextItem(m_data->hTree, hItemTmp, TVGN_NEXT);    // 取子的兄弟
            while (hItem == 0)
            {
                HTREEITEM hParent = TreeView_GetNextItem(m_data->hTree, hItemTmp, TVGN_PARENT);    // 退回上一级
                if (hParent == 0)
                {
                    done = true;
                    break;
                }
                hItemTmp = hParent;
                hItem = TreeView_GetNextItem(m_data->hTree, hParent, TVGN_NEXT);    // 取父的兄弟
            }
        }
        if (done)break;

        
    }
    return 0;
}

LRESULT CTreeList::__OnSize(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int size = m_header.size();
    if (size == 0) return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

    int max = m_header.at(size - 1).rc.right;    // 表头最大位置
    int width = m_data->width;
    BOOL isShow = FALSE;
    if (max > width)    // 需要横向滚动条
    {
        isShow = TRUE;
        width += 1;
    }
    else
    {
        width = max + 1;
    }
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nMin = 0;
    si.nMax = max;
    si.nPage = width;
    ShowScrollBar(m_data->hTree, SB_HORZ, isShow);
    SetScrollInfo(m_data->hTree, SB_HORZ, &si, TRUE);
    m_tips.Send(TTM_SETMAXTIPWIDTH, 0, width);
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

}

LRESULT CTreeList::__OnMouseLDown(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int len = 0;
    LPHTREELISTITEM data;
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    int indexCol;
    int indexColIcon;
    int scrollPos = GetScrollPos(m_data->hTree, SB_HORZ);
    HTREEITEM hItem = MouseIsItem(x, y, wParam, &data, &indexCol, &indexColIcon, scrollPos, true);
    //_str::dbg("按下项目 = 0x%p, 列 = %d\n", hItem, indexCol);
    m_data->indexDown = hItem;  // 左键按下表项
    if (hItem && hItem == m_data->index)    // 当前按下的是选中的表项
    {
        int state = GetUIItem(data);   // 查看有没有焦点状态
        // 如果没有焦点状态, 表示上一次选中后鼠标点击了外面, 失去焦点, 再次点击需要加上
        if (!(state & STATE::FOCUS))
        {
            SetUIItem(m_data->index, data, STATE::FOCUS, false, true);
        }
        m_data->indexCol = indexCol;
        m_data->indexColIcon = indexColIcon;
        
        //wstr::dbg(L"表项被按下, 与上一次点击表项相同, 列索引=%d, 图标=%d\n", indexCol, indexColIcon);
        //m_data->indexDown = hItem;
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    // 当前点击项目不等于旧项目, 去除旧项目焦点状态
    if (m_data->index && m_data->index != hItem)
    {
        LPHTREELISTITEM dataOld = (LPHTREELISTITEM)GetItemNum(m_data->index);
        SetUIItem(m_data->index, dataOld, STATE::FOCUS, true, true);
    }
    //wstr::dbg(L"表项被按下, 列索引=%d, 图标=%d\n", indexCol, indexColIcon);
    if (!hItem) return 0;   // 当前鼠标没有选择任何表项
    if (data->draw[indexCol].type == TLVTYPE_STATIC)    //选中列是静态文本, 并且点击的不是系统图标
    {
        if (indexColIcon == -1)
            return 0;
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    m_data->indexCol = indexCol;
    m_data->indexColIcon = indexColIcon;
    SetUIItem(hItem, data, STATE::DOWN, false, true);
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

LRESULT CTreeList::__OnMouseLUp(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_data->indexDown && m_data->indexCol != -1)
    {
        //  && m_data->indexHot == m_data->indexDown 
        if (m_data->indexHot == m_data->indexDown || !m_data->indexHot)
        {
            LPHTREELISTITEM data = (LPHTREELISTITEM)GetItemNum(m_data->indexDown);
            if (data && data->draw[m_data->indexCol].type == TLVTYPE_STATIC)
            {
                __OnItemClick(m_data->indexHot, data, m_data->indexCol, m_data->indexColIcon, message == WM_RBUTTONUP);
                m_data->indexDown = 0;
                return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
            }
        }
    }
    

    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    int indexCol;
    int indexColIcon;
    int scrollPos = GetScrollPos(m_data->hTree, SB_HORZ);
    LPHTREELISTITEM data = 0;
    HTREEITEM hItem = MouseIsItem(x, y, wParam, &data, &indexCol, &indexColIcon, scrollPos, true);
    if (hItem && data && hItem == m_data->index
        && indexCol == m_data->indexCol
        && hItem == m_data->indexDown
        && hItem == m_data->indexHot)    // 放开鼠标时还在同一列同一个表项上
    {
        __OnItemClick(m_data->indexHot, data, m_data->indexCol, m_data->indexColIcon, message == WM_RBUTTONUP);
        return 0;
    }
    //wstr::dbg(L"表项被放开, 按下项目句柄=0x%08X, indexCol = 0x%08X, indexColIcon = 0x%08X, indexHot = 0x%08X, 是否右键 = %d, msg = %d\n",
    //    m_data->indexDown, m_data->indexCol, m_data->indexColIcon, m_data->indexHot, (message == WM_RBUTTONUP), message);
    
    if (m_data->indexDown)
    {
        int state = GetUIItem(m_data->indexDown);
        if (m_data->index)     // 移到外面的表项是当前选中项
            SetUIItem(m_data->index, STATE::DOWN, false, true);     // 加入热点状态
        
        if (m_data->indexCol == -1) // 鼠标移动到了列表外面
        {
            // 走到这里表示按下鼠标后移动到了其他位置, 没有选择表项, 需要把按下状态去除
            if (m_data->indexDown != m_data->index)
                SetUIItem(m_data->indexDown, STATE::DOWN, true, true);
        }
        else
        {
            // 鼠标移到到了其他表项上
            if (m_data->indexDown != m_data->index)
                SetUIItem(m_data->indexDown, STATE::DOWN, true, true);
        }
        InvalidateItem(m_data->indexDown);  // 重画选中项
        m_data->indexDown = 0;
    }
    //if (m_data->index)
    //{
    //    int state = GetUIItem(m_data->index);
    //    if (state & STATE::DOWN)
    //    {
    //        // 走到这里表示按下鼠标后移动到了其他位置, 没有选择表项, 需要把按下状态去除
    //        SetUIItem(m_data->index, STATE::DOWN, true, false);
    //        InvalidateItem(m_data->index);  // 加入重画区域
    //    }

    //}
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

void CTreeList::__OnItemClick(HTREEITEM hItem, LPHTREELISTITEM data, int indexCol, int indexColIcon, bool isRightBtn)
{
    wstr::dbg(L"项目被单击 0x%08X, 列索引=%d, 图标索引=%d, 标题=%s, 是否右键单击 = %d\n", hItem, indexCol, indexColIcon, data->draw->title.c_str(), isRightBtn);
    if (m_evt.pClick)
    {
        if (m_evt.pClick(hItem, data, indexCol, indexColIcon, isRightBtn))
            return;
    }

    if (isRightBtn)
    {
        return;
    }

    if (indexColIcon == TREESYSICON_EXPAND)
    {
        TreeView_Expand(m_data->hTree, hItem, TVE_TOGGLE);
        //TVITEMEXW item;
        //item.hItem = hItem;
        //item.mask = TVIF_STATE | TVIF_PARAM | TVIF_CHILDREN;
        //TreeView_GetItem(m_data->hTree, &item);
        //data = (LPHTREELISTITEM)item.lParam;
        //if (!data)return;
        //if (item.cChildren) // 有子项, 并且
        //{
        //    if ((item.state & TVIS_EXPANDED))    // 已展开
        //}
    }
}

LRESULT CTreeList::__OnMouseMove(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);
    LPHTREELISTITEM data;
    int indexCol;
    int indexColIcon;
    int scrollPos = GetScrollPos(m_data->hTree, SB_HORZ);
    HTREEITEM hItem = MouseIsItem(x, y, wParam, &data, &indexCol, &indexColIcon, scrollPos, false);
    if (!hItem)     // 鼠标移动的区域不是表项矩形内
    {
        if(indexCol == m_data->indexCol)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        return 0;
    }

    HTREEITEM hItemOld = m_data->indexHot;      // 原来的热点项, 变量用于判断表项或者列是否改变
    int indexColOld = m_data->indexCol;         // 原来的列索引
    int indexColIconOld = m_data->indexColIcon; // 原来的图标索引


    if (indexColOld == indexCol && hItem == hItemOld && indexColIconOld == indexColIcon)
        return 0;   // 当前鼠标还是在上一次的表项内, 不往下处理

    //wstr::dbg(L"x=%d, 旧= %d,新= %d, 列索引=%d\n", x, indexColIconOld, indexColIcon, indexCol);

    if (data->draw[indexCol].type == TLVTYPE_STATIC)    //选中列是静态文本
    {

    }

    if (hItem != hItemOld)
    {
        // 先清除旧的热点项
        if (hItemOld)
        {
            // 去除热点状态
            LPHTREELISTITEM dataOld = (LPHTREELISTITEM)GetItemNum(hItemOld);
            SetUIItem(hItemOld, dataOld, STATE::HOT, true, false);
            SetUIItem(hItemOld, dataOld, STATE::HOVER, true, false);
            SetStateIcon(dataOld, indexColIconOld, STATE::HOT, true);  // 去除旧项目热点
            InvalidateItem(hItemOld);
        }
        m_data->indexHot = hItem;       // 记录热点项
    }
    if (indexCol != indexColOld)
        m_data->indexCol = indexCol;    // 记录鼠标所在列索引

    if (indexColIconOld != indexColIcon) // 记录鼠标所在系统图标位置
    {
        m_data->indexColIcon = indexColIcon;
        SetStateIcon(data, indexColIconOld, STATE::HOT, true);          // 去除旧项目热点
        SetStateIcon(data, m_data->indexColIcon, STATE::HOT, false);    // 添加新项目热点
        InvalidateItem(hItem);
        InvalidateItem(hItemOld);
        //wstr::dbg(L"indexColIconOld != indexColOld, col=%d, colIcon=%d, str=%s\n", indexCol, indexColIcon, data->draw[0].title.c_str());
    }


    if (m_isHotList)
    {
        if (hItem != hItemOld)
        {
            HTREEITEM hRoot = TreeView_GetRoot(m_data->hTree);
            HTREEITEM hParent = hItem;
            if (hRoot != hParent && !(TreeView_GetItemState(m_data->hTree, hParent, 0) & TVIS_EXPANDED))   // 未展开, 取父项
            {
                hParent = TreeView_GetParent(m_data->hTree, hItem);
                if (!hParent)
                    hParent = hItem;
            }
            if (hParent != m_data->hotParent)
            {
                ModifyParentHot(m_data->hotParent, hItemOld, true);
                m_data->hotParent = hParent;
            }

            ModifyParentHot(m_data->hotParent, hItem, false);
            ::InvalidateRect(m_data->hTree, 0, 0);

        }
    }
    else
    {
        if (data->draw[indexCol].type != TLVTYPE_STATIC)    //选中列是不静态文本
        {
            SetUIItem(m_data->indexHot, data, STATE::HOT, false, true);  // 加入热点状态
        }
    }

    


    TRACKMOUSEEVENT et;
    et.cbSize = sizeof(et);
    et.dwFlags = TME_LEAVE | TME_HOVER;
    et.hwndTrack = hWnd;
    et.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&et);

    //InvalidateRect(hWnd, 0, 0);
    if (hItem != hItemOld || indexCol != indexColOld || indexColIconOld != indexColIcon)
    {
        IndexChanged(hItem, data, indexCol, indexColIcon, x, y);
    }
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}
LRESULT CTreeList::__OnMouseLeave(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (m_isHotList)
    {
        ModifyParentHot(m_data->hotParent, m_data->indexHot, true);
        m_data->hotParent = 0;
        LPHTREELISTITEM data = (LPHTREELISTITEM)GetItemNum(m_data->indexHot);
        SetStateIcon(data, m_data->indexColIcon, STATE::HOT, true);
        m_data->indexColIcon = -1;
        ::InvalidateRect(m_data->hTree, 0, 0);

    }
    else if (m_data->indexHot)
    {
        LPHTREELISTITEM data = (LPHTREELISTITEM)GetItemNum(m_data->indexHot);
        if (GetUIItem(data) & STATE::HOT)
        {
            SetUIItem(m_data->indexHot, data, STATE::HOT, true, true);
        }
    }
    m_data->isHover = false;
    m_data->indexHot = 0;
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

LRESULT CTreeList::__OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //InvalidateRect(hWnd, 0, 0);
    RECT rc;
    ::GetClientRect(hWnd, &rc);
    __create_d2d_resouce();
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    LPCANVAS& hCanvas = m_data->hCanvas;
    _canvas_begindraw(hCanvas);

    rc.right -= rc.left;
    rc.bottom -= rc.top;
    _canvas_clear(hCanvas, ARGB_D2D(255, 255, 255, 255));
    
    HTREEITEM hItem = TreeView_GetNextItem(hWnd, 0, TVGN_FIRSTVISIBLE);
    int visibleCount = TreeView_GetVisibleCount(hWnd) + 1;
    int scrollPos = GetScrollPos(hWnd, SB_HORZ);
    RECT rcParent = { 0 };
    RECT_F rcf;
    for (int i = 0; i < visibleCount; i++)
    {
        if (hItem == 0)
            break;
        if (rcParent.right == 0)
        {
            TreeView_GetItemRect(m_data->hTree, hItem, &rcParent, 0);
            rcf(rcParent);
        }
        DrawItem(hCanvas, hItem, i, scrollPos, rcf);
        hItem = TreeView_GetNextItem(hWnd, hItem, TVGN_NEXTVISIBLE);
    }

    
    //_canvas_clear(hCanvas, 0);
    //LPEX_IMAGE hIm[20] = { 0 };
    //hIm[0] = _img_create(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\radio1.png)");
    //hIm[1] = _img_create(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\radio2.png)");
    //hIm[2] = _img_create(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\radio3.png)");
    //hIm[3] = _img_create(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\radio4.png)");
    //hIm[4] = _img_create(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\radio5.png)");
    //hIm[5] = _img_create(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\radio6.png)");
    //hIm[6] = _img_create(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\radio7.png)");
    //hIm[7] = _img_create(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\radio8.png)");
    //hIm[10] = _img_create(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\img.png)");

    //_canvas_drawimage(hCanvas, hIm[0], 220 + 15 * 0, 0, 220 + 15 * 1, 15, 255);
    //_canvas_drawimage(hCanvas, hIm[1], 220 + 15 * 1, 0, 220 + 15 * 2, 15, 255);
    //_canvas_drawimage(hCanvas, hIm[2], 220 + 15 * 2, 0, 220 + 15 * 3, 15, 255);
    //_canvas_drawimage(hCanvas, hIm[3], 220 + 15 * 3, 0, 220 + 15 * 4, 15, 255);
    //_canvas_drawimage(hCanvas, hIm[4], 220 + 15 * 4, 0, 220 + 15 * 5, 15, 255);
    //_canvas_drawimage(hCanvas, hIm[5], 220 + 15 * 5, 0, 220 + 15 * 6, 15, 255);
    //_canvas_drawimage(hCanvas, hIm[6], 220 + 15 * 6, 0, 220 + 15 * 7, 15, 255);
    //_canvas_drawimage(hCanvas, hIm[7], 220 + 15 * 7, 0, 220 + 15 * 8, 15, 255);

    //_canvas_drawimage(hCanvas, hIm[10], 0, 0, 220, 16, 255);


    //SIZE_T size;
    //LPBYTE pImg = _img_save(hCanvas, size, 0, 0, 220 + 15 * 8, 16);
    //CFileRW fi;
    //fi.open(LR"(D:\cpp\SVN工作目录\JYVipTB\TreeList\src\test.png)", 4, -1);
    //fi.write(pImg, size);

    HDC hdc = _canvas_getdc(hCanvas);
    BitBlt(ps.hdc, 0, 0, rc.right, rc.bottom, hdc, 0, 0, SRCCOPY);

    _canvas_releasedc(hCanvas);
    _canvas_enddraw(hCanvas);

    //static int adddd;
    //_str::dbg("onPaint 次数 = %d, 脏区域 = { %d, %d, %d, %d }\n", ++adddd, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
    EndPaint(hWnd, &ps);
    //ValidateRect(hWnd, 0);
    
    return 0;
}

void CTreeList::DrawItem(LPCANVAS hCanvas, HTREEITEM hItem, int index, int scrollPos, const RECT_F& rcParent)
{
    if (!hCanvas)return;
    LPHTREELISTITEM data;
    RECT rc;
    TreeView_GetItemRect(m_data->hTree, hItem, &rc, 0);
    int colLen = m_header.size();
    int max = m_header.at(colLen - 1).rc.right;
    if (rc.right > max)
    {
        rc.right = max;
    }
    RECT_F rcf(rc);

    TVITEMEXW item;
    item.hItem = hItem;
    item.mask = TVIF_STATE | TVIF_PARAM | TVIF_CHILDREN | TVIF_EXPANDEDIMAGE;
    TreeView_GetItem(m_data->hTree, &item);
    data = (LPHTREELISTITEM)item.lParam;
    if (!data)return;
    FillItem(hCanvas, data, scrollPos, rcf);
    DrawItemLine(hCanvas, hItem, data, rcf, item);
    //wstr::dbg(L"绘制第%d个表项, 表项状态=%d\n", index, data->state);

    HDITEMW hdi = { 0 };
    hdi.mask = HDI_ORDER;
    int* indexs = new int[colLen];
    Header_GetOrderArray(m_data->hHead, colLen, indexs);

    for (int i = 0; i < colLen; i++)
    {
        int headIndex = indexs[i];
        TREELIST_COL_DATA& header = m_header[headIndex];
        HTREELISTITEM_SUBDATA& subData = data->draw[headIndex];
        
        // 设置绘画区域
        rcf.left = (float)(subData.left && subData.isAbsLeft ? subData.left : header.rc.left);
        rcf.right = rcf.left + (float)(subData.width && subData.isAbsWidth ? subData.width : header.width) - 10.0f;

        if (rcf.right > (float)max)
            rcf.right = (float)max - 10;

        if (rcf.right <= 0) // 右边小于等于0, 不需要绘画
            continue;

        LPEX_FONT hFont = (subData.hFont ? subData.hFont : m_data->hFont);
        LPEX_BRUSH hbrText = (subData.hbrText ? subData.hbrText : m_data->hBrtext);
        rcf.offset(-((float)scrollPos), 0.f);
        
        // 设置裁剪区, 不让绘画超出列范围
        _canvas_setClip(hCanvas, rcf.left - 1, rcf.top, rcf.right + 10.f, rcf.bottom, D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        if (i == 0)
        {
            rcf.left += ((float)data->level * m_data->Indent) + 5;
            if (m_data->isExpand)
            {
                DrawItemExpand(hCanvas, hItem, data, rcf, item);
                rcf.left += 16;
            }
            if (m_data->isIcon)
            {
                rcf.left += 4;
                DrawItemIcon(hCanvas, hItem, data, rcf, item);
                rcf.left += 16;
            }
            if (m_data->CheckType)
            {
                rcf.left += 4;
                DrawItemCheckbox(hCanvas, hItem, data, rcf, item);
                rcf.left += 16;
            }
        }
        DWAWITEM_STRUCT draw;
        draw.data = data;
        draw.hbrText = hbrText;
        draw.hCanvas = hCanvas;
        draw.hFont = hFont;
        draw.hItem = hItem;
        draw.subData = subData;
        rcf.left += 5;
        if (subData.isAbsLeft && subData.left)
        {
            rcf.left = (float)((int)subData.left - scrollPos);
        }
        DrawItemType(&draw, rcf);
        _canvas_resetClip(hCanvas);
    }
    delete[] indexs;
}

void CTreeList::DrawItemLine(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item)
{
    return;
    for (int i = 0; i < data->level; i++)
    {
        float left = ((float)i + 1) * m_data->Indent;
        HTREEITEM hNext = TreeView_GetNextItem(m_data->hTree, hItem, TVGN_NEXT);
        float bottom = rcf.bottom;
        
        if (!hNext && i == data->level - 1 && !(item.state & TVIS_EXPANDED))    // 没有下一项, 并且没有展开, 只画一半
            bottom = rcf.top + (rcf.bottom - rcf.top) / 2.f;
        
        LINEPOINT_F pt(left, rcf.top, left, bottom);
        _canvas_drawLine(hCanvas, m_data->hpenLine, &pt);
        if (i == data->level - 1)
        {
            float top = rcf.top + (rcf.bottom - rcf.top) / 2.f;;
            pt(left, top, left + m_data->Indent, top);
            _canvas_drawLine(hCanvas, m_data->hpenLine, &pt);
        }
    }

}

void CTreeList::DrawItemBorder(LPDWAWITEM_STRUCT draw, const RECT_F& rcf)
{
    // 低8位是画笔类型
    unsigned char penType = (draw->subData.border & 0xFF);

    switch (penType)
    {
    case BORDER_PEN_NULL:       // 没有边框
        return;
    case BORDER_PEN_SOLID:      // 实线
        break;
    case BORDER_PEN_DASH:       // 划线
        break;
    case BORDER_PEN_DOT:        // 点线
        break;
    case BORDER_PEN_DASHDOT:    // 点划线
        break;
    case BORDER_PEN_DASHDOTDOT: // 点点划线
        break;
    default:
        return;
    }
    RECT_F rc(rcf);
    rc.right += 20;
    rc.left -= 5;
    if (1)
        rc.left -= 1;
    if (!_canvas_isDxRender())
    {
        rc.right -= 1;
        rc.bottom -= 1;
    }
    if ((draw->subData.border & BORDER_RECT) == BORDER_RECT)
    {
        _canvas_drawRectangle(draw->hCanvas, m_data->hpenLine, &rc);
        return;
    }
    if (draw->subData.border & BORDER_LEFT)
    {
        LINEPOINT_F pt(rc.left, rc.top, rc.left, rc.bottom);
        _canvas_drawLine(draw->hCanvas, m_data->hpenLine, &pt);
    }
    if (draw->subData.border & BORDER_TOP)
    {
        LINEPOINT_F pt(rc.left, rc.top, rc.right - 10, rc.top);
        _canvas_drawLine(draw->hCanvas, m_data->hpenLine, &pt);
    }
    if (draw->subData.border & BORDER_RIGHT)
    {
        LINEPOINT_F pt(rc.right - 10, rc.top, rc.right - 10, rc.bottom);
        _canvas_drawLine(draw->hCanvas, m_data->hpenLine, &pt);
    }
    if (draw->subData.border & BORDER_BOTTOM)
    {
        LINEPOINT_F pt(rc.left, rc.bottom, rc.right - 10, rc.bottom);
        _canvas_drawLine(draw->hCanvas, m_data->hpenLine, &pt);
    }
}

void CTreeList::SetUIItem(HTREEITEM hItem, LPHTREELISTITEM data, STATE state, bool isRemove, bool bDraw)
{
    if (!data)return;
    bool b = false;
    if (isRemove)
    {
        b = (data->state & state) != 0;
        data->state &= ~state;
    }
    else
    {
        b = (data->state & state) == 0;
        data->state |= state;
    }
    if (bDraw && b)
    {
        RECT rc = { 0 };
        TreeView_GetItemRect(m_data->hTree, hItem, &rc, 0);
        ::InvalidateRect(m_data->hTree, &rc, 0);
    }
}

void CTreeList::SetUIItem(HTREEITEM hItem, STATE state, bool isRemove, bool bDraw)
{
    if (!hItem)return;
    SetUIItem(hItem, (LPHTREELISTITEM)GetItemNum(hItem), state, isRemove, bDraw);
}

int CTreeList::GetUIItem(LPHTREELISTITEM data)
{
    if (!data)return 0;
    return data->state;
}

int CTreeList::GetUIItem(HTREEITEM hItem)
{
    if (!hItem)return 0;
    return GetUIItem((LPHTREELISTITEM)GetItemNum(hItem));
}

void CTreeList::SetStateIcon(LPHTREELISTITEM data, int index, STATE state, bool isRemove)
{
    if (!data || index == -1)return;
    if (isRemove)
    {
        if (index == TREESYSICON_EXPAND)
            data->stateExpand &= ~state;
        else if(index == TREESYSICON_CHECKBOX)
            data->stateCheckBox &= ~state;
        else
            data->stateIcon &= ~state;
    }
    else
    {
        if (index == TREESYSICON_EXPAND)
            data->stateExpand |= state;
        else if (index == TREESYSICON_CHECKBOX)
            data->stateCheckBox |= state;
        else
            data->stateIcon |= state;
    }
    return;
}

int CTreeList::GetStateIcon(LPHTREELISTITEM data, int index)
{
    if (!data || index == -1)return 0;
    if (index == TREESYSICON_EXPAND)
        return data->stateExpand;
    if (index == TREESYSICON_CHECKBOX)
        return data->stateCheckBox;
    return data->stateIcon;
}

int CTreeList::ModifyParentHot(HTREEITEM hParent, HTREEITEM hItem, bool isRemove)
{
    if (!hParent)return 0;
    bool isNotClearHot = false;
    
    if (isRemove && m_data->index == hParent && TreeView_GetItemState(m_data->hTree, hParent, TVIS_EXPANDED))
    {
        isNotClearHot = true;
    }
    int len = m_hotGroup.size();
    if (len != m_data->count)
    {
        enumItem();
        len = m_hotGroup.size();
    }
    bool done = false;
    int count = 0;
    HTREEITEM hRoot = m_hotGroup[0].hItem;
    HTREEITEM hNext = TreeView_GetNextItem(m_data->hTree, hParent, TVGN_NEXT);
    HTREEITEM hItemTmp = hParent;
    while (!hNext && hParent != hRoot && hItemTmp)
    {
        hItemTmp = TreeView_GetParent(m_data->hTree, hItemTmp);
        hNext = TreeView_GetNextItem(m_data->hTree, hItemTmp, TVGN_NEXT);
    }
    int start = -1;
    int end = 0;
    if (m_data->index)
    {
        LPHTREELISTITEM data = (LPHTREELISTITEM)GetItemNum(m_data->index);
        if (data)
        {
            start = data->index;
        }
    }
    if (m_data->hNext)
    {
        LPHTREELISTITEM data = (LPHTREELISTITEM)GetItemNum(m_data->hNext);
        if (data)
        {
            end = data->index;
        }
    }
    else
    {
        end = len;
    }
    // 循环完一次
    for (int i = 0; i < len; i++)
    {
        HOT_GROUP_ARY& data = m_hotGroup.at(i);
        if (data.hItem == hNext)
        {
            for (; i < len; i++)
            {
                SetUIItem(data.hItem, STATE::HOVER, true, true);
                if (data.data->flag & 1)
                {
                    SetUIItem(data.hItem, STATE::HOT, true, true);
                    data.data->flag &= ~1;
                }
            }
            break;
        }
        if (!done && hParent == data.hItem)
        {
            if (!isRemove)
            {
                m_data->rcFirstHotLeft = data.data->draw[0].isAbsLeft
                    ? data.data->draw[0].left
                    : data.data->level * (int)m_data->Indent;

            }
            done = true;
        }

        //if (isRemove && m_data->index == data.hItem)
        //{
        //    // 当前选中项是热点组的第一项, 并且已经展开
        //    TVITEMEXW item;
        //    item.hItem = data.hItem;
        //    item.mask = TVIF_STATE | TVIF_PARAM | TVIF_CHILDREN;
        //    TreeView_GetItem(m_data->hTree, &item);
        //    if (item.lParam)
        //        m_data->rcFirstHotLeft = ((LPHTREELISTITEM)item.lParam)->level * (int)m_data->Indent;
        //    if ((item.state & TVIS_EXPANDED) && item.cChildren) // 有子项, 并且已展开
        //    {
        //        // 循环把所有子项都加上热点状态
        //        bool bContinue = false;
        //        for (; i < len; i++)
        //        {
        //            HOT_GROUP_ARY& data1 = m_hotGroup.at(i);
        //            if (data1.data->hItem == m_data->hNext)
        //            {
        //                SetUIItem(data1.hItem, data1.data, STATE::HOT, true, true);
        //                SetUIItem(data1.hItem, data1.data, STATE::HOVER, true, true);
        //                data1.data->flag &= ~1;
        //                bContinue = true;
        //                done = false;
        //                break;
        //                //return 0;
        //            }
        //            // 去除进入状态, 加入热点状态
        //            SetUIItem(data1.hItem, data1.data, STATE::HOVER, true, true);
        //            SetUIItem(data1.hItem, data1.data, STATE::HOT, false, true);
        //            data1.data->flag |= 1;
        //
        //        }
        //        if (bContinue)continue;
        //    }
        //}


        if (done)
        {
            count++;
            if (isRemove)
            {
                SetUIItem(data.hItem, data.data, STATE::HOVER, isRemove, true);
                if (isNotClearHot)
                {
                    // 不清除热点, 加上个热点项
                    SetUIItem(data.hItem, data.data, STATE::HOT, false, true);
                }
                else
                {
                    SetUIItem(data.hItem, data.data, STATE::HOT, isRemove, true);
                    data.data->flag &= ~1;
                }
            }
            else
            {
                STATE state = (data.hItem == hItem) ? STATE::HOVER : STATE::HOT;
                SetUIItem(data.hItem, state, false, false);
                data.data->flag |= 1;
            }
        }
        else if (data.data->flag & 1)
        {
            SetUIItem(data.hItem, STATE::HOT, true, true);
            data.data->flag &= ~1;
        }
    }
    return count;
}

void CTreeList::FillItem(LPCANVAS hCanvas, LPHTREELISTITEM data, int scrollPos, const RECT_F& rcf)
{
    LPEX_BRUSH hbr;


    if ((data->state & STATE::HOVER))
    {
        if ((data->state & STATE::DOWN))
            hbr = m_data->hBrSF;
        else if((data->state & STATE::FOCUS))
            hbr = m_data->hBrSelect;
        else
            hbr = m_data->hBrHove;
    }
    else if ((data->state & STATE::FOCUS) && (data->state & STATE::DOWN))
    {
        if ((data->state & STATE::DOWN))
            hbr = m_data->hBrSF;
        else
            hbr = m_data->hBrHot;
    }
    else if ((data->state & STATE::HOT))
    {
        if ((data->state & STATE::DOWN))
            hbr = m_data->hBrSelect;
        else
            hbr = m_data->hBrHot;
    }
    else if ((data->state & STATE::DOWN))
    {
        hbr = m_data->hBrSelect;
    }
    else
    {
        hbr = m_data->hBrNormal;
    }

    RECT_F rcDraw(rcf);
    if ((data->flag & 1))
    {
        rcDraw.left = (float)m_data->rcFirstHotLeft - (float)scrollPos;
    }
    else if (data->level)    // 有缩进
    {
        if (data->draw[0].isAbsLeft)
            rcDraw.left = (float)((int)data->draw[0].left - 5);
        else
            rcDraw.left += data->level * (int)m_data->Indent;
        rcDraw.left -= ((float)scrollPos);
        if (rcDraw.left < 0)
            rcDraw.left = 0;
    }
    else
    {
        rcDraw = rcf;
    }
    if (data->draw->type == TLVTYPE_STATIC)
    {
        ARGB_D2D r = _brush_getcolor(hbr);
        BYTE a = r.getA();
        if (a > 50) r.setA(50);

        _brush_setcolor(hbr, r);
        _canvas_fillRectangle(hCanvas, hbr, &rcDraw);
        r.setA(a);
        _brush_setcolor(hbr, r);

        return;
    }

    _canvas_fillRectangle(hCanvas, hbr, &rcDraw);
    

}

void CTreeList::DrawItemExpand(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item)
{
    if (item.cChildren)
    {
        RECT_F rcDraw(64.f, 0.f, 80.f, TREEITEM_ICONWDITH);
        if (data->stateExpand & STATE::HOT)  // 有焦点
            rcDraw.offset(TREEITEM_ICONWDITH * 2, 0.f);

        if ((item.state & TVIS_EXPANDED))   // 已经展开
            rcDraw.offset(TREEITEM_ICONWDITH, 0.f);

        RECT_F rcImg(rcf);
        //rcImg.left -= 20;
        rcImg.top += ((float)(m_data->nHeadHeight) - TREEITEM_ICONWDITH) / 2;
        rcImg.right = rcImg.left + TREEITEM_ICONWDITH;
        rcImg.bottom = rcImg.top + TREEITEM_ICONWDITH;
        _canvas_drawimagerectrect(hCanvas, m_data->imgTree, rcImg.left, rcImg.top, rcImg.right, rcImg.bottom,
            rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom, 255);
    }
}

void CTreeList::DrawItemIcon(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item)
{
    RECT_F rcIcon(rcf);
    rcIcon.top += ((float)(m_data->nHeadHeight) - TREEITEM_ICONWDITH) / 2;
    rcIcon.right = rcIcon.left + TREEITEM_ICONWDITH;
    rcIcon.bottom = rcIcon.top + TREEITEM_ICONWDITH;
    _canvas_fillRectangle(hCanvas, m_data->hBrLine, &rcIcon);
}

void CTreeList::DrawItemCheckbox(LPCANVAS hCanvas, HTREEITEM hItem, LPHTREELISTITEM data, const RECT_F& rcf, const TVITEMEXW& item)
{
    RECT_F rcIcon(rcf);
    rcIcon.top += ((float)(m_data->nHeadHeight) - TREEITEM_ICONWDITH) / 2;
    rcIcon.right = rcIcon.left + TREEITEM_ICONWDITH;
    rcIcon.bottom = rcIcon.top + TREEITEM_ICONWDITH;
    _canvas_fillRectangle(hCanvas, m_data->hBrLine, &rcIcon);
}

void CTreeList::DrawItemType(LPDWAWITEM_STRUCT draw, const RECT_F& rcf)
{
    DWORD type = draw->subData.type;
    DrawItemBorder(draw, rcf);
    switch (type)
    {
    case TLVTYPE_TEXT:
    case TLVTYPE_STATIC:
        DrawItemType_Text(draw, rcf);
        break;
    case TLVTYPE_BUTTON:
        DrawItemType_Text(draw, rcf);
        break;
    case TLVTYPE_EDIT:
        DrawItemType_Text(draw, rcf);
        break;
    case TLVTYPE_CHECKBOX:
        DrawItemType_CheckBox(draw, rcf);
        break;
    case TLVTYPE_RADIO:
        DrawItemType_Radio(draw, rcf);
        break;
    case TLVTYPE_COMBOBOX:
        DrawItemType_Text(draw, rcf);
        break;
    default:
        return;
    }

    
}

void CTreeList::DrawItemType_Text(LPDWAWITEM_STRUCT draw, const RECT_F& rcf)
{
    wstr& str = draw->subData.title;
    DWORD fmt = draw->subData.format;
    if (fmt == 0)
        fmt = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
    if (!str.empty())
    {
        size_t count = str.size();
        float charWidth = 0.f, charheight = 0.f;
        draw->hFont->GetCharSize(&charWidth, &charheight);
        if (charWidth == 0)
        {
            _canvas_calctextsize(draw->hCanvas, draw->hFont, L"i", 1,
                DT_CALCRECT | fmt,
                rcf.right - rcf.left, rcf.bottom - rcf.top, &charWidth, &charheight);
            draw->hFont->SetCharSize(charWidth, charheight);

        }
        size_t n = (size_t)((rcf.right - rcf.left) / charWidth + 10);
        if (n < count) count = n;

        _canvas_drawtext(draw->hCanvas, draw->hFont, draw->hbrText, str.c_str(), count,
            fmt, &rcf);

    }
}



void CTreeList::DrawItemType_Button(LPDWAWITEM_STRUCT draw, const RECT_F& rcf)
{
    // 选择框图标 130像素开始, 顶部是0, 15*15, 0=正常未选中, 1=点燃未选中, 2=按下未选中, 3=正常选中, 4=点燃选中, 5=按下选中

}

void CTreeList::DrawItemType_Edit(LPDWAWITEM_STRUCT draw, const RECT_F& rcf)
{


}

void CTreeList::DrawItemTypeCombobox(LPDWAWITEM_STRUCT draw, const RECT_F& rcf)
{
}

void CTreeList::DrawItemType_CheckBox(LPDWAWITEM_STRUCT draw, const RECT_F& rcf)
{
    // 选择框图标 130像素开始, 顶部是0, 15*15, 0=正常未选中, 1=点燃未选中, 2=按下未选中, 3=正常选中, 4=点燃选中, 5=按下选中
    float left = rcf.left + (rcf.right - rcf.left - 15) / 2;
    float top = rcf.top + (rcf.bottom - rcf.top - 15) / 2;
    wchar_t ch = '0';
    if (!draw->subData.title.empty())
        ch = draw->subData.title[0];
    float ileft = 130.f + ((ch == '0') ? (15 * 0) : (15 * 3));
    _canvas_drawimagerectrect(draw->hCanvas, m_data->imgTree, left, top, left + 15, top + 15, ileft, 0, ileft + 15, 15, 255);

    return;
}

void CTreeList::DrawItemType_Radio(LPDWAWITEM_STRUCT draw, const RECT_F& rcf)
{
    // 单选框图标 220像素开始, 顶部是0, 15*15, 0=正常未选中, 1=点燃未选中, 2=按下未选中, 3=禁止未选中, 4=正常选中, 5=点燃选中, 6=按下选中, 7=禁止选中
    float left = rcf.left + (rcf.right - rcf.left - 15) / 2;
    float top = rcf.top + (rcf.bottom - rcf.top - 15) / 2;
    wchar_t ch = '0';
    if (!draw->subData.title.empty())
        ch = draw->subData.title[0];
    float ileft = 220.f + ((ch == '1') ? (15 * 4) : (15 * 0));
    _canvas_drawimagerectrect(draw->hCanvas, m_data->imgTree, left, top, left + 15, top + 15, ileft, 0, ileft + 15, 15, 255);
}

//int main(void)
//{
//    int i = -1;
//    if (i = 0)
//        printf("i = %d\n", i);
//    else if (i = 1)
//        printf("i = %d\n", i);
//    else
//        printf("i = %d\n", i);
//    return 0;
//    // 选择题
//    // A: 输出 i = 0
//    // B: 输出 i = 1
//    // C: 输出 i = -1
//    // D: 使用不同的编译器编译运行, 结果可能不一样
//}

void CTreeList::__create_d2d_resouce()
{
    if (!m_data || m_data->hCanvas || m_data->hCanvas_Head)return;
    RECT rc;
    ::GetWindowRect(m_data->hTree, &rc);
    int cx = rc.right - rc.left, cy = rc.bottom - rc.top;
    m_data->hCanvas = _canvas_create(m_data->hTree, cx, cy);

    ::GetWindowRect(m_data->hHead, &rc);
    cx = rc.right - rc.left;
    cy = rc.bottom - rc.top;

    m_data->hCanvas_Head = _canvas_create(m_data->hHead, cx, cy);
    m_data->hBrSF =   _brush_create(ARGB_D2D(200, 205, 232, 255));
    m_data->hBrHove =   _brush_create(ARGB_D2D(140, 205, 232, 255));
    m_data->hBrHot = _brush_create(ARGB_D2D(80, 205, 232, 255));
    m_data->hBrSelect = _brush_create(ARGB_D2D(180, 217, 217, 217));
    m_data->hBrNormal = _brush_create(ARGB_D2D(255, 255, 255, 255));
    m_data->hBrtext = _brush_create(ARGB_D2D(254, 0, 0, 0));
    m_data->hBrLine = _brush_create(ARGB_D2D(254, 229, 229, 229));
    m_data->hpenLine = _pen_create(ARGB_D2D(254, 229, 229, 229));
    m_data->hBrTopLine = _brush_create(ARGB_D2D(254, 160, 160, 160));
    m_data->hpenTopLine = _pen_create(ARGB_D2D(254, 160, 160, 160));
    m_data->imgTree = _img_create_frommemory(s_ImageTreep, sizeof(s_ImageTreep) / sizeof(s_ImageTreep[0]));
    LOGFONTW lf;
    GetObjectW(CWndBase::DefaultFont(), sizeof(lf), &lf);
    m_data->hFont = _font_create_logfont(&lf, 0);
}

void CTreeList::__delete_d2d_resouce()
{
    if (!m_data || m_data->hCanvas || m_data->hCanvas_Head)return;
    _brush_destroy(m_data->hBrSF);
    _brush_destroy(m_data->hBrHove);
    _brush_destroy(m_data->hBrHot);
    _brush_destroy(m_data->hBrSelect);
    _brush_destroy(m_data->hBrNormal);
    _brush_destroy(m_data->hBrtext);
    _brush_destroy(m_data->hBrLine);
    _brush_destroy(m_data->hBrTopLine);

    _pen_destroy(m_data->hpenLine);
    _pen_destroy(m_data->hpenTopLine);

    _img_destroy(m_data->imgTree);
    _font_destroy(m_data->hFont);
    _canvas_destroy(m_data->hCanvas);
    _canvas_destroy(m_data->hCanvas_Head);
}

void CTreeList::__modify_col_width(int indexCol, int cx)
{
    int len = m_header.size();
    if (indexCol < 0 || indexCol >= len) return;

    TREELIST_COL_DATA& item = m_header.at(indexCol);    // 取插入位置的成员
    int nWidth = cx - item.width;   // 增加长度
    item.rc.right = item.rc.left + cx;
    item.width = cx;
    int max = 0;
    for (int i = indexCol + 1; i < len; i++)    // 从插入位置往后每个成员增加当前列的宽度
    {
        TREELIST_COL_DATA& next = m_header.at(i);
        next.rc.left += nWidth;     // 后面每列往后移这一列的宽度
        next.rc.right += nWidth;
        max = next.rc.right;
    }
    if (max == 0)max = item.rc.right;
    int width = m_data->width;
    BOOL isShow = FALSE;
    if (max > width)    // 需要横向滚动条
    {
        isShow = TRUE;
        width += 1;
    }
    else
    {
        width = max + 1;
    }
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nMin = 0;
    si.nMax = max;
    si.nPage = width;
    ShowScrollBar(m_data->hTree, SB_HORZ, isShow);
    SetScrollInfo(m_data->hTree, SB_HORZ, &si, TRUE);

    si.fMask = SIF_ALL;
    GetScrollInfo(m_data->hTree, SB_HORZ, &si);
    if (m_data->rcHead.left != -si.nPos)
    {
        m_data->rcHead.left = -si.nPos;
        m_data->rcHead.right = m_data->width + si.nPos;

        //InvalidateRect(m_data->hHead, 0, 0);
        MoveWindow(m_data->hHead, m_data->rcHead.left, m_data->rcHead.top, m_data->rcHead.right, m_data->nHeadHeight, TRUE);
    }
    //wstr::dbg(L"显示滚动条=%d,max=%d, page=%d\n", isShow, max, si.nPage);
    //OutputDebugStringW(str.c_str());
    ::InvalidateRect(m_data->hTree, 0, 0);
    ::InvalidateRect(m_data->hHead, 0, 0);
}

LRESULT CTreeList::OnHScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_ALL;
    BOOL bRet = GetScrollInfo(hWnd, SB_HORZ, &si);
    int nMaxPos = si.nMax - si.nPage;
    const int LINESIZE = 8;
    int pos = HIWORD(wParam);
    int scrValue = 0;
    int code = LOWORD(wParam);
    switch (code)
    {
    case SB_LINEUP:
        if (nMaxPos - si.nPos < LINESIZE)
            scrValue = -(nMaxPos - si.nPos);
        else
            scrValue = -LINESIZE;
        break;
    case SB_PAGEUP:
        if (si.nPos)
        {
            scrValue = (si.nPos - si.nPage < 0) ? si.nPos : si.nPage;
        }
        break;

    case SB_PAGEDOWN:
        if (si.nPage != nMaxPos)
        {
            scrValue = (nMaxPos - si.nPos < (int)si.nPage) ? (-(nMaxPos - si.nPos)) : -((int)si.nPage);
        }
        break;

    case SB_THUMBTRACK:
        scrValue = si.nPos - pos;
        break;
    default:
        break;
    }
    ScrollWindow(hWnd, scrValue, 0, 0, 0);
    si.fMask = SIF_POS;
    si.nPos -= scrValue;
    SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

    if (m_data->rcHead.left != -si.nPos)
    {
        m_data->rcHead.left = -si.nPos;
        m_data->rcHead.right = m_data->width + si.nPos;

        //InvalidateRect(m_data->hHead, 0, 0);
        MoveWindow(m_data->hHead, m_data->rcHead.left, m_data->rcHead.top, m_data->rcHead.right, m_data->nHeadHeight, TRUE);
    }
    //wstr::dbg(L"code=%d,pos=%d, getpos = %d, min=%d,max=%d, bRet=%d\n", code, HIWORD(wParam), GetScrollPos(hWnd, SB_HORZ), si.nMin, si.nMax, bRet);

    return 0;
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

LRESULT CTreeList::OnPaintHeader(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    //InvalidateRect(m_data->hTree, 0, 0);
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    CMemDC dc;
    RECT rc;
    ::GetClientRect(hWnd, &rc);
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;
    HDC hDCMem = dc.create(cx, cy);
    HGDIOBJ hFontOld = SelectObject(hDCMem, CWndBase::DefaultFont());
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
    HGDIOBJ hPenOld = SelectObject(hDCMem, hPen);

    FillRect(hDCMem, &rc, GetStockBrush(WHITE_BRUSH));

    int len = m_header.size();
    int fmt = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
    for (int i = 0; i < len; i++)
    {
        TREELIST_COL_DATA& col = m_header[i];
        rc = col.rc;
        //_canvas_fillRectangle(hCanvas, m_data->hBrHot, &rcf);
        rc.left += 5;
        // 217,235,249 点燃
        // 188,220,244 按下

        HBRUSH hbr = 0;
        if (i == m_headerIndexHot)
        {
            hbr = CreateSolidBrush(m_headerIsDown ? RGB(188, 220, 244) : RGB(217, 235, 249));

            rc.left -= 5;
            FillRect(hDCMem, &rc, hbr);
            DeleteObject(hbr);
            rc.left += 5;
        }
        MoveToEx(hDCMem, rc.right - 1, 0, 0);
        LineTo(hDCMem, rc.right - 1, cy);
        if (i == m_headerIndexHot && m_headerIsDown)
            OffsetRect(&rc, 1, 1);
        DrawTextW(hDCMem, col.title.c_str(), (int)col.title.size(), &rc, fmt);
    }

    
    MoveToEx(hDCMem, 2, cy - 1, 0);
    LineTo(hDCMem, cx - 2, cy - 1);
    SelectObject(hDCMem, hPenOld);
    DeleteObject(hPen);

    //hPen = CreatePen(PS_SOLID, 1, RGB(160, 160, 160));
    //hPenOld = SelectObject(hDCMem, hPen);
    //MoveToEx(hDCMem, 0, 0, 0);
    //LineTo(hDCMem, cx, 0);
    //SelectObject(hDCMem, hPenOld);
    //DeleteObject(hPen);

    BitBlt(ps.hdc, 0, 0, cx, cy, hDCMem, 0, 0, SRCCOPY);
    SelectObject(hDCMem, hFontOld);

    EndPaint(hWnd, &ps);


    return 0;
}

void CTreeList::InvalidateItem(HTREEITEM hItem)
{
    if (!isItem(hItem))return;
    RECT rc;
    TreeView_GetItemRect(m_data->hTree, hItem, &rc, 0);
    ::InvalidateRect(m_data->hTree, &rc, 0);
}

void CTreeList::CalcExpand(LPHTREELISTITEM data, RECT* rc, int scrollPos)
{
    if (!data || !rc)return;
    if (!m_data->isExpand)
    {
        memset(rc, 0, sizeof(RECT));
        return;
    }
    HTREELISTITEM_SUBDATA& subData = data->draw[0];
    rc->left = (subData.left && subData.isAbsLeft ? subData.left : m_header[0].rc.left);
    rc->left -= scrollPos;
    rc->left += (data->level * (int)m_data->Indent) + 5;
    rc->right = rc->left + TREEITEM_ICONWDITH;
    rc->top = (m_data->nHeadHeight - TREEITEM_ICONWDITH) / 2;
    rc->bottom = rc->top + TREEITEM_ICONWDITH;
}

void CTreeList::CalcIcon(LPHTREELISTITEM data, RECT* rc, int scrollPos)
{
    if (!data || !rc)return;
    if (!m_data->isIcon)
    {
        memset(rc, 0, sizeof(RECT));
        return;
    }

    HTREELISTITEM_SUBDATA& subData = data->draw[0];
    rc->left = (subData.left && subData.isAbsLeft ? subData.left : m_header[0].rc.left);
    rc->left -= scrollPos;
    rc->left += (data->level * (int)m_data->Indent) + 5;
    if (m_data->isExpand)
        rc->left += 20;
    if (m_data->CheckType)
        rc->left += 20;
    rc->right = rc->left + TREEITEM_ICONWDITH;
    rc->top = (m_data->nHeadHeight - TREEITEM_ICONWDITH) / 2;
    rc->bottom = rc->top + TREEITEM_ICONWDITH;
}

void CTreeList::CalcCheckbox(LPHTREELISTITEM data, RECT* rc, int scrollPos)
{
    if (!data || !rc)return;
    if (!m_data->isIcon)
    {
        memset(rc, 0, sizeof(RECT));
        return;
    }

    HTREELISTITEM_SUBDATA& subData = data->draw[0];
    rc->left = (subData.left && subData.isAbsLeft ? subData.left : m_header[0].rc.left);
    rc->left -= scrollPos;
    rc->left += (data->level * (int)m_data->Indent) + 5;
    if (m_data->isExpand)
        rc->left += 20;
    rc->right = rc->left + TREEITEM_ICONWDITH;
    rc->top = (m_data->nHeadHeight - TREEITEM_ICONWDITH) / 2;
    rc->bottom = rc->top + TREEITEM_ICONWDITH;
}

bool CTreeList::ptinExpand(LPHTREELISTITEM data, int x, int y, int scrollPos)
{
    RECT rc;
    CalcExpand(data, &rc, scrollPos);
    bool ret = (x >= rc.left && x<rc.right && y>rc.top && y < rc.bottom);
    return ret;
}

bool CTreeList::ptinIcon(LPHTREELISTITEM data, int x, int y, int scrollPos)
{
    RECT rc;
    CalcIcon(data, &rc, scrollPos);
    bool ret = (x >= rc.left && x<rc.right && y>rc.top && y < rc.bottom);
    return ret;
}

bool CTreeList::ptinCheckbox(LPHTREELISTITEM data, int x, int y, int scrollPos)
{
    RECT rc;
    CalcCheckbox(data, &rc, scrollPos);
    bool ret = (x >= rc.left && x<rc.right && y>rc.top && y < rc.bottom);
    return ret;
}

int CTreeList::ptinSysIcon(LPHTREELISTITEM data, RECT* prc, int x, int y, int scrollPos)
{
    if (!data)return -1;
    if (!m_data->isIcon && !m_data->isExpand && !m_data->CheckType)   // 3个系统图标都没有
    {
        if (prc) memset(prc, 0, sizeof(RECT));
        return -1;
    }
    RECT rc[3];
    HTREELISTITEM_SUBDATA& subData = data->draw[0];
    rc[0].left = (subData.left && subData.isAbsLeft ? subData.left : m_header[0].rc.left);
    rc[0].left -= scrollPos;

    rc[0].left += (data->level * (int)m_data->Indent) + 5;
    rc[0].right = rc[0].left + TREEITEM_ICONWDITH;
    rc[0].top = (m_data->nHeadHeight - TREEITEM_ICONWDITH) / 2;
    rc[0].bottom = rc[0].top + TREEITEM_ICONWDITH;

    int offset = 20;
    rc[1] = { rc[0].left + offset, rc[0].top, rc[0].right + offset, rc[0].bottom };
    offset += 20;
    rc[2] = { rc[0].left + offset, rc[0].top, rc[0].right + offset, rc[0].bottom };
    int index = -1;
    for (int i = 0; i < sizeof(rc) / sizeof(rc[0]); i++)
    {
        if (x >= rc[i].left && x < rc[i].right && y >= rc[i].top && y < rc[i].bottom)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
        return -1;
    if (index == 0)
    {
        if (m_data->isExpand)
        {
            if (TreeView_GetChild(m_data->hTree, data->hItem))
                return TREESYSICON_EXPAND;
            return TREESYSICON_INVALID;
        }
        if (m_data->CheckType)
            return TREESYSICON_CHECKBOX;
        if (m_data->isIcon)
            return TREESYSICON_ICON;
        return TREESYSICON_INVALID;
    }
    if (index == 1)
    {
        if (m_data->isExpand)   // 加减框存在
        {
            if (m_data->CheckType)
                return TREESYSICON_CHECKBOX;
            if (m_data->isIcon)
                return TREESYSICON_ICON;
            return TREESYSICON_INVALID;
        }
        // 加减框不存在
        if (m_data->CheckType)
        {
            // 选择框在第一个图标位置
            if (m_data->isIcon) // 如果图标存在, 返回图标
                return TREESYSICON_ICON;
            return TREESYSICON_INVALID;
        }
    }
    if (index == 2)
    {
        // 在第三个图标位置, 一个3个图标, 如果第一第二个图标不存在, 就返回空
        if (!m_data->isExpand)return TREESYSICON_INVALID;
        if (!m_data->CheckType)return TREESYSICON_INVALID;
        return TREESYSICON_ICON;
    }
    return TREESYSICON_INVALID;
}

HTREEITEM CTreeList::MouseIsItem(int x, int y, WPARAM wParam, LPHTREELISTITEM* data, int* indexCol, int* indexColIcon, int scrollPos, bool isDown)
{
    if (data)*data = 0;
    if (indexCol)*indexCol = -1;
    if (indexColIcon)*indexColIcon = -1;
    int len = m_header.size();
    int max = m_header.at(len - 1).rc.right;
    TVHITTESTINFO ht;
    ht.pt.x = x;
    ht.pt.y = y;
    HTREEITEM hItem = TreeView_HitTest(m_data->hTree, &ht);
    if (!hItem || x > max)
    {
    exit:
        if (m_isHotList)
        {
            ModifyParentHot(m_data->hotParent, m_data->indexHot, true);
            m_data->hotParent = 0;
            if (m_data->indexHot)
            {
                LPHTREELISTITEM pData = (LPHTREELISTITEM)GetItemNum(m_data->indexHot);
                SetStateIcon(pData, m_data->indexColIcon, STATE::HOT, true);
                m_data->indexColIcon = -1;
            }
            ::InvalidateRect(m_data->hTree, 0, 0);
        }
        else if (m_data->indexHot)
        {
            SetUIItem(m_data->indexHot, STATE::HOT, true, false);
            InvalidateItem(m_data->indexHot);
        }
        m_data->indexHot = 0;
        m_data->indexCol = -1;
        if (m_data->indexDown)
        {
            SetUIItem(m_data->indexDown, STATE::DOWN, true, false);
            InvalidateItem(m_data->indexDown);
        }
        data = 0;
        return 0;
    }
    // 当前选中表项和按下表项相等, 并且移动中是鼠标按下状态
    if(!isDown && m_data->indexDown == 0 && ((wParam & MK_LBUTTON) || (wParam & MK_RBUTTON)))
    {
        //OutputDebugStringW(L"当前选中表项和按下表项相等, 并且移动中是鼠标按下状态\n");
        return 0;
    }
    LPHTREELISTITEM pData = (LPHTREELISTITEM)GetItemNum(hItem);
    if (!pData)
        return 0;
    
    DWORD left = 0;
    if (pData->draw[0].isAbsLeft)
        left = pData->draw[0].left;
    else
        left = (DWORD)((float)pData->level * m_data->Indent);
    if (x < (int)left && scrollPos < (int)left)
        goto exit;
    
    if (indexCol)
    {
        *indexCol = -1;
        const int offset = pData->level * (int)m_data->Indent;
        
        for (int i = 0; i < len; i++)
        {
            HTREELISTITEM_SUBDATA& subData = pData->draw[i];
            // 根据设置的位置来获取是哪一列
            left = subData.isAbsLeft ? pData->draw[i].left : m_header.at(i).rc.left;
            DWORD width = subData.isAbsWidth ? pData->draw[i].width : m_header.at(i).width;
            if (width > (DWORD)max)
                width = max;
            left -= (DWORD)scrollPos;
            if (i == 0 && !subData.isAbsLeft) // 第一列因为有子项缩进, 缩进部分不响应鼠标位置, 特别处理
                left += offset;
            int right = ((int)left + (int)width - (i == 0 ? offset : 0));
            if (x >= (int)left && x <= right) // 鼠标位置大于当前列左边 且 小于当前列右边
            {
                
                if (indexColIcon)
                {
                    RECT rc;
                    TreeView_GetItemRect(m_data->hTree, hItem, &rc, 0);
                    *indexColIcon = ptinSysIcon(pData, 0, x, y - rc.top, scrollPos);
                }
                //if (subData.type == TLVTYPE_STATIC && *indexColIcon == -1) // 静态控件, 不响应点击
                //{
                //    return 0;
                //}
                *indexCol = i;
                break;
            }
        }
        if(*indexCol == -1)
            goto exit;
    }
    if (((wParam & MK_LBUTTON) || (wParam & MK_RBUTTON)) && m_data->indexDown) // 移动鼠标+左/右键按下
    {
        if (m_data->indexDown != hItem) // 鼠标按下的表项和选择鼠标移动的表项不同
        {
            SetUIItem(m_data->indexHot, STATE::HOT, true, false);       // 去除热点状态
            InvalidateItem(m_data->indexHot);

            if (m_data->indexDown != m_data->index)
            {
                SetUIItem(m_data->indexDown, STATE::DOWN, true, false); // 去除按下状态
                InvalidateItem(m_data->indexDown);
            }
            
            if (!(GetUIItem(m_data->indexDown) & STATE::FOCUS))
            {
                // 给原来选中的表项加上焦点状态
                SetUIItem(m_data->index, STATE::FOCUS, false, false);   // 加入焦点状态
                InvalidateItem(m_data->index);
            }
            return 0;
        }
        else
        {
            return 0;
        }
    }

    
    if (data)*data = pData;
    return hItem;
}

CTreeList::CTreeList() : m_data(0), m_headerIndex(0), m_isHotList(false), m_hItems(0), m_hItemsCount(0)
{
    memset(&m_evt, 0, sizeof(m_evt));
}

CTreeList::~CTreeList()
{
    if (m_headerIndex)
        delete[] m_headerIndex;
    if (m_hItems)
    {
        delete[] m_hItems;
        m_hItems = 0;
        m_hItemsCount = 0;
    }
    __delete_d2d_resouce();
}

HWND CTreeList::create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree, int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID, LPVOID lpParam)
{
    if (IsWindow())return GetWindow();
    if (!m_data)
    {
        m_data = new TREELIST_DATA;
        memset(m_data, 0, sizeof(TREELIST_DATA));
        m_data->nHeadHeight = 30;
        m_data->pThis = this;
        m_data->Indent = 16.f;
        m_data->index = 0;
        m_data->indexCol = -1;
        m_data->indexColIcon = -1;
        m_data->indexHot = 0;
        m_data->isIcon = false;
        m_data->CheckType = 0;
        m_data->indexHotHead = -1;
        m_data->indexDownHead = -1;
        m_headerIndexHot = -1;
        m_headerIsDown = false;
    }
    
    CREATESTRUCT cs = { 0 };
    cs.lpszName = lpszTitle;
    cs.dwExStyle = dwExStyle;
    cs.style = WS_VISIBLE | WS_CHILD | WS_HSCROLL | SS_NOTIFY;
    cs.hwndParent = hWndParent;
    cs.x = x;
    cs.y = y;
    cs.cx = nWidth;
    cs.cy = nHeight;
    cs.hMenu = (HMENU)ID;
    cs.hInstance = 0;
    cs.lpCreateParams = lpParam;
    //cs.lpszClass = __WndBase_RegisterClass(L"SysTreeView32", className(), CWndBase::CWndBase_DefWndProc);
    //__WndBase_RegisterClass(L"ss", L"ss", static_WndProc);
    m_data->hStatic = CreateWindowExW(0, L"static", 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, x, y, nWidth, nHeight, hWndParent, (HMENU)ID, CWndBase::GetInst(), lpParam);
    if (!m_data->hStatic)return 0;
    m_data->oldProcStatic = (WNDPROC)SetWindowLongPtrW(m_data->hStatic, GWLP_WNDPROC, (LONG_PTR)static_WndProc);
    
    ::SetPropW(m_data->hStatic, L"proc", m_data);

    DWORD oldStyle = dwStyle;
    dwStyle &= ~TVS_LINESATROOT;
    dwStyle &= ~TVS_HASBUTTONS;
    dwStyle |= WS_CHILD | WS_CLIPSIBLINGS;

    if (!CWndBase::create(dwExStyle, lpszTitle, dwStyle, x, y + 50, nWidth, nHeight - 50, m_data->hStatic, ID, lpParam))return 0;
    m_data->hTree = m_hWnd;
    //m_hWnd = m_data->hStatic;
    //SetClassLongPtrW(m_data->hTree, GCL_STYLE, GetClassLongPtrW(m_data->hTree, GCL_STYLE) | CS_DBLCLKS);
    //m_data->oldProcTree = (WNDPROC)SetWindowLongPtrW(m_data->hTree, GWLP_WNDPROC, (LONG_PTR)static_WndProc);
    SetTreeStyleEx(dwExStyleTree);
    OnStyleChenged(false, oldStyle);
    OnStyleChenged(true, dwExStyleTree);
    HWND hWndTips = TreeView_GetToolTips(m_data->hTree);
    m_tips.create(TTS_ALWAYSTIP, WS_EX_LAYERED | WS_EX_TRANSPARENT);
    ::SetPropW(m_data->hTree, L"proc", m_data);
    m_tips.Insert(m_data->hTree, 0, 0, 0);
    m_tips.Send(TTM_SETMAXTIPWIDTH, 0, nWidth);
    m_tips.Send(TTM_SETDELAYTIME, TTDT_AUTOPOP, MAKELONG(0x7FFF, 0));

    RECT rcParent;
    HDLAYOUT hdl;
    WINDOWPOS wp;

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    if ((m_data->hHead = CreateWindowExW(0, WC_HEADER, 0,
        WS_CHILD | HDS_BUTTONS | HDS_HORZ | HDS_FULLDRAG | HDS_CHECKBOXES | HDS_DRAGDROP | WS_CLIPSIBLINGS,
        0, 0, nWidth, nHeight, m_data->hStatic, (HMENU)1, CWndBase::GetInst(),
        (LPVOID)NULL)) == NULL)
        return m_hWnd;
    
    SendMessageW(m_data->hHead, WM_SETFONT, (WPARAM)CWndBase::DefaultFont(), 0);

    ::GetClientRect(m_data->hStatic, &rcParent);
    hdl.prc = &rcParent;
    hdl.pwpos = &wp;
    if (!SendMessageW(m_data->hHead, HDM_LAYOUT, 0, (LPARAM)&hdl))
        return m_hWnd;

    SetWindowPos(m_data->hHead, wp.hwndInsertAfter, wp.x, wp.y,
        wp.cx, wp.cy, wp.flags | SWP_SHOWWINDOW);
    m_data->oldProcHeader = (WNDPROC)SetWindowLongPtrW(m_data->hHead, GWLP_WNDPROC, (LONG_PTR)static_WndProc);
    ::SetPropW(m_data->hHead, L"proc", m_data);
    return m_hWnd;
}


HWND CTreeList::create(HWND hWndParent, DWORD dwExStyleTree)
{
    return create(0, 0, 0, dwExStyleTree, 0, 0, 0, 0, hWndParent, 0, 0);
}

HWND CTreeList::create(HWND hWndParent, LPCWSTR title, RECT rc, int style, int styleEx, DWORD dwExStyleTree, LONG_PTR ID, void* lpParam)
{
    return create(styleEx, title, style, dwExStyleTree, rc.left, rc.top, rc.right, rc.bottom, hWndParent, ID, lpParam);
}

int CTreeList::SetHeaderHeight(int height)
{
    if (!m_data) return 0;
    int ret = m_data->nHeadHeight;
    m_data->nHeadHeight = height;
    SetWindowPos(m_data->hStatic, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOZORDER);
    return ret;
}

HTREEITEM CTreeList::InsertItem(HTREEITEM hParent, LPCWSTR itemText, LONG_PTR itemParam, HTREEITEM InsertMode)
{
    if (m_hItems)
    {
        delete[] m_hItems;
        m_hItems = 0;
        m_hItemsCount = 0;
    }
    int leave = 0;
    if (hParent)
    {
        LPHTREELISTITEM parent = (LPHTREELISTITEM)GetItemNum(hParent);
        if (parent)
        {
            leave = parent->level + 1;
        }
    }
    LPHTREELISTITEM data = new HTREELISTITEM(m_header);   // 每个项目都是一个结构, 项目数值保存指针
    data->param = itemParam;
    if (itemText)
        data->draw[0].title = itemText;
    data->level = leave;
    m_data->count++;
    HTREEITEM hItem = CTreeView::InsertItem(hParent, 0, -1, -1, (LONG_PTR)data, InsertMode);
    data->hItem = hItem;
    return hItem;
}

int CTreeList::DoInsertItem(int iInsertAfter, int nWidth, LPCWSTR lpsz)
{
    if (iInsertAfter == -1)
        iInsertAfter = m_header.size();
    HDITEM hdi;
    int index;
    //ListView_SetItem
    hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH | HDI_LPARAM;
    hdi.cxy = nWidth;
    hdi.pszText = (LPWSTR)lpsz;
    hdi.cchTextMax = (int)(LONG_PTR)wcslen(lpsz);
    hdi.fmt = HDF_LEFT | HDF_STRING;
    hdi.lParam = 0;

    TREELIST_COL_DATA data;
    data.rc.left = 0;
    data.rc.right = 0;
    data.rc.top = 0;
    data.rc.bottom = data.rc.top + m_data->nHeadHeight;
    int len = m_header.size();
    if (len > 0)    // 已经有成员, 往里新增
    {
        if (iInsertAfter >= len)        // 加入到最后
        {
            TREELIST_COL_DATA& prev = m_header.at(len - 1);
            data.rc.left = prev.rc.right;
        }
        else if (iInsertAfter == 0)     // 插入最前面
        {
            data.rc.left = 0;
            for (int i = 0; i < len; i++)
            {
                TREELIST_COL_DATA& next = m_header.at(i);
                next.rc.left += nWidth;     // 后面每列往后移第一列的宽度
                next.rc.right += nWidth;
            }
        }
        else
        {
            // 往中间插入成员
            // 因为还没有网数组里增加成员, 所以 iInsertAfter 指向旧数组的成员, 这个成员开始需要往后移动
            TREELIST_COL_DATA& prev = m_header.at(iInsertAfter);    // 取插入位置的成员
            data.rc.left = prev.rc.left;
            for (int i = iInsertAfter; i < len; i++)    // 从插入位置往后每个成员增加当前列的宽度
            {
                TREELIST_COL_DATA& next = m_header.at(i);
                next.rc.left += nWidth;     // 后面每列往后移这一列的宽度
                next.rc.right += nWidth;
            }
        }
    }
    ++len;
    if (m_headerIndex)delete[] m_headerIndex;
    m_headerIndex = new int[len];
    for (int i = 0; i < len; i++)
        m_headerIndex[i] = i;

    data.rc.right = data.rc.left + nWidth;
    data.width = nWidth;
    data.title = lpsz;
    m_header.Insert(data, iInsertAfter);
    index = (int)(DWORD)(LONG_PTR)SendMessageW(m_data->hHead, HDM_INSERTITEM, (WPARAM)iInsertAfter, (LPARAM)&hdi);
    return index;
}

bool CTreeList::SetItem(HTREEITEM hItem, const TLVITEM* itemData, int indexCol)
{
    if (!hItem || !m_data || !itemData)return false;
    LPHTREELISTITEM data = (LPHTREELISTITEM)CTreeView::GetItemNum(hItem);
    if (indexCol < 0 || indexCol >= data->count)
        return false;
    int index = indexCol;
    HTREELISTITEM_SUBDATA& drawItem = data->draw[index];
    if (itemData->mask & TLVIF_TEXT)
    {

        drawItem.title = itemData->text;
        if (itemData->text)
        {
            _canvas_calctextsize(m_data->hCanvas, m_data->hFont,
                itemData->text, wcslen(itemData->text),
                DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX,
                9999, 30, &drawItem.rcText.right, &drawItem.rcText.bottom);
        }
        else
        {
            drawItem.rcText.right = 0;
            drawItem.rcText.bottom = 0;
        }
    }
    if (itemData->mask & TLVIF_PARAM)
        drawItem.param = itemData->param;
    if (itemData->mask & TLVIF_TYPE)
        drawItem.type = itemData->type;
    //if (itemData->mask & TLVIF_)
    //    drawItem.type = itemData->type;
    if (itemData->mask & TLVIF_CRTEXT)
    {
        _brush_destroy(drawItem.hbrText);
        drawItem.crText = itemData->crText;
        drawItem.hbrText = _brush_create(itemData->crText.argb);
    }
    if (itemData->mask & TLVIF_BORDER)
    {
        drawItem.border = itemData->border;
        data->isManualBorder = itemData->isManualBorder;
    }
    if (itemData->mask & TLVIF_ABSLEFT)
    {
        //data->isAbsLeft = itemData->isAbsLeft;
        drawItem.isAbsLeft = itemData->isAbsLeft;
        drawItem.left = itemData->left;
    }
    if (itemData->mask & TLVIF_ABSCOL)
    {
        //data->isAbsWidth = itemData->isAbsWidth;
        drawItem.isAbsWidth = itemData->isAbsWidth;
        drawItem.width = itemData->width;
    }
    if(itemData->mask & TLVIF_TEXTFORMAT)
    {
        drawItem.format = itemData->format;
    }
    return true;
}

bool CTreeList::GetItem(HTREEITEM hItem, LPTLVITEM itemData)
{
    if (!hItem || !m_data || !itemData)return false;
    LPHTREELISTITEM data = (LPHTREELISTITEM)CTreeView::GetItemNum(hItem);
    if (itemData->indexCol < 0 || itemData->indexCol >= data->count)
        return false;
    int index = itemData->indexCol;
    HTREELISTITEM_SUBDATA& drawItem = data->draw[index];
    if ((itemData->mask & TLVIF_TEXT) && itemData->text)
    {
        wstr& str = drawItem.title;
        int len = (int)str.size() + 1;
        if (len > itemData->cchText)
            len = itemData->cchText;
        memcpy(itemData->text, str.c_str(), len * sizeof(wchar_t));
    }
    if (itemData->mask & TLVIF_PARAM)
        itemData->param = data->param;
    if (itemData->mask & TLVIF_TYPE)
        itemData->type = drawItem.type;
    if (itemData->mask & TLVIF_CRTEXT)
        itemData->crText = drawItem.crText;
    if (itemData->mask & TLVIF_BORDER)
    {
        itemData->border = drawItem.border;
        itemData->isManualBorder = data->isManualBorder;
    }
    if (itemData->mask & TLVIF_ABSLEFT)
    {
        itemData->left = drawItem.left;
        itemData->isAbsLeft = drawItem.isAbsLeft;
    }
    if (itemData->mask & TLVIF_ABSCOL)
    {
        itemData->width = drawItem.width;
        itemData->isAbsWidth = drawItem.isAbsWidth;
    }
    if (itemData->mask & TLVIF_TEXTFORMAT)
    {
        itemData->format = drawItem.format;
    }
    return true;
}

bool CTreeList::SetItemText(HTREEITEM hItem, int indexCol, LPCWSTR text)
{
    if (!hItem || indexCol < 0)return false;
    LPHTREELISTITEM data = (LPHTREELISTITEM)CTreeView::GetItemNum(hItem);
    if (!data || indexCol >= data->count)
        return false;
    HTREELISTITEM_SUBDATA& subData = data->draw[indexCol];
    
    subData.title = text;
    if (text)
    {
        //if (wcschr(text, '\n'))
        //    __debugbreak();
        __create_d2d_resouce();
        _canvas_calctextsize(m_data->hCanvas, m_data->hFont, text, wcslen(text),
            DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX,
            9999, 9999, &subData.rcText.right, &subData.rcText.bottom);
    }
    else
    {
        subData.rcText.right = 0;
        subData.rcText.bottom = 0;
    }
    return true;
}

wstr CTreeList::GetItemText(HTREEITEM hItem, int indexCol)
{
    if (!hItem || indexCol < 0)return wstr();
    LPHTREELISTITEM data = (LPHTREELISTITEM)CTreeView::GetItemNum(hItem);
    if (!data || indexCol >= data->count)
        return wstr();
    wstr ret(data->draw[indexCol].title);
    ret.Addref();
    return ret;
}

HIMAGELIST CTreeList::SetImageList(HIMAGELIST hImageList, int type)
{
    m_data->isIcon = hImageList != 0;
    return CTreeView::SetImageList(hImageList, type);
}

wstr CTreeList::GetColText(int index)
{
    int size = m_header.size();
    if (index < 0)index = 0;
    if (index >= size) return wstr();
    TREELIST_COL_DATA& data = m_header[index];
    return data.title;
}

bool CTreeList::SetColText(int index, LPCWSTR text)
{
    int size = m_header.size();
    if (index < 0)index = 0;
    if (index >= size) false;
    TREELIST_COL_DATA& data = m_header[index];
    data.title = text;
    HDITEMW item;
    item.mask = HDI_TEXT;
    item.pszText = (LPWSTR)text;
    Header_SetItem(m_data->hHead, index, &item);
    return true;
}

bool CTreeList::GetColInfo(int index, LPTREELIST_COL_DATA& info)
{
    int size = m_header.size();
    if (index < 0)index = 0;
    if (index >= size) return false;
    TREELIST_COL_DATA& data = m_header[index];
    info = &data;
    return true;
}



LPARAM CTreeList::GetItemColParam(HTREEITEM hItem, int indexCol)
{
    if (!isItem(hItem) || indexCol < 0)return 0;
    LPHTREELISTITEM data = (LPHTREELISTITEM)CTreeView::GetItemNum(hItem);
    if (!data || indexCol >= data->count)
        return 0;
    return data->draw[indexCol].param;
}

bool CTreeList::SetItemColParam(HTREEITEM hItem, int indexCol, LPARAM lParam)
{
    if (!isItem(hItem) || indexCol < 0)return false;
    LPHTREELISTITEM data = (LPHTREELISTITEM)CTreeView::GetItemNum(hItem);
    if (!data || indexCol >= data->count) return false;
    data->draw[indexCol].param = lParam;
    return true;
}

LPARAM CTreeList::GetItemParam(HTREEITEM hItem)
{
    if (!hItem)return 0;
    LPHTREELISTITEM data = (LPHTREELISTITEM)CTreeView::GetItemNum(hItem);
    if (!data) return 0;
    return data->param;
}

bool CTreeList::isItem(HTREEITEM hItem)
{
    if (!hItem)return false;
    if (!m_hItems)
        m_hItemsCount = EnumItem(m_hItems);
    
    for (int i = 0; i < m_hItemsCount; i++)
    {
        if (hItem == m_hItems[i])
            return true;
    }
    return false;
}

bool CTreeList::SetItemParam(HTREEITEM hItem, LPARAM lParam)
{
    if (!hItem)return false;
    LPHTREELISTITEM data = (LPHTREELISTITEM)CTreeView::GetItemNum(hItem);
    if (!data) return false;
    data->param = lParam;
    return true;
}

bool CTreeList::UpDateItem(HTREEITEM hItem)
{
    if (!m_data || !hItem)return false;
    if (!hItem) hItem = m_data->index;
    if (!hItem)return false;
    RECT rc = { 0 };
    TreeView_GetItemRect(m_data->hTree, hItem, &rc, 0);
    return ::InvalidateRect(m_data->hTree, &rc, 0);
}

int CTreeList::SetIndex(HTREEITEM hItem)
{
    if (!isItem(hItem))return 0;
    int ret = CTreeView::SetIndex(hItem);
    if (m_data->index)
    {
        LPHTREELISTITEM data = (LPHTREELISTITEM)GetItemNum(m_data->index);
        if (data)
        {
            int state = data->state;
            data->state = 0;
            UpDateItem(m_data->index);
        }
    }
    m_data->index = hItem;
    SetUIItem(hItem, (STATE)(STATE::FOCUS | STATE::DOWN), false, true);
    return ret;
}

void CTreeList::SetColWidth(int index, int width)
{
    HDITEMW item;
    item.mask = HDI_WIDTH;
    item.cxy = width;
    Header_SetItem(m_data->hHead, index, &item);
}
