#include "EWindow_Size_Header.h"
#include <iDraw_const.h>



//// 是否自动隐藏
//static bool state_isAutohide(LPTOPSTATE_INFO info)
//{
//    return __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE);
//}

_EWINDOW_BEGIN_NAMESPACE
const LPCWSTR oldSelParamProp = L"{7B745F5E-8958-4B6E-8EDD-49DC61312F7E}";
static bool m_isNoMove_state = false;
static bool m_move_end = false;     // 已经完成移动了
void EWindow_MoveStateChild(HDWP& hDwp);
static int m_tab_left_proportion;   // 左边选择夹比例, 万分比
struct TAB_SEL_CHANGE_STRUCT
{
    std::vector<HWND>* hWnds;   // 上一次显示的窗口数组
    int param;      // 上一次点击的项目数值, 可以根据这个数值知道上一次是显示哪些窗口
};

inline RECT& _get_rect_state()
{
    return IsAutoHide(s_info->hState) ? s_info->rcState1 : s_info->rcState;
}

// 获取选择夹项目数值, index=-1则获取当前选中项
inline int Tab_GetItemParam(HWND hTab, int index = -1)
{
    if (index == -1)
        index = TabCtrl_GetCurSel(hTab);
    TCITEMW item = { 0 };
    item.mask = TCIF_PARAM;
    TabCtrl_GetItem(hTab, index, &item);
    return (int)item.lParam;
}
void EWindow_MoveState(HDWP& hDwp)
{
    // 状态夹可以通过 状态条 易语言窗口 计算出4个边
    // 这几个组件都已经移动完毕, 直接使用他们的位置就行
    LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(s_info->hState);
    if (!pData) return;
    LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pData->param;
    RECT& rcState = _get_rect_state();  // 根据是否自动隐藏使用不同的结构
    const bool isAutoHide = &rcState == &s_info->rcState1;
    if (!isAutoHide && !IsWindowVisible(s_info->hState))
        ShowWindowEx(s_info->hState, SW_SHOW);
    //RECT& rcState = s_info->rcState;
    if (rcState.right == 0 || rcState.bottom == 0)
        GetClientRect(s_info->hState, &rcState);
    const int cyChild = rcState.bottom - rcState.top;
    const int cyStatus = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;
    bool isDrawFrame = false;
    // 状态夹位置是根据状态条来改变的
    rcState.left    = WINDOW_BORDER_WIDTH;
    rcState.right   = s_info->cxClient - WINDOW_BORDER_WIDTH;
    rcState.bottom  = s_info->rcStatusBar.top - s_info->scale((isAutoHide ? WINDOW_AUTO_HEIGHT : 0));
    rcState.top     = rcState.bottom - cyChild;
    if (IsAutoHide(s_info->hWork))
        rcState.left = s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH * 2);
    if (IsAutoHide(s_info->hControl))
        rcState.right = s_info->cxClient - s_info->scale(WINDOW_BORDER_WIDTH * 2 + WINDOW_AUTO_HEIGHT);

    // MDI最小高度 + 组合框 + 选择夹
    const int offset = WINDOW_MIN_MDIHEIGHT + WINDOW_CUSTOMTAB_HEIGHT * 2;

    // 如果顶边 + MDI + 组合框+选择夹 小于菜单栏的底边
    // 那状态夹的顶边 = 菜单栏的底边 + MDI + 组合框 + 选择夹, 这里的组合框应该会有偏差, 但无所谓.....
    if ((rcState.top - offset) < s_info->rcWndMenu.bottom)
        rcState.top = s_info->rcWndMenu.bottom + offset;

    if (rcState.bottom - rcState.top < WINDOW_MINTRACKSIZE)
        rcState.top = rcState.bottom - WINDOW_MINTRACKSIZE; // 应该是不会走到这里的

    if(isAutoHide)
    {
        if (s_info->iDraw->TopState_GetActivateWindow() == s_info->hState)
        {
            // 走到这里就一定是自动隐藏, 并且是当前激活的窗口, 那就是显示出来了, 需要处理显示的位置
            // 底边需要往上调, 底边已经留了一部分绘画自绘选择夹了
            m_isNoMove_state = true;
            m_move_end = false;
            EWindow_DeferWindowPos(hDwp, s_info->hState, rcState, false);
            m_isNoMove_state = false;
            isDrawFrame = true;
            if (!m_move_end)
            {
                WPARAM wp = 0;
                LPARAM lp = MAKELONG(rcState.right - rcState.left, rcState.bottom - rcState.top);
                EWindow_Size_OnStateSize(s_info->hState, WM_SIZE, wp, lp, pData);
            }

        }
    }
    else
    {
        EWindow_DeferWindowPos(hDwp, s_info->hState, rcState, false);
        isDrawFrame = true;

    }

    if (isDrawFrame)
    {
        const DWORD flags = SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOCOPYBITS;
        SetWindowPos(s_info->hState, 0, 0, 0, 0, 0, flags);
    }
}

void EWindow_MoveStateChild(HDWP& hDwp)
{

}

// 给左边或者右边的选择夹触发子夹被改变事件, 0=左边, 1=右边, 2=默认选择夹, -1=全部
inline void _tab_l_r_ItemChange(int flag)
{
    NMHDR hdr1 = { 0 };
    NMHDR hdr2 = { 0 };
    NMHDR hdr3 = { 0 };
    hdr1.hwndFrom = s_info->hTabStateLeft;
    hdr1.code = TCN_SELCHANGE;
    hdr1.idFrom = 0;
    hdr2.hwndFrom = s_info->hTabStateRight;
    hdr2.code = hdr1.code;
    hdr2.idFrom = hdr1.idFrom;
    hdr3.hwndFrom = s_info->eWnd->GetStateTab();
    hdr3.code = hdr1.code;
    hdr3.idFrom = hdr1.idFrom;
    LPOBJSTRUCT data32770 = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetState32770());
    if (flag == 0 || flag == -1)
        WndProc_OnStateTabItemChanged(data32770->hWnd, WM_NOTIFY, hdr1.code, (LPARAM)&hdr1, data32770, false);
    if (flag == 1 || flag == -1)
        WndProc_OnStateTabItemChanged(data32770->hWnd, WM_NOTIFY, hdr2.code, (LPARAM)&hdr2, data32770, false);
    if(flag == 2)
        WndProc_OnStateTabItemChanged(data32770->hWnd, WM_NOTIFY, hdr3.code, (LPARAM)&hdr3, data32770, false);

}

inline void __hide_vartable(bool isHideStatic)
{
    HWND hTree = s_info->eWnd->GetStateVarTableTree();
    HWND hCombobox = s_info->eWnd->GetStateStackCombobox();
    HWND hBtn = s_info->eWnd->GetStateTabVarTableWriteBtn();
    HWND hType = s_info->eWnd->GetStateTypeSize();
    HWND hEdit = s_info->eWnd->GetStateTabVarTableEdit();
    HWND hStatic = s_info->eWnd->GetStateTabVarTableStatic();
    ShowWindowEx(hTree, SW_HIDE);
    ShowWindowEx(hCombobox, SW_HIDE);
    ShowWindowEx(hBtn, SW_HIDE);
    ShowWindowEx(hType, SW_HIDE);
    ShowWindowEx(hEdit, SW_HIDE);
    if (isHideStatic) ShowWindowEx(hStatic, SW_HIDE);
}


// 调整变量表里面的组件的位置
inline void _tab_move_vartable(int param, RECT& rc, TAB_SEL_CHANGE_STRUCT* pOlds)
{
    HWND hWndShow = s_info->eWnd->GetStateTabVarTableStatic();
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    SetWindowPos(hWndShow, HWND_TOP, rc.left, rc.top, cxClient, cyClient, 0);

    // 标准工具条的停止按钮都是启用状态, 运行后停止按钮被启用, 运行按钮被禁用, 暂停后两个按钮都启用
    // 表示现在不是调试状态, 组件位置和其他组件一样
    if (!s_info->isRun || !s_info->isStop)  // 运行和停止按钮至少有一个不是启用状态, 不是在断点调试, 需要把标签隐显示出来, 其他组件隐藏
    {
        __hide_vartable(false);      // 隐藏变量表其他组件, 位置还需要调整
    }
    // 走到这就是调试状态, 有多个组件需要调整位置
    HWND hTree = s_info->eWnd->GetStateVarTableTree();
    HWND hCombobox = s_info->eWnd->GetStateStackCombobox();
    HWND hBtn = s_info->eWnd->GetStateTabVarTableWriteBtn();
    HWND hType = s_info->eWnd->GetStateTypeSize();
    HWND hEdit = s_info->eWnd->GetStateTabVarTableEdit();

    if (pOlds->param != param)    // 切换到这个选项之前有窗口在显示, 这里需要把他们隐藏
    {
        for (auto it = pOlds->hWnds->begin(); it != pOlds->hWnds->end(); ++it)
            ShowWindowEx(*it, SW_HIDE);   // 隐藏旧窗口
        pOlds->hWnds->clear();
        pOlds->hWnds->push_back(hWndShow);
        pOlds->hWnds->push_back(hTree);
        pOlds->hWnds->push_back(hCombobox);
        pOlds->hWnds->push_back(hBtn);
        pOlds->hWnds->push_back(hType);
        pOlds->hWnds->push_back(hEdit);
        pOlds->param = param;
    }

    int left = rc.left;
    int top = rc.top;
    int w = cxClient / 3;
    int comboboxHeight = 0;
    const int flags = 0;
    // 树型框占用左边 1/3 的位置
    SetWindowPos(hTree, HWND_TOP, left, top, w, cyClient, flags);

    w += 5;
    left += w;
    w = cxClient - w;
    SetWindowPos(hCombobox, HWND_TOP, left, top, w, 400, flags);
    RECT rcCombobox;
    GetWindowRect(hCombobox, &rcCombobox);
    comboboxHeight = rcCombobox.bottom - rcCombobox.top;

    top += comboboxHeight + 2;
    SetWindowPos(hBtn, HWND_TOP, left, top, 44, 21, flags);
    SetWindowPos(hType, HWND_TOP, left + 44 + 4, top, w - 44 - 4, 21, flags);

    top += 23;
    SetWindowPos(hEdit, HWND_TOP, left, top, w, cyClient - top, flags);
}

static bool _tmp_no_change = false;
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnStateSize_lr(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    bool isAutoHide = IsAutoHide(s_info->hState);
    RECT rc, rcLeft;
    GetClientRect(s_info->eWnd->GetStateTab(), &rc);
    GetWindowRect(s_info->hTabStateLeft, &rcLeft);

    const int cxTab = rc.right - rc.left;
    const int cyTab = rc.bottom - rc.top;

    const int cxClient = rcLeft.right - rcLeft.left;
    const int cyClient = rcLeft.bottom - rcLeft.top;
    MoveWindow(s_info->hTabStateRight, cxClient, 0, cxTab - cxClient, cyClient, true);
    _tab_l_r_ItemChange(isAutoHide ? 2 : -1);

    if (!isAutoHide && _tmp_no_change == false)
    {
        m_tab_left_proportion = (int)((float)cxClient / (float)cxTab * 10000.f);

        auto pfn_timer = [](HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
        {
            KillTimer(hWnd, id);
            const LPCWSTR appName = L"EWindowSize";
            s_info->ini->write(appName, L"left_tab_proportion", m_tab_left_proportion);
        };
        
        SetTimer(hWnd, (UINT_PTR)&pfn_timer, 300, pfn_timer);  // 延迟一秒保存到配置项
    }
    //pData->isReturn = true;
    return 0;
}

// 底边的状态夹容器, 主要是变量表需要修改, 其他都是一个窗口
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnStateSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    static int s_i;
    m_move_end = true;
    //wstr::dbg(L"状态夹 重画次数 = %d\n", ++s_i);
    HDWP hDwp = 0;
    RECT& rcState = _get_rect_state();
    const bool isAutoHide = &rcState == &s_info->rcState1;
    IRE_STRUCT_AUTOHIDE_MOVE evt;
    evt.hWnd = s_info->hState;
    evt.rc = rcState;
    int evtCount = 0;
    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;

    if (memcmp(&rcState, &pData->pos.rcClient, sizeof(RECT)) != 0)
    {
        rcState = pData->pos.rcClient;
        if (!isAutoHide)   // 不是自动隐藏的才移动工作夹/组件箱
        {
            // 需要移动工作夹和组件箱
            // MDI是根据工作夹和组件箱决定位置的
            // 工作夹是根据状态夹调整位置
            EWindow_MoveWork(hDwp);             // 5. 移动左边的工作夹
            EWindow_MoveControl(hDwp);          // 6. 移动组件箱, 组件箱根据状态夹来调整位置

            EWindow_MoveMDI_Tab_Cbx(hDwp);      // 7,8,9 移动 MDI, 自绘选择夹, 两个组合框
        }
        else
        {
            evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_DRAW_AUTOHIDE_MOVE, &evt, &isReturn, false);
        }
    }

    pData->isReturn = true;
    if (!IsWindowVisible(hWnd)) return 0;
    int cxClient = rcState.right - rcState.left;
    int cyClient = rcState.bottom - rcState.top;



    RECT rcMove = { 0, s_info->scale(WINDOW_TOP_STATE) + WINDOW_SPACING, cxClient, cyClient };    // 预留拖放空间
    MoveWindowEx(s_info->eWnd->GetState32770Parent(), rcMove, true);
    rcMove.bottom -= rcMove.top;
    rcMove.left = 0;
    rcMove.top = 0;
    MoveWindowEx(s_info->eWnd->GetState32770(), rcMove, true);
    rcMove.top -= 50;
    rcMove.bottom += 50;
    MoveWindowEx(s_info->eWnd->GetStateTab(), rcMove, true);
    rcMove.top += 50;
    rcMove.bottom -= 50;

    if (m_tab_left_proportion == 0)
    {
        const LPCWSTR appName = L"EWindowSize";
        m_tab_left_proportion = s_info->ini->read_int(appName, L"left_tab_proportion", 5000);
    }
    _tmp_no_change = true;
    rcMove.left = 0;
    rcMove.top = 0;
    if (TabCode_GetRightItemCount() > 0)
    {
        rcMove.right = (int)((float)m_tab_left_proportion / 10000.f * (float)cxClient) + 1;
    }
    else
    {
        rcMove.right = cxClient;
    }
    MoveWindowEx(s_info->hTabStateLeft, rcMove, true);

    rcMove.left = rcMove.right;
    rcMove.right = cxClient;
    if (isAutoHide)
        rcMove.left = 0;
    MoveWindowEx(s_info->hTabStateRight, rcMove, true);
    _tmp_no_change = false;


    //_tab_l_r_ItemChange(isAutoHide ? 2 : -1);
    // 调用这个会触发选项被改变, 易语言会把窗口显示出来, 这里直接调用自己的处理函数
    //SendMessageW(s_info->eWnd->GetState32770(), WM_NOTIFY, 0, (LPARAM)&hdr);
    if (isAutoHide && evtCount)
    {
        s_info->evt.pfn_Event_callback_after(arr, IRE_DRAW_AUTOHIDE_MOVE, &evt, evtCount, false);
    }
    pData->isReturn = true;
    return 0;
}


inline static HWND Wnd_State_GetWindowFromIndex(int index, LPARAM param)
{
    HWND hWndShow = 0;
    switch (param)
    {
    case TAB_PARAM_TIPS:     // 提示
        hWndShow = s_info->eWnd->GetStateTabTips();
        break;
    case TAB_PARAM_OUTPUT:     // 输出
        hWndShow = s_info->eWnd->GetStateTabOutput();
        break;
    case TAB_PARAM_CALLTABLE:     // 调用表
        hWndShow = s_info->eWnd->GetStateTabCallTable();
        break;
    case TAB_PARAM_MONITORTABLE:     // 监视表
        hWndShow = s_info->eWnd->GetStateTabMonitorTable();
        break;
    case TAB_PARAM_VARTABLE:     // 变量表
        hWndShow = s_info->eWnd->GetStateTabVarTableStatic();
        if (IsWindowVisible(hWndShow)) break;   // 显示这个组件表示现在不是调试状态, 组件位置和其他组件一样

        hWndShow = s_info->eWnd->GetStateVarTableTree();
        break;
    case TAB_PARAM_FIND1:     // 搜寻1
        hWndShow = s_info->eWnd->GetStateTabFind1();
        break;
    case TAB_PARAM_FIND2:     // 搜寻2
        hWndShow = s_info->eWnd->GetStateTabFind2();
        break;
    case TAB_PARAM_CLIP:     // 剪辑历史
        hWndShow = s_info->eWnd->GetStateTabClip();
        break;
    default:
        return 0;
    }
    return hWndShow;
}

inline int __GetState_ECtl(HWND** pHwnds)
{
    IGetEWindow& wnd = *s_info->eWnd;
    static HWND hWnds[] =
    {
        //wnd.GetState(),
        //wnd.GetStateTab(),
        //wnd.GetState32770(),
        //wnd.GetState32770Parent(),
        wnd.GetStateTabOutput(),
        wnd.GetStateTabTips(),
        wnd.GetStateTabVarTableEdit(),
        wnd.GetStateTabCallTable(),
        wnd.GetStateStackCombobox(),
        wnd.GetStateTypeSize(),
        wnd.GetStateVarTableTree(),
        wnd.GetStateTabClip(),
        wnd.GetStateTabFind1(),
        wnd.GetStateTabFind2(),
        wnd.GetStateTabMonitorTable(),
        wnd.GetStateTabVarTableWriteBtn(),
        wnd.GetStateTabVarTableStatic(),
    };
    *pHwnds = hWnds;
    return (sizeof(hWnds) / sizeof(hWnds[0]));
}

// 判断是否是易语言状态夹自带的组件, 不包括选择夹以及更上层的窗口
inline bool __isState_ECtl(HWND hWnd)
{
    HWND* hWnds;
    int size = __GetState_ECtl(&hWnds);
    for (int i = 0; i < size; i++)
    {
        if (hWnds[i] == hWnd)
            return true;
    }
    return false;
}

// 显示选择夹指定的子夹, 把子夹对应的窗口显示出来
inline void _tab_show_curSelItem(HWND hTab)
{
    int index = TabCtrl_GetCurSel(hTab);
    if (index == -1)
        return;
    TCITEMW item = { 0 };
    item.mask = TCIF_PARAM;
    TabCtrl_GetItem(hTab, index, &item);
    bool showWndIsVarCtl = false;

    const HWND hWnds[] =
    {
        s_info->eWnd->GetStateVarTableTree(),
        s_info->eWnd->GetStateStackCombobox(),
        s_info->eWnd->GetStateTabVarTableWriteBtn(),
        s_info->eWnd->GetStateTypeSize(),
        s_info->eWnd->GetStateTabVarTableEdit(),
    };

    HWND hWndShow = 0;
    switch (item.lParam)
    {
    case TAB_PARAM_TIPS:     // 提示
        hWndShow = s_info->eWnd->GetStateTabTips();
        break;
    case TAB_PARAM_OUTPUT:     // 输出
        hWndShow = s_info->eWnd->GetStateTabOutput();
        break;
    case TAB_PARAM_CALLTABLE:     // 调用表
        hWndShow = s_info->eWnd->GetStateTabCallTable();
        break;
    case TAB_PARAM_MONITORTABLE:     // 监视表
        hWndShow = s_info->eWnd->GetStateTabMonitorTable();
        break;
    case TAB_PARAM_VARTABLE:     // 变量表
    {
        if (s_info->isStop && s_info->isRun)    // 停止按钮是启动状态, 并且运行按钮也是启用状态, 那当前就是中断了, 需要显示那一堆组件
        {
            hWndShow = 0;
            showWndIsVarCtl = true;
            ShowWindowEx(hWndShow, SW_HIDE);    // 隐藏那个标签, 然后显示那一堆组件
            for (int i = 0; i < sizeof(hWnds) / sizeof(hWnds[0]); i++)
                ShowWindowEx(hWnds[i], SW_SHOW);
            break;
        }

        hWndShow = s_info->eWnd->GetStateTabVarTableStatic();
        for (int i = 0; i < sizeof(hWnds) / sizeof(hWnds[0]); i++)
            ShowWindowEx(hWnds[i], SW_HIDE);    // 把变量表其他组件隐藏
        break;
    }
    case TAB_PARAM_FIND1:     // 搜寻1
        hWndShow = s_info->eWnd->GetStateTabFind1();
        break;
    case TAB_PARAM_FIND2:     // 搜寻2
        hWndShow = s_info->eWnd->GetStateTabFind2();
        break;
    case TAB_PARAM_CLIP:     // 剪辑历史
        hWndShow = s_info->eWnd->GetStateTabClip();
        break;
    default:
        return;
    }

    auto pfn_isVarCtl = [&hWnds](HWND hWnd) ->bool
    {
        for (int i = 0; i < sizeof(hWnds) / sizeof(hWnds[0]); i++)
        {
            if (hWnd == hWnds[i])
                return true;
        }
        return false;
    };
    HWND hChild = GetWindow(hTab, GW_CHILD);
    while (hChild)  // 把选择夹下不是当前显示的窗口给隐藏, 只隐藏易语言自带窗口
    {
        // 当前需要显示的窗口和选择夹下的窗口不一样, 需要进一步判断是否需要隐藏
        if (hChild != hWndShow && IsWindowVisible(hChild))
        {
            // 如果当前选中的是变量表, 并且枚举到的窗口也是变量表里的窗口
            if (showWndIsVarCtl && pfn_isVarCtl(hChild))
            {
                hChild = GetWindow(hTab, GW_HWNDNEXT);
                continue;   // 都是变量表里的就不处理
            }
            
            if (__isState_ECtl(hChild))
                ShowWindowEx(hChild, SW_HIDE);  // 不是需要显示的窗口, 并且是易语言自带的窗口, 需要隐藏

        }
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }

    ShowWindowEx(hWndShow, SW_SHOW);
}



LRESULT CALLBACK WndProc_OnStateTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage)
{
    IRE_STRUCT_TAB_SIZE evt = { 0 };
    RECT& rc = evt.rc;  // 计算出来的选择夹的客户区, 窗口应该在这个客户区内
    HWND hWndShow = 0;
    LPNMHDR hdr = (LPNMHDR)lParam;
    evt.hWnd = hdr->hwndFrom;
    evt.index = TabCtrl_GetCurSel(hdr->hwndFrom); // 选择夹选择改变
    TCITEMW item = { 0 };
    item.mask = TCIF_PARAM;
    TabCtrl_GetItem(hdr->hwndFrom, evt.index, &item);
    RECT& rcState = _get_rect_state();
    const bool isAutoHide = &rcState == &s_info->rcState1;


    TAB_SEL_CHANGE_STRUCT* pOlds = (TAB_SEL_CHANGE_STRUCT*)GetPropW(hdr->hwndFrom, oldSelParamProp);
    if (!pOlds)
    {
        pOlds = new TAB_SEL_CHANGE_STRUCT;
        pOlds->hWnds = new std::vector<HWND>;
        pOlds->hWnds->reserve(20);
        pOlds->param = -1;
        SetPropW(hdr->hwndFrom, oldSelParamProp, pOlds);
    }


    NMHDR tmpHdr = { 0 };
    if (hdr->hwndFrom != s_info->hTabStateLeft && hdr->hwndFrom != s_info->hTabStateRight)
    {
        //_tab_show_curSelItem(s_info->hTabStateLeft);
        //_tab_show_curSelItem(s_info->hTabStateRight);
        const LPCWSTR prop = L"{9AAA4259-9DDD-4D53-9E74-C9C1CDEAF315}";
        std::vector<int>* params = (std::vector<int>*)GetPropW(s_info->hEWnd, prop);

        bool isRight = false;   // 是否添加到右边的选择夹, 如果为false, 就添加到左边
        for (auto it = params->begin(); it != params->end(); ++it)
        {
            if (item.lParam == *it)
            {
                isRight = true;
                break;
            }
        }
        HWND hWndMove = isRight ? s_info->hTabStateRight : s_info->hTabStateLeft;
        if (isAutoHide)
        {
            RECT rcTab;
            GetWindowRect(s_info->eWnd->GetState32770(), &rcTab);
            HWND hParent = GetParent(s_info->eWnd->GetStateTab());
            ScreenToClient(hParent, (LPPOINT)&rcTab);
            ScreenToClient(hParent, ((LPPOINT)(&rcTab)) + 1);
            SetWindowPos(hWndMove, HWND_TOP, rcTab.left, rcTab.top, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top, 0);
            //_tab_l_r_ItemChange(isRight ? 1 : 0);
        }
        tmpHdr.hwndFrom = hWndMove;
        tmpHdr.idFrom = 0;
        tmpHdr.code = hdr->code;
        hdr = &tmpHdr;

        if (Tab_GetItemParam(hWndMove) != item.lParam)  // 当前选中的和切换过来的选中项不一样, 需要设置新的选中项
        {
            int len = TabCtrl_GetItemCount(hWndMove);
            for (int i = 0; i < len; i++)
            {
                if (Tab_GetItemParam(hWndMove, i) == item.lParam)
                {
                    TabCtrl_SetCurSel(hWndMove, i);
                    break;
                }
            }
        }
    }



    if (isAutoHide && 0)
    {
        evt.direction = _tab_calc_client(hdr->hwndFrom, rcState, rc);
    }
    else
    {
        RECT rcTmp;
        GetClientRect(hdr->hwndFrom, &rcTmp);
        evt.direction = _tab_calc_client(hdr->hwndFrom, rcTmp, rc);
    }

    _tab_show_curSelItem(s_info->hTabStateLeft);
    _tab_show_curSelItem(s_info->hTabStateRight);
    if (isAutoHide && s_info->iDraw->TopState_GetActivateWindow() == s_info->hState)
    {
        rc = pData->pos.rcClient;
        rc.left = 1;
        rc.top = 1;
        rc.right--;
        rc.bottom--;
        //rc.right = (rcState.right - rcState.left) - 1;
        //rc.bottom = (rcState.bottom - rcState.top) - (WINDOW_TOP_STATE + WINDOW_SPACING) - 1;
        //rc.bottom -= (WINDOW_AUTO_HEIGHT);
        //rc.right -= (WINDOW_AUTO_HEIGHT);
    }



    HWND hParent32770 = s_info->eWnd->GetState32770Parent();
    if (!IsWindowVisible(hParent32770))
    {
        ShowWindowEx(hParent32770, SW_SHOW);
    }

    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
    int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_TAB_STATE_SIZE, &evt, &isReturn, false);
    if (isReturn) return 0;

    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    //const int flags = SWP_NOZORDER | SWP_SHOWWINDOW;
    int flags = SWP_SHOWWINDOW;
    const int asad = TCIF_TEXT;
    switch (item.lParam)
    {
    case TAB_PARAM_VARTABLE:     // 变量表
    {
        _tab_move_vartable(item.lParam, rc, pOlds);
        s_info->evt.pfn_Event_callback_after(arr, IRE_TAB_STATE_SIZE, &evt, evtCount, false);
        return 0;
    }
    default:
        hWndShow = Wnd_State_GetWindowFromIndex(evt.index, item.lParam);
        break;
    }

    if (item.lParam != pOlds->param)   // 切换到新的窗口
    {
        for (auto it = pOlds->hWnds->begin(); it != pOlds->hWnds->end(); ++it)
            ShowWindowEx(*it, SW_HIDE);   // 隐藏旧窗口
        pOlds->hWnds->clear();
        pOlds->hWnds->push_back(hWndShow);
        pOlds->param = item.lParam;
    }

    static int hideCount;
    if (hideCount < 10)
    {
        hideCount++;
        int indexLeft = TabCtrl_GetCurSel(s_info->hTabStateLeft);
        int indexRight = TabCtrl_GetCurSel(s_info->hTabStateRight);
        TCITEMW item1 = { 0 };
        TCITEMW item2 = { 0 };
        item1.mask = TCIF_PARAM;
        item2.mask = TCIF_PARAM;
        TabCtrl_GetItem(s_info->hTabStateLeft, indexLeft, &item1);
        TabCtrl_GetItem(s_info->hTabStateRight, indexRight, &item2);
        if (item1.lParam != TAB_PARAM_VARTABLE && item2.lParam != TAB_PARAM_VARTABLE)
            __hide_vartable(true);  // 左右两边都没有选中变量表的情况下才隐藏变量表的组件
    }
    // 不是变量表的组件位置都一样
    if (hWndShow)
        SetWindowPos(hWndShow, HWND_TOP, rc.left, rc.top, cxClient, cyClient, flags);
    s_info->evt.pfn_Event_callback_after(arr, IRE_TAB_STATE_SIZE, &evt, evtCount, false);
    return 0;
}

void EWindow_StateClick_State(LPTOPSTATE_INFO info, int id)
{
    switch (id)
    {
    case TOP_STATE_BUTTON_CLOSE:    // 关闭按钮, 需要隐藏这个窗口
    {
        wstr::dbg(L"关闭按钮被按下\n");
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

