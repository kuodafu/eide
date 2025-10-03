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

    //�����ļ�ͷ, ������䵽�����С�
    virtual bool ScanPEHeader() = 0;

    //��ȡsectionheader��������
    virtual WORD GetSectionCount()const = 0;

    //��ȡָ��������header
    virtual bool GetSectionHeader(int index, IMAGE_SECTION_HEADER* sectionHeader)const = 0;

private:

};

// PE ������
class PEOperation
{
public:
    PEOperation();

    // ���ļ������PE, fileName = ��ִ���ļ�����·��
    PEOperation(LPCWSTR fileName);

    // ���ڴ������PE, pData = ��ִ���ļ���������ʼ��ַ, size = ��ִ���ļ��ߴ�
    PEOperation(LPBYTE pData, int size);
    ~PEOperation();

    BOOL open(LPCWSTR fileName);
    BOOL open(LPBYTE pData, int size);

public:

    //��ȡPE����������
    virtual WORD GetSectionCount()const;

    //��ȡָ��������header, ���ⲿ�޸ķ���ֵָ���ֵ��Ӱ��������ɵĿ�ִ���ļ�
    PIMAGE_SECTION_HEADER GetSectionHeader(int index)const;
    PIMAGE_SECTION_HEADER GetSectionHeader(LPCSTR name)const;

    // ȡntHrader��ָ��, ���ⲿ�޸ķ���ֵָ���ֵ��Ӱ��������ɵĿ�ִ���ļ�
    PIMAGE_NT_HEADERS GetNTHeaderPostion()const;
public:
    // ����һ������
    // name = ������,���8���ֽ�,����8�ֽ���ο�msdn
    // data = �������ε�����
    // size = ���ݴ�С
    BOOL AddSectionData(char name[IMAGE_SIZEOF_SHORT_NAME], const LPVOID data, int size);

    // ɾ��ָ������,ɾ��������ָ�������Ҳһ��ɾ��, ��δʵ��
    // name = ������
    BOOL DeleteSction(LPCSTR name);

    // ȡָ��������ָ�������
    // name = ������
    // data = ��������ַ,���������Ϊ0,��size���ػ����������С
    // size = ��������С,ʵ��д��Ĵ�С,data=0ʱ����������ָ�����ݵĴ�С
    BOOL GetSectionData(LPCSTR name, LPVOID data, int* size);

    // ȡָ��������ָ�������
    // index = ����������,������0��ʼ
    // data = ��������ַ,���������Ϊ0,��size���ػ����������С
    // size = ��������С,ʵ��д��Ĵ�С,data=0ʱ����������ָ�����ݵĴ�С
    BOOL GetSectionData(UINT index, LPBYTE* data, int* size);

    // ȡ����ǰ��ִ���ļ�������, ���ⲿ�޸ķ���ֵָ���ֵ��Ӱ��������ɵĿ�ִ���ļ�
    LPBYTE GetData(int* size = 0);

private:
    LPBYTE m_pData;
    int m_size;
};


class PEHeaderFromMemory : public IPEHeader
{
public:
    PEHeaderFromMemory();
    //ģ��Ļ���ַ��
    PEHeaderFromMemory(void* pBase);


public:
    //�����ļ�ͷ, ������䵽�����С�
    bool ScanPEHeader();

    //��ȡsectionheader��������
    WORD GetSectionCount()const;

    //��ȡָ��������header
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
    //�����ļ�ͷ, ������䵽�����С�
    bool ScanPEHeader();

    //��ȡsectionheader��������
    WORD GetSectionCount()const;

    //��ȡָ��������header
    bool GetSectionHeader(int index, IMAGE_SECTION_HEADER* sectionHeader)const;

    int GetNTHeaderPostion()const;
    int GetSectionHeaderPostion()const;
private:
    HANDLE m_hFile;
    IMAGE_DOS_HEADER m_dosHeader;
    IMAGE_NT_HEADERS32 m_ntHeader;
};
