#pragma once
#include <windows.h>

// 加载查看ext窗口, 如果 isLoopMsg 为true, 则窗口销毁后才返回, 并且返回值无意义, 否则返回窗口句柄
HWND ExtView_LoadWindow(HINSTANCE hInst, bool isLoopMsg = false, int x = CW_USEDEFAULT, int y = 0, int cx = CW_USEDEFAULT, int cy = 0);
void ExtView_LoadExt(LPCWSTR extFileName);
