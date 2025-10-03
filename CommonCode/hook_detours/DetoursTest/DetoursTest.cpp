// DetoursTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include "../detours/detours.h"
#include <stdio.h>
#include "../ApiHook.h"

apiHook g_apiHook;

static int(WINAPI* OLD_MessageBoxW)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType) = MessageBoxW;
int WINAPI NEW_MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
	//修改输入参数，调用原函数
	int ret = OLD_MessageBoxW(hWnd, L"test3", L"test3", uType);
	return ret;
}


void Hook()
{
	g_apiHook.hook((void**)&OLD_MessageBoxW, NEW_MessageBoxW);
}
void UnHook()
{
	g_apiHook.unHook();
}
int main()
{
	MessageBoxW(NULL, L"test", L"test", 0);
	Hook();
	MessageBoxW(NULL, L"test2", L"test2", 0);
	UnHook();

	return 0;
}