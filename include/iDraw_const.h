#pragma once

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

enum SCROLL_DRAWTYPE : int
{
    SCROLL_DRAWTYPE_BACK    = 0x00,     // �滭����, ������յ���dc���ڴ�dc
    SCROLL_DRAWTYPE_BTN1    = 0x01,     // �滭�������İ�ť1, ������յ���dc���ڴ�dc
    SCROLL_DRAWTYPE_BTN2    = 0x02,     // �滭�������İ�2, ������յ���dc���ڴ�dc
    SCROLL_DRAWTYPE_THUMB   = 0x03,     // �滭�������Ļ���, ������յ���dc���ڴ�dc
    SCROLL_DRAWTYPE_ALERT   = 0x05,     // �滭����, ������յ���HDC�Ǵ��ڵ�hdc, ���յ���λ���Ǽ�������п�����Ҫ�滭��λ��, ��������¼���ʱ��������Ѿ��滭�����dc����
};

//////////////////////////////////////////////////////////////////////////

struct IRE_STRUCT_MENU_CALCSIZE
{
    int width;      // �˵����
    int height;     // �˵��߶�
    LPARAM param;   // ��Ӳ˵�ʱ�Ĳ˵���ֵ, ����ʽ�˵����ֵΪ0
};


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
    RECT_F* prc;        // isDxΪtrue��ָ�� IDRAW_RECT_F �ṹָ��, ����ָ�� IDRAW_RECT �ṹָ��, ��Ҫ��ͼ��λ��д�������ַ
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
    HWND hWnd;      // �л���ѡ�����Ĵ��ھ��, ������/״̬����ʱû��ʹ�������Ա
    HWND hWndOld;   // �л�ǰѡ�����Ĵ��ھ��, ������/״̬����ʱû��ʹ�������Ա
    HWND hTab;      // �����¼���ѡ���
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
    HWND hWnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
}*LPIRE_STRUCT_MSG;

// ������״̬�ı��¼��ṹ
typedef struct IRE_STRUCT_TOOLSTATE_CHANGE
{
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

//////////////////////////////////////////////////////////////////////////

// ����˵������ڶ����Ĳ˵�����
typedef struct IDC_STRUCT_MENU_INSERT
{
    int id;
    LPCWSTR lpszMenuText;
    int index;
    HMENU hPopupMenu;
    bool isPopup;
}*LPIDC_STRUCT_MENU_INSERT;

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


#define HRESOURCE_MENU                      ((HWND)(10000 - 4))     // ��ȡ�˵���Դ���
#define HRESOURCE_TREEEXPAND                ((HWND)(10000 - 8))     // ��ȡ���Ϳ�Ӽ�����Դ���


// ֪ͨ��Ϣ, ��λ����, ʹ�õĶ��Ǹ�λ
#define INE_MENU_CALCMENU                   0x01000000  // ������չ�˵��ߴ�, wParam���ղ˵�ID, lParam���� IRE_STRUCT_MENU_CALCSIZE �ṹָ��, ͨ���޸Ľṹ��ֵ������˵��ߴ�, ���Է���ֵ
#define INE_MENU_DRAWMENU_BEFORE            0x01010000  // �˵������滭, wParam���ղ˵�ID, lParam���� IRE_STRUCT_MENU_DRAW�ṹָ��, �����Ƿ�����
#define INE_MENU_DRAWMENU_AFTER             0x01020000  // �˵��滭���, wParam���ղ˵�ID, lParam���� IRE_STRUCT_MENU_DRAW�ṹָ��, ���Է���ֵ
#define INE_MENU_GETICON                    0x01030000  // ��ȡ�˵�ͼ��, wParam���ղ˵�ID, lParam���� IRE_STRUCT_GETICON�ṹָ��, ����ͼƬָ��(LPWINDOW_ICON_DATA), ����з���ֵ, ���� prcû��ֵ����Է���ֵ

#define INE_CONFIG_CHANGED                  0x02000000  // ���ñ��ı�, wParam ����, lParam ����
#define INE_STATE_CLICK                     0x02010000  // �ڲ�ʹ��, ����״̬��������, wParam, ��λ=�����id, ��λ=��ť״̬, lParam = ������Ϣ�Ĵ��ھ��
#define INE_STATE_UPDATEHWND                0x02020000  // �ڲ�ʹ��, ����״̬����ǰ����Ĵ��ڱ��ı�, wParam = ֮ǰ����Ĵ��ھ��, lParam = �¼���Ĵ��ھ��

#define INE_TOOL_BUTTONCLICK                0x03000000  // ֪ͨ�ӿڹ������İ�ť������, wParam = id, lParam = ���밴ťʱ��д����Ŀ��ֵ


#define IGL_HWND                            0x0000      // ���ھ��, ֻ��
#define IGL_WNDPROC                         0x0001      // �������໯�ĺ�����ַ, iDraw_SubclassWindow() �����ĵڶ���������¼����
#define IGL_CALLBACK_NOTIFY                 0x0002      // ֪ͨ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_DRAWITEM               0x0003      // �滭��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_MEASUREITEM            0x0004      // ����ߴ���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_COMMAND                0x0005      // �¼���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_CTLCOLOR               0x0006      // ��ɫ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
#define IGL_CALLBACK_CALLWINDOWPROC         0x0007      // CallWindowProc ������ַ, ����ԭ����ʱʹ�������������, �ڵ������໯ʱ����ݴ����Ƿ�Ϊunicode���¼
#define IGL_INTERCEPT                       0x0008      // �Ƿ����ش���Ϣ, �ҽ���Ϣʱ�Ż�ʹ�����ֵ
#define IGL_OLDPROC                         0x0009      // ���ڵ�ԭ�ص�����, �Ѳ��������Ϣת���������ȥ����
#define IGL_CALLPROC_RETURN                 0x000A      // ֻ��, ����ԭ�ص������ķ���ֵ, ע�ᴦ�����Ϣ�¼��¿���ͨ���������ȡ������ֵ
#define IGL_CANVAS                          0x000B      // �봰�ڹ�����dx�滭���, ������ڲ���ʹ��dx��Ⱦ, ���ֵΪ0
#define IGL_USERDATA                        0x000C      // �봰�ڹ������û�����


#define IGL_DRAW_NONCPAINT                  0x0100      // ���ػ��ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCPAINT ��Ϣ
#define IGL_DRAW_NOCALCSIZE                 0x0101      // ������ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCCALCSIZE ��Ϣ, ���ڲ����б߿�
#define IGL_DRAW_ISTRANSPARENT              0x0102      // �Ƿ�͸��, ���Ϊtrue, �����õ� CTLCOLOR �л���� SetMode() ����Ϊ͸��
#define IGL_DRAW_ISHOT                      0x0103      // �Ƿ������ȵ�, һ�����б���ص����ʹ��
#define IGL_DRAW_ISCLIENT                   0x0104      // �Ƿ�����������Ϣ����Ϊ�ͻ���, Ϊtrue�򲻻��յ� WM_NCHITTEST��Ϣ, �ڲ�ȫ������ HTCLIENT
#define IGL_DRAW_ISERASEBKG                 0x0105      // �Ƿ����ػ�������Ϣ, ���Ϊtrue, �򴰿ڻ��յ� WM_ERASEBACKGROUND ��Ϣ, �����ڲ�����1
#define IGL_DRAW_STATE                      0x0106      // ��ǰ�����״̬
#define IGL_DRAW_FONT_DX                    0x0110      // ��ȡDX����, ֻ��
#define IGL_DRAW_FONT                       0x0111      // ��ȡGDI����, ֻ��
#define IGL_DRAW_TEXTFORMAT                 0x0112      // �滭�ı������ʽ

#define IGL_DRAW_CRBACKGROUND               0x0150      // ������ɫ, ÿ���������ʹ�������ɫ���滭����
#define IGL_DRAW_CRTEXT                     0x0151      // �ı���ɫ, ÿ������������ı���ɫ����ʹ�������ɫ�滭
#define IGL_DRAW_CRTEXTHOT                  0x0152      // �ı��ȵ���ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRTEXTDOWN                 0x0153      // �ı�������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRTEXTBAN                  0x0154      // �ı���ֹ��ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRNORMAL                   0x0155      // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRHOVER                    0x0156      // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRDOWN                     0x0157      // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRFOCUS                    0x0158      // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRCHECKED                  0x0159      // ���ѡ����ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRSELECTED                 0x015A      // ���ѡ����ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRHOT                      0x015B      // ����ȵ���ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRBAN                      0x015C      // �����ֹ��ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRVISTED                   0x015D      // 
#define IGL_DRAW_CRSHADOW                   0x015E      // ���������ɫ, �ĸ�����õ���ο�����
#define IGL_DRAW_CRBORDER                   0x015F      // ����߿���ɫ, ���л��Ʊ߿�ʹ�õ���ɫ

#define IGL_DRAW_HBRBACKGROUND              0x0160      // ����GDI��ˢ, ÿ���������ʹ�������ˢ����䱳��
#define IGL_DRAW_HBRTEXT                    0x0161      // �ı�GDI��ˢ
#define IGL_DRAW_HBRTEXTHOT                 0x0162      // �ı��ȵ�GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRTEXTDOWN                0x0163      // �ı�����GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRTEXTBAN                 0x0164      // �ı���ֹGDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRNORMAL                  0x0165      // �������GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRHOVER                   0x0166      // �������GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRDOWN                    0x0167      // �������GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRFOCUS                   0x0168      // �������GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRCHECKED                 0x0169      // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRSELECTED                0x016A      // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRHOT                     0x016B      // ����ȵ�GDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRBAN                     0x016C      // �����ֹGDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRVISTED                  0x016D      // 
#define IGL_DRAW_HBRSHADOW                  0x016E      // �����ӰGDI��ˢ, �ĸ�����õ���ο�����
#define IGL_DRAW_HBRBORDER                  0x016F      // ����߿�GDI����, ���л��Ʊ߿�ʹ�õĻ���

#define IGL_POS_RECTSCREEN                  0x0200      // ��������Ļ�е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ��
#define IGL_POS_RECTCLIENT                  0x0201      // �����ڸ������е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ��
#define IGL_POS_X                           0x0202      // �����ڸ������е����λ��, ֻ��
#define IGL_POS_Y                           0x0203      // �����ڸ������еĶ���λ��, ֻ��
#define IGL_POS_WIDTH                       0x0204      // ���ڵĿ�� ֻ��
#define IGL_POS_HEIGHT                      0x0205      // ���ڵĸ߶�, ֻ��




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


#define IRE_SIZE_TABWORK                    0x0001  // ��߹����гߴ类�ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SIZE �ṹָ��
#define IRE_SIZE_TABSTATE                   0x0002  // �±�״̬�гߴ类�ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SIZE �ṹָ��
#define IRE_SIZE_TABMDICLIENT               0x0003  // MDIClient�ߴ类�ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_MDICLIENT_MOVE_EVENT �ṹָ��
#define IRE_TAB_WORK_SIZE                   IRE_SIZE_TABWORK
#define IRE_TAB_STATE_SIZE                  IRE_SIZE_TABSTATE
#define IRE_TAB_MDICLIENT_SIZE              IRE_SIZE_TABMDICLIENT
//#define IRE_SCROLL_GETRECT                  0x0010  // ��ȡ������λ��, ����ͨ������¼��޸ı߿�ﵽָ���滭������λ��, �¼��ص������������յ� IRE_STRUCT_SCROLL_GETRECT �ṹָ��, ���¼����ᴥ�����ú�
//#define IRE_SCROLL_CALCRECT                 0x0011  // ���������λ��, ����ͨ������¼��޸Ĵ��ڷǿͻ���λ��, �¼��ص������������յ� IRE_STRUCT_SCROLL_CALCRECT �ṹָ��, ���¼����ᴥ�����ú� 
//#define IRE_SCROLL_DRAW                     0x0012  // �滭������, �¼��ص������������յ� IRE_STRUCT_SCROLL_DRAW �ṹָ��, ���¼����ᴥ�����ú�



//#define IRE_E_RUNSTATE_CHANGE               0x0010  // ����, ���ᴥ��, ���������а�ť״̬���ı�, �¼��ص��ĵ��������������Ƿ�����, 1=���а�ť������, 0=���а�ť�ѽ���
//#define IRE_E_STOPSTATE_CHANGE              0x0011  // ����, ���ᴥ��, ������ֹͣ��ť״̬���ı�, �¼��ص��ĵ��������������Ƿ�����, 1=ֹͣ��ť������, 0=ֹͣ��ť�ѽ���
//#define IRE_TOOLSTATE_CHANGE_RUN            IRE_E_RUNSTATE_CHANGE
//#define IRE_TOOLSTATE_CHANGE_STOP           IRE_E_STOPSTATE_CHANGE
#define IRE_TOOLSTATE_CHANGE                0x0012  // �����Ա�׼��������ť״̬���ı�, �¼��ص��ĵ������������� IRE_STRUCT_TOOLSTATE_CHANGE �ṹ


#define IRE_TABSELCHANGED_WORK              0x0020  // ��߹�����ѡ�񱻸ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SELCHANGE �ṹָ��, ����ǰ���صĻ�����ı��Ӽ�
#define IRE_TABSELCHANGED_STATE             0x0021  // �±�״̬��ѡ�񱻸ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SELCHANGE �ṹָ��, ����ǰ���صĻ�����ı��Ӽ�
#define IRE_TABSELCHANGED_CODE              0x0022  // �����л�ѡ���ѡ�񱻸ı�, �¼��ص��ĵ����������յ� IRE_STRUCT_TAB_SELCHANGE �ṹָ��, ����ǰ���صĻ����ᴥ���ߴ�ı��¼�
#define IRE_TAB_WORK_SELCHANGED             IRE_TABSELCHANGED_WORK
#define IRE_TAB_STATE_SELCHANGED            IRE_TABSELCHANGED_STATE
#define IRE_TAB_CODE_SELCHANGED             IRE_TABSELCHANGED_CODE

#define IRE_MDICLIENT_CREATE                0x0023  // MDIClient�µ�mid���ڱ������¼�, �¼��ص��ĵ����������յ� IRE_STRUCT_MDICREATE �ṹָ��, ����ǰ�����Ҳ���ȷ����Ļ�������ᱻ����, ����ǰ�ṹ�е� hWnd��ԱΪ0, ���ú��ԱΪ�´�����mdi���ھ��
#define IRE_MDICLIENT_DESTROY               0x0024  // MDIClient�µ�mid���ڱ������¼�, �¼��ص��ĵ����������յ� �����ٵ�MDI���ھ��
#define IRE_MDICLIENT_ACTIVATE              0x0025  // MDIClient�µ�mid���ڱ������¼�, �¼��ص��ĵ����������յ� Ҫ����� MDI �Ӵ��ڵľ��
#define IRE_ECODESELCHANGED                 0x0026  // �����Դ��봰�ڹ��ı��¼�, �¼��ص��ĵ����������յ� IRE_STRUCT_EIDE_SELCHANGEDINFO �ṹָ��, ���¼����ᴥ�����ú��¼�


#define IRE_DRAW_BCAKGROUND                 0x0030  // �������滭�¼�, ����ֵ��λΪ�Ƿ�����, ��λ1=gdi,0=dx, �¼��ص��ĵ����������յ� IRE_STRUCT_BACKGROUND_DRAW �ṹָ��
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

// ��ʱ�ڲ�ʹ��, ���ݲ���ʹ��
struct IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT
{
    HWND                hWnd;           // ���¼�/���ɾ��ϵͳ��ť�Ĵ��ھ��
    union
    {
        int             id;             // ���,ɾ��,���� ϵͳ��ť��ID
        LPVOID          pfn;            // �¼���ַ
    };
    int                 nPosition;      // ���밴ť��������ID
    LPARAM              param;          // ������ֵ
    int                 flags;          // ��ʶ

};
// ��ʱ�ڲ�ʹ��, ���ݲ���ʹ��
struct IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT
{
    HWND                hWnd;
    UINT                message;
    WPARAM              wParam;
    LPARAM              lParam;
    WNDPROC             oldProc;
    LRESULT*            pRet;
};
//+ ����֪ͨ�����ǹ�����
#define IDC_IDRAW_STR2RGB               (IDC_IDRAW_BEGIN + 0x0100)  // ��һ���ı���ɫת����RGB��ɫ, wParam = Ĭ����ɫ, ����ʧ�ܷ��������ɫ, lParamָ����ɫ�ı�ָ��
#define IDC_IDRAW_STR2ARGB              (IDC_IDRAW_BEGIN + 0x0101)  // ��һ���ı���ɫת����ARGB��ɫ, wParam = Ĭ����ɫ, ����ʧ�ܷ��������ɫ, lParamָ����ɫ�ı�ָ��
#define IDC_IDRAW_RGB2STR               (IDC_IDRAW_BEGIN + 0x0102)  // ��RGB��ɫת�����ı� RGB(1,2,3), wParam=Ҫת����rgb, lParam = ����20��WCHAR������ָ��, ����д���ַ���
#define IDC_IDRAW_ARGB2STR              (IDC_IDRAW_BEGIN + 0x0103)  // ��ARGB��ɫת�����ı� ARGB(255,1,2,3), wParam=Ҫת����argb, lParam = ����26��WCHAR������ָ��, ����д���ַ���
#define IDC_IDRAW_GETWINDOWDATA         (IDC_IDRAW_BEGIN + 0x0104)  // ��ȡ��������, wParam����, lParam = Ҫ��ȡ�Ĵ��ھ��, �ɹ���������ָ��, ʧ�ܷ���0
#define IDC_IDRAW_GETCODEVISIBLERECT    (IDC_IDRAW_BEGIN + 0x0105)  // ��ȡ���봰�ڿ��ӵľ���, wParam����, lParam = ���վ��ε�ָ��(����������������λ��)
#define IDC_IDRAW_SUBWINDOWEX           (IDC_IDRAW_BEGIN + 0x0106)  // ���໯����, ����������ػ����Ӿ��ⴰ��һ���ķ��, ֻ�޶��㴰��, wParam����, lParam = DRAW_WINDOW_ARGUMENT �ṹָ��, ���ش��ھ��
#define IDC_IDRAW_CREATEWINDOW          (IDC_IDRAW_BEGIN + 0x0107)  // ����һ�����Ӿ��ⴰ��һ�����Ķ��㴰��, wParam = CREATESTRUCTW �ṹָ��, lParam = DRAW_WINDOW_ARGUMENT �ṹָ��, ���ش��ھ��
#define IDC_IDRAW_SYSDRAW               (IDC_IDRAW_BEGIN + 0x0108)  // �����ػ��¼�, wParam����, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT �ṹָ��, �޷���ֵ
#define IDC_IDRAW_SYSBTNCLICK           (IDC_IDRAW_BEGIN + 0x0109)  // ϵͳ��ť����¼�, wParam����, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT �ṹָ��, �޷���ֵ
#define IDC_IDRAW_ADDSYSBUTTON          (IDC_IDRAW_BEGIN + 0x010A)  // ��Ӱ�ť, wParam����, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT �ṹָ��, �޷���ֵ
#define IDC_IDRAW_DELSYSBUTTON          (IDC_IDRAW_BEGIN + 0x010B)  // ɾ����ť, wParam����, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT �ṹָ��, �޷���ֵ
#define IDC_IDRAW_INSSYSBUTTON          (IDC_IDRAW_BEGIN + 0x010C)  // ���밴ť, wParam����, lParam = IDRAW_SYSBUTTON_ICON_TITLE_ARGUMENT_STRUCT �ṹָ��, �޷���ֵ
#define IDC_IDRAW_DRAW_WINDOWPROC       (IDC_IDRAW_BEGIN + 0x010D)  // ��Ϣ������, ����Ϣ���ݵ��滭ϵͳ��ť����Ϣ�����ﴦ��, wParam����, lParam = IDRAW_SYSBUTTON_ICON_TITLE_WINDOWPROC_STRUCT�ṹ��ַ
#define IDC_IDRAW_DRAW_DEFARG           (IDC_IDRAW_BEGIN + 0x010E)  // ��ȡĬ�ϲ���, wParam = ���ھ��, lParam = DRAW_WINDOW_ARGUMENT�ṹ��ַ, �����Ƿ�ɹ�
#define IDC_IDRAW_SETBTNPARAM           (IDC_IDRAW_BEGIN + 0x010F)  // ����ϵͳ��ť��ֵ, wParam����, lParam = DRAW_WINDOW_ARGUMENT�ṹ��ַ, �����Ƿ�ɹ�
#define IDC_IDRAW_GETBTNPARAM           (IDC_IDRAW_BEGIN + 0x0110)  // ��ȡϵͳ��ť��ֵ, wParam����, lParam = DRAW_WINDOW_ARGUMENT�ṹ��ַ, �����Ƿ�ɹ�
#define IDC_IDRAW_GETBTNPARAM           (IDC_IDRAW_BEGIN + 0x0110)  // ��ȡϵͳ��ť��ֵ, wParam����, lParam = DRAW_WINDOW_ARGUMENT�ṹ��ַ, �����Ƿ�ɹ�
#define IDC_IDRAW_GETBTNID              (IDC_IDRAW_BEGIN + 0x0111)  // ��ȡϵͳ��ťID, wParam����, lParam = DRAW_WINDOW_ARGUMENT�ṹ��ַ, �����Ƿ�ɹ�
#define IDC_IDRAW_GETBTNPOS             (IDC_IDRAW_BEGIN + 0x0112)  // ��ȡϵͳ��ť����, wParam����, lParam = DRAW_WINDOW_ARGUMENT�ṹ��ַ, �����Ƿ�ɹ�

#define IDC_IDRAW_INLINEHOOK            (IDC_IDRAW_BEGIN + 0x0200)  // ����inlineHook, wParam = ԭ����, lParam = �º���, ����ԭ������ַ, ����ֱ�ӵ���ԭ����
#define IDC_IDRAW_UNINLINEHOOK          (IDC_IDRAW_BEGIN + 0x0201)  // ȡ��inlineHook, wParam = ԭ����, lParam = ����, �����Ƿ�ɹ�
#define IDC_IDRAW_UNINLINEHOOK_PLUGIN   (IDC_IDRAW_BEGIN + 0x0202)  // ȡ�����ͷŲ����inlineHook, wParam = ģ����ʼ��ַ, lParam = ģ�������ַ
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




#define IDC_IDRAW_SET_ETOOLSFUN         (IDC_IDRAW_BEGIN + 0x0001)  // �������ֹ��ܺ���, wParam����, lParam �������ֵĹ��ܺ���, int __stdcall callback(int, int, int)
#define IDC_INLINEHOOK                  (IDC_IDRAW_BEGIN + 0x0002)  // ����hook, wParam=hook��ʽ, 0=detoursHook, 1=MinHook, lParam=[��hook��ַ, ���յ�ַ], ����ԭ��ַ
#define IDC_UNINLINEHOOK                (IDC_IDRAW_BEGIN + 0x0003)  // ȡ��hook, wParam=����, lParam=hookʱ�ķ���ֵ
#define IDC_GETSCROLLDATA               (IDC_IDRAW_BEGIN + 0x0004)  // ��ȡ����������, wParam = ����������ָ��, lParam = IDRAW_STRUCT_SCROLL_DATA �ṹָ��



#define BUILD_STATE_BEGIN               0   // ���뿪ʼ
#define BUILD_STATE_END                 1   // �������
#define BUILD_STATE_OK                  2   // ����ɹ�
#define BUILD_STATE_FAIL                3   // ����ʧ��

#define IDC_ICONTROL_MENU_BEGIN         (IDC_ICONTROL_BEGIN + 0x1000)       // �滭����˵�������ʼ
#define IDC_ICONTROL_MENU_END           (IDC_ICONTROL_BEGIN + 0x1FFF)       // �滭����˵�����������
#define IDC_ICONTROL_INSERTMENUBAR      (IDC_ICONTROL_MENU_BEGIN + 0x0001)  // ����˵������ڶ����Ĳ˵�����, wParam����, lParam ���� IDC_STRUCT_MENU_INSERT�ṹָ��
#define IDC_ICONTROL_REMOVEMENUBAR      (IDC_ICONTROL_MENU_BEGIN + 0x0002)  // �Ӷ����˵�����ɾ���˵�, wParam �ǲ˵�ID, lParam ��ʾ wParam�Ƿ�Ϊ����
#define IDC_ICONTROL_INSERT_MENUEXT     (IDC_ICONTROL_MENU_BEGIN + 0x0003)  // ����˵�����չ�˵���, wParam����, lParam ���� IDC_STRUCT_MENUEXT_INSERT�ṹָ��
#define IDC_ICONTROL_ADD_MENUBAR        (IDC_ICONTROL_MENU_BEGIN + 0x0004)  // ���ֵ���, wParam���ݳ�Ա��, lParam ������������, ��������Ҫ���붥���˵��ľ��, ֧�ֿ�ֻ������Щ�˵����붥��
#define IDC_ICONTROL_ISADDMENUBAR       (IDC_ICONTROL_MENU_BEGIN + 0x0005)  // ����ר�ù���, ���붥���˵�ʱ�ж��Ƿ��������, wParam = 0, lParam = Ҫ����Ĳ˵����, ��Ϊ���ּ��붥���Ķ��ǵ���ʽ�˵�, ���Զ��ǲ˵����
#define IDC_ICONTROL_BUILD              (IDC_ICONTROL_MENU_BEGIN + 0x0006)  // ����֪ͨ�Ӿ�����뿪ʼ, wParam = ����״̬, BUILD_STATE_ ��ͷ����, lParam����
#define IDC_ICONTROL_UNLOADLIB          (IDC_ICONTROL_MENU_BEGIN + 0x0007)  // ֪֧ͨ�ֿ�ж��֧�ֿ�, wParam = 1=����,0=ͣ��,-1=��ȡ�Ƿ���֧�ֿ�, lParam = ����

#define IDC_ICONTROL_TOOL_BEGIN         (IDC_ICONTROL_BEGIN + 0x2000)       // �滭����˵�������ʼ
#define IDC_ICONTROL_TOOL_END           (IDC_ICONTROL_BEGIN + 0x2FFF)       // �滭����˵�����������
#define IDC_ICONTROL_TOOL_INSERT        (IDC_ICONTROL_TOOL_BEGIN + 0x0008)  // ���ӿڹ����������һ����ť, wParam = ����, lParam = IDRAW_STRUCT_TOOL_INSERTָ��
#define IDC_ICONTROL_TOOL_DELETE        (IDC_ICONTROL_TOOL_BEGIN + 0x0009)  // �ӽӿڹ�������ɾ��һ����ť, wParam = ��ťid, lParam = LPPLUGIN_INFO ָ��
#define IDC_ICONTROL_TOOL_GETBBUTTON    (IDC_ICONTROL_TOOL_BEGIN + 0x000A)  // �ӽӿڹ������л�ȡһ����ť������, wParam = ��ťid, lParam = LPPLUGIN_INFO ָ��, ���� IDRAW_STRUCT_TOOL_INSERTEX �ṹָ��, ���ָ��ť������
#define IDC_ICONTROL_TOOL_SETBBUTTON    (IDC_ICONTROL_TOOL_BEGIN + 0x000B)  // �ӽӿڹ�����������һ����ť������, wParam = ��ťid, lParam = IDRAW_STRUCT_TOOL_INSERT ָ��, lParam�Ĳ��ָ��Ͱ�ťID�����¼, ���򷵻�0

#define IDC_IMOVE_GETWINDOWTYPE         (IDC_IMOVE_BEGIN + 0X0001)   // ��ȡ��ǰMDIClient ����������ʾʲô����, -1=û�д���, 0=������, 1=���������, 2=��ʼҳ, ��λ�Ǵ�������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ
#define IDC_IMOVE_GETCODETYPE           (IDC_IMOVE_BEGIN + 0X0002)   // ��ȡ��ǰMDIClient ����������ʾʲô��������, 0=δ֪, 1=���������, 2=��ͨ����, 3=���ڳ���, 4=��, 5=ȫ�ֱ���, 6=��������, 7=DLLĿ¼�����, 8=����, 9=ͼƬ��Դ, 10=������Դ, ����Ҫ���ݲ���























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

























// > `IGL_HWND`                           `0x0000`       // ���ھ��, ֻ��
// > `IGL_WNDPROC`                        `0x0001`       // �������໯�ĺ�����ַ, iDraw_SubclassWindow() �����ĵڶ���������¼����
// > `IGL_CALLBACK_NOTIFY`                `0x0002`       // ֪ͨ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_DRAWITEM`              `0x0003`       // �滭��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_MEASUREITEM`           `0x0004`       // ����ߴ���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_COMMAND`               `0x0005`       // �¼���Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_CTLCOLOR`              `0x0006`       // ��ɫ��Ϣ����, �ɸ����ڵ���, ����ԭ�� LRESULT __stdcall fun(HWND, UINT, WPARAM, LPARAM, OBJSTRUCT*);
// > `IGL_CALLBACK_CALLWINDOWPROC`        `0x0007`       // CallWindowProc ������ַ, ����ԭ����ʱʹ�������������, �ڵ������໯ʱ����ݴ����Ƿ�Ϊunicode���¼
// > `IGL_INTERCEPT`                      `0x0008`       // �Ƿ����ش���Ϣ, �ҽ���Ϣʱ�Ż�ʹ�����ֵ
// > `IGL_OLDPROC`                        `0x0009`       // ���ڵ�ԭ�ص�����, �Ѳ��������Ϣת���������ȥ����
// > `IGL_CALLPROC_RETURN`                `0x000A`       // ֻ��, ����ԭ�ص������ķ���ֵ, ע�ᴦ�����Ϣ�¼��¿���ͨ���������ȡ������ֵ
// > `IGL_DRAW_NONCPAINT`                 `0x0100`       // ���ػ��ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCPAINT ��Ϣ
// > `IGL_DRAW_NOCALCSIZE`                `0x0101`       // ������ǿͻ���, Ϊtrueʱ�������໯��Ϣ�ĺ��������յ� WM_NCCALCSIZE ��Ϣ, ���ڲ����б߿�
// > `IGL_DRAW_ISTRANSPARENT`             `0x0102`       // �Ƿ�͸��, ���Ϊtrue, �����õ� CTLCOLOR �л���� SetMode() ����Ϊ͸��
// > `IGL_DRAW_ISHOT`                     `0x0103`       // �Ƿ������ȵ�, һ�����б���ص����ʹ��
// > `IGL_DRAW_ISCLIENT`                  `0x0104`       // �Ƿ�����������Ϣ����Ϊ�ͻ���, Ϊtrue�򲻻��յ� WM_NCHITTEST��Ϣ, �ڲ�ȫ������ HTCLIENT
// > `IGL_DRAW_ISERASEBKG`                `0x0105`       // �Ƿ����ػ�������Ϣ, ���Ϊtrue, �򴰿ڻ��յ� WM_ERASEBACKGROUND ��Ϣ, �����ڲ�����1
// > `IGL_DRAW_STATE`                     `0x0106`       // ��ǰ�����״̬
// > `IGL_DRAW_FONT_DX`                   `0x0110`       // ��ȡDX����, ֻ��
// > `IGL_DRAW_FONT`                      `0x0111`       // ��ȡGDI����, ֻ��
// > `IGL_DRAW_TEXTFORMAT`                `0x0112`       // �滭�ı������ʽ
// > `IGL_DRAW_CRBACKGROUND`              `0x0150`       // ������ɫ, ÿ���������ʹ�������ɫ���滭����
// > `IGL_DRAW_CRTEXT`                    `0x0151`       // �ı���ɫ, ÿ������������ı���ɫ����ʹ�������ɫ�滭
// > `IGL_DRAW_CRTEXTHOT`                 `0x0152`       // �ı��ȵ���ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRTEXTDOWN`                `0x0153`       // �ı�������ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRTEXTBAN`                 `0x0154`       // �ı���ֹ��ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRNORMAL`                  `0x0155`       // ���������ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRHOVER`                   `0x0156`       // ���������ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRDOWN`                    `0x0157`       // ���������ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRFOCUS`                   `0x0158`       // ���������ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRCHECKED`                 `0x0159`       // ���ѡ����ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRSELECTED`                `0x015A`       // ���ѡ����ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRHOT`                     `0x015B`       // ����ȵ���ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRBAN`                     `0x015C`       // �����ֹ��ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRVISTED`                  `0x015D`       // 
// > `IGL_DRAW_CRSHADOW`                  `0x015E`       // ���������ɫ, �ĸ�����õ���ο�����
// > `IGL_DRAW_CRBORDER`                  `0x015F`       // ����߿���ɫ, ���л��Ʊ߿�ʹ�õ���ɫ
// > `IGL_DRAW_HBRBACKGROUND`             `0x0160`       // ����GDI��ˢ, ÿ���������ʹ�������ˢ����䱳��
// > `IGL_DRAW_HBRTEXT`                   `0x0161`       // �ı�GDI��ˢ
// > `IGL_DRAW_HBRTEXTHOT`                `0x0162`       // �ı��ȵ�GDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRTEXTDOWN`               `0x0163`       // �ı�����GDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRTEXTBAN`                `0x0164`       // �ı���ֹGDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRNORMAL`                 `0x0165`       // �������GDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRHOVER`                  `0x0166`       // �������GDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRDOWN`                   `0x0167`       // �������GDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRFOCUS`                  `0x0168`       // �������GDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRCHECKED`                `0x0169`       // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRSELECTED`               `0x016A`       // ���ѡ��GDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRHOT`                    `0x016B`       // ����ȵ�GDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRBAN`                    `0x016C`       // �����ֹGDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRVISTED`                 `0x016D`       // 
// > `IGL_DRAW_HBRSHADOW`                 `0x016E`       // �����ӰGDI��ˢ, �ĸ�����õ���ο�����
// > `IGL_DRAW_HBRBORDER`                 `0x016F`       // ����߿�GDI����, ���л��Ʊ߿�ʹ�õĻ���
// > `IGL_POS_RECTSCREEN`                 `0x0200`       // ��������Ļ�е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ��
// > `IGL_POS_RECTCLIENT`                 `0x0201`       // �����ڸ������е�λ��, ����RECT�ṹָ��, �������޸����ָ��, ֻ��
// > `IGL_POS_X`                          `0x0202`       // �����ڸ������е����λ��, ֻ��
// > `IGL_POS_Y`                          `0x0203`       // �����ڸ������еĶ���λ��, ֻ��
// > `IGL_POS_WIDTH`                      `0x0204`       // ���ڵĿ�� ֻ��
// > `IGL_POS_HEIGHT`                     `0x0205`       // ���ڵĸ߶�, ֻ��