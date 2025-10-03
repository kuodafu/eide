#pragma once
#include "CStatic.h"
#include <d2d/d2d_old.h>
class CImage : public CMyStatic
{
private:
    LPEX_IMAGE m_hImage;
    LPCANVAS m_hCanvas;
protected:
    virtual LRESULT WndProc(WNDPROC oldProc, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch ( message )
        {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            CreateD2DResouce();
            COLORREF crBack = GetBkColor(ps.hdc);
            _canvas_begindraw(m_hCanvas);
            _canvas_clear(m_hCanvas, ARGB_D2D::toArgb(crBack, 255));
            RECT rc;
            GetClientRect(&rc);
            rc.right -= rc.left;
            rc.bottom -= rc.top;

            if ( m_hImage )
            {
                long cx = m_hImage->width;
                long cy = m_hImage->height;
                float left = ( (float)( rc.right - cx ) ) / 2.f;
                float top = ( (float)( rc.bottom - cy ) ) / 2.f;
                if ( cx > rc.right || cy > rc.bottom )
                {

                    float x = (float)rc.right / (float)cx;
                    float y = (float)rc.bottom / (float)cy;
                    cx = (long)( (float)cx * ( x < y ? x : y ) );
                    cy = (long)( (float)cy * ( x < y ? x : y ) );
                    left = ( (float)( rc.right - cx ) ) / 2.f;
                    top = ( (float)( rc.bottom - cy ) ) / 2.f;
                }
                _canvas_drawimagerectrect(m_hCanvas, m_hImage, left, top, left + (float)cx, top + (float)cy,
                                          0.f, 0.f, (float)m_hImage->width, (float)m_hImage->height, 255);
            }

            HDC hdc = _canvas_getdc(m_hCanvas);
            BitBlt(ps.hdc, 0, 0, rc.right, rc.bottom, hdc, 0, 0, SRCCOPY);
            _canvas_releasedc(m_hCanvas);
            _canvas_enddraw(m_hCanvas);
            EndPaint(hWnd, &ps);
            break;
        }
        case WM_SIZE:
        {
            _canvas_resize(m_hCanvas, LOWORD(lParam), HIWORD(lParam));
            break;
        }
        default:
            return CallWindowProcW(oldProc, hWnd, message, wParam, lParam);
        }
        return 0;
    }


public:
    CImage() :CMyStatic(), m_hImage(0), m_hCanvas(0) { ; }
    virtual ~CImage() { if (m_hCanvas)_canvas_destroy(m_hCanvas); m_hCanvas = 0; }
    virtual inline LPCWSTR className() { return CLASS_IMAGE; };
public:
    // 设置图片, 会销毁之前的图片
    void SetImage(const void* pData, unsigned int size)
    {
        if ( !pData || !size )return;
        if ( m_hImage ) _img_destroy(m_hImage);
        m_hImage = _img_create_frommemory(pData, size);
        InvalidateRect();
    }
    void CreateD2DResouce()
    {
        if ( !m_hCanvas )
        {
            RECT rc;
            GetClientRect(&rc);
            rc.right -= rc.left;
            rc.bottom -= rc.top;
            m_hCanvas = _canvas_create(m_hWnd, rc.right, rc.bottom);
        }
    }

    // 取当前图片
    inline LPEX_IMAGE GetImage() { return m_hImage; }

};

