// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <IDraw_Header.h>
#include <unordered_set>
#include <iDraw_const.h>
#include <iDraw_Version.h>
#include <tlhelp32.h>

static PLUGIN_INFO pluginInfo;

IDRAW_INFO* s_info;
DECLARE_HANDLE(HSTRHASH);   // 多定义一个类型来做键名, 这样只要用错了map, vs的环境马上就能提示出错

// 窗口句柄里存放消息表, MAP_MESSAGE
// 消息表里存放的是模块ID表, MAP_MODULE
// 查找方式
// 1. 先使用窗口句柄从 m_map_message 查找, 得到消息表
// 2. 在从消息表使用消息值查找, 得到模块表
// 3. 从模块表里使用guid查找, 得到回调数据
typedef std::map<HSTRHASH, REGISTER_MESSAGE_INFO> MAP_MODULE;                   // 模块ID表
typedef std::unordered_map<UINT, MAP_MODULE> MAP_MESSAGE;                       // 消息表
static std::unordered_map<HWND, MAP_MESSAGE>    m_map_message;                  // 已注册的消息回调表
static std::vector<LPSTR> s_hash_array;

static std::unordered_set<HMODULE>              m_set_module;                   // 记录所有模块句柄, 

static std::unordered_set<HWND>                 m_hWnds;


// 事件ID里存放模块表, 模块表里存放回调信息
// 事件查找方式
// 1. 先使用事件ID从 m_map_event 里查找, 得到模块表
// 2. 从模块表里使用guid查找, 得到回调数据
typedef std::unordered_map<HSTRHASH, REGISTER_EVENT_INFO> MAP_MODULE_EVT;       // 模块ID表
static std::map<UINT, MAP_MODULE_EVT>          m_map_event;                     // 已注册的事件回调表


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


inline static HSTRHASH _hash_fun(const char* pstr)
{
    if (!pstr || !pstr[0])return 0;
    for (auto it = s_hash_array.begin(); it != s_hash_array.end(); ++it)
    {
        const char* p = *it;
        if (p && strcmp(p, pstr) == 0)
            return (HSTRHASH)p;
    }
    size_t len = strlen(pstr) + 1;
    char* p = new char[len];
    memcpy(p, pstr, len);
    s_hash_array.push_back(p);
    return (HSTRHASH)p;
}


//计算Offset
ULONG RvaToOffset(PIMAGE_NT_HEADERS pNtHeader, ULONG Rva)
{
    //PE节
    PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)(pNtHeader + 1);

    ULONG sNum, i;
    //取得节表项数目
    sNum = pNtHeader->FileHeader.NumberOfSections;
    for (i = 0; i < sNum; i++)
    {
        //printf("PE 节名称: %s\n",p_section_header->Name);
        if ((pSection->VirtualAddress <= Rva) && Rva < (pSection->VirtualAddress + pSection->SizeOfRawData))
        {
            return Rva - pSection->VirtualAddress + pSection->PointerToRawData;
        }
        pSection++;
    }
    return 0;
}

// 获取这个地址是当前进程哪个模块里的地址, 用来调试注册消息/事件堆栈错误的问题
inline const char* GetDllNameFromAddress(LPCVOID pAddress)
{
    if (!pAddress)
        return 0;
    MEMORY_BASIC_INFORMATION mem;
    BOOL isOk = VirtualQuery(pAddress, &mem, sizeof(MEMORY_BASIC_INFORMATION));
    LPBYTE pCmp = (LPBYTE)mem.AllocationBase;
    if (!isOk) return 0;

    static char runFile[260];
    runFile[0] = 0;
    const int fileBuffer = sizeof(runFile) / sizeof(runFile[0]);
    GetModuleFileNameA((HMODULE)pCmp, runFile, fileBuffer);   // 先获取到程序运行路径
    if (runFile[0])
    {
        char* pos = strrchr(runFile, '\\');
        if (pos)
        {
            *pos++ = 0;
            return pos;
        }
        return runFile;
    }

    if (pCmp[0] != 'M' || pCmp[1] != 'Z')
        return 0;

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)pCmp;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pCmp + dosHeader->e_lfanew);

    ULONG offsetExportTable = RvaToOffset(nt, nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);    // 导出表偏移
    PIMAGE_EXPORT_DIRECTORY pExportTable = (PIMAGE_EXPORT_DIRECTORY)(pCmp + offsetExportTable);   // 导出表
    if (!offsetExportTable) return 0;

    char* dllName = (char*)(pCmp + RvaToOffset(nt, pExportTable->Name));
    return dllName;
}


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      返回需要调用的函数数量
// 
// 参数:
//      参数1: 窗口句柄
//      参数2: 注册的消息值
// 
// 返回值:
//      返回这个消息的回调信息结构指针, 如果没有找到则返回0
int iDraw_Event_GetMessageInfo(HWND hWnd, UINT message, LPREGISTER_MESSAGE_INFO* pArr, int bufSize)
{
    if (!hWnd) return 0;
    // 查找方式
    // 1. 先使用窗口句柄从 m_map_message 查找, 得到消息表
    // 2. 在从消息表使用消息值查找, 得到模块表
    // 3. 从模块表里使用模块ID查找, 得到回调数据

    auto itHwndTable = m_map_message.find(hWnd);
    if (itHwndTable == m_map_message.end()) return 0;   // 通过窗口句柄得到消息表, 如果没有, 就返回

    MAP_MESSAGE& msgTable   = itHwndTable->second;      // 这里是消息表
    auto itMsgTable         = msgTable.find(message);   // 通过消息从消息表里查找
    if (itMsgTable == msgTable.end()) return 0;         // 没有找到就返回

    MAP_MODULE& moduleTable = itMsgTable->second;       // 这里是模块表
    int count = (int)moduleTable.size();
    if (count == 0)return 0;
    int i = 0;
    for (auto it = moduleTable.begin(); it != moduleTable.end(); ++it)
    {
        if (i + 1 >= bufSize)break; // 最大不能超过缓冲区成员
        LPREGISTER_MESSAGE_INFO msg = &(it->second);
        if (msg->pfnCallbackAfter || msg->pfnCallbackBefore)
            pArr[i++] = msg;
    }
    
    return (int)i;    // 返回需要调用的函数数量
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      通过事件ID获取回调信息
// 
// 参数:
//      参数1: 事件ID
// 
// 返回值:
//      返回这个事件的回调信息结构指针, 如果没有找到则返回0
int iDraw_Event_GetEventInfo(UINT nEvent, LPREGISTER_EVENT_INFO* pArr, int bufSize)
{
    if (!nEvent) return 0;
    // 事件查找方式
    // 1. 先使用事件ID从 m_map_event 里查找, 得到模块表
    // 2. 从模块表里使用guid查找, 得到回调数据

    auto itModuleTable = m_map_event.find(nEvent);
    if (itModuleTable == m_map_event.end()) return 0;       // 通过窗口句柄得到消息表, 如果没有, 就返回

    MAP_MODULE_EVT& moduleTable = itModuleTable->second;    // 这里是模块表
    int count = (int)moduleTable.size();
    if (count == 0)return 0;
    int i = 0;
    for (auto it = moduleTable.begin(); it != moduleTable.end(); ++it)
    {
        pArr[i++] = &(it->second);
        if (i >= bufSize)break; // 最大不能超过缓冲区成员
    }
    return count;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      注册消息回调
// 
// 参数:
//      参数1: 模块ID, 插件
//      参数1: 注册哪个窗口的消息, 如果该窗口不存在, 则调用失败
//      参数2: 注册的消息值, 当窗口触发这个消息时会调用回调函数
//      参数3: 处理前的回调函数, 窗口处理这个消息前调用这个回调函数, 为0则取消挂接这个事件
//      参数4: 处理后的回调函数, 窗口已经处理这个消息后调用这个回调函数, 为0则取消挂接这个事件
//          参数3 和 参数4 的回调原型都是相同的
//              需要5个参数, 整数返回值, 参数数量必须匹配, 否则会导致堆栈不平衡从而导致程序崩溃
//              整数型 回调函数(窗口句柄, 消息值, 参数1, 参数2, 窗口数据指针)
//              与子类化接收消息函数一样
// 
// 返回值:
//      返回错误码, 0=成功
int iDraw_Register_Message(LPCSTR guid, HWND hWnd, UINT message, pfn_Register_Message pfnBefore, pfn_Register_Message pfnAfter)
{
    if (!guid || !guid[0]) return IDRAW_EVENT_ERROR::IDRAW_ERR_INVALIDGUID;
    if (!IsWindow(hWnd)) return IDRAW_EVENT_ERROR::IDRAW_ERR_INVALIDHWND;

    // 查找方式
    // 1. 先使用窗口句柄从 m_map_message 查找, 得到消息表
    // 2. 在从消息表使用消息值查找, 得到模块表
    // 3. 从模块表里使用模块ID查找, 得到回调数据
    HSTRHASH hash = _hash_fun(guid);
    //if (message == 78)
    //{
    //    LPCSTR s  = s_info->pfn_GetDllNameFromAddress(pfnAfter);
    //    //return 0;
    //}
    //if (message == 273)
    //{
    //    if (strcmp(guid, "{DC1F3633-3579-4153-833B-1A92DAE361BD}") == 0)
    //    {
    //        _str::dbg("%s, hWnd = 0x%08X, pfnBefore = 0x%08X, pfnAfter = 0x%08X\n", guid, hWnd, pfnBefore, pfnAfter);
    //        MEMORY_BASIC_INFORMATION mem;
    //        VirtualQuery(pfnBefore, &mem, sizeof(mem));
    //        mem.Type = 0;
    //    }
    //}

    MAP_MESSAGE& msgTable       = m_map_message[hWnd];      // 1. 先获取消息表
    MAP_MODULE& moduleTable     = msgTable[message];        // 2. 在获取模块表
    REGISTER_MESSAGE_INFO& data = moduleTable[hash];        // 3. 从模块表得到数据

    LPCSTR dllName = 0;

        dllName = s_info->pfn_GetDllNameFromAddress(guid);
    if (!dllName)
        dllName = s_info->pfn_GetDllNameFromAddress(pfnBefore);
    if (!dllName)
        dllName = s_info->pfn_GetDllNameFromAddress(pfnAfter);
    if (!dllName)
        dllName = "";
    if (s_info->logMsgEvt)
    {
        s_info->logMsgEvt("RegisterMessage, GUID = %s, message = %d, module = %s, pfnBefore = 0x%08X, pfnAfter = 0x%08X\r\n",
            guid, message, dllName, pfnBefore, pfnAfter);
    }

    // 记录回调信息, 
    data.dllName                = dllName;
    data.guid                   = (LPCSTR)hash;
    data.message                = message;
    data.hWnd                   = hWnd;
    data.pfnCallbackBefore      = pfnBefore;
    data.pfnCallbackAfter       = pfnAfter;
    return IDRAW_EVENT_ERROR::IDRAW_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消注册消息, 内部会删除这个窗口句柄相关的回调信息, 一般是在窗口销毁的时候调用, 目前暂不对外开放
// 
// 参数:
//      参数1: 窗口句柄
// 
// 返回值:
//      返回错误码, 0=成功
int iDraw_DetachRegister_Message(HWND hWnd)
{
    if (!hWnd) return IDRAW_EVENT_ERROR::IDRAW_ERR_INVALIDGUID;
    auto it = m_map_message.find(hWnd);
    if (it != m_map_message.end())
        m_map_message.erase(it);
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消指定插件的所有消息回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
int iDraw_UnRegister_Message(LPCSTR guid)
{
    if (!guid || !guid[0]) return IDRAW_EVENT_ERROR::IDRAW_ERR_INVALIDGUID;
    HSTRHASH hash = _hash_fun(guid);

    for (auto itMessage = m_map_message.begin(); itMessage != m_map_message.end(); ++itMessage) // 遍历所有消息表, 查找指定模块, 删除模块相关的回调事件
    {
        MAP_MESSAGE& msgTable = itMessage->second;
        for (auto itModule = msgTable.begin(); itModule != msgTable.end(); ++itModule)          // 遍历所有消息表, 从消息表里删除模块
        {
            MAP_MODULE& moduleTable = itModule->second;
            moduleTable.erase(hash);    // 模块表里需要删除这个模块
        }
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      取消指定插件的所有事件回调
// 
// 参数:
//      参数1: 插件唯一标识符
// 
// 返回值:
//      返回错误码, 0=成功
int iDraw_UnRegister_Event(LPCSTR guid)
{
    if (!guid || !guid[0]) return IDRAW_EVENT_ERROR::IDRAW_ERR_INVALIDGUID;
    HSTRHASH hash = _hash_fun(guid);

    for (auto itModule = m_map_event.begin(); itModule != m_map_event.end(); ++itModule) // 遍历所有事件表, 查找指定模块, 删除模块相关的回调事件
    {
        MAP_MODULE_EVT& moduleTable = itModule->second;
        moduleTable.erase(hash);    // 模块表里需要删除这个模块
    }
    return 0;
}


inline void CALLBACK _timer_proc_reg_msg(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    REGISTER_EVENT_INFO& data = (*((REGISTER_EVENT_INFO*)id));
    if (data.pfnCallbackBefore)
        data.dllName = s_info->pfn_GetDllNameFromAddress(data.pfnCallbackBefore);
    if (!data.dllName)
        data.dllName = s_info->pfn_GetDllNameFromAddress(data.pfnCallbackAfter);

    if (!data.dllName)
    {
        data.dllName = "";
    }
    else
    {
        KillTimer(hWnd, id);
    }
}


//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      注册事件回调
// 
// 参数:
//      参数1: 唯一标识符
//      参数2: 需要注册的事件, 会传递到回调函数里
//      参数3: 处理前的回调函数参数, 这个参数会在调用 参数4 时传递过去
//      参数4: 处理前的回调函数, 为0则取消挂接这个事件
//      参数5: 处理后的回调函数参数, 这个参数会在调用 参数6 时传递过去
//      参数6: 处理后的回调函数, 为0则取消挂接这个事件
//          参数4 和 参数6 的回调原型都是相同的
//              需要3个参数, 整数返回值, 参数数量必须匹配, 否则会导致堆栈不平衡从而导致程序崩溃
//              整数型 回调函数(事件类型, 回调参数, 事件指针)
// 
// 返回值:
//      返回错误码, 0=成功
int iDraw_Register_Event(LPCSTR guid, UINT nEvent, LONG_PTR paramBefore, pfn_Register_Event pfnBefore, LONG_PTR paramAfter, pfn_Register_Event pfnAfter)
{
    if (!guid || !guid[0]) return IDRAW_EVENT_ERROR::IDRAW_ERR_INVALIDGUID;

    MAP_MODULE_EVT& moduleTable = m_map_event[nEvent];
    // 记录回调信息
    HSTRHASH hash = _hash_fun(guid);
    REGISTER_EVENT_INFO& data = moduleTable[hash];


    LPCSTR dllName = 0;
    dllName = s_info->pfn_GetDllNameFromAddress(guid);
    if (pfnBefore)
        dllName = s_info->pfn_GetDllNameFromAddress(pfnBefore);
    if (!dllName)
        dllName = s_info->pfn_GetDllNameFromAddress(pfnAfter);
    if (!dllName)
    {
        dllName = "";
        //SetTimer(s_info->hEWnd, (ULONG_PTR)&data, 300, _timer_proc_reg_msg);
    }

    if (s_info->logMsgEvt)
    {
        s_info->logMsgEvt("RegisterEvent, GUID = %s, nEvent = %d, module = %s, pfnBefore = 0x%08X, pfnAfter = 0x%08X\r\n",
            guid, nEvent, dllName, pfnBefore, pfnAfter);
    }


    data.dllName                = dllName;
    data.guid                   = (LPCSTR)hash;
    data.nEvent                 = nEvent;
    data.paramBefore            = paramBefore;
    data.pfnCallbackBefore      = pfnBefore;
    data.paramAfter             = paramAfter;
    data.pfnCallbackAfter       = pfnAfter;
    return IDRAW_EVENT_ERROR::IDRAW_ERR_OK;
}





__declspec(naked) int __stdcall  call_3(LPVOID pfn, UINT nEvent, LONG_PTR param, LPVOID pArg)
{
    __asm
    {
        push ebp
        mov ebp, esp
        mov eax, dword ptr[ebp + 0x08]
        mov ebx, dword ptr[ebp + 0x04]
        mov dword ptr[ebp + 0x08], ebx
        pop ebp
        pop ebx
        jmp eax
    }
}

inline int _event_before(LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM], UINT nEvent, LPVOID pArgs, int* isReturn, bool isStop)
{
    int evtCount = iDraw_Event_GetEventInfo(nEvent, arr, EVENT_MAX_ITEM);
    int nRet = 0;
    for (int i = 0; i < evtCount; i++)
    {
        if (arr[i]->pfnCallbackBefore)
        {
            //nRet = call_3(arr[i]->pfnCallbackBefore, nEvent, arr[i]->paramBefore, pArgs);
            nRet = arr[i]->pfnCallbackBefore(nEvent, arr[i]->paramBefore, pArgs);
            if (nRet && *isReturn == 0) *isReturn = nRet;
            if (nRet && isStop)
            {
                if (s_info->logMsgEvt)
                {
                    LPCSTR dllName = arr[i]->dllName;
                    if (!dllName || !dllName[0])
                        dllName = s_info->pfn_GetDllNameFromAddress(arr[i]->pfnCallbackBefore);

                    LPCSTR guid = arr[i]->guid;
                    int _count = 0;
                    _str _dll(260);

                    for (i++; i < evtCount; i++)
                    {
                        if (arr[i]->pfnCallbackBefore)
                        {
                            _count++;
                            _dll.append("        ").append(s_info->pfn_GetDllNameFromAddress(arr[i]->pfnCallbackBefore)).append("\r\n");
                        }
                    }

                    s_info->logMsgEvt("插件 %s [GUID = %s]拦截了 %d 事件(处理前), 后面剩余 %d 个插件没有处理该事件\r\n%s\r\n",
                        dllName, guid, nEvent, _count, _dll.c_str());
                }

                return evtCount;    // 返回的是是否拦截, 1表示拦截其他插件的事件, 不往后处理
            }
            // 返回0则继续调用下一个事件
        }
    }
    return evtCount;
}

inline int _event_after(LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM], UINT nEvent, LPVOID pArgs, int evtCount, bool isStop)
{
    int nRet = 0;
    for (int i = 0; i < evtCount; i++)
    {
        if (arr[i]->pfnCallbackAfter)
        {
            //int ret = call_3(arr[i]->pfnCallbackAfter, nEvent, arr[i]->paramAfter, pArgs);
            int ret = arr[i]->pfnCallbackAfter(nEvent, arr[i]->paramAfter, pArgs);
            if (ret && nRet == 0) nRet = ret;
            if (ret && isStop)
            {
                if (s_info->logMsgEvt)
                {
                    LPCSTR dllName = arr[i]->dllName;
                    if (!dllName || !dllName[0])
                        dllName = s_info->pfn_GetDllNameFromAddress(arr[i]->pfnCallbackAfter);
                    LPCSTR guid = arr[i]->guid;

                    int _count = 0;
                    _str _dll(260);
                    for (i++; i < evtCount; i++)
                    {
                        if (arr[i]->pfnCallbackAfter)
                        {
                            _count++;
                            _dll.append("        ").append(s_info->pfn_GetDllNameFromAddress(arr[i]->pfnCallbackAfter)).append("\r\n");
                        }
                    }

                    s_info->logMsgEvt("插件 %s [GUID = %s]拦截了 %d 事件(处理后), 后面剩余 %d 个插件没有处理该事件\r\n%s\r\n",
                        dllName, guid, nEvent, _count, _dll.c_str());
                }
                return nRet;     // 返回是否拦截, 1表示拦截其他插件的事件, 不往后处理
            }
            // 返回0则继续调用下一个事件
        }
    }
    return 0;
}

// 与支持库交互的回调函数
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{

    return 0;
}

// 支持库调用, 会把窗口信息传递过来
extern "C" __declspec(dllexport) void* CALLBACK IDraw_Interface(IDRAW_INFO * info, IGetEWindow* eWnd, int reserved1, int reserved2)
{
    s_info = info;
    s_info->evt.pfnGetMessageInfo           = iDraw_Event_GetMessageInfo;
    s_info->evt.pfnGetEventInfo             = iDraw_Event_GetEventInfo;
    s_info->evt.pfnRegister_Message         = iDraw_Register_Message;
    s_info->evt.pfnRegister_Event           = iDraw_Register_Event;
    s_info->evt.pfnDetachRegister_Message   = iDraw_DetachRegister_Message;
    s_info->evt.pfnUnRegister_Message       = iDraw_UnRegister_Message;
    s_info->evt.pfnUnRegister_Event         = iDraw_UnRegister_Event;
    s_info->evt.pfn_Event_callback_before   = _event_before;
    s_info->evt.pfn_Event_callback_after    = _event_after;

    pluginInfo.cbSize           = sizeof(pluginInfo);
    pluginInfo.name             = "核心事件插件";
    pluginInfo.remarks          = "所有窗口消息注册, 事件注册都由这个插件完成";
    pluginInfo.version          = IDRAW_VERSIONA;
    pluginInfo.author           = "精易论坛";
    pluginInfo.Versionlimit     = 0;
    pluginInfo.Dependence       = 0;


    pluginInfo.guid                 = "{05A57CE4-A1A4-473D-0004-F90CAF05FE4A}";
    pluginInfo.pfnMenuExt_Command   = 0;
    pluginInfo.pfnCallback          = IDraw_NotifyCallback;

    return &pluginInfo;
}

// 插件被卸载, 需要把所有注册的事件/消息给释放
extern "C" __declspec(dllexport) void CALLBACK IDraw_UnInterface()
{

}
