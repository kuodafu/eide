#pragma once
#include <control/WndBase.h>

class Ctips : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return TOOLTIPS_CLASSW; };
public:
    Ctips() :CControlBase() { ; }
    virtual ~Ctips() { ; }
    virtual inline LPCWSTR className() { return CLASS_TIPS; };

public:
    virtual inline HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0)
    {
        return CWndBase::create(dwExStyle | WS_EX_TOPMOST, 0, dwStyle,
            x, y, nWidth, nHeight, hWndParent, ID, lpParam);
    }
    inline HWND create(DWORD style, DWORD exStyle)
    {
        if (!style)style = WS_VISIBLE;
        return create(exStyle | WS_EX_TOPMOST, 0, style, 0, 0, 0, 0, 0, NULL, this);
    }
    inline bool Insert(HWND hWnd, LPCWSTR text, bool bottom = false, bool b = false)
    {
        TTTOOLINFOW ti = { 0 };
        ti.cbSize = sizeof(ti);
        ti.lpszText = (LPWSTR)text;
        ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND | (bottom ? TTF_CENTERTIP : 0) | (b ? (TTF_TRACK | TTF_ABSOLUTE) : 0);
        ti.hwnd = hWnd;
        ti.uId = (UINT_PTR)hWnd;
        return Send(TTM_ADDTOOLW, 0, (LPARAM)&ti) != 0;
    }
    inline INT_PTR SetTipsText(HWND hWnd, LPCWSTR text)
    {
        wstr str(1100);
        if (text)
        {
            size_t len = wcslen(text);
            if (len > 1024)
            {
                str.assign(text, 1000);
                str.append(L"......省略了").append(len - 1000).append(L"个字符未显示");
                text = str.c_str();
            }
        }
        TTTOOLINFOW ti = { 0 };
        ti.cbSize = sizeof(ti);
        ti.lpszText = (LPWSTR)text;
        ti.uFlags = TTF_IDISHWND;
        ti.hwnd = hWnd;
        ti.uId = (UINT_PTR)hWnd;
        return Send(TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
    }
    inline INT_PTR Delete(HWND hWnd)
    {
        TTTOOLINFOW ti = { 0 };
        ti.cbSize = sizeof(ti);
        ti.uFlags = TTF_IDISHWND;
        ti.hwnd = hWnd;
        ti.uId = (UINT_PTR)hWnd;
        return Send(TTM_DELTOOLW, 0, (LPARAM)&ti);
    }
    inline INT_PTR SetTipsTitle(LPCWSTR text, int icon)
    {
        return Send(TTM_SETTITLEW, (WPARAM)icon, (LPARAM)text) != 0;
    }
    inline wstr GetTipsText(HWND hWnd)
    {
        TTTOOLINFOW ti = { 0 };
        ti.cbSize = sizeof(ti);
        ti.uFlags = TTF_IDISHWND;
        ti.hwnd = hWnd;
        ti.uId = (UINT_PTR)hWnd;
        wstr str(520);
        ti.lpszText = str.data();
        Send(TTM_GETTOOLINFOW, 0, (LPARAM)&ti);
        return str;
    }
    inline INT_PTR SetSleep(int type, int time)
    {
        return Send(TTM_SETDELAYTIME, type, time);
    }
    inline INT_PTR GetSleep(int type)
    {
        return Send(TTM_GETDELAYTIME, type, 0);
    }
    inline INT_PTR open(bool isOpen)
    {
        return Send(TTM_ACTIVATE, isOpen, 0);
    }
#ifndef TTM_POPUP
#define TTM_POPUP (WM_USER + 34)
#endif
    inline INT_PTR pop()
    {
        return Send(TTM_POPUP, 0, 0);
    }
    inline INT_PTR hide()
    {
        return Send(TTM_POP, 0, 0);
    }
    inline INT_PTR SetPos(POINT pt)
    {
        return Send(TTM_TRACKPOSITION, 0, MAKELONG(pt.x, pt.y));
    }
    inline INT_PTR Track(HWND hWnd, bool isCtivate)
    {
        TTTOOLINFOW ti = { 0 };
        ti.cbSize = sizeof(ti);
        ti.hwnd = hWnd;
        ti.uId = (UINT_PTR)hWnd;
        return Send(TTM_TRACKACTIVATE, isCtivate, (LPARAM)&ti);
    }

};

