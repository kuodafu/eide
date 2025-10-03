#pragma once
#include "WndBase.h"
class CMyEdit : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return L"edit"; }

public:
    CMyEdit() :CControlBase() { ; }
    virtual ~CMyEdit() { ; }
public:
    virtual inline LPCWSTR className() { return CLASS_EDIT; }

public:

    // 加入文本, 在编辑框最后位置加入
    inline void JoinText(LPCWSTR lpsz) {
        int len = (int)(DWORD)Send(WM_GETTEXTLENGTH, 0, 0);
        SetSel(len, len);
        ReplaceSel(lpsz);
    }

    // 插入文本, 在编辑框当前光标位置加入文本
    inline void InsertText(LPCWSTR lpsz) {
        ReplaceSel(lpsz);
    }

public:

#ifndef ECM_FIRST
#define ECM_FIRST               0x1500 
#endif
#ifndef EM_GETCARETINDEX
#define EM_GETCARETINDEX  (ECM_FIRST + 18)
#endif
#ifndef EM_SETCARETINDEX
#define EM_SETCARETINDEX        (ECM_FIRST + 17)
#endif
#ifndef EM_GETCUEBANNER
#define EM_GETCUEBANNER  (ECM_FIRST + 2)
#endif
#ifndef EM_GETEXTENDEDSTYLE
#define EM_GETEXTENDEDSTYLE     (ECM_FIRST + 11)
#endif
#ifndef EM_SETEXTENDEDSTYLE
#define EM_SETEXTENDEDSTYLE     (ECM_FIRST + 10)
#endif
#ifndef EM_HIDEBALLOONTIP
#define EM_HIDEBALLOONTIP   (ECM_FIRST + 4)
#endif
#ifndef EM_SHOWBALLOONTIP
#define EM_SHOWBALLOONTIP   (ECM_FIRST + 3) 
#endif
    // 获取编辑控件中插入符号位置的从零开始的索引, 返回值是插入符号位置的从零开始的索引值
    inline DWORD GetCaretIndex() const
    {
        return (DWORD)Send(EM_GETCARETINDEX, 0, 0);
    }

    // 取在编辑控件中显示为文本提示或提示的文本
    inline wstr GetCueBannerText() const
    {
        wstr ret(0x1000);
        Send(EM_GETCUEBANNER, (WPARAM)ret.data(), 0x1000);
        ret.resize(wcslen(ret.c_str()));
        return ret;
    }

    // 获取指定编辑控件的扩展样式
    // 编辑控件的扩展样式与CreateWindowEx函数或SetWindowLong函数使用的扩展样式无关
    // ES_EX_ 开头常量
    inline DWORD GetExtendedStyle() const
    {
        return (DWORD)Send(EM_GETEXTENDEDSTYLE, 0, 0);
    }

    // 获取多行编辑或丰富编辑控件中最上方可见行的索引
    // 返回值是多行编辑控件中最上方可见行的从零开始的索引
    // 对于单行编辑控件，返回值是第一个可见字符的从零开始的索引
    // 对于单行丰富的编辑控件，返回值为零
    // 编辑控件中的行数和行长取决于控件的宽度和当前的自动换行设置
    inline int GetFirstVisibleLine() const
    {
        return Edit_GetFirstVisibleLine(m_hWnd);
    }

    // 返回值是一个内存句柄，用于标识保存编辑控件内容的缓冲区。如果发生错误（例如，将消息发送到单行编辑控件），则返回值为零
    // 程序可以调用 LocalLock() 来访问编辑控件的内容, LocalLock返回一个指向缓冲区的指针
    // 该缓冲区是NULL终止的CHAR或WCHAR数组, 具体取决于是由ANSI还是Unicode函数创建的控件
    // 通过参数 isUnicode 来接收是否为Unicode wchar_t*
    // 调用 LocalLock 后必须调用 LocalUnlock 
    inline HLOCAL GetHandle(BOOL* isUnicode) const
    {
        if (isUnicode) *isUnicode = IsWindowUnicode(m_hWnd);
        HLOCAL hMem = Edit_GetHandle(m_hWnd);
        return hMem;
    }

    // 从编辑或丰富编辑控件中检索一行文本
    inline wstr GetLine(int line) const
    {
        wstr ret(0x1000);
        Edit_GetLine(m_hWnd, line, ret.data(), 0x1000);
        ret.resize(wcslen(ret.c_str()));
        return ret;
    }

    // 获取编辑控件文本中的行数
    inline int GetLineCount() const
    {
        return Edit_GetLineCount(m_hWnd);
    }

    // 获取编辑或丰富编辑控件的修改标志的状态, 该标志指示控件的内容是否已被修改
    inline BOOL GetModify() const
    {
        return Edit_GetModify(m_hWnd);
    }

    // 获取用于编辑或丰富编辑控件的密码字符
    inline wchar_t GetPasswordChar() const
    {
        return Edit_GetPasswordChar(m_hWnd);
    }

    // 获取编辑控件的格式矩形
    inline RECT GetRect() const
    {
        RECT rc = { 0 };
        Edit_GetRect(m_hWnd, &rc);
        return rc;
    }

    // 获取编辑或丰富编辑控件中当前所选内容的开始和结束字符位置
    inline DWORD GetSel(DWORD* start = 0, DWORD* end = 0) const
    {
        return (DWORD)(DWORD_PTR)Send(EM_GETSEL, (WPARAM)start, (LPARAM)end);
    }
    // 获取编辑框数值, 把文本转换成数值
    inline __int64 GetTextNumber() const
    {
        return _wtoi64(GetText().c_str());
    }
    // 获取编辑控件的文本
    inline wstr GetText() const
    {
        return GetWindowTextW();
    }

    // 获取编辑控件的文本
    inline _str GetTextA() const
    {
        return GetWindowTextA();
    }

    // 获取编辑控件文本中的字符数
    inline int GetTextLength() const
    {
        return GetWindowTextLength(m_hWnd);
    }

    // 获取编辑控件文本中的字符数
    inline int GetTextLengthW() const
    {
        return GetWindowTextLengthW(m_hWnd);
    }

    // 获取编辑控件文本中的字符数
    inline int GetTextLengthA()
    {
        return GetWindowTextLengthA(m_hWnd);
    }

    // 检索编辑或丰富编辑控件的Wordwrap函数的地址
    inline EDITWORDBREAKPROC GetWordBreakProc() const
    {
        return Edit_GetWordBreakProc(m_hWnd);
    }

    //// 获取当前缩放比例，该比例始终在1/64到64之间
    //// Windows 10 1809和更高版本中受支持。编辑控件需要设置ES_EX_ZOOMABLE扩展样式，此消息才能生效
    //inline BOOL GetZoom(int numerator, int denominator) {
    //    return Edit_GetZoom(m_hWnd, numerator, denominator);
    //}
    
    // 隐藏与编辑控件关联的所有气球提示
    inline BOOL HideBalloonTip() const
    {
        return Send(EM_HIDEBALLOONTIP, 0, 0) != FALSE;
    }

    // 限制可以输入到编辑控件中的文本的长度
    inline void LimitText(int cchMax) const
    {
        Edit_LimitText(m_hWnd, cchMax);
    }

    // 在多行编辑或丰富编辑控件中获取包含指定字符索引的行的索引, 返回值是包含wParam指定的字符索引的行的从零开始的行号
    // ich = 要检索其编号的行中包含的字符的字符索引。如果此参数为-1，则EM_LINEFROMCHAR检索当前行（包含插入号的行）的行号，或者如果存在选择，则检索包含选择的开始的行的行号
    inline int LineFromChar(int ich) const
    {
        return Edit_LineFromChar(m_hWnd, ich);
    }

    // 获取多行编辑或丰富编辑控件中指定行的第一个字符的字符索引
    // line = 从零开始的行号, 值-1指定当前行号（包含插入号的行）
    inline int LineIndex(int line) const
    {
        return Edit_LineIndex(m_hWnd, line);
    }

    // 在编辑或丰富编辑控件中检索一行的长度（以字符为单位）
    // 于多行编辑控件，返回值是wParam参数指定的行的长度（以TCHAR为单位）。对于ANSI文本，这是字节数。对于Unicode文本，这是字符数。该行的末尾不包含回车符。
    // 对于单行编辑控件，返回值是编辑控件中文本的长度（以TCHAR为单位）
    // 如果line大于控件中的字符数，则返回值为零
    // line = 要获取其长度的行中字符的字符索引。如果此参数大于控件中的字符数，则返回值为零
    // 此参数可以为-1。在这种情况下，该消息将返回包含选定字符的行上未选定字符的数量。例如，如果选择从一行的第四个字符扩展到下一行的第八个字符，则返回值为10（第一行三个字符，下一行七个）
    inline int LineLength(int line) const
    {
        return Edit_LineLength(m_hWnd, line);
    }


    // lpszReplace = 指向包含替换文本的以空字符结尾的字符串的指针
    // canUndone = 指定是否可以撤消替换操作。如果为TRUE，则可以撤消该操作。如果为FALSE，则操作无法撤消
    inline void ReplaceSel(LPCWSTR lpszReplace, BOOL canUndone = false) const
    {
        Send(EM_REPLACESEL, (WPARAM)canUndone, (LPARAM)lpszReplace);
    }

    // 在多行编辑或丰富编辑控件中垂直滚动文本
    // dv = 垂直滚值, dh = 水平滚动的值
    inline void Scroll(int dv, int dh) const
    {
        Edit_Scroll(m_hWnd, dv, dh);
    }

    // 在编辑或丰富编辑控件中将插入符滚动到视图中
    inline BOOL ScrollCaret() const
    {
        return Edit_ScrollCaret(m_hWnd);
    }

    // 设置编辑控件中插入符号位置的从零开始的索引
    // newCaretIndex = 字符索引
    inline BOOL SetCaretIndex(int newCaretIndex) const
    {
        return (BOOL)Send(EM_SETCARETINDEX, (WPARAM)(newCaretIndex), 0);
    }

    // 设置指定编辑控件的扩展样式
    // 编辑控件的扩展样式与CreateWindowEx函数或SetWindowLong函数使用的扩展样式无关
    // dwStyle = 要设置的扩展编辑控件样式
    // dwMask = 该值指定要受影响的样式
    inline BOOL SetExtendedStyle(DWORD dwStyle, DWORD dwMask) const
    {
        return Send(EM_SETEXTENDEDSTYLE, dwMask, dwStyle) != FALSE;
    }

    // 在应用程序设置新的内存句柄之前，应该调用 GetHandle() 获取当前内存缓冲区的句柄并释放该内存
    // 编辑控件在需要额外的文本空间时会自动重新分配给定的缓冲区，或者它会删除足够的文本，从而不再需要额外的空间
    // 设置多行编辑控件将使用的内存句柄
    // hMem = 编辑控件使用的内存缓冲区句柄，用于存储当前显示的文本，而不是分配自己的内存。如有必要，控件将重新分配此内存
    inline void SetHandle(HLOCAL hMem) const
    {
        Edit_SetHandle(m_hWnd, hMem);
    }

    // 设置或清除编辑控件的修改标志。修改标志指示编辑控件中的文本是否已被修改
    // fModified = 修改标志的新值。值为TRUE表示文本已被修改，值为FALSE表示文本尚未被修改
    inline void SetModify(BOOL fModified) const
    {
        Edit_SetModify(m_hWnd, fModified);
    }

    // 设置或删除用于编辑或丰富编辑控件的密码字符。设置密码字符后，将显示该字符来代替用户键入的字符
    // ch = 要显示的字符代替用户键入的字符。如果此参数为零，则控件将删除当前密码字符并显示用户键入的字符
    inline void SetPasswordChar(wchar_t ch) const
    {
        Edit_SetPasswordChar(m_hWnd, ch);
    }

    // 设置或删除编辑或丰富编辑控件的只读样式（ES_READONLY）
    // fReadOnly = 指定是设置还是删除ES_READONLY样式。值为TRUE设置ES_READONLY样式；值为FALSE会删除ES_READONLY样式
    inline BOOL SetReadOnly(BOOL fReadOnly) const
    {
        return Edit_SetReadOnly(m_hWnd, fReadOnly);
    }

    // 设置编辑控件的格式化矩形
    inline void SetRect(const RECT& rc) const
    {
        Edit_SetRect(m_hWnd, &rc);
    }

    // 设置多行编辑控件的格式矩形。该方法等效于SetRect，只是它不会重绘编辑控件窗口
    inline void SetRectNoPaint(const RECT& rc) const
    {
        Edit_SetRectNoPaint(m_hWnd, &rc);
    }

    // 在编辑或丰富编辑控件中选择字符范围
    // ichStart = 选择的起始字符位置
    // ichEnd = 所选内容的结束字符位置
    inline void SetSel(int ichStart, int ichEnd) const
    {
        Edit_SetSel(m_hWnd, ichStart, ichEnd);
        Edit_ScrollCaret(m_hWnd);
    }

    // 在多行编辑或丰富编辑控件中设置制表位。将文本复制到控件时，文本中的任何制表符都会导致生成空间，直到下一个制表位
    // cTabs = 数组中包含的制表位的数量。如果此参数为零，则lpTabs参数将被忽略，并且默认的制表位在每32个对话框模板单元中设置一次
    //      如果此参数为1，则每隔n个对话框模板单位设置一个制表位，其中n是lpTabs参数指向的距离。如果此参数大于1，则lpTabs是指向制表位数组的指针
    // lpTabs = 指向指定制表符的无符号整数数组的指针，以对话框模板为单位。如果cTabs为1，则此参数是指向无符号整数的指针，该整数包含所有制表位之间的距离，以对话框模板为单位
    inline void SetTabStops(int cTabs, int* lpTabs) const
    {
        Edit_SetTabStops(m_hWnd, cTabs, lpTabs);
    }

    // 设置编辑控件的文本
    inline BOOL SetText(UINT uValue, BOOL bSigned) const
    {
        const LPCWSTR fmt = bSigned ? L"%u" : L"%d";
        wchar_t buf[20];
#if _MSC_VER > 1200
        swprintf_s(buf, 20, fmt, uValue);
#else
        swprintf(buf, fmt, uValue);
#endif
        return SetWindowTextW(buf);
    }

    // 设置编辑控件的文本
    inline BOOL SetText(LPCWSTR lpsz) const
    {
        return SetWindowTextW(lpsz);
    }

    // 设置编辑控件的文本
    inline BOOL SetTextA(LPCSTR lpsz) const 
    {
        return SetWindowTextA(lpsz);
    }

    // 置W版窗口标题, 编辑框需要重新这个方法, 否则部分系统无法设置标题
    inline BOOL SetWindowTextW(LPCWSTR lpString) const
    {
        if ( !m_hWnd )return false;
        return Send(WM_SETTEXT, 0, (LPARAM)lpString) != 0;
    }

    // 置A版窗口标题, 编辑框需要重新这个方法, 否则部分系统无法设置标题
    inline BOOL SetWindowTextA(LPCSTR lpString) const
    {
        if ( !m_hWnd )return false;
        return SendA(WM_SETTEXT, 0, (LPARAM)lpString) != 0;
    }

    // 用应用程序定义的Wordwrap函数替换编辑控件的默认Wordwrap函数
    // lpfnWordBreak = 应用程序定义的自动换行功能的地址。有关换行的更多信息，请参见EditWordBreakProc回调函数的描述
    inline void SetWordBreakProc(EDITWORDBREAKPROC lpfnWordBreak) {
        Edit_SetWordBreakProc(m_hWnd, lpfnWordBreak);
    }

#if _MSC_VER < 1201
    typedef struct _tagEDITBALLOONTIP
    {
        DWORD   cbStruct;
        LPCWSTR pszTitle;
        LPCWSTR pszText;
        INT     ttiIcon; // From TTI_*
    } EDITBALLOONTIP, * PEDITBALLOONTIP;
#endif

    // 显示与编辑控件关联的气球提示
    // peditballoontip = 指向EDITBALLOONTIP结构的指针，该结构包含有关要显示的气球提示的信息
    inline BOOL ShowBalloonTip(PEDITBALLOONTIP peditballoontip) const
    {
        return Send(EM_SHOWBALLOONTIP, 0, (LPARAM)peditballoontip) != FALSE;
    }

    // 撤消编辑或丰富编辑控件的撤消队列中的最后一个操作
    inline BOOL Undo() const
    {
        return Edit_Undo(m_hWnd);
    }
    // 确定编辑或丰富编辑控件的撤消队列中是否有任何操作
    inline BOOL CanUndo() const
    {
        return Edit_CanUndo(m_hWnd);
    }
    // 确定编辑或丰富编辑控件的撤消队列中是否有任何操作
    inline BOOL IsUndo() const
    {
        return CanUndo();
    }
    
};

