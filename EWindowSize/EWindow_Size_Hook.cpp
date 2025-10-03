#include "EWindow_Size_Header.h"



static apiHook hook_MoveWindow;
static apiHook hook_DeferWindowPos;


static BOOL(WINAPI* MoveWindow_hook)(HWND hWnd, int x, int y, int nWidth, int nHeight, BOOL bRepaint) = ::MoveWindow;
static HDWP(WINAPI* DeferWindowPos_hook)(HDWP hWinPosInfo, HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) = ::DeferWindowPos;


static HDWP WINAPI MyDeferWindowPos_hook(HDWP hWinPosInfo, HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
#ifdef _DEBUG
    //char cls[260];
    //char name[260];
    //GetClassNameA(hWnd, cls, 260);
    //GetWindowTextA(hWnd, name, 260);

    //_str::dbg("hWinPosInfo = 0x%p, hWnd = %08d(0x%p), { x=%04d, y=%04d, cx=%04d, cy=%04d }, class = %s, title = %s\n",
    //          hWinPosInfo, hWnd, hWnd, X, Y, cx, cy, cls, name);

#endif

    if ((uFlags & SWP_NOSIZE) == 0 || (uFlags & SWP_NOMOVE) == 0)    // 改变了尺寸或者被移动了
    {
        if (s_info->hMDIClient == hWnd || s_info->hWndMenu == hWnd
            || s_info->eWnd->isWorkWindow(hWnd)
            || s_info->eWnd->isStateWindow(hWnd)
            || s_info->eWnd->isControlWindow(hWnd)
            || s_info->eWnd->isMDIClientWindow(hWnd))
        {
            if (hWnd != hWndInsertAfter)    // 两个不相等, 表示不是自己调用的, 返回掉
                return hWinPosInfo;
        }
    }
    if (hWnd == hWndInsertAfter)
        hWndInsertAfter = NULL;

    return DeferWindowPos_hook(hWinPosInfo, hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// 判断有没有自己的标识, 有就调用移动, 没有就直接返回
inline BOOL _move_(HWND hWnd, int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
    if (LOWORD(bRepaint) != __HOOK_MOVE_BREPAINT)
        return true;
    bRepaint = HIWORD(bRepaint);
    return MoveWindow_hook(hWnd, x, y, nWidth, nHeight, bRepaint);
}
static BOOL WINAPI MyMoveWindow_hook(HWND hWnd, int x, int y, int nWidth, int nHeight, BOOL bRepaint)
{
    return MoveWindow_hook(hWnd, x, y, nWidth, nHeight, bRepaint);
    if (!s_info)return MoveWindow_hook(hWnd, x, y, nWidth, nHeight, bRepaint);
    if (s_info->eWnd->isWorkWindow(hWnd))    // 工作夹单独处理, 里面的树型框编辑框需要让他移动
    {
        char szClass[MAX_PATH];
        GetClassNameA(hWnd, szClass, MAX_PATH);
        if (_stricmp(szClass, "edit") == 0)        // 工作夹里只有一个编辑框, 那就是属性框
            return MoveWindow_hook(hWnd, x, y, nWidth, nHeight, bRepaint);
        return _move_(hWnd, x, y, nWidth, nHeight, bRepaint);
    }
    else if (s_info->eWnd->isStateWindow(hWnd)
        || s_info->eWnd->isMenuWindow(hWnd)
        || hWnd == s_info->hCodeTab
        || s_info->eWnd->isMDIClientWindow(hWnd))    // 是否为MDI窗口内部已经处理了, 设计器/起始页不会返回true
    {
        return _move_(hWnd, x, y, nWidth, nHeight, bRepaint);
    }

    // 走到这里就不是上面几个需要处理的窗口, 这里就放行
    return MoveWindow_hook(hWnd, x, y, nWidth, nHeight, bRepaint);
}

_EWINDOW_BEGIN_NAMESPACE
void _EWINDOW_NAMESPACE EWindow_Size_InitHook()
{
    hook_MoveWindow.hook(&(PVOID&)MoveWindow_hook, MyMoveWindow_hook);
    hook_DeferWindowPos.hook(&(PVOID&)DeferWindowPos_hook, MyDeferWindowPos_hook);

}

_EWINDOW_END_NAMESPACE
