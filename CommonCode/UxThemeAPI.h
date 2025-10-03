#pragma once
#ifndef _UXTHEMEAPI_H_

#include <Windows.h>
#include <Uxtheme.h>

enum IMMERSIVE_HC_CACHE_MODE
{
    IHCM_USE_CACHED_VALUE,
    IHCM_REFRESH
};

// 1903 18362
enum PreferredAppMode
{
    Default,
    AllowDark,
    ForceDark,
    ForceLight,
    Max
};

enum WINDOWCOMPOSITIONATTRIB
{
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_HOLOGRAPHIC = 23,
    WCA_EXCLUDED_FROM_DDA = 24,
    WCA_PASSIVEUPDATEMODE = 25,
    WCA_USEDARKMODECOLORS = 26,
    WCA_LAST = 27
};
struct WINDOWCOMPOSITIONATTRIBDATA
{
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};


class CUxThemeApi
{
    static FARPROC GetThemeAddr(LPCSTR name)
    {
        static HMODULE hUxtheme;
        if (!hUxtheme)
            hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!hUxtheme)
            return 0;
        return GetProcAddress(hUxtheme, name);
    }


public:

    struct FUN_STRUCT
    {
        typedef  void (WINAPI* fnRtlGetNtVersionNumbers)(LPDWORD major, LPDWORD minor, LPDWORD build);
        typedef  BOOL(WINAPI* fnSetWindowCompositionAttribute)(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA*);
        // 1809 17763
        typedef  bool (WINAPI* fnShouldAppsUseDarkMode)(); // ordinal 132
        typedef  bool (WINAPI* fnAllowDarkModeForWindow)(HWND hWnd, bool allow); // ordinal 133
        typedef  bool (WINAPI* fnAllowDarkModeForApp)(bool allow); // ordinal 135, in 1809
        typedef  void (WINAPI* fnFlushMenuThemes)(); // ordinal 136
        typedef  void (WINAPI* fnRefreshImmersiveColorPolicyState)(); // ordinal 104
        typedef  bool (WINAPI* fnIsDarkModeAllowedForWindow)(HWND hWnd); // ordinal 137
        typedef  bool (WINAPI* fnGetIsImmersiveColorUsingHighContrast)(IMMERSIVE_HC_CACHE_MODE mode); // ordinal 106
        typedef  HTHEME(WINAPI* fnOpenNcThemeData)(HWND hWnd, LPCWSTR pszClassList); // ordinal 49
        // 1903 18362
        typedef  bool (WINAPI* fnShouldSystemUseDarkMode)(); // ordinal 138
        typedef  PreferredAppMode(WINAPI* fnSetPreferredAppMode)(PreferredAppMode appMode); // ordinal 135, in 1903
        typedef  bool (WINAPI* fnIsDarkModeAllowedForApp)(); // ordinal 139

        fnRtlGetNtVersionNumbers RtlGetNtVersionNumbers;
        fnSetWindowCompositionAttribute SetWindowCompositionAttribute;
        fnShouldAppsUseDarkMode ShouldAppsUseDarkMode;
        fnAllowDarkModeForWindow AllowDarkModeForWindow;
        fnAllowDarkModeForApp AllowDarkModeForApp;
        fnRefreshImmersiveColorPolicyState RefreshImmersiveColorPolicyState;
        fnIsDarkModeAllowedForWindow IsDarkModeAllowedForWindow;
        fnGetIsImmersiveColorUsingHighContrast GetIsImmersiveColorUsingHighContrast;
        fnOpenNcThemeData OpenNcThemeData;
        fnShouldSystemUseDarkMode ShouldSystemUseDarkMode;
        fnSetPreferredAppMode SetPreferredAppMode;
        fnIsDarkModeAllowedForApp IsDarkModeAllowedForApp;

        HMODULE hUxtheme;
        DWORD major;
        DWORD minor;
        DWORD buildNumber;
        bool isInit;
        bool isOk;
        bool darkModeSupported;
        bool darkModeEnabled;
        bool init()
        {
            if (isInit)
                return isOk;
            isOk = false;
            isInit = true;
            RtlGetNtVersionNumbers = reinterpret_cast<fnRtlGetNtVersionNumbers>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetNtVersionNumbers"));
            if (!RtlGetNtVersionNumbers)
                return false;
            RtlGetNtVersionNumbers(&major, &minor, &buildNumber);
            buildNumber &= ~0xF0000000;

            hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (!hUxtheme)
                return false;

            OpenNcThemeData = reinterpret_cast<fnOpenNcThemeData>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(49)));
            RefreshImmersiveColorPolicyState = reinterpret_cast<fnRefreshImmersiveColorPolicyState>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(104)));
            GetIsImmersiveColorUsingHighContrast = reinterpret_cast<fnGetIsImmersiveColorUsingHighContrast>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(106)));
            ShouldAppsUseDarkMode = reinterpret_cast<fnShouldAppsUseDarkMode>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(132)));
            AllowDarkModeForWindow = reinterpret_cast<fnAllowDarkModeForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));

            auto ord135 = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
            AllowDarkModeForApp = reinterpret_cast<fnAllowDarkModeForApp>(ord135);
            SetPreferredAppMode = reinterpret_cast<fnSetPreferredAppMode>(ord135);

            //_FlushMenuThemes = reinterpret_cast<fnFlushMenuThemes>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136)));
            IsDarkModeAllowedForWindow = reinterpret_cast<fnIsDarkModeAllowedForWindow>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(137)));
            SetWindowCompositionAttribute = reinterpret_cast<fnSetWindowCompositionAttribute>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));

            if (OpenNcThemeData &&
                RefreshImmersiveColorPolicyState &&
                ShouldAppsUseDarkMode &&
                AllowDarkModeForWindow &&
                (AllowDarkModeForApp || SetPreferredAppMode) &&
                //_FlushMenuThemes &&
                IsDarkModeAllowedForWindow)
            {
                isOk = true;
            }
            return isOk;
        }
    };


    static bool init(FUN_STRUCT** ppFun = 0)
    {
        static FUN_STRUCT fun;
        if (ppFun)*ppFun = &fun;
        return fun.init();
    }

    static DWORD GetVersionNumbers(LPDWORD major = 0, LPDWORD minor = 0, LPDWORD build = 0)
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return 0;
        if (major)*major = pFun->major;
        if (minor)*minor = pFun->minor;
        if (build)*build = pFun->buildNumber;
        return pFun->buildNumber;
    }

    static HTHEME WINAPI OpenNcThemeData(HWND hWnd, LPCWSTR pszClassList)
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return 0;
        if (!pFun->OpenNcThemeData)
            return 0;
        return pFun->OpenNcThemeData(hWnd, pszClassList);
    }

    static void WINAPI RefreshImmersiveColorPolicyState()
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return;
        if (!pFun->RefreshImmersiveColorPolicyState)
            return;
        pFun->RefreshImmersiveColorPolicyState();
    }

    static bool WINAPI GetIsImmersiveColorUsingHighContrast(IMMERSIVE_HC_CACHE_MODE mode)
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return false;
        if (!pFun->GetIsImmersiveColorUsingHighContrast)
            return false;
        return pFun->GetIsImmersiveColorUsingHighContrast(mode);
    }

    static bool WINAPI ShouldAppsUseDarkMode()
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return false;
        if (!pFun->ShouldAppsUseDarkMode)
            return false;
        return pFun->ShouldAppsUseDarkMode();
    }

    static bool WINAPI AllowDarkModeForWindow(HWND hWnd, bool allow)
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return false;
        if (!pFun->AllowDarkModeForWindow)
            return false;
        return pFun->AllowDarkModeForWindow(hWnd, allow);
    }

    static bool WINAPI AllowDarkModeForApp(bool allow)
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return false;
        if (!pFun->AllowDarkModeForApp || !pFun->SetPreferredAppMode)
            return false;
        if (pFun->buildNumber < 18362)
            return pFun->AllowDarkModeForApp(allow);
        return pFun->SetPreferredAppMode(allow ? AllowDark : ForceLight);
    }

    static PreferredAppMode WINAPI SetPreferredAppMode(PreferredAppMode appMode)
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return PreferredAppMode::ForceLight;
        if (!pFun->SetPreferredAppMode)
            return PreferredAppMode::ForceLight;
        return pFun->SetPreferredAppMode(appMode);
    }

    static bool WINAPI IsDarkModeAllowedForWindow(HWND hWnd)
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return false;
        if (!pFun->IsDarkModeAllowedForWindow)
            return false;
        return pFun->IsDarkModeAllowedForWindow(hWnd);
    }


    static BOOL WINAPI SetWindowCompositionAttribute(HWND hWnd, WINDOWCOMPOSITIONATTRIBDATA* data)
    {
        FUN_STRUCT* pFun; if (!init(&pFun)) return FALSE;
        if (!pFun->SetWindowCompositionAttribute)
            return FALSE;
        return pFun->SetWindowCompositionAttribute(hWnd, data);
    }



    static bool WINAPI ShouldSystemUseDarkMode()
    {
        typedef  bool (WINAPI* fnShouldSystemUseDarkMode)(); // ordinal 138
        static auto pfn = reinterpret_cast<fnShouldSystemUseDarkMode>(GetThemeAddr(MAKEINTRESOURCEA(138)));
        if (pfn)
            return pfn();
        return false;
    }

    static void WINAPI FlushMenuThemes()
    {
        typedef  void (WINAPI* fnFlushMenuThemes)(); // ordinal 136
        static auto pfn = reinterpret_cast<fnFlushMenuThemes>(GetThemeAddr(MAKEINTRESOURCEA(136)));
        if (pfn)
            pfn();
    }

    static bool WINAPI IsDarkModeAllowedForApp()
    {
        typedef  bool (WINAPI* fnIsDarkModeAllowedForApp)(); // ordinal 139
        static auto pfn = reinterpret_cast<fnIsDarkModeAllowedForApp>(GetThemeAddr(MAKEINTRESOURCEA(139)));
        if (pfn)
            return pfn();
        return false;
    }


};

class IATHOOK
{
public:
    template <typename T, typename T1, typename T2>
    static constexpr T RVA2VA(T1 base, T2 rva)
    {
        return reinterpret_cast<T>(reinterpret_cast<ULONG_PTR>(base) + rva);
    }

    template <typename T>
    static constexpr T DataDirectoryFromModuleBase(void* moduleBase, size_t entryID)
    {
        auto dosHdr = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleBase);
        auto ntHdr = RVA2VA<PIMAGE_NT_HEADERS>(moduleBase, dosHdr->e_lfanew);
        auto dataDir = ntHdr->OptionalHeader.DataDirectory;
        return RVA2VA<T>(moduleBase, dataDir[entryID].VirtualAddress);
    }

    static PIMAGE_THUNK_DATA FindAddressByName(void* moduleBase, PIMAGE_THUNK_DATA impName, PIMAGE_THUNK_DATA impAddr, const char* funcName)
    {
        for (; impName->u1.Ordinal; ++impName, ++impAddr)
        {
            if (IMAGE_SNAP_BY_ORDINAL(impName->u1.Ordinal))
                continue;

            auto import = RVA2VA<PIMAGE_IMPORT_BY_NAME>(moduleBase, impName->u1.AddressOfData);
            if (strcmp(import->Name, funcName) != 0)
                continue;
            return impAddr;
        }
        return nullptr;
    }

    static PIMAGE_THUNK_DATA FindAddressByOrdinal(void* moduleBase, PIMAGE_THUNK_DATA impName, PIMAGE_THUNK_DATA impAddr, uint16_t ordinal)
    {
        for (; impName->u1.Ordinal; ++impName, ++impAddr)
        {
            if (IMAGE_SNAP_BY_ORDINAL(impName->u1.Ordinal) && IMAGE_ORDINAL(impName->u1.Ordinal) == ordinal)
                return impAddr;
        }
        return nullptr;
    }

    static PIMAGE_THUNK_DATA FindIatThunkInModule(void* moduleBase, const char* dllName, const char* funcName)
    {
        auto imports = DataDirectoryFromModuleBase<PIMAGE_IMPORT_DESCRIPTOR>(moduleBase, IMAGE_DIRECTORY_ENTRY_IMPORT);
        for (; imports->Name; ++imports)
        {
            if (_stricmp(RVA2VA<LPCSTR>(moduleBase, imports->Name), dllName) != 0)
                continue;

            auto origThunk = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->OriginalFirstThunk);
            auto thunk = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->FirstThunk);
            return FindAddressByName(moduleBase, origThunk, thunk, funcName);
        }
        return nullptr;
    }

    static PIMAGE_THUNK_DATA FindDelayLoadThunkInModule(void* moduleBase, const char* dllName, const char* funcName)
    {
        auto imports = DataDirectoryFromModuleBase<PIMAGE_DELAYLOAD_DESCRIPTOR>(moduleBase, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
        for (; imports->DllNameRVA; ++imports)
        {
            if (_stricmp(RVA2VA<LPCSTR>(moduleBase, imports->DllNameRVA), dllName) != 0)
                continue;

            auto impName = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->ImportNameTableRVA);
            auto impAddr = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->ImportAddressTableRVA);
            return FindAddressByName(moduleBase, impName, impAddr, funcName);
        }
        return nullptr;
    }

    static PIMAGE_THUNK_DATA FindDelayLoadThunkInModule(void* moduleBase, const char* dllName, uint16_t ordinal)
    {
        auto imports = DataDirectoryFromModuleBase<PIMAGE_DELAYLOAD_DESCRIPTOR>(moduleBase, IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT);
        for (; imports->DllNameRVA; ++imports)
        {
            if (_stricmp(RVA2VA<LPCSTR>(moduleBase, imports->DllNameRVA), dllName) != 0)
                continue;

            auto impName = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->ImportNameTableRVA);
            auto impAddr = RVA2VA<PIMAGE_THUNK_DATA>(moduleBase, imports->ImportAddressTableRVA);
            return FindAddressByOrdinal(moduleBase, impName, impAddr, ordinal);
        }
        return nullptr;
    }

};


#endif // !_UXTHEMEAPI_H_