#include "../EWindow_Fne_Header.h"


LRESULT CALLBACK WndProc_MDIClient_EWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_MDIClient_WTWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_MDIClient_EWindow_Child2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// MDIClient�µĴ��ڱ༭��, MDIClient�µĵڶ����Ӵ���, pData�ǵ�һ���Ӵ��ڵ�����
LPOBJSTRUCT _subclass_MDIClient_EWindow(HWND hWnd, LPOBJSTRUCT pData, MDICLIENT_ALL_WINDOW* mdi)
{
    hWnd = GetWindow(hWnd, GW_CHILD);   // MDIClient�µĵڶ����Ӵ���
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_MDIClient_EWindow_Child2, true);
    if (!data)return 0;

    hWnd = GetWindow(hWnd, GW_CHILD);
    LPOBJSTRUCT ewindow = _subclass(hWnd, WndProc_MDIClient_EWindow, true);    // ���������, ��һ�㴰�ھ����������Ĵ�����
    ewindow->draw.color.crBackGround = RGB(37, 37, 38);
    data->draw.color.hbrBackGround = s_info->pfnCreateBrush(data->draw.color.crBackGround);

    if (s_info->MY_MF_OWNERDRAW)
    {
        ewindow->drawScroll = ScrollCreate(ewindow->hWnd, ewindow->draw.color.crBorder,
                                           0,          // m_isSysListView, ����ǳ����б��Ļ����ֵ���ǳ����б��ı���߶�
                                           false,      // m_isSysTreeView, �Ƿ�Ϊ���Ϳ�, ���Ϊ��, �������ù�����λ��, ���͹�����Ϣ, Ϊ�����ȷ��͹�����Ϣ������λ��
                                           true,       // m_isCallSetScroll, �Ƿ����ԭ�������ù�����λ�ú���
                                           false,      // isSubClass, �Ƿ����໯, ���Ϊtrue, �ڲ������໯�������, ���Ϊ��, ������ⲿ����Ϣ���ݵ� OnWndProc ������
                                           true        // isReCalcSize, �Ƿ����¼���ߴ�, Ϊtrue���ڲ������ SetWindowPos �������¼���ǿͻ���
        );
    }
    InsertStyle(ewindow->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    hWnd = GetWindow(hWnd, GW_CHILD);
    data = _subclass(hWnd, WndProc_MDIClient_WTWindow, false, false, 0);    // �������Ĵ���


    return data;
}


LRESULT CALLBACK WndProc_MDIClient_EWindow_Child2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    switch (message)
    {
    case WM_PAINT:
        return WndProc_DefPaint(hWnd, message, wParam, lParam, pData);
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

// �����������Ϣѭ��
LRESULT CALLBACK WndProc_MDIClient_EWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
    if (!pObj || s_info->MY_MF_OWNERDRAW == 0)
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    LRESULT ret = 0;
    
    switch (message)
    {
    case WM_NCHITTEST:
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCCALCSIZE:
    case WM_NCPAINT:
    case WM_PAINT:
    case WM_CAPTURECHANGED:
    case WM_STYLECHANGING:
    case WM_STYLECHANGED:
    case WM_HSCROLL:
    case WM_VSCROLL:
    case WM_MOUSEHWHEEL:    // �������
    case WM_MOUSEWHEEL:     // �������
    case SBM_SETSCROLLINFO:
    case SBM_SETRANGE:
    case SBM_SETPOS:
    {
        if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
        {
            return ret;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_DESTROY:
    {
        delete pData->drawScroll;
        pData->drawScroll = 0;
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

LRESULT CALLBACK WndProc_MDIClient_WTWindow(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_NCHITTEST:
        return HTCLIENT;
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

