#pragma once
#include "DrawScroll.h"
#include <commctrl.h>
#include "WndBaseState.h"

//typedef struct OBJDATA
//{
//#ifdef __cplusplus
//    OBJDATA() { memset(this, 0, sizeof(*this)); }
//#endif
//    void* pWnd;
//    RECT rc;                        // ������λ��
//    RECT rcClient;                  // ����ͻ���λ��
//    RECT rcWnd;                     // �������λ��
//    RECT rcDirty;                   // ���������
//    RECT rcText;                    // ����ı�ƫ��λ��
//
//    COLORINFO cr;                   // �����ɫ
//    
//    void* hObj;                     // ������
//    void* hChildFirst;              // ��һ�������
//    void* hChildLast;               // ���һ�������
//    void* hLayout;                  // ���־��
//    DWORD flags;                    // �����־
//    void* pBackgroundImage;         // �������ͼƬ
//    void* hTheme;                   // ���������
//
//    void* minmax_reserved_1;        // ����1
//    void* minmax_reserved_2;        // ����2
//    POINT minmax_maxsize;           // ������ߴ�
//    POINT minmax_maxpostion;        // ���
//    POINT minmax_mintracksize;
//    POINT minmax_maxtracksize;
//
//    void* hPathWindow;            
//    void* hPathClient;    
//    float radius_uint;
//    RECT_F radius;
//    DWORD propCount;
//    void* objNext;
//    void* objPrev;
//    void* objnextTabstop;
//    void* objVScroll;
//    void* objHScroll;
//    LPWSTR pTitle;
//    LPWSTR pTips;
//    void* pObjJS;
//    void* hTableJsEvents;
//    int state;
//    void* OwnerData;
//    LPCANVAS hCanvas;
//    void* userData;
//    LONG_PTR styleEx;
//    LONG_PTR style;
//    LPEX_FONT hFont;
//    LPWSTR atomName;
//    HCURSOR hCursor;
//    void* pPropListEntry;
//    LPWSTR pCls;
//    void* fnClsProc;
//    LONG_PTR id;
//    DWORD textFormat;
//    LPEX_RGN hRgnSizebox;
//    void* objParent;
//    void* lParam;
//    DWORD AlphaDisable;
//    DWORD Alpha;
//    void* fnSubClass;
//    float fHUE;
//    float fBlur;
//    LONG_PTR nodeid;
//}*LPOBJDATA;