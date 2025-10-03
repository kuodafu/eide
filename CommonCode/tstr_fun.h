#pragma once
#if _MSC_VER > 1200
#include <type_traits>
//#include "traits.h"
#include <vcruntime_exception.h>
#include <iostream>
#endif

template<typename T> inline void tstr_free(T* p)
{
    if ( p )
        free(p);
}
template<typename T> inline T* tstr_alloc(size_t size)
{
    if ( size == 0 ) size = 1;
    size_t len = size + 1;
    for ( ;;)
    {
        size_t bufSize = len * sizeof(T);
        T* p = (T*)malloc(bufSize);
        if ( p )
        {
            *p = 0;
            return p;
        }
#if _MSC_VER > 1200
        if ( _callnewh(len) == 0 )
        {
            if ( len == SIZE_MAX )
                throw std::bad_array_new_length{};
            else
                throw std::bad_alloc{};
        }
#else
        return 0;
#endif
    }
}
// 重新分配内存, 不把旧数据写入新分配的内存里, 返回新分配的内存, 释放旧的内存
template<typename T> inline T* tstr_realloc(T*& p, int size)
{
    tstr_free(p);
    p = tstr_alloc<T>(size);
    return p;
}

namespace tstr_hide
{
    inline int tstr_vsnprintf_s(char* const _Buffer, size_t const _BufferCount, char const* const _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return _vsnprintf_s(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#else
        typedef int(__cdecl* fn_vsnprintf_s)( char* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, char const* const _Format, va_list _ArgList );
        static fn_vsnprintf_s fnFun;
        if ( fnFun )
            return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
        HMODULE hModule = GetModuleHandleW(L"msvcrt.dll");
        if ( !hModule )
        {
            hModule = LoadLibraryW(L"msvcrt.dll");
            if ( !hModule )
                return vsprintf(_Buffer, _Format, _ArgList);
        }
        fnFun = (fn_vsnprintf_s)GetProcAddress(hModule, "_vsnprintf_s");
        if ( !fnFun )
            return vsprintf(_Buffer, _Format, _ArgList);
        return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#endif
    }

    inline int tstr_vsnwprintf_s(wchar_t* const _Buffer, size_t const _BufferCount, wchar_t const* const _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return _vsnwprintf_s(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#else
        typedef int(__cdecl* fn_vsnwprintf_s)( wchar_t* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, wchar_t const* const _Format, va_list _ArgList );
        static fn_vsnwprintf_s fnFun;
        if ( fnFun )
            return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
        HMODULE hModule = GetModuleHandleW(L"msvcrt.dll");
        if ( !hModule )
        {
            hModule = LoadLibraryW(L"msvcrt.dll");
            if ( !hModule )
                return vswprintf(_Buffer, _Format, _ArgList);
        }

        fnFun = (fn_vsnwprintf_s)GetProcAddress(hModule, "_vsnwprintf_s");
        if ( !fnFun )
            return vswprintf(_Buffer, _Format, _ArgList);
        return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#endif
    }
    inline int tstr_vsctprintf(char const* const _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return _vscprintf(_Format, _ArgList);
#else
        typedef int(__cdecl* fn_vscprintf)( char const* const _Format, va_list _ArgList );
        static fn_vscprintf fnFun;
        if ( fnFun )
            return fnFun(_Format, _ArgList);
        HMODULE hModule = GetModuleHandleW(L"msvcrt.dll");
        if ( !hModule )
            hModule = LoadLibraryW(L"msvcrt.dll");
        
        if ( hModule )
        {
            fnFun = (fn_vscprintf)GetProcAddress(hModule, "_vscprintf");
            if ( fnFun ) return fnFun(_Format, _ArgList);
        }
        // 走到这里就是失败了, 返回一个默认值吧, 不知道vc6有什么代替
        return 260;
#endif
    }
    
    inline int tstr_vsctprintf(wchar_t const* const _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return _vscwprintf(_Format, _ArgList);
#else
        typedef int(__cdecl* fn_vscprintf)( wchar_t const* const _Format, va_list _ArgList );
        static fn_vscprintf fnFun;
        if ( fnFun )
            return fnFun(_Format, _ArgList);
        HMODULE hModule = GetModuleHandleW(L"msvcrt.dll");
        if ( !hModule )
            hModule = LoadLibraryW(L"msvcrt.dll");
        
        if ( hModule )
        {
            fnFun = (fn_vscprintf)GetProcAddress(hModule, "_vscwprintf");
            if ( fnFun ) return fnFun(_Format, _ArgList);
        }
        // 走到这里就是失败了, 返回一个默认值吧, 不知道vc6有什么代替
        return 260;
#endif
    }
    inline int tstr_vstscanf(const char* buffer, const char* _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return vsscanf_s(buffer, _Format, _ArgList);
#else
        typedef int(__cdecl* fn_vsscanf)( const char* buffer, const char* _Format, va_list _ArgList );
        static fn_vsscanf fnFun;
        if ( !fnFun )
            return fnFun(buffer, _Format, _ArgList);
        HMODULE hModule = GetModuleHandleW(L"msvcrt.dll");
        if ( !hModule )
            hModule = LoadLibraryW(L"msvcrt.dll");
        
        if ( hModule )
        {
            fnFun = (fn_vsscanf)GetProcAddress(hModule, "vsscanf_s");
            if ( fnFun )
                return fnFun(buffer, _Format, _ArgList);
            fnFun = (fn_vsscanf)GetProcAddress(hModule, "vsscanf");
            if ( fnFun )
                return fnFun(buffer, _Format, _ArgList);
        }
        // 走到这里就是失败了, 返回一个默认值吧, 不知道vc6有什么代替
        return 0;
#endif
    }
    inline int tstr_vstscanf(const wchar_t* buffer, const wchar_t* _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return vswscanf_s(buffer, _Format, _ArgList);
#else
        typedef int(__cdecl* fn_vsscanf)( const wchar_t* buffer, const wchar_t* _Format, va_list _ArgList );
        static fn_vsscanf fnFun;
        if ( !fnFun )
            return fnFun(buffer, _Format, _ArgList);
        HMODULE hModule = GetModuleHandleW(L"msvcrt.dll");
        if ( !hModule )
            hModule = LoadLibraryW(L"msvcrt.dll");
        
        if ( hModule )
        {
            fnFun = (fn_vsscanf)GetProcAddress(hModule, "vswscanf_s");
            if ( fnFun )
                return fnFun(buffer, _Format, _ArgList);
            fnFun = (fn_vsscanf)GetProcAddress(hModule, "vswscanf");
            if ( fnFun )
                return fnFun(buffer, _Format, _ArgList);
        }
        // 走到这里就是失败了, 返回一个默认值吧, 不知道vc6有什么代替
        return 0;
#endif
    }

};


// 格式化文本
inline static int tstr_vsnprintf(char* const _Buffer, const size_t _bufferSize, const char* const _Format, va_list _ArgList)
{
    return tstr_hide::tstr_vsnprintf_s(_Buffer, _bufferSize, _Format, _ArgList);
}
inline static int tstr_vsnprintf(wchar_t* const _Buffer, const size_t _bufferSize, const wchar_t* const _Format, va_list _ArgList)
{
    return tstr_hide::tstr_vsnwprintf_s(_Buffer, _bufferSize, _Format, _ArgList);
}

#if _MSVC_LANG >= 202002L
inline static int tstr_vsnprintf(char8_t* const _Buffer, const size_t _bufferSize, const char8_t* const _Format, va_list _ArgList)
{
    return tstr_hide::tstr_vsnprintf_s((char*)_Buffer, _bufferSize, (LPCSTR)_Format, _ArgList);
}
#endif

// 取格式化文本长度
inline static int tstr_vscprintf(const char* const _Format, va_list _ArgList)
{
    return tstr_hide::tstr_vsctprintf(_Format, _ArgList);
}
inline static int tstr_vscprintf(const wchar_t* const _Format, va_list _ArgList)
{
    return tstr_hide::tstr_vsctprintf(_Format, _ArgList);
}
#include <cpplang.h>
#if _MSVC_LANG >= 202002L
// c++20需要加入这个类型的支持
inline static int tstr_vscprintf(const char8_t* const _Format, va_list _ArgList)
{
    return tstr_hide::tstr_vsctprintf((LPCSTR)_Format, _ArgList);
}
#endif

template<typename T>
inline static int tstr_sprintf(T* const _Buffer, const size_t _bufferSize, const T* const _Format, ...)
{
    va_list aptr;
    va_start(aptr, _Format);
    int r = tstr_vsnprintf(_Buffer, _bufferSize, _Format, aptr);
    va_end(aptr);
    return r;
}


// 获取格式化文本, 返回文本地址, 返回的文本需要调用 tstr_free() 释放
// pSize = 接收文本长度, _vsnprintf_s 的返回值
// fmt = 格式化文本
// list = 参数列表
template<typename T> inline T* tstr_GetFormat(int* pSize, const T* fmt, va_list list)
{
    int r = tstr_vscprintf(fmt, list) + 1;
    int bufferSize = r > 1 ? r : 260;
    T* buffer = tstr_alloc<T>(bufferSize);
    
    r = tstr_vsnprintf(buffer, bufferSize, fmt, list);
    while ( r == -1 )
    {
        bufferSize *= 2;
        buffer = tstr_realloc(buffer, bufferSize);
        r = tstr_vsnprintf(buffer, bufferSize, fmt, list);
    }
    if ( pSize )*pSize = r;
    return buffer;
}

// 获取格式化文本, 返回文本地址, 返回的文本需要调用 tstr_free() 释放
// pSize = 接收文本长度, _vsnprintf_s 的返回值
// fmt = 格式化文本
// list = 参数列表
template<typename T> inline T* tstr_GetFormat(int* pSize, const T* fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    T* ret = tstr_GetFormat(pSize, fmt, list);
    va_end(list);
    return ret;
}


inline int tstr_MessageBoxTimeout(HWND hWnd, const char* lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds, const char* fmt, va_list list)
{
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if ( !hUser32 )return 0;

    typedef int( WINAPI* PFN_MSGBOX )( HWND hWnd, const char* lpText, const char* lpCaption,
                                      UINT uType, WORD wLanguageId, DWORD dwMilliseconds );
    PFN_MSGBOX pfn = (PFN_MSGBOX)GetProcAddress(hUser32, "MessageBoxTimeoutA");
    if ( !pfn ) return 0;

    char* text = tstr_GetFormat(0, fmt, list);
    int ret = pfn(hWnd, text, lpCaption, uType, wLanguageId, dwMilliseconds);
    tstr_free(text);
    return ret;
}

inline int tstr_MessageBoxTimeout(HWND hWnd, const wchar_t* lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds, const wchar_t* fmt, va_list list)
{
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if ( !hUser32 )return 0;

    typedef int( WINAPI* PFN_MSGBOX )( HWND hWnd, const wchar_t* lpText, const wchar_t* lpCaption,
                                      UINT uType, WORD wLanguageId, DWORD dwMilliseconds );
    PFN_MSGBOX pfn = (PFN_MSGBOX)GetProcAddress(hUser32, "MessageBoxTimeoutW");
    if ( !pfn ) return 0;

    wchar_t* text = tstr_GetFormat(0, fmt, list);
    int ret = pfn(hWnd, text, lpCaption, uType, wLanguageId, dwMilliseconds);
    tstr_free(text);
    return ret;
}

template<typename T>
inline int tstr_MessageBoxTimeout(HWND hWnd, const T* lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds, const T* fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    int ret = tstr_MessageBoxTimeout(hWnd, lpCaption, uType, wLanguageId, dwMilliseconds, fmt, list);
    va_end(list);
    return ret;
}

template<typename T>
inline int tstr_MessageBoxEx(HWND hWnd, const T* lpCaption, UINT uType, WORD wLanguageId, const T* fmt, va_list list)
{
    return tstr_MessageBoxTimeout(hWnd, lpCaption, uType, wLanguageId, 0, fmt, list);
}

template<typename T>
inline int tstr_MessageBoxEx(HWND hWnd, const T* lpCaption, UINT uType, WORD wLanguageId, const T* fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    int ret = tstr_MessageBoxTimeout(hWnd, lpCaption, uType, wLanguageId, 0, fmt, list);
    va_end(list);
    return ret;
}

template<typename T>
inline int tstr_MessageBox(HWND hWnd, const T* lpCaption, UINT uType, const T* fmt, va_list list)
{
    return tstr_MessageBoxTimeout(hWnd, lpCaption, uType, 0, 0, fmt, list);
}

template<typename T>
inline int tstr_MessageBox(HWND hWnd, const T* lpCaption, UINT uType, const T* fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    int ret = tstr_MessageBoxTimeout(hWnd, lpCaption, uType, 0, 0, fmt, list);
    va_end(list);
    return ret;
}

inline void tstr_OutputDebugString(const char* fmt, va_list list)
{
    char* text = tstr_GetFormat(0, fmt, list);
    OutputDebugStringA((char*)text);
    tstr_free(text);
}
inline void tstr_OutputDebugString(const wchar_t* fmt, va_list list)
{
    wchar_t* text = tstr_GetFormat(0, fmt, list);
    OutputDebugStringW(text);
    tstr_free(text);
}
template<typename T> inline void tstr_OutputDebugString(const T* fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    tstr_OutputDebugString(fmt, list);
    va_end(list);
}

// 调试输出, 调试版编译版都会执行
template<typename T> inline void tstr_debug(const T* fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    tstr_OutputDebugString(fmt, list);
    va_end(list);
}

// 有定义 _DEBUG 才会执行这个函数的内容, 否则这个函数说明也不做, 总的来说就是调试版才执行
template<typename T> inline void tstr_dbg(const T* fmt, ...)
{
#ifdef _DEBUG
    va_list list;
    va_start(list, fmt);
    tstr_OutputDebugString(fmt, list);
    va_end(list);
#endif
}






inline static int tstr_cmp(const char* s1, const char* s2)
{
    return strcmp(s1, s2);
}

inline static int tstr_cmp(const wchar_t* s1, const wchar_t* s2)
{
    return wcscmp(s1, s2);
}

inline static int tstr_ncmp(const char* s1, const char* s2, size_t count)
{
    return strncmp(s1, s2, count);
}
inline static int tstr_ncmp(const wchar_t* s1, const wchar_t* s2, size_t count)
{
    return wcsncmp(s1, s2, count);
}


inline static int tstr_icmp(const char* s1, const char* s2)
{
#if _MSC_VER > 1200
    return _stricmp(s1, s2);
#else
    return stricmp(s1, s2);
#endif
}
inline static int tstr_icmp(const wchar_t* s1, const wchar_t* s2)
{
#if _MSC_VER > 1200
    return _wcsicmp(s1, s2);
#else
    return wcsicmp(s1, s2);
#endif
}


inline static int tstr_nicmp(const char* s1, const char* s2, size_t count)
{
#if _MSC_VER > 1200
    return _strnicmp(s1, s2, count);
#else
    return strnicmp(s1, s2, count);
#endif
}
inline static int tstr_nicmp(const wchar_t* s1, const wchar_t* s2, size_t count)
{
#if _MSC_VER > 1200
    return _wcsnicmp(s1, s2, count);
#else
    return wcsnicmp(s1, s2, count);
#endif
}


inline static const char* tstr_str(const char* s1, const char* s2)
{
    return strstr(s1, s2);
}
inline static char* tstr_str(char* s1, char* s2)
{
    return strstr(s1, s2);
}

inline static const wchar_t* tstr_str(const wchar_t* s1, const wchar_t* s2)
{
    return wcsstr(s1, s2);
}
inline static wchar_t* tstr_str(wchar_t* s1, wchar_t* s2)
{
    return wcsstr(s1, s2);
}


inline static int tstr_vsscanf(const char* buffer, const char* fmt, va_list list)
{
    return tstr_hide::tstr_vstscanf(buffer, fmt, list);
}

inline static int tstr_vsscanf(const wchar_t* buffer, const wchar_t* fmt, va_list list)
{
    return tstr_hide::tstr_vstscanf(buffer, fmt, list);
}



template<typename T> inline static int tstr_sscanf(const T* buffer, const T* fmt, ...)
{
    va_list list;
    va_start(list, fmt);
    int ret = tstr_hide::tstr_vstscanf(buffer, fmt, list);
    va_end(list);
    return ret;
}


template<typename T> inline static size_t tstr_len(const T* s)
{
    if ( !s )return 0;
    size_t _Count = 0;
    while ( *s != T() )
    {
        ++_Count;
        ++s;
    }
    return _Count;
}



inline static char* tstr_tok(char* s, const char* substr, char** next_token)
{
#if _MSC_VER > 1200
    return strtok_s(s, substr, next_token);
#else
    return strtok(s, substr);
#endif
}
inline static wchar_t* tstr_tok(wchar_t* s, const wchar_t* substr, wchar_t** next_token)
{
#if _MSC_VER > 1200
    return wcstok_s(s, substr, next_token);
#else
    return wcstok(s, substr);
#endif
}


inline static double tstr_stold(const char* val)
{
    char* _Eptr;
    return ( !val || !val[0] ) ? 0.0 : strtod(val, &_Eptr);
}

inline static double tstr_stold(const wchar_t* val)
{
    wchar_t* _Eptr;
    return ( !val || !val[0] ) ? 0.0 : wcstod(val, &_Eptr);
}


inline static double tstr_stod(const char* val)
{
    char* _Eptr;
    return ( !val || !val[0] ) ? 0.0 : strtod(val, &_Eptr);
}
inline static double tstr_stod(const wchar_t* val)
{
    wchar_t* _Eptr;
    return ( !val || !val[0] ) ? 0.0 : wcstod(val, &_Eptr);
}

template<typename T> inline static float tstr_stof(const T* val)
{
    return (float)tstr_stod(val);
}
