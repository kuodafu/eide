#include "CPropertyTree.h"
#include <MemDC.h>
#include <assist/assist.h>
#include <control/CButton.h>
#include <CFileDialog.h>
#include <control/DrawScroll.h>

//#include <vfw.h>
//#pragma comment(lib, "Vfw32.lib")


#define TREESUB_ID_EDIT     1000
#define TREESUB_ID_LIST     1001
#define TREESUB_ID_BTN      1002

#define COLORBLOCK_WIDTH    20     // 颜色小框的宽度, 编辑框需要加上这个宽度

struct SUB_OLDPROC_DATA
{
    WNDPROC oldProc;
    CWndBase* pThis;
};

static std::map<HWND, WNDPROC> s_map_parent;    // 父窗口的子类化, 子类化父窗口前先判断有没有子类化过
const unsigned char s_img_tree_exp[] = {
0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x80,
0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0xDE, 0x58, 0x77, 0x88, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47,
0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC,
0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0E, 0xC3, 0x00, 0x00, 0x0E, 0xC3, 0x01, 0xC7,
0x6F, 0xA8, 0x64, 0x00, 0x00, 0x03, 0x5F, 0x49, 0x44, 0x41, 0x54, 0x68, 0x43, 0xED, 0x98, 0xC1, 0x4B, 0xDC, 0x40, 0x14,
0xC6, 0x67, 0x92, 0x98, 0xAB, 0x1B, 0xA1, 0x8A, 0x50, 0xB0, 0x2D, 0xB6, 0x5E, 0x0A, 0x16, 0x3C, 0x89, 0x1E, 0xEC, 0xD1,
0x8B, 0x0A, 0xA2, 0x07, 0x2F, 0xEE, 0xBD, 0x95, 0xB5, 0xF8, 0x07, 0x2C, 0xFB, 0x07, 0x48, 0x29, 0xB5, 0xF4, 0x24, 0xB4,
0x97, 0x82, 0x8A, 0x82, 0xE0, 0xDD, 0x83, 0x7A, 0xA9, 0xA8, 0x15, 0x8A, 0x58, 0x53, 0x2A, 0x5E, 0x84, 0xB6, 0x62, 0x76,
0xAF, 0xBB, 0x71, 0xD3, 0xF7, 0x8D, 0x13, 0x9B, 0x9A, 0x26, 0x6B, 0xB2, 0x6D, 0xF6, 0x60, 0x7E, 0x10, 0x32, 0x79, 0xF3,
0x66, 0x5F, 0x92, 0xEF, 0xCB, 0x64, 0xB2, 0x2C, 0x25, 0x25, 0x25, 0xC5, 0x4F, 0x2E, 0x97, 0xEB, 0x1D, 0x1B, 0x1B, 0x53,
0xE5, 0x61, 0xE2, 0x18, 0xAF, 0xCD, 0x5E, 0xB6, 0xE8, 0x34, 0xAC, 0xFE, 0x6D, 0x41, 0x91, 0x7B, 0x1F, 0x9C, 0xF3, 0xF9,
0xF6, 0xF6, 0xF6, 0x95, 0x7C, 0x3E, 0xAF, 0xC9, 0x50, 0xB2, 0x70, 0x67, 0xDE, 0xF8, 0x69, 0xAE, 0xB0, 0xBC, 0x93, 0x78,
0xFD, 0xE1, 0xE1, 0xE1, 0x2C, 0x6D, 0xEF, 0xE4, 0x61, 0xE2, 0x34, 0xCF, 0x7D, 0xC9, 0x1A, 0x6F, 0x8E, 0x12, 0xA9, 0x1F,
0x68, 0x00, 0x55, 0x55, 0x07, 0xC9, 0x04, 0x3D, 0x96, 0x65, 0x2D, 0x37, 0xC2, 0x04, 0x0E, 0xD3, 0x06, 0x69, 0xD7, 0x63,
0xB4, 0x9A, 0xCB, 0x49, 0x9B, 0x80, 0xAE, 0x3B, 0xA7, 0x28, 0xCA, 0x64, 0xA3, 0x4C, 0xA0, 0x50, 0x7D, 0x3A, 0x87, 0xC9,
0x24, 0x4C, 0x10, 0x68, 0x80, 0xD9, 0xD9, 0xD9, 0x13, 0x4D, 0xD3, 0xFA, 0x1A, 0x65, 0x82, 0xE2, 0xF3, 0x07, 0x27, 0x8E,
0xA3, 0xF5, 0x51, 0x33, 0x71, 0x13, 0x38, 0x8E, 0xF3, 0x94, 0xB6, 0x4F, 0x51, 0x4D, 0x80, 0x99, 0x63, 0x64, 0x64, 0x64,
0x8F, 0xF6, 0x19, 0x19, 0xBA, 0x02, 0x31, 0xD9, 0x97, 0x95, 0xA1, 0x40, 0xAA, 0xE5, 0x26, 0x51, 0x3F, 0xAA, 0x09, 0xE4,
0xCC, 0xB1, 0xD7, 0xFC, 0xF2, 0xD8, 0x57, 0x1F, 0x31, 0xD1, 0x47, 0x39, 0x32, 0x24, 0x08, 0x34, 0x00, 0x88, 0x62, 0x02,
0xAC, 0x19, 0xA6, 0xA7, 0xA7, 0x0F, 0x68, 0x7F, 0x4F, 0x86, 0x7C, 0xCC, 0xCC, 0xCC, 0x74, 0xC8, 0x9C, 0x5E, 0x19, 0x0A,
0x25, 0x8A, 0x09, 0xB0, 0x66, 0x30, 0xE6, 0x8E, 0x0E, 0x9A, 0xDF, 0x1E, 0x07, 0xD6, 0xCF, 0xCC, 0x7D, 0xEB, 0x40, 0x8E,
0x58, 0x5F, 0x84, 0xB0, 0xBA, 0xBA, 0x5A, 0x8C, 0x69, 0x82, 0x01, 0xBA, 0x57, 0x4F, 0x68, 0x5B, 0xF7, 0x9A, 0x00, 0x6D,
0xC4, 0xD0, 0x47, 0x87, 0x03, 0x97, 0xD1, 0x60, 0x4A, 0x2F, 0xEE, 0x17, 0xE3, 0x98, 0x80, 0x66, 0x0E, 0x51, 0x5F, 0xD1,
0x2B, 0xEB, 0x5E, 0x13, 0xA0, 0x8D, 0x98, 0xE8, 0xA3, 0x1C, 0x19, 0x16, 0x84, 0x1A, 0x00, 0x78, 0x4D, 0x50, 0x2C, 0x16,
0x3F, 0xC8, 0xB0, 0x8F, 0x4C, 0x26, 0xB3, 0x5D, 0xAD, 0x56, 0x4D, 0x6A, 0x6E, 0x4C, 0x4D, 0x4D, 0xDD, 0xBD, 0x8C, 0xFE,
0x06, 0xE2, 0xDB, 0xB6, 0xBD, 0x85, 0x9C, 0xD3, 0xD3, 0xD3, 0x8F, 0x32, 0x5C, 0x13, 0xAF, 0x09, 0x5A, 0xDA, 0xBE, 0x06,
0xD6, 0xB7, 0xCE, 0x3A, 0xB7, 0x19, 0x67, 0xA6, 0x52, 0xB5, 0x37, 0x5A, 0x5E, 0x1D, 0xF9, 0xEA, 0x43, 0x7C, 0xCE, 0xED,
0x2D, 0xE4, 0x58, 0x6D, 0x9D, 0x35, 0xEB, 0xC7, 0x31, 0x01, 0x8D, 0xC9, 0xD2, 0xF5, 0xBD, 0xC7, 0x8D, 0xA6, 0x4D, 0x98,
0xC0, 0x2B, 0x3E, 0xFA, 0x90, 0x23, 0xD3, 0x43, 0x89, 0x63, 0x02, 0xEB, 0xD9, 0xA3, 0x2C, 0xE5, 0x8B, 0xFA, 0xAE, 0x09,
0xBC, 0xE2, 0xA3, 0x0F, 0x39, 0x32, 0x5D, 0x50, 0xD3, 0x00, 0xA0, 0x5C, 0x2E, 0x5F, 0xD0, 0x0F, 0x54, 0x68, 0x6B, 0x92,
0x21, 0x1F, 0x85, 0x42, 0xC1, 0x36, 0x0C, 0x63, 0x94, 0x8A, 0xEC, 0xD2, 0x0D, 0xDB, 0xF4, 0x9A, 0xC0, 0x15, 0x9F, 0xFA,
0x76, 0x90, 0xB3, 0xB4, 0xB4, 0x74, 0x21, 0xBB, 0x6E, 0x84, 0x72, 0x61, 0x23, 0xBF, 0xE2, 0x30, 0x27, 0xB0, 0x3E, 0x2B,
0x70, 0xDB, 0xFA, 0xF1, 0x70, 0x94, 0x26, 0xF0, 0x5D, 0x9A, 0x27, 0x36, 0xBD, 0x26, 0xB8, 0x12, 0x9F, 0xB1, 0x1D, 0x91,
0x33, 0xCE, 0x23, 0xD5, 0x8F, 0xC2, 0x75, 0x13, 0xC8, 0x2D, 0x92, 0xF8, 0xF5, 0x70, 0xDD, 0x04, 0x61, 0xE2, 0x03, 0x2E,
0xF7, 0x81, 0x40, 0x48, 0x08, 0x4A, 0x3F, 0x70, 0x48, 0xE2, 0x0D, 0x91, 0xD0, 0x65, 0xD9, 0xF5, 0x57, 0xF0, 0x9A, 0xC0,
0xEB, 0x82, 0x8A, 0x76, 0xD3, 0x45, 0xF7, 0xEB, 0xBA, 0xAE, 0x7A, 0xC5, 0x87, 0x51, 0x64, 0xEA, 0x8D, 0x80, 0x90, 0x10,
0x94, 0x4E, 0xF4, 0xF0, 0xFC, 0x4E, 0x79, 0x88, 0x8D, 0x3F, 0x0E, 0xAD, 0x8F, 0xD7, 0x84, 0x78, 0x5D, 0x30, 0xD6, 0xCD,
0x6D, 0xD6, 0x5F, 0x55, 0x35, 0xF5, 0x0F, 0xF1, 0xC9, 0x28, 0x22, 0xAF, 0x06, 0x71, 0x9F, 0x5C, 0x17, 0xCC, 0x18, 0x98,
0x39, 0xD0, 0x8E, 0x33, 0xBE, 0xD6, 0x93, 0x5B, 0x0B, 0xCC, 0x18, 0x34, 0x56, 0xD4, 0x0F, 0x1B, 0x1F, 0x6A, 0x00, 0x8F,
0xF8, 0xFB, 0x51, 0xC4, 0x93, 0x26, 0x58, 0xA3, 0xB1, 0x5D, 0x34, 0xB6, 0xA9, 0x5E, 0xF1, 0xA9, 0xB9, 0x1F, 0x45, 0x3C,
0x98, 0xA0, 0xA5, 0xD5, 0x5C, 0x73, 0x18, 0xEB, 0xA2, 0x23, 0xCC, 0x1A, 0x89, 0x8A, 0xEF, 0x02, 0x13, 0x60, 0x9F, 0xB4,
0xF8, 0x2E, 0xEE, 0x6B, 0x23, 0x6C, 0x7C, 0xA0, 0x01, 0xE4, 0x62, 0x6E, 0x83, 0x4E, 0x60, 0x37, 0x8E, 0x78, 0x64, 0x02,
0xBD, 0x54, 0x2A, 0x2D, 0xD0, 0xF8, 0x0A, 0xAD, 0x0F, 0x26, 0xA2, 0x8E, 0xC7, 0x62, 0x0E, 0xEF, 0x73, 0x4C, 0xE9, 0x91,
0xC4, 0x77, 0x59, 0xFC, 0xAC, 0x1B, 0x67, 0xFA, 0x02, 0x67, 0xBC, 0x72, 0xFE, 0xBD, 0x73, 0x22, 0xCA, 0x78, 0xAC, 0xD6,
0xEB, 0x15, 0xBF, 0x1E, 0xB0, 0x5A, 0xAF, 0x57, 0xFC, 0x9B, 0x12, 0x68, 0x00, 0xAC, 0xD6, 0xE9, 0x06, 0x98, 0x71, 0xC4,
0xFF, 0x17, 0x60, 0xB5, 0x2E, 0x16, 0x6C, 0x71, 0xC4, 0xAF, 0x13, 0x7A, 0x72, 0x71, 0xD3, 0x07, 0x1A, 0x21, 0x3E, 0xC0,
0xA7, 0x1A, 0x56, 0xEB, 0xFF, 0x5B, 0xFC, 0x50, 0xF0, 0xA9, 0x96, 0xF4, 0xB7, 0xBF, 0x17, 0xF1, 0xA9, 0x96, 0xF0, 0x1F,
0x40, 0x29, 0x29, 0x29, 0x29, 0xB7, 0x09, 0xC6, 0x7E, 0x01, 0xED, 0xC5, 0x40, 0xD5, 0x13, 0x9E, 0x4A, 0xF8, 0x00, 0x00,
0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
};
inline LPEX_IMAGE _get_checkbox_radio_hImage()
{
    static LPEX_IMAGE hImage;
    if (!hImage)
    {
        const BYTE pImageData[] =
        {
            0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x84,
            0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0xD7, 0xB3, 0xD7, 0xF2, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47,
            0x42, 0x00, 0xAE, 0xCE, 0x1C, 0xE9, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4D, 0x41, 0x00, 0x00, 0xB1, 0x8F, 0x0B, 0xFC,
            0x61, 0x05, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0E, 0xC3, 0x00, 0x00, 0x0E, 0xC3, 0x01, 0xC7,
            0x6F, 0xA8, 0x64, 0x00, 0x00, 0x06, 0x2B, 0x49, 0x44, 0x41, 0x54, 0x68, 0x43, 0xED, 0x99, 0x6B, 0x6C, 0x14, 0x55, 0x14,
            0xC7, 0xCF, 0xB9, 0xBB, 0x5B, 0xFC, 0x80, 0xE2, 0x23, 0x4A, 0x63, 0x50, 0x12, 0x90, 0x87, 0xD4, 0x50, 0x03, 0x18, 0x0D,
            0x5D, 0x5E, 0x11, 0x43, 0x9A, 0x18, 0x12, 0xD4, 0xE2, 0x2B, 0x42, 0x50, 0x0A, 0xA5, 0x96, 0x87, 0x44, 0x68, 0x4C, 0x50,
            0x1E, 0x21, 0x22, 0xA8, 0x11, 0xB5, 0xDA, 0x07, 0x3E, 0x12, 0x35, 0x44, 0x21, 0xC6, 0x44, 0xFC, 0xE0, 0x07, 0x14, 0x0B,
            0xDB, 0x26, 0xF8, 0x00, 0x2C, 0x50, 0x20, 0x10, 0x3E, 0x28, 0x2A, 0x58, 0x45, 0x45, 0x88, 0xD2, 0xD9, 0xBD, 0xD7, 0xFF,
            0x99, 0xBD, 0xBB, 0x6C, 0xDB, 0xDD, 0xCE, 0x4C, 0xBB, 0x25, 0x84, 0xF8, 0x4B, 0x36, 0x73, 0xCF, 0xB9, 0xF7, 0xEC, 0xCC,
            0xDC, 0xFB, 0x9F, 0x3B, 0xE7, 0xDE, 0xE1, 0x68, 0x5D, 0x7C, 0x25, 0x31, 0x4D, 0x26, 0x72, 0x7F, 0x41, 0xF8, 0x8A, 0x0C,
            0x7E, 0x02, 0xD3, 0x2A, 0xF7, 0x18, 0x00, 0x43, 0xE6, 0x3C, 0x1B, 0x5E, 0x17, 0xAB, 0x08, 0xAF, 0x1E, 0x5F, 0x17, 0x7F,
            0x50, 0x11, 0x8D, 0xB4, 0x55, 0xBE, 0xD0, 0x44, 0x87, 0x9B, 0x2B, 0xC2, 0x1F, 0x59, 0x33, 0x2B, 0xD1, 0x5A, 0x3D, 0x4A,
            0x8E, 0xB1, 0x05, 0xAA, 0xD5, 0x75, 0x04, 0xA4, 0x27, 0xF1, 0x25, 0xB5, 0xCE, 0x0C, 0x56, 0xBC, 0x8C, 0x8C, 0xB9, 0xD1,
            0x30, 0x9D, 0xC3, 0x3D, 0x1A, 0xF4, 0x4F, 0x91, 0xAD, 0xEE, 0x16, 0x63, 0x4C, 0x0B, 0x13, 0xED, 0x47, 0x87, 0x1E, 0x95,
            0x7E, 0xB1, 0xEE, 0xAC, 0x94, 0xD4, 0x3B, 0xA5, 0xE8, 0x84, 0x52, 0x66, 0x1E, 0x81, 0xC8, 0x62, 0x62, 0x1E, 0x68, 0xAB,
            0x72, 0x82, 0xFF, 0x6F, 0x43, 0xFB, 0x83, 0x38, 0x0E, 0x46, 0xFB, 0xE3, 0x6C, 0xCC, 0xA1, 0x58, 0x45, 0x64, 0xA1, 0xAD,
            0x76, 0x11, 0x41, 0x1C, 0x40, 0x83, 0xAA, 0xA6, 0x05, 0x91, 0xE4, 0xE0, 0xFA, 0x04, 0x37, 0x3E, 0x19, 0x7F, 0x5E, 0xE3,
            0xF7, 0x66, 0xB3, 0x21, 0xA2, 0x30, 0x86, 0x67, 0x2B, 0xA6, 0x0F, 0xAD, 0x2B, 0x10, 0xDA, 0xD0, 0x43, 0x9D, 0x45, 0x11,
            0xAD, 0x8F, 0x97, 0x19, 0x43, 0x4B, 0x88, 0xCD, 0x68, 0x9C, 0xE0, 0x84, 0xEB, 0x64, 0x1A, 0x44, 0x86, 0x5B, 0x30, 0x36,
            0xEB, 0x9B, 0xE7, 0x47, 0x3E, 0x75, 0x7D, 0x39, 0xE8, 0x4D, 0x7C, 0xB4, 0xCE, 0xF9, 0x00, 0x1D, 0x7D, 0x15, 0xC7, 0xF5,
            0x0B, 0x89, 0x7E, 0xE1, 0xFD, 0xCD, 0x73, 0xD5, 0xDF, 0xE2, 0x9F, 0x54, 0x63, 0xFA, 0x27, 0x42, 0x4E, 0xB1, 0x51, 0x3C,
            0x8B, 0x89, 0xE7, 0xB9, 0x8D, 0x3D, 0x30, 0x44, 0x7B, 0xC3, 0xAC, 0xA6, 0x37, 0xCE, 0x53, 0xC9, 0x6B, 0xC8, 0xC0, 0x9E,
            0xE7, 0x51, 0x6B, 0x7A, 0x82, 0xFF, 0x5A, 0x18, 0x56, 0xEA, 0xF3, 0xC6, 0x72, 0x75, 0xCC, 0xBA, 0x28, 0xDA, 0xA0, 0x87,
            0x93, 0x89, 0x97, 0xE1, 0xBE, 0x1E, 0x08, 0xC5, 0xCF, 0x97, 0x36, 0x56, 0xF5, 0x3F, 0x29, 0x7E, 0x46, 0x07, 0x98, 0xD8,
            0xFC, 0x30, 0x84, 0x19, 0x1C, 0x89, 0xB5, 0xC5, 0x9E, 0x63, 0x30, 0xBB, 0xF4, 0x60, 0x86, 0x71, 0x41, 0x6C, 0xE6, 0x93,
            0x84, 0xEB, 0xD9, 0x02, 0xDF, 0x18, 0xA3, 0xCC, 0xB2, 0xF6, 0xF3, 0xA1, 0x9D, 0xDF, 0x2C, 0x54, 0xBF, 0x8B, 0xFF, 0x8E,
            0xD7, 0xF5, 0x75, 0x05, 0xFD, 0x12, 0x13, 0x59, 0xF3, 0x8B, 0x38, 0xD7, 0x1E, 0xDC, 0xEF, 0x4C, 0x37, 0xA0, 0x13, 0xBD,
            0x89, 0x77, 0xFB, 0xC2, 0x50, 0x25, 0xAE, 0xA7, 0xD6, 0xBA, 0xB2, 0x12, 0xDD, 0xD4, 0x3E, 0x96, 0xB4, 0xFA, 0xD6, 0x9A,
            0xDE, 0x84, 0xD5, 0x0D, 0xB1, 0x27, 0x54, 0x9B, 0xB5, 0x82, 0xF5, 0xB9, 0xA1, 0x83, 0x5C, 0xA0, 0x4A, 0x77, 0x3D, 0xAE,
            0x7E, 0xB4, 0x9E, 0x2E, 0x4C, 0xA8, 0x6B, 0x9F, 0x68, 0x58, 0x35, 0x3A, 0xEA, 0x9F, 0xC2, 0xDD, 0xE5, 0x57, 0x9E, 0xC2,
            0x4C, 0x7D, 0x79, 0x60, 0x07, 0xF3, 0x20, 0x06, 0xE4, 0x96, 0xA6, 0x79, 0x91, 0x4F, 0x52, 0x83, 0x29, 0x48, 0x59, 0x7C,
            0x52, 0x27, 0x6D, 0x30, 0x2B, 0x76, 0x19, 0x90, 0xDE, 0xC4, 0x97, 0xD4, 0xC7, 0x9F, 0xC6, 0xA1, 0xDA, 0x4B, 0x0C, 0x42,
            0xAC, 0xBC, 0xE0, 0x3B, 0x1C, 0xB2, 0x0A, 0x32, 0x1B, 0xC6, 0xD1, 0x9B, 0x6D, 0x11, 0x33, 0x43, 0x7C, 0x91, 0x2D, 0x7A,
            0x02, 0x75, 0xFE, 0x85, 0xEB, 0xB9, 0xAD, 0x3B, 0x31, 0x08, 0xBB, 0x2A, 0x0A, 0x76, 0x3A, 0x4A, 0x15, 0x46, 0xF4, 0x15,
            0x9F, 0x89, 0x7D, 0x51, 0x05, 0x31, 0x7B, 0x0C, 0xD3, 0x9C, 0xB1, 0x3D, 0x9A, 0x8C, 0xBA, 0x05, 0x83, 0x29, 0xD3, 0xFC,
            0xCD, 0xE8, 0x80, 0x6E, 0xDF, 0xBB, 0x82, 0xB4, 0xC1, 0x23, 0x16, 0x47, 0xE7, 0xA6, 0xA7, 0xEE, 0xCE, 0xF1, 0xD1, 0xDA,
            0xF6, 0x4A, 0xF8, 0xF6, 0xE1, 0x67, 0x30, 0x3D, 0x6B, 0xB7, 0x0C, 0x9F, 0xD4, 0x75, 0x8E, 0x97, 0xFC, 0x07, 0x77, 0x54,
            0x8A, 0x59, 0x63, 0x83, 0xD8, 0x7E, 0x40, 0xDB, 0xAD, 0xF8, 0x0F, 0x77, 0x00, 0xBC, 0x60, 0xA6, 0xA9, 0x38, 0x7F, 0x65,
            0x74, 0x93, 0x2E, 0xC1, 0xEC, 0xF4, 0xAA, 0x75, 0x7B, 0x82, 0xDC, 0xE5, 0x29, 0x5B, 0xF4, 0x64, 0x77, 0xB9, 0x3A, 0x85,
            0x88, 0xCD, 0xB8, 0xA7, 0xA5, 0x17, 0x4D, 0x10, 0x0F, 0x8F, 0x66, 0x9A, 0x3B, 0x4E, 0xD1, 0xDD, 0x43, 0x15, 0x0D, 0xB9,
            0x36, 0xBF, 0xA2, 0x40, 0xE7, 0x2E, 0x26, 0x65, 0xD6, 0x5B, 0xD3, 0x13, 0x56, 0x6A, 0x2D, 0x12, 0xBE, 0x59, 0xD6, 0xEC,
            0x10, 0xEF, 0x0E, 0xBC, 0x52, 0x6F, 0xA0, 0x58, 0x2C, 0x36, 0x46, 0x44, 0x2E, 0xB6, 0x58, 0x7C, 0x29, 0x51, 0x64, 0xC6,
            0x2B, 0x32, 0xCB, 0x98, 0xD4, 0xB3, 0x52, 0x0E, 0x82, 0x22, 0xBD, 0xCE, 0x16, 0x3D, 0x81, 0x58, 0xAB, 0x4C, 0x22, 0xF1,
            0x88, 0x35, 0xBD, 0x31, 0x24, 0xB3, 0xD9, 0xBB, 0xD6, 0xF2, 0x05, 0x44, 0xFA, 0x0A, 0xFA, 0x61, 0x42, 0x5E, 0x05, 0x31,
            0x65, 0x48, 0xF6, 0x81, 0xBE, 0xBF, 0x88, 0xA9, 0xF2, 0x2E, 0x45, 0x3F, 0x9D, 0x21, 0x5A, 0xFD, 0xA5, 0xA6, 0xE3, 0xA7,
            0x71, 0xEA, 0x3C, 0x82, 0x6C, 0x79, 0x34, 0x87, 0x42, 0x31, 0x6B, 0x7A, 0x13, 0xA2, 0xDD, 0x12, 0x63, 0xAD, 0x8E, 0xF1,
            0x4A, 0xE5, 0x4E, 0xFA, 0x52, 0x75, 0x99, 0xF1, 0xCC, 0x63, 0x95, 0xC3, 0x2D, 0x6E, 0x39, 0x00, 0xCA, 0xF9, 0xC3, 0x77,
            0x0C, 0x24, 0x79, 0x2B, 0x74, 0xE9, 0xAE, 0x78, 0x7C, 0xF2, 0xBD, 0x3D, 0x06, 0x82, 0xC9, 0x1C, 0xCB, 0x9B, 0x20, 0x96,
            0x4F, 0x54, 0xB4, 0x66, 0xAA, 0xEA, 0x22, 0x8A, 0x7B, 0x47, 0x32, 0x2D, 0x29, 0x51, 0xD4, 0x76, 0x8E, 0x68, 0x0D, 0xC4,
            0x70, 0xF4, 0xB7, 0xFC, 0x8A, 0x61, 0xFC, 0x5B, 0x7A, 0x04, 0x9E, 0xD6, 0x93, 0x99, 0x89, 0x97, 0x17, 0xD2, 0x56, 0x62,
            0x24, 0x36, 0x4B, 0x7C, 0x72, 0x66, 0xC8, 0x86, 0x15, 0x41, 0x66, 0x3C, 0xEE, 0xA6, 0xB5, 0xB1, 0x8A, 0xCF, 0xBA, 0xF5,
            0x01, 0x68, 0xAC, 0x1A, 0x78, 0x56, 0xF2, 0x11, 0x6B, 0x7A, 0x82, 0x95, 0xA0, 0xFF, 0xD5, 0x1C, 0x53, 0x8F, 0x96, 0xD9,
            0x86, 0x39, 0x7F, 0x82, 0xF8, 0xE5, 0x4C, 0x72, 0xA0, 0x33, 0x45, 0x31, 0x6D, 0x18, 0x53, 0x35, 0x84, 0xF2, 0xE7, 0xBF,
            0x44, 0xAB, 0xBE, 0xD0, 0xD4, 0xFA, 0x6B, 0x7E, 0xC5, 0x70, 0x29, 0xC0, 0xB2, 0xB6, 0xB8, 0x4C, 0xC0, 0x92, 0x78, 0x50,
            0xDE, 0x04, 0xF1, 0xFE, 0x3E, 0x43, 0x1B, 0x9B, 0xB5, 0x5B, 0x4E, 0x89, 0x62, 0xC5, 0x14, 0x45, 0xE7, 0xDA, 0x89, 0x56,
            0x6E, 0xD7, 0xD4, 0x72, 0xB2, 0x6F, 0xFA, 0x0D, 0x6B, 0xFD, 0x23, 0x18, 0x94, 0xC2, 0xE8, 0xDB, 0xFA, 0x7A, 0xEB, 0xF2,
            0x44, 0xDA, 0x4A, 0x8C, 0xC4, 0x66, 0x89, 0xCF, 0x3D, 0xDD, 0x72, 0xF2, 0xD5, 0x90, 0x19, 0x8F, 0x5E, 0x2C, 0x92, 0x7D,
            0x06, 0xB7, 0x3E, 0x00, 0x93, 0x6A, 0x4E, 0xF5, 0x97, 0x58, 0x6B, 0x7A, 0x22, 0x1B, 0x4A, 0xB6, 0xE8, 0x8D, 0xA1, 0x20,
            0xAF, 0x97, 0x0B, 0x18, 0xFD, 0x58, 0x5E, 0x73, 0x88, 0x8F, 0x0F, 0x18, 0xDA, 0xB0, 0xF3, 0x82, 0x28, 0x9C, 0x04, 0xD1,
            0x73, 0x10, 0xC3, 0x9E, 0x9F, 0xFB, 0xF6, 0x21, 0xC2, 0x54, 0xD7, 0x42, 0x09, 0xBA, 0xD3, 0x9A, 0x9E, 0x20, 0x41, 0x8B,
            0xBA, 0x31, 0x96, 0x0E, 0xF1, 0x5A, 0x37, 0xB8, 0xC7, 0x6C, 0xD8, 0xBA, 0x0E, 0xF1, 0xC6, 0x9C, 0x8E, 0x47, 0x9C, 0xDB,
            0xDD, 0x72, 0x00, 0x74, 0xE4, 0x9A, 0x74, 0x0E, 0xE3, 0x05, 0x92, 0xCA, 0x43, 0x78, 0x65, 0x04, 0x79, 0x0D, 0xE4, 0x7E,
            0xED, 0x75, 0x07, 0xF3, 0xE0, 0xBC, 0x0A, 0x42, 0xD8, 0x76, 0xD8, 0xD0, 0xDA, 0x1D, 0x49, 0x51, 0x88, 0x18, 0xBE, 0x3E,
            0xD1, 0xF7, 0x33, 0x2A, 0x9E, 0xD6, 0xF7, 0x8C, 0xD6, 0x2B, 0xAC, 0xE9, 0x8D, 0xE6, 0x6A, 0xBC, 0xD4, 0xD2, 0x4B, 0xB8,
            0xCC, 0xF8, 0xD8, 0x82, 0x82, 0x37, 0x31, 0xF0, 0x4F, 0xA2, 0x98, 0x9C, 0x29, 0x30, 0x12, 0x6E, 0x19, 0x3E, 0xB7, 0x4E,
            0xC8, 0x88, 0x47, 0xED, 0x5C, 0xAC, 0x32, 0x9E, 0x77, 0xFD, 0x01, 0xD0, 0xA4, 0x9E, 0xB1, 0x45, 0x4F, 0x90, 0x54, 0xD6,
            0x20, 0xE9, 0x4D, 0xEF, 0x47, 0x78, 0xC2, 0x34, 0x03, 0x4B, 0xC8, 0x39, 0xD6, 0xF2, 0x05, 0x96, 0xB6, 0xCB, 0x25, 0xA7,
            0xE9, 0xB3, 0x9D, 0x4A, 0x79, 0x65, 0xEC, 0x38, 0xEE, 0x43, 0x0C, 0x79, 0xDA, 0xA9, 0x44, 0x07, 0xC8, 0x66, 0xD1, 0xB6,
            0x94, 0x9D, 0x0B, 0xB4, 0x93, 0x6F, 0x37, 0x45, 0x9D, 0x77, 0x1B, 0x7B, 0x13, 0x5F, 0x52, 0xE7, 0x40, 0x1C, 0xFC, 0x43,
            0x53, 0x45, 0xF8, 0x65, 0xEB, 0xEA, 0x16, 0xF4, 0x5B, 0x19, 0x0E, 0x5B, 0x92, 0x56, 0xF7, 0x40, 0x70, 0xDB, 0xF1, 0xBF,
            0xF7, 0x48, 0x19, 0xE7, 0x5E, 0x84, 0x73, 0xFB, 0xDA, 0x8B, 0x90, 0x8D, 0xA9, 0xA6, 0xF9, 0x91, 0xAB, 0xAD, 0xE9, 0x49,
            0x4A, 0x07, 0x79, 0x9F, 0x21, 0x52, 0xF8, 0x12, 0x43, 0x1E, 0xC1, 0x40, 0x8E, 0x93, 0x81, 0x42, 0xA7, 0x1D, 0x2B, 0x69,
            0x70, 0x66, 0xC8, 0x76, 0xB3, 0xAD, 0x72, 0xB7, 0x9E, 0xC5, 0x27, 0x75, 0xD9, 0xC4, 0x20, 0xF4, 0x26, 0xBE, 0xA9, 0x22,
            0xB2, 0x18, 0x4F, 0xF1, 0x4B, 0xD1, 0x86, 0x78, 0xB9, 0x75, 0xE5, 0xC4, 0xDD, 0xBA, 0xF6, 0x29, 0x06, 0x81, 0x23, 0x2A,
            0xBD, 0xFF, 0x80, 0x6B, 0x7C, 0xCD, 0x16, 0x3D, 0x41, 0x82, 0x38, 0x00, 0xD7, 0x7B, 0x60, 0xC2, 0x3B, 0xFA, 0x26, 0xEB,
            0xCA, 0x0A, 0xF2, 0x9F, 0x42, 0xB4, 0xDB, 0x4B, 0x5A, 0xB9, 0xF9, 0x8C, 0xCC, 0x10, 0x3B, 0x8C, 0x36, 0xAB, 0x7B, 0xF4,
            0x71, 0x4B, 0xF1, 0x4A, 0x14, 0x83, 0x7E, 0x25, 0x4D, 0xD3, 0x17, 0x1F, 0xB7, 0xC6, 0xD7, 0x3B, 0xD3, 0xD9, 0x70, 0x35,
            0xDE, 0x03, 0x5D, 0x3E, 0x4E, 0x61, 0xD0, 0x36, 0x62, 0x30, 0xB7, 0xBA, 0xBE, 0x1C, 0xF4, 0x26, 0x3E, 0x5A, 0x9F, 0xA8,
            0x41, 0x82, 0x31, 0x8A, 0x42, 0x66, 0xC9, 0xE9, 0x01, 0x05, 0x87, 0x5B, 0x67, 0x32, 0x52, 0xEA, 0x4B, 0xF8, 0xE3, 0x96,
            0x4E, 0x2C, 0x35, 0xC4, 0xB3, 0x51, 0x37, 0x2C, 0x75, 0x9E, 0xE4, 0xE7, 0x6F, 0xA2, 0x32, 0x51, 0xBE, 0x38, 0x7C, 0x93,
            0x5C, 0x43, 0x27, 0x3B, 0xE7, 0xFF, 0xCF, 0xDF, 0x69, 0xE4, 0xB3, 0x34, 0x72, 0x92, 0x69, 0xA4, 0x94, 0x03, 0xC5, 0x0E,
            0x85, 0x6B, 0xB8, 0xDF, 0xBE, 0x45, 0xBA, 0x72, 0xB1, 0x3E, 0x7F, 0x17, 0xA1, 0x7D, 0x1B, 0xAE, 0xF3, 0x08, 0xCE, 0x73,
            0x9F, 0xAD, 0x06, 0x44, 0xFF, 0x01, 0x7E, 0x4F, 0x56, 0xE6, 0xCC, 0x49, 0xFF, 0x70, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
            0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
        };
        const int size = sizeof(pImageData) / sizeof(pImageData[0]);
        hImage = _img_create_frommemory(pImageData, size);
    }
    return hImage;
}
static LPEX_IMAGE s_image;      // 每个图标16像素, 0=正常收缩, 1=正常展开, 2=热点收缩, 3=热点展开
static HCURSOR s_hCursor[2];    // 0=普通指针, 1=左右

inline bool Rect_isOverlap(const RECT& rc1, const RECT& rc2)
{
    return (rc1.right > rc2.left && rc2.right > rc1.left
        && rc1.bottom > rc2.top && rc2.bottom > rc1.top);
}

inline static LRESULT CALLBACK WndProc_Partent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WNDPROC oldProc = s_map_parent[hWnd];
    if (!oldProc)oldProc = DefWindowProcW;

    switch (message)
    {
    case WM_COMMAND:
    {
        HWND hWndList = (HWND)lParam;
        if (hWndList) 
        {
            CPropertyTree* ptr = (CPropertyTree*)::GetPropW(hWndList, L"pThis");
            if (ptr)
                return ptr->OnListCommand(oldProc, hWnd, message, wParam, lParam);
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

// 0=...按钮, 1=下拉按钮
inline static void __static_set_type(HWND hWnd, int type)
{
    SetPropW(hWnd, L"type", (HANDLE)(LONG_PTR)type);
}
// 0=...按钮, 1=下拉按钮
inline static int __static_get_type(HWND hWnd)
{
    return (int)(LONG_PTR)GetPropW(hWnd, L"type");
}
inline static LRESULT CALLBACK WndProc_Static(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (ptr)
        {
            SetPropW(hWnd, L"state", (HANDLE)STATE::DOWN);
            InvalidateRect(hWnd, 0, 0);
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONUP:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        POINT pt;
        RECT rc;
        GetCursorPos(&pt);
        GetWindowRect(hWnd, &rc);
        if (PtInRect(&rc, pt))
        {
            SetPropW(hWnd, L"state", (HANDLE)STATE::HOT);
            ptr->OnValueButtonClick(__static_get_type(hWnd));
        }
        else
        {
            SetPropW(hWnd, L"state", (HANDLE)STATE::NORMAL);
        }
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case WM_PAINT:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        return ptr->OnPaintStatic(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_MOUSEMOVE:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        int state = (int)(LONG_PTR)GetPropW(hWnd, L"state");
        if (state == STATE::NORMAL)     // 正常状态
        {
            TRACKMOUSEEVENT tm;
            tm.cbSize = sizeof(tm);
            tm.dwFlags = TME_LEAVE;
            tm.dwHoverTime = HOVER_DEFAULT;
            tm.hwndTrack = hWnd;
            TrackMouseEvent(&tm);
            SetPropW(hWnd, L"state", (HANDLE)STATE::HOT);
            InvalidateRect(hWnd, 0, 0);
        }
        break;
    }
    case WM_MOUSELEAVE:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

        SetPropW(hWnd, L"state", (HANDLE)STATE::NORMAL);
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}


inline static LRESULT CALLBACK WndProc_Edit(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        ULONGLONG* oldTime = (ULONGLONG*)GetPropW(hWnd, L"GetTickCount64");
        if (oldTime)delete oldTime;
        RemovePropW(hWnd, L"GetTickCount64");
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONDOWN:
    {
        ULONGLONG* oldTime = (ULONGLONG*)GetPropW(hWnd, L"GetTickCount64");
        if (!oldTime)
        {
            oldTime = new ULONGLONG(0);
            SetPropW(hWnd, L"GetTickCount64", oldTime);
        }
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (ptr)
        {
            ptr->OnEditLDown((int)wParam, (DWORD)lParam);
        }
        ULONGLONG t = GetTickCount64();
        //_str::dbg("编辑框按下, t = %llu, oldTime = %llu, 相差 = %llu\n", t, *oldTime, t - *oldTime);
        if (t - *oldTime < 300)     // 两次点击小于300毫秒, 触发双击
        {
            *oldTime = t;
            return WndProc_Edit(oldProc, hWnd, WM_LBUTTONDBLCLK, wParam, lParam);
        }
        *oldTime = t;
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONDBLCLK:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        ptr->OnEditDblClk();
        break;
    }
    case WM_SETFOCUS:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        ptr->OnEditSetFocus();
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_KILLFOCUS:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        ptr->OnEditKillFocus();
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_SHOWWINDOW:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        ptr->OnEditShow(wParam);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_CHAR:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        if (wParam == VK_RETURN)
        {
            ptr->OnEditReturn();
            return 0;
        }
        int ret = 0;
        if (ptr->OnEditChar((int)wParam, &ret))
        {
            wParam = ret;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_KEYDOWN:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        if (wParam == VK_DELETE)
        {
            if (ptr->OnItemDelDown())
                return 0;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_KEYUP:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        if (wParam == VK_DELETE)
        {
            ptr->OnItemDelUp(false);
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_MOUSEWHEEL:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        if (ptr->OnEditMouseWheel(HIWORD(wParam), LOWORD(wParam), lParam))
            return 0;
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_RBUTTONDOWN:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        if (ptr->OnEditRButtonDown((int)(DWORD)wParam, lParam))
            return 0;
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_NCCALCSIZE:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (!ptr)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        if (ptr->OnEditNcCalcSize(wParam, lParam))
            return 0;
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
inline static LRESULT CALLBACK WndProc_ListBox(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
    case WM_MOUSEWHEEL:
    case WM_VSCROLL:
    case WM_LBUTTONDOWN:
    {
        InvalidateRect(hWnd, 0, 0);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONUP:
    {
        const int len = (int)SendMessageW(hWnd, LB_GETCOUNT, 0, 0);

        if (len <= 0)ShowWindow(hWnd, SW_HIDE);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_PAINT:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (ptr)
            return ptr->OnPaintListBox(oldProc, hWnd, message, wParam, lParam);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_KILLFOCUS:
    {
        CPropertyTree* ptr = (CPropertyTree*)GetPropW(hWnd, L"pThis");
        if (ptr)
            ptr->OnListBoxKillFocus();
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

    }
    case WM_ERASEBKGND:
        return 1;
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
}

// 长文本对话框
INT_PTR CALLBACK DlgProc_InputText(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    struct DLG_DATA
    {
        wstr* ret;
        HWND hParent;
        CEdit* edit;
        CButton* load;
        CButton* ok;
        CButton* cancel;
    };
#define IDLOAD 100
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        DLG_DATA* data = new DLG_DATA{ 0 };
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, (LONG_PTR)data);
        HANDLE* param = (HANDLE*)lParam;

        data->edit = new CEdit;
        data->load = new CButton;
        data->ok = new CButton;
        data->cancel = new CButton;
        data->ret = (wstr*)param[0];
        data->hParent = (HWND)param[1];
        bool isMultLine = (bool)param[2];   // 是否多行

        int left, top, width, height;
        RECT rc;
        GetWindowRect(data->hParent, &rc);

        width = 500;
        height = 370;
        left = rc.left + (rc.right - rc.left - width) / 2;
        top = rc.top + (rc.bottom - rc.top - height) / 2;
        MoveWindow(hDlg, left, top, width, height, false);
        GetClientRect(hDlg, &rc);

        DWORD style = WS_CHILD | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VISIBLE | WS_VSCROLL | (isMultLine ? (ES_MULTILINE | ES_WANTRETURN) : 0);
        data->edit->create(WS_EX_CLIENTEDGE, 0, style, 8, 8, rc.right - rc.left - 16, rc.bottom - 56, hDlg);

        style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS;
        data->load->create(0, L"导入文本(&I)", style, 8, rc.bottom - rc.top - 40, 92, 32, hDlg, IDLOAD);
        data->ok->create(0, L"确认(&O)", style | BS_DEFPUSHBUTTON, rc.right - rc.left - 88 - 88, rc.bottom - rc.top - 40, 80, 32, hDlg, IDOK);
        data->cancel->create(0, L"取消(&C)", style, rc.right - rc.left - 88, rc.bottom - rc.top - 40, 80, 32, hDlg, IDCANCEL);
        SetWindowPos(hDlg, HWND_TOP, left, top, width, height, SWP_SHOWWINDOW);
        break;
    }
    case WM_ACTIVATE:
    {
        if (GetPropW(hDlg, L"WM_ACTIVATE") == 0)
        {
            SetPropW(hDlg, L"WM_ACTIVATE", (HANDLE)1);
            DLG_DATA* data = (DLG_DATA*)GetWindowLongPtrW(hDlg, GWLP_USERDATA);
            data->edit->SetText(data->ret->c_str());
            data->edit->Send(EM_SETSEL, 0, 0);
            data->edit->SetSel(0, 0);
        }
        return FALSE;
    }
    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id)
        {
        case IDOK:
        {
            DLG_DATA* data = (DLG_DATA*)GetWindowLongPtrW(hDlg, GWLP_USERDATA);
            data->ret->assign(data->edit->GetText());
            EndDialog(hDlg, 1);
            break;
        }
        case IDCANCEL:
        {
            EndDialog(hDlg, 0);
            break;
        }
        case IDLOAD:
        {
            //;*.ini;*.c;*.cpp;*.h;*.htm;*.html;*.js;*.xml;*.css;*.log
            wstr path = Dialog_Open(L"文本文件\0*.txt;*.ini;*.c;*.cpp;*.h;*.htm;*.html;*.js;*.xml;*.css;*.log\0所有文件(*.*)\0*.*\0",
                L"请选择导入的文本文件");
            if (path.empty())break;
            DLG_DATA* data = (DLG_DATA*)GetWindowLongPtrW(hDlg, GWLP_USERDATA);
            FILE* f;
            _wfopen_s(&f, path.c_str(), L"rb+");
            if (f)
            {
                fseek(f, 0, SEEK_END);
                long size = ftell(f);
                fseek(f, 0, SEEK_SET);
                _str buf(size);
                LPSTR ptr = buf.data();
                size = (long)fread_s(ptr, size, 1, size, f);
                fclose(f);
                if (ptr[0])
                {
                    data->edit->SetText(_str::A2W(ptr).c_str());
                }
                else
                {
                    wstr::dbg(L"文件 %s 读取数据失败\n", path.c_str());
                }
                break;
            }
            wstr::dbg(L"文件 %s 打开失败\n", path.c_str());
            break;
        }
        default:
            return (INT_PTR)FALSE;
        }
        break;
    }
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
        return (INT_PTR)GetStockObject(WHITE_BRUSH);
    case WM_CLOSE:
        EndDialog(hDlg, 0);
        break;
    case WM_DESTROY:
    {
        DLG_DATA* data = (DLG_DATA*)GetWindowLongPtrW(hDlg, GWLP_USERDATA);
        if (!data)break;
        delete data->cancel;
        delete data->load;
        delete data->ok;
        delete data->edit;
        delete data;
        SetWindowLongPtrW(hDlg, GWLP_USERDATA, 0);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps = { 0 };
        BeginPaint(hDlg, &ps);
        RECT rc = { 0 };
        GetClientRect(hDlg, &rc);
        FillRect(ps.hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        EndPaint(hDlg, &ps);
        break;
    }
    default:
        return (INT_PTR)FALSE;
    }
    return (INT_PTR)TRUE;
}

CPropertyTree::CPropertyTree() : CTreeView(), m_leftWidth(0), m_hCanvas(0), m_itemHeight(20)
{
    m_offsetLeft = 0;
    m_leftWidth = 500;
    m_defColor = 0;
    m_oldMid = 0;
    m_isCapture = false;
    m_hCursor = 0;
    m_hItemDown = 0;
    m_hItemSel = 0;
    memset(&m_draw, 0, sizeof(m_draw));
    m_isNoDblclk = false;
    m_isFillColorBack = true;
    m_editCaret = 0;
    m_midLineHeight = 0;
    memset(&m_evt, 0, sizeof(m_evt));
    memset(&m_rcCalcText, 0, sizeof(m_rcCalcText));
    memset(&m_rbutton, 0, sizeof(m_rbutton));
    memset(&m_expand, 0, sizeof(m_expand));
}

CPropertyTree::~CPropertyTree()
{
    __delete_d2d_resouce(true);
}

void CPropertyTree::SetItemHeight(int height, bool isUpdate)
{
    if (isUpdate)isUpdate = m_itemHeight != height;
    m_itemHeight = height;
    SetItemHeight(m_itemHeight);
    m_listBox.SetItemHeight(m_itemHeight);
    if (isUpdate)
    {
        InvalidateRect();
        m_listBox.InvalidateRect();
    }
}

bool CPropertyTree::SetIcon(PROPERTY_ICONTYPE type, LPCVOID pIcon, int size)
{
    LPEX_IMAGE hImg = _img_create_frommemory(pIcon, size);
    if (!hImg)
        return false;

    LPEX_IMAGE& oldImage = m_map_icon[type];
    _img_destroy(oldImage);
    oldImage = hImg;
    InvalidateRect();
    return true;
}

LPEX_IMAGE CPropertyTree::GetIcon(PROPERTY_ICONTYPE type)
{
    auto it = m_map_icon.find(type);
    if (it == m_map_icon.end())
        return 0;
    return it->second;
}

LRESULT CPropertyTree::WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //IDrawScroll* pObj =IDrawScroll::GetobjFromHwnd(hWnd);
    //if (!pObj) return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    //LRESULT ret = 0;
    //if (pObj->OnWndProc(oldProc, hWnd, message, wParam, lParam, &ret))
    //{
    //    return ret;
    //}

    switch (message)
    {
    case WM_CTLCOLOREDIT:
    {
        ::SetTextColor((HDC)wParam, m_draw.crTextGdi);
        ::SetBkColor((HDC)wParam, m_draw.crBackgroundGdi);
        return (LRESULT)m_draw.hbrBackgroundGdi;
    }
    case WM_DESTROY:
    {
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_SIZE:
        return OnSize(oldProc, hWnd, message, wParam, lParam);
    case WM_PAINT:
        return OnPaint(oldProc, hWnd, message, wParam, lParam);
    case WM_SETCURSOR:
    {
        if ((HWND)wParam != m_hWnd)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        ::SetCursor(m_hCursor);
        return 1;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDOWN:
    {
        TVHITTESTINFO info = { 0 };
        info.pt.x = GET_X_LPARAM(lParam);
        info.pt.y = GET_Y_LPARAM(lParam);
        Send(TVM_HITTEST, 0, (LPARAM)&info);
        if (info.hItem == 0)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

        if (message == WM_LBUTTONDOWN && m_hCursor == s_hCursor[1])  // 左右, 需要捕获鼠标
        {
            m_oldMid = calcMidLine();
            m_isCapture = true;
            SetCapture();
            return 0;
        }

        auto f = m_items.find(info.hItem);
        if (f == m_items.end())
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);


        if (m_hItemSel)
            InvalidateItem(m_hItemSel);
        m_hItemSel = info.hItem;

        PROPERTY_ITEM& data = f->second;
        if (IsGroup(data.type) && m_hCursor == s_hCursor[0])
        {
            OnItemClick(m_hItemSel, 1, MAKELONG(info.pt.x, info.pt.y));
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        POINT& pt = info.pt;

        if (pt.x < leftSize)
            return 0;


        int col = 0;
        if (!m_hItemSel)
        {
            m_static.hide();
            //m_rbutton.isVisible = false;
            //InvalidateItem(m_hItemSel);
            m_listBox.hide();
            m_edit.hide();
        }

        RECT rcExpand = GetItemRect(m_hItemSel);
        rcExpand.left += 2;
        rcExpand.top += 2;
        rcExpand.right = 18;  // 加减号的矩形位置
        rcExpand.bottom -= 2;
        PROPERTY_TYPE type = data.type;
        if (message == WM_RBUTTONDOWN)
        {
            col = 1;
            InvalidateItem(m_hItemSel);
            if (m_evt.pfnItemRButtonClick && m_evt.pfnItemRButtonClick(m_evt.pfnItemRButtonClickParam, m_hItemSel, type))
                break;

        }
        else if (IsSubItem(m_hItemSel) && PtInRect(&rcExpand, info.pt))
        {
            col = -1;
        }
        else
        {
            int mid = calcMidLine();
            col = info.pt.x > mid ? 2 : 1;
        }

        if (message == WM_LBUTTONDOWN && m_evt.pfnItemChanged)
            m_evt.pfnItemChanged(m_evt.pfnItemChangedParam, m_hItemSel, type, col);
    
        if (!m_hItemSel)
            modify_value_from_edit();
        OnItemClick(m_hItemSel, col, MAKELONG(info.pt.x, info.pt.y));
        if (m_hItemDown && m_hItemDown != m_hItemSel)
            InvalidateItem(m_hItemDown);
        m_hItemDown = 0;



        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONUP:
    {
        //const int x = LOWORD(lParam);
        //if (x < leftSize)
        //{
        //    HTREEITEM hItem = GetIndex();
        //    IPROPERTY_ITEM* item = GetItemData(hItem);
        //    if (item && isGroup(item->type))
        //    {
        //        HTREEITEM hItemFirst = GetFirstItem();
        //        Expand(hItem);
        //        EnsureVisible(hItemFirst);
        //        if (m_isCapture)
        //        {
        //            ReleaseCapture();
        //        }
        //        return 0;
        //    }
        //}
        if (m_isCapture)
        {
            ReleaseCapture();
            return 0;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_CAPTURECHANGED:
    {
        // 释放了捕获
        //if (m_rbutton.state == STATE::DOWN)
        //{
        //    POINT pt;
        //    GetCursorPos(&pt);
        //    ScreenToClient(hWnd, &pt);
        //    m_rbutton.state = PtInRect(&m_rbutton.rc, pt) ? STATE::HOT : STATE::NORMAL;
        //    if (m_rbutton.state == STATE::HOT)
        //        OnValueButtonClick(m_rbutton.type);

        //    InvalidateRect(&m_rbutton.rc);
        //}
        m_isCapture = false;
        break;
    }
    case WM_LBUTTONDBLCLK:
    {
        HTREEITEM hItem = GetIndex();
        IPROPERTY_ITEM* item = GetItemData(hItem);
        if (item && IsGroup(item->type))
        {
            //HTREEITEM hItemFirst = GetFirstItem();
            Expand(hItem);
            //EnsureVisible(hItemFirst);
            return 0;
        }
        if (m_isNoDblclk)
        {
            m_isNoDblclk = false;
            return 0;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_MOUSEWHEEL:
    case WM_VSCROLL:
        return OnVScroll(oldProc, hWnd, message, wParam, lParam);
    case WM_MOUSEMOVE:
    {
        //if (m_rbutton.state == STATE::DOWN) // 按下位置是右边的按钮, 不处理移动消息
        //    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

        TVHITTESTINFO info = { 0 };
        info.pt.x = GET_X_LPARAM(lParam);
        info.pt.y = GET_Y_LPARAM(lParam);
        if (m_isCapture)    // 鼠标捕获中, 移动中间的线
        {
            RECT rc;
            GetClientRect(&rc);
            int cx = rc.right - rc.left;
            int cy = rc.bottom - rc.top;
            info.pt.x += m_offsetLeft;
            int bfb = info.pt.x * 1000 / (cx);
            int width = max(200, min(900, bfb));
            if (width != m_leftWidth)
            {
                m_leftWidth = width;
                if (m_edit.IsVisible())
                    m_edit.hide();
                //_str::dbg("x = %d, 偏移 = %d, 百分比 = %d, 计算出结果 = %d\n", x, x - m_oldMid, bfb, width);
                moveEdit_Static(m_hItemSel);
                InvalidateRect();
            }
            break;
        }

        //if (OnMouseMove(info))  // 如果鼠标在右边按钮中, 那就不往下处理
        //    break;

        //if (m_rbutton.state != STATE::NORMAL)
        //{
        //    m_rbutton.state = STATE::NORMAL;
        //    InvalidateRect(&m_rbutton.rc);
        //}
        if (info.pt.y > m_midLineHeight)
        {
            m_hCursor = s_hCursor[0];
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }

        const int mid = calcMidLine();
        //_str::dbg("mid = %d, x = %d\n", mid, x);
        m_offsetLeft = mid - info.pt.x;
        m_hCursor = s_hCursor[(info.pt.x < mid - 3 || info.pt.x > mid + 3) ? 0 : 1];

        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_KEYUP:
    {
        if (wParam == VK_DELETE)
        {
            OnItemDelUp(true);
            break;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_KILLFOCUS:
    {

        break;
    }
    case TVM_DELETEITEM:
    {
        HTREEITEM hItemSel = m_hItemSel;
        HTREEITEM hParent  = hItemSel ? GetParent(hItemSel) : 0;
        HTREEITEM hItemDel = (HTREEITEM)lParam;
        LRESULT ret = CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        if (hItemSel && hItemDel)
        {
            if (hItemSel == hItemDel || hParent == hItemDel)
            {
                m_edit.hide();      // 隐藏编辑框
                m_static.hide();    // 隐藏标签
            }
        }
        modify_value_from_edit();
        return ret;
    }
    default:
        //wstr::dbg(L"message = %d\n", message);
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}



bool CPropertyTree::OnMouseMove(TVHITTESTINFO& info)
{
    Send(TVM_HITTEST, 0, (LPARAM)&info);
    m_hItemHot = info.hItem;
    if (m_hItemHot == 0 || m_hItemHot != m_hItemSel) return false;  // 当前鼠标移动的项目不是选中项, 只处理选中项

    auto f = m_items.find(m_hItemHot);
    if (f == m_items.end()) return false;

    RECT rcItem;
    GetPropertyItemRect(m_hItemHot, &rcItem, PROPERTY_RECT_BUTTON);
    if (!PtInRect(&rcItem, info.pt)) // 鼠标不在右边按钮上
        return false;

    // 走到这鼠标是在右边的按钮上
    if (m_rbutton.state == STATE::HOT)  // 原来状态还是热点, 不处理
        return true;

    TRACKMOUSEEVENT tm;
    tm.cbSize = sizeof(tm);
    tm.dwFlags = TME_LEAVE;
    tm.dwHoverTime = HOVER_DEFAULT;
    tm.hwndTrack = m_hWnd;
    TrackMouseEvent(&tm);

    m_hCursor = s_hCursor[0];
    m_rbutton.state = STATE::HOT;
    InvalidateRect(&m_rbutton.rc);

    return true;
}

void CPropertyTree::modify_value_from_edit()
{
    HTREEITEM hItemSel = (HTREEITEM)m_edit.GetPropW(L"hItemSel");
    //wstr::dbg(L"设置属性, 项目句柄 = %d\n", hItemSel);
    if (hItemSel)
    {
        PROPERTY_ITEM& data = m_items[hItemSel];
        if (IsEdit(data.type))  // 可编辑的项目才往下走
        {
            wstr text = m_edit.GetText();
            if (data.value != text)
            {
                if ((IsCombobox(data.type)) && data.comboboxData)
                {
                    size_t len = data.comboboxData->size();
                    if ((size_t)data.index < len)
                        data.comboboxData->at(data.index).name = text;
                }
                data.value = text;
                InvalidateItem(hItemSel);
                OnItemPropChanged(hItemSel, data);

            }
        }
        //m_edit.SetText(L"");
    }
    m_edit.SetPropW(L"hItemSel", 0);
}

void CPropertyTree::InvalidateItem(HTREEITEM hItem)
{
    if (!hItem)
    {
        ::InvalidateRect(m_hWnd, 0, 0);
        return;
    }
    RECT rc;
    TreeView_GetItemRect(m_hWnd, hItem, &rc, 0);
    ::InvalidateRect(m_hWnd, &rc, 0);
}

void CPropertyTree::CreateD2DResouce() {
    if (!m_hCanvas) {
        RECT rc;
        GetClientRect(&rc);
        rc.right -= rc.left;
        rc.bottom -= rc.top;
        m_hCanvas = _canvas_create(m_hWnd, rc.right, rc.bottom);
    }
}


void CPropertyTree::__create_d2d_resouce()
{
    if (m_hCanvas)return;
    if (!m_hCanvas)
    {
        CreateD2DResouce();
        if (!m_hCanvas)return;
    }
    RECT rc;
    ::GetWindowRect(m_hWnd, &rc);
    int cx = rc.right - rc.left, cy = rc.bottom - rc.top;
    m_hCanvas = _canvas_create(m_hWnd, cx, cy);
    SetColor(0, false);
}

void CPropertyTree::__delete_d2d_resouce(bool isDestroyCanvas)
{
    if (m_draw.hFont)               _font_destroy(m_draw.hFont);
    if (m_draw.hFontParent)         _font_destroy(m_draw.hFontParent);
    if (m_draw.hFontGdi)            DeleteObject(m_draw.hFontGdi);

    if (m_draw.hbrBackground)        _brush_destroy(m_draw.hbrBackground);
    if (m_draw.hbrBackgroundGdi)    DeleteObject(m_draw.hbrBackgroundGdi);

    if (m_draw.hbrParent)           _brush_destroy(m_draw.hbrParent);
    if (m_draw.hpenParent)          _pen_destroy(m_draw.hpenParent);

    if (m_draw.hbrText)             _brush_destroy(m_draw.hbrText);


    if (m_draw.hpenListBorder)      _pen_destroy(m_draw.hpenListBorder);
    if (m_draw.hpenListBorderGdi)   DeleteObject(m_draw.hpenListBorderGdi);

    if (m_draw.hbrSelect)           _brush_destroy(m_draw.hbrSelect);
    if (m_draw.hbrSelectGdi)        DeleteObject(m_draw.hbrSelectGdi);

    if (m_draw.hbrDisabled)         _brush_destroy(m_draw.hbrDisabled);



    if (isDestroyCanvas)
    {
        if (m_expand.imgClose)      _img_destroy(m_expand.imgClose);
        if (m_expand.imgOpen)       _img_destroy(m_expand.imgOpen);

        _canvas_destroy(m_hCanvas);
    }

    for (auto it = m_map_icon.begin(); it != m_map_icon.end(); ++it)
    {
        _img_destroy(it->second);
    }
    m_map_icon.clear();
}

LRESULT CPropertyTree::OnSize(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int cx = LOWORD(lParam);
    int cy = HIWORD(lParam);
    _canvas_resize(m_hCanvas, cx, cy);
    moveEdit_Static(m_hItemSel);
    InvalidateRect();
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

LRESULT CPropertyTree::OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    ::GetClientRect(hWnd, &rc);
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    //wstr::dbg(L"重画 脏区域 = %d, %d, %d, %d\n", ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
    _canvas_begindraw(m_hCanvas);
    _canvas_SetAntialiasMode(m_hCanvas, 0, 0);   // 去除抗锯齿
    //_canvas_SetTextAntialiasMode(m_hCanvas, D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
    _canvas_clear(m_hCanvas, m_draw.crBackground);
    RECT_F rcf(rc);
    rcf.right = leftSize;
    _canvas_fillRectangle(m_hCanvas, m_draw.hbrParent, &rcf);

    HTREEITEM hItem = GetFirstVisible();
    UINT count = GetVisibleCount() + 1;

    int scrollPos = GetScrollPos(hWnd, SB_HORZ);
    RECT rcParent = { 0 };
    m_midLineHeight = 0;
    for (UINT i = 0; i < count; i++)
    {
        if (!hItem)break;
        RECT rc = GetItemRect(hItem);
        if (Rect_isOverlap(rc, ps.rcPaint))
            DrawItem(m_hCanvas, hItem, rc, i, scrollPos, cxClient, cyClient);
        hItem = GetNextItem(hItem, TVGN_NEXTVISIBLE);
    }
    //_canvas_SetAntialiasMode(m_hCanvas, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    HDC hdc = _canvas_getdc(m_hCanvas);
    BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
    _canvas_releasedc(m_hCanvas);

    _canvas_enddraw(m_hCanvas);
    EndPaint(hWnd, &ps);

    if (0)
    {
        auto pfn_draw = [](LPCANVAS hCanvas, LPCWSTR file)
        {
            static float i = 0;
            float left = i * 16;
            if (i == 6)
                left += 10;
            i++;
            LPEX_IMAGE hImg = _img_create_fromfile(file);
            hCanvas->DrawImage(hImg, left, 0, 255);
            _img_destroy(hImg);
        };
        const int cx = 80 + 52;
        LPCANVAS hCanvas = _canvas_create(0, cx, 16);
        hCanvas->BeginDraw();
        hCanvas->clear(0);

        pfn_draw(hCanvas, LR"(D:\cpp\SVN工作目录\窗口设计器\选择框-未选择.png)");
        pfn_draw(hCanvas, LR"(D:\cpp\SVN工作目录\窗口设计器\选择框-选择.png)");
        pfn_draw(hCanvas, LR"(D:\cpp\SVN工作目录\窗口设计器\选择框_半选中.png)");
        pfn_draw(hCanvas, LR"(D:\cpp\SVN工作目录\窗口设计器\单选框_未选中.png)");
        pfn_draw(hCanvas, LR"(D:\cpp\SVN工作目录\窗口设计器\单选框_选中.png)");
        pfn_draw(hCanvas, LR"(D:\cpp\SVN工作目录\窗口设计器\开关 关.png)");
        pfn_draw(hCanvas, LR"(D:\cpp\SVN工作目录\窗口设计器\开关 开.png)");

        hCanvas->EndDraw();

        SIZE_T size;
        LPBYTE p = _img_save_formcanvas(hCanvas, size);
        FILE* f;
        fopen_s(&f, R"(D:\cpp\SVN工作目录\窗口设计器\资源.png)", "wb+");
        fwrite(p, 1, size, f);
        fclose(f);
    }
    return 0;
}

void CPropertyTree::DrawItem(LPCANVAS hCanvas, HTREEITEM hItem, RECT& rc, int index, int scrollPos, int cxClient, int cyClient)
{
    m_midLineHeight = rc.bottom;
    RECT_F rcf(rc);
    auto f = m_items.find(hItem);
    if (f == m_items.end()) return;
    IPROPERTY_ITEM& data = f->second;

    if (m_evt.pfnItemBeginDraw)     // 有挂接开始绘画, 需要获取出D2D对应的dc
    {
        HDC hdc = _canvas_getdc(hCanvas);
        bool isReturn = m_evt.pfnItemBeginDraw(m_evt.pfnItemBeginDrawParam, hItem, data.type, hdc);
        _canvas_releasedc(hCanvas);
        if (isReturn) return;   // 用户处理了绘画, 不往后处理
    }
    //_canvas_drawRectangle(hCanvas, m_draw.hpenParent, &rcf);

    int width = calcMidLine(cxClient);
    rcf.right = (float)width;
    rcf.left = leftSize;
    DWORD fmt = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
    LPEX_BRUSH hbr = 0;
    LPEX_FONT hFont = m_draw.hFont;
    const bool bGroup = IsGroup(data.type);
    if (bGroup)   // 是父项目
    {
        const float cxImg = 16;
        rcf.right = (float)rc.right;
        hbr = m_draw.hbrParent;
        const bool isExp = IsExpand(hItem);
        const float iconleft = (float)(isExp ? 7 : 6) * cxImg;
        RECT_F rcDraw;  // 画出位置
        rcDraw.left = ((float)leftSize - cxImg) / 2;
        rcDraw.top = rcf.top + ((float)m_itemHeight - cxImg) / 2;
        rcDraw.right = rcDraw.left + cxImg;
        rcDraw.bottom = rcDraw.top + cxImg;
        hFont = m_draw.hFontParent;

        if (isExp && m_expand.imgOpen)  // 是展开状态, 并且展开的图片有设置
        {
            _canvas_drawimagerectrect(hCanvas, m_expand.imgOpen, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
                0, 0, cxImg, cxImg, 255);
        }
        else if (!isExp && m_expand.imgClose)   // 是收缩状态, 并且收缩的图片有设置
        {
            _canvas_drawimagerectrect(hCanvas, m_expand.imgClose, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
                0, 0, cxImg, cxImg, 255);
        }
        else
        {
            LPEX_IMAGE hImage = isExp ? GetIcon(PROPERTY_ICONTYPE_TREE_ENPAND) : GetIcon(PROPERTY_ICONTYPE_TREE_UNENPAND);
            if (hImage)
            {
                UINT cx, cy;
                hImage->GetSize(&cx, &cy);
                _canvas_drawimagerectrect(hCanvas, hImage, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
                    0, 0, (float)cx, (float)cy, 255);
            }
            else
            {
                _canvas_drawimagerectrect(hCanvas, s_image, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
                    iconleft, 0, iconleft + cxImg, cxImg, 255);
            }
        }
    }
    else if (hItem == m_hItemDown)
    {
        hbr = m_draw.hbrSelect;
    }
    else if (hItem == m_hItemSel)
    {
        hbr = m_draw.hbrSelect;
    }
    if (hbr) _canvas_fillRectangle(hCanvas, hbr, &rcf);    // 填充项目背景色

    rcf.left += 5;
    hbr = IsDisabled(data.type) ? m_draw.hbrDisabled : m_draw.hbrText;
    float textWidth = 0, textHeight = 0;
    _canvas_calctextsize(hCanvas, hFont, data.name.c_str(), data.name.size(), fmt, rcf.right - rcf.left, rcf.bottom - rcf.top, &textWidth, &textHeight);
    m_rcCalcText.top = (int)((rcf.bottom - rcf.top - textHeight) / 2);
    _canvas_drawtext(hCanvas, hFont, hbr, data.name.c_str(), data.name.size(), fmt, &rcf);  // 画文本

    LINEPOINT_F pt1(rcf.right, rcf.top, rcf.right, rcf.bottom);         // 直线坐标, 在中间画个直线
    LINEPOINT_F pt2(leftSize, rcf.top, (float)rc.right, rcf.top);       // 直线坐标, 在项目底边画条直线
    LINEPOINT_F pt3(leftSize, rcf.bottom, (float)rc.right, rcf.bottom); // 直线坐标, 在项目底边画条直线
    _canvas_drawLine(hCanvas, m_draw.hpenParent, &pt1);
    _canvas_drawLine(hCanvas, m_draw.hpenParent, &pt2);
    _canvas_drawLine(hCanvas, m_draw.hpenParent, &pt3);


    // 绘画部分, 分组不绘画右边的数据
    if (bGroup)
    {
        if (m_evt.pfnItemEndDraw)     // 有挂接结束绘画, 需要获取出D2D对应的dc
        {
            HDC hdc = _canvas_getdc(hCanvas);
            bool isReturn = m_evt.pfnItemEndDraw(m_evt.pfnItemEndDrawParam, hItem, data.type, hdc);
            _canvas_releasedc(hCanvas);
            if (isReturn) return;   // 用户处理了绘画, 不往后处理
        }
        return;
    }
    if (m_draw.chWidth == 0)
    {
        _canvas_calctextsize(hCanvas, m_draw.hFont, L"i", 1, DT_CALCRECT | fmt,
            rcf.right - rcf.left, rcf.bottom - rcf.top, &m_draw.chWidth, &m_draw.chHeight);
    }


    rcf.left = (float)(width + 5);
    rcf.right = (float)rc.right;
    // 有自定义属性值的需要先获取属性值显示的文本在绘画
    if (IsCustom(data.type))
    {
        wstr tmp = MakeCustomText(data, hItem);
        if (!tmp.empty())
            _canvas_drawtext(hCanvas, m_draw.hFont, hbr, tmp.c_str(), tmp.size(), fmt, &rcf);
    }
    else if (IsColorDialog(data.type))
    {
        if (m_isFillColorBack)
        {
            float width = 0.f, height = 0.f;

            //_canvas_calctextsize(hCanvas, m_draw.hFont, data.value.c_str(), data.value.size(), fmt, rcf.right - rcf.left, rcf.bottom - rcf.top, &width, &height);
            int cr = 0;
            if (hItem == m_hItemSel && m_edit.GetVisible())
                cr = m_edit.GetWindowTextW().stoi();
            else
                cr = data.value.stoi();
            LPEX_BRUSH hbrColor = _brush_create(ARGB_D2D(255, cr));
            COLORREF crBorder = m_draw.crBackgroundGdi;
            LPBYTE tmp = (LPBYTE)&crBorder;
            tmp[0] = 255 - tmp[0];
            tmp[1] = 255 - tmp[1];
            tmp[2] = 255 - tmp[2];
            LPEX_PEN pen = _pen_create(ARGB_D2D(255, crBorder));

            RECT_F rcBack(rcf);
            const float colorBlockHeight = 16.f;
            rcBack.right = rcBack.left + colorBlockHeight;
            rcBack.top += (((float)m_itemHeight - colorBlockHeight) / 2);
            rcBack.bottom = rcBack.top + colorBlockHeight;
            _canvas_fillRectangle(hCanvas, hbrColor, &rcBack);
            _canvas_drawRectangle(hCanvas, pen, &rcBack);
            _brush_destroy(hbrColor);
            _pen_destroy(pen);
        }
        size_t count = data.value.size();
        float n = (rcf.right - rcf.left) / m_draw.chWidth + 10.f;
        if (n < count) count = (size_t)(DWORD)n;
        rcf.left += (float)COLORBLOCK_WIDTH;
        _canvas_drawtext(hCanvas, m_draw.hFont, hbr, data.value.c_str(), count, fmt, &rcf);
    }
    else if (IsCheckBox(data.type) || IsCheckBox3(data.type) || IsRadio(data.type) || IsSwitch(data.type) || IsAutoRadio(data.type))
    {
        DrawCheckBox_Radio(hCanvas, data, hItem, rcf, index, scrollPos, cxClient, cyClient);
    }
    else if (IsLink(data.type))
    {
        DrawLink(hCanvas, data, hItem, rcf, index, scrollPos, cxClient, cyClient);
    }
    //TODO 新增各种类型的话, 需要在这里新增判断, 然后进行对应的绘画, 目前所有类型都只是在右边进行绘画
    else if (!data.value.empty())
    {
        size_t count = data.value.size();
        const float n = (rcf.right - rcf.left) / m_draw.chWidth + 10.f;
        if (n < count) count = (size_t)(DWORD)n;
        _canvas_drawtext(hCanvas, m_draw.hFont, hbr, data.value.c_str(), count, fmt, &rcf);
    }
    //if (hItem == m_hItemSel)
    //{
    //    HDC hdcD2D = _canvas_getdc(hCanvas);
    //    //FillRect(hdcD2D, &rc, GetStockBrush(BLACK_BRUSH));
    //    PaintStatic(rc, data, hdcD2D);
    //    _canvas_releasedc(hCanvas);
    //}

    if (m_evt.pfnItemEndDraw)     // 有挂接结束绘画, 需要获取出D2D对应的dc
    {
        HDC hdc = _canvas_getdc(hCanvas);
        bool isReturn = m_evt.pfnItemEndDraw(m_evt.pfnItemEndDrawParam, hItem, data.type, hdc);
        _canvas_releasedc(hCanvas);
        if (isReturn) return;   // 用户处理了绘画, 不往后处理
    }

}

void CPropertyTree::DrawLink(LPCANVAS hCanvas, IPROPERTY_ITEM& data, HTREEITEM hItem, RECT_F& rc, int index, int scrollPos, int cxClient, int cyClient)
{
}

void CPropertyTree::DrawCheckBox_Radio(LPCANVAS hCanvas, IPROPERTY_ITEM& data, HTREEITEM hItem, RECT_F& rc, int index, int scrollPos, int cxClient, int cyClient)
{
    LPEX_IMAGE hImage = _get_checkbox_radio_hImage();
    // left: 0=选择框未选中, 16=选择框选中, 32=选择框半选中, 48=单选框未选中, 64=选择框选中, 80=开关-关, 106=开关-开
    float cxIcon = 16;
    float cyIcon = 16;
    float left = 0;
    LPCWSTR value = data.value.c_str();
    if (!value) value = L"0";
    PROPERTY_ICONTYPE type = PROPERTY_ICONTYPE_TREE_ENPAND;


    if (IsRadio(data.type) || IsAutoRadio(data.type))
    {
        const bool isCheck = value[0] != '0';
        left = isCheck ? 64.f : 48.f;
        type = isCheck ? PROPERTY_ICONTYPE_RADIO_SEL : PROPERTY_ICONTYPE_RADIO_UNSEL;
    }
    else if (IsCheckBox(data.type))
    {
        const bool isCheck = value[0] != '0';
        left = isCheck ? 16.f : 0.f;
        type = isCheck ? PROPERTY_ICONTYPE_CHECKBOX_SEL : PROPERTY_ICONTYPE_CHECKBOX_UNSEL;
    }
    else if (IsCheckBox3(data.type))
    {
        wchar_t ch = value[0];
        left = ch == '0' ? 0.f : (ch == '1' ? 16.f : 32.f); // 选中返回16, 未选中返回0, 半选中返回32
        type = ch == '0' ? PROPERTY_ICONTYPE_CHECKBOX_UNSEL
            : (ch == '1' ? PROPERTY_ICONTYPE_CHECKBOX_SEL : PROPERTY_ICONTYPE_CHECKBOX_INDETERMINATE);

    }
    else if (IsSwitch(data.type))
    {
        const bool isCheck = value[0] != '0';
        left = isCheck ? 106.f : 80.f;
        cxIcon = 26.f;
        type = isCheck ? PROPERTY_ICONTYPE_SWITCH_SEL : PROPERTY_ICONTYPE_SWITCH_UNSEL;
    }
    LPEX_IMAGE hImgMap = GetIcon(type);
    if (hImgMap)
    {
        UINT cx, cy;
        hImgMap->GetSize(&cx, &cy);
        cxIcon = (float)cx;
        cyIcon = (float)cy;
        hImage = hImgMap;
        left = 0;
    }
    RECT_F rcDraw;
    float offset    = (rc.height() - cyIcon) / 2;
    rcDraw.left     = rc.left + offset;
    rcDraw.top      = rc.top  + offset;
    rcDraw.right    = rcDraw.left + cxIcon;
    rcDraw.bottom   = rcDraw.top  + cyIcon;
    hCanvas->DrawImage(hImage, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
        left, 0, left + cxIcon, cyIcon, 255);

}

LRESULT CPropertyTree::OnPaintStatic(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!m_static.IsVisible() || !m_hItemSel)
        return 0;
    static Gdiplus::Image* hImage;
    static Gdiplus::Font* hFont;
    if (!hImage)
    {
        hImage = CMemDC::_img_createfrommemory(s_img_tree_exp, sizeof(s_img_tree_exp) / sizeof(s_img_tree_exp[0]));
        hFont = new Gdiplus::Font(L"微软雅黑", -12);
    }

    PROPERTY_ITEM& data = m_items[m_hItemSel];
    RECT rc;
    ::GetClientRect(hWnd, &rc);
    if (rc.right <= rc.left || rc.bottom <= rc.top)
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    int state = (int)(LONG_PTR)::GetPropW(hWnd, L"state");
    int type = __static_get_type(hWnd);  // 0=...按钮, 1=下拉按钮
    COLORREF crBack = m_draw.crSelectGdi;
    const BYTE r = GetRValue(crBack);
    const BYTE g = GetGValue(crBack);
    const BYTE b = GetBValue(crBack);


    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    CMemDC dc;
    HDC hdc = dc.create(rc.right - rc.left, rc.bottom - rc.top);
    if (m_evt.pfnItemRButtonPaint && m_evt.pfnItemRButtonPaint(m_evt.pfnItemRButtonPaintParam, m_hItemSel, data.type, hdc, state, rc.right - rc.left, rc.bottom - rc.top))
    {
        dc.show(ps.hdc, 0, 0);
        EndPaint(hWnd, &ps);
        return 0;
    }
    Gdiplus::Graphics* gp = dc.GetGraphics();
    SelectObject(hdc, CWndBase::DefaultFont());
    Gdiplus::RectF rcf;
    rcf.X = (float)rc.left;
    rcf.Y = (float)rc.top;
    rcf.Width = (float)rc.right - (float)rc.left;
    rcf.Height = (float)rc.bottom - (float)rc.top;

    BYTE alpha = (state == STATE::DOWN ? 180 : (state == STATE::HOT ? 220 : 250));
    Gdiplus::SolidBrush hbr(Gdiplus::Color(alpha, r, g, b));
    gp->FillRectangle(&hbr, rcf);

    if (type == 1)
    {
        const float itemBlockHeight = 16;
        Gdiplus::RectF destRect;
        destRect.X = ((float)m_itemHeight - itemBlockHeight) / 2;
        destRect.Y = ((float)m_itemHeight - itemBlockHeight) / 2;
        destRect.Width = itemBlockHeight;
        destRect.Height = itemBlockHeight;
        gp->DrawImage(hImage, destRect, 5 * 16, 0, 16, 16, Gdiplus::Unit::UnitPixel);
    }
    else if (type == 0)
    {
        rc.bottom -= 5;
        DrawTextW(hdc, L"...", 3, &rc, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
    }
    dc.show(ps.hdc, 0, 0);
    EndPaint(hWnd, &ps);
    return 0;
}

void CPropertyTree::PaintStatic(RECT& rcItem, IPROPERTY_ITEM& data, HDC hdcSrc)
{
    if (!m_rbutton.isVisible || !m_hItemSel)
        return;
    static Gdiplus::Image* hImage;
    static Gdiplus::Font* hFont;
    if (!hImage)
    {
        hImage = CMemDC::_img_createfrommemory(s_img_tree_exp, sizeof(s_img_tree_exp) / sizeof(s_img_tree_exp[0]));
        hFont = new Gdiplus::Font(L"微软雅黑", -12);
    }


    RECT rc = m_rbutton.rc;
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    const int state = m_rbutton.state;
    const int type = m_rbutton.type;  // 0=...按钮, 1=下拉按钮
    COLORREF crBack = m_draw.crSelectGdi;
    const BYTE r = GetRValue(crBack);
    const BYTE g = GetGValue(crBack);
    const BYTE b = GetBValue(crBack);

    LPMEMDCBITMAP md = md_create(cxClient, cyClient);
    if (m_evt.pfnItemRButtonPaint && m_evt.pfnItemRButtonPaint(m_evt.pfnItemRButtonPaintParam, m_hItemSel, data.type, md->hdc, state, cxClient, cyClient))
    {
        BLENDFUNCTION blend;
        blend.BlendOp = AC_SRC_OVER;
        blend.BlendFlags = 0;
        blend.AlphaFormat = AC_SRC_ALPHA;
        blend.SourceConstantAlpha = 255;
        AlphaBlend(hdcSrc, rc.left, rc.top, cxClient, cyClient, md->hdc, 0, 0, cxClient, cyClient, blend);
        //BitBlt(hdcSrc, rc.left, rc.top, cxClient, cyClient, md->hdc, 0, 0, SRCCOPY);
        md_destroy(md);
        return;
    }
    Gdiplus::Graphics gp(md->hdc);
    SelectObject(md->hdc, m_draw.hFontGdi);
    Gdiplus::RectF rcf;
    rcf.X = (float)0;
    rcf.Y = (float)0;
    rcf.Width = (float)rc.right - (float)rc.left;
    rcf.Height = (float)rc.bottom - (float)rc.top;

    BYTE alpha = (state == STATE::DOWN ? 180 : (state == STATE::HOT ? 220 : 250));
    Gdiplus::SolidBrush hbr(RGB2ARGB(crBack, alpha));
    gp.FillRectangle(&hbr, rcf);

    if (type == 1)
    {
        const float itemBlockHeight = 16;
        Gdiplus::RectF destRect;
        destRect.X = ((float)m_itemHeight - itemBlockHeight) / 2;
        destRect.Y = ((float)m_itemHeight - itemBlockHeight) / 2;
        destRect.Width = itemBlockHeight;
        destRect.Height = itemBlockHeight;
        gp.DrawImage(hImage, destRect, 5 * 16, 0, 16, 16, Gdiplus::Unit::UnitPixel);
    }
    else if (type == 0)
    {
        rcf.Height -= 5;
        Gdiplus::StringFormat fmt;
        fmt.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
        fmt.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
        Gdiplus::SolidBrush hbrText(RGB2ARGB(255, 255));
        gp.DrawString(L"...", 3, hFont, rcf, &fmt, &hbrText);
        //DrawTextW(md->hdc, L"...", 3, &rc, DT_VCENTER | DT_SINGLELINE | DT_CENTER);
    }
    BLENDFUNCTION blend;
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.AlphaFormat = AC_SRC_ALPHA;
    blend.SourceConstantAlpha = 255;
    AlphaBlend(hdcSrc, rc.left, rc.top, cxClient, cyClient, md->hdc, 0, 0, cxClient, cyClient, blend);
    //BitBlt(hdcSrc, rc.left, rc.top, cxClient, cyClient, md->hdc, 0, 0, SRCCOPY);
    md_destroy(md);
    return;
}

LRESULT CPropertyTree::OnPaintListBox(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    m_listBox.GetClientRect(&rc);
    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;

    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);

    CMemDC dc;
    HDC hdc = dc.create(cx, cy);

    ::SetTextColor(hdc, m_draw.crTextGdi);
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, m_draw.hFontGdi);
    SelectObject(hdc, m_draw.hbrBackgroundGdi);
    SelectObject(hdc, m_draw.hpenListBorderGdi);
    Rectangle(hdc, 0, 0, cx, cy);

    if (!m_hItemSel)
    {
        dc.show(ps.hdc, 0, 0);
        EndPaint(hWnd, &ps);
        return 0;
    }
    auto f = m_items.find(m_hItemSel);
    if (f == m_items.end() || !f->second.comboboxData || f->second.comboboxData->size() == 0)
    {
        dc.show(ps.hdc, 0, 0);
        EndPaint(hWnd, &ps);
        return 0;
    }

    PROPERTY_ITEM& data = f->second;
    std::vector<PROPERTY_ITEM>& arr = *data.comboboxData;
    size_t len = arr.size();

    int pos = GetScrollPos(hWnd, SB_VERT);

    int index = m_listBox.GetIndedx();
    int count = m_listBox.GetCount();
    wstr::dbg(L"列表框重画, 当前索引 = %d, 滚动位置 = %d\n", index, pos);
    rc.left += 1;
    rc.top = 1 - pos * m_itemHeight;
    rc.bottom = rc.top + m_itemHeight;
    const bool bIndex = IsIndex(data.type);
    wstr drawString;
    for (int i = 0; i < count; i++)
    {
        if (rc.bottom > 0)
        {
            if (index == i)
            {
                rc.right -= 1;
                FillRect(hdc, &rc, m_draw.hbrSelectGdi);
                rc.right += 1;
            }
            rc.left += 5;
            if (bIndex)
            {
                drawString.assign(i).append(L". ").append(arr[i].name);
                DrawTextW(hdc, drawString.c_str(), -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            else
            {
                DrawTextW(hdc, arr[i].name.c_str(), -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
            rc.left -= 5;
        }
        rc.top += m_itemHeight;
        rc.bottom = rc.top + m_itemHeight;
        if (rc.top >= cy) break;    //超出了可视范围就退出绘画
    }

    dc.show(ps.hdc, 0, 0);
    EndPaint(hWnd, &ps);
    return 0;
}

LRESULT CPropertyTree::OnListCommand(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int code = HIWORD(wParam);
    switch (code)
    {
    case LBN_SELCHANGE:
    {
        OnComboboxChanged(m_listBox.GetIndedx());
        break;
    }
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}



LRESULT CPropertyTree::OnEditProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return LRESULT();
}

bool CPropertyTree::OnEditMouseWheel(short pos, short key, LPARAM lParam)
{
    auto f = m_items.find(m_hItemSel);
    if (f == m_items.end())return false;
    PROPERTY_ITEM& data = f->second;

    if (IsNoMouseWheel(data.type))
    {
        m_edit.hide();
        m_listBox.hide();
        Send(WM_MOUSEWHEEL, MAKELONG(key, pos), lParam);
        return true;
    }


    if (IsCombobox(data.type))  // 是组合框, 处理组合框滚动
    {
        if (!data.comboboxData) return false;
        std::vector<PROPERTY_ITEM>& arr = *data.comboboxData;
        size_t len = arr.size();
        if (m_evt.pfnComboboxMouseWheel && m_evt.pfnComboboxMouseWheel(m_evt.pfnComboboxMouseWheelParam, m_hItemSel, data.type, (int)len, pos, key, lParam))
            return true;
        const int oldIndex = data.index;

        if (pos > 0)    // 往上滚
        {
            //if (--data.index < 0)
            //    data.index = (int)len - 1;   // 滚到第一个了, 设置为最后一个
            if (--data.index < 0)
                data.index = 0;
        }
        else
        {
            //if (++data.index >= (int)len)
            //    data.index = 0;
            if (++data.index >= (int)len)
                data.index = (int)(LONG_PTR)len - 1;
        }
        if (oldIndex != data.index)     // 修改后的索引不一样才触发改变
        {
            data.value = arr[data.index].name;
            m_edit.SetText(data.value.c_str());
            OnItemPropChanged(m_hItemSel, data);
        }
        return true;
    }

    //TODO 可以在这里处理数值型的滚动增加减少大小
    return false;
}

bool CPropertyTree::OnEditRButtonDown(int mk, LPARAM pt)
{
    IPROPERTY_ITEM* item = GetItemData(m_hItemSel);
    if (!item)return false;
    IPROPERTY_ITEM& data = *item;

    if (IsEdit(data.type)) return false;

    if (m_evt.pfnItemRButtonClick && m_evt.pfnItemRButtonClick(m_evt.pfnItemRButtonClickParam, m_hItemSel, data.type))
        return true;

    return false;
}

void CPropertyTree::OnItemClick(HTREEITEM hItem, int col, LPARAM lParam)
{
    if (!hItem)return;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return;
    modify_value_from_edit();
    PROPERTY_ITEM& data = f->second;

    m_listBox.hide();
    m_isNoDblclk = false;
    m_edit.hide();  // 先隐藏编辑框, 后面根据需要在显示
    if (IsGroup(data.type))
    {
        m_static.hide();
        //m_rbutton.isVisible = false;
        //InvalidateRect(&m_rbutton.rc);
        if (col == -1)
        {
            m_static.SetVisible(false);
            //m_rbutton.isVisible = false;
            m_isNoDblclk = true;
            Expand(hItem);
            return;
        }
        return;
    }
    //wstr::dbg(L"表项被单击, 列索引 = %d\n", col);
    const bool isBtn = IsButton(data.type);
    const bool isCbb = IsCombobox(data.type);

    // 单击的项目没有按钮, 没有下拉按钮
    if (!isCbb && !isBtn)
    {
        //m_rbutton.isVisible = false;
        //InvalidateRect(&m_rbutton.rc);
        //m_rbutton.type = -1;
        m_static.hide();
        __static_set_type(m_static, -1);
    }
    else
    {
        // 0=...按钮, 1=下拉组合框
        // 有按钮 返回0, 有组合框返回1, 否则返回-1
        int type = isBtn ? 0 : (isCbb ? 1 : -1);

        if (type != -1)
        {
            m_static.show();
            __static_set_type(m_static, type);
            //m_rbutton.isVisible = true;
            //InvalidateRect(&m_rbutton.rc);
            //m_rbutton.type = type;
        }
    }

    const bool isDisable    = IsDisabled(data.type);
    const bool isCheckbox   = IsCheckBox(data.type);
    const bool isCheckbox3  = IsCheckBox3(data.type);
    const bool isRadio      = IsRadio(data.type);
    const bool isAutoRadio  = IsAutoRadio(data.type);
    const bool isSwitch     = IsSwitch(data.type);
    const bool isLink       = IsLink(data.type);

    // 点击了属性名这一列, 或者项目是禁止状态, 或者是选择框/单选框/链接, 这些是不需要显示编辑框的
    if (col == 1 || isDisable || isCheckbox || isCheckbox3 || isRadio || isAutoRadio || isSwitch || isLink)
    {
        if (col == 1 || isDisable)
        {
            // 这两个暂时不做其他处理
        }
        else if (isLink)
        {
            // 链接的话应该是直接弹出链接?
        }
        else
        {
            // 剩下的就是选择组件了
            int width = calcMidLine();
            data.value.resize(1);
            wchar_t& ch = data.value[0];
            bool isChange = true;  // 这个值为true的时候才改变选中的状态
            if (!IsCheckLineSel(data.type))
            {
                // 不是整行选择的需要计算是否在图标的位置中点击
                int cxIcon = 16;
                int cyIcon = 16;
                RECT rcItem = GetItemRect(hItem);

                int offset = (m_itemHeight - cyIcon) / 2;
                const int left = width + offset + 5;
                RECT rc = { left, rcItem.top, left + cxIcon, rcItem.top + cyIcon };
                POINT pt = { LOWORD(lParam), HIWORD(lParam) };
                if (isSwitch)   // 开关横向是26个像素
                {
                    rc.right = left + cxIcon + 10;
                }
                isChange = PtInRect(&rc, pt);
            }

            if (isChange)
            {
                if (isCheckbox3)
                {
                    // 需要转换3个状态, 半选中转成选中, 选中改成不选中
                    ch = ch == '2' ? '1' : (ch == '0' ? '2' : '0'); // 切换新的值
                }
                else if (isAutoRadio)
                {
                    // 自动单选框需要把当前分组的其他自动单选框给清除选中
                    ch = '1';   // 自动单选框点击哪个哪个就是选中, 其他全部设置为未选中
                    HTREEITEM hParent = GetParent(hItem);
                    HTREEITEM hChild = GetChild(hParent);
                    while (hChild)
                    {
                        if (hChild != hItem)
                        {
                            PROPERTY_ITEM& item = m_items[hChild];
                            if (IsAutoRadio(item.type))
                            {
                                if (!item.value.empty() && item.value[0] != '0')
                                    OnItemPropChanged(hChild, item);    // 原来是选中的状态都需要触发属性改变
                                item.value.assign(L"0");
                            }
                        }
                        hChild = GetNextSibling(hChild);
                    }
                    InvalidateItem(0);

                }
                else
                {
                    // 只有真和假
                    ch = ch == '0' ? '1' : '0'; // 切换新的值
                }
                OnItemPropChanged(hItem, data);

                InvalidateItem(hItem);
            }
        }
        moveEdit_Static(hItem);
        return;
    }


    // 按下了属性值这一列, 需要显示编辑框

    //wstr::dbg(L"点击项目, 设置句柄, 项目句柄 = %d\n", hItem);
    m_edit.SetPropW(L"hItemSel", hItem);
    moveEdit_Static(hItem);
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(m_edit, &pt);
    if (IsCustom(data.type))
    {
        wstr str = MakeCustomText(data, hItem);
        m_edit.SetText(str.c_str());
    }
    else
    {
        m_edit.SetText(data.value.c_str());
    }

    m_edit.Send(WM_LBUTTONDOWN, 0, MAKELONG(pt.x, pt.y));
    m_edit.Send(WM_LBUTTONUP, 0, MAKELONG(pt.x, pt.y));
    m_edit.show();

    if (!IsEdit(data.type))
    {
        bool bhide = HideCaret(m_edit);
        if (bhide)
        {
            --m_editCaret;
        }
    }
    else
    {
        while (m_editCaret < 1)
        {
            BOOL bshow = ShowCaret(m_edit);
            ++m_editCaret;
        }
    }
    return;
}

void CPropertyTree::OnEditReturn()
{
    PROPERTY_ITEM& data = m_items[m_hItemSel];
    if (m_evt.pfnPropertyReturn && m_evt.pfnPropertyReturn(m_evt.pfnPropertyReturnParam, m_hItemSel, data.type))
        return;
    wstr text = m_edit.GetText();
    if (data.value != text)
    {
        if ((IsCombobox(data.type)) && data.comboboxData)
        {
            size_t len = data.comboboxData->size();
            if ((size_t)data.index < len)
                data.comboboxData->at(data.index).name = text;
        }
        data.value = text;
        InvalidateItem(m_hItemSel);
        m_edit.SetText(L"");
        m_edit.SetPropW(L"hItemSel", 0);
        m_edit.hide();
        OnItemPropChanged(m_hItemSel, data);
        return;
    }
    wstr::dbg(L"编辑框按下回车, 设置句柄为0\n");
    m_edit.SetPropW(L"hItemSel", 0);
    m_edit.hide();
    //wstr::dbg(L"编辑框输入回车, 内容 = %s\n", m_edit.GetText().c_str());
}

void CPropertyTree::OnEditLDown(int key, DWORD pt)
{
    if (m_listBox.IsVisible())m_listBox.hide();
}

void CPropertyTree::OnEditDblClk()
{
    //wstr::dbg(L"编辑框被双击\n");
    auto f = m_items.find(m_hItemSel);
    if (f == m_items.end())return;
    PROPERTY_ITEM& data = f->second;
    if (IsCombobox(data.type))
    {
        if (!data.comboboxData)
        {
            if (m_evt.pfnComboboxLDoubleClick && m_evt.pfnComboboxLDoubleClick(m_evt.pfnComboboxLDoubleClickParam, m_hItemSel, data.type, -1, (int)0))
                return;
            return;
        }
        std::vector<PROPERTY_ITEM>& arr = *data.comboboxData;
        size_t len = arr.size();
        if (m_evt.pfnComboboxLDoubleClick && m_evt.pfnComboboxLDoubleClick(m_evt.pfnComboboxLDoubleClickParam, m_hItemSel, data.type, data.index, (int)len))
            return;

        int index = data.index + 1;
        if (index >= (int)len) index = 0;

        if (m_evt.pfnComboboxChanged && m_evt.pfnComboboxChanged(m_evt.pfnComboboxChangedParam, m_hItemSel, data.type, index))
            return;

        data.index = index;
        data.value = arr[data.index].name;
        if (data.value.empty())__debugbreak();
        m_edit.SetText(data.value.c_str());
        m_edit.SetSel((int)data.value.size(), -1);

        OnItemPropChanged(m_hItemSel, data);
        return;
    }
    if (m_evt.pfnEditLDoubleClick && m_evt.pfnEditLDoubleClick(m_evt.pfnEditLDoubleClickParam, m_hItemSel, data.type))
        return;

    // 编辑框被双击, 字体, 长文本, 字节集 需要弹出一个对话框
    if (IsButton(data.type)
        && !IsTextButton(data.type)     // 带文本按钮不转成单击
        && !IsColorDialog(data.type)    // 颜色选择器不转成单击
        && !IsFileDialog(data.type)     // 文件选择器不转成单击
        )
    {
        OnValueButtonClick(0);  // 项目右边的按钮被单击, 0=按钮, 1=组合框
        return;
    }

    m_edit.SetSel(0, -1);
}

bool CPropertyTree::OnEditNcCalcSize(WPARAM wParam, LPARAM lParam)
{
    if (!wParam)return true;
    NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)lParam;
    //lpncsp->rgrc[0].bottom-lpncsp->rgrc[0].top
    lpncsp->rgrc[0].top += m_rcCalcText.top;
    lpncsp->rgrc[0].left -= m_rcCalcText.left;


    return true;
}

bool CPropertyTree::__Edit_insert_char(int ch, bool bUnsigned, bool isDouble, const DWORD start, const DWORD end, wstr& text, wstr& insertText)
{
    if (ch < '0' || ch > '9')
    {
        const size_t size = text.size();

        // 有符号
        if (ch == '-')  //输入了减号
        {
            if (bUnsigned) return false; // 无符号, 无符号没有减号, 不允许输入

            if (start == 0 && end == size)   // 输入了减号, 并且是全选状态, 允许输入
                return true;

            // 走到这就是输入了减号, 并且不是全选状态
            if (size > 0 && text[0] == '-') // 输入了减号, 并且原来的文本第一个字符是减号
            {
                if (start == 0 && end > 0) // 输入了减号, 并且原来的文本第一个字符是减号, 并且编辑框里选择了减号这个字符, 允许输入
                    return true;
                return false;
            }

            if (start > 0) return false; // 输入了减号, 当前起始选择位置不是第一个
            // 前面的代码都是判断输入位置的第一个

            // 下面是判断有没有插入字符后的文本里有没有减号
            size_t npos = insertText.find('-');

            // 如果插入字符后的文本第一个字符是减号, 那就允许输入
            // npos > 0 表示输入的字符不是在第一个, 不允许输入
            if (npos > 0) return false;

        }
        else if (ch == '.')     // 输入了点
        {
            if (!isDouble) return false;     // 不是浮点型, 不允许输入

            if (start == 0 && end == size)  // 输入了点, 并且是全选状态, 需要在最前面增加个0
            {
                m_edit.ReplaceSel(L"0");    // 加个0, 并且允许输入
                return true;
            }
            const size_t npos = text.find('.');
            if (npos != wstr::npos)     // 原来的文本里有点
            {
                if (insertText.find('.') == wstr::npos) // 输入后的文本里没有点
                {
                    // 走到这表示替换了选中的文本, 并且选中的文本中有点
                    if (insertText[0] == '.')
                    {
                        // 第一个字符是点, 那就表示选择的部分是从0开始, 把这部分替换成0, 然后在允许输入
                        m_edit.ReplaceSel(L"0");    // 加个0, 并且允许输入
                        return true;
                    }

                    // 新输入的点覆盖了旧的点, 且不是在最左边
                    return true;   // 直接允许输入
                }

                // 原文本有点, 又输入了一个点, 不允许输入
                return false;

            }

            // 走到这原文本没有点, 现在在输入点
            if (insertText[0] == '.')   // 输入后的文本第一个字符是点
            {
                // 输入后的第一个字符是点, 需要添加个0
                m_edit.ReplaceSel(L"0");    // 加个0, 并且允许输入
                return true;
            }

        }
        else
        {
            // 不是0-9, 也不是减号和点, 不允许输入
            return false;
        }
    }
    return true;
}

bool CPropertyTree::__Edit_insert_char_min_max(int ch, bool bUnsigned, double min, double max, double umax, LPCWSTR minstr, LPCWSTR maxstr, LPCWSTR umaxstr)
{
    wstr text = m_edit.GetWindowTextW();

    DWORD start = 0, end = 0;
    m_edit.GetSel(&start, &end);

    const wchar_t tmpStr[] = { (wchar_t)ch, 0 };
    m_edit.ReplaceSel(tmpStr, true);
    wstr insertText = m_edit.GetWindowTextW();
    m_edit.Undo();
    //m_edit.SetWindowTextW(text.c_str());
    //m_edit.SetSel(start, end);
    bool bRet = false;
    double num = insertText.stod();

    if (maxstr == 0)
        return __Edit_insert_char(ch, bUnsigned, true, start, end, text, insertText);

    bRet = __Edit_insert_char(ch, bUnsigned, false, start, end, text, insertText);
    if (!bRet) return false;   // 不允许输入

    // 允许输入字符, 需要判断范围
    if (bUnsigned)  // 无符号
    {
        if (num > umax)
        {
            m_edit.SetWindowTextW(umaxstr);
            m_edit.SetSel(0, (int)wcslen(umaxstr));
            return false;
        }
    }
    else
    {
        // 有符号
        if (num > max)
        {
            m_edit.SetWindowTextW(maxstr);
            m_edit.SetSel(0, (int)wcslen(maxstr));
            return false;
        }
        else if (num < min)
        {
            m_edit.SetWindowTextW(minstr);
            m_edit.SetSel(0, (int)wcslen(minstr));
            return false;
        }
    }
    return true;
}

struct _TIMER_PROC_DATA
{
    CPropertyTree* tree;
    HTREEITEM hItem;
};
void CALLBACK Timer_Proc_InvalidateItem(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    KillTimer(hWnd, id);
    _TIMER_PROC_DATA* ptr = (_TIMER_PROC_DATA*)id;
    ptr->tree->InvalidateItem(ptr->hItem);
    delete ptr;
}
bool CPropertyTree::OnEditChar(int ch, int* pRet)
{
    auto f = m_items.find(m_hItemSel);
    if (f == m_items.end()) return false;

    PROPERTY_ITEM& data = f->second;
    if (!IsEdit(data.type))  // 不可编辑的话, 所有输入都禁止
    {
        *pRet = 0;
        return true;
    }

    if (m_evt.pfnItemChar && m_evt.pfnItemChar(m_evt.pfnItemCharParam, m_hItemSel, data.type, ch, pRet))
        return true;
    if (ch == 1)    // Ctrl + A
    {
        m_edit.SetSel(0, -1);
        return false;
    }

    // 8 = 退格键
    // 24 = ctrl + x
    // 26 = ctrl + z
    // 22 = ctrl + v
    if (ch == 8 || ch == 22 || ch == 24 || ch == 26)
    {
        if (IsColorDialog(data.type))
        {
            _TIMER_PROC_DATA* ptr = new _TIMER_PROC_DATA;
            ptr->tree = this;
            ptr->hItem = m_hItemSel;
            SetTimer(m_hWnd, (UINT_PTR)ptr, 20, Timer_Proc_InvalidateItem);
        }
        return false;
    }
    if (ch < 32 || ch == 127) return false;

    // 不是颜色选择器且不是数值类型的全都不拦截输入消息
    const bool isColor = IsColorDialog(data.type);
    if (!isColor && !IsNumber(data.type)) return false;

    // 1=字节,2=短整数,3=整数,4=长整数,5=小数,6=双精度小数
    int type = GetNumberType(data.type);
    bool bUnsigned = IsUnsigned(data.type);     // 无符号, 默认是有符号
    if (isColor) { type = 3; }
    bool bRet = false;

    switch (type)
    {
    case 1:     // 字节
    {
        const int minint8 = MININT8;
        bRet = __Edit_insert_char_min_max(ch, bUnsigned, minint8, 0x7F, 0xFF, L"-128", L"127", L"255");
        return !bRet;
    }
    case 2:     // 短整数
    {
        const int minint16 = MININT16;
        bRet = __Edit_insert_char_min_max(ch, bUnsigned, minint16, 0x7FFF, 0xFFFF, L"-32768", L"32767", L"65535");
        return !bRet;
    }
    case 3:     // 整数
    {
        const int minint32 = MININT;
        bRet = __Edit_insert_char_min_max(ch, bUnsigned, minint32, 0x7FFFFFFF, 0xFFFFFFFF, L"-2147483648", L"2147483647", L"4294967295");
        if (isColor)
        {
            _TIMER_PROC_DATA* ptr = new _TIMER_PROC_DATA;
            ptr->tree = this;
            ptr->hItem = m_hItemSel;
            SetTimer(m_hWnd, (UINT_PTR)ptr, 20, Timer_Proc_InvalidateItem);
        }
        return !bRet;
    }
    case 4:     // 长整数
    {
        const long long minint64 = MININT64;
        bRet = __Edit_insert_char_min_max(ch, bUnsigned, minint64, (double)0x7FFFFFFFFFFFFFFF, (double)0xFFFFFFFFFFFFFFFF,
            L"-9223372036854775808", L"9223372036854775807", L"18446744073709551615");
        return !bRet;
    }
    case 5:     // 小数
    {
        bRet = __Edit_insert_char_min_max(ch, bUnsigned, 0, 0, 0, 0, 0, 0);
        return !bRet;
    }
    case 6:     // 双精度小数
    {
        bRet = __Edit_insert_char_min_max(ch, bUnsigned, 0, 0, 0, 0, 0, 0);
        return !bRet;
    }
    default:
        return false;
    }
    return false;
}

void CPropertyTree::OnEditShow(bool isShow)
{
    //HideCaret(m_edit);
    //auto f = m_items.find(m_hItemSel);
    //if (f == m_items.end()) return;
    //PROPERTY_ITEM& data = f->second;
    //if (!isEdit(data.type))
    //{
    //    HideCaret(m_edit);
    //}
    //else
    //{
    //    ShowCaret(m_edit);
    //}
}

void CPropertyTree::OnEditSetFocus()
{
    //HideCaret(m_edit);
    m_editCaret = 1;
    //auto f = m_items.find(m_hItemSel);
    //if (f == m_items.end()) return;
    //PROPERTY_ITEM& data = f->second;
    //if (!isEdit(data.type))
    //{
    //    HideCaret(m_edit);
    //}
    //else
    //{
    //    ShowCaret(m_edit);
    //}
    //m_edit.SetPropW(L"hItemSel", m_hItemSel);
    //wstr::dbg(L"获得焦点\n");
}

void CPropertyTree::OnEditKillFocus()
{
    modify_value_from_edit();
    m_edit.hide();

}

void CPropertyTree::OnListBoxKillFocus()
{
    m_listBox.hide();
}

void CPropertyTree::OnComboboxChanged(int index)
{
    if (index < 0)
    {
        wstr::dbg(L"没有选中任何表项\n");
        return;
    }
    m_listBox.hide();
    auto f = m_items.find(m_hItemSel);
    if (f == m_items.end())
    {
        wstr::dbg(L"当前选中项目没有存在map里, 项目句柄 = 0x%p, 索引 = %d\n", m_hItemSel, index);
        return;
    }
    PROPERTY_ITEM& data = f->second;
    if (!data.comboboxData)
    {
        wstr::dbg(L"当前选中项目数组为空, 属性名 = %s, 索引 = %d\n", data.name.c_str(), index);
        return;
    }
    std::vector<PROPERTY_ITEM>& arr = *data.comboboxData;
    size_t len = arr.size();
    if (index >= (int)len)
    {
        wstr::dbg(L"点击的索引比数组成员大, 数组成员 = %d, 索引 = %d\n", (int)len, index);
        return;
    }

    if (m_evt.pfnComboboxChanged && m_evt.pfnComboboxChanged(m_evt.pfnComboboxChangedParam, m_hItemSel, data.type, index))
        return;
    data.value = arr[index].name;
    m_edit.SetText(data.value.c_str());
    data.index = index;
    OnItemPropChanged(m_hItemSel, data);

    InvalidateItem(m_hItemSel);
}

bool CPropertyTree::OnItemDelDown()
{
    if (!m_hItemSel)return false;
    PROPERTY_ITEM& data = m_items[m_hItemSel];
    // 字体, 长文本, 字节集 不需要删除键按下事件, 只需要放开, 因为只可以删除一次
    if (data.type == PROPERTY_TYPE_FONT || data.type == PROPERTY_TYPE_TEXT_LONG || data.type == PROPERTY_TYPE_BIN)
        return true;
    return false;
}

void CPropertyTree::OnItemDelUp(bool isName)
{
    if (!m_hItemSel)return;
    PROPERTY_ITEM& data = m_items[m_hItemSel];

    if (m_evt.pfnItemDelUp && m_evt.pfnItemDelUp(m_evt.pfnItemDelUpParam, m_hItemSel, data.type, isName))
        return;
    if (!isName)    // 不是在属性名这一列按下
    {
        // 并且不是 字体, 长文本, 字节集类型, 则返回
        if (!(data.type == PROPERTY_TYPE_FONT || data.type == PROPERTY_TYPE_TEXT_LONG || data.type == PROPERTY_TYPE_BIN))
            return;
    }

    // 走到这里就是 字体, 长文本, 字节集, 直接清空值, 设置编辑框为空, 重画项目
    data.value.clear();
    m_edit.SetText(L"");
    InvalidateItem(m_hItemSel);
    //_str::dbg("delete放开\n");
}

void CPropertyTree::moveEdit_Static(HTREEITEM hItem)
{
    if (!hItem)return;
    RECT rc = GetItemRect(hItem);
    //wstr::dbg(L"滚动, 位置 = %d, %d, %d, %d\n", rc.left, rc.top, rc.right, rc.bottom);
    int mid = calcMidLine();
    //bool isVisibleStatic = m_rbutton.isVisible;
    bool isVisibleStatic = m_static.IsVisible();
    bool isVisibleEdit = m_edit.IsVisible();


    RECT rcText;
    if (isVisibleStatic)    // 按钮可视的情况下, 编辑框要减去按钮的宽度
    {
        rcText = { mid + 5, rc.top, rc.right - mid - m_itemHeight - 5, rc.bottom - rc.top - 1 };
    }
    else
    {
        rcText = { mid + 5, rc.top, rc.right - mid - 6, rc.bottom - rc.top - 1 };
    }
    if (!_canvas_isDxRender())
    {
        rcText.top += 1;
    }

    auto f = m_items.find(hItem);
    if (f != m_items.end())
    {
        PROPERTY_ITEM& data = f->second;
        if (IsColorDialog(data.type))
        {
            rcText.left += COLORBLOCK_WIDTH;
            rcText.right -= COLORBLOCK_WIDTH;
        }

    }

    SetWindowPos(m_edit, 0, rcText.left, rcText.top, rcText.right, rcText.bottom, SWP_NOZORDER | SWP_FRAMECHANGED);
    if (isVisibleEdit)
    {
        m_edit.InvalidateRect();
    }

    m_rbutton.rc = { 0 };
    if (isVisibleStatic)
    {
        m_rbutton.rc.left = rc.right - m_itemHeight;
        m_rbutton.rc.top = rc.top;
        m_rbutton.rc.right = m_rbutton.rc.left + m_itemHeight;
        m_rbutton.rc.bottom = m_rbutton.rc.top + m_itemHeight - 1;
        //InvalidateRect(&m_rbutton.rc);

        m_static.Move(rc.right - m_itemHeight, rc.top, m_itemHeight, m_itemHeight - 1, true);
        m_static.InvalidateRect();
    }

    if (m_listBox.IsVisible())
    {
        //TODO 取哪个位置
        RECT rcScreen;
        GetRect(m_hWnd, 0, 0, &rcScreen);
        m_listBox.Move(rcScreen.top + mid + 2, rcScreen.top + rc.bottom + 3, rc.right - mid, m_listBox.GetHeight());
    }
}

void CPropertyTree::OnValueButtonClick(int type)
{
    if (!m_hItemSel)return;
    auto f = m_items.find(m_hItemSel);
    if (f == m_items.end())return;
    PROPERTY_ITEM& data = f->second;

    modify_value_from_edit();

    if (type == 0)  // 按钮被单击
    {
        m_listBox.hide();
        m_edit.hide();
        wstr::dbg(L"按钮被单击, 根据不同属性做不同事情, 项目文本 = %s\n", data.name.c_str());
        if (m_evt.pfnItemButtonClick && m_evt.pfnItemButtonClick(m_evt.pfnItemButtonClickParam, m_hItemSel, data.type))
            return;
        if (IsDisabled(data.type))
            return;
        if (IsText(data.type) && IsButton(data.type))   // 是文本, 并且带按钮
        {
            wstr text = data.value;
            HANDLE param[3] = { &text, m_hWnd };
            param[2] = (HANDLE)IsLongText(data.type);
            INT_PTR b = DialogBoxPop(0, L"请输入文本:", WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_CENTER | DS_3DLOOK,
                0, 0, 300, 200, m_hWnd, CWndBase::GetInst(), DlgProc_InputText, param);

            if (b && data.value != text)
            {
                data.value = text;
                OnItemPropChanged(m_hItemSel, data);
                InvalidateItem(m_hItemSel);
            }
        }
        else if (IsColorDialog(data.type))  // 颜色选择器
        {
            static bool first = false;
            static COLORREF crDef[16] = { 0 };
            if (!first)
            {
                first = true;
                crDef[0] = RGB(255, 255, 255);
                crDef[1] = RGB(192, 192, 192);
                crDef[2] = RGB(128, 128, 128);
                crDef[3] = RGB(0, 0, 0);
                crDef[4] = RGB(255, 0, 0);
                crDef[5] = RGB(128, 0, 0);
                crDef[6] = RGB(255, 255, 0);
                crDef[7] = RGB(128, 128, 0);
                crDef[8] = RGB(0, 255, 0);
                crDef[9] = RGB(0, 128, 0);
                crDef[10] = RGB(0, 255, 255);
                crDef[11] = RGB(0, 128, 128);
                crDef[12] = RGB(0, 0, 255);
                crDef[13] = RGB(0, 0, 128);
                crDef[14] = RGB(255, 0, 255);
                crDef[15] = RGB(128, 0, 128);
            }
            CHOOSECOLORW cc = { 0 };
            cc.lStructSize = sizeof(cc);
            cc.hwndOwner = m_hWnd;
            cc.rgbResult = crDef[11];   // 起始颜色, 也是返回颜色
            cc.lpCustColors = crDef;
            cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
            if (!ChooseColorW(&cc))
                return;

            data.value.assign(cc.rgbResult);
            OnItemPropChanged(m_hItemSel, data);
            InvalidateItem(m_hItemSel);
        }
        else if (IsFileDialog(data.type))   // 文件选择器
        {
            OPENDLG_DATA& dlg = m_map_openDlg[PROPERTY_FLAG_FILE];
            LPCWSTR title = dlg.title.c_str();
            if (!title || !title[0])
                title = L"文件对话框 - 选择文件";
            LPWSTR filter = dlg.filter.data();
            wstr path = Dialog_Open(filter, title, dlg.defName.c_str(), dlg.path.c_str(), 0, dlg.noChangePath);
            if (path.empty())
                return;
            data.value.assign(std::move(path));
            OnItemPropChanged(m_hItemSel, data);
            InvalidateItem(m_hItemSel);
        }
        else if (IsBinDialog(data.type))    // 字节集
        {
            OPENDLG_DATA& dlg = m_map_openDlg[PROPERTY_FLAG_BIN];
            LPCWSTR title = dlg.title.c_str();
            if (!title || !title[0])
                title = L"字节集对话框 - 选择文件";
            LPWSTR filter = dlg.filter.data();
            wstr path = Dialog_Open(filter, title, dlg.defName.c_str(), dlg.path.c_str(), 0, dlg.noChangePath);
            if (path.empty())
                return;
            FILE* f;
            errno_t err = _wfopen_s(&f, path.c_str(), L"rb+");
            if (f)
            {
                fseek(f, 0, SEEK_END);
                int size = ftell(f);
                fseek(f, 0, SEEK_SET);
                _str buf(size);
                size = (int)fread(buf.data(), 1, size, f);
                fclose(f);
                data.value.clear();
                SetBinData(data, buf.c_str(), size);
                OnItemPropChanged(m_hItemSel, data);
            }
            else
            {
                if (m_evt.pfnItemReadError) m_evt.pfnItemReadError(m_evt.pfnItemReadErrorParam, m_hItemSel, data.type, err);
            }
            InvalidateItem(m_hItemSel);
        }
        else if (IsFontDialog(data.type))   // 字体选择器
        {
            CHOOSEFONTW cf = { 0 };

            LPLOGFONTW lplf = new LOGFONTW;
            memset(lplf, 0, sizeof(LOGFONTW));
            if (data.value.empty())
                GetObjectW(CWndBase::DefaultFont(), sizeof(LOGFONTW), lplf);
            else
                memcpy(lplf, data.value.c_str(), sizeof(LOGFONTW));

            cf.lStructSize = sizeof(CHOOSEFONTW);
            cf.hwndOwner = m_hWnd;
            cf.lpLogFont = lplf;
            cf.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_BOTH | CF_FORCEFONTEXIST | CF_LIMITSIZE;
            cf.nFontType = REGULAR_FONTTYPE;
            cf.rgbColors = 0;
            cf.nSizeMin = 8;
            cf.nSizeMax = 72;
            if (!ChooseFontW(&cf))
            {
                delete lplf;
                return;
            }
            SetFontData(data, &cf);
            OnItemPropChanged(m_hItemSel, data);
            InvalidateItem(m_hItemSel);
            delete lplf;
        }
        return;
    }




    if (m_evt.pfnDropWown && m_evt.pfnDropWown(m_evt.pfnDropWownParam, m_hItemSel, data.type))
        return;

    //if (!data.comboboxData) // 组合框没有数据
    //{
    //    m_listBox.hide();
    //    return;
    //}

    // 组合框下拉按钮被单击
    const size_t len = data.comboboxData ? data.comboboxData->size() : 0;
    if (m_listBox.IsVisible())
    {
        m_listBox.hide();
        return;
    }
    if (IsDisabled(data.type))
        return;

    RECT rc;
    CalcListRect(data, *data.comboboxData, (int)(LONG_PTR)len, rc);
    m_listBox.SetCount((int)(LONG_PTR)len);
    m_listBox.SetIndex(data.index);
    m_listBox.SetTopIndex(data.index);
    modify_value_from_edit();

    SetWindowPos(m_listBox, HWND_TOP, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOACTIVATE);

    m_edit.hide();
    m_listBox.SetVisible(true);
    m_listBox.SetFocus();
    m_listBox.InvalidateRect();
}

void CPropertyTree::CalcListRect(PROPERTY_ITEM& data, std::vector<PROPERTY_ITEM>& arr, const int len, RECT& ret)
{
    HWND hParent = ::GetParent(m_hWnd);
    if (::GetParent(m_listBox) != hParent) ::SetParent(m_listBox, hParent);
    RECT rcScreen;  // 屏幕位置
    RECT rcClient;  // 窗口客户区在屏幕里的位置
    RECT rc;        // 属性框在父窗口中的位置
    ::GetWindowRect(m_hWnd, &rcScreen);
    ::GetClientRect(m_hWnd, &rcClient);
    rc = rcScreen;
    ScreenToClient(hParent, (LPPOINT)&rc);
    ScreenToClient(hParent, (((LPPOINT)&rc) + 1));
    const int cxClient = rcClient.right - rcClient.left;    // 客户区宽度, 不包含滚动条
    const int cyClient = rcClient.bottom - rcClient.top;    // 客户区高度, 不包含边框
    const int cxScreen = rcScreen.right - rcScreen.left;    // 窗口宽度, 包含滚动条
    const int cyScreen = rcScreen.bottom - rcScreen.top;    // 窗口高度, 包含边框
    int scrollWidth = (cxScreen - cxClient) - (cyScreen - cyClient) / 2;

    RECT rcItem = GetItemRect(m_hItemSel);  // 当前选中项目的位置
    const int mid = calcMidLine(cxClient);  // 属性栏在属性框里的左边位置

    const int maxWidth = cxScreen * 80 / 100;   // 最大支持属性框的80%宽度
    const int minWidth = cxClient - mid;        // 最小宽度就是属性栏的宽度, 属性框宽度 - 属性栏左边 = 属性栏宽度

    int height = len * m_itemHeight + 10;
    if (height > 220) { height = 220; }

    const int right = rc.right;     // 列表框的理想右边就是属性框的右边, 如果属性框有滚动条需要减去滚动条的位置
    const int bottom = rc.bottom;   // 列表框的理想底边是属性框的底边

    // 列表框显示的宽度
    const int width = len == 0 ? minWidth : CalcListWidth(data, arr, len, maxWidth, minWidth, cxClient, cyClient);
    if (len == 0) height = 10;
    ret.left = rc.right - width - scrollWidth;
    ret.right = rc.right - scrollWidth;
    ret.top = rc.top + rcItem.bottom;
    ret.bottom = ret.top + height;

    if (ret.bottom > rc.bottom) // 理想的高度大于组件的底边
    {
        // 先判断是否超过属性框的父窗口底边, 如果没超过, 继续使用计算出来的理想位置
        RECT rScreenParent;
        RECT rClientParent;
        ::GetWindowRect(hParent, &rScreenParent);
        ::GetClientRect(hParent, &rClientParent);
        const int cxClientParent = rClientParent.right - rClientParent.left;
        const int cyClientParent = rClientParent.bottom - rClientParent.top;

        HWND hParentParent = ::GetParent(hParent);
        if (hParentParent && (GetWindowLongPtrW(hParent, GWL_STYLE) & WS_CHILD) == WS_CHILD)    // 有父窗口且是子窗口才计算相对父窗口位置
        {
            ScreenToClient(hParentParent, (LPPOINT)&rScreenParent);
            rScreenParent.right = rScreenParent.left + cxClientParent;
            rScreenParent.bottom = rScreenParent.top + cyClientParent;
        }
        if (ret.bottom > cyClientParent) // 理想底边大于父窗口的高度
        {
            // 需要判断往上的情况能不能放, 如果不能需要判断上面空间大还是下面空间大, 把位置调整到空间大的地方
            const int topHeight = ret.top - m_itemHeight;             // 顶边可用高度 = 理想项目顶边 - 列表框项目高度
            const int bottomHeight = cyClientParent - ret.top;      // 底边可用高度

            if (height > ret.top - m_itemHeight)   // 下面不够放, 上面也不够放
            {
                // 往上放的位置底边大于点击的项目, 这里需要判断是放在上面还是下面, 看哪边空间大
                if (topHeight > bottomHeight + m_itemHeight)  // 优先放到下面, 上面至少比下面多一个项目才会移动到上面
                {
                    // 上面比下面空间大, 需要把组件底边调整到弹出表项的顶边
                    ret.bottom = ret.top - m_itemHeight;
                    ret.top = ret.bottom - ((topHeight < height) ? topHeight : height);
                }
                else
                {
                    // 下面比上面空间大, 需要把组件底边调整到父窗口的底边, 顶边位置不边, 改变底边
                    ret.bottom = ret.top + ((bottomHeight < height) ? bottomHeight : height);
                }
            }
            else
            {
                // 上面还够放
                ret.bottom = ret.top - m_itemHeight;
                ret.top = ret.bottom - height;
            }
        }
    }
    assert(ret.top >= 0);

}

int CPropertyTree::CalcListWidth(PROPERTY_ITEM& data, std::vector<PROPERTY_ITEM>& arr, const int len, const int maxWidth, int width, const int cxClient, const int cyClient)
{
    const float offset = IsIndex(data.type) ? m_draw.chWidth * 6.f : 0;
    for (int i = 0; i < len; i++)
    {
        size_t strLen = arr[i].name.size();
        if (strLen * m_draw.chWidth > maxWidth) // 文本长度*字符i的长度大于最大值, 设置宽度为最大值, 然后跳出循环
        {
            width = maxWidth;
            break;
        }
        float textWidth, textHeight;
        const DWORD fmt = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
        _canvas_calctextsize(m_hCanvas, m_draw.hFont, arr[i].name.c_str(), strLen, fmt, (float)cxClient, (float)cyClient, &textWidth, &textHeight);
        const int newWidth = (int)(textWidth + 25.f + offset) + 5;
        //wstr::dbg(L"最大宽度 = %d, 计算文本宽度 = %d, %s\n", maxWidth, newWidth, arr[i].name.c_str());
        if (newWidth > width)
        {
            if (newWidth > maxWidth) return maxWidth;   // 如果计算出的宽度比最大值还大, 那就直接返回最大值
            width = newWidth;
        }
    }
    return width;
}

wstr CPropertyTree::MakeCustomText(IPROPERTY_ITEM& data, HTREEITEM hItem)
{
    if (!IsCustom(data.type)) return wstr();
    wstr str(100);
    // 先调用用户处理的函数, 如果用户处理, 则不进行内部处理
    if (m_evt.pfnItemMakeCustomValue && m_evt.pfnItemMakeCustomValue(m_evt.pfnItemMakeCustomValueParam, hItem, data.type, str.data()))
    {
        str.resize(wcslen(str.c_str()));
        str.Addref();
        return str;
    }
    if (IsFontDialog(data.type))        // 字体
    {
        LPCHOOSEFONTW cf = 0;
        LPLOGFONTW lf = GetFontData(data, &cf);
        if (!lf) return wstr();

        str.assign(lf->lfFaceName);
        str.append(L", ").append(cf->iPointSize / 10);
        if (lf->lfWeight == FW_BOLD || lf->lfItalic || lf->lfUnderline)
        {
            str.append(L", ");
            if (lf->lfWeight == FW_BOLD) str.append(L"B");
            if (lf->lfItalic) str.append(L"I");
            if (lf->lfUnderline) str.append(L"U");
        }
    }
    else if (IsBinDialog(data.type))    // 字节集
    {
        int* pData = (int*)data.value.c_str();
        if (!pData) return wstr();
        if (pData[0] != 1) return wstr();
        str.assign(L"<字节集长度: ").append(pData[1]).append(L">");
    }
    else if (IsLongText(data.type))     // 长文本
    {
        str.assign(L"<文本长度: ").append((int)data.value.size()).append(L">");
    }
    str.Addref();
    return str;
}

bool CPropertyTree::calcListBoxRect(RECT* prc, bool isMove, int mid, const RECT& rcItem, const RECT& treeClientRect)
{
    //RECT rcScreen = GetRect();
    //int cxClient = treeClientRect.right - treeClientRect.left;
    //int mid = calcMidLine(cxClient);
    //int top = 1;
    //int height = len * m_itemHeight + 10;
    //if (height > 220)height = 220;
    //int listLen = m_listBox.GetCount();
    //if (listLen != (int)len)    // 不相等就需要重新调整列表项目数
    //{
    //    int offset = listLen - (int)len;    // 列表项目数减去实际项目数, 大于就需要删除, 小于需要添加
    //    if (offset > 0)
    //    {
    //        while (offset--)
    //            m_listBox.deleteItem(0);
    //    }
    //    else
    //    {
    //        while (offset++)
    //            m_listBox.insert();

    //    }
    //}
    //m_listBox.Move(rcScreen.top + mid + 1, rcScreen.top + rcItem.bottom + top, treeClientRect.right - mid, height);
    //m_listBox.SetVisible(true);
    //m_listBox.InvalidateRect();

    return false;
}

LRESULT CPropertyTree::OnVScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //SCROLLINFO si;
    //si.cbSize = sizeof(si);
    //si.fMask = SIF_ALL;
    //si.fMask = SIF_POS;
    //BOOL bRet = GetScrollInfo(hWnd, SB_VERT, &si);
    if (m_listBox.IsVisible())
        m_listBox.SetVisible(false);
    if (m_edit.IsVisible())
        m_edit.SetVisible(false);
    LRESULT ret = CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    if (m_hItemSel)
    {
        moveEdit_Static(m_hItemSel);
    }
    return ret;
    //int nMaxPos = si.nMax - si.nPage;
    //const int LINESIZE = 8;
    //int pos = HIWORD(wParam);
    //int scrValue = 0;
    //int code = LOWORD(wParam);
    //switch (code)
    //{
    //case SB_LINEUP:
    //    if (nMaxPos - si.nPos < LINESIZE)
    //        scrValue = -(nMaxPos - si.nPos);
    //    else
    //        scrValue = -LINESIZE;
    //    break;
    //case SB_PAGEUP:
    //    if (si.nPos)
    //    {
    //        scrValue = (si.nPos - si.nPage < 0) ? si.nPos : si.nPage;
    //    }
    //    break;
    //case SB_PAGEDOWN:
    //    if (si.nPage != nMaxPos)
    //    {
    //        scrValue = (nMaxPos - si.nPos < (int)si.nPage) ? (-(nMaxPos - si.nPos)) : -((int)si.nPage);
    //    }
    //    break;
    //
    //case SB_THUMBTRACK:
    //    scrValue = si.nPos - pos;
    //    break;
    //default:
    //    break;
    //}
    //ScrollWindow(hWnd, scrValue, 0, 0, 0);
    //si.fMask = SIF_POS;
    //si.nPos -= scrValue;
    //SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
    //return 0;
    //return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

HWND CPropertyTree::create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle, DWORD dwExStyleTree, int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID, LPVOID lpParam)
{
    static bool isinit = false;
    if (isinit == false)
    {
        isinit = true;
        IDrawScroll_Init();
    }
    if (IsWindow())return GetWindow();
    if (!s_image)
    {
        s_hCursor[0] = LoadCursorW(0, IDC_ARROW);
        s_hCursor[1] = LoadCursorW(0, IDC_SIZEWE);
        s_image = _img_create_frommemory(s_img_tree_exp, sizeof(s_img_tree_exp) / sizeof(s_img_tree_exp[0]));
    }
    m_hCursor = s_hCursor[0];

    DWORD oldStyle = dwStyle;
    dwStyle &= ~TVS_LINESATROOT;
    dwStyle &= ~TVS_HASBUTTONS;
    dwStyle |= WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    SubClass();
    m_data->isSubClass = true;
    if (!CWndBase::create(dwExStyle, lpszTitle, dwStyle, x, y, nWidth, nHeight, hWndParent, ID, lpParam))return 0;
    //IDrawScroll* drawScroll = new IDrawScroll;
    //drawScroll->m_isCallSetScroll = true;
    //drawScroll->bind(m_hWnd, false, true);
    

    m_edit.SubClass(WndProc_Edit);
    DWORD style = WS_CLIPSIBLINGS | WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL;
    m_edit.create(0, 0, style, 0, 0, 110, 110, m_hWnd, TREESUB_ID_EDIT);
    m_edit.SetPropW(L"pThis", this);


    m_listBox.SubClass(WndProc_ListBox);
    style = WS_CLIPSIBLINGS | WS_CHILD | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT | LBS_NODATA | WS_VSCROLL;
    m_listBox.create(0, 0, style, 0, 0, 0, 0, hWndParent, TREESUB_ID_LIST);

    SetItemHeight(20);
    m_listBox.SetPropW(L"pThis", this);


    style = WS_CLIPSIBLINGS | WS_CHILD | WS_CLIPCHILDREN | SS_NOTIFY;
    m_static.SubClass(WndProc_Static);
    m_static.create(0, 0, style, 0, 0, 0, 0, m_hWnd, TREESUB_ID_BTN);
    m_static.SetPropW(L"pThis", this);

    auto itParent = s_map_parent.find(hWndParent);
    if (itParent == s_map_parent.end())
    {
        s_map_parent[hWndParent] = (WNDPROC)SetWindowLongPtrW(hWndParent, GWLP_WNDPROC, (LONG_PTR)WndProc_Partent);
    }

    SetWindowPos(m_listBox, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW | SWP_NOACTIVATE);
    __create_d2d_resouce();
    return m_hWnd;
}

bool CPropertyTree::SetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR filter, LPCWSTR title, LPCWSTR defName, LPCWSTR path, bool* noChangePath)
{
    OPENDLG_DATA* vl = 0;
    switch (type)
    {
    case PROPERTY_FLAG_BIN:
    case PROPERTY_FLAG_FILE:
    {
        OPENDLG_DATA& data = m_map_openDlg[type];
        vl = &data;
        break;
    }
    default:
        break;
    }
    if (!vl) return false;

    if (filter)
    {
        vl->filter1 = filter;
        vl->filter.resize(vl->filter1.size() + 2);
        vl->filter = vl->filter1;
        LPWSTR p = vl->filter.data();
        while (*p)
        {
            if (*p == '|')
                *p = 0;
            ++p;
        }
    }
    if (defName)
        vl->defName = defName;
    if (title)
        vl->title = title;
    if (path)
        vl->path = path;
    if (noChangePath)
        vl->noChangePath = *noChangePath;
    return true;
}

bool CPropertyTree::GetOpenDialogProperty(PROPERTY_TYPE type, LPCWSTR* filter, LPCWSTR* title, LPCWSTR* defName, LPCWSTR* path, bool* noChangePath)
{
    OPENDLG_DATA* vl = 0;
    switch (type)
    {
    case PROPERTY_FLAG_BIN:
    case PROPERTY_FLAG_FILE:
    {
        OPENDLG_DATA& data = m_map_openDlg[type];
        vl = &data;
        break;
    }
    default:
        break;
    }
    if (!vl) return false;

    if (filter)
        *filter = vl->filter1.c_str();
    if (defName)
        *defName = vl->defName.c_str();
    if (title)
        *title = vl->title.c_str();
    if (path)
        *path = vl->path.c_str();
    if (noChangePath)
        *noChangePath = vl->noChangePath;
    return true;

}

HTREEITEM CPropertyTree::InsertGroup(LPCWSTR name, bool isExpand, LPCWSTR text, LPARAM param)
{
    HTREEITEM hItem = CTreeView::InsertItem();
    if (!hItem)return 0;
    PROPERTY_ITEM& data = m_items[hItem];
    data.type = PROPERTY_TYPE_GROUP;
    data.name = name;
    data.text = text;
    data.param = param;
    if (isExpand)
        Expand(hItem, true);
    return hItem;
}

bool CPropertyTree::SetItemValue(IPROPERTY_ITEM& data, const void* value)
{
    if (!value) return false;
    if (IsBinDialog(data.type))
    {
        SetBinData(data, value);
    }
    else if (IsFontDialog(data.type))
    {
        SetFontData(data, (LPLOGFONTW)value);
    }
    else if (IsCombobox(data.type))
    {

    }
    else if (IsNumber(data.type))
    {
        //TODO 需要判断类型, 数值型需要调整大小
        //int numberType = GetNumberType(data.type);
        //bool bUnsigned = isUnsigned(data.type);
        //double num = 0.0;
        //double input = wstr::stod((LPCWSTR)value);
        //switch (numberType)
        //{
        //case 1:     // 字节

        //case 2:     // 短整数
        //case 3:     // 整数
        //case 4:     // 长整数
        //case 5:     // 小数
        //default:    // 双精度小数
        //}
        data.value = (LPCWSTR)value;
    }
    else
    {
        data.value = (LPCWSTR)value;
    }
    return false;
}

bool CPropertyTree::SetItemRemarks(HTREEITEM hItem, LPCWSTR value)
{
    if (!hItem)return 0;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;

    IPROPERTY_ITEM& data = f->second;
    data.text = value;
    return true;
}

LPCWSTR CPropertyTree::GetItemRemarks(HTREEITEM hItem)
{
    if (!hItem)return 0;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return 0;

    IPROPERTY_ITEM& data = f->second;
    return data.text.c_str();
}



HTREEITEM CPropertyTree::InsertItem(HTREEITEM hParent, LPCWSTR name, LPCWSTR value, PROPERTY_TYPE type, LPCWSTR text, LPARAM param)
{
    if (hParent && type == PROPERTY_TYPE_GROUP) return 0;
    if (hParent)
    {
        auto f = m_items.find(hParent);
        if (f == m_items.end())return 0;
        if (f->second.type != PROPERTY_TYPE_GROUP) return 0;
    }
    HTREEITEM hItem = CTreeView::InsertItem(hParent);
    if (!hItem)return 0;
    PROPERTY_ITEM& data = m_items[hItem];
    data.type = type;
    data.text = text;
    data.param = param;
    if (name) data.name = name;
    SetItemValue(data, value);
    return hItem;
}

BOOL CPropertyTree::DeleteItem(HTREEITEM hItem)
{
    if (!hItem)return FALSE;
    TreeView_DeleteItem(m_hWnd, hItem);
    auto f = m_items.find(hItem);
    if (f == m_items.end())return FALSE;
    m_items.erase(f);
    return TRUE;
}

LPCWSTR CPropertyTree::GetItemName(HTREEITEM hItem)
{
    if (!hItem)return 0;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return 0;
    return f->second.name.c_str();
}

bool CPropertyTree::SetItemName(HTREEITEM hItem, LPCWSTR name)
{
    if (!hItem)return false;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    f->second.name = name;
    InvalidateItem(hItem);
    return true;
}

LPCWSTR CPropertyTree::GetItemValue(HTREEITEM hItem)
{
    if (!hItem)return 0;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return 0;
    return f->second.value.c_str();
}

bool CPropertyTree::SetItemValue(HTREEITEM hItem, LPCWSTR value)
{
    if (!hItem)return false;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    PROPERTY_ITEM& data = f->second;
    SetItemValue(data, value);
    InvalidateItem(hItem);
    return true;
}

LPARAM CPropertyTree::GetItemParam(HTREEITEM hItem)
{
    if (!hItem)return 0;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return 0;
    return f->second.param;
}

bool CPropertyTree::SetItemParam(HTREEITEM hItem, LPARAM param)
{
    if (!hItem)return false;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    f->second.param = param;
    return true;
}

size_t CPropertyTree::AddComboboxData(HTREEITEM hItem, LPCWSTR name, LPARAM param)
{
    if (!hItem)return -1;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return -1;

    PROPERTY_ITEM& data = f->second;
    if (!IsCombobox(data.type))
        data.type = PROPERTY_TYPE_COMBOBOX;
    return data.AddComboboxData(name, param);
}

bool CPropertyTree::DelComboboxData(HTREEITEM hItem, int index)
{
    if (!hItem)return false;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;

    PROPERTY_ITEM& data = f->second;
    if (!IsCombobox(data.type))
        return false;
    bool ret = data.DelComboboxData(index);
    if (ret && m_listBox.IsVisible())
    {
        m_listBox.deleteItem(0);
        m_listBox.InvalidateRect();
    }
    return ret;
}

size_t CPropertyTree::GetComboboxDataCount(HTREEITEM hItem)
{
    if (!hItem)return -1;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return -1;
    PROPERTY_ITEM& data = f->second;
    if (!IsCombobox(data.type))
        return -1;
    if (!data.comboboxData)return -1;
    return data.comboboxData->size();
}

LPCWSTR CPropertyTree::GetComboboxData(HTREEITEM hItem, int index)
{
    if (index < 0)return 0;
    if (!hItem)return 0;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return 0;
    PROPERTY_ITEM& data = f->second;
    if (!IsCombobox(data.type))
        return 0;
    if (!data.comboboxData)return 0;
    size_t len = data.comboboxData->size();
    if (index >= (int)len) return 0;
    return data.comboboxData->at(index).name.c_str();
}

bool CPropertyTree::SetComboboxData(HTREEITEM hItem, int index, LPCWSTR value)
{
    if (index < 0)return false;
    if (!hItem)return false;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    PROPERTY_ITEM& data = f->second;
    if (!IsCombobox(data.type))
        return false;
    if (!data.comboboxData)return false;
    size_t len = data.comboboxData->size();
    if (index >= (int)len) return false;
    data.comboboxData->at(index).name = value;
    return true;
}

int CPropertyTree::GetComboboxIndex(HTREEITEM hItem)
{
    if (!hItem)return -1;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return -1;
    PROPERTY_ITEM& data = f->second;
    if (!IsCombobox(data.type))
        return -1;
    if (!data.comboboxData)return -1;
    return data.index;
}

bool CPropertyTree::SetComboboxIndex(HTREEITEM hItem, int index)
{
    if (index < 0)return false;
    if (!hItem)return false;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    PROPERTY_ITEM& data = f->second;
    if (!IsCombobox(data.type))
        return false;
    if (!data.comboboxData)return false;
    size_t len = data.comboboxData->size();
    if ((size_t)index >= len) return false;
    data.index = index;
    data.value = data.comboboxData->at(index).name;
    InvalidateItem(hItem);
    return true;
}

LPARAM CPropertyTree::GetComboboxParam(HTREEITEM hItem, int index)
{
    if (index < 0)return 0;
    if (!hItem)return 0;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return 0;
    PROPERTY_ITEM& data = f->second;
    if (!IsCombobox(data.type))
        return 0;
    if (!data.comboboxData)return 0;
    size_t len = data.comboboxData->size();
    if (index >= (int)len) return 0;
    return data.comboboxData->at(index).param;
}

bool CPropertyTree::SetComboboxParam(HTREEITEM hItem, int index, LPARAM param)
{
    if (index < 0)return false;
    if (!hItem)return false;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    PROPERTY_ITEM& data = f->second;
    if (!IsCombobox(data.type))
        return false;
    if (!data.comboboxData)return false;
    size_t len = data.comboboxData->size();
    if (index >= (int)len) return false;
    data.comboboxData->at(index).param = param;
    return true;
}

void CPropertyTree::SetItemType(HTREEITEM hItem, PROPERTY_TYPE type)
{
    if (!hItem)return;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return;
    if (f == m_items.end())return;

    PROPERTY_ITEM& data = f->second;
    if (data.type == type) return;

    data.value.clear();
    int drawType = 0;   // 0=...按钮, 1=下拉按钮   
    bool isVisible = false;

    // 有按钮画按钮, 有下拉按钮画下拉按钮
    if (IsButton(type))
    {
        isVisible = true;
        drawType = 0;
    }
    else if (IsCombobox(type))
    {
        isVisible = true;
        drawType = 1;
        data.index = 0;
        if (data.comboboxData && data.comboboxData->size() > 0)
            data.value = data.comboboxData->at(0).name;
        else
            data.value.clear();
        m_edit.SetPropW(L"hItemSel", 0);
    }


    m_edit.SetWindowTextW(L"");
    m_edit.hide();
    m_static.SetVisible(isVisible);
    //m_rbutton.isVisible = isVisible;
    if (isVisible)
    {
        moveEdit_Static(hItem);
        //m_rbutton.state = STATE::NORMAL;
        //m_rbutton.type = drawType;
        ::SetPropW(m_static, L"state", (LPVOID)STATE::NORMAL);
        __static_set_type(m_static, drawType);  // 0=...按钮, 1=下拉按钮    
    }

    data.type = type;
    InvalidateItem(hItem);
}

PROPERTY_TYPE CPropertyTree::GetItemType(HTREEITEM hItem)
{
    if (!hItem)return PROPERTY_TYPE_UNKNOWN;
    auto f = m_items.find(hItem);
    if (f == m_items.end())return PROPERTY_TYPE_UNKNOWN;

    PROPERTY_ITEM& data = f->second;
    return data.type;
}

void CPropertyTree::SetWidthRatio(bool isLeft, int value, bool isUpdate)
{
    int width = max(200, min(900, value));
    if (width == m_leftWidth)return;
    m_leftWidth = width;
    if (isUpdate)
        InvalidateRect();
}



LPBYTE CPropertyTree::save(DWORD* pSize, bool isColor, bool isList)
{
    if (!isColor && !isList || !pSize)
    {
#ifdef _DEBUG
        throw L"保存个东西啊喂";
#endif
        return 0;
    }
    *pSize = 0;


    DWORD size = 0;     // 生成的数据尺寸, 文件头 + 颜色 + 数据
    DWORD offset = 0;
    _str listData;
    if (isList) listData = make_listdata();

    size = sizeof(LIST_PROP_HEAD) + sizeof(PROPERTY_COLOR) + (DWORD)listData.size();
    size += (m_expand.nClose + m_expand.nOpen + 8);
    LPBYTE pData = (LPBYTE)malloc(size);
    if (!pData)return 0;
    memset(pData, 0, size);

    LIST_PROP_HEAD* head = (LIST_PROP_HEAD*)pData;
    offset = sizeof(LIST_PROP_HEAD);
    head->header[0] = 'P';
    head->header[1] = 'D';
    head->MajorVersion = LIST_PROP_MAJORVERSION;
    head->MinorVersion = LIST_PROP_MINORVERSION;
    if (isColor)
    {
        head->flags |= LIST_PROP_HEAD_FLAG_COLOR;
        PROPERTY_COLOR* color = (PROPERTY_COLOR*)(pData + offset);
        offset += sizeof(PROPERTY_COLOR);
        color->crBackground = m_draw.crBackground.toGdiplus();
        color->crBackParent = m_draw.crBackParent.toGdiplus();
        color->crText = m_draw.crText.toGdiplus();
        color->crListBorder = m_draw.crListBorder.toGdiplus();
        color->crSelect = m_draw.crSelect.toGdiplus();
        color->crDisabled = m_draw.crDisabled.toGdiplus();;
        color->ratioWidth = m_leftWidth;
        color->ItemHeight = m_itemHeight;
        memcpy(&color->lf, &m_draw.lf, sizeof(color->lf));
    }

    if (isList)
    {
        head->flags |= LIST_PROP_HEAD_FLAG_LIST;
        memcpy(pData + offset, listData.c_str(), listData.size());
        offset += (DWORD)(LONG_PTR)listData.size();
    }
    if (1)   // 有加减框图标数据
    {
        head->flags |= LIST_PROP_HEAD_FLAG_EXPAND;

        // 加减框图标保存两个, 分别是 图标长度+图标数据, 图标长度+图标数据, 如果两个都没保存的话, 那就保存8个字节的0
        int imgSIze = m_expand.nOpen;
        memcpy(pData + offset, &imgSIze, sizeof(imgSIze)); offset += sizeof(imgSIze);

        if (m_expand.pOpen && imgSIze) memcpy(pData + offset, m_expand.pOpen, imgSIze);
        offset += imgSIze;

        imgSIze = m_expand.nClose;
        memcpy(pData + offset, &imgSIze, sizeof(imgSIze)); offset += sizeof(imgSIze);

        if (m_expand.pClose && imgSIze) memcpy(pData + offset, m_expand.pClose, imgSIze);
        offset += imgSIze;

    }
    *pSize = size;
    return pData;
}

void CPropertyTree::GetPropertyItemRect(HTREEITEM hItem, RECT* prc, PROPERTY_RECT type)
{
    if (!prc)return;
    auto f = m_items.find(hItem);
    if (f == m_items.end()) return;
    IPROPERTY_ITEM& data = f->second;


    RECT rc = GetItemRect(hItem);
    memset(prc, 0, sizeof(RECT));
    const bool bGroup = IsGroup(data.type);
    int width = calcMidLine();

    switch (type)
    {
    case PROPERTY_RECT_ITEM:
        *prc = rc;
        break;
    case PROPERTY_RECT_WINDOW:
        GetWindowRect(prc);
        break;
    case PROPERTY_RECT_NAME:
    {
        rc.left += leftSize;
        if (bGroup) // 是父项目
        {
            *prc = rc;
            break;
        }
        rc.right = width;
        *prc = rc;
        break;
    }
    case PROPERTY_RECT_VALUE:
        rc.left += leftSize;
        if (bGroup) // 是父项目
            break;

        rc.left = width;
        *prc = rc;
        break;
    case PROPERTY_RECT_BUTTON:
    {
        if (!IsButton(data.type) && !IsCombobox(data.type))break; // 没有按钮且不是组合框则返回空矩形
        rc.left = rc.right - m_itemHeight;
        rc.bottom = rc.top + m_itemHeight - 1;
        *prc = rc;
        break;
    }
    case PROPERTY_RECT_EXPAND:
    {
        if (!bGroup)break;
        prc->left = (leftSize - 16) / 2;
        prc->top = rc.top + (m_itemHeight - 16) / 2;
        prc->right = prc->left + 16;
        prc->bottom = prc->top + 16;
        break;
    }
    default:
        break;
    }
}

LPBYTE CPropertyTree::GetExpand_Open(int* size)
{
    if (size)*size = m_expand.nOpen;
    return m_expand.pOpen;
}

bool CPropertyTree::SetExpand_Open(const void* pData, int size)
{
    if (!pData || size <= 0)
    {
        delete[] m_expand.pOpen;
        m_expand.pOpen = 0;
        m_expand.nOpen = 0;
        _img_destroy(m_expand.imgOpen);

        InvalidateRect();
        return true;
    }
    LPBYTE p = new BYTE[size];
    memcpy(p, pData, size);
    _img_destroy(m_expand.imgOpen);

    if (m_expand.pOpen) delete[] m_expand.pOpen;
    m_expand.pOpen = p;
    m_expand.nOpen = size;
    m_expand.imgOpen = _img_create_frommemory(pData, size);
    InvalidateRect();
    return true;
}

LPBYTE CPropertyTree::GetExpand_Close(int* size)
{
    if (size)*size = m_expand.nClose;
    return m_expand.pClose;
}

bool CPropertyTree::SetExpand_Close(const void* pData, int size)
{
    if (!pData || size <= 0)
    {
        delete[] m_expand.pClose;
        m_expand.pClose = 0;
        m_expand.nClose = 0;
        _img_destroy(m_expand.imgClose);
        InvalidateRect();
        return true;
    }
    LPBYTE p = new BYTE[size];
    memcpy(p, pData, size);
    _img_destroy(m_expand.imgClose);

    if (m_expand.pClose) delete[] m_expand.pClose;
    m_expand.pClose = p;
    m_expand.nClose = size;
    m_expand.imgClose = _img_create_frommemory(pData, size);
    InvalidateRect();
    return true;
}

_str CPropertyTree::make_listdata()
{
    HTREEITEM* hItems;
    int count = EnumItem(hItems);
    if (!count)
    {
        _str buf(20);
        buf.append((LPCSTR)&count, sizeof(int));
        FreeEnumData(hItems);
        return buf;
    }

    size_t bufSize = count * sizeof(LIST_PROP_DATA) * 10;
    //size_t bufSize = 50;
    _str buf(bufSize * 100);
    int zero = 0;
    buf.append((LPCSTR)&count, sizeof(int));
    LPBYTE p = ((LPBYTE)buf.c_str()) + sizeof(int);
    LIST_PROP_DATA_1_1* ptr = (LIST_PROP_DATA_1_1*)p;
    int offset = 0;

    for (int i = 0; i < count; i++)
    {
        PROPERTY_ITEM& data = m_items[hItems[i]];
        int subLen = (data.comboboxData ? (int)data.comboboxData->size() : 0);

        LPCWSTR name = data.name.c_str();
        LPCWSTR value = data.value.c_str();
        LPCWSTR text = data.text.c_str();
        if (!name)name = L"";
        if (!value)value = L"";
        if (!text)text = L"";

        int nameLen = (int)data.name.size() * sizeof(wchar_t) + sizeof(wchar_t);
        int valueLen = (int)data.value.size() * sizeof(wchar_t) + sizeof(wchar_t);
        int textLen = (int)data.text.size() * sizeof(wchar_t) + sizeof(wchar_t);

        if (IsBinDialog(data.type))
        {
            static int binData[2] = { 1, 0 };
            if (valueLen == 2)
            {
                value = (LPCWSTR)binData;
            }
            valueLen = ((int*)value)[1] + 8;

        }
        else if (IsFontDialog(data.type))
        {
            valueLen = sizeof(CHOOSEFONTW) + sizeof(LOGFONTW);
        }
        ptr->type = data.type;
        ptr->index = data.index;
        ptr->param = data.param;
        ptr->subItemCount = subLen;
        ptr->nameLen = nameLen;
        ptr->valueLen = valueLen;
        ptr->textLen = textLen;

        // 这一个表项的项目数据长度, 不包含组合框数据长度
        const int nItemBufSize = nameLen + valueLen + textLen + (sizeof(LIST_PROP_DATA_1_1) * 2);
        if (offset + nItemBufSize > (int)bufSize)
        {
            bufSize *= 2;
            bufSize += nItemBufSize;

            buf.resize(bufSize);
            p = ((LPBYTE)buf.c_str()) + sizeof(int);
            ptr = (LIST_PROP_DATA_1_1*)(p + offset);
        }

        offset += sizeof(LIST_PROP_DATA_1_1);
        memcpy(p + offset, name, nameLen);      offset += nameLen;
        memcpy(p + offset, value, valueLen);    offset += valueLen;
        memcpy(p + offset, text, textLen);      offset += textLen;
        ptr = (LIST_PROP_DATA_1_1*)(p + offset);

        for (int n = 0; n < subLen; n++)
        {
            PROPERTY_ITEM& sub = data.comboboxData->at(n);
            name = sub.name.c_str();
            if (!name)name = L"";
            value = L"";
            text = L"";

            nameLen = (int)sub.name.size() * sizeof(wchar_t) + sizeof(wchar_t);
            valueLen = sizeof(wchar_t);
            textLen = sizeof(wchar_t);

            ptr->type = 0;
            ptr->index = 0;
            ptr->param = sub.param;
            ptr->subItemCount = 0;
            ptr->nameLen = nameLen;
            ptr->valueLen = valueLen;
            ptr->textLen = textLen;
            const int nItemBufSize = nameLen + valueLen + textLen + (sizeof(LIST_PROP_DATA_1_1) * 2);
            if (offset + nItemBufSize > (int)bufSize)
            {
                bufSize *= 2;
                bufSize += nItemBufSize;

                buf.resize(bufSize);
                p = ((LPBYTE)buf.c_str()) + sizeof(int);
                ptr = (LIST_PROP_DATA_1_1*)(p + offset);
            }

            offset += sizeof(LIST_PROP_DATA_1_1);
            memcpy(p + offset, name, nameLen);      offset += nameLen;
            memcpy(p + offset, value, valueLen);    offset += valueLen;
            memcpy(p + offset, text, textLen);      offset += textLen;
            ptr = (LIST_PROP_DATA_1_1*)(p + offset);
        }

    }
    FreeEnumData(hItems);
    const int reSize = offset + 4;
    buf.resize(reSize);
    return buf;
}

bool CPropertyTree::load(const void* data, DWORD size, bool isUpdate)
{
    LPBYTE pData = (LPBYTE)data;
    LIST_PROP_HEAD* head = (LIST_PROP_HEAD*)pData;
    pData = (LPBYTE)(head + 1);
    if (size > 0x7fffffff || size <= sizeof(LIST_PROP_HEAD)) return false;

    if (head->MajorVersion == 1 && head->MinorVersion == 0 || head->MajorVersion == 1 && head->MinorVersion == 1)
    {
        // 1.0数据结构
        Load_Color(head, pData, isUpdate);
        Load_Insert_Tree(head, pData);
        Load_Expand(head, pData);
    }
    return true;
}


bool CPropertyTree::Load_Insert_Tree(LIST_PROP_HEAD* head, LPBYTE& pData)
{
    if ((head->flags & LIST_PROP_HEAD_FLAG_LIST) != LIST_PROP_HEAD_FLAG_LIST) return false;

    const bool is1_1 = head->MajorVersion == 1 && head->MinorVersion == 1;
    const int offsize = is1_1 ? sizeof(LIST_PROP_DATA_1_1) : sizeof(LIST_PROP_DATA);
    int count = *(int*)pData;
    pData += sizeof(int);
    LIST_PROP_DATA* data = (LIST_PROP_DATA*)pData;
    clear();
    m_items.clear();
    HTREEITEM hItem = 0;

    for (int i = 0; i < count; i++)
    {
        pData = (LPBYTE)(data);
        pData += offsize;

        LPCWSTR name = (LPCWSTR)(pData);    pData += data->nameLen;
        LPCWSTR value = (LPCWSTR)(pData);   pData += data->valueLen;
        LPCWSTR text = L"";
        if (is1_1)
        {
            text = (LPCWSTR)(pData);
            pData += ((LIST_PROP_DATA_1_1*)data)->textLen;
        }

        if (IsGroup((PROPERTY_TYPE)data->type))
        {
            if (hItem) Expand(hItem, true);
            hItem = InsertGroup(name, true, text, (LPARAM)data->param);
        }
        else
        {
            HTREEITEM hSub = InsertItem(hItem, name, 0, (PROPERTY_TYPE)data->type, text);

            PROPERTY_ITEM& sub = m_items[hSub];

            int len = data->valueLen / 2 - 1;
            if (IsBinDialog((PROPERTY_TYPE)data->type))
            {
                const int reSize = data->valueLen / 2 + 1;
                sub.value.resize(reSize);
                memcpy(sub.value.data(), value, data->valueLen);
            }
            else if (IsFontDialog((PROPERTY_TYPE)data->type))
            {
                const int reSize = data->valueLen / 2 + 1;
                sub.value.resize(reSize);
                memcpy(sub.value.data(), value, data->valueLen);
            }
            else if (len > 0)
            {
                sub.value.assign(value, len);
            }

            sub.param = (LPARAM)data->param;
            sub.index = data->index;
            if (data->subItemCount)
            {
                const int subCount = data->subItemCount;
                for (int n = 0; n < subCount; n++)
                {
                    data = (LIST_PROP_DATA*)pData;
                    pData += offsize;

                    name = (LPCWSTR)(pData);    pData += data->nameLen;
                    value = (LPCWSTR)(pData);   pData += data->valueLen;
                    if (is1_1)
                    {
                        text = (LPCWSTR)(pData);
                        pData += ((LIST_PROP_DATA_1_1*)data)->textLen;
                    }

                    sub.AddComboboxData(name, (LPARAM)data->param);
                }
            }
        }
        data = (LIST_PROP_DATA*)pData;
    }
    if (hItem) Expand(hItem, true);
    EnsureVisible(GetRoot());
    return true;
}
void CPropertyTree::SetColor(const PROPERTY_COLOR* color, bool isUpdate)
{
    static PROPERTY_COLOR defColor;
    if (defColor.ratioWidth == 0)
    {
        GetObjectW(CWndBase::DefaultFont(), sizeof(LOGFONTW), &defColor.lf);
        if (m_defColor == 0)
        {
            defColor.crBackground = ARGB_D2D::toArgb(RGB(37, 37, 38), 255);
            defColor.crBackParent = ARGB_D2D::toArgb(RGB(45, 45, 48), 255);
            defColor.crText = ARGB_D2D::toArgb(RGB(241, 241, 241), 255);
            defColor.crListBorder = ARGB_D2D::toArgb(RGB(100, 100, 100), 255);
            defColor.crSelect = ARGB_D2D::toArgb(RGB(51, 153, 255), 255);
            defColor.crDisabled = ARGB_D2D::toArgb(RGB(128, 128, 128), 255);
        }
        else
        {
            defColor.crBackground = ARGB_D2D::toArgb(RGB(254, 254, 254), 255);
            defColor.crBackParent = ARGB_D2D::toArgb(RGB(200, 200, 199), 255);
            defColor.crText = ARGB_D2D::toArgb(RGB(0, 0, 0), 255);
            defColor.crListBorder = ARGB_D2D::toArgb(RGB(100, 100, 100), 255);
            defColor.crSelect = ARGB_D2D::toArgb(RGB(0, 120, 215), 255);
            defColor.crDisabled = ARGB_D2D::toArgb(RGB(128, 128, 128), 255);
        }
        defColor.ratioWidth = 500;
        defColor.ItemHeight = m_itemHeight;
    }
    if (!color) color = &defColor;
    __delete_d2d_resouce(false);
    memcpy(&m_draw.lf, &color->lf, sizeof(LOGFONTW));


    m_leftWidth = color->ratioWidth;
    m_draw.crBackground(color->crBackground);
    m_draw.crBackParent(color->crBackParent);
    m_draw.crText(color->crText);
    m_draw.crListBorder(color->crListBorder);
    m_draw.crSelect(color->crSelect);
    m_draw.crDisabled(color->crDisabled);
    m_draw.chHeight = 0.f;
    m_draw.chWidth = 0.f;


    SetItemHeight(color->ItemHeight);
    m_draw.lf.lfWeight = FW_NORMAL;

    m_rcCalcText.left = _font_height2size(m_draw.lf.lfHeight) / 3 - 1;
    //m_rcCalcText.left = _font_height2size(m_draw.lf.lfHeight) / 3 - 1;

    m_draw.hFont = _font_create_logfont(&m_draw.lf, 0);
    if (!m_draw.hFont)
    {
        GetObjectW(CWndBase::DefaultFont(), sizeof(LOGFONTW), &m_draw.lf);
        m_draw.hFont = _font_create_logfont(&m_draw.lf, 0);
    }
    m_draw.hFontGdi = CreateFontIndirectW(&m_draw.lf);
    SendMessageW(m_edit, WM_SETFONT, (WPARAM)m_draw.hFontGdi, 1);
    m_draw.lf.lfWeight = FW_BOLD;
    m_draw.hFontParent = _font_create_logfont(&m_draw.lf, 0);

    // 背景颜色
    m_draw.crBackgroundGdi = m_draw.crBackground.toRgb();
    m_draw.hbrBackgroundGdi = CreateSolidBrush(m_draw.crBackgroundGdi);
    m_draw.hbrBackground = _brush_create(m_draw.crBackground);

    // 左边/父项目背景颜色
    m_draw.crBackParentGdi = m_draw.crBackParent.toRgb();
    m_draw.hbrParent = _brush_create(m_draw.crBackParent);
    m_draw.hpenParent = _pen_create(m_draw.crBackParent);

    // 文本颜色
    m_draw.crTextGdi = m_draw.crText.toRgb();
    m_draw.hbrText = _brush_create(m_draw.crText);

    // 线条颜色
    m_draw.crListBorderGdi = m_draw.crListBorder.toRgb();
    m_draw.hpenListBorder = _pen_create(m_draw.crListBorder);
    m_draw.hpenListBorderGdi = CreatePen(PS_SOLID, 1, m_draw.crListBorderGdi);

    // 选中颜色
    m_draw.crSelectGdi = m_draw.crSelect.toRgb();
    m_draw.hbrSelectGdi = CreateSolidBrush(m_draw.crSelectGdi);
    m_draw.hbrSelect = _brush_create(m_draw.crSelect);

    // 文本禁止
    m_draw.crDisabledGdi = m_draw.crDisabled.toRgb();
    m_draw.hbrDisabled = _brush_create(m_draw.crDisabled);

    if (isUpdate)
    {
        if (m_edit.IsVisible())m_edit.InvalidateRect();
        if (m_listBox.IsVisible())m_listBox.InvalidateRect();
        InvalidateRect();
    }
}

int CPropertyTree::GetColor(PROPERTY_COLOR* color)
{
    if (!color)return m_defColor;
    memcpy(&color->lf, &m_draw.lf, sizeof(LOGFONTW));
    color->crBackground = m_draw.crBackground.toGdiplus();
    color->crBackParent = m_draw.crBackParent.toGdiplus();
    color->crText = m_draw.crText.toGdiplus();
    color->crListBorder = m_draw.crListBorder.toGdiplus();
    color->crSelect = m_draw.crSelect.toGdiplus();
    color->crDisabled = m_draw.crDisabled.toGdiplus();
    color->ratioWidth = m_leftWidth;
    color->ItemHeight = m_itemHeight;
    return m_defColor;
}

int CPropertyTree::DefColor(int type, bool isUpdate)
{
    static PROPERTY_COLOR cr[2];
    if (cr[0].ratioWidth == 0)
    {
        GetObjectW(CWndBase::DefaultFont(), sizeof(LOGFONTW), &cr[0].lf);
        GetObjectW(CWndBase::DefaultFont(), sizeof(LOGFONTW), &cr[1].lf);
        cr[0].crBackground = ARGB_D2D::toArgb(RGB(37, 37, 38), 255);
        cr[0].crBackParent = ARGB_D2D::toArgb(RGB(45, 45, 48), 255);
        cr[0].crText = ARGB_D2D::toArgb(RGB(241, 241, 241), 255);
        cr[0].crListBorder = ARGB_D2D::toArgb(RGB(100, 100, 100), 255);
        cr[0].crSelect = ARGB_D2D::toArgb(RGB(51, 153, 255), 255);
        cr[0].crDisabled = ARGB_D2D::toArgb(RGB(128, 128, 128), 255);
        cr[0].ItemHeight = 20;

        cr[1].crBackground = ARGB_D2D::toArgb(RGB(254, 254, 254), 255);
        cr[1].crBackParent = ARGB_D2D::toArgb(RGB(200, 200, 199), 255);
        cr[1].crText = ARGB_D2D::toArgb(RGB(0, 0, 0), 255);
        cr[1].crListBorder = ARGB_D2D::toArgb(RGB(100, 100, 100), 255);
        cr[1].crSelect = ARGB_D2D::toArgb(RGB(0, 120, 215), 255);
        cr[1].crDisabled = ARGB_D2D::toArgb(RGB(128, 128, 128), 255);
        cr[1].ItemHeight = 20;
    }
    m_defColor = max(0, min(1, type));
    PROPERTY_COLOR* p = (m_defColor == 0 ? &cr[0] : &cr[1]);
    p->ratioWidth = m_leftWidth;
    p->ItemHeight = m_itemHeight;
    if (p->ratioWidth < 100)p->ratioWidth = 500;
    if (!m_hWnd) return m_defColor;
    SetColor(p, isUpdate);
    return m_defColor;
}

int CPropertyTree::SetItemFont(const LPLOGFONTW lf, bool isUpdate)
{
    if (!lf)return m_defColor;
    memcpy(&m_draw.lf, lf, sizeof(LOGFONTW));
    _font_destroy(m_draw.hFont);
    _font_destroy(m_draw.hFontParent);
    DeleteObject(m_draw.hFontGdi);

    m_draw.lf.lfWeight = FW_NORMAL;
    m_draw.hFont = _font_create_logfont(&m_draw.lf, 0);
    m_draw.hFontGdi = CreateFontIndirectW(&m_draw.lf);

    m_draw.lf.lfWeight = FW_BOLD;
    m_draw.hFontParent = _font_create_logfont(&m_draw.lf, 0);

    if (isUpdate) InvalidateRect();
    m_defColor = -1;
    return m_defColor;
}

int CPropertyTree::SetItemFont(bool isUpdate, const wchar_t* name, int size, bool lfItalic, bool lfUnderline, bool lfStrikeOut)
{
    CWndBase::CreateFontW();
    LOGFONTW lf = { 0 };
    SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);
    lf.lfHeight = size;
    lf.lfWeight = FW_NORMAL;
    lf.lfItalic = lfItalic;
    lf.lfUnderline = lfUnderline;
    lf.lfStrikeOut = lfStrikeOut;
    lf.lfCharSet = GB2312_CHARSET;
    if (!name) name = L"微软雅黑";
    size_t len = wcslen(name) + 1;
    memcpy(lf.lfFaceName, name, len * sizeof(wchar_t));
    SetItemFont(&lf, isUpdate);
    return m_defColor;
}


void CPropertyTree::Load_Color(LIST_PROP_HEAD* head, LPBYTE& pData, bool isUpdate)
{
    if ((head->flags & LIST_PROP_HEAD_FLAG_COLOR) == LIST_PROP_HEAD_FLAG_COLOR)
    {
        // 有配色信息
        PROPERTY_COLOR* color = (PROPERTY_COLOR*)pData;
        pData = (LPBYTE)(color + 1);
        SetColor(color, isUpdate);
        if (color->ItemHeight <= 0 || color->ItemHeight > 65535)
            color->ItemHeight = m_itemHeight;
        SetItemHeight(color->ItemHeight, true);
    }
}

void CPropertyTree::Load_Expand(LIST_PROP_HEAD* head, LPBYTE& pData)
{
    if ((head->flags & LIST_PROP_HEAD_FLAG_EXPAND) == LIST_PROP_HEAD_FLAG_EXPAND)   // 有加减框图标数据
    {
        // 加减框图标保存两个, 分别是 图标长度+图标数据, 图标长度+图标数据, 如果两个都没保存的话, 那就保存8个字节的0
        int size = *((int*)pData);
        pData += sizeof(int);
        SetExpand_Open(pData, size);
        pData += size;

        size = *((int*)pData);
        pData += sizeof(int);
        SetExpand_Close(pData, size);
        pData += size;

    }
}

int CPropertyTree::SetColorBackground(ARGB cr, bool isUpdate)
{
    m_draw.crBackground = cr;
    m_draw.crBackgroundGdi = m_draw.crBackground.toRgb();
    _brush_setcolor(m_draw.hbrBackground, m_draw.crBackground);
    DeleteObject(m_draw.hbrBackgroundGdi);
    m_draw.hbrBackgroundGdi = CreateSolidBrush(m_draw.crBackgroundGdi);
    if (isUpdate) InvalidateRect();
    m_defColor = -1;
    return m_defColor;
}

int CPropertyTree::SetColorBackParent(ARGB cr, bool isUpdate)
{
    m_draw.crBackParent = cr;
    m_draw.crBackParentGdi = m_draw.crBackParent.toRgb();
    _brush_setcolor(m_draw.hbrParent, m_draw.crBackParent);
    _pen_destroy(m_draw.hpenParent);
    m_draw.hpenParent = _pen_create(m_draw.crBackParent);
    if (isUpdate) InvalidateRect();
    m_defColor = -1;
    return m_defColor;
}

int CPropertyTree::SetColorText(ARGB cr, bool isUpdate)
{
    m_draw.crText = cr;
    m_draw.crTextGdi = m_draw.crText.toRgb();
    _brush_setcolor(m_draw.hbrText, m_draw.crText);
    if (isUpdate) InvalidateRect();
    m_defColor = -1;
    return m_defColor;
}

int CPropertyTree::SetColorListBorder(ARGB cr, bool isUpdate)
{
    m_draw.crListBorder = cr;
    m_draw.crListBorderGdi = m_draw.crListBorder.toRgb();
    _pen_setcolor(m_draw.hpenListBorder, m_draw.crListBorder);
    DeleteObject(m_draw.hpenListBorderGdi);
    m_draw.hpenListBorderGdi = CreatePen(PS_SOLID, 1, m_draw.crListBorderGdi);
    if (isUpdate) InvalidateRect();
    m_defColor = -1;
    return m_defColor;
}

int CPropertyTree::SetColorSelect(ARGB cr, bool isUpdate)
{
    m_draw.crSelect = cr;
    m_draw.crSelectGdi = m_draw.crSelect.toRgb();
    _brush_setcolor(m_draw.hbrSelect, m_draw.crSelect);
    DeleteObject(m_draw.hbrSelectGdi);
    m_draw.hbrSelectGdi = CreateSolidBrush(m_draw.crSelectGdi);
    if (isUpdate) InvalidateRect();
    m_defColor = -1;
    return m_defColor;
}

int CPropertyTree::SetColorDisabled(ARGB cr, bool isUpdate)
{
    m_draw.crDisabled = cr;
    m_draw.crDisabledGdi = m_draw.crSelect.toRgb();
    _brush_setcolor(m_draw.hbrDisabled, m_draw.crDisabled);
    if (isUpdate) InvalidateRect();
    m_defColor = -1;
    return m_defColor;
}

bool CPropertyTree::DisabledItem(HTREEITEM hItem)
{
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    PROPERTY_ITEM& data = f->second;
    data.type = (PROPERTY_TYPE)(data.type | PROPERTY_FLAG_DISABLED);
    InvalidateItem(hItem);
    return true;
}

bool CPropertyTree::EnabledItem(HTREEITEM hItem)
{
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    PROPERTY_ITEM& data = f->second;
    data.type = (PROPERTY_TYPE)(data.type & ~PROPERTY_FLAG_DISABLED);
    InvalidateItem(hItem);
    return true;
}

bool CPropertyTree::IsItemDisabled(HTREEITEM hItem)
{
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    PROPERTY_ITEM& data = f->second;
    return IsDisabled(data.type);
}

bool CPropertyTree::IsItemEnabled(HTREEITEM hItem)
{
    auto f = m_items.find(hItem);
    if (f == m_items.end())return false;
    PROPERTY_ITEM& data = f->second;
    return IsEnabled(data.type);
}
