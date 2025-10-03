#pragma once
#include "WndBase.h"
class CTab : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return WC_TABCONTROLW; }

public:
    CTab() :CControlBase() { ; }
    virtual ~CTab() { ; }
public:
    virtual inline LPCWSTR className() { return CLASS_TAB; };


public:
    inline int          DeleteAllItems()                    { return TabCtrl_DeleteAllItems(GetWindow()); }
    inline int          GetIndex()                          { return TabCtrl_GetCurSel(GetWindow()); }
    inline int          SetIndex(int index)                 { return TabCtrl_SetCurSel(GetWindow(), index); }
    inline int          GetCurSel()                         { return GetIndex(); }
    inline int          SetCurSel(int index)                { return SetIndex(index); }
    inline int          GetFocus()                          { return (int)(DWORD)(LONG_PTR)TabCtrl_GetCurFocus(GetWindow()); }
    inline int          SetFocus(int index)                 { return (int)(DWORD)(LONG_PTR)TabCtrl_SetCurFocus(GetWindow(), index); }
    inline DWORD        GetStyleEx()                        { return TabCtrl_GetExtendedStyle(GetWindow()); }
    inline DWORD        SetStyleEx(DWORD style)             { return TabCtrl_SetExtendedStyle(GetWindow(), style); }
    inline HIMAGELIST   GetImageList()                      { return TabCtrl_GetImageList(GetWindow()); }
    inline HIMAGELIST   SetImageList(HIMAGELIST list)       { return TabCtrl_SetImageList(GetWindow(), list); }
    inline int          GetItemCount()                      { return TabCtrl_GetItemCount(GetWindow()); }
    // 取项目矩形
    inline bool         GetItemRect(int index, PRECT prc)   { return TabCtrl_GetItemRect(GetWindow(), index, prc); }
    // 取当前行数
    inline bool         GetRowCount()                       { return TabCtrl_GetRowCount(GetWindow()); }
    inline HWND         GetToolTips()                       { return TabCtrl_GetToolTips(GetWindow()); }
    inline bool         DeleteItem(int index)               { return TabCtrl_DeleteItem(GetWindow(), index); }
    // 清除项目状态, isSelect 是否只清除选中的选项卡, 为false则清除全部
    inline void         DeselectAll(bool isSelect)          { TabCtrl_DeselectAll(GetWindow(), isSelect); }

    int     InsertItem(int index, const LPTCITEMW item)     { return TabCtrl_InsertItem(GetWindow(), index, item); }
    int     InsertItem(int index, LPCWSTR text = 0, LPARAM lParam = 0, int state = 0, int iImage = -1)
    {
        TCITEMW item = { 0 };
        if (text)
        {
            item.mask |= TCIF_TEXT;
            item.pszText = (LPWSTR)text;
        }
        if (lParam)
        {
            item.mask |= TCIF_PARAM;
            item.lParam = lParam;
        }
        if (state)
        {
            item.mask |= TCIF_STATE;
            item.iImage = iImage;
        }
        if (iImage != -1)
        {
            item.mask |= TCIF_IMAGE;
            item.dwState = state;
        }

        return TabCtrl_InsertItem(GetWindow(), index, &item);
    }
    bool    GetItem(int index, LPTCITEMW item)
    {
        return TabCtrl_GetItem(GetWindow(), index, item);
    }
    bool    SetItem(int index, const LPTCITEMW item)
    {
        return TabCtrl_SetItem(GetWindow(), index, item);
    }

    wstr    GetItemText(int index)
    {
        wstr buf(260);
        TCITEMW item = { 0 };
        item.mask = TCIF_TEXT;
        item.pszText = (LPWSTR)buf.data();
        item.cchTextMax = 260;
        TabCtrl_GetItem(GetWindow(), index, &item);
        buf.resize(wcslen(buf.c_str()));
        return buf;
    }
    int     GetState(int index)
    {
        TCITEMW item = { 0 };
        item.mask = TCIF_STATE;
        TabCtrl_GetItem(GetWindow(), index, &item);
        return item.dwState;
    }
    LPARAM  GetParam(int index)
    {
        TCITEMW item = { 0 };
        item.mask = TCIF_PARAM;
        TabCtrl_GetItem(GetWindow(), index, &item);
        return item.lParam;
    }
    int     GetImage(int index)
    {
        TCITEMW item = { 0 };
        item.mask = TCIF_IMAGE;
        TabCtrl_GetItem(GetWindow(), index, &item);
        return item.iImage;
    }

    bool    SetItemText(int index, LPCWSTR text)
    {
        TCITEMW item = { 0 };
        item.mask = TCIF_TEXT;
        item.pszText = (LPWSTR)text;
        bool ret = TabCtrl_SetItem(GetWindow(), index, &item);
        return ret;
    }
    bool    SetState(int index, int state)
    {
        TCITEMW item = { 0 };
        item.mask = TCIF_STATE;
        item.dwState = state;
        bool ret = TabCtrl_SetItem(GetWindow(), index, &item);
        return ret;
    }
    bool    SetParam(int index, LPARAM param)
    {
        TCITEMW item = { 0 };
        item.mask = TCIF_PARAM;
        item.lParam = param;
        bool ret = TabCtrl_SetItem(GetWindow(), index, &item);
        return ret;
    }
    bool    SetImage(int index, int iImage)
    {
        TCITEMW item = { 0 };
        item.mask = TCIF_IMAGE;
        item.iImage = iImage;
        bool ret = TabCtrl_SetItem(GetWindow(), index, &item);
        return ret;
    }

    // 取鼠标所在位置的项目, 返回项目索引, 没有在项目中返回-1
    int    HitTest(int x, int y, int* flags = 0)
    {
        TCHITTESTINFO ti = { 0 };
        ti.pt.x = x;
        ti.pt.y = y;

        int ret = TabCtrl_HitTest(GetWindow(), &ti);
        if ( flags )*flags = ti.flags;
        return ret;
    }
};

