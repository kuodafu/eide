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
    // 取根项目
    HTREEITEM GetRootItem();
    // 保证显示
    BOOL EnsureVisible(HTREEITEM hItem);
    // 扩展项目,b = 是否扩展
    BOOL Expand(HTREEITEM hItem, BOOL b);
    BOOL Expand(HTREEITEM hItem);
    // 加入项目
    HTREEITEM InsertItem(HTREEITEM hParent = TVGN_ROOT, LPCWSTR text = 0, int imgIndex = -1, int selIndex = -1, int num = 0, HTREEITEM InsertMode = 0);
    // 取项目矩形, isTitle = 是否值包含标题
    RECT GetItemRect(HTREEITEM hItem, BOOL isTitle = FALSE);
    BOOL DeleteItem(HTREEITEM hItem);
    // 清空
    BOOL clear();
    HTREEITEM GetParent(HTREEITEM hItem);
    // 是否有子项
    BOOL IsSubItem(HTREEITEM hItem);
    // 是否已扩展
    BOOL IsExpanded(HTREEITEM hItem);
    // 加粗, b=是否加粗
    BOOL bold(HTREEITEM hItem, BOOL b);
    // 是否已加粗
    BOOL bold(HTREEITEM hItem);
    HWND EditLabel(HTREEITEM hItem);
    // 是否选中
    BOOL isSel(HTREEITEM hItem);
    BOOL SetCheckBoxState(HTREEITEM hItem, int state);
    int GetCheckBoxState(HTREEITEM hItem);
    int SetIndex(HTREEITEM hItem);
    // 取现行选中项
    HTREEITEM GetIndex();
    int GetVisibleCount();
    // 取第一个可视项目
    HTREEITEM GetFirstVisible();
    // 取第一个项目
    HTREEITEM GetFirstItem();
    HTREEITEM GetNextVisible(HTREEITEM hItem);
    // 取所有选中项
    int GetSelAll(HTREEITEM** hItems);
    // 排序子项
    BOOL SortChildren(HTREEITEM hItem);
    // 排序子项回调
    BOOL SortChildrenCallback(HTREEITEM hItem, PFNTVCOMPARE fun, LPARAM lParam = 0);
    // 插入标志
    BOOL SetInsertMark(HTREEITEM hItem, int pos = 1);
    // 标志置颜色
    BOOL SetInsertMarkColor(COLORREF rgb);
    // 标志取颜色
    COLORREF GetInsertMarkColor();
    // 置图片组
    HIMAGELIST SetImageList(HIMAGELIST hImageList, int type = 0);
    // 取图片组
    HIMAGELIST GetImageList(int type = 0);
    // 创建拖放图片组
    HIMAGELIST CreateDragImage(HTREEITEM hItem);
    // 取坐标项目
    HTREEITEM GetPointItem(POINT pt, int* info = 0);
    // 取子项目数
    int GetSubCount(HTREEITEM hItem);
    // 取结束编辑文本
    wstr GetEditText();
    // 取编辑框句柄
    HWND GetEditControl();
    // 项目高度,参数小于等于0则返回项目高度,否则为设置
    int ItemHeight(int height = 0);
    // 缩进距离,参数小于等于0则返回缩进距离,否则为设置
    int Indent(int n = 0);
    // 文本色,参数小于0则返回文本颜色,否则为设置
    COLORREF TextColor(COLORREF clr = 0);
    // 背景色,参数小于0则返回背景颜色,否则为设置
    COLORREF BkColor(COLORREF clr = 0);
    // 连线色,参数小于0则返回连线颜色,否则为设置
    COLORREF LineColor(COLORREF clr = 0);
};

