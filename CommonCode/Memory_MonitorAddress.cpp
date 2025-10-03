#include "Memory_MonitorAddress.h"
#include "Memory_BeaEngine.h"
#include <unordered_map>
//#include "hook_detours/ApiHook.h"


static std::unordered_map<LPVOID, MONITORTYPE_DATA_STRUCT> m_map;               // 键名是需要监控的地址, 触发异常时异常地址是被写入的地址, 可能需要遍历才能确定是不是监控范围内
static std::unordered_map<LPVOID, MONITORTYPE_DATA_STRUCT*> m_map_breakPoint;   // 断点地址, 在设置内存断点时记录断点地址, 处理后删除
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

// 监控指定地址, 请确保地址的可用性, 返回是否成功
// pAddress 需要监控的地址, 当这个地址被读写时会通知 pCallback 回调函数
// size = 需要监控的尺寸, 地址在 pAddress + size 之间触发的读写操作都会触发回调函数
// type = 监控的类型, 目前仅支持 MONITORTYPE 枚举类型里的值, 当内存触发这个操作时调用回调函数
// pCallback = 接收被读写的回调函数
bool MonitorAddress(LPVOID pAddress, int size, MONITORTYPE type, PFN_MONITORADDRESS_CALLBACK pCallback, UINT64 param)
{
    if (!m_hException)
    {
        //m_hException = AddVectoredContinueHandler(-1, errorCallback);   // 继续处理程序
        m_hException = AddVectoredExceptionHandler(0, errorCallback);  // 异常处理程序
    }

    MEMORY_BASIC_INFORMATION mem;
    if (VirtualQuery(pAddress, &mem, sizeof(mem)) <= 0)
        return false;   // 查询这个地址失败直接返回
    //TODO 需要判断多个地址在同一个页面的情况
    //TODO 还有需要判断同一个地址使用不同回调的情况, 多个回调需要调用多次
    auto it = m_map.find(pAddress);
    if (MEM_FREE == mem.State)
    {
        if (it != m_map.end())
            m_map.erase(pAddress);
        return false;
    }

    DWORD Protect = 0;
    if (type == MONITORTYPE_READWRITE)
        Protect = PAGE_NOACCESS;    // 不可读写
    else if (type == MONITORTYPE_READ)
        Protect = PAGE_NOACCESS;    // 不可读
    else if (type == MONITORTYPE_WRITE)
        Protect = PAGE_READONLY;    // 不可写

    bool ret = false;
    MONITORTYPE_DATA_STRUCT& data = m_map[pAddress];
    if (!data.pPageBegin)   // 还没有初始化, 需要把内存设置为其他属性
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

// 取消监控指定地址
// pAddress = 调用 MonitorAddress 时填写的监控地址, 不使用时需要取消, 否则会继续调用回调地址, 如果回调地址是dll, 且dll已被释放, 那就会崩溃
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

// 从m_map里查找 pAddress, 如果这个地址是被监控的地址范围内, 放火true, 否则放false
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

// 设置内存断点
inline void write_0xcc(MONITORTYPE_DATA_STRUCT& data)
{
    MEMORY_BASIC_INFORMATION mem;
    if (VirtualQuery(data.pBreak, &mem, sizeof(mem)) <= 0)
        return;   // 查询这个地址失败直接返回

    data.pBreakBegin = (LPBYTE)mem.BaseAddress;
    data.breakSize = mem.RegionSize;

    DWORD old;

    // 把被监控的地址内存页改成可执行, 可读写可执行
    VirtualProtect(data.pPageBegin, data.pPageEnd - data.pPageBegin, PAGE_EXECUTE_READWRITE, &old);


    //HANDLE hThread = GetCurrentThread();
    ////获取线程环境块
    //CONTEXT ct = { 0 };
    //ct.ContextFlags = CONTEXT_CONTROL;
    //GetThreadContext(hThread, &ct);



    ////将TF标志位置置1
    //ct.EFlags |= (1 << 8); // TF 是第8位, 把第八位置为1

    ////设置线程环境块
    //SetThreadContext(hThread, &ct);
    //return;

    // 把断点的内存页改成可读写, 然后写入0xCC
    VirtualProtect(data.pBreakBegin, data.breakSize, PAGE_READWRITE, &old);
    data.oldOPCode = *data.pBreak;
    *data.pBreak = 0xcc;            // 记录断点字节后把字节改成0xCC断点
    VirtualProtect(data.pBreakBegin, data.breakSize, old, &old);    // 改回原来的属性
}

// 还原断点
inline void reset_0xcc(MONITORTYPE_DATA_STRUCT& data)
{
    DWORD old;

    //HANDLE hThread = GetCurrentThread();
    ////获取线程环境块
    //CONTEXT ct = { 0 };
    //ct.ContextFlags = CONTEXT_CONTROL;
    //GetThreadContext(hThread, &ct);



    ////将TF标志位置置1
    //ct.EFlags &= (~(1 << 8)); // TF 是第8位, 把第八位置为0

    ////设置线程环境块
    //SetThreadContext(hThread, &ct);
    if (1)
    {
        // 把断点的内存页改成可读写, 然后把0xCC改成原来的值
        VirtualProtect(data.pBreakBegin, data.breakSize, PAGE_READWRITE, &old);
        *data.pBreak = data.oldOPCode;
        data.oldOPCode = 0;             // 把0xCC改回原来的值
        VirtualProtect(data.pBreakBegin, data.breakSize, old, &old);    // 改回原来的属性
    }
    // 把被监控的地址内存页改成设置监控时的属性
    VirtualProtect(data.pPageBegin, data.pPageEnd - data.pPageBegin, data.newProtect, &old);
}

//__deplspec(naked) void _fun_write_memory()
//{
//
//}

LONG WINAPI errorCallback(LPEXCEPTION_POINTERS ExceptionInfo)
{
    // EXCEPTION_CONTINUE_EXECUTION;    // 将控制权返回到发生异常的点
    // EXCEPTION_CONTINUE_SEARCH;       // 程序继续执行
    LONG ret = EXCEPTION_CONTINUE_EXECUTION;

    switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:            // 线程尝试读取或写入它没有适当访问权限的虚拟地址。
    {
        // 数组的第一个元素包含一个读写标志，该标志指示导致访问冲突的操作类型。如果此值为零，则线程尝试读取无法访问的数据。如果此值为 1，则线程尝试写入无法访问的地址。
        // 如果此值为 8，则线程会导致用户模式数据执行保护 （DEP） 冲突。
        // 第二个数组元素指定不可访问数据的虚拟地址。
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
            if (item.isAddress && item.data->pCallback)   // 逐个调用回调
                item.data->pCallback(pErrorData, item.data->type, item.data->param);
        }
        MONITORTYPE_DATA_STRUCT& data = *arr[0].data;
        if (data.pPageBegin)
        {
            // 被监控的地址那个内存页其他地址被读写, 不需要调用回调, 需要跟回调一样的处理
            // 先把内存属性页改回原来的属性
            // 然后在下一行eip第一个字节改成0xCC
            // 然后继续执行, 等执行到0xCC的时候在断点断下里把0xCC还原
            LPBYTE pEip = (LPBYTE)ExceptionInfo->ExceptionRecord->ExceptionAddress;
#ifdef _WIN64
            int len = Disasm_BeaEngine((UINT64)pEip, true, 0, 0);
#else
            int len = Disasm_BeaEngine((UINT64)pEip, false, 0, 0);
#endif
            if (len != -1)
            {
                data.pBreak = pEip + len;   // 断点地址是当前异常地址的下一行代码
                m_map_breakPoint[data.pBreak] = &data;
                ExceptionInfo->ExceptionRecord->ExceptionAddress = data.pBreak;
#ifdef _WIN64
                ExceptionInfo->ContextRecord->Rip = (DWORD64)data.pBreak;
#else
                ExceptionInfo->ContextRecord->Eip = (DWORD)data.pBreak;
#endif

                //DetourTransactionBegin();               // 拦截开始
                //DetourUpdateThread(GetCurrentThread()); // 刷新当前线程
                //DetourAttach(&pOldFun, pNewFun);
                //DetourTransactionCommit();              // 拦截生效


                // 需要执行的代码, 把异常的那一句代码写入进来, 然后再跳转到这里继续执行
                //LPVOID pAddrExecut = VirtualAlloc(0, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
                //if (pAddrExecut)
                //{
                //    LPVOID pContinueAddress = 0;
                //    __asm
                //    {
                //        mov eax, test_break;
                //        mov pContinueAddress, eax;
                //    }
                //    const int opCodeSize = len + 5; // jmp XXXX 占用5个字节
                //    LPBYTE p = (LPBYTE)pAddrExecut;
                //    memcpy(p, pEip, len); p += len;
                //    *p++ = 0xe9;    // jmp, 需要计算位置, 需要jmp回来下面这一行位置
                //    const INT_PTR pFun = (INT_PTR)pContinueAddress - INT_PTR(pAddrExecut) - opCodeSize;
                //    memcpy(p, &pFun, 4);

                //    DWORD old = 0;
                //    // 把被监控的地址内存页改成可执行, 可读写可执行
                //    VirtualProtect(data.pPageBegin, data.pPageEnd - data.pPageBegin, PAGE_EXECUTE_READWRITE, &old);

                //    __asm
                //    {
                //        jmp pAddrExecut;
                //    }


                //test_break:
                //    VirtualProtect(data.pPageBegin, data.pPageEnd - data.pPageBegin, data.oldProtect, &old);
                //    VirtualFree(pAddrExecut, 0, MEM_RELEASE);

                //    // 我需要在这里执行一段机器指令, 有什么方式可以直接执行吗?
                //    // 我现在能想到的只有 申请一块可执行的内存, 把需要执行的指令拷贝进来, 然后jmp过去
                //    // 但是怎么jmp回来现在不会弄
                //    // c++应该是有可以直接执行机器指令的方法吧, 像直接内联汇编一样



                //}
                //write_0xcc(data);
                ret = EXCEPTION_CONTINUE_SEARCH;    // 程序继续执行
            }
        }

        break;
    }
    case EXCEPTION_BREAKPOINT:                  // 遇到断点。
    {
        auto it = m_map_breakPoint.find(ExceptionInfo->ExceptionRecord->ExceptionAddress);
        if (it != m_map_breakPoint.end())
        {
            MONITORTYPE_DATA_STRUCT* p = it->second;
            reset_0xcc(*p);
            m_map_breakPoint.erase(it);
            //ret = EXCEPTION_CONTINUE_SEARCH;    // 程序继续执行
        }
        break;
    }
    default:
        break;  // 默认处理是 将控制权返回到发生异常的点
    }

    return ret;
}
