#include "wnd_header.h"
#include <IDraw_Header.h>

LRESULT _wnd_wm_nchittest_sizebox(const POINT& pt, int cx, int cy)
{
    LONG offset = 6;
    LRESULT ret = HTCLIENT;
    if (pt.y < offset)                            // 鼠标在顶部
    {
        if (pt.x < offset) ret = HTTOPLEFT;                // 鼠标在左边
        else if (pt.x > cx - offset)ret = HTTOPRIGHT;    // 鼠标在右边
        else ret = HTTOP;                                // 鼠标在顶边
    }
    else if (pt.y > cy - offset)                    // 鼠标在底部
    {
        if (pt.x < offset) ret = HTBOTTOMLEFT;            // 鼠标在左边
        else if (pt.x > cx - offset)ret = HTBOTTOMRIGHT;// 鼠标在右边
        else ret = HTBOTTOM;                            // 鼠标在底边
    }
    else if (pt.x < offset)                        // 鼠标在左部
    {
        if (pt.y < offset) ret = HTTOPLEFT;                // 鼠标在顶边
        else if (pt.y > cy - offset)ret = HTBOTTOMLEFT;    // 鼠标在底边
        else ret = HTLEFT;                                // 鼠标在左边
    }
    else if (pt.x > cx - offset)                // 鼠标在右部
    {
        if (pt.y < offset) ret = HTTOPRIGHT;                // 鼠标在顶边
        else if (pt.y > cy - offset)ret = HTBOTTOMRIGHT;    // 鼠标在底边
        else ret = HTRIGHT;                                    // 鼠标在左边
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
        // 这里是右下角,从窗口右边减去设置的值范围内都算是右下角
        rc = pData->pos.rc;
        rc.left = pData->pos.rc.right - offset;    // 计算鼠标到那个矩形内,窗口右边底边 - 设置的值,就是右下角
        rc.top = pData->pos.rc.bottom - offset;
        if (PtInRect(&rc, pt))
            return HTBOTTOMRIGHT;    // 右下角

        rc = pData->pos.rc;
        rc.top = pData->pos.rc.bottom - offset;
        rc.right = pData->pos.rc.left + offset;
        if (PtInRect(&rc, pt))
            return HTBOTTOMLEFT;    // 左下角

        rc = pData->pos.rc;
        rc.right = pData->pos.rc.left + offset;
        rc.bottom = pData->pos.rc.top + offset;
        if (PtInRect(&rc, pt))
            return HTTOPLEFT;        // 左上角

        rc = pData->pos.rc;
        rc.left = pData->pos.rc.right - offset;
        rc.bottom = pData->pos.rc.top + offset;
        if (PtInRect(&rc, pt))
            return HTTOPRIGHT;        // 右上角


        rc = pData->pos.rc;
        rc.right = pData->pos.rc.left + offset;
        rc.top = pData->pos.rc.top + offset;
        rc.bottom = pData->pos.rc.bottom - offset;
        if (PtInRect(&rc, pt))
            return HTLEFT;            // 左边

        rc = pData->pos.rc;
        rc.bottom = pData->pos.rc.top + offset;
        if (PtInRect(&rc, pt))
            return HTTOP;            // 顶边

        rc = pData->pos.rc;
        rc.left = pData->pos.rc.right - offset;
        if (PtInRect(&rc, pt))
            return HTRIGHT;            // 右边

        rc = pData->pos.rc;
        rc.top = pData->pos.rc.bottom - offset;
        if (PtInRect(&rc, pt))
            return HTBOTTOM;            // 底边
    }



    //rcTmp = rc;
    //rcTmp.bottom = rc.top + 30;
    //rcTmp.left = rcTmp.right - 30;
    //if (PtInRect(&rcTmp, pt))
    //    return HTCLOSE;        // 关闭按钮

    ScreenToClient(pData->hObj, &pt);   // 菜单条
    if (PtInRect(&s_info->rcMenu, pt))
        return HTCLIENT;


    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    if (sysBtn)
    {
        if (PtInRect(&sysBtn->rcBtn, pt))
            return HTCLIENT;


        if (PtInRect(&sysBtn->rcIcon, pt))
            return HTSYSMENU;        // 菜单

        if (PtInRect(&sysBtn->rcCaption, pt))
            return HTCAPTION;        // 标题栏
        
    }
    //return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
    return HTCAPTION;
    return HTCLIENT;
}
