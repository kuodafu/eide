#pragma once
#include <windows.h>

// ������, �����Ϊ����ӦDPIŪ����
class iDrawScale
{
public:
    // ���ݴ��ݽ��������巵�ص�ǰ������Ҫ������
    inline LPEX_FONT GetFont(LPEX_FONT hFont)
    {
        return s_info->pfnCreateFontDpi(hFont, s_info->dpiX);
    }
    // ���ݴ��ݽ��������巵�ص�ǰ������Ҫ������
    inline HFONT GetGdiFont(HFONT hFont)
    {
        LOGFONTW lf = { 0 };
        GetObjectW(hFont, sizeof(lf), &lf);
        LPEX_FONT f = s_info->pfnCreateFontLfDx(&lf);

        f = s_info->pfnCreateFontDpi(f, s_info->dpiX);
        return f ? f->GetFont() : hFont;
    }
    // ���ݵ�ǰdpi�Ծ��ν�������
    inline void rerect(RECT& rc)
    {
        const float scale = (float)s_info->dpiX / 96.f;
        rc.left     = (int)((float)(rc.left  ) / scale);
        rc.top      = (int)((float)(rc.top   ) / scale);
        rc.right    = (int)((float)(rc.right ) / scale);
        rc.bottom   = (int)((float)(rc.bottom) / scale);
    }

    // ����ָ��dpi�Ծ��ν�������
    inline void rect(RECT& rc, UINT dpi)
    {
        // �Ȼ�ԭ��ԭ����dpi�ٵ����µ�dpi
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

