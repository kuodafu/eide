#pragma once
#include <windows.h>

struct REGISTER_MESSAGE_INFO;
struct REGISTER_EVENT_INFO;

typedef LRESULT (CALLBACK* pfn_Register_Message)    (HWND, UINT, WPARAM, LPARAM, LPOBJSTRUCT);
typedef int(CALLBACK* pfn_Register_Event)           (UINT, LONG_PTR, LPVOID);
typedef int(__cdecl* pfn_iDraw_Event_GetMessageInfo)(HWND, UINT, REGISTER_MESSAGE_INFO**, int);
typedef int(__cdecl* pfn_iDraw_Event_GetEventInfo)  (UINT, REGISTER_EVENT_INFO**, int);
typedef int(__cdecl* pfn_iDraw_Register_Message)    (LPCSTR, HWND, UINT, pfn_Register_Message, pfn_Register_Message);
typedef int(__cdecl* pfn_iDraw_Register_Event)      (LPCSTR, UINT, LONG_PTR, pfn_Register_Event, LONG_PTR, pfn_Register_Event);
typedef int(__cdecl* pfn_DetachRegister_Message)    (HWND);
typedef int(__cdecl* pfn_iDraw_UnRegister)          (LPCSTR);



typedef struct REGISTER_MESSAGE_INFO
{
    pfn_Register_Message    pfnCallbackBefore;  // 消息处理前调用
    pfn_Register_Message    pfnCallbackAfter;   // 消息处理后返回前调用
    UINT                    message;            // 注册消息的消息值
    HWND                    hWnd;               // 
    int                     priority;           // 优先级
    LPCSTR                  guid;               // 
    LPCSTR                  dllName;            // 注册这个消息的dll名字, 调试用, 如果出现堆栈调用错误的问题这里就可以方便的看到是哪个模块

}*LPREGISTER_MESSAGE_INFO;


typedef struct REGISTER_EVENT_INFO
{
    pfn_Register_Event      pfnCallbackBefore;  // 事件处理前调用
    pfn_Register_Event      pfnCallbackAfter;   // 事件处理后返回前调用
    LONG_PTR                paramBefore;        // 事件处理前调用的参数
    LONG_PTR                paramAfter;         // 事件处理后返回前调用的参数
    UINT                    nEvent;             // 注册事件的事件值
    int                     priority;           // 优先级
    LPCSTR                  guid;
    LPCSTR                  dllName;            // 注册这个消息的dll名字, 调试用, 如果出现堆栈调用错误的问题这里就可以方便的看到是哪个模块

}*LPREGISTER_EVENT_INFO;


enum IDRAW_EVENT_ERROR
{
    IDRAW_ERR_OK                        = 0x00000000,   // 成功
    IDRAW_ERR_INVALIDHWND               = 0x00000001,   // 窗口句柄无效
    IDRAW_ERR_INVALIDGUID               = 0x00000002,   // GUID无效
};