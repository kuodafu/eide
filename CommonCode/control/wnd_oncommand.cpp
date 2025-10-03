#include "wnd_header.h"



LRESULT WndProc_OnCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	int id = GET_X_LPARAM(wParam);
	int code = GET_Y_LPARAM(wParam);

	return 0;
}