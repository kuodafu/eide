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
            Send(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);    // ����BUTTON�ṹ��С
            Send(TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_DOUBLEBUFFER);
        }
        return hWnd;
    }
    // ���ù�������չ��ʽ, ����ԭ������ʽ
    inline DWORD SetToolStyleEx(DWORD style) const
    {
        return (DWORD)Send(TB_SETEXTENDEDSTYLE, 0, (LPARAM)style);
    }
    // ��ȡ��������չ��ʽ, ����ԭ������ʽ
    inline DWORD GetToolStyleEx() const
    {
        return (DWORD)Send(TB_GETEXTENDEDSTYLE, 0, (LPARAM)0);
    }
    // ��ӹ�������չ��ʽ, ����ԭ������ʽ
    inline DWORD AddToolStyleEx(DWORD style)
    {
        return SetToolStyleEx(GetToolStyleEx() | style);
    }
    // ɾ����������չ��ʽ, ����ԭ������ʽ
    inline DWORD DelToolStyleEx(DWORD style)
    {
        return SetToolStyleEx(GetToolStyleEx() & ~style);
    }
    // �޸Ĺ�������չ��ʽ, ����ԭ������ʽ
    inline DWORD ModifyToolStyleEx(DWORD style, BOOL isDel)
    {
        return SetToolStyleEx(GetToolStyleEx() & ~style);
    }
public:
    // ���һ����ť, ��������ǰѽṹչ���ɲ���, �ڲ�����ַ�����ӵ����������ַ�������
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
    // ���һ����ť, ʹ�õ��ǽṹ, ��Ҫ�ⲿ���ýṹ, ��������Ӱ�ť������
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

    // ���һ����ť
    // lptb = ��ť�ṹ
    inline INT_PTR AddButton(const LPTBBUTTON lptb)
    {
        return AddButtons(1, lptb);
    }
    // ���һ�鰴ť, �ṹ��Ҫ�ⲿ����
    // numButtons = ��ť����, ����� lptb �����Ա��
    // lptb = ��ť����
    inline INT_PTR AddButtons(DWORD numButtons, const LPTBBUTTON lptb) const
    {
        return Send(TB_ADDBUTTONSW, numButtons, (LPARAM)lptb);
    }
    // ���һ�鰴ť, �ṹ��Ҫ�ⲿ����
    // numButtons = ��ť����, ����� lptb �����Ա��
    // lptb = ��ť����
    inline INT_PTR InsertBtns(DWORD numButtons, const LPTBBUTTON lptb)
    {
        return AddButtons(numButtons, lptb);
    }
    // ���ı�������ӵ����������ַ�������, �����ַ�������, �ı�������ÿ����Ա\0��β, ���һ����Ա\0\0��β
    inline int AddStrings(LPCWSTR text) const
    {
        return (int)Send(TB_ADDSTRINGW, 0, (LPARAM)text);
    }
    // ���ı���ӵ����������ַ�������, �����ַ�������
    inline int AddString(LPCWSTR text) const
    {
        wstr str(tstr_len(text) + 2);   // ����ַ�����Ҫ����������־, ��Ȼ��һֱ�ҵ�����������־�ĲŽ���
        str.assign(text);               // ����Ӷ�һ��������־
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

    // ���ù�������ť�������ʶ��
    inline BOOL SetCmdID(int index, int ID) const
    {
        return Send(TB_SETCMDID, index, ID) != 0;
    }
    // �����ض��������Ϲ�������ť֮��ľ���
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

    // �Ӱ�ťid��ȡ��ť����
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

    // �ж�ָ����ť�Ƿ����ĳ��״̬
    // state = TBSTATE_ ��ͷ����
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
    // ɾ��ָ����ť
    inline BOOL DeleteBtn(int id, BOOL isIndex = false) const
    {
        // ɾ����ťʹ�õ�������, ������ݽ�����������, �ǾͲ���, ��������, �Ǿͻ�ȡ����
        if ( !isIndex )
            id = (int)GetBtnIndex(id);
        return Send(TB_DELETEBUTTON, id, 0) != 0;
    }
    // ��ȡ��ť����
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
    // ��ȡ��ť����
    inline wstr GetTitle(int id, BOOL isIndex = false) const
    {
        return GetBtnText(id, isIndex);
    }
    // ���ð�ť����
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
    // ���ð�ť����
    inline BOOL SetTitle(int id, LPCWSTR title, BOOL isIndex = false) const
    {
        return SetBtnText(id, title, isIndex);
    }
    // ��ȡ��ť�ߴ�
    inline SIZE GetBtnSize() const
    {
        INT_PTR size = Send(TB_GETBUTTONSIZE, 0, 0);
        SIZE si = { LOWORD(size), HIWORD(size) };
        return si;
    }
    // ���ð�ť�ߴ�
    inline BOOL SetBtnSize(int w, int h) const
    {
        BOOL b = Send(TB_SETBUTTONSIZE, 0, MAKELONG(w, h)) != 0;
        Send(TB_AUTOSIZE, 0, 0);
        return b;
    }
    // ���ð�ť��ֵ
    inline BOOL SetBtnNum(int id, DWORD_PTR num, BOOL isIndex = false) const
    {
        return SetBtnParam(id, num, isIndex);
    }
    // ���ð�ť��ֵ
    inline BOOL SetBtnParam(int id, DWORD_PTR num, BOOL isIndex = false) const
    {
        TBBUTTONINFOW tb = { 0 };
        tb.cbSize = sizeof(TBBUTTONINFOW);
        tb.dwMask = TBIF_LPARAM | (isIndex ? TBIF_BYINDEX : 0);
        tb.lParam = num;
        //id = GetBtnID(id, isIndex);
        return Send(TB_SETBUTTONINFOW, id, (LPARAM)&tb) != 0;
    }
    // ��ȡ��ť��ֵ
    inline INT_PTR GetButtonNum(int id, BOOL isIndex = false) const
    {
        return GetButtonParam(id, isIndex);
    }
    // ��ȡ��ť��ֵ
    inline INT_PTR GetButtonParam(int id, BOOL isIndex = false) const
    {
        TBBUTTONINFOW tb = { 0 };
        tb.cbSize = sizeof(TBBUTTONINFOW);
        tb.dwMask = TBIF_LPARAM | (isIndex ? TBIF_BYINDEX : 0);
        //id = GetBtnID(id, isIndex);
        Send(TB_GETBUTTONINFOW, id, (LPARAM)&tb);
        return tb.lParam;
    }
    // ���ð�ť���
    inline BOOL SetButtonWidth(int id, int w, BOOL isIndex = false) const
    {
        TBBUTTONINFOW tb = { 0 };
        tb.cbSize = sizeof(TBBUTTONINFOW);
        tb.dwMask = TBIF_SIZE | (isIndex ? TBIF_BYINDEX : 0);
        tb.cx = w;
        //id = GetBtnID(id, isIndex);
        return Send(TB_SETBUTTONINFOW, id, (LPARAM)&tb) != 0;
    }
    // ���ù������ؼ��е���С�����ť���
    inline BOOL SetButtonWidthMinMax(int minWidth, int maxWidth) const
    {
        return Send(TB_SETBUTTONWIDTH, 0, MAKELONG(minWidth, maxWidth)) != 0;
    }
    // ��ȡ��ť���
    inline int GetButtonWidth(int id, BOOL isIndex = false) const
    {
        TBBUTTONINFOW tb = { 0 };
        tb.cbSize = sizeof(TBBUTTONINFOW);
        tb.dwMask = TBIF_SIZE | (isIndex ? TBIF_BYINDEX : 0);
        //id = GetBtnID(id, isIndex);
        Send(TB_GETBUTTONINFOW, id, (LPARAM)&tb);
        return tb.cx;
    }
    // ��ȡ���������������ڵı߽����
    inline void GetItemDropDownRect(int id, PRECT prc, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        Send(TB_GETITEMDROPDOWNRECT, GetBtnIndex(id), (LPARAM)prc);
        return;
    }
    // ��ȡ��������ť�ı߽����
    inline void GetItemRect(int id, PRECT prc, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        Send(TB_GETITEMRECT, GetBtnIndex(id), (LPARAM)prc);
        return;
    }
    // ���������������пɼ���ť�ͷָ������ܴ�С
    inline BOOL GetMaxSize(SIZE* psize) const
    {
        return Send(TB_GETMAXSIZE, 0, (LPARAM)psize) != 0;
    }
    // ���ù������ؼ��е�һ����ť������
    inline BOOL SetIndent(int nIndent) const
    {
        return Send(TB_SETINDENT, nIndent, 0) != 0;
    }
    // ����ͼƬ��, ����֮ǰ��ͼƬ����, �ڶ�������ָ������ʲô״̬, 0=����, 1=�ȵ�, 2=��ֹ
    inline HIMAGELIST SetImgList(HIMAGELIST hIml, int type = 0) const
    {
        UINT msg = type == 0 ? TB_SETIMAGELIST : type == 1 ? TB_SETHOTIMAGELIST : TB_SETDISABLEDIMAGELIST;
        return (HIMAGELIST)Send(msg, 0, (LPARAM)hIml);
    }
    // ����ͼƬ��, ����֮ǰ��ͼƬ����
    inline HIMAGELIST SetImageList(HIMAGELIST hIml) const
    {
        return (HIMAGELIST)Send(TB_SETIMAGELIST, 0, (LPARAM)hIml);
    }
    // �����ȵ�ͼƬ��, ����֮ǰ��ͼƬ����
    inline HIMAGELIST SetImageListHot(HIMAGELIST hIml) const
    {
        return (HIMAGELIST)Send(TB_SETHOTIMAGELIST, 0, (LPARAM)hIml);
    }
    // ���ý�ֹ�ȵ�ͼƬ��, ����֮ǰ��ͼƬ����
    inline HIMAGELIST SetImageListBan(HIMAGELIST hIml) const
    {
        return (HIMAGELIST)Send(TB_SETDISABLEDIMAGELIST, 0, (LPARAM)hIml);
    }
    // ������ť, isEnabled, 0=��ֹ, 1=����, -1=ȡ��
    inline BOOL EnabledButton(int id, int isEnbaled, BOOL isIndex = false)
    {
        // TB_ENABLEBUTTON
        if (isIndex) id = GetBtnID(id, isIndex);
        if (isEnbaled == -1)
            isEnbaled = IsEbabledButton(id, isIndex) ? 0 : 1;

        return Send(TB_ENABLEBUTTON, id, isEnbaled) != 0;
    }
    // ��ť�Ƿ�����, ����true = ����, false = ��ֹ
    inline BOOL IsEbabledButton(int id, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        int state = (int)Send(TB_GETSTATE, id, 0);
        if (state == -1)
            return false;
        return (state & TBSTATE_INDETERMINATE) == TBSTATE_INDETERMINATE;
    }
    // ��ȡ��ť����
    inline BOOL GetBtnRect(int id, PRECT prc, BOOL isIndex = false) const
    {
        id = GetBtnID(id, isIndex);
        return Send(TB_GETRECT, id, (LPARAM)prc) != 0;
    }

};

