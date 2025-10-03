#include "../EWindow_Fne_Header.h"
LRESULT CALLBACK WndProc_StatusBar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

LPOBJSTRUCT _subclass_statusBar(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    LPOBJSTRUCT data = s_info->iDraw->_subclass_tool(hWnd, WndProc_StatusBar, reMoveBorder, isSetBack, reMoveClassStyle);
    return data;
}


void StatusBar_drawItem(HDC hdc, int i, LPOBJSTRUCT pData)
{
    RECT rcItem;
    SendMessageW(pData->hWnd, SB_GETRECT, i, (LPARAM)&rcItem);
    const int len = SendMessageW(pData->hWnd, SB_GETTEXTLENGTHW, i, 0);
    const int hi = HIWORD(len);
    wstr text(LOWORD(len) + 1);
    SendMessageW(pData->hWnd, SB_GETTEXT, i, (LPARAM)text.data());

    if (i == 0)
    {
        rcItem.left += 5;
        rcItem.right -= 10;
    }
    DrawTextW(hdc, text.data(), -1, &rcItem, DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_WORD_ELLIPSIS);

}

inline static LRESULT __status_update_eWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    if (s_info->rcStatusBar.right - s_info->rcStatusBar.left > 0 && s_info->rcStatusBar.bottom - s_info->rcStatusBar.top > 0)
    {
        RECT rcDraw = s_info->rcStatusBar;
        GetClientRect(s_info->hEWnd, &rcDraw);
        rcDraw.top = s_info->rcStatusBar.top;
        InvalidateRect(s_info->hEWnd, &rcDraw, 0);
    }
    // 每次状态夹文本被修改的时候都判断一下提示框是否显示
    bool isVisible = (IsWindowVisible(s_info->eWnd->GetStateTabTips()));
    if (!isVisible)
    {
        int index = TabCtrl_GetCurSel(s_info->hTabStateLeft);
        TCITEMW item = { 0 };
        item.mask = TCIF_PARAM;
        TabCtrl_GetItem(s_info->hTabStateLeft, index, &item);
        if (item.lParam != TAB_PARAM_TIPS)
        {
            index = TabCtrl_GetCurSel(s_info->hTabStateRight);
            item.mask = TCIF_PARAM;
            TabCtrl_GetItem(s_info->hTabStateRight, index, &item);
        }
        if (item.lParam == TAB_PARAM_TIPS)
        {
            // 当前选中的有提示框, 但是没有显示, 需要显示出来
            ShowWindowEx(s_info->eWnd->GetStateTabTips(), SW_SHOW);
        }
    }

    return 0;
}
LRESULT CALLBACK WndProc_StatusBar(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	switch (message)
	{
    case WM_ERASEBKGND:
        return 1;
    case WM_SETTEXT:
        if (hWnd == s_info->hStatusBar)
            __status_update_eWindow(hWnd, message, wParam, lParam, pData);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    case WM_WINDOWPOSCHANGING:
    {
        if (hWnd == s_info->hStatusBar)
        {
            LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
            pos->hwndInsertAfter = HWND_BOTTOM;
            pos->x = s_info->rcMDIClient.left + 10;
            pos->y = s_info->rcMDIClient.bottom - 50;
            pos->cx = s_info->rcMDIClient.right - s_info->rcMDIClient.left - 20;
            pos->cy = 20;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_PAINT:
    {
        if (hWnd == s_info->hStatusBar)
            __status_update_eWindow(hWnd, message, wParam, lParam, pData);

        BEGIN_END_PAINT_STRUCT ps;
        HDC hdc = s_info->iDraw->BeginPaint(pData, ps, false, false);
        if (hWnd == s_info->hStatusBar)
        {
            FillRect(hdc, &ps.rc, s_info->pWnd->draw.color.hbrBackGround);
        }
        else
        {
            FillRect(hdc, &ps.rc, pData->draw.color.hbrBackGround);
            const int len = SendMessageW(hWnd, SB_GETPARTS, 0, 0);
            for (int i = 0; i < len; i++)
            {
                StatusBar_drawItem(hdc, i, pData);
            }
        }

        s_info->iDraw->EndPaint(ps);
        break;
    }
    case WM_MOUSEMOVE:
        return 0;
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
	}
    return 0;
}