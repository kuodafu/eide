#pragma once
#include <windows.h>

// ���ز鿴ext����, ��� isLoopMsg Ϊtrue, �򴰿����ٺ�ŷ���, ���ҷ���ֵ������, ���򷵻ش��ھ��
HWND ExtView_LoadWindow(HINSTANCE hInst, bool isLoopMsg = false, int x = CW_USEDEFAULT, int y = 0, int cx = CW_USEDEFAULT, int cy = 0);
void ExtView_LoadExt(LPCWSTR extFileName);
