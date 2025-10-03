#include "EWindow_Size_Header.h"
#include <iDraw_const.h>

_EWINDOW_BEGIN_NAMESPACE
static bool m_isNoMove_work;
static bool m_move_end_work = false;     // �Ѿ�����ƶ���

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

    RECT& rcWork = _get_rect_work();    // �����Ƿ��Զ�����ʹ�ò�ͬ�Ľṹ
    if (rcWork.right == 0 || rcWork.bottom == 0)
        GetClientRect(hWnd, &rcWork);
    const int cxChild = rcWork.right - rcWork.left;
    const int cyChild = rcWork.bottom - rcWork.top;
    const int cyStatus = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;
    bool isDrawFrame = false;


    // �������Ǹ���״̬�кͲ˵��е����ߴ��
    // �����е�λ��, ���Ԥ���߿���
    // �����ǲ˵����ĵױ�, Ԥ��һС��λ�ý��е����ߴ�
    // ��ȿ����ԭ����ֵ
    // �ױ���״̬�еĶ���
    rcWork.left     = s_info->scale(WINDOW_BORDER_WIDTH);
    rcWork.top      = s_info->rcWndMenu.bottom;
    rcWork.right    = rcWork.left + cxChild;
    rcWork.bottom   = IsAutoHide(s_info->hState)  // ״̬�����Զ�����״̬, �����еĵױ߾��Ǿ���λ��
        //? (s_info->cyClient - s_info->scale(WINDOW_SPACING + WINDOW_AUTO_HEIGHT + WINDOW_BORDER_WIDTH + cyStatus))
        ? s_info->rcStatusBar.top - s_info->scale(WINDOW_AUTO_HEIGHT)
        : s_info->rcState.top;



    const int widthControl = s_info->rcControl.right - s_info->rcControl.left;
    // MDI��С��� + �������
    const int offset = WINDOW_MIN_MDIWIDTH + widthControl;

    // ��������е��ұ� + MDI + �������ڴ����ұ�
    // �ǹ����е��ұ� = �����ұ� - MDI - �����
    if ((rcWork.right + offset) > s_info->cxClient)
        rcWork.right = s_info->cxClient - offset;
    if (rcWork.right - rcWork.left < WINDOW_MINTRACKSIZE)
        rcWork.right = rcWork.left + WINDOW_MINTRACKSIZE;

    if (isAutoHide)
    {
        if (s_info->iDraw->TopState_GetActivateWindow() == hWnd)
        {
            // �ߵ������һ�����Զ�����, �����ǵ�ǰ����Ĵ���, �Ǿ�����ʾ������, ��Ҫ������ʾ��λ��
            // �����Ҫ���ҵ�, ����Ѿ�����һ���ֻ滭�Ի�ѡ�����
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
        // �����Զ�����, ��������λ��
        EWindow_DeferWindowPos(hDwp, hWnd, rcWork, false);
        isDrawFrame = true;

    }

    if (isDrawFrame)
    {
        const DWORD flags = SWP_NOZORDER | SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOCOPYBITS;
        SetWindowPos(hWnd, 0, 0, 0, 0, 0, flags);
    }

}

// ��߹����гߴ类�ı�, ��Ҫ�޸Ĺ�����������λ��
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnWorkSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    static int s_i;
    m_move_end_work = true;
    //wstr::dbg(L"������ �ػ����� = %d\n", ++s_i);
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
            if (!isAutoHide) // �����Զ����صĲ��ƶ�
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


    // ��������ᴥ��ѡ��ı�, �����Ի�Ѵ�����ʾ����, ����ֱ�ӵ����Լ��Ĵ�����
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
    RECT& rc = evt.rc;  // ���������ѡ��еĿͻ���, ����Ӧ��������ͻ�����
    HWND hWndShow = 0;
    LPNMHDR hdr = (LPNMHDR)lParam;
    HWND hOld = (HWND)GetPropW(hdr->hwndFrom, L"oldWnd");
    evt.index = TabCtrl_GetCurSel(hdr->hwndFrom); // ѡ���ѡ��ı�
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
        // �Զ�����, �ҵ�ǰ����ʾ״̬
        //GetClientRect(s_info->eWnd->GetWorkTab(), &rc);
        rc = pData->pos.rcClient;
        rc.left = 1;
        rc.top = 1; 
        rc.right -= 1;
        rc.bottom -= 1;
    }

    LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
    const int flags = SWP_NOZORDER;
    if (item.lParam == TAB_PARAM_PROPER) // ���Ե�������
    {
        // ��Ҫ��ʾ������Ͽ�����Կ�
        hWndShow = s_info->eWnd->GetWorkProperty();
        if (hWndShow != hOld)   // ���ؾɴ���, ��¼��ǰ���µĴ���
        {
            //if (hOld)ShowWindowEx(hOld, SW_HIDE);
            SetPropW(hdr->hwndFrom, L"oldWnd", hWndShow);
        }
        HWND hWndComboboxControl = s_info->eWnd->GetWorkControlComboBox();
        HWND hWndComboboxEvent = s_info->eWnd->GetWorkEventComboBox();
        switch (evt.direction)
        {
        case 0:     // ���
        {
            rc.left--;
            rc.top = 0;
            rc.right++;
            rc.bottom++;
            break;
        }
        case 1:     // ����
        {
            rc.left = 0;
            rc.top--;
            rc.right++;
            rc.bottom++;
            break;
        }
        case 2:     // �ұ�
        {
            rc.left = 0;
            rc.top = 0;
            rc.right++;
            rc.bottom++;
            break;
        }
        default:    // �ױ�
        {
            rc.left = 0;
            rc.top = 0;
            rc.right++;
            rc.bottom++;
            break;
        }
        }
        // �����switch�Ǽ������Կ���Ҫ��λ��, ����һ�д����Ǵ����ߴ�ı��¼�, ���û��Գߴ���ж��δ���
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

        SetWindowPos(hWndComboboxControl, 0, rc.left, rc.top, rc.right - rc.left, comListHeight, flags);    // ������Ͽ�������ƶ�
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
    case TAB_PARAM_FNE: // ֧�ֿ�
    {
        hWndShow = s_info->eWnd->GetWorkFneTree();
        break;
    }
    case TAB_PARAM_PROGRAM: // ����
    {
        hWndShow = s_info->eWnd->GetWorkProgramTree();
        break;
    }
    case TAB_PARAM_PROPER: // ����
    {
        break;
    }
    default:
        break;
    }

    ShowWindowEx(s_info->eWnd->GetWorkControlComboBox(), SW_HIDE);
    ShowWindowEx(s_info->eWnd->GetWorkEventComboBox(), SW_HIDE);
    if (hWndShow != hOld)   // �л����µĴ���
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
    case TOP_STATE_BUTTON_CLOSE:    // �رհ�ť, ��Ҫ�����������
    {

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

