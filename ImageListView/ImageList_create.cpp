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

    g_radioDPI[0].create(0, L"缩放100%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 8, 2, 100, 18, hWnd, ID_RADIO_100);
    g_radioDPI[1].create(0, L"缩放125%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 116, 2, 100, 18, hWnd, ID_RADIO_125);
    g_radioDPI[2].create(0, L"缩放150%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 224, 2, 100, 18, hWnd, ID_RADIO_150);
    g_radioDPI[3].create(0, L"缩放175%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 332, 2, 100, 18, hWnd, ID_RADIO_175);
    g_radioDPI[4].create(0, L"缩放200%", WS_VISIBLE | WS_CLIPSIBLINGS | BS_AUTORADIOBUTTON, 440, 2, 100, 18, hWnd, ID_RADIO_200);

    LPCWSTR szDpiIndex = s_xml_info.GetAttrValue(L"@1", XML_NAME_DPIINDEX);
    g_dpiIndex = wstr::stoi(szDpiIndex);
    if (g_dpiIndex < 0 || g_dpiIndex > 4)
        g_dpiIndex = 0;
    g_radioDPI[g_dpiIndex].SetCheck(true);

    s_listProject.SetBkColor(crBack);
    s_listItem.SetBkColor(crBack);
    s_listImage.SetBkColor(crBack);

    //TODO 忘了怎么挂接事件了, 代码迭代有问题....

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
    s_menuImage.add(ID_MENU_LOCATIONITEM1, L"打开目录(&O)");
    s_menuImage.add(ID_MENU_SELIMAGEPATH, L"选择图片素材目录");
    s_menuImage.add(ID_MENU_SAVE, L"&S.保存");
    s_menuImage.add((UINT_PTR)0, 0, MF_SEPARATOR);

    s_menuImage.add(ID_MENU_100_SETNORMAL1, L"设置正常状态图片");
    s_menuImage.add(ID_MENU_100_SETHOT1, L"设置热点状态图片");
    s_menuImage.add(ID_MENU_100_SETDOWN1, L"设置按下状态图片");
    s_menuImage.add(ID_MENU_100_SETBAN1, L"设置禁止状态图片");

    s_menuImage.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menuImage.add(ID_MENU_BLACK, L"黑色主题");
    s_menuImage.add(ID_MENU_WHITE, L"白色主题");

    s_menu.create(hWnd);
    s_menu.add(ID_MENU_LOCATIONITEM, L"打开目录(&O)");
    s_menu.add(ID_MENU_NEWITEM, L"添加项目");
    s_menu.add(ID_MENU_SAVE, L"&S.保存");
    s_menu.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menu.add(ID_MENU_NEWITEMS, L"批量添加项目");
    s_menu.add(ID_MENU_DELITEM, L"删除项目");
    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESET_NORMAL, L"正常状态");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESET_HOT,    L"热点状态");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESET_DOWN,   L"按下状态");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESET_BAN,    L"禁止状态");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESETITEM_DPI,L"重置当前选中DPI项目\t不删除id/备注/配色等");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RESETITEM,    L"整个项目\t包括id/备注/配色等");
    s_menu.add(hMenu, L"重置");

    hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RELOAD_NORMAL, L"正常状态");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RELOAD_HOT, L"热点状态");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RELOAD_DOWN, L"按下状态");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_RELOAD_BAN, L"禁止状态");
    s_menu.add(hMenu, L"重新从路径加载");

    hMenu = CreatePopupMenu();
    int menuid = ID_MENU_100_SETNORMAL;
    AppendMenuW(hMenu, MF_STRING, menuid + 0, L"正常状态");
    AppendMenuW(hMenu, MF_STRING, menuid + 1, L"热点状态");
    AppendMenuW(hMenu, MF_STRING, menuid + 2, L"按下状态");
    AppendMenuW(hMenu, MF_STRING, menuid + 3, L"禁止状态");


    s_menu.add(hMenu, L"设置状态图片");

    s_menu.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menu.add(ID_MENU_SETID, L"设置ID\t必须唯一");
    s_menu.add(ID_MENU_SETTEXT, L"设置替换文本\t把原文本替换显示成这个文本");
    s_menu.add(ID_MENU_SETREMARKS, L"设置说明");

    hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRTEXT1, L"正常颜色");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRTEXT2, L"热点颜色");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRTEXT3, L"按下颜色");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRTEXT4, L"禁止颜色");
    s_menu.add(hMenu, L"设置文本颜色");

    hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRBACK1, L"正常颜色");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRBACK2, L"热点颜色");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRBACK3, L"按下颜色");
    AppendMenuW(hMenu, MF_STRING, ID_MENU_SETCRBACK4, L"禁止颜色");
    s_menu.add(hMenu, L"设置背景颜色");
    //s_menu.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menu.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menu.add(ID_MENU_BLACK, L"黑色主题");
    s_menu.add(ID_MENU_WHITE, L"白色主题");


    s_menuFile.create(g_hWnd);
    s_menuFile.add(ID_MENU_LOCATIONPROJECT, L"打开目录(&O)");
    s_menuFile.add(ID_MENU_NEWTHEME, L"新建主题(&N)");
    s_menuFile.add(ID_MENU_SAVE, L"&S.保存");
    s_menuFile.add(ID_MENU_DELTHEME, L"删除主题(&D)");
    s_menuFile.add(ID_MENU_CLEAR_CACHE, L"清除缓存");
    //s_menuFile.add(ID_MENU_SETSIZE, L"设置尺寸(&S)");
    s_menuFile.add(ID_MENU_MAKETHEME, L"生成主题文件(&D)");
    s_menuFile.add((UINT_PTR)0, 0, MF_SEPARATOR);
    s_menuFile.add(ID_MENU_BLACK, L"黑色主题");
    s_menuFile.add(ID_MENU_WHITE, L"白色主题");





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