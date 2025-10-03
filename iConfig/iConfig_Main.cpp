// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "iConfig.h"
#include <iDraw_Version.h>
#include <iDraw_const.h>
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

static PLUGIN_INFO pluginInfo;


IDRAW_INFO* s_info;
static HMODULE m_hModule;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        m_hModule = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

struct _config_timer_id
{
    int size;
    LPBYTE p;
};
static _config_timer_id m_config_timer;
inline DWORD CALLBACK _config_init_thread(LPVOID pArg)
{
    while (1)
    {
        if (s_info && s_info->pfnControls)
        {
            s_info->pfnControls(IDC_EIDE_SETECOLOR, m_config_timer.size, (LPARAM)m_config_timer.p);
            break;
        }
        Sleep(200);
    }

    delete[] m_config_timer.p;
    m_config_timer.p = 0;
    m_config_timer.size = 0;
    return 0;
}

inline void _iConfig_ModifyEColor()
{
    LPCWSTR clr = s_info->themeColor->clr();    // 重新设置易语言配色
    if (!clr || !clr[0]) return;

    wstr file(260);
    file.assign(s_info->path_e).append(L"clr\\").append(clr);
    if (!PathFileExistsW(file.c_str())) return;


    CFileRW f;
    if (!f.open(file.c_str())) return;

    int size = f.size();
    if (size > 0)
    {
        LPBYTE p = new BYTE[size];
        f.read(p, size);
        if (!s_info->pfnControls && m_config_timer.p == 0)
        {
            // 如果绘画插件还没有初始化, 那就创建个时钟检测是否已经初始化
            m_config_timer.p = p;
            m_config_timer.size = size;
            HANDLE hThread = CreateThread(0, 0, _config_init_thread, 0, 0, 0);
            if (hThread)
                CloseHandle(hThread);
            return;
        }
        if (s_info && s_info->pfnControls)
        {
            // 走到这绘画插件已经初始化了, 直接设置颜色
            s_info->pfnControls(IDC_EIDE_SETECOLOR, size, (LPARAM)p);
        }
        delete[] p;
    }
}

// 初始化几个xml文件, 有就加载, 没有就创建
// 会初始化config.xml主配置文件
// 在从主配置文件中读出主题文件, 在加载主题文件
// 函数内部会初始化主题数据, 有就读, 没有就创建, 其他配置由其他函数处理
inline void iConfig_Init()
{
    s_info->ini->open(s_info->path_config);
    wstr ThemeName = s_info->ini->read(L"Theme", L"name", L"黑色主题");
    wstr ThemeFile = s_info->ini->read(L"Theme", ThemeName.c_str(), L"DefaultTheme.ini");
    if (ThemeFile.empty())
    {
        ThemeFile = L"DefaultTheme.ini";
    }

    wstr theme(260);
    theme.assign(s_info->path_data).append(ThemeFile);
    s_info->iniColor->open(theme.c_str());
    _window_get_config();   // 重新读取颜色配置

    _iConfig_ModifyEColor();

}

// 与支持库交互的回调函数
int CALLBACK IDraw_NotifyCallback(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case INE_CONFIG_CHANGED:    // 配置改变
    {
        LPOBJSTRUCT pData = s_info->pfnGetDataFromHwnd(s_info->hEWnd);
        iConfig_Init();         // 重新读取配置信息, 会根据主题记录的项名读取主题配置文件
        
        s_info->pfnTheme(nCode, wParam, (LPARAM)s_info);    // 通知主题插件重新加载默认配色, 内部使用
        s_info->pfnControls(nCode, wParam, (LPARAM)s_info); // 通知绘画插件重新插件绘画对象, 内部使用

        LPPLUGIN_INFO pInfo[EVENT_MAX_ITEM] = { 0 };
        int count = s_info->pfnPlugin_Enum(pInfo, EVENT_MAX_ITEM);

        // 这里面的插件都不通知配置被改变事件
        const pfn_IDraw_Callback arr[] =
        {
            s_info->pfnConfig,
            s_info->pfnTheme,
            s_info->pfnEvent,
            s_info->pfnControls,
            s_info->pfnResource,
            s_info->pfnMove,
            //s_info->pfnETools,
        };

        for (int i = 0; i < count; i++)
        {
            if (i >= EVENT_MAX_ITEM) break;
            LPPLUGIN_INFO info = pInfo[i];
            if (!info || !info->pfnCallback)continue;
            bool isContinue = false;
            for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
            {
                if (info->pfnCallback == arr[i])
                {
                    isContinue = true;
                    break;
                }
            }
            if (isContinue) continue;
            info->pfnCallback(INE_CONFIG_CHANGED, 0, 0);
        }
        SendEWindowSize(pData);
        break;
    }
    default:
        break;
    }
    return 0;
}

// 支持库调用, 会把窗口信息传递过来
extern "C" __declspec(dllexport) void* CALLBACK IDraw_Interface(IDRAW_INFO* info, IGetEWindow * eWnd, int reserved1, int reserved2)
{
    s_info = info;
    iConfig_Init();
    s_info->pfnInfoChanged = iConfig_InfoChanged;

    pluginInfo.cbSize = sizeof(pluginInfo);
    pluginInfo.name         = "核心配置插件";
    pluginInfo.remarks      = "所有配置操作都由这个插件完成";
    pluginInfo.version      = IDRAW_VERSIONA;
    pluginInfo.author       = "精易论坛";
    pluginInfo.Versionlimit = 0;
    pluginInfo.Dependence   = 0;


    pluginInfo.guid                 = "{05A57CE4-A1A4-473D-0000-F90CAF05FE4A}";
    pluginInfo.pfnMenuExt_Command   = 0;
    pluginInfo.pfnCallback          = IDraw_NotifyCallback;

    return &pluginInfo;
}

// 插件被卸载, 需要把所有注册的事件/消息给释放
extern "C" __declspec(dllexport) void CALLBACK IDraw_UnInterface()
{

}