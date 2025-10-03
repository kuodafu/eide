#include "wnd_header.h"

LRESULT WndProc_OnNcCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	LPCWSTR str = 0;
	switch (wParam)
	{
	case SC_MAXIMIZE:
		str = L"���";
		break;
	case SC_RESTORE:
		str = L"��ԭ";
		break;
	case SC_MINIMIZE:
		str = L"��С��";
		break;
	case SC_CLOSE:
		str = L"�ر�";
		break;
	case SC_HELP:
		str = L"����";
		break;
	case SC_MENU:
		str = L"�˵�";
		break;
	case SC_SETTING:
		str = L"����";
		break;
	case SC_SKIN:
		str = L"Ƥ��";
		break;
	default:
		return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
	}
	if (sysBtnCommand(pData, FALSE, (DWORD)wParam))
		return 0;
	
	wstr::dbg(L"ϵͳ��ť[%s]������, ID = 0x%X, %d\n", str, wParam, lParam);
	//return 0;
	return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
}

BOOL sysBtnCommand(LPOBJSTRUCT pData, BOOL isRBtn, DWORD ID)
{

    return 0;
}