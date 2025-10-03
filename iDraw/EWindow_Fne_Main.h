#pragma once
#include <IDraw_Header.h>
#include <iDraw_Version.h>
#include <iDraw_const.h>

extern std::unordered_map<int, PLUGIN_INFO>     s_plugin_info;      // ���в����Ϣ
extern std::unordered_map<int, LPPLUGIN_INFO>   s_plugin_info_hash; // ��¼�Ѿ����صĲ��, hashֵ���ⲿ���ݽ�����guid��ַ
extern std::vector<LPPLUGIN_INFO>               s_plugin_array;     // ���в����Ϣ, ������˳���¼, s_plugin_info ������ǰ�˳��, ������¼������һ��

LPPLUGIN_INFO Plugin_Init(LPCSTR guid, HMODULE hModule, ETOOLS_PLUGIN_INFO* etoolsInfo, pfn_MenuExt_Command pfnMenuExt_Command, pfn_IDraw_Callback callback);
LPPLUGIN_INFO Plugin_GetInfo(LPCSTR guid);
LPPLUGIN_INFO Plugin_Query(LPCSTR hash);
int Plugin_Enum(LPPLUGIN_INFO* pInfo, int bufCount);




//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡpInfo����Ϣ, �ӿڳ���ĵ�һ������
// 
// ����:
//      ����1: �ӿڳ���ĵ�һ������
//      ����2: ��ȡ�ı��, IGI_ ��ͷ����
//      ����3: ��ȡ�Ĳ���, ���ݱ�Ŵ���
// 
// ����ֵ:
//      ��ȡ����ֵ
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_GetInfo(IDRAW_INFO* pInfo, int index, LPARAM argument);

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ����pInfo����Ϣ, �ӿڳ���ĵ�һ������, ��������, �����������ȫ��ʹ��, һ������ͻᵼ�±���
// 
// ����:
//      ����1: �ӿڳ���ĵ�һ������
//      ����2: ��ȡ�ı��, IGI_ ��ͷ����
//      ����3: Ҫ���õ�ֵ
//      ����3: �Ƿ��ػ�����, ����Բ���Ӱ�촰�ڵ�������Ч
// 
// ����ֵ:
//      ��������֮ǰ��ֵ
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_SetInfo(IDRAW_INFO* pInfo, int index, LONG_PTR value, BOOL isUpdatwWindow);


EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_isDxRender();
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_destroy(LPCANVAS& hCanvas);
EXTERN_C _declspec(dllexport) LPCANVAS   WINAPI iDraw_canvas_create(HWND hWnd, UINT width, UINT height);
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_ExSetLastError(int code);
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_ExGetLastError();
// �������
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_clear(const LPCANVAS hCanvas, ARGB argb);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetTarget(const LPCANVAS hCanvas, LPEX_IMAGE bitmap);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_ReTarget(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_canvas_GetTarget(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) void       WINAPI iDraw_canvas_FreeTarget(const LPCANVAS hCanvas, LPEX_IMAGE image);

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_begindraw(const LPCANVAS hCanvas);
// ƽ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_translate(const LPCANVAS hCanvas, float x, float y);
// ����
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_scale(const LPCANVAS hCanvas, float x, float y, float scaleX, float scaleY);
// ��ת
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_rotate(const LPCANVAS hCanvas, float x, float y, float rotate);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_enddraw(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_resize(const LPCANVAS hCanvas, int cx, int cy);
EXTERN_C _declspec(dllexport) HDC        WINAPI iDraw_canvas_getdc(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) void       WINAPI iDraw_canvas_releasedc(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_calctextsize(const LPCANVAS hCanvas, LPEX_FONT font, LPCWSTR text, size_t textLen, DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_getsize(const LPCANVAS hCanvas, UINT* width, UINT* height);
// ���Ƽ�ͼ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECT_F rcF);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_fillRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECT_F rcF);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawEllipse(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCELLIPSE_F rcF);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_fillEllipse(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCELLIPSE_F rcF);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawRoundedRectangle(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCRECTROUNDED_F rcRounded);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_fillRoundedRectangle(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPCRECTROUNDED_F rcRounded);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawLine(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPCLINEPOINT_F pt);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_fillpath(const LPCANVAS hCanvas, const LPEX_BRUSH hBrush, const LPEX_PATH path);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawpath(const LPCANVAS hCanvas, const LPEX_PEN hPen, const LPEX_PATH path);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawimage(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, BYTE alpha);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawimagerect(const LPCANVAS hCanvas, LPEX_IMAGE image, float left, float top, float right, float bottom, BYTE alpha);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawimagerectrect(const LPCANVAS hCanvas, LPEX_IMAGE image, float dstLeft, float dstTop, float dstRight, float dstBottom, float srcLeft, float srcTop, float srcRight, float srcBottom, BYTE alpha);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawimagegridPadding(const LPCANVAS hCanvas, LPEX_IMAGE image, float dstLeft, float dstTop, float dstRight, float dstBottom, float srcLeft, float srcTop, float srcRight, float srcBottom, float gridPaddingLeft, float gridPaddingTop, float gridPaddingRight, float gridPaddingBottom, DWORD dwFlags, BYTE alpha);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawtext(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_drawtextex(const LPCANVAS hCanvas, LPEX_FONT font, LPEX_BRUSH hBrush, LPCWSTR text, size_t textLen, DWORD textFormat, const RECT_F* rc, DWORD iGlowsize, ARGB crShadow, LPDRAWTEXTPARAMS lParam, const RECT_F* prclayout, RECT_F* outrcText);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_alphablend(const LPCANVAS hCanvas, float xoriginDest, float yoriginDest, float wDest, float hDest, const LPCANVAS sCanvas, float xoriginSrc, float yoriginSrc, float wSrc, float hSrc, BYTE Alpha);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_bitblt(const LPCANVAS hCanvas, int x, int y, int cx, int cy, const LPCANVAS sCanvas, int x1, int y1, DWORD rop);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_flush(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_setrgn(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_setClip(const LPCANVAS hCanvas, float left, float top, float right, float bottom, int mode);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_resetClip(const LPCANVAS hCanvas);
// ���ÿ����ģʽ, 0=�رտ����, 1=����
// �˺������� _canvas_SetSmoothingMode, ����һģһ��, ���һ����Ϊ�˺�d2d����һ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode);
// ���ÿ����ģʽ, 0=�رտ����, 1=����
// �˺������� _canvas_SetAntialiasMode ʵ��, ����һģһ��, ���һ����Ϊ�˺�gdi+����һ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetSmoothingMode(const LPCANVAS hCanvas, int mode, int* oldMode);
// �������忹���ģʽ, 0=Ĭ��, 1=ClearType�����(�������), 2=�Ҷȿ����, 3=��ʹ�ÿ����
// �˺������� _canvas_SetTextRenderingHint, ����һģһ��, ���һ����Ϊ�˺�d2d����һ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetTextAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode);
// �������忹���ģʽ, 0=Ĭ��, 1=ClearType�����(�������), 2=�Ҷȿ����, 3=��ʹ�ÿ����
// �˺������� _canvas_SetTextAntialiasMode ʵ��, ����һģһ��, ���һ����Ϊ�˺�gdi+����һ��
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetTextRenderingHint(const LPCANVAS hCanvas, int mode, int* oldMode);
// ��ˢ
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_brush_create(ARGB color);
// �������Խ��仭ˢ
// pt1,pt2 = ���, �յ�����
// color,colorCount = ��ɫ����, ��ɫ��������
// fillMode = ���ģʽ
// pRatios, ratiosCount = �����������, ��0-1, ��������
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_brush_create_LinearGradient(const POINT_F& pt1, const POINT_F& pt2, ARGB* color, DWORD colorCount, int fillMode, const float* pRatios, DWORD ratiosCount);
// �������佥�仭ˢ, �����ĵ�������ɢ����
// pos = �������ĵ��Լ�������չ�ĳ���
// color,colorCount = ��ɫ����, ��ɫ��������
// fillMode = ���ģʽ
// pRatios, ratiosCount = �����������, ��0-1, ��������
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_brush_create_RadialGradient(const ELLIPSE_F* pos, ARGB* color, DWORD colorCount, int fillMode, const float* pRatios, DWORD ratiosCount);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_destroy(LPEX_BRUSH& pBrush);
EXTERN_C _declspec(dllexport) LPEX_PEN   WINAPI iDraw_pen_create(ARGB color, float width, int style, float* dashes, int dashesCount);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_pen_destroy(LPEX_PEN& pPen);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_pen_setcolor(LPEX_PEN pen, ARGB color);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_setcolor(LPEX_BRUSH brush, ARGB color);
EXTERN_C _declspec(dllexport) DWORD      WINAPI iDraw_brush_getcolor(LPEX_BRUSH brush);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_setstartpoint(LPEX_BRUSH brush, float x, float y);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_setendpoint(LPEX_BRUSH brush, float x, float y);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_getstartpoint(LPEX_BRUSH brush, float* x, float* y);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_getendpoint(LPEX_BRUSH brush, float* x, float* y);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_setcenter(LPEX_BRUSH brush, float x, float y);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_SetGradientOriginOffset(LPEX_BRUSH brush, float x, float y);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_SetRadiusX(LPEX_BRUSH brush, float x);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_SetRadiusY(LPEX_BRUSH brush, float x);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_GetCenter(LPEX_BRUSH brush, float* x, float* y);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_brush_GetGradientOriginOffset(LPEX_BRUSH brush, float* x, float* y);
EXTERN_C _declspec(dllexport) float      WINAPI iDraw_brush_GetRadiusY(LPEX_BRUSH brush);
// ·��
EXTERN_C _declspec(dllexport) LPEX_PATH  WINAPI iDraw_path_create(int fillMode);
EXTERN_C _declspec(dllexport) LPEX_PATH  WINAPI iDraw_path_createroundedrectangle(const LPCRECTROUNDED_F rcRounded, int fillMode);
EXTERN_C _declspec(dllexport) LPEX_PATH  WINAPI iDraw_path_createrectangle(const LPCRECT_F rcRounded, int fillMode);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_destroy(LPEX_PATH& path);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_beginfigure(LPEX_PATH path, const LPCPOINT_F pt);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_endfigure(LPEX_PATH path);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_reset(LPEX_PATH path);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addarc(LPEX_PATH path, float x1, float y1, float x2, float y2, float radiusX, float radiusY, bool fClockwise);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addbezier(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2, const POINT_F& pt3, const POINT_F& pt4);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addline(LPEX_PATH path, const POINT_F& pt1, const POINT_F& pt2);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addrect(LPEX_PATH path, const RECT_F& rc);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_addroundedrect(LPEX_PATH path, const RECT_F& rc, float radiusTopLeft, float radiusTopRight, float radiusBottomLeft, float radiusBottomRight);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_open(LPEX_PATH path);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_close(LPEX_PATH path);
// ȡ·���߽����
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_getbounds(LPEX_PATH path, LPRECT_F rc);
// ���������Ƿ��ڿɼ�·����
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_hittest(LPEX_PATH path, const POINT_F& pt);
// ͼƬ
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_destroy(LPEX_IMAGE& img);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create(UINT width, UINT height);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_frommemory(const void* pData, DWORD len);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_fromfile(LPCWSTR file);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_frombitmap(HBITMAP hBitmap, HPALETTE hPalette, bool fPreAlpha);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_formicon(HICON hIcon);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_create_fromstream(IStream* stream);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_getsize(LPEX_IMAGE image, UINT* width, UINT* height);
EXTERN_C _declspec(dllexport) UINT       WINAPI iDraw_img_width(LPEX_IMAGE image);
EXTERN_C _declspec(dllexport) UINT       WINAPI iDraw_img_height(LPEX_IMAGE image);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_copyrect(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_img_copy(LPEX_IMAGE image);
// ����λͼ, �ɹ�����true, ʧ�ܷ���false, ��ʹ��ʱ������� _img_unlock �ͷ�����
// x,y,width,height = �󶥿��, ���Ϊ0ʱ���Զ���ȡͼƬ��С
// flags = 1=��, 2=д, 3=��д
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_lock(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData);
// 
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_unlock(LPEX_IMAGE image, EX_IMAGELOCK* lockData);
// ȡͼ��֡��
EXTERN_C _declspec(dllexport) UINT       WINAPI iDraw_img_getframecount(LPEX_IMAGE image);
// ���õ�ǰ�֡
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_selectactiveframe(LPEX_IMAGE image, UINT index);
// ��D2Dλͼָ���󶥿�߻�ȡͼƬ��Ч������, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_frombitmap(ID2D1Image* pBitmap, SIZE_T& size, UINT left, UINT top, UINT right, UINT bottom);
// �ӻ滭������ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_formcanvas(const LPCANVAS hCanvas, SIZE_T& size);
// �ӻ滭���ָ���󶥿�߻�ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_formcanvasex(const LPCANVAS hCanvas, SIZE_T& size, UINT left, UINT top, UINT right, UINT bottom);
// ��ͼƬ�����ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save(LPEX_IMAGE image, SIZE_T& size);
// ��ͼƬ���ָ���󶥿�߻�ȡͼƬ����, ����ͼƬ����ָ��ͳߴ�, ��Ҫ���� _img_free �ͷ�
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_fromimage(LPEX_IMAGE image, SIZE_T& size, UINT left, UINT top, UINT right, UINT bottom);
EXTERN_C _declspec(dllexport) void       WINAPI iDraw_img_free(LPBYTE ptr);
// ��ͼƬת��gdi��λͼ
EXTERN_C _declspec(dllexport) HBITMAP    WINAPI iDraw_img_ToGdiBitmap(LPEX_IMAGE image);
//// λͼ
//virtual LPEX_IMAGE _bitmap_create(UINT width, UINT height, bool fGdi) = 0;
// ����
// ��LOGFONTW �ﴴ������
EXTERN_C _declspec(dllexport) LPEX_FONT  WINAPI iDraw_font_create_logfont(const LPLOGFONTW logFont, DWORD flags);
// ���������д�������, ���������Ϊ0�򴴽�Ĭ������
EXTERN_C _declspec(dllexport) LPEX_FONT  WINAPI iDraw_font_create(LPCWSTR name, LONG lfHeight, FONTSTYLE fontStyle);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_font_destroy(LPEX_FONT& font);
// �ַ��߶�ת�����С, �ַ��߶�һ���� LOGFONT �� lfHeight, �����Сһ���������Ե������С
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_font_height2size(int height);
// �����Сת�ַ��߶�, �ַ��߶�һ���� LOGFONT �� lfHeight, �����Сһ���������Ե������С
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_font_size2height(int size);
// ����
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_rgn_destroy(LPEX_RGN& hRgn);
EXTERN_C _declspec(dllexport) LPEX_RGN   WINAPI iDraw_rgn_create(float left, float top, float right, float bottom);
EXTERN_C _declspec(dllexport) LPEX_RGN   WINAPI iDraw_rgn_create_round(float left, float top, float right, float bottom, float radiusX, float radiusY);
EXTERN_C _declspec(dllexport) LPEX_RGN   WINAPI iDraw_rgn_combine(LPEX_RGN hRgnSrc, LPEX_RGN hRgnDst, DWORD nCombineMode, float dstOffsetX, float dstOffsetY);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_rgn_hittest(LPEX_RGN hRgn, float x, float y);



//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��Ӱ�ť���ӿڹ�������, �ɹ����ع�������ťID, ������������ť�����ʱ������ID���ݹ�ȥ
// 
// ����:
//      ����1: ���Ψһ��ʶ��, �������/�ػ��¼�ʱ������������Ļص�����
//      ����2: ��ť��ID, �����¼�ʱ������ID���ݹ�ȥ
//      ����3: ��ť����, ����Ϊ0, Ϊ0��û�б���, ����ƶ���ȥʱ��������ʾ˵������, �Ӵ���ʾ, ������ⲻ����ʾ�ڹ�������
//      ����4: ��ť˵��, ����Ϊ0, Ϊ0��û��˵��, ����ƶ���ȥʱ��������ʾ˵��
//      ����5: ��Ŀ��ֵ, ��������¼����ݹ�ȥ
//      ����6: ͼƬ����ָ��, �������ʾ���������ϵ�ͼ��
//      ����7: ͼƬ���ݴ�С
//      ����8: �滭�ص�����, pfn_ToolDrawButton ����
//              typedef int(WINAPI* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);
// ����ֵ:
//      �ɹ����ع�������ťID, ʧ�ܷ���0
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Tool_InsertButton(LPCSTR guid, int idCommand, LPCWSTR lpszTitle, LPCWSTR lpszTips, LPARAM param, const void* pImageData, int nImageSize, pfn_ToolDrawButton pfnDraw);

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ɾ���ӿڹ������İ�ť, ֻ��ɾ���Լ������ӵİ�ť
// 
// ����:
//      ����1: ���Ψһ��ʶ��, �������/�ػ��¼�ʱ������������Ļص�����
//      ����2: ��ť��ID, ���ʱ��ID
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_DeleteButton(LPCSTR guid, int idCommand);

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ��ȡ�ӿڹ������İ�ť��Ϣ, ֻ�ܻ�ȡ�Լ������ӵİ�ť
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: ��ť��ID, ���ʱ��ID
//      ����3: Ϊ0�򲻽���, ���հ�ť����
//      ����4: Ϊ0�򲻽���, ���հ�ť˵��
//      ����5: Ϊ0�򲻽���, ������Ŀ��ֵ
//      ����6: Ϊ0�򲻽���, ����ͼƬ����ָ��
//      ����7: Ϊ0�򲻽���, ����ͼƬ���ݴ�С
//      ����8: Ϊ0�򲻽���, ���ջ滭�ص�����
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_GetButtonData(LPCSTR guid, int idCommand, LPWSTR* lpszTitle, LPWSTR* lpszTips, LPARAM* param, LPCVOID* pImageData, int* nImageSize, pfn_ToolDrawButton* pfnDraw);

//////////////////////////////////////////////////////////////////////////
// ��������: 
//      ���ýӿڹ������İ�ť��Ϣ, ֻ�������Լ������ӵİ�ť
// 
// ����:
//      ����1: ���Ψһ��ʶ��
//      ����2: ��ť��ID, ���ʱ��ID
//      ����3: Ϊ0������, ���ð�ť����
//      ����4: Ϊ0������, ���ð�ť˵��
//      ����5: Ϊ0������, ������Ŀ��ֵ
//      ����6: Ϊ0������, ����ͼƬ����ָ��
//      ����7: Ϊ0������, ����ͼƬ���ݴ�С
//      ����8: Ϊ0������, ���û滭�ص�����
// 
// ����ֵ:
//      �����Ƿ�ɹ�
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_SetButtonData(LPCSTR guid, int idCommand, LPCWSTR lpszTitle, LPCWSTR lpszTips, LPARAM param, const void* pImageData, int nImageSize, pfn_ToolDrawButton pfnDraw);
