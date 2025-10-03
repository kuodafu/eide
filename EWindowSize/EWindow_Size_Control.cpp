#include "EWindow_Size_Header.h"
#include <iDraw_const.h>

_EWINDOW_BEGIN_NAMESPACE
static bool m_isNoMove_control = false;
inline RECT& _get_rect_control()
{
    return IsAutoHide(s_info->hControl) ? s_info->rcControl1 : s_info->rcControl;
}

void EWindow_MoveControl(HDWP& hDwp)
{
    if (!s_info->isReady)
        return;
    //if (!IsWindowVisible(s_info->hControl))
    //    ShowWindowEx(s_info->hControl, SW_SHOW);

    g_moveInfo.windowType = EWindow_IsSelWindow();

    //if (g_moveInfo.windowType == -1) return;
    
    static HWND hComboBox = s_info->eWnd->GetMenuComboBox1();
    if (hComboBox && g_moveInfo.heightCombobox == 0)    // 有组合框, 需要加上组合框的高度
    {
        RECT rcMenuComboBox;
        GetClientRect(hComboBox, &rcMenuComboBox);
        g_moveInfo.heightCombobox = rcMenuComboBox.bottom - rcMenuComboBox.top;
    }

    HWND hWnd = s_info->hControl;
    LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(hWnd);
    if (!pData) return;
    LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pData->param;
    RECT& rcControl = _get_rect_control();    // 根据是否自动隐藏使用不同的结构
    const bool isAutoHide = &s_info->rcControl1 == &rcControl;
    int cxChild = rcControl.right - rcControl.left;
    int cyChild = rcControl.bottom - rcControl.top;
    if (cxChild < 160)
    {
        rcControl.right = s_info->cxClient - WINDOW_BORDER_WIDTH - (isAutoHide ? s_info->scale(WINDOW_AUTO_HEIGHT) : 0);
        rcControl.left = rcControl.right - 160;
        cxChild = 160;
    }

    // 组件箱左边 = 需要判断
    // 组件箱顶边 = 菜单栏底边
    // 组件箱右边 = 窗口宽度 - 预留边框
    // 组件箱底边 = 状态夹顶边

    if (isAutoHide)
    {
        // 当前组件箱是激活状态, 需要显示出来
        if (s_info->iDraw->TopState_GetActivateWindow() != hWnd)
            return;

        rcControl.right = s_info->cxClient - s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH);
        rcControl.left = rcControl.right - cxChild;
        rcControl.top = s_info->rcWndMenu.bottom;
        rcControl.bottom = s_info->rcStatusBar.top;

        m_isNoMove_control = true;
        EWindow_DeferWindowPos(hDwp, hWnd, rcControl, false);
        m_isNoMove_control = false;
        return;
    }

    rcControl.top       = s_info->rcWndMenu.bottom;
    rcControl.right     = s_info->cxClient - WINDOW_BORDER_WIDTH;
    rcControl.left      = rcControl.right - cxChild;
    const int cyStatus  = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;
    rcControl.bottom    = IsAutoHide(s_info->hState)  // 状态夹是自动隐藏状态, 组件箱的底边就是绝对位置
        ? (s_info->cyClient - WINDOW_SPACING - s_info->scale(WINDOW_AUTO_HEIGHT) - cyStatus - s_info->scale(WINDOW_BORDER_WIDTH))
        : s_info->rcState.top;
    const bool isVisible = IsWindowVisible(hWnd);
    if (g_moveInfo.windowType == 1)  // 有组件箱
    {
        if (!isVisible)
            ShowWindowEx(hWnd, SW_SHOW);

        // 如果组件箱的左边大于组件箱的右边 + 最小MDI宽度
        // 那组件箱的左边 = 工作夹的右边 + MDI最小宽度
        if (rcControl.left - WINDOW_MIN_MDIWIDTH < s_info->rcWork.right)
            rcControl.left = s_info->rcWork.right + WINDOW_MIN_MDIWIDTH;

        if (rcControl.right - rcControl.left < WINDOW_MINTRACKSIZE)
            rcControl.left = rcControl.right - WINDOW_MINTRACKSIZE; // 应该是不会走到这里的

            //MoveWindowEx(s_info->hControl, s_info->rcControl, true);
        EWindow_DeferWindowPos(hDwp, s_info->hControl, rcControl, true);
    }
    else
    {
        if (isVisible)
            ShowWindowEx(hWnd, SW_HIDE);
    }



}



// 组件箱, 需要根据MDIClient窗口显示的内容来调整位置
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnControlSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    static int s_i;
    //wstr::dbg(L"组件箱 重画次数 = %d\n", ++s_i);

    pData->isReturn = true;
    HDWP hDwp = 0;
    RECT& rcControl = _get_rect_control();
    const bool isAutoHide = &rcControl == &s_info->rcControl1;    // 是自动隐藏就使用g_moveInfo里的结构
    IRE_STRUCT_AUTOHIDE_MOVE evt;
    evt.hWnd = s_info->hState;
    evt.rc = rcControl;
    int evtCount = 0;
    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;

    if (!m_isNoMove_control)
    {
        if (memcmp(&rcControl, &pData->pos.rcClient, sizeof(RECT)))
        {
            rcControl = pData->pos.rcClient;
            if (!isAutoHide)    // 组件箱不是自动隐藏的话就需要重新调整尺寸
            {
                EWindow_MoveMDI_Tab_Cbx(hDwp);
            }
            else
            {
                evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_DRAW_AUTOHIDE_MOVE, &evt, &isReturn, false);
            }
        }
    }

    // 下面这些计算位置的没什么用, 组件箱里面的几个组件是在 WM_WINDOWPOSCHANGEING 消息里改变
    int cxClient = s_info->rcControl.right - s_info->rcControl.left;
    int cyClient = s_info->rcControl.bottom - s_info->rcControl.top;
    if (m_isNoMove_control)
    {
        cxClient = pData->pos.width;
        cyClient = pData->pos.height;
    }

    HWND hWnd32770Parent = s_info->eWnd->GetControl32770Parent();
    RECT rcMove = { WINDOW_SPACING + 1, s_info->scale(WINDOW_TOP_STATE), cxClient - 1, cyClient - 1 };	// 偏移
    SetWindowPos(hWnd32770Parent, 0, rcMove.left, rcMove.top,
        rcMove.right - rcMove.left, rcMove.bottom - rcMove.top, SWP_NOZORDER | SWP_SHOWWINDOW);

    rcMove.right -= rcMove.left;
    rcMove.bottom = rcMove.top;
    MoveWindowEx(s_info->eWnd->GetControl32770(), rcMove, true);
    if (isAutoHide && evtCount)
    {
        s_info->evt.pfn_Event_callback_after(arr, IRE_DRAW_AUTOHIDE_MOVE, &evt, evtCount, false);
    }
    return 0;
}


void EWindow_StateClick_Control(LPTOPSTATE_INFO info, int id)
{
    switch (id)
    {
    case TOP_STATE_BUTTON_CLOSE:    // 关闭按钮, 需要隐藏这个窗口
    {

        break;
    }
    case TOP_STATE_BUTTON_MAXBOX:   // 最大化按钮, 需要判断当前状态进行操作
    {

        break;
    }
    case TOP_STATE_BUTTON_PUSHPIN:  // 图钉按钮, 需要判断是否选中然后处理是否浮动
    {
        break;
    }
    default:
        break;
    }
}

_EWINDOW_END_NAMESPACE