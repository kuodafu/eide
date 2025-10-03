// dllmain.cpp : 定义 DLL 应用程序的入口点。
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
#include <iDraw_virtual.h>
#include "EWindow_Resource.h"
#include <iDraw_Version.h>
static PLUGIN_INFO pluginInfo;
static HINSTANCE m_hInst;
static EWindowResource m_Resource;
IDRAW_INFO* s_info;     // 包含各个窗口的信息, 支持库和其他动态库都有使用, 支持库调用动态库的时候会传递这个指针过去

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        m_hInst = hModule;
        break;
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
    const int a = WM_VSCROLL;
    return 0;
}


// 支持库调用, 会把窗口信息传递过来
extern "C" __declspec(dllexport) void* CALLBACK IDraw_Interface(IDRAW_INFO * info, IGetEWindow* eWnd, int reserved1, int reserved2)
{
    s_info = info;
    m_Resource.SetModule(m_hInst);

    // 手动生成默认主题集合, 这里的代码不要删, 发布时设置为0
    if (0)
    {
        CFileRW DefaultTheme(LR"(D:\e\lib\iDraw\data\DefaultTheme.ini)");
        CFileRW WhiteTheme(LR"(D:\e\lib\iDraw\data\WhiteTheme.ini)");
        CFileRW VS2019Theme(LR"(D:\e\lib\iDraw\data\VS2019Theme.ini)");
        CFileRW DefaultTheme_clr(LR"(D:\e\clr\vs2019.clr)");
        CFileRW WhiteTheme_clr(LR"(D:\e\clr\白色主题.clr)");
        CFileRW VS2019Theme_clr(LR"(D:\e\clr\VS2019Theme.clr)");

        int size = DefaultTheme.size() + WhiteTheme.size() + VS2019Theme.size()
            + DefaultTheme_clr.size() + WhiteTheme_clr.size() + VS2019Theme_clr.size()
            + 6 * 8;

        char* p = new char[size] {0};
        char* ptr = p;
        auto pfn = [](CFileRW& f, char*& p)
        {
            LPBYTE data = 0;
            int size1 = f.size();
            f.read(&data, size1);
            memcpy(p, &size1, sizeof(int)); p += sizeof(int);
            memcpy(p, data, size1); p += size1;
        };

        pfn(DefaultTheme, p);
        pfn(DefaultTheme_clr, p);

        pfn(WhiteTheme, p);
        pfn(WhiteTheme_clr, p);

        pfn(VS2019Theme, p);
        pfn(VS2019Theme_clr, p);

        CFileRW f;
        f.open(LR"(D:\cpp\SVN工作目录\易语言界面\IDraw_Resource\src\默认主题数据集合.bin)", 4, -1);
        f.write((LPBYTE)ptr, size);
        delete[] ptr;
        __debugbreak();
    }
    info->pResource = &m_Resource;
    pluginInfo.cbSize           = sizeof(pluginInfo);
    pluginInfo.name             = "核心资源插件";
    pluginInfo.remarks          = "所有资源都由这个插件处理, 包括绘画使用到的各种图标";
    pluginInfo.version          = IDRAW_VERSIONA;
    pluginInfo.author           = "精易论坛";
    pluginInfo.Versionlimit     = 0;
    pluginInfo.Dependence       = 0;


    pluginInfo.guid                 = "{05A57CE4-A1A4-473D-0003-F90CAF05FE4A}";
    pluginInfo.pfnMenuExt_Command   = 0;
    pluginInfo.pfnCallback          = IDraw_NotifyCallback;

    return &pluginInfo;
}

// 插件被卸载, 需要把所有注册的事件/消息给释放
extern "C" __declspec(dllexport) void CALLBACK IDraw_UnInterface()
{

}
