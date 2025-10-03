#pragma once
#include <windows.h>
#include <comdef.h>


#define IDRAW_CALL __stdcall    // �Ӿ���ӿڵĵ���Լ��
DECLARE_HANDLE(LPOBJSTRUCT);
DECLARE_HANDLE(LPREGISTER_MESSAGE_INFO);
DECLARE_HANDLE(LPREGISTER_EVENT_INFO);
DECLARE_HANDLE(LPIDRAW_INFO);
DECLARE_HANDLE(ICTinyXml2_XMLNode);
DECLARE_HANDLE(IDRAW_HRESOURCE);
DECLARE_HANDLE(LPWINDOW_ICON_DATA);

#define __DEF_TYPE(_s) struct _s; typedef _s* P##_s, *LP##_s
__DEF_TYPE(CANVAS);
__DEF_TYPE(EX_BRUSH);
__DEF_TYPE(EX_PEN);
__DEF_TYPE(EX_IMAGE);
__DEF_TYPE(EX_FONT);
__DEF_TYPE(EX_PATH);
__DEF_TYPE(EX_RGN);

struct EX_IMAGELOCK;


// �ӿ�_IDE����༭�����
class IEIDE_CodeManage;
struct RECT_F;
typedef DWORD ARGB;
typedef void* ID2D1Image;
typedef LRESULT (IDRAW_CALL* pfn_SubClassWndProc)   (HWND, UINT, WPARAM, LPARAM, LPOBJSTRUCT);
typedef DWORD   (IDRAW_CALL* pfn_Register_Event)    (UINT, LONG_PTR, LPVOID);
typedef int     (CALLBACK* pfn_MenuExt_Command)     (int id, LPARAM param);
typedef int     (CALLBACK* pfn_IDraw_Callback)      (UINT, WPARAM, LPARAM);
typedef bool    (CALLBACK* pfn_CalcScroll)          (void* pScroll, LPOBJSTRUCT pData, RECT* prc);
typedef bool    (CALLBACK* pfn_DrawScroll)          (void* pScroll, LPOBJSTRUCT pData, bool isVScroll, HDC hdc, int type, RECT_F* rcDraw);
typedef bool    (CALLBACK* pfn_GetScrollRect)       (void* pScroll, LPOBJSTRUCT pData, bool isVScroll, RECT* prc);
typedef void    (CALLBACK* pfn_EWindowThreadCallback)(int);


// ö�ٳ�����ʼ

enum STATE
{
    NORMAL                  = 0X00000000,    // ����
    BAN                     = 0X00000001,    // ��ֹ
    SELECTED                = 0X00000002,    // ѡ��
    FOCUS                   = 0X00000004,    // ����
    DOWN                    = 0X00000008,    // ����
    CHECKED                 = 0X00000010,    // ѡ��
    INDETERMINATE           = 0X00000020,    // ��ѡ��
    READONLY                = 0X00000040,    // ֻ��
    HOT                     = 0X00000080,    // ��ȼ
    DEF                     = 0X00000100,    // Ĭ��
    SUBSHOW                 = 0X00000200,    // ����Ŀ����
    SUBHIDE                 = 0X00000400,    // ����Ŀ����
    BUSYING                 = 0X00000800,    // ��æ��
    HOVER                   = 0X00001000,    // ����
    ROLLING                 = 0X00002000,    // ������
    EFFECTING               = 0X00004000,    // ������
    HIDE                    = 0X00008000,    // ����
    ZHANWEI1                = 0X00010000,    // ռλ

    ALLOWMODIFYSIZE         = 0X00020000,    // �����޸ĳߴ�
    ALLOWDRAG               = 0X00040000,    // �����϶�
    ZHANWEI2                = 0X00080000,    // ռλ
    ALLOWFOCUS              = 0X00100000,    // ������
    ALLOWSELECT             = 0X00200000,    // ����ѡ��
    ALLOWHREFHOT            = 0X00400000,    // �������ӵ�ȼ
    ALLOWHREFEND            = 0X00800000,    // ���������ѷ���
    ALLOWMULTI              = 0X01000000,    // �����ѡ
    ZHANWEI3                = 0X02000000,    // ռλ
    ZHANWEI4                = 0X04000000,    // ռλ
    ZHANWEI5                = 0X08000000,    // ռλ
    ZHANWEI6                = 0X10000000,    // ռλ
    PASSWORD                = 0X20000000,    // ����ģʽ
};
typedef LPWINDOW_ICON_DATA(IDRAW_CALL* pfn_GetWindowIcon)(HWND hWnd, IDRAW_HRESOURCE hResource, LPCWSTR pszID, void* pArg, RECT_F* prc, STATE state, UINT dpi);
// ��ť���ػ�ʱ�Ļص�����
typedef int(IDRAW_CALL* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);

// ��ǰMDIClient��������ʾ�Ĵ�����ʲô����
enum MDICLIENT_TYPE : int
{
    MDICLIENT_TYPE_NONE     = -1,   // û�д���
    MDICLIENT_TYPE_CODE     = 0,    // ���봰��
    MDICLIENT_TYPE_IDE      = 1,    // ide�����, ���������
    MDICLIENT_TYPE_HONE     = 2,    // ��ʼҳ
};

// ��ǰ����༭������ʲô����
enum MDICLIENT_CODETYPE : int
{
    MDICLIENT_CODETYPE_UNDEFINE                 = 0,   // δ����
    MDICLIENT_CODETYPE_WINDOW                   = 1,   // ���������
    MDICLIENT_CODETYPE_ASSEMBLY                 = 2,   // ��ͨ����
    MDICLIENT_CODETYPE_ASSEMBLY_WINDOW          = 3,   // ���ڳ���
    MDICLIENT_CODETYPE_CLASS                    = 4,   // ��
    MDICLIENT_CODETYPE_GLOBALVAR                = 5,   // ȫ�ֱ���
    MDICLIENT_CODETYPE_STRUCT                   = 6,   // ��������
    MDICLIENT_CODETYPE_DLL                      = 7,   // DLL������
    MDICLIENT_CODETYPE_CONST                    = 8,   // ����
    MDICLIENT_CODETYPE_IMAGE                    = 9,   // ͼƬ��Դ
    MDICLIENT_CODETYPE_SOUND                    = 10,  // ������Դ
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

// ��������, �Ӵ��������ķ����л�ȡ�Ĵ�������
// CODELINETYPE = ����������
// CODETYPE = ��������
// CODEOFFSET = ��������ƫ������
enum class ECODETYPE : int
{
    CODELINETYPE_STRUCT                     = 5,    // ��������
    CODELINETYPE_STRUCTMEMBER               = 6,    // �������ͳ�Ա
    CODELINETYPE_GLOBALVAR                  = 7,    // ȫ�ֱ���
    CODELINETYPE_ASSEMBLY                   = 8,    // ����
    CODELINETYPE_ASSEMBLYVAR                = 9,    // ���򼯱���
    CODELINETYPE_FUN                        = 10,   // �ӳ���
    CODELINETYPE_FUNARG                     = 11,   // �ӳ������
    CODELINETYPE_FUNVAR                     = 12,   // �ӳ������
    CODELINETYPE_FUNCODE                    = 13,   // �ӳ������
    CODELINETYPE_CONST                      = 14,   // ����
    CODELINETYPE_IMAGE                      = 15,   // ��ԴͼƬ
    CODELINETYPE_SOUND                      = 16,   // ��Դ����

    CODETYPE_UNDEFINE                       = 0,    // δ����
    CODETYPE_ASSEMBLY                       = 1,    // ����
    CODETYPE_STRUCT                         = 2,    // ��������
    CODETYPE_GLOBALVAR                      = 3,    // ȫ�ֱ���
    CODETYPE_DLL                            = 4,    // DLL������
    CODETYPE_WINDOW                         = 5,    // ���������
    CODETYPE_CONST                          = 6,    // ����
    CODETYPE_IMAGE                          = 7,    // ͼƬ��Դ
    CODETYPE_SOUND                          = 8,    // ������Դ

    CODEOFFSET_STRUCT_NAME                  = 261,  // ������������
    CODEOFFSET_STRUCT_PUBLIC                = 263,  // �������͹���
    CODEOFFSET_STRUCT_REMARKS               = 262,  // �������ͱ�ע
    CODEOFFSET_STRUCT_MEMBER_NAME           = 313,  // �������� ��Ա ����
    CODEOFFSET_STRUCT_MEMBER_TYPE           = 314,  // �������� ��Ա ����
    CODEOFFSET_STRUCT_MEMBER_REFERENCE      = 317,  // �������� ��Ա ��ַ
    CODEOFFSET_STRUCT_MEMBER_ARRAY          = 315,  // �������� ��Ա ����
    CODEOFFSET_STRUCT_MEMBER_REMARKS        = 316,  // �������� ��Ա ��ע

    CODEOFFSET_GLOBALVAR_NAME               = 367,  // ȫ�ֱ��� ����
    CODEOFFSET_GLOBALVAR_TYPE               = 368,  // ȫ�ֱ��� ����
    CODEOFFSET_GLOBALVAR_ARRAY              = 369,  // ȫ�ֱ��� ����
    CODEOFFSET_GLOBALVAR_PUBLIC             = 371,  // ȫ�ֱ��� ����
    CODEOFFSET_GLOBALVAR_REMARKS            = 370,  // ȫ�ֱ��� ��ע


    CODEOFFSET_ASSEMBLY_NAME                = 580,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLY_BASECLASS           = 582,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLY_PUBLIC              = 573,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLY_REMARKS             = 581,  // ���򼯱��� ��ע

    CODEOFFSET_ASSEMBLYVAR_NAME             = 632,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLYVAR_TYPE             = 633,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLYVAR_ARRAY            = 634,  // ���򼯱��� ����
    CODEOFFSET_ASSEMBLYVAR_REMARKS          = 635,  // ���򼯱��� ��ע

    CODEOFFSET_FUN_NAME                     = 686,  // �ӳ�������
    CODEOFFSET_FUN_RETURNTYPE               = 687,  // �ӳ��� ��������
    CODEOFFSET_FUN_PUBLIC                   = 690,  // �ӳ��� ����
    CODEOFFSET_FUN_EPACKAGE                 = 688,  // �ӳ��� �װ�
    CODEOFFSET_FUN_REMARKS                  = 689,  // �ӳ��� ��ע

    CODEOFFSET_FUNARG_NAME                  = 740,  // �ӳ������ ����
    CODEOFFSET_FUNARG_TYPE                  = 741,  // �ӳ������ ����
    CODEOFFSET_FUNARG_REFERENCE             = 742,  // �ӳ������ �ο�
    CODEOFFSET_FUNARG_OPTIONAL              = 743,  // �ӳ������ �ɿ�
    CODEOFFSET_FUNARG_ARRAY                 = 744,  // �ӳ������ ����
    CODEOFFSET_FUNARG_REMARKS               = 745,  // �ӳ������ ��ע

    CODEOFFSET_FUNVAR_NAME                  = 796,  // �ӳ������ ����
    CODEOFFSET_FUNVAR_TYPE                  = 797,  // �ӳ������ ����
    CODEOFFSET_FUNVAR_STATIC                = 798,  // �ӳ������ ��̬
    CODEOFFSET_FUNVAR_ARRAY                 = 799,  // �ӳ������ ����
    CODEOFFSET_FUNVAR_REMARKS               = 800,  // �ӳ������ ��ע


    CODEOFFSET_CODE                         = 851,  // �ӳ������



};

enum SCROLL_DRAWTYPE : int
{
    SCROLL_DRAWTYPE_BACK    = 0x00,     // �滭����, ������յ���dc���ڴ�dc
    SCROLL_DRAWTYPE_BTN1    = 0x01,     // �滭�������İ�ť1, ������յ���dc���ڴ�dc
    SCROLL_DRAWTYPE_BTN2    = 0x02,     // �滭�������İ�2, ������յ���dc���ڴ�dc
    SCROLL_DRAWTYPE_THUMB   = 0x03,     // �滭�������Ļ���, ������յ���dc���ڴ�dc
    SCROLL_DRAWTYPE_ALERT   = 0x05,     // �滭����, ������յ���HDC�Ǵ��ڵ�hdc, ���յ���λ���Ǽ�������п�����Ҫ�滭��λ��, ��������¼���ʱ��������Ѿ��滭�����dc����
};

// ������ʽ
enum class FONTSTYLE
{
    FontStyleRegular    = 0,    // ����, Ĭ��
    FontStyleBold       = 1,    // ����
    FontStyleItalic     = 2,    // б��
    FontStyleBoldItalic = 3,    // �����б��
    FontStyleUnderline  = 4,    // �»���
    FontStyleStrikeout  = 8     // ɾ����
};

// ö�ٳ�������



// �������Ϳ�ʼ

// ���ֲ����Ϣ�ṹ
typedef struct PLUGIN_INFO
{
    DWORD cbSize;           // �ṹ�ߴ�
    LPCSTR name;            // �������
    LPCSTR remarks;         // ����
    LPCSTR version;         // �汾
    LPCSTR author;          // ����
    DWORD  Versionlimit;    // �汾����
    void* Dependence;       // �������

    LPCSTR  guid;                           // ���Ψһ��ʶ��
    pfn_MenuExt_Command pfnMenuExt_Command; // �˵��������¼�
    pfn_IDraw_Callback  pfnCallback;        // ���Ӿ���ͨѶ�Ļص�����
}*LPPLUGIN_INFO;

typedef struct IDRAW_STRUCT_SCROLL_DATA
{
    DWORD cbSize;       // �ṹ�ߴ�, ����ǰ��Ҫ�ȸ������Ա��ֵ
    float btn1Size;     // ��ť1�Ŀ��, ��������߰�ť���, �����Ƕ��߰�ť�߶�
    float btn2Size;     // ��ť2�Ŀ��, �������ұ߰�ť���, �����ǵױ߰�ť�߶�
    float btnThumbSize; // ������, ��������������������Ŀ��, �����Ǹ߶�, �����ǿ��
}*LPIDRAW_STRUCT_SCROLL_DATA;




typedef struct IDRAW_STRUCT_WINDOW_ARGUMENT
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

}*LPIDRAW_STRUCT_WINDOW_ARGUMENT;


// �滭�Ľṹ
typedef struct IDRAW_STRUCT_DRAWSTRUCT
{
    HWND            hWnd;       // ���ھ��
    LPCANVAS        hCanvas;    // �������
    DRAW_STAGE      stage;      // �滭�׶�
    const RECT_F*   rc;         // �滭�ľ���λ��
    int             id;         // �滭ϵͳ��ťʱʹ��, �����ʾ��ǰ�滭�İ�ťID
    STATE           state;      // ״̬, ϵͳ��ťʹ��
    LPARAM          param;      // ��Ŀ��ֵ, �滭�¼��󶨵���Ŀ��ֵ
    LPCWSTR         lpszTitle;  // �滭����ʱʹ�õı����ı�
    LPEX_IMAGE      hIcon;      // �滭ϵͳͼ��ʹ�õ�ͼ��
    LPEX_BRUSH      hbrText;    // �滭����ʹ�õĻ�ˢ

}*LPIDRAW_STRUCT_DRAWSTRUCT;

// ϵͳ��ť������¼�
typedef bool(CALLBACK* PFN_SysBtnClick)(int id, LPARAM btnParam, BOOL isRButton, LPARAM param);
// ϵͳ��ť���滭�¼�
typedef bool(CALLBACK* PFN_SysDraw)(LPIDRAW_STRUCT_DRAWSTRUCT dps);

typedef struct RECT_F
{
    float left;     // ���
    float top;      // ����
    float right;    // �ұ�
    float bottom;   // �ױ�
}*LPRECT_F;
typedef const RECT_F* LPCRECT_F;

typedef struct RECTROUNDED_F : RECT_F
{
    float radiusX;  // Բ��
    float radiusY;  // Բ��
}*LPRECTROUNDED_F;
typedef const RECTROUNDED_F* LPCRECTROUNDED_F;

typedef struct POINT_F
{
    float x;    // �����x
    float y;    // �����y
}*LPPOINT_F;
typedef const POINT_F* LPCPOINT_F;

typedef struct ELLIPSE_F : POINT_F
{
    float radiusX;  // �뾶���
    float radiusY;  // �뾶�߶�
}*LPELLIPSE_F;
typedef const ELLIPSE_F* LPCELLIPSE_F;

typedef struct LINEPOINT_F : POINT_F
{
    float x1;   // �ڶ�����������
    float y1;   // �ڶ�����������
}*LPLINEPOINT_F;
typedef const LINEPOINT_F* LPCLINEPOINT_F;

// ͼ�������ṹ
typedef struct EX_IMAGELOCK
{
    UINT width;     // ����ͼ��Ŀ��, �ڲ�ʹ��, �������޸�
    UINT height;    // ����ͼ��ĸ߶�, �ڲ�ʹ��, �������޸�
    UINT stride;    // �ڲ�ʹ��, �������޸�
    void* pScan0;   // �ڲ�ʹ��, �������޸�
    void* pLock;    // �ڲ�ʹ��, �������޸�
}*PEX_IMAGELOCK, * LPEX_IMAGELOCK;

// MDIClient ���ھ���ṹ
struct EWINDOW_MDICHILD
{
    HWND hMDIClient1;   // MDI�µĵ�һ�㴰��
    HWND hMDIClient2;   // MDI�µĵڶ��㴰��
    HWND hMDIClient3;   // MDI�µĵ����㴰��, ���Ը�����������ж��Ƿ�Ϊ��ʼҳ������������ڻ��ߴ�����
    HWND hMDIClient4;   // MDI�µĵ��Ĳ㴰��, ����Ǵ�����, ������ھ��Ǵ��봰��
    HWND hVScroll;      // ���������
    HWND hHScroll;      // ���������
};

// ���������ʼ�ͽ����滭ʹ�õĽṹ
struct BEGIN_END_PAINT_STRUCT : PAINTSTRUCT
{
    RECT rc;            // ����ͻ�������
    int cxClient;       // �ͻ������
    int cyClient;       // �ͻ����߶�
    LPVOID pFont;       // �ڲ�ʹ��, �������޸�
    LPVOID hFont;       // �ڲ�ʹ��, �������޸�
    LPVOID md;          // �ڲ�ʹ��, �������޸�
    BOOL isDxRender;    // �ڲ�ʹ��, �������޸�
    LPOBJSTRUCT pData;  // ��������
};

// ����˵��ߴ��¼��ṹ
typedef struct IRE_STRUCT_MENU_CALCSIZE
{
    int width;      // �˵����
    int height;     // �˵��߶�
    LPARAM param;   // ��Ӳ˵�ʱ�Ĳ˵���ֵ, ����ʽ�˵����ֵΪ0
}*LPIRE_STRUCT_MENU_CALCSIZE;


// �������滭�¼��ṹ
typedef struct IRE_STRUCT_MENU_DRAW
{
    LPCANVAS hCanvas;   // dxʹ�õĻ滭���, ��Ҫgdi/gdi+�Ļ���ʹ�� iDraw_canvas_getdc() ��ȡ�滭DC
    int width;          // ���
    int height;         // �߶�
    int id;             // �˵�ID
    UINT type;          // �˵�����
    LPCWSTR text;       // �˵�����
    int state;          // �˵�״̬, һ������ֵ�����
}*LPIRE_STRUCT_MENU_DRAW;


typedef struct IRE_STRUCT_GETICON
{
    HWND hWnd;          // Ҫ��ȡͼ��Ĵ��ھ��, ����ǲ˵����ֵΪ0
    LPVOID hResource;   // �󶨴���ʱ����Դ���
    LPCWSTR pszID;      // ��Ŀ�ı�
    LPVOID pArg;        // ����ǲ˵�, ���ֵΪ�˵�ID, ����������������ݲ�ͬ����, ������鿴�����ĵ�(δд....)
    RECT_F* prc;        // ָ�� IDRAW_RECT_F �ṹָ��, ��Ҫ��ͼ��λ��д�������ַ
    int state;          // ��ǰҪ��ȡ��״̬ͼ��, Ŀǰֻ�� STATE::NORMAL, STATE::HOT, STATE::DOWN, STATE::BAN �⼸��״̬
    int* pIconType;     // ͼ����Դ, 0=idָ���ͼ��, 1=Ĭ��ͼ��, Ŀǰ��ʱ��ʹ���������

}*LPIRE_STRUCT_GETICON;


// ѡ��гߴ�ı��¼��ṹ
typedef struct IRE_STRUCT_TAB_SIZE
{
    int index;      // ��ǰѡ����
    int direction;  // ��ǰѡ��з���
    RECT rc;        // ѡ����ڵ���������λ��, ���λ���Ѿ���ѡ��е�ͷ�����˳���, �ڲ���������ṹ����λ��, ���Ա��޸�
    HWND hWnd;      // ѡ��еľ��
}*LPIRE_STRUCT_TAB_CHANGE;

typedef struct IRE_STRUCT_MDICLIENT_MOVE_EVENT
{
    int windowType;     // ��ǰMDI�´��ڵ�����, ��λ -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ ��ǰMDIClient ����������ʾʲô��������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
    HWND hChild1;       // MDI�����µĵ�һ�㴰��
    HWND hChild2;       // MDI�����µĵڶ��㴰��
    HWND hChild3;       // MDI�����µĵ����㴰��
    HWND hChild4;       // MDI�µĵ��Ĳ㴰��, ��������������, ��������ھ������������, ��ʼҳ��Ϊ�����Դ�볬��, ���������Ǵ��봰��
    RECT rcChild1;      // MDI�µ�һ�㴰�ڵ��ƶ�λ��, ����MDIClient������ڵ�λ���޸�
    RECT rcChild2;      // MDI�µڶ��㴰�ڵ��ƶ�λ��, ����MDIClient�µĵ�һ�㴰�ڵ�λ���޸�
    RECT rcChild3;      // MDI�µ����㴰�ڵ��ƶ�λ��, ����MDIClient�µĵڶ��㴰�ڵ�λ���޸�
    RECT rcChild4;      // MDI�µ��Ĳ㴰�ڵ��ƶ�λ��, ����MDIClient�µĵ����㴰�ڵ�λ���޸�
}*LPIRE_STRUCT_MDICLIENT_MOVE_EVENT;

// �����л�ѡ���ѡ�б��ı��¼��ṹ
typedef struct IRE_STRUCT_TAB_SELCHANGE
{
    int index;      // �л��������, ���Ϊ-1Ҫô�Ǵ����޸�Ҫô����ѡ��п���
    int indexOld;   // �л�ǰ������
    HWND hWnd;      // �л���ѡ�����Ĵ��ھ��, ������/״̬�д��ݵ�ǰ�����¼���ѡ��о��
    HWND hWndOld;   // �л�ǰѡ�����Ĵ��ھ��, ������/״̬����ʱû��ʹ�������Ա
}*LPIRE_STRUCT_TAB_SELCHANGE;


typedef struct IRE_STRUCT_HOOKARG
{
    int code;           // ������ο� SetWindowsHookEx �ص�����˵��
    WPARAM wParam;      // ������ο� SetWindowsHookEx �ص�����˵��
    LPARAM lParam;      // ������ο� SetWindowsHookEx �ص�����˵��
    LRESULT ret;        // �����¼��ķ���ֵ
    HHOOK hhk;          // ���Ӿ��
}*LPIRE_STRUCT_HOOKARG;


// �������滭�¼��ṹ
typedef struct IRE_STRUCT_BACKGROUND_DRAW
{
    LPCANVAS hCanvas;   // dxʹ�õĻ滭���, ��Ҫgdi/gdi+�Ļ���ʹ�� iDraw_canvas_getdc() ��ȡ�滭DC
    int cxClient;       // ���
    int cyClient;       // �߶�
}*LPIRE_STRUCT_BACKGROUND_DRAW;

// ��Ϣ�¼�����Ϣ����
typedef struct IRE_STRUCT_MSG
{
    HWND hWnd;          // ������Ϣ�Ĵ��ھ��
    UINT message;       // ��Ϣֵ
    WPARAM wParam;      // ����1
    LPARAM lParam;      // ����2
}*LPIRE_STRUCT_MSG;

// ������״̬�ı��¼��ṹ
typedef struct IRE_STRUCT_TOOLSTATE_CHANGE
{
    HWND hWnd;  // ���ı�Ĺ��������
    int id;     // �����ı�״̬�İ�ťID
    int state;  // ״̬
}*LPIRE_STRUCT_TOOLSTATE_CHANGE;

// MDI�������¼�����
typedef struct IRE_STRUCT_MDICREATE
{
    HWND hWnd;                  // MDI���ھ��
    LPMDICREATESTRUCTA mcs;     // MDI�������ڽṹ, �����Դ��ڶ���A��, ������յľ���A��Ľṹ
}*LPIRE_STRUCT_MDICREATE;

// �����кű��ı��¼�����
typedef struct IRE_STRUCT_EIDE_SELCHANGEDINFO
{
    IEIDE_CodeManage* pCodeManage;  // ���������ָ��
    int line;                       // ��ǰѡ���к�
    int oldLine;                    // ѡ�е�ǰ��֮ǰ���к�
    int oldLineCol;                 // ѡ�е�ǰ��֮ǰ���к�
}*LPIRE_STRUCT_EIDE_SELCHANGEDINFO;

// �Զ����صĴ��ڱ������ߴ�
typedef struct IRE_STRUCT_AUTOHIDE_MOVE
{
    HWND hWnd;      // ���ƶ��Ĵ���
    RECT rc;        // �ƶ���λ��
}*LPIRE_STRUCT_AUTOHIDE_MOVE;


// ����˵�����չ�˵���, ������Ա���Բο�MSDN�� InsertMenu ���API
typedef struct IDC_STRUCT_MENUEXT_INSERT
{
    LPCSTR guid;            // ���뵽�Ĳ��guid��
    UINT uPosition;         // λ�û�ID
    UINT uFlags;            // �˵���ʶ
    UINT_PTR uIDNewItem;    // �˵�ID
    LPCWSTR lpNewItem;      // �˵�����
    LPARAM param;           // �˵���ֵ
}*LPIDC_STRUCT_MENUEXT_INSERT;

// �������ͽ���




// ͼƬ����
struct EX_IMAGE
{
public:

    // �ͷŵ�ǰ��ͼ��, �ͷź�����ʹ���������, ��������������������ָ��
    virtual int Release() = 0;

    // ��ȡͼ����
    virtual UINT GetWidth() = 0;

    // ��ȡͼ��߶�
    virtual UINT GetHeight() = 0;

    // ��ȡͼ��ߴ�, ����ͼ��ߴ�, ��λ=���, ��λ=�߶�
    virtual DWORD GetSize(UINT* cx = 0, UINT* cy = 0) = 0;

    // �ӵ�ǰͼƬ����һ���µ�ͼƬ����, �����µ�ͼƬ���
    // 4�������ֱ�Ϊ��Ҫ���Ƶ��󶥿��, ������/�߶ȵ���0, ��ʹ��ͼ��Ŀ��/�߶�
    virtual LPEX_IMAGE copy(UINT x = 0, UINT y = 0, UINT width = 0, UINT height = 0) = 0;

    // ����λͼ, �ɹ�����true, ʧ�ܷ���false, ��ʹ��ʱ������� unlock �ͷ�����
    // x,y,width,height = �󶥿��, ���Ϊ0ʱ���Զ���ȡͼƬ��С
    // flags = 1=��, 2=д, 3=��д
    // lockData = ����ͼ������, ����ͼ��ʱ��Ҫʹ��
    virtual bool lock(UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData) = 0;

    // ȡ������ͼƬ
    // lockData = ����ͼ������, �� lock ���
    virtual bool unlock(EX_IMAGELOCK* lockData) = 0;

    // ȡͼ��֡��
    virtual UINT GetFrameCount() = 0;

    // ���õ�ǰ�֡
    // index = �ڼ�֡
    virtual bool SelectActiveFrame(UINT index) = 0;

    // �ѵ�ǰͼƬ���󱣴��ͼƬ����, ���ص�ͼƬ���ݲ�ʹ��ʱ��Ҫ���ñ������ free ���ͷ�
    // pSize = ���շ���ͼƬ���ݵĴ�С
    // ���ҵ�, Ҫ�����λ��, Ĭ��ΪͼƬ�����С
    virtual LPBYTE save(SIZE_T* pSize, UINT left = 0, UINT top = 0, UINT right = 0, UINT bottom = 0) = 0;

    // �ѻ滭��������ͼƬ����, ���ص�ͼƬ���ݲ�ʹ��ʱ��Ҫ���ñ������ free ���ͷ�
    // hCanvas = �滭���
    // pSize = ���շ���ͼƬ���ݵĴ�С
    // ���ҵ�, Ҫ�����λ��, ���/�߶�Ϊ0��ʹ�û滭����Ŀ��/�߶�
    virtual LPBYTE save(const LPCANVAS hCanvas, SIZE_T* pSize, UINT left = 0, UINT top = 0, UINT right = 0, UINT bottom = 0) = 0;

    // �ѵ�ǰͼ��ת��GDIλͼ, ��ʹ��ʱ��Ҫ����win32API DeleteObject() �ͷ�
    virtual HBITMAP ToGdiBitmap() = 0;

    // �ͷ� save ���صĵ�ַ
    virtual void free(LPBYTE p) = 0;

};


struct EX_RGN
{
    // ��������, �ͷź�����ʹ���������, ��������������������ָ��
    virtual int Release() = 0;

    // ����ָ�������Ƿ���������
    virtual bool Hittest(float x, float y) = 0;
};

struct EX_FONT
{
    // ��������, �ͷź�����ʹ���������, ��������������������ָ��
    virtual int Release() = 0;

    // �ַ��߶�ת�����С, �ַ��߶�һ���� LOGFONT �� lfHeight, �����Сһ���������Ե������С
    virtual int height2size(int height) = 0;

    // �����Сת�ַ��߶�, �ַ��߶�һ���� LOGFONT �� lfHeight, �����Сһ���������Ե������С
    virtual int size2height(int size) = 0;

    // ��ȡ��ǰ�����LOGFONTW �ṹ����
    // lplf = LOGFONTW �ṹָ��, �ڲ��������д������ṹ��
    virtual bool GetLogfont(LPLOGFONTW lplf) = 0;

    // ��ȡ��ǰ��������GDI����, ��������� DeleteObject() �����ͷ�
    virtual HFONT GetFont() = 0;

};

struct EX_PATH
{
    // ����·��, �ͷź�����ʹ���������, ��������������������ָ��
    virtual int Release() = 0;

    // ��ָ���㿪ʼһ����·��
    // ppt = �����ָ��, Ϊ0��ʹ��
    virtual bool BeginFigure(const LPCPOINT_F pt = 0) = 0;

    // ������ǰ·��
    virtual bool EndFigure() = 0;

    // ����Ϊ��·��
    virtual bool Reset() = 0;

    // ��ӻ�
    virtual bool AddArc(float x1, float y1, float x2, float y2, float radiusX, float radiusY, bool fClockwise) = 0;

    // ��ӱ���������
    // pt1, pt2, pt3, pt4 = 4�������
    virtual bool AddBezier(const POINT_F& pt1, const POINT_F& pt2, const POINT_F& pt3, const POINT_F& pt4) = 0;

    // ���ֱ��
    // pt1, pt2 = ��ʼ������ͽ���������
    virtual bool AddLine(const POINT_F& pt1, const POINT_F& pt2) = 0;

    // ��Ӿ���
    // rc = ����
    virtual bool AddRect(const RECT_F& rc) = 0;

    // ���Բ�Ǿ���
    // rc = ����
    // radiusTopLeft = ���Ͻ�Բ�Ƕ���
    // radiusTopRight = ���Ͻ�Բ�Ƕ���
    // radiusBottomLeft = ���½�Բ�Ƕ���
    // radiusBottomRight = ���½�Բ�Ƕ���
    virtual bool AddRoundedRect(const RECT_F& rc, float radiusTopLeft, float radiusTopRight, float radiusBottomLeft, float radiusBottomRight) = 0;

    // ��·��, �򿪺������·�������
    virtual bool Open() = 0;

    // �ر�·��, �����Ϻ�, ʹ��·��ǰ��ر�·��
    virtual bool Close() = 0;

    // ȡ·���߽����
    // prc = ����·���߽����
    virtual bool GetBounds(LPRECT_F rc) = 0;

    // ���������Ƿ���·����Χ��
    // pt = �����
    virtual bool Hittest(const POINT_F& pt) = 0;


};

// ��ˢ
struct EX_BRUSH
{
    // ���ٻ�ˢ, �ͷź�����ʹ���������, ��������������������ָ��
    virtual int Release() = 0;

    // ���ô�ɫ��ˢ�Ļ�ˢ��ɫ, �Ǵ�ɫ��ˢ������ʧ��
    // color = �µĻ�ˢ��ɫ
    virtual bool SetColor(ARGB argb) = 0;

    // ��ȡ��ɫ��ˢ����ɫ, ���ص���ARGB��ɫ
    virtual ARGB GetColor() = 0;

    // ���ý��仭ˢ��ʼλ��, GDI+��δ֧��
    // x, y = �µ���ʼλ��
    virtual bool SetStartPoint(float x, float y) = 0;

    // ���ý��仭ˢ����λ��, GDI+��δ֧��
    // x, y = �µĽ���λ��
    virtual bool SetEndPoint(float x, float y) = 0;

    // ��ȡ���仭ˢ��ʼλ��, GDI+��δ֧��
    // x, y = �����µ���ʼλ��ָ��, Ϊ0�򲻻�ȡ
    virtual bool GetStartPoint(float* x = 0, float* y = 0) = 0;

    // ��ȡ���仭ˢ����λ��, GDI+��δ֧��
    // x, y = �����µĽ���λ��ָ��, Ϊ0�򲻻�ȡ
    virtual bool GetEndPoint(float* x = 0, float* y = 0) = 0;

    // ���÷��佥�仭ˢ��������, GDI+��δ֧��
    // x, y = �µ����ĵ�λ��
    virtual bool SetCenter(float x, float y) = 0;

    // ��ȡ���佥�仭ˢ��������, GDI+��δ֧��
    // x, y = �����µ����ĵ�λ��ָ��, Ϊ0�򲻻�ȡ
    virtual bool GetCenter(float* x = 0, float* y = 0) = 0;

    // ���÷��佥��ԭ��ƫ��, GDI+��δ֧��
    // x, y = �µ�ƫ��λ��
    virtual bool SetGradientOriginOffset(float x, float y) = 0;

    // ��ȡ���佥��ԭ��ƫ��, GDI+��δ֧��
    // x, y = �����µ�ƫ��λ��ָ��, Ϊ0�򲻻�ȡ
    virtual bool GetGradientOriginOffset(float* x = 0, float* y = 0) = 0;

    // ���÷��佥�仭ˢ����뾶, GDI+��δ֧��
    // x = ���ĵ����뾶
    virtual bool SetRadiusX(float x) = 0;

    // ��ȡ���佥�仭ˢ����뾶, GDI+��δ֧��
    virtual float GetRadiusX() = 0;

    // ���÷��佥�仭ˢ����뾶, GDI+��δ֧��
    // y = ���ĵ�����뾶
    virtual bool SetRadiusY(float y) = 0;

    // ��ȡ���佥�仭ˢ����뾶, GDI+��δ֧��
    virtual float GetRadiusY() = 0;

};

// ����
struct EX_PEN
{
    // ���ٻ���, �ͷź�����ʹ���������, ��������������������ָ��
    virtual int Release() = 0;

    // ���û��ʵ���ɫ
    // color = �µĻ���ARGB��ɫ
    virtual bool SetColor(ARGB argb) = 0;

    // ��ȡ���ʵ���ɫ
    virtual ARGB GetColor() = 0;

    // ���û��ʵĿ��
    // width = ���ʿ��
    virtual bool SetWidth(float width) = 0;

    // ��ȡ���ʵĿ��
    virtual float GetWidth() = 0;

    // ���û���������ʽ
    // style = 0=ʵ��, 1=����{2, 2}, 2=����{0, 2}, 3=�㻮��{2, 2, 0, 2}, 4=��㻮��{2, 2, 0, 2, 0, 2}, 5=�Զ��� Gdiplus::DashStyle ���� D2D1_DASH_STYLE ������ö�ٳ����ĳ�Ա
    // dashes = ���ߵ�����, ����={2,2}, ����={0,2}, �㻮��={2,2,0,2}, ��㻮��={2,2,0,2,0,2}, �������Զ���
    // dashesCount = �����Ա��
    virtual float SetDashStyle(int style = 0, float* dashes = 0, int dashesCount = 0) = 0;

};

// �滭��Ϣ
struct CANVAS
{
public:

    virtual int Release() = 0;
    virtual HWND GetWindow() = 0;
    virtual UINT GetWidth() = 0;
    virtual UINT GetHeight() = 0;
    virtual bool GetTargetRender(LPVOID* ppObj) = 0;
    virtual HDC GetGdiDC() = 0;

    // ��ʼ�滭, �滭ǰ�������, ���ú�ſ��Խ��л滭����, �����滭ʱ������� EndDraw() ���ύ�滭���
    virtual bool BeginDraw() = 0;

    // �����滭, ���ػ滭�Ƿ��д���, ����д�����Ե��� ExGetLastError() ��ȡ������, ����������� D2DERR_RECREATE_TARGET ����Ҫ���´���D2D��Դ��
    virtual bool EndDraw() = 0;

    // ���µ��������ߴ�
    // cx = �µĻ������
    // cy = �µĻ����߶�
    virtual bool Resize(int cx, int cy) = 0;

    // ��ȡ�����ĳߴ�
    // pWidth = ���ջ�����ȵ�ָ��, Ϊ0�򲻽���
    // pHeight = ���ջ����߶ȵ�ָ��, Ϊ0�򲻽���
    virtual bool GetSize(UINT* width, UINT* height) = 0;

    // ��ȡ������Ӧ��GDI HDC
    // ֻ���� BeginDraw() �� EndDraw() ֮�����
    // �ڵ��� EndDraw() ǰ������� ReleaseDC() �����ͷ�, ���� EndDraw() ������ʧ��
    virtual HDC GetDC() = 0;

    // �ͷ� GetDC() ��ȡ��HDC, ����е��� GetDC(), ����������������ͷ�
    virtual void ReleaseDC() = 0;

    // �������
    // argb = �������ʱ����ARGB��ɫ, 0=͸��
    virtual bool clear(ARGB argb) = 0;

    // ִ�����й���Ļ�ͼ����
    virtual bool flush() = 0;

    // ���õ�ǰ��Ⱦ��ͼ��, ����֮��滭�����ᱣ�浽 image ������, ������GDI��λͼ
    // image = ͼ����
    virtual bool SetTarget(LPEX_IMAGE bitmap) = 0;

    // ��ԭ��ǰ��Ⱦͼ��
    virtual bool ReTarget() = 0;

    // ��ȡ��ǰ��Ⱦͼ����, ���صľ����ʹ��ʱ��Ҫ���� FreeTarget() �����ͷ�
    virtual LPEX_IMAGE GetTarget() = 0;

    // �ͷ� GetTarget() ���ص�ͼ����
    // image = GetTarget() ���ص�ֵ
    virtual void FreeTarget(LPEX_IMAGE img) = 0;



    // ƽ��
    // x, y = ƽ�Ƶĺ�����������
    virtual bool translate(float x, float y) = 0;
    // ����
    // x, y = ���ŵ������
    // scaleX, scaleY = ���ŵĺ����������
    virtual bool scale(float x, float y, float scaleX, float scaleY) = 0;
    // ��ת
    // x, y = ��ת�������
    // rotate = ��ת�ĽǶ�, ����������ת
    virtual bool rotate(float x, float y, float rotate) = 0;


    // ���Ƽ�ͼ��

    // �滭����
    // hPen = �߿򻭱�
    // prc = RECT_F ����ָ��
    virtual bool DrawRectangle(const LPEX_PEN hPen, const LPCRECT_F rcF) = 0;

    // ������
    // hBrush = ���Ļ�ˢ
    // prc = RECT_F ����ָ��
    virtual bool FillRectangle(const LPEX_BRUSH hBrush, const LPCRECT_F rcF) = 0;

    // �滭��Բ
    // hPen = �߿򻭱�
    // prc = ELLIPSE_F ��Բλ��ָ��, ��¼һ�����ĵ��ԭ�Ŀ�Ⱥ͸߶�
    virtual bool DrawEllipse(const LPEX_PEN hPen, const LPCELLIPSE_F rcF) = 0;

    // �����Բ
    // hBrush = ��仭ˢ
    // prc = ELLIPSE_F ��Բλ��ָ��, ��¼һ�����ĵ��ԭ�Ŀ�Ⱥ͸߶�
    virtual bool FillEllipse(const LPEX_BRUSH hBrush, const LPCELLIPSE_F rcF) = 0;

    // �滭Բ�Ǿ���
    // hPen = �߿򻭱�
    // prcRounded = RECTROUNDED_F Բ�Ǿ���ָ��
    virtual bool DrawRoundedRectangle(const LPEX_PEN hPen, const LPCRECTROUNDED_F rcRounded) = 0;

    // ���Բ�Ǿ���
    // hBrush = ��仭ˢ
    // prcRounded = RECTROUNDED_F Բ�Ǿ���ָ��
    virtual bool FillRoundedRectangle(const LPEX_BRUSH hBrush, const LPCRECTROUNDED_F rcRounded) = 0;

    // �滭ֱ��
    // hPen = ������ɫ
    // ppt = ��������
    virtual bool DrawLine(const LPEX_PEN hPen, const LPCLINEPOINT_F pt) = 0;

    // �滭·��
    // hPen = �߿򻭱�
    // path = ·��ָ��
    virtual bool DrawPath(const LPEX_PEN hPen, const LPEX_PATH path) = 0;

    // ���·��
    // hBrush = ��仭ˢ
    // path = ·��ָ��
    virtual bool FillPath(const LPEX_BRUSH hBrush, const LPEX_PATH path) = 0;


    // �滭ͼ��, ������ͼ��滭������ָ����λ��, �����ͼ���������
    // image = ͼ����
    // left = �滭�������ϵ����λ��
    // top = �滭�������ϵĶ���λ��
    // alpha = ͸����, 0=��ȫ͸��, 255=��ȫ��͸��
    virtual bool DrawImage(LPEX_IMAGE image, float left, float top, BYTE alpha) = 0;

    // �滭ͼ��, ������ͼ��滭������ָ����λ��, ���ָ���Ĵ�С��ͼ��Ĳ�ͬ, �������
    // image = ͼ����
    // left = �滭�������ϵ����λ��
    // top = �滭�������ϵĶ���λ��
    // right = �滭�������ϵ��ұ�λ��
    // bottom = �滭�������ϵĵױ�λ��
    // alpha = ͸����, 0=��ȫ͸��, 255=��ȫ��͸��
    virtual bool DrawImage(LPEX_IMAGE image, float left, float top, float right, float bottom, BYTE alpha) = 0;

    // �滭ͼ��, ��ͼ���ĳ��λ�û滭������ĳ��λ��, �ߴ粻ͬ������
    // image = ͼ����
    // dstLeft = �滭�������ϵ����λ��
    // dstTop = �滭�������ϵĶ���λ��
    // dstRight = �滭�������ϵ��ұ�λ��
    // dstBottom = �滭�������ϵĵױ�λ��
    // srcLeft = ��ͼ������λ��
    // srcTop = ��ͼ��Ķ���λ��
    // srcRight = ��ͼ����ұ�λ��
    // srcBottom = ��ͼ��ĵױ�λ��
    // alpha = ͸����, 0=��ȫ͸��, 255=��ȫ��͸��
    virtual bool DrawImage(LPEX_IMAGE image, float dstLeft, float dstTop, float dstRight, float dstBottom,
                           float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha) = 0;

    // �滭�Ź�ͼ��, �ߴ粻ͬ������
    // image = ͼ����
    // dstLeft = �滭�������ϵ����λ��
    // dstTop = �滭�������ϵĶ���λ��
    // dstRight = �滭�������ϵ��ұ�λ��
    // dstBottom = �滭�������ϵĵױ�λ��
    // srcLeft = ��ͼ������λ��
    // srcTop = ��ͼ��Ķ���λ��
    // srcRight = ��ͼ����ұ�λ��
    // srcBottom = ��ͼ��ĵױ�λ��
    // gridPaddingLeft = �Ź��������
    // gridPaddingTop = �Ź����򶥱�
    // gridPaddingRight = �Ź������ұ�
    // gridPaddingBottom = �Ź�����ױ�
    // alpha = ͸����, 0=��ȫ͸��, 255=��ȫ��͸��
    virtual bool DrawImage(LPEX_IMAGE image, float dstLeft, float dstTop, float dstRight, float dstBottom,
                           float srcLeft, float srcTop, float srcRight, float srcBottom,
                           float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom,
                           DWORD dwFlags, BYTE alpha) = 0;

    // �����ı��ߴ�
    // font = ������
    // text = ��Ҫ������ı�
    // textLen = �ı�����, ���Ϊ-1��text������\\0��β
    // textFormat = �ı������ʽ, �ο� DT_ ��ͷ����, �� win32API DrawTextһ��
    // layoutWidth = ���ֿ��, �����ı�����������ȼ�������, ������ͬ�߶Ⱦͻ᲻ͬ
    // layoutHeight = ���ָ߶�
    // pWidth = ���ռ�������ı����
    // pHeight = ���ռ�������ı��߶�
    virtual bool calctextsize(LPEX_FONT font, LPCWSTR text, size_t textLen,
                              DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight) = 0;

    // �滭�ı�
    // font = ������
    // hbrText = �ı���ˢ
    // text = ��Ҫ�滭���ı�
    // textLen = �ı�����, ���Ϊ-1��text������\\0��β
    // textFormat = �ı������ʽ, �ο� DT_ ��ͷ����, �� win32API DrawTextһ��
    // prc = �ı��滭��λ��
    virtual bool drawtext(LPEX_FONT font, LPEX_BRUSH hBrush, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc) = 0;

    // �滭�ı�
    // hCanvas = �������
    // font = ������
    // hbrText = �ı���ˢ
    // text = ��Ҫ�滭���ı�
    // textLen = �ı�����, ���Ϊ-1��text������\\0��β
    // textFormat = �ı������ʽ, �ο� DT_ ��ͷ����, �� win32API DrawTextһ��
    // prc = �ı��滭��λ��
    // iGlowsize = ��δʹ��, ����0
    // crShadow = ��δʹ��, ����0
    // lParam = ��δʹ��, ����0
    // prclayout = ��δʹ��, ����0
    // outrcText = ���ջ滭�ı�������
    virtual bool drawtextex(LPEX_FONT font, LPEX_BRUSH hBrush,
                            LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc,
                            DWORD iGlowsize, ARGB crShadow, LPDRAWTEXTPARAMS lParam, const RECT_F* prclayout, RECT_F* outrcText) = 0;

    // ��Ͽ���, �������ݵ�������
    // xoriginDest = ���������������λ��
    // yoriginDest = ���������������λ��
    // wDest = �����������Ŀ��
    // hDest = �����������ĸ߶�
    // sCanvas = ���ĸ������Ͽ���
    // xoriginSrc = �� sCanvas �����ϵ��ĸ�λ�ÿ�ʼ����
    // yoriginSrc = �� sCanvas �����ϵ��ĸ�λ�ÿ�ʼ����
    // wSrc = �� sCanvas �����ϵ��ĸ�λ�ÿ�ʼ����
    // hSrc = �� sCanvas �����ϵ��ĸ�λ�ÿ�ʼ����
    // Alpha = ͸����
    virtual bool Alphablend(float xoriginDest, float yoriginDest, float wDest, float hDest, const LPCANVAS sCanvas,
                            float xoriginSrc, float yoriginSrc, float wSrc, float hSrc, BYTE Alpha) = 0;

    // λ����, �������ݵ�������
    // x = ���������������λ��
    // y = ���������������λ��
    // cx = �����������Ŀ��
    // cy = �����������ĸ߶�
    // sCanvas = ���ĸ������Ͽ���
    // x1 = �� sCanvas �����ϵ��ĸ�λ�ÿ�ʼ����
    // y1 = �� sCanvas �����ϵ��ĸ�λ�ÿ�ʼ����
    // rop = ��դ��������, һ���� SRCCOPY, D2D��Ⱦ�����������, D2Dֻ֧�� SRCCOPY
    virtual bool bitblt(int x, int y, int cx, int cy, const LPCANVAS sCanvas, int x1, int y1, DWORD rop) = 0;

    // ��δʵ��, �벻Ҫ����
    virtual bool SetRgn(float left, float top, float right, float bottom, int mode = 0) = 0;

    // ���òü���, ���ú�滭������ֻ�������õ���������ʾ
    // left = �ü��������
    // top = �ü����Ķ���
    // right = �ü������ұ�
    // bottom = �ü����ĵױ�
    // mode = �ü���ģʽ, һ����0, �����ڲ���ʱ��û��ΪD2D��GDI+����ͳһ�ı�׼, D2D��Ⱦ��鿴D2D��صĳ���, GDI+�����GDI+�ĳ���
    virtual bool SetClip(float left, float top, float right, float bottom, int mode = 0) = 0;
    virtual bool ResetClip() = 0;

    // ���ÿ����ģʽ, 0=�رտ����, 1=����
    // �˺������� SetSmoothingMode, ����һģһ��, ���һ����Ϊ�˺�d2d����һ��
    virtual bool SetAntialiasMode(int mode, int* oldMode = 0) = 0;

    // ���ÿ����ģʽ, 0=�رտ����, 1=����
    // �˺������� SetAntialiasMode ʵ��, ����һģһ��, ���һ����Ϊ�˺�gdi+����һ��
    virtual bool SetSmoothingMode(int mode, int* oldMode = 0) = 0;

    // �������忹���ģʽ, 0=Ĭ��, 1=ClearType�����(�������), 2=�Ҷȿ����, 3=��ʹ�ÿ����
    // �˺������� SetTextRenderingHint, ����һģһ��, ���һ����Ϊ�˺�d2d����һ��
    virtual bool SetTextAntialiasMode(int mode, int* oldMode = 0) = 0;
    // �������忹���ģʽ, 0=Ĭ��, 1=ClearType�����(�������), 2=�Ҷȿ����, 3=��ʹ�ÿ����
    // �˺������� SetTextAntialiasMode ʵ��, ����һģһ��, ���һ����Ϊ�˺�gdi+����һ��
    virtual bool SetTextRenderingHint(int mode, int* oldMode = 0) = 0;



    //inline operator HDC() const { return hdc; }
    //inline operator HBITMAP() const { return hBitmap; }
    //inline operator ID2D1Bitmap1* () const { return pBitmap; }
    //inline operator Gdiplus::Graphics* () const { return pGraihics; }
    //inline operator ID2D1DeviceContext* () const { return pRenderTarget; }

};



// ������ʼ

#define HRESOURCE_MENU                      ((HWND)(10000 - 4))     // ��ȡ�˵���Դ���
#define HRESOURCE_TREEEXPAND                ((HWND)(10000 - 8))     // ��ȡ���Ϳ�Ӽ�����Դ���


// ֪ͨ��Ϣ, ��λ����, ʹ�õĶ��Ǹ�λ
#define INE_MENU_CALCMENU                   0x01000000  // ������չ�˵��ߴ�, wParam���ղ˵�ID, lParam���� IRE_STRUCT_MENU_CALCSIZE �ṹָ��, ͨ���޸Ľṹ��ֵ������˵��ߴ�, ���Է���ֵ
#define INE_MENU_DRAWMENU_BEFORE            0x01010000  // �˵������滭, wParam���ղ˵�ID, lParam���� IRE_STRUCT_MENU_DRAW�ṹָ��, �����Ƿ�����
#define INE_MENU_DRAWMENU_AFTER             0x01020000  // �˵��滭���, wParam���ղ˵�ID, lParam���� IRE_STRUCT_MENU_DRAW�ṹָ��, ���Է���ֵ
#define INE_MENU_GETICON                    0x01030000  // ��ȡ�˵�ͼ��, wParam���ղ˵�ID, lParam���� IRE_STRUCT_GETICON�ṹָ��, ����ͼƬָ��(LPWINDOW_ICON_DATA), ����з���ֵ, ���� prcû��ֵ����Է���ֵ

#define INE_CONFIG_CHANGED                  0x02000000  // ���ñ��ı�, wParam ����, lParam ����

#define INE_TOOL_BUTTONCLICK                0x03000000  // ֪ͨ�ӿڹ������İ�ť������, wParam = id, lParam = ���밴ťʱ��д����Ŀ��ֵ




#define IGL_HWND                            0x0000       // ���ھ��, ֻ��
#define IGL_WNDPROC                         0x0001       // �������໯�ĺ�����ַ, iDraw_SubclassWindow() �����ĵڶ���������¼����
#define IGL_CALLBACK_NOTIFY                 0x0002       // ֪ͨ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_DRAWITEM               0x0003       // �滭��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_MEASUREITEM            0x0004       // ����ߴ���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_COMMAND                0x0005       // �¼���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_CTLCOLOR               0x0006       // ��ɫ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_CALLWINDOWPROC         0x0007       // CallWindowProc ������ַ, ����ԭ����ʱʹ�������������, �ڵ������໯ʱ����ݴ����Ƿ�Ϊunicode���¼
#define IGL_INTERCEPT                       0x0008       // �Ƿ����ش���Ϣ, �ҽ���Ϣʱ�Ż�ʹ�����ֵ
#define IGL_OLDPROC                         0x0009       // ���ڵ�ԭ�ص�����, �Ѳ��������Ϣת���������ȥ����
#define IGL_CALLPROC_RETURN                 0x000A       // ֻ��, ����ԭ�ص������ķ���ֵ, ע�ᴦ�����Ϣ�¼��¿���ͨ���������ȡ������ֵ
#define IGL_CANVAS                          0x000B      // �봰�ڹ�����dx�滭���, ������ڲ���ʹ��dx��Ⱦ, ���ֵΪ0
#define IGL_USERDATA                        0x000C      // �봰�ڹ������û�����

#define IGL_DRAW_NONCPAINT                  0x0100       // ���ػ��ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCPAINT ��Ϣ
#define IGL_DRAW_NOCALCSIZE                 0x0101       // ������ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCCALCSIZE ��Ϣ, ���ڲ����б߿�
#define IGL_DRAW_ISTRANSPARENT              0x0102       // �Ƿ�͸��, ���Ϊtrue, �����õ� CTLCOLOR �л���� SetMode() ����Ϊ͸��
#define IGL_DRAW_ISHOT                      0x0103       // �Ƿ������ȵ�, һ�����б���ص����ʹ��
#define IGL_DRAW_ISCLIENT                   0x0104       // �Ƿ�����������Ϣ����Ϊ�ͻ���, Ϊtrue�򲻻��յ� WM_NCHITTEST��Ϣ, �ڲ�ȫ������ HTCLIENT
#define IGL_DRAW_ISERASEBKG                 0x0105       // �Ƿ����ػ�������Ϣ, ���Ϊtrue, �򴰿ڻ��յ� WM_ERASEBACKGROUND ��Ϣ, �����ڲ�����1
#define IGL_DRAW_STATE                      0x0106       // ��ǰ�����״̬
#define IGL_DRAW_FONT_DX                    0x0110       // ��ȡDX����, ֻ��
#define IGL_DRAW_FONT                       0x0111       // ��ȡGDI����, ֻ��
#define IGL_DRAW_TEXTFORMAT                 0x0112       // �滭�ı������ʽ

#define IGL_DRAW_CRBACKGROUND               0x0150       // ������ɫ, ÿ���������ʹ�������ɫ���滭����
#define IGL_DRAW_CRTEXT                     0x0151       // �ı���ɫ, ÿ������������ı���ɫ����ʹ�������ɫ�滭
#define IGL_DRAW_CRTEXTHOT                  0x0152       // �ı��ȵ���ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRTEXTDOWN                 0x0153       // �ı�������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRTEXTBAN                  0x0154       // �ı���ֹ��ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRNORMAL                   0x0155       // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRHOVER                    0x0156       // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRDOWN                     0x0157       // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRFOCUS                    0x0158       // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRCHECKED                  0x0159       // ���ѡ����ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRSELECTED                 0x015A       // ���ѡ����ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRHOT                      0x015B       // ����ȵ���ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRBAN                      0x015C       // �����ֹ��ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRVISTED                   0x015D       // 
#define IGL_DRAW_CRSHADOW                   0x015E       // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRBORDER                   0x015F       // ����߿���ɫ, ���л��Ʊ߿�ʹ�õ���ɫ

#define IGL_DRAW_HBRBACKGROUND              0x0160       // ����GDI��ˢ, ÿ���������ʹ�������ˢ����䱳��
#define IGL_DRAW_HBRTEXT                    0x0161       // �ı�GDI��ˢ
#define IGL_DRAW_HBRTEXTHOT                 0x0162       // �ı��ȵ�GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRTEXTDOWN                0x0163       // �ı�����GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRTEXTBAN                 0x0164       // �ı���ֹGDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRNORMAL                  0x0165       // �������GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRHOVER                   0x0166       // �������GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRDOWN                    0x0167       // �������GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRFOCUS                   0x0168       // �������GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRCHECKED                 0x0169       // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRSELECTED                0x016A       // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRHOT                     0x016B       // ����ȵ�GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRBAN                     0x016C       // �����ֹGDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRVISTED                  0x016D       // 
#define IGL_DRAW_HBRSHADOW                  0x016E       // �����ӰGDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRBORDER                  0x016F       // ����߿�GDI����, ���л��Ʊ߿�ʹ�õĻ���

#define IGL_POS_RECTSCREEN                  0x0200       // ��������Ļ�е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ��
#define IGL_POS_RECTCLIENT                  0x0201       // �����ڸ������е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ��
#define IGL_POS_X                           0x0202       // �����ڸ������е����λ��, ֻ��
#define IGL_POS_Y                           0x0203       // �����ڸ������еĶ���λ��, ֻ��
#define IGL_POS_WIDTH                       0x0204       // ���ڵĿ�� ֻ��
#define IGL_POS_HEIGHT                      0x0205       // ���ڵĸ߶�, ֻ��




//////////////////////////////////////////////////////////////////////////
// 0x0000 - 0x00FF ��Щ������ֻ������, �Ǵ��Ӿ������ȡһЩ����
#define IGI_EWINDOW                         0x0000  // ֻ��, ������������
#define IGI_CUSTOMCODETAB                   0x0001  // ֻ��, ����ѡ��еĴ��ھ��
#define IGI_CODE_HWND                       0x0002  // ֻ��, ����������ȡ MDIClient �µĵ�һ�㴰�ھ��, argument ��Ҫ��������
#define IGI_SHADOWWINDOW                    0x0003  // ֻ��, ��Ӱ����, ��������ǻ���������ڻ滭��, ��������ǲ㴰��
#define IGI_MDICLIENTTYPE                   0x0004  // ֻ��, ��ǰMDIClient ����������ʾʲô����, ��λ MDICLIENT_TYPE ö�ٳ���, ��λ MDICLIENT_CODETYPE ö�ٳ���, -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
#define IGI_MDICLIENTCODETYPE               0x0005  // ֻ��, ��ȡָ��MDIClient ����������ʾʲô��������, �������������ݴ���ѡ�������, ����-1�򷵻ص�ǰ����, MDICLIENT_CODETYPE ö�ٳ���, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
#define IGI_MDICLIENTTYPE_FROMINDEX         0x0006  // ֻ��, ��ȡMDIClient ����������ʾʲô����, ��λ MDICLIENT_TYPE ö�ٳ���, ��λ MDICLIENT_CODETYPE ö�ٳ���, -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
#define IGI_DPI                             0x0007  // ֻ��, ��ȡ��ǰ����DPI

#define IGI_RCCUSTOMCODETAB                 0x0020  // ֻ��, ����ѡ��е�λ��, ���ڸ����ڵ�λ��, ���ص��� RECT �ṹ��ָ��, �������޸�����ṹ������
#define IGI_RCSTATE                         0x0021  // ֻ��, ����״̬�е�λ��, ���ڸ����ڵ�λ��, ���ص��� RECT �ṹ��ָ��, �������޸�����ṹ������
#define IGI_RCSTATUSBAR                     0x0022  // ֻ��, ����״̬����λ��, ���ڸ����ڵ�λ��, ���ص��� RECT �ṹ��ָ��, �������޸�����ṹ������
#define IGI_RCMDICLIENT                     0x0023  // ֻ��, �м��������λ��, ���ڸ����ڵ�λ��, ���ص��� RECT �ṹ��ָ��, �������޸�����ṹ������
#define IGI_RCCONTROL                       0x0024  // ֻ��, ������λ��, ���ڸ����ڵ�λ��, ���ص��� RECT �ṹ��ָ��, �������޸�����ṹ������
#define IGI_RCWORK                          0x0025  // ֻ��, �����е�λ��, ���ڸ����ڵ�λ��, ���ص��� RECT �ṹ��ָ��, �������޸�����ṹ������
#define IGI_RCMENUWND                       0x0026  // ֻ��, �˵���������λ��, ���ڸ����ڵ�λ��, ���ص��� RECT �ṹ��ָ��, �������޸�����ṹ������
#define IGI_DEFDXFONT                       0x0027  // ֻ��, ����Ĭ��ʹ�õ�DX����, ����ݵ�ǰ����DPI���ض�Ӧ������
#define IGI_CONFIG_DEFDXFONT                0x0027  // ֻ��, ����Ĭ��ʹ�õ�DX����, ����ݵ�ǰ����DPI���ض�Ӧ������
#define IGI_CONFIG_DEFGDIFONT               0x0028  // ֻ��, ����Ĭ��ʹ�õ�GDI����, ����ݵ�ǰ����DPI���ض�Ӧ������

#define IGI_IGETEWINDOW                     0x0030  // ֻ��, �����Ի�ȡ�������ڵ���ָ��
#define IGI_IDENOTIFY                       0x0031  // ֻ��, �����Խ�������, ��������������൱�ڵ��������� ������ϵͳ����() ����

#define IGI_LIBPATH                         0x0035  // ֻ��, ������֧�ֿ�Ŀ¼, �������б��, ����unicode�ı�ָ��
#define IGI_DATAPATH                        0x0036  // ֻ��, ��IDE���ݴ洢Ŀ¼, �������б��, һ���� ������֧�ֿ�Ŀ¼/iDraw/, ����unicode�ı�ָ��
#define IGI_IDRAW                           0x0037  // ֻ��, �Ի���ӿ�, Ŀǰ��ʱδ���⹫��
#define IGI_D2D                             0x0038  // ֻ��, D2D��ӿ�, Ŀǰ��ʱδ���⹫��

#define IGI_STATE_TOOL_GET                  0x0040  // ֻ��, ��ȡָ��������ID��״̬, ����3��Ҫ���ݰ�ťID
#define IGI_STATE_TOOL_RUN                  0x0041  // ֻ��, ��ǰ���������а�ť��״̬
#define IGI_STATE_TOOL_STOP                 0x0042  // ֻ��, ��ǰ������ֹͣ��ť��״̬

#define IGI_PATH_LIB                        0x0060  // ֻ��, ������֧�ֿ�Ŀ¼, �������б��, ����unicode�ı�ָ��
#define IGI_PATH_IDRAW                      0x0061  // ֻ��, ��IDE��Ҫʹ�õ�Ŀ¼, �������б��, һ���� ������֧�ֿ�Ŀ¼/iDraw/, ����unicode�ı�ָ��
#define IGI_PATH_DATA                       0x0062  // ֻ��, ��IDE���ݴ洢Ŀ¼, �������б��, һ���� ������֧�ֿ�Ŀ¼/iDraw/data/, ����unicode�ı�ָ��
#define IGI_PATH_PLUGIN                     0x0063  // ֻ��, ��IDE���·��
#define IGI_PATH_CONFIG                     0x0064  // ֻ��, ��IDE���������ļ�����·��


// 0x0100 ��ʼ������������س���, 0x0200 - 0x03FF ����ɫ������س���
#define IGI_CONFIG_ISDXRENDER               0x0100  // �Ƿ�ʹ��DX��Ⱦ, ������Ҫ������Ч
#define IGI_CONFIG_ISRUNSAVE                0x0101  // ����/������Ƿ񱣴�Դ��
#define IGI_CONFIG_WHEELPOS                 0x0102  // ������һ�εľ���
#define IGI_CONFIG_NOCLIPCHILDREN           0x0103  // �����ڻ滭ʱ���ų��Ӵ�������
#define IGI_CONFIG_ISDPIAWARE               0x0104  // �Ƿ���DPI��֪
#define IGI_CONFIG_ISDPISCALEWND            0x0105  // DPI�ı�ʱ�Ƿ����Ŵ���
#define IGI_CONFIG_SHOWCODETABCLOSE         0x0106  // ��ʾ����ѡ��еĹرհ�ť

#define IGI_CONFIG_MENUITEMHEIGHT           0x0110  // �˵���߶�
#define IGI_CONFIG_MENUSEPARATORHEIGHT      0x0111  // �ָ����˵���߶�
#define IGI_CONFIG_ISLOCATIONTOOLBAR        0x0112  // �Ƿ���ʾ��λ������
#define IGI_CONFIG_PROGRAMDBLCLKTYPE        0x0113  // �������Ϳ�˫��ģʽ, 0=ֻ�л�, 1=ֻչ��, 2=չ��+�л�
#define IGI_CONFIG_NOCHANGETAB_WORK         0x0114  // ���Զ��л�������
#define IGI_CONFIG_NOCHANGETAB_STATE        0x0115  // ���Զ��л�״̬��
#define IGI_CONFIG_SCROLL_WIDTH             0x0116  // �������Ŀ��, Ĭ����16, ������������Ǹ߶�
#define IGI_CONFIG_SCROLL_BTN_SIZE          0x0117  // ��������ť�ĳߴ�, Ĭ����12, �����������Ⱦ��ǹ������Ŀ��, ����ߴ�������ĸ߶�, �����෴

#define IGI_CONFIG_WORK_DIRECTION           0x0120  // �����е��Ӽз���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
#define IGI_CONFIG_WORK_ISICON              0x0121  // ��ߵĹ������Ƿ���ʾͼ��
//#define IGI_CONFIG_WORK_ISSHOW              0x0122  // �������Ƿ���ʾ, ���Ϊfalse, ��ʧȥ�������������
//#define IGI_CONFIG_WORK_POS                 0x0123  // ������λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
#define IGI_CONFIG_WORK_FLAGS               0x0124  // �����б�ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ

#define IGI_CONFIG_STATE_DIRECTION          0x0130  // ״̬�е��Ӽз���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
#define IGI_CONFIG_STATE_ISICON             0x0131  // ���µ�״̬ѡ����Ƿ���ʾͼ��
//#define IGI_CONFIG_STATE_ISSHOW             0x0132  // ״̬���Ƿ���ʾ, ���Ϊfalse, ��ʧȥ�������������
//#define IGI_CONFIG_STATE_POS                0x0133  // ״̬��λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
#define IGI_CONFIG_STATE_FLAGS              0x0134  // ״̬�б�ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ

#define IGI_CONFIG_CODE_DIRECTION           0x0140  // ����ѡ��е��Ӽз���, 3=�ױ�, ���� = ����
#define IGI_CONFIG_CODE_ISICON              0x0141  // �����л�ѡ����Ƿ���ʾͼ��

//#define IGI_CONFIG_CONTROL_ISSHOW           0x0150  // ������Ƿ���ʾ, ���Ϊfalse, ��ʧȥ�������������
//#define IGI_CONFIG_CONTROL_POS              0x0151  // �����λ��, 0=���, 1=����, 2=�ұ�, 3=�ױ�
#define IGI_CONFIG_CONTROL_FLAGS            0x0152  // ������ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ

#define IGI_CONFIG_TREE_FNE_ISHOT           0x0161  // ֧�ֿ����Ϳ��Ƿ�֧���ȵ�
#define IGI_CONFIG_TREE_PROGRAM_ISHOT       0x0162  // �������Ϳ��Ƿ�֧���ȵ�
#define IGI_CONFIG_TREE_CONTROL_ISHOT       0x0163  // ��������Ϳ��Ƿ�֧���ȵ�
#define IGI_CONFIG_TREE_VAR_ISHOT           0x0164  // ���������Ϳ��Ƿ�֧���ȵ�
#define IGI_CONFIG_TREE_CALL_ISHOT          0x0165  // ���ñ����Ϳ��Ƿ�֧���ȵ�
#define IGI_CONFIG_LIST_FIND1_ISHOT         0x0166  // ��Ѱ1�б���Ƿ�֧���ȵ�
#define IGI_CONFIG_LIST_FIND2_ISHOT         0x0167  // ��Ѱ1�б�����Ϳ��Ƿ�֧���ȵ�
#define IGI_CONFIG_TREE_CONTROL_ORGICON     0x0168  // ��������Ϳ���ʾԭͼ��

#define IGI_CONFIG_TREE_FNE_HEIGHT          0x0171  // ֧�ֿ����Ϳ���Ŀ�߶�
#define IGI_CONFIG_TREE_PROGRAM_HEIGHT      0x0172  // �������Ϳ���Ŀ�߶�
#define IGI_CONFIG_TREE_CONTROL_HEIGHT      0x0173  // ��������Ϳ���Ŀ�߶�
#define IGI_CONFIG_TREE_VAR_HEIGHT          0x0174  // ���������Ϳ���Ŀ�߶�
#define IGI_CONFIG_TREE_CALL_HEIGHT         0x0175  // ���ñ����Ϳ���Ŀ�߶�
#define IGI_CONFIG_LIST_FIND1_HEIGHT        0x0176  // ��Ѱ1�б����Ŀ�߶�
#define IGI_CONFIG_LIST_FIND2_HEIGHT        0x0177  // ��Ѱ1�б����Ŀ�߶�



// ���￪ʼ����ɫ������س���
// ������ɫ��ɫ����3�����Դ��� IGICR_ ��ͷ�ĳ�����ȡ��Ӧֵ, Ĭ���Ƿ�����ɫ, �������������Щֵ֮һ, �򷵻���ɫֵ
#define IGICR_COLOR         0x0000      // ������ɫ, ����Ĭ��ֵ
#define IGICR_BRUSH         0x0001      // ���������ɫ��Ӧ��gdi��ˢ
#define IGICR_PEN           0x0002      // ���������ɫ��Ӧ��gdi����
#define IGICR_BRUSHDX       0x0003      // ���������ɫ��Ӧ��dx��ˢ, �����ˢֻ��ʹ���Ӿ����ṩ�Ļ滭����ʹ��, �������D2D�Ļ�ˢ���
#define IGICR_PENDX         0x0004      // ���������ɫ��Ӧ��dx����, �������ֻ��ʹ���Ӿ����ṩ�Ļ滭����ʹ��, �������D2D�Ļ��ʾ��

//! Ĭ����ɫ 0x0200 - 0x021F
#define IGI_CONFIG_CRDEF_BACK               0x0200  // [DefaultColor], Ĭ����ɫ, ������ɫ
#define IGI_CONFIG_CRDEF_NORMAL             0x0201  // [DefaultColor], Ĭ����ɫ, ������ɫ
#define IGI_CONFIG_CRDEF_HOT                0x0202  // [DefaultColor], Ĭ����ɫ, �ȵ���ɫ
#define IGI_CONFIG_CRDEF_DOWN               0x0203  // [DefaultColor], Ĭ����ɫ, ������ɫ
#define IGI_CONFIG_CRDEF_BAN                0x0204  // [DefaultColor], Ĭ����ɫ, ��ֹ��ɫ
#define IGI_CONFIG_CRDEF_HOVER              0x0205  // [DefaultColor], Ĭ����ɫ, ������ɫ
#define IGI_CONFIG_CRDEF_FOCUS              0x0206  // [DefaultColor], Ĭ����ɫ, ������ɫ
#define IGI_CONFIG_CRDEF_CHECKED            0x0207  // [DefaultColor], Ĭ����ɫ, ѡ����ɫ
#define IGI_CONFIG_CRDEF_SELECTED           0x0208  // [DefaultColor], Ĭ����ɫ, ѡ����ɫ
#define IGI_CONFIG_CRDEF_VISTED             0x0209  // [DefaultColor], Ĭ����ɫ, 
#define IGI_CONFIG_CRDEF_SHADOW             0x020A  // [DefaultColor], Ĭ����ɫ, ��Ӱ��ɫ
#define IGI_CONFIG_CRDEF_BORDER             0x020B  // [DefaultColor], Ĭ����ɫ, �߿���ɫ
#define IGI_CONFIG_CRDEF_TEXT               0x020C  // [DefaultColor], Ĭ����ɫ, �ı���ɫ
#define IGI_CONFIG_CRDEF_TEXTHOT            0x020D  // [DefaultColor], Ĭ����ɫ, �ı��ȵ���ɫ
#define IGI_CONFIG_CRDEF_TEXTDOWN           0x020E  // [DefaultColor], Ĭ����ɫ, �ı�������ɫ
#define IGI_CONFIG_CRDEF_TEXTBAN            0x020F  // [DefaultColor], Ĭ����ɫ, �ı���ֹ��ɫ



// ������ɫ 0x0220 - 0x022F
#define IGI_CONFIG_CRBACK                   0x0220  // [Basics], ������ɫ
#define IGI_CONFIG_CRBACKEXTERN             0x0221  // [Basics], �ⲿ�ı�����ɫ, ����ѡ��е��Ӽ�
#define IGI_CONFIG_CRSTATEBAR               0x0222  // [Basics], �����/������/״̬�ж����������ɫ
#define IGI_CONFIG_CRTEXT                   0x0223  // [Basics], һ���ı���ɫ

// �߿�/��Ӱ��ɫ 0x0230 - 0x023F
#define IGI_CONFIG_CRBORDER                 0x0230  // [Basics], ���ڱ߿���ɫ
#define IGI_CONFIG_CRBORDER_RUN             0x0231  // [Basics], ���ڱ߿�����ʱ����ɫ
#define IGI_CONFIG_CRBORDER_NF              0x0232  // [Basics], ���ڱ߿�ʧȥ����ʱ����ɫ
#define IGI_CONFIG_CRSHADOW                 0x0238  // [Basics], ������Ӱ��ɫ
#define IGI_CONFIG_CRSHADOW_RUN             0x0239  // [Basics], ������Ӱ����ʱ����ɫ
#define IGI_CONFIG_CRSHADOW_NF              0x023A  // [Basics], ������Ӱʧȥ����ʱ����ɫ

// �˵��� 0x0240 - 0x024F
#define IGI_CONFIG_CRMENUBORDER             0x0240  // [Basics], �˵���ı߿���ɫ
#define IGI_CONFIG_CRMENUBACK               0x0241  // [Basics], �˵���ı�����ɫ, û���ȵ�, ���ǽ�ֹʱʹ�������ɫ�滭
#define IGI_CONFIG_CRMENUHOT                0x0242  // [Basics], �˵�����ȵ���ɫ
#define IGI_CONFIG_CRMENUBAN                0x0243  // [Basics], �˵���Ľ�ֹ��ɫ
#define IGI_CONFIG_CRMENUTEXT               0x0244  // [Basics], �˵��������ı���ɫ
#define IGI_CONFIG_CRMENUTEXTHOT            0x0245  // [Basics], �˵����ȵ��ı���ɫ
#define IGI_CONFIG_CRMENUTEXTBAN            0x0246  // [Basics], �˵����ֹ�ı���ɫ

// ��ʾ�ڱ���������Ĳ˵��� 0x0250 - 0x025F
#define IGI_CONFIG_CRMENUBARHOT             0x0250  // [Basics], �����˵����ȵ���ɫ
#define IGI_CONFIG_CRMENUBARTEXT            0x0251  // [Basics], �����˵������ֲ˵����ı���ɫ
#define IGI_CONFIG_CRMENUBARTEXTHOT         0x0252  // [Basics], �����˵������ֲ˵����ı��ȵ���ɫ

// ��ʾ�ڱ��������ɫ�����ɫ 0x0260 - 0x026F
#define IGI_CONFIG_CRTITLEBACK              0x0260  // [Basics], ���ߺڿ鲿�ֱ�����ɫ
#define IGI_CONFIG_CRTITLELEFT_ACT          0x0261  // [Basics], �����ڿ鲿����߼���ʱ�ı���ɫ, �����Դ����������Ŀ��
#define IGI_CONFIG_CRTITLERIGHT_ACT         0x0262  // [Basics], �����ڿ鲿���ұ߼���ʱ�ı���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ
#define IGI_CONFIG_CRTITLELEFT              0x0263  // [Basics], �����ڿ鲿������ı���ɫ, �����Դ����������Ŀ��
#define IGI_CONFIG_CRTITLERIGHT             0x0264  // [Basics], �����ڿ鲿���ұ��ı���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ

// �ײ�״̬������ɫ 0x0270 - 0x027F
#define IGI_CONFIG_CRTEXTSTATUSBAR          0x0270  // [Basics], �ײ�״̬�����ı���ɫ
#define IGI_CONFIG_CRTEXTSTATUSBAR_RUN      0x0271  // [Basics], �ײ�״̬������ʱ���ı���ɫ
#define IGI_CONFIG_CRTEXTSTATUSBAR_NF       0x0272  // [Basics], �ײ�״̬����������ı���ɫ

// ��������ɫ 0x0280 - 0x028F
#define IGI_CONFIG_CRSCROLLBTN1NORRMAL      0x0280  // [Basics], ��������ť1������ɫ, ��߻��߶��ߵ�С��ť
#define IGI_CONFIG_CRSCROLLBTN1HOT          0x0281  // [Basics], ��������ť1�ȵ���ɫ, ��߻��߶��ߵ�С��ť
#define IGI_CONFIG_CRSCROLLBTN1DOWN         0x0282  // [Basics], ��������ť1������ɫ, ��߻��߶��ߵ�С��ť
#define IGI_CONFIG_CRSCROLLBTN2NORRMAL      0x0283  // [Basics], ��������ť2������ɫ, �ұ߻��ߵױߵ�С��ť
#define IGI_CONFIG_CRSCROLLBTN2HOT          0x0284  // [Basics], ��������ť2�ȵ���ɫ, �ұ߻��ߵױߵ�С��ť
#define IGI_CONFIG_CRSCROLLBTN2DOWN         0x0285  // [Basics], ��������ť2������ɫ, �ұ߻��ߵױߵ�С��ť
#define IGI_CONFIG_CRSCROLLTHUMBNORRMAL     0x0286  // [Basics], ������������ɫ
#define IGI_CONFIG_CRSCROLLTHUMBHOT         0x0287  // [Basics], �����������ȵ���ɫ
#define IGI_CONFIG_CRSCROLLTHUMBDOWN        0x0288  // [Basics], ���������鰴����ɫ
#define IGI_CONFIG_CRSCROLLBACKGROUND       0x0289  // [Basics], ������������ɫ

// �ָ�����ɫ 0x0290 - 0x029F
#define IGI_CONFIG_CRLINELEFT               0x0290  // [Basics], ������, ���б�ͷ, ������ķָ��������ɫ, �ָ��߶�����������
#define IGI_CONFIG_CRLINERIGHT              0x0291  // [Basics], ������, ���б�ͷ, ������ķָ����ұ���ɫ, �ָ��߶�����������

// �Ի�����л�ѡ��йرհ�ť��ɫ, 0x02A0 - 0x02AF
#define IGI_CONFIG_CODETAB_CRCLOSEHOT               0x02A0  // [CustomCodeTab], �����л�ѡ��йرհ�ť�ȵ���ɫ
#define IGI_CONFIG_CODETAB_CRCLOSEDOWN              0x02A1  // [CustomCodeTab], �����л�ѡ��йرհ�ť������ɫ

// �Ի�����л�ѡ����ı���ɫ, 0x02B0 - 0x02BF
#define IGI_CONFIG_CODETAB_CRTEXT_WINDOW            0x02B0  // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
#define IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY          0x02B1  // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
#define IGI_CONFIG_CODETAB_CRTEXT_ASSEMBLY_WINDOW   0x02B2  // [CustomCodeTab], �����л�ѡ���, ���ڳ����ı���ɫ
#define IGI_CONFIG_CODETAB_CRTEXT_CLASS             0x02B3  // [CustomCodeTab], �����л�ѡ���, ���ı���ɫ
#define IGI_CONFIG_CODETAB_CRTEXT_GLOBALVAR         0x02B4  // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����ı���ɫ
#define IGI_CONFIG_CODETAB_CRTEXT_STRUCT            0x02B5  // [CustomCodeTab], �����л�ѡ���, �Զ������������ı���ɫ
#define IGI_CONFIG_CODETAB_CRTEXT_DLL               0x02B6  // [CustomCodeTab], �����л�ѡ���, DLL�ı���ɫ
#define IGI_CONFIG_CODETAB_CRTEXT_CONST             0x02B7  // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
#define IGI_CONFIG_CODETAB_CRTEXT_IMAGE             0x02B8  // [CustomCodeTab], �����л�ѡ���, ͼƬ�ı���ɫ
#define IGI_CONFIG_CODETAB_CRTEXT_SOUND             0x02B9  // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ

// �Ի�����л�ѡ��е���������ɫ, 0x02C0 - 0x02CF
#define IGI_CONFIG_CODETAB_CRLINE_WINDOW            0x02C0  // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
#define IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY          0x02C1  // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
#define IGI_CONFIG_CODETAB_CRLINE_ASSEMBLY_WINDOW   0x02C2  // [CustomCodeTab], �����л�ѡ���, ���ڳ����±�����ɫ
#define IGI_CONFIG_CODETAB_CRLINE_CLASS             0x02C3  // [CustomCodeTab], �����л�ѡ���, ���±�����ɫ
#define IGI_CONFIG_CODETAB_CRLINE_GLOBALVAR         0x02C4  // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����±�����ɫ
#define IGI_CONFIG_CODETAB_CRLINE_STRUCT            0x02C5  // [CustomCodeTab], �����л�ѡ���, �Զ������������±�����ɫ
#define IGI_CONFIG_CODETAB_CRLINE_DLL               0x02C6  // [CustomCodeTab], �����л�ѡ���, DLL�±�����ɫ
#define IGI_CONFIG_CODETAB_CRLINE_CONST             0x02C7  // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
#define IGI_CONFIG_CODETAB_CRLINE_IMAGE             0x02C8  // [CustomCodeTab], �����л�ѡ���, ͼƬ�±�����ɫ
#define IGI_CONFIG_CODETAB_CRLINE_SOUND             0x02C9  // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ

// ��ɫ��ɫ��Ҫ�� 0x03FF







#define IRE_SIZE_TABWORK                    0x0001  // ��߹����гߴ类�ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SIZE �ṹָ��, �����Ƿ�����, ����󷵻�ֵ��ʱ������
#define IRE_SIZE_TABSTATE                   0x0002  // �±�״̬�гߴ类�ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SIZE �ṹָ��, �����Ƿ�����, ����󷵻�ֵ��ʱ������
#define IRE_SIZE_TABMDICLIENT               0x0003  // MDIClient�ߴ类�ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_MDICLIENT_MOVE_EVENT �ṹָ��, �����Ƿ�����, ����󷵻�ֵ��ʱ������
#define IRE_TAB_WORK_SIZE                   IRE_SIZE_TABWORK
#define IRE_TAB_STATE_SIZE                  IRE_SIZE_TABSTATE
#define IRE_TAB_MDICLIENT_SIZE              IRE_SIZE_TABMDICLIENT



//#define IRE_E_RUNSTATE_CHANGE               0x0010  // ����, ���ᴥ��, ���������а�ť״̬���ı�, �¼��ص��ĵ��������������Ƿ�����, 1=���а�ť������, 0=���а�ť�ѽ���
//#define IRE_E_STOPSTATE_CHANGE              0x0011  // ����, ���ᴥ��, ������ֹͣ��ť״̬���ı�, �¼��ص��ĵ��������������Ƿ�����, 1=ֹͣ��ť������, 0=ֹͣ��ť�ѽ���
//#define IRE_TOOLSTATE_CHANGE_RUN            IRE_E_RUNSTATE_CHANGE
//#define IRE_TOOLSTATE_CHANGE_STOP           IRE_E_STOPSTATE_CHANGE
#define IRE_TOOLSTATE_CHANGE                0x0012  // �����Ա�׼��������ť״̬���ı�, �¼��ص��ĵ������������� IRE_STRUCT_TOOLSTATE_CHANGE �ṹ, �����Ƿ�����, ����󷵻�ֵ��ʱ������


#define IRE_TABSELCHANGED_WORK              0x0020  // ��߹�����ѡ�񱻸ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SELCHANGE �ṹָ��, ����ǰ���صĻ�����ı��Ӽ�, ����󷵻�ֵ��ʱ������
#define IRE_TABSELCHANGED_STATE             0x0021  // �±�״̬��ѡ�񱻸ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SELCHANGE �ṹָ��, ����ǰ���صĻ�����ı��Ӽ�, ����󷵻�ֵ��ʱ������
#define IRE_TABSELCHANGED_CODE              0x0022  // �����л�ѡ���ѡ�񱻸ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SELCHANGE �ṹָ��, ����ǰ���صĻ����ᴥ���ߴ�ı��¼�, ����󷵻�ֵ��ʱ������
#define IRE_TAB_WORK_SELCHANGED             IRE_TABSELCHANGED_WORK
#define IRE_TAB_STATE_SELCHANGED            IRE_TABSELCHANGED_STATE
#define IRE_TAB_CODE_SELCHANGED             IRE_TABSELCHANGED_CODE

#define IRE_MDICLIENT_CREATE                0x0023  // MDIClient�µ�mid���ڱ������¼�, �¼��ص��ĵ����������յ� IRE_STRUCT_MDICREATE �ṹָ��, ����ǰ�����Ҳ���ȷ����Ļ����ڲ��ᱻ����, ����ǰ�ṹ�е� hWnd��ԱΪ0, ���ú��ԱΪ�´�����mdi���ھ��, ����󷵻�ֵĿǰ������
#define IRE_MDICLIENT_DESTROY               0x0024  // MDIClient�µ�mid���ڱ������¼�, �¼��ص��ĵ����������յ� �����ٵ�MDI���ھ��, ����ǰ���ú�Ӧ�÷���0
#define IRE_MDICLIENT_ACTIVATE              0x0025  // MDIClient�µ�mid���ڱ������¼�, �¼��ص��ĵ����������յ� Ҫ����� MDI �Ӵ��ڵľ��, ����ǰ���ط�0����, ����󷵻�ֵ������
#define IRE_ECODESELCHANGED                 0x0026  // �����Դ��봰�ڹ��ı��¼�, �¼��ص��ĵ����������յ� IRE_STRUCT_EIDE_SELCHANGEDINFO �ṹָ��, ���¼����ᴥ�����ú��¼�, ����ֵ������


#define IRE_DRAW_BCAKGROUND                 0x0030  // �������滭�¼�, ����ֵ�Ƿ�����, �¼��ص��ĵ����������յ� IRE_STRUCT_BACKGROUND_DRAW �ṹָ��
#define IRE_BCAKGROUND_DRAW                 IRE_DRAW_BCAKGROUND
#define IRE_DRAW_AUTOHIDE_SHOW              0x0031  // �Զ����صĴ��ڱ���ʾ, ����ǰ�����Ƿ�����, ���غ󲻻ᱻ��ʾ, �¼��ص��ĵ����������յ� Ҫ��ʾ�Ĵ��ھ��
#define IRE_DRAW_AUTOHIDE_HIDE              0x0032  // �Զ����صĴ��ڱ�����, ����ǰ�����Ƿ�����, ���غ󲻻ᱻ����, �¼��ص��ĵ����������յ� Ҫ���صĴ��ھ��
#define IRE_DRAW_AUTOHIDE_MOVE              0x0033  // �Զ����صĴ��ڱ��ƶ�, ǰ�ͺ󶼺��Է���ֵ, �¼��ص��ĵ����������յ� IRE_STRUCT_AUTOHIDE_MOVE ָ��

#define IRE_HOOKPROC_CBT                    0x0050  // cbt�����¼�(WH_CBT), �¼��ص������������յ� IRE_STRUCT_HOOKARG �ṹָ��, ����ǰ�����Ƿ�����, ���ú���Է���ֵ
//#define IRE_HOOKPROC_GETMESSAGE             0x0051  // ��ȡ��Ϣ�����¼�(WH_GETMESSAGE), �¼��ص������������յ� IRE_STRUCT_HOOKARG �ṹָ��, ����ǰ�����Ƿ�����, ���ú���Է���ֵ

#define IRE_ICON_GET                        0x0060  // ��ȡͼ���¼�, 

#define IRE_MENUEXT_MANAGE                  0x0500  // ������չ�˵�, ���ᴥ�����ú��¼�, ����ǰ�����Ƿ�����, ���صĻ�������������յ�����¼�, ����������Ϊ0
#define IRE_MENUEXT_CUSTOP                  0x0501  // �Զ�����չ�˵�, ���ᴥ�����ú��¼�, ����ǰ�����Ƿ�����, ���صĻ�������������յ�����¼�, ����������Ϊ0








// �����Ӿ��⹦��, ���ݹ��ܺŵ��ò�ͬ����, ����������ο���Ӧ���ܺ�
// IDC id call, �����Ӿ��⹦��, ID=���ܺ� C=call , IDC = ���ܺŵ���




//////////////////////////////////////////////////////////////////////////
#define IDC_ICONFIG_BEGIN               0x01D00000  // ���ò�����ܺſ�ʼ
#define IDC_ICONFIG_END                 0x01D0FFFF  // ���ò�����ܺŽ���
#define IDC_ITHEME_BEGIN                0x02D00000  // ���������ܺſ�ʼ
#define IDC_ITHEME_END                  0x02D0FFFF  // ���������ܺŽ���
#define IDC_IEVENT_BEGIN                0x03D00000  // �¼�������ܺſ�ʼ
#define IDC_IEVENT_END                  0x03D0FFFF  // �¼�������ܺŽ���
#define IDC_ICONTROL_BEGIN              0x04D00000  // �滭������ܺſ�ʼ
#define IDC_ICONTROL_END                0x04D0FFFF  // �滭������ܺŽ���
#define IDC_IRESOURCE_BEGIN             0x05D00000  // ��Դ������ܺſ�ʼ
#define IDC_IRESOURCE_END               0x05D0FFFF  // ��Դ������ܺŽ���
#define IDC_IMOVE_BEGIN                 0x06D00000  // �ƶ�������ܺſ�ʼ
#define IDC_IMOVE_END                   0x06D0FFFF  // �ƶ�������ܺŽ���
#define IDC_IETOOLS_BEGIN               0x07D00000  // ���ֲ�����ܺſ�ʼ
#define IDC_IETOOLS_END                 0x07D0FFFF  // ���ֲ�����ܺŽ���
#define IDC_IDRAW_BEGIN                 0x08D00000  // ��֧�ֿ⹦�ܺſ�ʼ
#define IDC_IDRAW_END                   0x08D0FFFF  // ��֧�ֿ⹦�ܺŽ���



#define IDC_IDRAW_STR2RGB               (IDC_IDRAW_BEGIN + 0x0100)  // ��һ���ı���ɫת����RGB��ɫ, wParam = Ĭ����ɫ, ����ʧ�ܷ��������ɫ, lParamָ����ɫ�ı�ָ��
#define IDC_IDRAW_STR2ARGB              (IDC_IDRAW_BEGIN + 0x0101)  // ��һ���ı���ɫת����ARGB��ɫ, wParam = Ĭ����ɫ, ����ʧ�ܷ��������ɫ, lParamָ����ɫ�ı�ָ��
#define IDC_IDRAW_RGB2STR               (IDC_IDRAW_BEGIN + 0x0102)  // ��RGB��ɫת�����ı� RGB(1,2,3), wParam=Ҫת����rgb, lParam = ����20��WCHAR������ָ��, ����д���ַ���
#define IDC_IDRAW_ARGB2STR              (IDC_IDRAW_BEGIN + 0x0103)  // ��ARGB��ɫת�����ı� ARGB(255,1,2,3), wParam=Ҫת����argb, lParam = ����26��WCHAR������ָ��, ����д���ַ���
#define IDC_IDRAW_GETWINDOWDATA         (IDC_IDRAW_BEGIN + 0x0104)  // ��ȡ��������, wParam����, lParam = Ҫ��ȡ�Ĵ��ھ��, �ɹ���������ָ��, ʧ�ܷ���0
#define IDC_IDRAW_GETCODEVISIBLERECT    (IDC_IDRAW_BEGIN + 0x0105)  // ��ȡ���봰�ڿ��ӵľ���, wParam����, lParam = ���վ��ε�ָ��(����������������λ��)

#define IDC_IDRAW_INLINEHOOK            (IDC_IDRAW_BEGIN + 0x0200)  // ����inlineHook, wParam = ԭ����, lParam = �º���, ����ԭ������ַ, ����ֱ�ӵ���ԭ����
#define IDC_IDRAW_UNINLINEHOOK          (IDC_IDRAW_BEGIN + 0x0201)  // ȡ��inlineHook, wParam = ԭ����, lParam = �º���, �����Ƿ�ɹ�
#define IDC_IDRAW_CREATEPOPUPMENU       (IDC_IDRAW_BEGIN + 0x0203)  // �������Ի���ʽ�ĵ���ʽ�˵�, ���м��뵽�������ʽ�˵�����Ӳ˵����ᱻ�Ի�

#define IDC_EIDE_GETCODEMANAGE          (IDC_IDRAW_BEGIN + 0x0401)  // ��ȡIDE�������(IEIDE_CodeManage)��ָ��, ����wParam����, lParma = ���봰�ڴ��ھ��, Ϊ0���ȡ��ǰ����
#define IDC_EIDE_GETLINESPACING         (IDC_IDRAW_BEGIN + 0x0402)  // ��ȡ���м��, ����wParam�� lParam����
#define IDC_EIDE_SETLINESPACING         (IDC_IDRAW_BEGIN + 0x0403)  // ��δʵ��, ���õ��м��, ����wParam�� lParam����
#define IDC_EIDE_GETLINEHEIGHT          (IDC_IDRAW_BEGIN + 0x0404)  // ��ȡ���и߶�, ����wParam�� lParam����
#define IDC_EIDE_SETLINEHEIGHT          (IDC_IDRAW_BEGIN + 0x0405)  // ��δʵ��, ���õ��и߶�, ����wParam�� lParam����
#define IDC_EIDE_GETCODEWINDOW          (IDC_IDRAW_BEGIN + 0x0406)  // ��ȡ��ǰ�������, �����ǰ����ʼҳ��������������򷵻�0, ����wParam�� lParam����
#define IDC_EIDE_GETECOLOR              (IDC_IDRAW_BEGIN + 0x0407)  // ��ȡ�����Ե�ǰʹ�õ���ɫ, wParam = �������ߴ�, lParam = ��������ַ, ����д�뻺�����ֽ���, wParam/lParamΪ0�򷵻ػ�������Ҫ�ĳߴ�
#define IDC_EIDE_SETECOLOR              (IDC_IDRAW_BEGIN + 0x0408)  // ���������Ե�ǰʹ�õ���ɫ, wParamΪ0��lParam����������ɫ�ļ�·��Unicode��, ��Ϊ0��lParamָ����ɫ����, wParamΪ���ݳߴ�, lParam = ����wParam������ʲôֵ

#define IDC_SCROLL_GETRECT              (IDC_IDRAW_BEGIN + 0x0500)  // ��ȡ������λ��, ����ͨ������¼��޸ı߿�ﵽָ���滭������λ��, wParam = pfn_GetScrollRect �ص�����, lParam = ��������ָ��, �ɹ����� wParam, ʧ�ܷ���0
#define IDC_SCROLL_CALCRECT             (IDC_IDRAW_BEGIN + 0x0501)  // ���������λ��, ����ͨ������¼��޸Ĵ��ڷǿͻ���λ��, wParam = pfn_CalcScroll �ص�����, lParam = ��������ָ��, �ɹ����� wParam, ʧ�ܷ���0
#define IDC_SCROLL_DRAW                 (IDC_IDRAW_BEGIN + 0x0502)  // �滭������, wParam = pfn_DrawScroll �ص�����, lParam = ��������ָ��, �ɹ����� wParam, ʧ�ܷ���0


#define IDC_IMOVE_GETWINDOWTYPE         (IDC_IMOVE_BEGIN + 0X0001)  // ��ȡ��ǰMDIClient ����������ʾʲô����, -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
#define IDC_IMOVE_GETCODETYPE           (IDC_IMOVE_BEGIN + 0X0002)  // ��ȡ��ǰMDIClient ����������ʾʲô��������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ, ����Ҫ���ݲ���


#define DRAW_WND_BYCOMMAND      0x00000000L // nPosition ָ����ID
#define DRAW_WND_BYPOSITION     0x00000400L // nPosition ָ����λ��

// ��������

// ����dll��·��, �����������֧�ֿ�Ŀ¼�µ� iDraw.fne 
inline const wchar_t* iDrawSetDllName(const wchar_t* dllFileName)
{
    static wchar_t fileName[260] = L"iDraw.fne";
    if (!dllFileName || !dllFileName[0]) return fileName;
    size_t len = wcslen(dllFileName);
    if (len >= 259)len = 259;
    memcpy(fileName, dllFileName, len * sizeof(wchar_t));
    fileName[len] = 0;
    return fileName;
}

// ��ȡdll·��
inline const wchar_t* iDrawGetDllName()
{
    return iDrawSetDllName(0);
}

inline void* iDrawGetAddress(const char* funName)
{
    static HMODULE hModule = 0;
    if (!hModule)
        hModule = LoadLibraryW(iDrawGetDllName());

    if (!hModule)throw; // ����ʧ��ֱ���׳��쳣
    return GetProcAddress(hModule, funName);
}


class ID2DDraw
{
public:
    // ��ʼ�����
    virtual bool _canvas_init(bool isDxRender) = 0;
    virtual bool _canvas_isDxRender() = 0;
    virtual bool _canvas_destroy(LPCANVAS& hCanvas) = 0;
    virtual LPCANVAS _canvas_create(HWND hWnd, UINT width, UINT height) = 0;
    virtual bool _canvas_uninit() = 0;


    virtual int ExSetLastError(int code) = 0;
    virtual int ExGetLastError() = 0;


    // �������
    virtual bool _canvas_clear(const LPCANVAS hCanvas, ARGB argb) = 0;
    virtual bool _canvas_SetTarget(const LPCANVAS hCanvas, LPEX_IMAGE bitmap) = 0;
    virtual bool _canvas_ReTarget(const LPCANVAS hCanvas) = 0;
    virtual LPEX_IMAGE _canvas_GetTarget(const LPCANVAS hCanvas) = 0;
    virtual void _canvas_FreeTarget(const LPCANVAS hCanvas, LPEX_IMAGE image) = 0;
    virtual bool _canvas_begindraw(const LPCANVAS hCanvas) = 0;
    // ƽ��
    virtual bool _canvas_translate(const LPCANVAS hCanvas, float x, float y) = 0;
    // ����
    virtual bool _canvas_scale(const LPCANVAS hCanvas, float x, float y, float scaleX, float scaleY) = 0;
    // ��ת
    virtual bool _canvas_rotate(const LPCANVAS hCanvas, float x, float y, float rotate) = 0;
    virtual bool _canvas_enddraw(const LPCANVAS hCanvas) = 0;
    virtual bool _canvas_resize(const LPCANVAS hCanvas, int cx, int cy) = 0;
    virtual HDC _canvas_getdc(const LPCANVAS hCanvas) = 0;
    virtual void _canvas_releasedc(const LPCANVAS hCanvas) = 0;
    virtual bool _canvas_calctextsize(const LPCANVAS hCanvas, LPEX_FONT font, LPCWSTR text, size_t textLen,
                                      DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight) = 0;
    virtual bool _canvas_getsize(const LPCANVAS hCanvas, UINT* width, UINT* height) = 0;
    // ���Ƽ�ͼ��
    virtual bool _canvas_drawRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECT_F rcF) = 0;
    virtual bool _canvas_fillRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECT_F rcF) = 0;
    virtual bool _canvas_drawEllipse(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCELLIPSE_F rcF) = 0;
    virtual bool _canvas_fillEllipse(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCELLIPSE_F rcF) = 0;
    virtual bool _canvas_drawRoundedRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECTROUNDED_F rcRounded) = 0;
    virtual bool _canvas_fillRoundedRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECTROUNDED_F rcRounded) = 0;
    virtual bool _canvas_drawLine(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCLINEPOINT_F pt) = 0;


    virtual bool _canvas_fillpath(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPEX_PATH path) = 0;
    virtual bool _canvas_drawpath(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPEX_PATH path) = 0;



    virtual bool _canvas_drawimage(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, BYTE alpha) = 0;
    virtual bool _canvas_drawimagerect(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, float right, float bottom, BYTE alpha) = 0;
    virtual bool _canvas_drawimagerectrect(const LPCANVAS hCanvas, LPEX_IMAGE image,
                                           float dstLeft, float dstTop, float dstRight, float dstBottom,
                                           float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha) = 0;
    virtual bool _canvas_drawimagegridPadding(const LPCANVAS hCanvas, LPEX_IMAGE image,
                                              float dstLeft, float dstTop, float dstRight, float dstBottom,
                                              float srcLeft, float srcTop, float srcRight, float srcBottom,
                                              float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom,
                                              DWORD dwFlags, BYTE alpha) = 0;







    virtual bool _canvas_drawtext(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc) = 0;

    virtual bool _canvas_drawtextex(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush,
                                    LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc,
                                    DWORD iGlowsize, ARGB crShadow, LPDRAWTEXTPARAMS lParam, const RECT_F* prclayout, RECT_F* outrcText) = 0;

    virtual bool _canvas_alphablend(const LPCANVAS hCanvas,
                                    float xoriginDest, float yoriginDest, float wDest, float hDest, const LPCANVAS sCanvas,
                                    float xoriginSrc, float yoriginSrc, float wSrc, float hSrc, BYTE Alpha) = 0;
    virtual bool _canvas_bitblt(LPCANVAS hCanvas, int x, int y, int cx, int cy, const LPCANVAS sCanvas, int x1, int y1, DWORD rop) = 0;
    virtual bool _canvas_flush(const LPCANVAS hCanvas) = 0;
    virtual bool _canvas_setrgn(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode = 0) = 0;
    virtual bool _canvas_setClip(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode = 0) = 0;
    virtual bool _canvas_resetClip(const LPCANVAS hCanvas) = 0;

    // ���ÿ����ģʽ, 0=�رտ����, 1=����
    // �˺������� _canvas_SetSmoothingMode, ����һģһ��, ���һ����Ϊ�˺�d2d����һ��
    virtual bool _canvas_SetAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode = 0) = 0;

    // ���ÿ����ģʽ, 0=�رտ����, 1=����
    // �˺������� _canvas_SetAntialiasMode ʵ��, ����һģһ��, ���һ����Ϊ�˺�gdi+����һ��
    virtual bool _canvas_SetSmoothingMode(const LPCANVAS hCanvas, int mode, int* oldMode = 0) = 0;

    // �������忹���ģʽ, 0=Ĭ��, 1=ClearType�����(�������), 2=�Ҷȿ����, 3=��ʹ�ÿ����
    // �˺������� _canvas_SetTextRenderingHint, ����һģһ��, ���һ����Ϊ�˺�d2d����һ��
    virtual bool _canvas_SetTextAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode = 0) = 0;
    // �������忹���ģʽ, 0=Ĭ��, 1=ClearType�����(�������), 2=�Ҷȿ����, 3=��ʹ�ÿ����
    // �˺������� _canvas_SetTextAntialiasMode ʵ��, ����һģһ��, ���һ����Ϊ�˺�gdi+����һ��
    virtual bool _canvas_SetTextRenderingHint(const LPCANVAS hCanvas, int mode, int* oldMode = 0) = 0;



    // ��ˢ
    virtual LPEX_BRUSH _brush_create(ARGB color) = 0;
    // �������Խ��仭ˢ
    // pt1,pt2 = ���, �յ�����
    // color,colorCount = ��ɫ����, ��ɫ��������
    // fillMode = ���ģʽ
    // pRatios, ratiosCount = �����������, ��0-1, ��������
    virtual LPEX_BRUSH _brush_create_LinearGradient(const POINT_F& pt1, const POINT_F& pt2, ARGB* color, DWORD colorCount, int fillMode = 0, const float* pRatios = 0, DWORD ratiosCount = 0) = 0;
    // �������佥�仭ˢ, �����ĵ�������ɢ����
    // pos = �������ĵ��Լ�������չ�ĳ���
    // color,colorCount = ��ɫ����, ��ɫ��������
    // fillMode = ���ģʽ
    // pRatios, ratiosCount = �����������, ��0-1, ��������
    virtual LPEX_BRUSH _brush_create_RadialGradient(const ELLIPSE_F* pos, ARGB* color, DWORD colorCount, int fillMode = 0, const float* pRatios = 0, DWORD ratiosCount = 0) = 0;
    virtual bool _brush_destroy(LPEX_BRUSH& pBrush) = 0;

    virtual LPEX_PEN _pen_create(ARGB color, float width = 1.0f, int style = 0, const float* dashes = 0, int dashesCount = 0) = 0;
    virtual bool _pen_destroy(LPEX_PEN& hPen) = 0;
    virtual bool _pen_setcolor(LPEX_PEN pen, ARGB color) = 0;
    virtual DWORD _pen_getcolor(LPEX_PEN pen) = 0;

    virtual bool _brush_setcolor(LPEX_BRUSH brush, ARGB color) = 0;
    virtual DWORD _brush_getcolor(LPEX_BRUSH brush) = 0;

#ifndef __USEXP // ����xp, ʹ��D2D
    virtual bool _brush_setstartpoint(LPEX_BRUSH brush, float x, float y) = 0;
    virtual bool _brush_setendpoint(LPEX_BRUSH brush, float x, float y) = 0;
    virtual bool _brush_getstartpoint(LPEX_BRUSH brush, float* x, float* y) = 0;
    virtual bool _brush_getendpoint(LPEX_BRUSH brush, float* x, float* y) = 0;
    virtual bool _brush_setcenter(LPEX_BRUSH brush, float x, float y) = 0;
    virtual bool _brush_SetGradientOriginOffset(LPEX_BRUSH brush, float x, float y) = 0;
    virtual bool _brush_SetRadiusX(LPEX_BRUSH brush, float x) = 0;
    virtual bool _brush_SetRadiusY(LPEX_BRUSH brush, float x) = 0;
    virtual bool _brush_GetCenter(LPEX_BRUSH brush, float* x, float* y) = 0;
    virtual bool _brush_GetGradientOriginOffset(LPEX_BRUSH brush, float* x, float* y) = 0;
    virtual float _brush_GetRadiusY(LPEX_BRUSH brush) = 0;
#endif

    // ·��
    virtual LPEX_PATH _path_create(int fillMode) = 0;
    virtual LPEX_PATH _path_createroundedrectangle(const LPCRECTROUNDED_F rcRounded, int fillMode) = 0;
    virtual LPEX_PATH _path_createrectangle(const LPCRECT_F rcRounded, int fillMode) = 0;
    virtual bool _path_destroy(LPEX_PATH& path) = 0;
    virtual bool _path_beginfigure(LPEX_PATH path, const LPCPOINT_F pt) = 0;
    virtual bool _path_endfigure(LPEX_PATH path) = 0;
    virtual bool _path_reset(LPEX_PATH path) = 0;
    virtual bool _path_addarc(LPEX_PATH path, float x1, float y1, float x2, float y2, float radiusX, float radiusY, bool fClockwise) = 0;
    virtual bool _path_addbezier(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2, const POINT_F& pt3, const POINT_F& pt4) = 0;
    virtual bool _path_addline(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2) = 0;
    virtual bool _path_addrect(LPEX_PATH path, const RECT_F& rc) = 0;
    virtual bool _path_addroundedrect(LPEX_PATH path, const RECT_F& rc, float radiusTopLeft, float radiusTopRight, float radiusBottomLeft, float radiusBottomRight) = 0;
    virtual bool _path_open(LPEX_PATH path) = 0;
    virtual bool _path_close(LPEX_PATH path) = 0;
    // ȡ·���߽����
    virtual bool _path_getbounds(LPEX_PATH path, LPRECT_F rc) = 0;
    // ���������Ƿ��ڿɼ�·����
    virtual bool _path_hittest(LPEX_PATH path, const POINT_F& pt) = 0;


    // ͼƬ
    virtual bool _img_destroy(LPEX_IMAGE& img) = 0;
    virtual LPEX_IMAGE _img_create(UINT width, UINT height) = 0;
    virtual LPEX_IMAGE _img_create_frommemory(const void* pData, DWORD len) = 0;
    virtual LPEX_IMAGE _img_create_fromfile(LPCWSTR file) = 0;
    virtual LPEX_IMAGE _img_create_frombitmap(HBITMAP hBitmap, HPALETTE hPalette, bool fPreAlpha) = 0;
    virtual LPEX_IMAGE _img_create_formicon(HICON hIcon) = 0;
    virtual LPEX_IMAGE _img_create_fromstream(IStream* stream) = 0;

    virtual bool _img_getsize(LPEX_IMAGE image, UINT* width, UINT* height) = 0;
    virtual UINT _img_width(LPEX_IMAGE image) = 0;
    virtual UINT _img_height(LPEX_IMAGE image) = 0;
    virtual LPEX_IMAGE _img_copyrect(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height) = 0;
    virtual LPEX_IMAGE _img_copy(LPEX_IMAGE image) = 0;

    // ����λͼ, �ɹ�����true, ʧ�ܷ���false, ��ʹ��ʱ������� _img_unlock �ͷ�����
    // x,y,width,height = �󶥿��, ���Ϊ0ʱ���Զ���ȡͼƬ��С
    // flags = 1=��, 2=д, 3=��д
    virtual bool _img_lock(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData) = 0;
    // 
    virtual bool _img_unlock(LPEX_IMAGE image, EX_IMAGELOCK* lockData) = 0;

    // ȡͼ��֡��
    virtual UINT _img_getframecount(LPEX_IMAGE image) = 0;
    // ���õ�ǰ�֡
    virtual bool _img_selectactiveframe(LPEX_IMAGE image, UINT index) = 0;

#ifndef __USEXP
    // ��D2Dλͼָ���󶥿�߻�ȡͼƬ��Ч������, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
    virtual LPBYTE _img_save_frombitmap(ID2D1Image* pBitmap, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom) = 0;
#endif


    // �ӻ滭������ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
    virtual LPBYTE _img_save_formcanvas(const LPCANVAS hCanvas, SIZE_T* pSize) = 0;
    // �ӻ滭���ָ���󶥿�߻�ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
    virtual LPBYTE _img_save_formcanvasex(const LPCANVAS hCanvas, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom) = 0;
    // ��ͼƬ�����ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
    virtual LPBYTE _img_save(LPEX_IMAGE image, SIZE_T* pSize) = 0;
    // ��ͼƬ���ָ���󶥿�߻�ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
    virtual LPBYTE _img_save_rect(LPEX_IMAGE image, SIZE_T* pSize, UINT left, UINT top, UINT right, UINT bottom) = 0;

    virtual void _img_free(LPBYTE ptr) = 0;

    // ��ͼƬת��gdi��λͼ
    virtual HBITMAP _img_ToGdiBitmap(LPEX_IMAGE image) = 0;



    // λͼ
    virtual LPEX_IMAGE _bitmap_create(UINT width, UINT height, bool fGdi) = 0;


    // ����
    // ��LOGFONTW �ﴴ������
    virtual LPEX_FONT _font_create_logfont(const LPLOGFONTW logFont, DWORD flags) = 0;
    // ���������д�������, ���������Ϊ0�򴴽�Ĭ������
    virtual LPEX_FONT _font_create(LPCWSTR name = 0, LONG lfHeight = 0, FONTSTYLE fontStyle = FONTSTYLE::FontStyleRegular) = 0;
    virtual bool _font_destroy(LPEX_FONT& font) = 0;
    // �ַ��߶�ת�����С, �ַ��߶�һ���� LOGFONT �� lfHeight, �����Сһ���������Ե������С
    virtual int _font_height2size(int height) = 0;
    // �����Сת�ַ��߶�, �ַ��߶�һ���� LOGFONT �� lfHeight, �����Сһ���������Ե������С
    virtual int _font_size2height(int size) = 0;


    // ����

    virtual bool _rgn_destroy(LPEX_RGN& hRgn) = 0;
    virtual LPEX_RGN _rgn_create(float left, float top, float right, float bottom) = 0;
    virtual LPEX_RGN _rgn_create_round(float left, float top, float right, float bottom, float radiusX, float radiusY) = 0;
    virtual LPEX_RGN _rgn_combine(LPEX_RGN hRgnSrc, LPEX_RGN hRgnDst, DWORD nCombineMode, float dstOffsetX, float dstOffsetY) = 0;
    virtual bool _rgn_hittest(LPEX_RGN hRgn, float x, float y) = 0;


    // ���´����豸
    virtual bool recreate_Device() = 0;

};




class IGetEWindow   // �ӿ���
{
public:
    virtual HWND IDRAW_CALL GetEWindow() = 0;

    // ��ȡ�˵������ھ��, ��������������/����/�˵���
    virtual HWND IDRAW_CALL GetMenu() = 0;

    // ��ȡ�˵��������µĲ˵������ھ��
    virtual HWND IDRAW_CALL GetMenuMenu() = 0;
    // ��λ������
    virtual HWND IDRAW_CALL GetMenuLocation() = 0;
    // ���빤����
    virtual HWND IDRAW_CALL GetMenuAlign() = 0;
    // ��׼������
    virtual HWND IDRAW_CALL GetMenuStandard() = 0;
    // ��λ��������ߵ���Ͽ�
    virtual HWND IDRAW_CALL GetMenuComboBox1() = 0;
    // ��λ�������ұߵ���Ͽ�
    virtual HWND IDRAW_CALL GetMenuComboBox2() = 0;
    // ��ѯָ�������Ƿ�Ϊ�˵��µĴ���
    virtual bool IDRAW_CALL isMenuWindow(HWND hWnd) = 0;


    // ��ȡ�����������ѡ��о��
    virtual HWND IDRAW_CALL GetCodeTab() = 0;

    // ��ȡ�����Ĵ��ھ��, �������������д򿪵ı�ǩ
    virtual HWND IDRAW_CALL GetMDIClient() = 0;

    // ��ѯָ�������Ƿ�ΪMDI�µĴ���
    virtual bool IDRAW_CALL isMDIClientWindow(HWND hWnd) = 0;


    //! ѡ����������

    // ��ȡ״̬�еĴ��ھ��, ״̬�а��� ���/��ʾѡ���/ �����/��ʾ��
    virtual HWND IDRAW_CALL GetState() = 0;

    // ��ȡ״̬�е�ѡ��о��, ѡ��ма��� ���/��ʾѡ���/ �����/��ʾ��
    virtual HWND IDRAW_CALL GetStateTab() = 0;

    // ѡ��еĸ�����, ��Ҫ��䱳��
    virtual HWND IDRAW_CALL GetState32770() = 0;

    // ѡ��еĸ����ڵĸ�����, ��Ҫ��䱳��
    virtual HWND IDRAW_CALL GetState32770Parent() = 0;

    // ��ȡ״̬���������༭����
    virtual HWND IDRAW_CALL GetStateTabOutput() = 0;

    // ��ȡ״̬�������ʾ�༭����
    virtual HWND IDRAW_CALL GetStateTabTips() = 0;

    // ��ȡ״̬����ı���������༭����
    virtual HWND IDRAW_CALL GetStateTabVarTableEdit() = 0;

    // ��ȡ���ñ����Ϳ�
    virtual HWND IDRAW_CALL GetStateTabCallTable() = 0;

    // �������ջ�鿴��Ͽ�
    virtual HWND IDRAW_CALL GetStateStackCombobox() = 0;

    // ������������, ����, ���� ��������: �ֽڼ� / ����: 0�ֽ�
    virtual HWND IDRAW_CALL GetStateTypeSize() = 0;

    // ��ȡ���������Ϳ�
    virtual HWND IDRAW_CALL GetStateVarTableTree() = 0;

    // ��ȡ������ʷ�����б��
    virtual HWND IDRAW_CALL GetStateTabClip() = 0;

    // ��ȡ��Ѱ1�б����
    virtual HWND IDRAW_CALL GetStateTabFind1() = 0;

    // ��ȡ��Ѱ2�б����
    virtual HWND IDRAW_CALL GetStateTabFind2() = 0;

    // ��ȡ���ӱ����б��
    virtual HWND IDRAW_CALL GetStateTabMonitorTable() = 0;

    // ��ȡ������д�밴ť
    virtual HWND IDRAW_CALL GetStateTabVarTableWriteBtn() = 0;

    // ��ȡ�������ǩ, �������ж�״̬����Ч
    virtual HWND IDRAW_CALL GetStateTabVarTableStatic() = 0;

    // ��ѯָ�������Ƿ�Ϊ״̬���µĴ���
    virtual bool IDRAW_CALL isStateWindow(HWND hWnd) = 0;


    //! �������������


    // ��ȡ�����еĴ��ھ��, ���� ֧�ֿ�/����/���� ѡ��� ֧�ֿ�/���� ���Ϳ�
    virtual HWND IDRAW_CALL GetWork() = 0;

    // ��ȡ�����е�ѡ��д��ھ��, ѡ��е��Ӵ����� ֧�ֿ�/����/���� ѡ��� ֧�ֿ�/���� ���Ϳ�
    virtual HWND IDRAW_CALL GetWorkTab() = 0;

    // ѡ��еĸ�����, ��Ҫ��䱳��
    virtual HWND IDRAW_CALL GetWork32770() = 0;

    // ѡ��еĸ����ڵĸ�����, ��Ҫ��䱳��
    virtual HWND IDRAW_CALL GetWork32770Parent() = 0;

    // ��ȡ��������Ϳ���, id=1019
    virtual HWND IDRAW_CALL GetWorkProgramTree() = 0;

    // ��ȡ֧�ֿ�����Ϳ���, id=1022
    virtual HWND IDRAW_CALL GetWorkFneTree() = 0;

    // ��ȡ�¼���Ͽ�, ���Կ�����
    virtual HWND IDRAW_CALL GetWorkEventComboBox() = 0;

    // ��ȡ�����Ͽ�, ���Կ�����
    virtual HWND IDRAW_CALL GetWorkControlComboBox() = 0;

    // ��ȡ���Կ�
    virtual HWND IDRAW_CALL GetWorkProperty() = 0;

    // ��ȡ���Կ�����ı༭��
    virtual HWND IDRAW_CALL GetWorkEdit() = 0;

    // ��ѯָ�������Ƿ�Ϊ�������µĴ���
    virtual bool IDRAW_CALL isWorkWindow(HWND hWnd) = 0;

    //! ������������


    // ��ȡ�����Ĵ��ھ��
    virtual HWND IDRAW_CALL GetControl() = 0;
    virtual HWND IDRAW_CALL GetControl32770() = 0;
    virtual HWND IDRAW_CALL GetControl32770Parent() = 0;
    virtual HWND IDRAW_CALL GetControlTree() = 0;

    // ��ѯָ�������Ƿ�Ϊ������µĴ���
    virtual bool IDRAW_CALL isControlWindow(HWND hWnd) = 0;

    // ��ȡ�ײ�״̬��
    virtual HWND IDRAW_CALL GetBottomStatusBar() = 0;
    // ��ȡ�ײ�״̬��������ϼ�ͷ��ť
    virtual HWND IDRAW_CALL GetBottomStatusBarBtn() = 0;
    // ��ȡ�ײ�״̬����Ľ�����
    virtual HWND IDRAW_CALL GetBottomStatusBarProgress() = 0;

    // ��ȡMDI�µ�һ���Ӵ���, ���ų��������������Դ����Ĵ���
    virtual HWND GetMDIClient1() = 0;
    // ��MDI�µ�һ�㴰�����ȡ�����������Դ����Ӵ���
    // hMDIClient1 = MDI�µĵ�һ�㴰��, ���������Ϊ0, ���Զ���ȡ��һ��MDI�µĴ���
    virtual bool GetMDIClientChildWindow(HWND hMDIClient1, EWINDOW_MDICHILD& WndChild) = 0;
};


#define IDRAW_GET_PTR_ITERATOR(_name) \
    static __FN_##_name__ _PFN_CALLBACK_ = 0;\
    if(!_PFN_CALLBACK_) _PFN_CALLBACK_ = (__FN_##_name__)iDrawGetAddress(#_name);\
    if (!_PFN_CALLBACK_) {\
        MessageBoxA(((HWND)0), "δ���ҵ�\""## #_name ##"\"�ӿ�, �����޷���������", #_name ## "δ�ҵ�", 0);\
        exit(0);\
    }

#define IDRAW_FUNC0(_ret, _name) \
        inline _ret IDRAW_CALL _name(){\
        typedef _ret(IDRAW_CALL* __FN_##_name__)();\
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_();}

#define IDRAW_FUNC1(_ret, _name, _1, _2) \
        inline _ret IDRAW_CALL _name(_1 _2){\
        typedef _ret(IDRAW_CALL* __FN_##_name__)(_1);\
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2);}


#define IDRAW_FUNC2(_ret, _name, _1, _2, _3, _4) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4);}

#define IDRAW_FUNC3(_ret, _name, _1, _2, _3, _4, _5, _6) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6);}

#define IDRAW_FUNC4(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8);}

#define IDRAW_FUNC5(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10);}

#define IDRAW_FUNC6(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12);}

#define IDRAW_FUNC7(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14);}

#define IDRAW_FUNC8(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14, _15 _16){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13, _15); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14, _16);}

#define IDRAW_FUNC9(_ret, _name, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) \
        inline _ret IDRAW_CALL _name(_1 _2, _3 _4, _5 _6, _7 _8, _9 _10, _11 _12, _13 _14, _15 _16, _17 _18){\
        typedef _ret(IDRAW_CALL * __FN_##_name__)(_1, _3, _5, _7, _9, _11, _13, _15, _17); \
        IDRAW_GET_PTR_ITERATOR(_name)\
        return _PFN_CALLBACK_(_2, _4, _6, _8, _10, _12, _14, _16, _18);}

// API�ӿڿ�ʼ


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �����Ӿ����ṩ�Ĺ���
// 
// ����:
//      ����1: ���ܱ��, IDC_ ��ͷ����
//      ����2: ���ݹ��ܴ��ݲ���
//      ����3: ���ݹ��ܴ��ݲ���
// 
// ����ֵ:
//      �ο� IDC_ ��ͷ������˵��
IDRAW_FUNC3(int, iDraw_IDCall, int, nCode, WPARAM, wParam, LPARAM, lParam)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �����Ӿ����ṩ�Ĺ���
// 
// ����:
//      ����1: ���ܱ��
//      ����2: ���ݹ��ܴ��ݲ���
//      ����3: ���ݹ��ܴ��ݲ���
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC3(int, iDraw_PluginFunction, int, nCode, WPARAM, wParam, LPARAM, lParam)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰ֧�ֿ�İ汾��
// 
// ����:
//      ����1: �������汾��ָ��, ����Ϊ0
//      ����2: ���մΰ汾��ָ��, ����Ϊ0
//      ����3: ���ձ���汾��ָ��, ����Ϊ0
// 
// ����ֵ:
//      ���ذ汾��unicode�ı�ָ��
IDRAW_FUNC3(LPCWSTR, iDraw_GetVersion, int*, pMajor, int*, pMinor, int*, pBuild)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰ֧�ֿ�İ汾��
// 
// ����:
//      ����1: �������汾��ָ��, ����Ϊ0
//      ����2: ���մΰ汾��ָ��, ����Ϊ0
//      ����3: ���ձ���汾��ָ��, ����Ϊ0
// 
// ����ֵ:
//      ���ذ汾���ı�ָ��
IDRAW_FUNC3(LPCSTR, iDraw_GetVersionA, int*, pMajor, int*, pMinor, int*, pBuild)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰ�����Եİ汾��
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ��ȡ����ֵ, 5.93 ����593, 5.11 ����511, 5.92�������汾, �᷵��592��5922
IDRAW_FUNC0(int, iDraw_GetEVersion)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��֧�ֿ��������ڴ�, �ڴ��Ǵ� iDraw.fne ���ģ���������, �ڲ����õ��� malloc
// 
// ����:
//      ����1: ����ĳߴ�
// 
// ����ֵ:
//      ������ڴ��ַ, ������0, ����ʧ�ܲ����׳��쳣
IDRAW_FUNC1(void*, iDraw_malloc, int, size)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ͷ� iDraw_malloc ������ڴ�, �����ͷŵ��� iDraw.fne ���ģ����ڴ�, �ڲ����õ��� free
//      ���������ģ��ȥ�ͷ� iDraw_malloc ������ڴ�, һ�����һ���Ѳ���ͬ���쳣
// 
// ����:
//      ����1: iDraw_malloc ���صĵ�ַ
// 
// ����ֵ:
//      �޷���ֵ
IDRAW_FUNC1(void, iDraw_free, void*, ptr)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡxmlָ���ڵ�, ���ؽڵ�ָ��, ���ж�д��������Ҫ�ڵ�ָ��
// 
// ����:
//      ����1: xml�ڵ���
//      ����2: �ڵ�ֵ�ı�, ����Ϊ0
//      ����3: �ڵ㲻����ʱ�Ƿ񴴽��ýڵ�, ����Ϊ0
//      ����4: ����һ��ָ����շ��صĽڵ��Ƿ����´�����, ����Ϊ0
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC4(ICTinyXml2_XMLNode, iDraw_Xml_GetNode, LPCWSTR, lpszNodeName, LPCWSTR, lpszNodeValue, BOOL, isCreate, BOOL*, lpIsNewNode)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡxmlָ���ڵ��µ��ӽڵ�, ���ؽڵ�ָ��, ���ж�д��������Ҫ�ڵ�ָ��
// 
// ����:
//      ����1: xml�ڵ�, ������ֵΪ0, ������ iDraw_Xml_GetNode() һ��
//      ����2: Ҫ��ȡ���ӽڵ�ڵ���, ���Ϊ�������ʧ��
//      ����3: �ڵ�ֵ�ı�, ����Ϊ0
//      ����4: �ڵ㲻����ʱ�Ƿ񴴽��ýڵ�, ����Ϊ0
//      ����5: ����һ��ָ����շ��صĽڵ��Ƿ����´�����, ����Ϊ0
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC5(ICTinyXml2_XMLNode, iDraw_Xml_GetChildNode, ICTinyXml2_XMLNode, nodeParent, LPCWSTR, lpszNodeName, LPCWSTR, lpszNodeValue, BOOL, isCreate, BOOL*, lpIsNewNode)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ӽڵ㵽ĳ���ڵ���, �������ӵĽڵ��Ѿ�����, �򲻻����, ���ؽڵ�ָ��
// 
// ����:
//      ����1: xml�ڵ�ָ��
//      ����2: Ҫ�����xml�ڵ���
//      ����3: Ҫ�����xml�ڵ�ֵ, ����Ϊ0
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC3(ICTinyXml2_XMLNode, iDraw_Xml_InsertChildNode, ICTinyXml2_XMLNode, pNode, LPCWSTR, lpszNodeName, LPCWSTR, pszNodeValue)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ָ���ڵ�����ֵ
// 
// ����:
//      ����1: xml�ڵ���
//      ����2: Ҫ���õ�������
//      ����3: Ҫ���õ�����ֵ
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC3(bool, iDraw_Xml_SetAttrValue, ICTinyXml2_XMLNode, node, LPCWSTR, lpszAttrName, LPCWSTR, lpszAttrValue)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡָ���ڵ�����ֵ, ��������ֵ�ı�ָ��
// 
// ����:
//      ����1: xml�ڵ���
//      ����2: Ҫ��ȡ��������
//      ����3: Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC3(LPCWSTR, iDraw_Xml_GetAttrValue, ICTinyXml2_XMLNode, node, LPCWSTR, lpszAttrName, BOOL*, isAttribute)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ָ���ڵ�ֵ
// 
// ����:
//      ����1: xml�ڵ�ָ��
//      ����2: Ҫ���õĽڵ�ֵ
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC2(bool, iDraw_Xml_SetNodeValue, ICTinyXml2_XMLNode, node, LPCWSTR, lpszNodeValue)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����xml������
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC0(bool, iDraw_Xml_SaveFile)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡָ���ڵ�ֵ, ���ؽڵ�ֵ�ı�ָ��
// 
// ����:
//      ����1: xml�ڵ�ָ��
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC1(LPCWSTR, iDraw_Xml_GetNodeValue, ICTinyXml2_XMLNode, node)




//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯ָ������, ������һЩĬ�ϵĻ滭��Ϣ, �ڲ������������ػ�
//      ֧�ֿ��������໯����ͨ������ӿڽ��ж��η�װ
// ����:
//      ����1: ��Ҫ���໯�Ĵ��ھ��
//      ����2: �������໯��Ϣ�ص��ĺ���, ��5������, ��������Լ��Ϊ __stdcall, ����ֵΪ LRESULT ����
//              ǰ4����������ͨ���໯��ͬ, ������Ǹ��������ǵ�������ӿڵķ���ֵ
//      ����3: �Ƿ���Ҫɾ���߿�, Ϊtrue��ѱ߿���ص���ʽ��ɾ��
//      ����4: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_SubclassWindow, HWND, hWnd, pfn_SubClassWndProc, proc, bool, reMoveBorder, bool, isSetBack)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ѵ��ڵ���Ϣת���ػ����ڵĹ���ȥ����
// ����:
//      ����1: ��Ҫ�ػ��Ĵ��ھ��
//      ����2: ��Ҫ�������Ϣ
//      ����3: ����1
//      ����4: ����2
//      ����5: ����2
//      ����6: ����ԭ��������Ϣ�Ļص���ַ
//      ����7: ���մ��������Ϣ����ֵ
// ����ֵ:
//      �����Ƿ�����������Ϣ
IDRAW_FUNC6(bool, iDraw_DrawWindowProc, HWND, hWnd, UINT, message, WPARAM, wParam, LPARAM, lParam, WNDPROC, oldProc, LRESULT*, pRet)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡĬ�ϲ���
// ����:
//      ����1: ��Ҫ�ػ��Ĵ��ھ��, Ϊ0���ڷ��غ��Լ������ھ����ֵ
//      ����2: DRAW_STRUCT_WINDOW_ARGUMENT �ṹָ��, �ڲ���Ĭ�ϲ�����䵽����ṹ��
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
IDRAW_FUNC2(bool, iDraw_DrawGetDefArg, HWND, hWnd, LPIDRAW_STRUCT_WINDOW_ARGUMENT, arg)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��һ�����㴰���ػ������Ӿ�����ͬ���Ӿ�Ч��
// ����:
//      ����1: ��Ҫ�ػ��Ĵ��ھ��
//      ����2: IDRAW_STRUCT_WINDOW_ARGUMENT �ṹָ��, Ϊ0��ʹ���뵱ǰ����ƥ�����ɫ��Ϣ
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
IDRAW_FUNC2(LPOBJSTRUCT, iDraw_DrawWindow, HWND, hWnd, LPIDRAW_STRUCT_WINDOW_ARGUMENT, arg)



//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������ʽ����, ������ں��Ӿ�������ͬ���Ӿ�Ч��
// ����:
//      ����1: ���ڵ���չ��ʽ, ������ο� CreateWindowEx ��˵��
//      ����2: ���ڵı���, Ϊ0��û�б���, unicode�ı�
//      ����3: ���ڵ���ʽ, ������ο� CreateWindowEx ��˵��
//      ����4: ��������Ļ�����λ��
//      ����5: ��������Ļ�Ķ���λ��
//      ����6: ���ڿ��
//      ����7: ���ڸ߶�
//      ����8: ������, ���Ϊ0, ��û�и�����, ������򴴽��Ĵ���ʼ�����������֮��
//      ����9: IDRAW_STRUCT_WINDOW_ARGUMENT �ṹָ��, Ϊ0��ʹ���뵱ǰ����ƥ�����ɫ��Ϣ
// ����ֵ:
//      ���ر��洰����Ϣ�Ľṹָ��, ����ĸ�����ɫ��Ϣ��������ṹ�б���
IDRAW_FUNC9(LPOBJSTRUCT, iDraw_CreateWindow, DWORD, dwExStyle, LPCWSTR, lpWindowName, DWORD, dwStyle, int, x, int, y, int, cx, int, cy, HWND, hWndParent, LPIDRAW_STRUCT_WINDOW_ARGUMENT, arg)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ҽӴ��ڻ滭�¼�, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
//      �������ڻ滭 ϵͳͼ��, ����, ϵͳ��ʱ, �ᴥ���¼�
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: ���ջ滭�ص��ĺ���, ����ԭ�� = typedef bool(CALLBACK* PFN_SysDraw)(LPDRAW_PAINTSTRUCT dps);
//      ����3: ���ݵ��ص�������Ĳ���
// ����ֵ:
//      �����Ƿ�ɹ�
IDRAW_FUNC3(bool, iDraw_Event_Draw, HWND, hWnd, PFN_SysDraw, pfn, LPARAM, param)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ҽ�ϵͳ��ť����¼�, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: ���ջ滭�ص��ĺ���, ����ԭ�� = typedef bool(CALLBACK* PFN_SysBtnClick)(int id, BOOL isRButton, LPARAM param);
//      ����3: ���ݵ��ص�������Ĳ���
// ����ֵ:
//      �����Ƿ�ɹ�
IDRAW_FUNC3(bool, iDraw_Event_SysButtonClick, HWND, hWnd, PFN_SysBtnClick, pfn, LPARAM, param)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ϵͳ��ť, ϵͳ��ť�����Ǵ�������, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: ��ť��ID, �������ͬID, ���滻��ֵ
//      ����3: ��ť��ֵ
// ����ֵ:
//      ���������Ӱ�ť������, ϵͳ��ť�����Ǵ�������
IDRAW_FUNC3(int, iDraw_AddSysButton, HWND, hWnd, int, id, LPARAM, param)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ɾ��ϵͳ��ť, ϵͳ��ť�����Ǵ�������, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫɾ���İ�ťID
//      ����3: ָ������2��λ�û���ID, DRAW_WND_ ��ͷ����
// ����ֵ:
//      �����Ƿ�ɾ���ɹ�
IDRAW_FUNC3(bool, iDraw_DelSysButton, HWND, hWnd, int, nPosition, int, flags)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ϵͳ��ť, ϵͳ��ť�����Ǵ�������, ֻ�е��� iDraw_SubclassWindowEx, ���� iDraw_SubclassWindow �Ĵ��ڲ���Ч
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫ�����λ��, -1����뵽���(�����), 0����뵽��һ��(���ұ�)
//      ����3: Ҫ����İ�ťID
//      ����4: ��ť��ֵ
//      ����5: ָ������2��λ�û���ID, DRAW_WND_ ��ͷ����
// ����ֵ:
//      ���ز����λ��
IDRAW_FUNC5(int, iDraw_InsSysButton, HWND, hWnd, int, nPosition, int, id, LPARAM, param, int, flags)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ϵͳ��ť��Ŀ��ֵ, ��������ǰ����Ŀ��ֵ
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫ���õİ�ťID��λ��
//      ����3: ��ť��ֵ
//      ����4: ָ������2��λ�û���ID, DRAW_WND_ ��ͷ����
// ����ֵ:
//      ��������ǰ����Ŀ��ֵ
IDRAW_FUNC4(LPARAM, iDraw_SetButtonParam, HWND, hWnd, int, nPosition, LPARAM, param, int, flags)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡϵͳ��ť��Ŀ��ֵ
// ����:
//      ����1: �е��ù� iDraw_SubclassWindowEx �Ĵ��ھ��, ���� iDraw_SubclassWindow ���صĴ��ھ��
//      ����2: Ҫ��ȡ�İ�ťID��λ��
//      ����3: ָ������2��λ�û���ID, DRAW_WND_ ��ͷ����
// ����ֵ:
//      ���ذ�ť����Ŀ��ֵ
IDRAW_FUNC3(LPARAM, iDraw_GetButtonParam, HWND, hWnd, int, nPosition, int, flags)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����ԭ�ص�����
// ����:
//      ����1: ���ھ��, ��Ϣ�ص���ĵ�һ������
//      ����2: ��Ϣֵ, ��Ϣ�ص���ĵڶ�������
//      ����3: ����1, ��Ϣ�ص���ĵ���������
//      ����4: ����2, ��Ϣ�ص���ĵ��ĸ�����
//      ����5: �������, ��Ϣ�ص���ĵ��������
// ����ֵ:
//      ����ϵͳĬ�ϴ���ķ���ֵ
IDRAW_FUNC5(LRESULT, iDraw_CallWindowProc, HWND, hWnd, UINT, message, WPARAM, wParam, LPARAM, lParam, LPOBJSTRUCT, pData)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ���ť, ��ť������Ϊ Button
// ����:
//      ����1: ��ť�Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Button, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ��༭��, �༭�������Ϊ Edit
// ����:
//      ����1: �༭��Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: �Ƿ��ػ�������
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC5(LPOBJSTRUCT, iDraw_Subclass_Edit, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, bool, isScroll)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ���ǩ, ��ǩ������Ϊ static
// ����:
//      ����1: ��ǩ�Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Static, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ�״̬��, ״̬��������Ϊ msctls_statusbar32
// ����:
//      ����1: ״̬���Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_StatusBar, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ����Ϳ�, ���Ϳ������Ϊ SysTreeView32
// ����:
//      ����1: ���Ϳ�Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: ���Ϳ����Ŀ�߶�
//      ����6: �Ƿ��ػ����Ϳ�Ĺ�����
//      ����7: �Ƿ����ȵ�Ч��
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC7(LPOBJSTRUCT, iDraw_Subclass_TreeView, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight, bool, isScroll, bool, isHot)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ��б��, �б�������Ϊ ListBox
// ����:
//      ����1: �б��Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: �б�����Ŀ�߶�
//      ����6: �Ƿ��ػ�������
//      ����7: �Ƿ����ȵ�Ч��
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC7(LPOBJSTRUCT, iDraw_Subclass_ListBox, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight, bool, isScroll, bool, isHot)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ������б��, �����б�������Ϊ SysListView32
// ����:
//      ����1: �����б��Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: �����б�����Ŀ�߶�
//      ����6: �Ƿ��ػ�������
//      ����7: �Ƿ����ȵ�Ч��
//      ����8: �б���뷽ʽ
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC8(LPOBJSTRUCT, iDraw_Subclass_ListView, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight, bool, isScroll, bool, isHot, int, fmt)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ�������, ������������Ϊ ToolbarWindow32
// ����:
//      ����1: �������Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Tool, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)


// ��������: 
//      ���໯���ػ�ѡ���, ѡ��е�����Ϊ SysTabControl32
// ����:
//      ����1: ѡ��д��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_Tab, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ�������, ������������Ϊ msctls_updown32
// ����:
//      ����1: �������Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC4(LPOBJSTRUCT, iDraw_Subclass_msctls_updown32, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���໯���ػ���Ͽ�, ��Ͽ������Ϊ ComboBox
// ����:
//      ����1: ��Ͽ�Ĵ��ھ��
//      ����2: �Ƿ�ɾ���߿�
//      ����3: ���Ϊtrue, ������ SetClassLong() ���ñ�����ˢ, ��ͬ�����Ĵ��ڶ����ܵ�Ӱ��
//      ����4: ��Ҫɾ��������ʽ, Ϊ0��ɾ��
//      ����5: ��Ͽ���б����߶�
// ����ֵ:
//      ���ش�������, ���Ե��� iDraw_SetLong() �Դ������ݽ����޸�
//      �����Ҫ�Լ�������Ϣ, ����� iDraw_SetLong(��������, IGL_WNDPROC, �ص�����), ���Բο� IGL_WNDPROC ����˵��
IDRAW_FUNC5(LPOBJSTRUCT, iDraw_Subclass_ComboBox, HWND, hWnd, bool, reMoveBorder, bool, isSetBack, DWORD, reMoveClassStyle, int, nItemHeight)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ػ�ָ���ؼ��Ĺ�����, ����滭�е�����, �������пؼ���֧��
// ����:
//      ����1: ��Ҫ�ػ��������Ĵ��ھ��
//      ����2: ����ǳ����б��, ����������ǳ����б��ı���߶�, �������ر���
//      ����3: ��������Ϳ�, ��Ϊ��, ���Ϳ�����ر���
//      ����4: �Ƿ����ԭ����
// ����ֵ:
//      ���ع���������ָ��
IDRAW_FUNC4(LPVOID, iDraw_DrawScroll, HWND, hWnd, int, nListViewItemHeight, bool, isTreeView, bool, isCallSetScrollInfo)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ʼ���滭��Ϣ, �������� WM_PAINT ��Ϣ�µ���, ���׵ĺ����� iDraw_EndPaint()
//      ����� BeginPaint, �ᴴ���ڴ�DC, �����ڴ�DC
//      ��������õ�������ɫ, ������ˢ, �߿򻭱ʶ�ѡ��DC
//      ��� �����ô˽ӿ�, Ҳ���԰�һ�㴦��WM_PAINT��Ϣʱ����
// 
// ����:
//      ����1: ���໯�ĵ��������, �����ǵ������໯�ӿڵķ���ֵ
//      ����2: BEGIN_END_PAINT_STRUCT �ṹָ��, �ڲ���ѳ�ʼ�������ݼ�¼������ṹ��
//      ����3: �Ƿ�ΪDX��Ⱦ, �����DX��Ⱦ, ���ڲ������DX�ĳ�ʼ������
//      ����4: �Ƿ���䱳��, ���Ϊtrue, �򷵻ص�HDC�Ѿ������˱���
// 
// ����ֵ:
//      �����ڴ�DC, DX��Ⱦ�򷵻�0
IDRAW_FUNC4(HDC, iDraw_BeginPaint, LPOBJSTRUCT, pData, BEGIN_END_PAINT_STRUCT*, data, bool, isDxRender, bool, isFillBack)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ύ�滭��Ϣ, �������� WM_PAINT ��Ϣ�µ���, ���ô˺���ǰ�����Ѿ������� iDraw_BeginPaint()
//      ��iDraw_BeginPaint() ʱ�������ڴ�DC������������
//      ������ EndPaint �ύ�滭��Ϣ
// 
// ����:
//      ����1: BEGIN_END_PAINT_STRUCT �ṹָ��, �˺���������ýṹ������
// 
// ����ֵ:
//      �����Ƿ񿽱��ɹ�
IDRAW_FUNC1(bool, iDraw_EndPaint, BEGIN_END_PAINT_STRUCT*, data)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡDX�滭����ָ��, һ���������ò���, ������ʹ�� iDraw_ ��ͷ�Ļ滭��غ���
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ����dx�滭����ָ��
IDRAW_FUNC0(ID2DDraw*, iDraw_GetDxObject)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ʼ��ʱ�Ĳ���
// 
// ����:
//      ����1: �ṩһ��ָ��, ���� ���� iDraw_Interface �ӿڵĵ�һ������
//      ����2: �ṩһ��ָ��, ���� ���� iDraw_Interface �ӿڵĵڶ�������
//      ����3: �ṩһ��ָ��, ���� ���� iDraw_Interface �ӿڵĵ���������
//      ����4: �ṩһ��ָ��, ���� ���� iDraw_Interface �ӿڵĵ��ĸ�����
// 
// ����ֵ:
//      ����ֵ��δʹ��
IDRAW_FUNC4(int, iDraw_InterfaceArgument, LPIDRAW_INFO*, pInfo, IGetEWindow**, eWnd, int*, reserved1, int*, reserved2)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ػ���Ӱ����, ע��: ������� IRE_DRAW_BCAKGROUND ����¼�, ���ú�ᴥ������¼�, �벻Ҫ������¼��µ��øýӿ�
// 
// ����:
//      ����1: �Ƿ��첽�ػ�, �������Ҫ���ϻ滭������Ϊtrue, Ϊtrue��������һ�ε���20�����ʼ�ػ�
// 
// ����ֵ:
//      ������Ӱ���ھ��
IDRAW_FUNC1(HWND, iDraw_UpdateShadow, bool, bAsync)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ļ�����ext, �ɹ�������Դ���, ʧ�ܷ���0
// 
// ����:
//      ����1: ext���ļ�·��
// 
// ����ֵ:
//      ����ext�ļ�����Դ���, ����ʹ�� User_GetIcon/User_GetIconDx ��������������ȡͼ��
IDRAW_FUNC1(IDRAW_HRESOURCE, iDraw_LoadExtFromFile, LPCWSTR, pszFile)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ڴ����ext, �ɹ�������Դ���, ʧ�ܷ���0
// 
// ����:
//      ����1: ext���ļ�����
//      ����2: �����ݳߴ�
// 
// ����ֵ:
//      ����ext�ļ�����Դ���, ����ʹ�� User_GetIcon/User_GetIconDx ��������������ȡͼ��
IDRAW_FUNC2(IDRAW_HRESOURCE, iDraw_LoadExtFromMemory, const void*, ptr, int, size)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡĬ���������Դ���, ���ݴ��ھ����ȡ
// 
// ����:
//      ����1: ���ĸ������ϻ�ȡ, ����ֻ�� ��׼������, ����/֧�ֿ�/����� ���Ϳ��⼸����Ĭ��ͼ��
// 
// ����ֵ:
//      ���ش��ڹ�������Դ���
IDRAW_FUNC1(IDRAW_HRESOURCE, iDraw_GetDefControlIcon, HWND, hWnd)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ȡ��������а󶨴�����Դ�Ļص�
// 
// ����:
//      ����1: ���Ψһ��ʶ��
// 
// ����ֵ:
//      ȡ���󶨳ɹ�����true, ʧ�ܷ���false
IDRAW_FUNC1(bool, iDraw_UnBindWindow, LPCSTR, guid)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����Դ�󶨵�������, �´��ػ��������ʱ��������Դ��ȡͼ��
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: Ҫ�󶨵Ĵ��ھ��
//      ����3: ���ص���Դ���, ����������Ϊ0, �����һ������Ϊ0, �򲻻�����趨��ͼ��
//      ����4: �ص�����, �����������Ҫ����ͼ��ʱ����, Ϊ0��ʹ��Ĭ�ϵ���Դ����, ���û��Ĭ�ϵ���Դ�򲻼���
// 
// ����ֵ:
//      �󶨳ɹ�����true, ʧ�ܷ���false
IDRAW_FUNC4(bool, iDraw_BindWindow, LPCSTR, guid, HWND, hWnd, IDRAW_HRESOURCE, hResource, pfn_GetWindowIcon, callback)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ӵ��ھ����ȡ��Դ, ���û�а󶨹��������, �򷵻�0
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: ֮ǰ�󶨵Ĵ��ھ��
//      ����3: ���հ�ʱ���ݵĻص�����, ����Ϊ0
// 
// ����ֵ:
//      �󶨳ɹ�����true, ʧ�ܷ���false
IDRAW_FUNC3(IDRAW_HRESOURCE, iDraw_GetResourceFromHwnd, LPCSTR, guid, HWND, hWnd, pfn_GetWindowIcon*, callback)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����Դ������ȡͼ������
// 
// ����:
//      ����1: ��Դ���, ʹ�� iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon ���ص�ֵ
//      ����2: ͼ��ID
//      ����3: RECT_F�ṹָ��, �������ID����Դ�е�λ��
//      ����4: Ҫ��ȡ��״̬, Ŀǰֻ֧�� STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL ��4��
//      ����5: dpi
// 
// ����ֵ:
//      ����ͼ������, ���û��ͼ���򷵻�0
IDRAW_FUNC5(LPWINDOW_ICON_DATA, iDraw_GetIcon, IDRAW_HRESOURCE, hResource, LPCWSTR, pszID, RECT_F*, prc, int, state, UINT, dpi)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����Դ������ȡͼ������
// 
// ����:
//      ����1: ��Դ���, ʹ�� iDraw_LoadExtFromFile/iDraw_LoadExtFromMemory/iDraw_GetDefControlIcon ���ص�ֵ
//      ����2: ͼ��ID
//      ����3: RECT_F�ṹָ��, �������ID����Դ�е�λ��
//      ����4: Ҫ��ȡ��״̬, Ŀǰֻ֧�� STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL ��4��
//      ����5: dpi
// 
// ����ֵ:
//      ����ͼ������, ���û��ͼ���򷵻�0
IDRAW_FUNC5(LPWINDOW_ICON_DATA, iDraw_GetIconDx, IDRAW_HRESOURCE, hResource, LPCWSTR, pszID, RECT_F*, prc, int, state, UINT, dpi)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ͼ���������ȡdx����gdi+��ͼƬָ��, gdi+���ص��� Gdiplus::Bitmap* ָ��, dx�����ڲ�ʹ�õ�ͼƬָ��
// 
// ����:
//      ����1: ͼ������, iDraw_GetIcon ���� iDraw_MakeResourceReturn ���ص�ֵ
//      ����2: �Ƿ񷵻�gdiplusλͼ���, Ϊtrue�򷵻�Gdiplusλͼ���, false�򷵻�dxλͼ���
// 
// ����ֵ:
//      ����λͼ���, ���ݵڶ�����������
IDRAW_FUNC2(void*, iDraw_GetImageHandle, LPWINDOW_ICON_DATA, pIconData, bool, isGdiplus)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ʼ�����, ֻ�г�ʼ�����������չ�˵�����Ӳ˵���Ͱ���Դ
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: �����ģ����
//      ����3: ����Ĳ����Ϣ�ṹָ��, ��ȡ����ʼ��ǰ���뱣֤���ָ��Ĵ��
//      ����4: �Ƿ������չ�˵�, ���Ϊfalse, ����� iDraw_MenuExt_Insert ��ʧ��
//      ����5: ���Ӿ���ͨѶ�Ļص�����
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
IDRAW_FUNC5(bool, iDraw_InitPlugin, LPCSTR, guid, HMODULE, hModule, PLUGIN_INFO*, etInfo, pfn_MenuExt_Command, pfnMenuExt_Command, pfn_IDraw_Callback, callback)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ȡ����ʼ�����, ��֮ǰ��ʼ��ʱ���뵽��չ�˵��Ĳ˵���ɾ��
// 
// ����:
//      ����1: ���Ψһ��ʶ��
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
IDRAW_FUNC1(bool, iDraw_UnInitPlugin, LPCSTR, guid)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ѳ˵�����뵽��չ�˵���, �˵�����¼��� iDraw_InitPlugin ��Ļص���������
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: �����λ��, -1����뵽���
//      ����3: ��ʶ
//      ����4: �˵���ID, ���uFlags����MF_POPUP ����������ǵ���ʽ�Ĳ˵����
//      ����5: �˵���
//      ����6: �˵���Ŀ��ֵ
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
IDRAW_FUNC6(bool, iDraw_MenuExt_Insert, LPCSTR, guid, UINT, uPosition, UINT, uFlags, UINT_PTR, uIDNewItem, LPCWSTR, lpNewItem, LPARAM, param)





//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ���໯���ص�ָ�������, ���������໯������Ϣ�ص�������ĵ��������
// 
// ����:
//      ����1: ���໯���ص�ָ��, ���������໯������Ϣ�ص�������ĵ��������
//      ����2: ��������, ������ο� IGL_ ��ͷ����
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC2(LONG_PTR, iDraw_GetLong, LPOBJSTRUCT, pData, int, index)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �������໯���ص�ָ�������, ���������໯������Ϣ�ص�������ĵ��������, ��������ǰ������
// 
// ����:
//      ����1: ���໯���ص�ָ��, ���������໯������Ϣ�ص�������ĵ��������
//      ����2: ��������, ������ο� IGL_ ��ͷ����
//      ����3: Ҫ���õ�����
// 
// ����ֵ:
//      ��������ǰ��ֵ
IDRAW_FUNC3(LONG_PTR, iDraw_SetLong, LPOBJSTRUCT, pData, int, index, LONG_PTR, value)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ������ˢ, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫ�Ļ�ˢֻ�ᴴ��һ��
// 
// ����:
//      ����1: ��Ҫ�����Ļ�ˢ, RGB��ɫ
// 
// ����ֵ:
//      ���ش����õ�GDI��ˢ, ���ô˺��������Ļ�ˢ�������ֶ��ͷ�
IDRAW_FUNC1(HBRUSH, iDraw_CreateBrush, COLORREF, cr)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ������ˢ, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫ�Ļ�ˢֻ�ᴴ��һ��
// 
// ����:
//      ����1: ��Ҫ�����Ļ�ˢ, ARGB��ɫ
// 
// ����ֵ:
//      ���ش����õĻ�ˢ, ���ô˺��������Ļ�ˢ�������ֶ��ͷ�
IDRAW_FUNC1(LPEX_BRUSH, iDraw_CreateBrushDx, DWORD, cr)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: ��Ҫ�����Ļ���, ARGB��ɫ
// 
// ����ֵ:
//      ���ش����õ�GDI����, ���ô˺��������Ļ��ʲ������ֶ��ͷ�
IDRAW_FUNC3(HPEN, iDraw_CreatePen, int, style, int, width, COLORREF, cr)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: ��Ҫ�����Ļ���, ARGB��ɫ
// 
// ����ֵ:
//      ���ش����õ�GDI����, ���ô˺��������Ļ��ʲ������ֶ��ͷ�
IDRAW_FUNC3(LPEX_PEN, iDraw_CreatePenDx, int, style, float, width, DWORD, cr)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: ������
//      ����2: �����С
//      ����3: ������ʽ, 0=һ��, 1=����, 2=б��, 4=�»���, 8=ɾ����, �����ʽ��Ҫ������
//
// ����ֵ:
//      ���ش����õ�GDI����, ���ô˺������������岻�����ֶ��ͷ�
IDRAW_FUNC3(HFONT, iDraw_CreateFont, LPCWSTR, name, int, lfHeight, int, style)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: LPLOGFONTW �ṹָ��
//
// ����ֵ:
//      ���ش����õ�GDI����, ���ô˺������������岻�����ֶ��ͷ�
IDRAW_FUNC1(HFONT, iDraw_CreateFontLf, LPLOGFONTW, lf)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: ������
//      ����2: �����С
//      ����3: ������ʽ, 0=һ��, 1=����, 2=б��, 4=�»���, 8=ɾ����, �����ʽ��Ҫ������
//
// ����ֵ:
//      ���ش����õ�DX����, ���ô˺������������岻�����ֶ��ͷ�
IDRAW_FUNC3(LPEX_FONT, iDraw_CreateFontDx, LPCWSTR, name, int, lfHeight, int, style)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��������, �������ֶ��ͷ�, �����ڽ���ʱ���ͷ�, ��ͬ��ɫֻ�ᴴ��һ��
// 
// ����:
//      ����1: LPLOGFONTW �ṹָ��
//
// ����ֵ:
//      ���ش����õ�DX����, ���ô˺������������岻�����ֶ��ͷ�
IDRAW_FUNC1(LPEX_FONT, iDraw_CreateFontLfDx, LPLOGFONTW, lf)

//////////////////////////////////////////////////////////////////////////
// ��������: 
// ��ʱδ���⹫��
//      ͨ�����ھ������Ϣֵ��ȡ����Ϣ�󶨵Ļص�
// 
// ����:
//      ����1: ���ھ��
//      ����2: ע�����Ϣֵ
//      ����3: ����������ָ��, һ����Առ��20���ֽ�
//      ����4: �����������Ա��
// 
// ����ֵ:
//      ����д�뻺������Ա��
IDRAW_FUNC4(int, iDraw_Event_GetMessageInfo, HWND, hWnd, UINT, message, LPREGISTER_MESSAGE_INFO*, pArr, int, bufSize)


//////////////////////////////////////////////////////////////////////////
// ��������: 
// ��ʱδ���⹫��
//      ͨ���¼�ID��ȡ�ص���Ϣ
// 
// ����:
//      ����1: �¼�ID
//      ����2: ����������ָ��, һ����Առ��24���ֽ�
//      ����3: �����������Ա��
// 
// ����ֵ:
//      ����д�뻺������Ա��
IDRAW_FUNC3(int, iDraw_Event_GetEventInfo, UINT, nEvent, LPREGISTER_EVENT_INFO*, pArr, int, bufSize)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ע����Ϣ�ص�
// 
// ����:
//      ����1: ���Ψһ��ʶ
//      ����2: ע���ĸ����ڵ���Ϣ, ����ô��ڲ�����, �����ʧ��
//      ����3: ע�����Ϣֵ, �����ڴ��������Ϣʱ����ûص�����
//      ����4: ����ǰ�Ļص�����, ���ڴ��������Ϣǰ��������ص�����, Ϊ0��ȡ���ҽ�����¼�
//      ����5: �����Ļص�����, �����Ѿ����������Ϣ���������ص�����, Ϊ0��ȡ���ҽ�����¼�
//          ����4 �� ����5 �Ļص�ԭ�Ͷ�����ͬ��
//              ��Ҫ5������, ��������ֵ, ������������ƥ��, ����ᵼ�¶�ջ��ƽ��Ӷ����³������
//              ������ �ص�����(���ھ��, ��Ϣֵ, ����1, ����2, ��������ָ��)
//              �����໯������Ϣ����һ��
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
IDRAW_FUNC5(int, iDraw_Event_RegisterMessage, LPCSTR, guid, HWND, hWnd, UINT, message, pfn_SubClassWndProc, pfnBefore, pfn_SubClassWndProc, pfnAfter)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ע���¼��ص�
// 
// ����:
//      ����1: Ψһ��ʶ��
//      ����2: ��Ҫע����¼�, �ᴫ�ݵ��ص�������
//      ����3: ����ǰ�Ļص���������, ����������ڵ��� ����4 ʱ���ݹ�ȥ
//      ����4: ����ǰ�Ļص�����, Ϊ0��ȡ���ҽ�����¼�
//      ����5: �����Ļص���������, ����������ڵ��� ����6 ʱ���ݹ�ȥ
//      ����6: �����Ļص�����, Ϊ0��ȡ���ҽ�����¼�
//          ����4 �� ����6 �Ļص�ԭ�Ͷ�����ͬ��
//              ��Ҫ3������, ��������ֵ, ������������ƥ��, ����ᵼ�¶�ջ��ƽ��Ӷ����³������
//              ������ �ص�����(�¼�����, �ص�����, �¼�ָ��)
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
IDRAW_FUNC6(int, iDraw_Event_RegisterEvent, LPCSTR, guid, UINT, nEvent, LONG_PTR, paramBefore, pfn_Register_Event, pfnBefore, LONG_PTR, paramAfter, pfn_Register_Event, pfnAfter)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ȡ��ָ�������������Ϣ�ص�
// 
// ����:
//      ����1: ���Ψһ��ʶ��
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
IDRAW_FUNC1(int, iDraw_UnRegister_Message, LPCSTR, guid)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ȡ��ָ������������¼��ص�
// 
// ����:
//      ����1: ���Ψһ��ʶ��
// 
// ����ֵ:
//      ���ش�����, 0=�ɹ�
IDRAW_FUNC1(int, iDraw_UnRegister_Event, LPCSTR, guid)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡpInfo����Ϣ, �ӿڳ���ĵ�һ������
// 
// ����:
//      ����1: �ӿڳ���ĵ�һ������
//      ����2: ��ȡ�ı��, IGI_ ��ͷ����
//      ����3: ��ȡ�Ĳ���, ���ݱ�Ŵ���
// 
// ����ֵ:
//      ��ȡ����ֵ
IDRAW_FUNC3(int, iDraw_GetInfo, LPIDRAW_INFO, pInfo, int, index, LPARAM, argument)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����pInfo����Ϣ, �ӿڳ���ĵ�һ������, ��������, �����������ȫ��ʹ��, һ������ͻᵼ�±���
// 
// ����:
//      ����1: �ӿڳ���ĵ�һ������
//      ����2: ��ȡ�ı��, IGI_ ��ͷ����
//      ����3: Ҫ���õ�ֵ
// 
// ����ֵ:
//      ��������֮ǰ��ֵ
IDRAW_FUNC3(int, iDraw_SetInfo, LPIDRAW_INFO, pInfo, int, index, LONG_PTR, value)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �Ѻ����ŵ������Դ����߳������
// 
// ����:
//      ����1: ������ַ, û�з���ֵ, ����û�з���ֵ��һ�������Ͳ���, ������ջ��ƽ�⵼�±���
//      ����2: ���ݵ�������Ĳ���
// 
// ����ֵ:
//      ʧ�ܷ���0, �ɹ����ط�0ֵ
IDRAW_FUNC2(bool, iDraw_EWindowThreadCallback, pfn_EWindowThreadCallback, pfnCallback, LPARAM, param)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ͼƬ������������Դ����ֵ, ����ָ����뱣֤���, �ڲ����������ڴ������洫�ݽ������ı���λͼ
// 
// ����:
//      ����1: ͼƬ���ݵ�ַ
//      ����2: ͼƬ���ݴ�С
//      ����3: ���Ƶ�ǰ��Ŀ��ʾ���ı�, �����޸�ԭ����ı�, ֻ�ǻ滭ʱ��ʾ����ı�, ����Ϊ0
//      ����4: ��ע�ı�, ����Ϊ0
//      ����5: �ı�ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
//      ����6: ����ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
// 
// ����ֵ:
//      ������Դ����ֵ, ����ֵ��ʹ��ʱ������� iDraw_FreeResourceReturn ���ͷ�
IDRAW_FUNC6(LPWINDOW_ICON_DATA, iDraw_MakeResourceReturn, const void*, pImg, int, size, LPCWSTR, text, LPCWSTR, remarks, DWORD*, crText, DWORD*, crBack)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ͷ� iDraw_MakeResourceReturn() ���ɵķ���ֵ
// 
// ����:
//      ����1: iDraw_MakeResourceReturn() �ķ���ֵ
// 
// ����ֵ:
//      �����Ƿ��ͷųɹ�
IDRAW_FUNC1(bool, iDraw_FreeResourceReturn, LPWINDOW_ICON_DATA, pData)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ڴ����gdi+ͼƬ
// 
// ����:
//      ����1: ͼƬ����
//      ����2: ͼƬ���ݴ�С
// 
// ����ֵ:
//      �ɹ�����gdi+ͼƬָ��, ʧ�ܷ���0
IDRAW_FUNC2(void*, iDraw_LoadGpImageFromMemory, const void*, pData, int, size)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �ͷŴ� iDraw_LoadGpImageFromMemory() ���ص�ֵ
// 
// ����:
//      ����1: iDraw_LoadGpImageFromMemory() ���ص�ֵ
// 
// ����ֵ:
//      �ɹ�����true, ʧ�ܷ���false
IDRAW_FUNC1(bool, iDraw_FreeGpImage, void*, hImg)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��Ӱ�ť���ӿڹ�������, �ɹ����ع�������ťID, ������������ť�����ʱ������ID���ݹ�ȥ
// 
// ����:
//      ����1: ���Ψһ��ʶ��, �������/�ػ��¼�ʱ������������Ļص�����
//      ����2: ��ť��ID, �����¼�ʱ������ID���ݹ�ȥ
//      ����3: ��ť����, ����Ϊ0, Ϊ0��û�б���, ����ƶ���ȥʱ��������ʾ˵������, �Ӵ���ʾ, ������ⲻ����ʾ�ڹ�������
//      ����4: ��ť˵��, ����Ϊ0, Ϊ0��û��˵��, ����ƶ���ȥʱ��������ʾ˵��
//      ����5: ��Ŀ��ֵ, ��������¼����ݹ�ȥ
//      ����6: ͼƬ����ָ��, �������ʾ���������ϵ�ͼ��
//      ����7: ͼƬ���ݴ�С
//      ����8: �滭�ص�����, pfn_ToolDrawButton ����
//              typedef int(IDRAW_CALL* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);
// ����ֵ:
//      �ɹ����ع�������ťID, ʧ�ܷ���0
IDRAW_FUNC8(int, iDraw_Tool_InsertButton, LPCSTR, guid, int, idCommand, LPCWSTR, lpszTitle, LPCWSTR, lpszTips, LPARAM, param, const void*, pImageData, int, nImageSize, pfn_ToolDrawButton, pfnDraw)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ɾ���ӿڹ������İ�ť, ֻ��ɾ���Լ������ӵİ�ť
// 
// ����:
//      ����1: ���Ψһ��ʶ��, �������/�ػ��¼�ʱ������������Ļص�����
//      ����2: ��ť��ID, ���ʱ��ID
// 
// ����ֵ:
//      �����Ƿ�ɹ�
IDRAW_FUNC2(bool, iDraw_Tool_DeleteButton, LPCSTR, guid, int, idCommand)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ�ӿڹ������İ�ť��Ϣ, ֻ�ܻ�ȡ�Լ������ӵİ�ť
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: ��ť��ID, ���ʱ��ID
//      ����3: Ϊ0�򲻽���, ���հ�ť����
//      ����4: Ϊ0�򲻽���, ���հ�ť˵��
//      ����5: Ϊ0�򲻽���, ������Ŀ��ֵ
//      ����6: Ϊ0�򲻽���, ����ͼƬ����ָ��
//      ����7: Ϊ0�򲻽���, ����ͼƬ���ݴ�С
//      ����8: Ϊ0�򲻽���, ���ջ滭�ص�����
// 
// ����ֵ:
//      �����Ƿ�ɹ�
IDRAW_FUNC8(bool, iDraw_Tool_GetButtonData, LPCSTR, guid, int, idCommand, LPWSTR*, lpszTitle, LPWSTR*, lpszTips, LPARAM*, param, LPCVOID*, pImageData, int*, nImageSize, pfn_ToolDrawButton*, pfnDraw)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ýӿڹ������İ�ť��Ϣ, ֻ�������Լ������ӵİ�ť
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: ��ť��ID, ���ʱ��ID
//      ����3: Ϊ0������, ���ð�ť����
//      ����4: Ϊ0������, ���ð�ť˵��
//      ����5: Ϊ0������, ������Ŀ��ֵ
//      ����6: Ϊ0������, ����ͼƬ����ָ��
//      ����7: Ϊ0������, ����ͼƬ���ݴ�С
//      ����8: Ϊ0������, ���û滭�ص�����
// 
// ����ֵ:
//      �����Ƿ�ɹ�
IDRAW_FUNC8(bool, iDraw_Tool_SetButtonData, LPCSTR, guid, int, idCommand, LPCWSTR, lpszTitle, LPCWSTR, lpszTips, LPARAM, param, const void*, pImageData, int, nImageSize, pfn_ToolDrawButton, pfnDraw)


//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���������Խ�������, ��������������൱�ڵ��������� ������ϵͳ����() ����
// 
// ����:
//      ����1: ���ܺ�, ���ܺ���ο��������ṩ�� lib2.h �ļ�, NES_, NAS_, NRS_ ��ͷ����
//      ����2: ����� lib2.h ���ܺ�˵��
//      ����3: ����� lib2.h ���ܺ�˵��
// 
// ����ֵ:
//      ����� lib2.h ���ܺ�˵��
IDRAW_FUNC3(int, iDraw_EIDE_Notify, int, nMsg, DWORD, dwParam1, DWORD, dwParam2)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����������IDE�ӿڹ���, ������������൱�ڵ��� iDraw_EIDE_Notify(NES_RUN_FUNC, ����1, ����[����2, ����3]);
// 
// ����:
//      ����1: ���ܺ�, ���ܺ���ο��������ṩ�� PublicIDEFunctions.h �ļ�, 
//      ����2: ����� PublicIDEFunctions.h ���ܺ�˵��
//      ����3: ����� PublicIDEFunctions.h ���ܺ�˵��
// 
// ����ֵ:
//      ����ù��ܱ���������, δ�������򷵻ؼ�
IDRAW_FUNC3(bool, iDraw_EIDE_RunFunctions, int, nMsg, DWORD, dwParam1, DWORD, dwParam2)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��һ���ı���ɫת����RGB��ɫ
// 
// ����:
//      ����1: Ĭ����ɫ, ����ʧ�ܷ��������ɫ
//      ����2: ָ����ɫ�ı�ָ��
// 
// ����ֵ:
//      ����ת���õ�RGB��ɫֵ
IDRAW_FUNC2(COLORREF, iDraw_STR2RGB, COLORREF, crDef, LPCWSTR, lpszColorText)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��һ���ı���ɫת����ARGB��ɫ
// 
// ����:
//      ����1: Ĭ����ɫ, ����ʧ�ܷ��������ɫ
//      ����2: ָ����ɫ�ı�ָ��
// 
// ����ֵ:
//      ����ת���õ�ARGB��ɫֵ
IDRAW_FUNC2(ARGB, iDraw_STR2ARGB, ARGB, crDef, LPCWSTR, lpszColorText)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��RGB��ɫת�����ı� RGB(1,2,3)
// 
// ����:
//      ����1: Ҫת����rgb
//      ����2: ����20��WCHAR������ָ��
// 
// ����ֵ:
//      ����д���ַ���
IDRAW_FUNC2(int, iDraw_RGB2STR, COLORREF, crDef, LPWSTR, lpszBuffer)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��RGB��ɫת�����ı� ARGB(255,1,2,3)
// 
// ����:
//      ����1: Ҫת����argb
//      ����2: ����26��WCHAR������ָ��
// 
// ����ֵ:
//      ����д���ַ���
IDRAW_FUNC2(int, iDraw_ARGB2STR, ARGB, crDef, LPWSTR, lpszBuffer)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��������
// 
// ����:
//      ����1: Ҫ��ȡ�Ĵ��ھ��
// 
// ����ֵ:
//      �ɹ���������ָ��, ʧ�ܷ���0
IDRAW_FUNC1(LPOBJSTRUCT, iDraw_GetWindowData, HWND, hWnd)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ���봰�ڿ��ӵľ���
// 
// ����:
//      ����1: ���վ��ε�ָ��(����������������λ��)
// 
// ����ֵ:
//      �����Ƿ��ȡ�ɹ�
IDRAW_FUNC1(BOOL, iDraw_GetCodeVisibleRect, RECT*, prc)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����InlineHook, �ڲ��ṩ����hook��ʽ
// 
// ����:
//      ����1: ��hook�ĵ�ַ
//      ����2: ����hook�ĺ�����ַ
//      ����3: ʹ�õ�hook��, 0=detoursHook, 1=MinHook, Ŀǰֻ�ṩ������
// 
// ����ֵ:
//      ���ر�hook��ַ��ԭ��ַ, ����ֱ�ӵ��������ַ
IDRAW_FUNC3(LPVOID, iDraw_InlineHook, LPVOID, pOldFun, LPVOID, pNewFun, int, hookMode)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����InlineHook, �ڲ��ṩ����hook��ʽ
// 
// ����:
//      ����1: iDraw_InlineHook �ķ���ֵ
// 
// ����ֵ:
//      �����Ƿ�ȡ���ɹ�
IDRAW_FUNC1(bool, iDraw_UnInlineHook, LPVOID, pOldFun)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �������Ի���ʽ�ĵ���ʽ�˵�, ���м��뵽�������ʽ�˵�����Ӳ˵����ᱻ�Ի�
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ���ص���ʽ�˵����
IDRAW_FUNC0(HMENU, iDraw_CreatePopupMenu)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡIDE�������(IEIDE_CodeManage)��ָ��
// 
// ����:
//      ����1: ���봰�ڴ��ھ��, Ϊ0���ȡ��ǰ����
// 
// ����ֵ:
//      ���ش��봰�ڶ�Ӧ�� IEIDE_CodeManage ��ָ��, ʧ�ܷ���0
IDRAW_FUNC1(IEIDE_CodeManage*, iDraw_GetCodeManage, HWND, hCode)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰ�������, �����ǰ����ʼҳ��������������򷵻�0
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ���ص�ǰ�������, �����ǰ����ʼҳ��������������򷵻�0
IDRAW_FUNC0(HWND, iDraw_GetCodeWindow)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ������λ��, ����ͨ������¼��޸ı߿�ﵽָ���滭������λ��
// 
// ����:
//      ����1: pfn_GetScrollRect �ص�����
//      ����2: ��������ָ��
// 
// ����ֵ:
//      �ɹ����ط�0, ʧ�ܷ���0
IDRAW_FUNC2(BOOL, iDraw_Scroll_GetRect, pfn_GetScrollRect, pfnCallback, LPOBJSTRUCT, pData)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���������λ��, ����ͨ������¼��޸Ĵ��ڷǿͻ���λ��
// 
// ����:
//      ����1: pfn_CalcScroll �ص�����
//      ����2: ��������ָ��
// 
// ����ֵ:
//      �ɹ����ط�0, ʧ�ܷ���0
IDRAW_FUNC2(BOOL, iDraw_Scroll_CalcRect, pfn_CalcScroll, pfnCallback, LPOBJSTRUCT, pData)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      �滭������
// 
// ����:
//      ����1: pfn_DrawScroll �ص�����
//      ����2: ��������ָ��
// 
// ����ֵ:
//      �ɹ����ط�0, ʧ�ܷ���0
IDRAW_FUNC2(BOOL, iDraw_Scroll_Draw, pfn_DrawScroll, pfnCallback, LPOBJSTRUCT, pData)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰMDIClient ����������ʾʲô����
//      -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ��������ֵ֮һ: -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
IDRAW_FUNC0(int, iDraw_GetMDIClientType)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��ǰMDIClient ����������ʾʲô��������
//      0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
// 
// ����:
//      �޲���
// 
// ����ֵ:
//      ��������ֵ֮һ: 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
IDRAW_FUNC0(int, iDraw_GetMDIClientCodeType)

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ��������������
// 
// ����:
//      ����1: ����������ָ��
//      ����2: ���չ���������ָ��, ����ǰ��Ҫ�� IDRAW_STRUCT_SCROLL_DATA.cbSize ��ֵ
// 
// ����ֵ:
//      �����Ƿ�ɹ�
IDRAW_FUNC2(BOOL, iDraw_GetScrollData, LPVOID, pScroll, IDRAW_STRUCT_SCROLL_DATA*, ptr)

// API�ӿڽ���











// �ӿ�_IDE����༭�����
class IEIDE_CodeManage
{
public:
    // �Ӵ��ھ���г�ʼ������
    virtual bool init(HWND hWnd) = 0;
    // ����
    virtual void destroy() = 0;
    // ��ȡ���λ��, λ��Ϊ����ڲ����
    virtual POINT GetCursorPos() = 0;
    // ���ù��λ��, ��ѱ��к���Ϊ0
    virtual void SetCursorPos(POINT pt) = 0;
    // ��ȡѡ����Ϣ
    // pStartCol = ������ʼ��
    // pEndCol = ���ս�����
    // pNowLine = ���յ�ǰ��
    // pNowLine = ���յ�ǰ��
    // pSel = ����ѡ�з�Χ
    virtual void GetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0) = 0;

    // ����ѡ����Ϣ
    // pStartCol = ��ʼ��ָ��, Ϊ0��д��
    // pEndCol = ������ָ��, Ϊ0��д��
    // pNowLine = ��ǰ��ָ��, Ϊ0��д��
    // pNowLine = ��ǰ��ָ��, Ϊ0��д��
    // pSel = ѡ�з�Χָ��, Ϊ0��д��
    virtual void SetSelInfo(int* pStartCol = 0, int* pEndCol = 0, int* pNowLine = 0, POINT* pSel = 0) = 0;

    // �Ƿ�ѡ�д���
    virtual bool IsSelCode() = 0;

    // ��ȡѡ������
    virtual int GetSelLineCount() = 0;

    // ��ȡѡ������
    virtual int GetSelColumnCount() = 0;

    // ��ȡ����Ϣ
    // lineNumber = �к�
    // flags = ���ʶ, ETAG
    // pData = ������ָ��, EBase*
    virtual int GetLineInfo(int lineNumber, int* flags, int* pData) = 0;

    // ��ȡ��������
    virtual int GetCodeType() = 0;

    // ��ȡ����ָ��
    virtual void* GetObjectThis() = 0;

    // ���ö���ָ��
    virtual void SetObjectThis(void* pThis) = 0;

    // ��궨λ
    virtual void CursorLocation(int lineNumber) = 0;

    // ��֤��ʾ���λ��
    virtual void EnsurVisibleCursor() = 0;

    // ��ȡ�����ı�, ���ص��ı���Ҫ���� �������� free() �������ͷ�
    // nTableIndex = ����
    // nTableindexCol = ����
    virtual LPSTR GetCodeText(int nTableIndex, int nTableindexCol) = 0;

    // �����ڴ�
    virtual void* alloc(int size) = 0;
    // �ͷ��ڴ�, ֻ�ܴ��ݱ������ alloc() �������ص�ָ��, ��������ָ����ܻ������
    virtual void free(void* p) = 0;

    // ��ȡ��������ƫ��
    // nTableIndex = ����
    // nTableindexCol = ����
    // type = ��������ָ��
    // offset = ����ƫ��ָ��
    // pData = ����ָ��, EBase*
    virtual bool GetCodeDataOffset(int nTableIndex, int nTableindexCol, int* pType, int* pOffset, int* pData) = 0;

    // ��ȡ����λ��, λ��Ϊ������
    virtual POINT GetCursorTablePos() = 0;

    // ���ù���λ��, λ��Ϊ������
    // pt = ������
    virtual void SetCursorTablePos(POINT pt) = 0;

    // ��ȡ��ǰ����ָ��Ĵ������
    virtual HWND GetWindow() = 0;
};










