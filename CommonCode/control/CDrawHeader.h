#pragma once
#include "CDrawControl.h"

class CDrawHeader : public CDrawControl
{
    int m_indexHot; 
    int m_indexDown; 
    HFONT hfontSortArrow;
    int m_char_width;
protected:

public:
    CDrawHeader() : CDrawControl(), m_indexHot(-1), m_indexDown(-1), hfontSortArrow(0), m_char_width(0)
    {

    }
    virtual ~CDrawHeader()
    {

    }

public:
    // 绑定组件, 绑定后会子类化组件, 然后对组件进行重画操作
    // hWnd = 绑定的窗口, 如果为0则不子类化窗口, 只记录配色信息, 可以在外部调用指定方法处理
    // pClr = 窗口使用的配色结构, 如果为0会从当前进程的map里获取, 获取失败则使用默认色绘画
    // isRef = 参数2是否一直有效, 参数为真的时候内部不会分配配色结构, 会一直使用参数2, 所以为真时请确保参数2指针的存活
    virtual bool bind(HWND hWnd = 0, PTHEMECOLOR_STRUCT pClr = 0, bool isRef = false)
    {
        HDC hdc = GetDC(hWnd);

        TEXTMETRIC tm;
        GetTextMetrics(hdc, &tm);
        int cy = ((tm.tmHeight + tm.tmExternalLeading + GetSystemMetrics(SM_CYBORDER)) & 0xFFFE) - 1;
        hfontSortArrow = CreateFontW(cy, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, L"Marlett");
        HGDIOBJ hOldFont = SelectObject(hdc, hfontSortArrow);
        GetTextMetrics(hdc, &tm);
        m_char_width = tm.tmAveCharWidth;
        SelectObject(hdc, hOldFont);
        ReleaseDC(hWnd, hdc);
        return CDrawControl::bind(hWnd, pClr, isRef);
    }


    virtual LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if ((wParam & MK_LBUTTON) == 0)
        {
            const int len = Header_GetItemCount(hWnd);
            RECT rcItem;
            for (int i = 0; i < len; i++)
            {
                Header_GetItemRect(hWnd, i, &rcItem);
                if (x >= rcItem.left && x < rcItem.right)
                {
                    // 鼠标移动到某个项目中
                    TRACKMOUSEEVENT tme = { 0 };
                    tme.cbSize = sizeof(TRACKMOUSEEVENT);
                    tme.dwFlags = TME_LEAVE;
                    tme.dwHoverTime = INFINITE;
                    tme.hwndTrack = hWnd;
                    TrackMouseEvent(&tme);
                    if (m_indexHot != i)
                    {
                        m_indexHot = i;
                        ::InvalidateRect(hWnd, 0, 0);
                    }
                    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
                }
            }

            if (m_indexHot != -1)
                ::InvalidateRect(hWnd, 0, 0);
            m_indexHot = -1;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }
    virtual LRESULT OnMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        switch (message)
        {
        case WM_MOUSEMOVE:
            return OnMouseMove(hWnd, message, wParam, lParam, oldProc);
        case WM_MOUSELEAVE:
        {
            if (m_indexHot != -1)
                InvalidateRect(hWnd, 0, 0);
            m_indexHot = -1;
            break;
        }
        case WM_LBUTTONDOWN:
        {
            if (m_indexHot != -1)
            {
                m_indexDown = m_indexHot;
                ::InvalidateRect(hWnd, 0, 0);
            }
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        case WM_CAPTURECHANGED:
        case WM_LBUTTONUP:
        {
            if (m_indexHot != -1)
                ::InvalidateRect(hWnd, 0, 0);
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            const int len = Header_GetItemCount(hWnd);
            if (len > 0)
            {
                RECT rcFirst, rcLast;
                Header_GetItemRect(hWnd, 0, &rcFirst);
                Header_GetItemRect(hWnd, len - 1, &rcLast);
                if (y < 0 || y > rcFirst.bottom - rcFirst.top || x < rcFirst.left || x > rcLast.right)
                {
                    m_indexHot = -1;
                    ::InvalidateRect(hWnd, 0, 0);
                }
            }
            m_indexDown = -1;
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        default:
            break;
        }
        
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    virtual LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        RECT rcItem, rc;
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        const int cxClient = rc.right - rc.left;
        const int cyClient = rc.bottom - rc.top;
        LPMEMDCBITMAP md = md_create(cxClient, cyClient);
        HDC hdc = md->hdc;

        const int nItemCount = Header_GetItemCount(hWnd);
        int fmt = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
        COLORREF clrBack = RGB(255, 255, 255);
        COLORREF clrText = RGB(0, 0, 0);
        COLORREF clrLine = RGB(240, 240, 240);

        if (m_pClr)
            clrBack = m_pClr->crBack, clrText = m_pClr->crText, clrLine = m_pClr->crBorder;
        FillRect(hdc, &rc, CThemeColor::GetBrush(clrBack));
        SetTextColor(hdc, clrText);
        SetBkMode(hdc, TRANSPARENT);

        HFONT hFont = (HFONT)SendMessageW(hWnd, WM_GETFONT, 0, 0);
        if (!hFont)
            hFont = CWndBase::DefaultFont();
        SelectObject(hdc, hFont);
        HPEN hPenLine = CThemeColor::GetPen(PS_SOLID, 1, clrLine);
        SelectObject(hdc, hPenLine);

        const int bufSize = 0x1000;
        wchar_t text[bufSize];
        HDITEMW item = { 0 };
        item.mask = HDI_FORMAT | HDI_STATE | HDI_TEXT | HDI_WIDTH;
        item.pszText = text;
        item.cchTextMax = bufSize;


        for (int i = 0; i < nItemCount; i++)
        {
            Header_GetItemRect(hWnd, i, &rcItem);
            Header_GetItem(hWnd, i, &item);
            DrawItem(hdc, item, rcItem, i, nItemCount);

            HDITEMW hdItem = { 0 };
            hdItem.mask = HDI_FORMAT;
            SendMessageW(hWnd, HDM_GETITEM, i, (LPARAM)&hdItem);
            if ((hdItem.fmt & (HDF_SORTUP | HDF_SORTDOWN)) == 0)
                continue;
            BOOL fUpArrow = hdItem.fmt & HDF_SORTUP;
            LPCWSTR text = fUpArrow ? L"5" : L"6";

            int mid = rcItem.left + (rcItem.left + rcItem.right) / 2;
            POINT pt[3] =
            {
                { mid - 3, rcItem.top + 3 },
                { mid, rcItem.top },
                { mid + 3, rcItem.top + 3 },
            };
            MoveToEx(hdc, pt[0].x, pt[0].y, 0);
            LineTo(hdc, pt[1].x, pt[1].y);
            LineTo(hdc, pt[2].x, pt[2].y);
            //DrawTextW(hdc, text, 1, &rcItem, DT_TOP | DT_CENTER);

            int bkMode = SetBkMode(hdc, TRANSPARENT);
            COLORREF cr = SetTextColor(hdc, clrText);
            HFONT hFontOld = (HFONT)SelectObject(hdc, hfontSortArrow);

            const int cyItem = rcItem.bottom - rcItem.top;
            const int cxItem = rcItem.right - rcItem.left;
            ExtTextOutW(hdc, rcItem.left + (cxItem - m_char_width) / 2, -5, 0, &rcItem, fUpArrow ? L"5" : L"6", 1, NULL);

            SetTextColor(hdc, cr);
            SetBkMode(hdc, bkMode);

            SelectObject(hdc, hFontOld);

        }

        BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        md_destroy(md);
        return 0;
    }

    void DrawItem(HDC hdc, HDITEMW& item, RECT& rcItem, int index, int nItemCount) const
    {
        COLORREF clrBack = RGB(255, 255, 255);
        COLORREF clrHot = RGB(217, 235, 249);
        COLORREF clrDown = RGB(188, 220, 244);
        COLORREF clrLine = RGB(240, 240, 240);
        if (m_pClr)
        {
            clrBack = m_pClr->crBack;
            clrHot =  m_pClr->crHot;
            clrDown = m_pClr->crDown;
            clrLine = m_pClr->crBorder;
        }
        if (index == m_indexDown)
            FillRect(hdc, &rcItem, CThemeColor::GetBrush(clrDown));
        else if (index == m_indexHot)
            FillRect(hdc, &rcItem, CThemeColor::GetBrush(clrHot));



        HPEN hPen = CThemeColor::GetPen(PS_INSIDEFRAME, 1, clrLine);

        HGDIOBJ oldPen = SelectObject(hdc, hPen);

        const int offset = 0;// nItemCount != index + 1;
        MoveToEx(hdc, rcItem.right - offset, rcItem.top, 0);
        LineTo(hdc, rcItem.right - offset, rcItem.bottom);


        SelectObject(hdc, oldPen); // 还原DC

        rcItem.left += 5;
        rcItem.top += 2;
        if (index == m_indexHot && m_indexDown != -1)
            OffsetRect(&rcItem, 1, 1);

        DrawTextW(hdc, item.pszText, -1, &rcItem, DT_VCENTER | DT_SINGLELINE);
        rcItem.left -= 5;
        rcItem.top -= 2;
        if (index == m_indexHot && m_indexDown != -1)
            OffsetRect(&rcItem, -1, -1);
    }

};

