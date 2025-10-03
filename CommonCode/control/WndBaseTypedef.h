#pragma once
#include <tstr.h>
#include <d2d/d2d_old.h>
#include "WndBaseState.h"

#define EWF_bRendered 16384 


#pragma pack(1)
typedef struct SYSBUTTONIMAGE
{
    WORD flag;                  // �ļ���־ 19278 NK
    BYTE MajorVersion;          // ���������ļ����汾��
    BYTE MinorVersion;          // ���������ļ��ΰ汾��
    BYTE btnWidth;              // ϵͳ��ť���
    BYTE btnHeight;             // ϵͳ��ť�߶�
    BYTE count;                 // ϵͳ��ťͼƬ����
    BYTE Reservations;          // ����
    DWORD imgSize;              // ͼƬ�ߴ�
    WORD imgWidth;              // �������, �����ļ�ͼƬ���
    WORD imgHeight;             // �����߶�, �����ļ�ͼƬ�߶�
    DWORD offset;               // ͼƬƫ����
    void* v1;                   // ����
#ifndef _M_X64
    void* v2;                   // ����
#endif
    RECT_F rc[48];              // ��ť��ͼƬ�е�λ��, 12��ϵͳ��ť,ÿ����ť4��״̬, һ��48��
}*LPSYSBUTTONIMAGE;
#pragma pack()



struct SYSBUTTON
{
    RECT rc;                    // ϵͳ��ťλ��
    DWORD flag;                 // ��־
    DWORD ID;                   // ���, ���ݱ�Ż滭��ͬͼƬ
};

typedef struct tagSYSBTN
{
    LPSYSBUTTONIMAGE image;         // ����ϵͳ��ť�滭λ�ú�ͼƬָ��
    LPEX_IMAGE pImage;              // �滭ͼƬָ��
    LPEX_IMAGE pImage_light;        // �滭ͼƬָ��, ǳɫģʽʹ�õ�ͼƬ
    SYSBUTTON btn[10];              // ϵͳ��ť��, �������10��, ��̬����, ����0
    DWORD count;                    // ��ǰ����ʹ�õ�ϵͳ��ť����
    DWORD index;                    // ��ǰ�滭�����ĸ���ť
    STATE state;                    // ϵͳ��ť��ǰ״̬, ֻ����һ����ť��״̬
    DWORD downIndex;                // ϵͳ��ť��������
    DWORD rdownIndex;               // ϵͳ��ť�Ҽ���������
    RECT rcBtn;                     // ����ϵͳ��ťλ��
    RECT rcCaption;                 // ������λ��
    RECT rcIcon;                    // ͼ��λ��
}SYSBTN, * LPSYSBTN;

