#pragma once
#include <unordered_map>
#include <dwmapi.h>
#pragma comment(lib,"Dwmapi.lib")

#include <cpplang.h>

#if defined(__CPP17) && (_WIN32_WINNT >= 0x602)
#include <winrt/Windows.UI.ViewManagement.h>
#endif

#define WM_THEMECOLORCHANGED    (WM_APP + 1)    // 主题配色被改变, wParam = 是否是深色模式, lParam = THEMECOLOR_STRUCT 结构指针, 需要考虑跨进程的问题
#define WM_REDRAWCOLORCHANGED   (WM_APP + 2)    // 标题栏跟随主题颜色, wParam = 是否设置改变, lParam未使用


//// 主题颜色类, 内部内置了几套基础的配色, 用法和 GetSysColor 一样, 参数也一样
//class CThemeColor
//{
//
//
//    DWORD m_color;  // 当前颜色, 根据这个颜色判断是否是深色默认, 然后返回对应模式的配色
//public:
//    CThemeColor(DWORD color = 0) : m_color(color)
//    {
//
//    }
//
//    // 设置颜色, 返回设置之前的值, 内部会根据这个颜色计算是否是深色默认, 然后按照对应模式返回颜色值
//    inline DWORD SetColor(DWORD color)
//    {
//        DWORD ret = m_color;
//        m_color = color;
//        return ret;
//    }
//    // 返回是否是深色模式, 根据内部的颜色判断
//    inline static bool IsDarkMode(DWORD color)
//    {
//        return IsArgbLight(color);
//    }
//    // 返回是否是深色模式, 根据内部的颜色判断
//    inline bool IsDarkMode()
//    {
//        return IsArgbLight(m_color);
//    }
//    // 设置是否为深色模式, 根据内部的颜色判断
//    inline bool SetDarkMode(bool isDarkMode)
//    {
//        bool ret = IsArgbLight(m_color);
//        m_color = isDarkMode
//            ? ((COLORREF)((((BYTE)(255) | ((WORD)((BYTE)(255)) << 8)) | (((DWORD)(BYTE)(255)) << 16)) | (((DWORD)(BYTE)(255)) << 24)))
//            : ((COLORREF)((((BYTE)(0) | ((WORD)((BYTE)(0)) << 8)) | (((DWORD)(BYTE)(0)) << 16)) | (((DWORD)(BYTE)(255)) << 24)));
//        return ret;
//    }
//
//    // 取前景色, 返回ARGB颜色
//    inline static DWORD GetForegroud()
//    {
//#ifdef __CPP17
//
//        using namespace winrt::Windows::UI::ViewManagement;
//        auto settings = UISettings();
//        auto foreground = settings.GetColorValue(UIColorType::Foreground);
//        DWORD argb = ((COLORREF)(
//              (((BYTE)(foreground.B)
//            | ((WORD)((BYTE)(foreground.G)) << 8))
//            | (((DWORD)(BYTE)(foreground.R)) << 16))
//            | (((DWORD)(BYTE)(foreground.A)) << 24)));
//        return argb;
//#else
//        HKEY hKey;
//        const LPCWSTR subKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
//        const LPCWSTR valueName = L"AppsUseLightTheme";
//        DWORD value = -1;
//        DWORD valueSize = sizeof(DWORD);
//        if (RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
//        {
//            HRESULT hr = RegGetValueW(hKey, nullptr, valueName, RRF_RT_REG_DWORD, nullptr, &value, &valueSize);
//            if (hr != S_OK)
//            {
//                value = 0;    // 不要假定该键必须存在，如果找不到将返回默认值
//            }
//            RegCloseKey(hKey);
//        }
//
//        // 是黑色主题就返回亮色, 否则返回黑色
//        DWORD color = value == 0
//            ? ((COLORREF)((((BYTE)(255) | ((WORD)((BYTE)(255)) << 8)) | (((DWORD)(BYTE)(255)) << 16)) | (((DWORD)(BYTE)(255)) << 24)))
//            : ((COLORREF)((((BYTE)(0) | ((WORD)((BYTE)(0)) << 8)) | (((DWORD)(BYTE)(0)) << 16)) | (((DWORD)(BYTE)(255)) << 24)));
//
//        return color;
//#endif
//
//    }
//    // 启动/停用深色模式, 调用 DwmSetWindowAttribute 实现
//    // 调用后会设置窗口标题栏的颜色模式
//    inline static bool SwitchDarkMode(HWND hWnd, bool isDarkMode)
//    {
//        BOOL value = isDarkMode;
//        ::DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
//        return isDarkMode;
//    }
//
//
//public:
//
//    // 返回当前主题的颜色, 返回的是ARGB颜色, 失败返回0, ARGB里0就是完全透明, 主题颜色应该是没有透明的
//    // pfOpaqueBlend = 指示颜色是否为不透明混合
//    inline static DWORD GetThemeColor(BOOL* pfOpaqueBlend = 0)
//    {
//        DWORD color = 0;
//        BOOL opaque = FALSE;
//        if (pfOpaqueBlend)*pfOpaqueBlend = 0;
//        HRESULT hr = DwmGetColorizationColor(&color, &opaque);
//        if (FAILED(hr))
//            return 0;
//        if (pfOpaqueBlend)*pfOpaqueBlend = opaque;
//        return color;
//    }
//
//    //// 返回对应的画刷, 返回的画刷不能释放
//    //inline HBRUSH GetSysBrush(int nIndex)
//    //{
//    //    return GetSysBrush(nIndex, IsDarkMode());
//    //}
//    //// 返回对应的画刷, 返回的画刷不能释放
//    //inline static HBRUSH GetSysBrush(int nIndex, bool isDarkMode)
//    //{
//    //    auto& map = GetMap();
//    //    COLORREF clr = GetSysColor(nIndex, isDarkMode);
//    //    HBRUSH& hbr = map[clr];
//    //    if (!hbr)
//    //        hbr = CreateSolidBrush(clr);
//    //    return hbr;
//    //}
//    //inline COLORREF GetSysColor(int nIndex)
//    //{
//    //    // 是否是深色模式
//    //    const bool isDarkMode = IsDarkMode();
//    //    return GetSysColor(nIndex, isDarkMode);
//    //}
//    // 用法和 GetSysColor 一样, 参数也一样
//    inline static COLORREF GetSysColor(int nIndex, bool isDarkMode)
//    {
//        switch (nIndex)
//        {
//        case COLOR_SCROLLBAR:           // 滚动条灰色区域
//            break;
//
//        case COLOR_BACKGROUND:          // 背景颜色
//        //case COLOR_DESKTOP:
//        {
//            return isDarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
//        }
//        case COLOR_ACTIVECAPTION:       // 活动窗口标题栏。关联的前景色为 COLOR_CAPTIONTEXT
//        {
//            return isDarkMode ? RGB(93, 90, 88) : RGB(255, 255, 255);
//        }
//        case COLOR_INACTIVECAPTION:     // 非活动窗口标题。关联的前景色为 COLOR_INACTIVECAPTIONTEXT
//        {
//            return isDarkMode ? RGB(43, 43, 43) : RGB(255, 255, 255);
//        }
//        case COLOR_MENU:                // 菜单背景。关联的前景色为 COLOR_MENUTEXT
//        {
//            return isDarkMode ? RGB(43, 43, 43) : RGB(238, 238, 238);
//        }
//        case COLOR_WINDOW:              // 窗口背景。关联的前景色是COLOR_WINDOWTEXT和COLOR_HOTLITE
//        {
//            return isDarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
//        }
//        case COLOR_WINDOWFRAME:         // 窗口边框颜色
//        {
//            return isDarkMode ? RGB(100, 100, 100) : RGB(100, 100, 100);
//        }
//        case COLOR_MENUTEXT:            // 菜单文本颜色
//        {
//            return isDarkMode ? RGB(248, 248, 248) : RGB(0, 0, 0);
//        }
//        case COLOR_WINDOWTEXT:          // 窗口中的文本。关联的背景颜色为 COLOR_WINDOW
//        {
//            return isDarkMode ? RGB(248, 248, 248) : RGB(0, 0, 0);
//        }
//        case COLOR_CAPTIONTEXT:         // 标题、大小框和滚动条箭头框中的文本。关联的背景颜色为 COLOR_ACTIVECAPTION
//        {
//            return isDarkMode ? RGB(103, 103, 103) : RGB(0, 0, 0);
//        }
//        case COLOR_ACTIVEBORDER:        // 活动窗口边框
//        {
//            return isDarkMode ? RGB(107, 104, 102) : RGB(180, 180, 180);
//        }
//        case COLOR_INACTIVEBORDER:      // 非活动窗口边框
//        {
//            return isDarkMode ? RGB(86, 84, 83) : RGB(244, 248, 252);
//        }
//        case COLOR_APPWORKSPACE:        // MDI应用程序的背景色
//        {
//            break;
//        }
//        case COLOR_HIGHLIGHT:           // 在控件中选择的项。关联的前景色为COLOR_HIGHLIGHTTEXT色
//        {
//            return isDarkMode ? RGB(119, 119, 119) : RGB(204, 232, 255);
//        }
//        case COLOR_HIGHLIGHTTEXT:       // 控件中所选项的文本。关联的背景颜色为 COLOR_HIGHLIGHT
//        {
//            return isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
//        }
//        case COLOR_BTNFACE:             // 三维显示元素和对话框背景的面色
//        //case COLOR_3DFACE:            // 三维显示元素和对话框背景的面色
//        {
//            return isDarkMode ? RGB(61, 61, 61) : RGB(225, 225, 225);
//        }
//        case COLOR_BTNSHADOW:           // 按钮边框颜色
//        //case COLOR_3DSHADOW:
//        {
//            return isDarkMode ? RGB(112, 112, 112) : RGB(173, 173, 173);
//        }
//        case COLOR_GRAYTEXT:            // 灰色禁用文本
//        {
//            return isDarkMode ? RGB(109, 109, 109) : RGB(123, 123, 123);
//        }
//        case COLOR_BTNTEXT:             // 按钮上的文字。关联的背景色为 COLOR_BTNFACE
//        {
//            return isDarkMode ? RGB(214, 214, 214) : RGB(0, 0, 0);
//        }
//        case COLOR_INACTIVECAPTIONTEXT: // 非活动标题中的文本颜色。关联的背景色为 COLOR_INACTIVECAPTION
//        {
//            return isDarkMode ? RGB(170, 170, 170) : RGB(165, 165, 165);
//        }
//        case COLOR_BTNHIGHLIGHT:        // 三维显示元素的突出显示颜色
//        //case COLOR_BTNHILIGHT:
//        //case COLOR_3DHILIGHT:
//        //case COLOR_3DHIGHLIGHT:
//        {
//            break;
//        }
//        case COLOR_3DDKSHADOW:          // 三维显示元素的深色阴影
//        {
//            COLORREF clr1 = ::GetSysColor(COLOR_3DDKSHADOW);
//            COLORREF clr2 = ::GetSysColor(COLOR_3DLIGHT);
//            return isDarkMode ? clr2 : clr1;
//        }
//        case COLOR_3DLIGHT:             // 三维显示元素的浅色
//        {
//            COLORREF clr1 = ::GetSysColor(COLOR_3DDKSHADOW);
//            COLORREF clr2 = ::GetSysColor(COLOR_3DLIGHT);
//            return isDarkMode ? clr1 : clr2;
//        }
//        case COLOR_INFOTEXT:            // 工具提示控件的文本颜色
//        {
//            return isDarkMode ? RGB(255, 255, 255) : RGB(87, 87, 87);
//        }
//        case COLOR_INFOBK:              // 工具提示控件的背景色
//        {
//            return isDarkMode ? RGB(43, 43, 43) : RGB(255, 255, 255);
//        }
//        case COLOR_HOTLIGHT:            // 超链接或热跟踪项的颜色
//        {
//            return isDarkMode ? RGB(77, 77, 77) : RGB(0, 102, 204);
//        }
//        case COLOR_GRADIENTACTIVECAPTION:   // 活动窗口标题栏的颜色渐变中的右侧颜色。COLOR_ACTIVECAPTION指定左侧颜色
//        {
//            break;
//        }
//        case COLOR_GRADIENTINACTIVECAPTION: // 非活动窗口标题栏的颜色渐变中的右侧颜色。COLOR_INACTIVECAPTION指定左侧颜色
//        {
//            break;
//        }
//        case COLOR_MENUHILIGHT:             // 当菜单显示为平面菜单时，用于突出显示菜单项的颜色
//        {
//            break;
//        }
//        case COLOR_MENUBAR:                 // 当菜单显示为平面菜单时菜单栏的背景色
//        {
//            break;
//        }
//        default:
//            break;
//        }
//        return ::GetSysColor(nIndex);
//    }
//
//public:
//
//    // 计算ARGB颜色是否是亮色, 如果返回true, 程序应该把配色改成深色
//    inline static bool IsArgbLight(BYTE a, BYTE r, BYTE g, BYTE b)
//    {
//        return IsRgbLight(r, g, b);
//    }
//    // 计算ARGB颜色是否是亮色, 如果返回true, 程序应该把配色改成深色
//    inline static bool IsArgbLight(DWORD argb)
//    {
//        BYTE a = ((BYTE)((argb & 0xff000000) >> 24));
//        BYTE r = ((BYTE)((argb & 0x00ff0000) >> 16));
//        BYTE g = ((BYTE)((argb & 0x0000ff00) >> 8));
//        BYTE b = ((BYTE)((argb & 0x000000ff)));
//        return IsRgbLight(r, g, b);
//    }
//    // 计算RGB颜色是否是亮色, 如果返回true, 程序应该把配色改成深色
//    inline static bool IsRgbLight(COLORREF rgb)
//    {
//        BYTE r = ((BYTE)((rgb & 0x000000ff)));
//        BYTE g = ((BYTE)((rgb & 0x0000ff00) >> 8));
//        BYTE b = ((BYTE)((rgb & 0x00ff0000) >> 16));
//        return IsRgbLight(r, g, b);
//    }
//    // 计算RGB颜色是否是亮色, 如果返回true, 程序应该把配色改成深色
//    inline static bool IsRgbLight(BYTE r, BYTE g, BYTE b)
//    {
//        return (((5 * g) + (2 * r) + b) > (8 * 128));
//    }
//
//};
//



typedef struct THEMECOLOR_STRUCT
{
    COLORREF crBack                 ;   // 背景颜色
    COLORREF crBackExtern           ;   // 外部的背景颜色, 比如选择夹的子夹
    COLORREF crStateBar             ;   // 组件箱/工作夹/状态夹顶部激活的颜色

    COLORREF crShadow               ;   // 阴影颜色
    COLORREF crShadowRun            ;   // 运行时阴影颜色
    COLORREF crShadowNoFocus        ;   // 取消激活时的边框颜色

    COLORREF crTipsBack             ;   // 提示窗口背景颜色
    COLORREF crTipsText             ;   // 提示窗口文本颜色
    COLORREF crTipsBorder           ;   // 提示窗口边框颜色

    COLORREF crMenuBorder           ;   // 菜单边框颜色
    COLORREF crMenuBack             ;   // 菜单背景颜色, 弹出菜单使用的背景色, 菜单条按下时使用的背景色
    COLORREF crMenuHot              ;   // 菜单热点项颜色
    COLORREF crMenuBan              ;   // 菜单禁止颜色
    COLORREF crMenuText             ;   // 菜单文本颜色
    COLORREF crMenuTextHot          ;   // 菜单文本热点颜色
    COLORREF crMenuTextBan          ;   // 菜单文本禁止颜色

    COLORREF crText                 ;   // 普通文本绘画用的颜色
    COLORREF crTextHot              ;   // 热点文本绘画用的颜色
    COLORREF crTextDown             ;   // 按下文本绘画用的颜色
    COLORREF crTextBan              ;   // 禁止文本绘画用的颜色

    COLORREF crNormal               ;   // 组件普通颜色
    COLORREF crHover                ;   // 组件进入颜色
    COLORREF crFocus                ;   // 组件焦点颜色
    COLORREF crChecked              ;   // 组件选中颜色
    COLORREF crDown                 ;   // 组件按下颜色
    COLORREF crSelected             ;   // 组件选择颜色
    COLORREF crHot                  ;   // 组件热点颜色
    COLORREF crBan                  ;   // 组件禁止颜色
    COLORREF crBorder               ;   // 组件边框颜色
    COLORREF crBorderHot            ;   // 组件边框热点颜色

    COLORREF crTextStatusBar        ;   // 底部状态条的文本颜色
    COLORREF crTextStatusBarRun     ;   // 底部状态条运行时的文本颜色
    COLORREF crTextStatusBarNf      ;   // 底部状态条不激活的文本颜色

    COLORREF crScrollBtn1Normal     ;   // 滚动条按钮1正常颜色
    COLORREF crScrollBtn1Hot        ;   // 滚动条按钮1热点颜色
    COLORREF crScrollBtn1Down       ;   // 滚动条按钮1按下颜色
    COLORREF crScrollBtn2Normal     ;   // 滚动条按钮2正常颜色
    COLORREF crScrollBtn2Hot        ;   // 滚动条按钮2热点颜色
    COLORREF crScrollBtn2Down       ;   // 滚动条按钮2按下颜色
    COLORREF crScrollThumbNormal    ;   // 滚动条滑块颜色
    COLORREF crScrollThumbHot       ;   // 滚动条滑块热点颜色
    COLORREF crScrollThumbDown      ;   // 滚动条滑块按下颜色
    COLORREF crScrollBack           ;   // 滚动条背景颜色

    COLORREF crToolSeplineLeft      ;   // 工具条分割线左边颜色
    COLORREF crToolSeplineRight     ;   // 工具条分割线右边颜色

    COLORREF crStatelineNormal      ;   // 窗口顶部中间线的正常颜色
    COLORREF crStatelineHot         ;   // 窗口顶部中间线的热点颜色

    BOOL     isDarkMode             ;   // 当前结构的配色是否是深色模式的配色

}*PTHEMECOLOR_STRUCT;

typedef const THEMECOLOR_STRUCT* PCTHEMECOLOR_STRUCT;

struct THEMECOLOR_PEN_STRUCT
{
    int iStyle;
    int cWidth;
    COLORREF clr;
    bool operator<(const THEMECOLOR_PEN_STRUCT& p) const
    {
        return clr < p.clr;
    }
    bool operator==(const THEMECOLOR_PEN_STRUCT& p) const
    {
        return clr == p.clr;
    }
};
template<>
struct std::hash<THEMECOLOR_PEN_STRUCT>
{
    _NODISCARD size_t operator()(const THEMECOLOR_PEN_STRUCT& _Keyval) const noexcept
    {
        return std::_Fnv1a_append_value(std::_FNV_offset_basis, _Keyval);
    }
};

class CThemeColor
{
    template<typename T>
    struct THEMECOLOR_DELETEOBJECT
    {
        inline void operator()(const T& value)
        {
            if (value)
                DeleteObject(value);
        }
    };
    template<typename T, typename R, typename _FUN = THEMECOLOR_DELETEOBJECT<R>, typename _OBJ = std::unordered_map<T, R>>
    class UNMAP : public _OBJ
    {
    public:
        UNMAP() : _OBJ()
        {

        }
        ~UNMAP()
        {
            for (auto& item : *this)
                _FUN()(item.second);
        }
    };
    inline static UNMAP<COLORREF, HBRUSH>& GetMap()
    {
        static UNMAP<COLORREF, HBRUSH> s_map;
        return s_map;
    }

public:
    virtual void init(LPCWSTR iniFile, bool isDark) = 0;
    virtual PCTHEMECOLOR_STRUCT GetClr() = 0;

    virtual COLORREF crBack                 ( COLORREF* pValue = 0 ) = 0;   // 背景颜色
    virtual COLORREF crBackExtern           ( COLORREF* pValue = 0 ) = 0;   // 外部的背景颜色, 比如选择夹的子夹
    virtual COLORREF crStateBar             ( COLORREF* pValue = 0 ) = 0;   // 组件箱/工作夹/状态夹顶部激活的颜色

    virtual COLORREF crShadow               ( COLORREF* pValue = 0 ) = 0;   // 阴影颜色
    virtual COLORREF crShadowRun            ( COLORREF* pValue = 0 ) = 0;   // 运行时阴影颜色
    virtual COLORREF crShadowNoFocus        ( COLORREF* pValue = 0 ) = 0;   // 取消激活时的边框颜色

    virtual COLORREF crTipsBack             ( COLORREF* pValue = 0 ) = 0;   // 提示窗口背景颜色
    virtual COLORREF crTipsText             ( COLORREF* pValue = 0 ) = 0;   // 提示窗口文本颜色
    virtual COLORREF crTipsBorder           ( COLORREF* pValue = 0 ) = 0;   // 提示窗口边框颜色

    virtual COLORREF crMenuBorder           ( COLORREF* pValue = 0 ) = 0;   // 菜单边框颜色
    virtual COLORREF crMenuBack             ( COLORREF* pValue = 0 ) = 0;   // 菜单背景颜色, 弹出菜单使用的背景色, 菜单条按下时使用的背景色
    virtual COLORREF crMenuHot              ( COLORREF* pValue = 0 ) = 0;   // 菜单热点项颜色
    virtual COLORREF crMenuBan              ( COLORREF* pValue = 0 ) = 0;   // 菜单禁止颜色
    virtual COLORREF crMenuText             ( COLORREF* pValue = 0 ) = 0;   // 菜单文本颜色
    virtual COLORREF crMenuTextHot          ( COLORREF* pValue = 0 ) = 0;   // 菜单文本热点颜色
    virtual COLORREF crMenuTextBan          ( COLORREF* pValue = 0 ) = 0;   // 菜单文本禁止颜色

    virtual COLORREF crText                 ( COLORREF* pValue = 0 ) = 0;   // 普通文本绘画用的颜色
    virtual COLORREF crTextHot              ( COLORREF* pValue = 0 ) = 0;   // 热点文本绘画用的颜色
    virtual COLORREF crTextDown             ( COLORREF* pValue = 0 ) = 0;   // 按下文本绘画用的颜色
    virtual COLORREF crTextBan              ( COLORREF* pValue = 0 ) = 0;   // 禁止文本绘画用的颜色

    virtual COLORREF crNormal               ( COLORREF* pValue = 0 ) = 0;   // 组件普通颜色
    virtual COLORREF crHover                ( COLORREF* pValue = 0 ) = 0;   // 组件进入颜色
    virtual COLORREF crFocus                ( COLORREF* pValue = 0 ) = 0;   // 组件焦点颜色
    virtual COLORREF crChecked              ( COLORREF* pValue = 0 ) = 0;   // 组件选中颜色
    virtual COLORREF crDown                 ( COLORREF* pValue = 0 ) = 0;   // 组件按下颜色
    virtual COLORREF crSelected             ( COLORREF* pValue = 0 ) = 0;   // 组件选择颜色
    virtual COLORREF crHot                  ( COLORREF* pValue = 0 ) = 0;   // 组件热点颜色
    virtual COLORREF crBan                  ( COLORREF* pValue = 0 ) = 0;   // 组件禁止颜色
    virtual COLORREF crBorder               ( COLORREF* pValue = 0 ) = 0;   // 组件边框颜色
    virtual COLORREF crBorderHot            ( COLORREF* pValue = 0 ) = 0;   // 组件边框热点颜色

    virtual COLORREF crTextStatusBar        ( COLORREF* pValue = 0 ) = 0;   // 底部状态条的文本颜色
    virtual COLORREF crTextStatusBarRun     ( COLORREF* pValue = 0 ) = 0;   // 底部状态条运行时的文本颜色
    virtual COLORREF crTextStatusBarNf      ( COLORREF* pValue = 0 ) = 0;   // 底部状态条不激活的文本颜色

    virtual COLORREF crScrollBtn1Normal     ( COLORREF* pValue = 0 ) = 0;   // 滚动条按钮1正常颜色
    virtual COLORREF crScrollBtn1Hot        ( COLORREF* pValue = 0 ) = 0;   // 滚动条按钮1热点颜色
    virtual COLORREF crScrollBtn1Down       ( COLORREF* pValue = 0 ) = 0;   // 滚动条按钮1按下颜色
    virtual COLORREF crScrollBtn2Normal     ( COLORREF* pValue = 0 ) = 0;   // 滚动条按钮2正常颜色
    virtual COLORREF crScrollBtn2Hot        ( COLORREF* pValue = 0 ) = 0;   // 滚动条按钮2热点颜色
    virtual COLORREF crScrollBtn2Down       ( COLORREF* pValue = 0 ) = 0;   // 滚动条按钮2按下颜色
    virtual COLORREF crScrollThumbNormal    ( COLORREF* pValue = 0 ) = 0;   // 滚动条滑块颜色
    virtual COLORREF crScrollThumbHot       ( COLORREF* pValue = 0 ) = 0;   // 滚动条滑块热点颜色
    virtual COLORREF crScrollThumbDown      ( COLORREF* pValue = 0 ) = 0;   // 滚动条滑块按下颜色
    virtual COLORREF crScrollBack           ( COLORREF* pValue = 0 ) = 0;   // 滚动条背景颜色

    virtual COLORREF crToolSeplineLeft      ( COLORREF* pValue = 0 ) = 0;   // 工具条分割线左边颜色
    virtual COLORREF crToolSeplineRight     ( COLORREF* pValue = 0 ) = 0;   // 工具条分割线右边颜色

    virtual COLORREF crStatelineNormal      ( COLORREF* pValue = 0 ) = 0;   // 窗口顶部中间线的正常颜色
    virtual COLORREF crStatelineHot         ( COLORREF* pValue = 0 ) = 0;   // 窗口顶部中间线的热点颜色


public:

    // 根据进程id获取在内存中的共享内存
    // pid = 进程id, 如果为0, 则获取当前进程
    // isCreate = 不存在时是否创建
    inline static HANDLE GetColorMap(DWORD pid = 0, bool isCreate = false)
    {
        if (!pid)
            pid = GetCurrentProcessId();
        wchar_t mapName[100];
        swprintf_s(mapName, 100, L"{005E7DF6-%04d-4146-%X-AD15417DE176}", pid, pid);
        HANDLE hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, 0, mapName);

        if (!hMap && isCreate)
        {
            hMap = CreateFileMappingW(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, 0x1000, mapName);
        }
        return hMap;
    }
    inline static bool GetThemeColorFromMap(PTHEMECOLOR_STRUCT ret)
    {
        HANDLE hMap = GetColorMap();
        if (!hMap)
            return false;
        PTHEMECOLOR_STRUCT pClr = (PTHEMECOLOR_STRUCT)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
        if (pClr)
        {
            memcpy(ret, pClr, sizeof(THEMECOLOR_STRUCT));
            UnmapViewOfFile(pClr);
        }
        CloseHandle(hMap);
        return pClr != 0;
    }
    // 初始化当前进程的颜色结构
    inline static bool InitMapColor(bool isDark, PTHEMECOLOR_STRUCT ret = 0)
    {
        HANDLE hMap = GetColorMap(GetCurrentProcessId(), true);
        if (!hMap)
            return false;
        PTHEMECOLOR_STRUCT pClr = (PTHEMECOLOR_STRUCT)MapViewOfFile(hMap, FILE_MAP_WRITE, 0, 0, 0);
        if (!pClr)
            return false;

        pClr->isDarkMode = isDark;
        if (isDark)
        {
            pClr->crBack = RGB(45, 45, 48);
            pClr->crBackExtern = RGB(45, 45, 48);
            pClr->crStateBar = RGB(0, 122, 204);
            pClr->crShadow = RGB(0, 122, 204);
            pClr->crShadowRun = RGB(202, 81, 0);
            pClr->crShadowNoFocus = RGB(66, 66, 69);
            pClr->crTipsBack = RGB(90, 90, 96);
            pClr->crTipsText = RGB(244, 244, 244);
            pClr->crTipsBorder = RGB(107, 104, 102);
            pClr->crMenuBorder = RGB(63, 63, 70);
            pClr->crMenuBack = RGB(27, 27, 28);
            pClr->crMenuHot = RGB(51, 51, 52);
            pClr->crMenuBan = RGB(32, 32, 33);
            pClr->crMenuText = RGB(222, 222, 222);
            pClr->crMenuTextHot = RGB(222, 222, 222);
            pClr->crMenuTextBan = RGB(101, 101, 101);
            pClr->crText = RGB(222, 222, 222);
            pClr->crTextHot = RGB(222, 222, 222);
            pClr->crTextDown = RGB(222, 222, 222);
            pClr->crTextBan = RGB(78, 78, 80);
            pClr->crNormal = RGB(45, 45, 48);
            pClr->crHover = RGB(0, 122, 204);
            pClr->crFocus = RGB(0, 122, 204);
            pClr->crChecked = RGB(0, 122, 204);
            pClr->crDown = RGB(65, 65, 68);
            pClr->crSelected = RGB(119, 119, 119);
            pClr->crHot = RGB(77, 77, 77);
            pClr->crBan = RGB(45, 45, 48);
            pClr->crBorder = RGB(63, 63, 70);
            pClr->crBorderHot = RGB(107, 104, 102);
            pClr->crTextStatusBar = RGB(222, 222, 222);
            pClr->crTextStatusBarRun = RGB(222, 222, 222);
            pClr->crTextStatusBarNf = RGB(222, 222, 222);
            pClr->crScrollBtn1Normal = RGB(153, 153, 153);
            pClr->crScrollBtn1Hot = RGB(28, 151, 234);
            pClr->crScrollBtn1Down = RGB(0, 122, 204);
            pClr->crScrollBtn2Normal = RGB(153, 153, 153);
            pClr->crScrollBtn2Hot = RGB(28, 151, 234);
            pClr->crScrollBtn2Down = RGB(0, 122, 204);
            pClr->crScrollThumbNormal = RGB(104, 104, 104);
            pClr->crScrollThumbHot = RGB(158, 158, 158);
            pClr->crScrollThumbDown = RGB(239, 235, 239);
            pClr->crScrollBack = RGB(62, 62, 66);
            pClr->crToolSeplineLeft = RGB(34, 34, 34);
            pClr->crToolSeplineRight = RGB(70, 70, 74);
            pClr->crStatelineNormal = RGB(70, 70, 74);
            pClr->crStatelineHot = RGB(89, 168, 222);
        }
        else
        {
            pClr->crBack = RGB(245, 245, 245);
            pClr->crBackExtern = RGB(238, 238, 242);
            pClr->crStateBar = RGB(0, 122, 204);
            pClr->crShadow = RGB(0, 122, 204);
            pClr->crShadowRun = RGB(202, 81, 0);
            pClr->crShadowNoFocus = RGB(204, 206, 219);
            pClr->crTipsBack = RGB(255, 255, 255);
            pClr->crTipsText = RGB(0, 0, 0);
            pClr->crTipsBorder = RGB(118, 118, 118);
            pClr->crMenuBorder = RGB(204, 206, 219);
            pClr->crMenuBack = RGB(246, 246, 246);
            pClr->crMenuHot = RGB(201, 222, 245);
            pClr->crMenuBan = RGB(251, 251, 251);
            pClr->crMenuText = RGB(30, 30, 30);
            pClr->crMenuTextHot = RGB(0, 0, 0);
            pClr->crMenuTextBan = RGB(162, 164, 165);
            pClr->crText = RGB(30, 30, 30);
            pClr->crTextHot = RGB(10, 10, 10);
            pClr->crTextDown = RGB(10, 10, 10);
            pClr->crTextBan = RGB(162, 164, 165);
            pClr->crNormal = RGB(238, 238, 242);
            pClr->crHover = RGB(225, 225, 228);
            pClr->crFocus = RGB(0, 122, 204);
            pClr->crChecked = RGB(0, 122, 204);
            pClr->crDown = RGB(200, 200, 203);
            pClr->crSelected = RGB(210, 210, 213);
            pClr->crHot = RGB(225, 225, 228);
            pClr->crBan = RGB(45, 45, 48);
            pClr->crBorder = RGB(204, 206, 219);
            pClr->crBorderHot = RGB(164, 166, 189);
            pClr->crTextStatusBar = RGB(30, 30, 30);
            pClr->crTextStatusBarRun = RGB(30, 30, 30);
            pClr->crTextStatusBarNf = RGB(30, 30, 30);
            pClr->crScrollBtn1Normal = RGB(134, 137, 153);
            pClr->crScrollBtn1Hot = RGB(28, 151, 234);
            pClr->crScrollBtn1Down = RGB(0, 122, 204);
            pClr->crScrollBtn2Normal = RGB(134, 137, 153);
            pClr->crScrollBtn2Hot = RGB(28, 151, 234);
            pClr->crScrollBtn2Down = RGB(0, 122, 204);
            pClr->crScrollThumbNormal = RGB(194, 195, 201);
            pClr->crScrollThumbHot = RGB(104, 104, 104);
            pClr->crScrollThumbDown = RGB(91, 91, 91);
            pClr->crScrollBack = RGB(233, 233, 233);
            pClr->crToolSeplineLeft = RGB(204, 206, 219);
            pClr->crToolSeplineRight = RGB(255, 255, 255);
            pClr->crStatelineNormal = RGB(70, 70, 74);
            pClr->crStatelineHot = RGB(89, 168, 222);
        }

        if (ret)
            memcpy(ret, pClr, sizeof(THEMECOLOR_STRUCT));
        UnmapViewOfFile(pClr);
        return true;
    }
    // 重画这个窗口以及下面的所有子窗口
    // hWnd = 被重画的窗口, 会递归枚举这个窗口以下的子窗口
    // isSetWindowPos = 是否调用 SetWindowPos 改变非客户区
    static void UpdateAllWindow(HWND hWnd, bool isSetWindowPos)
    {
        if (!hWnd)
            return;

        HWND hChild = GetWindow(hWnd, GW_CHILD);
        while (hChild)
        {
            UpdateAllWindow(hChild, isSetWindowPos);
            hChild = GetWindow(hChild, GW_HWNDNEXT);
        }

        if (isSetWindowPos)
            SetWindowPos(hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
        InvalidateRect(hWnd, 0, 0);
    }

    static LRESULT OnCtlColor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, PTHEMECOLOR_STRUCT pClr)
    {
        if (!pClr)
            return (INT_PTR)GetStockObject(WHITE_BRUSH);

        HWND hChild = (HWND)lParam;
        HDC hdc = (HDC)wParam;
        const int id = GetDlgCtrlID(hChild);
        COLORREF clrBack = RGB(255, 255, 255);
        COLORREF clrText = RGB(0, 0, 0);

        HBRUSH hbrBack = GetBrush(clrBack);


        //if (message == WM_CTLCOLORSTATIC || message == WM_CTLCOLOREDIT)
        {
            if (!IsWindowEnabled(hChild))
            {
                hbrBack = CThemeColor::GetBrush(pClr->crBan);
                clrText = pClr->crTextBan;
                clrBack = pClr->crBan;
            }
            else
            {
                hbrBack = CThemeColor::GetBrush(pClr->crBack);
                clrText = pClr->crText;
                clrBack = pClr->crBack;
            }

            //SetBkMode(hdc, TRANSPARENT);
            SetBkColor(hdc, clrBack);
            SetTextColor(hdc, clrText);
        }
        return (INT_PTR)hbrBack;
    }


    // 返回是否是深色模式, 根据内部的颜色判断
    inline static bool IsDarkMode(DWORD color)
    {
        return IsArgbLight(color);
    }

    // 取前景色, 返回ARGB颜色
    inline static DWORD GetForegroud()
    {
#if defined(__CPP17) && (_WIN32_WINNT >= 0x602) && 0

        using namespace winrt::Windows::UI::ViewManagement;
        auto settings = UISettings();
        auto foreground = settings.GetColorValue(UIColorType::Foreground);
        DWORD argb = ((COLORREF)(
            (((BYTE)(foreground.B)
              | ((WORD)((BYTE)(foreground.G)) << 8))
             | (((DWORD)(BYTE)(foreground.R)) << 16))
            | (((DWORD)(BYTE)(foreground.A)) << 24)));
        return argb;
#else
        HKEY hKey;
        const LPCWSTR subKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
        const LPCWSTR valueName = L"AppsUseLightTheme";
        DWORD value = -1;
        DWORD valueSize = sizeof(DWORD);
        if (RegOpenKeyExW(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            HRESULT hr = RegGetValueW(hKey, nullptr, valueName, RRF_RT_REG_DWORD, nullptr, &value, &valueSize);
            if (hr != S_OK)
            {
                value = 0;    // 不要假定该键必须存在，如果找不到将返回默认值
            }
            RegCloseKey(hKey);
        }

        // 是黑色主题就返回亮色, 否则返回黑色
        DWORD color = value == 0
            ? ((COLORREF)((((BYTE)(255) | ((WORD)((BYTE)(255)) << 8)) | (((DWORD)(BYTE)(255)) << 16)) | (((DWORD)(BYTE)(255)) << 24)))
            : ((COLORREF)((((BYTE)(0) | ((WORD)((BYTE)(0)) << 8)) | (((DWORD)(BYTE)(0)) << 16)) | (((DWORD)(BYTE)(255)) << 24)));

        return color;
#endif
    }


    // 启动/停用深色模式, 调用 DwmSetWindowAttribute 实现
    // 调用后会设置窗口标题栏的颜色模式
    inline static bool SwitchDarkMode(HWND hWnd, bool isDarkMode)
    {
        BOOL value = isDarkMode;
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
        ::DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
        return isDarkMode;
    }

    // 返回当前主题的颜色, 返回的是ARGB颜色, 失败返回0, ARGB里0就是完全透明, 主题颜色应该是没有透明的
    // pfOpaqueBlend = 指示颜色是否为不透明混合
    inline static DWORD GetThemeColor(BOOL* pfOpaqueBlend = 0)
    {
        DWORD color = 0;
        BOOL opaque = FALSE;
        if (pfOpaqueBlend)*pfOpaqueBlend = 0;
        HRESULT hr = DwmGetColorizationColor(&color, &opaque);
        if (FAILED(hr))
            return 0;
        if (pfOpaqueBlend)*pfOpaqueBlend = opaque;
        return color;
    }


    // 用法和 GetSysColor 一样, 参数也一样
    inline static COLORREF GetSysColor(int nIndex, bool isDarkMode)
    {
        switch (nIndex)
        {
        case COLOR_SCROLLBAR:           // 滚动条灰色区域
            break;

        case COLOR_BACKGROUND:          // 背景颜色
        //case COLOR_DESKTOP:
        {
            return isDarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
        }
        case COLOR_ACTIVECAPTION:       // 活动窗口标题栏。关联的前景色为 COLOR_CAPTIONTEXT
        {
            return isDarkMode ? RGB(93, 90, 88) : RGB(255, 255, 255);
        }
        case COLOR_INACTIVECAPTION:     // 非活动窗口标题。关联的前景色为 COLOR_INACTIVECAPTIONTEXT
        {
            return isDarkMode ? RGB(43, 43, 43) : RGB(255, 255, 255);
        }
        case COLOR_MENU:                // 菜单背景。关联的前景色为 COLOR_MENUTEXT
        {
            return isDarkMode ? RGB(43, 43, 43) : RGB(238, 238, 238);
        }
        case COLOR_WINDOW:              // 窗口背景。关联的前景色是COLOR_WINDOWTEXT和COLOR_HOTLITE
        {
            return isDarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
        }
        case COLOR_WINDOWFRAME:         // 窗口边框颜色
        {
            return isDarkMode ? RGB(100, 100, 100) : RGB(100, 100, 100);
        }
        case COLOR_MENUTEXT:            // 菜单文本颜色
        {
            return isDarkMode ? RGB(248, 248, 248) : RGB(0, 0, 0);
        }
        case COLOR_WINDOWTEXT:          // 窗口中的文本。关联的背景颜色为 COLOR_WINDOW
        {
            return isDarkMode ? RGB(248, 248, 248) : RGB(0, 0, 0);
        }
        case COLOR_CAPTIONTEXT:         // 标题、大小框和滚动条箭头框中的文本。关联的背景颜色为 COLOR_ACTIVECAPTION
        {
            return isDarkMode ? RGB(103, 103, 103) : RGB(0, 0, 0);
        }
        case COLOR_ACTIVEBORDER:        // 活动窗口边框
        {
            return isDarkMode ? RGB(107, 104, 102) : RGB(180, 180, 180);
        }
        case COLOR_INACTIVEBORDER:      // 非活动窗口边框
        {
            return isDarkMode ? RGB(86, 84, 83) : RGB(244, 248, 252);
        }
        case COLOR_APPWORKSPACE:        // MDI应用程序的背景色
        {
            break;
        }
        case COLOR_HIGHLIGHT:           // 在控件中选择的项。关联的前景色为COLOR_HIGHLIGHTTEXT色
        {
            return isDarkMode ? RGB(119, 119, 119) : RGB(204, 232, 255);
        }
        case COLOR_HIGHLIGHTTEXT:       // 控件中所选项的文本。关联的背景颜色为 COLOR_HIGHLIGHT
        {
            return isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
        }
        case COLOR_BTNFACE:             // 三维显示元素和对话框背景的面色
        //case COLOR_3DFACE:            // 三维显示元素和对话框背景的面色
        {
            return isDarkMode ? RGB(61, 61, 61) : RGB(225, 225, 225);
        }
        case COLOR_BTNSHADOW:           // 按钮边框颜色
        //case COLOR_3DSHADOW:
        {
            return isDarkMode ? RGB(112, 112, 112) : RGB(173, 173, 173);
        }
        case COLOR_GRAYTEXT:            // 灰色禁用文本
        {
            return isDarkMode ? RGB(109, 109, 109) : RGB(123, 123, 123);
        }
        case COLOR_BTNTEXT:             // 按钮上的文字。关联的背景色为 COLOR_BTNFACE
        {
            return isDarkMode ? RGB(214, 214, 214) : RGB(0, 0, 0);
        }
        case COLOR_INACTIVECAPTIONTEXT: // 非活动标题中的文本颜色。关联的背景色为 COLOR_INACTIVECAPTION
        {
            return isDarkMode ? RGB(170, 170, 170) : RGB(165, 165, 165);
        }
        case COLOR_BTNHIGHLIGHT:        // 三维显示元素的突出显示颜色
        //case COLOR_BTNHILIGHT:
        //case COLOR_3DHILIGHT:
        //case COLOR_3DHIGHLIGHT:
        {
            break;
        }
        case COLOR_3DDKSHADOW:          // 三维显示元素的深色阴影
        {
            COLORREF clr1 = ::GetSysColor(COLOR_3DDKSHADOW);
            COLORREF clr2 = ::GetSysColor(COLOR_3DLIGHT);
            return isDarkMode ? clr2 : clr1;
        }
        case COLOR_3DLIGHT:             // 三维显示元素的浅色
        {
            COLORREF clr1 = ::GetSysColor(COLOR_3DDKSHADOW);
            COLORREF clr2 = ::GetSysColor(COLOR_3DLIGHT);
            return isDarkMode ? clr1 : clr2;
        }
        case COLOR_INFOTEXT:            // 工具提示控件的文本颜色
        {
            return isDarkMode ? RGB(255, 255, 255) : RGB(87, 87, 87);
        }
        case COLOR_INFOBK:              // 工具提示控件的背景色
        {
            return isDarkMode ? RGB(43, 43, 43) : RGB(255, 255, 255);
        }
        case COLOR_HOTLIGHT:            // 超链接或热跟踪项的颜色
        {
            return isDarkMode ? RGB(77, 77, 77) : RGB(0, 102, 204);
        }
        case COLOR_GRADIENTACTIVECAPTION:   // 活动窗口标题栏的颜色渐变中的右侧颜色。COLOR_ACTIVECAPTION指定左侧颜色
        {
            break;
        }
        case COLOR_GRADIENTINACTIVECAPTION: // 非活动窗口标题栏的颜色渐变中的右侧颜色。COLOR_INACTIVECAPTION指定左侧颜色
        {
            break;
        }
        case COLOR_MENUHILIGHT:             // 当菜单显示为平面菜单时，用于突出显示菜单项的颜色
        {
            break;
        }
        case COLOR_MENUBAR:                 // 当菜单显示为平面菜单时菜单栏的背景色
        {
            break;
        }
        default:
            break;
        }
        return ::GetSysColor(nIndex);
    }

public:

    // 计算ARGB颜色是否是亮色, 如果返回true, 程序应该把配色改成深色
    inline static bool IsArgbLight(BYTE a, BYTE r, BYTE g, BYTE b)
    {
        return IsRgbLight(r, g, b);
    }
    // 计算ARGB颜色是否是亮色, 如果返回true, 程序应该把配色改成深色
    inline static bool IsArgbLight(DWORD argb)
    {
        BYTE a = ((BYTE)((argb & 0xff000000) >> 24));
        BYTE r = ((BYTE)((argb & 0x00ff0000) >> 16));
        BYTE g = ((BYTE)((argb & 0x0000ff00) >> 8));
        BYTE b = ((BYTE)((argb & 0x000000ff)));
        return IsRgbLight(r, g, b);
    }
    // 计算RGB颜色是否是亮色, 如果返回true, 程序应该把配色改成深色
    inline static bool IsRgbLight(COLORREF rgb)
    {
        BYTE r = ((BYTE)((rgb & 0x000000ff)));
        BYTE g = ((BYTE)((rgb & 0x0000ff00) >> 8));
        BYTE b = ((BYTE)((rgb & 0x00ff0000) >> 16));
        return IsRgbLight(r, g, b);
    }
    // 计算RGB颜色是否是亮色, 如果返回true, 程序应该把配色改成深色
    inline static bool IsRgbLight(BYTE r, BYTE g, BYTE b)
    {
        return (((5 * g) + (2 * r) + b) > (8 * 128));
    }


    static HBRUSH GetBrush(COLORREF clr)
    {
        static UNMAP<COLORREF, HBRUSH> map;
        auto it = map.find(clr);
        if (it != map.end())
            return it->second;
        HBRUSH hbr = CreateSolidBrush(clr);
        map[clr] = hbr;
        return hbr;
    }


    static HPEN GetPen(int iStyle, int cWidth, COLORREF clr)
    {
        THEMECOLOR_PEN_STRUCT vl = { iStyle, cWidth, clr };
        static UNMAP<THEMECOLOR_PEN_STRUCT, HPEN> map;
        auto it = map.find(vl);
        if (it != map.end())
            return it->second;
        HPEN hPen = CreatePen(iStyle, cWidth, clr);
        map[vl] = hPen;
        return hPen;
    }



};

