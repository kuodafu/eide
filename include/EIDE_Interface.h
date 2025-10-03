#pragma once
#include "EIDE_Virtual.h"
//typedef int T;
// 简易版智能指针
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
    代码行类型_数据类型                 = 5,
    代码行类型_数据类型成员              = 6,
    代码行类型_全局变量                 = 7,
    代码行类型_程序集                   = 8,
    代码行类型_程序集变量               = 9,
    代码行类型_子程序                   = 10,
    代码行类型_子程序参数               = 11,
    代码行类型_子程序变量               = 12,
    代码行类型_子程序代码               = 13,
    代码行类型_常量                    = 14,
    代码行类型_资源图片                 = 15,
    代码行类型_资源音频                 = 16,
    代码类型_无                        = 0,
    代码类型_程序集                    = 1,
    代码类型_数据类型                   = 2,
    代码类型_全局变量                   = 3,
    代码类型_Dll命令                   = 4,
    代码类型_窗口                      = 5,
    代码类型_常量                      = 6,
    代码类型_资源图片                   = 7,
    代码类型_资源音频                   = 8,
    代码数据偏移类型_数据类型_名称        = 261,
    代码数据偏移类型_数据类型_公开        = 263,
    代码数据偏移类型_数据类型_备注        = 262,
    代码数据偏移类型_数据类型成员_名称    = 313,
    代码数据偏移类型_数据类型成员_类型    = 314,
    代码数据偏移类型_数据类型成员_传址    = 317,
    代码数据偏移类型_数据类型成员_数组    = 315,
    代码数据偏移类型_数据类型成员_备注    = 316,
    代码数据偏移类型_全局变量_名称        = 367,
    代码数据偏移类型_全局变量_类型        = 368,
    代码数据偏移类型_全局变量_数组        = 369,
    代码数据偏移类型_全局变量_公开        = 371,
    代码数据偏移类型_全局变量_备注        = 370,
    代码数据偏移类型_程序集_名称         = 580,
    代码数据偏移类型_程序集_基类         = 582,
    代码数据偏移类型_程序集_公开         = 583,
    代码数据偏移类型_程序集_备注         = 581,
    代码数据偏移类型_程序集变量_名称      = 632,
    代码数据偏移类型_程序集变量_类型      = 633,
    代码数据偏移类型_程序集变量_数组      = 634,
    代码数据偏移类型_程序集变量_备注      = 635,
    代码数据偏移类型_子程序_名称         = 686,
    代码数据偏移类型_子程序_返回类型      = 687,
    代码数据偏移类型_子程序_公开         = 690,
    代码数据偏移类型_子程序_易包         = 688,
    代码数据偏移类型_子程序_备注         = 689,
    代码数据偏移类型_子程序参数_名称      = 740,
    代码数据偏移类型_子程序参数_类型      = 741,
    代码数据偏移类型_子程序参数_参考      = 742,
    代码数据偏移类型_子程序参数_可空      = 743,
    代码数据偏移类型_子程序参数_数组      = 744,
    代码数据偏移类型_子程序参数_备注      = 745,
    代码数据偏移类型_子程序变量_名称      = 796,
    代码数据偏移类型_子程序变量_类型      = 797,
    代码数据偏移类型_子程序变量_静态      = 798,
    代码数据偏移类型_子程序变量_数组      = 799,
    代码数据偏移类型_子程序变量_备注      = 800,
    代码数据偏移类型_子程序代码          = 851,


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
#define CFrameWnd_m_listControlBars             108     // 非指针
#define CMDIFrameWnd_m_hWndMDIClient            188     // 
#define CWinThread_m_pMainWnd                   28      // 
#define CWinApp_m_pDocManager                   128     // 
#define CDocManager_m_templateList              4       // 非指针
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
        // edx是ecx的地址, 假设有3个成员, 那ecx就是3, 循环的分别是 edx[3], edx[2], edx[1]
        // edx[0] 指向count, edx[1] 指向 count的下一个参数, edx[2] 指向count后的第二个参数, 以此类推
        push[edx + ecx * 4];   // 从右往左push, ecx是递减
        loop s;
    c:
        mov ecx, pThis;
        call pFun;
        mov[nRet], eax
    }
    return nRet;
}


void EIDE_bindSelChanged(void* pfn);
// 从代码窗口创建一个代码框接口类
// hWnd = MDIClient下第四层窗口, 这个是写代码的窗口
IEIDE_CodeManage* EIDE_CreateIEIDE_CodeManage(HWND hWnd);
// 释放一个代码框接口类
void EIDE_DeleteIEIDE_CodeManage(IEIDE_CodeManage* pObj);

// 获取易语言配色数据, 返回写入缓冲区字节数
// pBuffer = 接收配色数据的缓冲区, 为0则获取缓冲区尺寸
// nBufferSize = 缓冲区大小, 为0则获取缓冲区尺寸
int EColor_GetData(void* pBuffer, int nBufferSize);
// 设置易语言配色数据, 返回写入易语言配色的字节数
// pBuffer = 配色数据, 整个clr文件的数据, 或者clr文件路径
// nBufferSize = 为0则pBuffer表示clr文件路径, 否则就是clr文件数据的大小, pBuffer执行clr文件数据
int EColor_SetData(void* pBuffer, int nBufferSize);

void EColor_SetBeaEnginePath(LPCWSTR file);
void EColor_ColorChangeCallback(PFN_COLORCHANGE pCallback);
