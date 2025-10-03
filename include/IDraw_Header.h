#pragma once
#include <control/WndBase.h>
#pragma comment(lib,"Comctl32.lib")
#include <e/CGetEWindow.h>
#include <control/CLayered.h>
#include <control/EControlID.h>
#include <assist/CFileRW.h>
#include "iDraw_virtual.h"
#include "iDraw_Event_typedef.h"

#define GUID_INLINE_HOOK                L"{790EFE11-D2FB-49A6-864E-D733038CBEE0}"   // GetProp ���ýӿ�����hookʱ��¼�������Դ�����, �ͷŲ����Ҫ�������ͷ�, INLINEHOOK_CALLBACK, key��¼hook���صĵ�ַ
#define GUID_SCROLL_CALLBACK_EWINDOW    L"{790EFE11-D2FB-49A6-864E-D733038CBEE1}"   // GetProp �������ص�, �ͷŲ��ʱ��Ҫ������ȡ��ָ������ͷ�, SCROLL_CALLBACK
#define GUID_TABPARAMS                  L"{9AAA4259-9DDD-4D53-9E74-C9C1CDEAF315}"   // ��¼״̬���ұ�ѡ��е��Ӽ���Ŀ��ֵ, std::vector<int>*

#define HOOKMODE_DETOURS    0
#define HOOKMODE_MINHOOK    1

struct SCROLL_CALLBACK_SET_DATA
{
    std::unordered_set<pfn_GetScrollRect>* pGet;
    std::unordered_set<pfn_CalcScroll>* pCalc;
    std::unordered_set<pfn_DrawScroll>* pDraw;
};

struct INLINEHOOK_CALLBACK_DATA
{
    LPVOID pOldFun;     // ��hook�ĵ�ַ
    LPVOID pNewFun;     // ����hook�ĵ�ַ, key��hook�ķ���ֵ
    int hookMode;       // hook��ģʽ, 0=detoursHook, 1=MinHook
};

typedef std::map<HWND, SCROLL_CALLBACK_SET_DATA> SCROLL_CALLBACK;
typedef std::map<LPVOID, INLINEHOOK_CALLBACK_DATA> INLINEHOOK_CALLBACK;

#ifdef _DEBUG
#include <assert.h>
#else
  #ifndef assert
    #define assert(_a) 
  #endif
#endif

#define IDRAW_FONT              L"΢���ź�"
#define IDRAW_FONT_LENGTH       10
#define __HOOK_MOVE_BREPAINT    1211

#define WINDOW_CAPTION_HEIGHT   30      // ���ڱ���߶�
#define WINDOW_BORDER_WIDTH     5       // ���ڱ߿���, �������ǿɵ������ڴ�С�Ŀ��
#define WINDOW_STATUS_HEIGHT    20      // �ײ�״̬���ĸ߶�
#define WINDOW_TOP_STATE        24      // �����϶�����ʱ������״̬���߶�, �������4����ť
#define WINDOW_CUSTOMTAB_HEIGHT 24      // �Ի�ѡ��и߶�, ���ѡ������л����봰�ڵ�ѡ���
#define WINDOW_AUTO_HEIGHT      24      // �Զ�����ʱ�Ĵ��ڸ߶�, ����ұߵĻ����ǿ��
//#define WINDOW_MENU_WIDTH       60      // �˵����, һ���˵���Ŀ��
#define WINDOW_SPACING          8       // ����֮��ļ��

#define WINDOW_MIN_MDIWIDTH     200     // MDI������С���, ����������, �����ʱ���봦��
#define WINDOW_MIN_MDIHEIGHT    200     // MDI������С�߶�, ����״̬��ʱ���봦��
#define WINDOW_MINTRACKSIZE     135     // ����������С��������ĳߴ�
#define EVENT_MAX_ITEM          128     // ͬһ����Ϣ/�¼����֧�ֵĻص���, Ŀǰ128�㹻ʹ����, �����������پͱ�ʾ֧����ô����ע��
// 1=��ʾ, 2=���, 3=���ñ�, 4=���ӱ�, 5=������, 6=��Ѱ1, 7=��Ѱ2, 8=������ʷ9=֧�ֿ�, 10=����, 11=����12 �����
#define TAB_PARAM_TIPS          1       // ��ʾ, ״̬���������ʾ�Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_OUTPUT        2       // ���, ״̬�����������Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_CALLTABLE     3       // ���ñ�, ״̬������ĵ��ñ��Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_MONITORTABLE  4       // ���ӱ�, ״̬������ļ��ӱ��Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_VARTABLE      5       // ������, ״̬������ı������Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_FIND1         6       // ��Ѱ1, ״̬���������Ѱ1�Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_FIND2         7       // ��Ѱ2, ״̬���������Ѱ2�Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_CLIP          8       // ������ʷ, ״̬������ļ�����ʷ�Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_FNE           9       // ֧�ֿ�, �����������֧�ֿ��Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_PROGRAM       10      // ����, ����������ĳ����Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_PROPER        11      // ����, ����������������Ӽе���Ŀ��ֵ, ���������Ŀ��ֵ�жϵ�����ĸ���Ŀ
#define TAB_PARAM_CONTROLBOX    12      // �����, �ұߵ������




#define IDRAW_FNE_IDRAW     L"iDraw"           // ֧�ֿ�ʹ�õ�Ŀ¼, ���������Ŀ¼, ϵͳ���������
#define IDRAW_FNE_DBGPATH   L"iDraw\\debug\\"  // ֧�ֿ���Բ��Ŀ¼
#define IDRAW_FNE_NAME      L"��IDE�Ӿ���"      // ֧�ֿ���
#define IDRAW_FNE_NAMEA     "��IDE�Ӿ���"      // ֧�ֿ���

struct IDRAW_INFO;

DECLARE_HANDLE(IDRAW_HPLUGIN);  // ������



typedef struct EMENU_DATA
{
    HMENU hMenu;
    wchar_t text[260];
    int id;
    DWORD dwItemData;
    RECT rcItem;        // �˵����
    bool isPopup;       // ��꾭���Ƿ��������˵�
    bool isHide;        // �Ƿ���������˵���
}*LPEMENU_DATA;

typedef struct TABCHANGE_INFO
{
    NMHDR hdr;
    int index;      // �µ�ѡ����
    int indexOld;   // �л�ǰ��ѡ����
    int selMode;    // ����ѡ���¼���ģʽ, TAB_SEL_MODE ö�ٳ���
}*LPTABCHANGE_INFO;


typedef INT(WINAPI* pfnEIDE_Notify) (INT nMsg, DWORD dwParam1, DWORD dwParam2);    // ������IDE֪ͨ�¼�
typedef LRESULT(CALLBACK* WND_EWINDOW_SUBPROC)(HWND, UINT, WPARAM, LPARAM, LPOBJSTRUCT);

//#define CUSTOMTAB_INFO_WINDOW               1   // ���������
//#define CUSTOMTAB_INFO_ASSEMBLY             2   // ��ͨ����
//#define CUSTOMTAB_INFO_ASSEMBLY_WINDOW      3   // ���ڳ���
//#define CUSTOMTAB_INFO_CLASS                4   // ��
//#define CUSTOMTAB_INFO_GLOBALVAR            5   // ȫ�ֱ���
//#define CUSTOMTAB_INFO_STRUCT               6   // ��������
//#define CUSTOMTAB_INFO_DLL                  7   // DLL������
//#define CUSTOMTAB_INFO_CONST                8   // ����
//#define CUSTOMTAB_INFO_IMAGE                9   // ͼƬ��Դ
//#define CUSTOMTAB_INFO_SOUND                10  // ������Դ
enum CUSTOMTAB_INFO
{
    CUSTOMTAB_INFO_UNDEFINE                 = 0,   // δ����
    CUSTOMTAB_INFO_WINDOW                   = 1,   // ���������
    CUSTOMTAB_INFO_ASSEMBLY                 = 2,   // ��ͨ����
    CUSTOMTAB_INFO_ASSEMBLY_WINDOW          = 3,   // ���ڳ���
    CUSTOMTAB_INFO_CLASS                    = 4,   // ��
    CUSTOMTAB_INFO_GLOBALVAR                = 5,   // ȫ�ֱ���
    CUSTOMTAB_INFO_STRUCT                   = 6,   // ��������
    CUSTOMTAB_INFO_DLL                      = 7,   // DLL������
    CUSTOMTAB_INFO_CONST                    = 8,   // ����
    CUSTOMTAB_INFO_IMAGE                    = 9,   // ͼƬ��Դ
    CUSTOMTAB_INFO_SOUND                    = 10,  // ������Դ
    CUSTOMTAB_INFO_CLOSEHOT                 = 11,  // �رհ�ť�ȵ�
    CUSTOMTAB_INFO_CLOSEDOWN                = 12,  // �رհ�ť����
};


// �Ӿ�������Ϣ
struct PLUGIN_INFO;
// ���ֲ����Ϣ�ṹ
struct ETOOLS_PLUGIN_INFO;

typedef int (CALLBACK* pfn_IDraw_Callback)(UINT, WPARAM, LPARAM);
// ���ֶ����˵�������
typedef int (WINAPI* pfn_etMenuSelect)(int id, HMENU hMenu);
typedef int(CALLBACK* pfn_MenuExt_Command)(int id, LPARAM param);
typedef int (CALLBACK* pfn_IDraw_UnInterface)();
typedef PLUGIN_INFO* (CALLBACK* pfn_IDraw_Interface)(IDRAW_INFO*, IGetEWindow*, int, int);
typedef void* (CALLBACK* pfn_IDraw_ConfigChange)(int, int, int);
typedef void(CALLBACK* pfn_EWindowThreadCallback)(int);
typedef int(CALLBACK* pfn_GetAllSubclassWindow)(LPOBJSTRUCT* pArr, int count);
typedef LPWSTR(CALLBACK* pfn_CopyStrW)(LPCWSTR, LPWSTR&);
typedef LPSTR(CALLBACK* pfn_CopyStrA)(LPCSTR, LPSTR&);
typedef void(CALLBACK* pfn_FreeStr)(void*);

typedef struct IDRAW_STRUCT_SCROLL_DATA
{
    DWORD cbSize;       // �ṹ�ߴ�, ����ǰ��Ҫ�ȸ������Ա��ֵ
    float btn1Size;     // ��ť1�Ŀ��, ��������߰�ť���, �����Ƕ��߰�ť�߶�
    float btn2Size;     // ��ť2�Ŀ��, �������ұ߰�ť���, �����ǵױ߰�ť�߶�
    float btnThumbSize; // ������, ��������������������Ŀ��, �����Ǹ߶�, �����ǿ��
}*LPIDRAW_STRUCT_SCROLL_DATA;

typedef struct TAB_DRAWINFO
{
    LPEX_BRUSH  hbrState;       // ���������򱳾���ˢ
    LPEX_BRUSH  hbrStateAct;    // ���������򼤻�ʱ�ı�����ˢ

    LPEX_BRUSH  hbrSel;         // ѡ����Ӽ�ѡ�л�ˢ
    LPEX_BRUSH  hbrExtern;      // ѡ����Ӽб�����ˢ, �����Ҫ�������䵽ѡ��е��ұ�
    LPEX_BRUSH  hbrBack;        // ѡ��б�����ˢ
    LPEX_BRUSH  hbrHot;         // ѡ����ȵ㻭ˢ
    LPEX_BRUSH  hbrDown;        // ѡ��а��»�ˢ
    LPEX_BRUSH  hbrText;        // ѡ����ı���ˢ
    LPEX_BRUSH  hbrTextHot;     // ѡ����ı��ȵ㻭ˢ
    LPEX_BRUSH  hbrTextDown;    // ѡ����ı����»�ˢ

    LPEX_PEN    hPenBorder;     // ѡ�����߿���ɫ
    LPEX_PEN    hPenDot;        // ����״̬���м��û����Ļ���
    LPEX_PEN    hPenDotAct;     // ����״̬���м�㼤��ʱ�Ļ���
}*LPTAB_DRAWINFO;

typedef struct TAB_INFO
{
    DWORD size;                 // �̶���ߵĿ�ȸ߶�, ��λ=���, ��λ=�߶�
    int minWidth;               // ��ؼ��������С���
    DWORD padding;              // ÿ��ѡ������ռ�, ��λ = �����, ��λ = ���߶�
    TAB_DRAWINFO draw;          // �滭��Ҫ�Ķ���
    int state;                  // ���״̬ == STATE::DOWN ���ǹرհ�ť������
    int indexSel;               // ѡ����
    int indexDown;              // ��갴�µ���Ŀ
    int indexHot;               // ����ƶ����ȵ���Ŀ
    int isUpdown;               // �Ƿ���ʾ������
    int lState;                 // ��������ߵ�״̬
    int rState;                 // �������ұߵ�״̬
    int indexFirst;             // ��һ����ʾ���Ӽ�
}*LPTAB_INFO;

// ���ֲ����Ϣ�ṹ
typedef struct ETOOLS_PLUGIN_INFO
{
    DWORD cbSize;           // �ṹ�ߴ�
    LPCSTR name;            // �������
    LPCSTR remarks;         // ����
    LPCSTR version;         // �汾
    LPCSTR author;          // ����
    DWORD Versionlimit;     // �汾����
    void* Dependence;       // �������
}*LPETOOLS_PLUGIN_INFO;

struct IDRAW_STRUCT_TOOL_INSERTEX;
// �����Ϣ, �����ֲ���ṹ�̳й���
typedef struct PLUGIN_INFO : ETOOLS_PLUGIN_INFO
{
    LPCSTR  guid;                           // ���Ψһ��ʶ��, ֱ�Ӽ�¼��ʼ��ʱ���ݵ�ָ��, hash �����Ա���ڲ�������guid�ĵ�ַ
    pfn_MenuExt_Command pfnMenuExt_Command; // �˵��������¼�
    pfn_IDraw_Callback  pfnCallback;        // ���Ӿ���ͨѶ�Ļص�����

    // ����ĳ�Ա���ڲ�ʹ�õ�, ��������ľ�ǰ��ĳ�Ա
    HMENU   hPopupMenu;             // ����Լ��ĵ���ʽ�˵����, �Լ����ֻ�ܼ��뵽�Լ��Ĳ˵�����
    HMODULE hModule;                // ��ǰ�����ģ����
    int     fileSize;               // ��ǰģ���ļ��ߴ�, �ͷŲ��ʱ��Ҫ�ͷ�ģ��+�ļ���С�������ĵ�ַ
    int     hash;                   // ����Ĺ�ϣֵ, ͨ�����ֵ���ҵ�����������ڴ����λ��

    pfn_IDraw_UnInterface   pfnUnInterface;     // ȡ����ʼ���ӿ�
    pfn_IDraw_Interface     pfnInterface;       // ��ʼ���ӿ�
    pfn_IDraw_ConfigChange  pfnEConfigChange;   // ���øı�ӿ�

    IDRAW_STRUCT_TOOL_INSERTEX* pToolButton;            // ��ǰ�����������ť����
    int                         nToolButtonCount;       // ��ǰ�����������ť����
    int                         nToolButtonBufferCount; // ��ǰ�����������ť����������
    LPCSTR                      dllName;                // ��ǰ�����dll�ļ���
}*LPPLUGIN_INFO;

typedef struct IDRAW_STRUCT_TOOL_INSERTEX : IDRAW_STRUCT_TOOL_INSERT
{
    int key;                    // ����ṹ��map���key
    wstr title;                 // ����, lpszTitle = title.c_str()
    wstr tips;                  // ��ʾ�ı�, lpszTips = tips.c_str()
    LPEX_IMAGE hImage;          // �滭ʹ�õ�ͼƬ
    LPPLUGIN_INFO plugin;       // �����¼����õĲ���ص�

}*LPIDRAW_STRUCT_TOOL_INSERTEX;

// xml����, ���캯�����봫��Ҫ�����Ľڵ���, ���ұ��뱣֤����ָ��Ĵ��
class XMLConfig;

typedef LRESULT             (WINAPI* pfn_ALLWINDOW_MESSAGE) (HWND hWnd, UINT message, WPARAM* wParam, LPARAM* lParam, LPOBJSTRUCT pData);
typedef Gdiplus::Bitmap*    (WINAPI* pfn_GetResourceData)   (LPCWSTR id, RECT* prc, STATE state);
typedef LPOBJSTRUCT         (__cdecl* pfn_GetDataFromHwnd)  (HWND hWnd);
typedef int                 (__cdecl* pfn_event_before)     (LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM], UINT nEvent, LPVOID pArgs, int* isReturn, bool isStop);
typedef int                 (__cdecl* pfn_event_after)      (LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM], UINT nEvent, LPVOID pArgs, int evtCount, bool isStop);
typedef HBRUSH              (__cdecl* pfn_CreateBrush)      (COLORREF cr);
typedef HPEN                (__cdecl* pfn_CreatePen)        (int iStyle, int cWidth, COLORREF color);
typedef LPEX_BRUSH          (__cdecl* pfn_CreateBrushDx)    (DWORD argb);
typedef LPEX_PEN            (__cdecl* pfn_CreatePenDx)      (DWORD argb, float width);
typedef LPEX_FONT           (__cdecl* pfn_CreateFontDx)     (LPCWSTR name, LONG lfHeight, FONTSTYLE fontStyle);
typedef LPEX_FONT           (__cdecl* pfn_CreateFontLfDx)   (LPLOGFONTW lf);
typedef LPEX_FONT           (__cdecl* pfn_CreateFontDpi)    (LPEX_FONT, UINT dpi);
typedef LPOBJSTRUCT         (__cdecl* pfn_GetDefData)       (LPOBJSTRUCT pData);
typedef void                (__cdecl* pfn_ReloadDxResource) ();
typedef LONG_PTR            (CALLBACK* pfn_GetInfo)         (IDRAW_INFO*, int, LPARAM);
typedef LONG_PTR            (CALLBACK* pfn_SetInfo)         (IDRAW_INFO*, int, LONG_PTR, BOOL);
typedef void                (CALLBACK* pfn_InfoChanged)     (int, LONG_PTR, LONG_PTR, bool);
typedef int                 (__cdecl*  pfn_Scale)           (int);




// ����������ָ���˵�����, ���ݲ˵�ID
typedef LRESULT(__cdecl* pfn_call_menu)(int id);


typedef LPPLUGIN_INFO(__cdecl* pfn_Plugin_init)(LPCSTR guid, HMODULE hModule, ETOOLS_PLUGIN_INFO* etoolsInfo, pfn_MenuExt_Command pfnMenuExt_Command, pfn_IDraw_Callback callback);
typedef LPPLUGIN_INFO(__cdecl* pfn_Plugin_GetInfo)(LPCSTR guid);
typedef LPPLUGIN_INFO(__cdecl* pfn_Plugin_Query)(LPCSTR hash);
typedef bool(__cdecl* pfn_Plugin_UnInit)(LPCSTR guid);
typedef int (__cdecl* pfn_Plugin_Enum)(LPPLUGIN_INFO* pInfo, int bufCount);
typedef bool(__cdecl* pfn_MenuExt_Insert)(LPCSTR guid, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPARAM param);
typedef bool(__cdecl* pfn_MenuExt_Delete)(LPCSTR guid);


// ����˵������ڶ����Ĳ˵�����
// id = �˵�id, ͨ�����id�������ĸ���ť
// lpszMenuText = ��ʾ�Ĳ˵��ı�
// index = Ҫ���뵽�ĸ�λ��, ��10��ʼ, -1����뵽���, ǰ��10���˵����������Դ���, ���������10��ʼ
// hPopupMenu = �Ƿ�Ҫ�����˵�, �����Ҫ�����˵�, ��Ҫ���ݲ˵����, ��������ťʱ�ᵯ���˵�, ����0�Ļ�����ᴥ������¼�
// isPopup = �Ƿ񵯳��˵�, ���Ϊ��, ������꾭����ʱ��ᵯ���˵�
typedef void(__cdecl* pfn_etInsertMenu)(int id, LPCWSTR lpszMenuText, int index, HMENU hPopupMenu, bool isPopup);
typedef void(__cdecl* pfn_etRemoveMenu)(int id, bool isIndex);
typedef void(__cdecl* pfn_SetLastError)(LPCWSTR fmt, ...);
typedef LPCWSTR(__stdcall* pfn_GetLastError)();
typedef void(__cdecl* PFN_log_dbg)(LPCSTR fmt, ...);
typedef const char* (__cdecl* PFN_GetDllNameFromAddress)(LPCVOID pAddress);

struct EWINDOW_MENUSTRUCT
{
    std::vector<EMENU_DATA> menu;       // �˵�����
    std::vector<EMENU_DATA> menu_etool; // ���ֲ˵�
    HMENU hMenu;                // �˵����, ����ṹ�����ݾ�������˵����Ӳ˵�
};

// �¼�����ĸ����ص�����, ʹ��һ���ṹ��¼����
typedef struct EVENT_CALLBACK_FUN
{
    pfn_iDraw_Event_GetMessageInfo  pfnGetMessageInfo;          // �¼������Ļ�ȡ��Ϣ�ص�����, ͨ�����ھ��-��Ϣֵ-GUID��ȡ, ���¼������ʼ����ʱ��ֵ
    pfn_iDraw_Event_GetEventInfo    pfnGetEventInfo;            // �¼������Ļ�ȡ�¼��ص�����, ͨ�����ھ��-��Ϣֵ-GUID��ȡ, ���¼������ʼ����ʱ��ֵ
    pfn_iDraw_Register_Message      pfnRegister_Message;        // �¼�������ע����Ϣ�ص�, ���¼������ʼ����ʱ��ֵ
    pfn_iDraw_Register_Event        pfnRegister_Event;          // �¼�������ע���¼��ص�����, ���¼������ʼ����ʱ��ֵ
    pfn_DetachRegister_Message      pfnDetachRegister_Message;  // ���ڱ�����, �ڲ�ʹ��, ɾ����������µ�������Ϣ�ص�
    pfn_iDraw_UnRegister            pfnUnRegister_Message;      // ȡ��ָ�������������Ϣ�ص�
    pfn_iDraw_UnRegister            pfnUnRegister_Event;        // ȡ��ָ������������¼��ص�
    pfn_event_before                pfn_Event_callback_before;  // �¼�����ǰ����, ��װ��һ���򵥵ĺ���
    pfn_event_after                 pfn_Event_callback_after;   // �¼���������, ��װ��һ���򵥵ĺ���
}*LPEVENT_CALLBACK_FUN;

typedef void (__cdecl* pfn_load_plugin)();


// ������ɫ��Ϣ, ��Щ��ɫ����ҪD2D����, ��Ҫ���Ի������ʼ��D2D�󴴽���Ӧ����
typedef struct THEME_D2D_INFO
{
    LPEX_BRUSH hbrBack;                 // ������ˢ

    LPEX_PEN hPenBorder;                // �߿򻭱�
    LPEX_PEN hPenBorder_run;            // ����ʱ�߿򻭱�
    LPEX_PEN hPenBorder_nf;             // ȡ������ʱ�ı߿򻭱�
    LPEX_PEN hPenShadow;                // ��Ӱ��ɫ
    LPEX_PEN hPenShadow_run;            // ����ʱ��Ӱ��ɫ
    LPEX_PEN hPenShadow_nf;             // ȡ������ʱ�ı߿���ɫ

    LPEX_BRUSH hbrBorder;               // �߿�ˢ
    LPEX_BRUSH hbrBorder_run;           // ����ʱ�߿�ˢ
    LPEX_BRUSH hbrBorder_nf;            // ȡ������ʱ�ı߿�ˢ
    LPEX_BRUSH hbrShadow;               // ��Ӱ��ˢ
    LPEX_BRUSH hbrShadow_run;           // ����ʱ��Ӱ��ˢ
    LPEX_BRUSH hbrShadow_nf;            // ȡ������ʱ�ı߿�ˢ


    LPEX_PEN   hPenMenuBorder;          // �˵��߿򻭱�
    LPEX_BRUSH hbrMenuBorder;           // �˵��߿�ˢ
    LPEX_BRUSH hbrMenuBack;             // �˵�������ˢ, �����˵�ʹ�õı�����ˢ, �˵�������ʱʹ�õı�����ˢ
    LPEX_BRUSH hbrMenuHot;              // �˵��ȵ��ˢ
    LPEX_BRUSH hbrMenuBan;              // �˵���ֹ��ˢ
    LPEX_BRUSH hbrMenuText;             // �˵��ı���ˢ
    LPEX_BRUSH hbrMenuTextHot;          // �˵��ı��ȵ㻭ˢ
    LPEX_BRUSH hbrMenuTextBan;          // �˵��ı���ֹ��ˢ

    LPEX_BRUSH hbrMenuDown;             // �˵�������ˢ, �����˵�ʹ�õı�����ˢ, �˵�������ʱʹ�õı�����ˢ
    LPEX_BRUSH hbrMenuBarHot;           // �����˵����ȵ��ˢ
    LPEX_BRUSH hbrMenuBarToolsText;     // �����˵������ֵ��ı���ˢ
    LPEX_BRUSH hbrMenuBarToolsTextHot;  // �����˵������ֵ��ı��ȵ㻭ˢ

    LPEX_BRUSH hbrTitleback;            // �м�ڿ����ı�����ˢ
    LPEX_BRUSH hbrTitle_left_act;       // �м�ڿ������ߵĻ�ˢ, �����Դ����������Ŀ��, ����ʱ���ı���ˢ
    LPEX_BRUSH hbrTitle_right_act;      // �м�ڿ�����ұߵĻ�ˢ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õĻ�ˢ, ����ʱ���ı���ˢ
    LPEX_BRUSH hbrTitle_left;           // �м�ڿ������ߵĻ�ˢ, �����Դ����������Ŀ��
    LPEX_BRUSH hbrTitle_right;          // �м�ڿ�����ұߵĻ�ˢ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õĻ�ˢ

    LPEX_BRUSH hbrText;                 // һ��滭�ı��Ļ�ˢ
    LPEX_BRUSH hbrTextStatusBar;        // �ײ�״̬�����ı���ˢ
    LPEX_BRUSH hbrTextStatusBar_run;    // �ײ�״̬������ʱ���ı���ˢ
    LPEX_BRUSH hbrTextStatusBar_nf;     // �ײ�״̬����������ı���ˢ


    LPEX_FONT  hFont;                   // �м����ʹ�õ�����, -13�Ӵ�
    LPEX_FONT  hFont1;                  // �м����ʹ�õ�����, -13���Ӵ�

    LPCANVAS hCanvas;                   // �˵�ר��d2d�滭���

}*LPTHEME_D2D_INFO;


struct IDRAW_INFO
{
    HWND hEWnd;                 // �����������ھ��
    HWND hShadow;               // ��������Ӱ����

    HWND hCodeTab;              // �����������ѡ��о��
    HWND hState;                // ״̬�о��, ����༭��/������������
    HWND hStatusBar;            // �ײ�״̬��
    HWND hMDIClient;            // �����Ĵ��ھ��, �������������д򿪵ı�ǩ, ����ĸ����ڵ�������������, �Ի�ѡ�����������ڵ�����
    HWND hControl;              // �����
    HWND hWork;                 // ������, ���֧�ֿ�/����
    HWND hWndMenu;              // �˵�������, ������Ŷ��빤����,��׼������
    HWND hMenuMenu;             // ��Ų˵��еĴ��ھ��
    HWND hMenu_Standard;        // �´����Ĺ�����, ����ͱ�׼��������ʹ�����
    HWND hWndTabControl;        // ��������ѡ���, �Լ�������, ��Ҫ��Ӧ�Ӽ��л��¼�����ʾ��ͬ�Ĵ���
    HWND hWndTool;              // ��׼����������Ĺ�����, �������Ϊһ���ӿ����û����Ӱ�ť������
    HWND hTabStateLeft;         // �Լ��´�����ѡ���, ��״̬������������鴰��, ��״̬�д���ѡ�����ͬ������, ��ߵĴ���
    HWND hTabStateRight;        // �Լ��´�����ѡ���, ��״̬������������鴰��, ��״̬�д���ѡ�����ͬ������, �ұߵĴ���

    EWINDOW_MENUSTRUCT menu;    // �˵�����
    EWINDOW_MENUSTRUCT menuDoc; // �ĵ��˵�����

    RECT rcEWindow;             // �����Դ��ڿͻ���λ��, �󶥲�����Ļ����
    RECT rcCustomTab;           // �Ի��ѡ���, ���ѡ������л�Դ����ʾ��ѡ���
    RECT rcCodeTab;             // �����������ѡ��о��
    RECT rcState;               // ״̬�о��, ����༭��/������������
    RECT rcStatusBar;           // �ײ�״̬��
    RECT rcMDIClient;           // �����Ĵ��ھ��, �������������д򿪵ı�ǩ, ����ĸ����ڵ�������������, �Ի�ѡ�����������ڵ�����
    RECT rcControl;             // �����
    RECT rcWork;                // ������, ���֧�ֿ�/����
    RECT rcWndMenu;             // �˵�������, ������Ŷ��빤����,��׼������
    RECT rcStandard;            // �˵���������ı�׼������λ��
    RECT rcMenu;                // �˵���λ��
    RECT rcControl1;            // ������λ��, �Զ�����ʱ��λ��
    RECT rcState1;              // ״̬��λ��, �Զ�����ʱ��λ��
    RECT rcWork1;               // ������λ��, �Զ�����ʱ��λ��
    int cxClient;               // �����Դ��ڿͻ������
    int cyClient;               // �����Դ��ڿͻ����߶�

    bool isActivation;          // �Ƿ񼤻��, ���ֵ���ı�ʱ�ᴥ�����ڱ���λ���ػ�, ���д�����Ӱ�����ػ�
    bool isZoomed;              // ��ǰ�����Ƿ��Ѿ����, �ڴ��ڳߴ�ı����޸�

    HWND hMenuLocation;         // ��λ������, �����໯ǰ���ø�����Ϊ������������, λ���ڴ���������, �Ի�ѡ��е�����
    HWND hComboBox1;            // ��λ��������ߵ���Ͽ�
    HWND hComboBox2;            // ��λ�������ұߵ���Ͽ�

    ICustomCodeTab* hWnds;      // �Ի�ѡ��еĴ�������, �±��Ӧѡ��е��±�, ѡ���ѡ��ʱ��������Ĵ���

    BOOL isRun;                 // �����Ƿ�������״̬, ���ֵ���ı�ʱ�ᴥ�����ڱ���λ���ػ�, ���д�����Ӱ�����ػ�
    BOOL isStop;                // ֹͣ�Ƿ�������״̬

    IGetEWindow*    eWnd;       // �����Դ���
    pfnEIDE_Notify  pfnIDE;     // �����Խ����ĺ���
    pfn_Scale       scale;      // ���ź���, ��Ҫ�������������������ֳߴ�
    UINT            dpiX;       // ��ǰ��Ļ��dpi����Xֵ
    UINT            dpiY;       // ��ǰ��Ļ��dpi����Yֵ
    bool            isReady;    // �Ƿ����, ���Ը�����Ҫ�����Ƿ������Ÿı�ߴ�


    UINT    MY_MF_OWNERDRAW;    // ֵΪ MF_OWNERDRAW ����ʹ���ڲ����Ի�, ����ʹ��win10����ɫ����

    LPCWSTR path_e;             // ������e.exe���ڵ�Ŀ¼, �����"\"
    LPCWSTR path_run;           // ������֧�ֿ��Ŀ¼, �����"\"    lib/
    LPCWSTR path_lib;           // ������֧�ֿ��Ŀ¼, �����"\"    lib/
    LPCWSTR path_file;          // ������֧�ֿ��Ŀ¼, �����"\"    lib/
    LPCWSTR path_data;          // ���ݱ���Ŀ¼, ����� "\", �����֧�ֿ�Ŀ¼�µ� lib/iDraw/data/ Ŀ¼
    LPCWSTR path_iDraw;         // ���Ĳ��Ŀ¼, ����� "\", �����֧�ֿ�Ŀ¼�µ� lib/iDraw/ Ŀ¼
    LPCWSTR path_plugin;        // ���Ŀ¼, ����� "\", �����֧�ֿ�Ŀ¼�µ� lib/iDraw/plugin/ Ŀ¼
    LPCWSTR path_ETplugin;      // ���ֲ��Ŀ¼, ����� "\", �����֧�ֿ�Ŀ¼�µ� lib/iDraw/eTools/plugin/ Ŀ¼
    LPCWSTR path_config;        // �����ļ�����·��, lib/iDraw/data/config.ini

    LPOBJSTRUCT defData;        // Ĭ�ϵ�����, Ĭ����ɫ
    IiDraw* iDraw;              // �Ի���ؽӿ�, �����Ի��йصĶ�Ӧ��ͨ���˽ӿڵ���
    ID2DDraw* d2d;              // D2D�滭����ӿ�, �ɻ滭����Ĳ������, �õ�D2D�Ķ�ʹ�����������в���
    CLayered* pTips;            // ��ʾ���
    wstr* pszTitle;             // ���ڱ���, ֻ��, �����������������໯ʱ��ֵ
    ICShadow* shadow;           // ��Ӱ������
    IEWindowResource* pResource;// ��Դ��ؽӿ�, ���ж������ģ���ﷵ��
    pfn_GetDataFromHwnd pfnGetDataFromHwnd; // ��ȡ���ھ����Ӧ��Ϣ�ĺ���, û���򷵻�0
    pfn_ALLWINDOW_MESSAGE pfnAllWindowProc; // �ƶ�ģ����Ҫ������Ϣ�ĺ���, ������Ҫ�������Ϣʱ�͵����������
    EVENT_CALLBACK_FUN evt;     // �¼�����ĺ���, ���ֵ�����¼������ʼ��ʱ��ʼ��

    LPPLUGIN_INFO pInfoMoveWindow;
    LPPLUGIN_INFO pInfoDrawControl;
    LPPLUGIN_INFO pInfoResource;
    LPPLUGIN_INFO pInfoEvent;
    LPPLUGIN_INFO pInfoConfig;
    LPPLUGIN_INFO pInfoETools;
    LPPLUGIN_INFO pInfoTheme;

    //THEME_COLOR_INFO    window_color_info;  // ���ڵ���ɫ��Ϣ, �ڳ�ʼ����������ʱ���ʼ������ṹ
    THEME_D2D_INFO      window_d2d_info;    // ���ڵ���ɫ��ˢ����, �ڳ�ʼ���Ի�����ʱ���ʼ������ṹ
    bool isDxRender;            // �Ƿ�Ϊdx��Ⱦ, �ڳ�ʼ���滭���ʱ��ֵ
    bool isGpRender;            // �Ƿ�ΪGdiplus��Ⱦ, �ڳ�ʼ���滭���ʱ��ֵ
    int nRenderMode;            // �и�ֵ��ʱδʹ��, ��Ⱦģʽ, �ڳ�ʼ���滭���ʱ��ֵ, δ�����ܻ�����dx9-dx11, openGL��ģʽ

    HMENU hMenuPopup;           // ��ǰ�����Ĳ˵����, �� TrackPopupMenu ���� TrackPopupMenu ����ǰ��ֵ, ���ú���0, �������������API�����Ĳ˵��򲻼�¼

    pfn_load_plugin pfnLoadPlugin;
    pfn_load_plugin pfnFreePlugin;

    LPOBJSTRUCT     pWnd;               // ����������������
    TAB_INFO        tab;                // �����л�ѡ�������
    pfn_CopyStrW    copyStrW;           // ͳһһ��λ�������ڴ�
    pfn_CopyStrA    copyStrA;           // ͳһһ��λ�������ڴ�
    pfn_FreeStr     freeStr;            // ͳһһ��λ���ͷ��ڴ�

    CPrivateProfile* ini;               // ������
    CPrivateProfile* iniColor;          // ��ɫ����
    IConfig* config;                    // ������Ϣ, ����ֵʱ��д��xml��, ��������ò����ʼ��, �ڳ�ʼ����ʱ���Ѿ�����Ĭ��ֵ
    IThemeColor* themeColor;            // �������ɫ, ����ֵʱ��д��xml��, ��������ò����ʼ��, �ڳ�ʼ����ʱ���Ѿ�����Ĭ��ֵ



    pfn_CreateBrush     pfnCreateBrush;     // �ڲ�ʹ�õĴ�����ˢ����, ���صĻ�ˢ�����ͷ�, ��iTheme�����ʼ��ʱ��ʼ��
    pfn_CreatePen       pfnCreatePen;       // �ڲ�ʹ�õĴ������ʺ���, ���صĻ��ʲ����ͷ�, ��iTheme�����ʼ��ʱ��ʼ��
    pfn_CreateBrushDx   pfnCreateBrushDx;   // �ڲ�ʹ�õĴ���Dx��ˢ����, ���صĻ�ˢ�����ͷ�, ��iTheme�����ʼ��ʱ��ʼ��
    pfn_CreatePenDx     pfnCreatePenDx;     // �ڲ�ʹ�õĴ���Dx���ʺ���, ���صĻ��ʲ����ͷ�, ��iTheme�����ʼ��ʱ��ʼ��
    pfn_CreateFontDx    pfnCreateFontDx;    // �ڲ�ʹ�õĴ���Dx���庯��, ���ص����岻���ͷ�, ��iTheme�����ʼ��ʱ��ʼ��
    pfn_CreateFontLfDx  pfnCreateFontLfDx;  // �ڲ�ʹ�õĴ���Dx���庯��, ���ص����岻���ͷ�, ��iTheme�����ʼ��ʱ��ʼ��
    pfn_CreateFontDpi   pfnCreateFontDpi;   // �ڲ�ʹ�õĸ���dpi����Dx���庯��, ���ص����岻���ͷ�, ��iTheme�����ʼ��ʱ��ʼ��
    pfn_GetDefData      pfnGetDefData;      // ��ȡĬ�ϻ滭��Ϣ, ���Ĭ����Ϣд�봫�ݽ����Ĳ�����, ��iTheme�����ʼ��ʱ��ʼ��
    pfn_ReloadDxResource pfnReloadDxResource;// ���´������л��ʻ�ˢ����

    pfn_GetInfo pfnGetInfo;             // ��ȡ������Ϣ
    pfn_SetInfo pfnSetInfo;             // ����������Ϣ, ��������Ļ��в��������ǻ�ֱ��д��xml���߼�ʱ��Ч��
    pfn_InfoChanged pfnInfoChanged;     // ���ñ��ı�ص�
    pfn_GetAllSubclassWindow pfnGetAllSubclassWindow;   // ��ȡ�����Ѿ����໯�Ĵ�������, ����д�뻺������Ա��, ����Ϊ0�򷵻ػ�������������

    pfn_call_menu pfnCallMenu;          // ����������ָ���˵�����, ���ݲ˵�ID

    pfn_Plugin_init     pfnPlugin_init;         // ��ʼ�����, �Ѳ��ע�ᵽ�������
    pfn_Plugin_GetInfo  pfnPlugin_GetInfo;      // ��ָ��guid��ȡ�����Ϣ
    pfn_Plugin_Query    pfnPlugin_Query;        // ��ָ��ֵ��ѯ�Ƿ�Ϊ�����guid
    pfn_Plugin_Enum     pfnPlugin_Enum;         // ö�ٵ�ǰ�����Ѿ���ʼ���Ĳ��, ��Ҫ�ⲿ���ݻ�����
    pfn_Plugin_UnInit   pfnPlugin_UnInit;       // ��ָ��guid�Ĳ�����б���ɾ��


    pfn_IDraw_Callback  pfnConfig;              // ���ò���Ļص�����, Ҫ���û��߻�ȡ���ݾ͵����������
    pfn_IDraw_Callback  pfnTheme;               // �������Ļص�����, Ҫ���û��߻�ȡ���ݾ͵����������
    pfn_IDraw_Callback  pfnEvent;               // �¼�����Ļص�����, Ҫ���û��߻�ȡ���ݾ͵����������
    pfn_IDraw_Callback  pfnControls;            // �滭����Ļص�����, Ҫ���û��߻�ȡ���ݾ͵����������, ����Ƚ���Ҫ, �˵�ʲô�Ķ�����������ʵ��
    pfn_IDraw_Callback  pfnResource;            // ��Դ����Ļص�����, Ҫ���û��߻�ȡ���ݾ͵����������
    pfn_IDraw_Callback  pfnMove;                // �ƶ�����Ļص�����, Ҫ���û��߻�ȡ���ݾ͵����������
    pfn_IDraw_Callback  pfnETools;              // ���ֻص�����, ��Ҫ�����������ֵ, ���Ե����������ȥ��ȡ�����������ֵ�һЩ����

    HMENU hMenuExtPopup;                        // ��չ����ʽ�˵�, ���в���Ĳ˵����Ǽ��뵽����˵���

    UINT uMsgEWindowThreadCallback;             // �����Դ����̵߳��ú�������Ϣֵ, wParam�Ǻ�����ַ, lParam�Ǻ�������

    pfn_SetLastError SetLastError;              // ����������
    pfn_GetLastError GetLastError;              // ��ȡ������
    PFN_log_dbg      logMsgEvt;                 // ���ע���¼�, ע����Ϣ, ������Ϣ���¼����ݵ��ļ�
    PFN_log_dbg      logPluginInit;             // �����ʼ����Ϣ
    PFN_GetDllNameFromAddress   pfn_GetDllNameFromAddress;  // ��ȡָ����ַ��dll����

    int             menuSelIndex;               // ��ǰ�����Ĳ˵���꾭������Ŀ����
    int             menuSelId;                  // ��ǰ�����Ĳ˵���꾭������ĿID
    HMENU           menuSelHmenu;               // ��ǰ�����Ĳ˵���꾭������Ŀ���ڵĲ˵����

    int             EVersion;                   // �����԰汾��593, 591, 595, 5922 ���ָ�ʽ
};
typedef IDRAW_INFO* LPIDRAW_INFO;
extern LPIDRAW_INFO s_info;     // �����������ڵ���Ϣ, ֧�ֿ��������̬�ⶼ��ʹ��, ֧�ֿ���ö�̬���ʱ��ᴫ�����ָ���ȥ

inline BOOL WINAPI ShowWindowEx(HWND hWnd, int nCmdShow)
{
    if (hWnd)
    {
        LPBYTE pShowWindow = (LPBYTE)ShowWindow;
        LPBYTE pJmp = 0;
        if (*pShowWindow == 0xe9)
        {
            DWORD offset = *(DWORD*)(pShowWindow + 1);
            pJmp = pShowWindow + 5 + offset;
        }
        else if (*pShowWindow == 0xff && pShowWindow[1] == 0x25)
        {
            DWORD offset = *(DWORD*)(pShowWindow + 2);
            pJmp = (LPBYTE)offset;
        }

        if (pJmp)
        {
            MEMORY_BASIC_INFORMATION mbi;
            if (VirtualQuery(pJmp, &mbi, sizeof(mbi)) != sizeof(mbi))
            {
                return FALSE;
            }
        }
        return ShowWindow(hWnd, MAKELONG(__HOOK_MOVE_BREPAINT, nCmdShow));
    }
    return FALSE;
}

inline BOOL WINAPI MoveWindowEx(HWND hWnd, const RECT& rc, bool bRepaint)
{
    //return SetWindowPos(hWnd, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_DRAWFRAME);
    return MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, MAKELONG(__HOOK_MOVE_BREPAINT, bRepaint));
}

//// ��ȡָ�����ֵ�node�ڵ�, ���û������ڵ�, �ᴴ��һ���µĽڵ�
//inline ICTinyXml2_XMLNode XML_GetNode(CTinyXml2W* lpXml, LPCWSTR lpszNodeName)
//{
//    wchar_t name[100];
//    swprintf_s(name, 100, L"@1/%s", lpszNodeName);
//    ICTinyXml2_XMLNode node = 0;
//    if (!lpXml->GetNode(name, node))
//        lpXml->InsertChildNode(L"@1", lpszNodeName, 0, false, &node);
//    return node;
//}
//
//// ��ȡxmlָ���ڵ������ֵ
//inline wstr XML_Read(CPrivateProfile* lpXml, ICTinyXml2_XMLNode node, LPCWSTR lpszAttrName, LPCWSTR lpszDefValue = 0)
//{
//    if (!node)return wstr();
//    LPCWSTR value =  lpXml->GetAttrValue(node, lpszAttrName);
//    if (!value || !value[0])value = lpszDefValue;
//    return value;
//}
//
//// ����xmlָ���ڵ������ֵ
//inline bool XML_Write(CPrivateProfile* lpXml, ICTinyXml2_XMLNode node, LPCWSTR lpszAttrName, LPCWSTR lpszValue)
//{
//    if (!node)return false;
//    return lpXml->SetAttrValue(node, lpszAttrName, lpszValue);
//}
//


inline wstr INI_Read(CPrivateProfile* pIni, LPCWSTR appName, LPCWSTR keyName, LPCWSTR defValue)
{
    return pIni->read(appName, keyName, defValue);
}
inline bool INI_Write(CPrivateProfile* pIni, LPCWSTR appName, LPCWSTR keyName, LPCWSTR value)
{
    return pIni->write(appName, keyName, value);
}
inline bool Rect_isOverlap(const RECT& rc1, const RECT& rc2)
{
    return (rc1.right > rc2.left && rc2.right > rc1.left
        && rc1.bottom > rc2.top && rc2.bottom > rc1.top);
}
// ����ʽ���ж��Ƿ��б߿�
inline bool IsBorder(DWORD style, DWORD styleEx)
{
    if (style & WS_BORDER)return true;
    if ((styleEx & WS_EX_CLIENTEDGE)
        || (styleEx & WS_EX_DLGMODALFRAME)
        || (styleEx & WS_EX_WINDOWEDGE)
        )
        return true;
    return false;
}
inline bool IsBorder(HWND hWnd)
{
    DWORD style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_BORDER)return true;
    DWORD styleEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((styleEx & WS_EX_CLIENTEDGE)
        || (styleEx & WS_EX_DLGMODALFRAME)
        || (styleEx & WS_EX_WINDOWEDGE)
        )
        return true;
    return false;
}


template<typename T, typename R>bool __query(T s, R n)
{
    return (s & n) != 0;
}
inline DWORD InsertStyle(HWND hWnd, DWORD style)
{
    DWORD dwStyle = GetWindowLongPtrA(hWnd, GWL_STYLE);
    SetWindowLongPtrA(hWnd, GWL_STYLE, dwStyle | style);
    return dwStyle;
}

inline DWORD RemoveStyle(HWND hWnd, DWORD style)
{
    DWORD dwStyle = GetWindowLongPtrA(hWnd, GWL_STYLE);
    SetWindowLongPtrA(hWnd, GWL_STYLE, dwStyle & ~style);
    return dwStyle;
}

// ����������IDE�ӿڹ���
inline int pfn_NES_RUN_FUNC(int message, DWORD wParam = 0, DWORD lParam = 0)
{
    // ֪ͨ�ױ༭��������ָ���Ĺ���,����һ��BOOLֵ��
    // dwParam1Ϊ���ܺš�
    // dwParam2Ϊһ��˫DWORD����ָ��,�ֱ��ṩ���ܲ���1��2��
    const int n_NES_RUN_FUNC = 2;
    DWORD arg[2] = { wParam, lParam };
    return s_info->pfnIDE(n_NES_RUN_FUNC, message, (DWORD)arg);
}

inline LRESULT CALLBACK WndProc_CallWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    pfn_CallWindowProc proc = pData->fnCallProc;
    if (!proc)proc = CallWindowProcW;
    return proc(pData->oldProc, hWnd, message, wParam, lParam);
}

inline int _tab_getdirection(DWORD style)
{
    const bool isBottom = (style & TCS_BOTTOM) == TCS_BOTTOM;   // ���ڵױ�, ����ұ�û����   
    int direction = (style & TCS_VERTICAL) == TCS_VERTICAL;     // ѡ�����, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    if (direction)  direction = isBottom ? 2 : 0;
    else            direction = isBottom ? 3 : 1;
    return direction;
}

// ����ѡ��еķ���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
// hWnd = ѡ��еĴ��ھ��
// rcTanControl = ѡ��пؼ��ľ���λ��
// rcCalc = ���ռ������ѡ��пͻ���λ��
inline int _tab_calc_client(HWND hWnd, const RECT& rcTabControl, RECT& rcCalc)
{
    RECT rcItem = { 0 };
    TabCtrl_GetItemRect(hWnd, 0, &rcItem);

    const int cxClient = rcTabControl.right - rcTabControl.left;
    const int cyClient = rcTabControl.bottom - rcTabControl.top;

    const DWORD dwStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);
    int direction = _tab_getdirection(dwStyle);

    switch (direction)
    {
    case 0:     // ���, ����=1, �ұߵױ߸���1
    {
        rcCalc.left = rcItem.right;
        rcCalc.top = 0;
        rcCalc.right = cxClient - 1;
        rcCalc.bottom = cyClient - 1;
        break;
    }
    case 1:     // ����, ���=1, �ұߵױ߸���1
    {
        rcCalc.left = 1;
        rcCalc.top = rcItem.bottom - 1;
        rcCalc.right = cxClient - 1;
        rcCalc.bottom = cyClient - 1;
        break;
    }
    case 2:     // �ұ�, ��߶���=1, �ױ߼�1
    {
        rcCalc.left = 1;
        rcCalc.top = 0;
        rcCalc.right = rcItem.left;
        rcCalc.bottom = cyClient - 1;
        break;
    }
    default:    // �ױ�, ��߶���=1, �ұ߼�1
    {
        rcCalc.left = 1;
        rcCalc.top = 0;
        rcCalc.right = cxClient - 1;
        rcCalc.bottom = rcItem.top;
        break;
    }
    }
    return direction;
}

inline void _tab_modify_icon(bool isIcon, LPOBJSTRUCT pData)
{
    if (!isIcon)
    {
        HIMAGELIST hList = TabCtrl_GetImageList(pData->hWnd);
        SetPropW(pData->hWnd, L"ImageList", hList);
        TabCtrl_SetImageList(pData->hWnd, 0);
        return;
    }

    HIMAGELIST hList = (HIMAGELIST)GetPropW(pData->hWnd, L"ImageList");
    if (hList)
        TabCtrl_SetImageList(pData->hWnd, hList);
}


// �޸�ѡ��з���, �����µĴ�����ʽ, ����ô�����ʽ������, ѡ������໯���������ֵ��¼��xml��, �����浽�ļ�
// direction = ����, 0=���, 1=����, 2=�ұ�, 3=�ױ�
// pData = ѡ��е�����, ���ݽ������뱣ֵ֤����ȷ��, �ڲ��������
// style = ѡ��е�ǰ������ʽ, ���Ϊ0, �ڲ����ȡ
// insertStyle = Ҫ�������ʽ, Ϊ0�򲻼����κ���ʽ
inline DWORD _tab_modify_direction(int direction, LPOBJSTRUCT pData, DWORD style = 0, DWORD insertStyle = 0)
{
    // 0=���, 1=����, 2=�ұ�, 3=�ױ�
    if (style == 0) style = GetWindowLongPtrW(pData->hWnd, GWL_STYLE);
    switch (direction)
    {
    case 0:     // ���
    {
        style &= ~(TCS_RIGHT);
        style |= TCS_VERTICAL;
        break;
    }
    case 1:     // ����
    {
        style &= ~(TCS_RIGHT | TCS_VERTICAL);
        break;
    }
    case 2:     // �ұ�
    {
        style |= (TCS_RIGHT | TCS_VERTICAL);
        break;
    }
    default:    // �ױ�
    {
        style &= ~(TCS_VERTICAL);
        style |= TCS_BOTTOM;
        direction = 3;
        break;
    }
    }
    style &= ~(TCS_MULTILINE);
    style |= insertStyle;
    SetWindowLongPtrW(pData->hWnd, GWL_STYLE, style);
    return style;
}

// �������������ڷ��ͳߴ�ı��¼�
inline void SendEWindowSize(LPOBJSTRUCT pData = 0)
{
    if (!pData)
        pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
    if (!pData)
        return;
    WPARAM wParam = s_info->isZoomed ? SIZE_MAXIMIZED : SIZE_RESTORED;
    LPARAM lParam = MAKELONG(pData->pos.width, pData->pos.height);
    SendMessageW(s_info->hEWnd, WM_SIZE, wParam, lParam);
}


// ��ȡ�ұ�ѡ�����Ŀ��ֵ����
inline std::vector<int>* prop_GetTabParams(bool isAlloc = false)
{
    std::vector<int>* params = (std::vector<int>*)GetPropW(s_info->hEWnd, GUID_TABPARAMS);
    if (!params && isAlloc)
    {
        params = new std::vector<int>;
        params->reserve(20);
        SetPropW(s_info->hEWnd, GUID_TABPARAMS, params);
    }
    return params;
}

// �ж�ָ����Ŀ��ֵ�Ƿ����ұߵ�ѡ���
inline bool TabCode_IsRightTab(int param)
{
    std::vector<int>* params = prop_GetTabParams();
    if (params)
    {
        for (auto it = params->begin(); it != params->end(); ++it)
        {
            if (param == *it)
                return true;
        }
    }
    return false;
}
// ��ȡ�ұ��Ӽ�����, û�оͷ���0
inline int TabCode_GetRightItemCount()
{
    std::vector<int>* params = prop_GetTabParams();
    if (!params)return 0;
    return (int)params->size();
}

// ��ȡ���й������ص���¼
inline SCROLL_CALLBACK* prop_GetScrollCallback(bool isAlloc = false)
{
    SCROLL_CALLBACK* ptr = (SCROLL_CALLBACK*)GetPropW(s_info->hEWnd, GUID_SCROLL_CALLBACK_EWINDOW);
    if (!ptr && isAlloc)
    {
        ptr = new SCROLL_CALLBACK;
        SetPropW(s_info->hEWnd, GUID_SCROLL_CALLBACK_EWINDOW, ptr);
    }
    return ptr;
}

// ��ȡ��¼inlineHook�ļ�¼
inline INLINEHOOK_CALLBACK* prop_GetInlineHookCallback(bool isAlloc = false)
{
    INLINEHOOK_CALLBACK* ptr = (INLINEHOOK_CALLBACK*)GetPropW(s_info->hEWnd, GUID_INLINE_HOOK);
    if (!ptr && isAlloc)
    {
        ptr = new INLINEHOOK_CALLBACK;
        SetPropW(s_info->hEWnd, GUID_INLINE_HOOK, ptr);
    }
    return ptr;
}