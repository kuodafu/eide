#pragma once

// ��ȡ������Դ�������
// hInst = ��Դ���ڵ�ģ����
// ID = ��Դ��id
// size = ������Դ����
// name = ��Դ����, ���� PNG
inline LPBYTE GetResource(HINSTANCE hInst, int ID, DWORD* size, LPCWSTR name)
{
    if (size)*size = 0;
    HRSRC hRsrc = ::FindResourceW(hInst, MAKEINTRESOURCEW(ID), name);
    if (!hRsrc) return NULL;

    //��ȡ��Դ����

    if (size)
        *size = SizeofResource(hInst, hRsrc);

    //��ָ������Դ���ص�����
    HGLOBAL hMem = LoadResource(hInst, hRsrc);
    if (hMem)
        return (LPBYTE)LockResource(hMem);
    return 0;
}

