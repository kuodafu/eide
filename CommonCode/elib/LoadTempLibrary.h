#include <tstr.h>
#include <assist/CFileRW.h>
#include <CStringBuffer.h>
#include <version_winos.h>
#include <CArray.h>
#include <assist/assist.h>
#include <time.h>

// ������ʱĿ¼dll��, ֧�ֿ�ʹ��, ����д��dll��, ֧�ֿ����dll, ����ļ���vc6ʹ��, ��Ҫ����vc6
typedef LPCVOID(__cdecl* PFN_GetDllData)( int* size );
class CLoadTempLibrary
{
#define __ADD_L____1(s) L ## s
#define __ADD_L____2(s) __ADD_L____1(s)
#define __FILE__W__ __ADD_L____2(__FILE__)

#if _MSC_VER < 1201 && defined(__E_STATIC_LIB) && 0  // ��VC6�ľ�̬��, �Ǿ͵���Ϣ����ʾ, �����1�ĳ�0�Ͳ�����Ϣ����
#define VC6MSGBOX tstr_MessageBox
#else
#define VC6MSGBOX 
#endif

private:
    HMODULE         hModule;        // ���ص�ģ����
    wstr            dllFile;        // ģ��·��, ����ʱ��ɾ������ļ�, ֻɾ��, �����Ƿ�ɾ���ɹ�
    PFN_GetDllData  pfn_GetDllData; // ��ȡdll���ݵĺ���
    LPCWSTR         dllName;        // д����dll����, ��ʱĿ¼ + dll����
public:
    CLoadTempLibrary() :hModule(0), pfn_GetDllData(0), dllName(0) { ; }
    void init(PFN_GetDllData pfn, LPCWSTR name)
    {
        dllName = name;
        pfn_GetDllData = pfn;
        if ( !dllName || !dllName[0] || !pfn_GetDllData )
            throw;  // ��֧�ִ��ݿ����ֺͿպ���
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
        DeleteFileW(dllFile.c_str());   // ɾ���ļ�, ����ɾ���Ƿ�ɹ�, �����ļ������ʹ��, �ǻ�ɾ��ʧ��
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

        VC6MSGBOX(0, L"VC6��̬����Լ���dll", MB_OK, L"��ʼ�����ж��ļ��Ƿ����, ·�� = %s", dllFile.c_str());
        if ( fnPathFileExistsW(dllFile.c_str()) )
        {
            //TODO ������Ҫ�ж���ʱĿ¼��DLL�Ƿ���ʹ��
            VC6MSGBOX(0, L"VC6��̬����Լ���dll", MB_OK, L"·������, ��ʼ���ļ�");
            if ( file.open(dllFile.c_str(), 1, -1) )
            {
                int oldSize = file.size();
                VC6MSGBOX(0, L"VC6��̬����Լ���dll", MB_OK, L"���ļ��ɹ�, �ļ��ߴ� = %d, �������ļ��ߴ�: %d", oldSize, size);
                if ( oldSize > 0 && oldSize == size)
                {
                    LPBYTE pOldData = new BYTE[oldSize];
                    DWORD dwNumOfByteRead = 0;
                    // �ļ��ߴ�һ��, �ǾͶ�ȡ�����Ա������ļ��Ƿ�һ��
                    if ( file.read(pOldData, oldSize) )
                    {
                        VC6MSGBOX(0, L"VC6��̬����Լ���dll", MB_OK, L"�����ļ��ɹ�, ��ʼ�ȶ�MD5", oldSize);
                        if ( memcmp(pOldData, dll, oldSize) == 0 )
                            reLoad = false; // һ���Ͳ����¼�����
                    }
                    delete[] pOldData;
                }
                file.reset();
            }
            // �����������ﲻд, ������ļ�ʧ��, ������ᴴ��һ���������, �ٴ�д��

        }

        VC6MSGBOX(0, L"VC6��̬����Լ���dll", MB_OK, L"reLoad = %d", reLoad);
        if ( reLoad )
        {
            bool isOpen = file.open(dllFile.c_str(), 4, -1);
            VC6MSGBOX(0, L"VC6��̬����Լ���dll", MB_OK, L"���¼����ļ�, ��ȡ��� = %d", isOpen);
            if ( !isOpen )
            {
                srand((unsigned int)time(0));
                dllFile.append(wstr::randStr(20, 6));
                isOpen = file.open(dllFile.c_str(), 4); // ��ʧ��, Ūһ���µ�����ļ���, Ȼ���ٴδ�
                VC6MSGBOX(0, L"VC6��̬����Լ���dll", MB_OK, L"���¼����ļ�, ���ļ��� = %s", dllFile.c_str());
            }

            // ���� �� ����������, ��һ�����������Ͳ����ߵڶ�������, ��ʧ�ܾͲ���ִ��д��, �򿪳ɹ����ж�д���Ƿ�ɹ�
            if ( !isOpen || !file.write((LPBYTE)dll, size) )
            {
                // ��ʧ��, �ǾͲ�����������
                int err = GetLastError();
                wchar_t str[260];
                const wchar_t* const fmtStr = L"û����ʱĿ¼��дȨ��, д���ļ�ʧ��\n���б�������Ҫ��ʱĿ¼��дȨ��, �볢���Թ���ԱȨ�޴򿪱�����\n������ = %d\nд���ļ�: %s";
#if _MSC_VER < 1201
                swprintf(str, fmtStr, err, dllFile.c_str());
#else
                swprintf_s(str, fmtStr, err, dllFile.c_str());
#endif
                MessageBoxW(0, str, L"���������������", 0);
                TerminateProcess(GetCurrentProcess(), 0);
                exit(0);
                return 0;
            }

            file.reset();
        }
        hModule = LoadLibraryW(dllFile.c_str());
        VC6MSGBOX(0, L"VC6��̬����Լ���dll", MB_OK, L"hModule = LoadLibraryW(dllFile.c_str()); = %p", hModule);
        return hModule;
    }


    // �Ӷ�̬�����ȡ������ַ
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
                break;       //���CSP��һ����Կ�����ľ��

            //��ʼ������������hash, ����������һ����CSP��hash������صľ��������������������    CryptHashData���á�

            if ( !CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) )
                break;

            if ( !CryptHashData(hHash, (LPBYTE)pData, size, 0) )
                break;      //hash�ļ�  


            unsigned char* pbHash;
            DWORD dwHashLen = sizeof(DWORD);
            if ( !CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0) )
                break; //��Ҳ��֪��ΪʲôҪ����������CryptGetHashParam, ����ǲ��յ�msdn       

            pbHash = (unsigned char*)malloc(dwHashLen);
            if ( !pbHash )
                break;
            
            retVl = CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0);    //���md5ֵ 
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

        //�ƺ���
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

        DWORD dwSizeOfHeaders = nt->OptionalHeader.SizeOfHeaders;   // ��ȡ����ͷ��+���α�Ĵ�С
        WORD wNumberOfSections = nt->FileHeader.NumberOfSections;   // ��ȡ��������
        PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(nt);   // ��ȡ���α��������Ԫ��
        dllData.resize(nt->OptionalHeader.SizeOfImage);
        LPBYTE lpBaseAddress = (LPBYTE)dllData.data();


        memcpy(lpBaseAddress, lpData, dwSizeOfHeaders); // ��ͷ��, �������α�, �������ڴ�
        LPVOID lpSrcMem = NULL;
        LPVOID lpDestMem = NULL;

        //ѭ��������������
        for ( WORD i = 0; i < wNumberOfSections; i++ )
        {
            //���˵���Ч����
            if ( 0 == pSection->VirtualAddress || 0 == pSection->SizeOfRawData )
            {
                pSection++;
                continue;
            }

            lpSrcMem = (LPVOID)( (DWORD)lpData + pSection->PointerToRawData );          // ��ȡ�������ļ��е�λ��
            lpDestMem = (LPVOID)( (DWORD)lpBaseAddress + pSection->VirtualAddress );    // ��ȡ����ӳ�䵽�ڴ��е�λ��

            memcpy(lpDestMem, lpSrcMem, pSection->SizeOfRawData);   // ���������ݿ������ڴ���

            pSection++; // ��ȡ��һ������ͷ
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
            LPCSTR fmt = "����\"%s\"ʧ��\r\n������ = %d\r\n"
                "ϵͳ�汾: MajorVersion = %d, MinorVersion = %d, BuildNumber = %d, ProductType = %d\r\n"
                "����ϵͳ����Ϊ: %s\r\n��ϸʧ����Ϣ����:\r\n\r\n";
            LPCSTR pszVersionA = 0;
            LPCWSTR pszVersionW = winos::GetWindowsVersionString(type, &pszVersionA);
            if ( !dllFile || !dllFile[0] )
            {
                fmt = "����DLLʧ��\r\n"
                    "ϵͳ�汾: MajorVersion = %d, MinorVersion = %d, BuildNumber = %d, ProductType = %d\r\n"
                    "����ϵͳ����Ϊ: %s\r\n��ϸʧ����Ϣ����:\r\n\r\n";
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
            dbgText.AppendFormat("����\"%s\"ʧ��\r\n������ = %d, ��ϸʧ����Ϣ����:\r\n\r\n",
                wstr::W2A(dllFile).c_str(), err);
        }

        dbgText.append("��֧�ֿⲻ֧�ִ�ϵͳ, ȱ������dll����dll�еĺ���\r\n");
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
            //IAT�������ȷ�������һ����ȫΪ0��β
            while ( pIat->u1.Ordinal )
            {
                // �ж��Ƿ����Ƶ���������ŵ���
                // ͨ�����λ�Ƿ�Ϊ1, ���Ϊ1, ��ô��ŵ���
                // ���Ϊ0, ���Ƶ���
                LPCSTR pszFunName = 0;
                if ( pIat->u1.Ordinal & 0x80000000 )
                {
                    // ���λΪ1, ��ŵ���
                    pszFunName = (LPCSTR)( pIat->u1.Ordinal & 0xFFFF );
                }
                else
                {
                    //���λλ0, ���Ƶ���
                    //��ʾ���, ������
                    PIMAGE_IMPORT_BY_NAME pName = (PIMAGE_IMPORT_BY_NAME)( base + pIat->u1.AddressOfData );
                    pszFunName = (LPCSTR)&pName->Name[0];
                }

                if ( !hModule || GetProcAddress(hModule, pszFunName) == 0 )
                {
                    // ��ȡ�������ʧ��, ȱ���������, ȱ�ٵĲż��뵽������
                    const LPCSTR fmt = ( (DWORD)pszFunName <= 0xffff ? "��������: %d" : "%s" );
                    arr.push_back(buf.AddFormat(fmt, pszFunName));
                }

                pIat++;
            }

            int count = arr.size();
            if ( count > 0 )
            {
                if ( !hModule )
                {
                    dbgText.append("ȱ��dll: ").append(pDllName).append("\r\n");
                }
                else
                {
                    dbgText.append("ȱ�� ").append(pDllName).append(" �е����º���:\r\n");

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

