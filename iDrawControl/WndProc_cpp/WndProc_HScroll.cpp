#include "../EWindow_Fne_Header.h"
LRESULT CALLBACK WndProc_HScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);


LPOBJSTRUCT _subclass_MDIClient_HScroll(HWND hWnd, LPOBJSTRUCT pMDIClientChild1Data, bool reMoveBorder)
{
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_HScroll, true);
    if (!data)return 0;
    data->draw.isNcCalcSize = true;
    data->draw.isNoNcPaint = true;
    data->param2 = pMDIClientChild1Data->param2;
    data->draw.offsetRect.top += 18;

    if (!data->drawScroll)
    {
        MDICLIENT_ALL_WINDOW* mdi = (MDICLIENT_ALL_WINDOW*)pMDIClientChild1Data->param2;
        LPOBJSTRUCT pCodeWnd = GetDataFromHwnd(mdi->hCode);
        if (pCodeWnd)
        {
            data->drawScroll = pCodeWnd->drawScroll;
            IDrawScroll::SetobjFromHwnd(data->hWnd, (IDrawScroll*)data->drawScroll);
            IDrawScroll::SetBindControl(data->hWnd, true, false);
            ((IDrawScroll*)data->drawScroll)->m_hWndHBind = data->hWnd;
        }
    }

    return data;
}

LRESULT CALLBACK WndProc_HScroll(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    //IDrawScroll* pObj = pData->drawScroll;
    //if (!pObj) return CallWindowProcW(pData->oldProc, hWnd, message, wParam, lParam);
    //LRESULT ret = 0;
    //if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
    //{
    //    return ret;
    //}
    switch (message)
    {
    case SBM_ENABLE_ARROWS:
    case SBM_GETPOS:
    case SBM_GETRANGE:
    case SBM_GETSCROLLBARINFO:
    case SBM_GETSCROLLINFO:
    case SBM_SETPOS:
    case SBM_SETRANGE:
    case SBM_SETRANGEREDRAW:
    case SBM_SETSCROLLINFO:
        //case WM_LBUTTONDOWN:
        //case WM_LBUTTONUP:
        //case WM_MOUSEMOVE:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    case WM_WINDOWPOSCHANGING:
    {
        LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
        if ((pos->flags & SWP_NOSIZE) == 0 || (pos->flags & SWP_NOMOVE) == 0)    // 改变了尺寸或者被移动了
        {
            pos->x = -1;
            pos->y = -1;
            pos->cx = 1;
            pos->cy = 1;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}