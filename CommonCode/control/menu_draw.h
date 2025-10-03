#pragma once
#include <windows.h>

#define DRAW_MENU_NAMESPACE_BEGIN namespace draw_menu{
#define DRAW_MENU_NAMESPACE_END }


typedef bool (CALLBACK* PFN_MENU_DRAWITEM)(HWND hWnd, LPDRAWITEMSTRUCT dw, LPCWSTR text);

void menu_draw_init();
void menu_draw_OnDrawItemText(HWND hWnd, PFN_MENU_DRAWITEM pfn);
HFONT menu_draw_GetFont(HFONT* hFontBold = 0);


