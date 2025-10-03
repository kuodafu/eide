#pragma once
#include "WndBase.h"
#include <assist/CFileRW.h>

typedef bool(CALLBACK* pfn_CustomListDrawItem)(HWND hWnd, HDC hdc, LPRECT rcItem, int index, STATE state, bool isFocus);
typedef void(CALLBACK* pfn_CustomListItemSelect)(HWND hWnd, int index);
typedef void(CALLBACK* pfn_CustomListItemRClick)(HWND hWnd, int index);
typedef void(CALLBACK* pfn_CustomListItemClick)(HWND hWnd, int index, bool isDblClk);

#define LVM_GETITEMHEIGHT       LVM_FIRST + 230
#define LVM_SETITEMHEIGHT       LVM_FIRST + 231
#define LVM_GETHOTLIGHTCOLOR    LVM_FIRST + 232
#define LVM_SETHOTLIGHTCOLOR    LVM_FIRST + 233
class CCustomList : public CControlBase
{
private:
    enum CUSTOM_FLAGS
    {
        CUSTOM_FLAGS_FOCUS  = 0x00000001,   // 焦点

        LVF_USERBKCLR       = 0x00000800,
    };
    struct CUSTOM_LISTDATA
    {
        int nItemCount;     // 表项数
        int nItemHeight;    // 表项高度
        int indexHot;       // 热点项
        int indexSel;       // 选中项
        int indexDown;      // 按下项
        int indexRDown;     // 右键按下项
        int scrollPos;      // 当前滚动位置
        DWORD dwCustom;
        HWND hParent;       // 控件父窗口
        int cxClient;
        int cyClient;
        DWORD flags;

        COLORREF clrBk;         // Background color
        COLORREF clrText;       // text color
        COLORREF clrTextBk;     // text background color
        COLORREF clrHotlight;   
        COLORREF clrOutline;    
        COLORREF clrDown;
        COLORREF clrSel;
        COLORREF clrKillFocus;
        HBRUSH  hbrBk;
        HBRUSH  hbrText;
        HBRUSH  hbrTextBk;
        HBRUSH  hbrHotlight;
        HBRUSH  hbrDown;
        HBRUSH  hbrSel;
        HBRUSH  hbrKillFocus;
        HPEN    hpenOutline;
        HFONT   hFont;

        WNDPROC                 fnNotify;

    };

    CUSTOM_LISTDATA m_data;
protected:
    virtual inline LPCWSTR SuperClassName() { return L"CCustomList"; }

public:
    CCustomList() :CControlBase()
    {
        memset(&m_data, 0, sizeof(m_data));
        m_data.indexHot = -1;
        m_data.indexSel = -1;
        m_data.indexDown = -1;
        m_data.nItemCount = 0;
        m_data.nItemHeight = 30;

        m_data.clrBk        = RGB(255, 255, 255);
        m_data.clrText      = RGB(0, 0, 0);
        m_data.clrTextBk    = RGB(255, 255, 255);
        m_data.clrHotlight  = RGB(229, 243, 255);
        m_data.clrDown      = RGB(205, 232, 255);
        m_data.clrSel       = RGB(205,232,255);
        m_data.clrKillFocus = RGB(217, 217, 217);
        m_data.clrOutline   = RGB(204, 206, 219);

    }
    virtual ~CCustomList()
    {
        if (m_data.hbrBk)       DeleteObject(m_data.hbrBk);
        if (m_data.hbrText)     DeleteObject(m_data.hbrText);
        if (m_data.hbrTextBk)   DeleteObject(m_data.hbrTextBk);
        if (m_data.hbrHotlight) DeleteObject(m_data.hbrHotlight);
        if (m_data.hbrDown)     DeleteObject(m_data.hbrDown);
        if (m_data.hbrSel)      DeleteObject(m_data.hbrSel);
        if (m_data.hbrKillFocus)DeleteObject(m_data.hbrKillFocus);
        if (m_data.hpenOutline) DeleteObject(m_data.hpenOutline);
        if (m_data.hFont)       DeleteObject(m_data.hFont);
    }
    virtual inline LPCWSTR className() { return L"CCustomList"; };

    virtual inline void SetNotifyCallback(WNDPROC pfnDraw) { m_data.fnNotify = pfnDraw; }
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id = 0, LPVOID lpParam = 0)
    {
        m_data.hParent = hWndParent;
        HWND hWnd = CWndBase::create(dwExStyle, lpszTitle, dwStyle, x, y, nWidth, nHeight, hWndParent, id, lpParam);
        LOGFONTW lf = { 0 };
        SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);
        lf.lfHeight = -12;
        lf.lfCharSet = GB2312_CHARSET;
        memcpy(lf.lfFaceName, L"微软雅黑", 10);
        m_data.hFont = CreateFontIndirectW(&lf);
        SetFont(m_data.hFont);
        SetClassLongPtrW(hWnd, GCL_STYLE, GetClassLongPtrW(hWnd, GCL_STYLE) | CS_DBLCLKS);
        return hWnd;
    }
public:

    inline BOOL SetBkColor(COLORREF clrBak)
    {
        m_data.flags |= LVF_USERBKCLR;
        return (LRESULT)ListView_OnSetBkColor(&m_data, (COLORREF)clrBak);
    }
    inline COLORREF GetBkColor() const
    {
        return m_data.clrBk;
    }
#define _RECREATE_BRUSH(_n)                                 \
    if (m_data.hbr##_n)                                     \
    {                                                       \
        DeleteObject(m_data.hbr##_n);                       \
        m_data.hbr##_n = CreateSolidBrush(m_data.clr##_n);  \
    }

    inline BOOL SetTextColor(COLORREF clrBak)
    {
        m_data.clrText = clrBak;
        _RECREATE_BRUSH(Text);
        return TRUE;
    }
    inline COLORREF GetTextColor() const
    {
        return m_data.clrText;
    }
    inline BOOL SetTextBkColor(COLORREF clrBak)
    {
        m_data.clrTextBk = clrBak;
        _RECREATE_BRUSH(TextBk);
        return TRUE;
    }
    inline COLORREF GetTextBkColor() const
    {
        return m_data.clrTextBk;
    }
    inline BOOL SetHotColor(COLORREF clrBak)
    {
        m_data.clrHotlight = clrBak;
        _RECREATE_BRUSH(Hotlight);
        return TRUE;
    }
    inline COLORREF GetHotColor() const
    {
        return m_data.clrHotlight;
    }
    inline COLORREF SetOutLineColor(COLORREF clrBak)
    {
        COLORREF lres = m_data.clrOutline;
        m_data.clrOutline = clrBak;
        if (m_data.hpenOutline)
        {
            DeleteObject(m_data.hpenOutline);
            m_data.hpenOutline = CreatePen(PS_SOLID, 1, m_data.clrOutline);
        }
        return lres;
    }
    inline COLORREF GetOutLineColor() const
    {
        return m_data.clrOutline;
    }


    inline COLORREF SetDownColor(COLORREF clrBak)
    {
        COLORREF lres = m_data.clrDown;
        m_data.clrDown = clrBak;
        _RECREATE_BRUSH(Down);
        return lres;
    }
    inline COLORREF GetDownColor() const
    {
        return m_data.clrDown;
    }
    inline COLORREF SetSelColor(COLORREF clrBak)
    {
        COLORREF lres = m_data.clrSel;
        m_data.clrSel = clrBak;
        _RECREATE_BRUSH(Sel);
        return lres;
    }
    inline COLORREF GetSelColor() const
    {
        return m_data.clrSel;
    }
    inline COLORREF SetKillFocusColor(COLORREF clrBak)
    {
        COLORREF lres = m_data.clrKillFocus;
        m_data.clrKillFocus = clrBak;
        _RECREATE_BRUSH(KillFocus);
        return lres;
    }
#undef _RECREATE_BRUSH

    inline COLORREF GetKillFocusColor() const
    {
        return m_data.clrKillFocus;
    }

    inline void SetDrawInfo(COLORREF crBack = -1, COLORREF crHot = -1, COLORREF crDown = -1, COLORREF crKillFocus = -1)
    {
    //    SetHbrush(m_data.hbrBack, crBack);
    //    SetHbrush(m_data.hbrHot, crHot);
    //    SetHbrush(m_data.hbrDown, crDown);
    //    SetHbrush(m_data.hbrKillFocus, crKillFocus);
    }

    // 设置表项数量
    // count = 项目数量
    // dwFlags = LVSICF_ 开头常量, 目前只支持 LVSICF_NOINVALIDATEALL 标志
    int SetItemCount(int count, DWORD dwFlags = 0)
    {
        BOOL fInvalidateAll = ((dwFlags & LVSICF_NOINVALIDATEALL) == 0);

        m_data.nItemCount = count;
        if (m_data.cxClient == 0)
        {
            RECT rc;
            ::GetClientRect(m_hWnd, &rc);
            m_data.cxClient = rc.right - rc.left;
            m_data.cyClient = rc.bottom - rc.top;
        }
        SCROLLINFO si;
        si.cbSize = sizeof(si);
        si.fMask = SIF_PAGE | SIF_RANGE;
        si.nMin = 0;
        si.nMax = count;
        si.nPage = m_data.cyClient / GetItemHeight() + 1;
        ShowScrollBar(m_hWnd, SB_VERT, si.nMax > (int)si.nPage);
        SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
        m_data.scrollPos = GetScrollPos(m_hWnd, SB_VERT);

        if (fInvalidateAll)
            InvalidateRect();
        return TRUE;
    }

    inline int GetItemCount() const
    {
        return m_data.nItemCount;
    }

    inline int GetItemHeight()
    {
        if (m_data.nItemHeight <= 0)
            m_data.nItemHeight = 30;
        return m_data.nItemHeight;
    }

    inline int SetItemHeight(int nItemHeight)
    {
        if (nItemHeight <= 0)
            nItemHeight = 30;
        m_data.nItemHeight = (int)nItemHeight;
        return m_data.nItemHeight;
    }


    inline int GetSelItem() const
    {
        return GetIndex();
    }
    inline int GetIndex() const
    {
        return m_data.indexSel;
    }

    inline int SetSelItem(int index, bool isUpdate = false)
    {
        return SetIndex(index, isUpdate);
    }
    inline int SetIndex(int index, bool isUpdate = false)
    {
        const int oldIndex = m_data.indexSel;
        m_data.indexSel = index;
        if (isUpdate)
        {
            if(oldIndex != -1)
                InvalidateItem(oldIndex);
            if (oldIndex != index)
                InvalidateItem(index);
        }
        return oldIndex;
    }

    // 保证显示
    inline bool SetEnsurVisible(int index, bool isShow = true)
    {
        if (index + 1 >= m_data.nItemCount || index < 0)return false;
        m_data.scrollPos = index;
        return ::SetScrollPos(m_hWnd, SB_VERT, m_data.scrollPos, isShow);
    }
    // 保证显示
    inline bool SetTopIndex(int index, bool isShow = true)
    {
        return SetEnsurVisible(index, isShow);
    }

    // 重画指定项目
    inline void InvalidateItem(int index)
    {
        if (index >= m_data.nItemCount)
            return;
        int top = (index - m_data.scrollPos) * m_data.nItemHeight;
        if (top + m_data.nItemHeight <= 0) return;   // 顶边+高度 小于0, 不需要重画

        RECT rc;
        if (m_data.cxClient == 0)
        {
            ::GetClientRect(m_hWnd, &rc);
            m_data.cxClient = rc.right - rc.left;
            m_data.cyClient = rc.bottom - rc.top;
        }
        rc = { 0, 0, m_data.cxClient, m_data.cyClient };
        if (top > rc.bottom - rc.top)
            return; // 顶边大于列表底边, 不需要重画
        rc.left = 0;
        rc.top = top;
        rc.bottom = rc.top + m_data.nItemHeight;
        ::InvalidateRect(m_hWnd, &rc, 0);
    }

private:

    inline int pt2index(int x, int y) {
        const int maxHeight = m_data.nItemCount * m_data.nItemHeight;
        if (y > maxHeight)
            return -1;
        return (y / m_data.nItemHeight) + m_data.scrollPos;

    }
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case LVM_GETITEMCOUNT:
        case LVM_SETITEMCOUNT:
        case LVM_GETITEMHEIGHT:
        case LVM_SETITEMHEIGHT:
        case LVM_GETBKCOLOR:
        case LVM_SETBKCOLOR:
        case LVM_GETTEXTCOLOR:
        case LVM_SETTEXTCOLOR:
        case LVM_GETTEXTBKCOLOR:
        case LVM_SETTEXTBKCOLOR:
        case LVM_GETHOTLIGHTCOLOR:
        case LVM_SETHOTLIGHTCOLOR:
        case LVM_SETOUTLINECOLOR:
        case LVM_GETOUTLINECOLOR:
        case LVM_ENSUREVISIBLE:
            return OnListViewEvent(oldProc, hWnd, message, wParam, lParam);
        case WM_PAINT:
            return OnPaint(oldProc, hWnd, message, wParam, lParam);
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEMOVE:
        {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            const int maxHeight = m_data.nItemCount * m_data.nItemHeight;
            
            TRACKMOUSEEVENT evt;
            evt.cbSize = sizeof(evt);
            evt.dwHoverTime = HOVER_DEFAULT;
            evt.hwndTrack = hWnd;
            evt.dwFlags = TME_LEAVE;
            TrackMouseEvent(&evt);

            if (y > maxHeight || x < 0 || x > m_data.cxClient)
            {
                if (m_data.indexHot != -1)
                {
                    InvalidateItem(m_data.indexHot);
                    m_data.indexHot = -1;
                }
                return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
            }
            int indexHot = ((y) / m_data.nItemHeight) + m_data.scrollPos;
            if (m_data.indexHot != indexHot)
            {
                InvalidateItem(m_data.indexHot);
                m_data.indexHot = indexHot;
                InvalidateItem(m_data.indexHot);
            }
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }

        case WM_LBUTTONDBLCLK:
        {
            SendNotify_Click(NM_DBLCLK, m_data.indexSel);
            break;
        }
        case WM_LBUTTONDOWN:
        {
            //DWORD style = GetStyle(false);
            //if (__query(style, WS_TABSTOP))
                SetFocus();
            SendNotify_Click(NM_LDOWN, m_data.indexSel);

            if (m_data.indexHot != -1)
            {
                m_data.indexDown = m_data.indexHot;
                InvalidateItem(m_data.indexDown);
                SetCapture();
            }
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        case WM_CAPTURECHANGED:
        {
            POINT pt;
            GetCursorPos(&pt);
            ScreenToClient(hWnd, &pt);
            int index = pt2index(pt.x, pt.y);


            if (index != -1 && index == m_data.indexDown)   // 放开和按下的是一个表项, 那就是选择
            {
                if (m_data.indexSel != -1)
                    InvalidateItem(m_data.indexSel);
                m_data.indexSel = m_data.indexDown;
                InvalidateItem(m_data.indexSel);

                NMLISTVIEW item = { 0 };
                item.iItem = m_data.indexSel;
                SendNotify(LVN_ITEMCHANGED, (LPNMHDR)&item);
                SendNotify_Click(NM_CLICK, index);
            }

            if (m_data.indexDown != -1)
            {
                InvalidateItem(m_data.indexDown);
                m_data.indexDown = -1;
            }
            return 0;
        }
        case WM_LBUTTONUP:
        {
            ReleaseCapture();
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        case WM_MOUSELEAVE:
        {
            if (m_data.indexHot != -1)
            {
                InvalidateItem(m_data.indexHot);
                m_data.indexHot = -1;
            }
            break;
        }
        case WM_RBUTTONDOWN:
        {
            //DWORD style = GetStyle(false);
            //if (__query(style, WS_TABSTOP))
                SetFocus();
            m_data.indexRDown = m_data.indexHot;
            SendNotify_Click(NM_RDOWN, m_data.indexRDown);
            break;
        }
        case WM_RBUTTONUP:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            int index = pt2index(pt.x, pt.y);
            if (index == m_data.indexRDown)
            {
                NMITEMACTIVATE item = { 0 };
                item.iItem = index;
                SendNotify(NM_RCLICK, (LPNMHDR)&item);
            }
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        case WM_MOUSEWHEEL:
        {
            const int count = 3;
            //int fwKeys = GET_KEYSTATE_WPARAM(wParam);
            const int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);  // 大于0 往上滚动, 小于0 往下滚动
            int pos;
            int newPos = m_data.scrollPos;
            pos = (zDelta / 120) * (count);

            newPos -= pos;

            int page = m_data.cyClient / GetItemHeight();

            const int maxPos = m_data.nItemCount - page;

            newPos = max(0, min(newPos, maxPos));
            if (newPos != m_data.scrollPos)
            {
                m_data.scrollPos = newPos;
                ::SetScrollPos(hWnd, SB_VERT, m_data.scrollPos, true);
                InvalidateRect();
            }
            break;
        }
        case WM_VSCROLL:
        {
            SetScrollPos(hWnd, true, wParam, lParam, true);
            return 0;
        }
        case WM_SIZE:
        {
            m_data.cxClient = LOWORD(lParam);
            m_data.cyClient = HIWORD(lParam);
            SetItemCount(m_data.nItemCount);
            break;
        }
        case WM_SETFOCUS:
        {
            m_data.flags |= CUSTOM_FLAGS_FOCUS;
            if (m_data.indexSel != -1)
            {
                InvalidateItem(m_data.indexSel);
                return 0;
            }
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        case WM_KILLFOCUS:
        {
            m_data.flags &= ~CUSTOM_FLAGS_FOCUS;
            if (m_data.indexSel != -1)
            {
                InvalidateItem(m_data.indexSel);
                return 0;
            }
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        case WM_SETFONT:
        {
            HFONT hFont = (HFONT)wParam;
            LOGFONTW lf = { 0 };
            GetObjectW(hFont, sizeof(lf), &lf);
            if (m_data.hFont)
                DeleteObject(m_data.hFont);
            m_data.hFont = CreateFontIndirectW(&lf);
            if (lParam)
                InvalidateRect();
            //return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
            return 1;
        }
        case WM_GETFONT:
            return (LRESULT)m_data.hFont;
        default:
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        return 0;
    }


    LRESULT OnListViewEvent(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        CUSTOM_LISTDATA* plv = &m_data;
        switch (message)
        {
        case LVM_SETITEMCOUNT:
            return SetItemCount((int)wParam, (DWORD)lParam);
        case LVM_GETITEMCOUNT:
            return GetItemCount();
        case LVM_GETITEMHEIGHT:
            return GetItemHeight();
        case LVM_SETITEMHEIGHT:
            return SetItemHeight((int)lParam);
        case LVM_GETBKCOLOR:
            return GetBkColor();
        case LVM_SETBKCOLOR:
            return SetBkColor((COLORREF)lParam);
        case LVM_GETTEXTCOLOR:
            return GetTextColor();
        case LVM_SETTEXTCOLOR:
            return SetTextColor((COLORREF)lParam);
        case LVM_GETTEXTBKCOLOR:
            return GetTextBkColor();
        case LVM_SETTEXTBKCOLOR:
            return SetTextBkColor((COLORREF)lParam);
        case LVM_GETHOTLIGHTCOLOR:
            return GetHotColor();
        case LVM_SETHOTLIGHTCOLOR:
            return SetHotColor((COLORREF)lParam);
        case LVM_SETOUTLINECOLOR:
            return SetOutLineColor((COLORREF)lParam);
        case LVM_GETOUTLINECOLOR:
            return GetOutLineColor();
        case LVM_ENSUREVISIBLE:
            return SetEnsurVisible((int)wParam, (bool)(int)lParam);
        default:
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }

        return 0;
    }
    void DrawItem(HWND hWnd, NMLVCUSTOMDRAW& draw)
    {
        HBRUSH hbr = 0;
        HDC hdc = draw.nmcd.hdc;
        const int sel = LVIS_SELECTED | CDIS_FOCUS;
        switch (draw.nmcd.uItemState)
        {
        case CDIS_FOCUS:    hbr = GetDownBrush();   break;
        case sel:           hbr = GetSelBrush();    break;
        case CDIS_HOT:      hbr = GetHotBrush();    break;
        default:
            hbr = GetBkBrush();    break;
        }

        if (hbr)
        {
            if ((!query(m_data.flags, CUSTOM_FLAGS_FOCUS)) && (draw.nmcd.uItemState == sel))
                hbr = GetKillFocusBrush();
            FillRect(hdc, &draw.nmcd.rc, hbr);
        }

    }

    inline void DrawItemText(HDC hdc, NMLVCUSTOMDRAW& draw)
    {
        NMLVDISPINFOW info = { 0 };
        info.item.iItem = (int)draw.nmcd.dwItemSpec;
        info.item.iSubItem = 0;
        info.item.state = draw.nmcd.uItemState;

        SendNotify(LVN_GETDISPINFOW, (LPNMHDR)&info);

        if (info.item.pszText && *info.item.pszText)
        {
            COLORREF clrText = ::SetTextColor(hdc, draw.clrText);
            RECT rcText = draw.nmcd.rc;
            rcText.left += 5;
            DrawTextW(hdc, info.item.pszText, -1, &rcText, DT_VCENTER | DT_SINGLELINE);
            ::SetTextColor(hdc, clrText);
        }
    }

    // 返回值是滚动框的当前位置
    int SetScrollPos(HWND hWnd, bool isVert, WPARAM wParam, LPARAM lParam, bool isUpdate)
    {
        SCROLLINFO si = { 0 };
        const int nBar = isVert ? SB_VERT : SB_HORZ;
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        BOOL bRet = GetScrollInfo(hWnd, nBar, &si);
        int nMaxPos = si.nMax - si.nPage;
        const int LINESIZE = m_data.nItemHeight;
        int pos = HIWORD(wParam);
        int scrValue = 0;
        int code = LOWORD(wParam);

        //switch (code)
        //{
        //case SB_LINEUP:
        //    if (nMaxPos - si.nPos < LINESIZE)
        //        scrValue = (nMaxPos - si.nPos);
        //    else
        //        scrValue = LINESIZE;
        //    break;
        //case SB_LINEDOWN:
        //    if (nMaxPos - si.nPos < LINESIZE)
        //        scrValue = -(nMaxPos - si.nPos);
        //    else
        //        scrValue = -LINESIZE;
        //    break;
        //case SB_PAGEUP:
        //    if (si.nPos)
        //    {
        //        scrValue = (si.nPos - si.nPage < 0) ? si.nPos : si.nPage;
        //    }
        //    break;
        //case SB_PAGEDOWN:
        //    if (si.nPage != nMaxPos)
        //    {
        //        scrValue = (nMaxPos - si.nPos < (int)si.nPage) ? (-(nMaxPos - si.nPos)) : -((int)si.nPage);
        //    }
        //    break;
        //case SB_THUMBTRACK:
        //    scrValue = si.nPos - pos;
        //    break;
        //default:
        //    break;
        //}
        //int x = 0, y = 0;
        //if (isVert) y = scrValue;
        //else x = scrValue;
        //ScrollWindow(hWnd, x, y * GetItemHeight(), 0, 0);

    

        switch (code)
        {
        case SB_LINEUP:
            si.nPos--;
            break;
        case SB_LINEDOWN:
            si.nPos++;
            break;
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
        default:
            break;
        }

        int x = 0, y = 0;
        if (isVert) y = si.nPos;
        else x = si.nPos;
        ScrollWindow(hWnd, x, y, 0, 0);

        si.fMask = SIF_POS;
        //si.nPos -= scrValue;
        m_data.scrollPos = si.nPos;
        int ret = SetScrollInfo(hWnd, nBar, &si, TRUE);
        if (isUpdate) ::InvalidateRect(hWnd, 0, 0);
        return ret;
    }
    
    inline void DrawItems(HDC hdc, PAINTSTRUCT& ps)
    {
        int pos = GetScrollPos(m_hWnd, SB_VERT);

        NMLVCUSTOMDRAW draw = { 0 };
        NMCUSTOMDRAW& nmcd = draw.nmcd;
        nmcd.hdc = hdc;
        nmcd.rc = { 0, 0, m_data.cxClient, m_data.cyClient };
        m_data.dwCustom = CICustomDrawNotify(CDDS_PREPAINT, &draw);

        // 自定义绘画, 不循环绘画表项, 直接返回
        if ((m_data.dwCustom & CDRF_SKIPDEFAULT))
            return;

        RECT& rcItem = nmcd.rc;
        const int nItemHeight = GetItemHeight();
        const int firstIndex = pos;
        rcItem.top = 0;
        rcItem.bottom = rcItem.top + nItemHeight;

        nmcd.uItemState = 0;
        for (int i = firstIndex; i < m_data.nItemCount; i++)
        {
            if (rcItem.top >= m_data.cyClient)
                break;
            // 项目的顶边大于重画的区域的底边, 项目在重画区域底下, 不需要重画
            // 项目的底边小于重画的区域顶边, 项目在重画区域的上面, 不需要重画
            if (!(rcItem.bottom < ps.rcPaint.top || rcItem.top > ps.rcPaint.bottom))
            {
                nmcd.uItemState =
                    i == m_data.indexDown ? CDIS_FOCUS :
                    i == m_data.indexSel ? (LVIS_SELECTED | CDIS_FOCUS) :
                    i == m_data.indexHot ? CDIS_HOT : 0;

                nmcd.dwItemSpec = i;
                nmcd.dwDrawStage = CDDS_ITEMPREPAINT;
                draw.clrText = m_data.clrText;
                draw.clrTextBk = m_data.clrTextBk;
                draw.clrFace = m_data.clrBk;

//#define COLORISLIGHT(clr) ((5*GetGValue((clr)) + 2*GetRValue((clr)) + GetBValue((clr))) > 8*128)
//                if (draw.clrText == draw.clrTextBk)
//                {
//                    if (COLORISLIGHT(draw.clrTextBk))
//                        draw.clrText = 0x000000; // black
//                    else
//                        draw.clrText = 0xFFFFFF; // white
//                }



                // 项目绘画前
                m_data.dwCustom = CICustomDrawNotify(CDDS_ITEMPREPAINT, &draw);

                // 不是自绘才处理
                if (!(m_data.dwCustom & CDRF_SKIPDEFAULT))
                {
                    DrawItem(m_hWnd, draw);
                    DrawItemText(hdc, draw);

                    // 控件将在绘制项后通知父级。当 dwDrawStage 等于 CDDS_PREPAINT 时，会发生这种情况
                    if (m_data.dwCustom & CDRF_NOTIFYPOSTPAINT)
                    {
                        draw.iSubItem = 0;
                        COLORREF clrTextOld = ::SetTextColor(hdc, draw.clrText);
                        COLORREF clrTextBkOld = ::SetBkColor(hdc, draw.clrTextBk);
                        CICustomDrawNotify(CDDS_ITEMPOSTPAINT, &draw);  // 绘制项目后
                        ::SetTextColor(hdc, clrTextOld);
                        ::SetBkColor(hdc, clrTextBkOld);
                    }
                }

                if (m_data.dwCustom & CDRF_NEWFONT)
                {
                    SelectObject(nmcd.hdc, m_data.hFont);
                }
            }
            rcItem.top += nItemHeight;
            rcItem.bottom += nItemHeight;
        }
    }
    LRESULT OnPaint(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        PAINTSTRUCT ps;
        RECT rc;
        BeginPaint(hWnd, &ps);

        if (m_data.cxClient == 0)
        {
            ::GetClientRect(hWnd, &rc);
            m_data.cxClient = rc.right - rc.left;
            m_data.cyClient = rc.bottom - rc.top;
        }
        rc = { 0, 0, m_data.cxClient, m_data.cyClient };

        LPMEMDCBITMAP md = md_create(m_data.cxClient, m_data.cyClient);
        FillRect(md->hdc, &rc, GetBkBrush());
        SetBkMode(md->hdc, TRANSPARENT);
        SelectObject(md->hdc, m_data.hFont);
        DrawItems(md->hdc, ps);
        BitBlt(ps.hdc, 0, 0, m_data.cxClient, m_data.cyClient, md->hdc, 0, 0, SRCCOPY);
        md_destroy(md);
        EndPaint(hWnd, &ps);
        return 0;
    }
    inline LRESULT SendNotify_Click(UINT code, int iItem)
    {
        NMITEMACTIVATE item = { 0 };
        item.iItem = iItem;
        return SendNotify(code, (LPNMHDR)&item);
    }
    inline LRESULT SendNotify(UINT code, LPNMHDR lpnmhdr)
    {
        lpnmhdr->code = code;
        lpnmhdr->hwndFrom = m_hWnd;
        lpnmhdr->idFrom = (UINT_PTR)GetWindowLongPtrW(m_hWnd, GWLP_ID);
        if (m_data.fnNotify)
            return m_data.fnNotify(m_data.hParent, WM_NOTIFY, lpnmhdr->idFrom, (LPARAM)lpnmhdr);
        return SendMessageW(m_data.hParent, WM_NOTIFY, lpnmhdr->idFrom, (LPARAM)lpnmhdr);
    }
    inline DWORD CICustomDrawNotify(DWORD dwStage, LPNMLVCUSTOMDRAW lpnmcd)
    {
        DWORD dwRet = CDRF_DODEFAULT;
        if ((dwStage & CDDS_ITEM) && !(m_data.dwCustom & CDRF_NOTIFYITEMDRAW))
        {
            return dwRet;
        }
        lpnmcd->nmcd.dwDrawStage = dwStage;
        dwRet = (DWORD)SendNotify(NM_CUSTOMDRAW, (LPNMHDR)lpnmcd);
        return dwRet;
    }
    inline HBRUSH GetBkBrush()
    {
        if (!m_data.hbrBk)
            m_data.hbrBk = CreateSolidBrush(m_data.clrBk);
        return m_data.hbrBk;
    }
    inline HBRUSH GetDownBrush()
    {
        if (!m_data.hbrDown)
            m_data.hbrDown = CreateSolidBrush(m_data.clrDown);
        return m_data.hbrDown;
    }
    inline HBRUSH GetSelBrush()
    {
        if (!m_data.hbrSel)
            m_data.hbrSel = CreateSolidBrush(m_data.clrSel);
        return m_data.hbrSel;
    }
    inline HBRUSH GetTextBrush()
    {
        if (!m_data.hbrText)
            m_data.hbrText = CreateSolidBrush(m_data.clrText);
        return m_data.hbrText;
    }
    inline HBRUSH GetTextBkBrush()
    {
        if (!m_data.hbrTextBk)
            m_data.hbrTextBk = CreateSolidBrush(m_data.clrTextBk);
        return m_data.hbrTextBk;
    }
    inline HBRUSH GetHotBrush()
    {
        if (!m_data.hbrHotlight)
            m_data.hbrHotlight = CreateSolidBrush(m_data.clrHotlight);
        return m_data.hbrHotlight;
    }
    inline HBRUSH GetKillFocusBrush()
    {
        if (!m_data.hbrKillFocus)
            m_data.hbrKillFocus = CreateSolidBrush(m_data.clrKillFocus);
        return m_data.hbrKillFocus;
    }
    inline HPEN GetLinePen()
    {
        if (!m_data.hpenOutline)
            m_data.hpenOutline = CreatePen(PS_SOLID, 1, m_data.clrHotlight);
        return m_data.hpenOutline;
    }

    template<typename T, typename R>
    inline bool query(T a, R b) const
    {
        return (a & b) == b;
    }
private:
    inline static BOOL ListView_OnSetBkColor(CUSTOM_LISTDATA* plv, COLORREF clrBk)
    {
        if (plv->clrBk != clrBk)
        {
            if (plv->hbrBk)
            {
                DeleteBrush(plv->hbrBk);
                plv->hbrBk = NULL;
            }

            if (clrBk != CLR_NONE)
            {
                plv->hbrBk = CreateSolidBrush(clrBk);
                if (!plv->hbrBk)
                    return FALSE;
            }
            plv->clrBk = clrBk;
        }
        return TRUE;
    }
};

