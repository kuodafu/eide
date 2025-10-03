#include "pch.h"
#include <unordered_map>
#include <commctrl.h>

#define IDCBK_ISENABLED             1000    // 是否启用行号插件
#define IDCBK_ISDRAWLINE            1001    // 是否绘画行号线
#define IDCBK_ISDRAWLINENUMBER      1002    // 是否绘画行号
#define IDCBK_ISBOLDLINE            1003    // 是否加粗行号线

#define IDEDIT_CRBACK               2000    // 背景颜色编辑框
#define IDEDIT_CRTEXT               2001    // 普通文本颜色编辑框
#define IDEDIT_CRTEXT_HOT           2002    // 热点文本颜色编辑框
#define IDEDIT_CRTEXT_EXPAND        2003    // 展开文本颜色编辑框

#define IDEDIT_LINEWIDTH            2004    // 行号宽度编辑框



static HWND s_hWnd;     // 设置窗口句柄
static HFONT s_hFont;   // 设置界面使用的字体
static HWND s_hWndTips; // 提示窗口
struct EDIT_STATIC_BLOCK
{
    LPCWSTR staticText; // 标签文本
    RECT rcText;        // 文本显示的位置
    RECT rcBlock;       // 颜色块显示的位置
    COLORREF crBlock;   // 颜色块颜色
    HBRUSH hbrBlock;    // 颜色块的画刷
    bool isModify;      // 编辑框内容是否被改变
    HWND hWnd;          // 编辑框的窗口句柄
};

static std::unordered_map<int, EDIT_STATIC_BLOCK> m_map_edit;   // 编辑框数据

// 窗口创建完毕, 需要在这里创建子组件
void _wnd_create(HWND hWnd);
void CheckBox_Check(HWND hWnd, bool isCheck);
bool CheckBox_IsCheck(HWND hWnd);
// 保存编辑框颜色到配置
void SaveColor(EDIT_STATIC_BLOCK& data);
LRESULT SettingCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);



inline LRESULT CALLBACK __WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        s_hWnd = hWnd;
        _wnd_create(hWnd);
        break;
    }
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 加载设置窗口
HWND LoadSettingWindow()
{
    if (!IsWindow(s_hWnd))
    {
        WNDCLASSEXW wcex;
        wcex.cbSize         = sizeof(WNDCLASSEX);
        wcex.style          = CS_DBLCLKS;
        wcex.lpfnWndProc    = __WndProc;
        wcex.cbClsExtra     = 0;
        wcex.cbWndExtra     = 0;
        wcex.hInstance      = g_hInst;
        wcex.hIcon          = 0;
        wcex.hCursor        = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground  = 0;
        wcex.lpszMenuName   = 0;
        wcex.lpszClassName  = L"LineCodeSetting";
        wcex.hIconSm        = 0;
        RECT rc;
        GetWindowRect(g_hEWnd, &rc);

        const int width = 279;
        const int height = 220;

        rc.left += (rc.right - rc.left - width) / 2;
        rc.top += (rc.bottom - rc.top - height) / 2;
        
        ATOM atom = RegisterClassExW(&wcex);
        HWND hWnd = CreateWindowExW(0, wcex.lpszClassName, L"行号插件设置", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            rc.left, rc.top, width, height, g_hEWnd, nullptr, g_hInst, nullptr);
    }
    return s_hWnd;
}

inline HWND _create_child(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent, LONG_PTR id, LPVOID param = 0)
{
    HWND hWnd = CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle | WS_CHILD, x, y, cx, cy, hWndParent, (HMENU)id, g_hInst, nullptr);
    if (!s_hFont)
    {
        LOGFONTW lf = { 0 };
        SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);
        lf.lfHeight = -12;
        lf.lfCharSet = GB2312_CHARSET;
        memcpy(lf.lfFaceName, L"微软雅黑", 10);
        s_hFont = CreateFontIndirectW(&lf);
    }
    SendMessageW(hWnd, WM_SETFONT, (WPARAM)s_hFont, 0);
    return hWnd;
}
HWND _create_checkbox(LPCWSTR lpWindowName, int x, int y, int cx, int cy, HWND hWndParent, LONG_PTR id, bool isCheck)
{
    HWND hChild = _create_child(0, L"button", lpWindowName, WS_VISIBLE | BS_AUTOCHECKBOX, x, y, cx, cy, hWndParent, id);
    iDraw_Subclass_Button(hChild, true, false, 0);
    CheckBox_Check(hChild, isCheck);
    return hChild;
}
inline LRESULT CALLBACK Edit_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_CHAR:
    {
        if(wParam != VK_RETURN) break;

        const int id = GetDlgCtrlID(hWnd);
        bool isModyfy = m_map_edit[id].isModify;


        switch (id)
        {
        case IDEDIT_CRBACK:             // 背景颜色编辑框
        case IDEDIT_CRTEXT:             // 普通文本颜色编辑框
        case IDEDIT_CRTEXT_HOT:         // 热点文本颜色编辑框
        case IDEDIT_CRTEXT_EXPAND:      // 展开文本颜色编辑框
        {
            KillTimer(hWnd, id);
            SaveColor(m_map_edit[id]);
            if (!g_ini.isDrawLine()) break;

            IEIDE_CodeManage* ide = (IEIDE_CodeManage*)iDraw_IDCall(IDC_EIDE_GETCODEMANAGE, 0, 0);
            if (ide)
            {
                HWND hMDIClient3 = GetParent(ide->GetWindow());
                RECT rc;
                GetClientRect(hMDIClient3, &rc);
                rc.right = g_ini.lineWidth();
                InvalidateRect(hMDIClient3, &rc, 0);
                UpdateWindow(hMDIClient3);
            }
            iDraw_SetLong(pData, IGL_INTERCEPT, 1);
            InvalidateRect(s_hWnd, 0, 0);
            return 0;
        }
        case IDEDIT_LINEWIDTH:          // 行号宽度编辑框
        {
            const int width = (int)GetDlgItemInt(s_hWnd, id, 0, false);
            if (width == g_ini.lineWidth())
                break;
            g_ini.lineWidth(width);
            const int newWidth = g_ini.lineWidth();
            if (newWidth != width)
                SetDlgItemInt(s_hWnd, id, newWidth, false);
            RECT rc;
            HWND hMDIClient = g_eWnd->GetMDIClient();
            GetClientRect(hMDIClient, &rc);
            SendMessageA(hMDIClient, WM_SIZE, 0, MAKELONG(rc.right - rc.left, rc.bottom - rc.top)); // 手动触发尺寸改变事件
            iDraw_SetLong(pData, IGL_INTERCEPT, 1);
            return 0;
            break;
        }
        default:
            break;
        }
        return 0;
    }
    default:
        break;
    }
    return 0;
}

bool TipsInsert(HWND hWnd, LPCWSTR text)
{
    TTTOOLINFOW ti = { 0 };
    ti.cbSize = sizeof(ti);
    ti.lpszText = (LPWSTR)text;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.hwnd = hWnd;
    ti.uId = (UINT_PTR)hWnd;
    bool ret = SendMessageW(s_hWndTips, TTM_ADDTOOLW, 0, (LPARAM)&ti) != 0;
    return ret;
}

// 左边标签, 中间编辑框, 右边一个小方块预览颜色
inline HWND _create_color(LPCWSTR lpszStaticName, COLORREF color, int y, HWND hWndParent, LONG_PTR id)
{
    const DWORD style = WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | ES_AUTOHSCROLL;
    const int width = 80;
    const int height = 20;
    wchar_t szColor[50];
    _itow_s(color, szColor, 50, 10);

    HWND hWnd = _create_child(WS_EX_CLIENTEDGE, L"edit", szColor, style, 104, y, width, height, hWndParent, id, 0);
    if (id != IDEDIT_LINEWIDTH)
        TipsInsert(hWnd, L"RGB颜色, 直接输入数值就是RGN颜色, 支持0x开头的十六进制, 支持 RGB(r,g,b), 支持ARGB(a,r,g,b)");

    EDIT_STATIC_BLOCK& data = m_map_edit[id];
    data.rcText = { 16, y, 16 + width, y + 20 };
    data.rcBlock = { 192, y, 192 + height, y + height };
    data.crBlock = color;
    data.hbrBlock = CreateSolidBrush(color);
    data.staticText = lpszStaticName;
    data.hWnd = hWnd;

    iDraw_Subclass_Edit(hWnd, false, false, 0, false);
    iDraw_Event_RegisterMessage(DLL_GUID, hWnd, WM_CHAR, Edit_WndProc, 0);
    return hWnd;
}

LRESULT CALLBACK Setting_WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        HDC hdc = iDraw_BeginPaint(pData, &ps, false, true);
        for (auto it = m_map_edit.begin(); it != m_map_edit.end(); ++it)
        {
            EDIT_STATIC_BLOCK& data = it->second;
            DrawTextW(hdc, data.staticText, -1, &data.rcText, DT_VCENTER | DT_SINGLELINE | DT_RIGHT);
            FillRect(hdc, &data.rcBlock, data.hbrBlock);
        }
        iDraw_EndPaint(&ps);
        break;
    }
    case WM_COMMAND:
        return SettingCommand(hWnd, message, wParam, lParam, pData);
    case WM_DESTROY:
    {
        SaveColor(m_map_edit[IDEDIT_CRBACK]);
        SaveColor(m_map_edit[IDEDIT_CRTEXT]);
        SaveColor(m_map_edit[IDEDIT_CRTEXT_HOT]);
        SaveColor(m_map_edit[IDEDIT_CRTEXT_EXPAND]);
        break;
    }
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
        return iDraw_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}



void _wnd_create(HWND hWnd)
{
    HWND hChild = 0;
    iDraw_SubclassWindow(hWnd, Setting_WndProc1, false, true);

    if (!IsWindow(s_hWndTips))
        s_hWndTips = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, TOOLTIPS_CLASSW, 0, TTS_ALWAYSTIP, 0, 0, 0, 0, 0, 0, g_hInst, 0);

    //hChild = _create_checkbox(L"是否启用行号插件", 8, 8, 112, 24, hWnd, IDCBK_ISENABLED, g_ini.isEnabled());
    hChild = _create_checkbox(L"行号",       8, 8, 80, 24, hWnd, IDCBK_ISDRAWLINENUMBER, g_ini.isDrawLineNumber());
    hChild = _create_checkbox(L"行号线",     88, 8, 80, 24, hWnd, IDCBK_ISDRAWLINE, g_ini.isDrawLine());
    hChild = _create_checkbox(L"行号线加粗", 176, 8, 80, 24, hWnd, IDCBK_ISBOLDLINE, g_ini.isBoldLine());
    EnableWindow(hChild, g_ini.isDrawLine());

    hChild = _create_color(L"背景颜色",    g_ini.crBack(),   32, hWnd, IDEDIT_CRBACK);
    hChild = _create_color(L"普通文本颜色", g_ini.crNormal(),  56, hWnd, IDEDIT_CRTEXT);
    hChild = _create_color(L"热点文本颜色", g_ini.crHot(), 80, hWnd, IDEDIT_CRTEXT_HOT);
    hChild = _create_color(L"展开文本颜色", g_ini.crExpand(), 104, hWnd, IDEDIT_CRTEXT_EXPAND);

    hChild = _create_color(L"行号宽度",     g_ini.lineWidth(),  128, hWnd, IDEDIT_LINEWIDTH);
    TipsInsert(hChild, L"行号宽度, 最小20, 最大300, 越界被设置为50");

}

void CheckBox_Check(HWND hWnd, bool isCheck)
{
    SendMessageW(hWnd, BM_SETCHECK, (isCheck ? BST_CHECKED : BST_UNCHECKED), 0);
}

bool CheckBox_IsCheck(HWND hWnd)
{
    DWORD state = (DWORD)SendMessageW(hWnd, BM_GETCHECK, 0, 0);
    return (state & BST_CHECKED) == BST_CHECKED;
}
// 保存编辑框颜色到配置
void SaveColor(EDIT_STATIC_BLOCK& data)
{
    int len = GetWindowTextLengthW(data.hWnd) + 1;
    wchar_t* text = new wchar_t[len] {0};
    GetWindowTextW(data.hWnd, text, len);
    COLORREF color = iDraw_IDCall(IDC_IDRAW_STR2RGB, 0, (LPARAM)text);
    delete[] text;
    data.isModify = false;
    if (color == data.crBlock)
        return;
    
    data.crBlock = color;
    DeleteObject(data.hbrBlock);
    data.hbrBlock = CreateSolidBrush(data.crBlock);

    const int id = GetDlgCtrlID(data.hWnd);
    switch (id)
    {
    case IDEDIT_CRBACK:             // 背景颜色编辑框
        g_ini.crBack(color); break;
    case IDEDIT_CRTEXT:             // 普通文本颜色编辑框
        g_ini.crNormal(color); break;
    case IDEDIT_CRTEXT_HOT:         // 热点文本颜色编辑框
        g_ini.crHot(color); break;
    case IDEDIT_CRTEXT_EXPAND:      // 展开文本颜色编辑框
        g_ini.crExpand(color); break;
    default:
        break;
    }

}
// 编辑框内容改变事件, 触发一次就销毁时钟, 然后保存编辑框文本
void CALLBACK TimerProc_Edit(HWND hWnd, UINT message, UINT_PTR id, DWORD dwTimer)
{
    KillTimer(hWnd, id);
    SaveColor(m_map_edit[id]);
}

LRESULT SettingCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    const int id = LOWORD(wParam);
    const int code = HIWORD(wParam);
    HWND hChild = (HWND)lParam;
    switch (id)
    {
    case IDCBK_ISDRAWLINENUMBER:    // 是否绘画行号
    {
        g_ini.isDrawLineNumber(CheckBox_IsCheck(hChild));
        RECT rc;
        HWND hMDIClient = g_eWnd->GetMDIClient();
        GetClientRect(hMDIClient, &rc);
        SendMessageA(hMDIClient, WM_SIZE, 0, MAKELONG(rc.right - rc.left, rc.bottom - rc.top)); // 手动触发尺寸改变事件
        break;
    }
    case IDCBK_ISDRAWLINE:          // 是否绘画行号线
    {
        bool isCheck = CheckBox_IsCheck(hChild);
        g_ini.isDrawLine(isCheck);
        EnableWindow(GetDlgItem(hWnd, IDCBK_ISBOLDLINE), isCheck);
        iDraw_UpdateShadow(true);
        break;
    }
    case IDCBK_ISBOLDLINE:          // 是否加粗行号线
    {
        g_ini.isBoldLine(CheckBox_IsCheck(hChild));
        iDraw_UpdateShadow(true);
        break;
    }
    case IDEDIT_CRBACK:             // 背景颜色编辑框
    case IDEDIT_CRTEXT:             // 普通文本颜色编辑框
    case IDEDIT_CRTEXT_HOT:         // 热点文本颜色编辑框
    case IDEDIT_CRTEXT_EXPAND:      // 展开文本颜色编辑框
    //case IDEDIT_LINEWIDTH:          // 行号宽度编辑框
    {
        if (code == EN_CHANGE)
        {
            m_map_edit[id].isModify = true;
            SetTimer(hChild, IDEDIT_CRBACK, 1000, TimerProc_Edit);
        }
        break;
    }
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}
