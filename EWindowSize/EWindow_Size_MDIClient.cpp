#include "EWindow_Size_Header.h"
#include <iDraw_const.h>

_EWINDOW_BEGIN_NAMESPACE

void EWindow_Size_OnMenuComboBox();
// 移动MDIClient下的子窗口, 调用前必须已经计算好MDIClient的位置, 子窗口都是根据这个位置进行移动
void EWindow_MoveMDIClientChild(HDWP& hDwp);
// 移动起始页
void EWindow_MoveMDIClientEPage(HDWP& hDwp, RECT& rcChild3);

void EWindow_MoveMDIClient(HDWP& hDwp)
{
    if (!s_info->isReady)
        return;

    if (!IsWindowVisible(s_info->hMDIClient))
        ShowWindowEx(s_info->hMDIClient, SW_SHOW);
    // 左边 = 工作夹右边
    // 顶边 = 菜单栏底边 + [组合框高度]  代码切换选择夹在顶边需要加上选择夹的高度
    // 右边 = 组件箱的左边
    // 底边 = 工作夹底边   代码选择夹在底边需要减去选择夹的高度

    bool isTop = s_info->config->tab_code_direction() != 3;             // 代码选择夹的子夹方向, 3=底边, 其他 = 顶边
    const bool isAutohideWork       = IsAutoHide(s_info->hWork);        // 工作夹是否是自动隐藏
    const bool isAutohideState      = IsAutoHide(s_info->hState);       // 状态夹是否是自动隐藏
    const bool isAutohideControl    = IsAutoHide(s_info->hControl);     // 状态夹是否是自动隐藏
    RECT& rcMDIClient = s_info->rcMDIClient;
    BOOL isShow = s_info->config->IsShowLocationTool();

    rcMDIClient.left        = s_info->rcWork.right;
    rcMDIClient.top         = s_info->rcWndMenu.bottom;
    rcMDIClient.right       = ((g_moveInfo.windowType == 1) ? s_info->rcControl.left : s_info->cxClient - WINDOW_BORDER_WIDTH);
    rcMDIClient.bottom      = s_info->rcState.top - (isTop ? 0 : s_info->scale(WINDOW_CUSTOMTAB_HEIGHT));
    if (isShow)
        rcMDIClient.top += s_info->scale(g_moveInfo.heightCombobox);
    if (isTop)
        rcMDIClient.top += s_info->scale(WINDOW_CUSTOMTAB_HEIGHT);
    
    if (!isShow)
    {
        if (isTop)
            rcMDIClient.bottom = s_info->rcState.top;   // 不显示定位工具条的话底边就是工作夹的底边
        else
            rcMDIClient.top = s_info->rcWndMenu.bottom;
        // 顶边的话就是自绘选择夹的顶边
        if (isTop && isShow)
            rcMDIClient.top -= s_info->scale(g_moveInfo.heightCombobox);
    }

    if (isAutohideWork)     // 工作夹是自动隐藏的话, mdi的左边就是固定的
        rcMDIClient.left = s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH * 2);
    if (isAutohideState)    // 状态夹是自动隐藏的话, mdi的底边是固定的
        rcMDIClient.bottom = s_info->rcStatusBar.top - s_info->scale(WINDOW_AUTO_HEIGHT) - (isTop ? 0 : s_info->scale(WINDOW_CUSTOMTAB_HEIGHT));
    if (isAutohideControl)  // 组件箱是自动隐藏的话, mdi的右边是固定的
        rcMDIClient.right = s_info->cxClient - (s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH * 2));

    EWindow_DeferWindowPos(hDwp, s_info->hMDIClient, rcMDIClient, true);

    //RECT& rc = rcMDIClient;
    //SetWindowPos(s_info->hMDIClient, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DRAWFRAME);

    //EWindow_MoveMDIClientChild(hDwp);
}

// 移动MDIClient下的子窗口, 调用前必须已经计算好MDIClient的位置, 子窗口都是根据这个位置进行移动
void EWindow_MoveMDIClientChild(HDWP& hDwp)
{
    const int cxClient = s_info->rcMDIClient.right - s_info->rcMDIClient.left;
    const int cyClient = s_info->rcMDIClient.bottom - s_info->rcMDIClient.top;

    IRE_STRUCT_MDICLIENT_MOVE_EVENT evt;
    evt.rcChild1 = { 0, 0, cxClient, cyClient }; // MDI子窗口的位置
    evt.rcChild2 = { 0, 0, cxClient, cyClient }; // MDI子窗口的位置
    evt.rcChild3 = { 0, 0, cxClient, cyClient }; // MDI子窗口的位置
    evt.rcChild4 = { 0, 0, cxClient, cyClient }; // MDI子窗口的位置
    evt.hChild1 = g_moveInfo.hChild1;
    evt.hChild2 = g_moveInfo.hChild2;
    evt.hChild3 = g_moveInfo.hChild3;
    evt.hChild4 = g_moveInfo.hChild4;
    evt.windowType = MAKELONG(g_moveInfo.windowType, g_moveInfo.code_type);
    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
    int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_TAB_MDICLIENT_SIZE, &evt, &isReturn, false);
    if (isReturn)
        return;     // 插件处理了位置信息, 不往后处理

    EWindow_DeferWindowPos(hDwp, g_moveInfo.hChild1, evt.rcChild1, true);     // MDI下的第一层窗口
    EWindow_DeferWindowPos(hDwp, g_moveInfo.hChild2, evt.rcChild2, true);     // MDI下的第二层窗口
    EWindow_DeferWindowPos(hDwp, g_moveInfo.hChild3, evt.rcChild3, true);

    if (g_moveInfo.windowType == 2)  // 2=起始页
    {
        EWindow_MoveMDIClientEPage(hDwp, evt.rcChild3);
    }
    else if (g_moveInfo.windowType == 1)    // 设计器窗口
    {
        MoveWindowEx(g_moveInfo.hChild3, evt.rcChild3, true);    // 设计器父窗口
    }
    else if (g_moveInfo.hChild4)    // 代码区
    {
        MoveWindowEx(g_moveInfo.hChild4, evt.rcChild4, true);    // 代码区父窗口
    }
    
    s_info->evt.pfn_Event_callback_after(arr, IRE_TAB_MDICLIENT_SIZE, &evt, evtCount, false);
    return;
}

void EWindow_MoveMDIClientEPage(HDWP& hDwp, RECT& rcChild3)
{
    if (s_info->rcControl.right - s_info->rcControl.left > 0)
    {
        s_info->rcControl.left = s_info->rcControl.right;
        EWindow_DeferWindowPos(hDwp, s_info->hControl, s_info->rcControl, true);
    }

    const int cx = rcChild3.right - rcChild3.left;
    const int cy = rcChild3.bottom - rcChild3.top;
    const RECT rcChild4[] =
    {
        { 13, 138, 254 + 13, cy - 238 + 138 },
        { 13, cy - 90, 254 + 13, 70 + (cy - 90) },
        { 13, 110, 40 + 13, 24 + 110 },
        { 55, 110, 60 + 55, 24 + 110 },
        { 117, 110, 40 + 117, 24 + 110 },
        { 159, 110, 40 + 159, 24 + 110 },
        { 281, 106, cx - 11, cy - 11 },
    };

    int i = 0;
    HWND hChild4 = g_moveInfo.hChild4;
    while (hChild4)
    {
        if (i >= sizeof(rcChild4) / sizeof(rcChild4[0])) break;    // 防止越界
        EWindow_DeferWindowPos(hDwp, hChild4, rcChild4[i++], TRUE);
        hChild4 = GetWindow(hChild4, GW_HWNDNEXT);
    }
}

// 中间代码区MDI窗口被改变, 这里需要修改不少东西
// 双击窗口设计器不会触发当前事件
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnMDIClientSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    struct _set_return 
    {
        LPOBJSTRUCT pData;
        bool b;
        _set_return(LPOBJSTRUCT p, bool isReturn) :pData(p), b(isReturn) { }
        ~_set_return()
        {
            pData->isReturn = b;
        }
        inline void operator=(bool isReturn) { b = isReturn; }
        
    };
    _set_return _set_ret(pData, true);
    static int s_i;
    //wstr::dbg(L"MDIClient 重画次数 = %d, 当前选项数 = %d\n", ++s_i, ((int)s_info->hWnds->size()));
    if (!s_info->hWnds || s_info->hWnds->size() == 0) return 0;
    if (!IsWindowVisible(hWnd))return 0;
    auto& arr = *s_info->hWnds;
    int index = TabCtrl_GetCurSel(s_info->hEWnd);
    if (index == -1)
        return 0;
    HWND hWndShow = arr[index].hWnd;

    HDWP hDwp = 0;
    EWindow_IsSelWindow();
    //wstr::dbg(L"error 当前窗口类型 = %d, 代码类型 = %d\n", g_moveInfo.windowType, g_moveInfo.code_type);
    if (g_moveInfo.windowType == -1) return 0;
    
    ShowWindowEx(hWndShow, SW_SHOW);


    RECT& rcMDIClient = s_info->rcMDIClient;
    RECT& rcControl = s_info->rcControl;
    if (rcMDIClient.right != s_info->cxClient - WINDOW_BORDER_WIDTH
        && rcControl.right - rcControl.left <= 0)
    {
        // MDI的右边不是在易语言窗口的右边, 且组件箱宽度为0, 那就需要调整组件箱的位置
        rcControl.left = rcMDIClient.right;
        rcControl.top = s_info->rcWndMenu.bottom;
        rcControl.right = s_info->cxClient - WINDOW_BORDER_WIDTH;
        rcControl.bottom = rcMDIClient.bottom;

        MoveWindowEx(s_info->hControl, rcControl, true);
        //EWindow_Size_OnControlSize_Child();
    }

    static bool firstMoveControl;
    if (rcControl.right - rcControl.left > 0 && !firstMoveControl)
    {
        HWND hControlTree = s_info->eWnd->GetControlTree();
        if (IsWindowVisible(hControlTree))
        {
            firstMoveControl = true;
            auto pfn_timer = [](HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
            {
                SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_DRAWFRAME);
                KillTimer(hWnd, id);
            };
            SetTimer(hControlTree, (UINT_PTR)&pfn_timer, 50, pfn_timer);
        }
    }

    EWindow_MoveMDIClientChild(hDwp);
    return 0;
}

// 改变定位工具条的组合框位置
void EWindow_Size_OnMenuComboBox()
{
    if (IsWindowVisible(s_info->hWork) && s_info->rcWork.bottom != s_info->rcMDIClient.bottom)
    {
        s_info->rcWork.bottom = s_info->rcMDIClient.bottom;
        MoveWindowEx(s_info->hWork, s_info->rcWork, true);
    }

    if (IsWindowVisible(s_info->hEWnd) && s_info->rcCustomTab.left != s_info->rcMDIClient.left)
    {
        s_info->rcCustomTab.left = s_info->rcMDIClient.left;
        MoveWindowEx(s_info->hEWnd, s_info->rcCustomTab, true);
    }
}

_EWINDOW_END_NAMESPACE

