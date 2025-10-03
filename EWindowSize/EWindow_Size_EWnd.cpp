#include "EWindow_Size_Header.h"
#include <iDraw_const.h>





_EWINDOW_BEGIN_NAMESPACE
void EWindow_MoveConmboBox(HDWP& hDwp);

LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    static int s_i;
    //wstr::dbg(L"易语言主窗口 重画次数 = %d\n", ++s_i);
    if (IsIconic(hWnd)) return 0;

    //GetWindowRect(hWnd, &s_info->rcEWindow);
    if (pData->pos.rc.right <= 0)
        GetWindowRect(hWnd, &pData->pos.rc);
    //s_info->rcEWindow = pData->pos.rc;
    if (!s_info->rcEWindow.right || !s_info->rcEWindow.bottom)
    {
        GetWindowRect(s_info->hEWnd, &s_info->rcEWindow);
        s_info->cxClient = s_info->rcEWindow.right - s_info->rcEWindow.left;
        s_info->cyClient = s_info->rcEWindow.bottom - s_info->rcEWindow.top;
    }
    //lParam = MAKELONG(cxClient, cyClient);
    //if (LOWORD(lParam) != cxClient || HIWORD(lParam) != cyClient) return 0;

    //HDWP hDwp = BeginDeferWindowPos(0);
    HDWP hDwp = 0;
/*
    // 每走一步都将位置记录好, 下一次移动都根据上一个的位置进行修改

    1. 先移动左上角的菜单
    2. 在移动顶边的菜单栏容器
    3. 移动底边状态条, 记录好状态条的位置, 不管可视与否, 状态夹都根据这个位置移动, 状态条根据易语言窗口调整
    4. 移动状态夹, 根据易语言窗口和状态条调整
    5. 移动左边的工作夹
    6. 移动组件箱, 这里必须计算好组件箱的位置, MDI是根据组件箱决定右边的
    7. 移动MDI, 内部会根据是否显示组件箱进行移动组件箱, 不管可视与否, 组件箱的位置都会记录
    8. 移动自绘选择夹
    9. 移动两个组合框
*/
    EWindow_MoveMenu(hDwp);             // 1. 先移动左上角的菜单
    EWindow_MoveMenuWindow(hDwp);       // 2. 在移动顶边的菜单栏容器
    if (s_info->isReady)
    {

        EWindow_MoveStatus(hDwp);           // 3. 移动底边状态条
        EWindow_MoveState(hDwp);            // 4. 移动状态夹
        EWindow_MoveWork(hDwp);             // 5. 移动左边的工作夹
        EWindow_MoveControl(hDwp);          // 6. 移动组件箱
        EWindow_MoveMDI_Tab_Cbx(hDwp);      // 7,8,9 移动 MDI, 自绘选择夹, 两个组合框
        MoveWindow(s_info->hStatusBar, 0, 0, 0, 0, true);
        static HWND hWnd32770Parent = s_info->eWnd->GetControl32770Parent();
        if (!IsWindowVisible(hWnd32770Parent))
            ShowWindowEx(hWnd32770Parent, SW_SHOW);
    }

    //EndDeferWindowPos(hDwp);

    return 0;
}

// 移动 MDI, 自绘选择夹, 两个组合框
// 这几个组件一般都是动一个就全部都得动
// 先移动MDI, 在移动自绘选择夹, 最后移动组合框, 其他组件都是根据MDI计算窗口位置
void EWindow_MoveMDI_Tab_Cbx(HDWP& hDwp)
{
    EWindow_MoveMDIClient(hDwp);        // 7. 移动MDI
    EWindow_MoveCustomTab(hDwp);        // 8. 移动自绘选择夹
    EWindow_MoveConmboBox(hDwp);        // 9. 移动两个组合框
}


void EWindow_MoveCustomTab(HDWP& hDwp)
{
    // 左边 = MDI窗口左边
    // 顶边 = 如果是在顶边显示, 那就是菜单栏底边, 否则就是MDI的底边
    // 右边 = MDI右边
    // 底边 = 顶边 + 高度, 高度固定
    bool isTop = s_info->config->tab_code_direction() != 3;    // 代码选择夹的子夹方向, 3=底边, 其他 = 顶边

    s_info->rcCustomTab.left    = s_info->rcMDIClient.left;
    s_info->rcCustomTab.top     = (isTop ? s_info->rcWndMenu.bottom : s_info->rcMDIClient.bottom);
    s_info->rcCustomTab.right   = s_info->rcMDIClient.right;
    s_info->rcCustomTab.bottom  = s_info->rcCustomTab.top + s_info->scale(WINDOW_CUSTOMTAB_HEIGHT);

    InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
}

void EWindow_MoveConmboBox(HDWP& hDwp)
{
    // 左边 = MDI窗口左边
    // 顶边 = MDI顶边 - 组合框高度
    // 右边 = 左边组合框 = MDI窗口宽度40%, 右边组合框=MDI右边
    // 底边 = MDI顶边
    static HWND hComboBox1 = s_info->eWnd->GetMenuComboBox1();
    static HWND hComboBox2 = s_info->eWnd->GetMenuComboBox2();
    if (!IsWindow(hComboBox1)) return;

    if (!g_moveInfo.heightCombobox)
    {
        RECT rc;
        GetWindowRect(hComboBox1, &rc);
        g_moveInfo.heightCombobox = rc.bottom - rc.top;
        if (g_moveInfo.heightCombobox <= 0)
            g_moveInfo.heightCombobox = 20;
    }

    if (!s_info->isReady)
        return;

    const int heightCombobox = s_info->scale(g_moveInfo.heightCombobox);
    RECT& rcMDIClient = s_info->rcMDIClient;
    const int width = rcMDIClient.right - rcMDIClient.left;
    RECT rc1, rc2;
    rc1.left    = rcMDIClient.left;
    rc1.top     = rcMDIClient.top - heightCombobox;
    rc1.right   = rc1.left + width * 40 / 100;
    rc1.bottom  = rcMDIClient.top;
    if (!s_info->config->IsShowLocationTool())
    {
        rc1.bottom = rc1.top;
    }
    else
    {
        if (!IsWindowVisible(hComboBox1))
            ShowWindowEx(hComboBox1, SW_SHOW);
        if (!IsWindowVisible(hComboBox2))
            ShowWindowEx(hComboBox2, SW_SHOW);
    }

    rc2.left    = rc1.right;
    rc2.top     = rc1.top;
    rc2.right   = rcMDIClient.right;
    rc2.bottom  = rc1.bottom;

    const DWORD flags = SWP_DRAWFRAME;
    SetWindowPos(hComboBox1, HWND_BOTTOM, rc1.left, rc1.top, rc1.right - rc1.left, 600, flags);
    SetWindowPos(hComboBox2, HWND_BOTTOM, rc2.left, rc2.top, rc2.right - rc2.left, 600, flags);

    SendMessageA(hComboBox1, CB_SETITEMHEIGHT, -1, heightCombobox - 6);
    SendMessageA(hComboBox2, CB_SETITEMHEIGHT, -1, heightCombobox - 6);

    //EWindow_DeferWindowPos(hDwp, hComboBox1, rc1, true);
    //EWindow_DeferWindowPos(hDwp, hComboBox2, rc2, true);
}

void EWindow_MoveMenu(HDWP& hDwp)
{
    //if (s_info->rcMenu.right == 0 || s_info->rcMenu.bottom == 0)
    //    GetClientRect(s_info->hMenu, &s_info->rcMenu);
    //const int cxChild = s_info->rcMenu.right - s_info->rcMenu.left;
    //const int cyChild = s_info->rcMenu.bottom - s_info->rcMenu.top;
    //s_info->rcMenu.top = 1;
    //s_info->rcMenu.left = WINDOW_CAPTION_HEIGHT;
    //s_info->rcMenu.right = s_info->rcMenu.left + cxChild;
    //s_info->rcMenu.bottom = WINDOW_CAPTION_HEIGHT - 1;
    //EWindow_DeferWindowPos(hDwp, s_info->hMenu, s_info->rcMenu, true);
}

void EWindow_MoveStatus(HDWP& hDwp)
{
    const bool isVisibleStatusBar = IsWindowVisible(s_info->hStatusBar);
    const int height = s_info->scale(WINDOW_STATUS_HEIGHT);
    const int border = s_info->scale(WINDOW_BORDER_WIDTH);

    // 状态条位置, 左边是预留边框的位置
    // 顶边是窗口客户区高度 - 状态条的高度 - 预留边框的高度
    // 右边 = 易语言窗口 - 预留的宽度
    // 左右底 都可以通过易语言窗口计算出来, 顶边是根据是否可视计算
    // 如果可视, 顶边就是底边-状态条高度
    // 如果不可视, 顶边就和底边一样大, 这样高度就是0
    s_info->rcStatusBar.left    = border;
    s_info->rcStatusBar.right   = s_info->cxClient - border;
    s_info->rcStatusBar.bottom  = s_info->cyClient - border;


    s_info->rcStatusBar.top = (isVisibleStatusBar)
        ? (s_info->rcStatusBar.bottom - height)
        : (s_info->rcStatusBar.bottom);
    
    // 一个是 窗口底边 - 状态夹高度-边框
    // 一个是 状态夹底边 - 边框
    // 如果两个值不一样, 那就是计算错误了
    assert((s_info->cyClient - height - border) == (s_info->rcStatusBar.bottom - height));
    EWindow_DeferWindowPos(hDwp, s_info->hStatusBar, s_info->rcStatusBar, true);
}


// 判断当前选中的是否是窗口设计器
int EWindow_IsSelWindow()
{
    g_moveInfo.hChild1 = 0;
    g_moveInfo.hChild2 = 0;
    g_moveInfo.hChild3 = 0;
    g_moveInfo.hChild4 = 0;
    ICustomCodeTab& arr = *s_info->hWnds;
    if (arr.empty()) return -1;    // 成员为0, 表示所有的选项都关了, 肯定没有显示窗口设计器

    HWND hTab = s_info->hEWnd;
    const int index = TabCtrl_GetCurSel(hTab);    // 当前选择夹选中项
    if ((int)arr.size() <= index || index < 0)    // 选择夹当前选中的有问题, 这个应该就是bug了, 调试版下个断点
    {
//#ifdef _DEBUG
//        __debugbreak();
//#endif
        return -1;
    }

    return EWindow_GetMDIClientChild(arr[index].hWnd);
}

// 获取这个窗口, 如果这个窗口id和传入的id不同, 则取这个窗口的下一个同级窗口
HWND _mdi_getwindow(HWND& hWnd, int id)
{
    while (hWnd)
    {
        if (GetDlgCtrlID(hWnd) == id)
            return hWnd;
        hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    }
    return hWnd;
}

// 获取MDI下当前显示的窗口, 包括子窗口, 一共获取4层
int EWindow_GetMDIClientChild(HWND hChild)
{
    if (!hChild)
        hChild = GetWindow(s_info->hMDIClient, GW_CHILD);
    if (!hChild) return -1;

    int nChild3ID = 0;
    while (1)
    {
        g_moveInfo.hChild1 = hChild;    // 当前MDI里面显示的窗口
        while (g_moveInfo.hChild1)
        {
            int id = GetDlgCtrlID(g_moveInfo.hChild1);
            if (id >= 65280 && id < 66000)  // 第一层窗口是 65280 开始
                break;
            g_moveInfo.hChild1 = GetWindow(g_moveInfo.hChild1, GW_HWNDNEXT);
        }
        if (!g_moveInfo.hChild1)
            break;

        g_moveInfo.hChild2 = GetWindow(g_moveInfo.hChild1, GW_CHILD);    // mdi下的第二层窗口
        if (!_mdi_getwindow(g_moveInfo.hChild2, 59648))
            break;


        g_moveInfo.hChild3 = GetWindow(g_moveInfo.hChild2, GW_CHILD);    // MDI下的第三层窗口, 根据这个窗口判断是写代码还是起始页或者是窗口设计器
        if (!g_moveInfo.hChild3)break;

        while (g_moveInfo.hChild3)  // 1=设计器或者起始页, 59648=代码区, 只要不是这两种情况都继续往后取
        {
            // 这里的做法是排除滚动条
            nChild3ID = GetDlgCtrlID(g_moveInfo.hChild3);
            if (nChild3ID == 1 || nChild3ID == 59648) break;
            g_moveInfo.hChild3 = GetWindow(g_moveInfo.hChild3, GW_HWNDNEXT);
        }
        break;
    }

    int nRet = 0;
    if (nChild3ID == 1)    // mdi下的第三层窗口id = 1 要么是窗口设计器, 要么是起始页, 代码的话id=59648
    {
        g_moveInfo.hChild4 = GetWindow(g_moveInfo.hChild3, GW_CHILD);
        // 117是窗口设计器的窗口, 等于117就返回1, 否则返回2, 1=窗口设计器, 2=起始页, 0=代码区
        nRet = (GetDlgCtrlID(g_moveInfo.hChild4) == 117) ? 1 : 2;
    }
    else
    {
        if (g_moveInfo.hChild3 && nChild3ID == 59648)     // 代码框父窗口, 修改这个里面的代码区窗口也会改变
        {
            g_moveInfo.hChild4 = GetWindow(g_moveInfo.hChild3, GW_CHILD);
            if (g_moveInfo.hChild4)
            {
                while (g_moveInfo.hChild4)
                {
                    if (GetDlgCtrlID(g_moveInfo.hChild4) == 1)
                        break;
                    g_moveInfo.hChild4 = GetWindow(g_moveInfo.hChild4, GW_HWNDNEXT);
                }
            }
        }
    }
    g_moveInfo.windowType = nRet;
    return nRet;
}

inline BOOL _insert_menu(HMENU hMenu, UINT id, LPCWSTR name)
{
    MENUITEMINFOW mi = { 0 };
    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_ID | MIIM_STRING | MIIM_DATA;
    mi.wID = id;
    mi.dwTypeData = (LPWSTR)name;
    mi.dwItemData = (LONG_PTR)hMenu;
    return InsertMenuItemW(hMenu, -1, true, &mi);
}

void EWindow_PopupMenu(LPTOPSTATE_INFO info)
{
    static HWND hControl = s_info->eWnd->GetControl();
    static HWND hWork = s_info->eWnd->GetWork();
    static HWND hState = s_info->eWnd->GetState();

    HMENU& hMenu = g_moveInfo.hMenu;
    if (!hMenu)
    {
        hMenu = (HMENU)s_info->pfnControls(IDC_IDRAW_CREATEPOPUPMENU, 0, 0);
        _insert_menu(hMenu, IDMENU_FLOAT, L"浮动(&F)");
        _insert_menu(hMenu, IDMENU_STOP, L"停靠(&K)");
        _insert_menu(hMenu, IDMENU_AUTOHIDE, L"自动隐藏(&T)");
        _insert_menu(hMenu, IDMENU_CLOSE, L"关闭(&C)");
    }
    g_moveInfo.pMenuClick = info;

    //bool isShow = false;    // 如果窗口是隐藏的, 那就自动隐藏菜单被禁止

    //
    //if (info->hWnd == hControl)
    //    isShow = s_info->config->tab_control_isShow();
    //else if (info->hWnd = hState)
    //    isShow = s_info->config->tab_state_isShow();
    //else if (info->hWnd == hWork)
    //    isShow = s_info->config->tab_work_isShow();

    const bool isAutoHide   = __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE); // 是否自动隐藏
    const bool isFloat      = __query_flags(info, TOP_STATE_BUTTON_ISFLOAT);    // 是否浮动
    const bool isStop       = __query_flags(info, TOP_STATE_BUTTON_ISSTOP);     // 是否停靠


    MENUITEMINFOW mi = { 0 };
    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_STATE;
    //mi.fState = isAutoHide ? MFS_DISABLED : MFS_ENABLED;    // 当前状态是自动隐藏就禁止自动隐藏菜单
    //SetMenuItemInfoW(hMenu, (UINT)IDMENU_AUTOHIDE, false, &mi);

    mi.fState = isFloat ? MFS_DISABLED : MFS_ENABLED;       // 当前状态是浮动就禁止自动隐藏菜单
    SetMenuItemInfoW(hMenu, (UINT)IDMENU_FLOAT, false, &mi);

    mi.fState = isStop ? MFS_DISABLED : MFS_ENABLED;        // 当前状态是停靠就禁止自动隐藏菜单
    SetMenuItemInfoW(hMenu, (UINT)IDMENU_STOP, false, &mi);

    POINT pt;
    GetCursorPos(&pt);
    TrackPopupMenu(g_moveInfo.hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, info->hWnd, 0);
}

_EWINDOW_END_NAMESPACE