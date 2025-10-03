#pragma once
#include "WndBase.h"
class Ctool : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return TOOLBARCLASSNAMEW; }

public:
    Ctool() :CControlBase() { ; }
    virtual ~Ctool() { ; }

public:
#ifndef TBSTYLE_EX_DOUBLEBUFFER
#define TBSTYLE_EX_DOUBLEBUFFER 0x00000080
#endif
#ifndef TB_SETLISTGAP
#define TB_SETLISTGAP (WM_USER + 96)
#endif
#ifndef TB_GETITEMDROPDOWNRECT
#define TB_GETITEMDROPDOWNRECT (WM_USER + 103)
#endif

    virtual inline LPCWSTR className() { return CLASS_TOOL; }
    virtual inline HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0)
    {
        if (dwStyle == 0)
            dwStyle = TBSTYLE_FLAT | CCS_NOPARENTALIGN | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE | CCS_NODIVIDER;
        dwStyle |= WS_CHILD;
        HWND hWnd = CWndBase::create(dwExStyle, lpszTitle, dwStyle, x, y, nWidth, nHeight, hWndParent, ID, lpParam);
        if (hWnd)
        {
            Send(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);    // 设置BUTTON结构大小
            Send(TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_DOUBLEBUFFER);
        }
        return hWnd;
    }
    // 设置工具条扩展样式, 返回原来的样式
    inline DWORD SetToolStyleEx(DWORD style) const
    {
        return (DWORD)Send(TB_SETEXTENDEDSTYLE, 0, (LPARAM)style);
    }
    // 获取工具条扩展样式, 返回原来的样式
    inline DWORD GetToolStyleEx() const
    {
        return (DWORD)Send(TB_GETEXTENDEDSTYLE, 0, (LPARAM)0);
    }
    // 添加工具条扩展样式, 返回原来的样式
    inline DWORD AddToolStyleEx(DWORD style)
    {
        return SetToolStyleEx(GetToolStyleEx() | style);
    }
    // 删除工具条扩展样式, 返回原来的样式
    inline DWORD DelToolStyleEx(DWORD style)
    {
        return SetToolStyleEx(GetToolStyleEx() & ~style);
    }
    // 修改工具条扩展样式, 返回原来的样式
    inline DWORD ModifyToolStyleEx(DWORD style, BOOL isDel)
    {
        return SetToolStyleEx(GetToolStyleEx() & ~style);
    }
public:
    // 添加一个按钮, 这个方法是把结构展开成参数, 内部会把字符串添加到工具条的字符串池中
    inline INT_PTR InsertBtn(int pos, int ID, LPCWSTR title, int fsStyle = BTNS_BUTTON, int iconIndex = -1, LPCWSTR tipsText = 0, LONG_PTR param = 0, int state = TBSTATE_ENABLED) const
    {
        if (iconIndex   < 0 ) iconIndex = -1;
        if (fsStyle     <= 0) fsStyle = BTNS_BUTTON;

        TBBUTTON tb     = { 0 };
        tb.iString      = -1;
        tb.iBitmap      = iconIndex;
        tb.idCommand    = ID;
        tb.fsState      = state;
        tb.fsStyle      = fsStyle;
        tb.dwData       = param;
        if (title && title[0])
            tb.iString = (INT_PTR)title;
        return InsertBtn(pos, &tb);
    }
    // 添加一个按钮, 使用的是结构, 需要外部填充好结构, 返回新添加按钮的索引
    inline INT_PTR InsertBtn(int pos, const LPTBBUTTON lptb) const
    {
        if ( pos < 0 )
            pos = (int)Send(TB_BUTTONCOUNT, 0, 0);
        if ( Send(TB_INSERTBUTTONW, pos, (LPARAM)lptb) == 0 )
        {
            return -1;
        }
        return pos;
    }

    // 添加一个按钮
    // lptb = 按钮结构
    inline INT_PTR AddButton(const LPTBBUTTON lptb)
    {
        return AddButtons(1, lptb);
    }
    // 添加一组按钮, 结构需要外部填充好
    // numButtons = 按钮数量, 这个是 lptb 数组成员数
    // lptb = 按钮数组
    inline INT_PTR AddButtons(DWORD numButtons, const LPTBBUTTON lptb) const
    {
        return Send(TB_ADDBUTTONSW, numButtons, (LPARAM)lptb);
    }
    // 添加一组按钮, 结构需要外部填充好
    // numButtons = 按钮数量, 这个是 lptb 数组成员数
    // lptb = 按钮数组
    inline INT_PTR InsertBtns(DWORD numButtons, const LPTBBUTTON lptb)
    {
        return AddButtons(numButtons, lptb);
    }
    // 把文本数组添加到工具条的字符串池中, 返回字符串索引, 文本数组是每个成员\0结尾, 最后一个成员\0\0结尾
    inline int AddStrings(LPCWSTR text) const
    {
        return (int)Send(TB_ADDSTRINGW, 0, (LPARAM)text);
    }
    // 把文本添加到工具条的字符串池中, 返回字符串索引
    inline int AddString(LPCWSTR text) const
    {
        wstr str(tstr_len(text) + 2);   // 添加字符串需要两个结束标志, 不然会一直找到两个结束标志的才结束
        str.assign(text);               // 这里加多一个结束标志
        return (int)Send(TB_ADDSTRINGW, 0, (LPARAM)str.c_str());
    }
    inline INT_PTR GetBtnCount() const
    {
        return Send(TB_BUTTONCOUNT, 0, 0);
    }
    inline int GetBtnID(int id, BOOL isIndex) const
    {
        if (!isIndex)return id;
        TBBUTTON tb = { 0 };
        Send(TB_GETBUTTON, id, (LPARAM)&tb);
        return tb.idCommand;
    }

    // 设置工具栏按钮的命令标识符
    inline BOOL SetCmdID(int index, int ID) const
    {
        return Send(TB_SETCMDID, index, ID) != 0;
    }
    // 设置特定工具栏上工具栏按钮之间的距离
    inline void SetListgap(int nListgap) const
    {
        Send(TB_SETLISTGAP, nListgap, 0);
    }
    inline BOOL GetBtnData(int id, LPTBBUTTON lptb, BOOL isIndex = false) const
    {
        if ( !isIndex )
            id = (int)GetBtnIndex(id);
        return Send(TB_GETBUTTON, id, (LPARAM)lptb) != 0;
    }

    // 从按钮id获取按钮索引
    inline INT_PTR GetBtnIndex(int ID) const
    {
        return Send(TB_COMMANDTOINDEX, ID, 0);
    }
    inline BOOL InsertState(int id, int state, BOOL isIndex = false) const
    {
        if (isIndex)
            id = GetBtnID(id, isIndex);
        INT_PTR n = Send(TB_GETSTATE, id, 0);
        n |= state;
        return Send(TB_SETSTATE, id, n) != 0;
    }
    inline BOOL RemoveState(int id, int state, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        int n = (int)Send(TB_GETSTATE, id, 0);
        n &= ~state;
        return Send(TB_SETSTATE, id, n) != 0;
    }
    inline BOOL SetState(int id, int state, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        return Send(TB_SETSTATE, id, state) != 0;
    }
    inline INT_PTR GetState(int id, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        return Send(TB_GETSTATE, id, 0);
    }
    inline BOOL IsCheck(int id, BOOL isIndex = false) const
    {
        return IsState(id, TBSTATE_CHECKED);
    }
    inline BOOL SetCheck(int id, BOOL isCheck, BOOL isIndex = false) const
    {
        if (isCheck)
            return InsertState(id, TBSTATE_CHECKED, isIndex);
        return RemoveState(id, TBSTATE_CHECKED, isIndex);
    }

    // 判断指定按钮是否存在某个状态
    // state = TBSTATE_ 开头常量
    inline BOOL IsState(int id, int state, BOOL isIndex = false) const
    {
        UINT type = 0;
        switch (state)
        {
        case TBSTATE_CHECKED:
            type = TB_ISBUTTONCHECKED;
            break;
        case TBSTATE_PRESSED:
            type = TB_ISBUTTONPRESSED;
            break;
        case TBSTATE_ENABLED:
            type = TB_ISBUTTONENABLED;
            break;
        case TBSTATE_HIDDEN:
            type = TB_ISBUTTONHIDDEN;
            break;
        case TBSTATE_INDETERMINATE:
            type = TB_ISBUTTONINDETERMINATE;
            break;
        default:
            break;
        }
        id = GetBtnID(id, isIndex);
        return Send(type, id, 0) > 0;
    }
    // 删除指定按钮
    inline BOOL DeleteBtn(int id, BOOL isIndex = false) const
    {
        // 删除按钮使用的是索引, 如果传递进来的是索引, 那就不管, 不是索引, 那就获取索引
        if ( !isIndex )
            id = (int)GetBtnIndex(id);
        return Send(TB_DELETEBUTTON, id, 0) != 0;
    }
    // 获取按钮标题
    inline wstr GetBtnText(int id, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        INT_PTR dwLen = Send(TB_GETBUTTONTEXTW, id, 0) + 1;
        wstr str(dwLen);
        LPWSTR text = str.data();
        int b = (int)Send(TB_GETBUTTONTEXTW, id, (LPARAM)text) != -1;
        str.resize(wcslen(str.c_str()));
        return str;
    }
    // 获取按钮标题
    inline wstr GetTitle(int id, BOOL isIndex = false) const
    {
        return GetBtnText(id, isIndex);
    }
    // 设置按钮标题
    inline BOOL SetBtnText(int id, LPCWSTR title, BOOL isIndex = false) const
    {
        TBBUTTONINFOW tb = { 0 };
        tb.cbSize = sizeof(TBBUTTONINFOW);
        tb.dwMask = TBIF_TEXT;
        if (title)
            tb.pszText = (LPWSTR)title;
        
        id = GetBtnID(id, isIndex);
        return Send(TB_SETBUTTONINFOW, id, (LPARAM)&tb) != 0;
    }
    // 设置按钮标题
    inline BOOL SetTitle(int id, LPCWSTR title, BOOL isIndex = false) const
    {
        return SetBtnText(id, title, isIndex);
    }
    // 获取按钮尺寸
    inline SIZE GetBtnSize() const
    {
        INT_PTR size = Send(TB_GETBUTTONSIZE, 0, 0);
        SIZE si = { LOWORD(size), HIWORD(size) };
        return si;
    }
    // 设置按钮尺寸
    inline BOOL SetBtnSize(int w, int h) const
    {
        BOOL b = Send(TB_SETBUTTONSIZE, 0, MAKELONG(w, h)) != 0;
        Send(TB_AUTOSIZE, 0, 0);
        return b;
    }
    // 设置按钮数值
    inline BOOL SetBtnNum(int id, DWORD_PTR num, BOOL isIndex = false) const
    {
        return SetBtnParam(id, num, isIndex);
    }
    // 设置按钮数值
    inline BOOL SetBtnParam(int id, DWORD_PTR num, BOOL isIndex = false) const
    {
        TBBUTTONINFOW tb = { 0 };
        tb.cbSize = sizeof(TBBUTTONINFOW);
        tb.dwMask = TBIF_LPARAM | (isIndex ? TBIF_BYINDEX : 0);
        tb.lParam = num;
        //id = GetBtnID(id, isIndex);
        return Send(TB_SETBUTTONINFOW, id, (LPARAM)&tb) != 0;
    }
    // 获取按钮数值
    inline INT_PTR GetButtonNum(int id, BOOL isIndex = false) const
    {
        return GetButtonParam(id, isIndex);
    }
    // 获取按钮数值
    inline INT_PTR GetButtonParam(int id, BOOL isIndex = false) const
    {
        TBBUTTONINFOW tb = { 0 };
        tb.cbSize = sizeof(TBBUTTONINFOW);
        tb.dwMask = TBIF_LPARAM | (isIndex ? TBIF_BYINDEX : 0);
        //id = GetBtnID(id, isIndex);
        Send(TB_GETBUTTONINFOW, id, (LPARAM)&tb);
        return tb.lParam;
    }
    // 设置按钮宽度
    inline BOOL SetButtonWidth(int id, int w, BOOL isIndex = false) const
    {
        TBBUTTONINFOW tb = { 0 };
        tb.cbSize = sizeof(TBBUTTONINFOW);
        tb.dwMask = TBIF_SIZE | (isIndex ? TBIF_BYINDEX : 0);
        tb.cx = w;
        //id = GetBtnID(id, isIndex);
        return Send(TB_SETBUTTONINFOW, id, (LPARAM)&tb) != 0;
    }
    // 设置工具栏控件中的最小和最大按钮宽度
    inline BOOL SetButtonWidthMinMax(int minWidth, int maxWidth) const
    {
        return Send(TB_SETBUTTONWIDTH, 0, MAKELONG(minWidth, maxWidth)) != 0;
    }
    // 获取按钮宽度
    inline int GetButtonWidth(int id, BOOL isIndex = false) const
    {
        TBBUTTONINFOW tb = { 0 };
        tb.cbSize = sizeof(TBBUTTONINFOW);
        tb.dwMask = TBIF_SIZE | (isIndex ? TBIF_BYINDEX : 0);
        //id = GetBtnID(id, isIndex);
        Send(TB_GETBUTTONINFOW, id, (LPARAM)&tb);
        return tb.cx;
    }
    // 获取工具栏的下拉窗口的边界矩形
    inline void GetItemDropDownRect(int id, PRECT prc, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        Send(TB_GETITEMDROPDOWNRECT, GetBtnIndex(id), (LPARAM)prc);
        return;
    }
    // 获取工具栏按钮的边界矩形
    inline void GetItemRect(int id, PRECT prc, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        Send(TB_GETITEMRECT, GetBtnIndex(id), (LPARAM)prc);
        return;
    }
    // 检索工具栏中所有可见按钮和分隔符的总大小
    inline BOOL GetMaxSize(SIZE* psize) const
    {
        return Send(TB_GETMAXSIZE, 0, (LPARAM)psize) != 0;
    }
    // 设置工具栏控件中第一个按钮的缩进
    inline BOOL SetIndent(int nIndent) const
    {
        return Send(TB_SETINDENT, nIndent, 0) != 0;
    }
    // 设置图片组, 返回之前的图片组句柄, 第二个参数指定设置什么状态, 0=正常, 1=热点, 2=禁止
    inline HIMAGELIST SetImgList(HIMAGELIST hIml, int type = 0) const
    {
        UINT msg = type == 0 ? TB_SETIMAGELIST : type == 1 ? TB_SETHOTIMAGELIST : TB_SETDISABLEDIMAGELIST;
        return (HIMAGELIST)Send(msg, 0, (LPARAM)hIml);
    }
    // 设置图片组, 返回之前的图片组句柄
    inline HIMAGELIST SetImageList(HIMAGELIST hIml) const
    {
        return (HIMAGELIST)Send(TB_SETIMAGELIST, 0, (LPARAM)hIml);
    }
    // 设置热点图片组, 返回之前的图片组句柄
    inline HIMAGELIST SetImageListHot(HIMAGELIST hIml) const
    {
        return (HIMAGELIST)Send(TB_SETHOTIMAGELIST, 0, (LPARAM)hIml);
    }
    // 设置禁止热点图片组, 返回之前的图片组句柄
    inline HIMAGELIST SetImageListBan(HIMAGELIST hIml) const
    {
        return (HIMAGELIST)Send(TB_SETDISABLEDIMAGELIST, 0, (LPARAM)hIml);
    }
    // 启动按钮, isEnabled, 0=禁止, 1=启用, -1=取反
    inline BOOL EnabledButton(int id, int isEnbaled, BOOL isIndex = false)
    {
        // TB_ENABLEBUTTON
        if (isIndex) id = GetBtnID(id, isIndex);
        if (isEnbaled == -1)
            isEnbaled = IsEbabledButton(id, isIndex) ? 0 : 1;

        return Send(TB_ENABLEBUTTON, id, isEnbaled) != 0;
    }
    // 按钮是否启用, 返回true = 启用, false = 禁止
    inline BOOL IsEbabledButton(int id, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        int state = (int)Send(TB_GETSTATE, id, 0);
        if (state == -1)
            return false;
        return (state & TBSTATE_INDETERMINATE) == TBSTATE_INDETERMINATE;
    }
    // 获取按钮矩形
    inline BOOL GetBtnRect(int id, PRECT prc, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        return Send(TB_GETRECT, id, (LPARAM)prc) != 0;
    }

};

