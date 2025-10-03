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
            0,          // m_isSysListView, ����ǳ����б��Ļ����ֵ���ǳ����б��ı���߶�
            false,      // m_isSysTreeView, �Ƿ�Ϊ���Ϳ�, ���Ϊ��, �������ù�����λ��, ���͹�����Ϣ, Ϊ�����ȷ��͹�����Ϣ������λ��
            false,      // m_isCallSetScroll, �Ƿ����ԭ�������ù�����λ�ú���
            false,      // isSubClass, �Ƿ����໯, ���Ϊtrue, �ڲ������໯�������, ���Ϊ��, ������ⲿ����Ϣ���ݵ� OnWndProc ������
            true        // isReCalcSize, �Ƿ����¼���ߴ�, Ϊtrue���ڲ������ SetWindowPos �������¼���ǿͻ���
        );
        if (hWnd == s_info->eWnd->GetStateTabOutput() || hWnd == s_info->eWnd->GetStateTabTips())
            s_info->pfnControls(IDC_SCROLL_CALCRECT, (WPARAM)EditScroll_CalcScroll, (LPARAM)pData); // ����һ�¹�����λ��
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
                    // �������� \r, �Ǿ����Լ����õĻ���, �޸ĳ�\r\n
                    newText = str;
                    LPSTR ptr = newText.data();
                    lParam = (LPARAM)ptr;
                    while (*ptr)    // ��\r\r����\r\n
                    {
                        char& ch = *ptr++;
                        if (ch == '\r' && *ptr == '\r')
                            *ptr++ = '\n';
                    }
                    break;
                }
                else
                {
                    pos = strchr(pos + 1, '\r');    // ��������\r�Ļ�, ����Ѱ�ҵ�û���ҵ�Ϊֹ
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
        //TODO ����༭��������������õ�����
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
        // ��ղ˵������
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
    if (0)  // ������, �����ú�ɫ�ı������׿�Ч��
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
    case WM_CTLCOLOREDIT:       // �༭�򱻻滭
    case WM_CTLCOLORSTATIC:     // ����ֹ�ı༭�򱻻滭
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
    if ((pData->style & ES_MULTILINE) == ES_MULTILINE)  // ���б༭��, ��Ҫ����EM_GETRECT ����߶���
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
        // ���Ƕ���, ����һ�¾��ж���
        HDC hdc = GetDC(hWnd);
        HFONT hFont = (HFONT)SendMessageA(hWnd, WM_GETFONT, 0, 0);
        if (!hFont)
            hFont = pData->draw.hFont->GetFont();
        HGDIOBJ hOldFont = SelectObject(hdc, hFont);
        RECT rcText = { 0, 0, 100, 20 };
        DrawTextW(hdc, L"����", 2, &rcText, DT_CALCRECT);
        SelectObject(hdc, hOldFont);
        ReleaseDC(hWnd, hdc);

        // �༭��߶�, �����ǿͻ���
        RECT rc;
        GetWindowRect(hWnd, &rc);
        const bool isBorder = IsBorder(pData->style, pData->styleEx);
        if (isBorder)
            rc.bottom -= 2;

        const int cyScreen = rc.bottom - rc.top;
        const int offsetTop = (cyScreen - (rcText.bottom - rcText.top)) / 2;

        p->rgrc[0].top      += offsetTop;
        p->rgrc[0].left     += 2;   // ����ƫ��һ��
        p->rgrc[0].right    -= 2;
        p->rgrc[0].bottom   -= offsetTop;
    }

    //TODO ��Ҫ�ж��Ƿ�Ϊ���б༭��, Ȼ������ı��ߴ�, ��Ҫ�ѱ༭����д���

}

bool CALLBACK EditScroll_CalcScroll(void* pScroll, OBJSTRUCT* pData, RECT* prc)
{
    prc->top -= 2;
    prc->right += 2;
    prc->bottom += 2;
    return false;
}