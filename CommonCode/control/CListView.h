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

    // ȡ����, index=��������, colIndex=������
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
    // ȡ�������
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


    // �ñ�����, ���ר��, ����2��ʾ���ñ�����Ƿ��ػ��Լ��Ƿ�ı������λ��
    BOOL SetItemCount(DWORD count, DWORD flags = LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL) const
    {
        return ListView_SetItemCountEx(m_hWnd, count, flags) != 0;
    }
    // ȡ������
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

    // ��״̬ͼƬ
    // iItem = ��Ŀ����
    // iImage = ״̬ͼƬ��������ָ����״̬ͼƬ�顱�����е�ĳ��ͼƬ������ֵ�� 0 ��ʼ��-1 ��ʾ��ͼƬ�������������ʡ�ԣ�Ĭ��ֵΪ -1
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
    // ɾ��������Ŀ
    BOOL DeleteAllItems() const
    {
        return ListView_DeleteAllItems(m_hWnd);
    }
    // ɾ����Ŀ
    BOOL DeleteItem(int index) const
    {
        return ListView_DeleteItem(m_hWnd, index);
    }

    // ������
    // iSubItem = ����λ��
    // text = �б���
    // width = �п�
    // fmt = �����ʽ
    // iImage = ͼƬ����
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

    // ������
    // iSubItem = ����λ��
    // text = �б���
    // width = �п�
    // fmt = �����ʽ
    // iImage = ͼƬ����
    int InsertColumn(const LVCOLUMNW* pItem) const
    {
        if (!pItem)return -1;
        int iSubItem = pItem->iSubItem;
        if ( iSubItem < 0 )
            iSubItem = GetColumnCount();
        return ListView_InsertColumn(m_hWnd, iSubItem, pItem);
    }

    // ɾ����
    // index = ɾ����������, ��0��ʼ
    inline BOOL DeleteColumn(int index) const
    {
        if (index < 0)return false;
        return ListView_DeleteColumn(m_hWnd, index);
    }
    // ɾ����
    inline void DeleteAllColumn() const
    {
        const int count = GetColumnCount();
        for (int i = 0; i < count; i++)
            ListView_DeleteColumn(m_hWnd, 0);
    }
    // ȡ����
    int GetColumnCount() const
    {
        HWND hWnd = ListView_GetHeader(m_hWnd);
        return (int)SendMessageW(hWnd, HDM_GETITEMCOUNT, 0, 0);
    }
    // ȡ��ͷ���
    inline HWND  GetHeader() const
    {
        return ListView_GetHeader(m_hWnd);
    }

    // �������
    // iSubItem = ����λ��
    // text = �б���
    // width = �п�
    // fmt = �����ʽ
    // iImage = ͼƬ����
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

    // �������
    int InsertItem(const LVITEMW* pItem)
    {
        return ListView_InsertItem(m_hWnd, pItem);
    }

    // ȡ��˳��, ��Ҫ�ⲿ���뻺����, ����������ʹ�� GetColumnCount() ��ȡ��������Ա��
    inline int GetColumnOrderArray(int* pArr, int nArrCount) const
    {
        return ListView_GetColumnOrderArray(m_hWnd, nArrCount, pArr);
    }
    // ����˳��, ����������ʹ�� GetColumnCount() ��ȡ��������Ա��
    inline BOOL SetColumnOrderArray(int* pArr, int nArrCount) const
    {
        return ListView_SetColumnOrderArray(m_hWnd, nArrCount, pArr);
    }
    // ȡ������
    int GetColumn(int iSubItem, LVCOLUMNW* pColumn) const
    {
        return ListView_GetColumn(m_hWnd, iSubItem, pColumn);
    }
    // ȡ������
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
    // ȡ�п�
    int GetColumnWidth(int iSubItem) const
    {
        return ListView_GetColumnWidth(m_hWnd, iSubItem);
    }
    // ��������
    int SetColumn(int iSubItem, const LVCOLUMNW* pColumn) const
    {
        return ListView_SetColumn(m_hWnd, iSubItem, pColumn);
    }
    // ���б���
    int SetColumnText(int iSubItem, LPCWSTR text) const
    {
        LVCOLUMNW pColumn = { 0 };
        pColumn.mask = LVCF_TEXT;
        pColumn.pszText = (LPWSTR)text;
        return ListView_SetColumn(m_hWnd, iSubItem, &pColumn);
    }
    // ���п�
    int SetColumnWidth(int iSubItem, int width) const
    {
        return ListView_SetColumnWidth(m_hWnd, iSubItem, width);
    }
    // ����ͼƬ
    BOOL SetColumnImage(int iSubItem, int iImage) const
    {
        HWND hHeader = GetHeader();
        if ( !hHeader )return false;
        HDITEMW hdi = { 0 };
        hdi.mask = HDI_IMAGE;
        hdi.iImage = iImage;
        return Header_SetItem(hHeader, iSubItem, &hdi);
    }
    
    // ����༭, �ɹ����ر༭��ľ��
    // iItem = �༭������, ȡ��������Ϊ-1
    HWND EditLabel(int iItem) const
    {
        SetFocus();
        return ListView_EditLabel(m_hWnd, iItem);
    }

public:
    // �����б���չ��ʽ
    LONG_PTR SetListStyle(LONG_PTR style) const
    {
        return (LONG_PTR)ListView_SetExtendedListViewStyle(m_hWnd, style);
    }
    // ȡ�б���չ��ʽ
    LONG_PTR GetListStyle() const
    {
        return (LONG_PTR)ListView_GetExtendedListViewStyle(m_hWnd);
    }
    // �޸��б���չ��ʽ, ��������ɾ��һ��������չ��ʽ
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
    // ȷ���Ƿ�ѡ�����б���ͼ�ؼ��е���Ŀ
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
    // ȡ������, isSmall = �Ƿ�ΪСͼ��
    BOOL GetItemSpacing(BOOL isSmall) const
    {
        return ListView_GetItemSpacing(m_hWnd, isSmall);
    }
    // ȷ���Ƿ�ѡ�����б���ͼ�ؼ��е���Ŀ
    inline BOOL GetCheck(int index) const
    {
        return IsCheck(index);
    }

    // ���б���ͼ�ؼ���ѡ���ȡ��ѡ��һ����Ŀ
    inline void SetCheck(int index, BOOL isCheck) const
    {
        ListView_SetCheckState(m_hWnd, index, isCheck);
    }

    // ��֤��ʾ
    inline BOOL SetEnsurVisible(int index, BOOL isShow = true) const
    {
        return Send(LVM_ENSUREVISIBLE, index, isShow) != 0;
    }
    // ������ѡ����/ѡ�����, isShow = �Ƿ�֤��ʾ
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

    // ȡ����ѡ����
    inline int GetIndex() const
    {
        return ListView_GetNextItem(m_hWnd, -1, ( LVNI_FOCUSED | LVNI_SELECTED ));
    }

    // ȡ��Ŀ״̬, Ĭ�ϻ�ȡ����״̬, ����״̬ͼƬ, ����ͼƬ, ����Ŀ״̬����Ϣ
    // mask��ѡ��ֵΪ���³�������
    // LVIS_CUT             ����Ŀ�����Ϊ���к�ճ������
    // LVIS_DROPHILITED     ����Ŀ��ͻ����ʾΪ�Ϸ�Ŀ��
    // LVIS_FOCUSED         ����Ŀ���н��㣬��˱���׼������ΰ�Χ�����ܿ���ѡ������Ŀ������ֻ��һ����Ŀ���Ծ��н���
    // LVIS_SELECTED        ���ѡ����ѡ��Ŀ�����ȡ�������Ƿ���н��㣬��ȡ��������ѡ���ϵͳ��ɫ
    // LVIS_OVERLAYMASK     ʹ�ô��ɰ������Ŀ�ĸ���ͼ������
    // LVIS_STATEIMAGEMASK  ʹ�ô�������Լ�����Ŀ��״̬ͼ������
    inline UINT GetItemState(int index, int mask = 0xffff) const
    {
        return ListView_GetItemState(m_hWnd, index, mask);
    }

    // �����б���ͼ�ؼ�����Ŀ��״̬
    // index = �б���ͼ�������������˲���Ϊ-1����״̬���Ľ�Ӧ����������Ŀ
    // state = ��Ŀ����״̬λ
    // mask = ��Ҫ���û������״̬������λ
    inline BOOL SetItemState(int index, UINT state, UINT mask) const
    {
        LVITEMW item = { 0 };
        item.iItem = index;
        item.state = state;
        item.stateMask = mask;
        return Send(LVM_SETITEMSTATE, index, (LPARAM)&item) != 0;
    }

    // ѡ�����, ѡ����������������ѡ����, ����ѡ������һ���������ʽ
    inline BOOL SelectItem(int index) const
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_STATE;
        item.iItem = index;
        item.state = LVIS_SELECTED;
        item.stateMask = LVIS_SELECTED;
        return Send(LVM_SETITEMSTATE, index, (LPARAM)&item) != 0;
    }
    // �����Ƿ�ѡ��
    inline BOOL IsSelectItem(int index) const
    {
        return GetItemState(index, LVIS_SELECTED) == LVIS_SELECTED;
    }

    // ȡ��ѡ�����
    inline BOOL UnSelectItem(int index) const
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_STATE;
        item.iItem = index;
        item.state = ~LVIS_SELECTED;
        item.stateMask = LVIS_SELECTED;
        return Send(LVM_SETITEMSTATE, index, (LPARAM)&item) != 0;
    }

    // �ñ���
    inline void SetItemText(int index, int iSubItem, LPCWSTR text) const
    {
        ListView_SetItemText(m_hWnd, index, iSubItem, (LPWSTR)text);
    }
    // �ñ���
    void SetTitle(int index, int iSubItem, LPCWSTR text) const
    {
        SetItemText(index, iSubItem, text);
    }

    // ȡ����
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

    // ȡ��ѡ����Ŀ��
    inline UINT GetSelectedCount() const
    {
        return ListView_GetSelectedCount(m_hWnd);
    }

    // ȡ��ѡ����Ŀ, ���ر�ѡ����Ŀ��
    // pIndexArr = ���ѡ����Ľ��, ���Ϊ0�򷵻�ѡ����Ŀ��
    // nIndrxArrCount = ���黺������Ա��
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

    // ���б�򱨱���ͼ�л�ȡ����ɼ���Ŀ������
    // ����ɹ����򷵻���Ŀ������������б���ͼ�ؼ�����ͼ���Сͼ����ͼ�������б���ͼ�ؼ����������������ϸ��Ϣ��ͼ���򷵻���
    inline int GetTopIndex() const
    {
        return GetFirstVisible();
    }

    // ���б�򱨱���ͼ�л�ȡ����ɼ���Ŀ������
    // ����ɹ����򷵻���Ŀ������������б���ͼ�ؼ�����ͼ���Сͼ����ͼ�������б���ͼ�ؼ����������������ϸ��Ϣ��ͼ���򷵻���
    inline int GetFirstVisible() const
    {
        return ListView_GetTopIndex(m_hWnd);
    }

    // �������б���ͼ�򱨱���ͼ��ʱ���Դ�ֱ�������б���ͼ�ؼ��Ŀɼ������е���������������ȫ�ɼ�����Ŀ
    // ����ɹ����򷵻���ȫ�ɼ������Ŀ�������ǰ��ͼ��ͼ����ͼ��Сͼ����ͼ���򷵻�ֵ���б���ͼ�ؼ��е�������
    inline int GetCountPerPage() const
    {
        return ListView_GetCountPerPage(m_hWnd);
    }
    
#ifndef LVM_ISITEMVISIBLE
#define LVM_ISITEMVISIBLE (LVM_FIRST + 182)
#endif
    // ָʾ�б���ͼ�ؼ��е���Ŀ�Ƿ�ɼ�
    // index = �б���ͼ�ؼ�����Ŀ������
    inline BOOL IsItemVisible(int index) const
    {
        return Send(LVM_ISITEMVISIBLE, (WPARAM)( index ), (LPARAM)0) != 0;
    }
    

    // �������ȡ��Ŀ����, x,y = �ͻ�������
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

    // �������ȡ��Ŀ����, x,y = �ͻ�������
    inline int GetItemFormPoint(int x, int y, int* iSubItem = 0) const
    {
        return HitTest(x, y, iSubItem);
    }
    // �������ȡ��Ŀ������, x,y = �ͻ�������
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
    // �������ȡ��Ŀ������, x,y = �ͻ�������
    inline int GetSubItemFromPoint(int x, int y) const
    {
        return SubItemHitTest(x, y);
    }
    
    // ��ͼ����ͼ��������Ŀ
    // code LVA_DEFAULT=�����б���ͼ�ؼ��ĵ�ǰ������ʽ��Ĭ��ֵ��������
    //      LVA_SNAPTOGRID=������ͼ�겶׽�����������λ��
    inline BOOL Arrange(int code) const
    {
        return ListView_Arrange(m_hWnd, code);
    }
#ifndef LVM_CANCELEDITLABEL
#define LVM_CANCELEDITLABEL (LVM_FIRST + 179)
#endif
    // ȡ����Ŀ�ı��༭����
    inline void CancelEditLabel() const
    {
        Send(LVM_CANCELEDITLABEL, (WPARAM)0, (LPARAM)0);
    }
    // Ϊָ������϶�ͼ���б�, ��������Ҫӳ���б�ʱ, Ӧ�ó���������ӳ���б�
    // index = ��Ŀ����
    // pptUpLeft = ����ͼ�����Ͻ�����ͼ�����еĳ�ʼλ��
    inline HIMAGELIST CreateDragImage(int index, POINT* pptUpLeft) const 
    {
        return ListView_CreateDragImage(m_hWnd, index, pptUpLeft);
    }

    // ���ñ�����ɫ
    inline BOOL SetBkColor(COLORREF clrBk) const
    {
        return ListView_SetBkColor(m_hWnd, clrBk);
    }

    // �����б���ͼ�ؼ����ı��ı���ɫ
    inline BOOL SetTextBkColor(COLORREF clrTextBk) const
    {
        return ListView_SetTextBkColor(m_hWnd, clrTextBk);
    }

    // �����б���ͼ�ؼ����ı���ɫ
    inline BOOL SetTextColor(COLORREF clrText) const
    {
        return ListView_SetTextColor(m_hWnd, clrText);
    }
    // ���ñ�����ɫ
    inline COLORREF GetBkColor() const
    {
        return ListView_GetBkColor(m_hWnd);
    }

    // �����б���ͼ�ؼ����ı��ı���ɫ
    inline COLORREF GetTextBkColor() const
    {
        return ListView_GetTextBkColor(m_hWnd);
    }

    // �����б���ͼ�ؼ����ı���ɫ
    inline COLORREF GetTextColor() const
    {
        return ListView_GetTextColor(m_hWnd);
    }

    // ��ͼ���б������б���ͼ�ؼ�
    // hImgList = Ҫ�����ͼ���б�ľ��
    // iImageList = ͼ���б������, LVSIL_ ��ͷ����
    inline HIMAGELIST SetImageList(HIMAGELIST hImgList, int iImageList = LVSIL_SMALL) const
    {
        return ListView_SetImageList(m_hWnd, hImgList, iImageList);
    }
    // ��ͼ���б������б���ͼ�ؼ�
    // hImgList = Ҫ�����ͼ���б�ľ��
    // iImageList = ͼ���б������, LVSIL_ ��ͷ����
    inline HIMAGELIST GetImageList(int iImageList = LVSIL_SMALL) const
    {
        return ListView_GetImageList(m_hWnd, iImageList);
    }
    // ��������ָ���������б���ͼ��
    // iStart = Ҫ����֮��ʼ�����������������ӿ�ͷ��ʼ�� -1 ������
    // plvfi = ָ�� LVFINDINFO �ṹ��ָ�룬�ýṹ�����й�Ҫ���������ݵ���Ϣ
    inline int FindItem(int iStart, const LVFINDINFOW* plvfi) const
    {
        return ListView_FindItem(m_hWnd, iStart, plvfi);
    }
    // ��������ָ���������б���ͼ��
    // iStart = Ҫ����֮��ʼ�����������������ӿ�ͷ��ʼ�� -1 ������
    // param = ���ҵ���Ŀ��ֵ
    inline int FindItemParam(int iStart, LPARAM param) const
    {
        LVFINDINFOW find = { 0 };
        find.flags = LVFI_PARAM;
        find.lParam = param;
        return ListView_FindItem(m_hWnd, iStart, &find);
    }
    // ��������ָ���������б���ͼ��
    // iStart = Ҫ����֮��ʼ�����������������ӿ�ͷ��ʼ�� -1 ������
    // text = Ҫ���ҵ��ı�
    // isCase = �Ƿ����ִ�Сд
    inline int FindItemText(int iStart, LPCWSTR text, BOOL isCase) const
    {
        LVFINDINFOW find = { 0 };
        find.flags = isCase ? LVFI_STRING : LVFI_PARTIAL;
        find.psz = text;
        return ListView_FindItem(m_hWnd, iStart, &find);
    }
    
    // ȡ��Ŀ������
    // iItem = ��Ŀ����
    // ppt = ��������, ��������Ŀ�����Ͻ�, ����������Ͻ�
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
    
    // ���������LVS_EX_BORDERSELECT��չ������ʽ��������б���ͼ�ؼ��߿����ɫ
    inline BOOL GetOutlineColor() const
    {
        return Send(LVM_GETOUTLINECOLOR, 0, 0) != FALSE;
    }
    
    // ���������LVS_EX_BORDERSELECT��չ������ʽ���������б���ͼ�ؼ��߿����ɫ
    inline BOOL SetOutlineColor(COLORREF clr) const
    {
        return Send(LVM_SETOUTLINECOLOR, 0, clr) != FALSE;
    }





};


