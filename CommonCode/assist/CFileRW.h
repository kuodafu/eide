#pragma once
#include <windows.h>
#define INVALID_SET_FILE_POINTER ((DWORD)-1)


class CFileRW
{
public:
    CFileRW() :m_hFile(0), m_data(0) { }
    // 打开文件,会先释放之前打开的文件
    // file        = 文件路径
    // openMode    = 打开方式, -1 = 读写, 1 = 读入, 2 = 写出, 4 = 重写, 5 = 改写, 6 = 改读
    // c        = 共享方式,0 = 禁止读写, 1 = 禁止写, 2 = 禁止读,-1 = 无限制
    CFileRW(LPCWSTR file, int openMode = -1, int c = -1) :m_hFile(0), m_data(0)
    {
        open(file, openMode, c);
    }
    // 打开文件,会先释放之前打开的文件
    // file        = 文件路径
    // openMode    = 打开方式, -1 = 读写, 1 = 读入, 2 = 写出, 4 = 重写, 5 = 改写, 6 = 改读
    // c        = 共享方式,0 = 禁止读写, 1 = 禁止写, 2 = 禁止读,-1 = 无限制
    CFileRW(LPCSTR file, int openMode = -1, int c = -1) :m_hFile(0), m_data(0)
    {
        open(file, openMode, c);
    }
    ~CFileRW()
    {
        reset();
    }

private:
    HANDLE m_hFile;
    LPBYTE m_data;
public:
    // 打开文件,会先释放之前打开的文件
    // file        = 文件路径
    // openMode    = 打开方式, -1 = 读写, 1 = 读入, 2 = 写出, 4 = 重写, 5 = 改写, 6 = 改读
    // c        = 共享方式,0 = 禁止读写, 1 = 禁止写, 2 = 禁止读,-1 = 无限制
    BOOL open(LPCWSTR file, int openMode = -1, int c = -1)
    {
        if (m_hFile)
        {
            CloseHandle(m_hFile);
            m_hFile = 0;
        }

        DWORD dwDesiredAccess, dwShareMode, dwCreationDisposition;

        switch (openMode)    // 打开方式,默认为读写
        {
        case 1://#读入
            dwDesiredAccess = FILE_READ_DATA;
            dwCreationDisposition = OPEN_EXISTING;
            break;
        case 2://#写出
            dwDesiredAccess = GENERIC_WRITE;
            dwCreationDisposition = OPEN_EXISTING;
            break;
        case 4://#重写
            dwDesiredAccess = GENERIC_WRITE;
            dwCreationDisposition = CREATE_ALWAYS;
            break;
        case 5://#改写
            dwDesiredAccess = GENERIC_WRITE;
            dwCreationDisposition = OPEN_ALWAYS;
            break;
        case 6://#改读
            dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
            dwCreationDisposition = OPEN_ALWAYS;
            break;
        default://#读写
            //dwDesiredAccess = FILE_READ_DATA | FILE_WRITE_DATA;
            dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
            dwCreationDisposition = OPEN_EXISTING;
            break;
        }

        if (c == 2)// 禁止读
            dwShareMode = FILE_SHARE_WRITE;
        else if (c == 1) // 禁止写
            dwShareMode = FILE_SHARE_READ;
        else if (c == 0) // 禁止读写
            dwShareMode = 0;
        else        // 无限制
            dwShareMode = (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);


        //HANDLE hFile = CreateFileW(file.c_str(), GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        HANDLE hFile = CreateFileW(file, dwDesiredAccess, dwShareMode, 0, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
        if (INVALID_HANDLE_VALUE == hFile)return FALSE;

        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        m_hFile = hFile;
        return TRUE;
    }

    // 打开文件,会先释放之前打开的文件
    // file        = 文件路径
    // openMode    = 打开方式, -1 = 读写, 1 = 读入, 2 = 写出, 4 = 重写, 5 = 改写, 6 = 改读
    // c        = 共享方式,0 = 禁止读写, 1 = 禁止写, 2 = 禁止读,-1 = 无限制
    BOOL open(LPCSTR file, int openMode = -1, int c = -1)
    {
        LPWSTR wzStr = AnsiToUnicode(file);
        BOOL ret = open(wzStr, openMode, c);
        delete[] wzStr;
        return ret;
    }

    // 清除数据,关闭文件,清空数据
    BOOL reset()
    {
        if (m_data)
        {
            delete[] m_data;
            m_data = 0;
        }
        if (m_hFile)
        {
            BOOL bRet = CloseHandle(m_hFile);
            if (bRet) m_hFile = 0;
            return bRet;
        }
        return FALSE;
    }

    // 读入数据,成功返回数据地址,失败返回0
    // size    = 读入尺寸,-1 = 文件长度, size大于文件尺寸则读入文件尺寸长度
    BOOL read(LPBYTE* buffer, int size = -1)
    {
        if (!buffer)return 0;
        if (m_data)
        {
            delete[] m_data;
            m_data = 0;
        }
        int len = CFileRW::size();
        if (len < 0)return 0;

        if (size <= 0 || size > len)size = len;

        DWORD dwNumOfByteRead = 0;
        LPBYTE pData = new BYTE[size];
        if (!ReadFile(m_hFile, pData, size, &dwNumOfByteRead, 0))
        {
            delete[] pData;
            return NULL;
        }


        m_data = pData;
        *buffer = pData;
        return TRUE;
    }
    BOOL read(LPBYTE buffer, int size)
    {
        if (!m_hFile || size <= 0)return 0;
        DWORD dwNumOfByteRead = 0;
        return ReadFile(m_hFile, buffer, size, &dwNumOfByteRead, 0);
    }

    // 从当前位置写入数据
    // data    = 要写入的数据
    // size     = 数据的尺寸
    BOOL write(LPCVOID data, UINT size)
    {
        if (!m_hFile)return 0;
        DWORD h = 0;
        return WriteFile(m_hFile, data, size, &h, NULL);
    }
    BOOL write(LPCSTR s)
    {
        if (!m_hFile)return 0;
        DWORD h = 0;
        return WriteFile(m_hFile, s, lstrlenA(s), &h, NULL);
    }
    


    // 取当前打开文件的总大小,失败返回-1
    int size()
    {
        if (!m_hFile)return -1;
        int err = 0;
        int size = GetFileSize(m_hFile, NULL);
        if (INVALID_FILE_SIZE == size)
        {
            err = GetLastError();
        }
        return size;
    }

    // 移动读写位置
    // lDistanceToMov    = 起始位置, 0 = 文件首, 1 = 现行位置, 2 = 文件尾,  FILE_ 开头常量
    BOOL SetPos(int lDistanceToMov, int dwMoveMethod)
    {
        if (!m_hFile)return 0;
        if (lDistanceToMov > 2 || lDistanceToMov < 0)lDistanceToMov = FILE_BEGIN;
        return SetFilePointer(m_hFile, dwMoveMethod, NULL, lDistanceToMov) != INVALID_SET_FILE_POINTER;
    }
    // 取读写位置
    DWORD GetPos()
    {
        if (!m_hFile)return 0;
        return SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
    }

    // 到文件首
    BOOL first()
    {
        if (!m_hFile)return 0;
        return SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER;
    }

    // 到文件尾
    BOOL tail()
    {
        if (!m_hFile)return 0;
        return SetFilePointer(m_hFile, 0, NULL, FILE_END) != INVALID_SET_FILE_POINTER;
    }
    HANDLE GetFile() { return m_hFile; }

    static LPSTR UnicodeToAnsi(LPCWSTR str, int len = 0, int cp = 936)
    {
        if (!str)return 0;
        if (len <= 0)len = lstrlenW(str);
        if (len <= 0)return 0;
        int aLen = WideCharToMultiByte(cp, 0, str, len, 0, 0, 0, 0) + 1;
        if (aLen < 1) return 0;

        LPSTR szStr = new CHAR[aLen];
        memset(szStr, 0, aLen);
        WideCharToMultiByte(cp, 0, str, len, szStr, aLen, 0, 0);
        return szStr;
    }
    static LPWSTR AnsiToUnicode(LPCSTR str, int len = 0, int cp = 936)
    {
        if (!str)return 0;
        if (len <= 0)len = lstrlenA(str);
        if (len <= 0)return 0;

        int uLen = MultiByteToWideChar(cp, 0, str, len, NULL, 0) + 1;
        if (uLen < 1) return 0;

        LPWSTR wzStr = new WCHAR[uLen];
        memset(wzStr, 0, uLen * sizeof(WCHAR));
        MultiByteToWideChar(cp, 0, str, len, wzStr, uLen);
        return wzStr;
    }
};

