#pragma once
#include "WndBase.h"
class CMyWnd : public CWndBase
{
public:
	CMyWnd();
	~CMyWnd();
public:
	ATOM RegClass(LPCTSTR szClass, WNDPROC proc);
	WPARAM MsgLoop();
	HWND create(LPCWSTR szClass, LPCWSTR title);


};

