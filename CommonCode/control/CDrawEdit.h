#pragma once
#include "CDrawControl.h"

class CDrawEdit : public CDrawControl
{
protected:

public:
    CDrawEdit() : CDrawControl()
    {

    }
    virtual ~CDrawEdit()
    {

    }

public:



    virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        switch (message)
        {
        case WM_NCCALCSIZE:
            return OnNcCalcSize(hWnd, message, wParam, lParam, oldProc);
        case WM_NCPAINT:
            return OnNcPaint(hWnd, message, wParam, lParam, oldProc);
        default:
            return CDrawControl::WndProc(hWnd, message, wParam, lParam, oldProc);
        }
        return 0;
    }

    // �ػ��༭��ǿͻ���
    virtual LRESULT OnNcPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        return OnNcPaint(hWnd, message, wParam, lParam, oldProc, m_pClr);
    }

    static LRESULT OnNcPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc, PTHEMECOLOR_STRUCT pClr)
    {
        RECT rcBorder, rcClient, rcWnd;
        CWndBase::GetRect(hWnd, &rcBorder, &rcWnd, 0, &rcClient);

        const int cxClient = rcClient.right - rcClient.left;
        const int cyClient = rcClient.bottom - rcClient.top;
        const int cxScreen = rcWnd.right - rcWnd.left;
        const int cyScreen = rcWnd.bottom - rcWnd.top;

        rcClient.left = rcClient.left - rcWnd.left;
        rcClient.top = rcClient.top - rcWnd.top;
        rcClient.right = rcClient.left + cxClient;
        rcClient.bottom = rcClient.top + cyClient;

        rcWnd = { 0, 0, cxScreen, cyScreen };

        RECT rcRgn2 = { rcClient.left, rcClient.top, rcClient.right, rcClient.bottom };

        HDC hdc = GetWindowDC(hWnd);
        HRGN hRgn1 = CreateRectRgn(0, 0, cxScreen, cyScreen);
        HRGN hRgn2 = CreateRectRgn(rcRgn2.left, rcRgn2.top, rcRgn2.right, rcRgn2.bottom);
        HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
        int ret = CombineRgn(hRgn, hRgn1, hRgn2, RGN_DIFF);

        COLORREF clrBack = RGB(255, 255, 255);
        COLORREF clrBorder = RGB(0, 0, 0);
        COLORREF clrBorderHot = RGB(0, 0, 0);

        if (pClr)
        {
            clrBack = pClr->crBack;
            clrBorder = pClr->crBorder;
            clrBorderHot = pClr->crBorderHot;
        }

        SelectClipRgn(hdc, hRgn);
        FillRect(hdc, &rcWnd, GetBrush(clrBack));

        if (0)  // ������, �����ú�ɫ�ı������׿�Ч��
        {
            HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0));
            FillRect(hdc, &rcWnd, hbr);
            DeleteObject(hbr);
        }

        static auto IsBorder = [](HWND hWnd) -> bool
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
        };

        if (IsBorder(hWnd))
        {
            //wstr::dbg(L"���㴰�� = %p, ��ǰ�ػ��༭�򴰿� = %p\n", GetFocus(), hWnd);
            clrBorder = GetFocus() == hWnd ? clrBorderHot : clrBorder;
            //clrBorder = GetFocus() == hWnd ? 255 : g_color->crBorder();
            HPEN hPen = GetPen(PS_SOLID, 1, clrBorder);
            HGDIOBJ hOldPen = SelectObject(hdc, hPen);
            HGDIOBJ hOldBrush = SelectObject(hdc, GetStockBrush(NULL_BRUSH));

            Rectangle(hdc, 0, 0, cxScreen, cyScreen);
            SelectObject(hdc, hOldBrush);
            SelectObject(hdc, hOldPen);
        }



        DeleteObject(hRgn1);
        DeleteObject(hRgn2);
        DeleteObject(hRgn);

        SelectClipRgn(hdc, 0);
        ReleaseDC(hWnd, hdc);
        return 0;
    }
    // ����ǿͻ����ߴ�, �������������ı��߶�, Ȼ����д���
    static LRESULT OnNcCalcSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        if (!wParam)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);

        LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
        const int id = GetDlgCtrlID(hWnd);

        HDC hdc = GetDC(hWnd);
        HFONT hFont = (HFONT)SendMessageW(hWnd, WM_GETFONT, 0, 0);
        if (!hFont)
            hFont = CWndBase::DefaultFont();

        HGDIOBJ hOldFont = SelectObject(hdc, hFont);
        RECT rcText = { 0, 0, 100, 20 };
        DrawTextW(hdc, L"����", 2, &rcText, DT_CALCRECT);
        SelectObject(hdc, hOldFont);
        ReleaseDC(hWnd, hdc);
        const int nHeightText = rcText.bottom - rcText.top;

        // �༭��߶�, �����ǿͻ���
        //RECT rc;
        //GetWindowRect(hWnd, &rc);
        //if (rc.bottom - rc.top <= 0)
        //    rc.top = 0, rc.bottom = 20;
        //
        int cyScreen = cs->rgrc[0].bottom - cs->rgrc[0].top;
        int offsetTop = (cyScreen - nHeightText) / 2;
        if (offsetTop == 0)
            offsetTop = 1;
        cs->rgrc[0].top += offsetTop;
        cs->rgrc[0].left += 2;   // ����ƫ��һ��
        cs->rgrc[0].right -= 2;
        cs->rgrc[0].bottom -= offsetTop;
        return 0;
    }

};

