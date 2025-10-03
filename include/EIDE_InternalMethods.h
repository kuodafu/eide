// �ڲ�����
#pragma once
#include <windows.h>
typedef void (WINAPI* pfn_EIDE_FreeMemory)(void* p);


// �Ӵ��봰�ھ����ȡCWnd��, �ɹ�������ָ��
LPBYTE CWnd_FromHandle(HWND hWnd);

// ��һ���ڴ���뵽�Զ��ͷ��б���, ��ʱ��������ͷŻص�����, �˺����̰߳�ȫ
// ptr = ��Ҫ�Զ��ͷŵ��ڴ�
// pfn = �ͷ��ڴ�ʱ���õĵ�ַ, ��ѱ��ͷŵ��ڴ洫�ݹ�ȥ
// freeTimer = ����ͷ�, �����ʱ���͵��� pfn�������ͷ��ڴ�
void* EIDE_PushMemory(void* ptr, pfn_EIDE_FreeMemory pfn, DWORD freeTimer);


#define __FINDARG(_arr) _arr, sizeof(_arr) / sizeof(_arr[0])
// �����������ڴ�����, �����ҵ����ڴ��ַ, ���û�ҵ�����0
// findData = Ҫ���ҵ�����
// findDataSize = Ҫ���ҵ����ݵĳ���
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

