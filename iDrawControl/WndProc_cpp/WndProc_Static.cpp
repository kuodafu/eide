#include "../EWindow_Fne_Header.h"
LRESULT CALLBACK WndProc_Static(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_StaticCtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

LPOBJSTRUCT _subclass_Static(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_Static, reMoveBorder, isSetBack, reMoveClassStyle);
    pData->draw.isTransparent = true;
    pData->draw.isErasebkg = true;
    pData->fnCtlcolor = WndProc_StaticCtlColor;
    return pData;
}

// 根据样式计算文本格式
inline int _static_calc_format(DWORD style)
{
#define _s_query(_v) ((style & _v) == _v)

    int fmt = 0;
    if (_s_query(SS_CENTER))        fmt = DT_CENTER;
    else if (_s_query(SS_RIGHT))    fmt = DT_RIGHT;
    else                            fmt = DT_LEFT;
    if (_s_query(SS_CENTERIMAGE))   fmt |= DT_VCENTER | DT_SINGLELINE;
    if (_s_query(SS_PATHELLIPSIS))  fmt |= DT_PATH_ELLIPSIS;
    if (_s_query(SS_ENDELLIPSIS))   fmt |= DT_END_ELLIPSIS;
    if (_s_query(SS_EDITCONTROL))   fmt |= DT_EDITCONTROL;
    if (fmt == 0)
        fmt = DT_VCENTER | DT_SINGLELINE;
    return fmt;
}
LRESULT CALLBACK WndProc_Static(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, pData->draw.color.hbrBackGround);
        SetBkColor(hdc, pData->draw.color.crBackGround);
        SetTextColor(hdc, pData->draw.color.crText);
        if (pData->draw.isTransparent)
            SetBkMode(hdc, TRANSPARENT);

        return 1;
    }
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        HDC hdc = s_info->iDraw->BeginPaint(pData, ps, false, true);

        int len = GetWindowTextLengthW(hWnd) + 1;
        wstr text(len);
        GetWindowTextW(hWnd, text.data(), len);
        const int fmt = _static_calc_format(pData->style);
        DrawTextW(hdc, text.c_str(), len - 1, &ps.rc, fmt);
        s_info->iDraw->EndPaint(ps);
        break;
    }
    case WM_NCPAINT:
        return WndProc_DefNcPaint(hWnd, message, wParam, lParam, pData);
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
LRESULT CALLBACK WndProc_StaticCtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    HWND hChild = (HWND)lParam;
    LPOBJSTRUCT pChild = GetDataFromHwnd(hChild);
    if (!pChild)
        return pData->fnCallProc(pData->oldProc, hWnd, message, wParam, lParam);
    HDC hdc = (HDC)wParam;
    SetBkColor(hdc, pData->draw.color.crBackGround);
    SetTextColor(hdc, pData->draw.color.crText);
    return (LRESULT)pChild->draw.color.hbrBackGround;
}
