// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "resource.h"
#include <map>
#include <string>
#include <Commdlg.h>

#define MENU_ITEMHEIGHT 300
#define SCROLL_BUTTON_WIDTH 60
#define RGB2ARGB(_cr,a)     ((DWORD)(BYTE)(a) << 24) | ((_cr & 0x000000ff) << 16) | ((_cr & 0x0000ff00)) | ((_cr & 0x00ff0000) >> 16)
#define ARGB2RGB(_cr)       ((COLORREF)( ((BYTE)((_cr & 0xff0000) >> 16)|((WORD)((BYTE)((_cr & 0xff00) >> 8))<<8)) | (((DWORD)(BYTE)(_cr & 0xff))<<16) ))


HINSTANCE g_hInst;
LPIDRAW_INFO g_info;
ID2DDraw* g_dx;
IDRAW_HRESOURCE g_hResource;    // 工具条资源句柄

struct SCROLL_DRAW_DATA
{
    STATE state;
    RECT rc;
};
static IGetEWindow* g_eWnd;
static HWND m_hDlg;
std::wstring Dialog_Open(LPCWSTR filter = 0, LPCWSTR title = 0, LPCWSTR defName = 0, LPCWSTR path = 0, std::wstring* fileName = 0, bool notChengedPath = false);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

LPWINDOW_ICON_DATA IDRAW_CALL callback_GetWindowIcon(HWND hWnd, IDRAW_HRESOURCE hResource, LPCWSTR pszID, void* pArg, RECT_F* prc, STATE state, UINT dpi)
{

    return 0;
}


static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        m_hDlg = hDlg;
        return (INT_PTR)TRUE;
    }
    case WM_CLOSE:
    {
        DestroyWindow(hDlg);
        break;
    }

    case WM_COMMAND:
    {
        const int id = LOWORD(wParam);
        switch (id)
        {
        case IDC_BUTTON1:
        {
            std::wstring path = Dialog_Open(L"IRES文件\0*.ires");
            if (path.empty())
                break;

            IDRAW_HRESOURCE hResource = iDraw_LoadExtFromFile(path.c_str());
            wchar_t text[100];
            swprintf_s(text, 100, L"从文件加载资源成功, 资源句柄 = 0x%p", hResource);
            MessageBoxW(0, text, 0, 0);
            break;
        }
        case IDC_BUTTON2:
        {
            std::wstring path = Dialog_Open(L"IRES文件\0*.ires");
            if (path.empty())
                break;

            FILE* f;
            errno_t err = _wfopen_s(&f, path.c_str(), L"rb+");
            if (err == 0 && f)
            {
                fseek(f, 0, SEEK_END);
                long size = ftell(f);
                fseek(f, 0, SEEK_SET);
                void* p = new BYTE[size];
                size = (long)fread_s(p, size, 1, size, f);
                fclose(f);

                IDRAW_HRESOURCE hResource = iDraw_LoadExtFromMemory(p, size);
                wchar_t text[100];
                swprintf_s(text, 100, L"从内存加载资源成功, 资源句柄 = 0x%p", hResource);
                MessageBoxW(0, text, 0, 0);

                delete[] p;
            }
            break;
        }
        case IDC_BUTTON3:
        {
             IDRAW_HRESOURCE hResource = iDraw_GetDefControlIcon(g_eWnd->GetMenuStandard());
             wchar_t text[100];
             swprintf_s(text, 100, L"获取默认资源, 资源句柄 = 0x%p", hResource);
             MessageBoxW(0, text, 0, 0);
             break;
        }
        case IDC_BUTTON4:
        {
            pfn_GetWindowIcon pfn = 0;
            IDRAW_HRESOURCE hResource = iDraw_GetResourceFromHwnd(DLL_GUID, m_hDlg, &pfn);
            wchar_t text[100];
            swprintf_s(text, 100, L"从绑定窗口获取资源, 资源句柄 = 0x%p, 绑定函数 = 0x%p", hResource, pfn);
            MessageBoxW(0, text, 0, 0);
            break;
        }
        case IDC_BUTTON5:
        {
            IDRAW_HRESOURCE hResource = iDraw_GetDefControlIcon(g_eWnd->GetMenuStandard());
            bool ret = iDraw_BindWindow(DLL_GUID, m_hDlg, hResource, callback_GetWindowIcon);
            wchar_t text[100];
            swprintf_s(text, 100, L"绑定窗口资源, 资源句柄 = 0x%p, 绑定结果 = %d", hResource, ret);
            MessageBoxW(0, text, 0, 0);
            break;
        }
        case IDC_BUTTON6:
        {
            iDraw_UnBindWindow(DLL_GUID);
            break;
        }
        case IDC_BUTTON7:
        {
            RECT_F rc;
            IDRAW_HRESOURCE hResource = iDraw_GetDefControlIcon(g_eWnd->GetMenuStandard());
            LPWINDOW_ICON_DATA hIcon = iDraw_GetIcon(hResource, L"32854", &rc, STATE::NORMAL, 96);
            wchar_t text[100];
            swprintf_s(text, 100, L"从资源获取图标, 资源句柄 = 0x%p, 图标数据 = 0x%p", hResource, hIcon);
            MessageBoxW(0, text, 0, 0);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        return (INT_PTR)FALSE;
    }
    return (INT_PTR)FALSE;
}

// 与支持库交互的回调函数
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{

    return 0;
}

// 扩展菜单被选择, 一个是菜单ID, 一个是添加到菜单时的项目数值
int CALLBACK IDraw_MenuCommand(int id, LPARAM param)
{
    // 获取左边工作夹支持库树型框的窗口数据

    switch (id)
    {
    case 1:
        // DialogBoxParamW
        m_hDlg = CreateDialogParamW(g_hInst, MAKEINTRESOURCEW(IDD_DIALOG1), g_eWnd->GetEWindow(), DlgProc, 0);
        ShowWindow(m_hDlg, SW_SHOW);
        break;
    default:
        break;
    }
    return 0;
}
extern "C" _declspec(dllexport) void* IDRAW_CALL IDraw_Interface(LPIDRAW_INFO pInfo, IGetEWindow * eWnd, int reserved1, int reserved2)
{
    g_eWnd = eWnd;
    g_info = pInfo;
    g_dx = iDraw_GetDxObject();
    g_hResource = iDraw_GetDefControlIcon(eWnd->GetMenuStandard());     // 工具条按钮比较丰富, 就选工具条了

    static PLUGIN_INFO pluginInfo = { 0 };

    pluginInfo.cbSize = sizeof(pluginInfo);
    pluginInfo.name = "插件开发系列教程 c++版";
    pluginInfo.remarks = "演示用, 插件开发教程";
    pluginInfo.version = "1.0";
    pluginInfo.author = "项目部006";
    pluginInfo.Versionlimit = 0;
    pluginInfo.Dependence = 0;


    pluginInfo.guid = DLL_GUID;
    pluginInfo.pfnMenuExt_Command = IDraw_MenuCommand;
    pluginInfo.pfnCallback = IDraw_NotifyCallback;
    iDraw_InitPlugin(pluginInfo.guid, g_hInst, &pluginInfo, pluginInfo.pfnMenuExt_Command, pluginInfo.pfnCallback); // 初始化插件

    UINT uFlags = MF_STRING | MF_BYPOSITION;
    iDraw_MenuExt_Insert(DLL_GUID, -1, uFlags, 1, L"载入窗口", 0);

    m_hDlg = CreateDialogParamW(g_hInst, MAKEINTRESOURCEW(IDD_DIALOG1), g_eWnd->GetEWindow(), DlgProc, 0);
    ShowWindow(m_hDlg, SW_SHOW);

    return &pluginInfo;
}

extern "C" __declspec(dllexport) void IDRAW_CALL IDraw_UnInterface()
{

}


// 对话框_打开文件
// filter = 过滤器, 格式：“MP3文件(*.mp3)\0*.mp3\0媒体文件(*.mpg)\0*.mpg\0\0”
// defName = 默认文件名
// title = 窗口标题
// path = 运行目录
// fileName = 接受文件名
// notChengedPath = 不改变目录
std::wstring Dialog_Open(LPCWSTR filter, LPCWSTR title, LPCWSTR defName, LPCWSTR path, std::wstring* fileName, bool notChengedPath)
{
    if (!title) title = L"打开文件";
    WCHAR file[MAX_PATH] = { 0 };
    WCHAR FileTitle[MAX_PATH] = { 0 };
    //memcpy(FileTitle, g_runPath, MAX_PATH * sizeof(WCHAR));
    if (defName)
        memcpy(file, defName, wcslen(defName) * sizeof(wchar_t) + sizeof(wchar_t));

    if (!filter) filter = L"所有文件(*.*)\0*.*";

    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.hInstance = 0;
    ofn.lpstrFilter = filter;
    ofn.lpstrCustomFilter = 0;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = FileTitle;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrInitialDir = path;
    ofn.lpstrTitle = title;

    ofn.Flags = OFN_EXPLORER;    // OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (notChengedPath) ofn.Flags |= OFN_NOCHANGEDIR;
    BOOL b = GetOpenFileNameW(&ofn);
    if (!b)
        return L"";
    if (fileName)fileName->assign(FileTitle);
    return file;
}