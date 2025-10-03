#pragma once
#ifndef _CDARKMODE_H_

#include "UxthemeAPI.h"

typedef  HTHEME(WINAPI* fnOpenNcThemeData)(HWND hWnd, LPCWSTR pszClassList); // ordinal 49

class CDarkMode
{
    using fnRtlGetNtVersionNumbers = void (WINAPI*)(LPDWORD major, LPDWORD minor, LPDWORD build);
    using fnSetWindowCompositionAttribute = BOOL(WINAPI*)(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA*);
    

public:

    // 初始化深色模式API集
    static bool InitDarkMode(bool isDark, fnOpenNcThemeData fn_Hook_OpenNcThemeData = 0)
    {
        DWORD major, minor, buildNumber;
        CUxThemeApi::GetVersionNumbers(&major, &minor, &buildNumber);
        if (!(major == 10 && minor == 0 && CheckBuildNumber(buildNumber)))
            return false;

        CUxThemeApi::FUN_STRUCT* pFun;
        if (CUxThemeApi::init(&pFun))
        {
            pFun->darkModeSupported = true;

            CUxThemeApi::SetPreferredAppMode(isDark ? ForceDark : ForceLight);
            //CUxThemeApi::AllowDarkModeForApp(isDark);
            CUxThemeApi::RefreshImmersiveColorPolicyState();

            pFun->darkModeEnabled = CUxThemeApi::ShouldAppsUseDarkMode() && !IsHighContrast();

            FixDarkScrollBar(true, fn_Hook_OpenNcThemeData);
            return true;
        }
        return false;
    }

    static void SwitchDarkMode(BOOL isDark)
    {
        CUxThemeApi::AllowDarkModeForApp(isDark);
        CUxThemeApi::RefreshImmersiveColorPolicyState();

        CUxThemeApi::ShouldAppsUseDarkMode();
        FixDarkScrollBar(isDark);
    }

    static bool darkModeSupported()
    {
        CUxThemeApi::FUN_STRUCT* pFun;
        if (CUxThemeApi::init(&pFun))
            return pFun->darkModeSupported;
        return false;
    }
    static bool darkModeEnabled(bool* pVl = 0)
    {
        CUxThemeApi::FUN_STRUCT* pFun;
        if (CUxThemeApi::init(&pFun))
        {
            if (pVl)
                pFun->darkModeEnabled = *pVl;
            return pFun->darkModeEnabled;
        }
        return false;
    }
public:

    static bool WINAPI AllowDarkModeForWindow(HWND hWnd, bool allow)
    {
        return CUxThemeApi::AllowDarkModeForWindow(hWnd, allow);
    }
    static bool WINAPI ShouldAppsUseDarkMode()
    {
        return CUxThemeApi::ShouldAppsUseDarkMode();
    }
    static bool IsHighContrast()
    {
        HIGHCONTRASTW highContrast = { sizeof(highContrast) };
        if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
            return highContrast.dwFlags & HCF_HIGHCONTRASTON;
        return false;
    }

    static void RefreshTitleBarThemeColor(HWND hWnd)
    {
        BOOL isDark = FALSE;
        if (CUxThemeApi::IsDarkModeAllowedForWindow(hWnd) &&
            CUxThemeApi::ShouldAppsUseDarkMode() &&
            !IsHighContrast())
        {
            isDark = TRUE;
        }
        if (CUxThemeApi::GetVersionNumbers() < 18362)
            SetPropW(hWnd, L"UseImmersiveDarkModeColors", reinterpret_cast<HANDLE>(static_cast<INT_PTR>(isDark)));
        else
        {
            WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &isDark, sizeof(isDark) };
            CUxThemeApi::SetWindowCompositionAttribute(hWnd, &data);
        }
    }

    static bool IsColorSchemeChangeMessage(LPARAM lParam)
    {
        bool is = false;
        if (lParam && CompareStringOrdinal(reinterpret_cast<LPCWCH>(lParam), -1, L"ImmersiveColorSet", -1, TRUE) == CSTR_EQUAL)
        {
            CUxThemeApi::RefreshImmersiveColorPolicyState();
            is = true;
        }
        CUxThemeApi::GetIsImmersiveColorUsingHighContrast(IHCM_REFRESH);
        return is;
    }

    static bool IsColorSchemeChangeMessage(UINT message, LPARAM lParam)
    {
        if (message == WM_SETTINGCHANGE)
            return IsColorSchemeChangeMessage(lParam);
        return false;
    }



private:
    
    inline static HMODULE GetNtDll()
    {
        return GetModuleHandleW(L"ntdll.dll");
    }
    constexpr static bool CheckBuildNumber(DWORD buildNumber)
    {
        return buildNumber >= 17763;    // 大于等于1809 的版本就视为支持深色模式

        return (buildNumber == 17763 || // 1809
                buildNumber == 18362 || // 1903
                buildNumber == 18363 || // 1909
                buildNumber == 19041); // 2004
    }

public:
    static void FixDarkScrollBar(bool isHook, fnOpenNcThemeData fn_Hook_OpenNcThemeData = 0)
    {
        static PIMAGE_THUNK_DATA addr;
        if (!addr)
        {
            HMODULE hComctl = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (!hComctl)
                return;
            addr = IATHOOK::FindDelayLoadThunkInModule(hComctl, "uxtheme.dll", 49); // OpenNcThemeData
        }

        if (!addr)
            return;
        
        static ULONGLONG oldFunction;
        if (!oldFunction)
            oldFunction = addr->u1.Function;
        
        DWORD oldProtect;
        if (VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), PAGE_READWRITE, &oldProtect))
        {
            fnOpenNcThemeData MyOpenThemeData = [](HWND hWnd, LPCWSTR classList) -> HTHEME {
                if (wcscmp(classList, L"ScrollBar") == 0)
                {
                    hWnd = nullptr;
                    classList = L"Explorer::ScrollBar";
                    //classList = L"DarkMode_Explorer";
                }
                return CUxThemeApi::OpenNcThemeData(hWnd, classList);
            };

            if (isHook)
            {
                addr->u1.Function = fn_Hook_OpenNcThemeData
                    ? reinterpret_cast<ULONG_PTR>(static_cast<fnOpenNcThemeData>(fn_Hook_OpenNcThemeData))
                    : reinterpret_cast<ULONG_PTR>(static_cast<fnOpenNcThemeData>(MyOpenThemeData));
            }
            else
            {
                addr->u1.Function = static_cast<ULONG_PTR>(oldFunction);
            }
            VirtualProtect(addr, sizeof(IMAGE_THUNK_DATA), oldProtect, &oldProtect);
        }


    }


};


#endif // !_CDARKMODE_H_