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
    // 关闭日期和时间选择器（DTP）控件
    inline void CloseMonthCal() {
        DateTime_CloseMonthCal(m_hWnd);
    }

    // 获取有关指定的日期和时间选择器（DTP）控件的信息
    inline void GetDateTimePickerInfo(LPDATETIMEPICKERINFO data) {
        if (!data) return;
        data->cbSize = sizeof(DATETIMEPICKERINFO);
        DateTime_GetDateTimePickerInfo(m_hWnd, data);
    }

    // 获取显示控件而不裁剪的大小
    inline SIZE GetIdealSize() {
        SIZE size;
        Send(DTM_GETIDEALSIZE, 0, (LPARAM)(&size));
    }
    // 获取日期和时间选择器（DTP）子月份日历控件的句柄, 如果成功，则返回DTP控件的子月份日历控件的句柄
    inline HWND GetMonthCal() {
        return (HWND)Send(DTM_GETMONTHCAL, 0, 0);
    }

    // 获取日期和时间选择器（DTP）控件中月日历给定部分的颜色
    inline COLORREF GetMonthCalColor(int iColor) {
        return (COLORREF)Send(DTM_GETMCCOLOR, iColor, 0);
    }

    // 获取日期和时间选择器（DTP）控件的子月份日历控件当前使用的字体
    inline HFONT GetMonthCalFont() {
        return (HFONT)Send(DTM_GETMCFONT, 0, 0);
    }

    // 获取日期和时间选择器（DTP）控件的样式
    inline DWORD GetMonthCalStyle() {
        return (DWORD)Send(DTM_GETMCSTYLE, 0, 0);
    }

    // 获取日期和时间选择器（DTP）控件的当前最小和最大允许系统时间
    // 返回值是GDTR_MIN或GDTR_MAX的组合, 如果设置了GDTR_MIN, 则SYSTEMTIME数组的第一个元素包含最小允许时间, 如果设置了GDTR_MAX, 则SYSTEMTIME数组的第二个元素包含最大允许时间
    // sysTimes = 两个成员的数组, [0] = 最小允许系统时间, [1] = 最大允许系统时间
    inline DWORD GetRange(LPSYSTEMTIME sysTimes) {
        return (DWORD)Send(DTM_GETRANGE, 0, (LPARAM)sysTimes);
    }

    // 从日期和时间选择器（DTP）控件中获取当前选择的时间，并将其放置在指定的SYSTEMTIME结构中
    // 如果时间信息已成功放入lParam中，则返回GDT_VALID 。如果控件设置为DTS_SHOWNONE样式并且未选中控件复选框，则返回GDT_NONE 。如果发生错误，则返回GDT_ERROR
    // sysTime = 指向SYSTEMTIME结构的指针。如果DTM_GETSYSTEMTIME返回GDT_VALID，则此结构将包含当前选择的时间。否则，它将不包含有效信息。该参数必须是一个有效的指针。它不能为NULL
    inline DWORD GetSystemtime(LPSYSTEMTIME sysTime) {
        return (DWORD)Send(DTM_GETSYSTEMTIME, 0, (LPARAM)sysTime);
    }
    
    // 根据给定的格式字符串设置日期和时间选择器（DTP）控件的显示
    // fmt = 指向零终止格式字符串的指针，该字符串定义了所需的显示。将此参数设置为NULL会将控件重置为当前样式的默认格式字符串
    inline bool SetFormat(LPCWSTR fmt) {
        return Send(DTM_SETFORMAT, 0, (LPARAM)fmt);
    }

    // 设置日期和时间选择器（DTP）控件中月份日历给定部分的颜色
    // 如果成功，则返回一个COLORREF值，该值代表月日历控件指定部分的先前颜色设置, 否则，该消息返回-1
    // iColor = 指定要设置的月历颜色, MCSC_ 开头常量
    // cr = 月份日历的指定区域设置的颜色
    inline COLORREF SetMonthCalColor(int iColor, COLORREF cr) {
        return (COLORREF)Send(DTM_SETMCCOLOR, iColor, (LPARAM)cr);
    }

    // 设置日期和时间选择器（DTP）控件的子月份日历控件使用的字体
    inline void SetMonthCalFont(HFONT hFont, bool isUpdate = false) {
        Send(DTM_SETMCFONT, (WPARAM)hFont, (LPARAM)isUpdate);
    }

    // 设置日期和时间选择器（DTP）控件的样式, 返回上一个样式的值
    // style = 控件样式, MCS_ 开头常量
    inline DWORD SetMonthCalStyle(DWORD style) {
        return (DWORD)Send(DTM_SETMCSTYLE, 0, style);
    }

    // 设置日期和时间选择器（DTP）控件的最小和最大允许系统时间
    // gd = 一个值，该值指定哪些范围值有效, GDTR_MIN sysTimes[0] 有效, GDTR_MAX sysTimes[1] 有效, GDTR_MIN | GDTR_MAX 两个成员有效
    // sysTimes = 两个成员的数组, [0] = 最小允许系统时间, [1] = 最大允许系统时间
    inline bool SetRange(int gd, LPSYSTEMTIME sysTimes) {
        return Send(DTM_SETRANGE, gd, (LPARAM)sysTimes);
    }

    // 日期和时间选择器（DTP）控件设置为给定的日期和时间
    // gd = 一个值，指定应执行的操作, GDT_VALID, GDT_NONE
    // pst = 指向包含系统时间信息的SYSTEMTIME结构的指针，通过该时间可以设置DTP控件
    inline bool SetSystemtime(int gd, LPSYSTEMTIME pst) {
        return Send(DTM_SETSYSTEMTIME, gd, (LPARAM)pst);
    }


};

