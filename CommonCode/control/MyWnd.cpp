#include "../pch.h"
#include "../spy.h"
#include "MyWnd.h"
#include "Btn.h"

CMyWnd::CMyWnd()
{
}


CMyWnd::~CMyWnd()
{
}
ATOM CMyWnd::RegClass(LPCTSTR szClass, WNDPROC proc)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInst;
	wcex.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	return RegisterClassEx(&wcex);
}

HWND CMyWnd::create(LPCWSTR szClass, LPCWSTR title)
{
	HWND hWnd = 0;
	int style = 0, styleEx = 0, cx = 0, cy = 0;
#ifdef _DEBUG
	style = WS_VISIBLE | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_GROUP | WS_THICKFRAME | WS_SYSMENU | WS_CLIPCHILDREN;
	styleEx = WS_EX_APPWINDOW;
#else
	style = WS_POPUP | WS_CLIPCHILDREN;
	styleEx = 0;
#endif
	style = WS_VISIBLE | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_GROUP | WS_THICKFRAME | WS_SYSMENU | WS_CLIPCHILDREN;
	styleEx = WS_EX_APPWINDOW;
	cx = GetSystemMetrics(SM_CXSCREEN);
	cy = GetSystemMetrics(SM_CYSCREEN);
	RECT rc = { 0 };
	rc.right = 800;
	rc.bottom = 600;
	rc.left = (cx - rc.right) / 2;
	rc.top = (cy - rc.bottom - 40) / 2;

	hWnd = CreateWindowExW(styleEx, szClass, title, style, rc.left, rc.top, rc.right, rc.bottom, 0, 0, g_hInst, this);

	if (!hWnd)
	{
		return FALSE;
	}
	m_hWnd = hWnd;
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return hWnd;
}

WPARAM CMyWnd::MsgLoop()
{
	MSG msg = { 0 };
	HACCEL hAccelTable = NULL;
	hAccelTable = LoadAccelerators(g_hInst, MAKEINTRESOURCE(IDC_SPY));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}
