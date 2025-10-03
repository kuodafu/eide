#include "../EWindow_Fne_Header.h"


LRESULT CALLBACK WndProc_MDIClient_EWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_MDIClient_WTWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_MDIClient_EWindow_Child2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// MDIClient下的窗口编辑器, MDIClient下的第二层子窗口, pData是第一层子窗口的数据
LPOBJSTRUCT _subclass_MDIClient_EWindow(HWND hWnd, LPOBJSTRUCT pData, MDICLIENT_ALL_WINDOW* mdi)
{
    hWnd = GetWindow(hWnd, GW_CHILD);   // MDIClient下的第二层子窗口
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_MDIClient_EWindow_Child2, true);
    if (!data)return 0;

    hWnd = GetWindow(hWnd, GW_CHILD);
    LPOBJSTRUCT ewindow = _subclass(hWnd, WndProc_MDIClient_EWindow, true);    // 窗口设计器, 下一层窗口就是设计器里的窗口了
    ewindow->draw.color.crBackGround = RGB(37, 37, 38);
    data->draw.color.hbrBackGround = s_info->pfnCreateBrush(data->draw.color.crBackGround);

    if (s_info->MY_MF_OWNERDRAW)
    {
        ewindow->drawScroll = ScrollCreate(ewindow->hWnd, ewindow->draw.color.crBorder,
                                           0,          // m_isSysListView, 如果是超级列表框的话这个值就是超级列表框的表项高度
                                           false,      // m_isSysTreeView, 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
                                           true,       // m_isCallSetScroll, 是否调用原来的设置滚动条位置函数
                                           false,      // isSubClass, 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
                                           true        // isReCalcSize, 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
        );
    }
    InsertStyle(ewindow->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    hWnd = GetWindow(hWnd, GW_CHILD);
    data = _subclass(hWnd, WndProc_MDIClient_WTWindow, false, false, 0);    // 设计器里的窗口


    return data;
}


LRESULT CALLBACK WndProc_MDIClient_EWindow_Child2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    switch (message)
    {
    case WM_PAINT:
        return WndProc_DefPaint(hWnd, message, wParam, lParam, pData);
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

// 窗口设计器消息循环
LRESULT CALLBACK WndProc_MDIClient_EWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
    if (!pObj || s_info->MY_MF_OWNERDRAW == 0)
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    LRESULT ret = 0;
    
    switch (message)
    {
    case WM_NCHITTEST:
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCCALCSIZE:
    case WM_NCPAINT:
    case WM_PAINT:
    case WM_CAPTURECHANGED:
    case WM_STYLECHANGING:
    case WM_STYLECHANGED:
    case WM_HSCROLL:
    case WM_VSCROLL:
    case WM_MOUSEHWHEEL:    // 横向滚动
    case WM_MOUSEWHEEL:     // 纵向滚动
    case SBM_SETSCROLLINFO:
    case SBM_SETRANGE:
    case SBM_SETPOS:
    {
        if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
        {
            return ret;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_DESTROY:
    {
        delete pData->drawScroll;
        pData->drawScroll = 0;
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

LRESULT CALLBACK WndProc_MDIClient_WTWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_NCHITTEST:
        return HTCLIENT;
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

