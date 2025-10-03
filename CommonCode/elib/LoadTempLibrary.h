#include <tstr.h>
#include <assist/CFileRW.h>
#include <CStringBuffer.h>
#include <version_winos.h>
#include <CArray.h>
#include <assist/assist.h>
#include <time.h>

// 加载临时目录dll类, 支持库使用, 功能写在dll里, 支持库加载dll, 这个文件是vc6使用, 需要兼容vc6
typedef LPCVOID(__cdecl* PFN_GetDllData)( int* size );
class CLoadTempLibrary
{
#define __ADD_L____1(s) L ## s
#define __ADD_L____2(s) __ADD_L____1(s)
#define __FILE__W__ __ADD_L____2(__FILE__)

#if _MSC_VER < 1201 && defined(__E_STATIC_LIB) && 0  // 是VC6的静态库, 那就弹信息框提示, 后面的1改成0就不弹信息框了
#define VC6MSGBOX tstr_MessageBox
#else
#define VC6MSGBOX 
#endif

private:
    HMODULE         hModule;        // 加载的模块句柄
    wstr            dllFile;        // 模块路径, 析构时会删除这个文件, 只删除, 不管是否删除成功
    PFN_GetDllData  pfn_GetDllData; // 获取dll数据的函数
    LPCWSTR         dllName;        // 写出的dll名字, 临时目录 + dll名字
public:
    CLoadTempLibrary() :hModule(0), pfn_GetDllData(0), dllName(0) { ; }
    void init(PFN_GetDllData pfn, LPCWSTR name)
    {
        dllName = name;
        pfn_GetDllData = pfn;
        if ( !dllName || !dllName[0] || !pfn_GetDllData )
            throw;  // 不支持传递空名字和空函数
    }
    inline LPCWSTR GetDllFile()
    {
        return dllFile.c_str();
    }
    ~CLoadTempLibrary()
    {
        if ( !hModule )return;
        FreeLibrary(hModule);
        hModule = 0;
        DeleteFileW(dllFile.c_str());   // 删除文件, 不管删除是否成功, 其他文件如果有使用, 那会删除失败
    }
    
    
    
    inline HMODULE GetModule(LPCVOID* pDll = 0, int* pSize = 0)
    {
        if (hModule)return hModule;
        dllFile.resize(260);
        GetTempPathW(260, dllFile.data());
        dllFile.resize_strlen();
        dllFile.append(dllName);

        bool reLoad = true;
        int size = 0;
        LPCVOID dll = pfn_GetDllData(&size);
        if ( pDll  ) *pDll = dll;
        if ( pSize ) *pSize = size;
        CFileRW file;

        VC6MSGBOX(0, L"VC6静态库调试加载dll", MB_OK, L"开始调用判断文件是否存在, 路径 = %s", dllFile.c_str());
        if ( fnPathFileExistsW(dllFile.c_str()) )
        {
            //TODO 这里需要判断临时目录的DLL是否能使用
            VC6MSGBOX(0, L"VC6静态库调试加载dll", MB_OK, L"路径存在, 开始打开文件");
            if ( file.open(dllFile.c_str(), 1, -1) )
            {
                int oldSize = file.size();
                VC6MSGBOX(0, L"VC6静态库调试加载dll", MB_OK, L"打开文件成功, 文件尺寸 = %d, 程序内文件尺寸: %d", oldSize, size);
                if ( oldSize > 0 && oldSize == size)
                {
                    LPBYTE pOldData = new BYTE[oldSize];
                    DWORD dwNumOfByteRead = 0;
                    // 文件尺寸一样, 那就读取进来对比两个文件是否一样
                    if ( file.read(pOldData, oldSize) )
                    {
                        VC6MSGBOX(0, L"VC6静态库调试加载dll", MB_OK, L"读入文件成功, 开始比对MD5", oldSize);
                        if ( memcmp(pOldData, dll, oldSize) == 0 )
                            reLoad = false; // 一样就不重新加载了
                    }
                    delete[] pOldData;
                }
                file.reset();
            }
            // 否则条件这里不写, 如果打开文件失败, 那下面会创建一个随机名字, 再次写出

        }

        VC6MSGBOX(0, L"VC6静态库调试加载dll", MB_OK, L"reLoad = %d", reLoad);
        if ( reLoad )
        {
            bool isOpen = file.open(dllFile.c_str(), 4, -1);
            VC6MSGBOX(0, L"VC6静态库调试加载dll", MB_OK, L"重新加载文件, 读取结果 = %d", isOpen);
            if ( !isOpen )
            {
                srand((unsigned int)time(0));
                dllFile.append(wstr::randStr(20, 6));
                isOpen = file.open(dllFile.c_str(), 4); // 打开失败, 弄一个新的随机文件名, 然后再次打开
                VC6MSGBOX(0, L"VC6静态库调试加载dll", MB_OK, L"重新加载文件, 新文件名 = %s", dllFile.c_str());
            }

            // 利用 或 条件的特性, 第一个条件成立就不会走第二个条件, 打开失败就不会执行写入, 打开成功就判断写入是否成功
            if ( !isOpen || !file.write((LPBYTE)dll, size) )
            {
                // 都失败, 那就不继续运行了
                int err = GetLastError();
                wchar_t str[260];
                const wchar_t* const fmtStr = L"没有临时目录读写权限, 写出文件失败\n运行本程序需要临时目录读写权限, 请尝试以管理员权限打开本程序\n错误码 = %d\n写出文件: %s";
#if _MSC_VER < 1201
                swprintf(str, fmtStr, err, dllFile.c_str());
#else
                swprintf_s(str, fmtStr, err, dllFile.c_str());
#endif
                MessageBoxW(0, str, L"程序出现致命错误", 0);
                TerminateProcess(GetCurrentProcess(), 0);
                exit(0);
                return 0;
            }

            file.reset();
        }
        hModule = LoadLibraryW(dllFile.c_str());
        VC6MSGBOX(0, L"VC6静态库调试加载dll", MB_OK, L"hModule = LoadLibraryW(dllFile.c_str()); = %p", hModule);
        return hModule;
    }


    // 从动态库里获取函数地址
    inline void* GetAddrProc(LPCSTR name)
    {
        if (!GetModule()) return 0;
        return GetProcAddress(hModule, name);
    }

private:

    inline static BOOL _GetMd5(LPCVOID pData, int size, wchar_t MD5[33])
    {
        MD5[0] = 0;
        if ( !pData || !size ) return false;
        HCRYPTPROV hProv = NULL;
        HCRYPTPROV hHash = NULL;
        BOOL retVl = false;

        do
        {
            if ( !CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) )
                break;       //获得CSP中一个密钥容器的句柄

            //初始化对数据流的hash, 创建并返回一个与CSP的hash对象相关的句柄。这个句柄接下来将被    CryptHashData调用。

            if ( !CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) )
                break;

            if ( !CryptHashData(hHash, (LPBYTE)pData, size, 0) )
                break;      //hash文件  


            unsigned char* pbHash;
            DWORD dwHashLen = sizeof(DWORD);
            if ( !CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0) )
                break; //我也不知道为什么要先这样调用CryptGetHashParam, 这块是参照的msdn       

            pbHash = (unsigned char*)malloc(dwHashLen);
            if ( !pbHash )
                break;
            
            retVl = CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0);    //获得md5值 
            if ( retVl )
            {
                if ( dwHashLen >= 16 )
                {
                    memcpy(MD5, pbHash, 16);
                }
                else
                {
                    memcpy(MD5, pbHash, dwHashLen);
                    for ( DWORD i = dwHashLen; i < 16; i++ )
                        MD5[i] = 0;
                }
                MD5[16] = 0;
            }
            free(pbHash);
        } while ( false );

        //善后工作
        if ( hHash )
            CryptDestroyHash(hHash);
        if ( hProv )
            CryptReleaseContext(hProv, 0);

        return retVl;
    }
    
    inline bool check_dos_nt(LPBYTE pData)
    {
        if ( !pData ) return false;
        LPBYTE base = pData;
        PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pData;
        if ( dos->e_magic != IMAGE_DOS_SIGNATURE )
            return false;
        PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)( base + dos->e_lfanew );
        if ( nt->Signature != IMAGE_NT_SIGNATURE || ( nt->FileHeader.Machine & IMAGE_FILE_MACHINE_I386 ) == 0 )
            return false;
        return true;
    }

    inline LPBYTE MmMapFile(LPCVOID lpData, _str& dllData)
    {
        LPBYTE base = (LPBYTE)lpData;
        if ( !check_dos_nt(base) )
            return 0;

        PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
        PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)( base + dos->e_lfanew );

        DWORD dwSizeOfHeaders = nt->OptionalHeader.SizeOfHeaders;   // 获取所有头部+区段表的大小
        WORD wNumberOfSections = nt->FileHeader.NumberOfSections;   // 获取区段数量
        PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(nt);   // 获取区段表数组的首元素
        dllData.resize(nt->OptionalHeader.SizeOfImage);
        LPBYTE lpBaseAddress = (LPBYTE)dllData.data();


        memcpy(lpBaseAddress, lpData, dwSizeOfHeaders); // 将头部, 包括区段表, 拷贝到内存
        LPVOID lpSrcMem = NULL;
        LPVOID lpDestMem = NULL;

        //循环加载所有区段
        for ( WORD i = 0; i < wNumberOfSections; i++ )
        {
            //过滤掉无效区段
            if ( 0 == pSection->VirtualAddress || 0 == pSection->SizeOfRawData )
            {
                pSection++;
                continue;
            }

            lpSrcMem = (LPVOID)( (DWORD)lpData + pSection->PointerToRawData );          // 获取区段在文件中的位置
            lpDestMem = (LPVOID)( (DWORD)lpBaseAddress + pSection->VirtualAddress );    // 获取区段映射到内存中的位置

            memcpy(lpDestMem, lpSrcMem, pSection->SizeOfRawData);   // 将区段数据拷贝到内存中

            pSection++; // 获取下一个区段头
        }
        return lpBaseAddress;
    }

public:
    inline void chech_import(LPBYTE pData, int size, int err, _str& dbgText, LPCWSTR dllFile)
    {
        _str dllData;
        CStringBufferA buf;
        buf.reserve(0x1000);
        dbgText.reserve(0x1000);
        dbgText.clear();

        winos::VERSION_STRUCT ver = { 0 };
        winos::WIN_VER_DETAIL type = winos::GetWindowsVersion(&ver);
        if ( type != winos::WINDOWS_VERSION_NONE )
        {
            LPCSTR fmt = "加载\"%s\"失败\r\n错误码 = %d\r\n"
                "系统版本: MajorVersion = %d, MinorVersion = %d, BuildNumber = %d, ProductType = %d\r\n"
                "操作系统名称为: %s\r\n详细失败信息如下:\r\n\r\n";
            LPCSTR pszVersionA = 0;
            LPCWSTR pszVersionW = winos::GetWindowsVersionString(type, &pszVersionA);
            if ( !dllFile || !dllFile[0] )
            {
                fmt = "加载DLL失败\r\n"
                    "系统版本: MajorVersion = %d, MinorVersion = %d, BuildNumber = %d, ProductType = %d\r\n"
                    "操作系统名称为: %s\r\n详细失败信息如下:\r\n\r\n";
                dbgText.AppendFormat(fmt, ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber, ver.wProductType, pszVersionA);
            }
            else
            {
                dbgText.AppendFormat(fmt, wstr::W2A(dllFile).c_str(), err,
                                     ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber, ver.wProductType, pszVersionA);
            }
            
        }
        else if ( dllFile )
        {
            dbgText.AppendFormat("加载\"%s\"失败\r\n错误码 = %d, 详细失败信息如下:\r\n\r\n",
                wstr::W2A(dllFile).c_str(), err);
        }

        dbgText.append("该支持库不支持此系统, 缺少以下dll或者dll中的函数\r\n");
        LPBYTE base = MmMapFile(pData, dllData);
        if ( !base )
            return;

        PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
        PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)( base + dos->e_lfanew );
        PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)( base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress );
        bool isFree = false;
        CArray<LPCSTR> arr(true, 100);
        arr.resize(0);

        while ( pImport->Name )
        {
            LPCSTR pDllName = (LPCSTR)( base + pImport->Name );
            if ( *pDllName == 0 )
            {
                pImport++;
                continue;
            }
            isFree = false;
            HMODULE hModule = GetModuleHandleA(pDllName);
            if ( !hModule )
            {
                hModule = LoadLibraryA(pDllName);
                isFree = hModule != 0;
            }


            arr.resize(0);
            PIMAGE_THUNK_DATA pIat = (PIMAGE_THUNK_DATA)( base + pImport->FirstThunk );
            //IAT表个数不确定，最后一项以全为0结尾
            while ( pIat->u1.Ordinal )
            {
                // 判断是否名称导出还是序号导出
                // 通过最高位是否为1, 如果为1, 那么序号导出
                // 如果为0, 名称导出
                LPCSTR pszFunName = 0;
                if ( pIat->u1.Ordinal & 0x80000000 )
                {
                    // 最高位为1, 序号导入
                    pszFunName = (LPCSTR)( pIat->u1.Ordinal & 0xFFFF );
                }
                else
                {
                    //最高位位0, 名称导入
                    //显示序号, 和名称
                    PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)( base + pIat->u1.AddressOfData );
                    pszFunName = (LPCSTR)&pName->Name[0];
                }

                if ( !hModule || GetProcAddress(hModule, pszFunName) == 0 )
                {
                    // 获取这个函数失败, 缺少这个函数, 缺少的才加入到数组里
                    const LPCSTR fmt = ( (DWORD)pszFunName <= 0xffff ? "函数索引: %d" : "%s" );
                    arr.push_back(buf.AddFormat(fmt, pszFunName));
                }

                pIat++;
            }

            int count = arr.size();
            if ( count > 0 )
            {
                if ( !hModule )
                {
                    dbgText.append("缺少dll: ").append(pDllName).append("\r\n");
                }
                else
                {
                    dbgText.append("缺少 ").append(pDllName).append(" 中的以下函数:\r\n");

                    for ( int i = 0; i < count; i++ )
                    {
                        dbgText.append("        ").append(arr[i]).append("\r\n");
                    }
                }
                dbgText.append("\r\n");
                arr.resize(0);
            }

            pImport++;
        }

    }

};

