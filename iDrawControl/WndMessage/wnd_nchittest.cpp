#include "wnd_header.h"
#include <IDraw_Header.h>

LRESULT _wnd_wm_nchittest_sizebox(const POINT& pt, int cx, int cy)
{
    LONG offset = 6;
    LRESULT ret = HTCLIENT;
    if (pt.y < offset)                              // ����ڶ���
    {
        if (pt.x < offset) ret = HTTOPLEFT;                 // ��������
        else if (pt.x > cx - offset)ret = HTTOPRIGHT;       // ������ұ�
        else ret = HTTOP;                                   // ����ڶ���
    }
    else if (pt.y > cy - offset)                    // ����ڵײ�
    {
        if (pt.x < offset) ret = HTBOTTOMLEFT;              // ��������
        else if (pt.x > cx - offset)ret = HTBOTTOMRIGHT;    // ������ұ�
        else ret = HTBOTTOM;                                // ����ڵױ�
    }
    else if (pt.x < offset)                         // �������
    {
        if (pt.y < offset) ret = HTTOPLEFT;                 // ����ڶ���
        else if (pt.y > cy - offset)ret = HTBOTTOMLEFT;     // ����ڵױ�
        else ret = HTLEFT;                                  // ��������
    }
    else if (pt.x > cx - offset)                    // ������Ҳ�
    {
        if (pt.y < offset) ret = HTTOPRIGHT;                // ����ڶ���
        else if (pt.y > cy - offset)ret = HTBOTTOMRIGHT;    // ����ڵױ�
        else ret = HTRIGHT;                                 // ��������
    }
    return ret;
}
LRESULT WndProc_OnNcHittest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    if (!s_info->isReady)
        return HTCAPTION;
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    //pt.x -= pData->rc.left;
    //pt.y -= pData->rc.top;
    //int cx = pData->rc.right - pData->rc.left;
    //int cy = pData->rc.bottom - pData->rc.top;
    //return _wnd_wm_nchittest_sizebox(pt, cx, cy);
    if (!s_info->isZoomed)
    {
        const int offset = s_info->scale(5);

        RECT rc;
        // ���������½�,�Ӵ����ұ߼�ȥ���õ�ֵ��Χ�ڶ��������½�
        rc = pData->pos.rc;
        rc.left = pData->pos.rc.right - offset;    // ������굽�Ǹ�������,�����ұߵױ� - ���õ�ֵ,�������½�
        rc.top = pData->pos.rc.bottom - offset;
        if (PtInRect(&rc, pt))
            return HTBOTTOMRIGHT;    // ���½�

        rc = pData->pos.rc;
        rc.top = pData->pos.rc.bottom - offset;
        rc.right = pData->pos.rc.left + offset;
        if (PtInRect(&rc, pt))
            return HTBOTTOMLEFT;    // ���½�

        rc = pData->pos.rc;
        rc.right = pData->pos.rc.left + offset;
        rc.bottom = pData->pos.rc.top + offset;
        if (PtInRect(&rc, pt))
            return HTTOPLEFT;        // ���Ͻ�

        rc = pData->pos.rc;
        rc.left = pData->pos.rc.right - offset;
        rc.bottom = pData->pos.rc.top + offset;
        if (PtInRect(&rc, pt))
            return HTTOPRIGHT;        // ���Ͻ�


        rc = pData->pos.rc;
        rc.right = pData->pos.rc.left + offset;
        rc.top = pData->pos.rc.top + offset;
        rc.bottom = pData->pos.rc.bottom - offset;
        if (PtInRect(&rc, pt))
            return HTLEFT;            // ���

        rc = pData->pos.rc;
        rc.bottom = pData->pos.rc.top + offset;
        if (PtInRect(&rc, pt))
            return HTTOP;            // ����

        rc = pData->pos.rc;
        rc.left = pData->pos.rc.right - offset;
        if (PtInRect(&rc, pt))
            return HTRIGHT;            // �ұ�

        rc = pData->pos.rc;
        rc.top = pData->pos.rc.bottom - offset;
        if (PtInRect(&rc, pt))
            return HTBOTTOM;            // �ױ�
    }



    //rcTmp = rc;
    //rcTmp.bottom = rc.top + 30;
    //rcTmp.left = rcTmp.right - 30;
    //if (PtInRect(&rcTmp, pt))
    //    return HTCLOSE;        // �رհ�ť


    ScreenToClient(pData->hObj, &pt);   // ϵͳ��ť, �˵������ǿͻ�������, ��Ҫת��һ��
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;   // ϵͳ��ť�����ȼ���Ҫ�Ȳ˵�����
    if (sysBtn)
    {
        if (PtInRect(&sysBtn->rcBtn, pt))
            return HTCLIENT;


        if (PtInRect(&sysBtn->rcIcon, pt))
            return HTSYSMENU;        // �˵�

        if (PtInRect(&sysBtn->rcCaption, pt))
            return HTCAPTION;        // ������

    }

    if (PtInRect(&s_info->rcMenu, pt))  // �˵���
        return HTCLIENT;


    //return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //if (s_info->isZoomed)   // ����󻯾ͷ��ؿͻ���
    //    return HTCLIENT;

    int statusHeight = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;

    if (pt.y >= s_info->rcStatusBar.top)
        return HTCAPTION;

    if (pt.y >= s_info->rcMDIClient.top)
        return HTCLIENT;


    // ��׼�������ұ�ʣ��Ŀ��
    RECT rcCaption = { s_info->rcWndMenu.right, s_info->rcWndMenu.top, s_info->rcMDIClient.right, s_info->rcWndMenu.bottom };
    //LPOBJSTRUCT pControl = s_info->pfnGetDataFromHwnd(s_info->hControl);
    //LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pControl->param;
    //if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
    //    rcCaption.right -= ;
    if (PtInRect(&rcCaption, pt))
        return HTCAPTION;


    if (!s_info->tab.isUpdown)  // û�����ҵ��������ж�������Ի�ѡ������Ƿ���ѡ����
    {
        int width = 0;
        ICustomCodeTab& arr = *s_info->hWnds;
        int count = arr.size();
        for (int i = s_info->tab.indexFirst; i < count; i++)
            width += s_info->scale(arr[i].width);
        // �������ж�����Ƿ������һ��ѡ����ұ�
        // ����е������Ļ��Ͳ���Ҫ�ж�
        rcCaption = { s_info->rcCustomTab.left + width, s_info->rcCustomTab.top, s_info->rcCustomTab.right, s_info->rcCustomTab.bottom };
        if (PtInRect(&rcCaption, pt))
            return HTCAPTION;
    }




    // x>�߿�<�ұ�-�߿�, y
    bool isTitle = pt.x > WINDOW_BORDER_WIDTH && pt.x < (int)pData->pos.width - WINDOW_BORDER_WIDTH
        && pt.y > 30 && pt.y < ((int)pData->pos.height - statusHeight);
    if (isTitle )
        return HTCLIENT;
    return HTCAPTION;
}
