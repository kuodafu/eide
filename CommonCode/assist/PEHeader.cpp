#include "PEHeader.h"
#pragma comment(lib,"Dbghelp.lib")
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

int GetOne(double num)
{
    int result = (int)num;
    if (result < num)
    {
        result++;
    }
    return result;
}

PEHeaderFromMemory::PEHeaderFromMemory()
{
    m_pBase = 0;
    ZeroMemory(&m_dosHeader, sizeof(IMAGE_DOS_HEADER));
    ZeroMemory(&m_ntHeader, sizeof(IMAGE_NT_HEADERS32));
}

PEHeaderFromMemory::PEHeaderFromMemory(void* pBase)
{
    m_pBase = 0;
    ZeroMemory(&m_dosHeader, sizeof(IMAGE_DOS_HEADER));
    ZeroMemory(&m_ntHeader, sizeof(IMAGE_NT_HEADERS32));
    m_pBase = pBase;
}

bool PEHeaderFromMemory::ScanPEHeader()
{
    RtlMoveMemory(&m_dosHeader, m_pBase, sizeof(IMAGE_DOS_HEADER));
    RtlMoveMemory(&m_ntHeader, GetNTHeaderPostion(), sizeof(IMAGE_NT_HEADERS32));
    return true;
}

WORD PEHeaderFromMemory::GetSectionCount() const
{
    return m_ntHeader.FileHeader.NumberOfSections;
}

bool PEHeaderFromMemory::GetSectionHeader(int index, IMAGE_SECTION_HEADER* sectionHeader)const
{
    if (index > GetSectionCount())
    {
        sectionHeader = 0;
        return false;
    }

    int nextPos = (int)GetSectionHeaderPostion();
    if (index > 0)
    {
        nextPos = nextPos + sizeof(IMAGE_SECTION_HEADER) * index;
    }
    RtlMoveMemory(sectionHeader, (LPVOID)(nextPos), sizeof(IMAGE_SECTION_HEADER));

    return true;
}

void* PEHeaderFromMemory::GetNTHeaderPostion() const
{
    return (LPVOID)((int)m_pBase + (int)m_dosHeader.e_lfanew);
}

void* PEHeaderFromMemory::GetSectionHeaderPostion() const
{
    return (LPVOID)((int)GetNTHeaderPostion() + sizeof(IMAGE_NT_HEADERS32));
}

PEHeaderFromFile::PEHeaderFromFile()
{
    m_hFile = INVALID_HANDLE_VALUE;
    ZeroMemory(&m_dosHeader, sizeof(IMAGE_DOS_HEADER));
    ZeroMemory(&m_ntHeader, sizeof(IMAGE_NT_HEADERS32));
}

PEHeaderFromFile::PEHeaderFromFile(HANDLE hFile)
{
    m_hFile = INVALID_HANDLE_VALUE;
    ZeroMemory(&m_dosHeader, sizeof(IMAGE_DOS_HEADER));
    ZeroMemory(&m_ntHeader, sizeof(IMAGE_NT_HEADERS32));
    m_hFile = hFile;
    SetFilePointer(m_hFile, 0, 0, FILE_BEGIN);
    ScanPEHeader();
}

bool PEHeaderFromFile::ScanPEHeader()
{
    BOOL bRet = FALSE;

    DWORD dwNumberReaded = 0;
    bRet = ReadFile(m_hFile, &m_dosHeader, sizeof(IMAGE_DOS_HEADER), &dwNumberReaded, 0);
    if (bRet == FALSE)
        return false;

    //�ƶ���ô��λ��, �õ��ļ�ͷ��ʼƫ��
    SetFilePointer(m_hFile, m_dosHeader.e_lfanew, 0, FILE_BEGIN);


    dwNumberReaded = 0;
    bRet = ReadFile(m_hFile, &m_ntHeader, sizeof(IMAGE_NT_HEADERS32), &dwNumberReaded, 0);
    if (bRet == FALSE)
        return false;

    if (LOWORD(m_ntHeader.Signature) != 0x4550) // 17744
    {
        memset(&m_ntHeader, 0, sizeof(IMAGE_NT_HEADERS32));
        return false;    // ����'pe' ����
    }

    /*
    // IMAGE_FILE_HEADER �ṹ
        m_ntHeader.FileHeader.Machine                // ���ļ�������ƽ̨, ��x86��x64����I64�ȵ�, IMAGE_FILE_MACHINE_ ��ͷ����
        m_ntHeader.FileHeader.NumberOfSections        // ��ִ���ļ�������, ������������Щֵ��λ���
        m_ntHeader.FileHeader.TimeDateStamp            // PE�ļ��Ĵ���ʱ��, һ������������д��
        m_ntHeader.FileHeader.PointerToSymbolTable    // COFF�ļ����ű����ļ��е�ƫ�ơ�
        m_ntHeader.FileHeader.NumberOfSymbols        // ���ű��������
        m_ntHeader.FileHeader.SizeOfOptionalHeader    // �������Ŀ�ѡͷ�Ĵ�С��
        m_ntHeader.FileHeader.Characteristics        // ��ִ���ļ�������, ������IMAGE_FILE_ ��ͷ���� ��Щֵ��λ���

    */

    /*
        IMAGE_OPTIONAL_HEADER32 �ṹ
        m_ntHeader.OptionalHeader.Magic        // ��ʾ��ѡͷ������ IMAGE_NT_OPTIONAL_HDR32_MAGIC 32λPE��ѡͷ IMAGE_NT_OPTIONAL_HDR64_MAGIC 64λPE��ѡͷ
        m_ntHeader.OptionalHeader.MajorLinkerVersion        // �������İ汾��
        m_ntHeader.OptionalHeader.MinorLinkerVersion        // �������İ汾��
        m_ntHeader.OptionalHeader.SizeOfCode                // ����εĳ���, ����ж�������, ���Ǵ���γ��ȵ��ܺ�
        m_ntHeader.OptionalHeader.SizeOfInitializedData        // ��ʼ�������ݳ���
        m_ntHeader.OptionalHeader.SizeOfUninitializedData    // δ��ʼ�������ݳ���
        m_ntHeader.OptionalHeader.AddressOfEntryPoint        // ������ڵ�RVA, ����exe�����ַ�������ΪWinMain��RVA������DLL, �����ַ�������ΪDllMain��RVA, 
                                                                �������������, �������ΪDriverEntry��RVA��
                                                                ��Ȼ, ʵ������ڵ㲢����WinMain, DllMain��DriverEntry, 
                                                                ����Щ����֮ǰ����һϵ�г�ʼ��Ҫ���
        m_ntHeader.OptionalHeader.BaseOfCode                // �������ʼ��ַ��RVA
        m_ntHeader.OptionalHeader.BaseOfData                // ���ݶ���ʼ��ַ��RVA
        m_ntHeader.OptionalHeader.ImageBase                    // ӳ�󣨼��ص��ڴ��е�PE�ļ����Ļ���ַ, �������ַ�ǽ���, ����DLL��˵, ����޷����ص������ַ, ϵͳ���Զ�Ϊ��ѡ���ַ
        m_ntHeader.OptionalHeader.SectionAlignment            // �ڶ���, PE�еĽڱ����ص��ڴ�ʱ�ᰴ�������ָ����ֵ������, �������ֵ��0x1000, ��ôÿ���ڵ���ʼ��ַ�ĵ�12λ��Ϊ0
        m_ntHeader.OptionalHeader.FileAlignment                // �����ļ��а���ֵ����, SectionAlignment������ڻ����FileAlignment
        m_ntHeader.OptionalHeader.MajorOperatingSystemVersion    // �������ϵͳ�İ汾��, ���Ų���ϵͳ�汾Խ��Խ��, �����������ô��Ҫ��
        m_ntHeader.OptionalHeader.MinorOperatingSystemVersion    // �������ϵͳ�İ汾��, ���Ų���ϵͳ�汾Խ��Խ��, �����������ô��Ҫ��
        m_ntHeader.OptionalHeader.MajorImageVersion            // ӳ��İ汾��, ����ǿ������Լ�ָ����, ����������д
        m_ntHeader.OptionalHeader.MinorImageVersion            // ӳ��İ汾��, ����ǿ������Լ�ָ����, ����������д
        m_ntHeader.OptionalHeader.MajorSubsystemVersion        // ������ϵͳ�汾��
        m_ntHeader.OptionalHeader.MinorSubsystemVersion        // ������ϵͳ�汾��
        m_ntHeader.OptionalHeader.Win32VersionValue            // ����, ����Ϊ0
        m_ntHeader.OptionalHeader.SizeOfImage                // ӳ��Ĵ�С, PE�ļ����ص��ڴ��пռ���������, ���ֵָ��ռ������ռ�Ĵ�С
        m_ntHeader.OptionalHeader.SizeOfHeaders                // �����ļ�ͷ�������ڱ��Ĵ�С, ���ֵ����FileAlignment�����
        m_ntHeader.OptionalHeader.CheckSum                    // ӳ���ļ���У���
        m_ntHeader.OptionalHeader.Subsystem                    // ���и�PE�ļ��������ϵͳ, ������IMAGE_SUBSYSTEM_ ��ͷ�����е�ĳһ��
        m_ntHeader.OptionalHeader.DllCharacteristics        // DLL���ļ�����, ֻ��DLL�ļ���Ч, ������IMAGE_DLLCHARACTERISTICS_��ͷ������ĳЩ�����
        m_ntHeader.OptionalHeader.SizeOfStackReserve        // ����ʱΪÿ���߳�ջ�����ڴ�Ĵ�С
        m_ntHeader.OptionalHeader.SizeOfStackCommit            // ����ʱÿ���߳�ջ��ʼռ���ڴ��С
        m_ntHeader.OptionalHeader.SizeOfHeapReserve            // ����ʱΪ���̶ѱ����ڴ��С
        m_ntHeader.OptionalHeader.SizeOfHeapCommit            // ����ʱ���̶ѳ�ʼռ���ڴ��С
        m_ntHeader.OptionalHeader.LoaderFlags                // ����, ����Ϊ0
        m_ntHeader.OptionalHeader.NumberOfRvaAndSizes        // ����Ŀ¼������, ������������������
        m_ntHeader.OptionalHeader.DataDirectory                // ����Ŀ¼, ����һ������, ������������
            {
                m_ntHeader.OptionalHeader.DataDirectory.VirtualAddress    // RVA
                m_ntHeader.OptionalHeader.DataDirectory.Size            // ��С

                ������������ʹ�� IMAGE_DIRECTORY_ENTRY_ ��ͷ�ĳ���, ÿ��������Ӧ��ͬ������
                DataDirectory�Ǹ�����, �����е�ÿһ���Ӧһ���ض������ݽṹ, ���������, ������ȵ�, 
                ���ݲ�ͬ������ȡ�������ǲ�ͬ�Ľṹ, ͷ�ļ��ﶨ��������ʾ�ĸ��ṹ, ������Ĵ�����ʾ
                ��������е�ÿһ���һ���ض��Ľṹ, ͨ��������ȡ�����е��������RtlImageDirectoryEntryToData����, DataDirectory�е�ÿһ����������������ȡ
            }

    */
    return true;
}

WORD PEHeaderFromFile::GetSectionCount() const
{
    return m_ntHeader.FileHeader.NumberOfSections;
}

bool PEHeaderFromFile::GetSectionHeader(int index, IMAGE_SECTION_HEADER* sectionHeader) const
{
    if (index > GetSectionCount())
    {
        sectionHeader = 0;
        return false;
    }

    int nextPos = (int)GetSectionHeaderPostion();
    if (index > 0)
    {
        nextPos = nextPos + sizeof(IMAGE_SECTION_HEADER) * index;
    }

    DWORD dwReaded = 0;
    SetFilePointer(m_hFile, nextPos, 0, FILE_BEGIN);
    BOOL bRet = ReadFile(m_hFile, (LPVOID)sectionHeader, sizeof(IMAGE_SECTION_HEADER), &dwReaded, 0);

    return bRet == TRUE;
}

int PEHeaderFromFile::GetNTHeaderPostion() const
{
    return m_dosHeader.e_lfanew;
}

int PEHeaderFromFile::GetSectionHeaderPostion() const
{
    return (int)GetNTHeaderPostion() + sizeof(IMAGE_NT_HEADERS32);
}

PEOperation::PEOperation()
{
    m_pData = 0;
    m_size = 0;
}

PEOperation::PEOperation(LPCWSTR fileName)
{
    m_pData = 0;
    m_size = 0;
    open(fileName);
}

PEOperation::PEOperation(LPBYTE pData, int size)
{
    m_pData = 0;
    m_size = 0;
    open(pData, size);
}

PEOperation::~PEOperation()
{
    if (m_pData)
    {
        delete[] m_pData;
        m_pData = 0;
    }
}

BOOL PEOperation::open(LPCWSTR fileName)
{
    if (!PathFileExistsW(fileName))
    {
        // �ļ�������
        return FALSE;
    }
    DWORD dwShareMode = (FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);
    DWORD dwDesiredAccess = GENERIC_READ;
    DWORD dwCreationDisposition = OPEN_EXISTING;
    HANDLE hFile = CreateFileW(fileName, dwDesiredAccess, dwShareMode, 0, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
    int err = 0;
    if (hFile == INVALID_HANDLE_VALUE)
    {
        err = GetLastError();
        // ���ļ�ʧ��
        return FALSE;
    }
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    int size = GetFileSize(hFile, NULL);
    if (INVALID_FILE_SIZE == size)
    {
        err = GetLastError();
        // ��ȡ�ļ���Сʧ��
        CloseHandle(hFile);
        return FALSE;
    }
    LPBYTE pData = new BYTE[size];
    memset(pData, 0, size);
    DWORD dwNumOfByteRead = 0;
    if (!ReadFile(hFile, pData, size, &dwNumOfByteRead, 0))
    {
        err = GetLastError();
        // ��ȡ�ļ�����ʧ��
        CloseHandle(hFile);
        return FALSE;
    }
    if (m_pData)
    {
        delete[] m_pData;
        m_pData = 0;
    }
    m_pData = pData;
    m_size = size;
    CloseHandle(hFile);
    return TRUE;
}

BOOL PEOperation::open(LPBYTE pData, int size)
{
    if (!(pData && size > 0x1000))    // �ļ��ߴ������� 0x1000
    {
        // �ļ����ݲ���ȷ
        return FALSE;
    }
    LPBYTE p = new BYTE[size];
    memset(p, 0, size);
    memcpy(p, pData, size);
    if (m_pData)
    {
        delete[] m_pData;
        m_pData = 0;
    }
    m_pData = p;
    m_size = size;
    return TRUE;
}


WORD PEOperation::GetSectionCount() const
{
    if (!m_pData)
    {
        // δ��ʼ��
        return FALSE;
    }
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);
    return nt->FileHeader.NumberOfSections;
}

PIMAGE_SECTION_HEADER PEOperation::GetSectionHeader(int index) const
{
    if (!m_pData)
    {
        // δ��ʼ��
        return 0;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);

    if (index < 0 || index > nt->FileHeader.NumberOfSections)
    {
        // ����Խ��
        return 0;
    }
    // nt �ṹ����һ���ǽ�����
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(nt + 1);
    return pSection + index;
}

PIMAGE_SECTION_HEADER PEOperation::GetSectionHeader(LPCSTR name) const
{
    if (!m_pData)
    {
        // δ��ʼ��
        return 0;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);
    // nt �ṹ����һ���ǽ�����
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(nt + 1);
    int i;
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++)
    {
        if (strcmp((LPSTR)pSection->Name, name) == 0)
        {
            return pSection;
        }
        pSection++;
    }
    return 0;
}

PIMAGE_NT_HEADERS PEOperation::GetNTHeaderPostion() const
{
    if (!m_pData)
    {
        // δ��ʼ��
        return FALSE;
    }
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);
    return nt;
}

BOOL PEOperation::AddSectionData(char name[IMAGE_SIZEOF_SHORT_NAME], const LPVOID data, int size)
{
    if (!m_pData)
    {
        // δ��ʼ��
        return 0;
    }
    if (name[0] == 0 || !data || size <= 0)return FALSE;    // ���ֲ���Ϊ��,���ݲ���Ϊ��,�ߴ粻��Ϊ��

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);
    PIMAGE_SECTION_HEADER pMySectionHeader = 0;
    // nt �ṹ����һ���ǽ�����
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(ntHeader + 1);
    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
    {
        if (strcmp((LPSTR)pSection->Name, name) == 0)
        {
            pMySectionHeader = pSection;
            break;
        }
        pSection++;
    }

    // ѭ������� pSection ָ�����һ�����ε���һ��
    if (pMySectionHeader == 0)
    {
        // ����0��ʾԭ��û�������
        ntHeader->FileHeader.NumberOfSections += 1;            // ���Ӷ�����
        pMySectionHeader = pSection;
    }
    pSection--;    // ���һ������

    memcpy(pMySectionHeader->Name, name, strlen(name));
    DWORD RawSize = ntHeader->OptionalHeader.SectionAlignment * GetOne((double)size / (double)ntHeader->OptionalHeader.SectionAlignment);
    double n = GetOne((double)pSection->Misc.VirtualSize / (double)ntHeader->OptionalHeader.SectionAlignment);
    n *= ntHeader->OptionalHeader.SectionAlignment;

    int newSize = m_size;    // ���ļ��ߴ� = ���ļ��ߴ� + �����������ݳߴ�, ����ͷ�������ӳߴ�
    if (RawSize != pMySectionHeader->SizeOfRawData)    // ԭ���Ķγ���, ����Ⱦ������ܴ�С
    {
        int tmpNum = RawSize - pMySectionHeader->SizeOfRawData;
        if (tmpNum != 0)    // ������0 ��ʾԭ��û����,����ԭ�������ݱ�����С, ���ֵ��4096�ı���
        {
            tmpNum = GetOne((double)tmpNum / (double)ntHeader->OptionalHeader.SectionAlignment);    // �³ߴ� - �ɳߴ�
            tmpNum *= ntHeader->OptionalHeader.SectionAlignment;
            ntHeader->OptionalHeader.SizeOfImage += tmpNum;    // �����ܴ�С
            newSize += tmpNum;
        }
    }

    pMySectionHeader->VirtualAddress = pSection->VirtualAddress + (DWORD)n;    // �öεĵ�һ���ֽڵĵ�ַ�������ص��ڴ���ʱ��
    pMySectionHeader->Misc.VirtualSize = size;        // ���ص��ڴ��еĽڵ��ܴ�С�����ֽ�Ϊ��λ��
    pMySectionHeader->SizeOfRawData = RawSize;        // �������ѳ�ʼ�����ݵĴ�С, ���ֽ�Ϊ��λ����ֵ������IMAGE_OPTIONAL_HEADER�ṹ��FileAlignment��Ա �ı���
    pMySectionHeader->PointerToRawData = pSection->PointerToRawData + pSection->SizeOfRawData;    // ָ��COFF�ļ��е�һҳ���ļ�ָ�롣��ֵ������IMAGE_OPTIONAL_HEADER�ṹ��FileAlignment��Ա �ı���
    pMySectionHeader->PointerToRelocations = 0;
    pMySectionHeader->PointerToLinenumbers = 0;
    pMySectionHeader->NumberOfRelocations = 0;
    pMySectionHeader->NumberOfLinenumbers = 0;
    pMySectionHeader->Characteristics = IMAGE_SCN_MEM_READ;

    
    

    if (newSize != m_size)    // ������ԭʼ���ȱ�ʾ��������κ�����������
    {
        LPBYTE newData = new BYTE[newSize];
        memset(newData, 0, newSize);

        memcpy(newData, m_pData, m_size);    // �Ȱ�ԭ���ݿ�������������
        memcpy(newData + m_size, data, size);// �ڰ��������������ݼ���
        delete[] m_pData;
        m_pData = newData;
        m_size = newSize;
    }
    else
    {
        // ����û����ԭʼ����, ���¼���etb�����ݵ�ַ, ����д������
        LPBYTE pWrite = m_pData + pMySectionHeader->PointerToRawData;
        memcpy(pWrite, data, size);
    }
    return TRUE;
}

BOOL PEOperation::DeleteSction(LPCSTR name)
{
    if (!m_pData)
    {
        // δ��ʼ��
        return 0;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);

    BOOL bRet = 0;
    // nt �ṹ����һ������������
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(ntHeader + 1);
    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
    {
        if (!bRet && strcmp((LPSTR)pSection->Name, name) == 0)
        {
            bRet = TRUE;
        }

        // bRet = TRUE��ʱ��, �����ÿ���ζ���Ҫ���¼����ַƫ��ʲô��
        // i = ��ǰ������,�����ǰ��ɾ���Ķ������һ����,�ǾͲ���Ҫ���������ε�ƫ��
        if (bRet && i < ntHeader->FileHeader.NumberOfSections)
        {
            DWORD newSize = m_size - pSection->SizeOfRawData;    // �³ߴ� = �ܳߴ� - ��ɾ���εĳߴ�
            LPBYTE newData = new BYTE[newSize];
            memset(newData, 0, newSize);

            memcpy(newData, m_pData, pSection->PointerToRawData);    // ���ļ���ʼ��ַ��ʼ����������ε�������ʼ��ַ
            if (pSection->PointerToRawData != newSize)    // �����ɾ���Ķβ������һ����, ���ڿ������������
            {
                PIMAGE_SECTION_HEADER nexSection = pSection + 1;
                memcpy(newData + pSection->PointerToRawData,    // ������ ������ + ǰ���Ѿ����������ݳ���
                    m_pData + nexSection->PointerToRawData,        // �ӱ�ɾ���ε���һ���ε���ʼ��ַ��ʼ����
                    m_size - nexSection->PointerToRawData);        // �����Ĵ�С���ܳߴ� - ��һ���ε���ʼ��ַ
            }

            PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)newData;        // ���濽����ʱ���ԭ���Ķ�Ҳ����������
            PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(newData + dos->e_lfanew);// ��������¼���һ�������ݵĶε�ַ
            PIMAGE_SECTION_HEADER p = (PIMAGE_SECTION_HEADER)(nt + 1);
            p = p + i;
            pSection = p;
            nt->FileHeader.NumberOfSections -= 1;            // ���Ӷ�����
            nt->OptionalHeader.SizeOfImage -= pSection->SizeOfRawData;    // �����ܴ�С
            memset(p, 0, sizeof(IMAGE_SECTION_HEADER));    // �������ε�����
            delete[] m_pData;
            m_pData = newData;
            m_size = newSize;
            break;
        }
        else
        {
            // ����ɾ���������һ����
        }
        pSection++;
    }

    return bRet;
}

BOOL PEOperation::GetSectionData(LPCSTR name, LPVOID data, int* size)
{
    if (!m_pData)
    {
        // δ��ʼ��
        return 0;
    }
    if (name[0] == 0 || !size)return FALSE;    // ���ֲ���Ϊ��,���ݲ���Ϊ��,�ߴ粻��Ϊ��

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);

    BOOL bRet = 0;
    // nt �ṹ����һ������������
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(ntHeader + 1);
    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
    {
        if (strcmp((LPSTR)pSection->Name, name) == 0)
        {
            bRet = TRUE;
            break;
        }
        pSection++;
    }
    if (!bRet)
    {
        // δ�ҵ�������
        return 0;
    }
    int dataLen = pSection->Misc.VirtualSize;
    LPBYTE p = m_pData + pSection->PointerToRawData;
    // Ҫ��ȡ�ĳߴ���������ݴ���������������ݵĴ�С
    if (data == 0)
    {
        *size = dataLen;
        return TRUE;
    }

    if (*size < dataLen)
        dataLen = *size;
    else
        *size = dataLen;

    memcpy(data, p, dataLen);
    return TRUE;

}

BOOL PEOperation::GetSectionData(UINT index, LPBYTE* data, int* size)
{
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(ntHeader + 1);
    if (index < 0 || index >= ntHeader->FileHeader.NumberOfSections)
    {
        return FALSE;
    }
    LPBYTE p = m_pData + pSection->PointerToRawData;
    int dataLen = pSection->Misc.VirtualSize;
    if (data == 0)
    {
        *size = dataLen;
        return TRUE;
    }

    // Ҫ��ȡ�ĳߴ���������ݴ���������������ݵĴ�С
    if (*size < dataLen)
        dataLen = *size;
    else
        *size = dataLen;

    memcpy(data, p, dataLen);
    return TRUE;
}

LPBYTE PEOperation::GetData(int* size)
{
    if (size)
        *size = m_size;
    return m_pData;
}
