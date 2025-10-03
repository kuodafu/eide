#pragma once
#include <tstr.h>

class CPrivateProfile
{
private:    
    inline void free() { /*m_file.Release();*/ }    // 释放记录的文件路径
    inline DWORD GetStrCount(const wchar_t* str)
    {
        DWORD len = 0;
        size_t strLen = wcslen(str);
        while (strLen)
        {
            len++;
            str += strLen + 1;
            strLen = wcslen(str);
        };
        return len;
    }
    inline DWORD GetStrCountA(const char* str)
    {
        DWORD len = 0;
        size_t strLen = strlen(str);
        while (strLen)
        {
            len++;
            str += strLen + 1;
            strLen = strlen(str);
        };
        return len;
    }

public:
    wstr m_file;    // 配置文件路径
    bool m_isWrite; // 配置项不存在时是否把默认值写入配置项
    CPrivateProfile() : m_isWrite(false) { }
    CPrivateProfile(const wchar_t* file, bool isWrite = false) : m_isWrite(isWrite) { open(file); }
    ~CPrivateProfile() { free(); }

public:
    inline const wchar_t* GetPathW() const { return m_file.c_str(); }
    inline const _str GetPathA() const { return m_file.w2a(); }

    // 打开配置文件
    inline bool open(const wchar_t* file)
    {
        if (!file)return false;
        m_file.assign(file);
        return file != 0;
    }

    // 写配置项
    // AppName = 配置节名
    // KeyName = 配置项名 为空则删除这个小节的所有设置项
    // String  = 写入内容 为空则删除这个项现有的字串
    inline bool write(const wchar_t* AppName, const wchar_t* KeyName = 0, const wchar_t* String = 0) const
    {
        if (m_file.empty())return 0;
        return WritePrivateProfileStringW(AppName, KeyName, String, m_file.c_str()) != FALSE;
    }
    // 写配置项A版
    // AppName = 配置节名
    // KeyName = 配置项名 为空则删除这个小节的所有设置项
    // String  = 写入内容 为空则删除这个项现有的字串
    inline bool write(const char* AppName, const char* KeyName = 0, const char* String = 0) const
    {
        if (m_file.empty())return 0;
        return WritePrivateProfileStringA(AppName, KeyName, String, m_file.w2a().c_str()) != FALSE;
    }

    // 写配置项
    // AppName = 配置节名
    // KeyName = 配置项名
    // String  = 写入内容
    inline bool write(const wchar_t* AppName, const wchar_t* KeyName, LONGLONG String) const
    {
        if (m_file.empty())return 0;
        wchar_t buffer[50] = { 0 };
        swprintf_api(buffer, 50, L"%lld", String);
        return WritePrivateProfileStringW(AppName, KeyName, buffer, m_file.c_str()) != FALSE;
    }

    // 读配置项
    // AppName = 配置节名 为空则返回所有节名, 多个节名之间有\0分割
    // KeyName = 配置项名 为空则返回所有项名, 多个项名之间有\0分割
    // Default = 默认内容
    // count   = AppName/KeyName 为空时返回对应的数量
    inline wstr read(const wchar_t* AppName = 0, const wchar_t* KeyName = 0, const wchar_t* Default = 0, DWORD* count = 0)
    {
        if (m_file.empty())return 0;

        DWORD bufferSize = (DWORD)260;
        wstr ret(bufferSize);

        DWORD len = GetPrivateProfileStringW(AppName, KeyName, Default, ret.data(), bufferSize, m_file.c_str());
        if (m_isWrite && GetLastError() == ERROR_FILE_NOT_FOUND) // 没有创建这个配置项, 需要写出
        {
            WritePrivateProfileStringW(AppName, KeyName, Default, m_file.c_str());
        }
        else
        {
            while (len == bufferSize - 1 || len == bufferSize - 2)
            {
                bufferSize *= 2;
                ret.resize(bufferSize);
                len = GetPrivateProfileStringW(AppName, KeyName, Default, ret.data(), bufferSize, m_file.c_str());
            }
        }
        if ((!AppName || !KeyName) && count)    // 项目或者节名为空才取长度
            *count = GetStrCount(ret.c_str());
        ret.resize(len);
        return ret;
    }

    // 读配置项
    // AppName = 配置节名 为空则返回所有节名, 多个节名之间有\0分割
    // KeyName = 配置项名 为空则返回所有项名, 多个项名之间有\0分割
    // Default = 默认内容
    // count   = AppName/KeyName 为空时返回对应的数量
    inline _str readA(const char* AppName = 0, const char* KeyName = 0, const char* Default = 0, DWORD* count = 0)
    {
        if (m_file.empty())return 0;

        DWORD bufferSize = (DWORD)260;
        _str ret(bufferSize);
        _str fileA = m_file.w2a();

        DWORD len = GetPrivateProfileStringA(AppName, KeyName, Default, ret.data(), bufferSize, fileA.c_str());
        if (m_isWrite && GetLastError() == ERROR_FILE_NOT_FOUND) // 没有创建这个配置项, 需要写出
        {
            WritePrivateProfileStringA(AppName, KeyName, Default, fileA.c_str());
        }
        else
        {
            while (len == bufferSize - 1 || len == bufferSize - 2)
            {
                bufferSize *= 2;
                ret.resize(bufferSize);
                len = GetPrivateProfileStringA(AppName, KeyName, Default, ret.data(), bufferSize, fileA.c_str());
            }
        }
        if ((!AppName || !KeyName) && count)    // 项目或者节名为空才取长度
            *count = GetStrCountA(ret.c_str());

        ret.resize(strlen(ret.c_str()));
        return ret;
    }


    // 读配置项整数
    // AppName = 配置节名 为空则返回节名数量
    // KeyName = 配置项名 为空则返回项名数量
    // Default = 默认内容
    inline int read_int(const wchar_t* AppName = 0, const wchar_t* KeyName = 0, int Default = 0)
    {
        wchar_t buffer[20] = { 0 };
        _itow_api(Default, buffer, 20, 10);

        DWORD count = 0;
        wstr str = read(AppName, KeyName, buffer, &count);
        if (!AppName || !KeyName) return count;    // 项目或者节名为空才取长度
        return str.stoi();  // 否则把文本转成数值
    }
    
    // 取所有节名, 取所有节名,成功返回所有节名
    inline wstr getAllAppname(DWORD* count)
    {
        return read(0, 0, 0, count);
    }

    // 取所有项名, 取某个节名下的所有项名,成功返回所有项名
    inline wstr getAllKeyname(const wchar_t* AppName, DWORD* count)
    {
        return read(AppName, 0, 0, count);
    }

    // 获取指定节点下有配置项数量, 成功返回配置项数量, 失败返回0
    // appName = 节点名, 空文本则返回0
    inline int GetChildCount(LPCWSTR appName)
    {
        if (!appName || !appName[0]) return 0;
        DWORD count = 0;
        wstr value = read(appName, 0, 0, &count);
        return (int)count;
    }

    // 获取指定节点下指定索引的配置项
    // appName = 节点名, 空文本则返回0
    // index = 第几个配置项, 从0开始
    // defValue = 获取失败时的默认值
    inline wstr GetChildValue(LPCWSTR appName, int index, LPCWSTR defValue = 0)
    {
        if (!appName || !appName[0]) return 0;
        DWORD count = 0;
        wstr value = read(appName, 0, 0, &count);
        LPCWSTR str = value.c_str();
        size_t strLen = 0;
        for (DWORD i = 0; i < count; i++)
        {
            if (index == i)
            {
                value = read(appName, str, defValue);
                return value;
            }
            str += strLen + 1;
            strLen = wcslen(str);
        }
        return defValue;
    }
    // 获取指定节点下指定索引的配置项
    // appName = 节点名, 空文本则返回0
    // index = 第几个配置项, 从0开始
    // defValue = 获取失败时的默认值
    inline int GetChildValue_int(LPCWSTR appName, int index, int defValue = 0)
    {
        if (!appName || !appName[0]) return 0;
        wstr def = wstr::to_string(defValue);
        def = GetChildValue(appName, index, def.c_str());
        return def.stol();
    }

    // 枚举子节点, 返回写入缓冲区的成员数
    // pBuf = 接收枚举到的配置项数组指针, 为0则返回成员数
    // nBufSize = 缓冲区数组成员数, 为0则返回成员数
    // pBufName = 接收枚举到的配置项名数组指针
    // nBufNameSize = 缓冲区数组成员数, 为0则返回成员数
    inline int EnumChild(LPCWSTR appName, wstr* pBuf, int nBufSize, wstr* pBufName = 0, int nBufNameSize = 0)
    {
        if (!appName || !appName[0]) return 0;

        DWORD count = 0;
        wstr value = read(appName, 0, 0, &count);
        if (!pBuf || !nBufSize) return (int)count;

        LPCWSTR str = value.c_str();
        size_t strLen = 0;
        DWORD i;
        for (i = 0; i < count; i++)
        {
            if (i < (DWORD)nBufSize)
                pBuf[i] = read(appName, str);
            if (i < (DWORD)nBufNameSize)
                pBufName[i] = str;

            str += strLen + 1;
            strLen = wcslen(str);
        }
        return (int)i;
    }

    // 枚举子节点, 返回写入缓冲区的成员数
    // pBuf = 接收枚举到的配置项数组指针, 为0则返回成员数
    // nBufSize = 缓冲区数组成员数, 为0则返回成员数
    // pBufName = 接收枚举到的配置项名数组指针
    // nBufNameSize = 缓冲区数组成员数, 为0则返回成员数
    inline int EnumChild_int(LPCWSTR appName, int* pBuf, int nBufSize, wstr* pBufName = 0, int nBufNameSize = 0)
    {
        if (!appName || !appName[0]) return 0;

        DWORD count = 0;
        wstr value = read(appName, 0, 0, &count);
        if (!pBuf || !nBufSize) return (int)count;

        LPCWSTR str = value.c_str();
        size_t strLen = 0;
        DWORD i;
        for (i = 0; i < count; i++)
        {
            if (i < (DWORD)nBufSize)
            {
                wstr vl = read(appName, str);
                pBuf[i] = vl.stol();
            }
            if (i < (DWORD)nBufNameSize)
                pBufName[i] = str;

            str += strLen + 1;
            strLen = wcslen(str);
        }
        return (int)i;
    }
};

