#include "../EWindow_Fne_Header.h"
#include "../EWindow.h"
#include <control/CShadow.h>
#include <iDraw_const.h>

class _CShadow : public ICShadow
{
    CShadow shadow;
public:
    // 创建阴影窗口, 根据 rcWindow 参数调整阴影窗口的大小和位置, 阴影位置是 rcWindow.左边顶边-10, 右边底边+10
    // argbColorShadow = 阴影RGB颜色, 默认是黑色
    virtual HWND create(HINSTANCE hInst, const RECT& rcWindow, HWND hWnd, DWORD crShadow = 0)
    {
        return shadow.create(hInst, rcWindow, hWnd, crShadow);
    }

    // 设置阴影颜色, 颜色值是ARGB
    virtual void SetColor(DWORD crShadow, DWORD argbBorder, bool isRedraw = false)
    {
        return shadow.SetColor(crShadow, argbBorder, isRedraw);
    }

    // 更新阴影窗口位置, 根据 rcWindow 参数调整阴影窗口的大小和位置, 阴影位置是 rcWindow.左边顶边-10, 右边底边+10
    // bForce = 强制刷新, 为假时窗口尺寸未改变则不会重画, 为true时则必须重画
    virtual void Update(const RECT& rcWindow, bool bForce = false)
    {
        return shadow.Update(rcWindow, bForce);
    }

    // 挂接绘画回调函数, 0为取消回调
    virtual void OnPaint(pfn_ShadowPaint pfn, LPVOID param = 0)
    {
        return shadow.OnPaint(pfn, param);
    }
    // 销毁当前类的阴影窗口
    virtual void destroy()
    {
        return shadow.destroy();
    }
    virtual bool isCreate()
    {
        return shadow.isCreate();
    }
    virtual void SetParent(HWND hWnd)
    {
        return shadow.SetParent(hWnd);
    }
    virtual void hide()
    {
        return shadow.hide();
    }
    virtual void show()
    {
        return shadow.show();
    }
    virtual void SetAlpha(int alpha)
    {
        return shadow.SetAlpha(alpha);
    }
    virtual int GetAlpha()
    {
        return shadow.GetAlpha();
    }
    virtual bool move(const RECT& rc)
    {
        return shadow.move(rc);
    }
    virtual HWND GetWindow()
    {
        return shadow;
    }

};

void _draw_ready_title(LPCANVAS hCanvas, ARGB crText, const RECT_F& rc, ARGB crClear)
{
    static LPEX_FONT hFontReady = s_info->pfnCreateFontDx(IDRAW_FONT, -40, FONTSTYLE::FontStyleRegular);
    LPEX_FONT hFont = g_scale.GetFont(hFontReady);
    if (crClear)
        hCanvas->FillRectangle(s_info->pfnCreateBrushDx(crClear), &rc);
    
    LPEX_BRUSH hbrText = s_info->pfnCreateBrushDx(crText);
    const LPCWSTR text = 
        L".................................................................................."
        L"视觉库准备中, 请稍后"
        L"..................................................................................";
    static int n = 0;
    if (n > 40)n = 0;
    const int fmt = DT_END_ELLIPSIS | DT_CENTER;
    RECT_F rcText(rc);
    float width = 0, height = 0;
    n++;
    if (n > 80) n = 0;
    LPCWSTR str = text + 82 ;
    size_t strlen = 10 + n;
#if 0  // 左右都使用点就改成1
    str -= n;
    strlen += n;
#endif

    hCanvas->calctextsize(hFont, str, strlen, fmt, rc.width(), rc.height(), &width, &height);
    rcText.top = rc.bottom - height - 30;
    hCanvas->drawtext(hFont, hbrText, str, strlen, fmt, &rcText);
}

bool WINAPI OnShadowPaint(LPCANVAS hCanvas, LPEX_IMAGE pImg, const RECT* prc, DWORD crShadow, DWORD argbBorder, int alpha, LPVOID param)
{
    int cxWin = prc->right - prc->left;
    int cyWin = prc->bottom - prc->top;
    RECT_F rc;
    rc.left = 10;
    rc.top = 10;
    rc.right = (float)cxWin - 10;
    rc.bottom = (float)cyWin - 10;
    const int cxClient = (int)(rc.right - rc.left);   // 不包括阴影在客户区宽度, 边框和背景插件都是在这个范围内绘画
    const int cyClient = (int)(rc.bottom - rc.top);   // 不包括阴影在客户区宽度, 边框和背景插件都是在这个范围内绘画
    if (alpha > 0)
    {
        _canvas_drawimagegridPadding(hCanvas, pImg,
            0, 0, (float)cxWin, (float)cyWin,   // 画到哪个位置上
            0, 0, 40, 40,                       // 从图片的哪个位置
            16, 16, 20, 20,                     // 九宫区域
            0, 255);

        LPEX_PEN hPen = _pen_create(argbBorder);


        int oldMode = 0;
        _canvas_SetAntialiasMode(hCanvas, 0, &oldMode);     // 画线不需要抗锯齿
        if (!s_info->isReady && s_info->pWnd)
        {
            COLORREF crBack = s_info->themeColor->crBackExtern();
            _draw_ready_title(hCanvas, RGB2ARGB(s_info->pWnd->draw.color.crText, 244), rc, RGB2ARGB(crBack, 255));
            //bool sas = IsWindowVisible(s_info->shadow->GetWindow());
            //RECT sdllkl;
            //GetWindowRect(s_info->shadow->GetWindow(), &sdllkl);
            //hCanvas->FillRectangle(s_info->pfnCreateBrushDx(MAKEABGR(255, 255, 0, 0)), &rc);
        }
        _canvas_drawRectangle(hCanvas, hPen, &rc);
        _canvas_SetAntialiasMode(hCanvas, oldMode, 0);
        _pen_destroy(hPen);
    }

    // 窗口显示才需要绘画
    if (IsWindowVisible(s_info->hEWnd))
    {
        IRE_STRUCT_BACKGROUND_DRAW evt = { 0 };
        evt.hCanvas = hCanvas;
        evt.cxClient = cxClient - 2;
        evt.cyClient = cyClient - 2;

        static int isDxRender = s_info->isDxRender ? 1 : 0;
        const float offsetBorder = 10.f + (isDxRender ? 0.f : 1.f);
        const float offsettranslate = offsetBorder + isDxRender;
        const float reoffsetBorder = (isDxRender ? 0.f : -offsettranslate);
        const float cxClip = (float)cxWin - offsetBorder + (isDxRender == 0 ? 1.f : 0.f);
        const float cyClip = (float)cyWin - offsetBorder + (isDxRender == 0 ? 1.f : 0.f);

        _canvas_setClip(hCanvas, offsetBorder, offsetBorder, cxClip, cyClip, 0);
        _canvas_translate(hCanvas, offsettranslate, offsettranslate);

        if (0)  // 调试位置改为1
        {
            LPEX_BRUSH hbr = _brush_create(MAKEARGB(200, 255, 0, 0));
            LPEX_PEN hpen = _pen_create(MAKEARGB(200, 255, 0, 0));
            RECT_F rcfill(0, 0, (float)evt.cxClient, (float)evt.cyClient);
            //_canvas_fillRectangle(hCanvas, hbr, &rcfill);
            _canvas_drawRectangle(hCanvas, hpen, &rcfill);
            _brush_destroy(hbr);
            _pen_destroy(hpen);
            if (!s_info->isReady)
            {

            }
            MDIClientCode_DrawLine(&evt);
            _canvas_translate(hCanvas, reoffsetBorder, reoffsetBorder);
            _canvas_resetClip(hCanvas);
            return true;
        }
        LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
        int evtCount = s_info->evt.pfn_Event_callback_before(arr, IRE_BCAKGROUND_DRAW, &evt, &isReturn, true);
        // 需要继续触发调用后事件
        s_info->evt.pfn_Event_callback_after(arr, IRE_BCAKGROUND_DRAW, &evt, evtCount, true);



        MDIClientCode_DrawLine(&evt);
        _canvas_translate(hCanvas, reoffsetBorder, reoffsetBorder);
        _canvas_resetClip(hCanvas);
    }

    return true;
}


ICShadow* EWindow_GetShadow()
{
    static _CShadow shadow;
    return &shadow;
}