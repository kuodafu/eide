#pragma once
#include <windows.h>
#include <dbghelp.h>

template <class T>
T* GetData(LPBYTE& pData)
{
    T* p = (T*)pData;
    int offset = sizeof(T);
    pData += offset;
    return p;
}


class IPEHeader
{
public:

    //分析文件头, 并且填充到变量中。
    virtual bool ScanPEHeader() = 0;

    //获取sectionheader的数量。
    virtual WORD GetSectionCount()const = 0;

    //获取指定索引的header
    virtual bool GetSectionHeader(int index, IMAGE_SECTION_HEADER* sectionHeader)const = 0;

private:

};

// PE 操作类
class PEOperation
{
public:
    PEOperation();

    // 从文件里操作PE, fileName = 可执行文件完整路径
    PEOperation(LPCWSTR fileName);

    // 从内存里操作PE, pData = 可执行文件的数据起始地址, size = 可执行文件尺寸
    PEOperation(LPBYTE pData, int size);
    ~PEOperation();

    BOOL open(LPCWSTR fileName);
    BOOL open(LPBYTE pData, int size);

public:

    //获取PE区段数量。
    virtual WORD GetSectionCount()const;

    //获取指定索引的header, 在外部修改返回值指向的值会影响最后生成的可执行文件
    PIMAGE_SECTION_HEADER GetSectionHeader(int index)const;
    PIMAGE_SECTION_HEADER GetSectionHeader(LPCSTR name)const;

    // 取ntHrader的指针, 在外部修改返回值指向的值会影响最后生成的可执行文件
    PIMAGE_NT_HEADERS GetNTHeaderPostion()const;
public:
    // 新增一个区段
    // name = 区段名,最多8个字节,超过8字节请参考msdn
    // data = 新增区段的数据
    // size = 数据大小
    BOOL AddSectionData(char name[IMAGE_SIZEOF_SHORT_NAME], const LPVOID data, int size);

    // 删除指定区段,删除后区段指向的数据也一并删除, 暂未实现
    // name = 区段名
    BOOL DeleteSction(LPCSTR name);

    // 取指定区段名指向的数据
    // name = 区段名
    // data = 缓冲区地址,如果缓冲区为0,则size返回缓冲区所需大小
    // size = 缓冲区大小,实际写入的大小,data=0时返回区段名指向数据的大小
    BOOL GetSectionData(LPCSTR name, LPVOID data, int* size);

    // 取指定区段名指向的数据
    // index = 区段名索引,索引从0开始
    // data = 缓冲区地址,如果缓冲区为0,则size返回缓冲区所需大小
    // size = 缓冲区大小,实际写入的大小,data=0时返回区段名指向数据的大小
    BOOL GetSectionData(UINT index, LPBYTE* data, int* size);

    // 取出当前可执行文件的数据, 在外部修改返回值指向的值会影响最后生成的可执行文件
    LPBYTE GetData(int* size = 0);

private:
    LPBYTE m_pData;
    int m_size;
};


class PEHeaderFromMemory : public IPEHeader
{
public:
    PEHeaderFromMemory();
    //模块的基地址。
    PEHeaderFromMemory(void* pBase);


public:
    //分析文件头, 并且填充到变量中。
    bool ScanPEHeader();

    //获取sectionheader的数量。
    WORD GetSectionCount()const;

    //获取指定索引的header
    bool GetSectionHeader(int index, IMAGE_SECTION_HEADER* sectionHeader)const;

    void* GetNTHeaderPostion()const;
    void* GetSectionHeaderPostion()const;


private:
    void* m_pBase;
    IMAGE_DOS_HEADER m_dosHeader;
    IMAGE_NT_HEADERS32 m_ntHeader;
};

class PEHeaderFromFile : public IPEHeader
{
public:
    PEHeaderFromFile();
    PEHeaderFromFile(HANDLE hFile);

public:
    //分析文件头, 并且填充到变量中。
    bool ScanPEHeader();

    //获取sectionheader的数量。
    WORD GetSectionCount()const;

    //获取指定索引的header
    bool GetSectionHeader(int index, IMAGE_SECTION_HEADER* sectionHeader)const;

    int GetNTHeaderPostion()const;
    int GetSectionHeaderPostion()const;
private:
    HANDLE m_hFile;
    IMAGE_DOS_HEADER m_dosHeader;
    IMAGE_NT_HEADERS32 m_ntHeader;
};
