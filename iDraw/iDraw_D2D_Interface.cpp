#if 1
#include "EWindow_Fne_Main.h"

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_isDxRender()
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_isDxRender();
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_destroy(LPCANVAS& hCanvas)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_destroy(hCanvas);
}

EXTERN_C _declspec(dllexport) LPCANVAS   WINAPI iDraw_canvas_create(HWND hWnd, UINT width, UINT height)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_create(hWnd, width, height);
}

EXTERN_C _declspec(dllexport) int        WINAPI iDraw_ExSetLastError(int code)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->ExSetLastError(code);
}

EXTERN_C _declspec(dllexport) int        WINAPI iDraw_ExGetLastError()
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->ExGetLastError();
}

// �������
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_clear(const LPCANVAS hCanvas, ARGB argb)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_clear(hCanvas, argb);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetTarget(const LPCANVAS hCanvas, LPEX_IMAGE bitmap)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_SetTarget(hCanvas, bitmap);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_ReTarget(const LPCANVAS hCanvas)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_ReTarget(hCanvas);
}

EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_canvas_GetTarget(const LPCANVAS hCanvas)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_GetTarget(hCanvas);
}

EXTERN_C _declspec(dllexport) void WINAPI iDraw_canvas_FreeTarget(const LPCANVAS hCanvas, LPEX_IMAGE image)
{
    if (!s_info || !s_info->d2d)
        return;
    return s_info->d2d->_canvas_FreeTarget(hCanvas, image);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_begindraw(const LPCANVAS hCanvas)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_begindraw(hCanvas);
}

// ƽ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_translate(const LPCANVAS hCanvas, float x, float y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_translate(hCanvas, x, y);
}

// ����
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_scale(const LPCANVAS hCanvas, float x, float y, float scaleX, float scaleY)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_scale(hCanvas, x, y, scaleX, scaleY);
}

// ��ת
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_rotate(const LPCANVAS hCanvas, float x, float y, float rotate)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_rotate(hCanvas, x, y, rotate);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_enddraw(const LPCANVAS hCanvas)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_enddraw(hCanvas);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_resize(const LPCANVAS hCanvas, int cx, int cy)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_resize(hCanvas, cx, cy);
}

EXTERN_C _declspec(dllexport) HDC        WINAPI iDraw_canvas_getdc(const LPCANVAS hCanvas)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_getdc(hCanvas);
}

EXTERN_C _declspec(dllexport) void       WINAPI iDraw_canvas_releasedc(const LPCANVAS hCanvas)
{
    if (!s_info || !s_info->d2d)
        return;
    return s_info->d2d->_canvas_releasedc(hCanvas);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_calctextsize(const LPCANVAS hCanvas, LPEX_FONT font, LPCWSTR text, size_t textLen, DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_calctextsize(hCanvas, font, text, textLen, textFormat, layoutWidth, layoutHeight, pWidth, pHeight);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_getsize(const LPCANVAS hCanvas, UINT* width, UINT* height)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_getsize(hCanvas, width, height);
}

// ���Ƽ�ͼ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECT_F rcF)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawRectangle(hCanvas, hPen, rcF);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_fillRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECT_F rcF)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_fillRectangle(hCanvas, hBrush, rcF);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawEllipse(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCELLIPSE_F rcF)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawEllipse(hCanvas, hPen, rcF);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_fillEllipse(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCELLIPSE_F rcF)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_fillEllipse(hCanvas, hBrush, rcF);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawRoundedRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECTROUNDED_F rcRounded)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawRoundedRectangle(hCanvas, hPen, rcRounded);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_fillRoundedRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECTROUNDED_F rcRounded)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_fillRoundedRectangle(hCanvas, hBrush, rcRounded);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawLine(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCLINEPOINT_F pt)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawLine(hCanvas, hPen, pt);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_fillpath(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPEX_PATH path)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_fillpath(hCanvas, hBrush, path);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawpath(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPEX_PATH path)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawpath(hCanvas, hPen, path);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawimage(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, BYTE alpha)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawimage(hCanvas, image, left, top, alpha);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawimagerect(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, float right, float bottom, BYTE alpha)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawimagerect(hCanvas, image, left, top, right, bottom, alpha);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawimagerectrect(const LPCANVAS hCanvas, LPEX_IMAGE image, float dstLeft, float dstTop, float dstRight, float dstBottom, float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawimagerectrect(hCanvas, image, dstLeft, dstTop, dstRight, dstBottom, srcLeft, srcTop, srcRight, srcBottom, alpha);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawimagegridPadding(const LPCANVAS hCanvas, LPEX_IMAGE image, float dstLeft, float dstTop, float dstRight, float dstBottom, float srcLeft, float srcTop, float srcRight, float srcBottom, float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom, DWORD dwFlags, BYTE alpha)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawimagegridPadding(hCanvas, image, dstLeft, dstTop, dstRight, dstBottom, srcLeft, srcTop, srcRight, srcBottom, gridPaddingLeft, gridPaddingTop, gridPaddingRight, gridPaddingBottom, dwFlags, alpha);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawtext(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawtext(hCanvas, font, hBrush, text, textLen, textFormat, rc);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawtextex(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc, DWORD iGlowsize, ARGB crShadow, LPDRAWTEXTPARAMS lParam, const RECT_F* prclayout, RECT_F* outrcText)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_drawtextex(hCanvas, font, hBrush, text, textLen, textFormat, rc, iGlowsize, crShadow, lParam, prclayout, outrcText);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_alphablend(const LPCANVAS hCanvas, float xoriginDest, float yoriginDest, float wDest, float hDest, const LPCANVAS sCanvas, float xoriginSrc, float yoriginSrc, float wSrc, float hSrc, BYTE Alpha)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_alphablend(hCanvas, xoriginDest, yoriginDest, wDest, hDest, sCanvas, xoriginSrc, yoriginSrc, wSrc, hSrc, Alpha);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_bitblt(const LPCANVAS hCanvas, int x, int y, int cx, int cy, const LPCANVAS sCanvas, int x1, int y1, DWORD rop)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_bitblt(hCanvas, x, y, cx, cy, sCanvas, x1, y1, rop);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_flush(const LPCANVAS hCanvas)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_flush(hCanvas);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_setrgn(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_setrgn(hCanvas, left, top, right, bottom, mode);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_setClip(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_setClip(hCanvas, left, top, right, bottom, mode);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_resetClip(const LPCANVAS hCanvas)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_resetClip(hCanvas);
}

// ���ÿ����ģʽ, 0=�رտ����, 1=����
// �˺������� _canvas_SetSmoothingMode, ����һģһ��, ���һ����Ϊ�˺�d2d����һ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_SetAntialiasMode(hCanvas, mode, oldMode);
}

// ���ÿ����ģʽ, 0=�رտ����, 1=����
// �˺������� _canvas_SetAntialiasMode ʵ��, ����һģһ��, ���һ����Ϊ�˺�gdi+����һ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetSmoothingMode(const LPCANVAS hCanvas, int mode, int* oldMode)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_SetSmoothingMode(hCanvas, mode, oldMode);
}

// �������忹���ģʽ, 0=Ĭ��, 1=ClearType�����(�������), 2=�Ҷȿ����, 3=��ʹ�ÿ����
// �˺������� _canvas_SetTextRenderingHint, ����һģһ��, ���һ����Ϊ�˺�d2d����һ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetTextAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_SetTextAntialiasMode(hCanvas, mode, oldMode);
}

// �������忹���ģʽ, 0=Ĭ��, 1=ClearType�����(�������), 2=�Ҷȿ����, 3=��ʹ�ÿ����
// �˺������� _canvas_SetTextAntialiasMode ʵ��, ����һģһ��, ���һ����Ϊ�˺�gdi+����һ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetTextRenderingHint(const LPCANVAS hCanvas, int mode, int* oldMode)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_canvas_SetTextRenderingHint(hCanvas, mode, oldMode);
}

// ��ˢ
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_brush_create(ARGB color)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_create(color);
}

// �������Խ��仭ˢ
// pt1,pt2 = ���, �յ�����
// color,colorCount = ��ɫ����, ��ɫ��������
// fillMode = ���ģʽ
// pRatios, ratiosCount = �����������, ��0-1, ��������
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_brush_create_LinearGradient(const POINT_F& pt1, const POINT_F& pt2, ARGB* color, DWORD colorCount, int fillMode, const float* pRatios, DWORD ratiosCount)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_create_LinearGradient(pt1, pt2, color, colorCount, fillMode, pRatios, ratiosCount);
}

// �������佥�仭ˢ, �����ĵ�������ɢ����
// pos = �������ĵ��Լ�������չ�ĳ���
// color,colorCount = ��ɫ����, ��ɫ��������
// fillMode = ���ģʽ
// pRatios, ratiosCount = �����������, ��0-1, ��������
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_brush_create_RadialGradient(const ELLIPSE_F* pos, ARGB* color, DWORD colorCount, int fillMode, const float* pRatios, DWORD ratiosCount)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_create_RadialGradient(pos, color, colorCount, fillMode, pRatios, ratiosCount);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_destroy(LPEX_BRUSH& pBrush)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_destroy(pBrush);
}

EXTERN_C _declspec(dllexport) LPEX_PEN   WINAPI iDraw_pen_create(ARGB color, float width, int style, float* dashes, int dashesCount)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_pen_create(color, width, style, dashes, dashesCount);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_pen_destroy(LPEX_PEN& pBrush)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_pen_destroy(pBrush);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_pen_setcolor(LPEX_PEN pen, ARGB color)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_pen_setcolor(pen, color);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_setcolor(LPEX_BRUSH brush, ARGB color)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_setcolor(brush, color);
}

EXTERN_C _declspec(dllexport) DWORD      WINAPI iDraw_brush_getcolor(LPEX_BRUSH brush)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_getcolor(brush);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_setstartpoint(LPEX_BRUSH brush, float x, float y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_setstartpoint(brush, x, y);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_setendpoint(LPEX_BRUSH brush, float x, float y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_setendpoint(brush, x, y);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_getstartpoint(LPEX_BRUSH brush, float* x, float* y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_getstartpoint(brush, x, y);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_getendpoint(LPEX_BRUSH brush, float* x, float* y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_getendpoint(brush, x, y);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_setcenter(LPEX_BRUSH brush, float x, float y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_setcenter(brush, x, y);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_SetGradientOriginOffset(LPEX_BRUSH brush, float x, float y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_SetGradientOriginOffset(brush, x, y);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_SetRadiusX(LPEX_BRUSH brush, float x)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_SetRadiusX(brush, x);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_SetRadiusY(LPEX_BRUSH brush, float x)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_SetRadiusY(brush, x);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_GetCenter(LPEX_BRUSH brush, float* x, float* y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_GetCenter(brush, x, y);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_GetGradientOriginOffset(LPEX_BRUSH brush, float* x, float* y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_GetGradientOriginOffset(brush, x, y);
}

EXTERN_C _declspec(dllexport) float      WINAPI iDraw_brush_GetRadiusY(LPEX_BRUSH brush)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_brush_GetRadiusY(brush);
}

// ·��
EXTERN_C _declspec(dllexport) LPEX_PATH  WINAPI iDraw_path_create(int fillMode)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_create(fillMode);
}

EXTERN_C _declspec(dllexport) LPEX_PATH  WINAPI iDraw_path_createroundedrectangle(const LPCRECTROUNDED_F rcRounded, int fillMode)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_createroundedrectangle(rcRounded, fillMode);
}

EXTERN_C _declspec(dllexport) LPEX_PATH  WINAPI iDraw_path_createrectangle(const LPCRECT_F rcRounded, int fillMode)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_createrectangle(rcRounded, fillMode);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_destroy(LPEX_PATH& path)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_destroy(path);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_beginfigure(LPEX_PATH path, const LPCPOINT_F pt)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_beginfigure(path, pt);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_endfigure(LPEX_PATH path)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_endfigure(path);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_reset(LPEX_PATH path)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_reset(path);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addarc(LPEX_PATH path, float x1, float y1, float x2, float y2, float radiusX, float radiusY, bool fClockwise)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_addarc(path, x1, y1, x2, y2, radiusX, radiusY, fClockwise);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addbezier(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2, const POINT_F& pt3, const POINT_F& pt4)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_addbezier(path, pt1, pt2, pt3, pt4);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addline(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_addline(path, pt1, pt2);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addrect(LPEX_PATH path, const RECT_F& rc)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_addrect(path, rc);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addroundedrect(LPEX_PATH path, const RECT_F& rc, float radiusTopLeft, float radiusTopRight, float radiusBottomLeft, float radiusBottomRight)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_addroundedrect(path, rc, radiusTopLeft, radiusTopRight, radiusBottomLeft, radiusBottomRight);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_open(LPEX_PATH path)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_open(path);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_close(LPEX_PATH path)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_close(path);
}

// ȡ·���߽����
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_getbounds(LPEX_PATH path, LPRECT_F rc)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_getbounds(path, rc);
}

// ���������Ƿ��ڿɼ�·����
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_hittest(LPEX_PATH path, const POINT_F& pt)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_path_hittest(path, pt);
}

// ͼƬ
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_destroy(LPEX_IMAGE& img)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_destroy(img);
}

EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create(UINT width, UINT height)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_create(width, height);
}

EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_frommemory(const void* pData, DWORD len)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_create_frommemory(pData, len);
}

EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_fromfile(LPCWSTR file)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_create_fromfile(file);
}

EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_frombitmap(HBITMAP hBitmap, HPALETTE hPalette, bool fPreAlpha)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_create_frombitmap(hBitmap, hPalette, fPreAlpha);
}

EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_formicon(HICON hIcon)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_create_formicon(hIcon);
}

EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_fromstream(IStream* stream)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_create_fromstream(stream);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_getsize(LPEX_IMAGE image, UINT* width, UINT* height)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_getsize(image, width, height);
}

EXTERN_C _declspec(dllexport) UINT       WINAPI iDraw_img_width(LPEX_IMAGE image)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_width(image);
}

EXTERN_C _declspec(dllexport) UINT       WINAPI iDraw_img_height(LPEX_IMAGE image)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_height(image);
}

EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_copyrect(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_copyrect(image, x, y, width, height);
}

EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_copy(LPEX_IMAGE image)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_copy(image);
}

// ����λͼ, �ɹ�����true, ʧ�ܷ���false, ��ʹ��ʱ������� _img_unlock �ͷ�����
// x,y,width,height = �󶥿��, ���Ϊ0ʱ���Զ���ȡͼƬ��С
// flags = 1=��, 2=д, 3=��д
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_lock(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_lock(image, x, y, width, height, flags, lockData);
}

// 
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_unlock(LPEX_IMAGE image, EX_IMAGELOCK* lockData)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_unlock(image, lockData);
}

// ȡͼ��֡��
EXTERN_C _declspec(dllexport) UINT       WINAPI iDraw_img_getframecount(LPEX_IMAGE image)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_getframecount(image);
}

// ���õ�ǰ�֡
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_selectactiveframe(LPEX_IMAGE image, UINT index)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_selectactiveframe(image, index);
}

// ��D2Dλͼָ���󶥿�߻�ȡͼƬ��Ч������, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_frombitmap(ID2D1Image* pBitmap, SIZE_T& size, UINT left, UINT top, UINT right, UINT bottom)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_save_frombitmap(pBitmap, &size, left, top, right, bottom);
}

// �ӻ滭������ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_formcanvas(const LPCANVAS hCanvas, SIZE_T& size)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_save_formcanvas(hCanvas, &size);
}

// �ӻ滭���ָ���󶥿�߻�ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_formcanvasex(const LPCANVAS hCanvas, SIZE_T& size, UINT left, UINT top, UINT right, UINT bottom)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_save_formcanvasex(hCanvas, &size, left, top, right, bottom);
}

// ��ͼƬ�����ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save(LPEX_IMAGE image, SIZE_T& size)
{
    if (!s_info || !s_info->d2d)
        return 0;
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_save(image, &size);
}

// ��ͼƬ���ָ���󶥿�߻�ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_fromimage(LPEX_IMAGE image, SIZE_T& size, UINT left, UINT top, UINT right, UINT bottom)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_save_rect(image, &size, left, top, right, bottom);
}

EXTERN_C _declspec(dllexport) void       WINAPI iDraw_img_free(LPBYTE ptr)
{
    if (!s_info || !s_info->d2d)
        return;
    return s_info->d2d->_img_free(ptr);
}

// ��ͼƬת��gdi��λͼ
EXTERN_C _declspec(dllexport) HBITMAP    WINAPI iDraw_img_ToGdiBitmap(LPEX_IMAGE image)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_img_ToGdiBitmap(image);
}

//// λͼ
//virtual LPEX_IMAGE _bitmap_create(UINT width, UINT height, bool fGdi) = 0;
// ����
// ��LOGFONTW �ﴴ������
EXTERN_C _declspec(dllexport) LPEX_FONT  WINAPI iDraw_font_create_logfont(const LPLOGFONTW logFont, DWORD flags)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_font_create_logfont(logFont, flags);
}

// ���������д�������, ���������Ϊ0�򴴽�Ĭ������
EXTERN_C _declspec(dllexport) LPEX_FONT  WINAPI iDraw_font_create(LPCWSTR name, LONG lfHeight, FONTSTYLE fontStyle)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_font_create(name, lfHeight, fontStyle);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_font_destroy(LPEX_FONT& font)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_font_destroy(font);
}

// �ַ��߶�ת�����С, �ַ��߶�һ���� LOGFONT �� lfHeight, �����Сһ���������Ե������С
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_font_height2size(int height)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_font_height2size(height);
}

// �����Сת�ַ��߶�, �ַ��߶�һ���� LOGFONT �� lfHeight, �����Сһ���������Ե������С
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_font_size2height(int size)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_font_size2height(size);
}

// ����
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_rgn_destroy(LPEX_RGN& hRgn)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_rgn_destroy(hRgn);
}

EXTERN_C _declspec(dllexport) LPEX_RGN   WINAPI iDraw_rgn_create(float left, float top, float right, float bottom)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_rgn_create(left, top, right, bottom);
}

EXTERN_C _declspec(dllexport) LPEX_RGN   WINAPI iDraw_rgn_create_round(float left, float top, float right, float bottom, float radiusX, float radiusY)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_rgn_create_round(left, top, right, bottom, radiusX, radiusY);
}

EXTERN_C _declspec(dllexport) LPEX_RGN   WINAPI iDraw_rgn_combine(LPEX_RGN hRgnSrc, LPEX_RGN hRgnDst, DWORD nCombineMode, float dstOffsetX, float dstOffsetY)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_rgn_combine(hRgnSrc, hRgnDst, nCombineMode, dstOffsetX, dstOffsetY);
}

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_rgn_hittest(LPEX_RGN hRgn, float x, float y)
{
    if (!s_info || !s_info->d2d)
        return 0;
    return s_info->d2d->_rgn_hittest(hRgn, x, y);
}

#endif