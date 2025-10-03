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

	// 清空表项
	virtual inline void clear() {
        Send(LB_RESETCONTENT, 0, 0);
	}

	// 取列表项目数
	int GetCount() {
        return (int)Send(LB_GETCOUNT, 0, 0);
    }

	// 获取多选列表框中选定项的总
    int GetSelCount() {
        return (int)Send(LB_GETSELCOUNT, 0, 0);
    }

	// 设置表项数, 需要有LBS_NODATA风格, 而不是LBS_HASSTRINGS风格
	int SetItemCount(int count) {
        return (int)Send(LB_SETCOUNT, count, 0);
    }
    // 设置表项数, 需要有LBS_NODATA风格, 而不是LBS_HASSTRINGS风格
    int SetCount(int count)
    {
        return (int)Send(LB_SETCOUNT, count, 0);
	}

	// 将字符串或项目数据插入列表框中。与addString消息不同，LB_INSERTSTRING消息不会导致对具有LBS_SORT样式的列表进行排序
	// 返回值是插入字符串的位置的索引。如果发生错误，则返回值为LB_ERR。如果没有足够的空间来存储新字符串，则返回值为LB_ERRSPACE
	int insert(LPCWSTR text = 0, int index = -1) {
        return (int)Send(LB_INSERTSTRING, index, (LPARAM)text);
	}
	// 将字符串添加到列表框。如果列表框不具有LBS_SORT样式，则将字符串添加到列表的末尾。否则，将字符串插入列表，并对列表进行排序
    // 返回值是列表框中字符串的从零开始的索引。如果发生错误，则返回值为LB_ERR。如果没有足够的空间来存储新字符串，则返回值为LB_ERRSPACE
	int addString(LPCWSTR text = 0) {
        return (int)Send(LB_ADDSTRING, 0, (LPARAM)text);
	}

	// 删除列表框中的字符串
	// 返回值是列表中剩余字符串的计数。如果wParam参数指定的索引大于列表中的项目数，则返回值为LB_ERR
	// index = 要删除的字符串的从零开始的索引
	int deleteItem(int index) {
        return (int)Send(LB_DELETESTRING, index, 0);
	}

	// 在单选列表框中获取当前所选项目的索引
    // 在单选列表框中，返回值是当前所选项目的从零开始的索引。如果没有选择，则返回值为LB_ERR
    int GetIndedx() {
        return GetCurSel();
	}
    // 在单选列表框中获取当前所选项目的索引
	// 在单选列表框中，返回值是当前所选项目的从零开始的索引。如果没有选择，则返回值为LB_ERR
    int GetCurSel() {
        return (int)Send(LB_GETCURSEL, 0, 0);
	}
	// 获取项的选择状态
	// index = 项的从零开始的索引
    bool GetSel(int index) {
        return (bool)(Send(LB_GETSEL, index, 0) > 0);
    }

	// 选择一个字符串，如有必要，将其滚动到视图中。选择新的字符串时，列表框将从先前选择的字符串中删除突出显示
	// 如果发生错误，则返回值为LB_ERR。如果wParam参数为-1，即使没有发生错误，返回值也为LB_ERR
	// index = 指定所选字符串的从零开始的索引。如果此参数为-1，则列表框设置为没有选择
    int SetIndex(int index) {
        return SetCurSel(index);
    }

    // 选择一个字符串，如有必要，将其滚动到视图中。选择新的字符串时，列表框将从先前选择的字符串中删除突出显示
    // 如果发生错误，则返回值为LB_ERR。如果wParam参数为-1，即使没有发生错误，返回值也为LB_ERR
    // index = 指定所选字符串的从零开始的索引。如果此参数为-1，则列表框设置为没有选择
	int SetCurSel(int index) {
        return (int)Send(LB_SETCURSEL, index, 0);
	}

	// 在多选列表框中选择一个项目，并在必要时将该项目滚动到视图中
	// isSel = 指定如何设置选择。如果此参数为 TRUE，则选择并突出显示该项;如果为 FALSE，则删除突出显示，并且不再选择该项
	// index = 指定要设置的项的从零开始的索引。如果此参数为 -1，则根据 wParam 的值将所选内容添加到所有项或从中删除所选内容，并且不会发生滚动
	int SetSel(bool isSel, int index = -1)
	{
		return (int)Send(LB_SETSEL, isSel, index);
	}
	
	// 获取列表框中项目的高度
	// 返回值是列表框中每个项目的高度（以像素为单位）。如果列表框具有LBS_OWNERDRAWVARIABLE样式，则返回值是wParam参数指定的项目的高度。如果发生错误，则返回值为LB_ERR
	// index = 列表框项目的从零开始的索引。仅当列表框具有LBS_OWNERDRAWVARIABLE样式时，才使用此索引。否则，必须为零
	int GetItemHeight(int index = 0) {
        return (int)Send(LB_GETITEMHEIGHT, index, 0);
	}

	// 设置列表框中项目的高度（以像素为单位）。如果列表框具有LBS_OWNERDRAWVARIABLE样式，则此消息将设置wParam参数指定的项目的高度。否则，此消息将设置列表框中所有项目的高度
	// height = 指定项目的高度（以像素为单位）。最大高度为255像素
    // index = 在列表框中指定该项目的从零开始的索引。仅当列表框具有LBS_OWNERDRAWVARIABLE样式时，才使用此参数。否则，将其设置为零
	int SetItemHeight(int height, int index = 0) {
        return (int)Send(LB_SETITEMHEIGHT, 0, height);
	}

	// 从列表框中获取一个字符串
	wstr GetItemText(int index) {
        int len = (int)Send(LB_GETTEXTLEN, index, 0) + 1;
        wstr str(len);
        Send(LB_GETTEXT, index, (LPARAM)str.data());
		str.resize(wcslen(str.c_str()));
        return str;
	}

	// 取与指定列表框项目关联的应用程序定义的值
	LPARAM GetItemNum(int index) {
        return GetItemParam(index);
	}
	
	// 取与指定列表框项目关联的应用程序定义的值
	LPARAM GetItemParam(int index) {
        return (LPARAM)Send(LB_GETITEMDATA, index, 0);
	}

    // 设置与指定列表框项目关联的应用程序定义的值
	bool SetItemNum(int index, LPARAM param) {
        return SetItemParam(index, param);
	}

    // 设置与指定列表框项目关联的应用程序定义的值
    bool SetItemParam(int index, LPARAM param) {
        return Send(LB_SETITEMDATA, index, param) != LB_ERR;
	}

	// 检索列表框中指定点的项目索引
	// ppt = 屏幕坐标
	// bAutoScroll = 滚动标志。如果此参数为TRUE，并且该点直接位于列表框的上方或下方，则该函数将列表框滚动一行并返回-1。否则，该功能将不会滚动列表框
	int GetPointItemIndex(LPPOINT ppt = 0, bool bAutoScroll = false) {
        POINT pt;
        if (ppt)pt = *ppt;
        else GetCursorPos(&pt);
        return LBItemFromPt(m_hWnd, pt, bAutoScroll);
	}

	// 保证显示
	inline bool SetTopIndex(int index) {
		return Send(LB_SETTOPINDEX, index, 0) != LB_ERR;
	}


};

