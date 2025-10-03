// 自绘选择夹, 
#include "top_state.h"
#include "pch.h"

static WNDPROC m_pfnTabWndProc;     // 选择夹的消息处理函数
LPIDRAW_INFO s_info;
#define __query(_a, _b) (((_a) & (_b)) == (_b))
LRESULT CALLBACK WndProc_Group_Cell(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData);

enum TAB_SEL_MODE : int
{
    TAB_SEL_MODE_CODE       = 0,    // 代码选中
    TAB_SEL_MODE_LBUTTON    = 1,    // 鼠标左键点击选中
    TAB_SEL_MODE_RBUTTON    = 2,    // 鼠标右键点击选中
    TAB_SEL_MODE_MBUTTON    = 3,    // 鼠标中键点击选中
};
// 选择夹选中被改变, 返回改变后的索引, 触发这个事件的时候选中还没有被改变, 不让改变的就返回 indexOld
// pData = 选择夹窗口数据
// index = 新的被选中的索引
// indexOld = 改变前选中的索引
// selMode = 选中模式, 是鼠标点击选中还是代码选中, TAB_SEL_MODE 枚举常量
int _tab_sel_change(LPTAB_INFO_TOPSTATE tabInfo, int index, int indexOld, int selMode);

LPOBJSTRUCT _subclass_top_state_tab(HWND hWnd, int subHeight)
{
    static std::map<HWND, OBJSTRUCT> map;
    
    OBJSTRUCT& data = map[hWnd];;
    static LPEX_FONT hFont;
    if (!data.hWnd)
    {
        data.hWnd = hWnd;
        data.oldProc = DefWindowProcW;
        hFont = _font_create(L"微软雅黑", -12);
        data.draw.hFont = hFont;
    }
    return &data;
}

inline int _tab_calctext(LPTAB_INFO_TOPSTATE tabInfo, LPCWSTR text)
{
    LPOBJSTRUCT pData = tabInfo->pData;
    const float width = 1000;
    const float height = 50;
    float textWidth, textHeight;
    _canvas_calctextsize(pData->draw.hCanvas, pData->draw.hFont, text, -1, 0, width, height, &textWidth, &textHeight);
    int ret = ((int)textWidth + 10);
    ret += LOWORD(tabInfo->padding);
    return ret;
}
static std::map<HWND, OBJSTRUCT> map;
LPOBJSTRUCT GetDataFromHwnd(HWND hWnd)
{
    auto it = map.find(hWnd);
    if (it == map.end())
        return 0;
    return &(it->second);
}
inline LRESULT CALLBACK __def_group_cell_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    OBJSTRUCT& data = map[hWnd];;
    static LPEX_FONT hFont;
    if (!data.hWnd)
    {
        data.hWnd = hWnd;
        data.oldProc = DefWindowProcW;
        hFont = _font_create(L"微软雅黑", -12);
        data.draw.hFont = hFont;
    }
    return WndProc_Group_Cell(hWnd, message, wParam, lParam, &data);
}

inline void InvalidItem(LPTAB_INFO_TOPSTATE tabInfo, int top = 0)
{
    LPOBJSTRUCT pData = tabInfo->pData;
    if (top == 0)
        top = Tab_CalcTop(pData);
    top -= 1;
    RECT rcItem = { 0, top, (LONG)pData->pos.width, (LONG)pData->pos.height };
    InvalidateRect(pData->hWnd, &rcItem, 0);
}
// index = 要获取矩形的子夹
inline bool _tab_get_item_rect(LPTAB_INFO_TOPSTATE tabInfo, DWORD index, RECT* prc)
{
    RECT rc;
    int i = 0;
    TAB_ITEMINFO_ARR& arr = *tabInfo->arr;
    size_t size = arr.size();
    if (!prc || index >= size) return false;
    rc.left = 0;
    rc.right = 0;
    rc.top = Tab_CalcTop(tabInfo->pData);
    rc.bottom = tabInfo->pData->pos.height;
    for (auto it = arr.begin(); it != arr.end(); ++it, ++i)
    {
        rc.left = rc.right;
        rc.right = rc.left + it->width;
        if (i == index)
        {
            memcpy(prc, &rc, sizeof(RECT));
            return true;
        }
    }
    return false;
}

// 测试鼠标位置是否在子夹上, 返回子夹索引, 鼠标没有在子夹上就返回-1
inline int _tab_hittest(LPTAB_INFO_TOPSTATE tabInfo, LPTCHITTESTINFO phi)
{
    // wParam = 0, lParam = TCHITTESTINFO 结构指针, 返回子夹索引, 鼠标没有在子夹上就返回-1
    // pos->flags = 
    // TCHT_NOWHERE     该位置不在选项卡上。
    // TCHT_ONITEM      该位置位于选项卡上方，但未位于其图标或其文本上方。对于所有者绘制的选项卡控件，如果位置在选项卡上的任何位置，则指定此值。
    // TCHT_ONITEEMICON 该位置位于选项卡的图标上方。
    // TCHT_ONITEMLABEL 该位置位于选项卡的文本上方。
    // TCHT_ONITEM 是对 TCHT_ONITEMICON 和 TCHT_ONITEMLABEL 的按位或运算。
    RECT rc;
    int i = 0;
    TAB_ITEMINFO_ARR& arr = *tabInfo->arr;
    size_t size = arr.size();
    phi->flags = TCHT_NOWHERE;
    rc.left = 0;
    rc.right = 0;
    rc.top = Tab_CalcTop(tabInfo->pData);
    rc.bottom = tabInfo->pData->pos.height;
    if (phi->pt.y < rc.top || phi->pt.y > rc.bottom)
        return -1;
    for (auto it = arr.begin(); it != arr.end(); ++it, ++i)
    {
        rc.left = rc.right;
        rc.right = rc.left + it->width;
        if (phi->pt.x >= rc.left && phi->pt.x < rc.right)
        {
            phi->flags = TCHT_ONITEM;
            return i;
        }
    }
    return -1;
}

// 向选择夹里插入子夹, 成功返回子夹索引, 失败返回-1
// index = 要插入的位置
// item = 插入的数据
// isUnicode = item这个结构是否为W版, 如果为false, 那就会转换成W版
inline int _tab_insert(LPTAB_INFO_TOPSTATE tabInfo, DWORD index, TCITEMW* item, bool isUnicode)
{
    LPTAB_ITEMINFO_ARR pArr = tabInfo->arr;
    size_t count = pArr->size();
    if (index > (DWORD)count)   // 索引==项目数, 那就是加入到最后
        index = (DWORD)count;   // DWORD 是无符号整数, 最小是0
    TAB_ITEMINFO info = { 0 };

    if (__query(item->mask, TCIF_IMAGE))
        info.iImage = item->iImage;
    if (__query(item->mask, TCIF_PARAM))
        info.lParam = item->lParam;
    if (__query(item->mask, TCIF_STATE))
        info.state = item->dwState;

    if (__query(item->mask, TCIF_TEXT))
    {
        if (isUnicode)
        {
            info.SetName(item->pszText);
        }
        else
        {
            wstr W = _str::A2W((LPSTR)item->pszText);
            info.SetName(W.c_str());
        }
        info.width = _tab_calctext(tabInfo, info.name);
    }
    if (tabInfo->pData->draw.indexSel == -1)
        tabInfo->pData->draw.indexSel = 0;
    pArr->insert(pArr->begin() + index, info);
    InvalidItem(tabInfo, 0);
    return index;
}

// 向选择夹里插入子夹, 成功返回子夹索引, 失败返回-1
// index = 要插入的位置
// item = 插入的数据
// isUnicode = item这个结构是否为W版, 如果为false, 那就会转换成W版
inline bool _tab_setitem(LPTAB_INFO_TOPSTATE tabInfo, DWORD index, TCITEMW* item, bool isUnicode)
{
    size_t count = tabInfo->arr->size();
    if (index >= count)
        return false;

    TAB_ITEMINFO& info = tabInfo->arr->at(index);

    if (__query(item->mask, TCIF_IMAGE))
        info.iImage = item->iImage;
    if (__query(item->mask, TCIF_PARAM))
        info.lParam = item->lParam;
    if (__query(item->mask, TCIF_STATE))
        info.state = item->dwState;

    if (__query(item->mask, TCIF_TEXT))
    {
        if (isUnicode)
        {
            info.SetName(item->pszText);
        }
        else
        {
            wstr W = _str::A2W((LPSTR)item->pszText);
            info.SetName(W.c_str());
        }
        info.width = _tab_calctext(tabInfo, info.name);
        InvalidItem(tabInfo, 0);
    }
    return true;
}
// 获取指定子夹的数据
// index = 要插入的位置
// item = 插入的数据
// isUnicode = item这个结构是否为W版, 如果为false, 那就会转换成W版
inline bool _tab_getitem(LPTAB_INFO_TOPSTATE tabInfo, DWORD index, TCITEMW* item, bool isUnicode)
{
    size_t count = tabInfo->arr->size();
    if (index >= count)
        return false;

    TAB_ITEMINFO& info = tabInfo->arr->at(index);

    if (__query(item->mask, TCIF_IMAGE))
        item->iImage = info.iImage;
    if (__query(item->mask, TCIF_PARAM))
        item->lParam = info.lParam;
    if (__query(item->mask, TCIF_STATE))
        item->dwState = info.state;

    if (__query(item->mask, TCIF_TEXT) && item->cchTextMax > 0 && info.name)
    {
        if (isUnicode)
        {
            wcscpy_s((LPWSTR)item->pszText, item->cchTextMax, info.name);
        }
        else
        {
            _str A = wstr::W2A(info.name);
            strcpy_s((LPSTR)item->pszText, item->cchTextMax, A.c_str());
        }
    }
    return true;
}
// 设置固定宽高的选项卡宽高, 返回旧宽度和高度。 宽度位于返回值的 LOWORD 中，高度在 HIWORD中
// width = 选项卡的宽度, 最小20, 小于20会设置为20
// height = 选项卡高度, 最小是初始化时的选项卡高度
inline DWORD _tab_setitemsize(LPTAB_INFO_TOPSTATE tabInfo, int width, int height)
{
    if (height < tabInfo->pData->draw.nItemHeight)
        height = tabInfo->pData->draw.nItemHeight;
    if (width < 20)
        width = 20;
    LRESULT ret = (LRESULT)tabInfo->size;
    tabInfo->size = (DWORD)MAKELONG(width, height);
    return ret;
}

inline LRESULT _tab_on_lbutton_down(LPTAB_INFO_TOPSTATE tabInfo, UINT message, WPARAM wParam, LPARAM lParam)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    LPOBJSTRUCT pData = tabInfo->pData;
    TopState_UpdateStateWindow(pData->hWnd);
    int top = Tab_CalcTop(pData);
    if (pt.y > top && pt.y <= (int)pData->pos.height && pData->draw.indexHot != -1)
    {
        // 鼠标点击在子夹内
        pData->draw.indexDown = pData->draw.indexHot;
        InvalidItem(tabInfo, top);
        SetCapture(pData->hWnd);
    }
    return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
}
inline LRESULT _tab_on_lbutton_up(LPTAB_INFO_TOPSTATE tabInfo, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHITTESTINFO hi = { 0 };
    POINT& pt = hi.pt;
    pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    LPOBJSTRUCT pData = tabInfo->pData;
    int top = Tab_CalcTop(pData);

    // 鼠标点击在子夹内
    if (pData->draw.indexDown != -1)
        ReleaseCapture();

    return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
}
inline LRESULT _tab_on_capture_changed(LPTAB_INFO_TOPSTATE tabInfo, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPOBJSTRUCT pData = tabInfo->pData;
    TCHITTESTINFO hi = { 0 };
    POINT& pt = hi.pt;
    GetCursorPos(&pt);
    ScreenToClient(pData->hWnd, &pt);
    int top = Tab_CalcTop(pData);
    if (pData->draw.indexDown != -1)    // 释放的是鼠标在子夹按下
    {
        // 鼠标点击在子夹内
        int index = _tab_hittest(tabInfo, &hi);
        if (index == pData->draw.indexDown)
        {
            // 放开时鼠标还是在按下的项目内, 触发选项被改变事件, 根据返回值确定新的选中项
            if (index == pData->draw.indexSel)  // 点击的还是当前选中的项目, 不触发
                return 0;
            pData->draw.indexSel = _tab_sel_change(tabInfo, index, pData->draw.indexSel, TAB_SEL_MODE_LBUTTON);
        }
        else
        {
            pData->draw.indexHot = index;   // 否则热点项就改成当前鼠标位置的项目
        }
        
        pData->draw.indexDown = -1;
        InvalidItem(tabInfo, top);
        
        return 0;
    }


    return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
}
inline LRESULT _tab_on_rbutton_down(LPTAB_INFO_TOPSTATE tabInfo, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPOBJSTRUCT pData = tabInfo->pData;

    return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
}
inline LRESULT _tab_on_rbutton_up(LPTAB_INFO_TOPSTATE tabInfo, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPOBJSTRUCT pData = tabInfo->pData;

    return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
}
inline static void CALLBACK TimerProc_Leave_Tab(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    LPOBJSTRUCT pData = (LPOBJSTRUCT)id;
    if (pData->draw.indexHot == -1)
    {
        KillTimer(hWnd, id);
        return;
    }
    POINT pt;
    RECT rcItem;
    GetCursorPos(&pt);
    ScreenToClient(hWnd, &pt);
    TabCtrl_GetItemRect(hWnd, pData->draw.indexHot, &rcItem);   // 取热点项的位置
    // 如果鼠标不在热点项中就需要重画热点项
    if (!PtInRect(&rcItem, pt))
    {
        KillTimer(hWnd, id);
        pData->draw.indexHot = -1;
        InvalidateRect(hWnd, &rcItem, 0);
    }
}
inline LRESULT _tab_on_mouse_move(LPTAB_INFO_TOPSTATE tabInfo, UINT message, WPARAM wParam, LPARAM lParam)
{
    if ((wParam & MK_LBUTTON))
        return 0;
    TCHITTESTINFO hi = { 0 };
    LPOBJSTRUCT pData = tabInfo->pData;
    TAB_ITEMINFO_ARR& arr = *tabInfo->arr;
    POINT& pt = hi.pt;
    pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    int top = Tab_CalcTop(pData);
    if (pt.y > top && pt.y <= (int)pData->pos.height)
    {
        // 鼠标点击在子夹内
        int index = _tab_hittest(tabInfo, &hi);
        if (index != pData->draw.indexHot)
        {
            SetTimer(pData->hWnd, (UINT_PTR)pData, 50, TimerProc_Leave_Tab);
            pData->draw.indexHot = index;
            InvalidItem(tabInfo, top);
        }
        return 0;
    }
    return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
}

inline LRESULT _tab_on_mouse_leave(LPTAB_INFO_TOPSTATE tabInfo, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPOBJSTRUCT pData = tabInfo->pData;

    return CallWindowProcW(pData->oldProc, pData->hWnd, message, wParam, lParam);
}
inline LRESULT _tab_on_nchittest(LPTAB_INFO_TOPSTATE tabInfo, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPOBJSTRUCT pData = tabInfo->pData;
    LPTOPSTATE_INFO info = tabInfo->info;
    RECT& rc = pData->pos.rc;
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    ScreenToClient(pData->hWnd, &pt);
    LRESULT ret = HTCLIENT;
    if (pt.x < WINDOW_SPACING)
    {
        // 鼠标在左边预留边框范围内, 需要判断这个窗口有没有预留左边
        if (__is_reserve_left(info))
            ret = HTLEFT;
    }
    else if (pt.y < WINDOW_SPACING)
    {
        // 鼠标在顶边预留边框范围内, 需要判断这个窗口有没有预留顶边
        if (__is_reserve_top(info))
            ret = HTTOP;
    }
    else if (pt.x > (pData->pos.width - WINDOW_SPACING) && pt.x < pData->pos.width)
    {
        // 鼠标在右边预留边框范围内, 需要判断这个窗口有没有预留右边
        if (__is_reserve_right(info))
            ret = HTRIGHT;
    }
    else if (pt.y > (pData->pos.height - WINDOW_SPACING) && pt.y < pData->pos.height)
    {
        // 鼠标在底边预留边框范围内, 需要判断这个窗口有没有预留右边
        if (__is_reserve_bottom(info))
            ret = HTBOTTOM;
    }
    return ret;
}

HWND CreateGroup(DWORD dwExStyle, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id, LPVOID lpParam)
{
    static HINSTANCE hinst = GetModuleHandleW(0);
    if (!m_pfnTabWndProc)
    {
        WNDCLASSEXW wcex = { 0 };
        wcex.cbSize = sizeof(wcex);
        GetClassInfoExW(hinst, L"SysTabControl32", &wcex);
        m_pfnTabWndProc = wcex.lpfnWndProc;

        wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_PARENTDC;
        wcex.lpfnWndProc    = __def_group_cell_proc;
        wcex.cbClsExtra     += sizeof(LPVOID);
        wcex.cbWndExtra     += sizeof(LPVOID);
        //wcex.hInstance      = hinst;
        wcex.lpszClassName  = L"CellGroup";
        RegisterClassExW(&wcex);
        s_info->d2d = _canvas_new();
    }

    HWND hWnd = CreateWindowExW(dwExStyle, L"CellGroup", lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, (HMENU)id, 0, 0);
    Tab_InsertHwnd(hWnd);
    SendMessageW(hWnd, WM_SETFONT, (WPARAM)CWndBase::DefaultFont(), 0);
    return hWnd;
}



LRESULT CALLBACK WndProc_Group_Cell(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPTAB_INFO_TOPSTATE tabInfo = _get_tabinfo(pData);
    LPTAB_ITEMINFO_ARR pArr = tabInfo->arr;
    switch (message)
    {
    case WM_CREATE:
    {
        pData->draw.nItemHeight = 20;
        pData->draw.hCanvas = _canvas_create(hWnd, 1, 1);
        pData->style = GetWindowLongPtrW(hWnd, GWL_STYLE);
        pData->styleEx = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

        GetClientRect(hWnd, &pData->pos.rcClient);

        pData->pos.width = pData->pos.rcClient.right - pData->pos.rcClient.left;
        pData->pos.height = pData->pos.rcClient.bottom - pData->pos.rcClient.top;

        break;
    }
    case WM_DESTROY:
    {
        _free_tabinfo(pData);
        _canvas_destroy(pData->draw.hCanvas);
        pData->draw.hCanvas = 0;
        break;
    }
    case WM_NCPAINT:
        return 0;
    case WM_PAINT:
        return Group_Cell_OnPaint(hWnd, message, wParam, lParam, pData);
    case WM_CTLCOLORSTATIC:
    {
        MAP_INFO* map = (MAP_INFO*)GetWindowLongPtrW((HWND)lParam, GWLP_USERDATA);
        return (LRESULT)map->hbrBack;
    }
    case WM_MOUSEMOVE:
        return _tab_on_mouse_move(tabInfo, message, wParam, lParam);
    case WM_MOUSELEAVE:
        return _tab_on_mouse_leave(tabInfo, message, wParam, lParam);
    case WM_LBUTTONDOWN:
        return _tab_on_lbutton_down(tabInfo, message, wParam, lParam);
    case WM_LBUTTONUP:
        return _tab_on_lbutton_up(tabInfo, message, wParam, lParam);
    case WM_CAPTURECHANGED:
        return _tab_on_capture_changed(tabInfo, message, wParam, lParam);

    case WM_RBUTTONDOWN:
        return _tab_on_rbutton_down(tabInfo, message, wParam, lParam);
    case WM_RBUTTONUP:
        return _tab_on_rbutton_up(tabInfo, message, wParam, lParam);
    case WM_WINDOWPOSCHANGED:
    {
        LPWINDOWPOS pos = (LPWINDOWPOS)lParam;
        if ((pos->flags & SWP_NOSIZE) == 0 || (pos->flags & SWP_NOMOVE) == 0)    // 改变了尺寸或者被移动了
        {
            pData->pos.rc.left = pos->x;
            pData->pos.rc.top = pos->y;
            pData->pos.rc.right = pos->x + pos->cx;
            pData->pos.rc.bottom = pos->y + pos->cy;

            //TODO 这里应该改, 好多地方都用到这个结构, 牵一发而动全身
            pData->pos.rcClient.left = pos->x;
            pData->pos.rcClient.top = pos->y;
            pData->pos.rcClient.right = pos->x + pos->cx;
            pData->pos.rcClient.bottom = pos->y + pos->cy;

            //pData->pos.rcClient.left = 0;
            //pData->pos.rcClient.top = 0;
            //pData->pos.rcClient.right = pos->cx;
            //pData->pos.rcClient.bottom = pos->cy;
            pData->pos.x = pos->x;
            pData->pos.y = pos->y;
            pData->pos.width = pos->cx;
            pData->pos.height = pos->cy;
        }
        if (!pData->fnWndproc)
            return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);

        return pData->fnWndproc(hWnd, message, wParam, lParam, pData);
    }
    case TCM_DELETEALLITEMS:     // wParam, lParam == 0;
    {
        pArr->clear();
        tabInfo->pData->draw.indexSel = -1;
        InvalidItem(tabInfo, 0);
        return 1;
    }
    case TCM_DELETEITEM:     // wParam = 项目索引, lParam == 0;
    {
        size_t count = pArr->size();
        if (wParam < 0 || wParam >= count)
            return 0;
        pArr->erase(pArr->begin() + wParam);
        if (count == 1)
            tabInfo->pData->draw.indexSel = -1;
        InvalidItem(tabInfo, 0);
        return 1;
    }
    case TCM_GETCURFOCUS:   // wParam, lParam == 0;
        return pData->draw.indexDown;
    case TCM_GETCURSEL:     // wParam, lParam == 0;
        return pData->draw.indexSel;
    //case TCM_GETIMAGELIST:
    case TCM_GETITEMA:
    case TCM_GETITEMW:      // wParam = 索引, lParam = TCITEM 结构指针, 如果成功，则返回 TRUE ; 否则返回 FALSE
        return (LRESULT)_tab_getitem(tabInfo, (DWORD)wParam, (LPTCITEMW)lParam, message == TCM_GETITEMW);
    case TCM_GETITEMCOUNT:
        return (LRESULT)pArr->size();
    case TCM_GETITEMRECT:   // wParam = 索引, lParam = RECT 结构指针, 返回真假
        return (LRESULT)_tab_get_item_rect(tabInfo, (DWORD)wParam, (RECT*)lParam);
    case TCM_HITTEST:       // wParam = 0, lParam = TCHITTESTINFO 结构指针, 返回子夹索引, 鼠标没有在子夹上就返回-1
        return (LRESULT)_tab_hittest(tabInfo, (LPTCHITTESTINFO)lParam);
    
    case TCM_INSERTITEMA:
    case TCM_INSERTITEMW:   // wParam = 索引, lParam = TCITEM 结构指针, 如果成功，则返回新的选项卡的索引; 否则返回-1
        return (LRESULT)_tab_insert(tabInfo, (DWORD)wParam, (LPTCITEMW)lParam, message == TCM_INSERTITEMW);
    case TCM_SETCURFOCUS:   // wParam = 索引, lParam = 0, 没有返回值
        pData->draw.indexDown = (int)wParam;
        break;
    case TCM_SETCURSEL:     // wParam = 索引, lParam = 0, 没有返回值
        pData->draw.indexSel = _tab_sel_change(tabInfo, (int)wParam, pData->draw.indexSel, TAB_SEL_MODE::TAB_SEL_MODE_CODE);
        break;
    case TCM_SETITEMA:
    case TCM_SETITEMW:      // wParam = 索引, lParam = TCITEM 结构指针, 如果成功，则返回 TRUE ; 否则返回 FALSE
        return (LRESULT)_tab_setitem(tabInfo, (DWORD)wParam, (LPTCITEMW)lParam, message == TCM_SETITEMW);
    case TCM_SETITEMSIZE:   // wParam = 0, lParam = LOWORD=宽度, HIWORD=高度, 返回旧宽度和高度。 宽度位于返回值的 LOWORD 中，高度在 HIWORD中
        return (LRESULT)_tab_setitemsize(tabInfo, LOWORD(lParam), HIWORD(lParam));
    case TCM_SETMINTABWIDTH:
    {
        // wParam = 0
        // lParam = 为选项卡控件项设置的最小宽度。 如果将此参数设置为-1，则控件将使用默认选项卡宽度
        // 返回上一个最小宽度
        LRESULT ret = (LRESULT)tabInfo->minWidth;
        tabInfo->minWidth = lParam;
        return ret;
    }
    case TCM_SETPADDING:
    {
        // wParam = 0
        // lParam = 每个选项卡的填充空间, 低位 = 填充宽度, 高位 = 填充高度
        // 没有返回值
        tabInfo->padding = lParam;
        break;
    }
    case TCM_INSERT_TOPSTATE:   // wParam = 1=添加,0=删除,-1=查询, lParam = 要添加/删除/查询的值, 返回是否成功
    {
        LPTOPSTATE_INFO info = tabInfo->info;
        bool ret = true;
        switch (wParam)
        {
        case 1:     __add_flags(info, lParam); break;   // 添加
        case 0:     __del_flags(info, lParam); break;   // 删除
        default:    ret = __query_flags(info, lParam); break; // 查询
            break;
        }
        return (LRESULT)ret;
    }
    case WM_NCHITTEST:
        return _tab_on_nchittest(tabInfo, message, wParam, lParam);
    case WM_ERASEBKGND:
        return 1;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 选择夹选中被改变, 返回改变后的索引, 触发这个事件的时候选中还没有被改变, 不让改变的就返回 indexOld
// pData = 选择夹窗口数据
// index = 新的被选中的索引
// indexOld = 改变前选中的索引
// selMode = 选中模式, 是鼠标点击选中还是代码选中, TAB_SEL_MODE 枚举常量
int _tab_sel_change(LPTAB_INFO_TOPSTATE tabInfo, int index, int indexOld, int selMode)
{
    LPOBJSTRUCT pData = tabInfo->pData;
    if (selMode == TAB_SEL_MODE_CODE)
    {
        // 是代码切换子夹, 需要重画
        //InvalidItem(tabInfo, 0);
    }
    int size = (int)tabInfo->arr->size();
    TABCHANGE_INFO hdr = { 0 };
    hdr.hdr.code = TCN_SELCHANGING;
    hdr.hdr.hwndFrom = tabInfo->pData->hWnd;
    hdr.hdr.idFrom = GetWindowLongPtrW(tabInfo->pData->hWnd, GWLP_ID);
    hdr.index = index;
    hdr.indexOld = indexOld;
    hdr.selMode = selMode;
    //if (index >= 0 && index < size)
    //    hdr.pItem = &(tabInfo->arr->at(index));
    //if (indexOld >= 0 && indexOld < size)
    //    hdr.pItemOld = &(tabInfo->arr->at(indexOld));

    HWND hParent = GetParent(hdr.hdr.hwndFrom);
    // 触发即将改变事件, 返回非0值就拦截
    if (SendMessageW(hParent, WM_NOTIFY, hdr.hdr.idFrom, (LPARAM)&hdr))
        return indexOld;

    // 触发选项已改变事件, 无返回值
    hdr.hdr.code = TCN_SELCHANGE;
    SendMessageW(hParent, WM_NOTIFY, hdr.hdr.idFrom, (LPARAM)&hdr);
    InvalidateRect(pData->hWnd, 0, 0);

    return index;
}
