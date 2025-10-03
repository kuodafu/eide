#include "../EWindow.h"
#include <shlwapi.h>


#include "../EMenu.h"
#include <elib/fne_decrypt.h>
static std::unordered_set<PFN_EMenuCommand> m_set_menu_command;     // 菜单事件回调

void EMenu_PushCommand(PFN_EMenuCommand pfn)
{
    m_set_menu_command.insert(pfn);
}



inline LRESULT _ewindow_menu_command(WPARAM wParam, HWND hControl, LPOBJSTRUCT pData)
{
    LPCWSTR toolsFileName = _menu_istool_(wParam);
    if (toolsFileName)
    {
        if (PathFileExistsW(toolsFileName))
        {
            ShellExecuteW(0, L"open", toolsFileName, 0, 0, SW_SHOW);
            //process_create(path.c_str(), L"", 0);
        }
        return 0;
    }
    LPMENUEXT_ID_INFO userID = _menu_isext_(wParam);
    if (userID)
    {
        MenuExt_Command(userID);
        return 0;
    }
    DWORD id = (DWORD)wParam;
    if (id >= IDMENU_SWITCH_THEME && id <= IDMENU_SWITCH_THEME_END)
    {
        return EWndProc_OnThemeCommand(pData->hWnd, WM_COMMAND, (WPARAM)wParam, (LPARAM)hControl, pData);
    }
    switch (id)
    {
    case IDMENU_UNINSERT:
    {
        int ret = MessageBoxW(s_info->hEWnd,
            L"停用支持库已经放到设置程序里, 需要打开设置程序停用" IDRAW_FNE_NAME "\n"
            L"    停用前需要先关闭所有易语言, 否则设置后会被最后一个关闭的易语言覆盖支持库启用信息\n\n"
            L"是否打开设置程序?"
            , L"停用" IDRAW_FNE_NAME, MB_YESNO | MB_ICONINFORMATION);
        if (ret == IDYES)
        {
            SendMessageW(s_info->hEWnd, WM_SYSCOMMAND, SC_SETTING, 0);
        }
        break;
    }
    case IDMENU_AD:
    {
        // 调试用, 把一些状态记录起来, 用一个窗口显示
        DWORD style = WS_SYSMENU | WS_CAPTION | WS_CLIPSIBLINGS | WS_VISIBLE | WS_THICKFRAME;
        DWORD styleEx = WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT;
        DialogBoxPop(styleEx, L"扩大福的广告", style, 0, 0, 0, 0, s_info->hEWnd, GetModuleHandleW(0), [](HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) -> INT_PTR
        {
            UNREFERENCED_PARAMETER(lParam);
            switch (message)
            {
            case WM_INITDIALOG:
            {
                static HFONT hFont = CWndBase::CreateFontW(L"微软雅黑", s_info->scale(-15));
                const int style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VSCROLL | WS_VISIBLE | WS_CHILD | ES_MULTILINE;
                const int styleEx = WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY;
                HWND hEdit = CreateWindowExW(styleEx, L"edit", 0, style, 0,0,0,0, hDlg, (HMENU)(LONG_PTR)10086, 0, 0);
                SetWindowFont(hEdit, hFont, true);

                const LPCWSTR pszAd =
                    L"当前广告发布时间: 2024-06-04, 最新广告信息请查看网页 www.kuodafu.com\r\n"
                    L"出售一些软件源码:\r\n"
                    L"    扩大福窗口探测, 5000, c++完整源码, 包含驱动源码\r\n"
                    L"    成品测试 https://github.com/121007124/spy\r\n"
                    L"        精易论坛帖子: https://bbs.125.la/thread-14814765-1-1.html\r\n"
                    L"\r\n"
                    L"    扩大福进程守护, 无限制成品300, 完整c++源码600\r\n"
                    L"        成品测试 https://bbs.125.la/thread-14814763-1-1.html\r\n"
                    L"\r\n"
                    L"    扩大福驱动, 成品400, 完整c语言源码1000\r\n"
                    L"        成品测试 https://bbs.125.la/thread-14789979-1-1.html\r\n"
                    L"        目前正在入门驱动, 这个驱动会慢慢完善, 2025年年底之前购买的都提供更新服务\r\n"
                    L"        目前驱动没有任何限制, 没有联网操作\r\n"
                    L"        2026年之前都不会加入限制, 2026年之前的驱动都永久使用\r\n"
                    L"        价格会随着驱动功能的完善增加, 近期的小目标, 把驱动基础的功能完善\r\n"
                    L"        更大一点的小目标: 过微软PatchGuard, 实现hook SSDT函数\r\n"
                    L"        终极目标: VT虚拟化\r\n"
                    L"\r\n"
                    L"接单 定制支持库, 定制易语言组件, 把c++开源库封装成易语言支持库\r\n"
                    L"\r\n"
                    L"  有需要请联系 QQ: 121007124\r\n"
                    L"\r\n"
                    ;
                SetWindowTextW(hEdit, pszAd);
                SetFocus(hEdit);
                // 635 422
                const int width = 635;
                const int heigth = 452;
                RECT rc;
                GetWindowRect(s_info->hEWnd, &rc);
                const int x = rc.left + (rc.right - rc.left - width) / 2;
                const int y = rc.top + (rc.bottom - rc.top - heigth) / 2;
                SetWindowPos(hDlg, 0, x, y, width, heigth, SWP_NOZORDER | SWP_NOACTIVATE);
                return (INT_PTR)FALSE;
            }
            case WM_SIZE:
            {
                const int cx = LOWORD(lParam);
                const int cy = HIWORD(lParam);
                HWND hEdit = GetDlgItem(hDlg, 10086);
                MoveWindow(hEdit, 10, 10, cx - 20, cy - 20, TRUE);
                return (INT_PTR)FALSE;
            }
            case WM_CLOSE:
                EndDialog(hDlg, 0);
                DestroyWindow(hDlg);
                break;
            default:
                return (INT_PTR)FALSE;
            }
            return (INT_PTR)TRUE;
        }
        , 0);
        break;
    }
    default:
        return WndProc_CallWindowProc(pData->hWnd, WM_COMMAND, (WPARAM)wParam, (LPARAM)hControl, pData);
    }
    return 0;


}

LRESULT EWindow_OnCommand(HWND hWnd, int id, int code, HWND hControl, LPOBJSTRUCT pData)
{
    WPARAM wParam = (WPARAM)MAKELONG(id, code);
    for (auto item : m_set_menu_command)
    {
        if (item && item((int)wParam))  // 优先先处理回调里的事件, 然后再处理自己的事件, 这样回调可以拦截掉一些事件
            return 0;
    }
    wstr::dbg(__FILEW__ L" - " __FUNCTIONW__ L" - " L"窗口事件 id=%d, wParam = 0x%p\n", id, wParam);

    switch (id)
    {
    case 129: case 130: case 132:
        return 0;
    case 57670:     // F1按下触发提示
    {
        std::vector<int>* params = prop_GetTabParams();
        if (!params)
        {
            wstr dbg;
            dbg.Format(L"程序遇到致命错误, 行号 = %d, 函数 = %s, 文件 = %s\n", __LINE__, __FUNCTIONW__, __FILEW__);
            MessageBoxW(0, dbg.c_str(), L"错误", 0);
            throw;
        }
        HWND hTabTips = 0;  // 存放提示的选择夹
        int indexTips = 0;  // 存放提示的选择夹子夹索引

        const bool isRight = TabCode_IsRightTab(TAB_PARAM_TIPS);
        hTabTips = isRight ? s_info->hTabStateRight : s_info->hTabStateLeft;
        int len = TabCtrl_GetItemCount(hTabTips);
        TCITEMW item = { 0 };
        for (int i = 0; i < len; i++)
        {
            item.mask = TCIF_PARAM;
            TabCtrl_GetItem(hTabTips, i, &item);
            if (item.lParam == TAB_PARAM_TIPS)
            {
                indexTips = i;
                break;
            }
        }

        int index = TabCtrl_GetCurSel(hTabTips);
        if (indexTips != index) // 切换到提示子夹
        {
            TabCtrl_SetCurSel(hTabTips, indexTips);
            NMHDR hdr = { 0 };
            hdr.hwndFrom = hTabTips;
            hdr.code = TCN_SELCHANGE;
            hdr.idFrom = 0;
            SendMessageW(s_info->eWnd->GetState32770(), WM_NOTIFY, 0, (LPARAM)&hdr);
        }
        return WndProc_CallWindowProc(pData->hWnd, WM_COMMAND, (WPARAM)wParam, (LPARAM)hControl, pData);
    }
    case 59393:     // 显示隐藏状态条
    {
        LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, WM_COMMAND, wParam, (LPARAM)hControl);
        SendEWindowSize(pData);
        RECT rc;
        GetClientRect(hWnd, &rc);
        rc.top = rc.bottom - WINDOW_STATUS_HEIGHT * 2;
        InvalidateRect(hWnd, &rc, 0);
        return ret;
    }
    case 32851:
    case 32912:
    {
        LRESULT ret = CallWindowProcA(pData->oldProc, hWnd, WM_COMMAND, MAKELONG(id, code), (LPARAM)hControl);
        if (s_info->config->IsRunSave())
            CallWindowProcA(pData->oldProc, hWnd, WM_COMMAND, 57603, 0);  // 57603 是保存源码菜单事件
        return ret;
    }
    default:
        return _ewindow_menu_command(wParam, hControl, pData);
    }
    return 0;
}
