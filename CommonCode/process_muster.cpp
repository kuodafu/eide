#include "process_muster.h"
#include <psapi.h>
#include <vector>



inline int __cdecl wcscmp(wchar_t const* _String1, wchar_t const* _String2, bool isCase)
{
    int(__cdecl* fn_wcscmp)(wchar_t const* _String1, wchar_t const* _String2);
    if (isCase) fn_wcscmp = wcscmp;
    else fn_wcscmp = wcsicmp_api;
    return fn_wcscmp(_String1, _String2);
}

HANDLE process_open(DWORD pid)
{
    if (!pid)return 0;
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}

HANDLE process_open(HWND hWnd)
{
    if (!IsWindow(hWnd))return 0;
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return process_open(pid);
}

bool process_close(HANDLE hProcess)
{
    return CloseHandle(hProcess);
}

DWORD process_create(LPCWSTR fileName, LPCWSTR command, LPPROCESS_INFORMATION proc)
{
    if (!command)command = L"";
    wstr c(wcslen(command) + 260 + wcslen(fileName));
    c.append(L"\"").append(fileName).append(L"\" ").append(command);
    size_t pos = wcsrchr(fileName, '\\') - fileName;
    wchar_t path[260] = { 0 };
    memcpy(path, fileName, pos * sizeof(wchar_t));
    STARTUPINFOW si = { 0 };
    si.cb = sizeof(si);
    PROCESS_INFORMATION p = { 0 };
    BOOL b = CreateProcessW(fileName, c.data(), 0, 0, 0, 0, 0, path, &si, &p);
    if (b && proc)
    {
        memcpy(proc, &p, sizeof(p));
    }
    else
    {
        if (p.hProcess)CloseHandle(p.hProcess);
        if (p.hThread)CloseHandle(p.hThread);
    }
    return p.dwProcessId;
}

DWORD process_create(LPCWSTR fileName, LPCWSTR command, bool wait, int showMode, LPPROCESS_INFORMATION proc)
{
    wstr c(wcslen(command) + 260 + wcslen(fileName));
    c.append(L"\"").append(fileName).append(L"\" ").append(command);
    size_t pos = wcsrchr(fileName, '\\') - fileName;
    wchar_t path[260] = { 0 };
    memcpy(path, fileName, pos * sizeof(wchar_t));
    STARTUPINFOW si = { 0 };
    si.cb = sizeof(si);
    if (showMode != -1)
    {
        si.dwFlags = 1;
        si.wShowWindow = (WORD)showMode;
    }
    PROCESS_INFORMATION p = { 0 };
    BOOL b = CreateProcessW(fileName, c.data(), 0, 0, 0, 0, 0, path, &si, &p);
    if (!b)
        return 0;
    if (wait) 
        WaitForSingleObject(p.hProcess, INFINITE);
    else WaitForInputIdle(p.hProcess, 1000);
    if (proc)
    {
        memcpy(proc, &p, sizeof(p));
    }
    else
    {
        if (p.hProcess)CloseHandle(p.hProcess);
        if (p.hThread)CloseHandle(p.hThread);
    }
    return p.dwProcessId;
}

wstr process_pid_getpath(DWORD pid)
{
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
    if (!hProc)return wstr();
    wstr ret = process_pid_getpath(hProc);

    CloseHandle(hProc);
    return ret;
}

wstr process_pid_getpath(HANDLE hProcess)
{
#if _MSC_VER < 1201
    return wstr();
#else
    if (!hProcess)return wstr();
    wstr wzFile(MAX_PATH);
    GetProcessImageFileNameW(hProcess, wzFile.data(), MAX_PATH);
    wzFile.resize(wcslen(wzFile.c_str()));

    wchar_t szDevName[MAX_PATH] = { 0 };
    wchar_t szDrive[] = L"A:";
    for (int i = 0; i < 26; i++)
    {
        if (QueryDosDeviceW(szDrive, szDevName, MAX_PATH))
        {
            if (wzFile.find(szDevName) != -1)
            {
                wzFile.replace(szDevName, szDrive);
                return wzFile;
            }
        }
        szDrive[0] += 1;
    }
    return wstr();
#endif
}


wstr process_pid_getpath(HWND hWnd)
{
#if _MSC_VER < 1201
    return wstr();
#else
    DWORD pid = 0;
    wstr wzFile;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid)return wstr();
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
    if (!hProc) return wstr();

    wzFile = process_pid_getpath(hProc);
    if (!wzFile.empty())
    {
        CloseHandle(hProc);
        return wzFile;
    }

    if (!hProc)return wzFile;
    HMODULE hModule[255] = { 0 };
    DWORD dwLen = 0;

    if (EnumProcessModulesEx(hProc, hModule, 255, &dwLen, LIST_MODULES_ALL))
    {
        WCHAR tmp[260] = { 0 };
        GetModuleFileNameExW(hProc, hModule[0], tmp, 256);
        wzFile = tmp;
        if (wzFile.find(L"\\??\\") != -1)
        {
            wzFile.replace(L"\\??\\", L"");
        }
    }
    CloseHandle(hProc);
    return wzFile;
#endif
}

DWORD process_enum(std::vector<wstr>* name, std::vector<DWORD>* pids, std::vector<PROCESSENTRY32W>* pes)
{
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
        return 0;
    if (name && name->size())name->resize(0);
    if (pids && pids->size())pids->resize(0);
    if (pes && pes->size())pes->resize(0);
    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32W);
    DWORD count = 0;

    BOOL b = Process32FirstW(hSnap, &pe);
    while (b)
    {
        count++;
        if (name)
            name->push_back(pe.szExeFile);
        if (pids)
            pids->push_back(pe.th32ProcessID);
        if (pes)
            pes->push_back(pe);
        b = Process32NextW(hSnap, &pe);
    }
    CloseHandle(hSnap);
    return count;
}

DWORD process_getid_equal(LPCWSTR name, std::vector<DWORD>* pids)
{
    if (!name)return 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
        return 0;
    if (pids && pids->size())pids->resize(0);
    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32W);
    DWORD count = 0;
    //if(name == 0 && pids == 0)
    BOOL b = Process32FirstW(hSnap, &pe);
    while (b)
    {
        if (wcsicmp_api(name, pe.szExeFile) == 0)
        {
            count++;
            if (pids)pids->push_back(pe.th32ProcessID);
        }
        b = Process32NextW(hSnap, &pe);
    }
    CloseHandle(hSnap);
    return count;
}


DWORD process_getids(LPCWSTR name, std::vector<DWORD>* pids, bool isCase, std::vector<PROCESSENTRY32W>* pes)
{
    if (!name)return 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
        return 0;

    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32W);
    DWORD count = 0;
    //if(name == 0 && pids == 0)
    BOOL b = Process32FirstW(hSnap, &pe);

    int(__cdecl * fn_wcscmp)(wchar_t const* _String1, wchar_t const* _String2);
    if (isCase) fn_wcscmp = wcscmp;
    else fn_wcscmp = wcsicmp_api;

    while (b)
    {
        if (fn_wcscmp(name, pe.szExeFile) == 0)
        {
            count++;
            if (pids)pids->push_back(pe.th32ProcessID);
            if (pes)pes->push_back(pe);
        }
        b = Process32NextW(hSnap, &pe);
    }

    CloseHandle(hSnap);
    return count;
}

wstr process_getName(DWORD pid, LPPROCESSENTRY32W ref)
{
    if (pid == 0)return wstr();
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
        return wstr();
    wstr ret;
    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32W);
    BOOL b = Process32FirstW(hSnap, &pe);
    while (b)
    {
        if (pid == pe.th32ProcessID)
        {
            ret.assign(pe.szExeFile);
            break;
        }
        b = Process32NextW(hSnap, &pe);
    }
    if (ref)
        memcpy(ref, &pe, sizeof(pe));
    CloseHandle(hSnap);
    return ret;
}

bool process_idisvalid(DWORD pid)
{
    if (!pid)return false;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
        return 0;
    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32W);
    bool ret = false;
    //if(name == 0 && pids == 0)
    BOOL b = Process32FirstW(hSnap, &pe);
    while (b)
    {
        if (pe.th32ProcessID == pid)
        {
            ret = true;
            break;
        }
        b = Process32NextW(hSnap, &pe);
    }
    CloseHandle(hSnap);
    return ret;
}

DWORD process_getid_formname(LPCWSTR name, bool isCase)
{
    if (!name)return 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
        return 0;
    int(__cdecl * fn_wcscmp)(wchar_t const* _String1, wchar_t const* _String2);
    if (isCase) fn_wcscmp = wcscmp;
    else fn_wcscmp = wcsicmp_api;

    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32W);
    BOOL b = Process32FirstW(hSnap, &pe);
    DWORD ret = 0;
    while (b)
    {
        if (fn_wcscmp(name, pe.szExeFile) == 0)
        {
            ret = pe.th32ProcessID;
            break;
        }
        b = Process32NextW(hSnap, &pe);
    }

    CloseHandle(hSnap);
    return ret;
}

HWND process_getHwnd(LPCWSTR name)
{
    std::vector<DWORD> arr;
    DWORD len = process_getids(name, &arr, false);
    HWND hWnd = 0;
    for (DWORD i = 0; i < len; i++)
    {
        hWnd = process_getHwnd(arr[i], 0, 0, 100);
        if (hWnd)
            return hWnd;
    }
    return 0;
}

HWND process_getHwnd(DWORD pid, LPCWSTR windowTitle, LPCWSTR windowClassName, DWORD timer, bool isVisible)
{
    HWND hWnd = 0;
#if _MSC_VER > 1200
    ULONGLONG t = GetTickCount64();
    while (GetTickCount64() - t < timer)
#else
    DWORD t = GetTickCount();
    while (GetTickCount() - t < timer)
#endif
    {
        hWnd = FindWindowExA(0, hWnd, 0, 0);
        if (IsWindow(hWnd))
        {
            if (isVisible)
                if (!IsWindowVisible(hWnd))
                    continue;
            DWORD id = hwnd_getpid(hWnd);
            if (id == pid && GetAncestor(hWnd, GA_ROOTOWNER) == hWnd)
            {
                wstr title = window_gettitle(hWnd);
                wstr cls = window_getclass(hWnd);
                if (windowTitle == 0 && windowClassName == 0)
                    return hWnd;
                if (windowTitle == 0)
                {
                    if (cls.find(windowClassName) != wstr::npos)
                        return hWnd;
                }
                else if(windowClassName == 0)
                {
                    if (title.find(windowTitle) != wstr::npos)
                        return hWnd;
                }
                else
                {
                    if (title.find(windowTitle) != wstr::npos && cls.find(windowClassName) != wstr::npos)
                        return hWnd;
                }
                continue;
            }

        }
    }
    return 0;
}

bool process_terminate(HANDLE hProc)
{
    if (!hProc)return false;
    return TerminateProcess(hProc, 0);
}

bool process_terminate(DWORD pid)
{
    HANDLE hProc = OpenProcess(PROCESS_TERMINATE, 0, pid);
    if (!hProc)return false;
    bool bRet = process_terminate(hProc);
    CloseHandle(hProc);
    return bRet;
}


typedef LONG(NTAPI* pfn_RtlAdjustPrivilege)(ULONG Privilege, bool Enable, bool CurrentThread, bool* Enabled);
typedef LONG(NTAPI* pfnNtWow64QueryInformationProcess64)(
    HANDLE ProcessHandle,
    ULONG ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PUINT64 ReturnLength
    );
typedef LONG(NTAPI* pfnNtWow64ReadVirtualMemory64)(
    HANDLE ProcessHandle,
    PVOID64 BaseAddress,
    PVOID Buffer,
    __int64 Size,
    __int64* NumberOfBytesRead
    );
typedef LONG(WINAPI* pfnZwQueryInformationProcess)(
    HANDLE    ProcessHandle,
    ULONG    ProcessInformationClass,
    PVOID    ProcessInformation,
    ULONG    ProcessInformationLength,
    PULONG    ReturnLength
    );
typedef
LONG(NTAPI* pfnNtReadVirtualMemory)(
    HANDLE    ProcessHandle,
    PVOID    BaseAddress,
    PVOID    Buffer,
    ULONG    NumberOfBytesToRead,
    PULONG    NumberOfBytesReaded
    );
typedef BOOL(NTAPI* pfnIsWow64Process)(
    HANDLE hProcess,
    PBOOL  Wow64Process
);



typedef
LONG(WINAPI* pfnNtQueryInformationProcess)(
    HANDLE ProcessHandle,
    ULONG ProcessInformationClass,
    PVOID ProcessInformation,
    UINT32 ProcessInformationLength,
    UINT32* ReturnLength);


typedef struct _UNICODE_STRING32
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING32, * PUNICODE_STRING32;



typedef struct _UNICODE_STRING64
{
    USHORT Length;
    USHORT MaximumLength;
    __int64 Buffer;
} UNICODE_STRING64, * PUNICODE_STRING64;

typedef struct _PROCESS_BASIC_INFORMATION64 {
    LONG ExitStatus;
    UINT32 Reserved0;
    UINT64 PebBaseAddress;
    UINT64 AffinityMask;
    UINT32 BasePriority;
    UINT32 Reserved1;
    UINT64 UniqueProcessId;
    UINT64 InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION64;

typedef struct PEB_LDR_DATA
{
    int Reserved1[6];
    __int64 Ldr;
    __int64 ProcessParameters;
}*LPPEB_LDR_DATA;

typedef struct RTL_USER_PROCESS_PARAMETERS
{
    UNICODE_STRING64 ImagePathName;
    UNICODE_STRING64 CommandLine;
}*LPRTL_USER_PROCESS_PARAMETERS;


typedef struct _PROCESS_BASIC_INFORMATION
{
    LONG    ExitStatus;
    LONG    PebBaseAddress;
    ULONG    AffinityMask;
    LONG    BasePriority;
    ULONG    UniqueProcessId;
    ULONG    InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION,* PPROCESS_BASIC_INFORMATION;



inline static void* GetNtdllFunction(const char* name)
{
    static HMODULE hModule;
    if (!hModule)
    {
        hModule = GetModuleHandleW(L"ntdll.dll");
        if (!hModule)
            hModule = LoadLibraryW(L"ntdll.dll");
        if (!hModule)
            throw;
        
    }
    return GetProcAddress(hModule, name);
}

int RtlAdjustPrivilege(ULONG Privilege, bool Enable, bool CurrentThread, bool* Enabled)
{
    static pfn_RtlAdjustPrivilege fun;
    if (!fun)
        fun = (pfn_RtlAdjustPrivilege)GetNtdllFunction("RtlAdjustPrivilege");
    if (!fun)
        return 0;
    return fun(Privilege, Enable, CurrentThread, Enabled);
}

//NTSTATUS RtlAdjustPrivilege1(ULONG Privilege, bool Enable, bool CurrentThread, bool* Enabled)
//{
//    TOKEN_PRIVILEGES NewState;
//    TOKEN_PRIVILEGES OldState;
//    ULONG ReturnLength;
//    HANDLE TokenHandle;
//    NTSTATUS Status;
//
//    if (CurrentThread)
//    {
//        Status = NtOpenThreadToken(GetCurrentThread(),
//            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
//            FALSE,
//            &TokenHandle);
//    }
//    else
//    {
//        Status = NtOpenProcessToken(GetCurrentProcess(),
//            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
//            &TokenHandle);
//    }
//
//    if (!NT_SUCCESS(Status))
//    {
//        WARN("Retrieving token handle failed (Status %x)\n", Status);
//        return Status;
//    }
//
//    OldState.PrivilegeCount = 1;
//
//    NewState.PrivilegeCount = 1;
//    NewState.Privileges[0].Luid.LowPart = Privilege;
//    NewState.Privileges[0].Luid.HighPart = 0;
//    NewState.Privileges[0].Attributes = (Enable) ? SE_PRIVILEGE_ENABLED : 0;
//
//    Status = NtAdjustPrivilegesToken(TokenHandle,
//        FALSE,
//        &NewState,
//        sizeof(TOKEN_PRIVILEGES),
//        &OldState,
//        &ReturnLength);
//    NtClose(TokenHandle);
//    if (Status == STATUS_NOT_ALL_ASSIGNED)
//    {
//        TRACE("Failed to assign all privileges\n");
//        return STATUS_PRIVILEGE_NOT_HELD;
//    }
//    if (!NT_SUCCESS(Status))
//    {
//        WARN("NtAdjustPrivilegesToken() failed (Status %x)\n", Status);
//        return Status;
//    }
//
//    if (OldState.PrivilegeCount == 0)
//        *Enabled = Enable;
//    else
//        *Enabled = (OldState.Privileges[0].Attributes & SE_PRIVILEGE_ENABLED);
//
//    return STATUS_SUCCESS;
//}


bool ImproveProcPriv()
{
#if _MSC_VER < 1201
    return false;
#else
    HANDLE token;
    //提升权限
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token))
    {
        return false;
    }
    TOKEN_PRIVILEGES tkp;
    tkp.PrivilegeCount = 1;
    ::LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(token, FALSE, &tkp, sizeof(tkp), NULL, NULL))
    {
        return false;
    }
    CloseHandle(token);
    return true;
#endif
}


inline static int GetPortFormCommand(LPCWSTR cmd, LPCWSTR procName = 0)
{
    size_t npos;
    //! 先判断edge调试端口, 在判断谷歌调试端口
    //! edge端口也会开启谷歌的调试端口 
    if (!procName)
    {
        npos = wstr::find(cmd, L"--devtools-server-port=");        // edge 调试端口
        if (npos == _str::npos)
            npos = wstr::find(cmd, L"-remote-debugging-port=");    // 谷歌 调试端口
    }
    else
    {
        if (wcscmp(procName, L"msedge.exe") == 0)
            npos = wstr::find(cmd, L"--devtools-server-port=");    // edge 调试端口
        else
            npos = wstr::find(cmd, L"-remote-debugging-port=");    // 谷歌 调试端口
    }
    if (npos != _str::npos)        // 这个进程有开启远程调试端口
    {
        wchar_t str[50] = { 0 };
        int count = 0;
        const wchar_t* pcmd = cmd + npos + 23;
        while (*pcmd == L'\"')
            pcmd++;
        while ((!(*pcmd == '\"' || *pcmd == ' ')) && count < 49)
            str[count++] = *pcmd++;
        return wstr::stoi(str);
    }
    return 0;
}


void __process_GetCommandLine(DWORD id, wstr* refCmd);
struct _CHROME_ENUM_CHROME_HWND
{
    HWND hWnd;
    DWORD pid;
};

struct _Chrome_Enum_hWnd_DATA
{
    std::vector<_CHROME_ENUM_CHROME_HWND>* arr;
    std::vector<DWORD>* pids;
    DWORD pid;
};
// 枚举当前窗口是谷歌浏览器顶层窗口的窗口句柄
inline static bool CALLBACK _Chrome_Enum_hWnd(HWND hWnd, void* param)
{
    _Chrome_Enum_hWnd_DATA* data = (_Chrome_Enum_hWnd_DATA*)param;
    if (!data)return false;
    if (!IsWindowVisible(hWnd))return false;
    std::vector<_CHROME_ENUM_CHROME_HWND>* arr = data->arr;
    std::vector<DWORD>* pids = data->pids;
    DWORD pid = 0;
    wchar_t cls[MAX_PATH] = { 0 };
    GetClassNameW(hWnd, cls, MAX_PATH);
    int cmp = wcscmp(cls, L"Chrome_WidgetWin_1");   // edge 类名也叫这个名字
    if (cmp == 0)
    {
        GetWindowThreadProcessId(hWnd, &pid);
        for (size_t i = 0; i < pids->size(); i++)
        {
            if (pid == pids->at(i))
            {
                _CHROME_ENUM_CHROME_HWND a;
                a.hWnd = hWnd;
                a.pid = pid;
                arr->push_back(a);
                return true;
            }
        }
    }
    return false;
}
// 枚举当前窗口是谷歌浏览器顶层窗口的窗口句柄
inline static bool CALLBACK _Chrome_Enum_hWnd_FormProcessId(HWND hWnd, void* param)
{
    _Chrome_Enum_hWnd_DATA* data = (_Chrome_Enum_hWnd_DATA*)param;
    if (!data)return false;
    if (!IsWindowVisible(hWnd))return false;
    std::vector<_CHROME_ENUM_CHROME_HWND>* arr = data->arr;
    wchar_t cls[MAX_PATH] = { 0 };
    GetClassNameW(hWnd, cls, MAX_PATH);
    int cmp = wcscmp(cls, L"Chrome_WidgetWin_1");   // edge 类名也叫这个名字
    if (cmp == 0)
    {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid == data->pid)
        {
            _CHROME_ENUM_CHROME_HWND a;
            a.hWnd = hWnd;
            a.pid = pid;
            arr->push_back(a);
            return true;
        }
    }
    return false;
}

struct _HWNS_PID_
{
    HWND hWnd;
    DWORD pid;
    int port;
};

inline static HWND GetHwndFormPid(std::vector<_HWNS_PID_>& arr, DWORD pid)
{
    size_t len = arr.size();
    for (size_t i = 0; i < len; i++)
    {
        _HWNS_PID_& data = arr[i];
        if (pid == data.pid)
            return data.hWnd;
    }
    return 0;
}

// 取带有调试端口启动的谷歌浏览器
int GetChromeCommandLinePort(std::vector<_HWNS_PID_>& arr)
{
    static int s_first;
    if (s_first == 0)
    {
        s_first = 1;
        bool b;
        HRESULT hr = RtlAdjustPrivilege(20, true, false, &b);
        if (FAILED(hr))
        {
            ImproveProcPriv();
        }
    }

    std::vector<_HWNS_PID_> wnds;
    HWND hWnd = GetWindow(GetDesktopWindow(), GW_CHILD);
    while (hWnd)
    {
        if (IsWindowVisible(hWnd))
        {
            _HWNS_PID_ data;
            data.hWnd = hWnd;
            data.port = 0;
            GetWindowThreadProcessId(hWnd, &data.pid);
            wnds.push_back(data);
        }
        hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    }


    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
        return 0;
    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32W);
    DWORD count = 0;

    BOOL b = Process32FirstW(hSnap, &pe);
    wstr cmd;
    while (b)
    {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pe.th32ProcessID);
        if (hProc)
        {
            cmd = process_GetCommandLine(hProc);
            DWORD pid = pe.th32ProcessID;
            int port = GetPortFormCommand(cmd.c_str(), pe.szExeFile);
            if (port)
            {
                _HWNS_PID_ data;
                data.pid = pid;
                data.port = port;
                data.hWnd = GetHwndFormPid(wnds, pid);
                arr.push_back(data);
                ++count;
            }
            CloseHandle(hProc);
        }
        b = Process32NextW(hSnap, &pe);
    }
    CloseHandle(hSnap);
    return count;
}

void __process_GetCommandLine(DWORD id, wstr* refCmd)
{
    refCmd->assign(process_GetCommandLine(id));
}


bool process_queryCommandLine(int port, HANDLE* phProc, DWORD* ppid, bool isClose, std::vector<int>* retPort, wstr* refCmd, HWND* retHwnd)
{
    if (refCmd)refCmd->clear();
    if (!phProc || !ppid)return false;

    std::vector<_HWNS_PID_> wnds;
    HWND hWnd = GetWindow(GetDesktopWindow(), GW_CHILD);
    while (hWnd)
    {
        if (IsWindowVisible(hWnd))
        {
            _HWNS_PID_ data;
            data.hWnd = hWnd;
            data.port = 0;
            GetWindowThreadProcessId(hWnd, &data.pid);
            wnds.push_back(data);
        }
        hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    }

    DWORD pid = *ppid;
    if (pid)
    {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE | SYNCHRONIZE, 0, pid);
        if (hProc)
        {
            wstr cmd = process_GetCommandLine(hProc);
            int port1 = GetPortFormCommand(cmd.c_str());
            if (port1 == port)
            {
                if (retHwnd)
                    *retHwnd = GetHwndFormPid(wnds, pid);

                *phProc = hProc;
                *ppid = pid;
                if (retPort)
                {
                    retPort->resize(1);
                    retPort->at(0) = port;
                }
                if (refCmd)refCmd->assign(cmd);
                return true;
            }
            CloseHandle(hProc);
        }
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
        return 0;
    PROCESSENTRY32W pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32W);
    DWORD count = 0;

    BOOL b = Process32FirstW(hSnap, &pe);
    wstr cmd;
    while (b)
    {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE | SYNCHRONIZE, 0, pe.th32ProcessID);
        if (hProc)
        {
            cmd = process_GetCommandLine(hProc);
            pid = pe.th32ProcessID;
            int port1 = GetPortFormCommand(cmd.c_str(), pe.szExeFile);
            if (port1 == port)
            {
                if (retHwnd) *retHwnd = GetHwndFormPid(wnds, pe.th32ProcessID);

                if (retPort) retPort->push_back(port1);
                *ppid = pe.th32ProcessID;    // 返回这个进程ID
                *phProc = hProc;            // 返回进程句柄
                if (refCmd)refCmd->assign(cmd);
                return true;
            }
            CloseHandle(hProc);
        }
        b = Process32NextW(hSnap, &pe);
    }
    CloseHandle(hSnap);
    return count;
}

inline static LONG NtWow64QueryInformationProcess64(
    HANDLE ProcessHandle,
    ULONG ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PUINT64 ReturnLength
)
{
    static pfnNtWow64QueryInformationProcess64 fun;
    if(!fun)
        fun = (pfnNtWow64QueryInformationProcess64)GetNtdllFunction("NtWow64QueryInformationProcess64");
    if (!fun) return 0;
    return fun(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}

inline static LONG NtWow64ReadVirtualMemory64(
    HANDLE ProcessHandle,
    PVOID64 BaseAddress,
    PVOID Buffer,
    __int64 Size,
    __int64* NumberOfBytesRead
)
{
    static pfnNtWow64ReadVirtualMemory64 fun;
    if (!fun)
        fun = (pfnNtWow64ReadVirtualMemory64)GetNtdllFunction("NtWow64ReadVirtualMemory64");
    
    if (!fun) return 0;
    //_str::dbg("NtWow64ReadVirtualMemory64(%p, %lld, %p, %lld, %p);", 
    //    ProcessHandle, BaseAddress, Buffer, Size, NumberOfBytesRead);
    return fun(ProcessHandle, BaseAddress, Buffer, Size, NumberOfBytesRead);
}

inline static LONG ZwQueryInformationProcess(
    HANDLE    ProcessHandle,
    ULONG    ProcessInformationClass,
    PVOID    ProcessInformation,
    ULONG    ProcessInformationLength,
    PULONG    ReturnLength
)
{
    static pfnZwQueryInformationProcess fun;
    if (!fun)
        fun = (pfnZwQueryInformationProcess)GetNtdllFunction("ZwQueryInformationProcess");
    if (!fun) return 0;
    return fun(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);
}

inline static LONG NtReadVirtualMemory(
    HANDLE    ProcessHandle,
    PVOID    BaseAddress,
    PVOID    Buffer,
    ULONG    NumberOfBytesToRead,
    PULONG    NumberOfBytesReaded
)
{
    static pfnNtReadVirtualMemory fun;
    if (!fun)
        fun = (pfnNtReadVirtualMemory)GetNtdllFunction("NtReadVirtualMemory");
    if (!fun) return 0;
    return fun(ProcessHandle, BaseAddress, Buffer, NumberOfBytesToRead, NumberOfBytesReaded);
}

BOOL fnIsWow64Process(HANDLE hProcess, PBOOL Wow64Process)
{
    if (Wow64Process) *Wow64Process = FALSE;
    static pfnIsWow64Process fun;
    if (!fun)
    {
        HMODULE hModule;
        hModule = GetModuleHandleW(L"Kernel32.dll");
        if (!hModule)
            hModule = LoadLibraryW(L"Kernel32.dll");
        if (!hModule)
            throw;
        fun = (pfnIsWow64Process)GetProcAddress(hModule, "IsWow64Process");
    }
    if (!fun) return FALSE;

    return fun(hProcess, Wow64Process);
}

wstr process_GetCommandLine(DWORD pid)
{
    //RtlAdjustPrivilege(20, true, false, 0);
    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid);
    if (!hProc)
        return wstr();
    wstr ret = process_GetCommandLine(hProc);
    CloseHandle(hProc);
    return ret;
}



wstr process_GetCommandLine(HANDLE hProc)
{
    if (!hProc)
        return wstr();

    static bool is32System;    // 是否为32位系统, 32位系统另外处理
    static bool frist;
    if (!frist)
    {
        frist = true;
#ifdef _M_X64
        is32System = false;
#else
        BOOL isWow64 = 0;
        // 因为当前进程只能被易语言调用, 所以当前进程一定是32位
        // 如果不是在Wow64环境下运行, 就表示系统是32位的
        BOOL ret = fnIsWow64Process(GetCurrentProcess(), &isWow64);
        if (!ret || !isWow64)
            is32System = true;
#endif
    }

    if (is32System)
    {
        PROCESS_BASIC_INFORMATION info = { 0 };
        MEMORY_BASIC_INFORMATION c = { 0 };
        DWORD buffer = 0;
        ZwQueryInformationProcess(hProc, 0, &info, sizeof(PROCESS_BASIC_INFORMATION), 0);
        // [[peb+10h]+28h]进程运行目录,[[peb+10h]+3ch]进程路径,[[peb+10h]+44h]命令行
        NtReadVirtualMemory(hProc, (PVOID)(ULONG_PTR)(info.PebBaseAddress + 16), &buffer, sizeof(buffer), 0);
        DWORD a = buffer;

        NtReadVirtualMemory(hProc, (PVOID)(ULONG_PTR)(a + 68), &buffer, sizeof(buffer), 0);    // 68命令行, 60路径, 40运行目录
        PVOID b = (PVOID)(ULONG_PTR)buffer;

        VirtualQueryEx(hProc, b, &c, sizeof(c));
        DWORD len = (DWORD)(c.RegionSize + SIZE_T(c.BaseAddress) - SIZE_T(b));

        wstr ret(len);
        NtReadVirtualMemory(hProc, b, ret.data(), len, 0);
        ret.resize(wcslen(ret.c_str()));
        return ret;
    }

    PROCESS_BASIC_INFORMATION64 pbi = { 0 };
    if (NtWow64QueryInformationProcess64(hProc, 0, &pbi, sizeof(pbi), 0) == 0)
    {
        PEB_LDR_DATA Ldr = { 0 };
        if (NtWow64ReadVirtualMemory64(hProc, (PVOID64)pbi.PebBaseAddress, &Ldr, sizeof(Ldr), 0) == 0)
        {
            RTL_USER_PROCESS_PARAMETERS RTL = { 0 };
            if (NtWow64ReadVirtualMemory64(hProc, (PVOID64)(Ldr.ProcessParameters + 96), &RTL, sizeof(RTL), 0) == 0)
            {
                if (RTL.CommandLine.Buffer > 0)
                {
                    wstr ret(RTL.CommandLine.Length);
                    NtWow64ReadVirtualMemory64(hProc, (PVOID64)RTL.CommandLine.Buffer, ret.data(), RTL.CommandLine.Length, 0);
                    ret.resize(wcslen(ret.c_str()));
                    return ret;
                }
            }
        }
    }
    return wstr();
}


DWORD hwnd_getpid(HWND hWnd)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid;
}

wstr window_getclass(HWND hWnd)
{
    if (!IsWindow(hWnd))return wstr();
    wstr str(260);
    int len = GetClassNameW(hWnd, str.data(), 260);
    str.resize(len);
    return str;
}

wstr window_gettitle(HWND hWnd)
{
    if (!IsWindow(hWnd))return wstr();
    int len = GetWindowTextLengthW(hWnd) + 1;
    wstr str(len);
    len = GetWindowTextW(hWnd, str.data(), len);
    str.resize(len);
    return str;
}

struct _ENUMCHILDWINDOW_PROCDATA
{
    std::vector<HWND>* hWnds;    // 存放结果数组
    LPCWSTR className;        // 过滤的类名
    LPCWSTR titleName;        // 过滤的标题
    int count;                // 枚举到的窗口数量
    bool isCase;            // 是否区分大小写
    void* param;            // 传递到回调函数里的参数
    pfnEnumWindowCompare callback;    // 使用这个来决定是否加入数组里
};
BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM param)
{
    if (!param)return FALSE;

    _ENUMCHILDWINDOW_PROCDATA* data = (_ENUMCHILDWINDOW_PROCDATA*)param;
    if (data->callback)
    {
        if (data->callback(hWnd, data->param))
        {
            data->count++;
            if (data->hWnds)
                data->hWnds->push_back(hWnd);
        }
        return TRUE;
    }
    bool isJoin = false;    // 是否加入数组, 系统判断标题或者类名, 符合一项就赋值为true

    int(__cdecl * fn_wcscmp)(wchar_t const* _String1, wchar_t const* _String2);
    if (data->isCase) fn_wcscmp = wcscmp;
    else fn_wcscmp = wcsicmp_api;


    if (!isJoin && data->className)
    {
        wchar_t cln[260] = { 0 };
        GetClassNameW(hWnd, cln, 260);
        isJoin = fn_wcscmp(data->className, cln) == 0;
    }
    if (!isJoin && data->titleName)
    {
        wstr title = window_gettitle(hWnd);
        isJoin = fn_wcscmp(data->titleName, title.c_str()) == 0;
    }
    if (isJoin)
    {
        data->count++;
        if (data->hWnds)
            data->hWnds->push_back(hWnd);
    }
    return TRUE;
    //要继续枚举，回调函数必须返回TRUE；要停止枚举，它必须返回FALSE。
}


int window_enumwindow(HWND hWndParent, std::vector<HWND>* hWnds, bool isCase, LPCWSTR className, LPCWSTR titleName, pfnEnumWindowCompare compare, void* param)
{
    _ENUMCHILDWINDOW_PROCDATA data;
    data.className = className;
    data.titleName = titleName;
    data.isCase = isCase;
    data.count = 0;
    data.hWnds = hWnds;
    data.param = param;
    data.callback = compare;
    EnumChildWindows(hWndParent, EnumChildProc, (LPARAM)&data);
    return data.count;
}
