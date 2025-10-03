#include "ImageExt.h"


static Gdiplus::Pen* s_pen;

LRESULT CALLBACK WndProc_static_save(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        if (!s_pen)s_pen = new Gdiplus::Pen(MAKEARGB(255, 0, 0, 0));

        PAINTSTRUCT ps;
        RECT rc;
        BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        const int cxClient = rc.right - rc.left;
        const int cyClient = rc.bottom - rc.top;
        LPMEMDCBITMAP md = md_create(cxClient, cyClient);
        HDC hdc = md->hdc;
        COLORREF crBack = str2rgb(s_editColor.GetText().c_str(), 0);

        HBRUSH hbr = CreateSolidBrush(crBack);
        FillRect(hdc, &rc, hbr);

        DeleteObject(hbr);
        BitBlt(ps.hdc, 0, 0, cxClient, cyClient, md->hdc, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        md_destroy(md);
        break;
    }
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}