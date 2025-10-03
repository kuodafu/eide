#include "EWindow_Fne_Header.h"
#include <hook_detours/apiHook.h>
#include "EWindow.h"
#include <hook_detours/apiHook.h>
#include "EMenu.h"



static apiHook hook_LoadStringA;
static apiHook hook_LoadStringW;
static BOOL(WINAPI* LoadStringA_hook)(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax) = LoadStringA;
static BOOL(WINAPI* LoadStringW_hook)(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax) = LoadStringW;
static std::unordered_set<PFN_LoadStringCallback> m_set;

// 加载不是默认的字符串, ID是经过处理的, 是菜单项的ID
int LoadString_Other(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax)
{

    switch (uID)
    {
    case ID_MENU_CLOSE:   // 关闭
        wcscpy_s(lpBuffer, cchBufferMax, L"关闭当前选中的窗口\n关闭窗口");
        break;
    case ID_MENU_CLOSEOTHER:   // 关闭其他
        wcscpy_s(lpBuffer, cchBufferMax, L"关闭非当前窗口\n关闭其他");
        break;
    case ID_MENU_CLOSEALL:   // 关闭全部
        wcscpy_s(lpBuffer, cchBufferMax, L"关闭全部窗口\n关闭全部");
        break;
    case ID_MENU_CLOSE_LEFT:   // 关闭左边
        wcscpy_s(lpBuffer, cchBufferMax, L"关闭左边所有\n关闭左边");
        break;
    case ID_MENU_CLOSE_RIGHT:   // 关闭右边
        wcscpy_s(lpBuffer, cchBufferMax, L"关闭右边所有\n关闭右边");
        break;
    case ID_MENU_CLOSE_TOP:   // 顶部显示
        wcscpy_s(lpBuffer, cchBufferMax, L"代码切换选择夹在顶部显示\n顶部显示");
        break;
    case ID_MENU_CLOSE_BOTTOM:   // 底部显示
        wcscpy_s(lpBuffer, cchBufferMax, L"代码切换选择夹在底部显示\n底部显示");
        break;
    case IDMENU_LOCATIONTOOLBAR:        // 显示/隐藏定位工具条
        return LoadStringW_hook(hInstance, 135, lpBuffer, cchBufferMax);    // 其他没有处理的都调用原函数处理
        break;


    default:
        return 0;
    }
    return (int)wcslen(lpBuffer);
}

static BOOL WINAPI MyLoadStringW_hook(HINSTANCE hInstance, UINT uID, LPWSTR lpBuffer, int cchBufferMax)
{
    int ret = 0;
    switch (uID)
    {
    case 32853:     // 单步
        wcscpy_s(lpBuffer, cchBufferMax, L"在程序现行运行位置单步执行一行程序，如果此程序行调用了子程序，系统不会跟踪到该子程序中去。\n单步(F8/F10)");
        break;
    case 32852:     // 单步进入
        wcscpy_s(lpBuffer, cchBufferMax, L"在程序现行运行位置单步执行一行程序，如果此程序行调用了子程序，则跟踪进入子程序。\n单步进入(Shift+F8/F11)");
        break;
    case 32867:     // 跟踪返回
        wcscpy_s(lpBuffer, cchBufferMax, L"在上级子程序调用现行子程序的语句后中断。\n跳出当前子程序(Ctrl+Shift+F8/Shift+F11)");
        break;
    case 57600:     // 新建源码
        wcscpy_s(lpBuffer, cchBufferMax, L"新建源码\n建立新程序(Shift+Ins)");
        break;
    case 32934:     // 整体搜寻
        wcscpy_s(lpBuffer, cchBufferMax, L"在程序中寻找指定文本并列出所有找到的项目\n整体搜寻(Ctrl+Shift+F)");
        break;
    case 32854:     // 运行
        wcscpy_s(lpBuffer, cchBufferMax, L"编译现行易程序的调试版本.EXE文件后立即运行\n运行(F5)");
        break;
    case 32860:     // 停止
        wcscpy_s(lpBuffer, cchBufferMax, L"终止现行易程序的运行\n停止调试(Shift+F5)");
        break;
    case 32905:     // 后退
        wcscpy_s(lpBuffer, cchBufferMax, L"跳回到先前跳转时的程序位置\n跳回先前位置(Ctrl+J/Ctrl+Shift+Backspace)"); //(Ctrl+Shift+Backspace)
        break;
    case 32906:     // 前进
        wcscpy_s(lpBuffer, cchBufferMax, L"跳转到当前光标所处符号的定义位置\n跳转到定义处(Ctrl+B/Ctrl+单击/Ctrl+Shift+Alt+Backspace)"); //(Ctrl+Shift+Alt+Backspace)
        break;
    case 32917:     // "&C.编译\tF7", 32851
        wcscpy_s(lpBuffer, cchBufferMax, L"编译现行易程序的最终发布版本，创建对应的.EXE可执行程序文件\n编译(F7)");
        break;
    case 32894:     // "&S.静态编译\tShift+F7", 32912
        wcscpy_s(lpBuffer, cchBufferMax, L"编译出静态可执行EXE文件，该文件不依赖任何易语言系统文件，可以在未安装易语言系统的电脑上运行。\n静态编译(Shift+F7)");
        break;
    case 32884: // "&B.黑月静态编译", 32912
        wcscpy_s(lpBuffer, cchBufferMax, L"黑月静态编译\n黑月静态编译");
        break;
    case 32882:      // "&D.独立编译", 32911
        wcscpy_s(lpBuffer, cchBufferMax, L"编译出独立可执行EXE文件，该文件不依赖任何易语言系统文件，可以在未安装易语言系统的电脑上运行。\n独立编译");
        break;
    case 32915:         // 重新运行
        wcscpy_s(lpBuffer, cchBufferMax, L"停止当前调试的程序并重新编译运行\n重新运行(Ctrl+Shift+F5)");
        break;
    case 32914:    // 标准工具条 -> 下一个书签
        wcscpy_s(lpBuffer, cchBufferMax, L"跳转到后面的最近书签位置\n下一个书签");
        break;
    case 32913:    // 标准工具条 -> 上一个书签
        wcscpy_s(lpBuffer, cchBufferMax, L"跳转到前面的最近书签位置\n上一个书签");
        break;
    case 32908:     // 标准工具条 -> 设置书签
        wcscpy_s(lpBuffer, cchBufferMax, L"设置或取消当前位置处的书签标志\n设置书签");
        break;
    case 32897:     // 标准工具条 -> 删除书签
        wcscpy_s(lpBuffer, cchBufferMax, L"删除当前位置处的书签标志\n删除书签");
        break;
    case 32892:       // 标准工具条 -> 注释
        wcscpy_s(lpBuffer, cchBufferMax, L"屏蔽当前所选中的代码块\n注释");
        break;
    case 32898:     // 标准工具条 -> 取消注释 
        wcscpy_s(lpBuffer, cchBufferMax, L"解除屏蔽当前所选中的代码块\n取消注释");
        break;
    default:
    {
        UINT lowID = uID;
        if (s_info->menuSelHmenu)
        {
            uID = s_info->menuSelId;
            int retLen = LoadString_Other(hInstance, uID, lpBuffer, cchBufferMax);
            if (retLen > 0)
                return retLen;

            LPCWSTR text = _menu_istool_(uID);
            if (text)
            {
                ret = wcslen(text);
                if (ret >= cchBufferMax)
                    ret = cchBufferMax - 1;
                wmemcpy(lpBuffer, text, ret);
                lpBuffer[ret] = 0;
                break;
            }

            if (uID >= IDMENU_SWITCH_THEME && uID < IDMENU_SWITCH_THEME_END)
                return ThemeMenu_LoadString(uID, lpBuffer, cchBufferMax);

        }
        //wstr::dbg(L"加载字符串, 获取到的菜单索引 = %d, ID = 0x%X(0x%04X), 句柄 = 0x%p\n", s_info->menuSelIndex, s_info->menuSelId, uID, s_info->menuSelHmenu);

        for (auto& item : m_set)
        {
            if (item)
            {
                int retLen = item(uID, lpBuffer, cchBufferMax);
                if (retLen > 0)
                    return retLen;
            }
        }

        return LoadStringW_hook(hInstance, lowID, lpBuffer, cchBufferMax);    // 其他没有处理的都调用原函数处理
    }
    }
    if (ret == 0)
        ret = (int)wcslen(lpBuffer);
    return ret;
}
static BOOL WINAPI MyLoadStringA_hook(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
    wstr text(cchBufferMax);
    int len = MyLoadStringW_hook(hInstance, uID, text.data(), cchBufferMax);
    text.resize(len);
    _str a = text.w2a();
    int ret = (int)a.size();
    if (ret >= cchBufferMax)
        ret = cchBufferMax - 1;

    memcpy(lpBuffer, a.c_str(), ret);
    lpBuffer[ret] = 0;
    return (int)strlen(lpBuffer);
}

// 初始化hook加载字符串的函数
void HookLoadStringInit()
{
    hook_LoadStringA.hook(&(PVOID&)LoadStringA_hook, MyLoadStringA_hook);
    hook_LoadStringW.hook(&(PVOID&)LoadStringW_hook, MyLoadStringW_hook);
}

void HookLoadString_PushCallback(PFN_LoadStringCallback pfn)
{
    m_set.insert(pfn);
}
