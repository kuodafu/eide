#include "../EWindow.h"
#include <shlwapi.h>


#include "../EMenu.h"
#include <elib/fne_decrypt.h>
static std::unordered_set<PFN_EMenuCommand> m_set_menu_command;     // �˵��¼��ص�

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
            L"ͣ��֧�ֿ��Ѿ��ŵ����ó�����, ��Ҫ�����ó���ͣ��" IDRAW_FNE_NAME "\n"
            L"    ͣ��ǰ��Ҫ�ȹر�����������, �������ú�ᱻ���һ���رյ������Ը���֧�ֿ�������Ϣ\n\n"
            L"�Ƿ�����ó���?"
            , L"ͣ��" IDRAW_FNE_NAME, MB_YESNO | MB_ICONINFORMATION);
        if (ret == IDYES)
        {
            SendMessageW(s_info->hEWnd, WM_SYSCOMMAND, SC_SETTING, 0);
        }
        break;
    }
    case IDMENU_AD:
    {
        // ������, ��һЩ״̬��¼����, ��һ��������ʾ
        DWORD style = WS_SYSMENU | WS_CAPTION | WS_CLIPSIBLINGS | WS_VISIBLE | WS_THICKFRAME;
        DWORD styleEx = WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT;
        DialogBoxPop(styleEx, L"���󸣵Ĺ��", style, 0, 0, 0, 0, s_info->hEWnd, GetModuleHandleW(0), [](HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) -> INT_PTR
        {
            UNREFERENCED_PARAMETER(lParam);
            switch (message)
            {
            case WM_INITDIALOG:
            {
                static HFONT hFont = CWndBase::CreateFontW(L"΢���ź�", s_info->scale(-15));
                const int style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VSCROLL | WS_VISIBLE | WS_CHILD | ES_MULTILINE;
                const int styleEx = WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY;
                HWND hEdit = CreateWindowExW(styleEx, L"edit", 0, style, 0,0,0,0, hDlg, (HMENU)(LONG_PTR)10086, 0, 0);
                SetWindowFont(hEdit, hFont, true);

                const LPCWSTR pszAd =
                    L"��ǰ��淢��ʱ��: 2024-06-04, ���¹����Ϣ��鿴��ҳ www.kuodafu.com\r\n"
                    L"����һЩ���Դ��:\r\n"
                    L"    ���󸣴���̽��, 5000, c++����Դ��, ��������Դ��\r\n"
                    L"    ��Ʒ���� https://github.com/121007124/spy\r\n"
                    L"        ������̳����: https://bbs.125.la/thread-14814765-1-1.html\r\n"
                    L"\r\n"
                    L"    ���󸣽����ػ�, �����Ƴ�Ʒ300, ����c++Դ��600\r\n"
                    L"        ��Ʒ���� https://bbs.125.la/thread-14814763-1-1.html\r\n"
                    L"\r\n"
                    L"    ��������, ��Ʒ400, ����c����Դ��1000\r\n"
                    L"        ��Ʒ���� https://bbs.125.la/thread-14789979-1-1.html\r\n"
                    L"        Ŀǰ������������, �����������������, 2025�����֮ǰ����Ķ��ṩ���·���\r\n"
                    L"        Ŀǰ����û���κ�����, û����������\r\n"
                    L"        2026��֮ǰ�������������, 2026��֮ǰ������������ʹ��\r\n"
                    L"        �۸�������������ܵ���������, ���ڵ�СĿ��, �����������Ĺ�������\r\n"
                    L"        ����һ���СĿ��: ��΢��PatchGuard, ʵ��hook SSDT����\r\n"
                    L"        �ռ�Ŀ��: VT���⻯\r\n"
                    L"\r\n"
                    L"�ӵ� ����֧�ֿ�, �������������, ��c++��Դ���װ��������֧�ֿ�\r\n"
                    L"\r\n"
                    L"  ����Ҫ����ϵ QQ: 121007124\r\n"
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
        if (item && item((int)wParam))  // �����ȴ���ص�����¼�, Ȼ���ٴ����Լ����¼�, �����ص��������ص�һЩ�¼�
            return 0;
    }
    wstr::dbg(__FILEW__ L" - " __FUNCTIONW__ L" - " L"�����¼� id=%d, wParam = 0x%p\n", id, wParam);

    switch (id)
    {
    case 129: case 130: case 132:
        return 0;
    case 57670:     // F1���´�����ʾ
    {
        std::vector<int>* params = prop_GetTabParams();
        if (!params)
        {
            wstr dbg;
            dbg.Format(L"����������������, �к� = %d, ���� = %s, �ļ� = %s\n", __LINE__, __FUNCTIONW__, __FILEW__);
            MessageBoxW(0, dbg.c_str(), L"����", 0);
            throw;
        }
        HWND hTabTips = 0;  // �����ʾ��ѡ���
        int indexTips = 0;  // �����ʾ��ѡ����Ӽ�����

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
        if (indexTips != index) // �л�����ʾ�Ӽ�
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
    case 59393:     // ��ʾ����״̬��
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
            CallWindowProcA(pData->oldProc, hWnd, WM_COMMAND, 57603, 0);  // 57603 �Ǳ���Դ��˵��¼�
        return ret;
    }
    default:
        return _ewindow_menu_command(wParam, hControl, pData);
    }
    return 0;
}
