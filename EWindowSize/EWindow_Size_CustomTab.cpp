#include "EWindow_Size_Header.h"

_EWINDOW_BEGIN_NAMESPACE

// 自绘的选择夹
LRESULT _EWINDOW_NAMESPACE EWindow_Size_OnCustomTab(HWND hWnd, UINT message, WPARAM& wParam, LPARAM& lParam, LPOBJSTRUCT pData)
{
	return 0;
}

LRESULT CALLBACK WndProc_OnCustomTabItemChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData, bool isMessage)
{
    HDWP hDwp = 0;
    LPTABCHANGE_INFO hdr = (LPTABCHANGE_INFO)lParam;
    const int index = hdr->index;   // 选择夹选择改变
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

    const int calcMdiRight = (g_moveInfo.windowType == 1)   // 计算出来的MDI右边, 需要根据是否显示组件箱来计算
        ? (s_info->rcControl.left)                          // 有组件箱, MDI的位置是组件箱的左边
        : (s_info->cxClient - WINDOW_BORDER_WIDTH);         // 没有组件箱, MDI的位置是窗口的右边 - 边框宽度


    if (info && __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
    {
        // 组件箱是自动隐藏的话, 每次切换都调整mdi的子窗口
        EWindow_MoveMDIClientChild(hDwp);
    }
    else if (s_info->rcMDIClient.right != calcMdiRight)
    {
        // MDI的位置有问题, 需要调整MDI和选择夹和两个组合框
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
            TAB_SEL_MODE_CODE = 0,    // 代码选中
            TAB_SEL_MODE_LBUTTON = 1,    // 鼠标左键点击选中
            TAB_SEL_MODE_RBUTTON = 2,    // 鼠标右键点击选中
            TAB_SEL_MODE_MBUTTON = 3,    // 鼠标中键点击选中
        };

        // 需要让易语言处理MDI被激活的事件, 不然组件箱会被隐藏
        // 不能通过发送消息的方式去处理, 发送消息的话内部会处理
        // 内部处理又会调用这个函数, 造成无限递归....

        //CallWindowProcA(pParent->oldProc, pParent->hWnd, WM_MDIACTIVATE, (WPARAM)g_moveInfo.hChild1, (WPARAM)g_moveInfo.hChild1);
        //ShowWindowEx(s_info->eWnd->GetControl32770Parent(), SW_SHOW);
    }
    ShowWindowEx(g_moveInfo.hChild1, SW_SHOW);

    return 0;
}

_EWINDOW_END_NAMESPACE