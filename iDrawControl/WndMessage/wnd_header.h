#pragma once
#include <IDraw_Header.h>


const LONG SYSBTN_WIDTH = 30;
const LONG SYSBTN_HEIGHT = 30;

#define SC_MENU         MAKELONG(0, 0x1)
#define SC_SETTING      MAKELONG(0, 0x2)
#define SC_SKIN         MAKELONG(0, 0x4)
#define SC_HELP         MAKELONG(0, 0x8)

#define SYSBTN_CLOSE        0
#define SYSBTN_MAX          1
#define SYSBTN_RESTORE      2
#define SYSBTN_MIN          3
#define SYSBTN_MENU         4
#define SYSBTN_SETTING      5
#define SYSBTN_SKIN         6
#define SYSBTN_HELP         7






LRESULT _sysbtn_mousemove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT _sysbtn_lDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT _sysbtn_lUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT _sysbtn_rDwon(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
LRESULT _sysbtn_rUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);
BOOL sysBtnCommand(LPOBJSTRUCT pData, BOOL isRBtn, DWORD ID);

