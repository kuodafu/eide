#include <control/WndControl6_0.h>
#include "ExtView.h"

#include <shellapi.h>
#pragma comment(lib, "Shell32.lib")
#include <control/CCustomList.h>
#include <ThemeResourceExt.h>
#include <vector>
#include <d2d/d2d_old.h>
#include <gdiplus/drawImage.h>
#include <control/CButton.h>


static CButton m_radio[5];
static int m_dpiIndex;
static bool m_isShowRadio;

#define MAX_LOADSTRING 100
#define ID_RADIO_100            2400    // 100%缩放
#define ID_RADIO_125            2401    // 125%缩放
#define ID_RADIO_150            2402    // 150%缩放
#define ID_RADIO_175            2403    // 175%缩放
#define ID_RADIO_200            2404    // 200%缩放

#define DRAW_COLOR_TEXT RGB(222,222,222)


// 全局变量:
HINSTANCE m_hInst;                                // 当前实例


struct EXT_LIST_DATA
{
    LPCWSTR id;
    LPCWSTR text;
    LPCWSTR remarks;
    DWORD crText[4];
    DWORD crBack[4];
    DWORD by[5];
    RECT rc[20];
};
static CCustomList m_list;
static std::vector<EXT_LIST_DATA> m_data;
static Gdiplus::Bitmap* m_hBitmap;
static int m_cxIcon, m_cyIcon;
static HWND m_hWnd;
static WNDPROC m_oldProc;
static bool m_isLoopMsg;

// 此代码模块中包含的函数的前向声明:
LRESULT CALLBACK    ExtView_WndProc(HWND, UINT, WPARAM, LPARAM);
bool CALLBACK ExtView_CustomListBox_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus);
void ExtView_LoadExt(LPCWSTR fileName);



void ExtView_Create(HWND hWnd)
{
    DWORD styleEx = WS_EX_CONTROLPARENT;
    DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER;
    const COLORREF crBack = RGB(45, 45, 48);
    RECT rc;
    GetWindowRect(hWnd, &rc);
    m_list.create(styleEx, 0, style, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hWnd, 0, 0);
    m_list.SetBkColor(crBack);


    m_list.SetNotifyCallback([](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
                             {
                                 LPNMHDR hdr = (LPNMHDR)lParam;
                                 switch (hdr->code)
                                 {
                                 case NM_CUSTOMDRAW:
                                 {
                                     auto lplvcd = (LPNMLVCUSTOMDRAW)lParam;
                                     switch (lplvcd->nmcd.dwDrawStage)
                                     {
                                     case CDDS_ITEMPREPAINT:
                                     {
                                         const bool isFocus = (lplvcd->nmcd.uItemState == CDIS_FOCUS);

                                         STATE state = (lplvcd->nmcd.uItemState == (LVIS_SELECTED | CDIS_FOCUS))
                                             ? STATE::SELECTED : (isFocus ? STATE::FOCUS
                                                                  : (lplvcd->nmcd.uItemState == CDIS_HOT ? STATE::HOT : STATE::NORMAL));
                                         ExtView_CustomListBox_DrawItem(hWnd,
                                                                        lplvcd->nmcd.hdc,
                                                                        &lplvcd->nmcd.rc,
                                                                        lplvcd->nmcd.dwItemSpec,
                                                                        state, isFocus);
                                         return CDRF_NOTIFYITEMDRAW;
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
                             });

    //TODO 太久没动了, 代码迭代后这个源码没更新, 忘了怎么挂接绘画回调了, 挂接不了的话就不能显示列表内容了
    //m_list.SetNotifyCallback(ExtView_CustomListBox_DrawItem);



    m_radio[0].create(0, L"缩放100%", WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON,   8, 2, 100, 18, hWnd, ID_RADIO_100);
    m_radio[1].create(0, L"缩放125%", WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 116, 2, 100, 18, hWnd, ID_RADIO_125);
    m_radio[2].create(0, L"缩放150%", WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 224, 2, 100, 18, hWnd, ID_RADIO_150);
    m_radio[3].create(0, L"缩放175%", WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 332, 2, 100, 18, hWnd, ID_RADIO_175);
    m_radio[4].create(0, L"缩放200%", WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 440, 2, 100, 18, hWnd, ID_RADIO_200);
    
}
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK ExtView_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATE:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    case WM_SIZE:
    {
        const int cxClient = LOWORD(lParam);
        const int cyClient = HIWORD(lParam);
        const int top = m_isShowRadio ? 20 : 0;
        for (int i = 0; i < sizeof(m_radio) / sizeof(m_radio[0]); i++)
            m_radio[i].SetVisible(m_isShowRadio);
        m_list.Move(0, top, cxClient, cyClient - top);
        break;
    }
    case WM_CTLCOLORSTATIC:
        return (LRESULT)GetStockBrush(WHITE_BRUSH);
    case WM_ERASEBKGND:
        return 1;
    case WM_DROPFILES:
    {
        wchar_t file[1000];
        HDROP hDrop = (HDROP)wParam;
        UINT count = DragQueryFileW(hDrop, -1, file, 1000);
        DragQueryFileW(hDrop, 0, file, 1000);
        ExtView_LoadExt(file);
        DragFinish(hDrop);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, GetStockBrush(WHITE_BRUSH));
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_COMMAND:
    {
        const int id = LOWORD(wParam);
        switch (id)
        {
        case ID_RADIO_100:
        case ID_RADIO_125:
        case ID_RADIO_150:
        case ID_RADIO_175:
        case ID_RADIO_200:
            m_dpiIndex = id - ID_RADIO_100;
            m_list.InvalidateRect();
            break;
        default:
            return CallWindowProcW(m_oldProc, hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_DESTROY:
        if (m_hBitmap)delete m_hBitmap;
        m_hBitmap = 0;
        if (m_isLoopMsg) PostQuitMessage(0);
        break;
    case WM_CLOSE:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    default:
        return CallWindowProcW(m_oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
inline static LPCWSTR _read_string(LPBYTE& ptr)
{
    LPCWSTR str = (LPCWSTR)ptr;
    size_t len = wcslen(str) + 1;
    len *= sizeof(wchar_t);
    ptr += len;
    return str;
}
inline static int _read_int(LPBYTE& ptr)
{
    int ret = *((int*)ptr);
    ptr += sizeof(int);
    return ret;
}
inline static void _read_rect(LPBYTE& ptr, RECT& rc)
{
    memcpy(&rc, ptr, sizeof(RECT));
    ptr += sizeof(RECT);
}

void ExtView_LoadExt(LPCWSTR extFileName)
{
    m_isShowRadio = false;
    CFileRW file;
    if (!file.open(extFileName, 1, -1)) return;

    int size = file.size();
    if (size <= sizeof(THEME_NAMESPACE THEME_FILE_HEADER)) return;
    _str data(size);
    file.read((LPBYTE)data.data(), size);
    wstr caption(100);
    LPCWSTR pName = wcsrchr(extFileName, '\\');
    if (!pName) pName = wcsrchr(extFileName, '/');
    caption.assign(L"ires文件查看器");
    if (pName)
        caption.append(L"  当前文件 [").append(pName + 1).append(L"]");

    THEME_NAMESPACE LPTHEME_FILE_HEADER pTheme = (THEME_NAMESPACE LPTHEME_FILE_HEADER)data.c_str();
    if (pTheme->cbSize > 200 || pTheme->dwReserved != 0 || pTheme->dwCount > 65535)
    {
        caption.append(L"不是本工具生成的文件, 无法查看");
        SetWindowTextW(m_hWnd, caption.c_str());
        return;
    }
    _str pngData;
    static _str xmlData;
    THEME_NAMESPACE EXT_ERR err = THEME_NAMESPACE UnExtData(pTheme, &pngData, &xmlData);
    if (m_hBitmap) delete m_hBitmap;
    m_hBitmap = _img_create_from_memory(pngData.c_str(), pngData.size());

    const int nChildCount = (int)pTheme->dwCount;


    if (pTheme->cbSize == sizeof(THEME_NAMESPACE THEME_FILE_HEADER))    // 1.0 版本
    {
        m_cxIcon = LOWORD(pTheme->dwMaxSize);
        m_cyIcon = HIWORD(pTheme->dwMaxSize);
        LPBYTE ptr = (LPBYTE)xmlData.c_str();
        m_data.resize(nChildCount);
        for (int i = 0; i < nChildCount; i++)
        {
            LPCWSTR id = _read_string(ptr);
            EXT_LIST_DATA& data = m_data[i];
            data.id = id;

            _read_rect(ptr, data.rc[0]);
            _read_rect(ptr, data.rc[1]);
            _read_rect(ptr, data.rc[2]);
            _read_rect(ptr, data.rc[3]);
        }
        caption.append(L" ires文件版本 = v1.0");
    }
    else if (pTheme->cbSize == sizeof(THEME_NAMESPACE THEME_FILE_HEADER_1_1))           // 1.1 版本
    {
        m_cxIcon = LOWORD(pTheme->dwMaxSize);
        m_cyIcon = HIWORD(pTheme->dwMaxSize);
        //TODO 1.1读取的数据更多
        LPBYTE ptr = (LPBYTE)xmlData.c_str();
        m_data.resize(nChildCount);
        for (int i = 0; i < nChildCount; i++)
        {
            LPCWSTR id = _read_string(ptr);
            EXT_LIST_DATA& data = m_data[i];
            LPCWSTR text = _read_string(ptr);
            LPCWSTR remarks = _read_string(ptr);
            data.id = id;
            data.text = text;
            data.remarks = remarks;
            data.crText[0] = _read_int(ptr);
            data.crText[1] = _read_int(ptr);
            data.crText[2] = _read_int(ptr);
            data.crText[3] = _read_int(ptr);

            data.crBack[0] = _read_int(ptr);
            data.crBack[1] = _read_int(ptr);
            data.crBack[2] = _read_int(ptr);
            data.crBack[3] = _read_int(ptr);

            _read_int(ptr); // 5个备用成员
            _read_int(ptr);
            _read_int(ptr);
            _read_int(ptr);
            _read_int(ptr);

            _read_rect(ptr, data.rc[0]);
            _read_rect(ptr, data.rc[1]);
            _read_rect(ptr, data.rc[2]);
            _read_rect(ptr, data.rc[3]);

        }
        caption.append(L" ires文件版本 = v1.1");
    }
    else if (pTheme->cbSize == sizeof(THEME_NAMESPACE THEME_FILE_HEADER_1_2))           // 1.2 版本
    {
        m_cxIcon = LOWORD(pTheme->dwMaxSize);
        m_cyIcon = HIWORD(pTheme->dwMaxSize);
        m_isShowRadio = true;
        //TODO 1.2读取的数据更多
        LPBYTE ptr = (LPBYTE)xmlData.c_str();
        m_data.resize(nChildCount);
        for (int i = 0; i < nChildCount; i++)
        {
            LPCWSTR id = _read_string(ptr);
            EXT_LIST_DATA& data = m_data[i];
            LPCWSTR text = _read_string(ptr);
            LPCWSTR remarks = _read_string(ptr);
            data.id = id;
            data.text = text;
            data.remarks = remarks;
            data.crText[0] = _read_int(ptr);
            data.crText[1] = _read_int(ptr);
            data.crText[2] = _read_int(ptr);
            data.crText[3] = _read_int(ptr);

            data.crBack[0] = _read_int(ptr);
            data.crBack[1] = _read_int(ptr);
            data.crBack[2] = _read_int(ptr);
            data.crBack[3] = _read_int(ptr);

            _read_int(ptr); // 5个备用成员
            _read_int(ptr);
            _read_int(ptr);
            _read_int(ptr);
            _read_int(ptr);

            for (int i = 0; i < 20; i++)
                _read_rect(ptr, data.rc[i]);

        }
        caption.append(L" ires文件版本 = v1.2");
    }

    RECT rc;
    GetClientRect(m_hWnd, &rc);
    ExtView_WndProc(m_hWnd, WM_SIZE, 0, MAKELONG(rc.right - rc.left, rc.bottom - rc.top));
    SetWindowTextW(m_hWnd, caption.c_str());
    
    m_list.SetIndex(-1);
    m_list.SetItemCount(nChildCount, true);
    return;
}

inline HBRUSH ExtView_GetBrush(STATE state, bool isFocus)
{

    const int offsetHot = 10;
    const int offsetDown = 20;
    const int offsetSel = 30;

    COLORREF crBack = RGB(45, 45, 48);
    COLORREF crHot = RGB(45 + offsetHot, 45 + offsetHot, 48 + offsetHot);
    COLORREF crDown = RGB(45 + offsetDown, 45 + offsetDown, 48 + offsetDown);
    COLORREF crSelected = RGB(45 + offsetSel, 45 + offsetSel, 48 + offsetSel);

    static HBRUSH hbrBack = CreateSolidBrush(crBack);
    static HBRUSH hbrHot = CreateSolidBrush(crHot);         // 热点画刷
    static HBRUSH hbrSel = CreateSolidBrush(crSelected);    // 选中画刷
    static HBRUSH hbrDown = CreateSolidBrush(crDown);       // 按下画刷
    static HBRUSH hbrKillFocus = CreateSolidBrush(crSelected); // 失去焦点

    HBRUSH hbr = 0;
    switch (state)
    {
    case DOWN:      hbr = hbrDown;   break;
    case SELECTED:  hbr = hbrSel;    break;
    case HOT:       hbr = hbrHot;    break;
    default:
        break;
    }

    if (hbr)
    {
        if (!isFocus && hbr == hbrSel)
            hbr = hbrKillFocus;
    }
    return hbr;
}

bool CALLBACK ExtView_CustomListBox_DrawItem(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus)
{
    SetTextColor(hdc, DRAW_COLOR_TEXT);
    HBRUSH hbr = ExtView_GetBrush(state, isFocus);
    if (hbr)  FillRect(hdc, rcItem, hbr);

    RECT rc = *rcItem;
    rc.left += 5;
    rc.right = 30;
    const int cyItem = rc.bottom - rc.top;
    wstr str;
    str.assign(index + 1);
    DrawTextW(hdc, str.c_str(), -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS);

    rc.left = rc.right + 4;
    rc.top = rc.top + ((cyItem) - m_cyIcon) / 2;
    EXT_LIST_DATA& data = m_data[index];

    Gdiplus::Rect rcDraw(rc.left, rc.top, m_cxIcon, m_cyIcon);
    Gdiplus::Graphics gp(hdc);
    for (int i = 0; i < 4; i++)
    {
        int _index = m_dpiIndex * 4 + i;
        RECT& rcIcon = data.rc[_index];
        if (rcIcon.right > rcIcon.left)
        {
            Gdiplus::Rect rcImageDraw;
            rcImageDraw.Width = rcIcon.right - rcIcon.left;
            rcImageDraw.Height = rcIcon.bottom - rcIcon.top;
            rcImageDraw.X = rcDraw.X + (m_cxIcon - rcImageDraw.Width) / 2;
            rcImageDraw.Y = rcDraw.Y + (m_cyIcon - rcImageDraw.Height) / 2;
            gp.DrawImage(m_hBitmap, rcImageDraw, rcIcon.left, rcIcon.top, rcImageDraw.Width, rcImageDraw.Height, Gdiplus::Unit::UnitPixel);

        }
        rcDraw.Offset(m_cxIcon + 10, 0);
    }

    rc.left = rcDraw.X + m_cxIcon + 10;
    rc.right = rcItem->right;
    rc.top = rcItem->top;
    rc.bottom = rcItem->bottom;


    RECT rcText = rc;
    DrawTextW(hdc, L"id=\"", 4, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    DrawTextW(hdc, L"id=\"", 4, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);

    rc.left = rcText.right;
    rcText.left = rcText.right;
    rcText.right = rc.right - 10;
    COLORREF crText = SetTextColor(hdc, RGB(222, 89, 178));
    DrawTextW(hdc, data.id, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    DrawTextW(hdc, data.id, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
    SetTextColor(hdc, crText);

    rc.left = rcText.right;
    DrawTextW(hdc, L"\"", 1, &rc, DT_VCENTER | DT_SINGLELINE);
    rc.left = rcText.right + 20;
    rc.right = rcItem->right;

    if (data.remarks)
    {
        RECT rcText = rc;
        DrawTextW(hdc, L"说明=\"", 4, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        DrawTextW(hdc, L"说明=\"", 4, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);

        rc.left = rcText.right;
        rcText.left = rcText.right;
        rcText.right = rc.right - 10;
        COLORREF crText = SetTextColor(hdc, RGB(80, 161, 60));
        DrawTextW(hdc, data.remarks, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        DrawTextW(hdc, data.remarks, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
        SetTextColor(hdc, crText);

        rc.left = rcText.right;
        DrawTextW(hdc, L"\"", 1, &rc, DT_VCENTER | DT_SINGLELINE);
    }

    return true;
}

HWND ExtView_LoadWindow(HINSTANCE hInst, bool isLoopMsg, int x, int y, int cx, int cy)
{
    if (IsWindow(m_hWnd))
    {
        return m_hWnd;
    }
    m_dpiIndex = 0;
    m_isLoopMsg = isLoopMsg;
    m_hInst = hInst;
    m_hWnd = CreateWindowExW(WS_EX_ACCEPTFILES, L"#32770", L"ires文件查看器", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
        x, y, cx, cy, nullptr, nullptr, m_hInst, nullptr);

    if (!m_hWnd) return 0;
    ExtView_Create(m_hWnd);
    m_oldProc = (WNDPROC)SetWindowLongPtrW(m_hWnd, GWLP_WNDPROC, (LONG_PTR)ExtView_WndProc);

    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    if (isLoopMsg)
    {
        MSG msg;
        // 主消息循环:
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        return 0;
    }
    return m_hWnd;
}
