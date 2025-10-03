#pragma once
#include <tstr.h>
#include <d2d/Color.h>
// 从字符串里获取取ARGB的值, 如果需要RGB的值可以使用 ARGB_GET 来获取对应的通道
// str = 要转成rgb的字符串, 支持 RGB(0xFF,12,85) ARGB(,,,) 0xXXXX, XXXX
// defValue = 转换失败返回的值
// isNumber = 传入的文本是否为数值
inline DWORD GetArgbFromString(LPCWSTR str, DWORD defValue, bool* isNumber)
{
    if (!str || !str[0])
    {
        if (isNumber)*isNumber = true;
        return defValue;
    }
    if (isNumber)*isNumber = false;
    LPCWSTR ptr = str;
    if (wcsnicmp_api(ptr, L"rgb", 3) == 0)  // 传入的是RGB(,,) 格式
    {
        ptr += 3;
        ptr = wcschr(ptr, '(');
        if (!ptr)
        {
            if (isNumber)*isNumber = true;
            return defValue;
        }

        BYTE rgb[4] = { 0 };
        int i = 0;
        wstr tmp(ptr + 1);
        wchar_t* next_token = 0;
        wchar_t* _s = wcstok_api(tmp.data(), L",", &next_token);

        while (_s)
        {
            while (*_s == ' ')++_s;
            if (wcsnicmp_api(ptr, L"0x", 2) == 0)
                rgb[i++] = (BYTE)wcstol(ptr, 0, 16);
            else
                rgb[i++] = (BYTE)wstr::stoi(_s);
            if (i >= 3)break;
            _s = wcstok_api(NULL, L",", &next_token);
        }

        return MAKEARGB(255, rgb[0], rgb[1], rgb[2]);
    }
    if (wcsnicmp_api(ptr, L"argb", 4) == 0)  // 传入的是ARGB(,,,) 格式
    {
        ptr += 4;
        ptr = wcschr(ptr, '(');
        if (!ptr)
        {
            if (isNumber)*isNumber = true;
            return defValue;
        }

        BYTE rgb[4] = { 0 };
        int i = 0;
        wstr tmp(ptr + 1);
        wchar_t* next_token = 0;
        wchar_t* _s = wcstok_api(tmp.data(), L",", &next_token);

        while (_s)
        {
            while (*_s == ' ')++_s;
            if (wcsnicmp_api(ptr, L"0x", 2) == 0)
                rgb[i++] = (BYTE)wcstol(ptr, 0, 16);
            else
                rgb[i++] = (BYTE)wstr::stoi(_s);
            if (i >= 4)break;
            _s = wcstok_api(NULL, L",", &next_token);
        }
        return MAKEARGB(rgb[0], rgb[1], rgb[2], rgb[3]);
    }

    while (*ptr == ' ')ptr++;
    const wchar_t* pos = wcschr(ptr, ',');   // 查看输入的是否为 xxx,xxx,xxx
    if (pos)     // 输入的是 xxx,xxx,xxx
    {
        BYTE rgb[4] = { 0 };
        int i = 0;
        wstr tmp(ptr);
        wchar_t* next_token = 0;
        wchar_t* _s = wcstok_api(tmp.data(), L",", &next_token);

        while (_s)
        {
            while (*_s == ' ')++_s;
            if (wcsnicmp_api(ptr, L"0x", 2) == 0)
                rgb[i++] = (BYTE)wcstol(ptr, 0, 16);
            else
                rgb[i++] = (BYTE)wstr::stoi(_s);
            if (i >= 4)break;
            _s = wcstok_api(NULL, L",", &next_token);
        }
        if (i <= 3)     // 没有输入4个字节, 视为输入了RGB, 默认透明度255
            return MAKEARGB(255, rgb[0], rgb[1], rgb[2]);
        return MAKEARGB(rgb[0], rgb[1], rgb[2], rgb[3]);
    }

    if (isNumber)*isNumber = true;
    DWORD ret = 0;
    if (wcsnicmp_api(ptr, L"0x", 2) == 0)   // 输入的是16进制
    {
        
        ret = wcstol(ptr, 0, 16);
        return ret;
    }
    // 不是RGB DWORD 0x, 那就当成十进制处理
    ret = wcstol(ptr, 0, 10);
    return ret;
}

// 把字符串转成rgb颜色值
// str = 要转成rgb的字符串, 支持 RGB(0xFF,12,85) DWORD(,,,) 0xXXXX, XXXX
// defValue = 转换失败返回的值
inline COLORREF str2rgb(LPCWSTR str, COLORREF defValue)
{
    bool isNumber = false;
    COLORREF cr = GetArgbFromString(str, defValue, &isNumber);
    if (isNumber)return cr;
    return ARGB2RGB(cr);
}

inline wstr rgb2str(COLORREF cr)
{
    const int r = RGB_GETR(cr);
    const int g = RGB_GETG(cr);
    const int b = RGB_GETB(cr);
    wstr ret;
    ret.Format(L"RGB(%d, %d, %d)", r, g, b);
    return ret;
}

inline DWORD str2argb(LPCWSTR str, DWORD defValue)
{
    return GetArgbFromString(str, defValue, 0);
}
inline wstr argb2str(DWORD cr)
{
    const int a = ARGB_GETA(cr);
    const int r = ARGB_GETR(cr);
    const int g = ARGB_GETG(cr);
    const int b = ARGB_GETB(cr);
    wstr ret;
    ret.Format(L"ARGB(%d, %d, %d, %d)", a, r, g, b);
    return ret;
}




// 在Windows系统下，HSL分量的范围是[0，240].参考“画笔”程序，可以看到RGB（红|绿|蓝）
// 和HSL（色调|饱和度|亮度）的联系。
// 下面的代码，把COLOR_HSL的分量值变为Windows的HSL分量，取值在[0，240]之间，需要：
// 下面为COLOR_HSL到Windows的HSL的转换：
// win_H = 240 * hsl.hue / 360.f;
// win_S = 240 * hsl.saturation / 100.f;
// win_L = 240 * hsl.luminance / 100.f;
typedef struct
{
    float hue;              // [0,360]
    float saturation;       // [0,100]
    float luminance;        // [0,100]
}COLOR_HSL;

// RGB转HSL, l是颜色, 取值范围0-100
inline void RGB2HSL(COLORREF rgb, COLOR_HSL* hsl)
{
#define min3v(v1, v2, v3)   ((v1)>(v2)? ((v2)>(v3)?(v3):(v2)):((v1)>(v3)?(v3):(v2)))
#define max3v(v1, v2, v3)   ((v1)<(v2)? ((v2)<(v3)?(v3):(v2)):((v1)<(v3)?(v3):(v1)))

    float h = 0, s = 0, l = 0;
    // normalizes red-green-blue values
    float r = GetRValue(rgb) / 255.0f;
    float g = GetGValue(rgb) / 255.0f;
    float b = GetBValue(rgb) / 255.0f;
    float maxVal = max3v(r, g, b);
    float minVal = min3v(r, g, b);

    // hue
    if (maxVal == minVal)
    {
        h = 0; // undefined
    }
    else if (maxVal == r && g >= b)
    {
        h = 60.0f * (g - b) / (maxVal - minVal);
    }
    else if (maxVal == r && g < b)
    {
        h = 60.0f * (g - b) / (maxVal - minVal) + 360.0f;
    }
    else if (maxVal == g)
    {
        h = 60.0f * (b - r) / (maxVal - minVal) + 120.0f;
    }
    else if (maxVal == b)
    {
        h = 60.0f * (r - g) / (maxVal - minVal) + 240.0f;
    }

    // luminance
    l = (maxVal + minVal) / 2.0f;

    // saturation
    if (l == 0 || maxVal == minVal)
    {
        s = 0;
    }
    else if (0 < l && l <= 0.5f)
    {
        s = (maxVal - minVal) / (maxVal + minVal);
    }
    else if (l > 0.5f)
    {
        s = (maxVal - minVal) / (2 - (maxVal + minVal)); //(maxVal-minVal > 0)?
    }

    hsl->hue = (h > 360) ? 360 : ((h < 0) ? 0 : h);
    hsl->saturation = ((s > 1) ? 1 : ((s < 0) ? 0 : s)) * 100;
    hsl->luminance = ((l > 1) ? 1 : ((l < 0) ? 0 : l)) * 100;
}


// Converts HSL to RGB
inline COLORREF HSL2RGB(const COLOR_HSL* hsl)
{
    float h = hsl->hue;                  // h must be [0, 360]
    float s = hsl->saturation / 100.f; // s must be [0, 1]
    float l = hsl->luminance / 100.f;      // l must be [0, 1]
    float R, G, B;
    if (hsl->saturation == 0)
    {
        // achromatic color (gray scale)
        R = G = B = l * 255.0f;
    }
    else
    {
        float q = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - (l * s));
        float p = (2.0f * l) - q;
        float Hk = h / 360.0f;
        float T[3];
        T[0] = Hk + 0.3333333f; // Tr   0.3333333f=1.0/3.0
        T[1] = Hk;              // Tb
        T[2] = Hk - 0.3333333f; // Tg
        for (int i = 0; i < 3; i++)
        {
            if (T[i] < 0) T[i] += 1.0f;
            if (T[i] > 1) T[i] -= 1.0f;
            if ((T[i] * 6) < 1)
            {
                T[i] = p + ((q - p) * 6.0f * T[i]);
            }
            else if ((T[i] * 2.0f) < 1) //(1.0/6.0)<=T[i] && T[i]<0.5
            {
                T[i] = q;
            }
            else if ((T[i] * 3.0f) < 2) // 0.5<=T[i] && T[i]<(2.0/3.0)
            {
                T[i] = p + (q - p) * ((2.0f / 3.0f) - T[i]) * 6.0f;
            }
            else T[i] = p;
        }
        R = T[0] * 255.0f;
        G = T[1] * 255.0f;
        B = T[2] * 255.0f;
    }


    unsigned char red = (unsigned char)((R > 255) ? 255 : ((R < 0) ? 0 : R));
    unsigned char green = (unsigned char)((G > 255) ? 255 : ((G < 0) ? 0 : G));
    unsigned char blue = (unsigned char)((B > 255) ? 255 : ((B < 0) ? 0 : B));
    return RGB(red, green, blue);
}

