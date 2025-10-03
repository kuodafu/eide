#include "wnd_header.h"


LRESULT WndProc_OnGetMinMaxInfo(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    HMONITOR hMonitor = MonitorFromWindow(pData->hObj, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { 0 };
    mi.cbSize = sizeof(MONITORINFO);
    if (hMonitor)
    {
        if (!GetMonitorInfoW(hMonitor, &mi))
            return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
    }

    OffsetRect(&mi.rcWork, -mi.rcMonitor.left, -mi.rcMonitor.top);

    LPMINMAXINFO pos = (LPMINMAXINFO)lParam;
    // ��߶���
    pos->ptMaxPosition.x = mi.rcWork.left;
    pos->ptMaxPosition.y = mi.rcWork.top;
    //pos->ptMaxPosition.x = 0;
    //pos->ptMaxPosition.y = 0;

    // ����ȸ߶�
    pos->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
    pos->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
    //pos->ptMaxSize.x = 1366;
    //pos->ptMaxSize.y = 728;
    // �����������С�ߴ�
    pos->ptMinTrackSize.x = 800;        // ��С�϶���С
    pos->ptMinTrackSize.y = 600;
    // ������������ߴ�
    //pos->ptMaxTrackSize.x = pos->ptMaxSize.x;        // ����϶���С, ������Ļ��С
    //pos->ptMaxTrackSize.y = pos->ptMaxSize.y;
    return 0;
}
