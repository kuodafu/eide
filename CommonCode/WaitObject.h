#pragma once

// ȡ����ʱ��, ������� GetTickCount64, ����� GetTickCount64, ������� GetTickCount
inline ULONGLONG GetTickCount32_64()
{
    typedef ULONGLONG(WINAPI* fnGetTickCount64)();
    static fnGetTickCount64 fun;
    static bool first;
    if ( !fun && !first )
    {
        first = true;
        HMODULE hModule = GetModuleHandleW(L"Kernel32.dll");
        if ( hModule )
            fun = (fnGetTickCount64)GetProcAddress(hModule, "GetTickCount64");
    }
    if ( fun )
        return fun();
    return (ULONGLONG)GetTickCount();
}

// �ȴ�1����������, ֻҪ��һ�����󴥷��¼��ͷ���, ���ش�������������������
// hObjects �ȴ�����Ķ�������
// ObjectCount = �ȴ��Ķ�������
// loopMsg = �Ƿ���Ϣѭ��, ��������߳���, ����Ϊ��, Ϊ��ʱ�ᴦ����Ϣ
// waitTimer = �ȴ�ʱ��, Ĭ��Ϊ INFINITE , ���޵ȴ�
inline DWORD WaitObj(HANDLE* hObjects, DWORD ObjectCount, bool loopMsg = false, DWORD waitTimer = INFINITE)
{
    if (ObjectCount == 0 || hObjects == 0 || *hObjects == 0) return 0;

    if (loopMsg)
    {
        ULONGLONG time = GetTickCount32_64();
        while (GetTickCount32_64() - time < (ULONGLONG)waitTimer)
        {
            DWORD ret = MsgWaitForMultipleObjects(ObjectCount, hObjects, FALSE, waitTimer, QS_ALLINPUT);
            if (ret >= WAIT_OBJECT_0 && ret < WAIT_OBJECT_0 + ObjectCount)
                return (DWORD)0;    // ��ʱ, ʧ��, �ɹ� ���˳�ѭ��
            if (ret == WAIT_TIMEOUT || ret == WAIT_FAILED)
                return (DWORD)-1;
            if (ret == WAIT_OBJECT_0 + ObjectCount)
            {
                MSG msg;
                if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
        }
    }
        else
        {
            if (ObjectCount == 1)
                return WaitForSingleObject(*hObjects, waitTimer);
            return WaitForMultipleObjects(ObjectCount, hObjects, FALSE, waitTimer);
        }
        return (DWORD)-1;
}
// �ȴ���������
// hObject �ȴ��Ķ�����
// loopMsg = �Ƿ���Ϣѭ��, ��������߳���, ����Ϊ��, Ϊ��ʱ�ᴦ����Ϣ
// waitTimer = �ȴ�ʱ��, Ĭ��Ϊ INFINITE , ���޵ȴ�
inline DWORD WaitObj(HANDLE hObject, bool loopMsg = false, DWORD waitTimer = INFINITE)
{
    return WaitObj(&hObject, 1, loopMsg, waitTimer);
}

#ifdef __cplusplus

class CWaitObj
{
    HANDLE m_hObj;
public:
    CWaitObj() :m_hObj(0)
    {

    }

    CWaitObj(HANDLE hObj) :m_hObj(hObj)
    {

    }

    bool wait(bool loopMsg = false, DWORD waitTimer = INFINITE)
    {
        if (!m_hObj)return false;
        return WaitObj(&m_hObj, 1, loopMsg, waitTimer) == 0;
    }

};

#endif