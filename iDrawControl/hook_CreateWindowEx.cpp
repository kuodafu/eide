#include "EWindow_Fne_Header.h"
//#include <hook_detours/ApiHook.h>
#include <CDarkMode.h>
#include <CThemeColor.h>
#include <vssym32.h>
#pragma comment(lib, "Uxtheme.lib")
#define WNDPROC_PROP L"{0E9D5C7A-C113-4B25-B97A-71B2D7F77023}"

LRESULT CALLBACK _hook_lv_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


enum WND_TYPE
{
    WND_TYPE_NONE               = 0x0000,   // 未知类型, 看怎么显示默认类型, 这个不知道如何获取信息
    WND_TYPE_BUTTON             = 0x0001,   // BS
    WND_TYPE_STATIC             = 0x0002,   // SS
    WND_TYPE_LISTBOX            = 0x0003,   // LBS
    WND_TYPE_COMBOBOX           = 0x0004,   // CBS
    WND_TYPE_SCROLLBAR          = 0x0005,   // SBS
    WND_TYPE_EDIT               = 0x0006,   // ES
    WND_TYPE_TREEVIEW           = 0x0007,   // TVS
    WND_TYPE_LISTVIEW           = 0x0008,   // LVS
    WND_TYPE_TAB                = 0x0009,   // TCS 选择夹
    WND_TYPE_MONTHCAL           = 0x000A,   // MCS 月历框
    WND_TYPE_DATETIMEPICK       = 0x000B,   // DTS 日期时间控件"SysDateTimePick32"
    WND_TYPE_HEADER             = 0x000C,   // HDS
    WND_TYPE_TOOL               = 0x000D,   // BTNS 工具条
    WND_TYPE_REBARWINDOW        = 0x000E,   // RBS 钢筋控件
    WND_TYPE_TOOLTIPS           = 0x000F,   // TTS tooltips_class,工具提示
    WND_TYPE_STATUSBAR          = 0x0010,   // SBARS 状态条
    WND_TYPE_TRACKBAR           = 0x0011,   // TBS 滑块条
    WND_TYPE_UPDOWN             = 0x0012,   // UDS 调节器
    WND_TYPE_PROGRESS           = 0x0013,   // PBS 进度条
    WND_TYPE_HOTKEY             = 0x0014,   // CCS 热键框
    WND_TYPE_SYSLINK            = 0x0015,   // LWS 链接框
    WND_TYPE_COMBOBOXEX         = 0x0016,   // CBES 组合框扩展
    WND_TYPE_ANIMATE            = 0x0017,   // ACS 动画框
    WND_TYPE_PAGER              = 0x0018,   // PGS 寻呼机?, 不知道叫什么控件  
    WND_TYPE_NATIVEFONTCTL      = 0x0019,   // NFS
    WND_TYPE_TASKDIALOGL        = 0x0020,   // TaskDialog

};
struct WND_CLASS
{
    LPCWSTR pszClass;
    WND_TYPE type;
};

// 记录hook创建窗口的句柄和组件信息
struct HOOK_DARK_WND
{
    HWND hWnd;
    WND_TYPE type;
    UINT dpi;
    bool isSubClass;
    COLORREF headerTextColor;
    WNDPROC oldProc;
    int ref_count;
    HOOK_DARK_WND()
    {
        clear();
        ref_count = 1;
    }
    ~HOOK_DARK_WND()
    {
        Release();
    }
    void clear()
    {
        hWnd = 0;
        type = WND_TYPE_NONE;
        dpi = 96;
        headerTextColor = 96;
        isSubClass = false;
        oldProc = 0;
    }
    int Addref()
    {
        return ++ref_count;
    }
    int Release()
    {
        int ret = --ref_count;
        if (ret == 0)
            delete this;
        return ret;
    }
};

typedef decltype(GetOpenFileNameA)* PFN_GetOpenFileNameA;
typedef decltype(GetOpenFileNameW)* PFN_GetOpenFileNameW;
typedef decltype(GetSaveFileNameA)* PFN_GetSaveFileNameA;
typedef decltype(GetSaveFileNameW)* PFN_GetSaveFileNameW;
typedef decltype(CreateWindowExA)* PFN_CreateWindowExA;
typedef decltype(CreateWindowExW)* PFN_CreateWindowExW;
static PFN_CreateWindowExA  CreateWindowExA_hook  = CreateWindowExA;
static PFN_CreateWindowExW  CreateWindowExW_hook  = CreateWindowExW;
static PFN_GetOpenFileNameA GetOpenFileNameA_hook = GetOpenFileNameA;
static PFN_GetOpenFileNameW GetOpenFileNameW_hook = GetOpenFileNameW;
static PFN_GetSaveFileNameA GetSaveFileNameA_hook = GetSaveFileNameA;
static PFN_GetSaveFileNameW GetSaveFileNameW_hook = GetSaveFileNameW;

static bool m_isDeal;
static std::vector<HOOK_DARK_WND*> m_arr_darkWnd; // 记录所有设置了主题的窗口句柄, 每次设置的时候都会检查已经销毁的窗口, 主题改变的时候会清除全部失效窗口

static HWND WINAPI MyCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle,
                                     int X, int Y, int nWidth, int nHeight,
                                     HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = CreateWindowExW_hook(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (0)
    {
        if ((ULONG)lpClassName < 0xffff)
            lpClassName = 0;
        wstr::dbg(L"hWnd = %p, lpClassName = %s, lpWindowName = %s, hWndParent = %p, hInstance = %p, lpParam = %p\n",
                  hWnd, lpClassName, lpWindowName, hWndParent, hInstance, lpParam);
    }
    if (!hWnd)
        return hWnd;
    if (m_isDeal)
        _hook_deal_wnd(hWnd, lpClassName);
    return hWnd;
}
static HWND WINAPI MyCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle,
                                     int X, int Y, int nWidth, int nHeight,
                                     HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hWnd = CreateWindowExA_hook(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd)
        return hWnd;
    wstr cls = _str::A2W(lpClassName);
    _hook_deal_wnd(hWnd, cls.c_str());
    return hWnd;

    wstr name = _str::A2W(lpWindowName);
    return MyCreateWindowExW(dwExStyle, cls.c_str(), name.c_str(), dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

BOOL  WINAPI MyGetOpenFileNameA(LPOPENFILENAMEA pArg)
{
    m_isDeal = false;
    BOOL ret = GetOpenFileNameA_hook(pArg);
    m_isDeal = true;
    return ret;
}
BOOL  WINAPI MyGetOpenFileNameW(LPOPENFILENAMEW pArg)
{
    m_isDeal = false;
    BOOL ret = GetOpenFileNameW_hook(pArg);
    m_isDeal = true;
    return ret;
}
BOOL  WINAPI MyGetSaveFileNameA(LPOPENFILENAMEA pArg)
{
    m_isDeal = false;
    BOOL ret = GetSaveFileNameA_hook(pArg);
    m_isDeal = true;
    return ret;
}
BOOL  WINAPI MyGetSaveFileNameW(LPOPENFILENAMEW pArg)
{
    m_isDeal = false;
    BOOL ret = GetSaveFileNameW_hook(pArg);
    m_isDeal = true;
    return ret;
}
template<typename T>
static void _hook_creatw_wnd_(T& fnOld, LPVOID fnNew)
{
    PVOID pOld = (PVOID)fnOld;
    LPVOID funs[2] = { pOld, fnNew };
    fnOld = (T)IDraw_NotifyCallback(IDC_INLINEHOOK, (WPARAM)1, (LPARAM)funs);
}

void _hook_CreateWindowEx()
{
    static bool first;
    if (!first)
    {
        first = true;
        m_isDeal = true;
#define _HOOK_(_fun) _hook_creatw_wnd_(_fun##_hook, My##_fun)
        _HOOK_(CreateWindowExA);
        _HOOK_(CreateWindowExW);
        _HOOK_(GetOpenFileNameA);
        _HOOK_(GetOpenFileNameW);
        _HOOK_(GetSaveFileNameA);
        _HOOK_(GetSaveFileNameW);
#undef _HOOK_

    }
}
void _Unhook_CreateWindowEx()
{
    if (CreateWindowExA_hook)
    {
        IDraw_NotifyCallback(IDC_IDRAW_UNINLINEHOOK, (WPARAM)CreateWindowExA_hook, (LPARAM)0);
        IDraw_NotifyCallback(IDC_IDRAW_UNINLINEHOOK, (WPARAM)CreateWindowExW_hook, (LPARAM)0);
    }
    CreateWindowExA_hook = 0;
    CreateWindowExW_hook = 0;
}

void _hook_deal_wnd(HWND hWnd, LPCWSTR lpClassName)
{
    //return;
    // 易语言设计器窗口, 如果窗口是设计器里的窗口, 那就不处理
    HWND hDesigner = 0;
    HWND hWndMDIClient = s_info->eWnd->GetMDIClient();
    if (GetDlgCtrlID(hWnd) == 0x4BE)
    {
        HWND hParent = GetParent(hWnd);
        if (GetDlgCtrlID(hParent) == 1)
        {
            hParent = GetParent(hParent);
            if (hParent && GetDlgCtrlID(hParent) == 0xE900)
            {
                hParent = GetParent(hParent);
                if (hParent)
                {
                    hParent = GetParent(hParent);
                    if (hParent == hWndMDIClient)
                    {
                        return;
                    }
                }

            }
        }
    }

    if (s_info && s_info->eWnd)
    {
        HWND hChild = GetWindow(hWndMDIClient, GW_CHILD);
        while (hChild)
        {
            HWND hMDI2 = GetDlgItem(hChild, 0xE900);
            if (hMDI2)
            {
                HWND hMDI3 = GetDlgItem(hMDI2, 1);
                if (hMDI3)
                {
                    hDesigner = GetDlgItem(hMDI3, 0x75);
                    if (hDesigner)
                        break;
                }
            }
            hChild = GetWindow(hChild, GW_HWNDNEXT);
        }
    }

    if (hDesigner)
    {
        HWND hParent = hWnd;
        do
        {
            if (hParent == hDesigner)
                return; // 是设计器里的窗口, 那就返回, 不处理
            hParent = GetParent(hParent);
        } while (hParent);
    }

    wchar_t cls_buf[260] = { 0 };
    if ((ULONG)lpClassName < 0xffff || !lpClassName || !*lpClassName)
    {
        GetClassNameW(hWnd, cls_buf, 260);
        lpClassName = cls_buf;
    }
    const WND_CLASS pszClass[] =
    {
        { L"SysListView32",     WND_TYPE_LISTVIEW       },
        //{ L"SysTreeView32",     WND_TYPE_TREEVIEW       },
        //{ L"ListBox",           WND_TYPE_LISTBOX        },
        { L"ComboBox",          WND_TYPE_COMBOBOX       },
        { L"Edit",              WND_TYPE_EDIT           },
        //{ L"ComboBoxEx",        WND_TYPE_COMBOBOXEX     },
        //{ L"扩大福QQ121007124",  WND_TYPE_NONE           },
        //{ L"button",            WND_TYPE_BUTTON         },
        //{ L"SysTabControl32",   WND_TYPE_TAB            },
        //{ L"msctls_hotkey",     WND_TYPE_HOTKEY         },

        //{ L"SysDateTimePick32", WND_TYPE_DATETIMEPICK   },
        //{ L"SysMonthCal32",     WND_TYPE_MONTHCAL       },
        //{ L"SysLink",           WND_TYPE_SYSLINK        },
        //{ L"SysAnimate32",      WND_TYPE_ANIMATE        },
        //{ L"SysPager",          WND_TYPE_PAGER          },
        //{ L"NativeFontCtl",     WND_TYPE_NATIVEFONTCTL  },
        //{ L"msctls_statusbar",  WND_TYPE_STATUSBAR      },
        //{ L"msctls_trackbar",   WND_TYPE_TRACKBAR       },
        //{ L"msctls_updown",     WND_TYPE_UPDOWN         },
        //{ L"msctls_progress",   WND_TYPE_PROGRESS       },
        //{ L"ToolbarWindow32",   WND_TYPE_TOOL           },
        //{ L"ReBarWindow",       WND_TYPE_REBARWINDOW    },
        //{ L"tooltips_class",    WND_TYPE_TOOLTIPS       },
        { L"SysHeader32",       WND_TYPE_HEADER         },
        { L"TaskDialog" ,       WND_TYPE_TASKDIALOGL    },
    };



    WND_TYPE wndType = WND_TYPE_NONE;
    bool isSet = false;
    COLORREF clr_menu_back = s_info->themeColor->crMenuBack();
    BYTE r = GetRValue(clr_menu_back);
    BYTE g = GetGValue(clr_menu_back);
    BYTE b = GetBValue(clr_menu_back);
    BOOL isDark = CThemeColor::IsArgbLight(255, r, g, b) == false;
    LPCWSTR pszTheme = isDark ? L"DarkMode_Explorer" : L"Explorer";

    for (WND_CLASS cls : pszClass)
    {
        if (lpClassName && HIWORD(lpClassName) > 0 && _wcsicmp(cls.pszClass, lpClassName) == 0)
        {
            isSet = true;
            wndType = cls.type;
            switch (wndType)
            {
            case WND_TYPE_NONE:
                break;
            case WND_TYPE_BUTTON:
                break;
            case WND_TYPE_STATIC:
                break;
            case WND_TYPE_LISTBOX:
                break;
            //case WND_TYPE_COMBOBOX:
            //    pszTheme = L"DarkMode_CFD::Combobox";
            //    break;
            //case WND_TYPE_SCROLLBAR:
            //    break;
            //case WND_TYPE_EDIT:
            //    pszTheme = L"DarkMode_CFD::Edit";
            //    break;
            case WND_TYPE_TREEVIEW:
                break;
            case WND_TYPE_LISTVIEW:
                break;
            case WND_TYPE_TAB:
                break;
            case WND_TYPE_MONTHCAL:
                break;
            case WND_TYPE_DATETIMEPICK:
                break;
            case WND_TYPE_HEADER:
                break;
            case WND_TYPE_TOOL:
                break;
            case WND_TYPE_REBARWINDOW:
                break;
            case WND_TYPE_TOOLTIPS:
                break;
            case WND_TYPE_STATUSBAR:
                break;
            case WND_TYPE_TRACKBAR:
                break;
            case WND_TYPE_UPDOWN:
                break;
            case WND_TYPE_PROGRESS:
                break;
            case WND_TYPE_HOTKEY:
                break;
            case WND_TYPE_SYSLINK:
                break;
            case WND_TYPE_COMBOBOXEX:
                break;
            case WND_TYPE_ANIMATE:
                break;
            case WND_TYPE_PAGER:
                break;
            case WND_TYPE_NATIVEFONTCTL:
                break;
            case WND_TYPE_TASKDIALOGL:
                pszTheme = L"DarkMode_CFD::TaskDialog";
                break;
            default:
                break;
            }
            break;
        }
    }
    if (!isDark)
        pszTheme = L"Explorer";
    SetWindowTheme(hWnd, pszTheme, NULL);


    //if (isSet)
    {
        HOOK_DARK_WND* pWndInfo = 0;

        // 在数组里找个位置保存这个窗口句柄
        for (HOOK_DARK_WND*& wnd_info : m_arr_darkWnd)
        {
            if (!wnd_info->hWnd || !IsWindow(wnd_info->hWnd))
            {
                wnd_info->clear();
                wnd_info->hWnd = hWnd;
                wnd_info->type = wndType;
                if (!pWndInfo)
                    pWndInfo = wnd_info;
            }
            if (wnd_info->hWnd == hWnd)
            {
                pWndInfo = wnd_info;    // 重复的窗口重复使用, 不能让数组记录同一个窗口两次
                break;
            }
        }

        // 没有值表示原数组里没有空位置, 需要添加一个新的
        if (!pWndInfo)
        {
            HOOK_DARK_WND* wnd_info = new HOOK_DARK_WND;
            m_arr_darkWnd.push_back(wnd_info);
            pWndInfo = wnd_info;
            wnd_info->hWnd = hWnd;
            wnd_info->type = wndType;
        }

        if (!pWndInfo->isSubClass && pWndInfo->type == WND_TYPE_LISTVIEW)
        {
            pWndInfo->Addref();
            pWndInfo->isSubClass = true;
            SetPropW(pWndInfo->hWnd, WNDPROC_PROP, (HANDLE)pWndInfo);
            pWndInfo->oldProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)_hook_lv_proc);

        }
        //PCTHEMECOLOR_STRUCT pClr = g_var.color->GetClr();
        //pWndInfo->dpi = GetDpiForWindow_d(pWndInfo->hWnd);
        //SetWindowColor(*pWndInfo, pClr, true);
    }

}

void _hook_ThemeChange(bool isDark)
{
    for (HOOK_DARK_WND* wnd_info : m_arr_darkWnd)
    {
        if (!wnd_info->hWnd || !IsWindow(wnd_info->hWnd))
        {
            wnd_info->hWnd = 0;
            wnd_info->type = WND_TYPE_NONE;
            continue;
        }
        LPCWSTR pszTheme = isDark ? L"DarkMode_Explorer" : L"Explorer";

        //if (wnd_info->type == WND_TYPE_HEADER || wnd_info->type == WND_TYPE_LISTVIEW)
        //{
        //    pszTheme = L"ItemsView";
        //}
        SetWindowTheme(wnd_info->hWnd, pszTheme, NULL);
    }
}



LRESULT CALLBACK _hook_lv_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HOOK_DARK_WND* pWndInfo = reinterpret_cast<HOOK_DARK_WND*>(GetPropW(hWnd, WNDPROC_PROP));
    WNDPROC oldProc = pWndInfo ? pWndInfo->oldProc : DefWindowProcW;
    switch (message)
    {
    case WM_NOTIFY:
    {
        if (reinterpret_cast<LPNMHDR>(lParam)->code == NM_CUSTOMDRAW)
        {
            LPNMCUSTOMDRAW nmcd = reinterpret_cast<LPNMCUSTOMDRAW>(lParam);
            switch (nmcd->dwDrawStage)
            {
            case CDDS_PREPAINT:
                return CDRF_NOTIFYITEMDRAW;
            case CDDS_ITEMPREPAINT:
            {
                SetTextColor(nmcd->hdc, pWndInfo->headerTextColor);
                return CDRF_DODEFAULT;
            }
            }
        }
    }
    break;
    case WM_THEMECHANGED:
    {
        if (CDarkMode::darkModeSupported())
        {
            HWND hHeader = ListView_GetHeader(hWnd);

            CDarkMode::AllowDarkModeForWindow(hWnd, CDarkMode::darkModeEnabled());
            CDarkMode::AllowDarkModeForWindow(hHeader, CDarkMode::darkModeEnabled());

            HTHEME hTheme = OpenThemeData(nullptr, L"ItemsView");
            if (hTheme)
            {
                COLORREF color;
                if (SUCCEEDED(GetThemeColor(hTheme, 0, 0, TMT_TEXTCOLOR, &color)))
                {
                    ListView_SetTextColor(hWnd, color);
                }
                if (SUCCEEDED(GetThemeColor(hTheme, 0, 0, TMT_FILLCOLOR, &color)))
                {
                    ListView_SetTextBkColor(hWnd, color);
                    ListView_SetBkColor(hWnd, color);
                }
                CloseThemeData(hTheme);
            }

            hTheme = OpenThemeData(hHeader, L"Header");
            if (hTheme)
            {
                GetThemeColor(hTheme, HP_HEADERITEM, 0, TMT_TEXTCOLOR, &(pWndInfo->headerTextColor));
                CloseThemeData(hTheme);
            }

            SendMessageW(hHeader, WM_THEMECHANGED, wParam, lParam);

            RedrawWindow(hWnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
        }
    }
    break;
    case WM_DESTROY:
    {
        pWndInfo->Release();
        SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
        RemovePropW(hWnd, WNDPROC_PROP);
    }
    break;
    }
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}


