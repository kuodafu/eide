#pragma once
#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ�ļ����ų�����ʹ�õ�����


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
#define INI_APPNAME         L"BackgroundImage"  // ���ý���


extern HINSTANCE g_hInst;           // ��ǰ�����ģ����
extern LPIDRAW_INFO g_info;
extern IGetEWindow* g_eWnd;
extern ID2DDraw* g_dx;
extern std::wstring m_imagePath;    // �����ȡͼƬ��Ŀ¼, ����Ŀ¼, �����'\'

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
    if (GetLastError() == ERROR_FILE_NOT_FOUND) // û�д������������, ��Ҫд��
    {
        WritePrivateProfileStringW(INI_APPNAME, lpszKeyName, szDefValue, szConfigFileName);
    }
    else
    {
        // ������������, �������������ڴ�
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