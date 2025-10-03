#include "../EWindow_Fne_Header.h"

#include <control/CCustomList.h>

// 是否调用原函数
#define _debug_iscallback(_if) \
    if(_if)\
        return pData->fnCallProc(pData->oldProc, hWnd, message, wParam, lParam);\
    return pData->fnDefProc(hWnd, message, wParam, lParam);


// 从指定坐标获取索引
int GetIndexFromPoint(LPOBJSTRUCT pData, int x, int y);
// 从指定坐标获取索引, lParam = 低位是x, 高位是y
int GetIndexFromPoint(LPOBJSTRUCT pData, LPARAM lParam);
// 通知父窗口触发指定事件
LRESULT SendCommand(LPOBJSTRUCT pData, int code);
typedef struct LIST_DRAW_INFO
{
    LPEX_BRUSH hbrText;     // 文本画刷
    LPEX_BRUSH hbrTextHot;  // 文本热点画刷
    LPEX_BRUSH hbrTextDown; // 文本按下/选中画刷
    LPEX_BRUSH hbrBack;     // 背景画刷
    LPEX_BRUSH hbrHot;      // 热点画刷
    LPEX_BRUSH hbrDown;     // 按下画刷
    LPEX_BRUSH hbrSel;      // 选中画刷
    

}*LPLIST_DRAW_INFO;


LRESULT CALLBACK WndProc_ListBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK WndProc_ListBox_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT CALLBACK List_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

static std::map<int, LPOBJSTRUCT> m_map_list;
// 自己处理列表的消息
LRESULT CALLBACK MyListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);


LPOBJSTRUCT _subclass_ListBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll, bool isHot)
{
    if (nItemHeight < 10) nItemHeight = 24;

    static int s_id = 0xBA5C;
    static WNDPROC oldProc;
    if (!oldProc)
    {
        WNDCLASSEXA wcex;
        wcex.cbSize = sizeof(wcex);
        GetClassInfoExA(CWndBase::GetInst(), "ListBox", &wcex);
        oldProc = wcex.lpfnWndProc;
    }

    const int index = (int)SendMessageA(hWnd, LB_GETCURSEL, 0, 0);
    LPOBJSTRUCT pData = _subclass(hWnd, WndProc_ListBox, reMoveBorder, isSetBack, reMoveClassStyle);
    pData->draw.nItemHeight  = nItemHeight;
    pData->draw.isHot        = isHot;
    pData->draw.indexDown    = -1;
    pData->draw.indexHot     = -1;
    pData->draw.indexSel     = index;
    pData->fnCtlcolor        = WndProc_ListBox_CtlColor;
    pData->fnCommand         = List_OnCommand;
    pData->draw.hCanvas      = _canvas_create(pData->hWnd, 0, 0); // dx绘画
    if (pData->id == 0)
    {
        pData->SetWndLong(hWnd, GWLP_ID, s_id);
        pData->id = s_id++;
    }
    m_map_list[pData->id] = pData;
    SendMessageW(hWnd, LB_SETITEMHEIGHT, 0, s_info->scale(pData->draw.nItemHeight));
    InsertStyle(hWnd, LBS_OWNERDRAWFIXED | LBS_HASSTRINGS);
    if(0)
        SetTimer(hWnd, 10086, 1000, 0);
    if (isScroll && s_info->MY_MF_OWNERDRAW)
    {
        pData->drawScroll = ScrollCreate(pData->hWnd, pData->draw.color.crBorder,
            0,          // m_isSysListView, 如果是超级列表框的话这个值就是超级列表框的表项高度
            false,      // m_isSysTreeView, 是否为树型框, 如果为真, 则先设置滚动条位置, 后发送滚动消息, 为假则先发送滚动消息后设置位置
            true,       // m_isCallSetScroll, 是否调用原来的设置滚动条位置函数
            false,      // isSubClass, 是否子类化, 如果为true, 内部会子类化这个窗口, 如果为假, 则必须外部把消息传递到 OnWndProc 方法里
            true        // isReCalcSize, 是否重新计算尺寸, 为true则内部会调用 SetWindowPos 进行重新计算非客户区
        );
    }

    return pData;
}


// 从列表框中获取一个字符串
int _listbox_GetItemText(HWND hWnd, int index, wstr& str) {
    int len = (int)SendMessageW(hWnd, LB_GETTEXTLEN, index, 0) + 1;
    str.resize(len);
    SendMessageW(hWnd, LB_GETTEXT, index, (LPARAM)str.data());
    len = (int)wcslen(str.c_str());
    str.resize(len);
    return len;
}

bool _draw_listBox(LPOBJSTRUCT pData, HDC hdcSrc)
{
    RECT rc;
    GetClientRect(pData->hWnd, &rc);
    RECT rcClient = rc;
    const int cxClient = rc.right - rc.left;
    const int cyClient = rc.bottom - rc.top;
    LPCANVAS hCanvas = pData->draw.hCanvas;
    if ((int)hCanvas->GetWidth() < cxClient || (int)hCanvas->GetHeight() < cyClient)
        _canvas_resize(pData->draw.hCanvas, cxClient, cyClient);

    _canvas_begindraw(hCanvas);
    _canvas_clear(hCanvas, RGB2ARGB(pData->draw.color.crBackGround, 255));

    LIST_DRAW_INFO draw;

    draw.hbrText     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crText, 255));
    draw.hbrTextHot  = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crTextHot, 255));
    draw.hbrTextDown = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crTextDown, 255));

    draw.hbrBack     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crBackGround, 255));
    draw.hbrDown     = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crDown, 255));
    draw.hbrHot      = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crHot, 255));
    draw.hbrSel      = s_info->pfnCreateBrushDx(RGB2ARGB(pData->draw.color.crSelected, 255));


    const int nTopIndex = (int)SendMessageA(pData->hWnd, LB_GETTOPINDEX, 0, 0);
    const int count     = (int)SendMessageA(pData->hWnd, LB_GETCOUNT, 0, 0);
    wstr text;
    RECT_F rcItem;
    LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    for (int i = nTopIndex; i < count; i++)
    {
        SendMessageW(pData->hWnd, LB_GETITEMRECT, i, (LPARAM)&rc);
        //if(Rect_isOverlap(rc, ps))
        if (rc.top >= cyClient) break;    //超出了可视范围就退出绘画
        rc.right -= 1;
        rcItem(rc);
        LPEX_BRUSH hbrText = draw.hbrText;
        LPEX_BRUSH hbrBack = 0;
        if (pData->draw.indexDown == i)
        {
            hbrText = draw.hbrTextDown;
            hbrBack = draw.hbrDown;
        }
        else if (pData->draw.indexHot == i)
        {
            hbrText = draw.hbrTextHot;
            hbrBack = draw.hbrHot;
        }
        else if (pData->draw.indexSel == i)
        {
            hbrText = draw.hbrTextDown;
            hbrBack = draw.hbrSel;
        }

        if (hbrBack)
        {
            RECT_F rcItemF(rcItem);
            int oldMode = 0;
            if (s_info->isGpRender)
                _canvas_SetAntialiasMode(hCanvas, 0, &oldMode);
            _canvas_fillRectangle(hCanvas, hbrBack, &rcItemF);
            if (s_info->isGpRender)
                _canvas_SetAntialiasMode(hCanvas, oldMode, 0);
        }

        rcItem.left += 5.f;
        const int len = _listbox_GetItemText(pData->hWnd, i, text);
        const int fmt = DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
        _canvas_drawtext(hCanvas, hFont, hbrText, text.c_str(), len, fmt, &rcItem);
    }

    HDC hdcD2D = _canvas_getdc(pData->draw.hCanvas);
    BitBlt(hdcSrc, 0, 0, cxClient, cyClient, hdcD2D, 0, 0, SRCCOPY);
    _canvas_releasedc(hCanvas);
    bool ret = _canvas_enddraw(hCanvas);
    return ret;
}

inline void InvalidateItem(HWND hWnd, int index) {
    if (index == -1)
    {
        InvalidateRect(hWnd, 0, 0);
        return;
    }
    RECT rcItem;
    SendMessageW(hWnd, LB_GETITEMRECT, index, (LPARAM)&rcItem);
    ::InvalidateRect(hWnd, &rcItem, 0);
}

LRESULT CALLBACK WndProc_ListBox_CtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    const HDC hdc = (HDC)wParam;
    const HWND hControl = (HWND)lParam;
    LPOBJSTRUCT pCtlData = GetDataFromHwnd(hControl);
    if (!pCtlData)
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

    InvalidateRect(hControl, 0, 0);
    return (LRESULT)pCtlData->draw.color.hbrBackGround;
}
LRESULT CALLBACK WndProc_ListBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    switch (message)
    {
    case LB_ADDSTRING:
    case LB_INSERTSTRING:
    case LB_DELETESTRING:
    case LB_SELITEMRANGEEX:
    case LB_RESETCONTENT:
    case LB_GETTEXT:
    case LB_GETTEXTLEN:
    case LB_GETCOUNT:
    case LB_SELECTSTRING:
    case LB_DIR:
    case LB_GETTOPINDEX:
    case LB_FINDSTRING:
    case LB_GETSELCOUNT:
    case LB_GETSELITEMS:
    case LB_SETTABSTOPS:
    case LB_GETHORIZONTALEXTENT:
    case LB_SETHORIZONTALEXTENT:
    case LB_SETCOLUMNWIDTH:
    case LB_ADDFILE:
    case LB_SETTOPINDEX:
    case LB_GETITEMRECT:
    case LB_GETITEMDATA:
    case LB_SETITEMDATA:
    case LB_SELITEMRANGE:
    case LB_SETANCHORINDEX:
    case LB_GETANCHORINDEX:
    case LB_SETCARETINDEX:
    case LB_GETCARETINDEX:
    case LB_SETITEMHEIGHT:
    case LB_GETITEMHEIGHT:
    case LB_FINDSTRINGEXACT:
    case LB_SETLOCALE:
    case LB_GETLOCALE:
    case LB_SETCOUNT:
    case LB_INITSTORAGE:
    case LB_ITEMFROMPOINT:
    case LB_GETLISTBOXINFO:
    case LB_MSGMAX:
    //case LB_SETCURSEL:
    //case LB_GETCURSEL:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    case LB_GETCURSEL:
        return pData->draw.indexSel;
    case LB_SETCURSEL:
        return pData->draw.indexSel = (int)(LONG_PTR)wParam;
    default:
        // 列表的消息列表自己处理, 其他消息自己处理
        return MyListProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

LRESULT CALLBACK MyListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    IDrawScroll* pObj = (IDrawScroll*)pData->drawScroll;
    if (pObj && s_info->MY_MF_OWNERDRAW)
    {
        LRESULT ret = 0;
        if (pObj->OnWndProc(pData->oldProc, hWnd, message, wParam, lParam, &ret))
        {
            return ret;
        }
    }
    switch (message)
    {
    case WM_DPICHANGED:
    {
        EWindow_DPIChange(pData);
        SendMessageW(pData->hWnd, LB_SETITEMHEIGHT, 0, s_info->scale(pData->draw.nItemHeight));
        break;
    }
    case WM_ERASEBKGND:
        _draw_listBox(pData, (HDC)wParam);
        return 1;
    case WM_NCPAINT:
        return WndProc_DefNcPaint(hWnd, message, wParam, lParam, pData);
        InvalidateRect(hWnd, 0, 0);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    case WM_PRINT:
        _draw_listBox(pData, (HDC)wParam);
        break;
    case WM_PRINTCLIENT:
        //CallWindowProcA(oldProc, hWnd, message, wParam, lParam);
        _draw_listBox(pData, (HDC)wParam);
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        _draw_listBox(pData, ps.hdc);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (!pData->draw.isHot)     // 不设置热点则不处理
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        //return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        TRACKMOUSEEVENT tme = { 0 };
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hWnd;
        tme.dwHoverTime = HOVER_DEFAULT;
        TrackMouseEvent(&tme);

        const int index = (int)SendMessageA(hWnd, LB_ITEMFROMPOINT, 0, lParam);
        if (HIWORD(index))          // 高位不为0表示鼠标不在项目内
        {
            if (pData->draw.indexHot != -1)
            {
                InvalidateItem(hWnd, pData->draw.indexHot);
                pData->draw.indexHot = -1;
            }
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
            //return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        }

        // 走到这里就是鼠标在项目内
        if (index != pData->draw.indexHot)
        {
            // 走到这就是鼠标不是在上一次热点的项目, 需要重画两个表项
            InvalidateItem(hWnd, pData->draw.indexHot);
            pData->draw.indexHot = index;
            InvalidateItem(hWnd, pData->draw.indexHot);
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_MOUSELEAVE:
    {
        pData->draw.indexHot = -1;
        InvalidateRect(hWnd, 0, 0);
        return 0;
    }
    case WM_MOUSEWHEEL:
    case WM_VSCROLL:
    case WM_HSCROLL:
        InvalidateRect(hWnd, 0, 0);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
#define __iscallback 0
    case WM_LBUTTONDOWN:
    {
        const int index = GetIndexFromPoint(pData, lParam);
        if (!__query(pData->draw.state, STATE::FOCUS))
        {
            pData->draw.state = (STATE)(pData->draw.state | STATE::FOCUS);
        }

        pData->draw.indexDown = HIWORD(index) ? -1 : LOWORD(index); // 高位不为0表示鼠标不在项目内
        InvalidateRect(hWnd, 0, 0);
        SetFocus(hWnd);
        _debug_iscallback(__iscallback);
    }
    case WM_LBUTTONUP:
    {
        const int index = GetIndexFromPoint(pData, lParam);
        if (HIWORD(index))          // 高位不为0表示鼠标不在项目内
        {
            SendCommand(pData, LBN_SELCANCEL);
            InvalidateRect(hWnd, 0, 0);
            _debug_iscallback(__iscallback);
        }
        if (index == pData->draw.indexDown)     // 放开的是按下的项目, 触发选择
        {
            pData->draw.indexSel = index;
            SendCommand(pData, LBN_SELCHANGE);
        }
        pData->draw.indexDown = -1;
        InvalidateRect(hWnd, 0, 0);
        _debug_iscallback(__iscallback);
    }
    case WM_LBUTTONDBLCLK:
    {
        SendCommand(pData, LBN_DBLCLK);
        _debug_iscallback(__iscallback);
    }
    //case WM_NCPAINT:
    //    InvalidateRect(hWnd, 0, 0);
    //    break;
    case WM_DESTROY:
    {
        delete pData->drawScroll;
        pData->drawScroll = 0;
        _canvas_destroy(pData->draw.hCanvas);

        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_STYLECHANGING:
    {
        LPSTYLESTRUCT ss = (LPSTYLESTRUCT)lParam;
        //wstr::dbg(L"即将改变样式, 新样式 = %d, 旧样式 = %d\n", ss->styleNew, ss->styleOld);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_STYLECHANGED:
    {
        LPSTYLESTRUCT ss = (LPSTYLESTRUCT)lParam;
        //wstr::dbg(L"样式改变完毕, 新样式 = %d, 旧样式 = %d\n", ss->styleNew, ss->styleOld);
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_TIMER:
    {
        DWORD style = pData->GetWndLong(hWnd, GWL_STYLE);
        DWORD styleNew = style | LBS_OWNERDRAWFIXED;
        //wstr::dbg(L"时钟id = %d, 新样式 = %d, 旧样式 = %d\n", wParam, styleNew, style);
        pData->SetWndLong(hWnd, GWL_STYLE, styleNew);
        return 0;
    }
    case WM_VKEYTOITEM:
    {

        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    case WM_KEYDOWN:
    {
        wstr::dbg(L"列表框按键按下, 按键 = %d\n", wParam);
        switch (wParam)
        {
        case VK_LEFT:
        case VK_UP:
        case VK_RIGHT:
        case VK_DOWN:
        {
            // 向上选择选中项
            if (wParam == VK_LEFT || wParam == VK_UP)
            {
                if (--pData->draw.indexSel < 0)
                    pData->draw.indexSel = 0;
            }
            else
            {
                int count = SendMessageA(hWnd, LB_GETCOUNT, 0, 0);

                if (++pData->draw.indexSel >= count)
                    pData->draw.indexSel = count - 1;
            }
            int topIndex = SendMessageA(hWnd, LB_GETTOPINDEX, 0, 0);
            int itemHeight = SendMessageA(hWnd, LB_GETITEMHEIGHT, 0, 0);
            int visibleCount = pData->pos.height / itemHeight;

            int bottomIndex = visibleCount + topIndex;
            if (!(pData->draw.indexSel >= topIndex && pData->draw.indexSel < bottomIndex))  // 不是在可视范围内, 需要让他可视
            {
                int showIndex = pData->draw.indexSel;   // 显示到最顶的项目, 如果是向上按键, 那就显示顶部项目的上一个项目
                if (pData->draw.indexSel == topIndex + visibleCount)
                {
                    // 选中的项目是可视的最后一个项目, 那就显示顶部的第二个可视的项目
                    showIndex = topIndex + 1;
                }
                SendMessageA(hWnd, LB_SETTOPINDEX, showIndex, 0);
            }
            InvalidateRect(hWnd, 0, 0);
            return 0;
            break;
        }
        default:
            break;
        }
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    default:
        return pData->fnCallProc(pData->oldProc, hWnd, message, wParam, lParam);
        return pData->fnDefProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK List_OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPDRAWITEMSTRUCT dw = (LPDRAWITEMSTRUCT)lParam;
    auto it = m_map_list.find(dw->CtlID);
    if (it == m_map_list.end())
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    LPOBJSTRUCT pList = it->second;

    HBRUSH hbrBack = 0;
    if (dw->itemState & ODS_SELECTED)
        hbrBack = pList->draw.color.hbrBackGround;
    else
        hbrBack = pList->draw.color.hbrDown;

    SetBkMode(dw->hDC, TRANSPARENT);
    FillRect(dw->hDC, &dw->rcItem, hbrBack);

    int len = (int)SendMessageW(pList->hWnd, LB_GETTEXTLEN, dw->itemID, 0);
    wstr text(len + 1);
    SendMessageW(pList->hWnd, LB_GETTEXT, dw->itemID, (LPARAM)text.data());

    DrawTextW(dw->hDC, text.c_str(), len, &dw->rcItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    return 1;
}

LRESULT CALLBACK List_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    const int id = LOWORD(wParam);
    const int code = HIWORD(wParam);
    HWND hList = (HWND)lParam;
    switch (code)
    {
    case LBN_SELCHANGE:
    {
        LPOBJSTRUCT pList = GetDataFromHwnd(hList);
        wstr::dbg(L"列表框当前选中项 = %d\n", SendMessageW(hList, LB_GETCURSEL, 0, 0));
        //pList->draw.indexSel = SendMessageW(hWnd, LB_GETCURSEL, 0, 0);
        break;
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}
int GetIndexFromPoint(LPOBJSTRUCT pData, int x, int y)
{
    return SendMessageA(pData->hWnd, LB_ITEMFROMPOINT, 0, MAKELONG(x, y));
}

int GetIndexFromPoint(LPOBJSTRUCT pData, LPARAM lParam)
{
    return SendMessageA(pData->hWnd, LB_ITEMFROMPOINT, 0, lParam);
}

LRESULT SendCommand(LPOBJSTRUCT pData, int code)
{
    if (pData->hWnd == s_info->eWnd->GetStateTabFind1() || pData->hWnd == s_info->eWnd->GetStateTabFind2())
        return SendMessageA(s_info->eWnd->GetState32770(), WM_COMMAND, MAKELONG(pData->id, code), (LPARAM)pData->hWnd);
    return SendMessageA(pData->hParent, WM_COMMAND, MAKELONG(pData->id, code), (LPARAM)pData->hWnd);
}
