#pragma once
#include "WndBase.h"
#include <windowsx.h>
class CDateTime : public CControlBase
{
protected:
    virtual inline LPCWSTR SuperClassName() { return DATETIMEPICK_CLASSW; };

public:
    CDateTime() :CControlBase() { ; }
    virtual ~CDateTime() { ; }
public:
    virtual inline LPCWSTR className() { return DATETIMEPICK_CLASSW; };

public:
    
    virtual HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR id = 0, LPVOID lpParam = 0) {
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(icex);
        icex.dwICC = ICC_DATE_CLASSES;
        InitCommonControlsEx(&icex);
        return CWndBase::create(dwExStyle, lpszTitle, dwStyle | WS_CHILD, x, y, nWidth, nHeight, hWndParent, id, lpParam);
    }
    // �ر����ں�ʱ��ѡ������DTP���ؼ�
    inline void CloseMonthCal() {
        DateTime_CloseMonthCal(m_hWnd);
    }

    // ��ȡ�й�ָ�������ں�ʱ��ѡ������DTP���ؼ�����Ϣ
    inline void GetDateTimePickerInfo(LPDATETIMEPICKERINFO data) {
        if (!data) return;
        data->cbSize = sizeof(DATETIMEPICKERINFO);
        DateTime_GetDateTimePickerInfo(m_hWnd, data);
    }

    // ��ȡ��ʾ�ؼ������ü��Ĵ�С
    inline SIZE GetIdealSize() {
        SIZE size;
        Send(DTM_GETIDEALSIZE, 0, (LPARAM)(&size));
    }
    // ��ȡ���ں�ʱ��ѡ������DTP�����·������ؼ��ľ��, ����ɹ����򷵻�DTP�ؼ������·������ؼ��ľ��
    inline HWND GetMonthCal() {
        return (HWND)Send(DTM_GETMONTHCAL, 0, 0);
    }

    // ��ȡ���ں�ʱ��ѡ������DTP���ؼ����������������ֵ���ɫ
    inline COLORREF GetMonthCalColor(int iColor) {
        return (COLORREF)Send(DTM_GETMCCOLOR, iColor, 0);
    }

    // ��ȡ���ں�ʱ��ѡ������DTP���ؼ������·������ؼ���ǰʹ�õ�����
    inline HFONT GetMonthCalFont() {
        return (HFONT)Send(DTM_GETMCFONT, 0, 0);
    }

    // ��ȡ���ں�ʱ��ѡ������DTP���ؼ�����ʽ
    inline DWORD GetMonthCalStyle() {
        return (DWORD)Send(DTM_GETMCSTYLE, 0, 0);
    }

    // ��ȡ���ں�ʱ��ѡ������DTP���ؼ��ĵ�ǰ��С���������ϵͳʱ��
    // ����ֵ��GDTR_MIN��GDTR_MAX�����, ���������GDTR_MIN, ��SYSTEMTIME����ĵ�һ��Ԫ�ذ�����С����ʱ��, ���������GDTR_MAX, ��SYSTEMTIME����ĵڶ���Ԫ�ذ����������ʱ��
    // sysTimes = ������Ա������, [0] = ��С����ϵͳʱ��, [1] = �������ϵͳʱ��
    inline DWORD GetRange(LPSYSTEMTIME sysTimes) {
        return (DWORD)Send(DTM_GETRANGE, 0, (LPARAM)sysTimes);
    }

    // �����ں�ʱ��ѡ������DTP���ؼ��л�ȡ��ǰѡ���ʱ�䣬�����������ָ����SYSTEMTIME�ṹ��
    // ���ʱ����Ϣ�ѳɹ�����lParam�У��򷵻�GDT_VALID ������ؼ�����ΪDTS_SHOWNONE��ʽ����δѡ�пؼ���ѡ���򷵻�GDT_NONE ��������������򷵻�GDT_ERROR
    // sysTime = ָ��SYSTEMTIME�ṹ��ָ�롣���DTM_GETSYSTEMTIME����GDT_VALID����˽ṹ��������ǰѡ���ʱ�䡣����������������Ч��Ϣ���ò���������һ����Ч��ָ�롣������ΪNULL
    inline DWORD GetSystemtime(LPSYSTEMTIME sysTime) {
        return (DWORD)Send(DTM_GETSYSTEMTIME, 0, (LPARAM)sysTime);
    }
    
    // ���ݸ����ĸ�ʽ�ַ����������ں�ʱ��ѡ������DTP���ؼ�����ʾ
    // fmt = ָ������ֹ��ʽ�ַ�����ָ�룬���ַ����������������ʾ�����˲�������ΪNULL�Ὣ�ؼ�����Ϊ��ǰ��ʽ��Ĭ�ϸ�ʽ�ַ���
    inline bool SetFormat(LPCWSTR fmt) {
        return Send(DTM_SETFORMAT, 0, (LPARAM)fmt);
    }

    // �������ں�ʱ��ѡ������DTP���ؼ����·������������ֵ���ɫ
    // ����ɹ����򷵻�һ��COLORREFֵ����ֵ�����������ؼ�ָ�����ֵ���ǰ��ɫ����, ���򣬸���Ϣ����-1
    // iColor = ָ��Ҫ���õ�������ɫ, MCSC_ ��ͷ����
    // cr = �·�������ָ���������õ���ɫ
    inline COLORREF SetMonthCalColor(int iColor, COLORREF cr) {
        return (COLORREF)Send(DTM_SETMCCOLOR, iColor, (LPARAM)cr);
    }

    // �������ں�ʱ��ѡ������DTP���ؼ������·������ؼ�ʹ�õ�����
    inline void SetMonthCalFont(HFONT hFont, bool isUpdate = false) {
        Send(DTM_SETMCFONT, (WPARAM)hFont, (LPARAM)isUpdate);
    }

    // �������ں�ʱ��ѡ������DTP���ؼ�����ʽ, ������һ����ʽ��ֵ
    // style = �ؼ���ʽ, MCS_ ��ͷ����
    inline DWORD SetMonthCalStyle(DWORD style) {
        return (DWORD)Send(DTM_SETMCSTYLE, 0, style);
    }

    // �������ں�ʱ��ѡ������DTP���ؼ�����С���������ϵͳʱ��
    // gd = һ��ֵ����ֵָ����Щ��Χֵ��Ч, GDTR_MIN sysTimes[0] ��Ч, GDTR_MAX sysTimes[1] ��Ч, GDTR_MIN | GDTR_MAX ������Ա��Ч
    // sysTimes = ������Ա������, [0] = ��С����ϵͳʱ��, [1] = �������ϵͳʱ��
    inline bool SetRange(int gd, LPSYSTEMTIME sysTimes) {
        return Send(DTM_SETRANGE, gd, (LPARAM)sysTimes);
    }

    // ���ں�ʱ��ѡ������DTP���ؼ�����Ϊ���������ں�ʱ��
    // gd = һ��ֵ��ָ��Ӧִ�еĲ���, GDT_VALID, GDT_NONE
    // pst = ָ�����ϵͳʱ����Ϣ��SYSTEMTIME�ṹ��ָ�룬ͨ����ʱ���������DTP�ؼ�
    inline bool SetSystemtime(int gd, LPSYSTEMTIME pst) {
        return Send(DTM_SETSYSTEMTIME, gd, (LPARAM)pst);
    }


};

