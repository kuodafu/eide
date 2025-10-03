#include "EMenu.h"
#include "EWindow.h"

iDrawScale g_scale;

LRESULT Scale_DPIChanged(UINT dpiX, UINT dpiY, RECT* rcWindow, LPOBJSTRUCT pData)
{
    const UINT oldDpi = s_info->dpiX;
    UINT dpi = dpiX;
    const float scale = (float)dpi / 96.f;

    g_scale.rect(s_info->rcWndMenu, dpi);
    //_rescale_rect(s_info->rcMenu);
    //s_info->rcMenu.left     = (int)((float)(s_info->rcMenu.left) * scale);
    //s_info->rcMenu.bottom   = (int)((float)(s_info->rcMenu.left) * scale);


    //_scale_rect(s_info->rcMenu              , dpi);
    g_scale.rect(s_info->rcCustomTab, dpi);
    g_scale.rect(s_info->rcState, dpi);
    g_scale.rect(s_info->rcStatusBar, dpi);
    g_scale.rect(s_info->rcMDIClient, dpi);
    g_scale.rect(s_info->rcControl, dpi);
    g_scale.rect(s_info->rcWork, dpi);

    g_scale.rect(s_info->rcControl1, dpi);
    g_scale.rect(s_info->rcState1, dpi);
    g_scale.rect(s_info->rcWork1, dpi);

    s_info->dpiX = dpiX;
    s_info->dpiY = dpiY;

    std::map<HWND, OBJSTRUCT>& map = _get_subclass_allWindow();
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        OBJSTRUCT& data = it->second;
        if (data.hWnd != pData->hWnd)
        {
            SendMessageW(data.hWnd, WM_DPICHANGED, MAKELONG(dpiX, dpiY), (LPARAM)rcWindow);
        }
    }
    int cxTool = s_info->rcWndMenu.bottom - s_info->rcWndMenu.top - WINDOW_BORDER_WIDTH;
    SendMessageA(s_info->eWnd->GetMenuStandard(), TB_SETBUTTONSIZE, 0, MAKELONG(cxTool, cxTool));
    SendMessageW(s_info->hWndTool, TB_SETBUTTONSIZE, 0, (LPARAM)MAKELONG(cxTool, cxTool));


    // 根据新字体重新计算菜单显示尺寸
    LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    HDC hdc = CreateCompatibleDC(0);
    SelectObject(hdc, hFont->GetFont());
    int width = __menu_calc_rect(hdc, s_info->menuDoc, false);
    DeleteDC(hdc);

    const int caption_height = s_info->scale(WINDOW_CAPTION_HEIGHT);
    s_info->rcMenu = { caption_height, 1, width, caption_height - 1 };
    // 重新计算标题显示位置
    _window_recalc_title(pData, true);

    RECT& rc = *rcWindow;
    if (s_info->config->IsDPIScaleWnd())
        SetWindowPos(pData->hWnd, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
    else
        SendEWindowSize(s_info->pWnd);

    wstr::dbg(L"--------DPI被改变 dpi = %d, 建议的位置 = {%d, %d, %d, %d} [%d x %d], 工具条大小 = %d\n",
        dpiX, rc.left, rc.top, rc.right, rc.bottom, rc.right - rc.left, rc.bottom - rc.top, cxTool);

    return 0;
}
