#include "../EWindow_Fne_Header.h"
#include <control/CInputBox.h>
#include <control/draw_Window.h>


const LPCWSTR GUID_pData = L"{2045B090-360C-4927-AC27-85A587FD2C17}";

struct DIALOG_FIND_TEXT
{
    wstr text;
    LPARAM param;       // text转成数值后的值
    bool isCtrl;
    bool isAlt;
    bool isShift;
    bool isDown;
    LPOBJSTRUCT pData;
    DIALOG_FIND_TEXT() :isCtrl(false), isAlt(false), isShift(false), isDown(false), pData(0), param(0)
    {

    }
};
typedef DIALOG_FIND_TEXT* LPDIALOG_FIND_TEXT;
static std::unordered_map<HWND, DIALOG_FIND_TEXT> m_map;    // 窗口对应的数据

void _tree_ctrl_f(HWND hWnd, LPOBJSTRUCT pData);
bool CALLBACK _tree_dialog_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, INT_PTR* pRet);

// 把一个文本转成数值, 0x开头是16进制, 否则就是10进制
LONG_PTR _tree_ToNumber(LPCWSTR str);

// 从当前选中项往下查找, 如果当前没有选中, 就从头开始查找, 查找成功返回项目句柄
// data = 需要搜索的数据
// hItemStart = 开始搜索的项目, 这个项目被跳过, 从这个项目的下一个开始搜索, 如果为0, 则从第一个项目开始搜索到尾
// hItemEnd = 搜索时遇到这个项目, 搜索完这个项目后就停止搜索, 为0则搜索到最后一个项目
HTREEITEM _find_next(DIALOG_FIND_TEXT& data, HTREEITEM hItemStart, HTREEITEM hItemEnd);

// 从当前选中项往上查找, 如果当前没有选中, 就从尾部开始查找, 查找成功返回项目句柄
// data = 需要搜索的数据
// hItemStart = 开始搜索的项目, 这个项目被跳过, 从这个项目的下一个开始搜索, 如果为0, 则从第一个项目开始搜索到尾
// hItemEnd = 搜索时遇到这个项目, 搜索完这个项目后就停止搜索, 为0则搜索到最后一个项目
HTREEITEM _find_prev(DIALOG_FIND_TEXT& data, HTREEITEM hItemStart, HTREEITEM hItemEnd);
bool _tree_find(DIALOG_FIND_TEXT& data, bool isNext);
void _tree_dialog_on_command(HWND hDlg, int code, int id, LPARAM lParam);
void _tree_set_check(HWND hDlg, HWND hCheck);
inline void _tree_set_StateWindow(DIALOG_FIND_TEXT* data)
{
    TopState_UpdateStateWindow(data->pData->hWnd);
}

LRESULT CALLBACK WndProc_TreeView_KeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    if (message != WM_KEYDOWN)
        return 0;

    if (wParam == 'F')
    {
        const int ctrl = GetAsyncKeyState(VK_CONTROL);
        const bool isCtrl = (ctrl & 32768) != 0;
        if (isCtrl)
        {
            //HACK Ctrl + F 键按下, 弹出一个搜索对话框
            _tree_ctrl_f(hWnd, pData);
            return 0;
        }
    }
    else if (wParam == VK_F3)
    {
        const int shift = GetAsyncKeyState(VK_SHIFT);
        const bool isShift = (shift & 32768) != 0;
        auto it = m_map.find(hWnd);
        if (it != m_map.end())
        {
            DIALOG_FIND_TEXT& data = it->second;
            _tree_find(data, !isShift);
        }

    }
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}

void _tree_ctrl_f(HWND hWnd, LPOBJSTRUCT pData)
{
    DIALOG_FIND_TEXT& data = m_map[hWnd];
    data.pData = pData;
    const bool b[] =
    {
        data.isCtrl,
        data.isAlt,
        data.isShift,
        data.isDown,
    };
    data.isDown = false;

    INPUTBOXSTRUCT input = { 0 };
    input.hParent = hWnd;
    input.text = L"Ctrl + \"查找\": 精确查找, 直接点击\"查找\"则使用模糊查找\r\n\r\n查找项目数值支持10进制和16进制, 16进制必须0x开头(不区分大小写)";
    input.caption = L"请输入查找的内容";
    input.defText = data.text.c_str();
    input.pszOkButton = L"查找(&O)";
    input.pszCancelButton = L"取消(&C)";
    input.lpDialogFunc = _tree_dialog_proc;
    input.dwInitParam = (LPARAM)&data;

    wchar_t* ret = input_OpenEx(&input);
    SetFocus(hWnd);
    _tree_set_StateWindow(&data);
    if (!ret)
    {
        data.isCtrl  = b[0];    // 取消的话, 需要把之前的状态还原
        data.isAlt   = b[1];
        data.isShift = b[2];
        data.isDown  = b[3];
        return;
    }
    data.text = ret;
    data.param = _tree_ToNumber(ret);
    input_free(ret);

    _tree_find(data, true);

    //else
    //{
    //    MessageBeep(MB_ICONASTERISK);   // 没有找到就弹个声音
    //}
}
bool _tree_find(DIALOG_FIND_TEXT& data, bool isNext)
{
    if (!data.pData)
        return false;

    HTREEITEM hItemIndex = TreeView_GetSelection(data.pData->hWnd);
    HTREEITEM hItem = isNext ? _find_next(data, hItemIndex, 0) : _find_prev(data, hItemIndex, 0);
    if (hItem)
        TreeView_SelectItem(data.pData->hWnd, hItem);
    return hItem != 0;
}
// 查找项目, 返回这个项目的数据是否和查找的内容一致
inline bool _find_item(DIALOG_FIND_TEXT& data, HTREEITEM hItem)
{
    wchar_t buffer[1024];
    buffer[0] = 0;
    TVITEMW item    = { 0 };
    item.mask       = data.isShift ? TVIF_PARAM : TVIF_TEXT;  // 有shift键就是需要搜索项目数值
    item.hItem      = hItem;
    item.pszText    = buffer;
    item.cchTextMax = 1024;
    TreeView_GetItem(data.pData->hWnd, &item);

    if (data.isShift)
    {
        // 搜索项目数值, 判断是否模糊搜索
        if (data.isCtrl)
        {
            // 模糊查找项目数值, 需要把数值转换成文本, 走到这里的话 buffer就是没用上, 这里可以用
#ifdef _WIN64
            const LPCWSTR fmt = L"%lld";
#else
            const LPCWSTR fmt = L"%d";
#endif
            wchar_t text[50];

            swprintf_s(buffer, 1024, fmt, item.lParam);
            swprintf_s(text  , 50  , fmt, data.param ); // 因为用户输入的有可能是十六进制, 所以得统一转成一种格式

            size_t pos = wstr::find(buffer, text, 0, true);
            return pos != wstr::npos;   // 把项目数值转成文本, 搜索是否包含输入的内容
        }
        return data.param == item.lParam;   // 精确查找就判断是否相等
    }

    // 搜索文本, buffer 就是当前项目的文本
    if (data.isCtrl)
    {
        // 精确搜索就返回两个文本是否相等
        return wcscmp(data.text.c_str(), buffer) == 0;
    }

    size_t pos = wstr::find(buffer, data.text.c_str(), 0, false);
    return pos != wstr::npos;
}

// 从当前选中项往下查找, 如果当前没有选中, 就从头开始查找, 查找成功返回项目句柄
// data = 需要搜索的数据
// hItemStart = 开始搜索的项目, 这个项目被跳过, 从这个项目的下一个开始搜索, 如果为0, 则从第一个项目开始搜索到尾
// hItemEnd = 搜索时遇到这个项目, 搜索完这个项目后就停止搜索, 为0则搜索到最后一个项目
HTREEITEM _find_next(DIALOG_FIND_TEXT& data, HTREEITEM hItemStart, HTREEITEM hItemEnd)
{
    HWND hWnd = data.pData->hWnd;

    HTREEITEM hRoot = TreeView_GetRoot(data.pData->hWnd);   // 根项目
    if (hItemEnd == hRoot)  // 需要停止的项目是根项目, 表示上一次是从根项目开始搜索
        return 0;

    if (!hItemStart) hItemStart = hRoot;
    HTREEITEM hItem = 0, hItemParent = 0, hItemChild = hItemStart;

    bool doen = false;
    while (hItemChild)
    {
        // 搜索这个项目, 不搜索当前选中项
        if (hItemChild != hItemStart && _find_item(data, hItemChild))
            return hItemChild;
        if (hItemChild == hItemEnd) // 最后一个项目已经搜索完了, 不用再继续搜索了
            break;
        hItem = hItemChild;
        hItemChild = TreeView_GetNextItem(hWnd, hItemChild, TVGN_CHILD);
        if (!hItemChild)    // 找不到子项目, 就找兄弟项目
        {
            hItemChild = TreeView_GetNextItem(hWnd, hItem, TVGN_NEXT);
            while (0 == hItemChild)    // 找不到兄弟项目, 就返回到上一级
            {
                hItemParent = TreeView_GetNextItem(hWnd, hItem, TVGN_PARENT); // 取父项目 
                if (hItemParent == 0)
                {
                    doen = true;
                    break;
                }
                hItem = hItemParent;
                hItemChild = TreeView_GetNextItem(hWnd, hItem, TVGN_NEXT);
            }
        }
        if (doen)break;
    }

    if (!hItemEnd)
    {
        MessageBeep(MB_ICONASTERISK);   // 没有找到就弹个声音
        return _find_next(data, hRoot, hItemStart);
    }
    return 0;
}

// 从当前选中项往上查找, 如果当前没有选中, 就从尾部开始查找, 查找成功返回项目句柄
// data = 需要搜索的数据
// hItemStart = 开始搜索的项目, 这个项目被跳过, 从这个项目的下一个开始搜索, 如果为0, 则从第一个项目开始搜索到尾
// hItemEnd = 搜索时遇到这个项目, 搜索完这个项目后就停止搜索, 为0则搜索到最后一个项目
HTREEITEM _find_prev(DIALOG_FIND_TEXT& data, HTREEITEM hItemStart, HTREEITEM hItemEnd)
{
    HWND hWnd = data.pData->hWnd;
    HTREEITEM hRoot = TreeView_GetRoot(data.pData->hWnd);   // 根项目

    int count = TreeView_GetCount(hWnd);
    if (count == 0)
        return 0;
    
    std::vector<HTREEITEM> arr(count);  // 把所有项目都枚举出来, 分两次查找
    arr.resize(0);

    HTREEITEM hItem = 0, hItemParent = 0, hItemChild = hRoot;

    bool doen = false;
    while (hItemChild)
    {
        arr.push_back(hItemChild);
        hItem = hItemChild;
        hItemChild = TreeView_GetNextItem(hWnd, hItemChild, TVGN_CHILD);
        if (!hItemChild)    // 找不到子项目, 就找兄弟项目
        {
            hItemChild = TreeView_GetNextItem(hWnd, hItem, TVGN_NEXT);
            while (0 == hItemChild)    // 找不到兄弟项目, 就返回到上一级
            {
                hItemParent = TreeView_GetNextItem(hWnd, hItem, TVGN_PARENT); // 取父项目 
                if (hItemParent == 0)
                {
                    doen = true;
                    break;
                }
                hItem = hItemParent;
                hItemChild = TreeView_GetNextItem(hWnd, hItem, TVGN_NEXT);
            }
        }
        if (doen)break;
    }

    bool isFind = false;
    for (auto it = arr.rbegin(); it != arr.rend(); ++it)    // 第一次先从选中项查找到根项目
    {
        HTREEITEM h = *it;
        if (!isFind)
        {
            if (h == hItemStart)
                isFind = true;  // 查找到选中项了, 不搜索这个项目, 从下一个开始搜索
        }
        else
        {
            // 这里开始搜索, 这里已经是选中项的上一个项目了
            if (_find_item(data, h))
                return h;
        }
    }

    MessageBeep(MB_ICONASTERISK);   // 没有找到就弹个声音
    // 走到这里就是从选中项到根项目都没有搜索到, 需要从最后一个项目开始搜索
    for (auto it = arr.rbegin(); it != arr.rend(); ++it)    // 从最后一个项目搜索到选中项
    {
        HTREEITEM h = *it;
        if (_find_item(data, h))
            return h;
        if (h == hItemStart)
            return 0;   // 先搜索选中项之后再返回
    }

    return 0;
}


// 把一个文本转成数值, 0x开头是16进制, 否则就是10进制
LONG_PTR _tree_ToNumber(LPCWSTR str)
{
    LONG_PTR ret = 0;
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))  // 0x开头, 是16进制
    {
#ifdef _M_X64
        swscanf_s(str, L"0x%016llx", &ret);
#else
        swscanf_s(str, L"0x%08X", &ret);
#endif
    }
    else
    {
        ret = (LONG_PTR)wstr::stoll(str);
    }
    return (LONG_PTR)ret;
}

const LONG_PTR IDC_CBK_DEFAULT      = 2000; // 模式0, 一般查找, 模糊查找文本
const LONG_PTR IDC_CBK_CTRL         = 2001; // 模式1, 精确查找文本
const LONG_PTR IDC_CBK_SHIFT        = 2002; // 模式2, 精确查找项目数值
const LONG_PTR IDC_CBK_CTRL_SHIFT   = 2003; // 模式3, 模糊查找项目数值
inline bool _tree_key_down(HWND hWnd, bool isDown, int key)
{
    if (key != VK_CONTROL && key != VK_SHIFT)
        return false;

    HWND hParent = GetParent(hWnd);
    if (!hParent)
        hParent = hWnd;

    DIALOG_FIND_TEXT* pData = (DIALOG_FIND_TEXT*)GetPropW(hParent, GUID_pData);
    if (!pData)
        return false;

    if (!isDown)    // 放开按键, 需要判断是否按下了按键
    {
        if (!pData->isDown)  // 没有按下事件就不处理放开
            return false;
    }
    pData->isDown = true;

    HWND hWnd0 = GetDlgItem(hParent, IDC_CBK_DEFAULT);      // 模式0, 一般查找, 模糊查找文本
    HWND hWnd1 = GetDlgItem(hParent, IDC_CBK_CTRL);         // 模式1, 精确查找文本, Ctrl
    HWND hWnd2 = GetDlgItem(hParent, IDC_CBK_SHIFT);        // 模式3, 模糊查找项目数值, Shift
    HWND hWnd3 = GetDlgItem(hParent, IDC_CBK_CTRL_SHIFT);   // 模式4, 精确查找项目数值, Ctrl + Shift
    const int shift     = GetAsyncKeyState(VK_SHIFT);
    const int ctrl      = GetAsyncKeyState(VK_CONTROL);
    const int alt       = GetAsyncKeyState(VK_MENU);
    const bool isShift  = (shift & 32768) != 0;
    const bool isCtrl   = (ctrl  & 32768) != 0;
    const bool isAlt    = (alt   & 32768) != 0;
    HWND hCheck = hWnd0;    // 模式0, 一般查找, 模糊查找文本
    if (isCtrl && isShift)
        hCheck = hWnd3;     // 模式3, 模糊查找项目数值, Shift
    else if (isCtrl && !isShift)
        hCheck = hWnd1;     // 模式1, 精确查找文本, Ctrl
    else if (!isCtrl && isShift)
        hCheck = hWnd2;     // 模式4, 精确查找项目数值, Ctrl + Shift

    const bool isCheck = SendMessageW(hCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;
    if (!isCheck)
    {
        SendMessageW(hWnd0, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(hWnd1, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(hWnd2, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(hWnd3, BM_SETCHECK, BST_UNCHECKED, 0);
        SendMessageW(hCheck, BM_SETCHECK, BST_CHECKED, 0);
    }
    return true;
}

// 对话框处理按键的消息处理函数
inline LRESULT CALLBACK _tree_dialog_Proc_KeyDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    pfn_SubClassWndProc pfn = (pfn_SubClassWndProc)GetPropW(hWnd, L"oldPorc");
    DIALOG_FIND_TEXT* data = (DIALOG_FIND_TEXT*)GetPropW(hWnd, GUID_pData);
    if (!pfn)
        return DefWindowProcW(hWnd, message, wParam, lParam);
    switch (message)
    {
    case WM_NCLBUTTONDOWN:
    case WM_LBUTTONDOWN:
        _tree_set_StateWindow(data);
        return pfn(hWnd, message, wParam, lParam, pData);
    case WM_KEYUP:
    case WM_KEYDOWN:
    {
        if (_tree_key_down(hWnd, message == WM_KEYDOWN, (int)wParam))
            return 0;
        return pfn(hWnd, message, wParam, lParam, pData);
    }
    default:
        return pfn(hWnd, message, wParam, lParam, pData);
    }
    return 0;
}

inline void _tree_set_wndproc(LPOBJSTRUCT pData, DIALOG_FIND_TEXT* data, COLORREF crBack = -1)
{
    if (!pData)
        throw;
    if (crBack != -1)
    {
        pData->draw.color.crBackGround = crBack;
        pData->draw.color.hbrBackGround = s_info->pfnCreateBrush(crBack);
    }
    SetPropW(pData->hWnd, L"oldPorc", pData->fnWndproc);
    SetPropW(pData->hWnd, GUID_pData, data);
    pData->fnWndproc = _tree_dialog_Proc_KeyDown;
}
// 子类化窗口, 处理键盘按键事件
inline void _tree_subwnd_edit(DIALOG_FIND_TEXT* data, HWND hWnd)
{
    _tree_set_wndproc(_subclass_Edit(hWnd, false, false, 0, false), data, -1);

}
inline void _tree_subwnd_btn(DIALOG_FIND_TEXT* data, HWND hWnd, COLORREF crBack)
{
    _tree_set_wndproc(_subclass_Button(hWnd, false, false, 0), data, crBack);
}

inline HWND _tree_CreateButton(DIALOG_FIND_TEXT* data, COLORREF crBack, int x, int y, int cx, int cy, HWND hParent, LONG_PTR id, bool isCheck, LPCWSTR title)
{
    DWORD style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD;
    DWORD styleEx = 0;
    HWND hWnd = CreateWindowExW(styleEx, L"button", title, style, x, y, cx, cy, hParent, (HMENU)(LONG_PTR)id, 0, 0);
    SendMessageW(hWnd, WM_SETFONT, (WPARAM)CWndBase::DefaultFont(), 0);
    if (isCheck)
        SendMessageW(hWnd, BM_SETCHECK, BST_CHECKED, 0);
    _tree_subwnd_btn(data, hWnd, crBack);
    return hWnd;
}

bool CALLBACK _tree_dialog_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, INT_PTR* pRet)
{
    switch (message)
    {
    case WM_INITDIALOG: // 初始化对话框, 需要子类化输入的编辑框, 实时判断输入的文本是否合法
    {
        DIALOG_FIND_TEXT* pData = (DIALOG_FIND_TEXT*)lParam;
        RECT rc;
        GetWindowRect(hDlg, &rc);
        const int cxScreen = rc.right - rc.left + 100;
        const int cyScreen = rc.bottom - rc.top + 50;
        OffsetRect(&rc, -25, -50);
        rc.right = rc.left + cxScreen;
        rc.bottom = rc.top + cyScreen;

        HMONITOR hMonitor = MonitorFromWindow(pData->pData->hWnd, MONITOR_DEFAULTTONEAREST);
        if (hMonitor)
        {
            MONITORINFO mi = { 0 };
            mi.cbSize = sizeof(MONITORINFO);
            if (GetMonitorInfoW(hMonitor, &mi))
            {
                RECT& rcWork = mi.rcWork;
                if (rc.left < rcWork.left)
                    rc.left = rcWork.left + 10, rc.right = rc.left + cxScreen;
                if (rc.top < rcWork.top)
                    rc.top = rcWork.top + 10, rc.bottom = rc.top + cyScreen;
                if (rc.right > rcWork.right)
                    rc.right = rcWork.right - 10, rc.left = rc.right - cxScreen;
                if (rc.bottom > rcWork.bottom)
                    rc.bottom = rcWork.bottom - 10, rc.top = rc.bottom - cyScreen;
            }

        }

        MoveWindow(hDlg, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, false);

        DRAW_WINDOW_ARGUMENT arg;
        memset(&arg, 0, sizeof(arg));

        s_info->pfnControls(IDC_IDRAW_DRAW_DEFARG, (WPARAM)hDlg, (LPARAM)&arg);
        arg.isDrawIcon = false;
        arg.isSubClass = true;

        s_info->pfnControls(IDC_IDRAW_SUBWINDOWEX, (WPARAM)hDlg, (LPARAM)&arg);

        HWND hBtn       = GetDlgItem(hDlg, IDOK);
        HWND hCancel    = GetDlgItem(hDlg, IDCANCEL);
        HWND hEdit      = GetDlgItem(hDlg, 1001);
        HWND hEdit_tips = GetDlgItem(hDlg, 1000);


        COLORREF crBack = ARGB2RGB(arg.crBack);

        _tree_subwnd_btn (pData, hBtn      , -1);
        _tree_subwnd_btn (pData, hCancel   , -1);
        _tree_subwnd_edit(pData, hEdit     );
        _tree_subwnd_edit(pData, hEdit_tips);

        const int mid = cxScreen / 2;
        const int cbkWidth = mid - 20;
        const bool isCheck[4] =
        {
            !pData->isCtrl && !pData->isShift,
            pData->isCtrl && !pData->isShift,
            !pData->isCtrl && pData->isShift,
            pData->isCtrl && pData->isShift,
        };
        pData->isCtrl = false;
        pData->isAlt = false;
        pData->isShift = false;
        _tree_CreateButton(pData, crBack, 14,  145, cbkWidth, 20, hDlg, IDC_CBK_DEFAULT   , isCheck[0], L"模糊查找(不区分大小写)"            );
        _tree_CreateButton(pData, crBack, mid, 145, cbkWidth, 20, hDlg, IDC_CBK_CTRL      , isCheck[1], L"精确查找(Ctrl+查找)"              );
        _tree_CreateButton(pData, crBack, 14 , 170, cbkWidth, 20, hDlg, IDC_CBK_SHIFT     , isCheck[2], L"精确查找项目数值(Shift+查找)"      );
        _tree_CreateButton(pData, crBack, mid, 170, cbkWidth, 20, hDlg, IDC_CBK_CTRL_SHIFT, isCheck[3], L"模糊查找项目数值(Ctrl+Shift+查找)" );


        MoveWindow(hEdit_tips   , 11, 41, 480, 70, true);
        MoveWindow(hEdit   , 11, 120, 480, 20, true);
        MoveWindow(hBtn   , mid - 100, 200, 90, 31, true);
        MoveWindow(hCancel, mid + 10 , 200, 90, 31, true);

        SetPropW(hDlg, GUID_pData, (HANDLE)lParam);
        break;
    }
    case WM_NCLBUTTONDOWN:
    case WM_LBUTTONDOWN:
    {
        DIALOG_FIND_TEXT* data = (DIALOG_FIND_TEXT*)GetPropW(hDlg, GUID_pData);
        _tree_set_StateWindow(data);
        break;
    }
    case WM_COMMAND:
    {
        const int id = LOWORD(wParam);
        const int code = HIWORD(wParam);
        _tree_dialog_on_command(hDlg, code, id, lParam);
        break;
    }
    case WM_DESTROY:
    {

        break;
    }
    case WM_KEYUP:
    case WM_KEYDOWN:
    {
        if (_tree_key_down(hDlg, message == WM_KEYDOWN, (int)wParam))
            return (INT_PTR)true;
        break;
    }
    default:
        return (INT_PTR)false;
    }
    return (INT_PTR)false;
}

void _tree_dialog_on_command(HWND hDlg, int code, int id, LPARAM lParam)
{
    if (code != BN_CLICKED)
        return;
    switch (id)
    {
    case IDOK:
    {
        DIALOG_FIND_TEXT* pData = (DIALOG_FIND_TEXT*)GetPropW(hDlg, GUID_pData);
        if (!pData)
            break;

        // 点击了确认, 需要记录当前的按键状态
        pData->isShift  = false;
        pData->isCtrl   = false;
        pData->isAlt    = false;
        
        //HWND hWnd0 = GetDlgItem(hDlg, IDC_CBK_DEFAULT);      // 模式0, 一般查找, 模糊查找文本
        HWND hWnd1 = GetDlgItem(hDlg, IDC_CBK_CTRL);         // 模式1, 精确查找文本, Ctrl
        HWND hWnd2 = GetDlgItem(hDlg, IDC_CBK_SHIFT);        // 模式2, 模糊查找项目数值, Shift
        HWND hWnd3 = GetDlgItem(hDlg, IDC_CBK_CTRL_SHIFT);   // 模式3, 精确查找项目数值, Ctrl + Shift

        //const bool isCheck0 = SendMessageW(hWnd0, BM_GETCHECK, 0, 0) == BST_CHECKED;
        const bool isCheck1 = SendMessageW(hWnd1, BM_GETCHECK, 0, 0) == BST_CHECKED;
        const bool isCheck2 = SendMessageW(hWnd2, BM_GETCHECK, 0, 0) == BST_CHECKED;
        const bool isCheck3 = SendMessageW(hWnd3, BM_GETCHECK, 0, 0) == BST_CHECKED;
        if      (isCheck1) pData->isCtrl  = true;
        else if (isCheck2) pData->isShift = true;
        else if (isCheck3) pData->isCtrl  = true, pData->isShift = true;
        break;
    }
    case IDC_CBK_DEFAULT:       // 模式0, 一般查找, 模糊查找文本
    case IDC_CBK_CTRL:          // 模式1, 精确查找文本
    case IDC_CBK_SHIFT:         // 模式2, 精确查找项目数值
    case IDC_CBK_CTRL_SHIFT:    // 模式3, 模糊查找项目数值
    {
        _tree_set_check(hDlg, (HWND)lParam);
        break;
    }
    default:
        break;
    }
}

void _tree_set_check(HWND hDlg, HWND hCheck)
{
    HWND hWndCbk[4];
    hWndCbk[0] = GetDlgItem(hDlg, IDC_CBK_DEFAULT);      // 模式0, 一般查找, 模糊查找文本
    hWndCbk[1] = GetDlgItem(hDlg, IDC_CBK_CTRL);         // 模式1, 精确查找文本, Ctrl
    hWndCbk[2] = GetDlgItem(hDlg, IDC_CBK_SHIFT);        // 模式2, 模糊查找项目数值, Shift
    hWndCbk[3] = GetDlgItem(hDlg, IDC_CBK_CTRL_SHIFT);   // 模式3, 精确查找项目数值, Ctrl + Shift

    for (int i = 0; i < sizeof(hWndCbk) / sizeof(hWndCbk[0]); i++)
    {
        if (hWndCbk[i] != hCheck)
            SendMessageW(hWndCbk[i], BM_SETCHECK, BST_UNCHECKED, 0);
    }
}
