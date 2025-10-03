#pragma once
#include <tstr.h>



class CMenu
{
private:
    HMENU m_hMenu;
    HWND m_hWnd;
public:
    // 默认构造
    CMenu() :m_hMenu(0), m_hWnd() { ; }

    CMenu(HWND hWnd) :m_hMenu(0), m_hWnd(hWnd) { ; }

    // 从一个菜单里构造对象, 增删改查都是从这个菜单下进行
    CMenu(HWND hWnd, HMENU hMenu) :m_hMenu(0), m_hWnd(0) { operator()(hWnd, hMenu); }
    CMenu& operator()(HWND hWnd, HMENU hMenu) {
        m_hMenu = hMenu;
        m_hWnd = hWnd;
        return *this;
    }
    ~CMenu() { ; }

public:
    
    // 创建当前程序所需要的菜单项
    virtual inline void create(HWND hWnd) {
        if (m_hMenu)
            DestroyMenu(m_hMenu);

        m_hWnd = hWnd;
        m_hMenu = CreatePopupMenu();
    }
    // 创建当前程序所需要的菜单项
    virtual inline void create(HWND hWnd, HMENU hMenu) {
        if (m_hMenu)
            DestroyMenu(m_hMenu);

        m_hWnd = hWnd;
        m_hMenu = hMenu;
    }
    inline HWND GetWindow() {
        return m_hWnd;
    }
    inline HMENU GetMenu() {
        return m_hMenu;
    }
    inline operator HWND() {
        return m_hWnd;
    }
    inline operator HMENU() {
        return m_hMenu;
    }

    // 弹出菜单,  为空则为鼠标当前位置
    virtual inline bool pop(int x = 0, int y = 0, HWND hWnd = 0) {
        POINT pt;
        GetCursorPos(&pt);
        if (!x) x = pt.x;
        if (!y) y = pt.y;
        return TrackPopupMenu(m_hMenu, TPM_LEFTALIGN, x, y, 0, hWnd ? hWnd : m_hWnd, 0);
    }

    // 取选中
    virtual inline bool GetCheck(UINT_PTR id, bool isIndex = false) {
        return GetState(id, isIndex) == MF_CHECKED;
    }

    // 设置选中, isSelect., 0=不选中, 1=选中, -1=取反
    virtual inline bool SetCheck(UINT_PTR id, int isSelect, bool isIndex = false) {
        int state = isSelect == 0 ? MF_UNCHECKED :
            (isSelect == 1 ? MF_CHECKED : (GetCheck(id, isIndex) ? MF_UNCHECKED : MF_CHECKED));
        return CheckMenuItem(m_hMenu, (DWORD)(LONG_PTR)id, state);
    }

    // 取标题
    virtual inline wstr GetTitle(UINT_PTR id, bool isIndex = false) {
        wstr text(MAX_PATH);
        MENUITEMINFOW mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STRING;
        mii.dwTypeData = text.data();
        mii.cch = MAX_PATH;
        GetMenuItemInfoW(m_hMenu, (DWORD)(LONG_PTR)id, isIndex, &mii);
        text.resize(wcslen(text.c_str()));
        return text;
    }

    // 置标题
    virtual inline bool SetTitle(UINT_PTR id, LPCWSTR text, bool isIndex = false) {
        MENUITEMINFOW mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STRING;
        mii.dwTypeData = (LPWSTR)text;
        mii.cch = lstrlenW(text);
        bool ret = SetMenuItemInfoW(m_hMenu, (UINT)(LONG_PTR)id, isIndex, &mii);
        return ret;
    }

    // 删除菜单项
    virtual inline bool del(UINT_PTR id, UINT uFlags = MF_BYCOMMAND) {
        return RemoveMenu(m_hMenu, (UINT)(LONG_PTR)id, uFlags);
    }

    // 添加菜单项
    virtual inline bool add(UINT_PTR id, LPCWSTR text, UINT uFlags = MF_STRING) {
        return AppendMenuW(m_hMenu, uFlags, (LONG_PTR)id, text);
    }

    // 添加弹出式菜单
    virtual inline bool add(HMENU id, LPCWSTR text, UINT uFlags = MF_POPUP) {
        return AppendMenuW(m_hMenu, uFlags, (UINT_PTR)id, text);
    }

    // 取子菜单数量
    virtual inline int GetSubItemCount() {
        int count = 0;
        while (1)
        {
            HMENU h = GetSubMenu(m_hMenu, count++);
            if (!h)
            {
                count--;
                break;
            }
        }
        return count;
    }

    // 取菜单项是否启用
    virtual inline bool IsEnabled(UINT_PTR id, bool isIndex = false) {
        MENUITEMINFOW mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STATE;

        GetMenuItemInfoW(m_hMenu, (DWORD)(LONG_PTR)id, isIndex, &mii);
        return (mii.fState & MFS_DISABLED) == 0;
    }

    // 取菜单项是否禁止
    virtual inline bool IsDisabled(UINT_PTR id, bool isIndex = false) {
        MENUITEMINFOW mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STATE;

        GetMenuItemInfoW(m_hMenu, (DWORD)(LONG_PTR)id, isIndex, &mii);
        return (mii.fState & MFS_DISABLED) == MFS_DISABLED;
    }

    // 设置菜单项是否启用, 0=禁止, 1=启用, -1=取反
    virtual inline bool enabled(UINT_PTR id, int isEnabled, bool isIndex = false) {
        MENUITEMINFOW mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STATE;
        switch (isEnabled)
        {
        case 0:
            mii.fState = MFS_DISABLED;
            break;
        case 1:
            mii.fState = MFS_ENABLED;
            break;
        case -1:
            mii.fState = IsEnabled(id, isIndex) ? MFS_DISABLED : MFS_ENABLED;
        default:
            return false;
        }
        return SetMenuItemInfoW(m_hMenu, (DWORD)(LONG_PTR)id, isIndex, &mii);
    }

    // 获取菜单条状态
    virtual inline UINT GetState(UINT_PTR id, bool isIndex = false) {
        MENUITEMINFOW mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STATE;
        GetMenuItemInfoW(m_hMenu, (DWORD)(LONG_PTR)id, isIndex, &mii);
        return mii.fState;
    }

    // 设置菜单条状态
    virtual inline bool SetState(UINT_PTR id, UINT state, bool isIndex = false) {
        MENUITEMINFOW mii = { 0 };
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_STATE;
        mii.fState = state;
        return SetMenuItemInfoW(m_hMenu, (DWORD)(LONG_PTR)id, isIndex, &mii);
    }

    // 增加状态
    virtual inline bool AddState(UINT_PTR id, UINT state, bool isIndex = false) {
        return SetState(id, GetState(id, isIndex) | state);
    }

    // 删除状态
    virtual inline bool DelState(UINT_PTR id, UINT state, bool isIndex = false) {
        return SetState(id, GetState(id, isIndex) & ~state);
    }


};

