#include "../EWindow.h"
#include "../EWindow_Fne_Header.h"

static LPOBJSTRUCT pWork;
static LPOBJSTRUCT pState;
static LPOBJSTRUCT pControl;
static LPOBJSTRUCT pTabWork;
static LPOBJSTRUCT pTabState;
static LPOBJSTRUCT pTabControl;

static LPTOPSTATE_INFO work;
static LPTOPSTATE_INFO state;
static LPTOPSTATE_INFO control;

static LPCANVAS m_hCanvas;      // 绘画旋转文本时使用


static LPOBJSTRUCT s_pData;     // 易语言主窗口的数据
struct CUSTAB_DRAW 
{
    LPEX_BRUSH hbrText;
    LPEX_BRUSH hbrTextHot;
    LPEX_BRUSH hbrTextDown;
    LPEX_BRUSH hbrBackHot;
    LPEX_BRUSH hbrBackDown;
    LPEX_BRUSH hbrLine;
    LPEX_BRUSH hbrLineHot;
    LPEX_BRUSH hbrLineDown;

};

// hCanvas = 绘画句柄, 这个是易语言主窗口的绘画句柄
// ps = BeginPaint 得到的结构, 这个是易语言主窗口WM_PAINT得到的结构
// pData = 工作夹/状态夹的窗口数据
// pTab = 工作夹/状态夹下的选择夹窗口数据
// rc = 工作夹/选择夹的窗口位置, 这个是隐藏后在易语言主窗口中绘画的位置
// isRotate = 是否旋转, 旋转都是90度
// isLeft = 绘画文本底边的一小块矩形是否在左边
void _draw_tab(LPCANVAS hCanvas, PAINTSTRUCT& ps, LPOBJSTRUCT pData, LPOBJSTRUCT pTab, RECT& rc, bool isRotate, bool isLeft);

void _draw_tab_control  (LPCANVAS hCanvas, PAINTSTRUCT& ps);


inline HWND _draw_get_draw(LPOBJSTRUCT pData, CUSTAB_DRAW& draw)
{
    draw.hbrText        = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crText    , 255));
    draw.hbrTextHot     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crTextHot , 255));
    draw.hbrTextDown    = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crTextDown, 255));
    draw.hbrBackHot     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot     , 255));
    draw.hbrBackDown    = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crDown    , 255));
    draw.hbrLine        = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crBorder  , 255));
    draw.hbrLineHot     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crTextHot , 255));
    draw.hbrLineDown    = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crTextDown, 255));
    return pData->hWnd;
}

void EWindow_TabInit(LPOBJSTRUCT pData)
{
    s_pData = pData;
    pWork = s_info->pfnGetDataFromHwnd(s_info->hWork);
    pState = s_info->pfnGetDataFromHwnd(s_info->hState);
    pControl = s_info->pfnGetDataFromHwnd(s_info->hControl);
    if (!pWork || !pState || !pControl)
    {
        auto pfnTimerProc = [](HWND hWnd, UINT message, UINT_PTR id, DWORD timer) -> void
        {
            pWork = s_info->pfnGetDataFromHwnd(s_info->hWork);
            if (pWork)
            {
                KillTimer(hWnd, id);
                EWindow_TabInit(s_pData);
            }
        };
        SetTimer(pData->hWnd, 34567896, 100, pfnTimerProc);
        return;
    }

    pTabWork = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetWorkTab());
    pTabState = s_info->pfnGetDataFromHwnd(s_info->eWnd->GetStateTab());
    pTabControl = s_info->pfnGetDataFromHwnd(s_info->hWndTabControl);

    work = (LPTOPSTATE_INFO)pWork->param;
    state = (LPTOPSTATE_INFO)pState->param;
    control = (LPTOPSTATE_INFO)pControl->param;

    pWork->draw.indexHot = -1;
    pState->draw.indexHot = -1;
    pControl->draw.indexHot = -1;

    m_hCanvas = _canvas_create(0, 0, 0);
    InvalidateRect(s_info->hEWnd, 0, 0);
    //SendEWindowSize();
}

void EWindow_DrawTab(LPCANVAS hCanvas, LPOBJSTRUCT pData, PAINTSTRUCT& ps)
{
    if (!pWork || !pState || !pControl)
        return;
    if (__query_flags(work, TOP_STATE_BUTTON_ISAOTUHIDE))
    {
        RECT rc;
        _draw_calc_work(rc);
        _draw_tab(hCanvas, ps, pWork, pTabWork, rc, true, true);
    }
    if (__query_flags(state, TOP_STATE_BUTTON_ISAOTUHIDE))
    {
        RECT rc;
        _draw_calc_state(rc);
        _draw_tab(hCanvas, ps, pState, pTabState, rc, false, false);
    }
    if (__query_flags(control, TOP_STATE_BUTTON_ISAOTUHIDE))
    {
        RECT rc;
        _draw_calc_control(rc);
        _draw_tab(hCanvas, ps, pControl, pTabControl, rc, true, false);
    }
}

// hCanvas = 绘画句柄, 这个是易语言主窗口的绘画句柄
// ps = BeginPaint 得到的结构, 这个是易语言主窗口WM_PAINT得到的结构
// pData = 工作夹/状态夹的窗口数据
// pTab = 工作夹/状态夹下的选择夹窗口数据
// rc = 工作夹/选择夹的窗口位置, 这个是隐藏后在易语言主窗口中绘画的位置
// isRotate = 是否旋转, 旋转都是90度
// isLeft = 绘画文本底边的一小块矩形是否在左边
void _draw_tab(LPCANVAS hCanvas, PAINTSTRUCT& ps, LPOBJSTRUCT pData, LPOBJSTRUCT pTab, RECT& rc, bool isRotate, bool isLeft)
{
    if (!Rect_isOverlap(rc, ps.rcPaint))   // 不是重画区域, 不处理
        return;
    int cxClient = rc.right - rc.left;
    int cyClient = rc.bottom - rc.top;

    LPCANVAS hdc = hCanvas;
    LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    const int fmt = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_END_ELLIPSIS;
    float textHeight = 0;
    _canvas_calctextsize(hdc, hFont, L"福", 1, fmt, 300, 300, 0, &textHeight);
    textHeight += (float)s_info->scale(2);
    const int lineHeight = s_info->scale(5);
    const int nDrawItemHeight = lineHeight + (int)textHeight;    // 自绘选择夹的高度

    Gdiplus::Font* gpFont = 0;
    if (isRotate)
    {
        int _canvasWidth  = nDrawItemHeight * 2;
        int _canvasHeight = nDrawItemHeight * 2;
        if (_canvasWidth < cxClient) _canvasWidth = cxClient;
        if (_canvasHeight < cyClient) _canvasHeight = cyClient;

        if ((int)m_hCanvas->GetWidth() < _canvasWidth || (int)m_hCanvas->GetHeight() < _canvasHeight)
            _canvas_resize(m_hCanvas, _canvasWidth, _canvasHeight);

        _canvas_begindraw(m_hCanvas);
        COLORREF crBack = s_pData->draw.color.crBackGround;
        _canvas_clear(m_hCanvas, RGB2ARGB(crBack, 255));
        _canvas_SetTextRenderingHint(m_hCanvas, 1, 0);
        if (s_info->isGpRender)
        {
            _canvas_SetAntialiasMode(m_hCanvas, 0, 0);
            //m_hCanvas->translate();
            Gdiplus::Graphics* pGraihics = 0;       // gdi+ 画布
            m_hCanvas->GetTargetRender((void**)&pGraihics);

            pGraihics->TranslateTransform((float)cxClient + 3, 0);
            pGraihics->RotateTransform(90);

            LOGFONTW lf;
            pData->draw.hFont->GetLogfont(&lf);
            gpFont = new Gdiplus::Font(hdc->GetGdiDC(), &lf);
        }
        else
        {
            _canvas_rotate(m_hCanvas, (float)nDrawItemHeight, (float)nDrawItemHeight, 90);
        }
        hdc = m_hCanvas;
    }

    // 这里开始就要绘画选择夹了
    CUSTAB_DRAW draw;
    HWND hWnd = _draw_get_draw(pTab, draw);
    const int index = TabCtrl_GetCurSel(hWnd);          // 当前选择夹选中项
    const int ItemCount = TabCtrl_GetItemCount(hWnd);   // 选择夹项目数
    const int bufSize = 0x1000;
    wchar_t text[bufSize] = { 0 };
    TCITEMW item = { 0 };
    item.mask = TCIF_TEXT;
    int maxHeight = 0;
    std::vector<int>* pArr = _drawtab_getarr(pData, 0, 0);


    if (isRotate)
    {
        RECT_F rcText, rcLine, rcTextOut;
        //const int width = WINDOW_AUTO_HEIGHT + WINDOW_SPACING;
        rcText.bottom = (float)s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_SPACING);
        for (int i = 0; i < ItemCount; i++)
        {
            LPEX_BRUSH hbrText = draw.hbrText;
            LPEX_BRUSH hbrLine = draw.hbrLine;
            if (i == pData->draw.indexDown)
            {
                hbrText = draw.hbrTextDown;
                hbrLine = draw.hbrLineDown;
            }
            else if (i == pData->draw.indexHot)
            {
                hbrText = draw.hbrTextHot;
                hbrLine = draw.hbrLineHot;
            }
            item.pszText = text;
            item.cchTextMax = bufSize;
            TabCtrl_GetItem(hWnd, i, &item);
            int& nRotateHeight = pArr->at(i);
            float textWidth = 0;
            _canvas_calctextsize(hdc, pData->draw.hFont, text, -1, fmt, 300, 300, &textWidth, 0);
            textWidth += 10;
            if (nRotateHeight != (int)textWidth)
                nRotateHeight = (int)textWidth;
            float nRotateHeightDpi = (float)s_info->scale(nRotateHeight);
            if (isLeft)
            {
                rcText(rcText.left, 0, rcText.left + nRotateHeightDpi, textHeight);
                rcLine(rcText.left, rcText.bottom, rcText.right, rcText.bottom + (float)lineHeight);
            }
            else
            {
                rcLine(rcText.left, 0, rcText.left + nRotateHeightDpi, (float)lineHeight);
                rcText(rcText.left, rcLine.bottom, rcText.left + nRotateHeightDpi, rcLine.bottom + textHeight);
            }

            if (s_info->isDxRender)
            {
                _canvas_drawtextex(hdc, hFont, hbrText, text, -1, fmt, &rcText, 0, 0, 0, 0, &rcTextOut);
            }
            else
            {
                Gdiplus::RectF rcTextGp(rcText.left, rcText.top, rcText.width(), rcText.height());

                COLORREF crText = pData->draw.color.crText;
                if (i == pData->draw.indexDown)
                    crText = pData->draw.color.crTextDown;
                else if (i == pData->draw.indexHot)
                    crText = pData->draw.color.crTextHot;
                Gdiplus::SolidBrush hbrTextGp(RGB2ARGB(crText, 255));
                
                Gdiplus::StringFormat gpfmt;
                gpfmt.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
                gpfmt.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
                Gdiplus::Graphics* pGraihics = 0;       // gdi+ 画布
                hdc->GetTargetRender((void**)&pGraihics);
                pGraihics->DrawString(text, -1, gpFont, rcTextGp, &gpfmt, &hbrTextGp);
            }
            _canvas_fillRectangle(hdc, hbrLine, &rcLine);
            maxHeight += (int)nRotateHeightDpi;
            rcText.left = rcText.right + 5.f;
        }
        RECT_I rcBitblt;
        int leftFromSrc = 0;
        int topFromSrc = 0;
        maxHeight = (int)rcText.left;
        if (s_info->isGpRender)
        {
            Gdiplus::Graphics* pGraihics = 0;       // gdi+ 画布
            m_hCanvas->GetTargetRender((void**)&pGraihics);
            pGraihics->ResetTransform();
            if (isLeft)
            {
                rcBitblt(0, rc.top, cxClient, rc.top + maxHeight);
            }
            else
            {
                const int left = s_pData->pos.width - WINDOW_AUTO_HEIGHT;
                rcBitblt(left, rc.top, left + cxClient, rc.top + maxHeight);
                leftFromSrc = -2;
            }
        }
        else
        {
            _canvas_rotate(m_hCanvas, 0, 0, 0);
            _canvas_enddraw(m_hCanvas);
            if (isLeft)
            {
                rcBitblt(0, rc.top, nDrawItemHeight, rc.top + maxHeight);
                leftFromSrc = nDrawItemHeight;
            }
            else
            {
                const int left = s_pData->pos.width - nDrawItemHeight;
                rcBitblt(left, rc.top, left + nDrawItemHeight, rc.top + maxHeight);
                leftFromSrc = nDrawItemHeight;

            }
        }
        _canvas_bitblt(hCanvas, rcBitblt.left, rcBitblt.top, rcBitblt.width(), rcBitblt.height(), m_hCanvas, leftFromSrc, topFromSrc, SRCCOPY);
    }
    else
    {
        // 不是旋转, 直接绘画到窗口上
        RECT_F rcText(rc), rcLine, rcTextOut;
        if (s_info->isGpRender)
            _canvas_SetAntialiasMode(hdc, 0, 0);
        for (int i = 0; i < ItemCount; i++)
        {
            LPEX_BRUSH hbrText = draw.hbrText;
            LPEX_BRUSH hbrLine = draw.hbrLine;
            if (i == pData->draw.indexDown)
            {
                hbrText = draw.hbrTextDown;
                hbrLine = draw.hbrLineDown;
            }
            else if (i == pData->draw.indexHot)
            {
                hbrText = draw.hbrTextHot;
                hbrLine = draw.hbrLineHot;
            }


            item.pszText = text;
            item.cchTextMax = bufSize;
            TabCtrl_GetItem(hWnd, i, &item);

            int& nRotateHeight = pArr->at(i);
            float nRotateHeightDpi = (float)s_info->scale(nRotateHeight);
            float textWidth = 0;
            _canvas_calctextsize(hdc, pData->draw.hFont, text, -1, fmt, 300, 300, &textWidth, 0);
            textWidth += 10;
            if (nRotateHeight != textWidth)
                nRotateHeight = (int)textWidth;

            const bool isTop = isLeft;
            if (isTop)  // 横线在顶边
            {
                const float offsetText = 3.f;

                rcLine.left = rcText.left;
                rcLine.top = (float)rc.top + offsetText;
                rcLine.right = rcLine.left + (float)(nRotateHeightDpi);
                rcLine.bottom = rcLine.top + lineHeight;

                rcText.top = rcLine.bottom;
                rcText.right = rcLine.right;
            }
            else
            {
                rcLine(rcText.left, (float)rc.bottom - lineHeight, rcText.left + nRotateHeightDpi, (float)rc.bottom);
                rcText.right = rcLine.right;
                rcText.bottom = rcLine.top;
            }

            _canvas_drawtext(hdc, hFont, hbrText, text, -1, fmt, &rcText);
            _canvas_fillRectangle(hdc, hbrLine, &rcLine);

            rcText.left = rcText.right + 5.f;
        }
    }
    delete gpFont;
}


void _draw_tab_control(LPCANVAS hCanvas, PAINTSTRUCT& ps)
{

}

void _draw_calc_work(RECT& rc)
{
    if (!state)return;
    const int height = s_info->scale(WINDOW_AUTO_HEIGHT);
    const int cyStatus = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;
    rc.left     = 0;
    rc.top      = s_info->rcWndMenu.bottom;
    rc.right    =  height;
    rc.bottom   = __query_flags(state, TOP_STATE_BUTTON_ISAOTUHIDE)
        ? (s_pData->pos.height - height - cyStatus)
        : s_info->rcState.top;
}

void _draw_calc_state(RECT& rc)
{
    if (!control || !work)return;
    const int height = s_info->scale(WINDOW_AUTO_HEIGHT);
    int right = s_pData->pos.width - WINDOW_BORDER_WIDTH;
    rc.left     = __query_flags(work, TOP_STATE_BUTTON_ISAOTUHIDE)
        ? (height + WINDOW_BORDER_WIDTH * 2)    // 如果工作夹是自动隐藏, 那左边就是工作夹的右边
        : WINDOW_BORDER_WIDTH;                              // 否则就是边框宽度的位置
    rc.right = __query_flags(control, TOP_STATE_BUTTON_ISAOTUHIDE)
        ? right - height - WINDOW_BORDER_WIDTH  // 如果组件箱是自动隐藏, 那右边就是 窗口右边 - 边框*2 - 自动隐藏预留的宽度
        : right;                                            // 否则就是窗口右边 - 边框宽度
    
    if (s_info->rcStatusBar.bottom - s_info->rcStatusBar.top <= 0)
        rc.bottom = s_info->pWnd->pos.height - 1;   // 没有状态条
    else
        rc.bottom = s_info->rcStatusBar.top;    // 有状态条

    rc.top = rc.bottom - height;
}

void _draw_calc_control(RECT& rc)
{
    if (!control || !work || !state)return;
    const int height = s_info->scale(WINDOW_AUTO_HEIGHT);

    const int cyStatus = s_info->rcStatusBar.bottom - s_info->rcStatusBar.top;
    rc.left     = s_pData->pos.width - height;
    rc.top      = s_info->rcWndMenu.bottom;
    rc.right    = rc.left + height;
    rc.bottom   = __query_flags(state, TOP_STATE_BUTTON_ISAOTUHIDE)
        ? (s_pData->pos.height - height - cyStatus)
        : s_info->rcState.top;
}

void CALLBACK TimerProc_Leave_mouse_tab(HWND hWnd, UINT message, UINT_PTR id, DWORD dwTimer)
{
    // 这里的id是工作夹/状态夹的窗口数据
    // 窗口句柄是易语言自带底部的代码切换选择夹的窗口句柄
    
    RECT rc;
    POINT pt;
    bool isCmp = false;
    GetCursorPos(&pt);
    ScreenToClient(s_info->hEWnd, &pt);
    LPOBJSTRUCT pData = (LPOBJSTRUCT)id;    
    if (pData->hWnd == s_info->hWork)
    {
        _draw_calc_work(rc);
        isCmp = true;

    }
    else if (pData->hWnd == s_info->hState)
    {
        _draw_calc_state(rc);
        isCmp = true;
    }
    else if (pData->hWnd == s_info->hControl)
    {
        //static int a = 0;
        //wstr::dbg(L"组件箱时钟触发, %d\n", ++a);
        _draw_calc_control(rc);
        isCmp = true;
    }

    // 鼠标不在范围内, 需要设置热点为-1, 并重画
    if (isCmp && !PtInRect(&rc, pt))
    {
        KillTimer(hWnd, id);
        pData->draw.indexHot = -1;
        InvalidateRect(s_info->hEWnd, &rc, 1); // 重画自绘选择夹的位置
        return;
    }

}

// 处理鼠标移动事件
// hWnd = 选择夹句柄
// rc = 选择夹对应的窗口容器矩形, 工作夹/状态夹的矩形
// pt = 触发鼠标移动时的鼠标位置
// pData = 工作夹/状态夹的窗口数据
// isRotate = 是否旋转, 如果旋转的话就需要重新处理项目矩形
inline void _mouse_move_(HWND hWnd, RECT& rc, POINT& pt, LPOBJSTRUCT pData, bool isRotate)
{
    std::vector<int>* pArr = (std::vector<int>*)pData->param1;

    const int index     = TabCtrl_GetCurSel(hWnd);      // 当前选择夹选中项
    const int ItemCount = TabCtrl_GetItemCount(hWnd);   // 选择夹项目数
    int width = 0;
    for (int i = 0; i < ItemCount; i++)
    {
        RECT rcItem;
        const int nRotateHeight = s_info->scale(pArr->at(i));
        if (isRotate)
        {
            rcItem.left     = rc.left;
            rcItem.top      = rc.top + width + i * 5;
            rcItem.bottom   = rcItem.top + nRotateHeight;
            rcItem.right    = rcItem.left + s_info->scale(WINDOW_AUTO_HEIGHT + WINDOW_SPACING);
        }
        else
        {
            rcItem.left     = rc.left + width + i * 5;
            rcItem.right    = rcItem.left + nRotateHeight;
            rcItem.top      = rc.top;
            rcItem.bottom   = rc.bottom;
        }
        width += nRotateHeight;

        if (PtInRect(&rcItem, pt))
        {
            if (i != pData->draw.indexHot)
            {
                SetTimer(s_info->hCodeTab, (UINT_PTR)pData, 50, TimerProc_Leave_mouse_tab);

                pData->draw.indexHot = i;
                InvalidateRect(s_info->hEWnd, &rc, 1); // 重画自绘状态条的位置
            }
            return;
        }
    }
    // 走到这里就是鼠标没在任何项目中, 需要判断是否重画原来的热点项
    if (pData->draw.indexHot != -1)
        InvalidateRect(s_info->hEWnd, &rc, 1); // 重画自绘状态条的位置
    pData->draw.indexHot = -1;
    return;
}
// pData = 工作夹/状态夹的窗口数据
// pTab = 工作夹/状态夹下面的选择夹窗口数据
inline void _mouse_lbutton(LPOBJSTRUCT pData, LPOBJSTRUCT pTab)
{
    if (pData->draw.indexHot == -1)
        return;

    if (pData->draw.indexDown == pData->draw.indexHot && IsWindowVisible(pData->hWnd))
    {
        pData->draw.indexDown = -1;
        s_info->iDraw->TopState_UpdateStateWindow(0);
    }
    else
    {
        //TODO 这里需要计算重画区域
        InvalidateRect(s_info->hEWnd, 0, 0);
        pData->draw.indexDown = pData->draw.indexHot;
        s_info->iDraw->TopState_UpdateStateWindow(pData->hWnd);
    }

    if (TabCtrl_GetCurSel(pTab->hWnd) != pData->draw.indexHot)
        TabCtrl_SetCurSel(pTab->hWnd, pData->draw.indexHot);
}

LRESULT EWindow_TabMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case WM_MOUSEMOVE:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        RECT rcState, rcWork, rcControl;
        _draw_calc_work(rcWork);
        _draw_calc_state(rcState);
        _draw_calc_control(rcControl);

        IGetEWindow* eWnd = s_info->eWnd;
        //rcWork.right -= WINDOW_SPACING;
        //rcState.top += WINDOW_SPACING;
        if (PtInRect(&rcWork, pt))
        {
            _mouse_move_(eWnd->GetWorkTab(), rcWork, pt, pWork, true);
        }
        else if (PtInRect(&rcState, pt))
        {
            _mouse_move_(eWnd->GetStateTab(), rcState, pt, pState, false);
        }
        else if (PtInRect(&rcControl, pt))
        {
            _mouse_move_(s_info->hWndTabControl, rcControl, pt, pControl, true);
            return 0;
        }
        else
        {
            return 0;
        }

        pData->isReturn = true;
        return 0;
        //pWork->draw.indexHot = -1;
        //pState->draw.indexHot = -1;
        //pControl->draw.indexHot = -1;
        break;
    }
    case WM_LBUTTONDOWN:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        RECT rcState, rcWork, rcControl;
        _draw_calc_work(rcWork);
        _draw_calc_state(rcState);
        _draw_calc_control(rcControl);
        if (PtInRect(&rcWork, pt))
        {
            _mouse_lbutton(pWork, pTabWork);
        }
        else if (PtInRect(&rcState, pt))
        {
            _mouse_lbutton(pState, pTabState);
        }
        else if (PtInRect(&rcControl, pt))
        {
            _mouse_lbutton(pControl, pTabControl);
        }
        else
        {
            return 0;
        }
        pData->isReturn = true;
        return 0;
    }
    case WM_LBUTTONUP:
    {
        break;
    }
    case WM_CAPTURECHANGED:
    {

        break;
    }
    default:
        return 0;
    }
    //pData->isReturn = true;
    return 0;
}

std::vector<int>* _drawtab_getarr(LPOBJSTRUCT pData, std::vector<int>* pArr, bool isSet)
{
    if (!pData) return 0;
    if (isSet)
        pData->param1 = pArr;
    return (std::vector<int>*)pData->param1;
}

LPTOPSTATE_INFO _drawtab_gettopstate_info(LPOBJSTRUCT pData, LPTOPSTATE_INFO info, bool isSet)
{
    if (!pData) return 0;
    if (isSet)
        pData->param = info;
    return (LPTOPSTATE_INFO)pData->param;
}
HWND _draw_top_gettab(LPOBJSTRUCT pData, HWND hTab, bool isSet)
{
    if (!pData) return 0;
    if (isSet)
        pData->param2 = hTab;
    return (HWND)pData->param2;
}

// 编译事件, 由助手通知, 返回处理结果
// code = 编译状态, BUILD_STATE_ 开头常量
// lParam = 保留参数
int EWindow_Build(int code, int lParam)
{
    if (!state || !__query_flags(state, TOP_STATE_BUTTON_ISAOTUHIDE))
        return 0;   // 状态夹不是自动隐藏就不处理
    switch (code)
    {
    case BUILD_STATE_BEGIN:     // 编译开始, 需要把输出弹出来
    {
        //TODO 需要处理子夹位置, 这里是固定的按第二个子夹来显示
        pState->draw.indexHot = 1;
        _mouse_lbutton(pState, pTabState);
        pState->draw.indexHot = -1;
        break;
    }
    case BUILD_STATE_END:       // 编译结束
    {

        break;
    }
    case BUILD_STATE_OK:        // 编译成功
    {

        break;
    }
    case BUILD_STATE_FAIL:      // 编译失败
    {

        break;
    }
    default:
        break;
    }
    return 0;
}