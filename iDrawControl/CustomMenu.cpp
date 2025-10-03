#include "EMenu.h"
#include <control/CListView.h>
#include <control/CStatic.h>
#include <control/draw_Window_include.h>/*
#include "inlineassember.api.export.h"
#pragma comment(lib, "../iDrawControl/InlineAssembler.lib")*/

#define ID_LIST_VIEW 1000
#define LISTVIEW_HEIGHT 30

static HWND m_hParent;
static EWINDOW_MENUSTRUCT* m_menu;
static CListView m_list;
static CMyStatic m_static;
static std::vector<EMENU_DATA*> m_menu_data;
static pfn_SubClassWndProc m_oldProc;

struct LISTVIEW_CUSTOM_DATA
{
    LPEX_BRUSH hbrText;
    LPEX_BRUSH hbrHot;
    LPEX_BRUSH hbrSel;
    bool isCheckBox;
};


INT_PTR CALLBACK CustomMenu_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void _cusmenu_update_arr(bool isPaint);
void _cusmenu_create(HWND hWnd);
LRESULT CALLBACK _cusmenu_lv_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void _custommenu_listView_DrawItem(LPCANVAS hCanvas, const RECT& rcItem, LPOBJSTRUCT pData, int i, int index, LISTVIEW_CUSTOM_DATA& draw);


void EMenu_CustomMenu(EWINDOW_MENUSTRUCT* menu)
{
    //size_t index;
    //size_t count = ia_const_resources_count();
    //auto name = ia_const_resources_get_name(0);
    //int asdsa = ia_get_const_resources(name, &index);
    //return;
    m_menu = menu;
    m_hParent = s_info->hEWnd;
    DWORD style = WS_SYSMENU | WS_CAPTION | WS_CLIPSIBLINGS | WS_VISIBLE | WS_THICKFRAME;
    DWORD styleEx = WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT;
    //DialogBoxPop(styleEx, L"自定义菜单", style, 0, 0, 0, 0, s_info->hEWnd, GetModuleHandleW(0), CustomMenu_WndProc);
    CreateDialogBoxPop(styleEx, L"自定义菜单", style, 0, 0, 0, 0, s_info->hEWnd, GetModuleHandleW(0), CustomMenu_WndProc);

    return;
}

void _cusmenu_create(HWND hWnd)
{
    const DWORD style = WS_TABSTOP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD | WS_BORDER
        | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL | LVS_NOCOLUMNHEADER | LVS_OWNERDATA;
    const DWORD styleEx = 0;
    const DWORD styleList = LVS_EX_DOUBLEBUFFER | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT
        | LVS_EX_FLATSB | LVS_EX_INFOTIP;

    m_list.create(hWnd, 0, 0, 0, 0, styleEx, style, styleList, ID_LIST_VIEW, 0);
    m_list.InsertColumn(0, L"项目");

    const LPCWSTR psz_text =
        L"目前就简单的做了这么一个功能\r\n"
        L"\r\n"
        L"    选中项就是在菜单栏显示的菜单\r\n"
        L"    取消选中后不会出现在菜单栏上\r\n"
        L"    能节省顶部标题栏位置的空间\r\n"
        L"\r\n"
        L"调整顺序调整标题菜单啥的以后再说\r\n"
        ;
    const DWORD styleStatic = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | SS_EDITCONTROL;

    m_static.create(0, psz_text, styleStatic, 0, 0, 0, 0, hWnd);
    _cusmenu_update_arr(true);

    DRAW_WINDOW_ARGUMENT arg;
    memset(&arg, 0, sizeof(arg));

    s_info->pfnControls(IDC_IDRAW_DRAW_DEFARG, (WPARAM)hWnd, (LPARAM)&arg);
    arg.crBack = RGB2ARGB(s_info->defData->draw.color.crBackGround, 255);
    arg.isSubClass = true;
    arg.isDrawIcon = false;
    s_info->pfnControls(IDC_IDRAW_SUBWINDOWEX, (WPARAM)hWnd, (LPARAM)&arg);

    //_subclass(hWnd, 0, false, true, 0);
    _subclass_Static(m_static, false, false, 0);
    auto pData = _subclass_listView(m_list, false, false, 0, LISTVIEW_HEIGHT, true, true);
    //_subclass(m_list, _cusmenu_lv_proc, false, true, 0);

    m_oldProc = pData->fnWndproc;
    pData->fnWndproc = _cusmenu_lv_proc;

}


int CustomMenu_GetMenuList(_str& str, std::vector<LPCWSTR>& arr)
{
    _str cus_menu_ini = s_info->ini->readA("iDraw", "CustomMenu");
    if (cus_menu_ini.empty())
        return 0;

    // 菜单名\0菜单名\0\0
    str = wstr::base64_decode(cus_menu_ini.c_str(), cus_menu_ini.size());
    LPCWSTR pMenuStr = (LPCWSTR)str.c_str();

    arr.reserve(20);
    while (pMenuStr)
    {
        size_t len = wcslen(pMenuStr);
        if (len == 0)
            break;
        arr.push_back(pMenuStr);
        pMenuStr += len + 1;
    }

    return (int)arr.size();
}

bool CustomMenu_RemoveMenuList(LPCWSTR menu_name)
{
    _str str;
    std::vector<LPCWSTR> arr;
    CustomMenu_GetMenuList(str, arr);
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        if (wcscmp(*it, menu_name) == 0)
        {
            arr.erase(it);
            break;
        }
    }

    // 重新编码然后写出配置
    wstr menu_str_buf;
    menu_str_buf.reserve(520);
    for (LPCWSTR text : arr)
    {
        menu_str_buf.append(text).push_back(L'\0');
    }
    menu_str_buf.push_back(L'\0');

    _str base_en_data = wstr::base64_encode(menu_str_buf.c_str(), menu_str_buf.size() * sizeof(wchar_t));

    return s_info->ini->write("iDraw", "CustomMenu", base_en_data.c_str());
}
bool CustomMenu_InsertMenuList(LPCWSTR menu_name)
{
    _str str;
    std::vector<LPCWSTR> arr;
    CustomMenu_GetMenuList(str, arr);
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        if (wcscmp(*it, menu_name) == 0)
        {
            return false;  // 已经存在
        }
    }

    // 重新编码然后写出配置
    wstr menu_str_buf;
    menu_str_buf.reserve(520);
    for (LPCWSTR text : arr)
    {
        menu_str_buf.append(text).push_back(L'\0');
    }
    menu_str_buf.append(menu_name).push_back(L'\0');

    _str base_en_data = wstr::base64_encode(menu_str_buf.c_str(), menu_str_buf.size() * sizeof(wchar_t));

    return s_info->ini->write("iDraw", "CustomMenu", base_en_data.c_str());
}

INT_PTR CALLBACK CustomMenu_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        RECT rc;
        GetWindowRect(m_hParent, &rc);
        _cusmenu_create(hWnd);

        const int width = 500;
        const int height = 550;
        int left = rc.left + (rc.right - rc.left - width) / 2;
        int top = rc.top + (rc.bottom - rc.top - height) / 2;
        SetWindowPos(hWnd, 0, left, top, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
        break;
    }
    case WM_SIZE:
    {
        const int cxClient = LOWORD(lParam);
        const int cyClient = HIWORD(lParam);
        const int offset = 8;
        const int list_width = 200;
        const int top = 30;
        const int height = cyClient - offset * 2 - top;

        m_list.Move(offset, offset + top, list_width, height);

        const int static_top = offset + top + 50;
        const int static_height = height - static_top - offset;
        const int statis_left = offset + list_width + offset;
        const int statis_width = cxClient - statis_left - offset;
        m_static.Move(statis_left, static_top, statis_width, static_height);

        RECT rc;
        GetClientRect(m_list, &rc);
        m_list.SetColumnWidth(0, rc.right - rc.left);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
        HBRUSH hBrush = s_info->pfnCreateBrush(s_info->defData->draw.color.crBackGround);
        FillRect(hdc, &rc, hBrush);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh->code != NM_CLICK)
            return false;
        
        LPNMITEMACTIVATE item = (LPNMITEMACTIVATE)lParam;
        if (item->iItem < 0)
            return false;
        
        _cusmenu_update_arr(true);  // 每次点击表项都更新菜单数据
        if (item->iItem >= (int)m_menu_data.size())
            break;

        RECT rcItem = { 0 };
        ListView_GetItemRect(pnmh->hwndFrom, item->iItem, &rcItem, LVIR_ICON);
        RECT rcCheckBox = rcItem;
        rcCheckBox.left = (LISTVIEW_HEIGHT - 16) / 2;
        rcCheckBox.right = rcCheckBox.left + 16;
        if (!PtInRect(&rcCheckBox, item->ptAction))
            break;
        
        // 点击在选择框上
        EMENU_DATA* menu = m_menu_data[item->iItem];
        menu->isHide = !menu->isHide;

        if (menu->isHide)
        {
            // 隐藏菜单, 把菜单标题写出配置
            CustomMenu_InsertMenuList(menu->text);
        }
        else
        {
            // 显示菜单, 从配置里把标题删除
            CustomMenu_RemoveMenuList(menu->text);
        }
        EMenu_MenuBarChange();
        InvalidateRect(pnmh->hwndFrom, 0, 0);
        break;
    }
    case WM_CLOSE:
    {
        EndDialog(hWnd, 0);
        DestroyWindow(hWnd);
        break;
    }
    default:
        return (INT_PTR)false;
    }
    return (INT_PTR)true;
}

void _cusmenu_update_arr(bool isPaint)
{
    m_menu_data.clear();
    auto pfn_insert = [](std::vector<EMENU_DATA>& menu)
    {
        for (auto& item : menu)
        {
            if (item.text[0]
                && wcscmp(item.text, L"&X.扩展") != 0 // 调整菜单显示就是这个菜单项, 所以这个不能隐藏
                && wcscmp(item.text, L"&H.帮助") != 0 // 广告信息在这里, 也不能隐藏
                )
            {
                m_menu_data.push_back(&item);
            }
        }
    };

    pfn_insert(m_menu->menu);
    if (!isPaint)
        return;

    int count = (int)m_menu_data.size();
    if (m_list.GetItemCount() != count)
    {
        m_list.SetItemCount(count);
    }
}

LRESULT CALLBACK _cusmenu_lv_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        const bool isDx = true;
        LPCANVAS hCanvas = pData->draw.hCanvas;
        s_info->iDraw->BeginPaint(pData, ps, isDx, true);

        _cusmenu_update_arr(false);

        int oldMode = 0;
        _canvas_SetTextRenderingHint(hCanvas, 1, &oldMode);

        LISTVIEW_CUSTOM_DATA draw;
        draw.hbrHot  = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));
        draw.hbrText = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crText, 255));
        draw.hbrSel  = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crSelected, 255));

        const int nTopIndex = ListView_GetTopIndex(hWnd);
        const int nItemCount = ListView_GetItemCount(hWnd);
        const int index = ListView_GetNextItem(hWnd, -1, LVNI_SELECTED);

        RECT rcItem;

        for (int i = nTopIndex; i < nItemCount; i++)
        {
            ListView_GetItemRect(hWnd, i, &rcItem, LVIR_BOUNDS);
            if (!Rect_isOverlap(rcItem, ps.rcPaint))    // 不是重画区域就进行下一次循环
                continue;

            _custommenu_listView_DrawItem(hCanvas, rcItem, pData, i, index, draw);


        }
        _canvas_SetTextRenderingHint(hCanvas, oldMode, 0);
        s_info->iDraw->EndPaint(ps);
        break;
    }

    default:
        break;
    }

    return m_oldProc(hWnd, message, wParam, lParam, pData);
}

void _custommenu_listView_DrawItem(LPCANVAS hCanvas, const RECT& rcItem, LPOBJSTRUCT pData, int i, int index, LISTVIEW_CUSTOM_DATA& draw)
{
    LPEX_BRUSH hbrBack = 0;

    if (i == index)
    {
        hbrBack = draw.hbrSel;
    }
    else if (i == pData->draw.indexHot)
    {
        hbrBack = draw.hbrHot;
    }

    if (hbrBack)
    {
        RECT_F rcItemF(rcItem);
        int oldMode = 0;
        if (s_info->isGpRender)
            _canvas_SetAntialiasMode(hCanvas, 0, &oldMode);
        _canvas_fillRectangle(hCanvas, hbrBack, &rcItemF);
        if (s_info->isGpRender)
            _canvas_SetAntialiasMode(hCanvas, oldMode, 0);
    }


    if (i >= (int)m_menu_data.size())
        return;

    EMENU_DATA* menu = m_menu_data[i];
    if (!menu)
        return;

    int nHeight = rcItem.bottom - rcItem.top;
    RECT_F rcIcon;
    rcIcon.left = (float)((nHeight - 16)) / 2;
    rcIcon.top = (float)(rcItem.top + (nHeight - 16) / 2);
    rcIcon.right = rcIcon.left + 16;
    rcIcon.bottom = rcIcon.top + 16;

    RECT_F rcf(rcItem);
    rcf.left = rcIcon.right + rcIcon.left;

    LPEX_IMAGE _check_uncheck();
    LPEX_IMAGE _check_check();
    LPEX_IMAGE hImg = menu->isHide ? _check_uncheck() : _check_check();
    _canvas_drawimagerect(hCanvas, hImg, rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 255);
    

    int fmt = pData->draw.fmt;
    LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    _canvas_drawtext(hCanvas, hFont, draw.hbrText, menu->text, -1, fmt, &rcf);

}
