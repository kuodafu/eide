#pragma once
#include "CDrawHeader.h"
#pragma comment(lib, "Comctl32.lib")
#if (_WIN32_WINNT < 0x602)
#   define __USEXP   // ֧��xp
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

    // �����, �󶨺�����໯���, Ȼ�����������ػ�����
    // hWnd = �󶨵Ĵ���, ���Ϊ0�����໯����, ֻ��¼��ɫ��Ϣ, �������ⲿ����ָ����������
    // pClr = ����ʹ�õ���ɫ�ṹ, ���Ϊ0��ӵ�ǰ���̵�map���ȡ, ��ȡʧ����ʹ��Ĭ��ɫ�滭
    // isRef = ����2�Ƿ�һֱ��Ч, ����Ϊ���ʱ���ڲ����������ɫ�ṹ, ��һֱʹ�ò���2, ����Ϊ��ʱ��ȷ������2ָ��Ĵ��
    // isHot = �Ƿ������ȵ���, ��껬���������ȵ���
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

    

    // ��Ҫ���Ӻ����ò���Ч
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

    // ��ʼ��ק, ������ʼ�ͽ�����������, �����ý���Ȼ�󲶻����
    static void OnBeginDrag(HWND hWnd, int iItem, int& iStart, int& iEnd)
    {
        SetFocus(hWnd);
        iStart = iItem;
        iEnd = iItem;
        SetCapture(hWnd);
    }
    // ��ק��, �������ʼλ�ú͵�ǰλ��������ѡ����, Ȼ�����б��ͱ���Ŀ��ѡ��
    static void OnDragIng(HWND hWnd, int iItem, int& iStart, int& iEnd)
    {
        if (iStart == -1 || iItem == -1)
            return;

        if (iEnd == iItem)
            return;  // �ϴ�ѡ��Ļ��������Ŀ, ������


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

            // ��֤ѡ����ܱ����ѡ��
            if (!isSel && iStart != i && i != iItem)
            {
                lv_item.state = 0;
                lv_item.stateMask = LVIS_SELECTED;  // �Ƴ����ѡ��״̬
            }

            SendMessageW(hWnd, LVM_SETITEMSTATE, i, (LPARAM)&lv_item);
        };

        bool isSel = false;
        int start = 0;
        int end = 0;

        if (newEnd == oldEnd)
        {
            // ��βһ��, �Ǿ�������ѡ��, �ж���ʼλ��, �Ǽ���ѡ�����Ǽ�Сѡ��
            isSel = newStart < oldStart;
            start = isSel ? newStart : oldStart;    // �µ�λ�ô��ھɵ�λ��, �Ǿ��Ǽ�С
            end = isSel ? oldStart : newStart;    // �µ�λ��С�ھɵ�λ��, �Ǿ�������

        }
        else if (newStart == oldStart)
        {
            // ��ʼλ����ͬ, �Ǿ������ѡ��
            isSel = newEnd > oldEnd;
            start = isSel ? oldEnd : newEnd;    // �½���λ�ô��ھɵ�λ��, �Ǿ�������ѡ��
            end = isSel ? newEnd : oldEnd;    // �½���λ��С�ھɵ�λ��, �Ǿ�����ǰѡ��

        }
        else
        {
            __debugbreak(); // ���ߵ���Ӧ�þ���bug, �ȶ��²���
        }


        for (int i = start; i <= end; i++)
            pfn_state(i, isSel);

    }
private:
    virtual LRESULT OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, WNDPROC oldProc)
    {
        if (!m_isHot)
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        // ��Ҫ�����ȵ���
        LVHITTESTINFO info;
        info.pt.x = GET_X_LPARAM(lParam);
        info.pt.y = GET_Y_LPARAM(lParam);
        int index = ListView_SubItemHitTest(hWnd, &info);
        index = info.iItem;
        if (index != -1)
        {
            if (index == m_indexHot)
            {
                // �����ƶ��Ļ����ϴε���Ŀ, ����Ҫ�ػ�
                return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
            }
            // �ƶ��Ĳ�����һ�ε���Ŀ, ��Ҫ�ػ���һ�ε���Ŀ, �����õ�ǰ��Ŀ
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
            // �ƶ��Ĳ����κ���Ŀ��, ��Ҫ�ػ�֮ǰ���ȵ���
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
        // �����Զ���滭��������
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
            bool isIcon = (style & LVS_LIST) == 0;  // ֻҪΪ0, �ͱ�ʾ�Ǵ�ͼ��
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
                if (!CDrawHeader::Rect_isOverlap(rcItem, ps.rcPaint))    // �����ػ�����ͽ�����һ��ѭ��
                    continue;
                ListView_GetItem(hWnd, &item);
                item.iItem = i;

                DrawItem(lv_draw);

            }

            // �л滭����߲Ż滭
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
                //TODO ��Ŀ�߶�=0��ʱ�����û����Ŀ, ��Ҫ��ȡ����߶�, ������ô��ȡ
                if (itemHeight == 0)
                {
                    //itemHeight = LOWORD(data->propNew.nItemHeight);
                    //if (itemHeight == 0)
                    //    itemHeight = HIWORD(data->propNew.nItemHeight);

                    // Ϊ0��ʾû�б���, û�б���Ļ�rcItem�Ķ��߾���0, ������Ҫ�Ѷ��ߵ�������ͷ�ĵױ�
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
                        break;  // �������ڳߴ�Ͳ������滭��

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


                RECT rcCol; // �о���, ���Ի�ȡ��ߺ��ұ�
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
                    continue;   // �ұ�Ϊ0, �Ǿ��Ǻ��������������, �����û��ʾ����Ļ��, ���滭

                RECT& rcText = nmCustomDraw.rcText;
                rcText = nmCustomDraw.nmcd.rc;
                rcText.left += 5;
                if (dispInfo.item.iImage != -1)
                    rcText.left += lv_draw.cxIcon;

                if (lv_draw.dwCustom & CDRF_NOTIFYSUBITEMDRAW)    // ��֪ͨ�滭����
                {
                    // ����滭ǰ
                    nmCustomDraw.nmcd.uItemState = uItemState;
                    dwCustom = CICustomDrawNotify(lv_draw, CDDS_SUBITEM | CDDS_ITEMPREPAINT, &nmCustomDraw.nmcd);
                    nmCustomDraw.nmcd.uItemState = uItemState;

                    if (dwCustom & CDRF_SKIPDEFAULT)
                        continue;   // �û����л���, ���ﲻ�������»���
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
                    // ����滭����
                    
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

        // DrawItem ��������
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


    // �ػ�ѡ�л����ȵ���Ŀ, ���ǰһ���ȵ�/������Ŀȡ��, Ȼ�������µ��ȵ�/������Ŀ
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

