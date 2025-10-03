#pragma once
#include "EIDE_Virtual.h"
//typedef int T;
// ���װ�����ָ��
template <typename T>
class EIDEPointer
{
    T* p;
public:
    EIDEPointer() : p(0)           { }
    EIDEPointer(T* ptr) : p(ptr)   { }
    ~EIDEPointer()                 { if (p) delete[] p; }

    inline operator T* ()          const throw() { return p; }
    inline T& operator*()          const         { return *p; }

    inline T** operator&()               throw() { return &p; }
    inline T* operator->()               throw() { return p; }
    inline bool operator!()        const throw() { return (p == NULL); }
    inline bool operator<(T* pT)   const throw() { return p < pT; }
    inline bool operator!=(T* pT)  const         { return !operator==(pT); }
    inline bool operator==(T* pT)  const throw() { return p == pT; }
};

typedef EIDEPointer<char> EIDEStringA;
typedef EIDEPointer<wchar_t> EIDEStringW;
typedef void(CALLBACK* PFN_COLORCHANGE)();

enum : int
{
    ����������_��������                 = 5,
    ����������_�������ͳ�Ա              = 6,
    ����������_ȫ�ֱ���                 = 7,
    ����������_����                   = 8,
    ����������_���򼯱���               = 9,
    ����������_�ӳ���                   = 10,
    ����������_�ӳ������               = 11,
    ����������_�ӳ������               = 12,
    ����������_�ӳ������               = 13,
    ����������_����                    = 14,
    ����������_��ԴͼƬ                 = 15,
    ����������_��Դ��Ƶ                 = 16,
    ��������_��                        = 0,
    ��������_����                    = 1,
    ��������_��������                   = 2,
    ��������_ȫ�ֱ���                   = 3,
    ��������_Dll����                   = 4,
    ��������_����                      = 5,
    ��������_����                      = 6,
    ��������_��ԴͼƬ                   = 7,
    ��������_��Դ��Ƶ                   = 8,
    ��������ƫ������_��������_����        = 261,
    ��������ƫ������_��������_����        = 263,
    ��������ƫ������_��������_��ע        = 262,
    ��������ƫ������_�������ͳ�Ա_����    = 313,
    ��������ƫ������_�������ͳ�Ա_����    = 314,
    ��������ƫ������_�������ͳ�Ա_��ַ    = 317,
    ��������ƫ������_�������ͳ�Ա_����    = 315,
    ��������ƫ������_�������ͳ�Ա_��ע    = 316,
    ��������ƫ������_ȫ�ֱ���_����        = 367,
    ��������ƫ������_ȫ�ֱ���_����        = 368,
    ��������ƫ������_ȫ�ֱ���_����        = 369,
    ��������ƫ������_ȫ�ֱ���_����        = 371,
    ��������ƫ������_ȫ�ֱ���_��ע        = 370,
    ��������ƫ������_����_����         = 580,
    ��������ƫ������_����_����         = 582,
    ��������ƫ������_����_����         = 583,
    ��������ƫ������_����_��ע         = 581,
    ��������ƫ������_���򼯱���_����      = 632,
    ��������ƫ������_���򼯱���_����      = 633,
    ��������ƫ������_���򼯱���_����      = 634,
    ��������ƫ������_���򼯱���_��ע      = 635,
    ��������ƫ������_�ӳ���_����         = 686,
    ��������ƫ������_�ӳ���_��������      = 687,
    ��������ƫ������_�ӳ���_����         = 690,
    ��������ƫ������_�ӳ���_�װ�         = 688,
    ��������ƫ������_�ӳ���_��ע         = 689,
    ��������ƫ������_�ӳ������_����      = 740,
    ��������ƫ������_�ӳ������_����      = 741,
    ��������ƫ������_�ӳ������_�ο�      = 742,
    ��������ƫ������_�ӳ������_�ɿ�      = 743,
    ��������ƫ������_�ӳ������_����      = 744,
    ��������ƫ������_�ӳ������_��ע      = 745,
    ��������ƫ������_�ӳ������_����      = 796,
    ��������ƫ������_�ӳ������_����      = 797,
    ��������ƫ������_�ӳ������_��̬      = 798,
    ��������ƫ������_�ӳ������_����      = 799,
    ��������ƫ������_�ӳ������_��ע      = 800,
    ��������ƫ������_�ӳ������          = 851,


};



#define OP_PUSH_EAX     80
#define OP_PUSH_EBX     83
#define OP_PUSH_EDI     87
#define OP_PUSH_ESI     86
#define OP_PUSHAD       96
#define OP_PUSHFD       156
#define OP_POP_EAX      88
#define OP_POP_EBX      91
#define OP_POP_EDI      95
#define OP_POP_ESI      94
#define OP_POPAD        97
#define OP_POPFD        157
#define OP_JO           112
#define OP_JNLE         127
#define OP_JECXZ        227
#define OP_CALL         232
#define OP_JMP_L        233
#define OP_JMP          235
#define OP_RET          195
#define OP_RETN         194
#define OP_INT3         204



#define CWnd_m_hWnd                             28      // 
#define CFrameWnd_m_hMenuDefault                68      // 
#define CFrameWnd_m_listControlBars             108     // ��ָ��
#define CMDIFrameWnd_m_hWndMDIClient            188     // 
#define CWinThread_m_pMainWnd                   28      // 
#define CWinApp_m_pDocManager                   128     // 
#define CDocManager_m_templateList              4       // ��ָ��
#define CMultiDocTemplate_m_hMenuShared         100     // 
#define CPtrList_m_pNodeHead                    4       // 
#define CPtrList_m_pNodeTail                    8       // 
#define CPtrList_m_nCount                       12      // 
#define CNode_pNext                             0       // 
#define CNode_pPrev                             4       // 
#define CNode_data                              8       // 
#define CControlBar_m_dwStyle                   100     // 
#define CControlBar_m_pDockSite                 108     // 
#define CControlBar_m_pDockBar                  112     // 
#define CControlBar_m_pDockContext              116     // 
#define ECodeEdit_nCodeType                     60      // 
#define ECodeEdit_nCursorRowStart               16      // 
#define ECodeEdit_nCursorTabColStart            120     // 
#define ECodeEdit_nCursorColumnStart            124     // 
#define ECodeEdit_nCursorColumnEnd              128     // 
#define ECodeEdit_nCursorRowEnd                 132     // 
#define ECodeEdit_nCursorTabColEnd              136     // 
#define ECodeEdit_pSelectRowInfo                192     // 
#define ECodeEdit_nSelectRowInfoMaxLen          196     // 
#define ECodeEdit_nSelectRowInfoLen             200     // 


inline int thiscall(const void* pThis, const void* pFun, int count, ...)
{
    if (!pFun || !pThis)return 0;
    int nRet = 0;
    __asm
    {
        lea edx, [ebp + 0x10];
        mov ecx, [edx];
        test ecx, ecx;
        jz c;

    s:
        // edx��ecx�ĵ�ַ, ������3����Ա, ��ecx����3, ѭ���ķֱ��� edx[3], edx[2], edx[1]
        // edx[0] ָ��count, edx[1] ָ�� count����һ������, edx[2] ָ��count��ĵڶ�������, �Դ�����
        push[edx + ecx * 4];   // ��������push, ecx�ǵݼ�
        loop s;
    c:
        mov ecx, pThis;
        call pFun;
        mov[nRet], eax
    }
    return nRet;
}


void EIDE_bindSelChanged(void* pfn);
// �Ӵ��봰�ڴ���һ�������ӿ���
// hWnd = MDIClient�µ��Ĳ㴰��, �����д����Ĵ���
IEIDE_CodeManage* EIDE_CreateIEIDE_CodeManage(HWND hWnd);
// �ͷ�һ�������ӿ���
void EIDE_DeleteIEIDE_CodeManage(IEIDE_CodeManage* pObj);

// ��ȡ��������ɫ����, ����д�뻺�����ֽ���
// pBuffer = ������ɫ���ݵĻ�����, Ϊ0���ȡ�������ߴ�
// nBufferSize = ��������С, Ϊ0���ȡ�������ߴ�
int EColor_GetData(void* pBuffer, int nBufferSize);
// ������������ɫ����, ����д����������ɫ���ֽ���
// pBuffer = ��ɫ����, ����clr�ļ�������, ����clr�ļ�·��
// nBufferSize = Ϊ0��pBuffer��ʾclr�ļ�·��, �������clr�ļ����ݵĴ�С, pBufferִ��clr�ļ�����
int EColor_SetData(void* pBuffer, int nBufferSize);

void EColor_SetBeaEnginePath(LPCWSTR file);
void EColor_ColorChangeCallback(PFN_COLORCHANGE pCallback);
