// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "EWindow_Size_Header.h"
#include <control/CMenu.h>
#include <iDraw_const.h>
#include <iDraw_Version.h>
static PLUGIN_INFO pluginInfo;

IDRAW_INFO* s_info;     // 包含各个窗口的信息, 支持库和其他动态库都有使用, 支持库调用动态库的时候会传递这个指针过去
_EWINDOW_NAMESPACE EWINDOW_SIZE_MOVEINFO _EWINDOW_NAMESPACE g_moveInfo;

LRESULT CALLBACK EWindowProc_AllSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData);


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

// 把那些不需要显示在易语言窗口上的组件位置强制修改掉
LRESULT CALLBACK EWindowProc_AllWindowPosChanging(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
    // 没改变尺寸就返回
    if (!((pos->flags & SWP_NOSIZE) == 0 || (pos->flags & SWP_NOMOVE) == 0))
        return 0;

    _EWINDOW_NAMESPACE EWINDOW_SIZE_MOVEINFO& move = _EWINDOW_NAMESPACE g_moveInfo;
    //HWND hParent = GetParent(hWnd);
    //if (hParent == s_info->hMDIClient
    //    || GetParent(hParent) == s_info->hMDIClient)
    //{
    //    pos->x = 0;
    //    pos->y = 0;
    //    pos->cx = s_info->rcMDIClient.right - s_info->rcMDIClient.left;
    //    pos->cy = s_info->rcMDIClient.bottom - s_info->rcMDIClient.top;
    //    pData->isReturn = true;
    //    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    //}


    static HWND hCodeTab = s_info->eWnd->GetCodeTab();
    if (hWnd == hCodeTab)
    {
        pos->x = -1;
        pos->y = -1;
        pos->cx = 1;
        pos->cy = 1;

        pData->isReturn = true;
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    if (s_info->eWnd->isControlWindow(hWnd))
    {
        static HWND hControl32770 = s_info->eWnd->GetControl32770();
        static HWND hControl32770Parent = s_info->eWnd->GetControl32770Parent();
        static HWND hControlTree = s_info->eWnd->GetControlTree();
        RECT& rcControl = _EWINDOW_NAMESPACE IsAutoHide(s_info->hControl) ? s_info->rcControl1 : s_info->rcControl;
        if (hWnd == hControl32770 || hWnd == hControl32770Parent)
        {
            // 组件箱里的几个组件都固定位置
            pos->x = 0;
            pos->y = 0;
            const int topHeight = s_info->scale(WINDOW_TOP_STATE);
            if (hWnd == hControl32770Parent)
            {
                pos->x = WINDOW_SPACING + 1;
                pos->y = topHeight;
            }
            pos->cx = rcControl.right - rcControl.left - 2 - WINDOW_SPACING;
            pos->cy = rcControl.bottom - rcControl.top - 1 - topHeight;
            pos->flags |= SWP_SHOWWINDOW;
            pData->isReturn = true;
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        }
        const int id = GetDlgCtrlID(hWnd);
        if (id >= 11225 && id < 11255 || hWnd == hControlTree)  // 组件箱里的切换组件列表按钮
        {
            pos->x = 0;
            pos->cx = rcControl.right - rcControl.left - 2 - WINDOW_SPACING;
            pos->flags |= SWP_DRAWFRAME;
            pData->isReturn = true;
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        }
    }

    static HWND hStandard = s_info->eWnd->GetMenuStandard();
    static HWND hAlign = s_info->eWnd->GetMenuAlign();
    if (hWnd == hStandard || hWnd == hAlign || hWnd == s_info->hWndMenu || hWnd == s_info->hWndTool)
    {
        // 菜单栏和菜单栏里面的组件都固定位置
        _EWINDOW_NAMESPACE EWindow_Menu_SetPos(hWnd, pos);
        //pos->x = 1;
        //pos->y = 1;
        //pos->cx = 1;
        //pos->cy = 1;
        //pos->flags &= ~SWP_NOZORDER;
        //pos->hwndInsertAfter = HWND_BOTTOM;
        pData->isReturn = true;
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    //if (hWnd == s_info->hWndMenu)
    //{
    //    if (s_info->rcWndMenu.left != WINDOW_BORDER_WIDTH)
    //    {
    //        s_info->rcWndMenu.left = WINDOW_BORDER_WIDTH;
    //        s_info->rcWndMenu.bottom -= s_info->rcWndMenu.top;
    //        s_info->rcWndMenu.top = s_info->rcMenu.bottom;
    //    }
    //    pos->x  = s_info->rcWndMenu.left;
    //    pos->y  = s_info->rcWndMenu.top;
    //    pos->cx = s_info->rcWndMenu.right - s_info->rcWndMenu.left;
    //    pos->cy = s_info->rcWndMenu.bottom - s_info->rcWndMenu.top;
    //    pData->isReturn = true;
    //    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    //}
    //if (hWnd == s_info->hMenu_Standard)
    //{
    //    int cx = s_info->rcMenu.right - s_info->rcMenu.left;
    //    if (s_info->rcWndMenu.left != WINDOW_BORDER_WIDTH)
    //        cx = s_info->rcMenu.right;
    //    pos->x  = 0;
    //    pos->y  = 0;
    //    pos->cx = cx;
    //    pos->cy = s_info->rcWndMenu.bottom - s_info->rcWndMenu.top;
    //    pData->isReturn = true;
    //    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    //}

    return 0;
}

// 获取选择夹项目数值, index=-1则获取当前选中项
inline int _ESize_Tab_GetItemParam(HWND hTab, int index = -1)
{
    if (index == -1)
        index = TabCtrl_GetCurSel(hTab);
    TCITEMW item = { 0 };
    item.mask = TCIF_PARAM;
    TabCtrl_GetItem(hTab, index, &item);
    return (int)item.lParam;
}

// 如果是默认的选择夹, 那就判断是左边还是右边的选择夹, 然后修改事件结构
// 不是默认选择夹触发的事件就不处理
inline void __cmp_lrtab_set_evt(LPNMHDR hdr, IRE_STRUCT_TAB_SELCHANGE& evt)
{
    if (hdr->hwndFrom != s_info->eWnd->GetStateTab())
        return;


    LPARAM tabParam = _ESize_Tab_GetItemParam(hdr->hwndFrom);
    HWND hWndTabEvent = 0;
    int len = TabCtrl_GetItemCount(s_info->hTabStateLeft);
    for (int i = 0; i < len; i++)
    {
        if (_ESize_Tab_GetItemParam(s_info->hTabStateLeft, i) == tabParam)
        {
            hWndTabEvent = s_info->hTabStateLeft;
            evt.indexOld = TabCtrl_GetCurSel(hWndTabEvent);
            evt.index = i;
            break;
        }
    }

    if (!hWndTabEvent)
    {
        len = TabCtrl_GetItemCount(s_info->hTabStateRight);
        for (int i = 0; i < len; i++)
        {
            if (_ESize_Tab_GetItemParam(s_info->hTabStateRight, i) == tabParam)
            {
                hWndTabEvent = s_info->hTabStateRight;
                evt.indexOld = TabCtrl_GetCurSel(hWndTabEvent);
                evt.index = i;
                break;
            }
        }
    }

    if (hWndTabEvent)
    {
        evt.hWnd = hWndTabEvent;
        evt.hWndOld = 0;
    }
#ifdef _DEBUG
    else
    {
        __debugbreak();
    }
#endif

}

LRESULT CALLBACK EWindowProc_AllNotify(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    LPNMHDR hdr = (LPNMHDR)lParam;
    switch (hdr->code)
    {
    case TCN_SELCHANGE:
    {
        //wstr::dbg(L"error %d已经改变子夹, 索引 = %d\n", hdr->hwndFrom, TabCtrl_GetCurSel(hdr->hwndFrom));
        LRESULT ret = 0;
        bool isCall = false;
        if (hdr->hwndFrom == s_info->eWnd->GetWorkTab())
            ret = _EWINDOW_NAMESPACE WndProc_OnWorkTabItemChanged(hWnd, message, wParam, lParam, pData, true), isCall = true;
        if (hdr->hwndFrom == s_info->eWnd->GetStateTab() || hdr->hwndFrom == s_info->hTabStateLeft || hdr->hwndFrom == s_info->hTabStateRight)
            ret = _EWINDOW_NAMESPACE WndProc_OnStateTabItemChanged(hWnd, message, wParam, lParam, pData, true), isCall = true;
        if(hdr->hwndFrom == s_info->hEWnd)
            ret = _EWINDOW_NAMESPACE WndProc_OnCustomTabItemChanged(hWnd, message, wParam, lParam, pData, true);
        if (isCall)
        {
            const int nEvent = hdr->hwndFrom == s_info->eWnd->GetWorkTab() ? IRE_TAB_WORK_SELCHANGED : IRE_TAB_STATE_SELCHANGED;
            int vl = (int)(LONG_PTR)GetPropW(hdr->hwndFrom, L"tab_old_sel");

            IRE_STRUCT_TAB_SELCHANGE evt = { 0 };
            evt.hWnd = hdr->hwndFrom;
            evt.hTab = hdr->hwndFrom;
            evt.index = TabCtrl_GetCurSel(hdr->hwndFrom);
            HWND hWndTabEvent = 0;
            evt.indexOld = -1;
            if (LOWORD(vl) == 1211)
                evt.indexOld = HIWORD(vl);

            __cmp_lrtab_set_evt(hdr, evt);

            LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; bool isReturn = false;
            int evtCount = s_info->evt.pfnGetEventInfo(nEvent, arr, EVENT_MAX_ITEM);
            s_info->evt.pfn_Event_callback_after(arr, nEvent, &evt, evtCount, false);
        }
        return ret;
    }
    case TCN_SELCHANGING:
    {
        //wstr::dbg(L"error 即将改变子夹\n");
        if (hdr->hwndFrom != s_info->eWnd->GetWorkTab() && hdr->hwndFrom != s_info->eWnd->GetStateTab() && hdr->hwndFrom != s_info->hTabStateLeft && hdr->hwndFrom != s_info->hTabStateRight)
            return 0;
        LPNMHDR hdr = (LPNMHDR)lParam;

        const int nEvent = hdr->hwndFrom == s_info->eWnd->GetWorkTab() ? IRE_TAB_WORK_SELCHANGED : IRE_TAB_STATE_SELCHANGED;

        IRE_STRUCT_TAB_SELCHANGE evt = { 0 };
        evt.hWnd = hdr->hwndFrom;
        evt.hTab = hdr->hwndFrom;
        TCHITTESTINFO tti = { 0 };
        GetCursorPos(&tti.pt);
        ScreenToClient(hdr->hwndFrom, &tti.pt);
        evt.index = TabCtrl_HitTest(hdr->hwndFrom, &tti);   // 为-1表示不是手动点击改变项目
        evt.indexOld = TabCtrl_GetCurSel(hdr->hwndFrom);    // 得到的是改变前的索引, 需要获取到改变后的索引

        SetPropW(hdr->hwndFrom, L"tab_old_sel", (HANDLE)(MAKELONG(1211, evt.indexOld)));
        //TODO 需要做一个开关, 让选择夹不会自动切换, evt.index == -1 需要返回拦截
        __cmp_lrtab_set_evt(hdr, evt);

        LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
        int evtCount = s_info->evt.pfn_Event_callback_before(arr, nEvent, &evt, &isReturn, false);

        if (!isReturn && evt.index == -1)
        {
            // 状态夹不是自动隐藏状态才拦截, 是自动隐藏状态就让他自动切换
            if (hdr->hwndFrom == s_info->eWnd->GetStateTab())
            {
                LPOBJSTRUCT pState = s_info->pfnGetDataFromHwnd(s_info->hState);
                LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pState->param;
                if (!__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
                    isReturn = s_info->config->noChangeTab_state();
            }
            else
            {
                isReturn = s_info->config->noChangeTab_work();
            }
        }
        
        if (isReturn)
            SetPropW(hdr->hwndFrom, L"tab_set_sel", (HANDLE)(MAKELONG(1211, 1)));
        pData->isReturn = true;
        return isReturn ? 1 : 0;
    }
    default:
        break;
    }
    return 0;
}


LRESULT CALLBACK EWindowProc_AllDestroy(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    _EWINDOW_NAMESPACE EWINDOW_SIZE_MOVEINFO& move = _EWINDOW_NAMESPACE g_moveInfo;
    if (hWnd == s_info->hEWnd)
    {
        bool isSave = false;
        // 判断是否需要保存xml, 如果位置没有修改过, 那就不需要保存
        {
#define _RCCMP_(_rc1, _rc2) (memcmp(&_rc1, &_rc2, sizeof(RECT)) != 0)
            RECT rcWork         = s_info->config->rcWork();
            RECT rcState        = s_info->config->rcState();
            RECT rcControl      = s_info->config->rcControl();
            RECT rcMenu         = s_info->config->rcMenu();
            RECT rcMDIClient    = s_info->config->rcMDIClient();

            RECT rcWork1        = s_info->config->rcWork1();
            RECT rcState1       = s_info->config->rcState1();
            RECT rcControl1     = s_info->config->rcControl1();

            // 工作夹/状态夹/菜单/mdi/组件箱 只要有一个不一样就设置isSave为真
            if (_RCCMP_(rcWork, s_info->rcWork) || _RCCMP_(rcState, s_info->rcState)
                || _RCCMP_(rcMenu, s_info->rcMenu) || _RCCMP_(rcMDIClient, s_info->rcMDIClient) || _RCCMP_(rcControl, s_info->rcControl)
                || _RCCMP_(rcWork1, s_info->rcWork1) || _RCCMP_(rcState1, s_info->rcState1) || _RCCMP_(rcControl1, s_info->rcControl1))
                isSave = true;
        }

        // isSave值为真的话才进行保存操作
        if (isSave)
        {

            // 还原到100% 缩放, 保存100% 缩放的尺寸
            auto pfnRerect = [](RECT& rc) -> RECT*
            {
                const float scale = (float)s_info->dpiX / 96.f;
                rc.left     = (int)((float)(rc.left  ) / scale);
                rc.top      = (int)((float)(rc.top   ) / scale);
                rc.right    = (int)((float)(rc.right ) / scale);
                rc.bottom   = (int)((float)(rc.bottom) / scale);
                return &rc;
            };

            s_info->config->rcWork(pfnRerect(s_info->rcWork));
            s_info->config->rcState(pfnRerect(s_info->rcState));

            // 当前组件箱没有显示, 那保存的时候就不能使用组件箱的位置, 而应该使用原来的组件箱位置
            if (s_info->rcControl.right - s_info->rcControl.left < s_info->scale(100))
                s_info->config->rcControl(pfnRerect(s_info->rcControl));
            else
                s_info->config->rcControl(pfnRerect(s_info->rcControl1));

            s_info->config->rcMenu(pfnRerect(s_info->rcMenu));
            s_info->config->rcMDIClient(pfnRerect(s_info->rcMDIClient));

            s_info->config->rcWork1(pfnRerect(s_info->rcWork1));
            s_info->config->rcState1(pfnRerect(s_info->rcState1));
            s_info->config->rcControl1(pfnRerect(s_info->rcControl1));

        }
    }
    return 0;
}
typedef void(__cdecl* pfn_call)(LPTOPSTATE_INFO, int);
inline pfn_call _get_call(LPTOPSTATE_INFO info)
{
    static HWND hControl = s_info->eWnd->GetControl();
    static HWND hWork = s_info->eWnd->GetWork();
    static HWND hState = s_info->eWnd->GetState();
    pfn_call pfn = 0;
    HWND hWnd = info->hWnd;
    if (hWnd == hWork)          pfn = _EWINDOW_NAMESPACE EWindow_StateClick_Work;
    else if (hWnd == hState)    pfn = _EWINDOW_NAMESPACE EWindow_StateClick_State;
    else if (hWnd == hControl)  pfn = _EWINDOW_NAMESPACE EWindow_StateClick_Control;
    return pfn;
}

// 图钉按钮被点击, 菜单的自动隐藏也会走这里
inline void _pushpin_click(LPTOPSTATE_INFO info)
{
    // 把当前激活的窗口改成其他, 在移动尺寸的时候会判断当前激活窗口, 如果有激活就按尺寸移动, 没激活就固定位置
    if (info->isSelect)
        s_info->iDraw->TopState_UpdateStateWindow(s_info->hEWnd);
    ShowWindowEx(info->hWnd, info->isSelect ? SW_HIDE : SW_SHOW);
    LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
    WPARAM wParam = s_info->isZoomed ? SIZE_MAXIMIZED : SIZE_RESTORED;
    LPARAM lParam = MAKELONG(pData->pos.width, pData->pos.height);
    _EWINDOW_NAMESPACE EWindow_Size_OnSize(s_info->hEWnd, WM_SIZE, wParam, lParam, pData);
    InvalidateRect(s_info->hEWnd, 0, 0);
}

inline LRESULT CALLBACK EWindowProc_OnCommand(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    if (!(HIWORD(wParam) == 0 && lParam == 0))
        return 0;

    LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pData->param;
    pfn_call pfn = _get_call(info);
    if (!pfn) return 0;

    //wstr::dbg(L"状态条菜单被选中, hWnd = %d, info->hWnd = %d, info = %d\n", hWnd, (info ? info->hWnd : 0), info);
    const int id = LOWORD(wParam);
    switch (id)
    {
    case IDMENU_FLOAT:      // 浮动(&F)
    {
        // 需要把当前选择夹里的窗口弄出来成一个单独的窗口
        break;
    }
    case IDMENU_STOP:       // 停靠(&K)
    {
        // 需要把浮动的窗口加入到选择夹里
        break;
    }
    case IDMENU_AUTOHIDE:   // 自动隐藏(&T)
    {
        _pushpin_click(info);
        break;
    }
    case IDMENU_CLOSE:      // 关闭(&C)
    {
        pfn(info, TOP_STATE_BUTTON_CLOSE);
        break;
    }
    default:
        return 0;
    }
    pData->isReturn = true;
    return 0;
}
// 所有组件的消息都会经过这个函数, 由这个函数处理几个需要处理的消息
LRESULT CALLBACK EWindowProc(HWND hWnd, UINT message, WPARAM* wParam, LPARAM* lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
        case WM_DESTROY:
            return EWindowProc_AllDestroy(hWnd, message, *wParam, *lParam, pData);
        case WM_SIZE:
            return EWindowProc_AllSize(hWnd, message, *wParam, *lParam, pData);
        case WM_WINDOWPOSCHANGING:
            return EWindowProc_AllWindowPosChanging(hWnd, message, *wParam, *lParam, pData);
        case WM_NOTIFY:
            return EWindowProc_AllNotify(hWnd, message, *wParam, *lParam, pData);
        case WM_COMMAND:
            return EWindowProc_OnCommand(hWnd, message, *wParam, *lParam, pData);
        default:
            return 0;
    }
    return 0;
}

// 所有组件的WM_SIZE消息都会经过这里, 这个函数主要分处理几个窗口
// 把需要处理的窗口消息转发到对应的处理函数, 不需要处理的直接返回0
// 函数返回值就是子类化的返回值, 如果不需要继续往下处理, pData->isReturn 请设置为true
LRESULT CALLBACK EWindowProc_AllSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    if (hWnd == s_info->hEWnd)              // 易语言主窗口
        return _EWINDOW_NAMESPACE EWindow_Size_OnSize(hWnd, message, wParam, lParam, pData);
    else if (hWnd == s_info->hWork)         // 工作夹
        return _EWINDOW_NAMESPACE EWindow_Size_OnWorkSize(hWnd, message, wParam, lParam, pData);
    else if (hWnd == s_info->hWndMenu)      // 菜单栏容器
        return _EWINDOW_NAMESPACE EWindow_Size_OnMenuSize(hWnd, message, wParam, lParam, pData);
    else if (hWnd == s_info->hMDIClient)    // 写代码的窗口
        return _EWINDOW_NAMESPACE EWindow_Size_OnMDIClientSize(hWnd, message, wParam, lParam, pData);
    else if (hWnd == s_info->hState)        // 底部状态夹的窗口
        return _EWINDOW_NAMESPACE EWindow_Size_OnStateSize(hWnd, message, wParam, lParam, pData);
    else if (hWnd == s_info->hTabStateLeft)// || hWnd == s_info->hTabStateRight)        // 底部状态夹的窗口
        return _EWINDOW_NAMESPACE EWindow_Size_OnStateSize_lr(hWnd, message, wParam, lParam, pData);
    else if (hWnd == s_info->hControl)        // 组件箱
        return _EWINDOW_NAMESPACE EWindow_Size_OnControlSize(hWnd, message, wParam, lParam, pData);
    else if (hWnd == s_info->hEWnd)
        return _EWINDOW_NAMESPACE EWindow_Size_OnCustomTab(hWnd, message, wParam, lParam, pData);
    return 0;
}


// 与支持库交互的回调函数
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case IDC_IMOVE_GETWINDOWTYPE:   // 获取当前MDIClient 窗口下是显示什么窗口, -1=没有窗口, 0=代码区, 1=设计器窗口, 2=起始页, 高位是窗口类型, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
        return MAKELONG(_EWINDOW_NAMESPACE g_moveInfo.windowType, _EWINDOW_NAMESPACE g_moveInfo.code_type);
    case IDC_IMOVE_GETCODETYPE:     // 获取指定MDIClient 窗口下是显示什么窗口类型, 第三个参数传递代码选择夹索引, 传递-1则返回当前索引, MDICLIENT_CODETYPE 枚举常量, 0=未知, 1=窗口设计器, 2=普通程序集, 3=窗口程序集, 4=类, 5=全局变量, 6=数据类型, 7=DLL目录定义表, 8=常量, 9=图片资源, 10=声音资源
    {
        if (wParam == lParam && HIWORD(wParam) == 1211)
            _EWINDOW_NAMESPACE g_moveInfo.code_type = LOWORD(wParam);
        if (wParam == -1)
        {
            return _EWINDOW_NAMESPACE g_moveInfo.code_type;
        }
        return _EWINDOW_NAMESPACE g_moveInfo.code_type;
    }
    case INE_STATE_CLICK:   // 顶部的状态条按钮被单击
    {
        LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)lParam;
        // 三角按钮, 需要弹出个菜单, 不把事件传递到下一层处理
        if (LOWORD(wParam) == TOP_STATE_BUTTON_TRIANGLE)
        {
            _EWINDOW_NAMESPACE EWindow_PopupMenu(info);
            break;
        }
        else if (LOWORD(wParam) == TOP_STATE_BUTTON_PUSHPIN)
        {
            _pushpin_click(info);
            break;
        }
        pfn_call pfn = _get_call(info);
        
        if (pfn)
            pfn(info, LOWORD(wParam));
        return 0;
    }
    case INE_STATE_UPDATEHWND:  // 顶部状态条当前激活的窗口被改变, wParam = 之前激活的窗口句柄, lParam = 新激活的窗口句柄
    {
        HWND hOld = (HWND)wParam;
        HWND hNew = (HWND)lParam;
        static HWND hControl = s_info->eWnd->GetControl();
        static HWND hWork = s_info->eWnd->GetWork();
        static HWND hState = s_info->eWnd->GetState();
        typedef void(__cdecl* pfn_MoveFun)(HDWP& hDwp);
        HWND hWnd = hOld;
        HWND hChild = 0;
        pfn_MoveFun pMove = 0;
        HDWP hDwp = 0;
        bool isOld = false;
        bool isAutoHide = false;
        if (hOld == hWork)
            pMove = _EWINDOW_NAMESPACE EWindow_MoveWork     , isOld = true;
        else if (hOld == hState)
            pMove = _EWINDOW_NAMESPACE EWindow_MoveState    , isOld = true;
        else if (hOld == hControl)
            pMove = _EWINDOW_NAMESPACE EWindow_MoveControl  , isOld = true;
        else if (hNew == hWork)
            pMove = _EWINDOW_NAMESPACE EWindow_MoveWork     , hWnd = hNew, hChild = s_info->eWnd->GetWork32770Parent();
        else if (hNew == hState)
            pMove = _EWINDOW_NAMESPACE EWindow_MoveState    , hWnd = hNew, hChild = s_info->eWnd->GetState32770Parent();
        else if (hNew == hControl)
            pMove = _EWINDOW_NAMESPACE EWindow_MoveControl  , hWnd = hNew, hChild = s_info->eWnd->GetControl32770Parent();

        if (!pMove) break;

        LPOBJSTRUCT pData = 0;
        isAutoHide = _EWINDOW_NAMESPACE IsAutoHide(hWnd, &pData);
        // 旧激活的窗口是这几个窗口中的一个, 那就是需要隐藏这个窗口
        if (isOld)
        {
            if (isAutoHide) // 如果是自动隐藏, 那就需要隐藏或者把这个窗口调整到最小
            {
                if (pData->draw.indexDown != -1)
                {
                    //TODO 这里需要计算重画区域
                    InvalidateRect(s_info->hEWnd, 0, 0);
                    pData->draw.indexDown = -1;
                }
                LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
                int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_DRAW_AUTOHIDE_HIDE, hOld, &isReturn, false);
                if (isReturn)
                    break;
                ShowWindowEx(hOld, SW_HIDE);
                pMove(hDwp);

                s_info->evt.pfn_Event_callback_after(arr, IRE_DRAW_AUTOHIDE_HIDE, hOld, evtCount, false);

            }
        }
        else
        {
            LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
            int evtCount = 0;
            if (isAutoHide)
                evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_DRAW_AUTOHIDE_SHOW, hNew, &isReturn, false);

            if (!IsWindowVisible(hNew))
                ShowWindowEx(hNew, SW_SHOW);
            if (hChild && !IsWindowVisible(hChild))
                ShowWindowEx(hChild, SW_SHOW);

            SetWindowPos(hNew, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);
            if (isAutoHide) // 如果是自动隐藏, 那就需要调整这个窗口的尺寸
            {
                pMove(hDwp);
                s_info->evt.pfn_Event_callback_after(arr, IRE_DRAW_AUTOHIDE_SHOW, hNew, evtCount, false);
            }
        }

        break;
    }
    default:
        break;
    }
    return 0;
}



// 支持库调用, 会把窗口信息传递过来
extern "C" __declspec(dllexport) void* CALLBACK IDraw_Interface(IDRAW_INFO * info, IGetEWindow* eWnd, int reserved1, int reserved2)
{
    s_info = info;              // 主要信息, 这个结构包含了易语言主要的一些窗口

    _EWINDOW_NAMESPACE EWindow_Size_InitHook();    // 有部分窗口不需要易语言移动, 这里需要hook几个API

    /*IniReader(L"rcWork");
    wstr::dbg(L"%s配置项输出：%s\n", ININAME_CONTROL, IniReader(ININAME_CONTROL).c_str());
    wstr::dbg(L"%s配置项输出：%s\n", ININAME_MDICLIENT, IniReader(ININAME_MDICLIENT).c_str());
    wstr::dbg(L"%s配置项输出：%s\n", ININAME_MENU, IniReader(ININAME_MENU).c_str());
    wstr::dbg(L"%s配置项输出：%s\n", ININAME_STATE, IniReader(ININAME_STATE).c_str());
    wstr::dbg(L"%s配置项输出：%s\n", ININAME_WORK, IniReader(ININAME_WORK).c_str());*/

    _EWINDOW_NAMESPACE EWINDOW_SIZE_MOVEINFO& move = _EWINDOW_NAMESPACE g_moveInfo;
    memset(&move, 0, sizeof(move));
    s_info->rcWork          = s_info->config->rcWork();
    s_info->rcState         = s_info->config->rcState();
    s_info->rcControl       = s_info->config->rcControl();
    s_info->rcMenu          = s_info->config->rcMenu();
    s_info->rcMDIClient     = s_info->config->rcMDIClient();

    s_info->rcWork1         = s_info->config->rcWork1();
    s_info->rcState1        = s_info->config->rcState1();
    s_info->rcControl1      = s_info->config->rcControl1();
    //move.rcControl          = s_info->rcControl;



    s_info->pfnAllWindowProc    = EWindowProc;

    pluginInfo.cbSize           = sizeof(pluginInfo);
    pluginInfo.name             = "核心移动插件";
    pluginInfo.remarks          = "易语言窗口布局都由这个插件完成";
    pluginInfo.version          = IDRAW_VERSIONA;
    pluginInfo.author           = "精易论坛";
    pluginInfo.Versionlimit     = 0;
    pluginInfo.Dependence       = 0;


    pluginInfo.guid                 = "{05A57CE4-A1A4-473D-0005-F90CAF05FE4A}";
    pluginInfo.pfnMenuExt_Command   = 0;
    pluginInfo.pfnCallback          = IDraw_NotifyCallback;

    return &pluginInfo;
    return 0;         // 返回的这个函数接收所有子类化窗口的消息, 需要拦截消息请设置 pData->isReturn 为 true
}

// 插件被卸载, 需要把所有注册的事件/消息给释放
extern "C" __declspec(dllexport) void CALLBACK IDraw_UnInterface()
{

}
