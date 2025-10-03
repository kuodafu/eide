// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "Commdlg.h"

HINSTANCE g_hInst;   // 当前插件的模块句柄
// 与支持库交互的回调函数
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

typedef BOOL(APIENTRY* PFN_ChooseColorW)(LPCHOOSECOLORW p);
typedef BOOL(APIENTRY* PFN_ChooseColorA)(LPCHOOSECOLORA p);
PFN_ChooseColorW pChooseColorW;
PFN_ChooseColorA pChooseColorA;
BOOL APIENTRY PFN_CALLBACK_ChooseColorW(LPCHOOSECOLORW p)
{

    return pChooseColorW(p);
}
BOOL APIENTRY PFN_CALLBACK_ChooseColorA(LPCHOOSECOLORA p)
{
    p->Flags = (CC_ANYCOLOR | CC_ENABLEHOOK | CC_FULLOPEN | CC_RGBINIT);
    p->rgbResult = 255;
    return pChooseColorA(p);
}

extern "C" _declspec(dllexport) void* IDRAW_CALL IDraw_Interface(LPIDRAW_INFO pInfo, IGetEWindow * eWnd, int reserved1, int reserved2)
{
    static PLUGIN_INFO pluginInfo = { 0 };

    pluginInfo.cbSize = sizeof(pluginInfo);
    pluginInfo.name = "插件框架 c++版";
    pluginInfo.remarks = "演示用, 插件框架";
    pluginInfo.version = "1.0";
    pluginInfo.author = "项目部006";
    pluginInfo.Versionlimit = 0;
    pluginInfo.Dependence = 0;


    pluginInfo.guid = DLL_GUID;
    pluginInfo.pfnMenuExt_Command = 0;
    pluginInfo.pfnCallback = IDraw_NotifyCallback;
    iDraw_InitPlugin(pluginInfo.guid, g_hInst, &pluginInfo, pluginInfo.pfnMenuExt_Command, pluginInfo.pfnCallback); // 初始化插件

    iDraw_Tool_InsertButton(DLL_GUID, 1, L"1", 0, 0, 0, 0, 0);

    pChooseColorA = (PFN_ChooseColorA)iDraw_IDCall(IDC_IDRAW_INLINEHOOK, (WPARAM)ChooseColorA, (LPARAM)PFN_CALLBACK_ChooseColorA);
    //pChooseColorW = (PFN_ChooseColorW)iDraw_IDCall(IDC_IDRAW_INLINEHOOK, (WPARAM)ChooseColorW, (LPARAM)PFN_CALLBACK_ChooseColorW);

    //MessageBoxW(0, L"插件框架 c++版 插件被加载", L"加载了", MB_OK);

    return &pluginInfo;
}

extern "C" __declspec(dllexport) void IDRAW_CALL IDraw_UnInterface()
{

}


// 与支持库交互的回调函数
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case INE_TOOL_BUTTONCLICK:
    {
        iDraw_IDCall(IDC_IDRAW_UNINLINEHOOK, (WPARAM)pChooseColorA, (LPARAM)PFN_CALLBACK_ChooseColorA);

        break;
    }
    default:
        break;
    }
    return 0;
}