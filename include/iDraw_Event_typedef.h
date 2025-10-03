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
    pfn_Register_Message    pfnCallbackBefore;  // ��Ϣ����ǰ����
    pfn_Register_Message    pfnCallbackAfter;   // ��Ϣ����󷵻�ǰ����
    UINT                    message;            // ע����Ϣ����Ϣֵ
    HWND                    hWnd;               // 
    int                     priority;           // ���ȼ�
    LPCSTR                  guid;               // 
    LPCSTR                  dllName;            // ע�������Ϣ��dll����, ������, ������ֶ�ջ���ô������������Ϳ��Է���Ŀ������ĸ�ģ��

}*LPREGISTER_MESSAGE_INFO;


typedef struct REGISTER_EVENT_INFO
{
    pfn_Register_Event      pfnCallbackBefore;  // �¼�����ǰ����
    pfn_Register_Event      pfnCallbackAfter;   // �¼�����󷵻�ǰ����
    LONG_PTR                paramBefore;        // �¼�����ǰ���õĲ���
    LONG_PTR                paramAfter;         // �¼�����󷵻�ǰ���õĲ���
    UINT                    nEvent;             // ע���¼����¼�ֵ
    int                     priority;           // ���ȼ�
    LPCSTR                  guid;
    LPCSTR                  dllName;            // ע�������Ϣ��dll����, ������, ������ֶ�ջ���ô������������Ϳ��Է���Ŀ������ĸ�ģ��

}*LPREGISTER_EVENT_INFO;


enum IDRAW_EVENT_ERROR
{
    IDRAW_ERR_OK                        = 0x00000000,   // �ɹ�
    IDRAW_ERR_INVALIDHWND               = 0x00000001,   // ���ھ����Ч
    IDRAW_ERR_INVALIDGUID               = 0x00000002,   // GUID��Ч
};