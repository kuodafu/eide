#pragma once
#include <d2d/d2d_old.h>

// 探测窗口的阴影窗口, 被探测的窗口原样显示, 被探测窗口之外的区域变暗显示
class CSpyWindowShadow
{
private:
    HWND     m_hWnd;     // 阴影窗口句柄
    LONG     m_cxScreen; // 屏幕宽度
    LONG     m_cyScreen; // 屏幕高度

    LPCANVAS hCanvas;    // 绘画使用的画布句柄
    LPEX_PEN hPen;       // 绘画边框使用的画笔
public:

    CSpyWindowShadow() :m_hWnd(0), m_cxScreen(0), m_cyScreen(0), hCanvas(0), hPen(0)
    {

    }

    // 初始化阴影窗口, 只能初始化一次, 在探测窗口的时候整个屏幕变暗, 用红框框出被探测的窗口
    // isInitD2D = 是否初始化D2D, 如果不初始化, 需要外部初始化
    // hInst = 模块句柄, 如果为0, 则调用 GetModuleHandleW(0) 获取
    HWND init(bool isInitD2D = true, HINSTANCE hInst = 0)
    {
        if (m_hWnd) return m_hWnd;
        if (!hInst)
            hInst = GetModuleHandleW(0);

        if (isInitD2D)
            _canvas_init(true);

        WNDCLASSEXW wcex    = { 0 };
        wcex.cbSize         = sizeof(WNDCLASSEXW);
        wcex.style          = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc    = DefWindowProcW;
        wcex.cbClsExtra     = 0;
        wcex.cbWndExtra     = 0;
        wcex.hInstance      = hInst;
        wcex.hCursor        = LoadCursorW(0, IDC_ARROW);
        wcex.lpszMenuName   = 0;
        wcex.lpszClassName  = L"shadow-qq-121007124";
        ATOM atom = RegisterClassExW(&wcex);

        m_cxScreen  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        m_cyScreen  = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        const DWORD Style = WS_POPUP | WS_SYSMENU | WS_VISIBLE;
        const DWORD ExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT;


        m_hWnd = CreateWindowExW(ExStyle, wcex.lpszClassName, wcex.lpszClassName, Style,
            0, 0, m_cxScreen, m_cyScreen, 0, 0, wcex.hInstance, 0);
        int err = GetLastError();
        if (!m_hWnd)return 0;

        hCanvas = _canvas_create(m_hWnd, m_cxScreen, m_cyScreen);
        hPen    = _pen_create(MAKEARGB(255, 255, 0, 0), (float)2.f);

        return m_hWnd;
    }

    // 更新阴影位置, 根据窗口位置重新绘画阴影窗口
    // hWnd = 需要显示的窗口
    // argb = 背景颜色
    void update(HWND hWnd, DWORD argb = -1275068416)
    {
        if (!IsWindow(m_hWnd))return;

        RECT rc;
        GetWindowRect(hWnd, &rc);

        if (hCanvas == 0)
            hCanvas = _canvas_create(m_hWnd, m_cxScreen, m_cyScreen);

        if (hCanvas->width < (UINT)m_cxScreen || hCanvas->height < (UINT)m_cyScreen)
            hCanvas->Resize(m_cxScreen, m_cyScreen);

        hCanvas->BeginDraw();
        hCanvas->clear(argb);
        hCanvas->SetAntialiasMode(0);

        // 窗口在屏幕中的位置, 需要在这个位置绘画边框, 以及设置裁剪区, 把这个位置变成透明
        RECT_F rcWindowF(rc);
        hCanvas->SetClip(rcWindowF.left, rcWindowF.top, rcWindowF.right, rcWindowF.bottom);
        hCanvas->clear(0);
        hCanvas->ResetClip();

        hCanvas->DrawRectangle(hPen, &rcWindowF);


        POINT pt0 = { 0, 0 };
        int ret = UpdateLayered(m_hWnd, m_cxScreen, m_cyScreen, hCanvas->GetDC(), &pt0);
        hCanvas->ReleaseDC();
        hCanvas->EndDraw();

        if (!IsWindowVisible(m_hWnd))
            ShowWindow(m_hWnd, SW_SHOWNA);
    }
    void destroy()
    {
        if (IsWindow(m_hWnd))
            DestroyWindow(m_hWnd);
        _pen_destroy(hPen);
        _canvas_destroy(hCanvas);
        m_hWnd = 0;
    }
    void hide()
    {
        if (IsWindow(m_hWnd))
            ShowWindow(m_hWnd, SW_HIDE);
    }



};
