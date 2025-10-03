#include "Ctree.h"

CTree::CTree()
{
}


CTree::~CTree()
{

}

void CTree::_create_wnd(HWND hWnd)
{
}

LRESULT CTree::WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

LRESULT CTree::OnMouseEvent(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* pUserData)
{
    //LPPROPDRAWINFO pMem = (LPPROPDRAWINFO)pUserData;
    //switch (message)
    //{
    //case WM_MOUSEMOVE:
    //{
    //    TRACKMOUSEEVENT tme = { 0 };
    //    tme.cbSize = sizeof(tme);
    //    tme.dwFlags = TME_LEAVE | TME_HOVER;
    //    tme.hwndTrack = hWnd;
    //    tme.dwHoverTime = HOVER_DEFAULT;
    //    TrackMouseEvent(&tme);
    //}
    //    break;
    //case WM_LBUTTONDOWN:
    //    pMem->pt = (int)lParam;
    //    break;
    //case WM_MOUSELEAVE:
    //case WM_NCMOUSELEAVE:
    //    pMem->indexHot = 0;
    //    break;
    //case WM_MOUSEHOVER:
    //    break;
    //default:
    //    break;
    //}
    return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
}

LRESULT CTree::OnPaint(HWND hWnd, void* pUserData)
{
    return 0;
}


LONG_PTR CTree::SetTreeStyle(LONG_PTR style)
{
    return SetStyle(style, false);
}
LONG_PTR CTree::GetTreeStyle()
{
    return GetStyle(false);
}

LONG_PTR CTree::InsertTreeStyle(LONG_PTR style)
{
    return InsertStyle(style, false);
}

LONG_PTR CTree::RemoveTreeStyle(LONG_PTR style)
{
    return RemoveStyle(style, false);
}

BOOL CTree::IsTreeStyle(LONG_PTR style)
{
    return IsStyle(style, false);
}

LONG_PTR CTree::SetTreeStyleEx(LONG_PTR style)
{
    LONG_PTR oldStyle = GetTreeStyleEx();
    if (SendMessageW(m_hWnd, TVM_SETEXTENDEDSTYLE, 0, style) == S_OK)
        return oldStyle;
    return 0;
}
LONG_PTR CTree::GetTreeStyleEx()
{
    return SendMessageW(m_hWnd, TVM_GETEXTENDEDSTYLE, 0, 0);
}

LONG_PTR CTree::InsertTreeStyleEx(LONG_PTR style)
{
    return SetTreeStyleEx(GetTreeStyleEx() | style);
}

LONG_PTR CTree::RemoveTreeStyleEx(LONG_PTR style)
{
    LONG_PTR oldStyle = GetTreeStyleEx();
    LONG_PTR newStyle = ~oldStyle;
    newStyle = oldStyle & newStyle;
    return SetTreeStyleEx(newStyle);
}

BOOL CTree::IsTreeStyleEx(LONG_PTR style)
{
    return (GetTreeStyleEx()&style) == style;
}

HTREEITEM CTree::GetHItem(int flag, HTREEITEM hItem)
{
    return TreeView_GetNextItem(m_hWnd, hItem, flag);
}

int CTree::enumHItem(HTREEITEM*& arr)
{
    int count = TreeView_GetCount(m_hWnd);
    arr = new HTREEITEM[count]{0};
    
    HTREEITEM hItemChild = TreeView_GetRoot(m_hWnd);  // 取第一个根项目
    int i = 0;
    HTREEITEM num = 0, hItemParent = 0;
    BOOL doen = false;
    while (hItemChild)
    {
        if (i >= count)throw;    // 下标越界,抛出异常

        arr[i] = hItemChild;
        i++;
        num = hItemChild;
        hItemChild = TreeView_GetNextItem(m_hWnd, hItemChild, TVGN_CHILD);
        //hItemChild = Send(TVM_GETNEXTITEM, TVGN_CHILD, hItemChild);    // 取第一个子项目
        if (!hItemChild)    // 找不到子项目，就找兄弟项目
        {
            hItemChild = TreeView_GetNextItem(m_hWnd, num, TVGN_NEXT);
            //hItemChild = Send(TVM_GETNEXTITEM, TVGN_NEXT, num);    // 取下一个兄弟项目
            while (0 == hItemChild)    // 找不到兄弟项目，就返回到上一级
            {
                hItemParent = TreeView_GetNextItem(m_hWnd, num, TVGN_PARENT);
                //hItemParent = Send(TVM_GETNEXTITEM, TVGN_PARENT, num);    // 取父项目
                if (hItemParent == 0)
                {
                    doen = true;
                    break;
                }
                num = hItemParent;
                hItemChild = TreeView_GetNextItem(m_hWnd, num, TVGN_NEXT);
                //hItemChild = Send(TVM_GETNEXTITEM, TVGN_NEXT, num);
            }
        }
        if (doen)break;
    }
    return i;
}

int CTree::GetItemCount()
{
    return TreeView_GetCount(m_hWnd);
}

int CTree::GetItemLevel(HTREEITEM hItem)
{
    HTREEITEM hParent = TreeView_GetParent(m_hWnd, hItem);
    int level = 0;
    while (hParent)
    {
        level++;
        hParent = TreeView_GetParent(m_hWnd, hParent);
    }
    return level;
}

BOOL CTree::GetItem(TVITEMEX& item)
{
    return TreeView_GetItem(m_hWnd, &item);
}

wstr CTree::GetItemText(HTREEITEM hItem)
{
    wstr str(MAX_PATH);
    WCHAR* pszText = str.data();;
    TVITEM item = { 0 };
    item.mask = TVIF_TEXT;
    item.hItem = hItem;
    item.pszText = pszText;
    item.cchTextMax = MAX_PATH;
    if (TreeView_GetItem(m_hWnd, &item) == 0)
        return wstr();
    return str;
}
BOOL CTree::SetItemText(HTREEITEM hItem, LPTSTR text)
{
    TVITEM item = { 0 };
    item.mask = TVIF_TEXT;
    item.hItem = hItem;
    item.pszText = text;
    item.cchTextMax = lstrlenW(text);
    return TreeView_SetItem(m_hWnd, &item) != 0;

}
int CTree::GetImg(HTREEITEM hItem)
{
    TVITEM item = { 0 };
    item.mask = TVIF_IMAGE;
    item.hItem = hItem;
    if (TreeView_GetItem(m_hWnd, &item) == 0)return 0;
    return item.iImage;
}

int CTree::SetImg(HTREEITEM hItem, int imgIndex)
{
    TVITEM item = { 0 };
    item.mask = TVIF_IMAGE;
    item.hItem = hItem;
    item.iImage = imgIndex;
    return TreeView_SetItem(m_hWnd, &item) != 0;
}

int CTree::GetImgState(HTREEITEM hItem)
{
    TVITEM item = { 0 };
    item.mask = TVIF_IMAGE;
    item.stateMask = TVIS_STATEIMAGEMASK;
    item.hItem = hItem;
    if (TreeView_GetItem(m_hWnd, &item) == 0)return 0;
    return item.state;
}

int CTree::SetImgState(HTREEITEM hItem, int stateIndex)
{
    TVITEM item = { 0 };
    item.mask = TVIF_IMAGE;
    item.stateMask = TVIS_STATEIMAGEMASK;
    item.hItem = hItem;
    item.state = stateIndex;
    return TreeView_SetItem(m_hWnd, &item) != 0;
}

int CTree::GetImgSel(HTREEITEM hItem)
{
    TVITEM item = { 0 };
    item.mask = TVIF_SELECTEDIMAGE;
    item.hItem = hItem;
    if (TreeView_GetItem(m_hWnd, &item) == 0)return 0;
    return item.iSelectedImage;
}

int CTree::SetImgSel(HTREEITEM hItem, int imgIndex)
{
    TVITEM item = { 0 };
    item.mask = TVIF_SELECTEDIMAGE;
    item.hItem = hItem;
    item.state = imgIndex;
    return TreeView_SetItem(m_hWnd, &item) != 0;
}

LONG_PTR CTree::GetItemNum(HTREEITEM hItem)
{
    TVITEM item = { 0 };
    item.mask = TVIF_PARAM;
    item.hItem = hItem;
    if (TreeView_GetItem(m_hWnd, &item) == 0)return 0;
    return (LONG_PTR)item.lParam;
}

int CTree::SetItemNum(HTREEITEM hItem, LONG_PTR num)
{
    TVITEM item = { 0 };
    item.mask = TVIF_PARAM;
    item.hItem = hItem;
    item.lParam = num;
    return TreeView_SetItem(m_hWnd, &item) != 0;
}

HTREEITEM CTree::GetRootItem()
{
    return TreeView_GetRoot(m_hWnd);
}

BOOL CTree::EnsureVisible(HTREEITEM hItem)
{
    return TreeView_EnsureVisible(m_hWnd, hItem);
}

BOOL CTree::Expand(HTREEITEM hItem, BOOL b)
{
    return TreeView_Expand(m_hWnd, hItem, b ? TVE_EXPAND : TVE_COLLAPSE);
}

BOOL CTree::Expand(HTREEITEM hItem)
{
    UINT state = TreeView_GetItemState(m_hWnd, hItem, 0);
    BOOL b = (state & TVIS_EXPANDED) == TVIS_EXPANDED;
    return Expand(hItem, !b);
}

HTREEITEM CTree::InsertItem(HTREEITEM hParent, LPCWSTR text, int imgIndex, int selIndex, int num, HTREEITEM InsertMode)
{
    if (!InsertMode)InsertMode = TVI_LAST;
    HTREEITEM hInsertAfter = InsertMode;
    TVINSERTSTRUCT is = { 0 };
    is.item.mask = TVIF_PARAM;
    if (imgIndex >= 0)
    {
        is.item.mask = is.item.mask | TVIF_IMAGE;
        is.item.iImage = imgIndex;
        is.item.iSelectedImage = selIndex;
    }
    else
    {
        is.item.iImage = -1;
        is.item.iSelectedImage = -1;
    }
    if (selIndex >= 0)
    {
        is.item.mask = is.item.mask | TVIF_SELECTEDIMAGE;
        is.item.iSelectedImage = selIndex;
    }
    is.hParent = hParent;
    is.hInsertAfter = hInsertAfter;
    if (text)
    {
        is.item.pszText = (LPWSTR)text;
        is.item.cchTextMax = (int)wcslen(text);
        is.item.mask |= TVIF_TEXT;
    }
    is.item.lParam = num;
    return TreeView_InsertItem(m_hWnd, &is);
}

RECT CTree::GetItemRect(HTREEITEM hItem, BOOL isTitle)
{
    RECT rc = { 0 };
    TreeView_GetItemRect(m_hWnd, hItem, &rc, isTitle);
    return rc;
}

BOOL CTree::DeleteItem(HTREEITEM hItem)
{
    return TreeView_DeleteItem(m_hWnd, hItem);
}

BOOL CTree::clear()
{
    return TreeView_DeleteItem(m_hWnd, TVI_ROOT);
}

HTREEITEM CTree::GetParent(HTREEITEM hItem)
{
    return TreeView_GetParent(m_hWnd, hItem);
}

BOOL CTree::IsSubItem(HTREEITEM hItem)
{
    TVITEM item = { 0 };
    item.mask = TVIF_CHILDREN;
    item.hItem = hItem;
    TreeView_GetItem(m_hWnd, &item);
    return item.cChildren != 0;
}

BOOL CTree::IsExpanded(HTREEITEM hItem)
{
    
    UINT state = TreeView_GetItemState(m_hWnd, hItem, 0);
    return (state & TVIS_EXPANDED) == TVIS_EXPANDED;
}

BOOL CTree::bold(HTREEITEM hItem, BOOL b)
{
    TVITEM item = { 0 };
    item.mask = TVIF_STATE;
    item.hItem = hItem;
    item.state = b ? TVIS_BOLD : (~TVIS_BOLD);
    item.stateMask = TVIS_BOLD;
    return TreeView_SetItem(m_hWnd, &item);
}

BOOL CTree::bold(HTREEITEM hItem)
{
    TVITEM item = { 0 };
    item.mask = TVIF_STATE;
    item.hItem = hItem;
    item.stateMask = TVIS_BOLD;
    TreeView_GetItem(m_hWnd, &item);
    return (item.state & TVIS_BOLD) != 0;
}

HWND CTree::EditLabel(HTREEITEM hItem)
{
    return TreeView_EditLabel(m_hWnd, hItem);
}

BOOL CTree::isSel(HTREEITEM hItem)
{
    UINT state = TreeView_GetItemState(m_hWnd, hItem, 0);
    return ((state&TVIS_STATEIMAGEMASK) >> 12) == 2;
}

BOOL CTree::SetCheckBoxState(HTREEITEM hItem, int state)
{
    TVITEM item = { 0 };
    item.mask = TVIF_STATE;
    item.hItem = hItem;
    item.state = ((state + 1) << 12);
    item.stateMask = TVIS_STATEIMAGEMASK;
    return TreeView_SetItem(m_hWnd, &item);
}

int CTree::GetCheckBoxState(HTREEITEM hItem)
{
    TVITEM item = { 0 };
    item.mask = TVIF_STATE;
    item.hItem = hItem;
    item.stateMask = TVIS_STATEIMAGEMASK;
    TreeView_GetItem(m_hWnd, &item);
    return ((item.state&TVIS_STATEIMAGEMASK) >> 12) - 1;
}

int CTree::SetIndex(HTREEITEM hItem)
{
    return TreeView_SelectItem(m_hWnd, hItem);
}

HTREEITEM CTree::GetIndex()
{
    return TreeView_GetNextItem(m_hWnd, 0, TVGN_CARET);
}

int CTree::GetVisibleCount()
{
    return TreeView_GetVisibleCount(m_hWnd);
}
HTREEITEM CTree::GetFirstVisible()
{
    return TreeView_GetFirstVisible(m_hWnd);
}

HTREEITEM CTree::GetFirstItem()
{
    return TreeView_GetRoot(m_hWnd);
}

HTREEITEM CTree::GetNextVisible(HTREEITEM hItem)
{
    return TreeView_GetNextVisible(m_hWnd, hItem);
}

int CTree::GetSelAll(HTREEITEM** hItems)
{
    //if (hItems)
    HTREEITEM hItemChild = TreeView_GetNextItem(m_hWnd, TVGN_ROOT, 0);
    TVITEM item = { 0 };
    item.mask = TVIF_STATE;
    int i = 0;
    int count = TreeView_GetCount(m_hWnd);
    HTREEITEM* arr = new HTREEITEM[count]{0};
    HTREEITEM num = 0, hItemParent = 0;
    BOOL doen = false;
    UINT state = 0;
    while (hItemChild)
    {
        item.hItem = hItemChild;
        state = Send(TVM_GETITEM, 0, (LPARAM)&item);
        if (state == 0)return 0;
        if (((state&TVIS_STATEIMAGEMASK) >> 12) == 2)
        {
            if (i >= count - 1)throw;    // 下标越界,抛出异常
            arr[i] = hItemChild;
            i++;
        }
        num = hItemChild;
        hItemChild = TreeView_GetNextItem(m_hWnd, hItemChild, TVGN_CHILD);
        //hItemChild = Send(TVM_GETNEXTITEM, TVGN_CHILD, hItemChild);    // 取第一个子项目
        if (!hItemChild)    // 找不到子项目，就找兄弟项目
        {
            hItemChild = TreeView_GetNextItem(m_hWnd, num, TVGN_NEXT);
            //hItemChild = Send(TVM_GETNEXTITEM, TVGN_NEXT, num);    // 取下一个兄弟项目
            while (0 == hItemChild)    // 找不到兄弟项目，就返回到上一级
            {
                hItemParent = TreeView_GetNextItem(m_hWnd, num, TVGN_PARENT);
                //hItemParent = Send(TVM_GETNEXTITEM, TVGN_PARENT, num);    // 取父项目
                if (hItemParent == 0)
                {
                    doen = true;
                    break;
                }
                num = hItemParent;
                hItemChild = TreeView_GetNextItem(m_hWnd, num, TVGN_NEXT);
                //hItemChild = Send(TVM_GETNEXTITEM, TVGN_NEXT, num);
            }
        }
        if (doen)break;
    }
    if (hItems)
    {
        HTREEITEM* tmp = new HTREEITEM[i]{ 0 };
        memcpy(tmp, arr, sizeof(HTREEITEM) * i);
        (*hItems) = tmp;
    }
    delete[] arr;
    return i;
    
}

BOOL CTree::SortChildren(HTREEITEM hItem)
{
    return TreeView_SortChildren(m_hWnd, hItem, 0);
}

BOOL CTree::SortChildrenCallback(HTREEITEM hItem, PFNTVCOMPARE fun, LPARAM lParam/* = 0*/)
{
    TVSORTCB tsb = { 0 };
    tsb.hParent = hItem;
    tsb.lpfnCompare = fun;
    tsb.lParam = lParam;
    return TreeView_SortChildrenCB(m_hWnd, &tsb, 0);
}

BOOL CTree::SetInsertMark(HTREEITEM hItem, int pos /*= 1*/)
{
    return TreeView_SetInsertMark(m_hWnd, hItem, pos);
}

BOOL CTree::SetInsertMarkColor(COLORREF rgb)
{
    return TreeView_SetInsertMarkColor(m_hWnd, rgb);
}

COLORREF CTree::GetInsertMarkColor()
{
    return TreeView_GetInsertMarkColor(m_hWnd);
}

HIMAGELIST CTree::SetImageList(HIMAGELIST hImageList, int type /*= 0*/)
{
    return TreeView_SetImageList(m_hWnd, hImageList, type);
}

HIMAGELIST CTree::GetImageList(int type /*= 0*/)
{
    return TreeView_GetImageList(m_hWnd, type);
}

HIMAGELIST CTree::CreateDragImage(HTREEITEM hItem)
{
    return TreeView_CreateDragImage(m_hWnd, hItem);
}

HTREEITEM CTree::GetPointItem(POINT pt, int* info /*= 0*/)
{
    TVHITTESTINFO hi = { 0 };
    hi.pt = pt;
    HTREEITEM hItem = TreeView_HitTest(m_hWnd, &hi);
    if (info)*info = hi.flags;
    return hItem;
}

int CTree::GetSubCount(HTREEITEM hItem)
{
    HTREEITEM hChild = TreeView_GetChild(m_hWnd, hItem);
    int count = 0;
    while (hChild)
    {
        count++;
        hChild = TreeView_GetNextItem(m_hWnd, hChild, TVGN_NEXT);
    }
    return count;
}

wstr CTree::GetEditText()
{
    HWND hEdit = TreeView_GetEditControl(m_hWnd);
    int len = GetWindowTextLengthW(hEdit) + 1;
    wstr str(len);
    LPWSTR text = str.data();
    ::GetWindowTextW(hEdit, text, len);
    str.resize_strlen();
    return str;
}


HWND CTree::GetEditControl()
{
    return TreeView_GetEditControl(m_hWnd);
}

int CTree::ItemHeight(int height /*= 0*/)
{
    if (height <= 0)
        return TreeView_GetItemHeight(m_hWnd);
    return TreeView_SetItemHeight(m_hWnd, height);
}

int CTree::Indent(int n /*= 0*/)
{
    if (n <= 0)
        return TreeView_GetIndent(m_hWnd);
    return TreeView_SetIndent(m_hWnd, n);
}

COLORREF CTree::TextColor(COLORREF clr /*= 0*/)
{
    if (clr < 0)
        return TreeView_GetTextColor(m_hWnd);
    return TreeView_SetTextColor(m_hWnd, clr);
}

COLORREF CTree::BkColor(COLORREF clr /*= 0*/)
{
    if (clr < 0)
        return TreeView_GetBkColor(m_hWnd);
    return TreeView_SetBkColor(m_hWnd, clr);
}

COLORREF CTree::LineColor(COLORREF clr /*= 0*/)
{
    if (clr < 0)
        return TreeView_GetLineColor(m_hWnd);
    return TreeView_SetLineColor(m_hWnd, clr);
}
