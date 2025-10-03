/*
* 此文件提供配色相关的命令
* 
*/

#include "iTheme.h"




static std::map<COLORREF, HBRUSH>  m_hbrushs;           // 所有申请的画刷都存放到这里, 相同颜色的画刷只申请一次
static std::map<COLORREF, HPEN>    m_hPens;             // 所有申请的画笔都存放到这里, 相同颜色宽度样式的画笔只申请一次
static std::map<DWORD, LPEX_BRUSH> m_hbrushsDx;         // 所有申请的Dx画刷都存放到这里, 相同颜色的画刷只申请一次
static std::map<DWORD, LPEX_PEN>   m_hPensDx;           // 所有申请的Dx画笔都存放到这里, 相同颜色相同宽度的画笔只申请一次
static std::unordered_map<DWORD, LPEX_FONT> s_fontDx;   // 记录所有创建的dx字体, (名字各个字节相加 + 字体大小 + 字体样式) 作为哈希值判断是否重复
static std::unordered_map<DWORD, LOGFONTW> s_fontLf;    // 记录所有创建字体的结构, (名字各个字节相加 + 字体大小 + 字体样式) 作为哈希值判断是否重复

static std::map<LPCWSTR, COLORREF> m_map_color_def; // 默认配色信息

enum ENUM_COLORINFO
{
    CRDEF_UNKOWN,
    CRDEF_BACKGROUND,
    CRDEF_TEXT,
    CRDEF_TEXTHOT,
    CRDEF_TEXTDOWN,
    CRDEF_TEXTBAN,
    CRDEF_NORMAL,
    CRDEF_HOVER,
    CRDEF_DOWN,
    CRDEF_FOCUS,
    CRDEF_CHECKED,
    CRDEF_SELECTED,
    CRDEF_HOT,
    CRDEF_BAN,
    CRDEF_BORDER,
    CRDEF_VISTED,
    CRDEF_SHADOW,
    CRDEF_END = 0xff,       // 

};



inline LPCWSTR _enum2str(ENUM_COLORINFO type)
{
    switch (type)
    {
    case CRDEF_UNKOWN:      return L"";
    case CRDEF_BACKGROUND:  return L"crBackGround";
    case CRDEF_TEXT:        return L"crText";
    case CRDEF_TEXTHOT:     return L"crTextHot";
    case CRDEF_TEXTDOWN:    return L"crTextDown";
    case CRDEF_TEXTBAN:     return L"crTextBan";
    case CRDEF_NORMAL:      return L"crNormal";
    case CRDEF_HOVER:       return L"crHover";
    case CRDEF_DOWN:        return L"crDown";
    case CRDEF_FOCUS:       return L"crFocus";
    case CRDEF_CHECKED:     return L"crChecked";
    case CRDEF_SELECTED:    return L"crSelected";
    case CRDEF_HOT:         return L"crHot";
    case CRDEF_BAN:         return L"crBan";
    case CRDEF_BORDER:      return L"crBorder";
    case CRDEF_VISTED:      return L"crVisted";
    case CRDEF_SHADOW:      return L"crShadow";
    default: break;
    }
    return L"";
}

inline ENUM_COLORINFO _str2enum(LPCWSTR str)
{
#define _cmp_(_s) wcscmp(str, _s) == 0
    if (_cmp_(L"crBackGround"   ))  return CRDEF_BACKGROUND;
    else if (_cmp_(L"crText"    ))  return CRDEF_TEXT;
    else if (_cmp_(L"crTextHot" ))  return CRDEF_TEXTHOT;
    else if (_cmp_(L"crTextDown"))  return CRDEF_TEXTDOWN;
    else if (_cmp_(L"crTextBan" ))  return CRDEF_TEXTBAN;
    else if (_cmp_(L"crNormal"  ))  return CRDEF_NORMAL;
    else if (_cmp_(L"crHover"   ))  return CRDEF_HOVER;
    else if (_cmp_(L"crDown"    ))  return CRDEF_DOWN;
    else if (_cmp_(L"crFocus"   ))  return CRDEF_FOCUS;
    else if (_cmp_(L"crChecked" ))  return CRDEF_CHECKED;
    else if (_cmp_(L"crSelected"))  return CRDEF_SELECTED;
    else if (_cmp_(L"crHot"     ))  return CRDEF_HOT;
    else if (_cmp_(L"crBan"     ))  return CRDEF_BAN;
    else if (_cmp_(L"crBorder"  ))  return CRDEF_BORDER;
    else if (_cmp_(L"crVisted"  ))  return CRDEF_VISTED;
    else if (_cmp_(L"crShadow"  ))  return CRDEF_SHADOW;
    else return CRDEF_UNKOWN;

}

void DeleteDefaultObject()
{
    for (auto i = m_hbrushs.begin(); i != m_hbrushs.end(); ++i)
    {
        DeleteObject(i->second);
    }
    for (auto i = m_hPens.begin(); i != m_hPens.end(); ++i)
    {
        DeleteObject(i->second);
    }
    for (auto i = m_hbrushsDx.begin(); i != m_hbrushsDx.end(); ++i)
    {
        s_info->d2d->_brush_destroy(i->second);
    }
    for (auto i = m_hPensDx.begin(); i != m_hPensDx.end(); ++i)
    {
        s_info->d2d->_pen_destroy(i->second);
    }
    for (auto i = s_fontDx.begin(); i != s_fontDx.end(); ++i)
    {
        s_info->d2d->_font_destroy(i->second);
    }
    m_hbrushs.clear();
    m_hPens.clear();
    m_hbrushsDx.clear();
    m_hPensDx.clear();
    s_fontDx.clear();
    s_fontLf.clear();

}

HBRUSH Brush_Create(COLORREF cr)
{
    auto it = m_hbrushs.find(cr);
    if (it != m_hbrushs.end())
        return it->second;
    HBRUSH hbr = CreateSolidBrush(cr);
    m_hbrushs[cr] = hbr;
    return hbr;
}
HPEN Pen_Create(int iStyle, int cWidth, COLORREF color)
{
    DWORD hash = iStyle + cWidth + color;
    auto it = m_hPens.find(hash);
    if (it != m_hPens.end())
        return it->second;
    HPEN hPen = CreatePen(iStyle, cWidth, color);
    m_hPens[hash] = hPen;
    return hPen;
}
LPEX_BRUSH Brush_CreateDx(DWORD cr)
{
    if (!s_info->d2d) return 0;
    auto it = m_hbrushsDx.find(cr);
    if (it != m_hbrushsDx.end())
        return it->second;
    LPEX_BRUSH hbr = s_info->d2d->_brush_create(cr);
    m_hbrushsDx[cr] = hbr;
    return hbr;
}
LPEX_PEN Pen_CreateDx(DWORD argb, float width)
{
    if (width <= 0.f)width = 1.0f;
    if (!s_info->d2d) return 0;
    DWORD hash = argb;// +(int)width;
    auto it = m_hPensDx.find(hash);
    if (it != m_hPensDx.end())
        return it->second;
    LPEX_PEN hPen = s_info->d2d->_pen_create(argb, width);
    m_hPensDx[hash] = hPen;
    return hPen;
}

LPEX_FONT Font_CreateLfDx(const LPLOGFONTW logFont)
{
    if (!logFont) return 0;

    DWORD hash = 0;
    for (int i = 0; i < sizeof(logFont->lfFaceName) / sizeof(logFont->lfFaceName); i++)
        hash += logFont->lfFaceName[i];
    if (logFont->lfWeight == 0)
        logFont->lfWeight = FW_NORMAL;
    hash += logFont->lfHeight;
    hash += logFont->lfWeight;
    hash += logFont->lfItalic;
    hash += logFont->lfUnderline;
    hash += logFont->lfStrikeOut;

    auto it = s_fontDx.find(hash);
    if (it != s_fontDx.end())
        return it->second;

    s_fontLf[hash] = *logFont;
    LPEX_FONT hFont = s_info->d2d->_font_create_logfont(logFont, 0);
    s_fontDx[hash] = hFont;
    return hFont;
}

LPEX_FONT Font_CreateDpi(LPEX_FONT hFont, UINT dpi)
{
    if (!hFont) return 0;
    if (dpi == 0) dpi = 96;
    if (dpi == 96) return hFont;
    const UINT scale = dpi * 100 / 96;
    LOGFONTW lf;
    hFont->GetLogfont(&lf);
    const int lfHeight = (int)((float)lf.lfHeight * ((float)scale / 100.f));
    if (lf.lfHeight == lfHeight)
        return hFont;
    lf.lfHeight = lfHeight;
    return Font_CreateLfDx(&lf);
}

LPEX_FONT Font_CreateDx(LPCWSTR name, LONG lfHeight, FONTSTYLE fontStyle)
{
    LOGFONTW logFont = { 0 };
    SystemParametersInfoW(SPI_GETICONTITLELOGFONT, sizeof(LOGFONTW), &logFont, 0);
    // 判断 是否启用dpi缩放,logFont.lfHeight /= dpiY;
    if (name)
        memcpy(logFont.lfFaceName, name, (wcslen(name) + 1) * sizeof(wchar_t));
    if (lfHeight != 0)
        logFont.lfHeight = lfHeight;
    int style = (int)fontStyle;
    if (style)
    {
        logFont.lfWeight = (style & (int)FONTSTYLE::FontStyleBold) ? FW_BOLD : FW_NORMAL;
        logFont.lfItalic = (style & (int)FONTSTYLE::FontStyleItalic) ? 1 : 0;
        logFont.lfUnderline = (style & (int)FONTSTYLE::FontStyleUnderline) ? 1 : 0;
        logFont.lfStrikeOut = (style & (int)FONTSTYLE::FontStyleStrikeout) ? 1 : 0;
    }
    return Font_CreateLfDx(&logFont);
}

// 这个函数会释放所有画刷, 画笔, 字体, 然后重新创建
// 当程序在运行过程中，图形设备可能因为某些原因而失效
// 例如，设备显示分辨率被改变，或者用户移除显示适配器（显示器）等等
// 如果设备丢失，那么渲染目标也会变得无效，顺带着所有的设备相关的资源都会失效
// Direct2D 当接收到设备丢失的信号后，会在调用 EndDraw 函数后返回 D2DERR_RECREATE_TARGET 代码
// 如果你收到这个错误码，你必须重新创建渲染目标以及设备相关资源
// 重新加载dx资源, 会删除所有dx对象, 然后重新创建
// 一般是在 EndDraw返回false, 且最后错误=D2DERR_RECREATE_TARGET 时调用
void ReloadDxResource()
{
    for (auto it = m_hbrushsDx.begin(); it != m_hbrushsDx.end(); ++it)
    {
        LPEX_BRUSH hbr = it->second;
        ARGB argb = s_info->d2d->_brush_getcolor(hbr);
        s_info->d2d->_brush_destroy(hbr);
        it->second = s_info->d2d->_brush_create(argb);
    }

    for (auto it = m_hPensDx.begin(); it != m_hPensDx.end(); ++it)
    {
        LPEX_PEN hpen = it->second;
        ARGB argb = s_info->d2d->_pen_getcolor(hpen);
        s_info->d2d->_pen_destroy(hpen);
        it->second = s_info->d2d->_pen_create(argb);
    }

    for (auto it = s_fontDx.begin(); it != s_fontDx.end(); ++it)
    {
        LPEX_FONT hFont = it->second;
        LOGFONT& lf = s_fontLf[it->first];
        s_info->d2d->_font_destroy(it->second);
        it->second = s_info->d2d->_font_create_logfont(&lf, 0);
        if (hFont == s_info->defData->draw.hFont)
        {
            s_info->defData->draw.hFont = it->second;
        }
        else if (hFont == s_info->defData->draw.hFontBold)
        {
            s_info->defData->draw.hFontBold = it->second;
        }
    }

    return;

}

// 获取默认配色
inline COLORREF _cr_def(ENUM_COLORINFO type)
{
    LPCWSTR str = _enum2str(type);
    if (m_map_color_def.empty())
    {
        COLORREF ret = 0;
        LPCWSTR mapName = 0;
        wstr _ini_ret;
        COLORREF readColor = 0;
#define _SET_COLOR(_name, _defcolor) \
        mapName = _enum2str(ENUM_COLORINFO::_name);\
        _ini_ret = s_info->iniColor->read(L"DefaultColor", mapName, L###_defcolor);\
        readColor = str2rgb(_ini_ret.c_str(), _defcolor); \
        m_map_color_def[mapName] = readColor; \
        if(mapName == str) ret = readColor;\

        _SET_COLOR(CRDEF_BACKGROUND, RGB(37, 37, 38)    );
        _SET_COLOR(CRDEF_TEXT      , RGB(222, 222, 222) );
        _SET_COLOR(CRDEF_TEXTHOT   , RGB(222, 222, 222) );
        _SET_COLOR(CRDEF_TEXTDOWN  , RGB(222, 222, 222) );
        _SET_COLOR(CRDEF_TEXTBAN   , RGB(78,  78,  80)  );
        _SET_COLOR(CRDEF_NORMAL    , RGB(45, 45, 48)    );
        _SET_COLOR(CRDEF_HOVER     , RGB(0, 122, 204)   );
        _SET_COLOR(CRDEF_FOCUS     , RGB(0, 122, 204)   );
        _SET_COLOR(CRDEF_CHECKED   , RGB(0, 122, 204)   );
        _SET_COLOR(CRDEF_DOWN      , RGB(65, 65, 68)    );
        _SET_COLOR(CRDEF_SELECTED  , RGB(95, 95, 98)    );
        _SET_COLOR(CRDEF_HOT       , RGB(80, 80, 83)    );
        _SET_COLOR(CRDEF_BAN       , RGB(45, 45, 48)    );
        _SET_COLOR(CRDEF_VISTED    , RGB(0, 0, 0)       );
        _SET_COLOR(CRDEF_SHADOW    , RGB(0, 0, 0)       );
        _SET_COLOR(CRDEF_BORDER    , RGB(63, 63, 70)    );

        return ret;
    }
    return m_map_color_def[str];
    wstr value = s_info->iniColor->read(L"DefaultColor", str);
    return str2rgb(value.c_str(), m_map_color_def[str]);
}

// 重新从配置文件里加载默认配色
void ReloadDefColor()
{
    m_map_color_def.clear();
    COLORINFO& cr = s_info->defData->draw.color;
    cr.crBackGround = 0;
    GetDefData();

}

LPOBJSTRUCT GetDefData(LPOBJSTRUCT pData)
{
    COLORINFO& cr = s_info->defData->draw.color;
    if (cr.crBackGround == 0)
    {
        cr.crBackGround = _cr_def(ENUM_COLORINFO::CRDEF_BACKGROUND);
        cr.crText       = _cr_def(ENUM_COLORINFO::CRDEF_TEXT);
        cr.crTextHot    = _cr_def(ENUM_COLORINFO::CRDEF_TEXTHOT);
        cr.crTextDown   = _cr_def(ENUM_COLORINFO::CRDEF_TEXTDOWN);
        cr.crTextBan    = _cr_def(ENUM_COLORINFO::CRDEF_TEXTBAN);
        cr.crNormal     = _cr_def(ENUM_COLORINFO::CRDEF_NORMAL);

        cr.crHover      = _cr_def(ENUM_COLORINFO::CRDEF_HOVER);
        cr.crFocus      = _cr_def(ENUM_COLORINFO::CRDEF_FOCUS);
        cr.crChecked    = _cr_def(ENUM_COLORINFO::CRDEF_CHECKED);
        cr.crDown       = _cr_def(ENUM_COLORINFO::CRDEF_DOWN);
        cr.crSelected   = _cr_def(ENUM_COLORINFO::CRDEF_SELECTED);
        cr.crHot        = _cr_def(ENUM_COLORINFO::CRDEF_HOT);
        cr.crBan        = _cr_def(ENUM_COLORINFO::CRDEF_BAN);
        cr.crVisted     = _cr_def(ENUM_COLORINFO::CRDEF_VISTED);
        cr.crShadow     = _cr_def(ENUM_COLORINFO::CRDEF_SHADOW);
        cr.crBorder     = _cr_def(ENUM_COLORINFO::CRDEF_BORDER);

        cr.hbrBackGround = Brush_Create(cr.crBackGround);
        cr.hbrText      = Brush_Create(cr.crText);
        cr.hbrTextHot   = Brush_Create(cr.crTextHot);
        cr.hbrTextDown  = Brush_Create(cr.crTextDown);
        cr.hbrTextBan   = Brush_Create(cr.crTextBan);
        cr.hbrNormal    = Brush_Create(cr.crNormal);
        cr.hbrHover     = Brush_Create(cr.crHover);
        cr.hbrDown      = Brush_Create(cr.crDown);
        cr.hbrFocus     = Brush_Create(cr.crFocus);
        cr.hbrChecked   = Brush_Create(cr.crChecked);
        cr.hbrSelected  = Brush_Create(cr.crSelected);
        cr.hbrHot       = Brush_Create(cr.crHot);
        cr.hbrBan       = Brush_Create(cr.crBan);
        cr.hbrVisted    = Brush_Create(cr.crVisted);
        cr.hbrShadow    = Brush_Create(cr.crShadow);
        cr.hpenBorder   = Pen_Create(PS_INSIDEFRAME, 1, cr.crBorder);

        if (!s_info->defData->draw.hFont)
        {
            s_info->defData->draw.hFont = Font_CreateDx(IDRAW_FONT, -12);
            s_info->defData->draw.hFontBold = Font_CreateDx(IDRAW_FONT, -12, FONTSTYLE::FontStyleBold);
        }
        s_info->defData->draw.fmt = DT_VCENTER | DT_SINGLELINE;
    }

    if (pData)
    {
        memcpy(&pData->draw.color, &cr, sizeof(COLORINFO));
        pData->draw.hFont = s_info->defData->draw.hFont;
        pData->draw.hFontBold = s_info->defData->draw.hFontBold;
        if (pData->draw.indexSel == 0)
        {
            pData->draw.indexSel = -1;
            pData->draw.indexHot = -1;
            pData->draw.indexDown = -1;
        }
        pData->index = -1;
    }
    return s_info->defData;
}
