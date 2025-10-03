#include "EWindow_Size_Header.h"
#include <iDraw_const.h>

_EWINDOW_BEGIN_NAMESPACE

void EWindow_Size_OnMenuComboBox();
// �ƶ�MDIClient�µ��Ӵ���, ����ǰ�����Ѿ������MDIClient��λ��, �Ӵ��ڶ��Ǹ������λ�ý����ƶ�
void EWindow_MoveMDIClientChild(HDWP& hDwp);
// �ƶ���ʼҳ
void EWindow_MoveMDIClientEPage(HDWP& hDwp, RECT& rcChild3);

void EWindow_MoveMDIClient(HDWP& hDwp)
{
    if (!s_info->isReady)
        return;

    if (!IsWindowVisible(s_info->hMDIClient))
        ShowWindowEx(s_info->hMDIClient, SW_SHOW);
    // ��� = �������ұ�
    // ���� = �˵����ױ� + [��Ͽ�߶�]  �����л�ѡ����ڶ�����Ҫ����ѡ��еĸ߶�
    // �ұ� = ���������
    // �ױ� = �����еױ�   ����ѡ����ڵױ���Ҫ��ȥѡ��еĸ߶�

    bool isTop = s_info->config->tab_code_direction() != 3;             // ����ѡ��е��Ӽз���, 3=�ױ�, ���� = ����
    const bool isAutohideWork       = IsAutoHide(s_info->hWork);        // �������Ƿ����Զ�����
    const bool isAutohideState      = IsAutoHide(s_info->hState);       // ״̬���Ƿ����Զ�����
    const bool isAutohideControl    = IsAutoHide(s_info->hControl);     // ״̬���Ƿ����Զ�����
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
            rcMDIClient.bottom = s_info->rcState.top;   // ����ʾ��λ�������Ļ��ױ߾��ǹ����еĵױ�
        else
            rcMDIClient.top = s_info->rcWndMenu.bottom;
        // ���ߵĻ������Ի�ѡ��еĶ���
        if (isTop && isShow)
            rcMDIClient.top -= s_info->scale(g_moveInfo.heightCombobox);
    }

    if (isAutohideWork)     // ���������Զ����صĻ�, mdi����߾��ǹ̶���
        rcMDIClient.left = s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH * 2);
    if (isAutohideState)    // ״̬�����Զ����صĻ�, mdi�ĵױ��ǹ̶���
        rcMDIClient.bottom = s_info->rcStatusBar.top - s_info->scale(WINDOW_AUTO_HEIGHT) - (isTop ? 0 : s_info->scale(WINDOW_CUSTOMTAB_HEIGHT));
    if (isAutohideControl)  // ��������Զ����صĻ�, mdi���ұ��ǹ̶���
        rcMDIClient.right = s_info->cxClient - (s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH * 2));

    EWindow_DeferWindowPos(hDwp, s_info->hMDIClient, rcMDIClient, true);

    //RECT& rc = rcMDIClient;
    //SetWindowPos(s_info->hMDIClient, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DRAWFRAME);

    //EWindow_MoveMDIClientChild(hDwp);
}

// �ƶ�MDIClient�µ��Ӵ���, ����ǰ�����Ѿ������MDIClient��λ��, �Ӵ��ڶ��Ǹ������λ�ý����ƶ�
void EWindow_MoveMDIClientChild(HDWP& hDwp)
{
    const int cxClient = s_info->rcMDIClient.right - s_info->rcMDIClient.left;
    const int cyClient = s_info->rcMDIClient.bottom - s_info->rcMDIClient.top;

    IRE_STRUCT_MDICLIENT_MOVE_EVENT evt;
    evt.rcChild1 = { 0, 0, cxClient, cyClient }; // MDI�Ӵ��ڵ�λ��
    evt.rcChild2 = { 0, 0, cxClient, cyClient }; // MDI�Ӵ��ڵ�λ��
    evt.rcChild3 = { 0, 0, cxClient, cyClient }; // MDI�Ӵ��ڵ�λ��
    evt.rcChild4 = { 0, 0, cxClient, cyClient }; // MDI�Ӵ��ڵ�λ��
    evt.hChild1 = g_moveInfo.hChild1;
    evt.hChild2 = g_moveInfo.hChild2;
    evt.hChild3 = g_moveInfo.hChild3;
    evt.hChild4 = g_moveInfo.hChild4;
    evt.windowType = MAKELONG(g_moveInfo.windowType, g_moveInfo.code_type);
    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
    int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_TAB_MDICLIENT_SIZE, &evt, &isReturn, false);
    if (isReturn)
        return;     // ���������λ����Ϣ, ��������

    EWindow_DeferWindowPos(hDwp, g_moveInfo.hChild1, evt.rcChild1, true);     // MDI�µĵ�һ�㴰��
    EWindow_DeferWindowPos(hDwp, g_moveInfo.hChild2, evt.rcChild2, true);     // MDI�µĵڶ��㴰��
    EWindow_DeferWindowPos(hDwp, g_moveInfo.hChild3, evt.rcChild3, true);

    if (g_moveInfo.windowType == 2)  // 2=��ʼҳ
    {
        EWindow_MoveMDIClientEPage(hDwp, evt.rcChild3);
    }
    else if (g_moveInfo.windowType == 1)    // ���������
    {
        MoveWindowEx(g_moveInfo.hChild3, evt.rcChild3, true);    // �����������
    }
    else if (g_moveInfo.hChild4)    // ������
    {
        MoveWindowEx(g_moveInfo.hChild4, evt.rcChild4, true);    // ������������
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
        if (i >= sizeof(rcChild4) / sizeof(rcChild4[0])) break;    // ��ֹԽ��
        EWindow_DeferWindowPos(hDwp, hChild4, rcChild4[i++], TRUE);
        hChild4 = GetWindow(hChild4, GW_HWNDNEXT);
    }
}

// �м������MDI���ڱ��ı�, ������Ҫ�޸Ĳ��ٶ���
// ˫��������������ᴥ����ǰ�¼�
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
    //wstr::dbg(L"MDIClient �ػ����� = %d, ��ǰѡ���� = %d\n", ++s_i, ((int)s_info->hWnds->size()));
    if (!s_info->hWnds || s_info->hWnds->size() == 0) return 0;
    if (!IsWindowVisible(hWnd))return 0;
    auto& arr = *s_info->hWnds;
    int index = TabCtrl_GetCurSel(s_info->hEWnd);
    if (index == -1)
        return 0;
    HWND hWndShow = arr[index].hWnd;

    HDWP hDwp = 0;
    EWindow_IsSelWindow();
    //wstr::dbg(L"error ��ǰ�������� = %d, �������� = %d\n", g_moveInfo.windowType, g_moveInfo.code_type);
    if (g_moveInfo.windowType == -1) return 0;
    
    ShowWindowEx(hWndShow, SW_SHOW);


    RECT& rcMDIClient = s_info->rcMDIClient;
    RECT& rcControl = s_info->rcControl;
    if (rcMDIClient.right != s_info->cxClient - WINDOW_BORDER_WIDTH
        && rcControl.right - rcControl.left <= 0)
    {
        // MDI���ұ߲����������Դ��ڵ��ұ�, ���������Ϊ0, �Ǿ���Ҫ����������λ��
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

// �ı䶨λ����������Ͽ�λ��
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

