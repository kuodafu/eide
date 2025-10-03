#include "DrawScroll.h"
#include <tstr.h>
#include <map>
#include <control/WndBaseTypedef.h>
#include <hook_detours/apiHook.h>
#include <gdiplus/drawImage.h>
#include <control/src/scroll.h>
#include <windowsx.h>

#define SCROLL_OLDWNDPROC   L"{52E87CF4-098E-4E4E-94DA-DB66D2710FCF}"
#define SCROLL_THISCLASS    L"{350C4F6A-7D58-43BF-8B92-AD9A3F4BC5B9}"
#define SCROLL_ISBIND       L"{958E4B00-EE56-4C4E-AAFF-0A8287111BB2}"   // 是否为绑定的组件

#define CAPTURE_NONE        0
#define CAPTURE_VSCROLL     1
#define CAPTURE_HSCROLL     2
#define CAPTURE_BUTTON      3

#define INDEX_BUTTON1       1
#define INDEX_BUTTON2       2
#define INDEX_THUMB         3
#define INDEX_OUTER         4
#define INDEX_NONE          0


#define _BORDER_WIDTH_      1



static apiHook hook_SetScrollInfo;
static apiHook hook_GetScrollInfo;

static int(WINAPI* SetScrollInfo_hook)(HWND hwnd, int nBar, LPCSCROLLINFO lpsi, BOOL redraw) = SetScrollInfo;
static BOOL(WINAPI* GetScrollInfo_hook)(HWND hwnd, int nBar, LPSCROLLINFO lpsi) = GetScrollInfo;

inline static int WINAPI MySetScrollInfo_hook(HWND hwnd, int nBar, LPCSCROLLINFO lpsi, BOOL redraw)
{
    IDrawScroll* obj = IDrawScroll::GetobjFromHwnd(hwnd);
    if (!obj)
        return SetScrollInfo_hook(hwnd, nBar, lpsi, redraw);
    bool isVScroll = false;
    bool isControl = IDrawScroll::IsBindControl(hwnd, &isVScroll);

    LPSCROLLINFO ptr = 0;

    if (nBar == SB_CTL || isControl) // 是滚动条组件
    {
        if (!isControl)
        {
            DWORD dwStyle = (DWORD)GetWindowLongPtrW(hwnd, GWL_STYLE);
            isVScroll = (dwStyle & SBS_VERT) == SBS_VERT;
            obj->m_isVScroll = isVScroll;
            obj->m_isHScroll = !isVScroll;
            obj->m_isAbsScroll = true;
            obj->m_isControl = true;
        }
        
        if (isVScroll)  ptr = &obj->m_vScroll;
        else            ptr = &obj->m_hScroll;
        
    }
    else if (nBar == SB_VERT)   ptr = &obj->m_vScroll;
    else if (nBar == SB_HORZ)   ptr = &obj->m_hScroll;
    else return 0;

    SCROLLINFO& dest = *ptr;

    dest.fMask = lpsi->fMask;
    dest.cbSize = lpsi->cbSize;

    if ((lpsi->fMask & SIF_RANGE) == SIF_RANGE)    // nMin和nMax成员包含滚动范围的最小值和最大值
    {
        dest.nMax = lpsi->nMax;
        dest.nMin = lpsi->nMin;
    }
    if ((lpsi->fMask & SIF_PAGE) == SIF_PAGE)     // nPage成员包含比例滚动条的页面大小
    {
        dest.nPage = lpsi->nPage;
    }
    if ((lpsi->fMask & SIF_POS) == SIF_POS)      // nPos成员包含滚动框位置, 在用户拖动滚动框时不会更新
    {
        //dest.nPos = lpsi->nPos;
        //const int minPos = dest.nMin;
        //const int maxPos = (dest.nPage - 1 > 0) ? (dest.nMax - dest.nPage + 1) : dest.nMax;
        //dest.nPos = (dest.nPos < minPos) ? minPos : maxPos;

        dest.nPos = lpsi->nPos;
        //check pos
        //dest.nPos = max(dest.nMin, min(lpsi->nPos, dest.nMax));

        int minPos = dest.nMin;
        int maxPos;
        if (dest.nPage - 1 > 0)
        {
            maxPos = dest.nMax - dest.nPage + 1;
        }
        else
        {
            maxPos = dest.nMax;
        }

        if (dest.nPos < minPos)
        {
            dest.nPos = minPos;
        }
        else if (dest.nPos > maxPos)
        {
            dest.nPos = maxPos;
        }

    }
    if ((lpsi->fMask & SIF_TRACKPOS) == SIF_TRACKPOS) // nTrackPos成员包含用户拖动时滚动框的当前位置
    {
        dest.nTrackPos = lpsi->nTrackPos;
    }
    if ((lpsi->fMask & SIF_DISABLENOSCROLL) == SIF_DISABLENOSCROLL)  // 仅在设置滚动条的参数时使用此值。如果滚动条的新参数使滚动条不再需要，请禁用滚动条而不是将其删除
    {
        //dest.nPos = 0;
        dest.nTrackPos = 0;
    }

    bool isScroll = false;
    if (dest.nMax - dest.nMin <= 0 || dest.nMax < (int)dest.nPage)
    {
        dest.nPos = 0;
        dest.nTrackPos = 0;
        isScroll = false;
    }
    else
    {
        isScroll = true;
    }
    if (obj->m_isAbsScroll) isScroll = true;

    if (obj->m_isCallSetScroll || obj->m_isSysListView)
    {
        if (nBar == SB_VERT)
            obj->m_isVScroll = isScroll;
        else if (nBar == SB_HORZ)
            obj->m_isHScroll = isScroll;
    }

    int ret = dest.nPos;
    if (obj->m_isCallSetScroll)
        ret = SetScrollInfo_hook(hwnd, nBar, &dest, false);

    //const DWORD dwStyle = GetWindowLongPtrW(hwnd, GWL_STYLE);
    //obj->m_isVScroll = (dwStyle & WS_VSCROLL) == WS_VSCROLL;
    //obj->m_isHScroll = (dwStyle & WS_HSCROLL) == WS_HSCROLL;
    //wstr::dbg(L"滚动条位置 = %d\n", dest.nPos);
    //if (!isScroll)
    //{
    //    if (nBar == SB_VERT)
    //        obj->m_isVScroll = isScroll;
    //    else if (nBar == SB_HORZ)
    //        obj->m_isHScroll = isScroll;
    //}

    if (redraw) obj->InvalidateRect();
    
    return ret;
}

inline static BOOL WINAPI MyGetScrollInfo_hook(HWND hwnd, int nBar, LPSCROLLINFO lpsi)
{
    IDrawScroll* obj = IDrawScroll::GetobjFromHwnd(hwnd);
    if (!obj)
        return GetScrollInfo_hook(hwnd, nBar, lpsi);

    if (obj->m_isCallSetScroll)
        return GetScrollInfo_hook(hwnd, nBar, lpsi);

    LPSCROLLINFO ptr = 0;
    if (nBar == SB_VERT)
        ptr = &obj->m_vScroll;
    else if (nBar == SB_HORZ)
        ptr = &obj->m_hScroll;
    else
        return 0;

    SCROLLINFO& dest = *ptr;

    //if (lpsi->fMask & SIF_DISABLENOSCROLL)  // 仅在设置滚动条的参数时使用此值。如果滚动条的新参数使滚动条不再需要，请禁用滚动条而不是将其删除
    //    return false;
    if ((lpsi->fMask & SIF_PAGE) == SIF_PAGE)           // nPage成员包含比例滚动条的页面大小
        lpsi->nPage = dest.nPage;
    if ((lpsi->fMask & SIF_POS) == SIF_POS)             // nPos成员包含滚动框位置, 在用户拖动滚动框时不会更新
        lpsi->nPos = dest.nPos;
    if ((lpsi->fMask & SIF_RANGE) == SIF_RANGE)         // nMin和nMax成员包含滚动范围的最小值和最大值
        lpsi->nMax = dest.nMax, lpsi->nMin = dest.nMin;
    if ((lpsi->fMask & SIF_TRACKPOS) == SIF_TRACKPOS)   // nTrackPos成员包含用户拖动时滚动框的当前位置
        lpsi->nTrackPos = dest.nTrackPos;
    return true;
}

static LRESULT CALLBACK WndProc_Scroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WNDPROC oldProc = (WNDPROC)GetPropW(hWnd, SCROLL_OLDWNDPROC);
    if (!oldProc)return DefWindowProcW(hWnd, message, wParam, lParam);

    IDrawScroll* ptr = (IDrawScroll*)GetPropW(hWnd, SCROLL_THISCLASS);
    if (!ptr)
        return DefWindowProcW(hWnd, message, wParam, lParam);
    LRESULT ret = 0;
    bool isRet = ptr->OnWndProc(oldProc, hWnd, message, wParam, lParam, &ret);
    if (isRet) return ret;
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

inline static bool IsBorder(HWND hWnd)
{
    DWORD style = (DWORD)GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_BORDER)return true;
    DWORD styleEx = (DWORD)GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((styleEx & WS_EX_CLIENTEDGE)
        || (styleEx & WS_EX_DLGMODALFRAME)
        || (styleEx & WS_EX_WINDOWEDGE)
        )
        return true;
    return false;
}

bool IDrawScroll_Init()
{
    bool ret = hook_SetScrollInfo.hook(&(PVOID&)SetScrollInfo_hook, MySetScrollInfo_hook);
    ret = hook_GetScrollInfo.hook(&(PVOID&)GetScrollInfo_hook, MyGetScrollInfo_hook);
    return ret;
}

bool IDrawScroll_UnInit()
{
    bool ret = hook_SetScrollInfo.unHook();
    ret = hook_GetScrollInfo.unHook();
    return ret;
}
// 取窗口客户区坐标尺寸, 左边顶边是客户区在屏幕中的位置
inline static bool Scroll_GetClientSize(HWND hWnd, RECT& rc, RECT& rcClient, RECT& rcBorder, int& cxClient, int& cyClient, int& cxScreen, int& cyScreen)
{
    ::GetWindowRect(hWnd, &rc);
    ::GetClientRect(hWnd, &rcClient);
    cxClient = rcClient.right - rcClient.left;  // 记录窗口客户区尺寸
    cyClient = rcClient.bottom - rcClient.top;

    ::ClientToScreen(hWnd, (LPPOINT)&rcClient);
    rcClient.right = rcClient.left + cxClient;
    rcClient.bottom = rcClient.top + cyClient;

    rcBorder.left = rcClient.left - rc.left;
    rcBorder.top = rcClient.top - rc.top;
    rcBorder.right = rc.right - rcClient.right;
    rcBorder.bottom = rc.bottom - rcClient.bottom;

    rcClient.left = rcBorder.left + rc.left;
    rcClient.top = rcBorder.top + rc.top;
    rcClient.right = rcClient.left + cxClient;
    rcClient.bottom = rcClient.top + cyClient;


    cxScreen = rc.right - rc.left;      // 窗口宽度, 包含非客户区
    cyScreen = rc.bottom - rc.top;      // 窗口高度, 包含非客户区
    //rc = { 0, 0, cxScreen, cyScreen };  // 非客户区的整体矩形

    return true;
}

inline bool pfn_DrawScroll(pfn_IScroll_DrawScroll m_pfn_DrawScroll, IDrawScroll* pObj, bool isVScroll, HDC hdc, Gdiplus::Graphics* gp, ISCROLL_TYPE type, RECT_F* rcDraw)
{
    if (!m_pfn_DrawScroll) return false;
    return m_pfn_DrawScroll(pObj, isVScroll, hdc, gp, type, rcDraw);
}

void IDrawScroll::clear()
{
    memset(this, 0, sizeof(*this));
}

IDrawScroll::IDrawScroll(COLORREF crBorder, SCROLL_RECT_INFO& info)
{
    clear();
    SetBorderColor(crBorder);
    m_vInfo = info;
    m_hInfo = info;
}
void IDrawScroll::SetBorderColor(COLORREF clrBorder)
{
    if (m_hPen)
        DeleteObject(m_hPen);
    m_hPen = CreatePen(PS_INSIDEFRAME, 1, clrBorder);
}


IDrawScroll::~IDrawScroll()
{
    md_destroy(&m_md, false);
    md_destroy(&m_vmd, false);
    md_destroy(&m_hmd, false);
    DeleteObject(m_hPen);
    if (IsWindow(m_hWnd))
    {
        SetPropW(m_hWnd, SCROLL_THISCLASS, 0);
        WNDPROC nowProc = (WNDPROC)GetWindowLongPtrW(m_hWnd, GWLP_WNDPROC);
        WNDPROC oldProc = (WNDPROC)GetPropW(m_hWnd, SCROLL_OLDWNDPROC);
        if (nowProc == WndProc_Scroll && oldProc)
            SetWindowLongPtrW(m_hWnd, GWLP_WNDPROC, (LONG_PTR)oldProc);
    }
    clear();
}


bool IDrawScroll::IsBindControl(HWND hWnd, bool* isVScroll)
{
    if (isVScroll)*isVScroll = false;
    if (!hWnd) return false;
    int ret = (int)(LONG_PTR)GetPropW(hWnd, SCROLL_ISBIND);
    if (isVScroll)*isVScroll = HIWORD(ret);
    return LOWORD(ret);
}

void IDrawScroll::SetBindControl(HWND hWnd, bool isBind, bool isVScroll)
{
    if (!hWnd) return;
    SetPropW(hWnd, SCROLL_ISBIND, (void*)(LONG_PTR)MAKELONG(isBind, isVScroll));
}

IDrawScroll* IDrawScroll::GetobjFromHwnd(HWND hWnd)
{
    return (IDrawScroll*)GetPropW(hWnd, SCROLL_THISCLASS);
}

IDrawScroll* IDrawScroll::SetobjFromHwnd(HWND hWnd, IDrawScroll* pScroll)
{
    IDrawScroll* old = (IDrawScroll*)GetPropW(hWnd, SCROLL_THISCLASS);
    SetPropW(hWnd, SCROLL_THISCLASS, pScroll);
    return old;
}


bool IDrawScroll::bind(HWND hWnd, bool isSubClass, bool isReCalcSize)
{
    m_hWnd = hWnd;
    if (!hWnd)return false;

    if (!m_isControl)
    {
        const DWORD dwStyle = (DWORD)GetWindowLongPtrW(hWnd, GWL_STYLE);
        m_isVScroll = (dwStyle & WS_VSCROLL) == WS_VSCROLL;
        m_isHScroll = (dwStyle & WS_HSCROLL) == WS_HSCROLL;
        m_nBorderWidth = IsBorder(hWnd) ? _BORDER_WIDTH_ : 0;
    }
    else
    {
        m_vScroll.cbSize = sizeof(SCROLLINFO);
        m_hScroll.cbSize = sizeof(SCROLLINFO);
        m_vScroll.fMask = SIF_ALL;
        m_hScroll.fMask = SIF_ALL;
        if (m_isVScroll)
            SendMessageW(hWnd, SBM_GETSCROLLINFO, SB_CTL, (LPARAM)&m_vScroll);
        else
            SendMessageW(hWnd, SBM_GETSCROLLINFO, SB_CTL, (LPARAM)&m_hScroll);

    }
    SetPropW(hWnd, SCROLL_THISCLASS, this);
    if (isSubClass)
    {
        WNDPROC oldProc = (WNDPROC)GetWindowLongPtrW(hWnd, GWLP_WNDPROC);
        if (oldProc != WndProc_Scroll)
        {
            SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc_Scroll);
            SetPropW(hWnd, SCROLL_OLDWNDPROC, oldProc);
        }
    }
    if (isReCalcSize)
    {
        SetWindowPos(m_hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
        //InvalidateRect();
    }
    return true;
}

bool IDrawScroll::OnWndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (!m_hWnd)return false;
    switch (message)
    {
    case WM_NCHITTEST:
        return OnNcHittest(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_NCMOUSEMOVE:
        return OnNcMouseMove(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_NCLBUTTONDOWN:
        return OnNcLbuttonDown(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_NCLBUTTONUP:
        return OnNcLbuttonUp(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_NCLBUTTONDBLCLK:
        return OnNcLbuttonDblclk(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_NCRBUTTONDOWN:
        return OnNcRbuttonDown(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_NCRBUTTONUP:
        return OnNcRbuttonUp(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_MOUSEMOVE:
    {
        if (m_isCapture)
            return OnThumbMove(oldProc, hWnd, message, wParam, lParam, pRet);
        if (m_isControl)
        {
            POINT pt;
            GetCursorPos(&pt);
            if (m_isVScroll)
                GetVScrollRect(&m_vrc.rc);
            else
                GetHScrollRect(&m_hrc.rc);

            return OnNcMouseMove(oldProc, hWnd, message, wParam, MAKELONG(pt.x, pt.y), pRet);
        }
        
        return false;
    }
    case WM_LBUTTONDOWN:
    {
        if (!m_isControl) return false;
        POINT pt;
        GetCursorPos(&pt);
        return OnNcLbuttonDown(oldProc, hWnd, message, m_isVScroll ? HTVSCROLL : HTHSCROLL, MAKELONG(pt.x, pt.y), pRet);
    }
    case WM_LBUTTONUP:
    {
        if (m_isCapture)
        {
            ReleaseCapture();
            return true;
        }
        if (m_isControl)
            return OnNcLbuttonUp(oldProc, hWnd, message, wParam, lParam, pRet);
        return false;
    }
    case WM_NCRBUTTONDBLCLK:
        return OnNcRbuttonDblclk(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_NCCALCSIZE:
        return OnNcCalcSize(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_NCPAINT:
        return OnNcPaint(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_PRINT:
        return OnPrint(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_PAINT:
        return OnPaint(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_CAPTURECHANGED:
        return OnCaptureChanged(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_STYLECHANGING:
    case WM_STYLECHANGED:
    {
        LPSTYLESTRUCT ss = (LPSTYLESTRUCT)lParam;

        m_nBorderWidth = 0;
        if (wParam == GWL_STYLE)
        {
            if (ss->styleNew & WS_BORDER)
                m_nBorderWidth = _BORDER_WIDTH_;
            m_isVScroll = (ss->styleNew & WS_VSCROLL) == WS_VSCROLL;
            m_isHScroll = (ss->styleNew & WS_HSCROLL) == WS_HSCROLL;
        }
        else
        {
            if ((ss->styleNew & WS_EX_CLIENTEDGE)
                || (ss->styleNew & WS_EX_DLGMODALFRAME)
                || (ss->styleNew & WS_EX_WINDOWEDGE)
                )
                m_nBorderWidth = _BORDER_WIDTH_;
        }
        return false;
    }
    //case WM_MOUSEHWHEEL:
    //case WM_MOUSEWHEEL:
    //{
    //    *pRet = CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    //    OnNcPaint(oldProc, hWnd, message, wParam, lParam, pRet);
    //    return true;
    //}
    case WM_HSCROLL:
        return OnHScroll(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_VSCROLL:
        return OnVScroll(oldProc, hWnd, message, wParam, lParam, pRet);
    case WM_MOUSEHWHEEL:    // 横向滚动
    {
        if (!m_isControl)return false;
        int pos = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        pos = m_hScroll.nPos - pos;
        pos = min(m_hScroll.nMax, max(pos, m_hScroll.nMin));
        if (pos != m_hScroll.nPos)
        {
            InvalidateRect();
            m_hScroll.nPos = pos;
        }
        return false;
    }
    case WM_MOUSEWHEEL:     // 纵向滚动
    {
        if (!m_isControl)return false;
        int pos = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        pos = m_vScroll.nPos - pos;
        pos = min(m_vScroll.nMax, max(pos, m_vScroll.nMin));
        if (pos != m_vScroll.nPos)
        {
            InvalidateRect();
            m_vScroll.nPos = pos;
        }
        return false;
    }
    case SBM_SETSCROLLINFO:
    {
        if (!m_isControl)return false;
        LPSCROLLINFO ptr;
        if (m_isVScroll)    ptr = &m_vScroll;
        else                ptr = &m_hScroll;

        SCROLLINFO& dest = *ptr;
        LPSCROLLINFO lpsi = (LPSCROLLINFO)lParam;
        if ((lpsi->fMask & SIF_PAGE) == SIF_PAGE)           // nPage成员包含比例滚动条的页面大小
            dest.nPage = lpsi->nPage;
        if ((lpsi->fMask & SIF_POS) == SIF_POS)             // nPos成员包含滚动框位置, 在用户拖动滚动框时不会更新
            dest.nPos = lpsi->nPos;
        if ((lpsi->fMask & SIF_RANGE) == SIF_RANGE)         // nMin和nMax成员包含滚动范围的最小值和最大值
            dest.nMax = lpsi->nMax, dest.nMin = lpsi->nMin;
        if ((lpsi->fMask & SIF_TRACKPOS) == SIF_TRACKPOS)   // nTrackPos成员包含用户拖动时滚动框的当前位置
            dest.nTrackPos = lpsi->nTrackPos;
        return false;
    }
    case SBM_SETRANGE:
    {
        if (!m_isControl)return false;
        LPSCROLLINFO ptr;
        if (m_isVScroll)    ptr = &m_vScroll;
        else                ptr = &m_hScroll;
        SCROLLINFO& dest = *ptr;
        dest.nMin = (int)wParam;
        dest.nMax = (int)lParam;
        return false;
    }
    case SBM_SETPOS:
    {
        if (!m_isControl)return false;
        LPSCROLLINFO ptr;
        if (m_isVScroll)    ptr = &m_vScroll;
        else                ptr = &m_hScroll;
        SCROLLINFO& dest = *ptr;
        dest.nPos = (int)wParam;
        return false;
    }
    default:
        return false;
    }
    return true;
}

// prcScreen = 滚动条在屏幕中的位置
// prcClient = 滚动条在这个窗口中的位置
// prcBtn1 = 滚动条顶边的按钮在屏幕中的位置
// prcBtn2 = 滚动条底边的按钮在屏幕中的位置
// prcThumb = 滚动条滑块在屏幕中的位置
// prcWindow = 滚动条所在窗口的非客户区左上角和右下角在屏幕坐标
// rcWindowClient = 滚动条所在窗口的客户区左上角和右下角在屏幕坐标
// prcBorder = 滚动条所在窗口的窗口4个边边框的宽高
bool IDrawScroll::GetVScrollRect(RECT* prcScreen, RECT* prcClient, RECT* prcBtn1, RECT* prcBtn2, RECT* prcThumb, RECT* prcWindow, RECT* rcWindowClient, RECT* prcBorder)
{
    if (prcScreen) memset(prcScreen, 0, sizeof(RECT));
    if (prcClient) memset(prcClient, 0, sizeof(RECT));
    if (prcBtn1) memset(prcBtn1, 0, sizeof(RECT));
    if (prcBtn2) memset(prcBtn2, 0, sizeof(RECT));
    if (prcThumb) memset(prcThumb, 0, sizeof(RECT));
    if (prcWindow) memset(prcWindow, 0, sizeof(RECT));
    if (rcWindowClient) memset(rcWindowClient, 0, sizeof(RECT));
    if (prcBorder) memset(prcBorder, 0, sizeof(RECT));
    if (m_vInfo.btnThumbSize <= 0 || m_isVScroll == false)
        return false;
    
    RECT rc, rcClient, rcBorder;
    int cxClient, cyClient, cxScreen, cyScreen;

    Scroll_GetClientSize(m_hWnd, rc, rcClient, rcBorder, cxClient, cyClient, cxScreen, cyScreen);

    if (m_pfn_GetScrollRect)
    {
        RECT rcBorder1 = rcBorder;
        m_pfn_GetScrollRect(this, true, ISCROLL_GETRECT_BORDER, &rcBorder1);
        if (memcmp(&rcBorder, &rcBorder1, sizeof(RECT)) != 0)
        {
            rcBorder = rcBorder1;
            rcClient = { rc.left + rcBorder.left, rc.top + rcBorder.top, rc.right - rcBorder.right, rc.bottom - rcBorder.bottom };
            cxClient = rcClient.right - rcClient.left;
            cyClient = rcClient.bottom - rcClient.top;
        }
    }
    if (prcWindow)memcpy(prcWindow, &rc, sizeof(RECT));         // 窗口屏幕坐标
    if (prcBorder)memcpy(prcBorder, &rcBorder, sizeof(RECT));   // 窗口边框, 通过窗口屏幕坐标可以计算客户区坐标
    if (rcWindowClient)memcpy(rcWindowClient, &rcClient, sizeof(RECT));   // 窗口边框, 通过窗口屏幕坐标可以计算客户区坐标

    if (!m_isControl)
    {
        // rc = 滚动条在屏幕中的位置, x,y是屏幕坐标
        rc.left = rcClient.right;
        rc.top = rcClient.top;
        rc.right = rcClient.right + (int)m_vInfo.btnThumbSize;
        rc.bottom = rcClient.bottom;

        if (m_isHScroll)
            rc.bottom += (int)m_hInfo.btnThumbSize;    // 如果有两个滚动条, 需要把纵向滚动条往下增加一个按钮的高度
    }
    
    if (prcScreen)memcpy(prcScreen, &rc, sizeof(RECT));         // 滚动条屏幕坐标
    if (prcClient)
    {
        if (m_isControl)
        {
            prcClient->left = 0;
            prcClient->top = 0;
            prcClient->right = cxClient;
            prcClient->bottom = cyClient;
        }
        else
        {
            prcClient->left = cxScreen - rcBorder.right;
            prcClient->top = rcBorder.top;
            prcClient->right = prcClient->left + (int)m_vInfo.btnThumbSize;
            prcClient->bottom = prcClient->top + cyClient;
        }
        if (m_isHScroll)
            prcClient->bottom += (int)m_hInfo.btnThumbSize;
    }
    if (prcBtn1)
    {
        prcBtn1->left = rc.left;
        prcBtn1->top = rc.top;
        prcBtn1->right = rc.right;
        prcBtn1->bottom = prcBtn1->top + (int)(m_vInfo.btn1Size);
    }
    if (prcBtn2)
    {
        prcBtn2->left = rc.left;
        prcBtn2->top = rc.bottom - (int)(m_vInfo.btn2Size);
        prcBtn2->right = rc.right;
        prcBtn2->bottom = rc.bottom;
    }
    if (prcThumb)
    {
        prcThumb->left = rc.left;
        prcThumb->top = rc.top + (int)(m_vInfo.btn1Size);
        prcThumb->right = rc.right;
        prcThumb->bottom = rc.bottom - (int)(m_vInfo.btn2Size);
    }

    return true;
}

// prcScreen = 滚动条在屏幕中的位置
// prcClient = 滚动条在这个窗口中的位置
// prcBtn1 = 滚动条左边的按钮在屏幕中的位置
// prcBtn2 = 滚动条右边的按钮在屏幕中的位置
// prcThumb = 滚动条滑块在屏幕中的位置
// prcWindow = 滚动条所在窗口的非客户区左上角和右下角在屏幕坐标
// rcWindowClient = 滚动条所在窗口的客户区左上角和右下角在屏幕坐标
// prcBorder = 滚动条所在窗口的窗口4个边边框的宽高
bool IDrawScroll::GetHScrollRect(RECT* prcScreen, RECT* prcClient, RECT* prcBtn1, RECT* prcBtn2, RECT* prcThumb, RECT* prcWindow, RECT* rcWindowClient, RECT* prcBorder)
{
    if (prcScreen) memset(prcScreen, 0, sizeof(RECT));
    if (prcClient) memset(prcClient, 0, sizeof(RECT));
    if (prcBtn1) memset(prcBtn1, 0, sizeof(RECT));
    if (prcBtn2) memset(prcBtn2, 0, sizeof(RECT));
    if (prcThumb) memset(prcThumb, 0, sizeof(RECT));
    if (prcWindow) memset(prcWindow, 0, sizeof(RECT));
    if (rcWindowClient) memset(rcWindowClient, 0, sizeof(RECT));
    if (prcBorder) memset(prcBorder, 0, sizeof(RECT));
    if ((int)m_hInfo.btnThumbSize <= 0 || m_isHScroll == false)
        return false;

    RECT rc, rcClient, rcBorder;
    int cxClient, cyClient, cxScreen, cyScreen;

    Scroll_GetClientSize(m_hWnd, rc, rcClient, rcBorder, cxClient, cyClient, cxScreen, cyScreen);
    if (m_pfn_GetScrollRect)
    {
        RECT rcBorder1 = rcBorder;
        m_pfn_GetScrollRect(this, false, ISCROLL_GETRECT_BORDER, &rcBorder1);
        if (memcmp(&rcBorder, &rcBorder1, sizeof(RECT)) != 0)
        {
            rcBorder = rcBorder1;
            rcClient = { rc.left + rcBorder.left, rc.top + rcBorder.top, rc.right - rcBorder.right, rc.bottom - rcBorder.bottom };
            cxClient = rcClient.right - rcClient.left;
            cyClient = rcClient.bottom - rcClient.top;
        }
    }
    if (prcWindow)memcpy(prcWindow, &rc, sizeof(RECT));         // 窗口屏幕坐标
    if (prcBorder)memcpy(prcBorder, &rcBorder, sizeof(RECT));   // 窗口边框, 通过窗口屏幕坐标可以计算客户区坐标
    if (rcWindowClient)memcpy(rcWindowClient, &rcClient, sizeof(RECT));   // 窗口边框, 通过窗口屏幕坐标可以计算客户区坐标

    if (!m_isControl)
    {    // rc = 滚动条在屏幕中的位置, x,y是屏幕坐标
        rc.left = rcClient.left;
        rc.top = rcClient.bottom;
        rc.right = rcClient.right;
        rc.bottom = rcClient.bottom + (int)m_hInfo.btnThumbSize;
    }

    if (prcScreen)memcpy(prcScreen, &rc, sizeof(RECT));         // 滚动条屏幕坐标
    if (prcClient)
    {
        if (m_isControl)
        {
            prcClient->left = 0;
            prcClient->top = 0;
            prcClient->right = cxClient;
            prcClient->bottom = cyClient;
        }
        else
        {
            prcClient->left = rcBorder.left;
            prcClient->top = cyScreen - rcBorder.bottom;
            prcClient->right = prcClient->left + cxClient;
            prcClient->bottom = prcClient->top + (int)m_hInfo.btnThumbSize;
        }
    }
    if (prcBtn1)
    {
        prcBtn1->left = rc.left;
        prcBtn1->top = rc.top;
        prcBtn1->right = prcBtn1->left + (int)(m_vInfo.btn1Size);
        prcBtn1->bottom = rc.bottom;
    }
    if (prcBtn2)
    {
        prcBtn2->left = rc.right - (int)(m_vInfo.btn2Size);
        prcBtn2->top = rc.top;
        prcBtn2->right = rc.right;
        prcBtn2->bottom = rc.bottom;
    }
    if (prcThumb)
    {
        prcThumb->left = rc.left + (int)(m_vInfo.btn1Size);
        prcThumb->top = rc.top;
        prcThumb->right = rc.right - (int)(m_vInfo.btn2Size);
        prcThumb->bottom = rc.bottom;
    }

    return true;
}


void CALLBACK TimerProc_NcMouseLeave(HWND hWnd, UINT message, UINT_PTR id, DWORD dwTimer)
{
    IDrawScroll* pObj = (IDrawScroll*)id;
    //KillTimer(hWnd, id);
    
    if (pObj->GetState(false) == STATE::NORMAL && pObj->GetState(true) == STATE::NORMAL)
    {
        KillTimer(hWnd, id);
        pObj->SetIndex(0, true);
        pObj->SetIndex(0, false);
        pObj->InvalidateRect();
        return;
    }

    POINT pt;
    GetCursorPos(&pt);

    RECT rcScroll;
    pObj->GetVScrollRect(&rcScroll);
    if (PtInRect(&rcScroll, pt))    // 鼠标在纵向滚动条里
    {
        return;
    }
    pObj->GetHScrollRect(&rcScroll);
    if (PtInRect(&rcScroll, pt))    // 鼠标在横向滚动条里
    {
        return;
    }

    // 走到这鼠标没有在任何滚动条里
    bool isDraw = false;
    if (pObj->IsState(STATE::HOT, true))
    {
        pObj->SetState(STATE::NORMAL, true);
        pObj->SetIndex(0, true);
        isDraw = true;
    }
    if (pObj->IsState(STATE::HOT, false))
    {
        pObj->SetState(STATE::NORMAL, false);
        pObj->SetIndex(0, false);
        isDraw = true;
    }
    if (isDraw)
        pObj->InvalidateRect();
}

bool IDrawScroll::OnNcHittest(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (!m_isVScroll && !m_isHScroll || m_isControl)return false;
    const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    RECT rcScroll;
    if (m_isVScroll && m_isCapture != CAPTURE_VSCROLL)  // 有纵向滚动条, 并且当前没有被捕获
    {
        GetVScrollRect(&rcScroll);
        if (PtInRect(&rcScroll, pt))    // 鼠标在纵向滚动条里
        {
            m_vrc.rc = rcScroll;
            *pRet = HTVSCROLL;
            return true;
        }
    }
    if (m_isHScroll && m_isCapture != CAPTURE_VSCROLL)  // 有横向滚动条, 并且当前没有被捕获
    {
        GetHScrollRect(&rcScroll);
        if (PtInRect(&rcScroll, pt))    // 鼠标在横向滚动条里
        {
            *pRet = HTHSCROLL;
            m_hrc.rc = rcScroll;
            return true;
        }
    }
    return false;
}

// 坐标点是哪个按钮的位置
int IDrawScroll::GetVHittestPos(const POINT& pt)
{
    if (PtInRect(&m_vrc.rc, pt))      // 鼠标在纵向滚动条里
    {
        if (PtInRect(&m_vrc.btn1, pt))
            return INDEX_BUTTON1;
        if (PtInRect(&m_vrc.btn2, pt))
            return INDEX_BUTTON2;
        if (PtInRect(&m_vrc.thumb, pt))
            return INDEX_THUMB;
        return INDEX_OUTER;
    }
    return INDEX_NONE;
}

// 坐标点是哪个按钮的位置
int IDrawScroll::GetHHittestPos(const POINT& pt)
{
    if (PtInRect(&m_hrc.rc, pt))      // 鼠标在横向滚动条里
    {
        if (PtInRect(&m_hrc.btn1, pt))
            return INDEX_BUTTON1;
        if (PtInRect(&m_hrc.btn2, pt))
            return INDEX_BUTTON2;
        if (PtInRect(&m_hrc.thumb, pt))
            return INDEX_THUMB;
        return INDEX_OUTER;
    }
    return INDEX_NONE;
}

bool IDrawScroll::OnNcMouseMove(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    bool isScroll = false;  // 是否有滚动条
    if (m_isCapture != CAPTURE_VSCROLL && m_isVScroll)  // 没有捕获纵向滚动条才触发鼠标移动在滚动条里, 并且存在横向滚动条
    {
        int index = GetVHittestPos(pt);
        if (index != INDEX_NONE)    // 鼠标在纵向滚动条里
        {
            if (index == GetIndex(true))
            {
                if (GetState(false) != STATE::NORMAL || GetIndex(false) != 0)   // 如果横向滚动条有状态, 就去除横向滚动条的状态, 然后重画
                {
                    SetState(STATE::NORMAL, false);
                    SetIndex(INDEX_NONE, false);
                    InvalidateRect();
                }
                return true;
            }

            SetIndex(index, true);          // 设置当前状态按钮的索引
            SetState(STATE::HOT, true);     // 设置当前状态
            SetState(STATE::NORMAL, false); // 去除横向滚动条状态
            SetIndex(INDEX_NONE, false);    // 设置横向滚动条索引
            InvalidateRect();
            SetTimer(hWnd, (UINT_PTR)this, 30, TimerProc_NcMouseLeave);
            return true;
        }
        isScroll = true;
    }
    if (m_isCapture != CAPTURE_HSCROLL && m_isHScroll)  // 没有捕获横向滚动条才触发鼠标移动在滚动条里, 并且存在横向滚动条
    {
        int index = GetHHittestPos(pt);
        if (index != INDEX_NONE)    // 鼠标在横向滚动条里
        {
            if (index == GetIndex(false))
            {
                if (GetState(true) != STATE::NORMAL || GetIndex(true) != 0)     // 如果纵向滚动条有状态, 就去除纵向滚动条的状态, 然后重画
                {
                    SetState(STATE::NORMAL, true);
                    SetIndex(INDEX_NONE, true);
                    InvalidateRect();
                }
                return true;
            }
            SetIndex(index, false);         // 设置当前状态按钮的索引
            SetState(STATE::HOT, false);    // 设置当前状态
            SetState(STATE::NORMAL, true);  // 去除纵向滚动条状态
            SetIndex(INDEX_NONE, true);     // 设置纵向滚动条索引

            InvalidateRect();
            SetTimer(hWnd, (UINT_PTR)this, 30, TimerProc_NcMouseLeave);
            return true;
        }
        isScroll = true;
    }

    if (isScroll)   // 有滚动条才处理重画信息
    {
        // 走到这里就不是在任何滚动条里, 去除滚动条状态
        bool isDraw = false;
        // 没有捕获鼠标, 并且纵向滚动条有绘画状态, 需要清除然后重画
        if (m_isCapture != CAPTURE_VSCROLL && (GetState(true) != STATE::NORMAL || GetIndex(true) != INDEX_NONE))
        {
            SetState(STATE::NORMAL, true);
            SetIndex(INDEX_NONE, true);
            isDraw = true;
        }

        // 没有捕获鼠标, 并且横向滚动条有绘画状态, 需要清除然后重画
        if (m_isCapture != CAPTURE_HSCROLL && (GetState(false) != STATE::NORMAL || GetIndex(false) != INDEX_NONE))
        {
            SetState(STATE::NORMAL, false);
            SetIndex(INDEX_NONE, false);
            isDraw = true;
        }
        if (isDraw)
            InvalidateRect();
        return true;
    }
    return false;
}

bool IDrawScroll::OnNcCalcSize(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (m_isControl)return false;
    //if (!m_isHScroll && !m_isVScroll)return false;

    if (wParam)
    {
        LPNCCALCSIZE_PARAMS p = (LPNCCALCSIZE_PARAMS)lParam;
        bool isReturn = false;
        if (m_pfn_CalcScroll)
        {
            isReturn = m_pfn_CalcScroll(this, &p->rgrc[0]);
        }
        if (!isReturn)
        {
            if (m_nBorderWidth)
            {
                p->rgrc[0].left += m_nBorderWidth;
                p->rgrc[0].top += m_nBorderWidth;
                p->rgrc[0].right -= m_nBorderWidth;
                p->rgrc[0].bottom -= m_nBorderWidth;
            }
            if (m_isVScroll)
                p->rgrc[0].right -= (int)m_vInfo.btnThumbSize;
            if (m_isHScroll)
                p->rgrc[0].bottom -= (int)m_hInfo.btnThumbSize;
        }
        *pRet = 0;
        return true;
    }
    PRECT pRc = (PRECT)lParam;
    if (m_isVScroll)
        pRc->right -= (int)m_vInfo.btnThumbSize;
    if (m_isHScroll)
        pRc->bottom -= (int)m_hInfo.btnThumbSize;
    *pRet = 0;
    return true;
}

// 取窗口非客户区左顶右底边框宽度
// prcBorder = 接收4个边框的宽度, 为0则不获取, 4个边都为0表示没有边框
// prcWindow = 接收该窗口在屏幕中的位置, GetWindowRect() 返回的值
// prcClient = 接收该窗口的客户区位置, GetClientRect() 返回的值
// prcClientScreen = 接收该窗口客户区在屏幕中的位置, 去除了边框
inline static bool WINAPI WndBase_GetRect(HWND hWnd, RECT* prcBorder = 0, RECT* prcWindow = 0, RECT* prcClient = 0, RECT* prcClientScreen = 0) {
    if (!hWnd)return false;
    if (!prcBorder && !prcWindow && !prcClientScreen)return false;
    RECT rc, rcClient, rcBorder;
    ::GetWindowRect(hWnd, &rc);
    ::GetClientRect(hWnd, &rcClient);
    if (prcWindow)memcpy(prcWindow, &rc, sizeof(RECT));
    if (prcClient)memcpy(prcClient, &rcClient, sizeof(RECT));

    int cxClient = rcClient.right - rcClient.left;  // 记录窗口客户区尺寸
    int cyClient = rcClient.bottom - rcClient.top;

    ::ClientToScreen(hWnd, (LPPOINT)&rcClient);
    rcClient.right = rcClient.left + cxClient;
    rcClient.bottom = rcClient.top + cyClient;

    rcBorder.left = rcClient.left - rc.left;
    rcBorder.top = rcClient.top - rc.top;
    rcBorder.right = rc.right - rcClient.right;
    rcBorder.bottom = rc.bottom - rcClient.bottom;

    if (prcBorder)
        memcpy(prcBorder, &rcBorder, sizeof(RECT));
    if (!prcClientScreen)return true;

    rcClient.left = rcBorder.left + rc.left;
    rcClient.top = rcBorder.top + rc.top;
    rcClient.right = rcClient.left + cxClient;
    rcClient.bottom = rcClient.top + cyClient;
    memcpy(prcClientScreen, &rcClient, sizeof(RECT));
    return true;
}

bool IDrawScroll::OnDrawScroll(HDC hdcWindow)
{
    RECT rc, rcClient, rcScroll, rcScrollClient;
    if (m_isVScroll)
        GetVScrollRect(&rcScroll, &rcScrollClient, 0, 0, 0, &rc, &rcClient);
    else
        GetHScrollRect(&rcScroll, &rcScrollClient, 0, 0, 0, &rc, &rcClient);
    const int cxClient = rcClient.right - rcClient.left;
    const int cyClient = rcClient.bottom - rcClient.top;
    const int cxScreen = rc.right - rc.left;
    const int cyScreen = rc.bottom - rc.top;

    if (rcScroll.right - rcScroll.left <= 0 || rcScroll.bottom - rcScroll.top <= 0)
    {
        return false;
        if (m_nBorderWidth == 0)  // 没有边框就默认处理
            return false;

        HGDIOBJ hOldPen = SelectObject(hdcWindow, m_hPen);
        HGDIOBJ hOldBrush = SelectObject(hdcWindow, GetStockBrush(NULL_BRUSH));

        Rectangle(hdcWindow, 0, 0, cxScreen, cyScreen);
        SelectObject(hdcWindow, hOldBrush);
        SelectObject(hdcWindow, hOldPen);
        return true;
    }


    
    // 绘画滚动条的时候是绘画到了内存DC上, 所以可以不使用双缓冲
    // 这里要做的就是把画好了两个滚动条给粘贴到窗口DC上
    const bool isMemoryDC = false;    // 这里不需要双缓冲, 直接绘画到结果DC上, 需要的设置为true
    HDC hdc = 0;
    if (isMemoryDC)
    {
        md_resize(&m_md, cxScreen, cyScreen);
        hdc = m_md.hdc;
        BitBlt(hdc, 0, 0, cxScreen, cyScreen, hdcWindow, 0, 0, SRCCOPY);
    }
    else
    {
        hdc = hdcWindow;
    }
    

    if (m_isVScroll)
    {
        RECT rc1 = { 0, 0, rcScroll.right - rcScroll.left, rcScroll.bottom - rcScroll.top };
        md_resize(&m_vmd, rc1.right, rc1.bottom);
        FillRect(m_vmd.hdc, &rc1, (HBRUSH)GetStockObject(WHITE_BRUSH));
        DrawVScroll(m_vmd.hdc, rc1.right, rc1.bottom, rcScroll);
        BitBlt(hdc, rcScrollClient.left, rcScrollClient.top,
            rcScrollClient.right - rcScrollClient.left, rcScrollClient.bottom - rcScrollClient.top,
            m_vmd.hdc, 0, 0, SRCCOPY);
        if (m_pfn_DrawScroll)
        {
            RECT pt = { rc.right - rc1.right, rc.top, rc.right, rc.bottom };
            ScreenToClient(m_hWnd, (POINT*)&pt);
            ScreenToClient(m_hWnd, ((POINT*)&pt) + 1);
            RECT_F rcF(pt);
            pfn_DrawScroll(m_pfn_DrawScroll, this, true, hdc, 0, ISCROLL_DRAWTYPE_ALERT, &rcF);
        }
        //BitBlt(hdc, cxScreen - rcBorder.right, rcBorder.top, rc1.right, rc1.bottom, m_vmd.hdc, 0, 0, SRCCOPY);
    }

    if (m_isHScroll)
    {
        GetHScrollRect(&rcScroll, &rcScrollClient);
        RECT rc2 = { 0, 0, rcScroll.right - rcScroll.left, rcScroll.bottom - rcScroll.top };
        md_resize(&m_hmd, rc2.right, rc2.bottom);
        FillRect(m_hmd.hdc, &rc2, (HBRUSH)GetStockObject(WHITE_BRUSH));
        DrawHScroll(m_hmd.hdc, rc2.right, rc2.bottom, rcScroll);

        BitBlt(hdc, rcScrollClient.left, rcScrollClient.top,
            rcScrollClient.right - rcScrollClient.left, rcScrollClient.bottom - rcScrollClient.top,
            m_hmd.hdc, 0, 0, SRCCOPY);

        if (m_pfn_DrawScroll)
        {
            RECT pt = { rc.left, rc.bottom - rc2.bottom, rc.right, rc.bottom };
            ScreenToClient(m_hWnd, (POINT*)&pt);
            ScreenToClient(m_hWnd, ((POINT*)&pt) + 1);
            RECT_F rcF(pt);
            pfn_DrawScroll(m_pfn_DrawScroll, this, false, hdc, 0, ISCROLL_DRAWTYPE_ALERT, &rcF);
        }
        //BitBlt(hdc, rcBorder.left, cyScreen - rc2.bottom, rc2.right, rc2.bottom, m_hmd.hdc, 0, 0, SRCCOPY);
    }

    if (0)
    {
        RECT rcBorder, rcClient, rcWnd;
        WndBase_GetRect(m_hWnd, &rcBorder, &rcWnd, 0, &rcClient);

        const int cxClient = rcClient.right - rcClient.left;
        const int cyClient = rcClient.bottom - rcClient.top;
        const int cxScreen = rcWnd.right - rcWnd.left;
        const int cyScreen = rcWnd.bottom - rcWnd.top;

        rcClient.left = rcClient.left - rcWnd.left;
        rcClient.top = rcClient.top - rcWnd.top;
        rcClient.right = rcClient.left + cxClient;
        rcClient.bottom = rcClient.top + cyClient;

        rcWnd = { 0, 0, cxScreen, cyScreen };
        RECT rcRgn2;
        m_vrc.blockHeight;
        if (m_isVScroll && m_isHScroll)
            rcRgn2 = { rcClient.left, rcClient.top, rcClient.right + (int)m_vInfo.btnThumbSize, rcClient.bottom + (int)m_hInfo.btnThumbSize };
        else if (m_isVScroll)
            rcRgn2 = { rcClient.left, rcClient.top, rcClient.right + (int)m_vInfo.btnThumbSize, rcClient.bottom };
        else if (m_isHScroll)
            rcRgn2 = { rcClient.left, rcClient.top, rcClient.right, rcClient.bottom + (int)m_hInfo.btnThumbSize };

        HRGN hRgn1 = CreateRectRgn(0, 0, cxScreen, cyScreen);

        HRGN hRgn2 = CreateRectRgn(rcRgn2.left, rcRgn2.top, rcRgn2.right, rcRgn2.bottom);
        HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
        int ret = CombineRgn(hRgn, hRgn1, hRgn2, RGN_DIFF);

        SelectClipRgn(hdc, hRgn);

        HBRUSH hbr = CreateSolidBrush(255);
        FillRect(hdc, &rcWnd, hbr);
        DeleteObject(hbr);

        DeleteObject(hRgn1);
        DeleteObject(hRgn2);
        DeleteObject(hRgn);

        SelectClipRgn(hdc, 0);
    }

    if (m_nBorderWidth)     // 有边框就画边框
    {
        RECT_F rcF(0, 0, (float)cxScreen, (float)cyScreen);
        if (!pfn_DrawScroll(m_pfn_DrawScroll, this, false, hdc, 0, ISCROLL_DRAWTYPE_BORDER, &rcF))
        {
            HGDIOBJ hOldPen = SelectObject(hdc, m_hPen);
            HGDIOBJ hOldBrush = SelectObject(hdc, GetStockBrush(NULL_BRUSH));

            Rectangle(hdc, 0, 0, cxScreen, cyScreen);
            SelectObject(hdc, hOldBrush);
            SelectObject(hdc, hOldPen);
        }

    }

    if (isMemoryDC)
        BitBlt(hdcWindow, 0, 0, cxScreen, cyScreen, hdc, 0, 0, SRCCOPY);
    return true;
}

bool IDrawScroll::OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (!m_isControl)return false;

    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    OnDrawScroll(ps.hdc);
    EndPaint(hWnd, &ps);
    return true;
}

bool IDrawScroll::OnPrint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    IDrawScroll* pObj = (IDrawScroll*)GetPropW(hWnd, SCROLL_THISCLASS);
    if (!pObj)
        return false;
    OnDrawScroll((HDC)wParam);
    return false;
}

bool IDrawScroll::OnNcPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    IDrawScroll* pObj = (IDrawScroll*)GetPropW(hWnd, SCROLL_THISCLASS);
    if (!pObj)
        return false;
    HDC hdcWindow = GetWindowDC(hWnd);
    bool ret = OnDrawScroll(hdcWindow);
    ReleaseDC(hWnd, hdcWindow);
    return ret;
}

// 鼠标按下时需要连续滚动的时钟, 只要在两个按钮上才触发这个时钟, 鼠标放开时销毁时钟
void CALLBACK TimerProc_NcLButtonDown(HWND hWnd, UINT message, UINT_PTR id, DWORD dwTimer)
{
    if ((ULONG_PTR)hWnd == id)
    {
        KillTimer(hWnd, id);
        SetTimer(hWnd, (UINT_PTR)10086, 120, TimerProc_NcLButtonDown);
        return;
    }

    IDrawScroll* pObj = IDrawScroll::GetobjFromHwnd(hWnd);
    if (!pObj)
    {
        KillTimer(hWnd, id);
        return;
    }

    int index = pObj->GetIndex(true);
    LPARAM lParam = pObj->m_isControl ? (LPARAM)hWnd : 0;
    if (index == INDEX_BUTTON1)         // 鼠标在纵向滚动条按钮1上
    {
        SendMessageW(hWnd, WM_VSCROLL, SB_LINEUP, lParam);
        return;
    }
    else if (index == INDEX_BUTTON2)    // 鼠标在纵向滚动条按钮2上
    {
        SendMessageW(hWnd, WM_VSCROLL, SB_LINEDOWN, lParam);
        return;
    }
    else if (index == INDEX_OUTER)      // 鼠标不在滑块/按钮上, 按页滚动
    {
        SendMessageW(hWnd, WM_VSCROLL, pObj->m_PageDown, lParam);
        return;
    }

    index = pObj->GetIndex(false);
    if (index == INDEX_BUTTON1)         // 鼠标在横向滚动条按钮1上
    {
        SendMessageW(hWnd, WM_HSCROLL, SB_LINEUP, lParam);
        return;
    }
    else if (index == INDEX_BUTTON2)    // 鼠标在横向滚动条按钮2上
    {
        SendMessageW(hWnd, WM_HSCROLL, SB_LINEDOWN, lParam);
        return;
    }
    else if (index == INDEX_OUTER)      // 鼠标不在滑块/按钮上, 按页滚动
    {
        SendMessageW(hWnd, WM_HSCROLL, pObj->m_PageDown, lParam);
        return;
    }
    else
    {
        // 鼠标没有在纵向/横向的按钮1/按钮2上, 销毁时钟
        KillTimer(hWnd, id);
    }
}


bool IDrawScroll::OnNcLbuttonDown(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (wParam != HTVSCROLL && wParam != HTHSCROLL) return false;
    const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    const int timerTick = 700;
    int index = 0;
    if (wParam == HTVSCROLL)
    {
        if (!PtInRect(&m_vrc.rc, pt))
            return false;
        // 纵向滚动条
        if (PtInRect(&m_vrc.thumb, pt))         // 在滑块里, 需要捕获鼠标
        {
            wParam = -1;
            index = INDEX_THUMB;
            m_downOffset.y = pt.y - m_vrc.thumb.top;
            m_downPos = m_vScroll.nPos;
            m_downPoint = pt;
            m_thumbPoint.y = pt.y;
        }
        else if (PtInRect(&m_vrc.btn1, pt))     // 在按钮1上, 需要向上滚动
        {
            wParam = SB_LINEUP;
            index = INDEX_BUTTON1;
        }
        else if (PtInRect(&m_vrc.btn2, pt))     // 在按钮2上, 需要向下滚动
        {
            wParam = SB_LINEDOWN;
            index = INDEX_BUTTON2;
        }
        else
        {
            // 不是在按钮1,按钮2,滑块里, 那就是需要翻一页, 需要判断按下位置是滑块的前面还是后面
            wParam = (pt.y < m_vrc.thumb.top) ? SB_PAGEUP : SB_PAGEDOWN; // 在滑块的上面, 向上滚动一页
            m_PageDown = (int)wParam;
            index = INDEX_OUTER;
        }
        m_lDownIsVscroll = true;
        m_isCapture = CAPTURE_VSCROLL;
    }
    else
    {
        if (!PtInRect(&m_hrc.rc, pt))
            return false;
        // 横向滚动条
        if (PtInRect(&m_hrc.thumb, pt))
        {
            wParam = -1;
            index = INDEX_THUMB;
            m_downOffset.x = pt.x - m_hrc.thumb.left;
            m_downPos = m_hScroll.nPos;
            m_downPoint = pt;
            m_thumbPoint.x = pt.x;
        }
        else if (PtInRect(&m_hrc.btn1, pt))
        {
            wParam = SB_LINEUP;
            index = INDEX_BUTTON1;
        }
        else if (PtInRect(&m_hrc.btn2, pt))
        {
            wParam = SB_LINEDOWN;
            index = INDEX_BUTTON2;
        }
        else
        {
            // 不是在按钮1,按钮2,滑块里, 那就是需要翻一页, 需要判断按下位置是滑块的左边还是右边
            wParam = (pt.x < m_hrc.thumb.left) ? SB_PAGEUP : SB_PAGEDOWN; // 在滑块的左边, 向左滚动一页
            m_PageDown = (int)wParam;
            index = INDEX_OUTER;
        }
        m_lDownIsVscroll = false;
        m_isCapture = CAPTURE_HSCROLL;
    }

    SetState(STATE::DOWN, m_lDownIsVscroll);
    SetIndex(index, m_lDownIsVscroll);

    InvalidateRect();
    if (wParam != -1)
    {
        SetTimer(hWnd, (UINT_PTR)m_hWnd, timerTick, TimerProc_NcLButtonDown);
        SendMessageW(hWnd, m_lDownIsVscroll ? WM_VSCROLL : WM_HSCROLL, wParam, m_isControl ? (LPARAM)m_hWnd : 0);
    }
    if (GetCapture() != hWnd)
        SetCapture(hWnd);
    return true;
}

bool IDrawScroll::OnCaptureChanged(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (!m_isCapture) return false;
    //wstr::dbg(L"鼠标释放\n");
    KillTimer(m_hWnd, (UINT_PTR)m_hWnd);
    KillTimer(m_hWnd, (UINT_PTR)10086);

    if (m_lDownIsVscroll)
    {
        // 纵向滚动条
        SendMessageW(hWnd, WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, m_vScroll.nPos), m_isControl ? (LPARAM)m_hWnd : 0);
        const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int index = GetVHittestPos(pt);
        SetIndex(index, true);
        SetState((index != INDEX_NONE) ? STATE::HOT : STATE::NORMAL, true);
    }
    else
    {
        // 横向滚动条
        SendMessageW(hWnd, WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, m_hScroll.nPos), m_isControl ? (LPARAM)m_hWnd : 0);
        const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int index = GetVHittestPos(pt);
        SetIndex(index, false);
        SetState((index != INDEX_NONE) ? STATE::HOT : STATE::NORMAL, false);
    }
    m_isCapture = CAPTURE_NONE;
    m_downOffset = { 0, 0 };
    //TODO 应该需要在鼠标拖动的时候按像素滚动, 然后在这里把最后的滑块位置计算好并重画
    InvalidateRect();
    return true;
}

bool IDrawScroll::OnNcLbuttonUp(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (m_isCapture == CAPTURE_BUTTON)
    {
        OnCaptureChanged(oldProc, hWnd, message, wParam, (LPARAM)m_hWnd, pRet);
        m_isCapture = false;
        return true;
    }
    if (m_isCapture)
        ReleaseCapture();
    m_isCapture = false;
    return true;
}
const int __temp_timer = 1000;
static IDrawScroll* __temp_obj;
static void CALLBACK __temp_timerProc_hscroll(HWND hWnd, UINT message, UINT_PTR id, DWORD dwTimer)
{
    if (!__temp_obj)return;
    if (__temp_obj == 0)  // 下断点进来关闭
    {
        KillTimer(hWnd, id);
    }
    IDrawScroll* pObj = __temp_obj;
    SendMessageA(hWnd, WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, pObj->m_hScroll.nPos), pObj->m_isControl ? (LPARAM)hWnd : 0);
}

static void CALLBACK __temp_timerProc_vscroll(HWND hWnd, UINT message, UINT_PTR id, DWORD dwTimer)
{
    if (!__temp_obj)return;
    if (__temp_obj == 0)  // 下断点进来关闭
    {
        KillTimer(hWnd, id);
    }
    IDrawScroll* pObj = __temp_obj;
    //SendMessageA(hWnd, WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, pObj->m_vScroll.nPos), pObj->m_isControl ? (LPARAM)pObj->m_hWnd : 0);
    PostMessageA(hWnd, WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, pObj->m_vScroll.nPos), pObj->m_isControl ? (LPARAM)pObj->m_hWnd : 0);
    static HWND hasadasd;
    if (hasadasd) SendMessageA(hasadasd, WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, pObj->m_vScroll.nPos), pObj->m_isControl ? (LPARAM)pObj->m_hWnd : 0);

}

// 鼠标按下滑块后移动鼠标, 没有按下滑块的话不会触发
bool IDrawScroll::OnThumbMove(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (!m_isVScroll && !m_isHScroll)return true;
    POINT pt;
    GetCursorPos(&pt);
    int index = GetIndex(true);
    if (index == INDEX_THUMB)    // 鼠标在纵向滚动条中
    {
        // 顶边位置 = 当前位置 / 最大位置 * 滚动条高度
        // 当前位置 = 顶边位置 / 滚动条高度 * 最大位置
        m_thumbPoint.y = (int)pt.y;

        const float y = (float)(pt.y - m_vrc.rc.top) - m_vInfo.btn1Size - (float)m_downOffset.y;
        float newPos = y / (m_vrc.maxBlockHeight - m_vrc.blockHeight) * (float)m_vrc.maxPos;

        const float newPosOffset = newPos - (float)((int)newPos);
        if (newPosOffset > 0.5) newPos++;

        //wstr::dbg(L"鼠标位置 = %d,%f, 计算出的新位置 = %f, 原位置 = %d\n", pt.x, y, newPos, m_vScroll.nPos);
        if (newPos < 0)newPos = 0;
        else if (newPos > m_vrc.maxPos)newPos = (float)m_vrc.maxPos;
        //else if (newPos > m_vrc.maxPos)newPos = (float)m_vScroll.nMax;
        const int offsetPos = (int)newPos - m_vScroll.nPos;
        //wstr::dbg(L"鼠标位置 = %d,%f, 计算出的新位置 = %f, 原位置 = %d\n", pt.x, y, newPos, m_vScroll.nPos);
        //if ((int)newPos == m_vScroll.nPos)
        //    return false;
        m_vScroll.nPos = (int)newPos;
        

        
        if (m_isSysListView)
        {
            if (m_vScroll.nPos == 0)
            {
                SendMessageW(hWnd, WM_VSCROLL, SB_TOP, 0);
            }
            else if (m_vScroll.nPos == m_vrc.maxPos)
            {
                SendMessageW(hWnd, WM_VSCROLL, SB_BOTTOM, 0);
            }
            else
            {
                const int caonima = offsetPos * m_isSysListView;
                if (caonima)
                    SendMessageW(hWnd, LVM_SCROLL, 0, caonima);
            }
            SetScrollPos(hWnd, SB_VERT, m_vScroll.nPos, true);
            return true;
        }

        if (m_isSysTreeView)
        {
            if (m_isCallSetScroll)
                SetScrollPos(hWnd, SB_VERT, m_vScroll.nPos, true);
            SendMessageW(hWnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, m_vScroll.nPos), m_isControl ? (LPARAM)m_hWnd : 0);
        }
        else
        {
            SendMessageW(hWnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, m_vScroll.nPos), m_isControl ? (LPARAM)m_hWnd : 0);
            if (m_isCallSetScroll)
                SetScrollPos(hWnd, SB_VERT, m_vScroll.nPos, true);
        }

        return true;
    }

    index = GetIndex(false);
    if (index == INDEX_THUMB)
    {
        // 顶边位置 = 当前位置 / 最大位置 * 滚动条高度
        // 当前位置 = 顶边位置 / 滚动条高度 * 最大位置
        m_thumbPoint.x = (int)pt.x;
        const float x = (float)(pt.x - m_hrc.rc.left) - m_hInfo.btn1Size - (float)m_downOffset.x;
        float newPos = x / (m_hrc.maxBlockHeight - m_hrc.blockHeight) * (float)m_hrc.maxPos;
        const float newPosOffset = newPos - (float)((int)newPos);
        if (newPosOffset > 0.5) newPos++;
        //wstr::dbg(L"鼠标位置 = %f,%d, 计算出的新位置 = %f\n", x, pt.y, newPos);
        if (newPos < 0)newPos = 0;
        else if (newPos > m_hrc.maxPos)newPos = m_hrc.maxPos;
        const int offsetPos = (int)newPos - m_hScroll.nPos;
        //wstr::dbg(L"鼠标位置 = %d,%f, 计算出的新位置 = %f, 原位置 = %d\n", x, pt.y, newPos, m_hScroll.nPos);
        //if ((int)newPos == m_hScroll.nPos)
        //    return false;
        m_hScroll.nPos = (int)newPos;

        if (m_isSysListView)
        {
            if (offsetPos == -1 && m_hScroll.nPos == 0)
            {
                SendMessageW(hWnd, WM_HSCROLL, SB_LEFT, 0);
            }
            else if (m_hScroll.nPos == m_hrc.maxPos)
            {
                SendMessageW(hWnd, WM_HSCROLL, SB_RIGHT, 0);
            }
            else
            {
                //const int caonima = offsetPos * m_isSysListView;
                const int caonima = offsetPos;
                if (caonima)
                    SendMessageW(hWnd, LVM_SCROLL, caonima, 0);
            }
            return true;
        }

        if (m_isSysTreeView)
        {
            if (m_isCallSetScroll)
                SetScrollPos(hWnd, SB_HORZ, m_hScroll.nPos, true);
            SendMessageW(hWnd, WM_HSCROLL, MAKELONG(SB_THUMBTRACK, m_hScroll.nPos), m_isControl ? (LPARAM)m_hWnd : 0);
        }
        else
        {
            SendMessageW(hWnd, WM_HSCROLL, MAKELONG(SB_THUMBTRACK, m_hScroll.nPos), m_isControl ? (LPARAM)m_hWnd : 0);
            if (m_isCallSetScroll)
                SetScrollPos(hWnd, SB_HORZ, m_hScroll.nPos, true);
        }

        return true;
    }
    return false;
}

bool IDrawScroll::OnNcLbuttonDblclk(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    return OnNcLbuttonDown(oldProc, hWnd, message, wParam, lParam, pRet);
    if (wParam != HTVSCROLL && wParam != HTHSCROLL) return false;
    if (wParam == HTVSCROLL)
    {
        // 纵向滚动条
    }
    else
    {
        // 横向滚动条
    }
    return true;
}

bool IDrawScroll::OnNcRbuttonDown(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (wParam != HTVSCROLL && wParam != HTHSCROLL) return false;
    if (wParam == HTVSCROLL)
    {
        // 纵向滚动条
    }
    else
    {
        // 横向滚动条
    }
    return true;
}

bool IDrawScroll::OnNcRbuttonUp(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (wParam != HTVSCROLL && wParam != HTHSCROLL) return false;
    if (wParam == HTVSCROLL)
    {
        // 纵向滚动条
    }
    else
    {
        // 横向滚动条
    }
    return true;
}

bool IDrawScroll::OnNcRbuttonDblclk(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    if (wParam != HTVSCROLL && wParam != HTHSCROLL) return false;
    if (wParam == HTVSCROLL)
    {
        // 纵向滚动条
    }
    else
    {
        // 横向滚动条
    }
    return true;
}

inline void __OnScroll(HWND hWnd, int nScrollCode, SCROLLINFO& si)
{
    int LineNum = 1;
    
    int nNewPos = si.nPos;
    switch (nScrollCode)
    {
    case SB_LINEUP:
        nNewPos -= LineNum;
        break;
    case SB_LINEDOWN:
        nNewPos += LineNum;
        break;
    case SB_PAGEUP:
        nNewPos -= si.nPage;
        break;
    case SB_PAGEDOWN:
        nNewPos += si.nPage;
        break;
    case SB_THUMBTRACK:
        nNewPos = si.nTrackPos;
        break;
    case SB_TOP:
        nNewPos = si.nMin;
        break;
    case SB_BOTTOM:
        nNewPos = si.nMax;
        break;
    case SB_ENDSCROLL:
        nNewPos = si.nPos;
        break;
    default:
        break;
    }
    si.nPos = nNewPos;
    SendMessageW(hWnd, SBM_SETPOS, si.nPos, 0);
    //InvalidateRect(hWnd, 0, 0);
}

bool IDrawScroll::OnHScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    //if (!m_isControl || (LPARAM)hWnd != lParam) return false;
    *pRet = CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    m_hScroll.cbSize = sizeof(m_hScroll);
    m_hScroll.fMask = SIF_ALL;
    bool isControl = IDrawScroll::IsBindControl(m_hWndHBind, 0);
    if (isControl)
        GetScrollInfo(m_hWndHBind, SB_CTL, &m_hScroll);
    else if (m_isControl)
        SendMessageW(hWnd, SBM_GETSCROLLINFO, SB_CTL, (LPARAM)&m_hScroll);
    else
        GetScrollInfo(hWnd, SB_HORZ, &m_hScroll);

    //__OnScroll((HWND)lParam, LOWORD(wParam), m_hScroll);
    InvalidateRect();
    return true;
}

bool IDrawScroll::OnVScroll(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pRet)
{
    //if (!m_isControl || (LPARAM)hWnd != lParam) return false;
    *pRet = CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    m_vScroll.cbSize = sizeof(m_vScroll);
    m_vScroll.fMask = SIF_ALL;
    bool isControl = IDrawScroll::IsBindControl(m_hWndVBind, 0);
    if (isControl)
        GetScrollInfo(m_hWndVBind, SB_CTL, &m_vScroll);
    else if (m_isControl)
        SendMessageW(hWnd, SBM_GETSCROLLINFO, SB_CTL, (LPARAM)&m_vScroll);
    else
        GetScrollInfo(hWnd, SB_VERT, &m_vScroll);

    //__OnScroll((HWND)lParam, LOWORD(wParam), m_vScroll);
    //wstr::dbg(L"纵向被滚动, 当前位置 = %d\n", m_vScroll.nPos);
    InvalidateRect();
    return true;
}



void IDrawScroll::DrawVScroll(HDC hdc, int cxClient, int cyClient, const RECT& rcScroll)
{
    LPSCROLL_RECT_INFO rcInfo = &m_vInfo;

    Gdiplus::SolidBrush* hbrBtn1 = 0, * hbrBtn2 = 0, * hbrThumb = 0;
    RECT_F* rcThumb = 0, * rcBtn1 = 0, * rcBtn2 = 0;
    bool isFillBack = false;

    const bool isGdip = m_VBmp == 0;
    rcThumb = &rcInfo->thumbNormal;
    rcBtn1 = &rcInfo->btnNormal1;
    rcBtn2 = &rcInfo->btnNormal2;

    const int index = GetIndex(true);
    if (index == INDEX_BUTTON1)
    {
        if (IsState(STATE::DOWN, true))     { if (isGdip) { hbrBtn1 = new Gdiplus::SolidBrush(rcInfo->crBtnDown1); } rcBtn1 = &rcInfo->btnDown1; }
        else if (IsState(STATE::HOT, true)) { if (isGdip) { hbrBtn1 = new Gdiplus::SolidBrush(rcInfo->crBtnHot1); }  rcBtn1 = &rcInfo->btnHover1; }
    }
    else if (index == INDEX_BUTTON2)
    {
        if (IsState(STATE::DOWN, true)) { if (isGdip) { hbrBtn2 = new Gdiplus::SolidBrush(rcInfo->crBtnDown2); } rcBtn2 = &rcInfo->btnDown2; }
        else if (IsState(STATE::HOT, true)) { if (isGdip) { hbrBtn2 = new Gdiplus::SolidBrush(rcInfo->crBtnHot2); }rcBtn2 = &rcInfo->btnHover2; }
    }
    else if (index == INDEX_THUMB)
    {
        if (IsState(STATE::DOWN, true)) { if (isGdip) { hbrThumb = new Gdiplus::SolidBrush(rcInfo->crThumbDown); } rcThumb = &rcInfo->thumbDown; isFillBack = true; }
        else if (IsState(STATE::HOT, true)) { if (isGdip) { hbrThumb = new Gdiplus::SolidBrush(rcInfo->crThumbHot); }  rcThumb = &rcInfo->thumbHover; isFillBack = true; }
    }
    else if (index == INDEX_OUTER)
    {
        isFillBack = true;
    }

    float btn1Size, btn2Size, btnMinSize, btnThumbSize;
    if (isGdip)
    {
        if (hbrBtn1 == 0)  hbrBtn1  = new Gdiplus::SolidBrush(rcInfo->crBtn1);
        if (hbrBtn2 == 0)  hbrBtn2  = new Gdiplus::SolidBrush(rcInfo->crBtn2);
        if (hbrThumb == 0) hbrThumb = new Gdiplus::SolidBrush(rcInfo->crThumb);
    }

    btn1Size = rcInfo->btn1Size;
    btn2Size = rcInfo->btn2Size;
    btnMinSize = rcInfo->btnMinSize;
    btnThumbSize = rcInfo->btnThumbSize;

    const float page = (float)(m_vScroll.nPage ? m_vScroll.nPage - 1 : 1);
    m_vrc.maxPos = (float)(m_vScroll.nMax - m_vScroll.nPage + 1);
    const float buttonSize = btn1Size + btn2Size;                           // 上下两个按钮的高度
    m_vrc.maxBlockHeight = (float)cyClient - buttonSize;                    // 滑块总的高度, 滑块条高度 - 上下两个按钮的高度
    float pageCount = m_vScroll.nMax / page;                                // 滚动的页数
    m_vrc.blockHeight = m_vrc.maxBlockHeight / pageCount;                   // 滑块的高度
    if (m_vrc.blockHeight < btnMinSize)
        m_vrc.blockHeight = btnMinSize;
    
    if (m_vScroll.nPage == 0)
    {
        pageCount = 2;
        m_vrc.blockHeight = 100;
        m_vrc.maxPos--;
    }

    //const float moveSize = m_vrc.maxBlockHeight / m_vScroll.nMax;           // 滑动一次的像素
    //const float maxHeight = cyClient * pageCount;                           // 显示所有内容需要的高度
    

    if (pageCount <= 1.f || m_vrc.maxPos < 0)
    {
        rcThumb = &rcInfo->thumbNormal;
        rcBtn1 = &rcInfo->btnNormal1;
        rcBtn2 = &rcInfo->btnNormal2;
        isFillBack = false;
    }

    Gdiplus::Graphics gp(hdc);

    RECT_F rcDrawBtn1(0, 0, btnThumbSize, btn1Size);
    RECT_F rcDrawThumb(0, btn1Size, btnThumbSize, (float)cyClient - btn1Size);  // 中间的滚动条背景
    RECT_F rcDrawBtn2(0, (float)cyClient - btn1Size, btnThumbSize, (float)cyClient);        // 底边的按钮
    bool isNoDraw = false;
    if (isGdip)
    {
        RECT_F rcback(rcDrawBtn1.left, rcDrawBtn1.top, rcDrawThumb.right, rcDrawBtn2.bottom);
        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, true, hdc, &gp, ISCROLL_DRAWTYPE_BACK, &rcback);
        if (!isNoDraw)
        {
            Gdiplus::SolidBrush hbrBack(rcInfo->crBack);
            gp.FillRectangle(&hbrBack, rcDrawBtn1.left, rcDrawBtn1.top,
                rcDrawThumb.right - rcDrawThumb.left, rcDrawBtn2.bottom - rcDrawBtn1.top);
        }
    }

    if (isGdip)   // 没有图片, 使用默认色绘画
    {
        Gdiplus::GraphicsPath path, path2;
        const float btnOffset = 3;
        Gdiplus::PointF pt[3];
        Gdiplus::PointF pt2[3];

        pt[0].X = btnThumbSize / 2;                 // 顶边x, 横向位置是中间
        pt[0].Y = rcDrawBtn1.top + btnOffset;       // 顶边y, 纵向是顶边+偏移

        pt[1].X = rcDrawBtn1.left + btnOffset-1;      // 左下角x, 左边+偏移
        pt[1].Y = rcDrawBtn1.bottom - btnOffset+1;    // 左下角y, 底边 - 偏移

        pt[2].X = rcDrawBtn1.right - btnOffset+1;     // 右下角x, 右边 - 偏移
        pt[2].Y = rcDrawBtn1.bottom - btnOffset+1;    // 右下角y, 底边 - 偏移



        pt2[0].X = rcDrawBtn2.left + btnOffset;      // 左上角x, 左边 + 偏移
        pt2[0].Y = rcDrawBtn2.top + btnOffset;       // 左上角y, 顶边 + 偏移

        pt2[1].X = rcDrawBtn2.right - btnOffset+1;    // 右上角x, 右边 - 偏移
        pt2[1].Y = rcDrawBtn2.top + btnOffset;      // 右上角y, 顶边 + 偏移

        pt2[2].X = btnThumbSize / 2;                // 底边x, 中间位置
        pt2[2].Y = rcDrawBtn2.bottom - btnOffset;   // 底边y, 底边 - 偏移

        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, true, hdc, &gp, ISCROLL_DRAWTYPE_BTN1, &rcDrawBtn1);
        if (!isNoDraw && m_vInfo.btn1Size > 0)
            gp.FillPolygon(hbrBtn1, pt, 3);

        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, true, hdc, &gp, ISCROLL_DRAWTYPE_BTN2, &rcDrawBtn2);
        if (!isNoDraw && m_vInfo.btn2Size > 0)
            gp.FillPolygon(hbrBtn2, pt2, 3);
    }
    else
    {
        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, true, hdc, &gp, ISCROLL_DRAWTYPE_BTN1, &rcDrawBtn1);
        if (!isNoDraw && m_vInfo.btn1Size > 0)
            DrawImageFromRect(&gp, m_VBmp, rcDrawBtn1, *rcBtn1);    // 绘画顶边按钮

        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, true, hdc, &gp, ISCROLL_DRAWTYPE_BTN2, &rcDrawBtn2);
        if (!isNoDraw && m_vInfo.btn2Size > 0)
            DrawImageFromRect(&gp, m_VBmp, rcDrawBtn2, *rcBtn2);    // 绘画底边按钮
    }

    

    if (m_vScroll.nMax < (int)m_vScroll.nPage)
    {
        delete hbrThumb;
        delete hbrBtn1;
        delete hbrBtn2;
        return;
    }

    if (isFillBack && !isGdip)
        DrawImageFromGrid(&gp, m_VBmp, rcDrawThumb, rcInfo->back, rcInfo->backGrid, 255);

    memset(&m_vrc.thumb, 0, sizeof(RECT));
    m_vrc.btn1.left     = rcScroll.left;
    m_vrc.btn1.top      = rcScroll.top + (long)rcDrawBtn1.top;
    m_vrc.btn1.right    = m_vrc.btn1.left + (long)(rcDrawBtn1.right - rcDrawBtn1.left);
    m_vrc.btn1.bottom   = m_vrc.btn1.top + (long)(rcDrawBtn1.bottom - rcDrawBtn1.top);

    m_vrc.btn2.left     = rcScroll.left;
    m_vrc.btn2.top      = rcScroll.top + (long)rcDrawBtn2.top;
    m_vrc.btn2.right    = m_vrc.btn2.left + (long)(rcDrawBtn2.right - rcDrawBtn2.left);
    m_vrc.btn2.bottom   = m_vrc.btn2.top + (long)(rcDrawBtn2.bottom - rcDrawBtn2.top);


    if (pageCount >= 1.f)
    {
        // 总行数/一页的行数=滚的页数
        //总的滑块显示高度/滚的页数=块高
        //总的滑块显示高度-块高=可滑动的距离
        //总的滑块显示高度/总行数=每次滑动距离

        //const float 每次滚动的像素 = (总画面高度 - cyClient) / (m_vrc.maxPos?m_vrc.maxPos:1);
        //const float 总要滚动的次数 = cyClient / 每次滚动的像素;
        // 总要滚动的次数 = 滚动条高度 / 每次滚动的像素
        // 块高度 = 总要滚动的次数 / 滚动条高度
        // 顶边位置 = 当前位置 / 最大位置 * 滚动条高度
        // 当前位置 = 顶边位置 / 滚动条高度 * 最大位置


        float currentPos = 0;
        if (isGdip)
            currentPos = (float)(m_vScroll.nPos / m_vrc.maxPos * (float)(m_vrc.maxBlockHeight - m_vrc.blockHeight));
        else
            currentPos = (float)((int)(m_vScroll.nPos / m_vrc.maxPos * (float)(m_vrc.maxBlockHeight - m_vrc.blockHeight)));

        // rcDrawThumb.top 决定了滑块画出的位置
        // 需要在按下的时候记录偏移位置, 如果不是鼠标按下状态就可以直接用 currentPos 作为位置
        if (m_downOffset.y > 0)
        {
            int y = (int)m_thumbPoint.y - rcScroll.top - m_downOffset.y;
            int min = (int)(m_vrc.btn1.bottom - m_vrc.btn1.top);
            int max = (int)(m_vrc.maxBlockHeight - m_vrc.blockHeight + min);
            if (y < min)        y = min;
            else if (y > max)   y = max;

            rcDrawThumb.top = (float)(y);
            //wstr::dbg(L"按下偏移 = %d, x,y坐标 = %d,%d, 当前位置 = %f, 当前位置和按下位置的差 = %d\n", m_downOffset.y, m_thumbPoint, rcDrawThumb.top, 0);
        }
        else
        {
            rcDrawThumb.top += (float)((int)currentPos);
        }

        if (isGdip)
            rcDrawThumb.bottom = (float)(rcDrawThumb.top + m_vrc.blockHeight);
        else
            rcDrawThumb.bottom = (float)((int)(rcDrawThumb.top + m_vrc.blockHeight));

        if (rcDrawThumb.bottom > (float)cyClient - btn1Size)
            rcDrawThumb.bottom = (float)cyClient - btn1Size;

        m_vrc.thumb.left = (long)rcScroll.left;
        m_vrc.thumb.top = (long)rcScroll.top + (long)rcDrawThumb.top;
        m_vrc.thumb.right = m_vrc.thumb.left + (long)(rcDrawThumb.right - rcDrawThumb.left);
        m_vrc.thumb.bottom = m_vrc.thumb.top + (long)(rcDrawThumb.bottom - rcDrawThumb.top);


        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, true, hdc, &gp, ISCROLL_DRAWTYPE_THUMB, &rcDrawThumb);
        if (!isNoDraw)
        {
            if (isGdip)
            {
                gp.FillRectangle(hbrThumb, rcDrawThumb.left, rcDrawThumb.top,
                    rcDrawThumb.right - rcDrawThumb.left, rcDrawThumb.bottom - rcDrawThumb.top);
            }
            else
            {
                DrawImageFromGrid(&gp, m_VBmp, rcDrawThumb, *rcThumb, rcInfo->backGrid);    // 滚动条
            }
        }
    }
    delete hbrThumb;
    delete hbrBtn1;
    delete hbrBtn2;
    return;
}

void IDrawScroll::DrawHScroll(HDC hdc, int cxClient, int cyClient, const RECT& rcScroll)
{
    LPSCROLL_RECT_INFO rcInfo = &m_hInfo;
    Gdiplus::SolidBrush* hbrBtn1 = 0, * hbrBtn2 = 0, * hbrThumb = 0;
    RECT_F* rcThumb, * rcBtn1, * rcBtn2;
    bool isFillBack = false;

    const bool isGdip = m_HBmp == 0;
    rcThumb = &rcInfo->thumbNormal;
    rcBtn1 = &rcInfo->btnNormal1;
    rcBtn2 = &rcInfo->btnNormal2;
    const int index = GetIndex(false);
    if (index == INDEX_BUTTON1)
    {
        if (IsState(STATE::DOWN, false))     { if (isGdip) { hbrBtn1 = new Gdiplus::SolidBrush(rcInfo->crBtnDown1); } rcBtn1 = &rcInfo->btnDown1; }
        else if (IsState(STATE::HOT, false)) { if (isGdip) { hbrBtn1 = new Gdiplus::SolidBrush(rcInfo->crBtnHot1); }  rcBtn1 = &rcInfo->btnHover1; }
    }
    else if (index == INDEX_BUTTON2)
    {
        if (IsState(STATE::DOWN, false))     { if (isGdip) { hbrBtn2 = new Gdiplus::SolidBrush(rcInfo->crBtnDown2); } rcBtn2 = &rcInfo->btnDown2; }
        else if (IsState(STATE::HOT, false)) { if (isGdip) { hbrBtn2 = new Gdiplus::SolidBrush(rcInfo->crBtnHot2); }  rcBtn2 = &rcInfo->btnHover2; }
    }
    else if (index == INDEX_THUMB)
    {
        if (IsState(STATE::DOWN, false))     { if (isGdip) { hbrThumb = new Gdiplus::SolidBrush(rcInfo->crThumbDown); } rcThumb = &rcInfo->thumbDown; isFillBack = true; }
        else if (IsState(STATE::HOT, false)) { if (isGdip) { hbrThumb = new Gdiplus::SolidBrush(rcInfo->crThumbHot); }  rcThumb = &rcInfo->thumbHover; isFillBack = true; }
    }
    else if (index == INDEX_OUTER)
    {
        isFillBack = true;
    }

    float btn1Size, btn2Size, btnMinSize, btnThumbSize;
    if (isGdip)
    {
        if (hbrBtn1 == 0)  hbrBtn1 = new Gdiplus::SolidBrush(rcInfo->crBtn1);
        if (hbrBtn2 == 0)  hbrBtn2 = new Gdiplus::SolidBrush(rcInfo->crBtn2);
        if (hbrThumb == 0) hbrThumb = new Gdiplus::SolidBrush(rcInfo->crThumb);
    }
    btn1Size = rcInfo->btn1Size;
    btn2Size = rcInfo->btn2Size;
    btnMinSize = rcInfo->btnMinSize;
    btnThumbSize = rcInfo->btnThumbSize;

    const float page = (float)(m_hScroll.nPage ? m_hScroll.nPage : 1);
    m_hrc.maxPos = (float)(m_hScroll.nMax - m_hScroll.nPage + 1);
    const float buttonSize = btn1Size + btn2Size;                           // 上下两个按钮的高度
    m_hrc.maxBlockHeight = (float)cxClient - buttonSize;                    // 滑块总的高度, 滑块条高度 - 上下两个按钮的高度
    float pageCount = m_hScroll.nMax / page;                                // 滚动的页数
    m_hrc.blockHeight = m_hrc.maxBlockHeight / pageCount;                   // 滑块的高度
    if (m_hrc.blockHeight < btnMinSize)
        m_hrc.blockHeight = btnMinSize;

    if (m_hScroll.nPage == 0)
    {
        pageCount = 2;
        m_hrc.blockHeight = 100;
        m_hrc.maxPos--;
    }

    //const float moveSize = m_hrc.maxBlockHeight / m_hScroll.nMax;           // 滑动一次的像素
    //const float maxHeight = cxClient * pageCount;                           // 显示所有内容需要的高度

    if (pageCount <= 1.f || m_hrc.maxPos < 0)
    {
        rcThumb = &rcInfo->thumbNormal;
        rcBtn1 = &rcInfo->btnNormal1;
        rcBtn2 = &rcInfo->btnNormal2;
        isFillBack = false;
    }

    Gdiplus::Graphics gp(hdc);
    bool isNoDraw = false;
    RECT_F rcDrawBtn1(0, 0, btn1Size, btnThumbSize);
    RECT_F rcDrawThumb(btn1Size, 0, (float)cxClient - btn1Size, btnThumbSize);  // 中间的滚动条背景
    RECT_F rcDrawBtn2((float)cxClient - btn1Size, 0, (float)cxClient, btnThumbSize);        // 底边的按钮
    if (isGdip)
    {

        RECT_F rcback(rcDrawBtn1.left, rcDrawBtn1.top, rcDrawThumb.right, rcDrawBtn2.bottom);
        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, false, hdc, &gp, ISCROLL_DRAWTYPE_BACK, &rcback);

        if (!isNoDraw)
        {
            Gdiplus::SolidBrush hbrBack(rcInfo->crBack);
            gp.FillRectangle(&hbrBack, rcDrawBtn1.left, rcDrawBtn1.top,
                rcDrawBtn2.right - rcDrawBtn1.left, rcDrawThumb.bottom - rcDrawThumb.top);
        }
    }

    if (isGdip)   // 没有图片, 使用默认色绘画
    {
        Gdiplus::GraphicsPath path, path2;
        const float btnOffset = 3;
        Gdiplus::PointF pt[3];
        Gdiplus::PointF pt2[3];
        pt[0].X = rcDrawBtn1.left + btnOffset;      // 左边x, 左边+偏移
        pt[0].Y = btnThumbSize / 2;                 // 左边x, 纵向位置是中间

        pt[1].X = rcDrawBtn1.right - btnOffset;     // 右上角x, 右边 - 偏移
        pt[1].Y = rcDrawBtn1.top + btnOffset-1;       // 右上角y, 顶边 + 偏移

        pt[2].X = rcDrawBtn1.right - btnOffset;     // 右下角x, 右边 - 偏移
        pt[2].Y = rcDrawBtn1.bottom - btnOffset+1;    // 右下角y, 底边 - 偏移



        pt2[0].X = rcDrawBtn2.left + btnOffset;      // 左上角x, 左边 + 偏移
        pt2[0].Y = rcDrawBtn2.top + btnOffset-1;       // 左上角y, 顶边 + 偏移

        pt2[1].X = rcDrawBtn2.left + btnOffset;     // 左下角x, 左边 + 偏移
        pt2[1].Y = rcDrawBtn2.bottom - btnOffset + 1;   // 左下角y, 底边 - 偏移

        pt2[2].X = rcDrawBtn2.right - btnOffset;    // 右边x, 中间位置
        pt2[2].Y = btnThumbSize / 2;                // 右边y, 底边 - 偏移

        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, false, hdc, &gp, ISCROLL_DRAWTYPE_BTN1, &rcDrawBtn1);
        if (!isNoDraw && m_hInfo.btn1Size > 0)
            gp.FillPolygon(hbrBtn1, pt, 3);

        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, false, hdc, &gp, ISCROLL_DRAWTYPE_BTN2, &rcDrawBtn2);
        if (!isNoDraw && m_hInfo.btn2Size > 0)
            gp.FillPolygon(hbrBtn2, pt2, 3);
    }
    else
    {
        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, false, hdc, &gp, ISCROLL_DRAWTYPE_BTN1, &rcDrawBtn1);
        if (!isNoDraw && m_hInfo.btn1Size > 0)
            DrawImageFromRect(&gp, m_HBmp, rcDrawBtn1, *rcBtn1);    // 绘画左边按钮

        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, false, hdc, &gp, ISCROLL_DRAWTYPE_BTN2, &rcDrawBtn2);
        if (!isNoDraw && m_hInfo.btn2Size > 0)
            DrawImageFromRect(&gp, m_HBmp, rcDrawBtn2, *rcBtn2);    // 绘画右边按钮

    }
    if ((UINT)m_hScroll.nMax < m_hScroll.nPage)
    {
        delete hbrThumb;
        delete hbrBtn1;
        delete hbrBtn2;
        return;
    }

    if (isFillBack && !isGdip)
        DrawImageFromGrid(&gp, m_HBmp, rcDrawThumb, rcInfo->back, rcInfo->backGrid, 255);

    memset(&m_hrc.thumb, 0, sizeof(RECT));
    m_hrc.btn1.left = rcScroll.left + (long)rcDrawBtn1.left;
    m_hrc.btn1.top = rcScroll.top;
    m_hrc.btn1.right = m_hrc.btn1.left + (long)(rcDrawBtn1.right - rcDrawBtn1.left);
    m_hrc.btn1.bottom = m_hrc.btn1.top + (long)(rcDrawBtn1.bottom - rcDrawBtn1.top);


    m_hrc.btn2.left = rcScroll.right - (long)(rcDrawBtn2.right - rcDrawBtn2.left);      // 滚动条右边减去按钮2宽度
    m_hrc.btn2.top = rcScroll.top;
    m_hrc.btn2.right = m_hrc.btn2.left + (long)(rcDrawBtn2.right - rcDrawBtn2.left);
    m_hrc.btn2.bottom = m_hrc.btn2.top + (long)(rcDrawBtn2.bottom - rcDrawBtn2.top);


    if (pageCount > 1.f)
    {
        // 总行数/一页的行数=滚的页数
        //总的滑块显示高度/滚的页数=块高
        //总的滑块显示高度-块高=可滑动的距离
        //总的滑块显示高度/总行数=每次滑动距离

        //const float 每次滚动的像素 = (总画面高度 - cxClient) / (m_hScroll.nMax?m_hScroll.nMax:1);
        //const float 总要滚动的次数 = cxClient / 每次滚动的像素;
        // 总要滚动的次数 = 滚动条高度 / 每次滚动的像素
        // 块高度 = 总要滚动的次数 / 滚动条高度
        // 当前位置 / 最大位置 * 滚动条高度 = 顶边位置
        // 比例 * 
        float currentPos = 0;
        if (isGdip)
            currentPos = (float)(m_hScroll.nPos / m_hrc.maxPos * (float)(m_hrc.maxBlockHeight - m_hrc.blockHeight));
        else
            currentPos = (float)((int)(m_hScroll.nPos / m_hrc.maxPos * (float)(m_hrc.maxBlockHeight - m_hrc.blockHeight)));

        // rcDrawThumb.left 决定了滑块画出的位置
        // 需要在按下的时候记录偏移位置, 如果不是鼠标按下状态就可以直接用 currentPos 作为位置
        if (m_downOffset.x > 0)
        {
            int x = (int)m_thumbPoint.x - rcScroll.left - m_downOffset.x;

            int min = (int)(m_hrc.btn1.right - m_hrc.btn1.left);
            int max = (int)(m_hrc.maxBlockHeight - m_hrc.blockHeight + min);
            if (x < min)        x = min;
            else if (x > max)   x = max;

            rcDrawThumb.left = (float)(x);
        }
        else
        {
            rcDrawThumb.left += (float)((int)currentPos);
        }


        if (isGdip)
            rcDrawThumb.right = (float)(rcDrawThumb.left + m_hrc.blockHeight);
        else
            rcDrawThumb.right = (float)((int)(rcDrawThumb.left + m_hrc.blockHeight));

        if (rcDrawThumb.right > (float)cxClient - btn1Size)
            rcDrawThumb.right = (float)cxClient - btn1Size;

        m_hrc.thumb.left = (long)rcScroll.left + (long)rcDrawThumb.left;
        m_hrc.thumb.top = (long)rcScroll.top;
        m_hrc.thumb.right = m_hrc.thumb.left + (long)(rcDrawThumb.right - rcDrawThumb.left);
        m_hrc.thumb.bottom = m_hrc.thumb.top + (long)(rcDrawThumb.bottom - rcDrawThumb.top);

        isNoDraw = pfn_DrawScroll(m_pfn_DrawScroll, this, false, hdc, &gp, ISCROLL_DRAWTYPE_THUMB, &rcDrawThumb);
        if (!isNoDraw)
        {
            if (isGdip)
            {
                gp.FillRectangle(hbrThumb, rcDrawThumb.left, rcDrawThumb.top,
                    rcDrawThumb.right - rcDrawThumb.left, rcDrawThumb.bottom - rcDrawThumb.top);
            }
            else
            {
                DrawImageFromGrid(&gp, m_HBmp, rcDrawThumb, *rcThumb, rcInfo->backGrid);    // 滚动条
            }
        }
    }
    delete hbrThumb;
    delete hbrBtn1;
    delete hbrBtn2;
}
