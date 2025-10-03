#include "CToolBar.h"
#include <windowsx.h>
#include <MemDC.h>

LRESULT CALLBACK CToolBar::WndProc(LPOBJSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        s_hWnd = pData->hObj;
        s_toolData.hObj = pData->hObj;
        LPCREATESTRUCTW pos = (LPCREATESTRUCTW)lParam;
        s_toolData.pos.height = pos->cy;
        s_toolData.pos.width = pos->cx;
        s_toolData.pos.x = pos->x;
        s_toolData.pos.y = pos->y;
        s_toolData.pos.rc.right = pos->x + pos->cx;
        s_toolData.pos.rc.bottom = pos->y + pos->cy;

        break;
    }
    case WM_SIZE:
    {
        s_toolData.pos.width = LOWORD(lParam);
        s_toolData.pos.height = HIWORD(lParam);
        s_toolData.pos.rc.bottom = s_toolData.pos.height;
        s_toolData.pos.rc.right = s_toolData.pos.width;
        //LPOBJSTRUCT pObj = (LPOBJSTRUCT)pData->param;
        //_canvas_resize(pObj->draw.hCanvas, s_toolData.pos.width, s_toolData.pos.height);
        break;
    }
    case WM_PAINT:
        return OnPaint(pData, message, wParam, lParam);
    case WM_MOUSEMOVE:
        return OnMouseMove(pData, message, wParam, lParam);
    case WM_MOUSEHOVER:
        break;
    case WM_MOUSELEAVE:
    {
        if (QueryUiState(STATE::HOVER))
        {
            SetUiState(STATE::HOVER, true, false);
            __set(s_indexHot, STATE::HOT, true, true);
            s_indexHot = -1;
            return 0;
        }
        break;
    }
    case WM_LBUTTONDOWN:
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        s_indexDown = s_indexHot;
        s_ptDown = lParam;
        SetUiState(STATE::DOWN, false, false);
        __set(s_indexHot, STATE::DOWN, false, true);
        ::SetCapture(pData->hObj);
        break;
    }
    case WM_LBUTTONUP:
    {
        SetUiState(STATE::DOWN, true, false);
        ReleaseCapture();
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        // 鼠标放开的位置还是在按下的按钮上,触发个单击事件
        if (s_indexDown != -1 && pt2index(pt, false) == s_indexDown)
        {
            bool isBan = __query(s_indexDown, STATE::BAN);
            if (!isBan)    // 不是禁止状态才触发单击事件
            {
                // TODO 触发单击事件
            }
            // TODO 判断按钮类型, 是选择按钮就改变选中状态
        }
        s_indexDown = -1;
        __set(s_indexHot, STATE::DOWN, true, true);
        return 0;
    }
    default:
        return 0;
        //return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CToolBar::OnPaint(LPOBJSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam)
{
    //PAINTSTRUCT ps;
    //BeginPaint(pData->hWnd, &ps);
    //LPOBJSTRUCT pObj = (LPOBJSTRUCT)pData->param;
    //_canvas_begindraw(pObj->draw.hCanvas);

    //const RECT& rc = s_toolData.pos.rc;
    //int cx = rc.right - rc.left;
    //int cy = rc.bottom - rc.top;
    //CMemDC dc;
    //Gdiplus::Graphics* gp = 0;
    //_canvas_clear(pObj->draw.hCanvas, ARGB_D2D(255, 255, 255, 255));
    ////HDC hDCMem = dc.create(cx, cy, &gp);
    ////SelectObject(hDCMem, m_hFont);
    ////FillRect(hDCMem, &rc, GetStockBrush(WHITE_BRUSH));
    //size_t len = s_sub.size();
    //size_t first = GetFirstVisible();
    //if (len && first == -1)
    //    assert(0);
    //if (len == 0 || first == -1)
    //{
    //    dc.show(ps.hdc, 0, 0, cx, cy);
    //    _canvas_enddraw(pObj->draw.hCanvas);
    //    EndPaint(pData->hWnd, &ps);
    //    return 0;
    //}
    //int width = s_sub[0].rc.left - 2;
    //int height = cy;

    //for (size_t i = first; i < len; i++)
    //{
    //    if (drawitem(pObj->draw.hCanvas, ps.hdc, gp, i))
    //        break;
    //}
    //dc.show(ps.hdc, rc);
    //_canvas_enddraw(pObj->draw.hCanvas);
    //EndPaint(pData->hWnd, &ps);
    return 0;
}

BOOL CToolBar::drawitem(LPCANVAS hCanvas, HDC hdc, Gdiplus::Graphics* gp, size_t i)
{
    static Gdiplus::StringFormat* format;
    static Gdiplus::Pen* pen;
    static LPEX_FONT font;
    if (format == 0)
    {
        font = _font_create();
        pen = new Gdiplus::Pen(Gdiplus::Color(180, 128, 128, 128));
        format = new Gdiplus::StringFormat;
        format->SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
        format->SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
    }
    TOOLCHILD& data = s_sub[i];
    ARGB crBack = 0;
    if (__query(i, STATE::DOWN))
    {
        crBack = data.crBack[2];
        crBack = -1;
    }
    else if (__query(i, STATE::HOT))
    {
        crBack = data.crBack[1];
        crBack = -1217829;
    }
    else
    {
        crBack = data.crBack[0];
        crBack = -125671;
    }
    
    RECT_F rc = data.rc;
    float width = rc.right - rc.left;
    float height = rc.bottom - rc.top;
    BOOL b = 0;
    rc.left -= (int)s_pos;
    rc.right = rc.left + width;
    b = (rc.right > (float)s_toolData.pos.width);
    //Gdiplus::SolidBrush brush = (Gdiplus::Color(crBack));
    LPEX_BRUSH brush = _brush_create(crBack);
    LPEX_PEN hPen = _pen_create(crBack);
    _canvas_fillRectangle(hCanvas, brush, &rc);
    _canvas_drawRectangle(hCanvas, hPen, &rc);
    //gp->FillRectangle(&brush, rc.left, rc.top, width, height);
    //gp->DrawRectangle(pen, rc.left, rc.top, width, height);
    if (data.text)
    {
        //DrawTextW(hdc, data.text, -1, &rc, DT_WORDBREAK);
        _canvas_drawtext(hCanvas, font, brush, data.text, 0, DT_WORDBREAK, &rc);
    }
    _brush_destroy(brush);
    _pen_destroy(hPen);
    return b;
}

LRESULT CToolBar::OnMouseMove(LPOBJSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (s_sub.empty())return 0;
    if (QueryUiState(STATE::DOWN))return 0;
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

    if (!QueryUiState(STATE::HOVER))    // 没有进入的状态
    {
        SetUiState(STATE::HOVER, false, false);
        TRACKMOUSEEVENT tm;
        tm.cbSize = sizeof(tm);
        tm.dwHoverTime = 10;
        tm.hwndTrack = pData->hObj;
        tm.dwFlags = TME_HOVER | TME_LEAVE;
        TrackMouseEvent(&tm);
    }
    size_t i = 0;
    size_t len = s_sub.length();
    for (i = 0; i < len; i++)
    {
        if (PtInRect(&s_sub[i].rc, pt))
        {
            __set(s_indexHot, STATE::HOT, true, false);
            __set(i, STATE::HOT, false, true);
            s_indexHot = i;
            return 0;
        }
    }
    if (i == len)    // 鼠标没在任何按钮上
    {
        if (s_indexHot != -1)
        {
            __set(s_indexHot, STATE::HOT, true, true);
            s_indexHot = -1;
            return 0;
        }
    }
    //return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
    return 0;
}

size_t CToolBar::pt2index(const POINT& pt, bool b)
{
    int x = pt.x;
    if (!b)x += (int)s_pos;

    size_t len = s_sub.length();
    for (size_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            if (x >= s_sub[i].rc.left)return i;
            continue;
        }
        if (x >= s_sub[i].rc.left && x < s_sub[i + 1].rc.left)
            return i;
    }

    for (size_t i = 0; i < len; i++)
    {
        if (PtInRect(&s_sub[i].rc, pt))
        {
            //TODO 判断工具条按钮类型, 如果是分隔条就不处理
            return i;
        }
    }
    return -1;
}

bool CToolBar::__query(size_t index, STATE state)
{
    if (s_sub.empty() || index >= (size_t)s_sub.length()) return false;
    return (s_sub[index].state & state) != 0;
}

bool CToolBar::__set(size_t index, STATE state, BOOL isRemove, BOOL isUpdate)
{
    size_t len = s_sub.length();
    if (index >= len)return false;
    TOOLCHILD& data = s_sub[index];
    if (isRemove)
        data.state = (STATE)(data.state & (~state));
    else
        data.state = (STATE)(data.state | state);

    if (isUpdate)
        ::InvalidateRect(s_hWnd, 0, 0);
    return true;
}

bool CToolBar::SetUiState(STATE state, BOOL isRemove, BOOL isUpdate)
{
    STATE old = s_toolData.draw.state;
    if (isRemove)
        s_toolData.draw.state = (STATE)(s_toolData.draw.state & (~state));
    else
        s_toolData.draw.state = (STATE)(s_toolData.draw.state | state);

    if (isUpdate)
        ::InvalidateRect(s_toolData.hObj, 0, 0);
    return old;
}

bool CToolBar::QueryUiState(STATE state)
{
    return (s_toolData.draw.state & state) != 0;
}


size_t CToolBar::GetFirstVisible()
{
    size_t len = s_sub.length();
    if (len == 0) return -1;
    if (s_pos < s_sub[0].rc.right)
        return 0;
    for (size_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            if (s_pos >= s_sub[i].rc.left)
                return i;
            continue;

        }
        if (s_pos >= s_sub[i].rc.left && s_pos < s_sub[i + 1].rc.left)
            return i;
    }
    if (s_pos <= s_sub[0].rc.right) return 0;
    if (s_pos >= s_sub[len - 1].rc.left) return len - 1;
    DWORD cx = s_toolData.pos.width;
    if ((DWORD)s_pos + cx > (DWORD)s_sub[len - 1].rc.left)
    {
        POINT pt = { 1, 1 };
        return pt2index(pt, false);
    }
    assert(false);    // 走到这是出错
    return -1;
}

LPOBJSTRUCT CToolBar::create(HWND hParent)
{
    //static HINSTANCE hInst;
    //if (hInst == 0)
    //{
    //    hInst = GetModuleHandleW(0);
    //    WNDCLASSEXW wcex = { 0 };
    //    wcex.cbSize = sizeof(WNDCLASSEX);
    //    wcex.cbClsExtra = 0;
    //    wcex.cbWndExtra = 0;
    //    wcex.hInstance = hInst;
    //    wcex.hIcon = 0;
    //    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    //    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //    wcex.lpszMenuName = NULL;

    //    //GetClassInfoExW(hInst, L"static", &wcex);
    //    LPOBJSTRUCT newProc = MakeProc(this, &CToolBar::WndProc, &s_toolData);
    //    //newProc->oldProc = wcex.lpfnWndProc;
    //    newProc->oldProc = DefWindowProcW;
    //    wcex.lpfnWndProc = (WNDPROC)newProc;
    //    wcex.hInstance = hInst;
    //    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    //    wcex.lpszClassName = L"tool";

    //    ATOM atom = RegisterClassExW(&wcex);
    //    if (!atom)return 0;
    //}
    //
    //HWND hWnd = __create(0, L"tool", 0,
    //    WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_OWNERDRAW,
    //    0, 30, 0, 80, hParent, 0, hInst, 0);

    return &s_toolData;
}




bool CToolBar::AddButton(LPCWSTR text, LPCWSTR tips)
{
    size_t len = s_sub.size();
    s_sub.resize(len + 1);
    TOOLCHILD& data = s_sub[len];
    wstr sText(text);
    wstr sTips(tips);
    sText.Addref();
    sTips.Addref();
    if (len > 0)
    {
        RECT& tmp = s_sub[len - 1].rc;
        data.rc = { tmp.right, tmp.top,  tmp.right + TOOL_WIDTH, tmp.top + TOOL_HEIGHT };
    }
    else
    {
        data.rc = { 0, 0,  TOOL_WIDTH, TOOL_HEIGHT };
    }
    data.state = STATE::NORMAL;
    data.text = sText.data();
    data.tips = sTips.data();
    return true;
}
