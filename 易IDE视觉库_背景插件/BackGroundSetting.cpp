#include "pch.h"
#include <vector>
#define IDDLG_SHOWMODE          1000    // 显示方式
#define IDDLG_ALPHA             1001    // 图片透明度
#define IDDLG_SHOWINCODE        1002    // 只显示在代码区

#define IDDLG_SLIDE_GROUP       2000    // 幻片灯分组框
#define IDDLG_SLIDE_PATH        2001    // 幻片灯路径
#define IDDLG_SLIDE_PATH_SEL    2002    // 幻片灯路径 编辑框后面的选择路径按钮
#define IDDLG_SLIDE_LOOP        2003    // 循环播放
#define IDDLG_SLIDE_RAND        2004    // 随机播放
#define IDDLG_SLIDE_TIME        2005    // 图片更换时间

#define IDDLG_SINGLE_FILE       3000    // 显示指定图片的图片路径
#define IDDLG_SINGLE_FILE_SEL   3001    // 显示指定图片的图片路径 编辑框后面的选择路径按钮



#define INI_KEY_SHOWMODE        L"showMode"
#define INI_KEY_ALPHA           L"alpha"
#define INI_KEY_SHOWINCODE      L"showInCode"
#define INI_KEY_SLIDE_PATH      L"slidePath"
#define INI_KEY_SLIDE_LOOP      L"slideLoop"
#define INI_KEY_SLIDE_RAND      L"slideRand"
#define INI_KEY_SLIDE_TIME      L"slideTime"
#define INI_KEY_SINGLE_FILE     L"SingleFile"

static HWND m_hWnd;
static pfn_SubClassWndProc m_oldProc;
static HBRUSH m_hbrBack;
static COLORREF m_crBack;

struct COMBOBOX_DATA_STRUCT
{
    LPCWSTR name;
    LPARAM param;
};
typedef std::vector<COMBOBOX_DATA_STRUCT> COMBOBOX_ARR;
enum IMAGE_SHOWMODE
{
    IMAGE_SHOWMODE_SINGLE,              // 显示指定图片
    IMAGE_SHOWMODE_SLIDESHOW,           // 幻片灯显示
    IMAGE_SHOWMODE_SINGLEEACH,          // 根据不同类型显示不同图片
};

void _wnd_create(HWND hDlg);
LRESULT CALLBACK DlgProc_Setting(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
bool DlgProc_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& ret);
LRESULT CALLBACK SettingProc_Group(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


HWND Setting_Open()
{
    if (IsWindow(m_hWnd))
        return m_hWnd;

    HWND hEWnd = g_eWnd->GetEWindow();
    LPOBJSTRUCT pEData = iDraw_GetWindowData(hEWnd);
    IDRAW_STRUCT_WINDOW_ARGUMENT arg = { 0 };
    iDraw_DrawGetDefArg(0, &arg);
    m_crBack            = iDraw_GetLong(pEData, IGL_DRAW_CRBACKGROUND);
    m_hbrBack           = (HBRUSH)iDraw_GetLong(pEData, IGL_DRAW_HBRBACKGROUND);
    arg.crBack          = m_crBack;
    arg.crBack          = RGB2ARGB(arg.crBack, 255);
    arg.isCasualMove    = TRUE;
    arg.isDrawIcon      = FALSE;


    const DWORD style = WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_POPUP | WS_CAPTION;
    LPOBJSTRUCT pData = iDraw_CreateWindow(0, L"设置", style, 0, 0, 0, 0, hEWnd, &arg);
    if (!pData) return 0;
    m_hbrBack = (HBRUSH)iDraw_GetLong(pData, IGL_DRAW_HBRNORMAL);
    m_hWnd = (HWND)iDraw_GetLong(pData, IGL_HWND);
    m_oldProc = (pfn_SubClassWndProc)iDraw_SetLong(pData, IGL_WNDPROC, (LONG_PTR)DlgProc_Setting);
    DlgProc_Setting(m_hWnd, WM_CREATE, 0, 0, pData);

    return m_hWnd;
}
inline void _set_color(LPOBJSTRUCT pData)
{
    iDraw_SetLong(pData, IGL_DRAW_CRBACKGROUND, (LONG_PTR)m_crBack);
    iDraw_SetLong(pData, IGL_DRAW_HBRBACKGROUND, (LONG_PTR)m_hbrBack);
}
inline HWND CreateControl(DWORD styleEx, LPCWSTR szClass, LPCWSTR title, DWORD style, int x, int y, int cx, int cy, HWND hParent, int id)
{
    HWND hWnd = CreateWindowExW(styleEx, szClass, title, style, x, y, cx, cy, hParent, (HMENU)(LONG_PTR)id, g_hInst, 0);
    SendMessageW(hWnd, WM_SETFONT, (WPARAM)iDraw_GetInfo(g_info, IGI_CONFIG_DEFGDIFONT, 0), 0);
    return hWnd;
}
inline HWND CreateStatic(HWND hParent, int x, int y, int cx, int cy, LPCWSTR lpszTitle)
{
    const int style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE;
    const int styleEx = 0;
    HWND hWnd = CreateControl(styleEx, L"static", lpszTitle, style, x, y, cx, cy, hParent, 0);
    _set_color(iDraw_Subclass_Static(hWnd, 0, false, 0));
    return hWnd;
}
inline HWND CreateComboBox(HWND hParent, COMBOBOX_ARR* items, int x, int y, int cx, int cy, int id)
{
    const int style = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CBS_AUTOHSCROLL | CBS_HASSTRINGS | CBS_DROPDOWNLIST;
    const int styleEx = 0;
    HWND hWnd = CreateControl(styleEx, L"ComboBox", 0, style, x, y, cx, cy, hParent, id);
    _set_color(iDraw_Subclass_ComboBox(hWnd, 0, false, 0, 24));

    if (!items)
        return hWnd;
    for (auto it = items->begin(); it != items->end(); ++it)
    {
        COMBOBOX_DATA_STRUCT& data = *it;
        int index = (int)(LONG_PTR)SendMessageW(hWnd, CB_ADDSTRING, 0, (LPARAM)data.name);
        if (index != CB_ERR)
        {
            SendMessageW(hWnd, CB_SETITEMDATA, index, data.param);
        }
    }
    return hWnd;
}
inline HWND CreateEdit(HWND hParent, int x, int y, int cx, int cy, int id)
{
    const int style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VISIBLE | WS_CHILD | ES_LEFT;
    const int styleEx = WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY;
    HWND hWnd = CreateControl(styleEx, L"edit", 0, style, x, y, cx, cy, hParent, id);
    _set_color(iDraw_Subclass_Edit(hWnd, 0, false, 0, false));
    return hWnd;
}
inline HWND CreateGroup(HWND hParent, int id, int x, int y, int cx, int cy, LPCWSTR lpszTitle)
{
    const int style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_GROUPBOX | BS_LEFT;
    const int styleEx = 0;
    HWND hWnd = CreateControl(styleEx, L"button", lpszTitle, style, x, y, cx, cy, hParent, id);

    _set_color(iDraw_Subclass_Button(hWnd, 0, false, 0));
    return hWnd;
}
inline HWND CreateCheckBox(HWND hParent, int id, bool isCheck, int x, int y, int cx, int cy, LPCWSTR lpszTitle)
{
    const int style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD;
    const int styleEx = 0;
    HWND hWnd = CreateControl(styleEx, L"button", lpszTitle, style, x, y, cx, cy, hParent, id);
    if (isCheck)
        SendMessageW(hWnd, BM_SETCHECK, BST_CHECKED, 0);
    _set_color(iDraw_Subclass_Button(hWnd, 0, false, 0));
    return hWnd;
}

LRESULT CALLBACK DlgProc_Setting(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_CREATE:
    {
        m_hWnd = hWnd;
        _wnd_create(hWnd);
        return 0;
    }
    case WM_COMMAND:
    {
        LRESULT ret = 0;
        if (DlgProc_OnCommand(hWnd, message, wParam, lParam, ret))
            return ret;
        return m_oldProc(hWnd, message, wParam, lParam, pData);
    }
    //case WM_CLOSE:
    //{
    //    DestroyWindow(hDlg);
    //    break;
    //}
    //case WM_PAINT:
    //{
    //    PAINTSTRUCT ps = { 0 };
    //    BeginPaint(hWnd, &ps);
    //    RECT rc = { 0 };
    //    GetClientRect(hWnd, &rc);
    //    FillRect(ps.hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    //    EndPaint(hWnd, &ps);

    //    //BEGIN_END_PAINT_STRUCT ps;
    //    //iDraw_BeginPaint(pData, &ps, false, true);

    //    //iDraw_EndPaint(&ps);

    //    break;
    //}
    default:
        return m_oldProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}
LRESULT CALLBACK WndProc_Setting(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_PAINT:
    {

        break;
    }
    default:
        return iDraw_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}
void _wnd_create(HWND hDlg)
{
    RECT rc;
    GetWindowRect(g_eWnd->GetEWindow(), &rc);
    if (rc.right == 0 || rc.bottom == 0)
    {
        rc.left = 0;
        rc.top = 0;
        rc.right = GetSystemMetrics(SM_CXSCREEN);
        //rc.bottom = GetSystemMetrics(SM_CYSCREEN);
        rc.bottom = GetSystemMetrics(SM_CYMAXIMIZED);
    }

    const int SettingWidth  = 365;
    const int SettingHeight = 340;
    int width  = rc.right  - rc.left;
    int height = rc.bottom - rc.top;
    const int SendLeft = rc.left + (width  - SettingWidth ) / 2;
    const int SendTop  = rc.top  + (height - SettingHeight) / 2;
    MoveWindow(hDlg, SendLeft, SendTop, SettingWidth, SettingHeight, true);

    const int staticLeft        = 10;       // 提示标签的左边
    const int staticWidth       = 75;       // 提示标签的宽度
    const int editLeft          = 90;       // 编辑框左边位置
    const int editWidth         = 260;      // 编辑框的宽度, editLeft+宽度 就是右边
    const int heightControl     = 24;       // 组件的高度, 每一行占这么高的位置, 隔行需要加上间隔
    const int widthSeparator    = 5;        // 每行间隔

    int top = widthSeparator + 30;
    int style = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | CBS_HASSTRINGS;
    int styleEx = 0;

    COMBOBOX_ARR items
    {
        { L"显示指定图片",    IMAGE_SHOWMODE_SINGLE },
        { L"幻片灯显示",     IMAGE_SHOWMODE_SLIDESHOW },
    };
    CreateStatic(hDlg, staticLeft, top, staticWidth, heightControl, L"展示方式");
    HWND hChild = CreateComboBox(hDlg, &items, editLeft, top, editWidth, heightControl + 200, IDDLG_SHOWMODE);
    int index = ini_read_int(INI_KEY_SHOWMODE, 0);
    SendMessageW(hChild, CB_SETCURSEL, index, 0);
    top += heightControl + widthSeparator;

    CreateStatic(hDlg, staticLeft, top, staticWidth, heightControl, L"透明度0-255");
    hChild = CreateEdit(hDlg, editLeft, top, editWidth, heightControl, IDDLG_ALPHA);
    std::wstring alphaText = ini_read(INI_KEY_ALPHA, L"120");
    SetWindowTextW(hChild, alphaText.c_str());
    top += heightControl + widthSeparator;

    CreateStatic(hDlg, staticLeft, top, staticWidth, heightControl, L"显示位置");
    CreateCheckBox(hDlg, IDDLG_SHOWINCODE, ini_read_int(INI_KEY_SHOWINCODE, 1), editLeft, top, editWidth, heightControl, L"只显示在代码区");
    top += heightControl + widthSeparator;

    style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_GROUPBOX | BS_LEFT;
    styleEx = 0;
    int groupHeight = 4 * (heightControl + widthSeparator) + 20 + widthSeparator;
    const int groupWidth = editWidth + editLeft - staticLeft;
    const int btnSelWidth = 30;
    const int btnSelLeft = groupWidth - widthSeparator - btnSelWidth;
    HWND hGroup = CreateGroup(hDlg, IDDLG_SLIDE_GROUP, 10, top, groupWidth, groupHeight, L"幻片灯设置");
    {
        const int childLeft = 10;
        const int childEditLeft = childLeft + staticWidth + 10;
        const int childWidth = groupWidth - childEditLeft - childLeft;
        int childTop = 20;
        std::wstring slidePath = ini_read(INI_KEY_SLIDE_PATH);
        CreateStatic(hGroup, childLeft, childTop, staticWidth, heightControl, L"幻片灯路径");
        hChild = CreateEdit(hGroup, childEditLeft, childTop, childWidth - btnSelWidth, heightControl, IDDLG_SLIDE_PATH);
        SetWindowTextW(hChild, slidePath.c_str());
        //SendMessageW(hChild, EC_RIGHTMARGIN, EC_RIGHTMARGIN, MAKELONG(0, btnSelWidth));
        style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        styleEx = 0;
        hChild = CreateControl(styleEx, L"button", L"...", style, childEditLeft + childWidth - btnSelWidth, childTop, btnSelWidth, heightControl, hGroup, IDDLG_SLIDE_PATH_SEL);
        _set_color(iDraw_Subclass_Button(hChild, 0, false, 0));
        childTop += heightControl + widthSeparator;

        CreateStatic(hGroup, childLeft, childTop, staticWidth, heightControl, L"循环播放");
        CreateCheckBox(hGroup, IDDLG_SLIDE_LOOP, ini_read_int(INI_KEY_SLIDE_LOOP, 1), childEditLeft, childTop, childWidth, heightControl, L"启用循环播放");
        childTop += heightControl + widthSeparator;
        
        CreateStatic(hGroup, childLeft, childTop, staticWidth, heightControl, L"随机播放");
        CreateCheckBox(hGroup, IDDLG_SLIDE_RAND, ini_read_int(INI_KEY_SLIDE_RAND, 1), childEditLeft, childTop, childWidth, heightControl, L"随机播放幻片灯");
        childTop += heightControl + widthSeparator;

        CreateStatic(hGroup, childLeft, childTop, staticWidth, heightControl, L"更换间隔(秒)");
        hChild = CreateEdit(hGroup, childEditLeft, childTop, childWidth, heightControl, IDDLG_SLIDE_TIME);
        std::wstring timeText = ini_read(INI_KEY_SLIDE_TIME, L"60");
        SetWindowTextW(hChild, timeText.c_str());
        childTop += heightControl + widthSeparator;

    }
    top += groupHeight + widthSeparator;

    style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_GROUPBOX | BS_LEFT;
    styleEx = 0;
    groupHeight = 1 * (heightControl + widthSeparator) + 20 + widthSeparator;
    hGroup = CreateGroup(hDlg, IDDLG_SLIDE_GROUP, 10, top, groupWidth, groupHeight, L"显示指定图片设置");
    {
        const int childLeft = 10;
        const int childEditLeft = childLeft + staticWidth + 10;
        const int childWidth = groupWidth - childEditLeft - childLeft;
        int childTop = 20;
        std::wstring SingleFile = ini_read(INI_KEY_SINGLE_FILE);
        CreateStatic(hGroup, childLeft, childTop, staticWidth, heightControl, L"图片路径");
        hChild = CreateEdit(hGroup, childEditLeft, childTop, childWidth - btnSelWidth, heightControl, IDDLG_SINGLE_FILE);
        SetWindowTextW(hChild, SingleFile.c_str());
        //SendMessageW(hChild, EC_RIGHTMARGIN, EC_RIGHTMARGIN, MAKELONG(0, btnSelWidth));
        style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
        styleEx = 0;
        hChild = CreateControl(0, L"button", L"...", style, childEditLeft + childWidth - btnSelWidth, childTop, btnSelWidth, heightControl, hGroup, IDDLG_SLIDE_PATH_SEL);
        _set_color(iDraw_Subclass_Button(hChild, 0, false, 0));
        childTop += heightControl + widthSeparator;
    }
    top += groupHeight + widthSeparator;


}
bool DlgProc_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& ret)
{
    const int id    = LOWORD(wParam);
    const int code  = HIWORD(wParam);
    HWND hChild     = (HWND)lParam;

    return false;
}

LRESULT CALLBACK SettingProc_Group(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WNDPROC oldProc = (WNDPROC)GetPropW(hWnd, L"proc");
    switch (message)
    {
    case WM_COMMAND:
    {
        LRESULT ret = 0;
        if (DlgProc_OnCommand(hWnd, message, wParam, lParam, ret))
            return ret;
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    default:
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
