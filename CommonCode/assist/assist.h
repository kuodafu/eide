#pragma once
#include <windows.h>
#include <wincrypt.h>
#include <tchar.h>
#include "tstr.h"
#if _MSC_VER < 1201
#include <assist/crypt_API.h>
#endif

// 需要置随机数种子 srand();
inline int rand(int min, int max) { return (rand() % (max - min + 1)) + min; }

inline BOOL GetRegValue(HKEY hKey, LPCWSTR path, LPCWSTR name, void* buffer, DWORD& len)
{
    HKEY _hKey;
    BOOL bRet = FALSE;
    int status = RegOpenKeyExW(hKey, path, 0, KEY_READ, &_hKey);
    if (ERROR_SUCCESS == status)
    {
        status = RegQueryValueExW(_hKey, name, 0, NULL, (LPBYTE)buffer, &len);
        if (ERROR_SUCCESS == status)
        {
            bRet = TRUE;
        }

        RegCloseKey(_hKey);
    }
    return bRet;
}

inline BOOL SetRegValue(HKEY hKey, LPCWSTR path, LPCWSTR name, const void* data, int dataLen)
{
    HKEY _hKey;
    BOOL bRet = FALSE;
    int status = RegOpenKeyExW(hKey, path, 0, KEY_SET_VALUE, &_hKey);
    if (ERROR_SUCCESS == status)
    {
        status = RegSetValueExW(_hKey, name, NULL, REG_BINARY, (LPBYTE)data, dataLen);
        if (ERROR_SUCCESS == status)
        {
            bRet = TRUE;
        }

        RegCloseKey(_hKey);
    }
    return bRet;
}

inline BOOL DeleteRegKey(HKEY hKey, LPCWSTR lpSubKey)
{
#if _MSC_VER < 1201
    return ERROR_SUCCESS == RegDeleteKeyW(hKey, lpSubKey);
#else
    return ERROR_SUCCESS == RegDeleteKeyExW(hKey, lpSubKey, KEY_WOW64_32KEY, 0);
#endif
}

template<typename _Ty>
inline int readFile(FILE* f, _Ty& data)
{
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    data.resize(size);
    size = (long)fread_s(&data[0], size, 1, size, f);
    return size;
}
// 读入文件, 不支持超过4G的文件
template<typename _Ty>
inline int readFile(LPCWSTR file, _Ty& data)
{
#if _MSC_VER > 1200
    FILE* f;
    errno_t err = _wfopen_s(&f, file, L"rb");
#else
    FILE* f = _wfopen(file, L"rb");
#endif

    if (!f)
        return 0;
    int size = readFile(f, data);
    fclose(f);
    return size;

}

// 读入文件, 不支持超过4G的文件
template<typename _Ty>
inline int readFile(LPCSTR file, _Ty& data)
{
#if _MSC_VER > 1200
    FILE* f;
    errno_t err = fopen_s(&f, file, "rb");
#else
    FILE* f = fopen(file, "rb");
#endif

    if (!f)
        return 0;
    int size = readFile(f, data);
    fclose(f);
    return size;
}

// 读入文件, 不支持超过4G的文件
inline int writeFile(LPCWSTR file, LPCVOID pData, int size)
{
#if _MSC_VER > 1200
    FILE* f;
    errno_t err = _wfopen_s(&f, file, L"wb+");
    if (!f)
        return 0;

#else
    FILE* f = _wfopen(file, L"rb+");
    if (!f)
        return 0;

#endif
    fseek(f, 0, SEEK_SET);
    int ret = (int)fwrite(pData, 1, size, f);
    fclose(f);
    return ret;
}

// 获取数据MD5值, 返回写入缓冲区的尺寸
inline int GetMd5(LPCVOID pData, size_t size, LPVOID pBuffer, size_t nBuffer)
{
    if (nBuffer < 16 || nBuffer > 0x7fffffff)
        return 0;

    memset(pBuffer, 0, nBuffer);
    if (!pData || !size)
        return 0;

    HCRYPTPROV hProv = NULL;
    HCRYPTPROV hHash = NULL;
    int retVl = 0;

    do
    {
        if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
            break;       //获得CSP中一个密钥容器的句柄

        //初始化对数据流的hash, 创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被    CryptHashData调用。

        //if ( !CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) )
        if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
            break;

        if (!CryptHashData(hHash, (LPBYTE)pData, (DWORD)size, 0))
            break;      //hash文件  


        unsigned char* pbHash;
        DWORD dwHashLen = sizeof(DWORD);
        if (!CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0))
            break; //我也不知道为什么要先这样调用CryptGetHashParam, 这块是参照的msdn       
        if (dwHashLen > nBuffer)
            break;
        //throw std::exception("获取到的尺寸大于16个字节");

        pbHash = (unsigned char*)malloc(dwHashLen);
        if (!pbHash)
            break;
        if (CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0))//获得md5值 
        {
            retVl = dwHashLen;
            memcpy(pBuffer, pbHash, dwHashLen);
        }

        free(pbHash);
    } while (false);

    //善后工作
    if (hHash)
        CryptDestroyHash(hHash);
    if (hProv)
        CryptReleaseContext(hProv, 0);

    return retVl;
}


template<typename T>
inline BOOL GetMd5(LPCVOID pData, int size, T& hash)
{
    return GetMd5(pData, size, (LPVOID)&hash, sizeof(T));
}


template<typename T, typename R>
inline BOOL GetMd5(const T& data, R& hash)
{
    return GetMd5(&data[0], data.size(), (LPVOID)&hash, sizeof(R));
}

template<typename T>
inline BOOL GetMd5(LPCWSTR pszFile, T& hash)
{
    _str file_data;
    readFile(pszFile, file_data);
    return GetMd5(file_data.c_str(), file_data.size(), (LPVOID)&hash, sizeof(T));
}

#if _MSC_VER > 1200
template<size_t _Size>
inline BOOL GetMd5(LPCVOID pData, int size, char(&hash)[_Size])
{
    return GetMd5(pData, size, hash, _Size);
}
template<size_t _Size>
inline BOOL GetMd5(LPCWSTR pszFile, char(&hash)[_Size])
{
    _str file_data;
    readFile(pszFile, file_data);
    return GetMd5(file_data.c_str(), file_data.size(), (LPVOID)hash, _Size);
}
template<size_t _Size>
inline BOOL GetMd5(LPCSTR pszFile, char(&hash)[_Size])
{
    _str file_data;
    readFile(pszFile, file_data);
    return GetMd5(file_data.c_str(), file_data.size(), (LPVOID)hash, _Size);
}
#endif

template<typename T>
inline BOOL GetMd5(LPCSTR pszFile, T& hash)
{
    _str file_data;
    readFile(pszFile, file_data);
    return GetMd5(file_data.c_str(), file_data.size(), (LPVOID)&hash, sizeof(T));
}




inline BOOL GetHash(LPCVOID pData, size_t size, LPVOID pBuffer, size_t nBuffer)
{
    if (nBuffer < 20 || nBuffer > 0x7fffffff)
        return FALSE;

    memset(pBuffer, 0, nBuffer);
    if (!pData || !size)
        return FALSE;
    HCRYPTPROV hProv = NULL;
    HCRYPTPROV hHash = NULL;
    BOOL retVl = false;

    do
    {
        if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
            break;       //获得CSP中一个密钥容器的句柄

        //初始化对数据流的hash, 创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被    CryptHashData调用。

        //if ( !CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) )
        if (!CryptCreateHash(hProv, CALG_SHA, 0, 0, &hHash))
            break;

        if (!CryptHashData(hHash, (LPBYTE)pData, (DWORD)size, 0))
            break;      //hash文件  


        unsigned char* pbHash;
        DWORD dwHashLen = sizeof(DWORD);
        if (!CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0))
            break; //我也不知道为什么要先这样调用CryptGetHashParam, 这块是参照的msdn       
        if (dwHashLen > nBuffer)
            break;
        //throw std::exception("获取到的尺寸大于16个字节");

        pbHash = (unsigned char*)malloc(dwHashLen);
        if (!pbHash)
            break;
        if (CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0))//获得md5值 
            memcpy(pBuffer, pbHash, dwHashLen);

        free(pbHash);
        retVl = true;
    } while (false);

    //善后工作
    if (hHash)
        CryptDestroyHash(hHash);
    if (hProv)
        CryptReleaseContext(hProv, 0);

    return retVl;
}


template<typename T>
inline BOOL GetHash(LPCVOID pData, int size, T& hash)
{
    return GetHash(pData, size, (LPVOID)&hash, sizeof(T));
}
#if _MSC_VER > 1200
template<size_t _Size>
inline BOOL GetHash(LPCVOID pData, int size, char(&hash)[_Size])
{
    return GetHash(pData, size, hash, _Size);
}
template<size_t _Size>
inline BOOL GetHash(LPCWSTR pszFile, char(&hash)[_Size])
{
    _str file_data;
    readFile(pszFile, file_data);
    return GetHash(file_data.c_str(), file_data.size(), (LPVOID)hash, _Size);
}
template<size_t _Size>
inline BOOL GetHash(LPCSTR pszFile, char(&hash)[_Size])
{
    _str file_data;
    readFile(pszFile, file_data);
    return GetHash(file_data.c_str(), file_data.size(), (LPVOID)hash, _Size);
}
#endif
template<typename T, typename R>
inline BOOL GetHash(const T& data, R& hash)
{
    return GetHash(&data[0], data.size(), (LPVOID)&hash, sizeof(R));
}

template<typename T>
inline BOOL GetHash(LPCWSTR pszFile, T& hash)
{
    _str file_data;
    readFile(pszFile, file_data);
    return GetHash(file_data.c_str(), file_data.size(), (LPVOID)&hash, sizeof(T));
}

template<typename T>
inline BOOL GetHash(LPCSTR pszFile, T& hash)
{
    _str file_data;
    readFile(pszFile, file_data);
    return GetHash(file_data.c_str(), file_data.size(), (LPVOID)&hash, sizeof(T));
}


inline static BOOL fnPathFileExistsW(LPCWSTR file)
{
    typedef BOOL(WINAPI* PFN_PathFileExistsW)( LPCWSTR pszPath );
    static PFN_PathFileExistsW pFun;
    if ( !pFun )
    {
        HMODULE hModule = LoadLibraryW(L"Shlwapi.dll");
        if ( !hModule )
            return false;
        pFun = (PFN_PathFileExistsW)GetProcAddress(hModule, "PathFileExistsW");
        if ( !pFun )
            return false;
    }
    return pFun(file);
}

inline void WaitForSingleObjectAndMsg_d(HANDLE hHandle)    // 等待信号
{
    while (TRUE)
    {
        DWORD result;
        MSG msg = { 0 };

        result = MsgWaitForMultipleObjects(1, &hHandle, FALSE, INFINITE, QS_ALLINPUT);

        if (result == (WAIT_OBJECT_0))break;    // HANDLE数组的索引,表示这个收到信号
        if (result == WAIT_TIMEOUT)break;        // 超时
        if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            if (msg.message == WM_DESTROY || msg.message == WM_QUIT)
                return;
        }
    }
}

// 程序_延时
inline void WINAPI sleep(DWORD time)
{
    if (time <= 0)time = 1000;
#if _MSC_VER < 1201
    HANDLE hTime = CreateWaitableTimerW_d(NULL, FALSE, NULL);
    if (!hTime)return;
    LARGE_INTEGER d = { 0 };
    d.QuadPart = (LONGLONG)((LONGLONG)(-10) * (LONGLONG)time * (LONGLONG)1000);
    SetWaitableTimer_d(hTime, (LARGE_INTEGER_d*)&d, NULL, NULL, NULL, FALSE);
    WaitForSingleObjectAndMsg_d(hTime);
#else
    HANDLE hTime = CreateWaitableTimerW(NULL, FALSE, NULL);
    if (!hTime)return;
    LARGE_INTEGER d = { 0 };
    d.QuadPart = (LONGLONG)((LONGLONG)(-10) * (LONGLONG)time * (LONGLONG)1000);
    SetWaitableTimer(hTime, (LARGE_INTEGER*)&d, NULL, NULL, NULL, FALSE);
    WaitForSingleObjectAndMsg_d(hTime);
#endif

    CloseHandle(hTime);
}





inline BOOL SetClipboard(LPCSTR str, int size = -1)
{
    if (!str) return FALSE;
    if (size == -1)
        size = (int)(LONG_PTR)strlen(str);
    size_t len = size + 1;
    HGLOBAL hMem = GlobalAlloc(GHND, len);
    if (!hMem)
        return FALSE;
    void* p = GlobalLock(hMem);
    if (p)
    {
        memcpy(p, str, len);
        GlobalUnlock(hMem);
        OpenClipboard(0);
        EmptyClipboard();
        SetClipboardData(CF_TEXT, hMem);
        //GlobalFree(hMem);
        CloseClipboard();
    }
    return TRUE;
}
inline BOOL SetClipboard(LPCWSTR str, int size = -1)
{
    if (!str) return FALSE;
    if (size == -1)
        size = (int)(LONG_PTR)wcslen(str);
    size_t len = size + 1;
    HGLOBAL hMem = GlobalAlloc(GHND, len * 2);
    if (!hMem)
        return FALSE;
    void* p = GlobalLock(hMem);
    if (p)
    {
        memcpy(p, str, len * 2);
        GlobalUnlock(hMem);
        OpenClipboard(0);
        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, hMem);
        //GlobalFree(hMem);
        CloseClipboard();
    }
    return TRUE;
}
inline wstr GetClipboard()
{
    OpenClipboard(0);

    if (IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
        void* lpData = GlobalLock(hMem);
        SIZE_T nSize = GlobalSize(hMem);
        if (lpData)
        {
            wstr ret(nSize);
            memcpy(ret.data(), lpData, nSize);
            ret.resize_strlen();
            GlobalUnlock(hMem);
            CloseClipboard();
            return ret;
        }
    }
    CloseClipboard();
    return wstr();
}
inline _str GetClipboardA()
{
    wstr w = GetClipboard();
    return w.w2a();
}
inline BOOL zjjbwb(LPCSTR str, int size = -1) { return SetClipboard(str, size); }
inline BOOL zjjbwb(LPCWSTR str, int size = -1) { return SetClipboard(str, size); }

