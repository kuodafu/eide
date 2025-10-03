#include "wnd_header.h"


LRESULT _Mouse_OnLDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT _Mouse_OnLUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT _Mouse_OnRDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT _Mouse_OnRUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT _Mouse_OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

LRESULT WndProc_OnMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
        return _Mouse_OnLDwon(hWnd, message, wParam, lParam, pData);
    case WM_LBUTTONUP:
        return _Mouse_OnLUp(hWnd, message, wParam, lParam, pData);
    case WM_MOUSEMOVE:
        return _Mouse_OnMove(hWnd, message, wParam, lParam, pData);
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
        return _Mouse_OnRDwon(hWnd, message, wParam, lParam, pData);
    case WM_RBUTTONUP:
        return _Mouse_OnRUp(hWnd, message, wParam, lParam, pData);
    case WM_MOUSEHOVER:

        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        break;
    case WM_MOUSELEAVE:
    {
        LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
        sysBtn->index = -1;
        if (sysBtn->state)
        {
            sysBtn->state = STATE::NORMAL;
            //UpdateSysbtn(pData);
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        break;
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT _Mouse_OnLDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    if (pt.y <= s_info->scale(SYSBTN_HEIGHT))
        return _sysbtn_lDown(hWnd, message, wParam, lParam, pData);
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT _Mouse_OnLUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    if (pt.y <= s_info->scale(SYSBTN_HEIGHT))
        return _sysbtn_lUp(hWnd, message, wParam, lParam, pData);
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT _Mouse_OnRDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    if (pt.y <= s_info->scale(SYSBTN_HEIGHT))
        return _sysbtn_rDwon(hWnd, message, wParam, lParam, pData);
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT _Mouse_OnRUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    if (pt.y <= s_info->scale(SYSBTN_HEIGHT))
        return _sysbtn_rUp(hWnd, message, wParam, lParam, pData);
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT _Mouse_OnMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    if (pt.y <= s_info->scale(SYSBTN_HEIGHT))
        return _sysbtn_mousemove(hWnd, message, wParam, lParam, pData);
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    if (sysBtn->index != -1)
    {
        sysBtn->index = -1;
        InvalidateRect(pData->hObj, &sysBtn->rcBtn, 0);
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}
