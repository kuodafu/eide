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
// ����_ȡͬ��ID, ȡ��ͬ���ƽ���, �ɹ����ؽ���id������
DWORD process_getid_equal(LPCWSTR name, std::vector<DWORD>* pids);

// �ӽ��������ȡ����ͬ������ID, �������ԭ����, ö�ٵ��Ľ���ID����������
DWORD process_getids(LPCWSTR name, std::vector<DWORD>* pids, bool isCase, std::vector<PROCESSENTRY32W>* pes = 0);
wstr process_getName(DWORD pid, LPPROCESSENTRY32W ref = 0);
// �����Ƿ���Ч
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

// ö�������Ӵ���
// hWndParent = ��Ҫö�ٵĸ�����, Ϊ0��ö����������
// hWnds = ����ö�ٽ������
// isCase = �Ƿ����ִ�Сд, �ж�����/����ʱʹ��, �������/������Ϊ��, ����Ըò���
// className = ��Ҫ�жϵ�����, �����ͱ�����һ�����Ͼͷ���
// titleName = ��Ҫ�жϵı���, �����ͱ�����һ�����Ͼͷ���
// compare = �Զ����жϺ���, ���ݴ˲��������ǰ��3������, ���� true���������
// param = ���ݵ��ص��������ֵ
int window_enumwindow(HWND hWndParent = 0, std::vector<HWND>* hWnds = 0, bool isCase = true, LPCWSTR className = 0, LPCWSTR titleName = 0, pfnEnumWindowCompare compare = 0, void* param = 0);


