#pragma once
#ifndef _CSCALE_H_
#define _CSCALE_H_
#include <GetDpiForWindow.h>

// 默认dpi是96, 也就是100%缩放
class CScale
{
    UINT m_dpi;
public:
    CScale() : m_dpi(USER_DEFAULT_SCREEN_DPI)
    {

    }

    static bool SetDPIAware()
    {
        typedef HRESULT(WINAPI* pfn_SetProcessDpiAwareness)(int value);
        typedef HRESULT(WINAPI* pfn_SetProcessDpiAwarenessContext)(int value);
        typedef DPI_AWARENESS_CONTEXT(WINAPI* pfn_SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext);
        bool isFree = false;
        HMODULE Shcore = GetModuleHandleW(L"Shcore.dll");
        if (!Shcore)
        {
            Shcore = LoadLibraryW(L"Shcore.dll");
            isFree = true;
        }
        if (!Shcore)
            return false;


        // win10才支持的设置dpi方式
        pfn_SetProcessDpiAwareness pfnSetProcessDpiAwareness = (pfn_SetProcessDpiAwareness)GetProcAddress(Shcore, "SetProcessDpiAwareness");
        pfn_SetProcessDpiAwarenessContext pfnSetProcessDpiAwarenessContext = (pfn_SetProcessDpiAwarenessContext)GetProcAddress(Shcore, "SetProcessDpiAwarenessContext");
        pfn_SetThreadDpiAwarenessContext pfnSetThreadDpiAwarenessContext = (pfn_SetThreadDpiAwarenessContext)GetProcAddress(Shcore, "SetThreadDpiAwarenessContext");
        if (pfnSetProcessDpiAwareness)
        {
            enum PROCESS_DPI_AWARENESS {
                PROCESS_DPI_UNAWARE = 0,    // DPI 不知道。 此应用不会缩放 DPI 更改，并且始终假定其比例系数为 100% (96 DPI) 。 系统将在任何其他 DPI 设置上自动缩放它
                PROCESS_SYSTEM_DPI_AWARE = 1,    // 统 DPI 感知。 此应用不会缩放 DPI 更改。 它将查询 DPI 一次，并在应用的生存期内使用该值。 如果 DPI 发生更改，应用将不会调整为新的 DPI 值。 当 DPI 与系统值发生更改时，系统会自动纵向扩展或缩减它。
                PROCESS_PER_MONITOR_DPI_AWARE = 2     // 按监视器 DPI 感知。 此应用在创建 DPI 时检查 DPI，并在 DPI 发生更改时调整比例系数。 系统不会自动缩放这些应用程序
            };

            // 感知多个屏幕的dpi
            HRESULT hr = pfnSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

            if (pfnSetThreadDpiAwarenessContext)
                pfnSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

            if (FAILED(hr))
            {
                //wstr::dbg(L"开启DPI感知失败, 错误码 = 0x%08X\n", GetLastError());
#ifdef _DEBUG
                __debugbreak();
#endif
                return false;
            }
        }
        else
        {
            // win10以下的设置dpi方式, 不能感知多个屏幕
            BOOL bRet = SetProcessDPIAware();
            //if (!bRet)
            //{
            //    wstr::dbg(L"开启DPI感知失败, 错误码 = 0x%08X\n", GetLastError());
            //    return 0;
            //}
        }

        if (isFree)
            FreeLibrary(Shcore);


        return true;
    }

    // 返回设置后的dpi, 一般在 WM_DPICHANGED 消息下调用, 传递 LOWORD(wParam)
    inline UINT SetDpi(UINT dpi)
    {
        m_dpi = dpi;
        return m_dpi;
    }
    // 返回设置后的dpi
    inline UINT SetDpi(HWND hWnd)
    {
        m_dpi = GetDpiForWindow_d(hWnd);
        return m_dpi;
    }
    inline UINT GetDpi() const
    {
        return m_dpi;
    }
    inline int scale(int x) const
    {
        int ret = MulDiv(x, m_dpi, USER_DEFAULT_SCREEN_DPI);
        if (ret == 0)
            return x;
        return ret;
    }
    inline int scale(RECT& rc) const
    {
        rc.left     = MulDiv(rc.left,   m_dpi, USER_DEFAULT_SCREEN_DPI);
        rc.top      = MulDiv(rc.top,    m_dpi, USER_DEFAULT_SCREEN_DPI);
        rc.right    = MulDiv(rc.right,  m_dpi, USER_DEFAULT_SCREEN_DPI);
        rc.bottom   = MulDiv(rc.bottom, m_dpi, USER_DEFAULT_SCREEN_DPI);
        return rc.left;
    };

    inline int unscale(int x) const
    {
        return MulDiv(x, USER_DEFAULT_SCREEN_DPI, m_dpi);
    };


    inline int rerect(RECT& rc) const
    {
        return unscale(rc);
    }
    inline int unscale(RECT& rc) const
    {
        rc.left     = MulDiv(rc.left    , USER_DEFAULT_SCREEN_DPI, m_dpi);
        rc.top      = MulDiv(rc.top     , USER_DEFAULT_SCREEN_DPI, m_dpi);
        rc.right    = MulDiv(rc.right   , USER_DEFAULT_SCREEN_DPI, m_dpi);
        rc.bottom   = MulDiv(rc.bottom  , USER_DEFAULT_SCREEN_DPI, m_dpi);
        return rc.left;
    };
    
    // 把矩形按指定dpi转换成新的dpi值
    // oldDpi表示x这个值原来的dpi值
    // newDpi就是缩放后的dpi值, 传递 USER_DEFAULT_SCREEN_DPI 就是还原到100%缩放
    static int scale(int x, UINT oldDpi, UINT newDpi)
    {
        int ret = MulDiv(x, newDpi, oldDpi);
        if (ret == 0)
            return x;
        return ret;
    }
    
    // 把矩形按指定dpi转换成新的dpi值
    // oldDpi表示rc这个矩形原来的dpi值
    // newDpi就是缩放后的dpi值, 传递 USER_DEFAULT_SCREEN_DPI 就是还原到100%缩放
    static int scale(RECT& rc, UINT oldDpi, UINT newDpi)
    {
        rc.left     = scale(rc.left,   oldDpi, newDpi);
        rc.top      = scale(rc.top,    oldDpi, newDpi);
        rc.right    = scale(rc.right,  oldDpi, newDpi);
        rc.bottom   = scale(rc.bottom, oldDpi, newDpi);
        return rc.left;
    };
    // 将指定dpi的值转换成新的dpi值
    static int unscale(int x, UINT oldDpi, UINT newDpi)
    {
        return MulDiv(x, oldDpi, newDpi);

        // 先转换为96的缩放值
        int oldVl = MulDiv(x, USER_DEFAULT_SCREEN_DPI, oldDpi);

        int ret = MulDiv(oldVl, newDpi, USER_DEFAULT_SCREEN_DPI);
        if (ret == 0 || oldVl == 0)
        {
            // 计算缩放因子
            const double scaleFactor = static_cast<double>(newDpi) / static_cast<double>(oldDpi);
            // 应用缩放因子并转换为整数
            return static_cast<int>(static_cast<double>(x) * scaleFactor);
        }
        return ret;
    };

    // 将指定dpi的值转换成新的dpi值
    static int unscale(RECT& rc, UINT oldDpi, UINT newDpi)
    {
        rc.left     = unscale((int)rc.left  , oldDpi, newDpi);
        rc.top      = unscale((int)rc.top   , oldDpi, newDpi);
        rc.right    = unscale((int)rc.right , oldDpi, newDpi);
        rc.bottom   = unscale((int)rc.bottom, oldDpi, newDpi);
        return rc.left;
    };
public:
    inline int operator()(int x) const
    {
        return scale(x);
    }
    inline int operator()(RECT& rc) const
    {
        return scale(rc);
    }
    inline operator UINT() const
    {
        return m_dpi;
    }
    inline int operator=(UINT dpi)
    {
        return SetDpi(dpi);
    }
    inline int operator=(HWND hWnd)
    {
        return SetDpi(hWnd);
    }
};



#endif  // _CSCALE_H_
