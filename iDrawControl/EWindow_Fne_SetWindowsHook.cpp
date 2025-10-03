#include "EWindow_Fne_Header.h"
#include "EWindow.h"
#include <iDraw_const.h>

LRESULT CALLBACK HookProc_Message(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HookProc_Cbt(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HookProc_CallWndProc(int code, WPARAM wParam, LPARAM lParam);


static HHOOK s_hhk_msg, s_hhk_cbt, s_hhk_msg_begin;
static HWND s_hWndActivate;
inline void IDraw_windowshook_Destroy()
{
    UnhookWindowsHookEx(s_hhk_msg);
    UnhookWindowsHookEx(s_hhk_cbt);
}

void EWindow_Hook_SetWindowsHook()
{
    if (!s_hhk_msg)
    {
        IDraw_PushPluginFreeCallback(IDraw_windowshook_Destroy);

        HINSTANCE hInst = CWndBase::GetInst();
        s_hhk_msg = SetWindowsHookExW(WH_GETMESSAGE, HookProc_Message, hInst, GetCurrentThreadId());
        //s_hhk_msg_begin = SetWindowsHookExW(WH_CALLWNDPROC, HookProc_CallWndProc, hInst, GetCurrentThreadId());
        s_hhk_cbt = ::SetWindowsHookExW(WH_CBT, HookProc_Cbt, hInst, ::GetCurrentThreadId());

        //assert(s_hhk_msg && s_hhk_cbt);
    }

}
LRESULT CALLBACK HookProc_CallWndProc(int code, WPARAM wParam, LPARAM lParam)
{
    LPCWPSTRUCT ptr = (LPCWPSTRUCT)lParam;
    switch (ptr->message)
    {
    case WM_KEYDOWN:
    {
        if (ptr->wParam == VK_TAB)
        {
            HWND hParent = GetParent(ptr->hwnd);
            if (!hParent)
                break;
            int idParent = GetDlgCtrlID(hParent);
            int idChild = GetDlgCtrlID(ptr->hwnd);
            if (idParent == 59648 && idChild == 1)  // 只处理代码窗口的
            {
                const short ctrl = GetAsyncKeyState(VK_CONTROL);
                const short shift = GetAsyncKeyState(VK_SHIFT);
                const bool isCtrl = ctrl & 0x8000;
                const bool isShift = shift & 0x8000;
                if (isCtrl)
                    return 0;
            }
        }
        break;
    }
    default:
        break;
    }
    return CallNextHookEx(s_hhk_msg_begin, code, wParam, lParam);
}

LRESULT CALLBACK HookProc_Message(int code, WPARAM wParam, LPARAM lParam)
{
    LPMSG ptr = (LPMSG)lParam;

    //LPCWPSTRUCT ptr = (LPCWPSTRUCT)lParam;
    //LPCWPRETSTRUCT ptr = (LPCWPRETSTRUCT)lParam;
    switch (ptr->message)
    {
    case WM_XBUTTONDOWN:
    case WM_NCXBUTTONDOWN:
    case WM_XBUTTONDBLCLK:
    case WM_NCXBUTTONDBLCLK:
    {
        const int fwKeys = GET_KEYSTATE_WPARAM(ptr->wParam);     // LOWORD, MK_XBUTTON1/MK_XBUTTON2
        const int fwButton = GET_XBUTTON_WPARAM(ptr->wParam);      // HIWORD, XBUTTON1/XBUTTON2
        const POINT pt = { GET_X_LPARAM(ptr->lParam), GET_Y_LPARAM(ptr->lParam) };
        LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetMenuStandard());  // 获取标准工具条的句柄, 触发前进后退按钮事件
        if (fwKeys && pData && pData->fnCommand)
        {
            HWND hParent = GetParent(pData->hWnd);
            // MK_XBUTTON1=后退, MK_XBUTTON2=前进
            // MK_XBUTTON1=侧边下面的键, MK_XBUTTON2=侧边上面的键
            const int id = (((fwKeys & MK_XBUTTON1) == MK_XBUTTON1) ? TOOL_STANDARD_BACK : TOOL_STANDARD_FORWARD);
            pData->fnCommand(hParent, WM_COMMAND, id, (LPARAM)pData->hWnd, s_info->pfnGetDataFromHwnd(hParent));
        }
        //wstr::dbg(L"侧边按钮被按下, lo=%d, hi=%d, x,y={%d,%d}\n", fwKeys, fwButton, pt);
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    case WM_NCRBUTTONDOWN:
    case WM_NCMBUTTONDOWN:


    //case WM_LBUTTONUP:
    //case WM_RBUTTONUP:
    //case WM_MBUTTONUP:
    //case WM_NCLBUTTONUP:
    //case WM_NCRBUTTONUP:
    //case WM_NCMBUTTONUP:

    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    //case WM_NCLBUTTONDBLCLK:
    //case WM_NCRBUTTONDBLCLK:
    //case WM_NCMBUTTONDBLCLK:
    {
        if (ptr->hwnd == s_info->hEWnd && (ptr->message == WM_NCLBUTTONDOWN || ptr->message == WM_NCRBUTTONDOWN || ptr->message == WM_NCMBUTTONDOWN))
            break;  // 易语言主窗口的非客户区消息不处理, 其他的都需要处理
        //wstr::dbg(L"鼠标按下 message = %d, 窗口 = %d\n", ptr->message, ptr->hwnd);
        static HWND hControl = s_info->eWnd->GetControl();
        static HWND hWork = s_info->eWnd->GetWork();
        static HWND hState = s_info->eWnd->GetState();
        static LPOBJSTRUCT pWork = s_info->pfnGetDataFromHwnd(s_info->hWork);
        static LPOBJSTRUCT pState = s_info->pfnGetDataFromHwnd(s_info->hState);
        static LPOBJSTRUCT pControl = s_info->pfnGetDataFromHwnd(s_info->hControl);
        static LPOBJSTRUCT s_pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
        if (!pWork || !pState || !pControl) break;
        static LPTOPSTATE_INFO work = (LPTOPSTATE_INFO)pWork->param;
        static LPTOPSTATE_INFO state = (LPTOPSTATE_INFO)pState->param;
        static LPTOPSTATE_INFO control = (LPTOPSTATE_INFO)pControl->param;

#define _act_cmp(_info, _calc, _if) \
        _if (__query_flags(_info, TOP_STATE_BUTTON_ISAOTUHIDE) && hAtiavete == _info->hWnd)\
        {\
            RECT rc; _calc(rc);\
            POINT pt, ptClient;\
            GetCursorPos(&pt);\
            ptClient = pt;\
            ScreenToClient(s_info->hEWnd, &ptClient);\
            if (PtInRect(&rc, ptClient))\
                return CallNextHookEx(s_hhk_msg, code, wParam, lParam);\
            if ((ptr->message == WM_LBUTTONDOWN || ptr->message == WM_RBUTTONDOWN) && IsWindowVisible(_info->hWnd)) {\
                GetWindowRect(_info->hWnd, &rc);\
                if (PtInRect(&rc, pt))\
                    return CallNextHookEx(s_hhk_msg, code, wParam, lParam);\
            }\
        }

        HWND hAtiavete = TopState_GetActivateWindow();
        //if (__query_flags(state, TOP_STATE_BUTTON_ISAOTUHIDE) && hAtiavete == state->hWnd)
        //{
        //    RECT rc; _draw_calc_state(rc);
        //    POINT pt, ptClient;
        //    GetCursorPos(&pt);
        //    ptClient = pt;
        //    ScreenToClient(s_info->hEWnd, &ptClient);
        //    if (PtInRect(&rc, ptClient))
        //        return CallNextHookEx(s_hhk_msg, code, wParam, lParam);
        //    if ((ptr->message == WM_LBUTTONDOWN || ptr->message == WM_RBUTTONDOWN) && IsWindowVisible(state->hWnd)) {

        //        GetWindowRect(state->hWnd, &rc);
        //        if (PtInRect(&rc, pt))
        //            return CallNextHookEx(s_hhk_msg, code, wParam, lParam);
        //    }
        //}

        _act_cmp(state  , _draw_calc_state  , if)
        _act_cmp(work   , _draw_calc_work   , else if)
        _act_cmp(control, _draw_calc_control, else if)



        TopState_UpdateStateWindow(ptr->hwnd);
        break;
    }
    case WM_MOUSEWHEEL:
    {
        //TODO 需要做一个在代码框里鼠标左键按住的情况下能滚动, 并且选中到滚动的那一行
        break;
        const short fwKeys  = GET_KEYSTATE_WPARAM(ptr->wParam);     // LOWORD, MK_LBUTTON
        const short zDelta  = GET_WHEEL_DELTA_WPARAM(ptr->wParam);  // HIWORD, 滚动距离, 正数是向上滚动, 负数是向下
        if (!((fwKeys & MK_LBUTTON) == MK_LBUTTON)) break;  // 左键没有按下的不处理

        HWND hParent = GetParent(ptr->hwnd);
        const int idChild = GetDlgCtrlID(ptr->hwnd);
        const int idParent = GetDlgCtrlID(hParent);
        // 得判断是不是代码窗口, 不是代码窗口不处理
        // 代码窗口的ID是1, 父窗口是59648
        if (!(idChild == 1 && idParent == 59648))
            break;
        LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(ptr->hwnd);
        if (!pData) break;
        IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
        if (!pObj) break;

        //wstr::dbg(L"hWnd = %d, message=%d, wParam=%d, lParam=%d\n", ptr->hwnd, ptr->message, ptr->wParam, ptr->lParam);
        LRESULT ret = CallNextHookEx(s_hhk_msg, code, wParam, lParam);

        const bool isUp = zDelta > 0;
        int pos = s_info->config->wheelPos();
        if (pos <= 0 || pos > 30) pos = 3;

        const int newpos = pObj->m_vScroll.nPos + (isUp ? -pos + 1 : pos - 1);
        IEIDE_CodeManage* ide = GetIEIDE_CodeManage(ptr->hwnd);
        if (!ide)break;
        POINT ptSel = { 0 };
        POINT ptPos = ide->GetCursorPos();
        int start = 0, end = 0, line = 0;
        ide->GetSelInfo(&start, &end, &line, &ptSel);
        ptSel.y = (ptSel.y + (isUp ? -pos : pos));
        if (ptSel.x > ptSel.y)
        {
            const int x = ptSel.x;
            ptSel.x = ptSel.y;
            ptSel.y = x;
        }
        ptPos.y = ptSel.y + 1;

        for (int i = 0; i < pos; i++)
            pfn_NES_RUN_FUNC(FN_MOVE_BLK_SEL_DOWN);
        for (int i = 0; i < pos; i++)
            pfn_NES_RUN_FUNC(FN_SCROLL_DOWN, 1, 0);
        //pfn_NES_RUN_FUNC(FN_BLK_ADD_DEF, ptSel.x, ptSel.y);
        pfn_NES_RUN_FUNC(FN_MOVE_CARET, ptPos.y, 0);

        //WndProc_CallWindowProc(ptr->hwnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, newpos), 0, pData);
        //ide->CursorLocation(ptSel.y);
        return ret;
    }
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        //if (ptr->wParam != VK_SHIFT && ptr->wParam != VK_CONTROL)
        //    wstr::dbg(L"按键按下, 当前键 = %d\n", ptr->wParam);
        // VK_BACK VK_MENU
        
        //for (int i = 0; i < 1000000; i++)
        //    GetAsyncKeyState('F');
        if (ptr->wParam == VK_TAB)
        {
            int size = s_info->hWnds->size();
            if (size == 0) break;

            HWND hParent = GetParent(ptr->hwnd);
            if (!hParent)
                break;
            int idParent = GetDlgCtrlID(hParent);
            int idChild = GetDlgCtrlID(ptr->hwnd);
            if (idParent != 59648 || idChild != 1)  // 只处理代码窗口的
                break;

            const short ctrl = GetAsyncKeyState(VK_CONTROL);
            const short shift = GetAsyncKeyState(VK_SHIFT);
            const bool isCtrl = ctrl & 0x8000;
            const bool isShift = shift & 0x8000;
            int index = s_info->tab.indexSel;
            // Ctrl + tab   和   Ctrl + Shift + tab 易语言自己处理
            if (isShift && !isCtrl) // 往回, 只要按下了shift键就往回
            {
                if (--index < 0)
                    index = size - 1;
                MDIClient_CustomTabCtrl_ClickItem(index, false, s_info->pWnd);
                //wstr::dbg(L"hWnd = %d, 设置新的选中项 = %d, 记录的选中项 = %d\n", ptr->hwnd, index, s_info->tab.indexSel);
            }

            //HWND hWndActivate = s_info->hWnds->at(index).hWnd;
            //SendMessageW(s_info->hMDIClient, WM_MDIACTIVATE, (WPARAM)hWndActivate, (LPARAM)hWndActivate);
            //MDIClient_CustomTabCtrl_ClickItem(index, false, s_info->pWnd, TAB_SEL_MODE_CODE);
            //s_hWndActivate = 0;
            break;
        }
        else if (ptr->wParam == VK_F10)
        {
            if (!s_info->isRun || !s_info->isStop) break;   // 运行和停止按钮都是启用的情况下才往下执行, 只要有一个不启用就不是在调试状态

            const short ctrl = GetAsyncKeyState(VK_CONTROL);
            const short shift = GetAsyncKeyState(VK_SHIFT);
            const bool isCtrl = ctrl & 0x8000;
            const bool isShift = shift & 0x8000;
            if (!isCtrl && !isShift)
                s_info->pfnCallMenu(TOOL_STANDARD_STEPOVER);    // 备注工具条 -> 单步
        }
        else if (ptr->wParam == VK_F11)
        {
            if (!s_info->isRun || !s_info->isStop) break;   // 运行和停止按钮都是启用的情况下才往下执行, 只要有一个不启用就不是在调试状态

            const short ctrl   = GetAsyncKeyState(VK_CONTROL);
            const short shift  = GetAsyncKeyState(VK_SHIFT);
            const bool isCtrl  = ctrl  & 0x8000;
            const bool isShift = shift & 0x8000;
            if (!isCtrl && !isShift)
                s_info->pfnCallMenu(TOOL_STANDARD_STEPINTO);    // 备注工具条 -> 单步步入
            else if (!isCtrl && isShift)
                s_info->pfnCallMenu(TOOL_STANDARD_STEPOUT);     // 备注工具条 -> 跟踪返回
        }
        else if (ptr->wParam == VK_BACK)    // 退格键被按下, Ctrl+Shift+Alt+Backspace=前进  Ctrl+Shift+Backspace=后退
        {
            const short ctrl   = GetAsyncKeyState(VK_CONTROL);
            const short shift  = GetAsyncKeyState(VK_SHIFT);
            const bool isCtrl  = ctrl & 0x8000;
            const bool isShift = shift & 0x8000;
            if (isCtrl && isShift)
            {
                const short alt = GetAsyncKeyState(VK_MENU);
                const bool isAlt = alt & 0x8000;
                const int id = isAlt ? 32885 : 33003;   // 32885 = 前进, 33003 = 后退
                s_info->pfnCallMenu(id);
            }
        }
        else if (ptr->wParam == VK_INSERT)  // 新建源码
        {
            const short ctrl = GetAsyncKeyState(VK_CONTROL);
            const short shift = GetAsyncKeyState(VK_SHIFT);
            const bool isCtrl = ctrl & 0x8000;
            const bool isShift = shift & 0x8000;
            if (!isCtrl && isShift)
                s_info->pfnCallMenu(57600); // 57600 = 新建源码
        }
        else if (ptr->wParam == VK_F5)      // 重新启动
        {
            const short ctrl = GetAsyncKeyState(VK_CONTROL);
            const short shift = GetAsyncKeyState(VK_SHIFT);
            const bool isCtrl = ctrl & 0x8000;
            const bool isShift = shift & 0x8000;
            if (isCtrl && isShift)
            {
                EWindow_ReStart();
            }
            else if (!isCtrl && isShift)
            {
                s_info->pfnCallMenu(TOOL_STANDARD_STOP);
            }
        }
        else if (GetAsyncKeyState('F') & 0x8000)
        {
            const short ctrl = GetAsyncKeyState(VK_CONTROL);
            const short shift = GetAsyncKeyState(VK_SHIFT);
            const bool isCtrl = ctrl & 0x8000;
            const bool isShift = shift & 0x8000;
            //GetKeyState();
            if (isShift && isCtrl)
            {
                static bool isCall = true;
                if (isCall)
                {
                    isCall = false;
                    //s_info->pfnCallMenu(32934); // 32934 = 整体搜寻
                    isCall = true;
                }
            }
        }


        break;
    }
    default:
        break;
    }
    return CallNextHookEx(s_hhk_msg, code, wParam, lParam);
    //IRE_STRUCT_HOOKARG evt;
    //evt.code = code;
    //evt.lParam = lParam;
    //evt.wParam = wParam;
    //evt.ret = 0;
    //LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
    //int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_HOOKPROC_GETMESSAGE, &evt, &isReturn, false);
    //if (isReturn)
    //    return (LRESULT)evt.ret;     // 插件处理了位置信息, 不往后处理

    //evt.ret = CallNextHookEx(s_hhk_msg, code, wParam, lParam);

    //s_info->evt.pfn_Event_callback_after(arr, IRE_HOOKPROC_GETMESSAGE, &evt, evtCount);
    //return (LRESULT)evt.ret;
}

HWND GetTabActiveWindow()
{
    return s_hWndActivate;
}

LRESULT CALLBACK HookProc_Cbt(int code, WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = 0;
    if (_menu_cbthook_proc(code, wParam, lParam, ret))
        return ret;

    //if (HCBT_CREATEWND == code)
    //{
    //    LPCBT_CREATEWND cc = (LPCBT_CREATEWND)lParam;
    //    wstr::dbg(L"hook创建被创建, 窗口标题 = %s, 窗口类名 = %d, 窗口句柄 = %d, 父窗口句柄 = %d, 易语言窗口句柄 = %d\n",
    //        cc->lpcs->lpszName, cc->lpcs->lpszClass, wParam, cc->lpcs->hwndParent, s_info->hEWnd);
    //}

    IRE_STRUCT_HOOKARG evt;
    evt.code = code;
    evt.lParam = lParam;
    evt.wParam = wParam;
    evt.ret = 0;
    evt.hhk = s_hhk_cbt;
    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
    int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_HOOKPROC_CBT, &evt, &isReturn, false);
    if (isReturn)
        return (LRESULT)evt.ret;     // 插件处理了位置信息, 不往后处理

    switch(code)
    {
    case HCBT_ACTIVATE:
    {
        // 激活窗口
        LPCBTACTIVATESTRUCT cs = (LPCBTACTIVATESTRUCT)lParam;
        HWND hWnd = (HWND)wParam;   // 激活的窗口句柄
        //DWORD dwStyle = GetWindowLongW(hWnd, GWL_STYLE);
        //if (__query(dwStyle, WS_POPUP) || __query(dwStyle, WS_CHILD) == false)
        //{
        //    EnumChildWindows(hWnd, [](HWND h, LPARAM l)->BOOL
        //    {
        //        _hook_deal_wnd(h);
        //        //RedrawWindow(h, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
        //        //SendMessageW(h, WM_THEMECHANGED, 0, 0);
        //        return TRUE;
        //    }, 0);
        //}
        break;
    }
    case HCBT_CREATEWND:
    {
        // 激活窗口
        LPCBT_CREATEWND cs = (LPCBT_CREATEWND)lParam;
        HWND hWnd = (HWND)wParam;   // 创建的窗口句柄
        break;
    }
    case HCBT_DESTROYWND:
    {
        // 窗口被销毁, lParam忽略
        HWND hWnd = (HWND)wParam;   // 被销毁的窗口句柄
        break;
    }
    default:
        break;
    }
    evt.ret = CallNextHookEx(s_hhk_cbt, code, wParam, lParam);

    
    s_info->evt.pfn_Event_callback_after(arr, IRE_HOOKPROC_CBT, &evt, evtCount, false);
    return (LRESULT)evt.ret;
}