// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#if defined(DEBUG) || defined(_DEBUG)
    #include <assert.h>
#else
    #ifndef assert
        #define assert(_s) 
    #endif
#endif

#define DLL_GUID "{16907696-7D30-41FB-AD92-44F9E75B17AA}"

#include "../include/iDraw.h"
#include <assist/CPrivateProfile.h>
extern LPIDRAW_INFO g_info;

class CLineCodeConfig
{
public:
    COLORREF m_crHot;                       // 行号焦点颜色
    COLORREF m_crBack;                      // 背景颜色
    COLORREF m_crNormal;                    // 通常文本颜色
    COLORREF m_crExpand;                    // 行号展开颜色
    COLORREF m_crLine;                      // 行号线颜色

    int m_lineWidth;                        // 行号显示宽度, 需要设置个最小值和最大值

    bool m_isEnabled;                       // 是否启用行号插件
    bool m_isDrawLine;                      // 是否绘画行号线
    bool m_isDrawLineNumber;                // 是否绘画行号
    bool m_isBoldLine;                      // 行号线是否加粗
    bool m_isAutoBack;                      // 自动背景颜色, 根据 标志列背景 获取颜色值
    bool m_isAutoWidth;                     // 自动计算背景宽度, 根据行号字符数计算宽度
    float* m_dot;                           // 点线
    int m_dotCount;                         // 点线的数量
    int m_penStyle;                         // 画笔样式
    CPrivateProfile ini;
#define LINECODE_KEYNAME L"LineCode"
public:
    CLineCodeConfig()
        :m_crHot(0), m_crBack(0), m_crNormal(0), m_crExpand(0), m_lineWidth(0),
        m_isEnabled(false), m_isDrawLine(false), m_isDrawLineNumber(false), m_isAutoBack(false), m_isBoldLine(0),
        m_dotCount(0), m_dot(0), m_crLine(0), m_penStyle(0), m_isAutoWidth(false)
    {

    }
    inline void init()
    {
        ini.m_isWrite = true;
        LPCWSTR str = (LPCWSTR)iDraw_GetInfo(g_info, IGI_PATH_CONFIG, 0);
        ini.open(str);

        m_isEnabled         = ini.read_int(LINECODE_KEYNAME, L"isEnabled", 1) != 0;
        m_isDrawLine        = ini.read_int(LINECODE_KEYNAME, L"isDrawLine", 1) != 0;
        m_isDrawLineNumber  = ini.read_int(LINECODE_KEYNAME, L"isDrawLineNumber", 1) != 0;
        m_isBoldLine        = ini.read_int(LINECODE_KEYNAME, L"isBoldLine", 1) != 0;
        m_isAutoBack        = ini.read_int(LINECODE_KEYNAME, L"isAutoBack", 1) != 0;
        m_isAutoWidth       = ini.read_int(LINECODE_KEYNAME, L"isAutoWidth", 1) != 0;
        m_lineWidth         = ini.read_int(LINECODE_KEYNAME, L"lineWidth", 50);

        m_dot = ReadDot(m_dotCount, m_penStyle);

        if (m_lineWidth < 20 || m_lineWidth > 300)
        {
            m_lineWidth = 50;
            ini.write(LINECODE_KEYNAME, L"lineWidth", L"50");
        }

        m_crHot             = GetNodeColor(L"crHot"     , L"RGB(193,249,253)"   );
        m_crBack            = GetNodeColor(L"crBack"    , L"RGB(51,51,51)"      );
        m_crNormal          = GetNodeColor(L"crNormal"  , L"RGB(0,133,196)"     );
        m_crExpand          = GetNodeColor(L"crExpand"  , L"RGB(255,128,0)"     );
        m_crLine            = GetNodeColor(L"crLine"    , L"RGB(66,66,66)"      );

    }


    // 设置行号显示宽度, 最小20, 最大300, 越界会设置为默认50
    inline int lineWidth(int width)
    {
        if (width == m_lineWidth) return m_lineWidth;
        int ret = m_lineWidth;
        if (width < 20 || width > 300) width = 50;
        m_lineWidth = width;
        wchar_t num[50]; _itow_s(m_lineWidth, num, 50, 10);
        ini.write(LINECODE_KEYNAME, L"lineWidth", num);
        return ret;
    }


#define _get_value(_ret, _name) inline _ret _name(){ return m_##_name; }
#define _set_color(_name) inline COLORREF _name(COLORREF color){ if (m_##_name == color) return color; return SetColor(L###_name, m_##_name, color); }

#define _bool_fun(_name) \
    inline bool _name(bool b){\
        bool ret = b;\
        m_##_name = b;\
        ini.write(LINECODE_KEYNAME, L###_name, (b ? L"1" : L"0"));\
        return ret;\
    }
    _set_color(crBack)        // 设置背景颜色, 返回设置前的值
    _set_color(crHot)         // 设置热点文本颜色, 返回设置前的值
    _set_color(crNormal)      // 设置普通文本颜色, 返回设置前的值
    _set_color(crExpand)      // 设置展开文本颜色, 返回设置前的值
    _set_color(crLine)        // 设置行号线颜色, 返回设置前的值

    _get_value(COLORREF, crBack)        // 获取背景颜色
    _get_value(COLORREF, crLine)        // 获取行号线颜色
    _get_value(COLORREF, crHot)         // 获取热点文本颜色
    _get_value(COLORREF, crNormal)      // 获取普通文本颜色
    _get_value(COLORREF, crExpand)      // 获取展开文本颜色
    _get_value(int,      lineWidth)     // 获取行号宽度

    _get_value(bool, isDrawLine)        // 获取是否绘画行号线
    _get_value(bool, isEnabled)         // 获取是否启用行号插件
    _get_value(bool, isDrawLineNumber)  // 获取是否绘画行号
    _get_value(bool, isBoldLine)        // 获取是否加粗行号线
    _get_value(bool, isAutoBack)        // 自动背景颜色, 根据 标志列背景 获取颜色值
    _get_value(bool, isAutoWidth)       // 自动背景背景宽度, 根据行号字符数计算宽度

    _bool_fun(isDrawLine)               // 设置是否绘画行号线, 返回设置前的值
    _bool_fun(isEnabled)                // 设置是否启用行号插件, 返回设置前的值
    _bool_fun(isDrawLineNumber)         // 设置是否绘画行号, 返回设置前的值
    _bool_fun(isBoldLine)               // 设置是否加粗行号线, 返回设置前的值
    _bool_fun(isAutoBack)               // 自动背景颜色, 根据 标志列背景 获取颜色值
    _bool_fun(isAutoWidth)              // 自动背景背景宽度, 根据行号字符数计算宽度

public:
    // 设置指定节点颜色, 返回设置前的颜色
    inline COLORREF SetColor(LPCWSTR keyName, COLORREF& cr, COLORREF newColor)
    {
        COLORREF ret = cr;
        cr = newColor;
        wchar_t buf[20] = { 0 };
        iDraw_IDCall(IDC_IDRAW_RGB2STR, newColor, (LPARAM)buf);
        ini.write(LINECODE_KEYNAME, keyName, buf);
        return ret;
    }
    // 获取指定节点的颜色值
    inline COLORREF GetNodeColor(LPCWSTR keyName, LPCWSTR defValue)
    {
        wstr ret = ini.read(LINECODE_KEYNAME, keyName, defValue);
        return iDraw_IDCall(IDC_IDRAW_STR2RGB, 0, (LPARAM)ret.c_str());
    }
    // 释放 ReadDot 返回的指针
    inline void FreeDot(float* pDot)
    {
        delete[] pDot;
    }
    // 从配置项里读取点的数组, 返回数组数据, 不使用时需要调用 FreeDot() 释放
    // count = 参考返回数组成员数
    // style = 画笔样式
    inline float* ReadDot(int& count, int& style)
    {
        wstr dot = ini.read(LINECODE_KEYNAME, L"dot", L"0");
        count = (int)dot.GetStringCount(L",") + 1;
        if (count == 1)
        {
            style = dot.stoi();
            if (style < 0 || style > 4)
                style = 0;
            count = 0;
            return 0;
        }

        style = 5;
        float* arr = new float[count];
        int index = 0;
        memset(arr, 0, sizeof(float) * count);
        LPWSTR p = dot.data();
        LPWSTR pFirst = p;
        while (*p)
        {
            wchar_t& ch = *p++;
            if (ch == ',')
            {
                ch = 0;
                arr[index++] = wstr::stof(pFirst);
                pFirst = p;
            }
            else if (ch == '{')
            {
                pFirst = p;
                continue;
            }
            else if (ch == ' ')
            {
                pFirst = p;
                continue;
            }
            else if (ch == '}')
            {
                ch = 0;
                arr[index++] = wstr::stof(pFirst);
                break;
            }
            else if (*p == 0)
            {
                arr[index++] = wstr::stof(pFirst);
            }
        }
        return arr;
    }
};

extern IGetEWindow* g_eWnd;     // 获取易语言窗口句柄
extern CLineCodeConfig g_ini;   // 程序配置
extern HINSTANCE g_hInst;       // 当前插件的模块句柄
extern HWND g_hEWnd;       // 易语言主界面窗口句柄

HWND LoadSettingWindow();

// 内存DC, 位图
typedef struct MEMDCBITMAP
{
    HDC hdc;            // 内存DC
    HBITMAP hBitmap;    // 32位绘制位图
    int width;          // 位图宽度, width * height = pBits 像素点数量, 像素点数量 * 4 = pBits 的字节数
    int height;         // 位图高度, width * height = pBits 像素点数量, 像素点数量 * 4 = pBits 的字节数
    LPBYTE lpBits;      // 指向位图数据指针, 可以直接修改这个地址里的数据达到绘画的效果, 4个字节为一个像素点
}*PMEMDCBITMAP, * LPMEMDCBITMAP;

// 释放内存DC
// pmd = md_create() 返回的值
// isFree = 是否释放pmd这个指针, 如果为假, 内部只释放hdc和hBitmap并清零, 为真则把pmd也释放
inline bool md_destroy(LPMEMDCBITMAP pmd, bool isFree = true)
{
    if (!pmd)return false;
    bool ret = false;
    if (pmd->hdc)
    {
        DeleteObject(pmd->hBitmap);
        DeleteDC(pmd->hdc);
        memset(pmd, 0, sizeof(MEMDCBITMAP));
        ret = true;
    }
    if (isFree)
        delete pmd;
    return ret;
}

// 重新调整内存dc尺寸, 如果尺寸一样则不修改, 如果传入的md没有创建dc, 则内部会创建一个
// pmd = 需要重新调整尺寸的md
// width = 位图宽度
// height = 位图高度
inline bool md_resize(LPMEMDCBITMAP pmd, int width, int height)
{
    if (!pmd)return false;
    if (width == pmd->width && height == pmd->height && width > 0 && height > 0) return true;

    if (width <= 0)width = 1;
    if (height <= 0)height = 1;
    pmd->width = width;
    pmd->height = height;

    HDC hdc = pmd->hdc;
    if (!hdc) hdc = CreateCompatibleDC(0);
    if (hdc)
    {
        BITMAPINFO bi = { 0 };
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biSizeImage = width * height * bi.bmiHeader.biBitCount / 8;
        void* lpBits = 0;
        HBITMAP hBmp = CreateDIBSection(hdc, &bi, 0, &lpBits, 0, 0);
        if (hBmp)
        {
            if (pmd->hBitmap)
                DeleteObject(pmd->hBitmap);

            SelectObject(hdc, hBmp);
            pmd->hdc = hdc;
            pmd->hBitmap = hBmp;
            pmd->lpBits = (LPBYTE)lpBits;
            pmd->width = width;
            pmd->height = height;
            return true;
        }

        DeleteDC(hdc);
        return false;
    }
    return false;
}
// 创建内存dc和32位位图
// width = 位图宽度
// height = 位图高度
// pmd = 结果保存位置, 如果这个值不为0, 则内部会把申请的hdc和hBitmap写入这个指针, 返回值是返回这个指针
inline LPMEMDCBITMAP md_create(int width, int height, LPMEMDCBITMAP pmd = 0)
{
    if (width <= 0)width = 1;
    if (height <= 0)height = 1;
    if (pmd)
    {
        md_destroy(pmd, false);
    }
    else
    {
        pmd = new MEMDCBITMAP;
        memset(pmd, 0, sizeof(MEMDCBITMAP));
    }

    md_resize(pmd, width, height);
    return pmd;
}


#endif //PCH_H
