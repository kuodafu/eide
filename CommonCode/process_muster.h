#pragma once
#include <tstr.h>
#include <vector>
#include "tlhelp32.h"

typedef bool (CALLBACK* pfnEnumWindowCompare)(HWND hWnd, void* param);


HANDLE process_open(DWORD pid);
HANDLE process_open(HWND hWnd);
bool process_close(HANDLE hProcess);
DWORD process_create(LPCWSTR fileName, LPCWSTR command, LPPROCESS_INFORMATION proc);
DWORD process_create(LPCWSTR fileName, LPCWSTR command, bool wait, int showMode, LPPROCESS_INFORMATION proc);
wstr process_pid_getpath(DWORD pid);
wstr process_pid_getpath(HANDLE hProcess);
wstr process_pid_getpath(HWND hWnd);
DWORD process_enum(std::vector<wstr>* name, std::vector<DWORD>* pids, std::vector<PROCESSENTRY32W>* pes);
// 进程_取同名ID, 取相同名称进程, 成功返回进程id数量。
DWORD process_getid_equal(LPCWSTR name, std::vector<DWORD>* pids);

// 从进程名里获取所有同名进程ID, 不会清除原数组, 枚举到的进程ID都往后增加
DWORD process_getids(LPCWSTR name, std::vector<DWORD>* pids, bool isCase, std::vector<PROCESSENTRY32W>* pes = 0);
wstr process_getName(DWORD pid, LPPROCESSENTRY32W ref = 0);
// 进程是否有效
bool process_idisvalid(DWORD pid);
DWORD process_getid_formname(LPCWSTR name, bool isCase);
HWND process_getHwnd(LPCWSTR name);
HWND process_getHwnd(DWORD pid, LPCWSTR winTitle = 0, LPCWSTR windowClassName = 0, DWORD timer = 31536000, bool isVisible = false);
bool process_terminate(HANDLE hProc);
bool process_terminate(DWORD pid);
wstr process_GetCommandLine(DWORD pid);
wstr process_GetCommandLine(HANDLE hProc);
int RtlAdjustPrivilege(ULONG Privilege, bool Enable, bool CurrentThread, bool* Enabled);
bool ImproveProcPriv();

bool process_queryCommandLine(int port, HANDLE* phProc, DWORD* ppid, bool isClose, std::vector<int>* retPort, wstr* refCmd, HWND* retHwnd);



DWORD hwnd_getpid(HWND hWnd);
wstr window_getclass(HWND hWnd);
wstr window_gettitle(HWND hWnd);

// 枚举所有子窗口
// hWndParent = 需要枚举的父窗口, 为0则枚举整个桌面
// hWnds = 接收枚举结果数组
// isCase = 是否区分大小写, 判断类名/标题时使用, 如果标题/类名都为空, 则忽略该参数
// className = 需要判断的类名, 类名和标题有一个符合就返回
// titleName = 需要判断的标题, 类名和标题有一个符合就返回
// compare = 自定义判断函数, 传递此参数则忽略前面3个参数, 返回 true则加入数组
// param = 传递到回调函数里的值
int window_enumwindow(HWND hWndParent = 0, std::vector<HWND>* hWnds = 0, bool isCase = true, LPCWSTR className = 0, LPCWSTR titleName = 0, pfnEnumWindowCompare compare = 0, void* param = 0);


