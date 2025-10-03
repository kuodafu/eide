#include "EWindow_Size_Header.h"
#include <iDraw_const.h>





_EWINDOW_BEGIN_NAMESPACE
void EWindow_MoveConmboBox(HDWP& hDwp);

LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    static int s_i;
    //wstr::dbg(L"������������ �ػ����� = %d\n", ++s_i);
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
    // ÿ��һ������λ�ü�¼��, ��һ���ƶ���������һ����λ�ý����޸�

    1. ���ƶ����ϽǵĲ˵�
    2. ���ƶ����ߵĲ˵�������
    3. �ƶ��ױ�״̬��, ��¼��״̬����λ��, ���ܿ������, ״̬�ж��������λ���ƶ�, ״̬�����������Դ��ڵ���
    4. �ƶ�״̬��, ���������Դ��ں�״̬������
    5. �ƶ���ߵĹ�����
    6. �ƶ������, �����������������λ��, MDI�Ǹ������������ұߵ�
    7. �ƶ�MDI, �ڲ�������Ƿ���ʾ���������ƶ������, ���ܿ������, ������λ�ö����¼
    8. �ƶ��Ի�ѡ���
    9. �ƶ�������Ͽ�
*/
    EWindow_MoveMenu(hDwp);             // 1. ���ƶ����ϽǵĲ˵�
    EWindow_MoveMenuWindow(hDwp);       // 2. ���ƶ����ߵĲ˵�������
    if (s_info->isReady)
    {

        EWindow_MoveStatus(hDwp);           // 3. �ƶ��ױ�״̬��
        EWindow_MoveState(hDwp);            // 4. �ƶ�״̬��
        EWindow_MoveWork(hDwp);             // 5. �ƶ���ߵĹ�����
        EWindow_MoveControl(hDwp);          // 6. �ƶ������
        EWindow_MoveMDI_Tab_Cbx(hDwp);      // 7,8,9 �ƶ� MDI, �Ի�ѡ���, ������Ͽ�
        MoveWindow(s_info->hStatusBar, 0, 0, 0, 0, true);
        static HWND hWnd32770Parent = s_info->eWnd->GetControl32770Parent();
        if (!IsWindowVisible(hWnd32770Parent))
            ShowWindowEx(hWnd32770Parent, SW_SHOW);
    }

    //EndDeferWindowPos(hDwp);

    return 0;
}

// �ƶ� MDI, �Ի�ѡ���, ������Ͽ�
// �⼸�����һ�㶼�Ƕ�һ����ȫ�����ö�
// ���ƶ�MDI, ���ƶ��Ի�ѡ���, ����ƶ���Ͽ�, ����������Ǹ���MDI���㴰��λ��
void EWindow_MoveMDI_Tab_Cbx(HDWP& hDwp)
{
    EWindow_MoveMDIClient(hDwp);        // 7. �ƶ�MDI
    EWindow_MoveCustomTab(hDwp);        // 8. �ƶ��Ի�ѡ���
    EWindow_MoveConmboBox(hDwp);        // 9. �ƶ�������Ͽ�
}


void EWindow_MoveCustomTab(HDWP& hDwp)
{
    // ��� = MDI�������
    // ���� = ������ڶ�����ʾ, �Ǿ��ǲ˵����ױ�, �������MDI�ĵױ�
    // �ұ� = MDI�ұ�
    // �ױ� = ���� + �߶�, �߶ȹ̶�
    bool isTop = s_info->config->tab_code_direction() != 3;    // ����ѡ��е��Ӽз���, 3=�ױ�, ���� = ����

    s_info->rcCustomTab.left    = s_info->rcMDIClient.left;
    s_info->rcCustomTab.top     = (isTop ? s_info->rcWndMenu.bottom : s_info->rcMDIClient.bottom);
    s_info->rcCustomTab.right   = s_info->rcMDIClient.right;
    s_info->rcCustomTab.bottom  = s_info->rcCustomTab.top + s_info->scale(WINDOW_CUSTOMTAB_HEIGHT);

    InvalidateRect(s_info->hEWnd, &s_info->rcCustomTab, 0);
}

void EWindow_MoveConmboBox(HDWP& hDwp)
{
    // ��� = MDI�������
    // ���� = MDI���� - ��Ͽ�߶�
    // �ұ� = �����Ͽ� = MDI���ڿ��40%, �ұ���Ͽ�=MDI�ұ�
    // �ױ� = MDI����
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

    // ״̬��λ��, �����Ԥ���߿��λ��
    // �����Ǵ��ڿͻ����߶� - ״̬���ĸ߶� - Ԥ���߿�ĸ߶�
    // �ұ� = �����Դ��� - Ԥ���Ŀ��
    // ���ҵ� ������ͨ�������Դ��ڼ������, �����Ǹ����Ƿ���Ӽ���
    // �������, ���߾��ǵױ�-״̬���߶�
    // ���������, ���߾ͺ͵ױ�һ����, �����߶Ⱦ���0
    s_info->rcStatusBar.left    = border;
    s_info->rcStatusBar.right   = s_info->cxClient - border;
    s_info->rcStatusBar.bottom  = s_info->cyClient - border;


    s_info->rcStatusBar.top = (isVisibleStatusBar)
        ? (s_info->rcStatusBar.bottom - height)
        : (s_info->rcStatusBar.bottom);
    
    // һ���� ���ڵױ� - ״̬�и߶�-�߿�
    // һ���� ״̬�еױ� - �߿�
    // �������ֵ��һ��, �Ǿ��Ǽ��������
    assert((s_info->cyClient - height - border) == (s_info->rcStatusBar.bottom - height));
    EWindow_DeferWindowPos(hDwp, s_info->hStatusBar, s_info->rcStatusBar, true);
}


// �жϵ�ǰѡ�е��Ƿ��Ǵ��������
int EWindow_IsSelWindow()
{
    g_moveInfo.hChild1 = 0;
    g_moveInfo.hChild2 = 0;
    g_moveInfo.hChild3 = 0;
    g_moveInfo.hChild4 = 0;
    ICustomCodeTab& arr = *s_info->hWnds;
    if (arr.empty()) return -1;    // ��ԱΪ0, ��ʾ���е�ѡ�����, �϶�û����ʾ���������

    HWND hTab = s_info->hEWnd;
    const int index = TabCtrl_GetCurSel(hTab);    // ��ǰѡ���ѡ����
    if ((int)arr.size() <= index || index < 0)    // ѡ��е�ǰѡ�е�������, ���Ӧ�þ���bug��, ���԰��¸��ϵ�
    {
//#ifdef _DEBUG
//        __debugbreak();
//#endif
        return -1;
    }

    return EWindow_GetMDIClientChild(arr[index].hWnd);
}

// ��ȡ�������, ����������id�ʹ����id��ͬ, ��ȡ������ڵ���һ��ͬ������
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

// ��ȡMDI�µ�ǰ��ʾ�Ĵ���, �����Ӵ���, һ����ȡ4��
int EWindow_GetMDIClientChild(HWND hChild)
{
    if (!hChild)
        hChild = GetWindow(s_info->hMDIClient, GW_CHILD);
    if (!hChild) return -1;

    int nChild3ID = 0;
    while (1)
    {
        g_moveInfo.hChild1 = hChild;    // ��ǰMDI������ʾ�Ĵ���
        while (g_moveInfo.hChild1)
        {
            int id = GetDlgCtrlID(g_moveInfo.hChild1);
            if (id >= 65280 && id < 66000)  // ��һ�㴰���� 65280 ��ʼ
                break;
            g_moveInfo.hChild1 = GetWindow(g_moveInfo.hChild1, GW_HWNDNEXT);
        }
        if (!g_moveInfo.hChild1)
            break;

        g_moveInfo.hChild2 = GetWindow(g_moveInfo.hChild1, GW_CHILD);    // mdi�µĵڶ��㴰��
        if (!_mdi_getwindow(g_moveInfo.hChild2, 59648))
            break;


        g_moveInfo.hChild3 = GetWindow(g_moveInfo.hChild2, GW_CHILD);    // MDI�µĵ����㴰��, ������������ж���д���뻹����ʼҳ�����Ǵ��������
        if (!g_moveInfo.hChild3)break;

        while (g_moveInfo.hChild3)  // 1=�����������ʼҳ, 59648=������, ֻҪ�����������������������ȡ
        {
            // ������������ų�������
            nChild3ID = GetDlgCtrlID(g_moveInfo.hChild3);
            if (nChild3ID == 1 || nChild3ID == 59648) break;
            g_moveInfo.hChild3 = GetWindow(g_moveInfo.hChild3, GW_HWNDNEXT);
        }
        break;
    }

    int nRet = 0;
    if (nChild3ID == 1)    // mdi�µĵ����㴰��id = 1 Ҫô�Ǵ��������, Ҫô����ʼҳ, ����Ļ�id=59648
    {
        g_moveInfo.hChild4 = GetWindow(g_moveInfo.hChild3, GW_CHILD);
        // 117�Ǵ���������Ĵ���, ����117�ͷ���1, ���򷵻�2, 1=���������, 2=��ʼҳ, 0=������
        nRet = (GetDlgCtrlID(g_moveInfo.hChild4) == 117) ? 1 : 2;
    }
    else
    {
        if (g_moveInfo.hChild3 && nChild3ID == 59648)     // ����򸸴���, �޸��������Ĵ���������Ҳ��ı�
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
        _insert_menu(hMenu, IDMENU_FLOAT, L"����(&F)");
        _insert_menu(hMenu, IDMENU_STOP, L"ͣ��(&K)");
        _insert_menu(hMenu, IDMENU_AUTOHIDE, L"�Զ�����(&T)");
        _insert_menu(hMenu, IDMENU_CLOSE, L"�ر�(&C)");
    }
    g_moveInfo.pMenuClick = info;

    //bool isShow = false;    // ������������ص�, �Ǿ��Զ����ز˵�����ֹ

    //
    //if (info->hWnd == hControl)
    //    isShow = s_info->config->tab_control_isShow();
    //else if (info->hWnd = hState)
    //    isShow = s_info->config->tab_state_isShow();
    //else if (info->hWnd == hWork)
    //    isShow = s_info->config->tab_work_isShow();

    const bool isAutoHide   = __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE); // �Ƿ��Զ�����
    const bool isFloat      = __query_flags(info, TOP_STATE_BUTTON_ISFLOAT);    // �Ƿ񸡶�
    const bool isStop       = __query_flags(info, TOP_STATE_BUTTON_ISSTOP);     // �Ƿ�ͣ��


    MENUITEMINFOW mi = { 0 };
    mi.cbSize = sizeof(mi);
    mi.fMask = MIIM_STATE;
    //mi.fState = isAutoHide ? MFS_DISABLED : MFS_ENABLED;    // ��ǰ״̬���Զ����ؾͽ�ֹ�Զ����ز˵�
    //SetMenuItemInfoW(hMenu, (UINT)IDMENU_AUTOHIDE, false, &mi);

    mi.fState = isFloat ? MFS_DISABLED : MFS_ENABLED;       // ��ǰ״̬�Ǹ����ͽ�ֹ�Զ����ز˵�
    SetMenuItemInfoW(hMenu, (UINT)IDMENU_FLOAT, false, &mi);

    mi.fState = isStop ? MFS_DISABLED : MFS_ENABLED;        // ��ǰ״̬��ͣ���ͽ�ֹ�Զ����ز˵�
    SetMenuItemInfoW(hMenu, (UINT)IDMENU_STOP, false, &mi);

    POINT pt;
    GetCursorPos(&pt);
    TrackPopupMenu(g_moveInfo.hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, info->hWnd, 0);
}

_EWINDOW_END_NAMESPACE