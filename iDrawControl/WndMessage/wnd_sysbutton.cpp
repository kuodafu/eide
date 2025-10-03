#include "wnd_header.h"
#include <IDraw_Header.h>
#include "../_scale.h"
// 重画系统按钮
inline void UpdateSysbtn(LPOBJSTRUCT pData)
{
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    InvalidateRect(pData->hObj, &sysBtn->rcBtn, 0);
}

void CALLBACK TimerProc_Leave_mouse(HWND hWnd, UINT message, UINT_PTR id, DWORD dwTimer)
{
    LPOBJSTRUCT pData = (LPOBJSTRUCT)id;
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    if (sysBtn->index == -1)
    {
        s_info->pTips->hide();
        KillTimer(hWnd, id);
        return;
    }

    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    if (!PtInRect(&sysBtn->rcBtn, pt))
    {
        sysBtn->index = -1;
        sysBtn->state = STATE::NORMAL;
        UpdateSysbtn(pData);
    }

}

inline LPCWSTR _sysbtnid2tips(int id, LPCWSTR* pTitle)
{
    LPCWSTR tips = L"", title = L"";
    switch (id)
    {
    case SYSBTN_CLOSE:
        tips = L"关闭";
        break;
    case SYSBTN_MAX:
        tips = L"最大化";
        break;
    case SYSBTN_RESTORE:
        tips = L"还原";
        break;
    case SYSBTN_MIN:
        tips = L"最小化";
        break;
    case SYSBTN_MENU:
        tips = L"菜单";
        break;
    case SYSBTN_SETTING:
        tips = L"设置";
        break;
    case SYSBTN_SKIN:
        tips = L"切换视觉库皮肤";
        break;
    case SYSBTN_HELP:
        tips = L"点击打开视觉库开发文档";
        break;
    default:
        break;
    }
    if (pTitle)*pTitle = title;
    return tips;
}

LRESULT _sysbtn_mousemove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    if (PtInRect(&sysBtn->rcBtn, pt))
    {
        for (DWORD i = 0; i < sysBtn->count; i++)
        {
            if (PtInRect(&sysBtn->btn[i].rc, pt))    // 在哪个按钮中
            {
                if (i == sysBtn->index)return 0;
                //TRACKMOUSEEVENT tm = { 0 };
                //tm.cbSize = sizeof(tm);
                //tm.dwHoverTime = 10;
                //tm.hwndTrack = pData->hObj;
                //tm.dwFlags = TME_NONCLIENT;
                //BOOL b = TrackMouseEvent(&tm);

                sysBtn->index = i;
                sysBtn->state = (wParam & VK_LBUTTON) ? STATE::DOWN : STATE::HOT;
                UpdateSysbtn(pData);
                SetTimer(hWnd, (UINT_PTR)pData, 50, TimerProc_Leave_mouse);
                if (sysBtn->state == STATE::HOT)
                {
                    LPCWSTR title = L"";
                    LPCWSTR tips = _sysbtnid2tips(sysBtn->btn[i].ID, &title);
                    if (tips && tips[0] || title && title[0])
                    {
                        s_info->pTips->SetTipsTitle(title);

                        LPEX_FONT hFont = g_scale.GetFont(s_info->defData->draw.hFont);
                        LPEX_FONT hFontBold = g_scale.GetFont(s_info->defData->draw.hFontBold);
                        POINT pt = { sysBtn->btn[i].rc.left, sysBtn->btn[i].rc.bottom };
                        pt.x += s_info->rcEWindow.left;
                        pt.y += s_info->rcEWindow.top;
                        s_info->pTips->show(tips, hFontBold, hFont, pt.x, pt.y);
                    }
                }

                return 0;
                //return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
            }
        }
    }

    // 走到这就是鼠标没有在任何系统按钮上
    if (sysBtn->index != -1)
    {
        sysBtn->index = -1;
        UpdateSysbtn(pData);
    }
    return 0;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT _sysbtn_lDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    if (sysBtn->index == -1)
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    if (message == WM_LBUTTONDBLCLK)return 0;
    switch (sysBtn->btn[sysBtn->index].flag)
    {
    case SC_CLOSE:
        if (sysBtn->btn[SYSBTN_MAX].flag == SC_MAXIMIZE
            && (pt.x > sysBtn->rcBtn.right - 5 || pt.y < 5))    // 在右边或者顶边5个像素
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        break;
    case SC_MAXIMIZE:
    case SC_RESTORE:
    case SC_MINIMIZE:
    case SC_MENU:
    case SC_SETTING:
    case SC_SKIN:
    case SC_HELP:
        if (sysBtn->btn[SYSBTN_MAX].flag == SC_MAXIMIZE && pt.y < 5)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        break;
    default:
        throw;
    }
    sysBtn->downIndex = sysBtn->index;
    sysBtn->state = STATE::DOWN;
    UpdateSysbtn(pData);
    return 0;
}

LRESULT _sysbtn_lUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    // 当前鼠标按下的按钮不为-1, 且当前鼠标进入的按钮等于按下的按钮, 表示放开时的按钮是按下的按钮
    if (sysBtn->downIndex != -1 && sysBtn->downIndex == sysBtn->index)
    {
        sysBtn->downIndex = -1;
        DWORD& flag = sysBtn->btn[sysBtn->index].flag;
        SendMessageW(pData->hObj, WM_SYSCOMMAND, flag, lParam);
        if (flag == SC_CLOSE) return 0;

        sysBtn->state = STATE::HOT;
        UpdateSysbtn(pData);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }

    if (sysBtn->state)    // 放开时有按钮有状态, 需要去除状态
    {
        for (DWORD i = 0; i < sysBtn->count; i++)
        {
            if (PtInRect(&sysBtn->btn[i].rc, pt))    // 在哪个按钮中
            {
                sysBtn->state = STATE::HOT;
                UpdateSysbtn(pData);
                return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
            }
        }
        sysBtn->state = STATE::NORMAL;
        UpdateSysbtn(pData);
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT _sysbtn_rUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    // 当前鼠标按下的按钮不为-1, 且当前鼠标进入的按钮等于按下的按钮, 表示放开时的按钮是按下的按钮
    if (sysBtn->rdownIndex != -1 && sysBtn->rdownIndex == sysBtn->index)
    {
        sysBtnCommand(pData, TRUE, sysBtn->btn[sysBtn->rdownIndex].flag);
        sysBtn->rdownIndex = -1;
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

LRESULT _sysbtn_rDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
    if (sysBtn->index != -1)
    {
        sysBtn->rdownIndex = sysBtn->index;    //右键按下的按钮索引是当前鼠标所在的索引
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}
