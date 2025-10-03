#pragma once

// 获取程序资源里的数据
// hInst = 资源所在的模块句柄
// ID = 资源的id
// size = 接收资源长度
// name = 资源类型, 比如 PNG
inline LPBYTE GetResource(HINSTANCE hInst, int ID, DWORD* size, LPCWSTR name)
{
    if (size)*size = 0;
    HRSRC hRsrc = ::FindResourceW(hInst, MAKEINTRESOURCEW(ID), name);
    if (!hRsrc) return NULL;

    //获取资源长度

    if (size)
        *size = SizeofResource(hInst, hRsrc);

    //将指定的资源加载到变量
    HGLOBAL hMem = LoadResource(hInst, hRsrc);
    if (hMem)
        return (LPBYTE)LockResource(hMem);
    return 0;
}

