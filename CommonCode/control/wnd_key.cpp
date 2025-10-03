#include "wnd_header.h"

LRESULT WndProc_OnKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{

	return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
}
