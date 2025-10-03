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
    
    // 枚举子项目
    // arr = 保存子项目句柄的数组
    // hParent = 要枚举子项目的项目句柄
    // isRecursion = 是否递归枚举, 为假的话只枚举第一层子项目
    // count = 保存子项目数量的变量, 这个是当前索引, 有值就写入这里
    // maxCount = 数组最大容量, 如果超过这个值, 那就重新分配数组
    inline void enum_sub_item(HTREEITEM*& arr, HTREEITEM hParent, BOOL isRecursion, int& count, int bufCount)
    {
        HTREEITEM hItem = TreeView_GetChild(m_hWnd, hParent);
        if ( !hItem )
            return;
        if ( count >= bufCount )
            throw;  // 走到这里肯定是出bug了, 走到这里之前数组是已经分配给足够的成员的
        
        // 走到这里就是有子项, 循环枚举到最后一个成员
        while ( hItem )
        {
            arr[count++] = hItem;   // 这个就是子项, 需要加入到数组里
            
            if ( isRecursion )      // 假设这个子项还有子项, 递归进去处理
                enum_sub_item(arr, hItem, isRecursion, count, bufCount);
            hItem = TreeView_GetNextSibling(m_hWnd, hItem); // 取下一个兄弟项
        }
    }

    // 枚举子项目
    // arr = 保存子项目句柄的数组
    // hParent = 要枚举子项目的项目句柄
    // isRecursion = 是否递归枚举, 为假的话只枚举第一层子项目
    template<typename TArray>
    inline void enum_sub_item(TArray& arr, HTREEITEM hParent, BOOL isRecursion)
    {
        HTREEITEM hItem = TreeView_GetChild(m_hWnd, hParent);
        if ( !hItem )
            return;

        // 走到这里就是有子项, 循环枚举到最后一个成员
        while ( hItem )
        {
            arr.push_back(hItem);   // 这个就是子项, 需要加入到数组里
            if ( isRecursion )
                enum_sub_item(arr, hItem, isRecursion);     // 假设这个子项还有子项, 递归进去处理
            hItem = TreeView_GetNextSibling(m_hWnd, hItem); // 取下一个兄弟项
        }

    }

public:
    // 释放枚举返回的数组
    inline void FreeEnumData(HTREEITEM* arr)
    {
        if ( arr )
            delete[] arr;
    }
    
    // 从指定项目枚举子项, 返回枚举的成员数, 包含传递进来的父项目, 不使用时需要调用 FreeEnumData 释放返回的项目数组
    // arr = 保存子项目句柄的数组, 不使用时需要调用 FreeEnumData() 释放数组
    // hParentItem = 要枚举子项目的项目句柄, 为0则枚举整个树
    // isPushFrst = 是否把第一个项目也加入到数组里, 指的是 hParentItem 这个项目
    // isRecursion = 是否递归枚举, 为假的话只枚举第一层子项目
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
            hItemChild = TreeView_GetNextSibling(m_hWnd, hItemChild); // 取下一个兄弟项
            while ( hItemChild )
            {
                arr[index++] = hItemChild;
                enum_sub_item(arr, hItemChild, isRecursion, index, count);
                hItemChild = TreeView_GetNextSibling(m_hWnd, hItemChild); // 取下一个兄弟项
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
            hItemChild = TreeView_GetNextSibling(m_hWnd, hItemChild); // 取下一个兄弟项
            while ( hItemChild )
            {
                arr.push_back(hItemChild);
                enum_sub_item(arr, hItemChild, isRecursion);
                hItemChild = TreeView_GetNextSibling(m_hWnd, hItemChild); // 取下一个兄弟项
            }
            return (int)(LONG_PTR)arr.size();
        }

        if ( isPushFrst )
            arr.push_back(hItemChild);
        enum_sub_item(arr, hItemChild, isRecursion);
        return (int)(LONG_PTR)arr.size();
    }

    // 取项目数
    inline int GetItemCount() {
        return TreeView_GetCount(m_hWnd);
    }
    // 取缩进层次, 逐层获取父项目判断
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


    // 取项目文本
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

    // 置项目文本
    inline BOOL SetItemText(HTREEITEM hItem, LPCWSTR text)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_TEXT;
        item.hItem = hItem;
        item.pszText = (LPWSTR)text;
        item.cchTextMax = lstrlenW(text);
        return TreeView_SetItem(m_hWnd, &item) != 0;
    }

    // 取图片
    inline int GetItemImage(HTREEITEM hItem)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_IMAGE;
        item.hItem = hItem;
        if ( TreeView_GetItem(m_hWnd, &item) == 0 )
            return -1;
        return item.iImage;
    }
    // 置图片
    inline int SetItemImage(HTREEITEM hItem, int iImage)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_IMAGE;
        item.hItem = hItem;
        item.iImage = iImage;
        return TreeView_SetItem(m_hWnd, &item);
    }
    // 取状态图片
    inline int GetItemImageState(HTREEITEM hItem)
    {
        return TreeView_GetCheckState(m_hWnd, hItem);
    }
    // 置状态图片
    inline int SetItemImageState(HTREEITEM hItem, int iImage)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        item.stateMask = TVIS_STATEIMAGEMASK;
        item.hItem = hItem;
        item.state = ( (iImage + 1) << 12 );
        return TreeView_SetItem(m_hWnd, &item);
    }
    // 取选中图片
    inline int GetItemImageSel(HTREEITEM hItem)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_SELECTEDIMAGE;
        item.hItem = hItem;
        if ( TreeView_GetItem(m_hWnd, &item) == 0 )
            return -1;
        return item.iSelectedImage;
    }
    // 置选中图片
    inline int SetItemImageSel(HTREEITEM hItem, int iSelectedImage)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_SELECTEDIMAGE;
        item.hItem = hItem;
        item.iSelectedImage = iSelectedImage;
        return TreeView_SetItem(m_hWnd, &item);
    }
    // 取项目数值
    inline LPARAM GetItemParam(HTREEITEM hItem)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_PARAM;
        item.hItem = hItem;
        if ( TreeView_GetItem(m_hWnd, &item) == 0 )
            return 0;
        return item.lParam;
    }
    // 置项目数值
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
    
    // 保证显示
    inline BOOL EnsureVisible(HTREEITEM hItem) {
        return TreeView_EnsureVisible(m_hWnd, hItem);
    }
    // 置项目展开状态, isExpand = 是否扩展
    inline BOOL SetExpand(HTREEITEM hItem, BOOL isExpand) {
        return TreeView_Expand(m_hWnd, hItem, isExpand ? TVE_EXPAND : TVE_COLLAPSE);
    }
    // 扩展项目, b = 是否扩展
    inline BOOL Expand(HTREEITEM hItem, BOOL isExpand) { return SetExpand(hItem, isExpand); }

    // 展开/收缩项目, 已经展开就收缩, 收缩状态就展开
    inline BOOL Expand(HTREEITEM hItem) {
        BOOL isExpand = IsExpand(hItem);
        return Expand(hItem, !isExpand);
    }

    // 是否已展开
    inline BOOL IsExpand(HTREEITEM hItem) {
        UINT state = TreeView_GetItemState(m_hWnd, hItem, 0);
        return (state & TVIS_EXPANDED) == TVIS_EXPANDED;
    }
    inline HTREEITEM InsertItem(const TVINSERTSTRUCTW* is)
    {
        return TreeView_InsertItem(m_hWnd, is);
    }
    // 加入项目
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
    // 取项目矩形, isTitle = 是否只包含标题
    inline RECT GetItemRect(HTREEITEM hItem, BOOL isTitle = false) {
        RECT rc = { 0 };
        TreeView_GetItemRect(m_hWnd, hItem, &rc, isTitle);
        return rc;
    }
    inline BOOL DeleteItem(HTREEITEM hItem) {
        return TreeView_DeleteItem(m_hWnd, hItem);
    }
    // 清空
    inline BOOL clear()
    {
        return TreeView_DeleteItem(m_hWnd, TVI_ROOT);
    }
    // 是否有子项
    inline BOOL IsSubItem(HTREEITEM hItem) { return IsChild(hItem); }
    // 是否有子项
    inline BOOL IsChild(HTREEITEM hItem) {
        TVITEMW item = { 0 };
        item.mask = TVIF_CHILDREN;
        item.hItem = hItem;
        TreeView_GetItem(m_hWnd, &item);
        return item.cChildren != 0;
    }
    // 是否已扩展
    BOOL IsExpanded(HTREEITEM hItem) {
        UINT state = TreeView_GetItemState(m_hWnd, hItem, 0);
        return (state & TVIS_EXPANDED) == TVIS_EXPANDED;
    }

    // 设置加粗, isBold=是否加粗
    inline BOOL SetBold(HTREEITEM hItem, BOOL isBold)
    {
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        item.hItem = hItem;
        item.state = isBold ? TVIS_BOLD : (~TVIS_BOLD);
        item.stateMask = TVIS_BOLD;
        return TreeView_SetItem(m_hWnd, &item);
    }
    // 返回项目是否加粗
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
    // 是否选中
    inline BOOL isSel(HTREEITEM hItem) {
        UINT state = TreeView_GetItemState(m_hWnd, hItem, 0);
        return ((state & TVIS_STATEIMAGEMASK) >> 12) == 2;
    }

    // 置选中状态, 置检查框状态
    inline BOOL SetCheckState(HTREEITEM hItem, int state) {
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        item.hItem = hItem;
        item.state = ((state + 1) << 12);
        item.stateMask = TVIS_STATEIMAGEMASK;
        return TreeView_SetItem(m_hWnd, &item);
    }

    // 取选中状态, 取检查框状态
    inline int GetCheckState(HTREEITEM hItem) {
        TVITEMW item = { 0 };
        item.mask = TVIF_STATE;
        item.hItem = hItem;
        item.stateMask = TVIS_STATEIMAGEMASK;
        TreeView_GetItem(m_hWnd, &item);
        return ((item.state & TVIS_STATEIMAGEMASK) >> 12) - 1;
    }

    // 置现行选中项
    inline BOOL SetIndex(HTREEITEM hItem)       { return SetSelectItem(hItem); }
    // 置现行选中项
    inline BOOL SetSel(HTREEITEM hItem)         { return SetSelectItem(hItem); }
    // 置现行选中项
    inline BOOL SetSelectItem(HTREEITEM hItem)  { return TreeView_SelectItem(m_hWnd, hItem); }
    // 置现行选中项
    inline BOOL SetSelItem(HTREEITEM hItem)     { return SetSelectItem(hItem); }

    // 取可视项目数
    inline unsigned int GetVisibleCount()       { return TreeView_GetVisibleCount(m_hWnd); }
    
    // 取第一个项目
    inline HTREEITEM GetFirstItem() { return GetRoot(); }

    // 取根项目
    inline HTREEITEM GetRootItem() { return GetRoot(); }
    
    // 取子项目
    inline HTREEITEM GetChild(HTREEITEM hItem){
        return TreeView_GetChild(m_hWnd, hItem);
    }

#ifdef GetNextSibling
#undef GetNextSibling
#endif
    // 取下一个兄弟项
    inline HTREEITEM GetNextSibling(HTREEITEM hItem) {
        return TreeView_GetNextSibling(m_hWnd, hItem);
    }

    
#ifdef GetPrevSibling
#undef GetPrevSibling
#endif
    // 取上一个兄弟项
    inline HTREEITEM GetPrevSibling(HTREEITEM hItem) {
        return TreeView_GetPrevSibling(m_hWnd, hItem);
    }

    // 取父项目
    inline HTREEITEM GetParent(HTREEITEM hItem) {
        return TreeView_GetParent(m_hWnd, hItem);
    }

    // 取第一个可视项目
    inline HTREEITEM GetFirstVisible(){
        return TreeView_GetFirstVisible(m_hWnd);
    }

    // 取下一个可视
    inline HTREEITEM GetNextVisible(HTREEITEM hItem) {
        return TreeView_GetNextVisible(m_hWnd, hItem);
    }

    // 取上一个可视
    inline HTREEITEM GetPrevVisible(HTREEITEM hItem) {
        return TreeView_GetPrevVisible(m_hWnd, hItem);
    }

    // 取现行选中项
    inline HTREEITEM GetSelection()     { return TreeView_GetSelection(m_hWnd); }
    // 取现行选中项
    inline HTREEITEM GetIndex()         { return GetSelectItem(); }
    // 取现行选中项
    inline HTREEITEM GetSel()           { return GetSelectItem(); }
    // 取现行选中项
    inline HTREEITEM GetSelectItem()    { return TreeView_GetSelection(m_hWnd); }
    // 取现行选中项
    inline HTREEITEM GetSelItem()       { return GetSelectItem(); }

    // 取根项目
    inline HTREEITEM GetRoot() {
        return TreeView_GetRoot(m_hWnd);
    }

    // 取最后一个可视项目
    inline HTREEITEM GetLastVisible() {
        return TreeView_GetLastVisible(m_hWnd);
    }

    // 取下一个项目
    inline HTREEITEM GetNextItem(HTREEITEM hItem = 0, int code = TVGN_NEXT) {
        return TreeView_GetNextItem(m_hWnd, hItem, code);
    }

#ifndef TVGN_NEXTSELECTED
#define TVGN_NEXTSELECTED      0x000B
#endif

    // 取下一个选中项
    inline HTREEITEM GetNextSelectedItem(HTREEITEM hItem) {
        return TreeView_GetNextItem(m_hWnd, hItem, TVGN_NEXTSELECTED);
    }



    // 取所有选中项, 不使用时需要调用 FreeEnumData() 释放
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
                if ( i >= count - 1 )throw;    // 下标越界,抛出异常
                arr[i] = hItemChild;
                i++;
            }
            num = hItemChild;
            hItemChild = TreeView_GetNextItem(m_hWnd, hItemChild, TVGN_CHILD);
            //hItemChild = Send(TVM_GETNEXTITEM, TVGN_CHILD, hItemChild);    // 取第一个子项目
            if ( !hItemChild )    // 找不到子项目, 就找兄弟项目
            {
                hItemChild = TreeView_GetNextItem(m_hWnd, num, TVGN_NEXT);
                //hItemChild = Send(TVM_GETNEXTITEM, TVGN_NEXT, num);    // 取下一个兄弟项目
                while ( 0 == hItemChild )    // 找不到兄弟项目, 就返回到上一级
                {
                    hItemParent = TreeView_GetNextItem(m_hWnd, num, TVGN_PARENT);
                    //hItemParent = Send(TVM_GETNEXTITEM, TVGN_PARENT, num);    // 取父项目
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
    // 排序子项
    inline BOOL SortChildren(HTREEITEM hItem) {
        return TreeView_SortChildren(m_hWnd, hItem, 0);
    }
    // 排序子项回调
    inline BOOL SortChildrenCallback(HTREEITEM hItem, PFNTVCOMPARE fun, LPARAM lParam = 0) {
        TVSORTCB tsb = { 0 };
        tsb.hParent = hItem;
        tsb.lpfnCompare = fun;
        tsb.lParam = lParam;
        return TreeView_SortChildrenCB(m_hWnd, &tsb, 0);
    }
    // 插入标志
    inline BOOL SetInsertMark(HTREEITEM hItem, int pos = 1) {
        return TreeView_SetInsertMark(m_hWnd, hItem, pos);
    }
    // 标志置颜色
    inline COLORREF SetInsertMarkColor(COLORREF rgb) {
        return TreeView_SetInsertMarkColor(m_hWnd, rgb);
    }
    // 标志取颜色
    inline COLORREF GetInsertMarkColor() {
        return TreeView_GetInsertMarkColor(m_hWnd);
    }
    // 置图片组
    inline HIMAGELIST SetImageList(HIMAGELIST hImageList, int type = TVSIL_NORMAL) {
        return TreeView_SetImageList(m_hWnd, hImageList, type);
    }
    // 取图片组
    inline HIMAGELIST GetImageList(int type = TVSIL_NORMAL) {
        return TreeView_GetImageList(m_hWnd, type);
    }
    // 创建拖放图片组
    inline HIMAGELIST CreateDragImage(HTREEITEM hItem) {
        return TreeView_CreateDragImage(m_hWnd, hItem);
    }
    // 取坐标项目
    inline HTREEITEM HitTest(const POINT& pt, int* info = 0) {
        TVHITTESTINFO hi = { 0 };
        hi.pt = pt;
        HTREEITEM hItem = TreeView_HitTest(m_hWnd, &hi);
        if (info)*info = hi.flags;
        return hItem;
    }
    // 取坐标项目
    inline HTREEITEM GetPointItem(const POINT& pt, int* info = 0) {
        return HitTest(pt, info);
    }
    // 取子项目数
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
    // 取结束编辑文本
    inline wstr GetEditText() {
        HWND hEdit = TreeView_GetEditControl(m_hWnd);
        int len = GetWindowTextLengthW(hEdit) + 1;
        wstr str(len);
        LPWSTR text = str.data();
        ::GetWindowTextW(hEdit, text, len);
        str.resize(wcslen(str.c_str()));
        return str;
    }
    // 取编辑框句柄
    inline HWND GetEditControl() {
        return TreeView_GetEditControl(m_hWnd);
    }
    

    // 获取项目高度
    inline int GetItemHeight() {
        return TreeView_GetItemHeight(m_hWnd);
    }
    // 设置项目高度
    inline int SetItemHeight(int iHeight) {
        return TreeView_SetItemHeight(m_hWnd, iHeight);
    }
    // 获取缩进距离
    int GetIndent() {
        return TreeView_GetIndent(m_hWnd);
    }
    // 设置缩进距离
    int SetIndent(int nIndent) {
        return TreeView_SetIndent(m_hWnd, nIndent);
    }
    // 获取文本色
    inline COLORREF GetTextColor() {
        return TreeView_GetTextColor(m_hWnd);
    }
    // 设置文本色
    inline COLORREF SetTextColor(COLORREF clr) {
        return TreeView_SetTextColor(m_hWnd, clr);
    }
    // 背景色,参数小于0则返回背景颜色,否则为设置
    inline COLORREF GetBkColor() {
        return TreeView_GetBkColor(m_hWnd);
    }
    // 背景色,参数小于0则返回背景颜色,否则为设置
    inline COLORREF SetBkColor(COLORREF clr) {
        return TreeView_SetBkColor(m_hWnd, clr);
    }
    // 连线色,参数小于0则返回连线颜色,否则为设置
    inline COLORREF GetLineColor() {
        return TreeView_GetLineColor(m_hWnd);
    }
    // 连线色,参数小于0则返回连线颜色,否则为设置
    inline COLORREF SetLineColor(COLORREF clr) {
        return TreeView_SetLineColor(m_hWnd, clr);
    }

};

