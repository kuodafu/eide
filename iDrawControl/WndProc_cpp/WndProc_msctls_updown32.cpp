#include "../EWindow_Fne_Header.h"
LRESULT CALLBACK WndProc_msctls_updown32(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void msctls_updown32_OnPaint_lr(HDC hdc, int cxClient, int cyClient, LPOBJSTRUCT pData);
void msctls_updown32_OnPaint_tb(HDC hdc, int cxClient, int cyClient, LPOBJSTRUCT pData);
void msctls_updown32_OnMouse_lr(HWND hWnd, UINT message, int x, int y, LPOBJSTRUCT pData);
void msctls_updown32_OnMouse_tb(HWND hWnd, UINT message, int x, int y, LPOBJSTRUCT pData);

inline void WINAPI _msctls_updown32_crChange(LPOBJSTRUCT pData)
{
    pData->draw.color.crBackGround = s_info->themeColor->crBackExtern();
    pData->draw.color.hbrBackGround = s_info->pfnCreateBrush(pData->draw.color.crBackGround);
}
LPOBJSTRUCT _subclass_msctls_updown32(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_msctls_updown32, reMoveBorder, isSetBack, reMoveClassStyle);
    _msctls_updown32_crChange(pData);
    pData->fnCrChange = _msctls_updown32_crChange;
    pData->draw.indexSel = 0;
    pData->draw.indexHot = 0;
    //pData->draw.color.crTextHot = RGB(85, 170, 255);
    //pData->draw.color.crTextDown = RGB(0, 151, 251);
    //pData->fnNotify = WndProc_OnNotify;
    return pData;
}

// 返回是否为左右方向是调节器
bool msctls_updown32_islr(LPOBJSTRUCT pData)
{
    return true;
}

LRESULT CALLBACK WndProc_msctls_updown32(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        HDC hdc = s_info->iDraw->BeginPaint(pData, ps, false, true);

        if (msctls_updown32_islr(pData))
            msctls_updown32_OnPaint_lr(hdc, ps.cxClient, ps.cyClient, pData);
        else
            msctls_updown32_OnPaint_tb(hdc, ps.cxClient, ps.cyClient, pData);

        s_info->iDraw->EndPaint(ps);
        break;
    }
    case WM_WINDOWPOSCHANGING:
    {
        LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
        if ((pos->flags & SWP_NOSIZE) == 0 || (pos->flags & SWP_NOMOVE) == 0)    // 改变了尺寸或者被移动了
        {
            //if (s_info->hCustomTab == GetParent(hWnd))
            //{
            //    pos->y -= 2;
            //    pos->cy += 2;
            //}

        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_MOUSELEAVE:
    case WM_MOUSEMOVE:
    case WM_LBUTTONUP:
    case WM_LBUTTONDOWN:
    {
        if (msctls_updown32_islr(pData))
            msctls_updown32_OnMouse_lr(hWnd, message, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), pData);
        else
            msctls_updown32_OnMouse_tb(hWnd, message, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), pData);

        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    default:
        //return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
}

inline void _draw_image(Gdiplus::Graphics& gp, LPWINDOW_ICON_DATA iconData, Gdiplus::RectF& rcDraw, RECT_F& rcIcon)
{
    if (iconData)
        gp.DrawImage(iconData->hBitmapGdip,
            rcDraw, rcIcon.left, rcIcon.top, rcIcon.width(), rcIcon.height(), Gdiplus::Unit::UnitPixel);

}
void msctls_updown32_OnMouse_lr(HWND hWnd, UINT message, int x, int y, LPOBJSTRUCT pData)
{
    const int mid = (pData->pos.width) / 2;
    const int index = (x > mid) ? 2 : 1;
    switch (message)
    {
    case WM_LBUTTONDOWN:
    {
        pData->draw.indexSel = index;
        pData->draw.indexHot = STATE::DOWN;
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case WM_LBUTTONUP:
    {
        pData->draw.indexSel = index;
        pData->draw.indexHot = STATE::HOT;
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (index == pData->draw.indexSel)
            break;
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.dwHoverTime = INFINITE;
        tme.hwndTrack = hWnd;
        TrackMouseEvent(&tme);

        pData->draw.indexSel = index;
        pData->draw.indexHot = STATE::HOT;
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case WM_MOUSELEAVE:
    {
        pData->draw.indexSel = -1;
        pData->draw.indexHot = STATE::NORMAL;
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    default:
        break;
    }

}
void msctls_updown32_OnMouse_tb(HWND hWnd, UINT message, int x, int y, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
    {

        break;
    }
    case WM_LBUTTONUP:
    {

        break;
    }
    case WM_MOUSEMOVE:
    {

        break;
    }
    case WM_MOUSELEAVE:
    {

        break;
    }
    default:
        break;
    }

}
void msctls_updown32_OnPaint_lr(HDC hdc, int cxClient, int cyClient, LPOBJSTRUCT pData)
{
    RECT_F rcLeft;
    RECT_F rcRight;
    

    STATE state1 = STATE::NORMAL;
    STATE state2 = STATE::NORMAL;

    if (pData->draw.indexSel == 1)  
    {
        state1 = (STATE)pData->draw.indexHot;
        state2 = STATE::NORMAL;
    }
    else
    {
        state1 = (STATE)STATE::NORMAL;
        state2 = (STATE)pData->draw.indexHot;
    }

    const int offset = 0;
    const int cxItem = offset + cxClient / 2;
    const int cyItem = cyClient;
    LPWINDOW_ICON_DATA hBitmapLeft = s_info->pResource->MsctlsUpdowm32_GetIcon_lr(L"0", &rcLeft, state1, s_info->dpiX);
    LPWINDOW_ICON_DATA hBitmapRight = s_info->pResource->MsctlsUpdowm32_GetIcon_lr(L"1", &rcRight, state2, s_info->dpiX);

    int cxIcon = hBitmapLeft ? (int)hBitmapLeft->cxIcon : 0;
    int cyIcon = hBitmapLeft ? (int)hBitmapLeft->cyIcon : 0;

    Gdiplus::Graphics gp(hdc);
    Gdiplus::RectF rcDrawLeft;
    Gdiplus::RectF rcDrawRight;

    rcDrawLeft.X = (float)((cxItem - cxIcon) / 2);
    rcDrawLeft.Y =  (float)((cyItem - cyIcon) / 2);
    rcDrawLeft.Width = (float)cxIcon;
    rcDrawLeft.Height = (float)cyIcon;

    rcDrawRight.X =  (float)(cxItem + (cxItem - cxIcon) / 2);
    rcDrawRight.Y =  (float)((cyItem - cyIcon) / 2);
    rcDrawRight.Width = (float)cxIcon;
    rcDrawRight.Height = (float)cyIcon;

    _draw_image(gp, hBitmapLeft, rcDrawLeft, rcLeft);
    _draw_image(gp, hBitmapRight, rcDrawRight, rcRight);
}

void msctls_updown32_OnPaint_tb(HDC hdc, int cxClient, int cyClient, LPOBJSTRUCT pData)
{
    RECT_F rcLeft;
    RECT_F rcRight;
    STATE state1 = STATE::NORMAL;
    STATE state2 = STATE::NORMAL;

    const int cxItem = cxClient / 2;
    const int cyItem = cyClient;
    LPWINDOW_ICON_DATA hBitmapLeft = s_info->pResource->MsctlsUpdowm32_GetIcon_tb(L"0", &rcLeft, state1, s_info->dpiX);
    LPWINDOW_ICON_DATA hBitmapRight = s_info->pResource->MsctlsUpdowm32_GetIcon_tb(L"1", &rcRight, state2, s_info->dpiX);
    int cxIcon = hBitmapLeft ? (int)hBitmapLeft->cxIcon : 0;
    int cyIcon = hBitmapLeft ? (int)hBitmapLeft->cyIcon : 0;

    Gdiplus::Graphics gp(hdc);
    Gdiplus::RectF rcDrawLeft;
    Gdiplus::RectF rcDrawRight;

    rcDrawLeft.X = (float)((cxItem - cxIcon) / 2);
    rcDrawLeft.Y = (float)((cyItem - cyIcon) / 2);
    rcDrawLeft.Width = (float)cxIcon;
    rcDrawLeft.Height = (float)cyIcon;

    rcDrawRight.X = (float)(cxItem + (cxItem - cxIcon) / 2);
    rcDrawRight.Y = (float)((cyItem - cyIcon) / 2);
    rcDrawRight.Width = (float)cxIcon;
    rcDrawRight.Height = (float)cyIcon;

    _draw_image(gp, hBitmapLeft, rcDrawLeft, rcLeft);
    _draw_image(gp, hBitmapRight, rcDrawRight, rcRight);
}