#include "EWindow_Size_Header.h"

_EWINDOW_BEGIN_NAMESPACE

static HWND hStandard, hAlign;
static RECT rcToolInterface;    // 接口工具条位置

// 获取标准工具条的位置
inline void _get_standard_rect()
{
    if (!hStandard)
    {
        hStandard = s_info->eWnd->GetMenuStandard();
        hAlign = s_info->eWnd->GetMenuAlign();
    }

    RECT& rcStandard = s_info->rcStandard;

    int len = SendMessageW(hStandard, TB_BUTTONCOUNT, 0, 0);                // 标准工具条数量
    SendMessageW(hStandard, TB_GETITEMRECT, len - 1, (LPARAM)&rcStandard);  // 取最后一个按钮的右边
    const int height = rcStandard.bottom - rcStandard.top;
    const int width  = rcStandard.right  - rcStandard.left;
    rcStandard.left = 0;
    rcStandard.top = 0;

    len = SendMessageW(s_info->hWndTool, TB_BUTTONCOUNT, 0, 0);             // 接口工具条按钮数量


    RECT& rcMenu = s_info->rcWndMenu;    // 菜单窗口的位置
    rcMenu.left = s_info->scale(WINDOW_BORDER_WIDTH);
    rcMenu.top = s_info->scale(WINDOW_CAPTION_HEIGHT + 5);
    rcMenu.right = rcMenu.left + rcStandard.right;
    rcMenu.bottom = rcMenu.top + (rcStandard.bottom - rcStandard.top) + WINDOW_BORDER_WIDTH;

    rcToolInterface.top = rcMenu.top;
    rcToolInterface.left = rcMenu.right;
    rcToolInterface.right = rcMenu.right + (len * width);
    rcToolInterface.bottom = rcToolInterface.top + (rcStandard.bottom - rcStandard.top);

}
// 获取对齐工具条位置, 这个函数也会计算菜单栏的位置
//inline void _get_align_rect()
//{
//    if (rcAlign.right <= 0 || rcAlign.bottom <= 0)
//    {
//        _get_standard_rect();
//        int len = SendMessageW(hAlign, TB_BUTTONCOUNT, 0, 0);               // 对齐工具条数量
//        SendMessageW(hAlign, TB_GETITEMRECT, len - 1, (LPARAM)&rcAlign);    // 取最后一个按钮的右边
//        const int width = rcAlign.right;
//        rcAlign.left    = rcStandard.right;
//        rcAlign.top     = rcStandard.top;
//        rcAlign.right   = rcAlign.left + width;
//        rcAlign.bottom  = rcStandard.bottom;
//
//
//        RECT& rcMenu    = s_info->rcWndMenu;    // 菜单窗口的位置
//        rcMenu.left     = WINDOW_BORDER_WIDTH;
//        rcMenu.top      = WINDOW_CAPTION_HEIGHT;
//        rcMenu.right    = rcMenu.left + rcAlign.right;
//        rcMenu.bottom   = rcMenu.top + (rcAlign.bottom - rcAlign.top) + WINDOW_BORDER_WIDTH;
//    }
//}

void EWindow_Menu_SetPos(HWND hWnd, LPWINDOWPOS pos)
{
    if (hWnd == hStandard)  // 标准工具条
    {
        _get_standard_rect();
        RECT& rcMenu = s_info->rcWndMenu;
        pos->x = 0;
        pos->y = 0;
        //if (rcMenu.right > s_info->cxClient - WINDOW_BORDER_WIDTH)   // 工具条的右边大于最大客户区位置
        //{
        //    pos->cx = s_info->cxClient - rcMenu.left - WINDOW_BORDER_WIDTH;
        //    pos->cy = rcMenu.bottom - rcMenu.top;
        //    return;
        //}
        pos->cx = rcMenu.right - rcMenu.left;
        pos->cy = rcMenu.bottom - rcMenu.top - WINDOW_BORDER_WIDTH;
        return;
    }
    if (hWnd == hAlign)        // 对齐工具条
    {
        //_get_align_rect();
        pos->cx = 0;
        pos->cy = 0;
        pos->x = 0;
        pos->y = 0;
        return;
    }
    if (hWnd == s_info->hWndTool)        // 接口工具条
    {
        //_get_align_rect();
        pos->cx = rcToolInterface.right - rcToolInterface.left;
        pos->cy = rcToolInterface.bottom - rcToolInterface.top;
        pos->x = rcToolInterface.left;
        pos->y = rcToolInterface.top;
        return;
    }
    if (hWnd == s_info->hWndMenu)    // 菜单窗口
    {
        _get_standard_rect();
        RECT& rcMenu = s_info->rcWndMenu;
        MoveWindowEx(s_info->hWndTool, rcToolInterface, true);
        if (rcMenu.right > s_info->cxClient - WINDOW_BORDER_WIDTH)   // 工具条的右边大于最大客户区位置
        {
            pos->cx = s_info->cxClient - rcMenu.left - WINDOW_BORDER_WIDTH;
            pos->cy = rcMenu.bottom - rcMenu.top;
            pos->x  = rcMenu.left;
            pos->y  = rcMenu.top;
            MoveWindowEx(hStandard, rcMenu, true);
            return;
        }
        pos->cx = rcMenu.right - rcMenu.left;
        pos->cy = rcMenu.bottom - rcMenu.top;
        pos->x  = rcMenu.left;
        pos->y  = rcMenu.top;
        return;
    }
}

void EWindow_MoveMenuWindow(HDWP& hDwp)
{
    _get_standard_rect();

    // 位置已经固定, 只要尺寸改变, 
    if (s_info->isReady)
    {
        if (!IsWindowVisible(s_info->hWndMenu))
            ShowWindowEx(s_info->hWndMenu, SW_SHOW);
        EWindow_DeferWindowPos(hDwp, s_info->hWndMenu, s_info->rcWndMenu, true);
    }

}



// 顶边菜单栏容器尺寸被改变, 需要修改菜单栏容器里面的组件
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnMenuSize(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
    _get_standard_rect();
    MoveWindowEx(hStandard, s_info->rcWndMenu, true);
    pData->isReturn = true;
    return 0;
}

_EWINDOW_END_NAMESPACE