#include "EMenu.h"
#include <shlwapi.h>
#include <CStringBuffer.h>
#include "EWindow.h"
#include <shlobj_core.h>
#include <assist/assist.h>
#include <process_muster.h>

static std::unordered_map<int, LPCWSTR> m_map_text; // ��Ų˵����Ӧ����ʾ�ı�
static std::unordered_map<int, LPCWSTR> m_map_path; // ��Ų˵����Ӧ���ļ�·��
static CStringBufferW m_MenuTextBuffer; // �˵��ַ���������
// ���˵�����ӳ���ģ��
void _insert_menu_Module(HMENU hMenu);
// ���˵�����ӳ���Դ��
void _insert_menu_SourceCode(HMENU hMenu);
// �����ʷԴ�뵽�˵���
void _insert_menu_HistorySourceCode(HMENU hMenu);
int RButton_LoadString(int id, LPWSTR lpBuffer, int cchBufferMax);

// �˵��¼�����
int RButton_Command(int id);

// ����ģ��Ĳ˵�����
int RButton_Command_Module(int id);
// ����Դ��Ĳ˵�����
int RButton_Command_SourceCode(int id);
// ��ʷ�򿪵�Դ��Ĳ˵�����
int RButton_Command_HistorySourceCode(int id);

// ����Դ��·��
void MenuEvent_CopySourceCodeFile();
// �˵��¼� ��λ������ɫ�ļ�
void MenuEvent_Location_theme_color_config();


void EMenu_InsertRButton(HMENU hMenuParent)
{
    // ���Ҽ������ʱ������ı�������
    m_MenuTextBuffer.clear();
    m_map_text.clear();
    m_map_path.clear();

    static bool isFirst;
    if (!isFirst)
    {
        isFirst = true;
        m_MenuTextBuffer.reserve(0x40000);  // Ԥ�ȷ�����ô��һ���ڴ�, �����Ļ�����������ڲ����Լ�����
        HookLoadString_PushCallback(RButton_LoadString);
        EMenu_PushCommand(RButton_Command);
    }
    //0 "&T.��׼������", 59392
    //1 "&A.���빤����", 133
    //2 "&K.��λ������", 135
    //3 "&S.״̬��", 59393
    //4 "&K.���������л���", 32963
    //5 "&W.������\tAlt+1", 129
    //6 "&O.״̬��\tAlt+2", 130
    //7 "&C.������״̬��ϼ�\tAlt+3", 132
    //8 "&U.���������\tAlt+4", 110
    //9 "&R.��ʾ��ʼҳ", 32981

    DeleteMenu(hMenuParent, 110, MF_BYCOMMAND);
    DeleteMenu(hMenuParent, 132, MF_BYCOMMAND);
    DeleteMenu(hMenuParent, 130, MF_BYCOMMAND);
    DeleteMenu(hMenuParent, 129, MF_BYCOMMAND);
    DeleteMenu(hMenuParent, 32963, MF_BYCOMMAND);
    DeleteMenu(hMenuParent, 135, MF_BYCOMMAND);
    DeleteMenu(hMenuParent, 133, MF_BYCOMMAND);
    DeleteMenu(hMenuParent, 59392, MF_BYCOMMAND);


    DWORD isEnabled = (PathFileExistsW(s_info->pszTitle->c_str())) ? MF_ENABLED : MF_DISABLED;

    DWORD flags = MF_BYPOSITION | (s_info->config->IsShowLocationTool() ? MF_CHECKED : MF_UNCHECKED);
    InsertMenuW(hMenuParent, 0, flags, IDMENU_LOCATIONTOOLBAR, L"&K.��λ������");
    AppendMenuW(hMenuParent, MF_SEPARATOR, 0, 0);
    AppendMenuW(hMenuParent, isEnabled, IDMENU_LOCATIONFILE, L"��Դ��·��");
    AppendMenuW(hMenuParent, isEnabled, IDMENU_COPYECODEFILE, L"����Դ��·��");
    AppendMenuW(hMenuParent, MF_SEPARATOR, 0, 0);

    m_map_text[IDMENU_LOCATIONFILE] = m_MenuTextBuffer.AddString(L"��λ��ǰ�����Դ򿪵�Դ���ļ�\n��λԴ���ļ�");
    m_map_text[IDMENU_COPYECODEFILE] = m_MenuTextBuffer.AddString(
        L"�ٰ�סCtrlֻ�����ļ�������׺\r\r"
        "�ڰ�סShiftֻ�����ļ���û�к�׺\r\r"
        "�۰�סCtrl+Shift��������·��ǰ�������\r\r"
        "�ܲ���ס�κΰ�����������·��\n����Դ���ļ�");



    HMENU hMenuLocation = CreatePopupMenu();
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_E         , L"��λ������Ŀ¼");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_ELIB      , L"��λ������֧�ֿ�Ŀ¼");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_IDRAW     , L"��λ�Ӿ���Ŀ¼");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_IDRAW_DATA, L"��λ�Ӿ�������Ŀ¼");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_CONFIG    , L"��λ�Ӿ��������ļ�");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_THEME     , L"��λ��ǰ������ɫ�ļ�");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_PLUGIN    , L"��λ�Ӿ�����Ŀ¼");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_ET_PLUGIN , L"��λ���ֲ��Ŀ¼");
    AppendMenuW(hMenuParent, MF_POPUP, (UINT_PTR)hMenuLocation  , L"��Ŀ¼");
    AppendMenuW(hMenuParent, MF_SEPARATOR, 0, 0);

    m_map_text[(int)hMenuLocation       ] = m_MenuTextBuffer.AddString(L"��λһЩ���õĹ���Ŀ¼");
    m_map_text[IDMENU_PATH_E            ] = m_MenuTextBuffer.AddString(L"��λ�����Ը�Ŀ¼");
    m_map_text[IDMENU_PATH_ELIB         ] = m_MenuTextBuffer.AddString(L"��λ������֧�ֿ�Ŀ¼, ������Ŀ¼\\lib\\");
    m_map_text[IDMENU_PATH_IDRAW        ] = m_MenuTextBuffer.AddString(L"��λ�Ӿ���Ŀ¼, ������Ŀ¼\\lib\\iDraw\\");
    m_map_text[IDMENU_PATH_IDRAW_DATA   ] = m_MenuTextBuffer.AddString(L"��λ�Ӿ�������Ŀ¼, ������Ŀ¼\\lib\\iDraw\\data\\");
    m_map_text[IDMENU_PATH_CONFIG       ] = m_MenuTextBuffer.AddString(L"��λ�Ӿ��������ļ�, ������Ŀ¼\\lib\\iDraw\\config.ini");
    m_map_text[IDMENU_PATH_THEME        ] = m_MenuTextBuffer.AddString(L"��λ��ǰ������ɫ�ļ�\r\r��λ����ǰʹ�õ��������ɫ�ļ�\r\r"
                                                                        "���� config.ini [Theme] �µ� name ����������λ��ɫ�ļ�");
    m_map_text[IDMENU_PATH_PLUGIN       ] = m_MenuTextBuffer.AddString(L"��λ�Ӿ�����Ŀ¼\r\r"
                                                                        "�Ӿ���Ĳ��Ŀ¼, �Ӿ���Ĳ������ iDraw_ ��ͷ\r\r"
                                                                        "����iDraw_��ͷ�Ĳ��, �Ӿ��ⲻ�����");

    m_map_text[IDMENU_PATH_ET_PLUGIN    ] = m_MenuTextBuffer.AddString(L"��λ���ֲ��Ŀ¼\r\r"
                                                                        "�Ӿɵ�������Ѳ���ŵ����Ŀ¼, �Ӿ��������Ҳ���Լ�����Щ���\r\r"
                                                                        "������Ĳ�������������Ӿ��������, ������Ĳ����Ҫ�Ž���\r\r"
                                                                        "��Ҫʹ����������, ���006���°�");







    HMENU hMenuSourceCode = CreatePopupMenu();
    HMENU hMenuModule = CreatePopupMenu();
    HMENU hMenuSource = CreatePopupMenu();
    AppendMenuW(hMenuParent, MF_POPUP, (UINT_PTR)hMenuSourceCode, L"��ʷ�򿪵�Դ��");
    AppendMenuW(hMenuParent, MF_POPUP, (UINT_PTR)hMenuModule, L"����ģ��");
    AppendMenuW(hMenuParent, MF_POPUP, (UINT_PTR)hMenuSource, L"����Դ��");

    m_map_text[(int)hMenuSourceCode] = m_MenuTextBuffer.AddString(L"�����½������¼����ʷ��Դ���г�����򿪵�Դ��");
    m_map_text[(int)hMenuModule] = m_MenuTextBuffer.AddString(L"�г��������õĳ��õ�ģ��, ���Կ��ټ��ػ��߲鿴ģ����Ϣ");
    m_map_text[(int)hMenuSource] = m_MenuTextBuffer.AddString(L"�г��������õĳ��õ�Դ��, ����ʹ�õ�ǰ�����Դ�Դ���ļ������½������Դ��ڴ�Դ���ļ�");


    _insert_menu_HistorySourceCode(hMenuSourceCode);
    _insert_menu_Module(hMenuModule);
    _insert_menu_SourceCode(hMenuSource);


    AppendMenuW(hMenuParent, MF_SEPARATOR, 0, 0);
    //AppendMenuW(hMenuParent, MF_DISABLED, 12, L"���ù���");
    AppendMenuW(hMenuParent, 0, IDMENU_LOADDBGPLUGIN, L"���ص��Բ��");
    AppendMenuW(hMenuParent, 0, IDMENU_FREEDBGPLUGIN, L"ж�ص��Բ��");
    m_map_text[IDMENU_LOADDBGPLUGIN] = m_MenuTextBuffer.AddString(L"���� ֧�ֿ�Ŀ¼\\iDraw\\debug Ŀ¼�� iDraw_ ��ͷ��dll�ļ� \n���ص��Բ��");
    m_map_text[IDMENU_FREEDBGPLUGIN] = m_MenuTextBuffer.AddString(L"ж�ش� ֧�ֿ�Ŀ¼\\iDraw\\debug Ŀ¼�¼��صĲ��, ȡ��ռ��, �ò�����Ա������ \nж�ص��Բ��");



    //EWINDOW_MENUSTRUCT& menu = s_info->menu;
    //HMENU hMenu = menu.hMenu;
    //if (menu.count != menu.index)   // ���������ӹ��˵���, ��Ҫ���뵽�µĲ˵���
    //{
    //    for (int i = menu.count; i < menu.index; i++)
    //    {
    //        UINT uFlags = MF_STRING;
    //        if (menu.menu[i].text[0])
    //        {
    //            if (wcscmp(menu.menu[i].text, L"�ʿ�") == 0 || wcscmp(menu.menu[i].text, L"����") == 0)
    //            {
    //                uFlags = MF_POPUP;
    //            }
    //            AppendMenuW(hMenuParent, uFlags, menu.menu[i].id, menu.menu[i].text);
    //        }
    //    }
    //}


}

// ��һ��·������һ��, ֻ��ʾ��һ������һ��·��, �м�ʹ�� ... ��ʾ
inline void _path_dot(LPCWSTR file, wstr& ret)
{
    LPCWSTR pos = wcschr(file + 3, '\\');    // ·������c:/xxx ��/���濪ʼ����
    LPCWSTR pos2 = wcsrchr(file, '\\');      // ���ҷ�б��, ���Ҫ��ʾ���˵���

    if (!pos || !pos2)
    {
        // �ӵڶ���б�ܿ�ʼû���ҵ�б��, �Ǿͼ�������·��
        ret.assign(file);
    }
    else
    {
        ret.assign(file);
        ret.assign(file, pos - file + 1);
        ret.append(L"...").append(pos2);                // c:xxx\...\xxx.e
    }
}

// ��һ��·������һ��, ֻ��ʾ��һ������һ��·��, �м�ʹ�� ... ��ʾ
inline void _path_dot(LPCSTR file, _str& ret)
{
    LPCSTR pos = strchr(file + 3, '\\');    // ·������c:/xxx ��/���濪ʼ����
    LPCSTR pos2 = strrchr(file, '\\');      // ���ҷ�б��, ���Ҫ��ʾ���˵���

    if (!pos || !pos2)
    {
        // �ӵڶ���б�ܿ�ʼû���ҵ�б��, �Ǿͼ�������·��
        ret.assign(file);
    }
    else
    {
        ret.assign(file);
        ret.assign(file, pos - file + 1);
        ret.append("...").append(pos2);                // c:xxx\...\xxx.e
    }
}

// �����ʷԴ�뵽�˵���
void _insert_menu_HistorySourceCode(HMENU hMenu)
{
    _str data;
    wstr file(260);
    file.assign(s_info->path_ETplugin).append(L"�½���ǿ���\\Data\\History.txt");
    if (!_get_file_data(file.c_str(), data))
        return;
    LPWSTR ptr = (LPWSTR)m_MenuTextBuffer.AddString(data.a2w().c_str());
    LPWSTR name = ptr, time = 0;
    wstr tmpName(260);  // ƴ�Ӳ˵�����ʹ�õ��ַ���
    int index = IDMENU_HISTORYCODE_START;   // ��ʼ�˵�ID
    while (*ptr)
    {
        wchar_t& ch = *ptr++;
        if (ch == '|')
        {
            // '|' ���濪ʼ��ʱ��, �ж��ļ��Ƿ����, Ȼ����ӵ��˵���
            // ��Ҫ���ļ�·�����д���, ֻ��ʾ c:/xxx/.../xxx.e
            time = ptr;
            ch = '\0';

            if (!PathFileExistsW(name))
                continue;
            // �ļ�����, ���˵��������Ŀ
            _path_dot(name, tmpName);

            LPCWSTR pos3 = wcschr(time, '\r');
            if (!pos3)
                pos3 = wcsrchr(time, '\n');
            size_t appendTimeSize = pos3 ? pos3 - time : wcslen(time);
            //tmpName.append(L"\t").append(time, appendTimeSize);

            LPCWSTR menuText = m_MenuTextBuffer.AddString(tmpName.c_str());

            tmpName.assign(time, appendTimeSize);
            LPCWSTR tips = m_MenuTextBuffer.AddFormat(
                L"��Դ�� %s\r\r\r\r"
                L"ֱ��ѡ�� ����Ŀ��ֱ�Ӵ�, �����½������Դ�\r\r"
                L"Shift + ѡ�� ʼ��ʹ�õ�ǰ�����Դ򿪸�Դ��\r\r"
                L"Ctrl  + ѡ�� ʼ���½�һ�������Դ򿪸�Դ��"
                , name, tmpName.c_str());
            m_map_text[index] = tips;
            m_map_path[index] = m_MenuTextBuffer.AddString(name);

            AppendMenuW(hMenu, MF_STRING, index++, menuText);
            if (index > IDMENU_HISTORYCODE_END)
                break;  // �˵��Ѿ����������ʾ����, ֱ������ѭ��, �����������

            continue;
        }
        if (ch == '\r' || ch == '\n')
        {
            name = ptr;
            ch = '\0';
            continue;
        }
    }

}



// ���˵�����ӳ���ģ��
void _insert_menu_Module(HMENU hMenu)
{
    typedef LPINT(__stdcall* pfn_E_GetEList)(LPCSTR dbFile, LPCSTR dllFile, LPCSTR** pRetFl);
    typedef struct _tagMODULE_INFO
    {
        int         id;         // id
        LPCSTR      name;       // �ļ���
        LPCSTR      path;       // ·��
        int         sort;       // ����
        int         favorite;   // �ղ�
        int         fenlei;     // ����ID, �����ķ���id
    }MODULE_INFO, *PMODULE_INFO;
    static pfn_E_GetEList E_GetEList;
    if (!E_GetEList)
    {
        wstr dllFile(260);
        dllFile.assign(s_info->path_iDraw).append(L"GetEcListData.dll");
        HMODULE hModule = LoadLibraryW(dllFile.c_str());
        if (!hModule)
            return;
        E_GetEList = (pfn_E_GetEList)GetProcAddress(hModule, "E_GetEList");
        if (!E_GetEList)
        {
            FreeLibrary(hModule);
            return;
        }
    }

    wstr dbFile(260), dllFile(260);
    dbFile.assign(s_info->path_iDraw).append(L"eTools\\Data\\ecList.db");
    dllFile.assign(s_info->path_iDraw).append(L"eTools\\linklib\\sqlite3_2401.dll");

    LPCSTR* fl = 0;
    LPINT pArr = E_GetEList(dbFile.w2a().c_str(), dllFile.w2a().c_str(), &fl);
    if (!pArr) return;


    int menuIndex = IDMENU_MODULE_START;
    _str name(260);
    int nArrCount = *(pArr - 1);    // ��Ա��

    for (int i = 0; i < nArrCount; i++)
    {
        LPCSTR flName = fl[i];
        if (!flName || !flName[0])
            continue;

        HMENU hMenuPop = CreatePopupMenu();
        AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hMenuPop, flName);       // ��ӷ��൯��ʽ�˵�

        LPINT pItemArr = (LPINT)pArr[i];
        if (!pItemArr) continue;
        int nItemCount = *(pItemArr - 1);
        for (int n = 0; n < nItemCount; n++)
        {
            PMODULE_INFO item = (PMODULE_INFO)pItemArr[n];
            if (!item || !PathFileExistsA(item->path)) continue;
            // �����·�����뵽�˵���, ��Ҫ����һ��·��
            _path_dot(item->path, name);

            wstr pathW = _str::A2W(item->path);
            LPCWSTR tips = m_MenuTextBuffer.AddFormat(
                L"%s\r\r\r\r"
                L"ֱ��ѡ�� ���ģ�鵽��ǰ����\r\r"
                L"Ctrl  + ѡ�� �鿴ģ����Ϣ\r\r"
                , pathW.c_str());

            m_map_text[menuIndex] = tips;
            m_map_path[menuIndex] = m_MenuTextBuffer.AddString(pathW.c_str());
            AppendMenuA(hMenuPop, MF_STRING, menuIndex++, name.c_str());
            if (menuIndex > IDMENU_MODULE_END)
                break;  // �˵��Ѿ����������ʾ����, ֱ������ѭ��, �����������
        }
        if (menuIndex > IDMENU_MODULE_END)
            break;  // �˵��Ѿ����������ʾ����, ֱ������ѭ��, �����������
    }

}

// �������ļ�����ӳ���Դ��
void _insert_menu_sourcecode_from_ini(HMENU hMenu, LPCWSTR iniFile)
{
    // iniFile ָ���¼Դ��·����ini�ļ�, ��Ҫ��ȡ·������
    // �����и�����, ������������, ʹ��һ��map�����, ����ʹ������
    // ����ʱ�жϼ���, �����������, �Ǽ���+1

    wstr menuName(100);
    LPCWSTR iniName = wcsrchr(iniFile, '\\');
    if (!iniName) return;   iniName++;
    LPCWSTR pos = wcsrchr(iniName, '.');
    if (!pos) return;
    
    menuName.assign(iniName, pos - iniName);


    HMENU hMenuPop = CreatePopupMenu();
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMenuPop, menuName.c_str());

    CPrivateProfile ini(iniFile, false);
    DWORD count = 0;
    wstr ret = ini.read(0, 0, 0, &count);
    LPCWSTR str = ret.c_str();

    std::map<int, LPCWSTR> map;
    int offset = 0;
    for (DWORD i = 0; i < count; i++)
    {
        // str = ����, ���ݽ�����ȡ ·�� �������, �������Դ��·��
        wstr efile = ini.read(str, L"·��");
        if (!PathFileExistsW(efile.c_str()))
            continue;

        int index = ini.read_int(str, L"����");
        while (map.find(index + offset) != map.end())
            offset++;   // ֻҪmap������������, �Ǻ������еļ�������Ҫ����offset���ƫ����
        
        map[index] = m_MenuTextBuffer.AddString(efile.c_str()); // �ȼ�¼�ļ�·��, ���¸���map����������ӵ��˵�

        size_t len = wcslen(str) + 1;
        str += len;
    }

    int menuIndex = IDMENU_SOURCECODE_START;

    for (auto& item : map)
    {
        LPCWSTR file = item.second;
        _path_dot(file, menuName);
        m_map_path[menuIndex] = file;

        LPCWSTR tips = m_MenuTextBuffer.AddFormat(
            L"��Դ�� %s\r\r\r\r"
            L"ֱ��ѡ�� ����Ŀ��ֱ�Ӵ�, �����½������Դ�\r\r"
            L"Shift + ѡ�� ʼ��ʹ�õ�ǰ�����Դ򿪸�Դ��\r\r"
            L"Ctrl  + ѡ�� ʼ���½�һ�������Դ򿪸�Դ��"
            , file);

        m_map_text[menuIndex] = tips;
        AppendMenuW(hMenuPop, MF_STRING, menuIndex++, menuName.c_str());

        if (menuIndex > IDMENU_SOURCECODE_END)
            break;  // �˵��Ѿ����������ʾ����, ֱ������ѭ��, �����������
    }

}

// ���˵�����ӳ���Դ��
void _insert_menu_SourceCode(HMENU hMenu)
{
    wstr iniFile(260);
    iniFile.assign(s_info->path_iDraw).append(L"eTools\\Data\\Notes\\�ܷ���.ini");

    CPrivateProfile ini(iniFile.c_str(), false);
    DWORD count = 0;
    wstr ret = ini.read(0, 0, 0, &count);
    LPCWSTR str = ret.c_str();

    for (DWORD i = 0; i < count; i++)
    {
        // str = ����, ���ݽ�����ȡ ·�� �������, ���ָ��һ��ini�ļ�, ini�ļ���¼Դ��·����˳��
        wstr file = ini.read(str, L"·��");


        _insert_menu_sourcecode_from_ini(hMenu, file.c_str());
        size_t len = wcslen(str) + 1;
        str += len;
    }



    //wstr tmpPath(260);
    //tmpPath.append(path).append(L"*.ini");

    //WIN32_FIND_DATA fd = { 0 };
    //DWORD dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM;

    //HANDLE hFind = FindFirstFileW(tmpPath.c_str(), &fd);
    //if (INVALID_HANDLE_VALUE == hFind) return;

    //wstr fileName(260);
    //do
    //{
    //    if ((fd.dwFileAttributes & dwFileAttributes) != 0)   // ƥ������
    //    {
    //        fileName.assign(s_info->path_plugin).append(fd.cFileName);



    //    }
    //} while (FindNextFileW(hFind, &fd));
    //FindClose(hFind);

}

int RButton_LoadString(int id, LPWSTR lpBuffer, int cchBufferMax)
{
    auto it = m_map_text.find(id);
    if (it == m_map_text.end())
        return 0;
    LPCWSTR str = it->second;
    if (!str || !str[0])
        return 0;

    size_t ret = wcslen(str);
    if ((int)ret >= cchBufferMax)
        ret = cchBufferMax - 1;
    wmemcpy(lpBuffer, str, ret);
    lpBuffer[ret] = 0;
    return (int)ret;
}

inline bool _open_path(LPCWSTR file, LPCWSTR err)
{
    LPITEMIDLIST pList = ILCreateFromPathW(file);
    if (!pList) return false;

    HRESULT hr = SHOpenFolderAndSelectItems(pList, 0, 0, 0);// isEdit ? OFASI_EDIT : 0);
    if (SUCCEEDED(hr))
    {
        SHFree(pList);
        return true;
    }
    const LPCWSTR fmt = err && err[0] ? L"%s, ��ǰ��λ��·�� = %s" : L"%s��ǰ��λ��·�� = %s";
    if (!err) err = L"";
    wstr text;
    text.Format(fmt, err, file);
    MessageBoxW(s_info->hEWnd, text.c_str(), L"��λ�ļ�ʧ��", 0);
    return false;
}

// ��Դ���ļ�, �ڲ���ȡ����״̬, ���ݰ���״̬ѡ�е�ǰ�����Դ�Դ�뻹���½������Դ�Դ��
inline void RButton_Command_OpenSourceCodeFile(LPCWSTR file)
{
    const short ctrl = GetAsyncKeyState(VK_CONTROL);
    const short shift = GetAsyncKeyState(VK_SHIFT);
    bool isCtrl = ctrl & 0x8000;
    bool isShift = shift & 0x8000;


    //    L"ֱ��ѡ�� ����Ŀ��ֱ�Ӵ�, �����½������Դ�\r\r"
    //    L"Shift + ѡ�� ʼ��ʹ�õ�ǰ�����Դ򿪸�Դ��\r\r"
    //    L"Ctrl  + ѡ�� ʼ���½�һ�������Դ򿪸�Դ��"

    if (!isCtrl && !isShift)
    {
        // ����������û����, Ĭ�ϴ���
        // �жϵ�ǰ������û�д�Դ��, �д򿪾��½����̴�, û�о͵�ǰ���̴�
        HWND hFne = s_info->eWnd->GetWorkProgramTree();
        bool isExis = TreeView_GetCount(hFne) > 1;  // �������Ϳ�, �Ѿ���Դ������½���һ��δ����ĳ���, �����ڴ���Դ��
        // ��ǰ�����Ѿ�����Դ��, �½��̴�, ���ð���״̬, �����洦��
        isCtrl = isExis;
        isShift = !isExis;
    }

    if (!isCtrl && isShift)    // ������shift
    {
        _str fileA = wstr::W2A(file);
        _nes_run_fun(FN_OPEN_FILE2, fileA.c_str(), 0);      // ��ǰ���̴�������Դ��
    }
    else if (isCtrl && !isShift)    // ������ctrl
    {
        wchar_t eFile[260];
        HMODULE hModule = GetModuleHandleW(0);
        GetModuleFileNameW(hModule, eFile, 260);
        process_create(eFile, file, 0);     // �½�һ�������Խ���, �����о���Դ��·��
    }
    else if (isCtrl && isShift)     // ������ ctrl + shift
    {
        //! ��ʱ��û�д���
    }

}

// ����ģ��Ĳ˵�����
int RButton_Command_Module(int id)
{
    if (id < IDMENU_MODULE_START || id > IDMENU_MODULE_END)
        return 0;   // ����δ����, �����ַ�, ֱ���к���ȥ����
    auto it = m_map_path.find(id);
    if (it == m_map_path.end())
        return 1;

    LPCWSTR file = it->second;  // ģ��·��

    const short ctrl = GetAsyncKeyState(VK_CONTROL);
    const short shift = GetAsyncKeyState(VK_SHIFT);
    const bool isCtrl = ctrl & 0x8000;
    const bool isShift = shift & 0x8000;

    //    L"ֱ��ѡ�� ���ģ�鵽��ǰ����\r\r"
    //    L"Ctrl  + ѡ�� �鿴ģ����Ϣ\r\r"

    if (!isCtrl && isShift)    // ������shift
    {
        //! ��ʱ��û�д���
    }
    else if (isCtrl && !isShift)    // ������ctrl
    {
        //TODO ��Ҫ��ģ��鿴���鿴ģ����Ϣ
        wstr ecViewFile(260);
        ecViewFile.assign(s_info->path_iDraw).append(L"eTools\\ViewEcInfo.exe");
        process_create(ecViewFile.c_str(), file, 0);    // ��ģ��·�����ݵ�ģ��鿴��������

    }
    else if (isCtrl && isShift)     // ������ ctrl + shift
    {
        //! ��ʱ��û�д���
    }
    else
    {
        // �������û�а���, Ĭ�ϴ���
        _str fileA = wstr::W2A(file);
        E_IncludeEc(fileA.c_str()); // ����ģ��
    }
    
    return 1;   // �Ѵ���, ��������¼�, ���������·ַ�
}

// ����Դ��Ĳ˵�����
int RButton_Command_SourceCode(int id)
{
    if (id < IDMENU_SOURCECODE_START || id > IDMENU_SOURCECODE_END)
        return RButton_Command_Module(id);
    // ����Դ�����ʷԴ�봦����������ȫһ����
    auto it = m_map_path.find(id);
    if (it == m_map_path.end())
        return 1;
    LPCWSTR file = it->second;
    RButton_Command_OpenSourceCodeFile(file);

    return 1;   // �Ѵ���, ��������¼�, ���������·ַ�
}

// ��ʷ�򿪵�Դ��Ĳ˵�����
int RButton_Command_HistorySourceCode(int id)
{
    if (id < IDMENU_HISTORYCODE_START || id > IDMENU_HISTORYCODE_END)
        return RButton_Command_SourceCode(id);

    // �ߵ������, һ������ʷԴ��Ĳ˵�����, ȡ��Դ��·��, ���ݰ���״ִ̬�в�ͬ����

    auto it = m_map_path.find(id);
    if (it == m_map_path.end())
        return 1;
    LPCWSTR file = it->second;
    RButton_Command_OpenSourceCodeFile(file);
    return 1;   // �Ѵ���, ��������¼�, ���������·ַ�
}

int RButton_Command(int id)
{
    int ret = 1;
    switch (id)
    {
    case IDMENU_LOADDBGPLUGIN:
        s_info->pfnLoadPlugin();
        break;
    case IDMENU_FREEDBGPLUGIN:
        s_info->pfnFreePlugin();
        break;

    case IDMENU_LOCATIONTOOLBAR:
    {
        BOOL isShow = s_info->config->IsShowLocationTool() == false;
        s_info->config->IsShowLocationTool(&isShow);
        if (!isShow)
        {
            static HWND hComboBox1 = s_info->eWnd->GetMenuComboBox1();
            static HWND hComboBox2 = s_info->eWnd->GetMenuComboBox2();
            if (!IsWindow(hComboBox1)) return 0;
            ShowWindowEx(hComboBox1, SW_HIDE);
            ShowWindowEx(hComboBox2, SW_HIDE);
        }
        SendEWindowSize(s_info->pWnd);

        break;
    }
    case IDMENU_LOCATIONFILE:
        _open_path(m_eCodeFileName.c_str(), L"��Դ��Ŀ¼ʧ��");
        break;
    case IDMENU_PATH_E:             // ��������Ŀ¼
        _open_path(s_info->path_e, L"��������Ŀ¼");
        break;
    case IDMENU_PATH_ELIB:          // ��������֧�ֿ�Ŀ¼
        _open_path(s_info->path_lib, L"��������֧�ֿ�Ŀ¼");
        break;
    case IDMENU_PATH_IDRAW:         // ���Ӿ���Ŀ¼
        _open_path(s_info->path_iDraw, L"��������֧�ֿ�Ŀ¼");
        break;
    case IDMENU_PATH_IDRAW_DATA:    // ���Ӿ�������Ŀ¼
        _open_path(s_info->path_data, L"��������֧�ֿ�Ŀ¼");
        break;
    case IDMENU_PATH_CONFIG:        // ���Ӿ��������ļ�Ŀ¼
        _open_path(s_info->path_config, L"���Ӿ��������ļ�Ŀ¼");
        break;
    case IDMENU_PATH_THEME:         // ���Ӿ��⵱ǰ��ɫ�ļ�·��
        MenuEvent_Location_theme_color_config();
        break;
    case IDMENU_PATH_PLUGIN:        // ���Ӿ�����Ŀ¼
        _open_path(s_info->path_plugin, L"���Ӿ�����Ŀ¼");
        break;
    case IDMENU_PATH_ET_PLUGIN:     // �����ֲ��Ŀ¼
        _open_path(s_info->path_ETplugin, L"�����ֲ��Ŀ¼");
        break;

    case IDMENU_COPYECODEFILE:
    {
        MenuEvent_CopySourceCodeFile();
        break;
    }
    default:
        return RButton_Command_HistorySourceCode(id);
    }
    return ret; // �Ѵ���
}

void MenuEvent_Location_theme_color_config()
{
    wstr name = s_info->ini->read(L"Theme", L"name");
    if (name.empty())
    {
        MessageBoxW(s_info->hEWnd, L"��ǰ�����ļ���� [Theme] ���ý��µ� name �������ֵΪ��, �޷���λ������ɫ�ļ�", L"��λʧ��", 0);
        return;
    }
    wstr themeFile = s_info->ini->read(L"Theme", name.c_str());
    if (themeFile.empty())
    {
        wstr dbg;
        dbg.Format(L"��ǰ�����ļ���� [Theme] ���ý��µ� %s �������ֵΪ��, �޷���λ������ɫ�ļ�", name.c_str());
        MessageBoxW(s_info->hEWnd, dbg.c_str(), L"��λʧ��", 0);
        return;
    }

    wstr file(260);
    file.assign(s_info->path_data).append(themeFile);
    _open_path(file.c_str(), L"�򿪵�ǰ������ɫ�ļ�Ŀ¼");

}

void MenuEvent_CopySourceCodeFile()
{
    const short ctrl = GetAsyncKeyState(VK_CONTROL);
    const short shift = GetAsyncKeyState(VK_SHIFT);
    const bool isCtrl = ctrl & 0x8000;
    const bool isShift = shift & 0x8000;
    if (isCtrl && !isShift)     // ֻ��סCtrl, ��Ҫ�����ļ�������׺
    {
        LPCWSTR text = m_eCodeFileName.c_str();
        LPCWSTR pos = wcsrchr(text, '\\');
        if (!pos)pos = wcsrchr(text, '/');
        if (!pos)
        {
            //TODO ����ʧ��, ����ʲô��ʾ����
            return;
        }
        SetClipboard(pos + 1);
    }
    else if (!isCtrl && isShift) // ֻ��סShift��, ��Ҫ�����ļ���������׺
    {
        LPCWSTR text = m_eCodeFileName.c_str();
        LPCWSTR pos = wcsrchr(text, '\\');
        if (!pos)pos = wcsrchr(text, '/');
        if (!pos)
        {
            //TODO ����ʧ��, ����ʲô��ʾ����
            return;
        }
        wstr s(pos + 1);
        text = s.c_str();
        pos = wcsrchr(text, '.');
        if (pos)
            *((LPWSTR)pos) = 0;
        SetClipboard(text);
    }
    else if (isCtrl && isShift) // shift�� �� Ctrl�� ����ס, �ļ�·��ǰ�������
    {
        wstr text(m_eCodeFileName.size() + 2);
        text.assign(L"\"").append(m_eCodeFileName).append(L"\"");
        SetClipboard(text.c_str());

    }
    else
    {
        SetClipboard(m_eCodeFileName.c_str());
    }
}