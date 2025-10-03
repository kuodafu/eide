#include "../EWindow_Fne_Header.h"
#include <gdiplus/drawImage.h>
#define HONE_BTN_NEW        0
#define HONE_BTN_OPEN       1
#define HONE_BTN_LATELY     2
#define HONE_BTN_CLEAR      3
struct HONE_PAGE_WINDOW
{
    HWND hStatic;   // ��ʾѡ��Դ��ı�ǩ
    HWND hList;     // ��ʾ�����Դ��ĳ���
    HWND hNew;      // �½���ť
    HWND hOpen;     // �򿪰�ť
    HWND hLately;   // �����ť
    HWND hClear;    // ��հ�ť
    HWND ie;        // ���ı������
};
struct HONE_PAGE_BUTTON
{
    STATE state;
    LPCWSTR name;
};


static HONE_PAGE_WINDOW m_honeData;     // ��ʼҳ����
static Gdiplus::Bitmap* m_hBitmap;      // ��ʼҳ��λͼ
static HONE_PAGE_BUTTON m_button[4];    // 4����ť����Ϣ

LPOBJSTRUCT _subclass_EHone_Button(HWND hWnd);
LRESULT CALLBACK WndProc_MDIClient_EHomePage_Child2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
// MDI�µĵ������Ӵ���, �����������Ҫ����ʾ��ʼҳ����Ϣ
LRESULT CALLBACK WndProc_MDIClient_EHomePage_Child3(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
// ��ʼҳ�µ� �½�/��/���/��� �⼸����ť�����໯����
LRESULT CALLBACK WndProc_MDIClient_EHomePage_Button(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

inline void _remove_border(HWND hWnd)
{
    if (!hWnd)return;
    DWORD style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    DWORD styleEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    styleEx &= ~WS_EX_CLIENTEDGE;
    styleEx &= ~WS_EX_WINDOWEDGE;
    //newStyleEx &= ~WS_EX_OVERLAPPEDWINDOW;
    //newStyleEx &= ~WS_EX_PALETTEWINDOW;
    styleEx &= ~WS_EX_STATICEDGE;
    styleEx &= ~WS_EX_DLGMODALFRAME;

    style &= ~WS_BORDER;
    style &= ~WS_SIZEBOX;

    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, styleEx);
    SetWindowLongPtrW(hWnd, GWL_STYLE, style);
}
// MDIClient�µ���ʼҳ
LPOBJSTRUCT _subclass_MDIClient_EHomePage(HWND hWnd, LPOBJSTRUCT pData, MDICLIENT_ALL_WINDOW* mdi)
{
    _remove_border(hWnd);
    hWnd = GetWindow(hWnd, GW_CHILD);   // MDIClient�µĵڶ����Ӵ���, ���ݹ����Ĳ�����mdi�µĵ�һ�㴰��
    _remove_border(hWnd);
    hWnd = GetWindow(hWnd, GW_CHILD);   // MDI�µĵ����㴰��, ������� �½�,��,���,��� �������Դ���б���
    _remove_border(hWnd);

    return 0;
    if (!m_hBitmap)
    {
        HBITMAP hBitmap = LoadBitmapW(CWndBase::GetInst(), MAKEINTRESOURCEW(620));
        m_hBitmap = new Gdiplus::Bitmap(hBitmap, 0);
        DeleteObject(hBitmap);

        //DWORD size = 0;
        //LPBYTE pLogo = s_info->pResource->GetELogo(&size);
        //m_hBitmap = _img_create_from_memory(pLogo, size);
    }
    LPOBJSTRUCT data = 0;
    hWnd = GetWindow(hWnd, GW_CHILD);   // MDIClient�µĵڶ����Ӵ���, ���ݹ����Ĳ�����mdi�µĵ�һ�㴰��
    data = _subclass(hWnd, WndProc_MDIClient_EHomePage_Child2, true, false, 3);
    if (!data)return 0;

    hWnd = GetWindow(hWnd, GW_CHILD);   // MDI�µĵ����㴰��, ������� �½�,��,���,��� �������Դ���б���


    LPOBJSTRUCT ewindow = _subclass(hWnd, WndProc_MDIClient_EHomePage_Child3, false, false, 0);
    //ewindow->draw.isErasebkg = true;
    ewindow->draw.color.crBackGround = RGB(37, 37, 38);
    data->draw.color.hbrBackGround = s_info->pfnCreateBrush(data->draw.color.crBackGround);
    InsertStyle(ewindow->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    //SetClassLongPtrA(ewindow->hWnd, GCL_STYLE, GetClassLongPtrA(ewindow->hWnd, GCL_STYLE) | CS_VREDRAW | CS_HREDRAW);


    // ����������ʾ��������
    hWnd = GetWindow(hWnd, GW_CHILD);
    int id = 0;
    while (hWnd)
    {
        id = GetDlgCtrlID(hWnd);
        switch (id)
        {
        case 1214:      // ��ʾ����򿪵�Դ�볬��
        {
            m_honeData.hList = hWnd;
            data = _subclass_listView(hWnd, true, false, 0, 24, true, false);
            DWORD style = ListView_GetExtendedListViewStyle(hWnd);
            ListView_SetExtendedListViewStyle(hWnd, LVS_EX_FULLROWSELECT | style);
            InsertStyle(data->hWnd, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
            break;
        }
        case 1211:      // �½���ť
            m_honeData.hNew = hWnd;
            m_button[HONE_BTN_NEW].name = L"�½�";
            m_button[HONE_BTN_NEW].state = STATE::NORMAL;
            _subclass_EHone_Button(hWnd);
            break;
        case 1212:      // �򿪰�ť
            m_honeData.hOpen = hWnd;
            m_button[HONE_BTN_OPEN].name = L"��...";
            m_button[HONE_BTN_OPEN].state = STATE::NORMAL;
            _subclass_EHone_Button(hWnd);
            break;
        case 1213:      // �����ť
            m_honeData.hLately = hWnd;
            m_button[HONE_BTN_LATELY].name = L"���";
            m_button[HONE_BTN_LATELY].state = STATE::NORMAL;
            _subclass_EHone_Button(hWnd);
            break;
        case 1215:      // ��հ�ť
            m_honeData.hClear = hWnd;
            m_button[HONE_BTN_CLEAR].name = L"���";
            m_button[HONE_BTN_CLEAR].state = STATE::NORMAL;
            _subclass_EHone_Button(hWnd);
            break;
        case 65535:     // ��ʾ��ǰѡ���б���Դ��·���ı�ǩ
            m_honeData.hStatic = hWnd;
            _subclass_Static(hWnd, true, false, 0);
            break;
        case 0:         // ���ı������, ��ʾ�����Ե�һЩ��Ϣ
            m_honeData.ie = hWnd;
            break;
        default:
            break;
        }
        hWnd = GetWindow(hWnd, GW_HWNDNEXT);
    }

    return data;
}

LPOBJSTRUCT _subclass_EHone_Button(HWND hWnd)
{
    LPOBJSTRUCT data = _subclass(hWnd, WndProc_MDIClient_EHomePage_Button, false, false, 0);
    return data;
}
// MDI�µĵڶ����Ӵ���
LRESULT CALLBACK WndProc_MDIClient_EHomePage_Child2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    //return WndProc_MDIClient_EHomePage_Child3(hWnd, message, wParam, lParam, pData);
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_ERASEBKGND:
        return 1;
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}

inline void _draw_mid_line(Gdiplus::Graphics& gp, const Gdiplus::Rect& rcDraw)
{
    // ���Ͻ� 10, 80,  ���Ͻ� 209, 80
    // ���½� 10, 280  ���½� 209, 280
    // 10, 105 �м���
    Gdiplus::Rect rc = rcDraw;
    rc.Width = 10;
    rc.Height = 26;
    gp.DrawImage(m_hBitmap, rc, 10, 80, 10, rc.Height, Gdiplus::Unit::UnitPixel); // ���Ͻ�

    rc.X = rcDraw.X + 10;
    rc.Width = rcDraw.Width - 20;
    gp.DrawImage(m_hBitmap, rc, 20, 80, 10, rc.Height, Gdiplus::Unit::UnitPixel); // �м�

    rc.X = rcDraw.Width + rcDraw.X - 10;
    rc.Width = 10;
    gp.DrawImage(m_hBitmap, rc, 200, 80, 10, rc.Height, Gdiplus::Unit::UnitPixel); // ���Ͻ�


    // ���
    rc.X = rcDraw.X;
    rc.Width = 1;
    rc.Y = rcDraw.Y + 26;
    rc.Height = rcDraw.Height - 26;
    gp.DrawImage(m_hBitmap, rc, 10, 106, rc.Width, 10, Gdiplus::Unit::UnitPixel); // ���

    rc.X = rcDraw.X + rcDraw.Width - 1;
    rc.Width = 1;
    rc.Y = rcDraw.Y + 26;
    rc.Height = rcDraw.Height - 26;
    gp.DrawImage(m_hBitmap, rc, 209, 106, rc.Width, 10, Gdiplus::Unit::UnitPixel); // �ұ�


    rc.X = rcDraw.X;
    rc.Width = rcDraw.Width;
    rc.Y = rcDraw.Y + rcDraw.Height - 1;
    rc.Height = 1;
    gp.DrawImage(m_hBitmap, rc, 10, 279, 10, rc.Height, Gdiplus::Unit::UnitPixel); // ����

}

// MDI�µĵ������Ӵ���, �����������Ҫ����ʾ��ʼҳ����Ϣ
LRESULT CALLBACK WndProc_MDIClient_EHomePage_Child3(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT data;
        HDC hdc = s_info->iDraw->BeginPaint(pData, data, false, true);
        Gdiplus::Graphics gp(hdc);
        static int cxIcon = (int)m_hBitmap->GetWidth();
        static int cyIcon = (int)m_hBitmap->GetHeight();
        const int iconLeft = 310;
        const int iconHeight = 65;
        // �ȰѶ��߻�����
        Gdiplus::Rect rcDraw(0, 0, iconLeft, iconHeight);
        gp.DrawImage(m_hBitmap, rcDraw, 0, 0, iconLeft, iconHeight, Gdiplus::Unit::UnitPixel);

        rcDraw.X = iconLeft;
        rcDraw.Y = 0;
        rcDraw.Width = data.cxClient - rcDraw.X;
        rcDraw.Height = iconHeight;
        gp.DrawImage(m_hBitmap, rcDraw, iconLeft, 0, cxIcon - iconLeft - 10, iconHeight, Gdiplus::Unit::UnitPixel);

        // �ڻ��м�, ����ɫ��һ��
        rcDraw.X = 0;
        rcDraw.Y = iconHeight;
        rcDraw.Width = data.cxClient;
        rcDraw.Height = 200 - iconHeight;
        gp.DrawImage(m_hBitmap, rcDraw, 230, iconHeight, 10, 200, Gdiplus::Unit::UnitPixel);

        // ����ɫ�������Ǵ�ɫ��һ��
        rcDraw.X = 0;
        rcDraw.Y = 200;
        rcDraw.Width = data.cxClient;
        rcDraw.Height = data.cyClient - 200;
        gp.DrawImage(m_hBitmap, rcDraw, 230, 200, cxIcon - 230, cyIcon - 200, Gdiplus::Unit::UnitPixel);

        rcDraw.X = 10;
        rcDraw.Y = 80;
        rcDraw.Width = 260;
        rcDraw.Height = data.cyClient - 10 - 80;
        _draw_mid_line(gp, rcDraw);

        RECT rc = { rcDraw.X + 1, rcDraw.Y + 26, rcDraw.GetRight() - 1, rcDraw.GetBottom() - 1 };
        FillRect(hdc, (LPRECT)&rc, pData->draw.color.hbrBackGround);

        rcDraw.X = 280;
        rcDraw.Y = 80;
        rcDraw.Width = data.cxClient - 290;
        rcDraw.Height = data.cyClient - 10 - 80;
        _draw_mid_line(gp, rcDraw);

        rc = { rcDraw.X + 1, rcDraw.Y + 26, rcDraw.GetRight() - 1, rcDraw.GetBottom() - 1 };
        FillRect(hdc, (LPRECT)&rc, pData->draw.color.hbrBackGround);

        //BitBlt(data.md->hdc, 0, 0, data.cxClient, data.cyClient, hdc, 0, 0, SRCCOPY);
        s_info->iDraw->EndPaint(data);
        break;
    }
    //case WM_CTLCOLOR:
    //case WM_CTLCOLORMSGBOX:
    //case WM_CTLCOLOREDIT:
    //case WM_CTLCOLORLISTBOX:
    //case WM_CTLCOLORBTN:
    //case WM_CTLCOLORDLG:
    //case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
    {
        const HDC hdc = (HDC)wParam;
        const HWND hControl = (HWND)lParam;
        if (hControl != m_honeData.hStatic)
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

        LPOBJSTRUCT pCtlData = s_info->pfnGetDataFromHwnd(hControl);
        if (!pCtlData)
            return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);

        if (pCtlData->draw.isTransparent)
            SetBkMode(hdc, TRANSPARENT);
        SetBkColor(hdc, pCtlData->draw.color.crBackGround);
        SetTextColor(hdc, pCtlData->draw.color.crText);
        return (LRESULT)pCtlData->draw.color.hbrBackGround;
    }
    case WM_ERASEBKGND:
        return 1;
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}


inline HONE_PAGE_BUTTON& _get_btn_info(HWND hWnd)
{
    int index = 0;
    if (hWnd == m_honeData.hNew)            index = HONE_BTN_NEW;
    else if (hWnd == m_honeData.hOpen)      index = HONE_BTN_OPEN;
    else if (hWnd == m_honeData.hLately)    index = HONE_BTN_LATELY;
    else if (hWnd == m_honeData.hClear)     index = HONE_BTN_CLEAR;
    return m_button[index];
}

// ��ʼҳ�µ� �½�/��/���/��� �⼸����ť�����໯����
LRESULT CALLBACK WndProc_MDIClient_EHomePage_Button(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT data;
        HDC hdc = s_info->iDraw->BeginPaint(pData, data, false, false);

        HONE_PAGE_BUTTON& btn = _get_btn_info(hWnd);
        HBRUSH hbr = 0;
        if (btn.state == STATE::DOWN)
            hbr = pData->draw.color.hbrDown;
        else if (btn.state == STATE::HOT)
            hbr = pData->draw.color.hbrHot;
        else
            hbr = pData->draw.color.hbrBackGround;

        FillRect(hdc, &data.rc, hbr);
        COLORREF crText = IsWindowEnabled(hWnd) ? RGB(0, 136, 255) : RGB(83, 83, 85);
        SetTextColor(hdc, crText);
        if (btn.state == STATE::DOWN)
            OffsetRect(&data.rc, 1, 1);
        DrawTextW(hdc, btn.name, -1, &data.rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

        s_info->iDraw->EndPaint(data);
        break;
    }
    case WM_MOUSEMOVE:
    {
        HONE_PAGE_BUTTON& btn = _get_btn_info(hWnd);
        if (btn.state != STATE::HOT)
        {
            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hWnd;
            tme.dwHoverTime = INFINITE;
            TrackMouseEvent(&tme);
            btn.state = (wParam & MK_LBUTTON) ? STATE::DOWN : STATE::HOT;
           ;
            InvalidateRect(hWnd, 0, 0);
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_MOUSELEAVE:
    {
        HONE_PAGE_BUTTON& btn = _get_btn_info(hWnd);
        if (btn.state != STATE::NORMAL)
        {
            btn.state = STATE::NORMAL;
            InvalidateRect(hWnd, 0, 0);
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONDOWN:
    {
        HONE_PAGE_BUTTON& btn = _get_btn_info(hWnd);
        btn.state = STATE::DOWN;
        InvalidateRect(hWnd, 0, 0);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONUP:
    {
        RECT rc;
        POINT pt;
        GetWindowRect(hWnd, &rc);
        GetCursorPos(&pt);
        HONE_PAGE_BUTTON& btn = _get_btn_info(hWnd);
        btn.state = PtInRect(&rc, pt) ? STATE::HOT : STATE::NORMAL;
        InvalidateRect(hWnd, 0, 0);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_ERASEBKGND:
        return 1;
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}
