#pragma once
#include <windows.h>

enum MONITORTYPE
{
    MONITORTYPE_READ            = 0x0000,   // 读
    MONITORTYPE_WRITE           = 0x0001,   // 写
    MONITORTYPE_READWRITE       = 0x0002,   // 读写
};

typedef void(CALLBACK* PFN_MONITORADDRESS_CALLBACK)(LPVOID pAddress, MONITORTYPE type, UINT64 param);

struct MONITORTYPE_DATA_STRUCT
{
    PFN_MONITORADDRESS_CALLBACK pCallback;      // 回调函数
    UINT64                      param;          // 回调函数附加参数
    LPBYTE                      pBegin;         // 被监控的首地址
    LPBYTE                      pEnd;           // 被监控的结束地址
    MONITORTYPE                 type;           // 监控的类型, 目前只有监控读/写/读写
    LPBYTE                      pPageBegin;     // 被监控地址的页首地址
    LPBYTE                      pPageEnd;       // 被监控地址的页结束地址
    DWORD                       newProtect;     // 新的内存的保护模式
    DWORD                       oldProtect;     // 原来内存的保护模式
    BYTE                        oldOPCode;      // 0xCC原来的值
    LPBYTE                      pBreak;         // 设置断点的地址
    LPBYTE                      pBreakBegin;    // 断点起始地址
    DWORD                       breakSize;      // 断点内存页大小
};


// 监控指定地址, 请确保地址的可用性, 成功返回true, 失败返回false
// pAddress 需要监控的地址, 当这个地址被读写时会通知 pCallback 回调函数
// size = 需要监控的尺寸, 地址在 pAddress + size 之间触发的读写操作都会触发回调函数
// type = 监控的类型, 目前仅支持 MONITORTYPE 枚举类型里的值, 当内存触发这个操作时调用回调函数
// pCallback = 接收被读写的回调函数
bool MonitorAddress(LPVOID pAddress, int size, MONITORTYPE type, PFN_MONITORADDRESS_CALLBACK pCallback, UINT64 param = 0);

// 取消监控指定地址
// pAddress = 调用 MonitorAddress 时填写的监控地址, 不使用时需要取消, 否则会继续调用回调地址, 如果回调地址是dll, 且dll已被释放, 那就会崩溃
bool UnMonitorAddress(LPVOID pAddress);

