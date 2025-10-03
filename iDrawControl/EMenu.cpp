#include "EMenu.h"
#include "EWindow.h"
#include <hook_detours/apiHook.h>
#include <control/CShadow.h>
#include <vector>
#include <set>
#include <WaitObject.h>
#include <shlobj_core.h>
#include <shlwapi.h>
#include <struct_simpleMempool.h>
#include <iDraw_const.h>


// ��̬��ӹ������Ĳ˵�ID����
static mystruct::simpleMempool<LPWSTR> s_pool;
static mystruct::simpleMempool<MENUEXT_ID_INFO> s_pool_ext;
static HMENU s_popMenu;         // ��ǰ�����Ĳ˵����
static LPOBJSTRUCT s_pData;
DWORD EMenu_PopUp(LPOBJSTRUCT pData);
static int s_offsetDx;          // DXƫ��, ��DX��ȾֵΪ1, gdi+��ȾΪ0
static bool m_isEndMenu;        // �Ƿ��ֶ������� EndMenu
static EWINDOW_MENUSTRUCT* m_menu;
static std::vector<EMENU_DATA> m_menu_etool;    // ���ֵĲ˵���, ����������, �����ǻ滭�����
static std::unordered_map<int, LPWSTR*> m_pool_id_menu_tool;    // ���ù��߲˵�id, ��ѯĳ��ֵ�Ƿ�Ϊ���߲˵��ʹ��������
static std::unordered_map<int, LPMENUEXT_ID_INFO> m_pool_id_menu_ext;     // ��չ����˵�id, ��ѯĳ��ֵ�Ƿ�Ϊ��չ�˵��ʹ��������
static std::unordered_map<HBITMAP, LPEX_IMAGE> m_menu_bitmap;   // �Ӳ˵��Դ���λͼ�ﴴ����dxͼƬ
static HMENU s_hMenu_Window;    // ���ڵĲ˵�, �Ǹ��Ա����ӻ���뵽����, ��Ҫ����
static HMENU s_hMenu_Help;      // �����Ĳ˵�, ��Ҫ���Ա����Ӽ��뵽����˵�����
int CALLBACK IDraw_Menu_Function(UINT nCode, WPARAM wParam, LPARAM lParam);

// ����˵������ڶ����Ĳ˵�����
// id = �˵�id, ͨ�����id�������ĸ���ť
// lpszMenuText = ��ʾ�Ĳ˵��ı�
// index = Ҫ���뵽�ĸ�λ��, ��10��ʼ, -1����뵽���, ǰ��10���˵����������Դ���, ���������10��ʼ
// hPopupMenu = �Ƿ�Ҫ�����˵�, �����Ҫ�����˵�, ��Ҫ���ݲ˵����, ��������ťʱ�ᵯ���˵�, ����0�Ļ�����ᴥ������¼�
// isPopup = �Ƿ񵯳��˵�, ���Ϊ��, ������꾭����ʱ��ᵯ���˵�
void __menu_insert(int id, LPCWSTR lpszMenuText, int index, HMENU hPopupMenu, bool isPopup);
void __menu_remove(int id, bool isIndex);



EWINDOW_MENUSTRUCT* _menu_getindex()
{
    return m_menu;
}

inline LPBYTE CWinApp_GetFirstDocTemplate(LPBYTE pThis)
{
    if (!pThis)return 0;
    LPBYTE pDocManager = (LPBYTE)(*((int*)(pThis + 128)));
    int pTemplateList = (int)pDocManager + 4;

    if (pTemplateList < 5) return 0;

    LPBYTE pNode = (LPBYTE)(*((int*)(pTemplateList + 4)));
    LPBYTE pDocTemplate = 0;
    if (pNode)
        pDocTemplate = (LPBYTE)(*((int*)(pNode + 8)));
    return pDocTemplate;
}


inline LPBYTE __find_address(const void* pBegin, const void* pEnd, const void* findData, int findDataSize)
{
    for (LPBYTE p = (LPBYTE)pBegin; p != pEnd; p++)
    {
        if (memcmp(p, findData, findDataSize) == 0)
            return p;
    }
    return 0;
}
inline HMENU _get_e_menu(LPBYTE pBegin, LPBYTE pEnd, LPBYTE& pIDE)
{
    const unsigned char findData[] = { 95, 94, 93, 139, 76, 36, 36, 100, 137, 13, 0, 0, 0, 0, 131, 196 };
    LPBYTE address = __find_address(pBegin, pEnd, findData, sizeof(findData));
    pIDE = (LPBYTE)(*((int*)(address - 9)));

    LPBYTE pMainWnd = (LPBYTE)(*((int*)(pIDE + 28)));
    HMENU hMenu = (HMENU)(*((int*)(pMainWnd + 68)));
    return hMenu;
}


// ��ѯָ��ֵ�Ƿ��ǹ��߲˵�ID, ���ز˵�ID�������ļ�·��, ���ǹ��߲˵�����0
LPCWSTR _menu_istool_(WPARAM id)
{
    auto it = m_pool_id_menu_tool.find(id);
    if (it == m_pool_id_menu_tool.end())
        return 0;
    LPWSTR* p = it->second;
    return *p;
}

LPMENUEXT_ID_INFO _menu_isext_(WPARAM id)
{
    auto it = m_pool_id_menu_ext.find(id);
    if (it == m_pool_id_menu_ext.end())
        return 0;
    return it->second;
}

// ������չ�˵�ID, ����������һ���ڴ�, �ڴ��ַ��Ϊ�˵�id, ���û�����Ĳ˵�IDд�������ַ
// �ڴ����˵��¼���ʱ������IDȡ����
inline UINT_PTR _menu_alloc_ext(int id, LPARAM param, LPPLUGIN_INFO info)
{
    LPMENUEXT_ID_INFO p = s_pool_ext.alloc(true);
    p->id = id;
    p->param = param;
    p->info = info;
    m_pool_id_menu_ext[(int)p] = p;
    return (UINT_PTR)p;
}

// ���빤�߲˵�, ���صĲ˵�ID��һ���ڴ��ַ, �ڴ��ַ���һ���ַ���ָ��, ��Ҫ�ͷ�����ַ���
inline UINT_PTR _menu_alloc_tool(LPCWSTR str)
{
    LPWSTR* p = s_pool.alloc(true);
    *p = wstr::copy(str);
    m_pool_id_menu_tool[(int)p] = p;
    return (UINT_PTR)p;
}
inline static bool IsEToolMenu(HMENU hMenu, int isSet = 0)
{
    static int count = 0;
    static HMENU* hMenus = 0;
    if (isSet)
    {
        count = isSet;
        hMenus = (HMENU*)hMenu;
        return false;
    }
    for (int i = 0; i < count; i++)
    {
        if (hMenu == hMenus[i])
            return true;
    }
    return false;
}
static EMENU_DATA* _menu_get_data(int index)
{
    if (index < 0)
        return 0;

    int size = (int)m_menu->menu.size();
    if (index < size)
        return &m_menu->menu[index];

    index -= size;
    size = (int)m_menu->menu_etool.size();
    if (index >= size)
        return 0;
    return &m_menu->menu_etool[index];
}

inline void __menu_insert_tool_(HMENU hMenu)
{
    //&T.����
    DWORD count = 0;
    wstr ret = s_info->ini->read(L"Tools", 0, 0, &count);
    wstr item(50);
    if (count == 0)
    {
        // xmlû�д�������ڵ�, �ֶ�����ToolsĿ¼������û���⼸�����ù���
        wstr path1(260);
        wstr path2(260);
        wstr path3(260);
        path1.assign(s_info->path_iDraw).append(L"Tools\\guidgen.exe");
        bool guid = PathFileExistsW(path1.c_str());

        path2.assign(s_info->path_iDraw).append(L"Tools\\errlook.exe");
        bool err = PathFileExistsW(path2.c_str());

        path3.assign(s_info->path_iDraw).append(L"Tools\\spyxx.exe");
        bool spy = PathFileExistsW(path3.c_str());

        if (guid || err || spy)
        {
            int i = 0;
            _menu_add(hMenu, 0, 0, MF_SEPARATOR);
            if (guid)
            {
                UINT_PTR id = _menu_alloc_tool(path1.c_str());
                _menu_add(hMenu, id, L"���� GUID(&G)");
                item.assign(L"Item").append(++i);
                s_info->ini->write(L"Tools", item.c_str(), L"���� GUID(&G)|Tools\\guidgen.exe");
            }
            if (err)
            {
                UINT_PTR id = _menu_alloc_tool(path2.c_str());
                _menu_add(hMenu, id, L"�������(&K)");
                item.assign(L"Item").append(++i);
                s_info->ini->write(L"Tools", item.c_str(), L"�������(&K)|Tools\\errlook.exe");
            }
            if (spy)
            {
                UINT_PTR id = _menu_alloc_tool(path3.c_str());
                _menu_add(hMenu, id, L"Spy++(&+)");
                item.assign(L"Item").append(++i);
                s_info->ini->write(L"Tools", item.c_str(), L"Spy++(&+)|Tools\\spyxx.exe");
            }
        }
        return;
    }

    wstr path(260);
    wstr* menuName = new wstr[count];
    UINT_PTR* menuid = new UINT_PTR[count];
    int n = 0;
    for (DWORD i = 0; i < count; i++)
    {
        item.assign(L"Item").append(i + 1);
        wstr iniValue = s_info->ini->read(L"Tools", item.c_str());
        LPWSTR strLeft_menuname = iniValue.data();

        size_t strLen = wcslen(strLeft_menuname);
        LPWSTR fileName =  wcschr(strLeft_menuname, '|');
        if (fileName)
        {
            *fileName++ = 0;
            path.assign(s_info->path_iDraw).append(fileName);
            if (PathFileExistsW(path.c_str()))
            {
                menuid[n] = _menu_alloc_tool(path.c_str());
                menuName[n].assign(strLeft_menuname);
                n++;
            }
        }
        fileName += strLen + 1;
    }

    if (n > 0)
    {
        _menu_add(hMenu, 0, 0, MF_SEPARATOR);
        for (int i = 0; i < n; i++)
        {
            //wstr::dbg(L"����˵���, id = %d, ��λ = %d, ��λ = %d\n", menuid[i], LOWORD(menuid[i]), HIWORD(menuid[i]));
            _menu_add(hMenu, menuid[i], menuName[i].c_str());
            item.assign(L"Item").append(i + 1);

        }
    }

    delete[] menuid;
    delete[] menuName;
}

int __menu_calc_rect(HDC hdc, EWINDOW_MENUSTRUCT& data, bool first)
{
    RECT rcText = { 0 };
    const int caption_height = s_info->scale(WINDOW_CAPTION_HEIGHT);
    const int offset_width = s_info->scale(10);
    int width = caption_height;
    MENUITEMINFOW mii = { 0 };
    wchar_t buf[256] = { 0 };
    BOOL isInsertHelp = FALSE;

    for (int i = 0; i < (int)data.menu.size(); i++)
    {
        rcText = { 0 , 0, 0, 0 };
        EMENU_DATA& menu = data.menu[i];
        RECT& rcItem = menu.rcItem;
        int old_width = width;
        if (menu.isHide == false && menu.text[0])
        {
            DrawTextW(hdc, menu.text, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
            rcItem.left = width;
            rcItem.right = rcItem.left + rcText.right - rcText.left + offset_width;
            rcItem.top = 0;
            rcItem.bottom = caption_height;
            width = rcItem.right;

        }
        else
        {
            rcItem = { 0 };
            //if (i != 9)
            //    width += offset_width;
        }

        if (!first)
            continue;   // ���ǵ�һ�μ���ľͼ����Ӳ˵���

        menu.isPopup = i < 11;
        if (wcscmp(menu.text, L"&T.����") == 0)       //1  &T.����
        {
            __menu_insert_tool_(menu.hMenu);
        }
        else if (wcscmp(menu.text, L"&V.�鿴") == 0)  // 2  &V.�鿴
        {
            DeleteMenu(menu.hMenu, 0, MF_BYPOSITION);   // �鿴��ǰ�����˵����ǵ������ֵ�, ��Ҫɾ��
            DeleteMenu(menu.hMenu, 0, MF_BYPOSITION);
        }
        else if (wcscmp(menu.text, L"&W.����") == 0)  // 9   &W.����
        {
            menu.text[0] = 0;
            width = old_width;
            menu.isPopup = false;
            s_hMenu_Window = menu.hMenu;
        }
        else if (wcscmp(menu.text, L"&H.����") == 0)  //10  &H.����
        {
            _menu_add(menu.hMenu, 0, 0, MF_SEPARATOR);
            _menu_add(menu.hMenu, IDMENU_UNINSERT, L"ͣ���Ӿ���", 0);
            _menu_add(menu.hMenu, IDMENU_AD, L"���", 0);
            if(s_hMenu_Window)
                s_hMenu_Help = menu.hMenu;
        }
    }

    for (EMENU_DATA& item : data.menu_etool)
    {
        DrawTextW(hdc, item.text, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
        RECT& rcItem = item.rcItem;

        rcItem.left = width;
        rcItem.right = rcItem.left + rcText.right - rcText.left + offset_width;
        rcItem.top = 0;
        rcItem.bottom = caption_height;
        width = rcItem.right;
    }
    return width;
}
void EMenu_Get_Help_Wnd(HMENU& hMenuWnd, HMENU& hMenuHelp)
{
    hMenuWnd = s_hMenu_Window;
    hMenuHelp = s_hMenu_Help;
}
inline void _menu_init_s_info(EWINDOW_MENUSTRUCT& menu, HMENU hMenu)
{
    menu.hMenu = hMenu;
    EMenu_InsertMenu(menu.hMenu);

    int count = GetMenuItemCount(menu.hMenu);
    menu.menu.resize(count);
    modify_menu_style(menu.hMenu, &menu.menu[0], count);
}

inline void IDraw_Menu_Destroy()
{
    s_pool.uninit();
    s_pool_ext.uninit();
    m_pool_id_menu_tool.clear();
    m_pool_id_menu_ext.clear();

    for (auto it = m_menu_bitmap.begin(); it != m_menu_bitmap.end(); ++it)
        _img_destroy(it->second);
    m_menu_bitmap.clear();
    m_menu = 0;

    s_info->menu.menu.clear();
    s_info->menu.menu_etool.clear();

    s_info->menuDoc.menu.clear();
    s_info->menuDoc.menu_etool.clear();

}
static void _menu_get_info()
{
    _str menu_buf;
    std::vector<LPCWSTR> arr_menu;
    CustomMenu_GetMenuList(menu_buf, arr_menu);

    auto pfn_find = [&arr_menu](LPCWSTR str) -> bool
    {
        for (LPCWSTR menu_text : arr_menu)
        {
            if(wcscmp(menu_text, str) == 0)
                return true;
        }
        return false;
    };

    for (EMENU_DATA& menu : m_menu->menu)
    {
        menu.isHide = pfn_find(menu.text);
    }
}

LPOBJSTRUCT _init_menu_(LPOBJSTRUCT pWindowData)
{
    IDraw_PushPluginFreeCallback(IDraw_Menu_Destroy);
    static _str pool_tool_menu(0x8000);
    static _str pool_menuext(0x8000);



    //MessageBoxW(0, L"1", 0, 0);
    s_pool.init(pool_tool_menu.data() + 0x15c, 0x8000 - 0x15c);
    s_pool_ext.init(pool_menuext.data() + 0x3a3, 0x8000 - 0x3a3);

    s_offsetDx = s_info->isDxRender ? 1 : 0;

    //! ��ʼ����չ�˵�
    s_info->hMenuExtPopup = CreatePopupMenu();
    _menu_add(s_info->hMenuExtPopup, _menu_alloc_ext(1, 0, 0), L"������չ(&M)", MF_STRING);
    _menu_add(s_info->hMenuExtPopup, _menu_alloc_ext(2, 0, 0), L"�Զ���˵�(&C)...", MF_STRING);

    //s_info->pfnMenuExt_Delete = MenuExt_Delete;

    HINSTANCE hInst = CWndBase::GetInst();
    s_pData = pWindowData;

    LPBYTE pBegin = (LPBYTE)GetModuleHandleW(0);
    LPBYTE pEnd = pBegin + 1376256;
    LPBYTE pIDE;
    
    HMENU hMenu = _get_e_menu(pBegin, pEnd, pIDE);
    //_menu_insert(hMenu, 8, (UINT_PTR)s_info->hMenuExtPopup, L"&X.��չ", MF_BYPOSITION | MF_POPUP);
    _menu_init_s_info(s_info->menu, hMenu);


    LPBYTE pMultiDocTemplate = CWinApp_GetFirstDocTemplate(pIDE);
    if (!pMultiDocTemplate)
    {
        MessageBoxW(0, L"��ȡ�˵�ʧ��, �޷�����", L"����", 0);
        throw;
    }

    hMenu = (HMENU)(*((int*)(pMultiDocTemplate + 100)));
    _menu_insert(hMenu, 8, (UINT_PTR)s_info->hMenuExtPopup, L"&X.��չ", MF_BYPOSITION | MF_POPUP);
    _menu_init_s_info(s_info->menuDoc, hMenu);

    //SetMenu(s_info->hEWnd, hMenu);
    m_menu = &s_info->menuDoc;



    HDC hdc = CreateCompatibleDC(0);
    SelectObject(hdc, CWndBase::DefaultFont());
    //int width = __menu_calc_rect(hdc, s_info->menu, true);
    int width = __menu_calc_rect(hdc, s_info->menuDoc, true);
    _menu_get_info();
    DeleteDC(hdc);

    //int width = WINDOW_CAPTION_HEIGHT + s_info->menu.count * WINDOW_MENU_WIDTH;
    s_info->rcMenu = { WINDOW_CAPTION_HEIGHT, 1, width, WINDOW_CAPTION_HEIGHT - 1 };

    _apihook_menu_init(hInst);
    return 0;
}

void EMenu_MenuBarChange()
{
    HDC hdc = CreateCompatibleDC(0);
    LPEX_FONT hFont = g_scale.GetFont(s_info->defData->draw.hFont);
    SelectObject(hdc, hFont->GetFont());
    int width = __menu_calc_rect(hdc, *m_menu, false);
    s_info->rcMenu = { WINDOW_CAPTION_HEIGHT, 1, width, WINDOW_CAPTION_HEIGHT - 1 };
    DeleteDC(hdc);
    _window_recalc_title(s_info->pWnd, true);
}


inline void EMenu_InvalidateItem(int index, LPOBJSTRUCT pData)
{
    if (index == -1)
        return;
    auto menu = _menu_get_data(index);
    if (!menu)
        return;
    ::InvalidateRect(pData->hWnd, &menu->rcItem, 0);
}

void CALLBACK TimerProc_EWindowMenu(HWND hWnd, UINT message, UINT_PTR id, DWORD timer)
{
    POINT pt, ptClient;
    GetCursorPos(&ptClient); pt = ptClient;
    ScreenToClient(hWnd, &ptClient);
    if (!PtInRect(&s_info->rcMenu, ptClient))
    {
        KillTimer(hWnd, id);
        LPOBJSTRUCT pData = GetDataFromHwnd(hWnd);
        if (pData->draw.indexHot == -1)
        {
            //wstr::dbg(L"����뿪, ��ǰ�˵����� = %d, %d\n", pData->draw.indexHot, pData->draw.indexDown);
            InvalidateRect(hWnd, &s_info->rcMenu, 0);
        }
        else
        {
            WndProc_Custom_Menu(hWnd, WM_MOUSELEAVE, 0, 0, pData);
        }

        return;
    }
    // ����ڲ˵���
}

// ȡָ�����������, ����û����ĳ���������򷵻�-1
inline int _menu_pt2index(const POINT& pt)
{
    int index = -1;
    for (auto& item : m_menu->menu)
    {
        index++;
        if (PtInRect(&item.rcItem, pt))
            return index;
    }

    for (auto& item : m_menu->menu_etool)
    {
        index++;
        if (PtInRect(&item.rcItem, pt))
            return index;
    }
    return -1;
}
// ȡָ�����������, ����û����ĳ���������򷵻�-1
inline int _menu_id2index(int id)
{
    int index = -1;
    for (auto& item : m_menu->menu)
    {
        index++;
        if (item.id == id)
            return index;
    }

    for (auto& item : m_menu->menu_etool)
    {
        index++;
        if (item.id == id)
            return index;
    }
    return -1;
}


// �����Ƿ��޸���ֵ
// ���������Ϊ��Ļ����
inline bool _menu_ismouse_hover(POINT& pt)
{
    ScreenToClient(s_info->hEWnd, &pt);
    if (!PtInRect(&s_info->rcMenu, pt))
        return false;

    int index = _menu_pt2index(pt);
    if (index != -1 && index != s_pData->draw.indexSel)
    {
        auto menu = _menu_get_data(index);
        if (!menu || !menu->isPopup)
            return false;

        //wstr::dbg(L"index=%d, s_pData->draw.indexSel = %d\n", index, s_pData->draw.indexSel);
        s_pData->draw.indexSel = index;
        s_pData->draw.indexHot = index;
        InvalidateRect(s_info->hEWnd, &s_info->rcMenu, 0);
        m_isEndMenu = true;
        EndMenu();
        return true;
    }
    return false;
}

void CALLBACK _close_menu(HWND hWnd, UINT message, UINT_PTR id, DWORD t)
{
    POINT pt;
    GetCursorPos(&pt);
    if (_menu_ismouse_hover(pt))
        KillTimer(hWnd, id);
}

LRESULT CALLBACK WndProc_Custom_Menu(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    //return CallWindowProcW(pData->oldProc, hWnd, message, wParam, lParam);
    switch (message)
    {
    case WM_USER + 0x12:
    {
        EMenu_IsSubClass((int)lParam);
        break;
    }
    case WM_PAINT:
    {
        const RECT& rc = s_info->rcMenu;
        RECT_F rcItem;
        LPCANVAS hCanvas = (LPCANVAS)wParam;
        //EWIDNOW_MENU_DRAWINFO* dw = (EWIDNOW_MENU_DRAWINFO*)pData->param1;

        int oldMode = 0;
        _canvas_SetAntialiasMode(hCanvas, 0, &oldMode);
        static bool isDxRender = s_offsetDx;
        const int itemHeight = s_info->scale(WINDOW_CAPTION_HEIGHT);
        LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
        float textHeight = 0;
        hCanvas->calctextsize(hFont, L"��", 1, 0, 100, 100, &textHeight, 0);
        //_canvas_setClip(hCanvas, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom - 1);

        auto pfn_draw = [&](std::vector<EMENU_DATA>& menu_data, bool isEToolMenu)
        {
            const int offset_index = isEToolMenu ? m_menu->menu.size() : 0;
            for (int i = 0; i < (int)menu_data.size(); i++)
            {
                EMENU_DATA& menu = menu_data[i];
                if ((!menu.text[0] && !menu.isPopup) || menu.isHide)
                    continue;
                bool isHot = false;
                rcItem(menu.rcItem);
                const int index = i + offset_index;
                if (index == pData->draw.indexHot || pData->draw.indexSel == index)
                {
                    rcItem.top = (float)((itemHeight - textHeight) / 2) - s_info->scale(2);
                    rcItem.bottom = rcItem.top + textHeight + s_info->scale(6);
                    if (pData->draw.indexSel == index)
                    {
                        _canvas_fillRectangle(hCanvas, s_info->window_d2d_info.hbrMenuDown, &rcItem);

                        rcItem.left += s_offsetDx;
                        rcItem.bottom += s_offsetDx;
                        _canvas_drawRectangle(hCanvas, s_info->window_d2d_info.hPenMenuBorder, &rcItem);


                        RECT_F rcFill(rcItem);
                        rcFill.top = rcFill.bottom - s_offsetDx;
                        _canvas_fillRectangle(hCanvas, s_info->window_d2d_info.hbrMenuDown, &rcFill);

                        rcItem.left -= s_offsetDx;
                        rcItem.bottom -= s_offsetDx;
                    }
                    else
                    {
                        LPEX_BRUSH
                            hbr = pData->draw.indexDown == 1 ? s_info->window_d2d_info.hbrBack : s_info->window_d2d_info.hbrMenuBarHot;
                        isHot = true;
                        _canvas_fillRectangle(hCanvas, hbr, &rcItem);
                    }

                }

                LPCWSTR drawText = menu.text;
                size_t textLen = wcslen(drawText);
                if (textLen > 0)
                {
                    const int fmt = DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
                    LPEX_BRUSH hbrText = s_info->window_d2d_info.hbrText;
                    if (isEToolMenu)
                        hbrText = isHot ? s_info->window_d2d_info.hbrMenuBarToolsTextHot : s_info->window_d2d_info.hbrMenuBarToolsText;

                    rcItem(menu.rcItem);
                    _canvas_drawtext(hCanvas, hFont, hbrText, drawText, textLen, fmt, &rcItem);
                }
            }
        };

        if (m_menu)
        {
            pfn_draw(m_menu->menu, false);
            pfn_draw(m_menu->menu_etool, true);
        }
        //_canvas_resetClip(hCanvas);
        _canvas_SetAntialiasMode(hCanvas, oldMode, 0);

        break;
    }
    case WM_MOUSEMOVE:
    {
        // ���ߵ���һ�����ڲ˵���
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        //if (pt.y > s_info->scale(WINDOW_CAPTION_HEIGHT)) return 0;
        const int index = _menu_pt2index(pt);
        if (index == pData->draw.indexHot)
        {
            // ��Ҫ�ж��Ƿ���, ���µĻ���Ҫ�����˵�
            return 0;
            return CallWindowProcW(pData->oldProc, hWnd, message, wParam, lParam);
        }

        //wstr::dbg(L"������, ���� = %d\n", index);
        SetTimer(hWnd, WM_MOUSEMOVE, 50, TimerProc_EWindowMenu);

        EMenu_InvalidateItem(pData->draw.indexHot, pData);
        pData->draw.indexHot = index;
        EMenu_InvalidateItem(pData->draw.indexHot, pData);
        return 0;
        return CallWindowProcW(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_MOUSELEAVE:
    {
        if (pData->draw.indexHot != 1)
        {
            EMenu_InvalidateItem(pData->draw.indexHot, pData);
            pData->draw.indexHot = -1;
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        if(pData->draw.indexHot == -1)
            return CallWindowProcW(pData->oldProc, hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);

        EMENU_DATA* menu = _menu_get_data(pData->draw.indexHot);
        if (!menu)
            return CallWindowProcW(pData->oldProc, hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);

        if (menu->hMenu == 0)    // ��Ҫ��������¼�, �����ǵ����˵��¼�
        {
            SendMessageW(s_info->hEWnd, WM_COMMAND, menu->id, 0);
            return 0;
        }
        SetFocus(hWnd);
        pData->draw.indexDown = 1;
        EMenu_PopUp(pData);
        return CallWindowProcW(pData->oldProc, hWnd, message, wParam, lParam);
    }
    case WM_KILLFOCUS:
    {
        pData->draw.indexHot = -1;
        pData->draw.indexDown = -1;
        InvalidateRect(hWnd, &s_info->rcMenu, 0);
        //wstr::dbg(L"�˵�ʧȥ����\n");
        break;
    }
    default:
        return CallWindowProcW(pData->oldProc, hWnd, message, wParam, lParam);
    }
    return 0;
}





DWORD EMenu_PopUp(LPOBJSTRUCT pData)
{
    LPCANVAS hCanvas = pData->draw.hCanvas;
    if (pData->draw.indexHot == -1 || !hCanvas)
        return 0;
    EMENU_DATA* menu = _menu_get_data(pData->draw.indexHot);
    if (!menu)
        return 0;

    EMenu_InvalidateItem(pData->draw.indexHot, pData);
    POINT pt;
    RECT& rcItem = menu->rcItem;



    LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    float textHeight = 0;
    hCanvas->calctextsize(hFont, L"��", 1, 0, 100, 100, &textHeight, 0);

    pt.y = ((s_info->scale(WINDOW_CAPTION_HEIGHT) - (int)textHeight) / 2) - s_info->scale(2);
    pt.y = pt.y + (int)textHeight + s_info->scale(6);
    pt.x = rcItem.left;

    ClientToScreen(pData->hWnd, &pt);
    pData->draw.indexSel = pData->draw.indexHot;
    InvalidateRect(pData->hWnd, &s_info->rcMenu, 0);
    s_popMenu = menu->hMenu;
    const int indexSel = pData->draw.indexSel;  // ��ֹ���޵ݹ�

    const UINT_PTR timerID = (UINT_PTR)s_info->hCodeTab;
    SetTimer(s_info->hCodeTab, timerID, 150, _close_menu);
    EMenu_IsSubClass(1);
    s_info->hMenuPopup = s_popMenu;
    //pfn_etMenuSelect pCallback = m_map_menu_event[menu.id];
    //if (pCallback)
    //    pCallback(menu.id, menu.hMenu);

    //wstr::dbg(L"��ʼ�����˵�, �����������Ĳ˵���\n");
    MENU_COPY_STRUCT hMenuCopy = _menu_copy_menu(s_popMenu, s_info->hEWnd);
    //hMenuCopy = s_popMenu;
    s_info->hMenuPopup = hMenuCopy;
    DWORD bRet = GetTrackPopupMenuEx()( s_info->hMenuPopup, TPM_VERTICAL, pt.x, pt.y, s_info->hEWnd, 0);
    //hMenuCopy = 0;
    //DWORD bRet = GetTrackPopupMenuEx()(s_popMenu, TPM_VERTICAL, pt.x, pt.y, s_info->hEWnd, 0);
    KillTimer(s_info->hCodeTab, timerID);

    // m_isEndMenu ֻ���ǵ��� EndMenu �رղ˵�ʱ�ŻḳֵΪtrue
    // ����EndMenu֮����Ϊ�˵�����һ���˵�
    // indexSel ��¼�����˵�ʱ������, ���������, �� m_isEndMenu = true
    // �ǾͲ����ڼ�������
    if (m_isEndMenu && indexSel != pData->draw.indexSel)
    {
        m_isEndMenu = false;
        // �ڸ� m_isEndMenu ��ֵΪtrue��ʱ����Ѿ����ú��˲˵�����
        // �����ֱ���ٴε����������
        EMenu_PopUp(s_pData);
        return bRet;
    }
    EMenu_IsSubClass(0);
    s_info->hMenuPopup = 0;
    m_isEndMenu = false;
    s_popMenu = 0;
    pData->draw.indexSel = -1;
    pData->draw.indexHot = -1;
    pData->draw.indexDown = -1;
    pData->hParent = 0;
    InvalidateRect(pData->hWnd, &s_info->rcMenu, 0);

    return bRet;
}



LRESULT CALLBACK EMenu_OnMeasureItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    LPMEASUREITEMSTRUCT item = (LPMEASUREITEMSTRUCT)lParam;
    if (item->CtlType != ODT_MENU)
        return WndProc_CallWindowProc(hWnd, message, wParam, lParam, pData);


    //if (item->itemID == 32854)
    //{
    //    auto timerProc = [](HWND hWnd, UINT message, UINT_PTR id, DWORD lParam) -> void
    //    {
    //        UINT* height = (UINT*)id;
    //        height[0] = (*height) + 10;
    //    };
    //    SetTimer(hWnd, (UINT_PTR)&item->itemHeight, 1000, timerProc);
    //}
    MENU_ID_DATA* data = EMenu_GetMenuItemInfoFromID(item->itemID);
    if (!data)// || item->itemID == -1)
    {
        item->itemHeight = s_info->config->MenuSeparatorHeight();
        if (item->itemHeight < 1)
            item->itemHeight = s_info->scale(5);
        else if (item->itemHeight > 50)
            item->itemHeight = s_info->scale(5);
    }
    else
    {
        if (data->width == 0)
        {
            GetMenuStringW(data->hMenu, data->id, data->text, 100, MF_BYCOMMAND);
            HDC hdc = GetDC(0);
            RECT rc = { 0 };
            DrawTextW(hdc, data->text, -1, &rc, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);
            ReleaseDC(0, hdc);
            data->width = rc.right - rc.left;
        }
        item->itemWidth = s_info->scale(data->width + 100);
        item->itemHeight = s_info->scale(data->height);
    }

    LPMENUEXT_ID_INFO userID = _menu_isext_(item->itemID);
    if (userID && userID->info)
    {
        //TODO ��Ҫ����˵��ߴ�
        IRE_STRUCT_MENU_CALCSIZE arg = { (int)item->itemWidth, (int)item->itemHeight, userID->param };
        userID->info->pfnCallback(INE_MENU_CALCMENU, userID->id, (LPARAM)&arg);
        item->itemWidth = arg.width;
        item->itemHeight = arg.height;
    }
    return 1;
}
inline LPEX_IMAGE _get_menu_icon()
{
    static LPEX_IMAGE hImg = 0;  // һ���򹴵�ͼ��
    if (hImg == 0)
    {
        const unsigned char pImageData[] =
        {
            0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10,
            0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0xF3, 0xFF, 0x61, 0x00, 0x00, 0x00, 0xDA, 0x49, 0x44, 0x41,
            0x54, 0x38, 0x4F, 0xC5, 0x92, 0xBD, 0x0E, 0x01, 0x41, 0x14, 0x85, 0xBF, 0xD1, 0xA8, 0x54, 0x2A, 0x95, 0x4A, 0x43, 0xA2,
            0xB1, 0x1A, 0x95, 0x4E, 0x25, 0x04, 0x51, 0x78, 0x13, 0xBB, 0x12, 0x34, 0xE3, 0x49, 0x56, 0x82, 0x20, 0xDE, 0xC3, 0xDA,
            0x27, 0x32, 0x57, 0x76, 0x15, 0xEC, 0x8F, 0xBF, 0x28, 0x76, 0xDA, 0x3B, 0xF7, 0xCB, 0x39, 0xE7, 0x1E, 0xC5, 0x9F, 0x4F,
            0xFD, 0xB9, 0x4F, 0x06, 0x00, 0xDB, 0xAB, 0x83, 0xCC, 0xD0, 0xD6, 0x38, 0x50, 0xFF, 0x9B, 0x02, 0xE7, 0x62, 0x21, 0xB2,
            0x06, 0xA9, 0x80, 0x5A, 0xA0, 0x1B, 0xCB, 0xEF, 0x01, 0x8E, 0xDF, 0x42, 0xAE, 0x6B, 0xA0, 0x0C, 0xEC, 0x92, 0x0A, 0x6C,
            0x6F, 0x1E, 0x10, 0x53, 0x43, 0x9D, 0xF9, 0x6D, 0x4C, 0xB8, 0x5C, 0x7A, 0x5E, 0x7E, 0x58, 0xB0, 0xCF, 0x13, 0xC0, 0x8D,
            0x0F, 0x43, 0x98, 0xED, 0x75, 0x40, 0x82, 0x59, 0x31, 0x6D, 0x7E, 0xB7, 0x30, 0xBD, 0x54, 0xC9, 0x99, 0x2D, 0x42, 0x2D,
            0xF2, 0xC9, 0xF1, 0xBA, 0x88, 0x71, 0x41, 0x15, 0x40, 0xF6, 0xE8, 0xE6, 0x28, 0xAE, 0xF0, 0x91, 0x41, 0x1C, 0xA2, 0x72,
            0x1B, 0x8C, 0x71, 0x51, 0xE4, 0x41, 0x0E, 0xE8, 0xE6, 0x30, 0xCD, 0x5E, 0x34, 0xC4, 0x67, 0x88, 0x20, 0xA8, 0xF0, 0x4A,
            0x47, 0xB4, 0x35, 0x78, 0x55, 0xB8, 0xE4, 0x15, 0xA2, 0x4A, 0x4E, 0x68, 0xAB, 0xFF, 0xAE, 0xAD, 0xE9, 0x67, 0x0C, 0x20,
            0x4A, 0x56, 0xE8, 0x46, 0xEF, 0x53, 0xD5, 0xBF, 0xEF, 0xC1, 0x0B, 0x52, 0xF6, 0x80, 0x1B, 0xBF, 0x86, 0x4D, 0x11, 0x93,
            0x2B, 0xE8, 0x35, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
        };

        hImg = _img_create_frommemory(pImageData, sizeof(pImageData) / sizeof(pImageData[0]));
    }
    return hImg;
}

// �Ӳ˵��ṹ�ﴴ��dxλͼ
// isCheck = �Ƿ�ѡ��, 0����1�����ȼ�� hbmpUnchecked �� hbmpChecked ��Ա, ���������ȼ�� hbmpItem ��Ա
inline LPEX_IMAGE _draw_menu_createimage(MENUITEMINFOW& mi, int isCheck)
{
    HBITMAP hBitmap = 0;
    switch (isCheck)
    {
    case 0:     // ���ȼ�� hbmpUnchecked
    {
        if (mi.hbmpUnchecked)
            hBitmap = mi.hbmpUnchecked;
        else if (mi.hbmpItem)
            hBitmap = mi.hbmpItem;
        else
            hBitmap = mi.hbmpChecked;
        break;
    }
    case 1:     // ���ȼ�� hbmpChecked
    {
        if (mi.hbmpChecked)
            hBitmap = mi.hbmpChecked;
        else if (mi.hbmpItem)
            hBitmap = mi.hbmpItem;
        else
            hBitmap = mi.hbmpUnchecked;
        break;
    }
    default:    // ���ȼ�� hbmpItem
    {
        if (mi.hbmpItem)
            hBitmap = mi.hbmpItem;
        else if (mi.hbmpUnchecked)
            hBitmap = mi.hbmpUnchecked;
        else
            hBitmap = mi.hbmpChecked;
        break;
    }
    }

    if (hBitmap)
    {
        LPEX_IMAGE& ret = m_menu_bitmap[hBitmap];
        if (!ret)
            ret = _img_create_frombitmap(hBitmap, 0, true);
        return ret;
    }
    return 0;
}

// 1. ����չ�˵��ص����ȡ
// 2. �ȴ��û��󶨵���Դ���ȡ
// 3. ��Ĭ��ͼ�����ȡ
// �������ȡ�����򷵻�0
inline LPEX_IMAGE _emenu_geticon(LPCWSTR pszID, RECT_F& rcIcon, int& cxIcon, int& cyIcon, MENUITEMINFOW& mi, LPMENUEXT_ID_INFO menuExt, STATE state = STATE::NORMAL)
{
    LPEX_IMAGE hIcon = 0;
    if (menuExt && menuExt->info && menuExt->info->pfnCallback)
    {
        IRE_STRUCT_GETICON getIcon = { 0 };
        RECT rcIconInt = { 0 };
        getIcon.hWnd = 0;
        getIcon.hResource = s_info->pResource->GetDefControlIcon(HRESOURCE_MENU);
        getIcon.pszID = mi.dwTypeData;
        getIcon.pArg = (LPVOID)menuExt->id;
        getIcon.prc = &rcIcon;
        getIcon.state = state;
        getIcon.pIconType = 0;

        LPWINDOW_ICON_DATA pIconData = (LPWINDOW_ICON_DATA)menuExt->info->pfnCallback(INE_MENU_GETICON, menuExt->id, (LPARAM)&getIcon);
        cxIcon = (int)(rcIcon.width());
        cyIcon = (int)(rcIcon.height());
        if (!pIconData || cxIcon <= 0 || cyIcon <= 0)
            return 0;
        hIcon = pIconData->hBitmapDx;
    }

    if (!hIcon)
    {
        LPWINDOW_ICON_DATA iconData = s_info->pResource->GetIconFromHwnd(HRESOURCE_MENU, pszID, (LPVOID)mi.wID, &rcIcon, state, s_info->dpiX);
        if (!iconData)
            iconData = s_info->pResource->Menu_GetIcon(pszID, &rcIcon, state, s_info->dpiX);

        if (iconData)
        {
            hIcon = iconData->hBitmapDx;
            cxIcon = (int)(rcIcon.width());
            cyIcon = (int)(rcIcon.height());
        }
    }
    return hIcon;
}


// �滭�˵���, ������䵱ǰ�˵����ʹ�õĻ�ˢ
inline LPEX_BRUSH _draw_menu_item(LPDRAWITEMSTRUCT dw, LPCANVAS hCanvas, RECT_F& rcItem, MENUITEMINFOW& mi, LPOBJSTRUCT pData, LPMENUEXT_ID_INFO menuExt)
{
    MENU_ID_DATA* data      = EMenu_GetMenuItemInfoFromID(dw->itemID);
    LPEX_BRUSH hbrBack      = s_info->window_d2d_info.hbrMenuBack;   // Ĭ�ϱ�����ˢ
    LPEX_BRUSH hpenBorder   = s_info->window_d2d_info.hbrMenuBorder; // Ĭ�ϱ߿򻭱�
    LPEX_BRUSH hbrText      = s_info->window_d2d_info.hbrMenuText;   // Ĭ���ı���ˢ

    if (dw->itemState & ODS_SELECTED)       // �˵����״̬��ѡ��, ����ƶ��ȵ�Ч��
    {
        hbrBack = s_info->window_d2d_info.hbrMenuHot;
        hbrText = s_info->window_d2d_info.hbrMenuTextHot;
        if (dw->itemState & ODS_DISABLED)
            hbrBack = s_info->window_d2d_info.hbrMenuBan;
    }
    if (dw->itemState & ODS_DISABLED)
        hbrText = s_info->window_d2d_info.hbrMenuTextBan;

    _canvas_fillRectangle(hCanvas, hbrBack, &rcItem);


    RECT_F rcIcon;
    int cxIcon, cyIcon;
    LPEX_IMAGE hIcon = 0;
    wchar_t pszID[50];
    swprintf_s(pszID, 50, L"%d", dw->itemID);
    if (dw->itemState & ODS_CHECKED)    // ѡ���˲˵�, ��Ҫ����ѡ���, ��������䱳��֮��滭
    {
        hIcon = _emenu_geticon(pszID, rcIcon, cxIcon, cyIcon, mi, menuExt);
        if (!hIcon) // �����ȡʧ�����Ĭ��ͼ�����ȡѡ�е�ͼ��
        {
            LPWINDOW_ICON_DATA iconData = s_info->pResource->Menu_GetIcon(L"select", &rcIcon, STATE::NORMAL, s_info->dpiX);
            if (iconData)
            {
                hIcon = iconData->hBitmapDx;
                cxIcon = (int)rcIcon.width();
                cyIcon = (int)rcIcon.height();
            }
        }
        
        if (!hIcon) // ѡ�б������һ��ͼ��, Ĭ��ͼ��Ҳ��ȡʧ������ڴ����ȡ
        {
            hIcon = _get_menu_icon();
            cxIcon = 16;
            cyIcon = 16;
            rcIcon = { 0, 0, (float)cxIcon, (float)cyIcon };
        }
    }
    else
    {
        // 1. ����չ�˵��ص����ȡ
        // 2. �ȴ��û��󶨵���Դ���ȡ
        // 3. ��Ĭ��ͼ�����ȡ
        // 4. �Ӳ˵���λͼ���ȡͼ��
        swprintf_s(pszID, 50, L"%d", dw->itemID);
        hIcon = _emenu_geticon(pszID, rcIcon, cxIcon, cyIcon, mi, menuExt);
        if (!hIcon)
            hIcon = _draw_menu_createimage(mi, -1);
    }

    if (hIcon)
    {
        RECT_F rcDraw;
        rcDraw.left = rcItem.left + 5;
        rcDraw.top = rcItem.top + ((rcItem.bottom - rcItem.top) - (float)s_info->scale(cyIcon)) / 2;
        rcDraw.right = rcDraw.left + (float)s_info->scale(cxIcon);
        rcDraw.bottom = rcDraw.top + (float)s_info->scale(cxIcon);
        _canvas_drawimagerectrect(hCanvas, hIcon, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom,
            rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 255);

    }

    rcItem.left     += s_info->scale(30);
    rcItem.top      += s_info->scale(5);
    rcItem.right    -= s_info->scale(30);
    rcItem.bottom   -= s_info->scale(5);

    LPCWSTR lpText = mi.dwTypeData;
    if (!lpText || !lpText[0] || data && data->text[0])
        lpText = data->text;
    size_t len = wcslen(lpText);
    const wchar_t* pos = wcschr(lpText, '\t');
    if (pos) len = pos++ - lpText;   // �п�ݼ�

    LPEX_FONT hFont = g_scale.GetFont(pData->draw.hFont);
    _canvas_drawtext(hCanvas, hFont, hbrText, lpText, len, DT_VCENTER | DT_SINGLELINE, &rcItem);

    if (pos)    // tab���������ı�, ���Ҷ��뻭��
    {
        _canvas_drawtext(hCanvas, hFont, hbrText, pos, -1, DT_VCENTER | DT_SINGLELINE | DT_RIGHT, &rcItem);
    }
    return hbrBack;
}

inline void _draw_menu_border(LPDRAWITEMSTRUCT dw, LPCANVAS hCanvas, LPEX_BRUSH hbrBack, int cxClient, int cyClient, LPOBJSTRUCT pData)
{
    LPEX_PEN hpenBorder = s_info->window_d2d_info.hPenMenuBorder;   // Ĭ�ϱ߿򻭱�
    const int offset = s_offsetDx ? 0 : 1;
    RECT_F rcItem((float)s_offsetDx, (float)s_offsetDx, (float)(cxClient - offset), (float)(cyClient - offset));
    _canvas_drawRectangle(hCanvas, hpenBorder, &rcItem);            // �ڲ˵�������Χ�����߿�


    if ((HMENU)dw->hwndItem == s_popMenu)   // �ǲ˵��е����Ĳ˵�, ���Ͻ���Ҫ�����ճ���
    {
        RECT_F rcFill(1, 0, 0, 1);
        if (pData->draw.indexHot != -1)
        {
            auto menu = _menu_get_data(pData->draw.indexHot);
            if (menu)
            {
                RECT& rcMenu = menu->rcItem;
                rcFill.right = (float)(rcMenu.right - rcMenu.left - s_offsetDx);
            }
        }
        _canvas_fillRectangle(hCanvas, hbrBack, &rcFill);
    }

}
//#define _dbg_text(_s, ...) __dbg_text(_s, __VA_ARGS__)
#define _dbg_text(_s, ...) 
inline void __dbg_text(LPCWSTR fmt, ...)
{
    //va_list aptr;
    //va_start(aptr, fmt);
    //wstr text = wstr::GetFormatEx(260, fmt, aptr);
    //va_end(aptr);

    //HWND hWnd = (HWND)1575946;
    //int len = (int)(DWORD)SendMessageW(hWnd, WM_GETTEXTLENGTH, 0, 0);
    //Edit_SetSel(hWnd, len, len);
    //SendMessageW(hWnd, EM_REPLACESEL, (WPARAM)0, (LPARAM)text.c_str());
}

LRESULT CALLBACK EMenu_OnDrawItem(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LPOBJSTRUCT pData)
{
    pData = s_pData;
    LPDRAWITEMSTRUCT dw = (LPDRAWITEMSTRUCT)lParam;
    if (s_info->MY_MF_OWNERDRAW)
        SelectClipRgn(dw->hDC, 0);
    //wstr::dbg(L"�ػ��˵�����, hdc = 0x%p, hMenu = %p\n", dw->hDC, dw->hwndItem);

    DRAWITEM_MENU_INFO* info = GetDrawItemInfo(dw->hDC);
    int cxClient = 0, cyClient = 0;
    bool isTopTriangle = false;
    if (!info)
    {
        //assert(false);
        BITMAP bm = { 0 };
        HBITMAP hBitmap = (HBITMAP)GetCurrentObject(dw->hDC, OBJ_BITMAP);
        int bmSize = GetObjectW(hBitmap, sizeof(bm), &bm);
        cxClient = bm.bmWidth;
        cyClient = bm.bmHeight;
        if ( cxClient <= 0 || cyClient <= 0 )
        {
            HWND hWnd = GetHwndFromMenu((HMENU)dw->hwndItem);
            if ( !hWnd )
                hWnd = GetCurrentMenuHwnd();
            RECT rc = { 0 };
            GetWindowRect(hWnd, &rc);
            cxClient = rc.right - rc.left;
            cyClient = rc.bottom - rc.top;
        }
    }
    else
    {
        cxClient = info->cxClient;
        cyClient = info->cyClient;
        isTopTriangle = info->isTopTriangle;
    }

    dw->rcItem.left = 0;
    dw->rcItem.right = cxClient;


    LPCANVAS hCanvas = s_info->window_d2d_info.hCanvas;
    const int cxItem = dw->rcItem.right - dw->rcItem.left;
    const int cyItem = dw->rcItem.bottom - dw->rcItem.top;
    const int offsetTriangle = s_info->scale(20);  // ����ƫ��, �ұ�������ô�����ز��滭
    HMENU hMenu = (HMENU)dw->hwndItem;

    RECT_F rcItem(dw->rcItem);
    //if (isTopTriangle)
    //    rcItem.offset(0, (float)offsetTriangle);
    rcItem.top = 0;
    rcItem.bottom = (float)(cyItem);
    const int cxCanvas = (int)hCanvas->GetWidth();
    const int cyCanvas = (int)hCanvas->GetHeight();
    if (cxCanvas < cxItem || cyCanvas < cyItem)
    {
        int maxWidth = cxCanvas;
        int maxheight = cyCanvas;

        if (cxClient > maxWidth)  maxWidth  = cxClient;
        if (cyClient > maxheight) maxheight = cyClient;

        _canvas_resize(hCanvas, maxWidth, maxheight);
    }
    _dbg_text(L"��ʼ�˵��ػ�, ��� = %d, �߶� = %d, �˵���� = 0x%p, ������� = %d, �߶� = %d\n",
              cxItem, cyItem, hMenu, hCanvas->width, hCanvas->height);


    int oldTextMode = 0, oldRenderMode = 0;
    _canvas_begindraw(hCanvas);

    LPEX_BRUSH hbrBack      = s_info->window_d2d_info.hbrMenuBack;      // Ĭ�ϱ�����ˢ
    LPEX_PEN   hpenBorder   = s_info->window_d2d_info.hPenMenuBorder;   // Ĭ�ϱ߿򻭱�
    LPEX_BRUSH crText       = s_info->window_d2d_info.hbrMenuText;      // Ĭ���ı���ˢ
    _canvas_SetAntialiasMode(s_info->window_d2d_info.hCanvas, 0, &oldRenderMode);   // ���߲���Ҫ�����

    MENU_ID_DATA* data = EMenu_GetMenuItemInfoFromID(dw->itemID);
    wchar_t szMenuText[260] = { 0 };
    MENUITEMINFOW mi = { 0 };
    mi.cbSize = sizeof(MENUITEMINFOW);
    mi.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_SUBMENU;
    mi.dwTypeData = szMenuText;
    mi.cch = 260;
    GetMenuItemInfoW((HMENU)dw->hwndItem, dw->itemID, false, &mi);



    IRE_STRUCT_MENU_DRAW menuDraw = { 0 };

    menuDraw.hCanvas = hCanvas;
    menuDraw.width = cxItem;
    menuDraw.height = cyItem;
    menuDraw.type = mi.fType;
    menuDraw.text = szMenuText;
    menuDraw.state = STATE::NORMAL;
    menuDraw.id = dw->itemID;


    STATE stateTriangle = STATE::NORMAL;
    if (dw->itemState & ODS_SELECTED)       // �˵����״̬��ѡ��, ����ƶ��ȵ�Ч��
        menuDraw.state |= STATE::HOT, stateTriangle = STATE::HOT;
    if (dw->itemState & ODS_DISABLED)       // �˵��ǽ�ֹ״̬
        menuDraw.state |= STATE::BAN;
    if (dw->itemState & ODS_CHECKED)        // ѡ���˲˵�, ��Ҫ����ѡ���, ��������䱳��֮��滭
        menuDraw.state |= STATE::CHECKED;


    LPMENUEXT_ID_INFO menuext = _menu_isext_(dw->itemID);
    RECT_F rcIcon;
    int cxIcon = 0, cyIcon = 0;
    LPEX_IMAGE hIcon = 0;   // ֻ���������ʱ��Ż�ȡ, �����ֵ�Ļ�һ����������
    if (mi.hSubMenu)
        hIcon = _emenu_geticon(L"triangle", rcIcon, cxIcon, cyIcon, mi, menuext, stateTriangle);

    while (true)
    {
        if (menuext && menuext->info)
        {
            menuDraw.id = menuext->id;
            if (menuext->info->pfnCallback)
            {
                if (menuext->info->pfnCallback(INE_MENU_DRAWMENU_BEFORE, menuDraw.id, (LPARAM)&menuDraw))
                    break;  // ���ػ滭, �����滭����
            }
        }
        if (dw->itemID == 0 || dw->itemID == -1 || (mi.fType & MFT_SEPARATOR))  // �ָ���
        {
            LINEPOINT_F linePoint(rcItem.left + 30, rcItem.top + 2, rcItem.right - 10, rcItem.top + 2);

            _canvas_fillRectangle(hCanvas, hbrBack, &rcItem);       // ʹ��Ĭ�ϱ������, Ȼ���ڻ���
            _canvas_drawLine(hCanvas, hpenBorder, &linePoint);      // ��������, �����30 ���� �ұ�-10��λ��, �ָ���ռ��5������, ����ʹӶ���2��ʼ��
        }
        else
        {
            hbrBack = _draw_menu_item(dw, hCanvas, rcItem, mi, pData, menuext);
        }

        if (menuext && menuext->info)
        {
            menuDraw.id = menuext->id;
            if (menuext->info->pfnCallback)     // �滭���¼�
                menuext->info->pfnCallback(INE_MENU_DRAWMENU_AFTER, menuDraw.id, (LPARAM)&menuDraw);
        }
        break;
    }

    if (hIcon)
    {
        RECT_F rcTriangle;
        rcTriangle.left = (float)(((cxClient - offsetTriangle) + ((offsetTriangle - s_info->scale(cxIcon)) / 2)));
        rcTriangle.top  = (float)((cyItem - s_info->scale(cyIcon)) / 2);
        rcTriangle.right  = rcTriangle.left + (float)s_info->scale(cxIcon);
        rcTriangle.bottom = rcTriangle.top  + (float)s_info->scale(cyIcon);

        _canvas_drawimagerectrect(hCanvas, hIcon,
            rcTriangle.left, rcTriangle.top, rcTriangle.right, rcTriangle.bottom,
            rcIcon.left, rcIcon.top, rcIcon.right, rcIcon.bottom, 255);
    }

    //SelectClipRgn(dw->hDC, 0);

    //RECT_F rcTest(dw->rcItem);
    //ExcludeClipRect(dw->hDC, dw->rcItem.left + 10, dw->rcItem.top + 5, dw->rcItem.left + 50, dw->rcItem.top + 20);
    //LPEX_BRUSH hbrTest =s_info->pfnCreateBrushDx(RGB2ARGB(255, 255));
    //_canvas_fillRectangle(hCanvas, hbrTest, &rcTest);


    // �Ȱѱ������ݻ滭����, Ȼ����ջ����ڻ滭�߿�
    HDC hdc = _canvas_getdc(hCanvas);
    BitBlt(dw->hDC, dw->rcItem.left, dw->rcItem.top, cxItem, cyItem, hdc, 0, 0, SRCCOPY);
    //BitBlt(dw->hDC, 0, 0, bm.bmWidth, bm.bmHeight, hdc, 0, 0, SRCCOPY);
    _canvas_releasedc(hCanvas);


    if (s_info->MY_MF_OWNERDRAW)
    {

        // �������, ��ʼ�滭�߿�
        _canvas_clear(hCanvas, 0);
        _draw_menu_border(dw, hCanvas, s_info->window_d2d_info.hbrMenuBack, cxClient, cyClient, pData);
        hdc = _canvas_getdc(hCanvas);
        //BitBlt(dw->hDC, 0, 0, cxClient, cyClient, hdc, 0, 0, SRCCOPY);
        
        BitBlt(dw->hDC, 0, 0, 1, cyClient, hdc, 0, 0, SRCCOPY); // ����߱߿򿽱���Ŀ��DC��
        BitBlt(dw->hDC, 0, 0, cxClient, 1, hdc, 0, 0, SRCCOPY); // �Ѷ��߱߿򿽱���Ŀ��DC��
        BitBlt(dw->hDC, cxClient - 1, 0, 1, cyClient, hdc, cxClient - 1, 0, SRCCOPY);   // ���ұ߱߿򿽱���Ŀ��DC��
        BitBlt(dw->hDC, 0, cyClient - 1, cxClient, 1, hdc, 0, cyClient - 1, SRCCOPY);   // �ѵױ߱߿򿽱���Ŀ��DC��
        _canvas_releasedc(hCanvas);

    }
    //_canvas_SetTextRenderingHint(hCanvas, oldTextMode);
    _canvas_SetAntialiasMode(hCanvas, oldRenderMode, 0);
    _canvas_enddraw(hCanvas);

    _dbg_text(L"�˵��滭����\n");


    RECT rcRgn = { 0, 0, cxClient, cyClient };
    if (isTopTriangle || 1)
    {
        rcRgn.top = offsetTriangle;
        rcRgn.bottom = cxClient - offsetTriangle;
    }
    if (hIcon) // �Ѿ���ȡ������ͼ��, �Ǿ����òü���, �������滭Ĭ�ϵ�����
    {
        rcRgn.right -= offsetTriangle;
    }

    //ExcludeClipRect(dw->hDC, rcRgn.left, rcRgn.top, rcRgn.right, rcRgn.bottom);

    if (s_info->MY_MF_OWNERDRAW)
    {
        HRGN hRgn = CreateRectRgn(rcRgn.left, rcRgn.top, rcRgn.right, rcRgn.bottom);
        SelectClipRgn(dw->hDC, hRgn);
        DeleteObject(hRgn);
    }
    return 1;
}



// ����˵������ڶ����Ĳ˵�����
// id = �˵�id, ͨ�����id�������ĸ���ť
// lpszMenuText = ��ʾ�Ĳ˵��ı�
// index = Ҫ���뵽�ĸ�λ��, ��10��ʼ, -1����뵽���, ǰ��10���˵����������Դ���, ���������10��ʼ
// hPopupMenu = �Ƿ�Ҫ�����˵�, �����Ҫ�����˵�, ��Ҫ���ݲ˵����, ��������ťʱ�ᵯ���˵�, ����0�Ļ�����ᴥ������¼�
// isPopup = �Ƿ񵯳��˵�, ���Ϊ��, ������꾭����ʱ��ᵯ���˵�
void __menu_insert(int id, LPCWSTR lpszMenuText, int index, HMENU hPopupMenu, bool isPopup)
{
    //wstr::dbg(L"����˵�������, id = %d, �ı� = %s, λ�� = %d, hPopupMenu = %p, �����˵� = %d\n", id, lpszMenuText, index, hPopupMenu, isPopup);
    BOOL isEToolMenu = id == 80000 || IsEToolMenu(hPopupMenu);
    //if (!isEToolMenu && lpszMenuText && *lpszMenuText)
    //    index = 9;

    EMENU_DATA* item = 0;
    int right = s_info->rcMenu.right;
    auto pfn_insert = [&](std::vector<EMENU_DATA>& menu)
    {
        int count = (int)menu.size();
        if (index == -1) // ��Ҫ�ӵ����
        {
            menu.emplace_back();
            item = &(menu.back());
            index = count;
            return;
        }

        const int _index = index - (isEToolMenu ? (int)(m_menu->menu.size() - 1) : 0);

        menu.emplace(menu.begin() + _index);
        item = &menu[_index];
        right = item->rcItem.left;

    };
    
    if (isEToolMenu)
        pfn_insert(m_menu->menu_etool);
    else
    {
        pfn_insert(m_menu->menu);
        // �в����˲˵�, ���µ����˵���Ϣ
        _menu_get_info();
    }


    item->id = id;
    item->dwItemData = 0;
    item->hMenu = hPopupMenu;
    item->isPopup = isPopup;
    if (!isEToolMenu)
        item->isPopup = true;   // �������ֲ˵����е�����־

    //item->isPopup = isPopup != 0;
    int itemWdith = 20;  // �����Ĳ˵���Ŀ��
    HDC hdc = CreateCompatibleDC(0);
    LPEX_FONT hFont = g_scale.GetFont(s_info->defData->draw.hFont);
    SelectObject(hdc, hFont->GetFont());
    //wstr::dbg(L"����˵���, ��ǰ���� = %d, �˵�ID = %d(0x%08X)\n", menu.count, item->id, item->id);
    if (lpszMenuText && lpszMenuText[0])
    {
        size_t len = wcslen(lpszMenuText);
        memcpy(item->text, lpszMenuText, len * sizeof(wchar_t));
        item->text[len] = 0;
        if (isEToolMenu && (wstr::find(lpszMenuText, L"����") != wstr::npos || wstr::find(lpszMenuText, L"�ʿ�") != wstr::npos))
            item->isPopup = true;
        RECT rcText = { 0 };
        DrawTextW(hdc, item->text, -1, &rcText, DT_VCENTER | DT_SINGLELINE | DT_CALCRECT);

        itemWdith = rcText.right - rcText.left + 10;
        RECT& rcItem = item->rcItem;
        rcItem.left = right;
        rcItem.right = rcItem.left + itemWdith;
        rcItem.top = 0;
        rcItem.bottom = WINDOW_CAPTION_HEIGHT;
    }
    else
    {
        RECT& rcItem = item->rcItem;
        rcItem.left = right;
        rcItem.right = rcItem.left;
        rcItem.top = 0;
        rcItem.bottom = WINDOW_CAPTION_HEIGHT;
    }
    //if (isEToolMenu)
    //{
    //    for (int i = index + 1; i < (int)m_menu->menu_etool.size(); i++)    // ���¼��������������Ĳ˵�������ƶ�
    //    {
    //        m_menu->menu_etool[i].rcItem.left += itemWdith;
    //        m_menu->menu_etool[i].rcItem.right += itemWdith;
    //    }
    //}
    //else
    //{
    //    for (int i = index + 1; i < (int)m_menu->menu.size(); i++)    // ���¼��������������Ĳ˵�������ƶ�
    //    {
    //        m_menu->menu[i].rcItem.left += itemWdith;
    //        m_menu->menu[i].rcItem.right += itemWdith;
    //    }
    //}

    if (s_info->isReady)
        __menu_calc_rect(hdc, *m_menu, false);

    s_info->rcMenu.right += itemWdith;
    _window_recalc_title(s_info->pWnd, true);
    DeleteDC(hdc);
}

void __menu_remove(int id, bool isIndex)
{
    int index = id;
    if (!isIndex)   // ����������ʽ����Ҫ���������������
    {
        index = _menu_id2index(id);
    }
    auto menu = _menu_get_data(index);
    if (!menu)
    {
//#ifdef _DEBUG
//        throw;
//#endif
        return;
    }

    int menu_size = (int)m_menu->menu.size();
    int width = menu->rcItem.right - menu->rcItem.left; // �˵�������Ҫ���ٵĳߴ�

    if (index >= menu_size)
    {
        index -= menu_size;
        m_menu->menu_etool.erase(m_menu->menu_etool.begin() + index);
    }
    else
    {
        m_menu->menu.erase(m_menu->menu.begin() + index);
    }

    if (s_info->isReady)
    {
        HDC hdc = CreateCompatibleDC(0);
        LPEX_FONT hFont = g_scale.GetFont(s_info->defData->draw.hFont);
        SelectObject(hdc, hFont->GetFont());
        __menu_calc_rect(hdc, *m_menu, false);
        DeleteDC(hdc);
    }

    s_info->rcMenu.right -= width;
    _window_recalc_title(s_info->pWnd, true);

    //wstr::dbg(L"ɾ�����ֲ˵���, id = %d, ���� = %d\n", id, index);

}



// ����һ��һģһ���Ĳ˵�����, ���ز˵����
inline UINT_PTR __menu_enum(HMENU hMenu, HMENU hMenuInsert, LPCWSTR menuName, UINT flags, LPPLUGIN_INFO info)
{
    HMENU hMenuPop = _menu_create();
    int len = GetMenuItemCount(hMenu);
    wchar_t text[260] = { 0 };
    MENUITEMINFOW item = { 0 };
    item.cbSize = sizeof(item);
    item.fMask = MIIM_DATA | MIIM_ID | MIIM_SUBMENU | MIIM_FTYPE | MIIM_STRING;
    for (int i = 0; i < len; i++)
    {
        item.cch = 260;
        item.dwTypeData = text;
        if (GetMenuItemInfoW(hMenu, i, true, &item))
        {
            if (item.hSubMenu)
            {
                HMENU hMenuSub = (HMENU)__menu_enum(item.hSubMenu, hMenuPop, text, item.fType, info);
                _menu_add(hMenuPop, (UINT_PTR)hMenuSub, menuName, flags);
            }
            else
            {
                if ((item.fType & MF_SEPARATOR) == 0)
                    item.wID = _menu_alloc_ext(item.wID, item.dwItemData, info);
                _menu_add(hMenuPop, item.wID, text, item.fType);
            }
        }
    }
    return (UINT_PTR)hMenuPop;
}
// ����guid���������뵽�ĸ��˵�����
bool MenuExt_Insert(LPCSTR guid, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCWSTR lpNewItem, LPARAM param)
{
    LPPLUGIN_INFO info = s_info->pfnPlugin_GetInfo(guid);
    if (!info) return 0;
    if (!lpNewItem)lpNewItem = L"";
    if (!info->hPopupMenu)
    {
        info->hPopupMenu = CreatePopupMenu();
        const int count = GetMenuItemCount(s_info->hMenuExtPopup);
        MENUITEMINFOA item = { 0 };
        item.cbSize = sizeof(item);
        item.fMask = MIIM_FTYPE;
        UINT id = GetMenuItemID(s_info->hMenuExtPopup, count - 3);
        int err = GetLastError();
        if (id == ((UINT)-1) && err == ERROR_INVALID_PARAMETER) // ��ȡʧ��, ��ʾ�ǵ�һ������Ĳ˵�, ֱ�Ӳ��뵽ͷ��
        {
            InsertMenuA(s_info->hMenuExtPopup, 0, MF_POPUP | MF_BYPOSITION, (UINT_PTR)info->hPopupMenu, info->name);
            InsertMenuA(s_info->hMenuExtPopup, 1, MF_SEPARATOR | MF_BYPOSITION, (UINT_PTR)0, 0);
        }
        else
        {
            InsertMenuA(s_info->hMenuExtPopup, count - 3, MF_POPUP | MF_BYPOSITION, (UINT_PTR)info->hPopupMenu, info->name);
        }
    }
    //_str::dbg("%s ��Ӳ˵�, �˵�id = %d, �˵��� = %s, �Ƿ񵯳�ʽ�˵� = %d\n", info->name, uIDNewItem, wstr::W2A(lpNewItem).c_str(), (uFlags & MF_POPUP) != 0);
    if (uFlags & MF_POPUP)  // �ǵ���ʽ�˵�, ��Ҫö�ٲ˵���Ϣ����
    {
        // 
        uIDNewItem = __menu_enum((HMENU)uIDNewItem, info->hPopupMenu, lpNewItem, uFlags, info);
    }
    else
    {
        // ���ǵ���ʽ�˵�, �Ǿ���Ҫ����һ���˵���
        uIDNewItem = _menu_alloc_ext(uIDNewItem, param, info);    // id���ǲ˵�����Ҫ����һ���˵�id
    }

    if ((uFlags & MF_BYPOSITION) == 0)  // ����ID�ķ�ʽ����˵�, ��Ҫ���Ѽ���˵���������ID
    {
        uPosition = MenuExt_Find(info, uPosition);
    }
    if (uPosition == ((UINT)-1))
    {
        uFlags |= MF_BYPOSITION;
    }
    return _menu_insert(info->hPopupMenu, uPosition, uIDNewItem, lpNewItem, uFlags);
    return InsertMenuW(info->hPopupMenu, uPosition, uFlags, uIDNewItem, lpNewItem);
}

UINT_PTR MenuExt_Find(LPPLUGIN_INFO info, int id)
{
    for (auto it = m_pool_id_menu_ext.begin(); it != m_pool_id_menu_ext.end(); ++it)
    {
        LPMENUEXT_ID_INFO menuExt = it->second;
        // �����������µ����в˵�
        if (menuExt->info && menuExt->info->hash == info->hash && menuExt->id == id)
        {
            return (UINT_PTR)menuExt;
        }
    }
    return id;
}

bool MenuExt_Delete(LPCSTR guid)
{
    LPPLUGIN_INFO info = s_info->pfnPlugin_GetInfo(guid);
    if (!info) return false;
    if (info->hPopupMenu)   // �в˵���
    {
        //RemoveMenu(s_info->hMenuExtPopup, (UINT)info->hPopupMenu, MF_BYCOMMAND);
        DeleteMenu(s_info->hMenuExtPopup, (UINT)info->hPopupMenu, MF_BYCOMMAND);
        info->hPopupMenu = 0;
    }
    return true;
    //return s_info->pfnPlugin_UnInit(guid);
}

void MenuExt_Command(LPMENUEXT_ID_INFO info)
{
    if (!info) return;
    const int id = info->id;
    const LPARAM param = info->param;
    if (!info->info)
    {
        switch (id)
        {
        case 1:     // ������չ(&M)
        {
            LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
            const int count = s_info->evt.pfn_Event_callback_before(arr, IRE_MENUEXT_MANAGE, 0, &isReturn, false);
            if (count == 0)
            {
                MessageBoxW(s_info->hEWnd, L"������չ��δʵ��, ��ȴ��°汾�ĸ���", L"��δʵ��", MB_OK);
            }
            return;
        }
        case 2:     // �Զ���˵�(&C)...
        {
            LPREGISTER_EVENT_INFO arr[EVENT_MAX_ITEM]; int isReturn = 0;
            const int count = s_info->evt.pfn_Event_callback_before(arr, IRE_MENUEXT_CUSTOP, 0, &isReturn, false);
            if (count == 0)
            {
                EMenu_CustomMenu(m_menu);
                //MessageBoxW(s_info->hEWnd, L"��ʱδʵ���Զ���˵�����, ��ȴ��°汾�ĸ���", L"��δʵ��", MB_OK);
            }
            return;
        }
        default:
            break;
        }
        return;
    }
    if (!info->info || !info->info->pfnMenuExt_Command)
        return;
    int ret = info->info->pfnMenuExt_Command(id, param);

    //wstr::dbg(L"��չ�˵�������, �˵�ID = %d, ��Ŀ��ֵ = %d\n", id, param);
}

int CALLBACK IDraw_Menu_Function(UINT nCode, WPARAM wParam, LPARAM lParam)
{
    switch (nCode)
    {
    case IDC_ICONTROL_INSERTMENUBAR:    // ����˵������ڶ����Ĳ˵�����, wParam����, lParam ���� IDC_STRUCT_MENU_INSERT�ṹָ��
    {
        IDC_STRUCT_MENU_INSERT* arg = (IDC_STRUCT_MENU_INSERT*)lParam;
        __menu_insert(arg->id, arg->lpszMenuText, arg->index, arg->hPopupMenu, arg->isPopup);
        return 0;
    }
    case IDC_ICONTROL_REMOVEMENUBAR:    // �Ӷ����˵�����ɾ���˵�, wParam �ǲ˵�ID, lParam ��ʾ wParam�Ƿ�Ϊ����
    {
        __menu_remove(wParam, lParam);
        return 0;
    }
    case IDC_ICONTROL_INSERT_MENUEXT:   // ����˵�����չ�˵���, wParam����, lParam ���� IDC_STRUCT_MENUEXT_INSERT�ṹָ��
    {
        IDC_STRUCT_MENUEXT_INSERT* arg = (IDC_STRUCT_MENUEXT_INSERT*)lParam;
        return (int)MenuExt_Insert(arg->guid, arg->uPosition, arg->uFlags, arg->uIDNewItem, arg->lpNewItem, arg->param);
    }
    case IDC_ICONTROL_ADD_MENUBAR:      // ���ֵ���, wParam���ݳ�Ա��, lParam ������������, ��������Ҫ���붥���˵��ľ��, ֧�ֿ�ֻ������Щ�˵����붥��
    case IDC_ICONTROL_ISADDMENUBAR:     // ����ר�ù���, ���붥���˵�ʱ�ж��Ƿ��������, wParam = 0, lParam = Ҫ����Ĳ˵����, ��Ϊ���ּ��붥���Ķ��ǵ���ʽ�˵�, ���Զ��ǲ˵����
    {
        if (nCode == IDC_ICONTROL_ADD_MENUBAR)
            return IsEToolMenu((HMENU)lParam, (int)wParam);
        return IsEToolMenu((HMENU)lParam, 0);
    }
    case IDC_ICONTROL_BUILD:
    {
        return EWindow_Build((int)wParam, (int)lParam);
    }
    case IDC_ICONTROL_UNLOADLIB:
    {

        break;
    }
    default:
        break;
    }
    return 0;
}

