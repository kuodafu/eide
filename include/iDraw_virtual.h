#pragma once
#include <control/WndBaseTypedef.h>
#include <control/WndBaseObjStruct.h>
#include <assist/CPrivateProfile.h>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "EIDE_Virtual.h"
#include <control/ReadColor.h>

//#include <windows.h>
//#include <d2d/_canvas_fundef.h>
//#include <assist_draw.h>
//#include <control/WndBaseState.h>
//#include <control/WndBaseObjStruct.h>

// ���������ʼ�ͽ����滭ʹ�õĽṹ
struct BEGIN_END_PAINT_STRUCT : PAINTSTRUCT
{
    RECT rc;
    int cxClient;
    int cyClient;
    LPEX_FONT pFont;
    HFONT hFont;
    LPMEMDCBITMAP md;
    BOOL isDxRender;
    LPOBJSTRUCT pData;
};


// ����״̬��Ϣ
typedef struct TOPSTATE_INFO
{
    RECT rc[5];         // 0=�رհ�ť, 1=ͼ����ť, 2=���ǰ�ť, 3=����л���ť, 4=���/��ԭ��ť ��ŵĶ��ǿͻ�������
    int index;          // ��ǰ�滭�����ĸ���ť
    bool isDown;        // �Ƿ��ǰ���״̬, ����Ѿ�����, �Ჶ�����, ����������뿪��ʱ�����ж����ֵ, Ϊfalseʱ���ػ�
    int state;          // ��ť״̬, ״̬��ʹ�� STATE:: ���״̬
    int count;          // ��ť����
    int flags;          // ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ
    bool isSelect;      // ͼ����ť�Ƿ�ѡ��, ѡ�еĻ�ʹ�� �����ͼ��, δѡ��ʹ������ͼ��, ûѡ�о���ͣ��, ѡ�о��Ǹ���, ѡ�л��Զ�����
    bool isRestore;     // �Ƿ���ʾ��ԭͼ��, ���Ϊtrue, �Ǿͻ滭��ԭͼ��, ����滭���ͼ��
    bool isDownTitle;   // �Ƿ����ڱ����λ��, �����λ�õ��һ�����Ҫ�ƶ�������
    wchar_t name[20];   // ���֧��20���ַ�
    RECT rcOffset;      // ƫ����
    RECT rcWindow;      // ����λ��, ��������ʱ��¼, �滭���ߵ�״̬����ʱ������������
    HWND hWnd;          // ��������¼��Ĵ��ھ��
}*LPTOPSTATE_INFO;

// ��ѯ����״̬���Ƿ����ָ��ֵ
// info = LPTOPSTATE_INFO �ṹָ��
// state = ��ѯ��ֵ, TOP_STATE_BUTTON ö�ٳ�����ֵ
#define __query_flags(_info, _state)    (((_info->flags) & (_state)) == (_state))

// ���ָ���ı�ʶ���ṹ��
// info = LPTOPSTATE_INFO �ṹָ��
// state = ��ѯ��ֵ, TOP_STATE_BUTTON ö�ٳ�����ֵ
#define __add_flags(_info, _state)      ((_info->flags) |= (_state))

// �ӽṹ��ɾ��ָ����ʶ
// info = LPTOPSTATE_INFO �ṹָ��
// state = ��ѯ��ֵ, TOP_STATE_BUTTON ö�ٳ�����ֵ
#define __del_flags(_info, _state)      ((_info->flags) &= ~(_state))

enum TOP_STATE_BUTTON : int
{
    TOP_STATE_BUTTON_UNKNOWN            = 0x00000000,   // û�а�ť
    TOP_STATE_BUTTON_CLOSE              = 0x00000001,   // �Ƿ���ڹرհ�ť
    TOP_STATE_BUTTON_PUSHPIN            = 0x00000002,   // �Ƿ����ͼ����ť, 
    TOP_STATE_BUTTON_PUSHPIN_SEL        = 0x00000004,   // �Ƿ����ͼ��ѡ�а�ť, ѡ�о��Ǻ���, ���Զ�����
    TOP_STATE_BUTTON_TRIANGLE           = 0x00000008,   // �Ƿ�������ǰ�ť
    TOP_STATE_BUTTON_SWITCH             = 0x00000010,   // �Ƿ����������л���ť
    TOP_STATE_BUTTON_MAXBOX             = 0x00000020,   // �Ƿ������󻯰�ť
    TOP_STATE_BUTTON_RESTORE            = 0x00000040,   // �Ƿ���ڻ�ԭ��ť

    TOP_STATE_BUTTON_ISFLOAT            = 0x00001000,   // �Ƿ񸡶�, �����ֵ��ʾ������󻯰�ť, Ҫô�����, Ҫô�ǻ�ԭ, �����˵���Ҫ��ֹ
    TOP_STATE_BUTTON_ISSTOP             = 0x00002000,   // �Ƿ�ͣ��, �����ֵ��ʾû����󻯰�ť, ͣ���˵���Ҫ��ֹ
    TOP_STATE_BUTTON_ISAOTUHIDE         = 0x00004000,   // �Ƿ��Զ�����, �����ֵ��ʾͼ����ť�����ŵ�, �����Զ�����, �Զ����ز˵���Ҫ��ֹ

    TOP_STATE_RESERVE_LEFT              = 0x00010000,   // ����Ԥ��λ��, ��߻�Ԥ��һ���ֳ�����������ߴ�
    TOP_STATE_RESERVE_TOP               = 0x00020000,   // ����Ԥ��λ��, ���߻�Ԥ��һ���ֳ�����������ߴ�
    TOP_STATE_RESERVE_RIGHT             = 0x00040000,   // ����Ԥ��λ��, �ұ߻�Ԥ��һ���ֳ�����������ߴ�
    TOP_STATE_RESERVE_BOTTOM            = 0x00080000,   // ����Ԥ��λ��, �ױ߻�Ԥ��һ���ֳ�����������ߴ�

    TOP_STATE_DIRECTION_LEFT            = 0x00100000,   // ����λ��, ��ǰ���������
    TOP_STATE_DIRECTION_TOP             = 0x00200000,   // ����λ��, ��ǰ�����ڶ���
    TOP_STATE_DIRECTION_RIGHT           = 0x00400000,   // ����λ��, ��ǰ�������ұ�
    TOP_STATE_DIRECTION_BOTTOM          = 0x00800000,   // ����λ��, ��ǰ�����ڵױ�

    TOP_STATE_BUTTON_DEF                = (TOP_STATE_BUTTON_CLOSE | TOP_STATE_BUTTON_PUSHPIN | TOP_STATE_BUTTON_TRIANGLE),  // Ĭ�ϰ�ť
    TOP_STATE_BUTTON_DEF_MAXBOX         = (TOP_STATE_BUTTON_DEF | TOP_STATE_BUTTON_MAXBOX),     // ��������ʱ��Ĭ�ϰ�ť

    TOP_STATE_BUTTON_CONTROL            = (TOP_STATE_BUTTON_DEF | TOP_STATE_BUTTON_SWITCH),     // �������л��ײ�...
    TOP_STATE_BUTTON_CONTROL_MAXBOX     = (TOP_STATE_BUTTON_CONTROL | TOP_STATE_BUTTON_MAXBOX), // ����両������ʱ���л��ײ�...

    TOP_STATE_ISSHOW                    = 0x01000000,   // �����Ƿ���ʾ
    TOP_STATE_ISCALCLAYOUT              = 0x02000000,   // ��������ʱ�Ƿ����λ��, û�������ʶ��ʱ��������ھ��ǹ̶�λ��
    TOP_STATE_ISTYPE_WIDTH              = 0x04000000,   // ��¼�ĳߴ��Ƿ�Ϊ���, ���û�������ʶ�Ǿ��Ǹ߶�
    TOP_STATE_ISGROUP                   = 0x08000000,   // ��ǰ�����Ƿ�Ϊ����, �������ʶ����Ҫ�滭ѡ���
};

struct MDICLIENT_ALL_WINDOW
{
    HWND hChild1;       // ����㴰��, MDIClient�µĵ�һ���Ӵ���
    HWND hChild2;       // �ڶ����Ӵ���, �������3��������
    HWND hChild3;       // �������Ӵ���, �����ĸ�����, ������������
    HWND hCode;         // ���봰��
    HWND hvScroll;      // �������������, id = 59904
    HWND hhScroll;      // �������������, id = 59920
};


// ͼ��ṹ, ���dx/gdi+ͼƬָ��, ͼ���λ��, ID, ˵��, ��ע, �ı���ɫ, ������ɫ....
// һ��IDָ������ݶ�����������ṹ��
typedef struct WINDOW_ICON_DATA
{
    Gdiplus::Bitmap* hBitmapGdip;   // gdi+ λͼ
    LPEX_IMAGE hBitmapDx;           // dx λͼ
    RECT rc[20];                    // ��ͼƬ�ĸ�λ�û滭, 4��״̬, 0=֧��,1=�ȵ�,2=����,3=��ֹ, ��¼5��DPI����λ��
    LPCWSTR id;                     // ����ṹ��id, ires��ÿ��ͼ�궼������ṹ, ��Ҫͨ�����id�ҵ�����ṹ
    LPCWSTR text;                   // �滭��ʾ���ı�, ���Ϊ0��ʹ��ԭ�����ı�
    LPCWSTR remarks;                // ��ע�ı�, Ŀǰ��ʱδʹ��
    DWORD crText[4];                // ARGB��ɫ, ��Ŀ�ı���ɫ
    DWORD crBack[4];                // ARGB��ɫ, ��Ŀ������ɫ
    DWORD by[5];                    // ����
    float cxIcon;                   // ͼ����
    float cyIcon;                   // ͼ��߶�
    int nIconType;                  // ͼ����ĸ���Դ���ȡ��, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    int nDpi;                       // ��ȡ����ͼ���DPI, �����ȡ150%���ŵ�ʧ����, ��ȡ��ȡ100%��ͼ��, ���100%��Ҳ��ȡʧ��, �Ǿͷ���0
    //TODO ��ȡͼ��Ĳ���Ӧ����Ҫ����, �Ѳ��ַ��ص�����д������ṹ
    // ����ͼ����Դ, ����ͼ��dpi����, ����ͼ��ߴ�, ��Ӧ��������ṹ����
}*LPWINDOW_ICON_DATA;

DECLARE_HANDLE(IDRAW_HRESOURCE);
typedef LPWINDOW_ICON_DATA(WINAPI* pfn_GetWindowIcon)(HWND hWnd, IDRAW_HRESOURCE hResource, LPCWSTR pszID, void* pArg, RECT_F* prc, STATE state, UINT dpi);
// ��ť���ػ�ʱ�Ļص�����
typedef int(WINAPI* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);


// ��Ӱ�ť���ӿڹ�������ʹ�õĽṹ
typedef struct IDRAW_STRUCT_TOOL_INSERT
{
    int idCommand;              // ��ť��ID, �����¼�ʱ������ID���ݹ�ȥ
    LPCWSTR lpszTitle;          // ��ť����, ����Ϊ0, Ϊ0��û�б���, ����ƶ���ȥʱ��������ʾ˵������, �Ӵ���ʾ, ������ⲻ����ʾ�ڹ�������
    LPCWSTR lpszTips;           // ��ť˵��, ����Ϊ0, Ϊ0��û��˵��, ����ƶ���ȥʱ��������ʾ˵��
    LPARAM param;               // ��Ŀ��ֵ
    BYTE fsState;               // ��ť״̬, ��ο�msdn��TBBUTTON �ṹ˵��
    BYTE fsStyle;               // ��ť��ʽ, ��ο�msdn��TBBUTTON �ṹ˵��
    BYTE bReserved[6];          // ��������
    pfn_ToolDrawButton pfnDraw; // �滭�ص�
    const void* pImageData;     // ͼƬ����ָ��
    int nImageSize;             // ͼƬ���ݴ�С
    void* pPlugin;              // �����¼����õĲ���ص�
}*LPIDRAW_STRUCT_TOOL_INSERT;



struct BINDWINDOW_INFO
{
    IDRAW_HRESOURCE hResource;
    pfn_GetWindowIcon callback;
    LPCSTR guid;
};

// �滭�ӿ���
class IiDraw
{
public:
    virtual int Release() = 0;
    virtual int Addref() = 0;

public:



    // ���໯����, �ڲ�ʹ�ù̶�����ɫ, ��Ҫ�����޸���ɫ�Ŀ��Է���ֵ��ָ������滻
    // hWnd = �����໯�Ĵ��ھ��
    // proc = �������໯�Ļص�����
    // reMoveBorder = �Ƿ�ɾ���߿�, ��ɾ�� WS_BORDER �� ��չ��ʽ��������ӱ߿����ʽ
    // isSetBack = �Ƿ����ñ�����ɫ, �������, ��ô��ͬ�����滭����ʱ����ʹ�������ɫ�滭, �������������Ҳһ��
    // reMoveClassStyle = Ҫɾ��������ʽ, ɾ�������ʽ��������������Ĵ��ڶ��ᱻɾ�������ʽ
    virtual LPOBJSTRUCT _subclass(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder = true, bool isSetBack = true, DWORD reMoveClassStyle = CS_HREDRAW | CS_VREDRAW) = 0;


    virtual LPOBJSTRUCT SubButton(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;
    virtual LPOBJSTRUCT SubEdit(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, bool isScroll = true) = 0;
    virtual LPOBJSTRUCT SubStatic(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;
    virtual LPOBJSTRUCT SubStatusBar(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;

    // �ػ����Ϳ�
    // hWnd = ���Ϳ򴰿ھ��
    // iItemHeight = ��Ŀ�߶�
    // isScroll = �Ƿ��ػ�������
    // isHot = �Ƿ����ȵ�Ч��
    virtual LPOBJSTRUCT SubTreeView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int ItemHeight, bool isScroll = true, bool isHot = false) = 0;

    virtual LPOBJSTRUCT SubListBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight, bool isScroll = true, bool isHot = false) = 0;
    virtual LPOBJSTRUCT SubListView(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int ItemHeight, bool isScroll = true, bool isHot = false, int fmt = DT_VCENTER | DT_SINGLELINE) = 0;

    virtual LPOBJSTRUCT SubTool(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;

    virtual LPOBJSTRUCT Submsctls_updown32(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;

    virtual LPOBJSTRUCT SubTab(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;
    virtual LPOBJSTRUCT SubComboBox(HWND hWnd, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle, int nItemHeight) = 0;
    virtual LPVOID DrawScroll(HWND hWnd, int nListViewItemHeight, bool isTreeView, bool isCallSetScrollInfo) = 0;


    // ���໯�˵�������ڵĴ���, ����WM_DRAWITEM��Ϣ, ����Ѿ����໯, �򲻴���
    virtual LPOBJSTRUCT _subclass_hMenu_Window(HWND hWnd, pfn_SubClassWndProc proc) = 0;








    virtual LPOBJSTRUCT _subclass_EWindow(HWND hWnd, HINSTANCE hInst, LPSYSBUTTONIMAGE pNskn) = 0;


    // ���໯�����Դ����м���Ǹ��Ի�ѡ���
    virtual LPOBJSTRUCT _subclass_CustomTab(HWND hWnd) = 0;
    // ��ʼ�������е�ѡ���, �޸�ѡ���λ��, ����ѡ�������
    // hTab = ����ʼ����ѡ���, hTabOld = �������Դ���ѡ���, isMove = Ϊ����ֻ�ƶ�����, ���Եڶ�������
    virtual void _Tab_Work_init(HWND hTab, bool isMove) = 0;
    // ��ʼ��״̬�е�ѡ���, �޸�ѡ���λ��, ����ѡ�������
    // hTab = ����ʼ����ѡ���, hTabOld = �������Դ���ѡ���, isMove = Ϊ����ֻ�ƶ�����, ���Եڶ�������
    virtual void _Tab_State_init(HWND hTab, bool isMove) = 0;

    // hook��Ϣ�������ﴰ��
    virtual void EWindow_Hook_SetWindowsHook() = 0;




    // ������ˢ, ���صĻ�ˢ�����ͷ�, Ҫ�ͷ�ֻ����֧�ֿⱻж�ػ��߹ر�������ʱ���ñ������ Release() �ͷ�
    // ��ͬ��ɫ���ᴴ�����, �ڲ�ʹ�� map ��¼���д����Ļ�ˢ
    virtual HBRUSH Brush_Create(COLORREF cr) = 0;



    virtual LPOBJSTRUCT _subclass_tool(HWND hWnd, pfn_SubClassWndProc proc, bool reMoveBorder, bool isSetBack, DWORD reMoveClassStyle) = 0;


    // ���µ�ǰ����Ĵ���, ���ػ������������ں�֮ǰ����Ĵ���
    virtual void TopState_UpdateStateWindow(HWND hWnd) = 0;
    // ��ȡ��ǰ����Ĵ���
    virtual HWND TopState_GetActivateWindow() = 0;

    virtual void TopState_SetWindowText(HWND hWnd, LPCWSTR lpszText, bool isUpdate) = 0;



    // ���������ԵĲ˵�, id�����������ԵĲ˵�id
    //127,    // �����˵����Ĳ˵�, һ��Ѷ����˵�+�Ӽ��˵�
    //235,    // �����ط��Ҽ��˵�, ���ֶ��빤����, ���߲˵�  �鿴->������
    //236,    // ������ �Ҽ��˵�
    //237,    // ֧�ֿ� -> ���Ϳ� -> �Ҽ��˵�
    //238,    // ���� -> ���Ϳ� -> ģ�����ñ� �Ҽ��˵�, ����ģ��/�ⲿ�ļ���¼��, �����ط��Ҽ��˵�Ҳ��238, �������˵��ͬ
    //239,    // ���� -> ���Ϳ� -> �������� �Ҽ��˵�
    //240,    // ����������Ҽ��˵�
    //276,    // ����ʱ ������ �Ҽ��˵�
    //293,    // ������������Կ� -> �������Ҽ��˵�
    //587,    // �ױ�״̬�� -> ���ӱ��Ҽ�
    //606,    // �����л�ѡ����Ҽ��˵�
    //609,    // ��ǩ�Ҽ��˵�
    //610,    // �����Ҽ��˵�
    //611,    // �ױ�״̬�� -> ������ʷ
    //626,    // ���� -> �����ļ����Ҽ��˵�
    //627,    // ���� -> ���Ϳ� -> �ⲿ�ļ���¼�� �Ҽ��˵�
    //1,      // �༭���Ҽ��˵�
    //12,     // �༭���Ҽ��˵� ���˸���/�ر����뷨
    virtual HMENU EMenu_LoadMenu(int id) = 0;

    // �����˵�
    virtual BOOL EMenu_PopUpMenu(HWND hWnd, HMENU hMenu, int x = 0, int y = 0) = 0;

public:
    // �������˵�����Ŀؼ�, ȥ������߿�, ���໯�������
    virtual void _subclass_menu() = 0;
    virtual void _subclass_work() = 0;
    // ����״̬����Ŀؼ�, ȥ������߿�, ���໯�������
    virtual void _subclass_state() = 0;
    // ����ײ�״̬��, ȥ������߿�, ���໯�������
    virtual void _subclass_statusBar() = 0;
    // �����м�������Ŀؼ�, ȥ������߿�, ���໯�������
    virtual void _subclass_MDIControl() = 0;
    // ���������, ȥ������߿�, ���໯�������
    virtual void _subclass_control() = 0;



public:
    
    // ��ʼ���滭��Ϣ, ����� BeginPaint, �ᴴ���ڴ�DC, �����ڴ�DC, DX��Ⱦ����0
    // ��������õ�������ɫ, ������ˢ, �߿򻭱ʶ�ѡ��DC
    // ��� isFillBack Ϊtrue, ���ڲ���ʹ�ñ�����ˢ����ڴ�DC
    virtual HDC BeginPaint(LPOBJSTRUCT pData, BEGIN_END_PAINT_STRUCT& data, bool isDxRender = false, bool isFillBack = true) = 0;


    // �����滭, ����� EndPaint, �����dx��Ⱦ����dx����
    virtual bool EndPaint(BEGIN_END_PAINT_STRUCT& data) = 0;

public:




};




// ��Դģ��
class IEWindowResource
{
public:
    // ��ȡ������������
    virtual LPBYTE GetELogo(DWORD* size) = 0;

    // ��ȡĬ��������ɫ��clr�ļ�, �����ļ�����, ����+����, Ŀǰһ����6���ļ�
    virtual LPBYTE GetDefThemeData(DWORD* size);

    // ��ȡ������������
    virtual LPSYSBUTTONIMAGE GetNskn() = 0;

    // ��ȡ������ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA Tool_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��ȡ֧�ֿ����Ϳ�ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TreeFne_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��ȡ�������Ϳ�ͼ��, ����ͼ������, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TreeProgram_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��ȡ�����ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TreeControl_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��ȡ���Ϳ�Ӽ���ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TreeExpand_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��ȡ��Ͽ�ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA Combobox_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��ȡ�˵����ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA Menu_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��ȡ���ҵ�������ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA MsctlsUpdowm32_GetIcon_lr(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��ȡ���µ�������ͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA MsctlsUpdowm32_GetIcon_tb(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��ȡ����״̬���İ�ťͼ��, pIconType = �Ƿ��Ǵ�Ĭ��ֵ��õ���ͼ��, ���ڵ��óɹ�ʱ��ֵ
    // pIconType = ���ջ�ȡ����ͼ����Դ, 0=�û�ͼ��, 1=�û�Ĭ��, 2=Ĭ��Ƥ����ͼ��, 3=Ĭ�����Ĭ��ͼ��
    virtual LPWINDOW_ICON_DATA TopState_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;
    
    // �ӳ���ͼ�����ȡͼ��, ����ͼ������, û�����ͼ�귵��0
    virtual LPWINDOW_ICON_DATA GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;


    // �ͷ���Դ
    virtual void EWindow_Resource_Free() = 0;

    // ���ļ�������Դ, ������Դ���, ���ػ�Ӱ��Ч��, һ��ext�ļ�����һ�ξ���, ����ʱ���Զ�����
    virtual IDRAW_HRESOURCE LoadFromFile(LPCWSTR szFile) = 0;
    // ���ڴ������Դ, ������Դ���, ���ػ�Ӱ��Ч��, һ��ext�ļ�����һ�ξ���, ����ʱ���Զ�����
    virtual IDRAW_HRESOURCE LoadFromMemory(const void* pData, int size) = 0;

    // ���û����ص���Դ���ȡָ��ID��ͼ��, ����idָ�������
    virtual LPWINDOW_ICON_DATA User_GetIcon(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi = 96) = 0;


    // ȡ��������а󶨴�����Դ�Ļص�
    virtual bool UnBindWindow(LPCSTR guid) = 0;

    // ��ȡĬ���������Դ���, ���ݴ��ھ����ȡ
    // hWnd = Ҫ��ȡ��Դ�Ĵ��ھ��, ��ȡ�˵�ʹ�� HRESOURCE_MENU, ��ȡ���Ϳ�չ��������ťʹ�� HRESOURCE_TREEEXPAND
    virtual IDRAW_HRESOURCE GetDefControlIcon(HWND hWnd) = 0;

    // ����Դ�󶨵�������, �´��ػ��������ʱ��������Դ��ȡͼ��
    // hWnd = Ҫ�󶨵Ĵ��ھ��, �󶨲˵�ʹ�� HRESOURCE_MENU, �����Ϳ�չ��������ťʹ�� HRESOURCE_TREEEXPAND
    // hResource = ���ص���Դ���, ����������Ϊ0, �����һ������Ϊ0, �򲻻�����趨��ͼ��
    // callback = �ص�����, �����������Ҫ����ͼ��ʱ����, Ϊ0��ʹ��Ĭ�ϵ���Դ����, ���û��Ĭ�ϵ���Դ�򲻼���
    //              �ص���������Ҫ�����Ƿ�Ϊdx��Ⱦ��д�� prc �ͷ���ֵ
    virtual bool BindWindow(LPCSTR guid, HWND hWnd, IDRAW_HRESOURCE hResource, pfn_GetWindowIcon callback) = 0;

    // �Ӵ��ھ����ȡ�ص�����, ���Ե��ûص���������ȡͼ��, ����д�뻺����������, bufCallback Ϊ0�򷵻���Ҫ�Ļ�������Ա��
    // hWnd = ���ĸ������ϻ�ȡ�ص���Ϣ
    // bufCallback = ���հ�ʱ���ݵĻص���������, ���Ϊ0�򷵻ذ󶨵�����
    // bufCount = ��ʾ bufCallback ����������ж��ٸ���Ա
    virtual int GetResourceCallbackFromHwnd(HWND hWnd, BINDWINDOW_INFO* bufCallback = 0, int bufCount = 0) = 0;

    // �Ӵ��ھ�����ȡͼ��, ����ڰ󶨵�ʱ����˻ص�����, �����ûص���������
    // �ڲ����� GetResourceFromHwnd() + User_GetIcon()/User_GetIconDx() ��ʵ��
    // hWnd = �󶨵Ĵ��ھ��, ���û�а��򷵻�0
    // pszID = Ҫȡ��ͼ��ID, ����һ������Ŀ�ı�
    // arg = ������ݲ�ͬ������ݲ�ͬ�ṹ, ���Ϳ򴫵� TVITEMEXW�ṹָ��, 
    // prc = ����ͼ����ͼƬ���λ��
    // state = ͼƬ״̬, STATE ö�ٳ������ֵ, Ŀǰֻ֧�� STATE::HOT, STATE::DOWN, STATE::BAN, STATE::NORMAL ��4��
    // pIconType = ����ͼ����Դ, 0=��ID�л�ȡ, 1=��Ĭ��ͼ�����ȡ
    virtual LPWINDOW_ICON_DATA GetIconFromHwnd(HWND hWnd, LPCWSTR pszID, void* arg, RECT_F* prc, STATE state, UINT dpi = 96) = 0;

    // ��gdi+λͼ����dxλͼ��������Դ����ֵ, ����ָ����뱣֤���, �ڲ����������ڴ������洫�ݽ������ı���λͼ
    // ����ֵ��ʹ��ʱ������� FreeResourceReturn ���ͷ�
    // pImg = ͼƬ���ݵ�ַ
    // pImgSize = ͼƬ���ݴ�С
    // text = ���Ƶ�ǰ��Ŀ��ʾ���ı�, �����޸�ԭ����ı�, ֻ�ǻ滭ʱ��ʾ����ı�, ����Ϊ0
    // remarks = ��ע�ı�, ����Ϊ0
    // crText = �ı�ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
    // crBack = ����ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
    virtual LPWINDOW_ICON_DATA MakeResourceReturn(const void* pImg, int pImgSize, LPCWSTR text, LPCWSTR remarks, DWORD* crText, DWORD* crBack) = 0;

    // �ͷ� MakeResourceReturn() ���ɵķ���ֵ
    virtual bool FreeResourceReturn(LPWINDOW_ICON_DATA pData) = 0;

    // ���ڴ����gdi+ͼƬ
    // pData = ͼƬ����
    // size = ͼƬ���ݴ�С
    virtual Gdiplus::Bitmap* LoadGpImageFromMemory(const void* pData, int size) = 0;

    // �ͷŴ� LoadGpImageFromMemory() ���ص�ֵ
    virtual bool FreeGpImage(Gdiplus::Bitmap*) = 0;

    // ��ȡmarlett����, ͼ������, �滭������
    virtual HFONT GetMarlettFont(int size) = 0;

    // ��ȡ��������
    virtual LPBYTE GetMarlettData(int* size) = 0;
};



DECLARE_HANDLE(ICTinyXml2_XMLPrinter);
DECLARE_HANDLE(ICTinyXml2_XMLNode);
DECLARE_HANDLE(ICTinyXml2_XMLElement);

class ICTinyXml2
{
public:

    // ��unicode�ַ���ת���ɵ�ǰ������Ҫ���ı���ʽ
    virtual LPCWSTR ToPointer(const wchar_t* pstr) = 0;
    // ��ansi�ַ���ת���ɵ�ǰ������Ҫ���ı���ʽ
    virtual LPCWSTR ToPointer(const char* pstr) = 0;
public:
    // ��յ�ǰxml, ����ʱ���Զ�����
    virtual void Clear() = 0;
    virtual ICTinyXml2_XMLElement NewElement(LPCSTR name) = 0;
    virtual ICTinyXml2_XMLNode InsertEndChild(ICTinyXml2_XMLNode node, ICTinyXml2_XMLElement child) = 0;
    virtual ICTinyXml2_XMLNode FirstChildElement(ICTinyXml2_XMLNode node, LPCSTR name) = 0;
    virtual void SetText(ICTinyXml2_XMLElement node, LPCSTR text) = 0;
    virtual void SetAttribute(ICTinyXml2_XMLElement node, LPCSTR name, LPCSTR value) = 0;
    virtual ICTinyXml2_XMLElement ToElement(ICTinyXml2_XMLNode node) = 0;


    // ����xml, ����յ�ǰ����xml����
    // pszRootName = ���ڵ���, Ϊ0�򴴽�xml�ն���
    virtual bool Create(LPCWSTR pszRootName) = 0;

    // ����xmlͷ
    virtual bool SetDeclaration(LPCWSTR pszText) = 0;

    // ����xml����, ����յ�ǰ����xml����
    virtual bool Parse(LPCWSTR p) = 0;

    // ����xml����, ����յ�ǰ����xml����
    virtual void Print(ICTinyXml2_XMLPrinter streamer = 0) = 0;

    // ���ļ�����xml, ����յ�ǰ����xml����
    virtual bool LoadFile(LPCWSTR filename) = 0;

    // ���Ѿ��򿪵��ļ�����xml, ����յ�ǰ����xml����
    virtual bool LoadFile(FILE* fp) = 0;

    // ���浱ǰxml������, ����ڲ���¼���ļ�Ϊ��ʱ�ᱣ�浱ǰ�ļ�·��
    // filename = �����·��, ���Ϊ��, ��ʹ���ڲ���¼��·��, ����ڲ�û�м�¼·�������ʧ��
    // compact = ����ʽ���ı�, Ĭ��Ϊfalse, Ĭ�ϻ��ʽ���ı�
    virtual bool SaveFile(LPCWSTR filename = 0, bool compact = false) = 0;

    // ���浱ǰxml���ݵ��򿪵��ļ���
    virtual bool SaveFile(FILE* fp, bool compact = false) = 0;

    // ���ñ���ʱʹ�õ�Ŀ¼
    virtual void SetSaveFile(LPCWSTR file) = 0;

    // ��ȡ�ڲ���¼���ļ�·��
    virtual LPCWSTR GetSaveFile() = 0;

public:

    // ȡ���ڵ���, ���δ�����򷵻ؿ�
    virtual LPCWSTR GetRootName() const = 0;

    // ȡ�ӽڵ���, ȡ�ò����ڵ�������ӽڵ�ĸ���
    // node = �ڵ�ָ��, ��ȡ����ڵ��µ��ӽڵ�����
    virtual int GetChildNodeCount(ICTinyXml2_XMLNode node) const = 0;

    // ȡ�ӽڵ���, ȡ�ò����ڵ�������ӽڵ�ĸ���
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual int GetChildNodeCount(LPCWSTR pszPath = 0) const = 0;
    // ȡ�ڵ���, ȡ�ò����ڵ������, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ��ȡ����ڵ�Ľڵ���
    virtual LPCWSTR GetNodeName(ICTinyXml2_XMLNode node) const = 0;
    // ȡ�ڵ���, ȡ�ò����ڵ������, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual LPCWSTR GetNodeName(LPCWSTR pszPath = 0) const = 0;
    // ȡ�ڵ�ֵ, ȡ�ò����ڵ��ֵ, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ��ȡ����ڵ�Ľڵ�ֵ
    virtual LPCWSTR GetNodeValue(ICTinyXml2_XMLNode node) const = 0;
    // ȡ�ڵ�ֵ, ȡ�ò����ڵ��ֵ, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual LPCWSTR GetNodeValue(LPCWSTR pszPath = 0) const = 0;

    // ö��ָ��·���ڵ����������, ������������, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pNames = ��������������, �����򲻻�ȡ
    // pValues = ��������Ӧ��ֵ, �����򲻻�ȡ
    virtual int EnumAttribute(LPCWSTR pszPath = 0, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const = 0;
    // ö��ָ��·���ڵ����������, ������������, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ����ֵ������ڵ��ȡ
    // pNames = ��������������, �����򲻻�ȡ
    // pValues = ��������Ӧ��ֵ, �����򲻻�ȡ
    virtual int EnumAttribute(ICTinyXml2_XMLNode node, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const = 0;
    // ȡ����������, ������������, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pNames = ��������������, �����򲻻�ȡ
    // pValues = ��������ֵ����, �����򲻻�ȡ
    virtual int GetAttrName(LPCWSTR pszPath = 0, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const = 0;

    // ȡ����������, ������������, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ����ֵ������ڵ��ȡ
    // pNames = ��������������, �����򲻻�ȡ
    // pValues = ��������ֵ����, �����򲻻�ȡ
    virtual int GetAttrName(ICTinyXml2_XMLNode node, std::vector<LPCWSTR>* pNames = 0, std::vector<LPCWSTR>* pValues = 0) const = 0;

    // ȡ����ֵ, ȡ�ò����ڵ���ض����Ե�ֵ, <�ڵ��� ������="����ֵ"/>
    // node = �ڵ�ָ��, ������ڵ��ȡ����ֵ
    // pszAttrName = ������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    // isAttribute = Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
    virtual LPCWSTR GetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, BOOL* isAttribute = 0) const = 0;
    // ȡ����ֵ, ȡ�ò����ڵ���ض����Ե�ֵ, <�ڵ��� ������="����ֵ"/>
    // pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszAttrName = ������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    // isAttribute = Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
    virtual LPCWSTR GetAttrValue(LPCWSTR pszPath, LPCWSTR pszAttrName, BOOL* isAttribute = 0) const = 0;
    //// ���뱸ע�ı�, ֵΪ0��ɾ���ñ�ע�ı�
    //// pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    //// pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    //virtual bool InsertComment(ICTinyXml2_XMLNode node, LPCWSTR pszCommentText = 0) = 0;

    //// ���뱸ע�ı�, ֵΪ0��ɾ���ñ�ע�ı�
    //// pszPath = ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    //// pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    //virtual bool InsertComment(LPCWSTR pszPath, LPCWSTR pszCommentText = 0) = 0;

    // �����ӽڵ�, �½�һ���ڵ㣬���Ѹò����ڵ���Ϊ���½ڵ�ĸ��ڵ㣬Ȼ����뵽XML����
    // pszParentPath = ���ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�, �¼���Ľڵ�·�� = @1/@1/@2/@�ڵ�����
    // pszNodeName = �µĽڵ���, Ϊ0�����ʧ��  <�ڵ���></�ڵ���>
    // pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    // isCDATA = ���������Ϊ�棬��ڵ�ֵ�ı����� "<![CDATA[" �� "]]>" ��Χ������ζ�Ÿ��ı����԰��������ַ�(�Բ�Ӱ��CDATA����Ϊ��}
    virtual bool InsertChildNode(LPCWSTR pszParentPath, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) = 0;
    // �����ӽڵ�, �½�һ���ڵ㣬���Ѹò����ڵ���Ϊ���½ڵ�ĸ��ڵ㣬Ȼ����뵽XML����
    // parentNode = ���ڵ�
    // pszNodeName = �µĽڵ���, Ϊ0�����ʧ��  <�ڵ���></�ڵ���>
    // pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    // isCDATA = ���������Ϊ�棬��ڵ�ֵ�ı����� "<![CDATA[" �� "]]>" ��Χ������ζ�Ÿ��ı����԰��������ַ�(�Բ�Ӱ��CDATA����Ϊ��}
    virtual bool InsertChildNode(ICTinyXml2_XMLNode parentnode, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) = 0;
    // ����ڵ�, ���뵽·���ڵ����
    // pszPath ����·��Ϊ@0 ����뵽��һ���ڵ�, @-1 ����뵽���һ���ڵ�, ������뵽 ָ��·������һ���ڵ�
    // pszNodeName = �µĽڵ���, Ϊ0�����ʧ��  <�ڵ���></�ڵ���>
    // pszNodeValue = �ڵ�ֵ  <�ڵ���>�ڵ�ֵ</�ڵ���>
    // isCDATA = ���������Ϊ�棬��ڵ�ֵ�ı����� "<![CDATA[" �� "]]>" ��Χ������ζ�Ÿ��ı����԰��������ַ�(�Բ�Ӱ��CDATA����Ϊ��}
    virtual bool InsertNode(LPCWSTR pszPath, LPCWSTR pszNodeName, LPCWSTR pszNodeValue = 0, bool isCDATA = false, ICTinyXml2_XMLNode* ppNode = 0) = 0;

    // ��������, �ڲ����ڵ��в���һ���µ����ԣ��������Ը�ֵ
    // node = �ڵ�ָ��, ������ڵ��������
    // pszAttrName = ��������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    // pszAttrValue = ����ֵ  <�ڵ��� ������="����ֵ"/>
    // isAttribute = Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
    virtual bool InsertAttr(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) = 0;
    // ��������, �ڲ����ڵ��в���һ���µ����ԣ��������Ը�ֵ
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszAttrName = ��������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    // pszAttrValue = ����ֵ  <�ڵ��� ������="����ֵ"/>
    // isAttribute = Ҫ��ȡ����������Ƿ����, ����Ϊ0, ������Բ�����, ���ֵΪ0, ������Ϊ1
    virtual bool InsertAttr(LPCWSTR pszPath, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) = 0;

    // ɾ���ڵ�, ɾ�������ڵ�����������ӽڵ�
    // node = �ڵ�ָ��, ɾ������ڵ�
    virtual bool DelNode(ICTinyXml2_XMLNode node) = 0;
    // ɾ���ڵ�, ɾ�������ڵ�����������ӽڵ�
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual bool DelNode(LPCWSTR pszPath) = 0;

    // ɾ������
    // node = �ڵ�ָ��, ������ڵ�ɾ������
    // pszAttrName = ������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    virtual bool DelAttr(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName) = 0;
    // ɾ������
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszAttrName = ������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    virtual bool DelAttr(LPCWSTR pszPath, LPCWSTR pszAttrName) = 0;

    // �޸Ľڵ���, �޸�ָ���ڵ������
    // node = �ڵ�ָ��, ������ڵ����ýڵ���
    // pszNodeName = �½ڵ���, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    virtual bool SetNodeName(ICTinyXml2_XMLNode node, LPCWSTR pszNodeName) = 0;
    // �޸Ľڵ���, �޸�ָ���ڵ������
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszNodeName = �½ڵ���, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    virtual bool SetNodeName(LPCWSTR pszPath, LPCWSTR pszNodeName) = 0;

    // �޸Ľڵ�ֵ, �޸�ָ���ڵ��ֵ
    // node = �ڵ�ָ��, ������ڵ����ýڵ�ֵ
    // pszNodeValue = �½ڵ�ֵ   <�ڵ��� ������="����ֵ">�ڵ�ֵ</�ڵ���>
    virtual bool SetNodeValue(ICTinyXml2_XMLNode node, LPCWSTR pszNodeValue) = 0;
    // �޸Ľڵ�ֵ, �޸�ָ���ڵ��ֵ
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszNodeValue = �½ڵ�ֵ   <�ڵ��� ������="����ֵ">�ڵ�ֵ</�ڵ���>
    virtual bool SetNodeValue(LPCWSTR pszPath, LPCWSTR pszNodeValue) = 0;

    //// �޸�������, �޸�ָ���ڵ����������
    //// pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    //// pszOldAttrName = ��������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/> 
    //// pszNewAttrName = ��������, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/>
    //virtual bool SetAttrName(LPCWSTR pszPath, LPCWSTR pszOldAttrName, LPCWSTR pszNewAttrName)
    //{
    //    ICTinyXml2_XMLNode node;
    //    if (!GetNode(pszPath, node)) return false;
    //    const tinyxml2::XMLAttribute* attr;
    //    tinyxml2::XMLElement* ele;
    //    if (!GetAttr(node, ele, pszOldAttrName, attr))return false;
    //    //attr->SetName(pszNewAttrName);
    //    return true;
    //}


    // �޸�����ֵ, �޸�ָ���ڵ������ֵ
    // node = �ڵ�ָ��, ������ڵ���������ֵ
    // pszAttrName = ������ֵ, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/> 
    // pszAttrValue = ������ֵ  <�ڵ��� ������="����ֵ"/>
    // isAttribute = ������������Ƿ����
    virtual bool SetAttrValue(ICTinyXml2_XMLNode node, LPCWSTR pszAttrName, LPCWSTR pszNewAttrValue, BOOL* isAttribute = 0) = 0;
    // �޸�����ֵ, �޸�ָ���ڵ������ֵ
    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    // pszAttrName = ������ֵ, Ϊ0�����ʧ��  <�ڵ��� ������="����ֵ"/> 
    // pszAttrValue = ������ֵ  <�ڵ��� ������="����ֵ"/>
    // isAttribute = ������������Ƿ����
    virtual bool SetAttrValue(LPCWSTR pszPath, LPCWSTR pszAttrName, LPCWSTR pszAttrValue, BOOL* isAttribute = 0) = 0;

    // ��ȡxml����, ����xml�ı�
    virtual LPCWSTR GetXmlData() = 0;

public:
    // ���������ڵ��Ƿ���ͬ, ���������ӽڵ�, 2���ڵ㲻��λ��ͬһDocument��
    // ע�⣺�����XMLDocument�ϵ��ã��򽫷���false��.
    virtual bool Compare(const ICTinyXml2_XMLNode compare) const = 0;
public:

    // pszPath = �ڵ�ȫ·��, ���ڵ�/�ӽڵ�/Ҷ�ӽڵ�, ���� @1/@1/@2, Ϊ0��Ϊ���ڵ�
    virtual bool GetNode(LPCWSTR pszPath, ICTinyXml2_XMLNode& node) const = 0;

};

class IConfig  // ���ýӿ���
{
public:

    virtual BOOL IsRunSave            ( BOOL* pValue = 0 ) = 0;   // ����/������Ƿ񱣴�Դ��, ȡֵ��Ҫ����һ��ָ����ս�
    virtual BOOL IsDxRender           ( BOOL* pValue = 0 ) = 0;   // �Ƿ�DX��Ⱦ, ȡֵ��Ҫ����һ��ָ����ս��
    virtual BOOL IsDPIAware           ( BOOL* pValue = 0 ) = 0;   // �Ƿ���DPI��֪
    virtual BOOL IsDPIScaleWnd        ( BOOL* pValue = 0 ) = 0;   // DPI�ı�ʱ�Ƿ����Ŵ���
    virtual BOOL IsShowCodeTabClose   ( BOOL* pValue = 0 ) = 0;   // ��ʾ����ѡ��еĹرհ�ť
    virtual BOOL IsShowLocationTool   ( BOOL* pValue = 0 ) = 0;   // �Ƿ���ʾ��λ������
    virtual int  programTreeDblclk    ( int * pValue = 0 ) = 0;   // �������Ϳ�˫��ģʽ, 0=ֻ�л�, 1=ֻչ��, 2=չ��+�л�
    virtual int  wheelPos             ( int * pValue = 0 ) = 0;   // ������һ�εľ���
    virtual BOOL noClipChildren       ( BOOL* pValue = 0 ) = 0;   // ���ų��Ӵ�������
    virtual int  MenuItemHeight       ( int * pValue = 0 ) = 0;   // �˵���߶�
    virtual int  MenuSeparatorHeight  ( int * pValue = 0 ) = 0;   // �ָ��˵���߶�
    virtual int  noChangeTab_work     ( int * pValue = 0 ) = 0;   // ���Զ��л�������
    virtual int  noChangeTab_state    ( int * pValue = 0 ) = 0;   // ���Զ��л�״̬��
    virtual int  scroll_width         ( int * pValue = 0 ) = 0;   // ���������
    virtual int  scroll_btn_size      ( int * pValue = 0 ) = 0;   // ��������ť�ߴ�

    virtual BOOL tree_fne_ishot       ( BOOL* pValue = 0 ) = 0;   // ֧�ֿ����Ϳ��Ƿ�֧���ȵ�
    virtual BOOL tree_program_ishot   ( BOOL* pValue = 0 ) = 0;   // �������Ϳ��Ƿ�֧���ȵ�
    virtual BOOL tree_control_ishot   ( BOOL* pValue = 0 ) = 0;   // ��������Ϳ��Ƿ�֧���ȵ�
    virtual BOOL tree_var_ishot       ( BOOL* pValue = 0 ) = 0;   // ���������Ϳ��Ƿ�֧���ȵ�
    virtual BOOL tree_call_ishot      ( BOOL* pValue = 0 ) = 0;   // ���ñ����Ϳ��Ƿ�֧���ȵ�
    virtual BOOL list_find1_ishot     ( BOOL* pValue = 0 ) = 0;   // ��Ѱ1�б���Ƿ�֧���ȵ�
    virtual BOOL list_find2_ishot     ( BOOL* pValue = 0 ) = 0;   // ��Ѱ1�б�����Ϳ��Ƿ�֧���ȵ�

    virtual int  tree_fne_height      ( int * pValue = 0 ) = 0;   // ֧�ֿ����Ϳ���Ŀ�߶�
    virtual int  tree_program_height  ( int * pValue = 0 ) = 0;   // �������Ϳ���Ŀ�߶�
    virtual int  tree_control_height  ( int * pValue = 0 ) = 0;   // ��������Ϳ���Ŀ�߶�
    virtual int  tree_control_orgIcon ( int * pValue = 0 ) = 0;   // ��������Ϳ���ʾԭͼ��
    virtual int  tree_var_height      ( int * pValue = 0 ) = 0;   // ���������Ϳ���Ŀ�߶�
    virtual int  tree_call_height     ( int * pValue = 0 ) = 0;   // ���ñ����Ϳ���Ŀ�߶�
    virtual int  list_find1_height    ( int * pValue = 0 ) = 0;   // ��Ѱ1�б����Ŀ�߶�
    virtual int  list_find2_height    ( int * pValue = 0 ) = 0;   // ��Ѱ2�б����Ŀ�߶�

    virtual int  tab_work_direction   ( int * pValue = 0 ) = 0;   // �����з���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    virtual BOOL tab_work_isIcon      ( BOOL* pValue = 0 ) = 0;   // �������Ƿ���ʾͼ��
    virtual int  tab_work_flags       ( int * pValue = 0 ) = 0;   // �����б�ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ

    virtual int  tab_state_direction  ( int * pValue = 0 ) = 0;   // ״̬�з���, 0=���, 1=����, 2=�ұ�, 3=�ױ�
    virtual BOOL tab_state_isIcon     ( BOOL* pValue = 0 ) = 0;   // ״̬���Ƿ���ʾͼ��
    virtual int  tab_state_flags      ( int * pValue = 0 ) = 0;   // ״̬�б�ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ

    virtual int  tab_control_flags    ( int * pValue = 0 ) = 0;   // ������ʶ, ��¼λ��, �Ƿ����, �Ƿ񸡶�, �Ƿ�ͣ������Ϣ TOP_STATE_BUTTON ö�ٳ������ֵ, ���� __query_flags �ж�ֵ

    virtual int  tab_code_direction   ( int * pValue = 0 ) = 0;   // �����л�ѡ��з���, 3=�ױ�, ��3=����
    virtual BOOL tab_code_isIcon      ( BOOL* pValue = 0 ) = 0;   // �����л�ѡ����Ƿ���ʾͼ��

    virtual RECT rcWork               ( RECT* pValue = 0 ) = 0;   // �����д���λ��, �����������������μ���λ��
    virtual RECT rcState              ( RECT* pValue = 0 ) = 0;   // ״̬�д���λ��, �����������������μ���λ��
    virtual RECT rcControl            ( RECT* pValue = 0 ) = 0;   // ����䴰��λ��, �����������������μ���λ��
    virtual RECT rcMenu               ( RECT* pValue = 0 ) = 0;   // �˵��д���λ��, �����������������μ���λ��
    virtual RECT rcMDIClient          ( RECT* pValue = 0 ) = 0;   // MDIClient����λ��, �����������������μ���λ��

    virtual RECT rcWork1              ( RECT* pValue = 0 ) = 0;   // �����д���λ��, ��ʾ����Ļ��λ��
    virtual RECT rcState1             ( RECT* pValue = 0 ) = 0;   // ״̬�д���λ��, ��ʾ����Ļ��λ��
    virtual RECT rcControl1           ( RECT* pValue = 0 ) = 0;   // ����䴰��λ��, ��ʾ����Ļ��λ��


};

class IThemeColor
{

public:

    virtual COLORREF crBack                  ( COLORREF* pValue = 0 ) = 0;  // [Basics], ������ɫ
    virtual COLORREF crBackExtern            ( COLORREF* pValue = 0 ) = 0;  // [Basics], �ⲿ�ı�����ɫ, ����ѡ��е��Ӽ�
    virtual COLORREF crStateBar              ( COLORREF* pValue = 0 ) = 0;  // [Basics], �����/������/״̬�ж����������ɫ

    virtual COLORREF crBorder                ( COLORREF* pValue = 0 ) = 0;  // [Basics], �߿���ɫ
    virtual COLORREF crBorder_run            ( COLORREF* pValue = 0 ) = 0;  // [Basics], ����ʱ�߿���ɫ
    virtual COLORREF crBorder_nf             ( COLORREF* pValue = 0 ) = 0;  // [Basics], ȡ������ʱ�ı߿���ɫ

    virtual COLORREF crShadow                ( COLORREF* pValue = 0 ) = 0;  // [Basics], ��Ӱ��ɫ
    virtual COLORREF crShadow_run            ( COLORREF* pValue = 0 ) = 0;  // [Basics], ����ʱ��Ӱ��ɫ
    virtual COLORREF crShadow_nf             ( COLORREF* pValue = 0 ) = 0;  // [Basics], ȡ������ʱ�ı߿���ɫ

    virtual COLORREF crMenuBorder            ( COLORREF* pValue = 0 ) = 0;  // [Basics], �˵��߿���ɫ
    virtual COLORREF crMenuBack              ( COLORREF* pValue = 0 ) = 0;  // [Basics], �˵�������ɫ, �����˵�ʹ�õı���ɫ, �˵�������ʱʹ�õı���ɫ
    virtual COLORREF crMenuHot               ( COLORREF* pValue = 0 ) = 0;  // [Basics], �˵��ȵ�����ɫ
    virtual COLORREF crMenuBan               ( COLORREF* pValue = 0 ) = 0;  // [Basics], �˵���ֹ��ɫ
    virtual COLORREF crMenuText              ( COLORREF* pValue = 0 ) = 0;  // [Basics], �˵��ı���ɫ
    virtual COLORREF crMenuTextHot           ( COLORREF* pValue = 0 ) = 0;  // [Basics], �˵��ı��ȵ���ɫ
    virtual COLORREF crMenuTextBan           ( COLORREF* pValue = 0 ) = 0;  // [Basics], �˵��ı���ֹ��ɫ

    virtual COLORREF crMenuBarHot            ( COLORREF* pValue = 0 ) = 0;  // [Basics], �����˵����ȵ�����ɫ
    virtual COLORREF crMenuBarToolsText      ( COLORREF* pValue = 0 ) = 0;  // [Basics], �����˵������ֵ��ı���ɫ
    virtual COLORREF crMenuBarToolsTextHot   ( COLORREF* pValue = 0 ) = 0;  // [Basics], �����˵������ֵ��ı��ȵ���ɫ

    virtual COLORREF crTitleback             ( COLORREF* pValue = 0 ) = 0;  // [Basics], �м�ڿ����ı�����ɫ
    virtual COLORREF crTitle_left_act        ( COLORREF* pValue = 0 ) = 0;  // [Basics], �м�ڿ������ߵ���ɫ, �����Դ����������Ŀ��, ����ʱ���ı���ɫ
    virtual COLORREF crTitle_right_act       ( COLORREF* pValue = 0 ) = 0;  // [Basics], �м�ڿ�����ұߵ���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ, ����ʱ���ı���ɫ
    virtual COLORREF crTitle_left            ( COLORREF* pValue = 0 ) = 0;  // [Basics], �м�ڿ������ߵ���ɫ, �����Դ����������Ŀ��
    virtual COLORREF crTitle_right           ( COLORREF* pValue = 0 ) = 0;  // [Basics], �м�ڿ�����ұߵ���ɫ, ����ǳ�������, ����δ����ʱ���Ǻ�ʹ�õ���ɫ

    virtual COLORREF crText                  ( COLORREF* pValue = 0 ) = 0;  // [Basics], ��ͨ�ı��滭�õ���ɫ
    virtual COLORREF crTextStatusBar         ( COLORREF* pValue = 0 ) = 0;  // [Basics], �ײ�״̬�����ı���ɫ
    virtual COLORREF crTextStatusBar_run     ( COLORREF* pValue = 0 ) = 0;  // [Basics], �ײ�״̬������ʱ���ı���ɫ
    virtual COLORREF crTextStatusBar_nf      ( COLORREF* pValue = 0 ) = 0;  // [Basics], �ײ�״̬����������ı���ɫ

    virtual COLORREF crScrollBtn1            ( COLORREF* pValue = 0 ) = 0;  // [Basics], ��������ť1������ɫ
    virtual COLORREF crScrollBtnHot1         ( COLORREF* pValue = 0 ) = 0;  // [Basics], ��������ť1�ȵ���ɫ
    virtual COLORREF crScrollBtnDown1        ( COLORREF* pValue = 0 ) = 0;  // [Basics], ��������ť1������ɫ
    virtual COLORREF crScrollBtn2            ( COLORREF* pValue = 0 ) = 0;  // [Basics], ��������ť2������ɫ
    virtual COLORREF crScrollBtnHot2         ( COLORREF* pValue = 0 ) = 0;  // [Basics], ��������ť2�ȵ���ɫ
    virtual COLORREF crScrollBtnDown2        ( COLORREF* pValue = 0 ) = 0;  // [Basics], ��������ť2������ɫ
    virtual COLORREF crScrollThumb           ( COLORREF* pValue = 0 ) = 0;  // [Basics], ������������ɫ
    virtual COLORREF crScrollThumbHot        ( COLORREF* pValue = 0 ) = 0;  // [Basics], �����������ȵ���ɫ
    virtual COLORREF crScrollThumbDown       ( COLORREF* pValue = 0 ) = 0;  // [Basics], ���������鰴����ɫ
    virtual COLORREF crScrollBack            ( COLORREF* pValue = 0 ) = 0;  // [Basics], ������������ɫ

    virtual COLORREF crToolSeplineLeft       ( COLORREF* pValue = 0 ) = 0;  // [Basics], �������ָ��������ɫ
    virtual COLORREF crToolSeplineRight      ( COLORREF* pValue = 0 ) = 0;  // [Basics], �������ָ����ұ���ɫ

    virtual COLORREF crStatelineNormal       ( COLORREF* pValue = 0 ) = 0;  // [Basics], ���ڶ����м��ߵ�������ɫ
    virtual COLORREF crStatelineHot          ( COLORREF* pValue = 0 ) = 0;  // [Basics], ���ڶ����м��ߵ��ȵ���ɫ

    virtual LPCWSTR  ires_Toolbar            ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], ��������Դ��
    virtual LPCWSTR  ires_TreeFne            ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], ֧�ֿ����Ϳ���Դ��
    virtual LPCWSTR  ires_TreeProgram        ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], �������Ϳ���Դ��
    virtual LPCWSTR  ires_TreeControl        ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], ��������Ϳ���Դ��
    virtual LPCWSTR  ires_TreeExpand         ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], ���Ϳ�չ��������ť��Դ��
    virtual LPCWSTR  ires_ComboboxLocation   ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], ��Ͽ���Դ��
    virtual LPCWSTR  ires_MenuItem           ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], �˵���Դ��
    virtual LPCWSTR  ires_msctls_updown32_lr ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], ���������Ҽ�ͷ��Դ��
    virtual LPCWSTR  ires_msctls_updown32_tb ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], ���������¼�ͷ��Դ��
    virtual LPCWSTR  ires_TopState           ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], ����״̬��Դ��
    virtual LPCWSTR  ires_icon               ( LPCWSTR   pValue = 0 ) = 0;  // [iresFile], Ĭ����Դ��Դ��
    virtual LPCWSTR  clr                     ( LPCWSTR   pValue = 0 ) = 0;  // [clr], ��ɫ�ļ�


    virtual COLORREF close_hot                  ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ��йرհ�ť�ȵ���ɫ
    virtual COLORREF close_down                 ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ��йرհ�ť������ɫ
    virtual COLORREF crTab_text_window          ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    virtual COLORREF crTab_text_assembly        ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    virtual COLORREF crTab_text_assembly_window ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, ���ڳ����ı���ɫ
    virtual COLORREF crTab_text_class           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, ���ı���ɫ
    virtual COLORREF crTab_text_globalvar       ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����ı���ɫ
    virtual COLORREF crTab_text_struct          ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �Զ������������ı���ɫ
    virtual COLORREF crTab_text_dll             ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, DLL�ı���ɫ
    virtual COLORREF crTab_text_const           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    virtual COLORREF crTab_text_image           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, ͼƬ�ı���ɫ
    virtual COLORREF crTab_text_sound           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �����ı���ɫ
    virtual COLORREF crTab_line_window          ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    virtual COLORREF crTab_line_assembly        ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    virtual COLORREF crTab_line_assembly_window ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, ���ڳ����±�����ɫ
    virtual COLORREF crTab_line_class           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, ���±�����ɫ
    virtual COLORREF crTab_line_globalvar       ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, ȫ�ֱ����±�����ɫ
    virtual COLORREF crTab_line_struct          ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �Զ������������±�����ɫ
    virtual COLORREF crTab_line_dll             ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, DLL�±�����ɫ
    virtual COLORREF crTab_line_const           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ
    virtual COLORREF crTab_line_image           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, ͼƬ�±�����ɫ
    virtual COLORREF crTab_line_sound           ( COLORREF* pValue = 0  ) = 0;   // [CustomCodeTab], �����л�ѡ���, �����±�����ɫ

};


typedef bool (WINAPI* pfn_ShadowPaint)(LPCANVAS hCanvas, LPEX_IMAGE pImg, const RECT* prc, COLORREF crShadow, DWORD argbBorder, int alpha, LPVOID param);
class ICShadow
{

public:
    // ������Ӱ����, ���� rcWindow ����������Ӱ���ڵĴ�С��λ��, ��Ӱλ���� rcWindow.��߶���-10, �ұߵױ�+10
    // argbColorShadow = ��ӰRGB��ɫ, Ĭ���Ǻ�ɫ
    virtual HWND create(HINSTANCE hInst, const RECT& rcWindow, HWND hWnd, DWORD crShadow = 0) = 0;

    // ������Ӱ��ɫ, ��ɫֵ��ARGB
    virtual void SetColor(DWORD crShadow, DWORD argbBorder, bool isRedraw = false) = 0;

    // ������Ӱ����λ��, ���� rcWindow ����������Ӱ���ڵĴ�С��λ��, ��Ӱλ���� rcWindow.��߶���-10, �ұߵױ�+10
    // bForce = ǿ��ˢ��, Ϊ��ʱ���ڳߴ�δ�ı��򲻻��ػ�, Ϊtrueʱ������ػ�
    virtual void Update(const RECT& rcWindow, bool bForce = false) = 0;

    // �ҽӻ滭�ص�����, 0Ϊȡ���ص�
    virtual void OnPaint(pfn_ShadowPaint pfn, LPVOID param = 0) = 0;

    // ���ٵ�ǰ�����Ӱ����
    virtual void destroy() = 0;
    virtual bool isCreate() = 0;
    virtual void SetParent(HWND hWnd) = 0;
    virtual void hide() = 0;
    virtual void show() = 0;
    virtual void SetAlpha(int alpha) = 0;
    virtual int GetAlpha() = 0;
    virtual bool move(const RECT& rc) = 0;
    virtual HWND GetWindow() = 0;

};


struct CUSTOMTAB_DATA
{
    HWND hWnd;          // ѡ���Ӧ�Ĵ��ھ��
    int type;           // ��ǰѡ�������, CUSTOMTAB_INFO ���ö�ٳ���, ������10������
    LPWSTR text;        // ��ע�ı�, ��������ɾ��ʱ��Ҫ���� wstr::free() �ͷ�
    COLORREF crText;    // �ı���ɫ

    LPWSTR  name;       // ��ǰ�Ӽ�����, ɾ����Ա��Ҫ�ͷ�, ��Ҫͨ�� s_info->copyStrW() ������ռ����ַ���, s_info->freeStr() �ͷ�
    int     width;      // ��ǰ�Ӽп��
    LPARAM  lParam;     // ��Ŀ��ֵ
    int     iImage;     // ͼ������
    int     state;      // ��ǰ�Ӽ�״̬
    //bool    isClick;    // �Ƿ��Ѿ������, ����е����������Ϊtrue, û�еĻ���
};

// �Զ�������л�ѡ�����
class ICustomCodeTab
{
public:
    virtual int size() = 0;
    virtual bool empty() = 0;
    virtual int erase(int index) = 0;
    virtual CUSTOMTAB_DATA& at(int index) = 0;
    virtual CUSTOMTAB_DATA& operator[](int index) = 0;
    virtual int push_back(const CUSTOMTAB_DATA& data) = 0;
    virtual int insert(const CUSTOMTAB_DATA& data, int index) = 0;
    virtual int resize(int count) = 0;
    virtual void clear() = 0;
};

//class iDrawScalc
//{
//public:
//    // ���ݵ�ǰdpi������Ҫʹ�õ�����
//    virtual LPEX_FONT GetFont(LPEX_FONT hFont) = 0;
//    virtual void rescale_rect(RECT& rc) = 0;
//    virtual void _scale_rect(RECT& rc, UINT dpi) = 0;
//};