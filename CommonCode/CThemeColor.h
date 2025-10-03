#pragma once
#include <unordered_map>
#include <dwmapi.h>
#pragma comment(lib,"Dwmapi.lib")

#include <cpplang.h>

#if defined(__CPP17) && (_WIN32_WINNT >= 0x602)
#include <winrt/Windows.UI.ViewManagement.h>
#endif

#define WM_THEMECOLORCHANGED    (WM_APP + 1)    // ������ɫ���ı�, wParam = �Ƿ�����ɫģʽ, lParam = THEMECOLOR_STRUCT �ṹָ��, ��Ҫ���ǿ���̵�����
#define WM_REDRAWCOLORCHANGED   (WM_APP + 2)    // ����������������ɫ, wParam = �Ƿ����øı�, lParamδʹ��


//// ������ɫ��, �ڲ������˼��׻�������ɫ, �÷��� GetSysColor һ��, ����Ҳһ��
//class CThemeColor
//{
//
//
//    DWORD m_color;  // ��ǰ��ɫ, ���������ɫ�ж��Ƿ�����ɫĬ��, Ȼ�󷵻ض�Ӧģʽ����ɫ
//public:
//    CThemeColor(DWORD color = 0) : m_color(color)
//    {
//
//    }
//
//    // ������ɫ, ��������֮ǰ��ֵ, �ڲ�����������ɫ�����Ƿ�����ɫĬ��, Ȼ���ն�Ӧģʽ������ɫֵ
//    inline DWORD SetColor(DWORD color)
//    {
//        DWORD ret = m_color;
//        m_color = color;
//        return ret;
//    }
//    // �����Ƿ�����ɫģʽ, �����ڲ�����ɫ�ж�
//    inline static bool IsDarkMode(DWORD color)
//    {
//        return IsArgbLight(color);
//    }
//    // �����Ƿ�����ɫģʽ, �����ڲ�����ɫ�ж�
//    inline bool IsDarkMode()
//    {
//        return IsArgbLight(m_color);
//    }
//    // �����Ƿ�Ϊ��ɫģʽ, �����ڲ�����ɫ�ж�
//    inline bool SetDarkMode(bool isDarkMode)
//    {
//        bool ret = IsArgbLight(m_color);
//        m_color = isDarkMode
//            ? ((COLORREF)((((BYTE)(255) | ((WORD)((BYTE)(255)) << 8)) | (((DWORD)(BYTE)(255)) << 16)) | (((DWORD)(BYTE)(255)) << 24)))
//            : ((COLORREF)((((BYTE)(0) | ((WORD)((BYTE)(0)) << 8)) | (((DWORD)(BYTE)(0)) << 16)) | (((DWORD)(BYTE)(255)) << 24)));
//        return ret;
//    }
//
//    // ȡǰ��ɫ, ����ARGB��ɫ
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
//                value = 0;    // ��Ҫ�ٶ��ü�������ڣ�����Ҳ���������Ĭ��ֵ
//            }
//            RegCloseKey(hKey);
//        }
//
//        // �Ǻ�ɫ����ͷ�����ɫ, ���򷵻غ�ɫ
//        DWORD color = value == 0
//            ? ((COLORREF)((((BYTE)(255) | ((WORD)((BYTE)(255)) << 8)) | (((DWORD)(BYTE)(255)) << 16)) | (((DWORD)(BYTE)(255)) << 24)))
//            : ((COLORREF)((((BYTE)(0) | ((WORD)((BYTE)(0)) << 8)) | (((DWORD)(BYTE)(0)) << 16)) | (((DWORD)(BYTE)(255)) << 24)));
//
//        return color;
//#endif
//
//    }
//    // ����/ͣ����ɫģʽ, ���� DwmSetWindowAttribute ʵ��
//    // ���ú�����ô��ڱ���������ɫģʽ
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
//    // ���ص�ǰ�������ɫ, ���ص���ARGB��ɫ, ʧ�ܷ���0, ARGB��0������ȫ͸��, ������ɫӦ����û��͸����
//    // pfOpaqueBlend = ָʾ��ɫ�Ƿ�Ϊ��͸�����
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
//    //// ���ض�Ӧ�Ļ�ˢ, ���صĻ�ˢ�����ͷ�
//    //inline HBRUSH GetSysBrush(int nIndex)
//    //{
//    //    return GetSysBrush(nIndex, IsDarkMode());
//    //}
//    //// ���ض�Ӧ�Ļ�ˢ, ���صĻ�ˢ�����ͷ�
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
//    //    // �Ƿ�����ɫģʽ
//    //    const bool isDarkMode = IsDarkMode();
//    //    return GetSysColor(nIndex, isDarkMode);
//    //}
//    // �÷��� GetSysColor һ��, ����Ҳһ��
//    inline static COLORREF GetSysColor(int nIndex, bool isDarkMode)
//    {
//        switch (nIndex)
//        {
//        case COLOR_SCROLLBAR:           // ��������ɫ����
//            break;
//
//        case COLOR_BACKGROUND:          // ������ɫ
//        //case COLOR_DESKTOP:
//        {
//            return isDarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
//        }
//        case COLOR_ACTIVECAPTION:       // ����ڱ�������������ǰ��ɫΪ COLOR_CAPTIONTEXT
//        {
//            return isDarkMode ? RGB(93, 90, 88) : RGB(255, 255, 255);
//        }
//        case COLOR_INACTIVECAPTION:     // �ǻ���ڱ��⡣������ǰ��ɫΪ COLOR_INACTIVECAPTIONTEXT
//        {
//            return isDarkMode ? RGB(43, 43, 43) : RGB(255, 255, 255);
//        }
//        case COLOR_MENU:                // �˵�������������ǰ��ɫΪ COLOR_MENUTEXT
//        {
//            return isDarkMode ? RGB(43, 43, 43) : RGB(238, 238, 238);
//        }
//        case COLOR_WINDOW:              // ���ڱ�����������ǰ��ɫ��COLOR_WINDOWTEXT��COLOR_HOTLITE
//        {
//            return isDarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
//        }
//        case COLOR_WINDOWFRAME:         // ���ڱ߿���ɫ
//        {
//            return isDarkMode ? RGB(100, 100, 100) : RGB(100, 100, 100);
//        }
//        case COLOR_MENUTEXT:            // �˵��ı���ɫ
//        {
//            return isDarkMode ? RGB(248, 248, 248) : RGB(0, 0, 0);
//        }
//        case COLOR_WINDOWTEXT:          // �����е��ı��������ı�����ɫΪ COLOR_WINDOW
//        {
//            return isDarkMode ? RGB(248, 248, 248) : RGB(0, 0, 0);
//        }
//        case COLOR_CAPTIONTEXT:         // ���⡢��С��͹�������ͷ���е��ı��������ı�����ɫΪ COLOR_ACTIVECAPTION
//        {
//            return isDarkMode ? RGB(103, 103, 103) : RGB(0, 0, 0);
//        }
//        case COLOR_ACTIVEBORDER:        // ����ڱ߿�
//        {
//            return isDarkMode ? RGB(107, 104, 102) : RGB(180, 180, 180);
//        }
//        case COLOR_INACTIVEBORDER:      // �ǻ���ڱ߿�
//        {
//            return isDarkMode ? RGB(86, 84, 83) : RGB(244, 248, 252);
//        }
//        case COLOR_APPWORKSPACE:        // MDIӦ�ó���ı���ɫ
//        {
//            break;
//        }
//        case COLOR_HIGHLIGHT:           // �ڿؼ���ѡ����������ǰ��ɫΪCOLOR_HIGHLIGHTTEXTɫ
//        {
//            return isDarkMode ? RGB(119, 119, 119) : RGB(204, 232, 255);
//        }
//        case COLOR_HIGHLIGHTTEXT:       // �ؼ�����ѡ����ı��������ı�����ɫΪ COLOR_HIGHLIGHT
//        {
//            return isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
//        }
//        case COLOR_BTNFACE:             // ��ά��ʾԪ�غͶԻ��򱳾�����ɫ
//        //case COLOR_3DFACE:            // ��ά��ʾԪ�غͶԻ��򱳾�����ɫ
//        {
//            return isDarkMode ? RGB(61, 61, 61) : RGB(225, 225, 225);
//        }
//        case COLOR_BTNSHADOW:           // ��ť�߿���ɫ
//        //case COLOR_3DSHADOW:
//        {
//            return isDarkMode ? RGB(112, 112, 112) : RGB(173, 173, 173);
//        }
//        case COLOR_GRAYTEXT:            // ��ɫ�����ı�
//        {
//            return isDarkMode ? RGB(109, 109, 109) : RGB(123, 123, 123);
//        }
//        case COLOR_BTNTEXT:             // ��ť�ϵ����֡������ı���ɫΪ COLOR_BTNFACE
//        {
//            return isDarkMode ? RGB(214, 214, 214) : RGB(0, 0, 0);
//        }
//        case COLOR_INACTIVECAPTIONTEXT: // �ǻ�����е��ı���ɫ�������ı���ɫΪ COLOR_INACTIVECAPTION
//        {
//            return isDarkMode ? RGB(170, 170, 170) : RGB(165, 165, 165);
//        }
//        case COLOR_BTNHIGHLIGHT:        // ��ά��ʾԪ�ص�ͻ����ʾ��ɫ
//        //case COLOR_BTNHILIGHT:
//        //case COLOR_3DHILIGHT:
//        //case COLOR_3DHIGHLIGHT:
//        {
//            break;
//        }
//        case COLOR_3DDKSHADOW:          // ��ά��ʾԪ�ص���ɫ��Ӱ
//        {
//            COLORREF clr1 = ::GetSysColor(COLOR_3DDKSHADOW);
//            COLORREF clr2 = ::GetSysColor(COLOR_3DLIGHT);
//            return isDarkMode ? clr2 : clr1;
//        }
//        case COLOR_3DLIGHT:             // ��ά��ʾԪ�ص�ǳɫ
//        {
//            COLORREF clr1 = ::GetSysColor(COLOR_3DDKSHADOW);
//            COLORREF clr2 = ::GetSysColor(COLOR_3DLIGHT);
//            return isDarkMode ? clr1 : clr2;
//        }
//        case COLOR_INFOTEXT:            // ������ʾ�ؼ����ı���ɫ
//        {
//            return isDarkMode ? RGB(255, 255, 255) : RGB(87, 87, 87);
//        }
//        case COLOR_INFOBK:              // ������ʾ�ؼ��ı���ɫ
//        {
//            return isDarkMode ? RGB(43, 43, 43) : RGB(255, 255, 255);
//        }
//        case COLOR_HOTLIGHT:            // �����ӻ��ȸ��������ɫ
//        {
//            return isDarkMode ? RGB(77, 77, 77) : RGB(0, 102, 204);
//        }
//        case COLOR_GRADIENTACTIVECAPTION:   // ����ڱ���������ɫ�����е��Ҳ���ɫ��COLOR_ACTIVECAPTIONָ�������ɫ
//        {
//            break;
//        }
//        case COLOR_GRADIENTINACTIVECAPTION: // �ǻ���ڱ���������ɫ�����е��Ҳ���ɫ��COLOR_INACTIVECAPTIONָ�������ɫ
//        {
//            break;
//        }
//        case COLOR_MENUHILIGHT:             // ���˵���ʾΪƽ��˵�ʱ������ͻ����ʾ�˵������ɫ
//        {
//            break;
//        }
//        case COLOR_MENUBAR:                 // ���˵���ʾΪƽ��˵�ʱ�˵����ı���ɫ
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
//    // ����ARGB��ɫ�Ƿ�����ɫ, �������true, ����Ӧ�ð���ɫ�ĳ���ɫ
//    inline static bool IsArgbLight(BYTE a, BYTE r, BYTE g, BYTE b)
//    {
//        return IsRgbLight(r, g, b);
//    }
//    // ����ARGB��ɫ�Ƿ�����ɫ, �������true, ����Ӧ�ð���ɫ�ĳ���ɫ
//    inline static bool IsArgbLight(DWORD argb)
//    {
//        BYTE a = ((BYTE)((argb & 0xff000000) >> 24));
//        BYTE r = ((BYTE)((argb & 0x00ff0000) >> 16));
//        BYTE g = ((BYTE)((argb & 0x0000ff00) >> 8));
//        BYTE b = ((BYTE)((argb & 0x000000ff)));
//        return IsRgbLight(r, g, b);
//    }
//    // ����RGB��ɫ�Ƿ�����ɫ, �������true, ����Ӧ�ð���ɫ�ĳ���ɫ
//    inline static bool IsRgbLight(COLORREF rgb)
//    {
//        BYTE r = ((BYTE)((rgb & 0x000000ff)));
//        BYTE g = ((BYTE)((rgb & 0x0000ff00) >> 8));
//        BYTE b = ((BYTE)((rgb & 0x00ff0000) >> 16));
//        return IsRgbLight(r, g, b);
//    }
//    // ����RGB��ɫ�Ƿ�����ɫ, �������true, ����Ӧ�ð���ɫ�ĳ���ɫ
//    inline static bool IsRgbLight(BYTE r, BYTE g, BYTE b)
//    {
//        return (((5 * g) + (2 * r) + b) > (8 * 128));
//    }
//
//};
//



typedef struct THEMECOLOR_STRUCT
{
    COLORREF crBack                 ;   // ������ɫ
    COLORREF crBackExtern           ;   // �ⲿ�ı�����ɫ, ����ѡ��е��Ӽ�
    COLORREF crStateBar             ;   // �����/������/״̬�ж����������ɫ

    COLORREF crShadow               ;   // ��Ӱ��ɫ
    COLORREF crShadowRun            ;   // ����ʱ��Ӱ��ɫ
    COLORREF crShadowNoFocus        ;   // ȡ������ʱ�ı߿���ɫ

    COLORREF crTipsBack             ;   // ��ʾ���ڱ�����ɫ
    COLORREF crTipsText             ;   // ��ʾ�����ı���ɫ
    COLORREF crTipsBorder           ;   // ��ʾ���ڱ߿���ɫ

    COLORREF crMenuBorder           ;   // �˵��߿���ɫ
    COLORREF crMenuBack             ;   // �˵�������ɫ, �����˵�ʹ�õı���ɫ, �˵�������ʱʹ�õı���ɫ
    COLORREF crMenuHot              ;   // �˵��ȵ�����ɫ
    COLORREF crMenuBan              ;   // �˵���ֹ��ɫ
    COLORREF crMenuText             ;   // �˵��ı���ɫ
    COLORREF crMenuTextHot          ;   // �˵��ı��ȵ���ɫ
    COLORREF crMenuTextBan          ;   // �˵��ı���ֹ��ɫ

    COLORREF crText                 ;   // ��ͨ�ı��滭�õ���ɫ
    COLORREF crTextHot              ;   // �ȵ��ı��滭�õ���ɫ
    COLORREF crTextDown             ;   // �����ı��滭�õ���ɫ
    COLORREF crTextBan              ;   // ��ֹ�ı��滭�õ���ɫ

    COLORREF crNormal               ;   // �����ͨ��ɫ
    COLORREF crHover                ;   // ���������ɫ
    COLORREF crFocus                ;   // ���������ɫ
    COLORREF crChecked              ;   // ���ѡ����ɫ
    COLORREF crDown                 ;   // ���������ɫ
    COLORREF crSelected             ;   // ���ѡ����ɫ
    COLORREF crHot                  ;   // ����ȵ���ɫ
    COLORREF crBan                  ;   // �����ֹ��ɫ
    COLORREF crBorder               ;   // ����߿���ɫ
    COLORREF crBorderHot            ;   // ����߿��ȵ���ɫ

    COLORREF crTextStatusBar        ;   // �ײ�״̬�����ı���ɫ
    COLORREF crTextStatusBarRun     ;   // �ײ�״̬������ʱ���ı���ɫ
    COLORREF crTextStatusBarNf      ;   // �ײ�״̬����������ı���ɫ

    COLORREF crScrollBtn1Normal     ;   // ��������ť1������ɫ
    COLORREF crScrollBtn1Hot        ;   // ��������ť1�ȵ���ɫ
    COLORREF crScrollBtn1Down       ;   // ��������ť1������ɫ
    COLORREF crScrollBtn2Normal     ;   // ��������ť2������ɫ
    COLORREF crScrollBtn2Hot        ;   // ��������ť2�ȵ���ɫ
    COLORREF crScrollBtn2Down       ;   // ��������ť2������ɫ
    COLORREF crScrollThumbNormal    ;   // ������������ɫ
    COLORREF crScrollThumbHot       ;   // �����������ȵ���ɫ
    COLORREF crScrollThumbDown      ;   // ���������鰴����ɫ
    COLORREF crScrollBack           ;   // ������������ɫ

    COLORREF crToolSeplineLeft      ;   // �������ָ��������ɫ
    COLORREF crToolSeplineRight     ;   // �������ָ����ұ���ɫ

    COLORREF crStatelineNormal      ;   // ���ڶ����м��ߵ�������ɫ
    COLORREF crStatelineHot         ;   // ���ڶ����м��ߵ��ȵ���ɫ

    BOOL     isDarkMode             ;   // ��ǰ�ṹ����ɫ�Ƿ�����ɫģʽ����ɫ

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

    virtual COLORREF crBack                 ( COLORREF* pValue = 0 ) = 0;   // ������ɫ
    virtual COLORREF crBackExtern           ( COLORREF* pValue = 0 ) = 0;   // �ⲿ�ı�����ɫ, ����ѡ��е��Ӽ�
    virtual COLORREF crStateBar             ( COLORREF* pValue = 0 ) = 0;   // �����/������/״̬�ж����������ɫ

    virtual COLORREF crShadow               ( COLORREF* pValue = 0 ) = 0;   // ��Ӱ��ɫ
    virtual COLORREF crShadowRun            ( COLORREF* pValue = 0 ) = 0;   // ����ʱ��Ӱ��ɫ
    virtual COLORREF crShadowNoFocus        ( COLORREF* pValue = 0 ) = 0;   // ȡ������ʱ�ı߿���ɫ

    virtual COLORREF crTipsBack             ( COLORREF* pValue = 0 ) = 0;   // ��ʾ���ڱ�����ɫ
    virtual COLORREF crTipsText             ( COLORREF* pValue = 0 ) = 0;   // ��ʾ�����ı���ɫ
    virtual COLORREF crTipsBorder           ( COLORREF* pValue = 0 ) = 0;   // ��ʾ���ڱ߿���ɫ

    virtual COLORREF crMenuBorder           ( COLORREF* pValue = 0 ) = 0;   // �˵��߿���ɫ
    virtual COLORREF crMenuBack             ( COLORREF* pValue = 0 ) = 0;   // �˵�������ɫ, �����˵�ʹ�õı���ɫ, �˵�������ʱʹ�õı���ɫ
    virtual COLORREF crMenuHot              ( COLORREF* pValue = 0 ) = 0;   // �˵��ȵ�����ɫ
    virtual COLORREF crMenuBan              ( COLORREF* pValue = 0 ) = 0;   // �˵���ֹ��ɫ
    virtual COLORREF crMenuText             ( COLORREF* pValue = 0 ) = 0;   // �˵��ı���ɫ
    virtual COLORREF crMenuTextHot          ( COLORREF* pValue = 0 ) = 0;   // �˵��ı��ȵ���ɫ
    virtual COLORREF crMenuTextBan          ( COLORREF* pValue = 0 ) = 0;   // �˵��ı���ֹ��ɫ

    virtual COLORREF crText                 ( COLORREF* pValue = 0 ) = 0;   // ��ͨ�ı��滭�õ���ɫ
    virtual COLORREF crTextHot              ( COLORREF* pValue = 0 ) = 0;   // �ȵ��ı��滭�õ���ɫ
    virtual COLORREF crTextDown             ( COLORREF* pValue = 0 ) = 0;   // �����ı��滭�õ���ɫ
    virtual COLORREF crTextBan              ( COLORREF* pValue = 0 ) = 0;   // ��ֹ�ı��滭�õ���ɫ

    virtual COLORREF crNormal               ( COLORREF* pValue = 0 ) = 0;   // �����ͨ��ɫ
    virtual COLORREF crHover                ( COLORREF* pValue = 0 ) = 0;   // ���������ɫ
    virtual COLORREF crFocus                ( COLORREF* pValue = 0 ) = 0;   // ���������ɫ
    virtual COLORREF crChecked              ( COLORREF* pValue = 0 ) = 0;   // ���ѡ����ɫ
    virtual COLORREF crDown                 ( COLORREF* pValue = 0 ) = 0;   // ���������ɫ
    virtual COLORREF crSelected             ( COLORREF* pValue = 0 ) = 0;   // ���ѡ����ɫ
    virtual COLORREF crHot                  ( COLORREF* pValue = 0 ) = 0;   // ����ȵ���ɫ
    virtual COLORREF crBan                  ( COLORREF* pValue = 0 ) = 0;   // �����ֹ��ɫ
    virtual COLORREF crBorder               ( COLORREF* pValue = 0 ) = 0;   // ����߿���ɫ
    virtual COLORREF crBorderHot            ( COLORREF* pValue = 0 ) = 0;   // ����߿��ȵ���ɫ

    virtual COLORREF crTextStatusBar        ( COLORREF* pValue = 0 ) = 0;   // �ײ�״̬�����ı���ɫ
    virtual COLORREF crTextStatusBarRun     ( COLORREF* pValue = 0 ) = 0;   // �ײ�״̬������ʱ���ı���ɫ
    virtual COLORREF crTextStatusBarNf      ( COLORREF* pValue = 0 ) = 0;   // �ײ�״̬����������ı���ɫ

    virtual COLORREF crScrollBtn1Normal     ( COLORREF* pValue = 0 ) = 0;   // ��������ť1������ɫ
    virtual COLORREF crScrollBtn1Hot        ( COLORREF* pValue = 0 ) = 0;   // ��������ť1�ȵ���ɫ
    virtual COLORREF crScrollBtn1Down       ( COLORREF* pValue = 0 ) = 0;   // ��������ť1������ɫ
    virtual COLORREF crScrollBtn2Normal     ( COLORREF* pValue = 0 ) = 0;   // ��������ť2������ɫ
    virtual COLORREF crScrollBtn2Hot        ( COLORREF* pValue = 0 ) = 0;   // ��������ť2�ȵ���ɫ
    virtual COLORREF crScrollBtn2Down       ( COLORREF* pValue = 0 ) = 0;   // ��������ť2������ɫ
    virtual COLORREF crScrollThumbNormal    ( COLORREF* pValue = 0 ) = 0;   // ������������ɫ
    virtual COLORREF crScrollThumbHot       ( COLORREF* pValue = 0 ) = 0;   // �����������ȵ���ɫ
    virtual COLORREF crScrollThumbDown      ( COLORREF* pValue = 0 ) = 0;   // ���������鰴����ɫ
    virtual COLORREF crScrollBack           ( COLORREF* pValue = 0 ) = 0;   // ������������ɫ

    virtual COLORREF crToolSeplineLeft      ( COLORREF* pValue = 0 ) = 0;   // �������ָ��������ɫ
    virtual COLORREF crToolSeplineRight     ( COLORREF* pValue = 0 ) = 0;   // �������ָ����ұ���ɫ

    virtual COLORREF crStatelineNormal      ( COLORREF* pValue = 0 ) = 0;   // ���ڶ����м��ߵ�������ɫ
    virtual COLORREF crStatelineHot         ( COLORREF* pValue = 0 ) = 0;   // ���ڶ����м��ߵ��ȵ���ɫ


public:

    // ���ݽ���id��ȡ���ڴ��еĹ����ڴ�
    // pid = ����id, ���Ϊ0, ���ȡ��ǰ����
    // isCreate = ������ʱ�Ƿ񴴽�
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
    // ��ʼ����ǰ���̵���ɫ�ṹ
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
    // �ػ���������Լ�����������Ӵ���
    // hWnd = ���ػ��Ĵ���, ��ݹ�ö������������µ��Ӵ���
    // isSetWindowPos = �Ƿ���� SetWindowPos �ı�ǿͻ���
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


    // �����Ƿ�����ɫģʽ, �����ڲ�����ɫ�ж�
    inline static bool IsDarkMode(DWORD color)
    {
        return IsArgbLight(color);
    }

    // ȡǰ��ɫ, ����ARGB��ɫ
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
                value = 0;    // ��Ҫ�ٶ��ü�������ڣ�����Ҳ���������Ĭ��ֵ
            }
            RegCloseKey(hKey);
        }

        // �Ǻ�ɫ����ͷ�����ɫ, ���򷵻غ�ɫ
        DWORD color = value == 0
            ? ((COLORREF)((((BYTE)(255) | ((WORD)((BYTE)(255)) << 8)) | (((DWORD)(BYTE)(255)) << 16)) | (((DWORD)(BYTE)(255)) << 24)))
            : ((COLORREF)((((BYTE)(0) | ((WORD)((BYTE)(0)) << 8)) | (((DWORD)(BYTE)(0)) << 16)) | (((DWORD)(BYTE)(255)) << 24)));

        return color;
#endif
    }


    // ����/ͣ����ɫģʽ, ���� DwmSetWindowAttribute ʵ��
    // ���ú�����ô��ڱ���������ɫģʽ
    inline static bool SwitchDarkMode(HWND hWnd, bool isDarkMode)
    {
        BOOL value = isDarkMode;
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
        ::DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
        return isDarkMode;
    }

    // ���ص�ǰ�������ɫ, ���ص���ARGB��ɫ, ʧ�ܷ���0, ARGB��0������ȫ͸��, ������ɫӦ����û��͸����
    // pfOpaqueBlend = ָʾ��ɫ�Ƿ�Ϊ��͸�����
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


    // �÷��� GetSysColor һ��, ����Ҳһ��
    inline static COLORREF GetSysColor(int nIndex, bool isDarkMode)
    {
        switch (nIndex)
        {
        case COLOR_SCROLLBAR:           // ��������ɫ����
            break;

        case COLOR_BACKGROUND:          // ������ɫ
        //case COLOR_DESKTOP:
        {
            return isDarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
        }
        case COLOR_ACTIVECAPTION:       // ����ڱ�������������ǰ��ɫΪ COLOR_CAPTIONTEXT
        {
            return isDarkMode ? RGB(93, 90, 88) : RGB(255, 255, 255);
        }
        case COLOR_INACTIVECAPTION:     // �ǻ���ڱ��⡣������ǰ��ɫΪ COLOR_INACTIVECAPTIONTEXT
        {
            return isDarkMode ? RGB(43, 43, 43) : RGB(255, 255, 255);
        }
        case COLOR_MENU:                // �˵�������������ǰ��ɫΪ COLOR_MENUTEXT
        {
            return isDarkMode ? RGB(43, 43, 43) : RGB(238, 238, 238);
        }
        case COLOR_WINDOW:              // ���ڱ�����������ǰ��ɫ��COLOR_WINDOWTEXT��COLOR_HOTLITE
        {
            return isDarkMode ? RGB(32, 32, 32) : RGB(255, 255, 255);
        }
        case COLOR_WINDOWFRAME:         // ���ڱ߿���ɫ
        {
            return isDarkMode ? RGB(100, 100, 100) : RGB(100, 100, 100);
        }
        case COLOR_MENUTEXT:            // �˵��ı���ɫ
        {
            return isDarkMode ? RGB(248, 248, 248) : RGB(0, 0, 0);
        }
        case COLOR_WINDOWTEXT:          // �����е��ı��������ı�����ɫΪ COLOR_WINDOW
        {
            return isDarkMode ? RGB(248, 248, 248) : RGB(0, 0, 0);
        }
        case COLOR_CAPTIONTEXT:         // ���⡢��С��͹�������ͷ���е��ı��������ı�����ɫΪ COLOR_ACTIVECAPTION
        {
            return isDarkMode ? RGB(103, 103, 103) : RGB(0, 0, 0);
        }
        case COLOR_ACTIVEBORDER:        // ����ڱ߿�
        {
            return isDarkMode ? RGB(107, 104, 102) : RGB(180, 180, 180);
        }
        case COLOR_INACTIVEBORDER:      // �ǻ���ڱ߿�
        {
            return isDarkMode ? RGB(86, 84, 83) : RGB(244, 248, 252);
        }
        case COLOR_APPWORKSPACE:        // MDIӦ�ó���ı���ɫ
        {
            break;
        }
        case COLOR_HIGHLIGHT:           // �ڿؼ���ѡ����������ǰ��ɫΪCOLOR_HIGHLIGHTTEXTɫ
        {
            return isDarkMode ? RGB(119, 119, 119) : RGB(204, 232, 255);
        }
        case COLOR_HIGHLIGHTTEXT:       // �ؼ�����ѡ����ı��������ı�����ɫΪ COLOR_HIGHLIGHT
        {
            return isDarkMode ? RGB(255, 255, 255) : RGB(0, 0, 0);
        }
        case COLOR_BTNFACE:             // ��ά��ʾԪ�غͶԻ��򱳾�����ɫ
        //case COLOR_3DFACE:            // ��ά��ʾԪ�غͶԻ��򱳾�����ɫ
        {
            return isDarkMode ? RGB(61, 61, 61) : RGB(225, 225, 225);
        }
        case COLOR_BTNSHADOW:           // ��ť�߿���ɫ
        //case COLOR_3DSHADOW:
        {
            return isDarkMode ? RGB(112, 112, 112) : RGB(173, 173, 173);
        }
        case COLOR_GRAYTEXT:            // ��ɫ�����ı�
        {
            return isDarkMode ? RGB(109, 109, 109) : RGB(123, 123, 123);
        }
        case COLOR_BTNTEXT:             // ��ť�ϵ����֡������ı���ɫΪ COLOR_BTNFACE
        {
            return isDarkMode ? RGB(214, 214, 214) : RGB(0, 0, 0);
        }
        case COLOR_INACTIVECAPTIONTEXT: // �ǻ�����е��ı���ɫ�������ı���ɫΪ COLOR_INACTIVECAPTION
        {
            return isDarkMode ? RGB(170, 170, 170) : RGB(165, 165, 165);
        }
        case COLOR_BTNHIGHLIGHT:        // ��ά��ʾԪ�ص�ͻ����ʾ��ɫ
        //case COLOR_BTNHILIGHT:
        //case COLOR_3DHILIGHT:
        //case COLOR_3DHIGHLIGHT:
        {
            break;
        }
        case COLOR_3DDKSHADOW:          // ��ά��ʾԪ�ص���ɫ��Ӱ
        {
            COLORREF clr1 = ::GetSysColor(COLOR_3DDKSHADOW);
            COLORREF clr2 = ::GetSysColor(COLOR_3DLIGHT);
            return isDarkMode ? clr2 : clr1;
        }
        case COLOR_3DLIGHT:             // ��ά��ʾԪ�ص�ǳɫ
        {
            COLORREF clr1 = ::GetSysColor(COLOR_3DDKSHADOW);
            COLORREF clr2 = ::GetSysColor(COLOR_3DLIGHT);
            return isDarkMode ? clr1 : clr2;
        }
        case COLOR_INFOTEXT:            // ������ʾ�ؼ����ı���ɫ
        {
            return isDarkMode ? RGB(255, 255, 255) : RGB(87, 87, 87);
        }
        case COLOR_INFOBK:              // ������ʾ�ؼ��ı���ɫ
        {
            return isDarkMode ? RGB(43, 43, 43) : RGB(255, 255, 255);
        }
        case COLOR_HOTLIGHT:            // �����ӻ��ȸ��������ɫ
        {
            return isDarkMode ? RGB(77, 77, 77) : RGB(0, 102, 204);
        }
        case COLOR_GRADIENTACTIVECAPTION:   // ����ڱ���������ɫ�����е��Ҳ���ɫ��COLOR_ACTIVECAPTIONָ�������ɫ
        {
            break;
        }
        case COLOR_GRADIENTINACTIVECAPTION: // �ǻ���ڱ���������ɫ�����е��Ҳ���ɫ��COLOR_INACTIVECAPTIONָ�������ɫ
        {
            break;
        }
        case COLOR_MENUHILIGHT:             // ���˵���ʾΪƽ��˵�ʱ������ͻ����ʾ�˵������ɫ
        {
            break;
        }
        case COLOR_MENUBAR:                 // ���˵���ʾΪƽ��˵�ʱ�˵����ı���ɫ
        {
            break;
        }
        default:
            break;
        }
        return ::GetSysColor(nIndex);
    }

public:

    // ����ARGB��ɫ�Ƿ�����ɫ, �������true, ����Ӧ�ð���ɫ�ĳ���ɫ
    inline static bool IsArgbLight(BYTE a, BYTE r, BYTE g, BYTE b)
    {
        return IsRgbLight(r, g, b);
    }
    // ����ARGB��ɫ�Ƿ�����ɫ, �������true, ����Ӧ�ð���ɫ�ĳ���ɫ
    inline static bool IsArgbLight(DWORD argb)
    {
        BYTE a = ((BYTE)((argb & 0xff000000) >> 24));
        BYTE r = ((BYTE)((argb & 0x00ff0000) >> 16));
        BYTE g = ((BYTE)((argb & 0x0000ff00) >> 8));
        BYTE b = ((BYTE)((argb & 0x000000ff)));
        return IsRgbLight(r, g, b);
    }
    // ����RGB��ɫ�Ƿ�����ɫ, �������true, ����Ӧ�ð���ɫ�ĳ���ɫ
    inline static bool IsRgbLight(COLORREF rgb)
    {
        BYTE r = ((BYTE)((rgb & 0x000000ff)));
        BYTE g = ((BYTE)((rgb & 0x0000ff00) >> 8));
        BYTE b = ((BYTE)((rgb & 0x00ff0000) >> 16));
        return IsRgbLight(r, g, b);
    }
    // ����RGB��ɫ�Ƿ�����ɫ, �������true, ����Ӧ�ð���ɫ�ĳ���ɫ
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

