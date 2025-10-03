#pragma once
#include <IDraw_Header.h>
#include <iDraw_Version.h>
#include <iDraw_const.h>

extern std::unordered_map<int, PLUGIN_INFO>     s_plugin_info;      // 所有插件信息
extern std::unordered_map<int, LPPLUGIN_INFO>   s_plugin_info_hash; // 记录已经加载的插件, hash值是外部传递进来的guid地址
extern std::vector<LPPLUGIN_INFO>               s_plugin_array;     // 所有插件信息, 按加入顺序记录, s_plugin_info 这个不是按顺序, 两个记录的数据一样

LPPLUGIN_INFO Plugin_Init(LPCSTR guid, HMODULE hModule, ETOOLS_PLUGIN_INFO* etoolsInfo, pfn_MenuExt_Command pfnMenuExt_Command, pfn_IDraw_Callback callback);
LPPLUGIN_INFO Plugin_GetInfo(LPCSTR guid);
LPPLUGIN_INFO Plugin_Query(LPCSTR hash);
int Plugin_Enum(LPPLUGIN_INFO* pInfo, int bufCount);




//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取pInfo的信息, 接口程序的第一个参数
// 
// 参数:
//      参数1: 接口程序的第一个参数
//      参数2: 获取的编号, IGI_ 开头常量
//      参数3: 获取的参数, 根据编号传递
// 
// 返回值:
//      获取到的值
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_GetInfo(IDRAW_INFO* pInfo, int index, LPARAM argument);

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置pInfo的信息, 接口程序的第一个参数, 谨慎设置, 这里的配置是全局使用, 一旦出错就会导致崩溃
// 
// 参数:
//      参数1: 接口程序的第一个参数
//      参数2: 获取的编号, IGI_ 开头常量
//      参数3: 要设置的值
//      参数3: 是否重画窗口, 仅针对部分影响窗口的配置有效
// 
// 返回值:
//      返回设置之前的值
EXTERN_C _declspec(dllexport) LONG_PTR WINAPI iDraw_SetInfo(IDRAW_INFO* pInfo, int index, LONG_PTR value, BOOL isUpdatwWindow);


EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_isDxRender();
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_destroy(LPCANVAS& hCanvas);
EXTERN_C _declspec(dllexport) LPCANVAS   WINAPI iDraw_canvas_create(HWND hWnd, UINT width, UINT height);
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_ExSetLastError(int code);
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_ExGetLastError();
// 绘制相关
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_clear(const LPCANVAS hCanvas, ARGB argb);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetTarget(const LPCANVAS hCanvas, LPEX_IMAGE bitmap);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_ReTarget(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) LPEX_IMAGE WINAPI iDraw_canvas_GetTarget(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) void       WINAPI iDraw_canvas_FreeTarget(const LPCANVAS hCanvas, LPEX_IMAGE image);

EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_begindraw(const LPCANVAS hCanvas);
// 平移
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_translate(const LPCANVAS hCanvas, float x, float y);
// 缩放
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_scale(const LPCANVAS hCanvas, float x, float y, float scaleX, float scaleY);
// 旋转
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_rotate(const LPCANVAS hCanvas, float x, float y, float rotate);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_enddraw(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_resize(const LPCANVAS hCanvas, int cx, int cy);
EXTERN_C _declspec(dllexport) HDC        WINAPI iDraw_canvas_getdc(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) void       WINAPI iDraw_canvas_releasedc(const LPCANVAS hCanvas);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_calctextsize(const LPCANVAS hCanvas, LPEX_FONT font, LPCWSTR text, size_t textLen, DWORD textFormat, float layoutWidth, float layoutHeight, float* pWidth, float* pHeight);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_getsize(const LPCANVAS hCanvas, UINT* width, UINT* height);
// 绘制简单图形
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
// 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
// 此函数调与 _canvas_SetSmoothingMode, 功能一模一样, 多加一个是为了和d2d名字一样
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode);
// 设置抗锯齿模式, 0=关闭抗锯齿, 1=开启
// 此函数调用 _canvas_SetAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetSmoothingMode(const LPCANVAS hCanvas, int mode, int* oldMode);
// 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
// 此函数调与 _canvas_SetTextRenderingHint, 功能一模一样, 多加一个是为了和d2d名字一样
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetTextAntialiasMode(const LPCANVAS hCanvas, int mode, int* oldMode);
// 设置字体抗锯齿模式, 0=默认, 1=ClearType抗锯齿(最高质量), 2=灰度抗锯齿, 3=不使用抗锯齿
// 此函数调用 _canvas_SetTextAntialiasMode 实现, 功能一模一样, 多加一个是为了和gdi+名字一样
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_canvas_SetTextRenderingHint(const LPCANVAS hCanvas, int mode, int* oldMode);
// 画刷
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_brush_create(ARGB color);
// 创建线性渐变画刷
// pt1,pt2 = 起点, 终点坐标
// color,colorCount = 颜色数组, 颜色数组数量
// fillMode = 填充模式
// pRatios, ratiosCount = 渐变比例数组, 从0-1, 数组数量
EXTERN_C _declspec(dllexport) LPEX_BRUSH WINAPI iDraw_brush_create_LinearGradient(const POINT_F& pt1, const POINT_F& pt2, ARGB* color, DWORD colorCount, int fillMode, const float* pRatios, DWORD ratiosCount);
// 创建放射渐变画刷, 从中心点往外扩散渐变
// pos = 包含中心点以及向外扩展的长度
// color,colorCount = 颜色数组, 颜色数组数量
// fillMode = 填充模式
// pRatios, ratiosCount = 渐变比例数组, 从0-1, 数组数量
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
// 路径
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
// 取路径边界矩形
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_getbounds(LPEX_PATH path, LPRECT_F rc);
// 测试坐标是否在可见路径内
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_path_hittest(LPEX_PATH path, const POINT_F& pt);
// 图片
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
// 锁定位图, 成功返回true, 失败返回false, 不使用时必须调用 _img_unlock 释放数据
// x,y,width,height = 左顶宽高, 宽高为0时会自动获取图片大小
// flags = 1=读, 2=写, 3=读写
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_lock(LPEX_IMAGE image, UINT x, UINT y, UINT width, UINT height, DWORD flags, EX_IMAGELOCK* lockData);
// 
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_unlock(LPEX_IMAGE image, EX_IMAGELOCK* lockData);
// 取图像帧数
EXTERN_C _declspec(dllexport) UINT       WINAPI iDraw_img_getframecount(LPEX_IMAGE image);
// 设置当前活动帧
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_img_selectactiveframe(LPEX_IMAGE image, UINT index);
// 从D2D位图指定左顶宽高获取图片或效果数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_frombitmap(ID2D1Image* pBitmap, SIZE_T& size, UINT left, UINT top, UINT right, UINT bottom);
// 从绘画句柄里获取图片数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_formcanvas(const LPCANVAS hCanvas, SIZE_T& size);
// 从绘画句柄指定左顶宽高获取图片数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_formcanvasex(const LPCANVAS hCanvas, SIZE_T& size, UINT left, UINT top, UINT right, UINT bottom);
// 从图片句柄获取图片数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save(LPEX_IMAGE image, SIZE_T& size);
// 从图片句柄指定左顶宽高获取图片数据, 返回图片数据指针和尺寸, 需要调用 _img_free 释放
EXTERN_C _declspec(dllexport) LPBYTE     WINAPI iDraw_img_save_fromimage(LPEX_IMAGE image, SIZE_T& size, UINT left, UINT top, UINT right, UINT bottom);
EXTERN_C _declspec(dllexport) void       WINAPI iDraw_img_free(LPBYTE ptr);
// 把图片转成gdi的位图
EXTERN_C _declspec(dllexport) HBITMAP    WINAPI iDraw_img_ToGdiBitmap(LPEX_IMAGE image);
//// 位图
//virtual LPEX_IMAGE _bitmap_create(UINT width, UINT height, bool fGdi) = 0;
// 字体
// 从LOGFONTW 里创建字体
EXTERN_C _declspec(dllexport) LPEX_FONT  WINAPI iDraw_font_create_logfont(const LPLOGFONTW logFont, DWORD flags);
// 从字体名中创建字体, 如果参数都为0则创建默认字体
EXTERN_C _declspec(dllexport) LPEX_FONT  WINAPI iDraw_font_create(LPCWSTR name, LONG lfHeight, FONTSTYLE fontStyle);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_font_destroy(LPEX_FONT& font);
// 字符高度转字体大小, 字符高度一般是 LOGFONT 的 lfHeight, 字体大小一般是易语言的字体大小
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_font_height2size(int height);
// 字体大小转字符高度, 字符高度一般是 LOGFONT 的 lfHeight, 字体大小一般是易语言的字体大小
EXTERN_C _declspec(dllexport) int        WINAPI iDraw_font_size2height(int size);
// 区域
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_rgn_destroy(LPEX_RGN& hRgn);
EXTERN_C _declspec(dllexport) LPEX_RGN   WINAPI iDraw_rgn_create(float left, float top, float right, float bottom);
EXTERN_C _declspec(dllexport) LPEX_RGN   WINAPI iDraw_rgn_create_round(float left, float top, float right, float bottom, float radiusX, float radiusY);
EXTERN_C _declspec(dllexport) LPEX_RGN   WINAPI iDraw_rgn_combine(LPEX_RGN hRgnSrc, LPEX_RGN hRgnDst, DWORD nCombineMode, float dstOffsetX, float dstOffsetY);
EXTERN_C _declspec(dllexport) bool       WINAPI iDraw_rgn_hittest(LPEX_RGN hRgn, float x, float y);



//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      添加按钮到接口工具条中, 成功返回工具条按钮ID, 触发工具条按钮被点击时会把这个ID传递过去
// 
// 参数:
//      参数1: 插件唯一标识符, 触发点击/重画事件时会调用这个插件的回调函数
//      参数2: 按钮的ID, 触发事件时会把这个ID传递过去
//      参数3: 按钮标题, 可以为0, 为0则没有标题, 鼠标移动上去时弹出的提示说明标题, 加粗显示, 这个标题不会显示在工具条上
//      参数4: 按钮说明, 可以为0, 为0则没有说明, 鼠标移动上去时弹出的提示说明
//      参数5: 项目数值, 会跟随点击事件传递过去
//      参数6: 图片数据指针, 这个是显示到工具条上的图标
//      参数7: 图片数据大小
//      参数8: 绘画回调函数, pfn_ToolDrawButton 类型
//              typedef int(WINAPI* pfn_ToolDrawButton)(LPCANVAS hCanvas, int id, LPARAM param, const RECT_F* prc, STATE state, UINT dpi);
// 返回值:
//      成功返回工具条按钮ID, 失败返回0
EXTERN_C _declspec(dllexport) int WINAPI iDraw_Tool_InsertButton(LPCSTR guid, int idCommand, LPCWSTR lpszTitle, LPCWSTR lpszTips, LPARAM param, const void* pImageData, int nImageSize, pfn_ToolDrawButton pfnDraw);

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      删除接口工具条的按钮, 只能删除自己插件添加的按钮
// 
// 参数:
//      参数1: 插件唯一标识符, 触发点击/重画事件时会调用这个插件的回调函数
//      参数2: 按钮的ID, 添加时的ID
// 
// 返回值:
//      返回是否成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_DeleteButton(LPCSTR guid, int idCommand);

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      获取接口工具条的按钮信息, 只能获取自己插件添加的按钮
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 按钮的ID, 添加时的ID
//      参数3: 为0则不接收, 接收按钮标题
//      参数4: 为0则不接收, 接收按钮说明
//      参数5: 为0则不接收, 接收项目数值
//      参数6: 为0则不接收, 接收图片数据指针
//      参数7: 为0则不接收, 接收图片数据大小
//      参数8: 为0则不接收, 接收绘画回调函数
// 
// 返回值:
//      返回是否成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_GetButtonData(LPCSTR guid, int idCommand, LPWSTR* lpszTitle, LPWSTR* lpszTips, LPARAM* param, LPCVOID* pImageData, int* nImageSize, pfn_ToolDrawButton* pfnDraw);

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      设置接口工具条的按钮信息, 只能设置自己插件添加的按钮
// 
// 参数:
//      参数1: 插件唯一标识符
//      参数2: 按钮的ID, 添加时的ID
//      参数3: 为0则不设置, 设置按钮标题
//      参数4: 为0则不设置, 设置按钮说明
//      参数5: 为0则不设置, 设置项目数值
//      参数6: 为0则不设置, 设置图片数据指针
//      参数7: 为0则不设置, 设置图片数据大小
//      参数8: 为0则不设置, 设置绘画回调函数
// 
// 返回值:
//      返回是否成功
EXTERN_C _declspec(dllexport) bool WINAPI iDraw_Tool_SetButtonData(LPCSTR guid, int idCommand, LPCWSTR lpszTitle, LPCWSTR lpszTips, LPARAM param, const void* pImageData, int nImageSize, pfn_ToolDrawButton pfnDraw);
