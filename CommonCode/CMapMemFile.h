#pragma once
#include <windows.h>

// 映射的内存, 析构自动释放
class CMapMemory
{
    LPVOID pMem;
public:
    CMapMemory(LPVOID ptr) :pMem(ptr) { }
    ~CMapMemory()
    {
        if ( pMem )
            UnmapViewOfFile(pMem);
        pMem = 0;
    }
    inline operator LPVOID() const { return pMem; }
    inline operator bool() const { return pMem != 0; }
    template<typename T>
    inline operator T()
    {
        return (T)pMem;
    }
};

class CMapMemFile
{
private:
    HANDLE hFileMap;
    wchar_t m_name[260];
public:
    // 内存映射文件的标志, 标志长度最大260个字符
    CMapMemFile(LPCWSTR name = 0) : hFileMap(0)
    {
        memset(m_name, 0, sizeof(m_name));
        this->name(name);
    }
    ~CMapMemFile()
    {
        close();
    }
public:
    inline bool empty() { return hFileMap == 0; }
    // 获取对象名字
    inline LPCWSTR name() { return m_name; }
    // 重新设置本对象的映射文件标志, 会关闭原有对象, 标志长度最大260个字符
    inline LPCWSTR name(LPCWSTR name)
    {
        if ( !name || !name[0] )
            return m_name;
        
        size_t len = wcslen(name) + 1;
        if ( len >= 260 )
            len = 259;
        memcpy(m_name, name, len * sizeof(wchar_t));
        return m_name;
    }
    
    inline operator LPCWSTR() { return m_name; }
    inline operator HANDLE() { return hFileMap; }
    inline void operator=(LPCWSTR name) { this->name(name); }
    inline HANDLE attach(HANDLE hMap)
    {
        HANDLE old = hFileMap;
        hFileMap = hMap;
        return old;
    }
    
    
    // 打开内存映射文件
    inline bool open(DWORD flag = FILE_MAP_ALL_ACCESS)
    {
        if (!m_name || !m_name[0])
            return false;
        if (hFileMap)
        {
            CloseHandle(hFileMap);
            hFileMap = 0;
        }
        hFileMap = OpenFileMappingW(flag, 0, m_name);
        return hFileMap != 0;
    }
    
    // 关闭当前映射文件, 类析构时会自动释放
    inline bool close()
    {
        bool bRet = 0;
        if (hFileMap)
            bRet = CloseHandle(hFileMap);
        hFileMap = 0;
        return bRet;
    }

    // 创建一个内存文件映射, 如果文件已经创建, 也会返回成功, 但是不是按传递的尺寸创建
    inline int create(const size_t size = 0x1000)
    {
        if ( !m_name || !m_name[0] )
            return false;
        
        const size_t len = ( size / 0x1000) * 0x1000 + (( size % 0x1000) > 0 ? 0x1000 : 0);  // 4k对齐
        hFileMap = CreateFileMappingW(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, (DWORD)len, m_name);
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            // 对象之前已存在, 返回的是原对象的句柄, 大小还是原对象的大小
            //close();
            return 2;
        }
        return hFileMap != 0;
    }

    // 读数据, 把数据拷贝到指定缓冲区中
    // _buffer = 缓冲区地址
    // _size = 欲读取的尺寸, 单位为字节
    // _off = 起始读取位置, 单位为字节
    inline bool read(void* _buffer, const size_t _size, const size_t _off = 0)
    {
        LPVOID mem = map(_off, _size);
        if ( !mem ) return 0;
        memcpy(_buffer, mem, _size);
        unmap(mem);
        return true;
    }
    // 读数据, 把数据拷贝到指定缓冲区中
    // _buffer = 缓冲区地址
    // _off = 起始读取位置, 单位为字节
    template<typename T>
    inline bool read(T* _buffer, const ULONG64 _off = 0)
    {
        return read(_buffer, sizeof(T), _off);
    }

    // 写入数据, 
    // _data = 写入数据的地址
    // _size = 写入数据的长度, 单位为字节
    // _off = 写入的位置
    inline bool write(const void* _data, const size_t _size, const ULONG64 _off = 0)
    {
        LPVOID mem = map(_off, _size);
        if ( !mem ) return 0;
        memcpy(mem, _data, _size);
        unmap(mem);
        return true;
    }

    // 写入数据, 
    // _data = 写入数据的地址
    // _off = 写入的位置
    template<typename T>
    inline bool write(const T* _data, const ULONG64 _off = 0)
    {
        return write(_data, sizeof(T), _off);
    }

    // 获取当前对象的数据地址, 不可以释放, 不可以调整内存大小, 不使用时必须调用 unmap() 来释放
    // 成功返回映射的内存地址, 失败返回0
    // _off = 从那个地方开始映射
    // mapSize = 映射的大小, 0表示映射从_off到文件结尾
    inline LPVOID map(ULONG64 _off = 0, SIZE_T mapSize = 0)
    {
        if (!hFileMap)return 0;
        LPINT pInt = (LPINT)&_off;
        DWORD hiOffset = pInt[1];
        DWORD loOffset = pInt[0];
        DWORD lo = loOffset;
        DWORD offset = 0;
        if ( loOffset % 4096 != 0 )
        {
            // 4k对齐
            lo = ( lo / 4096 ) * 4096;
            offset = loOffset - lo;
        }
        LPVOID mem = MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, hiOffset, lo, mapSize);
        if ( !mem ) return 0;
        
        return (LPBYTE)mem + offset;
    }

    // 获取当前对象的数据地址, 不可以释放, 不可以调整内存大小, 不使用时必须调用 unmap() 来释放
    // 成功返回映射的内存地址, 失败返回0
    // _off = 从那个地方开始映射
    // mapSize = 映射的大小, 0表示映射从_off到文件结尾
    template<typename T>
    inline T* map(ULONG64 _off)
    {
        return (T*)map(_off, sizeof(T));
    }

    // 释放map()获取的数据地址
    inline bool unmap(LPVOID ptr)
    {
        if ( !ptr )return false;
        return UnmapViewOfFile(ptr);
    }
};

