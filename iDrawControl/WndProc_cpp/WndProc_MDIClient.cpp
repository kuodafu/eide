#include "../EWindow_Fne_Header.h"
#include <EIDE_Interface.h>


// �����������, ���ڿ��ܻ�󶨴��봰�ڹ�����, ��������ʹ��һ���ṹ����
struct EIDE_CODEMANAGE_INFO
{
    IEIDE_CodeManage* pCode;
};
std::unordered_map<HWND, EIDE_CODEMANAGE_INFO> m_map_CodeManage;   // ���봰�ڶ�Ӧ�Ĵ��������, MDI����ʱ����, MDI����ʱɾ��
// MDIClient�µĵ�һ���Ӵ���, ������ڵ�MDI����
LPOBJSTRUCT _subclass_MDIClient_Child1(HWND hWnd);
// MDIClient�µĵ�һ���Ӵ������໯�ص�����, ���������mdi����
LRESULT CALLBACK WndProc_MDIClient_Child1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_MDIClient(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// MDIClient�µĵڶ����Ӵ������໯�ص�����
LRESULT CALLBACK WndProc_MDIClient_Child2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// MDIClient�µĵ������Ӵ������໯�ص�����
LRESULT CALLBACK WndProc_MDIClient_Child3(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// MDIClient�µĵ��Ĳ��Ӵ������໯�ص�����, Ҳ���Ǵ����
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
                                        0,          // m_isSysListView, ����ǳ����б��Ļ����ֵ���ǳ����б��ı���߶�
                                        false,      // m_isSysTreeView, �Ƿ�Ϊ���Ϳ�, ���Ϊ��, �������ù�����λ��, ���͹�����Ϣ, Ϊ�����ȷ��͹�����Ϣ������λ��
                                        true,       // m_isCallSetScroll, �Ƿ����ԭ�������ù�����λ�ú���
                                        false,      // isSubClass, �Ƿ����໯, ���Ϊtrue, �ڲ������໯�������, ���Ϊ��, ������ⲿ����Ϣ���ݵ� OnWndProc ������
                                        true,       // isReCalcSize, �Ƿ����¼���ߴ�, Ϊtrue���ڲ������ SetWindowPos �������¼���ǿͻ���
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
// MDIClient�µĵ������Ӵ������໯�ص�����, ���໯����򸸴��ںʹ���򴰿�
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

// ���ô������ɫ
LPOBJSTRUCT _subclass_MDIClient_Child1(HWND hWnd)
{
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_MDIClient_Child1, true, false, 0);        // ������������������, �������� MDIClient
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
            // ��ʼҳ
            _subclass_MDIClient_EHomePage(hWnd, data, mdi);
        }
        return data;
    }

    LPOBJSTRUCT pChild = _subclass(hChild, WndProc_MDIClient_Child2, true);    // ����������, ��������˼���������
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
        case 59904: // ���������
            ShowWindowEx(hChild, SW_HIDE);
            mdi->hhScroll = hChild;
            _subclass_MDIClient_HScroll(hChild, data);
            break;
        case 59920: // ���������
            ShowWindowEx(hChild, SW_HIDE);
            mdi->hvScroll = hChild;
            _subclass_MDIClient_VScroll(hChild, data);
            break;
        case 59936: // ���½ǹ�����
            ShowWindowEx(hChild, SW_HIDE);
            _subclass_MDIClient_VScroll(hChild, data);
            break;
        case 59648: // ����򸸴���
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
        //TODO ���ﷵ�صĻ����Լӿ������ٶ�
        //return (LRESULT)hRet;
        //if (s_hWndMdi)return (LRESULT)s_hWndMdi;
        //return (LRESULT)0;
        IRE_STRUCT_MDICREATE evt;
        evt.hWnd = 0;
        evt.mcs = (LPMDICREATESTRUCTA)lParam;
        LPREGISTER_EVENT_INFO arrEvt[EVENT_MAX_ITEM]; int isReturn = 0;
        int evtCount = s_info->evt.pfn_Event_callback_before(arrEvt, IRE_MDICLIENT_CREATE, &evt, &isReturn, false);
        if (isReturn) return 0;     // ���������, ��������
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

        //wstr::dbg(L"mdi���ڱ�����, ���ھ�� = %d\n", hWndMDI);

        if (hWndMDI)    // MDI���ڴ���, ��Ҫ��������, ���Ҽ���ѡ���
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
            _subclass_MDIClient_Child1(hWndMDI);        // MDIClient �µĵ�һ���Ӵ���

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
            assert(itCodeManage != m_map_CodeManage.end()); // ���û���ҵ��Ͷ���
            if (itCodeManage != m_map_CodeManage.end())
            {
                EIDE_CODEMANAGE_INFO& data = itCodeManage->second;
                EIDE_DeleteIEIDE_CodeManage(data.pCode);
                m_map_CodeManage.erase(itCodeManage);
            }
        }

        LPREGISTER_EVENT_INFO arrEvt[EVENT_MAX_ITEM]; int isReturn = 0;
        int evtCount = s_info->evt.pfn_Event_callback_before(arrEvt, IRE_MDICLIENT_DESTROY, hWnd, &isReturn, false);
        if (isReturn) return 0;     // ���������, ��������


        for (int i = 0; i < s_info->hWnds->size(); ++i)    // MDI���ڱ�����, ��Ҫ��������ɾ����Ӧ�ĳ�Ա, ����ɾ���Ի�ѡ��е���Ŀ
        {
            CUSTOMTAB_DATA& CustomTabData = s_info->hWnds->at(i);
            if (CustomTabData.hWnd == (HWND)wParam)
            {
                if (CustomTabData.text)
                    wstr::free(CustomTabData.text);
                //TabCtrl_DeleteItem(s_info->hEWnd, i);
                s_info->hWnds->erase(i);
                // ������mdi���ڵ���Ϣת�����Ի�ѡ��д���
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
        //TODO ���ﴦ��������, �ᴥ���ܶ��, ��Ҫ�Ż�����
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
        if (isReturn) return 0;     // ���������, ��������

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
            //wstr::dbg(L"���������� = %d\n", ((int)((short)HIWORD(wParam))));
            const bool isUp = ((short)HIWORD(wParam)) > 0;
            int pos = s_info->config->wheelPos();
            if (pos <= 0 || pos > 30) pos = 3;
            //for (int i = 1; i < pos; i++) // ���������
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

    //    if (wParam == VK_RETURN)    // �����˻س���
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


// ��ȡ��ǰ�������, �����ǰ��ʾ������������ڻ�����ʼҳ������0
HWND GetIEIDE_CodeWindow(HWND hWnd)
{
    EWINDOW_MDICHILD wnd;
    s_info->eWnd->GetMDIClientChildWindow(hWnd, wnd);
    if (!wnd.hMDIClient4) return 0;

    int id = GetDlgCtrlID(wnd.hMDIClient4);
    if (id != 1)return 0;
    return wnd.hMDIClient4;
}

// ��ȡָ�����봰�ڵĴ��������, ������Ϊ0���ȡ��ǰ�Ĵ������, �����ǰ��ʾ������������ڻ�����ʼҳ������0
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