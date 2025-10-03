#pragma once
#include <windows.h>

// ӳ����ڴ�, �����Զ��ͷ�
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
    // �ڴ�ӳ���ļ��ı�־, ��־�������260���ַ�
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
    // ��ȡ��������
    inline LPCWSTR name() { return m_name; }
    // �������ñ������ӳ���ļ���־, ��ر�ԭ�ж���, ��־�������260���ַ�
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
    
    
    // ���ڴ�ӳ���ļ�
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
    
    // �رյ�ǰӳ���ļ�, ������ʱ���Զ��ͷ�
    inline bool close()
    {
        bool bRet = 0;
        if (hFileMap)
            bRet = CloseHandle(hFileMap);
        hFileMap = 0;
        return bRet;
    }

    // ����һ���ڴ��ļ�ӳ��, ����ļ��Ѿ�����, Ҳ�᷵�سɹ�, ���ǲ��ǰ����ݵĳߴ紴��
    inline int create(const size_t size = 0x1000)
    {
        if ( !m_name || !m_name[0] )
            return false;
        
        const size_t len = ( size / 0x1000) * 0x1000 + (( size % 0x1000) > 0 ? 0x1000 : 0);  // 4k����
        hFileMap = CreateFileMappingW(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, (DWORD)len, m_name);
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            // ����֮ǰ�Ѵ���, ���ص���ԭ����ľ��, ��С����ԭ����Ĵ�С
            //close();
            return 2;
        }
        return hFileMap != 0;
    }

    // ������, �����ݿ�����ָ����������
    // _buffer = ��������ַ
    // _size = ����ȡ�ĳߴ�, ��λΪ�ֽ�
    // _off = ��ʼ��ȡλ��, ��λΪ�ֽ�
    inline bool read(void* _buffer, const size_t _size, const size_t _off = 0)
    {
        LPVOID mem = map(_off, _size);
        if ( !mem ) return 0;
        memcpy(_buffer, mem, _size);
        unmap(mem);
        return true;
    }
    // ������, �����ݿ�����ָ����������
    // _buffer = ��������ַ
    // _off = ��ʼ��ȡλ��, ��λΪ�ֽ�
    template<typename T>
    inline bool read(T* _buffer, const ULONG64 _off = 0)
    {
        return read(_buffer, sizeof(T), _off);
    }

    // д������, 
    // _data = д�����ݵĵ�ַ
    // _size = д�����ݵĳ���, ��λΪ�ֽ�
    // _off = д���λ��
    inline bool write(const void* _data, const size_t _size, const ULONG64 _off = 0)
    {
        LPVOID mem = map(_off, _size);
        if ( !mem ) return 0;
        memcpy(mem, _data, _size);
        unmap(mem);
        return true;
    }

    // д������, 
    // _data = д�����ݵĵ�ַ
    // _off = д���λ��
    template<typename T>
    inline bool write(const T* _data, const ULONG64 _off = 0)
    {
        return write(_data, sizeof(T), _off);
    }

    // ��ȡ��ǰ��������ݵ�ַ, �������ͷ�, �����Ե����ڴ��С, ��ʹ��ʱ������� unmap() ���ͷ�
    // �ɹ�����ӳ����ڴ��ַ, ʧ�ܷ���0
    // _off = ���Ǹ��ط���ʼӳ��
    // mapSize = ӳ��Ĵ�С, 0��ʾӳ���_off���ļ���β
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
            // 4k����
            lo = ( lo / 4096 ) * 4096;
            offset = loOffset - lo;
        }
        LPVOID mem = MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, hiOffset, lo, mapSize);
        if ( !mem ) return 0;
        
        return (LPBYTE)mem + offset;
    }

    // ��ȡ��ǰ��������ݵ�ַ, �������ͷ�, �����Ե����ڴ��С, ��ʹ��ʱ������� unmap() ���ͷ�
    // �ɹ�����ӳ����ڴ��ַ, ʧ�ܷ���0
    // _off = ���Ǹ��ط���ʼӳ��
    // mapSize = ӳ��Ĵ�С, 0��ʾӳ���_off���ļ���β
    template<typename T>
    inline T* map(ULONG64 _off)
    {
        return (T*)map(_off, sizeof(T));
    }

    // �ͷ�map()��ȡ�����ݵ�ַ
    inline bool unmap(LPVOID ptr)
    {
        if ( !ptr )return false;
        return UnmapViewOfFile(ptr);
    }
};

