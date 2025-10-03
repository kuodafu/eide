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

    //移动这么多位置, 得到文件头开始偏移
    SetFilePointer(m_hFile, m_dosHeader.e_lfanew, 0, FILE_BEGIN);


    dwNumberReaded = 0;
    bRet = ReadFile(m_hFile, &m_ntHeader, sizeof(IMAGE_NT_HEADERS32), &dwNumberReaded, 0);
    if (bRet == FALSE)
        return false;

    if (LOWORD(m_ntHeader.Signature) != 0x4550) // 17744
    {
        memset(&m_ntHeader, 0, sizeof(IMAGE_NT_HEADERS32));
        return false;    // 不是'pe' 返回
    }

    /*
    // IMAGE_FILE_HEADER 结构
        m_ntHeader.FileHeader.Machine                // 该文件的运行平台, 是x86、x64还是I64等等, IMAGE_FILE_MACHINE_ 开头常量
        m_ntHeader.FileHeader.NumberOfSections        // 可执行文件的属性, 可以是下面这些值按位相或。
        m_ntHeader.FileHeader.TimeDateStamp            // PE文件的创建时间, 一般有连接器填写。
        m_ntHeader.FileHeader.PointerToSymbolTable    // COFF文件符号表在文件中的偏移。
        m_ntHeader.FileHeader.NumberOfSymbols        // 符号表的数量。
        m_ntHeader.FileHeader.SizeOfOptionalHeader    // 紧随其后的可选头的大小。
        m_ntHeader.FileHeader.Characteristics        // 可执行文件的属性, 可以是IMAGE_FILE_ 开头常量 这些值按位相或。

    */

    /*
        IMAGE_OPTIONAL_HEADER32 结构
        m_ntHeader.OptionalHeader.Magic        // 表示可选头的类型 IMAGE_NT_OPTIONAL_HDR32_MAGIC 32位PE可选头 IMAGE_NT_OPTIONAL_HDR64_MAGIC 64位PE可选头
        m_ntHeader.OptionalHeader.MajorLinkerVersion        // 链接器的版本号
        m_ntHeader.OptionalHeader.MinorLinkerVersion        // 链接器的版本号
        m_ntHeader.OptionalHeader.SizeOfCode                // 代码段的长度, 如果有多个代码段, 则是代码段长度的总和
        m_ntHeader.OptionalHeader.SizeOfInitializedData        // 初始化的数据长度
        m_ntHeader.OptionalHeader.SizeOfUninitializedData    // 未初始化的数据长度
        m_ntHeader.OptionalHeader.AddressOfEntryPoint        // 程序入口的RVA, 对于exe这个地址可以理解为WinMain的RVA。对于DLL, 这个地址可以理解为DllMain的RVA, 
                                                                如果是驱动程序, 可以理解为DriverEntry的RVA。
                                                                当然, 实际上入口点并非是WinMain, DllMain和DriverEntry, 
                                                                在这些函数之前还有一系列初始化要完成
        m_ntHeader.OptionalHeader.BaseOfCode                // 代码段起始地址的RVA
        m_ntHeader.OptionalHeader.BaseOfData                // 数据段起始地址的RVA
        m_ntHeader.OptionalHeader.ImageBase                    // 映象（加载到内存中的PE文件）的基地址, 这个基地址是建议, 对于DLL来说, 如果无法加载到这个地址, 系统会自动为其选择地址
        m_ntHeader.OptionalHeader.SectionAlignment            // 节对齐, PE中的节被加载到内存时会按照这个域指定的值来对齐, 比如这个值是0x1000, 那么每个节的起始地址的低12位都为0
        m_ntHeader.OptionalHeader.FileAlignment                // 节在文件中按此值对齐, SectionAlignment必须大于或等于FileAlignment
        m_ntHeader.OptionalHeader.MajorOperatingSystemVersion    // 所需操作系统的版本号, 随着操作系统版本越来越多, 这个好像不是那么重要了
        m_ntHeader.OptionalHeader.MinorOperatingSystemVersion    // 所需操作系统的版本号, 随着操作系统版本越来越多, 这个好像不是那么重要了
        m_ntHeader.OptionalHeader.MajorImageVersion            // 映象的版本号, 这个是开发者自己指定的, 由连接器填写
        m_ntHeader.OptionalHeader.MinorImageVersion            // 映象的版本号, 这个是开发者自己指定的, 由连接器填写
        m_ntHeader.OptionalHeader.MajorSubsystemVersion        // 所需子系统版本号
        m_ntHeader.OptionalHeader.MinorSubsystemVersion        // 所需子系统版本号
        m_ntHeader.OptionalHeader.Win32VersionValue            // 保留, 必须为0
        m_ntHeader.OptionalHeader.SizeOfImage                // 映象的大小, PE文件加载到内存中空间是连续的, 这个值指定占用虚拟空间的大小
        m_ntHeader.OptionalHeader.SizeOfHeaders                // 所有文件头（包括节表）的大小, 这个值是以FileAlignment对齐的
        m_ntHeader.OptionalHeader.CheckSum                    // 映象文件的校验和
        m_ntHeader.OptionalHeader.Subsystem                    // 运行该PE文件所需的子系统, 可以是IMAGE_SUBSYSTEM_ 开头常量中的某一个
        m_ntHeader.OptionalHeader.DllCharacteristics        // DLL的文件属性, 只对DLL文件有效, 可以是IMAGE_DLLCHARACTERISTICS_开头常量中某些的组合
        m_ntHeader.OptionalHeader.SizeOfStackReserve        // 运行时为每个线程栈保留内存的大小
        m_ntHeader.OptionalHeader.SizeOfStackCommit            // 运行时每个线程栈初始占用内存大小
        m_ntHeader.OptionalHeader.SizeOfHeapReserve            // 运行时为进程堆保留内存大小
        m_ntHeader.OptionalHeader.SizeOfHeapCommit            // 运行时进程堆初始占用内存大小
        m_ntHeader.OptionalHeader.LoaderFlags                // 保留, 必须为0
        m_ntHeader.OptionalHeader.NumberOfRvaAndSizes        // 数据目录的项数, 即下面这个数组的项数
        m_ntHeader.OptionalHeader.DataDirectory                // 数据目录, 这是一个数组, 数组的项定义如下
            {
                m_ntHeader.OptionalHeader.DataDirectory.VirtualAddress    // RVA
                m_ntHeader.OptionalHeader.DataDirectory.Size            // 大小

                数组索引可以使用 IMAGE_DIRECTORY_ENTRY_ 开头的常量, 每个索引对应不同的数据
                DataDirectory是个数组, 数组中的每一项对应一个特定的数据结构, 包括导入表, 导出表等等, 
                根据不同的索引取出来的是不同的结构, 头文件里定义各个项表示哪个结构, 如下面的代码所示
                这个数组中的每一项都是一个特定的结构, 通过函数获取数组中的项可以用RtlImageDirectoryEntryToData函数, DataDirectory中的每一项都可以用这个函数获取
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
        // 文件不存在
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
        // 打开文件失败
        return FALSE;
    }
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    int size = GetFileSize(hFile, NULL);
    if (INVALID_FILE_SIZE == size)
    {
        err = GetLastError();
        // 获取文件大小失败
        CloseHandle(hFile);
        return FALSE;
    }
    LPBYTE pData = new BYTE[size];
    memset(pData, 0, size);
    DWORD dwNumOfByteRead = 0;
    if (!ReadFile(hFile, pData, size, &dwNumOfByteRead, 0))
    {
        err = GetLastError();
        // 读取文件数据失败
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
    if (!(pData && size > 0x1000))    // 文件尺寸必须大于 0x1000
    {
        // 文件数据不正确
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
        // 未初始化
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
        // 未初始化
        return 0;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);

    if (index < 0 || index > nt->FileHeader.NumberOfSections)
    {
        // 索引越界
        return 0;
    }
    // nt 结构的下一个是节数据
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(nt + 1);
    return pSection + index;
}

PIMAGE_SECTION_HEADER PEOperation::GetSectionHeader(LPCSTR name) const
{
    if (!m_pData)
    {
        // 未初始化
        return 0;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);
    // nt 结构的下一个是节数据
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
        // 未初始化
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
        // 未初始化
        return 0;
    }
    if (name[0] == 0 || !data || size <= 0)return FALSE;    // 名字不能为空,数据不能为空,尺寸不能为空

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);
    PIMAGE_SECTION_HEADER pMySectionHeader = 0;
    // nt 结构的下一个是节数据
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

    // 循环走完后 pSection 指向最后一个区段的下一个
    if (pMySectionHeader == 0)
    {
        // 等于0表示原来没有这个段
        ntHeader->FileHeader.NumberOfSections += 1;            // 增加段数量
        pMySectionHeader = pSection;
    }
    pSection--;    // 最后一个区段

    memcpy(pMySectionHeader->Name, name, strlen(name));
    DWORD RawSize = ntHeader->OptionalHeader.SectionAlignment * GetOne((double)size / (double)ntHeader->OptionalHeader.SectionAlignment);
    double n = GetOne((double)pSection->Misc.VirtualSize / (double)ntHeader->OptionalHeader.SectionAlignment);
    n *= ntHeader->OptionalHeader.SectionAlignment;

    int newSize = m_size;    // 新文件尺寸 = 旧文件尺寸 + 新增区段数据尺寸, 区段头不会增加尺寸
    if (RawSize != pMySectionHeader->SizeOfRawData)    // 原来的段长度, 不相等就增加总大小
    {
        int tmpNum = RawSize - pMySectionHeader->SizeOfRawData;
        if (tmpNum != 0)    // 不等于0 表示原来没数据,或者原来的数据比现在小, 这个值是4096的倍数
        {
            tmpNum = GetOne((double)tmpNum / (double)ntHeader->OptionalHeader.SectionAlignment);    // 新尺寸 - 旧尺寸
            tmpNum *= ntHeader->OptionalHeader.SectionAlignment;
            ntHeader->OptionalHeader.SizeOfImage += tmpNum;    // 区段总大小
            newSize += tmpNum;
        }
    }

    pMySectionHeader->VirtualAddress = pSection->VirtualAddress + (DWORD)n;    // 该段的第一个字节的地址（当加载到内存中时）
    pMySectionHeader->Misc.VirtualSize = size;        // 加载到内存中的节的总大小（以字节为单位）
    pMySectionHeader->SizeOfRawData = RawSize;        // 磁盘上已初始化数据的大小, 以字节为单位。此值必须是IMAGE_OPTIONAL_HEADER结构的FileAlignment成员 的倍数
    pMySectionHeader->PointerToRawData = pSection->PointerToRawData + pSection->SizeOfRawData;    // 指向COFF文件中第一页的文件指针。此值必须是IMAGE_OPTIONAL_HEADER结构的FileAlignment成员 的倍数
    pMySectionHeader->PointerToRelocations = 0;
    pMySectionHeader->PointerToLinenumbers = 0;
    pMySectionHeader->NumberOfRelocations = 0;
    pMySectionHeader->NumberOfLinenumbers = 0;
    pMySectionHeader->Characteristics = IMAGE_SCN_MEM_READ;

    
    

    if (newSize != m_size)    // 不等于原始长度表示增加这个段后数据增加了
    {
        LPBYTE newData = new BYTE[newSize];
        memset(newData, 0, newSize);

        memcpy(newData, m_pData, m_size);    // 先把原数据拷贝到新数据里
        memcpy(newData + m_size, data, size);// 在把新增的区段数据加入
        delete[] m_pData;
        m_pData = newData;
        m_size = newSize;
    }
    else
    {
        // 这是没增加原始长度, 重新计算etb的数据地址, 重新写入数据
        LPBYTE pWrite = m_pData + pMySectionHeader->PointerToRawData;
        memcpy(pWrite, data, size);
    }
    return TRUE;
}

BOOL PEOperation::DeleteSction(LPCSTR name)
{
    if (!m_pData)
    {
        // 未初始化
        return 0;
    }

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);

    BOOL bRet = 0;
    // nt 结构的下一个是区段数据
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(ntHeader + 1);
    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
    {
        if (!bRet && strcmp((LPSTR)pSection->Name, name) == 0)
        {
            bRet = TRUE;
        }

        // bRet = TRUE的时候, 往后的每个段都需要重新计算地址偏移什么的
        // i = 当前段索引,如果当前被删除的段是最后一个段,那就不需要计算其他段的偏移
        if (bRet && i < ntHeader->FileHeader.NumberOfSections)
        {
            DWORD newSize = m_size - pSection->SizeOfRawData;    // 新尺寸 = 总尺寸 - 被删除段的尺寸
            LPBYTE newData = new BYTE[newSize];
            memset(newData, 0, newSize);

            memcpy(newData, m_pData, pSection->PointerToRawData);    // 从文件起始地址开始拷贝到这个段的数据起始地址
            if (pSection->PointerToRawData != newSize)    // 如果被删除的段不是最后一个段, 就在拷贝后面的数据
            {
                PIMAGE_SECTION_HEADER nexSection = pSection + 1;
                memcpy(newData + pSection->PointerToRawData,    // 拷贝到 新数据 + 前面已经拷贝的数据长度
                    m_pData + nexSection->PointerToRawData,        // 从被删除段的下一个段的起始地址开始拷贝
                    m_size - nexSection->PointerToRawData);        // 拷贝的大小是总尺寸 - 下一个段的起始地址
            }

            PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)newData;        // 上面拷贝的时候把原来的段也拷贝进来了
            PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(newData + dos->e_lfanew);// 这里得重新计算一下新数据的段地址
            PIMAGE_SECTION_HEADER p = (PIMAGE_SECTION_HEADER)(nt + 1);
            p = p + i;
            pSection = p;
            nt->FileHeader.NumberOfSections -= 1;            // 增加段数量
            nt->OptionalHeader.SizeOfImage -= pSection->SizeOfRawData;    // 区段总大小
            memset(p, 0, sizeof(IMAGE_SECTION_HEADER));    // 清除这个段的数据
            delete[] m_pData;
            m_pData = newData;
            m_size = newSize;
            break;
        }
        else
        {
            // 否则被删除的是最后一个段
        }
        pSection++;
    }

    return bRet;
}

BOOL PEOperation::GetSectionData(LPCSTR name, LPVOID data, int* size)
{
    if (!m_pData)
    {
        // 未初始化
        return 0;
    }
    if (name[0] == 0 || !size)return FALSE;    // 名字不能为空,数据不能为空,尺寸不能为空

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)m_pData;
    PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(m_pData + dosHeader->e_lfanew);

    BOOL bRet = 0;
    // nt 结构的下一个是区段数据
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
        // 未找到区段名
        return 0;
    }
    int dataLen = pSection->Misc.VirtualSize;
    LPBYTE p = m_pData + pSection->PointerToRawData;
    // 要读取的尺寸比区段数据大则调整到区段数据的大小
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

    // 要读取的尺寸比区段数据大则调整到区段数据的大小
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
