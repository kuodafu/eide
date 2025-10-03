#include "../EWindow_Fne_Header.h"
#include <EIDE_Interface.h>


// 代码管理数据, 后期可能会绑定代码窗口管理类, 所以现在使用一个结构保存
struct EIDE_CODEMANAGE_INFO
{
    IEIDE_CodeManage* pCode;
};
std::unordered_map<HWND, EIDE_CODEMANAGE_INFO> m_map_CodeManage;   // 代码窗口对应的代码管理类, MDI创建时加入, MDI销毁时删除
// MDIClient下的第一层子窗口, 这个窗口的MDI窗口
LPOBJSTRUCT _subclass_MDIClient_Child1(HWND hWnd);
// MDIClient下的第一个子窗口子类化回调函数, 这个窗口是mdi窗口
LRESULT CALLBACK WndProc_MDIClient_Child1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_MDIClient(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// MDIClient下的第二层子窗口子类化回调函数
LRESULT CALLBACK WndProc_MDIClient_Child2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// MDIClient下的第三层子窗口子类化回调函数
LRESULT CALLBACK WndProc_MDIClient_Child3(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// MDIClient下的第四层子窗口子类化回调函数, 也就是代码框
LRESULT CALLBACK WndProc_MDIClient_Child4_Code(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);


LPOBJSTRUCT _subclass_MDIClient(HWND hWnd)
{
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_MDIClient, true, false);
    data->draw.isNcCalcSize = true;
    InsertStyle(data->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    return data;
}

inline LPOBJSTRUCT _subclass_MDIClient_Code(HWND hWnd, LPOBJSTRUCT pMDIClientChild1Data)
{
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_MDIClient_Child4_Code, true, false, 0);
    MDICLIENT_ALL_WINDOW* mdi = (MDICLIENT_ALL_WINDOW*)pMDIClientChild1Data->param2;

    EIDE_CODEMANAGE_INFO& CodeManageData = m_map_CodeManage[hWnd];
    if (!CodeManageData.pCode)
        CodeManageData.pCode = EIDE_CreateIEIDE_CodeManage(hWnd);

    mdi->hCode = hWnd;
    data->param2 = pMDIClientChild1Data->param2;
    InsertStyle(data->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL);
    SetClassLongPtrW(data->hWnd, GCL_STYLE, GetClassLongPtrW(data->hWnd, GCL_STYLE) | CS_VREDRAW | CS_HREDRAW);

    IDrawScroll* iScroll = ScrollCreate(data->hWnd, data->draw.color.crBorder,
                                        0,          // m_isSysListView, 如果是超级列表框的话这个值就是超级列表框的表项高度
                                        false,      // m_isSysTreeView, 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
                                        true,       // m_isCallSetScroll, 是否调用原来的设置滚动条位置函数
                                        false,      // isSubClass, 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
                                        true,       // isReCalcSize, 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
                                        true );

    if (!iScroll)
        return data;

    //iScroll->m_vInfo.btnThumbSize = 17;
    //iScroll->m_vInfo.btn1Size = 10;
    data->drawScroll = iScroll;

    ((IDrawScroll*)data->drawScroll)->m_isCallSetScroll = true;
    ((IDrawScroll*)data->drawScroll)->bind(data->hWnd, false, true);

    ((IDrawScroll*)data->drawScroll)->m_vScroll.cbSize = sizeof(SCROLLINFO);
    ((IDrawScroll*)data->drawScroll)->m_vScroll.fMask = SIF_ALL;
    GetScrollInfo(mdi->hvScroll, SB_CTL, &((IDrawScroll*)data->drawScroll)->m_vScroll);

    ((IDrawScroll*)data->drawScroll)->m_hScroll.cbSize = sizeof(SCROLLINFO);
    ((IDrawScroll*)data->drawScroll)->m_hScroll.fMask = SIF_ALL;
    GetScrollInfo(mdi->hhScroll, SB_CTL, &((IDrawScroll*)data->drawScroll)->m_hScroll);
    LPOBJSTRUCT pScrollWnd = GetDataFromHwnd(mdi->hhScroll);
    if (pScrollWnd)
    {
        pScrollWnd->drawScroll = data->drawScroll;
        IDrawScroll::SetobjFromHwnd(pScrollWnd->hWnd, ((IDrawScroll*)data->drawScroll));
        IDrawScroll::SetBindControl(pScrollWnd->hWnd, true, false);
        ((IDrawScroll*)data->drawScroll)->m_hWndHBind = pScrollWnd->hWnd;
    }
    pScrollWnd = GetDataFromHwnd(mdi->hvScroll);
    if (pScrollWnd)
    {
        pScrollWnd->drawScroll = data->drawScroll;
        IDrawScroll::SetobjFromHwnd(pScrollWnd->hWnd, ((IDrawScroll*)data->drawScroll));
        IDrawScroll::SetBindControl(pScrollWnd->hWnd, true, true);
        ((IDrawScroll*)pScrollWnd->drawScroll)->m_hWndVBind = pScrollWnd->hWnd;
    }
    return data;
}
// MDIClient下的第三层子窗口子类化回调函数, 子类化代码框父窗口和代码框窗口
inline LPOBJSTRUCT _subclass_MDIClient_Child3(HWND hWnd, LPOBJSTRUCT pMDIClientChild1Data)
{
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_MDIClient_Child3, true, false, 0);
    if (!data)return 0;
    MDICLIENT_ALL_WINDOW* mdi = (MDICLIENT_ALL_WINDOW*)pMDIClientChild1Data->param2;
    data->draw.isNcCalcSize = true;
    data->draw.isNoNcPaint = true;
    data->param2 = pMDIClientChild1Data->param2;
    data->draw.offsetRect.right += 18;
    data->draw.offsetRect.bottom += 18;
    InsertStyle(data->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    //SetClassLongPtrW(data->hWnd, GCL_STYLE, GetClassLongPtrW(data->hWnd, GCL_STYLE) | CS_VREDRAW | CS_HREDRAW);

    HWND hChild = GetWindow(hWnd, GW_CHILD);
    while (hChild)
    {
        int id = GetDlgCtrlID(hChild);
        if (id == 1)break;
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }
    if (!hChild)
        __debugbreak();

    SendMessageW(hChild, WM_HSCROLL, MAKELONG(SB_THUMBTRACK, 0), 0);
    _subclass_MDIClient_Code(hChild, pMDIClientChild1Data);
    SendMessageW(hChild, WM_HSCROLL, MAKELONG(SB_THUMBTRACK, 0), 0);

    return data;
}

// 设置代码框配色
LPOBJSTRUCT _subclass_MDIClient_Child1(HWND hWnd)
{
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_MDIClient_Child1, true, false, 0);        // 代码框容器最外层容器, 父窗口是 MDIClient
    if (!data)return 0;
    data->draw.isNcCalcSize = true;
    data->draw.isNoNcPaint = true;
    InsertStyle(data->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    MDICLIENT_ALL_WINDOW* mdi = new MDICLIENT_ALL_WINDOW;
    memset(mdi, 0, sizeof(MDICLIENT_ALL_WINDOW));

    data->draw.color.crBackGround       = RGB(37, 37, 38);
    data->draw.color.crText             = RGB(222,222,222);
    data->draw.color.crTextHot          = RGB(222,222,222);
    data->draw.color.crTextDown         = RGB(222,222,222);
    data->draw.color.crTextBan          = RGB(222,222,222);
    data->param2 = mdi;
    mdi->hChild1 = hWnd;

    HWND hChild = GetWindow(hWnd, GW_CHILD);
    HWND hEWindow = GetWindow(hChild, GW_CHILD);
    if (GetDlgCtrlID(hEWindow) == 1)    // 
    {
        wchar_t cls[260] = { 0 };
        HWND WTWindow = GetWindow(hEWindow, GW_CHILD);
        int id = GetDlgCtrlID(WTWindow);
        if (id == 117)
        {
            _subclass_MDIClient_EWindow(hWnd, data, mdi);
        }
        else
        {
            // 起始页
            _subclass_MDIClient_EHomePage(hWnd, data, mdi);
        }
        return data;
    }

    LPOBJSTRUCT pChild = _subclass(hChild, WndProc_MDIClient_Child2, true);    // 代码框的容器, 里面包含了几个滚动条
    pChild->draw.isNcCalcSize = true;
    pChild->draw.isNoNcPaint = true;
    pChild->param2 = mdi;
    InsertStyle(pChild->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    //SetClassLongPtrW(pChild->hWnd, GCL_STYLE, GetClassLongPtrW(pChild->hWnd, GCL_STYLE) | CS_VREDRAW | CS_HREDRAW);
    mdi->hChild2 = hChild;


    hChild = GetWindow(hChild, GW_CHILD);
    while (hChild)
    {
        int id = GetDlgCtrlID(hChild);
        switch (id)
        {
        case 59904: // 横向滚动条
            ShowWindowEx(hChild, SW_HIDE);
            mdi->hhScroll = hChild;
            _subclass_MDIClient_HScroll(hChild, data);
            break;
        case 59920: // 纵向滚动条
            ShowWindowEx(hChild, SW_HIDE);
            mdi->hvScroll = hChild;
            _subclass_MDIClient_VScroll(hChild, data);
            break;
        case 59936: // 右下角滚动条
            ShowWindowEx(hChild, SW_HIDE);
            _subclass_MDIClient_VScroll(hChild, data);
            break;
        case 59648: // 代码框父窗口
            mdi->hChild3 = hChild;
            _subclass_MDIClient_Child3(hChild, data);
            break;
        default:
            break;
        }
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }
    return data;
}

static bool m_isCallActivate;
LRESULT CALLBACK WndProc_MDIClient(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    static HWND s_hWndMdi;
    switch (message)
    {
    case WM_ERASEBKGND:
        return 1;
    //case WM_MDIGETACTIVE:
    //    return (LRESULT)s_hWndMdi;
    case WM_MDIMAXIMIZE:
    case WM_MDIRESTORE:
    case WM_MDITILE:
    case WM_MDICASCADE:
    case WM_MDIICONARRANGE:

        return 0;
    case WM_MDICREATE:
    {
        //TODO 这里返回的话可以加快启动速度
        //return (LRESULT)hRet;
        //if (s_hWndMdi)return (LRESULT)s_hWndMdi;
        //return (LRESULT)0;
        IRE_STRUCT_MDICREATE evt;
        evt.hWnd = 0;
        evt.mcs = (LPMDICREATESTRUCTA)lParam;
        LPREGISTER_EVENT_INFO arrEvt[EVENT_MAX_ITEM]; int isReturn = 0;
        int evtCount = s_info->evt.pfn_Event_callback_before(arrEvt, IRE_MDICLIENT_CREATE, &evt, &isReturn, false);
        if (isReturn) return 0;     // 插件处理了, 不往后处理
        HWND hWndMDI = (HWND)WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        s_hWndMdi = hWndMDI;
        //HWND hWndCode = GetIEIDE_CodeWindow(hWndMDI);
        //if (hWndCode)
        //{
        //    EIDE_CODEMANAGE_INFO& data = m_map_CodeManage[hWndCode];
        //    data.pCode = EIDE_CreateIEIDE_CodeManage(hWndCode);
        //}

        //return (LRESULT)hWndMDI;
        evt.hWnd = hWndMDI;

        //wstr::dbg(L"mdi窗口被创建, 窗口句柄 = %d\n", hWndMDI);

        if (hWndMDI)    // MDI窗口创建, 需要加入数组, 并且加入选择夹
        {
            int len = GetWindowTextLengthW(hWndMDI) + 1;
            wstr text(len);
            GetWindowTextW(hWndMDI, text.data(), len);

            CUSTOMTAB_DATA CustomTabData = { 0 };
            CustomTabData.hWnd = hWndMDI;
            CustomTabData.iImage = -1;
            CustomTabData.type = 0;
            CustomTabData.crText = RGB(222, 222, 222);
            CustomTabData.text = 0;
            CustomTabData.lParam = (LPARAM)hWndMDI;
            CustomTabData.name = s_info->copyStrW(text.c_str(), CustomTabData.name);

            s_info->hWnds->push_back(CustomTabData);
            _subclass_MDIClient_Child1(hWndMDI);        // MDIClient 下的第一个子窗口

        }

        s_info->evt.pfn_Event_callback_after(arrEvt, IRE_MDICLIENT_CREATE, &evt, evtCount, false);

        return (LRESULT)hWndMDI;
    }
    case WM_PAINT:
        return WndProc_DefPaint(hWnd, message, wParam, lParam, pData);
    case WM_MDIDESTROY:
    {
        HWND hWndMDI = (HWND)wParam;
        HWND hWndCode = GetIEIDE_CodeWindow(hWndMDI);
        if (hWndCode)
        {
            auto itCodeManage = m_map_CodeManage.find(hWndCode);
            assert(itCodeManage != m_map_CodeManage.end()); // 如果没有找到就断下
            if (itCodeManage != m_map_CodeManage.end())
            {
                EIDE_CODEMANAGE_INFO& data = itCodeManage->second;
                EIDE_DeleteIEIDE_CodeManage(data.pCode);
                m_map_CodeManage.erase(itCodeManage);
            }
        }

        LPREGISTER_EVENT_INFO arrEvt[EVENT_MAX_ITEM]; int isReturn = 0;
        int evtCount = s_info->evt.pfn_Event_callback_before(arrEvt, IRE_MDICLIENT_DESTROY, hWnd, &isReturn, false);
        if (isReturn) return 0;     // 插件处理了, 不往后处理


        for (int i = 0; i < s_info->hWnds->size(); ++i)    // MDI窗口被销毁, 需要从数组里删除对应的成员, 并且删除自绘选择夹的项目
        {
            CUSTOMTAB_DATA& CustomTabData = s_info->hWnds->at(i);
            if (CustomTabData.hWnd == (HWND)wParam)
            {
                if (CustomTabData.text)
                    wstr::free(CustomTabData.text);
                //TabCtrl_DeleteItem(s_info->hEWnd, i);
                s_info->hWnds->erase(i);
                // 把销毁mdi窗口的消息转发给自绘选择夹处理
                WndProc_MDIClient_CustomTabCtrl(hWnd, WM_MDIDESTROY, wParam, 121007124);
                
                break;
            }
        }
        if (s_info->hWnds->size() == 0)
            s_info->tab.indexSel = -1;
        s_info->evt.pfn_Event_callback_after(arrEvt, IRE_MDICLIENT_DESTROY, hWnd, evtCount, false);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}


LRESULT CALLBACK WndProc_MDIClient_Child1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_ERASEBKGND:
        return 1;
    case WM_TIMER:
    {
        if (wParam == 10086)
        {
            EIDE_Init();
            KillTimer(hWnd, wParam);
            break;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_MDIACTIVATE:
    {
        //TODO 这里处理有问题, 会触发很多次, 需要优化处理
        class _auto_release
        {
            HWND m_hWnd;
            const LPCWSTR guid = L"{EA551624-F4BA-4768-AB23-A408376270FE}";
        public:
            _auto_release(HWND hWnd) : m_hWnd(0)
            {
                HWND hMDI = (HWND)GetPropW(s_info->hMDIClient, guid);
                if (hMDI != hWnd)
                {
                    SetPropW(s_info->hMDIClient, guid, hWnd);
                    m_hWnd = hWnd;
                }
            }
            ~_auto_release()
            {

            }
            inline operator bool()
            {
                return m_hWnd != 0;
            }

        };
        auto pfnTimeProc = [](HWND hWnd, UINT message, UINT_PTR id, DWORD timer) -> void
        {
            if (IsWindowVisible(s_info->eWnd->GetControl()) && !IsWindowVisible(hWnd))
            {
                ShowWindowEx(hWnd, SW_SHOW);
                KillTimer(hWnd, id);
            }
        };
        SetTimer(s_info->eWnd->GetControl32770Parent(), 10, 10, pfnTimeProc);
        HWND hOldActivate = (HWND)SendMessageW(s_info->hMDIClient, WM_MDIGETACTIVE, 0, 0);
        HWND hNewActivate = (HWND)lParam;
        _auto_release prop((HWND)lParam);
        //if (hOldActivate == hNewActivate)
        //    return 0;
        if (!prop)
            return DefWindowProcA(hWnd, message, wParam, lParam);
            //return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        LPREGISTER_EVENT_INFO arrEvt[EVENT_MAX_ITEM]; int isReturn = 0;
        int evtCount = s_info->evt.pfn_Event_callback_before(arrEvt, IRE_MDICLIENT_ACTIVATE, (void*)lParam, &isReturn, false);
        if (isReturn) return 0;     // 插件处理了, 不往后处理

        int index = -1;
        const int count = s_info->hWnds->size();
        for (int i = 0; i < count; i++)
        {
            CUSTOMTAB_DATA& CustomTabData = s_info->hWnds->at(i);
            if (CustomTabData.hWnd == (HWND)lParam)
            {
                //TabCtrl_SetCurSel(s_info->hCustomTab, i);
                index = i;
                break;
            }
            else
            {
                //ShowWindowEx(CustomTabData.hWnd, SW_HIDE);
            }
        }

        MDIClient_CustomTabCtrl_ClickItem(index, false, s_info->pWnd, TAB_SEL_MODE_CODE);
        
        s_info->evt.pfn_Event_callback_after(arrEvt, IRE_MDICLIENT_ACTIVATE, (void*)lParam, evtCount, false);
        if (wParam != lParam && hWnd == (HWND)lParam)
        {
            IEIDE_CodeManage* ide = GetIEIDE_CodeManage(0);
            EIDE_PushStack(ide, (HWND)lParam);
        }
        if (hWnd == 0)return 0;
        return DefWindowProcA(hWnd, message, wParam, lParam);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_SETTEXT:
    {
        WndProc_MDIClient_CustomTabCtrl(hWnd, WM_SETTEXT, MAKELONG(0, 0xFFFF), lParam);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_DESTROY:
    {
        MDICLIENT_ALL_WINDOW* mdi = (MDICLIENT_ALL_WINDOW*)pData->param2;
        delete mdi;
        pData->param2 = 0;
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_STYLECHANGING:
    {
        LPSTYLESTRUCT ss = (LPSTYLESTRUCT)lParam;
        if (wParam == GWL_STYLE)
        {
            ss->styleNew &= ~WS_BORDER;
            ss->styleNew &= ~SS_SUNKEN;
        }
        else
        {
            ss->styleNew &= ~WS_EX_CLIENTEDGE;
            ss->styleNew &= ~WS_EX_OVERLAPPEDWINDOW;
            ss->styleNew &= ~WS_EX_PALETTEWINDOW;
            ss->styleNew &= ~WS_EX_WINDOWEDGE;
            ss->styleNew &= ~WS_EX_STATICEDGE;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_PAINT:
        return WndProc_DefPaint(hWnd, message, wParam, lParam, pData);
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}



LRESULT CALLBACK WndProc_MDIClient_Child2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_PARENTNOTIFY:
    {
        const int code = LOWORD(wParam);
        const int id = HIWORD(wParam);
        if (code == WM_CREATE && id >= 59600)
        {
            HWND hChild = (HWND)lParam;
            hChild = GetWindow(hChild, GW_CHILD);
            if (GetDlgCtrlID(hChild) == 1)
            {
                _subclass_MDIClient_Code(hChild, pData);
            }
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_HSCROLL:
    case WM_VSCROLL:
    {
        MDICLIENT_ALL_WINDOW* mdi = (MDICLIENT_ALL_WINDOW*)pData->param2;
        LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        LPOBJSTRUCT pChild = GetDataFromHwnd((HWND)mdi->hCode);
        if (pChild && pChild->drawScroll)
        {
            SCROLLINFO& si = ((IDrawScroll*)pChild->drawScroll)->m_vScroll;
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;
            SendMessageW(mdi->hvScroll, SBM_GETSCROLLINFO, SB_CTL, (LPARAM)&si);
            ((IDrawScroll*)pChild->drawScroll)->InvalidateRect();
        }
        return ret;
    }
    case WM_PAINT:
        return WndProc_DefPaint(hWnd, message, wParam, lParam, pData);
    case WM_ERASEBKGND:
        return 1;
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

LRESULT CALLBACK WndProc_MDIClient_Child3(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_SIZE:
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
        return WndProc_DefPaint(hWnd, message, wParam, lParam, pData);
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}
LRESULT CALLBACK WndProc_MDIClient_Child4_Code(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
    if (pObj)
    {
        LRESULT ret = 0;
        switch (message)
        {
        case WM_MOUSEWHEEL:
        {
            //wstr::dbg(L"鼠标滚动距离 = %d\n", ((int)((short)HIWORD(wParam))));
            const bool isUp = ((short)HIWORD(wParam)) > 0;
            int pos = s_info->config->wheelPos();
            if (pos <= 0 || pos > 30) pos = 3;
            //for (int i = 1; i < pos; i++) // 多滚动行数
            //    WndProc_CallWindowProc(hWnd, WM_VSCROLL, (isUp ? SB_LINEUP : SB_LINEDOWN), 0, pData);

            const int newpos = pObj->m_vScroll.nPos + (isUp ? -pos + 1 : pos - 1) ;
            WndProc_CallWindowProc(hWnd, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, newpos), 0, pData);
            break;
        }
        default:
            break;
        }
        if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
        {
            return ret;
        }
    }
    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_ERASEBKGND:
        return 1;
    case WM_DESTROY:
    {
        if (pData->drawScroll)
            delete pData->drawScroll;
        pData->drawScroll = 0;
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_PAINT:
    {
        return MDIClientCode_BeginPaint(hWnd, message, wParam, lParam, pData, pObj);
    }
    case WM_NCHITTEST:
    {
        return HTCLIENT;
    }
    case WM_LBUTTONDOWN:
    {
        static bool b = false;
        if (b) return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        IEIDE_CodeManage* pCodeManage = GetIEIDE_CodeManage(hWnd);
        if (!pCodeManage) return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        b = true;
        IRE_STRUCT_EIDE_SELCHANGEDINFO* info = new IRE_STRUCT_EIDE_SELCHANGEDINFO;
        memset(info, 0, sizeof(IRE_STRUCT_EIDE_SELCHANGEDINFO));
        info->pCodeManage = GetIEIDE_CodeManage(hWnd);
        POINT pt = info->pCodeManage->GetCursorPos();
        info->oldLine = pt.x;
        info->oldLineCol = pt.y;

        LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        info->line = info->pCodeManage->GetCursorPos().x;

        EIDE_LButtonDown(info);
        EIDE_Init(true);
        return ret;
    }
    //case WM_KEYDOWN:
    //{
    //    if (wParam == 'F')
    //    {
    //        const int shift     = GetAsyncKeyState(VK_SHIFT);
    //        const int ctrl      = GetAsyncKeyState(VK_CONTROL);
    //        const bool isShift  = (shift & 32768) != 0;
    //        const bool isCtrl   = (ctrl  & 32768) != 0;
    //        if (isCtrl && isShift)
    //            return false;
    //    }
    //    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //}
    //case WM_CHAR:
    //{
    //    IEIDE_CodeManage* pCodeManage = GetIEIDE_CodeManage(hWnd);
    //    if (!pCodeManage) return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    //    if (wParam == VK_RETURN)    // 输入了回车键
    //    {
    //        IRE_EIDE_SELCHANGEDINFO* info = new IRE_EIDE_SELCHANGEDINFO;
    //        memset(info, 0, sizeof(IRE_EIDE_SELCHANGEDINFO));
    //        info->pCodeManage = GetIEIDE_CodeManage(hWnd);
    //        POINT pt = info->pCodeManage->GetCursorPos();
    //        info->oldLine = pt.x;
    //        info->oldLineCol = pt.y;

    //        LRESULT ret = WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //        info->line = info->pCodeManage->GetCursorPos().x;
    //        EIDE_LButtonDown(info);
    //        return ret;
    //    }
    //    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //}
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}


// 获取当前代码框句柄, 如果当前显示的是设计器窗口或者起始页都返回0
HWND GetIEIDE_CodeWindow(HWND hWnd)
{
    EWINDOW_MDICHILD wnd;
    s_info->eWnd->GetMDIClientChildWindow(hWnd, wnd);
    if (!wnd.hMDIClient4) return 0;

    int id = GetDlgCtrlID(wnd.hMDIClient4);
    if (id != 1)return 0;
    return wnd.hMDIClient4;
}

// 获取指定代码窗口的代码管理类, 如果句柄为0则获取当前的代码框句柄, 如果当前显示的是设计器窗口或者起始页都返回0
IEIDE_CodeManage* GetIEIDE_CodeManage(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))
        hWnd = GetIEIDE_CodeWindow(0);
    
    if (!hWnd) return 0;

    auto it = m_map_CodeManage.find(hWnd);
    if (it == m_map_CodeManage.end())
        return 0;
    EIDE_CODEMANAGE_INFO& data = it->second;
    return data.pCode;
}