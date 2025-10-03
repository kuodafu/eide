#include "../EWindow_Fne_Header.h"
LRESULT CALLBACK WndProc_Edit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void _edit_ncpaint(HWND hWnd, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_Edit_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void _edit_nc_calc_size(HWND hWnd, LPARAM lParam, LPOBJSTRUCT pData);
bool CALLBACK EditScroll_CalcScroll(void* pScroll, OBJSTRUCT* pData, RECT* prc);

LPOBJSTRUCT _subclass_Edit(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, bool isScroll)
{
    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_Edit, reMoveBorder, isSetBack, reMoveClassStyle);
    SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_NOZORDER);
    pData->draw.isErasebkg = true;
    pData->fnCtlcolor = WndProc_Edit_CtlColor;
    if (isScroll && s_info->MY_MF_OWNERDRAW)
    {
        pData->drawScroll = ScrollCreate(pData->hWnd, pData->draw.color.crBorder,
            0,          // m_isSysListView, 如果是超级列表框的话这个值就是超级列表框的表项高度
            false,      // m_isSysTreeView, 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
            false,      // m_isCallSetScroll, 是否调用原来的设置滚动条位置函数
            false,      // isSubClass, 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
            true        // isReCalcSize, 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
        );
        if (hWnd == s_info->eWnd->GetStateTabOutput() || hWnd == s_info->eWnd->GetStateTabTips())
            s_info->pfnControls(IDC_SCROLL_CALCRECT, (WPARAM)EditScroll_CalcScroll, (LPARAM)pData); // 调整一下滚动条位置
    }
    return pData;
}

LRESULT CALLBACK WndProc_Edit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
    if (pObj && s_info->MY_MF_OWNERDRAW)
    {
        LRESULT ret = 0;
        if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
        {
            switch (message)
            {
            case WM_NCCALCSIZE:
            {
                if (wParam)
                    _edit_nc_calc_size(hWnd, lParam, pData);
                break;
            }
            case WM_NCPAINT:
            {
                _edit_ncpaint(hWnd, pData);
                break;
            }
            default:
                break;
            }
            return ret;
        }
    }
    switch (message)
    {
    case WM_ENTERIDLE:
    {
        if (wParam == MSGF_DIALOGBOX)
        {
            //HMENU hMenu = (HMENU)SendMessageW(hWnd, MN_GETHMENU, 0, 0);
            break;
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_ERASEBKGND:
    {
        RECT rc;
        HDC hdc = (HDC)wParam;
        //HDC hdc = GetWindowDC(hWnd);
        GetClientRect(hWnd, &rc);

        const bool IsEnabled = IsWindowEnabled(pData->hWnd);
        HBRUSH hbrBack = IsEnabled ? pData->draw.color.hbrBackGround : pData->draw.color.hbrBan;
        COLORREF crBack = IsEnabled ? pData->draw.color.crBackGround : pData->draw.color.crBan;
        COLORREF crText = IsEnabled ? pData->draw.color.crText : pData->draw.color.crTextBan;

        FillRect(hdc, &rc, hbrBack);
        SetBkColor(hdc, crBack);
        SetTextColor(hdc, crText);

        if (pData->draw.isTransparent)
            SetBkMode(hdc, TRANSPARENT);

        //ReleaseDC(hWnd, hdc);
        return 1;
    }
    case WM_KEYDOWN:
    {
        if (wParam == 'A' && GetAsyncKeyState(VK_CONTROL))
        {
            SendMessageA(hWnd, EM_SETSEL, 0, -1);
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_RBUTTONUP:
    {
        SendMessageW(s_info->hEWnd, WM_USER + 0x12, 0, 1);
        LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        SendMessageW(s_info->hEWnd, WM_USER + 0x12, 0, 0);
        ret = 0;
        break;
    }
    case WM_SETTEXT:
    {
        _str newText;
        if (hWnd == s_info->eWnd->GetStateTabTips())
        {
            LPCSTR str = (LPSTR)lParam;
            LPCSTR pos = strchr(str, '\r');
            while (pos)
            {
                if (pos[1] == 0)
                    break;
                if (pos[1] == '\r')
                {
                    // 连续两个 \r, 那就是自己设置的换行, 修改成\r\n
                    newText = str;
                    LPSTR ptr = newText.data();
                    lParam = (LPARAM)ptr;
                    while (*ptr)    // 把\r\r换成\r\n
                    {
                        char& ch = *ptr++;
                        if (ch == '\r' && *ptr == '\r')
                            *ptr++ = '\n';
                    }
                    break;
                }
                else
                {
                    pos = strchr(pos + 1, '\r');    // 不是两个\r的话, 继续寻找到没有找到为止
                }
            }
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_DESTROY:
    {
        IDrawScroll* pScroll = ((IDrawScroll*)pData->drawScroll);
        delete pScroll;
        pData->drawScroll = 0;
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_NCPAINT:
    {
        _edit_ncpaint(hWnd, pData);
        break;
    }
    case WM_NCCALCSIZE:
    {
        if (wParam && s_info->MY_MF_OWNERDRAW)
        {
            //LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
            _edit_nc_calc_size(hWnd, lParam, pData);
            //return ret;
        }
        break;
    }
    case WM_DPICHANGED:
    {
        EWindow_DPIChange(pData);
        //TODO 输出编辑框跟随易语言设置的字体
        break;
    }
#ifdef _DEBUG
    case WM_SETFONT:
    {
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
#endif
    case 12100:
    {
        // 清空菜单被点击
        SetWindowTextW(hWnd, L"");
        return 0;
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    return 0;
}


void _edit_ncpaint(HWND hWnd, LPOBJSTRUCT pData)
{
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;

    RECT rcBorder, rcClient, rcWnd;
    CWndBase::GetRect(hWnd, &rcBorder, &rcWnd, 0, &rcClient);

    const int cxClient = rcClient.right - rcClient.left;
    const int cyClient = rcClient.bottom - rcClient.top;
    const int cxScreen = rcWnd.right - rcWnd.left;
    const int cyScreen = rcWnd.bottom - rcWnd.top;

    rcClient.left = rcClient.left - rcWnd.left;
    rcClient.top = rcClient.top - rcWnd.top;
    rcClient.right = rcClient.left + cxClient;
    rcClient.bottom = rcClient.top + cyClient;

    rcWnd = { 0, 0, cxScreen, cyScreen };
    bool isBorder = (pData->style & WS_BORDER) != 0;
    if (!isBorder)
        isBorder = (((pData->styleEx & WS_EX_CLIENTEDGE)
            || (pData->styleEx & WS_EX_DLGMODALFRAME)
            || (pData->styleEx & WS_EX_WINDOWEDGE)
            )) != 0;

    RECT rcRgn2 = { rcClient.left, rcClient.top, rcClient.right, rcClient.bottom };
    if (pObj && s_info->MY_MF_OWNERDRAW)
    {
        LONG vThumbSize = (LONG)pObj->m_vInfo.btnThumbSize;
        LONG hThumbSize = (LONG)pObj->m_hInfo.btnThumbSize;

        if (pObj->m_isVScroll && pObj->m_isHScroll)
            rcRgn2 = { rcClient.left, rcClient.top, rcClient.right + vThumbSize, rcClient.bottom + hThumbSize };
        else if (pObj->m_isVScroll)
            rcRgn2 = { rcClient.left, rcClient.top, rcClient.right + vThumbSize, rcClient.bottom };
        else if (pObj->m_isHScroll)
            rcRgn2 = { rcClient.left, rcClient.top, rcClient.right, rcClient.bottom + hThumbSize };
    }


    HDC hdc     = GetWindowDC(hWnd);
    HRGN hRgn1  = CreateRectRgn(0, 0, cxScreen, cyScreen);
    HRGN hRgn2  = CreateRectRgn(rcRgn2.left, rcRgn2.top, rcRgn2.right, rcRgn2.bottom);
    HRGN hRgn   = CreateRectRgn(0, 0, 0, 0);
    int ret     = CombineRgn(hRgn, hRgn1, hRgn2, RGN_DIFF);

    SelectClipRgn(hdc, hRgn);
    FillRect(hdc, &rcWnd, IsWindowEnabled(hWnd) ? pData->draw.color.hbrBackGround : pData->draw.color.hbrBan);
    if (0)  // 调试用, 调试用红色的背景容易看效果
    {
        HBRUSH hbr = CreateSolidBrush(RGB(255, 0, 0));
        FillRect(hdc, &rcWnd, hbr);
        DeleteObject(hbr);
    }
    if (isBorder)
    {
        HGDIOBJ hOldPen = SelectObject(hdc, pData->draw.color.hpenBorder);
        HGDIOBJ hOldBrush = SelectObject(hdc, GetStockBrush(NULL_BRUSH));

        Rectangle(hdc, 0, 0, cxScreen, cyScreen);
        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
    }

    DeleteObject(hRgn1);
    DeleteObject(hRgn2);
    DeleteObject(hRgn);

    SelectClipRgn(hdc, 0);
    ReleaseDC(hWnd, hdc);
}

LRESULT CALLBACK WndProc_Edit_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPOBJSTRUCT pChild = GetDataFromHwnd((HWND)lParam);
    if (!pChild)
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    HDC hdc = (HDC)wParam;

    switch (message)
    {
    case WM_CTLCOLOREDIT:       // 编辑框被绘画
    case WM_CTLCOLORSTATIC:     // 被禁止的编辑框被绘画
    {
        HBRUSH hbr = 0;
        COLORREF crText = 0;
        COLORREF crBack = 0;

        if (message == WM_CTLCOLOREDIT || pChild->hWnd == s_info->eWnd->GetStateTabTips() || pChild->hWnd == s_info->eWnd->GetStateTabOutput())
        {
            hbr     = pChild->draw.color.hbrBackGround;
            crText  = pChild->draw.color.crText;
            crBack  = pChild->draw.color.crBackGround;

        }

        if (hbr == 0 && !IsWindowEnabled(pChild->hWnd))
        {
            hbr     = pChild->draw.color.hbrBan;
            crText  = pChild->draw.color.crTextBan;
            crBack  = pChild->draw.color.crBan;
        }


        SetBkColor(hdc, crBack);
        SetTextColor(hdc, crText);
        //FillRect(hdc, &pChild->pos.rcClient, hbr);
        return (LRESULT)hbr;
    }
    default:
        break;
    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

void _edit_nc_calc_size(HWND hWnd, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPNCCALCSIZE_PARAMS p = (LPNCCALCSIZE_PARAMS)lParam;
    WPARAM margin = SendMessageW(hWnd, EM_GETMARGINS, 0, 0);
    p->rgrc[0].left += (LOWORD(margin));
    p->rgrc[0].right -= (HIWORD(margin));
    if ((pData->style & ES_MULTILINE) == ES_MULTILINE)  // 多行编辑框, 需要加载EM_GETRECT 的左边顶边
    {
        RECT rc;
        SendMessageW(hWnd, EM_GETRECT, 0, (LPARAM)&rc);
        if (rc.left < 2)    rc.left = 2;
        if (rc.top < 2)     rc.top = 2;
        p->rgrc[0].top      += rc.top;
        p->rgrc[0].left     += rc.left;
        p->rgrc[0].right    -= rc.left;
        p->rgrc[0].bottom   -= rc.top;

    }
    else
    {
        // 不是多行, 处理一下居中对齐
        HDC hdc = GetDC(hWnd);
        HFONT hFont = (HFONT)SendMessageA(hWnd, WM_GETFONT, 0, 0);
        if (!hFont)
            hFont = pData->draw.hFont->GetFont();
        HGDIOBJ hOldFont = SelectObject(hdc, hFont);
        RECT rcText = { 0, 0, 100, 20 };
        DrawTextW(hdc, L"测试", 2, &rcText, DT_CALCRECT);
        SelectObject(hdc, hOldFont);
        ReleaseDC(hWnd, hdc);

        // 编辑框高度, 包含非客户区
        RECT rc;
        GetWindowRect(hWnd, &rc);
        const bool isBorder = IsBorder(pData->style, pData->styleEx);
        if (isBorder)
            rc.bottom -= 2;

        const int cyScreen = rc.bottom - rc.top;
        const int offsetTop = (cyScreen - (rcText.bottom - rcText.top)) / 2;

        p->rgrc[0].top      += offsetTop;
        p->rgrc[0].left     += 2;   // 往右偏移一点
        p->rgrc[0].right    -= 2;
        p->rgrc[0].bottom   -= offsetTop;
    }

    //TODO 需要判断是否为单行编辑框, 然后计算文本尺寸, 需要把编辑框居中处理

}

bool CALLBACK EditScroll_CalcScroll(void* pScroll, OBJSTRUCT* pData, RECT* prc)
{
    prc->top -= 2;
    prc->right += 2;
    prc->bottom += 2;
    return false;
}