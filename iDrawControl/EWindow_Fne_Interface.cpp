#include "EWindow_Fne_Header.h"
#include <windowsx.h>
#include <assist/CFileRW.h>
#include <control/CImgList.h>
#include <queue>
#include <unordered_map>
#include <hook_detours/apiHook.h>


IDRAW_INFO* s_info;
static apiHook hook_GetMessageA;
static apiHook hook_GetMessageW;
static apiHook hook_PeekMessageA;
static apiHook hook_PeekMessageW;
static BOOL(WINAPI* GetMessageW_hook)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax) = GetMessageW;
static BOOL(WINAPI* GetMessageA_hook)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax) = GetMessageA;
static BOOL(WINAPI* PeekMessageA_hook)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) = PeekMessageA;
static BOOL(WINAPI* PeekMessageW_hook)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) = PeekMessageW;
static UINT m_message_invalid;  // 无效的消息, 把alt + 1, 2, 3 转换成这个消息
// 子类化接口, 内部所有窗口的子类化都走这个函数, 由这个函数把数据分发到各个处理的函数上
LRESULT CALLBACK WndProc_Default_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

static std::map<HWND, OBJSTRUCT> m_map_AllWindow_Subclass;  // 存放所有子类化窗口的各种信息

std::map<HWND, OBJSTRUCT>& _get_subclass_allWindow()
{
    return m_map_AllWindow_Subclass;
}

__declspec(naked) int __stdcall  call_5(LPVOID pfn, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    __asm
    {
        push ebp
        mov ebp, esp
        mov eax, dword ptr[ebp + 0x08]
        mov ebx, dword ptr[ebp + 0x04]
        mov dword ptr[ebp + 0x08], ebx
        pop ebp
        pop ebx
        jmp eax
    }
}

LRESULT CALLBACK SubClassWindow_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#if 0
    struct _calc_timer 
    {
        ULONGLONG t;
        HWND hWnd;
        UINT message;
        WPARAM wParam;
        LPARAM lParam;
        _calc_timer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
            :t(0), hWnd(hWnd), message(message), wParam(wParam), lParam(lParam) { t = GetTickCount64(); }
        ~_calc_timer()
        {
            ULONGLONG t2 = GetTickCount64() - t;
            if (t2> 40)
                wstr::dbg(L"message 处理时长 = %lld, hWnd = 0x%08X(%d), message = %d, wParam = %d, lParam = %d\n",
                t2, hWnd, hWnd, message, wParam, lParam);
        }
    };
    _calc_timer _cl_t(hWnd, message, wParam, lParam);
#endif

    //if (!(message == 553 || message == 135))
    //    wstr::dbg(L"窗口消息, hWnd = %d, msg = %d, wParam = %d, H=%d,L=%d, lParam = %d, H=%d,L=%d\n",
    //        hWnd, message, wParam, HIWORD(wParam), LOWORD(wParam), lParam, HIWORD(lParam), LOWORD(lParam));

    if (!s_info)
        return DefWindowProcA(hWnd, message, wParam, lParam);

    auto it = m_map_AllWindow_Subclass.find(hWnd);
    if (it == m_map_AllWindow_Subclass.end())
        return DefWindowProcA(hWnd, message, wParam, lParam);
    OBJSTRUCT& data = it->second;
    data.isReturn = false;
    LPOBJSTRUCT pData = &data;

    if (!data.oldProc)
        return DefWindowProcA(hWnd, message, wParam, lParam);

    //if (message == WM_WINDOWPOSCHANGING)
    //{
    //    LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
    //    if ((pos->flags & SWP_NOSIZE) == 0) // 尺寸被改变了
    //    {
    //        pos->cx = s_info->scale(pos->cx);
    //        pos->cy = s_info->scale(pos->cy);
    //    }
    //    if ((pos->flags & SWP_NOMOVE) == 0) // 位置被移动了
    //    {
    //        //pos->x = s_info->scale(pos->x);
    //        //pos->y = s_info->scale(pos->y);
    //        
    //    }
    //}

    //////////////////////////////////////////////////////////////////////////
    if (s_info->pfnAllWindowProc)
    {
        switch (message)
        {
        case WM_DESTROY:
        case WM_SIZE:
        case WM_WINDOWPOSCHANGING:
        case WM_NOTIFY:
        {
            // 这些消息需要传递到移动插件里处理, 这个优先级最高
            LRESULT ret = s_info->pfnAllWindowProc(hWnd, message, &wParam, &lParam, pData);
            if (data.isReturn)
                return ret;
            break;
        }
        default:
            break;
        }

    }
    //return data.fnWndproc(hWnd, message, wParam, lParam, &data);

    LPREGISTER_MESSAGE_INFO arr[EVENT_MAX_ITEM] = { 0 };


    static auto pfn = s_info->evt.pfnGetMessageInfo;
    int count = pfn(hWnd, message, arr, EVENT_MAX_ITEM);
    for (int i = 0; i < count; i++)
    {
        if (i < EVENT_MAX_ITEM && arr[i] && arr[i]->pfnCallbackBefore)
        {
            //MEMORY_BASIC_INFORMATION mem;
            //VirtualQuery(arr[i]->pfnCallbackBefore, &mem, sizeof(mem));

            //TODO 有优先级待处理
            data.isReturn = false;
            try
            {
                //data.lResult = call_5(arr[i]->pfnCallbackBefore, hWnd, message, wParam, lParam, pData);
                data.lResult = arr[i]->pfnCallbackBefore(hWnd, message, wParam, lParam, pData);
            }
            catch (...)
            {
                data.isReturn = false;
                data.isReturn = false;
            }
            if (data.isReturn)
            {
                if (s_info->logMsgEvt)
                {
                    int _count = 0;
                    _str _dll(260);
                    LPCSTR dllName = arr[i]->dllName;
                    if (!dllName || !dllName[0])
                        dllName = s_info->pfn_GetDllNameFromAddress(arr[i]->pfnCallbackBefore);
                    LPCSTR guid = arr[i]->guid;
                    for (i++; i < count; i++)
                    {
                        if (arr[i]->pfnCallbackBefore)
                        {
                            _count++;
                            _dll.append("        ").append(s_info->pfn_GetDllNameFromAddress(arr[i]->pfnCallbackBefore)).append("\r\n");
                        }
                    }

                    s_info->logMsgEvt("插件 %s [GUID = %s]拦截了 %d 消息(处理前), 返回值 = %d, 后面剩余 %d 个插件没有处理该事件\r\n%s\r\n",
                        dllName, guid, message, data.lResult, _count, _dll.c_str());
                }

                if (message == WM_NCDESTROY)
                    s_info->evt.pfnDetachRegister_Message(hWnd);
                return data.lResult;
            }
        }
    }

    static OBJSTRUCT ncDestroy;
    if (message == WM_NCDESTROY)    // 非客户区销毁后data就失效了, 需要特别处理
    {
        ncDestroy = data;
        pData = &ncDestroy;
    }

    LRESULT ret = WndProc_Default_Proc(hWnd, message, wParam, lParam, pData);
    if (pData != &ncDestroy && m_map_AllWindow_Subclass.find(hWnd) == m_map_AllWindow_Subclass.end())
        return ret;
    //! 调用完之后不能在使用 data 这个变量
    pData->lResult = ret;

    for (int i = 0; i < count; i++)
    {
        if (i < EVENT_MAX_ITEM && arr[i] && arr[i]->pfnCallbackAfter)
        {
            //TODO 有优先级待处理
            pData->isReturn = false;
            //LRESULT n = call_5(arr[i]->pfnCallbackAfter, hWnd, message, wParam, lParam, pData);
            LRESULT n = arr[i]->pfnCallbackAfter(hWnd, message, wParam, lParam, pData);
            if (pData->isReturn)
            {
                ret = n;
                if (s_info->logMsgEvt)
                {
                    int _count = 0;
                    _str _dll(260);
                    LPCSTR dllName = arr[i]->dllName;
                    if (!dllName || !dllName[0])
                        dllName = s_info->pfn_GetDllNameFromAddress(arr[i]->pfnCallbackAfter);
                    LPCSTR guid = arr[i]->guid;
                    for (i++; i < count; i++)
                    {
                        if (arr[i]->pfnCallbackAfter)
                        {
                            _count++;
                            _dll.append("        ").append(s_info->pfn_GetDllNameFromAddress(arr[i]->pfnCallbackAfter)).append("\r\n");
                        }
                    }

                    s_info->logMsgEvt("插件 %s [GUID = %s]拦截了 %d 消息(处理后), 返回值 = %d, 后面剩余 %d 个插件没有处理该事件\r\n%s\r\n",
                        dllName, guid, message, ret, _count, _dll.c_str());
                }
                break;
            }
        }
    }
    if (message == WM_NCDESTROY)
        s_info->evt.pfnDetachRegister_Message(hWnd);
    return ret;
}

//! 这里处理一些固定配置的消息
LRESULT CALLBACK WndProc_Default_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
        //case WM_INITMENU:
    case WM_INITMENUPOPUP:
        return EMenu_OnInitMenuPopup(hWnd, message, wParam, lParam, pData);
    case WM_UNINITMENUPOPUP:
        return EMenu_OnUnInitMenuPopup(hWnd, message, wParam, lParam, pData);
    case WM_NCPAINT:
    {
        if (pData->draw.isNoNcPaint && pData->drawScroll == 0)
            return 0;    // 有屏蔽非客户区重画消息而且没有滚动条, 返回, 否则丢回去处理
        break;
    }
    case WM_ERASEBKGND:
    {
        if (!pData->draw.isErasebkg)return 1;
        break;
    }
    case WM_NCHITTEST:
    {
        if (pData->draw.isClient) return HTCLIENT;
        break;
    }
    case WM_NCCALCSIZE:
    {
        if (pData->draw.isNcCalcSize && pData->drawScroll == 0) // 有重新调整尺寸 并且 没有滚动条才执行内部的默认处理, 否则丢回去处理
        {
            if (wParam)
            {
                LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
                memcpy(&cs->rgrc[1], &cs->rgrc[0], sizeof(cs->rgrc[0]));
                memcpy(&cs->rgrc[2], &cs->rgrc[0], sizeof(cs->rgrc[0]));
                return WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;

                return WVR_REDRAW;
            }
            LPRECT prc = (LPRECT)lParam;
            return 0;
        }
        break;
    }
    case WM_CTLCOLOR:
    case WM_CTLCOLORMSGBOX:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    {
        HWND hControl = (HWND)lParam;
        LPOBJSTRUCT pCtlData = GetDataFromHwnd(hControl);

        if (pCtlData)
        {
            if (pCtlData->fnCtlcolor)
                return pCtlData->fnCtlcolor(hWnd, message, wParam, lParam, pData);
            return (LRESULT)pCtlData->draw.color.hbrBackGround;
        }

        LRESULT ctlRet = (pData->fnWndproc)
            ? pData->fnWndproc(hWnd, message, wParam, lParam, pData)
            : WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        return ctlRet;
    }
    case WM_NOTIFY:
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        LPOBJSTRUCT pCtlData = GetDataFromHwnd(hdr->hwndFrom);
        if (pCtlData && pCtlData->fnNotify)
            return pCtlData->fnNotify(hWnd, message, wParam, lParam, pData);
        break;
    }
    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT dw = (LPDRAWITEMSTRUCT)lParam;
        if (dw->CtlType == ODT_MENU)
            return EMenu_OnDrawItem(hWnd, message, wParam, lParam, pData);
        if (dw->CtlType == ODT_LISTBOX)
            return List_OnDrawItem(hWnd, message, wParam, lParam, pData);
        LPOBJSTRUCT pCtlData = GetDataFromHwnd(GetDlgItem(hWnd, dw->CtlID));
        if (pCtlData && pCtlData->fnDrawItem)
            return pCtlData->fnDrawItem(hWnd, message, wParam, lParam, pData);
        break;
    }
    case WM_MEASUREITEM:
    {
        LPMEASUREITEMSTRUCT item = (LPMEASUREITEMSTRUCT)lParam;
        if (item->CtlType == ODT_MENU)
            return EMenu_OnMeasureItem(hWnd, message, wParam, lParam, pData);
        if (item->CtlType == ODT_LISTVIEW)
            return ListView_OnMeasureItem(hWnd, message, wParam, lParam, pData);
        LPOBJSTRUCT pCtlData = GetDataFromHwnd(GetDlgItem(hWnd, item->CtlID));
        if (pCtlData && pCtlData->fnMeasureItem)
            return pCtlData->fnMeasureItem(hWnd, message, wParam, lParam, pData);
        break;
    }
    case WM_DESTROY:
    {
        if (!pData->fnWndproc)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        LRESULT ret = pData->fnWndproc(hWnd, message, wParam, lParam, pData);
        return ret;
    }
    case WM_NCDESTROY:
    {
        LRESULT ret = 0;
        if (!pData->fnWndproc)
            ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        else
            ret = pData->fnWndproc(hWnd, message, wParam, lParam, pData);

        pData->SetWndLong(hWnd, GWLP_WNDPROC, (LONG_PTR)pData->oldProc);
        m_map_AllWindow_Subclass.erase(hWnd);
        if (hWnd == s_info->hEWnd)
        {
            for (auto it = m_map_AllWindow_Subclass.begin(); it != m_map_AllWindow_Subclass.end(); ++it)
            {
                OBJSTRUCT& data = it->second;
                pfn_SetLong SetLong = data.SetWndLong;
                HWND hWnd1 = data.hWnd;
                WNDPROC oldProc = data.oldProc;
                SetLong(hWnd1, GWLP_WNDPROC, (LONG_PTR)oldProc);
            }
        }
        return ret;
    }
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == 1182)
        {
            wstr::dbg(L"接收事件的窗口 = %d, 列表事件, 列表句柄 = %d, 列表id = %d, 列表事件 = %d\n", hWnd, lParam, (LOWORD(wParam)), HIWORD(wParam));
        }
        LPOBJSTRUCT pCtlData = GetDataFromHwnd((HWND)lParam);
        if (pCtlData && pCtlData->fnCommand)
            return pCtlData->fnCommand(hWnd, message, wParam, lParam, pData);
        break;
    }
    case WM_WINDOWPOSCHANGED:
    {
        LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
        if ((pos->flags & SWP_NOSIZE) == 0 || (pos->flags & SWP_NOMOVE) == 0)    // 改变了尺寸或者被移动了
        {
            pData->pos.rc.left = pos->x;
            pData->pos.rc.top = pos->y;

            //TODO 这里应该改, 好多地方都用到这个结构, 牵一发而动全身
            pData->pos.rcClient.left = pos->x;
            pData->pos.rcClient.top = pos->y;

            //pData->pos.rcClient.left = 0;
            //pData->pos.rcClient.top = 0;
            //pData->pos.rcClient.right = pos->cx;
            //pData->pos.rcClient.bottom = pos->cy;
            pData->pos.x = pos->x;
            pData->pos.y = pos->y;

            if ((pos->flags & SWP_NOSIZE) == SWP_NOSIZE) // 有不调整大小的标识, 宽度高度得设置一下
            {
                pos->cx = pData->pos.width;
                pos->cy = pData->pos.height;
            }


            pData->pos.width = pos->cx;
            pData->pos.height = pos->cy;

            pData->pos.rcClient.right = pos->x + pos->cx;
            pData->pos.rcClient.bottom = pos->y + pos->cy;

            pData->pos.rc.right = pos->x + pos->cx;
            pData->pos.rc.bottom = pos->y + pos->cy;

        }
        if (!pData->fnWndproc)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        return pData->fnWndproc(hWnd, message, wParam, lParam, pData);
    }
    case WM_STYLECHANGED:
    {
        LPSTYLESTRUCT ss = (LPSTYLESTRUCT)lParam;
        bool oldIsBorder, newIsBorder;
        if (wParam == GWL_STYLE)
        {
            pData->style = ss->styleNew;
            oldIsBorder = (ss->styleOld & WS_BORDER) != 0;
            newIsBorder = (ss->styleNew & WS_BORDER) != 0;

        }
        else
        {
            pData->styleEx = ss->styleNew;
            oldIsBorder = ((ss->styleOld & WS_EX_CLIENTEDGE)
                || (ss->styleOld & WS_EX_DLGMODALFRAME)
                || (ss->styleOld & WS_EX_WINDOWEDGE)) != 0;
            newIsBorder = ((ss->styleNew & WS_EX_CLIENTEDGE)
                || (ss->styleNew & WS_EX_DLGMODALFRAME)
                || (ss->styleNew & WS_EX_WINDOWEDGE)) != 0;
        }
        if (oldIsBorder != newIsBorder)
        {
            SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME);
        }

        if (!pData->fnWndproc)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        return pData->fnWndproc(hWnd, message, wParam, lParam, pData);
    }
    default:
        break;
    }

    //////////////////////////////////////////////////////////////////////////
    if (!pData->fnWndproc)
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    return pData->fnWndproc(hWnd, message, wParam, lParam, pData);
}

// 0=GetMessageW_hook, 1=GetMessageA_hook, 2=PeekMessageW_hook, 3=PeekMessageA_hook
inline BOOL _GetMessage_(int type, LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    BOOL ret = 0;
    switch (type)
    {
    case 0:
        ret = GetMessageW_hook(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
        break;
    case 1:
        ret = GetMessageA_hook(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
        break;
    case 2:
        ret = PeekMessageW_hook(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
        break;
    case 3:
        ret = PeekMessageA_hook(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
        break;
    default:
        break;
    }

    if (lpMsg->message == WM_KEYDOWN)
    {
        const short alt = GetAsyncKeyState(VK_MENU);
        const bool isAlt = alt & 0x8000;
        wstr::dbg(L"按键按下, 键代码 = %d, alt键状态 = %d\n", lpMsg->wParam, alt);
        if (lpMsg->wParam == '1' || lpMsg->wParam == '2' || lpMsg->wParam == '3')    // alt + 1, 2, 3
        {
            const short alt = GetAsyncKeyState(VK_MENU);
            const bool isAlt = alt & 0x8000;
            if (isAlt)
            {
                const short ctrl = GetAsyncKeyState(VK_CONTROL);
                const short shift = GetAsyncKeyState(VK_SHIFT);
                const bool isCtrl = ctrl & 0x8000;
                const bool isShift = shift & 0x8000;
                if (!isCtrl && !isShift)
                {
                    // 按下了 alt + 1, 2, 3 不能让他处理这个消息
                    lpMsg->message = m_message_invalid; // 把按键消息转换成其他消息
                }
            }
        }
    }
    return ret;
}
static BOOL WINAPI MyGetMessageW_hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    return _GetMessage_(0, lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, 0);
}
static BOOL WINAPI MyGetMessageA_hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    return _GetMessage_(1, lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, 0);
}
static BOOL WINAPI MyPeekMessageW_hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    return _GetMessage_(2, lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}
static BOOL WINAPI MyPeekMessageA_hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    return _GetMessage_(3, lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
}

// 子类化窗口, 把窗口句柄存放到map里, 并设置窗口默认的绘画数据, 如果需要自定义数据, 请在通过返回值进行修改
LPOBJSTRUCT _subclass(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    if (!IsWindow(hWnd))
        return 0;
    _hook_deal_wnd(hWnd);
    //static bool first;
    //if (!first)
    //{
    //    first = true;
    //    hook_GetMessageA.hook(&(PVOID&)GetMessageA_hook, MyGetMessageA_hook);
    //    hook_GetMessageW.hook(&(PVOID&)GetMessageW_hook, MyGetMessageW_hook);
    //    hook_PeekMessageW.hook(&(PVOID&)PeekMessageW_hook, MyPeekMessageW_hook);
    //    hook_PeekMessageA.hook(&(PVOID&)PeekMessageA_hook, MyPeekMessageA_hook);

    //    m_message_invalid = RegisterWindowMessageW(L"MyGetMessageA_hook");
    //}
    OBJSTRUCT& data = m_map_AllWindow_Subclass[hWnd];
    GetClientRect(hWnd, &data.pos.rcClient);
    RECT rcClient;
    CWndBase::GetRectFromParent(hWnd, &rcClient);
    data.pos.x      = rcClient.left;
    data.pos.x      = rcClient.top;
    data.pos.width  = rcClient.right  - rcClient.left;
    data.pos.height = rcClient.bottom - rcClient.top;

    data.hParent    = GetParent(hWnd);
    data.pParent    = GetDataFromHwnd(data.hParent);
    data.id         = GetDlgCtrlID(hWnd);
    assert(data.hWnd == 0); //如果不等于0, 代表已经子类化过了
    if (data.hWnd)return &data;
    s_info->pfnGetDefData(&data);

    if (IsWindowUnicode(hWnd))
    {
        data.fnCallProc = CallWindowProcW;
        data.fnDefProc  = DefWindowProcW;
        data.SetWndLong = (pfn_SetLong)SetWindowLongPtrW;
        data.GetWndLong = (pfn_GetLong)GetWindowLongPtrW;
        data.SetClsLong = (pfn_SetLong)SetClassLongPtrW;
        data.GetClsLong = (pfn_GetLong)GetClassLongPtrW;
    }
    else
    {
        data.fnCallProc = CallWindowProcA;
        data.fnDefProc  = DefWindowProcA;
        data.SetWndLong = (pfn_SetLong)SetWindowLongPtrA;
        data.GetWndLong = (pfn_GetLong)GetWindowLongPtrA;
        data.SetClsLong = (pfn_SetLong)SetClassLongPtrA;
        data.GetClsLong = (pfn_GetLong)GetClassLongPtrA;
    }

    if (isSetBack)
    {
        data.SetClsLong(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)data.draw.color.hbrBackGround);
    }

    data.fnWndproc = proc;
    data.hWnd = hWnd;
    
    //WS_CLIPSIBLINGS

    data.style   = data.GetWndLong(hWnd, GWL_STYLE);
    data.styleEx = data.GetWndLong(hWnd, GWL_EXSTYLE);

    if (reMoveClassStyle)
    {
        DWORD dwStyle = data.GetClsLong(hWnd, GCL_STYLE);
        dwStyle &= ~reMoveClassStyle;
        data.SetClsLong(hWnd, GCL_STYLE, dwStyle);
    }

    if (reMoveBorder)
    {
        data.styleEx &= ~(WS_EX_OVERLAPPEDWINDOW | WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME);
        data.style &= ~(WS_BORDER | WS_SIZEBOX);

        data.SetWndLong(hWnd, GWL_EXSTYLE, data.styleEx);
        data.SetWndLong(hWnd, GWL_STYLE, data.style);
    }

    if (data.oldProc == 0)
    {
        data.oldProc = (WNDPROC)data.SetWndLong(hWnd, GWLP_WNDPROC, (LONG_PTR)SubClassWindow_Proc);
    }
    //if (USER_DEFAULT_SCREEN_DPI != s_info->dpiX && hWnd != s_info->hEWnd)
    //    PostMessageW(hWnd, WM_DPICHANGED, 0, 0);
    return &data;
}



LPOBJSTRUCT _subclass_hMenu_Window(HWND hWnd, pfn_SubClassWndProc proc)
{
    auto it = m_map_AllWindow_Subclass.find(hWnd);
    if (it != m_map_AllWindow_Subclass.end())
    {
        LPOBJSTRUCT pData = &it->second;
        if (!pData->fnDrawItem)
            pData->fnDrawItem = EMenu_OnDrawItem;
        if (!pData->fnMeasureItem)
            pData->fnMeasureItem = EMenu_OnMeasureItem;
        return pData;
    }

    LPOBJSTRUCT pData = _subclass(hWnd, proc, false, false, 0);
    pData->fnDrawItem = EMenu_OnDrawItem;
    pData->fnMeasureItem = EMenu_OnMeasureItem;
    return pData;
    return 0;
}



LPOBJSTRUCT GetDataFromHwnd(HWND hWnd)
{
    if (!hWnd)return 0;
    auto it = m_map_AllWindow_Subclass.find(hWnd);
    if (it == m_map_AllWindow_Subclass.end())
        return 0;
    OBJSTRUCT& data = it->second;
    return &data;
}

LPOBJSTRUCT SetDataFromHwnd(HWND hWnd)
{
    LPOBJSTRUCT ret = GetDataFromHwnd(hWnd);
    if (ret)return ret;
    OBJSTRUCT& data = m_map_AllWindow_Subclass[hWnd];
    ret = &data;
    return ret;
}




bool save_eil(HWND hWnd, HIMAGELIST hList, LPCWSTR name)
{
    if (!hList)return false;

    //CImgList list;
    //list.Attach(hList);
    //int lsize;
    //LPBYTE pl = list.save(&lsize, true);
    //CFileRW f;
    //wstr file;
    //file.Format(L"D:\\cpp\\SVN工作目录\\易语言界面\\iDraw\\src\\%d%s.eli", hWnd, (name ? name : L""));
    //f.open(file.c_str(), 4, -1);
    //f.write(pl, lsize);
    //list.Attach(0);
    return true;
}

