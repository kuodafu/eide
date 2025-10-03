#pragma once
#include <windows.h>
#include <stdio.h>

#if _MSC_VER > 1200
#    include <type_traits>
#endif



// �ı�������, �ۼ�����������ı�ʱʹ��
// ����Ҫ�޸��ı�����, �����Ҫ�޸��ı����ݵĻ�, ���������������
// ������ʵ��������һ����ڴ�, ���ı��������涪, �ռ䲻��������ڴ�

//typedef char _Elem;
template<typename _Elem>
class CStringBuffer_base
{
private:

#if _MSC_VER > 1200
    using value_type        = typename std::remove_cv<_Elem>::type;
    using pointer           = value_type*;
    using const_pointer     = const value_type*;
    using reference         = value_type&;
    using const_reference   = const value_type&;

#else
    typedef _Elem                               value_type;
    typedef value_type*                         pointer;
    typedef const value_type*                   const_pointer;
    typedef value_type&                         reference;
    typedef const value_type&                   const_reference;

#endif

    typedef struct STRINGBUFFER
    {
        pointer pStart;     // �洢�ַ�����λ��, ÿ�μ����ַ��������뵽�����ַ, ÿ�μ���������ַ��ָ����һ������ַ����ĵ�ַ
        pointer pEnd;       // �洢�ַ����Ľ�����ַ, ������ʶ�������ֵ���ܴ��, ���û�н�����ʶ�������ֵ, �Ǿ��ǻ�����������, ��Ҫ����
    }*LPSTRINGBUFFER;

    LPSTRINGBUFFER m_ptr;   // ��������Ϣ
    LPSTRINGBUFFER* m_arr;  // ��¼����������ڴ��ַ
    size_t m_arrCount;      // ��ǰ����ĳ�Ա��, �����¼�����ڴ��, ����ʱ���Զ�����
    size_t m_arrBufCount;   // ���黺�����ߴ�
    size_t m_allocSize;     // ÿ������ĳߴ�, ÿ�����붼������


    // ����ռ�
    inline static pointer alloc(size_t size, value_type fill = 0)
    {
        if (size == 0)
            size = 1;
        pointer p = (pointer)malloc(size);
        if (!p) return 0;
        *p = '\0';
        return p;
    }
    inline static pointer alloc_str(size_t size)
    {
        size = size * sizeof(value_type) + sizeof(value_type);
        return alloc(size);
    }
    // �ͷſռ�
    inline static void dealloc(pointer ptr)
    {
        if (ptr)
            ::free(ptr);
    }
public:
    CStringBuffer_base() : m_ptr(0), m_arr(0), m_arrCount(0), m_arrBufCount(0), m_allocSize(0)
    {

    }
    explicit CStringBuffer_base(size_t size) : m_ptr(0), m_arr(0), m_arrCount(0), m_arrBufCount(0), m_allocSize(0)
    {
        m_ptr = _AddItem(size);
    }
    ~CStringBuffer_base()
    {
        if (m_arr && m_arrCount)
        {
            for (size_t i = 0; i < m_arrCount; i++)
            {
                pointer ptr = (pointer)m_arr[i];
                dealloc(ptr);
            }
            delete[] m_arr;
        }
    }

private:
    // Unicodeתansi, �ڲ���ʹ�û��������ڴ�����Ļ�����, ����Ҫ�ⲿ�ͷ�
    inline LPCSTR UnicodeToAnsi(const wchar_t* const unicode, size_t len = 0, int cp = 936)
    {
        if (!unicode)return "";
        if (len <= 0)len = wcslen(unicode);
        if (len <= 0)return "";
        int aLen = WideCharToMultiByte(cp, 0, unicode, (int)len, 0, 0, 0, 0) + 1;
        if (aLen <= 1) return "";

        LPSTR szStr = (LPSTR)apply_noclear(aLen);
        memset(szStr, 0, aLen);
        WideCharToMultiByte(cp, 0, unicode, (int)len, szStr, aLen, 0, 0);
        return szStr;
    }

    // ansiתUnicode, �ڲ���ʹ�û��������ڴ�����Ļ�����, ����Ҫ�ⲿ�ͷ�
    inline LPCWSTR AnsiToUnicode(const char* const ansi, size_t len = 0, int cp = 936)
    {
        if (!ansi)return L"";
        if (len <= 0)len = strlen(ansi);
        if (len <= 0)return L"";

        int uLen = MultiByteToWideChar(cp, 0, ansi, (int)len, NULL, 0) + 1;
        if (uLen <= 1) return L"";

        LPWSTR wzStr = (LPWSTR)apply_noclear(uLen * sizeof(wchar_t));
        memset(wzStr, 0, uLen * sizeof(wchar_t));
        uLen = MultiByteToWideChar(cp, 0, ansi, (int)len, wzStr, uLen);
        return wzStr;
    }

    // unicodeתUTF8, �ڲ���ʹ�û��������ڴ�����Ļ�����, ����Ҫ�ⲿ�ͷ�
    inline LPCSTR UnicodeToUtf8(const wchar_t* const str, size_t len = 0)
    {
        return UnicodeToAnsi(str, len, CP_UTF8);
    }

    // UTF8תunicode, �ڲ���ʹ�û��������ڴ�����Ļ�����, ����Ҫ�ⲿ�ͷ�
    inline LPCWSTR Utf8ToUnicode(const char* const str, size_t len = 0)
    {
        return AnsiToUnicode(str, len, CP_UTF8);
    }

    // UTF8תansi, �ڲ���ʹ�û��������ڴ�����Ļ�����, ����Ҫ�ⲿ�ͷ�
    inline LPCSTR Utf8ToAnsi(const char* const utf8, size_t len = 0)
    {
        // �Ȱ�utf8�ı�ת��unicode, �ڰ�unicodeת��ansi
        return UnicodeToAnsi(Utf8ToUnicode(utf8, len));
    }

    // ansiתUTF8, �ڲ���ʹ�û��������ڴ�����Ļ�����, ����Ҫ�ⲿ�ͷ�
    inline LPCSTR AnsiToUtf8(const char* const ansi, size_t len = 0)
    {
        // �Ȱ�ansi�ı�ת��unicode, �ڰ�unicodeת��utf8
        return UnicodeToAnsi(AnsiToUnicode(ansi, len), 0, CP_UTF8);
    }

public:
    // ���µ���ÿ�η����ڴ�ĳߴ�, �������������������ڴ�, ��һ�������ڴ�ʱ�Ż�ʹ��
    inline void reserve(size_t size)
    {
        m_allocSize = size;
    }

    // ��յ�ǰ�����Ѽ�����ַ���
    // isClearString = �Ƿ�����Ѵ�ŵ��ַ���, Ϊfalse��ֻ�ѻ������ճ���, �������ڴ�
    inline void clear(bool isClearString = false)
    {
        for (size_t i = 0; i < m_arrBufCount; i++)
        {
            LPSTRINGBUFFER _ptr = m_arr[i];
            if ( !_ptr ) continue;
            // ����ʼ��ַ�ĳɵ�һ������ַ�����λ��
            _ptr->pStart = (pointer)(((LPBYTE)_ptr) + sizeof(STRINGBUFFER));
            if (isClearString)
                memset(_ptr->pStart, 0, _ptr->pEnd - _ptr->pStart);
            *_ptr->pStart = 0;
        }
        if ( m_arr )
            m_ptr = m_arr[0];
    }

    // �����ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pstr = Ҫ������ַ���
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline const_pointer AddFormat(const_pointer fmt, ...)
    {
        va_list list;
        va_start(list, fmt);
        const_pointer ret = AddFormatEx(fmt, list);
        va_end(list);
        return ret;
    }

    // �����ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pstr = Ҫ������ַ���
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline const_pointer AddFormatEx(const_pointer fmt, va_list list)
    {

        size_t fmtBufSize = 0x1000;
        pointer fmtBuf = alloc_str(fmtBufSize);
        
        int r = __vsprintf(fmtBuf, fmtBufSize, fmt, list);
        while (r == -1)
        {
            fmtBufSize *= 2;
            dealloc(fmtBuf);
            fmtBuf = alloc_str(fmtBufSize);
            r = __vsprintf(fmtBuf, fmtBufSize, fmt, list);
        }

        const_pointer ret = AddString(fmtBuf);
        dealloc(fmtBuf);
        return ret;
    }

    // �����ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pstr = Ҫ������ַ���
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline const_pointer AddString(const_pointer pstr, size_t size = 0)
    {
        if (sizeof(value_type) == 1)
            return (const_pointer)AddStringA((LPCSTR)pstr, size);
        return (const_pointer)AddStringW((LPCWSTR)pstr, size);
    }

    // ��Ansi�ַ�������Unicode�ַ���, �ڲ����Ansiת����Unicode�ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pAnsi = Ҫ�����Ansi�ַ���, �ڲ��������ַ���ת����Unicode, ���ط���Unicode�ַ�����ַ
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline LPCWSTR AddStringWFromAnsi(LPCSTR pAnsi, size_t size = 0)
    {
        return AnsiToUnicode(pAnsi, size);
    }
    // ��Utf8�ַ�������Unicode�ַ���, �ڲ����Utf8ת����Unicode�ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pUtf8 = Ҫ�����Utf8�ַ���, �ڲ��������ַ���ת����Unicode, ���ط���Unicode�ַ�����ַ
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline LPCWSTR AddStringWFromUtf8(LPCSTR pUtf8, size_t size = 0)
    {
        return Utf8ToUnicode(pUtf8, size);
    }

    // ����Unicode�ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pstr = Ҫ������ַ���
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline LPCWSTR AddStringW(LPCWSTR pstr, size_t size = 0)
    {
        if (!pstr) return L"";
        if (size == 0 || size >= 0x7fffffff)
            size = wcslen(pstr);
        if (size == 0) return L"";

        const size_t bufSize = size * sizeof(wchar_t);

        LPWSTR pWrite = (LPWSTR)apply(bufSize + sizeof(wchar_t));
        memcpy(pWrite, pstr, bufSize);
        pWrite[size] = '\0';
        return pWrite;
    }

    // ��Unicode�ַ�������Ansi�ַ���, �ڲ����Unicodeת����Ansi�ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pUnicode = Ҫ�����Unicode�ַ���, �ڲ��������ַ���ת����Ansi, ���ط���Ansi�ַ�����ַ
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline LPCSTR AddStringAFromUnicode(LPCWSTR pUnicode, size_t size = 0)
    {
        return UnicodeToAnsi(pUnicode, size);
    }
    // ��Utf8�ַ�������Ansi�ַ���, �ڲ����Utf8ת����Ansi�ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pUtf8 = Ҫ�����Utf8�ַ���, �ڲ��������ַ���ת����Ansi, ���ط���Ansi�ַ�����ַ
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline LPCSTR AddStringAFromUtf8(LPCSTR pUtf8, size_t size = 0)
    {
        return Utf8ToAnsi(pUtf8, size);
    }

    // ����Ansi�ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pstr = Ҫ������ַ���
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline LPCSTR AddStringA(LPCSTR pstr, size_t size = 0)
    {
        static char empty[2] = { 0 };
        if (!pstr)
            return (LPCSTR)empty;
        if (size == 0 || size >= 0x7fffffff)
            size = strlen(pstr);
        if (size == 0)
            return (LPCSTR)empty;

        const size_t bufSize = size * sizeof(char);

        LPSTR pWrite = (LPSTR)apply(bufSize + sizeof(char));
        memcpy(pWrite, pstr, bufSize);
        pWrite[size] = '\0';
        return pWrite;
    }

    // ����Utf8�ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pUtf8 = Ҫ������ַ���
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline LPCSTR AddUtf8(LPCSTR pUtf8, size_t size = 0)
    {
        return AddStringA(pUtf8, size);
    }

    // ��Unicode�����Utf8�ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pUnicode = Ҫ�����Unicode�ַ���, �ڲ���ת����Utf8�ַ���
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline LPCSTR AddUtf8FromUnicode(LPCWSTR pUnicode, size_t size = 0)
    {
        return UnicodeToUtf8(pUnicode, size);
    }

    // ��Ansi�����Utf8�ַ���, �����ַ����ĵ�ַ, ���Բ��᷵��0
    // pAnsi = Ҫ�����Ansi�ַ���, �ڲ���ת����Utf8�ַ���
    // size = ����ĳ���, Ϊ0���Զ���ȡ�ı�����
    inline LPCSTR AddUtf8FromAnsi(LPCSTR pAnsi, size_t size = 0)
    {
        return AnsiToUtf8(pAnsi, size);
    }

    // �ӻ����������һ���д�����ݵ��ڴ�, ���ص��ڴ�д��һ������Խ��
    // ch = ����ʱʹ������ֽ�����ڴ�
    inline PVOID apply(size_t size, value_type ch = 0)
    {
        if (!m_arr)
        {
            m_ptr = _AddItem(0x1000);
        }

        if (size == 0 || size >= 0x7fffffff)
            size = 4;

        if (!m_ptr)
        {
            m_ptr = _AddItem(0x1000);
        }

        pointer pStart = m_ptr->pStart;
        pointer pEnd = pStart + size;

        LPSTRINGBUFFER AddPtr = m_ptr;  // ��Ҫ��ӵ��ĸ��ڴ����

        if ( pEnd >= m_ptr->pEnd )    // ����� pEnd�ǽ�����ַ, ���ڼ�¼�Ľ�����ַ, �Ǿ��ǻ�����������, ��Ҫ��������
        {
            // Ѱ���Ŀ��ڴ��ܹ����, �����û�о�����һ���µ��ڴ�
            size_t newSize = size + 1;
            m_ptr = _AddItem(newSize);   // ���������ڴ��ǰʹ�õ��ڴ���ָ�������������ڴ�
            AddPtr = m_ptr;
        }

        pointer pRet = AddPtr->pStart;
        for (size_t i = 0; i < size; i++)
            pRet[i] = ch;
        AddPtr->pStart += size;    // ָ����һ��������ݵĵ�ַ
        if (AddPtr->pStart < AddPtr->pEnd)
            AddPtr->pStart[0] = 0;
        return pRet;
    }

    // �ӻ����������һ���д�����ݵ��ڴ�, ���ص��ڴ�д��һ������Խ��
    inline PVOID apply_noclear(size_t size)
    {
        if ( !m_arr )
        {
            m_ptr = _AddItem(0x1000);
        }

        if ( size == 0 || size >= 0x7fffffff )
            size = 4;

        if ( !m_ptr )
        {
            m_ptr = _AddItem(0x1000);
        }

        pointer pStart = m_ptr->pStart;
        pointer pEnd = pStart + size;

        LPSTRINGBUFFER AddPtr = m_ptr;  // ��Ҫ��ӵ��ĸ��ڴ����

        if ( pEnd >= m_ptr->pEnd )    // ����� pEnd�ǽ�����ַ, ���ڼ�¼�Ľ�����ַ, �Ǿ��ǻ�����������, ��Ҫ��������
        {
            // Ѱ���Ŀ��ڴ��ܹ����, �����û�о�����һ���µ��ڴ�
            size_t newSize = size + 1;
            m_ptr = _AddItem(newSize);   // ���������ڴ��ǰʹ�õ��ڴ���ָ�������������ڴ�
            AddPtr = m_ptr;
        }

        pointer pRet = AddPtr->pStart;
        *pRet = 0;
        AddPtr->pStart += size;    // ָ����һ��������ݵĵ�ַ
        if ( AddPtr->pStart < AddPtr->pEnd )
            AddPtr->pStart[0] = 0;
        return pRet;
    }

    // ���������д���ı�, ���ԭ�����ı��㹻���, �Ǿͼ������, ����������ռ���, �����Ƿ����·������ڴ�
    // buf = ����ض���ָ��
    // ppszText = ����ı�ָ���ָ��, ������·������ڴ�, ����·���ĵ�ַд�����ָ��
    // text = Ҫд����ı�
    template<typename T>
    static bool SetBufText(CStringBuffer_base<value_type>* buf, const T** ppszText, const T* text)
    {
#if _MSC_VER > 1200
        using value_type = typename std::remove_cv<T>::type;
        using pointer = value_type*;
        using const_pointer = const value_type*;
#else
        typedef T                   value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
#endif

        if ( !text ) text = (pointer)L"";
        pointer pstr = (pointer)*ppszText;
        if ( !pstr )
        {
            pstr = (pointer)L"";
            *ppszText = pstr;
        }
        if ( !text[0] && !pstr[0] )
            return false;   // �������ǿ��ַ���, ������
        if ( __strcmp(text, pstr) == 0 )
            return false;   // �����ַ���һ��, ������

        size_t len1 = __lstrlen(pstr);
        size_t len2 = __lstrlen(text);
        if ( len1 >= len2 )
        {
            // �����, ֱ�Ӵ�Ž�ȥ
            memcpy(pstr, text, len2 * sizeof(value_type) + sizeof(value_type));
            return false;
        }


        // �������, �����µĻ��������
        if ( sizeof(value_type) == sizeof(wchar_t) )
            *ppszText = (pointer)buf->AddStringW((LPCWSTR)text, len2);
        else
            *ppszText = (pointer)buf->AddStringA((LPCSTR)text, len2);
        return true;

    }
    // ��ѯ�����ַ�ǲ�������ı�����ĵ�ַ
    bool query(const void* ptr)
    {
        auto pStart = (pointer)ptr;
        if (!pStart)
            return false;

        if (m_arr && m_arrCount)
        {
            for (size_t i = 0; i < m_arrCount; i++)
            {
                auto ptr = m_arr[i];
                if (ptr->pStart >= pStart && pStart < ptr->pEnd)
                    return true;
            }
        }
        return false;
    }
private:
    // ȡ�ı�����, ������������־
    inline static size_t __lstrlen(LPCSTR s) { return s ? strlen(s) : 0; }
    inline static size_t __lstrlen(LPCWSTR s) { return s ? wcslen(s) : 0; }

    inline static int ___vsnprintf_s(char* const _Buffer, size_t const _BufferCount, char const* const _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return _vsnprintf_s(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#else
        typedef int(__cdecl* fn_vsnprintf_s)(char* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, char const* const _Format, va_list _ArgList);
        static fn_vsnprintf_s fnFun;
        if (fnFun)
            return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
        HMODULE hModule = GetModuleHandleA("msvcrt.dll");
        if (!hModule)
        {
            hModule = LoadLibraryW(L"msvcrt.dll");
            if (!hModule)
                return vsprintf(_Buffer, _Format, _ArgList);
        }
        fnFun = (fn_vsnprintf_s)GetProcAddress(hModule, "_vsnprintf_s");
        if (!fnFun)
            return vsprintf(_Buffer, _Format, _ArgList);
        return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#endif
    }
    inline static int ___vsnwprintf_s(wchar_t* const _Buffer, size_t const _BufferCount, wchar_t const* const _Format, va_list _ArgList)
    {
#if _MSC_VER > 1200
        return _vsnwprintf_s(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#else
        typedef int(__cdecl* fn_vsnwprintf_s)(wchar_t* const _Buffer, size_t const _BufferCount, size_t const _MaxCount, wchar_t const* const _Format, va_list _ArgList);
        static fn_vsnwprintf_s fnFun;
        if (fnFun)
            return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
        HMODULE hModule = GetModuleHandleA("msvcrt.dll");
        if (!hModule)
        {
            hModule = LoadLibraryW(L"msvcrt.dll");
            if (!hModule)
                return vswprintf(_Buffer, _Format, _ArgList);
        }

        fnFun = (fn_vsnwprintf_s)GetProcAddress(hModule, "_vsnwprintf_s");
        if (!fnFun)
            return vswprintf(_Buffer, _Format, _ArgList);
        return fnFun(_Buffer, _BufferCount, _BufferCount - 1, _Format, _ArgList);
#endif
    }


    inline static int __vsprintf(char* const _Buffer, const size_t _bufferSize, const char* const _Format, va_list _ArgList) { return ___vsnprintf_s(_Buffer, _bufferSize, _Format, _ArgList); }
    inline static int __vsprintf(wchar_t* const _Buffer, const size_t _bufferSize, const wchar_t* _Format, va_list _ArgList) { return ___vsnwprintf_s(_Buffer, _bufferSize, _Format, _ArgList); }


    inline static int __strcmp(const char* const _String1, const char* const _String2) { return strcmp(_String1, _String2); }
    inline static int __strcmp(const wchar_t* const _String1, const wchar_t* _String2) { return wcscmp(_String1, _String2); }


    // ���һ���ڴ��¼��������, ��������ڴ�ĵ�ַ
    // size = ����ڴ�ĳߴ�, ��λΪ�ַ�, С�� 0x1000 ����뵽 0x1000
    inline LPSTRINGBUFFER _AddItem(size_t size)
    {
        if (m_allocSize <= 0)
            m_allocSize = 0x1000;

        if ( size < m_allocSize )
            size = m_allocSize; // ��С����4k

        // ����ټ���һ���ڴ��ͳ����˼�¼�ڴ�������, �Ǿ�����һ���µ��ڴ����������
        if (m_arrCount + 1 > m_arrBufCount)
        {
            if (m_arrBufCount == 0)
            {
                m_arrBufCount = 100;
                m_arr = new LPSTRINGBUFFER[m_arrBufCount];
                memset(m_arr, 0, sizeof(LPSTRINGBUFFER) * m_arrBufCount);
            }
            else
            {
                size_t newBufSize = m_arrBufCount * 2;
                LPSTRINGBUFFER* newArr = new LPSTRINGBUFFER[newBufSize];
                memset(newArr, 0, sizeof(LPSTRINGBUFFER) * newBufSize);
                memcpy(newArr, m_arr, sizeof(LPSTRINGBUFFER) * m_arrBufCount);

                delete[] m_arr;
                m_arr = newArr;
                m_arrBufCount = newBufSize;
            }
        }

        // ѭ���ӻ�����������鿴�ǿ��ڴ��㹻�������������
        for ( size_t i = 0; i < m_arrBufCount; i++ )
        {
            LPSTRINGBUFFER info = m_arr[i]; // �����Ա��Ϊ0, ֱ�ӷ��������Ա
            if ( info )
            {
                size_t _m_size = info->pEnd - info->pStart;
                if ( _m_size >= size )
                    return info;
            }
            else
            {
                m_arrCount = i; // �������Ա��ʼû�д������, ����ѭ��, �����ڴ�, ���ڴ��ŵ������Ա��
                break;
            }
        }

        size_t newSize = size;

        // ��һ����ŵĳ�Ա��0, û��������ڴ�, ����һ���ڴ��¼
        if ( m_allocSize < 0x1000000 && size < 0x1000000 )
        {
            m_allocSize += size;    // С��10M�Ļ������Ͷ����, ������Ҫ���ٷ������
            if ( m_allocSize > 0x1000000 )
                m_allocSize = 0x1000000; // ÿ��������10M
            newSize = m_allocSize;
        }
        LPBYTE p = (LPBYTE)alloc(newSize * sizeof(value_type) + sizeof(STRINGBUFFER));
        if ( p == 0 ) throw m_allocSize;

        LPSTRINGBUFFER ptr = (LPSTRINGBUFFER)p;
        ptr->pStart = (pointer)(p + sizeof(STRINGBUFFER));
        ptr->pEnd = ptr->pStart + newSize;
        *ptr->pStart = '\0';
        m_arr[m_arrCount++] = ptr;   // �ڴ������ڴ涼�ŵ����һ����Ա��
        return ptr;
    }

    // �ڲ�����ַ���, ���������ʱ������ڴ���һ������ŵ�
    inline const_pointer _AddString(LPSTRINGBUFFER ptr, const_pointer pstr, size_t size)
    {
        pointer p = ptr->pStart;
        memcpy(p, pstr, size * sizeof(value_type) + sizeof(value_type));
        ptr->pStart += size + 1;    // ָ����һ������ַ����ĵ�ַ
        *ptr->pStart = '\0';
        return p;
    }

};


typedef CStringBuffer_base<wchar_t> CStringBufferW;
typedef CStringBuffer_base<char>    CStringBufferA;
#ifdef _UNICODE
typedef CStringBuffer_base<wchar_t> CStringBuffer;
#else
typedef CStringBuffer_base<char>    CStringBuffer;
#endif