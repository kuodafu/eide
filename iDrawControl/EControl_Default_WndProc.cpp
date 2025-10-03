#include "EMenu.h"
#include "EControl_WndProc.h"

static std::unordered_map<HWND, HFONT> s_map_font;  // 记录所有设置字体的组件, 在重新创建资源的时候把需要重新设置字体
static std::vector<HFONT> s_free_font;              // 记录所有需要释放的字体
#define GUID_FONT L"{0B314A44-2092-4C11-B653-B79685DB6B97}"

LRESULT CALLBACK WndProc_DefPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    RECT rc;
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    GetClientRect(hWnd, &rc);
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    const LPEX_FONT pFont = pData->draw.hFont ? pData->draw.hFont : s_info->defData->draw.hFont;    // 绘画字体
    const HFONT hFont = pFont->GetFont();    // 绘画字体

    LPMEMDCBITMAP md = md_create(cxClient, cyClient);
    assert(md);
    HDC hdc = md->hdc;  // 如果md=空, 就让他崩溃
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, pData->draw.color.crText);
    SelectObject(hdc, hFont);

    FillRect(hdc, &rc, pData->draw.color.hbrBackGround);

    BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
    EndPaint(hWnd, &ps);
    md_destroy(md);
    return 0;
}

LRESULT CALLBACK WndProc_DefNcPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    bool isBorder = false;
    if (pData->style & WS_BORDER)
        isBorder = true;
    if (!isBorder && (pData->styleEx & WS_EX_CLIENTEDGE)
        || (pData->styleEx & WS_EX_DLGMODALFRAME)
        || (pData->styleEx & WS_EX_WINDOWEDGE) )
        isBorder = true;

    if (!isBorder)  // 没有边框就默认处理
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    RECT rc;
    GetWindowRect(hWnd, &rc);
    const int cxScreen = rc.right - rc.left;
    const int cyScreen = rc.bottom - rc.top;
    HDC hdc = GetWindowDC(hWnd);
    HGDIOBJ hOldPen = SelectObject(hdc, pData->draw.color.hpenBorder);
    HGDIOBJ hOldBrush = SelectObject(hdc, GetStockBrush(NULL_BRUSH));

    Rectangle(hdc, 0, 0, cxScreen, cyScreen);   // 直接画到窗口DC上

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    ReleaseDC(hWnd, hdc);

    return 0;
}

LRESULT CALLBACK WndProc_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_CTLCOLOR:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    {
        const HDC hdc = (HDC)wParam;
        const HWND hControl = (HWND)lParam;
        LPOBJSTRUCT pCtlData = s_info->pfnGetDataFromHwnd(hControl);
        if (!pCtlData)
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

        if (pCtlData->draw.isTransparent)
            SetBkMode(hdc, TRANSPARENT);
        SetBkColor(hdc, pCtlData->draw.color.crBackGround);
        SetTextColor(hdc, pCtlData->draw.color.crText);
        return (LRESULT)pCtlData->draw.color.hbrBackGround;
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc_EraseBkgnd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
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
            SetBkMode((HDC)wParam, TRANSPARENT);

        return 1;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
}


void EWindow_DPIChange(LPOBJSTRUCT pData)
{
    LPEX_FONT hFont = (LPEX_FONT)GetPropW(pData->hWnd, GUID_FONT);
    if (!hFont)
    {
        HFONT hGdiFont = (HFONT)SendMessageW(pData->hWnd, WM_GETFONT, 0, 0);
        if (!hGdiFont)
            hGdiFont = pData->draw.hFont->GetFont();

        LOGFONTW lf = { 0 };
        GetObjectW(hGdiFont, sizeof(lf), &lf);
        if (lf.lfHeight < -12)
        {
            const float scale = (float)s_info->dpiX / 96.f;
            lf.lfHeight     = (int)((float)(lf.lfHeight) / scale);
        }
        hFont = s_info->pfnCreateFontLfDx(&lf);
        if (!hFont)
            hFont = pData->draw.hFont;
        SetPropW(pData->hWnd, GUID_FONT, hFont);
    }

    hFont = g_scale.GetFont(hFont);
    s_map_font[pData->hWnd] = hFont->GetFont();
    SendMessageW(pData->hWnd, WM_SETFONT, (WPARAM)hFont->GetFont(), 0);
}

inline void _free_obj()
{
    for (HFONT hFont : s_free_font)
    {
        DeleteObject(hFont);
    }
    s_free_font.clear();
    s_map_font.clear();
}
void EWindow_ResetFont()
{
    static bool first;
    if (!first)
    {
        first = true;
        IDraw_PushPluginFreeCallback(_free_obj);
    }
    for (auto item : s_map_font)
    {
        HFONT& hFont = item.second; // 新创建的字体需要保存, 这里使用引用
        HWND hWnd = item.first;
        LOGFONTW lf = { 0 };
        GetObjectW(hFont, sizeof(lf), &lf);
        hFont = CreateFontIndirectW(&lf);
        s_free_font.push_back(hFont);
        SetPropW(hWnd, GUID_FONT, 0);
        RemovePropW(hWnd, GUID_FONT);
        SendMessageW(hWnd, WM_SETFONT, (WPARAM)hFont, 1);
    }
}
