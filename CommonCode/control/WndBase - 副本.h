#pragma once
#include <tstr.h>
#include "WndBaseTypedef.h"
#include <commctrl.h>



class CWndBase
{
public:
	CWndBase();
	~CWndBase();
	LPOBJSTRUCT m_hObj;
	static HINSTANCE m_hInst;	// 本程序实例句柄
	static HFONT m_hFont;		// 默认字体

protected:
	// 纯虚函数, 子类必须实现这个方法的功能
	virtual LRESULT CALLBACK WndProc(LPSUBCLASSSTRUCT pData, UINT message, WPARAM wParam, LPARAM lParam) = 0;

	// 子类应该调用这个来创建窗口, 这里有设置默认字体
	inline HWND __create(DWORD dwExStyle, LPCWSTR lpClassName,
		LPCWSTR lpWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight,
		HWND hWndParent, INT_PTR hMenu,
		HINSTANCE hInstance, LPVOID lpParam)
	{
		m_hObj->hWnd = CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight,
			hWndParent, (HMENU)hMenu, hInstance, lpParam);
		Send(WM_SETFONT, (WPARAM)m_hFont, 0);
		return m_hObj->hWnd;
	}
	inline HWND __create(const LPCREATESTRUCT cs)
	{
		return __create(cs->dwExStyle, cs->lpszClass, cs->lpszName, cs->style,
			cs->x, cs->y, cs->cx, cs->cy, cs->hwndParent,
			(INT_PTR)cs->hMenu, cs->hInstance, cs->lpCreateParams);
	}

protected:
	LPCWSTR RegClass(LPCWSTR oldClassName, LPCWSTR newClassName);
	bool SetUiState(STATE state, BOOL isRemove, BOOL isUpdate);
	bool QueryUiState(STATE state);
public:
	inline bool		Update()		{ return UpdateWindow(GetWindow()); }
	inline bool		IsWindow()		{ return ::IsWindow(GetWindow()); }
	inline bool		Destroy()		{ return DestroyWindow(GetWindow()); }
	inline HWND		GetWindow()		{ return m_hObj ? m_hObj->hWnd : 0; }
	inline bool		SetFocus()		{ return ::SetFocus(GetWindow()) != NULL; }
	inline bool		IsFocus()		{ return ::SetFocus(GetWindow()) == GetWindow(); }

	inline bool		Invalidate(const RECT* prc, bool bErase = FALSE)
					{ return InvalidateRect(GetWindow(), prc, bErase); }
	inline bool		Invalidate(HRGN hRgn, bool bErase = FALSE)
					{ return InvalidateRgn(GetWindow(), hRgn, bErase); }
	inline bool		Move(int x, int y, int cx, int cy, bool bRepaint = TRUE)
					{ return MoveWindow(GetWindow(), x, y, cx, cy, bRepaint); }
	inline bool		Move(const RECT& rc, bool bRepaint = TRUE)
					{ return MoveWindow(GetWindow(), rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, bRepaint); }
	inline bool		Move(const RECT* rc, bool bRepaint = TRUE)
					{ return MoveWindow(GetWindow(), rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, bRepaint); }
	inline bool		After(HWND after, UINT uFlags = 0)
					{ return SetWindowPos(GetWindow(), after, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | uFlags); }
	inline INT_PTR	Send(UINT message, WPARAM wParam, LPARAM lParam)
					{ return (INT_PTR)::SendMessageW(GetWindow(), message, wParam, lParam); }
	inline bool		Post(UINT message, WPARAM wParam, LPARAM lParam)
					{ return ::PostMessageW(GetWindow(), message, wParam, lParam); }
	inline bool		Visible(bool isVisible)
					{ return ::ShowWindow(GetWindow(), isVisible ? SW_SHOW : SW_HIDE); }
	inline bool		Visible()
					{ return ::IsWindowVisible(GetWindow()); }
	inline bool		Enabled(bool isEnable)
					{ return ::EnableWindow(GetWindow(), isEnable); }
	inline bool		Enabled()
					{ return ::IsWindowEnabled(GetWindow()); }
	inline LONG_PTR	Style(LONG_PTR style, bool isEx)
					{ return SetWindowLongPtrW(GetWindow(), isEx ? GWL_EXSTYLE : GWL_STYLE, style); }
	inline LONG_PTR	Style(bool isEx)
					{ return ::GetWindowLongPtrW(GetWindow(), isEx ? GWL_EXSTYLE : GWL_STYLE); }
	inline LONG_PTR	InsterStyle(LONG_PTR style, bool isEx)
					{ return ::SetWindowLongPtrW(GetWindow(), isEx ? GWL_EXSTYLE : GWL_STYLE, Style(isEx) | style); }
	inline LONG_PTR	RemoveStyle(LONG_PTR style, bool isEx)
					{ return ::SetWindowLongPtrW(GetWindow(), isEx ? GWL_EXSTYLE : GWL_STYLE, (Style(isEx) & ~style)); }
	inline bool		IsStyle(LONG_PTR style, bool isEx)
					{ return (Style(isEx) & style) == style; }
	inline bool		SetProp(LPCWSTR name, void* pData)
					{ return ::SetPropW(GetWindow(), name, (HANDLE)pData); }
	inline void*	GetProp(LPCWSTR name)
					{ return ::GetPropW(GetWindow(), name); }
	inline void*	RemoveProp(LPCWSTR name)
					{ return ::RemovePropW(GetWindow(), name); }
	inline LONG_PTR SetCursor(HCURSOR hCursor)
					{ return ::SetClassLongPtrW(GetWindow(), GCLP_HCURSOR, (LONG_PTR)hCursor); }

};



enum class state
{
	normal = 0,
	hot = 1,
	down = 2,
};




// 鼠标移动,算是进入,只设置状态为热点
#define CONST_CONTROL_MOUSEMOVE \
{\
	if (!QueryUiState(STATE::HOVER))\
	{\
		TRACKMOUSEEVENT tm = { 0 };\
		tm.cbSize = sizeof(tm);\
		tm.dwFlags = TME_LEAVE | TME_HOVER;\
		tm.dwHoverTime = 50;\
		tm.hwndTrack = pData->hWnd;\
		TrackMouseEvent(&tm);\
		SetUiState((STATE)(STATE::HOVER | STATE::HOT), false, true);\
	}\
}

// 鼠标离开,只设置状态为正常
#define CONST_CONTROL_MOUSELEAVE \
	if (QueryUiState(STATE::HOVER))\
		SetUiState((STATE)(STATE::HOVER | STATE::HOT), true, true)


// 鼠标左键按下,只设置状态为按下
#define CONST_CONTROL_LDOWN \
	SetUiState(STATE::DOWN, false, true)


// 鼠标左键放开,会判断当前鼠标位置是否在控件之中,在就设置为点燃状态,否则就设置为正常状态
#define CONST_CONTROL_LUP \
{\
	RECT rc;\
	POINT pt;\
	GetWindowRect(pData->hWnd, &rc);\
	GetCursorPos(&pt);\
	if (PtInRect(&rc, pt))\
		SetUiState(STATE::DOWN, true, true);\
	else\
		SetUiState(STATE::HOT, false, true);\
}

