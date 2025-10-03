#pragma once
#include <windows.h>
#if _MSC_VER < 1201
typedef LONG LSTATUS;
#define SMTO_ERRORONEXIT 0x0020
#endif


class CRegList
{
public:
    CRegList() : m_hKey(0), m_path(0), m_keyName(0), m_status(0), hKeyRoot(0) { }
    CRegList(HKEY hKey, const wchar_t* path, bool isCreate = false, DWORD samDesired = KEY_READ): m_hKey(0), m_path(0), m_keyName(0), m_status(0), hKeyRoot(0)
    {
        open(hKey, path, isCreate, samDesired);
    }
    CRegList(const CRegList& c) : m_hKey(0), m_path(0), m_keyName(0), m_status(0), hKeyRoot(0)
    {
        open(c.hKeyRoot, c.m_path);
    }
    ~CRegList()
    {
        free();
    }
    LSTATUS GetError() { return m_status; }
    inline bool empty() { return m_hKey == 0; }
protected:
    HKEY m_hKey;
    wchar_t* m_path;
    wchar_t* m_keyName;    // 打开的这个注册表的项名
    LSTATUS m_status;    // 错误码
    //static DWORD flags;    // 标志位,x64则操作64位注册表,权限是所有权限
    HKEY hKeyRoot;        // 根项
public:
    // 打开注册表,打开前会清除上一次打开的注册表
    bool open(HKEY hKey, const wchar_t* path, bool isCreate = false, DWORD samDesired = KEY_ALL_ACCESS)
    {
        free();
        DWORD flags = samDesired;

        bool isWow64 = true;
        flags |= (samDesired | (isWow64 ? KEY_WOW64_64KEY : KEY_WOW64_32KEY));

        HKEY key = 0;
        if (hKey == 0)
            hKey = HKEY_CURRENT_USER;

        if (isCreate)
        {
        create:
            m_status = RegCreateKeyExW(hKey, path, 0, NULL, REG_OPTION_NON_VOLATILE, flags, NULL, &key, NULL);
        }
        else
        {
            m_status = ::RegOpenKeyExW(hKey, path, 0, flags, &key);
            if (m_status != ERROR_SUCCESS) goto create;
        }

        if (ERROR_SUCCESS != m_status)return false;
        hKeyRoot = hKey;
        size_t len = wcslen(path) + 1;
        m_path = new wchar_t[len];
        memcpy(m_path, path, len * sizeof(wchar_t));

        if (m_path[len - 2] == '\\')
            m_path[len - 2] = 0;

        m_keyName = wcsrchr(m_path, '\\') + 1;
        m_hKey = key;
        return true;
    }

    // 释放已经打开的注册表,析构时会自动释放
    void free()
    {
        if (m_hKey)
        {
            m_status = RegCloseKey(m_hKey);
            m_hKey = 0;
        }
        if (m_path)
        {
            delete[] m_path;
            m_path = 0;
            m_keyName = 0;
        }
        m_status = 0;
        hKeyRoot = 0;
    }

    // 读DWORD类型数据
    DWORD read(const wchar_t* name)
    {
        DWORD value = 0;
        DWORD type = 0, size = 0;
        if (!GetType(name, type, &size))
            return 0;

        m_status = ::RegQueryValueExW(m_hKey, name, 0, &type, (LPBYTE)&value, &size);
        return  value;
    }

    // 读字符串类型数据, value 为0时返回缓冲区长度
    DWORD read(const wchar_t* name, wchar_t* value)
    {
        DWORD type = 0, size = 0;
        if (!GetType(name, type, &size))
            return 0;
        if (value == 0)return size;

        m_status = ::RegQueryValueExW(m_hKey, name, 0, &type, (LPBYTE)value, &size);
        return m_status == ERROR_SUCCESS ? size : 0;
    }

    // 读字符串类型数据, 
    DWORD read(const char* name, char* value)
    {
        DWORD type = 0, size = 0;
        if (!GetType(name, type, &size))
            return 0;

        m_status = ::RegQueryValueExA(m_hKey, name, 0, &type, (LPBYTE)value, &size);
        return m_status == ERROR_SUCCESS ? size : 0;
    }


    // 读取注册表,按指定类型指定尺寸读取数据, 需要自己申请缓冲区
    DWORD read(const wchar_t* name, DWORD type, LPVOID value, DWORD size)
    {
        if (!m_hKey)
            return 0;
        m_status = ::RegQueryValueExW(m_hKey, name, 0, &type, (LPBYTE)value, &size);
        return m_status == ERROR_SUCCESS ? size : 0;
    }

    // 写入DWORD类型数据
    bool write(const wchar_t* name, DWORD value)
    {
        if (!m_hKey)return false;
        m_status = RegSetValueExW(m_hKey, name, 0, REG_DWORD, (LPBYTE)&value, sizeof(DWORD));
        return m_status == ERROR_SUCCESS;
    }

    // 写入字符串类型数据
    bool write(const wchar_t* name, const wchar_t* value)
    {
        if (!m_hKey)return false;
        DWORD len = lstrlenW(value) * sizeof(WCHAR) + 2;
        m_status = RegSetValueExW(m_hKey, name, 0, REG_MULTI_SZ, (LPBYTE)value, len);
        return m_status == ERROR_SUCCESS;
    }

    // 写入字符串类型数据
    bool write(const char* name, const char* value)
    {
        if (!m_hKey)return false;
        if (!value) value = "";
        size_t len = strlen(value) + 1;

        m_status = RegSetValueExA(m_hKey, name, 0, REG_SZ, (LPBYTE)value, (DWORD)len);
        return m_status == ERROR_SUCCESS;
    }

    // 写入字节集类型数据
    bool write(const wchar_t* name, LPBYTE value, DWORD size)
    {
        if (!m_hKey)return false;
        m_status = RegSetValueExW(m_hKey, name, 0, REG_BINARY, (LPBYTE)value, size);
        return m_status == ERROR_SUCCESS;
    }

    // 写入指定类型的数据, type = REG_ 开头常量
    bool write(const wchar_t* name, DWORD type, LPVOID value, DWORD size)
    {
        if (!m_hKey)return false;
        m_status = RegSetValueExW(m_hKey, name, 0, type, (LPBYTE)value, size);
        return m_status == ERROR_SUCCESS;
    }

    // 删除项下的值
    bool DeleteValue(const wchar_t* name)
    {
        if (!m_hKey)return false;
        m_status = RegDeleteValueW(m_hKey, name);
        return ERROR_SUCCESS == m_status;
    }
    bool DeleteValue(const char* name)
    {
        if (!m_hKey)return false;
        m_status = RegDeleteValueA(m_hKey, name);
        return ERROR_SUCCESS == m_status;
    }

    // 删除当前打开的注册表项, 删除整个项,项下面所有的键值将会被删除
    bool DeleteKey(const wchar_t* name = 0)
    {
        if (!m_hKey)return false;
        if (name)
        {
#if _MSC_VER > 1200
            m_status = RegDeleteKeyExW(m_hKey, name, KEY_WOW64_64KEY, NULL);
#else
            m_status = RegDeleteKeyW(m_hKey, name);
#endif
            return ERROR_SUCCESS == m_status;
        }

        wchar_t* p = wcsrchr(m_path, '\\');
        if (!p)return false;
        p[0] = 0;
        HKEY hKey = 0;
        LSTATUS status = ::RegOpenKeyExW(hKeyRoot, m_path, 0, KEY_SET_VALUE, &hKey);
        p[0] = '\\';
        if (status != ERROR_SUCCESS || !hKey) return false;
#if _MSC_VER < 1201
        m_status = RegDeleteKeyW(hKey, m_keyName);
#else
        m_status = RegDeleteKeyExW(hKey, m_keyName, KEY_WOW64_64KEY, NULL);
#endif
        RegCloseKey(hKey);
        return ERROR_SUCCESS == m_status;
    }

    bool DeleteKeyA(const char* name = 0)
    {
        if (!m_hKey)return false;
        if (name)
        {
#if _MSC_VER > 1200
            m_status = RegDeleteKeyExA(m_hKey, name, KEY_WOW64_64KEY, NULL);
#else
            m_status = RegDeleteKeyA(m_hKey, name);
#endif
            return ERROR_SUCCESS == m_status;
        }


        char* path = UnicodeToAnsi(m_path);
        if (!path) return false;
        char* p = strrchr(path, '\\');
        if (!p)return false;
        p[0] = 0;
        char* keyName = p + 1;
        HKEY hKey = 0;
        LSTATUS status = ::RegOpenKeyExA(hKeyRoot, path, 0, KEY_SET_VALUE, &hKey);
        p[0] = '\\';
        delete[] path;
        if (status != ERROR_SUCCESS || !hKey) return false;
#if _MSC_VER < 1201
        m_status = RegDeleteKeyA(hKey, keyName);
#else
        m_status = RegDeleteKeyExA(hKey, keyName, KEY_WOW64_64KEY, NULL);
#endif
        RegCloseKey(hKey);
        return ERROR_SUCCESS == m_status;
    }

    // 取注册项类型, REG_ 开头常量
    bool GetType(const wchar_t* keyName, DWORD& type, DWORD* dataSize = 0)
    {
        type = 0;
        if (dataSize)*dataSize = 0;
        if (!m_hKey)return false;
        DWORD dwSize = 0;
        m_status = ::RegQueryValueExW(m_hKey, keyName, 0, &type, 0, &dwSize);
        if (dataSize)*dataSize = dwSize;
        return (m_status == ERROR_SUCCESS);
    }
    bool GetType(const char* keyName, DWORD& type, DWORD* dataSize = 0)
    {
        type = 0;
        if (dataSize)*dataSize = 0;
        if (!m_hKey)return false;
        DWORD dwSize = 0;
        m_status = ::RegQueryValueExA(m_hKey, keyName, 0, &type, 0, &dwSize);
        if (dataSize)*dataSize = dwSize;
        return (m_status == ERROR_SUCCESS);
    }

    // 刷新注册表
    bool upDate()
    {
        //return SendMessageW((HWND)65535, 26, 0, 0);
        DWORD_PTR ret = 0;
        LRESULT r = SendMessageTimeoutW((HWND)65535, 26, 0, 0, SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG | SMTO_ERRORONEXIT, 1000, &ret);
        return r != 0;
    }

private:
    // Unicode转ansi, 返回的结果需要调用 delete[] 释放
    inline static char* UnicodeToAnsi(const wchar_t* const unicode, size_t len = 0, int cp = 936)
    {
        if (!unicode)return 0;
        if (len <= 0)len = wcslen(unicode);
        if (len <= 0)return 0;
        int aLen = WideCharToMultiByte(cp, 0, unicode, (int)len, 0, 0, 0, 0) + 1;
        if (aLen < 1) return 0;

        char* szStr = new char[aLen];
        memset(szStr, 0, aLen);
        WideCharToMultiByte(cp, 0, unicode, (int)len, szStr, aLen, 0, 0);
        return szStr;
    }

    // ansi转Unicode, 返回的结果需要调用 delete[] 释放
    inline static wchar_t* AnsiToUnicode(const char* const ansi, size_t len = 0, int cp = 936)
    {
        if (!ansi)return 0;
        if (len <= 0)len = strlen(ansi);
        if (len <= 0)return 0;

        int uLen = MultiByteToWideChar(cp, 0, ansi, (int)len, NULL, 0) + 1;
        if (uLen < 1) return 0;

        wchar_t* wzStr = new wchar_t[uLen];
        memset(wzStr, 0, uLen * sizeof(wchar_t));
        uLen = MultiByteToWideChar(cp, 0, ansi, (int)len, wzStr, uLen);
        return wzStr;
    }
};


