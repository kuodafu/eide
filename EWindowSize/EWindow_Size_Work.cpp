#include "EWindow_Size_Header.h"
#include <iDraw_const.h>

_EWINDOW_BEGIN_NAMESPACE
static bool m_isNoMove_work;
static bool m_move_end_work = false;     // 已经完成移动了

inline RECT& _get_rect_work()
{
    return IsAutoHide(s_info->hWork) ? s_info->rcWork1 : s_info->rcWork;
}

void EWindow_MoveWork(HDWP& hDwp)
{
    HWND hWnd = s_info->hWork;
    LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(hWnd);
    if (!pData) return;
    LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pData->param;
    const bool isAutoHide = IsAutoHide(hWnd);
    if (!isAutoHide && !IsWindowVisible(s_info->hWork))
        ShowWindowEx(s_info->hWork, SW_SHOW);

    RECT& rcWork = _get_rect_work();    // 根据是否自动隐藏使用不同的结构
    if (rcWork.right == 0 || rcWork.bottom == 0)
        GetClientRect(hWnd, &rcWork);
    const int cxChild = rcWork.right - rcWork.left;
    const int cyChild = rcWork.bottom - rcWork.top;
    const int cyStatus = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;
    bool isDrawFrame = false;


    // 工作夹是根据状态夹和菜单夹调整尺寸的
    // 工作夹的位置, 左边预留边框宽度
    // 顶边是菜单栏的底边, 预留一小块位置进行调整尺寸
    // 宽度宽度是原来的值
    // 底边是状态夹的顶边
    rcWork.left     = s_info->scale(WINDOW_BORDER_WIDTH);
    rcWork.top      = s_info->rcWndMenu.bottom;
    rcWork.right    = rcWork.left + cxChild;
    rcWork.bottom   = IsAutoHide(s_info->hState)  // 状态夹是自动隐藏状态, 工作夹的底边就是绝对位置
        //? (s_info->cyClient - s_info->scale(WINDOW_SPACING + WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH + cyStatus))
        ? s_info->rcStatusBar.top - s_info->scale(WINDOW_AUTO_HEIGHT)
        : s_info->rcState.top;



    const int widthControl = s_info->rcControl.right - s_info->rcControl.left;
    // MDI最小宽度 + 组件箱宽度
    const int offset = WINDOW_MIN_MDIWIDTH + widthControl;

    // 如果工作夹的右边 + MDI + 组件箱大于窗口右边
    // 那工作夹的右边 = 窗口右边 - MDI - 组件箱
    if ((rcWork.right + offset) > s_info->cxClient)
        rcWork.right = s_info->cxClient - offset;
    if (rcWork.right - rcWork.left < WINDOW_MINTRACKSIZE)
        rcWork.right = rcWork.left + WINDOW_MINTRACKSIZE;

    if (isAutoHide)
    {
        if (s_info->iDraw->TopState_GetActivateWindow() == hWnd)
        {
            // 走到这里就一定是自动隐藏, 并且是当前激活的窗口, 那就是显示出来了, 需要处理显示的位置
            // 左边需要往右调, 左边已经留了一部分绘画自绘选择夹了
            rcWork.left = s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH);
            rcWork.right = rcWork.left + (cxChild < WINDOW_MINTRACKSIZE ? WINDOW_MINTRACKSIZE : cxChild);
            rcWork.bottom = s_info->rcStatusBar.top;
            m_isNoMove_work = true;
            m_move_end_work = false;
            EWindow_DeferWindowPos(hDwp, hWnd, rcWork, false);
            m_isNoMove_work = false;
            isDrawFrame = true;
            if (!m_move_end_work)
            {
                WPARAM wp = 0;
                LPARAM lp = MAKELONG(rcWork.right - rcWork.left, rcWork.bottom - rcWork.top);
                EWindow_Size_OnWorkSize(s_info->hWork, WM_SIZE, wp, lp, pData);
            }
        }
    }
    else
    {
        // 不是自动隐藏, 单独处理位置
        EWindow_DeferWindowPos(hDwp, hWnd, rcWork, false);
        isDrawFrame = true;

    }

    if (isDrawFrame)
    {
        const DWORD flags = SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOCOPYBITS;
        SetWindowPos(hWnd, 0, 0, 0, 0, 0, flags);
    }

}

// 左边工作夹尺寸被改变, 需要修改工作夹里的组件位置
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnWorkSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    static int s_i;
    m_move_end_work = true;
    //wstr::dbg(L"工作夹 重画次数 = %d\n", ++s_i);
    HDWP hDwp = 0;
    RECT& rcWork = _get_rect_work();
    const bool isAutoHide = &rcWork == &s_info->rcWork1;
    IRE_STRUCT_AUTOHIDE_MOVE evt;
    evt.hWnd = s_info->hState;
    evt.rc = rcWork;
    int evtCount = 0;
    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
    if (!m_isNoMove_work)
    {
        if (memcmp(&rcWork, &pData->pos.rcClient, sizeof(RECT)))
        {
            rcWork = pData->pos.rcClient;
            if (!isAutoHide) // 不是自动隐藏的才移动
            {
                EWindow_MoveMDI_Tab_Cbx(hDwp);
            }
            else
            {
                evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_DRAW_AUTOHIDE_MOVE, &evt, &isReturn, false);

            }
        }
    }


    int cxClient = rcWork.right - rcWork.left;
    int cyClient = rcWork.bottom - rcWork.top;

    NMHDR hdr = { 0 };
    hdr.hwndFrom = s_info->eWnd->GetWorkTab();
    hdr.code = TCN_SELCHANGE;
    hdr.idFrom = GetDlgCtrlID(hdr.hwndFrom);

    RECT rcMove = { 0, s_info->scale(WINDOW_TOP_STATE), cxClient - WINDOW_SPACING, cyClient };
    MoveWindowEx(s_info->eWnd->GetWork32770Parent(), rcMove, true);
    rcMove.left = rcMove.top = 0;
    rcMove.bottom -= s_info->scale(WINDOW_TOP_STATE);
    MoveWindowEx(s_info->eWnd->GetWork32770(), rcMove, true);
    MoveWindowEx(hdr.hwndFrom, rcMove, true);
    //SetWindowPos(s_info->eWnd->GetWork32770(), 0, rcMove.left, rcMove.top, rcMove.right - rcMove.left, rcMove.bottom - rcMove.top, SWP_NOZORDER | SWP_DRAWFRAME);
    //SetWindowPos(hdr.hwndFrom, 0, rcMove.left, rcMove.top, rcMove.right - rcMove.left, rcMove.bottom - rcMove.top, SWP_NOZORDER | SWP_DRAWFRAME);


    // 调用这个会触发选项被改变, 易语言会把窗口显示出来, 这里直接调用自己的处理函数
    //SendMessageW(s_info->eWnd->GetWork32770(), WM_NOTIFY, 0, (LPARAM)&hdr);

    LPOBJSTRUCT data32770 = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWork32770());
    WndProc_OnWorkTabItemChanged(data32770->hWnd, WM_NOTIFY, hdr.code, (LPARAM)&hdr, data32770, false);
    if (isAutoHide && evtCount)
    {
        s_info->evt.pfn_Event_callback_after(arr, IRE_DRAW_AUTOHIDE_MOVE, &evt, evtCount, false);
    }
    pData->isReturn = true;
    return 0;
}



LRESULT CALLBACK WndProc_OnWorkTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage)
{
    IRE_STRUCT_TAB_SIZE evt = { 0 };
    RECT& rc = evt.rc;  // 计算出来的选择夹的客户区, 窗口应该在这个客户区内
    HWND hWndShow = 0;
    LPNMHDR hdr = (LPNMHDR)lParam;
    HWND hOld = (HWND)GetPropW(hdr->hwndFrom, L"oldWnd");
    evt.index = TabCtrl_GetCurSel(hdr->hwndFrom); // 选择夹选择改变
    evt.hWnd = hdr->hwndFrom;
    TCITEMW item = { 0 };
    item.mask = TCIF_PARAM;
    TabCtrl_GetItem(hdr->hwndFrom, evt.index, &item);
    RECT& rcWork = _get_rect_work();
    const bool isAutoHide = &rcWork == &s_info->rcWork1;
    if (isAutoHide)
    {
        evt.direction = _tab_calc_client(hdr->hwndFrom, rcWork, rc);
    }
    else
    {
        RECT rcTmp;
        GetClientRect(s_info->eWnd->GetWorkTab(), &rcTmp);
        evt.direction = _tab_calc_client(hdr->hwndFrom, rcTmp, rc);
    }
    if (isAutoHide && s_info->iDraw->TopState_GetActivateWindow() == s_info->hWork)
    {
        // 自动隐藏, 且当前是显示状态
        //GetClientRect(s_info->eWnd->GetWorkTab(), &rc);
        rc = pData->pos.rcClient;
        rc.left = 1;
        rc.top = 1; 
        rc.right -= 1;
        rc.bottom -= 1;
    }

    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
    const int flags = SWP_NOZORDER;
    if (item.lParam == TAB_PARAM_PROPER) // 属性单独处理
    {
        // 需要显示两个组合框和属性框
        hWndShow = s_info->eWnd->GetWorkProperty();
        if (hWndShow != hOld)   // 隐藏旧窗口, 记录当前按下的窗口
        {
            //if (hOld)ShowWindowEx(hOld, SW_HIDE);
            SetPropW(hdr->hwndFrom, L"oldWnd", hWndShow);
        }
        HWND hWndComboboxControl = s_info->eWnd->GetWorkControlComboBox();
        HWND hWndComboboxEvent = s_info->eWnd->GetWorkEventComboBox();
        switch (evt.direction)
        {
        case 0:     // 左边
        {
            rc.left--;
            rc.top = 0;
            rc.right++;
            rc.bottom++;
            break;
        }
        case 1:     // 顶边
        {
            rc.left = 0;
            rc.top--;
            rc.right++;
            rc.bottom++;
            break;
        }
        case 2:     // 右边
        {
            rc.left = 0;
            rc.top = 0;
            rc.right++;
            rc.bottom++;
            break;
        }
        default:    // 底边
        {
            rc.left = 0;
            rc.top = 0;
            rc.right++;
            rc.bottom++;
            break;
        }
        }
        // 上面的switch是计算属性框需要的位置, 下面一行代码是触发尺寸改变事件, 让用户对尺寸进行二次处理
        int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_TAB_WORK_SIZE, &evt, &isReturn, false);
        if (isReturn) return 0;

        const int comListHeight = 400;
        if (!g_moveInfo.heightCombobox)
        {
            RECT rcCombobox;
            GetWindowRect(hWndComboboxControl, &rcCombobox);
            g_moveInfo.heightCombobox = rcCombobox.bottom - rcCombobox.top;
            if (g_moveInfo.heightCombobox <= 0)
                g_moveInfo.heightCombobox = 20;
        }
        const int comHeight = s_info->scale(g_moveInfo.heightCombobox);
        const int height = (rc.bottom - rc.top) - (comHeight * 2);
        SetWindowPos(hWndShow, 0, rc.left + 1, rc.top + comHeight, rc.right - rc.left - 2, height, flags);

        const int top = rc.top + comHeight + height;

        SetWindowPos(hWndComboboxControl, 0, rc.left, rc.top, rc.right - rc.left, comListHeight, flags);    // 两个组合框得最后才移动
        SetWindowPos(hWndComboboxEvent, 0, rc.left, top, rc.right - rc.left, comListHeight, flags);

        SendMessageA(hWndComboboxControl, CB_SETITEMHEIGHT, -1, comHeight - 6);
        SendMessageA(hWndComboboxEvent, CB_SETITEMHEIGHT, -1, comHeight - 6);

        s_info->evt.pfn_Event_callback_after(arr, IRE_TAB_WORK_SIZE, &evt, evtCount, false);

        return 0;
    }

    int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_TAB_WORK_SIZE, &evt, &isReturn, false);
    if (isReturn) return 0;

    //const int flags = SWP_NOZORDER | SWP_SHOWWINDOW;

    switch (item.lParam)
    {
    case TAB_PARAM_FNE: // 支持库
    {
        hWndShow = s_info->eWnd->GetWorkFneTree();
        break;
    }
    case TAB_PARAM_PROGRAM: // 程序
    {
        hWndShow = s_info->eWnd->GetWorkProgramTree();
        break;
    }
    case TAB_PARAM_PROPER: // 属性
    {
        break;
    }
    default:
        break;
    }

    ShowWindowEx(s_info->eWnd->GetWorkControlComboBox(), SW_HIDE);
    ShowWindowEx(s_info->eWnd->GetWorkEventComboBox(), SW_HIDE);
    if (hWndShow != hOld)   // 切换到新的窗口
    {
        //if (hOld)ShowWindowEx(hOld, SW_HIDE);
        SetPropW(hdr->hwndFrom, L"oldWnd", hWndShow);
    }
    if (hWndShow)
    {
        //MoveWindowEx(hWndShow, rc, true);
        SetWindowPos(hWndShow, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, flags | SWP_DRAWFRAME);
    }

    s_info->evt.pfn_Event_callback_after(arr, IRE_TAB_WORK_SIZE, &evt, evtCount, false);
    return 0;
}

void EWindow_StateClick_Work(LPTOPSTATE_INFO info, int id)
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

