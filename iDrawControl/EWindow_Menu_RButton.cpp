#include "EMenu.h"
#include <shlwapi.h>
#include <CStringBuffer.h>
#include "EWindow.h"
#include <shlobj_core.h>
#include <assist/assist.h>
#include <process_muster.h>

static std::unordered_map<int, LPCWSTR> m_map_text; // 存放菜单项对应的提示文本
static std::unordered_map<int, LPCWSTR> m_map_path; // 存放菜单项对应的文件路径
static CStringBufferW m_MenuTextBuffer; // 菜单字符串缓冲区
// 往菜单里添加常用模块
void _insert_menu_Module(HMENU hMenu);
// 往菜单里添加常用源码
void _insert_menu_SourceCode(HMENU hMenu);
// 添加历史源码到菜单里
void _insert_menu_HistorySourceCode(HMENU hMenu);
int RButton_LoadString(int id, LPWSTR lpBuffer, int cchBufferMax);

// 菜单事件处理
int RButton_Command(int id);

// 常用模块的菜单处理
int RButton_Command_Module(int id);
// 常用源码的菜单处理
int RButton_Command_SourceCode(int id);
// 历史打开的源码的菜单处理
int RButton_Command_HistorySourceCode(int id);

// 复制源码路径
void MenuEvent_CopySourceCodeFile();
// 菜单事件 定位主题配色文件
void MenuEvent_Location_theme_color_config();


void EMenu_InsertRButton(HMENU hMenuParent)
{
    // 当右键点击的时候清除文本缓冲区
    m_MenuTextBuffer.clear();
    m_map_text.clear();
    m_map_path.clear();

    static bool isFirst;
    if (!isFirst)
    {
        isFirst = true;
        m_MenuTextBuffer.reserve(0x40000);  // 预先分配这么大一块内存, 不够的话这个缓冲区内部会自己申请
        HookLoadString_PushCallback(RButton_LoadString);
        EMenu_PushCommand(RButton_Command);
    }
    //0 "&T.标准工具条", 59392
    //1 "&A.对齐工具条", 133
    //2 "&K.定位工具条", 135
    //3 "&S.状态条", 59393
    //4 "&K.工作窗口切换条", 32963
    //5 "&W.工作夹\tAlt+1", 129
    //6 "&O.状态夹\tAlt+2", 130
    //7 "&C.工作与状态组合夹\tAlt+3", 132
    //8 "&U.窗口组件箱\tAlt+4", 110
    //9 "&R.显示起始页", 32981

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
    InsertMenuW(hMenuParent, 0, flags, IDMENU_LOCATIONTOOLBAR, L"&K.定位工具条");
    AppendMenuW(hMenuParent, MF_SEPARATOR, 0, 0);
    AppendMenuW(hMenuParent, isEnabled, IDMENU_LOCATIONFILE, L"打开源码路径");
    AppendMenuW(hMenuParent, isEnabled, IDMENU_COPYECODEFILE, L"复制源码路径");
    AppendMenuW(hMenuParent, MF_SEPARATOR, 0, 0);

    m_map_text[IDMENU_LOCATIONFILE] = m_MenuTextBuffer.AddString(L"定位当前易语言打开的源码文件\n定位源码文件");
    m_map_text[IDMENU_COPYECODEFILE] = m_MenuTextBuffer.AddString(
        L"①按住Ctrl只复制文件名带后缀\r\r"
        "②按住Shift只复制文件名没有后缀\r\r"
        "③按住Ctrl+Shift复制完整路径前后带引号\r\r"
        "④不按住任何按键复制完整路径\n复制源码文件");



    HMENU hMenuLocation = CreatePopupMenu();
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_E         , L"定位易语言目录");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_ELIB      , L"定位易语言支持库目录");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_IDRAW     , L"定位视觉库目录");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_IDRAW_DATA, L"定位视觉库数据目录");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_CONFIG    , L"定位视觉库配置文件");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_THEME     , L"定位当前主题配色文件");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_PLUGIN    , L"定位视觉库插件目录");
    AppendMenuW(hMenuLocation, MF_STRING, IDMENU_PATH_ET_PLUGIN , L"定位助手插件目录");
    AppendMenuW(hMenuParent, MF_POPUP, (UINT_PTR)hMenuLocation  , L"打开目录");
    AppendMenuW(hMenuParent, MF_SEPARATOR, 0, 0);

    m_map_text[(int)hMenuLocation       ] = m_MenuTextBuffer.AddString(L"定位一些常用的工作目录");
    m_map_text[IDMENU_PATH_E            ] = m_MenuTextBuffer.AddString(L"定位易语言根目录");
    m_map_text[IDMENU_PATH_ELIB         ] = m_MenuTextBuffer.AddString(L"定位易语言支持库目录, 易语言目录\\lib\\");
    m_map_text[IDMENU_PATH_IDRAW        ] = m_MenuTextBuffer.AddString(L"定位视觉库目录, 易语言目录\\lib\\iDraw\\");
    m_map_text[IDMENU_PATH_IDRAW_DATA   ] = m_MenuTextBuffer.AddString(L"定位视觉库数据目录, 易语言目录\\lib\\iDraw\\data\\");
    m_map_text[IDMENU_PATH_CONFIG       ] = m_MenuTextBuffer.AddString(L"定位视觉库配置文件, 易语言目录\\lib\\iDraw\\config.ini");
    m_map_text[IDMENU_PATH_THEME        ] = m_MenuTextBuffer.AddString(L"定位当前主题配色文件\r\r定位到当前使用的主题的配色文件\r\r"
                                                                        "根据 config.ini [Theme] 下的 name 配置项来定位配色文件");
    m_map_text[IDMENU_PATH_PLUGIN       ] = m_MenuTextBuffer.AddString(L"定位视觉库插件目录\r\r"
                                                                        "视觉库的插件目录, 视觉库的插件都是 iDraw_ 开头\r\r"
                                                                        "不是iDraw_开头的插件, 视觉库不会加载");

    m_map_text[IDMENU_PATH_ET_PLUGIN    ] = m_MenuTextBuffer.AddString(L"定位助手插件目录\r\r"
                                                                        "从旧的助手里把插件放到这个目录, 视觉库的助手也可以加载这些插件\r\r"
                                                                        "带界面的插件基本不兼容视觉库的助手, 带界面的插件不要放进来\r\r"
                                                                        "需要使用这个插件的, 请催006更新吧");







    HMENU hMenuSourceCode = CreatePopupMenu();
    HMENU hMenuModule = CreatePopupMenu();
    HMENU hMenuSource = CreatePopupMenu();
    AppendMenuW(hMenuParent, MF_POPUP, (UINT_PTR)hMenuSourceCode, L"历史打开的源码");
    AppendMenuW(hMenuParent, MF_POPUP, (UINT_PTR)hMenuModule, L"常用模块");
    AppendMenuW(hMenuParent, MF_POPUP, (UINT_PTR)hMenuSource, L"常用源码");

    m_map_text[(int)hMenuSourceCode] = m_MenuTextBuffer.AddString(L"根据新建插件记录的历史打开源码列出最近打开的源码");
    m_map_text[(int)hMenuModule] = m_MenuTextBuffer.AddString(L"列出助手设置的常用的模块, 可以快速加载或者查看模块信息");
    m_map_text[(int)hMenuSource] = m_MenuTextBuffer.AddString(L"列出助手设置的常用的源码, 可以使用当前易语言打开源码文件或者新建易语言窗口打开源码文件");


    _insert_menu_HistorySourceCode(hMenuSourceCode);
    _insert_menu_Module(hMenuModule);
    _insert_menu_SourceCode(hMenuSource);


    AppendMenuW(hMenuParent, MF_SEPARATOR, 0, 0);
    //AppendMenuW(hMenuParent, MF_DISABLED, 12, L"常用功能");
    AppendMenuW(hMenuParent, 0, IDMENU_LOADDBGPLUGIN, L"加载调试插件");
    AppendMenuW(hMenuParent, 0, IDMENU_FREEDBGPLUGIN, L"卸载调试插件");
    m_map_text[IDMENU_LOADDBGPLUGIN] = m_MenuTextBuffer.AddString(L"加载 支持库目录\\iDraw\\debug 目录下 iDraw_ 开头的dll文件 \n加载调试插件");
    m_map_text[IDMENU_FREEDBGPLUGIN] = m_MenuTextBuffer.AddString(L"卸载从 支持库目录\\iDraw\\debug 目录下加载的插件, 取消占用, 让插件可以编译出来 \n卸载调试插件");



    //EWINDOW_MENUSTRUCT& menu = s_info->menu;
    //HMENU hMenu = menu.hMenu;
    //if (menu.count != menu.index)   // 后面有增加过菜单项, 需要加入到新的菜单中
    //{
    //    for (int i = menu.count; i < menu.index; i++)
    //    {
    //        UINT uFlags = MF_STRING;
    //        if (menu.menu[i].text[0])
    //        {
    //            if (wcscmp(menu.menu[i].text, L"词库") == 0 || wcscmp(menu.menu[i].text, L"设置") == 0)
    //            {
    //                uFlags = MF_POPUP;
    //            }
    //            AppendMenuW(hMenuParent, uFlags, menu.menu[i].id, menu.menu[i].text);
    //        }
    //    }
    //}


}

// 把一个路径处理一下, 只显示第一层和最后一层路径, 中间使用 ... 显示
inline void _path_dot(LPCWSTR file, wstr& ret)
{
    LPCWSTR pos = wcschr(file + 3, '\\');    // 路径都是c:/xxx 从/后面开始查找
    LPCWSTR pos2 = wcsrchr(file, '\\');      // 倒找反斜杠, 这个要显示到菜单上

    if (!pos || !pos2)
    {
        // 从第二个斜杠开始没有找到斜杠, 那就加入整个路径
        ret.assign(file);
    }
    else
    {
        ret.assign(file);
        ret.assign(file, pos - file + 1);
        ret.append(L"...").append(pos2);                // c:xxx\...\xxx.e
    }
}

// 把一个路径处理一下, 只显示第一层和最后一层路径, 中间使用 ... 显示
inline void _path_dot(LPCSTR file, _str& ret)
{
    LPCSTR pos = strchr(file + 3, '\\');    // 路径都是c:/xxx 从/后面开始查找
    LPCSTR pos2 = strrchr(file, '\\');      // 倒找反斜杠, 这个要显示到菜单上

    if (!pos || !pos2)
    {
        // 从第二个斜杠开始没有找到斜杠, 那就加入整个路径
        ret.assign(file);
    }
    else
    {
        ret.assign(file);
        ret.assign(file, pos - file + 1);
        ret.append("...").append(pos2);                // c:xxx\...\xxx.e
    }
}

// 添加历史源码到菜单里
void _insert_menu_HistorySourceCode(HMENU hMenu)
{
    _str data;
    wstr file(260);
    file.assign(s_info->path_ETplugin).append(L"新建增强插件\\Data\\History.txt");
    if (!_get_file_data(file.c_str(), data))
        return;
    LPWSTR ptr = (LPWSTR)m_MenuTextBuffer.AddString(data.a2w().c_str());
    LPWSTR name = ptr, time = 0;
    wstr tmpName(260);  // 拼接菜单标题使用的字符串
    int index = IDMENU_HISTORYCODE_START;   // 起始菜单ID
    while (*ptr)
    {
        wchar_t& ch = *ptr++;
        if (ch == '|')
        {
            // '|' 后面开始是时间, 判断文件是否存在, 然后添加到菜单里
            // 需要对文件路径进行处理, 只显示 c:/xxx/.../xxx.e
            time = ptr;
            ch = '\0';

            if (!PathFileExistsW(name))
                continue;
            // 文件存在, 往菜单里添加项目
            _path_dot(name, tmpName);

            LPCWSTR pos3 = wcschr(time, '\r');
            if (!pos3)
                pos3 = wcsrchr(time, '\n');
            size_t appendTimeSize = pos3 ? pos3 - time : wcslen(time);
            //tmpName.append(L"\t").append(time, appendTimeSize);

            LPCWSTR menuText = m_MenuTextBuffer.AddString(tmpName.c_str());

            tmpName.assign(time, appendTimeSize);
            LPCWSTR tips = m_MenuTextBuffer.AddFormat(
                L"打开源码 %s\r\r\r\r"
                L"直接选中 空项目则直接打开, 否则新建易语言打开\r\r"
                L"Shift + 选中 始终使用当前易语言打开该源码\r\r"
                L"Ctrl  + 选中 始终新建一个易语言打开该源码"
                , name, tmpName.c_str());
            m_map_text[index] = tips;
            m_map_path[index] = m_MenuTextBuffer.AddString(name);

            AppendMenuW(hMenu, MF_STRING, index++, menuText);
            if (index > IDMENU_HISTORYCODE_END)
                break;  // 菜单已经超过最大显示数量, 直接跳出循环, 不继续添加了

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



// 往菜单里添加常用模块
void _insert_menu_Module(HMENU hMenu)
{
    typedef LPINT(__stdcall* pfn_E_GetEList)(LPCSTR dbFile, LPCSTR dllFile, LPCSTR** pRetFl);
    typedef struct _tagMODULE_INFO
    {
        int         id;         // id
        LPCSTR      name;       // 文件名
        LPCSTR      path;       // 路径
        int         sort;       // 排序
        int         favorite;   // 收藏
        int         fenlei;     // 分类ID, 父级的分类id
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
    int nArrCount = *(pArr - 1);    // 成员数

    for (int i = 0; i < nArrCount; i++)
    {
        LPCSTR flName = fl[i];
        if (!flName || !flName[0])
            continue;

        HMENU hMenuPop = CreatePopupMenu();
        AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hMenuPop, flName);       // 添加分类弹出式菜单

        LPINT pItemArr = (LPINT)pArr[i];
        if (!pItemArr) continue;
        int nItemCount = *(pItemArr - 1);
        for (int n = 0; n < nItemCount; n++)
        {
            PMODULE_INFO item = (PMODULE_INFO)pItemArr[n];
            if (!item || !PathFileExistsA(item->path)) continue;
            // 这里把路径加入到菜单里, 需要处理一下路径
            _path_dot(item->path, name);

            wstr pathW = _str::A2W(item->path);
            LPCWSTR tips = m_MenuTextBuffer.AddFormat(
                L"%s\r\r\r\r"
                L"直接选中 添加模块到当前工程\r\r"
                L"Ctrl  + 选中 查看模块信息\r\r"
                , pathW.c_str());

            m_map_text[menuIndex] = tips;
            m_map_path[menuIndex] = m_MenuTextBuffer.AddString(pathW.c_str());
            AppendMenuA(hMenuPop, MF_STRING, menuIndex++, name.c_str());
            if (menuIndex > IDMENU_MODULE_END)
                break;  // 菜单已经超过最大显示数量, 直接跳出循环, 不继续添加了
        }
        if (menuIndex > IDMENU_MODULE_END)
            break;  // 菜单已经超过最大显示数量, 直接跳出循环, 不继续添加了
    }

}

// 从配置文件里添加常用源码
void _insert_menu_sourcecode_from_ini(HMENU hMenu, LPCWSTR iniFile)
{
    // iniFile 指向记录源码路径的ini文件, 需要读取路径出来
    // 里面有个索引, 根据索引排序, 使用一个map来存放, 键名使用索引
    // 加入时判断键名, 如果键名存在, 那键名+1

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
        // str = 节名, 根据节名获取 路径 这个项名, 这个就是源码路径
        wstr efile = ini.read(str, L"路径");
        if (!PathFileExistsW(efile.c_str()))
            continue;

        int index = ini.read_int(str, L"索引");
        while (map.find(index + offset) != map.end())
            offset++;   // 只要map里存在这个键名, 那后面所有的键名都需要增加offset这个偏移量
        
        map[index] = m_MenuTextBuffer.AddString(efile.c_str()); // 先记录文件路径, 等下根据map的排序来添加到菜单

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
            L"打开源码 %s\r\r\r\r"
            L"直接选中 空项目则直接打开, 否则新建易语言打开\r\r"
            L"Shift + 选中 始终使用当前易语言打开该源码\r\r"
            L"Ctrl  + 选中 始终新建一个易语言打开该源码"
            , file);

        m_map_text[menuIndex] = tips;
        AppendMenuW(hMenuPop, MF_STRING, menuIndex++, menuName.c_str());

        if (menuIndex > IDMENU_SOURCECODE_END)
            break;  // 菜单已经超过最大显示数量, 直接跳出循环, 不继续添加了
    }

}

// 往菜单里添加常用源码
void _insert_menu_SourceCode(HMENU hMenu)
{
    wstr iniFile(260);
    iniFile.assign(s_info->path_iDraw).append(L"eTools\\Data\\Notes\\总分类.ini");

    CPrivateProfile ini(iniFile.c_str(), false);
    DWORD count = 0;
    wstr ret = ini.read(0, 0, 0, &count);
    LPCWSTR str = ret.c_str();

    for (DWORD i = 0; i < count; i++)
    {
        // str = 节名, 根据节名获取 路径 这个项名, 这个指向一个ini文件, ini文件记录源码路径和顺序
        wstr file = ini.read(str, L"路径");


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
    //    if ((fd.dwFileAttributes & dwFileAttributes) != 0)   // 匹配类型
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
    const LPCWSTR fmt = err && err[0] ? L"%s, 当前定位的路径 = %s" : L"%s当前定位的路径 = %s";
    if (!err) err = L"";
    wstr text;
    text.Format(fmt, err, file);
    MessageBoxW(s_info->hEWnd, text.c_str(), L"定位文件失败", 0);
    return false;
}

// 打开源码文件, 内部获取按键状态, 根据按键状态选中当前易语言打开源码还是新建易语言打开源码
inline void RButton_Command_OpenSourceCodeFile(LPCWSTR file)
{
    const short ctrl = GetAsyncKeyState(VK_CONTROL);
    const short shift = GetAsyncKeyState(VK_SHIFT);
    bool isCtrl = ctrl & 0x8000;
    bool isShift = shift & 0x8000;


    //    L"直接选中 空项目则直接打开, 否则新建易语言打开\r\r"
    //    L"Shift + 选中 始终使用当前易语言打开该源码\r\r"
    //    L"Ctrl  + 选中 始终新建一个易语言打开该源码"

    if (!isCtrl && !isShift)
    {
        // 两个按键都没按下, 默认处理
        // 判断当前程序有没有打开源码, 有打开就新建进程打开, 没有就当前进程打开
        HWND hFne = s_info->eWnd->GetWorkProgramTree();
        bool isExis = TreeView_GetCount(hFne) > 1;  // 程序树型框, 已经打开源码或者新建了一个未保存的程序, 都属于打开了源码
        // 当前程序已经打开了源码, 新进程打开, 设置按键状态, 让下面处理
        isCtrl = isExis;
        isShift = !isExis;
    }

    if (!isCtrl && isShift)    // 按下了shift
    {
        _str fileA = wstr::W2A(file);
        _nes_run_fun(FN_OPEN_FILE2, fileA.c_str(), 0);      // 当前进程打开易语言源码
    }
    else if (isCtrl && !isShift)    // 按下了ctrl
    {
        wchar_t eFile[260];
        HMODULE hModule = GetModuleHandleW(0);
        GetModuleFileNameW(hModule, eFile, 260);
        process_create(eFile, file, 0);     // 新建一个易语言进程, 命令行就是源码路径
    }
    else if (isCtrl && isShift)     // 按下了 ctrl + shift
    {
        //! 暂时还没有处理
    }

}

// 常用模块的菜单处理
int RButton_Command_Module(int id)
{
    if (id < IDMENU_MODULE_START || id > IDMENU_MODULE_END)
        return 0;   // 返回未处理, 继续分发, 直到有函数去处理
    auto it = m_map_path.find(id);
    if (it == m_map_path.end())
        return 1;

    LPCWSTR file = it->second;  // 模块路径

    const short ctrl = GetAsyncKeyState(VK_CONTROL);
    const short shift = GetAsyncKeyState(VK_SHIFT);
    const bool isCtrl = ctrl & 0x8000;
    const bool isShift = shift & 0x8000;

    //    L"直接选中 添加模块到当前工程\r\r"
    //    L"Ctrl  + 选中 查看模块信息\r\r"

    if (!isCtrl && isShift)    // 按下了shift
    {
        //! 暂时还没有处理
    }
    else if (isCtrl && !isShift)    // 按下了ctrl
    {
        //TODO 需要打开模块查看器查看模块信息
        wstr ecViewFile(260);
        ecViewFile.assign(s_info->path_iDraw).append(L"eTools\\ViewEcInfo.exe");
        process_create(ecViewFile.c_str(), file, 0);    // 把模块路径传递到模块查看器进程里

    }
    else if (isCtrl && isShift)     // 按下了 ctrl + shift
    {
        //! 暂时还没有处理
    }
    else
    {
        // 否则就是没有按下, 默认处理
        _str fileA = wstr::W2A(file);
        E_IncludeEc(fileA.c_str()); // 引用模块
    }
    
    return 1;   // 已处理, 拦截这个事件, 不继续往下分发
}

// 常用源码的菜单处理
int RButton_Command_SourceCode(int id)
{
    if (id < IDMENU_SOURCECODE_START || id > IDMENU_SOURCECODE_END)
        return RButton_Command_Module(id);
    // 常用源码和历史源码处理流程是完全一样的
    auto it = m_map_path.find(id);
    if (it == m_map_path.end())
        return 1;
    LPCWSTR file = it->second;
    RButton_Command_OpenSourceCodeFile(file);

    return 1;   // 已处理, 拦截这个事件, 不继续往下分发
}

// 历史打开的源码的菜单处理
int RButton_Command_HistorySourceCode(int id)
{
    if (id < IDMENU_HISTORYCODE_START || id > IDMENU_HISTORYCODE_END)
        return RButton_Command_SourceCode(id);

    // 走到这里的, 一定是历史源码的菜单项被点击, 取出源码路径, 根据按键状态执行不同操作

    auto it = m_map_path.find(id);
    if (it == m_map_path.end())
        return 1;
    LPCWSTR file = it->second;
    RButton_Command_OpenSourceCodeFile(file);
    return 1;   // 已处理, 拦截这个事件, 不继续往下分发
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
        _open_path(m_eCodeFileName.c_str(), L"打开源码目录失败");
        break;
    case IDMENU_PATH_E:             // 打开易语言目录
        _open_path(s_info->path_e, L"打开易语言目录");
        break;
    case IDMENU_PATH_ELIB:          // 打开易语言支持库目录
        _open_path(s_info->path_lib, L"打开易语言支持库目录");
        break;
    case IDMENU_PATH_IDRAW:         // 打开视觉库目录
        _open_path(s_info->path_iDraw, L"打开易语言支持库目录");
        break;
    case IDMENU_PATH_IDRAW_DATA:    // 打开视觉库数据目录
        _open_path(s_info->path_data, L"打开易语言支持库目录");
        break;
    case IDMENU_PATH_CONFIG:        // 打开视觉库配置文件目录
        _open_path(s_info->path_config, L"打开视觉库配置文件目录");
        break;
    case IDMENU_PATH_THEME:         // 打开视觉库当前配色文件路径
        MenuEvent_Location_theme_color_config();
        break;
    case IDMENU_PATH_PLUGIN:        // 打开视觉库插件目录
        _open_path(s_info->path_plugin, L"打开视觉库插件目录");
        break;
    case IDMENU_PATH_ET_PLUGIN:     // 打开助手插件目录
        _open_path(s_info->path_ETplugin, L"打开助手插件目录");
        break;

    case IDMENU_COPYECODEFILE:
    {
        MenuEvent_CopySourceCodeFile();
        break;
    }
    default:
        return RButton_Command_HistorySourceCode(id);
    }
    return ret; // 已处理
}

void MenuEvent_Location_theme_color_config()
{
    wstr name = s_info->ini->read(L"Theme", L"name");
    if (name.empty())
    {
        MessageBoxW(s_info->hEWnd, L"当前配置文件里的 [Theme] 配置节下的 name 配置项的值为空, 无法定位主题配色文件", L"定位失败", 0);
        return;
    }
    wstr themeFile = s_info->ini->read(L"Theme", name.c_str());
    if (themeFile.empty())
    {
        wstr dbg;
        dbg.Format(L"当前配置文件里的 [Theme] 配置节下的 %s 配置项的值为空, 无法定位主题配色文件", name.c_str());
        MessageBoxW(s_info->hEWnd, dbg.c_str(), L"定位失败", 0);
        return;
    }

    wstr file(260);
    file.assign(s_info->path_data).append(themeFile);
    _open_path(file.c_str(), L"打开当前主题配色文件目录");

}

void MenuEvent_CopySourceCodeFile()
{
    const short ctrl = GetAsyncKeyState(VK_CONTROL);
    const short shift = GetAsyncKeyState(VK_SHIFT);
    const bool isCtrl = ctrl & 0x8000;
    const bool isShift = shift & 0x8000;
    if (isCtrl && !isShift)     // 只按住Ctrl, 需要复制文件名带后缀
    {
        LPCWSTR text = m_eCodeFileName.c_str();
        LPCWSTR pos = wcsrchr(text, '\\');
        if (!pos)pos = wcsrchr(text, '/');
        if (!pos)
        {
            //TODO 复制失败, 得有什么提示才行
            return;
        }
        SetClipboard(pos + 1);
    }
    else if (!isCtrl && isShift) // 只按住Shift键, 需要复制文件名不带后缀
    {
        LPCWSTR text = m_eCodeFileName.c_str();
        LPCWSTR pos = wcsrchr(text, '\\');
        if (!pos)pos = wcsrchr(text, '/');
        if (!pos)
        {
            //TODO 复制失败, 得有什么提示才行
            return;
        }
        wstr s(pos + 1);
        text = s.c_str();
        pos = wcsrchr(text, '.');
        if (pos)
            *((LPWSTR)pos) = 0;
        SetClipboard(text);
    }
    else if (isCtrl && isShift) // shift键 和 Ctrl键 都按住, 文件路径前后加引号
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