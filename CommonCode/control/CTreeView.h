#pragma once
#include <control/WndBase.h>



class CTreeView : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return WC_TREEVIEWW; };

public:
    CTreeView() :CControlBase() { }
    virtual ~CTreeView() { }
    virtual LPCWSTR className() { return CLASS_TREE; };

public:
    inline BOOL InvalidateItem(HTREEITEM hItem)
    {
        if (!hItem)
            return false;
        RECT rc;
        TreeView_GetItemRect(m_hWnd, hItem, &rc, 0);
        return ::InvalidateRect(m_hWnd, &rc, 0);
    }
    inline DWORD SetTreeStyle(DWORD style) {
        return SetStyle(style, false);
    }
    inline DWORD GetTreeStyle() {
        return GetStyle(false);
    }

    inline DWORD InsertTreeStyle(DWORD style) {
        return InsertStyle(style, false);
    }
    inline DWORD RemoveTreeStyle(DWORD style) {
        return RemoveStyle(style, false);
    }
    inline BOOL IsTreeStyle(DWORD style) {
        return IsStyle(style, false);
    }
#ifndef TVM_SETEXTENDEDSTYLE
#define TVM_SETEXTENDEDSTYLE      (TV_FIRST + 44)
#endif
#ifndef TVM_GETEXTENDEDSTYLE
#define TVM_GETEXTENDEDSTYLE      (TV_FIRST + 45)
#endif
    inline DWORD SetTreeStyleEx(DWORD style) {
        LONG_PTR oldStyle = GetTreeStyleEx();
        if (Send(TVM_SETEXTENDEDSTYLE, style, style) == S_OK)
            return (DWORD)(LONG_PTR)oldStyle;
        return 0;
    }
    inline DWORD GetTreeStyleEx() {
        return (DWORD)Send(TVM_GETEXTENDEDSTYLE, 0, 0);
    }
    inline DWORD InsertTreeStyleEx(DWORD style) {
        return SetTreeStyleEx(GetTreeStyleEx() | style);
    }
    inline DWORD RemoveTreeStyleEx(DWORD style) {
        DWORD oldStyle = GetTreeStyleEx();
        DWORD newStyle = ~oldStyle;
        newStyle = oldStyle & newStyle;
        return SetTreeStyleEx(newStyle);
    }
    inline BOOL IsTreeStyleEx(DWORD style) {
        return (GetTreeStyleEx() & style) == style;
    }

public:
    inline HTREEITEM GetHItem(int flag, HTREEITEM hItem) {
        return TreeView_GetNextItem(m_hWnd, hItem, flag);
    }

private:
    
    // ö������Ŀ
    // arr = ��������Ŀ���������
    // hParent = Ҫö������Ŀ����Ŀ���
    // isRecursion = �Ƿ�ݹ�ö��, Ϊ�ٵĻ�ֻö�ٵ�һ������Ŀ
    // count = ��������Ŀ�����ı���, ����ǵ�ǰ����, ��ֵ��д������
    // maxCount = �����������, ����������ֵ, �Ǿ����·�������
    inline void enum_sub_item(HTREEITEM*& arr, HTREEITEM hParent, BOOL isRecursion, int& count, int bufCount)
    {
        HTREEITEM hItem = TreeView_GetChild(m_hWnd, hParent);
        if ( !hItem )
            return;
        if ( count >= bufCount )
            throw;  // �ߵ�����϶��ǳ�bug��, �ߵ�����֮ǰ�������Ѿ�������㹻�ĳ�Ա��
        
        // �ߵ��������������, ѭ��ö�ٵ����һ����Ա
        while ( hItem )
        {
            arr[count++] = hItem;   // �����������, ��Ҫ���뵽������
            
            if ( isRecursion )      // ����������������, �ݹ��ȥ����
                enum_sub_item(arr, hItem, isRecursion, count, bufCount);
            hItem = TreeView_GetNextSibling(m_hWnd, hItem); // ȡ��һ���ֵ���
        }
    }

    // ö������Ŀ
    // arr = ��������Ŀ���������
    // hParent = Ҫö������Ŀ����Ŀ���
    // isRecursion = �Ƿ�ݹ�ö��, Ϊ�ٵĻ�ֻö�ٵ�һ������Ŀ
    template<typename TArray>
    inline void enum_sub_item(TArray& arr, HTREEITEM hParent, BOOL isRecursion)
    {
        HTREEITEM hItem = TreeView_GetChild(m_hWnd, hParent);
        if ( !hItem )
            return;

        // �ߵ��������������, ѭ��ö�ٵ����һ����Ա
        while ( hItem )
        {
            arr.push_back(hItem);   // �����������, ��Ҫ���뵽������
            if ( isRecursion )
                enum_sub_item(arr, hItem, isRecursion);     // ����������������, �ݹ��ȥ����
            hItem = TreeView_GetNextSibling(m_hWnd, hItem); // ȡ��һ���ֵ���
        }

    }

public:
    // �ͷ�ö�ٷ��ص�����
    inline void FreeEnumData(HTREEITEM* arr)
    {
        if ( arr )
            delete[] arr;
    }
    
    // ��ָ����Ŀö������, ����ö�ٵĳ�Ա��, �������ݽ����ĸ���Ŀ, ��ʹ��ʱ��Ҫ���� FreeEnumData �ͷŷ��ص���Ŀ����
    // arr = ��������Ŀ���������, ��ʹ��ʱ��Ҫ���� FreeEnumData() �ͷ�����
    // hParentItem = Ҫö������Ŀ����Ŀ���, Ϊ0��ö��������
    // isPushFrst = �Ƿ�ѵ�һ����ĿҲ���뵽������, ָ���� hParentItem �����Ŀ
    // isRecursion = �Ƿ�ݹ�ö��, Ϊ�ٵĻ�ֻö�ٵ�һ������Ŀ
    inline int EnumItem(HTREEITEM*& arr, HTREEITEM hParentItem = 0, BOOL isPushFrst = true, BOOL isRecursion = true)
    {
        int count = TreeView_GetCount(m_hWnd);
        if ( count == 0 )
        {
            arr = 0;
            return 0;
        }
        HTREEITEM hItemChild = hParentItem ? hParentItem : TreeView_GetRoot(m_hWnd);
        
        arr = new HTREEITEM[count];
        memset(arr, 0, sizeof(HTREEITEM) * count);

        int index = 0;
        if ( !hParentItem )
        {
            arr[index++] = hItemChild;
            enum_sub_item(arr, hItemChild, isRecursion, index, count);
            hItemChild = TreeView_GetNextSibling(m_hWnd, hItemChild); // ȡ��һ���ֵ���
            while ( hItemChild )
            {
                arr[index++] = hItemChild;
                enum_sub_item(arr, hItemChild, isRecursion, index, count);
                hItemChild = TreeView_GetNextSibling(m_hWnd, hItemChild); // ȡ��һ���ֵ���
            }
            return index;
        }
        
        if ( isPushFrst )
            arr[index++] = hItemChild;
        enum_sub_item(arr, hItemChild, isRecursion, index, count);

        return index;
    }
    
    template<typename TArray>
    inline int EnumItem(TArray& arr, HTREEITEM hParentItem = 0, BOOL isPushFrst = true, BOOL isRecursion = true)
    {
        arr.clear();
        int count = TreeView_GetCount(m_hWnd);
        if ( count == 0 )
            return 0;
        
        HTREEITEM hItemChild = hParentItem ? hParentItem : TreeView_GetRoot(m_hWnd);
        arr.reserve(count);

        if ( !hParentItem )
        {
            arr.push_back(hItemChild);
            enum_sub_item(arr, hItemChild, isRecursion);
            hItemChild = TreeView_GetNextSibling(m_hWnd, hItemChild); // ȡ��һ���ֵ���
            while ( hItemChild )
            {
                arr.push_back(hItemChild);
                enum_sub_item(arr, hItemChild, isRecursion);
                hItemChild = TreeView_GetNextSibling(m_hWnd, hItemChild); // ȡ��һ���ֵ���
            }
            return (int)(LONG_PTR)arr.size();
        }

        if ( isPushFrst )
            arr.push_back(hItemChild);
        enum_sub_item(arr, hItemChild, isRecursion);
        return (int)(LONG_PTR)arr.size();
    }

    // ȡ��Ŀ��
    inline int GetItemCount() {
        return TreeView_GetCount(m_hWnd);
    }
    // ȡ�������, ����ȡ����Ŀ�ж�
    inline int GetItemLevel(HTREEITEM hItem) {
        HTREEITEM hParent = TreeView_GetParent(m_hWnd, hItem);
        int level = 1;
        while (hParent)
        {
            level++;
            hParent = TreeView_GetParent(m_hWnd, hParent);
        }
        return level;
    }
    inline BOOL GetItem(TVITEMEX& item)
    {
        return TreeView_GetItem(m_hWnd, &item);
    }
    inline BOOL SetItem(const TVITEMEX& item)
    {
        return TreeView_SetItem(m_hWnd, &item);
    }


    // ȡ��Ŀ�ı�
    inline wstr GetItemText(HTREEITEM hItem)
    {
        int len = 0x800;
        wstr ret;
        int strLen = 0;
        do
        {
            len = len * 2 + 100;
            ret.resize(len);
            TVITEMW item = { 0 };
            item.mask = TVIF_TEXT;
            item.hItem = hItem;
            item.pszText = ret.data();
            item.cchTextMax = len;
            strLen = (int)(INT_PTR)Send(TVM_GETITEMW, 0, (LPARAM)&item);
        } while ( strLen == len - 1 );

        ret.resize_strlen();
        return ret;
    }

    // ����Ŀ�ı�
    inline BOOL SetItemText(HTREEITEM hItem, LPCWSTR text)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_TEXT;
        item.hItem = hItem;
        item.pszText = (LPWSTR)text;
        item.cchTextMax = lstrlenW(text);
        return TreeView_SetItem(m_hWnd, &item) != 0;
    }

    // ȡͼƬ
    inline int GetItemImage(HTREEITEM hItem)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_IMAGE;
        item.hItem = hItem;
        if ( TreeView_GetItem(m_hWnd, &item) == 0 )
            return -1;
        return item.iImage;
    }
    // ��ͼƬ
    inline int SetItemImage(HTREEITEM hItem, int iImage)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_IMAGE;
        item.hItem = hItem;
        item.iImage = iImage;
        return TreeView_SetItem(m_hWnd, &item);
    }
    // ȡ״̬ͼƬ
    inline int GetItemImageState(HTREEITEM hItem)
    {
        return TreeView_GetCheckState(m_hWnd, hItem);
    }
    // ��״̬ͼƬ
    inline int SetItemImageState(HTREEITEM hItem, int iImage)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        item.stateMask = TVIS_STATEIMAGEMASK;
        item.hItem = hItem;
        item.state = ( (iImage + 1) << 12 );
        return TreeView_SetItem(m_hWnd, &item);
    }
    // ȡѡ��ͼƬ
    inline int GetItemImageSel(HTREEITEM hItem)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_SELECTEDIMAGE;
        item.hItem = hItem;
        if ( TreeView_GetItem(m_hWnd, &item) == 0 )
            return -1;
        return item.iSelectedImage;
    }
    // ��ѡ��ͼƬ
    inline int SetItemImageSel(HTREEITEM hItem, int iSelectedImage)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_SELECTEDIMAGE;
        item.hItem = hItem;
        item.iSelectedImage = iSelectedImage;
        return TreeView_SetItem(m_hWnd, &item);
    }
    // ȡ��Ŀ��ֵ
    inline LPARAM GetItemParam(HTREEITEM hItem)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_PARAM;
        item.hItem = hItem;
        if ( TreeView_GetItem(m_hWnd, &item) == 0 )
            return 0;
        return item.lParam;
    }
    // ����Ŀ��ֵ
    inline BOOL SetItemParam(HTREEITEM hItem, LPARAM lParam)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_PARAM;
        item.hItem = hItem;
        item.lParam = lParam;
        return TreeView_SetItem(m_hWnd, &item);
    }

    int GetImg(HTREEITEM hItem) {
        TVITEMW item = { 0 };
        item.mask = TVIF_IMAGE;
        item.hItem = hItem;
        if (TreeView_GetItem(m_hWnd, &item) == 0)return 0;
        return item.iImage;
    }
    int SetImg(HTREEITEM hItem, int imgIndex) {
        TVITEMW item = { 0 };
        item.mask = TVIF_IMAGE;
        item.hItem = hItem;
        item.iImage = imgIndex;
        return TreeView_SetItem(m_hWnd, &item) != 0;
    }
    int GetImgState(HTREEITEM hItem) {
        TVITEMW item = { 0 };
        item.mask = TVIF_IMAGE;
        item.stateMask = TVIS_STATEIMAGEMASK;
        item.hItem = hItem;
        if (TreeView_GetItem(m_hWnd, &item) == 0)return 0;
        return item.state;
    }
    int SetImgState(HTREEITEM hItem, int stateIndex) {
        TVITEMW item = { 0 };
        item.mask = TVIF_IMAGE;
        item.stateMask = TVIS_STATEIMAGEMASK;
        item.hItem = hItem;
        item.state = stateIndex;
        return TreeView_SetItem(m_hWnd, &item) != 0;
    }
    int GetImgSel(HTREEITEM hItem) {
        TVITEMW item = { 0 };
        item.mask = TVIF_SELECTEDIMAGE;
        item.hItem = hItem;
        if (TreeView_GetItem(m_hWnd, &item) == 0)return 0;
        return item.iSelectedImage;
    }
    int SetImgSel(HTREEITEM hItem, int imgIndex) {
        TVITEMW item = { 0 };
        item.mask = TVIF_SELECTEDIMAGE;
        item.hItem = hItem;
        item.state = imgIndex;
        return TreeView_SetItem(m_hWnd, &item) != 0;
    }
    inline LPARAM GetItemNum(HTREEITEM hItem) { return GetItemParam(hItem); }
    inline BOOL SetItemNum(HTREEITEM hItem, LPARAM num) { return SetItemParam(hItem, num); }
    
    // ��֤��ʾ
    inline BOOL EnsureVisible(HTREEITEM hItem) {
        return TreeView_EnsureVisible(m_hWnd, hItem);
    }
    // ����Ŀչ��״̬, isExpand = �Ƿ���չ
    inline BOOL SetExpand(HTREEITEM hItem, BOOL isExpand) {
        return TreeView_Expand(m_hWnd, hItem, isExpand ? TVE_EXPAND : TVE_COLLAPSE);
    }
    // ��չ��Ŀ, b = �Ƿ���չ
    inline BOOL Expand(HTREEITEM hItem, BOOL isExpand) { return SetExpand(hItem, isExpand); }

    // չ��/������Ŀ, �Ѿ�չ��������, ����״̬��չ��
    inline BOOL Expand(HTREEITEM hItem) {
        BOOL isExpand = IsExpand(hItem);
        return Expand(hItem, !isExpand);
    }

    // �Ƿ���չ��
    inline BOOL IsExpand(HTREEITEM hItem) {
        UINT state = TreeView_GetItemState(m_hWnd, hItem, 0);
        return (state & TVIS_EXPANDED) == TVIS_EXPANDED;
    }
    inline HTREEITEM InsertItem(const TVINSERTSTRUCTW* is)
    {
        return TreeView_InsertItem(m_hWnd, is);
    }
    // ������Ŀ
    inline HTREEITEM InsertItem(HTREEITEM hParent = TVGN_ROOT, LPCWSTR text = 0, int imgIndex = -1, int selIndex = -1, LONG_PTR num = 0, HTREEITEM InsertMode = 0)
    {
        if (!InsertMode)InsertMode = TVI_LAST;
        HTREEITEM hInsertAfter = InsertMode;
        TVINSERTSTRUCTW is = { 0 };
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
        return (HTREEITEM)Send(TVM_INSERTITEMW, 0, (LPARAM)&is);
    }
    // ȡ��Ŀ����, isTitle = �Ƿ�ֻ��������
    inline RECT GetItemRect(HTREEITEM hItem, BOOL isTitle = false) {
        RECT rc = { 0 };
        TreeView_GetItemRect(m_hWnd, hItem, &rc, isTitle);
        return rc;
    }
    inline BOOL DeleteItem(HTREEITEM hItem) {
        return TreeView_DeleteItem(m_hWnd, hItem);
    }
    // ���
    inline BOOL clear()
    {
        return TreeView_DeleteItem(m_hWnd, TVI_ROOT);
    }
    // �Ƿ�������
    inline BOOL IsSubItem(HTREEITEM hItem) { return IsChild(hItem); }
    // �Ƿ�������
    inline BOOL IsChild(HTREEITEM hItem) {
        TVITEMW item = { 0 };
        item.mask = TVIF_CHILDREN;
        item.hItem = hItem;
        TreeView_GetItem(m_hWnd, &item);
        return item.cChildren != 0;
    }
    // �Ƿ�����չ
    BOOL IsExpanded(HTREEITEM hItem) {
        UINT state = TreeView_GetItemState(m_hWnd, hItem, 0);
        return (state & TVIS_EXPANDED) == TVIS_EXPANDED;
    }

    // ���üӴ�, isBold=�Ƿ�Ӵ�
    inline BOOL SetBold(HTREEITEM hItem, BOOL isBold)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        item.hItem = hItem;
        item.state = isBold ? TVIS_BOLD : (~TVIS_BOLD);
        item.stateMask = TVIS_BOLD;
        return TreeView_SetItem(m_hWnd, &item);
    }
    // ������Ŀ�Ƿ�Ӵ�
    inline BOOL IsBold(HTREEITEM hItem)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        item.hItem = hItem;
        item.stateMask = TVIS_BOLD;
        TreeView_GetItem(m_hWnd, &item);
        return (item.state & TVIS_BOLD) == TVIS_BOLD;
    }

    inline HWND EditLabel(HTREEITEM hItem)
    {
        return TreeView_EditLabel(m_hWnd, hItem);
    }
    // �Ƿ�ѡ��
    inline BOOL isSel(HTREEITEM hItem) {
        UINT state = TreeView_GetItemState(m_hWnd, hItem, 0);
        return ((state & TVIS_STATEIMAGEMASK) >> 12) == 2;
    }

    // ��ѡ��״̬, �ü���״̬
    inline BOOL SetCheckState(HTREEITEM hItem, int state) {
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        item.hItem = hItem;
        item.state = ((state + 1) << 12);
        item.stateMask = TVIS_STATEIMAGEMASK;
        return TreeView_SetItem(m_hWnd, &item);
    }

    // ȡѡ��״̬, ȡ����״̬
    inline int GetCheckState(HTREEITEM hItem) {
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        item.hItem = hItem;
        item.stateMask = TVIS_STATEIMAGEMASK;
        TreeView_GetItem(m_hWnd, &item);
        return ((item.state & TVIS_STATEIMAGEMASK) >> 12) - 1;
    }

    // ������ѡ����
    inline BOOL SetIndex(HTREEITEM hItem)       { return SetSelectItem(hItem); }
    // ������ѡ����
    inline BOOL SetSel(HTREEITEM hItem)         { return SetSelectItem(hItem); }
    // ������ѡ����
    inline BOOL SetSelectItem(HTREEITEM hItem)  { return TreeView_SelectItem(m_hWnd, hItem); }
    // ������ѡ����
    inline BOOL SetSelItem(HTREEITEM hItem)     { return SetSelectItem(hItem); }

    // ȡ������Ŀ��
    inline unsigned int GetVisibleCount()       { return TreeView_GetVisibleCount(m_hWnd); }
    
    // ȡ��һ����Ŀ
    inline HTREEITEM GetFirstItem() { return GetRoot(); }

    // ȡ����Ŀ
    inline HTREEITEM GetRootItem() { return GetRoot(); }
    
    // ȡ����Ŀ
    inline HTREEITEM GetChild(HTREEITEM hItem){
        return TreeView_GetChild(m_hWnd, hItem);
    }

#ifdef GetNextSibling
#undef GetNextSibling
#endif
    // ȡ��һ���ֵ���
    inline HTREEITEM GetNextSibling(HTREEITEM hItem) {
        return TreeView_GetNextSibling(m_hWnd, hItem);
    }

    
#ifdef GetPrevSibling
#undef GetPrevSibling
#endif
    // ȡ��һ���ֵ���
    inline HTREEITEM GetPrevSibling(HTREEITEM hItem) {
        return TreeView_GetPrevSibling(m_hWnd, hItem);
    }

    // ȡ����Ŀ
    inline HTREEITEM GetParent(HTREEITEM hItem) {
        return TreeView_GetParent(m_hWnd, hItem);
    }

    // ȡ��һ��������Ŀ
    inline HTREEITEM GetFirstVisible(){
        return TreeView_GetFirstVisible(m_hWnd);
    }

    // ȡ��һ������
    inline HTREEITEM GetNextVisible(HTREEITEM hItem) {
        return TreeView_GetNextVisible(m_hWnd, hItem);
    }

    // ȡ��һ������
    inline HTREEITEM GetPrevVisible(HTREEITEM hItem) {
        return TreeView_GetPrevVisible(m_hWnd, hItem);
    }

    // ȡ����ѡ����
    inline HTREEITEM GetSelection()     { return TreeView_GetSelection(m_hWnd); }
    // ȡ����ѡ����
    inline HTREEITEM GetIndex()         { return GetSelectItem(); }
    // ȡ����ѡ����
    inline HTREEITEM GetSel()           { return GetSelectItem(); }
    // ȡ����ѡ����
    inline HTREEITEM GetSelectItem()    { return TreeView_GetSelection(m_hWnd); }
    // ȡ����ѡ����
    inline HTREEITEM GetSelItem()       { return GetSelectItem(); }

    // ȡ����Ŀ
    inline HTREEITEM GetRoot() {
        return TreeView_GetRoot(m_hWnd);
    }

    // ȡ���һ��������Ŀ
    inline HTREEITEM GetLastVisible() {
        return TreeView_GetLastVisible(m_hWnd);
    }

    // ȡ��һ����Ŀ
    inline HTREEITEM GetNextItem(HTREEITEM hItem = 0, int code = TVGN_NEXT) {
        return TreeView_GetNextItem(m_hWnd, hItem, code);
    }

#ifndef TVGN_NEXTSELECTED
#define TVGN_NEXTSELECTED      0x000B
#endif

    // ȡ��һ��ѡ����
    inline HTREEITEM GetNextSelectedItem(HTREEITEM hItem) {
        return TreeView_GetNextItem(m_hWnd, hItem, TVGN_NEXTSELECTED);
    }



    // ȡ����ѡ����, ��ʹ��ʱ��Ҫ���� FreeEnumData() �ͷ�
    inline int GetSelAll(HTREEITEM** hItems)
    {
        //if (hItems)
        HTREEITEM hItemChild = TreeView_GetNextItem(m_hWnd, TVGN_ROOT, 0);
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        int i = 0;
        int count = TreeView_GetCount(m_hWnd);
        HTREEITEM* arr = new HTREEITEM[count];
        memset(arr, 0, sizeof(HTREEITEM) * count);

        HTREEITEM num = 0, hItemParent = 0;
        BOOL doen = false;
        UINT state = 0;
        while ( hItemChild )
        {
            item.hItem = hItemChild;
            state = (UINT)(LONG_PTR)Send(TVM_GETITEM, 0, (LPARAM)&item);
            if ( state == 0 )return 0;
            if ( ( ( state & TVIS_STATEIMAGEMASK ) >> 12 ) == 2 )
            {
                if ( i >= count - 1 )throw;    // �±�Խ��,�׳��쳣
                arr[i] = hItemChild;
                i++;
            }
            num = hItemChild;
            hItemChild = TreeView_GetNextItem(m_hWnd, hItemChild, TVGN_CHILD);
            //hItemChild = Send(TVM_GETNEXTITEM, TVGN_CHILD, hItemChild);    // ȡ��һ������Ŀ
            if ( !hItemChild )    // �Ҳ�������Ŀ, �����ֵ���Ŀ
            {
                hItemChild = TreeView_GetNextItem(m_hWnd, num, TVGN_NEXT);
                //hItemChild = Send(TVM_GETNEXTITEM, TVGN_NEXT, num);    // ȡ��һ���ֵ���Ŀ
                while ( 0 == hItemChild )    // �Ҳ����ֵ���Ŀ, �ͷ��ص���һ��
                {
                    hItemParent = TreeView_GetNextItem(m_hWnd, num, TVGN_PARENT);
                    //hItemParent = Send(TVM_GETNEXTITEM, TVGN_PARENT, num);    // ȡ����Ŀ
                    if ( hItemParent == 0 )
                    {
                        doen = true;
                        break;
                    }
                    num = hItemParent;
                    hItemChild = TreeView_GetNextItem(m_hWnd, num, TVGN_NEXT);
                    //hItemChild = Send(TVM_GETNEXTITEM, TVGN_NEXT, num);
                }
            }
            if ( doen )break;
        }
        if ( hItems )
        {
            HTREEITEM* tmp = new HTREEITEM[i];
            memcpy(tmp, arr, sizeof(HTREEITEM) * i);
            ( *hItems ) = tmp;
        }
        delete[] arr;
        return i;
    }
    // ��������
    inline BOOL SortChildren(HTREEITEM hItem) {
        return TreeView_SortChildren(m_hWnd, hItem, 0);
    }
    // ��������ص�
    inline BOOL SortChildrenCallback(HTREEITEM hItem, PFNTVCOMPARE fun, LPARAM lParam = 0) {
        TVSORTCB tsb = { 0 };
        tsb.hParent = hItem;
        tsb.lpfnCompare = fun;
        tsb.lParam = lParam;
        return TreeView_SortChildrenCB(m_hWnd, &tsb, 0);
    }
    // �����־
    inline BOOL SetInsertMark(HTREEITEM hItem, int pos = 1) {
        return TreeView_SetInsertMark(m_hWnd, hItem, pos);
    }
    // ��־����ɫ
    inline COLORREF SetInsertMarkColor(COLORREF rgb) {
        return TreeView_SetInsertMarkColor(m_hWnd, rgb);
    }
    // ��־ȡ��ɫ
    inline COLORREF GetInsertMarkColor() {
        return TreeView_GetInsertMarkColor(m_hWnd);
    }
    // ��ͼƬ��
    inline HIMAGELIST SetImageList(HIMAGELIST hImageList, int type = TVSIL_NORMAL) {
        return TreeView_SetImageList(m_hWnd, hImageList, type);
    }
    // ȡͼƬ��
    inline HIMAGELIST GetImageList(int type = TVSIL_NORMAL) {
        return TreeView_GetImageList(m_hWnd, type);
    }
    // �����Ϸ�ͼƬ��
    inline HIMAGELIST CreateDragImage(HTREEITEM hItem) {
        return TreeView_CreateDragImage(m_hWnd, hItem);
    }
    // ȡ������Ŀ
    inline HTREEITEM HitTest(const POINT& pt, int* info = 0) {
        TVHITTESTINFO hi = { 0 };
        hi.pt = pt;
        HTREEITEM hItem = TreeView_HitTest(m_hWnd, &hi);
        if (info)*info = hi.flags;
        return hItem;
    }
    // ȡ������Ŀ
    inline HTREEITEM GetPointItem(const POINT& pt, int* info = 0) {
        return HitTest(pt, info);
    }
    // ȡ����Ŀ��
    inline int GetSubCount(HTREEITEM hItem) {
        HTREEITEM hChild = TreeView_GetChild(m_hWnd, hItem);
        int count = 0;
        while (hChild)
        {
            count++;
            hChild = TreeView_GetNextItem(m_hWnd, hChild, TVGN_NEXT);
        }
        return count;
    }
    // ȡ�����༭�ı�
    inline wstr GetEditText() {
        HWND hEdit = TreeView_GetEditControl(m_hWnd);
        int len = GetWindowTextLengthW(hEdit) + 1;
        wstr str(len);
        LPWSTR text = str.data();
        ::GetWindowTextW(hEdit, text, len);
        str.resize(wcslen(str.c_str()));
        return str;
    }
    // ȡ�༭����
    inline HWND GetEditControl() {
        return TreeView_GetEditControl(m_hWnd);
    }
    

    // ��ȡ��Ŀ�߶�
    inline int GetItemHeight() {
        return TreeView_GetItemHeight(m_hWnd);
    }
    // ������Ŀ�߶�
    inline int SetItemHeight(int iHeight) {
        return TreeView_SetItemHeight(m_hWnd, iHeight);
    }
    // ��ȡ��������
    int GetIndent() {
        return TreeView_GetIndent(m_hWnd);
    }
    // ������������
    int SetIndent(int nIndent) {
        return TreeView_SetIndent(m_hWnd, nIndent);
    }
    // ��ȡ�ı�ɫ
    inline COLORREF GetTextColor() {
        return TreeView_GetTextColor(m_hWnd);
    }
    // �����ı�ɫ
    inline COLORREF SetTextColor(COLORREF clr) {
        return TreeView_SetTextColor(m_hWnd, clr);
    }
    // ����ɫ,����С��0�򷵻ر�����ɫ,����Ϊ����
    inline COLORREF GetBkColor() {
        return TreeView_GetBkColor(m_hWnd);
    }
    // ����ɫ,����С��0�򷵻ر�����ɫ,����Ϊ����
    inline COLORREF SetBkColor(COLORREF clr) {
        return TreeView_SetBkColor(m_hWnd, clr);
    }
    // ����ɫ,����С��0�򷵻�������ɫ,����Ϊ����
    inline COLORREF GetLineColor() {
        return TreeView_GetLineColor(m_hWnd);
    }
    // ����ɫ,����С��0�򷵻�������ɫ,����Ϊ����
    inline COLORREF SetLineColor(COLORREF clr) {
        return TreeView_SetLineColor(m_hWnd, clr);
    }

};

