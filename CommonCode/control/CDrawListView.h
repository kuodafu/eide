#pragma once
#include "CDrawHeader.h"
#pragma comment(lib, "Comctl32.lib")
#if (_WIN32_WINNT < 0x602)
#   define __USEXP   // 支持xp
#endif


class CDrawListView : public CDrawControl
{
    int m_indexHot;
    int m_indexDown;
    bool m_isHot;
    CDrawHeader m_header;
protected:

public:
    CDrawListView() : CDrawControl(), m_indexHot(-1), m_indexDown(-1), m_isHot(false)
    {

    }
    virtual ~CDrawListView()
    {

    }

public:

    // 绑定组件, 绑定后会子类化组件, 然后对组件进行重画操作
    // hWnd = 绑定的窗口, 如果为0则不子类化窗口, 只记录配色信息, 可以在外部调用指定方法处理
    // pClr = 窗口使用的配色结构, 如果为0会从当前进程的map里获取, 获取失败则使用默认色绘画
    // isRef = 参数2是否一直有效, 参数为真的时候内部不会分配配色结构, 会一直使用参数2, 所以为真时请确保参数2指针的存活
    // isHot = 是否允许热点项, 鼠标滑动后会出现热点项
    virtual bool bind(HWND hWnd = 0, PTHEMECOLOR_STRUCT pClr = 0, bool isRef = false, bool isHot = false)
    {
        m_isHot = isHot;
        m_hWnd = hWnd;
        if (IsWindow(hWnd))
        {
            HWND hHeader = ListView_GetHeader(hWnd);
            m_header.bind(hHeader, pClr, isRef);
        }
        return CDrawControl::bind(hWnd, pClr, isRef);
    }

    

    // 需要可视后设置才有效
    static bool SetItemHeight(HWND hWnd, int height)
    {
        DWORD style = (DWORD)GetWindowLongPtrW(hWnd, GWL_STYLE);
        SetWindowLongPtrW(hWnd, GWL_STYLE, style | LVS_OWNERDRAWFIXED);
        WINDOWPOS pos = { 0 };
        pos.hwnd = hWnd;
        SendMessageA(hWnd, WM_WINDOWPOSCHANGED, 0, (LPARAM)&pos);

        //SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
        SetWindowLongPtrW(hWnd, GWL_STYLE, style);
        SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOCOPYBITS);
        //SendMessageA(hWnd, LVM_UPDATE, 0, 0);
        return true;
    }

    // 开始拖拽, 设置起始和结束的索引项, 会设置焦点然后捕获鼠标
    static void OnBeginDrag(HWND hWnd, int iItem, int& iStart, int& iEnd)
    {
        SetFocus(hWnd);
        iStart = iItem;
        iEnd = iItem;
        SetCapture(hWnd);
    }
    // 拖拽中, 会根据起始位置和当前位置来计算选中项, 然后向列表发送被项目被选择
    static void OnDragIng(HWND hWnd, int iItem, int& iStart, int& iEnd)
    {
        if (iStart == -1 || iItem == -1)
            return;

        if (iEnd == iItem)
            return;  // 上次选择的还是这个项目, 不处理


        const int oldStart = min(iStart, iEnd);
        const int oldEnd = max(iStart, iEnd);

        const int newStart = min(iStart, iItem);
        const int newEnd = max(iStart, iItem);
        iEnd = iItem;

        LVITEMW lv_item = { 0 };
        lv_item.mask = LVIF_STATE;

        auto pfn_state = [&](int i, bool isSel)
        {
            lv_item.state = LVIS_SELECTED;
            lv_item.stateMask = lv_item.state;

            // 保证选中项不能被清除选择
            if (!isSel && iStart != i && i != iItem)
            {
                lv_item.state = 0;
                lv_item.stateMask = LVIS_SELECTED;  // 移除这个选中状态
            }

            SendMessageW(hWnd, LVM_SETITEMSTATE, i, (LPARAM)&lv_item);
        };

        bool isSel = false;
        int start = 0;
        int end = 0;

        if (newEnd == oldEnd)
        {
            // 结尾一样, 那就是往上选择, 判断起始位置, 是加入选区还是减小选区
            isSel = newStart < oldStart;
            start = isSel ? newStart : oldStart;    // 新的位置大于旧的位置, 那就是减小
            end = isSel ? oldStart : newStart;    // 新的位置小于旧的位置, 那就是增加

        }
        else if (newStart == oldStart)
        {
            // 起始位置相同, 那就是向后选择
            isSel = newEnd > oldEnd;
            start = isSel ? oldEnd : newEnd;    // 新结束位置大于旧的位置, 那就是往后选择
            end = isSel ? newEnd : oldEnd;    // 新结束位置小于旧的位置, 那就是往前选择

        }
        else
        {
            __debugbreak(); // 能走到这应该就是bug, 先断下测试
        }


        for (int i = start; i <= end; i++)
            pfn_state(i, isSel);

    }
private:
    virtual LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        if (!m_isHot)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        // 需要处理热点项
        LVHITTESTINFO info;
        info.pt.x = GET_X_LPARAM(lParam);
        info.pt.y = GET_Y_LPARAM(lParam);
        int index = ListView_SubItemHitTest(hWnd, &info);
        index = info.iItem;
        if (index != -1)
        {
            if (index == m_indexHot)
            {
                // 本次移动的还是上次的项目, 不需要重画
                return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
            }
            // 移动的不是上一次的项目, 需要重画上一次的项目, 在设置当前项目
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.dwHoverTime = INFINITE;
            tme.hwndTrack = hWnd;
            TrackMouseEvent(&tme);
            InvalidateItem(hWnd, index, true);
        }
        else
        {
            // 移动的不在任何项目内, 需要重画之前的热点项
            if (m_indexHot != -1)
            {
                InvalidateItem(hWnd, -1, true);
            }
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    virtual LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        switch (message)
        {
        case WM_HSCROLL:
        case WM_MOUSEHWHEEL:
        {
            LRESULT ret = CDrawControl::WndProc(hWnd, message, wParam, lParam, oldProc);
            InvalidateRect(hWnd, 0, 0);
            return ret;
            break;
        }
        default:
            return CDrawControl::WndProc(hWnd, message, wParam, lParam, oldProc);
        }
        return 0;
    }
    virtual LRESULT OnMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        switch (message)
        {
        case WM_MOUSEMOVE:
            return OnMouseMove(hWnd, message, wParam, lParam, oldProc);
        case WM_LBUTTONDOWN:
        {
            LVHITTESTINFO info;
            info.pt.x = GET_X_LPARAM(lParam);
            info.pt.y = GET_Y_LPARAM(lParam);
            int index = ListView_SubItemHitTest(hWnd, &info);
            InvalidateItem(hWnd, info.iItem, false);
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        case WM_MOUSELEAVE:
        {
            if (m_indexHot != -1)
                InvalidateItem(hWnd, -1, true);
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }

        default:
            break;
        }
        return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
    }

    struct LV_DRAW
    {
        NMLVCUSTOMDRAW nmCustomDraw;
        std::vector<int> arr_index;
        std::vector<int> arr_widths;
        int right;
        LVITEMW item;
        HWND hHeader;
        HDC hdc;
        RECT rcItem;
        int cxClient;
        int cyClient;
        COLORREF clrBack;
        COLORREF clrText;
        COLORREF clrLine;
        COLORREF clrHot;
        COLORREF clrDown;
        DWORD styleList;
        HFONT hFont;
        DWORD dwCustom;
        LONG_PTR id;
        HWND hParent;
        HIMAGELIST hImageList;
        int cxIcon;
        int cyIcon;
        LV_DRAW()
        {
            const int offset = offsetof(LV_DRAW, nmCustomDraw);
            const int cbSize = sizeof(LV_DRAW) - offset;
            memset(&nmCustomDraw, 0, cbSize);
        }
    };
    virtual LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        LV_DRAW lv_draw;
        RECT rc;
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        lv_draw.cxClient = rc.right - rc.left;
        lv_draw.cyClient = rc.bottom - rc.top;
        LPMEMDCBITMAP md = md_create(lv_draw.cxClient, lv_draw.cyClient);
        HDC hdc = md->hdc;

        lv_draw.clrBack = RGB(255, 255, 255);
        lv_draw.clrText = RGB(0, 0, 0);
        lv_draw.clrLine = RGB(240, 240, 240);
        lv_draw.clrHot = RGB(217, 235, 249);
        lv_draw.clrDown = RGB(188, 220, 244);

        if (m_pClr)
        {
            lv_draw.clrBack = m_pClr->crBack, lv_draw.clrText = m_pClr->crText, lv_draw.clrLine = m_pClr->crBorder;
            lv_draw.clrHot = m_pClr->crHot, lv_draw.clrDown = m_pClr->crHot;
        }
        FillRect(hdc, &rc, CThemeColor::GetBrush(lv_draw.clrBack));
        COLORREF clrTextOld = ::SetTextColor(hdc, lv_draw.clrText);
        SetBkMode(hdc, TRANSPARENT);
        SetBkColor(hdc, lv_draw.clrBack);

        lv_draw.hFont = (HFONT)SendMessageW(hWnd, WM_GETFONT, 0, 0);
        if (!lv_draw.hFont)
            lv_draw.hFont = CWndBase::DefaultFont();
        SelectObject(hdc, lv_draw.hFont);
        HPEN hPenLine = CThemeColor::GetPen(PS_SOLID, 1, lv_draw.clrLine);
        SelectObject(hdc, hPenLine);
        NMCUSTOMDRAW& nmcd = lv_draw.nmCustomDraw.nmcd;

        lv_draw.hdc = hdc;
        nmcd.hdr.hwndFrom = m_hWnd;
        lv_draw.id = GetDlgCtrlID(nmcd.hdr.hwndFrom);
        lv_draw.hParent = GetParent(nmcd.hdr.hwndFrom);
        nmcd.hdr.idFrom = lv_draw.id;

        lv_draw.dwCustom = CICustomDrawNotify(lv_draw, CDDS_PREPAINT, &nmcd);
        // 不是自定义绘画才往下走
        if (!(lv_draw.dwCustom & CDRF_SKIPDEFAULT))
        {
            lv_draw.styleList = ListView_GetExtendedListViewStyle(m_hWnd);
            const int nTopIndex = ListView_GetTopIndex(hWnd);
            const int nItemCount = ListView_GetItemCount(hWnd);

            RECT& rcItem = lv_draw.rcItem;
            LVITEMW& item = lv_draw.item;
            item.mask = LVIF_INDENT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
            item.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
            item.iSubItem = 0;

            DWORD style = (DWORD)GetWindowLongPtrW(hWnd, GWL_STYLE);
            bool isIcon = (style & LVS_LIST) == 0;  // 只要为0, 就表示是大图标
            lv_draw.hImageList = ListView_GetImageList(hWnd, isIcon ? LVSIL_NORMAL : LVSIL_SMALL);
            if (lv_draw.hImageList)
                ImageList_GetIconSize(lv_draw.hImageList, &lv_draw.cxIcon, &lv_draw.cyIcon);

            lv_draw.hHeader = ListView_GetHeader(hWnd);
            int nColumnCount = (int)SendMessageW(lv_draw.hHeader, HDM_GETITEMCOUNT, 0, 0);
            if (nColumnCount <= 0)
                nColumnCount = 1;


            int first_top = 0;
            int itemHeight = 0;

            lv_draw.right = -GetScrollPos(hWnd, SB_HORZ);
            std::vector<int>& arr_index = lv_draw.arr_index; arr_index.resize(nColumnCount);
            std::vector<int>& arr_widths = lv_draw.arr_widths; arr_widths.resize(nColumnCount);
            ListView_GetColumnOrderArray(hWnd, nColumnCount, &arr_index[0]);
            for (int& iSubItem : arr_index)
                arr_widths[iSubItem] = ListView_GetColumnWidth(hWnd, iSubItem);

            for (int i = nTopIndex; i < nItemCount; i++)
            {
                item.iItem = i;
                ListView_GetItemRect(hWnd, i, &rcItem, LVIR_BOUNDS);
                if (i == nTopIndex)
                    first_top = rcItem.top, itemHeight = rcItem.bottom - rcItem.top;
                if (rcItem.top > lv_draw.cyClient)
                    break;
                if (!CDrawHeader::Rect_isOverlap(rcItem, ps.rcPaint))    // 不是重画区域就进行下一次循环
                    continue;
                ListView_GetItem(hWnd, &item);
                item.iItem = i;

                DrawItem(lv_draw);

            }

            // 有绘画表格线才绘画
            if ((lv_draw.styleList & LVS_EX_GRIDLINES) == LVS_EX_GRIDLINES)
            {
                for (int& iWidth : arr_widths)
                {
                    lv_draw.right += iWidth;

                    if (lv_draw.right >= 0)
                    {
                        MoveToEx(hdc, lv_draw.right, rc.top, 0);
                        LineTo(hdc, lv_draw.right, rc.bottom);
                    }
                }
                //TODO 项目高度=0的时候就是没有项目, 需要获取表项高度, 看看怎么获取
                if (itemHeight == 0)
                {
                    //itemHeight = LOWORD(data->propNew.nItemHeight);
                    //if (itemHeight == 0)
                    //    itemHeight = HIWORD(data->propNew.nItemHeight);

                    // 为0表示没有表项, 没有表项的话rcItem的顶边就是0, 这里需要把顶边调整到表头的底边
                    RECT rcHead;
                    GetWindowRect(lv_draw.hHeader, &rcHead);
                    rcItem.top = rcHead.bottom - rcHead.top;
                    rcItem.bottom = rcItem.top + itemHeight;
                }

                rcItem.left = 0;
                rcItem.right = lv_draw.right;
                rcItem.top = first_top;
                rcItem.bottom = rcItem.top + itemHeight;

                while (itemHeight)
                {
                    const int top = rcItem.bottom - 1;
                    if (top > lv_draw.cyClient)
                        break;  // 超过窗口尺寸就不继续绘画了

                    MoveToEx(hdc, rcItem.left, top, 0);
                    LineTo(hdc, rcItem.right, top);

                    rcItem.top += itemHeight;
                    rcItem.bottom += itemHeight;
                }

            }
            // notify parent afterwards if they want us to
            if (lv_draw.dwCustom & CDRF_NOTIFYPOSTPAINT)
            {
                CICustomDrawNotify(lv_draw, CDDS_POSTPAINT, &nmcd);
            }
        }
        BitBlt(ps.hdc, 0, 0, lv_draw.cxClient, lv_draw.cyClient, hdc, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        md_destroy(md);
        return 0;
    }

    inline void DrawItem(LV_DRAW& lv_draw)
    {
        HBRUSH hbrBack = 0;
        NMLVCUSTOMDRAW& nmCustomDraw = lv_draw.nmCustomDraw;
        NMLVDISPINFOW dispInfo = { 0 };

        UINT uItemState = 0;
        COLORREF clrBack = lv_draw.clrBack;
        COLORREF clrText = lv_draw.clrText;
        if ((lv_draw.item.state & LVIS_FOCUSED) == LVIS_FOCUSED)
        {
            clrBack = lv_draw.clrDown;
            hbrBack = CThemeColor::GetBrush(clrBack);
            uItemState |= CDIS_FOCUS;
        }
        else if (lv_draw.item.iItem == m_indexHot)
        {
            clrBack = lv_draw.clrHot;
            hbrBack = CThemeColor::GetBrush(clrBack);
            uItemState |= CDIS_HOT;
        }
        else if ((lv_draw.item.state & LVIS_SELECTED) == LVIS_SELECTED)
        {
            clrBack = lv_draw.clrDown;
            hbrBack = CThemeColor::GetBrush(clrBack);
            uItemState |= CDIS_SELECTED;
        }
        if (hbrBack)
            FillRect(lv_draw.hdc, &lv_draw.rcItem, hbrBack);

        if ((lv_draw.item.state & LVIS_SELECTED) == LVIS_SELECTED)
            uItemState |= CDIS_SELECTED;


        //rcf.left += 5.f;
        int fmt = DT_VCENTER | DT_SINGLELINE | DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS;




        HDC hdc = lv_draw.hdc;
        HWND hParent = GetParent(nmCustomDraw.nmcd.hdr.hwndFrom);

        nmCustomDraw.clrText = lv_draw.clrText;
        nmCustomDraw.clrTextBk = clrBack;
        lv_draw.dwCustom = CICustomDrawNotify(lv_draw, CDDS_ITEMPREPAINT, &nmCustomDraw.nmcd);
        //DWORD ret_state = (DWORD)SendMessageW(hParent, WM_NOTIFY, nmCustomDraw.nmcd.hdr.idFrom, (LPARAM)&nmCustomDraw);
        if (!(lv_draw.dwCustom & CDRF_SKIPDEFAULT))
        {
            RECT& rcItem = lv_draw.rcItem;
            const int CCHLABELMAX = 260;
            wchar_t ach[CCHLABELMAX];
            for (int nColIndex : lv_draw.arr_index)
            {
                DWORD dwCustom = 0;

                if (nColIndex == 0)
                {
                    dispInfo.item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT;
                }
                else
                {
                    // Next time through, we only want text for subitems...
                    dispInfo.item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
                }

                dispInfo.hdr = nmCustomDraw.nmcd.hdr;
                dispInfo.item.iItem = lv_draw.item.iItem;
                dispInfo.item.iSubItem = nColIndex;
                dispInfo.item.pszText = ach;
                dispInfo.item.cchTextMax = CCHLABELMAX;
                ListView_GetItem(m_hWnd, &dispInfo.item);
                //CCSendNotify(&lv_draw, LVN_GETDISPINFOW, &dispInfo.hdr);


                RECT rcCol; // 列矩形, 可以获取左边和右边
                SendMessage(lv_draw.hHeader, HDM_GETITEMRECT, nColIndex, (LPARAM)&rcCol);
                const int nColWidth = rcCol.right - rcCol.left;

                nmCustomDraw.nmcd.hdc = hdc;
                nmCustomDraw.uAlign = fmt;
                nmCustomDraw.iSubItem = nColIndex;

                nmCustomDraw.nmcd.dwItemSpec = dispInfo.item.iItem;
                nmCustomDraw.nmcd.lItemlParam = dispInfo.item.lParam;
                nmCustomDraw.nmcd.rc.left = rcCol.left + lv_draw.right;
                nmCustomDraw.nmcd.rc.top = rcItem.top;
                nmCustomDraw.nmcd.rc.right = nmCustomDraw.nmcd.rc.left + nColWidth;
                nmCustomDraw.nmcd.rc.bottom = rcItem.bottom;
                if (nmCustomDraw.nmcd.rc.right <= 0)
                    continue;   // 右边为0, 那就是横向滚动条滚动了, 这个列没显示在屏幕中, 不绘画

                RECT& rcText = nmCustomDraw.rcText;
                rcText = nmCustomDraw.nmcd.rc;
                rcText.left += 5;
                if (dispInfo.item.iImage != -1)
                    rcText.left += lv_draw.cxIcon;

                if (lv_draw.dwCustom & CDRF_NOTIFYSUBITEMDRAW)    // 有通知绘画子项
                {
                    // 子项绘画前
                    nmCustomDraw.nmcd.uItemState = uItemState;
                    dwCustom = CICustomDrawNotify(lv_draw, CDDS_SUBITEM | CDDS_ITEMPREPAINT, &nmCustomDraw.nmcd);
                    nmCustomDraw.nmcd.uItemState = uItemState;

                    if (dwCustom & CDRF_SKIPDEFAULT)
                        continue;   // 用户自行绘制, 这里不继续往下绘制
                }

                if (lv_draw.hImageList && dispInfo.item.iImage >= 0)
                    DrawItem_Icon(m_hWnd, hdc, dispInfo.item, lv_draw.hImageList, dispInfo.item.iImage);

                ::SetTextColor(hdc, nmCustomDraw.clrText);
                ::SetBkColor(hdc, nmCustomDraw.clrTextBk);
                DrawTextW(hdc, dispInfo.item.pszText, -1, &rcText, fmt);
                ::SetTextColor(hdc, clrText);
                ::SetBkColor(hdc, clrBack);

                if (dwCustom & CDRF_NOTIFYPOSTPAINT)
                {
                    // 子项绘画结束
                    
                    CICustomDrawNotify(lv_draw, CDDS_SUBITEM | CDDS_ITEMPOSTPAINT, &nmCustomDraw.nmcd);
                }

            }

            if (lv_draw.dwCustom & CDRF_NOTIFYPOSTPAINT)
            {
                lv_draw.nmCustomDraw.iSubItem = 0;
                CICustomDrawNotify(lv_draw, CDDS_ITEMPOSTPAINT, &lv_draw.nmCustomDraw.nmcd);
            }

            if (lv_draw.dwCustom & CDRF_NEWFONT)
            {
                SelectObject(hdc, lv_draw.hFont);
            }

        }

        // DrawItem 函数结束
    }

    LRESULT CCSendNotify(LV_DRAW* lv_draw, int code, LPNMHDR hdr) const
    {
        hdr->code = code;
        hdr->hwndFrom = m_hWnd;
        HWND hParent = 0;
        if (lv_draw)
        {
            hdr->idFrom = lv_draw->id;
            hParent = lv_draw->hParent;
        }
        else
        {
            hdr->idFrom = GetDlgCtrlID(hdr->hwndFrom);
            hParent = GetParent(hdr->hwndFrom);
        }
        return SendMessageW(hParent, WM_NOTIFY, hdr->idFrom, (LPARAM)hdr);
    }

    DWORD CICustomDrawNotify(LV_DRAW& lv_draw, DWORD dwStage, LPNMCUSTOMDRAW lpnmcd) const
    {
        DWORD dwRet = CDRF_DODEFAULT;


        // bail if...


        // this is an item notification, but an item notification wasn't asked for
        if ((dwStage & CDDS_ITEM) && !(lv_draw.dwCustom & CDRF_NOTIFYITEMDRAW)) {
            return dwRet;
        }

        lpnmcd->dwDrawStage = dwStage;
        dwRet = (DWORD)CCSendNotify(&lv_draw, NM_CUSTOMDRAW, &lpnmcd->hdr);

        return dwRet;
    }
    inline void DrawItem_Icon(HWND hWnd, HDC hdc, LVITEMW& item, HIMAGELIST hImageList, int index)
    {
        RECT rcIcon = { 0 };
        ListView_GetItemRect(hWnd, index, &rcIcon, LVIR_ICON);
        const int cx = rcIcon.right - rcIcon.left;
        const int cy = rcIcon.bottom - rcIcon.top;
        if (cx <= 0)
            return;

        int cxIcon = 0;
        int cyIcon = 0;
        ImageList_GetIconSize(hImageList, &cxIcon, &cyIcon);

        const int offsetX = (cx - cxIcon) / 2;
        const int offsetY = (cy - cyIcon) / 2;
        const int x = rcIcon.left + offsetX;
        const int y = rcIcon.top + offsetY;
        ImageList_Draw(hImageList, item.iImage, hdc, x, y, ILD_NORMAL);
    }


    // 重画选中或者热点项目, 会把前一个热点/按下项目取消, 然后设置新的热点/按下项目
    inline void InvalidateItem(HWND hWnd, int index, bool isHot)
    {
        RECT rc = { 0 };
        int* pIndex = isHot ? &m_indexHot : &m_indexDown;

        if (*pIndex != -1)
        {
            ListView_GetItemRect(hWnd, *pIndex, &rc, LVIR_BOUNDS);
            ::InvalidateRect(hWnd, &rc, 0);
        }
        *pIndex = index;
        if (index != -1)
        {
            ListView_GetItemRect(hWnd, index, &rc, LVIR_BOUNDS);
            ::InvalidateRect(hWnd, &rc, 0);
        }
    }



};

