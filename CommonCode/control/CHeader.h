#pragma once
#include "WndBase.h"
class CHeader : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return WC_HEADERW; }

public:
    CHeader() :CControlBase() { ; }
    virtual ~CHeader() { ; }
public:
    virtual inline LPCWSTR className() { return WC_HEADERW; }

public:
    inline HWND create(HWND hWndParent, int x, int y, int cx, int cy, LONG_PTR styleEx, LONG_PTR style, LONG_PTR ID, void* lpParam = 0)
    {
        if ( !CWndBase::create((int)styleEx, 0, (int)style | WS_CHILD, x, y, cx, cy, hWndParent, ID, lpParam) )return 0;
        return m_hWnd;
    }
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
                        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0)
    {
        return CWndBase::create(dwExStyle, lpszTitle, dwStyle | WS_CHILD, x, y, nWidth, nHeight, hWndParent, ID, lpParam);
    }

    // 插入项目
    inline int InsertItem(int index, const HDITEMW* pItem)
    {
        if ( index < 0 ) index = GetItemCount();
        return (int)Send(HDM_INSERTITEMW, index, (LPARAM)pItem);
    }
    // 插入项目
    inline int InsertItem(int index = -1, int width = 50, LPCWSTR lpszTitle = L"", int iImage = -1, LPARAM lParam = 0)
    {
        HDITEMW item = { 0 };
        item.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH | HDI_LPARAM;
        if ( iImage >= 0 )
            item.mask |= HDI_IMAGE, item.fmt |= HDF_IMAGE;
        item.cxy = width;
        item.iImage = iImage;
        item.pszText = (LPWSTR)lpszTitle;
        item.lParam = lParam;
        return InsertItem(index, &item);
    }

    // 取项目数
    inline int GetItemCount()
    {
        return (int)Send(HDM_GETITEMCOUNT, 0, 0);
    }

    // 删除项目
    inline int DeleteItem(int index)
    {
        return (int)Send(HDM_DELETEITEM, index, 0);
    }
    // 清空项目
    inline void clear()
    {
        const int count = GetItemCount();
        for ( int i = 0; i < count; i++ )
        {
            Send(HDM_DELETEITEM, 0, 0);
        }
    }
    // 取标题
    inline wstr GetItemText(int index)
    {
        int len = 0x800;
        wstr ret;
        int strLen = 0;
        do
        {
            len = len * 2 + 100;
            ret.resize(len);
            HDITEMW hdi = { 0 };
            hdi.mask = HDI_TEXT;
            hdi.cchTextMax = len;
            hdi.pszText = ret.data();
            strLen = (int)(INT_PTR)Send(HDM_GETITEMW, index, (LPARAM)&hdi);
        } while ( strLen == len - 1 );

        ret.resize(strLen);
        return ret;
    }
    // 置标题
    inline bool SetItemText(int index, LPCWSTR text)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_TEXT;
        hdi.pszText = (LPWSTR)text;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }
    // 取图片间距
    inline int GetBitmapMargin(int index)
    {
        return (int)Send(HDM_GETBITMAPMARGIN, 0, 0);
    }
    // 置图片间距, 返回位图边距的宽度(以像素为单位). 如果以前未指定位图边距, 则返回默认值 3 * GetSystemMetrics(SM_CXEDGE)
    inline int SetBitmapMargin(int index, int nMargin)
    {
        return (int)Send(HDM_SETBITMAPMARGIN, nMargin, 0);
    }

    // 取项目数据, 如果isAll为true, 则内部会把mask填充上标志, 会根据pszText参数决定是否添加或删除文本标志
    inline bool GetItem(int index, HDITEMW* pHdi, bool isAll = false)
    {
        if ( isAll )
        {
            if ( pHdi->pszText && pHdi->cchTextMax > 0 )
                pHdi->mask |= HDI_TEXT;
            else
                pHdi->mask &= ~HDI_TEXT;
            pHdi->mask |= HDI_BITMAP | HDI_DI_SETITEM | HDI_FORMAT | HDI_HEIGHT | HDI_IMAGE | HDI_LPARAM | HDI_ORDER | HDI_STATE | HDI_WIDTH;
        }
        return Send(HDM_GETITEMW, index, (LPARAM)pHdi) != 0;

    }
    // 置项目数据
    inline bool SetItem(int index, const HDITEMW* pHdi)
    {
        return Send(HDM_SETITEMW, index, (LPARAM)pHdi) != 0;
    }

    // 取图片, 返回图片索引
    inline int GetItemImage(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_IMAGE;
        if ( (int)(INT_PTR)Send(HDM_GETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.iImage;
        return -1;
    }
    // 置图片
    inline bool SetItemImage(int index, int iImage)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_IMAGE;
        hdi.iImage = iImage;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }

    // 取项目数值
    inline LPARAM GetItemParam(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_LPARAM;
        if ( Send(HDM_GETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.lParam;
        return 0;
    }

    // 置项目数值
    inline bool SetItemParam(int index, LPARAM lParam)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_LPARAM;
        hdi.lParam = lParam;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }

    // 取项目宽度
    inline int GetItemWidth(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_WIDTH;
        if ( Send(HDM_GETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.cxy;
        return 0;
    }

    // 置项目宽度
    inline bool SetItemWidth(int index, int nWidth)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_WIDTH;
        hdi.cxy = nWidth;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }
    // 取项目格式
    inline int GetItemFormat(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_FORMAT;
        if ( Send(HDM_GETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.fmt;
        return 0;
    }

    // 置项目格式
    inline bool SetItemFormat(int index, int fmt)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_FORMAT;
        hdi.fmt = fmt;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }

    // 取项目序号
    inline int GetItemOrder(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_ORDER;
        if ( Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.iOrder;
        return 0;
    }
    
    // 取图片组
    inline HIMAGELIST GetImageList(int type = HDSIL_NORMAL)
    {
        return (HIMAGELIST)Send(HDM_GETIMAGELIST, type, 0);
    }
    
    // 置图片组
    inline HIMAGELIST SetImageList(HIMAGELIST hImgList, int type = HDSIL_NORMAL)
    {
        return (HIMAGELIST)Send(HDM_SETIMAGELIST, type, (LPARAM)hImgList);
    }
    // 取项目矩形
    inline bool GetImageRect(int index, PRECT prc)
    {
        if ( !prc )return false;
        return Send(HDM_GETITEMRECT, index, (LPARAM)prc) != 0;
    }
    // 取排序数组, 需要外部申请缓冲区, 缓冲区成员数必须等于项目数, 可以使用 GetItemCount获取
    inline bool GetOrderArray(int* pArr, int nArrCount)
    {
        return Send(HDM_GETORDERARRAY, nArrCount, (LPARAM)pArr) != 0;
    }
    // 置排序数组, 缓冲区成员数必须等于项目数, 可以使用 GetItemCount 获取
    // pArr = 指向数组的指针, 该数组指定项应从左到右的显示顺序. 例如, 如果数组的内容为 {2, 0, 1}, 则该控件将从左到右显示项2, 项0 和项1
    inline bool SetOrderArray(int* pArr, int nArrCount)
    {
        return Send(HDM_SETORDERARRAY, nArrCount, (LPARAM)pArr) != 0;
    }
    // 顺序到索引, 返回项目索引
    // nOrder = 顺序, 最左边是0
    inline int OrderToIndex(int nOrder)
    {
        return (int)Send(HDM_ORDERTOINDEX, nOrder, 0);
    }
    // 取坐标项目, pFlags 接收坐标在项目的哪个位置
    inline int HitTest(const int x, const int y, int* pFlags = 0)
    {
        HDHITTESTINFO hi = { 0 };
        hi.pt.x = x;
        hi.pt.y = y;
        Send(HDM_ORDERTOINDEX, 0, (LPARAM)&hi);
        if ( pFlags )*pFlags = hi.flags;
        return hi.iItem;
    }


};

