#if 1
#include "EWindow_Fne_Main.h"
#include <control/ReadColor.h>
#include <elib/fne_decrypt.h>


inline int _idraw_function(int nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case IDC_IDRAW_SET_ETOOLSFUN:
    {
        s_info->pfnETools = (pfn_IDraw_Callback)lParam;
        return 0;
    }
    case IDC_IDRAW_STR2RGB:     // 将一个文本颜色转换成RGB颜色, wParam = 默认颜色, 调用失败返回这个颜色, lParam指向颜色文本指针
    {
        if (!lParam)return wParam;
        return str2rgb((LPCWSTR)lParam, wParam);
    }
    case IDC_IDRAW_STR2ARGB:    // 将一个文本颜色转换成ARGB颜色, wParam = 默认颜色, 调用失败返回这个颜色, lParam指向颜色文本指针
    {
        if (!lParam)return wParam;
        return str2argb((LPCWSTR)lParam, wParam);
    }
    case IDC_IDRAW_RGB2STR:     // 将RGB颜色转换成文本 RGB(1,2,3), wParam=要转换的rgb, lParam = 至少20个WCHAR缓冲区指针, 返回写入字符数
    {
        if (!lParam) return 0;
        const wstr str = rgb2str(wParam);
        const size_t ret = str.size();
        const size_t len = ret * sizeof(wchar_t) + sizeof(wchar_t);
        memcpy((LPWSTR)lParam, str.c_str(), len);
        return (int)ret;
    }
    case IDC_IDRAW_ARGB2STR:    // 将ARGB颜色转换成文本 ARGB(255,1,2,3), wParam=要转换的argb, lParam = 至少26个WCHAR缓冲区指针, 返回写入字符数
    {
        if (!lParam) return 0;
        const wstr str = argb2str(wParam);
        const size_t ret = str.size();
        const size_t len = ret * sizeof(wchar_t) + sizeof(wchar_t);
        memcpy((LPWSTR)lParam, str.c_str(), len);
        return (int)ret;
    }
    case IDC_IDRAW_GETWINDOWDATA:
    {
        if (!lParam || !s_info) return 0;
        return (int)s_info->pfnGetDataFromHwnd((HWND)lParam);
    }
    case IDC_IDRAW_GETCODEVISIBLERECT:  // 获取代码窗口可视的矩形, wParam保留, lParam = 接收矩形的指针(基于易语言主窗口位置)
    case IDC_SCROLL_GETRECT:            // 获取滚动条位置, 可以通过这个事件修改边框达到指定绘画滚动条位置, wParam = pfn_GetScrollRect 回调函数, lParam = 窗口数据指针
    case IDC_SCROLL_CALCRECT:           // 计算滚动条位置, 可以通过这个事件修改窗口非客户区位置, wParam = pfn_CalcScroll 回调函数, lParam = 窗口数据指针
    case IDC_SCROLL_DRAW:               // 绘画滚动条, wParam = pfn_DrawScroll 回调函数, lParam = 窗口数据指针
    case IDC_IDRAW_INLINEHOOK:          // 设置inlineHook, wParam = 原函数, lParam = 新函数
    case IDC_IDRAW_UNINLINEHOOK:        // 取消inlineHook, wParam = 原函数, lParam = 新函数, 返回是否成功
    case IDC_IDRAW_UNINLINEHOOK_PLUGIN: // 取消被释放插件的inlineHook, wParam = 模块起始地址, lParam = 模块结束地址
    case IDC_IDRAW_CREATEPOPUPMENU:     // 创建有自绘样式的弹出式菜单, 所有加入到这个弹出式菜单里的子菜单都会被自绘
    case IDC_EIDE_GETCODEMANAGE:        // 获取IDE代码管理(IEIDE_CodeManage)类指针, 忽略wParam参数, lParma = 代码窗口窗口句柄, 为0则获取当前窗口
    case IDC_EIDE_GETLINESPACING:       // 获取单行间隔, 忽略wParam和 lParam参数
    case IDC_EIDE_SETLINESPACING:       // 暂未实现, 设置单行间隔, 忽略wParam和 lParam参数
    case IDC_EIDE_GETLINEHEIGHT:        // 获取单行高度, 忽略wParam和 lParam参数
    case IDC_EIDE_SETLINEHEIGHT:        // 暂未实现, 设置单行高度, 忽略wParam和 lParam参数
    case IDC_EIDE_GETCODEWINDOW:        // 获取当前代码框句柄, 如果当前是起始页或者设计器窗口则返回0, 忽略wParam和 lParam参数
    case IDC_EIDE_GETECOLOR:            // 获取易语言当前使用的配色, wParam = 缓冲区尺寸, lParam = 缓冲区地址, 返回写入缓冲区字节数, wParam/lParam为0则返回缓冲区需要的尺寸
    case IDC_EIDE_SETECOLOR:            // 设置易语言当前使用的配色, wParam为0则lParam是完整的配色文件路径Unicode版, 不为0则lParam指向配色数据, wParam为数据尺寸, lParam = 根据wParam决定是什么值
    case IDC_IDRAW_SUBWINDOWEX:         // 子类化窗口, 把这个窗口重画成视觉库窗口一样的风格, 只限顶层窗口, wParam保留, lParam = DRAW_WINDOW_ARGUMENT 结构指针, 返回窗口句柄
    case IDC_IDRAW_CREATEWINDOW:        // 创建一个和视觉库窗口一样风格的顶层窗口, wParam= CREATESTRUCTW 结构指针, lParam = DRAW_WINDOW_ARGUMENT 结构指针, 返回窗口句柄
    {
        return s_info->pfnControls(nCode, wParam, lParam);
    }
    default:
        break;
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// 函数功能: 
//      调用视觉库提供的功能
// 
// 参数:
//      参数1: 功能编号, IDC_ 开头常量
//      参数2: 根据功能传递参数
//      参数3: 根据功能传递参数
// 
// 返回值:
//      参考 IDC_ 开头常量的说明
EXTERN_C _declspec(dllexport) int WINAPI iDraw_IDCall(int nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case IDC_ICONTROL_UNLOADLIB:
    {
        bool isOk = false;
        const LPCSTR name = "iDraw";
        if (wParam == 1)
        {
            isOk = fne::addFne(name);
            fne::deleteFne("eTools");
        }
        else if (wParam == 0)
        {
            isOk = fne::deleteFne(name);
        }
        else if (wParam == -1)
        {
            isOk = fne::isFne(name);
        }
        return isOk;
    }
    default:
        break;
    }
    if (nCode >= IDC_IDRAW_BEGIN && nCode <= IDC_IDRAW_END)                 // 主支持库功能
    {
        return _idraw_function(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_ICONTROL_BEGIN && nCode <= IDC_ICONTROL_END)      // 绘画插件
    {
        return s_info->pfnControls(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_ICONFIG_BEGIN && nCode <= IDC_ICONFIG_END)        // 配置插件
    {
        return s_info->pfnConfig(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_ITHEME_BEGIN && nCode <= IDC_ITHEME_END)          // 主题插件
    {
        return s_info->pfnTheme(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_IEVENT_BEGIN && nCode <= IDC_IEVENT_END)          // 事件插件
    {
        return s_info->pfnEvent(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_IRESOURCE_BEGIN && nCode <= IDC_IRESOURCE_END)    // 资源插件
    {
        return s_info->pfnResource(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_IMOVE_BEGIN && nCode <= IDC_IMOVE_END)            // 移动插件
    {
        return s_info->pfnMove(nCode, wParam, lParam);
    }
    else if (nCode >= IDC_IETOOLS_BEGIN && nCode <= IDC_IETOOLS_END)        // 助手插件
    {
        return s_info->pfnETools(nCode, wParam, lParam);
    }
    return s_info->pfnControls(nCode, wParam, lParam);
}

EXTERN_C _declspec(dllexport) int WINAPI iDraw_PluginFunction(int nCode, WPARAM wParam, LPARAM lParam)
{
    return iDraw_IDCall(nCode, wParam, lParam);
}

#endif