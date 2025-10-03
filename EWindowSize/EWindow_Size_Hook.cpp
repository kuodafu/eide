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

    if ((uFlags & SWP_NOSIZE) == 0 || (uFlags & SWP_NOMOVE) == 0)    // �ı��˳ߴ���߱��ƶ���
    {
        if (s_info->hMDIClient == hWnd || s_info->hWndMenu == hWnd
            || s_info->eWnd->isWorkWindow(hWnd)
            || s_info->eWnd->isStateWindow(hWnd)
            || s_info->eWnd->isControlWindow(hWnd)
            || s_info->eWnd->isMDIClientWindow(hWnd))
        {
            if (hWnd != hWndInsertAfter)    // ���������, ��ʾ�����Լ����õ�, ���ص�
                return hWinPosInfo;
        }
    }
    if (hWnd == hWndInsertAfter)
        hWndInsertAfter = NULL;

    return DeferWindowPos_hook(hWinPosInfo, hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// �ж���û���Լ��ı�ʶ, �о͵����ƶ�, û�о�ֱ�ӷ���
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
    if (s_info->eWnd->isWorkWindow(hWnd))    // �����е�������, ��������Ϳ�༭����Ҫ�����ƶ�
    {
        char szClass[MAX_PATH];
        GetClassNameA(hWnd, szClass, MAX_PATH);
        if (_stricmp(szClass, "edit") == 0)        // ��������ֻ��һ���༭��, �Ǿ������Կ�
            return MoveWindow_hook(hWnd, x, y, nWidth, nHeight, bRepaint);
        return _move_(hWnd, x, y, nWidth, nHeight, bRepaint);
    }
    else if (s_info->eWnd->isStateWindow(hWnd)
        || s_info->eWnd->isMenuWindow(hWnd)
        || hWnd == s_info->hCodeTab
        || s_info->eWnd->isMDIClientWindow(hWnd))    // �Ƿ�ΪMDI�����ڲ��Ѿ�������, �����/��ʼҳ���᷵��true
    {
        return _move_(hWnd, x, y, nWidth, nHeight, bRepaint);
    }

    // �ߵ�����Ͳ������漸����Ҫ����Ĵ���, ����ͷ���
    return MoveWindow_hook(hWnd, x, y, nWidth, nHeight, bRepaint);
}

_EWINDOW_BEGIN_NAMESPACE
void _EWINDOW_NAMESPACE EWindow_Size_InitHook()
{
    hook_MoveWindow.hook(&(PVOID&)MoveWindow_hook, MyMoveWindow_hook);
    hook_DeferWindowPos.hook(&(PVOID&)DeferWindowPos_hook, MyDeferWindowPos_hook);

}

_EWINDOW_END_NAMESPACE
