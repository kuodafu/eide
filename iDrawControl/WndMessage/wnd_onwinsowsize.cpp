#include <control/CToolBar.h>
#include <windowsx.h>
#include "wnd_header.h"
#define EWF_SIZED 0x0008
#define EWF_bRedrawBackground 0x0020
#define RGN_COMBINE_EXCLUDE 3





void _wnd_recalcclient(LPOBJSTRUCT pData, int cx, int cy)
{
    pData->pos.width = cx;
    pData->pos.height = cy;
    _canvas_resize(pData->draw.hCanvas, cx, cy);

    //_rgn_destroy(pData->hrgn_client);
    //_rgn_destroy(pData->hrgn_sizebox);
    int nSizebox = 0;
    if (__query(pData->style, EWS_SIZEABLE))
    {
        nSizebox = 5;
    }

    //int rectround = pData->rectround;
    //pData->hrgn_client = _rgn_createfromroundrect(nSizebox, nSizebox,
    //    cx - nSizebox, cy - nSizebox, rectround, rectround);

    //if (nSizebox)
    //{
    //    //int hRgnNC = _rgn_createfromrect(0, 0, cx, cy);
    //    //pData->hrgn_sizebox = _rgn_combine(hRgnNC, pData->hrgn_client, RGN_COMBINE_EXCLUDE, 0, 0);
    //    //_rgn_destroy(hRgnNC);
    //}

    //if (rectround)
    //{
    //    HRGN hRgn = CreateRoundRectRgn(0, 0, cx, cy, rectround, rectround);
    //    SetWindowRgn(pData->hObj, hRgn, 1);
    //    DeleteObject(hRgn);
    //}

}

void _wnd_calc_captionrect(LPOBJSTRUCT pData, RECT& rc)
{

}

LRESULT WndProc_OnSize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}


LRESULT WndProc_OnNcCalcsize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    if (wParam)
    {
        LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
        memcpy(&cs->rgrc[1], &cs->rgrc[0], sizeof(cs->rgrc[0]));
        memcpy(&cs->rgrc[2], &cs->rgrc[0], sizeof(cs->rgrc[0]));
        return WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;
    }
    return 0;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}
LRESULT WndProc_OnWindowPosChanging(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    //static int cx = GetSystemMetrics(SM_CXSCREEN);
    LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
    
    if ((pos->flags & SWP_NOSIZE) == 0)    // 改变了尺寸
    {
        //pos->cy = pData->pos.rc.bottom;
        //if (pos->x = -8 && pos->y == -8)
        //{
        //    pos->x += 8;
        //    pos->y += 8;
        //    pos->cx -= 16;
        //    pos->cy -= 16;
        //}
        //return 0;
    }
    if ((pos->flags & SWP_NOMOVE) == 0)    // 被移动了
    {
        pData->pos.rc.left = pos->x;
        pData->pos.rc.top = pos->y;
        return 0;
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}
LRESULT WndProc_OnWindowPosChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

