#pragma once
#include <windows.h>
#include <stdio.h>

#if _MSC_VER > 1200
#    include <type_traits>
#endif



// 文本缓冲区, 累计往里面加入文本时使用
// 不需要修改文本内容, 如果需要修改文本内容的话, 这个缓冲区不合适
// 这个类的实现是申请一块大内存, 有文本就往里面丢, 空间不足会申请内存

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
        pointer pStart;     // 存储字符串的位置, 每次加入字符串都加入到这个地址, 每次加入后这个地址都指向下一个存放字符串的地址
        pointer pEnd;       // 存储字符串的结束地址, 结束标识等于这个值还能存放, 如果没有结束标识大于这个值, 那就是缓冲区不够了, 需要增大
    }*LPSTRINGBUFFER;

    LPSTRINGBUFFER m_ptr;   // 缓冲区信息
    LPSTRINGBUFFER* m_arr;  // 记录所有申请的内存地址
    size_t m_arrCount;      // 当前数组的成员数, 这个记录所有内存块, 不够时会自动申请
    size_t m_arrBufCount;   // 数组缓冲区尺寸
    size_t m_allocSize;     // 每次申请的尺寸, 每次申请都会增大


    // 申请空间
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
    // 释放空间
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
    // Unicode转ansi, 内部是使用缓冲池里的内存申请的缓冲区, 不需要外部释放
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

    // ansi转Unicode, 内部是使用缓冲池里的内存申请的缓冲区, 不需要外部释放
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

    // unicode转UTF8, 内部是使用缓冲池里的内存申请的缓冲区, 不需要外部释放
    inline LPCSTR UnicodeToUtf8(const wchar_t* const str, size_t len = 0)
    {
        return UnicodeToAnsi(str, len, CP_UTF8);
    }

    // UTF8转unicode, 内部是使用缓冲池里的内存申请的缓冲区, 不需要外部释放
    inline LPCWSTR Utf8ToUnicode(const char* const str, size_t len = 0)
    {
        return AnsiToUnicode(str, len, CP_UTF8);
    }

    // UTF8转ansi, 内部是使用缓冲池里的内存申请的缓冲区, 不需要外部释放
    inline LPCSTR Utf8ToAnsi(const char* const utf8, size_t len = 0)
    {
        // 先把utf8文本转成unicode, 在把unicode转成ansi
        return UnicodeToAnsi(Utf8ToUnicode(utf8, len));
    }

    // ansi转UTF8, 内部是使用缓冲池里的内存申请的缓冲区, 不需要外部释放
    inline LPCSTR AnsiToUtf8(const char* const ansi, size_t len = 0)
    {
        // 先把ansi文本转成unicode, 在把unicode转成utf8
        return UnicodeToAnsi(AnsiToUnicode(ansi, len), 0, CP_UTF8);
    }

public:
    // 重新调整每次分配内存的尺寸, 不会调整现有已申请的内存, 下一次申请内存时才会使用
    inline void reserve(size_t size)
    {
        m_allocSize = size;
    }

    // 清空当前所有已加入的字符串
    // isClearString = 是否清空已存放的字符串, 为false则只把缓冲区空出来, 不清理内存
    inline void clear(bool isClearString = false)
    {
        for (size_t i = 0; i < m_arrBufCount; i++)
        {
            LPSTRINGBUFFER _ptr = m_arr[i];
            if ( !_ptr ) continue;
            // 把起始地址改成第一个存放字符串的位置
            _ptr->pStart = (pointer)(((LPBYTE)_ptr) + sizeof(STRINGBUFFER));
            if (isClearString)
                memset(_ptr->pStart, 0, _ptr->pEnd - _ptr->pStart);
            *_ptr->pStart = 0;
        }
        if ( m_arr )
            m_ptr = m_arr[0];
    }

    // 加入字符串, 返回字符串的地址, 绝对不会返回0
    // pstr = 要加入的字符串
    // size = 加入的长度, 为0则自动获取文本长度
    inline const_pointer AddFormat(const_pointer fmt, ...)
    {
        va_list list;
        va_start(list, fmt);
        const_pointer ret = AddFormatEx(fmt, list);
        va_end(list);
        return ret;
    }

    // 加入字符串, 返回字符串的地址, 绝对不会返回0
    // pstr = 要加入的字符串
    // size = 加入的长度, 为0则自动获取文本长度
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

    // 加入字符串, 返回字符串的地址, 绝对不会返回0
    // pstr = 要加入的字符串
    // size = 加入的长度, 为0则自动获取文本长度
    inline const_pointer AddString(const_pointer pstr, size_t size = 0)
    {
        if (sizeof(value_type) == 1)
            return (const_pointer)AddStringA((LPCSTR)pstr, size);
        return (const_pointer)AddStringW((LPCWSTR)pstr, size);
    }

    // 从Ansi字符串加入Unicode字符串, 内部会把Ansi转换成Unicode字符串, 返回字符串的地址, 绝对不会返回0
    // pAnsi = 要加入的Ansi字符串, 内部会把这个字符串转换成Unicode, 返回返回Unicode字符串地址
    // size = 加入的长度, 为0则自动获取文本长度
    inline LPCWSTR AddStringWFromAnsi(LPCSTR pAnsi, size_t size = 0)
    {
        return AnsiToUnicode(pAnsi, size);
    }
    // 从Utf8字符串加入Unicode字符串, 内部会把Utf8转换成Unicode字符串, 返回字符串的地址, 绝对不会返回0
    // pUtf8 = 要加入的Utf8字符串, 内部会把这个字符串转换成Unicode, 返回返回Unicode字符串地址
    // size = 加入的长度, 为0则自动获取文本长度
    inline LPCWSTR AddStringWFromUtf8(LPCSTR pUtf8, size_t size = 0)
    {
        return Utf8ToUnicode(pUtf8, size);
    }

    // 加入Unicode字符串, 返回字符串的地址, 绝对不会返回0
    // pstr = 要加入的字符串
    // size = 加入的长度, 为0则自动获取文本长度
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

    // 从Unicode字符串加入Ansi字符串, 内部会把Unicode转换成Ansi字符串, 返回字符串的地址, 绝对不会返回0
    // pUnicode = 要加入的Unicode字符串, 内部会把这个字符串转换成Ansi, 返回返回Ansi字符串地址
    // size = 加入的长度, 为0则自动获取文本长度
    inline LPCSTR AddStringAFromUnicode(LPCWSTR pUnicode, size_t size = 0)
    {
        return UnicodeToAnsi(pUnicode, size);
    }
    // 从Utf8字符串加入Ansi字符串, 内部会把Utf8转换成Ansi字符串, 返回字符串的地址, 绝对不会返回0
    // pUtf8 = 要加入的Utf8字符串, 内部会把这个字符串转换成Ansi, 返回返回Ansi字符串地址
    // size = 加入的长度, 为0则自动获取文本长度
    inline LPCSTR AddStringAFromUtf8(LPCSTR pUtf8, size_t size = 0)
    {
        return Utf8ToAnsi(pUtf8, size);
    }

    // 加入Ansi字符串, 返回字符串的地址, 绝对不会返回0
    // pstr = 要加入的字符串
    // size = 加入的长度, 为0则自动获取文本长度
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

    // 加入Utf8字符串, 返回字符串的地址, 绝对不会返回0
    // pUtf8 = 要加入的字符串
    // size = 加入的长度, 为0则自动获取文本长度
    inline LPCSTR AddUtf8(LPCSTR pUtf8, size_t size = 0)
    {
        return AddStringA(pUtf8, size);
    }

    // 从Unicode里加入Utf8字符串, 返回字符串的地址, 绝对不会返回0
    // pUnicode = 要加入的Unicode字符串, 内部会转换成Utf8字符串
    // size = 加入的长度, 为0则自动获取文本长度
    inline LPCSTR AddUtf8FromUnicode(LPCWSTR pUnicode, size_t size = 0)
    {
        return UnicodeToUtf8(pUnicode, size);
    }

    // 从Ansi里加入Utf8字符串, 返回字符串的地址, 绝对不会返回0
    // pAnsi = 要加入的Ansi字符串, 内部会转换成Utf8字符串
    // size = 加入的长度, 为0则自动获取文本长度
    inline LPCSTR AddUtf8FromAnsi(LPCSTR pAnsi, size_t size = 0)
    {
        return AnsiToUtf8(pAnsi, size);
    }

    // 从缓冲池里申请一块可写入数据的内存, 返回的内存写入一定不能越界
    // ch = 申请时使用这个字节填充内存
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

        LPSTRINGBUFFER AddPtr = m_ptr;  // 需要添加到哪个内存块里

        if ( pEnd >= m_ptr->pEnd )    // 这里的 pEnd是结束地址, 大于记录的结束地址, 那就是缓冲区不够大, 需要重新申请
        {
            // 寻找哪块内存能够存放, 如果都没有就申请一块新的内存
            size_t newSize = size + 1;
            m_ptr = _AddItem(newSize);   // 重新申请内存后当前使用的内存块就指向新申请的这块内存
            AddPtr = m_ptr;
        }

        pointer pRet = AddPtr->pStart;
        for (size_t i = 0; i < size; i++)
            pRet[i] = ch;
        AddPtr->pStart += size;    // 指向下一个存放数据的地址
        if (AddPtr->pStart < AddPtr->pEnd)
            AddPtr->pStart[0] = 0;
        return pRet;
    }

    // 从缓冲池里申请一块可写入数据的内存, 返回的内存写入一定不能越界
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

        LPSTRINGBUFFER AddPtr = m_ptr;  // 需要添加到哪个内存块里

        if ( pEnd >= m_ptr->pEnd )    // 这里的 pEnd是结束地址, 大于记录的结束地址, 那就是缓冲区不够大, 需要重新申请
        {
            // 寻找哪块内存能够存放, 如果都没有就申请一块新的内存
            size_t newSize = size + 1;
            m_ptr = _AddItem(newSize);   // 重新申请内存后当前使用的内存块就指向新申请的这块内存
            AddPtr = m_ptr;
        }

        pointer pRet = AddPtr->pStart;
        *pRet = 0;
        AddPtr->pStart += size;    // 指向下一个存放数据的地址
        if ( AddPtr->pStart < AddPtr->pEnd )
            AddPtr->pStart[0] = 0;
        return pRet;
    }

    // 往缓冲池里写入文本, 如果原来的文本足够存放, 那就继续存放, 不够就申请空间存放, 返回是否重新分配了内存
    // buf = 缓冲池对象指针
    // ppszText = 存放文本指针的指针, 如果重新分配了内存, 会把新分配的地址写入这个指针
    // text = 要写入的文本
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
            return false;   // 两个都是空字符串, 不处理
        if ( __strcmp(text, pstr) == 0 )
            return false;   // 两个字符串一样, 不处理

        size_t len1 = __lstrlen(pstr);
        size_t len2 = __lstrlen(text);
        if ( len1 >= len2 )
        {
            // 够存放, 直接存放进去
            memcpy(pstr, text, len2 * sizeof(value_type) + sizeof(value_type));
            return false;
        }


        // 不够存放, 创建新的缓冲区存放
        if ( sizeof(value_type) == sizeof(wchar_t) )
            *ppszText = (pointer)buf->AddStringW((LPCWSTR)text, len2);
        else
            *ppszText = (pointer)buf->AddStringA((LPCSTR)text, len2);
        return true;

    }
    // 查询这个地址是不是这个文本池里的地址
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
    // 取文本长度, 不包含结束标志
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


    // 添加一块内存记录到数组里, 返回这块内存的地址
    // size = 这块内存的尺寸, 单位为字符, 小于 0x1000 则对齐到 0x1000
    inline LPSTRINGBUFFER _AddItem(size_t size)
    {
        if (m_allocSize <= 0)
            m_allocSize = 0x1000;

        if ( size < m_allocSize )
            size = m_allocSize; // 最小分配4k

        // 如果再加入一块内存块就超过了记录内存块的数组, 那就申请一块新的内存存放这个数组
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

        // 循环从缓冲区数组里查看那块内存足够存放新增的数据
        for ( size_t i = 0; i < m_arrBufCount; i++ )
        {
            LPSTRINGBUFFER info = m_arr[i]; // 这个成员不为0, 直接返回这个成员
            if ( info )
            {
                size_t _m_size = info->pEnd - info->pStart;
                if ( _m_size >= size )
                    return info;
            }
            else
            {
                m_arrCount = i; // 从这个成员开始没有存放数据, 跳出循环, 申请内存, 把内存存放到这个成员内
                break;
            }
        }

        size_t newSize = size;

        // 下一个存放的成员是0, 没有申请过内存, 申请一块内存记录
        if ( m_allocSize < 0x1000000 && size < 0x1000000 )
        {
            m_allocSize += size;    // 小于10M的缓冲区就多分配, 否则需要多少分配多少
            if ( m_allocSize > 0x1000000 )
                m_allocSize = 0x1000000; // 每块最大分配10M
            newSize = m_allocSize;
        }
        LPBYTE p = (LPBYTE)alloc(newSize * sizeof(value_type) + sizeof(STRINGBUFFER));
        if ( p == 0 ) throw m_allocSize;

        LPSTRINGBUFFER ptr = (LPSTRINGBUFFER)p;
        ptr->pStart = (pointer)(p + sizeof(STRINGBUFFER));
        ptr->pEnd = ptr->pStart + newSize;
        *ptr->pStart = '\0';
        m_arr[m_arrCount++] = ptr;   // 内次申请内存都放到最后一个成员内
        return ptr;
    }

    // 内部存放字符串, 调用这个的时候这块内存是一定够存放的
    inline const_pointer _AddString(LPSTRINGBUFFER ptr, const_pointer pstr, size_t size)
    {
        pointer p = ptr->pStart;
        memcpy(p, pstr, size * sizeof(value_type) + sizeof(value_type));
        ptr->pStart += size + 1;    // 指向下一个存放字符串的地址
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