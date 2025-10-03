// 内部方法
#pragma once
#include <windows.h>
typedef void (WINAPI* pfn_EIDE_FreeMemory)(void* p);


// 从代码窗口句柄获取CWnd类, 成功返回类指针
LPBYTE CWnd_FromHandle(HWND hWnd);

// 把一个内存加入到自动释放列表里, 到时间后会调用释放回调函数, 此函数线程安全
// ptr = 需要自动释放的内存
// pfn = 释放内存时调用的地址, 会把被释放的内存传递过去
// freeTimer = 多久释放, 到这个时间后就调用 pfn来进行释放内存
void* EIDE_PushMemory(void* ptr, pfn_EIDE_FreeMemory pfn, DWORD freeTimer);


#define __FINDARG(_arr) _arr, sizeof(_arr) / sizeof(_arr[0])
// 查找易语言内存数据, 返回找到的内存地址, 如果没找到返回0
// findData = 要查找的数据
// findDataSize = 要查找的数据的长度
inline LPBYTE EIDE_FindAddress(const void* findData, int findDataSize)
{
    static LPBYTE pBegin = (LPBYTE)GetModuleHandleW(0);
    static LPBYTE pEnd = pBegin + 2097152;
    //static LPBYTE pEnd = pBegin + 1376256;
    for (LPBYTE p = (LPBYTE)pBegin; p != pEnd; p++)
    {
        if (memcmp(p, findData, findDataSize) == 0)
            return p;
    }
    return 0;
}

