#include "wnd_header.h"

LRESULT WndProc_OnNcCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	LPCWSTR str = 0;
	switch (wParam)
	{
	case SC_MAXIMIZE:
		str = L"最大化";
		break;
	case SC_RESTORE:
		str = L"还原";
		break;
	case SC_MINIMIZE:
		str = L"最小化";
		break;
	case SC_CLOSE:
		str = L"关闭";
		break;
	case SC_HELP:
		str = L"帮助";
		break;
	case SC_MENU:
		str = L"菜单";
		break;
	case SC_SETTING:
		str = L"设置";
		break;
	case SC_SKIN:
		str = L"皮肤";
		break;
	default:
		return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
	}
	if (sysBtnCommand(pData, FALSE, (DWORD)wParam))
		return 0;
	
	wstr::dbg(L"系统按钮[%s]被按下, ID = 0x%X, %d\n", str, wParam, lParam);
	//return 0;
	return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
}

BOOL sysBtnCommand(LPOBJSTRUCT pData, BOOL isRBtn, DWORD ID)
{

    return 0;
}