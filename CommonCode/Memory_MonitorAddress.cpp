#include "Memory_MonitorAddress.h"
#include "Memory_BeaEngine.h"
#include <unordered_map>
//#include "hook_detours/ApiHook.h"


static std::unordered_map<LPVOID, MONITORTYPE_DATA_STRUCT> m_map;               // ��������Ҫ��صĵ�ַ, �����쳣ʱ�쳣��ַ�Ǳ�д��ĵ�ַ, ������Ҫ��������ȷ���ǲ��Ǽ�ط�Χ��
static std::unordered_map<LPVOID, MONITORTYPE_DATA_STRUCT*> m_map_breakPoint;   // �ϵ��ַ, �������ڴ�ϵ�ʱ��¼�ϵ��ַ, �����ɾ��
static LPVOID m_hException;

LONG WINAPI errorCallback(LPEXCEPTION_POINTERS ExceptionInfo);

inline bool IsAddress(LPCVOID pAddr, LPCVOID pBegin, LPCVOID pEnd)
{
    return (pAddr >= pBegin && pAddr < pEnd);
};

inline MONITORTYPE_DATA_STRUCT* QueryAddress(LPVOID pAddress, int size)
{
    LPBYTE pBegin = (LPBYTE)pAddress;
    LPBYTE pEnd = pBegin + size;
    for (auto it = m_map.begin(); it != m_map.end(); ++it)
    {
        MONITORTYPE_DATA_STRUCT& item = it->second;


    }
    return 0;
}

// ���ָ����ַ, ��ȷ����ַ�Ŀ�����, �����Ƿ�ɹ�
// pAddress ��Ҫ��صĵ�ַ, �������ַ����дʱ��֪ͨ pCallback �ص�����
// size = ��Ҫ��صĳߴ�, ��ַ�� pAddress + size ֮�䴥���Ķ�д�������ᴥ���ص�����
// type = ��ص�����, Ŀǰ��֧�� MONITORTYPE ö���������ֵ, ���ڴ津���������ʱ���ûص�����
// pCallback = ���ձ���д�Ļص�����
bool MonitorAddress(LPVOID pAddress, int size, MONITORTYPE type, PFN_MONITORADDRESS_CALLBACK pCallback, UINT64 param)
{
    if (!m_hException)
    {
        //m_hException = AddVectoredContinueHandler(-1, errorCallback);   // �����������
        m_hException = AddVectoredExceptionHandler(0, errorCallback);  // �쳣�������
    }

    MEMORY_BASIC_INFORMATION mem;
    if (VirtualQuery(pAddress, &mem, sizeof(mem)) <= 0)
        return false;   // ��ѯ�����ַʧ��ֱ�ӷ���
    //TODO ��Ҫ�ж϶����ַ��ͬһ��ҳ������
    //TODO ������Ҫ�ж�ͬһ����ַʹ�ò�ͬ�ص������, ����ص���Ҫ���ö��
    auto it = m_map.find(pAddress);
    if (MEM_FREE == mem.State)
    {
        if (it != m_map.end())
            m_map.erase(pAddress);
        return false;
    }

    DWORD Protect = 0;
    if (type == MONITORTYPE_READWRITE)
        Protect = PAGE_NOACCESS;    // ���ɶ�д
    else if (type == MONITORTYPE_READ)
        Protect = PAGE_NOACCESS;    // ���ɶ�
    else if (type == MONITORTYPE_WRITE)
        Protect = PAGE_READONLY;    // ����д

    bool ret = false;
    MONITORTYPE_DATA_STRUCT& data = m_map[pAddress];
    if (!data.pPageBegin)   // ��û�г�ʼ��, ��Ҫ���ڴ�����Ϊ��������
    {
        data.pPageBegin = (LPBYTE)mem.BaseAddress;
        data.pPageEnd = data.pPageBegin + mem.RegionSize;
    }
    data.pCallback  = pCallback;
    data.param      = param;
    data.pBegin     = (LPBYTE)pAddress;
    data.pEnd       = data.pBegin + size;
    data.newProtect = Protect;
    data.type       = type;

    if (mem.Protect != Protect)
        ret = VirtualProtect(data.pPageBegin, mem.RegionSize, Protect, &data.oldProtect);

    return ret;
}

// ȡ�����ָ����ַ
// pAddress = ���� MonitorAddress ʱ��д�ļ�ص�ַ, ��ʹ��ʱ��Ҫȡ��, �����������ûص���ַ, ����ص���ַ��dll, ��dll�ѱ��ͷ�, �Ǿͻ����
bool UnMonitorAddress(LPVOID pAddress)
{
    auto it = m_map.find(pAddress);
    if (it == m_map.end())
        return false;
    m_map.erase(pAddress);
    return true;
}

struct MAPDATA_ARRAY
{
    bool isAddress;
    MONITORTYPE_DATA_STRUCT* data;
};

// ��m_map����� pAddress, ��������ַ�Ǳ���صĵ�ַ��Χ��, �Ż�true, �����false
inline int GetMapData(LPBYTE pAddress, std::vector<MAPDATA_ARRAY>& arr)
{
    auto it = m_map.find(pAddress);
    auto pfn_push = [](MONITORTYPE_DATA_STRUCT* pMapData, std::vector<MAPDATA_ARRAY>& arr, LPBYTE pAddress) -> int
    {
        MAPDATA_ARRAY data = { 0 };
        data.data = pMapData;
        data.isAddress = IsAddress(pAddress, data.data->pBegin, data.data->pEnd);
        arr.push_back(data);
        return (int)(LONG_PTR)arr.size();
    };

    if (it != m_map.end())
    {
        return pfn_push(&it->second, arr, pAddress);
    }


    for (it = m_map.begin(); it != m_map.end(); ++it)
    {
        MONITORTYPE_DATA_STRUCT& item = it->second;
        if (pAddress >= item.pPageBegin && pAddress < item.pPageEnd)
        {
            pfn_push(&it->second, arr, pAddress);
            break;
        }
    }
    return (int)(LONG_PTR)arr.size();
}

// �����ڴ�ϵ�
inline void write_0xcc(MONITORTYPE_DATA_STRUCT& data)
{
    MEMORY_BASIC_INFORMATION mem;
    if (VirtualQuery(data.pBreak, &mem, sizeof(mem)) <= 0)
        return;   // ��ѯ�����ַʧ��ֱ�ӷ���

    data.pBreakBegin = (LPBYTE)mem.BaseAddress;
    data.breakSize = mem.RegionSize;

    DWORD old;

    // �ѱ���صĵ�ַ�ڴ�ҳ�ĳɿ�ִ��, �ɶ�д��ִ��
    VirtualProtect(data.pPageBegin, data.pPageEnd - data.pPageBegin, PAGE_EXECUTE_READWRITE, &old);


    //HANDLE hThread = GetCurrentThread();
    ////��ȡ�̻߳�����
    //CONTEXT ct = { 0 };
    //ct.ContextFlags = CONTEXT_CONTROL;
    //GetThreadContext(hThread, &ct);



    ////��TF��־λ����1
    //ct.EFlags |= (1 << 8); // TF �ǵ�8λ, �ѵڰ�λ��Ϊ1

    ////�����̻߳�����
    //SetThreadContext(hThread, &ct);
    //return;

    // �Ѷϵ���ڴ�ҳ�ĳɿɶ�д, Ȼ��д��0xCC
    VirtualProtect(data.pBreakBegin, data.breakSize, PAGE_READWRITE, &old);
    data.oldOPCode = *data.pBreak;
    *data.pBreak = 0xcc;            // ��¼�ϵ��ֽں���ֽڸĳ�0xCC�ϵ�
    VirtualProtect(data.pBreakBegin, data.breakSize, old, &old);    // �Ļ�ԭ��������
}

// ��ԭ�ϵ�
inline void reset_0xcc(MONITORTYPE_DATA_STRUCT& data)
{
    DWORD old;

    //HANDLE hThread = GetCurrentThread();
    ////��ȡ�̻߳�����
    //CONTEXT ct = { 0 };
    //ct.ContextFlags = CONTEXT_CONTROL;
    //GetThreadContext(hThread, &ct);



    ////��TF��־λ����1
    //ct.EFlags &= (~(1 << 8)); // TF �ǵ�8λ, �ѵڰ�λ��Ϊ0

    ////�����̻߳�����
    //SetThreadContext(hThread, &ct);
    if (1)
    {
        // �Ѷϵ���ڴ�ҳ�ĳɿɶ�д, Ȼ���0xCC�ĳ�ԭ����ֵ
        VirtualProtect(data.pBreakBegin, data.breakSize, PAGE_READWRITE, &old);
        *data.pBreak = data.oldOPCode;
        data.oldOPCode = 0;             // ��0xCC�Ļ�ԭ����ֵ
        VirtualProtect(data.pBreakBegin, data.breakSize, old, &old);    // �Ļ�ԭ��������
    }
    // �ѱ���صĵ�ַ�ڴ�ҳ�ĳ����ü��ʱ������
    VirtualProtect(data.pPageBegin, data.pPageEnd - data.pPageBegin, data.newProtect, &old);
}

//__deplspec(naked) void _fun_write_memory()
//{
//
//}

LONG WINAPI errorCallback(LPEXCEPTION_POINTERS ExceptionInfo)
{
    // EXCEPTION_CONTINUE_EXECUTION;    // ������Ȩ���ص������쳣�ĵ�
    // EXCEPTION_CONTINUE_SEARCH;       // �������ִ��
    LONG ret = EXCEPTION_CONTINUE_EXECUTION;

    switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:            // �̳߳��Զ�ȡ��д����û���ʵ�����Ȩ�޵������ַ��
    {
        // ����ĵ�һ��Ԫ�ذ���һ����д��־���ñ�־ָʾ���·��ʳ�ͻ�Ĳ������͡������ֵΪ�㣬���̳߳��Զ�ȡ�޷����ʵ����ݡ������ֵΪ 1�����̳߳���д���޷����ʵĵ�ַ��
        // �����ֵΪ 8�����̻߳ᵼ���û�ģʽ����ִ�б��� ��DEP�� ��ͻ��
        // �ڶ�������Ԫ��ָ�����ɷ������ݵ������ַ��
        if (ExceptionInfo->ExceptionRecord->ExceptionInformation[0] != 1)
            break;

        LPBYTE pErrorData = (LPBYTE)ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
        std::vector<MAPDATA_ARRAY> arr;
        const int arrLen = GetMapData(pErrorData, arr);
        if (arrLen == 0)
            break;

        for (auto it = arr.begin(); it != arr.end(); ++it)
        {
            MAPDATA_ARRAY& item = *it;
            if (item.isAddress && item.data->pCallback)   // ������ûص�
                item.data->pCallback(pErrorData, item.data->type, item.data->param);
        }
        MONITORTYPE_DATA_STRUCT& data = *arr[0].data;
        if (data.pPageBegin)
        {
            // ����صĵ�ַ�Ǹ��ڴ�ҳ������ַ����д, ����Ҫ���ûص�, ��Ҫ���ص�һ���Ĵ���
            // �Ȱ��ڴ�����ҳ�Ļ�ԭ��������
            // Ȼ������һ��eip��һ���ֽڸĳ�0xCC
            // Ȼ�����ִ��, ��ִ�е�0xCC��ʱ���ڶϵ�������0xCC��ԭ
            LPBYTE pEip = (LPBYTE)ExceptionInfo->ExceptionRecord->ExceptionAddress;
#ifdef _WIN64
            int len = Disasm_BeaEngine((UINT64)pEip, true, 0, 0);
#else
            int len = Disasm_BeaEngine((UINT64)pEip, false, 0, 0);
#endif
            if (len != -1)
            {
                data.pBreak = pEip + len;   // �ϵ��ַ�ǵ�ǰ�쳣��ַ����һ�д���
                m_map_breakPoint[data.pBreak] = &data;
                ExceptionInfo->ExceptionRecord->ExceptionAddress = data.pBreak;
#ifdef _WIN64
                ExceptionInfo->ContextRecord->Rip = (DWORD64)data.pBreak;
#else
                ExceptionInfo->ContextRecord->Eip = (DWORD)data.pBreak;
#endif

                //DetourTransactionBegin();               // ���ؿ�ʼ
                //DetourUpdateThread(GetCurrentThread()); // ˢ�µ�ǰ�߳�
                //DetourAttach(&pOldFun, pNewFun);
                //DetourTransactionCommit();              // ������Ч


                // ��Ҫִ�еĴ���, ���쳣����һ�����д�����, Ȼ������ת���������ִ��
                //LPVOID pAddrExecut = VirtualAlloc(0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
                //if (pAddrExecut)
                //{
                //    LPVOID pContinueAddress = 0;
                //    __asm
                //    {
                //        mov eax, test_break;
                //        mov pContinueAddress, eax;
                //    }
                //    const int opCodeSize = len + 5; // jmp XXXX ռ��5���ֽ�
                //    LPBYTE p = (LPBYTE)pAddrExecut;
                //    memcpy(p, pEip, len); p += len;
                //    *p++ = 0xe9;    // jmp, ��Ҫ����λ��, ��Ҫjmp����������һ��λ��
                //    const INT_PTR pFun = (INT_PTR)pContinueAddress - INT_PTR(pAddrExecut) - opCodeSize;
                //    memcpy(p, &pFun, 4);

                //    DWORD old = 0;
                //    // �ѱ���صĵ�ַ�ڴ�ҳ�ĳɿ�ִ��, �ɶ�д��ִ��
                //    VirtualProtect(data.pPageBegin, data.pPageEnd - data.pPageBegin, PAGE_EXECUTE_READWRITE, &old);

                //    __asm
                //    {
                //        jmp pAddrExecut;
                //    }


                //test_break:
                //    VirtualProtect(data.pPageBegin, data.pPageEnd - data.pPageBegin, data.oldProtect, &old);
                //    VirtualFree(pAddrExecut, 0, MEM_RELEASE);

                //    // ����Ҫ������ִ��һ�λ���ָ��, ��ʲô��ʽ����ֱ��ִ����?
                //    // ���������뵽��ֻ�� ����һ���ִ�е��ڴ�, ����Ҫִ�е�ָ�������, Ȼ��jmp��ȥ
                //    // ������ôjmp�������ڲ���Ū
                //    // c++Ӧ�����п���ֱ��ִ�л���ָ��ķ�����, ��ֱ���������һ��



                //}
                //write_0xcc(data);
                ret = EXCEPTION_CONTINUE_SEARCH;    // �������ִ��
            }
        }

        break;
    }
    case EXCEPTION_BREAKPOINT:                  // �����ϵ㡣
    {
        auto it = m_map_breakPoint.find(ExceptionInfo->ExceptionRecord->ExceptionAddress);
        if (it != m_map_breakPoint.end())
        {
            MONITORTYPE_DATA_STRUCT* p = it->second;
            reset_0xcc(*p);
            m_map_breakPoint.erase(it);
            //ret = EXCEPTION_CONTINUE_SEARCH;    // �������ִ��
        }
        break;
    }
    default:
        break;  // Ĭ�ϴ����� ������Ȩ���ص������쳣�ĵ�
    }

    return ret;
}
