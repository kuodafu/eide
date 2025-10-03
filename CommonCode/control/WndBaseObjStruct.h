#pragma once
#include <d2d/d2d_old.h>
#include <CommCtrl.h>
#include "WndBaseState.h"

typedef struct COLORINFO
{
    DWORD   crBackGround;           // ������ɫ
    DWORD   crText;                 // �ı���ɫ
    DWORD   crTextHot;              // �ı���ɫ
    DWORD   crTextDown;             // �ı���ɫ
    DWORD   crTextBan;              // �ı���ɫ
    DWORD   crNormal;               // �ؼ�������ɫ
    DWORD   crHover;                // �ؼ�������ɫ
    DWORD   crDown;                 // �ؼ�������ɫ
    DWORD   crFocus;                // �ؼ�������ɫ
    DWORD   crChecked;              // �ؼ�ѡ����ɫ
    DWORD   crSelected;             // �ؼ�ѡ����ɫ
    DWORD   crHot;                  // �ؼ��ȵ���ɫ
    DWORD   crBan;                  // �ؼ���ֹ��ɫ
    DWORD   crVisted;               // 
    DWORD   crShadow;               // ��Ӱ��ɫ
    DWORD   crBorder;               // �߿���ɫ

    HBRUSH  hbrBackGround;          // ������ɫ
    HBRUSH  hbrText;                // �ı���ɫ
    HBRUSH  hbrTextHot;             // �ı���ɫ
    HBRUSH  hbrTextDown;            // �ı���ɫ
    HBRUSH  hbrTextBan;             // �ı���ɫ
    HBRUSH  hbrNormal;              // �ؼ�������ɫ
    HBRUSH  hbrHover;               // �ؼ�������ɫ
    HBRUSH  hbrDown;                // �ؼ�������ɫ
    HBRUSH  hbrFocus;               // �ؼ�������ɫ
    HBRUSH  hbrChecked;             // �ؼ�ѡ����ɫ
    HBRUSH  hbrSelected;            // �ؼ�ѡ����ɫ
    HBRUSH  hbrHot;                 // �ؼ��ȵ���ɫ
    HBRUSH  hbrBan;                 // �ؼ���ֹ��ɫ
    HBRUSH  hbrVisted;              // 
    HBRUSH  hbrShadow;              // ��Ӱ��ɫ
    HPEN    hpenBorder;             // �߿򻭱���ɫ


}*LPCOLORINFO;
typedef struct DRAWINFO
{
    LPCANVAS    hCanvas;            // d2d���ƾ��
    LPEX_FONT   hFont;              // ����滭����
    LPEX_FONT   hFontBold;          // ����滭�Ӵ�����
    DWORD       textFormat;         // �滭�ı������ʽ
    STATE       state;              // �����ǰ״̬
    COLORINFO   color;              // �����ɫ

    bool        isNoNcPaint;        // ���ػ��ǿͻ���
    bool        isNcCalcSize;       // ���´���ǿͻ���, �ѷǿͻ�������Ϊ0
    bool        isTransparent;      // �Ƿ�͸��
    bool        isHot;              // �Ƿ�֧���ȵ�
    bool        isNoSize;           // ������Size��Ϣ
    bool        isClient;           // ���������Ϣ��ת���ɿͻ�����Ϣ
    bool        isErasebkg;         // �������¼�, ������Ϊfalse, ��ײ㲻����WM_ERASEBACKGROUND ��Ϣ
    bool        isCheckBox;         // �Ƿ���ѡ���

    int         fmt;                // �ı�����ģʽ
    int         nItemHeight;        // ��Ŀ�߶�
    RECT        offsetRect;         // ����λ��ƫ��, ���� WM_WINDOWPOSCHANGING �޸� lParamָ�������

    HTREEITEM   hItemSel;           // ���Ϳ�ר��, ��ǰѡ�б�����
    HTREEITEM   hItemHot;           // ���Ϳ�ר��, ��ǰ�ȵ������
    HTREEITEM   hItemDown;          // ���Ϳ�ר��, ��ǰ���±�����

    int         indexSel;           // �б������ר��, ��ǰѡ�б�������
    int         indexHot;           // �б������ר��, ��ǰѡ�б�������
    int         indexDown;          // �б������ר��, ��ǰѡ�б�������

}*LPDRAWINFO;
typedef struct POSINFO
{
    RECT        rc;                 // ������Ļλ��, ���ҵ�
    RECT        rcIng;              // windowchange��Ϣʱ��λ��
    RECT        rcClient;           // ����ͻ���λ��, ���ҵ�
    int         x;                  // �������
    int         y;                  // ���󶥱�
    int         width;              // ������
    int         height;             // ����߶�
}*LPPOSINFO;

struct OBJSTRUCT;
typedef LRESULT(CALLBACK* pfn_SubClassWndProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, OBJSTRUCT* pData);
typedef LRESULT(CALLBACK* pfn_CallWindowProc)(WNDPROC lpPrevWndFunc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
typedef LRESULT(CALLBACK* pfn_DefWindowProc)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
typedef LONG_PTR(WINAPI* pfn_SetLong)(HWND hWnd, int nIndex, LONG_PTR dwNewLong);
typedef LONG_PTR(WINAPI* pfn_GetLong)(HWND hWnd, int nIndex);
typedef void (WINAPI* pfn_ColorChange)(OBJSTRUCT* pData);
typedef bool (CALLBACK* pfn_CalcScroll)(void* pScroll, OBJSTRUCT* pData, RECT* prc);
typedef bool (CALLBACK* pfn_DrawScroll)(void* pScroll, OBJSTRUCT* pData, bool isVScroll, HDC hdc, int type, RECT_F* rcDraw);
typedef bool (CALLBACK* pfn_GetScrollRect)(void* pScroll, OBJSTRUCT* pData, bool isVScroll, RECT* prc);

typedef struct OBJSTRUCT
{
    union
    {
        HWND        hObj;               // ���������߽д��ھ��
        HWND        hWnd;               // ���ھ�����߽ж�����
    };

    pfn_SubClassWndProc fnWndproc;      // ����Ĵ�����, ���໯�ص�����������������
    pfn_SubClassWndProc fnNotify;       // �����֪ͨ������, ÿһ���������յ������Ϣʱ�����ȡ���������Ϣ�Ĵ�������, 5���������Ǹ����ڵĲ���
    pfn_SubClassWndProc fnDrawItem;     // ����Ļ滭������, ÿһ���������յ������Ϣʱ�����ȡ���������Ϣ�Ĵ�������, 5���������Ǹ����ڵĲ���
    pfn_SubClassWndProc fnMeasureItem;  // ����Ĳ˵����¼��㺯��, ÿһ���������յ������Ϣʱ�����ȡ���������Ϣ�Ĵ�������, 5���������Ǹ����ڵĲ���
    pfn_SubClassWndProc fnCommand;      // �����WM_COMMAND��Ϣ, 5���������Ǹ����ڵĲ���, pData����Ҳ�Ǹ����ڵ�����
    pfn_SubClassWndProc fnCtlcolor;     // �����WM_ERASEBKGND��Ϣ, 5���������Ǹ����ڵĲ���, pData����Ҳ�Ǹ����ڵ�����
    pfn_CallWindowProc  fnCallProc;     // ����ԭ����ʹ�õ�API, ���û���������ֵ, Ĭ�ϻ��ȡ CallWindowProcW
    pfn_DefWindowProc   fnDefProc;      // Ĭ�ϵ���ʹ�õ�API, ���û���������ֵ, Ĭ�ϻ��ȡ DefWindowProcW
    pfn_ColorChange     fnCrChange;     // ��ɫ�ı亯��, ��Щ���ʹ�õĲ���Ĭ����ɫ, ��ɫ�ı����Ҫ���������������ɫ
    //pfn_CalcScroll      fnCalcScroll;   // ���������λ��, ����ͨ������¼��޸Ĵ��ڷǿͻ���λ��
    //pfn_DrawScroll      fnDrawScroll;   // �滭������
    //pfn_GetScrollRect   fnGetScrollRect;// ��ȡ������λ��, ����ͨ������¼��޸ı߿�ﵽָ���滭������λ��


    pfn_SetLong         SetWndLong;     // ���ô�������, �Ѿ�������W���A��
    pfn_GetLong         GetWndLong;     // ��ȡ��������, �Ѿ�������W���A��
    pfn_SetLong         SetClsLong;     // ����������, �Ѿ�������W���A��
    pfn_GetLong         GetClsLong;     // ��ȡ������, �Ѿ�������W���A��

    LRESULT             lResult;        // ���ú���Ϣ�¼�, ����ԭ���̵ķ���ֵ
    bool                isReturn;       // ������Ϣ, ʹ�ûص�����Ϣ����
    void*               drawScroll;     // ���ƹ���������, ��Ҫǿת�� IDrawScroll*

    WNDPROC oldProc;                    // ���ԭ������Ϣ�ص�������

    DRAWINFO    draw;                   // �滭��Ϣ
    POSINFO     pos;                    // λ����Ϣ
    LONG_PTR    style;                  // ������ʽ
    LONG_PTR    styleEx;                // ������չ��ʽ
    HCURSOR     hCursor;                // ���ָ��
    LONG_PTR    id;                     // ����ID
    LPVOID      param;                  // ���󸽼�����
    LPVOID      param1;                 // ���󸽼�����1
    LPVOID      param2;                 // ���󸽼�����2
    LPVOID      userdata;               // �û�����, �ڲ�����ռ�����ֵ
    LPWSTR      title;                  // �������
    LPWSTR      tips;                   // ������ʾ��Ϣ
    ATOM        atom;                   // ��ԭ��
    size_t      index;                  // �б�/���������������Ŀؼ�ʹ��, ��ǰ����
    size_t      count;                  // �б�/���������������Ŀؼ�ʹ��, ����
    DWORD       flags;                  // ���ڱ�־
    LONG_PTR    winState;               // ���ڵ�ǰ״̬, ���, ��С��, ����
    LPEX_RGN    hrgn_client;            // ���ڿͻ�������
    LPEX_RGN    hrgn_sizebox;           // ���ڱ߿�����
    FLOAT       rectroundX;             // ����Բ��x
    FLOAT       rectroundY;             // ����Բ��y
    HWND        hParent;                // ��������ھ��
    struct OBJSTRUCT* pParent;          // �����������Ϣ
#ifdef __cplusplus
    OBJSTRUCT() { memset(this, 0, sizeof(*this)); }
#endif
}*POBJSTRUCT, *LPOBJSTRUCT;

