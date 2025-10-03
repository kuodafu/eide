#include "../EWindow_Fne_Header.h"
// ���һ��״̬�������
#define __ADD(_state) (pData->draw.state = (STATE)(pData->draw.state | _state))
// �������ɾ��һ��״̬
#define __DEL(_state) (pData->draw.state = (STATE)(pData->draw.state & ~_state))
// ��ѯ�����û�����״̬
#define __QUE(_state) ((pData->draw.state & _state) == _state)

LRESULT CALLBACK WndProc_Button(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_Button_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
inline void WINAPI _button_crChange(LPOBJSTRUCT pData)
{
    pData->draw.color.crBan = pData->draw.color.crBackGround;
    pData->draw.color.hbrBan = s_info->pfnCreateBrush(pData->draw.color.crBan);
}
LPOBJSTRUCT _subclass_Button(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle)
{
    DWORD state = (DWORD)SendMessageW(hWnd, BM_GETSTATE, 0, 0);
    const bool isCheck  = (state & BST_CHECKED) == BST_CHECKED;
    const bool isHot    = (state & BST_HOT)     == BST_HOT;
    const bool isFocus  = (state & BST_FOCUS)   == BST_FOCUS;
    const bool isDown   = (state & BST_PUSHED)  == BST_PUSHED;   // ����״̬
    const bool isEnabled= IsWindowEnabled(hWnd);   // ����״̬

    const bool isCheck3 = (state & BST_INDETERMINATE)   == BST_INDETERMINATE;   // ��̬ѡ��
    const bool isDown1  = (state & BST_DROPDOWNPUSHED)  == BST_DROPDOWNPUSHED;  // �ð�ť��������״̬, ���ڰ�ť����TBSTYLE_DROPDOWN��ʽʱ����

    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_Button, reMoveBorder, isSetBack, reMoveClassStyle);
    InsertStyle(pData->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    pData->fnCrChange = _button_crChange;
    _button_crChange(pData);
    if (isCheck)    __ADD(STATE::SELECTED);
    if (isHot)      __ADD(STATE::HOT);
    if (isFocus)    __ADD(STATE::FOCUS);
    if (isDown)     __ADD(STATE::DOWN);

    if (isCheck3)   __ADD(STATE::INDETERMINATE);
    if (isDown1)    __ADD(STATE::DOWNDROP);
    if (!isEnabled) __ADD(STATE::BAN);

    pData->fnCtlcolor = WndProc_Button_CtlColor;
    return pData;
}

// ����״̬����滭��Ҫ�ı�����ˢ���ı���ɫ
inline void _calc_brush_crtext(LPOBJSTRUCT pData, HBRUSH& hbr, COLORREF& crText, STATE& state)
{
    if (__QUE(STATE::BAN))      // ��ֹ״̬�����ȴ���
    //if (!IsWindowEnabled(pData->hWnd))      // ��ֹ״̬�����ȴ���
    {
        hbr = pData->draw.color.hbrBan;
        crText = pData->draw.color.crTextBan;
        state = STATE::BAN;
    }
    else if (__QUE(STATE::DOWN))
    {
        hbr = pData->draw.color.hbrDown;
        crText = pData->draw.color.crTextDown;
        state = STATE::DOWN;
    }
    else if (__QUE(STATE::HOT))
    {
        hbr = pData->draw.color.hbrHot;
        crText = pData->draw.color.crTextHot;
        state = STATE::HOT;
    }
    else
    {
        hbr = pData->draw.color.hbrBackGround;
        crText = pData->draw.color.crText;
        state = STATE::NORMAL;
    }
}

// ������ʽ�����Ƿ��б߿�
inline bool _calc_isborder(DWORD style, DWORD styleEx)
{
    bool isBorder = (style & WS_BORDER) != 0;
    if (!isBorder && (styleEx & WS_EX_CLIENTEDGE)
        || (styleEx & WS_EX_DLGMODALFRAME)
        || (styleEx & WS_EX_WINDOWEDGE))
        isBorder = true;
    return isBorder;
}
// ������ʽ�����ı���ʽ
inline int _button_calc_format(DWORD style)
{
#define _btn_query(_v) ((style & _v) == _v)
    int fmt = 0;
    if (_btn_query(BS_CENTER))      fmt = DT_CENTER;
    else if (_btn_query(BS_RIGHT))  fmt = DT_RIGHT;
    else                            fmt = DT_LEFT;

    if (_btn_query(BS_VCENTER))     fmt |= DT_VCENTER | DT_SINGLELINE;
    else if (_btn_query(BS_BOTTOM)) fmt |= DT_BOTTOM;
    else if (_btn_query(BS_TOP))    fmt |= DT_TOP;
    if (fmt == 0)
    {
        int btnType = LOBYTE(style);
        const int isChechBox = (btnType == BS_CHECKBOX || btnType == BS_AUTOCHECKBOX || btnType == BS_3STATE || btnType == BS_AUTO3STATE);
        const int isRadio = (btnType == BS_RADIOBUTTON || btnType == BS_AUTORADIOBUTTON);
        if (isChechBox || isRadio)   // ��ѡ�����ߵ�ѡ��
            fmt = DT_VCENTER | DT_SINGLELINE;
        else
            fmt = DT_VCENTER | DT_CENTER | DT_SINGLELINE;
    }
    return fmt;
}

LRESULT CALLBACK WndProc_Button(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    //return DefWindowProcA(hWnd, message, wParam, lParam);
    switch (message)
    {
    case WM_ENABLE:
    {
        if (wParam) __DEL(STATE::BAN);  // ������״̬��ȥ����ֹ״̬
        else        __ADD(STATE::BAN);  // �ǽ�ֹ״̬�ͼ����ֹ״̬
        InvalidateRect(hWnd, 0, 0);
        return pData->fnDefProc(hWnd, message, wParam, lParam);
        return pData->fnCallProc(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_DESTROY:
        if (pData->draw.hCanvas)
            _canvas_destroy(pData->draw.hCanvas);
        return pData->fnCallProc(pData->oldProc, hWnd, message, wParam, lParam);
        return pData->fnDefProc(hWnd, message, wParam, lParam);
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        HDC hdc = s_info->iDraw->BeginPaint(pData, ps, false, false);
        const bool isControlBtn = GetParent(hWnd) == s_info->eWnd->GetControl32770();   // ���Ϊtrue�Ļ�, һ��Ҫ����ͨ��ť�滭

        HBRUSH hbr = 0;
        COLORREF crText = 0;
        STATE state;
        const int fmt = isControlBtn ? (DT_VCENTER | DT_CENTER | DT_SINGLELINE) : (_button_calc_format(pData->style)); // ��ť�ı���ʽ
        //const bool isBorder = _calc_isborder(pData->style, pData->styleEx);
        const int len = GetWindowTextLengthW(hWnd) + 1;
        wstr str;
        LPCWSTR lpszText = 0;
        if (len > 1)
        {
            str.reserve(len);
            GetWindowTextW(hWnd, str.data(), len);
            lpszText = str.c_str();
        }
        _calc_brush_crtext(pData, hbr, crText, state);
        SetTextColor(hdc, crText);
        SelectObject(hdc, hbr);

        RECT rcText = ps.rc;    // �ı��滭λ��
        int btnType = LOBYTE(pData->style);
        const int isChechBox = (btnType == BS_CHECKBOX || btnType == BS_AUTOCHECKBOX || btnType == BS_3STATE || btnType == BS_AUTO3STATE);
        const int isRadio = (btnType == BS_RADIOBUTTON || btnType == BS_AUTORADIOBUTTON);
        if (!isControlBtn && (isChechBox || isRadio))   // ��ѡ�����ߵ�ѡ��
        {
            FillRect(hdc, &ps.rc, pData->draw.color.hbrBackGround);
            RECT_F rcIcon;
            LPWINDOW_ICON_DATA icon = 0;
            bool isCheck = __QUE(STATE::SELECTED);

            const LPCWSTR pszID = isChechBox
                ? (isCheck ? L"CheckBox_Check"  : L"CheckBox")
                : (isCheck ? L"Radio_Check"     : L"Radio");
            icon = s_info->pResource->GetIcon(pszID, &rcIcon, state, s_info->dpiX);
            // ��ѡ���ѡ���滭��ʽ����һ����, ����ͼ������, �ı�����, ����ֻ�����������
            if (icon)
            {
                const float cxIcon = rcIcon.width();
                const float cyIcon = rcIcon.height();
                Gdiplus::Graphics gp(hdc);
                Gdiplus::RectF rcDest;
                rcDest.X = 3;
                rcDest.Y = (float)((ps.cyClient - (int)cyIcon) / 2);
                rcDest.Width = cxIcon;
                rcDest.Height = cyIcon;
                gp.DrawImage(icon->hBitmapGdip, rcDest, rcIcon.left, rcIcon.top,
                    rcIcon.right - rcIcon.left, rcIcon.bottom - rcIcon.top, Gdiplus::Unit::UnitPixel);
                rcText.left += ((int)cxIcon + 5);
            }
        }
        else if (!isControlBtn && btnType == BS_GROUPBOX)          // �����
        {
            FillRect(hdc, &ps.rc, pData->draw.color.hbrBackGround);

            // ��Ҫ������ı��ĸ߶�, ��߿�����Ҫ���ı����м�滭
            RECT rcTmp = rcText;
            DrawTextW(hdc, lpszText ? lpszText : L"����", -1, &rcTmp, DT_CALCRECT | DT_SINGLELINE);

            rcText.left     += 10;
            rcText.top      += 2;
            rcText.right    = rcText.left + rcTmp.right;
            rcText.bottom   = rcText.top + rcTmp.bottom;

            RoundRect(hdc, 0, rcText.top + (rcTmp.bottom / 2), ps.cxClient, ps.cyClient, 5, 5);
            FillRect(hdc, &rcText, pData->draw.color.hbrBackGround);    // ���ı��������ĺ���ȥ��
        }
        else
        {
            // ��ͨ��ť
            FillRect(hdc, &ps.rc, hbr);
            //if (isBorder)
            //    Rectangle(hdc, ps.rc.left, ps.rc.top, ps.cxClient, ps.cyClient);
            //else
            //    FillRect(hdc, &ps.rc, hbr);
            if (__QUE(STATE::DOWN))
                OffsetRect(&rcText, 1, 1);
        }

        // ��������滭�ı�����һ����, �滭��λ�����ж���ʲô���������λ��
        if (lpszText)
            DrawTextW(hdc, lpszText, -1, &rcText, fmt);
        s_info->iDraw->EndPaint(ps);
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (LOBYTE(pData->style) != BS_GROUPBOX && __QUE(STATE::NORMAL))
        {
            __ADD(STATE::HOT);
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hWnd;
            tme.dwHoverTime = HOVER_DEFAULT;
            TrackMouseEvent(&tme);
            InvalidateRect(hWnd, 0, 0);
        }
        return pData->fnDefProc(hWnd, message, wParam, lParam);
    }
    case WM_MOUSELEAVE:
    {
        if (LOBYTE(pData->style) != BS_GROUPBOX)
        {
            __DEL(STATE::HOT);
            InvalidateRect(hWnd, 0, 0);
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        if (LOBYTE(pData->style) != BS_GROUPBOX)
        {
            __ADD(STATE::DOWN);
            InvalidateRect(hWnd, 0, 0);
        }
        SetCapture(hWnd);
        return pData->fnDefProc(hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONUP:
        ReleaseCapture();
        break;
    case WM_CAPTURECHANGED:
    {
        if (LOBYTE(pData->style) == BS_GROUPBOX)
            return 0;
        POINT pt;
        RECT rc;
        GetCursorPos(&pt);
        GetWindowRect(hWnd, &rc);
        __DEL(STATE::DOWN);
        if (PtInRect(&rc, pt))
        {
            if (((pData->style & BS_CHECKBOX) == BS_CHECKBOX) || ((pData->style & BS_RADIOBUTTON) == BS_RADIOBUTTON))   // ��ѡ�����ߵ�ѡ��
            {
                if (__QUE(STATE::SELECTED)) __DEL(STATE::SELECTED); // ��ѡ��״̬��ȥ��
                else                        __ADD(STATE::SELECTED); // û�оͼ���
            }
            __ADD(STATE::HOT);  // �ſ����ʱ��껹�ڰ�ť��, ��Ҫ�����ȵ�״̬
            // ������ť������п��������ť�Ѿ�����
            // ���Դ�����ť�����Ͳ�Ҫʹ�� pData ���ָ����
            SendMessageA(GetParent(hWnd), WM_COMMAND, GetDlgCtrlID(hWnd), (LPARAM)hWnd);
        }
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case BM_GETCHECK:
        return (__QUE(STATE::SELECTED) ? BST_CHECKED : BST_UNCHECKED);
    case BM_SETCHECK:
    {
        if (LOBYTE(pData->style) == BS_GROUPBOX)
            return 0;
        if (wParam == BST_CHECKED)
            __ADD(STATE::SELECTED);
        else
            __DEL(STATE::SELECTED);
        InvalidateRect(hWnd, 0, 0);
        return pData->fnCallProc(pData->oldProc, hWnd, message, wParam, lParam);
        return 0;
    }
    case WM_NCPAINT:
        return WndProc_DefNcPaint(hWnd, message, wParam, lParam, pData);
    //case BM_GETSTATE:
    //case BM_SETSTATE:
    //case BM_SETSTYLE:
    //case BM_CLICK:
    //case BM_GETIMAGE:
    //case BM_SETIMAGE:
        //return pData->fnCallProc(pData->oldProc, hWnd, message, wParam, lParam);

    default:
        return pData->fnDefProc(hWnd, message, wParam, lParam);
        return pData->fnCallProc(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc_Button_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    HWND hChild = (HWND)lParam;
    LPOBJSTRUCT pChild = GetDataFromHwnd(hChild);
    if (!pChild)
        return pData->fnCallProc(pData->oldProc, hWnd, message, wParam, lParam);
    return (LRESULT)pChild->draw.color.hbrBackGround;
}
