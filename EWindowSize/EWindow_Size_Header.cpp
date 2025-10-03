#include "EWindow_Size_Header.h"


wstr Window_Get_Title(HWND hWnd)
{
    wstr ret;
    int len = GetWindowTextLengthW(hWnd) + 1;
    ret.resize(len);

    len = GetWindowTextW(hWnd, ret.data(), len);
    ret.resize(len);

    return ret;
}

void MergeRECT(RECT& rcFather, RECT rcChild)
{
    rcFather.left += rcChild.left;
    rcFather.top += rcChild.top;
    rcFather.right += rcChild.right;
    rcFather.bottom += rcChild.bottom;
}



HDWP _EWINDOW_NAMESPACE EWindow_DeferWindowPos(HDWP& hDwp, HWND hWnd, const RECT& rc, bool bRepaint)
{
    if (!hWnd)return hDwp;
    //const DWORD flags = SWP_NOZORDER | (bRepaint ? SWP_DRAWFRAME : SWP_NOREDRAW);
    //SetWindowPos(hWnd, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, flags);

    MoveWindowEx(hWnd, rc, bRepaint);
    //hDwp = DeferWindowPos(hDwp, hWnd, hWnd, rc.left, rc.top,
    //    rc.right - rc.left, rc.bottom - rc.top,
    //    SWP_NOZORDER | (bRepaint ? 0 : SWP_NOREDRAW));
    return hDwp;
}
// 指定窗口是否自动隐藏
bool _EWINDOW_NAMESPACE IsAutoHide(HWND hWnd, LPOBJSTRUCT* ppData, LPTOPSTATE_INFO* pInfo)
{
    LPOBJSTRUCT pData = 0;
    if (hWnd == s_info->hWork)
    {
        static LPOBJSTRUCT pWork = s_info->pfnGetDataFromHwnd(hWnd);
        pData = pWork;
    }
    else if (hWnd == s_info->hState)
    {
        static LPOBJSTRUCT pState = s_info->pfnGetDataFromHwnd(hWnd);
        pData = pState;
    }
    else if (hWnd == s_info->hControl)
    {
        static LPOBJSTRUCT pControl = s_info->pfnGetDataFromHwnd(hWnd);
        pData = pControl;
    }
    if (!pData) return false;
    if (!pData)
    {
        //wstr str;
        //str.Format(L"判断窗口是否是自动隐藏失败, 窗口数据失败");
        MessageBoxW(s_info->hEWnd, L"判断窗口是否是自动隐藏失败, 窗口数据失败, 点击确定后抛出异常", L"错误", 0);
        throw;
    }
    LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pData->param;
    if (ppData) *ppData = pData;
    if (pInfo) *pInfo = info;
    return __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE);
}

