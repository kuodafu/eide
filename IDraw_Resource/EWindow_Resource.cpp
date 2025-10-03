#include "EWindow_Resource.h"
#include <assist/assist_Resource.h>
#include <zlib/Czlib.h>
#include <ThemeResourceExt.h>
#include "resource.h"
#include <control/src/scroll.h>
#include <iDraw_const.h>
#include <set>
#include <assist/assist_Resource.h>

static HINSTANCE m_hInst;

struct _GET_ICON_ARG
{
    LPCWSTR pszID;
    RECT_F* prc;
    STATE state;
    LPCWSTR lpszExtName;
    LPCWSTR lpszNodeName;
    int resourceID;
};

typedef std::map<int, WINDOW_ICON_DATA> MAP_ICON;
struct RESOURCE_MAPDATA
{
    MAP_ICON Toolbar;
    MAP_ICON TreeFne;
    MAP_ICON TreeProgram;
    MAP_ICON TreeControl;
    MAP_ICON TreeExpand;
    MAP_ICON ComboboxLocation;
    MAP_ICON MenuItem;
    MAP_ICON msctls_updown32_lr;
    MAP_ICON msctls_updown32_tb;
    MAP_ICON TopState;
    MAP_ICON icon;
};




#define FIRST_HRESOURCE 10000
static IDRAW_HRESOURCE m_hResource = (IDRAW_HRESOURCE)FIRST_HRESOURCE;              // ��Դ���, �û����м���һ�ε���4
static std::map<IDRAW_HRESOURCE, MAP_ICON>          m_user_icon;                    // �û����м��ص�����
static std::map<HWND, std::vector<BINDWINDOW_INFO>> m_user_bind;                    // �û��󶨵�����
static std::map<int, MAP_ICON>                      m_icon;                         // ����������ص���Դ


struct RESOURCE_FREE_BITMAP
{
    // ���ͷ�dx/gdi+ͼƬ
    Gdiplus::Bitmap* gp;
    LPEX_IMAGE dx;
    RESOURCE_FREE_BITMAP() :gp(0), dx(0) { }
    RESOURCE_FREE_BITMAP(Gdiplus::Bitmap* gp, LPEX_IMAGE dx) :gp(gp), dx(dx) { }
};
static std::vector<RESOURCE_FREE_BITMAP> m_free;

static RESOURCE_MAPDATA m_map_data;
static std::vector<wchar_t*> s_hash_array;                  // �ı���ϣ��һ���ڴ�, �����ϣ�ͷ��������ַ, ��Ҫ�ͷ�


void Resource_SetInst(HINSTANCE hInst)
{
    m_hInst = hInst;
}

LPWINDOW_ICON_DATA Ext_GetIcon(_GET_ICON_ARG& arg, std::map<int, WINDOW_ICON_DATA>& defMap, std::map<int, WINDOW_ICON_DATA>& map, UINT dpi);

inline LONG_PTR _hash_fun(const wchar_t* pstr)
{
    if (!pstr || !pstr[0])return 0;
    for (auto it = s_hash_array.begin(); it != s_hash_array.end(); ++it)
    {
        const wchar_t* p = *it;
        if (p && wcscmp(p, pstr) == 0)
            return (LONG_PTR)p;
    }
    size_t len = wcslen(pstr) + 1;
    wchar_t* p = new wchar_t[len];
    memcpy(p, pstr, len * sizeof(wchar_t));
    s_hash_array.push_back(p);
    return (LONG_PTR)p;
}

inline LPCWSTR _string_copy(LPCWSTR pstr)
{
    return (LPCWSTR)_hash_fun(pstr);
}


inline static LPCWSTR _read_string(LPBYTE& ptr)
{
    LPCWSTR str = (LPCWSTR)ptr;
    size_t len = wcslen(str) + 1;
    len *= sizeof(wchar_t);
    ptr += len;
    return str;
}
inline static int _read_int(LPBYTE& ptr)
{
    int ret = *((int*)ptr);
    ptr += sizeof(int);
    return ret;
}
inline static void _read_rect(LPBYTE& ptr, RECT& rc)
{
    memcpy(&rc, ptr, sizeof(RECT));
    ptr += sizeof(RECT);
}


// ���������ݼ�������, ������������ݼ��ص� map ���������
// extFileName = �����ļ�����·��
// map = ���ս����map
inline static bool _load_theme(THEME_NAMESPACE LPTHEME_FILE_HEADER pTheme, std::map<int, WINDOW_ICON_DATA>& map)
{
    _str pngData, xmlData;
    THEME_NAMESPACE EXT_ERR err = THEME_NAMESPACE UnExtData(pTheme, &pngData, &xmlData);
    if (err != THEME_NAMESPACE EXT_ERR::EXT_ERR_OK)
    {
#ifdef _DEBUG
        __debugbreak();
#endif
        return false;
    }



    LPEX_IMAGE hBitmapDx = s_info->d2d->_img_create_frommemory(pngData.c_str(), pngData.size());
    Gdiplus::Bitmap* hBitmapGdip = _img_create_from_memory(pngData.c_str(), pngData.size());

    LPBYTE ptr = (LPBYTE)xmlData.c_str();
    const int nChildCount = (int)pTheme->dwCount;
    if (pTheme->cbSize == sizeof(THEME_NAMESPACE THEME_FILE_HEADER))    // 1.0 �汾
    {
        for (int i = 0; i < nChildCount; i++)
        {
            LPCWSTR id = _read_string(ptr);
            LONG_PTR hash = _hash_fun(id);
            WINDOW_ICON_DATA& data = map[hash];
            data.id = (LPCWSTR)hash;
            _read_rect(ptr, data.rc[0]);
            _read_rect(ptr, data.rc[1]);
            _read_rect(ptr, data.rc[2]);
            _read_rect(ptr, data.rc[3]);
            data.hBitmapDx = hBitmapDx;
            data.hBitmapGdip = hBitmapGdip;
        }
    }
    else if(pTheme->cbSize == sizeof(THEME_NAMESPACE THEME_FILE_HEADER_1_1))    // 1.1�汾
    {
        for (int i = 0; i < nChildCount; i++)
        {
            LPCWSTR id      = _read_string(ptr);
            LONG_PTR hash   = _hash_fun(id);
            WINDOW_ICON_DATA& data = map[hash];

            LPCWSTR text    = _read_string(ptr);
            LPCWSTR remarks = _read_string(ptr);
            data.id         = (LPCWSTR)hash;
            data.text       = _string_copy(text);
            data.remarks    = _string_copy(remarks);

            data.crText[0]  = _read_int(ptr);
            data.crText[1]  = _read_int(ptr);
            data.crText[2]  = _read_int(ptr);
            data.crText[3]  = _read_int(ptr);

            data.crBack[0]  = _read_int(ptr);
            data.crBack[1]  = _read_int(ptr);
            data.crBack[2]  = _read_int(ptr);
            data.crBack[3]  = _read_int(ptr);

            data.by[0]  = _read_int(ptr); // 5�����ó�Ա
            data.by[1]  = _read_int(ptr);
            data.by[2]  = _read_int(ptr);
            data.by[3]  = _read_int(ptr);
            data.by[4]  = _read_int(ptr);

            _read_rect(ptr, data.rc[0]);
            _read_rect(ptr, data.rc[1]);
            _read_rect(ptr, data.rc[2]);
            _read_rect(ptr, data.rc[3]);
            data.hBitmapDx = hBitmapDx;
            data.hBitmapGdip = hBitmapGdip;
        }
    }
    else if (pTheme->cbSize == sizeof(THEME_NAMESPACE THEME_FILE_HEADER_1_2))   // 1.2�汾, �м�¼5��DPI���ŵ�ͼ��λ��
    {
        for (int i = 0; i < nChildCount; i++)
        {
            LPCWSTR id      = _read_string(ptr);
            LONG_PTR hash   = _hash_fun(id);
            WINDOW_ICON_DATA& data = map[hash];

            LPCWSTR text    = _read_string(ptr);
            LPCWSTR remarks = _read_string(ptr);
            data.id         = (LPCWSTR)hash;
            data.text       = _string_copy(text);
            data.remarks    = _string_copy(remarks);

            data.crText[0]  = _read_int(ptr);
            data.crText[1]  = _read_int(ptr);
            data.crText[2]  = _read_int(ptr);
            data.crText[3]  = _read_int(ptr);

            data.crBack[0]  = _read_int(ptr);
            data.crBack[1]  = _read_int(ptr);
            data.crBack[2]  = _read_int(ptr);
            data.crBack[3]  = _read_int(ptr);

            data.by[0]  = _read_int(ptr); // 5�����ó�Ա
            data.by[1]  = _read_int(ptr);
            data.by[2]  = _read_int(ptr);
            data.by[3]  = _read_int(ptr);
            data.by[4]  = _read_int(ptr);

            //for (int i = 0; i < sizeof(data.rc) / sizeof(data.rc[0]); i++)    // �����¼����20������λ��
            for (int i = 0; i < 20; i++)    // �����¼����20������λ��
                _read_rect(ptr, data.rc[i]);

            data.hBitmapDx = hBitmapDx;
            data.hBitmapGdip = hBitmapGdip;
        }
    }

    m_free.push_back(RESOURCE_FREE_BITMAP(hBitmapGdip, hBitmapDx));
    return true;
}


// ���ļ���������, �������ļ�������ݼ��ص� map ���������
// extFileName = �����ļ�����·��
// map = ���ս����map
// pTheme = ���������ļ�ͷ�Ľṹָ��
inline static bool _load_theme(LPCWSTR extFileName, std::map<int, WINDOW_ICON_DATA>& map, THEME_NAMESPACE LPTHEME_FILE_HEADER pTheme = 0)
{
    CFileRW file;
    if (!file.open(extFileName, 1, -1)) return false;

    int size = file.size();
    if (size <= sizeof(THEME_NAMESPACE THEME_FILE_HEADER)) return false;

    LPBYTE data = new BYTE[size];
    file.read(data, size);
    bool ret = _load_theme((THEME_NAMESPACE LPTHEME_FILE_HEADER)data, map);
    if (pTheme)memcpy(pTheme, data, sizeof(THEME_NAMESPACE THEME_FILE_HEADER));
    delete[] data;
    return ret;
}


inline void _Ext_Init(_GET_ICON_ARG& arg, std::map<int, WINDOW_ICON_DATA>& defMap, std::map<int, WINDOW_ICON_DATA>& map)
{
    if (!map.empty()) return;   // map�Ѿ���ʼ��, ֱ�ӷ���

    bool isSave = false;
    bool isOk = false;
    wstr extFileName(260);
    DWORD nResourceSize = 0;    // Ĭ����Դ�ĳߴ�
    THEME_NAMESPACE LPTHEME_FILE_HEADER pResource = 0;  // Ĭ����Դ������, ��������û�������ʧ��, �Ͱ������Դд���ļ�

    //! ��һ���ȼ���Ĭ����Դ, ���ص�Ĭ�ϵ�map��, ��ʱ�����û���Դû�ҵ���ʱ��ʹ�Ĭ����Դ�����
    pResource = (THEME_NAMESPACE LPTHEME_FILE_HEADER)GetResource(m_hInst, arg.resourceID, &nResourceSize, L"IRES");
    // ����Ĭ������, �Ѽ��ص����ݼ�¼�� defMap ���map��
    isOk = _load_theme(pResource, defMap);
    if (!isOk)
    {
        MessageBoxW(s_info->hEWnd, L"Ĭ����Դ����ʧ��, �޷�����", L"����", 0);
        throw;
    }

#if 1   // ��Ϊ0�������û�����, ֻ����Ĭ������
    //! �ڶ���, �������ж�ȡ����, Ҫ��ȡ���Ǽ����ĸ�ires�ļ�, �������û��д, �ǾͰ��ļ���д��������
    wstr themePath = s_info->iniColor->read(L"iresFile", arg.lpszNodeName, arg.lpszExtName);
    extFileName.assign(s_info->path_data).append(themePath);  // ƴװ��������ires�ļ�·��


    //! ������, ����ires���ݵ�map��, map���û�����, ����ʧ�����Ĭ������д������
    THEME_NAMESPACE THEME_FILE_HEADER theme = { 0 };
    isOk = _load_theme(extFileName.c_str(), map, &theme);
    if (!isOk)    // �������ʧ��, ��Ҫ����Դ��д��Ĭ�ϵ�����
    {
        memcpy(&theme, pResource, sizeof(THEME_NAMESPACE THEME_FILE_HEADER));
        FILE* f;
        errno_t err = _wfopen_s(&f, extFileName.c_str(), L"wb+");
        if (err || !f)
        {
            wstr msgText;
            msgText.Format(L"д��Ƥ���ļ�ʧ��, ���ֹ�������\n·��Ϊ = %s\n������ = %d\n\n"
                "���Գ������½������:\n"
                "    1. ������������Ŀ¼\\lib\\iDraw\\data\\ ����ļ���\n"
                "    2. ��������֧�ֿⰲװ\n"
                , extFileName.c_str(), err);
            MessageBoxW(s_info->hEWnd, msgText.c_str(), L"����", 0);
        }
        else
        {
            fwrite(pResource, 1, nResourceSize, f);
            fclose(f);
        }
    }
#else
    THEME_NAMESPACE THEME_FILE_HEADER theme;
    memcpy(&theme, pResource, sizeof(THEME_NAMESPACE THEME_FILE_HEADER));

#endif

}


// ��ȡDPI��Ӧ�ľ���, ���ͼ��ߴ�, dpi���ż�¼�� data����ṹ��, ��¼��dpi��100, 125, 150, 175, 200
// dpi = dpi, 96, 192...
// index = 0=����, 1=��ȼ, 2=����, 3=��ֹ
inline int _getDPIRect(UINT dpi, int index, RECT_F* prc, WINDOW_ICON_DATA* data)
{
    enum _DPI
    {
        DPI_100 = 96,
        DPI_125 = DPI_100 * 115 / 100,
        DPI_150 = DPI_100 * 150 / 100,
        DPI_175 = DPI_100 * 175 / 100,
        DPI_200 = DPI_100 * 200 / 100,
    };

    int dpiIndex = 0;
    switch (dpi)
    {
    case DPI_100:   dpiIndex = 0; data->nDpi = 100; break;
    case DPI_125:   dpiIndex = 1; data->nDpi = 125; break;
    case DPI_150:   dpiIndex = 2; data->nDpi = 150; break;
    case DPI_175:   dpiIndex = 3; data->nDpi = 175; break;
    case DPI_200:   dpiIndex = 4; data->nDpi = 200; break;
    default:        dpiIndex = 0; data->nDpi = 100; break;
    }
    const int rcIndex = dpiIndex * 4 + index;   // ÿ��DPI��¼�ľ��ζ���4��, ��ȡ��Ӧ��DPI�ͳ���4
    *prc = data->rc[rcIndex];
    data->cxIcon = prc->width();
    data->cyIcon = prc->height();
    if (data->cxIcon <= 0 && data->cyIcon <= 0)    // ������DPI�ľ���������, �Ǿ�ȡ100%DPI�ľ���
    {
        *prc = data->rc[index];
        data->cxIcon = prc->width();
        data->cyIcon = prc->height();
        data->nDpi = 100;
    }
    return rcIndex;
}

LPWINDOW_ICON_DATA Ext_GetIcon(_GET_ICON_ARG& arg, std::map<int, WINDOW_ICON_DATA>& defMap, std::map<int, WINDOW_ICON_DATA>& map, UINT dpi)
{
    bool isOk = false;
    _Ext_Init(arg, defMap, map);

    LONG_PTR hash = _hash_fun(arg.pszID);
    int index = 0;
    switch (arg.state)
    {
    case STATE::HOT:    index = 1; break;
    case STATE::DOWN:   index = 2; break;
    case STATE::BAN:    index = 3; break;
    default:            index = 0; break;
    }

    static LONG_PTR defHash = _hash_fun(L"default");
    WINDOW_ICON_DATA* data = 0;
    while (1)
    {
        //! ���Ҳ���
        //! 1. �ȴ��û�����������, �ѵ��ͷ���
        //! 2. û�ѵ��ʹ��û�����������Ĭ��ͼ��
        //! 3. ��Ĭ������������
        //! 4. ��Ĭ������������Ĭ��ͼ��
        //! 5. û���ҵ�, ����0


        //! 1. �ȴ��û�����������, �ѵ��ͷ���
        {
            auto it = map.find(hash);
            if (it != map.end())
            {
                data = &(it->second);
                data->nIconType = 0;
                break;
            }
        }

        //! 2. û�ѵ��ʹ��û�����������Ĭ��ͼ��
        {
            auto it = map.find(defHash);
            if (it != map.end()) // �û����ص�Ƥ����Ĭ��ֵ, �����û���Ĭ��ֵ
            {
                data = &(it->second);
                data->nIconType = 1;
                break;
            }
        }

        //! 3. ��Ĭ������������
        {
            auto it = defMap.find(hash);
            if (it != defMap.end())            // ���û��������ҵ���ͼ��, ֱ�ӷ���
            {
                data = &(it->second);
                data->nIconType = 2;
                break;
            }
        }

        //! 4. ��Ĭ������������Ĭ��ͼ��
        {
            auto it = defMap.find(defHash);
            if (it != defMap.end())     // �û����ص�Ƥ����Ĭ��ֵ, �����û���Ĭ��ֵ
            {
                data = &(it->second);
                data->nIconType = 3;
                break;
            }
        }
        break;
    }

    if (!data) return 0;    // û�ҵ��ͷ���0
    _getDPIRect(dpi, index, arg.prc, data);
    if (arg.prc->width() == 0 || arg.prc->height() == 0)
        return 0;
    return data;
}







LPBYTE EWindowResource::GetELogo(DWORD* size)
{
    return GetResource(m_hInst, IDPNG_ELOGO, size, L"PNG");
}

LPBYTE EWindowResource::GetDefThemeData(DWORD* size)
{
    return GetResource(m_hInst, IDTHEME_DATA, size, L"IRES");
}

void EWindowResource::SetModule(HINSTANCE hInst)
{
    m_hInst = hInst;
}

LPSYSBUTTONIMAGE EWindowResource::GetNskn()
{
    DWORD size = 0;
    return (LPSYSBUTTONIMAGE)GetResource(m_hInst, IDR_NSKN1, &size, L"NSKN");
}

inline MAP_ICON* _getmapfrom_ini(LPCWSTR themeName)
{
    LONG_PTR hash = _hash_fun(themeName);
    MAP_ICON& map = m_icon[hash];
    return &map;
}

#define _make_argument(_arg, _pszID, _prc, _state, _lpszExtName, _lpszNodeName, _resourceID) \
                _arg = { _pszID, _prc, _state, _lpszExtName, _lpszNodeName, _resourceID }

//inline _GET_ICON_ARG& _make_argument(_GET_ICON_ARG& arg, LPCWSTR pszID, RECT_F* prc, STATE state, bool isDx, LPCWSTR lpszExtName, LPCWSTR lpszNodeName, int resourceID)
//{
//    arg.lpszExtName = lpszExtName;
//    arg.lpszNodeName = lpszNodeName;
//    arg.prc = prc;
//    arg.pszID = pszID;
//    arg.resourceID = resourceID;
//    arg.state = state;
//    return arg;
//}

#define _MAKE_ARG(_name, _resid) \
    LPCWSTR lpszExtName = s_info->themeColor->ires_##_name();\
    _GET_ICON_ARG arg = { pszID, prc, state, lpszExtName, L###_name, _resid };\
    return Ext_GetIcon(arg, m_map_data._name, *(_getmapfrom_ini(lpszExtName)), dpi)



LPWINDOW_ICON_DATA EWindowResource::Tool_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(Toolbar, IDEXT_TOOLICON);
    //_GET_ICON_ARG arg = { 0 };
    //LPCWSTR lpszNodeName = s_info->themeColor->ires_Toolbar();
    //_make_argument(arg, pszID, prc, state, cxIcon, cyIcon, false, pIconType, L"Toolbar.ires", L"Toolbar", IDEXT_TOOLICON);
    //return Ext_GetIcon(arg, m_map_data.Toolbar, *(_getmapfrom_ini(lpszNodeName)));
}

LPWINDOW_ICON_DATA EWindowResource::TreeFne_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(TreeFne, IDEXT_TREEFNE);
}

LPWINDOW_ICON_DATA EWindowResource::TreeProgram_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(TreeProgram, IDEXT_TREEPROGRAM);
}


LPWINDOW_ICON_DATA EWindowResource::TreeControl_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(TreeControl, IDEXT_TREECONTROL);
}

LPWINDOW_ICON_DATA EWindowResource::TreeExpand_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(TreeExpand, IDEXT_EXPAND);
}

LPWINDOW_ICON_DATA EWindowResource::Combobox_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(ComboboxLocation, IDEXT_COMBOBOXLOCATION);
}

LPWINDOW_ICON_DATA EWindowResource::Menu_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(MenuItem, IDEXT_MENUITEM);
}

LPWINDOW_ICON_DATA EWindowResource::MsctlsUpdowm32_GetIcon_lr(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(msctls_updown32_lr, IDEXT_MSCTLS_UPDOWN32_LR);
}

LPWINDOW_ICON_DATA EWindowResource::MsctlsUpdowm32_GetIcon_tb(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(msctls_updown32_tb, IDEXT_MSCTLS_UPDOWN32_TB);
}

LPWINDOW_ICON_DATA EWindowResource::TopState_GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _MAKE_ARG(TopState, IDEXT_TOPSTATE);
}

LPWINDOW_ICON_DATA EWindowResource::GetIcon(LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _GET_ICON_ARG arg = { 0 };
    _make_argument(arg, pszID, prc, state, L"icon.ires", L"icon", IDR_ICON);
    return Ext_GetIcon(arg, m_map_data.icon, m_map_data.icon, dpi);
}

// �ͷ���Դ
void EWindowResource::EWindow_Resource_Free()
{
    for (auto i = s_hash_array.begin(); i != s_hash_array.end(); ++i)
        delete[](*i);
    s_hash_array.clear();


    m_map_data.Toolbar.clear();
    m_map_data.TreeFne.clear();
    m_map_data.TreeProgram.clear();
    m_map_data.TreeControl.clear();
    m_map_data.TreeExpand.clear();
    m_map_data.ComboboxLocation.clear();
    m_map_data.MenuItem.clear();
    m_map_data.msctls_updown32_lr.clear();
    m_map_data.msctls_updown32_tb.clear();

    for (auto i = m_free.begin(); i != m_free.end(); ++i)
    {
        RESOURCE_FREE_BITMAP& data = *i;
        s_info->d2d->_img_destroy(data.dx);
        delete data.gp;
    }
    m_free.clear();
}

IDRAW_HRESOURCE EWindowResource::LoadFromFile(LPCWSTR szFile)
{
    if (!szFile || !szFile[0]) return 0;
    FILE* f;
    void* p;
    errno_t err = _wfopen_s(&f, szFile, L"rb+");
    if (err || !f) return 0;


    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if ((p = malloc(size)))
        size = (long)fread_s(p, size, 1, size, f);

    fclose(f);
    IDRAW_HRESOURCE hResource = LoadFromMemory(p, size);
    free(p);
    return hResource;
}

IDRAW_HRESOURCE EWindowResource::LoadFromMemory(const void* pData, int size)
{
    if (!pData || !size)
        return 0;

    std::map<int, WINDOW_ICON_DATA>& map = m_user_icon[m_hResource];
    if (!_load_theme((THEME_NAMESPACE LPTHEME_FILE_HEADER)pData, map))
        return 0;

    IDRAW_HRESOURCE ret = m_hResource;
    m_hResource = (IDRAW_HRESOURCE)(((DWORD)m_hResource) + sizeof(IDRAW_HRESOURCE));

    return ret;
}

inline LPWINDOW_ICON_DATA _res_get_icon_from_map(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    _GET_ICON_ARG arg = { 0 };
    WINDOW_ICON_DATA* data = 0;
    std::map<int, WINDOW_ICON_DATA>* pMap1 = 0, *pMap2 = 0;
    LPCWSTR name = 0;
    if ((void*)hResource == &m_map_data.MenuItem)           // �˵�
    {
        name = s_info->themeColor->ires_MenuItem();
        _make_argument(arg, pszID, prc, state, name, L"MenuItem", IDEXT_MENUITEM);
        pMap1 = &m_map_data.MenuItem;
    }
    else if ((void*)hResource == &m_map_data.TreeExpand)    // ���Ϳ���չͼ��
    {
        name = s_info->themeColor->ires_TreeExpand();
        _make_argument(arg, pszID, prc, state, name, L"TreeExpand", IDEXT_EXPAND);
        pMap1 = &m_map_data.TreeExpand;
    }
    else if ((void*)hResource == &m_map_data.Toolbar)       // ��׼������
    {
        name = s_info->themeColor->ires_Toolbar();
        _make_argument(arg, pszID, prc, state, name, L"Toolbar", IDEXT_TOOLICON);
        pMap1 = &m_map_data.Toolbar;
    }
    else if ((void*)hResource == &m_map_data.TreeFne)       // ֧�ֿ����Ϳ�
    {
        name = s_info->themeColor->ires_TreeFne();
        _make_argument(arg, pszID, prc, state, name, L"TreeFne", IDEXT_TREEFNE);
        pMap1 = &m_map_data.TreeFne;
    }
    else if ((void*)hResource == &m_map_data.TreeProgram)   // �������Ϳ�
    {
        name = s_info->themeColor->ires_TreeProgram();
        _make_argument(arg, pszID, prc, state, name, L"TreeProgram", IDEXT_TREEPROGRAM);
        pMap1 = &m_map_data.TreeProgram;

    }
    else if ((void*)hResource == &m_map_data.TreeControl)   // ��������Ϳ�
    {
        name = s_info->themeColor->ires_TreeControl();
        _make_argument(arg, pszID, prc, state, name, L"TreeControl", IDEXT_TREECONTROL);
        pMap1 = &m_map_data.TreeControl;
    }
    else if ((void*)hResource == &m_map_data.ComboboxLocation)  // ������Ͽ�
    {
        name = s_info->themeColor->ires_ComboboxLocation();
        _make_argument(arg, pszID, prc, state, name, L"ComboboxLocation", IDEXT_COMBOBOXLOCATION);
        pMap1 = &m_map_data.ComboboxLocation;
    }
    if (name)
        pMap2 = _getmapfrom_ini(name);
    if (pMap1 && pMap2)
    {
        return Ext_GetIcon(arg, *pMap1, *pMap2, dpi);
    }
    return 0;
}
inline LPWINDOW_ICON_DATA _res_get_usericon(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    IDRAW_HRESOURCE ret = 0;

    if ((int)hResource == (FIRST_HRESOURCE - 4))
    {
        hResource = (IDRAW_HRESOURCE)&m_map_data.MenuItem;
    }
    else if ((int)hResource == (FIRST_HRESOURCE - 8))
    {
        hResource = (IDRAW_HRESOURCE)&m_map_data.TreeExpand;
    }
    if ((int)hResource >= 0x8000)
    {
        return _res_get_icon_from_map(hResource, pszID, prc, state, dpi);
    }

    // �ߵ�������û����ص�����
    auto it = m_user_icon.find(hResource);
    if (it == m_user_icon.end()) return 0;

    std::map<int, WINDOW_ICON_DATA>& map = it->second;
    LONG_PTR hash = _hash_fun(pszID);
    static LONG_PTR defHash = _hash_fun(L"default");
    WINDOW_ICON_DATA* data = 0;
    int index = 0;
    switch (state)
    {
    case STATE::HOT:    index = 1; break;
    case STATE::DOWN:   index = 2; break;
    case STATE::BAN:    index = 3; break;
    default:            index = 0; break;
    }

    while (1)
    {
        //! ���Ҳ���
        //! 1. �ȴ��û�����������, �ѵ��ͷ���
        {
            auto it = map.find(hash);
            if (it != map.end())
            {
                data = &(it->second);
                data->nIconType = 0;
                break;
            }
        }

        //! 2. û�ѵ��ʹ��û�����������Ĭ��ͼ��
        {
            auto it = map.find(defHash);
            if (it != map.end()) // �û����ص�Ƥ����Ĭ��ֵ, �����û���Ĭ��ֵ
            {
                data = &(it->second);
                data->nIconType = 1;
                break;
            }
        }
        break;
    }

    if (!data) return 0;    // û�ҵ��ͷ���0
    _getDPIRect(dpi, index, prc, data);
    if (prc->width() == 0 || prc->height() == 0)
        return 0;
    return data;
}
LPWINDOW_ICON_DATA EWindowResource::User_GetIcon(IDRAW_HRESOURCE hResource, LPCWSTR pszID, RECT_F* prc, STATE state, UINT dpi)
{
    LPWINDOW_ICON_DATA ret = _res_get_usericon(hResource, pszID, prc, state, dpi);
    return ret;
}


IDRAW_HRESOURCE EWindowResource::GetDefControlIcon(HWND hWnd)
{
    IDRAW_HRESOURCE ret = 0;
    if (!IsWindow(hWnd))
    {
        if (hWnd == HRESOURCE_MENU)          // �˵�
        {
            ret = (IDRAW_HRESOURCE)&m_map_data.MenuItem;
        }
        else if (hWnd == HRESOURCE_TREEEXPAND)     // ���Ϳ���չͼ��
        {
            ret = (IDRAW_HRESOURCE)&m_map_data.TreeExpand;
        }
        return ret;
    }


    if (hWnd == s_info->eWnd->GetMenuStandard())            // ��׼������
    {
        ret = (IDRAW_HRESOURCE)&m_map_data.Toolbar;
    }
    else if (hWnd == s_info->eWnd->GetWorkFneTree())        // ֧�ֿ����Ϳ�
    {
        ret = (IDRAW_HRESOURCE)&m_map_data.TreeFne;
    }
    else if (hWnd == s_info->eWnd->GetWorkProgramTree())    // �������Ϳ�
    {
        ret = (IDRAW_HRESOURCE)&m_map_data.TreeProgram;
    }
    else if (hWnd == s_info->eWnd->GetControlTree())        // ��������Ϳ�
    {
        ret = (IDRAW_HRESOURCE)&m_map_data.TreeControl;
    }
    else if (hWnd == s_info->eWnd->GetMenuComboBox1() || hWnd == s_info->eWnd->GetMenuComboBox2())  // ������Ͽ�
    {
        ret = (IDRAW_HRESOURCE)&m_map_data.ComboboxLocation;
    }
    return ret;
}
bool EWindowResource::UnBindWindow(LPCSTR guid)
{
    LPPLUGIN_INFO plugin = s_info->pfnPlugin_GetInfo(guid);
    if (!plugin) return false;

    // �����а󶨵Ĵ������ҵ���ǰ���, Ȼ���������ɾ��
    for (auto it = m_user_bind.begin(); it != m_user_bind.end(); ++it)
    {
        std::vector<BINDWINDOW_INFO>& arr = it->second;
        for (auto itArr = arr.begin(); itArr != arr.end(); ++itArr)
        {
            if (itArr->guid == (LPCSTR)plugin->hash)
            {
                itArr = arr.erase(itArr);
                if (itArr == arr.end()) break;
            }
        }
    }
    return true;
}
bool EWindowResource::BindWindow(LPCSTR guid, HWND hWnd, IDRAW_HRESOURCE hResource, pfn_GetWindowIcon callback)
{
    LPPLUGIN_INFO plugin = s_info->pfnPlugin_GetInfo(guid);
    if (!plugin) return false;

    if (!IsWindow(hWnd))
    {
        if (hWnd != HRESOURCE_MENU && hWnd != HRESOURCE_TREEEXPAND)
            return false;
    }
    std::vector<BINDWINDOW_INFO>& arr = m_user_bind[hWnd];
    for (auto it = arr.begin(); it != arr.end(); ++it)
    {
        if (it->guid == (LPCSTR)plugin->hash)   // ��ε��ô��ݵĻص���������ͬһ��, ��������
        {
            it->hResource = hResource;
            it->callback = callback;
            return true;
        }
    }
    BINDWINDOW_INFO data = { hResource, callback, (LPCSTR)plugin->hash };
    arr.push_back(data);
    return true;
}

// �Ӵ��ھ����ȡ�ص�����, ���Ե��ûص���������ȡͼ��, ����д�뻺����������, bufCallback Ϊ0�򷵻���Ҫ�Ļ�������Ա��
// hWnd = ���ĸ������ϻ�ȡ�ص���Ϣ
// bufCallback = ���հ�ʱ���ݵĻص���������, ���Ϊ0�򷵻ذ󶨵�����
// bufCount = ��ʾ bufCallback ����������ж��ٸ���Ա
int EWindowResource::GetResourceCallbackFromHwnd(HWND hWnd, BINDWINDOW_INFO* bufCallback, int bufCount)
{
    auto it = m_user_bind.find(hWnd);
    if (it == m_user_bind.end())
        return 0;

    std::vector<BINDWINDOW_INFO>& arr = it->second;
    if (!bufCallback)
        return (int)arr.size();

    int i = 0;
    for (auto itArr = arr.begin(); itArr != arr.end(); ++itArr)
    {
        BINDWINDOW_INFO& info = *itArr;
        if (i + 1 > bufCount)
            break;
        bufCallback[i].callback = info.callback;
        bufCallback[i].hResource = info.hResource;
        bufCallback[i].guid = info.guid;
        i++;
    }
    return i;
}

LPWINDOW_ICON_DATA EWindowResource::GetIconFromHwnd(HWND hWnd, LPCWSTR pszID, void* arg, RECT_F* prc, STATE state, UINT dpi)
{
    auto it = m_user_bind.find(hWnd);
    if (it == m_user_bind.end())
        return 0;

    std::vector<BINDWINDOW_INFO>& arr = it->second;
    LPWINDOW_ICON_DATA ret = 0;
    for (auto itArr = arr.begin(); itArr != arr.end(); ++itArr)
    {
        BINDWINDOW_INFO& item = *itArr;
        if (item.callback)
        {
            ret = item.callback(hWnd, item.hResource, pszID, arg, prc, state, dpi);
            if (ret)
            {
                ret->cxIcon = prc->width();
                ret->cyIcon = prc->height();
            }
        }
        if (!ret)
        {
            ret = User_GetIcon(item.hResource, pszID, prc, state, dpi);
        }
        if (ret)
            break;
    }
    return ret;
}

// ��gdi+λͼ����dxλͼ��������Դ����ֵ, ����ָ����뱣֤���, �ڲ����������ڴ������洫�ݽ������ı���λͼ
// hImage = gdi+������λͼ����dx������λͼ
// isDx = ��ʾhImage��gdi+�����Ļ���dx������
// text = ���Ƶ�ǰ��Ŀ��ʾ���ı�, �����޸�ԭ����ı�, ֻ�ǻ滭ʱ��ʾ����ı�, ����Ϊ0
// remarks = ��ע�ı�, ����Ϊ0
// crText = �ı�ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
// crBack = ����ARGB��ɫ, ����Ϊ0, ��Ϊ0�������4����Ա, �ֱ��� ����, �ȵ�, ����, ��ֹ
LPWINDOW_ICON_DATA EWindowResource::MakeResourceReturn(const void* pImg, int pImgSize, LPCWSTR text, LPCWSTR remarks, DWORD* crText, DWORD* crBack)
{
    if (pImgSize <= 0 || !pImg)
        return 0;
    LPWINDOW_ICON_DATA ret = new WINDOW_ICON_DATA;
    memset(ret, 0, sizeof(WINDOW_ICON_DATA));

    ret->hBitmapDx = s_info->d2d->_img_create_frommemory(pImg, pImgSize);
    ret->hBitmapGdip = _img_create_from_memory(pImg, pImgSize);
    if (crText)
        memcpy(ret->crText, crText, sizeof(ret->crText));
    if (crBack)
        memcpy(ret->crBack, crBack, sizeof(ret->crBack));
    ret->text = text;
    ret->remarks = remarks;
    return ret;
}

bool EWindowResource::FreeResourceReturn(LPWINDOW_ICON_DATA pData)
{
    if (!pData) return false;
    s_info->d2d->_img_destroy(pData->hBitmapDx);
    delete pData->hBitmapGdip;
    delete pData;
    return true;
}

Gdiplus::Bitmap* EWindowResource::LoadGpImageFromMemory(const void* pData, int size)
{
    return _img_create_from_memory(pData, size);
}

bool EWindowResource::FreeGpImage(Gdiplus::Bitmap* hImg)
{
    bool ret = hImg != 0;
    if (hImg)
        delete hImg;
    return ret;
}




class _LOAD_FONT
{
public:
    HANDLE handle;
    _LOAD_FONT(LPBYTE pData, int size)
    {
        DWORD nFontIndex = 0;
        handle = AddFontMemResourceEx(pData, size, NULL, &nFontIndex);
    }
    ~_LOAD_FONT()
    {
        RemoveFontMemResourceEx(handle);
    }
    static HFONT _create_font(LPCWSTR name, int size)
    {
        LOGFONTW lf = { 0 };
        lf.lfHeight = size;
        lf.lfWidth = size;
        //lf.lfEscapement          = 0	;
        //lf.lfOrientation         = 0	;
        //lf.lfWeight              = 400;
        //lf.lfItalic              = 0	;
        //lf.lfUnderline           = 0	;
        //lf.lfStrikeOut           = 0	;
        //lf.lfCharSet             = 0	;
        //lf.lfOutPrecision        = 3	;
        //lf.lfClipPrecision       = 2	;
        //lf.lfQuality             = 1	;
        //lf.lfPitchAndFamily      = 49	;
        wcscpy_s(lf.lfFaceName, name);
        return CreateFontIndirectW(&lf);
    }
    static HFONT get(LPCWSTR name, int size)
    {
        static std::unordered_map<int, HFONT> map;
        HFONT& hFont = map[size];
        if (!hFont)
            hFont = _create_font(name, size);
        return hFont;
    }

};



HFONT EWindowResource::GetMarlettFont(int size)
{
    int nSize = 0;
    LPBYTE pData = GetMarlettData(&nSize);
    static _LOAD_FONT _font(pData, nSize);
    return _LOAD_FONT::get(L"Marlett", size);
}

LPBYTE EWindowResource::GetMarlettData(int* size)
{
    return GetResource(m_hInst, IDR_FONT_MARLETT, (DWORD*)size, L"IRES");
}
