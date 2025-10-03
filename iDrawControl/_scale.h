#pragma once
#include <windows.h>

// 缩放类, 这个是为了适应DPI弄的类
class iDrawScale
{
public:
    // 根据传递进来的字体返回当前缩放需要的字体
    inline LPEX_FONT GetFont(LPEX_FONT hFont)
    {
        return s_info->pfnCreateFontDpi(hFont, s_info->dpiX);
    }
    // 根据传递进来的字体返回当前缩放需要的字体
    inline HFONT GetGdiFont(HFONT hFont)
    {
        LOGFONTW lf = { 0 };
        GetObjectW(hFont, sizeof(lf), &lf);
        LPEX_FONT f = s_info->pfnCreateFontLfDx(&lf);

        f = s_info->pfnCreateFontDpi(f, s_info->dpiX);
        return f ? f->GetFont() : hFont;
    }
    // 根据当前dpi对矩形进行缩放
    inline void rerect(RECT& rc)
    {
        const float scale = (float)s_info->dpiX / 96.f;
        rc.left     = (int)((float)(rc.left  ) / scale);
        rc.top      = (int)((float)(rc.top   ) / scale);
        rc.right    = (int)((float)(rc.right ) / scale);
        rc.bottom   = (int)((float)(rc.bottom) / scale);
    }

    // 根据指定dpi对矩形进行缩放
    inline void rect(RECT& rc, UINT dpi)
    {
        // 先还原成原来的dpi再调整新的dpi
        const float scale = (float)dpi / 96.f;
        rerect(rc);
        rc.left     = (int)((float)(rc.left  ) * scale);
        rc.top      = (int)((float)(rc.top   ) * scale);
        rc.right    = (int)((float)(rc.right ) * scale);
        rc.bottom   = (int)((float)(rc.bottom) * scale);
    }

protected:
private:
};

extern iDrawScale g_scale;
LRESULT Scale_DPIChanged(UINT dpiX, UINT dpiY, RECT* rcWindow, LPOBJSTRUCT pData);

