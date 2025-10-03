#pragma once
#include "WndBase.h"
#include <d2d/d2d_old.h>
class CLayered : public CControlBase
{
    struct LAYERED_DATA
    {
        LPMEMDCBITMAP md;
        LPEX_FONT hFont;
        LPEX_FONT hFontTitle;
        DWORD argbBack;
        DWORD argbText;
        DWORD argbBorder;
        LPEX_BRUSH hbrBack;
        LPEX_BRUSH hbrText;
        LPEX_PEN hpenBorder;
        int nMaxWidth;
        int nMaxHeight;
        int isShow;
        POINT pt;
        int alpha;
        SIZE sizeWindow;

        LPCANVAS hCanvas;
    };
    wstr m_tips;
    wstr m_title;
    LAYERED_DATA m_data;
    const static int timerID_showwindow = 10;
    const static int timerID_hidewindow = 11;

protected:
    virtual inline LPCWSTR SuperClassName() { return L"LayeredWindow"; }

public:
    CLayered() : CControlBase()
    {
        memset(&m_data, 0, sizeof(m_data));
    }
    virtual ~CLayered() { ; }
public:
    virtual inline LPCWSTR className() { return L"LayeredWindow"; }

public:
    virtual inline HWND create(DWORD dwExStyle, LPCWSTR lpszTitle, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent, LONG_PTR ID = 0, LPVOID lpParam = 0)
    {
        return CWndBase::create(dwExStyle | WS_EX_LAYERED | WS_EX_TOOLWINDOW, 0, dwStyle | WS_POPUP,
            x, y, nWidth, nHeight, hWndParent, ID, lpParam);
    }
    inline HWND create(DWORD style, DWORD exStyle)
    {
        if (!style)style = WS_VISIBLE;
        return create(exStyle | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW, 0, style | WS_POPUP, 0, 0, 0, 0, NULL, NULL, this);
    }


public:
    inline HDC GetDC() { return m_data.md ? m_data.md->hdc : 0; }
    inline bool show(LPCWSTR text, LPEX_FONT hFontTitle = 0, LPEX_FONT hFont = 0, int x = -1, int y = -1)
    {
        SetTipsText(text);
        init_canvas();
        POINT pt;
        GetCursorPos(&pt);
        if (x == -1)x = pt.x;
        if (y == -1)y = pt.y;
        m_data.pt = { x, y };

        if (!IsWindowVisible(m_hWnd))   // 窗口没显示的时候需要有个渐入的效果
            m_data.alpha = 10;
        else if (m_data.alpha < 250)
            m_data.alpha += 5;

        _canvas_begindraw(m_data.hCanvas);
        _canvas_clear(m_data.hCanvas, m_data.argbBack);

        m_data.sizeWindow.cx = 5;
        m_data.sizeWindow.cy = 5;
        const int fmt = DT_NOPREFIX;
        float textWidth = 0;
        float textHeight = 0;
        const int offsetWidth = 18;
        const int offsetHeight = 10;
        RECT_F rcText(10, 6, (float)m_data.nMaxWidth, (float)m_data.nMaxHeight);
        if (hFontTitle == 0)
            hFontTitle = m_data.hFontTitle;
        if (!hFont)
            hFont = m_data.hFont;

        // 计算标题位置
        if (!m_title.empty())
        {
            wstr title = GetShowTextTitle();
            _canvas_calctextsize(m_data.hCanvas, hFontTitle, title.c_str(), title.size(), fmt,
                (float)m_data.nMaxWidth, (float)m_data.nMaxHeight, &textWidth, &textHeight);

            m_data.sizeWindow.cx = (int)textWidth + offsetWidth;
            m_data.sizeWindow.cy += (int)textHeight + offsetHeight;
            rcText.bottom = rcText.top + (float)m_data.sizeWindow.cy;
            _canvas_drawtext(m_data.hCanvas, hFontTitle, m_data.hbrText, title.c_str(), title.size(), fmt, &rcText);
            rcText.top = rcText.bottom - offsetHeight;
            m_data.sizeWindow.cy -= (offsetHeight - 2);
        }

        // 计算绘画文本的位置
        if (!m_tips.empty())
        {
            wstr drawText = GetShowText();
            _canvas_calctextsize(m_data.hCanvas, hFont, drawText.c_str(), drawText.size(), fmt,
                (float)m_data.nMaxWidth, (float)m_data.nMaxHeight, &textWidth, &textHeight);

            if ((int)textWidth + offsetWidth > m_data.sizeWindow.cx)
                m_data.sizeWindow.cx = (int)textWidth + offsetWidth;
            m_data.sizeWindow.cy += (int)textHeight + offsetHeight;

            rcText.bottom = rcText.top + (float)m_data.sizeWindow.cy;
            _canvas_drawtext(m_data.hCanvas, hFont, m_data.hbrText, drawText.c_str(), drawText.size(), fmt, &rcText);
        }
        else
        {
            m_data.sizeWindow.cy += offsetHeight;
        }
        RECT_F rcBorder(1, 1, (float)m_data.sizeWindow.cx, (float)m_data.sizeWindow.cy);

        int oldMode = 0;
        _canvas_SetAntialiasMode(m_data.hCanvas, 1, &oldMode);
        _canvas_drawRectangle(m_data.hCanvas, m_data.hpenBorder, &rcBorder);
        _canvas_SetAntialiasMode(m_data.hCanvas, oldMode, 0);


        m_data.isShow = 1;
        HDC hdc = _canvas_getdc(m_data.hCanvas);
        // 绘画结果保存起来, 下次不需要重画的时候直接从这个DC里拷贝出来
        BitBlt(m_data.md->hdc, 0, 0, m_data.nMaxWidth, m_data.nMaxHeight, hdc, 0, 0, SRCCOPY);

        int cxScreen = GetSystemMetrics(SM_CXSCREEN);
        int cyScreen = GetSystemMetrics(SM_CYSCREEN);
        int index = m_data.pt.x / cxScreen;
        cxScreen *= (index + 1);

        if (m_data.pt.x + m_data.sizeWindow.cx + 5 > cxScreen)
            m_data.pt.x = cxScreen - m_data.sizeWindow.cx - 5;

        int ret = UpdateLayered(m_hWnd, m_data.sizeWindow.cx, m_data.sizeWindow.cy, m_data.md->hdc, &m_data.pt, m_data.alpha);
        _canvas_releasedc(m_data.hCanvas);
        ShowWindow(m_hWnd, SW_SHOWNA);

        _canvas_enddraw(m_data.hCanvas);
        SetTimer(m_hWnd, timerID_showwindow, 10, 0);
        return ret == 0;
    }
    inline bool hide()
    {
        if (m_data.isShow != 1)return true;
        m_data.isShow = 0;
        return SetTimer(m_hWnd, timerID_showwindow, 10, 0);
        return CWndBase::hide();
    }

    inline void SetTipsTitle(LPCWSTR text)
    {
        m_title = text;
    }
    inline void SetTipsText(LPCWSTR text)
    {
        m_tips = text;
    }
private:
    inline void _show_window()
    {
        if (m_data.alpha == 0)
            ShowWindow(m_hWnd, SW_HIDE);
        UpdateLayered(m_hWnd, m_data.sizeWindow.cx, m_data.sizeWindow.cy, m_data.md->hdc, &m_data.pt, m_data.alpha);
    }
    inline void TimerProc(WPARAM id)
    {
        switch (id)
        {
        case timerID_showwindow:
        {
            m_data.alpha += (m_data.isShow == 1 ? 20 : -20);
            if (m_data.alpha >= 255 || m_data.alpha <= 0)
                KillTimer(m_hWnd, id);
            if (m_data.alpha > 255)m_data.alpha = 255;
            else if (m_data.alpha < 0)m_data.alpha = 0;
            _show_window();
            break;
        }
        default:
            break;
        }
    }
    inline void uninit_canvas()
    {
        _brush_destroy(m_data.hbrBack);
        _brush_destroy(m_data.hbrText);
        _pen_destroy(m_data.hpenBorder);
        _font_destroy(m_data.hFont);
        _font_destroy(m_data.hFontTitle);
        _canvas_destroy(m_data.hCanvas);
        md_destroy(m_data.md);
    }
    inline void init_canvas()
    {
        if (m_data.md)return;
        if (m_data.nMaxWidth <= 0) m_data.nMaxWidth = GetSystemMetrics(SM_CXSCREEN);
        if (m_data.nMaxHeight <= 0)m_data.nMaxHeight = GetSystemMetrics(SM_CYSCREEN);
        if (!m_data.argbBack) m_data.argbBack = MAKEARGB(255, 66, 66, 69);
        if (!m_data.argbText) m_data.argbText = MAKEARGB(255, 222, 222, 222);
        if (!m_data.argbBorder) m_data.argbBorder = MAKEARGB(255, 77, 77, 80);

        m_data.hbrBack = _brush_create(m_data.argbBack);
        m_data.hbrText = _brush_create(m_data.argbText);
        m_data.hpenBorder = _pen_create(m_data.argbBorder);
        m_data.md = md_create(m_data.nMaxWidth, m_data.nMaxHeight);


        LOGFONTW lf = { 0 };
        SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0);
        lf.lfCharSet = GB2312_CHARSET;
        memcpy(lf.lfFaceName, L"微软雅黑", 10);
  
        m_data.hFont = _font_create_logfont(&lf, 0);
        lf.lfWeight = 700;
        lf.lfHeight = -14; 
        m_data.hFontTitle = _font_create_logfont(&lf, 0);
        m_data.hCanvas = _canvas_create(0, m_data.nMaxWidth, m_data.nMaxHeight);
    }
    inline wstr GetShowText()
    {
        size_t len = m_tips.size();
        if (len < 1024) return m_tips;

        wstr ret;
        ret.reserve(1100);
        ret.assign(m_tips.c_str(), 1000);
        ret.append(L"......省略了").append(len - 1000).append(L"个字符未显示");
        return ret;
    }
    inline wstr GetShowTextTitle()
    {
        size_t len = m_title.size();
        if (len < 1024) return m_title;

        wstr ret;
        ret.reserve(1100);
        ret.assign(m_title.c_str(), 1000);
        ret.append(L"......省略了").append(len - 1000).append(L"个字符未显示");
        return ret;
    }
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CREATE:
            break;
        case WM_TIMER:
            TimerProc(wParam);
            break;
        case WM_DESTROY:
            uninit_canvas();
            break;
        default:
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        return 0;
    }

    //static LRESULT CALLBACK CWndBase_DefWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    //{

    //}

};

