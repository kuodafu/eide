#include "EWindow_Size_Header.h"
#include <iDraw_const.h>



//// �Ƿ��Զ�����
//static bool state_isAutohide(LPTOPSTATE_INFO info)
//{
//    return __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE);
//}

_EWINDOW_BEGIN_NAMESPACE
const LPCWSTR oldSelParamProp = L"{7B745F5E-8958-4B6E-8EDD-49DC61312F7E}";
static bool m_isNoMove_state = false;
static bool m_move_end = false;     // �Ѿ�����ƶ���
void EWindow_MoveStateChild(HDWP& hDwp);
static int m_tab_left_proportion;   // ���ѡ��б���, ��ֱ�
struct TAB_SEL_CHANGE_STRUCT
{
    std::vector<HWND>* hWnds;   // ��һ����ʾ�Ĵ�������
    int param;      // ��һ�ε������Ŀ��ֵ, ���Ը��������ֵ֪����һ������ʾ��Щ����
};

inline RECT& _get_rect_state()
{
    return IsAutoHide(s_info->hState) ? s_info->rcState1 : s_info->rcState;
}

// ��ȡѡ�����Ŀ��ֵ, index=-1���ȡ��ǰѡ����
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
    // ״̬�п���ͨ�� ״̬�� �����Դ��� �����4����
    // �⼸��������Ѿ��ƶ����, ֱ��ʹ�����ǵ�λ�þ���
    LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(s_info->hState);
    if (!pData) return;
    LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pData->param;
    RECT& rcState = _get_rect_state();  // �����Ƿ��Զ�����ʹ�ò�ͬ�Ľṹ
    const bool isAutoHide = &rcState == &s_info->rcState1;
    if (!isAutoHide && !IsWindowVisible(s_info->hState))
        ShowWindowEx(s_info->hState, SW_SHOW);
    //RECT& rcState = s_info->rcState;
    if (rcState.right == 0 || rcState.bottom == 0)
        GetClientRect(s_info->hState, &rcState);
    const int cyChild = rcState.bottom - rcState.top;
    const int cyStatus = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;
    bool isDrawFrame = false;
    // ״̬��λ���Ǹ���״̬�����ı��
    rcState.left    = WINDOW_BORDER_WIDTH;
    rcState.right   = s_info->cxClient - WINDOW_BORDER_WIDTH;
    rcState.bottom  = s_info->rcStatusBar.top - s_info->scale((isAutoHide ? WINDOW_AUTO_HEIGHT : 0));
    rcState.top     = rcState.bottom - cyChild;
    if (IsAutoHide(s_info->hWork))
        rcState.left = s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH * 2);
    if (IsAutoHide(s_info->hControl))
        rcState.right = s_info->cxClient - s_info->scale(WINDOW_BORDER_WIDTH * 2 + WINDOW_AUTO_HEIGHT);

    // MDI��С�߶� + ��Ͽ� + ѡ���
    const int offset = WINDOW_MIN_MDIHEIGHT + WINDOW_CUSTOMTAB_HEIGHT * 2;

    // ������� + MDI + ��Ͽ�+ѡ��� С�ڲ˵����ĵױ�
    // ��״̬�еĶ��� = �˵����ĵױ� + MDI + ��Ͽ� + ѡ���, �������Ͽ�Ӧ�û���ƫ��, ������ν.....
    if ((rcState.top - offset) < s_info->rcWndMenu.bottom)
        rcState.top = s_info->rcWndMenu.bottom + offset;

    if (rcState.bottom - rcState.top < WINDOW_MINTRACKSIZE)
        rcState.top = rcState.bottom - WINDOW_MINTRACKSIZE; // Ӧ���ǲ����ߵ������

    if(isAutoHide)
    {
        if (s_info->iDraw->TopState_GetActivateWindow() == s_info->hState)
        {
            // �ߵ������һ�����Զ�����, �����ǵ�ǰ����Ĵ���, �Ǿ�����ʾ������, ��Ҫ������ʾ��λ��
            // �ױ���Ҫ���ϵ�, �ױ��Ѿ�����һ���ֻ滭�Ի�ѡ�����
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

// ����߻����ұߵ�ѡ��д����Ӽб��ı��¼�, 0=���, 1=�ұ�, 2=Ĭ��ѡ���, -1=ȫ��
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


// ��������������������λ��
inline void _tab_move_vartable(int param, RECT& rc, TAB_SEL_CHANGE_STRUCT* pOlds)
{
    HWND hWndShow = s_info->eWnd->GetStateTabVarTableStatic();
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    SetWindowPos(hWndShow, HWND_TOP, rc.left, rc.top, cxClient, cyClient, 0);

    // ��׼��������ֹͣ��ť��������״̬, ���к�ֹͣ��ť������, ���а�ť������, ��ͣ��������ť������
    // ��ʾ���ڲ��ǵ���״̬, ���λ�ú��������һ��
    if (!s_info->isRun || !s_info->isStop)  // ���к�ֹͣ��ť������һ����������״̬, �����ڶϵ����, ��Ҫ�ѱ�ǩ����ʾ����, �����������
    {
        __hide_vartable(false);      // ���ر������������, λ�û���Ҫ����
    }
    // �ߵ�����ǵ���״̬, �ж�������Ҫ����λ��
    HWND hTree = s_info->eWnd->GetStateVarTableTree();
    HWND hCombobox = s_info->eWnd->GetStateStackCombobox();
    HWND hBtn = s_info->eWnd->GetStateTabVarTableWriteBtn();
    HWND hType = s_info->eWnd->GetStateTypeSize();
    HWND hEdit = s_info->eWnd->GetStateTabVarTableEdit();

    if (pOlds->param != param)    // �л������ѡ��֮ǰ�д�������ʾ, ������Ҫ����������
    {
        for (auto it = pOlds->hWnds->begin(); it != pOlds->hWnds->end(); ++it)
            ShowWindowEx(*it, SW_HIDE);   // ���ؾɴ���
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
    // ���Ϳ�ռ����� 1/3 ��λ��
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
        
        SetTimer(hWnd, (UINT_PTR)&pfn_timer, 300, pfn_timer);  // �ӳ�һ�뱣�浽������
    }
    //pData->isReturn = true;
    return 0;
}

// �ױߵ�״̬������, ��Ҫ�Ǳ�������Ҫ�޸�, ��������һ������
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnStateSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    static int s_i;
    m_move_end = true;
    //wstr::dbg(L"״̬�� �ػ����� = %d\n", ++s_i);
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
        if (!isAutoHide)   // �����Զ����صĲ��ƶ�������/�����
        {
            // ��Ҫ�ƶ������к������
            // MDI�Ǹ��ݹ����к���������λ�õ�
            // �������Ǹ���״̬�е���λ��
            EWindow_MoveWork(hDwp);             // 5. �ƶ���ߵĹ�����
            EWindow_MoveControl(hDwp);          // 6. �ƶ������, ��������״̬��������λ��

            EWindow_MoveMDI_Tab_Cbx(hDwp);      // 7,8,9 �ƶ� MDI, �Ի�ѡ���, ������Ͽ�
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



    RECT rcMove = { 0, s_info->scale(WINDOW_TOP_STATE) + WINDOW_SPACING, cxClient, cyClient };    // Ԥ���Ϸſռ�
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
    // ��������ᴥ��ѡ��ı�, �����Ի�Ѵ�����ʾ����, ����ֱ�ӵ����Լ��Ĵ�����
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
    case TAB_PARAM_TIPS:     // ��ʾ
        hWndShow = s_info->eWnd->GetStateTabTips();
        break;
    case TAB_PARAM_OUTPUT:     // ���
        hWndShow = s_info->eWnd->GetStateTabOutput();
        break;
    case TAB_PARAM_CALLTABLE:     // ���ñ�
        hWndShow = s_info->eWnd->GetStateTabCallTable();
        break;
    case TAB_PARAM_MONITORTABLE:     // ���ӱ�
        hWndShow = s_info->eWnd->GetStateTabMonitorTable();
        break;
    case TAB_PARAM_VARTABLE:     // ������
        hWndShow = s_info->eWnd->GetStateTabVarTableStatic();
        if (IsWindowVisible(hWndShow)) break;   // ��ʾ��������ʾ���ڲ��ǵ���״̬, ���λ�ú��������һ��

        hWndShow = s_info->eWnd->GetStateVarTableTree();
        break;
    case TAB_PARAM_FIND1:     // ��Ѱ1
        hWndShow = s_info->eWnd->GetStateTabFind1();
        break;
    case TAB_PARAM_FIND2:     // ��Ѱ2
        hWndShow = s_info->eWnd->GetStateTabFind2();
        break;
    case TAB_PARAM_CLIP:     // ������ʷ
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

// �ж��Ƿ���������״̬���Դ������, ������ѡ����Լ����ϲ�Ĵ���
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

// ��ʾѡ���ָ�����Ӽ�, ���Ӽж�Ӧ�Ĵ�����ʾ����
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
    case TAB_PARAM_TIPS:     // ��ʾ
        hWndShow = s_info->eWnd->GetStateTabTips();
        break;
    case TAB_PARAM_OUTPUT:     // ���
        hWndShow = s_info->eWnd->GetStateTabOutput();
        break;
    case TAB_PARAM_CALLTABLE:     // ���ñ�
        hWndShow = s_info->eWnd->GetStateTabCallTable();
        break;
    case TAB_PARAM_MONITORTABLE:     // ���ӱ�
        hWndShow = s_info->eWnd->GetStateTabMonitorTable();
        break;
    case TAB_PARAM_VARTABLE:     // ������
    {
        if (s_info->isStop && s_info->isRun)    // ֹͣ��ť������״̬, �������а�ťҲ������״̬, �ǵ�ǰ�����ж���, ��Ҫ��ʾ��һ�����
        {
            hWndShow = 0;
            showWndIsVarCtl = true;
            ShowWindowEx(hWndShow, SW_HIDE);    // �����Ǹ���ǩ, Ȼ����ʾ��һ�����
            for (int i = 0; i < sizeof(hWnds) / sizeof(hWnds[0]); i++)
                ShowWindowEx(hWnds[i], SW_SHOW);
            break;
        }

        hWndShow = s_info->eWnd->GetStateTabVarTableStatic();
        for (int i = 0; i < sizeof(hWnds) / sizeof(hWnds[0]); i++)
            ShowWindowEx(hWnds[i], SW_HIDE);    // �ѱ����������������
        break;
    }
    case TAB_PARAM_FIND1:     // ��Ѱ1
        hWndShow = s_info->eWnd->GetStateTabFind1();
        break;
    case TAB_PARAM_FIND2:     // ��Ѱ2
        hWndShow = s_info->eWnd->GetStateTabFind2();
        break;
    case TAB_PARAM_CLIP:     // ������ʷ
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
    while (hChild)  // ��ѡ����²��ǵ�ǰ��ʾ�Ĵ��ڸ�����, ֻ�����������Դ�����
    {
        // ��ǰ��Ҫ��ʾ�Ĵ��ں�ѡ����µĴ��ڲ�һ��, ��Ҫ��һ���ж��Ƿ���Ҫ����
        if (hChild != hWndShow && IsWindowVisible(hChild))
        {
            // �����ǰѡ�е��Ǳ�����, ����ö�ٵ��Ĵ���Ҳ�Ǳ�������Ĵ���
            if (showWndIsVarCtl && pfn_isVarCtl(hChild))
            {
                hChild = GetWindow(hTab, GW_HWNDNEXT);
                continue;   // ���Ǳ�������ľͲ�����
            }
            
            if (__isState_ECtl(hChild))
                ShowWindowEx(hChild, SW_HIDE);  // ������Ҫ��ʾ�Ĵ���, �������������Դ��Ĵ���, ��Ҫ����

        }
        hChild = GetWindow(hChild, GW_HWNDNEXT);
    }

    ShowWindowEx(hWndShow, SW_SHOW);
}



LRESULT CALLBACK WndProc_OnStateTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage)
{
    IRE_STRUCT_TAB_SIZE evt = { 0 };
    RECT& rc = evt.rc;  // ���������ѡ��еĿͻ���, ����Ӧ��������ͻ�����
    HWND hWndShow = 0;
    LPNMHDR hdr = (LPNMHDR)lParam;
    evt.hWnd = hdr->hwndFrom;
    evt.index = TabCtrl_GetCurSel(hdr->hwndFrom); // ѡ���ѡ��ı�
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

        bool isRight = false;   // �Ƿ���ӵ��ұߵ�ѡ���, ���Ϊfalse, ����ӵ����
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

        if (Tab_GetItemParam(hWndMove) != item.lParam)  // ��ǰѡ�еĺ��л�������ѡ���һ��, ��Ҫ�����µ�ѡ����
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
    case TAB_PARAM_VARTABLE:     // ������
    {
        _tab_move_vartable(item.lParam, rc, pOlds);
        s_info->evt.pfn_Event_callback_after(arr, IRE_TAB_STATE_SIZE, &evt, evtCount, false);
        return 0;
    }
    default:
        hWndShow = Wnd_State_GetWindowFromIndex(evt.index, item.lParam);
        break;
    }

    if (item.lParam != pOlds->param)   // �л����µĴ���
    {
        for (auto it = pOlds->hWnds->begin(); it != pOlds->hWnds->end(); ++it)
            ShowWindowEx(*it, SW_HIDE);   // ���ؾɴ���
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
            __hide_vartable(true);  // �������߶�û��ѡ�б����������²����ر���������
    }
    // ���Ǳ���������λ�ö�һ��
    if (hWndShow)
        SetWindowPos(hWndShow, HWND_TOP, rc.left, rc.top, cxClient, cyClient, flags);
    s_info->evt.pfn_Event_callback_after(arr, IRE_TAB_STATE_SIZE, &evt, evtCount, false);
    return 0;
}

void EWindow_StateClick_State(LPTOPSTATE_INFO info, int id)
{
    switch (id)
    {
    case TOP_STATE_BUTTON_CLOSE:    // �رհ�ť, ��Ҫ�����������
    {
        wstr::dbg(L"�رհ�ť������\n");
        break;
    }
    case TOP_STATE_BUTTON_MAXBOX:   // ��󻯰�ť, ��Ҫ�жϵ�ǰ״̬���в���
    {

        break;
    }
    case TOP_STATE_BUTTON_PUSHPIN:  // ͼ����ť, ��Ҫ�ж��Ƿ�ѡ��Ȼ�����Ƿ񸡶�
    {
        break;
    }
    default:
        break;
    }
}

_EWINDOW_END_NAMESPACE

