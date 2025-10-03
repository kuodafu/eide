#include "wnd_header.h"
#include <IDraw_Header.h>

LRESULT _wnd_wm_nchittest_sizebox(const POINT& pt, int cx, int cy)
{
    LONG offset = 6;
    LRESULT ret = HTCLIENT;
    if (pt.y < offset)                              // 鼠标在顶部
    {
        if (pt.x < offset) ret = HTTOPLEFT;                 // 鼠标在左边
        else if (pt.x > cx - offset)ret = HTTOPRIGHT;       // 鼠标在右边
        else ret = HTTOP;                                   // 鼠标在顶边
    }
    else if (pt.y > cy - offset)                    // 鼠标在底部
    {
        if (pt.x < offset) ret = HTBOTTOMLEFT;              // 鼠标在左边
        else if (pt.x > cx - offset)ret = HTBOTTOMRIGHT;    // 鼠标在右边
        else ret = HTBOTTOM;                                // 鼠标在底边
    }
    else if (pt.x < offset)                         // 鼠标在左部
    {
        if (pt.y < offset) ret = HTTOPLEFT;                 // 鼠标在顶边
        else if (pt.y > cy - offset)ret = HTBOTTOMLEFT;     // 鼠标在底边
        else ret = HTLEFT;                                  // 鼠标在左边
    }
    else if (pt.x > cx - offset)                    // 鼠标在右部
    {
        if (pt.y < offset) ret = HTTOPRIGHT;                // 鼠标在顶边
        else if (pt.y > cy - offset)ret = HTBOTTOMRIGHT;    // 鼠标在底边
        else ret = HTRIGHT;                                 // 鼠标在左边
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


    ScreenToClient(pData->hObj, &pt);   // 系统按钮, 菜单条都是客户区坐标, 需要转换一下
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;   // 系统按钮的优先级需要比菜单条高
    if (sysBtn)
    {
        if (PtInRect(&sysBtn->rcBtn, pt))
            return HTCLIENT;


        if (PtInRect(&sysBtn->rcIcon, pt))
            return HTSYSMENU;        // 菜单

        if (PtInRect(&sysBtn->rcCaption, pt))
            return HTCAPTION;        // 标题栏

    }

    if (PtInRect(&s_info->rcMenu, pt))  // 菜单条
        return HTCLIENT;


    //return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    //if (s_info->isZoomed)   // 是最大化就返回客户区
    //    return HTCLIENT;

    int statusHeight = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;

    if (pt.y >= s_info->rcStatusBar.top)
        return HTCAPTION;

    if (pt.y >= s_info->rcMDIClient.top)
        return HTCLIENT;


    // 标准工具条右边剩余的宽度
    RECT rcCaption = { s_info->rcWndMenu.right, s_info->rcWndMenu.top, s_info->rcMDIClient.right, s_info->rcWndMenu.bottom };
    //LPOBJSTRUCT pControl = s_info->pfnGetDataFromHwnd(s_info->hControl);
    //LPTOPSTATE_INFO info = (LPTOPSTATE_INFO)pControl->param;
    //if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
    //    rcCaption.right -= ;
    if (PtInRect(&rcCaption, pt))
        return HTCAPTION;


    if (!s_info->tab.isUpdown)  // 没有左右调节器才判断鼠标在自绘选择夹中是否在选项上
    {
        int width = 0;
        ICustomCodeTab& arr = *s_info->hWnds;
        int count = arr.size();
        for (int i = s_info->tab.indexFirst; i < count; i++)
            width += s_info->scale(arr[i].width);
        // 这里是判断鼠标是否在最后一个选项卡的右边
        // 如果有调节器的话就不需要判断
        rcCaption = { s_info->rcCustomTab.left + width, s_info->rcCustomTab.top, s_info->rcCustomTab.right, s_info->rcCustomTab.bottom };
        if (PtInRect(&rcCaption, pt))
            return HTCAPTION;
    }




    // x>边框<右边-边框, y
    bool isTitle = pt.x > WINDOW_BORDER_WIDTH && pt.x < (int)pData->pos.width - WINDOW_BORDER_WIDTH
        && pt.y > 30 && pt.y < ((int)pData->pos.height - statusHeight);
    if (isTitle )
        return HTCLIENT;
    return HTCAPTION;
}
