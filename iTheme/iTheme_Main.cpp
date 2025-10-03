// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "iTheme.h"
#include <control/ReadColor.h>
#include <iDraw_Version.h>
#include <iDraw_const.h>

static PLUGIN_INFO pluginInfo;

IDRAW_INFO* s_info;

static OBJSTRUCT m_defData;         // 默认的配置, 默认配色


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}




// 与支持库交互的回调函数
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case INE_CONFIG_CHANGED:    // 配置被改变
    {
        if (lParam != (LPARAM)s_info) break;
        ReloadDefColor();
        break;
    }
    default:
        break;
    }
    return 0;
}



// 支持库调用, 会把窗口信息传递过来
extern "C" __declspec(dllexport) void* CALLBACK IDraw_Interface(IDRAW_INFO * info, IGetEWindow * eWnd, int reserved1, int reserved2)
{
    s_info = info;
    s_info->defData = &m_defData;
    //TODO 这些应该弄一个新的插件来处理
    s_info->pfnCreateBrush      = Brush_Create;
    s_info->pfnCreatePen        = Pen_Create;
    s_info->pfnCreateBrushDx    = Brush_CreateDx;
    s_info->pfnCreatePenDx      = Pen_CreateDx;
    s_info->pfnCreateFontDx     = Font_CreateDx;
    s_info->pfnCreateFontLfDx   = Font_CreateLfDx;
    s_info->pfnCreateFontDpi    = Font_CreateDpi;
    s_info->pfnGetDefData       = GetDefData;
    s_info->pfnReloadDxResource = ReloadDxResource;

    iTheme_Init_Window();
    iTheme_Init_CustomCodeTab();



    pluginInfo.cbSize           = sizeof(pluginInfo);
    pluginInfo.name             = "核心主题插件";
    pluginInfo.remarks          = "所有主题配置操作都由这个插件完成";
    pluginInfo.version          = IDRAW_VERSIONA;
    pluginInfo.author           = "精易论坛";
    pluginInfo.Versionlimit     = 0;
    pluginInfo.Dependence       = 0;


    pluginInfo.guid                 = "{05A57CE4-A1A4-473D-0001-F90CAF05FE4A}";
    pluginInfo.pfnMenuExt_Command   = 0;
    pluginInfo.pfnCallback          = IDraw_NotifyCallback;

    return &pluginInfo;
}

// 插件被卸载, 需要把所有注册的事件/消息给释放
extern "C" __declspec(dllexport) void CALLBACK IDraw_UnInterface()
{
    DeleteDefaultObject();

}
