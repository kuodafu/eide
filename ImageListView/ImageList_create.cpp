#include "ImageListView.h"

inline HWND CreateControl(DWORD styleEx, LPCWSTR className, LPCWSTR name, DWORD style, int x, int y, int cx, int cy, HWND hParent, int id = 0)
{
    HWND hWnd = CreateWindowExW(styleEx, className, name, style, x, y, cx, cy, hParent, (HMENU)(LONG_PTR)id, g_hInst, 0);
    SendMessageW(hWnd, WM_SETFONT, (WPARAM)CWndBase::GetDefaultFont(), 0);
    return hWnd;
}
inline HWND CreateStatic(HWND hParent, LPCWSTR name, int x, int y, int cx, int cy, int id = 0)
{
    DWORD style = WS_CHILD | WS_CLIPSIBLINGS | SS_CENTERIMAGE;
    return CreateControl(0, L"static", name, style, x, y, cx, cy, hParent, id);
}
inline HWND CreateEdit(HWND hParent, int x, int y, int cx, int cy, int id = 0)
{
    DWORD style = WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS | ES_AUTOHSCROLL;
    DWORD styleEx = WS_EX_CLIENTEDGE;
    return CreateControl(styleEx, L"edit", L"", style, x, y, cx, cy, hParent, id);
}
inline HWND CreateButton(HWND hParent, LPCWSTR name, int x, int y, int cx, int cy, int id = 0, DWORD dwStyle = 0)
{
    dwStyle |= (WS_TABSTOP | WS_CHILD | WS_CLIPSIBLINGS | BS_PUSHBUTTON);
    return CreateControl(0, L"button", name, dwStyle, x, y, cx, cy, hParent, id);
}



bool CALLBACK WndProc_create(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lpResult)
{
    const COLORREF crBack = g_color.crBack;
    g_hWnd = hWnd;
    DWORD styleEx = WS_EX_CONTROLPARENT;
    DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER;

    s_listProject.create(styleEx, 0, style, 0, 0, 0, 0, hWnd, 0, 0);
    s_listItem.create(styleEx, 0, style, 0, 0, 0, 0, hWnd, 0, 0);
    s_listImage.create(styleEx, 0, style, 0, 0, 0, 0, hWnd, 0, 0);

    g_radioDPI[0].create(0, L"����100%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 8, 2, 100, 18, hWnd, ID_RADIO_100);
    g_radioDPI[1].create(0, L"����125%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 116, 2, 100, 18, hWnd, ID_RADIO_125);
    g_radioDPI[2].create(0, L"����150%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 224, 2, 100, 18, hWnd, ID_RADIO_150);
    g_radioDPI[3].create(0, L"����175%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 332, 2, 100, 18, hWnd, ID_RADIO_175);
    g_radioDPI[4].create(0, L"����200%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 440, 2, 100, 18, hWnd, ID_RADIO_200);

    LPCWSTR szDpiIndex = s_xml_info.GetAttrValue(L"@1", XML_NAME_DPIINDEX);
    g_dpiIndex = wstr::stoi(szDpiIndex);
    if (g_dpiIndex < 0 || g_dpiIndex > 4)
        g_dpiIndex = 0;
    g_radioDPI[g_dpiIndex].SetCheck(true);

    s_listProject.SetBkColor(crBack);
    s_listItem.SetBkColor(crBack);
    s_listImage.SetBkColor(crBack);

    //TODO ������ô�ҽ��¼���, �������������....

    //s_listProject.SetSelCallback(CustomList_ItemSelect);
    //s_listProject.SetDrawCallback(CustomListBox_DrawItem);
    //s_listProject.SetRClickCallback(CustomList_ItemRClick);

    //s_listItem.SetSelCallback(CustomList_ItemSelect);
    //s_listItem.SetDrawCallback(CustomListBoxItem_DrawItem);
    //s_listItem.SetRClickCallback(CustomList_ItemRClick);
    //s_listItem.SetClickCallback(CustomList_ItemClick);

    ////s_listImage.SetSelCallback(CustomList_ItemSelect);
    //s_listImage.SetDrawCallback(CustomListBoxImage_DrawItem);
    //s_listImage.SetRClickCallback(CustomList_ItemRClick);
    //s_listImage.SetClickCallback(CustomList_ItemClick);



    s_menuImage.create(hWnd);
    s_menuImage.add(ID_MENU_LOCATIONITEM1, L"��Ŀ¼(&O)");
    s_menuImage.add(ID_MENU_SELIMAGEPATH, L"ѡ��ͼƬ�ز�Ŀ¼");
    s_menuImage.add(ID_MENU_SAVE, L"&S.����");
    s_menuImage.add((UINT_PTR)0, 0, MF_SEPARATOR);

    s_menuImage.add(ID_MENU_100_SETNORMAL1, L"��������״̬ͼƬ");
    s_menuImage.add(ID_MENU_100_SETHOT1, L"�����ȵ�״̬ͼƬ");
    s_menuImage.add(ID_MENU_100_SETDOWN1, L"���ð���״̬ͼƬ");
    s_menuImage.add(ID_MENU_100_SETBAN1, L"���ý�ֹ״̬ͼƬ");

    s_menuImage.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menuImage.add(ID_MENU_BLACK, L"��ɫ����");
    s_menuImage.add(ID_MENU_WHITE, L"��ɫ����");

    s_menu.create(hWnd);
    s_menu.add(ID_MENU_LOCATIONITEM, L"��Ŀ¼(&O)");
    s_menu.add(ID_MENU_NEWITEM, L"�����Ŀ");
    s_menu.add(ID_MENU_SAVE, L"&S.����");
    s_menu.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menu.add(ID_MENU_NEWITEMS, L"���������Ŀ");
    s_menu.add(ID_MENU_DELITEM, L"ɾ����Ŀ");
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESET_NORMAL, L"����״̬");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESET_HOT,    L"�ȵ�״̬");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESET_DOWN,   L"����״̬");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESET_BAN,    L"��ֹ״̬");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESETITEM_DPI,L"���õ�ǰѡ��DPI��Ŀ\t��ɾ��id/��ע/��ɫ��");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESETITEM,    L"������Ŀ\t����id/��ע/��ɫ��");
    s_menu.add(hMenu, L"����");

    hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RELOAD_NORMAL, L"����״̬");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RELOAD_HOT, L"�ȵ�״̬");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RELOAD_DOWN, L"����״̬");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RELOAD_BAN, L"��ֹ״̬");
    s_menu.add(hMenu, L"���´�·������");

    hMenu = CreatePopupMenu();
    int menuid = ID_MENU_100_SETNORMAL;
    AppendMenuW(hMenu, MF_STRING, menuid + 0, L"����״̬");
    AppendMenuW(hMenu, MF_STRING, menuid + 1, L"�ȵ�״̬");
    AppendMenuW(hMenu, MF_STRING, menuid + 2, L"����״̬");
    AppendMenuW(hMenu, MF_STRING, menuid + 3, L"��ֹ״̬");


    s_menu.add(hMenu, L"����״̬ͼƬ");

    s_menu.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menu.add(ID_MENU_SETID, L"����ID\t����Ψһ");
    s_menu.add(ID_MENU_SETTEXT, L"�����滻�ı�\t��ԭ�ı��滻��ʾ������ı�");
    s_menu.add(ID_MENU_SETREMARKS, L"����˵��");

    hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRTEXT1, L"������ɫ");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRTEXT2, L"�ȵ���ɫ");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRTEXT3, L"������ɫ");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRTEXT4, L"��ֹ��ɫ");
    s_menu.add(hMenu, L"�����ı���ɫ");

    hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRBACK1, L"������ɫ");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRBACK2, L"�ȵ���ɫ");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRBACK3, L"������ɫ");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRBACK4, L"��ֹ��ɫ");
    s_menu.add(hMenu, L"���ñ�����ɫ");
    //s_menu.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menu.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menu.add(ID_MENU_BLACK, L"��ɫ����");
    s_menu.add(ID_MENU_WHITE, L"��ɫ����");


    s_menuFile.create(g_hWnd);
    s_menuFile.add(ID_MENU_LOCATIONPROJECT, L"��Ŀ¼(&O)");
    s_menuFile.add(ID_MENU_NEWTHEME, L"�½�����(&N)");
    s_menuFile.add(ID_MENU_SAVE, L"&S.����");
    s_menuFile.add(ID_MENU_DELTHEME, L"ɾ������(&D)");
    s_menuFile.add(ID_MENU_CLEAR_CACHE, L"�������");
    //s_menuFile.add(ID_MENU_SETSIZE, L"���óߴ�(&S)");
    s_menuFile.add(ID_MENU_MAKETHEME, L"���������ļ�(&D)");
    s_menuFile.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menuFile.add(ID_MENU_BLACK, L"��ɫ����");
    s_menuFile.add(ID_MENU_WHITE, L"��ɫ����");





    init_project();
    init_Image(s_xml_info.GetAttrValue(L"@1", XML_INI_IMAGEPATH));
    if (s_data.size())
    {
        s_listProject.SetIndex(0, true);
        CustomList_ItemSelect(s_listProject, 0);
    }
    //s_list.SetDrawCallback(CustomListBox_DrawItem);
    return true;
}