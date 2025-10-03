#pragma once

// 取运行时间, 如果存在 GetTickCount64, 则调用 GetTickCount64, 否则调用 GetTickCount
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

// 等待1个或多个对象, 只要有一个对象触发事件就返回, 返回触发对象在数组中索引
// hObjects 等待对象的对象数组
// ObjectCount = 等待的对象数量
// loopMsg = 是否消息循环, 如果是在线程内, 可以为假, 为真时会处理消息
// waitTimer = 等待时间, 默认为 INFINITE , 无限等待
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
                return (DWORD)0;    // 超时, 失败, 成功 都退出循环
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
// 等待单个对象
// hObject 等待的对象句柄
// loopMsg = 是否消息循环, 如果是在线程内, 可以为假, 为真时会处理消息
// waitTimer = 等待时间, 默认为 INFINITE , 无限等待
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