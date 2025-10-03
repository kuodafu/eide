#pragma once
#include "WndBase.h"
#pragma comment(lib,"Comctl32.lib")

class CListBox :public CControlBase
{
protected:
	virtual inline LPCWSTR SuperClassName() { return WC_LISTBOXW; }

public:
    CListBox() :CControlBase() { ; }
    ~CListBox() { ; }

public:
    virtual inline LPCWSTR className() { return CLASS_LISTBOX; }

public:

	// ��ձ���
	virtual inline void clear() {
        Send(LB_RESETCONTENT, 0, 0);
	}

	// ȡ�б���Ŀ��
	int GetCount() {
        return (int)Send(LB_GETCOUNT, 0, 0);
    }

	// ��ȡ��ѡ�б����ѡ�������
    int GetSelCount() {
        return (int)Send(LB_GETSELCOUNT, 0, 0);
    }

	// ���ñ�����, ��Ҫ��LBS_NODATA���, ������LBS_HASSTRINGS���
	int SetItemCount(int count) {
        return (int)Send(LB_SETCOUNT, count, 0);
    }
    // ���ñ�����, ��Ҫ��LBS_NODATA���, ������LBS_HASSTRINGS���
    int SetCount(int count)
    {
        return (int)Send(LB_SETCOUNT, count, 0);
	}

	// ���ַ�������Ŀ���ݲ����б���С���addString��Ϣ��ͬ��LB_INSERTSTRING��Ϣ���ᵼ�¶Ծ���LBS_SORT��ʽ���б��������
	// ����ֵ�ǲ����ַ�����λ�õ�������������������򷵻�ֵΪLB_ERR�����û���㹻�Ŀռ����洢���ַ������򷵻�ֵΪLB_ERRSPACE
	int insert(LPCWSTR text = 0, int index = -1) {
        return (int)Send(LB_INSERTSTRING, index, (LPARAM)text);
	}
	// ���ַ�����ӵ��б������б�򲻾���LBS_SORT��ʽ�����ַ�����ӵ��б��ĩβ�����򣬽��ַ��������б������б��������
    // ����ֵ���б�����ַ����Ĵ��㿪ʼ��������������������򷵻�ֵΪLB_ERR�����û���㹻�Ŀռ����洢���ַ������򷵻�ֵΪLB_ERRSPACE
	int addString(LPCWSTR text = 0) {
        return (int)Send(LB_ADDSTRING, 0, (LPARAM)text);
	}

	// ɾ���б���е��ַ���
	// ����ֵ���б���ʣ���ַ����ļ��������wParam����ָ�������������б��е���Ŀ�����򷵻�ֵΪLB_ERR
	// index = Ҫɾ�����ַ����Ĵ��㿪ʼ������
	int deleteItem(int index) {
        return (int)Send(LB_DELETESTRING, index, 0);
	}

	// �ڵ�ѡ�б���л�ȡ��ǰ��ѡ��Ŀ������
    // �ڵ�ѡ�б���У�����ֵ�ǵ�ǰ��ѡ��Ŀ�Ĵ��㿪ʼ�����������û��ѡ���򷵻�ֵΪLB_ERR
    int GetIndedx() {
        return GetCurSel();
	}
    // �ڵ�ѡ�б���л�ȡ��ǰ��ѡ��Ŀ������
	// �ڵ�ѡ�б���У�����ֵ�ǵ�ǰ��ѡ��Ŀ�Ĵ��㿪ʼ�����������û��ѡ���򷵻�ֵΪLB_ERR
    int GetCurSel() {
        return (int)Send(LB_GETCURSEL, 0, 0);
	}
	// ��ȡ���ѡ��״̬
	// index = ��Ĵ��㿪ʼ������
    bool GetSel(int index) {
        return (bool)(Send(LB_GETSEL, index, 0) > 0);
    }

	// ѡ��һ���ַ��������б�Ҫ�������������ͼ�С�ѡ���µ��ַ���ʱ���б�򽫴���ǰѡ����ַ�����ɾ��ͻ����ʾ
	// ������������򷵻�ֵΪLB_ERR�����wParam����Ϊ-1����ʹû�з������󣬷���ֵҲΪLB_ERR
	// index = ָ����ѡ�ַ����Ĵ��㿪ʼ������������˲���Ϊ-1�����б������Ϊû��ѡ��
    int SetIndex(int index) {
        return SetCurSel(index);
    }

    // ѡ��һ���ַ��������б�Ҫ�������������ͼ�С�ѡ���µ��ַ���ʱ���б�򽫴���ǰѡ����ַ�����ɾ��ͻ����ʾ
    // ������������򷵻�ֵΪLB_ERR�����wParam����Ϊ-1����ʹû�з������󣬷���ֵҲΪLB_ERR
    // index = ָ����ѡ�ַ����Ĵ��㿪ʼ������������˲���Ϊ-1�����б������Ϊû��ѡ��
	int SetCurSel(int index) {
        return (int)Send(LB_SETCURSEL, index, 0);
	}

	// �ڶ�ѡ�б����ѡ��һ����Ŀ�����ڱ�Ҫʱ������Ŀ��������ͼ��
	// isSel = ָ���������ѡ������˲���Ϊ TRUE����ѡ��ͻ����ʾ����;���Ϊ FALSE����ɾ��ͻ����ʾ�����Ҳ���ѡ�����
	// index = ָ��Ҫ���õ���Ĵ��㿪ʼ������������˲���Ϊ -1������� wParam ��ֵ����ѡ������ӵ�����������ɾ����ѡ���ݣ����Ҳ��ᷢ������
	int SetSel(bool isSel, int index = -1)
	{
		return (int)Send(LB_SETSEL, isSel, index);
	}
	
	// ��ȡ�б������Ŀ�ĸ߶�
	// ����ֵ���б����ÿ����Ŀ�ĸ߶ȣ�������Ϊ��λ��������б�����LBS_OWNERDRAWVARIABLE��ʽ���򷵻�ֵ��wParam����ָ������Ŀ�ĸ߶ȡ�������������򷵻�ֵΪLB_ERR
	// index = �б����Ŀ�Ĵ��㿪ʼ�������������б�����LBS_OWNERDRAWVARIABLE��ʽʱ����ʹ�ô����������򣬱���Ϊ��
	int GetItemHeight(int index = 0) {
        return (int)Send(LB_GETITEMHEIGHT, index, 0);
	}

	// �����б������Ŀ�ĸ߶ȣ�������Ϊ��λ��������б�����LBS_OWNERDRAWVARIABLE��ʽ�������Ϣ������wParam����ָ������Ŀ�ĸ߶ȡ����򣬴���Ϣ�������б����������Ŀ�ĸ߶�
	// height = ָ����Ŀ�ĸ߶ȣ�������Ϊ��λ�������߶�Ϊ255����
    // index = ���б����ָ������Ŀ�Ĵ��㿪ʼ�������������б�����LBS_OWNERDRAWVARIABLE��ʽʱ����ʹ�ô˲��������򣬽�������Ϊ��
	int SetItemHeight(int height, int index = 0) {
        return (int)Send(LB_SETITEMHEIGHT, 0, height);
	}

	// ���б���л�ȡһ���ַ���
	wstr GetItemText(int index) {
        int len = (int)Send(LB_GETTEXTLEN, index, 0) + 1;
        wstr str(len);
        Send(LB_GETTEXT, index, (LPARAM)str.data());
		str.resize(wcslen(str.c_str()));
        return str;
	}

	// ȡ��ָ���б����Ŀ������Ӧ�ó������ֵ
	LPARAM GetItemNum(int index) {
        return GetItemParam(index);
	}
	
	// ȡ��ָ���б����Ŀ������Ӧ�ó������ֵ
	LPARAM GetItemParam(int index) {
        return (LPARAM)Send(LB_GETITEMDATA, index, 0);
	}

    // ������ָ���б����Ŀ������Ӧ�ó������ֵ
	bool SetItemNum(int index, LPARAM param) {
        return SetItemParam(index, param);
	}

    // ������ָ���б����Ŀ������Ӧ�ó������ֵ
    bool SetItemParam(int index, LPARAM param) {
        return Send(LB_SETITEMDATA, index, param) != LB_ERR;
	}

	// �����б����ָ�������Ŀ����
	// ppt = ��Ļ����
	// bAutoScroll = ������־������˲���ΪTRUE�����Ҹõ�ֱ��λ���б����Ϸ����·�����ú������б�����һ�в�����-1�����򣬸ù��ܽ���������б��
	int GetPointItemIndex(LPPOINT ppt = 0, bool bAutoScroll = false) {
        POINT pt;
        if (ppt)pt = *ppt;
        else GetCursorPos(&pt);
        return LBItemFromPt(m_hWnd, pt, bAutoScroll);
	}

	// ��֤��ʾ
	inline bool SetTopIndex(int index) {
		return Send(LB_SETTOPINDEX, index, 0) != LB_ERR;
	}


};

