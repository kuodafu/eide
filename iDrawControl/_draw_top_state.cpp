#include "_draw_top_state.h"



const int ImageOffset = 2;
const int buttonwidth = 18;     // ��ť�滭���
const int buttonheight = 18;    // ��ť�滭�߶�
static HWND s_currentStateWindow;           // ��¼��ǰ����Ĵ���
static HWND hControl;
static HWND hWork;
static HWND hState;

// ��ť������
// hWnd = �����¼��Ĵ��ھ��
// info = �滭����
// index = �ڼ�����ť�����ĵ���¼�
LRESULT CALLBACK WndProc_Top_State_OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void WndProc_Top_State_InvalidateItem(HWND hWnd, LPTOPSTATE_INFO info, int index);
void WndProc_Top_State_TrackMouse(HWND hWnd);
// ������Ҫ�滭״̬�������໯��ָ������ص�����, �ڲ�����滭�͵���¼�
LRESULT CALLBACK WndProc_Top_State(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_Top_State_OnMenuSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// ��ť������
// hWnd = �����¼��Ĵ��ھ��
// info = �滭����
// index = �ڼ�����ť�����ĵ���¼�
void WndProc_Top_State_OnButtonClick(HWND hWnd, LPTOPSTATE_INFO info, TOP_STATE_BUTTON btnType, LPOBJSTRUCT pData);


LRESULT CALLBACK WndProc_Top_State_OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
void calc_button_pos(LPTOPSTATE_INFO info, HWND hWnd);


inline TOP_STATE_BUTTON index2enum(int index)
{
    switch (index)
    {
    case 0: return TOP_STATE_BUTTON_CLOSE;
    case 1: return TOP_STATE_BUTTON_PUSHPIN;
    case 2: return TOP_STATE_BUTTON_TRIANGLE;
    case 3: return TOP_STATE_BUTTON_SWITCH;
    case 4: return TOP_STATE_BUTTON_MAXBOX;
    default: return TOP_STATE_BUTTON_UNKNOWN;
    }
}
inline int enum2index(int nEnum)
{
    switch (nEnum)
    {
    case TOP_STATE_BUTTON_CLOSE:    return 0;
    case TOP_STATE_BUTTON_PUSHPIN:  return 1;
    case TOP_STATE_BUTTON_TRIANGLE: return 2;
    case TOP_STATE_BUTTON_SWITCH:   return 3;
    case TOP_STATE_BUTTON_MAXBOX:   return 4;
    default: return -1;
    }
}

// ���໯��Ҫ����״̬����
LPOBJSTRUCT _subclass_top_state(HWND hWnd, const RECT& rcOffset)
{
    LPOBJSTRUCT pData = s_info->iDraw->_subclass_tool(hWnd, WndProc_Top_State, true, false, 0);
    if (!pData)return 0;

    pData->draw.hCanvas = _canvas_create(pData->hWnd, 0, 0);
    hControl    = s_info->eWnd->GetControl();
    hWork       = s_info->eWnd->GetWork();
    hState      = s_info->eWnd->GetState();

    pData->SetClsLong(pData->hWnd, GCL_STYLE, CS_VREDRAW | CS_HREDRAW);

    //{
    //    DWORD style = pData->GetClsLong(pData->hWnd, GCL_STYLE);
    //    //style &= ~(CS_VREDRAW | CS_HREDRAW);
    //    //style |= (CS_VREDRAW | CS_HREDRAW);
    //    style |= (CS_PARENTDC | CS_SAVEBITS);
    //    pData->SetClsLong(pData->hWnd, GCL_STYLE, style);
    //}



    HWND hWndTab = 0;
    wstr title;
    LPTOPSTATE_INFO info = new TOPSTATE_INFO;
    memset(info, 0, sizeof(TOPSTATE_INFO));
    int flags = 0;
    if (hWnd == hControl)
    {
        //title = L"�����";
        hWndTab = s_info->hWndTabControl;
        info->flags = s_info->config->tab_control_flags();
        flags = info->flags;
        __add_flags(info, TOP_STATE_BUTTON_SWITCH); // ���������ѡ��ť״̬
    }
    else if (hWnd == hState)
    {
        hWndTab = s_info->eWnd->GetStateTab();
        info->flags = s_info->config->tab_state_flags();
        flags = info->flags;
    }
    else if (hWnd == hWork)
    {
        hWndTab = s_info->eWnd->GetWorkTab();
        info->flags = s_info->config->tab_work_flags();
        flags = info->flags;
    }

    __add_flags(info, TOP_STATE_BUTTON_DEF);

    bool isAutoHide = __query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE);  // ������Զ�����, �Ǿ���Ҫѡ��ͼ��
    bool isFloat = __query_flags(info, TOP_STATE_BUTTON_ISFLOAT);
    if (isFloat)    // ��������, ����󻯻��߻�ԭ��ť, ��Ҫɾ��ͼ��ͼ��
    {
        info->isRestore = __query_flags(info, TOP_STATE_BUTTON_RESTORE);
        // ������󻯰�ť״̬
        __add_flags(info, TOP_STATE_BUTTON_MAXBOX);
        // ȥ��ͣ��״̬, ͼ����ť
        __del_flags(info, TOP_STATE_BUTTON_ISSTOP | TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_PUSHPIN_SEL);
    }
    else
    {
        // ���Ǹ�������, ����ͣ��״̬, ͼ����ť
        __add_flags(info, TOP_STATE_BUTTON_ISSTOP | TOP_STATE_BUTTON_PUSHPIN);
        // ȥ������/���/��ԭ ״̬
        __del_flags(info, TOP_STATE_BUTTON_ISFLOAT | TOP_STATE_BUTTON_MAXBOX | TOP_STATE_BUTTON_RESTORE);
    }
    if (isAutoHide) // �����������ȼ����Զ����ظ�, �и���״̬ʱ�������Զ�����
    {

        ShowWindow(hWnd, SW_HIDE);  // ���Զ����صĻ�, �����Ͱ������ص�
        //info->flags |= (TOP_STATE_BUTTON_ISAOTUHIDE | TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_PUSHPIN_SEL);
        __add_flags(info, TOP_STATE_BUTTON_PUSHPIN_SEL | TOP_STATE_BUTTON_ISAOTUHIDE);        // ����ͼ��״̬
        if (!isFloat)
            __add_flags(info, TOP_STATE_BUTTON_PUSHPIN_SEL);    // ����ͼ��ѡ��״̬
    }

    std::vector<int>* pArr = Tab_GetArr(s_info->pfnGetDataFromHwnd(hWndTab), 0, 0);
    _drawtab_getarr(pData, pArr, true);
    _drawtab_gettopstate_info(pData, info, true);
    info->index = -1;
    info->hWnd = hWnd;

    info->rcOffset = rcOffset;
    info->isSelect = isAutoHide;
    calc_button_pos(info, hWnd);
    if (flags != info->flags)   // �õ��Ľ����ͬ, ��Ҫд������
    {
        if (hWnd == hControl)
        {
            s_info->config->tab_control_flags(&info->flags);
        }
        else if (hWnd == hState)
        {
            s_info->config->tab_state_flags(&info->flags);
        }
        else if (hWnd == hWork)
        {
            s_info->config->tab_work_flags(&info->flags);
        }
    }
    if (hWndTab && pArr)
    {
        const int ItemCount = TabCtrl_GetItemCount(hWndTab);   // ѡ�����Ŀ��
        const int bufSize = 0x1000;
        wchar_t text[bufSize] = { 0 };
        TCITEMW item = { 0 };
        item.mask = TCIF_TEXT;
        const int fmt = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
        float width = 0, height = 0;
        int indexTab = TabCtrl_GetCurSel(hWndTab);
        if ((int)pArr->size() != ItemCount)
            pArr->resize(ItemCount);
        for (int i = 0; i < ItemCount; i++)
        {
            item.pszText = text;
            item.cchTextMax = bufSize;
            TabCtrl_GetItem(hWndTab, i, &item);
            _canvas_calctextsize(s_info->window_d2d_info.hCanvas, pData->draw.hFont, text, -1, fmt, 80, 20, &width, &height);
            int newWidth = (int)width + 10;
            int& arrWidth = pArr->at(i);
            if (arrWidth != newWidth)
                arrWidth = newWidth;
            if (i == indexTab)
                title = text;
        }
    }
    if (!title.empty())
        wcscpy_s(info->name, 20, title.c_str());
    return pData;
}

void calc_button_pos(LPTOPSTATE_INFO info, HWND hWnd)
{
    RECT& rc = info->rcWindow;
    GetClientRect(hWnd, &rc);
    rc.left     += info->rcOffset.left;
    rc.top      += info->rcOffset.top;
    rc.right    += info->rcOffset.right;
    rc.bottom   += info->rcOffset.bottom;

    const int cxItem = rc.right - rc.left;
    const int cyItem = s_info->scale(WINDOW_TOP_STATE);
    RECT rcDraw = { 0, 0, cxItem, cyItem };
    const int _width = s_info->scale(buttonwidth);
    const int _height = s_info->scale(buttonheight);

    int left = 0;
    int top = (cyItem - _height) / 2;
    int index = 0;

    if (__query_flags(info, TOP_STATE_BUTTON_CLOSE))     // �йرհ�ť
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[0] = { left, top, left + _width, top + _height };
        index++;
    }
    if (__query_flags(info, TOP_STATE_BUTTON_MAXBOX))    // ����󻯰�ť
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[4] = { left, top, left + _width, top + _height };
        index++;
    }
    if (__query_flags(info, TOP_STATE_BUTTON_PUSHPIN))   // ��ͼ����ť
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[1] = { left, top, left + _width, top + _height };
        index++;
    }

    if (__query_flags(info, TOP_STATE_BUTTON_TRIANGLE))  // �����ǰ�ť
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[2] = { left, top, left + _width, top + _height };
        index++;
    }
    if (__query_flags(info, TOP_STATE_BUTTON_SWITCH))    // ���л���ť
    {
        left = rcDraw.right - (_width * (index + 1)) - 4;
        info->rc[3] = { left, top, left + _width, top + _height };
        index++;
    }

    info->count = index;
    return;
}


// �滭С��ť
// gp = gdi+�滭ͼ��
// rc = ��ť��λ��, Сͼ���ǻ滭�����������, �ڲ�������������������
// index = �滭�ڼ�����ť, TOP_STATE_BUTTON ö�ٳ�����ֵ
inline void _draw_icon(LPCANVAS hCanvas, const LPTOPSTATE_INFO info, TOP_STATE_BUTTON index, LPOBJSTRUCT pData)
{
    // iconIndex = ͼ����ͼƬ�е����λ��, 0=�ر�, 1=ͼ������(�̶�), 2=ͼ������(����), 3=����, 4=������л�
    LPCWSTR iconIndex;
    int infoIndex;
    if      (index == TOP_STATE_BUTTON_CLOSE)       iconIndex = L"0", infoIndex = 0;
    else if (index == TOP_STATE_BUTTON_PUSHPIN)     iconIndex = L"1", infoIndex = 1;
    else if (index == TOP_STATE_BUTTON_TRIANGLE)    iconIndex = L"3", infoIndex = 2;
    else if (index == TOP_STATE_BUTTON_MAXBOX)      iconIndex = L"5", infoIndex = 4;
    else iconIndex = L"4", infoIndex = 3;
    if (index == TOP_STATE_BUTTON_PUSHPIN && info->isSelect)        iconIndex = L"2";
    else if (index == TOP_STATE_BUTTON_MAXBOX && info->isRestore)   iconIndex = L"6";
    // iconIndex ��������Ѿ���ź��˸û滭��ͼ������


    const RECT& rc = info->rc[infoIndex];
    const int cxIcon = s_info->scale(10);
    const int cyIcon = s_info->scale(10);
    const int cxItem = rc.right - rc.left;
    const int cyItem = rc.bottom - rc.top;

    if (info->index == infoIndex)
    {
        RECT_F rcBack(rc);

        // ��Ҫ�ж���û��״̬, ��״̬��Ҫ���һ�±���
        LPEX_BRUSH hbrBack = 0;
        if (info->state == STATE::HOT)
        {
            hbrBack = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));
        }
        else if (info->state == STATE::DOWN)
        {
            hbrBack = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crDown, 255));
        }
        if (hbrBack)
            hCanvas->FillRectangle(hbrBack, &rcBack);

    }

    RECT_F rcDest;
    rcDest.left     = (float)(int)(rc.left + (cxItem - cxIcon) / 2);
    rcDest.top      = (float)(int)(rc.top  + (cyItem - cyIcon) / 2);
    rcDest.right    = (float)(int)((rcDest.left + (float)cxIcon));
    rcDest.bottom   = (float)(int)((rcDest.top  + (float)cyIcon));

    RECT_F rcIconDraw;
    LPWINDOW_ICON_DATA iconData = s_info->pResource->TopState_GetIcon(iconIndex, &rcIconDraw, STATE::NORMAL, s_info->dpiX);
    if (iconData)
    {
        hCanvas->DrawImage(iconData->hBitmapDx, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom,
            rcIconDraw.left, rcIconDraw.top, rcIconDraw.right, rcIconDraw.bottom, 255);
    }
}

// ����һ���滭ͼ��, �滭λ��, �����ɫ �ڲ������hdc�ϻ滭��������+��䱳��+�滭�ı�
// hCanvas = �滭ͼ��
// rcDraw = �滭�ķ�Χ, �ڲ�����������λ滭, ���һ���Ƕ���0,0 ��ʼ�� �ұ�,WINDOW_TOP_STATE ��λ��
// pData = 
// rcClient = ���ڵĿͻ��� GetClientRect ��ֵ
void _draw_top_state(LPCANVAS hCanvas, RECT& rcDraw, LPOBJSTRUCT pData, const RECT& rcClient)
{
    hCanvas->SetAntialiasMode(0);
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    s_info->themeColor->crStatelineNormal();
    ARGB crDot, crBack, crText;
    if (s_currentStateWindow == pData->hWnd)
    {
        crDot = s_info->themeColor->crStatelineHot();   crDot = RGB2ARGB(crDot, 255);
        crBack = s_info->themeColor->crStateBar();      crBack = RGB2ARGB(crBack, 255);
        crText = RGB2ARGB(pData->draw.color.crTextHot, 255);
    }
    else
    {
        crDot = s_info->themeColor->crStatelineNormal();    crDot = RGB2ARGB(crDot, 255);
        crBack = RGB2ARGB(pData->draw.color.crBackGround, 255);
        crText = RGB2ARGB(pData->draw.color.crText, 255);
    }

    const bool isTransverse = true;

    LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    calc_button_pos(info, pData->hWnd);

    LPEX_BRUSH hbrBack = s_info->pfnCreateBrushDx(crBack);
    LPEX_BRUSH hbrText = s_info->pfnCreateBrushDx(crText);
    LPEX_PEN hPenDot = s_info->pfnCreatePenDx(crDot, 1);
    LPEX_PEN hPenBorder = s_info->pfnCreatePenDx(RGB2ARGB(pData->draw.color.crBorder, 255), 1);
    //LPEX_PEN hPenBorder = s_info->pfnCreatePenDx(RGB2ARGB(255, 255), 1);

    if (pData->hWnd == s_info->hState)
    {
        rcDraw.top += WINDOW_SPACING;
        rcDraw.bottom += WINDOW_SPACING;

    }
    for (int i = 0; i < sizeof(info->rc) / sizeof(info->rc[0]); i++)
    {
        if (info->rc[i].bottom)
        {
            if (pData->hWnd == s_info->hState)
            {
                info->rc[i].top += WINDOW_SPACING;
                info->rc[i].bottom += WINDOW_SPACING;
            }
            if (info->rcWindow.left)
            {
                info->rc[i].left += info->rcWindow.left;
                info->rc[i].right += info->rcWindow.left;
            }
        }
    }
    //wstr::dbg(L"0x%p(%d) rcDraw = {%d, %d, %d, %d} (%d * %d), info->rcWindow = {%d, %d, %d, %d} (%d * %d)\n",
    //    pData->hWnd, pData->hWnd,
    //    rcDraw, rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
    //    info->rcWindow, info->rcWindow.right - info->rcWindow.left, info->rcWindow.bottom - info->rcWindow.top);

    rcDraw.left = info->rcWindow.left;
    rcDraw.right = info->rcWindow.right;


    RECT_F rcFill(rcDraw);
    
    hCanvas->FillRectangle(hbrBack, &rcFill);

    rcFill.bottom = (float)rcClient.bottom - 1;
    rcFill.left++;
    rcFill.top++;
    hCanvas->DrawRectangle(hPenBorder, &rcFill);

    RECT_F rcText(0, 0, 12, 0);
    RECT_F rcOutput;
    if (info->name && info->name[0])
    {
        rcText.left     = (float)rcDraw.left + 3;
        rcText.top      = (float)rcDraw.top;
        rcText.bottom   = (float)(rcDraw.bottom);
        rcText.right    = (float)rcDraw.right;

        const int fmt = DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX;
        hCanvas->drawtextex(hFont, hbrText, info->name, -1, fmt, &rcText, 0, 0, 0, 0, &rcOutput);
    }

    if (__query_flags(info, TOP_STATE_BUTTON_CLOSE))     // �йرհ�ť
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_CLOSE, pData);
    if (__query_flags(info, TOP_STATE_BUTTON_MAXBOX))    // ����󻯰�ť
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_MAXBOX, pData);
    if (__query_flags(info, TOP_STATE_BUTTON_PUSHPIN))   // ��ͼ����ť
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_PUSHPIN, pData);
    if (__query_flags(info, TOP_STATE_BUTTON_TRIANGLE))  // �����ǰ�ť
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_TRIANGLE, pData);
    if (__query_flags(info, TOP_STATE_BUTTON_SWITCH))    // ���л���ť
        _draw_icon(hCanvas, info, TOP_STATE_BUTTON_SWITCH, pData);


    RECT_F rcDot;
    int width  = rcDraw.right  - rcDraw.left;
    int height = rcDraw.bottom - rcDraw.top;

    rcDot.left      = (float)rcDraw.left + (rcOutput.width() + s_info->scale(6));
    rcDot.top       = (float)rcDraw.top  + (((float)(height - s_info->scale(5))) / 2) + 1;
    rcDot.right     = (float)(info->rcWindow.right - (s_info->scale(buttonwidth) * info->count) - s_info->scale(5));
    rcDot.bottom    = rcDot.top + s_info->scale(5);
    _draw_top_state_dot(hCanvas, rcDot, crDot, isTransverse);

}


// ����һ���滭ͼ��, �滭λ��, �����ɫ �ڲ������hdc�ϻ滭��������
// hCanvas = �滭ͼ��
// rcDraw = �滭�ķ�Χ, �ڲ�����������λ滭
// crDot = �����ɫ
// isTransverse = �Ƿ����, Ϊ true ��������һ滭, false ��������»滭
void _draw_top_state_dot(LPCANVAS hCanvas, const RECT_F& rcDraw, ARGB crDot, bool isTransverse)
{
    //const float dashArray[] = { (float)s_info->scale(1), (float)s_info->scale(3) };
    const float dashArray[] = { 1.f, 3.f };
    LPEX_PEN hPen = _pen_create(crDot, (float)s_info->scale(1), 5, dashArray, sizeof(dashArray) / sizeof(dashArray[0]));
    LINEPOINT_F pt[3];
    const float x1 = rcDraw.left;
    const float y1 = rcDraw.top;
    const float x2 = rcDraw.right;
    const float y2 = rcDraw.bottom;

    const int n0 = 0;
    const int n2 = s_info->scale(2);
    const int n4 = s_info->scale(4);

    if (isTransverse)
    {
        pt[0](x1 + n0, y1 + n0, x2 - n0, y1 + n0);  // ����
        pt[1](x1 + n2, y1 + n2, x2 - n2, y1 + n2);  // �м�
        pt[2](x1 + n0, y1 + n4, x2 - n0, y1 + n4);  // ����

        //gp->DrawLine(&pen, x1 + 0, y1 + 0, x2 - 0, y1 + 0);  // ����
        //gp->DrawLine(&pen, x1 + 2, y1 + 2, x2 - 2, y1 + 2);  // �м�
        //gp->DrawLine(&pen, x1 + 0, y1 + 4, x2 - 0, y1 + 4);  // ����
    }
    else
    {
        // �ߵ����������滭, �������»滭
        pt[0](x1 + n0, y1 + n0, x1 - n0, y2 + n0);  // ���
        pt[1](x1 + n2, y1 + n2, x1 + n2, y2 - n2);  // �м�
        pt[2](x1 + n4, y1 + n0, x1 + n4, y2 - n0);  // �ұ�

        //gp->DrawLine(&pen, x1 + 0, y1 + 0, x1 - 0, y2 + 0);  // ���
        //gp->DrawLine(&pen, x1 + 2, y1 + 2, x1 + 2, y2 - 2);  // �м�
        //gp->DrawLine(&pen, x1 + 4, y1 + 0, x1 + 4, y2 - 0);  // �ұ�
    }
    hCanvas->DrawLine(hPen, &pt[0]);
    hCanvas->DrawLine(hPen, &pt[1]);
    hCanvas->DrawLine(hPen, &pt[2]);
    _pen_destroy(hPen);
}


// ���µ�ǰ����Ĵ���, ���ػ������������ں�֮ǰ����Ĵ���
void TopState_UpdateStateWindow(HWND hWnd)
{
    HWND hWndClick = hWnd;

    while (hWnd && hWnd != hControl && hWnd != hWork && hWnd != hState)
    {
        hWnd = (HWND)GetWindowLongPtrW(hWnd, GWLP_HWNDPARENT);
    }
    if (!hWnd)
    {
        hWnd = hWndClick;
        while (hWnd && hWnd != hControl && hWnd != hWork && hWnd != hState)
        {
            hWnd = GetParent(hWnd);
        }
    }
    if (s_currentStateWindow != hWnd)
    {
        if (!hWnd && hWndClick)
        {
            wchar_t cls[260];
            GetClassNameW(hWndClick, cls, 260);
            if (wcscmp(cls, L"ComboLBox") == 0) // �������Ͽ���б��򲻴���
                return;
        }

        if (hWnd && (hWnd == hControl || hWnd == hWork || hWnd == hState))
        {
            const bool isLButton = (GetAsyncKeyState(VK_LBUTTON) & 0xFF00) > 0;
            if (isLButton)
            {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hWnd, &pt);
                LPOBJSTRUCT pData = GetDataFromHwnd(hWnd);
                LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
                if (info)
                {
                    const int count = sizeof(info->rc) / sizeof(info->rc[0]);
                    int min = 0x7fffffff;
                    for (int i = 0; i < count; i++)
                    {
                        //if (info->rc[i].left < min)
                        //    min = info->rc[i].left;
                        if (PtInRect(&info->rc[i], pt)) // �ڰ�ť��, ������
                            return;
                    }
                }
            }
        }

        HWND hOld = s_currentStateWindow;
        if (s_currentStateWindow)
            InvalidateRect(s_currentStateWindow, 0, 0);
        s_currentStateWindow = hWnd;
        if (s_currentStateWindow)
            InvalidateRect(s_currentStateWindow, 0, 0);
        s_info->pfnMove(INE_STATE_UPDATEHWND, (WPARAM)hOld, (LPARAM)s_currentStateWindow);
    }
}
HWND TopState_GetActivateWindow()
{
    return s_currentStateWindow;
}
void TopState_SetWindowText(HWND hWnd, LPCWSTR lpszText, bool isUpdate)
{
    LPOBJSTRUCT pData = GetDataFromHwnd(hWnd);
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    if (!info)return;
    if (!lpszText || !lpszText[0])
    {
        info->name[0] = 0;
    }
    else
    {
        size_t len = wcslen(lpszText) + 1;
        if (len >= 20)len = 20;
        memcpy(info->name, lpszText, len * sizeof(wchar_t));
    }
    if (isUpdate)
        InvalidateRect(hWnd, 0, 0);
}


inline static void CALLBACK TimerProc_LeaveState(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    LPOBJSTRUCT pData = (LPOBJSTRUCT)id;
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    if (info->isDown)   // ����״̬������
        return;
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);

    for (int i = 0; i < sizeof(info->rc) / sizeof(info->rc[0]); i++)
    {
        if (PtInRect(&info->rc[i], pt))
        {
            return;
        }
    }

    if (info->index != -1)
    {
        info->index = -1;
        info->state = STATE::HOT;
        InvalidateRect(hWnd, 0, 0);
    }
    KillTimer(hWnd, id);
}

LRESULT CALLBACK WndProc_Top_State(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    //wstr::dbg(L"�����Ϣ, ��Ϣֵ = %d\n", message);
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    if (!info)
        return DefWindowProcW(hWnd, message, wParam, lParam);

    switch (message)
    {
    case WM_COMMAND:
    {
        if (HIWORD(wParam) == 0 && lParam == 0)
        {
            return WndProc_Top_State_OnMenuSelect(hWnd, message, wParam, lParam, pData);
        }
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_PAINT:
    {
        BEGIN_END_PAINT_STRUCT ps;
        s_info->iDraw->BeginPaint(pData, ps, true, true);
        RECT rcDraw = { 0, 0, ps.cxClient, s_info->scale(WINDOW_TOP_STATE) };
        _draw_top_state(pData->draw.hCanvas, rcDraw, pData, ps.rc);
        s_info->iDraw->EndPaint(ps);
        break;
    }
    case WM_NCHITTEST:
    {
        const int x = GET_X_LPARAM(lParam);
        const int y = GET_Y_LPARAM(lParam);
        RECT rc;
        GetWindowRect(hWnd, &rc);
        if (hWnd == s_info->hState)         // ״̬�����ƶ�����
        {
            if (y < rc.top + WINDOW_SPACING)
                return HTTOP;
        }
        else if (hWnd == s_info->hWork)     // �������ƶ��ұ�
        {
            if (x > rc.right - WINDOW_SPACING)
                return HTRIGHT;
        }
        else if (hWnd == s_info->hControl)  // ������ƶ����
        {
            if (x < rc.left + WINDOW_SPACING)
                return HTLEFT;
        }
        return HTCLIENT;
    }
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    {
        const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        const int left = info->rc->right - (s_info->scale(buttonwidth) * info->count + 5);
        SetCapture(hWnd);
        if (pt.x < left)
        {
            info->isDownTitle = true;
            // ����ڼ�����ť�����, ���������Ҫ�ƶ�������
            return _draw_top_state_lbuttondown(pData, info, wParam, pt.x, pt.y);
        }
        info->isDownTitle = false;
        info->isDown = true;
        info->state = STATE::DOWN;
        WndProc_Top_State_InvalidateItem(hWnd, info, info->index);
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    case WM_LBUTTONUP:
    {
        if (info->isDownTitle)
            return _draw_top_state_lbuttonup(pData, info, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        ReleaseCapture();
        break;
    }
    case WM_CAPTURECHANGED:
    {
        if (info->isDownTitle)
        {
            info->isDownTitle = false;
            // ���ǰ���״̬�Ǿ����ƶ����ڽ���
            return _draw_top_state_capture_changed(pData, info, wParam, lParam);
        }
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);

        if (PtInRect(&info->rc[info->index], pt))  // �ſ�ʱ��껹��ԭ������Ŀ��
        {
            info->state = STATE::HOT;
            WndProc_Top_State_OnButtonClick(hWnd, info, index2enum(info->index), pData);
        }
        else
        {
            // ���ſ�����ԭ������Ŀ��
            //info->indexHot = -1;
            info->state = STATE::NORMAL;
        }

        WndProc_Top_State_InvalidateItem(hWnd, info, info->index);   // �ػ�����Ŀ
        info->index = -1;
        info->isDown = false;
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (info->isDownTitle)
        {
            // && (wParam & MK_LBUTTON) == MK_LBUTTON
            return _draw_top_state_move(pData, info, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        return WndProc_Top_State_OnMouseMove(hWnd, message, wParam, lParam, pData);
    }
    case WM_DESTROY:
    {
        if (info)delete info;
        _drawtab_getarr(pData, 0, true);
        _drawtab_gettopstate_info(pData, 0, true);
        _canvas_destroy(pData->draw.hCanvas);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_SIZE:
    {
        calc_button_pos(info, hWnd);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_NCCALCSIZE:
    {
        if (wParam)
        {
            LPNCCALCSIZE_PARAMS cs = (LPNCCALCSIZE_PARAMS)lParam;
            memcpy(&cs->rgrc[1], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            memcpy(&cs->rgrc[2], &cs->rgrc[0], sizeof(cs->rgrc[0]));
            return WVR_ALIGNTOP | WVR_ALIGNLEFT | WVR_ALIGNBOTTOM | WVR_ALIGNRIGHT;

            return WVR_REDRAW;
        }
        LPRECT prc = (LPRECT)lParam;
        return 0;
    }
    case WM_GETMINMAXINFO:
    {
        // TODO ��Ҫ���ݵ�ǰ����λ�þ��������Сλ��
        LPMINMAXINFO pos = (LPMINMAXINFO)lParam;
        if (hWnd == s_info->hControl)   // �������С���min, ����Ȳ��ܴ���MDI���ڵ����+mdi
        {
            // �����������С�ߴ�
            pos->ptMinTrackSize.x = WINDOW_MINTRACKSIZE;    // ��С�϶����
            const int x = s_info->rcMDIClient.left + WINDOW_MIN_MDIWIDTH;
            const int width = s_info->rcControl.right - x;
            pos->ptMaxTrackSize.x = width;  // ����϶����
            if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
                pos->ptMinTrackSize.x = WINDOW_AUTO_HEIGHT + WINDOW_SPACING;
        }
        else if (hWnd == s_info->hState)    // ״̬��, ��С�߶�min, ���߶Ȳ��ܴ���MDI���ڵĶ���+mdi
        {
            pos->ptMinTrackSize.y = WINDOW_MINTRACKSIZE;    // ��С�϶��߶�
            const int y = s_info->rcMDIClient.top + WINDOW_MIN_MDIHEIGHT;    // ������MDI���ڵĶ���+mdi
            const int height = s_info->rcState.bottom - y;  // ���߶� = ״̬�еĵױ� - MDI����+mdi
            pos->ptMaxTrackSize.y = height; // ����϶��߶�
            // ���Զ�������С�߶Ⱦ����ó�״̬���߶�+�ָ����߶�
            if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
            {
                pos->ptMinTrackSize.y = 75;     // WINDOW_TOP_STATE + WINDOW_SPACING;
                pos->ptMaxTrackSize.y = s_info->rcState1.bottom - y; // ����϶��߶�
            }
        }
        else if (hWnd == s_info->hWork)     // ������, ��С��� min, ����Ȳ��ܴ���MDI���ڵ��ұ�-mdi
        {
            pos->ptMinTrackSize.x = WINDOW_MINTRACKSIZE;    // ��С�϶����
            pos->ptMaxTrackSize.x = s_info->rcMDIClient.right - WINDOW_MIN_MDIWIDTH; // ����϶����

            if (__query_flags(info, TOP_STATE_BUTTON_ISAOTUHIDE))
                pos->ptMinTrackSize.x = 120;
        }

        return 0;
    }
    case WM_NCLBUTTONDOWN:
    {
        SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREDRAW);
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_ERASEBKGND:
        return 1;
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        return WndProc_CtlColor(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}


void WndProc_Top_State_InvalidateItem(HWND hWnd, LPTOPSTATE_INFO info, int index)
{
    if (index == -1) return;
    InvalidateRect(hWnd, &info->rc[index], 0);
}
void WndProc_Top_State_TrackMouse(HWND hWnd)
{
    TRACKMOUSEEVENT tme = { 0 };
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = hWnd;
    tme.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&tme);
}

LRESULT CALLBACK WndProc_Top_State_OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    if ((wParam & MK_LBUTTON) == MK_LBUTTON || (wParam & MK_RBUTTON) == MK_RBUTTON || !info)
    {
        return 0;  // ����Ҽ�����ʱ������
    }

    const POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    // �ߵ����������ĳ����Ŀ��
    for (int i = 0; i < sizeof(info->rc) / sizeof(info->rc[0]); i++)
    {
        if (PtInRect(&info->rc[i], pt))  // ��ĳ����Ŀ��
        {
            WndProc_Top_State_TrackMouse(hWnd);
            if (info->index == i)
                return 0;   // ��껹������һ�εİ�ť��
            // �ߵ���Ͳ�������һ�εİ�ť����, ������Ҫ�����µ��ȵ㰴ť����, ��Ҫ�ػ���ť

            WndProc_Top_State_InvalidateItem(hWnd, info, info->index);
            info->index = i;
            info->state = STATE::HOT;
            WndProc_Top_State_InvalidateItem(hWnd, info, info->index);
            SetTimer(hWnd, (UINT_PTR)pData, 50, TimerProc_LeaveState);
            return 0;
        }
    }

    // �ߵ�������ǲ������κ���Ŀ��, ��Ҫ�ػ���ǰ�ȵ���
    if (info->index != -1)
    {
        WndProc_Top_State_InvalidateItem(hWnd, info, info->index);
        info->index = -1;
        info->state = STATE::NORMAL;
    }
    return 0;
}

// ��ť������
// hWnd = �����¼��Ĵ��ھ��
// info = �滭����
// btnType = �ĸ���ť�����, �ڴ�������¼���ʱ�����ֵ�ǵ�����־λ
void WndProc_Top_State_OnButtonClick(HWND hWnd, LPTOPSTATE_INFO info, TOP_STATE_BUTTON btnType, LPOBJSTRUCT pData)
{
    if (btnType == TOP_STATE_BUTTON_PUSHPIN)
    {
        info->isSelect = !info->isSelect;
        // ѡ�оͼ����Զ����غ�ͼ��ѡ��״̬
        if (info->isSelect)
        {
            info->flags |= (TOP_STATE_BUTTON_ISAOTUHIDE |  TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_PUSHPIN_SEL);
        }
        else
        {
            info->flags &= ~(TOP_STATE_BUTTON_ISAOTUHIDE | TOP_STATE_BUTTON_PUSHPIN_SEL);
        }

        
    }
    else if (btnType == TOP_STATE_BUTTON_MAXBOX)
    {
        info->isRestore = !info->isRestore;
        info->flags |= (TOP_STATE_BUTTON_ISFLOAT | TOP_STATE_BUTTON_MAXBOX);
        if (info->isRestore)
        {
            info->flags |= (TOP_STATE_BUTTON_RESTORE);
        }
        else
        {
            info->flags &= ~(TOP_STATE_BUTTON_RESTORE);
        }
    }


    WPARAM wParam = 0;
    switch (btnType)
    {
    case TOP_STATE_BUTTON_TRIANGLE: // ���ǰ�ť�����
    case TOP_STATE_BUTTON_CLOSE:    // �رհ�ť�����
        wParam = btnType;
        break;
    case TOP_STATE_BUTTON_PUSHPIN:  // ͼ����ť�����
    case TOP_STATE_BUTTON_MAXBOX:   // ��󻯰�ť�����
    {
        // ͼ����ť/��󻯰�ť ���������Ҫ�ı�����
        if (info->hWnd == hControl)
        {
            s_info->config->tab_control_flags(&info->flags);
        }
        else if (info->hWnd == hState)
        {
            s_info->config->tab_state_flags(&info->flags);
        }
        else if (info->hWnd == hWork)
        {
            s_info->config->tab_work_flags(&info->flags);
        }
        wParam = btnType;
        break;
    }
    case TOP_STATE_BUTTON_SWITCH:   // ������л���ʾЧ��
    {
        // ���һ��ֻ���������
        LPOBJSTRUCT data = GetDataFromHwnd(s_info->eWnd->GetControl32770Parent());
        if (!data)return;
        RECT rc;
        GetWindowRect(data->hWnd, &rc);
        DWORD pt = MAKELONG(rc.right - 40, rc.top + 12);
        data->oldProc(data->hWnd, WM_NCLBUTTONDOWN, 1000, pt);
        data->oldProc(data->hWnd, WM_NCLBUTTONUP, 1000, pt);
        if (!IsWindowVisible(data->hWnd))
            ShowWindow(data->hWnd, SW_SHOW);
        return;
    }
    default:
        return;
    }
    info->hWnd = hWnd;
    s_info->pfnMove(INE_STATE_CLICK, wParam, (LPARAM)info);

}

LRESULT CALLBACK WndProc_Top_State_OnMenuSelect(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
#define IDMENU_FLOAT            1000    // ����(&F)
#define IDMENU_STOP             1001    // ͣ��(&K)
#define IDMENU_AUTOHIDE         1002    // �Զ�����(&T)
#define IDMENU_CLOSE            1003    // �ر�(&C)
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData, 0, 0);
    const int id = LOWORD(wParam);
    switch (id)
    {
    case IDMENU_FLOAT:       // ����(&F)
    {
        // ֱ�Ӽ��ϸ���/���ͼ��, ȥ��ͼ��ͼ��, ȥ��ͣ��״̬
        __add_flags(info, TOP_STATE_BUTTON_ISFLOAT | TOP_STATE_BUTTON_MAXBOX);
        __del_flags(info, TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_ISSTOP);
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case IDMENU_STOP:        // ͣ��(&K)
    {
        // ֱ��ȥ������/���ͼ��/��ԭͼ��, ����ͼ��ͼ��, ����ͣ��״̬
        info->flags &= ~(TOP_STATE_BUTTON_ISFLOAT | TOP_STATE_BUTTON_MAXBOX);
        info->flags |= (TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_ISSTOP);
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case IDMENU_AUTOHIDE:    // �Զ�����(&T)
    {
        // ֱ������Ϊtrue, ��ѡ������˵���ԭ�������Զ�����
        // �����Զ����غ�ͼ��ѡ��״̬
        info->isSelect = true;
        info->flags |= (TOP_STATE_BUTTON_ISAOTUHIDE | TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_PUSHPIN_SEL);
        InvalidateRect(hWnd, 0, 0);
        break;
    }
    case IDMENU_CLOSE:       // �ر�(&C)
    {

        break;
    }
    default:
        return CallWindowProcA(pData->oldProc, hWnd, message, wParam, lParam);
    }


    // ���в˵����������Ҫ��������
    if (info->hWnd == hControl)
    {
        s_info->config->tab_control_flags(&info->flags);
    }
    else if (info->hWnd == hState)
    {
        s_info->config->tab_state_flags(&info->flags);
    }
    else if (info->hWnd == hWork)
    {
        s_info->config->tab_work_flags(&info->flags);
    }


    LRESULT ret = s_info->pfnAllWindowProc(hWnd, message, &wParam, &lParam, pData);
    if (pData->isReturn)
        return ret;
    return 0;
}