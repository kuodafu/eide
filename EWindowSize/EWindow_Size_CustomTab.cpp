#include "EWindow_Size_Header.h"

_EWINDOW_BEGIN_NAMESPACE

// �Ի��ѡ���
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnCustomTab(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
	return 0;
}

LRESULT CALLBACK WndProc_OnCustomTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage)
{
    HDWP hDwp = 0;
    LPTABCHANGE_INFO hdr = (LPTABCHANGE_INFO)lParam;
    const int index = hdr->index;   // ѡ���ѡ��ı�
    auto& arr = *s_info->hWnds;
    const int count = (int)arr.size();
    if (count == 0) return 0;
    //assert(index >= 0 && index < count);
    //HWND hWndShow = arr[index].hWnd;
    //EWindow_GetMDIClientChild(hWndShow);
    EWindow_MoveControl(hDwp);
    LPOBJSTRUCT pControl = s_info->pfnGetDataFromHwnd(s_info->hControl);
    if (!pControl)return 0;
    LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pControl->param;

    const int calcMdiRight = (g_moveInfo.windowType == 1)   // ���������MDI�ұ�, ��Ҫ�����Ƿ���ʾ�����������
        ? (s_info->rcControl.left)                          // �������, MDI��λ�������������
        : (s_info->cxClient - WINDOW_BORDER_WIDTH);         // û�������, MDI��λ���Ǵ��ڵ��ұ� - �߿���


    if (info && __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
    {
        // ��������Զ����صĻ�, ÿ���л�������mdi���Ӵ���
        EWindow_MoveMDIClientChild(hDwp);
    }
    else if (s_info->rcMDIClient.right != calcMdiRight)
    {
        // MDI��λ��������, ��Ҫ����MDI��ѡ��к�������Ͽ�
        EWindow_MoveMDI_Tab_Cbx(hDwp);
    }
    else
    {
        EWindow_MoveMDIClientChild(hDwp);
    }
    LPOBJSTRUCT pParent = s_info->pfnGetDataFromHwnd(s_info->hMDIClient);
    if (pParent)
    {
        enum TAB_SEL_MODE : int
        {
            TAB_SEL_MODE_CODE = 0,    // ����ѡ��
            TAB_SEL_MODE_LBUTTON = 1,    // ���������ѡ��
            TAB_SEL_MODE_RBUTTON = 2,    // ����Ҽ����ѡ��
            TAB_SEL_MODE_MBUTTON = 3,    // ����м����ѡ��
        };

        // ��Ҫ�������Դ���MDI��������¼�, ��Ȼ�����ᱻ����
        // ����ͨ��������Ϣ�ķ�ʽȥ����, ������Ϣ�Ļ��ڲ��ᴦ��
        // �ڲ������ֻ�����������, ������޵ݹ�....

        //CallWindowProcA(pParent->oldProc, pParent->hWnd, WM_MDIACTIVATE, (WPARAM)g_moveInfo.hChild1, (WPARAM)g_moveInfo.hChild1);
        //ShowWindowEx(s_info->eWnd->GetControl32770Parent(), SW_SHOW);
    }
    ShowWindowEx(g_moveInfo.hChild1, SW_SHOW);

    return 0;
}

_EWINDOW_END_NAMESPACE