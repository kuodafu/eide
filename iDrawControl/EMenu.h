#pragma once
#include "EWindow_Fne_Header.h"

#define IDMENU_LOCATIONFILE     0x1A500     // ��λԴ���ļ�
#define IDMENU_COPYECODEFILE    0x1A501     // ����Դ��·��
#define IDMENU_LOCATIONTOOLBAR  0x1A502     // ��ʾ/���ض�λ������
#define IDMENU_UNINSERT         0x1A503     // ж���Ӿ���
#define IDMENU_AD               0x1A504     // ���

#define IDMENU_PATH_E           0x1A504     // ��������Ŀ¼
#define IDMENU_PATH_ELIB        0x1A505     // ��������֧�ֿ�Ŀ¼
#define IDMENU_PATH_IDRAW       0x1A506     // ���Ӿ���Ŀ¼
#define IDMENU_PATH_IDRAW_DATA  0x1A507     // ���Ӿ�������Ŀ¼
#define IDMENU_PATH_CONFIG      0x1A508     // ���Ӿ��������ļ�Ŀ¼
#define IDMENU_PATH_THEME       0x1A509     // ���Ӿ��⵱ǰ��ɫ�ļ�·��
#define IDMENU_PATH_PLUGIN      0x1A50A     // ���Ӿ�����Ŀ¼
#define IDMENU_PATH_ET_PLUGIN   0x1A50B     // �����ֲ��Ŀ¼

#define IDMENU_LOADDBGPLUGIN    0x1A510     // ���ص��Բ��
#define IDMENU_FREEDBGPLUGIN    0x1A511     // ж�ص��Բ��

#define IDMENU_HISTORYCODE_START 0x1AA00    // ��ʷ�򿪵�Դ��, ��ʼ����
#define IDMENU_HISTORYCODE_END  0x1AAFF     // ��ʷ�򿪵�Դ���������, ���ֻ֧����ʾ��ô��Դ��
#define IDMENU_MODULE_START     0x1AB00     // ����ģ��, ��ʼ����
#define IDMENU_MODULE_END       0x1ABFF     // ����ģ���������, ���ֻ֧����ʾ��ô��ģ��
#define IDMENU_SOURCECODE_START 0x1AC00     // ����Դ��, ��ʼ����
#define IDMENU_SOURCECODE_END   0x1ACFF     // ����Դ���������, ���ֻ֧����ʾ��ô��Դ��

#define IDMENU_SWITCH_THEME     0xCA5B0     // �л�����Ĳ˵�ID
#define IDMENU_SWITCH_THEME_END 0xCA6FF     // �л�����Ĳ˵����ID


#define IDMENU_RUNGUID          0x1A600     // ����guid
#define IDMENU_RUNSPY__         0x1A601     // ����spy++
#define IDMENU_RUNERROR         0x1A602     // ���д�����ҳ���

#define IDMENU_USER_START       0x1A800     // �û���ʼ�Ĳ˵�ID

#define ID_MENU_CLOSE           0x2DA49 // �ر�
#define ID_MENU_CLOSEOTHER      0x2DA4A // �ر�����
#define ID_MENU_CLOSEALL        0x2DA4B // �ر�ȫ��
#define ID_MENU_CLOSE_LEFT      0x2DA4C // �ر����
#define ID_MENU_CLOSE_RIGHT     0x2DA4D // �ر��ұ�
#define ID_MENU_CLOSE_TOP       0x2DA4E // ������ʾ
#define ID_MENU_CLOSE_BOTTOM    0x2DA4F // �ײ���ʾ
#define ID_MENU_CLOSE_SHOWCLOSE 0x2DA50 // ��ʾ�رհ�ť




struct DRAWITEM_MENU_INFO
{
    HWND    hWnd;
    HMENU   hMenu;
    HDC     hdc;
    int     cxClient;
    int     cyClient;
    RECT    rc;
    bool    isTopTriangle;  // �Ƿ���Ҫ���ζ���������, �˵������ܸ߶ȴ�����Ļ�߶�-�������߶Ⱦ���Ҫ�ı�
};

struct MENU_ID_DATA
{
    UINT id;
    HMENU hMenu;        // ����˵���Ŀ���ڵĲ˵����
    int width;
    int height;
    wchar_t text[100];  // �˵������ı�
    UINT flags;         // �˵���ʶ
    UINT     fType;         // used if MIIM_TYPE (4.0) or MIIM_FTYPE (>4.0)
    UINT     fState;        // used if MIIM_STATE
    HMENU    hSubMenu;      // used if MIIM_SUBMENU
    HBITMAP  hbmpChecked;   // used if MIIM_CHECKMARKS
    HBITMAP  hbmpUnchecked; // used if MIIM_CHECKMARKS
    ULONG_PTR dwItemData;   // used if MIIM_DATA
    HBITMAP  hbmpItem;      // used if MIIM_BITMAP
};

//struct EWIDNOW_MENU_DRAWINFO
//{
//    LPBRUSHPEN hPenBorder;
//
//    LPBRUSHPEN menu_down;           // ���Ͻǲ˵��еİ��»�ˢ
//    LPBRUSHPEN menu_hot;            // ���Ͻǲ˵��е��ȵ㻭ˢ
//
//    LPBRUSHPEN hbr_StatusBar;       // �ײ�״̬����������ɫ, ��ɫ����������ɫ
//    LPBRUSHPEN hbr_StatusBar_run;   // �ײ�״̬����������ɫ, ��ɫ�������ǻ�ɫ
//
//
//
//    LPBRUSHPEN hbrText;             // �����Դ��ڻ滭�ı�ʱĬ��ʹ�õ���ɫ
//    LPBRUSHPEN hbrText_zhushou;     // ���ֲ˵���ʾ����ɫ
//    
//    LPBRUSHPEN hbrTitleBack;        // �м�滭����ı�����ˢ
//    LPBRUSHPEN hbrTitleText1;       // �м�滭������ı���ˢ, �н���ʱ�Ļ�ˢ
//    LPBRUSHPEN hbrTitleText2;       // �м�滭������ı���ˢ, û����ʱ�Ļ�ˢ
//    LPBRUSHPEN hbrTitleText3;       // �м�滭������ı���ˢ, ��������
//    LPEX_FONT  hFontTitle;          // �м�滭����ʹ�õ�����
//    LPEX_FONT  hFontTitle2;         // �м�滭����ʹ�õ�����
//
//};

struct MENU_COPY_STRUCT
{
    struct DEL_MENU
    {
        HMENU hMenu;
        UINT  hDel;
    };
    std::vector<DEL_MENU> arr_delete;  // ���ٲ˵�ǰ��Ҫɾ���Ĳ˵���
    HMENU   hMenu;
    BOOL    isDestroy;
    MENU_COPY_STRUCT() : hMenu(0), isDestroy(0)
    {

    }
    ~MENU_COPY_STRUCT()
    {
        BOOL isOk = false;
        for ( DEL_MENU& item : arr_delete )
        {
            isOk = RemoveMenu(item.hMenu, item.hDel, MF_BYCOMMAND);
        }
        if ( isDestroy )
            DestroyMenu(hMenu);

    }
    inline operator HMENU()
    {
        return hMenu;
    }
};
typedef BOOL(WINAPI* FN_TrackPopupMenuEx)(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm);
FN_TrackPopupMenuEx GetTrackPopupMenuEx();
MENU_COPY_STRUCT _menu_copy_menu(HMENU hMenu, HWND hWnd);


// ��ʼ���˵�, ö�ٲ����������Բ˵�, �½��˵�����ڱ���λ��
LPOBJSTRUCT _init_menu_(LPOBJSTRUCT pWindowData);
int __menu_calc_rect(HDC hdc, EWINDOW_MENUSTRUCT& data, bool first);

void modify_menu_style(HMENU hMenu, LPEMENU_DATA pMenu, int count);
void _apihook_menu_init(HINSTANCE hInst);
// ��ʱ��ʼ��hook�˵���, ����/���ý�ֹ�˵�, ��ЩAPI���ǵȴ�����ʾ����hook
void _apihook_menu_init_sleep();
// ��һ���˵������ӵ��ػ��б���, ���û�м��뵽����б���, �򲻻��ػ�����˵������
void EMenu_InsertMenu(HMENU hMenu);
// �Ƿ����໯�˵�����, 0=����Ҫ, 1=��Ҫ���໯, 2=������LoadMenu
void EMenu_IsSubClass(int code);
// �Ƿ����໯�˵�����, 0=����Ҫ, 1=��Ҫ���໯, 2=������LoadMenu
int EMenu_GetIsSubClass();
void EMenu_Get_Help_Wnd(HMENU& hMenuWnd, HMENU& hMenuHelp);
MENU_ID_DATA* EMenu_GetMenuItemInfoFromID(UINT id);

LRESULT CALLBACK WndProc_Custom_Menu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

// �����˵�, ����ڲ��ᴦ���Ի�����
HMENU _menu_create();
// ��Ӳ˵���, �ڲ��ᴦ���Ի���Ϣ
bool _menu_add(HMENU hMenu, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, UINT uFlags = MF_STRING);
bool _menu_insert(HMENU hMenu, UINT uPosition, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, UINT uFlags = MF_STRING);
// �����˵�
bool _menu_pop(HWND hWnd, HMENU hMenu, int x = 0, int y = 0);

EWINDOW_MENUSTRUCT* _menu_getindex();

// ���¼������λ��, ����λ���Ǹ��ݲ˵�λ�ú�ϵͳ��ť�����, �����ڲ������ȡ����
void _window_recalc_title(LPOBJSTRUCT pData, bool isDrawMenu);

// ����guid���������뵽�ĸ��˵�����
bool MenuExt_Insert(LPCSTR guid, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPARAM param);
bool MenuExt_Delete(LPCSTR guid);
UINT_PTR MenuExt_Find(LPPLUGIN_INFO info, int id);
HWND  GetHwndFromMenu(HMENU hMenu);
HMENU GetMenuFromHwnd(HWND hWnd);
// ��ȡ��ǰ�����˵��Ĵ��ھ��
HWND GetCurrentMenuHwnd();

DRAWITEM_MENU_INFO* GetDrawItemInfo(HDC hdc);

// ��������Դ��ڵ��Ҽ��˵�
// �����ط��Ҽ��˵�, ���ֶ��빤����, ���߲˵�  �鿴->������, ��Ҫɾ������Ӱ�첼�ֵĲ˵���
void EMenu_InsertRButton(HMENU hMenuParent);

typedef int(__cdecl* PFN_LoadStringCallback)(int id, LPWSTR lpBuffer, int cchBufferMax);
typedef int(__cdecl* PFN_EMenuCommand)(int id);

// ��ʼ��hook�����ַ����ĺ���
void HookLoadStringInit();
// ����һ����������ַ����Ļص�, �ص����������, �뷵��0
void HookLoadString_PushCallback(PFN_LoadStringCallback pfn);

// �󶨲˵�������¼�, �ص������Ƿ��Ѵ���, ����ص����Ѿ�����, �򲻼������·ַ��¼�
void EMenu_PushCommand(PFN_EMenuCommand pfn);
void EMenu_CustomMenu(EWINDOW_MENUSTRUCT* menu);
// �˵����˵���Ŀ���ı�
void EMenu_MenuBarChange();
// ��ȡ�����ļ�������ز˵��б�, str�Ǵ�Ż�ȡ���Ĳ˵��б��ַ���, arr���ÿ���˵�����ʼ��ַ
int CustomMenu_GetMenuList(_str& str, std::vector<LPCWSTR>& arr);
// ��ָ���˵����Ӳ˵��б���ɾ��, ɾ����˵�����ʾ�ڲ˵�����, ����0��ʾ�ɹ�, 1��ʾ�˵���������
bool CustomMenu_RemoveMenuList(LPCWSTR menu_name);
// ��ָ���˵������뵽�˵��б���, ���������˵����Ͳ���ʾ�ڲ˵�����, ����0��ʾ�ɹ�, 1��ʾ�˵����Ѵ���
bool CustomMenu_InsertMenuList(LPCWSTR menu_name);


// ��ȡ�ļ�����, �����ļ��ߴ�, ����0��ʾ��ȡʧ��, �����ļ�û������
inline int _get_file_data(LPCWSTR file, _str& ret)
{
    CFileRW f;
    if (!f.open(file))
        return 0;
    int size = f.size();
    if (size <= 0)
        return 0;
    ret.resize(size);
    f.read((LPBYTE)ret.data(), size);
    return size;
}
