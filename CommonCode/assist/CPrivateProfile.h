#pragma once
#include <tstr.h>

class CPrivateProfile
{
private:    
    inline void free() { /*m_file.Release();*/ }    // �ͷż�¼���ļ�·��
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
    wstr m_file;    // �����ļ�·��
    bool m_isWrite; // ���������ʱ�Ƿ��Ĭ��ֵд��������
    CPrivateProfile() : m_isWrite(false) { }
    CPrivateProfile(const wchar_t* file, bool isWrite = false) : m_isWrite(isWrite) { open(file); }
    ~CPrivateProfile() { free(); }

public:
    inline const wchar_t* GetPathW() const { return m_file.c_str(); }
    inline const _str GetPathA() const { return m_file.w2a(); }

    // �������ļ�
    inline bool open(const wchar_t* file)
    {
        if (!file)return false;
        m_file.assign(file);
        return file != 0;
    }

    // д������
    // AppName = ���ý���
    // KeyName = �������� Ϊ����ɾ�����С�ڵ�����������
    // String  = д������ Ϊ����ɾ����������е��ִ�
    inline bool write(const wchar_t* AppName, const wchar_t* KeyName = 0, const wchar_t* String = 0) const
    {
        if (m_file.empty())return 0;
        return WritePrivateProfileStringW(AppName, KeyName, String, m_file.c_str()) != FALSE;
    }
    // д������A��
    // AppName = ���ý���
    // KeyName = �������� Ϊ����ɾ�����С�ڵ�����������
    // String  = д������ Ϊ����ɾ����������е��ִ�
    inline bool write(const char* AppName, const char* KeyName = 0, const char* String = 0) const
    {
        if (m_file.empty())return 0;
        return WritePrivateProfileStringA(AppName, KeyName, String, m_file.w2a().c_str()) != FALSE;
    }

    // д������
    // AppName = ���ý���
    // KeyName = ��������
    // String  = д������
    inline bool write(const wchar_t* AppName, const wchar_t* KeyName, LONGLONG String) const
    {
        if (m_file.empty())return 0;
        wchar_t buffer[50] = { 0 };
        swprintf_api(buffer, 50, L"%lld", String);
        return WritePrivateProfileStringW(AppName, KeyName, buffer, m_file.c_str()) != FALSE;
    }

    // ��������
    // AppName = ���ý��� Ϊ���򷵻����н���, �������֮����\0�ָ�
    // KeyName = �������� Ϊ���򷵻���������, �������֮����\0�ָ�
    // Default = Ĭ������
    // count   = AppName/KeyName Ϊ��ʱ���ض�Ӧ������
    inline wstr read(const wchar_t* AppName = 0, const wchar_t* KeyName = 0, const wchar_t* Default = 0, DWORD* count = 0)
    {
        if (m_file.empty())return 0;

        DWORD bufferSize = (DWORD)260;
        wstr ret(bufferSize);

        DWORD len = GetPrivateProfileStringW(AppName, KeyName, Default, ret.data(), bufferSize, m_file.c_str());
        if (m_isWrite && GetLastError() == ERROR_FILE_NOT_FOUND) // û�д������������, ��Ҫд��
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
        if ((!AppName || !KeyName) && count)    // ��Ŀ���߽���Ϊ�ղ�ȡ����
            *count = GetStrCount(ret.c_str());
        ret.resize(len);
        return ret;
    }

    // ��������
    // AppName = ���ý��� Ϊ���򷵻����н���, �������֮����\0�ָ�
    // KeyName = �������� Ϊ���򷵻���������, �������֮����\0�ָ�
    // Default = Ĭ������
    // count   = AppName/KeyName Ϊ��ʱ���ض�Ӧ������
    inline _str readA(const char* AppName = 0, const char* KeyName = 0, const char* Default = 0, DWORD* count = 0)
    {
        if (m_file.empty())return 0;

        DWORD bufferSize = (DWORD)260;
        _str ret(bufferSize);
        _str fileA = m_file.w2a();

        DWORD len = GetPrivateProfileStringA(AppName, KeyName, Default, ret.data(), bufferSize, fileA.c_str());
        if (m_isWrite && GetLastError() == ERROR_FILE_NOT_FOUND) // û�д������������, ��Ҫд��
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
        if ((!AppName || !KeyName) && count)    // ��Ŀ���߽���Ϊ�ղ�ȡ����
            *count = GetStrCountA(ret.c_str());

        ret.resize(strlen(ret.c_str()));
        return ret;
    }


    // ������������
    // AppName = ���ý��� Ϊ���򷵻ؽ�������
    // KeyName = �������� Ϊ���򷵻���������
    // Default = Ĭ������
    inline int read_int(const wchar_t* AppName = 0, const wchar_t* KeyName = 0, int Default = 0)
    {
        wchar_t buffer[20] = { 0 };
        _itow_api(Default, buffer, 20, 10);

        DWORD count = 0;
        wstr str = read(AppName, KeyName, buffer, &count);
        if (!AppName || !KeyName) return count;    // ��Ŀ���߽���Ϊ�ղ�ȡ����
        return str.stoi();  // ������ı�ת����ֵ
    }
    
    // ȡ���н���, ȡ���н���,�ɹ��������н���
    inline wstr getAllAppname(DWORD* count)
    {
        return read(0, 0, 0, count);
    }

    // ȡ��������, ȡĳ�������µ���������,�ɹ�������������
    inline wstr getAllKeyname(const wchar_t* AppName, DWORD* count)
    {
        return read(AppName, 0, 0, count);
    }

    // ��ȡָ���ڵ���������������, �ɹ���������������, ʧ�ܷ���0
    // appName = �ڵ���, ���ı��򷵻�0
    inline int GetChildCount(LPCWSTR appName)
    {
        if (!appName || !appName[0]) return 0;
        DWORD count = 0;
        wstr value = read(appName, 0, 0, &count);
        return (int)count;
    }

    // ��ȡָ���ڵ���ָ��������������
    // appName = �ڵ���, ���ı��򷵻�0
    // index = �ڼ���������, ��0��ʼ
    // defValue = ��ȡʧ��ʱ��Ĭ��ֵ
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
    // ��ȡָ���ڵ���ָ��������������
    // appName = �ڵ���, ���ı��򷵻�0
    // index = �ڼ���������, ��0��ʼ
    // defValue = ��ȡʧ��ʱ��Ĭ��ֵ
    inline int GetChildValue_int(LPCWSTR appName, int index, int defValue = 0)
    {
        if (!appName || !appName[0]) return 0;
        wstr def = wstr::to_string(defValue);
        def = GetChildValue(appName, index, def.c_str());
        return def.stol();
    }

    // ö���ӽڵ�, ����д�뻺�����ĳ�Ա��
    // pBuf = ����ö�ٵ�������������ָ��, Ϊ0�򷵻س�Ա��
    // nBufSize = �����������Ա��, Ϊ0�򷵻س�Ա��
    // pBufName = ����ö�ٵ���������������ָ��
    // nBufNameSize = �����������Ա��, Ϊ0�򷵻س�Ա��
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

    // ö���ӽڵ�, ����д�뻺�����ĳ�Ա��
    // pBuf = ����ö�ٵ�������������ָ��, Ϊ0�򷵻س�Ա��
    // nBufSize = �����������Ա��, Ϊ0�򷵻س�Ա��
    // pBufName = ����ö�ٵ���������������ָ��
    // nBufNameSize = �����������Ա��, Ϊ0�򷵻س�Ա��
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

