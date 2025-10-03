#pragma once
#include "WndBase.h"
#include <tstr.h>
#include <comdef.h>
#include <GDIPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "GdiPlus.lib")


class CTree : public CWndBase
{
protected:
    virtual void _create_wnd(HWND hWnd);
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnMouseEvent(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, void* pUserData);
    virtual LRESULT OnPaint(HWND hWnd, void* pUserData);
public:
    CTree();
    virtual ~CTree();

public:
    HWND create(HWND hWndParent, int x, int y, int cx, int cy, int style, int treeStyleEx);
    HWND create(HWND hWndParent, int x, int y, int cx, int cy, int style, int treeStyleEx, int exStyle, LONG_PTR ID);

    LONG_PTR SetTreeStyle(LONG_PTR style);
    LONG_PTR GetTreeStyle();
    LONG_PTR InsertTreeStyle(LONG_PTR style);
    LONG_PTR RemoveTreeStyle(LONG_PTR style);
    BOOL IsTreeStyle(LONG_PTR style);

    LONG_PTR SetTreeStyleEx(LONG_PTR style);
    LONG_PTR GetTreeStyleEx();
    LONG_PTR InsertTreeStyleEx(LONG_PTR style);
    LONG_PTR RemoveTreeStyleEx(LONG_PTR style);
    BOOL IsTreeStyleEx(LONG_PTR style);

public:
    HTREEITEM GetHItem(int flag, HTREEITEM hItem);
    int enumHItem(HTREEITEM*& arr);
    int GetItemCount();
    int GetItemLevel(HTREEITEM hItem);
    BOOL GetItem(TVITEMEX& item);
    wstr GetItemText(HTREEITEM hItem);
    BOOL SetItemText(HTREEITEM hItem, LPTSTR text);
    int GetImg(HTREEITEM hItem);
    int SetImg(HTREEITEM hItem, int imgIndex);
    int GetImgState(HTREEITEM hItem);
    int SetImgState(HTREEITEM hItem, int stateIndex);
    int GetImgSel(HTREEITEM hItem);
    int SetImgSel(HTREEITEM hItem, int imgIndex);
    LONG_PTR GetItemNum(HTREEITEM hItem);
    int SetItemNum(HTREEITEM hItem, LONG_PTR num);
    // ȡ����Ŀ
    HTREEITEM GetRootItem();
    // ��֤��ʾ
    BOOL EnsureVisible(HTREEITEM hItem);
    // ��չ��Ŀ,b = �Ƿ���չ
    BOOL Expand(HTREEITEM hItem, BOOL b);
    BOOL Expand(HTREEITEM hItem);
    // ������Ŀ
    HTREEITEM InsertItem(HTREEITEM hParent = TVGN_ROOT, LPCWSTR text = 0, int imgIndex = -1, int selIndex = -1, int num = 0, HTREEITEM InsertMode = 0);
    // ȡ��Ŀ����, isTitle = �Ƿ�ֵ��������
    RECT GetItemRect(HTREEITEM hItem, BOOL isTitle = FALSE);
    BOOL DeleteItem(HTREEITEM hItem);
    // ���
    BOOL clear();
    HTREEITEM GetParent(HTREEITEM hItem);
    // �Ƿ�������
    BOOL IsSubItem(HTREEITEM hItem);
    // �Ƿ�����չ
    BOOL IsExpanded(HTREEITEM hItem);
    // �Ӵ�, b=�Ƿ�Ӵ�
    BOOL bold(HTREEITEM hItem, BOOL b);
    // �Ƿ��ѼӴ�
    BOOL bold(HTREEITEM hItem);
    HWND EditLabel(HTREEITEM hItem);
    // �Ƿ�ѡ��
    BOOL isSel(HTREEITEM hItem);
    BOOL SetCheckBoxState(HTREEITEM hItem, int state);
    int GetCheckBoxState(HTREEITEM hItem);
    int SetIndex(HTREEITEM hItem);
    // ȡ����ѡ����
    HTREEITEM GetIndex();
    int GetVisibleCount();
    // ȡ��һ��������Ŀ
    HTREEITEM GetFirstVisible();
    // ȡ��һ����Ŀ
    HTREEITEM GetFirstItem();
    HTREEITEM GetNextVisible(HTREEITEM hItem);
    // ȡ����ѡ����
    int GetSelAll(HTREEITEM** hItems);
    // ��������
    BOOL SortChildren(HTREEITEM hItem);
    // ��������ص�
    BOOL SortChildrenCallback(HTREEITEM hItem, PFNTVCOMPARE fun, LPARAM lParam = 0);
    // �����־
    BOOL SetInsertMark(HTREEITEM hItem, int pos = 1);
    // ��־����ɫ
    BOOL SetInsertMarkColor(COLORREF rgb);
    // ��־ȡ��ɫ
    COLORREF GetInsertMarkColor();
    // ��ͼƬ��
    HIMAGELIST SetImageList(HIMAGELIST hImageList, int type = 0);
    // ȡͼƬ��
    HIMAGELIST GetImageList(int type = 0);
    // �����Ϸ�ͼƬ��
    HIMAGELIST CreateDragImage(HTREEITEM hItem);
    // ȡ������Ŀ
    HTREEITEM GetPointItem(POINT pt, int* info = 0);
    // ȡ����Ŀ��
    int GetSubCount(HTREEITEM hItem);
    // ȡ�����༭�ı�
    wstr GetEditText();
    // ȡ�༭����
    HWND GetEditControl();
    // ��Ŀ�߶�,����С�ڵ���0�򷵻���Ŀ�߶�,����Ϊ����
    int ItemHeight(int height = 0);
    // ��������,����С�ڵ���0�򷵻���������,����Ϊ����
    int Indent(int n = 0);
    // �ı�ɫ,����С��0�򷵻��ı���ɫ,����Ϊ����
    COLORREF TextColor(COLORREF clr = 0);
    // ����ɫ,����С��0�򷵻ر�����ɫ,����Ϊ����
    COLORREF BkColor(COLORREF clr = 0);
    // ����ɫ,����С��0�򷵻�������ɫ,����Ϊ����
    COLORREF LineColor(COLORREF clr = 0);
};

