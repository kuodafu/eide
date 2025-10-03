#pragma once
#include "WndBase.h"
class CListView : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return WC_LISTVIEWW; }

public:
    CListView() :CControlBase() { ; }
    virtual ~CListView() { ; }
public:
    HWND create(HWND hWndParent, int x, int y, int cx, int cy, LONG_PTR styleEx, LONG_PTR style, LONG_PTR listStyleEx, LONG_PTR ID, void* lpParam = 0)
    {
        if (!CWndBase::create((int)styleEx, 0, (int)style | WS_CHILD, x, y, cx, cy, hWndParent, ID, lpParam))return 0;
        SetListStyle(listStyleEx);
        return m_hWnd;
    }
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0)
    {
        return CWndBase::create(dwExStyle, lpszTitle, dwStyle | WS_CHILD, x, y, nWidth, nHeight, hWndParent, ID, lpParam);
    }
    virtual inline LPCWSTR className() { return CLASS_LISTVIEW; }

    // 取标题, index=表项索引, colIndex=列索引
    wstr GetTitle(int index, int iSubItem) const
    {
        const int size = 0x1000;
        wstr ret(size);
        ListView_GetItemText(m_hWnd, index, iSubItem, ret.data(), (int)size);
        ret.resize(wcslen(ret.c_str()));
        return ret;
    }
    int GetItemImage(int index, int iSubItem) const
    {
        LVITEMW lv = { 0 };
        lv.mask = LVIF_IMAGE;
        lv.iItem = index;
        lv.iSubItem = iSubItem;
        ListView_GetItem(m_hWnd, &lv);
        return lv.iImage;
    }
    // 取缩进层次
    int GetItemIndent(int index) const
    {
        LVITEMW lv = { 0 };
        lv.mask = LVIF_INDENT;
        lv.iItem = index;
        lv.iSubItem = 0;
        ListView_GetItem(m_hWnd, &lv);
        return lv.iIndent;
    }
    int GetItemImageState(int index) const
    {
        //LVITEMW lv;
        //lv.mask = LVIF_STATE;
        //lv.iItem = index;
        //ListView_GetItem(m_hWnd, &lv);
        //INDEXTOSTATEIMAGEMASK(lv.mask);
        return ListView_GetItemState(m_hWnd, index, LVIS_STATEIMAGEMASK);
        return 0;
    }


    // 置表项数, 虚表专用, 参数2表示设置表项后是否重画以及是否改变滚动条位置
    BOOL SetItemCount(DWORD count, DWORD flags = LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL) const
    {
        return ListView_SetItemCountEx(m_hWnd, count, flags) != 0;
    }
    // 取表项数
    DWORD GetItemCount() const
    {
        return (DWORD)ListView_GetItemCount(m_hWnd);
    }
    LPARAM GetItemParam(int index) const
    {
        LVITEMW lv = { 0 };
        lv.mask = LVIF_PARAM;
        lv.iItem = index;
        ListView_GetItem(m_hWnd, &lv);
        return (LPARAM)lv.lParam;
    }

    BOOL SetItemParam(int index, LPARAM lParam) const
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_PARAM;
        item.iItem = index;
        item.iSubItem = 0;
        item.lParam = lParam;
        return ListView_SetItem(m_hWnd, &item);
    }
    BOOL SetItemImage(int index, int iSubItem, int imgIndex) const
    {
        LVITEMW lv = { 0 };
        lv.iItem = index;
        lv.iSubItem = iSubItem;
        lv.mask = LVIF_IMAGE;
        lv.iImage = imgIndex;
        return ListView_SetItem(m_hWnd, &lv);
    }
    BOOL SetItem(const LVITEMW* pItem) const
    {
        return ListView_SetItem(m_hWnd, pItem);
    }

    // 置状态图片
    // iItem = 项目索引
    // iImage = 状态图片索引用于指定“状态图片组”属性中的某张图片。索引值从 0 开始，-1 表示无图片。如果本参数被省略，默认值为 -1
    BOOL SetItemImageState(int iItem, int iImage) const
    {
        LVITEMW lvi = { 0 };
        lvi.stateMask = LVIS_STATEIMAGEMASK;
        lvi.state = INDEXTOSTATEIMAGEMASK(iImage + 1);
        return Send(LVM_SETITEMSTATE, (WPARAM)( iItem ), (LPARAM)(LV_ITEM*)&lvi) != FALSE; 
        //ListView_SetItemState(m_hWnd, iItem, INDEXTOSTATEIMAGEMASK(iImage + 1), LVIS_STATEIMAGEMASK);
    }
    BOOL GetItem(LVITEMW* pItem) const
    {
        return ListView_GetItem(m_hWnd, pItem);
    }
    // 删除所有项目
    BOOL DeleteAllItems() const
    {
        return ListView_DeleteAllItems(m_hWnd);
    }
    // 删除项目
    BOOL DeleteItem(int index) const
    {
        return ListView_DeleteItem(m_hWnd, index);
    }

    // 插入列
    // iSubItem = 插入位置
    // text = 列标题
    // width = 列宽
    // fmt = 对齐格式
    // iImage = 图片索引
    int InsertColumn(int iSubItem = -1, LPCWSTR text = 0, int width = 50, int fmt = 0, int iImage = -1) const
    {
        if (!text)text = L"";
        LVCOLUMNW data = { 0 };
        data.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
        if (iImage != -1)
            data.mask |= LVCF_IMAGE;
        if (iSubItem < 0)
            iSubItem = GetColumnCount();
        data.fmt = fmt;
        data.iSubItem = iSubItem;
        data.cx = width;
        data.pszText = (LPWSTR)text;
        data.cchTextMax = (int)wcslen(text);
        data.iImage = iImage;
        return ListView_InsertColumn(m_hWnd, iSubItem, &data);
    }

    // 插入列
    // iSubItem = 插入位置
    // text = 列标题
    // width = 列宽
    // fmt = 对齐格式
    // iImage = 图片索引
    int InsertColumn(const LVCOLUMNW* pItem) const
    {
        if (!pItem)return -1;
        int iSubItem = pItem->iSubItem;
        if ( iSubItem < 0 )
            iSubItem = GetColumnCount();
        return ListView_InsertColumn(m_hWnd, iSubItem, pItem);
    }

    // 删除列
    // index = 删除的列索引, 从0开始
    inline BOOL DeleteColumn(int index) const
    {
        if (index < 0)return false;
        return ListView_DeleteColumn(m_hWnd, index);
    }
    // 删除列
    inline void DeleteAllColumn() const
    {
        const int count = GetColumnCount();
        for (int i = 0; i < count; i++)
            ListView_DeleteColumn(m_hWnd, 0);
    }
    // 取列数
    int GetColumnCount() const
    {
        HWND hWnd = ListView_GetHeader(m_hWnd);
        return (int)SendMessageW(hWnd, HDM_GETITEMCOUNT, 0, 0);
    }
    // 取表头句柄
    inline HWND  GetHeader() const
    {
        return ListView_GetHeader(m_hWnd);
    }

    // 插入表项
    // iSubItem = 插入位置
    // text = 列标题
    // width = 列宽
    // fmt = 对齐格式
    // iImage = 图片索引
    int InsertItem(int iItem = -1, LPCWSTR text = 0, int iImage = -1, int iImageState = 0, int iIndent = 0, LPARAM param = 0) const
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_PARAM;
        if (iItem < 0)
            iItem = ListView_GetItemCount(m_hWnd);
        if (text)
        {
            item.mask |= LVIF_TEXT;
            item.pszText = (LPWSTR)text;
            item.cchTextMax = (int)wcslen(text);
        }
        if (iImage < 0)
            iImage = -1;

        item.iItem      = iItem;
        item.iSubItem   = 0;
        item.state      = INDEXTOSTATEIMAGEMASK(iImageState);
        item.stateMask  = LVIS_STATEIMAGEMASK;
        item.iImage     = iImage;
        item.lParam     = param;
        item.iIndent    = iIndent;


        if ( iImageState >= 0 )
            item.mask |= LVIF_STATE;
        if ( iIndent > 0 )
            item.mask |= LVIF_INDENT;
        //if ( iImage >= 0 )
            item.mask |= LVIF_IMAGE;

        int ret = ListView_InsertItem(m_hWnd, &item);

        //LVITEMW lvi;
        //lvi.stateMask = LVIS_STATEIMAGEMASK;
        //lvi.state = INDEXTOSTATEIMAGEMASK(iImageState + 1);
        //Send(LVM_SETITEMSTATE, (WPARAM)( iItem ), (LPARAM)(LV_ITEM*)&lvi);
        //ListView_SetItemState(m_hWnd, iItem, INDEXTOSTATEIMAGEMASK(iImage + 1), LVIS_STATEIMAGEMASK);

        return ret;
    }

    // 插入表项
    int InsertItem(const LVITEMW* pItem)
    {
        return ListView_InsertItem(m_hWnd, pItem);
    }

    // 取列顺序, 需要外部申请缓冲区, 缓冲区可以使用 GetColumnCount() 获取缓冲区成员数
    inline int GetColumnOrderArray(int* pArr, int nArrCount) const
    {
        return ListView_GetColumnOrderArray(m_hWnd, nArrCount, pArr);
    }
    // 置列顺序, 缓冲区可以使用 GetColumnCount() 获取缓冲区成员数
    inline BOOL SetColumnOrderArray(int* pArr, int nArrCount) const
    {
        return ListView_SetColumnOrderArray(m_hWnd, nArrCount, pArr);
    }
    // 取列数据
    int GetColumn(int iSubItem, LVCOLUMNW* pColumn) const
    {
        return ListView_GetColumn(m_hWnd, iSubItem, pColumn);
    }
    // 取列数据
    wstr GetColumnText(int iSubItem) const
    {
        LVCOLUMNW pColumn = { 0 };
        pColumn.mask = LVCF_TEXT;
        pColumn.cchTextMax = 1024;
        wstr ret(pColumn.cchTextMax);
        pColumn.pszText = ret.data();
        ListView_GetColumn(m_hWnd, iSubItem, &pColumn);
        ret.resize_strlen();
        return ret;
    }
    // 取列宽
    int GetColumnWidth(int iSubItem) const
    {
        return ListView_GetColumnWidth(m_hWnd, iSubItem);
    }
    // 置列数据
    int SetColumn(int iSubItem, const LVCOLUMNW* pColumn) const
    {
        return ListView_SetColumn(m_hWnd, iSubItem, pColumn);
    }
    // 置列标题
    int SetColumnText(int iSubItem, LPCWSTR text) const
    {
        LVCOLUMNW pColumn = { 0 };
        pColumn.mask = LVCF_TEXT;
        pColumn.pszText = (LPWSTR)text;
        return ListView_SetColumn(m_hWnd, iSubItem, &pColumn);
    }
    // 置列宽
    int SetColumnWidth(int iSubItem, int width) const
    {
        return ListView_SetColumnWidth(m_hWnd, iSubItem, width);
    }
    // 置列图片
    BOOL SetColumnImage(int iSubItem, int iImage) const
    {
        HWND hHeader = GetHeader();
        if ( !hHeader )return false;
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_IMAGE;
        hdi.iImage = iImage;
        return Header_SetItem(hHeader, iSubItem, &hdi);
    }
    
    // 进入编辑, 成功返回编辑框的句柄
    // iItem = 编辑的索引, 取消则设置为-1
    HWND EditLabel(int iItem) const
    {
        SetFocus();
        return ListView_EditLabel(m_hWnd, iItem);
    }

public:
    // 设置列表扩展样式
    LONG_PTR SetListStyle(LONG_PTR style) const
    {
        return (LONG_PTR)ListView_SetExtendedListViewStyle(m_hWnd, style);
    }
    // 取列表扩展样式
    LONG_PTR GetListStyle() const
    {
        return (LONG_PTR)ListView_GetExtendedListViewStyle(m_hWnd);
    }
    // 修改列表扩展样式, 新增或者删除一个或多个扩展样式
    LONG_PTR ModifyListStyle(LONG_PTR style, BOOL isRemove) const
    {
        LONG_PTR oldStyle = GetListStyle();
        if (isRemove)
        {
            return SetListStyle(oldStyle & ~style);
        }
        return SetListStyle(oldStyle | style);
    }


public:
    // 确定是否选择了列表视图控件中的项目
    inline BOOL IsCheck(int index) const
    {
        return ListView_GetCheckState(m_hWnd, index) == 1;
    }

    BOOL GetItemRect(int index, RECT* rcItem, int code = LVIR_BOUNDS) const
    {
        return ListView_GetItemRect(m_hWnd, index, rcItem, code);
    }
    BOOL InvalidateItem(int index, int code = LVIR_BOUNDS) const
    {
        RECT rc;
        if ( GetItemRect(index, &rc, code) )
            return InvalidateRect(&rc);
        return InvalidateRect();
    }
    // 取表项间距, isSmall = 是否为小图标
    BOOL GetItemSpacing(BOOL isSmall) const
    {
        return ListView_GetItemSpacing(m_hWnd, isSmall);
    }
    // 确定是否选择了列表视图控件中的项目
    inline BOOL GetCheck(int index) const
    {
        return IsCheck(index);
    }

    // 在列表视图控件中选择或取消选择一个项目
    inline void SetCheck(int index, BOOL isCheck) const
    {
        ListView_SetCheckState(m_hWnd, index, isCheck);
    }

    // 保证显示
    inline BOOL SetEnsurVisible(int index, BOOL isShow = true) const
    {
        return Send(LVM_ENSUREVISIBLE, index, isShow) != 0;
    }
    // 置现行选中项/选择表项, isShow = 是否保证显示
    inline void SetIndex(int index, BOOL isShow = false) const
    {
        if ( index == -1 )
        {
            UnSelectItem(GetIndex());
            return;
        }
        LVITEMW item = { 0 };
        item.mask = LVIF_STATE;
        item.state = LVIS_SELECTED | LVIS_FOCUSED;
        item.stateMask = item.state;
        Send(LVM_SETITEMSTATE, index, (LPARAM)&item);
        if (isShow)
            SetEnsurVisible(index, true);
    }

    // 取现行选中项
    inline int GetIndex() const
    {
        return ListView_GetNextItem(m_hWnd, -1, ( LVNI_FOCUSED | LVNI_SELECTED ));
    }

    // 取项目状态, 默认获取所有状态, 包括状态图片, 叠加图片, 和项目状态等信息
    // mask可选的值为以下常量常量
    // LVIS_CUT             该项目被标记为剪切和粘贴操作
    // LVIS_DROPHILITED     该项目将突出显示为拖放目标
    // LVIS_FOCUSED         该项目具有焦点，因此被标准焦点矩形包围。尽管可以选择多个项目，但是只有一个项目可以具有焦点
    // LVIS_SELECTED        该项被选择。所选项目的外观取决于其是否具有焦点，还取决于用于选择的系统颜色
    // LVIS_OVERLAYMASK     使用此蒙版检索项目的覆盖图像索引
    // LVIS_STATEIMAGEMASK  使用此掩码可以检索项目的状态图像索引
    inline UINT GetItemState(int index, int mask = 0xffff) const
    {
        return ListView_GetItemState(m_hWnd, index, mask);
    }

    // 更改列表视图控件中项目的状态
    // index = 列表视图项的索引。如果此参数为-1，则状态更改将应用于所有项目
    // state = 项目的新状态位
    // mask = 您要设置或清除的状态参数的位
    inline BOOL SetItemState(int index, UINT state, UINT mask) const
    {
        LVITEMW item = { 0 };
        item.iItem = index;
        item.state = state;
        item.stateMask = mask;
        return Send(LVM_SETITEMSTATE, index, (LPARAM)&item) != 0;
    }

    // 选择表项, 选择表项不等于设置现行选中项, 现行选中项会多一个焦点的样式
    inline BOOL SelectItem(int index) const
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_STATE;
        item.iItem = index;
        item.state = LVIS_SELECTED;
        item.stateMask = LVIS_SELECTED;
        return Send(LVM_SETITEMSTATE, index, (LPARAM)&item) != 0;
    }
    // 表项是否被选择
    inline BOOL IsSelectItem(int index) const
    {
        return GetItemState(index, LVIS_SELECTED) == LVIS_SELECTED;
    }

    // 取消选择表项
    inline BOOL UnSelectItem(int index) const
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_STATE;
        item.iItem = index;
        item.state = ~LVIS_SELECTED;
        item.stateMask = LVIS_SELECTED;
        return Send(LVM_SETITEMSTATE, index, (LPARAM)&item) != 0;
    }

    // 置标题
    inline void SetItemText(int index, int iSubItem, LPCWSTR text) const
    {
        ListView_SetItemText(m_hWnd, index, iSubItem, (LPWSTR)text);
    }
    // 置标题
    void SetTitle(int index, int iSubItem, LPCWSTR text) const
    {
        SetItemText(index, iSubItem, text);
    }

    // 取标题
    inline wstr GetItemText(int index, int iSubItem) const
    {
        int len = 0x800;
        wstr ret;
        int strLen = 0;
        do 
        {
            len = len * 2 + 100;
            ret.resize(len);
            LVITEMW lvi = { 0 };
            lvi.iSubItem = iSubItem;
            lvi.cchTextMax = len;
            lvi.pszText = ret.data();
            strLen = (int)(INT_PTR)Send(LVM_GETITEMTEXT, index, (LPARAM)&lvi);
            //ListView_GetItemText(m_hWnd, index, iSubItem, ret.data(), len);

        } while (strLen == len - 1);

        ret.resize(strLen);
        return ret;
    }

    // 取被选择项目数
    inline UINT GetSelectedCount() const
    {
        return ListView_GetSelectedCount(m_hWnd);
    }

    // 取被选择项目, 返回被选择项目数
    // pIndexArr = 存放选中项的结果, 如果为0则返回选中项目数
    // nIndrxArrCount = 数组缓冲区成员数
    inline UINT GetSelecteds(int* pIndexArr, int nIndrxArrCount) const
    {
        if (!pIndexArr)return GetSelectedCount();

        DWORD len = GetItemCount();
        int n = 0;
        for (DWORD i = 0; i < len; i++)
        {
            if (GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
            {
                pIndexArr[n++] = i;
                if ( n >= nIndrxArrCount )
                    break;
            }
        }
        return (UINT)n;
    }

    // 在列表或报表视图中获取最顶部可见项目的索引
    // 如果成功，则返回项目的索引。如果列表视图控件处于图标或小图标视图，或者列表视图控件处于启用了组的详细信息视图，则返回零
    inline int GetTopIndex() const
    {
        return GetFirstVisible();
    }

    // 在列表或报表视图中获取最顶部可见项目的索引
    // 如果成功，则返回项目的索引。如果列表视图控件处于图标或小图标视图，或者列表视图控件处于启用了组的详细信息视图，则返回零
    inline int GetFirstVisible() const
    {
        return ListView_GetTopIndex(m_hWnd);
    }

    // 计算在列表视图或报表视图中时可以垂直容纳在列表视图控件的可见区域中的项数。仅计算完全可见的项目
    // 如果成功，则返回完全可见项的数目。如果当前视图是图标视图或小图标视图，则返回值是列表视图控件中的项总数
    inline int GetCountPerPage() const
    {
        return ListView_GetCountPerPage(m_hWnd);
    }
    
#ifndef LVM_ISITEMVISIBLE
#define LVM_ISITEMVISIBLE (LVM_FIRST + 182)
#endif
    // 指示列表视图控件中的项目是否可见
    // index = 列表视图控件中项目的索引
    inline BOOL IsItemVisible(int index) const
    {
        return Send(LVM_ISITEMVISIBLE, (WPARAM)( index ), (LPARAM)0) != 0;
    }
    

    // 从坐标获取项目索引, x,y = 客户区坐标
    inline int HitTest(int x, int y, int* iSubItem = 0, int* pFlags = 0) const
    {
        LVHITTESTINFO info = { 0 };
        info.pt.x = x;
        info.pt.y = y;
        int index = ListView_HitTest(m_hWnd, &info);
        if (iSubItem)*iSubItem = info.iSubItem;
        if (pFlags)*pFlags = info.flags;
        return index;
    }

    // 从坐标获取项目索引, x,y = 客户区坐标
    inline int GetItemFormPoint(int x, int y, int* iSubItem = 0) const
    {
        return HitTest(x, y, iSubItem);
    }
    // 从坐标获取项目列索引, x,y = 客户区坐标
    inline int SubItemHitTest(int x, int y, int* iSubItem = 0, int* pFlags = 0) const
    {
        LVHITTESTINFO info = { 0 };
        info.pt.x = x;
        info.pt.y = y;
        int ret = ListView_SubItemHitTest(m_hWnd, &info);
        if ( iSubItem ) *iSubItem = info.iSubItem;
        if ( pFlags ) *pFlags = info.flags;
        return info.iItem;
    }
    // 从坐标获取项目列索引, x,y = 客户区坐标
    inline int GetSubItemFromPoint(int x, int y) const
    {
        return SubItemHitTest(x, y);
    }
    
    // 在图标视图中排列项目
    // code LVA_DEFAULT=根据列表视图控件的当前对齐样式（默认值）对齐项
    //      LVA_SNAPTOGRID=将所有图标捕捉到最近的网格位置
    inline BOOL Arrange(int code) const
    {
        return ListView_Arrange(m_hWnd, code);
    }
#ifndef LVM_CANCELEDITLABEL
#define LVM_CANCELEDITLABEL (LVM_FIRST + 179)
#endif
    // 取消项目文本编辑操作
    inline void CancelEditLabel() const
    {
        Send(LVM_CANCELEDITLABEL, (WPARAM)0, (LPARAM)0);
    }
    // 为指定项创建拖动图像列表, 当不再需要映像列表时, 应用程序负责销毁映像列表
    // index = 项目索引
    // pptUpLeft = 接收图像左上角在视图坐标中的初始位置
    inline HIMAGELIST CreateDragImage(int index, POINT* pptUpLeft) const 
    {
        return ListView_CreateDragImage(m_hWnd, index, pptUpLeft);
    }

    // 设置背景颜色
    inline BOOL SetBkColor(COLORREF clrBk) const
    {
        return ListView_SetBkColor(m_hWnd, clrBk);
    }

    // 设置列表视图控件中文本的背景色
    inline BOOL SetTextBkColor(COLORREF clrTextBk) const
    {
        return ListView_SetTextBkColor(m_hWnd, clrTextBk);
    }

    // 设置列表视图控件的文本颜色
    inline BOOL SetTextColor(COLORREF clrText) const
    {
        return ListView_SetTextColor(m_hWnd, clrText);
    }
    // 设置背景颜色
    inline COLORREF GetBkColor() const
    {
        return ListView_GetBkColor(m_hWnd);
    }

    // 设置列表视图控件中文本的背景色
    inline COLORREF GetTextBkColor() const
    {
        return ListView_GetTextBkColor(m_hWnd);
    }

    // 设置列表视图控件的文本颜色
    inline COLORREF GetTextColor() const
    {
        return ListView_GetTextColor(m_hWnd);
    }

    // 将图像列表分配给列表视图控件
    // hImgList = 要分配的图像列表的句柄
    // iImageList = 图像列表的类型, LVSIL_ 开头常量
    inline HIMAGELIST SetImageList(HIMAGELIST hImgList, int iImageList = LVSIL_SMALL) const
    {
        return ListView_SetImageList(m_hWnd, hImgList, iImageList);
    }
    // 将图像列表分配给列表视图控件
    // hImgList = 要分配的图像列表的句柄
    // iImageList = 图像列表的类型, LVSIL_ 开头常量
    inline HIMAGELIST GetImageList(int iImageList = LVSIL_SMALL) const
    {
        return ListView_GetImageList(m_hWnd, iImageList);
    }
    // 搜索具有指定特征的列表视图项
    // iStart = 要在其之后开始搜索的项的索引，或从开头开始的 -1 的索引
    // plvfi = 指向 LVFINDINFO 结构的指针，该结构包含有关要搜索的内容的信息
    inline int FindItem(int iStart, const LVFINDINFOW* plvfi) const
    {
        return ListView_FindItem(m_hWnd, iStart, plvfi);
    }
    // 搜索具有指定特征的列表视图项
    // iStart = 要在其之后开始搜索的项的索引，或从开头开始的 -1 的索引
    // param = 查找的项目数值
    inline int FindItemParam(int iStart, LPARAM param) const
    {
        LVFINDINFOW find = { 0 };
        find.flags = LVFI_PARAM;
        find.lParam = param;
        return ListView_FindItem(m_hWnd, iStart, &find);
    }
    // 搜索具有指定特征的列表视图项
    // iStart = 要在其之后开始搜索的项的索引，或从开头开始的 -1 的索引
    // text = 要查找的文本
    // isCase = 是否区分大小写
    inline int FindItemText(int iStart, LPCWSTR text, BOOL isCase) const
    {
        LVFINDINFOW find = { 0 };
        find.flags = isCase ? LVFI_STRING : LVFI_PARTIAL;
        find.psz = text;
        return ListView_FindItem(m_hWnd, iStart, &find);
    }
    
    // 取项目的坐标
    // iItem = 项目索引
    // ppt = 接收坐标, 坐标是项目的左上角, 基于组件左上角
    inline BOOL GetItemPosition(int iItem, PPOINT ppt) const
    {
        return ListView_GetItemPosition(m_hWnd, iItem, ppt);
    }

#ifndef LVM_GETOUTLINECOLOR
#define LVM_GETOUTLINECOLOR (LVM_FIRST + 176)
#endif
#ifndef LVM_SETOUTLINECOLOR
#define LVM_SETOUTLINECOLOR (LVM_FIRST + 177)
#endif
    
    // 如果设置了LVS_EX_BORDERSELECT扩展窗口样式，则检索列表视图控件边框的颜色
    inline BOOL GetOutlineColor() const
    {
        return Send(LVM_GETOUTLINECOLOR, 0, 0) != FALSE;
    }
    
    // 如果设置了LVS_EX_BORDERSELECT扩展窗口样式，则设置列表视图控件边框的颜色
    inline BOOL SetOutlineColor(COLORREF clr) const
    {
        return Send(LVM_SETOUTLINECOLOR, 0, clr) != FALSE;
    }





};


