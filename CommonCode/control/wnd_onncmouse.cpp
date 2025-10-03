#include "wnd_header.h"

LRESULT WndProc_OnNcMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	switch (message)
	{
	case WM_NCMOUSEMOVE:
	{
		LPSYSBTN sysBtn = (LPSYSBTN)pData->param;
		if (sysBtn->index != -1)
		{
			sysBtn->index = -1;
			sysBtn->state = STATE::NORMAL;
			InvalidateRect(pData->hObj, &sysBtn->rcBtn, 0);
		}
		break;
	}
	case WM_NCLBUTTONDBLCLK:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCMOUSEHOVER:
	case WM_NCMOUSELEAVE:
		break;
	default:
		return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
	}
	return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
}
