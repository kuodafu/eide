#include "EMenu.h"
#include <vector>
#include <set>
#include <hook_detours/apiHook.h>
#include <shlobj_core.h>
#include <shlwapi.h>


#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")
#include <stack>


//菜单ID定义
#define	MEMU_COMPILE			0x8053	//易语言自身的"编译"菜单ID(32851)
#define	MEMU_STARTCOMPILE		0x8090	//易语言自身的"静态编译"菜单ID(32912)
#define	MEMU_EPKCOMPILE			0x80D4	//易语言自身的"编译为易包(EPK)"菜单ID(32980)
#define	MEMU_CALLWIZARD			0x80A2	//易语言自身的"执行易向导"菜单ID(32930)

#define	MEMU_COMPILE_BM_WIN32	0x9001	//新增加的"WIN32"菜单ID
#define	MEMU_COMPILE_BM_WINCE	0x9002	//新增加的"WINCE"菜单ID
#define	MEMU_COMPILE_BM_EFNE	0x9003	//新增加的"支持库编译"菜单ID
#define	MEMU_COMPILE_BM_MORE	0x9004	//新增加的"编译为指定类型"菜单ID
#define	MEMU_COMPILE_BM_ASM		0x9005	//新增加的"编译为->汇编"菜单ID
#define	MEMU_COMPILE_BM_CPP		0x9006	//新增加的"编译为->C/C++"菜单ID
#define	MEMU_COMPILE_BM_VC		0x9007	//新增加的"编译为->VC++"菜单ID
//菜单名称定义
#define BMMENUNAME_WIN32		L"&B.黑月静态编译"
#define BMMENUNAME_WINCE		L"黑月编译(WinCE)"
#define BMMENUNAME_EFNE			L"黑月编译(易支持库)"
#define BMMENUNAME_MORE			L"&Z.黑月指定模式编译->"
#define BMMENUNAME_ASM			L"&1.汇编模式\t(最小)"
#define BMMENUNAME_CPP			L"&2.C/C++模式\t(标准)"
#define BMMENUNAME_VC			L"&3.VC/MFC模式\t(兼容)"
#define LI_LIB_GUID_STR     "12512500-1250-1250-1250-1250125125EE"
#define PROP_HMENU          L"{A5B04D7F-E617-49D0-963D-D9D55714510B}"


static std::unordered_map<HMENU, HWND> s_menu_hwnd;     // 菜单对应的窗口句柄
static std::stack<HWND> s_stack_menu_wnd;               // 当前子类化的菜单窗口层次, 最后一个push的就是当前显示的菜单
static std::set<HMENU> s_iDrawmenu;     // 需要自绘的菜单, 在添加菜单项的时候判断, 如果添加的菜单项是这里的菜单, 那就加上自绘标识, 否则不处理
static int s_isSubClass;                // 是否子类化菜单窗口, 0=不需要, 1=需要子类化, 2=不处理LoadMenu

static std::unordered_map<HDC, DRAWITEM_MENU_INFO> s_draw_item_info;    // hdc对应的绘画结构数据

static std::map<int, MENU_ID_DATA> s_mapMenu;
static std::map<int, std::vector<EMENU_DATA>> s_loadMenuMap;   // 每次LoadMenu的时候都加进来


LRESULT CALLBACK SystemMenu_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void _change_menu_title(HMENU hMenu);

// 将一个菜单句柄加入到自绘列表, 内部会计算菜单项的宽高, 并把菜单基本信息保存起来
void EMenu_SetMenuOwnerDraw(HMENU hMenu, MENUITEMINFOW& item);

// 内部判断是否是菜单窗口, 是菜单窗口就子类化, 否则返回
bool __sub_menu_wnd(HWND hWnd);

inline HDC _get_calctext_dc()
{
    static HDC hdc = 0;      // 测量文本宽度的dc
    if (!hdc)
    {
        hdc = CreateCompatibleDC(0);
        SelectObject(hdc, CWndBase::DefaultFont());
    }
    return hdc;
}



// 将一个菜单句柄添加到重画列表里, 如果没有加入到这个列表上, 则不会重画这个菜单项及子项
void EMenu_InsertMenu(HMENU hMenu)
{
    s_iDrawmenu.insert(hMenu);

}

// 是否子类化菜单窗口, 0=不需要, 1=需要子类化, 2=不处理LoadMenu
void EMenu_IsSubClass(int code)
{
    s_isSubClass = code;
}
// 是否子类化菜单窗口, 0=不需要, 1=需要子类化, 2=不处理LoadMenu
int EMenu_GetIsSubClass()
{
    return s_isSubClass;
}

LRESULT CALLBACK EMenu_OnInitMenuPopup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    HMENU hMenu = (HMENU)wParam;
    int menuID = (LOWORD(lParam));
    int isWindowMenu = (HIWORD(lParam));
    //s_isSubClass = 1;

    //DWORD dwStyle = ::GetWindowLongPtrA(hWnd, GWL_STYLE);
    //DWORD dwNewStyle = (dwStyle & ~WS_BORDER);
    //::SetWindowLongPtrA(hWnd, GWL_STYLE, dwNewStyle);

    //DWORD dwExStyle = ::GetWindowLongPtrA(hWnd, GWL_EXSTYLE);
    //DWORD dwNewExStyle = (dwExStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE));
    //::SetWindowLongPtrA(hWnd, GWL_EXSTYLE, dwNewExStyle);

    return ret;
}
LRESULT CALLBACK EMenu_OnUnInitMenuPopup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    //s_isSubClass = 0;

    return ret;
}


namespace _emenu_api_hook
{
    static apiHook hook_TrackPopupMenu;
    static apiHook hook_TrackPopupMenuEx;


    static apiHook hook_AppendMenuA;
    static apiHook hook_AppendMenuW;

    static apiHook hook_InsertMenuA;
    static apiHook hook_InsertMenuW;

    static apiHook hook_InsertMenuItemA;
    static apiHook hook_InsertMenuItemW;

    static apiHook hook_LoadMenuA;
    static apiHook hook_LoadMenuW;

    static apiHook hook_ModifyMenuA;
    static apiHook hook_ModifyMenuW;

    static apiHook hook_SetMenuItemInfoA;
    static apiHook hook_SetMenuItemInfoW;

    static apiHook hook_GetMenuItemInfoA;
    static apiHook hook_GetMenuItemInfoW;

    static apiHook hook_GetMenuStringA;
    static apiHook hook_GetMenuStringW;

    static apiHook hook_EnableMenuItem;
    static apiHook hook_DestroyMenu;
    static apiHook hook_RemoveMenu;


    static BOOL(WINAPI* EnableMenuItem_hook)(HMENU hMenu, UINT uIDEnableItem, UINT uEnable) = EnableMenuItem;
    static BOOL(WINAPI* DestroyMenu_hook)(HMENU hMenu) = DestroyMenu;
    static BOOL(WINAPI* RemoveMenu_hook)(HMENU hMenu, UINT uPosition, UINT uFlags) = RemoveMenu;

    static BOOL(WINAPI* InsertMenuItemA_hook)(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi) = InsertMenuItemA;
    static BOOL(WINAPI* InsertMenuItemW_hook)(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi) = InsertMenuItemW;


    static BOOL(WINAPI* ModifyMenuA_hook)(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem) = ModifyMenuA;
    static BOOL(WINAPI* ModifyMenuW_hook)(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem) = ModifyMenuW;


    static BOOL(WINAPI* SetMenuItemInfoA_hook)(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi) = SetMenuItemInfoA;
    static BOOL(WINAPI* SetMenuItemInfoW_hook)(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi) = SetMenuItemInfoW;

    static BOOL(WINAPI* GetMenuItemInfoA_hook)(HMENU hMenu, UINT item, BOOL fByPosition, LPMENUITEMINFOA lpmi) = GetMenuItemInfoA;
    static BOOL(WINAPI* GetMenuItemInfoW_hook)(HMENU hMenu, UINT item, BOOL fByPosition, LPMENUITEMINFOW lpmi) = GetMenuItemInfoW;

    static int(WINAPI* GetMenuStringA_hook)(HMENU hMenu, UINT uIDItem, LPSTR lpString, int cchMax, UINT flags) = GetMenuStringA;
    static int(WINAPI* GetMenuStringW_hook)(HMENU hMenu, UINT uIDItem, LPWSTR lpString, int cchMax, UINT flags) = GetMenuStringW;

    //static BOOL(WINAPI* InsertMenuW_hook)(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem) = InsertMenuW;


    static BOOL(WINAPI* InsertMenuA_hook)(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem) = InsertMenuA;
    static BOOL(WINAPI* InsertMenuW_hook)(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem) = InsertMenuW;

    static BOOL(WINAPI* AppendMenuA_hook)(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem) = AppendMenuA;
    static BOOL(WINAPI* AppendMenuW_hook)(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem) = AppendMenuW;

    static HMENU(WINAPI* LoadMenuA_hook)(HINSTANCE hInstance, LPCSTR lpMenuName) = LoadMenuA;
    static HMENU(WINAPI* LoadMenuW_hook)(HINSTANCE hInstance, LPCWSTR lpMenuName) = LoadMenuW;



    //static apiHook hook_NtUserTrackPopupMenuEx;
    //
    //typedef BOOL(WINAPI* pfn_NtUserTrackPopupMenuEx)(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm);
    //inline static pfn_NtUserTrackPopupMenuEx __get_NtUserTrackPopupMenuEx()
    //{
    //    HMODULE h = LoadLibraryW(L"win32u.dll");
    //    if (!h)throw;
    //    pfn_NtUserTrackPopupMenuEx fun = (pfn_NtUserTrackPopupMenuEx)GetProcAddress(h, "NtUserTrackPopupMenuEx");
    //    return fun;
    //}
    //static pfn_NtUserTrackPopupMenuEx NtUserTrackPopupMenuEx_hook = __get_NtUserTrackPopupMenuEx();


    static BOOL(WINAPI* TrackPopupMenu_hook)(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT* prcRect) = TrackPopupMenu;
    static BOOL(WINAPI* TrackPopupMenuEx_hook)(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) = TrackPopupMenuEx;


    static bool isDesignerWnd(HWND hWnd)
    {
        HWND hWndMDIClient = s_info->eWnd->GetMDIClient();
        HWND hParent = GetParent(hWnd);
        for ( int i = 0; i < 3 && hParent; i++ )
        {
            hParent = GetParent(hParent);
        }
        return hParent == hWndMDIClient;
    }
    static BOOL WINAPI MyTrackPopupMenu_hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT* prcRect)
    {
        //NtUserTrackPopupMenuEx
        // 不能全部菜单都hook, 不然有些东西不好处理, 就这样吧, 哪个窗口子类化了才自绘这个菜单
        // _subclass_hMenu_Window(hWnd, 0);

        bool isDraw = s_iDrawmenu.find(hMenu) != s_iDrawmenu.end();
        if ( !isDraw && GetDataFromHwnd(hWnd) != 0 )
        {
            s_iDrawmenu.insert(hMenu);
            isDraw = true;
        }
        
        if ( isDesignerWnd(hWnd) )
            isDraw = false;
        // 有设置了重画, 或者菜单所在窗口有子类化过, 那就加上重画标志
        if ( isDraw )
        {
            _change_menu_title(hMenu);
            s_isSubClass = 1;
            _subclass_hMenu_Window(hWnd, 0);
        }
        MENU_COPY_STRUCT hMenuCopy = _menu_copy_menu(hMenu, hWnd);
        s_info->hMenuPopup = hMenuCopy;
        BOOL ret = TrackPopupMenu_hook(s_info->hMenuPopup, uFlags, x, y, nReserved, hWnd, prcRect);

        s_info->menuSelHmenu = 0;
        s_info->menuSelIndex = 0;
        s_info->menuSelId = 0;

        s_isSubClass = 0;
        s_info->hMenuPopup = 0;
        return ret;
    }

    // 即将使用快捷键弹出易语言的菜单, 如果是手动弹出是不会进入这个函数
    static bool __track_menu(HMENU hMenu, EWINDOW_MENUSTRUCT& menu, LPOBJSTRUCT pData)
    {
        for (int i = 0; i < (int)menu.menu.size(); i++)
        {
            if (menu.menu[i].hMenu == hMenu)
            {
                EWINDOW_MENUSTRUCT* menu = _menu_getindex();
                //wstr::dbg(L"使用快捷键弹出易语言菜单, hMenu = 0x%p, index = %d, s_info->hMenuPopup = %d\n", hMenu, i, s_info->hMenuPopup);
                int x = WINDOW_CAPTION_HEIGHT, y = 10;
                x += menu->menu[i].rcItem.left + 10;

                WndProc_Custom_Menu(s_info->hEWnd, WM_MOUSEMOVE, 0, MAKELONG(x, y), pData);     // 先触发鼠标进入, 在触发鼠标左键按下
                WndProc_Custom_Menu(s_info->hEWnd, WM_LBUTTONDOWN, 0, MAKELONG(x, y), pData);   // 触发左键按下
                s_isSubClass = 0;
                return true;
            }
        }
        return false;
    }
    static BOOL WINAPI MyTrackPopupMenuEx_hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm)
    {
        //wstr::dbg(L"弹出菜单 菜单句柄 = 0x%p, s_menu[6] = 0x%p, menuDOc[6] = 0x%p\n", hMenu, s_info->menu[6], 0);
        // 不能全部菜单都hook, 不然有些东西不好处理, 就这样吧, 哪个窗口子类化了才自绘这个菜单
        // _subclass_hMenu_Window(hWnd, 0);
        bool isDraw = s_iDrawmenu.find(hMenu) != s_iDrawmenu.end();
        if ( !isDraw && GetDataFromHwnd(hWnd) != 0 )
        {
            s_iDrawmenu.insert(hMenu);
            isDraw = true;
        }

        if ( isDesignerWnd(hWnd) )
            isDraw = false;

        static HWND hWndOut = s_info->eWnd->GetStateTabOutput();
        if (hWnd == hWndOut)
        {
            InsertMenuW(hMenu, 6, MF_BYPOSITION | s_info->MY_MF_OWNERDRAW, (UINT)12100, L"清空输出框");
        }


        // 有设置了重画, 或者菜单所在窗口有子类化过, 那就加上重画标志
        if ( isDraw )
        {

            _change_menu_title(hMenu);
            s_isSubClass = 1;
            _subclass_hMenu_Window(hWnd, 0);
            LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
            if (pData && pData->draw.indexSel == -1 && __track_menu(hMenu, s_info->menuDoc, pData) || __track_menu(hMenu, s_info->menu, pData))
                return true;

        }

        MENU_COPY_STRUCT hMenuCopy = _menu_copy_menu(hMenu, hWnd);
        s_info->hMenuPopup = hMenuCopy;
        BOOL ret = TrackPopupMenuEx_hook(s_info->hMenuPopup, uFlags, x, y, hWnd, lptpm);

        s_info->menuSelHmenu = 0;
        s_info->menuSelIndex = 0;
        s_info->menuSelId = 0;
        s_isSubClass = 0;
        s_info->hMenuPopup = 0;
        return ret;
    }


    //static BOOL WINAPI MyNtUserTrackPopupMenuEx_hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm)
    //{
    //    _subclass_hMenu_Window(hWnd, 0);
    //    return NtUserTrackPopupMenuEx_hook(hMenu, uFlags, x, y, hWnd, lptpm);
    //}


    // 填写插入菜单的参数, 返回菜单结构指针
    inline static MENU_ID_DATA* _hook_insert_menu(HMENU hMenu, UINT& uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem)
    {
        if (uFlags & MFT_SEPARATOR)
        {
            //MENU_ID_DATA& data = s_mapMenu[uIDNewItem];
            //data.flags = uFlags;
            //data.hMenu = hMenu;
            //data.id = uIDNewItem;

            uFlags |= s_info->MY_MF_OWNERDRAW;
            uIDNewItem = 0;
            return 0; // 是分隔条
        }
        MENU_ID_DATA& data = s_mapMenu[uIDNewItem];

        data.flags = uFlags;
        data.hMenu = hMenu;
        data.id = uIDNewItem;

        if (!lpNewItem) lpNewItem = L"";
        size_t len = wcslen(lpNewItem) + 1;
        if (len > 100)len = 100;
        memcpy(data.text, lpNewItem, sizeof(wchar_t) * len);
        data.text[len - 1] = 0;


        RECT rc = { 0 };
        DrawTextW(_get_calctext_dc(), lpNewItem, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
        data.width = rc.right - rc.left;
        //if (wstr::find(lpNewItem, L"到上一组件") != wstr::npos)
        //    __debugbreak();
        if (wcschr(lpNewItem, '\t') == 0)
            data.width += 30;
        data.height = rc.bottom - rc.top;
        data.height = s_info->config->MenuItemHeight();
        if (data.height < 16)
            data.height = 24;
        return &data;

    }
    inline static MENU_ID_DATA* _hook_insert_menu(HMENU hMenu, UINT& uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
    {
        if (uFlags & MFT_SEPARATOR)
        {
            uFlags |= s_info->MY_MF_OWNERDRAW;
            uIDNewItem = 0;
            return 0; // 是分隔条
        }
        wstr str = _str::A2W(lpNewItem);
        return _hook_insert_menu(hMenu, uFlags, uIDNewItem, str.c_str());
    }
    inline void _hook_insert_drawList(HMENU hMenu, UINT_PTR uIDNewItem, LPCWSTR lpNewItem)
    {
        MENUITEMINFOW item = { 0 };
        EMenu_SetMenuOwnerDraw(hMenu, item);
    }

    static BOOL WINAPI MyEnableMenuItem_hook(HMENU hMenu, UINT uIDEnableItem, UINT uEnable)
    {
        UINT id = uIDEnableItem;
        if (uEnable & MF_BYPOSITION)
            id = GetMenuItemID(hMenu, uIDEnableItem);

        if (id == MEMU_COMPILE_BM_WIN32
            || id == MEMU_COMPILE_BM_ASM
            || id == MEMU_COMPILE_BM_CPP
            || id == MEMU_COMPILE_BM_VC)
        {
            static HMENU hMenuCompile;    // 易语言自带的编译菜单, 黑月的几个菜单项是否可用根据这个来判断

            if (uIDEnableItem == MEMU_COMPILE_BM_WIN32)
                hMenuCompile = hMenu;
            if (hMenuCompile)
            {
                UINT state = GetMenuState(hMenuCompile, MEMU_COMPILE, MF_BYCOMMAND);

                if (state != ((UINT)(-1)) && 0 == (state & (MF_DISABLED | MF_GRAYED)))   // 没有变灰和禁用两个标志位, 那就是要启用 
                {
                    uEnable &= ~MF_DISABLED;    // 去除禁止状态
                    uEnable &= ~MF_GRAYED;      // 去除变灰状态
                }
            }
        }
        //else if (id >= 0xA500 && id <= 0xA600) // A500-A600 是内部使用的菜单
        //{
        //    uEnable &= ~MF_DISABLED;    // 去除禁止状态
        //    uEnable &= ~MF_GRAYED;      // 去除变灰状态
        //}
        return EnableMenuItem_hook(hMenu, uIDEnableItem, uEnable);
    }


    static BOOL WINAPI MyRemoveMenu_hook(HMENU hMenu, UINT uPosition, UINT uFlags)
    {
        EWINDOW_MENUSTRUCT* menu = _menu_getindex();
        if (hMenu == menu->hMenu)
        {
            s_info->pfnControls(IDC_ICONTROL_REMOVEMENUBAR, uPosition, uFlags == MF_BYPOSITION);
        }
        return RemoveMenu_hook(hMenu, uPosition, uFlags);
    }
    static BOOL WINAPI MyDestroyMenu_hook(HMENU hMenu)
    {
        auto it = s_iDrawmenu.find(hMenu);
        if (it != s_iDrawmenu.end())
        {
            s_iDrawmenu.erase(it);
        }
        return DestroyMenu_hook(hMenu);
    }


    // 返回是否允许加入菜单
    inline bool _is_insert_menu(HMENU hMenu, UINT_PTR uIDNewItem, UINT uFlags)
    {
        return true;
        EWINDOW_MENUSTRUCT* menu = _menu_getindex();
        if (!menu) return true;

        if (hMenu == menu->hMenu)   // 不允许往顶部的菜单夹里加菜单项
        {
            // 助手允许添加
            if (uIDNewItem == 80000 && (uFlags & MF_POPUP))
                return true;
            return IDraw_Menu_Function(IDC_ICONTROL_ISADDMENUBAR, 0, (LPARAM)uIDNewItem);   // 是助手添加的就处理
        }
        return true;
    }
    BOOL _insert_menu(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPCSTR lpNewItemA);

    static BOOL _append_menu(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPCSTR lpNewItemA)
    {
        if (!_is_insert_menu(hMenu, uIDNewItem, uFlags))      // 不允许往顶部的菜单夹里加菜单项
        {
            //wstr::dbg(L"AppendMenu%s, hMenu = 0x%p, id = %p, 标题 = %s\n", (!lpNewItemA ? L"W" : L"A"), hMenu, (int)(LONG_PTR)uIDNewItem, (lpNewItem ? lpNewItem : L""));
            //_insert_menu(hMenu, 9, uFlags, uIDNewItem, lpNewItem, lpNewItemA);
            //return false;
        }
        LPPLUGIN_INFO info = s_info->pfnPlugin_Query((LPCSTR)hMenu);
        if (info)
        {
            return MenuExt_Insert((LPCSTR)hMenu, -1, MF_BYPOSITION, uIDNewItem, lpNewItem, 0);
        }
        //wstr::dbg(L"AppendMenu%s, hMenu = 0x%p, id = %p, 标题 = %s\n", (!lpNewItemA ? L"W" : L"A"), hMenu, (int)(LONG_PTR)uIDNewItem, (lpNewItem ? lpNewItem : L""));
        if (uIDNewItem == 80029)    // 80029 = &L.锁定菜单条
            return true;
        bool isInsert = false;
        if (s_iDrawmenu.find(hMenu) != s_iDrawmenu.end())
        {
            isInsert = true;
            MENU_ID_DATA* data = _hook_insert_menu(hMenu, uFlags, uIDNewItem, lpNewItem);
            //TODO 调试加入顶部菜单夹
            EWINDOW_MENUSTRUCT* menu = _menu_getindex();
            if (hMenu == menu->hMenu)
            {
                bool isPopup = false;
                if (lpNewItem && (wstr::find(lpNewItem, L"设置") != wstr::npos || wstr::find(lpNewItem, L"词库") != wstr::npos))
                    isPopup = true;
                IDC_STRUCT_MENU_INSERT arg = { (int)uIDNewItem, data->text, -1, (HMENU)(uIDNewItem), isPopup };
                s_info->pfnControls(IDC_ICONTROL_INSERTMENUBAR, 0, (LPARAM)&arg);
            }
            if (uFlags & MF_POPUP)
                _hook_insert_drawList((HMENU)uIDNewItem, uIDNewItem, lpNewItem);
        }
        BOOL ret = 0;
        if (lpNewItemA)
            ret = AppendMenuA_hook(hMenu, uFlags, uIDNewItem, lpNewItemA);
        else
            ret = AppendMenuW_hook(hMenu, uFlags, uIDNewItem, lpNewItem);

        if (!isInsert || s_info->MY_MF_OWNERDRAW == 0 || ((uFlags & s_info->MY_MF_OWNERDRAW) == s_info->MY_MF_OWNERDRAW))
            return ret;

        // AppendMenu添加的菜单一定是id, 就按id来获取菜单数据就行
        MENUITEMINFOW mi = { 0 };
        mi.cbSize = sizeof(MENUITEMINFOW);
        mi.fMask = MIIM_FTYPE | MIIM_ID;
        mi.dwTypeData = 0;
        mi.cch = 0;
        BOOL isOk = GetMenuItemInfoW_hook(hMenu, uIDNewItem, false, &mi);
        UINT uPos = uIDNewItem;
        bool isPos = false;
        if (!isOk)
        {
            isPos = true;
            uPos = GetMenuItemCount(hMenu) - 1;
            isOk = GetMenuItemInfoW_hook(hMenu, uPos, true, &mi);
        }

        mi.fMask = MIIM_FTYPE;
        mi.fType |= MF_OWNERDRAW;
        isOk = SetMenuItemInfoW_hook(hMenu, uPos, isPos, &mi);
#ifdef _DEBUG
        if (!isOk)
            __debugbreak();
#endif
        return ret;
    }
    static BOOL WINAPI MyAppendMenuW_hook(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem)
    {
        return _append_menu(hMenu, uFlags, uIDNewItem, lpNewItem, 0);
    }

    static BOOL WINAPI MyAppendMenuA_hook(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
    {
        wstr str = _str::A2W(lpNewItem);
        return _append_menu(hMenu, uFlags, uIDNewItem, str.c_str(), lpNewItem);
    }

    static inline BOOL _insert_menu_item(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi, LPCMENUITEMINFOA lpmiA)
    {
        if (!_is_insert_menu(hMenu, (UINT_PTR)lpmi->hSubMenu, lpmi->fType))      // 不允许往顶部的菜单夹里加菜单项
            return false;
        LPPLUGIN_INFO info = s_info->pfnPlugin_Query((LPCSTR)hMenu);
        if (info)
        {
            UINT uFlags = fByPosition ? MF_BYPOSITION : MF_BYCOMMAND;
            if (lpmi->fMask & MIIM_STATE)
                uFlags |= lpmi->fState;
            if (lpmi->fMask & MIIM_TYPE)
                uFlags |= lpmi->fType;
            return MenuExt_Insert((LPCSTR)hMenu, item, uFlags, lpmi->wID, lpmi->dwTypeData, lpmi->dwItemData);
        }
        //wstr::dbg(L"InsertMenuItemW, hMenu = 0x%p, id = %p, 标题 = %s\n", hMenu, (int)(LONG_PTR)item, (lpmi->dwTypeData ? lpmi->dwTypeData : L""));
        LPCMENUITEMINFOW pmi = lpmi;
        bool isInsert = false;

        if (s_isSubClass != 2 && s_iDrawmenu.find(hMenu) != s_iDrawmenu.end())
        {
            isInsert = true;
            static MENUITEMINFOW mi = { 0 };
            memcpy(&mi, lpmi, sizeof(mi));
            _hook_insert_menu(hMenu, mi.fType, mi.wID, mi.dwTypeData);
            mi.fMask |= MIIM_FTYPE;
            //mi.fType |= s_info->MY_MF_OWNERDRAW;
            pmi = &mi;
            if (lpmi->hSubMenu)
                _hook_insert_drawList((HMENU)lpmi->hSubMenu, item, lpmi->dwTypeData);

        }
        BOOL ret = false;
        if (lpmiA)
            ret = InsertMenuItemA_hook(hMenu, item, fByPosition, lpmiA);
        else
            ret = InsertMenuItemW_hook(hMenu, item, fByPosition, pmi);

        //return ret;

        if (!isInsert || s_info->MY_MF_OWNERDRAW == 0)
            return ret;

        if (fByPosition)
        {
            const int menuCount = GetMenuItemCount(hMenu);
            if (item > (UINT)menuCount)
                item = menuCount - 1;
        }

        MENUITEMINFOW mi = { 0 };
        mi.cbSize = sizeof(MENUITEMINFOW);
        mi.fMask = MIIM_FTYPE | MIIM_ID;
        mi.dwTypeData = 0;
        mi.cch = 0;
        BOOL isOk = GetMenuItemInfoW_hook(hMenu, item, fByPosition, &mi);
        bool isPos = false;
        UINT uPos = mi.wID;
        if (!isOk)
        {
            isPos = true;
            uPos = item;
            isOk = GetMenuItemInfoW_hook(hMenu, mi.wID, false, &mi);
        }
        mi.fMask = MIIM_FTYPE;
        mi.fType |= MF_OWNERDRAW;
        isOk = SetMenuItemInfoW_hook(hMenu, uPos, isPos, &mi);
#ifdef _DEBUG
        if (!isOk)
            __debugbreak();
#endif
        return ret;
    }
    static BOOL WINAPI MyInsertMenuItemW_hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi)
    {
        return _insert_menu_item(hMenu, item, fByPosition, lpmi, 0);
    }
    static BOOL WINAPI MyInsertMenuItemA_hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi)
    {
        MENUITEMINFOW mi;
        memcpy(&mi, lpmi, sizeof(mi));
        wstr str;
        if (lpmi->fMask & MIIM_STRING)
            str = _str::A2W(lpmi->dwTypeData);
        mi.dwTypeData = str.data();
        return _insert_menu_item(hMenu, item, fByPosition, &mi, lpmi);
    }


    BOOL _insert_menu(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPCSTR lpNewItemA)
    {
        if (!_is_insert_menu(hMenu, uIDNewItem, uFlags))      // 不允许往顶部的菜单夹里加菜单项
            return false;
        LPPLUGIN_INFO info = s_info->pfnPlugin_Query((LPCSTR)hMenu);
        if (info)
        {
            return MenuExt_Insert((LPCSTR)hMenu, uPosition, uFlags, uIDNewItem, lpNewItem, 0);
        }
        HMENU hMenuWnd = 0, hMenuHelp = 0;
        EMenu_Get_Help_Wnd(hMenuWnd, hMenuHelp);
        if (hMenu == hMenuWnd && lpNewItem && wcscmp(lpNewItem, L"&W.购买易语言 - 淘宝") == 0)
        {
            uFlags |= MF_BYPOSITION;
            uPosition = 10;
            hMenu = hMenuHelp;
        }
        //wstr::dbg(L"InsertMenuW, hMenu = 0x%p, id = %p, 标题 = %s\n", hMenu, (int)(LONG_PTR)uIDNewItem, (lpNewItem ? lpNewItem : L""));
        //return InsertMenuW_hook(hMenu, uPosition, uFlags, uIDNewItem, lpNewItem);
        bool isInsert = false;
        if (s_isSubClass != 2 && s_iDrawmenu.find(hMenu) != s_iDrawmenu.end())
        {
            isInsert = true;
            MENU_ID_DATA* data = _hook_insert_menu(hMenu, uFlags, uIDNewItem, lpNewItem);
            EWINDOW_MENUSTRUCT* menu = _menu_getindex();
            if (hMenu == menu->hMenu)
            {
                bool isPopup = false;
                if (lpNewItem && (wstr::find(lpNewItem, L"设置") != wstr::npos || wstr::find(lpNewItem, L"词库") != wstr::npos))
                    isPopup = true;
                IDC_STRUCT_MENU_INSERT arg = { (int)uIDNewItem, data->text, (int)uPosition, (HMENU)(uIDNewItem), isPopup };
                s_info->pfnControls(IDC_ICONTROL_INSERTMENUBAR, 0, (LPARAM)&arg);
            }
            if (uFlags & MF_POPUP)
                _hook_insert_drawList((HMENU)uIDNewItem, uIDNewItem, lpNewItem);
        }
        BOOL ret = 0;
        if (lpNewItemA)
            ret = InsertMenuA_hook(hMenu, uPosition, uFlags, uIDNewItem, lpNewItemA);
        else
            ret = InsertMenuW_hook(hMenu, uPosition, uFlags, uIDNewItem, lpNewItem);
        if (!isInsert || s_info->MY_MF_OWNERDRAW == 0 || ((uFlags & s_info->MY_MF_OWNERDRAW) == s_info->MY_MF_OWNERDRAW))
            return ret;

        bool isIndex = ((uFlags & MF_BYPOSITION) == MF_BYPOSITION);
        if (!isIndex)
        {
            uPosition = GetMenuPosFromID(hMenu, uIDNewItem);
        }

        if (uPosition == ((UINT)-1))
        {
            uPosition = GetMenuPosFromID(hMenu, uIDNewItem);
        }
        MENUITEMINFOW mi = { 0 };
        mi.cbSize = sizeof(MENUITEMINFOW);
        mi.fMask = MIIM_FTYPE | MIIM_ID;
        mi.dwTypeData = 0;
        mi.cch = 0;
        BOOL isOk = GetMenuItemInfoW_hook(hMenu, uPosition, true, &mi);
        bool isPos = true;
        UINT uPos = uPosition;
        if (!isOk)
        {
            isPos = false;
            uPos = uIDNewItem;
            isOk = GetMenuItemInfoW_hook(hMenu, uIDNewItem, false, &mi);
        }
        mi.fMask = MIIM_FTYPE;
        mi.fType |= MF_OWNERDRAW;
        isOk = SetMenuItemInfoW_hook(hMenu, uPos, isPos, &mi);
#ifdef _DEBUG
        if (!isOk)
            __debugbreak();
#endif
        return ret;
    }
    static BOOL WINAPI MyInsertMenuW_hook(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem)
    {
        return _insert_menu(hMenu, uPosition, uFlags, uIDNewItem, lpNewItem, 0);
    }

    static BOOL WINAPI MyInsertMenuA_hook(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
    {
        wstr str = _str::A2W(lpNewItem);
        return _insert_menu(hMenu, uPosition, uFlags, uIDNewItem, str.c_str(), lpNewItem);
    }


    // 加载菜单, 是易语言的菜单就加入自绘列表, 不处理全部的菜单, 只处理易语言自己的菜单
    inline static void _loadmenu_(HMENU hMenu, UINT id)
    {
        //wstr::dbg(L"LoadMenuA, hMenu = 0x%p, id = %d\n", hMenu, id);
        const UINT menuid[] = {
            127,    // 顶部菜单栏的菜单, 一大堆顶级菜单+子级菜单
            235,    // 其他地方右键菜单, 各种对齐工具条, 顶边菜单  查看->工具条
            236,    // 代码区 右键菜单
            237,    // 支持库 -> 树型框 -> 右键菜单
            238,    // 程序 -> 树型框 -> 模块引用表 右键菜单, 除了模块/外部文件记录表, 其他地方右键菜单也是238, 不过父菜单项不同
            239,    // 程序 -> 树型框 -> 程序数据 右键菜单
            240,    // 窗口设计器右键菜单
            276,    // 运行时 代码区 右键菜单
            293,    // 左边易语言属性框 -> 属性栏右键菜单
            587,    // 底边状态夹 -> 监视表右键
            606,    // 代码切换选择夹右键菜单
            609,    // 书签右键菜单
            610,    // 常量右键菜单
            611,    // 底边状态夹 -> 剪辑历史
            626,    // 程序 -> 分类文件夹右键菜单
            627,    // 程序 -> 树型框 -> 外部文件记录表 右键菜单

            1,      // 编辑框右键菜单
            12,     // 编辑框右键菜单 多了个打开/关闭输入法


        };
        if (id == 1 && s_isSubClass != 1) return;

        bool isReturn = true;
        for (int i = 0; i < sizeof(menuid) / sizeof(menuid[0]); i++)    // 不是上面的id就不处理
        {
            if (id == menuid[i])
            {
                isReturn = false;
                break;
            }
        }
        if (isReturn)return;

        MENUITEMINFO item;
        item.cbSize = sizeof(MENUITEMINFO);
        item.fMask = MIIM_SUBMENU;
        GetMenuItemInfoW(hMenu, 0, true, &item);

        if (id == 235)  // 其他地方右键菜单, 各种对齐工具条, 顶边菜单  查看->工具条, 需要删除几个影响布局的菜单项
        {
            if (item.hSubMenu)
                EMenu_InsertRButton(item.hSubMenu);
        }
        else if (id == 236)
        {
#ifdef _DEBUG
            //for (int i = 0; i < 20; i++)
            //    InsertMenuW(item.hSubMenu, 0, MF_STRING, IDMENU_LOCATIONTOOLBAR, L"1");
#endif
        }
        std::vector<EMENU_DATA>& arr = s_loadMenuMap[id];
        int len = GetMenuItemCount(hMenu);
        arr.resize(len);
        if (s_info->MY_MF_OWNERDRAW)
            modify_menu_style(hMenu, arr.data(), len);
        //for (auto it = s_iDrawmenu.begin(); it != s_iDrawmenu.end(); ++it)
        //{
        //    HMENU hEraseMenu = *it;
        //    if (!IsMenu(hEraseMenu))
        //    {
        //        s_iDrawmenu.erase(it++);
        //    }
        //}
        s_iDrawmenu.insert(hMenu);
    }

    static HMENU WINAPI MyLoadMenuA_hook(HINSTANCE hInstance, LPCSTR lpMenuName)
    {
        HMENU hMenu = LoadMenuA_hook(hInstance, lpMenuName);
        _loadmenu_(hMenu, (UINT)(LONG_PTR)lpMenuName);
        return hMenu;
    }
    static HMENU WINAPI MyLoadMenuW_hook(HINSTANCE hInstance, LPCWSTR lpMenuName)
    {
        HMENU hMenu = LoadMenuW_hook(hInstance, lpMenuName);
        _loadmenu_(hMenu, (UINT)(LONG_PTR)lpMenuName);
        return hMenu;
    }

    static BOOL WINAPI __ModifyMenu_(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPCSTR lpNewItemA)
    {
        if (s_iDrawmenu.find(hMenu) != s_iDrawmenu.end())
            uFlags |= s_info->MY_MF_OWNERDRAW;
        LPPLUGIN_INFO info = s_info->pfnPlugin_Query((LPCSTR)hMenu);
        int id = (int)uPosition;
        if (uFlags & MF_BYPOSITION) // 是索引方式修改
            id = GetMenuItemID(hMenu, uPosition);
        if (info)
        {
            id = MenuExt_Find(info, id);
            if (id != uPosition)
            {
                LPMENUEXT_ID_INFO menuExt = (LPMENUEXT_ID_INFO)id;
                menuExt->id = uIDNewItem;   // 记录新的ID
                if (menuExt->info)
                    hMenu = menuExt->info->hPopupMenu;
            }
            uPosition = id;
            uFlags &= ~MF_BYPOSITION;
        }
        auto it = s_mapMenu.find(id);
        if (it != s_mapMenu.end())
        {
            MENU_ID_DATA& data = it->second;
            if (!lpNewItem)lpNewItem = L"";
            wcscpy_s(data.text, 100, lpNewItem);
            if (id != uIDNewItem)
            {
                s_mapMenu[uIDNewItem] = data;   // 记录新的ID
            }
        }

        if(lpNewItemA)
            return ModifyMenuA_hook(hMenu, uPosition, uFlags, uIDNewItem, lpNewItemA);
        return ModifyMenuW_hook(hMenu, uPosition, uFlags, uIDNewItem, lpNewItem);

    }

    static BOOL WINAPI MyModifyMenuA_hook(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
    {
        return __ModifyMenu_(hMenu, uPosition, uFlags, uIDNewItem, wstr::A2W(lpNewItem).c_str(), lpNewItem);
    }

    static BOOL WINAPI MyModifyMenuW_hook(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem)
    {
        return __ModifyMenu_(hMenu, uPosition, uFlags, uIDNewItem, lpNewItem, 0);
    }
    static BOOL WINAPI __SetMenuItemInfo_(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi, LPCMENUITEMINFOA lpmiA)
    {
        LPCMENUITEMINFOW pmi = lpmi;
        if (s_iDrawmenu.find(hMenu) != s_iDrawmenu.end())
        {
            static MENUITEMINFOW mi = { 0 };
            memcpy(&mi, (lpmiA ? (void*)lpmiA : (void*)lpmi), sizeof(mi));
            mi.fType |= s_info->MY_MF_OWNERDRAW;
            pmi = &mi;
        }

        LPPLUGIN_INFO info = s_info->pfnPlugin_Query((LPCSTR)hMenu);
        int id = (int)item;
        if (fByPosition)    // 是按位置加入, 需要获取到ID
            id = GetMenuItemID(hMenu, item);
        if (info)
        {
            id = MenuExt_Find(info, id);
            if (id != item)
            {
                LPMENUEXT_ID_INFO menuExt = (LPMENUEXT_ID_INFO)id;
                if (menuExt->info)
                    hMenu = menuExt->info->hPopupMenu;
                if (pmi->fMask & MIIM_ID) menuExt->id = pmi->wID;           // 记录新的ID
                if (pmi->fMask & MIIM_DATA) menuExt->id = pmi->dwItemData;  // 记录新的项目数值
            }
            item = id;
            fByPosition = false;
        }

        auto it = s_mapMenu.find(id);
        if (it != s_mapMenu.end())
        {
            MENU_ID_DATA& data = it->second;
            LPCWSTR lpNewItem = pmi->dwTypeData;
            if ((pmi->fMask & MIIM_STRING) == MIIM_STRING)  // 有设置菜单标题, 记录菜单标题
            {
                if (!lpNewItem)lpNewItem = L"";
                if (lpmiA)
                {
                    wstr str = _str::A2W(lpmiA->dwTypeData);
                    lpNewItem = str.c_str();
                    if (!lpNewItem)lpNewItem = L"";
                    wcscpy_s(data.text, 100, lpNewItem);
                }
                else
                {
                    wcscpy_s(data.text, 100, lpNewItem);
                }
            }

            if (pmi->fMask & MIIM_BITMAP)       data.hbmpItem = pmi->hbmpItem;
            if (pmi->fMask & MIIM_DATA)         data.dwItemData = pmi->dwItemData;
            if (pmi->fMask & MIIM_CHECKMARKS)   data.hbmpChecked = pmi->hbmpChecked, data.hbmpUnchecked = pmi->hbmpUnchecked;
            if (pmi->fMask & MIIM_FTYPE)        data.fType = pmi->fType;
            if (pmi->fMask & MIIM_STATE)        data.fState = pmi->fState;
            if (pmi->fMask & MIIM_SUBMENU)      data.hSubMenu = pmi->hSubMenu;

            if (pmi->fMask & MIIM_ID)
            {
                data.id = pmi->wID;
                s_mapMenu[data.id] = data;  // map里记录新的id项
            }
        }

        if (lpmiA)
            return SetMenuItemInfoA_hook(hMenu, item, fByPosition, (LPCMENUITEMINFOA)pmi);
        return SetMenuItemInfoW_hook(hMenu, item, fByPosition, pmi);
    }

    static BOOL WINAPI MySetMenuItemInfoA_hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOA lpmi)
    {
        return __SetMenuItemInfo_(hMenu, item, fByPosition, (LPCMENUITEMINFOW)lpmi, lpmi);
    }
    static BOOL WINAPI MySetMenuItemInfoW_hook(HMENU hMenu, UINT item, BOOL fByPosition, LPCMENUITEMINFOW lpmi)
    {
        return __SetMenuItemInfo_(hMenu, item, fByPosition, lpmi, 0);
    }

    inline BOOL __getmenu_iteminfo(HMENU hMenu, UINT item, BOOL fByPosition, LPMENUITEMINFOW lpmi, LPMENUITEMINFOA lpmiA)
    {
        BOOL ret = false;
        int cch = lpmi ? lpmi->cch : lpmiA->cch;
        UINT oldMask = lpmi ? lpmi->fMask : lpmiA->fMask;

        if ( lpmiA )
        {
            lpmiA->fMask |= MIIM_ID;
            ret = GetMenuItemInfoA_hook(hMenu, item, fByPosition, lpmiA);
            lpmiA->fMask = oldMask;
        }
        else
        {
            lpmi->fMask |= MIIM_ID;
            ret = GetMenuItemInfoW_hook(hMenu, item, fByPosition, lpmi);
            lpmi->fMask = oldMask;
        }
        UINT wID = lpmi ? lpmi->wID : lpmiA->wID;
        if ( s_iDrawmenu.find(hMenu) == s_iDrawmenu.end() )
            return ret;
        UINT id = item;
        if ( fByPosition )
        {
            id = GetMenuItemID(hMenu, fByPosition);
            if ( id != wID )
                id = wID;
        }
        LPMENUITEMINFOW mi = lpmi ? lpmi : (LPMENUITEMINFOW)lpmiA;
        if (!mi)
            return ret;
        auto it = s_mapMenu.find(id);
        if (it == s_mapMenu.end())
            return ret;
        MENU_ID_DATA& data = it->second;
        if ((mi->fMask & MIIM_STRING) == MIIM_STRING)
        {
            if (lpmiA)
            {
                if (data.text[0] == 0)
                {
                    wstr str = _str::A2W(lpmiA->dwTypeData);
                    wcscpy_s(data.text, 100, str.c_str());
                    return ret;
                }

                _str str = wstr::W2A(data.text);
                LPCSTR pstr = str.c_str();
                if (!pstr) pstr = "";
                strcpy_s(lpmiA->dwTypeData, cch, pstr);
            }
            else
            {
                if (data.text[0] == 0)
                {
                    wcscpy_s(data.text, 100, lpmi->dwTypeData);
                    return ret;
                }
                wcscpy_s(lpmi->dwTypeData, cch, data.text);
            }
        }
        return ret;
    }
    static BOOL WINAPI MyGetMenuItemInfoA_hook(HMENU hMenu, UINT item, BOOL fByPosition, LPMENUITEMINFOA lpmi)
    {
        return __getmenu_iteminfo(hMenu, item, fByPosition, 0, lpmi);
    }
    static BOOL WINAPI MyGetMenuItemInfoW_hook(HMENU hMenu, UINT item, BOOL fByPosition, LPMENUITEMINFOW lpmi)
    {
        return __getmenu_iteminfo(hMenu, item, fByPosition, lpmi, 0);
    }

    inline BOOL __getmenu_string(HMENU hMenu, UINT uIDItem, LPWSTR lpString, LPSTR lpStringA, int cchMax, UINT flags)
    {
        if (s_iDrawmenu.find(hMenu) == s_iDrawmenu.end())
        {
            if (lpStringA)
                return GetMenuStringA_hook(hMenu, uIDItem, lpStringA, cchMax, flags);
            return GetMenuStringW_hook(hMenu, uIDItem, lpString, cchMax, flags);
        }
        UINT id = uIDItem;
        if ((flags & MF_BYPOSITION) == MF_BYPOSITION)   // 按位置获取, 需要转换成ID
        {
            id = GetMenuItemID(hMenu, uIDItem);
        }

        auto it = s_mapMenu.find(id);
        if (it == s_mapMenu.end())
        {
            if (lpStringA)
                return GetMenuStringA_hook(hMenu, uIDItem, lpStringA, cchMax, flags);
            return GetMenuStringW_hook(hMenu, uIDItem, lpString, cchMax, flags);
        }

        MENU_ID_DATA& data = it->second;
        if (!data.text[0])
        {
            GetMenuStringW_hook(hMenu, uIDItem, data.text, 100, flags);
        }
        if (lpStringA)
        {
            _str str = wstr::W2A(data.text);
            if (str.empty())
            {
                lpStringA[0] = 0;
                return true;
            }
            strcpy_s(lpStringA, cchMax, str.c_str());
        }
        else
        {
            wcscpy_s(lpString, cchMax, data.text);
        }
        return true;
    }

    static BOOL WINAPI MyGetMenuStringA_hook(HMENU hMenu, UINT uIDItem, LPSTR lpString, int cchMax, UINT flags)
    {
        return __getmenu_string(hMenu, uIDItem, 0, lpString, cchMax, flags);
    }
    static BOOL WINAPI MyGetMenuStringW_hook(HMENU hMenu, UINT uIDItem, LPWSTR lpString, int cchMax, UINT flags)
    {
        return __getmenu_string(hMenu, uIDItem, lpString, 0, cchMax, flags);
    }


    typedef BOOL(WINAPI* AppendMenuA_hook___)(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem);
    static void* hook;
    static BOOL WINAPI MyAppendMenuA_hook111(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
    {
        BOOL bret = 0;
        __asm
        {
            push lpNewItem;
            push uIDNewItem;
            push uFlags;
            push hMenu;
            call hook;
            mov [bret], eax;
        }
        return bret;
    }
    inline void _hook_()
    {
        //return;
        ////static void* hook = AppendMenuA_hook;
        //void* hook1 = &AppendMenuA;
        ////hook_AppendMenuA.hook((void**)&AppendMenuA_hook, MyAppendMenuA_hook);
        //{
        //    //DetourRestoreAfterWith();   //恢复原来状态
        //    DetourTransactionBegin();   //拦截开始
        //    DetourUpdateThread(GetCurrentThread());//刷新当前线程
        //    DetourAttach(&(LPVOID&)hook1, MyAppendMenuA_hook111);
        //    DetourTransactionCommit();  //拦截生效
        //}
        //hook = (AppendMenuA_hook___)hook1;


        hook_AppendMenuA.hook(&(PVOID&)AppendMenuA_hook, MyAppendMenuA_hook);
        hook_AppendMenuW.hook(&(PVOID&)AppendMenuW_hook, MyAppendMenuW_hook);
    }

    inline void _hook_sleep()
    {
        hook_InsertMenuItemA.hook(&(PVOID&)InsertMenuItemA_hook, MyInsertMenuItemA_hook);
        hook_InsertMenuItemW.hook(&(PVOID&)InsertMenuItemW_hook, MyInsertMenuItemW_hook);

        hook_InsertMenuA.hook(&(PVOID&)InsertMenuA_hook, MyInsertMenuA_hook);
        hook_InsertMenuW.hook(&(PVOID&)InsertMenuW_hook, MyInsertMenuW_hook);

        hook_LoadMenuA.hook(&(PVOID&)LoadMenuA_hook, MyLoadMenuA_hook);
        hook_LoadMenuW.hook(&(PVOID&)LoadMenuW_hook, MyLoadMenuW_hook);

        hook_SetMenuItemInfoA.hook(&(PVOID&)SetMenuItemInfoA_hook, MySetMenuItemInfoA_hook);
        hook_SetMenuItemInfoW.hook(&(PVOID&)SetMenuItemInfoW_hook, MySetMenuItemInfoW_hook);

        hook_ModifyMenuA.hook(&(PVOID&)ModifyMenuA_hook, MyModifyMenuA_hook);
        hook_ModifyMenuW.hook(&(PVOID&)ModifyMenuW_hook, MyModifyMenuW_hook);

        hook_TrackPopupMenu.hook(&(PVOID&)TrackPopupMenu_hook, MyTrackPopupMenu_hook);
        hook_TrackPopupMenuEx.hook(&(PVOID&)TrackPopupMenuEx_hook, MyTrackPopupMenuEx_hook);
        hook_EnableMenuItem.hook(&(PVOID&)EnableMenuItem_hook, MyEnableMenuItem_hook);
        hook_DestroyMenu.hook(&(PVOID&)DestroyMenu_hook, MyDestroyMenu_hook);
        hook_RemoveMenu.hook(&(PVOID&)RemoveMenu_hook, MyRemoveMenu_hook);

        hook_GetMenuStringA.hook(&(PVOID&)GetMenuStringA_hook, MyGetMenuStringA_hook);
        hook_GetMenuStringW.hook(&(PVOID&)GetMenuStringW_hook, MyGetMenuStringW_hook);

        hook_GetMenuItemInfoA.hook(&(PVOID&)GetMenuItemInfoA_hook, MyGetMenuItemInfoA_hook);
        hook_GetMenuItemInfoW.hook(&(PVOID&)GetMenuItemInfoW_hook, MyGetMenuItemInfoW_hook);

    }
}


// 内部判断是否是菜单窗口, 是菜单窗口就子类化, 否则返回
bool __sub_menu_wnd(HWND hWnd)
{
    wchar_t sClassName[10];
    int len = ::GetClassNameW(hWnd, sClassName, sizeof(sClassName) / sizeof(sClassName[0]));
    //wstr::dbg(L"创建新的窗口, 窗口类名 = %s, s_isSubClass = %d\n", sClassName, s_isSubClass);

    //检查是否菜单窗口
    if (s_isSubClass == 0)
        return false;
    if (len != 6 || wcscmp(sClassName, L"#32768") != 0)
        return false;

    WNDPROC oldProc = (WNDPROC)GetWindowLongPtrA(hWnd, GWLP_WNDPROC);
    if (oldProc != SystemMenu_Proc)
    {
        s_stack_menu_wnd.push(hWnd);
        //替换菜单窗口过程函数
        SetWindowLongPtrA(hWnd, GWLP_WNDPROC, (LONG_PTR)SystemMenu_Proc);
        SetPropW(hWnd, L"proc", oldProc);
        //wstr::dbg(L"子类化菜单窗口, 窗口句柄 = 0x%08X\n", hWnd);
        //SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
    }

    return true;
}

static inline void DrawMenuItem(HWND hWnd, LPCANVAS hCanvas, MENU_ID_DATA& data, HMENU hMenu, RECT_F& rcItem, LPEX_FONT hFont)
{
    LPEX_BRUSH hbrText      = s_info->window_d2d_info.hbrMenuText;   // 默认文本画刷
    LPEX_BRUSH hbrBack      = 0;   // 默认背景画刷
    const float height = (float)data.height;
    rcItem.bottom = rcItem.top + height;

    const int state = GetMenuState(hMenu, data.id, MF_BYCOMMAND);
    const int flag = LOWORD(state);
    const int subCount = HIWORD(state);

    if ( __query(state, MFT_SEPARATOR) )
    {
        // 分隔条, 画条横线
        float offsetTop = ( height - 1 ) / 2;
        float top = rcItem.top + offsetTop;

        LPEX_PEN hpenBorder = s_info->window_d2d_info.hPenMenuBorder;   // 默认边框画笔
        LINEPOINT_F linePoint(rcItem.left + 30, top, rcItem.right - 10, top);
        hCanvas->DrawLine(hpenBorder, &linePoint);  // 画条横线, 从左边30 画到 右边-10的位置, 分隔条占用5个像素, 这里就从顶边2开始画
        rcItem.offset(0, height);
        return;
    }



    if ( __query(state, MF_HILITE) )
    {
        // 热点
        hbrBack = s_info->window_d2d_info.hbrMenuHot;
        hbrText = s_info->window_d2d_info.hbrMenuTextHot;
        if ( __query(state, MF_DISABLED) )
            hbrBack = s_info->window_d2d_info.hbrMenuBan;
    }

    if ( __query(state, MF_DISABLED) )
        hbrText = s_info->window_d2d_info.hbrMenuTextBan;

    if ( hbrBack )
        hCanvas->FillRectangle(hbrBack, &rcItem);


    hCanvas->drawtext(hFont, hbrText, data.text, -1, DT_VCENTER | DT_SINGLELINE, &rcItem);

    rcItem.offset(0, height);   // 往下一个项目偏移
}

static inline void OnDrawMenu(HWND hWnd, HDC hdc)
{
    RECT rc = { 0 };
    GetWindowRect(hWnd, &rc);
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    rc = { 0, 0, cxClient, cyClient };
    RECT_F rcItem(rc);

    LPCANVAS hCanvas = s_info->window_d2d_info.hCanvas;
    const int cxCanvas = (int)hCanvas->GetWidth();
    const int cyCanvas = (int)hCanvas->GetHeight();
    if ( cxCanvas < cxClient || cyCanvas < cyClient )
    {
        int maxWidth = cxCanvas;
        int maxheight = cyCanvas;

        if ( cxClient > maxWidth )  maxWidth = cxClient;
        if ( cyClient > maxheight ) maxheight = cyClient;
        _canvas_resize(hCanvas, maxWidth, maxheight);
    }

    int oldRenderMode = 0;
    HMENU hMenu = (HMENU)GetPropW(hWnd, PROP_HMENU);
    hCanvas->BeginDraw();
    _canvas_SetAntialiasMode(s_info->window_d2d_info.hCanvas, 0, &oldRenderMode);   // 画线不需要抗锯齿

    LPEX_BRUSH hbrBack      = s_info->window_d2d_info.hbrMenuBack;   // 默认背景画刷
    hCanvas->FillRectangle(hbrBack, &rcItem);

    int count = GetMenuItemCount(hMenu);
    LPEX_FONT hFont = s_info->defData->draw.hFont;
    for ( int i = 0; i < count; i++ )
    {
        const int id = GetMenuItemID(hMenu, i);
        MENU_ID_DATA& data = s_mapMenu[id];
        DrawMenuItem(hWnd, hCanvas, data, hMenu, rcItem, hFont);

    }

    LPEX_PEN hpenBorder = s_info->window_d2d_info.hPenMenuBorder; // 默认边框画笔
    rcItem(rc);
    hCanvas->DrawRectangle(hpenBorder, &rcItem);


    HDC hdcD2D = _canvas_getdc(hCanvas);
    BitBlt(hdc, 0, 0, cxClient, cyClient, hdcD2D, 0, 0, SRCCOPY);
    _canvas_releasedc(hCanvas);

    _canvas_SetAntialiasMode(s_info->window_d2d_info.hCanvas, oldRenderMode, &oldRenderMode);
    hCanvas->EndDraw();

}


LRESULT CALLBACK SystemMenu_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

#define MN_SETHMENU                     0x01E0
//#define MN_GETHMENU                     0x01E1
#define MN_SIZEWINDOW                   0x01E2
#define MN_OPENHIERARCHY                0x01E3
#define MN_CLOSEHIERARCHY               0x01E4
#define MN_SELECTITEM                   0x01E5
#define MN_CANCELMENUS                  0x01E6
#define MN_SELECTFIRSTVALIDITEM         0x01E7
#define MN_GETPPOPUPMENU                0x01EA
#define MN_FINDMENUWINDOWFROMPOINT      0x01EB
#define MN_SHOWPOPUPWINDOW              0x01EC
#define MN_BUTTONDOWN                   0x01ED
#define MN_MOUSEMOVE                    0x01EE
#define MN_BUTTONUP                     0x01EF
#define MN_SETTIMERTOOPENHIERARCHY      0x01F0

#define MN_DBLCLK                       0x01F1
#define MN_ENDMENU                      0x01F2
#define MN_DODRAGDROP                   0x01F3
//#define MN_ENDMENU                      0x01F4

    //if (message != MN_SELECTITEM && message != WM_NCHITTEST)
    //    wstr::dbg(L"message = 0x%X\n", message);
    WNDPROC oldProc = (WNDPROC)GetPropW(hWnd, L"proc");
    switch (message)
    {
    case MN_SELECTITEM:
    {
        int index = (int)wParam;
        if (index >= 0)
        {
            HMENU hMenu = (HMENU)GetPropW(hWnd, PROP_HMENU);
            s_info->menuSelIndex = index;
            s_info->menuSelHmenu = hMenu;
            s_info->menuSelId = GetMenuItemID(hMenu, index);
        }
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_CREATE:
    {
        //首先要去掉菜单窗口的WS_BORDER风格和WS_EX_DLGMODALFRAME , WS_EX_WINDOWEDGE扩展风格
        LRESULT ret = CallWindowProcA(oldProc, hWnd, message, wParam, lParam);

        DWORD dwStyle = ::GetWindowLongPtrA(hWnd, GWL_STYLE);
        DWORD dwNewStyle = (dwStyle & ~WS_BORDER);
        ::SetWindowLongPtrA(hWnd, GWL_STYLE, dwNewStyle);

        DWORD dwExStyle = ::GetWindowLongPtrA(hWnd, GWL_EXSTYLE);
        DWORD dwNewExStyle = (dwExStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE));
        ::SetWindowLongPtrA(hWnd, GWL_EXSTYLE, dwNewExStyle);

        //dwStyle = GetClassLongPtrW(hWnd, GCL_STYLE);
        //dwStyle &= ~CS_DROPSHADOW;
        //SetClassLongPtrW(hWnd, GCL_STYLE, dwStyle);
        return ret;
    }
    case MN_SIZEWINDOW:   // MN_SIZEWINDOW
    {
        HMENU hMenu = (HMENU)SendMessageW(hWnd, MN_GETHMENU, 0, 0);
        if (hMenu)
        {
            s_menu_hwnd[hMenu] = hWnd;
            SetPropW(hWnd, PROP_HMENU, hMenu);
        }
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        break;
    }
    //case WM_PAINT:
    //{
    //    PAINTSTRUCT ps;
    //    BeginPaint(hWnd, &ps);
    //    OnDrawMenu(hWnd, ps.hdc);
    //    EndPaint(hWnd, &ps);
    //    break;
    //}
    //case WM_NCPAINT:
    //    return 0;
    //case WM_SYNCPAINT:
    //    return 0;
    //case WM_SETREDRAW:
    //    return 0;
    case WM_PRINT:  //这里/阻止非客户区的绘制
    {
        //OnDrawMenu(hWnd, (HDC)wParam);
        //return 0;
        CallWindowProcA(oldProc, hWnd, WM_PRINT, wParam, lParam);
        return CallWindowProcA(oldProc, hWnd, WM_PRINTCLIENT, wParam, lParam);
        //CallWindowProcA(oldProc, hWnd, WM_PRINT, wParam, lParam);
        return 0;

        HDC hdc = (HDC)wParam;
        DRAWITEM_MENU_INFO& info = s_draw_item_info[hdc];
        info.hWnd = hWnd;
        info.hdc = (HDC)wParam;
        info.hMenu = GetMenuFromHwnd(hWnd);
        RECT& rc = info.rc;
        GetWindowRect(hWnd, &rc);
        info.cxClient = rc.right - rc.left;
        info.cyClient = rc.bottom - rc.top;
        LPCANVAS hCanvas = s_info->window_d2d_info.hCanvas;

        int cyMaximized = GetSystemMetrics(SM_CYMAXIMIZED); // 屏幕高度 - 任务栏
        info.isTopTriangle = info.cyClient > cyMaximized;
        if (info.isTopTriangle)
        {
            info.cyClient = cyMaximized - 200;
            info.rc.bottom = info.rc.top + info.cyClient;
            SetWindowPos(hWnd, 0, rc.left, rc.top, info.cxClient, info.cyClient, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (0)
        {
            int maxHeight = 0;  // 菜单总高度
            int count = GetMenuItemCount(info.hMenu);
            for (int i = 0; i < count; i++)
            {
                int id = GetMenuItemID(info.hMenu, i);
                MENU_ID_DATA* data = EMenu_GetMenuItemInfoFromID(id);
                if (data)
                    maxHeight += data->height;
            }
            if (maxHeight > cyMaximized)
                info.isTopTriangle = true;
        }


        const int cxCanvas = (int)hCanvas->GetWidth();
        const int cyCanvas = (int)hCanvas->GetHeight();
        if (cxCanvas < info.cxClient || cyCanvas < info.cyClient)
        {
            int maxWidth = cxCanvas;
            int maxheight = cyCanvas;

            if (info.cxClient > maxWidth)  maxWidth  = info.cxClient;
            if (info.cyClient > maxheight) maxheight = info.cyClient;

            _canvas_resize(hCanvas, maxWidth, maxheight);
        }

        //rcF(rc);
        //return 0;

        //wstr::dbg(L"WM_PRINT 即将调用, hdc = 0x%p, hMenu = %p\n", hdc, hMenu);

        LRESULT ret = CallWindowProcA(oldProc, hWnd, WM_PRINTCLIENT, wParam, lParam);
        //wstr::dbg(L"WM_PRINT 调用完毕, hdc = 0x%p, hMenu = %p\n", hdc, hMenu);

        //LPEX_PEN hpenBorder = s_info->window_d2d_info.hPenMenuBorder;   // 默认边框画笔

        //_canvas_begindraw(hCanvas);

        //int oldTextMode = 0, oldRenderMode = 0;
        //_canvas_SetAntialiasMode(s_info->window_d2d_info.hCanvas, 0, &oldRenderMode);   // 画线不需要抗锯齿

        //_canvas_drawRectangle(hCanvas, hpenBorder, &rcF);            // 在菜单窗口外围画个边框


        //HDC hDCD2D = _canvas_getdc(hCanvas);
        //BitBlt(hdc, 0, 0, cxClient, cyClient, hDCD2D, 0, 0, SRCCOPY);
        //_canvas_releasedc(hCanvas);

        //_canvas_SetAntialiasMode(hCanvas, oldRenderMode, 0);
        //_canvas_enddraw(hCanvas);

        return ret;
    }
    case WM_PRINTCLIENT:
    {
        //OnDrawMenu(hWnd, (HDC)wParam);
        //return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        //return CallWindowProcA(oldProc, hWnd, WM_PRINTCLIENT, wParam, lParam);
        return 0;
    }
    case WM_WINDOWPOSCHANGING:
    {
        //最后, 因为在MeasureItem里指定的菜单大小，系统会自动加上边框，这里必须去掉这部分额外的尺寸
        //将菜单大小改小
        if (s_isSubClass == 1)
        {
            LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
            if ((pos->flags & SWP_NOSIZE) == 0) // 尺寸被改变了
            {
                pos->cx -= 2 * GetSystemMetrics(SM_CXBORDER) + 4;
                pos->cy -= 2 * GetSystemMetrics(SM_CYBORDER) + 4;
            }
            if ((pos->flags & SWP_NOMOVE) == 0) // 位置被移动了
            {
                if (s_stack_menu_wnd.size() > 1)    // 栈成员大于1表示这个是弹出的子菜单
                {
                    s_stack_menu_wnd.pop();
                    HWND hParent = s_stack_menu_wnd.top();
                    s_stack_menu_wnd.push(hWnd);

                    //wstr::dbg(L"hWnd = 0x%08X, hParent = 0x%08X, EWindow = 0x%08X\n", hWnd, hParent, s_info->hEWnd);
                    if (hParent)
                    {
                        RECT rc;
                        GetWindowRect(hParent, &rc);
                        if (pos->x < rc.left)
                            pos->x += 6;    // 新弹出的菜单在之前菜单的左边, 那左边位置就是加
                        else
                            pos->x -= 4;    // 这里就是弹出在原菜单的右边

                    }
                }

            }
        }

        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    //case WM_ERASEBKGND:
    //    return 1;
    //case WM_WINDOWPOSCHANGED:
    //{
    //    LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
    //    if ((pos->flags & SWP_NOSIZE) == 0      // 改变了尺寸
    //        || (pos->flags & SWP_NOMOVE) == 0)  // 被移动了
    //    {
    //        if (pos->x == 0 && pos->y == 0)
    //            return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);

    //        RECT rc;
    //        rc.left = pos->x;
    //        rc.top = pos->y;
    //        rc.left = 0;
    //        rc.top = 0;
    //        rc.right = rc.left + pos->cx;
    //        rc.bottom = rc.top + pos->cy;
    //        s_shadowMenu.Update(rc);
    //        SetWindowPos(s_shadowMenu, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
    //        wstr::dbg(L"窗口矩形 = %d,%d,%d,%d,  宽度高度 = %d,%d\n", rc.left, rc.top, rc.right, rc.bottom, pos->cx, pos->cy);
    //    }
    //    return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    //}
    case WM_DESTROY:
    {
        //wstr::dbg(L"菜单销毁窗口, 窗口句柄 = %d\n", hWnd);
        //s_shadowMenu.hide();
        s_stack_menu_wnd.pop();
        s_menu_hwnd.erase(GetMenuFromHwnd(hWnd));
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    default:
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
HWND GetCurrentMenuHwnd()
{
    if ( s_stack_menu_wnd.empty() )
        return 0;
    return s_stack_menu_wnd.top();
}
HWND  GetHwndFromMenu(HMENU hMenu)
{
    auto it = s_menu_hwnd.find(hMenu);
    if (it == s_menu_hwnd.end())
        return 0;
    return it->second;
}
HMENU GetMenuFromHwnd(HWND hWnd)
{
    return (HMENU)GetPropW(hWnd, PROP_HMENU);
}
DRAWITEM_MENU_INFO* GetDrawItemInfo(HDC hdc)
{
    auto it = s_draw_item_info.find(hdc);
    if (it == s_draw_item_info.end())
        return 0;
    return &it->second;
}
// 将一个菜单句柄加入到自绘列表, 内部会计算菜单项的宽高, 并把菜单基本信息保存起来
void EMenu_SetMenuOwnerDraw(HMENU hMenu, MENUITEMINFOW& item)
{
    if (!hMenu)
        return;
    int len = GetMenuItemCount(hMenu);
    s_iDrawmenu.insert(hMenu);
    item.cbSize = sizeof(MENUITEMINFOW);
    static HDC hdc = _get_calctext_dc();
    for (int i = 0; i < len; i++)
    {
        item.fMask = MIIM_DATA | MIIM_ID | MIIM_SUBMENU | MIIM_FTYPE | MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_STATE;
        item.cch = 260;
        GetMenuItemInfoW(hMenu, i, true, &item);
        if (!item.wID)
        {
            if (item.fType & MFT_SEPARATOR)     // 是分隔条
            {
                MENU_ID_DATA& data = s_mapMenu[0];
                data.height = s_info->config->MenuSeparatorHeight();
                if (data.height < 1)
                    data.height = 5;
                else if (data.height > 50)
                    data.height = 5;

                if (s_info->MY_MF_OWNERDRAW)
                {
                    item.fMask = MIIM_FTYPE;
                    item.fType |= s_info->MY_MF_OWNERDRAW;
                    //SetMenuItemInfoW(hMenu, i, true, &item);    // 调用这个会记录设置的数据
                    _emenu_api_hook::SetMenuItemInfoW_hook(hMenu, i, true, &item);
                }
            }
            continue;
        }


        MENU_ID_DATA& data = s_mapMenu[item.wID];
        GetMenuStringW(hMenu, item.wID, data.text, 100, MF_BYCOMMAND);

        data.hbmpItem       = item.hbmpItem;
        data.dwItemData     = item.dwItemData;
        data.hbmpChecked    = item.hbmpChecked, data.hbmpUnchecked = item.hbmpUnchecked;
        data.fType          = item.fType;
        data.fState         = item.fState;
        data.hSubMenu       = item.hSubMenu;
        data.hMenu          = item.hSubMenu;
        data.id             = item.wID;
        RECT rc = { 0 };

        //if (wcschr(data.text, '\t') == 0)
        //    wstr::dbg(L"加入菜单, 菜单标题 = %s, id = %d\n", data.text, data.id);

        DrawTextW(hdc, data.text, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
        data.width = rc.right - rc.left;
        //data.height = rc.bottom - rc.top;
        data.height = s_info->config->MenuItemHeight();
        if (data.height < 16)
            data.height = 24;

        if (s_info->MY_MF_OWNERDRAW)
        {
            item.fMask = MIIM_FTYPE;
            item.fType |= MF_OWNERDRAW;
            _emenu_api_hook::SetMenuItemInfoW_hook(hMenu, i, true, &item);
            //wstr::dbg(L"%02d EMenu_SetMenuOwnerDraw 菜单标题 = %s, 菜单句柄 = 0x%p, 子菜单句柄 = %d\n", i, data.text, hMenu, item.hSubMenu);
        }
        EMenu_SetMenuOwnerDraw(item.hSubMenu, item); // 递归处理下一层菜单项

    }

}

MENU_ID_DATA* EMenu_GetMenuItemInfoFromID(UINT id)
{
    auto it = s_mapMenu.find(id);
    if (it == s_mapMenu.end()) return 0;
    return &(it->second);
}

void modify_menu_style(HMENU hMenu, LPEMENU_DATA pMenu, int count)
{
    MENUITEMINFOW item = { 0 };
    item.cbSize = sizeof(MENUITEMINFOW);
    for (int i = 0; i < count; i++)
    {
        EMENU_DATA& menu_data = pMenu[i];
        item.dwTypeData = menu_data.text;
        item.cch = sizeof(menu_data.text) / sizeof(menu_data.text[0]);


        item.fMask = MIIM_DATA | MIIM_ID | MIIM_STRING | MIIM_SUBMENU | MIIM_FTYPE;
        GetMenuItemInfoW(hMenu, i, true, &item);

        menu_data.hMenu = item.hSubMenu;
        menu_data.dwItemData = item.dwItemData;
        menu_data.id = item.wID;
        //wstr::dbg(L"%02d 菜单标题 = %s, 菜单句柄 = 0x%p, 子菜单句柄 = 0x%p\n", i, menu.text, hMenu, item.hSubMenu);

        EMenu_SetMenuOwnerDraw(item.hSubMenu, item);

    }
}

bool _menu_cbthook_proc(int code, WPARAM wParam, LPARAM lParam, LRESULT& ret)
{
    CBT_CREATEWND* pStruct = (CBT_CREATEWND*)lParam;
    if (HCBT_CREATEWND == code)
    {
        //pStruct->lpcs->dwExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE);
        //pStruct->lpcs->style &= ~(WS_BORDER);
        HWND hWnd = (HWND)wParam;

        if (s_info->MY_MF_OWNERDRAW)
            __sub_menu_wnd(hWnd);
    }
    //while (code == HC_ACTION)
    //{
    //    HWND hWnd = pStruct->hwnd;

    //    //捕捉菜单创建WM_CREATE, 0x01E2是什么消息我不知道,呵呵,这一段参考了别人的代码
    //    if (pStruct->message != WM_CREATE && pStruct->message != 0x01E2)
    //        break;
    //    __sub_menu_wnd(hWnd);
    //    break;
    //}
    return false;
}
inline void IDraw_hook_menu_Destroy()
{
    s_iDrawmenu.clear();
    s_mapMenu.clear();
    s_loadMenuMap.clear();

    _emenu_api_hook::hook_AppendMenuA       .unHook();
    _emenu_api_hook::hook_AppendMenuW       .unHook();
    _emenu_api_hook::hook_InsertMenuA       .unHook();
    _emenu_api_hook::hook_InsertMenuW       .unHook();
    _emenu_api_hook::hook_InsertMenuItemA   .unHook();
    _emenu_api_hook::hook_InsertMenuItemW   .unHook();
    _emenu_api_hook::hook_LoadMenuA         .unHook();
    _emenu_api_hook::hook_LoadMenuW         .unHook();
    _emenu_api_hook::hook_ModifyMenuA       .unHook();
    _emenu_api_hook::hook_ModifyMenuW       .unHook();
    _emenu_api_hook::hook_SetMenuItemInfoA  .unHook();
    _emenu_api_hook::hook_SetMenuItemInfoW  .unHook();
    _emenu_api_hook::hook_GetMenuItemInfoA  .unHook();
    _emenu_api_hook::hook_GetMenuItemInfoW  .unHook();
    _emenu_api_hook::hook_GetMenuStringA    .unHook();
    _emenu_api_hook::hook_GetMenuStringW    .unHook();
    _emenu_api_hook::hook_EnableMenuItem    .unHook();
    _emenu_api_hook::hook_DestroyMenu       .unHook();
    _emenu_api_hook::hook_RemoveMenu        .unHook();

}
void _apihook_menu_init(HINSTANCE hInst)
{
    static bool isFirst = false;
    if (!isFirst)
    {
        isFirst = true;
        IDraw_PushPluginFreeCallback(IDraw_hook_menu_Destroy);
        HookLoadStringInit();
        _emenu_api_hook::_hook_();

    }
}

void _apihook_menu_init_sleep()
{
    _emenu_api_hook::_hook_sleep();
}


HMENU _menu_create()
{
    HMENU hMenu = CreatePopupMenu();
    EMenu_InsertMenu(hMenu);
    return hMenu;
}

bool _menu_add(HMENU hMenu, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, UINT uFlags)
{
    return _emenu_api_hook::MyAppendMenuW_hook(hMenu, uFlags, uIDNewItem, lpNewItem);
    return AppendMenuW(hMenu, uFlags, uIDNewItem, lpNewItem);
}
bool _menu_insert(HMENU hMenu, UINT uPosition, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, UINT uFlags)
{
    return _emenu_api_hook::MyInsertMenuW_hook(hMenu, uPosition, uFlags, uIDNewItem, lpNewItem);
    return InsertMenuW(hMenu, uPosition, uFlags, uIDNewItem, lpNewItem);
}

bool _menu_pop(HWND hWnd, HMENU hMenu, int x, int y)
{
    return s_info->iDraw->EMenu_PopUpMenu(hWnd, hMenu, x, y);
}


inline void _recreate_menu(HMENU hMenu, HMENU hCopyToMenu)
{
    return;
    MENUITEMINFOW item = { 0 };
    int count = GetMenuItemCount(hMenu);
    for (int i = count - 1; i >= 0; i--)
    {
        item.cbSize = sizeof(item);
        item.fMask = MIIM_SUBMENU | MIIM_FTYPE | MIIM_ID;
        _emenu_api_hook::GetMenuItemInfoW_hook(hMenu, i, true, &item);

        auto it = s_mapMenu.find(item.wID);
        if (it != s_mapMenu.end())
        {
            MENU_ID_DATA& data = it->second;
            DWORD style = MF_BYPOSITION | data.fType | MF_OWNERDRAW | item.fType;
            if (item.hSubMenu)
            {
                HMENU hPopupMenu = _menu_create();
                data.id = (UINT_PTR)hPopupMenu;
                _recreate_menu(item.hSubMenu, hPopupMenu);
                RemoveMenu(hMenu, i, MF_BYPOSITION);
                _emenu_api_hook::InsertMenuW_hook(hCopyToMenu, i, style | MF_POPUP, (UINT_PTR)hPopupMenu, data.text);
                s_mapMenu[data.id] = data;
            }
            else
            {
                DeleteMenu(hMenu, i, MF_BYPOSITION);
                _emenu_api_hook::InsertMenuW_hook(hCopyToMenu, i, style, item.wID, data.text);
            }
        }

    }
}
void _change_menu_title(HMENU hMenu)
{
    return;
    static UINT dpi = 0;
    MENUITEMINFOW item = { 0 };
    if (dpi != s_info->dpiX || 1)
    {
        dpi = s_info->dpiX;
        _recreate_menu(hMenu, hMenu);
        return;
        int count = GetMenuItemCount(hMenu);
        for (int i = 0; i < count; i++)
        {
            int id = GetMenuItemID(hMenu, i);
            auto it = s_mapMenu.find(id);
            if (it != s_mapMenu.end())
            {
                MENU_ID_DATA& data = it->second;
                item.cbSize = sizeof(item);
                item.fMask = MIIM_FTYPE | MIIM_STRING;
                item.fType &= ~s_info->MY_MF_OWNERDRAW;
                item.dwTypeData = data.text;
                _emenu_api_hook::SetMenuItemInfoW_hook(hMenu, id, false, &item);

                //item.fMask = MIIM_FTYPE;
                //item.fType = 0;
                //_emenu_api_hook::GetMenuItemInfoW_hook(hMenu, id, false, &item);

                //item.fMask = MIIM_FTYPE | MIIM_STRING;
                //item.fType |= data.fType | s_info->MY_MF_OWNERDRAW;
                //_emenu_api_hook::SetMenuItemInfoW_hook(hMenu, id, false, &item);

                //item.fMask = MIIM_FTYPE;
                //item.fType = 0;
                //_emenu_api_hook::GetMenuItemInfoW_hook(hMenu, id, false, &item);
            }
        }
    }
}
FN_TrackPopupMenuEx GetTrackPopupMenuEx()
{
    //_change_menu_title(s_info->hMenuPopup);
    return _emenu_api_hook::TrackPopupMenuEx_hook;
}

HMENU _menu_copy_menu1(HMENU hMenu, MENU_COPY_STRUCT& ret)
{
    int count = GetMenuItemCount(hMenu);
    if ( count < 1 )
        return 0;

    HMENU hMenuCopy = CreatePopupMenu();
    const bool isDraw = s_iDrawmenu.find(hMenu) != s_iDrawmenu.end();
    if ( isDraw )
        EMenu_InsertMenu(hMenuCopy);
    for ( int i = 0; i < count; i++ )
    {
        MENUITEMINFOW mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU | MIIM_DATA;
        mii.cch = 256;
        wchar_t buf[256] = { 0 };
        mii.dwTypeData = buf;
        //BOOL isOk = _emenu_api_hook::GetMenuItemInfoW_hook(hMenu, i, TRUE, &mii);
        BOOL isOk = GetMenuItemInfoW(hMenu, i, TRUE, &mii);
        //wstr::dbg(L"%d    %d : %s\n", i, mii.wID, buf);
        if ( mii.hSubMenu )
        {
            //MENUINFO info1 = { 0 };
            //info1.cbSize = sizeof(info1);
            //HMENU hTest = mii.hSubMenu;
            //info1.fMask = MIM_APPLYTOSUBMENUS | MIM_HELPID;
            //isOk = SetMenuInfo(hTest, &info1);
            HMENU hSubMenu = _menu_copy_menu1(mii.hSubMenu, ret);   // 如果有子菜单, 递归拷贝子菜单
            if ( !hSubMenu )
            {
                hSubMenu = mii.hSubMenu;
                mii.hSubMenu = hSubMenu;
                MENU_COPY_STRUCT::DEL_MENU del_menu = { hMenuCopy, (UINT)mii.hSubMenu };
                ret.arr_delete.push_back(del_menu);

                //wstr::dbg(L"%d    id = %d, 菜单句柄 = %p, 父句柄 = %p, 标题 = %s\n", i, mii.wID, hSubMenu, hMenuCopy, buf);

            }
            else
            {
                mii.hSubMenu = hSubMenu;
            }
            //mii.wID = (UINT)mii.hSubMenu;
            //mii.fType |= MF_POPUP;
        }
        //mii.fType |= s_info->MY_MF_OWNERDRAW;
        InsertMenuItemW(hMenuCopy, i, TRUE, &mii);
        ////_emenu_api_hook::InsertMenuItemW_hook(hMenuCopy, i, TRUE, &mii);
        //UINT uFlags = s_info->MY_MF_OWNERDRAW | mii.fType | mii.fState;
        ////_emenu_api_hook::AppendMenuW_hook(hMenuCopy, uFlags, mii.wID, buf);

        //_emenu_api_hook::GetMenuItemInfoW_hook(hMenu, i, TRUE, &mii);
    }
    return hMenuCopy;
}

// 弹出菜单前, 拷贝所有菜单项, 然后弹出这个拷贝出来的菜单项, 最后删除
MENU_COPY_STRUCT _menu_copy_menu(HMENU hMenu, HWND hWnd)
{
    MENU_COPY_STRUCT ret;
    //if ( !_emenu_api_hook::isDesignerWnd(hWnd) )
    //{
    //    ret.isDestroy = true;
    //    ret.hMenu = _menu_copy_menu1(hMenu, ret);
    //}

    if ( !ret.hMenu )
    {
        ret.hMenu = hMenu;
        ret.isDestroy = false;
    }
    return ret;
}
