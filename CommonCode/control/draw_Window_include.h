#pragma once
#include "WndBase.h"
#include <control/CShadow.h>
#include <control/WndBaseObjStruct.h>


#define SC_MENU         MAKELONG(0, 0x1)
#define SC_SETTING      MAKELONG(0, 0x2)
#define SC_SKIN         MAKELONG(0, 0x4)
#define SC_HELP         MAKELONG(0, 0x8)

typedef struct DRAW_WINDOW_COLOR
{
    DWORD               cbSize;         // ��ǰ�ṹ�ߴ�, �������Ļ���������ߴ��ж�
    ARGB                crNormal;       // ������ɫ, ARGB��ɫ, Ϊ0��ʹ��
    ARGB                crHot;          // ��ȼ��ɫ, ARGB��ɫ, Ϊ0��ʹ��
    ARGB                crDown;         // ������ɫ, ARGB��ɫ, Ϊ0��ʹ��
    ARGB                crBack;         // ������ɫ, ARGB��ɫ, Ϊ0��ʹ��
    ARGB                crBorder;       // ��Ӱ�߿���ɫ, Ϊ0��û�б߿�, ARGB��ɫ
    ARGB                crBorder_nf;    // ��Ӱ�߿�δ����ʱ����ɫ, Ϊ0��û�б߿�, ARGB��ɫ
    ARGB                crShadow;       // ��Ӱ��ɫ, ARGB��ɫ, Ϊ0��ʹ����Ӱ
    ARGB                crShadow_nf;    // ��Ӱδ����ʱ����ɫ, ARGB��ɫ, Ϊ0��ʹ����Ӱ

    ARGB                crTitle;        // �����Ѽ�����ɫ, ARGB��ɫ
    ARGB                crTitle_nf;     // ����δ������ɫ, ARGB��ɫ
}*LPDRAW_WINDOW_COLOR;

typedef struct DRAW_WINDOW_ARGUMENT : DRAW_WINDOW_COLOR
{
    HWND                hWnd;           // ��Ҫ����Ĵ���
    int                 sysbtnWidth;    // ϵͳ��ť���
    int                 sysbtnHeight;   // ϵͳ��ť�߶�
    BOOL                isSubClass;     // �Ƿ��ڲ����໯�ô���, Ϊfalse���ⲿ���� iDraw_DrawWindowProc ����Ҫ�������Ϣ���ݽ���
    BOOL                isUnicode;      // �����Ƿ�ΪUnicode����, Ϊtrue��ʹ��W�溯��
    BOOL                isCasualMove;   // ���������ƶ�, Ϊfalse��ֻ���ڱ������в����ƶ�
    BOOL                isDrawIcon;     // �Ƿ�滭ͼ��, ������滭ͼ��, �����ͼ��λ�õ��������ֲ˵�, ����Ҳ��ռ��ͼ���λ��
    BOOL                isDrawDefBtn;   // �Ƿ�滭Ĭ�ϵİ�ť, ���Ϊtrue, ����ݴ��ڷ��, ����ϵͳ��ť, ��ť��ID�� SC_ ��ͷ�ĳ���

    int                 nMinWidth;      // ��С���ڿ��
    int                 nMinHeight;     // ��С���ڸ߶�

    LPCANVAS            hCanvas;        // �滭���, �ⲿ�ṩһ���滭���, ������ṩ, ���ڲ��ᴴ��һ��
    LPEX_FONT           hFont;          // �滭����ʹ�õ�����, ����ⲿ���ṩ, �ڲ��ᴴ��һ��
    LPEX_BRUSH          hbrTextTitle;   // �滭����ʹ�õ�����, ����ⲿ���ṩ, �ڲ��ᴴ��һ��
    LPEX_BRUSH          hbrTextTitle_nf; // �滭����ʹ�õ�����, ����ⲿ���ṩ, �ڲ��ᴴ��һ��

}*LPDRAW_WINDOW_ARGUMENT;


template<typename T>
struct DRAW_EVENT_STRUCT
{
    T       pfn;    // �¼��Ļص�����
    LPARAM  param;  // �¼��ĸ��Ӳ���
    inline operator bool()
    {
        return pfn != 0;
    }
};

struct SYSBUTTON_INFO
{
    RECT                rc;             // ϵͳ��ťλ��
    DWORD               ID;             // ��ťID, ����¼��Ǹ���ID�������
    LPARAM              param;          // ��ť����Ŀ��ֵ
};

// �滭�Ľ׶�
enum DRAW_STAGE
{
    DRAW_STAGE_BUTTON_BEGIN,    // �滭ϵͳ��ť��ʼ, �����滭ϵͳ��ť, ����true��ʹ���ڲ��Ļ���
    DRAW_STAGE_BUTTON_END,      // �滭ϵͳ��ť����, ��ť�Ѿ��滭���
    DRAW_STAGE_ICON_BEGIN,      // �滭ϵͳͼ�꿪ʼ, �����滭ϵͳͼ��, ����true��ʹ���ڲ��Ļ���
    DRAW_STAGE_ICON_END,        // �滭ϵͳͼ�����, ϵͳͼ���Ѿ��滭���
    DRAW_STAGE_TITLE_BEGIN,     // �滭���⿪ʼ, �����滭����, ����true��ʹ���ڲ��Ļ���
    DRAW_STAGE_TITLE_END,       // �滭�������, �����Ѿ��滭���
};

// �滭�Ľṹ
typedef struct DRAW_PAINTSTRUCT
{
    HWND            hWnd;       // ���ھ��
    LPCANVAS        hCanvas;    // �������
    DRAW_STAGE      stage;      // �滭�׶�
    const RECT_F*   rc;         // �滭�ľ���λ��
    int             id;         // �滭ϵͳ��ťʱʹ��, �����ʾ��ǰ�滭�İ�ťID
    STATE           state;      // ״̬, ϵͳ��ťʹ��
    LPARAM          param;      // ���Ӳ���, �滭�¼��󶨵���Ŀ��ֵ
    LPCWSTR         lpszTitle;  // �滭����ʱʹ�õı����ı�
    LPEX_IMAGE      hIcon;      // �滭ϵͳͼ��ʹ�õ�ͼ��
    LPEX_BRUSH      hbrText;    // �滭����ʹ�õĻ�ˢ

}*LPDRAW_PAINTSTRUCT;




// ϵͳ��ť������¼�
typedef bool(CALLBACK* PFN_SysBtnClick)(int id, LPARAM btnParam, BOOL isRButton, LPARAM param);
// ϵͳ��ť���滭�¼�
typedef bool(CALLBACK* PFN_SysDraw)(LPDRAW_PAINTSTRUCT dps);

typedef struct DRAW_WINDOW_DATA : DRAW_WINDOW_ARGUMENT
{
    WNDPROC             oldProc;
    pfn_CallWindowProc  fnCallProc;     // ����ԭ����ʹ�õ�API, ���û���������ֵ, Ĭ�ϻ��ȡ CallWindowProcW
    pfn_DefWindowProc   fnDefProc;      // Ĭ�ϵ���ʹ�õ�API, ���û���������ֵ, Ĭ�ϻ��ȡ DefWindowProcW
    pfn_ColorChange     fnCrChange;     // ��ɫ�ı亯��, ��Щ���ʹ�õĲ���Ĭ����ɫ, ��ɫ�ı����Ҫ���������������ɫ

    pfn_SetLong         SetWndLong;     // ���ô�������, �Ѿ�������W���A��
    pfn_GetLong         GetWndLong;     // ��ȡ��������, �Ѿ�������W���A��
    pfn_SetLong         SetClsLong;     // ����������, �Ѿ�������W���A��
    pfn_GetLong         GetClsLong;     // ��ȡ������, �Ѿ�������W���A��

    SYSBUTTON_INFO      btn[100];       // ϵͳ��ť��, �������100��, ��̬����, ����0
    DWORD               count;          // ��ǰ����ʹ�õ�ϵͳ��ť����
    DWORD               index;          // ��ǰ�滭�����ĸ���ť
    STATE               state;          // ϵͳ��ť��ǰ״̬, ֻ����һ����ť��״̬
    DWORD               downIndex;      // ϵͳ��ť��������
    DWORD               rdownIndex;     // ϵͳ��ť�Ҽ���������
    RECT                rcBtn;          // ����ϵͳ��ťλ��
    RECT                rcTitle;        // ������λ��
    RECT                rcIcon;         // ͼ��λ��
    RECT                rcCaption;      // ������, ϵͳͼ��, ϵͳ��ť����ռ�õ�λ��

    DWORD               style;          // ������ʽ
    DWORD               styleEx;        // ������չ��ʽ

    bool                isZoomed;       // �Ƿ����
    bool                isActivation;   // �Ƿ��Ǽ���״̬
    bool                isThickFrame;   // �Ƿ��пɵ��߿�
    bool                reserve;        // ����
    DWORD               freeFlags;      // �ͷŵı�ʶ, ���ݱ�ʶ�����ͷŵ�����, ����, ��ˢ, ����
    BOOL                showImage;      // ������ʾʹ��, ���ڼ���ʱ����ı�, ���ڽ�����ʾ��ʱ����Ҫ��ʱ�Ӹı�
    int                 showCount;      // ������ʾ����, Ϊ0��ʱ�����ʾ��ǰ������߲����������
    DRAW_EVENT_STRUCT<PFN_SysBtnClick>  pfnSysBtnClick;     // ϵͳ��ť����¼�
    DRAW_EVENT_STRUCT<PFN_SysDraw>      pfnDraw;            // �滭�¼�, ͼ��, ����, ϵͳ��ť


    RECT                rcWindow;       // GetWindowRect ��ֵ, ��WINDOWPOSCHANGE���¼��¼�¼
    RECT                rcClient;       // GetClientRect ��ֵ, ��WINDOWPOSCHANGE���¼��¼�¼
    CShadow*            shadow;         // ��Ӱ����

    LPEX_IMAGE          hImageShdow;    // ��Ӱ���ڼ���ʱ��ͼƬ
    LPEX_IMAGE          hImageShdow_nf; // ��Ӱ����ȡ������ʱ��ͼƬ



}*LPDRAW_WINDOW_DATA;


