#pragma once
#include "CEdit.h"
#include <commdlg.h>
#include <Richedit.h>
class CRichEdit : public CEdit
{
//#define FR_DOWN         1
//#define FR_MATCHCASE    4
//#define FR_WHOLEWORD    2
protected:
    virtual inline LPCWSTR SuperClassName() { return MSFTEDIT_CLASS; }

public:
    CRichEdit() :CEdit() { ; }
    virtual ~CRichEdit() { ; }
public:
    virtual inline LPCWSTR className() { return MSFTEDIT_CLASS; }// MSFTEDIT_CLASS; RICHEDIT_CLASS; }

    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id = 0, LPVOID lpParam = 0) {
        static HMODULE hModule;
        if (!hModule)
        {
            InitCommonControls();
            hModule = LoadLibraryW(L"Msftedit.dll");
        }
        
        return CWndBase::create(dwExStyle, lpszTitle, dwStyle | WS_CHILD, x, y, nWidth, nHeight, hWndParent, id, lpParam);
    }
    virtual bool SetFormat() {
        PARAFORMAT2 pf;
        memset(&pf, 0, sizeof(pf));
        pf.cbSize = sizeof(PARAFORMAT2); //ʶ��paraformat��paraformat2
        pf.dwMask = PFM_LINESPACING;
        pf.dyLineSpacing = 300; //�о��ڴ�����
        pf.bLineSpacingRule = 4;

        Send(EM_SETPARAFORMAT, 0, (LPARAM)&pf);
        return false;
    }

    // ���start��end��Ϊ-1, ����start ���� end, �Ǿ�ѡ��ǰѡ��, �����ǰû��ѡ���򷵻ؿ��ı�
    // �����Ϊ-1, ��end����start, �Ǿ�ѡ��start��end������
    wstr GetSelText(DWORD start = -1, DWORD end = -1) const
    {
        wstr ret;
        DWORD nStart, nEnd;
        GetSel(&nStart, &nEnd);
        if (start == -1 && end == -1 || end < start)
        {
            ret.resize(nEnd - nStart + 10);
            Send(EM_GETSELTEXT, 0, (LPARAM)ret.data());
        }
        else
        {
            if (start == -1)start = 0;
            if (end == -1)end = GetTextLengthW();
            ret.resize(end - start + 10);
            SetSel(start, end);
            Send(EM_GETSELTEXT, 0, (LPARAM)ret.data());
            SetSel(nStart, nEnd);
        }

        ret.resize(wcslen(ret.c_str()));
        ret.Addref();
        return ret;
    }
    // ȷ���ḻ�ı༭�ؼ��Ƿ����ճ��ָ���ļ������ʽ
    inline bool CanPaste() {
        return Send(EM_CANPASTE, 0, 0);
    }
    // ȷ���ḻ�ı༭�ؼ��Ƿ����ճ��ָ���ļ������ʽ
    inline bool IsPaste() {
        return CanPaste();
    }
    
    // ����
    inline bool reod() {
        return Send(EM_REDO, 0, 0);
    }

    // �Ƿ������
    inline bool CanReod() {
        return Send(EM_REDO, 0, 0);
    }
    // �Ƿ������
    inline bool IsReod() {
        return CanReod();
    }
    // ����
    inline bool cut() {
        return Send(WM_CUT, 0, 0);
    }
    // ����
    inline bool copy() {
        return Send(WM_COPY, 0, 0);
    }
    // ɾ��
    inline bool clear() {
        return Send(WM_CLEAR, 0, 0);
    }
    // ճ��
    inline bool paste() {
        return Send(WM_PASTE, 0, 0);
    }
public:
    inline bool SetSelFormat(const CHARFORMATW* cf) {
        return Send(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)cf);
    }
    inline bool GetSelFormat(CHARFORMATW* cf) {
        return Send(EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)cf);
    }
    inline bool SetSelColor(COLORREF crText) {
        CHARFORMATW cf;
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_EFFECTS | CFM_COLOR;
        GetSelFormat(&cf);
        cf.dwEffects &= ~CFE_AUTOCOLOR;
        cf.crTextColor = crText;
        return SetSelFormat(&cf);
    }

    // ����ѡ���ı���ʽ, Ϊ�����޸ĵ�ǰֵ
    // fontName = ��������
    // crText = ������ɫ
    // size = �����С, ��λ: �(1/1440Ӣ�� �� 1/20 ��)
    // bold = �Ƿ�Ӵ�
    // italic = �Ƿ�б��
    // strikeout = �Ƿ�ɾ����
    // underline = �Ƿ��»���
    // _protected = �Ƿ�Ϊ����
    // link = �Ƿ�Ϊ����
    inline bool SetSelFormat(LPCWSTR fontName = 0, COLORREF crText = 0, int size = 0,
        bool bold = false, bool italic = false, bool strikeout = false,
        bool underline = false, bool _protected = false, bool link = false) {
        CHARFORMATW cf;
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_FACE | CFM_EFFECTS | CFM_SIZE | CFM_OFFSET | CFM_COLOR | CFM_CHARSET;
        GetSelFormat(&cf);

        cf.dwEffects &= ~CFE_AUTOCOLOR;
        cf.dwEffects |= (bold ? CFE_BOLD : 0);
        cf.dwEffects |= (italic ? CFE_ITALIC : 0);
        cf.dwEffects |= (strikeout ? CFE_STRIKEOUT : 0);
        cf.dwEffects |= (underline ? CFE_UNDERLINE : 0);
        cf.dwEffects |= (_protected ? CFE_PROTECTED : 0);
        cf.dwEffects |= (link ? CFE_LINK : 0);
        
        if (fontName)
        {
            size_t len = wcslen(fontName) + 1;
            if (len > 32)len = 32;
            memcpy(cf.szFaceName, fontName, len * sizeof(wchar_t));
        }
        cf.crTextColor = crText;
        if (size)cf.yHeight = size;
        return SetSelFormat(&cf);
    }

public:
    // str = �����ҵ��ı�
    // isNext = �Ƿ�����Ѱ��, Ϊfalse��Ӻ���ǰ
    // isCare = �Ƿ����ִ�Сд
    // comAll = ȫ��ƥ��
    // _off = ��ʼ����λ��
    // _end = ��������λ��
    inline size_t find(LPCWSTR str, size_t _off = 0, size_t _end = -1, bool isNext = true, bool isCare = true, bool comAll = false) {
        FINDTEXTW ft = { 0 };
        ft.chrg.cpMin = (int)_off;
        ft.chrg.cpMax = (int)_end;
        if (ft.chrg.cpMax == 0)
            ft.chrg.cpMax = GetTextLengthW();
        ft.lpstrText = str;
        int flag = 0;
        if (isNext) flag |= FR_DOWN;
        if (isCare) flag |= FR_MATCHCASE;
        if (comAll) flag |= FR_WHOLEWORD;
        return (size_t)Send(EM_FINDTEXTW, flag, (LPARAM)&ft);
    }
};

