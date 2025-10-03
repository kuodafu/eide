#include "../EWindow.h"
#include <process_muster.h>
#include "../EMenu.h"
#include <assist/assist.h>
#include <control/CInputBox.h>
#include <Shlwapi.h>
#include <control/draw_Window_include.h>
#include <CDarkMode.h>
#include <CThemeColor.h>

typedef int (CALLBACK* pfn_IDraw_UnInterface)();
typedef void* (CALLBACK* pfn_EWindowInit)(IDRAW_INFO*, IGetEWindow*, int, int);
static pfn_EWindowInit pfn_Setting_IDraw_Interface;
static pfn_IDraw_UnInterface pfn_Setting_IDraw_UnInterface;

static HMODULE m_hModuleSetting;    // ����ģ����, �ڵ�����õ�ʱ�����
static HMENU m_hMenuTheme;
static HMENU m_hMenuTheme_newTheme;
const LPCWSTR APPNAME_THEME = L"Theme";

static LPEX_IMAGE m_image_sysbtn;       // ϵͳ��ťͼƬ��

BOOL EsysBtnCommand(LPOBJSTRUCT pData, BOOL isRBtn, DWORD ID);
bool Theme_WriteDefaultTheme(LPCWSTR themeNamme);


void EWindow_DrawSystemButton(LPCANVAS hCanvas, LPOBJSTRUCT pData)
{
    for (DWORD i = 0; i < sysBtn.count; i++)
    {
        RECT_F rc = sysBtn.btn[i].rc;
        DWORD id = sysBtn.btn[i].ID;
        if (sysBtn.btn[i].flag == SC_RESTORE)
            id = SYSBTN_RESTORE;
        DWORD index = id * 4;

        if (i == sysBtn.index)
        {
            //if (__query(sysBtn.state, STATE::HOT))
            //    index++;
            //else if (__query(sysBtn.state, STATE::DOWN))
            //    index += 2;
            //else if (__query(sysBtn.state, STATE::BAN))
            //    index += 3;

            DWORD argb = 0;
            if (sysBtn.btn[i].flag == SC_CLOSE)
            {
                if (__query(sysBtn.state, STATE::HOT))
                    argb = MAKEARGB(255, 212, 64, 39);
                else if (__query(sysBtn.state, STATE::DOWN))
                    argb = MAKEARGB(255, 166, 42, 21);
                else if (__query(sysBtn.state, STATE::BAN))
                    index += 3;
            }
            else
            {
                if (__query(sysBtn.state, STATE::HOT))
                    argb = RGB2ARGB(pData->draw.color.crHot, 255);
                else if (__query(sysBtn.state, STATE::DOWN))
                    argb = RGB2ARGB(pData->draw.color.crDown, 255);
                else if (__query(sysBtn.state, STATE::BAN))
                    index += 3;
            }
            if (argb)
            {
                LPEX_BRUSH hbr = s_info->pfnCreateBrushDx(argb);
                _canvas_fillRectangle(hCanvas, hbr, &rc);
            }
            if (__query(sysBtn.state, STATE::DOWN))
            {
                rc.offset(1, 1);
            }

        }

        assert(index < 48);

        if (!m_image_sysbtn)
        {
            COLORREF clr_menu_back = s_info->themeColor->crMenuBack();
            BYTE r = GetRValue(clr_menu_back);
            BYTE g = GetGValue(clr_menu_back);
            BYTE b = GetBValue(clr_menu_back);
            BOOL isDark = CThemeColor::IsArgbLight(255, r, g, b) == false;
            m_image_sysbtn = isDark ? sysBtn.pImage : sysBtn.pImage_light;
        }
        const RECT_F& rcImg = sysBtn.image->rc[index];
        _canvas_drawimagerectrect(hCanvas, m_image_sysbtn,
            rc.left, rc.top, rc.right, rc.bottom,
            rcImg.left, rcImg.top, rcImg.right, rcImg.bottom, 255);

    }
}

LRESULT EWndProc_OnNcCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPCWSTR str = 0;
    switch (wParam)
    {
    case SC_MAXIMIZE:
        str = L"���";
        break;
    case SC_RESTORE:
        str = L"��ԭ";
        break;
    case SC_MINIMIZE:
        str = L"��С��";
        break;
    case SC_CLOSE:
        str = L"�ر�";
        ShowWindowEx(s_info->eWnd->GetMenuMenu(), SW_SHOW);
        break;
    case SC_HELP:
    {
        str = L"����";
        ShellExecuteW(0, L"open", L"https://www.showdoc.com.cn/1503905757897784/7259668884727845", 0, 0, 1);
        return 0;
    }
    case SC_MENU:
    {
        //HMENU hMenu = s_info->iDraw->EMenu_LoadMenu(235);
        //if (hMenu)
        //{
        //    MENUITEMINFO item;
        //    item.cbSize = sizeof(MENUITEMINFO);
        //    item.fMask = MIIM_SUBMENU;
        //    GetMenuItemInfoW(hMenu, 0, true, &item);
        //    if (item.hSubMenu)
        //        s_info->iDraw->EMenu_PopUpMenu(hWnd, item.hSubMenu);
        //    DestroyMenu(hMenu);
        //}
        str = L"�˵�";
        return 0;
    }
    case SC_SETTING:
    {
        wstr set(260);
        set.assign(s_info->path_iDraw).append(L"Set").append(L".").append(L"exe");
        //set.assign(LR"(D:\cpp\SVN����Ŀ¼\�����Խ���\��IDE�Ӿ���\lib\iDraw\eTools\Set.exe)");
        DWORD pid = process_create(set.c_str(), 0, 0);
        //ShellExecuteW(0, L"open", set.c_str(), 0, 0, SW_SHOW);
        if (!pid)
        {
            wstr err;
            int lastErr = GetLastError();
            err.Format(L"���ó����ʧ��, ������ = %d\n���� \"%s\" �Ƿ����", lastErr, set.c_str());
            MessageBoxW(s_info->hEWnd, err.c_str(), IDRAW_FNE_NAME L" - �����ó���ʧ��", 0);
        }
        return 0;
        str = L"����";
        if (!m_hModuleSetting)
        {
            wstr settingFileName(260);
            settingFileName.assign(s_info->path_iDraw).append(L"iDraw_iSetting.dll");
            m_hModuleSetting = LoadLibraryW(settingFileName.c_str());
        }
        if (m_hModuleSetting)
        {

            // ����������ȥ������
            pfn_Setting_IDraw_Interface = (pfn_EWindowInit)GetProcAddress(m_hModuleSetting, "IDraw_Interface");
            pfn_Setting_IDraw_UnInterface = (pfn_IDraw_UnInterface)GetProcAddress(m_hModuleSetting, "IDraw_UnInterface");
            if (!pfn_Setting_IDraw_Interface || !pfn_Setting_IDraw_UnInterface)
            {
                FreeLibrary(m_hModuleSetting);
                m_hModuleSetting = 0;
                pfn_Setting_IDraw_Interface = 0;
                pfn_Setting_IDraw_UnInterface = 0;
                return 0;
            }
            pfn_Setting_IDraw_Interface(s_info, s_info->eWnd, 0, 0);

        }
        return 0;
    }
    case SC_SKIN:
    {
        str = L"Ƥ��";
        if (m_hMenuTheme)
            DestroyMenu(m_hMenuTheme);
        m_hMenuTheme = CreatePopupMenu();
        EMenu_InsertMenu(m_hMenuTheme);
        UINT menuID = IDMENU_SWITCH_THEME;
        MENUITEMINFOW item = { 0 };
        item.cbSize = sizeof(item);
        auto pfn_AppendMenu = [&](UINT id, LPCWSTR title, bool isChrck = false)
        {
            item.fMask = MIIM_ID | MIIM_STATE | MIIM_STRING;
            item.wID = id;
            item.dwTypeData = (LPWSTR)title;
            item.fState = isChrck ? MFS_CHECKED : MFS_UNCHECKED;
            // ����һ������/���õĲ˵���, Ĭ����ѡ��״̬
            InsertMenuItemW(m_hMenuTheme, -1, true, &item);   // ���뵽���ð�ť��ǰ��
        };

        DWORD count = 0;
        wstr value = s_info->ini->getAllKeyname(APPNAME_THEME, &count);
        LPWSTR pstr = value.data();
        wstr selName = s_info->ini->read(APPNAME_THEME, L"name");
#define _TMP_APPENDMENU(_s) pfn_AppendMenu(menuID++, _s, selName == _s)

        LPCWSTR* names = new LPCWSTR[count + 3];
        int menuCount = 0;
        names[menuCount++] = L"��ɫ����";
        names[menuCount++] = L"��ɫ����";
        names[menuCount++] = L"ǳɫVS����";
        _TMP_APPENDMENU(names[0]);
        _TMP_APPENDMENU(names[1]);
        _TMP_APPENDMENU(names[2]);

        for (DWORD i = 0, arrIndex = 3; i < count; i++, arrIndex++)
        {
            size_t strLen = wcslen(pstr);
            if (wcsicmp_api(pstr, L"name") != 0)
            {
                if (!( wcsicmp_api(pstr, L"��ɫ����") == 0
                    || wcsicmp_api(pstr, L"��ɫ����") == 0
                    || wcsicmp_api(pstr, L"ǳɫVS����") == 0))
                {
                    // ������Ĭ�ϵ��⼸����������˵������

                    names[menuCount++] = pstr;
                    pfn_AppendMenu(menuID++, pstr, selName == pstr);
                }
            }
            pstr += strLen + 1;
        }

        m_hMenuTheme_newTheme = CreatePopupMenu();
        wstr textMenu(20);
        int newMenuID = IDMENU_SWITCH_THEME_END;
        for (int i = 0; i < menuCount; i++)
        {
            textMenu.Format(L"�� \"%s\" ��������", names[i]);
            AppendMenuW(m_hMenuTheme_newTheme, MF_POPUP, newMenuID--, textMenu.c_str());;
        }
        AppendMenuW(m_hMenuTheme, MF_SEPARATOR, 0, L"");;
        AppendMenuW(m_hMenuTheme, MF_POPUP, (UINT_PTR)m_hMenuTheme_newTheme, L"�½�����");;
        delete[] names;
        POINT pt = { 0 };
        for (DWORD i = 0; i < sysBtn.count; i++)
        {
            if (sysBtn.btn[i].ID == SYSBTN_SKIN)
            {
                pt = { sysBtn.btn[i].rc.left, sysBtn.btn[i].rc.bottom };
                pt.x += s_info->rcEWindow.left;
                pt.y += s_info->rcEWindow.top;
                break;
            }
        }
        _menu_pop(hWnd, m_hMenuTheme, pt.x, pt.y);

        return 0;
    }
    default:
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
    }
    if (EsysBtnCommand(pData, FALSE, (DWORD)wParam))
        return 0;
    wstr::dbg(L"ϵͳ��ť[%s]������, ID = 0x%X, %d\n", str, wParam, lParam);
    //return 0;
    return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);
}


BOOL EsysBtnCommand(LPOBJSTRUCT pData, BOOL isRBtn, DWORD ID)
{

    return 0;
}


// �л�����, ��Ͷ����Ϣ���������������ԵĴ���
inline void Theme_Switch(LPCWSTR ThemeName)
{
    s_info->ini->write(APPNAME_THEME, L"name", ThemeName);      // �����µ�����
    s_info->pfnConfig(INE_CONFIG_CHANGED, 0, (LPARAM)s_info);   // ֪ͨ�滭������²���滭����, �ڲ�ʹ��
    HWND hEWnd = GetWindow(GetDesktopWindow(), GW_CHILD);
    wchar_t clsName[260] = { 0 };
    while (hEWnd)
    {
        if (hEWnd != s_info->hEWnd)
        {

            GetClassNameW(hEWnd, clsName, 260);
            if (wcsicmp_api(clsName, L"ENewFrame") == 0)
                PostMessageW(hEWnd, LVM_UPDATE, 0, 0);  // Ͷ�ݸ��µ���Ϣ�����������ԵĴ���
        }
        hEWnd = GetWindow(hEWnd, GW_HWNDNEXT);
    }

    COLORREF clr_menu_back = s_info->themeColor->crMenuBack();
    BYTE r = GetRValue(clr_menu_back);
    BYTE g = GetGValue(clr_menu_back);
    BYTE b = GetBValue(clr_menu_back);
    BOOL isDark = CThemeColor::IsArgbLight(255, r, g, b) == false;
    CUxThemeApi::SetPreferredAppMode(isDark ? ForceDark : ForceLight);
    CUxThemeApi::FlushMenuThemes();
    _hook_ThemeChange(isDark);
    m_image_sysbtn = isDark ? sysBtn.pImage : sysBtn.pImage_light;

    // LVM_UPDATE �����������ڴ��������Ϣ���ǵ��� s_info->pfnConfig(INE_CONFIG_CHANGED, wParam, (LPARAM)s_info);
}

struct INPUT_NEWTHEME_DATA
{
    HWND hBtn;          // ȷ�ϰ�ť, ��������ȷ���Ƿ�Ϸ�, �Ϸ���������, ���Ϸ��ͽ�ֹ���
    HWND hEdit_tips;    // ��ʾ��Ϣ�ı༭��
    HWND hEdit;         // �����ı༭��, ��Ҫʵʱ�ж�������Ƿ�Ϸ�
    int  count;         // �������������Ա��
    LPWSTR* themeName;  // ��������������
    LPWSTR* themeFile;  // ���������ļ�������

    wchar_t* writeName; // д����������, ʵʱ�ж��ļ�����ʱ���¼, �򿪶Ի���ķ���ֵ�Ͳ���Ҫ��
    wchar_t* writeFile; // д���������ļ���, ʵʱ�ж��ļ�����ʱ���¼, �򿪶Ի���ķ���ֵ�Ͳ���Ҫ��
};
inline INPUT_NEWTHEME_DATA* _new_theme_getData(HWND hDlg)
{
    const LPCWSTR paopName = L"{A3B11AE9-5979-4A1F-AD5C-1A6A30A9DDB9}";
    INPUT_NEWTHEME_DATA* data = (INPUT_NEWTHEME_DATA*)GetPropW(hDlg, paopName);
    if (!data)
    {
        data = new INPUT_NEWTHEME_DATA;
        memset(data, 0, sizeof(INPUT_NEWTHEME_DATA));
        SetPropW(hDlg, paopName, data);
    }
    return data;
}

// �ж�·���Ƿ���ڲ��Ϸ����ַ�, �Ϸ�����true, ���Ϸ�����false
inline bool _new_theme_cmp_file(LPCWSTR file)
{
    while (*file++)
    {
        wchar_t ch = *file;
        switch (ch)
        {
        // \/:*?"<>|
        case '\\':
        case '/':
        case ':':
        case '*':
        case '?':
        case '<':
        case '>':
        case '|':
            return false;
        default:
            break;
        }
    }
    return true;
}
// �ж�����������Ƿ�Ϸ�
inline void _new_theme_cmp_input_theme(HWND hDlg)
{
    INPUT_NEWTHEME_DATA* data = _new_theme_getData(hDlg);
    static wstr text(520);
    text.resize(520);
    int len = GetWindowTextLengthW(data->hEdit) + 1;
    len = GetWindowTextW(data->hEdit, text.data(), len);
    text.resize(len);
    bool isEnable = !text.empty();  // ��Ϊ�վ�����

    bool isRight = text.find(L'|') != wstr::npos;

    wstr themeName = isRight ? text.left(L"|") : text;
    wstr themeFile;
    themeFile.resize(text.size() + 4);

    themeFile = text.right(L"|");
    if (themeFile.empty())
        themeFile = themeName;
    wstr r4 = themeFile.right(4);
    if (r4.compare(L".ini", false) != 0)
        themeFile.append(L".ini");

    isEnable = isEnable && _new_theme_cmp_file(themeName.c_str());
    isEnable = isEnable && _new_theme_cmp_file(themeFile.c_str());

    for (int i = 0; isEnable && i < data->count; i++)
    {
        if (themeName.compare(data->themeName[i], false) == 0)  // �����������е���������ͬ, ��ȷ�ϰ�ť��ֹ
        {
            isEnable = false;
            break;
        }
        if (themeFile.compare(data->themeFile[i], false) == 0)  // ����·�������е�����·����ͬ, ��ȷ�ϰ�ť��ֹ
        {
            isEnable = false;
            break;
        }
    }

    wcscpy_s(data->writeName, 260, themeName.c_str());
    wcscpy_s(data->writeFile, 260, themeFile.c_str());

    const LPCWSTR inputTextTips = L"�����ʽ: ������|�ļ���\r\n����ļ���������׺�ڲ������.ini\r\n����������ļ���, ��Ĭ��ʹ�ø�������ͬ���ļ���";
    wstr tips;
    tips.Format(L"%s\r\n����Ԥ����%s=%s��%s", inputTextTips, themeName.c_str(), themeFile.c_str(), (isEnable ? L"" : (text.empty() ? L"û�������ı�" : L", �����������ͻ")));
    SetWindowTextW(data->hEdit_tips, tips.c_str());
    EnableWindow(data->hBtn, isEnable);
}

// �½�����, ��Ҫ���û�ѡ����ĸ����⿽��
// ��Ҫ������������û�����������, ��Ҫ�ж��������Ƿ��ظ�
inline void _new_theme(DWORD id)
{
    wchar_t name[260];
    MENUITEMINFOW item = { 0 };
    item.cbSize = sizeof(item);
    item.fMask = MIIM_STRING;
    item.dwTypeData = name;
    item.cch = 260;
    GetMenuItemInfoW(m_hMenuTheme_newTheme, id, false, &item);
    if (!name[0])
    {
        wstr dbg;
        dbg.Format(L"��������߼�����, ���ͼ���͸�006���з���\n�к� = %d\n���� = %s\n�ļ� = %s\n",
            __LINE__, __FUNCTIONW__, __FILEW__);
        MessageBoxW(s_info->hEWnd, dbg.c_str(), L"�߼�����", MB_ICONERROR);
        return;
    }

    wchar_t writeName[260];
    wchar_t writeFile[260];
    LPWSTR param[] = { writeName, writeFile };
    PFN_DLGPROC pfn_DlgProc = [](HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, INT_PTR* pRet) -> bool
    {
        switch (message)
        {
        case WM_INITDIALOG: // ��ʼ���Ի���, ��Ҫ���໯����ı༭��, ʵʱ�ж�������ı��Ƿ�Ϸ�
        {

            INPUT_NEWTHEME_DATA* data = _new_theme_getData(hDlg);
            LPWSTR* param = (LPWSTR*)lParam;
            data->writeName = param[0];
            data->writeFile = param[1];

            data->hBtn       = GetDlgItem(hDlg, IDOK);
            data->hEdit      = GetDlgItem(hDlg, 1001);
            data->hEdit_tips = GetDlgItem(hDlg, 1000);
   

            DWORD count = 0;
            wstr value = s_info->ini->getAllKeyname(APPNAME_THEME, &count);
            data->themeName = new LPWSTR[count];
            data->themeFile = new LPWSTR[count];
            LPWSTR pstr = value.data();
            wstr iniReadThemeFile;
            for (DWORD i = 0; i < count; i++)
            {
                size_t strLen = wcslen(pstr);
                if (wcsicmp_api(pstr, L"name") != 0)
                {
                    iniReadThemeFile = s_info->ini->read(APPNAME_THEME, pstr);
                    data->themeName[data->count] = wstr::copy(pstr);
                    data->themeFile[data->count] = wstr::copy(iniReadThemeFile.c_str());
                    ++data->count;
                }
                pstr += strLen + 1;
            }

            Dialog_DrawWindow(hDlg);
            break;
        }
        case WM_COMMAND:
        {
            const int id  = LOWORD(wParam);
            const int code = HIWORD(wParam);
            if (code == EN_CHANGE && id == 1001)
            {
                _new_theme_cmp_input_theme(hDlg);
                break;
            }
            break;
        }
        case WM_DESTROY:
        {
            INPUT_NEWTHEME_DATA* data = _new_theme_getData(hDlg);
            for (int i = 0; i < data->count; i++)
            {
                wstr::free(data->themeName[i]);
                wstr::free(data->themeFile[i]);
            }
            delete[] data->themeName;
            delete[] data->themeFile;
            delete data;
            break;
        }
        default:
            break;
        }
        return false;
    };
    INPUTBOXSTRUCT input    = { 0 };
    input.hParent           = s_info->hEWnd;
    input.text              = L"�����ʽ: ������|�ļ���\r\n����ļ���������׺�ڲ������.ini\r\n����������ļ���, ��Ĭ��ʹ�ø�������ͬ���ļ���";
    input.caption           = L"������������|�ļ���, �ļ�����ʡ��";
    input.defText           = L"������|�����Ӧ��ɫ�ļ���";
    input.pszOkButton       = L"ȷ������(&O)";
    input.pszCancelButton   = L"ȡ��(&C)";
    input.lpDialogFunc      = pfn_DlgProc;
    input.dwInitParam       = (LPARAM)param;



    wchar_t* ret = input_OpenEx(&input);
    if (!ret) return;
    input_free(ret);


    wstr keyName = name;
    keyName = keyName.substr(L"\"", L"\"");
    wstr keyValue = s_info->ini->read(APPNAME_THEME, keyName.c_str());  // ��ȡ���ĸ��ļ��������ļ���

    if (keyValue.empty())
    {
        wstr dbg;
        dbg.Format(L"��ȡ���� %s ʧ��, ���� Theme ���ý����Ƿ����������[%s]", keyName.c_str(), keyName.c_str());
        MessageBoxW(0, dbg.c_str(), L"��ȡ��������ʧ��", 0);
        return;
    }

    wstr themeFileName(260), copyTo(260);
    themeFileName.assign(s_info->path_data).append(keyValue);

    if (!PathFileExistsW(themeFileName.c_str()))
    {
        // DefaultTheme.ini
        // WhiteTheme.ini
        // VS2019Theme.ini
        bool isWrite = false;
        if (wcsicmp_api(keyValue.c_str(), L"DefaultTheme.ini") == 0)
        {
            isWrite = Theme_WriteDefaultTheme(L"DefaultTheme.ini");
        }
        else if (wcsicmp_api(keyValue.c_str(), L"WhiteTheme.ini") == 0)
        {
            isWrite = Theme_WriteDefaultTheme(L"WhiteTheme.ini");
        }
        else if (wcsicmp_api(keyValue.c_str(), L"VS2019Theme.ini") == 0)
        {
            isWrite = Theme_WriteDefaultTheme(L"VS2019Theme.ini");
        }

        if (!isWrite)
        {
            wstr dbg;
            dbg.Format(L"�����ļ�%s������, �Ҹ����ⲻ��Ĭ������, �޷�д��Ĭ������, �½�����ʧ��", themeFileName.c_str());
            MessageBoxW(0, dbg.c_str(), L"�½�����ʧ��", 0);
            return;
        }
    }

    copyTo.assign(s_info->path_data).append(writeFile);
    s_info->ini->write(APPNAME_THEME, writeName, writeFile);            // ������һ����������
    if (!CopyFileW(themeFileName.c_str(), copyTo.c_str(), false))
    {
        auto pfn_createFile = [](LPCWSTR themeFileName, LPCWSTR copyTo) -> int
        {
            CFileRW f1;
            bool isOk = false;
            while (1)
            {
                if (!f1.open(themeFileName, 1, -1))
                    break;
                int size = f1.size();
                if (size == -1)
                    break;
                LPBYTE pData = 0;
                if (!f1.read(&pData))
                    break;
                CFileRW f2;
                if (!f2.open(copyTo, 4, -1))
                    break;
                if (!f2.write(pData, size))
                    break;
                isOk = true;
                break;
            }
            if (!isOk)
                return GetLastError();
            return 0;
        };
        int err = GetLastError();
        int errCreateFile = pfn_createFile(themeFileName.c_str(), copyTo.c_str());
        int errfopen = 0;
        if (errCreateFile != 0)
        {
            bool isOk = false;
            while (true)
            {
                FILE* f;
                errfopen = (int)_wfopen_s(&f, themeFileName.c_str(), L"rb+");
                if (errfopen != 0 || !f)
                    break;

                fseek(f, 0, SEEK_END);
                long size = ftell(f);
                fseek(f, 0, SEEK_SET);

                _str buf(size);
                size = (long)fread_s(buf.data(), size, 1, size, f);
                fclose(f);
                isOk = true;
                break;
            }

            if (!isOk)
            {
                wstr dbg;
                dbg.Format(L"�����ļ�����, CopyFile ������ = %d\n�� %s ������ %s\n"
                    "����ʹ��fopenд������, ������ = %d\n"
                    "����ʹ��CreateFileд������, ������ = %d\n"
                    "���ͼ����Ϣ���͸�006���з���\n�к� = %d\n���� = %s\n�ļ� = %s\n",
                    err, themeFileName.c_str(), copyTo.c_str(), errfopen, errCreateFile,
                    __LINE__, __FUNCTIONW__, __FILEW__);
                MessageBoxW(s_info->hEWnd, dbg.c_str(), L"�����ļ�����", MB_ICONERROR);
                return;
            }
        }

    }

    Theme_Switch(writeName);    // �л����½�������
}

bool Theme_WriteDefaultTheme(LPCWSTR themeNamme)
{
    // ѡ�е���ǰ3��Ĭ������, ��Ҫ�ж�Ĭ��������ļ�, ��ͬ��д��
    LPCWSTR keyName = L"", clrName = L"", iniName = L"";
    int skip = 0;
    DWORD size;
    LPBYTE ptr = s_info->pResource->GetDefThemeData(&size);
    struct _CMP_MD5_DATA
    {
        wchar_t iniMd5[65];
        wchar_t clrMd5[65];
        LPBYTE pIniStart;
        int nIniSize;
        LPBYTE pClrStart;
        int nClrSize;
        _CMP_MD5_DATA() { memset(this, 0, sizeof(*this)); }
    };

    auto pfn_getData = [](LPBYTE& ptr, _CMP_MD5_DATA* pData = 0)
    {
        int readSize = *((int*)ptr); ptr += 4;
        if (pData)
        {
            pData->pIniStart = ptr;
            pData->nIniSize = readSize;
            GetMd5(ptr, readSize, pData->iniMd5);
        }
        ptr += readSize;

        readSize = *((int*)ptr); ptr += 4;
        if (pData)
        {
            pData->pClrStart = ptr;
            pData->nClrSize = readSize;
            GetMd5(ptr, readSize, pData->clrMd5);
        }
        ptr += readSize;
    };

    _CMP_MD5_DATA data;

    if (wcsicmp_api(themeNamme, L"DefaultTheme.ini") == 0)
    {
        keyName = L"��ɫ����";
        clrName = L"vs2019.clr";
        iniName = L"DefaultTheme.ini";
        skip = 0;
    }
    else if (wcsicmp_api(themeNamme, L"WhiteTheme.ini") == 0)
    {
        keyName = L"��ɫ����";
        clrName = L"WhiteTheme.clr";
        iniName = L"WhiteTheme.ini";
        skip = 1;
    }
    else if (wcsicmp_api(themeNamme, L"VS2019Theme.ini") == 0)
    {
        keyName = L"ǳɫVS����";
        clrName = L"VS2019Theme.clr";
        iniName = L"VS2019Theme.ini";
        skip = 2;
    }
    else
    {
        return false;
    }
    
    for (int i = 0; i < skip; i++)
        pfn_getData(ptr, 0);
    pfn_getData(ptr, &data);

    auto pfn_GetMd5 = [](LPCWSTR fileName) -> wstr
    {
        wstr md5(60);
        CFileRW f(fileName);
        int size = f.size();
        if (size == -1) return L"-1";
        LPBYTE pData = new BYTE[size];
        f.read(pData, size);
        GetMd5(pData, size, md5);
        delete[] pData;
        md5.resize_strlen();
        return md5;
    };

    wstr vl = s_info->ini->read(APPNAME_THEME, keyName, iniName);
    if (vl != iniName)
        s_info->ini->write(APPNAME_THEME, keyName, iniName);


    // �ж�INI�ļ�, ��ͬ��д��ini
    {
        wstr theme(260);    // ��ǰ��ȡ������ɫini����·��
        theme.assign(s_info->path_data).append(iniName);
        wstr md5 = pfn_GetMd5(theme.c_str());

        if (md5 != data.iniMd5)
        {
            CFileRW f;
            f.open(theme.c_str(), 4, -1);
            f.write(data.pIniStart, data.nIniSize);
            f.reset();
        }

    }

    // �ж�clr�ļ�, ��ͬ��д��clr
    {
        wstr clrFile(260);    // ��ǰ��ȡ������ɫini����·��
        clrFile.assign(s_info->path_e).append(L"clr\\").append(clrName);
        wstr md5 = pfn_GetMd5(clrFile.c_str());

        if (md5 != data.clrMd5)
        {
            CFileRW f;
            f.open(clrFile.c_str(), 4, -1);
            f.write(data.pClrStart, data.nClrSize);
            f.reset();
        }

    }
    return true;
}

LRESULT EWndProc_OnThemeCommand(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    DWORD id = (DWORD)wParam;

    const int menuCount = GetMenuItemCount(m_hMenuTheme_newTheme);
    int newMenuID = IDMENU_SWITCH_THEME_END - menuCount + 1;
    if (id >= (DWORD)newMenuID && id <= IDMENU_SWITCH_THEME_END)
    {
        // ѡ�е����½�����
        _new_theme(id);
        return 0;
    }

    const short shift = GetAsyncKeyState(VK_SHIFT);
    const bool isShift = (shift & 0x8000) != 0;
    if (isShift && id >= IDMENU_SWITCH_THEME && id < IDMENU_SWITCH_THEME + 3)
    {
        // ѡ�е���ǰ3��Ĭ������, ��Ҫ�ж�Ĭ��������ļ�, ��ͬ��д��
        LPCWSTR keyName = L"";
        switch (id)
        {
        case IDMENU_SWITCH_THEME + 0:   // ��ɫ����
        {
            keyName = L"��ɫ����";
            break;
        }
        case IDMENU_SWITCH_THEME + 1:   // ��ɫ����
        {
            keyName = L"��ɫ����";
            break;
        }
        case IDMENU_SWITCH_THEME + 2:   // ǳɫVS����
        {
            keyName = L"ǳɫVS����";
            break;
        }
        default:
            break;
        }
        Theme_WriteDefaultTheme(keyName);
        
        s_info->ini->write(APPNAME_THEME, L"name", keyName);    // �����µ�����
        s_info->pfnConfig(INE_CONFIG_CHANGED, wParam, (LPARAM)s_info); // ֪ͨ�滭������²���滭����, �ڲ�ʹ��
        //TODO ��Ҫ�����������Դ��ڷ���LVM_UPDATE��Ϣ, wParam, lParam ��Ϊ0
        // LVM_UPDATE �����������ڴ��������Ϣ���ǵ��� s_info->pfnConfig(INE_CONFIG_CHANGED, wParam, (LPARAM)s_info);

        return 0;
    }
    wchar_t name[260];
    MENUITEMINFOW item = { 0 };
    item.cbSize = sizeof(item);
    item.fMask = MIIM_STRING;
    item.dwTypeData = name;
    item.cch = 260;
    GetMenuItemInfoW(m_hMenuTheme, id, false, &item);

    if (!name[0])return 0;

    LPCWSTR lpszName = name;
    wchar_t* pos = wcschr(name, '\"');
    if (pos)
    {
        *pos++ = 0;
        lpszName = pos;
        pos = wcschr(pos, '\"');
        if (pos)
        {
            *pos = 0;
        }
    }

    Theme_Switch(lpszName);


    return 0;
}

int ThemeMenu_LoadString(int id, LPWSTR lpBuffer, int cchBufferMax)
{
    if (id >= IDMENU_SWITCH_THEME && id < IDMENU_SWITCH_THEME + 3)
    {
        LPCWSTR vl = L"";
        switch (id)
        {
        case IDMENU_SWITCH_THEME + 0:   // ��ɫ����
            vl = L"��ɫ����: DefaultTheme.ini, �Ӿ���Ĭ�ϵĺ�ɫ����, ����Shift��ѡ�к���Ĭ�ϵĺ�ɫ������ɫд��";
            break;
        case IDMENU_SWITCH_THEME + 1:   // ��ɫ����
            vl = L"��ɫ����: WhiteTheme.ini, �Ӿ���Ĭ�ϵİ�ɫ����, ����Shift��ѡ�к���Ĭ�ϵİ�ɫ������ɫд��";
            break;
        case IDMENU_SWITCH_THEME + 2:   // ǳɫVS����
            vl = L"ǳɫVS����: VS2019Theme.ini, �Ӿ���Ĭ�ϵ�ǳɫVS����, ����Shift��ѡ�к���Ĭ�ϵ�ǳɫVS������ɫд��";
            break;
        default:
            break;
        }
        int len = (int)wcslen(vl);
        wcscpy_api(lpBuffer, cchBufferMax, vl);
        return len;
    }

    wchar_t name[260];
    MENUITEMINFOW item = { 0 };
    item.cbSize = sizeof(item);
    item.fMask = MIIM_STRING;
    item.dwTypeData = name;
    item.cch = 260;
    GetMenuItemInfoW(m_hMenuTheme, id, false, &item);
    if (!name[0])return 0;

    wstr value = s_info->ini->read(APPNAME_THEME, name);
    if (value.empty())
        return 0;

    wstr showName;
    showName.Format(L"%s: %s, ѡ�иò˵���Ҫ�л����������������ļ�", name, value.c_str());

    int len = (int)showName.size();
    if (len + 1 > cchBufferMax)
        len = cchBufferMax - 1;
    
    wmemcpy(lpBuffer, showName.c_str(), len);

    return (int)len;
}


void AddSysButton(DWORD flag, int ID, LPOBJSTRUCT pData)
{
    SYSBUTTON& btn = sysBtn.btn[sysBtn.count];
    const RECT& rc = pData->pos.rcClient;        // ���ڿͻ�������
    if (sysBtn.count == 0)
    {
        btn.rc.right = rc.right;
        btn.rc.left = btn.rc.right - SYSBTN_WIDTH;
        btn.rc.top = 0;
        btn.rc.bottom = SYSBTN_HEIGHT;
    }
    else
    {
        const SYSBUTTON& prve = sysBtn.btn[sysBtn.count - 1];

        btn.rc.right = prve.rc.left;
        btn.rc.left = btn.rc.right - SYSBTN_WIDTH;
        btn.rc.top = prve.rc.top;
        btn.rc.bottom = prve.rc.bottom;
    }
    ++sysBtn.count;
    btn.flag = flag;
    btn.ID = ID;
}

void ModifySysButtonPos(LPOBJSTRUCT pData)
{
    const RECT& rc      = sysBtn.rcBtn;  // ��ť����
    const int width     = s_info->scale(SYSBTN_WIDTH);
    const int height    = s_info->scale(SYSBTN_HEIGHT);
    for (DWORD i = 0; i < sysBtn.count; i++)
    {
        sysBtn.btn[i].rc.right  = rc.right - i * width;
        sysBtn.btn[i].rc.left   = sysBtn.btn[i].rc.right - width;
        sysBtn.btn[i].rc.top    = rc.top;
        sysBtn.btn[i].rc.bottom = rc.top + height;
    }


    sysBtn.rcCaption.left   = s_info->rcMenu.right;
    sysBtn.rcCaption.right  = sysBtn.rcBtn.left;
    sysBtn.rcCaption.top    = sysBtn.rcBtn.top;
    sysBtn.rcCaption.bottom = sysBtn.rcBtn.top + height;

    sysBtn.rcBtn.left   = sysBtn.rcBtn.right - width * sysBtn.count;
    sysBtn.rcBtn.bottom = sysBtn.rcBtn.top + height;

    m_rcTitle.left      = s_info->rcMenu.right + 10;
    m_rcTitle.top       = sysBtn.rcBtn.top;
    //m_rcTitle.right     = sysBtn.rcBtn.left - 10;
    m_rcTitle.bottom    = sysBtn.rcBtn.bottom;

    //const int maxWidth = 260;
    //if (m_rcTitle.right - m_rcTitle.left > maxWidth)
    //    m_rcTitle.right = m_rcTitle.left + maxWidth;


    sysBtn.rcIcon.left      = 0;
    sysBtn.rcIcon.top       = 0;
    sysBtn.rcIcon.right     = width;
    sysBtn.rcIcon.bottom    = height;

}


void ewnd_createed(HWND hWnd, DWORD style, LPOBJSTRUCT pData)
{
    GetWindowRect(hWnd, &pData->pos.rc);

    const int width     = s_info->scale(SYSBTN_WIDTH);
    const int height    = s_info->scale(SYSBTN_HEIGHT);

    pData->pos.rcClient.right  = pData->pos.rc.right  - pData->pos.rc.left;
    pData->pos.rcClient.bottom = pData->pos.rc.bottom - pData->pos.rc.top;

    pData->pos.width = pData->pos.rcClient.right;
    pData->pos.height = pData->pos.rcClient.bottom;

    if ((style & (WS_SYSMENU | WS_CAPTION)))
        AddSysButton(SC_CLOSE, SYSBTN_CLOSE, pData);
    if (style & WS_MAXIMIZEBOX)    // ����󻯰�ť
        AddSysButton(SC_MAXIMIZE, SYSBTN_MAX, pData);
    if (style & WS_MINIMIZEBOX)    // ����С����ť
        AddSysButton(SC_MINIMIZE, SYSBTN_MIN, pData);
    //AddSysButton(SC_MENU, SYSBTN_MENU, pData);
    AddSysButton(SC_SETTING, SYSBTN_SETTING, pData);
    AddSysButton(SC_SKIN, SYSBTN_SKIN, pData);
    AddSysButton(SC_HELP, SYSBTN_HELP, pData);

    sysBtn.rcBtn.right = pData->pos.rc.right - pData->pos.rc.left;
    sysBtn.rcBtn.left = sysBtn.rcBtn.right - sysBtn.count * width;
    sysBtn.rcBtn.top = 0;
    sysBtn.rcBtn.bottom = height;

    sysBtn.rcIcon.left      = 0;
    sysBtn.rcIcon.top       = 0;
    sysBtn.rcIcon.right     = width;
    sysBtn.rcIcon.bottom    = height;

    sysBtn.rcCaption.left   = sysBtn.rcIcon.right;
    sysBtn.rcCaption.top    = sysBtn.rcIcon.top;
    sysBtn.rcCaption.right  = sysBtn.rcBtn.left;
    sysBtn.rcCaption.bottom = sysBtn.rcIcon.bottom;



}


void Dialog_DrawWindow(HWND hDlg)
{
    DRAW_WINDOW_ARGUMENT arg;
    memset(&arg, 0, sizeof(arg));

    s_info->pfnControls(IDC_IDRAW_DRAW_DEFARG, (WPARAM)hDlg, (LPARAM)&arg);
    arg.isDrawIcon = false;
    arg.isSubClass = true;

    s_info->pfnControls(IDC_IDRAW_SUBWINDOWEX, (WPARAM)hDlg, (LPARAM)&arg);

    HWND hBtn       = GetDlgItem(hDlg, IDOK);
    HWND hCancel    = GetDlgItem(hDlg, IDCANCEL);
    HWND hEdit      = GetDlgItem(hDlg, 1001);
    HWND hEdit_tips = GetDlgItem(hDlg, 1000);

    _subclass_Button(hBtn       , false, false, 0);
    _subclass_Button(hCancel    , false, false, 0);
    _subclass_Edit  (hEdit      , false, false, 0, false);
    _subclass_Edit  (hEdit_tips , false, false, 0, false);
}
