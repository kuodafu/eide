#include "wnd_header.h"


LRESULT WndProc_OnGetMinMaxInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    HMONITOR hMonitor = MonitorFromRect(&pData->pos.rcIng, MONITOR_DEFAULTTONEAREST);
    //HMONITOR hMonitor = MonitorFromWindow(pData->hObj, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { 0 };
    mi.cbSize = sizeof(MONITORINFO);

    if (!GetMonitorInfoW(hMonitor, &mi))
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    RECT test_rc;
    GetWindowRect(pData->hObj, &test_rc);
    OffsetRect(&mi.rcWork, -mi.rcMonitor.left, -mi.rcMonitor.top);

    LPMINMAXINFO pos = (LPMINMAXINFO)lParam;
    // 左边顶边
    pos->ptMaxPosition.x = mi.rcWork.left;
    pos->ptMaxPosition.y = mi.rcWork.top;
    //pos->ptMaxPosition.x = 0;
    // 
    //pos->ptMaxPosition.y = 0;

    // 最大宽度高度
    pos->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
    pos->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
    //pos->ptMaxSize.x = 1366;
    //pos->ptMaxSize.y = 728;
    // 允许调整的最小尺寸
    pos->ptMinTrackSize.x = 800;        // 最小拖动大小
    pos->ptMinTrackSize.y = 600;
    // 允许调整的最大尺寸
    pos->ptMaxTrackSize.x = pos->ptMaxSize.x;   // 最大拖动大小, 不设置这个值的话其他屏幕有dpi缩放的话最大化会出问题
    pos->ptMaxTrackSize.y = pos->ptMaxSize.y;
    return 0;
}
