#include "_draw_top_state.h"

static HWND m_hWndMove;     // 移动窗口时的容器
static POINT m_ptDown;
static bool m_isSet;

LRESULT _draw_top_state_lbuttondown(LPOBJSTRUCT pData, LPTOPSTATE_INFO info, WPARAM wParam, int x, int y)
{
    m_ptDown = { x, y };
    return DefWindowProcA(info->hWnd, WM_LBUTTONDOWN, wParam, MAKELONG(x, y));
}

LRESULT _draw_top_state_lbuttonup(LPOBJSTRUCT pData, LPTOPSTATE_INFO info, WPARAM wParam, int x, int y)
{
    ReleaseCapture();
    return 0;
}

LRESULT _draw_top_state_capture_changed(LPOBJSTRUCT pData, LPTOPSTATE_INFO info, WPARAM wParam, LPARAM lParam)
{
    m_isSet = false;
    return 0;
}
LRESULT _draw_top_state_move(LPOBJSTRUCT pData, LPTOPSTATE_INFO info, WPARAM wParam, int x, int y)
{
    return 0;
    if ((wParam & MK_LBUTTON) != MK_LBUTTON)
        return 0;
    
    const int cxClient = pData->pos.width;
    const int cyClient = pData->pos.height;
    RECT rc = { 0, 0, 0, 0 };
    ClientToScreen(info->hWnd, (LPPOINT)&rc);
    rc.right = rc.left + cxClient;
    rc.bottom = rc.top + cyClient;
    if (!m_isSet)
    {
        m_isSet = true;
        if (!IsWindowVisible(m_hWndMove))
        {
            m_hWndMove = CreateWindowExW(0, L"#32770", 0, WS_VISIBLE | WS_POPUP, rc.left, rc.top, cxClient, cyClient, 0, 0, 0, 0);
            SetWindowLongPtrW(m_hWndMove, GWLP_HWNDPARENT, (LONG_PTR)s_info->hEWnd);
            RECT rcTmp = { 0, 0, 0, 0 };
            _subclass_top_state(m_hWndMove, rcTmp);
        }
        
        HWND hChild = _draw_top_gettab(pData, 0, 0);;
        hChild = GetWindow(hChild, GW_CHILD);
        while (hChild)
        {
            if (IsWindowVisible(hChild))
            {
                RECT rcChild;
                int id = GetDlgCtrlID(hChild);
                CWndBase::GetRectFromParent(hChild, &rcChild);

            }
        }
        return 0;
    }
    POINT pt = { rc.left + x, rc.top + y };
    pt.x -= m_ptDown.x;
    pt.y -= m_ptDown.y;
    SetWindowPos(m_hWndMove, 0, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    wstr::dbg(L"x, y = %d,%d\n", pt);
    return 0;
}
