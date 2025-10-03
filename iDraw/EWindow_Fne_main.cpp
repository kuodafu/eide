// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <elib/lib2.h>
#include <elib/lang.h>
#include "iDraw_Version.h"
#include "resource.h"
#include <assist/CFileRW.h>
#include <psapi.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <control/Ctool.h>
#include <datetime.h>
#include <control/CImgList.h>
#include "EWindow_Fne_Main.h"
#include <control/ReadColor.h>
#include <elib/fne_decrypt.h>
#include <CMapMemFile.h>
#include <unordered_map>
#include <unordered_set>
#include <CDarkMode.h>
#include <CThemeColor.h>
#include <assist/PEHeader.h>
#include <e/GetEVersion.h>


#define PLUGIN_MOVEWINDOW   L"iWindowSize.dll"  // 移动插件
#define PLUGIN_RESOURCE     L"iResource.dll"    // 资源插件
#define PLUGIN_DRAWCONTROL  L"iControls.dll"    // 绘画插件
#define PLUGIN_EVENT        L"iEvent.dll"       // 事件插件
#define PLUGIN_CONFIG       L"iConfig.dll"      // 配置插件, 所有配置读写都由这里完成
#define PLUGIN_THEME        L"iTheme.dll"       // 主题插件, 配色相关的都从这里读写, 第二个加载的模块
#define EXTERADLL_CETOOLS   L"superTools.dll"   // 助手插件

#define IDRAW_MESSAGETITLE  IDRAW_FNE_NAMEA L" - " 



//static std::vector<CUSTOMTAB_DATA> s_hWnds; // 自绘选择夹对应的窗口句柄
static PFN_NOTIFY_SYS s_fnNotifySys;
static HWND s_hWndEWindow;
static CGetEWindow s_eWnd;
static HINSTANCE m_hInst;
static IDRAW_INFO allInfo;
static CPrivateProfile s_ini;
static CPrivateProfile s_iniColor;

// 自定义代码切换选择夹类
class CustomCodeTab : public ICustomCodeTab
{
    std::vector<CUSTOMTAB_DATA> arr; // 自绘选择夹对应的窗口句柄
public:
    virtual int             size()                                  { return arr.size(); }
    virtual bool            empty()                                 { return arr.empty(); }
    virtual int             erase(int index)                        { arr.erase(arr.begin() + index); return (int)arr.size(); }
    virtual CUSTOMTAB_DATA& at(int index)                           { return arr[index]; }
    virtual CUSTOMTAB_DATA& operator[](int index)                   { return arr[index]; }
    virtual int             push_back(const CUSTOMTAB_DATA& data)   { arr.push_back(data); return (int)arr.size(); }
    virtual int insert(const CUSTOMTAB_DATA& data, int index)       { arr.insert(arr.begin() + index, data); return (int)arr.size(); }
    virtual int             resize(int count)                       { arr.resize(count); return (int)arr.size(); }
    virtual void            clear()                                 { arr.resize(0); }

};
static CustomCodeTab s_hWnds;


IDRAW_INFO* s_info;                    // 包含各个窗口的信息, 支持库和其他动态库都有使用, 支持库调用动态库的时候会传递这个指针过去
static std::vector<LPPLUGIN_INFO> m_debug_module; // 调试插件数组
static wchar_t m_ePath[520];                // 易语言所在目录
static wchar_t m_runFile[520];              // 动态库的完整路径
static wchar_t m_runPath[520];              // 支持库所在目录, 这是从易语言环境获取到的路径, 后面带 "\"
static wchar_t m_dataPath[520];             // 数据保存目录, 后面带 "\", 这个是支持库目录下的 iDraw\\data\\ 目录
static wchar_t m_iDrawPath[520];            // 核心插件目录, 后面带 "\", 这个是支持库目录下的 iDraw\ 目录
static wchar_t m_pluginPath[520];           // 插件目录, 后面带 "\", 这个是支持库目录下的 iDraw\plugin\ 目录
static wchar_t m_etpluginPath[520];         // 助手插件目录, 后面带 "\", 这个是支持库目录下的 iDraw\plugin\ 目录
static wchar_t m_configPath[520];           // 配置文件完整路径

static std::vector<LPPLUGIN_INFO> m_module; // 已加载的插件模块句柄数组
static bool m_isFirstLoad;                  // 是否首次加载支持库, 为true的话不加载支持库任何功能
void _debug_load_plugin();
void _debug_free_plugin();
void _ide_ready();
void _ide_free(INT);
// 从插件目录加载插件
void iDraw_LoadPlugin();
void iDraw_SetLastError(LPCWSTR fmt, ...);
EXTERN_C _declspec(dllexport) LPCWSTR WINAPI iDraw_GetLastError();
EXTERN_C _declspec(dllexport) LPCSTR  WINAPI iDraw_GetLastErrorA();


std::unordered_map<int, PLUGIN_INFO>     s_plugin_info;      // 所有插件信息
std::unordered_map<int, LPPLUGIN_INFO>   s_plugin_info_hash; // 记录已经加载的插件, hash值是外部传递进来的guid地址
std::vector<LPPLUGIN_INFO>               s_plugin_array;     // 所有插件信息, 按加入顺序记录, s_plugin_info 这个不是按顺序, 两个记录的数据一样





bool Plugin_UnInit(LPCSTR guid);

//计算Offset
ULONG RvaToOffset(PIMAGE_NT_HEADERS pNtHeader, ULONG Rva)
{
    //PE节
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(pNtHeader + 1);

    ULONG sNum, i;
    //取得节表项数目
    sNum = pNtHeader->FileHeader.NumberOfSections;
    for (i = 0; i < sNum; i++)
    {
        //printf("PE 节名称: %s\n",p_section_header->Name);
        if ((pSection->VirtualAddress <= Rva) && Rva < (pSection->VirtualAddress + pSection->SizeOfRawData))
        {
            return Rva - pSection->VirtualAddress + pSection->PointerToRawData;
        }
        pSection++;
    }
    return 0;
}

// 获取这个地址是当前进程哪个模块里的地址, 用来调试注册消息/事件堆栈错误的问题
inline const char* GetDllNameFromAddress(LPCVOID pAddress)
{
    if (!pAddress)
        return 0;
    MEMORY_BASIC_INFORMATION mem;
    BOOL isOk = VirtualQuery(pAddress, &mem, sizeof(MEMORY_BASIC_INFORMATION));
    LPBYTE pCmp = (LPBYTE)mem.AllocationBase;
    if (!isOk) return 0;

    static char runFile[260];
    runFile[0] = 0;
    const int fileBuffer = sizeof(runFile) / sizeof(runFile[0]);
    GetModuleFileNameA((HMODULE)pCmp, runFile, fileBuffer);   // 先获取到程序运行路径
    if (runFile[0])
    {
        char* pos = strrchr(runFile, '\\');
        if (pos)
        {
            *pos++ = 0;
            return pos;
        }
        return runFile;
    }

    if (pCmp[0] != 'M' || pCmp[1] != 'Z')
        return 0;

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)pCmp;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pCmp + dosHeader->e_lfanew);

    ULONG offsetExportTable = RvaToOffset(nt, nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);    // 导出表偏移
    PIMAGE_EXPORT_DIRECTORY pExportTable = (PIMAGE_EXPORT_DIRECTORY)(pCmp + offsetExportTable);   // 导出表
    if (!offsetExportTable) return 0;

    char* dllName = (char*)(pCmp + RvaToOffset(nt, pExportTable->Name));
    return dllName;



    ULONG offsetImportTable = RvaToOffset(nt, nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);    // 导出表偏移
    PIMAGE_IMPORT_DESCRIPTOR pImportTable = (PIMAGE_IMPORT_DESCRIPTOR)(pCmp + offsetImportTable);   // 导入表
    if (!offsetImportTable) return 0;


    IMAGE_IMPORT_DESCRIPTOR null_iid;
    IMAGE_THUNK_DATA null_thunk;
    memset(&null_iid, 0, sizeof(null_iid));
    memset(&null_thunk, 0, sizeof(null_thunk));

    //每个元素代表了一个引入的DLL。
    for (int i = 0; memcmp(pImportTable + i, &null_iid, sizeof(null_iid)) != 0; i++)
    {
        char* dllName = (char*)(pCmp + RvaToOffset(nt, pImportTable[i].Name));

        //拿到了DLL的名字

        printf("模块[%d]: %s\n", i, (char*)dllName);
        PIMAGE_THUNK_DATA32 pThunk = (PIMAGE_THUNK_DATA32)(pCmp + RvaToOffset(nt, pImportTable[i].FirstThunk));

        while (pThunk->u1.Ordinal != NULL)
        {
            PIMAGE_IMPORT_BY_NAME pname = (PIMAGE_IMPORT_BY_NAME)(pCmp + RvaToOffset(nt, pThunk->u1.AddressOfData));
            printf("函数编号: %d 名称: %s\n", pname->Hint, pname->Name);
            pThunk++;
        }
    }

    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    const int x = IDC_ICONTROL_ADD_MENUBAR; // 80744452
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        m_hInst = hModule;
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



inline static void __createPath(LPWSTR path)
{
    if (PathIsDirectoryW(path)) return;
    LPWSTR find = wcschr(path, '\\');
    wchar_t old = 0;
    while (find)
    {
        old = *find;
        *find = 0;
        if (!PathIsDirectoryW(path))
            CreateDirectoryW(path, 0);


        *find = old;
        find = wcschr(find + 1, '\\');

    }
    if (!PathIsDirectoryW(path))
        CreateDirectoryW(path, 0);
}

inline void _log_dbg_output_to_fileEx(LPCSTR fmt, va_list list)
{
    static CFileRW log;
    if (log.GetFile() == 0)
    {
        wstr file(260);
        file.assign(m_dataPath).append(L"_log_dbg.txt");
        log.open(file.c_str(), 4, -1);
    }

    _str str;
    str.FormatEx(260, fmt, list);

    char time[50];
    _str dbg;
    dbg.Format("【%s】%s", datetimeA::to_string(::time(0), time), str.c_str());
    log.write(dbg.c_str());
}

inline void _log_dbg_output_to_file(LPCSTR fmt, ...)
{
    va_list aptr;
    va_start(aptr, fmt);
    _log_dbg_output_to_fileEx(fmt, aptr);
    va_end(aptr);
}


// 返回记录这个插件的指针, 返回的值不是调用初始化接口返回的值
inline LPPLUGIN_INFO _load_plugin(HMODULE& hModule, bool isMessageBox)
{
    if (!hModule)return 0;
    pfn_IDraw_Interface pfn = (pfn_IDraw_Interface)GetProcAddress(hModule, "IDraw_Interface");
    pfn_IDraw_UnInterface pfnUn = (pfn_IDraw_UnInterface)GetProcAddress(hModule, "IDraw_UnInterface");
    while (1)
    {
        if (!pfn || !pfnUn)
        {
            if (isMessageBox)
                MessageBoxW(0,
                    L"插件必须拥有\n整数返回值 IDraw_Interface(参数1, 参数2, 参数3, 参数4)\n"
                    L"无返回值 IDraw_UnInterface(无参数)\n两个接口",
                    IDRAW_MESSAGETITLE L"错误的接口",
                    MB_ICONERROR);
            break;
        }

        LPPLUGIN_INFO info = pfn(s_info, s_info->eWnd, 0, 0);
        if (!info || !info->cbSize || !info->name || !info->pfnCallback || !info->guid)
            break;

        LPPLUGIN_INFO data = Plugin_Init(info->guid, hModule, info, info->pfnMenuExt_Command, info->pfnCallback);
        if (data)
        {
            data->pfnInterface = pfn;
            data->pfnUnInterface = pfnUn;
            

            //data->pfnEConfigChange = (pfn_IDraw_ConfigChange)GetProcAddress(hModule, "IDraw_ConfigChange");
            return data;
        }
        Plugin_UnInit(info->guid);  // 初始化失败, 取消这个插件挂接的所有事件, 然后卸载
        break;
    }

    FreeLibrary(hModule);
    hModule = 0;
    return 0;
}

struct _PLUGIN_LOAD_INFO
{
    HMODULE hModule;
    pfn_IDraw_Interface pfn;
    pfn_IDraw_UnInterface pfnUn;
    LPCWSTR fileName;
};
// 加载核心插件, 会检测 IDraw_Interface 和 IDraw_UnInterface 两个接口, 如果没有这两个接口, 则释放dll, 并返回0
inline pfn_IDraw_Interface _load_plugin(LPCWSTR fileName, _PLUGIN_LOAD_INFO& info, bool isExit, wstr& errText)
{
    info.fileName = fileName;
    if (!info.hModule)
    {
        wstr dllPath(260);
        dllPath.assign(m_runPath).append(IDRAW_FNE_IDRAW).append(L"\\").append(fileName);
        int fileSize = 0;
        {
            CFileRW f;
            f.open(dllPath.c_str(), 1, -1);
            fileSize = f.size();
            f.reset();
        }
        info.hModule = LoadLibraryW(dllPath.c_str());    // 加载支持库第一步先加载基础插件
        if (isExit)
        {
            assert(info.hModule != 0);
        }
    }
    if (!info.hModule)
    {
        if (isExit)
        {
            errText.append(L"易语言主程序目录\\lib\\iDraw 目录下 缺少插件 ").append(fileName).append(L", 请下载新的").append(fileName).append(L"覆盖 或者重新安装支持库\n");
            //wchar_t text[260];
            //swprintf_s(text, 260, L"iDraw 目录下 缺少插件 %s, 请下载新的 %s 覆盖 或者重新安装支持库", fileName, fileName);
            //MessageBoxW(0, text, L"缺少必备插件", MB_ICONERROR);
            //exit(0);
        }
        return 0;
    }

    info.pfn = (pfn_IDraw_Interface)GetProcAddress(info.hModule, "IDraw_Interface");
    info.pfnUn = (pfn_IDraw_UnInterface)GetProcAddress(info.hModule, "IDraw_UnInterface");
    if (!info.pfn || !info.pfnUn)
    {
        errText.append(L"插件 ").append(fileName).append(L" 没有必备接口 IDraw_Interface/IDraw_UnInterface, 或者接口没有返回插件信息, 请检查插件是否被篡改");
        //wchar_t text[260];
        //swprintf_s(text, 260, L"核心插件插件 %s 初始化失败\n没有必备接口 IDraw_Interface/IDraw_UnInterface\n或者接口没有返回插件信息\n\n请检查插件是否被篡改", fileName);
        //MessageBoxW(0, text, L"初始化失败", MB_ICONERROR);
        //exit(0);
        return 0;
    }
    return info.pfn;
}

// 退出易语言
inline void _exit_e(LPCWSTR str)
{
    LPCWSTR n = L"\n";
    if (!str || !str[0])str = L"", n = L"";

    wstr text;
    text.Format(L"%s%s%s核心插件数据损坏, 无法正常启动易语言\n"      \
        L"请尝试以下两种解决方法\n" \
        L"    1. 重置易语言配置 (按住 Shift + 启动易语言)\n"   \
        L"    2. 重新安装%s支持库",
        str, n,
        IDRAW_FNE_NAME,
        IDRAW_FNE_NAME);
    MessageBoxW(s_hWndEWindow, text.c_str(), IDRAW_MESSAGETITLE L"核心插件出错", MB_ICONERROR);
    exit(0);
    return;
}

inline LPPLUGIN_INFO _call_plugin(_PLUGIN_LOAD_INFO& loadInfo, pfn_IDraw_Callback& pfnCallback)
{
    LPPLUGIN_INFO info = loadInfo.pfn(s_info, s_info->eWnd, 0, 0);  // 最先加载配置插件
    LPCWSTR text = 0;
    while (1)
    {
#define __err_return(_v, _s) if(_v) { text = _s; break; } 
        __err_return(!info,              L"插件没有返回插件信息");
        __err_return(!info->cbSize,      L"插件返回的结构尺寸错误, 请调用模块里的 易绘_生成插件信息(), 把返回值返回给支持库");
        __err_return(!info->name,        L"插件未返回插件名");
        __err_return(!info->pfnCallback, L"插件未返回与视觉库交互的回调函数, 函数原型 = 整数型返回值(消息值, 参数1, 参数2)");
        __err_return(!info->guid,        L"插件未返回插件唯一标识符");

        LPPLUGIN_INFO data = Plugin_Init(info->guid, loadInfo.hModule, info, info->pfnMenuExt_Command, info->pfnCallback);
        if (data)
        {
            data->pfnInterface = loadInfo.pfn;
            data->pfnUnInterface = loadInfo.pfnUn;
            pfnCallback = info->pfnCallback;
            //data->pfnEConfigChange = (pfn_IDraw_ConfigChange)GetProcAddress(hModule, "IDraw_ConfigChange");
            return data;
        }
        break;
    }
    FreeLibrary(loadInfo.hModule);
    loadInfo = { 0 };
    wchar_t errText[260];
    swprintf_s(errText, 260, L"插件 %s 加载失败\n失败原因:\n\t%s", loadInfo.fileName, text);
    return 0;
    
}

inline void _make_path(LPWSTR buf, LPCWSTR s1, LPCWSTR s2, LPCWSTR s3)
{
    wcscpy_s(buf, 520, s1);
    if (s2) wcscat_s(buf, 520, s2);
    if (s3) wcscat_s(buf, 520, s3);
}

// 将第一个字符串拷贝到第二个字符串, 如果第二个字符串不够存放, 会新申请内存存放, 返回第二个字符串的地址
// 不使用时需要调用 _free_str 进行释放
inline static wchar_t* WINAPI _copy_str(const wchar_t* str, wchar_t*& freeStr)
{
    if (!str)
    {
        if (freeStr)
            *freeStr = 0;
        return 0;
    }
    if (str == freeStr)return freeStr;

    size_t len1 = wcslen(str) + 1;
    size_t len2 = 0;
    if (freeStr)
        len2 = wcslen(freeStr) + 1;

    if (freeStr && len1 == len2 && wcscmp(str, freeStr) == 0)
        return 0;

    if (len2 < len1)	// 被释放的文本缓冲区不够大
    {
        if (freeStr)
            wstr::free(freeStr);
        freeStr = wstr::alloc(len1);
    }
    if (!freeStr)
        return 0;

    memcpy(freeStr, str, len1 * sizeof(wchar_t));
    return freeStr;
}

// 将第一个字符串拷贝到第二个字符串, 如果第二个字符串不够存放, 会新申请内存存放, 返回第二个字符串的地址
// 不使用时需要调用 _free_str 进行释放
inline static char* WINAPI _copy_strA(const char* str, char*& freeStr)
{
    if (!str)
    {
        if (freeStr)
            *freeStr = 0;
        return 0;
    }
    if (str == freeStr)return freeStr;

    size_t len1 = strlen(str) + 1;
    size_t len2 = 0;
    if (freeStr)
        len2 = strlen(freeStr) + 1;

    if (freeStr && len1 == len2 && strcmp(str, freeStr) == 0)
        return 0;

    if (len2 < len1)	// 被释放的文本缓冲区不够大
    {
        if (freeStr)
            _str::free(freeStr);
        freeStr = _str::alloc(len1);
    }
    if (!freeStr)
        return 0;

    memcpy(freeStr, str, len1 * sizeof(char));
    return freeStr;
}

// 释放由_copy_str返回的字符串
inline static void WINAPI _free_str(void* str)
{
    if (str)
        wstr::free(str);
    str = 0;
}

__declspec(naked) int __stdcall  call_5(LPVOID pfn, int hWnd, UINT message, WPARAM wParam, LPARAM lParam, int pData)
{
    __asm
    {
        push ebp
        mov ebp, esp
        mov eax, dword ptr[ebp + 0x08]
        mov ebx, dword ptr[ebp + 0x04]
        mov dword ptr[ebp + 0x08], ebx
        pop ebp
        pop ebx
        jmp eax
    }
}

void __stdcall _test_(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, int aa, int bb, int cc)
{

    return ;
}

int ___test()
{
    int as = call_5(_test_, 11, 22, 33, 44, 55);
    return as;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      初始化自绘库, 必须初始化, 否则调用其中一个命令都会崩溃
// 参数:
//      参数1: 易语言支持库目录
//      参数2: 是否为易语言调用, 外部调用需传递false
// 
// 返回值:
//      返回数据地址
EXTERN_C _declspec(dllexport) void* WINAPI iDraw_Init(LPCWSTR eLibName, bool isECall, HWND hEWnd)
{
    //___test();

    if (s_info) return s_info;
    s_info = &allInfo;
#define __dbg_init_(_s) 

    s_ini.m_file.reserve(520);
    s_iniColor.m_file.reserve(520);
    s_ini.m_isWrite = true;
    s_iniColor.m_isWrite = true;
    s_info->EVersion = GetEVersion();


    s_info->uMsgEWindowThreadCallback = RegisterWindowMessageW(L"{228F36F9-3246-44B5-8E2B-C513A3B7435B}");
    size_t len = wcslen(eLibName);
    memcpy(m_runPath, eLibName, len * sizeof(wchar_t) + sizeof(wchar_t));
    if (m_runPath[len - 1] != '\\')
        m_runPath[len++] = '\\';
    __dbg_init_(wstr::dbg(L"初始化1\n"));

    _make_path(m_runFile,    m_runPath, 0, 0);                                      // lib/
    _make_path(m_iDrawPath,  m_runPath, IDRAW_FNE_IDRAW, L"\\");                    // lib/iDraw/
    _make_path(m_dataPath,   m_runPath, IDRAW_FNE_IDRAW, L"\\data\\");              // lib/iDraw/data/
    _make_path(m_configPath, m_runPath, IDRAW_FNE_IDRAW, L"\\data\\config.ini");    // lib/iDraw/data/config.ini
    _make_path(m_pluginPath, m_runPath, IDRAW_FNE_IDRAW, L"\\plugin\\");            // lib/iDraw/plugin/

    _make_path(m_etpluginPath, m_runPath, IDRAW_FNE_IDRAW, L"\\eTools\\Plugin\\");  // lib/iDraw/eTools/plugin/


    __createPath(m_dataPath);
    __createPath(m_pluginPath);

    s_info->pfn_GetDllNameFromAddress = GetDllNameFromAddress;


    s_hWnds.resize(500);
    s_hWnds.resize(0);

    __dbg_init_(wstr::dbg(L"初始化2\n"));

    if (0)
    {
        HWND hMenu = GetDlgItem(hEWnd, 59419);
        if (GetDlgItem(hMenu, 59392) == 0)
            SendMessageA(hEWnd, WM_COMMAND, 59392, 0);  // 没有工具条, 触发显示标准工具条菜单
        if (GetDlgItem(hMenu, 133) == 0)
            SendMessageA(hEWnd, WM_COMMAND, 133, 0);    // 没有对齐工具条, 触发显示对齐工具条菜单
        HWND hToolNormal = GetDlgItem(hMenu, 59392);
        HWND hToolAlign = GetDlgItem(hMenu, 133);
        if (hToolNormal == 0 || hToolAlign == 0)
        {
            DWORD pid;
            GetWindowThreadProcessId(hEWnd, &pid);
            // 选中后还没显示, 那就有可能是顶层窗口, 枚举顶层窗口
            HWND hWnd = GetWindow(GetDesktopWindow(), GW_CHILD);
            wchar_t cls[260];
            while (hWnd)
            {
                DWORD pid_2;
                GetWindowThreadProcessId(hWnd, &pid_2);
                if (pid == pid_2)
                {
                    GetClassNameW(hWnd, cls, 260);
                    if (wcsnicmp_api(cls, L"Afx:", 4) == 0)
                    {
                        HWND hToolParent = GetDlgItem(hWnd, 59423);
                        if (hToolParent)
                        {
                            HWND hTool = GetWindow(hToolParent, GW_CHILD);
                            GetClassNameW(hTool, cls, 260);
                            if (wcsnicmp_api(cls, L"Toolbar", 7) == 0)
                            {
                                int id = GetDlgCtrlID(hTool);
                                if (id == 59423)
                                    hToolNormal = hTool;
                                else if (id == 133)
                                    hToolAlign = hTool;
                                if (hToolAlign && hToolNormal)
                                    break;
                            }
                        }
                    }
                }
                hWnd = GetWindow(hWnd, GW_HWNDNEXT);
            }
        }

    }

    s_hWndEWindow = hEWnd;
    s_eWnd(s_hWndEWindow);
    s_info->hEWnd                   = s_hWndEWindow;
    s_info->hWndMenu                = s_eWnd.GetMenu();
    s_info->hMenuMenu               = s_eWnd.GetMenuMenu();
    s_info->hCodeTab                = s_eWnd.GetCodeTab();
    s_info->hState                  = s_eWnd.GetState();
    s_info->hStatusBar              = s_eWnd.GetBottomStatusBar();
    s_info->hMDIClient              = s_eWnd.GetMDIClient();
    s_info->hControl                = s_eWnd.GetControl();
    s_info->hWork                   = s_eWnd.GetWork();
    s_info->hWnds                   = &s_hWnds;
    s_info->eWnd                    = &s_eWnd;
    s_info->pfnIDE                  = s_fnNotifySys;
    s_info->path_e                  = m_ePath;
    s_info->path_run                = m_runPath;
    s_info->path_lib                = m_runPath;
    s_info->path_file               = m_runFile;
    s_info->path_data               = m_dataPath;
    s_info->path_iDraw              = m_iDrawPath;
    s_info->path_plugin             = m_pluginPath;
    s_info->path_ETplugin           = m_etpluginPath;
    s_info->path_config             = m_configPath;
    s_info->pfnLoadPlugin           = _debug_load_plugin;
    s_info->pfnFreePlugin           = _debug_free_plugin;
    s_info->pfnGetInfo              = iDraw_GetInfo;
    s_info->pfnSetInfo              = iDraw_SetInfo;
    s_info->pfnPlugin_init          = Plugin_Init;
    s_info->pfnPlugin_GetInfo       = Plugin_GetInfo;
    s_info->pfnPlugin_Query         = Plugin_Query;
    s_info->pfnPlugin_UnInit        = Plugin_UnInit;
    s_info->pfnPlugin_Enum          = Plugin_Enum;
    s_info->copyStrW                = _copy_str;
    s_info->copyStrA                = _copy_strA;
    s_info->freeStr                 = _free_str;
    s_info->SetLastError            = iDraw_SetLastError;
    s_info->GetLastError            = iDraw_GetLastError;
    s_info->pfn_GetDllNameFromAddress = GetDllNameFromAddress;

    s_info->ini                     = &s_ini;
    s_info->iniColor                = &s_iniColor;
    s_info->tab.indexSel            = -1;
    s_info->tab.indexDown           = -1;
    s_info->tab.indexHot            = -1;
    s_info->tab.minWidth            = -1;
    s_info->MY_MF_OWNERDRAW         = 0;


    {
        s_info->ini->open(s_info->path_config);
        const LPCWSTR appName = L"iDraw_Debug";
        //s_info->ini->write(appName, L"开发者专用", L"调试用的, 用于帮助开发者排查错误, 如果不制作视觉库插件, 不需要修改这些配置项");
        if (s_info->ini->read_int(appName, L"Register_Msg_Evt"))
        {
            s_info->logMsgEvt = _log_dbg_output_to_file;
        }
        if (s_info->ini->read_int(appName, L"PluginInit"))
        {
            s_info->logPluginInit = _log_dbg_output_to_file;
        }
        
    }

    wstr errText;
    _PLUGIN_LOAD_INFO loadInfo[7] = { 0 };
    pfn_IDraw_Interface pfnConfig   = _load_plugin(PLUGIN_CONFIG,       loadInfo[0], true, errText);
    pfn_IDraw_Interface pfnTheme    = _load_plugin(PLUGIN_THEME,        loadInfo[1], true, errText);
    pfn_IDraw_Interface pfnEControl = _load_plugin(PLUGIN_DRAWCONTROL,  loadInfo[2], true, errText);
    pfn_IDraw_Interface pfnResource = _load_plugin(PLUGIN_RESOURCE,     loadInfo[3], true, errText);
    pfn_IDraw_Interface pfnEvent    = _load_plugin(PLUGIN_EVENT,        loadInfo[4], true, errText);
    pfn_IDraw_Interface pfnMoveSize = _load_plugin(PLUGIN_MOVEWINDOW,   loadInfo[5], true, errText);
    pfn_IDraw_Interface pfnEtools =  _load_plugin(EXTERADLL_CETOOLS, loadInfo[6], false, errText);
    // 不加载就正常, 一加载易语言就触发暗装了, 还有一个助手插件, 也不知道是哪个
    // 插件都是内存加载的, 我通过回调地址获取dll的文件名只取到了 plugin
    // guid是 125-125  一大堆125的那个
    // 有注册了一个50000 多的一个消息值, 也会崩溃, 


    if (!errText.empty())
    {
        MessageBoxW(s_hWndEWindow, errText.c_str(), IDRAW_MESSAGETITLE L"核心插件初始化失败", MB_ICONERROR);
        exit(0);
        return 0;
    }
    __dbg_init_(wstr::dbg(L"初始化3, %d, %d, %d, %d, %d, %d, %d\n", pfnConfig, pfnTheme, pfnEControl, pfnResource, pfnEvent, pfnEtools, pfnMoveSize));

    if (!pfnConfig || !pfnTheme || !pfnEControl || !pfnResource || !pfnEvent || !pfnMoveSize)
    {
        LPCWSTR text[6] = { 0 };
        text[0] = (!pfnConfig   ? PLUGIN_CONFIG         : 0);
        text[1] = (!pfnTheme    ? PLUGIN_THEME          : 0);
        text[2] = (!pfnEControl ? PLUGIN_DRAWCONTROL    : 0);
        text[3] = (!pfnResource ? PLUGIN_RESOURCE       : 0);
        text[4] = (!pfnEvent    ? PLUGIN_EVENT          : 0);
        text[5] = (!pfnMoveSize ? PLUGIN_MOVEWINDOW     : 0);
        //text[6] = (!pfnEtools   ? EXTERADLL_CETOOLS     : 0);
        wstr msgText(260);
        msgText.assign(L"核心插件被篡改, 无法启动程序, 请重新安装以下插件\n");
        int n = 0;
        for (int i = 0; i < sizeof(text) / sizeof(text[0]); i++)
        {
            if (text[i])
                msgText.append(++n).append(L". ").append(text[i]).append(L"\n");
        }
        MessageBoxW(hEWnd, msgText.c_str(), IDRAW_MESSAGETITLE L"无法启动程序", MB_ICONERROR);
        exit(0);
        return 0;
    }

    __dbg_init_(wstr::dbg(L"初始化 开始初始化核心插件\n"));
    _call_plugin(loadInfo[0], s_info->pfnConfig);  __dbg_init_(wstr::dbg(L"配置插件初始化完毕\n"));  // 0. 最先加载配置插件 
    _call_plugin(loadInfo[1], s_info->pfnTheme);   __dbg_init_(wstr::dbg(L"主题插件初始化完毕\n"));  // 1. 然后加载主题插件 
    if (!s_info->config || !s_info->defData || !s_info->pfnCreateBrush)
    {
        LPCWSTR outText = L"";
        if (!s_info->config)
            outText = PLUGIN_CONFIG;
        else
            outText = PLUGIN_THEME;

        __dbg_init_(wstr::dbg(L"配置或主题插件加载失败\n"));
        // 插件加载成功的话这几个值肯定不为0, 整个程序都会使用这几个值
        _exit_e(outText);
        return 0;
    }

    if (!isECall)
    {
        pfnEvent(s_info, s_info->eWnd, 0, 0);
        pfnEControl(s_info, s_info->eWnd, 0, 0);
        pfnResource(s_info, s_info->eWnd, 0, 0);
        if (!s_info->iDraw || !s_info->pResource || !s_info->evt.pfnGetMessageInfo
            )
        {
            _exit_e(0);
            return 0;
        }

        return s_info;
    }

    //TODO 这里改成0就是使用自绘滚动条和菜单, 非0是使用win10 1809, 17763 开始的的深色主题
    if (1)
    {
        COLORREF clr_menu_back = s_info->themeColor->crMenuBack();
        BYTE r = GetRValue(clr_menu_back);
        BYTE g = GetGValue(clr_menu_back);
        BYTE b = GetBValue(clr_menu_back);
        BOOL isDark = CThemeColor::IsArgbLight(255, r, g, b) == false;
        s_info->MY_MF_OWNERDRAW = CDarkMode::InitDarkMode(isDark) ? 0 : MF_OWNERDRAW;
        if (s_info->MY_MF_OWNERDRAW)
        {
            CDarkMode::AllowDarkModeForWindow(s_info->hEWnd, true);
            CDarkMode::RefreshTitleBarThemeColor(s_info->hEWnd);
        }
    }
    else
    {
        s_info->MY_MF_OWNERDRAW = MF_OWNERDRAW;
    }

    _call_plugin(loadInfo[2], s_info->pfnControls); __dbg_init_(wstr::dbg(L"绘画插件初始化完毕\n"));  // 2. 初始化绘画插件
    _call_plugin(loadInfo[3], s_info->pfnResource); __dbg_init_(wstr::dbg(L"资源插件初始化完毕\n"));  // 3. 初始化资源插件
    _call_plugin(loadInfo[4], s_info->pfnEvent);    __dbg_init_(wstr::dbg(L"事件插件初始化完毕\n"));  // 4. 初始化事件插件
    if (!s_info->iDraw || !s_info->d2d || !s_info->pResource || !s_info->evt.pfnGetMessageInfo)
    {
        LPCWSTR outText = L"";
        if (!s_info->iDraw || !s_info->d2d)
            outText = PLUGIN_DRAWCONTROL;
        else if (!s_info->pResource)
            outText = PLUGIN_RESOURCE;
        else if (!s_info->evt.pfnGetMessageInfo)
            outText = PLUGIN_EVENT;
        __dbg_init_(wstr::dbg(L"事件/绘画/资源 有插件初始化失败, 失败的插件 = %s\n", outText));
        // 绘画插件会给 iDraw d2d 赋值
        // 资源插件会给 pResource 赋值
        // 事件有几个接口, 只要有一个为0都是失败
        _exit_e(outText);
        return 0;
    }

    LPSYSBUTTONIMAGE pNskn = s_info->pResource->GetNskn();
    LPOBJSTRUCT pData = s_info->iDraw->_subclass_EWindow(s_hWndEWindow, m_hInst, pNskn);
    __dbg_init_(wstr::dbg(L"初始化20\n"));



    __dbg_init_(wstr::dbg(L"初始化11\n"));
    s_info->iDraw->EWindow_Hook_SetWindowsHook();   __dbg_init_(wstr::dbg(L"初始化12\n"));
    s_info->iDraw->_subclass_menu();                __dbg_init_(wstr::dbg(L"初始化13\n"));
    s_info->iDraw->_subclass_work();                __dbg_init_(wstr::dbg(L"初始化14\n"));
    s_info->iDraw->_subclass_state();               __dbg_init_(wstr::dbg(L"初始化15\n"));
    s_info->iDraw->_subclass_statusBar();           __dbg_init_(wstr::dbg(L"初始化16\n"));
    s_info->iDraw->_subclass_MDIControl();          __dbg_init_(wstr::dbg(L"初始化17\n"));
    s_info->iDraw->_subclass_control();             __dbg_init_(wstr::dbg(L"初始化18\n"));

    _call_plugin(loadInfo[5], s_info->pfnMove);     __dbg_init_(wstr::dbg(L"移动插件初始化完毕\n"));  // 6. 初始化移动插件
    if (!s_info->pfnAllWindowProc)
    {
        _exit_e(PLUGIN_MOVEWINDOW);
        return 0;
    }


    if (pfnEtools)
        _call_plugin(loadInfo[6], s_info->pfnETools);  __dbg_init_(wstr::dbg(L"助手初始化完毕\n"));  // 7. 初始化助手
    
    __dbg_init_(wstr::dbg(L"初始化完毕\n"));

    return s_info;
}

// 文件映射有失败, 应该是要结束易语言, 不允许继续执行
inline void _map_failed()
{
    m_isFirstLoad = true;
    MessageBoxW(0,
        L"初始化基础插件失败\n"
        L"请重启易语言尝试, 如果一直失败请联系006修复\n\n"
        L"本次不会启用支持库的任何功能"
        , IDRAW_MESSAGETITLE L"初始化失败", MB_ICONWARNING);

}


// 和助手互斥, 返回是否加载了助手, 已加载助手的话内部会弹窗警告, 并设置 m_isFirstLoad 为真
// m_isFirstLoad 这个值为真则支持库不做任何处理
inline bool _mutex_eTools()
{
#define PROP_GUID_NAME L"{B2FCD26A-E93C-4C05-A653-AD434D38E9D2}"

    if (fne::isFne("eTools"))
    {
        m_isFirstLoad = true;
        fne::deleteFne("eTools");
        MessageBoxW(0,
            L"从易语言支持库列表取消【易语言助手】支持库\n\n"
            L"本次不会启用支持库的任何功能"
            , IDRAW_MESSAGETITLE L"请关闭易语言助手", MB_ICONHAND);
        return true;
    }

    HMODULE hETools = GetModuleHandleW(L"eTools.fne");
    if (hETools)    // 助手已经加载, 不允许运行
    {
        m_isFirstLoad = true;
        fne::deleteFne("eTools");
        MessageBoxW(0,
            L"从易语言支持库列表取消【易语言助手】支持库\n\n"
            L"本次不会启用支持库的任何功能"
            , IDRAW_MESSAGETITLE L"请关闭易语言助手", MB_ICONHAND);
        return true;
    }

    struct MAP_FILE_DATA
    {
        DWORD header;   // 文件头
        DWORD cbSize;   // 结构尺寸
        DWORD offset;   // 数据偏移量
        DWORD pid;      // 最后一个打开文件映射的进程id
        DWORD IsIDraw;  // 是否为视觉库加载
    };

    static MAP_FILE_DATA data = { 0 };
    if (data.cbSize == 0)
    {
        data.cbSize = sizeof(data);
        data.header = MAKELONG('D', 'W');   // 5701700  DW
        data.offset = data.cbSize;
        data.pid = GetCurrentProcessId();
    }
    HWND hWnd = (HWND)s_fnNotifySys(NES_GET_MAIN_HWND, 0, 0);
    MAP_FILE_DATA* pData = (MAP_FILE_DATA*)GetPropW(hWnd, PROP_GUID_NAME);
    if (pData == &data)     // 里面存放的是这个静态变量地址, 那就表示是视觉库加载
    {
        return false;
    }

    if (pData == 0)    // 首次加载, 没有加载助手
    {
        data.IsIDraw = true;
        SetPropW(hWnd, PROP_GUID_NAME, &data);  // 没有加载助手就把地址写入, 下次在触发这个函数的时候就不走这里
        return false;
    }

    // pData 不是0, 那就表示不是首次加载, 不是视觉库加载, 那就是助手加载了
    m_isFirstLoad = true;
    MessageBoxW(0,
        L"从易语言支持库列表取消【易语言助手】支持库\n\n"
        L"本次不会启用支持库的任何功能"
        , IDRAW_MESSAGETITLE L"请关闭助手", MB_ICONWARNING);
    return true;
}



void _hook_();

INT WINAPI jy_estudio_ProcessNotifyLib(INT nMsg, DWORD dwParam1, DWORD dwParam2)
{
    INT nRet = NR_OK;
    if (m_isFirstLoad)      // 首次加载支持库的不处理任何消息
        return nRet;

    switch (nMsg)
    {
    case NL_UNLOAD_FROM_IDE:    // 当支持库在IDE中被用户取消选择后发送本通知
    case NL_FREE_LIB_DATA:      // 通知支持库释放资源准备退出及释放指定的附加数据。
    {
        if (!s_info)
            return nRet;
        for (auto it = s_plugin_info.begin(); it != s_plugin_info.end(); ++it)
        {
            PLUGIN_INFO& info = it->second;
            if (info.pfnCallback)
                info.pfnCallback(nMsg, dwParam1, dwParam2);
        }

        
        _ide_free(nMsg);

        return nRet;
    }
    case NL_SYS_NOTIFY_FUNCTION:
    {
        //MessageBoxA(0, "1231312", 0, 0);
        const bool isReturn = s_fnNotifySys != 0;
        s_fnNotifySys = (PFN_NOTIFY_SYS)dwParam1;

        if (isReturn)
            return 0;
        if (_mutex_eTools()) return nRet; // 已经加载助手则所有功能都不启用

        // 告知库通知系统用的函数指针,在装载支持库前通知,可能有多次,后通知的值应该覆盖前面所通知的值,忽略返回值。
        // 库可将此函数指针记录下来以便在需要时使用它通知信息到系统。
        // dwParam1: (PFN_NOTIFY_SYS)
        //L"2021-06-01 00:00:00";
        //const time_t t1 = time(0);
        ////time_t t2 = datetime::make_time(L"2021-06-01 00:00:00");    // 到期时间, 如果到期时间比服务器时间小, 表示此版本不能使用
        //const time_t t2 = 1626278417;    // 1626278417 = 2021-07-15 0:0:17
        //wchar_t debug_time1[50];
        //wchar_t debug_time2[50];
        //datetime::to_string(t1, debug_time1);
        //datetime::to_string(t2, debug_time2);
        //MessageBoxW(0, L"123", IDRAW_MESSAGETITLE L"重启", MB_ICONINFORMATION);

        m_isFirstLoad = fne::isFne("iDraw") == false;
        if (m_isFirstLoad)
        {
            // 第一次加载支持库
            const LPCWSTR msgText =
                L"首次加载支持库需要重新启动易语言\n"
                L"本次启动不会加载易IDE视觉库\n"
                ;
            MessageBoxW(0, msgText, IDRAW_MESSAGETITLE L"重启", MB_ICONINFORMATION);

            return 0;
        }
        //if (t2 < t1)
        //{
        //    // https://bbs.125.la/thread-14671519-1-1.html
        //    LPCWSTR text = L"请到论坛下载新版本\n下载地址为 https://bbs.125.la/thread-14671519-1-1.html\n";
        //    MessageBoxW(0, text, L"已有新版本, 更新", MB_ICONINFORMATION);
        //}

        char ePath[260];
        s_fnNotifySys(NAS_GET_PATH, 1005, (DWORD)ePath);
        if (!ePath[0])
        {
            m_isFirstLoad = true;
            // 第一次加载支持库
            const LPCWSTR msgText =
                L"获取易语言路径失败, 请重新打开易语言\n"
                L"如果一直失败请重新安装视觉库或者易语言\n"
                ;
            MessageBoxW(0, msgText, IDRAW_MESSAGETITLE L"获取易语言路径失败", MB_ICONINFORMATION);

            return 0;
        }
        wstr path = _str::A2W(ePath);
        m_ePath;
        size_t strLen = path.size();
        strLen -= 4;
        if (strLen > 520)strLen = 519;
        memcpy(m_ePath, path.c_str(), strLen * sizeof(wchar_t));
        iDraw_Init(path.c_str(), true, (HWND)s_fnNotifySys(NES_GET_MAIN_HWND, 0, 0));
        if (s_info->pfnETools)
            s_info->pfnETools(nMsg, dwParam1, dwParam2);
        iDraw_LoadPlugin();

        SetWindowPos(s_info->hControl, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
        break;
    }
    case NL_IDE_READY:
    {
        if (!s_info) return 0;
#ifndef _DEBUG
        _ide_ready();
#endif
        
        for (auto it = s_plugin_info.begin(); it != s_plugin_info.end(); ++it)
        {
            PLUGIN_INFO& info = it->second;
            if (info.pfnCallback)
                info.pfnCallback(nMsg, dwParam1, dwParam2);
        }
        if (!s_info->isReady)
        {
            RECT rc;
            UINT dpiX = s_info->dpiX, dpiY = s_info->dpiY;
            GetWindowRect(s_info->hEWnd, &rc);
            s_info->dpiX = USER_DEFAULT_SCREEN_DPI;
            s_info->dpiY = USER_DEFAULT_SCREEN_DPI;
            s_info->isReady = true;
            SendMessageA(s_info->hEWnd, WM_DPICHANGED, MAKELONG(dpiX, dpiY), (LPARAM)&rc);
        }
        UnregisterHotKey(s_info->hEWnd, 0);
        SendEWindowSize(s_info->pWnd);
        //SetWindowPos(s_info->hEWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
        // 当IDE已经准备好接受用户操作后,此通知被发送到所有具有LBS_IDE_PLUGIN标志的支持库.
        // 注意: 当用户在支持库配置对话框中取消已选择支持库或选择新的支持库后,本通知会被再次发送,支持库内需要处理好此种情况.
        break;
    }
    case NL_RIGHT_POPUP_MENU_SHOW:
    {
        // 当用户在IDE中单击鼠标右键即将弹出菜单前,此通知被发送到所有具有LBS_IDE_PLUGIN标志的支持库.
        // dwParam1: 欲弹出菜单句柄(HMENU)
        // dwParam2: 欲弹出菜单在易语言主程序(e.exe)中的资源ID
        for (auto it = s_plugin_info.begin(); it != s_plugin_info.end(); ++it)
        {
            PLUGIN_INFO& info = it->second;
            if (info.pfnCallback)
                info.pfnCallback(nMsg, dwParam1, dwParam2);
        }
        break;
    }
    case NL_ADD_NEW_ELEMENT:
    {
        // 当IDE准备显示新加入成员对话框之前,此通知被发送到所有具有LBS_IDE_PLUGIN标志的支持库.
        // 返回1表示该通知被处理且IDE根据返回内容添加所指定的新成员,返回2表示该通知被处理且IDE中止添加新成员,返回0表示未被处理.
        // dwParam1: 提供欲加入成员的类型(ADD_NEW_ELEMENT_TYPE)
        // dwParam2:
        //   如果dwParam1为NAN_NEW_VAR,则为一个指向RES_NEW_VAR_INFO的指针;
        //   如果dwParam1为NAN_NEW_SUB,则为一个指向欲加入子程序名称的文本指针;
        //   如果dwParam1为NAN_NEW_CONST_RES,则为一个指向RES_NEW_CONST_RES_INFO的指针.
        for (auto it = s_plugin_info.begin(); it != s_plugin_info.end(); ++it)
        {
            PLUGIN_INFO& info = it->second;
            if (info.pfnCallback)
                info.pfnCallback(nMsg, dwParam1, dwParam2);
        }
        break;
    }
    case NR_DELAY_FREE:
    {
        // 返回NR_DELAY_FREE表示本支持库需要被延迟释放
        for (auto it = s_plugin_info.begin(); it != s_plugin_info.end(); ++it)
        {
            PLUGIN_INFO& info = it->second;
            if (info.pfnCallback)
                info.pfnCallback(nMsg, dwParam1, dwParam2);
        }
        break;
    }
    default:
        nRet = NR_ERR;
        break;
    }

    if (s_info->pfnETools)
        s_info->pfnETools(nMsg, dwParam1, dwParam2);

    return nRet;

}



EXTERN_C _declspec(dllexport) PLIB_INFO WINAPI GetNewInf()
{
    //TODO 易语言支持库接口数据类型定义
    static LIB_INFO s_LibInfo =
    {
        LIB_FORMAT_VER,        // 库格式号，应该等于LIB_FORMAT_VER。主要用途如下：
        "{228F36F9-3246-44B5-8E2B-C513A3B7435B}",    // 对应于本库的唯一GUID串，不能为NULL或空，库的所有版本此串都应相同。
        IDRAW_MAJORVERSION,    //本库的主版本号，必须大于0。
        IDRAW_MINORVERSION,    //本库的次版本号。

        IDRAW_BUILDVERSION,    //构建版本号，无需对此版本号作任何处理。

        5,    // 所需要的易语言系统的主版本号。
        0,    // 所需要的易语言系统的次版本号。
        5,    // 所需要的系统核心支持库的主版本号。
        0,    // 所需要的系统核心支持库的次版本号。

        "\n" IDRAW_FNE_NAMEA,    // 库名，不能为NULL或空。
        __GBK_LANG_VER,                        // 库所支持的语言。
        "反馈bug请加入QQ群 955171833 进行反馈\r\n本库主要修改易语言主界面, 编译时不会写出任何代码到文件",    // 库详细解释

        _LIB_OS(OS_ALL) | LBS_IDE_PLUGIN | LBS_FUNC_NO_RUN_CODE,      //

        "揭阳精易科技有限公司",    //作者
        "515500",               //邮编
        "",                     //地址
        "",                     //联系电话
        "11903273",             //QQ
        "800073686@b.qq.com",   //邮箱
        "http://bbs.125.la/",   //主页
        "反馈群 955171833",                    //其他信息

        0,  // 本库中自定义数据类型的数目。
        0,  // 本库中所有的自定义数据类型。

        0,  // 全局命令类别数目，可为0。
        0,  // 全局命令类别说明表

        0,  // 本库中提供的所有命令（全局命令及对象命令）的数目（可为0）。
        0,  // 可为NULL
        0,  // 指向每个命令的实现代码首地址，可为NULL

        0,   // 可为NULL,易语言IDE系统添加辅助功能，所提供功能会被自动添加到IDE的“工具”菜单中。
                //     有关AddIn功能的说明，两个字符串说明一个功能。第一个为功能名称
                // (仅限一行20字符，如果希望自行初始位置而不被自动加入到工具菜单，则
                // 名称应该以@开始，此时会接收到值为 -(nAddInFnIndex + 1) 的调用通知），
                // 第二个为功能详细介绍（仅限一行60字符），最后由两个空串结束
        0,  //功能详细介绍（仅限一行60字符），最后由两个空串结束

        jy_estudio_ProcessNotifyLib,        // 不能为NULL,和易语言通讯的子程序

        NULL,   // 模板 可为NULL 有关SuperTemplate的说明，两个字符串说明一个SuperTemplate。
                // 第一个为SuperTemplate名称（仅限一行30字符），第二个为详细介绍（不限），
                // 最后由两个空串结束
        NULL,

        0,      // 本库预先定义的所有常量。
        0,

        NULL    // 本库正常运行所需要依赖的其他支持文件,可为NULL
    };

    return &s_LibInfo;
}

void _debug_load_plugin()
{
    wstr path(260);
    wstr tmpPath(260);

    path.assign(s_info->path_run).append(IDRAW_FNE_DBGPATH);
    tmpPath.append(path).append(L"iDraw_*.dll");

    WIN32_FIND_DATA fd = { 0 };
    DWORD dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM;

    HANDLE hFind = FindFirstFileW(tmpPath.c_str(), &fd);
    if (INVALID_HANDLE_VALUE == hFind) return;
    wstr fileName(260);
    do
    {
        if ((dwFileAttributes & fd.dwFileAttributes) == fd.dwFileAttributes)   // 匹配类型
        {
            fileName.assign(path).append(fd.cFileName);
            HMODULE hModule = LoadLibraryW(fileName.c_str());
            LPPLUGIN_INFO info = _load_plugin(hModule, true);
            if (info)
            {
                bool isPush = true;
                for (auto it = m_debug_module.begin(); it != m_debug_module.end(); ++it)
                {
                    LPPLUGIN_INFO info1 = *it;
                    if (info1->hModule == hModule)
                    {
                        isPush = false;
                        break;
                    }
                }
                if (isPush)
                    m_debug_module.push_back(info);
            }
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
    return;
}


void _debug_free_plugin()
{
    for (auto it = m_debug_module.begin(); it != m_debug_module.end(); ++it)
    {
        LPPLUGIN_INFO info = *it;
        if (info && info->pfnUnInterface)
        {
            HMODULE hModule = info->hModule;
            info->pfnUnInterface();
            Plugin_UnInit((LPCSTR)info->hash);  // 调用这个之后info就不能在使用了
            FreeLibrary(hModule);
        }
    }
    m_debug_module.clear();
}

void _ide_free(INT msg)
{

}
void _ide_ready()
{

}





// 从插件目录加载插件
void iDraw_LoadPlugin()
{
    //TODO 需要判断什么样的插件要加载, 应该是需要给插件签名, 从签名信息里判断是否需要加载插件

    // 从插件目录查找 iDraw_ 开头的插件, 应该考虑改个扩展名
    // 应该是加载插件的时候有配置项, 从配置项里按顺序加载插件
    wstr tmpPath(260);
    tmpPath.append(s_info->path_plugin).append(L"iDraw_*.dll");

    WIN32_FIND_DATA fd = { 0 };
    DWORD dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM;

    HANDLE hFind = FindFirstFileW(tmpPath.c_str(), &fd);
    if (INVALID_HANDLE_VALUE == hFind) return;

    wstr fileName(260);
    do
    {
        if ((fd.dwFileAttributes & dwFileAttributes) != 0)   // 匹配类型
        {
            fileName.assign(s_info->path_plugin).append(fd.cFileName);
            HMODULE hModule = LoadLibraryW(fileName.c_str());
            LPPLUGIN_INFO info = _load_plugin(hModule, false);
            if (!info)continue;

            m_module.push_back(info);
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
    return;
}


static std::vector<char*> s_hash_array;                  // 文本哈希是一块内存, 计算哈希就返回这个地址, 需要释放
inline LONG_PTR _hash_fun(const char* pstr)
{
    if (!pstr || !pstr[0])return 0;
    for (auto it = s_hash_array.begin(); it != s_hash_array.end(); ++it)
    {
        const char* p = *it;
        if (p && strcmp(p, pstr) == 0)
            return (LONG_PTR)p;
    }
    size_t len = strlen(pstr) + 1;
    char* p = new char[len];
    memcpy(p, pstr, len);
    s_hash_array.push_back(p);
    return (LONG_PTR)p;
}

// 从已经运行的文件里取PE特征, 成功返回QM_ERROR_OK, 失败返回错误码, 参考返回所有段前200个字节的集合+被签名程序文件名.扩展名, 需要调用 delete[] 释放返回值
inline int GetModuleSize(HMODULE hModule)
{
    LPBYTE hInst = (LPBYTE)hModule;
    if (!hInst)
        return 0;
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hInst;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(hInst + dosHeader->e_lfanew);

    return nt->OptionalHeader.SizeOfImage;
}

inline void CALLBACK _timer_proc_init_plugin(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    PLUGIN_INFO& info = (*((PLUGIN_INFO*)id));


    info.dllName = s_info->pfn_GetDllNameFromAddress(info.guid);
    if (!info.dllName)
    {
        info.dllName = "";
    }
    else
    {
        MEMORY_BASIC_INFORMATION mem;
        BOOL isOk = VirtualQuery(info.pfnCallback, &mem, sizeof(MEMORY_BASIC_INFORMATION));
        if (!isOk)
            isOk = VirtualQuery(info.guid, &mem, sizeof(MEMORY_BASIC_INFORMATION));
        if (!isOk)
            isOk = VirtualQuery(info.pfnMenuExt_Command, &mem, sizeof(MEMORY_BASIC_INFORMATION));
        info.hModule = (HMODULE)mem.AllocationBase;

        info.fileSize = GetModuleSize(info.hModule);
        KillTimer(hWnd, id);
    }
}

LPPLUGIN_INFO Plugin_Init(LPCSTR guid, HMODULE hModule, ETOOLS_PLUGIN_INFO* etoolsInfo, pfn_MenuExt_Command pfnMenuExt_Command, pfn_IDraw_Callback callback)
{
    if (!guid || !guid[0]) return 0;
    
    int hash = _hash_fun(guid);
    PLUGIN_INFO& info = s_plugin_info[hash];
    if (info.hash == 0)
        s_plugin_array.push_back(&info);
    s_plugin_info_hash[(int)guid] = &info;
    LPCSTR name = etoolsInfo->name; if (!name || !name[0])name = "未命名插件";
    if (!info.hModule && hModule)   // 如果之前没有初始化模块句柄, 这里就初始化一下
    {
        info.hModule = hModule;
        info.fileSize = GetModuleSize(hModule);
    }
    // 如果插件名和传递进来的插件名相同, 并且GUID相同, 那就视为同一个插件, 已经被加载了, 这里不重复加载
    if (info.name && strcmp(name, info.name) == 0 && info.guid && strcmp(info.guid, guid) == 0)
        return &info; // 重复初始化了

    info.cbSize         = etoolsInfo->cbSize;
    info.name           = name;
    info.remarks        = etoolsInfo->remarks;
    info.version        = etoolsInfo->version;
    info.author         = etoolsInfo->author;
    info.Versionlimit   = etoolsInfo->Versionlimit;
    info.Dependence     = etoolsInfo->Dependence;

    if (!hModule)
    {
        MEMORY_BASIC_INFORMATION mem;
        BOOL isOk = VirtualQuery(callback, &mem, sizeof(MEMORY_BASIC_INFORMATION));
        if (!isOk)
            isOk = VirtualQuery(guid, &mem, sizeof(MEMORY_BASIC_INFORMATION));
        if (!isOk)
            isOk = VirtualQuery(pfnMenuExt_Command, &mem, sizeof(MEMORY_BASIC_INFORMATION));
        if (!isOk)
            isOk = VirtualQuery(etoolsInfo, &mem, sizeof(MEMORY_BASIC_INFORMATION));
        hModule = (HMODULE)mem.AllocationBase;
    }

    // 下面的成员都是视觉库插件的成员
    if (pfnMenuExt_Command) info.pfnMenuExt_Command = pfnMenuExt_Command;
    info.guid               = guid;
    info.pfnCallback        = callback;
    info.hModule            = hModule;
    info.fileSize           = GetModuleSize(hModule);
    info.hash               = hash;
    info.dllName            = s_info->pfn_GetDllNameFromAddress(hModule);
    if (!info.dllName)
    {
        SetTimer(s_info->hEWnd, (ULONG_PTR)&info, 300, _timer_proc_init_plugin);
        info.dllName = "";
    }

    if (s_info->logPluginInit)
    {
        s_info->logPluginInit("初始化插件, 模块句柄 = 0x%08X, 插件名称 = %s, 插件版本 = %s, 插件说明 = %s,   module = %s, GUID = %s\r\n",
            hModule, info.name, info.version, info.remarks, info.dllName, guid);
    }
    //if (pfnMenuExt_Command)
    //{
    //    if (!info.hPopupMenu)
    //    {
    //        info.hPopupMenu = CreatePopupMenu();
    //        const int count = GetMenuItemCount(s_info->hMenuExtPopup);
    //        MENUITEMINFOA item = { 0 };
    //        item.cbSize = sizeof(item);
    //        item.fMask = MIIM_FTYPE;
    //        UINT id = GetMenuItemID(s_info->hMenuExtPopup, count - 3);
    //        int err = GetLastError();
    //        if (id == ((UINT)-1) && err == ERROR_INVALID_PARAMETER) // 获取失败, 表示是第一个加入的菜单, 直接插入到头部
    //        {
    //            InsertMenuA(s_info->hMenuExtPopup, 0, MF_POPUP | MF_BYPOSITION, (UINT_PTR)info.hPopupMenu, info.name);
    //            InsertMenuA(s_info->hMenuExtPopup, 1, MF_SEPARATOR | MF_BYPOSITION, (UINT_PTR)0, 0);
    //        }
    //        else
    //        {
    //            InsertMenuA(s_info->hMenuExtPopup, count - 3, MF_POPUP | MF_BYPOSITION, (UINT_PTR)info.hPopupMenu, info.name);
    //        }
    //    }
    //}
    return &info;
}
LPPLUGIN_INFO Plugin_GetInfo(LPCSTR guid)
{
    if (!guid || !guid[0]) return 0;
    int hash = _hash_fun(guid);
    auto it = s_plugin_info.find(hash);
    if (it == s_plugin_info.end())
        return 0;
    return &(it->second);
}

LPPLUGIN_INFO Plugin_Query(LPCSTR hash)
{
    if (!hash) return 0;
    auto it = s_plugin_info_hash.find((int)hash);
    if (it == s_plugin_info_hash.end())
        return 0;
    return it->second;
}

int Plugin_Enum(LPPLUGIN_INFO* pInfo, int bufCount)
{
    if (!pInfo || bufCount <= 0) return (int)s_plugin_info.size();
    int i = 0;
    for (auto it = s_plugin_info.begin(); it != s_plugin_info.end(); ++it)
    {
        if (i + 1 >= bufCount)
            break;
        pInfo[i++] = &(it->second);
    }
    return i;
}

// 释放指定区域的地址
template<typename T>
inline int ScrollCallback_free(T& set, LPBYTE pBegin, LPBYTE pEnd)
{
    int count = 0;
    for (auto it = set.begin(); it != set.end(); it++)
    {
        LPBYTE pfn = (LPBYTE)(*it);
        if (pfn >= pBegin && pfn < pEnd)
        {
            set.erase(it++);
            count++;
            if (it == set.end())
                break;
        }
    }
    return count;
};

bool Plugin_UnInit(LPCSTR guid)
{
    if (!guid || !guid[0]) return false;
    int hash = _hash_fun(guid);
    auto it = s_plugin_info.find(hash);
    if (it == s_plugin_info.end())
        return false;
    LPPLUGIN_INFO info = &(it->second);

    LPBYTE pBegin = (LPBYTE)info->hModule;
    if (info->fileSize == 0)
        info->fileSize = GetModuleSize(info->hModule);
    LPBYTE pEnd = pBegin + info->fileSize;

    if (info->hPopupMenu)   // 有菜单项, 需要删除
    {
        //RemoveMenu(s_info->hMenuExtPopup, (UINT)info->hPopupMenu, MF_BYCOMMAND);
        DeleteMenu(s_info->hMenuExtPopup, (UINT)info->hPopupMenu, MF_BYCOMMAND);
        info->hPopupMenu = 0;
    }

    SCROLL_CALLBACK* pArr = (SCROLL_CALLBACK*)GetPropW(s_info->hEWnd, GUID_SCROLL_CALLBACK_EWINDOW);
    if (pArr)
    {
        for (auto it = pArr->begin(); it != pArr->end(); ++it)
        {
            SCROLL_CALLBACK_SET_DATA& data = it->second;
            ScrollCallback_free(*data.pCalc, pBegin, pEnd);
            ScrollCallback_free(*data.pDraw, pBegin, pEnd);
            ScrollCallback_free(*data.pGet, pBegin, pEnd);
        }

    }

    s_info->pfnControls(IDC_IDRAW_UNINLINEHOOK_PLUGIN, (WPARAM)pBegin, (LPARAM)pEnd);

    if (info->pToolButton)  // 有工具条按钮, 需要删除
    {
        for (int i = 0; i < info->nToolButtonCount; i++)
        {
            IDRAW_STRUCT_TOOL_INSERTEX& data = info->pToolButton[i];
            TBBUTTON tb = { 0 };
            int index = (int)SendMessageW(s_info->hWndTool, TB_COMMANDTOINDEX, (WPARAM)&data, 0);
            BOOL isOk = SendMessageW(s_info->hWndTool, TB_DELETEBUTTON, (WPARAM)index, 0);
            tb.bReserved[0] = isOk;
        }
        delete[] info->pToolButton;
        info->pToolButton = 0;
        info->nToolButtonCount = 0;
        info->nToolButtonBufferCount = 0;
        MoveWindow(s_info->hWndTool, 1, 1, 1, 1, true);
    }
    s_info->evt.pfnUnRegister_Message(guid);    // 释放已经注册的消息
    s_info->evt.pfnUnRegister_Event(guid);      // 释放已经注册的事件
    s_info->pResource->UnBindWindow(guid);      // 释放已经绑定的窗口
    s_plugin_info_hash.erase((int)info->guid);
    s_plugin_info.erase(it);
    return true;
}

static wstr s_error;
static _str s_errorA;
void iDraw_SetLastError(LPCWSTR fmt, ...)
{
    va_list aptr;
    va_start(aptr, fmt);
    s_error.FormatEx(260, fmt, aptr);
    va_end(aptr);
    s_errorA = s_error.w2a();
}
EXTERN_C _declspec(dllexport) LPCWSTR WINAPI iDraw_GetLastError()
{
    return s_error.c_str();
}
EXTERN_C _declspec(dllexport) LPCSTR WINAPI iDraw_GetLastErrorA()
{
    return s_errorA.c_str();
}


