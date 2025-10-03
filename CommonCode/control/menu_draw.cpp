#include "menu_draw.h"

#include <CThemeColor.h>
#include <gdiplus.h>
#include <comdef.h>
#include <CStringBuffer.h>
#include <hook_detours/ApiHook.h>
#include <stack>
#include <gdiplus/drawImage.h>

#pragma comment(lib, "gdiplus.lib")

typedef bool (CALLBACK* PFN_MENU_DRAWITEM)(HWND hWnd, LPDRAWITEMSTRUCT dw, LPCWSTR text);

#define DRAW_MENU_NAMESPACE_BEGIN namespace draw_menu{
#define DRAW_MENU_NAMESPACE_END }

DRAW_MENU_NAMESPACE_BEGIN

struct MENU_COPY_STRUCT
{
    struct DEL_MENU
    {
        HMENU       hMenu;
        ULONG_PTR   hDel;
    };
    std::vector<DEL_MENU> arr_delete;  // 销毁菜单前需要删除的菜单项
    HMENU   hMenu;
    BOOL    isDestroy;
    HWND    hWnd;
    WNDPROC oldProc;
    MENU_COPY_STRUCT() : hMenu(0), isDestroy(0), hWnd(0), oldProc(0)
    {

    }
    inline void free()
    {
        BOOL isOk = false;
        for (DEL_MENU& item : arr_delete)
        {
            isOk = RemoveMenu(item.hMenu, (UINT)item.hDel, MF_BYCOMMAND);
        }
        if (isDestroy)
            DestroyMenu(hMenu);

    }
    inline operator HMENU()
    {
        return hMenu;
    }
};

struct MENU_ID_STRUCT
{
    UINT        id;
    HMENU       hMenu;          // 这个菜单条目所在的菜单句柄
    int         width;
    int         height;
    LPCWSTR     text;           // 菜单标题文本
    UINT        flags;          // 菜单标识
    UINT        fType;          // used if MIIM_TYPE (4.0) or MIIM_FTYPE (>4.0)
    UINT        fState;         // used if MIIM_STATE
    HMENU       hSubMenu;       // used if MIIM_SUBMENU
    HBITMAP     hbmpChecked;    // used if MIIM_CHECKMARKS
    HBITMAP     hbmpUnchecked;  // used if MIIM_CHECKMARKS
    ULONG_PTR   dwItemData;     // used if MIIM_DATA
    HBITMAP     hbmpItem;       // used if MIIM_BITMAP
};

struct MENU_DRAW_FONT
{
    HFONT hFont;
    HFONT hFontBold;
};
struct MENU_DRAW_BITMAP
{
    Gdiplus::Bitmap* bmp;
    HICON hIcon;
    int cxIcon;
    int cyIcon;
};

static HHOOK s_hhk; // cbt钩子
static std::stack<HWND> s_stack_menu_wnd;               // 当前子类化的菜单窗口层次, 最后一个push的就是当前显示的菜单

static apiHook hook_TrackPopupMenu;
static apiHook hook_TrackPopupMenuEx;
static UINT m_now_menu_window_dpi;  // 当前弹出菜单的窗口的dpi, 菜单字体需要根据这个dpi来获取
static HWND m_now_menu_window;      // 当前弹出菜单的窗口句柄
static THEMECOLOR_STRUCT m_clr;     // 颜色配色
static bool m_isClr;                // 是否是否clr这个结构的配色
static std::unordered_map<UINT, MENU_DRAW_FONT> m_font_dpi;         // 保存每个dpi对应的字体, 避免重复创建
static std::unordered_map<UINT, MENU_DRAW_BITMAP> m_map_check;      // 菜单选中项的打钩, 键名是高位表示是否是深色模式, 低位是dpi
static std::unordered_map<UINT, MENU_DRAW_BITMAP> m_map_triangle;   // 菜单右边的小三角, 键名是高位表示是否是深色模式, 低位是dpi
static std::unordered_map<int, MENU_ID_STRUCT> m_map_menu;
static CStringBufferW m_menu_buffer;
static std::unordered_map<HWND, PFN_MENU_DRAWITEM> m_map_drawtext;


static BOOL(WINAPI* TrackPopupMenu_hook)(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT* prcRect) = TrackPopupMenu;
static BOOL(WINAPI* TrackPopupMenuEx_hook)(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) = TrackPopupMenuEx;
BOOL WINAPI MyTrackPopupMenu_hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT* prcRect);
BOOL WINAPI MyTrackPopupMenuEx_hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm);

void _menu_draw_item(HWND hWnd, LPDRAWITEMSTRUCT dw, MENU_ID_STRUCT& data);
void _menu_draw_line(LPDRAWITEMSTRUCT dw);

LRESULT CALLBACK CBTProc_PopupMenuWindow(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SystemMenu_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT _menu_draw_OnMeasureItem(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT _menu_draw_OnDrawItem(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);




MENU_COPY_STRUCT _menu_copy_menu(HMENU hMenu, HWND hWnd);



DRAW_MENU_NAMESPACE_END


void menu_draw_init()
{
    if (!draw_menu::s_hhk)
    {
        draw_menu::hook_TrackPopupMenu.hook(&(PVOID&)draw_menu::TrackPopupMenu_hook, draw_menu::MyTrackPopupMenu_hook);
        draw_menu::hook_TrackPopupMenu.hook(&(PVOID&)draw_menu::TrackPopupMenu_hook, draw_menu::MyTrackPopupMenu_hook);
        draw_menu::m_menu_buffer.reserve(0x4000);

        draw_menu::s_hhk = SetWindowsHookExW(WH_CBT, draw_menu::CBTProc_PopupMenuWindow, GetModuleHandleW(0), GetCurrentThreadId());
    }
}

void menu_draw_OnDrawItemText(HWND hWnd, PFN_MENU_DRAWITEM pfn)
{
    draw_menu::m_map_drawtext[hWnd] = pfn;
}

HFONT menu_draw_GetFont(HFONT* hFontBold)
{
    draw_menu::MENU_DRAW_FONT font = draw_menu::m_font_dpi[draw_menu::m_now_menu_window_dpi];
    if (hFontBold)
        *hFontBold = font.hFontBold;
    return font.hFont;
}


DRAW_MENU_NAMESPACE_BEGIN
static int _menu_scale(int n)
{
    if (m_now_menu_window_dpi == 0)
        return n;
    double scale = (double)m_now_menu_window_dpi / (double)USER_DEFAULT_SCREEN_DPI;
    return (int)((LONG64)((double)n * scale));

    return MulDiv(n, m_now_menu_window_dpi, USER_DEFAULT_SCREEN_DPI);
}

constexpr LPCWSTR pszMenuPropName = L"{673AD1B1-0554-4F90-8CC7-4948CF18FA49}";
static LRESULT CALLBACK MenuDrawProc_DrawMenu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WNDPROC oldProc = (WNDPROC)GetPropW(hWnd, pszMenuPropName);
    switch (message)
    {
    case WM_MEASUREITEM:
        return _menu_draw_OnMeasureItem(oldProc, hWnd, message, wParam, lParam);
    case WM_DRAWITEM:
        return _menu_draw_OnDrawItem(oldProc, hWnd, message, wParam, lParam);
        break;
    default:
        break;
    }
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

}

static HFONT _popup_menucreate_font(const wchar_t* name = L"微软雅黑", int size = -12, bool lfWeight = false, bool lfItalic = false, bool lfUnderline = false, bool lfStrikeOut = false)
{
    LOGFONTW lf = { 0 };
    SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);
    lf.lfHeight = size;
    if (lfWeight)
        lf.lfWeight = FW_BOLD;
    lf.lfItalic = lfItalic;
    lf.lfUnderline = lfUnderline;
    lf.lfStrikeOut = lfStrikeOut;
    lf.lfCharSet = GB2312_CHARSET;
    if (!name) name = L"微软雅黑";
    size_t len = wcslen(name) + 1;
    memcpy(lf.lfFaceName, name, len * sizeof(wchar_t));
    return CreateFontIndirectW(&lf);
}
#include <GetDpiForWindow.h>
static MENU_COPY_STRUCT _popup_menu_begin(HWND hWnd, HMENU hMenu)
{
    //HMENU hadskj = LoadMenuW(g_hInst, MAKEINTRESOURCEW(IDC_SPY));
    //return TrackPopupMenu_hook(hadskj, uFlags, x, y, nReserved, hWnd, prcRect);

    m_isClr = CThemeColor::GetThemeColorFromMap(&m_clr);

    m_now_menu_window = hWnd;
    m_now_menu_window_dpi = GetDpiForWindow_d(m_now_menu_window);
    MENU_DRAW_FONT& font = m_font_dpi[m_now_menu_window_dpi];
    if (!font.hFont)
    {
        const UINT scale = m_now_menu_window_dpi * 100 / USER_DEFAULT_SCREEN_DPI;
        const int lfHeight = (int)((-12.f) * ((float)scale / 100.f));
        font.hFont = _popup_menucreate_font(L"微软雅黑", lfHeight);
        font.hFontBold = _popup_menucreate_font(L"微软雅黑", lfHeight, true);
    }
    WNDPROC oldProc = (WNDPROC)GetWindowLongPtrW(hWnd, GWLP_WNDPROC);
    if (oldProc != MenuDrawProc_DrawMenu)
    {
        oldProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)MenuDrawProc_DrawMenu);
        SetPropW(hWnd, pszMenuPropName, (HANDLE)oldProc);
    }

    MENU_COPY_STRUCT hMenuCopy = _menu_copy_menu(hMenu, hWnd);
    hMenuCopy.hWnd = hWnd;
    hMenuCopy.oldProc = oldProc;
    return hMenuCopy;
}

static void _popup_menu_end(MENU_COPY_STRUCT& hMenuCopy)
{
    m_now_menu_window = 0;
    m_menu_buffer.clear();
    m_map_menu.clear();
    SetWindowLongPtrW(hMenuCopy.hWnd, GWLP_WNDPROC, (LONG_PTR)hMenuCopy.oldProc);
    RemovePropW(hMenuCopy.hWnd, pszMenuPropName);
    hMenuCopy.free();
}
BOOL WINAPI MyTrackPopupMenu_hook(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT* prcRect)
{
    MENU_COPY_STRUCT hMenuCopy = _popup_menu_begin(hWnd, hMenu);
    BOOL ret = TrackPopupMenu_hook(hMenuCopy, uFlags, x, y, nReserved, hWnd, prcRect);
    _popup_menu_end(hMenuCopy);
    return ret;
}

BOOL __stdcall MyTrackPopupMenuEx_hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm)
{
    MENU_COPY_STRUCT hMenuCopy = _popup_menu_begin(hWnd, hMenu);
    BOOL ret = TrackPopupMenuEx_hook(hMenuCopy, uFlags, x, y, hWnd, lptpm);
    _popup_menu_end(hMenuCopy);
    return ret;
}


static HMENU _menu_copy_menu1(HMENU hMenu, MENU_COPY_STRUCT& ret)
{
    int count = GetMenuItemCount(hMenu);
    if (count < 1)
        return 0;

    HMENU hMenuCopy = CreatePopupMenu();
    for (int i = 0; i < count; i++)
    {
        MENUITEMINFOW mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU | MIIM_DATA;
        mii.cch = 256;
        wchar_t buf[256] = { 0 };
        mii.dwTypeData = buf;
        BOOL isOk = GetMenuItemInfoW(hMenu, i, TRUE, &mii);
        if (mii.hSubMenu)
        {
            HMENU hSubMenu = _menu_copy_menu1(mii.hSubMenu, ret);   // 如果有子菜单, 递归拷贝子菜单
            if (!hSubMenu)
            {
                hSubMenu = mii.hSubMenu;
                mii.hSubMenu = hSubMenu;
                MENU_COPY_STRUCT::DEL_MENU del_menu = { hMenuCopy, (ULONG_PTR)mii.hSubMenu };
                ret.arr_delete.push_back(del_menu);
            }
            else
            {
                mii.hSubMenu = hSubMenu;
            }
        }

        HDC hdc = GetDC(0);
        RECT rc = { 0 };
        DrawTextW(hdc, buf, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
        ReleaseDC(0, hdc);

        MENU_ID_STRUCT& data = m_map_menu[mii.wID];
        data.width = rc.right - rc.left;
        data.height = 22;
        data.text = m_menu_buffer.AddString(buf);
        data.id = mii.wID;
        data.flags = mii.fType;
        data.fType = mii.fType;
        data.fState = mii.fState;
        data.hSubMenu = mii.hSubMenu;
        data.hbmpChecked = mii.hbmpChecked;
        data.hbmpUnchecked = mii.hbmpUnchecked;
        data.dwItemData = mii.dwItemData;
        data.hbmpItem = mii.hbmpItem;

        mii.fType |= MFT_OWNERDRAW;
        InsertMenuItemW(hMenuCopy, i, TRUE, &mii);
    }
    return hMenuCopy;
}

// 弹出菜单前, 拷贝所有菜单项, 然后弹出这个拷贝出来的菜单项, 最后删除
MENU_COPY_STRUCT _menu_copy_menu(HMENU hMenu, HWND hWnd)
{
    MENU_COPY_STRUCT ret;
    //if (m_now_menu_window_dpi != 96)
    {
        ret.isDestroy = true;
        ret.hMenu = _menu_copy_menu1(hMenu, ret);
    }
    if (!ret.hMenu)
    {
        ret.hMenu = hMenu;
        ret.isDestroy = false;
    }
    return ret;
}

// 获取系统图标句柄, 返回句柄, 失败返回0
static HICON _menu_draw_get_system_icon(LPCWSTR fileName, int index, bool isBig, int& cxIcon, int& cyIcon)
{
    wchar_t file[520];
    if (GetSystemDirectoryW(file, sizeof(file) / sizeof(file[0])) == 0)
        return 0;

    wcscat_s(file, L"\\");
    wcscat_s(file, fileName);

    int count = ExtractIconExW(file, -1, 0, 0, 0);
    if (count <= 0)
        return 0;

    HICON hIcon = 0;
    if (isBig)
        ExtractIconExW(file, index, &hIcon, 0, 1);
    else
        ExtractIconExW(file, index, 0, &hIcon, 1);

    ICONINFO ii;
    BOOL fResult = GetIconInfo(hIcon, &ii);
    if (fResult)
    {
        BITMAP bm{};
        fResult = GetObjectW(ii.hbmMask, sizeof(bm), &bm) == sizeof(bm);
        if (fResult)
        {
            cxIcon = bm.bmWidth;
            cyIcon = ii.hbmColor ? bm.bmHeight : bm.bmHeight / 2;
        }
        if (ii.hbmMask)  DeleteObject(ii.hbmMask);
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
    }

    return hIcon;
};

static MENU_DRAW_BITMAP& _menu_draw_get_image(HWND hWnd, LPCBYTE pImage, int nSize,
                                              LPCWSTR fileName, bool isDark, int index, bool isBig,
                                              std::unordered_map<UINT, MENU_DRAW_BITMAP>& map)
{
    // 根据深色或者浅色模式获取不同的图标
    const int dpi = m_now_menu_window_dpi;  // dpi大于96那就获取大图标
    const UINT key = MAKELONG(dpi, isDark);
    MENU_DRAW_BITMAP& bitmap_info = map[key];

    if (bitmap_info.hIcon || bitmap_info.bmp)
        return bitmap_info;
    

    // imageres.dll, 280蓝色小三角, 其实就是个播放图标, 也能使用
    // shell32.dll, 137 也是蓝色三角
    // 296/302 绿色打钩, 144 红色打钩
    HICON hIcon = _menu_draw_get_system_icon(fileName, index, isBig, bitmap_info.cxIcon, bitmap_info.cyIcon);
    //HICON hIcon = _menu_draw_get_system_icon(L"shell32.dll", 296, dpi > 96);


    if (hIcon)
    {
        bitmap_info.bmp = new Gdiplus::Bitmap(hIcon);
        bitmap_info.hIcon = hIcon;
        return bitmap_info;
    }

    bitmap_info.bmp = _img_create_from_memory(pImage, nSize);
    bitmap_info.cxIcon = (int)bitmap_info.bmp->GetWidth();
    bitmap_info.cyIcon = (int)bitmap_info.bmp->GetHeight();
    return bitmap_info;
}
static MENU_DRAW_BITMAP& _menu_draw_get_check_image(HWND hWnd, int& cxIcon, int& cyIcon)
{
    const BYTE iconData[] =
    {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10,
        0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0xF3, 0xFF, 0x61, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59,
        0x73, 0x00, 0x00, 0x0E, 0xC4, 0x00, 0x00, 0x0E, 0xC4, 0x01, 0x95, 0x2B, 0x0E, 0x1B, 0x00, 0x00, 0x00, 0xB6, 0x49, 0x44,
        0x41, 0x54, 0x78, 0x5E, 0xBD, 0xD3, 0x31, 0x0A, 0xC2, 0x30, 0x14, 0xC7, 0xE1, 0x67, 0x26, 0x07, 0x67, 0xA7, 0x82, 0x10,
        0x3D, 0x84, 0x37, 0xF0, 0x00, 0xE2, 0x21, 0x7A, 0x05, 0x77, 0x1D, 0xC5, 0xC9, 0xC9, 0x5D, 0x07, 0xE7, 0x4E, 0x05, 0xBD,
        0x80, 0x38, 0x38, 0x74, 0x48, 0x71, 0xD2, 0x45, 0xC4, 0xA9, 0x64, 0x7D, 0xBE, 0x27, 0x04, 0x95, 0x0A, 0x7F, 0xAB, 0x60,
        0xA1, 0x3C, 0x42, 0xF3, 0xFD, 0x42, 0x86, 0xD6, 0x98, 0x99, 0xFE, 0xFE, 0x78, 0xEF, 0xEB, 0x01, 0x98, 0x2F, 0x70, 0x2C,
        0x63, 0x2F, 0x33, 0xAA, 0x1C, 0x50, 0x7C, 0xBA, 0xFA, 0xD9, 0x34, 0xC9, 0x3A, 0x02, 0x36, 0x1A, 0x31, 0x55, 0x71, 0x6F,
        0x94, 0xD2, 0x70, 0xB1, 0x23, 0x89, 0xB4, 0x05, 0xAC, 0x4D, 0xE9, 0x5E, 0x00, 0x1F, 0xCE, 0x05, 0xD9, 0x66, 0x83, 0x06,
        0xDD, 0x96, 0xB2, 0x49, 0xC0, 0x91, 0xBC, 0x4E, 0x37, 0xBE, 0xC3, 0xEE, 0x78, 0x61, 0x1B, 0x2F, 0x99, 0xFA, 0xF3, 0xFB,
        0xD4, 0xB5, 0x46, 0x9F, 0x71, 0x3E, 0x5E, 0x6D, 0x5F, 0x3E, 0x60, 0xFC, 0xD8, 0xE4, 0x04, 0x97, 0xEA, 0x10, 0xA3, 0x53,
        0x10, 0x46, 0x11, 0x80, 0x71, 0x04, 0x60, 0x1C, 0x01, 0x18, 0x47, 0x10, 0xC6, 0x91, 0x4F, 0xF1, 0xCF, 0xBF, 0xF3, 0x0D,
        0x7D, 0x9A, 0x1B, 0xDB, 0x87, 0x01, 0x4F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
    };
    const bool isDark = m_isClr ? m_clr.isDarkMode : false;
    MENU_DRAW_BITMAP& bitmap_info = _menu_draw_get_image(hWnd, iconData, sizeof(iconData),
                                                         L"aclui.dll", isDark, isDark ? 1 : 0,
                                                         m_now_menu_window_dpi > 120, m_map_check);
    cxIcon = bitmap_info.cxIcon;
    cyIcon = bitmap_info.cyIcon;
    return bitmap_info;
}

// 获取菜单右边的三角形图标
static MENU_DRAW_BITMAP& _menu_draw_get_triangle_image(HWND hWnd, int& cxIcon, int& cyIcon)
{
    const BYTE iconData[] =
    {
        0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10,
        0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0xF3, 0xFF, 0x61, 0x00, 0x00, 0x00, 0xAB, 0x49, 0x44, 0x41,
        0x54, 0x38, 0x4F, 0x63, 0x64, 0xA0, 0x10, 0x30, 0x52, 0xA8, 0x9F, 0x81, 0x46, 0x06, 0x94, 0x9D, 0x93, 0x62, 0xE0, 0x32,
        0x7C, 0xC1, 0xD0, 0xC0, 0xF8, 0x8F, 0x90, 0x0B, 0xB1, 0xBB, 0xA0, 0xEA, 0xB2, 0x38, 0xC3, 0xFF, 0x1F, 0x0B, 0x18, 0xD8,
        0x4D, 0xBC, 0x09, 0x19, 0x82, 0xDD, 0x80, 0xD2, 0x53, 0x12, 0x0C, 0xAC, 0x4C, 0xCF, 0x19, 0xFE, 0xFF, 0xDF, 0x41, 0xC8,
        0x10, 0xFC, 0x06, 0x80, 0xDC, 0x0F, 0x32, 0xE4, 0xCE, 0x7D, 0x1F, 0x86, 0xD5, 0x61, 0x7F, 0xB1, 0x79, 0x07, 0xBB, 0x01,
        0x0D, 0xA7, 0x24, 0x18, 0x7E, 0x31, 0x3D, 0x87, 0x6B, 0x60, 0xFC, 0xBF, 0x83, 0xE1, 0x16, 0x76, 0x43, 0x70, 0x84, 0xC1,
        0x19, 0x49, 0x06, 0x06, 0x86, 0x67, 0x48, 0x36, 0xAE, 0x62, 0x68, 0x33, 0x09, 0x27, 0xDE, 0x05, 0x55, 0x28, 0x06, 0xE0,
        0xD4, 0x0C, 0x32, 0x90, 0x90, 0x0B, 0xF0, 0x6A, 0xC6, 0x6D, 0x40, 0xD9, 0x11, 0x29, 0x06, 0x56, 0xCE, 0x3E, 0x86, 0x56,
        0xE3, 0x08, 0xF2, 0xD2, 0x41, 0xE5, 0x49, 0x61, 0x86, 0x76, 0xF3, 0xB7, 0x84, 0x34, 0xE3, 0x76, 0x01, 0x31, 0x3A, 0xA1,
        0x6A, 0x68, 0x94, 0x17, 0x48, 0x70, 0x01, 0x00, 0x50, 0x7C, 0x35, 0x11, 0x56, 0xD9, 0x07, 0x76, 0x00, 0x00, 0x00, 0x00,
        0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
    };

    const bool isDark = m_isClr ? m_clr.isDarkMode : false;
    MENU_DRAW_BITMAP& bitmap_info = _menu_draw_get_image(hWnd, iconData, sizeof(iconData),
                                                         L"shell32.dll", isDark, 137,
                                                         m_now_menu_window_dpi > 120, m_map_triangle);

    cxIcon = bitmap_info.cxIcon;
    cyIcon = bitmap_info.cyIcon;
    return bitmap_info;
}

LRESULT _menu_draw_OnMeasureItem(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPMEASUREITEMSTRUCT item = (LPMEASUREITEMSTRUCT)lParam;
    if (item->CtlType != ODT_MENU)
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

    MENU_ID_STRUCT& data = m_map_menu[item->itemID];
    if (!data.id)// || item->itemID == -1)
    {
        item->itemHeight = 5;
        if (item->itemHeight < 1)
            item->itemHeight = _menu_scale(5);
        else if (item->itemHeight > 50)
            item->itemHeight = _menu_scale(5);
    }
    else
    {
        item->itemWidth  = _menu_scale(data.width + 60);
        item->itemHeight = _menu_scale(data.height);
    }

    return 1;
}

LRESULT _menu_draw_OnDrawItem(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPDRAWITEMSTRUCT dw = (LPDRAWITEMSTRUCT)lParam;
    if (dw->CtlType != ODT_MENU)
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

    HDC hdc = dw->hDC;
    SelectClipRgn(hdc, 0);
    MENU_ID_STRUCT& data = m_map_menu[dw->itemID];
    const int offsetTriangle = _menu_scale(20);  // 三角偏移, 右边留空这么多像素不绘画

    if ((data.fType & MFT_SEPARATOR) == MFT_SEPARATOR)
    {
        _menu_draw_line(dw);
    }
    else
    {
        _menu_draw_item(hWnd, dw, data);
    }

    if (data.hSubMenu)
    {
        // 有子项, 绘画后面的三角

        const int cxItem = dw->rcItem.right - dw->rcItem.left;
        const int cyItem = dw->rcItem.bottom - dw->rcItem.top;

        int cxIcon = 0, cyIcon = 0;
        MENU_DRAW_BITMAP& bitmap_info = _menu_draw_get_triangle_image(hWnd, cxIcon, cyIcon);

        Gdiplus::Rect rcTriangle;
        rcTriangle.Width = cxIcon;
        rcTriangle.Height = cyIcon;
        if (rcTriangle.Height > cyItem)
        {
            rcTriangle.Width = cyItem - 2;
            rcTriangle.Height = cyItem - 2;
        }

        rcTriangle.X = (((cxItem - offsetTriangle) + ((offsetTriangle - rcTriangle.Width) / 2)));
        rcTriangle.Y = dw->rcItem.top + ((cyItem - rcTriangle.Height) / 2);


        if (bitmap_info.hIcon)
        {
            const int left = rcTriangle.X;
            const int top = rcTriangle.Y;
            DrawIconEx(hdc, left, top, bitmap_info.hIcon, rcTriangle.Width, rcTriangle.Height, 0, 0, DI_NORMAL);
        }
        else
        {
            Gdiplus::Graphics gp(hdc);
            gp.DrawImage(bitmap_info.bmp, rcTriangle);
            //_canvas_drawimagerectrect(hCanvas, hIcon,
            //                          rcTriangle.left, rcTriangle.top, rcTriangle.right, rcTriangle.bottom,
            //                          rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 255);

        }
    }

    const int cxItem = dw->rcItem.right - dw->rcItem.left;
    const int cyItem = dw->rcItem.bottom - dw->rcItem.top;
    RECT rcRgn = { 0, 0, cxItem, cyItem };

    rcRgn.top = offsetTriangle;
    rcRgn.bottom = cxItem - offsetTriangle;

    if (data.hSubMenu)  // 已经获取到三角图标, 那就设置裁剪区, 不让他绘画默认的三角
    {
        rcRgn.right -= offsetTriangle;
    }

    //ExcludeClipRect(hdc, rcRgn.left, rcRgn.top, rcRgn.right, rcRgn.bottom);

    COLORREF clrLine = m_isClr ? m_clr.crMenuBorder : GetSysColor(COLOR_WINDOWFRAME);
    HPEN hPenLine = CreatePen(PS_SOLID, 1, clrLine);

    HGDIOBJ hObdBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HGDIOBJ hObdPen   = SelectObject(hdc, hPenLine);

    BITMAP bm = { 0 };
    HBITMAP hBitmap = (HBITMAP)GetCurrentObject(dw->hDC, OBJ_BITMAP);
    int bmSize = GetObjectW(hBitmap, sizeof(bm), &bm);
    const int cxClient = bm.bmWidth;
    const int cyClient = bm.bmHeight;

    Rectangle(hdc, 0, 0, cxClient, cyClient);

    HRGN hRgn = CreateRectRgn(rcRgn.left, rcRgn.top, rcRgn.right, rcRgn.bottom);
    SelectClipRgn(hdc, hRgn);
    SelectObject(hdc, hObdBrush);
    SelectObject(hdc, hObdPen);

    DeleteObject(hRgn);
    DeleteObject(hPenLine);

    return 1;
}

void _menu_draw_item(HWND hWnd, LPDRAWITEMSTRUCT dw, MENU_ID_STRUCT& data)
{
    COLORREF clrBack, clrText, clrBackSel, clrTextSel, clrTextGray;
    if (m_isClr)
    {
        clrBack = m_clr.crMenuBack;
        clrText = m_clr.crMenuText;
        clrBackSel = m_clr.crMenuHot;
        clrTextSel = m_clr.crMenuTextHot;
        clrTextGray = m_clr.crMenuTextBan;
    }
    else
    {
        clrBack = GetSysColor(COLOR_MENU);
        clrText = GetSysColor(COLOR_MENUTEXT);
        clrBackSel = GetSysColor(COLOR_MENUHILIGHT);
        clrTextSel = GetSysColor(COLOR_HIGHLIGHTTEXT);
        clrTextGray = GetSysColor(COLOR_GRAYTEXT);
    }


    const int cxItem = dw->rcItem.right - dw->rcItem.left;
    const int cyItem = dw->rcItem.bottom - dw->rcItem.top;
    const int offsetTriangle = _menu_scale(20);  // 三角偏移, 右边留空这么多像素不绘画
    HDC hdc = dw->hDC;

    RECT rcItem = dw->rcItem;
    rcItem.left     += _menu_scale(30);
    rcItem.top      += _menu_scale(5);
    rcItem.right    -= _menu_scale(20);
    rcItem.bottom   -= _menu_scale(5);
    COLORREF clrBackDraw = clrBack;
    COLORREF clrTextOld = SetTextColor(hdc, clrText);
    if (dw->itemState & ODS_SELECTED)       // 菜单项的状态被选择, 鼠标移动热点效果
    {
        if (dw->itemState & ODS_DISABLED)
        {
            BYTE r = GetRValue(clrBack) - 12;
            BYTE g = GetGValue(clrBack) - 12;
            BYTE b = GetBValue(clrBack) - 12;
            clrBackDraw = RGB(r, g, b);
            SetTextColor(hdc, clrTextGray);
        }
        else
        {
            clrBackDraw = clrBackSel;
            //SetTextColor(hdc, clrTextSel);
        }
    }
    else if (dw->itemState & ODS_DISABLED)
    {
        SetTextColor(hdc, clrTextGray);
        clrBackDraw = clrBack;
    }

    //clrBackDraw = GetSysColor(COLOR_SCROLLBAR);

    HBRUSH hbr = CreateSolidBrush(clrBackDraw);
    FillRect(hdc, &dw->rcItem, hbr);
    DeleteObject(hbr);

    int oldBkMode = SetBkMode(hdc, TRANSPARENT);

    RECT rcIcon = { 0 };
    int cxIcon = 0, cyIcon = 0;
    Gdiplus::Bitmap* hIcon = 0;
    if (dw->itemState & ODS_CHECKED)    // 选中了菜单, 需要画上选择框, 必须在填充背景之后绘画
    {
        MENU_DRAW_BITMAP& bitmap_info = _menu_draw_get_check_image(hWnd, cxIcon, cyIcon);

        Gdiplus::Rect rcDraw;
        rcDraw.Width = cxIcon;
        rcDraw.Height = cyIcon;
        if (rcDraw.Height > cyItem)
        {
            rcDraw.Width = cyItem - 2;
            rcDraw.Height = cyItem - 2;
        }

        rcDraw.X = dw->rcItem.left + 5;
        rcDraw.Y = rcItem.top + ((rcItem.bottom - rcItem.top) - rcDraw.Height) / 2;

        if (bitmap_info.hIcon)
        {
            const int left = rcDraw.X;
            const int top = rcDraw.Y;
            DrawIconEx(hdc, left, top, bitmap_info.hIcon, rcDraw.Width, rcDraw.Height, 0, 0, DI_NORMAL);
        }
        else
        {
            Gdiplus::Graphics gp(hdc);
            gp.DrawImage(hIcon, rcDraw);

            //BitBlt(hdc, rcDraw.left, rcDraw.top, rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top, hdcMem, 0, 0, SRCCOPY);
            //_canvas_drawimagerectrect(hCanvas, hIcon, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
            //                          rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 255);
        }
    }


    LPCWSTR lpText = data.text ? data.text : L"";
    size_t len = wcslen(lpText);
    if (len)
    {
        MENU_DRAW_FONT font = m_font_dpi[m_now_menu_window_dpi];
        HGDIOBJ hOldFont = SelectObject(hdc, font.hFont);
        const wchar_t* pos = wcschr(lpText, '\t');
        if (pos) len = pos++ - lpText;   // 有快捷键

        for (auto& it : m_map_drawtext)
        {
            PFN_MENU_DRAWITEM pfn = it.second;
            if (pfn)
                pfn(hWnd, dw, lpText);
        }
        

        //TODO 创建对应dpi的字体, 需要创建全局的
        DrawTextW(hdc, lpText, (int)len, &rcItem, DT_VCENTER | DT_SINGLELINE | DT_HIDEPREFIX);
        if (pos)    // tab键后面有文本, 向右对齐画出
            DrawTextW(hdc, pos, -1, &rcItem, DT_VCENTER | DT_SINGLELINE | DT_RIGHT | DT_HIDEPREFIX);
        SelectObject(hdc, hOldFont);
    }
    SetTextColor(hdc, clrTextOld);
    SetBkMode(hdc, oldBkMode);
}

void _menu_draw_line(LPDRAWITEMSTRUCT dw)
{
    // 分隔条
    RECT rcLine = dw->rcItem;
    rcLine.left += _menu_scale(30);
    rcLine.right -= _menu_scale(10);
    rcLine.top += 2;
    HDC hdc = dw->hDC;
    COLORREF clrLine = m_isClr ? m_clr.crMenuBorder : GetSysColor(COLOR_WINDOWFRAME);
    COLORREF clrBack = m_isClr ? m_clr.crMenuBack : GetSysColor(COLOR_MENU);
    HPEN hPenLine = CreatePen(PS_SOLID, 1, clrLine);
    HBRUSH hbr = CreateSolidBrush(clrBack);

    HGDIOBJ hOldPen = SelectObject(hdc, hPenLine);
    FillRect(hdc, &dw->rcItem, hbr);

    MoveToEx(hdc, rcLine.left, rcLine.top, 0);
    LineTo(hdc, rcLine.right, rcLine.top);
    SelectObject(hdc, hOldPen);

    DeleteObject(hbr);
    DeleteObject(hPenLine);
}


LRESULT CALLBACK CBTProc_PopupMenuWindow(int nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case HCBT_CREATEWND:
    {
        HWND hWnd = (HWND)wParam;
        auto pfn_sub_menu_wnd = [hWnd]()
        {
            wchar_t sClassName[10];
            int len = ::GetClassNameW(hWnd, sClassName, sizeof(sClassName) / sizeof(sClassName[0]));
            //wstr::dbg(L"创建新的窗口, 窗口类名 = %s, s_isSubClass = %d\n", sClassName, s_isSubClass);

            //检查是否菜单窗口
            if (m_now_menu_window == 0)
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
        };
        pfn_sub_menu_wnd();
        break;
    }
    default:
        break;
    }
    return CallNextHookEx(s_hhk, nCode, wParam, lParam);
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
        //wstr::dbg(L"鼠标经过菜单项, wp = %lld, lp = %lld\n", wParam, lParam);
        //if (index >= 0)
        //{
        //    HMENU hMenu = (HMENU)GetPropW(hWnd, PROP_HMENU);
        //    s_info->menuSelIndex = index;
        //    s_info->menuSelHmenu = hMenu;
        //    s_info->menuSelId = GetMenuItemID(hMenu, index);
        //}
        //return 0;
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    case WM_CREATE:
    {
        //首先要去掉菜单窗口的WS_BORDER风格和WS_EX_DLGMODALFRAME , WS_EX_WINDOWEDGE扩展风格
        LRESULT ret = CallWindowProcA(oldProc, hWnd, message, wParam, lParam);

        DWORD dwStyle = (DWORD)::GetWindowLongPtrA(hWnd, GWL_STYLE);
        DWORD dwNewStyle = (dwStyle & ~WS_BORDER);
        ::SetWindowLongPtrA(hWnd, GWL_STYLE, dwNewStyle);

        DWORD dwExStyle = (DWORD)::GetWindowLongPtrA(hWnd, GWL_EXSTYLE);
        DWORD dwNewExStyle = (dwExStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE));
        ::SetWindowLongPtrA(hWnd, GWL_EXSTYLE, dwNewExStyle);

        //dwStyle = GetClassLongPtrW(hWnd, GCL_STYLE);
        //dwStyle &= ~CS_DROPSHADOW;
        //SetClassLongPtrW(hWnd, GCL_STYLE, dwStyle);
        return ret;
    }
    //case MN_SIZEWINDOW:   // MN_SIZEWINDOW
    //{
    //    HMENU hMenu = (HMENU)SendMessageW(hWnd, MN_GETHMENU, 0, 0);
    //    if (hMenu)
    //    {
    //        s_menu_hwnd[hMenu] = hWnd;
    //        SetPropW(hWnd, PROP_HMENU, hMenu);
    //    }
    //    return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    //    break;
    //}
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
        /*
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

            if (info.cxClient > maxWidth)  maxWidth = info.cxClient;
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
        */
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
        //return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        //最后, 因为在MeasureItem里指定的菜单大小，系统会自动加上边框，这里必须去掉这部分额外的尺寸
        //将菜单大小改小
        if (m_now_menu_window)
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
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    default:
        return CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

DRAW_MENU_NAMESPACE_END