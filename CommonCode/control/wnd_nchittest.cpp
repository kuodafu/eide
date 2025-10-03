#include "wnd_header.h"
#include <IDraw_Header.h>

LRESULT _wnd_wm_nchittest_sizebox(const POINT& pt, int cx, int cy)
{
    LONG offset = 6;
    LRESULT ret = HTCLIENT;
    if (pt.y < offset)                            // ����ڶ���
    {
        if (pt.x < offset) ret = HTTOPLEFT;                // ��������
        else if (pt.x > cx - offset)ret = HTTOPRIGHT;    // ������ұ�
        else ret = HTTOP;                                // ����ڶ���
    }
    else if (pt.y > cy - offset)                    // ����ڵײ�
    {
        if (pt.x < offset) ret = HTBOTTOMLEFT;            // ��������
        else if (pt.x > cx - offset)ret = HTBOTTOMRIGHT;// ������ұ�
        else ret = HTBOTTOM;                            // ����ڵױ�
    }
    else if (pt.x < offset)                        // �������
    {
        if (pt.y < offset) ret = HTTOPLEFT;                // ����ڶ���
        else if (pt.y > cy - offset)ret = HTBOTTOMLEFT;    // ����ڵױ�
        else ret = HTLEFT;                                // ��������
    }
    else if (pt.x > cx - offset)                // ������Ҳ�
    {
        if (pt.y < offset) ret = HTTOPRIGHT;                // ����ڶ���
        else if (pt.y > cy - offset)ret = HTBOTTOMRIGHT;    // ����ڵױ�
        else ret = HTRIGHT;                                    // ��������
    }
    return ret;
}
LRESULT WndProc_OnNcHittest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    //pt.x -= pData->rc.left;
    //pt.y -= pData->rc.top;
    //int cx = pData->rc.right - pData->rc.left;
    //int cy = pData->rc.bottom - pData->rc.top;
    //return _wnd_wm_nchittest_sizebox(pt, cx, cy);
    if (!IsZoomed(hWnd))
    {
        const int offset = 5;

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

    ScreenToClient(pData->hObj, &pt);   // �˵���
    if (PtInRect(&s_info->rcMenu, pt))
        return HTCLIENT;


    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    if (sysBtn)
    {
        if (PtInRect(&sysBtn->rcBtn, pt))
            return HTCLIENT;


        if (PtInRect(&sysBtn->rcIcon, pt))
            return HTSYSMENU;        // �˵�

        if (PtInRect(&sysBtn->rcCaption, pt))
            return HTCAPTION;        // ������
        
    }
    //return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
    return HTCAPTION;
    return HTCLIENT;
}
