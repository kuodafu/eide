#pragma once
#include <windows.h>

enum MONITORTYPE
{
    MONITORTYPE_READ            = 0x0000,   // ��
    MONITORTYPE_WRITE           = 0x0001,   // д
    MONITORTYPE_READWRITE       = 0x0002,   // ��д
};

typedef void(CALLBACK* PFN_MONITORADDRESS_CALLBACK)(LPVOID pAddress, MONITORTYPE type, UINT64 param);

struct MONITORTYPE_DATA_STRUCT
{
    PFN_MONITORADDRESS_CALLBACK pCallback;      // �ص�����
    UINT64                      param;          // �ص��������Ӳ���
    LPBYTE                      pBegin;         // ����ص��׵�ַ
    LPBYTE                      pEnd;           // ����صĽ�����ַ
    MONITORTYPE                 type;           // ��ص�����, Ŀǰֻ�м�ض�/д/��д
    LPBYTE                      pPageBegin;     // ����ص�ַ��ҳ�׵�ַ
    LPBYTE                      pPageEnd;       // ����ص�ַ��ҳ������ַ
    DWORD                       newProtect;     // �µ��ڴ�ı���ģʽ
    DWORD                       oldProtect;     // ԭ���ڴ�ı���ģʽ
    BYTE                        oldOPCode;      // 0xCCԭ����ֵ
    LPBYTE                      pBreak;         // ���öϵ�ĵ�ַ
    LPBYTE                      pBreakBegin;    // �ϵ���ʼ��ַ
    DWORD                       breakSize;      // �ϵ��ڴ�ҳ��С
};


// ���ָ����ַ, ��ȷ����ַ�Ŀ�����, �ɹ�����true, ʧ�ܷ���false
// pAddress ��Ҫ��صĵ�ַ, �������ַ����дʱ��֪ͨ pCallback �ص�����
// size = ��Ҫ��صĳߴ�, ��ַ�� pAddress + size ֮�䴥���Ķ�д�������ᴥ���ص�����
// type = ��ص�����, Ŀǰ��֧�� MONITORTYPE ö���������ֵ, ���ڴ津���������ʱ���ûص�����
// pCallback = ���ձ���д�Ļص�����
bool MonitorAddress(LPVOID pAddress, int size, MONITORTYPE type, PFN_MONITORADDRESS_CALLBACK pCallback, UINT64 param = 0);

// ȡ�����ָ����ַ
// pAddress = ���� MonitorAddress ʱ��д�ļ�ص�ַ, ��ʹ��ʱ��Ҫȡ��, �����������ûص���ַ, ����ص���ַ��dll, ��dll�ѱ��ͷ�, �Ǿͻ����
bool UnMonitorAddress(LPVOID pAddress);

