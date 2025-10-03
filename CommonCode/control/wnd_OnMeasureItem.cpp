#include "wnd_header.h"


LRESULT WndProc_OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
	LPMEASUREITEMSTRUCT item = (LPMEASUREITEMSTRUCT)lParam;
	if (item->CtlType != ODT_MENU)
		return WndProc_CallWindpwProc(hWnd, message, wParam, lParam, pData);
	
	if (item->CtlID == 0)	//MF_SEPARATOR
	{
        
	}
	item->itemWidth = 400;
	item->itemHeight = 30;
	return 1;
}