#include "../EWindow_Fne_Header.h"
#include "../EWindow.h"
#include <control/CShadow.h>
#include <iDraw_const.h>

class _CShadow : public ICShadow
{
    CShadow shadow;
public:
    // ������Ӱ����, ���� rcWindow ����������Ӱ���ڵĴ�С��λ��, ��Ӱλ���� rcWindow.��߶���-10, �ұߵױ�+10
    // argbColorShadow = ��ӰRGB��ɫ, Ĭ���Ǻ�ɫ
    virtual HWND create(HINSTANCE hInst, const RECT& rcWindow, HWND hWnd, DWORD crShadow = 0)
    {
        return shadow.create(hInst, rcWindow, hWnd, crShadow);
    }

    // ������Ӱ��ɫ, ��ɫֵ��ARGB
    virtual void SetColor(DWORD crShadow, DWORD argbBorder, bool isRedraw = false)
    {
        return shadow.SetColor(crShadow, argbBorder, isRedraw);
    }

    // ������Ӱ����λ��, ���� rcWindow ����������Ӱ���ڵĴ�С��λ��, ��Ӱλ���� rcWindow.��߶���-10, �ұߵױ�+10
    // bForce = ǿ��ˢ��, Ϊ��ʱ���ڳߴ�δ�ı��򲻻��ػ�, Ϊtrueʱ������ػ�
    virtual void Update(const RECT& rcWindow, bool bForce = false)
    {
        return shadow.Update(rcWindow, bForce);
    }

    // �ҽӻ滭�ص�����, 0Ϊȡ���ص�
    virtual void OnPaint(pfn_ShadowPaint pfn, LPVOID param = 0)
    {
        return shadow.OnPaint(pfn, param);
    }
    // ���ٵ�ǰ�����Ӱ����
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
        L"�Ӿ���׼����, ���Ժ�"
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
#if 0  // ���Ҷ�ʹ�õ�͸ĳ�1
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
    const int cxClient = (int)(rc.right - rc.left);   // ��������Ӱ�ڿͻ������, �߿�ͱ�����������������Χ�ڻ滭
    const int cyClient = (int)(rc.bottom - rc.top);   // ��������Ӱ�ڿͻ������, �߿�ͱ�����������������Χ�ڻ滭
    if (alpha > 0)
    {
        _canvas_drawimagegridPadding(hCanvas, pImg,
            0, 0, (float)cxWin, (float)cyWin,   // �����ĸ�λ����
            0, 0, 40, 40,                       // ��ͼƬ���ĸ�λ��
            16, 16, 20, 20,                     // �Ź�����
            0, 255);

        LPEX_PEN hPen = _pen_create(argbBorder);


        int oldMode = 0;
        _canvas_SetAntialiasMode(hCanvas, 0, &oldMode);     // ���߲���Ҫ�����
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

    // ������ʾ����Ҫ�滭
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

        if (0)  // ����λ�ø�Ϊ1
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
        // ��Ҫ�����������ú��¼�
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