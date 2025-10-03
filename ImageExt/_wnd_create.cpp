#include "ImageExt.h"
LRESULT CALLBACK WndProc_static_group(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT rc;
        BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);
        FillRect(ps.hdc, &rc, GetStockBrush(WHITE_BRUSH));
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CTLCOLORSTATIC:
        return WndProc(hWnd, message, wParam, lParam);
    case WM_COMMAND:
        return _wndproc_command(oldProc, hWnd, message, wParam, lParam);
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}
void _wnd_create(HWND hWnd)
{
    s_ini.open(LR"(D:\cpp\SVN工作目录\易语言界面\ImageExt.ini)");
    DWORD styleEx = 0;
    DWORD style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER | WS_CLIPCHILDREN;
    DWORD styleBtn = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    DWORD styleStatic = WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | SS_CENTERIMAGE | SS_RIGHT;
    s_static_back_color.SubClass(WndProc_static_back_color);
    s_static_back_color.create(0, 0, styleBtn, 10, 100, 322, 194, hWnd);

    const int height = 24;
    s_static_draw.SubClass(WndProc_static_draw);
    s_static_save.SubClass(WndProc_static_save);
    s_static_group.SubClass(WndProc_static_group);
    g_hWnd = s_static_draw.create(0, 0, styleBtn, 0, 0, 0, 0, hWnd);
    s_static_save.create(0, 0, styleBtn, 0, 0, 0, 0, hWnd);
    s_static_group.create(0, 0, styleBtn | WS_BORDER, 0, 0, 0, 0, hWnd);
    _wnd_create_command(s_static_group);
    s_static_path.create(0, L"图片路径", styleStatic, 0, 0, 80, height, hWnd);
    s_loadFile = ini_read(L"s_editPath", 0);
    _load_image(s_loadFile);
    s_editPath.create(styleEx, s_loadFile.c_str(), style, 80, 0, 500, height, hWnd);
    s_btnLoad.create(styleEx, L"...", styleBtn, 580, 0, 30, height, hWnd, ID_BTN_LOAD);


    const int offset = 26;
    int top = offset;
    s_static_Color.create(0, L"预览背景", styleStatic, 0, top, 80, height, hWnd);
    s_editColor.create(styleEx, ini_read(L"s_editColor", wstr::to_string(RGB(255, 255, 255)).c_str()).c_str(), style, 80, top, 200, height, hWnd);

    top += offset;
    DWORD dwStyleCombobox = WS_TABSTOP | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD | CBS_DROPDOWN | CBS_AUTOHSCROLL | CBS_HASSTRINGS;
    s_static_ModifyColor1.create(0, L"要修改颜色", styleStatic, 0, top, 80, height, hWnd);
    s_editModifyColor1.create(styleEx, 0, dwStyleCombobox, 80, top, 200, 300, hWnd);
    ini_read(L"s_editModifyColor1", 0).c_str();
    
    top += offset;
    s_static_ModifyColor2.create(0, L"改成的颜色", styleStatic, 0, top, 80, height, hWnd);
    s_editModifyColor2.create(styleEx, ini_read(L"s_editModifyColor2", 0).c_str(), dwStyleCombobox, 80, top, 200, 300, hWnd);

    wstr name(50);
    for (int i = 0; i < 100; i++)
    {
        name.assign(L"s_editModifyColor1_").append(i);
        wstr str = ini_read(name.c_str(), 0);
        if (str.empty()) break;
        s_editModifyColor1.AddString(str.c_str());
    }
    for (int i = 0; i < 100; i++)
    {
        name.assign(L"s_editModifyColor2_").append(i);
        wstr str = ini_read(name.c_str(), 0);
        if (str.empty()) break;
        s_editModifyColor2.AddString(str.c_str());
    }
    s_editModifyColor1.SetSel(ini_read_int(L"s_editModifyColor1_index", 0));
    s_editModifyColor2.SetSel(ini_read_int(L"s_editModifyColor2_index", 0));

    top += offset;
    s_btnSave.create(styleEx, L"保存", styleBtn, 10, top, 80, height, hWnd, ID_BTN_SAVE);
    s_btnUpdate.create(styleEx, L"刷新", styleBtn, 95, top, 80, height, hWnd, ID_BTN_UPDATE);
    s_btnReload.create(styleEx, L"重新加载", styleBtn, 180, top, 80, height, hWnd, ID_BTN_RELOAD);

    top += offset;
    s_btnModify1.create(styleEx, L"混合修改", styleBtn, 10, top, 80, height, hWnd, ID_BTN_MODIFY1);
    s_btnModify2.create(styleEx, L"修改指定色", styleBtn, 95, top, 80, height, hWnd, ID_BTN_MODIFY2);
    s_btnModify3.create(styleEx, L"修改非透明", styleBtn, 180, top, 80, height, hWnd, ID_BTN_MODIFY3);

    top += offset;
    s_lastBtnBottom = top;
    s_list.create(0, 0, styleBtn | WS_BORDER, 0, 0, 0, 0, hWnd);

    s_list.SetNotifyCallback([](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT
                             {
                                 LPNMHDR hdr = (LPNMHDR)lParam;
                                 switch (hdr->code)
                                 {
                                 case NM_RCLICK:
                                 {
                                     auto pItem = (NMITEMACTIVATE*)lParam;
                                     CustomList_ItemRClick(hWnd, pItem->iItem);
                                     break;
                                 }
                                 case LVN_ITEMCHANGED:
                                 {
                                     auto pItem = (NMLISTVIEW*)lParam;
                                     CustomList_ItemSelect(hWnd, pItem->iItem);
                                     break;
                                 }
                                 case NM_CUSTOMDRAW:
                                 {
                                     auto lplvcd = (LPNMLVCUSTOMDRAW)lParam;
                                     switch (lplvcd->nmcd.dwDrawStage)
                                     {
                                     case CDDS_ITEMPREPAINT:
                                     {
                                         const bool isFocus = (lplvcd->nmcd.uItemState == CDIS_FOCUS);

                                         STATE state = (lplvcd->nmcd.uItemState == (LVIS_SELECTED | CDIS_FOCUS))
                                             ? STATE::SELECTED : (isFocus ? STATE::FOCUS
                                                                  : (lplvcd->nmcd.uItemState == CDIS_HOT ? STATE::HOT : STATE::NORMAL));
                                         CustomListBox_DrawItem(hWnd,
                                                                lplvcd->nmcd.hdc,
                                                                &lplvcd->nmcd.rc,
                                                                lplvcd->nmcd.dwItemSpec,
                                                                state, isFocus);
                                         return CDRF_NOTIFYITEMDRAW;
                                     }
                                     default:
                                         break;
                                     }
                                 }
                                 default:
                                     break;
                                 }
                                 return 0;
                             });

    //s_list.SetSelCallback(CustomList_ItemSelect);
    //s_list.SetDrawCallback(CustomListBox_DrawItem);
    //s_list.SetRClickCallback(CustomList_ItemRClick);
    if (!s_loadFile.empty())
    {
        wstr path = s_loadFile;
        wchar_t* pos = wcsrchr(path.data(), '\\');
        if (pos)
        {
            pos[1] = 0;
            enum_file(path.c_str());
        }
    }


    wstr rect = ini_read(L"windowPos", 0);
    if (!rect.empty())
    {
        RECT rc = { 0 };
        swscanf_s(rect.c_str(), L"%d,%d,%d,%d", &rc.left, &rc.top, &rc.right, &rc.bottom);
        MoveWindow(hWnd, rc.left, rc.top, rc.right, rc.bottom, true);
    }
}

