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

    // �����ı�, �ڱ༭�����λ�ü���
    inline void JoinText(LPCWSTR lpsz) {
        int len = (int)(DWORD)Send(WM_GETTEXTLENGTH, 0, 0);
        SetSel(len, len);
        ReplaceSel(lpsz);
    }

    // �����ı�, �ڱ༭��ǰ���λ�ü����ı�
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
    // ��ȡ�༭�ؼ��в������λ�õĴ��㿪ʼ������, ����ֵ�ǲ������λ�õĴ��㿪ʼ������ֵ
    inline DWORD GetCaretIndex() const
    {
        return (DWORD)Send(EM_GETCARETINDEX, 0, 0);
    }

    // ȡ�ڱ༭�ؼ�����ʾΪ�ı���ʾ����ʾ���ı�
    inline wstr GetCueBannerText() const
    {
        wstr ret(0x1000);
        Send(EM_GETCUEBANNER, (WPARAM)ret.data(), 0x1000);
        ret.resize(wcslen(ret.c_str()));
        return ret;
    }

    // ��ȡָ���༭�ؼ�����չ��ʽ
    // �༭�ؼ�����չ��ʽ��CreateWindowEx������SetWindowLong����ʹ�õ���չ��ʽ�޹�
    // ES_EX_ ��ͷ����
    inline DWORD GetExtendedStyle() const
    {
        return (DWORD)Send(EM_GETEXTENDEDSTYLE, 0, 0);
    }

    // ��ȡ���б༭��ḻ�༭�ؼ������Ϸ��ɼ��е�����
    // ����ֵ�Ƕ��б༭�ؼ������Ϸ��ɼ��еĴ��㿪ʼ������
    // ���ڵ��б༭�ؼ�������ֵ�ǵ�һ���ɼ��ַ��Ĵ��㿪ʼ������
    // ���ڵ��зḻ�ı༭�ؼ�������ֵΪ��
    // �༭�ؼ��е��������г�ȡ���ڿؼ��Ŀ�Ⱥ͵�ǰ���Զ���������
    inline int GetFirstVisibleLine() const
    {
        return Edit_GetFirstVisibleLine(m_hWnd);
    }

    // ����ֵ��һ���ڴ��������ڱ�ʶ����༭�ؼ����ݵĻ���������������������磬����Ϣ���͵����б༭�ؼ������򷵻�ֵΪ��
    // ������Ե��� LocalLock() �����ʱ༭�ؼ�������, LocalLock����һ��ָ�򻺳�����ָ��
    // �û�������NULL��ֹ��CHAR��WCHAR����, ����ȡ��������ANSI����Unicode���������Ŀؼ�
    // ͨ������ isUnicode �������Ƿ�ΪUnicode wchar_t*
    // ���� LocalLock �������� LocalUnlock 
    inline HLOCAL GetHandle(BOOL* isUnicode) const
    {
        if (isUnicode) *isUnicode = IsWindowUnicode(m_hWnd);
        HLOCAL hMem = Edit_GetHandle(m_hWnd);
        return hMem;
    }

    // �ӱ༭��ḻ�༭�ؼ��м���һ���ı�
    inline wstr GetLine(int line) const
    {
        wstr ret(0x1000);
        Edit_GetLine(m_hWnd, line, ret.data(), 0x1000);
        ret.resize(wcslen(ret.c_str()));
        return ret;
    }

    // ��ȡ�༭�ؼ��ı��е�����
    inline int GetLineCount() const
    {
        return Edit_GetLineCount(m_hWnd);
    }

    // ��ȡ�༭��ḻ�༭�ؼ����޸ı�־��״̬, �ñ�־ָʾ�ؼ��������Ƿ��ѱ��޸�
    inline BOOL GetModify() const
    {
        return Edit_GetModify(m_hWnd);
    }

    // ��ȡ���ڱ༭��ḻ�༭�ؼ��������ַ�
    inline wchar_t GetPasswordChar() const
    {
        return Edit_GetPasswordChar(m_hWnd);
    }

    // ��ȡ�༭�ؼ��ĸ�ʽ����
    inline RECT GetRect() const
    {
        RECT rc = { 0 };
        Edit_GetRect(m_hWnd, &rc);
        return rc;
    }

    // ��ȡ�༭��ḻ�༭�ؼ��е�ǰ��ѡ���ݵĿ�ʼ�ͽ����ַ�λ��
    inline DWORD GetSel(DWORD* start = 0, DWORD* end = 0) const
    {
        return (DWORD)(DWORD_PTR)Send(EM_GETSEL, (WPARAM)start, (LPARAM)end);
    }
    // ��ȡ�༭����ֵ, ���ı�ת������ֵ
    inline __int64 GetTextNumber() const
    {
        return _wtoi64(GetText().c_str());
    }
    // ��ȡ�༭�ؼ����ı�
    inline wstr GetText() const
    {
        return GetWindowTextW();
    }

    // ��ȡ�༭�ؼ����ı�
    inline _str GetTextA() const
    {
        return GetWindowTextA();
    }

    // ��ȡ�༭�ؼ��ı��е��ַ���
    inline int GetTextLength() const
    {
        return GetWindowTextLength(m_hWnd);
    }

    // ��ȡ�༭�ؼ��ı��е��ַ���
    inline int GetTextLengthW() const
    {
        return GetWindowTextLengthW(m_hWnd);
    }

    // ��ȡ�༭�ؼ��ı��е��ַ���
    inline int GetTextLengthA()
    {
        return GetWindowTextLengthA(m_hWnd);
    }

    // �����༭��ḻ�༭�ؼ���Wordwrap�����ĵ�ַ
    inline EDITWORDBREAKPROC GetWordBreakProc() const
    {
        return Edit_GetWordBreakProc(m_hWnd);
    }

    //// ��ȡ��ǰ���ű������ñ���ʼ����1/64��64֮��
    //// Windows 10 1809�͸��߰汾����֧�֡��༭�ؼ���Ҫ����ES_EX_ZOOMABLE��չ��ʽ������Ϣ������Ч
    //inline BOOL GetZoom(int numerator, int denominator) {
    //    return Edit_GetZoom(m_hWnd, numerator, denominator);
    //}
    
    // ������༭�ؼ�����������������ʾ
    inline BOOL HideBalloonTip() const
    {
        return Send(EM_HIDEBALLOONTIP, 0, 0) != FALSE;
    }

    // ���ƿ������뵽�༭�ؼ��е��ı��ĳ���
    inline void LimitText(int cchMax) const
    {
        Edit_LimitText(m_hWnd, cchMax);
    }

    // �ڶ��б༭��ḻ�༭�ؼ��л�ȡ����ָ���ַ��������е�����, ����ֵ�ǰ���wParamָ�����ַ��������еĴ��㿪ʼ���к�
    // ich = Ҫ�������ŵ����а������ַ����ַ�����������˲���Ϊ-1����EM_LINEFROMCHAR������ǰ�У���������ŵ��У����кţ������������ѡ�����������ѡ��Ŀ�ʼ���е��к�
    inline int LineFromChar(int ich) const
    {
        return Edit_LineFromChar(m_hWnd, ich);
    }

    // ��ȡ���б༭��ḻ�༭�ؼ���ָ���еĵ�һ���ַ����ַ�����
    // line = ���㿪ʼ���к�, ֵ-1ָ����ǰ�кţ���������ŵ��У�
    inline int LineIndex(int line) const
    {
        return Edit_LineIndex(m_hWnd, line);
    }

    // �ڱ༭��ḻ�༭�ؼ��м���һ�еĳ��ȣ����ַ�Ϊ��λ��
    // �ڶ��б༭�ؼ�������ֵ��wParam����ָ�����еĳ��ȣ���TCHARΪ��λ��������ANSI�ı��������ֽ���������Unicode�ı��������ַ��������е�ĩβ�������س�����
    // ���ڵ��б༭�ؼ�������ֵ�Ǳ༭�ؼ����ı��ĳ��ȣ���TCHARΪ��λ��
    // ���line���ڿؼ��е��ַ������򷵻�ֵΪ��
    // line = Ҫ��ȡ�䳤�ȵ������ַ����ַ�����������˲������ڿؼ��е��ַ������򷵻�ֵΪ��
    // �˲�������Ϊ-1������������£�����Ϣ�����ذ���ѡ���ַ�������δѡ���ַ������������磬���ѡ���һ�еĵ��ĸ��ַ���չ����һ�еĵڰ˸��ַ����򷵻�ֵΪ10����һ�������ַ�����һ���߸���
    inline int LineLength(int line) const
    {
        return Edit_LineLength(m_hWnd, line);
    }


    // lpszReplace = ָ������滻�ı����Կ��ַ���β���ַ�����ָ��
    // canUndone = ָ���Ƿ���Գ����滻���������ΪTRUE������Գ����ò��������ΪFALSE��������޷�����
    inline void ReplaceSel(LPCWSTR lpszReplace, BOOL canUndone = false) const
    {
        Send(EM_REPLACESEL, (WPARAM)canUndone, (LPARAM)lpszReplace);
    }

    // �ڶ��б༭��ḻ�༭�ؼ��д�ֱ�����ı�
    // dv = ��ֱ��ֵ, dh = ˮƽ������ֵ
    inline void Scroll(int dv, int dh) const
    {
        Edit_Scroll(m_hWnd, dv, dh);
    }

    // �ڱ༭��ḻ�༭�ؼ��н��������������ͼ��
    inline BOOL ScrollCaret() const
    {
        return Edit_ScrollCaret(m_hWnd);
    }

    // ���ñ༭�ؼ��в������λ�õĴ��㿪ʼ������
    // newCaretIndex = �ַ�����
    inline BOOL SetCaretIndex(int newCaretIndex) const
    {
        return (BOOL)Send(EM_SETCARETINDEX, (WPARAM)(newCaretIndex), 0);
    }

    // ����ָ���༭�ؼ�����չ��ʽ
    // �༭�ؼ�����չ��ʽ��CreateWindowEx������SetWindowLong����ʹ�õ���չ��ʽ�޹�
    // dwStyle = Ҫ���õ���չ�༭�ؼ���ʽ
    // dwMask = ��ֵָ��Ҫ��Ӱ�����ʽ
    inline BOOL SetExtendedStyle(DWORD dwStyle, DWORD dwMask) const
    {
        return Send(EM_SETEXTENDEDSTYLE, dwMask, dwStyle) != FALSE;
    }

    // ��Ӧ�ó��������µ��ڴ���֮ǰ��Ӧ�õ��� GetHandle() ��ȡ��ǰ�ڴ滺�����ľ�����ͷŸ��ڴ�
    // �༭�ؼ�����Ҫ������ı��ռ�ʱ���Զ����·�������Ļ���������������ɾ���㹻���ı����Ӷ�������Ҫ����Ŀռ�
    // ���ö��б༭�ؼ���ʹ�õ��ڴ���
    // hMem = �༭�ؼ�ʹ�õ��ڴ滺������������ڴ洢��ǰ��ʾ���ı��������Ƿ����Լ����ڴ档���б�Ҫ���ؼ������·�����ڴ�
    inline void SetHandle(HLOCAL hMem) const
    {
        Edit_SetHandle(m_hWnd, hMem);
    }

    // ���û�����༭�ؼ����޸ı�־���޸ı�־ָʾ�༭�ؼ��е��ı��Ƿ��ѱ��޸�
    // fModified = �޸ı�־����ֵ��ֵΪTRUE��ʾ�ı��ѱ��޸ģ�ֵΪFALSE��ʾ�ı���δ���޸�
    inline void SetModify(BOOL fModified) const
    {
        Edit_SetModify(m_hWnd, fModified);
    }

    // ���û�ɾ�����ڱ༭��ḻ�༭�ؼ��������ַ������������ַ��󣬽���ʾ���ַ��������û�������ַ�
    // ch = Ҫ��ʾ���ַ������û�������ַ�������˲���Ϊ�㣬��ؼ���ɾ����ǰ�����ַ�����ʾ�û�������ַ�
    inline void SetPasswordChar(wchar_t ch) const
    {
        Edit_SetPasswordChar(m_hWnd, ch);
    }

    // ���û�ɾ���༭��ḻ�༭�ؼ���ֻ����ʽ��ES_READONLY��
    // fReadOnly = ָ�������û���ɾ��ES_READONLY��ʽ��ֵΪTRUE����ES_READONLY��ʽ��ֵΪFALSE��ɾ��ES_READONLY��ʽ
    inline BOOL SetReadOnly(BOOL fReadOnly) const
    {
        return Edit_SetReadOnly(m_hWnd, fReadOnly);
    }

    // ���ñ༭�ؼ��ĸ�ʽ������
    inline void SetRect(const RECT& rc) const
    {
        Edit_SetRect(m_hWnd, &rc);
    }

    // ���ö��б༭�ؼ��ĸ�ʽ���Ρ��÷�����Ч��SetRect��ֻ���������ػ�༭�ؼ�����
    inline void SetRectNoPaint(const RECT& rc) const
    {
        Edit_SetRectNoPaint(m_hWnd, &rc);
    }

    // �ڱ༭��ḻ�༭�ؼ���ѡ���ַ���Χ
    // ichStart = ѡ�����ʼ�ַ�λ��
    // ichEnd = ��ѡ���ݵĽ����ַ�λ��
    inline void SetSel(int ichStart, int ichEnd) const
    {
        Edit_SetSel(m_hWnd, ichStart, ichEnd);
        Edit_ScrollCaret(m_hWnd);
    }

    // �ڶ��б༭��ḻ�༭�ؼ��������Ʊ�λ�����ı����Ƶ��ؼ�ʱ���ı��е��κ��Ʊ�����ᵼ�����ɿռ䣬ֱ����һ���Ʊ�λ
    // cTabs = �����а������Ʊ�λ������������˲���Ϊ�㣬��lpTabs�����������ԣ�����Ĭ�ϵ��Ʊ�λ��ÿ32���Ի���ģ�嵥Ԫ������һ��
    //      ����˲���Ϊ1����ÿ��n���Ի���ģ�嵥λ����һ���Ʊ�λ������n��lpTabs����ָ��ľ��롣����˲�������1����lpTabs��ָ���Ʊ�λ�����ָ��
    // lpTabs = ָ��ָ���Ʊ�����޷������������ָ�룬�ԶԻ���ģ��Ϊ��λ�����cTabsΪ1����˲�����ָ���޷���������ָ�룬���������������Ʊ�λ֮��ľ��룬�ԶԻ���ģ��Ϊ��λ
    inline void SetTabStops(int cTabs, int* lpTabs) const
    {
        Edit_SetTabStops(m_hWnd, cTabs, lpTabs);
    }

    // ���ñ༭�ؼ����ı�
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

    // ���ñ༭�ؼ����ı�
    inline BOOL SetText(LPCWSTR lpsz) const
    {
        return SetWindowTextW(lpsz);
    }

    // ���ñ༭�ؼ����ı�
    inline BOOL SetTextA(LPCSTR lpsz) const 
    {
        return SetWindowTextA(lpsz);
    }

    // ��W�洰�ڱ���, �༭����Ҫ�����������, ���򲿷�ϵͳ�޷����ñ���
    inline BOOL SetWindowTextW(LPCWSTR lpString) const
    {
        if ( !m_hWnd )return false;
        return Send(WM_SETTEXT, 0, (LPARAM)lpString) != 0;
    }

    // ��A�洰�ڱ���, �༭����Ҫ�����������, ���򲿷�ϵͳ�޷����ñ���
    inline BOOL SetWindowTextA(LPCSTR lpString) const
    {
        if ( !m_hWnd )return false;
        return SendA(WM_SETTEXT, 0, (LPARAM)lpString) != 0;
    }

    // ��Ӧ�ó������Wordwrap�����滻�༭�ؼ���Ĭ��Wordwrap����
    // lpfnWordBreak = Ӧ�ó�������Զ����й��ܵĵ�ַ���йػ��еĸ�����Ϣ����μ�EditWordBreakProc�ص�����������
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

    // ��ʾ��༭�ؼ�������������ʾ
    // peditballoontip = ָ��EDITBALLOONTIP�ṹ��ָ�룬�ýṹ�����й�Ҫ��ʾ��������ʾ����Ϣ
    inline BOOL ShowBalloonTip(PEDITBALLOONTIP peditballoontip) const
    {
        return Send(EM_SHOWBALLOONTIP, 0, (LPARAM)peditballoontip) != FALSE;
    }

    // �����༭��ḻ�༭�ؼ��ĳ��������е����һ������
    inline BOOL Undo() const
    {
        return Edit_Undo(m_hWnd);
    }
    // ȷ���༭��ḻ�༭�ؼ��ĳ����������Ƿ����κβ���
    inline BOOL CanUndo() const
    {
        return Edit_CanUndo(m_hWnd);
    }
    // ȷ���༭��ḻ�༭�ؼ��ĳ����������Ƿ����κβ���
    inline BOOL IsUndo() const
    {
        return CanUndo();
    }
    
};

