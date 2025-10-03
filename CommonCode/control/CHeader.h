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

    // ������Ŀ
    inline int InsertItem(int index, const HDITEMW* pItem)
    {
        if ( index < 0 ) index = GetItemCount();
        return (int)Send(HDM_INSERTITEMW, index, (LPARAM)pItem);
    }
    // ������Ŀ
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

    // ȡ��Ŀ��
    inline int GetItemCount()
    {
        return (int)Send(HDM_GETITEMCOUNT, 0, 0);
    }

    // ɾ����Ŀ
    inline int DeleteItem(int index)
    {
        return (int)Send(HDM_DELETEITEM, index, 0);
    }
    // �����Ŀ
    inline void clear()
    {
        const int count = GetItemCount();
        for ( int i = 0; i < count; i++ )
        {
            Send(HDM_DELETEITEM, 0, 0);
        }
    }
    // ȡ����
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
    // �ñ���
    inline bool SetItemText(int index, LPCWSTR text)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_TEXT;
        hdi.pszText = (LPWSTR)text;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }
    // ȡͼƬ���
    inline int GetBitmapMargin(int index)
    {
        return (int)Send(HDM_GETBITMAPMARGIN, 0, 0);
    }
    // ��ͼƬ���, ����λͼ�߾�Ŀ��(������Ϊ��λ). �����ǰδָ��λͼ�߾�, �򷵻�Ĭ��ֵ 3 * GetSystemMetrics(SM_CXEDGE)
    inline int SetBitmapMargin(int index, int nMargin)
    {
        return (int)Send(HDM_SETBITMAPMARGIN, nMargin, 0);
    }

    // ȡ��Ŀ����, ���isAllΪtrue, ���ڲ����mask����ϱ�־, �����pszText���������Ƿ���ӻ�ɾ���ı���־
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
    // ����Ŀ����
    inline bool SetItem(int index, const HDITEMW* pHdi)
    {
        return Send(HDM_SETITEMW, index, (LPARAM)pHdi) != 0;
    }

    // ȡͼƬ, ����ͼƬ����
    inline int GetItemImage(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_IMAGE;
        if ( (int)(INT_PTR)Send(HDM_GETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.iImage;
        return -1;
    }
    // ��ͼƬ
    inline bool SetItemImage(int index, int iImage)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_IMAGE;
        hdi.iImage = iImage;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }

    // ȡ��Ŀ��ֵ
    inline LPARAM GetItemParam(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_LPARAM;
        if ( Send(HDM_GETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.lParam;
        return 0;
    }

    // ����Ŀ��ֵ
    inline bool SetItemParam(int index, LPARAM lParam)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_LPARAM;
        hdi.lParam = lParam;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }

    // ȡ��Ŀ���
    inline int GetItemWidth(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_WIDTH;
        if ( Send(HDM_GETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.cxy;
        return 0;
    }

    // ����Ŀ���
    inline bool SetItemWidth(int index, int nWidth)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_WIDTH;
        hdi.cxy = nWidth;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }
    // ȡ��Ŀ��ʽ
    inline int GetItemFormat(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_FORMAT;
        if ( Send(HDM_GETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.fmt;
        return 0;
    }

    // ����Ŀ��ʽ
    inline bool SetItemFormat(int index, int fmt)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_FORMAT;
        hdi.fmt = fmt;
        return Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0;
    }

    // ȡ��Ŀ���
    inline int GetItemOrder(int index)
    {
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_ORDER;
        if ( Send(HDM_SETITEMW, index, (LPARAM)&hdi) != 0 )
            return hdi.iOrder;
        return 0;
    }
    
    // ȡͼƬ��
    inline HIMAGELIST GetImageList(int type = HDSIL_NORMAL)
    {
        return (HIMAGELIST)Send(HDM_GETIMAGELIST, type, 0);
    }
    
    // ��ͼƬ��
    inline HIMAGELIST SetImageList(HIMAGELIST hImgList, int type = HDSIL_NORMAL)
    {
        return (HIMAGELIST)Send(HDM_SETIMAGELIST, type, (LPARAM)hImgList);
    }
    // ȡ��Ŀ����
    inline bool GetImageRect(int index, PRECT prc)
    {
        if ( !prc )return false;
        return Send(HDM_GETITEMRECT, index, (LPARAM)prc) != 0;
    }
    // ȡ��������, ��Ҫ�ⲿ���뻺����, ��������Ա�����������Ŀ��, ����ʹ�� GetItemCount��ȡ
    inline bool GetOrderArray(int* pArr, int nArrCount)
    {
        return Send(HDM_GETORDERARRAY, nArrCount, (LPARAM)pArr) != 0;
    }
    // ����������, ��������Ա�����������Ŀ��, ����ʹ�� GetItemCount ��ȡ
    // pArr = ָ�������ָ��, ������ָ����Ӧ�����ҵ���ʾ˳��. ����, ������������Ϊ {2, 0, 1}, ��ÿؼ�����������ʾ��2, ��0 ����1
    inline bool SetOrderArray(int* pArr, int nArrCount)
    {
        return Send(HDM_SETORDERARRAY, nArrCount, (LPARAM)pArr) != 0;
    }
    // ˳������, ������Ŀ����
    // nOrder = ˳��, �������0
    inline int OrderToIndex(int nOrder)
    {
        return (int)Send(HDM_ORDERTOINDEX, nOrder, 0);
    }
    // ȡ������Ŀ, pFlags ������������Ŀ���ĸ�λ��
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

