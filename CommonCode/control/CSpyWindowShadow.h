#pragma once
#include <d2d/d2d_old.h>

// ̽�ⴰ�ڵ���Ӱ����, ��̽��Ĵ���ԭ����ʾ, ��̽�ⴰ��֮�������䰵��ʾ
class CSpyWindowShadow
{
private:
    HWND     m_hWnd;     // ��Ӱ���ھ��
    LONG     m_cxScreen; // ��Ļ���
    LONG     m_cyScreen; // ��Ļ�߶�

    LPCANVAS hCanvas;    // �滭ʹ�õĻ������
    LPEX_PEN hPen;       // �滭�߿�ʹ�õĻ���
public:

    CSpyWindowShadow() :m_hWnd(0), m_cxScreen(0), m_cyScreen(0), hCanvas(0), hPen(0)
    {

    }

    // ��ʼ����Ӱ����, ֻ�ܳ�ʼ��һ��, ��̽�ⴰ�ڵ�ʱ��������Ļ�䰵, �ú������̽��Ĵ���
    // isInitD2D = �Ƿ��ʼ��D2D, �������ʼ��, ��Ҫ�ⲿ��ʼ��
    // hInst = ģ����, ���Ϊ0, ����� GetModuleHandleW(0) ��ȡ
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

    // ������Ӱλ��, ���ݴ���λ�����»滭��Ӱ����
    // hWnd = ��Ҫ��ʾ�Ĵ���
    // argb = ������ɫ
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

        // ��������Ļ�е�λ��, ��Ҫ�����λ�û滭�߿�, �Լ����òü���, �����λ�ñ��͸��
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
