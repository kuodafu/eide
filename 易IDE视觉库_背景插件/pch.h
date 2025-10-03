#pragma once
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容


#include "../include/iDraw.h"
#include <string>

#if defined(DEBUG) || defined(_DEBUG)
#include <assert.h>
#else
#ifndef assert
#define assert(_s) 
#endif
#endif

#define RGB2ARGB(_cr,a)     ((DWORD)(BYTE)(a) << 24) | ((_cr & 0x000000ff) << 16) | ((_cr & 0x0000ff00)) | ((_cr & 0x00ff0000) >> 16)
#define DLL_GUID "{408DB622-122B-4D5D-BDFA-4BBC0483498C}"
#define INI_APPNAME         L"BackgroundImage"  // 配置节名


extern HINSTANCE g_hInst;           // 当前插件的模块句柄
extern LPIDRAW_INFO g_info;
extern IGetEWindow* g_eWnd;
extern ID2DDraw* g_dx;
extern std::wstring m_imagePath;    // 插件读取图片的目录, 完整目录, 后面带'\'

HWND Setting_Open();

template<typename T>
inline T scale(T n)
{
    int dpi = iDraw_GetInfo(g_info, IGI_DPI, 0);
    float scale = (float)dpi / 96.f;
    return static_cast<T>(static_cast<float>(n * scale));
}

inline std::wstring ini_read(LPCWSTR lpszKeyName, LPCWSTR szDefValue = 0)
{
    LPCWSTR szConfigFileName = (LPCWSTR)iDraw_GetInfo(g_info, IGI_PATH_CONFIG, 0);

    DWORD bufferSize = (DWORD)260;
    LPWSTR buffer = new wchar_t[bufferSize];

    DWORD len = GetPrivateProfileStringW(INI_APPNAME, lpszKeyName, szDefValue, buffer, bufferSize, szConfigFileName);
    if (GetLastError() == ERROR_FILE_NOT_FOUND) // 没有创建这个配置项, 需要写出
    {
        WritePrivateProfileStringW(INI_APPNAME, lpszKeyName, szDefValue, szConfigFileName);
    }
    else
    {
        // 缓冲区不够大, 继续分配更大的内存
        while (len == bufferSize - 1 || len == bufferSize - 2)
        {
            bufferSize *= 2;
            delete[] buffer;
            buffer = new wchar_t[bufferSize];
            len = GetPrivateProfileStringW(INI_APPNAME, lpszKeyName, szDefValue, buffer, bufferSize, szConfigFileName);
        }
    }
    std::wstring retStr = buffer;
    delete[] buffer;
    return retStr;
}
inline int ini_read_int(LPCWSTR lpszKeyName, int nDefValue = 0)
{
    wchar_t text[50];
    swprintf_s(text, 50, L"%d", nDefValue);
    std::wstring ret = ini_read(lpszKeyName, text);
    return _wtoi(ret.c_str());
}
inline int ini_write(LPCWSTR lpszKeyName, LPCWSTR value)
{
    LPCWSTR szConfigFileName = (LPCWSTR)iDraw_GetInfo(g_info, IGI_PATH_CONFIG, 0);
    return WritePrivateProfileStringW(INI_APPNAME, lpszKeyName, value, szConfigFileName);
}
inline int ini_write(LPCWSTR lpszKeyName, int value)
{
    wchar_t str[50];
    _itow_s(value, str, 50, 10);
    return ini_write(lpszKeyName, str);
}