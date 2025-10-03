#include "top_state.h"
#include <unordered_set>
#define WINDOW_TOP_STATE 24

#define _is_nogrop(_style)      (((_style) & GS_NOGROP) == GS_NOGROP)
#define _is_left(_style)        (((_style) & GS_VERTICAL) == GS_VERTICAL)
#define _is_right(_style)       (((_style) & GS_BOTTOM) == GS_BOTTOM)
#define _is_bottom(_style)      _is_right(_style)
#define _is_fixedwidth(_style)  (((_style) & GS_FIXEDWIDTH) == GS_FIXEDWIDTH)

#define __isclose(_info)        __query_flags(_info, TOP_STATE_BUTTON_CLOSE)
#define __maxbox(_info)         __query_flags(_info, TOP_STATE_BUTTON_MAXBOX)
#define __pushpin(_info)        __query_flags(_info, TOP_STATE_BUTTON_PUSHPIN)
#define __istriangle(_info)     __query_flags(_info, TOP_STATE_BUTTON_TRIANGLE)
#define __isswitch(_info)       __query_flags(_info, TOP_STATE_BUTTON_SWITCH)


const int ImageOffset = 2;
const int buttonwidth = 18;     // 按钮绘画宽度
const int buttonheight = 18;    // 按钮绘画高度
static HWND s_currentStateWindow;
static std::unordered_set<HWND> s_set_hWnd;
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

// 绘画带选择夹样式
void Paint_Tab(LPCANVAS hCanvas, RECT& rcClient, LPOBJSTRUCT pData, LPTAB_INFO_TOPSTATE tabInfo);

// 绘画不带选择夹样式
void Paint_NoTab(LPCANVAS hCanvas, RECT& rcClient, LPOBJSTRUCT pData, LPTAB_INFO_TOPSTATE tabInfo);


void _draw_top_state(LPCANVAS hCanvas, RECT& rcDraw, LPTAB_INFO_TOPSTATE tabInfo, const RECT& rcClient, LPCWSTR text);

LRESULT CALLBACK Group_Cell_OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    RECT rc;
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    GetClientRect(hWnd, &rc);
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    LPCANVAS hCanvas = pData->draw.hCanvas;
    LPTAB_INFO_TOPSTATE tabInfo = _get_tabinfo(pData);
    TAB_DRAWINFO& draw = tabInfo->draw;
    if (!draw.hbrBack)
    {
        pData->draw.color.crBackGround  = RGB(245, 245, 245);
        pData->draw.color.crSelected    = RGB(210, 210, 213);
        pData->draw.color.crHot         = RGB(225, 225, 228);
        pData->draw.color.crDown        = RGB(200, 200, 203);
        pData->draw.color.crText        = RGB(30, 30, 30);
        pData->draw.color.crTextHot     = RGB(85, 170, 255);
        pData->draw.color.crTextDown    = RGB(0, 151, 251);
        pData->draw.color.crBorder      = RGB(204, 206, 219);
        pData->draw.color.crNormal      = RGB(238, 238, 242);
        const float dashes[] = { 1, 3 };

        COLORREF cr = s_info->themeColor->crStatelineNormal(); cr = RGB2ARGB(cr, 255);
        draw.hPenDot        = _pen_create(cr, 1, 5, dashes, sizeof(dashes) / sizeof(dashes[0]));

        cr = s_info->themeColor->crStatelineHot(); cr = RGB2ARGB(cr, 255);
        draw.hPenDotAct     = _pen_create(cr, 1, 5, dashes, sizeof(dashes) / sizeof(dashes[0]));

        cr = s_info->themeColor->crStateBar(); cr = RGB2ARGB(cr, 255);
        draw.hbrStateAct    = _brush_create(cr);
        draw.hbrState       = _brush_create(RGB2ARGB(pData->draw.color.crNormal     , 255));

        draw.hbrBack        = _brush_create(RGB2ARGB(pData->draw.color.crBackGround , 255));
        draw.hbrSel         = _brush_create(RGB2ARGB(pData->draw.color.crSelected   , 255));
        draw.hbrExtern      = _brush_create(RGB2ARGB(pData->draw.color.crNormal     , 255));
        draw.hbrHot         = _brush_create(RGB2ARGB(pData->draw.color.crHot        , 255));
        draw.hbrDown        = _brush_create(RGB2ARGB(pData->draw.color.crDown       , 255));
        draw.hbrText        = _brush_create(RGB2ARGB(pData->draw.color.crText       , 255));
        draw.hbrTextHot     = _brush_create(RGB2ARGB(pData->draw.color.crTextHot    , 255));
        draw.hbrTextDown    = _brush_create(RGB2ARGB(pData->draw.color.crTextDown   , 255));

        draw.hPenBorder     = _pen_create  (RGB2ARGB(pData->draw.color.crBorder     , 255));

    }

    if (hCanvas->width < (UINT)cxClient || hCanvas->height < (UINT)cyClient)
        _canvas_resize(hCanvas, cxClient, cyClient);

    _canvas_begindraw(hCanvas);
    
    _canvas_SetAntialiasMode(hCanvas, 0, 0);
    _canvas_SetTextAntialiasMode(hCanvas, 1, 0);

    _canvas_clear(hCanvas, RGB2ARGB(pData->draw.color.crNormal, 255));

    if (__is_reserve_left(tabInfo->info))
        rc.left += WINDOW_SPACING;
    if (__is_reserve_top(tabInfo->info))
        rc.top += WINDOW_SPACING;
    if (__is_reserve_right(tabInfo->info))
        rc.right -= WINDOW_SPACING;
    if (__is_reserve_bottom(tabInfo->info))
        rc.bottom -= WINDOW_SPACING;

    if (_is_nogrop(pData->style))   // 根据样式绘画选择夹或者不绘画选择夹
        Paint_NoTab(hCanvas, rc, pData, tabInfo);
    else
        Paint_Tab(hCanvas, rc, pData, tabInfo);


    HDC hdcD2D = _canvas_getdc(hCanvas);
    BitBlt(ps.hdc, 0, 0, cxClient, cyClient, hdcD2D, 0, 0, SRCCOPY);
    _canvas_releasedc(hCanvas);

    _canvas_enddraw(hCanvas);
    EndPaint(hWnd, &ps);
    return 0;
}

inline void Drawline(LPCANVAS hCanvas, LPEX_PEN hPen, float x1, float y1, float x2, float y2)
{
    LINEPOINT_F line(x1, y1, x2, y2);
    _canvas_drawLine(hCanvas, hPen, &line);
}
void Paint_Tab(LPCANVAS hCanvas, RECT& rc, LPOBJSTRUCT pData, LPTAB_INFO_TOPSTATE tabInfo)
{
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    TAB_ITEMINFO_ARR& arr = *tabInfo->arr;
    TAB_DRAWINFO& draw = tabInfo->draw;
    int i = 0;
    size_t size = arr.size();


    RECT_F rcF(rc);
    rcF.left += (int)s_info->isDxRender;
    rcF.top += (int)s_info->isDxRender;
    rcF.right -= (int)s_info->isGpRender;
    const int tabTop = Tab_CalcTop(pData);
    rcF.bottom = (float)tabTop + (int)(s_info->isDxRender) - (int)(s_info->isGpRender);
    _canvas_fillRectangle(hCanvas, draw.hbrBack, &rcF);   // 填充选择夹整个容器的背景
    _canvas_drawRectangle(hCanvas, draw.hPenBorder, &rcF);

    rcF.top = (float)tabTop + (int)(s_info->isDxRender);
    rcF.bottom = (float)rc.bottom + (int)(s_info->isDxRender) - (int)(s_info->isGpRender);
    _canvas_fillRectangle(hCanvas, draw.hbrExtern, &rcF);   // 填充子夹的背景


    rcF.top = (float)rc.top;
    //Drawline(hCanvas, draw.hPenBorder, rcF.left + 1, rcF.top, rcF.left + 1, rcF.bottom);  // 子夹左边的线
    //Drawline(hCanvas, draw.hPenBorder, rcF.left + 1, rcF.top, rcF.right, rcF.top);  // 子夹顶边的线
    //Drawline(hCanvas, draw.hPenBorder, rcF.right, rcF.top, rcF.right, rcF.bottom);  // 子夹右边的线


    LPCWSTR text = 0;
    rcF.top = (float)tabTop;
    rcF.right = rcF.left;
    const int fmt = DT_VCENTER | DT_SINGLELINE | DT_CENTER | DT_NOPREFIX | DT_END_ELLIPSIS;
    for (auto it = arr.begin(); it != arr.end(); ++it, ++i)
    {
        TAB_ITEMINFO& info = *it;
        rcF.left = rcF.right;
        rcF.right = rcF.left + (float)info.width;
        LPEX_BRUSH hbr = 0, hbrText = draw.hbrText;
        if (i == pData->draw.indexSel)
        {
            hbr = draw.hbrBack;
            hbrText = draw.hbrTextDown;
        }
        else if (i == pData->draw.indexDown)
        {
            hbr = draw.hbrDown;
            hbrText = draw.hbrTextDown;
        }
        else if (i == pData->draw.indexHot)
        {
            hbr = draw.hbrHot;
            hbrText = draw.hbrTextHot;
        }
        

        if (hbr)
        {
            rcF.bottom -= (int)(s_info->isDxRender);
            if (i == pData->draw.indexSel)
            {
                text = info.name;
                rcF.top--; rcF.top -= (int)s_info->isGpRender;
                _canvas_fillRectangle(hCanvas, hbr, &rcF);
                rcF.top++;
                Drawline(hCanvas, draw.hPenBorder, rcF.left, rcF.top - (i == 0), rcF.left, rcF.bottom);    // 选中项左边的竖线
                Drawline(hCanvas, draw.hPenBorder, rcF.right, rcF.top, rcF.right, rcF.bottom);  // 选中项右边的竖线
                Drawline(hCanvas, draw.hPenBorder, rcF.left, rcF.bottom, rcF.right, rcF.bottom);// 选中项底边的竖线

                rcF.top += (int)s_info->isGpRender;
            }
            else
            {
                if (s_info->isGpRender)
                    rcF.left++, rcF.bottom++;
                rcF.top += s_info->isDxRender;
                _canvas_fillRectangle(hCanvas, hbr, &rcF);
                if (s_info->isGpRender)
                    rcF.left--, rcF.bottom--;
                rcF.top -= s_info->isDxRender;
            }
            rcF.bottom += (int)(s_info->isDxRender);
        }
        _canvas_drawtext(hCanvas, pData->draw.hFont, hbrText, info.name, -1, fmt, &rcF);
    }

    RECT rcDraw = rc;
    rcDraw.bottom = rcDraw.top + WINDOW_TOP_STATE;
    _draw_top_state(hCanvas, rcDraw, tabInfo, rc, text);

    return;
}

void Paint_NoTab(LPCANVAS hCanvas, RECT& rcClient, LPOBJSTRUCT pData, LPTAB_INFO_TOPSTATE tabInfo)
{
    const int cxClient = rcClient.right - rcClient.left;
    const int cyClient = rcClient.bottom - rcClient.top;
    TAB_ITEMINFO_ARR& arr = *tabInfo->arr;



    return;
}



void calc_button_pos(LPTOPSTATE_INFO info, HWND hWnd, const RECT& rc)
{
    const int cxItem = rc.right - rc.left;
    const int cyItem = WINDOW_TOP_STATE;

    int left = 0;
    int top = (cyItem - buttonheight) / 2;
    int index = 0;

    if (__isclose(info))     // 有关闭按钮
    {
        left = rc.right - (buttonwidth * (index + 1)) - 4;
        info->rc[0] = { left, top, left + buttonwidth, top + buttonheight };
        index++;
    }
    if (__maxbox(info))    // 有最大化按钮
    {
        left = rc.right - (buttonwidth * (index + 1)) - 4;
        info->rc[4] = { left, top, left + buttonwidth, top + buttonheight };
        index++;
    }
    if (__pushpin(info))   // 有图钉按钮
    {
        left = rc.right - (buttonwidth * (index + 1)) - 4;
        info->rc[1] = { left, top, left + buttonwidth, top + buttonheight };
        index++;
    }

    if (__istriangle(info))  // 有三角按钮
    {
        left = rc.right - (buttonwidth * (index + 1)) - 4;
        info->rc[2] = { left, top, left + buttonwidth, top + buttonheight };
        index++;
    }
    if (__isswitch(info))    // 有切换按钮
    {
        left = rc.right - (buttonwidth * (index + 1)) - 4;
        info->rc[3] = { left, top, left + buttonwidth, top + buttonheight };
        index++;
    }

    info->count = index;
    return;
}


// 绘画小按钮
// gp = gdi+绘画图形
// rc = 按钮的位置, 小图标是绘画在这个区域内, 内部计算居中在这个区域里
// index = 绘画第几个按钮, TOP_STATE_BUTTON 枚举常量的值
inline void _draw_icon(LPCANVAS hCanvas, const LPTOPSTATE_INFO info, TOP_STATE_BUTTON index, LPOBJSTRUCT pData)
{
    if (!__query_flags(info, index))
        return;
    // iconIndex = 图标在图片中的左边位置, 0=关闭, 1=图钉纵向(固定), 2=图钉横向(浮动), 3=三角, 4=组件箱切换
    LPCWSTR iconIndex;
    int infoIndex;
    if (index == TOP_STATE_BUTTON_CLOSE)            iconIndex = L"0", infoIndex = 0;
    else if (index == TOP_STATE_BUTTON_PUSHPIN)     iconIndex = L"1", infoIndex = 1;
    else if (index == TOP_STATE_BUTTON_TRIANGLE)    iconIndex = L"3", infoIndex = 2;
    else if (index == TOP_STATE_BUTTON_MAXBOX)      iconIndex = L"5", infoIndex = 4;
    else iconIndex = L"4", infoIndex = 3;
    if (index == TOP_STATE_BUTTON_PUSHPIN && info->isSelect)        iconIndex = L"2";
    else if (index == TOP_STATE_BUTTON_MAXBOX && info->isRestore)   iconIndex = L"6";
    // iconIndex 这个变量已经存放好了该绘画的图标索引


    const RECT& rc = info->rc[infoIndex];
    const int cxIcon = 10;
    const int cyIcon = 10;
    const int cxItem = rc.right - rc.left;
    const int cyItem = rc.bottom - rc.top;

    if (info->index == infoIndex)
    {
        RECT_F rcBack;
        rcBack.left     = (float)(rc.left);
        rcBack.top      = (float)(rc.top);
        rcBack.right    = (float)(rc.left + cxItem);
        rcBack.bottom   = (float)(rc.top  + cyItem);

        // 需要判断有没有状态, 有状态需要填充一下背景
        if (info->state == STATE::HOT)
        {
            LPEX_BRUSH hbrBack = _brush_create(RGB2ARGB(pData->draw.color.crHot, 255));
            _canvas_fillRectangle(hCanvas, hbrBack, &rcBack);
        }
        else if (info->state == STATE::DOWN)
        {
            LPEX_BRUSH hbrBack = _brush_create(RGB2ARGB(pData->draw.color.crDown, 255));
            _canvas_fillRectangle(hCanvas, hbrBack, &rcBack);
        }
    }

    RECT_F rcDest;
    rcDest.left     = (float)(rc.left + (cxItem - cxIcon) / 2);
    rcDest.top      = (float)(rc.top  + (cyItem - cyIcon) / 2);
    rcDest.right    = rcDest.left + (float)(cxIcon);
    rcDest.bottom   = rcDest.top  + (float)(cyIcon);

    RECT_F rcIconDraw;
    LPWINDOW_ICON_DATA retData = s_info->pResource->TopState_GetIcon(iconIndex, &rcIconDraw, STATE::NORMAL);
    if (retData)
    {
        _canvas_drawimagerectrect(hCanvas, retData->hBitmapDx,
            rcDest.left, rcDest.top, rcDest.right, rcDest.bottom,
            rcIconDraw.left, rcIconDraw.top, rcIconDraw.right, rcIconDraw.bottom, 255);
    }
}

// 给定一个绘画图形, 绘画位置, 点的颜色 内部往这个hdc上绘画三条点线
// hCanvas = 绘画句柄
// rcDraw = 绘画的范围, 内部根据这个矩形绘画
// crDot = 点的颜色
// isTransverse = 是否横向, 为 true 则从左往右绘画, false 则从上往下绘画
void _draw_top_state_dot(LPCANVAS hCanvas, const RECT_F& rcDraw, LPEX_PEN hPenDot, bool isTransverse)
{
    LINEPOINT_F pt1, pt2, pt3;
    const float x1 = (float)rcDraw.left;
    const float y1 = (float)rcDraw.top;
    const float x2 = (float)rcDraw.right;
    const float y2 = (float)rcDraw.bottom;
    if (isTransverse)
    {
        // 从做往右绘画
        pt1(x1 + 0, y1 + 0, x2 - 0, y1 + 0);
        pt2(x1 + 2, y1 + 2, x2 - 2, y1 + 2);
        pt3(x1 + 0, y1 + 4, x2 - 0, y1 + 4);
    }
    else
    {
        // 从上往下绘画
        pt1(x1 + 0, y1 + 0, x1 - 0, y2 + 0);
        pt2(x1 + 2, y1 + 2, x1 + 2, y2 - 2);
        pt3(x1 + 4, y1 + 0, x1 + 4, y2 - 0);
    }

    _canvas_drawLine(hCanvas, hPenDot, &pt1);   // 上面
    _canvas_drawLine(hCanvas, hPenDot, &pt2);   // 中间
    _canvas_drawLine(hCanvas, hPenDot, &pt3);   // 下面

}

// 给定一个绘画图形, 绘画位置, 点的颜色 内部往这个hdc上绘画三条点线+填充背景+绘画文本
// hCanvas = 绘画句柄
// rcDraw = 绘画的范围, 内部根据这个矩形绘画, 这个一般是顶边0,0 开始到 右边,WINDOW_TOP_STATE 的位置
// pData = 
// rcClient = 窗口的客户区 GetClientRect 的值
void _draw_top_state(LPCANVAS hCanvas, RECT& rcDraw, LPTAB_INFO_TOPSTATE tabInfo, const RECT& rcClient, LPCWSTR text)
{
    LPOBJSTRUCT pData = tabInfo->pData;
    TAB_DRAWINFO& draw = tabInfo->draw;
    LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData);
    LPEX_PEN hPenDot;
    LPEX_BRUSH hbrBack, hbrText;
    if (s_currentStateWindow == pData->hWnd)
    {
        // 激活状态
        hPenDot = draw.hPenDotAct;
        hbrBack = draw.hbrStateAct;
        hbrText = draw.hbrText;
    }
    else
    {
        // 未激活
        hPenDot = draw.hPenDot;
        hbrBack = draw.hbrState;
        hbrText = draw.hbrText;
    }

    const bool isTransverse = true;
    calc_button_pos(info, pData->hWnd, rcDraw);


    //wstr::dbg(L"0x%p(%d) rcDraw = {%d, %d, %d, %d} (%d * %d), info->rcWindow = {%d, %d, %d, %d} (%d * %d)\n",
    //    pData->hWnd, pData->hWnd,
    //    rcDraw, rcDraw.right - rcDraw.left, rcDraw.bottom - rcDraw.top,
    //    info->rcWindow, info->rcWindow.right - info->rcWindow.left, info->rcWindow.bottom - info->rcWindow.top);


    RECT_F rcFill(rcDraw);
    ++rcFill.left; ++rcFill.top; --rcFill.right; --rcFill.bottom;
    _canvas_fillRectangle(hCanvas, hbrBack, &rcFill);

    RECT_F rcTextOut;
    if (text && text[0])
    {
        rcFill.left += 5;
        const int fmt = DT_VCENTER | DT_SINGLELINE;
        _canvas_drawtextex(hCanvas, pData->draw.hFont, hbrText, text, -1, fmt, &rcFill, 0, 0, 0, 0, &rcTextOut);
    }

    // 绘画按钮, 内部会判断需不需要绘画这个按钮
    _draw_icon(hCanvas, info, TOP_STATE_BUTTON_CLOSE, pData);
    _draw_icon(hCanvas, info, TOP_STATE_BUTTON_MAXBOX, pData);
    _draw_icon(hCanvas, info, TOP_STATE_BUTTON_PUSHPIN, pData);
    _draw_icon(hCanvas, info, TOP_STATE_BUTTON_TRIANGLE, pData);
    _draw_icon(hCanvas, info, TOP_STATE_BUTTON_SWITCH, pData);

    // 计算绘画的点线, 点线高度5像素, 从文本右边到按钮左边
    RECT_F rcDot;
    const int cyItem = rcDraw.bottom - rcDraw.top;
    rcDot.left      = (float)(rcTextOut.right + 5);
    rcDot.top       = (float)(rcDraw.top + ((cyItem - 5) / 2) + 1);
    rcDot.right     = (float)(rcDraw.right - (buttonwidth * info->count) - 5);
    rcDot.bottom    = (float)(rcDot.top + 5);
    _draw_top_state_dot(hCanvas, rcDot, hPenDot, isTransverse);

}




void Tab_InsertHwnd(HWND hWnd)
{
    if (IsWindow(hWnd))
        s_set_hWnd.insert(hWnd);
}

// 更新当前激活的窗口, 会重画激活的这个窗口和之前激活的窗口
void TopState_UpdateStateWindow(HWND hWnd)
{
    HWND hWndClick = hWnd;
    auto itEnd = s_set_hWnd.end();
    while (hWnd)
    {
        if (s_set_hWnd.find(hWnd) != itEnd)
            break;
        hWnd = (HWND)GetWindowLongPtrW(hWnd, GWLP_HWNDPARENT);
 
    }
    if (!hWnd)
    {
        hWnd = hWndClick;
        while (hWnd)
        {
            if (s_set_hWnd.find(hWnd) != itEnd)
                break;
            hWnd = GetParent(hWnd);
        }
    }
    if (s_currentStateWindow != hWnd)
    {
        if (!hWnd && hWndClick)
        {
            wchar_t cls[260];
            GetClassNameW(hWndClick, cls, 260);
            if (wcscmp(cls, L"ComboLBox") == 0) // 点击了组合框的列表则不处理
                return;
        }

        if (hWnd && s_set_hWnd.find(hWnd) != itEnd)
        {
            const bool isLButton = (GetAsyncKeyState(VK_LBUTTON) & 0xFF00) > 0;
            if (isLButton)
            {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hWnd, &pt);
                LPOBJSTRUCT pData = GetDataFromHwnd(hWnd);
                LPTOPSTATE_INFO info = _drawtab_gettopstate_info(pData);
                if (info)
                {
                    const int count = sizeof(info->rc) / sizeof(info->rc[0]);
                    int min = 0x7fffffff;
                    for (int i = 0; i < count; i++)
                    {
                        //if (info->rc[i].left < min)
                        //    min = info->rc[i].left;
                        if (PtInRect(&info->rc[i], pt)) // 在按钮中, 不触发
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
        //s_info->pfnMove(INE_STATE_UPDATEHWND, (WPARAM)hOld, (LPARAM)s_currentStateWindow);
    }
}
HWND TopState_GetActivateWindow()
{
    return s_currentStateWindow;
}