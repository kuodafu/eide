#include "wnd_header.h"

LRESULT WndProc_OnStyleChanged(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
}
LRESULT WndProc_OnStyleChanging(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	if (wParam == GWL_STYLE)
	{
		LPSTYLESTRUCT pStyle = (LPSTYLESTRUCT)lParam;
		int s = pStyle->styleNew;
		s = s & (~(WS_DLGFRAME | WS_BORDER));
		s |= WS_MINIMIZEBOX;
		s |= WS_MAXIMIZEBOX;
		s |= WS_SYSMENU;
		s |= WS_THICKFRAME;
		pStyle->styleNew = s;
		return 0;
	}
	return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
}

